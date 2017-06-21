/* $Id: c4000_atmzr.h,v 3.6.22.4 1996/09/04 23:59:43 hxu Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/c4000_atmzr.h,v $
 *------------------------------------------------------------------
 * C4000 router family ATMizer definitions.
 *
 * October, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c4000_atmzr.h,v $
 * Revision 3.6.22.4  1996/09/04  23:59:43  hxu
 * CSCdi58134:  ATM: 4500 hangs after atmzr_hi_irq_hand(ATM0): Secondary
 * port error
 * Branch: California_branch
 * Reset the interface when the unexpected interrupt takes place
 *
 * Revision 3.6.22.3  1996/05/31  15:31:03  rzagst
 * CSCdi54940:  ATM driver creates low-bit-rate SVCs
 * Branch: California_branch
 *
 * Revision 3.6.22.2  1996/05/13  18:41:23  rzagst
 * CSCdi57247:  Burst size handled improperly between AIP and 4500
 * Branch: California_branch
 *
 * Revision 3.6.22.1  1996/04/25  23:22:24  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.6.26.2  1996/04/10  18:28:32  rzagst
 * ATM Modularity: Add () to #define per code review action item.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.6.26.1  1996/03/30  02:18:56  rzagst
 * ATM Modularity: Clean up and modularize by platform ATM Commands.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.6  1996/01/25  01:03:21  hxu
 * CSCdi46323:  atmzr shows wrong VCCs associated w/ a rateq
 * An active VC is expected to be associated with a non-zero value of
 * vcmode.
 * A new constant ATMZR_AIP_HACK is created to guarantee this.
 *
 * Revision 3.5  1996/01/24  21:42:30  mohsen
 * CSCdi46503:  atm nim transmit hung sometimes
 * Have the driver clear the command queue entries after they are
 * processed by the firmware. Leftover entries put the firmware in
 * an infinite command processing loop which results in driver
 * reseting the interface.
 *
 * Revision 3.4  1995/12/11  01:48:27  mohsen
 * CSCdi38494:  traffic exceeds peak rate on the ATM NMP/4500
 * When openeing rate queues explicitly via atm rate queue command,
 * select the clock source properly. Also fix the arithmetic.
 *
 * Revision 3.3  1995/11/23  19:58:16  bbenson
 * CSCdi42518:  vc creation fails because of wrong max_vpibits in ILMI
 * Fixes bits per VPI/VCI reported by the ILMI.
 *
 * Revision 3.2  1995/11/17  18:44:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:27:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/30  00:59:37  wfried
 * CSCdi39530:  Need to limit the number of ATM interfaces per box
 * Limit number of ATM interfaces per type (only 1 ATM OC-3)
 *
 * Revision 2.4  1995/07/18  06:48:37  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.3  1995/07/06 01:14:44  wfried
 * CSCdi36736:  Interface reset causes infinite recursion
 * Prevent transmission of packets on closed VCs and update interface
 * control data structures before calling media registry functions.
 *
 * Revision 2.2  1995/06/27  00:30:58  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.1  1995/06/07  22:56:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__C4000_ATMZR_H__
#define	__C4000_ATMZR_H__



/*******************************************************************
		Include Files
 *******************************************************************/

#include "c4000_atmzr_if.h"


/*******************************************************************
		Basic Data Types
 *******************************************************************/

typedef void				(*inthand_t)(hwidbtype *);


/*******************************************************************
		Global Definitions
 *******************************************************************/

#define PKTDESC_THRESHHOLD      20              /* packet descriptor threshhold */
#define MAX_ATMZR_OC3_NIMS	1		/* Max # OC-3 NIMs/box */
#define CELL_BITS               (48 * 8)        /* number of data bits per cell */
#define ATMZR_MIN_BURST_SIZE    1               /* one cell is min burst */
#define ATMZR_MAX_BURST_SIZE    (MAXUSHORT)     /* 65535 is max burst */

#define ATMZR_BAD_RATEQ		ATMZR_RATEQ_NO  /* Bad rate queue ID error */
#define ATMZR_CONFIG_DELAY	2		/* Interf config reset delay */
#define ATM_RAWQ_DEFAULT	32		/* Raw buffer queue size dflt */

