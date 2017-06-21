/* $Id: if_pas_ibm2692.h,v 3.14.4.13 1996/08/24 01:51:13 abaindur Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_ibm2692.h,v $
 *------------------------------------------------------------------
 * if_pas_ibm2692.h - Platform independent header file for 4R
 *		      IBM Token Ring PCI port adaptor
 *
 * May 1995, HC Wang
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_ibm2692.h,v $
 * Revision 3.14.4.13  1996/08/24  01:51:13  abaindur
 * CSCdi66979:  Traffic shaping broken on the c7200
 * Branch: California_branch
 *
 * Revision 3.14.4.12  1996/06/07  23:52:28  sthormod
 * CSCdi57928:  OIR of 4R Wedges Console (No Throttling Code)
 * Branch: California_branch
 *
 * Revision 3.14.4.11  1996/06/07  00:17:50  hwang
 * CSCdi59533:  4r driver need to implement srb command queue
 * Branch: California_branch
 *
 * Revision 3.14.4.10  1996/05/24  20:00:15  hwang
 * CSCdi58573:  4R receiver become deaf
 * Enable Hermon timerinterrupt to sample pci busmaster status
 * register every 500 ms, if it indicates receiver channel is
 * not enabled or disabled reenable receive channel.
 * Branch: California_branch
 *
 * Revision 3.14.4.9  1996/05/24  00:50:05  mbeesley
 * CSCdi58316:  Need to fully support CPQ on new interfaces
 * Branch: California_branch
 *
 * Revision 3.14.4.8  1996/05/23  00:11:34  sthormod
 * CSCdi55611:  4r port adapter output queue wedges under stress...
 * Branch: California_branch
 *
 * Revision 3.14.4.7  1996/05/13  04:58:09  hwang
 * CSCdi57003:  4R DFS support
 * Branch: California_branch
 *
 * Revision 3.14.4.6  1996/05/07  00:48:41  sthormod
 * CSCdi55832:  4r PA driver needs to clean-up after OIR
 * Branch: California_branch
 *
 * Revision 3.14.4.5  1996/05/01  03:39:38  tboyle
 * CSCdi54205: Reduce buffer space reqd by 4R
 * Branch: California_branch
 *
 * Revision 3.14.4.4  1996/04/23  02:23:57  sthormod
 * CSCdi54994:  4R uses cpu to byte-swap
 * Branch: California_branch
 *
 * Revision 3.14.4.3  1996/03/30  01:30:42  hwang
 * CSCdi51725:  4r driver need to provide lan manager support
 * Branch: California_branch
 * Set up proper address match bit when processing mac frame, note that
 * we have two different path to receive a mac frame, either via normal
 * LLC channel or using ARB.
 *
 * Revision 3.14.4.2  1996/03/21  23:23:34  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.14.4.1  1996/03/18  21:34:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.11  1996/03/18  23:28:58  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.10  1996/03/10  02:49:56  sthormod
 * CSCdi51196:  add tbridge switching path for predator 4R
 * Branch: ELC_branch
 *
 * Revision 3.2.2.9  1996/03/06  19:51:43  sthormod
 * CSCdi50859:  add srb switching path for predator 4R
 * Branch: ELC_branch
 * o  process & fast switching path for srb & explorers.
 * o  routing with rif field.
 * o  use dma to coalesce particle based packet.
 * o  process mac frames.
 *
 * Revision 3.2.2.8  1996/02/21  22:46:04  sthormod
 * CSCdi49158:  predator/4R keep alives dont work properly
 * Branch: ELC_branch
 * - add token-ring error support
 * - add keep alive code
 * - update lastoutput
 * - add ibm2692_version_string()
 * - remove ibm2692_linestate(), use default tring_linestate
 * - move ibm2692_periodic() to idb->device_periodic vector
 * - add idb->system_configured vector
 *
 * Revision 3.2.2.7  1996/02/08  08:48:03  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.6  1996/01/31  01:37:34  sthormod
 * Branch: ELC_branch
 * Add Predator 4R support.
 *
 * Revision 3.2.2.5  1996/01/16  01:43:06  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.4  1995/12/19  23:32:36  sthormod
 * Branch: ELC_branch
 * Initial structure changes for porting 4R driver to predator.
 *
 * Revision 3.2.2.3  1995/12/11  19:13:51  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.2  1995/12/06  04:08:53  sthormod
 * Branch: ELC_branch
 * Bring in HC's code review changes.
 *
 * Revision 3.2.2.1  1995/11/30  02:06:01  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.14  1996/03/11  17:46:54  hwang
 * CSCdi50643:  4r driver doesnt handle mac frame correctly
 *
 * Revision 3.13  1996/02/22  02:29:11  hwang
 * CSCdi49454:  4R driver need to use new ibm mpc ucode feature in tb mode
 * Set disable ASA copy option if configured in tb mode, this bit is
 * not documented in the ibm spec.
 *
 * Revision 3.12  1996/02/14  22:02:06  hwang
 * CSCdi48583:  vines packets not forwarded in 4r-4r transparent bridge
 * Reduce the size of reject address table, if tb is configured xmt one
 * packet at a time (no chained packet to avoid rejection addr tbl
 * overflow). When access the packet memory use cpu view address instead
 * of pci byte swap address.
 *
 * Revision 3.11  1996/02/12  23:06:49  hwang
 * CSCdi46480:  4R failed hot standby test
 * Add configuration mechanism to allow 4r driver to configure
 * new functional address.
 *
 * Revision 3.10  1996/02/05  03:37:07  hwang
 * CSCdi47198:  router with rvip (4R & 4T) hang after config SRT
 * ibm token ring chip if configured to transparent bridge mode it will
 * copy broad cast frame it transmitted, which causing the network
 * flooding.
 * Implement software filter to drop such frame received.
 *
 * Revision 3.9  1996/01/31  18:59:00  hwang
 * CSCdi47842:  4R xmt channel disabled after xmt fifo underrun error
 * If tx error bits are set check the tx busmaster status, if tx is
 * disabled re enable it.
 *
 * Revision 3.8  1996/01/23  23:50:47  hwang
 * CSCdi46141:  rvip 4r/4r crashes with ipx traffic
 * Hermon tx bug work around, don't give hermon any packet to xmt before
 * end of chain tx interrupt.
 *
 * Revision 3.7  1996/01/11  19:39:31  hwang
 * CSCdi46480:  4R failed hot standby test
 * vip_token_classify function to return device stat in trip style.
 * Remove some unused error messages.
 *
 * Revision 3.6  1995/12/21  17:49:13  hwang
 * CSCdi45680:  appletalk over SRB using RVIP fails
 * Enable limited RI broadcast copy, only screening all
 * bridge multicast packet.
 *
 * Revision 3.5  1995/12/14  19:42:07  hwang
 * Modify packet classify function to provide correct tb/srb type packet,
 * use latest ibm info to set the hidden copy_all_options in modify_receive
 * option srb command.
 * CSCdi43509:  srb bridging over rvip fails
 *
 * Revision 3.4  1995/12/12  01:47:03  hwang
 * CSCdi45395:  4r driver need to update pci device id
 *
 * Revision 3.3  1995/12/03  06:39:44  hwang
 * CSCdi44890:  incorporate code review comments into 4r driver
 *
 * Revision 3.2  1995/11/17  18:54:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include platform dependant header file here to pick up definitions.
 * They are included once more at the end of this file to pick
 * up static inline definitions.
 */
