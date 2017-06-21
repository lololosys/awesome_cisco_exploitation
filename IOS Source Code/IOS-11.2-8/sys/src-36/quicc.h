/* $Id: quicc.h,v 3.2.62.1 1996/06/12 19:56:40 jng Exp $
 * $Source: /release/112/cvs/Xsys/src-36/quicc.h,v $
 *------------------------------------------------------------------
 * quicc.h - structs and defines for the MC68360 QUICC chip
 *
 * November 1994, Greg Stovall
 *
 * Copyright (c) 1994 - 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: quicc.h,v $
 * Revision 3.2.62.1  1996/06/12  19:56:40  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.2  1995/11/17  18:39:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:51:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __QUICC_H__
#define __QUICC_H__

/*
 *	HERE BE DRAGONS.
 * 
 * The top part of this file contains the C language structures.
 * They are all bounded by the ASMINCLUDE derivative.  If you
 * add entries to this file please be carefull.  There are various
 * naming conventions documented throughout the file please try
 * to add entries using the conventions.
 */

#ifndef ASMINCLUDE
/*
 * The convention for all parameter ram structures is as follows:
 *
 *	quicc_XXX_param where:
 *		XXX - section of ram the structure represents.
 * All structures have a name and a type. The type is the name
 * with an _t suffix.  When naming the members of the structure
 * please use the name as given in the QUICC user manual.
 */
/*
 * QUICC Parameter RAM.
 */
typedef struct quicc_scc_param {
    ushort	rbase;			/* Receive BD Base Address */
    ushort	tbase;			/* Transmit BD Base Address */
    uchar	rfcr;			/* Receive Function Code */
    uchar	tfcr;			/* Transmit Function Code */
    ushort	mrblr;			/* Receive Buffer Length */
    ulong	rstate;			/* Receive Internal State */
    ulong	rptr;			/* Receive Internal Data Pointer */
    ushort	rbptr;			/* Receive BD Pointer */
    ushort	rcount;			/* Receive Internal Byte Count */
    ulong	rtemp;			/* Receive Temp */
    ulong	tstate;			/* Transmit Internal State */
    ulong	tptr;			/* Transmit Internal Data Pointer */
    ushort	tbptr;			/* Transmit BD Pointer */
    ushort	tcount;			/* Transmit Byte Count */
    ulong	ttemp;			/* Transmit Temp */
    ulong	rcrc;			/* Temp Receive CRC */
    ulong	tcrc;			/* Temp Transmit CRC */
} quicc_scc_param_t;

typedef struct quicc_hdlc_param {
    quicc_scc_param_t scc_param;	/* Common to all protocols */
    uchar	hdlc_res_0[0x4];	/* Reserved */
    ulong	c_mask;			/* CRC Constant */
    ulong	c_pres;			/* CRC Preset */
    ushort	disfc;			/* Discarded Frame Counter */
    ushort	crcec;			/* CRC Error Counter */
    ushort	abtsc;			/* Abort Sequence Counter */
    ushort	nmarc;			/* Nonmatching Address Rx Counter */
    ushort	retrc;			/* Frame Retransmission Counter */
    ushort	mflr;			/* Maximum Frame Length Register */
    ushort 	max_cnt;		/* Maximum Length Counter */
    ushort	rfthr;			/* Received Frames Threshold */
    ushort	rfcnt;			/* Received Frames Count */
    ushort	hmask;			/* User Defined Frame Address Mask */
    ushort	haddr1;			/* User Defined Frame Address 1 */
    ushort	haddr2;			/* User Defined Frame Address 2 */
    ushort	haddr3;			/* User Defined Frame Address 3 */
    ushort	haddr4;			/* User Defined Frame Address 4 */
    ushort	tmp;			/* Temp Storage */
    ushort 	tmp_mb;			/* Temp Storage */
} quicc_hdlc_param_t;

typedef struct quicc_uart_param {
    quicc_scc_param_t scc_param;	/* Common to all protocols */
    uchar	uart_res_0[0x8];	/* Reserved */
    ushort	max_idl;		/* Maximum Idle Characters */
    ushort	idlc;			/* Temporary Idle Counter */
    ushort	brkcr;			/* Break Count Register (Transmit) */
    ushort	parec;			/* Receive Parity Error Counter */
    ushort	frmer;			/* Receive Framing Error Counter */
    ushort	nosec;			/* Receive Noise Counter */
    ushort	brkec;			/* Receive Break Character Counter */
    ushort	brkln;			/* Last Received Break Length */
    ushort	uaddr1;			/* UART Address Character 1 */
    ushort	uaddr2;			/* UART Address Character 2 */
    ushort	rtemp;			/* Temp Storage */
    volatile ushort	toseq;		/* Transmit Out-of-Sequence Character */
    ushort	cc[0x8];		/* Receive Control Characters */
    ushort	rccm;			/* Receive Control Character Mask */
    ushort	rccr;			/* Receive Control Character Register */
    ushort	rlbc;			/* Receive Last Break Character */
} quicc_uart_param_t;

typedef struct quicc_ether_param {
    quicc_scc_param_t scc_param;	/* Common to all protocols */
    ulong	c_pres;			/* Preset CRC */
    ulong	c_mask;			/* Constant Mask for CRC */
    ulong	crcec;			/* CRC Error Counter */
    ulong	alec;			/* Alighnment Error Counter */
    ulong	disfc;			/* Discard Frame Counter */
    ushort	pads;			/* Short Frame PAD Characters */
    ushort	ret_lim;		/* Retry Limit Threshold */
    ushort	ret_cnt;		/* Retry Limit Counter */
    ushort	mflr;			/* Maximum Frame Length Register */
    ushort	minflr;			/* Minimum Frame Length Register */
    ushort	maxd1;			/* Maximum DMA1 Length Register */
    ushort	maxd2;			/* Maximum DMA2 Length Register */
    ushort	maxd;			/* Rx Max DMA */
    ushort	dma_cnt;		/* Rx DMA Counter */
    ushort	max_b;			/* Max BD Byte Count */
    ushort	gaddr1;			/* Group Address Filter 1 */
    ushort	gaddr2;			/* Group Address Filter 2 */
    ushort	gaddr3;			/* Group Address Filter 3 */
    ushort	gaddr4;			/* Group Address Filter 4 */
    ulong	tbuf0_data0;		/* Save Area 0 - Current Frame */
    ulong	tbuf0_data1;		/* Save Area 1 - Current Frame */
    ulong	tbuf0_rba0;
    ulong	tbuf0_crc;
    ushort	tbuf0_bcnt;
    ushort	paddr_h;		/* Physical Address (MSB) */
    ushort	paddr_m;		/* Physical Address */
    ushort	paddr_l;		/* Physical Address (LSB) */
    ushort	p_per;			/* Persistence */
    ushort	rfbd_ptr;		/* Rx First BD Pointer */
    ushort	tfbd_ptr;		/* Tx First BD Pointer */
    ushort	tlbd_ptr;		/* Tx Last BD Pointer */
    ulong	tbuf1_data0;		/* Save Area 0 - Next Frame */
    ulong	tbuf1_data1;		/* Save Area 1 - Next Frame */
    ulong	tbuf1_rba0;
    ulong	tbuf1_crc;
    ushort	tbuf1_bcnt;
    ushort	tx_len;			/* Tx Frame Length Counter */
    ushort	iaddr1;			/* Individual Address Filter 1 */
    ushort	iaddr2;			/* Individual Address Filter 2 */
    ushort	iaddr3;			/* Individual Address Filter 3 */
    ushort	iaddr4;			/* Individual Address Filter 4*/
    ushort	boff_cnt;		/* Backoff Counter */
    ushort	taddr_h;		/* Temp Address (MSB) */
    ushort	taddr_m;		/* Temp Address */
    ushort	taddr_l;		/* Temp Address (LSB) */
} quicc_ether_param_t;

typedef struct quicc_smc_uart_param {
    ushort	rbase;			/* Rx BD Base Address */
    ushort	tbase;			/* Tx BD Base Address */
    uchar	rfcr;			/* Rx Function Code */
    uchar	tfcr;			/* Tx Function Code */
    ushort	mrblr;			/* Rx Buffer Length */
    ulong	rstate;			/* Rx Internal State */
    ulong	rptr;			/* Rx Internal Data Pointer */
    ushort	rbptr;			/* Rb BD Pointer */
    ushort	rcount;			/* Rx Internal Byte Count */
    ulong	rtemp;			/* Rx Temp */
    ulong	tstate;			/* Tx Internal State */
    ulong	tptr;			/* Tx Internal Data Pointer */
    ushort	tbptr;			/* Tx BD Pointer */
    ushort	tcount;			/* Tx Byte Count */
    ulong	ttemp;			/* Tx Temp */
    ushort	max_idl;		/* Maximum Idle Characters */
    ushort	idlc;			/* Temporary Idle Counter */
    ushort	brkln;			/* Last Rx Break Length */
    ushort	brkec;			/* Rx Break Condition Counter */
    ushort	brkcr;			/* Break Count Register (Tx) */
    ushort	r_mask;			/* Temporary Bit Mask */
} quicc_smc_uart_param_t;

typedef struct quicc_smc_trans_param {
    ushort	rbase;			/* Rx BD Base Address */
    ushort	tbase;			/* Tx BD Base Address */
    uchar	rfcr;			/* Rx Function Code */
    uchar	tfcr;			/* Tx Function Code */
    ushort	mrblr;			/* Rx Buffer Length */
    ulong	rstate;			/* Rx Internal State */
    ulong	rptr;			/* Rx Internal Data Pointer */
    ushort	rbptr;			/* Rb BD Pointer */
    ushort	rcount;			/* Rx Internal Byte Count */
    ulong	rtemp;			/* Rx Temp */
    ulong	tstate;			/* Tx Internal State */
    ulong	tptr;			/* Tx Internal Data Pointer */
    ushort	tbptr;			/* Tx BD Pointer */
    ushort	tcount;			/* Tx Byte Count */
    ulong	ttemp;			/* Tx Temp */
    ushort      smc_trans_res_0[0x5];   /* no transparent-spec param */
} quicc_smc_trans_param_t;

typedef struct quicc_smc_gci_param {
    ushort      m_rxbd;                 /* Monitor Channel Rx BD */
    ushort      m_txbd;                 /* Monitor Channel Tx BD */
    ushort      cl_rxbd;                /* C/I Channel Rx BD */
    ushort      cl_txbd;                /* C/I Channel Tx BD */
    ulong       rstate;                 /* Rx & Tx Internal State */
    ushort      m_rxd;                  /* Monitor Rx Data */
    ushort      m_txd;                  /* Monitor Tx Data */
    ushort      cl_rxd;                 /* C/I Rx Data */
    ushort      cl_txd;                 /* C/I Tx Data */
} quicc_smc_gci_param_t;

typedef struct quicc_misc_param {
    ushort      rev_num;                /* Microcode Revision Number */
    uchar	misc_res_0[0xA];	/* Reserved */
    uchar       misc_pad[0x4];		/* Misc Param Overall Size = 0x10 */
} quicc_misc_param_t;

