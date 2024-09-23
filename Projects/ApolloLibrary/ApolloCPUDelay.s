* Apollo CPU Delay *

	XDEF _ApolloCPUDelay
	CNOP 0,4

_ApolloCPUDelay:
	move.l #92000,d1
	mulu.l d1,d0				* Multiply d0 (wait time in ms) with Cycles per ms to get desired cycle waits as result in d0
	dc.w $4e7a,$1809			* Get current CPU cycle counter in d1
	add.l d0,d1					* Add current CPU cycle counter to desired cycle waits to get target cycle

.WaitLoop:
	dc.w $4e7a,$0809			* Get current CPU cycle counter in d0
	cmp.l d1,d0 				* Compare desired target cycle with current cycle
	bcs .WaitLoop				* If target cycle is greater or equal we loop

	rts
