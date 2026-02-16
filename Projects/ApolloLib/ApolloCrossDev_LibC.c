// ApolloCrossDev Library
// 21-1-2025

#include "ApolloCrossDev_Lib.h"

extern char ApolloDebugMessage[200];

uint8_t ApolloLoadFile(struct ApolloFile *file)
{
	unsigned long file_size = 0;    // length of file
	unsigned long file_read = 0;   	// bytes read from file
	FILE *file_handle = 0;          // stream of file 

	ADX(sprintf(ApolloDebugMessage, "ApolloLoadFile: Opening File = %s", file->filename);)
    ADX(ApolloDebugPutStr(ApolloDebugMessage);)

    file_handle = fopen(file->filename, "rb");
    if (!file_handle)
    {
	   	ADX(ApolloDebugPutStr( "= ERROR\n");)
	    return APOLLO_SOUND_LOADERROR;
    } else {
		ADX(ApolloDebugPutStr( "= SUCCESS\n");)
    }

    fseek(file_handle, 0, SEEK_END);													// Goto end of file
    file_size = ftell(file_handle);														// Retrieve filesize

	file->size = file_size - file->offset;
	
	fseek(file_handle, file->offset, SEEK_SET);											// goto offset in file	

	ADX(ApolloDebugPutStr("ApolloLoadFile: Reading file -> ");)

	file_read = fread(file->buffer, 1, file->size, file_handle);	// Read file into aligned buffer	
    if(file_read != file->size)
    {
	   	ADX(ApolloDebugPutStr( "ERROR: Cannot load file\n");)
	    return APOLLO_SOUND_LOADERROR;
    } else {
	   	ADX(ApolloDebugPutStr( "SUCCESS: File loaded\n");)
    }

    ADX(ApolloDebugPutStr("ApolloLoadFile: Closing file -> ");)

    if (fclose(file_handle) == EOF)
    {
	   	ADX(ApolloDebugPutStr( "ERROR: Cannot close file\n");)
		return APOLLO_SOUND_CLOSEERR;					
    } else {
	   	ADX(ApolloDebugPutStr( "SUCCESS: File closed\n");)
    }

	ADX(sprintf(ApolloDebugMessage, "ApolloLoadFile: %s | Filesize: %d | Offset = %d\n", file->filename, file->size, file->offset);)
	ADX(ApolloDebugPutStr(ApolloDebugMessage);)

	return APOLLO_SOUND_OK;
}


// Apollo Sound Functions 

uint8_t ApolloAllocSound( struct ApolloSound *sound)
{
	uint8_t *buffer_aligned = 0;	// 32-Byte aligned input buffer
	
	sound->buffer = (uint8_t*)AllocVec(sound->size+31, MEMF_ANY);	// allocate buffer memory with extra 31 bytes for alignment 
	if (!sound->buffer)
	{
		ADX(ApolloDebugPutStr( "ApolloAllocPicture: Buffer memory allocation ERROR\n");)
		return APOLLO_PICTURE_MEMERROR;
	} else {
		ADX(ApolloDebugPutStr( "ApolloAllocPicture: Buffer memory allocated\n");)
	}

	buffer_aligned = (uint8_t*)(((uint32_t)(sound->buffer+31) & ~31));	// align buffer to 32-byte boundary

	sound->position = buffer_aligned - sound->buffer;				// Report back position of aligned buffer within allocated buffer

	//sound->filename[0] = 0;	// Clear filename to indicate memory-only sound
	
	ADX(sprintf(ApolloDebugMessage, "ApolloAllocSound: Sound Allocated: Size=%d | Position=%d\n",
		 sound->size, sound->position);)
	ADX(ApolloDebugPutStr(ApolloDebugMessage);)

	return APOLLO_SOUND_OK;
}

void ApolloFreeSound( struct ApolloSound *sound)
{
	ADX(sprintf(ApolloDebugMessage, "ApolloFreeSound: Free Sound: %s . . . ", sound->filename);)
	ADX(ApolloDebugPutStr(ApolloDebugMessage);)

	if (sound->buffer)
	{
		FreeVec(sound->buffer);					
		sound->buffer = NULL;
	}

	ADX(ApolloDebugPutStr("Done.\n");)
}

