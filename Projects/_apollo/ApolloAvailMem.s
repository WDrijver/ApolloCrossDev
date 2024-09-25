*************************************************************
* Apollo Get Available Mememory - AvailMEM()                *
*************************************************************

	XDEF _ApolloAvailMem

	INCLUDE "exec/types.i"

	CNOP 0,4

_ApolloAvailMem:

	movem.l a6,-(sp)				* Save all registers to Stack

	movea.l $4.w,a6					* Load Exec base address in a6

    jsr _LVOAvailMem(a6)            * Retrieve Available Memory in d0
    
	movem.l (sp)+,a6				* Save all registers to Stack

  	rts






