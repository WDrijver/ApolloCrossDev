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

    File    :   mpegappc_library.c

    Author  :   Stéphane TAVENARD

    (C) Copyright 1998-1998 Stéphane TAVENARD
        All Rights Reserved

    #Rev|   Date   |                      Comment
    ----|----------|--------------------------------------------------------
    0   |02/05/1998| Initial revision                                     ST
    1   |02/06/1998| Try to fix multi-instances bug                       ST
    2   |21/06/1998| Added Scale function                                 ST
    3   |23/06/1998| Optimized ppc cache                                  ST

    ------------------------------------------------------------------------

    Mpega library for PowerPC

------------------------------------------------------------------------------*/

#include "defs.h"
#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <utility/tagitem.h>
#include <powerup/ppclib/interface.h>
#include <powerup/ppclib/message.h>
#include <powerup/ppclib/tasks.h>
#include <powerup/gcclib/powerup_protos.h>

#include <libraries/mpega.h>
#include "mpegappc.h"
#include "mpegdec.h"

#define bye( m ) ppc_bye( m ); return

int errno = 0;

//struct StartupData *StartupData = NULL;

struct M68kData *M68kData = NULL; // From M68k
struct PPCData  *PPCData  = NULL; // To M68k

void  *ReplyPort = NULL;
void  *PPCPort = NULL;
void  *PPCMsg = NULL;
void  *M68kPort = NULL;
void  *M68kMsg = NULL;

BPTR  MyFile = NULL;
ULONG DataArray[ 10 ];
char  TextBuffer[ 500 ];

// ************ ATTENTION ************
// *** Pas de fonctions avant MAIN ***
// ***********************************

static void ppc_bye( char *msg );

