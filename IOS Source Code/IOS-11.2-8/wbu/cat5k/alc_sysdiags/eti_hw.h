/* $Id: eti_hw.h,v 1.1.4.1 1996/05/09 15:01:05 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/eti_hw.h,v $
 *------------------------------------------------------------------
 * eti_hw.h -- Ethernet Interface H/W Definitions & Constants
 *
 * ?
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: eti_hw.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:05  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:03  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:27  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:14  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:59  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:33  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* CALYPSO DIAGNOSTICS                                                        */
/*                                                                            */
/* This file contains "Ethernet Interface H/W Definitions & Constants" that   */
/* are included as part of CALYPSO Diagnostics.                               */
/*                                                                            */
/* NOTE TO THE REVIEWER:                                                      */
/* --------------------                                                       */
/* You may find that Comments & Remarks are kept to the minimum extent, which */
/* eventually would be updated in due course of time.  The author believes in */
/* "Self-documentation" by way of meaningful names for procedures, variables  */
/* etc. and "Hierarchical programming" style which would work out better in   */
/* terms of time & productivity.  In the meantime, if you encounter any       */
/* problems in going thru the code or have any comments/suggestions, do let   */
/* the author know about it.                                                  */
/*                                                                            */
/******************************************************************************/

#define	ETHERNET_MIN_PACKETSIZE		64
#define	ETHERNET_MAX_PACKETSIZE		1518
#define	ETHERNET_DEFAULT_PACKETSIZE	256