uint8_t ApolloLoadSound( struct ApolloSound *sound)
{
	uint8_t *buffer_aligned = 0;	// 32-Byte aligned input buffer	
	unsigned long file_size = 0;    // length of file
	unsigned long file_read = 0;   	// bytes read from file
	FILE *file_handle = 0;          // stream of file 

	uint16_t samplerate_exponent = 0;
	uint16_t samplerate_fraction = 0;
	
	uint32_t sample_offset = 0;
	bool scanning = true;
	uint32_t offset = 0; 
	
	struct AIFFHeader aiffheader;
	struct WAVHeader wavheader;

	ADX(sprintf(ApolloDebugMessage, "ApolloLoadSound: Opening File = %s", sound->filename);)
    ADX(ApolloDebugPutStr(ApolloDebugMessage);)

    file_handle = fopen(sound->filename, "rb");
    if (!file_handle)
    {
	   	ADX(ApolloDebugPutStr( "= ERROR\n");)
	    return APOLLO_SOUND_LOADERROR;
    } else {
		ADX(ApolloDebugPutStr( "= SUCCESS\n");)
    }

    fseek(file_handle, 0, SEEK_END);													// Goto end of file
    file_size = ftell(file_handle);														// Retrieve filesize

	switch(sound->format)
	{
		case APOLLO_AIFF_FORMAT:							
			fseek(file_handle, 0, SEEK_SET);				
			fread(&aiffheader, sizeof(struct AIFFHeader), 1, file_handle); 				// Read first Chunk Header
			offset += 12;																// Increase offset with 8 = ID [LONG] + File Size [LONG] + FORM Type [LONG]											
			if( aiffheader.file_id != 0x464F524D) return APOLLO_SOUND_NOHEADER;			// 'FORM'
			while(scanning)																
			{
				fseek(file_handle, offset, SEEK_SET);
				fread(&aiffheader, sizeof(struct AIFFHeader), 1, file_handle);			// ID [LONG] + Chunk Size [LONG]
				offset +=8 ;															// Increase offset with 8 = ID [LONG] + Chunk Size [LONG] 
				switch(aiffheader.file_id)
				{
					case 0x434F4D4D: 													// 'COMM'
						offset += 8;													// Increase offset with 8 = NumChannels [WORD] + NumSampleFrames [LONG] + SampleSize [WORD]
						fseek(file_handle, offset, SEEK_SET); 
						fread(&samplerate_exponent, 2, 1, file_handle);					// Read SampleRate Exponent
						samplerate_exponent=samplerate_exponent - 16383;				// Clear Exponent Bias (-16383)
						fread(&samplerate_fraction, 2, 1, file_handle);					// Read SampleRate Fraction (first 16-Bits only)
						samplerate_fraction = samplerate_fraction>>(15-samplerate_exponent);	// Convert binary fraction to decimal fraction
						sound->period = (APOLLO_PAL_CLOCK/samplerate_fraction);			// Calculate Period from SampleRate
						offset += 10;													// Increase offset with 10 BYTE (80-bits) SampleRate Bits	
						break;

					case 0x53534E44: 													// 'SSND'
						fread(&sample_offset, sizeof(uint32_t), 1, file_handle); 		
						fseek(file_handle, aiffheader.chunk_size - 8 + sample_offset, SEEK_SET);
						offset +=4;
						sound->size = aiffheader.chunk_size - 8 - sample_offset;
						offset +=4 + sample_offset;						
						scanning = false;
						break;

					default:
						offset += aiffheader.chunk_size;								// Skip chunk
						if(offset >= file_size) return APOLLO_SOUND_ENDOFFILE;			// Reached end of file without finding SSND chunk
						break;
				}
			}
			break;

		case APOLLO_WAV_FORMAT:							
			fseek(file_handle, 0, SEEK_SET);				
			fread(&wavheader, sizeof(struct WAVHeader), 1, file_handle); 				// Read first Chunk Header
			offset += sizeof(struct WAVHeader);											// Increase offset with 44 Byte Header size
			if( wavheader.file_id != 0x52494646) return APOLLO_SOUND_NOHEADER;			// 'RIFF'
			sound->size = ApolloSwapLong(wavheader.file_size) - 36;						// Set size to filesize minus 44 Byte WAV Header - 8 Byte (file_id[4] + filesize[4])
			if( wavheader.wave_id != 0x57415645) return APOLLO_SOUND_NOHEADER;			// 'WAVE'
			if( wavheader.fmt_id != 0x666d7420) return APOLLO_SOUND_NOHEADER;			// 'fmt '
			if( ApolloSwapWord(wavheader.audio_format) != 1) return APOLLO_SOUND_COMPRERR;				// PCM only supported
			if( ApolloSwapWord(wavheader.num_channels) == 2) sound->stereo = true; else sound->stereo = false;
			sound->period = (APOLLO_PAL_CLOCK / ApolloSwapLong(wavheader.sample_rate));	// Calculate Period from SampleRate
			sound->datarate = ApolloSwapLong(wavheader.byte_rate);
			sound->bitspersample = ApolloSwapWord(wavheader.bits_per_sample);
			break;

		default:
			sound->size = file_size - offset;
			break;
	}
	
	fseek(file_handle, offset, SEEK_SET);						// goto offset in file	

	sound->buffer = (uint8_t*)AllocVec(file_size+31, MEMF_ANY);	// allocate buffer memory with extra 31 bytes for alignment 
    if (!sound->buffer)
    {
    	ADX(ApolloDebugPutStr( "ApolloLoadSound: Buffer memory allocation ERROR\n");)
		return APOLLO_SOUND_MEMERROR;
    } else {
		ADX(ApolloDebugPutStr( "ApolloLoadSound: Buffer memory allocated\n");)
    }

	buffer_aligned = (uint8_t*)(((uint32_t)(sound->buffer+31) & ~31));	// align buffer to 32-byte boundary

	ADX(ApolloDebugPutStr("ApolloLoadSound: Reading file -> ");)

	file_read = fread(buffer_aligned, 1, sound->size, file_handle);	// Read file into aligned buffer	
    if(file_read != sound->size)
    {
	   	ADX(ApolloDebugPutStr( "ERROR: Cannot load file\n");)
		FreeVec(sound->buffer);					
	    return APOLLO_SOUND_LOADERROR;
    } else {
	   	ADX(ApolloDebugPutStr( "SUCCESS: File loaded\n");)
    }

    ADX(ApolloDebugPutStr("ApolloLoadSound: Closing file -> ");)

    if (fclose(file_handle) == EOF)
    {
	   	ADX(ApolloDebugPutStr( "ERROR: Cannot close file\n");)
		FreeVec(sound->buffer);
		return APOLLO_SOUND_CLOSEERR;					
    } else {
	   	ADX(ApolloDebugPutStr( "SUCCESS: File closed\n");)
    }

	sound->position = buffer_aligned-sound->buffer;				// Report back position of aligned buffer within allocated buffer

	ADX(sprintf(ApolloDebugMessage, "ApolloLoadSound: %s | Filesize: %d | Format: %d | Size = %8d Bytes | SampleRate = %d | Period = %d | Position = %d | Offset = %d\n",
		 sound->filename, file_size, sound->format, sound->size, samplerate_fraction, sound->period, sound->position, offset);)
	ADX(ApolloDebugPutStr(ApolloDebugMessage);)

	return APOLLO_SOUND_OK;
}

