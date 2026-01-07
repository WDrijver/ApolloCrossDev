/*
* This file is part of MPEGALibrary.
* Copyright (C) 1999 Stephane Tavenard
* 
* MPEGALibrary is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* MPEGALibrary is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with MPEGALibrary.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/*
   mpega.library (C) 1997-1998 Stéphane TAVENARD
*/
#ifndef PRAGMAS_MPEGA_PRAGMAS_H
#define PRAGMAS_MPEGA_PRAGMAS_H

#ifndef CLIB_MPEGA_PROTOS_H
#include <clib/mpega_protos.h>
#endif

#pragma libcall MPEGABase MPEGA_open         01E 9802
#pragma libcall MPEGABase MPEGA_close        024 801
#pragma libcall MPEGABase MPEGA_decode_frame 02A 9802
#pragma libcall MPEGABase MPEGA_seek         030 0802
#pragma libcall MPEGABase MPEGA_time         036 9802
#pragma libcall MPEGABase MPEGA_find_sync    03C 0802
#pragma libcall MPEGABase MPEGA_scale        042 0802


#endif /* PRAGMAS_MPEGA_PRAGMAS_H */
