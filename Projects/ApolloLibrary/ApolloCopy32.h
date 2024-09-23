//***********************************************************
//* Apollo Copy (32-Byte aligned width mandatory) 		    *
//***********************************************************
//* a0 = s = source pointer									*	
//* a1 = d = destination pointer							*
//* d3 = w = blitbox width									*
//* d4 = h = blitbox height									*
//* d5 = spitch = source pitch in WORDS						*
//* d6 = dpitch = destination pitch in WORDS				*
//***********************************************************	

#ifdef __cplusplus
extern "C"{
#endif 

#include "stdint.h"
#include "stdlib.h"
#include <exec/types.h>
#include "ApolloRegParam.h"

extern _REG void ApolloCopy32Loop( _A0(UWORD *s), _A1(UWORD *d), _D3(UWORD width), _D4(UWORD height), _D5(UWORD spitch), _D6(UWORD dpitch) );

#ifdef __cplusplus
}
#endif