uint8_t ApolloPlaySound( struct ApolloSound *sound)
{
	bool channelfree;
	uint8_t channel;
	
	for (channel=0; channel<16; channel++)
	{
		channelfree = ( ( (channel < 4) && ( (*((volatile uint16_t*)0xDFF002) & (1<<channel)) == 0) ) || ( (channel >=4) && (*((volatile uint16_t*)0xDFF202) & (1<<(channel-4))) == 0 ) );
		ADX(sprintf(ApolloDebugMessage, "ApolloPlaySound: Channel = %d | DMA Channel Free = %s\n", channel, channelfree? "YES":"NO");)
		ADX(ApolloDebugPutStr(ApolloDebugMessage);)
		if (channelfree)  break;
	}
	if(channel==16)
	{
		return APOLLO_SOUND_NOCHANNEL;
	} else {
		sound->channel = channel;
	}
	
	AD(sprintf(ApolloDebugMessage, "ApolloPlaySound: File=%-25s | Size=%8d | Cache=%12d | Channel=%02d | Vol-L = %3d | Vol-R = %3d | Loop = %d | Fadein = %d | Period = %3d |\n",
		sound->filename, sound->size, sound->position, sound->channel, sound->volume_left, sound->volume_right, sound->loop, sound->fadein, sound->period);)
	AD(ApolloDebugPutStr(ApolloDebugMessage);)


	*((volatile uint32_t*)(0xDFF400 + (sound->channel * 0x10))) = (uint32_t)(sound->buffer+sound->position);  	// Set Channel Pointer
	*((volatile uint32_t*)(0xDFF404 + (sound->channel * 0x10))) = (uint32_t)(sound->size/8);					// Set Channel Music length (in pairs of stereo sample = 2 * 2 * 16-bit = 64-bit chunksize = filesize in bytes / 8)
	*((volatile uint16_t*)(0xDFF408 + (sound->channel * 0x10))) = 0;                							// Set Channel Volume to Zero 

	if (sound->loop)
	{
		*((volatile uint16_t*)(0xDFF40A + (sound->channel * 0x10))) = (uint16_t)0x0005;            				// %0101 = $05 - Sample 16bit (bit0 = 1) / OneShot Disabled (bit1 = 0) / Stereo Enabled (bit2 = 1)
	} else {
		*((volatile uint16_t*)(0xDFF40A + (sound->channel * 0x10))) = (uint16_t)0x0007;            				// %0111 = $07 - Sample 16bit (bit0 = 1) / OneShot Enabled (bit1 = 1) / Stereo Enabled (bit2 = 1)
	}
	*((volatile uint16_t*)(0xDFF40C + (sound->channel * 0x10))) = (uint16_t)sound->period;						// PERIOD= 3640000 * (1/44100 Hz)
	if (sound->channel < 4)
	{ 
		*((volatile uint16_t*)0xDFF096) = (uint16_t)(0x8000) + (1<<sound->channel);                				// DMACON Write  = enable DMA and enable DMA for specific channel AUD0-3 (start current stream)    
	} else {
		*((volatile uint16_t*)0xDFF296) = (uint16_t)(0x8000) + (1<<(sound->channel-4));            				// DMACON2 Write = enable DMA and enable DMA for specific channel AUD4-15 (start current stream)       
	}

	if(sound->fadein)
	{
		ApolloFadeInSound(sound);
	} else {
		ApolloVolumeSound(sound);												
	}
	return APOLLO_SOUND_OK;
}

void ApolloStopSound(struct ApolloSound *sound)
{
	ADX(sprintf(ApolloDebugMessage, "ApolloStopSound: Stop Sound: %s on channel %d . . . ", sound->filename, sound->channel);)
	ADX(ApolloDebugPutStr(ApolloDebugMessage);)

	if (sound->channel < 4)
	{ 
		*((volatile uint16_t*)0xDFF096) = (uint16_t)(0x0000) + (1<<sound->channel);                // DMACON = clear AUD0-3 (stop current stream)    
	} else {
		*((volatile uint16_t*)0xDFF296) = (uint16_t)(0x0000) + (1<<(sound->channel-4));            // DMACON2 = clear AUD4-15 (stop current stream)      
	}

	ADX(ApolloDebugPutStr("Done.\n");)
}

void ApolloStartSound(struct ApolloSound *sound)
{
	ADX(ApolloDebugPutDec("ApolloStop: Starting audio on channel", sound->channel);)	

	if (sound->channel < 4)
	{ 
		*((volatile uint16_t*)0xDFF096) = (uint16_t)(0x8000) + (1<<sound->channel);                // DMACON = enable AUD0-3 (start current stream)    
	} else {
		*((volatile uint16_t*)0xDFF296) = (uint16_t)(0x8000) + (1<<(sound->	channel-4));            // DMACON2 = enable AUD4-15 (start current stream)      
	}
}

void ApolloVolumeSound(struct ApolloSound *sound)
{
	int volume = (sound->volume_left << 8) + (sound->volume_right);

	*((volatile uint16_t*)(0xDFF408 + (sound->channel * 0x10))) = (uint16_t)volume;                // Set Channel Volume (0-FF / 0-FF) 
}

void ApolloFadeInSound(struct ApolloSound *sound)
{
	uint16_t target_volume = (sound->volume_left + sound->volume_right)/2;

	for (uint16_t volume = 0; volume < target_volume; volume++)
	{
		sound->volume_left = volume;
		sound->volume_right = volume;
		ApolloVolumeSound(sound);
		ApolloWaitVBL();
	}
}

void ApolloFadeOutSound(struct ApolloSound *sound)
{
	for (uint16_t volume = (sound->volume_left + sound->volume_right)/2; volume > 0; volume--)
	{
		sound->volume_left = volume;
		sound->volume_right = volume;
		ApolloVolumeSound(sound);
		ApolloWaitVBL();
	}
	ApolloStopSound(sound);
}

// Apollo Picture Functions 

uint8_t ApolloAllocPicture( struct ApolloPicture *picture)
{
	uint8_t *buffer_aligned = 0;	// 32-Byte aligned input buffer
	
	if(picture->size == 0) picture->size = (picture->width * picture->height * (picture->depth/8));	// Calculate size if not provided

	picture->buffer = (uint8_t*)AllocVec(picture->size+31, MEMF_ANY);	// allocate buffer memory with extra 31 bytes for alignment 
	if (!picture->buffer)
	{
		ADX(ApolloDebugPutStr( "ApolloAllocPicture: Buffer memory allocation ERROR\n");)
		return APOLLO_PICTURE_MEMERROR;
	} else {
		ADX(ApolloDebugPutStr( "ApolloAllocPicture: Buffer memory allocated\n");)
	}

	buffer_aligned = (uint8_t*)(((uint32_t)(picture->buffer+31) & ~31));	// align buffer to 32-byte boundary

	picture->position = buffer_aligned - picture->buffer;				// Report back position of aligned buffer within allocated buffer

	strcpy(picture->filename, "");	// Clear filename to indicate memory-only picture
	
	ADX(sprintf(ApolloDebugMessage, "ApolloAllocPicture: Picture Allocated: Width=%d | Height=%d | Depth=%d | Size=%d | Position=%d\n",
		 picture->width, picture->height, picture->depth, picture->size, picture->position);)
	ADX(ApolloDebugPutStr(ApolloDebugMessage);)

	return APOLLO_PICTURE_OK;
}