int main( void ) {
   struct TagItem MyTags[ 10 ];
   ULONG result;
   ULONG i = 0;
   ULONG j;
   int running = 1;

//   StartupData = (struct StartupData *)PPCGetTaskAttr( PPCTASKTAG_STARTUP_MSGDATA );
//   MsgCount = StartupData->MsgCount;

#ifdef DEBUG
//   MyFile = PPCOpen( "*", MODE_OLDFILE );
   MyFile = PPCOpen( "CON:0/0/640/200/MPEGALibraryPPC output/CLOSE", MODE_NEWFILE );
#endif

   // Create PPC Port
   MyTags[ 0 ].ti_Tag  = PPCPORTTAG_NAME;
   MyTags[ 0 ].ti_Data = (ULONG)MPEGA_PPC_PORT;
   MyTags[ 1 ].ti_Tag  = TAG_DONE;
DP( "PPC: Create PPC Port\n" );
   PPCPort = PPCCreatePort( MyTags );
   if( !PPCPort ) {
      bye( "Can't create PPC port\n" );
   }

   MyTags[ 0 ].ti_Tag = TAG_DONE;
DP( "PPC: Create Reply Port\n" );
   ReplyPort = PPCCreatePort( MyTags );
   if( !ReplyPort ) {
      bye( "Can't create ReplyPort\n" );
   }

DP( "PPC: Create PPC Msg\n" );
   PPCMsg = PPCCreateMessage( ReplyPort, sizeof( struct PPCData ) );
   if( !PPCMsg ) {
      bye( "Can't create PPCMsg\n" );
   }

   // Allocate PPC Message body
DP( "PPC: Allocate PPCData\n" );
   PPCData = (struct PPCData *)PPCAllocVec( sizeof(struct PPCData), MEMF_PUBLIC );
   if( !PPCData ) {
      bye( "Can't allocate PPCData\n" );
   }
   // Non cachable memory area (Written by PPC, Read by M68k)
   for( i=0; i<MPEGA_MAX_CHANNELS; i++ ) {
      PPCData->pcm[ i ] = (INT16 *)PPCAllocVec( MPEGA_PCM_SIZE * sizeof( INT16 ),
                                                MEMF_PUBLIC | MEMF_CLEAR
                                                /* #3 supressed | MEMF_NOCACHESYNCPPC | MEMF_NOCACHESYNCM68K*/ );
      if( !PPCData->pcm[ i ] ) {
         bye( "Can't allocate PPCData->pcm\n" );
      }
   }

   do {
      ULONG msg_id;

      PPCWaitPort( PPCPort );
      M68kMsg = PPCGetMessage( PPCPort );
      if( M68kMsg ) { // #1
         msg_id = PPCGetMessageAttr( M68kMsg, PPCMSGTAG_MSGID );
         if( (msg_id != MSG_ID_START) && (msg_id != MSG_ID_STOP) ) {
            M68kData = (struct M68kData *)PPCGetMessageAttr( M68kMsg, PPCMSGTAG_DATA );
            if( M68kData ) M68kPort = M68kData->M68kPort;
         }
         switch( msg_id ) {
            case MSG_ID_START:
               break;
            case MSG_ID_STOP:
DP( "PPC:STOP\n" );
               running = 0;
               break;
            case MSG_ID_OPEN:
               {
                  MPEGDEC_CTRL ctrl;
                  MPEGAPPC_CTRL *ppc_ctrl;

DP( "PPC:OPEN\n" );
DPI( "PPC: sizeof( MPEGAPPC_STREAM ) = %ld\n", sizeof( MPEGAPPC_STREAM ) );
DPI( "PPC: sizeof( MPEGDEC_STREAM ) = %ld\n", sizeof( MPEGDEC_STREAM ) );
DPI( "PPC: sizeof( MPEGDEC_CTRL ) = %ld\n", sizeof( MPEGDEC_CTRL ) );
DPI( "PPC: sizeof( MPEGAPPC_CTRL ) = %ld\n", sizeof( MPEGAPPC_CTRL ) );
                  ppc_ctrl = &M68kData->func.open.ctrl;

                  ctrl.bs_access = (MPEGDEC_ACCESS *)ppc_ctrl->bs_access;
DPI( "PPC: Access 0x%08lX\n", (int)ctrl.bs_access );
                  ctrl.layer_1_2.force_mono = ppc_ctrl->layer_1_2.force_mono;
                  ctrl.layer_1_2.mono.freq_div = ppc_ctrl->layer_1_2.mono.freq_div;
                  ctrl.layer_1_2.mono.quality = ppc_ctrl->layer_1_2.mono.quality;
                  ctrl.layer_1_2.mono.freq_max = ppc_ctrl->layer_1_2.mono.freq_max;
                  ctrl.layer_1_2.stereo.freq_div = ppc_ctrl->layer_1_2.stereo.freq_div;
                  ctrl.layer_1_2.stereo.quality = ppc_ctrl->layer_1_2.stereo.quality;
                  ctrl.layer_1_2.stereo.freq_max = ppc_ctrl->layer_1_2.stereo.freq_max;

                  ctrl.layer_3.force_mono = ppc_ctrl->layer_3.force_mono;
                  ctrl.layer_3.mono.freq_div = ppc_ctrl->layer_3.mono.freq_div;
                  ctrl.layer_3.mono.quality = ppc_ctrl->layer_3.mono.quality;
                  ctrl.layer_3.mono.freq_max = ppc_ctrl->layer_3.mono.freq_max;
                  ctrl.layer_3.stereo.freq_div = ppc_ctrl->layer_3.stereo.freq_div;
                  ctrl.layer_3.stereo.quality = ppc_ctrl->layer_3.stereo.quality;
                  ctrl.layer_3.stereo.freq_max = ppc_ctrl->layer_3.stereo.freq_max;

                  ctrl.check_mpeg = ppc_ctrl->check_mpeg;
                  ctrl.stream_buffer_size = ppc_ctrl->stream_buffer_size;
DPI( "PPC: Stream Buffer Size %ld\n", ctrl.stream_buffer_size );

DP( "PPC: Filename='" );
DP( M68kData->func.open.filename );
DP( "'\n" );
                  PPCData->stream = (MPEGAPPC_STREAM *)MPEGDEC_open( M68kData->func.open.filename, &ctrl );
PPCCacheFlush( PPCData->stream, sizeof( MPEGDEC_STREAM ) );
DPI( "PPC: Stream PTR 0x%08lX\n", (int)PPCData->stream );
               }
               break;
            case MSG_ID_CLOSE:
//PPCCacheFlushAll();
               PPCData->stream = M68kData->func.run.stream;
DPI( "PPC: CLOSE Stream PTR 0x%08lX...", (int)PPCData->stream );
               MPEGDEC_close( (MPEGDEC_STREAM *)PPCData->stream );
               PPCData->return_code = 0;
//PPCCacheFlushAll();
               break;
            case MSG_ID_DECODE:
               PPCData->stream = M68kData->func.run.stream;
//PPCCacheFlushAll();
DPI( "PPC: DECODE Stream PTR 0x%08lX...", (int)PPCData->stream );
               PPCData->return_code = MPEGDEC_decode_frame( (MPEGDEC_STREAM *)PPCData->stream,
                                                            PPCData->pcm );
//PPCCacheFlush( PPCData->stream, sizeof( MPEGDEC_STREAM ) );
               // #3 Begin
               PPCCacheFlush( PPCData->pcm[ 0 ], MPEGA_PCM_SIZE * sizeof( INT16 ) );
               PPCCacheFlush( PPCData->pcm[ 1 ], MPEGA_PCM_SIZE * sizeof( INT16 ) );
               // #3 End
               break;
            case MSG_ID_SEEK:
DP( "PPC:SEEK..." );
               PPCData->stream = M68kData->func.run.stream;
               PPCData->return_code = MPEGDEC_seek( (MPEGDEC_STREAM *)PPCData->stream,
                                                    M68kData->func.run.ms_time_position );
               break;
            case MSG_ID_TIME:
DP( "PPC:TIME..." );
               {
                  ULONG ms;

                  PPCData->stream = M68kData->func.run.stream;
                  PPCData->return_code = MPEGDEC_time( (MPEGDEC_STREAM *)PPCData->stream, &ms );
                  PPCData->ms_time_position = ms;
               }
               break;
            case MSG_ID_FINDSYNC:
DP( "PPC:FINDSYNC..." );
               PPCData->return_code = MPEGDEC_find_sync( M68kData->func.find_sync.buffer,
                                                         M68kData->func.find_sync.buffer_size );
               break;
            // #2 Begin
            case MSG_ID_SCALE:
DP( "PPC:SCALE..." );
               PPCData->stream = M68kData->func.scale.stream;
               PPCData->return_code = MPEGDEC_scale( (MPEGDEC_STREAM *)PPCData->stream,
                                                     M68kData->func.scale.scale_percent );
               break;
            // #2 End
            default:
               break;
         }
         PPCReplyMessage( M68kMsg );

         if( running ) {
            // Send Msg to M68k
//PPCCacheFlushAll();
            PPCSendMessage( M68kPort, PPCMsg, PPCData, sizeof(struct PPCData), msg_id );
            PPCWaitPort( ReplyPort );
DP( "OK\n" );
         }
      }

   } while( running );

DP( "PPC: *END*\n" );

   bye( NULL );
}

