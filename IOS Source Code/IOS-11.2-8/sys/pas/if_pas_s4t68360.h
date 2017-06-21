/* $Id: if_pas_s4t68360.h,v 3.15.6.8 1996/08/23 04:47:30 xtang Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_s4t68360.h,v $
 *------------------------------------------------------------------
 * if_pas_s4t68360.h -- definitions for 4T PCI Port Adaptor
 *
 * 3/02/95   H.M. Robison 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_s4t68360.h,v $
 * Revision 3.15.6.8  1996/08/23  04:47:30  xtang
 * CSCdi59553:  clockrate 250000 is not configurable on FSIP in 75xx
 * Branch: California_branch
 *              added 250000 Hz clockrate to s4t68360 4T
 *              port adaptor. The corresponding microcode change are in
 *              cvs -d /micro s4t68360
 *
 * Revision 3.15.6.7  1996/06/18  19:25:55  gstovall
 * CSCdi58572:  Serial line comes back from the dead
 * Branch: California_branch
 * Check physical line state before deciding if the line is up or not.
 *
 * Revision 3.15.6.6  1996/06/07  04:08:19  amcrae
 * CSCdi56758:  4T does not recover after power surge - should be restarted
 * Branch: California_branch
 * Added structure to carry 4T PA data. Added periodic watchdog check
 * and restart handling. Nuked some unused counters.
 *
 * Revision 3.15.6.5  1996/05/22  23:28:15  rramacha
 * CSCdi58249:  Enable IP DFS on 4T
 * Branch: California_branch
 *  o receive serial encap cmd
 *  o turn-on DFS on 4T
 *  o decode HDLC, PPP encapsulation on the received packet
 *
 * Revision 3.15.6.4  1996/05/06  16:43:11  gstovall
 * CSCdi54863:  Priority queuing not working on new interface
 * Branch: California_branch
 * Allow the various queueing options to be enabled on the 4T in a
 * predator.
 *
 * Revision 3.15.6.3  1996/04/30  16:50:03  gstovall
 * CSCdi55513:  Fix OIR problems
 * Branch: California_branch
 * Fix oir memory handling on 4t.
 *
 * Revision 3.15.6.2  1996/04/10  02:43:26  rramacha
 * CSCdi52294:  Add 4T support for VIP2
 * Branch: California_branch
 * VIP2 support for 4T
 *
 * Revision 3.15.6.1  1996/03/21  23:23:45  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.8  1996/03/08  22:10:02  gstovall
 * CSCdi51111:  Need new 4T firmware
 * Branch: ELC_branch
 * Suck in some of Andrews magic firmware, plus update the driver to deal
 * with it.
 *
 * Revision 3.2.2.7  1996/02/14  01:58:12  gstovall
 * CSCdi48946:  Pull in 4T bugfixes from mainline
 * Branch: ELC_branch
 * Bring the ELC 4T driver up to date.
 *
 * Revision 3.2.2.6  1996/02/08  08:48:08  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.5  1996/01/24  04:16:47  gstovall
 * Branch: ELC_branch
 * Pull in some bug fixes from the 11.1 mainline, make fastswitching work
 * AND count correctly, notice if a cable gets pulled.
 *
 * Revision 3.2.2.4  1996/01/18  22:11:17  gstovall
 * Branch: ELC_branch
 * Preadator now knows the joy and splendor of serial interfaces.
 *
 * Revision 3.2.2.3  1996/01/16  01:43:20  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.2  1995/12/11  19:13:56  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.1  1995/12/04  22:18:35  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.12  1996/02/04  09:32:51  rharagan
 * CSCdi45869:  4T does not receive data when cabled as rs-232 DCE
 * Set no_txce control based on sensed cable type, handle rx errors.
 *
 * Revision 3.11  1996/01/20  04:59:28  amcrae
 * CSCdi47215:  4T exception processing doesnt signal host
 * CSCdi44907:  Fixed macros overlooked in previous commit
 *
 * Revision 3.10  1996/01/18  23:38:37  bburns
 * CSCdi43000:  4T driver should not call ../if/pci_discover_devices()
 * Deleted call to registry by removing call to pci_device_discover().
 *
 * Revision 3.9  1996/01/13  03:24:11  amcrae
 * CSCdi45724:  4T f/w is not answering mbx msgs during an interface reset
 * CSCdi43260:  VIP 4T has no implementation for transmit-delay
 * New interrupt scheme added. Firmware counter update added.
 *
 * Revision 3.8  1996/01/09  22:32:45  rharagan
 * CSCdi46625:  Holdq dequeueing broken for 4T
 * Pick up 4T version CSCdi41406. Sweep up a bit.
 *
 * Revision 3.7  1995/12/23  01:14:33  rharagan
 * CSCdi45811:  4T interface linestate does not refletct modem control
 * lines
 * Straighten out changes in linestate, continue to bring in code review
 * comments
 *
 * Revision 3.6  1995/12/19  20:24:33  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.5  1995/12/14  07:41:08  rharagan
 * CSCdi42045:  tx0 output hung (800E - queue full) when pinging on serial
 * 4T
 * Strip out now unneeded tx pkt fragmentation, lose unwieldy ddelays on
 * mbx reads.
 *
 * Revision 3.4  1995/12/02  05:50:26  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.3  1995/11/28  02:26:19  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.2  1995/11/17  18:54:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if defined(RVIP) || defined(SVIP)
#include "if_vip_s4t68360.h"
#endif

#if defined(PREDATOR)
#include "if_c7100_s4t68360.h"
#endif

#define VISIBLE_SERIAL_BANDWIDTH 1544
#define SERIAL_DELAY        2000    /* 20 millisecond */
#define SERIAL_MAXDGRAM		1524    