#if defined(RVIP) || defined(SVIP)
#include "if_vip_ibm2692.h"
#endif

#if defined(PREDATOR)
#include "if_c7100_ibm2692.h"
#endif

#include "if_pas_ibm2692_common.h"

/* IBM Hermon PCI controller device/vendor ID */
#define IBMTR_VENDOR_DEVICE_ID 0x00181014

#define PCI_4R_NUM_INTERFACES	4

#define HERMON_MPC_RESET_DELAY  20
#define HERMON_MPC_ENABLE_DELAY	20

/* transparent bridging source address reject table */
 
#define REJECT_TBL_SIZE         8
#define REJECT_INDEX_MASK       REJECT_TBL_SIZE - 1
 
#define MAX_TR_PKT_SIZE 4496
#define MIN_TR_PKT_SIZE 18

#define IBM2692_CRC_SIZE	4

/* Hermon PCI command register bit definition  offset 0x01 */

#define	FBB_ENABLE	0x02000000	/* fast back to back enable */
#define SERR_ENABLE	0x01000000	/* PCI adr bus parity error enable */
#define PARITY_ENABLE	0x00400000	/* parity error enable */
#define MWI_ENABLE	0x00100000	/* memory write/invalidate enable */
#define BUS_MASTER	0x00040000	/* bus master mode enable */
#define MEM_SPACE	0x00020000	/* pci memory access enable */
#define IO_SPACE	0x00010000	/* pci I/O access enable */

/* Hermon LAP access region equate */

#define	LAP_REG_WE	0x0080	/* enable LAP registers write */
#define LAP_REG_RE	0x0040	/* enable LAP registers read */
#define LAP_RSV_WE	0x0020	/* enable LAP reserved region write */
#define LAP_RSV_RE	0x0010	/* enable LAP reserved region read */
#define LAP_ROM_WE	0x0008	/* enable LAP ROM region write */
#define LAP_ROM_RE	0x0004	/* enable LAP ROM region read */
#define LAP_RAM_WE	0x0002	/* enable LAP RAM region write */

/* Hermon system interrupt status mask register SISRM */

#define	SYS_INT_ENA	0x8000	/* system interrupt enable */
#define	SCB_STS		0x4000	/* SCB status port interrupt enable */
#define	SCB_CTL		0x2000	/* SCB control port interrupt enable */
#define SCB_INT_MASK	0x1000	/* SCB interrupt mask */
#define	TMR_INT_MASK	0x0800	/* timer interrupt mask */
#define LAP_PERR_MASK	0x0400	/* LAP port data parity error int mask */
#define LAP_ACC_ERR_M	0x0200	/* LAP port access violation error int mask */
#define PCI_PERR_MASK	0x0100	/* PCI parity error interrupt mask */

#define ADP_CHK		0x0040	/* adapter check interrupt mask */
#define SRB_RESP	0x0020	/* SRB response interrupt mask */
#define ASB_FREE	0x0010	/* ASB free interrupt mask */
#define ARB_CMD		0x0008	/* ARB command interrupt mask */
#define TRB_RESP	0x0004	/* TRB response interrupt mask */

/* 
 * Hermon local interrupt status register LISR 
 * bit 15 - 8 of this register is the local interrupt mask and status
 * bit 7 - 0 is used to generate interrupt to MPC
 */

#define	LOCAL_INT_ENA	0x8000	/* local interrupt enable */
#define	LOCAL_INT_MASK	0x2000	/* local interrupt mask */
#define	L_SCB_INT_MASK	0x0800	/* local scb interrupt mask */
#define ICDB_PERR_MASK	0x0200	/* local icdb parity error int mask */

#define	SIF_SRB_CMD	0x0020	/* SIF SRB command interrupt */
#define	SIF_ASB_RSP	0x0010	/* SIF ASB free interrupt */
#define SIF_ASB_FREE	0x0004	/* SIF ASB free request interrupt */
#define SIF_ARB_FREE	0x0002	/* SIF ARB free interrupt */
#define SIF_TRB_CMD	0x0001	/* SIF frame in TRB command interrupt */

/* Hermon system interrupt status register SISR */

#define	MI_IND		0x8000	/* master interrupt indication bit */
#define SCB_STAT_INT	0x4000	/* SCB status port interrupt bit */
#define SCB_CNTL_INT	0x2000	/* SCB control port interrupt bit */
#define SCB_SIG_INT	0x1000	/* SCB signalling port interrupt */
#define TMR_INT		0x0800	/* timer expired interrupt */
#define LAP_PERR_INT	0x0400	/* LAP data parity error interrupt */
#define LAP_ACC_INT	0x0200	/* LAP access violation error interrupt */
#define PCI_PERR_INT	0x0100	/* PCI parity error interrupt */

#define UNUSED_INT	SCB_STAT_INT | SCB_CNTL_INT | SCB_SIG_INT |\
			LAP_PERR_INT | LAP_ACC_INT | PCI_PERR_INT

/* Hermon bus master interrupt status register */

#define TX2_EOC_INT	0x8000	/* transmit channel 2 end of chain int */
#define TX2_NOST_INT	0x4000	/* transmit channel 2 no status posted int */
#define TX2_HALT_INT	0x2000	/* transmit channel 2 halt interrupt */
#define TX2_EOF_INT	0x1000	/* transmit channel 2 end of frame interrupt */
#define TX2_INT_MASK	0xf000	/* transmit channel 2 interrupt mask */

#define TX1_EOC_INT     0x0800  /* transmit channel 1 end of chain int */
#define TX1_NOST_INT    0x0400  /* transmit channel 1 no status posted int */
#define TX1_HALT_INT    0x0200  /* transmit channel 1 halt interrupt */
#define TX1_EOF_INT     0x0100  /* transmit channel 1 end of frame interrupt */
#define TX1_INT_MASK	0x0f00	/* transmit channel 1 interrupt mask */

#define RX_NBA_INT	0x0020	/* receive channel no buffer int */
#define RX_EOB_INT	0x0010	/* receive channel end of frame int */
#define RX_NOSTA_INT	0x0004	/* receive channel no status posted int */
#define RX_HALT_INT	0x0002	/* receive channel halt int */
#define RX_EOF_INT	0x0001	/* receive channel end of frame interrupt */
#define RX_INT_MASK	0x0037	/* receive channel int mask */

