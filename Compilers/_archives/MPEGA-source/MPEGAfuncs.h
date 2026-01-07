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
/*------------------------------------------------------------------------------

    File    :   MPEGAfuncs.h

    Author  :   Stéphane TAVENARD

    (C) Copyright 1997-1998 Stéphane TAVENARD
        All Rights Reserved

    #Rev|   Date   |                      Comment
    ----|----------|--------------------------------------------------------
    0   |25/10/1997| Initial revision                                     ST
    1   |01/05/1998| PPC Version                                          ST
    2   |14/06/1998| Merged M68k & PPC Versions                           ST
    3   |21/06/1998| Added MPEGA_scale                                    ST

    ------------------------------------------------------------------------

    MPEGA library functions definition (M68k & PPC version)

------------------------------------------------------------------------------*/

#ifndef MPEGAFUNCS_H
#define MPEGAFUNCS_H

#include <libraries/mpega.h>

#ifdef _PPC // #2 Begin

extern struct Library *PPCLibBase;
extern void  *ElfObject;

typedef struct {
   // For PPC I/O
   void *M68kPort;
   struct M68kData *M68kData; // To PPC
   void *ReplyPort;
   void *M68kMsg;
   void *PPCPort;

   void *PPCMsg;
   struct PPCData  *PPCData; // From PPC

   // MPEG Audio stream
   void *Stream;

} MPEGA_FUNC_DATA;

#endif // #2 end

MPEGA_STREAM* __saveds __asm MPEGA_open( register __a0 char *filename,
                                         register __a1 MPEGA_CTRL *ctrl );

void  __saveds __asm MPEGA_close( register __a0 MPEGA_STREAM *mpds );

LONG  __saveds __asm MPEGA_decode_frame( register __a0 MPEGA_STREAM *mpds,
                                         register __a1 WORD *pcm[ MPEGA_MAX_CHANNELS ] );

LONG  __saveds __asm MPEGA_seek( register __a0 MPEGA_STREAM *mpds,
                                 register __d0 ULONG ms_time_position );

LONG  __saveds __asm MPEGA_time( register __a0 MPEGA_STREAM *mpds,
                                 register __a1 ULONG *ms_time_position );

LONG  __saveds __asm MPEGA_find_sync( register __a0 BYTE *buffer,
                                      register __d0 LONG buffer_size );

LONG  __saveds __asm MPEGA_scale( register __a0 MPEGA_STREAM *mpds,  // #3
                                  register __d0 LONG scale_percent );

#endif /* MPEGAFUNCS_H */

