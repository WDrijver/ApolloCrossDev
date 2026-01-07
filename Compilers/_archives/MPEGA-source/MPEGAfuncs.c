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

    File    :   MPEGAfuncs.c

    Author  :   Stéphane TAVENARD

    (C) Copyright 1997-1998 Stéphane TAVENARD
        All Rights Reserved

    #Rev|   Date   |                      Comment
    ----|----------|--------------------------------------------------------
    0   |25/10/1997| Initial revision                                     ST
    1   |01/05/1998| PPC Version                                          ST
    2   |02/06/1998| Added semaphore for PPC                              ST
    3   |05/06/1998| Use Read instead FRead (PPC very slow !!!)           ST
    4   |14/06/1998| Merged M68k & PPC versions                           ST
    5   |21/06/1998| Added MPEGA_scale                                    ST
    6   |23/06/1998| Optimized ppc cache                                  ST

    ------------------------------------------------------------------------

    MPEGA library functions (PPC version)

------------------------------------------------------------------------------*/

#define __USE_SYSBASE

#include <exec/exec.h>
#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <proto/exec.h>

#ifdef _PPC // #4 Begin
// #1 Begin
#include <utility/tagitem.h>
#include <powerup/ppclib/interface.h>
#include <powerup/ppclib/message.h>
#include <powerup/ppclib/tasks.h>
#include <powerup/proto/ppc.h>
#include <dos/stdio.h>
#include <proto/dos.h>
// #1 End
#endif // #4 End

#include "defs.h"
#include "mpegdec.h"

#ifndef _PPC // #4 Begin
//#include "compiler.h"
#endif // #4 End

#include "MPEGAfuncs.h"

#ifdef _PPC // #4 Begin
#include "mpegappc.h"
extern struct SignalSemaphore PPCSemaphore; // #2
#endif // #4 End


#ifndef _PPC // #4 Begin

// note a6 always point to our library

MPEGA_STREAM* __saveds __asm MPEGA_open( register __a0 char *filename,
                                         register __a1 MPEGA_CTRL *ctrl ) {

   return (MPEGA_STREAM *)MPEGDEC_open( filename, (MPEGDEC_CTRL *)ctrl );
}

void  __saveds __asm MPEGA_close( register __a0 MPEGA_STREAM *mpds ) {

   MPEGDEC_close( (MPEGDEC_STREAM *)mpds );
}

LONG  __saveds __asm MPEGA_decode_frame( register __a0 MPEGA_STREAM *mpds,
                                         register __a1 WORD *pcm[ MPEGA_MAX_CHANNELS ] ) {

   return (LONG)MPEGDEC_decode_frame( (MPEGDEC_STREAM *)mpds, pcm );
}

LONG  __saveds __asm MPEGA_seek( register __a0 MPEGA_STREAM *mpds,
                                 register __d0 ULONG ms_time_position ) {

   return (LONG)MPEGDEC_seek( (MPEGDEC_STREAM *)mpds, (UINT32)ms_time_position );
}

LONG  __saveds __asm MPEGA_time( register __a0 MPEGA_STREAM *mpds,
                                 register __a1 ULONG *ms_time_position ) {

   return (LONG)MPEGDEC_time( (MPEGDEC_STREAM *)mpds, (UINT32 *)ms_time_position );
}

LONG  __saveds __asm MPEGA_find_sync( register __a0 BYTE *buffer,
                                      register __d0 LONG buffer_size ) {

   return (LONG)MPEGDEC_find_sync( (INT8 *)buffer, (INT32)buffer_size );

}

// #5 Begin
LONG  __saveds __asm MPEGA_scale( register __a0 MPEGA_STREAM *mpds,
                                  register __d0 LONG scale_percent ) {

   return (LONG)MPEGDEC_scale( (MPEGDEC_STREAM *)mpds, (INT32)scale_percent );
}
// #5 End

#else // PPC Library follows

/* #1 Begin PPC link open/close */

//BPTR Out = NULL;