void ApolloFreePicture( struct ApolloPicture *picture)
{
	if (picture->buffer)
	{
		FreeVec(picture->buffer);					
		picture->buffer = NULL;
		ADX(ApolloDebugPutStr( "ApolloFreePicture: Buffer memory freed\n");)
	}
}

uint8_t ApolloLoadPicture(struct ApolloPicture *picture)
{
	static uint8_t *buffer_aligned = 0;						// 32-Byte aligned input buffer	
	static unsigned long file_size = 0;    					// length of file
	static unsigned long file_read = 0;   					// bytes read from file
	static FILE *file_handle = 0;          					// stream of file 
	uint32_t offset = 0, endclipping = 0;

	uint32_t color;

	struct BMPHeader bmpheader;
	struct DDSHeader ddsheader;

	ADX(sprintf(ApolloDebugMessage, "ApolloLoadPicture: Opening File = %s", picture->filename);)
    ADX(ApolloDebugPutStr(ApolloDebugMessage);)

    file_handle = fopen(picture->filename, "rb");
    if (!file_handle)
    {
	   	ADX(ApolloDebugPutStr( "= ERROR\n");)
	    return APOLLO_PICTURE_LOADERR;
    } else {
		ADX(ApolloDebugPutStr( "= SUCCESS\n");)
    }

    fseek(file_handle, 0, SEEK_END);						// goto end of file
    file_size = ftell(file_handle);							// retrieve filesize

	switch(picture->format)
	{
		case APOLLO_DDS_FORMAT:
			fseek(file_handle, 0, SEEK_SET);
			fread(&ddsheader, sizeof(struct DDSHeader), 1, file_handle);
			ADX(ApolloDebugPutHex("ApolloLoad: DDS Header", ddsheader.dwMagic);)
			if( ddsheader.dwMagic != 0x44445320) return APOLLO_PICTURE_NOHEADER;

			picture->width 		= ApolloSwapLong(ddsheader.dwWidth);
			picture->height 	= ApolloSwapLong(ddsheader.dwHeight);
			picture->depth 		= (ApolloSwapLong(ddsheader.dwPitchOrLinearSize)  / picture->width)*8;

			ADX(ApolloDebugPutDec("ApolloLoad: DDS Height", picture->height);)
			ADX(ApolloDebugPutDec("ApolloLoad: DDS Width", picture->width);)
			ADX(ApolloDebugPutDec("ApolloLoad: DDS Depth", picture->depth);)

			ADX(sprintf(ApolloDebugMessage, "ApolloLoad: DDS Width=%d | Height=%d | Depth=%d\n", picture->width, picture->height, picture->depth);)
			ADX(ApolloDebugPutStr(ApolloDebugMessage);)
			offset = sizeof(struct DDSHeader);
			picture->size = file_size-offset;
			break;
		case APOLLO_BMP_FORMAT:
			fseek(file_handle, 0, SEEK_SET);
			fread(&bmpheader, sizeof(struct BMPHeader), 1, file_handle);

			if( bmpheader.type != 0x424D) return APOLLO_PICTURE_NOHEADER;						// Check for 'BM' (0x424D) Marker
			if( (ApolloSwapWord(bmpheader.planes)!=1) ) return APOLLO_PICTURE_PLANEERR;		   	// Check for 1 Color Plane
			if( (ApolloSwapLong(bmpheader.compression)!=0) ) return APOLLO_PICTURE_COMPRERR;	// Only Uncompressed BMP Supported

			picture->size   	= ApolloSwapLong(bmpheader.sizeimage);
			picture->width  	= ApolloSwapLong(bmpheader.width);
			picture->height 	= ApolloSwapLong(bmpheader.height);
			picture->depth  	= ApolloSwapWord(bmpheader.bpp);
			picture->palette	= ApolloSwapLong(bmpheader.palette);	
			offset 				= ApolloSwapLong(bmpheader.offset);
			
			if(picture->size == 0) picture->size = file_size - offset;										// If Image Size is 0, calculate it	
			if( (picture->depth<=8) && (picture->palette==0) ) picture->palette = 1 << picture->depth;		// If Color Indexes is 0, calculate it

			fseek(file_handle, 54, SEEK_SET);
			for(uint16_t colorcounter=0; colorcounter<picture->palette; colorcounter++)						// Set Apollo SAGA Chunky Color Registers
			{
				fread(&color, 1, 4, file_handle);
				*(volatile uint32_t*)APOLLO_SAGA_CHUNKY_COL = (colorcounter<<24) + (((color >> 8) & 0xFF)<<16) + (((color >> 16) & 0xFF)<<8) + ((color >> 24) & 0xFF);
				*(volatile uint32_t*)APOLLO_SAGA_PIPCHK_COL = (colorcounter<<24) + (((color >> 8) & 0xFF)<<16) + (((color >> 16) & 0xFF)<<8) + ((color >> 24) & 0xFF);
				//*(volatile uint32_t*)APOLLO_SAGA_PIPCHK_COL = 0x00FF00FF;   // Enable only when Color00 = Transparent
			}

			ADX(sprintf(ApolloDebugMessage, "ApolloLoadPicture: BMP Width=%d | Height=%d | BPP=%d | ImageSize=%d | Palette=%d\n",
				 picture->width, picture->height, picture->depth, picture->size, picture->palette);)
			ADX(ApolloDebugPutStr(ApolloDebugMessage);)
			break;
		default:
			picture->size = file_size;
			offset = 0;
			break;
	}
	
	fseek(file_handle, offset, SEEK_SET);					// goto offset in file	

	picture->buffer = (uint8_t*)AllocVec(picture->size+31, MEMF_ANY);	// allocate buffer memory with extra 31 bytes for alignment 
    
    if (!picture->buffer)
    {
    	ADX(ApolloDebugPutStr( "ApolloLoad: Buffer memory allocation ERROR\n");)
		return APOLLO_PICTURE_MEMERROR;
    } else {
		ADX(ApolloDebugPutStr( "ApolloLoad: Buffer memory allocated\n");)
    }

	buffer_aligned = (uint8_t*)(((uint32_t)(picture->buffer+31) & ~31));	// align buffer to 32-byte boundary

	ADX(ApolloDebugPutStr("ApolloLoad: Reading file -> ");)

	file_read = fread(buffer_aligned, 1, picture->size, file_handle);	// Read file into aligned buffer	
    if(file_read != picture->size)
    {
	   	ADX(ApolloDebugPutStr( "ERROR: Cannot load file\n");)
		FreeVec(picture->buffer);					
	    return APOLLO_PICTURE_OPENERR;
    } else {
	   	ADX(ApolloDebugPutStr( "SUCCESS: File loaded\n");)
    }

    ADX(ApolloDebugPutStr("ApolloLoad: Closing file -> ");)

    if (fclose(file_handle) == EOF)
    {
	   	ADX(ApolloDebugPutStr( "ERROR: Cannot close file\n");)
		FreeVec(picture->buffer);
		return APOLLO_PICTURE_CLOSEERR;					
    } else {
	   	ADX(ApolloDebugPutStr( "SUCCESS: File closed\n");)
    }

	picture->position = buffer_aligned-picture->buffer;			// Report back position of aligned buffer within allocated buffer

	if(picture->endian)
	{
		ADX(ApolloDebugPutStr( "ApolloLoad: Converting to Big Endian format\n"); )

		uint8_t *buffer_pixel;

		switch(picture->depth)
		{
			case 16:
				for (uint32_t pixel=0; pixel<picture->size;)
				{
					buffer_pixel[0] = buffer_aligned[pixel];
					buffer_aligned[pixel] = buffer_aligned[pixel+1];
					buffer_aligned[pixel+1] = buffer_pixel[0];
					pixel+=2;
				}	
				break;
			
			case 24:
				for (uint32_t pixel=0; pixel<picture->size;)
				{
					buffer_pixel[0] = buffer_aligned[pixel];
					buffer_aligned[pixel] = buffer_aligned[pixel+2];
					buffer_aligned[pixel+2] = buffer_pixel[0];
					pixel+=3;
				}	
				break;

			case 32:
				for (uint32_t pixel=0; pixel<picture->size;)
				{
					buffer_pixel[0] = buffer_aligned[pixel];
					buffer_aligned[pixel] = buffer_aligned[pixel+3];
					buffer_aligned[pixel+3] = buffer_pixel[0];
					buffer_pixel = (uint8_t*)&buffer_aligned[pixel+1];
					buffer_aligned[pixel+1] = buffer_aligned[pixel+2];
					buffer_aligned[pixel+2] = buffer_pixel[0];
					pixel+=4;
				}	
				break;

			default:
				break;
		}
	}

	if(picture->format == APOLLO_BMP_FORMAT)
	{
		if(picture->height>0)
		{	
			ADX(ApolloDebugPutStr( "Flip BMP Bitmap Vertically\n");)						// BMP bitmap is stored bottom to top, so we flip vertically
			uint16_t row_bytes = (picture->width * (picture->depth / 8) + 3) & ~3; 		// Each row is padded to a multiple of 4 bytes
			uint8_t *temp_row = (uint8_t*)AllocVec(row_bytes, MEMF_ANY);
			if(temp_row)
			{
				for(uint32_t y = 0; y < picture->height / 2; y++)
				{
					uint8_t *row_top = buffer_aligned + y * row_bytes;
					uint8_t *row_bottom = buffer_aligned + (picture->height - 1 - y) * row_bytes;
					memcpy(temp_row, row_top, row_bytes);								// Swap top and bottom rows
					memcpy(row_top, row_bottom, row_bytes);
					memcpy(row_bottom, temp_row, row_bytes);
				}
				FreeVec(temp_row);
			} else {
				ADX(ApolloDebugPutStr( " ERROR: Cannot allocate memory for row flipping\n");)
				FreeVec(picture->buffer);
				return APOLLO_PICTURE_MEMERROR;
			}
		} else {
			bmpheader.height = -bmpheader.height;										// Make height positive for further processing						
		}
	}

	ADX(sprintf(ApolloDebugMessage, "ApolloLoad: Picture File Loaded: %s | Filesize = %8d | Format: %d | Size = %8d BYTES | Width = %d | Height = %d | Depth = %d | Palette = %d | Position = %d | Offset = %d\n",
		 picture->filename, file_size, picture->format, picture->size, picture->width, picture->height, picture->depth, picture->palette, picture->position, offset);)
	ADX(ApolloDebugPutStr(ApolloDebugMessage);)

	return APOLLO_PICTURE_OK;
}

