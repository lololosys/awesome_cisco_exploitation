/* $Id: if_les_hd64570.c,v 3.9.12.14 1996/09/15 18:19:29 srane Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_hd64570.c,v $
 *------------------------------------------------------------------
 * if_les_hd64570.c - LES driver for Hitachi serial controller
 *
 * August 1992, Greg Stovall
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_les_hd64570.c,v $
 * Revision 3.9.12.14  1996/09/15  18:19:29  srane
 * CSCdi62516:  buffer changes lost after reload on a serial SMDS interface
 * Branch: California_branch
 *
 * Revision 3.9.12.13  1996/09/09  01:34:28  xtang
 * CSCdi56632:  unknown clock rates on 4T PA, FSIP
 * Branch: California_branch
 *         add ALT_HELPS to use csb->which in different
 *         driver procedures, and print the
 *         driver specific info. In this case, the clockrate
 *         option table, and csb->which = CFG_INT_CLOCKRATE
 *
 * Revision 3.9.12.12  1996/09/05  01:16:45  snyder
 * CSCdi68132:  declare consts in lapb x25 atm_arp tcl
 *              save 248 out of data, 12 from image
 * Branch: California_branch
 *
 * Revision 3.9.12.11  1996/08/28  12:58:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.12.10  1996/08/13  02:20:12  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.9.12.9  1996/07/23  13:25:53  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.9.12.8  1996/06/17  08:33:56  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.9.12.7  1996/06/13  19:08:15  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.9.12.6  1996/05/17  11:26:28  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.2.4  1996/05/07  21:34:39  jbalestr
 * CSCdi52473:  bsc local-ack fails link for rx frames larger than 888
 * bytes at 2400
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.2.3  1996/05/05  23:31:47  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.9.2.2  1996/04/03  19:59:12  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.2.1  1996/02/28  21:24:12  ppearce
 * CSCdi50230:  IBU modularity - Remove unnecessary header file includes
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.12.5  1996/04/27  06:33:00  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.5.12.3  1996/04/26  01:42:12  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.5.12.2  1996/04/17  03:34:49  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.5.12.1  1996/02/23  20:51:48  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.9.12.4  1996/04/17  13:43:41  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.9.12.3  1996/04/03  01:59:26  sagarwal
 * CSCdi51875:  Serial Int. MTU change doesnt get new buffers if pulsetime
 * confged
 * Branch: California_branch
 *  Compare current bufferpool size against idb maxdgram instead of
 *  relying on boolean passed to controller init to remember mtu
 *  changes across DTR pulse intervals
 *
 * Revision 3.9.12.2  1996/03/21  22:52:46  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.9.12.1  1996/03/18  20:42:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.4  1996/03/18  23:19:26  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.3  1996/02/08  08:13:17  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.2  1996/01/26  02:49:05  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.3.2.1  1995/12/11  18:56:22  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.9  1996/02/13  22:10:52  fred
 * CSCdi47956:  WFQ severely degrades RSRB performance
 *              drivers: increase TQL in short queue mode for link
 *              stability
 *              fastsrb_les.c: make sure pointers are there for queuing
 *              routines
 *
 * Revision 3.8  1996/02/13  21:26:17  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.7  1996/01/25  23:11:56  gstovall
 * CSCdi47390:  down-when-looped command not working (regression)
 * Remember to reset the itemlist before tossing it up to the next level.
 *
 * Revision 3.6.2.1  1996/01/24  22:24:13  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/01/22  06:37:38  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/07  22:30:23  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/12/06  18:31:03  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  17:40:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.14  1995/11/08  21:14:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.13  1995/10/06  02:26:14  lbustini
 * CSCdi39576:  dialer dtr does not leave DTR low permanently after idle
 * timeout
 * Raise DTR only when a call is placed.
 *
 * Revision 2.12  1995/10/04  03:33:00  jbalestr
 * CSCdi40969:  Unconfig of encap bstun causes system crash
 *
 * Revision 2.11  1995/09/07  20:37:27  arothwel
 * CSCdi39913:  BSC Tx failure on Hitachi serial chip at low baud rate.
 * Increase the transmit delay time for bisync, to allow short length
 * frames to actually get onto the wire.
 *
 * Revision 2.10  1995/09/03  01:06:40  arothwel
 * CSCdi39398:  Stop BSC crashing: abort outstanding Tx/Rx DMA ops.
 *
 * Revision 2.9  1995/08/08  19:47:22  fred
 * CSCdi38414:  congestion of compressed interfaces reduces speed.
 * when compressing data AND using WFQ/PQ/CQ, use a ring size of two, not
 * one.
 *
 * Revision 2.8  1995/07/26  06:18:55  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.7  1995/07/18  06:47:36  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.6  1995/06/23 16:39:41  fred
 * CSCdi36295:  Commit Queue MIB to 11.0
 *
 * Revision 2.5  1995/06/21  08:55:24  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.4  1995/06/21  03:01:11  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.3  1995/06/19  06:47:36  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/16  06:21:54  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  21:36:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../les/msg_hd64570.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../wan/ppp.h"
#include "../if/ether.h"
#include "../ibm/sdlc.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"
#include "../wan/dialer_registry.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "if_les.h"
#include "if_les_hd64570.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../wan/atm_dxi.h"
#include "../wan/dialer_registry.h"
#include "../if/serial_inline.h"
#include "if_les_serial.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../util/itemlist.h"
#include "../if/traffic_shape.h"


/*
 * HD64570 subsystem header
 */

#define HD64570_MAJVERSION   1
#define HD64570_MINVERSION   0
#define HD64570_EDITVERSION  1

SUBSYS_HEADER(hd64570,
	      HD64570_MAJVERSION, HD64570_MINVERSION, HD64570_EDITVERSION,
	      hd64570_subsys_init, SUBSYS_CLASS_DRIVER,
	      "seq: integrated_csu_dsu",
	      NULL);

/*
 * Clock rate table.  These values are plugged into the serial controller
 * to generate the correct clock rate.
 *
 * we have two clock rate tables now, hd64570_clock1 is old clock table for
 * 4T serial nim's version (<16). hd64570_clock2 is new clock table for
 * 4T serial nim's version (>=16) because new oscillator and fixes MSD sync
 * problem in SDLC multidrop environment.
 */

const hd64570_clocktype *hd64570_clock;

const hd64570_clocktype hd64570_clock1[NHD64570_CLOCKS] = {
    {0, 0, 0},
    {1200, 6, 130}, {2400, 5, 130}, {4800, 4, 130}, {9600, 3, 130},
    {19200, 2, 130}, {38400, 1, 130}, {56000, 1, 89}, {64000, 1, 78},
    {72000, 1, 69}, {125000, 1, 40}, {148000, 1, 34}, {250000, 1, 20},
    {500000, 1, 10}, {800000, 0, 13}, {1000000, 1, 5}, {1300000, 1, 4}, 
    {2000000, 0, 5}, {4000000, 0, 3}

};

const hd64570_clocktype hd64570_clock2[NHD64570_CLOCKS] = {
    {0, 0, 0},
    {1200, 6, 140}, {2400, 5, 140}, {4800, 4, 140}, {9600, 3, 140},
    {19200, 2, 140}, {38400, 1, 140}, {56000, 1, 96}, {64000, 1, 84},
    {72000, 0, 149}, {125000, 0, 86}, {148000, 0, 73}, {250000, 1, 20},
    {500000, 0, 22}, {800000, 0, 13}, {1000000, 0, 11}, {1300000, 0, 8}, 
    {2000000, 0, 5}, {4000000, 0, 3}

};



/*
 * hd64570_getbuf_inline
 * Get a packet buffer, initialize appropriate fields in the packet, link
 * it to the receive ring, and give ownership of the packet to the HD64570.
 * This routine is used by the receive interrupt routine and implements
 * input throttling:
 */
static inline paktype *hd64570_getbuf_inline (hwidbtype           *idb,
					      hd64570_buf_descr_t *bd,
					      boolean              cache_ok)
{
    uchar *start;
    paktype *pak = NULL;

    /*
     * Check for input congestion:
     */
    if (idb->input_qcount > idb->input_defqcount)
	return(NULL);

    /*
     * If cache_ok is set to be TRUE when the inline is used, this
      * code will be compiled in. *NO* conditional will be generated.
     */
    if (cache_ok) {
	/*
	 * Try to get a cached buffer. Else get a new buffer.
	 */
	pak = pool_dequeue_cache(idb->pool);
    }

    if (!pak)
	pak = pool_getbuffer(idb->pool);

    if (pak) {
	/*
	 * Initialize the packet encapsulation type and datagramstart:
	 */
	start = pak->data_area + idb->start_offset;
	pak->datagramstart = start;

	/*
	 * Initialize the ring entry for this packet:
	 */
        if (!bd)
	   pak->datagramsize  = 0;
	else {
	   PUTLONG((ulong *)&bd->buf_ptr, (ulong)start);
	   bd->pak = pak;
	}
    } else {
	idb->counters.output_nobuffers++;
    }
    return(pak);
}


/*
 * hd64570_getbuf
 *
 * Function call wrapper around inline function
 */

static paktype *hd64570_getbuf (hwidbtype *idb, hd64570_buf_descr_t *bd)
{
    return(hd64570_getbuf_inline(idb, bd, FALSE));
}

static paktype *hd64570_getbuf_alternate (hwidbtype *idb)
{
    return(hd64570_getbuf_inline(idb, NULL, TRUE));
}


/*
 * hd64570_RX_interrupt()
 * hd64570 receive interrupt handler
 */
