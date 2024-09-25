* Apollo CPU Time *

	XDEF _ApolloCPUTime
	CNOP 0,4

_ApolloCPUTime:
	dc.w $4e7a,$0809			* Get CPU Cycles in d0
	divu.l #92000,d0			* Divide d0 with Cycles per ms to get milliseconds as output in d0

	rts
