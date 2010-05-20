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

#include "audio.h"
#include "endian.h"

static int16_t MULAWTable[256] = { 
     -32124,-31100,-30076,-29052,-28028,-27004,-25980,-24956, 
     -23932,-22908,-21884,-20860,-19836,-18812,-17788,-16764, 
     -15996,-15484,-14972,-14460,-13948,-13436,-12924,-12412, 
     -11900,-11388,-10876,-10364, -9852, -9340, -8828, -8316, 
      -7932, -7676, -7420, -7164, -6908, -6652, -6396, -6140, 
      -5884, -5628, -5372, -5116, -4860, -4604, -4348, -4092, 
      -3900, -3772, -3644, -3516, -3388, -3260, -3132, -3004, 
      -2876, -2748, -2620, -2492, -2364, -2236, -2108, -1980, 
      -1884, -1820, -1756, -1692, -1628, -1564, -1500, -1436, 
      -1372, -1308, -1244, -1180, -1116, -1052,  -988,  -924, 
       -876,  -844,  -812,  -780,  -748,  -716,  -684,  -652, 
       -620,  -588,  -556,  -524,  -492,  -460,  -428,  -396, 
       -372,  -356,  -340,  -324,  -308,  -292,  -276,  -260, 
       -244,  -228,  -212,  -196,  -180,  -164,  -148,  -132, 
       -120,  -112,  -104,   -96,   -88,   -80,   -72,   -64, 
        -56,   -48,   -40,   -32,   -24,   -16,    -8,     0, 
      32124, 31100, 30076, 29052, 28028, 27004, 25980, 24956, 
      23932, 22908, 21884, 20860, 19836, 18812, 17788, 16764, 
      15996, 15484, 14972, 14460, 13948, 13436, 12924, 12412, 
      11900, 11388, 10876, 10364,  9852,  9340,  8828,  8316, 
       7932,  7676,  7420,  7164,  6908,  6652,  6396,  6140, 
       5884,  5628,  5372,  5116,  4860,  4604,  4348,  4092, 
       3900,  3772,  3644,  3516,  3388,  3260,  3132,  3004, 
       2876,  2748,  2620,  2492,  2364,  2236,  2108,  1980, 
       1884,  1820,  1756,  1692,  1628,  1564,  1500,  1436, 
       1372,  1308,  1244,  1180,  1116,  1052,   988,   924, 
        876,   844,   812,   780,   748,   716,   684,   652, 
        620,   588,   556,   524,   492,   460,   428,   396, 
        372,   356,   340,   324,   308,   292,   276,   260, 
        244,   228,   212,   196,   180,   164,   148,   132, 
        120,   112,   104,    96,    88,    80,    72,    64, 
         56,    48,    40,    32,    24,    16,     8,     0 
}; 

static int16_t ALAWTable[256] =  { 
     -5504, -5248, -6016, -5760, -4480, -4224, -4992, -4736, 
     -7552, -7296, -8064, -7808, -6528, -6272, -7040, -6784, 
     -2752, -2624, -3008, -2880, -2240, -2112, -2496, -2368, 
     -3776, -3648, -4032, -3904, -3264, -3136, -3520, -3392, 
     -22016,-20992,-24064,-23040,-17920,-16896,-19968,-18944, 
     -30208,-29184,-32256,-31232,-26112,-25088,-28160,-27136, 
     -11008,-10496,-12032,-11520,-8960, -8448, -9984, -9472, 
     -15104,-14592,-16128,-15616,-13056,-12544,-14080,-13568, 
     -344,  -328,  -376,  -360,  -280,  -264,  -312,  -296, 
     -472,  -456,  -504,  -488,  -408,  -392,  -440,  -424, 
     -88,   -72,   -120,  -104,  -24,   -8,    -56,   -40, 
     -216,  -200,  -248,  -232,  -152,  -136,  -184,  -168, 
     -1376, -1312, -1504, -1440, -1120, -1056, -1248, -1184, 
     -1888, -1824, -2016, -1952, -1632, -1568, -1760, -1696, 
     -688,  -656,  -752,  -720,  -560,  -528,  -624,  -592, 
     -944,  -912,  -1008, -976,  -816,  -784,  -880,  -848, 
      5504,  5248,  6016,  5760,  4480,  4224,  4992,  4736, 
      7552,  7296,  8064,  7808,  6528,  6272,  7040,  6784, 
      2752,  2624,  3008,  2880,  2240,  2112,  2496,  2368, 
      3776,  3648,  4032,  3904,  3264,  3136,  3520,  3392, 
      22016, 20992, 24064, 23040, 17920, 16896, 19968, 18944, 
      30208, 29184, 32256, 31232, 26112, 25088, 28160, 27136, 
      11008, 10496, 12032, 11520, 8960,  8448,  9984,  9472, 
      15104, 14592, 16128, 15616, 13056, 12544, 14080, 13568, 
      344,   328,   376,   360,   280,   264,   312,   296, 
      472,   456,   504,   488,   408,   392,   440,   424, 
      88,    72,   120,   104,    24,     8,    56,    40, 
      216,   200,   248,   232,   152,   136,   184,   168, 
      1376,  1312,  1504,  1440,  1120,  1056,  1248,  1184, 
      1888,  1824,  2016,  1952,  1632,  1568,  1760,  1696, 
      688,   656,   752,   720,   560,   528,   624,   592, 
      944,   912,  1008,   976,   816,   784,   880,   848 
}; 

