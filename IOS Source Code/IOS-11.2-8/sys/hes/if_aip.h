/* $Id: if_aip.h,v 3.4.54.4 1996/05/31 15:30:49 rzagst Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_aip.h,v $
 *------------------------------------------------------------------
 * if_aip.h: Holds the AIP specific constants & definitions
 *
 * November, 1993, Rick Li Fo Sjoe
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: if_aip.h,v $
 * Revision 3.4.54.4  1996/05/31  15:30:49  rzagst
 * CSCdi54940:  ATM driver creates low-bit-rate SVCs
 * Branch: California_branch
 *
 * Revision 3.4.54.3  1996/05/13  18:41:09  rzagst
 * CSCdi57247:  Burst size handled improperly between AIP and 4500
 * Branch: California_branch
 *
 * Revision 3.4.54.2  1996/05/07  16:00:02  rzagst
 * CSCdi45984:  CBUS-3-OUTHUNG: ATM3/0: tx0 output hung (800E = queue full)
 * Branch: California_branch
 *
 * Revision 3.4.54.1  1996/04/25  23:12:06  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.4.58.2  1996/04/10  18:28:17  rzagst
 * ATM Modularity: Add () to #define per code review action item.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.4.58.1  1996/03/30  03:46:47  rzagst
 * ATM Modularity: Clean up and modularize by platform ATM parser files.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.4  1995/11/23  19:57:48  bbenson
 * CSCdi42518:  vc creation fails because of wrong max_vpibits in ILMI
 * Fixes bits per VPI/VCI reported by the ILMI.
 *
 * Revision 3.3  1995/11/17  09:17:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:00:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:40:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __IF_AIP_H__
#define __IF_AIP_H__

#define AIP_RATEQ_NO            8       /* No. of RATEQs supported */
#define AIP_MIN_PEAK_RATE       130     /* 130kb min peak rate */
#define	AIP_HOLDQ		40	/* Max. HOLDQ size */

/*
 * The bandwidth value for display (in Kbps)
 */
#define VISIBLE_AIP_BANDWIDTH  100000
#define	VISIBLE_AIP_BANDWIDTH_SONET 156250
#define	VISIBLE_AIP_BANDWIDTH_4B5B  100000
#define	VISIBLE_AIP_BANDWIDTH_E3    34013
#define	VISIBLE_AIP_BANDWIDTH_DS3   45000

/*
 * Delay values (in 10 usec units)
 */
#define	AIP_DELAY		10	/* Default IGRP delay interval */
#define	AIP_SONET_DELAY		8	/* IGRP delay for SONET */
#define	AIP_4B5B_DELAY		10	/* IGRP delay for TAXI */
#define	AIP_E3_DELAY		20	/* IGRP delay for E3 */
#define	AIP_DS3_DELAY		19	/* IGRP delay for DS3 */
#define AIP_DS3_FRAMING_DEFAULT		ATM_FRAMING_DS3CBITPLCP
#define AIP_E3_FRAMING_DEFAULT          ATM_FRAMING_E3G832ADM

#define	AIP_MAX_BURST		(4470/48)	/* 4470 byte packets -> cells */
#define	AIP_CONFIG_DELAY	2	/* Config reset delay (sec) */

/*
 * Max number of bits for VCI/VPI.  Used by ILMI to tell the switch
 * how to allocate these.
 */
#define AIP_MAX_VPIBITS		8
#define AIP_MAX_VCIBITS		16
#define AIP_TOTAL_VPIVCI_BITS (AIP_MAX_VPIBITS + AIP_MAX_VCIBITS)

/* 
 * Define valid AIP commands
 */
#define	AIP_CMD_SETUP_VC	1
#define	AIP_CMD_TEARDOWN_VC	2
#define	AIP_CMD_CONFIG		3
#define	AIP_CMD_GET_STATS	4

/*
 * Define AIP return(status) codes on command completion
 */
#define	AIP_NOCMD		0
#define	AIP_CONFIG_FAIL		1
#define	AIP_CMD_OK		2
#define	AIP_CMD_FAIL		4
#define	AIP_INVLD_PLIM		8

/*
 * Below are error codes returned to higher layer(s).
 */
