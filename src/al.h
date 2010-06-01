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

#ifndef AL_H
#define AL_H

#include "audio.h"
#include "endian.h"
#include <AL/al.h>
#include <AL/alc.h>

typedef struct
{
   ALuint source;
   ALuint *buffers;
   ALenum format;
   int num_buffers;
   int channels;
   int rate;
   int latency;
   int queue;

   enum rsd_format fmt;
   int conv;

} al_t;

#endif



