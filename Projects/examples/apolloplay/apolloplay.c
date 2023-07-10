/***************************************************************************
 * Apollo Play 
 * play 16bit music
 */

#include <stdio.h>
#include <stdint.h>
#include <proto/exec.h>


static unsigned char *src = 0;  /* input buffer.    */
static size_t src_size = 0;     /* length of src file  */
static size_t bytes_read = 0;
static FILE *f_from = 0;        /* stream of source file. */
void *malloc(size_t size);


int main(int argc, char *argv[])
{
	char *file_path_from;	/* path to source file.   */
	int error;

	

	/* read command line arguments */
	if (argc != 2 || !argv[1] ) {
		fprintf(stderr, "Usage: %s <aiff file path> 2\n", argv[0]);
		return(1);
	}

	file_path_from = argv[1];

	/* open the source and the target files. */
	f_from = fopen(file_path_from, "rb");

	if (!f_from) {
		fprintf(stderr, "Cannot open source file: ");
		perror("");
		return(1);
	}

	fseek(f_from,0,SEEK_END);
	src_size=ftell(f_from);		/* read length of src file */
	rewind(f_from);

	src = (unsigned char*)AllocMem((LONG)src_size,0);
	if (!src) {
		fprintf(stderr, "Cannot alloc AIFF buffer memory\n");
		return (1);
	}
	/* initial source load */
	bytes_read = fread(src, 1, src_size, f_from);

	if(bytes_read != src_size) {
		fprintf(stderr, "Error reading source file: ");
	}
	/* close source file streams. */
	if (fclose(f_from) == EOF) {
		fprintf(stderr, "Error when closing source file: ");
		perror("");
	}


	*((volatile uint32_t*)0xDFF440) = (uint32_t)src+64;		// PTR
	*((volatile uint32_t*)0xDFF444) = (uint32_t)((src_size-64)/8);	// LEN
	*((volatile uint16_t*)0xDFF448) = (uint16_t)0x7F7F;		// VOL
	*((volatile uint16_t*)0xDFF44A) = (uint16_t)0x0004;		// 16bit Stereo=32bit
	*((volatile uint16_t*)0xDFF44C) = (uint16_t)80;		// 44.1 Khz
	*((volatile uint16_t*)0xDFF296) = (uint16_t)0x8001;	// DMAON
        *((volatile uint16_t*)0xDFF096) = (uint16_t)0x8200;	// DMAON
	

	return (error);
}
