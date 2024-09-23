

init:
	move.w	$DFF002,D0			; SAVE DMACON
	or.w	D0,DMACON
	move.w	#$7FFF,$DFF096		; ALL DMA OFF
	move.w	$DFF01C,D0			; SAVE INTENA
	or.w	D0,INTENA
	move.w	#$7FFF,$DFF09A		; ALL INTENA OFF
	move.l	$DFE1EC,GFXPTR      ; Save GFXPTR	
	move.w	$DFE1F4,GFXMODE		; Save GFXMode

saga:
	move.l  #picture,$DFF1EC	; Set PictureAddr
	move.w  #$0F02,$DFF1F4      ; Set Gfxmode to 848x480 / 16 bit (R5G6B5)

loop:
	btst #6,$bfe001
	bne loop

exit:
	move.w	#$7FFF,$DFF096
	move.w	DMACON,$DFF096
	move.w	INTENA,$DFF09A
	move.l	GFXPTR,$DFF1EC
	move.w	GFXMODE,$DFF1F4

	rts

	section picture,DATA_F
picture	incbin "RHLOS_848x480x16.raw"
picture_end

DMACON	dc.w $8000
INTENA	dc.w $8000
GFXMODE	dc.w $0000
GFXPTR  dc.l 0