#define	ATMZR_CLOCK		50		/* ATMizer clock freq (MHz) */
#define	ATMZR_SHMEMMAX		(2*LARGEDATA*RXPKTDLIMIT)
						/* Max usable sh mem (bytes) */
#define	ATMZR_PLIMRATEMAX	LITE_OC3_LINERATE/* Max sup line rate (kbps) */
#define	ATMZR_PRUMAX		4095		/* ATMizer PRU ctr max value */
#define ATMZR_PRU_1U_RATE	(100000/128)	/* PRU rate 1/1.28us = 781kHz */
#define ATMZR_PRU_2U_RATE	(100000/256)	/* PRU rate 1/2.56us = 391kHz */
#define ATMZR_PRU_5U_RATE	(100000/512)	/* PRU rate 1/5.12us = 195kHz */
#define ATMZR_PRU_10U_RATE	(100000/1024)	/* PRU rate 1/10.24us = 98kHz */
#define	ATMZR_RSPTOUT		3		/* ATMizer rsp tout (ms) */
#define	ATMZR_HOLDQ		40		/* ATMizer output hold queue */

#define ATMZR_PRUCLK_INT	0		/* PRU ref clk = internal */
#define ATMZR_PRUCLK_EXT	1		/* PRU ref clk = external */

#define ATMZR_DELAY		10		/* Default IGRP delay */
#define ATMZR_SONET_DELAY       8		/* IGRP delay for SONET */
#define ATMZR_4B5B_DELAY        10		/* IGRP delay for TAXI */
#define ATMZR_E3_DELAY          20		/* IGRP delay for E3 */
#define ATMZR_DS3_DELAY         19		/* IGRP delay for DS3*/
#define ATMZR_DS3_FRAMING_DEFAULT ATM_FRAMING_DS3CBITADM
#define ATMZR_E3_FRAMING_DEFAULT  ATM_FRAMING_E3G751PLCP

#define ATMZR_VISIBLE_BANDWIDTH  100000
#define ATMZR_VISIBLE_BANDWIDTH_SONET 156250
#define ATMZR_VISIBLE_BANDWIDTH_4B5B  100000
#define ATMZR_VISIBLE_BANDWIDTH_E3    34013
#define ATMZR_VISIBLE_BANDWIDTH_DS3   45000

#define ATMZR_MAX_VPIBITS	3		/* Default max VPI bits */
#define ATMZR_MAX_VCIBITS	10		/* Default max VCI bits */
#define ATMZR_TOTAL_VPIVCI_BITS (ATMZR_MAX_VPIBITS + ATMZR_MAX_VCIBITS)
#define ATMZR_MIN_PEAK_RATE     56              /* Default peak cell rate */
#define ATMZR_MIN_AVG_RATE      1


/*
 * An active VC is expected to be associated with a non-zero
 * value of vcmode; 
 * ATMZR_AIP_HACK guarantees non-zero vcmode; 
 * This hacking should be removed in California code moduration
 */
#define ATMZR_AIP_HACK          1


/*******************************************************************
		Memory Map Definitions
 *******************************************************************/

typedef struct regs_block_db_t_ {
    char		*reg_name;		/* Register name */
    ulong		reg_offset;		/* Register offset */
} regs_block_db_t;

#include "c4000_atmzr_sunilite.h"		/* PMC Sierra's SUNI-LITE */
#include "c4000_atmzr_sunipdh.h"		/* PMC Sierra's SUNI-PDH */

/*
 * ATMizer NIM registers definitions
 */
/* PC_ID Register (0x0000) */
#define ATMZR_PCIDR_MSK		0x00FF		/* NIM ID field mask */

/* PC_REV Register (0x0002) */
#define ATMZR_PCREVR_MSK	0x00FF		/* NIM revision field mask */

