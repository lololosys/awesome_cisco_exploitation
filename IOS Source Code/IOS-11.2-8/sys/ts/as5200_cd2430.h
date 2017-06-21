/* $Id: as5200_cd2430.h,v 3.1.52.3 1996/08/28 13:17:53 thille Exp $
 * $Source: /release/112/cvs/Xsys/ts/as5200_cd2430.h,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * Month Year, Your Name
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200_cd2430.h,v $
 * Revision 3.1.52.3  1996/08/28  13:17:53  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.52.2  1996/08/10  01:08:38  billw
 * CSCdi54603:  make cirrus hardware gotchas more obvious in source code
 * Branch: California_branch
 *
 * Revision 3.1.52.1  1996/06/16  21:20:00  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/01/10  22:46:04  towu
 * create
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 *
 *  The common code for the CD2430, "tty_cd2430.c", is being included
 *  here as a temporarily measure to isolate the Brasil async driver
 *  from the incoming CD2430 sync/async code to the 11.1 mainline.  The
 *  Brasil async driver will be revamped latter when the code separation
 *  effort which is currently underway for the CD2430 sync/async driver
 *  for mulitiple platform, is committed.  At that time it will conform
 *  separation architecture defined.
 *  
 ****************************************************************************/

#ifndef CD2430_H
#define CD2430_H

/*
 * CD2430 register layout.
 * The names are taken from the Cirrus spec sheet.
 */
typedef unsigned char cd2430_res;
typedef volatile unsigned char cd2430_rw, cd2430_ro, cd2430_wo;
typedef volatile unsigned short cd2430_wrw, cd2430_wro;