#define S4T_MIN_PACKET	2	/* Min size packet for tx */

#define S4T_MAGIC_PAK_PTR	((paktype *)0x03)

#define S4T_LINK_UP		1
#define S4T_LINK_DOWN		2


/* 
 * 4T 5in1 applique codes 
 */
#define S4T_PA_NONE                         0x00
#define S4T_PA_V35_DTE                      0x02
#define S4T_PA_V35_DCE                      0x03
#define S4T_PA_RS232_DTE                    0x06
#define S4T_PA_RS232_DCE                    0x07
#define S4T_PA_RS530_DTE                    0x08
#define S4T_PA_RS530_DCE                    0x09
#define S4T_PA_RS449_DTE                    0x0a
#define S4T_PA_RS449_DCE                    0x0b
#define S4T_PA_X21_DTE                      0x0c
#define S4T_PA_X21_DCE                      0x0d
#define S4T_PA_NO_CABLE                     0x0f

/* 
 *  BIT RATE CODES
 */
enum s4t68360_bps {
    BPS_NONE = 0,
    BPS_1200,
    BPS_2400,
    BPS_4800,
    BPS_9600,
    BPS_19200,
    BPS_38400,
    BPS_56K,
    BPS_64K,
    BPS_72K,
    BPS_125K,
    BPS_148K,
    BPS_250K,
    BPS_500K,
    BPS_800K,
    BPS_1000K,
    BPS_1300K,
    BPS_2M,
    BPS_4M,
    BPS_8M,

    S4T_CLOCKS  /* S4T_CLOCKS must be the last entry */
};

/*
 * Struct for clock rate command
 */
typedef struct s4t_clocktype {
    uint rate;
    uchar bitrate_code;
} s4t_clocktype_t;

extern const s4t_clocktype_t s4t_clock[];

/*
 * AMCC S5933 PCI Controller Device specific registers
 *   Note: Standard PCI device stuff is defined in ../if/pci.h
 *   such as config register offsets, ...
 */

#define S4T_PCI_DEVICE_NO 0      /* PCI device number for 4T PA */

/* default: AMCC vendor id = 0x10e8; S5933 device id = 0x4750 */
#define S4T_AMCC_S5933_VENDOR_DEVICE_ID     0x475010E8


/* For LAT latency timer register: part of 32bit value */
/* 1st byte is CALN = 0; 3rd is HDR = 0; 4th is BIST = 0; */
/*#define S5933_LATENCY_TIMER_VALUE 0x00000000 */

/*  errata D14 from AMCC Rev 3 8/30/95,    */
#define S5933_LATENCY_TIMER_VALUE 0xF800

/* use when reading mbx_status_reg */
#define S5933_OUT_MBX_STATUS_MASK   0x00000FFF /* only mailboxes 1-3 */
#define S5933_IN_MBX_STATUS_MASK    0x0FFF0000
#define S5933_OUT_INT_STATUS_MASK   0x0000F000 /* only mailbox 4 */
#define S5933_IN_INT_STATUS_MASK    0xF0000000

#define S5933_IN_MBX1_CMD_MASK      0xFF000000 /* cmd from mbx 1 */

#define S5933_MBX_EMPTY_RETRIES     100 /* delay as parameter*/

#define S5933_MBX_EMPTY_RETRIES_DWNLD     500 /* no delay, just flogs
						 the bus */

/* define order of writing mailboxes 1-2-3, 3-2-1, or just 3 */
#define S5933_REVERSE_MBX_WRITE 1
#define S5933_NORMAL_MBX_WRITE 0
#define S5933_CON_DWNLD_MBX_WRITE -1

/*
 * Interrupt control/status register
 */
#define	S5933_INT_CTRL_IMBOX_ENABLE		0x00001000
#define	S5933_INT_CTRL_IMBOX_4			0x00000C00
#define S5933_INT_CTRL_IMBOX_INTR		0x00020000