uint8_t ApolloShowPicture(struct ApolloPicture *picture)
{
	uint16_t gfx_mode = 0;

	switch(picture->depth)
	{
		case 8 : gfx_mode |= APOLLO_SAGA_8_INDEX; break;
		case 15: gfx_mode |= APOLLO_SAGA_15_1R5G5B5; break;
		case 16: gfx_mode |= APOLLO_SAGA_16_R5G6B5;	break;
		case 24: gfx_mode |= APOLLO_SAGA_24_R8G8B8;	break;
		case 32: gfx_mode |= APOLLO_SAGA_32_A8R8G8B8; break;
		default: return APOLLO_PICTURE_D_ERROR;
	}

	switch(picture->width)
	{
		case 304 : gfx_mode |= APOLLO_SAGA_304_224; break;
		case 320 :
			switch(picture->height)
			{
				case 200: gfx_mode |= APOLLO_SAGA_320_200; break;
				case 240: gfx_mode |= APOLLO_SAGA_320_240; break;
				case 256: gfx_mode |= APOLLO_SAGA_320_256; break;
				default: return APOLLO_PICTURE_H_ERROR;
			}
		case 480: gfx_mode |= APOLLO_SAGA_480_270; break;
		case 640:
			switch(picture->height)
			{
				case 200: gfx_mode |= APOLLO_SAGA_640_200; break;
				case 360: gfx_mode |= APOLLO_SAGA_640_360; break;
				case 400: gfx_mode |= APOLLO_SAGA_640_400; break;
				case 480: gfx_mode |= APOLLO_SAGA_640_480; break;
				case 512: gfx_mode |= APOLLO_SAGA_640_512; break;
				default: return APOLLO_PICTURE_H_ERROR;
			}
			break;
		case 720:
			switch(picture->height)
			{
				case 405: gfx_mode |= APOLLO_SAGA_720_405; break;
				case 576: gfx_mode |= APOLLO_SAGA_720_576; break;
				default: return APOLLO_PICTURE_H_ERROR;
			}
			break;
		case 800: gfx_mode |= APOLLO_SAGA_800_600; break;
		case 848: gfx_mode |= APOLLO_SAGA_848_480; break;
		case 960: gfx_mode |= APOLLO_SAGA_960_540; break;
		case 1024: gfx_mode |= APOLLO_SAGA_1024_768; break;
		case 1280:
			switch(picture->height)
			{
				case 720: gfx_mode |= APOLLO_SAGA_1280_720; break;
				case 800: gfx_mode |= APOLLO_SAGA_1280_800; break;
				case 1024: gfx_mode |= APOLLO_SAGA_1280_1024; break;
				default: return APOLLO_PICTURE_H_ERROR;
			}
			break;

		case 1440: gfx_mode |= APOLLO_SAGA_1440_900; break;
		case 1920: gfx_mode |= APOLLO_SAGA_1920_1080; break;
		default: return APOLLO_PICTURE_W_ERROR;
	}

	ADX(sprintf(ApolloDebugMessage, "ApolloShowPicture: Width=%d | Height=%d | Depth=%d | Modulo=%d | Position=%d | Gfxmode = %x |\n",
		 picture->width, picture->height, picture->depth, picture->modulo, picture->position, gfx_mode);)
	ADX(ApolloDebugPutStr(ApolloDebugMessage);)

	*((volatile uint16_t*)0xDFF1F4) = (uint16_t)(gfx_mode);
	*((volatile uint16_t*)0xDFF1E6) = (int16_t)(picture->modulo); 
	*((volatile uint32_t*)0xDFF1EC) = (uint32_t)(picture->buffer + picture->position);
}