#define PCI_FIFO_SIZE   0x0002	/* hermon PCI busmaster fifo threshold */

/* Hermon bus master control register (bmctl) bit definition */

#define TX1_DISABLE_REQ	0x8000	/* transmit channel 1 disable request bit */
#define TX1_DISABLED	0x4000	/* transmit channel 1 disabled */
#define TX1_ENABLED	0x2000	/* transmit channel 1 enabled */
#define TX1_DISABLE_PENDING	0x1000	/* transmit channel 1 disable pending */

#define TX2_DISABLE_REQ	0x0800	/* transmit channel 2 disable request bit */
#define TX2_DISABLED	0x0400	/* transmit channel 2 disabled */
#define TX2_ENABLED	0x0200	/* transmit channel 2 enabled */
#define TX2_DISABLE_PENDING	0x0100	/* transmit channel 2 disable pending */

#define RX_DISABLE_REQ	0x0080	/* receive channel disable request bit */
#define RX_DISABLED	0x0040	/* receive channel disabled */
#define RX_ENABLED	0x0020	/* receive channel enabled */
#define RX_DISABLE_PENDING	0x0010	/* receive channel disable pending */
#define RX_BM_ACTIVE_MASK	0x00f0	/* receive channel activity mask */

/* Hermon basic control register (bctl) bit definition */

#define HERMON_SOFT_RESET 0x8000

#define MAX_TX_BFR_SIZE	(8*1024/IBM2692_PARTICLE_SIZE)+2

/* LAN Status defines */

#define SIGNAL_LOSS		0x8000
#define HARD_ERROR		0x4000
#define SOFT_ERROR		0x2000
#define TRANSMIT_BEACON		0x1000
#define LOBE_WIRE_FAULT		0x0800
#define AUTO_REMOVAL		0x0400
#define REMOVE_RECEIVED		0x0100
#define COUNTER_OVERFLOW	0x0080
#define SINGLE_STATION		0x0040
#define RING_RECOVERY		0x0020
#define SR_COUNTER_OVERFLOW	0x0010

/* System/MPC SRB commands */

#define	CLOSE_ADAPTER	        0x04    /* close adapter command */
#define CONFIG_BRIDGE 	        0x0c    /* configurate bridge command */
#define	CFG_HP_CHANNEL	        0x13
#define	MODIFY_BG_PARM	        0x15
#define MODIFY_OPEN_OPTION      0x01
#define MODIFY_RCV_OPTION       0x17
#define MODIFY_TB_OPTION        0x1a
#define NO_OP		        0x00
#define OPEN_ADAPTER	        0x03
#define READ_LOG	        0x08
#define	READ_SR_COUNT	        0x16
#define RESET_GROUP_ADDR        0x02
#define RESET_TARGET_SEG        0x14
#define SAVE_CFG	        0x1b
#define SET_BRDGE_PARM	        0x09
#define SET_FUNC_ADDR	        0x07
#define SET_GROUP_ADDR	        0x06
#define SET_TARGET_SEG	        0x05
#define TB_REJET_ADDR	        0x18
#define INIT_RESP               0x80    /* initialization response srb */

#define SIZE_OF_INIT_RESP	10	/* size of init response buffer */
#define BRIDGE_ACCESS_PRIORITY	0x80	/* priority of 4 */
#define MPC_INITIAL_STAT	0xfe
#define MPC_OK_STAT		0x0
#define PRODUCT_ID_LEN		18

/* modify receive option copy all options equate */

#define ENA_COPY_ALL_LLC	0x40
#define ENA_COPY_ALL_MAC	0x20
#define ENA_TB_COPY		0x08
#define ENA_COPY_ALL_SRB	0x01

/* following is the mpc micro code password to access copy all option */

#define MPC_PWD1		0x4a41
#define MPC_PWD2		0x4d45
#define MPC_PWD3		0x5320
#define MPC_PWD4		0x2020

/* System/MPC ARB Command */

#define	RECEIVE_DATA		0x81
#define	LAN_STATUS_CHANGE	0x84

#define ARB_SIZE		12

#define MAX_BRIDGE_PROTOS 32

typedef struct receive_data_arb_t {
    uchar	command;
    uchar	spare1[5];
    ushort	rcv_buff;
    uchar	header_len;
    uchar	spare2;
    ushort	frame_len;
} receive_data_arb;

typedef struct lan_status_change_arb_t {
    uchar	command;
    uchar	spare[5];
    ushort	status;
} lan_status_change_arb;

typedef union arb_ {
    receive_data_arb		receive_data;
    lan_status_change_arb	lan_status;
    ushort	data[6];
} arb_t;

typedef struct tx_bfr_ {
    uchar       *bptr;          /* data buffer pointer */
    ushort      space1;         /* place holder */
    ushort      b_count;        /* number of byte in buffer */
} tx_bfr_t;

typedef struct tx_bfr_desc_ {
    struct tx_bfr_desc_ *next;          /* transmit forward pointer */
    volatile ulong      tx_status;      /* transmit status */
    ushort              bfr_count;      /* number of tx buffer in frame */
    ushort              frame_length;         /* number of byte in frame */
    tx_bfr_t            bfr[MAX_TX_BFR_SIZE];  /* # of tx buffer per desc */
#if IBM2692_PARTICLE_SIZE == 512
    ulong               tx_bfr_filler;  /* for 512 particle, align to 16 b */
#else
    ErrorReadjustFillerForParticleSize  /* readjust tx_bfr_filler */
#endif
} tx_bfr_desc_t;

typedef struct rx_bfr_desc_ {
    struct rx_bfr_desc_ *next;          /* receive forward pointer */
    volatile ulong      rx_status;      /* receive frame status */
    uchar               *bptr;
    ushort              frame_length;   /* data byte count in frame */
    ushort              buf_length;     /* number of byte in receive buffer */
} rx_bfr_desc_t;

typedef struct rx_desc_shadow {
    struct rx_bfr_desc_ *pci_view_addr;	/* physical pci address of descriptor */
    struct rx_bfr_desc_ *cpu_view_addr;  /* uncached address of descriptor */
    particletype *particle;	/* pointer to particletype */
} rx_desc_shadow_t;

typedef struct tx_desc_shadow {
    struct tx_bfr_desc_ *pci_view_addr;
    struct tx_bfr_desc_ *cpu_view_addr;
    paktype *tx_pak;		/* store paktype pointer */
    particletype *particle[MAX_TX_BFR_SIZE]; /* store particle member */
    uchar        tb_saddr[IEEEBYTES];  /* copy of saddr for tb filter */
} tx_desc_shadow_t;

typedef struct init_buffer_ {
    uchar	data[512];
} ibm2692_init_buffer_t;

/* Hermon's PCI register definition */

