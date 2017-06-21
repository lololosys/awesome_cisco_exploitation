/* $Id: les_console.h,v 3.2 1995/11/17 17:41:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/les_console.h,v $
 *------------------------------------------------------------------
 * les_console.h -- Definitions for LES console and aux port
 *
 * Copyright (c) 1989-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: les_console.h,v $
 * Revision 3.2  1995/11/17  17:41:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:33:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:37:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * On Sierra (and IRSP?), the UART registers are available on bits 31..24
 * which requires 4byte padding at the start and 7 byte padding
 * throughout the structure
 */
#ifdef __mips

#define PAD(x)		pad_ ## x [7],
#define STARTPAD(x)     pad_ ## x [4],

#else /* __mips */

#define PAD(x)
#define STARTPAD(x)

#endif /* __mips */

/*
 * All UART registers on Pancake are one byte wide:
 */

typedef struct lineaddress {		/* 68k,    R4k  */
    volatile				/* ------------ */
    unsigned char   STARTPAD(0)		/*         00   */
		    modeA,		/*  00     04   */
	 	    PAD(1)		/*         05   */  
                    statusA,		/*  01     0C   */
		    PAD(2)              /*         0D   */
                    commandA,		/*  02     14   */
		    PAD(3)              /*         15   */
                    dataA,		/*  03     1C   */
		    PAD(4)              /*         1D   */
                    portchange,		/*  04     24   */
		    PAD(5)              /*         25   */
                    interrupt,		/*  05     2C   */
		    PAD(6)      	/*         2D   */
                    CTUpper,		/*  06     34   */
		    PAD(7) 	 	/*         35   */
                    CTLower,		/*  07     3C   */
		    PAD(8)		/*         3D   */
                    modeB,		/*  08     44   */
		    PAD(9)		/*         45   */
                    statusB,		/*  09     4C   */
		    PAD(10)             /*         4D   */
                    commandB,		/*  0A     54   */
		    PAD(11)             /*         55   */
                    dataB,		/*  0B     5C   */
		    PAD(12)  		/*         5D   */
                    reserved,		/*  0C     64   */
		    PAD(13)		/*         65   */
                    portstatus,		/*  0D     6C   */
		    PAD(14)		/*         6D   */
                    setport,		/*  0E     74   */
		    PAD(15)		/*         75   */
                    resetport;		/*  0F     7C   */
} uartaddress;

/*
 * Undefine the padding macros
 */
#undef STARTPAD
#undef PAD

#define uartaddr ((uartaddress *)UARTDEVBASEADDR)
#define WHICHUART(linenum) uartaddr

/*
 * Auxiliary Control register (Portchange):
 */

#define RINGAXENB	0x01	/* Enable interrupt on RING A state change */
#define CTSBXENB	0x02	/* Enable interrupt on CTS B state change */
#define RINGBXENB	0x08	/* Enable interrupt on RING B state change */
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
 * Input Port register:
 */

#define RINGA		0x01	/* Port A RING (hardware name is IDTRA) */
#define CTSB		0x02	/* Port B CTS */
#define RINGB		0x08	/* Port B Ring indication */
#define DCDB		0x10	/* Port B DCD */
#define TXCLKB		0x20	/* Port B TX clock input */
#define RXCLKB		0x40	/* Port B RX clock input */



/*
 * Input Port Change register:
 */

/* Bits 0 through 3 are the same as the input port register:
   DTRA, CTSB, and RINGB */
#define DELTA_RINGA	0x10	/* Port A RING changed state */
#define DELTA_CTSB	0x20	/* Port B CTS changed state */
#define DELTA_RINGB	0x80	/* Port B RING changed state */

/*
 * Output Port Configuration register:
 * (All other bits should be zero for Pancake)
 */

#define TX_CLK_A_16X	0x01	/* OP2 is Port A TX clock (16X) */
#define TX_CLK_A_1X	0x02	/* OP2 is Port A TX clock (1X) */

/*
 * Output Port Set and Reset registers
 */
#define DTRA		0x01	/* Port A DTR (hardware name is CDA) */
#define RTSB		0x02	/* Port B RTS */
#define TXCLKA		0x04	/* Port A TX clock */
#define DTRB		0x08	/* Port B DTR */
#define BRUT_PORT_0_OK  0x10    /* Port 0 OK LED (Brut only) */

/*
 * Prototypes
 */

extern void pan_console_interrupt(void);
extern void pan_tx_disable(tt_soc *);
