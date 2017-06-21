/* $Id: c4000_bsi.h,v 3.5 1996/02/02 21:03:18 nrvankay Exp $
 * $Source: /release/111/cvs/Xsys/src-c4000/c4000_bsi.h,v $
 *------------------------------------------------------------------
 * C4000 BSI FDDI device definitions.
 *
 * September 1992, Chris Shaker, Scott Mackie
 * November 1993, Lele Nardin
 * March 1994, Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c4000_bsi.h,v $
 * Revision 3.5  1996/02/02  21:03:18  nrvankay
 * CSCdi44246:  FDDI channel one doesnt receive any packets after sometime
 * Under heavy traffic, sometimes the BSI chip reuses the same PSP
 * buffer or skips at most 2 PSP buffers. A fix had been added to
 * check for the skipped buffers.
 *
 * Revision 3.4  1995/12/02  02:01:10  nrvankay
 * CSCdi38185:  OSPF adjacency lost after a few minutes/hours
 * This one has two fixes. One in the firmware and one in the software.
 * Firmware: Not handling small packets properly
 * Software: When multicast packects are switched the buffers are
 *           not replinshed properly
 *
 * Revision 3.3  1995/11/17  18:45:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:26:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:27:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/15  23:05:48  wfried
 * CSCdi38123:  BSI bug (Zero IS on discarded frame) needs workaround
 * Ensure that a single PSP page is not used twice in a row on a given
 * receive channel. This allows us to use the PSP location check for
 * error conditions in a safe way.
 *
 * Revision 2.1  1995/06/07  22:56:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __C4000_BSI_H__
#define __C4000_BSI_H__

#define BMAC_LOOPBACK		0
#define REQ_CHAN                rq1
#define BEAC_CHAN               rq0
#define REQ1_IN_USE		1

#define DEFAULT_SMTBUFCOUNT    20   /* twice the quantum0 count */
                                    /* but let's not say 2*quantum0 just yet */
#define MAX_SMTBUFCOUNT        255
#define XDI_BUFCOUNT           185  /* MAX_EC_ELEMENTS from xdi/fbmsrp.c */
#define XDI_BUFSIZE            128

#define NATL_EXTRA_FC_BYTES    3

#define ERROR_RETURN           (-1)

#define BYTE(a)                (a & 0xFF)

#define BSI_MAGIC              0xfa5005af
#define SINGLEMODE             2


/*
 * in xx, players are at 8000400, 8000480
 * bmac at 8000600, bsi 8000500
 */
/*
 ******************************************************************************
 * National Player (DP83251) registers:
 ******************************************************************************
 */

#define BIGEND			/* 6800 family is big endian */

 /*
  * MR (Mode Register) bits:
  */
#define PLAYER_MR_STOP  0x00    /* STOP */
#define PLAYER_MR_RUN	0x01	/* RUN */
#define PLAYER_MR_ILB	0x02	/* Internal Loopback */
#define PLAYER_MR_EXLB	0x04	/* External Loopback */
#define PLAYER_MR_CM	0x08	/* Cascade Mode */
#define PLAYER_MR_TQL	0x10	/* Transmit Quiet Level */
#define PLAYER_MR_TE	0x20	/* Transmit Enable */
#define PLAYER_MR_TNRZ	0x40	/* Transmit NRZ */
#define PLAYER_MR_RNRZ	0x80	/* Receive NRZ */

 /*
  * CR (Configuration Register) bits:
  */
#define PLAYER_CR_AIS_MSK	0x03	/* A Indicate Selector */
#define PLAYER_CR_AIS_PINV	0x00
#define PLAYER_CR_AIS_RBUS	0x01
#define PLAYER_CR_AIS_ABUS	0x02
#define PLAYER_CR_AIS_BBUS	0x03
#define PLAYER_CR_BIS_MSK	0x0C	/* B Indicate Selector */
#define PLAYER_CR_BIS_PINV	0x00
#define PLAYER_CR_BIS_RBUS	0x04
#define PLAYER_CR_BIS_ABUS	0x08
#define PLAYER_CR_BIS_BBUS	0x0C
#define PLAYER_CR_TRS_MSK	0x30	/* Transmit Request Selector */
#define PLAYER_CR_TRS_PINV	0x00
#define PLAYER_CR_TRS_RBUS	0x10
#define PLAYER_CR_TRS_ABUS	0x20
#define PLAYER_CR_TRS_BBUS	0x30
#define PLAYER_CR_AIE		0x40	/* A Indicate Enable */
#define PLAYER_CR_BIE		0x80	/* B Indicate Enable */

#define	PLAYER_S_CR_ISOLATED\
	(PLAYER_CR_AIS_ABUS | PLAYER_CR_BIS_BBUS | PLAYER_CR_TRS_RBUS |\
	 PLAYER_CR_AIE | PLAYER_CR_BIE)

#define	PLAYER_A_CR_ISOLATED\
	(PLAYER_CR_AIS_BBUS | PLAYER_CR_BIS_ABUS | PLAYER_CR_TRS_RBUS |\
	 PLAYER_CR_AIE | PLAYER_CR_BIE)

#define	PLAYER_A_CR_INSERT_P\
	(PLAYER_CR_AIS_RBUS | PLAYER_CR_BIS_BBUS | PLAYER_CR_TRS_ABUS |\
	 PLAYER_CR_AIE | PLAYER_CR_BIE)

#define	PLAYER_A_CR_INSERT_X\
	(PLAYER_CR_AIS_RBUS | PLAYER_CR_BIS_ABUS | PLAYER_CR_TRS_BBUS |\
	 PLAYER_CR_AIE | PLAYER_CR_BIE)

#define	PLAYER_B_CR_ISOLATED\
	(PLAYER_CR_AIS_ABUS | PLAYER_CR_TRS_RBUS | PLAYER_CR_AIE)

#define	PLAYER_B_CR_INSERT_P\
	(PLAYER_CR_AIS_RBUS | PLAYER_CR_TRS_ABUS | PLAYER_CR_AIE)

#define	PLAYER_B_CR_INSERT_X\
	(PLAYER_CR_AIS_RBUS | PLAYER_CR_TRS_ABUS | PLAYER_CR_AIE)

 /* Interrupt Condition Register (ICR) */
#define PLAYER_ICR_DPE		0x01	/* PHY Request Data Parity Error */
#define PLAYER_ICR_CPE		0x02	/* CBUS Data Parity Error */
#define PLAYER_ICR_CCR		0x04	/* CBUS Write Command Reject */
#define PLAYER_ICR_CWI		0x08	/* Conditional Write Inhibit */
#define PLAYER_ICR_LEMT		0x10	/* Link Error Monitor Threshold */
#define PLAYER_ICR_RCA		0x20	/* Receive Condition A */
#define PLAYER_ICR_RCB		0x40	/* Receive Condition B */
#define PLAYER_ICR_UDI		0x80	/* User Definable Interrupt */

 /* Interrupt Condition Mask Register (ICRM) */
#define PLAYER_ICMR_DPE		0x01	/* PHY Request Data Parity Error */
#define PLAYER_ICMR_CPE		0x02	/* CBUS Data Parity Error */
#define PLAYER_ICMR_CCR		0x04	/* CBUS Write Command Reject */
#define PLAYER_ICMR_CWI		0x08	/* Conditional Write Inhibit */
#define PLAYER_ICMR_LEMT	0x10	/* Link Error Monitor Threshold */
#define PLAYER_ICMR_RCA		0x20	/* Receive Condition A */
#define PLAYER_ICMR_RCB		0x40	/* Receive Condition B */
#define PLAYER_ICMR_UDI		0x80	/* User Definable Interrupt */

 /* Interrupt Condition Compare Register (ICCR) */
#define PLAYER_ICCR_DPE		0x01	/* PHY Request Data Parity Error */
#define PLAYER_ICCR_CPE		0x02	/* CBUS Data Parity Error */
#define PLAYER_ICCR_CCR		0x04	/* CBUS Write Command Reject */
#define PLAYER_ICCR_CWI		0x08	/* Conditional Write Inhibit */
#define PLAYER_ICCR_LEMT	0x10	/* Link Error Monitor Threshold */
#define PLAYER_ICCR_RCA		0x20	/* Receive Condition A */
#define PLAYER_ICCR_RCB		0x40	/* Receive Condition B */
#define PLAYER_ICCR_UDI		0x80	/* User Definable Interrupt */

 /* Current Transmit State Register (CTSR) bits */
#define PLAYER_CTSR_TM_MSK	0x07	/* Transmit Mode */
#define PLAYER_CTSR_TM_ATM	0x00	/* Active Transmit Mode */
#define PLAYER_CTSR_TM_ITM	0x01	/* Idle Transmit Mode */
#define PLAYER_CTSR_TM_OTM	0x02	/* Off Transmit Mode */
#define PLAYER_CTSR_TM_MTM	0x04	/* Master Transmit Mode */
#define PLAYER_CTSR_TM_HTM	0x05	/* Halt Transmit Mode */
#define PLAYER_CTSR_TM_QTM	0x06	/* Quiet Transmit Mode */
#define PLAYER_CTSR_IC_MSK	0x18	/* Injection Control */
#define PLAYER_CTSR_IC_NOINJ	0x00	/* No injection */
#define PLAYER_CTSR_IC_ONESHOT	0x08	/* One shot */
#define PLAYER_CTSR_IC_PERIOD	0x10	/* Periodic */
#define PLAYER_CTSR_IC_CONT	0x18	/* Continuous */
#define PLAYER_CTSR_SE		0x20	/* Smoother Enable */
#define PLAYER_CTSR_PRDPE	0x40	/* PHY Request Data Parity Enable */

 /* Current Receive State Register (CRSR) bits */
#define PLAYER_CRSR_LS_MSK	0x07	/* Line State Mask */
#define PLAYER_CRSR_LS_ALS	0x00	/* Active Line State */
#define PLAYER_CRSR_LS_ILS	0x01	/* Idle Line State */
#define PLAYER_CRSR_LS_NSD	0x02	/* No Signal Detect */
#define PLAYER_CRSR_LS_MLS	0x04	/* Master Line State */
#define PLAYER_CRSR_LS_HLS	0x05	/* Halt Line State */
#define PLAYER_CRSR_LS_QLS	0x06	/* Quiet Line State */
#define PLAYER_CRSR_LS_NLS	0x07	/* Noise Line State */
#define PLAYER_CRSR_LSU		0x08	/* Line State Unknown */

 /* Receive Condition Register A (RCRA), Mask (RCMRA) */
 /* and Comparison (RCCRA) bits  */
#define PLAYER_RCRA_NSD     0x01/* No Signal Detect */
#define PLAYER_RCRA_QLS     0x02/* Quiet Line State */
#define PLAYER_RCRA_HLS     0x04/* Halt Line State */
#define PLAYER_RCRA_MLS     0x08/* Master Line State */
#define PLAYER_RCRA_NLS     0x10/* Noise Line State */
#define PLAYER_RCRA_NT	    0x20/* Noise Threshold */
#define PLAYER_RCRA_LSC     0x40/* Line State Change */
#define PLAYER_RCRA_LSUPI   0x80/* Line State Unknown and PHY Invalid */

 /* Receive Condition Register B (RCRB), Mask (RCMRB) */
 /* and Comparison (RCCRB) bits */
#define PLAYER_RCRB_ILS     0x01/* Idle Line State */
#define PLAYER_RCRB_ST	    0x02/* State Threshold */
#define PLAYER_RCRB_ALS     0x04/* Active Line State */
#define PLAYER_RCRB_LSUPV   0x08/* Line State Unknown and PHY Valid */
#define PLAYER_RCRB_CSE     0x10/* Cascade Synchronization Error */
#define PLAYER_RCRB_EBOU    0x20/* Elasticity Buffer Overflow/Underflow */
#define PLAYER_RCRB_SILS    0x40/* Super Idle Line State */

 /* User Definable Register (UDR) */