typedef struct hermon_reg_map_t {
    volatile ulong     space0;   /* 0x00, space holder */
    volatile ushort    scbstat;	/* 0x06, scb status port register */
    volatile ushort    scbsig;	/* 0x04, scb signalling and control port reg */
    volatile ushort    blk1[4];  /* 0x08 - 0F, space holder */

    volatile ushort    lisr_sum;/* 0x12, set under mask register */
    volatile ushort    lisr;	/* 0x10, local interrupt status register */
    volatile ushort    sisr;	/* 0x16, system interrupt status register */
    volatile ushort    lisr_rum;/* 0x14, reset under mask register */
    volatile ushort    sisr_rum;/* 0x1A, reset under mask reg */
    volatile ushort    sisr_sum;/* 0x18, set under mask reg */
    volatile ushort    space1;	/* 0x1E, place holder */
    volatile ushort    led_fm;	/* 0x1C, led/flash memory control register */

    volatile ulong     rxstati;	/* 0x20, rcv channel status internal reg */
    volatile ulong     tx1stati;/* 0x24, tx chan 1 status internal reg */
    volatile ulong     tx2stati;/* 0x28, tx chan 2 status internal reg */
    volatile ulong     space2;	/* 0x2C - 2F */

    volatile ulong     blk3[4];  /* 0x30 - 3F */

    volatile ushort    blk4[6];	 /* 0x40 - 4B */
    volatile ushort    timer;    /* 0x4E, timer control register */
    volatile ushort    pad;      /* 0x4C, pad */

    volatile ushort    bmctl_rum;/* 0x52, bus master control reg (write only) */
    volatile ushort    bmctl_sum;/* 0x50, bus master control reg (read only) */
    volatile ushort    sisrm_sum;/* 0x56, sisr mask sum reg */
    volatile ushort    sisrm;	 /* 0x54, system interrupt status mask reg */
    volatile ushort    misr;     /* 0x5A, bus master interrupt status reg */
    volatile ushort    sisrm_rum;/* 0x58, sisr mask rum reg */
    volatile ushort    misrm_rum;/* 0x5E, bus master int status mask rum reg */
    volatile ushort    misrm_sum;/* 0x5C, bus master int status mask reg */

    volatile ushort    lapa;	/* 0x62, local access port address reg */
    volatile ushort    bctl;    /* 0x60, basic control reg */
    volatile ushort    lapd;	/* 0x66, local access port data reg */
    volatile ushort    lape;	/* 0x64, local access port extended addr reg */
    volatile ushort    lapwwo;	/* 0x6A, lap RAM write open reg */
    volatile ushort    lapd_inc;/* 0x68, local access port data Dinc reg */
    volatile ushort    lapctl;	/* 0x6E, local access port access control reg */
    volatile ushort    lapwwc;	/* 0x6C, lap RAM write close reg */

    volatile ulong     block5[3];/* 0x70 - 7B, space holder */
    ushort             cid;      /* 0x7E, chip id register */
    ushort             pad1;	 /* 0x7C */

    volatile ushort    pcidid;   /* 0x82 pci device id register */
    volatile ushort    pcivid;   /* 0x80 pci vendor id register */
    volatile ushort    pcimtr;   /* 0x86 pci minimum latency*/
    volatile ushort    pcirid;   /* 0x84 pci revision id reg */
    volatile ushort    pciscr;   /* 0x8a pci sub class reg */
    volatile ushort    pcimgr;   /* 0x88 pci minimum grant */
    volatile ushort    pct;      /* 0x8E, pci threshold control register */
    volatile ushort    space4;   /* 0x8C, place holder */

    volatile rx_bfr_desc_t     * volatile rxlbda;/* 0x90, rcv last bfr desc addr reg */
    volatile rx_bfr_desc_t     * volatile rxbda; /* 0x94; rcv bfr desc addr reg */    
    volatile ulong     rxstat;	/* 0x98, rcv channel status reg */
    volatile ulong     rxdba;	/* 0x9C, rcv channel data buffer addr reg */

    volatile tx_bfr_desc_t     * volatile tx1lfda;	/* 0xA0, tx 1 lfda reg */
    volatile tx_bfr_desc_t     * volatile tx1fda;	/* 0xA4, tx 1 fda reg */
    volatile ulong     tx1stat;	/* 0xA8, tx chan 1 status reg */
    volatile ulong     tx1dba;	/* 0xAC, tx chan 1 data bfr addr reg */
 
    volatile tx_bfr_desc_t     * volatile tx2lfda;/* 0xB0, tx 2 lfda reg */
    volatile tx_bfr_desc_t     * volatile tx2fda;	/* 0xB4, tx 2 fda reg */
    volatile ulong     tx2stat;	/* 0xB8, tx chan 2 status reg */
    volatile ulong     tx2dba;	/* 0xBC, tx chan 2 data bfr addr reg */
} hermon_reg_map;

/* Hermon transmit frame status equate */

#define   TX_PCPE	0x40000000	/* pci read error */
#define   TX_IPRE	0x20000000	/* internal parity error/read */
#define   TX_IWBE	0x10000000	/* internal parity error/write */
#define   TX_IABE	0x08000000	/* internal parity error/address */
#define   TX_FDP	0x01000000	/* tx frame descriptor processed */
#define   TX_WER	0x00800000	/* tx status write error */
#define   TX_EOC	0x00400000	/* tx end of chain */
#define   TX_TFUR	0x00020000	/* tx fifo underrun */
#define   TX_TPE	0x00010000	/* tx internal fifo parity error */

#define   TX_ERROR_MASK	0x78831800	/* tx error mask */

#define   CANCEL_FRAME	0x00001800	/* MPC tx status */

/* Hermon receive frame status equate */

#define   RX_EOF	0x80000000	/* end of frame */
#define   RX_PCPE	0x40000000	/* rx descriptor read error */
#define   RX_IPRE	0x20000000	/* internal parity error/read */
#define   RX_IWBE	0x10000000	/* internal parity error/write */
#define   RX_IABE	0x08000000	/* internal parity error/address */
#define   RX_FPE	0x04000000	/* ICD fifo parity error */
#define   RX_NRBDA	0x02000000	/* no buffer available */
#define   RX_BDP	0x01000000	/* rx buffer descriptor processed */
#define   RX_WER	0x00800000	/* PCI bus write error */
#define   RX_GMAT	0x00400000	/* group match */
#define   RX_IMAT	0x00200000	/* individual address match */
#define   RX_FMAT	0x00100000	/* functional address match */
#define   RX_RIBC	0x00080000	/* RI broadcast */
#define   RX_NRIBC	0x00040000	/* non-RI broadcast */
#define   RX_RFOR	0x00020000	/* Rx fifo overrun */
#define   RX_RPE	0x00010000	/* Rx parity error */

#define   RX_IMP_ABORTED_FRAME  0x00000400  /* Received  implicit aborted frame */
#define   RX_EXP_ABORTED_FRAME  0x00000600  /* Received  explicit aborted frame */
#define   RX_ABORT_MASK         0x00000600  /* Abort mask */

