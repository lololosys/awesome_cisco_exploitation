/* $Id: s68562.h,v 3.2 1995/11/17 09:19:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/s68562.h,v $
 *------------------------------------------------------------------
 * ../hes/s68562.h - Signetics Dual Universal Serial Communications Controller
 *
 * September 1987, Greg Satz
 *
 * Copyright (c) 1987-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: s68562.h,v $
 * Revision 3.2  1995/11/17  09:19:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:43:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

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
	volatile u_char ivr;		/* Interrupt Vector Unmodified (A) */
					/* Interrupt Vector Modified (B) */
	volatile u_char icr;		/* Interrupt Control Register (A) */
					/* Not used (B) */
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

#define	CMR1_ASYNC		0x07
#define CMR1_PARITY_OFF		0x00
#define CMR1_PARITY_ON		0x10
#define CMR1_FORCE_PARITY	0x18
#define CMR1_EVEN_PARITY	0x00
#define CMR1_ODD_PARITY		0x20

/*
 * Channel Mode Register 2
 */

#define CMR2_POLLED	0x38
#define	CMR2_LOOP	0x80

/*
 * Transmitter parameter register
 */

#define	TPR_2_STOP_BITS		0xF0
#define TPR_15_STOP_BITS	0x80
#define TPR_1_STOP_BIT		0x70

#define TPR_TX_ENABLE_RTS_CTL	0x08
#define TPR_CTS_ENABLE_TX	0x04

#define TPR_8_DATA_BITS		0x03
#define TPR_7_DATA_BITS		0x02
#define TPR_6_DATA_BITS		0x01
#define TPR_5_DATA_BITS		0x00

/*
 * Transmitter timing register
 */

#define	TTR_CLOCKBRG	0x30
#define TTR_9600_BAUD	0x0D

/*
 * Receiver parameter register
 */

#define RPR_RX_RTS_CTL		0x10
#define RPR_STRIP_PARITY	0x08
#define RPR_DCD_ENABLE_RX	0x04

#define	RPR_8_DATA_BITS		0x03
#define RPR_6_DATA_BITS		0x01
#define RPR_7_DATA_BITS		0x02
#define RPR_5_DATA_BITS		0x00

/*
 * Receiver timing register
 */

#define	RTR_CLOCKBRG	0x20

/*
 * Output and Miscellaneous Register
 */
#define OMR_TXRDY_FIFO_EMPTY	0x10
#define OMR_ASSERT_DTR		0x02
#define OMR_ASSERT_RTS		0x01

/*
 * Pin Configuration register
 */
#define PCR_RTS_NOT_SYNOUT	0x20

/*
 * Channel Command register
 */

#define	CCR_TCMD	0x00   /* Xmitter commands */
#define	CCR_RCMD	0x40   /* Set this bit for Receiver commands */
#define	CCR_RESET	0x00   /* Reset, Cease operation, clear Status */
#define	CCR_ENABLE	0x02   /* Enable operation */
#define	CCR_DISABLE	0x03   /* Disable, after pending FIFO service */
#define	CCR_TBREAK	0x07   /* Transmit Break. Needs to be reset */

/*
 * Receiver Status register
 */

#define	RSR_CHAR_CMP	0x80
#define	RSR_RTS_NEG	0x40
#define	RSR_OVERRUN	0x20
#define	RSR_BRK_END	0x08
#define	RSR_BRK_START	0x04
#define	RSR_FRAME	0x02
#define	RSR_PARITY	0x01

/*
 * Transmitter and Receiver Status register
 */

#define	TRSR_TX_EMPTY	0x80
#define	TRSR_CTS_UNDER	0x40
#define	TRSR_BREAK_ACK	0x10
#define	TRSR_DPLL_ERR	0x08

/*
 * Input and Counter/Timer Status register
 */

#define	ICTSR_CTZERO	0x40	/* Counter/Timer Zero detect */
#define	ICTSR_DDCD	0x20	/* Change in DCD signal detected */
#define	ICTSR_DCTS	0x10	/* Change in CTS signal detected */
#define ICTSR_DCD	0x8	/* Current state of DCD signal */
#define ICTSR_CTS	0x4	/* Current state of CTS signal */
#define ICTSR_GPI2	0x2	/* Current state of GPI2 signal (unused) */
#define ICTSR_RING	0x1	/* Current state of GPI1 signal (RING) */

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

#define	IER_DCD_CTS	0x80
#define	IER_TX_RDY	0x40
#define	IER_TRSR_73	0x20
#define	IER_RX_RDY	0x10
#define	IER_RSR_76	0x08
#define	IER_RSR_54	0x04
#define	IER_RSR_32	0x02
#define	IER_RSR_10	0x01

/*
 * Interrupt control register
 */

#define ICR_INTERLEAVE_B	0xC0
#define ICR_INTERLEAVE_A	0x80
#define ICR_PRIORITY_B		0x40
#define ICR_PRIORITY_A		0x00
#define	ICR_VSTATUS		0x04
#define	ICR_A_ENABLE		0x02
#define	ICR_B_ENABLE		0x01