#define PLAYER_UDR_SB0		0x01	/* Sense Bit 0 */
#define PLAYER_UDR_SB1		0x02	/* Sense Bit 1 */
#define PLAYER_UDR_EB0		0x04	/* Enable Bit 0 */
#define PLAYER_UDR_EB1		0x08	/* Enable Bit 1 */
#define PLAYER_UDR_LED          PLAYER_UDR_EB0 /* Turn port LED on */
#define PLAYER_UDR_BYPASS       PLAYER_UDR_EB1 /* Enable bypass */

/*
 * A note about the strange 'pad_<chipname>_<number>' variables below:
 * The registers on the National FDDI chipset are all 8 bit registers.
 * The DBUS on XX is 16 bits wide, and the hardware types don't want
 * to pull the undriven bits to 0, so we define each register as a
 * pair of uchars, the register and its pad.
 */

#define vchar volatile unsigned char
typedef struct player {	/* 32 8 bit registers, each on a short word */
    vchar pad_player_0;
    vchar mr;			/* Mode Register */
    vchar pad_player_1;
    vchar cr;			/* Configuration Register */
    vchar pad_player_2;
    vchar icr;			/* Interrupt Condition Register */
    vchar pad_player_3;
    vchar icmr;			/* Interrupt Condition Mask Register */
    vchar pad_player_4;
    vchar ctsr;			/* Current Transmit State Register */
    vchar pad_player_5;
    vchar ijtr;			/* Injection Threshold Register */
    vchar pad_player_6;
    vchar isra;			/* Injection Symbol Register A */
    vchar pad_player_7;
    vchar isrb;			/* Injection Symbol Register B */
    vchar pad_player_8;
    vchar crsr;			/* Current Receive State Register */
    vchar pad_player_9;
    vchar rcra;			/* Receive Condition Register A */
    vchar pad_player_10;
    vchar rcrb;			/* Receive Condition Register B */
    vchar pad_player_11;
    vchar rcmra;		/* Receive Condition Mask Register A */
    vchar pad_player_12;
    vchar rcmrb;		/* Receive Condition Mask Register B */
    vchar pad_player_13;
    vchar ntr;			/* Noise Threshold Register */
    vchar pad_player_14;
    vchar nptr;			/* Noise Prescale Threshold Register */
    vchar pad_player_15;
    vchar cncr;			/* Current Noise Count Register */
    vchar pad_player_16;
    vchar cnpcr;		/* Current Noise Prescale Count Register */
    vchar pad_player_17;
    vchar str;			/* State Threshold Register */
    vchar pad_player_18;
    vchar sptr;			/* State Prescale Threshold Register */
    vchar pad_player_19;
    vchar cscr;			/* Current State Count Register */
    vchar pad_player_20;
    vchar cspcr;		/* Current State Prescale Count Register */
    vchar pad_player_21;
    vchar letr;			/* Link Error Threshold Register */
    vchar pad_player_22;
    vchar clecr;		/* Current Link Error Count Register */
    vchar pad_player_23;
    vchar udr;			/* User Definable Register */
    vchar pad_player_24;
    vchar idr;			/* Device ID Register */
    vchar pad_player_25;
    vchar cijcr;		/* Current Injection Count Register */
    vchar pad_player_26;
    vchar iccr;			/* Interrupt Condition Comparison Register */
    vchar pad_player_27;
    vchar ctscr;		/* Current TX State Comparison Register */
    vchar pad_player_28;
    vchar rccra;		/* Receive Condition Comparison Register A */
    vchar pad_player_29;
    vchar rccrb;		/* Receive Condition Comparison Register B */
    vchar pad_player_30;
    vchar rr0;			/* Reserved Register 0 */
    vchar pad_player_31;
    vchar rr1;			/* Reserved Register 1 */
} player_t;

/*
 ******************************************************************************
 * National BSI (DP83265) registers:
 ******************************************************************************
 */

/*
 * MR (Mode register) bits:
 */
#define BSI_MR_SMLB	0x80	/* Small Bursts */
#define BSI_MR_SMLQ	0x40	/* Small Queue */
#define BSI_MR_VIRT	0x20	/* Virtual Address Mode */
#define BSI_MR_BIGEND	0x10	/* Big Endian Data Format */
#define BSI_MR_FLOW	0x08	/* Flow Parity */
#define BSI_MR_MRST	0x04	/* Master Reset */
#define BSI_MR_FABCLK	0x02	/* Fast Abus Clock */
#define BSI_MR_TEST	0x01	/* Test Mode */

/*
 * PCAR (Pointer RAM control and address register) bits:
 */
#define BSI_PCAR_BP	0xc0	/* Byte Pointer */
#define BSI_PCAR_READ	0x20	/* PTOP Read/Write (1=read,0=write) */
#define BSI_PCAR_ADDR	0x1f	/* Pointer RAM Address */

/*
 * MAR (Master Attention Register) bits:
 */
#define BSI_MAR_STA	0x80	/* State Attention Register */
#define BSI_MAR_NSA	0x40	/* No Space Attention Register */
#define BSI_MAR_SVA	0x20	/* Service Attention Register */
#define BSI_MAR_RQA	0x10	/* Request Attention Register */
#define BSI_MAR_INA	0x08	/* Indicate Attention Register */
#define BSI_MAR_RES	0x07	/* Reserved */

/*
 * MNR (Master Notify Register) bits:
 */
#define BSI_MNR_STAN	0x80	/* State Attention Register Notify */
#define BSI_MNR_NSAN	0x40	/* No Space Attention Register Notify */
#define BSI_MNR_SVAN	0x20	/* Service Attention Register Notify */
#define BSI_MNR_RQAN	0x10	/* Request Attention Register Notify */
#define BSI_MNR_INAN	0x08	/* Indicate Attention Register Notify */
#define BSI_MNR_RES	0x07	/* Reserved */

/*
 * STAR (State Attention Register) bits:
 */
#define BSI_STAR_ERR	0x80	/* Error */
#define BSI_STAR_BPE	0x40	/* BMAC Parity Error */
#define BSI_STAR_CPE	0x20	/* Control Bus Parity Error */
#define BSI_STAR_CWI	0x10	/* Conditional Write Inhibit */
#define BSI_STAR_CMDE	0x08	/* Command Error */
#define BSI_STAR_SPSTOP	0x04	/* Status/Space Stop */
#define BSI_STAR_RQSTOP	0x02	/* Request Stop */
#define BSI_STAR_INSTOP	0x01	/* Indicate Stop */

#define BSI_STAR_STOPBITS (BSI_STAR_SPSTOP | BSI_STAR_RQSTOP | BSI_STAR_INSTOP)

/*
 * STNR (State Notify Register) bits:
 */
#define BSI_STNR_ERR	0x80	/* Error */
#define BSI_STNR_BPE	0x40	/* BMAC Parity Error */
#define BSI_STNR_CPE	0x20	/* Control Bus Parity Error */
#define BSI_STNR_CWI	0x10	/* Conditional Write Inhibit */
#define BSI_STNR_CMDE	0x08	/* Command Error */
#define BSI_STNR_SPSTOP	0x04	/* Status/Space Stop */
#define BSI_STNR_RQSTOP	0x02	/* Request Stop */
#define BSI_STNR_INSTOP	0x01	/* Indicate Stop */

#define BSI_STNR_STOPBITS (BSI_STNR_SPSTOP | BSI_STNR_RQSTOP | BSI_STNR_INSTOP)

/*
 * SAR (Service Attention Register) bits:
 */
#define BSI_SAR_RES	0xf0	/* Reserved */
#define BSI_SAR_ABR0	0x08	/* RCHN0 Abort Request */
#define BSI_SAR_ABR1	0x04	/* RCHN1 Abort Request */
#define BSI_SAR_LMOP	0x02	/* Limit RAM Operation */
#define BSI_SAR_PTOP	0x01	/* Pointer RAM Operation */

/*
 * SNR (Service Notify Register) bits:
 */
#define BSI_SNR_RES	0xf0	/* Reserved */
#define BSI_SNR_ABR0N	0x08	/* RCHN0 Abort Request Notify */
#define BSI_SNR_ABR1N	0x04	/* RCHN1 Abort Request Notify */
#define BSI_SNR_LMOPN	0x02	/* Limit RAM Operation Notify */
#define BSI_SNR_PTOPN	0x01	/* Pointer RAM Operation Notify */

/*
 * NSAR (No Space Attention Register) bits:
 */
#define BSI_NSAR_NSR0	0x80	/* No Status Space on RCHN0 */
#define BSI_NSAR_NSR1	0x40	/* No Status Space on RCHN1 */
#define BSI_NSAR_LDI0	0x20	/* Low Data Space on ICHN0 */
#define BSI_NSAR_NSI0	0x10	/* No Status Space on ICHN0 */
#define BSI_NSAR_LDI1	0x08	/* Low Data Space on ICHN1 */
#define BSI_NSAR_NSI1	0x04	/* No Status Space on ICHN1 */
#define BSI_NSAR_LDI2	0x02	/* Low Data Space on ICHN2 */
#define BSI_NSAR_NSI2	0x01	/* No Status Space on ICHN2 */

/*
 * NSNR (No Space Notify Register) bits:
 */
#define BSI_NSNR_NSR0N	0x80	/* No Status Space on RCHN0 Notify */
#define BSI_NSNR_NSR1N	0x40	/* No Status Space on RCHN1 Notify */
#define BSI_NSNR_LDI0N	0x20	/* Low Data Space on ICHN0 Notify */
#define BSI_NSNR_NSI0N	0x10	/* No Status Space on ICHN0 Notify */
#define BSI_NSNR_LDI1N	0x08	/* Low Data Space on ICHN1 Notify */
#define BSI_NSNR_NSI1N	0x04	/* No Status Space on ICHN1 Notify */
#define BSI_NSNR_LDI2N	0x02	/* Low Data Space on ICHN2 Notify */
#define BSI_NSNR_NSI2N	0x01	/* No Status Space on ICHN2 Notify */

/*
 * LAR (Limit Address Register) bits:
 */
#define BSI_LAR_LRA	0xf0	/* Limit RAM Register Address */
#define BSI_LAR_READ	0x08	/* LMOP Read/Write (1=read,0=write) */
#define BSI_LAR_RES	0x06	/* Reserved */
#define BSI_LAR_LRD8	0x01	/* Limit RAM Data Bit 8 */

#define BSI_LCAR_ADDR	0x0f	/* 4 bit limit RAM address field */
#define BSI_LCAR_ADDR_SHIFT	4
#define BSI_LCAR_DATA		0x1ff	/* 9 bits in data field */
#define BSI_LCAR_D8		0x100	/* MS Bit of data */
#define BSI_LCAR_D8_SHIFT	8
#define BSI_LCAR_LSB_DATA	0xff	/* LS Byte of data */

#define BSI_PTR_TO_LIMIT(ptr)	((((uint)ptr) >> 3) & BSI_LCAR_DATA)

/*
 * RAR (Request Attention Register) bits:
 */
#define BSI_RAR_USRR0	0x80	/* Unserviceable Request on RCHN0 */
#define BSI_RAR_RCMR0	0x40	/* Request Complete on RCHN0 */
#define BSI_RAR_EXCR0	0x20	/* Exception on RCHN0 */
#define BSI_RAR_BRKR0	0x10	/* Breakpoint on RCHN0 */
#define BSI_RAR_USRR1	0x08	/* Unserviceable Request on RCHN1 */
#define BSI_RAR_RCMR1	0x04	/* Request Complete on RCHN1 */
#define BSI_RAR_EXCR1	0x02	/* Exception on RCHN1 */
#define BSI_RAR_BRKR1	0x01	/* Breakpoint on RCHN1 */

/*
 * RNR (Request Notify Register) bits:
 */
