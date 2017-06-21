/* $Id: s2681.h,v 3.2 1995/11/17 18:56:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/s2681.h,v $
 *------------------------------------------------------------------
 * s2681.h  --  Signetics 2681 DUART definitions
 *
 * Copyright (c) 1986-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: s2681.h,v $
 * Revision 3.2  1995/11/17  18:56:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:10:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Mode register #1 bit definitions
 */

#define DBITS5		0
#define DBITS6		1
#define DBITS7		2
#define DBITS8		3

#define PRTYODD		4		/* Only applies when parity is on */
#define PRTYEVEN	0

#define PRTYON		0
#define PRTYFORCE	8
#define PRTYOFF		16

#define MULTIDROP	24		/* weird operating mode - ignore  */

#define CHARERROR	0		/* received character error mode  */
#define BLOCKERROR	32

#define RXRDYINTR	0		/* interrupt on RxRdy		  */
#define RXFULLINTR	64		/* interrupt on Rx Fifo Full	  */

#define RXRTSCONTROL	128		/* RTS will drop if the Fifo fills*/


/*
 * Mode register #2 bit definitions
 */

#define STOP9X16	0		/* Stop bit(s) range from:	   */
#define STOP10X16	1		/* 9/16 to 1, and 25/16 to 2	   */
#define STOP11X16	2
#define STOP12X16	3
#define STOP13X16	4
#define STOP14X16	5
#define STOP15X16	6
#define STOP16X16	7
#define STOP25X16	8
#define STOP26X16	9
#define STOP27X16	10
#define STOP28X16	11
#define STOP29X16	12
#define STOP30X16	13
#define STOP31X16	14
#define STOP32X16	15

#define TXCTSCONTROL	16		/* To transmit, CTS must be high  */
#define TXRTSCONTROL	32		/* RTS will drop if Tx is disabled */

#define MODENORMAL	0
#define MODEECHO	64
#define MODELOCALLOOP	128
#define MODEREMOTELOOP	192


/*
 * Clock Select register bit definitions
 */

#define TXCLOCK		1	/* use these as multipliers:		   */
#define RXCLOCK		16	/* txclock*S0baud50 + rxclock*S0baud50	   */

#define S0BAUD50	0
#define S0BAUD110	1
#define S0BAUD134	2		/* The S0 or S1 refers to the baud */
#define S0BAUD200	3		/* rate generator set select bit,  */
#define S0BAUD300	4		/* ACR bit 07.			   */
#define S0BAUD600	5
#define S0BAUD1200	6
#define S0BAUD1050	7
#define S0BAUD2400	8
#define S0BAUD4800	9
#define S0BAUD7200	10
#define S0BAUD9600	11
#define S0BAUD38400	12
#define S0BAUDTIMER     13
#define S0BAUDEXT16X    14
#define S0BAUDEXT1X     15

#define S1BAUD75	0
#define S1BAUD110	1
#define S1BAUD134	2
#define S1BAUD150	3
#define S1BAUD300	4
#define S1BAUD600	5
#define S1BAUD1200	6
#define S1BAUD2000	7
#define S1BAUD2400	8
#define S1BAUD4800	9
#define S1BAUD1800	10
#define S1BAUD9600	11
#define S1BAUD19200	12
#define S1BAUDTIMER     13
#define S1BAUDEXT16X    14
#define S1BAUDEXT1X     15

/*
 * Status register bit definitions
 */
#define RXREADY		1	/* Rx and Tx status	   */
#define FIFOFULL	2
#define TXREADY		4
#define TXEMPTY		8
#define OVRRNERROR	16
#define PRTYERROR	32	/* Fifo character status   */
#define FRMERROR	64
#define RCVBREAK	128


/*
 * Command register bit definitions
 */

#define STP_RXENABLE	1		/* Rx and Tx enable and disable	*/
#define RXDISABLE	2
#define STP_TXENABLE	4
#define TXDISABLE	8
#define RESETMRPTR	16		/* channel commands */
#define RESETRX		32
#define RESETTX		48
#define RESETERROR	64
#define RESETDBREAK	80
#define STARTBREAK	96
#define STOPBREAK	112

/*
 * Interrupt Status and Mask register
 */
#define TXRDYA		1
#define RXRDYA		2
#define BREAKXA		4
#define CNTRREADY	8
#define TXRDYB		16
#define RXRDYB		32	/* Fifo character status   */
#define BREAKXB		64
#define MODEMX		128

/*
 * OPR bits:
 */
#define CDA		0x01	/* Port A Carrier detect */

/*
 * ACR register (write to a.aux1):
 */

#define CTEXT	    	0	/* External clock */
#define CTTXCA	    	0x10	/* TxCA 1X A clock */
#define CTTXCB	    	0x20	/* TxCB 1X B clock */
#define CTXTAL	    	0x30	/* Crystal or External clock divided by 16 */
#define TIMEXT	    	0x40	/* External IP2 1X */
#define TIMEXTDIV16 	0x50	/* External IP2 16X */
#define TIMXTAL	    	0x60	/* Crystal or External clock (X1/CLK) */
#define TIMXTALDIV16	0x70	/* Crystal or External clock (X1/CLK) / 16 */
#define BRGSET0	    	0	/* Baud rate set 0 */
#define BRGSET1	    	0x80	/* Baud rate set 1 */

#ifdef __TTYSRV_H__
/*
 * Prototypes
 */
extern void STP_vector_init(void);
extern vectortype STPvector;
extern uchar uartstatus[];

#endif          /*  __TTYSRV_H__*/