#define   RX_MPC_ERROR  0x00001800      /* Rx MPC errors */

#define   RX_ERROR_MASK	0x7e831c00

/* hermon timer register bit definition */

#define TIMER_ENABLE  0x2000
#define MS_500                0x32
#define MS_20                 0x02

/* MPC receive status bit 0 - 15 of receive frame status */

#define DEST_ADDR_MATCH		0x01	/* destination address match */
#define ROUTE_ADDR_MATCH	0x02	/* source route address match */
#define	TRANB_ADDR_MATCH	0x04	/* transparent bridge address match */
#define ADDR_MATCH_MASK		0x0f	/* address match status mask */
#define RXSTAT_MAC		0x10	/* trip style mac indication */

#define FUNCTIONAL_ADDR		0xc0	/* functional address mask */
#define FRAME_TYPE_MASK		0xc0	/* type of frame, llc or mac */
#define RI_INDICATION		0x80	/* if there is routing info */
#define BRIDGE_FUNCTIONAL	0x0100	/* bridge functional address */

/* Address filter output bit definition */

#define STATION_TYPE	0x100
#define MULTICAST_TYPE	0x200
#define BRIDGING_TYPE	0x400
#define MAC_TYPE	0x800

#define DOD_IP_TYPE	0x1000
#define DECNET_TYPE	0x2000
#define XNS_TYPE	0x4000
#define CLNS_TYPE	0x8000

#define CLASSIFY_MASK   0xff00

#define TYPE_IP10MB	0x0800
#define TYPE_RFC826_ARP	0x0806
#define TYPE_DECNET	0x6003
#define TYPE_XNS	0x0600

/* open adapter srb command option equate */

#define EXPLICIT_CONFIG_PARM	0x08
#define RJ45			0x02
#define RS16			0x01

#define EXTERNAL_WRAP		0x8000
#define NO_HARD_ERROR		0x4000
#define NO_SOFT_ERROR		0x2000
#define APAPTER_MAC		0x1000
#define ATTENTION_MAC		0x0800
#define SR_COUNT_4		0x0400
#define CONTENDER		0x0100
#define BEACON_MAC		0x0080
#define RPL			0x0020
#define ETR			0x0010
#define INTERNAL_WRAP		0x0008

#define NO_TX_LLC_COPY		0x80
#define NO_TX_RI_COPY		0x40
#define NO_DA_LLC_COPY		0x20
#define DA_MAC_COPY		0x10
#define ALL_NODE_MAC_COPY	0x08

#define DISABLE_ALL_RCV_OPTION	0xe0
#define ENABLE_LLC_COPY         0x40
#define DISABLE_ASA_LLC_COPY    0x01    /* this bit is not documented */

/* local port access control word */

#define   MPC_RAM	0x0000	/* MPC RAM access */
#define   MPC_ROM	0x0001	/* MPC ROM access */
#define   MPC_REG	0x0003	/* MPC register access */


#define INIT_RESP_SIZE	20

struct init_resp {
    uchar	command;
    uchar	status_low;
    uchar	status_high;
    uchar	spare[3];
    ushort	bring_up_code;
    ushort	uaa_addr;
    ushort      ibm_ucode_level_addr;
    ushort      addr_table_addr;
    ushort	parms_addr;
    ushort	mac_bfr_addr;
    ushort	ru_count_addr;
};

typedef union init_resp_ {
    struct init_resp   init_result;
    ushort	data[10];
} init_resp_t;

#define ADDR_TABLE_SIZE 14

struct addr_table {
    uchar	node_addr[6];
    uchar	spare[4];
    uchar	functional_addr[4];
};

typedef union adr_tbl_ {
    struct addr_table	addr_tbl;
    ushort	data[7];
} adr_tbl_t; 

#define UAA_ADDR_SIZE	6

struct uaa_addr {
    ushort	UAA_HIGH;
    ushort	UAA_MIDDLE;
    ushort	UAA_LOW;
};

typedef union ua_adr_ {
    struct uaa_addr    mac_address;
    ushort      data[3];
} ua_adr_t;


#define PARMS_TABLE_SIZE	68

struct parms_table {
    uchar	phys_addr[4];
    uchar	up_node_addr[6];
    uchar	up_phys_addr[4];
    uchar	poll_addr[6];
    uchar	spare1[2];
    uchar	acc_priority[2];
    uchar	auth_source_class[2];
    uchar	att_code[2];
    uchar	source_addr[6];
    uchar	beacon_type[2];
    uchar	major_vector[2];
    uchar	lan_status[2];
    uchar	soft_error_time[2];
    uchar	spare2[2];
    uchar	local_ring[2];
    uchar	mon_error[2];
    uchar	beacon_transmit[2];
    uchar	beacon_receive[2];
    uchar	frame_correl[2];
    uchar	beacon_naun[6];
    uchar	spare3[4];
    uchar	beacon_phys[4];
};

typedef union parms_tbl_ {
    struct parms_table parm_tbl;
    ushort	data[34];
} parms_tbl_t;

#define UCODE_LEVEL_SIZE	10

struct ucode_level {
    uchar	level_string[10];
};

typedef union ucode_level_ {
    struct ucode_level	name_string;
    ushort	data[5];
} ucode_level_t;

/* different types of srbs used by MPC */

/* close adapter srb, length 3 byte */

#define CLOSE_ADAPTER_SIZE	4

typedef struct close_adapter_ {
    uchar	command;
    uchar	spare;
    uchar	ret_code;
} close_adapter_t;

/* configure bridge srb, length 5 byte */

#define CONFIGURE_BRIDGE_SIZE	6

typedef struct configure_bridge_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2;
    uchar	access_priority;
} configure_bridge_t;

/* configure high priority transmit channel srb, length 5 byte */

#define CONFIGURE_HP_CHANNEL_SIZE	6

typedef struct configure_hp_channel_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2;
    uchar	access_priority;
} configure_hp_channel_t;

/* modify bridge parameter srb, length 16 byte */

#define MODIFY_BRIDGE_PARMS_SIZE	16

typedef struct modify_bridge_parms_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2;
    uchar	limited_broadcast;
    uchar	spare3;
    uchar	inhibit_sr_copy;
    uchar	spare4[9];
    uchar	on_board_rps;
} modify_bridge_parms_t;

/* modify open option srb, length 6 byte */

#define MODIFY_OPEN_OPTIONS_SIZE	6

typedef struct modify_open_options_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2;
    uchar	open_options[2];
} modify_open_options_t;

/* modify receive option srb, length 14 byte */

#define MODIFY_RECEIVE_OPTIONS_SIZE	14

typedef struct modify_receive_options_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2;
    uchar	receive_options;
    uchar	copy_all_options;
    ushort	password[4];
} modify_receive_options_t;

