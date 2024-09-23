//***********************************************************
//* Apollo Uncompress Vector Sprite		 				    *
//***********************************************************
//* a0 = s = source pointer to base of dictionary			*
//* a1 = s = index pointer for offset from base 			*
//* a2 = d = destination pointer 							*
//* d3 = w = sprite width 								    *
//* d4 = h = sprite height 								    *
//* d5 = spitch = source pitch in BYTES 					*
//***********************************************************

#ifdef __cplusplus
extern "C"{
#endif 

#include "stdint.h"
#include "stdlib.h"
#include <exec/types.h>
#include "ApolloRegParam.h"

extern _REG void ApolloUncompressVectorLoop( _A0(UWORD* s), _A1(UWORD* i), _A2(UWORD* d), _D3(UWORD w), _D4(UWORD h), _D5(ULONG spitch));

#ifdef __cplusplus
}
#endif
