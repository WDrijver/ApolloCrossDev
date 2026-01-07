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

    File    :   mpegappc.h

    Author  :   Stéphane TAVENARD

    (C) Copyright 1998-1998 Stéphane TAVENARD
        All Rights Reserved

    #Rev|   Date   |                      Comment
    ----|----------|--------------------------------------------------------
    0   |02/05/1998| Initial revision                                     ST
    1   |21/06/1998| Added Scale function                                 ST

    ------------------------------------------------------------------------

    MPEGA PPC Library definitions for M68k <-> PPC messages

------------------------------------------------------------------------------*/

#ifndef MPEGAPPC_H
#define MPEGAPPC_H

typedef struct {
   INT32 freq_div;    /* 1, 2 or 4 */
   INT32 quality;     /* 0 (low) .. 2 (high) */
   INT32 freq_max;    /* for automatic freq_div (if mono_freq_div == 0) */
} MPEGAPPC_OUTPUT;

/* Decoding layer settings */
typedef struct {
   INT32 force_mono;        /* 1 to decode stereo stream in mono, 0 otherwise */
   MPEGAPPC_OUTPUT mono;    /* mono settings */
   MPEGAPPC_OUTPUT stereo;  /* stereo settings */
} MPEGAPPC_LAYER;

/* Full control structure of MPEG Audio decoding */
typedef struct {
   struct Hook *bs_access;     /* NULL for default access (file I/O) or give your own bitstream access */
   MPEGAPPC_LAYER layer_1_2;   /* Layer I & II settings */
   MPEGAPPC_LAYER layer_3;     /* Layer III settings */
   INT32 check_mpeg;           /* 1 to check for mpeg audio validity at start of stream, 0 otherwise */
   INT32 stream_buffer_size;   /* size of bitstream buffer in bytes (0 -> default size) */
                               /* NOTE: stream_buffer_size must be multiple of 4 bytes */
} MPEGAPPC_CTRL;

// Struct aligned to PPC requirements

typedef struct {
   /* Public data (read only) */
   /* Stream info */
   INT16 norm;          /* 1 or 2 */
   INT16 layer;         /* 1..3 */
   INT16 mode;          /* 0..3  (MPEGA_MODE_xxx) */
   INT16 bitrate;       /* in kbps */
   INT32 frequency;     /* in Hz */
   INT16 channels;      /* 1 or 2 */
   INT16 pad1;          /* PPC Pad */
   UINT32 ms_duration;   /* stream duration in ms */
   INT16 private_bit;   /* 0 or 1 */
   INT16 copyright;     /* 0 or 1 */
   INT16 original;      /* 0 or 1 */
   /* Decoding info according to MPEG control */
   INT16 dec_channels;  /* decoded channels 1 or 2 */
   INT16 dec_quality;   /* decoding quality 0..2 */
   INT16 pad2;          /* PPC Pad */
   INT32 dec_frequency; /* decoding frequency in Hz */
   /* Private data */
   void  *handle;
} MPEGAPPC_STREAM;

/*
struct StartupData {
   void  *MsgPort;
   ULONG MsgCount;
};
*/

struct M68kData {
   void *M68kPort;
   void *pad;
   union {
      struct {
         char             *filename;
         MPEGAPPC_CTRL     ctrl;
      } open;
      struct {
         MPEGAPPC_STREAM  *stream;
         ULONG ms_time_position;
      } run;
      struct {
         BYTE *buffer;
         LONG buffer_size;
      } find_sync;
      struct { // #1
         MPEGAPPC_STREAM  *stream;
         LONG scale_percent;
      } scale;
   } func;
};

struct PPCData {
   MPEGAPPC_STREAM   *stream;
   INT32  return_code;
   UINT32 ms_time_position;
   INT16  *pcm[ MPEGA_MAX_CHANNELS ];
};

#define MSG_ID_START    0
#define MSG_ID_STOP     1
#define MSG_ID_OPEN     2
#define MSG_ID_CLOSE    3
#define MSG_ID_DECODE   4
#define MSG_ID_SEEK     5
#define MSG_ID_TIME     6
#define MSG_ID_FINDSYNC 7
#define MSG_ID_END      8
#define MSG_ID_SCALE    9 // #1

#define MPEGA_PPC_PORT "MPEGALIB_PPC port"
#define MPEGA_68K_PORT "MPEGALIB_68K port"


#endif /* MPEGAPPC_H */
