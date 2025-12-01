//*********************************************************
//* Apollo Blit Green Alpha to Background                 *
//*********************************************************
//* a0 = src = source pointer                             *
//* a1 = dst = destination pointer                        *
//* a2 = alpha = alpha pointer                            *
//* d3 = width = blitbox width                            *
//* d4 = height = blitbox height                          *
//* d5 = spitch = source pitch in BYTES                   *
//* d6 = dpitch = destination pitch in BYTES              * 
//* d7 = apitch = alpha pitch in BYTES                    * 
//*********************************************************

#ifdef __cplusplus
extern "C"{
#endif 

#include "stdint.h"
#include "stdlib.h"
#include <exec/types.h>
#include "ApolloRegParam.h"

extern _REG void ApolloBlitGreenAlphaBGLoop( _A0(UWORD* src), _A1(UWORD* dst), _A2(UWORD* alpha), _D3(UWORD width), _D4(UWORD height), _D5(ULONG spitch), _D6(ULONG dpitch), _D7(ULONG apitch));

#ifdef __cplusplus
}
#endif