typedef struct cd2430_quadart_ {
    cd2430_res res1[7];	/* 00..06 */
    cd2430_rw cor7;	/* 07		Channel Option Register 7 */
#define tsmap2 cor7	/*		  ...name in PPP mode */
    cd2430_res res2[1];	/* 08 */
    cd2430_rw livr;	/* 09		Local Interrupt Vector Register */
    cd2430_res res3[6];	/* 0a..0f */
    cd2430_rw cor1;	/* 10		Channel Option Register 1 */
    cd2430_rw ier;	/* 11		Interrupt Enable Register */
    cd2430_rw stcr;	/* 12		Special Transmit Command Register */
    cd2430_rw ccr;	/* 13		Channel Command Register */
    cd2430_rw cor5;	/* 14		Channel Option Register 5 */
    cd2430_rw cor4;	/* 15		Channel Option Register 4 */
    cd2430_rw cor3;	/* 16		Channel Option Register 3 */
    cd2430_rw cor2;	/* 17		Channel Option Register 2 */
    cd2430_rw cor6;	/* 18		Channel Option Register 6 */
#define tsmap1 cor6	/*		  ...name in PPP mode */
    cd2430_ro dmabsts;	/* 19		DMA Buffer Status */
    cd2430_ro csr;	/* 1a		Channel Status Register */
    cd2430_rw cmr;	/* 1b		Channel Mode Register */
    cd2430_rw schr4;	/* 1c		Special Character Register 4 */
#define txmap4 schr4	/*		 ...its name in PPP mode */
    cd2430_rw schr3;	/* 1d		Special Character Register 3 */
#define txmap3 schr3	/*		 ...its name in PPP mode */
    cd2430_rw schr2;	/* 1e		Special Character Register 2 */
#define txmap2 schr2	/*		 ...its name in PPP mode */
    cd2430_rw schr1;	/* 1f		Special Character Register 1 */
#define txmap1 schr1	/*		 ...its name in PPP mode */
    cd2430_rw rxmap4;	/* 20		Receive control char map PPP */
    cd2430_rw rxmap3;	/* 21		 ...four bytes in a row */
    cd2430_rw scrh;	/* 22		Special Character Range high */
#define rxmap2 scrh	/*		  ...its name in PPP mode */
    cd2430_rw scrl;	/* 23		Special Character Range low */
#define rxmap1 scrl	/*		  ...its name in PPP mode */
    cd2430_wrw rtpr;	/* 24		Receive Timeout Period Register */
    cd2430_rw licr;	/* 26		Local Interrupting Channel Register */
    cd2430_res res5[2];	/* 27..28 */
    cd2430_ro ttr;	/* 29		Transmit Timer Register */
    cd2430_wrw gt1;	/* 2a..2b	General Timer 1 */
    cd2430_res res6[2];	/* 2c..2d */
    cd2430_rw lnxt;	/* 2e		Literal Next Character */
#define tsmap3 lnxt	/*		  ...name in PPP mode */
    cd2430_res res7[1];	/* 2f */
    cd2430_ro rfoc;	/* 30		Receive FIFO Output Count */
    cd2430_res res8[7];	/* 31..37 */
    cd2430_wro tcbadru;	/* 38		Transmit Current Buffer Address upper */
    cd2430_wro tcbadrl;	/* 3a		Transmit Current Buffer Address lower */
    cd2430_wro rcbadru;	/* 3c		Receive Current Buffer Address upper */
    cd2430_wro rcbadrl;	/* 3e		Receive Current Buffer Address lower */
    cd2430_wrw arbadru;	/* 40		A Receive Buffer Address upper */
    cd2430_wrw arbadrl;	/* 42		A Receive Buffer Address lower */
    cd2430_wrw brbadru;	/* 44		B Receive Buffer Address upper */
    cd2430_wrw brbadrl;	/* 46		B Receive Buffer Address lower */
    cd2430_wrw brbcnt;	/* 48		B Receive Buffer Byte Count */
    cd2430_wrw arbcnt;	/* 4a		A Receive Buffer Byte Count */
    cd2430_res res9[2];	/* 4c..4d */
    cd2430_rw brbsts;	/* 4e		B Receive Buffer Status */
    cd2430_rw arbsts;	/* 4f		A Receive Buffer Status */
    cd2430_wrw atbadru;	/* 50		A Transmit Buffer Address upper */
    cd2430_wrw atbadrl;	/* 52		A Transmit Buffer Address lower */
    cd2430_wrw btbadru;	/* 54		B Transmit Buffer Address upper */
    cd2430_wrw btbadrl;	/* 56		B Transmit Buffer Address lower */
    cd2430_wrw btbcnt;	/* 58		B Transmit Buffer Byte Count */
    cd2430_wrw atbcnt;	/* 5a		A Transmit Buffer Byte Count */
    cd2430_res resa[2];	/* 5c..5d */
    cd2430_rw btbsts;	/* 5e		B Transmit Buffer Status */
    cd2430_rw atbsts;	/* 5f		A Transmit Buffer Status */
    cd2430_res resb[32];/* 60..7f */
    cd2430_ro tftc;	/* 80		Transmit FIFO Transfer Count */
    cd2430_ro gfrcr;	/* 81		Global Firmware Revision Code */
    cd2430_res resc[2];	/* 82..83 */
    cd2430_wo reoir;	/* 84		Receive End of Interrupt Register */
    cd2430_wo teoir;	/* 85		Transmit End of Interrupt Register */
    cd2430_wo meoir;	/* 86		Modem End of Interrupt Register */
    cd2430_res resd[1];	/* 87*/
    cd2430_wrw risr;	/* 88		Receive Interrupt Status Register */
    cd2430_ro tisr;	/* 8a		Transmit Interrupt Status Register */
    cd2430_ro misr;	/* 8b		Modem Interrupt Status Register */
    cd2430_res rese[2];	/* 8c..8d */
    cd2430_rw bercnt;	/* 8e		Bus Error Retry Count */
    cd2430_res resf[49];/* 8f..bf */
    cd2430_rw tcor;	/* c0		Transmit Clock Option Register */
    cd2430_res resg[2];	/* c1..c2 */
    cd2430_rw tbpr;	/* c3		Transmit Baud Rate Period Register */
    cd2430_res resh[4];	/* c4..c7 */
    cd2430_rw rcor;	/* c8		Receive Clock Option Register */
    cd2430_res resi[2];	/* c9..ca */
    cd2430_rw rbpr;	/* cb		Receive Baud Rate Period Register */
    cd2430_res resj[10];/* cc..d5 */
    cd2430_rw cpsr;	/* d6		CRC Polynomial Select Register */
    cd2430_res resJ[3];	/* d7..d9 */
    cd2430_rw tpr;	/* da		Timer Period Register */
    cd2430_res resk[3];	/* db..dd */
    cd2430_rw msvr_rts;	/* de		Modem Signal Value--RTS */
    cd2430_rw msvr_dtr;	/* df		Modem Signal Value--DTR */
    cd2430_rw tpilr;	/* e0		Transmit Priority Interrupt Level */
    cd2430_rw rpilr;	/* e1		Receive Priority Interrupt Level */
    cd2430_ro stk;	/* e2		Stack Register */
    cd2430_rw mpilr;	/* e3		Modem Priority Interrupt Level */
    cd2430_res resl[8];	/* e4..eb */
    cd2430_ro tir;	/* ec		Transmit Interrupt Register */
    cd2430_ro rir;	/* ed		Receive Interrupt Register */
    cd2430_rw car;	/* ee		Channel Access Register */
    cd2430_ro mir;	/* ef		Modem Interrupt Register */
    cd2430_rw airh;	/* f0		Auxilary Instruction Register 17..16 */
    cd2430_rw mtcr;	/* f1		MTCR */
    cd2430_rw airl;	/* f2		 ...AIR bits 7..0 */
    cd2430_rw airm;	/* f3		 ... bits 15..8 */
    cd2430_res resm[2];	/* f4..f5 */
    cd2430_wo dmr;	/* f6		DMA Mode Register */
    cd2430_rw btcr;	/* f7		BTCR */
    cd2430_ro rdr;	/* f8		Read/Write Data Register */
#define tdr rdr
    cd2430_res reso[7];	/* f9..ff */
} cd2430_quadart;

