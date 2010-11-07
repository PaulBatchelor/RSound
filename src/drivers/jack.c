/*  RSound - A PCM audio client/server
 *  Copyright (C) 2010 - Hans-Kristian Arntzen
 * 
 *  RSound is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RSound is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RSound.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "jack.h"
#include "../rsound.h"

static void jack_close(void *data)
{
   jack_t *jd = data;

   for (int i = 0; i < jd->channels; i++)
      if (jd->buffer[i] != NULL)
         jack_ringbuffer_free(jd->buffer[i]);

   if (jd->client != NULL)
   {
      jack_deactivate(jd->client);
      jack_client_close(jd->client);
   }
   free(jd);
}

/* Opens and sets params */
static int jack_init(void **data)
{
   jack_t *sound = calloc(1, sizeof(jack_t));
   if ( sound == NULL )
      return -1;
   *data = sound;
   return 0;
}

static int process_cb(jack_nframes_t nframes, void *data) 
{
   jack_t *jd = data;
   if (nframes <= 0)
      return 0;

   jack_default_audio_sample_t *out;
   jack_nframes_t available = jack_ringbuffer_read_space(jd->buffer[0]);
   available /= sizeof(jack_default_audio_sample_t);

   if (available > nframes)
      available = nframes;

   for (int i = 0; i < jd->channels; i++)
   {
      out = jack_port_get_buffer(jd->ports[i], nframes);
      jack_ringbuffer_read(jd->buffer[i], (char*)out, available * sizeof(jack_default_audio_sample_t));

      for (jack_nframes_t f = available; f < nframes; f++)
         out[f] = 0.0f;
   }
   return 0;
}

static void shutdown_cb(void *data)
{
   jack_t *jd = data;
   jd->shutdown = 1;
}

static inline int audio_conv_op(enum rsd_format format)
{
   return converter_fmt_to_s16ne(format) | RSD_S16_TO_FLOAT;
}

static int jack_open(void *data, wav_header_t *w)
{
   const char **jports = NULL;
   const char *dest_ports[MAX_PORTS];
   jack_t *jd = data;
   jd->channels = w->numChannels;
   jd->format = w->rsd_format;
   jd->conv_op = audio_conv_op(jd->format);

   jd->client = jack_client_open(JACK_CLIENT_NAME, JackNullOption, NULL);
   if (jd->client == NULL)
      return -1;

   jack_set_process_callback(jd->client, process_cb, jd);
   jack_on_shutdown(jd->client, shutdown_cb, jd);

   for (int i = 0; i < jd->channels; i++)
   {
      char buf[1024];
      if (i == 0)
         strcpy(buf, "left");
      else if (i == 1)
         strcpy(buf, "right");
      else
         sprintf(buf, "%s%d", "channel", i);

      jd->ports[i] = jack_port_register(jd->client, buf, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
      if (jd->ports[i] == NULL)
      {
         fprintf(stderr, "Couldn't create jack ports\n");
         goto error;
      }
   }

   for (int i = 0; i < jd->channels; i++)
   {
      jd->buffer[i] = jack_ringbuffer_create(JACK_BUFFER_SIZE);
      if (jd->buffer[i] == NULL)
      {
         fprintf(stderr, "Couldn't create ringbuffer\n");
         return -1;
      }
   }

   if (jack_activate(jd->client) < 0)
   {
      fprintf(stderr, "Couldn't connect to JACK server\n");
      goto error;
   }

   jports = jack_get_ports(jd->client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);
   if (jports == NULL)
   {
      fprintf(stderr, "Can't get ports ...\n");
      goto error;
   }

   for (int i = 0; i < MAX_PORTS && jports[i] != NULL; i++)
   {
      dest_ports[i] = jports[i];
   }

   for (int i = 0; i < jd->channels; i++)
   {
      int rd = jack_connect(jd->client, jack_port_name(jd->ports[i]), dest_ports[i]);
      if (rd != 0)
      {
         fprintf(stderr, "Can't connect ports...\n");
         goto error;
      }
   }


   return 0;
error:
   if (jports != NULL)
      free(jports);
   return -1;
}

static void jack_get_backend(void *data, backend_info_t *backend_info)
{
   (void)data;
   backend_info->latency = DEFAULT_CHUNK_SIZE;
   backend_info->chunk_size = DEFAULT_CHUNK_SIZE;
}

static int jack_latency(void* data)
{
   (void)data;
   int delay = 0;

   return delay;
}

static size_t write_buffer(jack_t *jd, const void* buf, size_t size)
{
   // Convert our data to float, deinterleave and write.
   float out_buffer[BYTES_TO_SAMPLES(size, jd->format)];
   float out_deinterleaved_buffer[jd->channels][BYTES_TO_SAMPLES(size, jd->format)/jd->channels];
   memcpy(out_buffer, buf, size);
   audio_converter(out_buffer, jd->format, jd->conv_op, size);

   for (int i = 0; i < jd->channels; i++)
      for (size_t j = 0; j < BYTES_TO_SAMPLES(size, jd->format)/jd->channels; j++)
         out_deinterleaved_buffer[i][j] = out_buffer[j * jd->channels + i];

   // Stupid busy wait for available write space.
   for(;;)
   {
      if (jd->shutdown)
         return 0;

      size_t avail = jack_ringbuffer_write_space(jd->buffer[0]);

      if (avail > sizeof(jack_default_audio_sample_t) * BYTES_TO_SAMPLES(size, jd->format))
         break;

      // TODO: Need to do something more intelligent here!
      usleep(1000);
   }

   for (int i = 0; i < jd->channels; i++)
      jack_ringbuffer_write(jd->buffer[i], (const char*)out_deinterleaved_buffer[i], BYTES_TO_SAMPLES(size, jd->format) * sizeof(jack_default_audio_sample_t) / jd->channels);
   return size;
}

static size_t jack_write (void *data, const void* buf, size_t size)
{
   jack_t *jd = data;
   if (jd->shutdown)
      return 0;

   return write_buffer(jd, buf, size);
}

const rsd_backend_callback_t rsd_jack = {
   .init = jack_init,
   .open = jack_open,
   .write = jack_write,
   .latency = jack_latency,
   .get_backend_info = jack_get_backend,
   .close = jack_close,
   .backend = "JACK"
};