typedef struct quicc_spi_param {
    ushort	rbase;			/* Rx BD Base Address */
    ushort	tbase;			/* Tx BD Base Address */
    uchar	rfcr;			/* Rx Function Code */
    uchar	tfcr;			/* Tx Function Code */
    ushort	mrblr;			/* Rx Buffer Length */
    ulong	rstate;			/* Rx Internal State */
    ulong	rptr;			/* Rx Internal Data Pointer */
    ushort	rbptr;			/* Rx BD Pointer */
    ushort	rcount;			/* Rx Internal Byte Count */
    ulong	rtemp;			/* Rx Temp */
    ulong	tstate;			/* Tx Internal State */
    ulong	tptr;			/* Tx Internal Data Pointer */
    ushort	tbptr;			/* Tx BD Pointer */
    ushort	tcount;			/* Tx Byte Count */
    ulong	ttemp;			/* Tx Temp */
    uchar       pad[8];                 /* spi param overall size = 0x30 */
} quicc_spi_param_t;

typedef struct quicc_timer_param {
    ushort	tm_base;		/* RISC Timer Table Base Address */
    ushort	tm_ptr;			/* RISC Timer Table Pointer */
    ushort	r_tmr;			/* RISC Timer Mode Register */
    ushort	r_tmv;			/* RISC Timer Valid Register */
    ulong	tm_cmd;			/* RISC Timer Command Register */
    ulong	tm_cnt;			/* RISC Timer Internal Count */
} quicc_timer_param_t;

typedef struct quicc_idma_param {
    ushort      ibase;                  /* IDMA BD Base Address */
    ushort      ibptr;                  /* IDMA BD Pointer */
    ulong       istate;                 /* IDMA Internal State */
    ulong       itemp;                  /* IDMA Temp */
    uchar       idma_pad[0x4];		/* IDMA Overall Size = 0x10 */
} quicc_idma_param_t;

/*
 * There are 4 pages of parameter RAM that define the CPM sub-modules.
 * See the MC68360 manual Table 3-2 CMP Sub-Module Base Addresses for
 * a description of each page.  Artwork supplied by Yvette Yuen.
 *
 *	|-------------------------------------------------------------------|
 *	| SCC1           | SCC2           | SCC3           | SCC4           |
 *	|________________|________________|________________|________________|
 * 0x00 | ( 0x30 bytes ) | ( 0x30 bytes ) | ( 0x30 bytes ) | ( 0x30 bytes ) |
 *      | Scc parameter  | Scc parameter  | Scc parameter  | Scc parameter  |
 *	| common to all  | common to all  | common to all  | common to all  |
 *	| protocol       | protocol       | protocol       | protocol       |
 *	|________________|________________|________________|________________|
 * 0x30 | ( 0x80 bytes ) | ( 0x50 bytes ) | ( 0x40 bytes ) |( 0x40 bytes )  |
 *	| Protocol       | Protocol       | Protocol       | Protocol       |
 *	| Specific       | Specific       | Specific       | Specific       |
 *	| Parameter area | Parameter area | Parameter area | Parameter area |
 *	| . . . . . . . .|. . . . . . . . |________________|________________|
 * 0x70 |                |                | ( 0x10 bytes ) | ( 0x10 bytes ) |
 *	|                |                | IDMA1 Parameter| IDMA2 Parameter|
 *	| . . . . . . . .|________________|________________|________________|
 * 0x80 |                | ( 0x30 bytes ) | ( 0x28 bytes ) | ( 0x28 bytes ) |
 *	|                | SPI Parameter  | SMC1 Parameter | SMC2 Parameter |
 *	| . . . . . . . .| . . . . . . . .|________________|________________|
 * 0xA8 |                |                | ( 0x18 bytes ) | ( 0x18 bytes ) |
 *	|                |                | Protocol       | Protocol       |
 *	|________________|________________| Specific       | Specific       |
 * 0xB0 | ( 0x10 bytes ) | ( 0x10 bytes ) | Parameter area | Parameter area |
 *	| MISC parameter | Timer parameter|                |                |
 *	|________________|________________|________________|________________|
 * 0xC0 | ( 0x40 bytes ) | ( 0x40 bytes ) | ( 0x40 bytes ) | ( 0x40 bytes ) |
 *	| reserved       | reserved       |  reserved      |  reserved      |
 *	|________________|________________|________________|________________|
 */

typedef union quicc_scc1_param {
    quicc_ether_param_t ether;
    quicc_hdlc_param_t  hdlc;
    quicc_uart_param_t  uart;
    uchar pad[0xB0];
} quicc_scc1_param_t;

typedef union quicc_scc2_param {
    quicc_hdlc_param_t  hdlc;
    quicc_uart_param_t  uart;
    uchar pad[0x80];
} quicc_scc2_param_t;

typedef union quicc_scc34_param {
    quicc_hdlc_param_t  hdlc;
    quicc_uart_param_t  uart;
    uchar pad[0x70];
} quicc_scc34_param_t;

typedef union quicc_smc_param {
    quicc_smc_uart_param_t  uart;
    quicc_smc_trans_param_t trans;
    quicc_smc_gci_param_t   gci;
    uchar pad[0x40];
} quicc_smc_param_t;

/*
 * This section of code describes all the internal registers.
 * The format is taken from the QUICC Users Manual and is as
 * follows:
 *
 *	Block_name;	description
 *
 * Thus the SIM moduale Configuration Register (mcr) would be
 * sim_mcr (see below).  Please do not violate this namespace.
 */