static void close_ppc_link( MPEGA_FUNC_DATA *mfd ) {

   if( !mfd ) return;

   if( mfd->PPCPort ) {
      PPCReleasePort( mfd->PPCPort );
      mfd->PPCPort = NULL;
   }

   if( mfd->M68kMsg ) {
      PPCDeleteMessage( mfd->M68kMsg );
      mfd->M68kMsg = NULL;
   }
   if( mfd->ReplyPort ) {
      while( PPCDeletePort( mfd->ReplyPort ) == FALSE ) Delay( 1 );
      mfd->ReplyPort = NULL;
   }

   if( mfd->M68kData ) {
      PPCFreeVec( mfd->M68kData );
      mfd->M68kData = NULL;
   }

   if( mfd->M68kPort ) {
      while( PPCDeletePort( mfd->M68kPort ) == FALSE ) Delay( 1 );
      mfd->M68kPort = NULL;
   }

   FreeVec( mfd );
}

static MPEGA_FUNC_DATA *open_ppc_link( void ) {

   MPEGA_FUNC_DATA *mfd;
   struct TagItem MyTags[ 10 ];

   mfd = AllocVec( sizeof( MPEGA_FUNC_DATA ), MEMF_PUBLIC | MEMF_CLEAR );
   if( !mfd ) return NULL;

   MyTags[0].ti_Tag = TAG_DONE;
   mfd->M68kPort = PPCCreatePort( MyTags );
   if( !mfd->M68kPort ) goto error;

   // Allocate M68k Message body
   mfd->M68kData = (struct M68kData *)PPCAllocVec( sizeof(struct M68kData), MEMF_PUBLIC );
   if( !mfd->M68kData ) goto error;

   mfd->M68kData->M68kPort = mfd->M68kPort;

   // Create Reply Port of M68k Message
   MyTags[0].ti_Tag = TAG_DONE;
   mfd->ReplyPort = PPCCreatePort( MyTags );
   if( !mfd->ReplyPort ) goto error;

   // Create M68k Message
   mfd->M68kMsg = PPCCreateMessage( mfd->ReplyPort, sizeof(struct M68kData) );
   if( !mfd->M68kMsg ) goto error;

   // Get PPC Port
   MyTags[0].ti_Tag  = PPCPORTTAG_NAME;
   MyTags[0].ti_Data = (ULONG)MPEGA_PPC_PORT;
   MyTags[1].ti_Tag  = TAG_DONE;
   while( !(mfd->PPCPort = PPCObtainPort(MyTags)) ) Delay( 1 );

   return mfd;

error:
   close_ppc_link( mfd );
   return NULL;
}

/* #1 End PPC link open/close */

/* Default file I/O functions */

static long def_open( char *stream_name, long buffer_size, long *stream_size ) {
/*----------------------------------------------------------------------------
*/
   BPTR file_ptr;

   *stream_size = 0;
   file_ptr = Open( stream_name, MODE_OLDFILE );
   if( (file_ptr) && (buffer_size > 0) ) {
      SetVBuf( file_ptr, NULL, BUF_FULL, buffer_size );
   }

   *stream_size = 0;
   if( file_ptr ) {
       if( Seek( file_ptr, 0, OFFSET_END ) != -1 ) {
          *stream_size = Seek( file_ptr, 0, OFFSET_CURRENT );
          Seek( file_ptr, 0, OFFSET_BEGINNING );
       }
   }
   return (long)file_ptr;
}

static void def_close( long handle ) {
/*----------------------------------
*/
   if( handle ) Close( (BPTR)handle );
}

static long def_read( long handle, void *buffer, long num_bytes ) {
/*---------------------------------------------------------------
*/
   long read_size = -1;

   if( handle ) {
//      read_size = FRead( (BPTR)handle, buffer, 1, num_bytes ); // #3
      read_size = Read( (BPTR)handle, buffer, num_bytes ); // #3
   }
   return read_size;
}

static int def_seek( long handle, long abs_byte_seek_pos ) {
/*--------------------------------------------------------
*/
   int err = 0;

   if( handle ) {
      if( Seek( (BPTR)handle, abs_byte_seek_pos, OFFSET_BEGINNING ) == -1 ) err = -1;
   }
   return err;
}