#define BSI_RNR_USRR0N	0x80	/* Unserviceable Request on RCHN0 Notify */
#define BSI_RNR_RCMR0N	0x40	/* Request Complete on RCHN0 Notify */
#define BSI_RNR_EXCR0N	0x20	/* Exception on RCHN0 Notify */
#define BSI_RNR_BRKR0N	0x10	/* Breakpoint on RCHN0 Notify */
#define BSI_RNR_USRR1N	0x08	/* Unserviceable Request on RCHN1 Notify */
#define BSI_RNR_RCMR1N	0x04	/* Request Complete on RCHN1 Notify */
#define BSI_RNR_EXCR1N	0x02	/* Exception on RCHN1 Notify */
#define BSI_RNR_BRKR1N	0x01	/* Breakpoint on RCHN1 Notify */

/*
 * R0CR or R1CR (Request Channel [01] Configuration Register) bits:
 */
#define BSI_RxCR_TT_MSK	0xc0	/* Transmit Threshhold mask */
#define BSI_RxCR_TT_8	0	/*  transmit threshold is 8 words */
#define BSI_RxCR_TT_16	0x40	/*  transmit threshold is 16 words */
#define BSI_RxCR_TT_128	0x80	/*  transmit threshold is 128 words */
#define BSI_RxCR_TT_256	0xc0	/*  transmit threshold is 256 words */
#define BSI_RxCR_PRE	0x20	/* Preempt/Prestage */
#define BSI_RxCR_HLD	0x10	/* Hold */
#define BSI_RxCR_FCT	0x08	/* Frame Control Transparency */
#define BSI_RxCR_SAT	0x04	/* Source Address Transparency */
#define BSI_RxCR_VST	0x02	/* Void Stripping */
#define BSI_RxCR_FCS	0x01	/* Frame Check Sequence Disable */

/*
 * R0EFSR or R1EFSR (Request Channel [01] Expected Frame Status Register) bits:
 */
#define BSI_RxEFSR_VDL	0x80	/* Valid Data Length */
#define BSI_RxEFSR_VFCS	0x40	/* Valid FCS */
#define BSI_RxEFSR_EE	0x30	/* Expected E Indicator (Error Detected) */
#define BSI_RxEFSR_EA	0x0c	/* Expected A Indicator (Address Recognized) */
#define BSI_RxEFSR_EC	0x03	/* Expected C Indicator (Frame Copied) */

/*
 * IAR (Indicate Attention Register) bits:
 */
#define BSI_IAR_RES	0xc0	/* Reserved */
#define BSI_IAR_EXCI0	0x20	/* Exception on ICHN0 */
#define BSI_IAR_BRKI0	0x10	/* Breakpoint on ICHN0 */
#define BSI_IAR_EXCI1	0x08	/* Exception on ICHN1 */
#define BSI_IAR_BRKI1	0x04	/* Breakpoint on ICHN1 */
#define BSI_IAR_EXCI2	0x02	/* Exception on ICHN2 */
#define BSI_IAR_BRKI2	0x01	/* Breakpoint on ICHN0 */

/*
 * INR (Indicate Notify Register) bits:
 */
#define BSI_INR_RES	0xc0	/* Reserved */
#define BSI_INR_EXC0N	0x20	/* Exception on ICHN0 Notify */
#define BSI_INR_BRK0N	0x10	/* Breakpoint on ICHN0 Notify */
#define BSI_INR_EXC1N	0x08	/* Exception on ICHN1 Notify */
#define BSI_INR_BRK1N	0x04	/* Breakpoint on ICHN1 Notify */
#define BSI_INR_EXC2N	0x02	/* Exception on ICHN2 Notify */
#define BSI_INR_BRK2N	0x01	/* Breakpoint on ICHN0 Notify */

/*
 * IMR (Indicate Mode Register) bits:
 * Note that BOT1 and BOT2 were exchanged, as indicated on page 19
 * of the Dec 20, 1991, rev H, "Proprietary User Information
 * for National Semiconductor FDDI Products", (PUIFNSFP).
 */
#define BSI_IMR_SM		0xc0 /* Sort Mode */
#define BSI_IMR_SM_SHIFT	6
#define BSI_IMR_SKIP		0x20 /* Skip Enabled */
#define BSI_IMR_FRAM_PAGE	0x10 /* Enable frame per page mode */
#define BSI_IMR_BOT2		0x08 /* Breakpoint on Threshold for ICHN2 */
#define BSI_IMR_BOT1		0x04 /* Breakpoint on Threshold for ICHN1 */
#define BSI_IMR_BOB		0x02 /* Breakpoint on Burst */
#define BSI_IMR_BOS		0x01 /* Breakpoint on Service Opportunity */

#define BSI_IMR_SM_ASYNCH_SYNC	0
#define BSI_IMR_SM_EXTRN_INTRN	1
#define BSI_IMR_SM_INFO_HEADER	2
#define BSI_IMR_SM_LO_HI_PRTY	3


/*
 * ICR (Indicate Configuration Register) bits:
 */
#define BSI_ICR_CC0		0xc0	/* Copy Control ICHN0 */
#define BSI_ICR_CC0_SHIFT	6
#define BSI_ICR_RES0		0x20	/* Reserved */
#define BSI_ICR_CC1		0x18	/* Copy Control ICHN1 */
#define BSI_ICR_CC1_SHIFT	3
#define BSI_ICR_RES1		0x04	/* Reserved */
#define BSI_ICR_CC2		0x03	/* Copy Control ICHN2 */

#define BSI_ICR_NO_COPY			0
#define BSI_ICR_COPY_OURS		1
#define BSI_ICR_COPY_OURS_HEARSELF	2
#define BSI_ICR_COPY_PROMISCUOUSLY	3

 /*
  * BSI registers:
  */
typedef struct bsi {	/* 32 registers */
   vchar pad_bsi_0;
   vchar mr;		/* Mode Register */
   vchar pad_bsi_1;
   vchar res_01;	/* Reserved */
   vchar pad_bsi_2;
   vchar pcar;		/* Pointer RAM Control and Address Register */
   vchar pad_bsi_3;
   vchar mbar;		/* Mailbox Address Register (also silicon rev code) */
   vchar pad_bsi_4;
   vchar mar;		/* Master Attention Register */
   vchar pad_bsi_5;
   vchar mnr;		/* Master Notify Register */
   vchar pad_bsi_6;
   vchar star;		/* State Attention register */
   vchar pad_bsi_7;
   vchar stnr;		/* State Notify register */
   vchar pad_bsi_8;
   vchar sar;		/* Service Attention register */
   vchar pad_bsi_9;
   vchar snr;		/* Service Notify register */
   vchar pad_bsi_10;
   vchar nsar;		/* No Space Attention register */
   vchar pad_bsi_11;
   vchar nsnr;		/* No Space Notify register */
   vchar pad_bsi_12;
   vchar lar;		/* Limit Address register (see pg 26 of PUIFNSFP) */
   vchar pad_bsi_13;
   vchar ldr;		/* Limit Data register (see pg 26 of PUIFNSFP) */
   vchar pad_bsi_14;
   vchar rar;		/* Request attention */
   vchar pad_bsi_15;
   vchar rnr;		/* Request notify */
   vchar pad_bsi_16;
   vchar r0cr;		/* RCHN0 Request configuration register */
   vchar pad_bsi_17;
   vchar r1cr;		/* RCHN1 Request configuration register */
   vchar pad_bsi_18;
   vchar r0efsr;	/* RCHN0 Request Expected Frame Status */
   vchar pad_bsi_19;
   vchar r1efsr;	/* RCHN1 Request Expected Frame Status */
   vchar pad_bsi_20;
   vchar iar;		/* Indicate Attention register */
   vchar pad_bsi_21;
   vchar inr;		/* Indicate Notify register */
   vchar pad_bsi_22;
   vchar itr;		/* Indicate Threshold */
   vchar pad_bsi_23;
   vchar imr;		/* Indicate Mode register */
   vchar pad_bsi_24;
   vchar icr;		/* Indicate Configuration register */
   vchar pad_bsi_25;
   vchar ihlr;		/* Indicate Header Length */
   vchar pad_bsi_26;
   vchar res_1a;	/* Reserved */
   vchar pad_bsi_27;
   vchar res_1b;	/* Reserved */
   vchar pad_bsi_28;
   vchar res_1c;	/* Reserved */
   vchar pad_bsi_29;
   vchar res_1d;	/* Reserved */
   vchar pad_bsi_30;
   vchar res_1e;	/* Reserved */
   vchar pad_bsi_31;
   vchar cmp;		/* Conditional Write Comparison register */
} bsi_t;


/*
 * Silicon Rev Codes (In Memory Mailbox)
 */
#define BSI_SILICON_BSI_1A 0x00
#define BSI_SILICON_BSI_1B 0x04
#define BSI_SILICON_BSI_2A 0x40
#define BSI_SILICON_BSI_2B 0x4c

#define BSIQ_NEXT_1K(p)\
       (((((uint)p) + 8) & 0x3ff) | (((uint)p) & 0x0ffff000))
#define BSIQ_PREV_1K(p)\
       (((((uint)p) - 8) & 0x3ff) | (((uint)p) & 0x0ffff000))
#define BSIQ_FULL_1K(q)\
       ((BSIQ_NEXT_1K((q)->limit) == BSIQ_PREV_1K((q)->ptr)) ? 1 : 0)

#define BSI_4KB_PAGE_SIZE	4096
#define BSI_4K_PAGE(a)	((uint)a & (~(uint)0x00000fff))
#define BSI_8K_PAGE(a)	((uint)a & (~(uint)0x00001fff))

/* Convert a 28 bit BSI address to a 68k 32 bit address */
#define BSI_TO_68K(a)	((BSI_ADDR28 & (uint)a) | ADRSPC_SHAREDMEM)


 /* PTR RAM Addresses */
#define BSI_OPR1        0x00	/* RCHN1 ODU Pointer */
#define BSI_OLPR1       0x01	/* RCHN1 ODUD List Pointer */
#define BSI_CQPR1       0x02	/* RCHN1 CNF Queue Pointer */
#define BSI_RQPR1       0x03	/* RCHN1 REQ Queue Pointer */
#define BSI_OPR0        0x04	/* RCHN0 ODU Pointer */
#define BSI_OLPR0       0x05	/* RCHN0 ODUD List Pointer */
#define BSI_CQPR0       0x06	/* RCHN0 CNF Queue Pointer */
#define BSI_RQPR0       0x07	/* RCHN0 REQ Queue Pointer */
#define BSI_IPI2        0x08	/* ICHN2 IDU Pointer */
#define BSI_IQPI2       0x09	/* ICHN2 IDUD Queue Pointer */
#define BSI_PQPI2       0x0a	/* ICHN2 PSP Queue Pointer */
#define BSI_NPI2        0x0b	/* ICHN2 Next PSP */
#define BSI_IPI1        0x0c	/* ICHN1 IDU Pointer */
#define BSI_IQPI1       0x0d	/* ICHN1 IDUD Queue Pointer */
#define BSI_PQPI1       0x0e	/* ICHN1 PSP Queue Pointer */
#define BSI_NPI1        0x0f	/* ICHN1 Next PSP */
#define BSI_IPI0        0x10	/* ICHN0 IDU Pointer */
#define BSI_IQPI0       0x11	/* ICHN0 IDUD Queue Pointer */
#define BSI_PQPI0       0x12	/* ICHN0 PSP Queue Pointer */
#define BSI_NPI0        0x13	/* ICHN0 Next PSP */
#define BSI_ISR         0x14	/* IDU Shadow Register */
#define BSI_OSR         0x15	/* ODUD Shadow Register */

 /* LIMIT RAM Addresses */
#define BSI_RQLR1       0x00	/* RCHN1 REQ Queue Limit */
#define BSI_CQLR1       0x01	/* RCHN1 CNF Queue Limit */
#define BSI_RQLR0       0x02	/* RCHN0 REQ Queue Limit */
#define BSI_CQLR0       0x03	/* RCHN0 CNF Queue Limit */
#define BSI_IQLI2       0x04	/* ICHN2 IDUD Queue Limit */
#define BSI_PQLI2       0x05	/* ICHN2 PSP Queue Limit */
#define BSI_IQLI1       0x06	/* ICHN1 IDUD Queue Limit */
#define BSI_PQLI1       0x07	/* ICHN1 PSP Queue Limit */
#define BSI_IQLI0       0x08	/* ICHN0 IDUD Queue Limit */
#define BSI_PQLI0       0x09	/* ICHN0 PSP Queue Limit */