typedef struct quicc_internal_regs {
    /*
     * SIM - System Integration Module
     */
    volatile ulong	sim_mcr;	/* Module Configuration Register */
    volatile uchar	sim_res_0[0x4];	/* Reserved */
    volatile uchar	sim_avr;	/* Autovector Register */
    volatile uchar	sim_rsr;	/* Reset Status Register */
    volatile uchar	sim_res_1[0x2];	/* Reserved */
    volatile uchar	sim_clkocr;	/* CLKO Control Register */
    volatile uchar	sim_res_2[0x3];	/* Reserved */
    volatile ushort	sim_pllcr;	/* PLL Control Register */
    volatile uchar	sim_res_3[0x2];	/* Reserved */
    volatile ushort	sim_cdvcr;	/* Clock Divider Control Register */
    volatile ushort	sim_pepar;	/* Port E Pin Assignment Register */
    volatile uchar	sim_res_4[0xA];	/* Reserved */
    volatile uchar	sim_sypcr;	/* System Protection Control */
    volatile uchar	sim_swiv;	/* Software Interrupt Vector */
    volatile uchar	sim_res_5[0x2];	/* Reserved */
    volatile ushort	sim_picr;	/* Periodic Interrupt Control Register */
    volatile uchar	sim_res_6[0x2];	/* Reserved */
    volatile ushort	sim_pitr;	/* Periodic Interrupt Timing Register */
    volatile uchar	sim_res_7[0x3];	/* Reserved */
    volatile uchar	sim_swsr;	/* Software Service Register */
    volatile ulong	sim_bkar;	/* Breakpoint Address Register */
    volatile ulong	sim_bkcr;	/* Breakpoint Control Register */
    volatile uchar	sim_res_8[0x8];	/* Reserved */
    /*
     * MEMC - Memory Controller
     */
    volatile ulong	memc_gmr;	/* Global Memory Register */
    volatile ushort	memc_mstat;	/* Memory Controller Status Register */
    volatile uchar	memc_res_0[0xA]; /* Reserved */
    volatile ulong	memc_br0;	/* Base Register 0 */
    volatile ulong	memc_or0;	/* Option Register 0 */
    volatile uchar	memc_res_1[0x8]; /* Reserved */
    volatile ulong	memc_br1;	/* Base Register 1 */
    volatile ulong	memc_or1;	/* Option Register 1 */
    volatile uchar	memc_res_2[0x8]; /* Reserved */
    volatile ulong	memc_br2;	/* Base Register 2 */
    volatile ulong	memc_or2;	/* Option Register 2 */
    volatile uchar	memc_res_3[0x8]; /* Reserved */
    volatile ulong	memc_br3;	/* Base Register 3 */
    volatile ulong	memc_or3;	/* Option Register 3 */
    volatile uchar	memc_res_4[0x8]; /* Reserved */
    volatile ulong	memc_br4;	/* Base Register 4 */
    volatile ulong	memc_or4;	/* Option Register 4 */
    volatile uchar	memc_res_5[0x8]; /* Reserved */
    volatile ulong	memc_br5;	/* Base Register 5 */
    volatile ulong	memc_or5;	/* Option Register 5 */
    volatile uchar	memc_res_6[0x8]; /* Reserved */
    volatile ulong	memc_br6;	/* Base Register 6 */
    volatile ulong	memc_or6;	/* Option Register 6 */
    volatile uchar	memc_res_7[0x8]; /* Reserved */
    volatile ulong	memc_br7;	/* Base Register 7 */
    volatile ulong	memc_or7;	/* Option Register 7 */
    volatile uchar	memc_res_8[0x438]; /* Reserved */
    /*
     * IDMA1 - Independent Direct Memory Access
     */
    volatile ushort	idma1_iccr;	/* Channel Configuration Register */
    volatile uchar	idma1_res_0[0x2]; /* Reserved */
    volatile ushort	idma1_cmr;	/* IDMA1 Mode Register */
    volatile uchar	idma1_res_1[0x2]; /* Reserved */
    volatile ulong	idma1_sapr;	/* IDMA1 Source Address Pointer */
    volatile ulong	idma1_dapr;	/* IDMA1 Destination Address Pointer*/
    volatile ulong	idma1_bcr;	/* IDMA1 Byte Count Register */
    volatile uchar	idma1_fcr;	/* IDMA1 Function Code Register */
    volatile uchar	idma1_res_2[0x1]; /* Reserved */
    volatile uchar	idma1_cmar;	/* Channel Mask Register */
    volatile uchar	idma1_res_3[0x1]; /* Reserved */
    volatile uchar	idma1_csr;	/* IDMA1 Channel Status Register */
    volatile uchar	idma1_res_4[0x3]; /* Reserved */
    /*
     * SDMA - Serial Direct Memory Access
     */
    volatile uchar	sdma_sdsr;	/* Status Register */
    volatile uchar	sdma_res_0[0x1]; /* Reserved */
    volatile ushort	sdma_sdcr;	/* Configuration Register */
    volatile ulong	sdma_sdar;	/* Address Register */
    /*
     * IDMA2 - Independent Direct Memory Access
     */
    volatile uchar	idma2_res_0[0x2]; /* Reserved */
    volatile ushort	idma2_cmr;	/* IDMA2 Mode Register */
    volatile ulong	idma2_sapr;	/* IDMA2 Source Address Pointer */
    volatile ulong	idma2_dapr;	/* IDMA2 Destination Address Pointer */
    volatile ulong	idma2_bcr;	/* IDMA2 Byte Count Register */
    volatile uchar	idma2_fcr;	/* IDMA2 Function Code Register */
    volatile uchar	idma2_res_1[0x1]; /* Reserved */
    volatile uchar	idma2_cmar;	/* Channel Mask Register */
    volatile uchar	idma2_res_2[0x1]; /* Reserved */
    volatile uchar	idma2_csr;	/* IDMA2 Channel Status Register */
    volatile uchar	idma2_res_3[0x7]; /* Reserved */
    /*
     * CPIC - Communications Processor Interrupt Controller
     */
    volatile ulong	cpic_cicr;	/* CP Interrupt Configuration Register */
    volatile ulong	cpic_cipr;	/* CP Interrupt Pending Register */
    volatile ulong	cpic_cimr;	/* CP Interrupt Mask Register */
    volatile ulong	cpic_cisr;	/* CP Interrupt In-Service Register */
    /*
     * Parallel I/O
     */
    volatile ushort	pio_padir;	/* Port A Data Direction Register */
    volatile ushort	pio_papar;	/* Port A Pin Assignment Register */
    volatile ushort	pio_paodr;	/* Port A Open Drain Register */
    volatile ushort	pio_padat;	/* Port A Data Register */
    volatile uchar	pio_res_0[0x8];	/* Reserved */
    volatile ushort	pio_pcdir;	/* Port C Data Direction Register */
    volatile ushort	pio_pcpar;	/* Port C Pin Assignment Register */
    volatile ushort	pio_pcso;	/* Port C Special Options */
    volatile ushort	pio_pcdat;	/* Port C Data Register */
    volatile ushort	pio_pcint;	/* Port C Interrupt Control Register */
    volatile uchar	pio_res_1[0x16]; /* Reserved */
    /*
     * TIMER
     */
    volatile ushort	tmr_tgcr;	/* Timer Global Configuration Register */
    volatile uchar	tmr_res_0[0xE];	/* Reserved */
    volatile ushort	tmr_tmr1;	/* Timer 1 Mode Register */
    volatile ushort	tmr_tmr2;	/* Timer 2 Mode Register */
    volatile ushort	tmr_trr1;	/* Timer 1 Reference Register */
    volatile ushort	tmr_trr2;	/* Timer 2 Reference Register */
    volatile ushort	tmr_tcr1;	/* Timer 1 Capture Register */
    volatile ushort	tmr_tcr2;	/* Timer 2 Capture Register */
    volatile ushort	tmr_tcn1;	/* Timer 1 Counter Register */
    volatile ushort	tmr_tcn2;	/* Timer 2 Counter Register */
    volatile ushort	tmr_tmr3;	/* Timer 3 Mode Register */
    volatile ushort	tmr_tmr4;	/* Timer 4 Mode Register */
    volatile ushort	tmr_trr3;	/* Timer 3 Reference Register */
    volatile ushort	tmr_trr4;	/* Timer 4 Reference Register */
    volatile ushort	tmr_tcr3;	/* Timer 3 Capture Register */
    volatile ushort	tmr_tcr4;	/* Timer 4 Capture Register */
    volatile ushort	tmr_tcn3;	/* Timer 3 Counter Register */
    volatile ushort	tmr_tcn4;	/* Timer 4 Counter Register */
    volatile ushort	tmr_ter1;	/* Timer 1 Event Register */
    volatile ushort	tmr_ter2;	/* Timer 2 Event Register */
    volatile ushort	tmr_ter3;	/* Timer 3 Event Register */
    volatile ushort	tmr_ter4;	/* Timer 4 Event Register */
    volatile uchar	tmr_res_1[0x8];	/* Reserved */
    /*
     * CP - Communications Processor
     */
    volatile ushort	cp_cr;		/* Command Register */
    volatile uchar	cp_res_0[0x2];	/* Reserved */
    volatile ushort	cp_rccr;	/* Main Configuration Register */
    volatile uchar	cp_res_1[0x10];	/* Reserved */
    volatile ushort	cp_rter;	/* RISC Timers Event Register */
    volatile uchar	cp_res_2[0x2];	/* Reserved */
    volatile ushort	cp_rtmr;	/* RISC Timers Mask Register */
    volatile uchar	cp_res_3[0x14];	/* Reserved */
    /*
     * BRG - Baud Rate Generator
     */
    volatile ulong	brg_brgc1;	/* BRG1 Configuration Register */
    volatile ulong	brg_brgc2;	/* BRG2 Configuration Register */
    volatile ulong	brg_brgc3;	/* BRG3 Configuration Register */
    volatile ulong	brg_brgc4;	/* BRG4 Configuration Register */
    /*
     * SCC - Serial Communications Controller
     */
    struct scc_regs {
	volatile ulong	scc_gsmrl;	/* SCC General Mode Register */
	volatile ulong	scc_gsmrh;	/* SCC General Mode Register */
	volatile ushort	scc_psmr;	/* Protocol Specific Mode Register */
	volatile uchar	scc_res_0[0x2];	/* Reserved */
	volatile ushort	scc_todr;	/* SCC Transmit on Demand */
	volatile ushort	scc_dsr;	/* SCC Data Sync Register */
	volatile ushort	scc_scce;	/* SCC Event Register */
	volatile uchar	scc_res_1[0x2];	/* Reserved */
	volatile ushort	scc_sccm;	/* SCC Mask Register */
	volatile uchar	scc_res_2[0x1];	/* Reserved */
	volatile uchar	scc_sccs;	/* SCC Status Register */
	volatile uchar	scc_res_3[0x8];	/* Reserved */
    } scc[0x4];
    /*
     * SMC - Management Controller
     */
    struct smc_regs {
	volatile uchar	smc_res_0[0x2];	/* Reserved */
	volatile ushort	smc_smcmr;	/* SMC Mode Register */
	volatile uchar	smc_res_1[0x2];	/* Reserved */
	volatile uchar	smc_smce;	/* SMC Event Register */
	volatile uchar	smc_res_2[0x3];	/* Reserved */
	volatile uchar	smc_smcm;	/* SMC Mask Register */
	volatile uchar	smc_res_3[0x5];	/* Reserved */
    } smc_regs[0x2];
    /*
     * SPI - Serial Peripheral Interface
     */
    volatile ushort	spi_spmode;	/* SPI Mode Register */
    volatile uchar	spi_res_0[0x4];	/* Reserved */
    volatile uchar	spi_spie;	/* SPI Event Register */
    volatile uchar	spi_res_1[0x3];	/* Reserved */
    volatile uchar	spi_spim;	/* SPI Mask Register */
    volatile uchar	spi_res_2[0x2];	/* Reserved */
    volatile uchar	spi_spcom;	/* SPI Command Register */
    volatile uchar	spi_res_3[0x4];	/* Reserved */
    /*
     * PIP - Parallel Interface Port
     */
    volatile ushort	pip_pipc;	/* PIP Configuration Register */
    volatile uchar	pip_res_0[0x2];	/* Reserved */
    volatile ushort	pip_ptpr;	/* PIP Timing Parameters Register */
    volatile ulong	pip_pbdir;	/* Port B Data Direction Register */
    volatile ulong	pip_pbpar;	/* Port B Pin Assignment Register */
    volatile uchar	pip_res_1[0x2];	/* Reserved */
    volatile ushort	pip_pbodr;	/* Port B Open Drain Register */
    volatile ulong	pip_pbdat;	/* Port B Data Register */
    volatile uchar	pip_res_2[0x18]; /* Reserved */
    /*
     * SI - Serial Interface
     */
    volatile ulong	si_simode;	/* SI Mode Register */
    volatile uchar	si_sigmr;	/* SI Global Mode Register */
    volatile uchar	si_res_0[0x1];	/* Reserved */
    volatile uchar	si_sistr;	/* SI Status Register */
    volatile uchar	si_sicmr;	/* SI Command Register */
    volatile uchar	si_res_1[0x4];	/* Reserved */
    volatile ulong	si_sicr;	/* SI Clock Routing */
    volatile uchar	si_res_3[0x2];	/* Reserved */
    volatile ushort	si_sirpt;	/* SI RAM Pointer Transmit */
    volatile ushort	si_sirpr;	/* SI RAM Pointer Receive */
    volatile uchar	si_res_4[0xA];	/* Reserved */
    volatile ushort	si_siram[0x80];	/* SI Routing RAM */
} quicc_internal_regs_t;

/*
 * This structure defines everything in the QUICC parameter
 * ram.
 */
typedef struct quicc_dpr {
    uchar    user_data_0[0x400];        /* User Data/BDs/Micro Prog Area */
    uchar    user_data_1[0x200];        /* User Data/BDs Area */
    uchar    user_data_2[0x100];        /* User Data/BDs/ Micro Scratch */
    uchar    dp_ram_res_0[0x500];       /* Reserved */
    /*
     * Parameter RAM Page 1
     */
    quicc_scc1_param_t	scc1;		/* SCC1 Base */
    quicc_misc_param_t  misc;		/* Misc Base */
    uchar               dpr_res_1[0x40];
    /*
     * Parameter RAM Page 2
     */
    quicc_scc2_param_t  scc2;		/* SCC2 Base */
    quicc_spi_param_t   spi;		/* SPI Base */
    quicc_timer_param_t timer;		/* Timer Base */
    uchar               dpr_res_2[0x40];
    /*
     * Parameter RAM Page 3
     */
    quicc_scc34_param_t scc3;		/* SCC3 Base */
    quicc_idma_param_t  idma1;		/* IDMA1 Base */
    quicc_smc_param_t   smc1;		/* SMC1 Base */
    uchar               dpr_res_3[0x40];
    /*
     * Parameter RAM Page 4
     */
    quicc_scc34_param_t scc4;		/* SCC4 Base */
    quicc_idma_param_t  idma2;		/* IDMA2 Base */
    quicc_smc_param_t   smc2;		/* SMC2 Base */
    uchar               dpr_res_4[0x40];
    /*
     * Internal Registers
     */
    quicc_internal_regs_t regs;		/* Internal Register Map */
} quicc_dpr_t;

/*
 * quicc buffer descriptors.  These buffer descriptors are used
 * for all interfaces.
 */
typedef struct quicc_bd {
    volatile ushort status;		/* Status and Control */
    volatile ushort length;		/* Length of Data in buffer */
    volatile uchar *buf_ptr;		/* Pointer to Data buffer */
} quicc_bd_t;

/*
 * End of ifndef ASMINCLUDE - No more C structures after this point.
 */
#endif  /* ASMINCLUDE */

/*
 * Buffer Descriptor Flags.  If a flag is specific to a
 * protocol then the mode where that flag is defined is
 * included in parenthesis.
 */
