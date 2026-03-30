#NO_APP
	.stabs	"/home/willem/ApolloCrossDev/Projects/ol/",100,0,2,.Ltext0
	.stabs	"/home/willem/ApolloCrossDev/Projects/ol/ol.c",100,0,2,.Ltext0
	.text
.Ltext0:
	.stabs	"gcc2_compiled.",60,0,0,0
	.file 1 "/home/willem/ApolloCrossDev/Projects/ol/ol.c"
	.stabs	"/home/willem/ApolloCrossDev/Projects/ol/ol.c",132,0,0,.Ltext1
.Ltext1:
	.stabs	"int:t1=r1;-2147483648;2147483647;",128,0,0,0
	.stabs	"char:t2=r2;0;127;",128,0,0,0
	.stabs	"long int:t3=r3;-2147483648;2147483647;",128,0,0,0
	.stabs	"unsigned int:t4=r4;0;037777777777;",128,0,0,0
	.stabs	"long unsigned int:t5=r5;0;037777777777;",128,0,0,0
	.stabs	"__int128:t6=@s128;r6;02000000000000000000000000000000000000000000;01777777777777777777777777777777777777777777;",128,0,0,0
	.stabs	"__int128 unsigned:t7=@s128;r7;0;03777777777777777777777777777777777777777777;",128,0,0,0
	.stabs	"long long int:t8=@s64;r8;01000000000000000000000;00777777777777777777777;",128,0,0,0
	.stabs	"long long unsigned int:t9=@s64;r9;0;01777777777777777777777;",128,0,0,0
	.stabs	"short int:t10=@s16;r10;-32768;32767;",128,0,0,0
	.stabs	"short unsigned int:t11=@s16;r11;0;65535;",128,0,0,0
	.stabs	"signed char:t12=@s8;r12;-128;127;",128,0,0,0
	.stabs	"unsigned char:t13=@s8;r13;0;255;",128,0,0,0
	.stabs	"float:t14=r1;4;0;",128,0,0,0
	.stabs	"double:t15=r1;8;0;",128,0,0,0
	.stabs	"long double:t16=r1;12;0;",128,0,0,0
	.stabs	"void:t17=17",128,0,0,0
	.section .text
	.align	2
	.globl	_main
_main:
	.stabd	46,0,0
	.loc 1 4 0
	.stabd	68,0,4
.LFBB1:
	move.l a3,-(sp)
	move.l a2,-(sp)
	.loc 1 4 0
	.stabd	68,0,4
	move.l (12,sp),a0
	move.l (16,sp),a1
	move.l (20,sp),d0
	.loc 1 6 0
	.stabd	68,0,6
	tst.w d0
	jle .L1
	subq.w #1,d0
	move.l a0,a3
	and.l #65535,d0
	lea (8,d0.l*8),a2
	sub.l a2,a3
.L3:
	move.w (-2,a0),(a1)+
	subq.l #8,a0
	move.w (4,a0),(a1)+
	move.w (2,a0),(a1)+
	move.w (a0),(a1)+
	.loc 1 6 0
	.stabd	68,0,6
	cmp.l a3,a0
	jne .L3
.L1:
	.loc 1 12 0
	.stabd	68,0,12
	move.l (sp)+,a2
	move.l (sp)+,a3
	rts
	.stabs	"main:F17",36,0,3,_main
	.stabs	"src:p18=*19=k10",160,0,3,12
	.stabs	"dst:p20=*10",160,0,3,16
	.stabs	"count:p1",160,0,3,20
	.stabs	"src:r18",64,0,3,8
	.stabs	"dst:r20",64,0,3,9
.Lscope1:
	.stabs	"",36,0,0,.Lscope1-.LFBB1
	.stabd	78,0,0