#define	ETHERNET_HEADER_SIZE		14
#define	ETHERNET_MIN_PACKET_DATA_SIZE	46
#define	ETHERNET_TRAILER_SIZE		4

	struct SONIC_REGS {
	   uInt16	unused_1;
	   uInt16	cr;
#define CR_LCAM 	0x0200		/* Load CAM */
#define CR_RRRA		0x0100		/* Read RRA */
#define CR_RST		0x0080  	/* Software Reset */
#define CR_ST		0x0020		/* Start Timer */
#define CR_STP		0x0010  	/* Stop Timer */
#define CR_RXEN		0x0008  	/* Receive Enable */
#define CR_RXDIS	0x0004 	 	/* Receive Disable */
#define CR_TXP		0x0002 	 	/* Transmit Packet */
#define CR_HTX		0x0001		/* Halt Transmission */
#define CR_SOFT_RST1	0x0094		/* CR after soft reset */
#define CR_SOFT_RST2	0x00a4		/* CR after soft reset */

	   uInt16	unused_2;
	   uInt16	dcr;
#define DCR_VCC			/* Pins tied to Vcc */
#define DCR_GROUND		/* Pins tied to Ground */
#define DCR_WC0		0x0000  /* Zero Bus Cycles Added */
#define DCR_WC1		0x0040  /* One Bus Cycles Added */
#define DCR_WC2		0x0080  /* Two Bus Cycles Added */
#define DCR_WC3		0x00c0  /* Three Bus Cycles Added */
#define DCR_DW16	0x0000  /* 16-Bit Data Width */
#define DCR_DW32	0x0020  /* 32-Bit Data Width */
#define DCR_BMS0	0x0000  /* Empty/Fill Mode */
#define DCR_BMS1	0x0010  /* Block Mode */
#define	DCR_SYNC	0x0400	/* Synchronous mode */
#define DCR_RFT2	0x0000  /* 2 Words Receive FIFO Threshold */
#define DCR_RFT4	0x0004  /* 4 Words Receive FIFO Threshold */
#define DCR_RFT8	0x0008  /* 8 Words Receive FIFO Threshold */
#define DCR_RFT12	0x000c  /* 12 Words Receive FIFO Threshold */
#define DCR_TFT4	0x0000  /* 4 Words Transmit FIFO Threshold */
#define DCR_TFT8 	0x0001  /* 8 Words Transmit FIFO Threshold */
#define DCR_TFT12	0x0002  /* 12 Words Transmit FIFO Threshold */
#define DCR_TFT14	0x0003  /* 14 Words Transmit FIFO Threshold */

	   uInt16	unused_3;
	   uInt16	rcr;		/* receive control */
#define RCR_ERR		0x8000		/* Accept Error Packet */
#define RCR_RNT		0x4000  	/* Accept Runt Packet */
#define RCR_BRD		0x2000  	/* Accept Broadcast Packet */
#define RCR_PRO		0x1000		/* Promiscuious Mode */
#define RCR_AMC		0x0800		/* Accept All Multicast Packet */ 
#define RCR_LB1		0x0400
#define RCR_LB0		0x0200
#define RCR_MC		0x0100  	/* Multicast Packet Received */
#define RCR_BC		0x0080  	/* Broadcast Packet Received */
#define RCR_LPKT	0x0040 	 	/* Last Packet In RBA */
#define RCR_CRS		0x0020  	/* Carrier Sense Activity */
#define RCR_COL		0x0010  	/* Collision Activity */
#define RCR_CRC		0x0008  	/* CRC Error */
#define RCR_FAE		0x0004  	/* FAE Error */
#define RCR_LBK		0x0002  	/* Loopback Packet Received */
#define RCR_PRX		0x0001  	/* Packet Received ok */
#define	RCR_LGEN_STATUS_MASK 0x000D

	   uInt16	unused_4;
	   uInt16	tcr;		/* transmit control */
#define TCR_PINT	0x8000	/* Programmable Interrupt */
#define TCR_POWC	0x4000  /* Program "Out Of Window" Timer */
#define TCR_CRCI	0x2000  /* CRC Inhibit */
#define	TCR_EXDIS	0x1000	/* Excessive deferral timer */
#define TCR_LOOP1	0x0000  /* No Loopback */
#define TCR_LOOP2	0x0800  /* Internal Loopback */
#define TCR_LOOP3	0x1000	/* ENDEC Loopback */
#define TCR_LOOP4	0x1800  /* Transceiver Loopback */
#define TCR_EXD		0x0400  /* Excessive Deferal */
#define TCR_DEF		0x0200  /* Deferred Transmission */
#define TCR_NCRS	0x0100  /* No CRS */
#define TCR_CRSL	0x0080  /* CRS Lost */
#define TCR_EXC		0x0040  /* Excessive Collisions */
#define TCR_OWC		0x0020  /* Out of Window Collisions */
#define TCR_HBL		0x0010  /* CD Heartbeat Lost */
#define TCR_PMB		0x0008  /* Transmitted Packet Not Received OK */
#define TCR_FU		0x0004  /* FIFO Underrun */
#define TCR_BCM		0x0002  /* Byte Count Mismatch */
#define TCR_PTX		0x0001  /* Packet Transmitted OK */
#define	TCR_ERROR	(TCR_EXD|TCR_EXC|TCR_PMB|TCR_FU|TCR_BCM)
#define	TCR_COLL	0xF800
#define	TCR_STATUS_MASK	0xFFFE
#define	TCR_LGEN_STATUS_MASK 0x05FF

	   uInt16	unused_5;
	   uInt16	imr;		/* interrupt mask */
#define IMR_PINTEN	0x0800	/* Programmable Interrupt Enable */
#define IMR_PRXEN	0x0400  /* Packet Received Enable */
#define IMR_PTXEN	0x0200  /* Packet Transmitted OK Enable */
#define IMR_TXEREN	0x0100  /* Transmit Error Enable */
#define IMR_TCEN	0x0080  /* Timer Complete Enable */
#define IMR_RDEEN	0x0040  /* Receive Descriptors Exhausted Enable */
#define IMR_RBEEN	0x0020  /* Receive Buffers Exhausted Enable */
#define IMR_RBAEEN	0x0010  /* RBA Exceeded Enable */
#define IMR_CRCEN	0x0008  /* CRC Tally Counter Overflow Enable */
#define IMR_FAEEN	0x0004  /* FAE Tally Counter Overflow Enable */
#define IMR_MPEN	0x0002  /* MP Tally Counter Overflow Enable */
#define IMR_RFOEN	0x0001  /* Receive FIFO Enable */
#define	SONIC_intmask	(IMR_PRXEN|IMR_PTXEN|IMR_TXEREN|IMR_RBEEN)

	   uInt16	unused_6;
	   uInt16	isr;		/* interrupt status */
#define ISR_LCD		0x1000	/* LCAM done */
#define ISR_PINT	0x0800	/* Programmable Interrupt */
#define ISR_PKTRX	0x0400  /* Packet Received */
#define ISR_TXDN	0x0200  /* Transmission Done */
#define ISR_TXER	0x0100  /* Transmission Error */
#define ISR_TC		0x0080	/* Timer Complete */
#define ISR_RDE		0x0040  /* Receive Descriptors Exhausted */
#define ISR_RBE		0x0020  /* Receive Buffers Exhausted */
#define ISR_RBAE	0x0010  /* RBA Exceeded */
#define ISR_CRC		0x0008  /* CRC Tally Counter Rollover */
#define ISR_FAE		0x0004  /* FAE Tally Counter Rollover */
#define ISR_MP		0x0002  /* Missed Packet Counter Rollover */
#define ISR_RFO 	0x0001	/* Receive FIFO Overrun */
			/* Interrupt mask */
#define ISR_IMASK	(ISR_PKTRX | ISR_TXDN | ISR_TXER | ISR_RBE)
#define SONIC_REGMASK	0x0f

	   uInt16	unused_7;
	   uInt16	utda;		/* upper transmit descriptor addr */
	   uInt16	unused_8;
	   uInt16	ctda;		/* current transmit descriptor addr */
	   uInt16	unused_9;
	   uInt16	tps;		/* transmit packet size */
	   uInt16	unused_10;
	   uInt16	tfc;		/* transmit fragment count */
	   uInt16	unused_11;
	   uInt16	tsa0;		/* transmit start address 0 */
	   uInt16	unused_12;
	   uInt16	tsa1;		/* transmit start address 1 */
	   uInt16	unused_13;
	   uInt16	tfs;		/* transmit fragment size */
	   uInt16	unused_14;
	   uInt16	urda;		/* upper receive descriptor address */
	   uInt16	unused_15;
	   uInt16	crda;		/* current receive descriptor address */
	   uInt16	unused_16;
	   uInt16	crba0;		/* current receive buffer addr 0 */
	   uInt16	unused_17;
	   uInt16	crba1;		/* current receive buffer addr 1 */
	   uInt16	unused_18;
	   uInt16	rbwc0;		/* remaining buffer word count 0 */
	   uInt16	unused_19;
	   uInt16	rbwc1;		/* remaining buffer word count 1 */
	   uInt16	unused_20;
	   uInt16	eobc;		/* end of buffer word count */
	   uInt16	unused_21;
	   uInt16	urra;		/* upper receive resource addr */
	   uInt16	unused_22;
	   uInt16	rsa;		/* resource start address */
	   uInt16	unused_23;
	   uInt16	rea;		/* resource end address */
	   uInt16	unused_24;
	   uInt16	rrp;		/* resource read address */
	   uInt16	unused_25;
	   uInt16	rwp;		/* resource write address */
	   uInt16	unused_26;
	   uInt16	trba0;		/* temp. recv. buffer addr 0 */
	   uInt16	unused_27;
	   uInt16	trba1;		/* temp. recv. buffer addr 1 */
	   uInt16	unused_28;
	   uInt16	tbwc0;		/* temp. buffer word count 0 */
	   uInt16	unused_29;
	   uInt16	tbwc1;		/* temp. buffer word count 1 */
	   uInt16	unused_30;
	   uInt16	addr0;		/* address generator 0 */
	   uInt16	unused_31;
	   uInt16	addr1;		/* address generator 1 */
	   uInt16	unused_32;
	   uInt16	llfa;		/* last linked field address */
	   uInt16	unused_33;
	   uInt16	ttda;		/* temp transmit descriptor addr */
	   uInt16	unused_34;
	   uInt16	cep;		/* CAM entry point */
	   uInt16	unused_35;
	   uInt16	cap2;		/* CAM address port 2 */
	   uInt16	unused_36;
	   uInt16	cap1;		/* CAM address port 1 */
	   uInt16	unused_37;
	   uInt16	cap0;		/* CAM address port 0 */
	   uInt16	unused_38;
	   uInt16	ce;		/* CAM enable */
	   uInt16	unused_39;
	   uInt16	cdp;		/* CAM descriptor pointer */
	   uInt16	unused_40;
	   uInt16	cdc;		/* CAM descriptor count */
	   uInt16	unused_41;
	   uInt16	sr;		/* silicon revision */
	   uInt16	unused_42;
	   uInt16	wt0;		/* watchdog timer 0 */
	   uInt16	unused_43;
	   uInt16	wt1;		/* watchdog timer 1 */
	   uInt16	unused_44;
	   uInt16	rsc;		/* receive sequence counter */
	   uInt16	unused_45;
	   uInt16	crct;		/* CRC error tally */
	   uInt16	unused_46;
	   uInt16	faet;		/* FAE error tally */
	   uInt16	unused_47;
	   uInt16	mpt;		/* missed packet tally */
	   uInt16	unused_48;
	   uInt16	mdt;		/* maximum deferral timer */
	   uInt16	unused_49;
	   uInt16	rtc;		/* receive test control */
	   uInt16	unused_50;
	   uInt16	ttc;		/* transmit test control */
	   uInt16	unused_51;
	   uInt16	dtc;		/* DMA test control */
	   uInt16	unused_52;
	   uInt16	cc0;		/* CAM comparison 0 */
	   uInt16	unused_53;
	   uInt16	cc1;		/* CAM comparison 1 */
	   uInt16	unused_54;
	   uInt16	cc2;		/* CAM comparison 2 */
	   uInt16	unused_55;
	   uInt16	cm;		/* CAM match */
	   uInt16	unused_56;
	   uInt16	res1;		/* reserved */
	   uInt16	unused_57;
	   uInt16	res2;		/* reserved */
	   uInt16	unused_58;
	   uInt16	rbc;		/* receive byte count */
	   uInt16	unused_59;
	   uInt16	res3;		/* reserved */
	   uInt16	unused_60;
	   uInt16	tbc;		/* transmitter backoff counter */
	   uInt16	unused_61;
	   uInt16	trc;		/* transmitter random counter */
	   uInt16	unused_62;
	   uInt16	tbm;		/* transmitter backoff mask */ 
	   uInt16	unused_63;
	   uInt16	res4;		/* reserved */
	   uInt16	unused_64;
	   uInt16	dcr2;		/* data configuration register 2 */
};