/* MEM_CONFIG Register (0x0004) */
#define ATMZR_MCFGR_PUSRAM_8K	0x0000		/* Public SRAM size = 8kB */
#define ATMZR_MCFGR_PUSRAM_16K	0x0001		/* Public SRAM size = 16kB */
#define ATMZR_MCFGR_PUSRAM_32K	0x0002		/* Public SRAM size = 32kB */
#define ATMZR_MCFGR_PUSRAM_64K	0x0003		/* Public SRAM size = 64kB */
#define ATMZR_MCFGR_PUSRAM_MSK	0x000F		/* Public SRAM size mask */
#define ATMZR_MCFGR_PRSRAM_32K	0x0000		/* Private SRAM size = 32kB */
#define ATMZR_MCFGR_PRSRAM_128K	0x0010		/* Private SRAM size = 128kB */
#define ATMZR_MCFGR_PRSRAM_512K	0x0020		/* Private SRAM size = 512kB */
#define ATMZR_MCFGR_PRSRAM_2M	0x0030		/* Private SRAM size = 2MB */
#define ATMZR_MCFGR_PRSRAM_MSK	0x00F0		/* Private SRAM size mask */

/* PLIM_ID_REV Register (0x0006) */
#define ATMZR_PLIMR_ID_MSK	0x00FF		/* PLIM ID field mask */
#define ATMZR_PLIMR_ID_STS3CSMF	0x0001		/* STS-3c single mode fiber */
#define ATMZR_PLIMR_ID_STS3CMMF	0x0002		/* STS-3c multiple mode fiber */
#define ATMZR_PLIMR_ID_DS3	0x00D3		/* DS-3 */
#define ATMZR_PLIMR_ID_E3	0x00E3		/* E3 */
#define ATMZR_PLIMR_ID_DS1	0x00D1		/* DS-1 */
#define ATMZR_PLIMR_ID_E1	0x00E1		/* E1 */
#define ATMZR_PLIMR_ID_NONE	0x00FF		/* No PLIM installed */
#define ATMZR_PLIMR_REV_MSK	0xFF00		/* PLIM REV field mask */

/* CONTROL Register (0x0200) */
#define ATMZR_CTLR_RSTNIMN	0x0001		/* Reset NIM (Active Low) */
#define ATMZR_CTLR_RSTPLIMN	0x0002		/* Reset PLIM (Active Low) */
#define ATMZR_CTLR_WRBURST12	0x0004		/* D-bus wr burst = 12 words */
#define ATMZR_CTLR_RDBURST12	0x0008		/* D-bus rd burst = 12 words */
#define ATMZR_CTLR_CLRLINT	0x0010		/* INT_CLR OR IACK LINT clear */
#define ATMZR_CTLR_PRUCLK_1U	0x0000		/* PRU clock = 1.28us */
#define ATMZR_CTLR_PRUCLK_2U	0x0020		/* PRU clock = 2.56us */
#define ATMZR_CTLR_PRUCLK_5U	0x0040		/* PRU clock = 5.12us */
#define ATMZR_CTLR_PRUCLK_10U	0x0060		/* PRU clock = 10.24us */
#define ATMZR_CTLR_PRUCLK_MSK	0x0060		/* PRU clock mask */
#define ATMZR_CTLR_ENLIRQ	0x0100		/* Enable LIRQ */
#define ATMZR_CTLR_ENHIRQ1	0x0200		/* Enable HIRQ1 = ATMizer */
#define ATMZR_CTLR_ENHIRQ2	0x0400		/* Enable HIRQ2 = ATMizer */
#define ATMZR_CTLR_ENHIRQ3	0x0800		/* Enable HIRQ3 = PLIM */
#define ATMZR_CTLR_ENHIRQ4	0x1000		/* Enable HIRQ4 = error */

/* ERR_STAT Register (0x0202) */
#define ATMZR_ERSTR_XILINXRDY	0x0001		/* Xilinx cfg complete */
#define ATMZR_ERSTR_ASPADDRER	0x0002		/* ATMizer 2ndary port adr er */
#define ATMZR_ERSTR_APPDATER	0x0004		/* ATMizer prim port data er */
#define ATMZR_ERSTR_XILINXDWNL	0x0080		/* Xilinx rdy to be downladed */
#define ATMZR_ERSTR_LIRQ	0x0100		/* ATMizer Low IRQ */
#define ATMZR_ERSTR_EVENT_HIRQ1	0x0200		/* ATMizer High IRQ 1 */
#define ATMZR_ERSTR_RSRDY_HIRQ2	0x0400		/* ATMizer High IRQ 2 */
#define ATMZR_ERSTR_PLIMIRQ	0x0800		/* PLIM IRQ */
#define ATMZR_ERSTR_XILVER_MSK	0xF000		/* Xilinx version field mask */

