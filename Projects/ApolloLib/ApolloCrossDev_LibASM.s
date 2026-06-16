***************************************************
* ApolloCrossDev Assembler Library                *
* 30-3-2026                                       *
***************************************************

* 1. ApolloFillBitMap		= Fill BitMap with 32-bit values
* 2. ApolloFillBlock		= Fill Block of Memory with 32-bit values (32-Byte Aligned)
* 3. ApolloFillColor		= Fill BitMap with Color values
* 4. ApolloCopyPicture		= Copy Source to Destination (Generic)
* 5. ApolloCopyPicture32	= Copy Source to Destination (32-Byte Aligned)

***************************************************
* ApolloFillBitMap                                *
***************************************************
* a0 = dst = destination pointer                  *
* d3 = w = blitbox width in Pixels                *
* d4 = h = blitbox height in Pixels               *
* d5 = d = blitbox colordepth in Bits             *
* d6 = dstmod = destination modulo in Pixels      *
* d7 = value in uint32_t (4 Bytes / 32 Bits)      *
***************************************************

	XDEF _ApolloFillBitMap
	CNOP 0,4

_ApolloFillBitMap:
	movem.l d7/d6/d5/d4/d3,-(sp)			      		* Save registers to Stack
	lsr.l #3,d5											* d5 = convert colordepth in Bits to colordepth in Bytes
	mulu.l d5,d3										* d3 = calculate effective width in Bytes (width * colordepth)
	mulu.l d5,d6										* d6 = calculate effective destination modulo in Bytes (modulo * colordepth)
	vperm #$45674567,d7,d7,d7							* fill d7 with 2 x uint32_t value
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d0										* d0 = reset width loop counter each height loop
	lsr.l  #3,d0										* shift 3 positions left to change 1 to 8 Byte Chunk counter
	bra.s .GoLoopWidth8Byte	

.LoopWidth8Byte:
	store d7,(a0)+										* store 8 Byte value into destination

.GoLoopWidth8Byte:
	dbra.l d0,.LoopWidth8Byte								* d0 = decrease 8-byte CHUNK width loop counter and jump to .LoopWidth8Byte			
	moveq #3,d0											* d0 = set lowest two bits to %11 ( = 3 WORDS)
	and.l d3,d0											* d0 = mask lowest two bits of width for remainder WORD counter
	bra.s .GoLoopWidth2Byte								* Process remainder width WORD

.LoopWidth2Byte:
	move.w d7,(a0)+                                     * store 2 Byte value into destination

.GoLoopWidth2Byte:
	dbra.l d0,.LoopWidth2Byte                             * d2 = decrease width loop counter 
	add.l d6,a0 										* a0 = add source pitch d5 to source pointer (modulo)

.EndLoopHeight:
	dbra.l d4,.LoopHeight                                 * d0 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d3/d4/d5/d6/d7			    		* Restore all registers from Stack
	rts
    

***************************************************
* ApolloFillBlock                                 *
***************************************************
* a0 = dst = destination pointer                  *
* d3 = c = byte counter				              *
* d4 = value in uint32_t (4 Bytes / 32 Bits)      *
***************************************************

	XDEF _ApolloFillBlock
	CNOP 0,4

_ApolloFillBlock:
	movem.l d4/d3,-(sp)			      					* Save registers to Stack
	lsr.l  #5,d3										* divide width value by 32 to for 32-Byte chunk counter
	vperm #$45674567,d4,d4,d4							* fill d7 with 2 x uint32_t value
	bra.s .EndLoop

.Loop32Byte:
	store d4,(a0)+										* store 8 Byte value into destination
	store d4,(a0)+										* store 8 Byte value into destination
	store d4,(a0)+										* store 8 Byte value into destination
	store d4,(a0)+										* store 8 Byte value into destination

.EndLoop:
	dbra.l d3,.Loop32Byte                              	* d0 = decrease loop counter and jump to .LoopHeight
	movem.l (sp)+,d3/d4						    		* Restore all registers from Stack
	rts