#define	AIP_STS_OK		0		/* Command completed */
#define	AIP_STS_NO_VC		1		/* VC index isn't enabled */
#define	AIP_STS_BAD_VC		2		/* VC index value invalid */
#define	AIP_STS_BAD_RATEQ	3		/* RATEQ value invalid */
#define	AIP_STS_BAD_VPI		4		/* VC VPI value invalid */
#define	AIP_STS_BAD_VCI		5		/* VC VCI value invalid */
#define	AIP_STS_BAD_AAL		6		/* AAL not supported */
#define	AIP_STS_BAD_CONFIG	7		/* Config. request bad */
#define	AIP_STS_CMD_FAIL	8		/* AIP command rejected */
#define	AIP_STS_CONFIG_FAIL	9		/* AIP config. rejected */
#define	AIP_STS_TIMEOUT		0x8000		/* AIP command timed out */
#define	AIP_STS_BUSY		0xFFFF		/* AIP processing command */

/*
 * Define the FCI flags to handle the AIP 'FCI_CMD_CONDITIONS' 
 * register values.
 */
#define	AIP_COND_CMD		0x2000		/* Command complete signal */
#define	AIP_COND_RSVD1		0x1000		/* A free bit */
#define	AIP_COND_STS_MASK	0x0F00		/* Completion status(above)*/
#define	AIP_COND_RSVD2		0x0080		/* Another free bit! */

#define	AIP_BAD_RATEQ		-1
#define AIP_RATEQ_DELAYED       -2

/*
 * Define AIP love letter codes.
 */
#define	AIP_EVNT_VC_STATS	0x0001		/* per-VC statistic info */

/*
 * Define AIP specific commands and wait periods
 */
#define	AIP_CMD_WAIT		1		/* AIP maximum wait time(S) */

typedef struct aip_cmd_t_ {
    ushort	cmd;
    ushort	status;
} aip_cmd_t;

/*
 * Define the Descriptor Mode bits for per-packet processing
 */
#define	AIP_DM_PTI		0xE000		/* (see PTI below) */
#define	AIP_DM_XD_INTT		0x1000
#define	AIP_DM_EOM_EN		0x0800
#define	AIP_DM_APP_CRC32	0x0400
#define	AIP_DM_PKT_AAL34	0x0000		/* AAL 3/4 fragmentation */
#define	AIP_DM_PKT_AAL5		0x0100		/* AAL 5 fragmentation */
#define	AIP_DM_PKT_OAM_VC	0x0200		/* OAM info. from VC table */
#define	AIP_DM_PKT_OAM_DM	0x0300		/* OAM info. from PTI */

/*
 * Below are the definitions of the RATEQ values possible in each of the
 * AIP RATEQUEUE registers.
 * NOTE:  (From the SARA manual)
 *	"The aggregate sum of all peak rates of active queueus should not
 *	exceed either the link or SARA-S bandwidth to avoid over-subscribtion.
 *	Over-subscription can result in blocking the low priority rate queue
 *	and starving other rate queues in the same bank."
 *
 * Rate = (255 - (424/Peak_Rate)*(1000/Tclk)*(1/Prescaler))
 * where:
 *	Rate is the 'PreLoad' value in the RATE register
 *	Peak_Rate is the Peak segmentation rate desired in Mbps
 *	Tclk is the SARA-S "CLK" period in nS.
 *	PreScaler is a choice of 4 16 64 or 256 in the RATE register
 *
 * The table below gives common 'PreLoad' values given desired rates & Scaler.
 * This assumes the SARA-S is working with a 20Mhz(50nS) CLK value.
 * 					Prescaler value
 *	Desired rate	00		01		10		11
 *      ------------------------------------------------------------------------
 *	1Mbps		INVALID		INVALID		122		221
 *	5Mbps		INVALID		149		228		248
 *	10Mbps		43		202		241		251
 *	20Mbps		149		228		248		INVALID
 *	45Mbps		207		243		252		INVALID
 *	50Mbps		212		244		INVALID		INVALID
 *	100Mbps		233		249		INVALID		INVALID
 *	155Mbps		241		251		INVALID		INVALID
 *
 * NOTE: The AIP has a CLK value of 20Mhz(50nS) so the values above can
 *	be used directly.
 */