/*
 * CMR bits
 */
#define CMR_RXDMA (0x80)	/* Receive using DMA (else interrupt) */
#define CMR_TXDMA (0x40)	/*  ...transmit */
#define CMR_MODE_ASYNC (0x2)	/* Set mode for async */
#define CMR_MODE_PPP (0x4)	/*  ...for async PPP */
#define CMR_MODE_SLIP (0x5)	/*  ...for async SLIP */
#define CMR_MODE_MNP4 (0x6)	/*  ...for MNP4 */
#define CMR_MODE_AUTO (0x7)	/*  ...for autobaud */

/*
 * COR1 bits
 */
#define COR1_PARITY_ODD (0x80)	/* Odd parity (else even) */
#define COR1_PARITY_NONE (0x0)		/* No parity */
#define COR1_PARITY_FORCE (0x20)	/* Force parity (odd=1, even=0) */
#define COR1_PARITY_NORM (0x40)		/* Normal parity */
#define COR1_PARITY_IGN (0x10)		/* Ignore parity on received */
#define COR1_5BIT (0x4)		/* Char length--5 bits */
#define COR1_6BIT (0x5)		/*  ...6 */
#define COR1_7BIT (0x6)		/*  ...7 */
#define COR1_8BIT (0x7)		/*  ...8 */

/*
 * COR2 bits
 **** REFERENCES TO RTS AND DTR are SWAPPED for HW flow ctrl ******
 * See comments in ts/tty_cd2430.c
 */
#define COR2_IXANY (0x80)	/* Any char serves as XON (else only XON) */
#define COR2_TXINBAND (0x40)	/* Transmit inband flow control enable */
#define COR2_ETC (0x20)		/* Embedded transmitter command enable */
#define COR2_LOOP (0x8)		/* Enable remote loopback */
#define COR2_RTS (0x4)		/* Auto-RTS */
#define COR2_CTS (0x2)		/* Auto-CTS */
#define COR2_DSR (0x1)		/* Auto-DSR */

/*
 * Values which can be sent once COR2_ETC is set
 */
enum ETC {
    ETC_ESC = 0x0,		/* Start escape sequence */
    ETC_BREAK = 0x81,		/* Send break */
    ETC_DELAY = 0x82,		/* Insert delay */
    ETC_ENDBREAK = 0x83		/* Stop sending break */
};

#define CD2430BREAKDATA 8       /* ETC cmd Data bytes needed to send BRK */


/*
 * COR3 bits
 */
#define COR3_ESCDE (0x80)	/* Extended special char detect enable */
#define COR3_RANGEDE (0x40)	/* Range detect enable */
#define COR3_FCT (0x20)		/* Flow control transparency */
#define COR3_SCDE (0x10)	/* Special char detect enable */
#define COR3_SCSTRIP (0x8)	/* Strip high bit for special char check */
#define COR3_1STOP (0x2)	/* 1 stop bit */
#define COR3_15STOP (0x3)	/*  ...1.5 */
#define COR3_2STOP (0x4)	/*  ...2 */
#define COR3_TXGEN (0x10)	/* PPP: generate CRC on transmit */
#define COR3_RXCK (0x20)	/*  ...check received CRC */
#define COR3_CRCPASS (0x40)	/*  ...pass CRC in packet (else strip) */

