/* $Id: stp.h,v 3.3 1995/11/17 18:56:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/stp.h,v $
 *------------------------------------------------------------------
 * stp.h -- Definitions for the cisco STP eight duart board
 *
 * Copyright (c) 1988-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stp.h,v $
 * Revision 3.3  1995/11/17  18:56:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:45:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:40:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:10:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * All registers in the STP board are one word (16 bits) wide.
 */

typedef struct lineaddress {
    volatile
    unsigned short  modeA,		/* 	00 */
                    statusA,		/* 	02 */
                    commandA,		/* 	04 */
                    dataA,		/* 	06 */
                    portchange,		/* 	08 */
                    interrupt,		/* 	0A */
                    CTUpper,		/* 	0C */
                    CTLower,		/* 	0E */
                    modeB,		/* 	10 */
                    statusB,		/* 	12 */
                    commandB,		/* 	14 */
                    dataB,		/* 	16 */
                    boardstatus,	/* 	18 */
                    portstatus,		/* 	1A */
                    setport,		/* 	1C */
                    resetport;		/* 	1E */
} uartaddress;

typedef struct _hardpointers {
	volatile ushort *boardtxstatus;
	volatile ushort *boardrxstatus;
	volatile ushort *ttystat;
	volatile ushort *ttydata;
	ushort inready;
	ushort outready;
} uartregs;

#define UARTSTART ((uartaddress *)(MULTIMEMBASE+0x40000))

/*
 * Auxiliary Control register
 */

#define CTSAXENB	1		/* Enable interrupts on modem	  */
#define CTSBXENB	2		/* input transitions		  */
#define RINGAXENB	4
#define RINGBXENB	8
#define CTEXT	    	0
#define CTTXCA	    	0x10
#define CTTXCB	    	0x20
#define CTXTAL	    	0x30
#define TIMEXT	    	0x40
#define TIMEXTDIV16 	0x50
#define TIMXTAL	    	0x60
#define TIMXTALDIV16	0x70
#define BRGSET0	    	0
#define BRGSET1	    	0x80

/*
 * Input Port Change register
 */

#define CTSA		1		/* State of inputs from modem	  */
#define CTSB		2
#define RINGA		4
#define RINGB		8
#define CTSAX		16		/* Change of state detected on	  */
#define CTSBX		32		/* on input from modem		  */
#define RINGAX		64
#define RINGBX		128

/*
 * Output Port Configuration register
 */

#define TXCA16X		1	/* Outputs can be either   */
#define TXCA1X		2	/* programmable or status  */
#define RXCA1X		3
#define CTOUT		4
#define TXCB1X		8
#define RXCB1X		12
#define OPRXRDYA	16
#define OPRXRDYB	32
#define OPTXRDYA	64
#define OPTXRDYB	128

/*
 * Output Port Set and Reset registers
 */
#define DTRA		1
#define DTRB		2

#define WHICHUART(linenum)  UARTSTART + ((linenum - STPBase)>>1)
#define OTHERLINE(linenum) ((linenum & 1) ? linenum + 1 : linenum - 1)


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

extern int nSTPlines;
extern int STPBase;

/*
 * ts/tty2681.c
 */
extern int STP_baud2code(int);
extern void STP_txdisable(tt_soc *);
extern void modem_scanner(void);