/*
 * Receive Resource Area Descriptor
 */

struct RRA {
	uInt32 buff_ptr0;		/* buffer address (low 16 bits) */
	uInt32 buff_ptr1;		/* buffer address (high 16 bits) */
	uInt32 buff_wc0;		/* buffer size (low 16 bits) */
	uInt32 buff_wc1;		/* buffer size (high 16 bits) */
};

/*
 * CAM Descriptor Area
 */

struct CDA {
	uInt32 ep;
	uInt32 cap0;
	uInt32 cap1;
	uInt32 cap2;
};


/* RDA data structure */

struct RDA {
	uInt32 status ;	/* receive status = rcr register */
	uInt32 byte_count ;	/* bytes in packet */
	uInt32 pkt_ptr0;		/* pointer to data (low) */
	uInt32 pkt_ptr1;		/* pointer to data (high) */
	uInt32 seq_no;	/* packet number, sequnce number */
	uInt32		rda_link;	/* link to next rda, including eol */
	volatile uInt32 inuse;	/* sonic/system ownership flag */
	uInt32 filler;
};

/*
 * These macros extract the packet and sequence numbers from a 16-bit value.
 * The RBA sequence number is in bits 8-15, and the packet sequence number
 * is in bits 0-7.
 */

#define	EOL	0x1			/* end of list bit in rda_link */