#define S5933_BUS_MSTR_RD_XFER_ENABLE 0x4000     /* Bit 14 */
#define S5933_BUS_MSTR_WR_XFER_ENABLE 0x0400     /* bit 10 */

typedef struct s5933_ops_regs {
    volatile ulong out_mbx1;
    volatile ulong out_mbx2;
    volatile ulong out_mbx3;
    volatile ulong out_mbx4;
    volatile ulong in_mbx1;
    volatile ulong in_mbx2;
    volatile ulong in_mbx3;
    volatile ulong in_mbx4;
    volatile ulong fifo_reg_port;
    volatile ulong mstr_wr_addr_reg;
    volatile ulong mstr_wr_xfr_cnt_reg;
    volatile ulong mstr_rd_addr_reg;
    volatile ulong mstr_rd_xfr_cnt_reg;
    volatile ulong mbx_status_reg;
    volatile ulong int_ctrl_reg;
    volatile ulong bus_mstr_ctrl_reg;
    } s4t_regs_t;



/*
 * buffer dscrptr description
 */
typedef struct s4t_dscrptr_t_ {
    volatile ushort status;             /* See below for description */
    volatile ushort length;             /* byte length */
    volatile ulong  buf_addr;            /* buffer address */
} s4t_dscrptr_t;

typedef struct s4t_tx_shadow_t_ {
    paktype *pak;		/* pointer to pak */
    particletype *mp;		/* pointer to particle */
} s4t_tx_shadow_t;


/* RX descriptor defines for status word */
#define S4T_RXD_NB	0x4000 /* device - ran out of buffers */
#define S4T_RXD_LG	0x2000 /* device - frame length over max */
#define S4T_RXD_NO	0x1000 /* device - frame not byte aligned */
#define S4T_RXD_AB	0x0800 /* device - ones violation */

#define S4T_RXD_CR	0x0400 /* device - CRC error */
#define S4T_RXD_OV	0x0200 /* device - receiver overrun */
#define S4T_RXD_CD	0x0100 /* device - carrier detect lost */
#define	S4T_RXD_OWN	0x0080 /* Buffer is owned by device */

#define S4T_RXD_ERR	0x0040 /* Some error - OR of errors */
#define S4T_UNUSED5     0x0020 /* unused */
#define S4T_RXD_OFLO	0x0010 /* FIFO overflow - ran out of memory */
#define S4T_UNUSED3	0x0008 /* unused */

#define S4T_RXD_BUFF	0x0004 /* ran out of mini-buffers */
#define S4T_RXD_EOP	0x0002 /* Buffer contains end of packet */
#define S4T_RXD_STP	0x0001 /* buffer contains start of packet */

/* For S4T_RXD_ERR: */
#define S4T_RXD_FATAL_ERRS  (S4T_RXD_NB | S4T_RXD_LG | S4T_RXD_NO | S4T_RXD_AB \
                          | S4T_RXD_CR | S4T_RXD_OV | S4T_RXD_CD \
                          | S4T_RXD_OFLO | S4T_RXD_BUFF)

#define S4T_OWNED_BY_MB 0  /* descriptor is owned by motherboard (Orion) */


/* TX descriptor defines for status word */
#define S4T_TXD_CT	0x200	 /* device - CTS lost */
#define S4T_TXD_UN	0x100	 /* device - underrun */
#define	S4T_TXD_OWN	0x080	 /* Buffer is owned by device */
#define S4T_TXD_ERR	0x040    /* Some error - OR of errors */

#define S4T_TXD_UFLO	0x020    /* Underflow */
#define S4T_TXD_RTRY	0x010    /* Max retries exceeded */
#define S4T_TXD_DEF	0x008    /* Had to defer */
#define S4T_TXD_BUFF	0x004    /* EOP never found - ran out of mini-buffers */

#define S4T_TXD_STP	0x002    /* buffer contains start of packet */
#define S4T_TXD_EOP	0x001	 /* buffer constains end of packet */

#define S4T_TXD_FATAL_ERRS (S4T_TXD_BUFF | S4T_TXD_UFLO | S4T_TXD_UN \
                           | S4T_TXD_CT | S4T_TXD_RTRY)
#define S4T_TXD_ALL_ERRS (S4T_TXD_DEF | S4T_TXD_FATAL_ERRS)

/*
 *	Interrupt cause bits. The cause word is 32 bits, with
 *	each port allocated 8 bits. MSB is port 0 etc.
 */
#define	S4T_INT_CAUSE_RX		0x01	/* Receive interrupt */
#define	S4T_INT_CAUSE_TX		0x02	/* Transmit interrupt */
#define	S4T_INT_CAUSE_STAT		0x04	/* Line/cable status interrupt */

/*
   4T Port Adapter Init Block
*/