void hd64570_RX_interrupt (hwidbtype *idb)
{
    hd64570_buf_descr_t *rmd;
    ushort rmd_short;
    hd64570_instance_t *ds;
    paktype *newpak;
    paktype *pak = NULL;
    ushort packets = 1;
    uchar status;

    ds = INSTANCE;

    /*
     * Clear interrupt
     */
    ds->dmac_rx_reg->dmr_dsr = DMR_NF_TMOD | HD64570_DSR_EOM |
			       HD64570_DSR_EOT | HD64570_DSR_DWE;
    ds->dmac_rx_reg->dcr_dir = HD64570_DCR_CLR_FCT |
                HD64570_DIR_BOFE | HD64570_DIR_EOME | HD64570_DIR_EOTE;

    /*
     * Receive all input frames:
     */
  next_RX_packet:
    rmd = ds->rx_last; 
    rmd_short = (ushort)((ulong)rmd & 0xFFFF);
    
    if ((rmd_short != ds->dmac_rx_reg->cda) && (packets++ < ds->rx_size)) {

        /*
         * If we have a scheduled run interval configured, go see if we need
         * to drop packets to give processes a chance to run:
         */
        if (sched_run_interval) {
            if (AWAKE(sched_run_time)) {

                /*
                 * The scheduler has not run recently enough,
                 * so, throttle the HD64570:
                 */
                idb->counters.input_drops++;
                MAYBE_DROPPED(idb);
		rmd->status = HD64570_RX_BUF_EOM;
                NEXT_READ_PACKET(ds, rmd);
            }
        }

	/*
	 * Read the descriptor information
	 */
	pak = rmd->pak;
	pak->datagramsize = rmd->data_len;
	set_if_input(pak, idb->firstsw);
        status = rmd->status & 0xFF;

	/*
	 * Check for packet errors
	 */
	if (status != HD64570_RX_BUF_EOM) {
	    if (status & HD64570_RX_BUF_SF)
		idb->counters.input_frame++;
	    
	    if (status & HD64570_RX_BUF_ABORT)
		idb->counters.input_abort++;
	    
	    if (status & HD64570_RX_BUF_RB)
		ds->residual_bit++;
	    
	    if (status & HD64570_RX_BUF_OR)
		idb->counters.input_overrun++;
	    
	    if (status & HD64570_RX_BUF_CRC)
		idb->counters.input_crc++;
	    
	    /*
	     * Drop the packet
	     */
	    rmd->status = HD64570_RX_BUF_EOM;
	    idb->counters.input_error++;
	    MAYBE_DROPPED(idb);
	    clear_if_input(pak);
	    NEXT_READ_PACKET(ds, rmd);
	}

	/*
	 * Attempt to get a new packet buffer to replace
	 * this packet buffer on the RX ring:
	 */
	newpak = hd64570_getbuf_inline(idb, rmd, TRUE);
	if (newpak) {
	    serial_RX_interrupt_inline(idb, pak);
	    NEXT_READ_PACKET(ds, rmd);
	} else {
	    /*
	     * We could not get a new big buffer.
	     * Are we an HDLC or Frame relay encapsulated serial line?
	     */
	    if ((idb->enctype == ET_HDLC) ||
		(idb->enctype == ET_FRAME_RELAY)) {
		vhdlctype *hdlc;
		/*
		 * If this packet is a high priority packet (keepalive, etc),
		 * try to get a smaller buffer big enough to copy the packet.
		 * Else, drop the packet:
		 */
		hdlc = (vhdlctype *)pak->datagramstart;
		if (serial_high_priority(hdlc->var_hdlctype)) {
		    newpak = getbuffer(pak->datagramsize);
		    if (newpak) {
			/*
			 * Queue packet for process level:
			 */
			clear_if_input(pak);

			newpak->enctype = pak->enctype;
			newpak->flags = pak->flags;
			newpak->datagramsize = pak->datagramsize;
			newpak->datagramstart = newpak->data_area +
			    (pak->datagramstart - pak->data_area);
			bcopy(pak->datagramstart, newpak->datagramstart,
			      pak->datagramsize);

			set_if_input(newpak, idb->firstsw);

			rmd->status = HD64570_RX_BUF_EOM; /* pak now owned by
							     HD64570 */
			serial_queue_for_process(idb, newpak);
			NEXT_READ_PACKET(ds, rmd);
		    }
		}
	    }
	    
	    /*
	     * We could not get a new big buffer, so drop the packet:
	     */
	    clear_if_input(pak);
	    rmd->status = HD64570_RX_BUF_EOM;
	    idb->counters.input_drops++;
	    MAYBE_DROPPED(idb);
	    NEXT_READ_PACKET(ds, rmd);
	}
    }
}


/*
 * hd64570_flush_restart_receive()
 * 
 */
static void hd64570_flush_restart_receive (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    ds = (hd64570_instance_t *)INSTANCE;
    ds->msci_reg->cmd = HD64570_CMD_RX_RST;
    ds->msci_reg->cmd = HD64570_CMD_RX_ENB;

}

/*
 * hd64570_disable_receiver()
 *
 */
static void hd64570_disable_receiver (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    ds = (hd64570_instance_t *)INSTANCE;
    ds->msci_reg->cmd = HD64570_CMD_RX_RST;
    
}    
/*
 * hd64570_syn_detected()
 *
 */
int hd64570_syn_detected (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    ds = (hd64570_instance_t *)INSTANCE;
    /* check ST1 for the Syn pattern detection flag */
    return(ds->msci_reg->st1_st0 & HD64570_ST1_FLGD);
}    


/*
 * hd64570_RX_HDX_interrupt()
 * hd64570 receive interrupt handler for half_duplex
 */
void hd64570_RX_HDX_interrupt (hwidbtype *idb)
{
    hd64570_buf_descr_t *rmd;
    ushort rmd_short;
    hd64570_instance_t *ds;
    paktype *newpak;
    paktype *pak;
    ushort packets = 1;
    uchar status;

    ds = INSTANCE;

    /*
     * Clear interrupt
     */
    ds->dmac_rx_reg->dmr_dsr = DMR_NF_TMOD | HD64570_DSR_EOM |
                               HD64570_DSR_EOT | HD64570_DSR_DWE;
    ds->dmac_rx_reg->dcr_dir = HD64570_DCR_CLR_FCT |
                HD64570_DIR_BOFE | HD64570_DIR_EOME | HD64570_DIR_EOTE;

    /*
     * Receive all input frames:
     */
  next_RX_packet:
    rmd = ds->rx_last;
    rmd_short = (ushort)((ulong)rmd & 0xFFFF);

    if ((rmd_short != ds->dmac_rx_reg->cda) && (packets++ < ds->rx_size)) {

        /*
         * If we have a scheduled run interval configured, go see if we need
         * to drop packets to give processes a chance to run:
         */
        if (sched_run_interval) {
            if (AWAKE(sched_run_time)) {

                /*
                 * The scheduler has not run recently enough,
                 * so, throttle the HD64570:
                 */
                idb->counters.input_drops++;
                MAYBE_DROPPED(idb);
                NEXT_READ_PACKET(ds, rmd);
            }
        }

	/*
	 * Read the descriptor information
	 */
	pak = rmd->pak;
	pak->datagramsize = rmd->data_len;
	set_if_input(pak, idb->firstsw);

	/*
	 * If we're sending, don't recieve
	 */
	if (idb->dte_interface) {
            if (hd64570_dte_tx_ok(ds) &&
                (ds->hdx_state == HDX64570_DTE_TX_SENDING)) {
		idb->half_duplex_violation++;
		clear_if_input(pak);
		rmd->status = HD64570_RX_BUF_EOM;
		NEXT_READ_PACKET(ds, rmd);
	    }
	} else
	    if (ds->hdx_state == HDX64570_DCE_TX) {
		idb->half_duplex_violation++;
		clear_if_input(pak);
		rmd->status = HD64570_RX_BUF_EOM;
		NEXT_READ_PACKET(ds, rmd);
	    }

	/*
	 * Check for packet errors
	 */
        status = rmd->status & 0xFF;
	if (status != HD64570_RX_BUF_EOM) {

	    if (status & HD64570_RX_BUF_SF)
		idb->counters.input_frame++;
	    
	    if (status & HD64570_RX_BUF_ABORT)
		idb->counters.input_abort++;

	    if (status & HD64570_RX_BUF_RB)
		ds->residual_bit++;
	    
	    if (status & HD64570_RX_BUF_OR)
		idb->counters.input_overrun++;

	    if (status & HD64570_RX_BUF_CRC)
		idb->counters.input_crc++;

	    /*
	     * Drop the packet
	     */
	    rmd->status = HD64570_RX_BUF_EOM;
	    idb->counters.input_error++;
	    clear_if_input(pak);
	    NEXT_READ_PACKET(ds, rmd);
	}

	/*
	 * Attempt to get a new packet buffer to replace
	 * this packet buffer on the RX ring:
	 */
	newpak = hd64570_getbuf_inline(idb, rmd, TRUE);
	if (newpak) {
	    /*
	     * All we do is SDLC. This is always tossed up to process level
	     */
	    serial_queue_for_process(idb, pak);
	    NEXT_READ_PACKET(ds, rmd);
	} else {
	    /*
	     * We could not get a new big buffer, so drop the packet:
	     */
	    rmd->status = HD64570_RX_BUF_EOM;
	    idb->counters.input_drops++;
	    clear_if_input(pak);
	    NEXT_READ_PACKET(ds, rmd);
	}
    }
}