/* BSI data structures */

typedef unsigned long bsi_addr_t;
#define BSI_ADDR28	0x0fffffff
#define BSI_ADDRFL	0xc0000000
#define BSI_FIRST	0x80000000
#define BSI_LAST	0x40000000
#define BSI_ONLY	0xc0000000
#define BSI_MIDDLE	0x00000000
#define BSI_DUD_CNT	0x00001fff	/* 13 bit mask */

#define SET_BSI_FIRST_DUD(dest, source)\
    ((ulong)(dest) = (ulong)(BSI_FIRST | ((bsi_addr_t)(source) & BSI_ADDR28)))
#define SET_BSI_LAST_DUD(dest, source)\
    ((ulong)(dest) = (ulong)(BSI_LAST | ((bsi_addr_t)(source) & BSI_ADDR28)))
#define SET_BSI_ONLY_DUD(dest, source)\
    ((ulong)(dest) = (ulong)(BSI_ONLY | ((bsi_addr_t)(source) & BSI_ADDR28)))
#define SET_BSI_MIDDLE_DUD(dest, source)\
    ((ulong)(dest) = (ulong)((bsi_addr_t)(source) & BSI_ADDR28))
#define SET_BSI_DUD_CNT(dest, source)\
    (*(ulong *)(dest) = (source & BSI_DUD_CNT))

 /* general BSI descriptor */
typedef struct descr {
   unsigned long w0;
   unsigned long w1;
} descr_t;

 /* PSP descriptor */
 /* Note that location must be aligned according to the enabled burst size */
 /* See page 20 of the Dec 20, 1991, rev H, "Proprietary User Information  */
 /* for National Semiconductor FDDI Products", PUIFNSFP.                   */
#ifdef BIGEND
typedef struct psp_descr {
   unsigned    res1a:16;	/* reserved */
   unsigned    res1b:3;		/* reserved */
   unsigned    cnt:13;		/* Byte Count */
   bsi_addr_t  loc;		/* Location */
} psp_descr_t;

#else
typedef struct psp_descr {
   unsigned    cnt:13;		/* Byte Count */
   unsigned    res1b:3;		/* reserved */
   unsigned    res1a:16;	/* reserved */
   bsi_addr_t  loc;		/* Location */
} psp_descr_t;

#endif

 /* IDUD descriptor */
#ifdef BIGEND
typedef struct idud_descr {
   unsigned    is:4;		/* Indicate Status */
   unsigned    fra_mflag:1;	/* MFLAG */
   unsigned    fra_aflag:1;	/* AFLAG */
   unsigned    fra_tc:2;	/* Terminating Condition */
   unsigned    frs_vdl:1;	/* Valid Data Length */
   unsigned    frs_vfcs:1;	/* Valid FCS */
   unsigned    frs_e:2;		/* E Indicator */
   unsigned    frs_a:2;		/* A Indicator */
   unsigned    frs_c:2;		/* C Indicator */
   unsigned    vc:1;		/* VCOPY */
   unsigned    res:2;		/* Reserved */
   unsigned    cnt:13;		/* Byte Count of IDU */
   bsi_addr_t  loc;		/* location of IDU */
} idud_descr_t;

#else
typedef struct idud_descr {
   unsigned    cnt:13;		/* Byte Count of IDU */
   unsigned    res:2;		/* Reserved */
   unsigned    vc:1;		/* VCOPY */
   unsigned    frs_c:2;		/* C Indicator */
   unsigned    frs_a:2;		/* A Indicator */
   unsigned    frs_e:2;		/* E Indicator */
   unsigned    frs_vfcs:1;	/* Valid FCS */
   unsigned    frs_vdl:1;	/* Valid Data Length */
   unsigned    fra_tc:2;	/* Terminating Condition */
   unsigned    fra_aflag:1;	/* AFLAG */
   unsigned    fra_mflag:1;	/* MFLAG */
   unsigned    is:4;		/* Indicate Status */
   bsi_addr_t  loc;		/* location of IDU */
} idud_descr_t;

#endif

 /* IDUD Indicate Status definitions */
#define BSI_IDUD_IS_LAST_PAGEX           0 /* PSP queue is empty! */
#define BSI_IDUD_IS_PAGEX                1
#define BSI_IDUD_IS_HEADER               2
#define BSI_IDUD_IS_HEADER_PAGEX         3
#define BSI_IDUD_IS_INTERMEDIATE         4
#define BSI_IDUD_IS_BURST                5
#define BSI_IDUD_IS_THRESHOLD            6
#define BSI_IDUD_IS_SERVOP               7
#define BSI_IDUD_IS_NO_DATA_SPACE        8 /* copy aborted */
#define BSI_IDUD_IS_NO_HEADER_SPACE      9 /* copy aborted */
#define BSI_IDUD_IS_NO_INFO_SPACE       10 /* copy aborted */
#define BSI_IDUD_IS_INCOMPLETE_INFO     11 /* copy aborted */
#define BSI_IDUD_IS_FIFO_OVERRUN        12 /* error */
#define BSI_IDUD_IS_BAD_FRAME           13 /* error */
#define BSI_IDUD_IS_PARITY_ERR          14 /* error */
#define BSI_IDUD_IS_INTERNAL_ERR        15 /* error */

#define BSI_IDUD_CNT_MASK       0x00001FFF
#define BSI_IDUD_VC_MASK        0x00008000
#define BSI_IDUD_FRS_MASK       0x00FF0000
#define BSI_IDUD_FRS_C_MASK     0x00030000
#define BSI_IDUD_FRS_A_MASK     0x000C0000
#define BSI_IDUD_FRS_E_MASK     0x00300000
#define BSI_IDUD_FRS_VALID_MASK 0x00C00000
#define BSI_IDUD_FRA_MASK       0x0F000000
#define BSI_IDUD_FRA_TC_MASK    0x03000000
#define BSI_IDUD_IS_MASK        0xF0000000

#define BSI_IDUD_FRS_C_N        0x00000000
#define BSI_IDUD_FRS_C_R        0x00010000
#define BSI_IDUD_FRS_C_S        0x00020000
#define BSI_IDUD_FRS_C_T        0x00030000

#define BSI_IDUD_FRS_A_N        0x00000000
#define BSI_IDUD_FRS_A_R        0x00040000
#define BSI_IDUD_FRS_A_S        0x00080000
#define BSI_IDUD_FRS_A_T        0x000C0000

#define BSI_IDUD_FRS_E_N        0x00000000
#define BSI_IDUD_FRS_E_R        0x00100000
#define BSI_IDUD_FRS_E_S        0x00200000
#define BSI_IDUD_FRS_E_T        0x00300000

#define BSI_IDUD_FRS_VFCS       0x00400000
#define BSI_IDUD_FRS_VDL        0x00800000

#define BSI_IDUD_FRA_TC_ED      0x01000000
#define BSI_IDUD_FRA_TC_FE      0x02000000
#define BSI_IDUD_FRA_TC_FS      0x03000000

#define BSI_IDUD_FRA_AFLAG      0x04000000
#define BSI_IDUD_FRA_MFLAG      0x08000000

#define BSI_IDUD_IS_ERROR       0x80000000

#define BSI_IDUD_VALID_MASK     (BSI_IDUD_FRA_TC_MASK | \
                                 BSI_IDUD_FRS_VALID_MASK | \
                                 BSI_IDUD_IS_ERROR)

#define BSI_IDUD_VALID_MATCH    (BSI_IDUD_FRA_TC_ED | \
                                 BSI_IDUD_FRS_VALID_MASK)

#define BSI_IDUD_INVALID_FRS(s) ((((s)&BSI_IDUD_FRS_E_MASK) == BSI_IDUD_FRS_E_N) || \
				 (((s)&BSI_IDUD_FRS_E_MASK) == BSI_IDUD_FRS_E_T) || \
				 (((s)&BSI_IDUD_FRS_A_MASK) == BSI_IDUD_FRS_A_N) || \
				 (((s)&BSI_IDUD_FRS_A_MASK) == BSI_IDUD_FRS_A_T) || \
				 (((s)&BSI_IDUD_FRS_C_MASK) == BSI_IDUD_FRS_C_N) || \
				 (((s)&BSI_IDUD_FRS_C_MASK) == BSI_IDUD_FRS_C_T))


#define BSI_IDUD_IS_COPY_ABORT	0x8 /* Is set for all above copy aborts
				       or errors*/
#define BSI_NULL_IDUD_LOC	0x1
#define BSI_LS2B_IDUD_LOC	0x3

#define BSI_VALID_IDUD_DESC(x)\
    (((uint)x & BSI_LS2B_IDUD_LOC) != BSI_NULL_IDUD_LOC)
#define BSI_ERROR_IDUD_DESC(x)\
    ((x & BSI_IDUD_VALID_MASK) != BSI_IDUD_VALID_MATCH)

#define BSI_IDUD_GET_LENGTH(x)  ((x) & BSI_IDUD_CNT_MASK)

 /* Output Data Unit Descriptor (ODUD) */
#ifdef BIGEND
typedef struct odud_descr {
   unsigned    res1a:16;	/* Reserved */
   unsigned    res1b:3;		/* Reserved */
   unsigned    cnt:13;		/* Byte Count of ODU */
   bsi_addr_t  loc;		/* Location of ODU */
} odud_descr_t;

#else
typedef struct odud_descr {
   unsigned    cnt:13;		/* Byte Count of ODU */
   unsigned    res1b:3;		/* Reserved */
   unsigned    res1a:16;	/* Reserved */
   bsi_addr_t  loc;		/* Location of ODU */
} odud_descr_t;

#endif

 /* Request descriptor (REQ) */
#ifdef BIGEND
typedef struct req_descr {
   unsigned    res:2;		/* Reserved */
   unsigned    uid:6;		/* User Identification */
   unsigned    size:8;		/* Size of REQ in Frames */
   unsigned    cnfcls:4;	/* Confirmation Class */
   unsigned    rqcls:4;		/* Request/Release Class */
   unsigned    fc:8;		/* Frame Control */
   bsi_addr_t  loc;		/* Location of ODUD array */
} req_descr_t;

#else
typedef struct req_descr {
   unsigned    fc:8;		/* Frame Control */
   unsigned    rqcls:4;		/* Request/Release Class */
   unsigned    cnfcls:4;	/* Confirmation Class */
   unsigned    size:8;		/* Size of REQ in Frames */
   unsigned    uid:6;		/* User Identification */
   unsigned    res:2;		/* Reserved */
   bsi_addr_t  loc;		/* Location of ODUD array */
} req_descr_t;

#endif


 /* Request Class Values */
#define BSI_REQ_RQCLS_NONE	0	/* None */
#define BSI_REQ_RQCLS_APR1	1	/* Async Priority 1 */
#define BSI_REQ_RQCLS_APR2	2	/* Async Priority 2 */
#define BSI_REQ_RQCLS_APR3	3	/* Async Priority 3 */
#define BSI_REQ_RQCLS_SYN	4	/* Sync */
#define BSI_REQ_RQCLS_IMM	5	/* Immediate, issue no token */
#define BSI_REQ_RQCLS_IMMN	6	/* Immediate, issue non-restrict tok */
#define BSI_REQ_RQCLS_IMMR	7	/* Immediate, issue restricted token */
#define BSI_REQ_RQCLS_ASYN	8	/* Async */
#define BSI_REQ_RQCLS_RBEG	9	/* Restricted Begin */
#define BSI_REQ_RQCLS_REND	10	/* Restricted End */
#define BSI_REQ_RQCLS_RCNT	11	/* Restricted Continue */
#define BSI_REQ_RQCLS_ASYND	12	/* Async without THT */
#define BSI_REQ_RQCLS_RBEGD	13	/* Restricted Begin without THT */
#define BSI_REQ_RQCLS_RENDD	14	/* Restricted End without THT */
#define BSI_REQ_RQCLS_RCNTD	15	/* Restricted Continue with THT */
 /* THT = Token Hold Timer */

 /* Confirmation Class Values */
 /* CNF only on exception */