const char* rsnd_format_to_string(enum rsd_format fmt)
{
   switch(fmt)
   {
      case RSD_S16_LE:
         return "Signed 16-bit little-endian";
      case RSD_S16_BE:
         return "Signed 16-bit big-endian";
      case RSD_U16_LE:
         return "Unsigned 16-bit little-endian";
      case RSD_U16_BE:
         return "Unsigned 16-bit big-endian";
      case RSD_U8:
         return "Unsigned 8-bit";
      case RSD_S8:
         return "Signed 8-bit";
      case RSD_ALAW:
         return "a-law";
      case RSD_MULAW:
         return "mu-law";
      case RSD_UNSPEC:
         break;
   }
   return "Unknown format";
}

int rsnd_format_to_bytes(enum rsd_format fmt)
{
   switch(fmt)
   {
      case RSD_S16_LE:
      case RSD_S16_BE:
      case RSD_U16_LE:
      case RSD_U16_BE:
         return 2;
      case RSD_U8:
      case RSD_S8:
      case RSD_ALAW:
      case RSD_MULAW:
         return 1;
      case RSD_UNSPEC:
         break;
   }
   return -1;
}


inline static void swap_bytes16(uint16_t *data, size_t bytes)
{
   int i;
   for ( i = 0; i < (int)bytes/2; i++ )
   {
      swap_endian_16(&data[i]);
   }
}

inline static void alaw_to_s16(uint8_t *data, size_t bytes)
{
   int16_t buf[bytes];
   int i;
   for ( i = 0; i < (int)bytes; i++ )
   {
      buf[i] = ALAWTable[data[i]];
   }
   memcpy(data, buf, sizeof(buf));
}

inline static void mulaw_to_s16(uint8_t *data, size_t bytes)
{
   int16_t buf[bytes];
   int i;
   for ( i = 0; i < (int)bytes; i++ )
   {
      buf[i] = MULAWTable[data[i]];
   }
   memcpy(data, buf, sizeof(buf));
}

void audio_converter(void* data, enum rsd_format fmt, int operation, size_t bytes)
{
   if ( operation == RSD_NULL )
      return;

   if ( operation & RSD_ALAW_TO_S16 )
   {
      alaw_to_s16((uint8_t*)data, bytes);
      return;
   }

   if ( operation & RSD_MULAW_TO_S16 )
   {
      mulaw_to_s16((uint8_t*)data, bytes);
      return;
   }

   // Temporarily hold the data that is to be sign-flipped.
   int32_t temp32;
   int i;
   int swapped = 0;
   int bits = rsnd_format_to_bytes(fmt) * 8;

   uint8_t buffer[bytes];

   // Fancy union to make the conversions more clean looking ;)
   union
   {
      uint8_t *u8;
      int8_t *s8;
      uint16_t *u16;
      int16_t *s16;
      void *ptr;
   } u;

   memcpy(buffer, data, bytes);
   u.ptr = buffer;

   i = 0;
   if ( is_little_endian() )
      i++;
   if ( fmt & (RSD_S16_BE | RSD_U16_BE) )
      i++;

   // If we're gonna operate on the data, we better make sure that we are in native byte order
   if ( (i % 2) == 0 && bits == 16 )
   {
      swap_bytes16(u.u16, bytes);
      swapped = 1;
   }
   
   if ( operation & RSD_S_TO_U )
   {
      if ( bits == 8 )
      {
         for ( i = 0; i < (int)bytes; i++ )
         {
            temp32 = (u.s8)[i];
            temp32 += (1 << 7);
            (u.u8)[i] = (uint8_t)temp32;
         }
      }

      else if ( bits == 16 )
      {
         for ( i = 0; i < (int)bytes/2; i++ )
         {
            temp32 = (u.s16)[i];
            temp32 += (1 << 15);
            (u.u16)[i] = (uint16_t)temp32;
         }
      }
   }

   else if ( operation & RSD_U_TO_S )
   {
      if ( bits == 8 )
      {
         for ( i = 0; i < (int)bytes; i++ )
         {
            temp32 = (u.u8)[i];
            temp32 -= (1 << 7);
            (u.s8)[i] = (int8_t)temp32;
         }
      }

      else if ( bits == 16 )
      {
         for ( i = 0; i < (int)bytes/2; i++ )
         {
            temp32 = (u.u16)[i];
            temp32 -= (1 << 15);
            (u.s16)[i] = (int16_t)temp32;
         }
      }
   }

   if ( operation & RSD_SWAP_ENDIAN )
   {
      if ( !swapped && bits == 16 )
      {
         swap_bytes16(u.u16, bytes);
      }
   }

   else if ( swapped ) // We need to flip back. Kinda expensive, but hey. An endian-flipping-less algorithm will be more complex. :)
   {
      swap_bytes16(u.u16, bytes);
   }

   memcpy(data, buffer, bytes);
}