/* modify transparent bridge options srb, length 5 byte */

#define MODIFY_TB_OPTIONS_SIZE	6

typedef struct modify_tb_options_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2;
    uchar	enable_tb_copy;
} modify_tb_options_t;

/* no operation srb, length 3 byte */

#define	NO_OPERATION_SIZE	4

typedef struct no_operation_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
} no_operation_t;

/* open adapter srb, length 50 byte */

#define OPEN_ADAPTER_SIZE	50

typedef struct open_adapter_ {
    uchar	command;
    uchar	spare1[6];
    uchar	ext_open_options;
    ushort	open_options;
    uchar	spare2[2];
    uchar	node_address[6];
    uchar	group_address[6];
    ulong	functional_address;
    ushort	trb_buf_length;
    uchar	receive_options;
    uchar	number_local_addr;
    uchar	product_id[18];
} open_adapter_t;

/* open adapter response parameters, length 19 byte */

#define OPEN_RESP_SIZE	20

typedef struct open_resp_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2[3];
    ushort	open_error_code;
    ushort	asb_addr;
    ushort	srb_addr;
    ushort	arb_addr;
    uchar	spare3[2];
    ushort	trb_addr;
    uchar	configuration;
} open_resp_t;

/* read error log srb, length 17 byte */

#define	READ_LOG_SIZE	18

typedef struct read_log_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2[3];
    uchar	line_error;
    uchar	internal_error;
    uchar	burst_error;
    uchar	ari_fci_error;
    uchar	abort_error;
    uchar	space1;
    uchar	lost_frame_error;
    uchar	receive_congestion;
    uchar	frame_copy_error;
    uchar	space2;
    uchar	token_error;
} read_log_t;

/* read source route bridge related counters srb, length 20 byte */

#define READ_SR_COUNTERS_SIZE	20

typedef struct read_sr_counters_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2;
    uchar	nrb[4];
    uchar	srcntr[4];
    uchar	svcntr[4];
    uchar	sicntr[4];
} read_sr_counters_t;

/* reset group address srb, length 15 byte */

#define RESET_GROUP_ADDRESS_SIZE	16

typedef struct reset_group_address_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2[3];
    uchar	group_addr[6];
    ushort	num_address;
    uchar	group_type;
} reset_group_address_t;

/* reset target segment srb, lenth 6 byte */

#define RESET_TARGET_SEGMENT_SIZE	6

typedef struct reset_target_segment_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2;
    uchar	target_ring[2];
} reset_target_segment_t;

/* save configuration srb, length 7 byte */

#define SAVE_CONFIGURATION_SIZE	8

typedef struct save_configuration_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2;
    uchar	password;
    uchar	spare3;
    uchar	pci_rom_disable;
} save_configuration_t;

/* set bridge parameters srb, length 16 byte */

#define SET_BRIDGE_PARMS_SIZE	16
#define ENA_LIMITED_BROADCAST	1

typedef struct set_bridge_parms_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2[3];
    ushort	source_ring;
    ushort	target_ring;
    ushort	bridge_number;
    uchar	multi_port_bridge;
    uchar	limited_broadcast;
    uchar	inhibit_sr_copy;
    uchar	on_board_rps;
} set_bridge_parms_t;

/* set functional address srb, length 10 byte */

#define SET_FUNCTIONAL_ADDRESS_SIZE	10

typedef struct set_functional_address_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2[3];
    uchar	functional_address[4];
} set_functional_address_t;

/* set group address srb, length 17 byte */

#define SET_GROUP_ADDRESS_SIZE	18

typedef struct set_group_address_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2[3];
    ushort	group_addr_01;
    ulong	group_addr_2345;
    ushort	num_address;
    uchar	group_type;
    uchar	spare3;
    uchar	location;
} set_group_address_t;

/* set target segment srb, length 8 byte */

#define SET_TARGET_SEGMENT_SIZE	8

typedef struct set_target_segment_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2;
    uchar	target_ring[2];
    uchar	all_routes_exp;
    uchar	span_tree_exp;
} set_target_segment_t;

/* transparent bridge reject address srb, length 17 byte */

#define TB_REJECT_ADDRESS_SIZE	18

#define SET_ONE		0x0
#define RESET_ONE	0x01
#define RESET_ALL	0x02

typedef struct tb_reject_address_ {
    uchar	command;
    uchar	spare1;
    uchar	ret_code;
    uchar	spare2;
    uchar	tb_reject_addr[6];
    uchar	set_reset_flag;
    uchar	spare3[5];
    uchar	location;
} tb_reject_address_t;

typedef struct trb_hdr_ {
    ushort	spare1;
    uchar	ret_code;
    uchar	spare2[4];
    uchar	tx_error;
    ushort	length;
} trb_hdr_t; 

typedef union trb_ {
    trb_hdr_t	trb;
    ushort	data[5];
} trb_t;
 
#define TRB_HDR_LEN	10

/* shadow image of the srbs in shared memory, length 60 byte */

typedef struct rp_srb_ {
    ushort	data[30];
} rp_srb_t;

typedef union srb_command_block_ {
    close_adapter_t		close_adapter;
    configure_bridge_t		configure_bridge;
    configure_hp_channel_t	configure_hp_channel;
    modify_bridge_parms_t	modify_bridge_parms;
    modify_open_options_t	modify_open_options;
    modify_receive_options_t	modify_receive_options;
    modify_tb_options_t		modify_tb_option;
    no_operation_t		nop;
    open_adapter_t		open_adapter;
    read_log_t			read_log;
    read_sr_counters_t		read_sr_counter;
    reset_group_address_t	reset_group_address;
    reset_target_segment_t	reset_target_segment;
    save_configuration_t	save_configuration;
    set_bridge_parms_t		set_bridge_parms;
    set_functional_address_t	set_functional_address;
    set_group_address_t		set_group_address;
    set_target_segment_t	set_target_segment;
    tb_reject_address_t		tb_reject_address;
    open_resp_t			open_response;
    rp_srb_t			srb_shadow;
} srb_cmd_block_t;

#define ASB_AVAILABLE	0xff
#define ILLEGAL_CMD	0x01
#define ILLEGAL_ADDR	0x26

#define MPC_ASB_SIZE	8
#define ASB_QUEUE_SIZE	8
#define ASB_INDEX_MASK	ASB_QUEUE_SIZE - 1

#define MAX_SRB_QUEUE_SIZE    4
#define MAX_SRB_QUEUE_MASK MAX_SRB_QUEUE_SIZE - 1

typedef struct mpc_asb {
    uchar	command;	/* asb command code */
    uchar	spare1;
    uchar	ret_code;	/* asb command return code */
    uchar	spare[3];
    ushort	rcv_bfr;	/* address of receive buffer */
} mpc_asb_t;

typedef struct mac_ {
    ushort	next_bfr;
    ushort	status;
    ushort	length;
} mac_hdr_t;

