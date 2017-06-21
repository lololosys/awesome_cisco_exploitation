/* $Id: if_c3000_tms380.h,v 3.3.62.1 1996/08/22 05:15:33 echeng Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_tms380.h,v $
 *------------------------------------------------------------------
 * if_c3000_tms380.h - C3000 specific defs for tms380 
 *
 * April 1993, Dit Morse
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_tms380.h,v $
 * Revision 3.3.62.1  1996/08/22  05:15:33  echeng
 * CSCdi62388:  Many bugs in the code handling TokenRing error counters
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/17  17:38:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:52  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/05  20:49:23  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.1  1995/06/07 21:33:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define SIF_REGS        ds->sif_regs

struct rx_slot_ptr_ {
    paktype *pak;                /* ptr to pak associated w/ this slot */
    ushort   fm_rx_space;        /* ptr to slot addr in fm+ */
    ushort   pad;
};
 
struct tx_slot_ptr_ {
    paktype *pak;                /* ptr to pak associated w/ this slot */
    ushort   fm_tx_space;        /* ptr to slot addr in fm+ */
    ushort   pad;
};


typedef struct tx_slot_ptr_  tx_slot_ptr;
typedef struct rx_slot_ptr_  rx_slot_ptr;

/**********************************************************************
 *
 * values for the number of rx/tx slots and buffers for fastmac plus
 * these numbers are platform dependent
 *
 **********************************************************************/
#define TMS380_RX_BUFS      360   /* # of rx bufs w/in the eagle */
#define TMS380_TX_BUFS      32    /* # of large tx bufs w/in the eagle */
#define TMS380_MAX_RX_SLOTS 32    /* # of rx slots */
#define TMS380_MAX_TX_SLOTS 32    /* # of tx slots */

#define TMS380_MAX_RING	    16
 

struct tms380_instance {

    struct _sifreg_t *sif_regs;	/* TMS380 register addresses              */
    ushort	reset_mask;	/* Reset bit in control register          */

    /*
     * ### Lifted from local.h ###
     * Pointers to the Madge used data structures in the TI chipsets
     * local memory.  All of these pointers are in the form 0x0001pppp.
     */
    ushort   ssb_ptr;		/* System Status Block pointer            */
    ushort   srb_ptr;		/* System Request Block pointer           */
    ushort   arb_ptr;		/* Adapter Request Block pointer          */
    ushort   stb_ptr;		/* STatus  Block pointer                  */
    ushort   ipb_ptr;		/* Initialization Parameter Block pointer */

    /*
     * Pointers to the TI MAC code data structures in the TI chipsets
     * local memory.  All of these pointers are in the form 0x0001pppp.
     */
    ushort   bia_addr;		/* internal address of bia on 380            */
    ushort   swlev_addr;	/* 380 s/w revision code                     */
    ushort   address_addr;	/* internal addr of other addresses          */
    ushort   parm_addr;		/* internal parameters                       */
    ushort   mac_buff;		/* special internal mac buffer               */
    ushort   ti_ring_speed_ptr;	/* Ptr to TI firmware's idea of ring speed   */
    ushort   ti_ring_speed;	/* TI firmware's idea of ring speed          */
    ushort   adapter_ram_ptr;	/* Ptr to total adapter number               */
    ushort   adapter_ram;	/* Amount of adapter RAM Kb from TI firmware */
    int      memory_size;	/* Size of adapter RAM from driver tests     */
    ushort   paragraphs;	/* Number of 64 Kb paragraphs of adapter RAM */

    /*
     * general control cells
     */
    scbssb_t    *scb380;	/* pointer to 380 scb and ssb                */

    /*
     * Support for source route bridging.
     */
    boolean	bridge_enabled;
    boolean	server_rem;	
    boolean	server_rps;	
    boolean	server_netmgr;	
    ulong	rng_br_rng_f;		/* forward ring-bridge-ring triplet */
    ulong	rng_br_rng_r;		/* reverse ring-bridge-ring triplet */
    ulong	max_rif;		/* maximum RIF length in bytes      */

