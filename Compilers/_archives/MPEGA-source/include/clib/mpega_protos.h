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

    File    :   MPEGA_protos.h

    Author  :   Stéphane TAVENARD

    $VER:   MPEGA_protos.h  2.0  (21/06/1998)

    (C) Copyright 1997-1998 Stéphane TAVENARD
        All Rights Reserved

    #Rev|   Date   |                      Comment
    ----|----------|--------------------------------------------------------
    0   |25/10/1997| Initial revision                                     ST
    1   |21/06/1998| Added MPEGA_scale                                    ST

    ------------------------------------------------------------------------

    MPEGA decoder library functions prototypes

------------------------------------------------------------------------------*/

#ifndef CLIB_MPEGA_PROTOS_H
#define CLIB_MPEGA_PROTOS_H

#ifndef LIBRARIES_MPEGA_H
#include <libraries/mpega.h>
#endif

MPEGA_STREAM *MPEGA_open( char *stream_name, MPEGA_CTRL *ctrl );
/*--------------------------------------------------------------------------
   Open an MPEG Audio stream
   Inputs: stream_name = name of stream to decode
           ctrl = decoding controls
   Return the mpeg audio stream ptr or NULL if failed to open stream
*/

void MPEGA_close( MPEGA_STREAM *mpds );
/*--------------------------------------------------------------------------
   Close an MPEG Audio stream
   Input:  mpds =  mpeg audio stream ptr returned by MPEGA_open
*/

LONG MPEGA_decode_frame( MPEGA_STREAM *mpds, WORD *pcm[ MPEGA_MAX_CHANNELS ] );
/*--------------------------------------------------------------------------
   Decode the current MPEG Audio frame
   Input:  mpds =  mpeg audio stream ptr returned by MPEGA_open
   Output: pcm[] = 16-bit samples
                   pcm[ 0 ] is mono or left voice or channel 1
                   pcm[ 1 ] is right or channel 2
   Return the number of samples or error code:
      MPEGA_ERR_EOF if end of stream
      MPEGA_ERR_BADFRAME if bad frame

   Note: pcm[]'s be at least arrays of MPEGA_PCM_SIZE
         number of samples can be 0 if current frame is skipped, in case
         of error in crc or not enough data for decoding (layer III)
         number of samples = 0 does not indicate end of stream !
*/

LONG MPEGA_seek( MPEGA_STREAM *mpds, ULONG ms_time_position );
/*--------------------------------------------------------------------------
   Seek into an MPEG Audio stream
   Inputs:  mpds = mpeg audio stream ptr returned by MPEGA_open
            ms_time_position = absolute time position in ms
   Return 0 if Ok, MPEGA_ERR_EOF if outside of stream
*/

LONG MPEGA_time( MPEGA_STREAM *mpds, ULONG *ms_time_position );
/*--------------------------------------------------------------------------
   Get the current time position of an MPEG Audio stream
   Input:  mpds = mpeg audio stream ptr returned by MPEGA_open
   Output: ms_time_position = absolute time position in ms
   Return 0 if Ok
*/

LONG MPEGA_find_sync( BYTE *buffer, LONG buffer_size );
/*--------------------------------------------------------------------------
   Find an mpeg synchronization pattern in a buffer
   This function can be use to check if a file contains MPEG audio stream
   Inputs: buffer = stream buffer to analyze
           buffer_size = need to know top of buffer (must be >= 4)
   Return the the sync position (>=0) or MPEGA_ERR_NO_SYNC if not found
*/

LONG MPEGA_scale( MPEGA_STREAM *mpds, LONG scale_percent ); /* #1 */
/*--------------------------------------------------------------------------
   Set the output scale for the current stream
   Avalaible for Version >= 2.0
   Inputs:  mpds = mpeg audio stream ptr returned by MPEGA_open
            scale_percent = scale factor in % to apply to the decoded output
                            100 is the nominal value
   Return 0 if Ok, MPEGA_ERR_BADVALUE if invalid scale
*/


#endif /* CLIB_MPEGA_PROTOS_H */
