/*  RSound - A PCM audio client/server
 *  Copyright (C) 2009 - Hans-Kristian Arntzen
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

#include "endian.h"
#include "rsound.h"

#ifdef _ALSA
#include "alsa.h"
#endif

#ifdef _OSS
#include "oss.h"
#endif

#ifdef _AO
#include "ao.h"
#endif

#ifdef _PORTA
#include "porta.h"
#endif

#define _GNU_SOURCE
#include <getopt.h>

/* This file defines some backend independed operations */

void new_sound_thread ( int socket )
{

   pthread_t thread;
   
   int *s = malloc ( sizeof(int));
   *s = socket;


   pthread_create(&thread, NULL, backend, (void*)s);     

   if ( no_threading )
      pthread_join(thread, NULL);
}

void parse_input(int argc, char **argv)
{
   int c, option_index = 0;

   struct option opts[] = {
      { "port", 1, NULL, 'p' },
      { "help", 0, NULL, 'h' },
      { "backend", 1, NULL, 'b' },
      { "device", 1, NULL, 'd' },
      { "no-daemon", 0, NULL, 'n' },
      { "verbose", 0, NULL, 'v' },
      { "no-threading", 0, NULL, 'T' },
      { NULL, 0, NULL, 0 }
   };

   char optstring[] = "d:b:p:nvh";

   while ( 1 )
   {
      c = getopt_long ( argc, argv, optstring, opts, &option_index );

      if ( c == -1 )
         break;

      switch ( c )
      {
         case 'd':
            strncpy(device, optarg, 127);
            device[127] = 0;
            break;

         case 'p':
            strncpy(port, optarg, 127);
            port[127] = 0;
            break;
         
         case '?':
            print_help(argv[0]);
            exit(1);

         case 'h':
            print_help(argv[0]);
            exit(0);

         case 'T':
            no_threading = 1;
            break;

         case 'b':
#ifdef _ALSA
            if ( !strcmp( "alsa", optarg ) )
            {
               backend = alsa_thread;
               break;
            }  
#endif
#ifdef _OSS
            if ( !strcmp( "oss", optarg ) )
            {
               backend = oss_thread;
               break;
            }
#endif
#ifdef _AO
            if ( !strcmp( "libao", optarg ) )
            {
               backend = ao_thread;
               break;
            }
#endif
#ifdef _PORTA
            if ( !strcmp( "portaudio", optarg ) )
            {
               backend = porta_thread;
               break;
            }
#endif
            fprintf(stderr, "\nValid backend not given. Exiting ...\n\n");
            print_help(argv[0]);
            exit(1);

         case 'n':
            daemonize = 0;
            break;

         case 'v':
            verbose = 1;
            break;

         default:
            fprintf(stderr, "Error parsing arguments.\n");
            exit(1);
      }
   }
   
   if ( backend == NULL )
   {

#ifdef __CYGWIN__
   /* We prefer libao if we're in Windows. */
   #ifdef _AO
      backend = ao_thread;
   #elif _PORTA
      backend = porta_thread;
   #elif _OSS
      backend = oss_thread;
   #endif
#else
   #ifdef _ALSA
      backend = alsa_thread;
   #elif _OSS
      backend = oss_thread;
   #elif _AO
      backend = ao_thread;
   #elif _PORTA
      backend = porta_thread;
   #endif
#endif

   }

   if ( backend == NULL )
   {
      fprintf(stderr, "%s was not compiled with any output support, exiting ...", argv[0]);
   }

}

void print_help(char *appname)
{
   putchar('\n');
   printf("Usage: %s [ -d/--device | -b/--backend | -p/--port | -n/--no-daemon | -v/--verbose | -h/--help | --no-threading ]\n", appname);
   printf("\n-d/--device: Specifies an ALSA or OSS device to use.\n");
   printf("  Examples:\n\t-d hw:1,0\n\t-d /dev/audio\n\t Defaults to \"default\" for alsa and /dev/dsp for OSS\n");

   printf("\n-b/--backend: Specifies which audio backend to use.\n");
   printf("Supported backends: ");
#ifdef _ALSA
   printf("alsa ");
#endif
#ifdef _OSS
   printf("oss ");
#endif
#ifdef _AO
   printf("libao ");
#endif
#ifdef _PORTA
   printf("portaudio ");
#endif
   putchar('\n');
   putchar('\n');

   printf("-p/--port: Defines which port to listen on.\n");
   printf("\tExample: -p 18453. Defaults to port 12345.\n");
   printf("-v/--verbose: Enables verbosity\n");
   printf("-n/--no-daemon: Do not run as daemon.\n");
   printf("--no-threading: Only allows one connection at a time.\n");
   printf("-h/--help: Prints this help\n\n");
}


void pheader(wav_header *w)
{
   fprintf(stderr, "============================================\n");
   fprintf(stderr, "WAV header:\n");

   if (w->numChannels == 1)
      fprintf(stderr, "  Mono | ");
   else if (w->numChannels == 2)
      fprintf(stderr, "  Stereo | ");
   else
      fprintf(stderr, "  Multichannel | ");

   fprintf(stderr, "%d / ", w->sampleRate);
   fprintf(stderr, "%d\n", w->bitsPerSample);

   fprintf(stderr, "============================================\n\n");
}

