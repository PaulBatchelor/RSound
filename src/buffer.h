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

#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct rsound_fifo_buffer rsound_fifo_buffer_t;

rsound_fifo_buffer_t* fifo_new(size_t size);
void fifo_write(rsound_fifo_buffer_t* buffer, const void* in_buf, size_t size);
void fifo_read(rsound_fifo_buffer_t* buffer, void* in_buf, size_t size);
void fifo_free(rsound_fifo_buffer_t* buffer);
size_t fifo_read_avail(rsound_fifo_buffer_t* buffer);
size_t fifo_write_avail(rsound_fifo_buffer_t* buffer);