/*
 * hd64570_tx_start()
 * Start transmitter
 */
static void hd64570_tx_start (hwidbtype *idb)
{
    hd64570_instance_t *ds = INSTANCE;
    int bytes;
    volatile hd64570_buf_descr_t *tmd;
    volatile hd64570_dmac_reg_t *hd64570_dmac_tx_reg;
    paktype *pak;
    leveltype SR;

    SR = raise_interrupt_level(NETS_DISABLE);
    hd64570_dmac_tx_reg = ds->dmac_tx_reg;

    /*
     * While there is another packet to output and room for it in the
     * hd64570 TX queue, enqueue the packet:
     */
    while ((ds->tx_count < ds->tx_size) &&
	   (pak = (*idb->oqueue_dequeue)(idb))) {

	bytes = pak->datagramsize;
	if (bytes > idb->max_pak_size) {
	    errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, bytes);
	    datagram_done(pak);
            /* BSC code expects tx complete on all frames */
	    if (idb->poll_protocol_flags != PP_ROLE_UNDEFINED) {
                (*idb->tx_comp_notify)(idb);
            } 
	    continue;
	}

	if (cpu_type == CPU_CANCUN) {
	    paktype *new_pak;
	    /*
	     * Make sure that datagramstart will not be at an odd
	     * address -  Cancun may get underruns with odd address
	     * Note that multiple protocols may be referencing this
	     * packet, thus even though there is room to just shift
	     * it to an word aligned boundry, it is correct to
	     * copy the packet to a new buffer and use that new packet
	     */
	    if ((uint)pak->datagramstart & 1) {
		new_pak = serial_fix_datagramstart(pak, idb);
		if (new_pak)
		    pak = new_pak;
		else {
		    datagram_done(pak);
                    /* BSC code expects tx complete on all frames */
	            if (idb->poll_protocol_flags != PP_ROLE_UNDEFINED) {
                       (*idb->tx_comp_notify)(idb);
                    } 
		    continue;
		}
	    }
	}
	/*
	 * Get pointers for the TX ring entry at tail:
	 */
	tmd = (hd64570_buf_descr_t *)(ds->shift_tx_cpb | 
				      (ulong)hd64570_dmac_tx_reg->eda);
	tmd->pak = pak;
	ds->tx_count++;

	idb->counters.tx_cumbytes += bytes;
	TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */
	idb->counters.outputs++;
	
	if ((pak->enctype == ET_HDLC) &&
	    (GETSHORT(pak->datagramstart) & HDLC_BCAST))
	    idb->counters.output_broad++;

	/*
	 * Set up this TX ring entry for this packet
	 */
	PUTLONG((ulong *)&tmd->buf_ptr, (ulong)(pak->datagramstart));
	tmd->data_len = bytes;
	tmd->status = HD64570_TX_BUF_EOM;

	hd64570_dmac_tx_reg->eda = tmd->chain_ptr;
    }

    /*
     * Enable DMA and transmitter
     */
    ds->msci_reg->cmd = HD64570_CMD_TX_ENB;
    hd64570_dmac_tx_reg->dmr_dsr = DMR_NF_TMOD | HD64570_DSR_DE;

    reset_interrupt_level(SR);	/* Restore interrupt level */
}


/*
 * hd64570_TX_interrupt()
 * hd64570 transmit interrupt handler
 */
void hd64570_TX_interrupt (hwidbtype *idb)
{
    hd64570_buf_descr_t *tmd;
    register hd64570_instance_t *ds;
    volatile hd64570_dmac_reg_t *hd64570_dmac_tx_reg;
    paktype *pak;


    /*
     * Process any transmitted packets:
     */
    ds = INSTANCE;
    hd64570_dmac_tx_reg = ds->dmac_tx_reg;

    /*
     * Clear interrupt
     */
    hd64570_dmac_tx_reg->dmr_dsr = DMR_NF_TMOD | HD64570_DSR_EOM |
                                   HD64570_DSR_EOT | HD64570_DSR_DWE;
    hd64570_dmac_tx_reg->dcr_dir = HD64570_DCR_CLR_FCT |
                HD64570_DIR_BOFE | HD64570_DIR_EOME | HD64570_DIR_EOTE;

    tmd = ds->tx_last;
    pak = tmd->pak;

    while (pak && (tmd->short_buf_ptr != hd64570_dmac_tx_reg->cda)) {

	/*
	 * Do packet TX accounting and clean up:
	 */
	datagram_done(pak); /* Release output packet */
	GET_TIMESTAMP(idb->lastoutput);
	tmd->pak = 0;
	ds->tx_count--;

	/*
	 * Next packet:
	 */
	tmd = tmd->next;
	pak = tmd->pak;
    }
    /*
     * Adjust the pointers
     */
    ds->tx_last = (hd64570_buf_descr_t *)tmd;
    hd64570_tx_start(idb);	/* Possibly restart output */

}


/*
 * hd64570_TX_HDX_interrupt()
 * hd64570 transmit interrupt handler for half duplex
 */
void hd64570_TX_HDX_interrupt (hwidbtype *idb)
{
    register hd64570_buf_descr_t *tmd;
    register hd64570_instance_t *ds;
    volatile hd64570_dmac_reg_t *hd64570_dmac_tx_reg;
    paktype *pak;
    volatile hd64570_reg_t *hd64570_reg;
    boolean bisync_cfgd = (idb->poll_protocol_flags != PP_ROLE_UNDEFINED);

    /*
     * Process any transmitted packets:
     */
    ds = INSTANCE;
    hd64570_dmac_tx_reg = ds->dmac_tx_reg;
    
    /*
     * Clear interrupt
     */
    hd64570_dmac_tx_reg->dmr_dsr = DMR_NF_TMOD | HD64570_DSR_EOM |
                                   HD64570_DSR_EOT | HD64570_DSR_DWE;
    hd64570_dmac_tx_reg->dcr_dir = HD64570_DCR_CLR_FCT |
                HD64570_DIR_BOFE | HD64570_DIR_EOME | HD64570_DIR_EOTE;

    tmd = ds->tx_last;
    pak = tmd->pak;

    while (pak && (tmd->short_buf_ptr != hd64570_dmac_tx_reg->cda)) {
	
	/*
	 * If last packet, drop modem signals
	 */
	if ((pak->datagramstart[1] & (uchar)SDLC_PF) || bisync_cfgd) {
	    if (idb->dte_interface) {
                /*
                 * Last DMAed frame had the PF flag set.
                 * Wait for the transmitter to start transmitting
                 * the packet.
                 */
                hd64570_reg = ds->hd64570_reg;
                ds->hdx_state = HDX64570_DTE_TX_DMA_DONE;

                if (HD64570_EVEN(idb->subunit)) {
                    hd64570_reg->timer_1_tcnt = 0;
                    if (bisync_cfgd)
                       hd64570_reg->timer_1_tconr =
			   HDX64570_BSC_TRANSMIT_DELAY;
                    else
                       hd64570_reg->timer_1_tconr = HDX64570_TRANSMIT_DELAY;
                    hd64570_reg->timer_1_tepr_tcsr = HD64570_TIMER_PRE |
                    HD64570_TCSR_ECMI | HD64570_TCSR_TME;
                } else {
                    hd64570_reg->timer_3_tcnt = 0;
                    if (bisync_cfgd)
                       hd64570_reg->timer_3_tconr =
			   HDX64570_BSC_TRANSMIT_DELAY;
                    else
                       hd64570_reg->timer_3_tconr = HDX64570_TRANSMIT_DELAY;
                    hd64570_reg->timer_3_tepr_tcsr = HD64570_TIMER_PRE |
                        HD64570_TCSR_ECMI | HD64570_TCSR_TME;
                }
	    } else
		ds->hdx_state = HDX64570_NULL;
        }

	/*
	 * Do packet TX accounting and clean up:
	 */
	datagram_done(pak); /* Release output packet */
	GET_TIMESTAMP(idb->lastoutput);
	tmd->pak = 0;
	ds->tx_count--;
	
	/*
	 * Next packet:
	 */
	tmd = tmd->next;
	pak = tmd->pak;
    }

    /*
     * Adjust the pointers
     */
    ds->tx_last = tmd;

    if (bisync_cfgd) {
      (*idb->tx_comp_notify)(idb);
    } else {
      hd64570_tx_HDX_start(idb);	/* Possibly restart output */
    }
}


/*
 * hd64570_HDX_txint(idb)
 * We get this interrupt when the transmitter goes idle.  This interrupt gets
 * enabled when we transmit a SDLC packet with the P/F set.
 */
void hd64570_HDX_txint (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    volatile hd64570_reg_t *hd64570_reg;

    ds = INSTANCE;

    /*
     * Clear the interrupt
     */
    ds->msci_reg->ie1_ie0 &= ~HD64570_IE1_IDLE;

    if (idb->dte_interface) {
        if (ds->hdx_state == HDX64570_DTE_TX_TRANSMITING) {
            /* 
             * Wait after the transmittor has gone idle instead of
             * dropping the RTS immediately.
             */
            hd64570_reg = ds->hd64570_reg;
            ds->hdx_state = HDX64570_DTE_TX_TRANSMIT_DONE;

	    if (HD64570_EVEN(idb->subunit)) {
		hd64570_reg->timer_1_tcnt = 0;
		hd64570_reg->timer_1_tconr = HDX64570_RTS_DROP_DELAY;
		hd64570_reg->timer_1_tepr_tcsr = HD64570_TIMER_PRE |
		HD64570_TCSR_ECMI | HD64570_TCSR_TME;
	    } else {
		hd64570_reg->timer_3_tcnt = 0;
		hd64570_reg->timer_3_tconr = HDX64570_RTS_DROP_DELAY;
		hd64570_reg->timer_3_tepr_tcsr = HD64570_TIMER_PRE |
		    HD64570_TCSR_ECMI | HD64570_TCSR_TME;
            }
        }
     }
}


