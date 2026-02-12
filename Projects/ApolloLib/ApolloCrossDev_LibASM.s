***************************************************
* ApolloCrossDev Assembler Library                *
* 6-2-2026                                        *
***************************************************

* 1. ApolloFill		= Fill Destination with value (Generic)
* 2. ApolloCopyPicture		= Copy Source to Destination (Generic)
* 3. ApolloCopyPicture32	= Copy Source to Destination (32-Byte Aligned)

***************************************************
* ApolloFill                                      *
***************************************************
* a0 = dst = destination pointer                  *
* d3 = w = blitbox width in Pixels                *
* d4 = h = blitbox height in Pixels               *
* d5 = d = blitbox colordepth in Bits             *
* d6 = dstmod = destination modulo in Pixels      *
* d7 = value in uint32_t (4 Bytes / 32 Bits)      *
***************************************************

	XDEF _ApolloFill
	CNOP 0,4

_ApolloFill:
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

	