#define BSI_REQ_CNFCLS_NONE	0
 /* Transmitter confirm, CNF on exception or completion */
#define BSI_REQ_CNFCLS_TEND	1
 /* Transmitter confirm, CNF on exception, completion or intermediate */
#define BSI_REQ_CNFCLS_TINT	3
 /* Transmitter confirm, CNF on exception or completion */
#define BSI_REQ_CNFCLS_FEND	5
 /* Full confirm, CNF on exception, completion or intermediate */
#define BSI_REQ_CNFCLS_FINT	7
 /* Transmitter confirm, CNF on exception or completion, repeat frame */
#define BSI_REQ_CNFCLS_TENDR	9
 /* Transmitter confirm, CNF on exc, compl or intermed, repeat frame */
#define BSI_REQ_CNFCLS_TINTR	11
 /* CNF only on exception, repeat frame */
#define BSI_REQ_CNFCLS_NONER	12
 /* Full confirm, CNF on exception or completion, repeat frame */
#define BSI_REQ_CNFCLS_FENDR	13
 /* Full confirm, CNF on exc, compl or intermediate, repeat frame */
#define BSI_REQ_CNFCLS_FINTR	15

 /* CNF descriptor */
#ifdef BIGEND
typedef struct cnf_descr {
   /* first word */
   unsigned    rs:4;		/* Request Status */
   unsigned    fra_mflag:1;	/* MFLAG */
   unsigned    fra_aflag:1;	/* AFLAG */
   unsigned    fra_tc:2;	/* Terminating Condition */
   unsigned    frs_vdl:1;	/* Valid Data Length */
   unsigned    frs_vfcs:1;	/* Valid FCS */
   unsigned    frs_e:2;		/* E Indicator */
   unsigned    frs_a:2;		/* A Indicator */
   unsigned    frs_c:2;		/* C Indicator */
   unsigned    tfc:8;		/* Transmitted Frame Count */
   unsigned    cfc:8;		/* Confirmed Frame Count */
   /* second word */
   unsigned    f:1;		/* first bit */
   unsigned    l:1;		/* last bit */
   unsigned    uid:6;		/* User Identification */
   unsigned    fc:8;		/* Frame Control */
   unsigned    cs_t:1;		/* Transmit Class */
   unsigned    cs_r:1;		/* Ring-Op */
   unsigned    cs_e:1;		/* Exception */
   unsigned    cs_p:1;		/* Parity */
   unsigned    cs_u:1;		/* Unexpected Frame Status */
   unsigned    cs_f:1;		/* Full Confirm */
   unsigned    cs_ft:2;		/* Frame Type */
   unsigned    res:8;		/* Reserved */
} cnf_descr_t;

#else
typedef struct cnf_descr {
   /* first word */
   unsigned    cfc:8;		/* Confirmed Frame Count */
   unsigned    tfc:8;		/* Transmitted Frame Count */
   unsigned    frs_c:2;		/* C Indicator */
   unsigned    frs_a:2;		/* A Indicator */
   unsigned    frs_e:2;		/* E Indicator */
   unsigned    frs_vfcs:1;	/* Valid FCS */
   unsigned    frs_vdl:1;	/* Valid Data Length */
   unsigned    fra_tc:2;	/* Terminating Condition */
   unsigned    fra_aflag:1;	/* AFLAG */
   unsigned    fra_mflag:1;	/* MFLAG */
   unsigned    rs:4;		/* Request Status */
   /* second word */
   unsigned    res:8;		/* Reserved */
   unsigned    cs_ft:2;		/* Frame Type */
   unsigned    cs_f:1;		/* Full Confirm */
   unsigned    cs_u:1;		/* Unexpected Frame Status */
   unsigned    cs_p:1;		/* Parity */
   unsigned    cs_e:1;		/* Exception */
   unsigned    cs_r:1;		/* Ring-Op */
   unsigned    cs_t:1;		/* Transmit Class */
   unsigned    fc:8;		/* Frame Control */
   unsigned    uid:6;		/* User Identification */
   unsigned    l:1;		/* last bit */
   unsigned    f:1;		/* first bit */
} cnf_descr_t;

#endif

 /* CNF Request Status Values */
#define BSI_CNF_RS_NONE		0
#define BSI_CNF_RS_PREEMPTED	1
#define BSI_CNF_RS_PART_DONE	2
#define BSI_CNF_RS_SERV_LOSS	3
#define BSI_CNF_RS_COMPLETED_BC	5
#define BSI_CNF_RS_COMPLETED_OK	6
#define BSI_CNF_RS_BAD_CONF	7
#define BSI_CNF_RS_UNDERRUN	8
#define BSI_CNF_RS_HOST_ABORT	9
#define BSI_CNF_RS_BAD_RINGOP	10
#define BSI_CNF_RS_MAC_ABORT	11
#define BSI_CNF_RS_TIMEOUT	12
#define BSI_CNF_RS_MAC_RESET	13
#define BSI_CNF_RS_CON_FAILURE	14
#define BSI_CNF_RS_FATAL_ERROR	15

#define BSI_CNF_RS_VALID_MASK   0xF0000000
#define BSI_CNF_RS_VALID_MATCH  0x60000000

#define BSI_CNF_FT_ANY_OTHER	0
#define BSI_CNF_FT_TOKEN	1
#define BSI_CNF_FT_OTHER_VOID	2
#define BSI_CNF_FT_MY_VOID	3

#define BSI_VALID_CNF_DESC(cnf_desc_ptr)\
	(cnf_desc_ptr->f || cnf_desc_ptr->l)
#define BSI_ERROR_CNF_DESC(cnf_descr_ptr)\
    ((*(ulong *)cnf_descr_ptr & BSI_CNF_RS_VALID_MASK) != BSI_CNF_RS_VALID_MATCH)


 /* E/A/C Indicator Values */
#define BSI_INDICATOR_NONE	0
#define BSI_INDICATOR_R		1
#define BSI_INDICATOR_S		2
#define BSI_INDICATOR_T		3

 /* Terminating Condition Values */
#define BSI_TC_OTHER		0	/* other condition */
#define BSI_TC_ED		1	/* ED Symbol */
#define BSI_TC_FORMAT_ERR	2	/* Frame Format Error */
#define BSI_TC_FRAME_STRIPPED	3	/* Frame Stripped */
#define BSI_TC_MASK		0x3

/* Number of Descriptors Pipelined for output */
#define BSI_CNF_PIPELINE_SIZE	2
#define BSI_IDUD_PIPELINE_SIZE	2

#define BSI_QSIZE	1024

#define BSI_CNF_QSIZE	(BSI_QSIZE/sizeof(cnf_descr_t))
#define BSI_REQ_QSIZE	(BSI_QSIZE/sizeof(req_descr_t))
#define BSI_IDUD_QSIZE	(BSI_QSIZE/sizeof(idud_descr_t))
#define BSI_PSP_QSIZE	(BSI_QSIZE/sizeof(psp_descr_t))

/*
 * BSI Queue types:
 */

/* Confirmation status message queue */
typedef struct cnf_descrq cnf_descrQ_t;
struct cnf_descrq {
    cnf_descr_t q[BSI_CNF_QSIZE];
};

/* Request descriptor queue */
typedef struct req_descrq req_descrQ_t;
struct req_descrq {
    req_descr_t q[BSI_REQ_QSIZE];
};

/* Input data unit descriptor queue */
typedef struct idud_descrq idud_descrQ_t;
struct idud_descrq {
    idud_descr_t q[BSI_IDUD_QSIZE];
};

/* Pool space descriptor queue */
typedef struct psp_descrq psp_descrQ_t;
struct psp_descrq {
    psp_descr_t q[BSI_PSP_QSIZE];
};

/*
 * Cache ring descriptor definitions
 */
typedef struct bsi_ring_entry bsi_ring_entry_t;
struct bsi_ring_entry {
    paktype          *addr;
    bsi_ring_entry_t *next;
    bsi_ring_entry_t *prev;
};

typedef struct bsi_ring bsi_ring_t;
struct bsi_ring {
    bsi_ring_entry_t  *ring;
    bsi_ring_entry_t  *limit;
    bsi_ring_entry_t  *ptr;
    int                size;
};

#define BSIQ_FULL(x) ((x).limit->next == (x).ptr->prev)

/*
 ******************************************************************************
 * Music MU9C1480 CAM Registers
 ******************************************************************************
 */

/*
 * CAM LCA software states
 */

#define CAM_FIRMWARE_NONE   0x0000
#define CAM_FIRMWARE_ROM    0x0001
#define CAM_FIRMWARE_SOFT   0x0002
#define CAM_FIRMWARE_FAIL   0x0003

/*
 * CAM LCR bit definitions
 */
#define CAM_LCA_CONFIG_MODE 0x0000
#define CAM_LCA_OPERATIONAL 0x0001

/* 
 * CAM CSR bit definitions
 */
#define CAM_CSR_FIFO_GO            0x8000
#define CAM_CSR_FIFO_STATUS        0x8000
#define CAM_CSR_FIFO_DONE          0x4000
#define CAM_CSR_DA_HIT             0x2000
#define CAM_CSR_SA_MISS            0x1000
#define CAM_CSR_FIFO_CAM_DATA      0x0800
#define CAM_CSR_FIFO_FULL          0x0400
#define CAM_CSR_FIFO_READY         0x0200
#define CAM_CSR_CAM_FULL           0x0100
#define CAM_CSR_CAM_COMPARE        0x0080
#define CAM_CSR_MULTICAST_ENABLE   0x0020
#define CAM_CSR_FIFO_CLEAR         0x0040
#define CAM_CSR_FIFO_DONE_INTR_EN  0x0010
#define CAM_CSR_SA_MISS_INTR_EN    0x0008
#define CAM_CSR_RIF_SCAN_ENABLE    0x0004
#define CAM_CSR_CAM_COPY_HIT       0x0002
#define CAM_CSR_CAM_COMPARE_ENABLE 0x0001

#define CAM_CSR_SET_BIT(csr, value) ((csr) = ((csr) & 0x00FF) | (value))
#define CAM_CSR_CLEAR_BIT(csr, value) ((csr) = ((csr) & 0x00FF) & ~(value))


/*
 ******************************************************************************
 * National BMAC (DP83261) registers:
 ******************************************************************************
 */

/*
 * MODE register bits:
 */
#define BMAC_MR_RUN	0x01
#define BMAC_MR_CBP	0x02
#define BMAC_MR_MRP	0x04
#define BMAC_MR_PIP	0x08
#define BMAC_MR_ILB	0x40
#define BMAC_MR_DIAG	0x80

/*
 * OPTION register bits:
 */
#define BMAC_OPTION_ESA		0x01
#define BMAC_OPTION_ELA		0x02
#define BMAC_OPTION_ITR		0x04
#define BMAC_OPTION_IRR		0x08
#define BMAC_OPTION_IRPT	0x10
#define BMAC_OPTION_IFCS	0x20
#define BMAC_OPTION_EMIND	0x40
#define BMAC_OPTION_ITC		0x80

/*
 * FUNCTION register bits:
 */
#define BMAC_FUNCTION_MARST	0x01
#define BMAC_FUNCTION_MCRST	0x04
#define BMAC_FUNCTION_BCN	0x08
#define BMAC_FUNCTION_CLM	0x10

/*
 * CSR0 (Current Receiver Status Register) bits:
 */
#define BMAC_CRS0_RTS_MASK	0x07
#define BMAC_CRS0_RS_MASK	0x70
#define BMAC_CRS0_RFLAG		0x80

/*
 * BMAC_CRS0_RTS (Receive Timing State) values:
 */
