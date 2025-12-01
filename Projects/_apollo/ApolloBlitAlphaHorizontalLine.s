*********************************************************
* Apollo Blit Alpha Horizontal Line                     *
*********************************************************
* a0 = s = source pointer 								*
* a1 = d = destination pointer 							*
* d3 = w = line width 									*
* d6 = sc = source color to be added (R8G8B8 24-bit)	* 
* d7 = al = alpha level to be multiplied (0-255) 		*
*********************************************************

	XDEF _ApolloBlitAlphaHorizontalLineLoop
	CNOP 0,4

_ApolloBlitAlphaHorizontalLineLoop:
	movem.l d7/d6/d3,-(sp)						      	* Save registers to Stack
	perm #@3000,d7,d7									* d7 = BYTE#1 = alpha-level (A) for use with pmula on src-pixel
	bra.s .EndLoopWidth16bit

.LoopWidth16bit:
	unpack1632 -2(a0),d0:d1								* unpack src-pixels with 2 byte left-shift to get our src-pixel in lower d0 word
	pmula d7,d0,d0										* multiply with alpha level to src-pixel in d0
	add.l d6,d0											* add source color to src-pixel in d0
	pack3216 d0,d0,d0									* pack dest-pixels from d0 and d0 from ARGB into R5G6B5 d0
	move.w d0,(a1)+                                    	* move d0 to dst-pixel and increase dst-pixel pointer
	addq.w #2,a0										* increase src-pixel pointer

.EndLoopWidth16bit:
	dbra d3,.LoopWidth16bit                             * d3 = decrease width loop counter 
	movem.l (sp)+,d3/d6/d7					    		* Restore all registers from Stack
	rts
