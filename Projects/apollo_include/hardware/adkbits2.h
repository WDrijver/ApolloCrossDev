#ifndef	VAMPIRE_ADKBITS2_H
#define	VAMPIRE_ADKBITS2_H
/*	
**	$VER: adkbits2.h 0.1 (15.11.2020)
**	Vampire software development kit
**	
**	Bit definitions for adkcon2 register
**	
**	(C) Copyright 2015-2020 APOLLO-Team
**	    All Right Reserved
*/

#ifndef CUSTOM_REGBASE
#define CUSTOM_REGBASE       (0xDFF000)
#endif

#define ADKCONR1             (CUSTOM_REGBASE + 0x010)
#define ADKCONR2             (CUSTOM_REGBASE + 0x010 + 0x200)
#define ADKCON1              (CUSTOM_REGBASE + 0x09E)
#define ADKCON2              (CUSTOM_REGBASE + 0x09E + 0x200)

#define ADK2B_SETCLR         (15)    /* standard set/clear */
#define ADK2B_BIT14          (14)    /* unused             */
#define ADK2B_BIT13          (13)    /* unused             */
#define ADK2B_BIT12          (12)    /* unused             */
#define ADK2B_BIT11          (11)    /* unused             */
#define ADK2B_BIT10          (10)    /* unused             */
#define ADK2B_BIT9           (9)     /* unused             */
#define ADK2B_BIT8           (8)     /* unused             */
#define ADK2B_BIT7           (7)     /* unused             */
#define ADK2B_BIT6           (6)     /* unused             */
#define ADK2B_BIT5           (5)     /* unused             */
#define ADK2B_BIT4           (4)     /* unused             */
#define ADK2B_BIT3           (3)     /* unused             */
#define ADK2B_BIT2           (2)     /* unused             */
#define ADK2B_BIT1           (1)     /* unused             */
#define ADK2B_BIT0           (0)     /* unused             */

#define ADK2F_SETCLR         (1L<<ADK2B_SETCLR)
#define ADK2F_BIT14          (1L<<ADK2B_BIT14)
#define ADK2F_BIT13          (1L<<ADK2B_BIT13)
#define ADK2F_BIT12          (1L<<ADK2B_BIT12)
#define ADK2F_BIT11          (1L<<ADK2B_BIT11)
#define ADK2F_BIT10          (1L<<ADK2B_BIT10)
#define ADK2F_BIT9           (1L<<ADK2B_BIT9)
#define ADK2F_BIT8           (1L<<ADK2B_BIT8)
#define ADK2F_BIT7           (1L<<ADK2B_BIT7)
#define ADK2F_BIT6           (1L<<ADK2B_BIT6)
#define ADK2F_BIT5           (1L<<ADK2B_BIT5)
#define ADK2F_BIT4           (1L<<ADK2B_BIT4)
#define ADK2F_BIT3           (1L<<ADK2B_BIT3)
#define ADK2F_BIT2           (1L<<ADK2B_BIT2)
#define ADK2F_BIT1           (1L<<ADK2B_BIT1)
#define ADK2F_BIT0           (1L<<ADK2B_BIT0)

#endif	/* VAMPIRE_ADKBITS2_H */