static ULONG SAVEDS ASM def_baccess( REG(a0) struct Hook  *hook,
                                     REG(a2) APTR          handle,
                                     REG(a1) MPEGA_ACCESS *access ) {
/*-----------------------------------------------------------------------
*/

   switch( access->func ) {

      case MPEGA_BSFUNC_OPEN:
         return (ULONG)def_open( access->data.open.stream_name,
                                 access->data.open.buffer_size,
                                 &access->data.open.stream_size );
      case MPEGA_BSFUNC_CLOSE:
         def_close( (long)handle );
         break;
      case MPEGA_BSFUNC_READ:
         return (ULONG)def_read( (long)handle, access->data.read.buffer,
                                 access->data.read.num_bytes );
      case MPEGA_BSFUNC_SEEK:
         return (ULONG)def_seek( (long)handle, access->data.seek.abs_byte_seek_pos );
   }
   return 0;
}

static void send_ppc( MPEGA_FUNC_DATA *mfd, ULONG msg_id ) {

   ObtainSemaphore( &PPCSemaphore ); // #2

   mfd->M68kData->M68kPort = mfd->M68kPort;

   if( PPCSendMessage( mfd->PPCPort, mfd->M68kMsg, mfd->M68kData, sizeof(struct M68kData), msg_id ) ) {
      PPCWaitPort( mfd->ReplyPort );
      PPCWaitPort( mfd->M68kPort );
      mfd->PPCMsg = PPCGetMessage( mfd->M68kPort );
      if( mfd->PPCMsg ) {
         mfd->PPCData = (struct PPCData *)PPCGetMessageAttr( mfd->PPCMsg, PPCMSGTAG_DATA );
      }
   }
}

static void ack_ppc( MPEGA_FUNC_DATA *mfd ) {

   if( mfd->PPCMsg ) {
      PPCReplyMessage( mfd->PPCMsg );
      mfd->PPCMsg = NULL;
      ReleaseSemaphore( &PPCSemaphore ); // #2
   }
}

static void ppc_to_mpds( MPEGAPPC_STREAM *ppcmpds, MPEGA_STREAM *mpds ) {

   mpds->norm = ppcmpds->norm;
   mpds->layer = ppcmpds->layer;
   mpds->mode = ppcmpds->mode;
   mpds->bitrate = ppcmpds->bitrate;
   mpds->frequency = ppcmpds->frequency;
   mpds->channels = ppcmpds->channels;
   mpds->ms_duration = ppcmpds->ms_duration;
   mpds->private_bit = ppcmpds->private_bit;
   mpds->copyright = ppcmpds->copyright;
   mpds->original = ppcmpds->original;
   mpds->dec_channels = ppcmpds->dec_channels;
   mpds->dec_quality = ppcmpds->dec_quality;
   mpds->dec_frequency = ppcmpds->dec_frequency;
}


// note a6 always point to our library

