#ifdef __cplusplus
extern "C"{
#endif 

// ApolloCrossDev Library
// 21-1-2025

// Include Apollo Headers
#include "ApolloCrossDev_Base.h"
#include "ApolloCrossDev_Debug.h"

// Global Variables
char ApolloDebugMessage[200];

// ApolloCrossDev C Functions ######################################

struct ApolloSound
{
    // Input Values
    char        *filename;
    uint8_t     format;
    bool        loop;
    bool        fadein;
    bool        fadeout;
    uint16_t    volume_left;
    uint16_t    volume_right;
    // Output Values
    uint8_t     *buffer;
    uint8_t     position;
    uint32_t    size;
    uint16_t    period;
    uint8_t     channel;
};
uint8_t ApolloLoadSound(struct ApolloSound *sound);

struct ApolloPicture
{
    // Input Values
    char        *filename;
    uint8_t     format;
    bool        endian;
    // Return Values
    uint8_t     *buffer;
    uint8_t     position;
    uint32_t    size;
    uint16_t    width;
    uint16_t    height;
    uint8_t     depth;
    uint32_t    palette;
    // Display Values
    int16_t     modulo;
};
uint8_t ApolloLoadPicture(struct ApolloPicture *picture);

void ApolloCacheFile(const char *filename, uint8_t **cache_base, uint32_t *cache_offset, uint16_t file_offset);

// Audio Operations: Audio Play, Start, Stop, Fade and Volume
uint8_t ApolloPlaySound(struct ApolloSound *sound);

void ApolloStopSound(struct ApolloSound *sound);
void ApolloStartSound(struct ApolloSound *sound);

void ApolloFadeInSound(struct ApolloSound *sound);
void ApolloFadeOutSound(struct ApolloSound *sound);

void ApolloVolumeSound(struct ApolloSound *sound);

// CPU Operations: Wait for Vertical Blank
void ApolloWaitVBL();

// Graphic Operations: Picture Show
uint8_t ApolloShowPicture(struct ApolloPicture *picture);
void ApolloShowPattern(uint8_t *buffer, uint16_t width, uint16_t height, uint8_t depth);

// ApolloCrossDev Assembler Functions ######################################

// ApolloFill: fill a block of memory with a 32-bit value (32-bit alignment not required) 
void ApolloFill( _A0(UBYTE* dst), _D3(uint16_t w), _D4(uint16_t h), _D5(uint16_t d), _D6(uint32_t dstmod), _D7(uint32_t value) );

// ApolloCopy/ApolloCopy32: copy a block of memory from source to destination (ApolloCopy 32 requires 32-bit alignment) 
void ApolloCopy( _A0(uint32_t *src), _A1(uint32_t *dst), _D3(uint16_t width), _D4(uint16_t height), _D5(uint16_t srcmod), _D6(uint16_t dstmod) );
void ApolloCopy32( _A0(uint32_t *src), _A1(uint32_t *dst), _D3(uint16_t width), _D4(uint16_t height), _D5(uint16_t srcmod), _D6(uint16_t dstmod) );

// ApolloCPUDelay: Wait fox <WaitTime> milliseconds
void ApolloCPUDelay( _D0(uint32_t WaitTime));

// ApolloCPUTick: Returns Apollo CPU Counter
uint32_t ApolloCPUTick();

// ApolloCPUTime:  
uint32_t ApolloCPUTime();

// Graphic Operations: Endian Swap Functions
void ApolloEndianSwapWordBuffer(_A0(UBYTE *s), _D0(uint32_t l));
void ApolloEndianSwapLongBuffer(_A0(UBYTE *s), _D0(uint32_t l));

uint16_t ApolloSwapWord( _D0(uint16_t SwapWord));
uint32_t ApolloSwapLong( _D0(uint32_t SwapLong));
uint64_t ApolloSwapOcta( _D0(uint64_t SwapOcta));

// 3rd Party Functions ######################################

double makeDoubleFromExtended(const unsigned char x[10]);

// Apollo Structures ######################################

struct BMPHeader
{
    uint16_t	type;
    uint32_t	size;
    uint16_t	reserved1;
    uint16_t	reserved2;
    uint32_t	offset;

    uint32_t	headersize;
    int32_t		width;
    int32_t		height;
    uint16_t	planes;
    uint16_t	bpp;
    uint32_t	compression;
    uint32_t	sizeimage;
    int32_t		hppm;
    int32_t		vppm;
    uint32_t	palette;
    uint32_t	colorimportant;
};

struct DDSHeader
{
    uint32_t    dwMagic;
    uint32_t    dwSize;
    uint32_t    dwFlags;
    uint32_t    dwHeight;
    uint32_t    dwWidth;
    uint32_t    dwPitchOrLinearSize;
    uint32_t    dwDepth;
    uint32_t    dwMipMapCount;
    uint32_t    dwReserved1[11];

    // DDPIXELFORMAT
    uint32_t    ddpfSize;
    uint32_t    ddpfFlags;
    uint32_t    ddpfFourCC;
    uint32_t    ddpfRGBBitCount;
    uint32_t    ddpfRBitMask;
    uint32_t    ddpfGBitMask;
    uint32_t    ddpfBBitMask;
    uint32_t    ddpfABitMask;

    // DDCAPS2
    uint32_t    ddsCaps1;
    uint32_t    ddsCaps2;
    uint32_t    ddsCapsReserved[2];
    uint32_t    dwReserved2;
};

struct AIFFHeader
{
    uint32_t    ckID;
    uint32_t    ckSize;
};






#ifdef __cplusplus
}
#endif
