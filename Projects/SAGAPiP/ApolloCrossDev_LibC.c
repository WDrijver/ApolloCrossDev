// ApolloCrossDev Library
// 21-1-2025

#include "ApolloCrossDev_Library.h"

char ApolloDebugText[120];

extern ULONG ApolloMemoryFree;

void ApolloLoad(const char *filename, uint8_t **buffer, int*position, uint32_t *lenght, uint16_t offset)
{
	static uint8_t *buffer_aligned = 0;						// 64-bit aligned input buffer	
	
	static unsigned long file_size = 0;    					// length of file
	static unsigned long file_read = 0;   					// bytes read from file
	static FILE *file_handle = 0;          					// stream of file 

    #ifdef APOLLO_DEBUG
	DebugPutStr("ApolloLoad: Opening File = ");
	DebugPutStr(filename);
	#endif

    file_handle = fopen(filename, "rb");
    if (!file_handle)
    {
		#ifdef APOLLO_DEBUG
	   	DebugPutStr( "=ERROR\n");
		#endif
	    return;
    } else {
		#ifdef APOLLO_DEBUG
		DebugPutStr( "=SUCCESS\n");
		#endif
    }

    fseek(file_handle, 0, SEEK_END);						// goto end of file
    file_size=ftell(file_handle);							// retrieve filesize

	fseek(file_handle, offset, SEEK_SET);
	file_size-=offset;

	#ifdef APOLLO_DEBUG
    sprintf(ApolloDebugText,"ApolloLoad: Audio Data Size = %d BYTES\n", file_size);
    DebugPutStr(ApolloDebugText);
	#endif

	*buffer = (uint8_t*)AllocVec(file_size+15, MEMF_ANY);
    
    if (!*buffer)
    {
        #ifdef APOLLO_DEBUG
    	DebugPutStr( "Apollo_DEBUGLoad: buffer memory ERROR\n");
		#endif
		return;
    } else {
		#ifdef APOLLO_DEBUG
		DebugPutStr( "ApolloLoad: buffer memory allocated\n");
		#endif
    }

	memset(*buffer, 0, file_size+15);

	buffer_aligned = (uint8_t*)(((uint32_t)(*buffer+15) & 0xFFFFFFF0));

	#ifdef APOLLO_DEBUG
	DebugPutStr("ApolloLoad: Reading file -> ");
	#endif

	file_read = fread(buffer_aligned, 1, file_size, file_handle);
    if(file_read != file_size)
    {
		#ifdef APOLLO_DEBUG
	   	DebugPutStr( "ERROR: cannot load file\n");
		#endif	
	    return;
    } else {
		#ifdef APOLLO_DEBUG
	   	DebugPutStr( "SUCCESS: file loaded\n");
		#endif
    }

	#ifdef APOLLO_DEBUG
    DebugPutStr("ApolloLoad: Closing file -> ");
	#endif

    if (fclose(file_handle) == EOF)
    {
		#ifdef APOLLO_DEBUG
	   	DebugPutStr( "ERROR: cannot close file\n");
		#endif
    } else {
		#ifdef APOLLO_DEBUG
	   	DebugPutStr( "SUCCESS: file closed\n");
		#endif
    }

	*lenght = file_size;
	*position = buffer_aligned-*buffer;

	#ifdef APOLLO_DEBUG
	sprintf(ApolloDebugText, "ApolloLoad: buffer = 0x%x | aligned buffer = 0x%x | position = %d | filesize =%d |\n", *buffer, buffer_aligned, *position, *lenght);
	DebugPutStr(ApolloDebugText);
	#endif
}

