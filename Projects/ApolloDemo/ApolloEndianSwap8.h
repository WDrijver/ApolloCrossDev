//*********************************************************
//* Apollo Endian Swap (8-Byte chunks)				   	  *
//*********************************************************
//* a0 = s = buffer source pointer 						  *
//* d0 = l = buffer length	 							  *
//*********************************************************

#ifdef __cplusplus
extern "C"{
#endif 

#include "stdint.h"
#include "stdlib.h"
#include <exec/types.h>
#include "ApolloRegParam.h"

extern void ApolloEndianSwap8(_A0(UWORD *s), _D0(ULONG l));

#ifdef __cplusplus
}
#endif
