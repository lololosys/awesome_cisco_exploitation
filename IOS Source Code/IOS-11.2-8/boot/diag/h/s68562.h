/* $Id: s68562.h,v 3.1 1995/11/09 09:06:44 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/s68562.h,v $
 *------------------------------------------------------------------
 * s68562.h - Signetics Dual Universal Serial Communications Controller
 *
 * September 1987, Greg Satz
 *
 * Copyright (c) 1987-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: s68562.h,v $
 * Revision 3.1  1995/11/09  09:06:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:51  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	UARTDEVBASEADDR	0x02120100
#define	uartaddr	((struct uartdevice *)UARTDEVBASEADDR)

struct regpair {
	volatile u_char cmr1;		/* Channel Mode 1 */
	volatile u_char cmr2;		/* Channel Mode 2 */
	volatile u_char s1r;		/* SYN 1/Secondary address 1 */
	volatile u_char s2r;		/* SYN 2/Secondary address 2 */
	volatile u_char tpr;		/* Transmitter Parameter */
	volatile u_char ttr;		/* Transmitter timing */
	volatile u_char rpr;		/* Receiver Parameter */
	volatile u_char rtr;		/* Receiver timing */
	volatile u_char ctprh;		/* Counter/Timer Preset High */
	volatile u_char ctprl;		/* Counter/Timer Preset Low */
	volatile u_char ctcr;		/* Counter/Timer Control */
	volatile u_char omr;		/* Output and Misc. */
	volatile u_char cth;		/* Counter/Timer High */
	volatile u_char ctl;		/* Counter/Timer Low */
	volatile u_char pcr;		/* Pin Configuration */
	volatile u_char ccr;		/* Channel Command */
	volatile u_char txfifo[4];	/* Transmitter FIFO */
	volatile u_char rxfifo[4];	/* Receiver FIFO */
	volatile u_char rsr;		/* Receiver Status */
	volatile u_char trsr;		/* Transmitter and Receiver Status */
	volatile u_char ictsr;		/* Input and Counter/Timer Status */
	volatile u_char gsr;		/* General Status */
	volatile u_char ier;		/* Interrupt Enable */
	volatile u_char fill1;		/* Not Used */
	volatile u_char ivr;		/* Interrupt Vector Unmodified */
					/* Interrupt Vector Modified */
	volatile u_char icr;		/* Interrupt Control Register */
					/* Not used */
};

struct uartdevice {
	struct regpair a;		/* channel A */
	struct regpair b;		/* channel B */
};

/*
 * Register and bit definitions
 */

/*
 * Channel Mode Register 1
 */

#define	CMR1_NRZ	0x00
#define	CMR1_FM0	0xC0
#define	CMR1_ASYNC	0x07

/*
 * Channel Mode Register 2
 */

#define	CMR2_LOOP	0x80
#define CMR2_POLLED	0x38

/*
 * Transmitter parameter register
 */

#define	TPR_2STOPS	0xF0
#define TPR_8BITS	0x03

/*
 * Transmitter timing register
 */

#define	TTR_CLOCKBRG	0x30

/*
 * Receiver parameter register
 */

#define	RPR_8BITS	0x03

/*
 * Receiver timing register
 */

#define	RTR_CLOCKBRG	0x20

/*
 * Channel Command register
 */

#define	CCR_TCMD	0x00
#define	CCR_RCMD	0x40
#define	CCR_RESET	0x00
#define	CCR_ENABLE	0x02
#define	CCR_DISABLE	0x03
#define	CCR_TBREAK	0x07

/*
 * Receiver Status register
 */

#define	RSR_CHARCMP	0x80
#define	RSR_RTSNEG	0x40
#define	RSR_OVERRUN	0x20
#define	RSR_BRKEND	0x08
#define	RSR_BRKSTART	0x04
#define	RSR_FRAME	0x02
#define	RSR_PARITY	0x01

/*
 * Transmitter and Receiver Status register
 */

#define	TRSR_TEMPTY	0x80
#define	TRSR_CTSUNDER	0x40
#define	TRSR_BREAKACK	0x10
#define	TRSR_DPLLERR	0x08

/*
 * Input and Counter/Timer Status register
 */

#define	ICTSR_CTZERO	0x40
#define	ICTSR_DDCD	0x20
#define	ICTSR_DCTS	0x10

/*
 * General Status register
 */

#define	GSR_B_TXRDY	0x20
#define	GSR_B_RXRDY	0x10
#define	GSR_A_TXRDY	0x02
#define	GSR_A_RXRDY	0x01

/*
 * Interrupt enable register
 */

#define	IER_DCDCTS	0x80
#define	IER_TXRDY	0x40
#define	IER_TRSR73	0x20
#define	IER_RXRDY	0x10
#define	IER_RSR76	0x08
#define	IER_RSR54	0x04
#define	IER_RSR32	0x02
#define	IER_RSR10	0x01

/*
 * Interrupt control register
 */

#define	ICR_VSTATUS	0x04
#define	ICR_AENABLE	0x02
#define	ICR_BENABLE	0x01