/* Reads raw 44 bytes WAV header and parses this */
int get_wav_header(int socket, wav_header* head)
{

   int i = is_little_endian();
   /* WAV files are little-endian. If server is big-endian, swaps over data to get sane results. */
   uint16_t temp16;
   uint32_t temp32;

   int rc = 0;
   char header[HEADER_SIZE] = {0};

   rc = recv(socket, header, HEADER_SIZE, 0);
   if ( rc != HEADER_SIZE )
   {
      fprintf(stderr, "Didn't read enough data for WAV header. recv() returned %d. \n", rc);
      return -1;
   }


   temp16 = *((uint16_t*)(header+22));
   if (!i)
      swap_endian_16 ( &temp16 );
   head->numChannels = temp16;

   temp32 = *((uint32_t*)(header+24));
   if (!i)
      swap_endian_32 ( &temp32 );
   head->sampleRate = temp32;

   temp16 = *((uint16_t*)(header+34));
   if (!i)
      swap_endian_16 ( &temp16 );
   head->bitsPerSample = temp16;

   /* Useless stuff (so far)
      memcpy(head->chunkId, header, 4);

      memcpy(&head->chunkSize, header+4, 4);

      memcpy(head->format, header+8, 4);

      memcpy(head->subChunkId, header+12, 4);

      memcpy(&head->subChunkSize, header+16, 4);

      memcpy(&head->audioFormat, header+20, 2);

      memcpy(&head->byteRate, header+28,  4);

      memcpy(&head->blockAlign, header+32, 2);

      memcpy(head->subChunkId, header+36, 4);

      memcpy(&head->subChunkSize2, header+40, 4);
    */

   /* Checks some basic sanity of header file */
   if ( head->sampleRate <= 0 || head->sampleRate > 192000 || head->bitsPerSample % 8 != 0 || head->bitsPerSample == 0 )
   {
      fprintf(stderr, "Got garbage header data ...\n");
      fprintf(stderr, "Channels: %d, Samplerate: %d, Bits/sample: %d\n",
            (int)head->numChannels, (int)head->sampleRate, (int)head->bitsPerSample );
      return -1;
   }

   return 1;
}

int send_backend_info(int socket, uint32_t *chunk_size, uint32_t buffer_size, int channels)
{
   #define IDEAL_PACKAGE_SIZE 1024

   int rc;
   int socket_buffer_size = (int)buffer_size;

   uint32_t temp_chunk_size = *chunk_size;
   
   /* Tries to split package size in 2 until the package size reaches the ideal size. It also needs to be divisble by samplesize (channels * 2) (16 bits) */
   while ( ((temp_chunk_size / (channels*2*2)) * channels*2*2 == temp_chunk_size ) && temp_chunk_size > IDEAL_PACKAGE_SIZE ) 
      temp_chunk_size >>= 1;

   *chunk_size = temp_chunk_size;
   
   if ( setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &socket_buffer_size, sizeof(int)) == -1 )
   {
         fprintf(stderr, "Couldn't set socket buffer size.\n");
         return 0;
   }

   if ( verbose )
      fprintf(stderr, "TCP Packet size = %u\n", temp_chunk_size);

   temp_chunk_size = htonl(*chunk_size);
   buffer_size = htonl(buffer_size);

   rc = send(socket, &temp_chunk_size, sizeof(uint32_t), 0);
   if ( rc != sizeof(uint32_t))
      return 0;
   rc = send(socket, &buffer_size, sizeof(uint32_t), 0);
   if ( rc != sizeof(uint32_t))
      return 0;

   return 1;
}


/* Sets up listening socket for further use */
int set_up_socket()
{
   int rc;
   int s;
   struct addrinfo hints, *servinfo;
   int yes = 1;

   memset(&hints, 0, sizeof (struct addrinfo));
#ifdef __CYGWIN__
   /* Because Windows fails. */
   hints.ai_family = AF_INET;
#else
   hints.ai_family = AF_UNSPEC;
#endif
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;

   if ( verbose )
   {
      fprintf(stderr, "Binding on port %s\n", port);
   }
   if ((rc = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
   {
      fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
      return -1;
   }

   s = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
   if ( s == -1 )
   {
      fprintf(stderr, "Error getting socket\n");
      goto error;
   }

   if ( setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) 
   {
      perror("setsockopt");
      goto error;
   }

   rc = bind(s, servinfo->ai_addr, servinfo->ai_addrlen);
   if ( rc == -1 )
   {
      fprintf(stderr, "Error binding on port %s.\n", port);
      goto error;
   }

   freeaddrinfo(servinfo);
   return s;

error:
   freeaddrinfo(servinfo);
   return -1;

}

int recieve_data(int socket, char* buffer, size_t chunk_size, size_t full_size)
{
   int rc;
   int written = 0;
   int n = socket + 1;
   int count;

   fd_set readfds;
   struct timeval tv;

   FD_ZERO(&readfds);
   FD_SET(socket, &readfds);

   tv.tv_sec = 20;
   tv.tv_usec = 0;

   for ( count = 0; count < (int)full_size; count += (int)chunk_size )
   {
      rc = select(n, &readfds, NULL, NULL, &tv);
      if ( rc == -1 )
         return 0;
      else if ( rc == 0 )
         return 0;

      rc = recv(socket, buffer + count, chunk_size, 0);
      if ( rc <= 0 )
         return 0;
      written += rc;
   }
   return written;
}


