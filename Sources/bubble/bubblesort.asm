	idnt	"bubblesort.c"
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	section	"CODE",code
	public	_bubbleSort
	cnop	0,4
_bubbleSort
	movem.l	l22,-(a7)
	move.w	(10+l24,a7),d3
	move.l	(4+l24,a7),a5
l18
	moveq	#0,d0
	move.l	a5,a2
	move.w	d3,d2
	subq.w	#1,d2
	move.w	d2,d1
	tst.w	d2
	beq	l21
l19
	lea	(2,a2),a1
	move.w	(a2),a4
	move.w	(a1),a3
	cmp.w	a4,a3
	bge	l10
	addq.w	#1,d0
	move.l	a2,a0
	move.l	a1,a2
	move.w	a3,(a0)
	move.w	a4,(a1)
	bra	l11
l10
	move.l	a1,a2
l11
	subq.w	#1,d1
	bne	l19
l21
	move.w	d2,d3
	tst.w	d0
	bne	l18
l22	reg	a2/a3/a4/a5/d2/d3
	movem.l	(a7)+,a2/a3/a4/a5/d2/d3
l24	equ	24
	rts
; stacksize=24
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	public	_main
	cnop	0,4
_main
	sub.w	#12,a7
	movem.l	l40,-(a7)
	moveq	#5,d3
	move.l	#3276812,(0+l42,a7)
	move.l	#1966087,(4+l42,a7)
	move.l	#65536,(8+l42,a7)
	move.l	d3,-(a7)
	lea	(4+l42,a7),a2
	move.l	a2,-(a7)
	jsr	_bubbleSort
	moveq	#0,d2
	addq.w	#8,a7
l38
	move.w	d2,d0
	lsl.w	#1,d0
	and.l	#65535,d0
	move.w	(0,a2,d0.l),d1
	ext.l	d1
	move.l	d1,a0
	move.l	a0,-(a7)
	pea	l32
	jsr	_printf
	addq.l	#1,d2
	addq.w	#8,a7
	cmp.l	d2,d3
	bgt	l38
	pea	l33
	jsr	___v0printf
	moveq	#0,d0
	addq.w	#4,a7
l40	reg	a2/d2/d3
	movem.l	(a7)+,a2/d2/d3
l42	equ	12
	add.w	#12,a7
	rts
	cnop	0,4
l32
	dc.b	37
	dc.b	100
	dc.b	32
	dc.b	0
	cnop	0,4
l27
	dc.w	50
	dc.w	12
	dc.w	30
	dc.w	7
	dc.w	1
	ds.b	2
	cnop	0,4
l33
	dc.b	10
	dc.b	0
	public	_printf
	public	___v0printf