bool ApolloPlay(int channel, int volume_left, int volume_right, bool loop, bool fadein, uint8_t *buffer, uint32_t lenght, uint16_t period, int *channel_chosen)
{
	bool channelfree;
	// Choose Channel
	if(channel)
	{
		// Check if channel is free
		if ( ( (channel < 4) && (*((volatile uint16_t*)0xDFF002) & (1<<channel)) ) || ( (channel >=4) && (*((volatile uint16_t*)0xDFF202) & (1<<channel)) ) )
		{
			#ifdef APOLLO_DEBUG
			DebugPutStr("ApolloPlay: ERROR = Requested Channel is BUSY\n");
			#endif
			return false;
		}
	} else {
		for (channel=0; channel<16; channel++)
		{
			channelfree = ( ( (channel < 4) && ( (*((volatile uint16_t*)0xDFF002) & (1<<channel)) == 0) ) || ( (channel >=4) && (*((volatile uint16_t*)0xDFF202) & (1<<channel)) == 0 ) );
			#ifdef APOLLO_DEBUG
			sprintf(ApolloDebugText, "ApolloPlay: channel = %d | DMA Channel Free = %s\n", channel, channelfree? "YES":"NO");
			DebugPutStr(ApolloDebugText);
			#endif
			if (channelfree)  break;
		}
		if(channel==16)
		{
			#ifdef APOLLO_DEBUG
			DebugPutStr("ApolloPlay: ERROR = NO Free Channel Found\n");
			#endif
			return false;
		}
	}

	#ifdef APOLLO_DEBUG
	sprintf(ApolloDebugText, "ApolloPlay: Buffer = 0x%x | Channel = %d | Lenght = %d | Vol-L = %d | Vol-R = %d | Loop = %d | Fadein = %d | Period = %d\n", buffer, channel, lenght, volume_left, volume_right, loop, fadein, period);
	DebugPutStr(ApolloDebugText);
	#endif

	*((volatile uint32_t*)(0xDFF400 + (channel * 0x10))) = (uint32_t)buffer;        	   	// Set Channel Pointer
	*((volatile uint32_t*)(0xDFF404 + (channel * 0x10))) = (uint32_t)((lenght/8)-32);	// Set Channel Music Lenght (in pairs of stereo sample = 2 * 2 * 16-bit = 64-bit chunksize = filesize in bytes / 8)
	
	ApolloVolume(channel, volume_left, volume_right);

	if (loop)
	{
		*((volatile uint16_t*)(0xDFF40A + (channel * 0x10))) = (uint16_t)0x0005;            // %0101 = $05 - Sample 16bit (bit0 = 1) / OneShot Disabled (bit1 = 0) / Stereo Enabled (bit2 = 1)
	} else {
		*((volatile uint16_t*)(0xDFF40A + (channel * 0x10))) = (uint16_t)0x0007;            // %0111 = $07 - Sample 16bit (bit0 = 1) / OneShot Enabled (bit1 = 1) / Stereo Enabled (bit2 = 1)
	}
	*((volatile uint16_t*)(0xDFF40C + (channel * 0x10))) = (uint16_t)period;					// PERIOD=44.1 Khz
	if (channel < 4)
	{ 
		*((volatile uint16_t*)0xDFF096) = (uint16_t)(0x8000) + (1<<channel);                // DMACON = enable DMA and enable DMA for specific channel AUD0-3 (start current stream)    
	} else {
		*((volatile uint16_t*)0xDFF296) = (uint16_t)(0x8000) + (1<<(channel-4));            // DMACON = enable DMA and enable DMA for specific channel AUD4-15 (start current stream)       
	}

	if(fadein)
	{
		ApolloFadeIn(channel, 0x00, (volume_left+volume_right)/2);
	} 
}

void ApolloFadeIn(int channel, int volume_start, int volume_end)
{
	for (;volume_start < volume_end;volume_start++)
	{
		ApolloVolume(channel, volume_start, volume_start);
		ApolloWaitVBL();
	}
}

void ApolloFadeOut(int channel, int volume_start, int volume_end)
{
	for (;volume_start > volume_end;volume_start--)
	{
		ApolloVolume(channel, volume_start, volume_start);
		ApolloWaitVBL();
	}
}

void ApolloStop(int channel)
{
	#ifdef APOLLO_DEBUG
	DebugPutDec("ApolloStop: Stopping audio on channel", channel);
	#endif

	if (channel < 4)
	{ 
		*((volatile uint16_t*)0xDFF096) = (uint16_t)(0x0000) + (1<<channel);                // DMACON = clear AUD0-3 (stop current stream)    
	} else {
		*((volatile uint16_t*)0xDFF296) = (uint16_t)(0x0000) + (1<<(channel-4));            // DMACON2 = clear AUD4-15 (stop current stream)      
	}
}

void ApolloStart(int channel)
{
	#ifdef APOLLO_DEBUG
	DebugPutDec("ApolloStop: Starting audio on channel", channel);
	#endif

	if (channel < 4)
	{ 
		*((volatile uint16_t*)0xDFF096) = (uint16_t)(0x8000) + (1<<channel);                // DMACON = enable AUD0-3 (start current stream)    
	} else {
		*((volatile uint16_t*)0xDFF296) = (uint16_t)(0x8000) + (1<<(channel-4));            // DMACON2 = enable AUD4-15 (start current stream)      
	}
}

void ApolloVolume(int channel, int volume_left, int volume_right)
{
	int volume = (volume_left << 8) + (volume_right);

	#ifdef APOLLO_DEBUG
	DebugPutHex("ApolloPlay: Volume", volume);
	#endif

	*((volatile uint16_t*)(0xDFF408 + (channel * 0x10))) = (uint16_t)volume;                // Set Channel Volume (0-FF / 0-FF) 
}

void ApolloWaitVBL()
{
	asm volatile(
		"1: 		btst	#5,0xdff01f			\n"
		"			beq		1b					\n"
		"			move.w 	#0x0020,0xdff09c	\n"
	);
}