/*
 * COR4 bits
 */
#define COR4_DSRZD (0x80)	/* Detect 1->0 DSR transition */
#define COR4_CDZD (0x40)	/*  ...CD */
#define COR4_CTSZD (0x20)	/*  ...CTS */
#define COR4_FIFOMASK (0xF)	/* FIFO threshold value */

/*
 * Calculation for threshold based on target latency (thanks, kph!)
 * Since we're using DMA, we can assume a somewhat better latency than
 * byte-at-a-time interrupt handling.
 */
#define TARGET_INTERRUPT_LATENCY 250 /* Maximum interrupt latency in us */
/* #define COR4_RXTHRESH(baud) (15-((baud)/(10000000/TARGET_INTERRUPT_LATENCY))) */

/*
 * Nyet.  This is hard-coded, per advice from Cirrus.  The effective range
 * of buffering is much lower than expected, and their advice is to run
 * with the threshold half way between max and min values--8.
 */
#define COR4_RXTHRESH(baud) (8)

/*
 * COR5 bits
 */
#define COR5_DSROD (0x80)	/* Detect 0->1 DSR transition */
#define COR5_CDOD (0x40)	/*  ...CD */
#define COR5_CTSOD (0x20)	/*  ...CTS */
#define COR5_RXMASK (0xF)	/* Receive flow control threshold */

/*
 * COR6 bits
 */
#define COR6_IGNCR (0x80)	/* Ignore CR on input */
#define COR6_ICRNL (0x40)	/*  ...Map CR -> NL */
#define COR6_INLCR (0x20)	/*  ...NL -> CR */
#define COR6_IGNBRK (0x10)	/* Ignore input break */
#define COR6_NOBRKINT (0x8)	/* Inhibit break interrupt */
#define COR6_PARMRK (0x4)	/* Mark parity */
#define COR6_INPCK (0x2)	/* Check parity */
#define COR6_PARINT (0x1)	/* Interrupt on bad parity */

/*
 * COR7 bits
 */
#define COR7_ISTRIP (0x80)	/* Strip high bit */
#define COR7_LNE (0x40)		/* Enable literal-next quotiing */
#define COR7_FCERR (0x20)	/* Flow control on error chars */
#define COR7_ONLCR (0x2)	/* Map NL->CR on output */
#define COR7_OCRNL (0x1)	/*  ...CR->NL */

/*
 * RCOR bits
 */
#define RCOR_TLVAL (0x80)	/* Transmit line value */
#define RCOR_DPLL (0x20)	/* DPLL enable */
#define RCOR_NRZ (0x0)		/*  ...NRZ */
#define RCOR_NRZI (0x8)		/*  ...NRZI */
#define RCOR_MANCH (0x10)	/*  ...Manchester */
#define RCOR_CLK0 (0)		/* Receive clock source--clock 0 */
#define RCOR_CLK1 (1)		/*  ...1 */
#define RCOR_CLK2 (2)		/*  ...2 */
#define RCOR_CLK3 (3)		/*  ...3 */
#define RCOR_CLK4 (4)		/*  ...4 */
#define RCOR_EXT (6)		/*  ...external */

/*
 * TCOR bits
 */
#define TCOR_CLK0 (0)		/* Transmit clock source--clock 0 */
#define TCOR_CLK1 (0x20)	/*  ...1 */
#define TCOR_CLK2 (0x40)	/*  ...2 */
#define TCOR_CLK3 (0x60)	/*  ...3 */
#define TCOR_CLK4 (0x80)	/*  ...4 */
#define TCOR_EXT (0xc0)		/*  ...external */
#define TCOR_RX (0xe0)		/*  ...receive clock */
#define TCOR_EXT1X (0x8)	/* x1 external clock */
#define TCOR_LLM (0x2)		/* Local loopback mode */

/*
 * CCR bits
 */
