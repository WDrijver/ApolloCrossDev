*************************************************************
* Apollo Permit 										 	*
*************************************************************

	XDEF _ApolloPermit
	XREF _LVOPermit

	INCLUDE "exec/types.i"

	CNOP 0,4

_ApolloPermit:

	movem.l a6,-(sp)				* Save all registers to Stack

	movea.l $4.w,a6					* Load Exec base address in a6

	jsr _LVOPermit(a6)	    		* Forbid()
    
	movem.l (sp)+,a6				* Save all registers to Stack

  	rts




