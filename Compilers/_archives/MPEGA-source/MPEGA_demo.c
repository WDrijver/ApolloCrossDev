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

    File    :   MPEGA_demo.c

    Author  :   Stéphane TAVENARD

    $VER:   MPEGA_demo.c 1.2  (05/06/1998)

    (C) Copyright 1997-1998 Stéphane TAVENARD
        All Rights Reserved

    #Rev|   Date   |                      Comment
    ----|----------|--------------------------------------------------------
    0   |25/10/1997| Initial revision                                     ST
    1   |02/05/1998| Added some time features                             ST
    2   |05/06/1998| Added standard access option                         ST
    3   |08/09/1999| Added verbose informations                           ST

    ------------------------------------------------------------------------

    Demo of how to use MPEGA library
    Use of private bitstream access functions (access to ram buffer)

------------------------------------------------------------------------------*/

#include <exec/exec.h>
#include <clib/exec_protos.h>
#include <pragmas/exec_pragmas.h>
#include <dos.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <libraries/mpega.h>
#include <clib/mpega_protos.h>
#include <pragmas/mpega_pragmas.h>

static char *Version = "$VER:MPEGA_demo 1.2 (05.06.98) (C)1997-1998 Stéphane TAVENARD";

struct Library *MPEGABase = NULL;

char *mpega_name = "mpega.library";

MPEGA_STREAM *mps = NULL;

// Ram buffer
#define MPEGA_BUFFER_SIZE  (256*1024) // in bytes

BYTE   *mpega_buffer = NULL;
ULONG  mpega_buffer_offset = 0;
ULONG  mpega_buffer_size = 0;

static int break_cleanup( void )
{
   if( mps ) {
      MPEGA_close( mps );
      mps = NULL;
   }
   if( MPEGABase ) {
      CloseLibrary( MPEGABase );
      MPEGABase = NULL;
   }
   return 1;
}

static void exit_cleanup( void )
{
   (void)break_cleanup();
}

// Here start our own bitstream access routines

static ULONG __saveds __asm def_baccess( register __a0 struct Hook  *hook,
                                         register __a2 APTR          handle,
                                         register __a1 MPEGA_ACCESS *access ) {
/*----------------------------------------------------------------------------
*/

   switch( access->func ) {

      case MPEGA_BSFUNC_OPEN:
         // We don't really need stream_name
         // buffer_size indicate the following read access size

printf( "bitstream open: filename='%s'\n", access->data.open.stream_name );
printf( "bitstream open: buffer_size=%d\n", access->data.open.buffer_size );

         // Some open errors...
         if( !mpega_buffer ) return NULL;

         // initialize some variables
         mpega_buffer_offset = 0;

         // We know total size, we can set it
         access->data.open.stream_size = mpega_buffer_size;

         // Just return a dummy handle (not NULL)
         return 1;

      case MPEGA_BSFUNC_CLOSE:
         if( handle ) {
            // Clean up
printf( "bitstream close\n" );
         }
         break;
      case MPEGA_BSFUNC_READ: {
         LONG read_size;

         if( !handle ) return 0; // Check valid handle

         read_size = mpega_buffer_size - mpega_buffer_offset;
         if( read_size > access->data.read.num_bytes ) read_size = access->data.read.num_bytes;

         if( read_size > 0 ) {
            if( !access->data.read.buffer ) return 0;
            // Fill buffer with our MPEG audio data
            memcpy( access->data.read.buffer, &mpega_buffer[ mpega_buffer_offset ], read_size );
            mpega_buffer_offset += read_size;
         }
         else {
            read_size = 0; // End of stream
         }
//printf( "bitstream read: requested %d bytes, read %d\n", access->data.read.num_bytes, read_size );

         return (ULONG)read_size;
      }
      case MPEGA_BSFUNC_SEEK:
         if( !handle ) return 0;

printf( "bitstream seek: pos = %d\n", access->data.seek.abs_byte_seek_pos );
         if( access->data.seek.abs_byte_seek_pos <= 0 ) mpega_buffer_offset = 0;
         else if( access->data.seek.abs_byte_seek_pos >= mpega_buffer_size ) return 1;
         else mpega_buffer_offset = access->data.seek.abs_byte_seek_pos;
         return 0;
   }
   return 0;
}