/*
 * hd64570_DMIA_int()
 * Handle RX buffer wrap arounds
 */
void hd64570_DMIA_int (hwidbtype *idb)
{
    hd64570_instance_t *ds;

    ds = INSTANCE;

    /*
     * Check for correct interrupt
     */
    if (ds->dmac_rx_reg->dmr_dsr & HD64570_DSR_BOF) {
	/*
	 * Clear interrupt and restart RX
	 */
	if (serial_debug)
	    buginf("\nHD(%d): Input buffer overflow.\n", idb->unit);

        /*
         * Increment the "ignore" counter for this channel
         */
        idb->counters.input_error++;
        idb->counters.input_resource++;

	hd64570_RX_interrupt(idb);
        ds->dmac_rx_reg->dmr_dsr = DMR_NF_TMOD |
                                   HD64570_DSR_BOF | HD64570_DSR_DE;
    }
}


/*
 * hd64570_error_int()
 * Handle TX/RX errors
 */
void hd64570_error_int (hwidbtype *idb)

{
    register hd64570_buf_descr_t *tmd;
    register hd64570_instance_t *ds; 
    paktype *pak;


    ds = INSTANCE;
    tmd = ds->tx_last;
    pak = tmd->pak;

    /*
     * Clear interrupt
     */
    ds->msci_reg->st1_st0 = HD64570_ST1_UDRN;

    if (serial_debug)
        buginf("\nTX udrn err!!! DMR_DSR=0x%x, ISR1_ISR0=0x%x, ST3_ST2=0x%x\n",               ds->dmac_tx_reg->dmr_dsr, ds->hd64570_reg->isr1_isr0,
               ds->msci_reg->st3_st2);

    /*
     * Clean up and continue
     */
    idb->counters.output_underrun++;
    while (pak && (tmd->short_buf_ptr != ds->dmac_tx_reg->cda)) {
        datagram_done(pak); /* Release output packet */

        /* BSC code expects tx complete on all frames */
	if (idb->poll_protocol_flags != PP_ROLE_UNDEFINED) {
            (*idb->tx_comp_notify)(idb);
        } 
	GET_TIMESTAMP(idb->lastoutput);
	tmd->pak = 0;
	ds->tx_count--;

	ds->tx_last = tmd->next;
	tmd = tmd->next;
	pak = tmd->pak;
    }

    hd64570_tx_start(idb);	/* Possibly restart output */
}


/*
 * hd64570_tx_HDX_start()
 *
 * Start transmitter
 * since BSC uses the same queue as BSTUN for the transmit frames there is
 * a small problem because BSC needs to fiddle the frames at the start of the
 * queue before passing them on
 */
void hd64570_tx_HDX_start (hwidbtype *idb)
{
    hd64570_instance_t *ds = INSTANCE;
    int bytes;
    hd64570_buf_descr_t *tmd;
    volatile hd64570_dmac_reg_t *hd64570_dmac_tx_reg;
    paktype *pak;
    leveltype SR;

    SR = raise_interrupt_level(NETS_DISABLE);
    hd64570_dmac_tx_reg = ds->dmac_tx_reg;

    /*
     * While there is another packet to output and room for it in the
     * hd64570 TX queue, enqueue the packet:
     */
    while ((ds->tx_count < ds->tx_size) &&
	   (pak = (*idb->oqueue_dequeue)(idb))) {

	/*
	 * See if we can transmit
	 */
	if (idb->dte_interface) {
            if ((ds->hdx_state == HDX64570_DTE_TX) ||
                (ds->hdx_state == HDX64570_DTE_TX_SENDING)) {
                ds->hdx_state = HDX64570_DTE_TX_SENDING;
	    } else {
	        if (!hd64570_dte_tx_request(idb)) { 
		    pak_requeue_head(idb, pak);
		    ds->msci_reg->cmd = HD64570_CMD_TX_ENB;
                    hd64570_dmac_tx_reg->dmr_dsr = DMR_NF_TMOD |
			HD64570_DSR_DE;
		    reset_interrupt_level(SR);	/* Restore interrupt level */
		    return;
                } else 
		    ds->hdx_state = HDX64570_DTE_TX_SENDING;
	    }
	} else {
	    if (hd64570_dce_tx_ok(ds)) {
		ds->hdx_state = HDX64570_DCE_TX;
	    } else {
		pak_requeue_head(idb, pak);
		ds->msci_reg->cmd = HD64570_CMD_TX_ENB;
                hd64570_dmac_tx_reg->dmr_dsr = DMR_NF_TMOD | HD64570_DSR_DE;
		reset_interrupt_level(SR);	/* Restore interrupt level */
		return;
	    }
        }
	
	bytes = pak->datagramsize;
	if (bytes > idb->max_pak_size) {
	    errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, bytes);
	    datagram_done(pak);
            /* BSC code expects tx complete on all frames */
	    if (idb->poll_protocol_flags != PP_ROLE_UNDEFINED) {
                (*idb->tx_comp_notify)(idb);
            } 
	    continue;
	}

	/*
	 * Get pointers for the TX ring entry at tail:
	 */
	tmd = (hd64570_buf_descr_t *)(ds->shift_tx_cpb | 
				      (ulong)hd64570_dmac_tx_reg->eda);
	tmd->pak = pak;
	ds->tx_count++;
	
	idb->counters.tx_cumbytes += bytes;
	TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */
	idb->counters.outputs++;
	
	/*
	 * Set up this TX ring entry for this packet
	 */
	PUTLONG((ulong *)&tmd->buf_ptr, (ulong)pak->datagramstart);
	tmd->data_len = bytes;
	tmd->status = HD64570_TX_BUF_EOM;
	
	hd64570_dmac_tx_reg->eda = tmd->chain_ptr;
    }

    /*
     * Enable DMA and transmitter
     */
    ds->msci_reg->cmd = HD64570_CMD_TX_ENB;
    hd64570_dmac_tx_reg->dmr_dsr = DMR_NF_TMOD | HD64570_DSR_DE;

    reset_interrupt_level(SR);	/* Restore interrupt level */
}


/*
 * hd64570_restart()
 * Start transmitter - adds wrapper for bisync
 */
static void hd64570_restart (hwidbtype *idb)
{
    leveltype SR;

    /*
     * bsc needs to do hd64570_restart only once upon
     * restart and is called from interface going up code
     */
    if (idb->poll_protocol_flags == PP_ROLE_UNDEFINED) {
	
	SR = raise_interrupt_level(NETS_DISABLE);
	
	holdq_reset(idb);
	if (idb->state != IDBS_ADMINDOWN)
	    hd64570_init(idb);
	
	reset_interrupt_level(SR);
    } else
	hd64570_init(idb);

    idb->counters.resets++;
}


/*
 * hd64570_shutdown()
 * Shutdown transmitter
 */
static void hd64570_shutdown (hwidbtype *idb, int newstate)
{
    hd64570_instance_t *ds = INSTANCE;
    leveltype SR;
    volatile ushort tmp;

    net_cstate(idb, newstate);

    SR = raise_interrupt_level(NETS_DISABLE);

    hd64570_disable_modem_interrupt(idb);
    hd64570_msci_reset(idb);
    HD64570_PLATFORM_READ_PORT_ID(idb);

    if (idb->hdx_flags & HDX_FLAG_ON) {
	/*
	 * Disable, clear the half duplex interrupts.
	 */
	if (HD64570_EVEN(idb->subunit)) {
	    ds->hd64570_reg->ier2 &= 
		~(HD64570_IER2_T0IRQE | HD64570_IER2_T1IRQE);
	    ds->hd64570_reg->timer_0_tepr_tcsr = 0;
	    tmp = ds->hd64570_reg->timer_0_tepr_tcsr;
	    tmp = ds->hd64570_reg->timer_0_tcnt;
	    
	    ds->hd64570_reg->timer_1_tepr_tcsr = 0;
	    tmp = ds->hd64570_reg->timer_1_tepr_tcsr;
	    tmp = ds->hd64570_reg->timer_1_tcnt;
	} else {
	    ds->hd64570_reg->ier2 &=
		~(HD64570_IER2_T2IRQE | HD64570_IER2_T3IRQE);
	    ds->hd64570_reg->timer_2_tepr_tcsr = 0;
	    tmp = ds->hd64570_reg->timer_2_tepr_tcsr;
	    tmp = ds->hd64570_reg->timer_2_tcnt;
	    
	    ds->hd64570_reg->timer_3_tepr_tcsr = 0;
	    tmp = ds->hd64570_reg->timer_3_tepr_tcsr;
	    tmp = ds->hd64570_reg->timer_3_tcnt;
	}
    }
    reset_interrupt_level(SR);
}


/*
 * hd64570_clockrate()
 * Set the DCE clock rate. 
 */
static void hd64570_clockrate_command (parseinfo *csb)