void ApolloShowPiP( struct ApolloPicture *picture)
{
	 *(volatile int16_t*)APOLLO_SAGA_PIP_DMAROWS = picture->width * (picture->depth/8); 
}

void ApolloHidePiP()
{
	*(volatile int16_t*)APOLLO_SAGA_PIP_DMAROWS = 0;
}

void ApolloShowPattern(uint8_t *buffer, uint16_t width, uint16_t height, uint8_t depth)
{
    uint8_t d=depth/8;
	for(uint16_t h=0; h<height; h++)
    {
        for(uint16_t w=0; w<width; w++)
        {
            switch(d)
			{
				case 1:
					buffer[(h*width*d)+(w*d)] = w%0XFF;
					break;
				case 2:
					buffer[(h*width*d)+(w*d)] = w%0XFFFF;
					break;
				case 3:
					buffer[(h*width*d)+(w*d)] = w%0XFFFFFF;
					break;
				case 4:
					buffer[(h*width*d)+(w*d)] = w%0XFFFFFFFF;
					break;
			}
        }
    }
}

void ApolloBackupWBScreen(struct ApolloPicture *picture)
{
	struct Screen *wb_screen;  	  
	wb_screen = LockPubScreen(NULL);
    strcpy(picture->filename, "WorkBench Screen Backup");
	picture->buffer 	= (uint8_t*)wb_screen->RastPort.BitMap->Planes[0];
	picture->width 		= (uint16_t)wb_screen->Width;
	picture->height 	= (uint16_t)wb_screen->Height;
	picture->depth 		= (uint8_t)(8*(wb_screen->RastPort.BitMap->BytesPerRow / wb_screen->Width));
	UnlockPubScreen(NULL, wb_screen);
}

// Apollo CPU Functions

void ApolloWaitVBL()
{
	asm volatile(
		"1: 		btst	#5,0xdff01f			\n"
		"			beq		1b					\n"
		"			move.w 	#0x0020,0xdff09c	\n"
	);
}


// Apollo HID Functions

void ApolloJoypad(ApolloJoypadState *JoypadState)
{
	UWORD * const Joypad_Pointer  = (UWORD*)0xDFF220;
	
	JoypadState->Joypad_Value = *Joypad_Pointer;

	if ((JoypadState->Joypad_Value & 0x8000) == 0x8000) JoypadState->Joypad_LeftX_Delta = 1;
	else if ((JoypadState->Joypad_Value & 0x4000) == 0x4000) JoypadState->Joypad_LeftX_Delta = -1;
	else JoypadState->Joypad_LeftX_Delta = 0;

	if ((JoypadState->Joypad_Value & 0x2000) == 0x2000) JoypadState->Joypad_LeftY_Delta = 1;
	else if ((JoypadState->Joypad_Value & 0x1000) == 0x1000) JoypadState->Joypad_LeftY_Delta = -1;
	else JoypadState->Joypad_LeftY_Delta = 0;

	if ((JoypadState->Joypad_Value & 0x0004) == 0x0004) JoypadState->Joypad_RightX_Delta = 1;
	else if ((JoypadState->Joypad_Value & 0x0008) == 0x0008) JoypadState->Joypad_RightX_Delta = -1;
	else JoypadState->Joypad_RightX_Delta = 0;

	if ((JoypadState->Joypad_Value & 0x0002) == 0x0002) JoypadState->Joypad_RightY_Delta = 1;
	else if ((JoypadState->Joypad_Value & 0x0010) == 0x0010) JoypadState->Joypad_RightY_Delta = -1;
	else JoypadState->Joypad_RightY_Delta = 0;

	if ((JoypadState->Joypad_Value & 0x0400) == 0x0400) JoypadState->Joypad_Start = true; else JoypadState->Joypad_Start = false;
	if ((JoypadState->Joypad_Value & 0x0200) == 0x0200) JoypadState->Joypad_Back = true; else JoypadState->Joypad_Back = false;
			
	if ((JoypadState->Joypad_Value & 0x0100) == 0x0100) JoypadState->Joypad_TR = true; else JoypadState->Joypad_TR = false;
	if ((JoypadState->Joypad_Value & 0x0080) == 0x0080) JoypadState->Joypad_TL = true; else JoypadState->Joypad_TL = false;
	if ((JoypadState->Joypad_Value & 0x0040) == 0x0040) JoypadState->Joypad_BR = true; else JoypadState->Joypad_BR = false;
	if ((JoypadState->Joypad_Value & 0x0020) == 0x0020) JoypadState->Joypad_BL = true; else JoypadState->Joypad_BL = false;
	if ((JoypadState->Joypad_Value & 0x0010) == 0x0010) JoypadState->Joypad_Y = true; else JoypadState->Joypad_Y = false;
	if ((JoypadState->Joypad_Value & 0x0008) == 0x0008) JoypadState->Joypad_X = true; else JoypadState->Joypad_X = false;
	if ((JoypadState->Joypad_Value & 0x0004) == 0x0004) JoypadState->Joypad_B = true; else JoypadState->Joypad_B = false;
	if ((JoypadState->Joypad_Value & 0x0002) == 0x0002) JoypadState->Joypad_A = true; else JoypadState->Joypad_A = false;
	if ((JoypadState->Joypad_Value & 0x0001) == 0x0001) JoypadState->Joypad_Connect = true; else JoypadState->Joypad_Connect = false;
	if (JoypadState->Joypad_Value > 1)
	{
		ADX(sprintf(ApolloDebugMessage, "Joypad State: Value=%04x | LeftX-Delta=%2d | LeftY-Delta=%2d | RightX-Delta=%2d | RightY-Delta=%2d | Start=%d | Back=%d | TR=%d | TL=%d | BR=%d | BL=%d | Y=%d | X=%d | B=%d | A=%d | Connect=%d\n",
			JoypadState->Joypad_Value, JoypadState->Joypad_LeftX_Delta, JoypadState->Joypad_LeftY_Delta, JoypadState->Joypad_RightX_Delta, JoypadState->Joypad_RightY_Delta, JoypadState->Joypad_Start, JoypadState->Joypad_Back, JoypadState->Joypad_TR, JoypadState->Joypad_TL, JoypadState->Joypad_BR, JoypadState->Joypad_BL, JoypadState->Joypad_Y, JoypadState->Joypad_X, JoypadState->Joypad_B, JoypadState->Joypad_A, JoypadState->Joypad_Connect);)
		ADX(ApolloDebugPutStr(ApolloDebugMessage);)
	}
}