#define S4T_NRZ 0
#define S4T_NRZI 1
#define S4T_TXC_NORMAL 0
#define S4T_TXC_INVERT 1
#define S4T_DCE_TXCE 0
#define S4T_DCE_NO_TXCE 1
#define S4T_RX_SYNC_NORMAL 0
#define S4T_RX_SYNC_DISABLE 1
#define S4T_FULL_DUPLEX 0
#define S4T_HDLC_MODE 0
#define S4T_BISYNC_MODE 1

/* WARNING: do not change anything in here w/o correponding change in
 * 4t pa f/w structure located in s4t68360_pa_def2.h
 */
typedef struct s4t_init_block_t_ {
    ulong rx_beg;
    ulong rx_end;
    ulong tx_beg;
    ulong tx_end;
    
    uchar tx_bit_rate;        /* generated bit rate if DCE */
    uchar idle_code;          /* 0 for flag or 1 for all mark idle */
    uchar coding_format;      /* NRZ or NRZI: NRZ = 0; NRZI = 1 */
    uchar crc_type;           /* 0 = CRC16; 1 = CRC32 */
    
    uchar txc_invert;         /* 0 = normal; 1 = invert */
    uchar spare_flag;        /* this space available */
    uchar rx_sync_disable;    /* 0 = normal; 1 = rxsynch disabled*/
    uchar full_half_dplx;     /* 0 = Full Duplex; 1 = Half Duplex */
    
    ulong tx_delay;           /* transmit delay in microseconds */
    ushort mtu;                /* maximum transmission unit */
    ushort mode;              /* 0 = HDLC; 1 = BISYNC */
    ushort particle_size;              /* 0 = HDLC; 1 = BISYNC */
} s4t_init_block_t;

/*
 *	Statistics counters.
 */
typedef struct
	{
	volatile int updated;				/* Flag indicating update. */
	s4t_init_block_t init_copy;			/* Copy of init block */
/*
 *	Global stats.
 */
	unsigned int mbox_read;				/* # mailbox reads */
	unsigned int mbox_write;			/* # mailbox writes */
	unsigned int dma_read;				/* # PCI->4T transfers */
	unsigned int dma_write;				/* # 4T->PCI transfers */
	unsigned int dma_nomem;				/* # malloc fails */
	unsigned int dma_spurint;			/* # DMA spurious intrs */
	unsigned int dma_mrar;				/* # DMA read errors */
	unsigned int dma_mwar;				/* # DMA read errors */
	unsigned int dma_mrtc;				/* # DMA read errors */
	unsigned int dma_mwtc;				/* # DMA read errors */
	unsigned int interrupts;			/* # Host interrupts */
	unsigned int freemem;				/* # bytes of free memory */
	unsigned int idle;					/* Idle loops */
	unsigned int loops;					/* # Total service loops */
/*
 *	Per-port transmit stats
 */
	unsigned int tx_ring_polled;		/* # host ring polled */
	unsigned int tx_ring_updated;		/* # TX host ring written */
	unsigned int tx_head;				/* Current host ring head */
	unsigned int tx_tail;				/* Current host ring tail */
	unsigned int tx_bad_buf;			/* instances of bad tx ring entries */
	unsigned int tx_enable;				/* # tx enabled */
	unsigned int tx_disable;			/* # tx disabled */
	unsigned int tx_pending;			/* Pending to be sent */
	unsigned int tx_pkt;				/* Total packets sent */
	unsigned int tx_err;				/* TX errors */
	unsigned int tx_ring_err;			/* Errors on TX ring */
	unsigned int tx_spurint;			/* Error on polling */
	unsigned int tx_toobig;				/* Spare counters */
	unsigned int tx_scc_used;			/* SCC entries in-use */
	unsigned int tx_sp3;
	unsigned int tx_sp4;
	unsigned int tx_sp5;
	unsigned int line_change;			/* Status updates */
/*
 *	Per port receive stats
 */
	unsigned int rx_ring_polled;		/* # host RX ring polled */
	unsigned int rx_ring_updated;		/* # RX host ring written */
	unsigned int rx_head;				/* Current host ring head */
	unsigned int rx_bad_ring;			/* Insane ring entry */
	unsigned int rx_enable;				/* # rx enabled */
	unsigned int rx_disable;			/* # rx disabled */
	unsigned int rx_buf;				/* RX particles */
	unsigned int rx_pkt;				/* RX packets */
	unsigned int rx_err;				/* RX errors */
	unsigned int rx_framing;			/* RX Framing errors */
	unsigned int rx_dropped;			/* RX No buffers */
	unsigned int rx_scc_head;			/* Current SCC head */
	unsigned int rx_sp2;				/* Spare counters */
	unsigned int rx_sp3;
	unsigned int rx_sp4;
	unsigned int rx_sp5;
	} s4t_stats;