    /*
     * Support for transparent bridging.
     */
    boolean tbridge_enabled;		/* Transparent bridging enabled  */
    uchar tbridge_addrs[MAX_ADDRS][6];	/* addresses on packets sent     */
    int tbridge_index;			/* Next slot in above table      */
    ulong spanning_violations;		/* spanning proto violations */

    /*
     * Local counters:
     */
    madge_smt_errors_t log380_errors;   /* snapshot tms380 error counts */
    madge_smt_errors_t active_errors;	/* cumulative madge error counts */
    sbe_counters_t   active_counts;	/* active counts                 */
    ushort tx_pak_count;                /* number of paks in tx ring     */
    ushort tx_pak_count_reset;          /* number of paks in ring at reset */

    /*
     * Board state
     */
    ushort	brd_flags;		/* tra flags                     */
    ushort	if_state;		/* interface state               */
    boolean	adapter_reset;		/* was reset under us            */
    long	error_log_counter;	/* count times we read error log */
    int		error_log_failures;	/* count number of times failed  */
    boolean	reset_tms380_on_fail;	/* do hard reset on next failure */

    /*
     * Debugging and crash information
     */
    ulong     debug0;                 /* Info saved from last panic     */
    ulong     debug1;
    ulong     debug2;
    ulong     debug3;
    ulong     debug4;
    ushort    sca_fail_code;          /* other info about death        */
    ushort    sca_fail;               /* catastrophic failcode         */

    /*
     * Adapter check debug info:
     */
    ulong     adapter_checks;         /* Number of adapter checks      */
    ushort    adapter_check_status;   /* Status word                   */
    ushort    parm0, parm1, parm2;    /* More information              */
    ushort    intval;                 /* information saved from error  */
    ushort    where_code;             /* code for error location       */

    /*
     * Previous error counts:
     */
    sbe_counters_t   prev_counts;	/* Previous counts */


    /*
     * System statisics.  These numbers are copied here from other locations
     * so that they will not be changing as the main system reads them.
     */
    sbe_smt_stats_t     frozen_stats;	/* Current stats from the TI chipset*/
    sbe_counters_t      frozen_counts;

    /*
     * Current interface configuration.
     */
    ushort   last_open_options;	/* how the ring was opened last time */
    uchar    bia[6];		/* default address */
    uchar    node_addr[6];	/* current address */
    ulong    group;		
    ulong    functional;	
    ushort   max_rd;		/* max rif distance ? */
    ushort   snapcodes[MAX_BRIDGE_PROTOS+1];
    ushort   llc1codes[MAX_BRIDGE_PROTOS+1];
    ushort   llc2codes[MAX_BRIDGE_PROTOS+1];
    ushort   buffersize;	/* Size of RX and TX buffers */

    /*
     * General Madge cells.
     */
    ulong    madge_kernel;
    ulong    madge_fastmac;
    ushort   madge_kernel_version;
    ushort   madge_fastmac_version;

    /*
     * Shared pointers (Madge firmware and TMS380 driver):
     */
    short rx_size, tx_size;             /* transmit & receive ring size */
    rx_slot_ptr *rx_p;              /* receive ring */
    int tms380_rx_slot;        /* pointer to the current rx slot   */
 
    tx_slot_ptr *tx_p;              /* transmit ring */
    int tms380_tx_slot;        /* pointer to the current tx slot   */
    int tms380_tx_free_slot;   /* pointer to the last free tx slot */

    /*
     * TX error counters:
     */
    long send_toobig;			/* sends too big */
    long tx_buffer_full;		/* TX buffer was full when TXing */

    /* 
     * RX error counters:
     */
    long rx_delimiter_error;             /* frame contained a CSC error */ 
    long rx_imp_error;                   /* unexpected SDEL, misalign EDEL */
    long rx_exp_error;                   /* An explicit abort SDEL-EDEL  */
    long rx_dma_overrun;
    long rx_buf_runout;                  /* Buffers exhausted */

    /*
     * process id for creating the autofix or initialisation process
     */
    int pid;
};

/*
 * Prototypes
 */
void tms380_interrupt(register hwidbtype *idb);
