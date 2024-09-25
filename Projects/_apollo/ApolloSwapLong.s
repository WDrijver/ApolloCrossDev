* Apollo Endian Swap Long *

	XDEF _ApolloSwapLong
	CNOP 0,4

_ApolloSwapLong:
	perm #@3210,d0,d0									* swap LONG
	
	rts
