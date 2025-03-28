//*********************************************************
//* Apollo Blit Alpha with Color Key and Transparency 	*
//*********************************************************
//* a0 = s = source pointer *
//* a1 = d = destination pointer *
//* d3 = w = blitbox width *
//* d4 = h = blitbox height *
//* d5 = spitch = source pitch in BYTES *
//* d6 = dpitch = destination pitch in BYTES * 
//* d7 = al = alpha level (0-255) *
//* a2 = ac = alpha color (R5G6B5) *

#ifdef __cplusplus
extern "C"{
#endif 

#include "stdint.h"
#include "stdlib.h"
#include <exec/types.h>
#include "ApolloRegParam.h"

extern _REG void ApolloBlitAlphaKeyTransLoop( _A0(UWORD* s), _A1(UWORD* d), _D3(UWORD w), _D4(UWORD h), _D5(ULONG spitch), _D6(ULONG dpitch), _D7(UWORD al), _A2(UWORD ac));

#ifdef __cplusplus
}
#endif
