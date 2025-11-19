VERSION = 1
REVISION = 2

.macro DATE
.ascii "24.1.2012"
.endm

.macro VERS
.ascii "XZ 1.2"
.endm

.macro VSTRING
.ascii "XZ 1.2 (24.1.2012)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: XZ 1.2 (24.1.2012)"
.byte 0
.endm