#define QUICC_BDSTAT_RX_EMPTY	0x8000		/* Empty */
#define QUICC_BDSTAT_RX_WRAP	0x2000		/* Wrap */
#define QUICC_BDSTAT_RX_INT	0x1000		/* Interrupt */
#define QUICC_BDSTAT_RX_LAST	0x0800		/* Last */
#define QUICC_BDSTAT_RX_CNTRL	0x0800		/* Control Char UART only */
#define QUICC_BDSTAT_RX_ADDR	0x0400		/* Address UART only */
#define QUICC_BDSTAT_RX_FIRST	0x0400		/* First */
#define QUICC_BDSTAT_RX_CONTIN	0x0200		/* Continuious Mode */
#define QUICC_BDSTAT_RX_MISS	0x0100		/* Miss (Ethernet) */
#define QUICC_BDSTAT_RX_AM	0x0100		/* Address Match UART only */
#define QUICC_BDSTAT_RX_DERR	0x0080		/* DPLL Error (HDLC) */
#define QUICC_BDSTAT_RX_FLV	0x0020		/* Frame Length Violation */
#define QUICC_BDSTAT_RX_BREAK	0x0020		/* Break detect UART only */
#define QUICC_BDSTAT_RX_NOFA	0x0010		/* Non-octet frame alignment */
#define QUICC_BDSTAT_RX_FR      0x0010          /* Framing error UART only */
#define QUICC_BDSTAT_RX_ABORT	0x0008		/* Abort Sequence (HDLC) */
#define QUICC_BDSTAT_RX_SHORT	0x0008		/* Short Frame (Ethernet) */
#define QUICC_BDSTAT_RX_PR      0x0008          /* Parity error UART only */
#define QUICC_BDSTAT_RX_CRCERR	0x0004	 	/* CRC Error */
#define QUICC_BDSTAT_RX_OVRRUN	0x0002		/* Overrun */
#define QUICC_BDSTAT_RX_CDLOST	0x0001		/* Carrier Detect Lost (HDLC) */
#define QUICC_BDSTAT_RX_COLLIS	0x0001		/* Collision (Ethernet) */
#define QUICC_BDSTAT_RX_MMERR	0x0001		/* Multi-Master Error (SPI) */

/*
 * Transmit Descriptor Flags.
 */
#define QUICC_BDSTAT_TX_RDY	0x8000		/* Ready */
#define QUICC_BDSTAT_TX_PAD	0x4000		/* Short Frame Padding (Ethernet) */
#define QUICC_BDSTAT_TX_WRAP	0x2000		/* Wrap */
#define QUICC_BDSTAT_TX_INT	0x1000		/* Interrupt */
#define QUICC_BDSTAT_TX_LAST	0x0800		/* Last */
#define QUICC_BDSTAT_TX_TRCRC	0x0400		/* Transmit CRC */
#define QUICC_BDSTAT_TX_CONTIN	0x0200		/* Cont. Mode UART only  */
#define QUICC_BDSTAT_TX_DEFIND	0x0200		/* Defer Indication (Ethernet) */
#define QUICC_BDSTAT_TX_HBEAT	0x0100		/* Heart Beat (Ethernet) */
#define QUICC_BDSTAT_TX_PREM	0x0100		/* Premable (UART only) */
#define QUICC_BDSTAT_TX_LCOL	0x0080		/* Late Collision (Ethernet) */
#define QUICC_BDSTAT_TX_NS	0x0080		/* No Stop Bit.  UART only */
#define QUICC_BDSTAT_TX_RLIM	0x0040		/* Retransmission Limit (Ethernet) */
#define QUICC_BDSTAT_TX_UNRRUN	0x0002		/* Underrun */
#define QUICC_BDSTAT_TX_CTSLOS	0x0001		/* CTS Lost */
#define QUICC_BDSTAT_TX_CSLOS	0x0001		/* Carrier Sence Lost (Ethernet) */
#define QUICC_BDSTAT_TX_MMERR	0x0001		/* Multi-Master Error (SPI) */

/*
 * Function Codes (FC3-FC0)
 */
#define QUICC_FC_USER_DATA_SPACE  0x01
#define QUICC_FC_USER_PROG_SPACE  0x02
#define QUICC_FC_SUP_DATA_SPACE   0x05
#define QUICC_FC_SUP_PROG_SPACE   0x06
#define QUICC_FC_SUP_CPU_SPACE    0x07
#define QUICC_FC_DMA_SPACE        0x08

/*
 * SCC Parameter Ram - SCC Function code register (RFCR, TECR)
 */
#define QUICC_FCR_MOT             0x10          /* 0:swapped 1:normal operation */
#define QUICC_FCR_FUNC_CODE(x)    (x)           /* function code */

/*
 * This section defines all the QUICC registers.  The format
 * for all registers is:
 *
 *	QUICC_XXX_YYY where:
 *		XXX is the register name
 *		YYY is the field in the register
 *
 * To assist others please do not violate the above convention.
 * Please note that the naming convention is not a replacement
 * for an appropriate comment.
 */

/*
 * spi_spmode - SPI Mode register (SPMODE)
 */
#define QUICC_SPMR_LOOP  	0x4000		/* Loop Mode */
#define QUICC_SPMR_CLKINV	0x2000		/* Clock Invert */
#define QUICC_SPMR_CLKPH	0x1000		/* Clock Phase */
#define QUICC_SPMR_DIV16	0x0800		/* Divide by 16 */
#define QUICC_SPMR_REVDAT	0x0400		/* Reverse Data */
#define QUICC_SPMR_MASTER	0x0200		/* Master Mode */
#define QUICC_SPMR_ENASPI	0x0100		/* Enable SPI */
#define QUICC_SPMR_LEN(x)	((x) << 4)	/* Character Length */

/*
 * spi_spie - SPI Event Register (SPIE)
 */
#define QUICC_SPIE_MMERR	0x0020		/* Multi-Master Error */
#define QUICC_SPIE_TXERR	0x0010		/* Tx Error */
#define QUICC_SPIE_BSY		0x0004		/* Busy Condition */
#define QUICC_SPIE_TXBUF	0x0002		/* Tx Buffer */
#define QUICC_SPIE_RXBUF	0x0001		/* Rx Buffer */

/*
 * spi_spim - SPI Mask Register (SPIM)
 */
#define QUICC_SPIM_MMERR	0x0020		/* Multi-Master Error */
#define QUICC_SPIM_TXERR	0x0010		/* Tx Error */
#define QUICC_SPIM_BSY		0x0004		/* Busy Condition */
#define QUICC_SPIM_TXBUF	0x0002		/* Tx Buffer */
#define QUICC_SPIM_RXBUF	0x0001		/* Rx Buffer */

/*
 * spi_spcom - SPI Command Register (SPCOM)
 */
#define QUICC_SPCOM_START	0x0080		/* Start Transmit */

/*
 * SPI Maximum Buffer Size.
 */
#define QUICC_SPI_MTU		0x0004		/* Largest SPI Command */
#define QUICC_SPI_8BITS		0x0007		/* size 8 bit */
#define QUICC_SPI_SPIN_MAX	0x4000		/* Amount of time to spin */

/*
 * scc_gsmr[lh] - General SCC Mode Register (GSMR)
 * This register is 64 bits long.  It has been split into 2 32 bit
 * registers.  scc_gsmrl contains the first (low-order) 32 bits and
 * scc_gsmrh contains the last (high-order) 32 bits.  It is left as
 * an exercise to the reader to figure out the ordering.
 */
#define QUICC_SCC_GSMRH_GDEN	0x00010000	/* Glitch Detect Enable */
#define QUICC_SCC_GSMRH_TRANRX	0x1000		/* Transparent Receiver */
#define QUICC_SCC_GSMRH_TRANTX	0x0800		/* Transparent Transmitter */
#define QUICC_SCC_GSMRH_CDP	0x0400		/* Carrier Detect Pulse */
#define QUICC_SCC_GSMRH_CTSP	0x0200		/* Clear to Send Pulse */
#define QUICC_SCC_GSMRH_CDSAMP	0x0100		/* Carrier Detect Sampling */
#define QUICC_SCC_GSMRH_CTSSAMP	0x0080		/* Clear to Send Sampling */
#define QUICC_SCC_GSMRH_TFLEN	0x0040		/* Tx FIFO Length */
#define QUICC_SCC_GSMRH_RFLEN	0x0020		/* Rx FIFO Length */
#define QUICC_SCC_GSMRH_TXSYNC	0x0010		/* Tx Sync to Receiver */
#define QUICC_SCC_GSMRH_RTSMD	0x0002		/* Ready to Send Mode */
#define QUICC_SCC_GSMRH_RSYN	0x0001		/* Rx Sync Timing */

#define QUICC_SCC_GSMRL_TCINV	0x10000000	/* Tx Clock Invert */
#define QUICC_SCC_GSMRL_RINV	0x02000000	/* DPLL Rx Input Invert */
#define QUICC_SCC_GSMRL_TINV	0x01000000	/* DPLL Tx Input Invert */
#define QUICC_SCC_GSMRL_TPL(x)	((x) << 21)	/* Tx Preamble Length */
#define QUICC_SCC_GSMRL_TPP(x)	((x) << 19)	/* Tx Preamble Pattern */
#define QUICC_SCC_GSMRL_TEND	0x00040000	/* Tx Frame Ending */
#define QUICC_SCC_GSMRL_TDCR(x)	((x) << 16)	/* Tx Divide Clock Rate */
#define QUICC_SCC_GSMRL_RDCR(x)	((x) << 14)	/* Rx Divide Clock Rate */
#define QUICC_SCC_GSMRL_RENC(x)	((x) << 11)	/* Rx Encoding Method */
#define QUICC_SCC_GSMRL_TENC(x)	((x) << 8)	/* Tx Encoding Method */
#define QUICC_SCC_GSMRL_DIAG(x)	((x) << 6)	/* Diagnostic Mode */
#define QUICC_SCC_GSMRL_ENR	0x0020		/* Enable Receiver */
#define QUICC_SCC_GSMRL_ENT	0x0010		/* Enable Transmitter */
#define QUICC_SCC_GSMRL_MODE(x)	(x)		/* Channel Protocol Mode */

/*
 * QUICC_SCC_GSMRH_RTSM Parameters.
 */
#define QUICC_HDLC_IDLE_ONES	0x0000		/* Send ones to idle between frames */ 
#define QUICC_HDLC_IDLE_FLAGS	0x0002		/* Send flags to idle between frames */

/*
 * QUICC_SCC_GSMRL_TPL Parameters
 */
#define QUICC_TX_PREAM_0BITS    0x0
#define QUICC_TX_PREAM_48BITS   0x4             /* for ethernet operation */

/*
 * QUICC_SCC_GSMRL_TPP Parameters
 */
#define QUICC_TX_PREAM_PAT_00S  0x0             /* all zero */
#define QUICC_TX_PREAM_PAT_10S  0x1             /* repeating 10's (ethernet)*/
#define QUICC_TX_PREAM_PAT_01S  0x2             /* repeating 01's */
#define QUICC_TX_PREAM_PAT_11S  0x3             /* all ones (LocalTalk) */

/*
 * QUICC_SCC_GSMRL_TDCR Parameters
 */
#define QUICC_TX_DIV_CLK_1X     0x0
#define QUICC_TX_DIV_CLK_8X     0x1
#define QUICC_TX_DIV_CLK_16X    0x2             /* (UART and AppleTalk) */
#define QUICC_TX_DIV_CLK_32X    0x3
 