/* 4T messages between driver & PA */
#define S4T_CMD_DOWNLOAD_READY 0       /* no message body */
#define S4T_CMD_DOWNLOAD 1
#define S4T_CMD_DOWNLOAD_DONE 2 
#define S4T_CMD_DOWNLOAD_EXEC 3

#define S4T_CMD_DOWNLOAD_REPORT 20
#define S4T_CMD_INIT_PORT 21
#define S4T_CMD_CONTROL_PORT 22
#define S4T_CMD_REPORT_STATUS 23
#define S4T_CMD_RESET_PORT 24
#define S4T_CMD_REPORT_ERROR 25
#define S4T_CMD_CHG_INIT_PARAM 26
#define S4T_CMD_REPORT_VERSION 27
#define S4T_CMD_ENABLE_PORT 28   
#define S4T_CMD_MASK_PORT_STATUS 29   
#define S4T_CMD_QUERY_STATUS 30
#define S4T_CMD_QUERY_RING_PTRS  31    /* used w/ PORT_POINTERS */
#define S4T_CMD_START_TRANSMIT   32
#define	S4T_CMD_UPDATE_STATS	 33		/* Update statistics */
#define	S4T_CMD_CLEAR_STATS		 34	/* Clear statistics */
#define	S4T_CMD_GET_TRACE		 35	/* Get trace code */
#define	S4T_CMD_RESET			 36	/* Reset port adaptor */
#define	S4T_CMD_LOCKUP			 37	/* Lockup PA (for testing) */
#define S4T_CMD_MSGQ             38     /* Size of message queue */

/* diagnostics commands */
#define S4T_CMD_PEEK_68360  100	/* diagnostics-peek 68360 internal ram */
#define S4T_CMD_PEEK_PCI    101	/* diagnostics-peek PCI ram */
#define S4T_CMD_POKE_68360  102	/* diagnostics-poke 68360 internal ram */
#define S4T_CMD_POKE_PCI    103	/* diagnostics-poke PCI ram */


/* Message body structures */
typedef struct {
   volatile uchar no_message_body;
   } DOWNLOAD_READY;

typedef struct {
   volatile uchar *load_address;
   volatile ulong data;
   } DOWNLOAD;

typedef struct {
   volatile ulong result;
   } DOWNLOAD_REPORT;

typedef struct {
   volatile long checksum;
   } DOWNLOAD_DONE;

typedef struct {
   volatile uchar *exec_address;
   } DOWNLOAD_EXEC;

typedef struct {
   volatile uchar port_id;            /* which port id (0-3) */
   volatile ushort length;            /* length of init block in bytes */
   s4t_init_block_t *init_block;      /* start addr of init block */
   } INIT_PORT;

typedef struct {
   volatile uchar port_id;            /* which port id */
   volatile uchar offset;             /* byte offset into init block */
   volatile ushort size;              /* size of parameter in bytes */
   volatile ulong new_value;          /* new value of parameter */
   } CHG_INIT_PARAM;

#define TX_BIT_RATE_OFFSET	16
#define IDLE_CODE_OFFSET	17
#define CODING_FORMAT_OFFSET 	18
#define CRC_TYPE_OFFSET		19
#define TXC_INVERSION_OFFSET	20
#define DCE_NO_TXCE_OFFSET	21
#define RX_SYNC_OFFSET          22
#define FULL_HALF_DPLX_OFFSET   23
#define TX_DELAY_OFFSET 	24
#define MTU_OFFSET              28
#define MODE_OFFSET             30


typedef struct {
   volatile uchar port_id;
   volatile ushort port_control;
   } CONTROL_PORT;

/* Defines for CONTROL_PORT command */
#define S4T_CONTROL_RTS_CTS 0x01   /* set/clr- DCE mode: RTS; DTE mode: CTS */
#define S4T_CONTROL_DTR_DSR 0x02   /* set/clr- DCE mode: DTR; DTE mode: DSR */
#define S4T_CONTROL_LOOP    0x08   /* put port in loopback */
#define S4T_CONTROL_NOLOOP  0x00   /* take port out of loopback */
#define S4T_DISABLE_MODEM_CTL   0x0000   


/* reset port: resets 5in1 HW; RX/TX Ring SW.
   Followed by INIT_BLOCK cmd
*/
typedef struct {
   volatile uchar port_id;  /* 1 = reset; 0 = normal */
   volatile uchar port_reset; 
   } RESET_PORT;

#define S4T_RESET_PORT 1
#define S4T_NORMAL_PORT 0


/* Enables/Disables Rx/Tx transmission.
   Disable is default power-up status.
   When disabled, PA no longer OWNs any PCI memory BD's or mini-buffers;
   Drop all Rx/Tx packets.
   No new init block when enabled.
*/ 
typedef struct {
    /* 0 = disable; BIT0 = enable RX; BIT1 = enable TX; */
   volatile uchar port_id;   
   volatile uchar port_en; 
   } ENABLE_PORT;

