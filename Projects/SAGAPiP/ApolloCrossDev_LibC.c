// ApolloCrossDev Library
// 21-1-2025

#include "ApolloCrossDev_Library.h"

extern char ApolloDebugMessage[200];

// ApolloSound Functions ######################################

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

	AD(sprintf(ApolloDebugMessage, "ApolloLoadSound: Opening File = %s", sound->filename);)
    AD(ApolloDebugPutStr(ApolloDebugMessage);)

    file_handle = fopen(sound->filename, "rb");
    if (!file_handle)
    {
	   	AD(ApolloDebugPutStr( "= ERROR\n");)
	    return APOLLO_SOUND_LOADERROR;
    } else {
		AD(ApolloDebugPutStr( "= SUCCESS\n");)
    }

    fseek(file_handle, 0, SEEK_END);													// Goto end of file
    file_size = ftell(file_handle);														// Retrieve filesize

	switch(sound->format)
	{
		case APOLLO_AIFF_FORMAT:							
			fseek(file_handle, 0, SEEK_SET);				
			fread(&aiffheader, sizeof(struct AIFFHeader), 1, file_handle); 				// Read first Chunk Header
			offset += 12;																// Increase offset with 8 = ID [LONG] + Chunk Size [LONG] + FORM Type [LONG]											
			ApolloDebugPutHex("ApolloLoadSound: AIFF Header ID", aiffheader.ckID);
			if( aiffheader.ckID != 0x464F524D) return APOLLO_SOUND_NOHEADER;			// 'FORM'
			while(scanning)																
			{
				fseek(file_handle, offset, SEEK_SET);
				fread(&aiffheader, sizeof(struct AIFFHeader), 1, file_handle);			// ID [LONG] + Chunk Size [LONG]
				offset +=8 ;															// Increase offset with 8 = ID [LONG] + Chunk Size [LONG] 
				ApolloDebugPutHex("ApolloLoadSound: AIFF Header ID", aiffheader.ckID);
				switch(aiffheader.ckID)
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
						fread(&sample_offset, sizeof(uint32_t), 1, file_handle); 		//
						fseek(file_handle, aiffheader.ckSize - 8 + sample_offset, SEEK_SET);
						offset +=4;
						sound->size = aiffheader.ckSize - 8 - sample_offset;
						offset += 4 + sample_offset;						
						scanning = false;
						break;

					default:
						offset += aiffheader.ckSize;									// Skip chunk
						if(offset >= file_size) return APOLLO_SOUND_ENDOFFILE;			// Reached end of file without finding SSND chunk
						break;
				}
			}
			break;

		default:
			sound->size = file_size - offset;
			break;
	}
	
	fseek(file_handle, offset, SEEK_SET);						// goto offset in file	

	sound->buffer = (uint8_t*)AllocVec(file_size+31, MEMF_ANY);	// allocate buffer memory with extra 31 bytes for alignment 
    if (!sound->buffer)
    {
    	AD(ApolloDebugPutStr( "ApolloLoadSound: Buffer memory allocation ERROR\n");)
		return APOLLO_SOUND_MEMERROR;
    } else {
		AD(ApolloDebugPutStr( "ApolloLoadSound: Buffer memory allocated\n");)
    }

	buffer_aligned = (uint8_t*)(((uint32_t)(sound->buffer+31) & ~31));	// align buffer to 32-byte boundary

	AD(ApolloDebugPutStr("ApolloLoadSound: Reading file -> ");)

	file_read = fread(buffer_aligned, 1, sound->size, file_handle);	// Read file into aligned buffer	
    if(file_read != sound->size)
    {
	   	AD(ApolloDebugPutStr( "ERROR: Cannot load file\n");)
		FreeVec(sound->buffer);					
	    return APOLLO_SOUND_LOADERROR;
    } else {
	   	AD(ApolloDebugPutStr( "SUCCESS: File loaded\n");)
    }

    AD(ApolloDebugPutStr("ApolloLoadSound: Closing file -> ");)

    if (fclose(file_handle) == EOF)
    {
	   	AD(ApolloDebugPutStr( "ERROR: Cannot close file\n");)
		FreeVec(sound->buffer);
		return APOLLO_SOUND_CLOSEERR;					
    } else {
	   	AD(ApolloDebugPutStr( "SUCCESS: File closed\n");)
    }

	sound->position = buffer_aligned-sound->buffer;				// Report back position of aligned buffer within allocated buffer

	AD(sprintf(ApolloDebugMessage, "ApolloLoadSound: Sound File Loaded: %s | Filesize: %d | Format: %d | Length = %d BYTES | SampleRate = %d | Period = %d | Position = %d | Offset = %d\n",
		 sound->filename, file_size, sound->format, sound->size, samplerate_fraction, sound->period, sound->position, offset);)
	AD(ApolloDebugPutStr(ApolloDebugMessage);)

	return APOLLO_SOUND_OK;
}

