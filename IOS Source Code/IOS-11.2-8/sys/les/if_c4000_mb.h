/* $Id: if_c4000_mb.h,v 3.3.62.2 1996/06/16 21:15:46 vnguyen Exp $ 
 * $Source: /release/112/cvs/Xsys/les/if_c4000_mb.h,v $
 *------------------------------------------------------------------
 * if_c4000_mb.h -- The Multi BRI/TP3420 driver defines for the 4xxx.
 *
 * March 1994, Manoj Leelanivas.
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c4000_mb.h,v $
 * Revision 3.3.62.2  1996/06/16  21:15:46  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3.62.1  1996/03/18  20:41:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:23:42  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  17:39:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:44  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:34:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prevent multiple inculsion.
 */
#ifndef __IF_C4000_MB_H__
#define __IF_C4000_MB_H__

/*
 * The following are multi bri adapter specific defines.
 */
#define MBRI_MAX_ADAPTERS_PER_SLOT  2
#define MBRI_MAX_PORTS_PER_ADAPTER  4
#define MBRI_MAX_PORTS_PER_SLOT     8

#define MBRI_ADAPTER_OFFSET         0x0100	/* offset to bri regs */
#define MBRI_ALL_DISABLE	    0x0000	/* clear */


/*
 * The register map for the bri adapter specific registers.
 *
 * Offset from slot base             Register
 * -------------------------------------------
 * 	0x0100			Interrupt enable for 8 bris in a slot.
 * 	0x0102			Interrupt status of 8 bris in a slot.
 * 	0x0104			Clock switch register for 8 bris.
 *      0x0106			Command write done register for 8 bris.
 * 	0x0108			Loop current state for 8 bris.
 * 	0x010A                  Loop current enable for 8 bris.
 * 	0x010C			Loop current change interrupt for 8 bris.
 *
 * 	0x0112			Interrupt status and cmd write for bri 1.
 * 	0x0114			Interrupt status and cmd write for bri 2.
 * 	0x0116			Interrupt status and cmd write for bri 3.
 * 	0x0118			Interrupt status and cmd write for bri 4.
 *
 *      0x0192			Interrupt status and cmd write for bri 5.
 * 	0x0194			Interrupt status and cmd write for bri 6.
 * 	0x0196			Interrupt status and cmd write for bri 7.
 * 	0x0198			Interrupt status and cmd write for bri 8.
 */

/* Multiple BRI register structure for 8 possible TP3420s*/
struct mb_block {
    volatile ushort int_enable;		/* enable for 8 possible bris */ 
    volatile ushort int_status;         /* status for 8 possible bris */
    volatile ushort clock_switch;       /* which bri drives the clock? */
    volatile ushort cmd_done;           /* to poll for cmd completion */
    volatile ushort loop_current_state; /* loop current state for 8 bris */
    volatile ushort loop_current_enable;/* loop current enable for 8 bris */
    volatile ushort loop_current_status;/* which bri's loop current changed ?*/
    volatile ushort pad1[2];   
    volatile ushort stat_reg1[MBRI_MAX_PORTS_PER_ADAPTER]; /* bri 0 - 3  */
    volatile ushort pad2[60];
    volatile ushort stat_reg2[MBRI_MAX_PORTS_PER_ADAPTER]; /* bri 4 - 7  */
};

/*
 * The following is an instance of the multi bri. This instance conatins
 * the chip/L1 specific info for 8 bris in a slot.The master clock bitmap
 * keeps track of the i/f deriving the clock. (bit 0 is 1 => i/f 0, bit
 * 8 => i/f 7). The activated bit map keeps all activated i/fs in its
 * bit fields so that any one of those i/fs can be chosen if the
 * current master goes down. The current state information keeps the
 * layer1 state information.
 */
struct mb_instance {
    mb_block_t  *mb_regs;			/* multi bri regs */
    ushort  master_clock_bitmap;		/* who is the master ? */
    ushort  activated_bitmap;			/* which i/fs activated ? */
    ulong   no_of_subunits_present;		/* current # ports (dynamic)*/
    l1_state_t current_state[MBRI_MAX_PORTS_PER_SLOT];/*current layer1 state*/
    ulong   last_sid[MBRI_MAX_PORTS_PER_SLOT];	/* last SID status */
    ulong   d_chan_coll[MBRI_MAX_PORTS_PER_SLOT];/* collision count */
    ulong   delay_4_usec;             /* pre-calibrated timer vars */
    ulong   delay_500_usec;
};

/*
 * To see/decide if an interface is the master currently/before .
 */
typedef enum {
   MASTER_TRUE = 1,
   MASTER_FALSE = 0,
   MASTER_PENDING = -1,
} master_status_t;

/*
 * The following definitions are for the timeouts used by the MBRI. These
 * are specific to the BRI and hence included here.
 */

/*
 * The 3 bri timeouts. ISDN T3 is set to 8 secs, deactivation timeout
 * is 600 ms, and the power up timeout is 3ms. Note that the actual 
 * defines are in units of 123.48 microseconds.(the tick of the on-board
 * counter)
 */
#define MBRI_T3_TIMEOUT				64787
#define MBRI_DEACT_TIMEOUT			4859
#define MBRI_PUP_TIMEOUT			24

/*
 * The following defines are for identifying each timeout and the macro
 * derives a unique identifier for each timeout of any interface of
 * any slot in the box. This identifier is used for dequeueing a
 * specific element if necessary from the deltaq.
 */
#define MBRI_PUP_TIMEOUT_TYPE 			0
#define MBRI_DEACT_TIMEOUT_TYPE 		1
#define MBRI_T3_TIMEOUT_TYPE			2
#define MBRI_NUM_TIMEOUTS			3

/*
 * The following macro is used for creating a unique identifier for
 * each element queued into the mcn deltaq. The sequence space is
 * 0 to 71. This identifier is used for deq/enq ing a particular
 * element, ie, a timeout of a bri interface in any slot, from the Q.
 */
#define MB_SEQUENCE(slot, subunit, timeout_type)\
    (slot * MBRI_MAX_PORTS_PER_SLOT * MBRI_NUM_TIMEOUTS +\
     subunit * MBRI_NUM_TIMEOUTS + timeout_type)

/*
 * Prototypes and externs needed in if_c4000_mb.h
 */
void mb_analyze_mcn_port_adaptor(int slot, int unit, int total_ctrlr_unit);
void mb_tp3420_init(hwidbtype *);
void mb_tp3420_enable_line(hwidbtype *);
void mb_tp3420_disable_line(hwidbtype *);
void mb_tp3420_deactivate_line(hwidbtype *);
void mb_tp3420_enable_loopback(hwidbtype *, enum Loopback);
void mb_set_master_clock(parseinfo *);
void mb_isdn_activate_line(hwidbtype *dchan_idb);
void mb_isdn_disable_bri_channel(hwidbtype *bchan_idb);
void mb_isdn_enable_bri_channel(hwidbtype *bchan_idb);

#endif	/* to prevent multiple inclusion */
















