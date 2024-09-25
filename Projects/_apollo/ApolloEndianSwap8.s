*********************************************************
* Apollo Endian Swap (8-byte Chunks)				 	*
*********************************************************
* a0 = s = buffer source pointer 						*
* d0 = l = buffer length	 							*
*********************************************************

	XDEF _ApolloEndianSwap8
	CNOP 0,4

_ApolloEndianSwap8:
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