/*
 * ATMizer NIM memory map.
 * The offset where the NIM address space starts depends on the slot
 * the NIM is plugged in.
 */
typedef struct atmzr_regs_t_ {
    vshort		pcidr;			/* PC_ID Register */
    vshort		pcrevr;			/* PC_REV Register */
    vshort		memcfgr;		/* MEM_CONFIG Register */
    vshort		plimr;			/* PLIM ID/REV Register */
    vshort		res1r[252];		/* Undefined: CPU acc->error */
    vshort		ctlr;			/* CONTROL Register */
    vshort		erstr;			/* ERR_STAT Register */
    vshort		lirqvecr;		/* LIRQ_VEC Register */
    vshort		clrlirqr;		/* CLR_LIRQ Location */
    vshort		res2r[252];		/* Undefined: CPU acc->error */
    union {					/* PLIM 3KB address space */
	suni_lite_t	sunilite;		/* SUNI-LITE PLIM */
	suni_pdh_t	sunipdh;		/* SUNI-PDH PLIM */
    } plim;
    vchar		res3r[0x3F000];		/* Undefined: CPU acc->error */
    pusram_t		pusram;			/* Public SRAM */
} atmzr_regs_t;
#define plim_sunilite		plim.sunilite
#define plim_sunipdh		plim.sunipdh


/*******************************************************************
		System Control Definitions
 *******************************************************************/

/*
 * Hardware database, for fast translation of slot number to
 * the assocaited hardware IDB structure pointer.
 */
typedef struct atmzr_hw_t_ {
    hwidbtype           *hwidb;			/* Hardware IDB pointer */
} atmzr_hw_t;

/*
 * RECEIVE FREE BUFFER circular index queue control structure.
 * There is one of this structures for each free receive circular
 * queues (one for each MTU size). It controls the flow of free
 * buffers allocation. The high water mark determines the maximum
 * number of free buffers to be allocated to this queue at any point
 * in time, and its value depends on what is the total bandwidth for
 * all the VCs using this buffer queue. The low water mark triggers
 * the refilling of the buffer queue, so that buffers are returned
 * in batches of (HighWaterMark - LowWaterMark) buffers for better
 * performance.
 */
typedef struct rxfreeq_db_t_ {
    char		*name;			/* Name of this queue */
    indx_t	 	*base;			/* Queue base address */
    indx_t	 	*wr_ptr;		/* Queue write pointer */
    pooltype		*bufpool;		/* Pointer to buffer pool */
    ushort		size;			/* Queue size (# entries) */
    ushort		high_mark;		/* Queue high water mark */
    ushort		level;			/* # queue elements */
    ushort		num_vc;			/* # VCs using this pool */
    ushort		pad;			/* Alignment padding field */
    ulong		bufmiss;		/* No rx buffer in system */
    ulong		cachemiss;		/* No rx buffer in cache */
    ulong		pktidmiss;		/* No rx pktid errors */
} rxfreeq_db_t;

/*
 * Circular index queue PRODUCER control structure.
 * This structure type is used to control each interface circular index
 * queue for which the host is the producer.
 */
typedef struct wrq_db_t_ {
    char		*name;			/* Name of this queue */
    indx_t	 	*base;			/* Queue base address */
    indx_t	 	*wr_ptr;		/* Queue write pointer */
    ushort		size;			/* Queue size (# entries) */
    ushort		level;			/* Number of queue elements */
} wrq_db_t;

/*
 * Circular index queue CONSUMER control structure.
 * This structure type is used to control each interface circular index
 * queue for which the host is the consumer (this includes the event
 * queue).
 */
typedef struct rdq_db_t_ {
    char		*name;			/* Name of this queue */
    indx_t	 	*base;			/* Queue base address */
    indx_t	 	*rd_ptr;		/* Queue read pointer */
    ushort		size;			/* Queue size (# entries) */
} rdq_db_t;