#define CCR_SET0 (0)		/* Select bit set 0 */
#define CCR_CLRCH (0x40)	/* Clear channel */
#define CCR_INITCH (0x20)	/* Init channel */
#define CCR_RESET (0x10)	/* Reset all */
#define CCR_ENTX (0x8)		/* Enable transmitter */
#define CCR_DISTX (0x4)		/*  ...disable */
#define CCR_ENRX (0x2)		/* Enable receiver */
#define CCR_DISRX (0x1)		/*  ...disable */

#define CCR_SET1 (0x80)		/* Select bit set 1 */
#define CCR_CLRT1 (0x40)	/* Clear timer 1 */
#define CCR_CLRT2 (0x20)	/*  ...2 */
#define CCR_CLRRCV (0x10)	/* Clear receiver command */
#define CCR_CLRTX (0x08)	/*  ...transmitter */

/*
 * STCR bits
 */
#define STCR_ABORTTX (0x40)	/* Abort transmit */
#define STCR_APNDCOMPLT (0x20)	/* Append mode complete */
#define STCR_SNDSPEC (0x8)	/* Send special char: */
#define STCR_SPC1 (1)		/*  ...special char 1 */
#define STCR_SPC2 (2)		/*  ...special char 2 */
#define STCR_SPC3 (3)		/*  ...special char 3 */
#define STCR_SPC4 (4)		/*  ...special char 4 */
#define STCR_PPPSNDSPEC (0x4)	/* Send special char in async PPP mode */
#define STCR_PPPXON (0x2)	/*  ...send XON (0x13) */
#define STCR_PPPXOFF (0x1)	/*  ...send XOFF (0x11) */

/*
 * CSR bits
 */
#define CSR_RXEN (0x80)		/* Receiver enabled */
#define CSR_RXFLOFF (0x40)	/* Receive flow off requested */
#define CSR_RXFLON (0x20)	/* Receive flow on requested */
#define CSR_RXIDLE (0x10)	/* Receiver idle */
#define CSR_TXEN (0x8)		/* Transmit enabled */
#define CSR_TXFLOFF (0x4)	/* Transmit flow off seen */
#define CSR_TXFLON (0x2)	/* Transmit flow on seen */
#define CSR_TXIDLE (0x1)	/* Transmitter idle */

/*
 * MSVR bits
 **** REFERENCES TO RTS AND DTR are SWAPPED for HW flow ctrl ******
 * See comments in ts/tty_cd2430.c
 */
#define MSVR_DSR (0x80)		/* Value of DSR */
#define MSVR_CD (0x40)		/*  ...CD */
#define MSVR_CTS (0x20)		/*  ...CTS */
#define MSVR_DTROPT (0x10)	/* Output clock on DTR pin */
#define MSVR_PORTID (0x4)	/* Device is 2400 (else 2401) */
#define MSVR_DTR (0x2)		/* Value of DTR */
#define MSVR_RTS (0x1)		/*  ...RTS */

/*
 * LIVR bits
 */
#define LIVR_MASK (0x3)		/* Mask for interrupt reason: */
#define LIVR_MODEM (0x1)	/* Modem change interrupt */
#define LIVR_TX (0x2)		/*  ...transmit */
#define LIVR_RX (0x3)		/*  ...receive */
#define LIVR_RXEX (0)		/*  ...receive exception */

/*
 * IER bits
 */
#define IER_MDM (0x80)		/* Enable modem change interrupts */
#define IER_RET (0x20)		/*  ...receive exception timeouts */
#define IER_RXD (0x8)		/*  ...receive FIFO threshold */
#define IER_TIMER (0x4)		/*  ...timers */
#define IER_TXMPTY (0x2)	/*  ...transmit buffer entirely empty */
#define IER_TXD (0x1)		/*   ...transmit FIFO threshold */

/*
 * LICR bits
 */
#define LICR_MASK (0xc)		/* Mask for channel number bits */
#define LICR_CHAN0 (0x0)	/*  ...channel 0 */
#define LICR_CHAN1 (0x4)	/*  ...channel 1 */
#define LICR_CHAN2 (0x8)	/*  ...channel 2 */
#define LICR_CHAN3 (0xc)	/*  ...channel 3 */

/*
 * STK bits
 */
#define STK_MASK0 (0x81)	/* Mask for current level */
#define STK_NONE (0x0)		/*  ...not nested to interrupt */
#define STK_MODEM (0x1)		/*  ...in modem */
#define STK_TX (0x80)		/*  ...transmit */
#define STK_RX (0x81)		/*  ...receive */

