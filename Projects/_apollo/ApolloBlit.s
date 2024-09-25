*********************************************************
* Apollo Blit with Transparency 						*
*********************************************************
* a0 = s = source pointer 								*
* a1 = d = destination pointer 							*
* d3 = w = blitbox width 								*
* d4 = h = blitbox height 								*
* d5 = spitch = source pitch in WORDS 					*
* d6 = dpitch = destination pitch in WORDS 				* 
*********************************************************

	XDEF _ApolloBlitLoop
	CNOP 0,4

_ApolloBlitLoop:
	movem.l a6/d6/d5/d4/d3,-(sp)					* Save 4 registers to Stack 
	lsl.l #1,d5										* d5 = convert spitch from pixel WORDS to BYTES 
	lsl.l #1,d6 									* d6 = convert dpitch from pixel WORDS to BYTES  	
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d0									* d0 = reset width loop counter each height loop
	lsr.w  #2,d0									* divide width value by 4 to for 8-byte chunk counter
	bra.s .GoLoopWidth8Byte	

.LoopWidth8Byte:
	load (a0)+,d1
	storem3 d1,d2,(a1)+								* d2 = Mode #2 - VASM bug *	
.GoLoopWidth8Byte:
	dbra d0,.LoopWidth8Byte							* d0 = decrease 64-bit CHUNK width loop counter and jump to .Loop64bit			
	moveq #3,d0										* d0 = set lowest two bits to %11 ( = 3 WORDS)
	and.w d3,d0										* d0 = mask lowest two bits of width for remainder WORD counter
	bra.s .GoLoopWidth2Byte							* Process remainder width WORD

.LoopWidth2Byte:
	move.w (a0)+,d1									* copy from src-pixel and increase source pointer
	cmp.w #$f81f,d1									* check if d1 source-pixel = 0xF81F transparency color
	beq.s .BlitTrans								* if true jump to .BlitTrans else continue	
	move.w d1,(a1)
.BlitTrans:
	addq.l #2,a1
.GoLoopWidth2Byte:
	dbra d0,.LoopWidth2Byte							* d0 = decrease WORD width loop counter and jump to .Loop16bit

.EndLoopWidth:
	add.l d5,a0										* a0 = add source pitch d4 to source pointer (modulo)
	add.l d6,a1										* a1 = add destination pitch d5 to destination pointer (modulo)

.EndLoopHeight:
	dbra d4,.LoopHeight								* d4 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d3/d4/d5/d6/a6					* Restore all registers from Stack
	rts