MPEGA_STREAM* __saveds __asm MPEGA_open( register __a0 char *filename,
                                         register __a1 MPEGA_CTRL *ctrl ) {
   MPEGA_FUNC_DATA *mfd;
   MPEGAPPC_CTRL *ppc_ctrl;
   MPEGA_STREAM *mpds;

   mfd = open_ppc_link();
   if( !mfd ) return NULL;

   // 68k allocate this struct to avoid alignement pbs with ppc
   mpds = AllocVec( sizeof( MPEGA_STREAM ), MEMF_PUBLIC ) ;
   if( !mpds ) {
      close_ppc_link( mfd );
      return NULL;
   }

   // Fill PPC Ctrl structure (coz different alignements)
   mfd->M68kData->func.open.filename = filename;
   ppc_ctrl = &mfd->M68kData->func.open.ctrl;

   ppc_ctrl->bs_access = ctrl->bs_access;
   if( ppc_ctrl->bs_access == NULL ) { // Set our default 68k Hook access
      static struct Hook def_baccess_hook = {
         { NULL, NULL }, def_baccess, NULL, NULL
      };
      ppc_ctrl->bs_access = &def_baccess_hook;
   }

   ppc_ctrl->layer_1_2.force_mono = ctrl->layer_1_2.force_mono;
   ppc_ctrl->layer_1_2.mono.freq_div = ctrl->layer_1_2.mono.freq_div;
   ppc_ctrl->layer_1_2.mono.quality = ctrl->layer_1_2.mono.quality;
   ppc_ctrl->layer_1_2.mono.freq_max = ctrl->layer_1_2.mono.freq_max;
   ppc_ctrl->layer_1_2.stereo.freq_div = ctrl->layer_1_2.stereo.freq_div;
   ppc_ctrl->layer_1_2.stereo.quality = ctrl->layer_1_2.stereo.quality;
   ppc_ctrl->layer_1_2.stereo.freq_max = ctrl->layer_1_2.stereo.freq_max;

   ppc_ctrl->layer_3.force_mono = ctrl->layer_3.force_mono;
   ppc_ctrl->layer_3.mono.freq_div = ctrl->layer_3.mono.freq_div;
   ppc_ctrl->layer_3.mono.quality = ctrl->layer_3.mono.quality;
   ppc_ctrl->layer_3.mono.freq_max = ctrl->layer_3.mono.freq_max;
   ppc_ctrl->layer_3.stereo.freq_div = ctrl->layer_3.stereo.freq_div;
   ppc_ctrl->layer_3.stereo.quality = ctrl->layer_3.stereo.quality;
   ppc_ctrl->layer_3.stereo.freq_max = ctrl->layer_3.stereo.freq_max;

   ppc_ctrl->check_mpeg = ctrl->check_mpeg;
   ppc_ctrl->stream_buffer_size = ctrl->stream_buffer_size;

   send_ppc( mfd, MSG_ID_OPEN );
   if( mfd->PPCData->stream == NULL ) {
      ack_ppc( mfd );
      FreeVec( mpds );
      close_ppc_link( mfd );
      return NULL;
   }

   // Store our private pointers
   mpds->handle = mfd;
   mfd->Stream = mfd->PPCData->stream;

   ppc_to_mpds( mfd->PPCData->stream, mpds );

   ack_ppc( mfd );

   return mpds;

//   return (MPEGA_STREAM *)MPEGDEC_open( filename, (MPEGDEC_CTRL *)ctrl );
}

void  __saveds __asm MPEGA_close( register __a0 MPEGA_STREAM *mpds ) {

   MPEGA_FUNC_DATA *mfd;

   mfd = (MPEGA_FUNC_DATA *)mpds->handle;
   mfd->M68kData->func.run.stream = mfd->Stream;

   send_ppc( mfd, MSG_ID_CLOSE );
   ack_ppc( mfd );

   FreeVec( mpds );

//   MPEGDEC_close( (MPEGDEC_STREAM *)mpds );

   close_ppc_link( mfd );
}

LONG  __saveds __asm MPEGA_decode_frame( register __a0 MPEGA_STREAM *mpds,
                                         register __a1 WORD *pcm[ MPEGA_MAX_CHANNELS ] ) {

   INT32 samples;
   MPEGA_FUNC_DATA *mfd;

   mfd = (MPEGA_FUNC_DATA *)mpds->handle;
   mfd->M68kData->func.run.stream = mfd->Stream;

   // #6 Begin
   {
      int c;
      for( c=0; c<mpds->dec_channels; c++ ) {
         CacheClearE( mfd->PPCData->pcm[ c ], MPEGA_PCM_SIZE * sizeof( WORD ),
                      CACRF_ClearD );
      }
   }
   // #6 End
   send_ppc( mfd, MSG_ID_DECODE );
//   ppc_to_mpds( mfd->PPCData->stream, mpds );
   samples = mfd->PPCData->return_code;

   if( samples > 0 ) {
      int c;
      UINT32 size;

      size = samples * sizeof( WORD );
      for( c=0; c<mpds->dec_channels; c++ ) {
         if( ((ULONG)pcm[ c ] & 0x03) == 0 ) { // Long word aligned
            CopyMemQuick( mfd->PPCData->pcm[ c ], pcm[ c ], size );
         }
         else {
            CopyMem( mfd->PPCData->pcm[ c ], pcm[ c ], size );
         }
      }
   }

   ack_ppc( mfd );

   return samples;

//   return (LONG)MPEGDEC_decode_frame( (MPEGDEC_STREAM *)mpds, pcm );
}

