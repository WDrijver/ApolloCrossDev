*************************************************************
* Apollo Wait VBL										 	*
*************************************************************

	XDEF _ApolloWaitVBL

_ApolloWaitVBL:

wait:
    btst #5,$dff01f
    beq wait
    move.w #$20,$dff09c

    rts