#define ASB_MAC_HDR_SIZE  6      

typedef struct mac_addr {
    uchar	addr[6];
} mac_addr_t;

/* token ring driver variables per port */

typedef struct ibm2692_instance {

    pas_cbtype 		pacb;		/* universal pa device control */
    hermon_reg_map	*ibm2692_ptr;	/* PCI base address of hermon/mpc */

    hwidbtype		*hwidb;
    paktype 		*rx_pak;

/* interrupt service routine variables */

    ushort		hermon_sisr, hermon_misr;

/* srb, arb, trb, asb, address table, parameter table, shadows for MPC */

    init_resp_t     mpc_init_response;
    adr_tbl_t       mpc_address_table;
    adr_tbl_t       mpc_addr_table;
    ua_adr_t        mpc_uaa_addr;
    parms_tbl_t     mpc_parms_table;
    ucode_level_t   mpc_ucode_level;
    ushort	    mpc_utilization;

    srb_cmd_block_t mpc_srb;
    arb_t           mpc_arb;

/* local control flags for MPC shared memory command access */

    boolean		mpc_init_flag;
    boolean		srb_in_use;
    boolean		trb_in_use;
    boolean		bridge_mode;
    boolean		tb_mode;
    boolean		tx_in_progress;

/* address of shared memory control blocks */

    ushort		srb_loc;
    ushort		trb_loc;
    ushort		arb_loc;
    ushort		asb_loc;
    ushort		adp_chk_addr;
    ushort		adp_chk_data[4];	/* adapter check data */
    ushort		lan_status;
    ushort		mac_bfr_addr;
    ushort		mac_bfr_header_len;
    ushort		mac_bfr_length;
    ushort		mpc_lapwwc;

/* transmit/receive buffer descriptors used by Hermon as PCI bus master */

    int			rx_start;
    int			tx_inptr, tx_outptr;
    int			tx_outstanding, tx_hw_queue_count;
    int			tx_hw_queue_head, tx_hw_queue_tail;

/* debug rx & tx channel disabled count */

    int			rx_chan_disabled;
    int                 tx_chan_disabled;
    
/* pointers to malloc'ed memory that are free'd after OIR */

    tx_bfr_desc_t       *txr_malloc;
    rx_bfr_desc_t       *rxr_malloc;

/* local book keeping shadow data structure of descriptors */

    rx_desc_shadow_t		rx_desc_shadow[IBM2692_RX_RING_ENTRIES];
    tx_desc_shadow_t		tx_desc_shadow[IBM2692_TX_RING_ENTRIES];

/* rx buffer descriptor and mini buffer used for MPC init */

    rx_bfr_desc_t	*mpc_init_rxdesc;
    uchar		*mpc_init_bptr;
    volatile rx_bfr_desc_t *queued_rx_desc;
    volatile rx_bfr_desc_t *ready_rx_desc;

/* mpc asb queue and control */

    int			asb_inptr;
    int			asb_outptr;
    int			asb_queue_count;
    mpc_asb_t		sys_asb[ASB_QUEUE_SIZE];
    
/* mpc srb queue and control */

    int			srb_inptr;
    int			srb_outptr;
    int			srb_queue_count;
    srb_cmd_block_t	srb_queue[MAX_SRB_QUEUE_SIZE];
 
/* MPC error log */

    mpc_errors mpc_err;

/* flag to indicate that flush packet is active */

    boolean	flush_pkt;

/* configuration data */

   boolean		cfg_in_progress;
   int			cfg_write_count;
   ushort		*cfg_write_ptr;
   ibmtr_cfg_data_t	config_data;

/* TX errors from TX descriptor */

    tx_errors tx_err;

    ulong     tx_pci_stat_fail;

/* RX error from descriptors */

    rx_errors rx_err;

/* broad cast packet filter */

    int		rt_index;
    mac_addr_t	reject[REJECT_TBL_SIZE];

/* Throttle statistics */

    ulong throttled;            /* # times we have been throttled */
    ulong enabled;              /* # times we have been enabled */

    ushort   snapcodes[MAX_BRIDGE_PROTOS+1];
    ushort   llc1codes[MAX_BRIDGE_PROTOS+1];
    ushort   llc2codes[MAX_BRIDGE_PROTOS+1];
    boolean  tx2_chnl_disabled;  /* tx-chnl disabled due to error */

/* Temporary save area for the token ring FCS */

    ulong fcs_save;
  
} ibm2692_instance_t;

/*****************************************************************
 * Static inlines
 *****************************************************************/

/*
 * in_reject_list - check if the sa is in reject table
 */
