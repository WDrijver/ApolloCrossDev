*************************************************************
* Apollo Copy 											 	*
*************************************************************
* a0 = s = source pointer									*	
* a1 = d = destination pointer								*
* d3 = w = blitbox width									*
* d4 = h = blitbox height									*
* d5 = spitch = source pitch in WORDS						*
* d6 = dpitch = destination pitch in WORDS					*
*************************************************************

	XDEF _ApolloCopyLoop
	CNOP 0,4

_ApolloCopyLoop:
	movem.l a6/d6/d5/d4/d3,-(sp)	* Save all registers to Stack
	lsl.l #1,d5						* convert spitch from pixel WORDS to BYTES
	lsl.l #1,d6						* convert dpitch from pixel WORDS to BYTES
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d0					* d0 = reset width loop counter each height loop
	lsr.w  #3,d0					* divide width counter by 8 to convert 2-byte pixel to 16-byte pixel counter
	bra.s .GoLoopWidth16Byte

.LoopWidth16Byte:
	move16 (a0)+,(a1)+				* copy 16 bytes from a0 source to a1 destination
	
.GoLoopWidth16Byte:
	dbra   d0,.LoopWidth16Byte
	moveq #7,d0						* d0 = set lowest three bits to %111 (= #7)
	and.w d3,d0						* d0 = mask lowest three bits of width for remainder 2-byte pixel counter
	bra.s .GoLoopWidth2Byte			* Process remainder width WORD

.LoopWidth2Byte:
	move.w (a0)+,(a1)+				* copy src-pixel to dst-pixel
.GoLoopWidth2Byte:
	dbra d0,.LoopWidth2Byte			* d0 = decrease WORD width loop counter and jump to .Loop16bit

.EndLoopWidth:
	add.l d5,a0						* a0 = add source pitch d4 to source pointer (modulo)
	add.l d6,a1						* a1 = add destination pitch d5 to destination pointer (modulo)

.EndLoopHeight:
	dbra d4,.LoopHeight				* d4 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d3/d4/d5/d6/a6	* Restore all registers from Stack
	rts