/*
 * QUICC_SCC_GSMRL_RDCR Parameters
 */
#define QUICC_RX_DIV_CLK_1X     0x0
#define QUICC_RX_DIV_CLK_8X     0x1
#define QUICC_RX_DIV_CLK_16X    0x2             /* (UART and AppleTalk) */
#define QUICC_RX_DIV_CLK_32X    0x3
 
/*
 * QUICC_SCC_GSMRL_RENC Parameters
 */
#define QUICC_ENC_NRZ              0x0
#define QUICC_ENC_NRZI             0x1
#define QUICC_ENC_FM0              0x2
#define QUICC_ENC_MANCHESTER       0x4
#define QUICC_ENC_DIFF_MANCHESTER  0x6

/*
 * QUICC_SCC_GSMRL_DIAG Parameters
 */
#define QUICC_NORMAL_OP           0x0
#define QUICC_LOCAL_LOOPBACK      0x1
#define QUICC_AUTO_ECHO           0x2
#define QUICC_LOOPBACK_N_ECHO     0x3
#define QUICC_SCC_GSMRL_DIAG_MASK 0x000000c0

/*
 * QUICC_SCC_GSMRL_MODE Parameters.
 */
#define QUICC_SCC_HDLC_MODE	0x0000		/* Hdlc Mode */
#define QUICC_SCC_UART_MODE	0x0004		/* Uart Mode */
#define QUICC_SCC_ETHER_MODE	0x000C		/* Ethernet Mode */

/*
 * scc_psmr - Protocol Specific mode register.
 */
#define QUICC_PSMR_NOF(x)	((x) << 12)	/* Number of Flags (HDLC) */
#define QUICC_PSMR_HBC		0x8000		/* Heartbeat Check (Ether) */
#define QUICC_PSMR_FLC          0x8000          /* Flow control (uart) */
#define QUICC_PSMR_FC		0x4000		/* Force Collision (Ether) */
#define QUICC_PSMR_RSH		0x2000		/* Receive short frames (Ether) */
#define QUICC_PSMR_IAM		0x1000		/* Individual Address Mode */
#define QUICC_PSMR_CRC(x)	((x) << 10)	/* CRC Selection */
#define QUICC_PSMR_PRO		0x0200		/* Promiscuious Mode (Ether) */
#define QUICC_PSMR_RTE		0x0200		/* Retransmit Enable (HDLC) */
#define QUICC_PSMR_BRO		0x0100		/* Broadcast Address (Ether) */
#define QUICC_PSMR_FSE		0x0080		/* Flag Sharing Enabled (HDLC) */
#define QUICC_PSMR_SBT		0x0080		/* Stop Backoff Timer (Ether) */
#define QUICC_PSMR_DRT		0x0040		/* Disable Receiver while transmitting (HDLC) */
#define QUICC_PSMR_LPB		0x0040		/* Loopback Operation (Ether) */
#define QUICC_PSMR_BUS		0x0020		/* HDLC Bus Mode (HDLC) */
#define QUICC_PSMR_SIP		0x0020		/* Sample Input Pins (Ether) */
#define QUICC_PSMR_BRM		0x0010		/* HDLC Bus RTS Mode (HDLC) */
#define QUICC_PSMR_LCW		0x0010		/* Late Collision Window (Ether) */
#define QUICC_PSMR_MFF		0x0008		/* Multiple Frame in FIFO (HDLC) */
#define QUICC_PSMR_NIB(x)	((x) << 1)	/* Number of Ignore Bits (Ether) */
#define QUICC_PSMR_FDE          0x0001          /* Full Duplex (Ethernet) */

/*
 * QUICC_PSMR_CRC Parameters
 */
#define QUICC_16bit_CCITT_CRC   0x0             /* for HDLC only */
#define QUICC_32bit_CCITT_CRC   0x2             /* for Ethernet and HDLC */
#define QUICC_CRC_ETHER		0x0002		/* 32bit CCITT-CRC */
#define QUICC_CRC_HDLC		0x0000		/* 16bit CCITT-CRC */

/*
 * QUICC_PSMR_NIB Parameters
 */
#define QUICC_IGNORE_22BITS     0x5             /* for most situations */

/*
 * scc_scce - SCC Event Register.
 */

#define QUICC_SCCE_GLRX		0x1000		/* Glitch on Rx (HDLC) */
#define QUICC_SCCE_GLTX		0x0800		/* Glitch on Tx (HDLC) */
#define QUICC_SCCE_DCSCHG	0x0400		/* DPLL CS Changed (HDLC) */
#define QUICC_SCCE_FLGSTAT	0x0200		/* Flag Status (HDLC) */
#define QUICC_SCCE_AUTO_BAUD	0x0200		/* Auto baud status (UART) */
#define QUICC_SCCE_IDLCHG	0x0100		/* Idle Sequence Status Changed */

#define QUICC_SCCE_GSTOP	0x0080		/* Graceful Stop Completed */
#define QUICC_SCCE_BRKE		0x0040		/* Rx Break End (UART) 	*/
#define QUICC_SCCE_BRKS		0x0020		/* Rx Break Start (UART) */
#define QUICC_SCCE_TXERR	0x0010		/* Tx Error (HDLC) */
#define QUICC_SCCE_RXFRAME	0x0008		/* Rx Frame (HDLC) */
#define QUICC_SCCE_RXCCR	0x0008		/* Cntrl Char Rx (UART) */
#define QUICC_SCCE_BSY		0x0004		/* Busy Condition */
#define QUICC_SCCE_TXBUF	0x0002		/* Transmit Buffer */
#define QUICC_SCCE_RXBUF	0x0001		/* Receive Buffer */

/*
 * scc_sccm - SCC Mask Register.
 */

#define QUICC_SCCM_GLRX		0x1000		/* Glitch on Rx (HDLC) */
#define QUICC_SCCM_GLTX		0x0800		/* Glitch on Tx (HDLC) */
#define QUICC_SCCM_DCSCHG	0x0400		/* DPLL CS Changed (HDLC) */
#define QUICC_SCCM_FLGSTAT	0x0200		/* Flag Status (HDLC) */
#define QUICC_SCCM_AUTO_BAUD	0x0200		/* Auto baud status (UART) */
#define QUICC_SCCM_IDLCHG	0x0100		/* Idle Sequence Status Changed (HDLC) */


#define QUICC_SCCM_GSTOP	0x0080		/* Graceful Stop Conpleted */
#define QUICC_SCCM_BRKE		0x0040		/* Rx Break End (UART) 	*/
#define QUICC_SCCM_BRKS		0x0020		/* Rx Break Start (UART) */
#define QUICC_SCCM_TXERR	0x0010		/* Tx Error (HDLC) */
#define QUICC_SCCM_RXFRAME	0x0008		/* Rx Frame (HDLC) */
#define QUICC_SCCM_RXCCR	0x0008		/* Cntrl Char Rx (UART) */
#define QUICC_SCCM_BSY		0x0004		/* Busy Condition */
#define QUICC_SCCM_TXBUF	0x0002		/* Transmit Buffer */
#define QUICC_SCCM_RXBUF	0x0001		/* Receive Buffer */

/*
 * scc_sccs - SCC Status Register.
 */
#define QUICC_SCCS_FLG		0x04		/* Flags (HDLC) */
#define QUICC_SCCS_CS		0x02		/* Carrier Sence (HDLC) */
#define QUICC_SCCS_IDL   	0x01		/* Idle Status (HDLC) */


/*
 * cp_cr - RISC Command Register
 */
#define QUICC_CPCR_RST		0x8000		/* Software Reset Command */
#define QUICC_CPCR_OPCODE(x)	((x) << 8)	/* Operation Code */
#define QUICC_CPCR_CHNUM(x)	((x) << 4)	/* Channel Number */
#define QUICC_CPCR_SEMA		0x0001		/* Command Semaphore Flag */

/*
 * RISC commands
 */
#define QUICC_CPC_INIT_RXTX	0x0000		/* Initalize both Rx and Tx */
#define QUICC_CPC_INIT_RX	0x0001		/* Initalize Rx Parameters */
#define QUICC_CPC_INIT_TX	0x0002		/* Initalize Tx Parameters */
#define QUICC_CPC_ENTER_HUNT	0x0003		/* Enter Hunt Mode */
#define QUICC_CPC_TX_STOP	0x0004		/* Ignorant Stop Tx */
#define QUICC_CPC_GR_TX_STOP	0x0005		/* Polite Stop Tx */
#define QUICC_CPC_TX_RESTART	0x0006		/* Restart Tx */
#define QUICC_CPC_RX_CLOSE_BD	0x0007		/* Close Rx Buffer Descriptor */
#define QUICC_CPC_SET_GROUP	0x0008		/* Set Ethernet Group Address */
#define QUICC_CPC_SET_TIMER	0x0008		/* [De-]Activate Risc Timers */
#define QUICC_CPC_SPIN_MAX	0x4000		/* Amount of time to spin */

/*
 * Risc Channel numbers.
 */
#define QUICC_CHAN_SCC1		0x0000		/* SCC1 Channel Number */
#define QUICC_CHAN_SCC2		0x0004		/* SCC2 Channel Number */
#define QUICC_CHAN_SPI		0x0005		/* SPI Channel Number */
#define QUICC_CHAN_TIMER	0x0005		/* Timer Channel Number */
#define QUICC_CHAN_SCC3		0x0008		/* SCC3 Channel Number */
#define QUICC_CHAN_SCC4		0x000C		/* SCC4 Channel Number */
#define QUICC_CHAN_SMC1		0x0009		/* SMC1 Channel Number */
#define QUICC_CHAN_SMC2		0x000D		/* SMC2 Channel Number */

/*
 * cpic_cicr - CPM Interrupt Configuration Register (CICR)
 */
#define QUICC_CICR_SPS		0x0001		/* Spread Priority Scheme */
#define QUICC_CICR_SCdP(x)	((x) << 22)	/* SCCd Priority Order */
#define QUICC_CICR_SCcP(x)	((x) << 20)	/* SCCc Priority Order */
#define QUICC_CICR_SCbP(x)	((x) << 18)	/* SCCd Priority Order */
#define QUICC_CICR_SCaP(x)	((x) << 16)	/* SCCc Priority Order */
#define QUICC_CICR_IRL(x)	((x) << 13)	/* Interrupt Request Level */
#define QUICC_CICR_HP(x)	((x) << 8)	/* Highest Priority */
#define QUICC_CICR_VBA(x)	((x) << 5)	/* Vector Base Address */

/*
 * cpic_cipr - CPM Interrupt Pending Register (CIPR)
 */
