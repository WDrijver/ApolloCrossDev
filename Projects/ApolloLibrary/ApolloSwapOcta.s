* Apollo Endian Swap Octa *

	XDEF _ApolloSwapOcta
	CNOP 0,4

_ApolloSwapOcta:
	vperm #$76543210,d0,d0,d0							* swap OCTA

	rts