/*
 * Masks for the nested interrupt level bits in the stack register.
 */
#define STK_CLVL_MASK   0x81  /* Current level mask */
#define STK_MLVL_MASK   0x42  /* Middle level mask */
#define STK_TLVL_MASK   0x24  /* Tertiary level mask */
 
/*
 * Defines for the codes that indicate whether the (possibly) nested
 * interrupts are transmit, receive or modem interrupts.
 */
#define STK_TX_CLVL             0x80
#define STK_TX_MLVL             0x40
#define STK_TX_TLVL             0x20
#define STK_RX_CLVL             0x81
#define STK_RX_MLVL             0x42
#define STK_RX_TLVL             0x24
#define STK_MODEM_CLVL          0x01
#define STK_MODEM_MLVL          0x02
#define STK_MODEM_TLVL          0x04
 
/*
 * RIR bits
 */
#define RIR_VECT(x) (((x) & 0xc) >> 2)
				/* Vector number of type of interrupt */
#define RIR_CHAN(x) ((x) & 0x3)	/*  ...channel */

/*
 * RISR bits (16-bit register; could also be read byte low/high)
 */
#define RISR_BRK (0x1)		/* Break detected */
#define RISR_FE (0x2)		/* Framing error */
#define RISR_PE (0x4)		/* Parity error */
#define RISR_OE (0x8)		/* Overrun error */
#define RISR_SCMASK (0x70)	/* Mask for special char match: */
#define RISR_SC1 (0x10)		/*  ...match 1 */
#define RISR_SC2 (0x20)		/*  ...match 2 */
#define RISR_SC3 (0x30)		/*  ...match 3 */
#define RISR_SC4 (0x40)		/*  ...match 4 */
#define RISR_SCRNG (0x70)	/*  ...match with range */
#define RISR_TO (0x80)		/* Receive data timeout */
#define RISR_BUFB (0x800)	/* Status during B (else A) */
#define RISR_EOB (0x2000)	/* End of receive buffer */
#define RISR_BERR (0x8000)	/* Bus error */

/*
 * RISR low bits during async PPP
 */
#define RISR_CRC (0x10)		/* CRC error */
#define RISR_RXABT (0x20)	/* Receiver abort */
#define RISR_EOF (0x40)		/* End of frame received */

/*
 * REOIR bits
 */
#define REOIR_TERM (0x80)	/* Terminate current DMA buffer */
#define REOIR_DISCEXC (0x40)	/* Discard exception character */
#define REOIR_NOTRANSF (0x8)	/* No data transferred */

/*
 * TIR bits
 */
#define TIR_VECT(x) (((x) & 0xc) >> 2)
				/* Vector number of type of interrupt */
#define TIR_CHAN(x) ((x) & 0x3)	/*  ...channel */

/*
 * TISR bits
 */
#define TISR_BERR (0x80)	/* Bus error */
#define TISR_EOF (0x40)		/* End of frame */
#define TISR_EOB (0x20)		/* End of buffer */
#define TISR_UE (0x10)		/* Transmitter underrun */
#define TISR_BUFB (0x8)		/* Status during B (else A) */
#define TISR_MPTY (0x2)		/* Transmit buffer empty */
#define TISR_TXD (0x1)		/* Transmit threshold reached */

/*
 * TEOIR bits
 */
#define TEOIR_TERM (0x80)	/* Terminate current DMA buffer */
#define TEOIR_NOTRANSF (0x8)	/* No data transferred */

/*
 * MIR bits
 */
#define MIR_VECT(x) (((x) & 0xc) >> 2)
				/* Vector number of type of interrupt */
#define MIR_CHAN(x) ((x) & 0x3)	/*  ...channel */

/*
 * MISR bits
 */
#define MISR_DSR (0x80)		/* DSR changed */
#define MISR_CD (0x40)		/*  ...CD */
#define MISR_CTS (0x20)		/*  ...CTS */
#define MISR_TIMER1 (0x1)	/* Timer 1 popped */
#define MISR_TIMER2 (0x2)	/*  ...timer 2 */

/*
 * DMR bit
 */
#define DMR_BYTE (0x8)		/* Do byte DMA (else word) */
#define DMR_SSTEP (0x2)		/* BTCR mode--single step */
#define DMR_CLKDIS (0x1)	/*  ...clock disable */