/*
 * Circular COMMAND/RESPONSE queue control structure.
 * This structure controls access to the Command/Response circular
 * queue. The Write pointer always indicates the next location to write
 * a command to. The Read pointer indicates the oldest outstanding
 * command which is to be the next response to become available.
 */
typedef struct rspmon_t_ {
    cmdrsp_t	*mb_ptr;			/* Cmd/Rsp mailbox pointer */
    uchar	*data_buf;			/* User response data buffer */
    rsp_t	rsp;				/* Response code */
    sys_timestamp timer;			/* Reponse time-out timestamp */
    ushort	rsp_dsize;			/* response length (bytes) */
    boolean	async;				/* Asynchronous option flag */
} rspmon_t;

typedef struct cmdrspq_db_t_ {
    char		*name;			/* Name of this queue */
    cmdrsp_t	 	*base;			/* Queue base address */
    cmdrsp_t	 	*limit;			/* Queue limit address */
    ushort	 	wr_ndx;			/* Queue write index */
    ushort	 	rd_ndx;			/* Queue read index */
    ushort	 	async_cnt;		/* Q# outstanding async cmds */
    ushort		level;			/* Number of queue elements */
    rspmon_t		rspmon[CMDRSPQSZ];	/* Rsp monitoring structure */
} cmdrspq_db_t;

/*
 * Packet Descriptor Index to paktype pointer translation table.
 */
typedef struct id2pak_t_ {
    paktype		*pak_ptr;		/* Packet header pointer */
    void		*qdb_ptr;		/* Queue control struc ptr */
    ushort		next;			/* Free list next entry */
    boolean		recv_flag;		/* Set to TRUE for Rx */
} id2pak_t;

/*
 * NIM global control structure.
 * This structure is associated to a hwidb structure. There is one of
 * those structures per hardware interface (NIM).
 */
typedef struct atmzr_instance_t_ {
    /* NIM hardware variables */
    hwidbtype		*hwidb;			/* Associated hardware IDB */
    atmzr_regs_t	*nim_regs;		/* NIM hardware registers */
    ulong		nim_pusram_size;	/* Public SRAM size (bytes) */
    ulong		nim_prsram_size;	/* Private SRAM size (bytes) */
    ulong		nim_proc_clock;		/* ATMizer clock freq (kbps) */
    ulong		nim_pru_clock;		/* ATMizer PRU clk frq (kbps) */
    ushort		plim_type;		/* PLIM type */
    ushort		plim_version;		/* PLIM version */
    ulong		plim_linerate;		/* PLIM line rate (kbps) */
    inthand_t		plim_reset;		/* PLIM reset init func ptr */
    inthand_t		plim_show_regs;		/* PLIM show regs func ptr */
    inthand_t		plim_inthand;		/* PLIM intr handler func ptr */

    /* Interface queues & access control variables */
    rxfreeq_db_t	rxfreeQ[NUM_FREEQ];	/* Free buf rx index queues */
    rdq_db_t		rxrdyQ;			/* Receive Ready index queue */
    wrq_db_t		txrdyQ;			/* Transmit Ready index queue */
    rdq_db_t		txdoneQ;		/* Trasnmit Done index queue */
    rdq_db_t		eventQ;			/* Event queue */
    cmdrspq_db_t	cmdrspQ;		/* Command/Response queues */
    ulong		ndxq_ptrmask;		/* Index q wrap-around msk */
    boolean		adjust_pools;		/* Buf pools need adjustment */

    /* Packet descriptors table & access control variables */
    pktd_t		*pktdT;			/* Packet descr table address */
    id2pak_t		id2pakT[PKTDESCRTSZ];	/* PktDescr <-> Paktype table */
    ushort		pktid_nextfree;		/* Next free pkt descr to use */
    ushort		pktid_rxcount;		/* Total # rx pkt descr */
    ushort		pktid_rxlimit;		/* Max # rx pkt descr */
    ushort		pktid_txcount;		/* Tx outstanding pkt descr */
    ushort		pktid_txlimit;		/* Max number of tx pkt descr */
    ushort              fastest_vc_rate;        /* the fastest va rate */
    boolean             tx_control_flag;        /* when this flag is on, it means
                                                   transmit packet descriptor is used up */

    /* Interrupt Request/Clear memory location pointers */
    intr_t		*intr;			/* Interrupt handling mem ptr */

    /* Statistic counters */
    ulong		fast_switched;		/* # packets fast-switched */
    ulong		bridged;		/* # packets bridged */
    ulong		to_process;		/* # packets sent to process */
    ulong		tx_errors;		/* Tx failed */
    ulong		tx_badid;		/* Bad tx packet id */
    ulong		tx_wrongq;		/* Rx id being received on Tx */
    ulong		tx_nopkt;		/* No packet found for id */
    ulong		tx_restarts;		/* Interface restarts */
    ulong		tx_pktidmiss;		/* No tx pktid errors */
    ulong		tx_badvc;		/* Bad/closed VC */
    ulong		rx_errors[NUM_RXERR];	/* Rx errors */
    ulong		rx_badid;		/* Bad rx packet id */
    ulong		rx_wrongq;		/* Tx id being received on Rx */
    ulong		rx_nopkt;		/* No packet found for id */

    /* General purpose variables */
    ulong		delay_1ms;		/* 1 ms delay count */
    boolean		nim_operational;	/* NIM operational */
    boolean		system_configured;	/* NVRAM parsed -> syst cfgd */
    ushort		last_event;		/* Last NIM reported event */
    ushort		last_rsp_stat;		/* Last NIM response status */
    boolean		hi_irq_reset;		/* hi_irq reset flag */
} atmzr_instance_t;


