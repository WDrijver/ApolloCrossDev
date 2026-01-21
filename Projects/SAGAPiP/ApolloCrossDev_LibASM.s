***************************************************
* ApolloCrossDev Assembler Library                *
* 21-1-2025                                       *
***************************************************

* 1. ApolloFill		= Fill Destination with value (Generic)
* 2. ApolloCopy		= Copy Source to Destination (Generic)
* 3. ApolloCopy32	= Copy Source to Destination (32-Byte Aligned)

***************************************************
* ApolloFill                                      *
***************************************************
* a0 = dst = destination pointer                  *
* d3 = w = blitbox width in Bytes                 *
* d4 = h = blitbox height in Bytes                *
* d5 = dstmod = destination modulo in Bytes       *
* d7 = value in ULONG (4 Bytes / 32 Bits)         *
***************************************************

	XDEF _ApolloFill
	CNOP 0,4

_ApolloFill:
	movem.l d7/d5/d4/d3,-(sp)			      			* Save registers to Stack
	vperm #$45674567,d7,d7,d7							* fill d7 with 2 x ULONG value
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d0										* d0 = reset width loop counter each height loop
	lsr.w  #3,d0										* shift 3 positions left to change 1 to 8 Byte Chunk counter
	bra.s .GoLoopWidth8Byte	

.LoopWidth8Byte:
	store d7,(a0)+										* store 8 Byte value into destination

.GoLoopWidth8Byte:
	dbra d0,.LoopWidth8Byte								* d0 = decrease 8-byte CHUNK width loop counter and jump to .LoopWidth8Byte			
	moveq #3,d0											* d0 = set lowest two bits to %11 ( = 3 WORDS)
	and.w d3,d0											* d0 = mask lowest two bits of width for remainder WORD counter
	bra.s .GoLoopWidth2Byte								* Process remainder width WORD

.LoopWidth2Byte:
	move.w d7,(a0)+                                     * store 2 Byte value into destination

.GoLoopWidth2Byte:
	dbra d0,.LoopWidth2Byte                             * d2 = decrease width loop counter 
	add.l d5,a0 										* a0 = add source pitch d5 to source pointer (modulo)

.EndLoopHeight:
	dbra d4,.LoopHeight                                 * d0 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d3/d4/d5/d7			    			* Restore all registers from Stack
	rts
    

*************************************************************
* ApolloCopy                                                *
*************************************************************
* a0 = src = source pointer									*	
* a1 = dst = destination pointer							*
* d3 = w = blitbox width in Bytes                           *
* d4 = h = blitbox height in Bytes                          *
* d5 = srcmod = source modulo in Bytes						*
* d6 = dstmod = destination modulo in Bytes					*
*************************************************************

	XDEF _ApolloCopy
	CNOP 0,4

_ApolloCopy:
	movem.l a6/d6/d5/d4/d3,-(sp)	* Save all registers to Stack
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