#define QUICC_CIPR_PC0		0x80000000	/* Port C Pin 0 */
#define QUICC_CIPR_SCC1		0x40000000	/* SCC 1 */
#define QUICC_CIPR_SCC2		0x20000000	/* SCC 2 */
#define QUICC_CIPR_SCC3		0x10000000	/* SCC 3 */
#define QUICC_CIPR_SCC4		0x08000000	/* SCC 4 */
#define QUICC_CIPR_PC1		0x04000000	/* Port C Pin 1 */
#define QUICC_CIPR_TIMER1	0x02000000	/* Timer 1 */
#define QUICC_CIPR_PC2		0x01000000	/* Port C Pin 2 */
#define QUICC_CIPR_PC3		0x00800000	/* Port C Pin 3 */
#define QUICC_CIPR_SDMA		0x00400000	/* SDMA */
#define QUICC_CIPR_IDMA1	0x00200000	/* IDMA 1 */
#define QUICC_CIPR_IDMA2	0x00100000	/* IDMA 2 */
#define QUICC_CIPR_TIMER2	0x00040000	/* Timer 2 */
#define QUICC_CIPR_RTT		0x00020000	/* Risc Timer Table */
#define QUICC_CIPR_PC4		0x00008000     	/* Port C Pin 4 */
#define QUICC_CIPR_PC5		0x00004000     	/* Port C Pin 5 */
#define QUICC_CIPR_TIMER3	0x00001000     	/* Timer 3 */
#define QUICC_CIPR_PC6		0x00000800     	/* Port C Pin 6 */
#define QUICC_CIPR_PC7		0x00000400     	/* Port C Pin 7 */
#define QUICC_CIPR_PC8		0x00000200     	/* Port C Pin 8 */
#define QUICC_CIPR_TIMER4	0x00000080     	/* Timer 4 */
#define QUICC_CIPR_PC9		0x00000040     	/* Port C Pin 9 */
#define QUICC_CIPR_SPI		0x00000020     	/* SPI */
#define QUICC_CIPR_SMC1		0x00000010     	/* SMC 1 */
#define QUICC_CIPR_SMC2		0x00000008     	/* SMC 2 */
#define QUICC_CIPR_PC10		0x00000004     	/* Port C Pin 10 */
#define QUICC_CIPR_PC11		0x00000002     	/* Port C Pin 11 */

/*
 * cpic_cimr - CPM Interrupt Mask Register (CIMR)
 */
#define QUICC_CIMR_PC0		0x80000000	/* Port C Pin 0 */
#define QUICC_CIMR_SCC1		0x40000000	/* SCC 1 */
#define QUICC_CIMR_SCC2		0x20000000	/* SCC 2 */
#define QUICC_CIMR_SCC3		0x10000000	/* SCC 3 */
#define QUICC_CIMR_SCC4		0x08000000	/* SCC 4 */
#define QUICC_CIMR_PC1		0x04000000	/* Port C Pin 1 */
#define QUICC_CIMR_TIMER1	0x02000000	/* Timer 1 */
#define QUICC_CIMR_PC2		0x01000000	/* Port C Pin 2 */
#define QUICC_CIMR_PC3		0x00800000	/* Port C Pin 3 */
#define QUICC_CIMR_SDMA		0x00400000	/* SDMA */
#define QUICC_CIMR_IDMA1	0x00200000	/* IDMA 1 */
#define QUICC_CIMR_IDMA2	0x00100000	/* IDMA 2 */
#define QUICC_CIMR_TIMER2	0x00040000	/* Timer 2 */
#define QUICC_CIMR_RTT		0x00020000	/* Risc Timer Table */
#define QUICC_CIMR_PC4		0x00008000	/* Port C Pin 4 */
#define QUICC_CIMR_PC5		0x00004000	/* Port C Pin 5 */
#define QUICC_CIMR_TIMER3	0x00001000	/* Timer 3 */
#define QUICC_CIMR_PC6		0x00000800	/* Port C Pin 6 */
#define QUICC_CIMR_PC7		0x00000400	/* Port C Pin 7 */
#define QUICC_CIMR_PC8		0x00000200	/* Port C Pin 8 */
#define QUICC_CIMR_TIMER4	0x00000080	/* Timer 4 */
#define QUICC_CIMR_PC9		0x00000040	/* Port C Pin 9 */
#define QUICC_CIMR_SPI		0x00000020	/* SPI */
#define QUICC_CIMR_SMC1		0x00000010	/* SMC 1 */
#define QUICC_CIMR_SMC2		0x00000008	/* SMC 2 */
#define QUICC_CIMR_PC10		0x00000004	/* Port C Pin 10 */
#define QUICC_CIMR_PC11		0x00000002	/* Port C Pin 11 */

/*
 * cpic_cisr - CPM Interrupt in-Service Register (CISR)
 */
#define QUICC_CISR_PC0		0x80000000	/* Port C Pin 0 */
#define QUICC_CISR_SCC1		0x40000000	/* SCC 1 */
#define QUICC_CISR_SCC2		0x20000000	/* SCC 2 */
#define QUICC_CISR_SCC3		0x10000000	/* SCC 3 */
#define QUICC_CISR_SCC4		0x08000000	/* SCC 4 */
#define QUICC_CISR_PC1		0x04000000	/* Port C Pin 1 */
#define QUICC_CISR_TIMER1	0x02000000	/* Timer 1 */
#define QUICC_CISR_PC2		0x01000000	/* Port C Pin 2 */
#define QUICC_CISR_PC3		0x00800000	/* Port C Pin 3 */
#define QUICC_CISR_SDMA		0x00400000	/* SDMA */
#define QUICC_CISR_IDMA1	0x00200000	/* IDMA 1 */
#define QUICC_CISR_IDMA2	0x00100000	/* IDMA 2 */
#define QUICC_CISR_TIMER2	0x00040000	/* Timer 2 */
#define QUICC_CISR_RTT		0x00020000	/* Risc Timer Table */
#define QUICC_CISR_PC4		0x8000		/* Port C Pin 4 */
#define QUICC_CISR_PC5		0x4000		/* Port C Pin 5 */
#define QUICC_CISR_TIMER3	0x1000		/* Timer 3 */
#define QUICC_CISR_PC6		0x0800		/* Port C Pin 6 */
#define QUICC_CISR_PC7		0x0400		/* Port C Pin 7 */
#define QUICC_CISR_PC8		0x0200		/* Port C Pin 8 */
#define QUICC_CISR_TIMER4	0x0080		/* Timer 4 */
#define QUICC_CISR_PC9		0x0040		/* Port C Pin 9 */
#define QUICC_CISR_SPI		0x0020		/* SPI */
#define QUICC_CISR_SMC1		0x0010		/* SMC 1 */
#define QUICC_CISR_SMC2		0x0008		/* SMC 2 */
#define QUICC_CISR_PC10		0x0004		/* Port C Pin 10 */
#define QUICC_CISR_PC11		0x0002		/* Port C Pin 11 */

/*
 * Interrupt numbers defined for CPIC devices. Note that there are not
 * vector numbers. The vector number is constructed using a base in the
 * CPIC and these offsets.
 */

#define QUICC_CPIC_INTERRUPT_ERROR	0x00 /* Error  */
#define QUICC_CPIC_INTERRUPT_PC11	0x01 /* Parallel I/O */
#define QUICC_CPIC_INTERRUPT_PC10	0x02 /* Parallel I/O */
#define QUICC_CPIC_INTERRUPT_SMC2	0x03 /* SMC2 or PIP */
#define QUICC_CPIC_INTERRUPT_SMC1	0x04 /* SMC1 */
#define QUICC_CPIC_INTERRUPT_SPI	0x05 /* SPI */
#define QUICC_CPIC_INTERRUPT_PC9	0x06 /* Parallel I/O */
#define QUICC_CPIC_INTERRUPT_TIMER4	0x07 /* Timer 4 */
#define QUICC_CPIC_INTERRUPT_RSVD1	0x08 /* Reserved */
#define QUICC_CPIC_INTERRUPT_PC8	0x09 /* Parallel I/O */
#define QUICC_CPIC_INTERRUPT_PC7	0x0a /* Parallel I/O */
#define QUICC_CPIC_INTERRUPT_PC6	0x0b /* Parallel I/O */
#define QUICC_CPIC_INTERRUPT_TIMER3	0x0c /* Timer 3 */
#define QUICC_CPIC_INTERRUPT_RSVD2	0x0d /* Reserved */
#define QUICC_CPIC_INTERRUPT_PC5	0x0e /* Parallel I/O */
#define QUICC_CPIC_INTERRUPT_PC4	0x0f /* Parallel I/O */
#define QUICC_CPIC_INTERRUPT_RSVD3	0x10 /* Reserved */
#define QUICC_CPIC_INTERRUPT_RISC_TIMER	0x11 /* RISC timers */
#define QUICC_CPIC_INTERRUPT_TIMER2	0x12 /* Timer 2 */
#define QUICC_CPIC_INTERRUPT_RSVD4	0x13 /* Reserved */
#define QUICC_CPIC_INTERRUPT_IDMA2	0x14 /* IDMA 2 */
#define QUICC_CPIC_INTERRUPT_IDMA1	0x15 /* IDMA 1 */
#define QUICC_CPIC_INTERRUPT_SDMA	0x16 /* SDMA Channel bus error */
#define QUICC_CPIC_INTERRUPT_PC3	0x17 /* Parallel I/O */
#define QUICC_CPIC_INTERRUPT_PC2	0x18 /* Parallel I/O */
#define QUICC_CPIC_INTERRUPT_TIMER1	0x19 /* Timer 1 */
#define QUICC_CPIC_INTERRUPT_PC1	0x1a /* Parallel I/O */
#define QUICC_CPIC_INTERRUPT_SCC4	0x1b /* SCC4 */
#define QUICC_CPIC_INTERRUPT_SCC3	0x1c /* SCC3 */
#define QUICC_CPIC_INTERRUPT_SCC2	0x1d /* SCC2 */
#define QUICC_CPIC_INTERRUPT_SCC1	0x1e /* SCC1 */
#define QUICC_CPIC_INTERRUPT_PC0	0x1f /* Parallel I/O */


/*
 * PIT Parameters.
 */
/*
 * sim_picr - Periodic Interrupt Control Register
 */
#define QUICC_PICR_PIRQL(x)	((x) << 8)	/* Periodic Interrupt Level */
#define QUICC_PICR_PIV(x)	(x)		/* Periodic Interrupt Vector */

/*
 * sim_pitr - Periodic Interrupt Timing Register
 */
#define QUICC_PITR_SWP(x)	((x) << 9)	/* Software Watchdog Prescale */
#define QUICC_PITR_PTP(x)	((x) << 8)	/* Periodic Timer Prescale */

#define QUICC_PITR_PITR(x)	((x) & 0x00ff)  /* Periodic Interrupt Timer Register */

#define QUICC_PITR_PITR_M	0xff		/* PITR mask */

/*
 * sim_swsr - Software Service Values
 */
#define QUICC_SWSR_VAL1  0x55  /* magic number 1 */
#define QUICC_SWSR_VAL2  0xaa  /* magic number 2 */

/*
 * sim_bkcr - breakpoint control register
 * (actually a watchpoint)
 */
