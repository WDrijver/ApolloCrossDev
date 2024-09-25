*********************************************************
* Apollo Blit Green Alpha to Background                 *
*********************************************************
* a0 = src = source pointer                             *
* a1 = dst = destination pointer                        *
* a2 = alpha = alpha pointer                            *
* d3 = width = blitbox width                            *
* d4 = height = blitbox height                          *
* d5 = spitch = source pitch in BYTES                   *
* d6 = dpitch = destination pitch in BYTES              * 
* d7 = apitch = alpha pitch in BYTES                    * 
*********************************************************

	XDEF _ApolloBlitGreenAlphaBGLoop
	CNOP 0,4

_ApolloBlitGreenAlphaBGLoop:
	movem.l a3/a2/d7/d6/d5/d4/d3,-(sp)					* Save all registers to Stack
	lsl.l #1,d5											* d5 = convert spitch from pixel WORDS to BYTES
	lsl.l #1,d6 										* d6 = convert dpitch from pixel WORDS to BYTES 
	lsl.l #1,d7											* d5 = convert apitch from pixel WORDS to BYTES	
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d2										* d3 = width loop counter = reset each height loop from d2
	bra.s .GoLoopWidth16bit                             * if h > 0 start width loop by jumping to .LoopWidth

.LoopWidth16bit:
	move.w (a0),d0										* copy src-pixel from a0 to d0	
	cmp.w #$f81f,d0										* test for transparent src-pixel
	beq.w .GoNextPixel									* skip to next pixel if tranparent
	unpack1632 -2(a0),d0:d1								* unpack src-pixels with 2 byte left-shift to get our src-pixel in lower d0 word
	clr.l d1											* clear register for alpha-pixel
	move.w (a2),d1                                      * copy alpha-pixel from a2 to d1
	lsr.w #4,d1											* shift-right R5G6B5 3 positions to get GREEN in upper 6 bits of lowest byte	
	*and.w #$007f,d1										* extract GREEN color component (high 6 bits of lowest byte = $fc) to use as alpha level in d1
	perm #@3000,d1,d1									* d1 = BYTE#1 = alpha-level (A) for use with pmula on source
	pmula d1,d0,d0										* apply alpha-level to our src-pixels in lower d0 and store in d0
	not.l d1											* inverse alpha-pixel for use on dst-pixel	
	perm #@0567,d1,d0									* put alpha-level (BYTE#0 from d1) in BYTE#0 position and keep alpha-src-pixel colors in BYTE1/2/3 unchanged
	move.l d0,a3										* backup d0 to a3 to clear d0 for use on dst-pixel
	unpack1632 -2(a1),d0:d1								* unpack dst-pixels with 2 byte left-shift to get our src-pixel in lower d0 word
	move.l a3,d1										* restore a3 to d1	
	pmula d1,d0,d0										* apply inverse-alpha-level to our dst-pixel in lower d0
	pack3216 d0,d0,d0									* pack dest-pixels from d0 (ARGB) into d0 (R5G6B5)
	move.w d0,(a1)                                    	* move d0 to dst-pixel

.GoNextPixel:
	addq.l #2,a0                                        * increase ao source pointer  
	addq.l #2,a1                                        * increase a1 destination pointer  
	addq.l #2,a2                                        * increase a2 alpha pointer 

.GoLoopWidth16bit:
	dbra d2,.LoopWidth16bit                             * d2 = decrease width loop counter 
	add.l d5,a0 										* a0 = add source pitch d5 to source pointer (modulo)
	add.l d6,a1 										* a1 = add destination pitch d6 to destination pointer (modulo)
	add.l d7,a2 										* a2 = add alpha pitch d6 to alpha pointer (modulo)

.EndLoopHeight:
	dbra d4,.LoopHeight                                 * d0 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d3/d4/d5/d6/d7/a2/a3	    			* Restore all registers from Stack
	rts