/*
 * DMABSTS bits
 */
#define DMABSTS_RSTAPD (0x40)	/* Terminate append pending */
#define DMABSTS_APPEND (0x10)	/* Append mode for buffer A */
#define DMABSTS_NTBUF (0x8)	/* Next transmit buf (0=A, 1=B) */
#define DMABSTS_TBUSY (0x4)	/* Current transmit buffer active */
#define DMABSTS_NRBUF (0x2)	/* Next receive buf (0=A, 1=B) */
#define DMABSTS_RBUSY (0x1)	/* Current receive buffer active */

/*
 * [AB]RBSTS bits
 */
#define RBSTS_BERR (0x80)	/* Bus error */
#define RBSTS_EOF (0x40)	/* End of frame */
#define RBSTS_EOB (0x20)	/* End of buffer */
#define RBSTS_24OWN (0x1)	/* 2400 owns buffer */

/*
 * [AB]TBSTS bits
 */
#define TBSTS_BERR (0x80)	/* Bus error */
#define TBSTS_EOF (0x40)	/* End of frame */
#define TBSTS_EOB (0x20)	/* End of buffer */
#define TBSTS_APND (0x4)	/* Append */
#define TBSTS_MAP32 (0x4)	/* PPP async--use conservative ACCM */
#define TBSTS_INTR (0x2)	/* Interrupt after buffer sent */
#define TBSTS_24OWN (0x1)	/* 2400 owns buffer */

/*
 * CPSR bits
 */
#define CPSR_CRC16 0x1		/* Use CRC-16 calculation */
#define CPSR_V41 0x0		/*  ...V.41 polynomial instead */

/*
 * Constants
 */
#define CD2430_BUFSIZE (16)	/* # bytes in transmit or receive FIFO */
#define CD2430_CHIPCHAN (4)	/* # channels per 2430 */
#define CD2430_DISABLE (3)	/* Disable all CD2430 interrupts */
#define CD2430T_DISABLE (1)	/*  ...transmit */
#define CD2430R_DISABLE (2)	/*  ...receive */
#define CD2430M_DISABLE (3)	/*  ...modem signals */

/* Extract low and high buffer address fields */
#define CD2430LO(addr) ((ulong)addr & 0xFFFF)
#define CD2430HI(addr) (((ulong)addr >> 16) & 0xFFFF)

/*
 * per channel private variables for
 * the 2430 driver
 */
typedef struct cd2430_priv_ {
    ulong status;               /* Various status flags */
    int rxbuffs_outstanding;    /* system buffers allocated */
    int rxbuffs_disallowed;     /* buffs disallowed for exceeding max */
    int nested_rx_interrupts;   /* nested receive interrupts */
    int nested_tx_interrupts;   /* nested transmit interrupts */
    int nested_mdm_interrupts;  /* nested modem interrupts */
} cd2430_priv;

/* Location of vectors & size */
#define CD2430Vector 64         /* Base vector for CD2430 */
#define CD2430VectorCount 4     /* Number of vectors per controller */

/*
 * GETCSR()
 *      Given ttybase, get appropriate TTY and register base
 *
 * The CD2430 interrupt vector is per-chip; thus, when we get an interrupt
 * we know which chip has interrupted, but not which channel.  By reading
 * the local interrupt control register, we then get the channel number,
 * encoded as bits 2 and 3.  This is usable as an index into an array of
 * pointers, which gives us our actual TTY.
 */
#define GETCSR(ttybase, csr, tty) \
        (csr) = (cd2430_quadart *)((*(ttybase))->uartdata); \
        (tty) = *(tt_soc **)(((uchar *)(ttybase)) + (csr)->licr);
 

/*
 * Private status flags in cd2430_priv.status
 *   we keep a copy of current flow control on channel
 *      to avoid having to set same flow control type
 */
#define SEEINGBREAK      0x00000001  /* A break was received but no */
				     /*            end of break yet */
#define SFLOW_IN         0x00000002  /* SW flow control in */
#define SFLOW_OUT        0x00000004  /* SW flow control out */
#define HFLOW_IN         0x00000008  /* SW flow control in */
#define HFLOW_OUT        0x00000010  /* SW flow control out */


/*
 * Maximum number of buffers a cd2430 channel can
 * pull from the system pool
 */