void ApolloMouse(ApolloMouseState *MouseState)
{
	UBYTE MouseButtonLeft_Value;	
	UBYTE MouseButtonRight_Value;	
	UBYTE MouseButtonMiddle_Value;

	// Initialize Mouse Buttons
	MouseState->Button_Left = false;
	MouseState->Button_Right = false;
	MouseState->Button_Middle = false;
	
	// Read Mouse Buttons 
	MouseButtonLeft_Value = *((volatile uint8_t*)APOLLO_MOUSE_BUTTON1);
	if ((MouseButtonLeft_Value & 0x40) == 0) MouseState->Button_Left = true;
	MouseButtonRight_Value= *((volatile uint16_t*)APOLLO_MOUSE_BUTTON23);
	if ((MouseButtonRight_Value & 0x400) == 0) MouseState->Button_Right = true;
	MouseState->Button_Middle = *((volatile uint16_t*)APOLLO_MOUSE_WHEEL);

	// Read Mouse Movement	
	MouseState->MouseX_Value 		= *((signed char *)APOLLO_MOUSE_GET_X);
	MouseState->MouseY_Value 		= *((signed char *)APOLLO_MOUSE_GET_Y);
	MouseState->MouseX_Value_Delta 	= MouseState->MouseX_Value - MouseState->MouseX_Value_Old;
	MouseState->MouseY_Value_Delta 	= MouseState->MouseY_Value - MouseState->MouseY_Value_Old;
	MouseState->MouseX_Value_Old 	= MouseState->MouseX_Value;
	MouseState->MouseY_Value_Old 	= MouseState->MouseY_Value;

	// Correct Delta for BYTE overflow
	if (MouseState->MouseX_Value_Delta < -128) MouseState->MouseX_Value_Delta += 256;
	if (MouseState->MouseX_Value_Delta >  128) MouseState->MouseX_Value_Delta -= 256;
	if (MouseState->MouseY_Value_Delta < -128) MouseState->MouseY_Value_Delta += 256;
	if (MouseState->MouseY_Value_Delta >  128) MouseState->MouseY_Value_Delta -= 256;	

	// Check for Mouse Screen Boundaries
	if (MouseState->MouseX_Value_Delta != 0 || MouseState->MouseY_Value_Delta != 0)
	{
		if (MouseState->MouseX_Pointer + MouseState->MouseX_Value_Delta < 1)
		{
			MouseState->MouseX_Pointer = 1;
		} else {
			if (MouseState->MouseX_Pointer + MouseState->MouseX_Value_Delta > MouseState->MouseX_Pointer_Max)
			{
				MouseState->MouseX_Pointer = MouseState->MouseX_Pointer_Max - 1;
			} else {
				MouseState->MouseX_Pointer += MouseState->MouseX_Value_Delta;
			}
		}	

		if (MouseState->MouseY_Pointer + MouseState->MouseY_Value_Delta <1)
		{
			MouseState->MouseY_Pointer = 1;
		} else {
			if (MouseState->MouseY_Pointer + MouseState->MouseY_Value_Delta >  MouseState->MouseY_Pointer_Max)
			{
				MouseState->MouseY_Pointer = MouseState->MouseY_Pointer_Max - 1;
			} else {
				MouseState->MouseY_Pointer += MouseState->MouseY_Value_Delta;	
			}
		}

		// Reposition MousePointer Sprite
		*((volatile uint16_t*)APOLLO_POINTER_SET_X) = (uint16_t)(MouseState->MouseX_Pointer) + 16; 
		*((volatile uint16_t*)APOLLO_POINTER_SET_Y) = (uint16_t)(MouseState->MouseY_Pointer) +  8;  

		sprintf(ApolloDebugMessage,"Mouse_X = X-Pos:%4d X-New:%4d X-Old:%4d X-Delta:%4d | Mouse_Y = Y-Pos:%4d Y-New:%4d Y-Old:%4d Y-Delta:%4d\n",
				MouseState->MouseX_Pointer, MouseState->MouseX_Value, MouseState->MouseX_Value_Old, MouseState->MouseX_Value_Delta,
				MouseState->MouseY_Pointer, MouseState->MouseY_Value, MouseState->MouseY_Value_Old, MouseState->MouseY_Value_Delta);
		ApolloDebugPutStr(ApolloDebugMessage);
	} 

	// Translate Mouse Buttons to Mouse ButtonState
	MouseState->Button_State = 0;

	if( MouseState->Button_Left_Count > 0 ) MouseState->Button_Left_Count--;
	if( MouseState->Button_Right_Count > 0 ) MouseState->Button_Right_Count--;
	if( MouseState->Button_Middle_Count > 0 ) MouseState->Button_Middle_Count--;

	if( MouseState->Button_Left == false && MouseState->Button_Left_Old == true )
	{
		if( MouseState->Button_Left_Count == 0 )
		{
			MouseState->Button_State |= APOLLOMOUSE_LEFTCLICK;
			MouseState->Button_Left_Count = APOLLOMOUSE_DOUBLECLICKCOUNTER;
		}
		else
		{
			MouseState->Button_State |= APOLLOMOUSE_LEFTDOUBLECLICK;
		}
	}

	if( MouseState->Button_Left == true )
	{
		MouseState->Button_State |= APOLLOMOUSE_LEFTDOWN;
	}
	
	if( MouseState->Button_Right == false && MouseState->Button_Right_Old == true )
	{
		if( MouseState->Button_Right_Count == 0 )
		{
			MouseState->Button_State |= APOLLOMOUSE_RIGHTCLICK;
			MouseState->Button_Right_Count = APOLLOMOUSE_DOUBLECLICKCOUNTER;
		}
		else
		{
			MouseState->Button_State |= APOLLOMOUSE_RIGHTDOUBLECLICK;
		}
	}

	if( MouseState->Button_Right == true )
	{
		MouseState->Button_State |= APOLLOMOUSE_RIGHTDOWN;
	}

	if( MouseState->Button_Middle == false && MouseState->Button_Middle_Old == true )
	{
		if( MouseState->Button_Middle_Count == 0 )
		{
			MouseState->Button_State |= APOLLOMOUSE_MIDDLECLICK;
			MouseState->Button_Middle_Count = APOLLOMOUSE_DOUBLECLICKCOUNTER;
		}
		else
		{
			MouseState->Button_State |= APOLLOMOUSE_MIDDLEDOUBLECLICK;
		}
	}

	if( MouseState->Button_Middle == true )
	{
		MouseState->Button_State |= APOLLOMOUSE_MIDDLEDOWN;
	}

	MouseState->Button_Left_Old = MouseState->Button_Left;
	MouseState->Button_Right_Old = MouseState->Button_Right;
	MouseState->Button_Middle_Old = MouseState->Button_Middle;

	return;
}

