// hand crafted for AROS

#include <aros/cpu.h>
#if AROS_BIG_ENDIAN
#define WORDS_BIGENDIAN
#endif

#define HAVE_INTTYPES_H
#define HAVE_STDINT_H
#define HAVE_LIMITS_H
#define HAVE_STDBOOL_H
#define HAVE_STRING_H
#define HAVE_STRINGS_H
#define HAVE_MEMORY_H

#define SIZEOF_SIZE_T 4

//#define HAVE_SMALL
#define HAVE_DECODER_LZMA1
#define HAVE_DECODER_LZMA2
#define HAVE_DECODER_X86
#define HAVE_DECODER_POWERPC
#define HAVE_DECODER_IA64
#define HAVE_DECODER_ARM
#define HAVE_DECODER_ARMTHUMB
#define HAVE_DECODER_SPARC
#define HAVE_DECODER_DELTA
#define HAVE_CHECK_CRC32
#define HAVE_CHECK_CRC64
#define HAVE_CHECK_SHA256
