/***************************************************************************
 * Apollo 24->16 
 * Convert RAW RGB888 to RAW RGB565
 */

#include <stdio.h>
#include <stdint.h>

static unsigned char *src = 0;  /* input buffer.    */
static unsigned char *dst = 0;  /* input buffer.    */
static size_t src_size = 0;     /* length of src file  */
static size_t dst_size = 0;     /* length of src file  */
static size_t bytes_read = 0;
static FILE *f_from = 0;        /* stream of source file. */
static FILE *f_to = 0;        /* stream of source file. */
void *malloc(size_t size);


int main(int argc, char *argv[])
{
	char *file_path_from;	/* path to source file.   */
	char *file_path_to;	/* path to dst file.   */
	int error;

 	int i=0;
 	int j=0;
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned short HI;

	/* read command line arguments */
	if (argc != 3 || !argv[1] || !argv[2] ) {
		fprintf(stderr, "Usage: %s <24bit.data> <16bit.data> >\n", argv[0]);
		return(1);
	}
	file_path_from = argv[1];
	file_path_to = argv[2];

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

	src = (unsigned char*)malloc(src_size);
	if (!src) {
		fprintf(stderr, "Cannot alloc source buffer memory\n");
		return (1);
	}


	dst_size=src_size*2/3;
	dst = (unsigned char*)malloc(dst_size);
	if (!dst) {
		fprintf(stderr, "Cannot alloc dst buffer memory\n");
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


	for(; i<src_size; i+=3){
          R= src[i];
          G= src[i+1];
          B= src[i+2];
          HI= ((R>>3)<<11) | ((G>>2)<<5) | (B>>3);


          dst[j]=(HI>>8) &0xff;
          dst[j+1]=(HI) &0xff;
          j+=2;
        }

        f_to = fopen(file_path_to, "wb+");
        if (!f_to) {
                fprintf(stderr, "Cannot open target file: ");
                perror("");
                return (1);
        }
        /* Write last DST block */
        if (fwrite(dst, 1, dst_size, f_to) == (size_t) EOF) {
                fprintf(stderr, "Error writing to target file: ");
                perror("");
                return (1);
        }
        /* close dst file streams. */
        if (fclose(f_to) == EOF) {
                fprintf(stderr, "Error when closing target file: ");
                perror("");
        }

	

	return (error);
}