#ifdef DEBUG
void print_int( char *fmt, int value ) {

   if( !MyFile ) return;
   DataArray[ 0 ] = value;
   PPCRawDoFmt( fmt,
                &DataArray,
                0,
                TextBuffer);
   PPCWrite( MyFile, TextBuffer, strlen(TextBuffer) );
}
#endif


static void ppc_bye( char *msg ) {

   int i;

   if( msg ) {
      if( MyFile ) {
         PPCWrite( MyFile, msg, strlen( msg ) );
      }
   }

DP( "PPC:END free PPCData..." );
   if( PPCData ) {
      for( i=0; i<MPEGA_MAX_CHANNELS; i++ ) {
         if( PPCData->pcm[ i ] ) {
            PPCFreeVec( PPCData->pcm[ i ] );
         }
      }
      PPCFreeVec( PPCData );
      PPCData = NULL;
   }

DP( "PPC:END Delete Message PPCMsg..." );
   if( PPCMsg ) {
      PPCDeleteMessage( PPCMsg );
      PPCMsg = NULL;
   }
DP( "PPC:END DeletePort ReplyPort..." );
   if( ReplyPort ) {
      while( PPCDeletePort( ReplyPort ) == FALSE );
      ReplyPort = NULL;
   }
DP( "PPC:END DeletePort PPCPort..." );
   if( PPCPort ) {
      while( PPCDeletePort( PPCPort ) == FALSE );
      PPCPort = NULL;
   }
   if( MyFile ) {
      PPCClose( MyFile );
      MyFile = NULL;
   }
//   exit( 0 );
}