/*
 * transmit descriptor area.
 */
#define MAX_NTFRAGS	2

struct TDA {
	volatile uInt32  status;	/* transmit status */
	uInt32	pkt_config;	/* configuration stuff */
	uInt32	pkt_size;	/* packet size */
	uInt32	frag_count;	/* fragment count */
	uInt32	frag_ptr0;	/* low order 16 bits of address */
	uInt32	frag_ptr1;	/* high order 16 bits of address */
	uInt32	frag_size;
	uInt32  tda_link;
};

struct SONIC_CAM {
	uInt32	cam_ep;
	uInt32	cam_port0;
	uInt32	cam_port1;
	uInt32	cam_port2;
	uInt32	cam_mask;
};

#define	SONIC_PORT_QSRAM_REGION_SIZE	0x4000
#define	SONIC_PORT_FRAMEBUF_REGION_SIZE	0x20000

#ifndef RISC
#define	RRA_START	0x09000000
#define	RRA_SIZE	0x10

#define	CDA_START	0x09000400
#define	CDA_SIZE	0x10

#define	RDA_START	0x09000800
#define	RDA_SIZE	0x20

#define	TDA_START	0x09001000
#define	TDA_SIZE	0x20

#define	RXBUF_START	0x0E000000
#define	RXBUF_SIZE	0x800

#define	TXBUF_START	0x0E100000
#define	TXBUF_SIZE	0x800

#ifdef RAM
#define	RRA_START_LCP	0x01300000

#define	CDA_START_LCP	0x01300400

#define	RDA_START_LCP	0x01300800

#define	TDA_START_LCP	0x01301000

#define	RXBUF_START_LCP	0x01320000

#define	TXBUF_START_LCP	0x01330000
#else /* RAM */
#define	RRA_START_LCP	0x01000000

#define	CDA_START_LCP	0x01000400

#define	RDA_START_LCP	0x01000800

#define	TDA_START_LCP	0x01001000

#define	RXBUF_START_LCP	0x01020000

#define	TXBUF_START_LCP	0x01030000
#endif /* RAM */
#else /* RISC */
#define	RRA_START	0xA9020000
#define	RRA_SIZE	0x10

#define	CDA_START_ARB	0xA9000400
#define	CDA_START	0xA9020400
#define	CDA_SIZE	0x10

#define	RDA_START	0xA9020800
#define	RDA_SIZE	0x20

#define	TDA_START_ARB	0xA9001000
#define	TDA_START	0xA9021000
#define	TDA_SIZE	0x20

#define	RXBUF_START	0xAE200000
#define	RXBUF_SIZE	0x800

#define	TXBUF_START_ARB	0xAE100000
#define	TXBUF_START	0xAE300000
#define	TXBUF_SIZE	0x800
#endif /* RISC */