#define BMAC_CRS0_RTS_AWAIT_SD		0x00
#define BMAC_CRS0_RTS_CHECK_FC		0x01
#define BMAC_CRS0_RTS_CHECK_SA		0x02
#define BMAC_CRS0_RTS_CHECK_DA		0x03
#define BMAC_CRS0_RTS_CHECK_INFO	0x04
#define BMAC_CRS0_RTS_CHECK_MAC		0x05

/*
 * BMAC_CSR0_RS (Receive State) values:
 */
#define BMAC_CRS0_RS_LISTEN		0x00
#define BMAC_CRS0_RS_AWAIT_SD		0x10
#define BMAC_CRS0_RS_RC_FR_CTRL		0x20
#define BMAC_CRS0_RS_RC_FR_BODY		0x30
#define BMAC_CRS0_RS_RC_FR_AC_STATUS	0x40
#define BMAC_CRS0_RS_CHECK_TOKEN	0x50
#define BMAC_CRS0_RS_RC_FR_OI_STATUS	0x60

/*
 * CTS0 (Current Transmitter Status Register) bits:
 */
#define BMAC_CTS0_TTS_MASK		0x0f
#define BMAC_CTS0_TS_MASK		0x70
#define BMAC_CTS0_ROP			0x80

/*
 * BMAC_CTS0_TTS values:
 */
#define	BMAC_CTS0_TTS_IDLE		0x00
#define BMAC_CTS0_TTS_TX_PREAMBLE	0x01
#define BMAC_CTS0_TTS_DATA_WAIT		0x02
#define BMAC_CTS0_TTS_TX_SD_FC		0x03
#define BMAC_CTS0_TTS_TX_DA		0x04
#define BMAC_CTS0_TTS_TX_SA		0x05
#define BMAC_CTS0_TTS_TX_INFO		0x06
#define BMAC_CTS0_TTS_TX_FCS		0x07
#define BMAC_CTS0_TTS_TX_ED_FS		0x08

/*
 * TR_STATE values:
 */
#define BMAC_CTS0_TS_IDLE		0x00
#define BMAC_CTS0_TS_REPEAT		0x10
#define BMAC_CTS0_TS_DATA		0x20
#define BMAC_CTS0_TS_ISSUE_TOKEN	0x30
#define BMAC_CTS0_TS_CLAIM		0x40
#define BMAC_CTS0_TS_BEACON		0x50
#define BMAC_CTS0_TS_VOID		0x70

/*
 * RELR0 (Ring Event Latch Register) and
 */
#define BMAC_RELR0_ROP			0x01
#define BMAC_RELR0_RNOP			0x02
#define BMAC_RELR0_BCNR			0x04
#define BMAC_RELR0_CLMR			0x08
#define BMAC_RELR0_OTRMAC		0x10
#define BMAC_RELR0_PINV			0x20
#define BMAC_RELR0_DUPADD		0x40

/*
 * REMR0 (Ring Event Mask Register) bits:
 */
#define BMAC_REMR0_ROP			0x01
#define BMAC_REMR0_RNOP			0x02
#define BMAC_REMR0_BCNR			0x04
#define BMAC_REMR0_CLMR			0x08
#define BMAC_REMR0_OTRMAC		0x10
#define BMAC_REMR0_PINV			0x20
#define BMAC_REMR0_DUPADD		0x40


/*
 * RELR1 (Ring Event Latch Register 1) and
 */
#define BMAC_RELR1_OTRBCN		0x01
#define BMAC_RELR1_MYBCN		0x02
#define BMAC_RELR1_MYCLM		0x20
#define BMAC_RELR1_HICLM		0x40
#define BMAC_RELR1_LOCLM		0x80

/*
 * REMR1 (Ring Event Mask Register 1) bits:
 */
#define BMAC_REMR1_OTRBCN		0x01
#define BMAC_REMR1_MYBCN		0x02
#define BMAC_REMR1_MYCLM		0x20
#define BMAC_REMR1_HICLM		0x40
#define BMAC_REMR1_LOCLM		0x80

/*
 * TELR0 (Token and Timer Event Latch Register 0) and
 */
#define BMAC_TELR0_ENTRMD		0x01
#define BMAC_TELR0_TVXEXP		0x02
#define BMAC_TELR0_TRTEXP		0x04
#define BMAC_TELR0_DUPTKR		0x08
#define BMAC_TELR0_CBERR		0x10
#define BMAC_TELR0_TKCAPT		0x20
#define BMAC_TELR0_TKPASS		0x40
#define BMAC_TELR0_RLVLD		0x80

/*
 * TEMR0 (Token and Timer Event Mask Register 0) bits:
 */
#define BMAC_TEMR0_ENTRMD		0x01
#define BMAC_TEMR0_TVXEXP		0x02
#define BMAC_TEMR0_TRTEXP		0x04
#define BMAC_TEMR0_DUPTKR		0x08
#define BMAC_TEMR0_CBERR		0x10
#define BMAC_TEMR0_TKCAPT		0x20
#define BMAC_TEMR0_TKPASS		0x40
#define BMAC_TEMR0_RLVLD		0x80

/*
 * CILR (Counter Increment Latch Register),
 */
#define BMAC_CILR_FRRCV			0x01
#define BMAC_CILR_FREI			0x02
#define BMAC_CILR_FRLST			0x04
#define BMAC_CILR_FRCOP			0x08
#define BMAC_CILR_FRNCOP		0x10
#define BMAC_CILR_FRTRX			0x20
#define BMAC_CILR_TKRCVD		0x40

/*
 * CIMR (Counter Increment Mask Register), 
 */
#define BMAC_CIMR_FRRCV			0x01
#define BMAC_CIMR_FREI			0x02
#define BMAC_CIMR_FRLST			0x04
#define BMAC_CIMR_FRCOP			0x08
#define BMAC_CIMR_FRNCOP		0x10
#define BMAC_CIMR_FRTRX			0x20
#define BMAC_CIMR_TKRCVD		0x40

/*
 * COLR (Counter Overflow Latch Register), and
 */
#define BMAC_COLR_FRRCV			0x01
#define BMAC_COLR_FREI			0x02
#define BMAC_COLR_FRLST			0x04
#define BMAC_COLR_FRCOP			0x08
#define BMAC_COLR_FRNCOP		0x10
#define BMAC_COLR_FRTRX			0x20
#define BMAC_COLR_TKRCVD		0x40

/*
 * COMR (Counter Overflow Mask Register) bits:
 */
#define BMAC_COMR_FRRCV			0x01
#define BMAC_COMR_FREI			0x02
#define BMAC_COMR_FRLST			0x04
#define BMAC_COMR_FRCOP			0x08
#define BMAC_COMR_FRNCOP		0x10
#define BMAC_COMR_FRTRX			0x20
#define BMAC_COMR_TKRCVD		0x40

/*
 * IELR (Internal Event Latch Register) bits:
 */
#define BMAC_IELR_MPE			0x01
#define BMAC_IELR_RSMERR		0x04
#define BMAC_IELR_TSMERR		0x08

/*
 * ESR (Exception Status Register), and
 */
#define BMAC_ESR_PPE			0x01
#define BMAC_ESR_MPE			0x02 /* Reserved on page 2-176, CJS */
#define BMAC_ESR_CPE			0x20
#define BMAC_ESR_CCE			0x40
#define BMAC_ESR_CWI			0x80 

/*
 * EMR (Exception Mask Register) bits:
 */
#define BMAC_EMR_PPE			0x01
#define BMAC_EMR_MPE			0x02 /* Reserved on page 2-176, CJS */
#define BMAC_EMR_CPE			0x20
#define BMAC_EMR_CCE			0x40
#define BMAC_EMR_CWI			0x80 

/*
 * ICR (Interrupt Condition Register), and
 */
#define BMAC_ICR_RNG			0x01
#define BMAC_ICR_TTE			0x02
#define BMAC_ICR_CIE			0x04
#define BMAC_ICR_COE			0x08
#define BMAC_ICR_IERR			0x40
#define BMAC_ICR_ESE			0x80

/*
 * IMR (Interrupt Mask Register) bits:
 */
#define BMAC_IMR_RNG			0x01
#define BMAC_IMR_TTE			0x02
#define BMAC_IMR_CIE			0x04
#define BMAC_IMR_COE			0x08
#define BMAC_IMR_IERR			0x40
#define BMAC_IMR_ESE			0x80

/*
 * Request service classes:
 */
#define NO_CLASS	0
#define APRI1		1
#define APRI2	 	2
#define APRI3	 	3
#define SYN	   	4
#define IMM	   	5
#define IMMN	  	6
#define IMMR  		7
#define ASYN  		8
#define RBEG		9
#define REND		10
#define RCNT		11
#define ASYND		12
#define RBEGD		13
#define RENDD		14
#define RCNTD		15

