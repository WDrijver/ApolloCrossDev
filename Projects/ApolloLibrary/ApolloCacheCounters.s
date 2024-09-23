*************************************************************
* Apollo Cache Counters									 	*
*************************************************************
* a0[0] = dcache_miss	(SPR $80F)							*
* a0[1] = dcache_hit	(SPR $80E)							*
* a0[2] = icache_miss	(SPR $814)							*
* a0[3] = mips_pipe_1	(SPR $80A)							*
* a0[4] = mips_pipe_2	(SPR $80B)							*
*************************************************************

	XDEF _ApolloCacheCounters
	CNOP 0,4

_ApolloCacheCounters:
	dc.w $4e7a,$080f					* d0 = dcache miss
	move.l d0,(a0)+
	dc.w $4e7a,$080e					* d1 = dcache hit
	move.l d0,(a0)+
	dc.w $4e7a,$0814					* d2 = icache miss
	move.l d0,(a0)+
	dc.w $4e7a,$080a					* d3 = mips pipe #1
	move.l d0,(a0)+
	dc.w $4e7a,$080b					* d3 = mips pipe #2
	move.l d0,(a0)+

	rts

