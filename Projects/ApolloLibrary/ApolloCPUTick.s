* Apollo CPU Tick *

	XDEF _ApolloCPUTick
	CNOP 0,4

_ApolloCPUTick:
	dc.w $4e7a,$0809
	rts