uint8_t ApolloPlaySound( struct ApolloSound *sound)
{
	bool channelfree;
	uint8_t channel;
	
	for (channel=0; channel<16; channel++)
	{
		channelfree = ( ( (channel < 4) && ( (*((volatile uint16_t*)0xDFF002) & (1<<channel)) == 0) ) || ( (channel >=4) && (*((volatile uint16_t*)0xDFF202) & (1<<channel)) == 0 ) );
		AD(sprintf(ApolloDebugMessage, "ApolloPlay: Channel = %d | DMA Channel Free = %s\n", channel, channelfree? "YES":"NO");)
		AD(ApolloDebugPutStr(ApolloDebugMessage);)
		if (channelfree)  break;
	}
	if(channel==16) return APOLLO_SOUND_NOCHANNEL;

	AD(sprintf(ApolloDebugMessage, "ApolloPlay: Channel = %d | length = %d | Vol-L = %d | Vol-R = %d | Loop = %d | Fadein = %d | Period = %d\n",
		 channel, sound->size, sound->volume_left, sound->volume_right, sound->loop, sound->fadein, sound->period);)
	AD(ApolloDebugPutStr(ApolloDebugMessage);)

	*((volatile uint32_t*)(0xDFF400 + (channel * 0x10))) = (uint32_t)(sound->buffer+sound->position);  	// Set Channel Pointer
	*((volatile uint32_t*)(0xDFF404 + (channel * 0x10))) = (uint32_t)(sound->size/8);					// Set Channel Music length (in pairs of stereo sample = 2 * 2 * 16-bit = 64-bit chunksize = filesize in bytes / 8)
	*((volatile uint16_t*)(0xDFF408 + (sound->channel * 0x10))) = 0;                					// Set Channel Volume to Zero 

	if (sound->loop)
	{
		*((volatile uint16_t*)(0xDFF40A + (channel * 0x10))) = (uint16_t)0x0005;            // %0101 = $05 - Sample 16bit (bit0 = 1) / OneShot Disabled (bit1 = 0) / Stereo Enabled (bit2 = 1)
	} else {
		*((volatile uint16_t*)(0xDFF40A + (channel * 0x10))) = (uint16_t)0x0007;            // %0111 = $07 - Sample 16bit (bit0 = 1) / OneShot Enabled (bit1 = 1) / Stereo Enabled (bit2 = 1)
	}
	*((volatile uint16_t*)(0xDFF40C + (channel * 0x10))) = (uint16_t)sound->period;			// PERIOD= 3640000 * (1/44100 Hz)
	if (channel < 4)
	{ 
		*((volatile uint16_t*)0xDFF096) = (uint16_t)(0x8000) + (1<<channel);                // DMACON = enable DMA and enable DMA for specific channel AUD0-3 (start current stream)    
	} else {
		*((volatile uint16_t*)0xDFF296) = (uint16_t)(0x8000) + (1<<(channel-4));            // DMACON = enable DMA and enable DMA for specific channel AUD4-15 (start current stream)       
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
	AD(ApolloDebugPutDec("ApolloStop: Stopping audio on channel", sound->channel);)

	if (sound->channel < 4)
	{ 
		*((volatile uint16_t*)0xDFF096) = (uint16_t)(0x0000) + (1<<sound->channel);                // DMACON = clear AUD0-3 (stop current stream)    
	} else {
		*((volatile uint16_t*)0xDFF296) = (uint16_t)(0x0000) + (1<<(sound->channel-4));            // DMACON2 = clear AUD4-15 (stop current stream)      
	}
}

void ApolloStartSound(struct ApolloSound *sound)
{
	AD(ApolloDebugPutDec("ApolloStop: Starting audio on channel", sound->channel);)	

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


// ApolloPicture Functions ######################################

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

	AD(sprintf(ApolloDebugMessage, "ApolloLoad: Opening File = %s", picture->filename);)
    AD(ApolloDebugPutStr(ApolloDebugMessage);)

    file_handle = fopen(picture->filename, "rb");
    if (!file_handle)
    {
	   	AD(ApolloDebugPutStr( "= ERROR\n");)
	    return APOLLO_PICTURE_LOADERR;
    } else {
		AD(ApolloDebugPutStr( "= SUCCESS\n");)
    }

    fseek(file_handle, 0, SEEK_END);						// goto end of file
    file_size = ftell(file_handle);							// retrieve filesize

	switch(picture->format)
	{
		case APOLLO_DDS_FORMAT:
			fseek(file_handle, 0, SEEK_SET);
			fread(&ddsheader, sizeof(struct DDSHeader), 1, file_handle);
			ApolloDebugPutHex("ApolloLoad: DDS Header", ddsheader.dwMagic);
			if( ddsheader.dwMagic != 0x44445320) return APOLLO_PICTURE_NOHEADER;

			picture->width 		= ApolloSwapLong(ddsheader.dwWidth);
			picture->height 	= ApolloSwapLong(ddsheader.dwHeight);
			picture->depth 		= (ApolloSwapLong(ddsheader.dwPitchOrLinearSize)  / picture->width)*8;

			ApolloDebugPutDec("ApolloLoad: DDS Height", picture->height);
			ApolloDebugPutDec("ApolloLoad: DDS Width", picture->width);
			ApolloDebugPutDec("ApolloLoad: DDS Depth", picture->depth);

			sprintf(ApolloDebugMessage, "ApolloLoad: DDS Width=%d | Height=%d | Depth=%d\n", picture->width, picture->height, picture->depth);
			ApolloDebugPutStr(ApolloDebugMessage);
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

				*(volatile uint32_t*)APOLLO_SAGA_PIPCHK_COL = 0x00FF00FF;
			}

			sprintf(ApolloDebugMessage, "ApolloLoad: BMP Width=%d | Height=%d | BPP=%d | ImageSize=%d | Palette=%d\n",
				 bmpheader.width, bmpheader.height, bmpheader.bpp, bmpheader.sizeimage, bmpheader.palette);
			ApolloDebugPutStr(ApolloDebugMessage);
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
    	AD(ApolloDebugPutStr( "ApolloLoad: Buffer memory allocation ERROR\n");)
		return APOLLO_PICTURE_MEMERROR;
    } else {
		AD(ApolloDebugPutStr( "ApolloLoad: Buffer memory allocated\n");)
    }

	buffer_aligned = (uint8_t*)(((uint32_t)(picture->buffer+31) & ~31));	// align buffer to 32-byte boundary

	AD(ApolloDebugPutStr("ApolloLoad: Reading file -> ");)

	file_read = fread(buffer_aligned, 1, picture->size, file_handle);	// Read file into aligned buffer	
    if(file_read != picture->size)
    {
	   	AD(ApolloDebugPutStr( "ERROR: Cannot load file\n");)
		FreeVec(picture->buffer);					
	    return APOLLO_PICTURE_OPENERR;
    } else {
	   	AD(ApolloDebugPutStr( "SUCCESS: File loaded\n");)
    }

    AD(ApolloDebugPutStr("ApolloLoad: Closing file -> ");)

    if (fclose(file_handle) == EOF)
    {
	   	AD(ApolloDebugPutStr( "ERROR: Cannot close file\n");)
		FreeVec(picture->buffer);
		return APOLLO_PICTURE_CLOSEERR;					
    } else {
	   	AD(ApolloDebugPutStr( "SUCCESS: File closed\n");)
    }

	picture->position = buffer_aligned-picture->buffer;			// Report back position of aligned buffer within allocated buffer

	if(picture->endian)
	{
		AD( ApolloDebugPutStr( "ApolloLoad: Converting to Big Endian format\n"); )

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
			AD(ApolloDebugPutStr( "Flip BMP Bitmap Vertically\n");)						// BMP bitmap is stored bottom to top, so we flip vertically
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
				AD(ApolloDebugPutStr( " ERROR: Cannot allocate memory for row flipping\n");)
				FreeVec(picture->buffer);
				return APOLLO_PICTURE_MEMERROR;
			}
		} else {
			bmpheader.height = -bmpheader.height;										// Make height positive for further processing						
		}
	}

	sprintf(ApolloDebugMessage, "ApolloLoad: Picture File Loaded: %s | Filesize = %d | Format: %d | Size = %d BYTES | Width = %d | Height = %d | Depth = %d | Palette = %d | Position = %d | Offset = %d\n",
		 picture->filename, file_size, picture->format, picture->size, picture->width, picture->height, picture->depth, picture->palette, picture->position, offset);
	ApolloDebugPutStr(ApolloDebugMessage);

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

	AD(sprintf(ApolloDebugMessage, "ApolloShow: Gfxmode = %x | Modulo = % d | Position = %x |\n", gfx_mode, picture->modulo, (uint32_t)(picture->position));)
	AD(ApolloDebugPutStr(ApolloDebugMessage);)

	*((volatile uint16_t*)0xDFF1F4) = (uint16_t)(gfx_mode);
	*((volatile uint16_t*)0xDFF1E6) = (int16_t)(picture->modulo); 
	*((volatile uint32_t*)0xDFF1EC) = (uint32_t)(picture->buffer + picture->position);
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

// Apollo CPU Functions ######################################

void ApolloWaitVBL()
{
	asm volatile(
		"1: 		btst	#5,0xdff01f			\n"
		"			beq		1b					\n"
		"			move.w 	#0x0020,0xdff09c	\n"
	);
}