***************************************************
* ApolloFillColor                                 *
***************************************************
* a0 = dst = destination pointer                  *
* d3 = w = blitbox width in Pixels                *
* d4 = h = blitbox height in Pixels               *
* d5 = d = blitbox colordepth in Bits             *
* d6 = dstmod = destination modulo in Pixels      *
* d7 = color value (BYTE REVERSED !!!             *
***************************************************

	XDEF _ApolloFillColor
	CNOP 0,4

_ApolloFillColor:
	movem.l d7/d6/d5/d4/d3/d2,-(sp)			      		* Save registers to Stack
	lsr.l #3,d5											* d5 = convert colordepth in Bits to colordepth in Bytes
	mulu.l d5,d6										* d6 = calculate effective destination modulo in Bytes (modulo * colordepth)
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d0										* d0 = reset width loop counter each height loop
	bra.s .GoLoopWidthColor	

.LoopWidthColor:
	move.l d5,d1
	move.l d7,d2	
	bra.s .GoLoopWidthColorByte							* d1 = reset color byte counter each width loop

.LoopWidthColorByte:
	move.b d2,(a0)+										* store Color Byte value into destination
	lsr.l #8,d2											* shift d2 right by 8 bits to get next color byte into lowest byte position	

.GoLoopWidthColorByte:
	dbra.l d1,.LoopWidthColorByte						* d1 = decrease color byte counter and jump to .GoLoopWidthColorByte

.GoLoopWidthColor:
	dbra.l d0,.LoopWidthColor							* d0 = decrease width loop counter and jump to .LoopWidthColor
	add.l d6,a0 										* a0 = add source pitch d5 to source pointer (modulo)

.EndLoopHeight:
	dbra.l d4,.LoopHeight                               * d0 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d2/d3/d4/d5/d6/d7			    		* Restore all registers from Stack
	rts


*************************************************************
* ApolloCopyPicture                                         *
*************************************************************
* a0 = src = source pointer									*	
* a1 = dst = destination pointer							*
* d3 = w = blitbox width in Bytes                           *
* d4 = h = blitbox height in Bytes                          *
* d5 = srcmod = source modulo in Bytes						*
* d6 = dstmod = destination modulo in Bytes					*
*************************************************************

	XDEF _ApolloCopyPicture
	CNOP 0,4

_ApolloCopyPicture:
	movem.l a6/d6/d5/d4/d3,-(sp)	* Save all registers to Stack
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d0					* d0 = reset width loop counter each height loop
	lsr.l  #4,d0					* divide width counter by 16 to convert byte pixel to 16-byte pixel chunk counter
	bra.s .GoLoopWidth16Byte

.LoopWidth16Byte:
	move16 (a0)+,(a1)+				* copy 16 bytes from a0 source to a1 destination
	
.GoLoopWidth16Byte:
	dbra.l   d0,.LoopWidth16Byte
	moveq #15,d0					* d0 = set lowest four bits to %1111 (= #15)
	and.l d3,d0						* d0 = mask lowest four bits of width for remainder byte pixel counter
	bra.s .GoLoopWidthByte			* Process remainder pixels

.LoopWidthByte:
	move.b (a0)+,(a1)+				* copy src-pixel to dst-pixel
.GoLoopWidthByte:
	dbra.l d0,.LoopWidthByte			* d0 = decrease byte pixel width loop counter and jump to .LoopWidthByte

.EndLoopWidth:
	add.l d5,a0						* a0 = add source pitch d4 to source pointer (modulo)
	add.l d6,a1						* a1 = add destination pitch d5 to destination pointer (modulo)

.EndLoopHeight:
	dbra.l d4,.LoopHeight				* d4 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d3/d4/d5/d6/a6	* Restore all registers from Stack
	rts


*************************************************************
* ApolloCopyPicture32 (32-Byte aligned width mandatory) 	*
*************************************************************
* a0 = s = source pointer									*	
* a1 = d = destination pointer								*
* d3 = w = blitbox width in Bytes							*
* d4 = h = blitbox height in Bytes							*
* d5 = smodulo = source modulo in Bytes						*
* d6 = dmodulo = destination pitch in Bytes					*
*************************************************************

	XDEF _ApolloCopyPicture32
	CNOP 0,4

_ApolloCopyPicture32:
	movem.l d6/d5/d4/d3,-(sp)		* Save all registers to Stack
    lsr.l  #5,d3					* divide width value by 32 to for 256-bit chunk counter
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d0					* d0 = reset width loop counter each height loop
	bra.s .GoLoopWidth32Byte

.LoopWidth32Byte:
	move16 (a0)+,(a1)+				* copy 16 Bytes from a0 source to a1 destination
	move16 (a0)+,(a1)+				* copy 16 Bytes from a0 source to a1 destination
	
.GoLoopWidth32Byte:
	dbra.l   d0,.LoopWidth32Byte

.EndLoopWidth:
	add.l d5,a0						* a0 = add source pitch d4 to source pointer (modulo)
	add.l d6,a1						* a1 = add destination pitch d5 to destination pointer (modulo)

.EndLoopHeight:
	dbra.l d4,.LoopHeight				* d4 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d3/d4/d5/d6	    * Restore all registers from Stack
	rts


*************************************************************
* ApolloCopyBlock                                           *
*************************************************************
* a0 = src = source pointer									*	
* a1 = dst = destination pointer							*
* d3 = size = block size in Bytes                           *
*************************************************************

	XDEF _ApolloCopyBlock
	CNOP 0,4

_ApolloCopyBlock:
	movem.l a6/d6/d5/d4/d3,-(sp)	* Save all registers to Stack

.LoopSize:
	move.l d3,d0					* d0 = set size loop counter
	lsr.l  #4,d0					* divide size counter by 16 to determine 16-byte chunk counter
	bra.s .GoLoopSize16Byte

.LoopSize16Byte:
	move16 (a0)+,(a1)+				* copy 16 bytes from a0 source to a1 destination
	
.GoLoopSize16Byte:
	dbra.l  d0,.LoopSize16Byte
	moveq #15,d0					* d0 = set lowest four bits to %1111 (= #15)
	and.l d3,d0						* d0 = mask lowest four bits of width for remainder byte counter
	bra.s .GoLoopSizeByte			* Process remainder bytes

.LoopSizeByte:
	move.b (a0)+,(a1)+				* copy src-byte to dst-byte

.GoLoopSizeByte:
	dbra.l d0,.LoopSizeByte			* d0 = decrease byte counter and jump to .LoopSizeByte

.EndLoopSize:
	movem.l (sp)+,d3/d4/d5/d6/a6	* Restore all registers from Stack
	rts


*************************************************************
* ApolloCopyBlock32 (32-Byte aligned width mandatory) 	    *
*************************************************************
* a0 = s = source pointer									*	
* a1 = d = destination pointer								*
* d3 = w = block size in Bytes				     			*
*************************************************************

	XDEF _ApolloCopyBlock32
	CNOP 0,4

_ApolloCopyBlock32:
	movem.l d6/d5/d4/d3,-(sp)		* Save all registers to Stack

.LoopSize:
	move.l d3,d0					* d0 = reset width loop counter each height loop
    lsr.l  #5,d0					* divide size value by 32 to determine 256-bit chunk counter
	bra.s .GoLoopSize32Byte

.LoopSize32Byte:
	move16 (a0)+,(a1)+				* copy 16 Bytes from a0 source to a1 destination
	move16 (a0)+,(a1)+				* copy 16 Bytes from a0 source to a1 destination
	
.GoLoopSize32Byte:
	dbra.l   d0,.LoopSize32Byte

.EndLoopSize:
	movem.l (sp)+,d3/d4/d5/d6	    * Restore all registers from Stack
	rts


*************************************************************
* ApolloCopyLongs                                           *
*************************************************************
* a0 = src = source pointer									*	
* a1 = dst = destination pointer							*
* d3 = size = block size in Bytes                           *
*************************************************************

	XDEF _ApolloCopyLongs
	CNOP 0,4

_ApolloCopyLongs:
	movem.l a6/d6/d5/d4/d3,-(sp)	* Save all registers to Stack

.LoopSize:
	move.l d3,d0					* d0 = set size loop counter
	lsr.l  #2,d0					* divide size counter by 4 to determine 4-byte (LONG) chunk counter
	bra.s .GoLoopSize4Byte

.LoopSize4Byte:
	move.l (a0)+,(a1)+				* copy 4 bytes from a0 source to a1 destination
	
.GoLoopSize4Byte:
	dbra.l  d0,.LoopSize4Byte

.EndLoopSize:
	movem.l (sp)+,d3/d4/d5/d6/a6	* Restore all registers from Stack
	rts


*************************************************************
* ApolloFillLongs                                           *
*************************************************************
* a1 = dst = destination pointer							*
* d4 = value = fill value									*
* d3 = size = block size in Bytes                           *
*************************************************************

	XDEF _ApolloFillLongs
	CNOP 0,4

_ApolloFillLongs:
	movem.l a6/d6/d5/d4/d3,-(sp)	* Save all registers to Stack

.LoopSize:
	move.l d3,d0					* d0 = set size loop counter
	lsr.l  #2,d0					* divide size counter by 4 to determine 4-byte (LONG) chunk counter
	bra.s .GoLoopSize4Byte

.LoopSize4Byte:
	move.l d4,(a1)+					* fill 4 bytes at a1 with value in d4
	
.GoLoopSize4Byte:
	dbra.l  d0,.LoopSize4Byte

.EndLoopSize:
	movem.l (sp)+,d3/d4/d5/d6/a6	* Restore all registers from Stack
	rts

* Apollo CPU Tick *

	XDEF _ApolloCPUTick
	CNOP 0,4

_ApolloCPUTick:
	dc.w $4e7a,$0809			* Get current CPU cycle counter in d0
	rts


* Apollo CPU Delay *

	XDEF _ApolloCPUDelay
	CNOP 0,4

_ApolloCPUDelay:
	move.l #92000,d1
	mulu.l d1,d0				* Multiply d0 (wait time in ms) with Cycles per ms to get desired cycle waits as result in d0
	dc.w $4e7a,$1809			* Get current CPU cycle counter in d1
	add.l d0,d1					* Add current CPU cycle counter to desired cycle waits to get target cycle
	cmp.l #4294967295,d1		* Check for 32-bit overflow of CPU cycle
	bcc.s .WaitLoop2			* If no 32-bit overflow we skip to Waitloop2
	sub.l #4294967295,d1		* Correct CPU cycle for 32-bit overflow
	
.WaitLoop1:
	dc.w $4e7a,$0809			* Get current CPU cycle counter in d0
	cmp.l d0,d1 				* Compare current CPU Cycle with Target Cycle
	bcs.s .WaitLoop1			* If current CPU cycle is greater or equal we loop

.WaitLoop2:
	dc.w $4e7a,$0809			* Get current CPU cycle counter in d0
	cmp.l d1,d0 				* Compare Target Cycle with current CPU cycle
	bcs.s .WaitLoop2			* If target cycle is greater or equal we loop

	rts


*********************************************************
* ApolloEndianSwapWordBuffer >=8Byte & 2Byte Aligned	*
*********************************************************
* a0 = s = buffer source pointer 						*
* d0 = l = buffer length	 							*
*********************************************************

	XDEF _ApolloEndianSwapWordBuffer
	CNOP 0,4

_ApolloEndianSwapWordBuffer:
	movem.l d2,-(sp)			      					* Save registers to Stack
	move.l d0,d2										* save loopcounter in d2
	lsr.l #3,d0											* divide loopcounter by 8 for 64-bit chunks
	bra.s .Go8ByteLoop

.Loop8Byte:
	load (a0),d1										* load 4 x WORD from a0
	vperm #$10325476,d1,d1,d1							* swap BYTE pairs
	store d1,(a0)+										* store 4 x WORD back in a0

.Go8ByteLoop:
	dbra.l d0,.Loop8Byte								* countdown loop for 64-bit chunks
	moveq #3,d0											* d0 = set d0 = set mask bits to %11
	and.l d2,d0											* d0 = mask width for remainder WORD counter
	bra.s .Go2ByteLoop

.Loop2Byte:
	move.w (a0),d1										* load WORD from a0
	perm #@0032,d1,d1									* swap BYTE pair
	move.w d1,(a0)+										* store WORD back in a0

.Go2ByteLoop:
	dbra.l d0,.Loop2Byte								* countdown loop for 64-bit chunks
	movem.l (sp)+,d2						    		* Restore all registers from Stack
	rts


*********************************************************
* ApolloEndianSwapLongBuffer >=8Byte & 4Byte Aligned	*
*********************************************************
* a0 = s = buffer source pointer 						*
* d0 = l = buffer length	 							*
*********************************************************

	XDEF _ApolloEndianSwapLongBuffer
	CNOP 0,4

_ApolloEndianSwapLongBuffer:
	movem.l d2,-(sp)			      					* Save registers to Stack
	move.l d0,d2										* save loopcounter in d2
	lsr.l #3,d0											* divide loopcounter by 8 for 64-bit chunks
	bra.s .Go8ByteLoop

.Loop8Byte:
	load (a0),d1										* load 2 x LONG from a0
	vperm #$23016745,d1,d1,d1							* swap WORD pairs
	store d1,(a0)+										* store 2 x LONG back in a0

.Go8ByteLoop:
	dbra.l d0,.Loop8Byte								* countdown loop for 64-bit chunks
	moveq #5,d0											* d0 = set mask bits to %111
	and.l d2,d0											* d0 = mask width for remainder LONG counter
	bra.s .Go4ByteLoop

.Loop4Byte:
	move.l (a0),d1										* load LONG from a0
	perm #@2301,d1,d1									* swap BYTE pair
	move.l d1,(a0)+										* store LONG back in a0

.Go4ByteLoop:
	dbra.l d0,.Loop4Byte								* countdown loop for 64-bit chunks
	movem.l (sp)+,d2						    		* Restore all registers from Stack
	rts


* Apollo Endian Swap Word *

	XDEF _ApolloSwapWord
	CNOP 0,4

_ApolloSwapWord:
	perm #@0032,d0,d0									* swap WORD
	
	rts
	
* Apollo Endian Swap Long *

	XDEF _ApolloSwapLong
	CNOP 0,4

_ApolloSwapLong:
	perm #@3210,d0,d0									* swap LONG
	
	rts

* Apollo Endian Swap Octa *

	XDEF _ApolloSwapOcta
	CNOP 0,4

_ApolloSwapOcta:
	vperm #$76543210,d0,d0,d0							* swap OCTA

	rts


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
	movem.l d6/d5/d4/d3,-(sp)	* Save all registers to Stack
	lsl.l #1,d5						* convert spitch from pixel WORDS to BYTES
	lsl.l #1,d6						* convert dpitch from pixel WORDS to BYTES
    lsr.w  #4,d3					* divide width value by 16 to for 256-bit chunk counter
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d0					* d0 = reset width loop counter each height loop
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
	movem.l (sp)+,d3/d4/d5/d6	    * Restore all registers from Stack
	rts


*********************************************************
* Apollo Endian Swap (2-Byte)						 	*
*********************************************************
* a0 = s = buffer source pointer 						*
* d0 = l = buffer length	 							*
*********************************************************

	XDEF _ApolloEndianSwap2Loop
	CNOP 0,4

_ApolloEndianSwap2Loop:
	lsr.l #1,d0											* divide loopcounter by 2 for 2-Byte chunks
	bra.s .Go2ByteLoop

.Loop2Byte:
	move.w (a0),d1										* load WORD from a0
	perm #@0032,d1,d1									* swap BYTE pair
	move.w d1,(a0)+										* store WORD back in a0

.Go2ByteLoop:
	dbra.l d0,.Loop2Byte								* countdown loop for2-Byte chunks
	rts


*********************************************************
* Apollo Endian Swap (8-byte Chunks)				 	*
*********************************************************
* a0 = s = buffer source pointer 						*
* d0 = l = buffer length	 							*
*********************************************************

	XDEF _ApolloEndianSwap8Loop
	CNOP 0,4

_ApolloEndianSwap8Loop:
	movem.l d2,-(sp)			      					* Save registers to Stack
	move.l d0,d2										* save loopcounter in d2
	lsr.l #3,d0											* divide loopcounter by 8 for 64-bit chunks
	bra.s .Go8ByteLoop

.Loop8Byte:
	load (a0),d1										* load 4 x WORD from a0
	vperm #$10325476,d1,d1,d1							* swap BYTE pairs
	store d1,(a0)+										* store 4 x WORD back in a0

.Go8ByteLoop:
	dbra.l d0,.Loop8Byte								* countdown loop for 64-bit chunks
	moveq #3,d0											* d0 = set lowest two bits to %11
	and.w d2,d0											* d0 = mask lowest two bits of width for remainder WORD counter
	bra.s .Go2ByteLoop

.Loop2Byte:
	move.w (a0),d1										* load WORD from a0
	perm #@0032,d1,d1									* swap BYTE pair
	move.w d1,(a0)+										* store WORD back in a0

.Go2ByteLoop:
	dbra.l d0,.Loop2Byte								* countdown loop for 64-bit chunks
	movem.l (sp)+,d2						    		* Restore all registers from Stack
	rts



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

*************************************************************
* Apollo Take Over										 	*
*************************************************************

	XDEF _ApolloTakeOver
	XREF _LVOOpenLibrary
	XREF _LVOCloseWorkBench
	XREF _LVOFindTask
	XREF _LVOSetTaskPri

	INCLUDE "exec/types.i"
	INCLUDE "dos/dos.i"
	INCLUDE "lvo/intuition_lib.i"
	INCLUDE "intuition/intuition.i"
	INCLUDE "intuition/intuitionbase.i"

	CNOP 0,4

_ApolloTakeOver:

	movem.l a6,-(sp)				* Save all registers to Stack

	movea.l $4.w,a6					* Load Exec base address in a6

	lea INTName(pc),a1 				* a1 = intuition.library name
	moveq #0,d0						* d0 = intuition library version (0 = whatever)							
  	jsr _LVOOpenLibrary(a6)        	* open intuition library

	movea.l d0,a6
  	jsr _LVOCloseWorkBench(a6)     	* close workbench

	movea.l $4.w,a6					* Load Exec base address in a6

	clr.l a1            			* Store 0 in a1 (current task)
	jsr _LVOFindTask(a6)			* Find current task ID (return in d0)
	move.l d0,a1					* Store Task ID in a1
    moveq #20,d0		 			* Store Task Prio in d0
    jsr _LVOSetTaskPri(a6)

	movem.l (sp)+,a6				* Save all registers to Stack

  	rts

INTName dc.b    'intuition.library',0


*************************************************************
* Apollo Forbid 										 	*
*************************************************************

	XDEF _ApolloForbid
	XREF _LVOForbid

	INCLUDE "exec/types.i"

	CNOP 0,4

_ApolloForbid:

	movem.l a6,-(sp)				* Save all registers to Stack

	movea.l $4.w,a6					* Load Exec base address in a6
	jsr _LVOForbid(a6)	    		* Forbid()
    
	movem.l (sp)+,a6				* Save all registers to Stack

  	rts


*************************************************************
* Apollo Disable										 	*
*************************************************************

	XDEF _ApolloDisable
    XREF _LVODisable

	INCLUDE "exec/types.i"

	CNOP 0,4

_ApolloDisable:

	movem.l a6,-(sp)				* Save all registers to Stack

    ; --- To Disable Everything ---
    movea.l 4.w,a6      ; ExecBase
    jsr _LVODisable(a6) ; Call Disable()

	movem.l (sp)+,a6				* Save all registers to Stack

    rts



********************************************
* Apollo Timer                             *
********************************************

    XDEF _ApolloTimer               * Export ApolloTimer() function to C++
    XDEF _GlobalTimer               * Export GlobalTimer variable to C++
    XREF _LVOAddIntServer           * Reference to Include

    CNOP 0,4

_ApolloTimer:
	movem.l a6,-(sp)	            * Save registers to Stack 
    movea.l $4.w,a6                 * a6 = Exec Base
    moveq.l #5,d0                   * d0 = VBL Interrupt Number
    lea ApolloTimerServer(pc),a1    * a1 = ApolloTimerServer
    jsr _LVOAddIntServer(a6)        * call AddIntServer(d0, a1)
    movem.l (sp)+,a6   	            * Restore all registers from Stack
    rts

ApolloTimerServer:                  * Struct Interrupt {Struct Node _Node,
    dc.l    0,0                     * Struct Node {Struct Node _Succ = 0, Struct Node _Pred = 0
    dc.b    2,0                     * UBYTE _Type = 2, BYTE _Prio = 0
    dc.l    IntName                 * char *_Name }
    dc.l    0,ApolloTimerCode       * APTR _Data = 0, void (*_Code)() = ApolloTimerIRQ }

ApolloTimerCode:
    addq.l #1,_GlobalTimer          * Increase GlobalTimer Counter each VBL
    moveq #0,d0                     * Z-Flag (latest code in chain)
	rts

IntName:        dc.b    "ApolloTimerIRQ",0

_GlobalTimer    dc.l    0


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
    

*********************************************************
* Apollo Blit Blue Alpha to UI                          *
*                                                       *
* Destination = Trans ($f81f) -> Blit when Alpha > $c0  *
* Destination = Color         -> Perform Alpha Blit     *
*                                                       *
* UI = PiP = Cookie-Cut on Background (Trans = $f81f)   *
* So Alpha Blit is not possible on Transparent Surface  *  
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

	XDEF _ApolloBlitBlueAlphaUILoop
	CNOP 0,4

_ApolloBlitBlueAlphaUILoop:
	movem.l a3/a2/d7/d6/d5/d4/d3,-(sp)					* Save all registers to Stack
	lsl.l #1,d5											* d5 = convert spitch from pixel WORDS to BYTES
	lsl.l #1,d6 										* d6 = convert dpitch from pixel WORDS to BYTES 
	lsl.l #1,d7											* d5 = convert apitch from pixel WORDS to BYTES
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d2										* d3 = width loop counter = reset each height loop from d2
	bra.s .GoLoopWidth16bit                             * if h > 0 start width loop by jumping to .LoopWidth

.LoopWidth16bit:
	move.w (a1),d0										* copy dst-pixel from a1 to d0	
	cmp.w #$f81f,d0										* if dst-pixel = transparent then Blit with fixed alpha threshhold else Blit with alpha 
	bne.w .BlitAlpha

.BlitAlphaFixed:
	move.w (a2),d1                                      * copy alpha-pixel from a2 to d1 
	lsl.w #3,d1											* shift-left R5G6B5 3 positions to get BLUE in upper 5 bits of lowest byte
	and.w #$00f8,d1										* extract BLUE color component (high 5 bits of lowest byte = $f8) to use as alpha level in d1
	cmp.w #$00c0,d1                                     * if d1 alpha level < $00c0 draw pixel else skip pixel
    ble.w .GoNextPixel                                  * if true jump to next pixel
	move.w (a0),(a1)									* copy src-pixel to dst-pixel
	bra.s .GoNextPixel

.BlitAlpha:
	move.w (a0),d0										* copy src-pixel from a0 to d0	
	cmp.w #$f81f,d0										* test for transparent src-pixel
	beq.w .GoNextPixel									* skip to next pixel if tranparent
	unpack1632 -2(a0),d0:d1								* unpack src-pixels with 2 byte left-shift to get our src-pixel in lower d0 word
	clr.l d1											* clear register for alpha-pixel
	move.w (a2),d1                                      * copy alpha-pixel from a2 to d1
	lsr.w #8,d1											* shift R5G6B5 8 positions to get RED in upper 5 bits of lowest byte
	and.w #$00f8,d1										* extract RED color component (high 5 bits of lowest byte = $f8) to use as alpha level in d1
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


*********************************************************
* Apollo Blit Red Alpha to Background                   *
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

	XDEF _ApolloBlitRedAlphaBGLoop
	CNOP 0,4

_ApolloBlitRedAlphaBGLoop:
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
	lsr.w #8,d1											* shift-right R5G6B5 8 positions to get RED in lowest byte
	*and.w #$00ff,d1										* extract RED color component (high 5 bits of lowest byte = $f8) to use as alpha level in d1
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


*********************************************************
* Apollo Blit Blue Alpha to Background                  *
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

	XDEF _ApolloBlitBlueAlphaBGLoop
	CNOP 0,4

_ApolloBlitBlueAlphaBGLoop:
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
	lsl.w #3,d1											* shift-left R5G6B5 3 positions to get BLUE in upper 5 bits of lowest byte
	and.w #$00f8,d1										* extract BLUE color component (high 5 bits of lowest byte = $f8) to use as alpha level in d1
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


*********************************************************
* Apollo Blit Alpha with Transparency 					*
*********************************************************
* a0 = s = source pointer 								*
* a1 = d = destination pointer 							*
* d3 = w = blitbox width 								*
* d4 = h = blitbox height 								*
* d5 = spitch = source pitch in BYTES			 		*
* d6 = dpitch = destination pitch in BYTES 				* 
* d7 = al = constant alpha level (0-255) 0=transparent 	*
*********************************************************

	XDEF _ApolloBlitAlphaConstLoop
	CNOP 0,4

_ApolloBlitAlphaConstLoop:
	movem.l a2/d7/d6/d5/d4/d3/d2,-(sp)			      	* Save registers to Stack
	lsl.l #1,d5											* d5 = convert spitch from pixel WORDS to BYTES
	lsl.l #1,d6 										* d6 = convert dpitch from pixel WORDS to BYTES 
	perm #@3000,d7,d7									* d1 = BYTE#1 = alpha-level (A) for use with pmula on source
	move.l d7,a2										* save alpha-level to a2
	bra.s .EndLoopHeight

.LoopHeight:
	move.l d3,d2										* d3 = width loop counter = reset each height loop from d2
	bra.s .GoLoopWidth16bit                             * if h > 0 start width loop by jumping to .LoopWidth

.LoopWidth16bit:
	move.w (a0),d0                                      * copy from src-pixel
	cmp.w #$f81f,d0                                     * check if d0 source-pixel = $f81f transparency color
    beq .BlitTrans                                      * if true jump to .BlitTrans else continue

.BlitAlpha:
	unpack1632 -2(a0),d0:d1								* unpack src-pixels with 2 byte left-shift to get our src-pixel in lower d0 word
	move.l a2,d7										* get a fresh copy of alpha-level from a2 to d7
	pmula d7,d0,d1										* apply alpha-level to src-pixel and store result in d1
	not.l d7											* inverse alpha-level for use on dst-pixel	
	perm #@0567,d7,d1									* put alpha-level (BYTE#0 from d7) in BYTE#0 position and keep alpha-src-pixel colors in BYTE1/2/3 unchanged
	move.l d1,d7										* move d1 to d7 to clear d1 for use on dst-pixel
	unpack1632 -2(a1),d0:d1								* unpack dst-pixels with 2 byte left-shift to get our src-pixel in lower d0 word
	pmula d7,d0,d1										* apply inverse-alpha-level on dst-pixel and add alpha-src-pixel to alpha-dst-pixel
	pack3216 d1,d1,d1									* pack dest-pixels from d1 and d1 from ARGB into R5G6B5 d1
	move.w d1,(a1)                                    	* move d1 to dst-pixel
	
.BlitTrans:
	addq.l #2,a0                                        * increase ao source pointer  
	addq.l #2,a1                                        * increase a1 destination pointer  

.GoLoopWidth16bit:
	dbra d2,.LoopWidth16bit                             * d2 = decrease width loop counter 
	add.l d5,a0 										* a0 = add source pitch d5 to source pointer (modulo)
	add.l d6,a1 										* a1 = add destination pitch d6 to destination pointer (modulo)

.EndLoopHeight:
	dbra d4,.LoopHeight                                 * d0 = decrease height loop counter and jump to .LoopHeight
	movem.l (sp)+,d2/d3/d4/d5/d6/d7/a2		    		* Restore all registers from Stack
	rts


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


*********************************************************
* ApolloGetMouseDelta                                   *
*********************************************************
* a0 = Mouse_Old-X [15-8] | Mouse_Old-Y [7-0]           *
*********************************************************

    XDEF _ApolloGetMouseDelta
    CNOP 0,4

_ApolloGetMouseDelta:
    lea.l $dff00a,a1        ; a1 = [15-8] Mouse Y | [7-0] Mouse X

.GetMouseDeltaX:
    clr.l d0
    clr.l d1
    move.b 1(a1),d0         ; d0 = current mouse X in lower byte
    move.b 1(a0),d1         ; d1 = old mouse X in lower byte
    move.b d0,1(a0)         ; store Delta in a0 (Mouse_Old)
    sub.w d1,d0             ; d0 = Delta X

.CheckUnderFlowX:
    cmp.w   #-127,d0
    bge     .CheckOverFlowX
    add.w   #255,d0            ; d0 = Delta X + 255

.CheckOverFlowX:
    cmp.w   #127,d0
    ble     .ShiftDeltaX
    sub.w   #255,d0            ; d0 = Delta X - 255

.ShiftDeltaX:
    lsl.l #8,d0              ; d0 = Delta X in upper WORD
    lsl.l #8,d0              ; d0 = Delta X in upper WORD
    
.GetMouseDeltaY:
    clr.l d1
    move.b 0(a1),d0         ; d0 = current mouse Y in lower byte
    move.b 0(a0),d1         ; d1 = old mouse Y in lower byte
    move.b d0,0(a0)         ; store Delta in a0 (Mouse_Old)
    sub.w d1,d0             ; d0 = Delta Y in lower WORD

.CheckUnderFlowY:
    cmp.w   #-127,d0
    bge     .CheckOverFlowY
    add.w   #255,d0            ; d0 = Delta Y + 255

.CheckOverFlowY:
    cmp.w   #127,d0
    ble     .ReadMouse
    sub.w   #255,d0            ; d0 = Delta Y - 255

.ReadMouse:
    move.l $dfe1d0,d1         ; d1 = read current mouse X from CIA-A
    add.l d0,d1                 ; d1 = new mouse X

.CheckBorderTop:
    cmp.w #0,d1
    bge     .CheckBorderBottom
    move.w #0,d1            ; d1 = 0

.CheckBorderBottom:
    cmp.w #480,d1
    ble     .WriteMouse
    move.w #480,d1            ; d1 = 480

.WriteMouse:
    move.l d1,$dff1d0          ; write new mouse X to CIA-A

.Exit:
    rts

