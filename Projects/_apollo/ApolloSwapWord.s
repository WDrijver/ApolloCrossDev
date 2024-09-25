* Apollo Endian Swap Word *

	XDEF _ApolloSwapWord
	CNOP 0,4

_ApolloSwapWord:
	perm #@0032,d0,d0									* swap WORD
	
	rts
