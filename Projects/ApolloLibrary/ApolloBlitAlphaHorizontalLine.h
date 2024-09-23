//*********************************************************
//* Apollo Blit Alpha Horizontal Line                     *
//*********************************************************
//* a0 = s = source pointer 						      *
//* a1 = d = destination pointer 						  *
//* d3 = w = line width 								  *
//* d6 = ac = alpha color (R8G8B8 24-bit)				  * 
//* d7 = al = alpha level (0-255) 						  *
//*********************************************************

#ifdef __cplusplus
extern "C"{
#endif 

#include "stdint.h"
#include "stdlib.h"
#include <exec/types.h>
#include "ApolloRegParam.h"

extern _REG void ApolloBlitAlphaHorizontalLineLoop( _A0(UWORD* s), _A1(UWORD* d), _D3(UWORD w), _D6(ULONG ac), _D7(UWORD al));

#ifdef __cplusplus
}
#endif