#define	AIP_RATEQ_RSVD		0x0000
#define	AIP_RATEQ_XOFF_EN	0x0800		/* XOFF enable */
#define	AIP_RATEQ_ENABLE	0x0400		/* Enable this RATEQ */
#define	AIP_RATEQ_PRESCALE0	0x0000		/* CLK/4 & Rate clock */
#define	AIP_RATEQ_PRESCALE1	0x0100		/* CLK/16 & Rate clock */
#define	AIP_RATEQ_PRESCALE2	0x0200		/* CLK/64 & Rate clock */
#define	AIP_RATEQ_PRESCALE3	0x0300		/* CLK/256 & Rate clock */
#define	AIP_RATEQ_PRELOAD	0x00FF		/* PRELOADer mask */

typedef struct aip_cmd_cfg_t_ {
    aip_cmd_t	aip_cmd;
    atm_config	aip_cfg;			/* AIP config. info. */
} aip_cmd_cfg_t;

/*
 * The DEFINES below describe the "Virtual Circuit Mode Bits" of the
 * VC table struct in the AIP.
 */
#define	AIP_VCM_CRC32		0x8000		/* Enable CRC32 generation */
#define	AIP_VCM_CCMODE_MASK	0x7000		/* CC mode mask */
#define	AIP_VCM_RATEQ_MASK	0x0E00		/* RateQ Mask */
#define	AIP_VCM_SWCONG_MASK	0x01E0		/* S/W Congestion mask */
#define	AIP_VCM_TIQ_MASK	0x001F		/* Time Interval Quota mask */

#define	AIP_CCMODE_1CELL	0x0000		/* 1 cell recovery */
#define	AIP_CCMODE_2CELL	0x1000		/* 2 cell recovery */
#define	AIP_CCMODE_4CELL	0x2000		/* 4 cell recovery */
#define	AIP_CCMODE_8CELL	0x3000		/* 8 cell recovery */
#define	AIP_CCMODE_16CELL	0x4000		/* 16 cell recovery */
#define	AIP_CCMODE_RSVD		0x5000		/* RSVD */
#define	AIP_CCMODE_SWCONG	0x6000		/* S/W congestion controll */
#define	AIP_CCMODE_FLUSHVC	0x7000		/* Flush packets this VC */

#define	AIP_SWCONG_NORMAL	0x0000		/* Normal S/W congestion */
#define	AIP_SWCONG_50		0x0180		/* 50 % */
#define	AIP_SWCONG_25		0x0190		/* 25 % */
#define	AIP_SWCONG_13		0x01A0		/* 12.5 % */
#define	AIP_SWCONG_6		0x01B0		/* 6.25 % */
#define	AIP_SWCONG_SEGSUSP	0x01C0		/* Segmentation suspend */
#define	AIP_SWCONG_RSVD1	0x01D0
#define	AIP_SWCONG_RSVD2	0x01E0

#define	AIP_MIN_CQ		1
#define	AIP_MAX_CQ		63
#define	AIP_MIN_TIQ		1
#define	AIP_MAX_TIQ		63
#define AIP_MIN_BURST_SIZE      32
#define	AIP_DEFAULT_CQ		AIP_MAX_CQ	/* Default CellQuota value */
#define AIP_MIN_REAK_RATE       130         /* 130Kbps is min aip peak rate */

