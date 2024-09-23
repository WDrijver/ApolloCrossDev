*************************************************************
* Apollo Copy (32-Byte aligned width mandatory) 			*
*************************************************************
* a0 = s = source pointer									*	
* a1 = d = destination pointer								*
* d3 = w = blitbox width									*
* d4 = h = blitbox height									*
* d5 = spitch = source pitch in WORDS						*
* d6 = dpitch = destination pitch in WORDS					*
*************************************************************

	XDEF _ApolloCopy32Loop
	CNOP 0,4

_ApolloCopy32Loop:
	movem.l a6/d6/d5/d4/d3,-(sp)	* Save all registers to Stack
	lsl.l #1,d5						* convert spitch from pixel WORDS to BYTES
	lsl.l #1,d6						* convert dpitch from pixel WORDS to BYTES
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d0					* d0 = reset width loop counter each height loop
	lsr.w  #4,d0					* divide width value by 16 to for 256-bit chunk counter
	bra.s .GoLoopWidth32Byte

.LoopWidth32Byte:
	move16 (a0)+,(a1)+				* copy 16 Bytes from a0 source to a1 destination
	move16 (a0)+,(a1)+				* copy 16 Bytes from a0 source to a1 destination
	
.GoLoopWidth32Byte:
	dbra   d0,.LoopWidth32Byte

.EndLoopWidth:
	add.l d5,a0						* a0 = add source pitch d4 to source pointer (modulo)
	add.l d6,a1						* a1 = add destination pitch d5 to destination pointer (modulo)

.EndLoopHeight:
	dbra d4,.LoopHeight				* d4 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d3/d4/d5/d6/a6	* Restore all registers from Stack
	rts