/*******************************************************************
			Macros
 *******************************************************************/

/*
 * Packet padding macros
 */
#define AAL5PADSIZ(sz) (((sz%48)>40?96:48) - ((sz)%48))
#define ALIGN16(addr) (((ulong)(addr) & 0xF) ?				\
	(((ulong)(addr) & ~0xF) + 0x10) : (ulong)(addr))

/*
 * Index queue macros
 */
#define	RXNEXTINDEX(ds)							\
	(ds->pktid_nextfree && (ds->pktid_rxcount<ds->pktid_rxlimit)) ?	\
	    ds->pktid_nextfree : 0

#define	TXNEXTINDEX(ds)							\
	(ds->pktid_nextfree && (ds->pktid_txcount<ds->pktid_txlimit)) ?	\
	    ds->pktid_nextfree : 0

#define	ALLOCINDEX(id,pak,q,ds,rxflag)					\
	ds->pktid_nextfree = ds->id2pakT[id].next;			\
	ds->id2pakT[id].pak_ptr = pak;					\
	ds->id2pakT[id].qdb_ptr = (void *)(q);				\
	ds->id2pakT[id].recv_flag = rxflag;				\
	(q)->level++;

#define	ID2PAK(pak,id,ds,rxflag) {					\
	id2pak_t *p = &(ds)->id2pakT[id];				\
	if ((id) && ((id)<PKTDESCRTSZ) &&				\
	    (p->pak_ptr) && (p->recv_flag == rxflag)) {			\
	    pak = p->pak_ptr;						\
	} else {							\
	    pak = NULL;							\
	    if (!((id) && ((id)<PKTDESCRTSZ))) {			\
		if (rxflag) {DEBUG_COUNTERS (ds)->rx_badid++;}		\
		else        {DEBUG_COUNTERS (ds)->tx_badid++;}		\
	    } else if (p->recv_flag != rxflag) {			\
		if (rxflag) {DEBUG_COUNTERS (ds)->rx_wrongq++;}		\
		else        {DEBUG_COUNTERS (ds)->tx_wrongq++;}		\
	    } else if (!(p->pak_ptr)) {					\
		if (rxflag) {DEBUG_COUNTERS (ds)->rx_nopkt++;}		\
		else        {DEBUG_COUNTERS (ds)->tx_nopkt++;}		\
	    }								\
	} }
	    

#define RETID(id,type,ds) {						\
	id2pak_t *p = &(ds)->id2pakT[id];				\
	p->next = (ds)->pktid_nextfree;					\
	p->pak_ptr = NULL;						\
	((type *)(p->qdb_ptr))->level--;				\
	(ds)->pktid_nextfree = id; }

