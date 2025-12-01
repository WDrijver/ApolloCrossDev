*********************************************************
* Apollo Uncompress Sprite incl. Endian & Color Swap    *
*********************************************************
* a0 = s = source pointer 								*
* a1 = d = destination pointer 							*
* d3 = w = sprite width 								*
* d4 = h = sprite height 								*
* d5 = spitch = source pitch in BYTES 					*
* $f81f = transparency color to replace $07C0 (green)	* 
*********************************************************

	XDEF _ApolloUncompressLoop
	CNOP 0,4

_ApolloUncompressLoop:
	movem.l d6/d5/d4/d3/d2,-(sp)					    * Save registers to Stack 
	bra.s .GoLoopHeight

.LoopHeight:
	move.l d3,d0									* d0 = reset width loop counter each height loop
	move.w (a0)+,d1									* d1 = read first WORD value from row (= number of lead transparency pixels)
	move.w (a0)+,d2									* d2 = read second WORD value from row (= number of lead transparency + sprite pixels -1)

.StartLoopWidth

.LeadPixels:
	cmp.w #$ffff,d1									* check if we have leading transparency pixels (0xffff = no)
	beq.s .SpritePixels								* if we don't then skip to GoPixels, if we do proceeed to GoLeadPixels
	sub.w d1,d0										* decrease width loop counter with amount of leading transparency pixels
	move.w d1,d6									* d6 = copy from d1 (= number of lead transparency pixels) 
	bra.s .GoLoopLeadPixels
.LoopLeadPixels:
	move.w #$f81f,(a1)+								* set destination pixel to transparent 
.GoLoopLeadPixels:
	dbra d1,.LoopLeadPixels							* decrease lead transparency pixel counter and if zero proceed to .GoPixels

.SpritePixels:
	cmp.w #$ffff,d2									* check if we have sprite pixels (0xffff = no)
	beq.s .TrailPixels								* if we don't then skip to GoTrailPixels, if we do proceeed to GoPixels
	addq.w #1,d2									* increase d2 with 1 (= number of lead transparency + sprite pixels)
	sub.w d6,d2										* substract the lead transparency to get the correct amount of sprite pixels
	sub.w d2,d0										* decrease width loop counter with amount of sprite pixels	
	bra.s .GoLoopSpritePixels

.LoopSpritePixels:
	move.w (a0)+,d1
	cmp.w #$07c0,d1
	beq.s .BlitTransparent
	move.w d1,(a1)+									* copy src-pixel to dest-pixel
	bra.s .GoLoopSpritePixels

.BlitTransparent:
	move.w #$f81f,(a1)+									* copy shadow color pixel to dest-pixel

.GoLoopSpritePixels:
	dbra d2,.LoopSpritePixels						* decrease lead transparency pixel counter and if zero proceed to .GoPixels

.TrailPixels:
	bra.s .GoLoopTrailPixels
.LoopTrailPixels:
	move.w #$f81f,(a1)+								* set destination pixel to transparent - change back to #$f81f
.GoLoopTrailPixels:
	dbra d0,.LoopTrailPixels						* decrease (remaining) width counter and if zero proceed

.EndLoopWidth:
	add.l d5,a1										* a1 = add pitch d5 to destination pointer (modulo)

.GoLoopHeight:
	dbra d4,.LoopHeight								* d4 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d2/d3/d4/d5/d6   					* Restore all registers from Stack
	rts