static struct Hook def_bsaccess_hook = {
   { NULL, NULL }, def_baccess, NULL, NULL
};

int output_pcm( WORD channels, WORD *pcm[ 2 ], LONG count, FILE *out_file )
/*---------------------------------------------------------------------------
   Ouput the current decoded PCM to a file
   Return 0 if Ok
*/
{
#define PCM_BUFFER_SIZE (MPEGA_MAX_CHANNELS*MPEGA_PCM_SIZE)
   static WORD *pcm_buffer = NULL;
   if( !out_file ) return -1;

   if( !pcm_buffer ) {
      pcm_buffer = (WORD *)malloc( PCM_BUFFER_SIZE * sizeof(WORD) );
      if( !pcm_buffer ) return -1;
   }
   if( channels == 2 ) {
      register WORD *pcm0, *pcm1, *pcmLR;
      register LONG i;

      pcm0 = pcm[ 0 ];
      pcm1 = pcm[ 1 ];
      pcmLR = pcm_buffer;
      i = count;
      while( i-- ) {
         *pcmLR++ = *pcm0++;
         *pcmLR++ = *pcm1++;
      }
      fwrite( pcm_buffer, 4, count, out_file );
   }
   else {
      fwrite( pcm[ 0 ], 2, count, out_file );
   }

   return 0;

} /* output_pcm */