LONG  __saveds __asm MPEGA_seek( register __a0 MPEGA_STREAM *mpds,
                                 register __d0 ULONG ms_time_position ) {

   LONG ret;
   MPEGA_FUNC_DATA *mfd;

   mfd = (MPEGA_FUNC_DATA *)mpds->handle;

   mfd->M68kData->func.run.stream = mfd->Stream;
   mfd->M68kData->func.run.ms_time_position = ms_time_position;

   send_ppc( mfd, MSG_ID_SEEK );
   ppc_to_mpds( mfd->PPCData->stream, mpds );
   ret = mfd->PPCData->return_code;
   ack_ppc( mfd );

   return ret;

//   return (LONG)MPEGDEC_seek( (MPEGDEC_STREAM *)mpds, (UINT32)ms_time_position );
}

LONG  __saveds __asm MPEGA_time( register __a0 MPEGA_STREAM *mpds,
                                 register __a1 ULONG *ms_time_position ) {

   LONG ret;
   MPEGA_FUNC_DATA *mfd;

   mfd = (MPEGA_FUNC_DATA *)mpds->handle;

   mfd->M68kData->func.run.stream = mfd->Stream;

   send_ppc( mfd, MSG_ID_TIME );
   ppc_to_mpds( mfd->PPCData->stream, mpds );
   *ms_time_position = mfd->PPCData->ms_time_position;
   ret = mfd->PPCData->return_code;
   ack_ppc( mfd );

   return ret;

//   return (LONG)MPEGDEC_time( (MPEGDEC_STREAM *)mpds, (UINT32 *)ms_time_position );
}


#define SYNC_VALID( v ) ( ((v & 0xFFE00000) == 0xFFE00000) &&\
                          ((v & 0x00060000) != 0x00000000) &&\
                          ((v & 0xF000) != 0xF000) &&\
                          ((v & 0xF000) != 0x0000) &&\
                          ((v & 0x0C00) != 0xC000) )

INT32 MPEGDEC_find_sync( INT8 *buffer, INT32 buffer_size )
/*--------------------------------------------------------------------------
   Find an mpeg synchronization pattern in a buffer
   This function can be use to check if a file contains MPEG audio stream
   Inputs: buffer = stream buffer to analyze
           buffer_size = need to know top of buffer (must be >= 4)
   Return the the sync position (>=0) or MPEGDEC_ERR_NO_SYNC if not found
*/
{
   register INT32 index = 0;
   register UINT32 value = 0;
   register UINT8 *b;

   b = (UINT8 *)buffer;

   while( index < buffer_size ) {
      value <<= 8;
      value |= (UINT32)(*b++);
      if( (index >= 3) && SYNC_VALID( value ) ) return (index - 3);
      index++;
   }
   return MPEGDEC_ERR_NO_SYNC;
}

LONG  __saveds __asm MPEGA_find_sync( register __a0 BYTE *buffer,
                                      register __d0 LONG buffer_size ) {

   return MPEGDEC_find_sync( buffer, buffer_size );

//   return (LONG)MPEGDEC_find_sync( (INT8 *)buffer, (INT32)buffer_size );
}

// #5 Begin
LONG  __saveds __asm MPEGA_scale( register __a0 MPEGA_STREAM *mpds,
                                  register __d0 LONG scale_percent ) {

   LONG ret;
   MPEGA_FUNC_DATA *mfd;

   mfd = (MPEGA_FUNC_DATA *)mpds->handle;

   mfd->M68kData->func.scale.stream = mfd->Stream;
   mfd->M68kData->func.scale.scale_percent = scale_percent;

   send_ppc( mfd, MSG_ID_SCALE );
   ppc_to_mpds( mfd->PPCData->stream, mpds );
   ret = mfd->PPCData->return_code;
   ack_ppc( mfd );

   return ret;
}
// #5 End

#endif // #4 End

