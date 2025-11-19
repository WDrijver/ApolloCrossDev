VERSION = 2
REVISION = 4

.macro DATE
.ascii "18.9.2010"
.endm

.macro VERS
.ascii "7-Zip 2.4"
.endm

.macro VSTRING
.ascii "7-Zip 2.4 (18.9.2010)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: 7-Zip 2.4 (18.9.2010)"
.byte 0
.endm