#define UNAVAILABLE_PORT 0xF0
#define DISABLE_PORT 0
#define RX_ENABLE 0x01
#define TX_ENABLE 0x02

/* Other states kept internally but not sent to PA */
#define PA_FAILED 0xFF       /* download failed */

typedef struct {
   volatile uchar port_id;
   volatile ushort port_status;
   } REPORT_STATUS;

/* Defines for STATUS_PORT command */
#define S4T_STATUS_RTS_CTS 0x0001   /* DCE mode: RTS; DTE mode: CTS */
#define S4T_STATUS_DTR_DSR 0x0002   /* DCE mode: DTR; DTE mode: DSR */
#define S4T_STATUS_DCD     0x0004   /* only in DTE mode */
#define S4T_STATUS_LOOP    0x0008   /* loopback status: 1 = on */
#define S4T_CABLE_MODE     0x00F0   /* four bits to indicate cable
				       type */

#define S4T_SIGNAL_MASK (S4T_STATUS_RTS_CTS | S4T_STATUS_DTR_DSR | \
                          S4T_STATUS_DCD )

                    

#define S4T_LOOPBACK_OFF 0
#define S4T_LOOPBACK_ON  1

typedef struct {
   volatile uchar port_id;
   volatile ushort port_mask;
   } MASK_PORT_STATUS;

/* Defines for STATUS_PORT command */
#define S4T_MASK_RTS_CTS      0x0001   /* DCE mode: RTS; DTE mode: CTS */
#define S4T_MASK_DTR_DSR      0x0002   /* DCE mode: DTR; DTE mode: DSR */
#define S4T_MASK_DCD          0x0004   /* only in DTE mode */
#define S4T_MASK_LOOP         0x0008   /* loopback status: 1 = on */
#define S4T_MASK_MODE0        0x0010   /* 4-bit 5in1 applique value */
#define S4T_MASK_MODE1        0x0020
#define S4T_MASK_MODE2        0x0040
#define S4T_MASK_MODE3        0x0080
/*...*/
#define S4T_MASK_STATUS_INT   0x8000 /* mask status interrupt for port */


typedef struct {         /* report receiver aborts */
   volatile uchar port_number;
   volatile ushort event_cnt;
   } REPORT_ERROR; 

typedef struct {         /* Start transmit */
   volatile uchar port_number;
   } START_TRANSMIT; 

typedef struct {                    /* for S4T_CMD_UPDATE_STATS */
	unsigned char port_id;          /* init block of port# (0-3) */
	unsigned char padding;		 	/* Round up to 16 bit alignment */
	unsigned short length;          /* length of stats block in bytes */
	s4t_stats *stats; 			    /* start stats addr in PCI memory */
	} UPDATE_STATS;

typedef struct {         /* Clear stats */
   volatile uchar port_id;
   } CLEAR_STATS; 

typedef struct {
   volatile ushort sw_version;	/* software version */
   volatile ushort hw_version;	/* hardware version */
   volatile ushort cpu_speed;	/* hardware version */
   } REPORT_VERSION;


typedef struct {
   volatile ulong *address;      /* address to peek/poke */
   volatile ulong data;         /* data read/written */ 
   } PEEK_POKE;

typedef struct {
   volatile ulong tracecode;
   volatile ulong tracedata;
   } GET_TRACE;

typedef struct {              /* S4T_CMD_MSGQ */
    volatile int qsize;       /* Length of message queue */
    } MSGQ_SIZE;

/*
 * 4T mailbox message structures & commands:
 *
 * Note: Only mailboxes 1-3 are used for messages.
 *   The fourth mailbox is reserved for 'ACK'ing
 *   the rxtx_interrupt from the port adapter.
 */
typedef union {
   struct mailbox_msg_regs {
      volatile ulong mbx1;
      volatile ulong mbx2;
      volatile ulong mbx3;
      } mbx_regs;
 
   struct message {
      volatile uchar cmd_byte;
      uchar not_use[3];
      union {
         DOWNLOAD_READY download_ready;
         DOWNLOAD download;             /* download 68360 code to given addr */
         DOWNLOAD_DONE download_done;   /* stop download */
         DOWNLOAD_EXEC download_exec;   /* run code at given address */
         DOWNLOAD_REPORT download_report; /* report download status */
         INIT_PORT init_port;           /* assign init block for each port */
	 START_TRANSMIT start_transmit;	/* Start transmit */
         UPDATE_STATS update_stats;     /* Copy stats counters */
         CLEAR_STATS clear_stats;       /* Clear stats counters */
         CHG_INIT_PARAM chg_init_param; /* change value of init block param */
         CONTROL_PORT control_port;     /* control modem lines, etc. */
         RESET_PORT reset_port;         /* reset port */
         ENABLE_PORT enable_port;       /* enable port */
         REPORT_STATUS report_status;   /* report modem control line status */
         REPORT_STATUS query_status;     /* ask for modem control line status */
         REPORT_VERSION report_version; /* report sw & hw versions of PA */
         REPORT_ERROR report_error;     /* report frame errors */
         MASK_PORT_STATUS mask_port_status;  /* mask status interrupt */
         GET_TRACE get_trace;           /* Trace report */
	 MSGQ_SIZE msgq;
         } msg_body;
    } msg;

  } mbx_message_t;