#define PRTFRQSTAT(q)							\
	printf ("\n    %14s 0x%08X 0x%08X %-3d/%3d %-3d  %04d/%04d/%04d",\
	    q.name, q.base, q.wr_ptr, q.level, INDXQSZ,			\
	    q.high_mark, q.cachemiss, q.bufmiss, q.pktidmiss);

#define	PRTRDQSTAT(q)							\
	printf ("\n    %14s 0x%08X 0x%08X %-3d",			\
	    q.name, q.base, q.rd_ptr, INDXQSZ);

#define	PRTWRQSTAT(q)							\
	printf ("\n    %14s 0x%08X 0x%08X %-3d/%3d",			\
	    q.name, q.base, q.wr_ptr, q.level, INDXQSZ);

#define PRTEVQSTAT(ds)							\
	printf ("\n                   0x%08X 0x%08X %-3d  0x%02X",	\
	    ds->eventQ.base, ds->eventQ.rd_ptr, EVENTQSZ, ds->last_event);

#define	PRTPKTSTAT(ds)							\
	printf ("\n                   0x%08X %-3d    %-3d/%3d %-3d/%3d",\
	    ds->pktdT, PKTDESCRTSZ-1, ds->pktid_rxcount,		\
	    ds->pktid_rxlimit, ds->pktid_txcount, ds->pktid_txlimit);

/*
 * Command/Response queue macros
 */
#define	NEXTCMD(ds)							\
	(ds->cmdrspQ.level < CMDRSPQSZ - 1) ?				\
	    &ds->cmdrspQ.rspmon[ds->cmdrspQ.wr_ndx] : 0

#define	PRTCRQSTAT(q,ds) {						\
	printf ("\n                   0x%08X 0x%08X %-3d/%3d ",		\
	    q.base, q.base+q.wr_ndx, q.level, CMDRSPQSZ);		\
	printf ("%-3d    %-3d    %-3d  0x%02X",				\
	    q.wr_ndx, q.rd_ndx, q.async_cnt, ds->last_rsp_stat); }

/*
 * Packet descriptor macros
 */
#define TXPD_STAT(stat)		(((stat) & TXSTAT_MSK) >> TXSTAT_SHIFT)
#define RXPD_VC(vc)		((vc) & RXVCID_MSK)
#define RXPD_STAT(stat)		(((stat) & RXSTAT_MSK) >> RXSTAT_SHIFT)

/*
 * Packet parsing macros
 */
#define RXTYPE_SNAP(rxtype,snap,dsap)					\
        /*								\
         * Check to see if we've got a SNAP code			\
         * in the right place.						\
         */								\
	if ((dsap == (SAP_SNAP | SAP_SNAP << 8)) ||			\
	    (dsap == (ATM_NLPID_SNAP1PAD))) {				\
	    /*								\
	     * Okay. We've got an ISO2 or aligned NLPID/SNAP packet:	\
	     * Find out what type it is from the SNAP header		\
	     */								\
            pak->network_start = snap->data;				\
	    switch ((snap)->type) {					\
	    case TYPE_IP10MB:						\
		rxtype = RXTYPE_DODIP;					\
		break;							\
	    case TYPE_DECNET:						\
		rxtype = RXTYPE_DECNET;					\
		break;							\
	    case TYPE_XNS:						\
		rxtype = RXTYPE_XNS;					\
		break;							\
	    case TYPE_ETHERTALK:					\
		rxtype = RXTYPE_APPLE;					\
		break;							\
	    case TYPE_NOVELL1:						\
		rxtype = RXTYPE_NOVELL1;				\
		break;							\
	    case TYPE_NOVELL2:						\
		rxtype = RXTYPE_NOVELL2;				\
		break;							\
	    case TYPE_VINES2:						\
	    case TYPE_VINES_ECHO2:					\
	    case TYPE_VINES:      /* optional */			\
	    case TYPE_VINES_ECHO: /* optional */			\
		rxtype = RXTYPE_VINES;					\
		break;							\
	    case TYPE_DEC_SPANNING:					\
		rxtype = RXTYPE_UNKNOWN;				\
		break;							\
	    case TYPE_RSRB:						\
		rxtype = RXTYPE_SRB;					\
		break;							\
	    default:							\
		rxtype = RXTYPE_UNKNOWN;				\
	    }								\
        } else {							\
            rxtype = RXTYPE_UNKNOWN;					\
        }

