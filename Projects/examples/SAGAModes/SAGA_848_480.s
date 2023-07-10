        move.l  #pic,$DFF1EC         	; Set PictureAddr
        move.w  #$0F02,$DFF1F4          ; Set Gfxmode to 848x480 / 16 bit (R5G6B5)


       section pic,DATA_F
pic    incbin  "RHLOS_848x480x16.raw"
pic_e