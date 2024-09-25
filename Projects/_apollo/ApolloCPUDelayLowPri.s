* Apollo CPU Delay in Low Pri (-127) *
    
    XDEF _ApolloCPUDelayLowPri
	
    XREF _LVOFindTask
	XREF _LVOSetTaskPri

	INCLUDE "exec/types.i"
	INCLUDE "dos/dos.i"

	CNOP 0,4

_ApolloCPUDelayLowPri:
	movem.l a6,-(sp)				* Save all registers to Stack

	movea.l $4.w,a6					* Load Exec base address in a6

	clr.l a1            			* Store 0 in a1 (current task)
	jsr _LVOFindTask(a6)			* Find current task ID (return in d0)
	move.l d0,a1					* Store Task ID in a1
    moveq #-127,d0		 			* Store Task Prio in d0 (highest value)
    jsr _LVOSetTaskPri(a6)
    
    move.l #92000,d0
	mulu.l d0,d1				    * Multiply d1 (wait time in ms) with d0 (Cycles per ms) to get desired cycle waits as result in d1
	dc.w $4e7a,$0809			    * Get current CPU cycle counter in d0
	add.l d0,d1					    * Add current CPU cycle counter to desired cycle waits to get target cycle

.WaitLoop:
	dc.w $4e7a,$0809			    * Get current CPU cycle counter in d0
	cmp.l d1,d0 				    * Compare desired target cycle with current cycle
	bcs .WaitLoop				    * If target cycle is greater or equal we loop

	clr.l a1            			* Store 0 in a1 (current task)
	jsr _LVOFindTask(a6)			* Find current task ID (return in d0)
	move.l d0,a1					* Store Task ID in a1
    moveq #-1,d0		 			* Store Task Prio in d0 (highest value)
    jsr _LVOSetTaskPri(a6)

	movem.l (sp)+,a6				* Save all registers to Stack

    rts