#define QUICC_BKCR_BAS          0x80000         /* breakpoint acknowledge support */
#define QUICC_BKCR_BUSS         0x40000         /* bus select */
#define QUICC_BKCR_RW_MSK       0x30000         /* read/write bits */
#define QUICC_BKCR_RW           0x20000         /* read & write cycles */
#define QUICC_BKCR_WRITE        0x10000         /* write cycles */
#define QUICC_BKCR_READ         0x00000         /* read cycles */
#define QUICC_BKCR_SIZM         0x08000         /* don't use size bits */
#define QUICC_BKCR_SIZ_MSK      0x06000         /* size bits mask */
#define QUICC_BKCR_SIZ_3B       0x06000         /* size 3 bytes */
#define QUICC_BKCR_SIZ_W        0x04000         /* size word */
#define QUICC_BKCR_SIZ_B        0x02000         /* size byte */
#define QUICC_BKCR_SIZ_LW       0x00000         /* size longword */
#define QUICC_BKCR_NEG          0x01000         /* use negative logic on block */
#define QUICC_BKCR_MA_MSK       0x00C00         /* mask address (block size) bits */
#define QUICC_BKCR_MA_32K       0x00C00         /* 32k block size */
#define QUICC_BKCR_MA_8K        0x00800         /* 8k block size */
#define QUICC_BKCR_MA_2K        0x00400         /* 2k block size */
#define QUICC_BKCR_MA_NON       0x00000         /* no block size */
#define QUICC_BKCR_AS_MSK       0x003FE         /* address space bits mask */
#define QUICC_BKCR_AS_DMA       0x00200         /* ignore DMA accesses */
#define QUICC_BKCR_AS_CPU       0x00100         /* ignore CPU accesses */
#define QUICC_BKCR_AS_SUPPROG   0x00080         /* ignore supervisor prog accesses */
#define QUICC_BKCR_AS_SUPDAT    0x00040         /* ignore supervisor data accesses */
#define QUICC_BKCR_AS_USRPROG   0x00008         /* ignore user program accesses */
#define QUICC_BKCR_AS_USRDAT    0x00004         /* ignore user data accesses */
#define QUICC_BKCR_AS_VALID     0x00001         /* enable the watchpoint */

/*
 * si_sicr - SI Clock Route Register.
 * The following macros may look complicated but the idea is simple.
 * Given a port (x) configure the parameters.  The parameter y specifies
 * where the clock source comes from.  The macros were written in a port
 * independent way to allow code sharing.
 */
#define QUICC_SICR_GRX(x)	(1 << (((x) << 3) + 7))	/* Grant Support of SCCx */
#define QUICC_SICR_SCX(x)	(1 << (((x) << 3) + 6))	/* SCCx Connection */
#define QUICC_SICR_RXCS(x,y)	((y) << (((x) << 3) + 3)) /* Receive Clock source */
#define QUICC_SICR_TXCS(x,y)	((y) << ((x) << 3))	/* Receive Clock source */

/*
 * si_sigmr - SI Global Mode Register.
 */
#define QUICC_SIGMR_ENABLE_A	0x04		/* Enable TDMa */
#define QUICC_SIGMR_ENABLE_B	0x08		/* Enable TDMb */
#define QUICC_SIGMR_ONE_DYNAMIC	0x01		/* One TDM, dynamic cheanges */

/*
 * Offsets into SI RAM for the bandwidth maps.
 */
#define QUICC_SIRAM_MAP1_RX	0x0000		/* Offset for RX Active Map */
#define QUICC_SIRAM_MAP1_TX	0x0040		/* Offset for TX Active Map */
#define QUICC_SIRAM_MAP2_RX	0x0020		/* Offset for RX Shadow Map */
#define QUICC_SIRAM_MAP2_TX	0x0060		/* Offset for TX Shadow Map */

#define QUICC_SIRAM_BITS_64	0x8		/* Significant bit at 64K */
#define QUICC_SIRAM_BITS_56	0x7		/* Significant bit at 56K */
#define QUICC_SIRAM_PAD1	0x2		/* Padding at 64K */ 
#define QUICC_SIRAM_PAD2	0x1		/* Padding at 64K */
#define QUICC_SIRAM_PAD		0x1		/* Pad because of bit shift */

/*
 * si_sicr - SI Clock Route register.
 */
#define QUICC_SCC_TO_BRG1	0x0000		/* Connect SCC to BGR1 */
#define QUICC_SCC_TO_BRG2	0x0001		/* Connect SCC to BGR2 */
#define QUICC_SCC_TO_BRG3	0x0002		/* Connect SCC to BGR3 */
#define QUICC_SCC_TO_BRG4	0x0003		/* Connect SCC to BGR4 */
#define QUICC_SCC_TO_CLK1	0x0004		/* Connect SCC to pin CLK1 */
#define QUICC_SCC_TO_CLK2	0x0005		/* Connect SCC to pin CLK2 */
#define QUICC_SCC_TO_CLK3	0x0006		/* Connect SCC to pin CLK3 */
#define QUICC_SCC_TO_CLK4	0x0007		/* Connect SCC to pin CLK4 */
#define QUICC_SCC_TO_CLK5	0x0004		/* Connect SCC to pin CLK5 */
#define QUICC_SCC_TO_CLK6	0x0005		/* Connect SCC to pin CLK6 */
#define QUICC_SCC_TO_CLK7	0x0006		/* Connect SCC to pin CLK7 */
#define QUICC_SCC_TO_CLK8	0x0007		/* Connect SCC to pin CLK8 */
#define QUICC_SCC_CS_MASK       0x0007


/*
 * These values should be used when the port number is
 * required.
 */
#define QUICC_PORT_SCC1		0x0000		/* SCC1 Port Number */
#define QUICC_PORT_SCC2		0x0001		/* SCC2 Port Number */
#define QUICC_PORT_SCC3		0x0002		/* SCC3 Port Number */
#define QUICC_PORT_SCC4		0x0003		/* SCC4 Port Number */
#define QUICC_PORT_SMC1		0x0004		/* SMC1 Port Number */
#define QUICC_PORT_SMC2		0x0005		/* SMC2 Port Number */
#define QUICC_PORT_SPI		0x0006		/* SPI Port Number */
#define QUICC_PORT_TIMER	0x0007		/* Timer Port */

#define QUICC_VBA_MASK		0x00E0		/* Vector Base Address Mask */

/*
 * smc_smcmr - SMC UART Mode Register.
 */
#define QUICC_SMCMR_CHAR_LEN9	0x4800 /* 8 data, no parity, 1 stop */
#define QUICC_SMCMR_CHAR_LEN10	0x5000 /* 8 data, no parity, 2 stop */
#define QUICC_SMCMR_CLEN(x)	((x) << 11)	/* Character Length */
#define QUICC_SMCMR_2STOP	0x0400		/* 2 stop bits */
#define QUICC_SMCMR_PAR_ENA	0x0200		/* Parity Enable */
#define QUICC_SMCMR_PAR_EVEN	0x0100		/* Even Parity */
#define QUICC_SMCMR_SM(x)	((x) << 4)	/* SMC Mode */
#define QUICC_SMCMR_DM(x)	((x) << 2)	/* Diagnostic Mode */
#define QUICC_SMCMR_UART	0x0020		/* UART mode */
#define QUICC_SMCMR_ECHO	0x0008
#define QUICC_SMCMR_LOC_LOOP	0x0004          /* local loopback */
#define QUICC_SMCMR_TX_EN	0x0002		/* Tx Enable */
#define QUICC_SMCMR_RX_EN	0x0001		/* Rx Enable */

/*
 * SCC UART Mode Register
 */
#define QUICC_UART_STOPBITS(x)		(((x)-1) << 14)	/* x = {1,2}, Number stop bits */

/* set the databit length */
#define QUICC_UART_MIN_DATA_LEN		5
#define QUICC_UART_MAX_DATA_LEN		8       
#define QUICC_UART_DATABIT_LEN(x)	(((x)-QUICC_UART_MIN_DATA_LEN) << 12)
#define QUICC_UART_TX_PARITY(x)		(x)
#define QUICC_UART_RX_PARITY(x)		((x) << 2) /* set the receiver parity */

#define QUICC_UART_CTS_FLOW(x)		(1 << (4 + ((x) * 2)))
#define QUICC_UART_DCD_FLOW(x)		(1 << (5 + ((x) * 2)))
#define QUICC_UART_ENABLE_PARITY	0x0010	/* Enable parity */

/* SMC UART function code register values */
#define QUICC_SMCFC_MOT		0x18
#define QUICC_SPIFC_MOT		0x18
#define QUICC_SCCFC_MOT		0x18
#define QUICC_SMCFC_INTEL	0x08

/*
 * smc_smce - SMC Event Register
 */
#define QUICC_SMCE_BRK		0x0010		/* Break Character Received */
#define QUICC_SMCE_BSY		0x0004		/* Busy Condition */
#define QUICC_SMCE_TXBUF	0x0002		/* Tx Buffer */
#define QUICC_SMCE_RXBUF	0x0001		/* Rx Buffer */

/*
 * smc_smcm - SMC Mask Register
 */
#define QUICC_SMCM_BRK		0x0010		/* Break Character Received */
#define QUICC_SMCM_BSY		0x0004		/* Busy Condition */
#define QUICC_SMCM_TXBUF	0x0002		/* Tx Buffer */
#define QUICC_SMCM_RXBUF	0x0001		/* Rx Buffer */

/* BRGC values */
#define QUICC_BRGC_RESET		0x20000
#define QUICC_BRGC_EN_CLK		0x10000
#define QUICC_BRGC_CLK_SRC(n)		((n) << 14)
#define QUICC_BRGC_AUTO_BAUD		0x2000
#define QUICC_BRGC_CLK_DIV(n)		(((n) << 1) & 0x01ffe)
#define QUICC_BRGC_CLK_DIV_50		1952 /* at 25 Mhz system clock */
#define QUICC_BRGC_CLK_DIV_75		1301
#define QUICC_BRGC_CLK_DIV_150		650
#define QUICC_BRGC_CLK_DIV_300		324
#define QUICC_BRGC_CLK_DIV_600		2603
#define QUICC_BRGC_CLK_DIV_1200		1301
#define QUICC_BRGC_CLK_DIV_2400		650
#define QUICC_BRGC_CLK_DIV_4800		324
#define QUICC_BRGC_CLK_DIV_9600		162
#define QUICC_BRGC_CLK_DIV_19200	80
#define QUICC_BRGC_CLK_DIV_38400	40
#define QUICC_BRGC_CLK_DIV_57600	26
#define QUICC_BRGC_CLK_DIV_115200	13
#define QUICC_BRGC_CLK_DIV_SHIFT	1
#define QUICC_BRGC_CLK_MASK		0x00001FFE
#define QUICC_BRGC_DIV_BY_16		0x1
#define QUICC_BRGC_MAX_DIV_VAL		4096