/*
 * Global macros
 */
#define	ATMZR_HUNG_CHECK(idb,ds,level)					\
	level = raise_interrupt_level (NETS_DISABLE);			\
	if (TIMER_RUNNING_AND_AWAKE ((idb)->xmittimer) ||               \
            ds->hi_irq_reset) {                  			\
	    GET_TIMESTAMP ((idb)->lastreset);				\
	    (ds)->tx_restarts++;					\
	    DEBUG_ERRORS ("\natmzr_output(%s): transmit hung",		\
		idb->hw_namestring);					\
	    atmzr_reset (idb);						\
	}								\
	reset_interrupt_level (level);

#define	HIHW(w)		(((w) & 0xFFFF0000) >> 16)
#define	LOHW(w)		((w) & 0xFFFF)




/*******************************************************************
			ATMizer function prototypes
 *******************************************************************/

extern void atmzr_subsys_init (subsystype *subsys);
extern void atmzr_analyze_interface (int slot);
extern hwidbtype *atmzr_init_idb (int slot, int unit);
extern boolean atmzr_init_hw_ds (hwidbtype *idb);
extern void atmzr_init_sw_ds (hwidbtype *idb);
extern void atmzr_if_final_init (void);
extern void atmzr_system_configured (hwidbtype *idb);
extern char *atmzr_nim_type_string (int type);
extern char *atmzr_plim_type_string (ushort type);
extern char *atmzr_plim_loopback_type_string (ushort type);
extern void atmzr_shutdown (hwidbtype *idb, int newstate);
extern void atmzr_line_action (hwidbtype *idb);
extern boolean atmzr_line_state (hwidbtype *idb);
extern void atmzr_enable (hwidbtype *idb);
extern void atmzr_disable (hwidbtype *idb);
extern void atmzr_periodic (hwidbtype *idb);
extern boolean atmzr_show_controller (hwidbtype *idb, parseinfo *csb);
extern ulong atmzr_dump_prsram (hwidbtype *idb, uchar *buf, ulong offset, ulong size);
extern boolean atmzr_download_xilinx (hwidbtype *idb);
extern void atmzr_reset (hwidbtype *idb); 
extern void atmzr_setup (hwidbtype *idb);
extern unsigned atmzr_encaps (atm_db_t *atmzr, paktype *pak,
			      vc_info_t *vc, ushort txopt);
extern void atmzr_adjust_pools (hwidbtype *idb);
extern boolean atmzr_setup_vc (hwidbtype *idb, vc_info_t *vcinfo);
extern boolean atmzr_teardown_vc (hwidbtype *idb, vc_info_t *vcinfo);
extern void atmzr_raw_input (hwidbtype *idb, paktype *pak);
extern boolean atmzr_get_vc_stats (hwidbtype *idb, vc_info_t *vc);
extern ushort atmzr_get_nlpidtype (hwidbtype *idb, uchar *encaps);
extern ushort *atmzr_sniff (hwidbtype *hwidb, paktype *pak, uchar *data,
                            ushort *sniff_buffer, int offset, int count);
extern void atmzr_output (hwidbtype *idb);
extern void atmzr_fastsend (hwidbtype *idb, paktype *pak);
extern boolean atmzr_set_maxdgram (hwidbtype *idb, int buffersize, int maxdgram);
extern void atmzr_clear_counters (hwidbtype *idb);
extern void atmzr_refill_freeq (atmzr_instance_t *ds, rxfreeq_db_t *freeq);
extern void atmzr_lo_irq_hand (hwidbtype *idb);
extern void atmzr_hi_irq_hand (ushort slot);
extern boolean init_atmzr_dev (hwidbtype *idb);
extern void atmzr_erstr_aspaddrer_hand (hwidbtype *idb);

#endif	__C4000_ATMZR_H__