/*
 * VC cell header definitions: (Source "ATM UNI Spec." version 2.4)
 * |----0----|----1----|----2----|----3----|----4----|
 * |GFC |   VPI   |        VCI        |PTI |   HEC   |
 * |____|____|____|____|____|____|____|____|____|____|
 *                                     xxx0          Unassigned
 *                                0001 0a0c          Meta-signal(default)
 *       yyyy yyyy                0001 0a0c          Meta-Signal
 *                                0010 0aac          General-Broadcast(default)
 *       yyyy yyyy                0010 0aac          General-Broadcast
 *                                0101 0aac          Point-to-Point signal(def.)
 *       yyyy yyyy                0101 0aac          Point-to-Point signal
 *  xxxx                          0000 xxx1          INVALID pattern
 *       aaaa aaaa                0011 0a0a          Segment OAM F4 cell
 *       aaaa aaaa                0100 0a0a          End-to-End OAM F4 cell
 *
 *                           0001 0000 aaa0          ILMI message
 *
 * For protocol traffic, the VCI & VPI contain valid connection values.
 *                                     000x          User data no cong. sdu=0
 *                                     001x          User data no cong. sdu=1
 *                                     010x          User data cong. sdu=0
 *                                     011x          User data cong. sdu=1
 *                                     100x          OAM F5 segment cell
 *                                     101x          OAM F5 end-to-end cell
 *                                     110x          RSVD
 *                                     111x          RSVD
 *
 * Essentially, the following VPI/VCI values are predefined
 *	VPI		VCI		Connection
 *	x		1		Meta-Signalling
 *	x		2		General-Broadcasting
 *	0		3		OAM F4 segment cell
 *	0		4		OAM F4 end-to-end cell
 *	x		5		Point-to-Point signalling
 *	0		16		ILMI message
 *
 * Where:
 * Blank- Implies a ZERO in those locations
 *    y	- VPI value != ZERO
 *    x - Anything "Don't care"
 *    a - For use by ATM layer function
 *    c - Signal entity sets CLP=0, network may change it.
 *
 * NOTE:
 * (1) GFC MUST be set to ZERO by originating router/host
 * (2) HEC is set by the AIP during fragmentation.
 */

#define	AIP_RP_COMM_MSG		(-1)	/* Comm. msg. to RP from AIP */

typedef struct aip_cmd_setup_vc_t_ {
    aip_cmd_t	aip_cmd;
    vcd_t	vcd;				/* VCD # to create */
    ushort	enctype;			/* encaps type */
    /*
     * Define the first 4 octets of each cell now 
     */
    ushort	gfc_vpi;		/* (4)GFC & (8)VPI & (4)VCI */
    ushort	vci_pti;		/* (12)VCI & (4)PTI */
    ushort	vcmode;			/* (16)Congestion, Rate,etc...*/
    ushort	cellquota;		/* (6)Average metering stuff */
    ushort	mid_low;		/* (10)Lower MID value */
    ushort	mid_high;		/* (10)Upper MID value */
    ushort	betag;			/* (8)BeTAG starting value */
} aip_cmd_setup_vc_t;

typedef struct aip_cmd_teardown_vc_t_ {
    aip_cmd_t 	aip_cmd;
    vcd_t	vcd;				/* VCD # to delete */
} aip_cmd_teardown_vc_t;

typedef	struct	aip_cmd_getvc_stats_t_ {
    aip_cmd_t	aip_cmd;
    volatile vcd_t	vcd;
    volatile ushort	vpi;
    volatile ushort	vci;
    volatile ushort	tx_pkt_hi;
    volatile ushort	tx_pkt_lo;
    volatile ushort	tx_byte_hi;
    volatile ushort	tx_byte_lo;
    volatile ushort	rx_pkt_hi;
    volatile ushort	rx_pkt_lo;
    volatile ushort	rx_byte_hi;
    volatile ushort	rx_byte_lo;
} aip_getvc_stats_t;

/*
 * Define the Interface specific encapsulation fields.
 */
typedef struct	vpd_t_ {
    vcd_t	vcnum;				/* VPD to use */
    ushort	vctype;				/* Encaps. type */
} vpd_t;
	

/*
 * Prototypes
 */
void aip_restart_init(hwidbtype *);
void aip_platform_init(subsystype*);
void aip_get_plim_info(hwidbtype *hwidb);
void aip_setup(hwidbtype*);
ushort * aip_sniff(hwidbtype *, paktype *, uchar *, ushort *, int, int);
void aip_love_note_common (hwidbtype *, ushort);
uint aip_verify_qos_params(hwidbtype *hwidb,unsigned peakrate,unsigned avgrate,
			   unsigned burstcells);


#ifndef	RSP
void aip_love_note(cdbtype *, ushort);
#endif
#ifdef	RSP
void aip_love_note(hwidbtype *, lovenotetype *);
#endif

#endif     /* __IF_AIP_H__ */
