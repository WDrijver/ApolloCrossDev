*********************************************************
* Apollo Blit Alpha with Color Key and Transparency 	*
*********************************************************
* a0 = s = source pointer 								*
* a1 = d = destination pointer 							*
* d3 = w = blitbox width 								*
* d4 = h = blitbox height 								*
* d5 = spitch = source pitch in BYTES 					*
* d6 = dpitch = destination pitch in BYTES 				* 
* d7 = al = alpha level (0-255) 						*
* a2 = ac = alpha color (R5G6B5)			 			*
*********************************************************

	XDEF _ApolloBlitAlphaKeyTransLoop
	CNOP 0,4

_ApolloBlitAlphaKeyTransLoop:
	movem.l a2/d7/d6/d5/d4/d3/d2,-(sp)			      	* Save registers to Stack
	lsl.l #1,d5											* d5 = convert spitch from pixel WORDS to BYTES
	lsl.l #1,d6 										* d6 = convert dpitch from pixel WORDS to BYTES 
	perm #@3000,d7,d7									* d1 = BYTE#1 = alpha-level (A) for use with pmula on dst
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d2										* d3 = width loop counter = reset each height loop from d2
	bra.s .GoLoopWidth16bit                             * if h > 0 start width loop by jumping to .LoopWidth

.LoopWidth16bit:
	move.w (a0)+,d0                                     * copy from src-pixel and increase source pointer
	cmp.w a2,d0                                         * check if d0 source-pixel = a2 alpha color
  	beq .BlitAlpha                                      * if true jump to alpha blend routine .BlitAlpha else continue
	cmp.w #$f81f,d0                                     * check if d0 source-pixel = $f81f transparency color
    beq .BlitTrans                                      * if true jump to .BlitTrans else continue
	move.w d0,(a1)                                      * copy to dst-pixel

.BlitTrans:
	addq.l #2,a1                                        * increase a1 destination pointer

.GoLoopWidth16bit:
	dbra d2,.LoopWidth16bit                             * d2 = decrease width loop counter
	add.l d5,a0 										* a0 = add source pitch d5 to source pointer (modulo)
	add.l d6,a1 										* a1 = add destination pitch d6 to destination pointer (modulo)

.EndLoopHeight:
	dbra d4,.LoopHeight                                 * d0 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d2/d3/d4/d5/d6/d7/a2		    		* Restore all registers from Stack
	rts

.BlitAlpha:
	unpack1632 -2(a1),d0:d1								* unpack dst-pixels with 2 byte left-shift to get our src-pixel in lower d0 word
	pmula d7,d0,d1										* apply alpha-level to dst-pixel and store result in d1
	pack3216 d1,d1,d1									* pack dest-pixels from d1 and d1 from ARGB into R5G6B5 d1
    move.w d1,(a1)+                                     * move d1 to dst-pixel and increase destination pointer
	bra.s .GoLoopWidth16bit                             * continue loop to .EndLoopWidth