typedef struct mailbox_msg_regs mbx_msg_regs_t;
typedef struct message message_t;


/*
 * Structure to store 4T pa-specific information.
 */
typedef struct s4t_pa_ {
    int bus;				/* PCI bus number */
    int slot;				/* Bay number */
    s4t_regs_t *devbase;		/* Device base */
    int references;			/* How many instances set up */
    int no_response;			/* No responses */
    int download_delay;			/* time to download */
    int report_delay;			/* time to respond after d/l */
    REPORT_VERSION versions;
    hwidbtype *idbs[S4T_MAX_PORTS];	/* IDBs of the channels */
} s4t_pa;

extern s4t_pa *s4t_info[];		/* Indexed by slot(bay) number */


/* 4T INSTANCE: 
  everything you want to know about an interface i.e. port
*/
typedef struct s4t_instance_t_ {

    pas_cbtype pacb;			/* Universal PA device control blk */

    /*
     * Receive ring information
     */
    s4t_dscrptr_t *rx_beg;       /* Beginning of rx ring */
    s4t_dscrptr_t *rx_end;       /* End of rx ring */
    s4t_dscrptr_t  *rxr;		/* Rx ring pointer */
    s4t_dscrptr_t  *rxr_malloc;	/* Rx ring pointer, usable for free */
    uint rx_head, rx_tail;		/* Indexes into rx ring */
    particletype **rxr_shadow;		/* Rx ring shadow */
    paktype *rx_pak;			/* Rx temp paktype pointer */
    boolean rx_discard;      /* Discard frame, recycle bufs if true */
    ulong rx_no_enp;         /* # times no EOP in buf & OWN=0 in mp */
    int crc_size;	     /* set to four or two to adjust rx length */			
    /*
     * Transmit ring
     */
    s4t_dscrptr_t *tx_beg;       /* Beginning of tx ring */
    s4t_dscrptr_t *tx_end;       /* End of tx ring */
    s4t_dscrptr_t  *txr;	 /* Tx ring pointer */
    s4t_dscrptr_t  *txr_malloc;	 /* Tx ring pointer, usable for free */
    uint tx_head, tx_tail;	 /* Indexes into tx ring */
    uint pa_tx_head, pa_tx_tail; /* PA's Indexes into tx ring */
    uint pa_rx_head, pa_rx_tail; /* PA's Indexes into rx ring */
    uint tx_count;		 /* Count of Tx ring entries used */
    boolean tx_limited;		 /* Limit one tx pak at a time */
    s4t_tx_shadow_t *txr_shadow; /* Tx ring shadow */

    /* Throttle statistics */
    ulong throttled;             /* # times we have been throttled */
    ulong enabled;               /* # times we have been enabled */
 
    /* RX errors */
    ulong rx_cd_lost_err;        /* # carrier detect loss */

    /* TX errors */
    ulong tx_soft_buffer_err;   /* # buffer errors by driver */
    ulong tx_started;			/* # Start tx messages sent */
    ulong tx_fullring;			/* # ring full */
    boolean fatal_tx_err;       /* A fatal tx error occurred. Reset device  */

    /* 4T specific entities */
    s4t_init_block_t *init_block; /* pointer to this interface's init block */
    s4t_init_block_t *init_block_malloc;
    s4t_stats *fw_stats;		/* firmware counters */

    ushort init_block_size;     /* size in bytes of init_block */

    volatile ushort port_cmd;    /* last written port command */
    volatile ushort port_status; /* last read status */
    ushort port_status_mask;    /* status interrupt mask */
    uchar applique_id;		/* port adapter id from 5/1 adapter */
    uchar loopback;             /* loopback control for QUICC */
    volatile uchar pa_state;    /* state of PA for ENABLE_PORT cmd */

    boolean s4t_lineup;          /* TRUE or FALSE */

    s4t_pa *pa_info;			/* Per-PA table */
    ushort up_signal;                 /* PCI device bus number */

} s4t_instance_t;

/*****************************************************************
 * Macros
 *****************************************************************/
 
 /*
  * S4T_ADVANCE_TX_RING_INDEX :
  * Advance a ring index for S4T Tx ring
  */
#define S4T_ADVANCE_TX_RING_INDEX(index)		 \
     index = ((index + 1) & S4T_TX_RING_INDEX_MASK);
 
