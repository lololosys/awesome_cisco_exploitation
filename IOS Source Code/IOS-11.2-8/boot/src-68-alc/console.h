/* $Id: console.h,v 3.1.58.1 1996/05/09 13:54:17 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/src-68-alc/console.h,v $
 *------------------------------------------------------------------
 * console.h -- ROM monitor console for Synergy ATM line card using
 *		MC 68901 MFP/UART (file organization based on c3000).
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: console.h,v $
 * Revision 3.1.58.1  1996/05/09  13:54:17  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.1.88.1  1996/04/27  06:13:17  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.78.1  1996/04/08  01:34:46  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.64.2  1996/03/23  04:43:10  rlowe
 * Fix includes to look inside wbu top-level directory.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.64.1  1996/03/22  09:07:13  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:19:12  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:07:10  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.32.1  1996/02/29  07:42:12  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  09:08:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:50  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:39:46  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.2  1995/09/02  00:01:47  rlowe
 * Fix includes so that makefile can give search paths, rather than
 * hardcoding them.
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1.28.1  1995/08/22  18:18:37  lcheung
 * move files from synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:15:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.1  1995/04/19  02:28:43  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14 05:22:02  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *------------------------------------------------------------------
 * Revision 1.1.1.1  1995/02/21  19:33:45  kchristi
 * creating new ios directory
 *
 * Revision 1.2  1995/02/17  09:05:39  rlowe
 * Fix SYNALC/ACP includes so that dependencies can be computed.
 *
 * Revision 1.1  1995/02/01  10:59:54  rlowe
 * Ram's initial merge of ALC IOS-Boot code (from older 10.2 tree).
 *
 * Revision 1.1  1995/01/11  23:04:33  rlowe
 * Initial port of IOS Boot to ACP.  Got Monitor up.
 *
 * Revision 1.1.1.1  1994/08/31  22:16:48  rahuja
 * IPS ver 10.2 snapshot
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../wbu/cat5k/alc_include/acp_hw_defs.h"
#include "../../wbu/cat5k/alc_include/mfp_acp_defs.h"

#ifndef	SYNALC
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

#endif	/* SYNALC */

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

#ifndef	SYNALC

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
extern u_char UARTspeeds[];

/*
 * Registers and bits:
 */
#ifndef UARTDEVBASEADDR
#define UARTDEVBASEADDR	0x2120100
#endif

#ifndef uartaddr
#define uartaddr	((struct uartdevice *)UARTDEVBASEADDR)
#endif

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

#else	/* SYNALC */

#ifndef UARTDEVBASEADDR
#define UARTDEVBASEADDR	(ACP_HW_MFP_BASE + MFP_RO_UCR)
#endif

#ifndef uartaddr
#define uartaddr	((struct uartdevice *)(UARTDEVBASEADDR))
#endif

/*
 * Status register bit definitions
 */
#define STP_RXENABLE	(MFP_RSR_RX_ENABLE)
#define RXREADY		(MFP_RSR_BUFFER_FULL)

#define STP_TXENABLE	(MFP_TSR_TX_ENABLE | MFP_TSR_TXSO_HIGH)
#define TXREADY		(MFP_TSR_BUF_EMPTY)

struct regpair {
    volatile u_char ucr;
    volatile u_char rsr;
    volatile u_char tsr;
    volatile u_char data ;
};

struct uartdevice {
    struct regpair a;		/* channel a */
};

/*
 * Basic IO primitives needed when initializing (since you can't
 * yet trust memory).
 */

#define PUTSTRING(str,arg) \
    str = arg; \
    while (*str) { \
        while ((uartaddr->a.tsr & TXREADY) == 0) \
	    ; \
	uartaddr->a.data = *str++; \
    }

#define PUTDIGIT(digit,shift) \
    while ((uartaddr->a.tsr & TXREADY) == 0) \
        ; \
    uartaddr->a.data = digits[((digit >> shift) & 0xF)];

#endif	/* SYNALC */

#define PUTHEXNUM(hexnum) \
    PUTDIGIT(hexnum,28); \
    PUTDIGIT(hexnum,24); \
    PUTDIGIT(hexnum,20); \
    PUTDIGIT(hexnum,16); \
    PUTDIGIT(hexnum,12); \
    PUTDIGIT(hexnum,8); \
    PUTDIGIT(hexnum,4); \
    PUTDIGIT(hexnum,0);
