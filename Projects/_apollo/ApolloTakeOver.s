*************************************************************
* Apollo Take Over										 	*
*************************************************************

	XDEF _ApolloTakeOver
	XREF _LVOOpenLibrary
	XREF _LVOCloseWorkBench
	XREF _LVOFindTask
	XREF _LVOSetTaskPri

	INCLUDE <exec/types.i>
	INCLUDE "dos/dos.i"
	INCLUDE "lvo/intuition_lib.i"
	INCLUDE "intuition/intuition.i"
	INCLUDE "intuition/intuitionbase.i"

	CNOP 0,4

_ApolloTakeOver:

	movem.l a6,-(sp)				* Save all registers to Stack

	movea.l $4.w,a6					* Load Exec base address in a6

	lea INTName(pc),a1 				* a1 = intuition.library name
	moveq #0,d0						* d0 = intuition library version (0 = whatever)							
  	jsr _LVOOpenLibrary(a6)        	* open intuition library

	movea.l d0,a6
  	jsr _LVOCloseWorkBench(a6)     	* close workbench

	movea.l $4.w,a6					* Load Exec base address in a6

	clr.l a1            			* Store 0 in a1 (current task)
	jsr _LVOFindTask(a6)			* Find current task ID (return in d0)
	move.l d0,a1					* Store Task ID in a1
    moveq #20,d0		 			* Store Task Prio in d0
    jsr _LVOSetTaskPri(a6)

	movem.l (sp)+,a6				* Save all registers to Stack

  	rts

INTName dc.b    'intuition.library',0

**********************************************************************
* DATA
**********************************************************************