/* SIMODE register values */
#define QUICC_SIMODE_SMC2_MODE		0 /* NMSI mode */
#define QUICC_SIMODE_SMC2_CLK		0x30000000 /* BRG04 */
#define QUICC_SIMODE_SMC1_MODE		0 /* NMSI mode */
#define QUICC_SIMODE_SMC1_CLK		0x00002000 /* BRG03 */
#define QUICC_SIMODE_TDMA_AUTO_ECHO	0x04000000
#define QUICC_SIMODE_TDMA_INT_LOOP 	0x08000000
#define QUICC_SIMODE_TDMA_LOOPBACK 	0x0C000000
#define QUICC_SIMODE_TDMB_AUTO_ECHO	0x00000400
#define QUICC_SIMODE_TDMB_INT_LOOP 	0x00000800
#define QUICC_SIMODE_TDMB_LOOPBACK 	0x00000C00
#define QUICC_SIMODE_RFSDa(x)		((x) << 8)
#define QUICC_SIMODE_CRTa		0x00000040
#define QUICC_SIMODE_STZa		0x00000020

#define QUICC_SIMODE_SMC_CS_BRG1	0
#define QUICC_SIMODE_SMC_CS_BRG2	1
#define QUICC_SIMODE_SMC_CS_BRG3	2
#define QUICC_SIMODE_SMC_CS_BRG4	3
#define QUICC_SIMODE_SMC_CS_CLK5	4
#define QUICC_SIMODE_SMC_CS_CLK6	5
#define QUICC_SIMODE_SMC_CS_CLK7	6
#define QUICC_SIMODE_SMC_CS_CLK8	7

#define QUICC_SIMODE_SMC1_CS(n)		((n) << 12)
#define QUICC_SIMODE_SMC2_CS(n)		((n) << 28)

/*
 * Macros for building SIRAM entries.
 */
#define QUICC_SIRAM_SWRT		0x4000
#define QUICC_SIRAM_SSEL(x)		((x) << 10)
#define QUICC_SIRAM_CSEL(x)		(((x) + 1)<< 6)
#define QUICC_SIRAM_CNT_BYTE(x)		((((x) - 1) << 2) | QUICC_SIRAM_BYTE)
#define QUICC_SIRAM_CNT_BIT(x)		(((x) - 1) << 2)
#define QUICC_SIRAM_BYTE		0x0002
#define QUICC_SIRAM_LST			0x0001

#define QUICC_SIRAM_SSEL1		0x0001
#define QUICC_SIRAM_SSEL2		0x0002
#define QUICC_SIRAM_SSEL3		0x0004
#define QUICC_SIRAM_SSEL4		0x0008

/*
 * TGCR Register - Timer Global Configuration Register
 */
#define QUICC_TGCR_TMR1_EN   0x0001    /* Reset or enable a timer */
#define QUICC_TGCR_TMR2_EN   0x0010
#define QUICC_TGCR_TMR3_EN   0x0100
#define QUICC_TGCR_TMR4_EN   0x1000

/*
 * TMR Registers - The 4 Timer Mode Registers (TMR1, TMR2, TMR3, TMR4)
 */
#define QUICC_TMR_USEC_PRESCALE (25 << 8) /* With 25 MHz clock:   */
                                          /* (40ns x 25) = 1 usec */
#define QUICC_TMR_MAX_PRESCALE 0xFF00
#define QUICC_TMR_INT_EN       0x0010 /* ORI - Output Reference Int Enable */
#define QUICC_TMR_RESET_MODE   0x0008 /* FRR - Free Run/Restart mode */

/* ICLK - Hardware Timer Clock Source bits */
#define QUICC_TMR_ICLK_CASCADE      0x0000 /* Source is internally cascaded */
#define QUICC_TMR_ICLK_INTCLK       0x0002 /* Source is internal clock */
#define QUICC_TMR_ICLK_INTCLK_DIV16 0x0004 /* Source is internal clock/16 */
#define QUICC_TMR_ICLK_TIN_PIN      0x0006 /* Source is corresponding TIN pin */

/*
 * TER Registers - The 4 Timer Event Registers (TER1, TER2, TER3, TER4)
 */
#define QUICC_TER_CAP_EVENT  0x0001  /* Counter value latched into TCR */
#define QUICC_TER_REF_EVENT  0x0002  /* Counter has reached TRR value */

/*
 * Memory Controller Registers
 */

/*
 * Base Register (BR)
 */
#define QUICC_BR_WRITE_PROTECT 0x00000002 /* Allow only read accesses within */
                                          /* the address range of a BR       */

/*
** MBAR defines
*/
#define QUICC_MBAR_ADDR      0x3FF00     /* Address of MBAR */
#define QUICC_MBAR_MASK      0xffffe000  /* Mask for address of MBAR */

/*
** Base Register defines
*/
#define QUICC_BR_VALID       0x00000001  /* register is valid */
#define QUICC_BR_WR_PROT     0x00000002  /* chip select is write protected */
#define QUICC_BR_PAR_EN      0x00000004  /* parity checking is enabled */

/*
 * IPL Levels
 */
#define QUICC_IPL_LEVEL7     0x0007      /* NMI Interrupt */
#define QUICC_IPL_LEVEL6     0x0006
#define QUICC_IPL_LEVEL5     0x0005
#define QUICC_IPL_LEVEL4     0x0004
#define QUICC_IPL_LEVEL3     0x0003
#define QUICC_IPL_LEVEL2     0x0002
#define QUICC_IPL_LEVEL1     0x0001
#define QUICC_IPL_LEVEL0     0x0000      /* No Interrupt */

/*
 ** AVR defines
 */
#define QUICC_AVR_IRQ1   0x02        /* External Interrupt level 1 */
#define QUICC_AVR_IRQ2   0x04        /* External Interrupt level 2 */
#define QUICC_AVR_IRQ3   0x08        /* External Interrupt level 3 */
#define QUICC_AVR_IRQ4   0x10        /* External Interrupt level 4 */
#define QUICC_AVR_IRQ5   0x20        /* External Interrupt level 5 */
#define QUICC_AVR_IRQ6   0x40        /* External Interrupt level 6 */
#define QUICC_AVR_IRQ7   0x80        /* External Interrupt level 7 */

/*
 * QUICC Auto vector numbers for exception table.
 */
#define QUICC_AVEC1		25
#define QUICC_AVEC2		26
#define QUICC_AVEC3		27
#define QUICC_AVEC4		28
#define QUICC_AVEC5		29
#define QUICC_AVEC6		30
#define QUICC_AVEC7		31

/*
 * QUICC AVR Register.
 */
#define QUICC_AVR0		0x01
#define QUICC_AVR1		0x02
#define QUICC_AVR2		0x04
#define QUICC_AVR3		0x08
#define QUICC_AVR4		0x10
#define QUICC_AVR5		0x20
#define QUICC_AVR6		0x40
#define QUICC_AVR7		0x80

/*
 * QUICC CPU SSW defines
 */
#define QUICC_SSW_TP     0x8000   /* BERR frame type, 1 = exception processing */
#define QUICC_SSW_MV     0x4000   /* movem instruction in progress */
#define QUICC_SSW_SZC1   0x2000   /* bit 1 of original operand size field */
#define QUICC_SSW_TR     0x1000   /* trace pending */
#define QUICC_SSW_B1     0x0800   /* breakpoint channel 1 pending */
#define QUICC_SSW_B0     0x0400   /* breakpoint channel 0 pending */
#define QUICC_SSW_RR     0x0200   /* rerun write cycle ofter RTE */
#define QUICC_SSW_RM     0x0100   /* fault was a read-modify-write */
#define QUICC_SSW_IN     0x0080   /* instruction prefetch, 0 = operand fetch */
#define QUICC_SSW_RW     0x0040   /* read or write cycle, 1 = read */
#define QUICC_SSW_SZC0   0x0020   /* bit 0 of original operand size field */
#define QUICC_SSW_SIZ_M  0x0018   /* 2 bit field for remaining size of bus cycle */
#define QUICC_SSW_FUNC_M 0x0007   /* 3 bit field for function code of bus cycle */

/*
 * RSR Reset Status Register defines
 */
#define QUICC_RSR_SRSTP   0x01    /* soft reset pin */
#define QUICC_RSR_SRST    0x02    /* soft reset */
#define QUICC_RSR_LOC     0x04    /* loss of clock reset */
#define QUICC_RSR_DBF     0x10    /* double bus fault reset */
#define QUICC_RSR_SW      0x20    /* software watchdog reset */
#define QUICC_RSR_POW     0x40    /* power up reset */
#define QUICC_RSR_EXT     0x80    /* external total system reset */

/*
 * QUICC GMR defines
 */

#define QUICC_GMR_OPAR	0x00000800	/* odd parity */
#define QUICC_GMR_PBEE	0x00000400	/* parity bus error enable */


/*
 * QUICC MSTAT defines
 */

#define QUICC_MSTAT_WPER	0x0100	/* write protect error */
#define QUICC_MSTAT_PER8	0x0080	/* parity error bank 8 */
#define QUICC_MSTAT_PER7	0x0040	/* parity error bank 7 */
#define QUICC_MSTAT_PER6	0x0020	/* parity error bank 6 */
#define QUICC_MSTAT_PER5	0x0010	/* parity error bank 5 */
#define QUICC_MSTAT_PER4	0x0008	/* parity error bank 4 */
#define QUICC_MSTAT_PER3	0x0004	/* parity error bank 3 */
#define QUICC_MSTAT_PER2	0x0002	/* parity error bank 2 */
#define QUICC_MSTAT_PER1	0x0001	/* parity error bank 1 */
#define QUICC_MSTAT_PER_MASK	0x00ff	/* parity error mask */
#define QUICC_MSTAT_ERROR_MASK	0x01ff	/* all errors mask */

/*
 * TOSEQ values (UART)
 */
#define QUICC_TOSEQ_READY       0x2000  /* Ready to transmit */
#define QUICC_TOSEQ_INTR        0x1000  /* Interrupt on completion */
#define QUICC_TOSEQ_CTSLOST     0x0800  /* CTS lost during char */
#define QUICC_TOSEQ_ADDR        0x0100  /* Address for multidrop */

/*
 * UART protocol specific RAM.
 * CHARCTER 1-8.
 */
#define QUICC_UART_CHARX_INVALID 0x8000 /* Entry is invalid */
#define QUICC_UART_CHARX_REJECT  0x4000 /* Reject char - don't write to bd */

/* 
 * UART protocol specific RAM
 * RCCM (control character mask)
 */
#define QUICC_UART_NO_CTRL_CHAR_MASK 0xc0ff /* No mask */

/*
 * PSMR Masks.  Any fields that needs clearing will need to have a
 * mask.  This is to handle cases where setting a bit field 0 have
 * a specific meaning. 
 */
#define QUICC_UART_FLOWC_MASK	  0x8000 /* Mask for flow control */
#define QUICC_UART_STOPBITS_MASK  0x4000 /* Mask for stop bits */
#define QUICC_UART_CHARLEN_MASK   0x3000 /* Mask for char length */
#define QUICC_UART_MODE_MASK   	  0x0c00 /* Mask for UART Mode */
#define QUICC_UART_PARITY_MASK	  0x000F /* Mask for Parity Mask for RX/TX */
#define QUICC_PSMR_FRZ		  0x0200 /* Freeze transmission (UART) */

/*
 * End of ifndef __QUICC_H__
 */
#endif

