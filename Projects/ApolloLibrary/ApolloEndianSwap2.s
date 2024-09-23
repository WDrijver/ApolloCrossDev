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