{
    hwidbtype *idb = csb->interface->hwptr;
    int  rate = 0;
    int i;
    serialsb *ssb;

    if (is_g703(idb)) {
        if (csb->resolvemethod == RES_MANUAL)
	    printf("\nClockrate is ignored on G.703 interfaces.");
	return;
    }

#ifdef DEBUG
    buginf("HD64570(%d): hd64570_clockrate: idb = 0x%x\n", idb->unit, idb);
#endif

    if (csb->sense) {
        rate = GETOBJ(int,1);
    }

    if ((csb->which == CFG_INT_CLOCKRATE) && (rate == 0)) {
        printf("\n  \t%s\n", "Speed (bits per second)");

	for (i = 1; i < NHD64570_CLOCKS; i++) {
	  printf("  %d\n", hd64570_clock[i].rate);
	}
	return;
    }

    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);

    if (csb->nvgen) {
        if (!idb->dte_interface) {
            nv_write(ssb->serial_clockindex, "%s %d", csb->nv_command,
                     hd64570_clock[ssb->serial_clockindex].rate);
        }
	idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
        return;
    }

    /*
     * Scan up through the array looking for the
     * divider index
     */
    for (i = 0; i < NHD64570_CLOCKS; i++) {
	if (rate == hd64570_clock[i].rate) {
	    break;
	}
    }
    
    if (i == NHD64570_CLOCKS) {
	printf("\nunknown clock rate");
	idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
	return;
    }
    
    /*
     * Always store the clock index irregardless of
     * whether the cable is DTE or DCE
     */
    ssb->serial_clockindex = i; 
    
    if (!idb->dte_interface || !csb->sense) {
	/*
	 * Reset the interface for the clockrate to take
	 * effect
	 */
	hd64570_init(idb);
    }
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
}


/*
 * hd64570_periodic()
 * Poll for HD64570 reported errors and ensure that HD64570 is running
 */
static void hd64570_periodic (hwidbtype *idb)
{
    hd64570_instance_t *ds;
    leveltype SR;

    ds = INSTANCE;

    /*
     * Check to see if we're getting pinned by modem
     * control interrupts
     */
    if (ds->flap_enabled) {
	/*
	 * We've got a runaway interface. Choo Choo!!
	 */
	if (!ds->flap_backoff)
	    errmsg(&msgsym(LINEFLAP, HD), idb->unit);
	
	if (++ds->flap_backoff > SERIAL_FLAP_BACKOFF) {
	    /*
	     * We're finished with the probation period
	     */
	    ds->flap_count = 0;
	    TIMER_STOP(ds->flap_time);
	    ds->flap_backoff = 0;
	    ds->flap_enabled = FALSE;
	    
	    hd64570_enable_modem_interrupt(idb);
	}
    }	
    
    /*
     * Check to make sure this MSCI is running correctly
     */
    if (idb->state == IDBS_UP && !reg_invoke_dialer_spoofing_int(idb)) {
	if (ds->appl_periodic) {
	    /*
	     * Call applique specific periodic routine.
	     */
	    SR = raise_interrupt_level(NETS_DISABLE);
	    (*ds->appl_periodic)(idb);
	    reset_interrupt_level(SR);
	}
    }
}


/*
 * hd64570_txqlength()
 * Returns the number of packets on the TX ring
 */
static int hd64570_txqlength (hwidbtype *idb)
{
    hd64570_instance_t *ds = INSTANCE;

    return (ds->tx_count);
}


/*
 * hd64570_set_maxdgram()
 * Start transmitter
 */
static boolean hd64570_set_maxdgram (hwidbtype *idb, int buffersize,
				     int maxdgram)
{
    if_maxdgram_adjust(idb, maxdgram);

    /*
     * Initialize the interface.
     */
    hd64570_init(idb);
    idb->counters.resets++;
    return(TRUE);
}


/*
 *  hd64570_set_rxoffset
 * Set receive offset
 */
static void hd64570_set_rxoffset (hwidbtype *idb)
{
    hd64570_init(idb);
}


/*
 * hd64570_show_errors()
 * Show HD64570 specfic errors
 */
static void hd64570_show_errors (hwidbtype *idb)

{
    hd64570_instance_t *ds = INSTANCE;

    printf("%d residual bit errors\n", ds->residual_bit);
}


/*
 * hd64570_fastsend()
 * Attempt to transmit this packet immediately.
 * Called from interrupt level.
 */
static void hd64570_fastsend (hwidbtype *idb, paktype *pak)
{
    hd64570_instance_t *ds;
    hd64570_buf_descr_t * tmd;
    volatile hd64570_dmac_reg_t *hd64570_dmac_tx_reg;
    paktype *tx_pak;
    ushort bytes;
    traffic_shape_t *shape;

    if (idb->state != IDBS_UP) {
	idb->counters.output_total_drops++;
	datagram_done(pak);
	return;
    }

    bytes = pak->datagramsize;

    if (cpu_type == CPU_CANCUN) { 
	paktype *new_pak;
	/*
	 * Make sure that datagramstart will not be at an odd
	 * address -  Cancun may get underruns with odd address
	 * Note that multiple protocols may be referencing this
	 * packet, thus even though there is room to just shift
	 * it to an word aligned boundry, it is correct to
	 * copy the packet to a new buffer and use that new packet
	 */
	if ((uint)pak->datagramstart & 1) {
	    new_pak = serial_fix_datagramstart(pak, idb);
	    if (new_pak)
		pak = new_pak;
	    else {
		retbuffer(pak);
		return;
	    }
	}
    }

    /*
     * determine whether there is window available to send the message with
     * in the rate control bucket
     */
    shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
    if (shape) {
        traffic_shape_defer_sending(shape, pak);
        return;
    }
    
    ds = INSTANCE;

    /*
     * If no packets are queued for output on this interface,
     * immediately transmit the packet:
     */
    if (!idb->output_qcount) {
	/*
	 * Get pointer for the TX ring entry:
	 */
	hd64570_dmac_tx_reg = ds->dmac_tx_reg;
	
	/*
	 * Is there already room for another packet in the output ring?
	 */
	if (ds->tx_count < ds->tx_size) {
	    /*
	     * Transmit our packet:
	     */

	    /*
	     * Set up this TX ring entry for this packet and
	     * force TX ring polling:
	     */
	    tmd = (hd64570_buf_descr_t *)(ds->shift_tx_cpb | 
					  (ulong)hd64570_dmac_tx_reg->eda);
	    tmd->pak = pak;
	    PUTLONG((ulong *)&tmd->buf_ptr, (ulong)pak->datagramstart);
	    tmd->data_len = bytes;
	    tmd->status = HD64570_TX_BUF_EOM;

	    hd64570_dmac_tx_reg->eda = tmd->chain_ptr;

	    /*
	     * Enable DMA
	     */
	    ds->msci_reg->cmd = HD64570_CMD_TX_ENB;
            hd64570_dmac_tx_reg->dmr_dsr = DMR_NF_TMOD | HD64570_DSR_DE;

	    /*
	     * Do packet accounting after starting the HD64570:
	     */
	    ds->tx_count++;
	    idb->counters.tx_cumbytes_fast[pak->acct_proto] += bytes;
	    idb->counters.outputs_fast[pak->acct_proto]++;
	    TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */
	    if ((idb->enctype == ET_HDLC) &&
		(GETSHORT(pak->datagramstart) & HDLC_BCAST))
		idb->counters.output_broad++;
	    return;
	} else {
            /*
             * We have a full TX ring. Is the first packet in the ring done?
             * If so, we can service the TX interrupt on this packet and use
             * the ring entry for our new packet:
             */
            tmd = ds->tx_last;
            tx_pak = tmd->pak;
            if (tx_pak && (tmd->short_buf_ptr != hd64570_dmac_tx_reg->cda)) {
                /*
                 * Do packet TX accounting and clean up:
                 */
                datagram_done(tx_pak); /* Release output packet */
                GET_TIMESTAMP(idb->lastoutput);
                tmd->pak = 0;
                ds->tx_last = tmd->next;

                /*
                 * Now, we can transmit our packet, set up this TX ring
                 * entry for this packet and force TX ring polling.
                 */
                tmd = (hd64570_buf_descr_t *)(ds->shift_tx_cpb |
                                          (ulong)hd64570_dmac_tx_reg->eda);
                tmd->pak = pak;
                PUTLONG((ulong *)&tmd->buf_ptr, (ulong)pak->datagramstart);
                tmd->data_len = bytes;
                tmd->status = HD64570_TX_BUF_EOM;
                hd64570_dmac_tx_reg->eda = tmd->chain_ptr;

                /*
                 * Enable DMA
                 */
                ds->msci_reg->cmd = HD64570_CMD_TX_ENB;
                hd64570_dmac_tx_reg->dmr_dsr = DMR_NF_TMOD | HD64570_DSR_DE;

                /*
                 * Do packet accounting after starting the HD64570:
                 */
                idb->counters.tx_cumbytes_fast[pak->acct_proto] += bytes;
                idb->counters.outputs_fast[pak->acct_proto]++;
                TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */
                if ((idb->enctype == ET_HDLC) &&
                    (GETSHORT(pak->datagramstart) & HDLC_BCAST))
                idb->counters.output_broad++;
                return;
	    } else {
		/*
		 * We have a full TX ring, so queue the packet in the output
		 * hold queue.
		 */
		holdq_enqueue_fast(idb, pak);
		hd64570_TX_interrupt(idb);
		return;    
	    }
        }
    } else {
	/*
	 * There are already packets on the output hold queue,
	 * so add this one to the end of the queue to minimize
	 * out of order situations:
	 */
	holdq_enqueue_fast(idb, pak);

	/*
	 * If the software thinks the TX ring is full, go check with
	 * the Hitachi and see if we can clear some space before pulling
	 * frames off of the holdq by faking a TX interrupt. The TX
	 * interrupt routine will call hd64570_tx_start() for us.
	 *
	 * This causes a smoother saturation roll-off
	 */
	if (ds->tx_count == ds->tx_size)
	    hd64570_TX_interrupt(idb);
	else
	    hd64570_tx_start(idb);
    }
}


