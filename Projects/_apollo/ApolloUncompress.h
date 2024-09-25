//***********************************************************
//* Apollo Uncompress Sprite		 						*
//***********************************************************
//* a0 = s = source pointer 								*
//* a1 = d = destination pointer 							*
//* d3 = w = blitbox width 								    *
//* d4 = h = blitbox height 								*
//* d5 = spitch = source pitch in BYTES 					*
//* $f81f = transparency color to replace $07C0 (green) 	* 
//***********************************************************

#ifdef __cplusplus
extern "C"{
#endif 

#include "stdint.h"
#include "stdlib.h"
#include <exec/types.h>
#include "ApolloRegParam.h"

extern _REG void ApolloUncompressLoop( _A0(UWORD* s), _A1(UWORD* d), _D3(UWORD w), _D4(UWORD h), _D5(ULONG spitch) );

#ifdef __cplusplus
}
#endif
