/*  RSound - A PCM audio client/server
 *  Copyright (C) 2010-2011 - Hans-Kristian Arntzen
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

#ifndef _PROTO_H
#define _PROTO_H

#include "audio.h"

#define RSD_PROTO_CHUNKSIZE 8
#define RSD_PROTO_MAXSIZE 256

// Defines protocol for RSound
enum
{
   RSD_PROTO_NULL = 0x0000,
   RSD_PROTO_STOP = 0x0001,
   RSD_PROTO_INFO = 0x0002,
   RSD_PROTO_IDENTITY = 0x0003,
   RSD_PROTO_CLOSECTL = 0x0004,
};

int handle_ctl_request(connection_t *conn, void* data);

#endif
