#ifndef	VAMPIRE_INTBITS2_H
#define	VAMPIRE_INTBITS2_H
/*	
**	$VER: intbits2.h 0.1 (15.11.2020)
**	Vampire software development kit
**	
**	Bit definitions for intena2 and intreq2 registers
**	
**	(C) Copyright 2015-2020 APOLLO-Team
**	    All Right Reserved
*/

#ifndef CUSTOM_REGBASE
#define CUSTOM_REGBASE       (0xDFF000)
#endif

#define INTENAR1             (CUSTOM_REGBASE + 0x01C)
#define INTENAR2             (CUSTOM_REGBASE + 0x01C + 0x200)
#define INTENA1              (CUSTOM_REGBASE + 0x09A)
#define INTENA2              (CUSTOM_REGBASE + 0x09A + 0x200)

#define INTREQR1             (CUSTOM_REGBASE + 0x01E)
#define INTREQR2             (CUSTOM_REGBASE + 0x01E + 0x200)
#define INTREQ1              (CUSTOM_REGBASE + 0x09C)
#define INTREQ2              (CUSTOM_REGBASE + 0x09C + 0x200)

#define INT2B_SETCLR         (15)    /* standard set/clear             */
#define INT2B_BIT14          (14)    /* unused                         */
#define INT2B_ETHERNET       (13)    /* ethernet block finished        */
#define INT2B_BIT12          (12)    /* unused                         */
#define INT2B_BIT11          (11)    /* unused                         */
#define INT2B_BIT10          (10)    /* unused                         */
#define INT2B_BIT9           (9)     /* unused                         */
#define INT2B_BIT8           (8)     /* unused                         */
#define INT2B_BIT7           (7)     /* unused                         */
#define INT2B_BIT6           (6)     /* unused                         */
#define INT2B_BIT5           (5)     /* unused                         */
#define INT2B_BIT4           (4)     /* unused                         */
#define INT2B_AUD7           (3)     /* audio channel 7 block finished */
#define INT2B_AUD6           (2)     /* audio channel 6 block finished */
#define INT2B_AUD5           (1)     /* audio channel 5 block finished */
#define INT2B_AUD4           (0)     /* audio channel 4 block finished */

#define INT2F_SETCLR         (1L<<INT2B_SETCLR)
#define INT2F_BIT14          (1L<<INT2B_BIT14)
#define INT2F_ETHERNET       (1L<<INT2B_ETHERNET)
#define INT2F_BIT12          (1L<<INT2B_BIT12)
#define INT2F_BIT11          (1L<<INT2B_BIT11)
#define INT2F_BIT10          (1L<<INT2B_BIT10)
#define INT2F_BIT9           (1L<<INT2B_BIT9)
#define INT2F_BIT8           (1L<<INT2B_BIT8)
#define INT2F_BIT7           (1L<<INT2B_BIT7)
#define INT2F_BIT6           (1L<<INT2B_BIT6)
#define INT2F_BIT5           (1L<<INT2B_BIT5)
#define INT2F_BIT4           (1L<<INT2B_BIT4)
#define INT2F_AUD7           (1L<<INT2B_AUD7)
#define INT2F_AUD6           (1L<<INT2B_AUD6)
#define INT2F_AUD5           (1L<<INT2B_AUD5)
#define INT2F_AUD4           (1L<<INT2B_AUD4)

#endif	/* VAMPIRE_INTBITS2_H */