static inline boolean in_reject_list (hwidbtype *idb, uchar *ptr)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    int i;

    for (i = 0; i < REJECT_TBL_SIZE; i++) {
	if (ieee_equal_sans_rii(&ds->reject[i].addr[0], ptr)) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * add_reject_addr - add an entry to reject source address table
 */
static inline void add_reject_addr (hwidbtype *idb, uchar *ptr)
{
    ibm2692_instance_t *ds = INSTANCE(idb);

    ieee_copy(ptr, &ds->reject[ds->rt_index].addr[0]);
    /*
     * advance the index to next available entry
     */
    ds->rt_index++;
    ds->rt_index &= REJECT_INDEX_MASK;
}

/*
 * ibm2692_queue_tx_desc
 */
static inline void ibm2692_queue_tx_desc (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    hermon_reg_map *hermon_ptr = ds->ibm2692_ptr;

    if (!ds->tx_in_progress) {
	tx_desc_shadow_t *tx_desc = &ds->tx_desc_shadow[ds->tx_inptr];
	if (idb->tr_ring_mode & RINGMODE_TBRIDGE) {
	    add_reject_addr(idb, tx_desc->tb_saddr);
	}
	hermon_ptr->tx2lfda = tx_desc->pci_view_addr;
	ds->tx_in_progress = TRUE;
    } else {
	if (!ds->tx_hw_queue_count) {
	    ds->tx_hw_queue_head = ds->tx_hw_queue_tail = ds->tx_inptr;
	} else {
	    ds->tx_hw_queue_tail = ds->tx_inptr;
	}
	ds->tx_hw_queue_count++;
    }
    ds->tx_inptr++;
    ds->tx_inptr &= IBM2692_TX_RING_INDEX_MASK;
    ds->tx_outstanding++;
}


/*
 * ibm2692_flush_pak
 */
static inline void ibm2692_flush_pak (paktype *pak)
{
    pak->datagramsize = 0;
    pak->datagramstart = NULL;
    pak->particlequeue.count = 0;
    pak->particlequeue.qhead = NULL;
    pak->particlequeue.qtail = NULL;
}

/*
 * ibm2692_cleanup_pak
 */
static inline void ibm2692_cleanup_pak (paktype *pak)
{
    particle_retbuffer(pak);
    ibm2692_flush_pak(pak);
}

/*
 * ibm2692_output_enqueue
 */
static inline void ibm2692_output_enqueue (paktype *pak, hwidbtype *idb)
{
    paktype *newpak;

    if (pak_has_particles(pak)) {
	newpak = particle_reparent(pak, idb->headerpool);
	if (!newpak) {
	    ibm2692_cleanup_pak(pak);
	    idb->counters.output_total_drops++;
	    return;
	}
	if (idb->priority_list) {
	    pas_stuff_pak_for_holdq(newpak, pak);
	}
    } else {
	newpak = pak;
    }
    /*
     * Invalidate the accounting protocol type to prevent 
     * packet accounting in holdq_enqueue_fast() if accounting
     * flag is not set.
     */
    if (!IBM2692_PKT_STATS)
        newpak->acct_proto = ACCT_PROTO_LAST;
    holdq_enqueue_fast(idb, newpak);
}


/*
 * ibm2692_return_buffer
 */
static inline void ibm2692_return_buffer (ibm2692_instance_t *ds)
{
    hermon_reg_map *hermon_ptr = ds->ibm2692_ptr;

    if (ds->queued_rx_desc) {
	hermon_ptr->rxlbda = ds->queued_rx_desc;
    } 
    ds->queued_rx_desc = ds->rx_desc_shadow[ds->rx_start].pci_view_addr;
    ds->rx_start++;
    ds->rx_start &= IBM2692_RX_RING_INDEX_MASK;
}


/*
 * ibm2692_rx_error_log
 */
static inline void ibm2692_rx_error_log (ulong stat, ibm2692_instance_t *ds)
{
    rx_errors *rx;

    rx = &ds->rx_err;
    if (stat & RX_NRBDA)
	rx->rx_no_data_bfr++;
    if (stat & RX_PCPE)
	rx->rx_pci_read_parity_error++;
    if (stat & RX_IPRE)
	rx->rx_int_read_parity_error++;
    if (stat & RX_IWBE)
	rx->rx_int_write_parity_error++;
    if (stat & RX_IABE)
	rx->rx_int_pci_address_bfr++;
    if (stat & RX_FPE)
	rx->rx_fifo_parity_error++;
    if (stat & RX_WER)
	rx->rx_pci_write_error++;
    if (stat & RX_RFOR)
	rx->rx_fifo_overrun++;
    if (stat & RX_RPE)
	rx->rx_parity_error++;
    if (stat & RX_MPC_ERROR)
	rx->rx_mpc_error++;
    if ((stat & RX_ABORT_MASK) == RX_IMP_ABORTED_FRAME)
	rx->rx_imp_abrt_frm++;
    if ((stat & RX_ABORT_MASK) == RX_EXP_ABORTED_FRAME)
	rx->rx_exp_abrt_frm++;
}

/*
 * ibm2692_get_particle_inline
 */
static inline particletype *ibm2692_get_particle_inline (hwidbtype *idb,
							 boolean cache_ok)
{
    particletype *mp = NULL;

    /*
     * If cache_ok, try the buffer cache. This check will compile out.
     */
    if (cache_ok) {
        mp = pool_dequeue_cache(idb->pool);
     }

    /*
     * If we did not get one from the cache, try a global pool
     */
    if (!mp) {
	mp = pool_getparticle(idb->pool->fallback);
    }

    /*
     * If we still do not have one, mark it as a nobuffer.
     */
    if (mp) {
    /*
     * Initialize start
     */
    mp->data_start = particle_center(mp);
    return(mp);
    } else {
        idb->counters.output_nobuffers++;
        return(NULL);
    }
}


/*
 * Function prototypes
 */

/* if_pas_ibm2692.c */

extern void            ibm2692_reset(hwidbtype *);
extern void            ibm2692_tx_start(hwidbtype *);
extern void            ibm2692_safe_start(hwidbtype *);
extern void            get_init_resp(hwidbtype *);
extern void            do_tx_accounting(ulong, ibm2692_instance_t *); 
extern void            config_bridge(hwidbtype *);
extern void            do_mpc_open(hwidbtype *);
extern void            read_sreqb_response(hwidbtype *);
extern char            *ibm2692_version_string(uchar *);
extern void            set_ibm2692_functional_address(hwidbtype *);
extern void            ibm2692_throttle(hwidbtype *);
extern void            ibm2692_cache_threshold(pooltype *);

/* if_ [platform] _ibm2692.c */

extern hwidbtype       *ibm2692_idb_create(uint, uint);
extern void            ibm2692_deallocate(hwidbtype *);
extern void            ibm2692_show_instance(ibm2692_instance_t *);
extern void            ibm2692_snmp_link_trap(int, hwidbtype *);
extern void            ibm2692_lineproto_change(hwidbtype *, boolean);
extern void            ibm2692_get_default_hw_address(hwidbtype *, uchar *);
extern boolean         ibm2692_init_buffer_pools(hwidbtype *);
extern boolean         ibm2692_platform_init(hwidbtype *);
extern void            ibm2692_process_receive_packet(hwidbtype *, ulong, 
						      paktype *); 
extern void            ibm2692_process_mac_frame(hwidbtype *, paktype *);
extern void            ibm2692_adp_chk(hwidbtype *);
extern void            ibm2692_init_fail(hwidbtype *, ushort);
extern void            ibm2692_lan_status_change(hwidbtype *, ushort);
extern void            ibm2692_open_fail(hwidbtype *);
extern void            ibm2692_open_okay(hwidbtype *);
extern void            ibm2692_cfg_brg_fail(hwidbtype *);
extern void            ibm2692_lap_read(hermon_reg_map *, ushort,
					ushort, ushort *, ushort);
extern void            ibm2692_lap_write(hermon_reg_map *, ushort,
					 ushort, ushort *, ushort);
extern void            ibm2692_modify_srb(hwidbtype *, ulong, ulong);
extern void            ibm2692_set_bridge_filters(hwidbtype *);
extern boolean         ibm2692_set_addresses(hwidbtype *);
extern void            ibm2692_get_smt_stats(hwidbtype *);
extern void            ibm2692_add_counters(hwidbtype *);
extern boolean         ibm2692_rev_okay(int);

/* if_isr_ibm2692.c */

extern void            ibm2692_fastsend(hwidbtype *, paktype *);
extern void            hermon_sreqb_command(hwidbtype *, srb_cmd_block_t *);

/* asm wrapper file */

extern pa_wrapper_t    ibm2692_eim_wrapper;

/*
 * Include platform dependant header file again to pick up static 
 * inline definitions. Note, this is the second inclusion of these files.
 */

#define IBM2692_STATIC_INLINES_ONLY

#if defined(RVIP) || defined(SVIP)
#include "if_vip_ibm2692.h"
#endif

#if defined(PREDATOR)
#include "if_c7100_ibm2692.h"
#endif

#undef IBM2692_STATIC_INLINES_ONLY

/* end of file */
