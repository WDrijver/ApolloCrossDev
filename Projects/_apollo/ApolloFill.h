//***************************************************
//* Apollo Fill                                     *
//***************************************************
//* a0 = d = destination pointer                    *
//* d3 = w = blitbox width                          *
//* d4 = h = blitbox height                         *
//* d5 = dpitch = destination pitch in BYTES        *
//* d7 = fill color (R5G6B5)                        *
//***************************************************

#ifdef __cplusplus
extern "C"{
#endif 

#include "stdint.h"
#include "stdlib.h"
#include <exec/types.h>
#include "ApolloRegParam.h"

extern _REG void ApolloFillLoop( _A0(UWORD* d), _D3(UWORD w), _D4(UWORD h), _D5(ULONG dpitch), _D7(UWORD fc) );

#ifdef __cplusplus
}
#endif