/*
 * hd64570_show_buf()
 * Show a Receive ring entry:
 */
static void hd64570_show_buf (hd64570_buf_descr_t *bd, int n)
{
    ulong data_start;

    data_start = GETLONG((ulong *)&bd->buf_ptr);

    printf("%02d bd_ptr=0x%04x pak=0x%06x ds=0x%06x status=%02x pak_size=%d\n",
	   n, bd->short_buf_ptr, bd->pak, data_start, bd->status,
	   bd->data_len);

    if (bd->pak && (data_start == 0))
	printf("Error: datagramstart = NULL!!!\n");
}


/*
 * hd64570_show_controller()
 */
static boolean hd64570_show_controller (hwidbtype *idb, parseinfo *csb)
{
    hd64570_instance_t *ds = INSTANCE;
    int i;
    hd64570_buf_ring_t *hd64570_rx_ring, *hd64570_tx_ring;
    leveltype SR;

    printf("HD unit %d, ", idb->unit);

    hd64570_show_structure_info(idb);
    printf("buffer size %d  ", idb->max_buffer_size);
    
    /*
     * Show the cable related info.
     */
    hd64570_show_cable_state(idb);
    
    /*
     * Allocate memory for the RX and TX rings:
     */
    hd64570_rx_ring = malloc(HD64570_BUF_SIZE);
    if (!hd64570_rx_ring) {
	free(hd64570_rx_ring);
	return(TRUE);
    }
    hd64570_tx_ring = malloc(HD64570_BUF_SIZE);
    if (!hd64570_tx_ring) {
	free(hd64570_tx_ring);
	return(TRUE);
    }
    
    /*
     * Disable network interrupts and take a snapshot of the ring state:
     */
    SR = raise_interrupt_level(NETS_DISABLE);
    bcopy(ds->rx_ra, hd64570_rx_ring,
	  ((MAX_RING + 1) * sizeof(hd64570_buf_descr_t)));
    bcopy(ds->tx_ra, hd64570_tx_ring,
	  (MAX_RING * sizeof(hd64570_buf_descr_t)));
    reset_interrupt_level(SR);
    
    printf("cpb = 0x%x, eda = 0x%x, cda = 0x%x\n", 
	   ds->dmac_rx_reg->cpb & 0xff, 
	   ds->dmac_rx_reg->eda, ds->dmac_rx_reg->cda);
    printf("RX ring with %d entries at 0x%x\n", ds->rx_size,
	   ds->rx_ra);
    
    for (i = 0; i <= ds->rx_size; i++) {
	hd64570_show_buf((hd64570_buf_descr_t *)
			 &hd64570_rx_ring->ring_entry[i], i);
    }
    free(hd64570_rx_ring);
    
    printf("cpb = 0x%x, eda = 0x%x, cda = 0x%x\n", 
	   ds->dmac_tx_reg->cpb & 0xff, 
	   ds->dmac_tx_reg->eda, ds->dmac_tx_reg->cda);
    printf("TX ring with %d entries at 0x%x\n", ds->tx_size, 
	   ds->tx_ra);
    
    for (i = 0; i <= ds->tx_size; i++) {
	hd64570_show_buf((hd64570_buf_descr_t *)
			 &hd64570_tx_ring->ring_entry[i], i);
    }
    free(hd64570_tx_ring);
    
    les_serial_show_errors(idb);

    hd64570_show_errors(idb);
    
    return(FALSE);
}


/*
 * hd64570_init()
 * Initialize the interface
 */
