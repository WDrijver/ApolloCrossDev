*********************************************************
* Apollo Uncompress Vector Sprite		 				*
*********************************************************
* a0 = s = source pointer to base of dictionary			*
* a1 = s = index pointer for offset from base 			*
* a2 = d = destination pointer 							*
* d3 = w = sprite width 								*
* d4 = h = sprite height 								*
* d5 = spitch = source pitch in BYTES 					*
*********************************************************

	XDEF _ApolloUncompressVectorLoop
	CNOP 0,4

_ApolloUncompressVectorLoop:
	movem.l a2/a3/d5/d4/d3,-(sp)					* Save registers to Stack 
	lsl.l #1,d5										* convert spitch from pixel WORDS to BYTES
	lsr.l #2,d3										* d3 = width counter = Sprite Width / 4 (4xWORD chunks) 
	bra.s .GoLoopHeight

.LoopHeight:
	move.l d3,d0									* d0 = reset width loop counter each height loop

.LoopWidth:
	bra.s .GoLoopWidth
.LoopChunks:
	move.l a0,a3									* a3 = copy of base pointer to sprite dictionary
	move.w (a1)+,d1									* d1 = read index value (offset) in UOCTA (8BYTE) from (a1)
	lsl.w #3,d1										* recalculate index to BYTE = x8 = shift-left 3 positions
	add.w d1,a3										* add index to copy of base address of sprite dictionary
	load (a3),d1									* load 8BYTES from the index location
	store d1,(a2)+									* store 8BYTES to destination
.GoLoopWidth:
	dbra d0,.LoopChunks								* decrease width counter and if zero proceed to .EndLoopWidth
.EndLoopWidth:
	add.l d5,a2										* a1 = add pitch d5 to destination pointer (modulo)

.GoLoopHeight:
	dbra d4,.LoopHeight								* d4 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d3/d4/d5/a2/a3					* Restore all registers from Stack
	rts
