***************************************************
* Apollo Fill                                     *
***************************************************
* a0 = d = destination pointer                    *
* d3 = w = blitbox width                          *
* d4 = h = blitbox height                         *
* d5 = dpitch = destination pitch in BYTES        *
* d7 = fill color (R5G6B5)                        *
***************************************************

	XDEF _ApolloFillLoop
	CNOP 0,4

_ApolloFillLoop:
	movem.l d7/d5/d4/d3,-(sp)			      			* Save registers to Stack
	lsl.l #1,d5											* d5 = convert spitch from pixel WORDS to BYTES
	vperm #$67676767,d7,d7,d7							* fill d7 with 4 x WORD fill color
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d0										* d0 = reset width loop counter each height loop
	lsr.w  #2,d0										* divide width value by 4 to for 8-byte chunk counter
	bra.s .GoLoopWidth8Byte	

.LoopWidth8Byte:
	store d7,(a0)+										* store 4 x WORD fill color into destination

.GoLoopWidth8Byte:
	dbra d0,.LoopWidth8Byte								* d0 = decrease 8-byte CHUNK width loop counter and jump to .LoopWidth8Byte			
	moveq #3,d0											* d0 = set lowest two bits to %11 ( = 3 WORDS)
	and.w d3,d0											* d0 = mask lowest two bits of width for remainder WORD counter
	bra.s .GoLoopWidth2Byte								* Process remainder width WORD

.LoopWidth2Byte:
	move.w d7,(a0)+                                     * store 1 x WORD fill color into destination

.GoLoopWidth2Byte:
	dbra d0,.LoopWidth2Byte                             * d2 = decrease width loop counter 
	add.l d5,a0 										* a0 = add source pitch d5 to source pointer (modulo)

.EndLoopHeight:
	dbra d4,.LoopHeight                                 * d0 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d3/d4/d5/d7			    			* Restore all registers from Stack
	rts