void hd64570_init (hwidbtype *idb)
{
    SAVE_CALLER();
    hd64570_buf_descr_t	*bd;
    hd64570_buf_ring_t	*bdring;
    hd64570_instance_t	*ds;
    volatile hd64570_reg_t *hd64570_reg;
    volatile hd64570_msci_reg_t *msci_reg;
    volatile hd64570_dmac_reg_t *dmac_rx_reg, *dmac_tx_reg; 
    paktype *pak;
    leveltype SR;
    int i;
    int rx_ring_size, old_rx_ring_size = 0, tx_ring_size;
    serialsb *ssb;
    boolean maxdgram_change = FALSE;

    SR = raise_interrupt_level(NETS_DISABLE);
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    MAYBE_DROPPED(idb);    /* May lose output from reset */

    /*
     * initialise the idb vectors for hd chip set things
     */
    idb->restart_rx = hd64570_flush_restart_receive ;
    idb->disable_rx = hd64570_disable_receiver ;
    idb->getsysbuf  = hd64570_getbuf_alternate;
    idb->rx_in_progress = hd64570_syn_detected;
 
    /*
     * Has the MTU changed? If so, we need to get a new
     * ring of receive buffers
     */
    if (idb->pool && (idb->max_buffer_size != idb->pool->size))
	maxdgram_change = TRUE;
 
    /*
     * Reset this MSCI on the controller
     */
    ds = INSTANCE;

    hd64570_reg = (hd64570_reg_t *)ds->hd64570_reg;
    msci_reg = ds->msci_reg;

    hd64570_msci_reset(idb);
    
    /*
     * Change state to reset
     */
    if (idb->state != IDBS_ADMINDOWN)
	net_cstate(idb, IDBS_RESET);
    
    if (serial_debug)
	buginf("\nHD(%d): Reset from 0x%x", idb->unit, caller());
    
    /*
     * Flush the hold queue
     */
    holdq_reset(idb);
    
    if (!serial_pulse_DTR(idb)) {
	reset_interrupt_level(SR);
	return;
    }
    
    /*
     * If a DTR dialer is configured on this interface we may not want
     * to raise DTR, so return here if this is the case.
     */
    if (is_ddr(idb)) {
	if (reg_invoke_dialer_keep_dtr_low(idb)) {
	    reset_interrupt_level(SR);
	    return;
	}
    }

    /*
     * Figure out what encapsulation we are configured for and cache
     * the encapsulation type and datagramstart offset for hd64570_getbuf:
     */
    pak = 0;
    les_serial_init_offset(idb);
    
    /*
     * if the encaps is anything but BSTUN the poll_protocl_route
     * are invalid.
     */
    if (idb->enctype != ET_BSTUN) {
	idb->poll_protocol_flags = PP_ROLE_UNDEFINED;
    }

    if ( idb->poll_protocol_flags == PP_ROLE_UNDEFINED ) {
	
	/*
	 * Initialize the variables used for RX & TX ring access
	 */
	old_rx_ring_size = ds->rx_size;
	select_ring_size(&rx_ring_size, &tx_ring_size, idb->max_buffer_size);

        /* adjust rx_ring_size for SMDS encapsulation */
        if (idb->enctype == ET_SMDS)
            adj_smds_ring_size(&rx_ring_size);

	ds->rx_size = make_ring_size(rx_ring_size);
	if (ds->rx_size == 0) {
	    errmsg(&msgsym(BADRING, HD), "receive");
	    crashdump(0);
	}
    }

    ds->tx_count = 0;
    if ((hwidb_use_short_tx_q_inline(idb))
	|| (idb->poll_protocol_flags != PP_ROLE_UNDEFINED)) {
	ds->tx_size = make_ring_size(2);
    } else {
	ds->tx_size = make_ring_size(tx_ring_size);
    }
    idb->mci_txcount = ds->tx_size;
    if (ds->tx_size == 0) {
	errmsg(&msgsym(BADRING, HD), "transmit");
	crashdump(0);
    }
    
    /*
     * Set up the RX ring and ensure that it is full of buffers owned 
     * by the HD64570.
     *
     * If the MTU is changing, free all current buffers, and require
     * buffers of the new size. Otherwise, just give ownership of
     * the buffers back to the HD64570:
     */
    bdring = ds->rx_ra;
    
    /*
     * Setup hd64570 registers
     */
    dmac_rx_reg = ds->dmac_rx_reg;
    
    dmac_rx_reg->cpb = (uchar)(((ulong)bdring >> 16) & 0xFF);
    dmac_rx_reg->eda = (ushort)((ulong)&bdring->ring_entry[ds->rx_size]) &
	0xFFFF;
    dmac_rx_reg->cda = (ushort)((ulong)bdring & 0xFFFF);
    dmac_rx_reg->bfl = idb->max_buffer_size;
    
    ds->rx_last = (hd64570_buf_descr_t *)bdring;
    
    /*
     * If a private buffer pool for this interface exists AND there
     * has been an MTU change, discard the pool with the old-sized
     * buffers. Also discard any buffers in the old-sized receive ring.
     */
    if (idb->pool && maxdgram_change) {
	idb->pool = pool_resize(idb->pool, idb->max_buffer_size, 0,
                                PRIV_NUM_BUF(ds->rx_size),
                                PRIV_NUM_BUF(ds->rx_size));
        if (!idb->pool)
            crashdump(0);
        
	for (i = 0; i < old_rx_ring_size; i++) {
	    bd = &bdring->ring_entry[i];
	    bd->data_len = 0;
	    /*
	     * Does this RX ring entry already have a packet?
	     */
	    if (bd->pak) {
		pak = bd->pak;
		
		/* Yes.  Get rid of it. */
		discard_buffer(pak);
		bd->pak = 0;
	    }
	}
    }
    
    /* 
     * If the private buffer pool has not been allocated yet,allocate it now.
     */

    if (!idb->pool) {
	if (idb->pool_group == POOL_GROUP_INVALID)
	    idb->pool_group = pool_create_group();
	

	if (!idb->hw_namestring)
	    idb_init_names(idb->firstsw, FALSE);
        idb->pool = pak_pool_create(idb->hw_namestring, idb->pool_group,
                                    idb->max_buffer_size, POOL_SANITY,
                                    NULL);
	if (!idb->pool) {
	    crashdump(0);
        }
        /*
         * Populate the pool with buffers
         */
        pool_adjust(idb->pool, 0, PRIV_NUM_BUF(ds->rx_size),
                    PRIV_NUM_BUF(ds->rx_size), TRUE);
    }
    /*
     * If required, set the fallback pool
     */
    if (!idb->pool->fallback)
        idb->pool->fallback = pak_pool_find_by_size(idb->max_buffer_size);
    /*
     * If required, create a buffer cache
     */
    if (!(idb->pool->flags & POOL_CACHE)) {
         pak_pool_create_cache(idb->pool, PRIV_NUM_CACHE(ds->rx_size));
         pool_adjust_cache(idb->pool, PRIV_NUM_CACHE(ds->rx_size));
    }
    for (i = 0; i <= ds->rx_size; i++) {
	bd = &bdring->ring_entry[i];
	bd->data_len = 0;
	
	/*
	 * Set up chain pointer
	 */
	if (i == (ds->rx_size)){
	    bd->chain_ptr = (ushort)((ulong)&(bdring->ring_entry[0]) &
				      0xFFFF);
	    bd->next = (hd64570_buf_descr_t *)&(bdring->ring_entry[0]);
	} else {
	    bd->chain_ptr = (ushort)((ulong)&(bdring->ring_entry[i+1]) &
				      0xFFFF);
	    bd->next = (hd64570_buf_descr_t *)&(bdring->ring_entry[i+1]);
	}
	
	bd->short_buf_ptr = (ushort)((ulong)bd & 0xFFFF);

	/*
	 * Does this RX ring entry already have a packet?
	 */
	if (bd->pak) {
	    pak = bd->pak;
	    
	    /*
	     * Yes, there is a packet. Is the MTU changing?
	     * Are we now configured BSC?
	     */
	    if ((maxdgram_change) ||
		(idb->poll_protocol_flags != PP_ROLE_UNDEFINED)) {
		/*
		 * Yes.  Discard our current buffers
		 */
		discard_buffer(pak);
		bd->pak = 0;
	    } else {
		/*
		 * No. Give this packet back to the hd64570
		 */
		pak->datagramstart = pak->data_area + idb->start_offset;
		PUTLONG((ulong *)&bd->buf_ptr, (ulong)pak->datagramstart);
		bd->status = HD64570_RX_BUF_EOM;
	    }
	}

	/*
	 * If this ring entry doesn't have a packet yet, get one:
	 */
	if (!bd->pak) {
            if (idb->poll_protocol_flags == PP_ROLE_UNDEFINED) {
		pak = hd64570_getbuf(idb, bd);
		if (!pak) {
		    /*
		     * If we could not get a packet, mark our interface as
		     * down so we try again later:
		     */
		    net_cstate(idb, IDBS_DOWN);
		    reset_interrupt_level(SR);
		    return;
		}
	    }
	}
    }

    /*
     * Set up the TX buffer ring and say that we own all of the
     * buffers so that the HD64570 does not try to transmit any of them:
     */
    bdring = ds->tx_ra;
    ds->tx_last = (hd64570_buf_descr_t *)bdring;

    /*
     * Setup hd64570 registers
     */
    dmac_tx_reg = ds->dmac_tx_reg;

    ds->shift_tx_cpb = ((ulong)bdring & 0xFFFF0000);
    dmac_tx_reg->cpb = (uchar)(((ulong)bdring >> 16) & 0xFF);
    dmac_tx_reg->eda = (ushort)((ulong)bdring & 0xFFFF);
    dmac_tx_reg->cda = (ushort)((ulong)bdring & 0xFFFF);

    for (i = 0; i <= ds->tx_size; i++) {
	/*
	 * Initialize this TX ring entry:
	 */
	bd = &bdring->ring_entry[i];
	if (i == (ds->tx_size)) {
	    bd->chain_ptr = (ushort)((ulong)&(bdring->ring_entry[0]) &
				      0xFFFF);
	    bd->next = (hd64570_buf_descr_t *)&(bdring->ring_entry[0]);
	} else {
	    bd->chain_ptr = (ushort)((ulong)&(bdring->ring_entry[i+1]) &
				      0xFFFF);
	    bd->next = (hd64570_buf_descr_t *)&(bdring->ring_entry[i+1]);
	}
	PUTLONG((ulong *)&bd->buf_ptr, 0);
	bd->data_len = 0;
	bd->status = HD64570_TX_BUF_EOM;
	bd->short_buf_ptr = (ushort)((ulong)bd & 0xFFFF);
	
	/*
	 * If this ring entry had a packet, free it:
	 */
	if (bd->pak) {
	    pak = bd->pak;
	    datagram_done(pak);
	    bd->pak = 0;

            /* BSC code expects tx complete on all frames */
	    if (idb->poll_protocol_flags != PP_ROLE_UNDEFINED) {
                (*idb->tx_comp_notify)(idb);
            } 
	}
    }
    
    /*
     * If the interface is administratively down, don't re-enable it.  Note
     * that we perform this check after shutting down the MSCI on the HD64570
     * and setting up the rx and tx rings.
     */
    if (idb->state == IDBS_ADMINDOWN) {
        reset_interrupt_level(SR);
        return;
    }

    /*
     * Setup soutput and other HDX stuff
     */
    if (idb->hdx_flags & HDX_FLAG_ON) {
        idb->soutput = hd64570_tx_HDX_start;
	ds->hdx_state = HDX64570_NULL;
    }
    else {
        idb->soutput = hd64570_tx_start;
        if (idb->poll_protocol_flags != PP_ROLE_UNDEFINED)
            ds->hdx_state = HDX64570_NULL;
    }

    /*
     * Select the proper assembly language level 4 interrupt wrapper:
     */
    hd64570_setup_int(idb);

    /*
     * Initialize the wait controller
     */
    hd64570_reg->wait_wcrm_wcrl = 0;
    hd64570_reg->wait_wcrh = 0;

    /*
     * Initialize the MSCI registers
     */
    msci_reg->cmd = HD64570_CMD_CHAN_RST;
    if (idb->poll_protocol_flags != PP_ROLE_UNDEFINED ) {
        msci_reg->md1_md0 = HD64570_MD0_BiSYNC;
    } else {
        msci_reg->md1_md0 = HD64570_MD0_HDLC | HD64570_MD0_CRCCC |
                            HD64570_MD0_CRC1 | HD64570_MD0_CRC0;
    }

    /*
     * Set the transmit and receive fifo thresholds.
     */
    hd64570_set_fifo_limits(idb);

    if (idb->poll_protocol_flags != PP_ROLE_UNDEFINED ) {
        msci_reg->tmc_idl = HD64570_BSC_IDLE_PATTERN;
	
        /* Xmit idle; store Rx SYNs into Rx buffer */
        msci_reg->ctl_md2 = HD64570_CTL_IDLC | HD64570_CTL_SYNCLD;
        msci_reg->sa1_sa0 = idb->syn_char | (idb->syn_char << 8);
    } else {
	if (idb->serial_flags & SERIAL_FLAGS_DO_ALT_FLAG)
	    msci_reg->tmc_idl = HD64570_IDLE_MARK;
	else
	    msci_reg->tmc_idl = HD64570_IDLE_FLAG;
	
	msci_reg->ctl_md2 = HD64570_CTL_IDLC;   /* Xmit idle */
	
	if (ssb->nrzi_enable)
	    msci_reg->ctl_md2 |= HD64570_MD2_NRZI;
    }

    hd64570_init_config(idb);
    
    /*
     * Initialize the DMAC
     */
    
    /*
     * NOTE: dmer and pcr registers can only be accessed in the 8 bit mode,
     * and should only be updated via hd64570_dmer_pcr_write() routine.
     */
    hd64570_dmer_pcr_write(ds, HD64570_ODD_BYTE_ACCESS, HD64570_DMA_PRIORITY);
    
    /* BSC uses Rx character interrupts */
    if ( idb->poll_protocol_flags == PP_ROLE_UNDEFINED ) {
	dmac_rx_reg->dmr_dsr = HD64570_DMR_NF | HD64570_DMR_TMOD |
	    HD64570_DSR_DE;
	dmac_rx_reg->dcr_dir = HD64570_DIR_BOFE | HD64570_DIR_EOME |
	    HD64570_DIR_EOTE;
    }
    
    dmac_tx_reg->dmr_dsr = HD64570_DMR_NF | HD64570_DMR_TMOD;
    dmac_tx_reg->dcr_dir = HD64570_DIR_BOFE | HD64570_DIR_EOME |
	HD64570_DIR_EOTE;
    
    /*
     * Initialize the interrupt registers
     */
    if (idb->poll_protocol_flags != PP_ROLE_UNDEFINED ) {
        msci_reg->ie1_ie0 = HD64570_IE0_RXRDYE | HD64570_IE1_UDRNE |
	    HD64570_IE0_TXINTE | HD64570_IE0_RXINTE;
	/*
	 * enable receive overrun interrupt
	 */
	msci_reg->fie_ie2 = HD64570_IE2_OVRNE;
	
    } else {
	msci_reg->ie1_ie0 = HD64570_IE0_TXINTE | HD64570_IE1_UDRNE;
	
	if (idb->hdx_flags & HDX_FLAG_ON) {
	    msci_reg->ie1_ie0 |= HD64570_IE1_IDLE;
	}
    }
    
    if (HD64570_EVEN(idb->subunit)) {
        if (idb->poll_protocol_flags != PP_ROLE_UNDEFINED) {
            hd64570_reg->ier1_ier0 |= HD64570_IER0_TXINT0E |
		                      HD64570_IER0_RXRDY0E |
				      HD64570_IER1_DMIB1E  |
				      HD64570_IER0_RXINT0E ;
            hd64570_reg->ier2 |= HD64570_IER2_T0IRQE | HD64570_IER2_T1IRQE;
        } else {
	    hd64570_reg->ier1_ier0 |= HD64570_IER0_TXINT0E |
		                      HD64570_IER1_DMIA0E |
				      HD64570_IER1_DMIB0E |
				      HD64570_IER1_DMIB1E;
	    
	    /*
	     * If doing HDX, fire up the timer and idle interrupts
	     */
	    if (idb->hdx_flags & HDX_FLAG_ON)
		hd64570_reg->ier2 |= HD64570_IER2_T0IRQE | HD64570_IER2_T1IRQE;
        }
    } else {
        if (idb->poll_protocol_flags != PP_ROLE_UNDEFINED ) {
            hd64570_reg->ier1_ier0 |= HD64570_IER0_TXINT1E |
                                      HD64570_IER0_RXRDY1E |
                                      HD64570_IER1_DMIB3E  |
                                      HD64570_IER0_RXINT1E ;
            hd64570_reg->ier2 |= HD64570_IER2_T2IRQE | HD64570_IER2_T3IRQE;
	} else {
            hd64570_reg->ier1_ier0 |= HD64570_IER0_TXINT1E |
                                      HD64570_IER1_DMIA2E |
                                      HD64570_IER1_DMIB2E |
                                      HD64570_IER1_DMIB3E;

	    /*
	     * If doing HDX, fire up the timer and idle interrupts
	     */
	    if (idb->hdx_flags & HDX_FLAG_ON)
		hd64570_reg->ier2 |= HD64570_IER2_T2IRQE | HD64570_IER2_T3IRQE;
	}
    }
    
    hd64570_enter_transparent_mode(idb);
    
    reset_interrupt_level(SR);
}