typedef struct bmac {		/* 256 registers  */
    /* Operation registers */
   vchar pad_bmac_0;
   vchar mr;			/* Mode Register */
   vchar pad_bmac_1;
   vchar option;		/* Option Register */
   vchar pad_bmac_2;
   vchar function;		/* Function Register */
   vchar pad_bmac_3[4];
   vchar res0[4];
   vchar pad_bmac_4;
   vchar rev;			/* Revision Register */
   /* Event registers */
   vchar pad_bmac_5;
   vchar cmp;			/* Compare Register */
   vchar pad_bmac_6[3];
   vchar res1[3];
   vchar pad_bmac_7;
   vchar crs0;			/* Current Receiver Status Register */
   vchar pad_bmac_8;
   vchar res2;
   vchar pad_bmac_9;
   vchar cts0;			/* Current Transmitter Status Register */
   vchar pad_bmac_10;
   vchar res3;
   vchar pad_bmac_11;
   vchar relr0;			/* Ring Event Latch Register 0 */
   vchar pad_bmac_12;
   vchar remr0;			/* Ring Event Mask Register 0 */
   vchar pad_bmac_13;
   vchar relr1;			/* Ring Event Latch Register 1 */
   vchar pad_bmac_14;
   vchar remr1;			/* Ring Event Mask Register 1 */
   vchar pad_bmac_15;
   vchar telr0;			/* Token & Timer Event Latch Register */
   vchar pad_bmac_16;
   vchar temr0;			/* Token & Timer Event Mask Register */
   vchar pad_bmac_17[2];
   vchar res4[2];
   vchar pad_bmac_18;
   vchar cilr;			/* Counter Increment Latch Register */
   vchar pad_bmac_19;
   vchar cimr;			/* Counter Increment Mask Register */
   vchar pad_bmac_20[2];
   vchar res5[2];
   vchar pad_bmac_21;
   vchar colr;			/* Counter Overflow Latch Register */
   vchar pad_bmac_22;
   vchar comr;			/* Counter Overflow Mask Register */
   vchar pad_bmac_23[10];
   vchar res6[10];
   vchar pad_bmac_24;
   vchar ielr;			/* Internal Event Latch Register */
   vchar pad_bmac_25[3];
   vchar res7[3];
   vchar pad_bmac_26;
   vchar esr;			/* Exception Status Register */
   vchar pad_bmac_27;
   vchar emr;			/* Exception Mask Register */
   vchar pad_bmac_28;
   vchar icr;			/* Interrupt Condition Register */
   vchar pad_bmac_29;
   vchar imr;			/* Interrupt Mask Register */
   vchar pad_bmac_30[16];
   vchar res8[16];
   /* MAC Parameter RAM */
   vchar pad_bmac_31;
   vchar mla0;			/* My Long Address: bits 47-40 */
   vchar pad_bmac_32;
   vchar mla1;			/* bits 39-32 */
   vchar pad_bmac_33;
   vchar mla2;			/* bits 31-24 */
   vchar pad_bmac_34;
   vchar mla3;			/* bits 23-16 */
   vchar pad_bmac_35;
   vchar mla4;			/* bits 15-8 */
   vchar pad_bmac_36;
   vchar mla5;			/* bits 7-0 */
   vchar pad_bmac_37;
   vchar msa0;			/* My Short Address: bits 15-8 */
   vchar pad_bmac_38;
   vchar msa1;			/* bits 7-0 */
   vchar pad_bmac_39;
   vchar gla0;			/* Group Long Address: bitts 47-40 */
   vchar pad_bmac_40;
   vchar gla1;			/* bits 39-32 */
   vchar pad_bmac_41;
   vchar gla2;			/* bits 31-24 */
   vchar pad_bmac_42;
   vchar gla3;			/* bits 23-16 */
   vchar pad_bmac_43;
   vchar gla4;			/* bits 15-8 */
   vchar pad_bmac_44;
   vchar res9;
   vchar pad_bmac_45;
   vchar gsa0;			/* Group Short Address: bits 15-8 */
   vchar pad_bmac_46;
   vchar res10;
   vchar pad_bmac_47;
   vchar treq0;			/* Requested Target Rotation Timer: bits 31-24 */
   vchar pad_bmac_48;
   vchar treq1;			/* bits 23-16 */
   vchar pad_bmac_49;
   vchar treq2;			/* bits 15-8 */
   vchar pad_bmac_50;
   vchar treq3;			/* bits 7-0 */
   vchar pad_bmac_51;
   vchar tbt0;			/* Transmit Beacon Type: bits 31-24 */
   vchar pad_bmac_52;
   vchar tbt1;			/* bits 23-16 */
   vchar pad_bmac_53;
   vchar tbt2;			/* bits 15-8 */
   vchar pad_bmac_54;
   vchar tbt3;			/* bits 7-0 */
   vchar pad_bmac_55;
   vchar fgm0;			/* Fixed Group Address Map */
   vchar pad_bmac_56;
   vchar fgm1;
   vchar pad_bmac_57[6];
   vchar res11[6];
   vchar pad_bmac_58;
   vchar gam0;			/* Programmable Group Address Map */
   vchar pad_bmac_59;		/* (is pgm10-pgm1F in databook) */
   vchar gam1;
   vchar pad_bmac_60;
   vchar gam2;
   vchar pad_bmac_61;
   vchar gam3;
   vchar pad_bmac_62;
   vchar gam4;
   vchar pad_bmac_63;
   vchar gam5;
   vchar pad_bmac_64;
   vchar gam6;
   vchar pad_bmac_65;
   vchar gam7;
   vchar pad_bmac_66;
   vchar gam8;
   vchar pad_bmac_67;
   vchar gam9;
   vchar pad_bmac_68;
   vchar gama;
   vchar pad_bmac_69;
   vchar gamb;
   vchar pad_bmac_70;
   vchar gamc;
   vchar pad_bmac_71;
   vchar gamd;
   vchar pad_bmac_72;
   vchar game;
   vchar pad_bmac_73;
   vchar gamf;
   vchar pad_bmac_74[16];	/* pgm0-pgmf is copied from pgm10-pgm1f */
   vchar res12[16];
   /* MAC Counters and Timer Thresholds */
   vchar pad_bmac_75[7];
   vchar res13[7];
   vchar pad_bmac_76;
   vchar thsh1;			/* Async Pri Threshold 1 */
   vchar pad_bmac_77[3];
   vchar res14[3];
   vchar pad_bmac_78;
   vchar thsh2;			/* Async Pri Threshold 2 */
   vchar pad_bmac_79[3];
   vchar res15[3];
   vchar pad_bmac_80;
   vchar thsh3;			/* Async Pri Threshold 3 */
   vchar pad_bmac_81[3];
   vchar res16[3];
   vchar pad_bmac_82;
   vchar tmax;			/* Max Token Rotation Timer */
   vchar pad_bmac_83[3];
   vchar res17[3];
   vchar pad_bmac_84;
   vchar tvx;			/* Valid Transmission Timer */
   vchar pad_bmac_85;
   vchar tneg0;			/* Negotiated Target Rotation Timer */
   vchar pad_bmac_86;
   vchar tneg1;
   vchar pad_bmac_87;
   vchar tneg2;
   vchar pad_bmac_88;
   vchar tneg3;
   vchar pad_bmac_89[3];
   vchar res18[3];
   vchar pad_bmac_90;
   vchar ltct;			/* Late Count Counter */
   vchar pad_bmac_91;
   vchar res19;
   vchar pad_bmac_92;
   vchar frct1;			/* Frame Received Counter */
   vchar pad_bmac_93;
   vchar frct2;
   vchar pad_bmac_94;
   vchar frct3;
   vchar pad_bmac_95;
   vchar res20;
   vchar pad_bmac_96;
   vchar eict1;			/* Error Isolated Counter */
   vchar pad_bmac_97;
   vchar eict2;
   vchar pad_bmac_98;
   vchar eict3;
   vchar pad_bmac_99;
   vchar res21;
   vchar pad_bmac_100;
   vchar lfct1;			/* Lost Frame Counter */
   vchar pad_bmac_101;
   vchar lfct2;
   vchar pad_bmac_102;
   vchar lfct3;
   vchar pad_bmac_103;
   vchar res22;
   vchar pad_bmac_104;
   vchar fcct1;			/* Frame Copied Counter */
   vchar pad_bmac_105;
   vchar fcct2;
   vchar pad_bmac_106;
   vchar fcct3;
   vchar pad_bmac_107;
   vchar res23;
   vchar pad_bmac_108;
   vchar fnct1;			/* Frame Not Copied Counter */
   vchar pad_bmac_109;
   vchar fnct2;
   vchar pad_bmac_110;
   vchar fnct3;
   vchar pad_bmac_111;
   vchar res24;
   vchar pad_bmac_112;
   vchar ftct1;			/* Frame Transmitted Counter */
   vchar pad_bmac_113;
   vchar ftct2;
   vchar pad_bmac_114;
   vchar ftct3;
   vchar pad_bmac_115;
   vchar res25;
   vchar pad_bmac_116;
   vchar tkct1;			/* Token Received Counter */
   vchar pad_bmac_117;
   vchar tkct2;
   vchar pad_bmac_118;
   vchar tkct3;
   vchar pad_bmac_119;
   vchar res26;
   vchar pad_bmac_120;
   vchar rlct1;			/* Ring Latency Counter */
   vchar pad_bmac_121;
   vchar rlct2;
   vchar pad_bmac_122;
   vchar rlct3;
} bmac_t;

/*
 * FDDI NIM register definition:
 */

typedef struct bsi_interface_module_reg bsi_interface_module_reg_t;
typedef struct bsi_interface_module_reg xx_fddi_regs_t;
struct bsi_interface_module_reg {

    volatile ushort id_prom[2];	/* 0 is ID, 1 is hw rev */
    ushort pad1[254];		/* pad out to slot addr+$200 */
    volatile ushort csr0;	/* reset control and status info */
    volatile ushort csr1;	/* interrupt vector byte */
    volatile ushort lcr;
    ushort pad2[253];		/* pad out to slot addr+$400*/

    /*
     * FDDI Registers go here:
     */
    player_t	player0;	/* Player 0, 32 registers */
    ushort      pad3[32];
    player_t	player1;	/* Player 1, 32 registers */
    ushort      pad4[32];
    bsi_t	bsi;		/* BSI, 32 registers */
    ushort      pad5[32];

    /*
     * CAM Registers
     */
    volatile ushort csr;
    volatile ushort dcr;
    volatile ushort ddr;
    volatile ushort lsar;
    volatile ushort msar;
    volatile ushort usar;
    volatile ushort ccf;
    volatile ushort cdf;
    volatile ushort srd;
    volatile ushort crd;
    volatile ushort pad6[54];

    bmac_t	bmac;		/* BMAC, 256 registers */
};

/*
 * Bits in NIM csr0 register:
 */
#define CSR0_LEDS_MASK    0xFF02  /* LEDs mask                            */
#define CSR0_TREE_A_LED   0x8000  /* Tree_A Synopt 3809 LED (0=On) (WO)   */
#define CSR0_ACTV_A_LED   0x4000  /* Active_A Synopt 3809 LED (0=On) (WO) */
#define CSR0_PART_B_LED   0x2000  /* Partit_B Synopt 3809 LED (0=On) (WO) */
#define CSR0_TREE_B_LED   0x1000  /* Tree_B Synopt 3809 LED (0=On) (WO)   */
#define CSR0_ACTV_B_LED   0x0800  /* Active_B Synopt 3809 LED (0=On) (WO) */
#define CSR0_THRU_LED     0x0400  /* Thru Synopt 3809 LED (0=On) (WO)     */
#define CSR0_WRAP_B_LED   0x0200  /* Wrap_B Synopt 3809 LED (0=On) (WO)   */
#define CSR0_WRAP_A_LED   0x0100  /* Wrap_A Synopt 3809 LED (0=On) (WO)   */
#define CSR0_CAM_IRQ        0x80  /* CAM requests an interrupt (RO)       */
#define CSR0_BSI_IRQ        0x40  /* BSI requests an interrupt (RO)       */
#define CSR0_BMAC_IRQ       0x20  /* MBAC requests an interrupt (RO)      */
#define CSR0_PLAYER1_IRQ    0x10  /* Player1 requests an interrupt (RO)   */
#define CSR0_PLAYER0_IRQ    0x08  /* Player0 requests an interrupt (RO)   */
#define CSR0_OPTICAL_BYPASS 0x04  /* Optical bypass switch present (RO)   */
#define CSR0_DAS            0x02  /* Dual Attach (DAS) board present (RO) */
#define CSR0_PART_A_LED     0x02  /* Partit_A Synopt 3809 LED (0=On) (WO) */
#define CSR0_NIM_RESET      0x01  /* 1 to reset, 0 to clear reset (RW)    */

#define BSI_IOMEMORY_MIN    0x00400000    /* Minimum IO memory in bytes */
#define BSI_BUFFER_PAGES        256  /* FDDI buffer pages                   */
#define BSI_BUFFER_PAGE_SIZE    8192 /* National BSI w/mapper uses 8 KB pgs */

/*
 * Re-enable psp empty interrupts when the buffer cache reaches 
 * this threshold
 */
#define BSI_BUFFER_THRESHOLD	14

/*
 * Provide some padding when allocating Q blocks from IO memory
 * to protect the system from the BSI writing past the end of queues.
 */
#define BSI_QPAD		128

/*
 * The size of the actual BSI data area. This is 8Kbytes minus the size of the
 * overhead of the free memory pool system
 */
#define BSI_BUFFER_SIZE         (BSI_BUFFER_PAGE_SIZE - MEMORY_BLOCK_OVERHEAD)

/*
 * The size of the default FDDI encapsulation is a SNAP encapsulation, which !
 * is 21 bytes, plus 3 bytes of padding to take it to the longword aligned
 * value of 24
 */
#define BSI_PADDED_ENCAPBYTES   (FDDI_SNAP_ENCAPBYTES + 3)

#define FDDI_QUEUE_LIMIT_OFFSET	((uint)(BSI_QSIZE - 24)) /* End of queue    */
#define FDDI_PAGE_COLUMNS       8    /* Print 8 columns of page ptrs        */


typedef struct transmit_data txd_t;
struct transmit_data {
    odud_descr_t	odud1;	/* The 1st odud descriptor for this packet */
    odud_descr_t	odud2;	/* The 2nd odud descriptor for this packet */
    odud_descr_t	odud3;	/* The 3rd odud descriptor for this packet */
};



/* ## #define REQUEST_LIMIT	64
#define BSI_REQ_NDX_SIZE 63 ## */
#define REQUEST_LIMIT	128
#define BSI_REQ_NDX_SIZE 127

typedef struct shrd_ram shrd_ram_t;
struct shrd_ram {
    /*
     * Mailbox shared between 68K and BSI:
     */
    volatile charlong mbox;

    /*
     * ODUDs, packet pointer, etc:
     */
    txd_t	oduds[REQUEST_LIMIT];
};



