/* $Id: console.h,v 3.1 1995/11/09 09:09:31 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c4000/console.h,v $
 *------------------------------------------------------------------
 * console.h -- ROM monitor console for Pancake using 2681 DUART
 *
 * October 19, 1989, Chris Shaker
 *
 * Copyright (c) 1992-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: console.h,v $
 * Revision 3.1  1995/11/09  09:09:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:33:46  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.2  1993/07/01  23:11:23  hampton
 * Bring modular system to Aries 9.21(0.12) patch level.
 *
 * Revision 1.1  1992/04/15  02:11:32  clev
 * Initial checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:08  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../sys/ts/s2681.h"

extern u_char UARTspeeds[];

/*
 * Registers and bits:
 */

#define UARTDEVBASEADDR	0x2120100
#define uartaddr	((struct uartdevice *)UARTDEVBASEADDR)

struct regpair {
    volatile u_char mode;	/* MR1A/MR2A, MR1B/MR2B */
    volatile u_char state;	/* SRA/CSRA, SRB/CSRB */
    volatile u_char command;	/* CRA, CRB */
    volatile u_char data;	/* RHRA/THRA, RHRB/THRB */
    volatile u_char aux1;	/* A=IPCR/ACR, B=reserved */
    volatile u_char aux2;	/* A=ISR/IMR, B=IPR/OPCR */
    volatile u_char aux3;	/* A=CTU/CTUR, B=Start Counter/Set OPR */
    volatile u_char aux4;	/* A=CTL/CTLR, B=Stop Counter/Reset OPR */
};

struct uartdevice {
    struct regpair a;		/* channel a */
    struct regpair b;		/* channel b */
};

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

/*
 * Basic IO primitives needed when initializing (since you can't
 * yet trust memory).
 */

#define PUTSTRING(str,arg) \
    str = arg; \
    while (*str) { \
        while ((uartaddr->a.state & TXREADY) == 0) \
	    ; \
	uartaddr->a.data = *str++; \
    }

#define PUTDIGIT(digit,shift) \
    while ((uartaddr->a.state & TXREADY) == 0) \
        ; \
    uartaddr->a.data = digits[((digit >> shift) & 0xF)];

#define PUTHEXNUM(hexnum) \
    PUTDIGIT(hexnum,28); \
    PUTDIGIT(hexnum,24); \
    PUTDIGIT(hexnum,20); \
    PUTDIGIT(hexnum,16); \
    PUTDIGIT(hexnum,12); \
    PUTDIGIT(hexnum,8); \
    PUTDIGIT(hexnum,4); \
    PUTDIGIT(hexnum,0);