int main( int argc, char **argv )
{
   char *in_filename;
   FILE *in_file;
   int frame = 0;
   char *out_filename = NULL;
   FILE *out_file = NULL;
   WORD i;
   LONG pcm_count, total_pcm = 0;
   LONG index;
   WORD *pcm[ MPEGA_MAX_CHANNELS ];
   clock_t clk; // #1
   double secs; // #1
   int custom = 1; // #2
   long br_sum = 0;
//ULONG ms;

   static const char *layer_name[] = { "?", "I", "II", "III" };
   static const char *mode_name[] = { "stereo", "j-stereo", "dual", "mono" };

   MPEGA_CTRL mpa_ctrl = {
      NULL,    // Bitstream access is default file I/O
      // Layers I & II settings (mono, stereo)
      { FALSE, { 1, 2, 48000 }, { 1, 2, 48000 } },
      // Layer III settings (mono, stereo)
      { FALSE, { 1, 2, 48000 }, { 1, 2, 48000 } },
      0,           // Don't check mpeg validity at start (needed for mux stream)
      32678        // Stream Buffer size
   };


//printf( "MPEGA_demo: sizeof( MPEGA_CTRL ) = %d\n", sizeof( MPEGA_CTRL ) );
//printf( "MPEGA_demo: sizeof( MPEGA_STREAM ) = %d\n", sizeof( MPEGA_STREAM ) );

   onbreak( break_cleanup );
   atexit( exit_cleanup );

   if( argc <= 1 ) {
      fprintf( stderr, "%s\n", &Version[ 5 ] );
      fprintf( stderr, "Usage %s <input mpeg audio file> [<output pcm file>] [-d]\n", argv[ 0 ] );
      fprintf( stderr, "option: -d = default file access (custom bitstream access otherwise)\n" ); // #2
      fprintf( stderr, "This is a demo of how to use mpega.library\n" );
      fprintf( stderr, "This also show how to use custom bitstream access\n" );
      exit( 0 );
   }

   MPEGABase = OpenLibrary( mpega_name, 0L );
   if( !MPEGABase ) {
      printf( "Unable to open '%s'\n", mpega_name );
      exit( 0 );
   }

   in_filename = argv[ 1 ];

   for( i=2; i<argc; i++ ) { // #2
      if( strcmp( argv[ i ], "-d" ) == 0 ) {
         custom = 0;
      }
      else {
         out_filename = argv[ i ];
      }
   }

   for( i=0; i<MPEGA_MAX_CHANNELS; i++ ) {
      pcm[ i ] = malloc( MPEGA_PCM_SIZE * sizeof( WORD ) );
      if( !pcm[ i ] ) {
         fprintf( stderr, "Can't allocate PCM buffers\n" );
         exit( 0 );
      }
   }

   // Open the output file
   if( out_filename ) {
      out_file = fopen( out_filename, "wb" );
      if( !out_file ) {
         fprintf( stderr, "Can't create output file '%s'\n", out_filename );
         exit( 0 );
      }
   }

   if( custom ) { // #2 (custom bitstream access)
      printf( "Custom bitstream access used\n" ); // #2
      mpega_buffer = (BYTE *)malloc( MPEGA_BUFFER_SIZE );
      if( !mpega_buffer ) {
         fprintf( stderr, "Can't allocate MPEG buffer\n" );
         exit( 0 );
      }


      // Load the stream into a ram buffer
      in_file = fopen( in_filename, "rb" );
      if( !in_file ) {
         fprintf( stderr, "Unable to open file '%s'\n", in_filename );
         exit( 0 );
      }
      mpega_buffer_size = fread( mpega_buffer, 1, MPEGA_BUFFER_SIZE, in_file );
      fclose( in_file );
      printf( "Read %d bytes from file '%s'\n", mpega_buffer_size, in_filename );

      // #1 Begin
      printf( "Test if MPEG Audio sync inside...\n" );
      index = MPEGA_find_sync( mpega_buffer, MPEGA_BUFFER_SIZE );
      if( index >= 0 ) {
         printf( "Ok, found MPEG Audio sync at position %d\n", index );
      }
      else {
         printf( "* Error %d *\n", index );
      }
      // #1 End

      // Set our bitstream access routines and open the stream
      mpa_ctrl.bs_access = &def_bsaccess_hook;
   }
   else {
      printf( "Default bitstream access used\n" ); // #2
printf( "Buffer size = %d\n", mpa_ctrl.stream_buffer_size );
   }

   mps = MPEGA_open( in_filename, &mpa_ctrl );
   if( !mps ) {
      printf( "Unable to find MPEG Audio stream in file '%s'\n", in_filename );
      exit( 0 );
   }

   printf( "\n" );
   printf( "MPEG norm %d Layer %s\n", mps->norm, layer_name[ mps->layer ] );
   printf( "Bitrate: %d kbps\n", mps->bitrate );
   printf( "Frequency: %d Hz\n", mps->frequency );
   printf( "Mode: %d (%s)\n", mps->mode, mode_name[ mps->mode ] );
   printf( "Stream duration: %ld ms\n", mps->ms_duration );
   printf( "\n" );
   printf( "Output decoding parameters\n" );
   printf( "Channels: %d\n", mps->dec_channels );
   printf( "Quality: %d\n", mps->dec_quality );
   printf( "Frequency: %d Hz\n", mps->dec_frequency );
   printf( "\n" );

   clk = clock(); // #1
   while( (pcm_count = MPEGA_decode_frame( mps, pcm )) >= 0 ) {
//MPEGA_time( mps, &ms );

      br_sum += mps->bitrate; // #3
      total_pcm += pcm_count;
      if( out_file ) output_pcm( mps->dec_channels, pcm, pcm_count, out_file );
      frame++;
      if( (frame & 31) == 0 ) {
         fprintf( stderr, "{%04d} %7.3fkbps\r", frame, (double)br_sum / (double)(frame+1) ); fflush( stderr );
      }
   }
   clk = clock() - clk; // #1
   secs = (double)clk / (double)CLK_TCK; // #1
   printf( "\ntime used = %7.3f secs\n", secs ); // #1
   printf( "%ld samples / sec\n", (int)((double)total_pcm / secs) );
   printf( "%7.3f % CPU used on real time\n", ((double)mps->frequency * 100) / ((double)total_pcm / secs) );

   fprintf( stderr, "\n" );

   fprintf( stderr, "last pcm_count = %d\n", pcm_count );
   fprintf( stderr, "total_pcm = %d\n", total_pcm );

   MPEGA_close( mps );
   mps = NULL;
   CloseLibrary( MPEGABase );
   MPEGABase = NULL;

}