/*
 * S4T_RETRACT_TX_RING_INDEX :
 * Move an S4T ring index backwards
 */
#define S4T_RETRACT_TX_RING_INDEX(index)		\
    index = ((index - 1) & S4T_TX_RING_INDEX_MASK);
 
/*
 * S4T_ADVANCE_RX_RING_INDEX :
 * Advance a ring index for S4T Rx ring
 */
#define S4T_ADVANCE_RX_RING_INDEX(index)		 \
     index = ((index + 1) & S4T_RX_RING_INDEX_MASK);
  
/*
 * S4T_RETRACT_RX_RING_INDEX :
 * Move an S4T ring index backwards
 */
#define S4T_RETRACT_RX_RING_INDEX(index)		\
     index = ((index - 1) & S4T_RX_RING_INDEX_MASK);

/*****************************************************************
 * Static inlines
 *****************************************************************/

/*
 * s4t_rxring_flush :
 * Flush the Rx ring from the data cache
 */
static inline void s4t_rxring_flush (s4t_instance_t *ds)
{
    if (S4T_RX_RING_ATTRIBUTE == TLB_UNCACHED) {
	return;
    }
}

/*
 * s4t_flush_pak:
 * NULL out the relevant entries in the paktype
 */
static inline void s4t_flush_pak (paktype *pak)
{
    pak->datagramsize = 0;
}

/*
 * s4t_cleanup_pak:
 * Return any particles and then NULL out relevant entries in the paktype
 */
static inline void s4t_cleanup_pak (paktype *pak)
{
    particletype *mp;

    while (mp = particle_dequeue_inline(pak), mp) {
	retparticle(mp);
    }
    pak->datagramstart = NULL;
    pak->datagramsize = 0;
}

/*
 * Function prototypes
 */

/* if_pas_s4t68360.c */

extern boolean		s4t_hardware_setup(s4t_pa *);
extern void 		s4t_set_rx_discard(s4t_instance_t *, boolean);
extern void		s4t_throttle(hwidbtype *);
extern void 		s4t_rx_error(hwidbtype *, ushort, particletype *);
extern void 		s4t_tx_error(ushort, uint, hwidbtype *);
extern boolean		s4t_init(hwidbtype *);
extern void 		s4t_shutdown(hwidbtype *, int);
extern void		s4t_restart (hwidbtype *);
extern boolean          s4t_read_port_status (hwidbtype *);

extern void 		s4t_reset(hwidbtype *);
extern boolean          s4t_chg_init_param(hwidbtype *, uchar, ushort, ulong);
extern boolean          s4t_start_transmit (hwidbtype *);
extern boolean          s4t_set_maxdgram(hwidbtype *, int, int);
extern void             s4t_enable(hwidbtype *);
extern boolean          s4t_linestate(hwidbtype *);
extern void             s4t_check_pa_health(s4t_pa *);
/* need for below should be examined */
 extern boolean          s4t_show_controller(hwidbtype *, parseinfo *);



/* if_ [platform] _s4t68360.c */

extern hwidbtype	*s4t68360_idb_create(s4t_pa *, uint port);
extern void		s4t_deallocate(hwidbtype *);
extern boolean		s4t_platform_init(hwidbtype *);
extern void		s4t_show_hw_instance(s4t_instance_t *);
extern void		s4t_snmp_link_trap(int, hwidbtype *);
extern void 		s4t_lineproto_change(hwidbtype *idb, boolean);
extern boolean		s4t_init_buffer_pools(hwidbtype *);
extern void		s4t_tx_start(hwidbtype *);
extern void		s4t_fastsend(hwidbtype *, paktype *);
extern void 		s4t_clockrate_command(hwidbtype *hwidb,
					      parseinfo *csb);
extern void		s4t_periodic(hwidbtype *);
extern void 		s4t_status_int_handle(hwidbtype *hwidb);
extern void             s4t_init_clockrate(hwidbtype *hwidb);
extern void		s4t_setup_queuing(hwidbtype *hwidb);
extern void		s4t_is_really_dead_jim(s4t_pa *);
extern boolean		s4t_line_is_up(hwidbtype *hwidb);

/* belongs in src-vip */
extern boolean 		vip_send_loveletter(pas_cbtype *, 
				     uchar *, ushort);
extern particletype	*s4t_get_particle(hwidbtype *);

/* asm wrapper file */

extern pa_wrapper_t	s4t_interrupt_wrapper;

#define S4T_STATIC_INLINES_ONLY

#if defined(RVIP) || defined(SVIP)
#include "if_vip_serial_inline.h"
#include "if_vip_s4t68360.h"
#endif

#if defined(PREDATOR)
#include "if_c7100_s4t68360.h"
#endif

#undef S4T_STATIC_INLINES_ONLY
