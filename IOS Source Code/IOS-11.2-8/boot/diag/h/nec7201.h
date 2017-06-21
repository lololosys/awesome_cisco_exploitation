/* $Id: nec7201.h,v 3.1 1995/11/09 09:06:25 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/nec7201.h,v $
 *------------------------------------------------------------------
 * nec7201.h -- NEC PD7201 UART
 *
 * 18-June-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nec7201.h,v $
 * Revision 3.1  1995/11/09  09:06:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:48  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define PORTA 0
#define PORTB 1

#ifndef MPSC

	/*
	 * Definitions for the CSC1 processor
	 */
#define DEVADR 0x600000
#define linedata(line) *((volatile char*)DEVADR+line*4)
#define linecontrol(line) *((volatile char*)DEVADR+2+line*4)

#else

	/*
	 * Definitions for the SBE processor
	 */
#define linedata(line) *((volatile char*)MPSC+(line)*2)
#define linecontrol(line) *((volatile char*)MPSC+4+(line)*2)

#endif

/*
 * The 7201 has two similar channels.  The following definitions apply
 * to registers for either channel unless otherwise noted.
 */

#define WR0	0		/* Write registers */
#define WR1	1
#define WR2	2
#define WR3	3
#define WR4	4
#define WR5	5
#define WR6	6
#define WR7	7

#define RR0	0		/* Read registers */
#define RR1	1
#define RR2	2

/* Read Register 0 */

#define RR0_RXRDY	0x01	/* rx char avail */
#define RR0_INTPND	0x02	/* interrupt pending (chn A only) */
#define RR0_TXRDY	0x04	/* tx buffer empty */
#define RR0_DCD		0x08	/* carrier detect */
#define RR0_SYNC	0x10	/* sync/hunt */
#define RR0_CTS		0x20	/* CTS */
#define RR0_TXUNDER	0x40	/* tx underrun */
#define	RR0_BREAK	0x80	/* break/abort detected */

/* Read Register 1 */
#define RR1_ALLSENT	0x01	/* all sent */
#define RR1_PARERROR	0x10	/* parity error */
#define RR1_RXOVER	0x20	/* rx overrun */
#define RR1_FRAMERR	0x40	/* CRC or framing error */
#define RR1_EOF		0x80	/* end of frame (SDLC mode) */

/* Read Register 2 */
#define BTXINT		0	/* B port tx interrupt */ 
#define BEXTINT		1	/* B port external status change */
#define BRXINT		2	/* B port rx interrupt */
#define BSPINT		3	/* B port rx special interrupt */
#define ATXINT		4	/* A port tx interrupt */ 
#define AEXTINT		5	/* A port external status change */
#define ARXINT		6	/* A port rx interrupt */
#define ASPINT		7	/* A port rx special interrupt */



/* Write Register 0 */
#define WR0_SABORT	0x08	/* send abort (SDLC) */
#define WR0_RESETXST	0x10	/* reset external/status interrupts */
#define WR0_CHRESET	0x18	/* channel reset */
#define WR0_RXNEXTCH	0x20	/* enable int on next rx char */
#define WR0_TXRESET	0x28	/* tx interrupt reset */
#define WR0_ERRRESET	0x30	/* error reset */
#define WR0_EOI		0x38	/* end of interrupt */
#define WR0_RXCRCRESET	0x40	/* reset rx crc checker */
#define WR0_TXCRCRESET	0x80	/* reset tx crc generator */
#define WR0_TXUNDER	0xC0	/* reset tx underflow latch */

/* Write register 1 */
#define WR1_EXTINT	0x01	/* external interrupt enable */
#define WR1_TXINT	0x02	/* tx interrupt enable */
#define WR1_STATUS	0x04	/* status affects vector (cf. WR2, chn B) */
#define WR1_RXINT1	0x08	/* rx int on first char or special */
#define WR1_RXINT2	0x10	/* rx int on all cond, parity */
#define WR1_RXINT	0x18	/* rx int on all characters, no parity */
#define WR1_RXWAIT	0x20	/* wait on rx, 0 to wait on tx */
#define	WR1_RXWAITENA	0x80	/* wait enable */

/* Write Register 2 (chn A only) */
#define WR2_INTAB	0x00	/* both A and B use interrupts */
#define WR2_DMAA	0x01	/* A uses DMA, B uses int */
#define WR2_DMAAB	0x02	/* both A and B use DMA */
#define WR2_ABPRIOR	0x04	/* adjust A and B int priorities */

#define WR2_8085_1	0x00	/* 8085 mode 1 */
#define WR2_8085_2	0x08	/* 8085 mode 2 */
#define WR2_8086	0x10	/* 8086 mode */
#define WR2_VECTORED	0x20	/* vectored interrupts */

#define WR2_SYNDET	0x80	/* SYNDET */

/* Write Register 3 */
#define WR3_RXENA	0x01	/* rx enable */
#define WR3_SYNCINH	0x02	/* sync char load inhibit */
#define WR3_ADDSRCH	0x04	/* address search mode */
#define WR3_RXCRCENA	0x08	/* rx CRC enable */
#define WR3_HUNT	0x10	/* enter hunt mode */
#define WR3_AUTOE	0x20	/* auto enable */

#define WR3_RX5BT	0x00	/* rx 5 bit chars */
#define WR3_RX7BT	0x40	/* rx 7 bit chars */
#define WR3_RX6BT	0x80	/* rx 6 bit chars */
#define WR3_RX8BT	0xC0	/* Rx 8 bit characters */

/* Write Register 4 */
#define WR4_PARENA	0x01	/* parity enable */
#define WR4_PAREVEN	0x02	/* even parity (off -> odd parity) */

#define WR4_SYNC	0x00	/* sync modes */
#define WR4_RX1SB	0x04	/* rx 1 stop bit */
#define WR4_RX15SB	0x08	/* rx 1.5 stop bits */
#define WR4_RX2SB	0x0C	/* rx 2 stop bit */

#define WR4_SYNC8	0x00	/* 8 bit sync char */
#define WR4_SYNC16	0x10	/* 16 bit sync char */
#define WR4_SDLC	0x20	/* SDLC mode */
#define WR4_SYNCEXT	0x30	/* external sync mode */

#define WR4_1CLK	0x00	/* external clock mode */
#define WR4_16CLK	0x40	/* 16x clock mode */
#define WR4_32CLK	0x40	/* 32x clock mode */
#define WR4_64CLK	0x40	/* 64x clock mode */

/* Write Register 5 */
#define WR5_TXCRCENA	0x01	/* tx CRC enable */
#define WR5_TXRTS	0x02	/* RTS */
#define WR5_SDLCCRC	0x04	/* SDLC/CRC-16 select */
#define WR5_TXENA	0x08	/* tx enable */
#define	WR5_SNDBRK	0x10	/* send "break" on Tx */

#define WR5_TX5BT	0x00	/* tx 5 bit chars */
#define WR5_TX7BT	0x20	/* tx 7 bit chars */
#define WR5_TX6BT	0x40	/* tx 6 bit chars */
#define WR5_TX8BT	0x60	/* tx 8 bit chars */

#define WR5_DTR		0x80	/* Data Terminal Ready */

/* Write Register 6 */
/*
 * address byte in SDLC mode.
 */
#define WR6_SYNC	0x00	/* sync character is all zeros */

/* Write Register 7 */
/*
 * flag char in SDLC mode.
 */
#define WR7_FLAG	0x7E	/* flag character in sync mode */