static boolean hd64570_hw_set_config (hwidbtype *hwidb, itemlist *ilist,
				      serialsb *ssb)
{
    itemdesc *item;
    boolean any_unhandled = FALSE;   /* assume we've handled all items */
    tinybool this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;

	switch (item->item) {

	  case SERIAL_HW_DCE_TERMINAL_TIMING_ENABLE:
	    ssb->dce_terminal_timing_enable = item->u.l_value;
	    hd64570_init(hwidb);	/* Reset code will set it up */
	    break;

	  case SERIAL_HW_INVERT_TXC:
	    ssb->invert_txc = item->u.l_value;
	    hd64570_init(hwidb);	/* Reset code will set it up */
	    break;

	  case SERIAL_HW_NRZI_ENABLE:
	    ssb->nrzi_enable = item->u.l_value;
	    hd64570_init(hwidb);	/* Reset code will set it up */
	    break;

	  case SERIAL_HW_TIMESLOT16_ENABLED:
	    ssb->timeslot16_enabled = item->u.l_value;
	    hd64570_init(hwidb);	/* Reset code will set it up */
	    break;

	  case SERIAL_HW_CRC4_ENABLED:
	    ssb->crc4_enabled = item->u.l_value;
	    hd64570_init(hwidb);	/* Reset code will set it up */
	    break;
	    
	  case SERIAL_HW_RX_SYNC_DISABLE:
	    ssb->rx_sync_disable = item->u.l_value;
	    hd64570_init(hwidb);	/* Reset code will set it up */
	    break;

	  case SERIAL_HW_IGNORE_DCD:
	    ssb->ignore_dcd = item->u.l_value;
	    hd64570_init(hwidb);	/* Reset code will set it up */
    	    break;

	  case SERIAL_HW_RTS_TIMEOUT:
	    ssb->hdx_rts_timeout = item->u.l_value;
	    hd64570_init(hwidb);	/* Reset code will set it up */
	    break;

	  case SERIAL_HW_CTS_DELAY:
	    ssb->hdx_cts_delay = item->u.l_value;
	    hd64570_init(hwidb);	/* Reset code will set it up */
	    break;

	  case SERIAL_HW_SET_CLOCKRATE:
	    hd64570_clockrate_command(item->u.buffer);
	    break;

	  default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}

	if (!this_item_unhandled)
	    item->handled = TRUE;
    }

    return(!any_unhandled);
}

static boolean hd64570_hwidb_state_config (hwidbtype *hwidb, ulong opcode,
					   itemlist *ilist)
{
    boolean ret;
    serialsb *ssb;

    if ((hwidb == NULL) && (ilist == NULL))
	return (FALSE);

    if (ilist->item_count == -1)
	ilist->item_count = itemlist_count(ilist);

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    if (ssb == NULL)
	return (FALSE);

    switch (opcode) {
      case IDB_CONTROL_SET_CONFIG:
	ret = hd64570_hw_set_config(hwidb, ilist, ssb);
	break;

#if 0
      case IDB_CONTROL_READ_CONFIG:
	ret = serial_hw_read_config(hwidb, ilist, ssb);
	break;
	
      case IDB_CONTROL_SET_STATE:
	ret = serial_hw_set_state(hwidb, ilist, ssb);
	break;
	
      case IDB_CONTROL_READ_STATE:
	ret = serial_hw_read_state(hwidb, ilist, ssb);
	break;

#endif
      default:
	ret = FALSE;
	break;
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (!ret) {
	itemlist_reset(ilist);
	ret = (*ssb->hw_super_state_config)(hwidb, opcode, ilist);
    }
    return (ret);
}


/*
 * hd64570_init_idb_common()
 */
void hd64570_init_idb_common (hwidbtype *idb)

{
    hd64570_instance_t *ds;
    uchar *ds_base;
    serialsb *ssb;

    les_serial_init_idb_common(idb);

    idb->type = IDBTYPE_HD64570;
    idb->typestring = "HD64570";
    idb->serial_flags |= SERIAL_FLAGS_CAN_ALT_FLAG;
    idb->serial_flags |= SERIAL_FLAGS_CAN_INT_TX | SERIAL_FLAGS_NO_DTE_INT_TX |
	SERIAL_FLAGS_CAN_CLK_INV | SERIAL_FLAGS_CAN_NRZI |
	    SERIAL_FLAGS_CAN_IGNORE_DCD;

    /*
     * Set up Function calls
     */
    idb->soutput = hd64570_tx_start;
    idb->reset = hd64570_restart;
    idb->shutdown = hd64570_shutdown;
    idb->enable = (enable_t)return_nothing;
    idb->setup_bridging = les_serial_setup_bridging;
    idb->device_periodic = hd64570_periodic;
    idb->txqlength = hd64570_txqlength;
    idb->fastsend = hd64570_fastsend;
    idb->show_controller = hd64570_show_controller;
    idb->show_serial_state = hd64570_show_serial_state;

    /*
     * idb->set_maxdgram must be initialized after calling serial_idb_init()
     * so that hdlc_setup does not attempt to call hd64570_set_maxdgram()
     * while doing system init
     */
    idb->set_maxdgram = hd64570_set_maxdgram;

    /*
     * Init half-duplex stuff
     */
    idb->hdx_flags |= HDX_FLAG_CAPABLE;

    serial_idb_enqueue(idb);
    idb_enqueue(idb);

    /*
     * Set up state config vector
     */
    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);
    ssb->hw_super_state_config = idb->state_config;
    idb->state_config = hd64570_hwidb_state_config;
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * Allocate this instance of the driver:
     */
    ds_base = malloc_named((sizeof(hd64570_instance_t) + 8), "HD64570 Instance");
    if (!ds_base) {
	crashdump(0);
    } else {
	ds = (hd64570_instance_t *)(((ulong)ds_base +7) & ~7L);
	INSTANCE = ds;
	
	/*
	 * Allocate a RX ring for this interface
	 */
	ds->rx_base = malloc_named_iomem_aligned(HD64570_BUF_SIZE,
					HD64570_BUF_2K_ALIGN, "HD64570 RX Ring");
				  
	if (ds->rx_base) 
	    ds->rx_ra = (hd64570_buf_ring_t *)(((ulong)ds->rx_base + 7) & ~7L);
	else {
	    crashdump(0);
	}

	/*
	 * Allocate a transmit ring for this interface:
	 */
	ds->tx_base = malloc_named_iomem_aligned(HD64570_BUF_SIZE,
					HD64570_BUF_2K_ALIGN, "HD64570 TX Ring");
				  
	if (ds->tx_base)
	    ds->tx_ra = (hd64570_buf_ring_t *)(((ulong)ds->tx_base + 7) & ~7L);
	else {
	    crashdump(0);
	}

	/*
	 * Set up flap protection parameters
	 */
	ds->flap_rate  = SERIAL_FLAP_RATE;
	ds->flap_count = 0;
	TIMER_STOP(ds->flap_time);
	ds->flap_backoff = 0;
	ds->flap_enabled = FALSE;

	/*
	 * Populate the registries
	 */
	reg_add_media_set_rxoffset(idb->type, hd64570_set_rxoffset,
				   "hd64570_set_rxoffset");
      

    }
}



