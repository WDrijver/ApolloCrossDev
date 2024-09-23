#NO_APP
	.text
	.even
	.globl	_ApolloBlitter

_ApolloBlitter:
	movem.l #0x3c00,sp@-
	move.l sp@(28),a1
	move.l sp@(32),a0
	move.w sp@(26),d5
	move.w sp@(22),d2
	move.l sp@(40),d4
	add.l d4,d4
	move.l sp@(36),d3
	add.l d3,d3
	move.w #0x0010,0xDFF1FC

.wait:
	btst #6,0xDFF002
	bne.s .wait
	move.l a1,0xDFF050 
	move.l a0,0xDFF054
	move.w d3,0xDFF064
	move.w d4,0xDFF066
	move.l #0x09F00020,0xDFF040
	move.w d2,0xDFF05C
	move.w d5,0xDFF05E
	movem.l sp@+,#0x3c
	rts
