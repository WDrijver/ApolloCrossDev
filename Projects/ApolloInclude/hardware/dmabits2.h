#ifndef	VAMPIRE_DMABITS2_H
#define	VAMPIRE_DMABITS2_H
/*	
**	$VER: dmabits2.h 0.1 (15.11.2020)
**	Vampire software development kit
**	
**	Bit definitions for dmacon2 register
**	
**	(C) Copyright 2015-2020 APOLLO-Team
**	    All Right Reserved
*/

#ifndef CUSTOM_REGBASE
#define CUSTOM_REGBASE       (0xDFF000)
#endif

#define DMACONR1             (CUSTOM_REGBASE + 0x002)
#define DMACONR2             (CUSTOM_REGBASE + 0x002 + 0x200)
#define DMACON1              (CUSTOM_REGBASE + 0x096)
#define DMACON2              (CUSTOM_REGBASE + 0x096 + 0x200)

#define DMA2B_SETCLR         (15)    /* standard set/clear */
#define DMA2B_BIT14          (14)    /* unused             */
#define DMA2B_BIT13          (13)    /* unused             */
#define DMA2B_BIT12          (12)    /* unused             */
#define DMA2B_BIT11          (11)    /* unused             */
#define DMA2B_BIT10          (10)    /* unused             */
#define DMA2B_BIT9           (9)     /* unused             */
#define DMA2B_BIT8           (8)     /* unused             */
#define DMA2B_BIT7           (7)     /* unused             */
#define DMA2B_BIT6           (6)     /* unused             */
#define DMA2B_BIT5           (5)     /* unused             */
#define DMA2B_BIT4           (4)     /* unused             */
#define DMA2B_AUD7           (3)     /* audio channel 7    */
#define DMA2B_AUD6           (2)     /* audio channel 6    */
#define DMA2B_AUD5           (1)     /* audio channel 5    */
#define DMA2B_AUD4           (0)     /* audio channel 4    */

#define DMA2F_SETCLR         (1L<<DMA2B_SETCLR)
#define DMA2F_BIT14          (1L<<DMA2B_BIT14)
#define DMA2F_BIT13          (1L<<DMA2B_BIT13)
#define DMA2F_BIT12          (1L<<DMA2B_BIT12)
#define DMA2F_BIT11          (1L<<DMA2B_BIT11)
#define DMA2F_BIT10          (1L<<DMA2B_BIT10)
#define DMA2F_BIT9           (1L<<DMA2B_BIT9)
#define DMA2F_BIT8           (1L<<DMA2B_BIT8)
#define DMA2F_BIT7           (1L<<DMA2B_BIT7)
#define DMA2F_BIT6           (1L<<DMA2B_BIT6)
#define DMA2F_BIT5           (1L<<DMA2B_BIT5)
#define DMA2F_BIT4           (1L<<DMA2B_BIT4)
#define DMA2F_AUD7           (1L<<DMA2B_AUD7)
#define DMA2F_AUD6           (1L<<DMA2B_AUD6)
#define DMA2F_AUD5           (1L<<DMA2B_AUD5)
#define DMA2F_AUD4           (1L<<DMA2B_AUD4)
#define DMA2F_AUDIO          (DMA2F_AUD4|DMA2F_AUD5|DMA2F_AUD6|DMA2F_AUD7) /* all audio channels */

#endif	/* VAMPIRE_DMABITS2_H */