#define CD2430_MAX_SYS_BUFFS 8
#define CD2430_DEF_RXBUFF  3
#define CD2430_DEF_TXBUFF  2


/*
 * Buffers in this driver are of this size
 */
#define CD2430_PAKSIZE (BIGDATA)

/*
 * Messages we may need
 */
extern cd2430_priv *cd2430info_base;
extern int cd2430_controllers;          /* # 2430 chips in system */
extern boolean can_ppp;         /* Supports HW async HDLC */
extern vectortype cd2430_vector;
extern int nasync;                     /* # 2430 ports on system */
extern pooltype *cd2430_buffpool;        /* cd2430 permanent buffers pool */
extern int cd2430Base;
extern uint cd2430clock;

extern void cd2430_vector_init(void);
extern void cd2430_channel_setup(tt_soc *tty, cd2430_quadart *csr, int channel_id);
extern boolean cd2430_setup_break_packet(void);
extern void cd2430_download_controller_firmware (cd2430_quadart *);

extern boolean tty_cd2430_xon(tt_soc *tty);
extern void cd2430_print_modemcontrol (tt_soc *tty);
extern void cd2430_show_contr (void);
extern void cd2430_pppmode (tt_soc *tty, int on);
extern void cd2430_pppmap (tt_soc *tty, ulong txmap, ulong rxmap);
extern void cd2402_download (cd2430_quadart *csr);
extern void sendccr(cd2430_quadart *csr, uchar cmd);
extern void tty_cd2430_parser_init(void);

#endif /* CD2430_H */

/****************************************************************************
 *
 *	AS5200 Specific Definitions and Declarations.
 *
 ****************************************************************************/

#ifndef AS5200_CD2430_H
#define AS5200_CD2430_H

/* The offset of 1st CD2430 register set */
#define BRASIL_CD2430_OFFSET  (0x0200)

/* Clock rate 2430 driven at */
#define CD2430CLOCK (20*1024*1024)

/* Minimum available bytes in rx fifo for RTS flow control */
/* This was raised 8 to handle CRC errors during PPP mode */
#define RTS_OVERRUN_BUFSIZE		8

#define SLOT_CD2430ADDRESS(slot, ctlr) \
	(cd2430_quadart *)(((uint)(GET_SLOT_BASE(slot)) + \
	BRASIL_CD2430_OFFSET) + sizeof(cd2430_quadart)*ctlr)

#define CD2430ADDRESS(unit) \
	(cd2430_quadart *) ((tt_soc *)(*cd2430_ttyptr[unit]))->uartdata

#define CD2430_TO_TTY(cntrl, chan) channel_to_tty(cntrl, chan) 

/* Convert interrupt vector to its PILR value */
#define CD2430PILR(x) (((x) << 1) | 0xF1)

/* total number of controller per slot */
#define CD2430_NUM_CONTROLLERS	((MAX_MODEM_PER_SLOT/CD2430_CHIPCHAN) + 1)

/* Motherboard IRQ Vector Register Offsets */
#define TX_IRQ_VECTOR_OFFSET			1
#define RX_IRQ_VECTOR_OFFSET			2
#define MODEM_IRQ_VECTOR_OFFSET			3

/*
 * Virtual to physical
 *    -	For AS5200, there is a separate I/O bus from system bus.
 *	The CPU/main memory sits only on the system bus, while
 *	the packet (or I/O or DMA) memory will sit on both buses.
 *	The physical addresses of the packet memory as seen from
 *	the IO/DMA devices are different from that which the CPU
 *	will see.  Thus the VTOP is used to translate the packet 
 *	buffer addresses which the CPU has allocated, into the
 *	phsyical addresses that the IO/DMA devices will see.
 */
#define VTOP(addr) ((ulong)addr & 0x1FFFFFFF)

extern void (*as5200_cd2430_vectors[]);	/* Front-line interrupt vectors */

extern void tty_cd2430_subsystem_init( void );
extern void tty_as5200_cd2430_init(modem_slot_t *slotinfo, int slot);
extern int tty_as5200_cd2430_line_init( void);
extern tt_soc *channel_to_tty(int cntrl, int chan);
extern tt_soc **cd2430_ttyptr[];
extern uint cd2430_tx_mask;	
extern uint cd2430_rx_mask;
extern uint cd2430_mod_mask;
#endif /* AS5200_CD2430_H */