typedef struct requests req_t;
struct requests {
    req_descrQ_t	*base;		/* Base address of request queue */
    req_descr_t		*ptr;		/* Ptr to first tx desc in use */
    req_descr_t		*limit;		/* Ptr to first free tx desc */
    uint		ptr_index;	/* Index into pointer RAM for this Q */
    uint		lmt_index;	/* Index into limit RAM for this Q */
    char		*name;		/* Name of this queue */

    short		tx_reqs;	/* Number of outstanding TX requests */
    short		my_ndx;		/* Index for next tx request */
};

typedef struct confirmations cnf_t;
struct confirmations {
    cnf_descrQ_t	*base;	/* Base address of confirmation queue */
    cnf_descr_t		*ptr;	/* Ptr to next expected confirmation */
    cnf_descr_t		*limit;	/* Last conf. descriptor BSI can write */
    uint	ptr_index;	/* Index into pointer RAM for this queue */
    uint	lmt_index;	/* Index into limit RAM for this queue */
    char	*name;		/* Name of this queue */
};

typedef struct request_channel req_chan_t;
struct request_channel {
    req_t	req;		/* Request queue */
    cnf_t	cnf;		/* Confirmation queue */
    int		queue_number;	/* 0=>ic0, 1=>ic1, 2=>ic2 */

    bsi_ring_t  req_ring;
    bsi_ring_t  cnf_ring;
    bsi_ring_t  odud_ring;
    bsi_ring_t  pak_ring;
};



typedef struct input_data_units idud_t;
struct input_data_units {
    idud_descrQ_t	*base;           /* Base address of idud queue */
    idud_descr_t	*ptr;            /* Ptr to next expected idud */
    idud_descr_t	*limit;          /* Last idud desc. BSI can write */
    uint		ptr_index;       /* Index into pointer RAM for queue */
    uint		lmt_index;       /* Index into limit RAM for queue */
    int			max_iduds;       /* Max iduds to process per call */
    uint		pad1;
    paktype		*pak;            /* Save partial packet if no LAST */
    char		*name;           /* Name of this queue */
};
#define FDDI_IDUD0_MAX_IDUDS		8
/* ## #define FDDI_IDUD1_MAX_IDUDS		32 */
#define FDDI_IDUD1_MAX_IDUDS		64
#define FDDI_IDUD2_MAX_IDUDS		8
#define FDDI_IDUD2_BRI_MAX_IDUDS	32

typedef struct pool_space_pointers psp_t;
struct pool_space_pointers {
    psp_descrQ_t	*base;	/* Base address of psp queue */
    psp_descr_t		*ptr;	/* Ptr to first valid psp ptr we wrote */
    psp_descr_t		*limit;	/* Ptr to first free psp descr */
    uint         ptr_index;	/* Index into pointer RAM for this queue */
    uint         lmt_index;	/* Index into limit RAM for this queue */
    int          quantum;	/* Max Number of pages to add each time */
    int          max_psps;	/* Max number of total pages to put in queue */
    int          count;		/* How many pages are in this queue? */
    char         *name;		/* Name of this queue */
};

#define FDDI_PSPQ0_QUANTUM	10
/* ## #define FDDI_PSPQ1_QUANTUM	34 ## */
#define FDDI_PSPQ1_QUANTUM	68
#define FDDI_PSPQ2_QUANTUM	10
#define FDDI_PSPQ2_BRI_QUANTUM  34

#define FDDI_PSPQ0_MAX_PSPS	20
/* ## #define FDDI_PSPQ1_MAX_PSPS	68 ## */
#define FDDI_PSPQ1_MAX_PSPS	136
#define FDDI_PSPQ2_MAX_PSPS	20
#define FDDI_PSPQ2_BRI_MAX_PSPS	68

typedef struct indicate_channel ind_chan_t;
struct indicate_channel {
    idud_t	idud;		/* Input data unit queue */
    psp_t	psp;		/* Pool space pointer queue */
    int		queue_number;	/* 0=>ic0, 1=>ic1, 2=>ic2 */
    paktype   *bsi_page_last;	/* Last allocated page to this channel */

    bsi_ring_t  idud_ring;
    bsi_ring_t  psp_ring;
};


typedef struct bsi_instance bsi_instance_t;
typedef struct bsi_instance bmac_instance_t;
typedef struct bsi_instance player_instance_t;
struct bsi_instance {

    /*
     *************************
     *  General information  *
     *************************
     */
    bsi_interface_module_reg_t *module_regs; /* Ptr to NIM address space */
    shrd_ram_t *ioram;			/* Pointer to shrd_ram structure */
    hwidbtype *idb;			/* Hardware IDB */
    idbtype   *swidb;			/* The first software IDB */
    ulong delay_10_usec;		/* Calibrated delay for 10 usec */
    boolean board_reset;                /* status flag used in bsi_init 
					   with side effect of being 
					   checked in init_smt */

    /*
     *********************
     *  CAM information  *
     *********************
     */
    boolean cam_learning;		/* Do we need the CAM to learn? */
    boolean cam_present;		/* Whether we have a useable CAM */
    int     cam_state;		        /* The current CAM state */
    boolean cam_all_multicasts;         /* Multicast promiscuous mode */

    /*
     **********************
     *  BMAC information  *
     **********************
     */
    boolean bsi_bmac_reset;             /* set to true to init bmac later */
    ushort  bmac_address[3];            /* current bmac addr set into hw */
    boolean beacontype;                 /* true if we be beaconing */
    uchar   curBeaconType;		/* current beacon frame type (if any) */
    paktype *bcn_ptr;			/* Beacon frame pointer */
    ushort  origtmax;                   /* tmax in use before beaconing */

    /*
     ************************
     *  PLAYER information  *
     ************************
     */
    /* bonus phy layer info for the very curious CE and user */
    ulong   cmt_lsupi[2];                  /* line state unknown phy invalid */
    ulong   cmt_lsupv[2];                  /* line state unknown phy valid */
    ulong   cmt_ebou[2];                   /* elasticitybuffer over/underflow*/
    ushort  previousLS[2];		   /* previous line state received */
    uchar   cur_rcmra_mask[2];		   /* current RCMRA mask */
    uchar   cur_rcmrb_mask[2];		   /* current RCMRB mask */
    uchar   last_ls_rcmra_mask[2];	   /* RCMRA mask for last detected LS */
    uchar   last_ls_rcmrb_mask[2];	   /* RCMRB mask for last detected LS */
    ushort   LEMremaining[2];		   /* Auxiliar 32 bits LEM counter */

    /*
     *********************
     *  BSI information  *
     *********************
     */
    boolean bsi_ok;		/* true if bsi packet interface ok */
    charlong bsi_rev;		/* Revision code of BSI silicon */
    ushort buf_needed;		/* Number of buffers needed to fill all Ind Q */

    /*
     * Indicate queues (RX):
     */
    ind_chan_t ic0;		/* Indicate Channel 0 (RX) */
    ind_chan_t ic1;		/* Indicate Channel 1 (RX) */
    ind_chan_t ic2;		/* Indicate Channel 2 (RX) */

    /* FOR RX DEBUGGING *CJS* */
    ulong llc;			/* # of LLC frames received on chan 1       */
    ulong not_llc;		/* # of non LLC frames received on chan 1   */
    ulong bridge;		/* # of frames considered for bridging      */
    ulong mac;			/* # of MAC frames received                 */
    ulong smt;			/* # of SMT frames received                 */
    ulong chained_iduds;	/* # of chained IDUDs received              */
    ulong switched;		/* # of packets fast switched               */
    ulong to_process;		/* # of packets passed up to process level  */

    /* FOR TX DEBUGGING *CJS* */
    ulong tx_full;		/* count times oduds full while switching   */
    ulong tx_1odud;		/* # of only ODUD packets TXd               */
    ulong tx_2odud;		/* # of 2 ODUD packets TXd                  */
    ulong tx_3odud;		/* # of 3 ODUD packets TXd                  */

    /*
     * TX structures:
     */
    req_chan_t BEAC_CHAN;	/* Request channel for TX beacons           */
    req_chan_t REQ_CHAN;	/* Request Channel for TX                   */
    ulong tx_restarts;		/* Number of times that we reset the fddi   */
				/*  interface because of a hung transmitter */

    /*
     * Error counters:
     */

    /* RX error counters */
    ulong no_page;		/* Couldn't get a page for the psp Q        */
    ulong full_pspQ_err;	/* Got a psp low space interrupt w/full Q   */
    ulong max_pspQ_err;		/* PSP Q topped with max value		    */
    ulong empty_pspQ0;		/* Got an empty pspQ interrupt on IC0       */
    ulong empty_pspQ1;		/* Got an empty pspQ interrupt on IC1       */
    ulong empty_pspQ2;		/* Got an empty pspQ interrupt on IC2       */
    ulong psp_reuse_err;	/* BSI reused an FDDI page error!           */
    ulong psp_unknown_use_err;  /* BSI used an unknown FDDI page!           */
    ulong bsi_page_reversed;	/* PSP page usage reversed (avoid reuse er) */
    ulong full_idudQ0;		/* Got a full idudQ interrupt on IC0        */
    ulong full_idudQ1;		/* Got a full idudQ interrupt on IC1        */
    ulong full_idudQ2;		/* Got a full idudQ interrupt on IC2        */
    ulong invalid_data_length;	/* Invalid data length error in IDUD	    */
    ulong internal_error;	/* Internal error reported in IDUD	    */
    ulong no_first;		/* First IDUD didn't have BSI_FIRST	    */
    ulong no_middle_last;	/* Got FIRST when expecting MIDDLE or LAST  */
    ulong waited_valid_idud;	/* Waited for and got a valid idud for LAST */
    ulong no_valid_idud;	/* Didn't get a LAST in a multipage frame   */
    ulong stacked_saved;	/* Got another saved idud while one was     */
				/*  being processed                         */
    ulong tc_other;		/* Got termination condition OTHER          */
    ulong tc_format_err;	/* Got termination condition FORMAT_ERR     */
    ulong tc_frame_stripped;	/* Got termination condition FRAME_STRIPPED */
    ulong bad_frame;		/* IS in IDUD was Bad Frame                 */
    ulong iar_exci0;		/* Indicate exception errors		    */
    ulong iar_exci1;
    ulong iar_exci2;
    ulong throttled;		/* Number of times we've been throttled */

    /* TX error counters */
    ulong rs_none;		/* Request status 'none' errors		  */
    ulong rs_preempted;		/* Preempted status			  */
    ulong rs_part_done;		/* Part done status			  */
    ulong rs_service_loss;	/* Service loss errors			  */
    ulong rs_beacon_or_claim;	/* Got beacon or won claim status	  */
    ulong rs_bad_confirm;	/* Bad confirmation errors		  */
    ulong rs_host_abort;	/* Host abort errors			  */
    ulong rs_bad_ringop;	/* Bad ringop errors			  */
    ulong rs_bmac_abort;	/* BMAC aborted this packet		  */
    ulong rs_trt_timeout;	/* Token rotation timer timeout errors	  */
    ulong rs_bmac_reset;	/* BMAC was reset status		  */
    ulong rs_consistency_fail;	/* REQ or ODUD stream inconsistency error */
    ulong rs_fatal_bsi_error;	/* BSI reported fatal error		  */
    ulong rar_excr0;		/* Request Exceptions reported by BSI	  */
    ulong rar_excr1;
    ulong rar_usrr0;
    ulong rar_usrr1;
    ulong bad_tx_pak;		/* Bad pak from pak_ptrs[tx_index]         */
};


/*
 * Prototypes
 */
void cam_interrupt(hwidbtype *idb);
void bsi_init_req_channel(bsi_instance_t *ds, req_chan_t *req);
void bsi_interrupt(uchar mar, hwidbtype *idb);
boolean bsi_initialize(hwidbtype *idb);
void bsi_repeat(hwidbtype *idb, paktype *pak);

extern boolean init_bsi_dev(hwidbtype *);
extern void bsi_check_version(int slot);
extern void fddi_idb_init_les(hwidbtype *);

#endif __C4000_BSI_H__
