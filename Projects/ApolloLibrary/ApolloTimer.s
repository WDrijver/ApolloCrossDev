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