void ApolloKeyboard(ApolloKeyBoardState *KeyboardState)
{
	UBYTE* const 	Keyboard_Pointer = (UBYTE*)0xBFEC01; 
	UBYTE			Keyboard_Raw, Keyboard_Now;

	Keyboard_Raw = *Keyboard_Pointer;														// retrieve RAW value from register
	Keyboard_Raw = ~Keyboard_Raw;															// not.b
	KeyboardState->Current_Key = (UBYTE) ((Keyboard_Raw>>1) | (Keyboard_Raw<<7));			// ror.b #1

	if ((KeyboardState->Current_Key < 127) && (KeyboardState->Current_Key!=KeyboardState->Previous_Key))
	{
		KeyboardState->Previous_Key = KeyboardState->Current_Key;							// Use only "Low" KB values (0-127)

		sprintf(ApolloDebugMessage,"Keyboard_Now: %d\n", Keyboard_Now);
		ApolloDebugPutStr(ApolloDebugMessage);

	} else {
		KeyboardState->Current_Key = 127;
	}
}

UBYTE ApolloKeyboardToUnicode(UBYTE KeyboardAmiga)
{
	UBYTE KeyboardUnicode;

	switch (KeyboardAmiga)
	{
		case 0x0:	return  96;		// '
		case 0x1:	return  49;		// 1
		case 0x2:	return  50;		// 2
		case 0x3:	return  51;		// 3
		case 0x4:	return  52;		// 4
		case 0x5:	return  53;		// 5
		case 0x6:	return  54;		// 6
		case 0x7:	return  55;		// 7
		case 0x8:	return  56;		// 8
		case 0x9:	return  57;		// 9
		case 0xA:	return  48;		// 0
		case 0xB:	return  45;		// -
		case 0xC:	return  61;		// =
		case 0xD:	return  92;		// Backslash

		case 0xF:	return  48;		// Numpad 0
		case 0x10:	return  81;		// Q
		case 0x11:	return  87;		// W
		case 0x12:	return  69;		// E
		case 0x13:	return  82;		// R
		case 0x14:	return  84;		// T
		case 0x15:	return  89;		// Y
		case 0x16:	return  85;		// U
		case 0x17:	return  73;		// I
		case 0x18:	return  79;		// O
		case 0x19:	return  80;		// P
		case 0x1A:	return  91;		// [
		case 0x1B:	return  93;		// ]

		case 0x1D:	return  49;		// Numpad 1
		case 0x1E:	return  50;		// Numpad 2
		case 0x1F:	return  51;		// Numpad 3

		case 0x20:	return  65;		// A
		case 0x21:	return  83;		// S
		case 0x22:	return  68;		// D
		case 0x23:	return  70;		// F
		case 0x24:	return  71;		// G
		case 0x25:	return  72;		// H
		case 0x26:	return  74;		// J
		case 0x27:	return  75;		// K
		case 0x28:	return  76;		// L
		case 0x29:	return  59;		// ;
		case 0x2A:	return  39;		// '

		case 0x2D:	return  52;		// Numpad 4
		case 0x2E:	return  53;		// Numpad 5
		case 0x2F:	return  54;		// Numpad 6

		case 0x31:	return  90;		// Z
		case 0x32:	return  88;		// X
		case 0x33:	return  67;		// C
		case 0x34:	return  86;		// V
		case 0x35:	return  66;		// B
		case 0x36:	return  78;		// N
		case 0x37:	return  77;		// M
		case 0x38:	return  44;		// ,
		case 0x39:	return  46;		// .
		case 0x3A:	return  47;		// Forwardslash
		
		case 0x3C:	return  46;		// Numpad .
		case 0x3D:	return  55;		// Numpad 7
		case 0x3E:	return  56;		// Numpad 8
		case 0x3F:	return  57;		// Numpad 9

		case 0x40:	return  32;		// Space
		case 0x41:	return   8;		// Backspace
		case 0x42:	return   9;		// Tab
		case 0x43:	return  13;		// Enter (CR)
		case 0x44:	return  13;		// Return (CR)

		case 0x45:	return  27;		// Esc
		case 0x46:	return 127;		// Del

		case 0x4A:	return  45;		// -

		case 0x5A:	return  40;		// (
		case 0x5B:	return  41;		// )
		case 0x5C:	return  47;		// Forwardslash
		case 0x5D:	return  42;		// *
	}

	return 0;
}





