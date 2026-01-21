#ifdef __cplusplus
extern "C"{
#endif 

// ApolloCrossDev Library
// 21-1-2025

// Include Basic C Headers
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// Include Amiga/Apollo Defines
#include "ApolloCrossDev_Defines.h"

// Include Basic Amiga Headers
#include "exec/types.h"
#include "clib/exec_protos.h"

// ApolloCrossDev C Functions
void ApolloLoad(const char *filename, uint8_t **buffer, int*position, uint32_t *lenght, uint16_t offset);
bool ApolloPlay(int channel, int volume_left, int volume_right, bool loop, bool fadein, uint8_t *buffer, uint32_t lenght, uint16_t period, int *channel_chosen);
void ApolloStop(int channel);
void ApolloStart(int channel);
void ApolloFadeIn(int channel, int volume_start, int volume_end);
void ApolloFadeOut(int channel, int volume_start, int volume_end);
void ApolloVolume(int channel, int volume_left, int volume_right);
void ApolloWaitVBL();


// ApolloCrossDev Assembler Functions
void ApolloFill  ( _A0(ULONG* dst), _D3(UWORD w), _D4(UWORD h), _D5(ULONG dstmod), _D7(ULONG value) );
void ApolloCopy  ( _A0(ULONG *src), _A1(ULONG *dst), _D3(UWORD width), _D4(UWORD height), _D5(UWORD srcmod), _D6(UWORD dstmod) );
void ApolloCopy32( _A0(ULONG *src), _A1(ULONG *dst), _D3(UWORD width), _D4(UWORD height), _D5(UWORD srcmod), _D6(UWORD dstmod) );


#ifdef __cplusplus
}
#endif
