/* $Id: if_les_mk5025.c,v 3.7.12.10 1996/09/15 18:19:32 srane Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_mk5025.c,v $
 *------------------------------------------------------------------
 * if_les_mk5025.c - Pancake Network serial interface driver
 * 
 * March 1990, Chris Shaker
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * PLEASE NOTE: I am now using the convention that RAP must be 0 whenever
 * interrupts are enabled so that the level 4 interrupt routine does not
 * have to write RAP = 0 to save time.
 *------------------------------------------------------------------
 * $Log: if_les_mk5025.c,v $
 * Revision 3.7.12.10  1996/09/15  18:19:32  srane
 * CSCdi62516:  buffer changes lost after reload on a serial SMDS interface
 * Branch: California_branch
 *
 * Revision 3.7.12.9  1996/09/10  03:08:24  snyder
 * CSCdi68568:  more constant opportuniites
 *              160 out of data
 *               72 out of image
 * Branch: California_branch
 *
 * Revision 3.7.12.8  1996/08/28  12:58:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.12.7  1996/08/13  02:20:32  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.7.12.6  1996/07/20  03:19:59  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.7.12.5  1996/06/11  04:54:45  mschaef
 * CSCdi59601:  Bus Error occurs when switching Ipx,Apple over HDLC
 * Branch: California_branch
 *
 * Revision 3.7.12.4  1996/04/27  06:33:09  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.5.12.3  1996/04/26  01:42:43  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.5.12.2  1996/04/17  03:35:16  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.5.12.1  1996/02/23  20:51:52  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.7.12.3  1996/04/17  13:44:00  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.7.12.2  1996/04/03  01:59:45  sagarwal
 * CSCdi51875:  Serial Int. MTU change doesnt get new buffers if pulsetime
 * confged
 * Branch: California_branch
 *  Compare current bufferpool size against idb maxdgram instead of
 *  relying on boolean passed to controller init to remember mtu
 *  changes across DTR pulse intervals
 *
 * Revision 3.7.12.1  1996/03/18  20:42:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.16.3  1996/03/13  01:50:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.16.2  1996/02/20  01:08:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/13  22:10:59  fred
 * CSCdi47956:  WFQ severely degrades RSRB performance
 *              drivers: increase TQL in short queue mode for link
 *              stability
 *              fastsrb_les.c: make sure pointers are there for queuing
 *              routines
 *
 * Revision 3.6  1996/01/25  23:12:01  gstovall
 * CSCdi47390:  down-when-looped command not working (regression)
 * Remember to reset the itemlist before tossing it up to the next level.
 *
 * Revision 3.5.16.1  1996/01/24  22:24:29  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1995/12/07  22:30:31  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/12/06  18:31:08  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  17:41:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:14:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/06  02:26:18  lbustini
 * CSCdi39576:  dialer dtr does not leave DTR low permanently after idle
 * timeout
 * Raise DTR only when a call is placed.
 *
 * Revision 2.8  1995/08/08  19:47:30  fred
 * CSCdi38414:  congestion of compressed interfaces reduces speed.
 * when compressing data AND using WFQ/PQ/CQ, use a ring size of two, not
 * one.
 *
 * Revision 2.7  1995/07/18  06:47:40  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.6  1995/06/23 16:39:49  fred
 * CSCdi36295:  Commit Queue MIB to 11.0
 *
 * Revision 2.5  1995/06/21  08:55:36  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.4  1995/06/21  03:01:17  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.3  1995/06/19  06:47:52  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:11:35  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:37:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "packet.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../wan/ppp.h"
#include "../if/ether.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"
#include "../wan/dialer_registry.h"
#include "../os/pool.h"
#include "../os/buffers.h"

#include "logger.h"
#include "../les/msg_mk5025.c"		/* Not a typo, see logger.h */
#include "if_les.h"
#include "if_les_mk5025.h"  

#include "../if/network.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "../dev/monitor1.h"
#include "../wan/atm_dxi.h"
#include "../if/serial_inline.h"
#include "if_les_serial.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../util/itemlist.h"
#include "../if/traffic_shape.h"

static const u_char flag_table[MAX_FLAG_INDEX] = {
    0, 2, 4, 9, 18, 5, 11, 22, 12, 25, 19, 7, 15, 31, 30, 28,
    24, 17, 3, 6, 13, 27, 23, 14, 29, 26, 21, 10, 20, 8, 16};

/*
 * MK5025 subsystem header
 */
#define MK5025_MAJVERSION   1
#define MK5025_MINVERSION   0
#define MK5025_EDITVERSION  1

SUBSYS_HEADER(mk5025, MK5025_MAJVERSION, MK5025_MINVERSION, MK5025_EDITVERSION,
	      mk5025_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);


/*
 * mk5025_set_transmit_delay - registry function for transmitter-delay cmd. 
 * Specify the minimum number of flags inserted between succesive
 * transmissions, a nice thing to do for slow receivers.
 * The minimum number of flags is 2. The maximum number of flags is 62.
 * Only even values are allowed (odd values will be rounded up)
 */
static void mk5025_set_transmit_delay (hwidbtype *idb, int value)
{
    if (value < 0) { /* Bounds check */
	value = 0;
    }
    else if (value > MAX_DELAY_FLAGS ) {
	   value = MAX_DELAY_FLAGS;
    }

    idb->xmitdelay = value;

    mk5025_init(idb); /* Reset the interface */
}


/*
 * mk5025_getbuf_inline
 *
 * Get a packet buffer, initialize appropriate fields in the packet, link
 * it to the receive ring, and give ownership of the packet to the mk5025.
 * This routine is used by the receive interrupt routine and implements
 * input throttling:
 */
static inline paktype *mk5025_getbuf_inline (hwidbtype              *idb,
			                     mk5025_rx_descriptor_t *bd,
			                     boolean                 cache_ok)
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
#ifdef DEBUG
	if (!validmem(pak)) {
	    buginf("\nmk5025_getbuf: Invalid p=%x\n", pak);
	    return(NULL);
	}
	if (pak->next != NULL) {
	    buginf("\nmk5025_getbuf: buffer in list, pak=%x\n", pak);
	    return(NULL);
	}
	if (pak->refcount != 1) {
	    buginf("\nmk5025_getbuf: p->refcount not zero (%d), pak=%x\n",
		   pak->refcount, pak);
	    return(NULL);
	}
#endif
	
	/*
	 * Initialize the packet encapsulation type and datagramstart:
	 */
	start = pak->data_area + idb->start_offset;

#ifdef DEBUG
	if ((uint)start & 1) {
	    start = (uchar *)((uint)start & BUFFER_WORD_ALIGN);
	    buging("mk5025_getbuf: Error! Datagramstart is odd\n");
	}
#endif	
	pak->datagramstart = start;

	/*
	 * Initialize the ring entry for this packet:
	 */
	bd->buffer_low = MK5025_BUFF_LO(start);
	bd->buffer_high = MK5025_BUFF_HI(start);
	bd->status = MK5025_RMD_OWNA;	/* MK5025 owns the buffer now */
	return(pak);
    } else {
	idb->counters.output_nobuffers++;
	return(NULL);
    }
}


/*
 * mk5025_getbuf
 *
 * Function call wrapper around inline function
 */
static paktype *mk5025_getbuf (hwidbtype *idb, mk5025_rx_descriptor_t *bd)
{
    return(mk5025_getbuf_inline(idb, bd, FALSE));
}


/*
 * Set the maximum datagramsize for this interface and then
 * reset the interface to use it.  Don't bother using buffersize - it
 * is used for the mci/cbus routines.
 * Return TRUE if the datagram size was changed, else return FALSE.
 */

static boolean mk5025_set_maxdgram (hwidbtype *idb, int buffersize,
				    int maxdgram)
{
    if (!(idb->type == IDBTYPE_MK5025)) {
	printf(set_maxdgram_failed, maxdgram, idb->hw_namestring);
	return(FALSE);
    }

    if_maxdgram_adjust(idb, maxdgram);

    /*
     * Initialize the interface.
     */
    mk5025_init(idb);
    idb->counters.resets++;
    return(TRUE);
}


/*
 * Wrapper routine for idb->reset vector:
 */
static void mk5025_restart (hwidbtype *idb)
{
    leveltype SR;

    SR = raise_interrupt_level(NETS_DISABLE);

    holdq_reset(idb);
    if (idb->state != IDBS_ADMINDOWN) {
	mk5025_init(idb);
	idb->counters.resets++;
    }

    reset_interrupt_level(SR);
}

/*
 * mk5025_set_rxoffset
 * Set receive offset
 */
static void mk5025_set_rxoffset (hwidbtype *idb)
{
    mk5025_restart(idb);
}


/*
 * mk5025_shutdown
 * Shut down a mk5025 interface
 */
static void mk5025_shutdown (hwidbtype *idb, int newstate)
{
    mk5025_instance_t *ds = INSTANCE;
    leveltype SR;
    
    net_cstate(idb, newstate);

    SR = raise_interrupt_level(NETS_DISABLE);

    /*
     * Kill one source of interrupts now
     */
    mk5025_disable_modem_interrupt(ds);
    mk5025_stop(idb, ds->regaddr);
    mk5025_reset(idb);
    
    reset_interrupt_level(SR);
}


/*
 * Set up for bridging on this interface:
 */
static void mk5025_setup_bridging (hwidbtype *idb)
{
    mk5025_init(idb);	/* Reset code will set it up */
}


/*
 * mk5025_resetpak
 * Set up the MK5025 ring entry to reuse the packet buffer:
 */
static void mk5025_resetpak (mk5025_rx_descriptor_t *bd)
{
    /*
     * Re-initialize fields in the buffer descriptor:
     */
    bd->status = MK5025_RMD_OWNA;	/* MK5025 owns the buffer now */
}


/*
 * Start or restart output.
 */
static void mk5025_tx_start (hwidbtype *idb)
{
    mk5025_instance_t *ds = INSTANCE;
    int ring_index, bytes;
    mk5025_tx_descriptor_t *tmd;
    mk5025_hw_reg_t *reg;
    paktype *pak;
    leveltype SR;
    
    SR = raise_interrupt_level(NETS_DISABLE);
    reg = ds->regaddr;

    /*
     * While there is another packet to output and room for it in the
     * MK5025 TX queue, enqueue the packet:
     */
    while ((ds->tx_count < ds->tx_size) &&
	   (pak = (*idb->oqueue_dequeue)(idb))) {

	bytes = pak->datagramsize;
	if (bytes > idb->max_pak_size) {
	    errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, bytes);
	    datagram_done(pak);
	    continue;
	}

	/*
	 * Make sure that datagramstart will not be at an odd
	 * address - the MK5025 cannot deal with this!
	 * Note that multiple protocols may be referencing this
	 * packet, thus even though there is room to just shift
	 * it to an word aligned boundry, it is correct to
	 * copy the packet to a new buffer and use that new packet
	 */
	if ((uint)pak->datagramstart & 1) {
	    paktype *new_pak;
	    new_pak = serial_fix_datagramstart(pak, idb);
	    if (new_pak)
		pak = new_pak;
	    else {
		datagram_done(pak);
		continue;
	    }
	}

	/*
	 * Get pointers for the TX ring entry at tx_tail:
	 */
	ring_index = ds->tx_tail++;
	if (ds->tx_tail >= ds->tx_size)
	    ds->tx_tail = 0;
	tmd = &ds->tx_ra->ring_entry[ring_index];

	ds->tx_p[ring_index] = pak;
	ds->tx_count++;

	idb->counters.tx_cumbytes += bytes;
	TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */
	idb->counters.outputs++;

#ifdef DEBUG
	/*
	 * Check for bad encapsulation:
	 */
	if ((pak->enctype == ET_HDLC) &&
	    (((*(ushort *)pak->datagramstart) != 0x8f00) &&
	     ((*(ushort *)pak->datagramstart) != 0x0f00))) {
	    errmsg(&msgsym(OUTENCAP, MK5),
		   idb->unit, (*(ushort *)pak->datagramstart));
	    datagram_done(pak);
	    continue;
	}
#endif

	if ((pak->enctype == ET_HDLC) &&
	    ((*(ushort *)pak->datagramstart) & HDLC_BCAST))
	    idb->counters.output_broad++;

	/*
	 * Set up this TX ring entry for this packet and
	 * force TX ring polling:
	 */
	tmd->buffer_low = MK5025_BUFF_LO(pak->datagramstart);
	tmd->buffer_high = MK5025_BUFF_HI(pak->datagramstart);
	tmd->packet_size = -(bytes);
	tmd->status =
	    MK5025_TMD_OWNA | MK5025_TMD_SLF | MK5025_TMD_ELF | MK5025_TMD_TUI;

	/*
	 * Force the Mk5025 to poll the TX ring and see this packet:
	 */
	reg->csr = MK5025_CSR0_INEA | MK5025_CSR0_TDMD;
    }
    reset_interrupt_level(SR);	/* Restore interrupt level */
}


/*
 * Handle a RX interrupt:
 */
void mk5025_RX_interrupt (hwidbtype *idb)
{
    mk5025_rx_descriptor_t *rmd;
    mk5025_instance_t *ds;
    paktype *newpak;
    paktype *pak = NULL;
    char packets = 1;

    /*
     * Receive all input frames:
     */
  next_RX_packet:
    ds = INSTANCE;

    rmd = &ds->rx_ra->ring_entry[ds->rx_head];
    if (((rmd->status & MK5025_RMD_OWNA) == 0) && (packets++ < ds->rx_size)) {
	pak = ds->rx_p[ds->rx_head];
	pak->datagramsize = (ushort)(-((short)rmd->packet_size));
	set_if_input(pak, idb->firstsw);

	/*
	 * If we have a scheduled run interval configured, go see if we need
	 * to drop packets to give processes a chance to run:
	 */
	if (sched_run_interval)
	    goto mk5025_check_sched;
return_mk5025_check_sched:
	/*
	 * Attempt to get a new packet buffer to replace
	 * this packet buffer on the RX ring:
	 */
	newpak = mk5025_getbuf_inline(idb, rmd, TRUE);
	if (newpak) {

            ds->rx_p[ds->rx_head] = newpak;

	    serial_RX_interrupt_inline(idb, pak);
	    NEXT_READ_PACKET(ds);

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

			mk5025_resetpak(rmd); /* pak now owned by MK5025 */
			serial_queue_for_process(idb, newpak);
			NEXT_READ_PACKET(ds);
		    }
		}
	    }

	    /*
	     * We could not get a new big buffer, so drop the packet:
	     */
	    clear_if_input(pak);
	    mk5025_resetpak(rmd);
	    idb->counters.input_drops++;
	    MAYBE_DROPPED(idb);
	    idb_mark_throttled(idb);
	mk5025_next_packet:
	    NEXT_READ_PACKET(ds);
	}
    }
    return;

mk5025_check_sched:
    if (AWAKE(sched_run_time)) {
	
	/*
	 * The scheduler has not run recently enough,
	 * so, throttle the MK5025:
	 */
	clear_if_input(pak);
	mk5025_resetpak(rmd);
	idb->counters.input_drops++;
	MAYBE_DROPPED(idb);
	idb_mark_throttled(idb);
	goto mk5025_next_packet;
    } else
	goto return_mk5025_check_sched;
}


void mk5025_TX_interrupt (hwidbtype *idb)
{
    mk5025_tx_descriptor_t *tmd;
    mk5025_instance_t *ds;
    int ring_index;
    paktype *pak;
    
    /*
     * Process any transmitted packets:
     */
    ds = INSTANCE;
    ring_index = ds->tx_head;
    tmd = &ds->tx_ra->ring_entry[ring_index];
    pak = ds->tx_p[ring_index];
    
    while (pak && ((tmd->status & MK5025_TMD_OWNA) == 0)) {
	
	/*
	 * Update tx_head to point to next packet in queue:
	 */
	ds->tx_head++;
	if (ds->tx_head >= ds->tx_size)
	    ds->tx_head = 0;

	/*
	 * Do packet TX accounting and clean up:
	 */
	datagram_done(pak); /* Release output packet*/
	GET_TIMESTAMP(idb->lastoutput);
	ds->tx_p[ring_index] = NULL;
	ds->tx_count--;
	
	/*
	 * Next packet:
	 */
	ring_index = ds->tx_head;
	tmd = &ds->tx_ra->ring_entry[ring_index];
	pak = ds->tx_p[ring_index];
    }
    mk5025_tx_start(idb);	/* Possibly restart output */
}


/*
 * Process MK5025 Error interrupts:
 */
void mk5025_E_interrupt (ushort csr0_status, hwidbtype *idb)
{
    mk5025_instance_t *ds = INSTANCE;

    /*
     * Check for a primitive interrupt:
     */
    if (csr0_status & MK5025_CSR0_PINT) {
	mk5025_hw_reg_t *reg;
	int csr1;

	/*
	 * See what's up:
	 */
	reg = ds->regaddr;
	reg->rap = MK5025_CSR1;
	csr1 = reg->csr;
	
	if (csr1 & MK5025_CSR1_UERR) {
	    ds->regaddr->csr = MK5025_CSR1_UERR;
	    ds->user_prim_err++;
	    errmsg(&msgsym(UPRIMERR, MK5), idb->unit, csr0_status, csr1);
	}
	if (csr1 & MK5025_CSR1_PLOST) {
	    ds->regaddr->csr = MK5025_CSR1_PAV; /* Clear PLOST */
	    ds->prov_prim_lost++;
	    errmsg(&msgsym(PLOSTERR, MK5), idb->unit, csr0_status, csr1);
	}
	if (csr1 & MK5025_CSR1_PAV) {
	    ds->regaddr->csr = MK5025_CSR1_PAV;
	    ds->spurious_prov_prim++;
	    errmsg(&msgsym(PPRIMERR, MK5), idb->unit, csr0_status, csr1);
	} else {
	    ds->spurious_prim_int++;
	    errmsg(&msgsym(SPURPPRIMERR, MK5), idb->unit, csr0_status, csr1);
	}
	reg->rap = MK5025_CSR0;	/* Leave pointing at CSR0 */
    }
	
    /*
     * Error Handling:
     */
    if (csr0_status & MK5025_CSR0_MISS) {
	idb->counters.input_error++;
	idb->counters.input_resource++;
    }
    if (csr0_status & MK5025_CSR0_ROR) {
	idb->counters.input_error++;
	idb->counters.input_overrun++;
    }
    if (csr0_status & MK5025_CSR0_MERR) {
	idb->dma_memory_error++;
	errmsg(&msgsym(MEMERR, MK5), idb->unit, 0, csr0_status);
	mk5025_shutdown(idb, IDBS_DOWN);
    }
    if (csr0_status & MK5025_CSR0_TUR) {
	idb->counters.output_error++;
	idb->counters.output_underrun++;
    }
    MAYBE_DROPPED(idb);
}


/*
 * mk5025_fastsend
 * Attempt to transmit this packet immediately.
 * Called from interrupt level.
 */
static void mk5025_fastsend (hwidbtype *idb, paktype *pak)
{
    mk5025_instance_t *ds;
    mk5025_tx_descriptor_t *tmd;
    paktype *tx_pak;
    traffic_shape_t *shape;
    ushort bytes;

    if (idb->state != IDBS_UP) {
        idb->counters.output_total_drops++;
        datagram_done(pak);
        return;
    }

    bytes = pak->datagramsize;
#ifdef DEBUG
    if (bytes > idb->max_pak_size) {
	idb->counters.output_total_drops++;
	retbuffer(pak);
	errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, bytes);
	return;
    }
#endif

    /*
     * determine whether there is window available to send the message with
     * in the rate control bucket
     */
    shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
    if (shape) {
	traffic_shape_defer_sending(shape, pak);
	return;
    }

    /*
     * Make sure that datagramstart will not be at an odd
     * address - the MK5025 cannot deal with this!
     * Note that multiple protocols may be referencing this
     * packet, thus even though there is room to just shift
     * it to an word aligned boundry, it is correct to
     * copy the packet to a new buffer and use that new packet
     */
    if ((uint)pak->datagramstart & 1) {
        paktype *new_pak;
	new_pak = serial_fix_datagramstart(pak, idb);
	if (new_pak)
	    pak = new_pak;
	else {
	    retbuffer(pak);
	    return;
	  }
    }

    ds = INSTANCE;

    /*
     * If no packets are queued for output on this interface,
     * immediately transmit the packet:
     */
    if (!idb->output_qcount) {
	
	/*
	 * Get pointer for the TX ring entry at tx_tail:
	 */
	tmd = &ds->tx_ra->ring_entry[ds->tx_tail];
	
	/*
	 * Is there already room for another packet in the output ring?
	 */
	if (ds->tx_count < ds->tx_size) {
	    if ((tmd->status & MK5025_TMD_OWNA) == 0) {
		
		/*
		 * Transmit our packet:
		 */
		
		/*
		 * Give the MK5025 ownership of the buffer,
		 * and force TX ring polling:
		 */
		tmd->buffer_low = MK5025_BUFF_LO(pak->datagramstart);
		tmd->buffer_high = MK5025_BUFF_HI(pak->datagramstart);
		tmd->packet_size = -(bytes);
		tmd->status = MK5025_TMD_OWNA | MK5025_TMD_SLF |
		    MK5025_TMD_ELF | MK5025_TMD_TUI;
		
		/*
		 * Force the MK5025 to poll the TX ring so it sees this packet:
		 */
		ds->regaddr->csr = MK5025_CSR0_INEA | MK5025_CSR0_TDMD;

		/*
		 * Do packet accounting after starting the MK5025:
		 */
		ds->tx_count++;
		idb->counters.tx_cumbytes_fast[pak->acct_proto] += bytes;
		idb->counters.outputs_fast[pak->acct_proto]++;
		TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */
		if ((idb->enctype == ET_HDLC) &&
		    ((*(ushort *)pak->datagramstart) & HDLC_BCAST))
		    idb->counters.output_broad++;
		ds->tx_p[ds->tx_tail++] = pak; /* Save packet ptr */
		if (ds->tx_tail >= ds->tx_size) /* End of ring? */
		    ds->tx_tail = 0;
		return;

	    } else {
		
		/*
		 * Error, we don't own the buffer:
		 */
		retbuffer(pak);
		idb->counters.output_total_drops++;
#ifdef DEBUG
		buginf("\nMK5(%d): fastsend error - mk5025 owns packet, tx_tail=%d, tx_count=%d, tx_head=%d\n",
		       idb->unit, ds->tx_tail, ds->tx_count, ds->tx_head);
#endif
		return;
	    }
	} else if (ds->tx_tail == ds->tx_head) {
	    
	    /*
	     * We have a full TX ring. Is the first packet in the ring done?
	     * If so, we can service the TX interrupt on this packet and use
	     * the ring entry for our new packet:
	     */
	    if ((tmd->status & MK5025_TMD_OWNA) == 0) {

		/*
		 * Was this ring entry transmitting a packet?
		 */
		tx_pak = ds->tx_p[ds->tx_tail];
		if (tx_pak) {
		    /*
		     * Do TX packet accounting and clean up:
		     */
		    datagram_done(tx_pak); /* Release output packet*/
		    GET_TIMESTAMP(idb->lastoutput);
		}
		
		/*
		 * Now, we can transmit our packet:
		 */

		/*
		 * Give the MK5025 ownership of the buffer and
		 * force TX ring polling:
		 */
		tmd->buffer_low = MK5025_BUFF_LO(pak->datagramstart);
		tmd->buffer_high = MK5025_BUFF_HI(pak->datagramstart);
		tmd->packet_size = -(bytes);
		tmd->status = MK5025_TMD_OWNA | MK5025_TMD_SLF |
		    MK5025_TMD_ELF | MK5025_TMD_TUI;
		ds->regaddr->csr = MK5025_CSR0_INEA | MK5025_CSR0_TDMD;

		/*
		 * Do packet accounting after starting the MK5025:
		 */
		/* Don't change ds->tx_count, it should stay the same */
		idb->counters.tx_cumbytes_fast[pak->acct_proto] += bytes;
		idb->counters.outputs_fast[pak->acct_proto]++;
		TIMER_START(idb->xmittimer, TIMEOUT); /* set timeout timer */
		if ((idb->enctype == ET_HDLC) &&
		    ((*(ushort *)pak->datagramstart) & HDLC_BCAST))
		    idb->counters.output_broad++;
		ds->tx_p[ds->tx_tail++] = pak; /* Address of packet */
		if (ds->tx_tail >= ds->tx_size) /* End of ring? */
		    ds->tx_tail = 0;
		/*
		 * Have next TX interrupt bypass this packet since we are
		 * processing it here.
		 */
		ds->tx_head++;
		if (ds->tx_head >= ds->tx_size) /* End of ring? */
		    ds->tx_head = 0;
		return;
	    } else {
		
		/*
		 * We have a full TX ring, so queue the packet in the output
		 * hold queue.
		 */
		holdq_enqueue_fast(idb, pak);
		mk5025_tx_start(idb);
		return;
	    }
	} else {
	    
	    /*
	     * Error, invalid tx_count:
	     */
	    retbuffer(pak);
	    idb->counters.output_total_drops++;
	    return;
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
         * the MK5025 (and in particular, the ownership bits, and see
         * if we can clear some space before pulling frames off of
         * the holdq by faking a TX interrupt. The TX interrupt
         * routine will call mk5025_tx_start() for us.
         *
         * This causes a smoother saturation roll-off
         */
        if (ds->tx_count == ds->tx_size)
            mk5025_TX_interrupt(idb);
        else
            mk5025_tx_start(idb);
    }
}


/*
 * mk5025_enable
 * Enable a MK5025 interface, most likely shut down for input congestion.
 * Please note that this function must be called with NETS_DISABLED to 
 * work correctly.
 */
static void mk5025_enable (hwidbtype *idb)
{
    if (idb->type == IDBTYPE_MK5025)
	idb_mark_unthrottled(idb);
}


/*
 * Read reported errors from the MK5025.
 */
static void read_mk5025_errors (hwidbtype *idb)
{
    mk5025_initblock_t *ib;
    mk5025_instance_t *ds;
    leveltype SR;

    ds = INSTANCE;

    ib = ds->ib;
	
    /*
     * Poll for MK5025 detected errors. Reset the error counts.
     */
    SR = raise_interrupt_level(NETS_DISABLE);
    if (ib->bad_frames) {
	idb->counters.input_error += ib->bad_frames;
	idb->counters.input_crc += ib->bad_frames;
	ib->bad_frames = 0;
	MAYBE_DROPPED(idb);
    }
    if (ib->frmrs) {
	idb->counters.input_error += ib->frmrs;
	idb->counters.input_frame += ib->frmrs;
	ib->frmrs = 0;
	MAYBE_DROPPED(idb);
    }
    if (ib->runts) {
	idb->counters.input_error += ib->runts;
	idb->counters.runts += ib->runts;
	ib->runts = 0;
	MAYBE_DROPPED(idb);
    }
    reset_interrupt_level(SR);
}


/*
 * Poll for MK5025 reported errors and ensure that MK5025 is running
 * (if it should be)
 */
static void mk5025_periodic (hwidbtype *idb)
{
    mk5025_instance_t *ds;
    leveltype SR;

    ds = INSTANCE;
	
    SR = raise_interrupt_level(NETS_DISABLE);
    /*
     * Read the error registers.
     */
    read_mk5025_errors(idb);
    
    /*
     * Check to see if we're getting pinned by modem
     * control interrupts
     */
    if (ds->flap_enabled) {
	/*
	 * We've got a runaway interface. Choo Choo!!
	 */
	if (!ds->flap_backoff)
	    errmsg(&msgsym(LINEFLAP, MK5), idb->unit);
	
	if (++ds->flap_backoff > SERIAL_FLAP_BACKOFF) {
	    /*
	     * We're finished with the probation period
	     */
	    ds->flap_count = 0;
	    TIMER_STOP(ds->flap_time);
	    ds->flap_backoff = 0;
	    ds->flap_enabled = FALSE;
	    
	    mk5025_enable_modem_interrupt(ds);
	}
    }
    
    /*
     * Poll the modem control registers. Prevents us
     * missing a state change due to a missed interrupt
     * or if we've enabled flap protection 
     */
    (void)mk5025_get_interface_state(idb, TRUE);
    
    /*
     * Check the MK5025 CSR0 register
     * If it should be running, read CSR0 and ensure that it is
     * running and has a normal value. Otherwise, reset it.
     */
    if (idb->state == IDBS_UP && !reg_invoke_dialer_spoofing_int(idb)) {
	ushort csr0;
	
	ds->regaddr->rap = MK5025_CSR0;
	csr0 = ds->regaddr->csr;
	if ((csr0 & MK5025_CSR0_STOP) ||
	    (csr0 & MK5025_CSR0_NORMAL) != MK5025_CSR0_NORMAL) {
	    if (serial_debug)
		buginf("\nMK5(%d): Bad state in CSR0=(0x%x)\n",
		       idb->unit, csr0);
	    mk5025_init(idb);
	    idb->counters.resets++;
	}
    }
    reset_interrupt_level(SR);
}


/*
 * Show the XID/Test descriptors:
 */
static boolean mk5025_show_xid_test (mk5025_initblock_t *ib)
{
    printf("XID/Test TX desc at 0x%x%04x, status=0x%x, max_buffer_size=%d, packet_size=%d\n",
	   ib->xid_tx_desc.buffer_high, ib->xid_tx_desc.buffer_low,
	   ib->xid_tx_desc.status,
	   MK5025_GET_NEG_W(ib->xid_tx_desc.max_buffer_size),
	   MK5025_GET_NEG_W(ib->xid_tx_desc.packet_size));
    printf("XID/Test RX desc at 0x%x%04x, status=0x%x, max_buffer_size=%d, packet_size=%d\n",
	   ib->xid_rx_desc.buffer_high, ib->xid_rx_desc.buffer_low,
	   ib->xid_rx_desc.status,
	   MK5025_GET_NEG_W(ib->xid_rx_desc.max_buffer_size),
	   MK5025_GET_NEG_W(ib->xid_rx_desc.packet_size));
    return(FALSE);
}


/*
 *  Show the status buffer:
 */
static boolean mk5025_show_sba (mk5025_status_buffer_t *sb)
{
    printf("Status Buffer at 0x%x: rcv=%d, tcv=%d, local_state=%d, remote_state=%d\n",
	   sb, sb->rcv, sb->tcv, sb->local_state, sb->remote_state);
    printf("phase=%d, tac=%d, currd=0x%x%04x, curxd=0x%x%04x\n",
	   sb->phase, sb->tac, sb->currd_hi, sb->currd_lo, 
	   sb->curxd_hi, sb->curxd_lo);
    return(FALSE);
}


static ushort mk5025_get_register (mk5025_hw_reg_t *reg_addr, uint reg)
{
    leveltype SR;
    ushort value;

    process_sleep_for(1);		/* Let other processes run */
    SR = raise_interrupt_level(NETS_DISABLE);
    reg_addr->rap = reg;	/* Set RAP to desired register */
    value = reg_addr->csr;	/* Read desired register */
    reg_addr->rap = MK5025_CSR0; /* Leave RAP at CSR0 */
    reset_interrupt_level(SR);
    process_sleep_for(1);		/* Let other processes run again */
    return(value);
}


/*
 * Show the mk5025 registers:
 */
static boolean mk5025_registers (mk5025_hw_reg_t *reg_addr)
{

    printf("mk5025 registers: csr0 = 0x%04x, ",
	   mk5025_get_register(reg_addr, MK5025_CSR0));
    printf("csr1 = 0x%04x, ",
	   mk5025_get_register(reg_addr, MK5025_CSR1));
    printf("csr2 = 0x%04x\n",
	   mk5025_get_register(reg_addr, MK5025_CSR2));

    printf("                  csr3 = 0x%04x, ",
	   mk5025_get_register(reg_addr, MK5025_CSR3));
    printf("csr4 = 0x%04x, ",
	   mk5025_get_register(reg_addr, MK5025_CSR4));
    printf("csr5 = 0x%04x\n",
	   mk5025_get_register(reg_addr, MK5025_CSR5));

    return(FALSE);
}


/*
 * Show mk5025 errors:
 */
static void mk5025_show_errors (hwidbtype *idb)
{
    mk5025_instance_t *ds = INSTANCE;

    printf("%d user primitive errors, %d spurious primitive interrupts\n",
	   ds->user_prim_err, ds->spurious_prim_int);

    printf("%d provider primitives lost, %d unexpected provider primitives\n",
	   ds->prov_prim_lost, ds->spurious_prov_prim);
}


/*
 * Show a Receive ring entry:
 */
static void mk5025_show_rmd (paktype *pak, mk5025_rx_descriptor_t *rmd,
			     int n, uint iobase)
{
    uint data_start;
    char flag = ' ';

    data_start = iobase +
	         (uint)MK5025_MAKE_PTR(rmd->buffer_high, rmd->buffer_low);
    if (pak && pak->if_input && (pak->flags & PAK_INPUTQ))
	flag = '*';
    printf("%02d pak=0x%06x%c ds=0x%06x status=%02x max_size=%d pak_size=%d\n",
	   n, pak, flag, data_start, rmd->status,
	   MK5025_GET_NEG_W(rmd->max_buffer_size),
	   MK5025_GET_NEG_W(rmd->packet_size));

    if (pak && (pak->datagramstart == NULL)) {
	printf("Error: datagramstart = NULL!!!\n");
    }
    if (data_start & 1) {
	printf("Error: datagramstart is an odd address\n");
    }
}


/*
 * Show a Transmit ring entry:
 */
static void mk5025_show_tmd (paktype *pak, mk5025_tx_descriptor_t *tmd,
			     int n, uint iobase)
{
    uint data_start;

    data_start = iobase + 
	         (uint)MK5025_MAKE_PTR(tmd->buffer_high, tmd->buffer_low);
    printf("%02d pak=0x%06x ds=0x%06x status=0x%02x max_size=%d pak_size=%d\n",
	   n, pak, data_start, tmd->status,
	   MK5025_GET_NEG_W(tmd->max_buffer_size),
	   MK5025_GET_NEG_W(tmd->packet_size));

    if (pak && (pak->datagramstart == NULL)) {
	printf("Error: datagramstart = NULL!!!\n");
    }
    if (data_start & 1) {
	printf("Error: datagramstart is an odd address\n");
    }
}


/*
 * Show MK5025 status:
 */
static boolean mk5025_show_controller (hwidbtype *idb, parseinfo *csb)
{
    mk5025_instance_t *ds = INSTANCE;
    mk5025_initblock_t *ib;
    int i;
    char rx_head, tx_count, tx_head, tx_tail;
    mk5025_rx_ring_t *mk5025_rx_ring;
    mk5025_tx_ring_t *mk5025_tx_ring;
    paktype *rx_p[MAX_RING];
    paktype *tx_p[MAX_RING];
    leveltype SR;
    uint iobase;

    
    printf("MK5 unit %d, ", idb->unit);

    ib = ds->ib;

    /*
     * Get the base of IO memory. If the return code is -1, then
     * the function is unsupported and iobase taken to be zero.
     */
    iobase = (uint)mon_get_iobase();

    if ((int)iobase == -1)
	iobase = 0;
    else
	iobase = MK5025_16M_PAGE(iobase);
    
    mk5025_show_structure_info(idb);
    
    printf("IB at 0x%lx: mode=0x%04x, local_addr=%d, remote_addr=%d\n",
	   ib, ib->mode, ib->local_addr, ib->remote_addr);
    
    printf("N1=%d, N2=%d, scaler=%d, T1=%d, T3=%d, TP=%d\n",
	   MK5025_GET_NEG_W(ib->N1), MK5025_GET_NEG_B(ib->N2),
	   MK5025_GET_NEG_B(ib->scaler), MK5025_GET_NEG_W(ib->T1),
	   MK5025_GET_NEG_W(ib->T3), MK5025_GET_NEG_W(ib->TP));
    
    printf("buffer size %d\n", idb->max_buffer_size);
    
    mk5025_show_cable_state(idb); 
    
    /*
     * Allocate memory for the RX and TX rings:
     */
    mk5025_rx_ring = malloc(MK5025_RX_SIZE);
    if (!mk5025_rx_ring) {
	return(TRUE);
    }
    mk5025_tx_ring = malloc(MK5025_TX_SIZE);
    if (!mk5025_tx_ring) {
	free(mk5025_rx_ring);
	return(TRUE);
    }
    
    /*
     * Disable network interrupts and take a snapshot of the ring state:
     */
    SR = raise_interrupt_level(NETS_DISABLE);
    rx_head = ds->rx_head;
    bcopy(ds->rx_ra, mk5025_rx_ring,
	  (MAX_RING * sizeof(mk5025_rx_descriptor_t)));
    for (i = 0; i < ds->rx_size; i++)
	rx_p[i] = ds->rx_p[i];
    tx_count = ds->tx_count;
    tx_head = ds->tx_head;
    tx_tail = ds->tx_tail;
    bcopy(ds->tx_ra, mk5025_tx_ring,
	  (MAX_RING * sizeof(mk5025_tx_descriptor_t)));
    for (i = 0; i < ds->tx_size; i++)
	tx_p[i] = ds->tx_p[i];
    reset_interrupt_level(SR);
    
    printf("RX ring with %d entries at 0x%x%04x : RLEN=%d, Rxhead %d\n",
	   ds->rx_size, MK5025_GET_HI(ib->mk5025_rx_ring[0]),
	   ib->mk5025_rx_ring[1], MK5025_GET_RLEN(ib->mk5025_rx_ring[0]),
	   rx_head);
    for (i = 0; i < ds->rx_size; i++) {
	mk5025_show_rmd(rx_p[i],
			(mk5025_rx_descriptor_t *)
			&mk5025_rx_ring->ring_entry[i], i, iobase);
    }
    free(mk5025_rx_ring);
    
    printf("TX ring with %d entries at 0x%x%04x : TLEN=%d, TWD=%d\n",
	   ds->tx_size, MK5025_GET_HI(ib->mk5025_tx_ring[0]),
	   ib->mk5025_tx_ring[1], MK5025_GET_TLEN(ib->mk5025_tx_ring[0]),
	   MK5025_GET_TWD(ib->mk5025_tx_ring[0]));
    printf("tx_count = %d, tx_head = %d, tx_tail = %d\n",
	   tx_count, tx_head, tx_tail);
    for (i = 0; i < ds->tx_size; i++) {
	mk5025_show_tmd(tx_p[i],
			(mk5025_tx_descriptor_t *)
			&mk5025_tx_ring->ring_entry[i], i, iobase);
    }
    free(mk5025_tx_ring);
    
    if (mk5025_show_xid_test(ib))
	return(TRUE);
    if (mk5025_show_sba(ds->sb_ra))
	return(TRUE);
    
    printf("bad_frames=%d, frmrs=%d, T1_timeouts=%d, rej_rxs=%d, runts=%d\n",
	   ib->bad_frames, ib->frmrs, ib->T1_timeouts,
	   ib->rej_rxs, ib->runts);
    
    les_serial_show_errors(idb);

    mk5025_show_errors(idb);
    
    if (mk5025_registers(ds->regaddr))
	return(TRUE);
    
    return(FALSE);
}


/*
 * mk5025_txqlength
 * Returns the number of active packets in the transmit ring.
 */
static int mk5025_txqlength (hwidbtype *idb)
{
    mk5025_instance_t *ds = INSTANCE;

    return(ds->tx_count);
}


/*
 * Initialize interface.
 */
void mk5025_init (hwidbtype *idb)
{
    mk5025_rx_descriptor_t *rmd;
    mk5025_tx_descriptor_t *tmd;
    mk5025_status_buffer_t *sb;
    mk5025_rx_ring_t *rxring;
    mk5025_tx_ring_t *txring;
    mk5025_initblock_t *ib;
    mk5025_instance_t *ds;
    paktype *pak;
    leveltype SR;
    ushort csr1;
    int i;
    int rx_ring_size, old_rx_ring_size, tx_ring_size;
    int delay;
    boolean maxdgram_change = FALSE;

#ifdef DEBUG
    buginf("\nmk5025_init: maxdgram_change = %d", maxdgram_change);
#endif
    
    SR = raise_interrupt_level(NETS_DISABLE);

    MAYBE_DROPPED(idb);		/* May lose output from reset */

    /*
     * Has the MTU changed? If so, we need to get a new
     * ring of receive buffers
     */
    if (idb->pool && (idb->max_buffer_size != idb->pool->size))
	maxdgram_change = TRUE;

    /*
     * Yes, stop the MK5025 and count the reset:
     */
    ds = INSTANCE;
    mk5025_get_regaddr(idb);

    mk5025_disable_modem_interrupt(ds);

    mk5025_stop(idb, ds->regaddr);
    mk5025_reset(idb);  /* After stop, so we don't mess up a DMA */
    mk5025_stop(idb, ds->regaddr);

    if (idb->state != IDBS_ADMINDOWN) {
	/*
	 * Show that we are being reset:
	 */
	net_cstate(idb, IDBS_RESET);
    }

    if (serial_debug) {
	buginf("\nMK5(%d): Reset", idb->unit);
    }

    /*
     * Flush the hold queue:
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

    les_serial_init_offset(idb);

    /*
     * The MK5025 requires an even datagramstart address:
     */
    if ((uint)idb->start_offset & 1) {
#ifdef DEBUG
	if (serial_debug)
	    buginf("mk5025_init: Changing start_offset from 0x%x to even address 0x", idb->start_offset);
#endif	
	idb->start_offset &= BUFFER_WORD_ALIGN;
#ifdef DEBUG
	if (serial_debug)
	    buginf("%x\n", idb->start_offset);
#endif
    }

    /*
     * Initialize the variables for RX & TX ring access:
     */
    old_rx_ring_size = ds->rx_size;
    select_ring_size(&rx_ring_size, &tx_ring_size, idb->max_buffer_size);

    /* adjust rx_ring_size for SMDS encapsulation */
    if (idb->enctype == ET_SMDS)
        adj_smds_ring_size(&rx_ring_size);

    ds->rx_head = 0;
    ds->rx_size = make_ring_size(rx_ring_size);
    if (ds->rx_size == 0) {
	errmsg(&msgsym(BADRING, MK5), "receive");
	crashdump(0);
    }

    ds->tx_count = ds->tx_head = ds->tx_tail = 0;
    if (hwidb_use_short_tx_q_inline(idb)) {
	/*
	 * shorten TQL to make reponsive to prioritization
	 * but not so short it leaves air gaps on the wire
	 * when compressing or line is high speed
	 */
	    ds->tx_size = make_ring_size(2);
    } else {
	ds->tx_size = make_ring_size(tx_ring_size);
    }
    idb->mci_txcount = ds->tx_size;
    if (ds->tx_size == 0) {
	errmsg(&msgsym(BADRING, MK5), "transmit");
	crashdump(0);
    }

    /*
     * Clear error counters:
     */
    ib = ds->ib;
    ib->bad_frames = ib->frmrs = ib->T1_timeouts = ib->rej_rxs =
	ib->rej_txs = ib->runts = 0;

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
	    /*
	     * Does this RX ring entry already have a packet?
	     */
	    if (ds->rx_p[i]) {
		pak = (paktype *)ds->rx_p[i];
		
		/* Yes.  Get rid of it. */
		discard_buffer(pak);
		ds->rx_p[i] = 0;
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
    
    /*
     * Set up the RX ring in the initialization block and ensure
     * that it is full of buffers owned by the mk5025.
     *
     * If the MTU is changing, free all current buffers, and require
     * buffers of the new size. Otherwise, just give ownership of
     * the buffers back to the MK5025:
     */
    rxring = ds->rx_ra;
    ib->mk5025_rx_ring[0] = (MK5025_BUFF_HI(rxring) |
			     (log2n(ds->rx_size) << 12));
    ib->mk5025_rx_ring[1] = MK5025_BUFF_LO(rxring);
    
    for (i = 0; i < ds->rx_size; i++) {
	rmd = &rxring->ring_entry[i];
	rmd->packet_size = 0;	/* packet size is zero */
	rmd->max_buffer_size = -(idb->max_buffer_size);

	/*
	 * Does this RX ring entry already have a packet?
	 */
	if (ds->rx_p[i]) {
	    pak = ds->rx_p[i];

	    /*
	     * Yes, there is a packet. Is the MTU changing?
	     */
	    if (maxdgram_change) {
		/*
		 * Yes.  Discard our current buffers:
		 */
		discard_buffer(pak);
		ds->rx_p[i] = NULL;
	    } else {
		/*
		 * No. Give this packet back to the MK5025:
		 */
		pak->datagramstart = pak->data_area + idb->start_offset;
		rmd->buffer_low = MK5025_BUFF_LO(pak->datagramstart);
		rmd->buffer_high = MK5025_BUFF_HI(pak->datagramstart);
		rmd->status = MK5025_RMD_OWNA; /* MK5025 owns the buffer now */
	    }
	}
	/*
	 * If this ring entry doesn't have a packet yet, get one:
	 */
	if (!ds->rx_p[i]) {
	    pak = mk5025_getbuf(idb, rmd);
	    if (pak) {
		ds->rx_p[i] = pak;
	    } else {
		/*
		 * If we could not get a packet, mark our interface as
		 * down so we try again later:
		 */
#ifdef DEBUG	
		buginf("MK5(%d): RXRING mk5025_getbuf failed\n", idb->unit);
#endif
		net_cstate(idb, IDBS_DOWN);
		reset_interrupt_level(SR);
		return;
	    }
	}
    }

    /*
     * Set up the TX buffer ring and say that we own all of the
     * buffers so that the MK5025 does not try to transmit any of them:
     */
    txring = ds->tx_ra;
    ib->mk5025_tx_ring[0] =
	(MK5025_BUFF_HI(txring) | (MK5025_TWD << 8) |
	 (log2n(ds->tx_size) << 12));
    ib->mk5025_tx_ring[1] = MK5025_BUFF_LO(txring);
    for (i = 0; i < ds->tx_size; i++) {
	/*
	 * Initialize this TX ring entry:
	 */
	tmd = &txring->ring_entry[i];
	tmd->status = MK5025_TMD_SLF | MK5025_TMD_ELF | MK5025_TMD_TUI;
	tmd->max_buffer_size = -(idb->max_buffer_size);
	tmd->buffer_low = tmd->buffer_high = tmd->packet_size = 0;

	/*
	 * If this ring entry had a packet, free it:
	 */
	if (ds->tx_p[i]) {
	    pak = ds->tx_p[i];
	    datagram_done(pak);
	    ds->tx_p[i] = NULL;
	}
    }

    /*
     * If interface is administratively down, don't re-enable it. Note that
     * we perform this check after shutting down the MK5025 and after
     * setting up the rx and tx rings.
     */
    if (idb->state == IDBS_ADMINDOWN) {
        reset_interrupt_level(SR);
        return;
    }

    /*
     * Set up the XID/TEST TX Descriptor, even though we don't use it:
     */
    ib->xid_tx_desc.status = MK5025_TMD_SLF | MK5025_TMD_ELF; /* we own it */
    ib->xid_tx_desc.max_buffer_size = ib->xid_tx_desc.packet_size = 0;
    ib->xid_tx_desc.buffer_high = 0xFF;
    ib->xid_tx_desc.buffer_low = 0xFFFF;
    
    /*
     * Set up the XID/TEST RX descriptor, even though we don't use it:
     */
    ib->xid_rx_desc.buffer_high = 0xFF;	/* Cause error if we access it */
    ib->xid_rx_desc.buffer_low = 0xFFFF;
    ib->xid_rx_desc.status = 
	ib->xid_rx_desc.max_buffer_size =
	    ib->xid_rx_desc.packet_size = 0;

    /*
     * Set up the Status buffer Address:
     */
    sb = ds->sb_ra;
    ib->sba[0] = MK5025_BUFF_HI(sb);
    ib->sba[1] = MK5025_BUFF_LO(sb);

    /*
     * Set up the initialization Block:
     * for the mode, look up the MFS value
     */    

    delay = (idb->xmitdelay > 0) ? (idb->xmitdelay - 1) / 2 : 0;
    ib->mode = MK5025_MODE_NORMAL |
	(flag_table[delay] << MK5025_MODE_MFS_SHIFT);

    ib->N1 = -(idb->max_buffer_size); /* Max frame length */
    ib->N2 = -1;		/* No retransmissions */
    ib->scaler = -100;		/* One ms prescaler period */
    ib->T1 = -1000;		/* One second for retransmission timer */
    ib->T3 = -2000;		/* Two seconds for link idle timer */
    ib->TP = -1;

    /*
     * Select the proper interrupt wrapper:
     */
    mk5025_set_vectors(idb);
    
    /*
     * Initialize the serial config options specified in
     * NVRAM or via the network
     */
    mk5025_init_config_options(idb);
    
    /*
     * Finish initializing the MK5025 registers:
     */
    ds->regaddr->rap = MK5025_CSR4;
    ds->regaddr->csr = MK5025_CSR4_NORMAL;
    ds->regaddr->rap = MK5025_CSR3;
    ds->regaddr->csr = (ushort) ((ulong)ib & 0xFFFF);
    ds->regaddr->rap = MK5025_CSR2;
    ds->regaddr->csr = (ushort) (((ulong)ib >> 16) | MK5025_CSR2_NORMAL);
    ds->regaddr->rap = MK5025_CSR5;
    ds->regaddr->csr = (ushort)MK5025_CSR5_NORMAL;
    ds->regaddr->rap = MK5025_CSR1;
    ds->regaddr->csr = (ushort)MK5025_CSR1_UPRIM_INIT | MK5025_CSR1_UAV;
    
    /* 
     * Poll 10 times the mk5025 to check that init is complete.
     * It should respond with MK5025_CSR1_PAV set when done or if an 
     * error occurs
     */
    ds->regaddr->rap = MK5025_CSR1;
    for (i = 10; i > 0; i--) {
	csr1 = ds->regaddr->csr;
	if (csr1 & MK5025_CSR1_PAV) {
	    ds->regaddr->csr = MK5025_CSR1_PAV; /* Clear PAV */
	    break;
	}
	/*
	 * Wait 1 msec between polls
	 */
	DELAY(1);
    }

    if (i == 0) {
	errmsg(&msgsym(INITFAIL, MK5), idb->unit, 1, csr1);
	mk5025_shutdown(idb, IDBS_DOWN);
    } else if (csr1 & MK5025_CSR1_UERR) {
	errmsg(&msgsym(INITUERR, MK5), idb->unit, csr1);
	mk5025_shutdown(idb, IDBS_DOWN);
    } else if ((csr1 & MK5025_CSR1_PPRIM_MASK) !=
	       MK5025_CSR1_PPRIM_INIT_CONF) {
	errmsg(&msgsym(INITNOPPRIM, MK5), idb->unit, csr1);
	mk5025_shutdown(idb, IDBS_DOWN);
    } else {
        mk5025_enter_transparent_mode(idb);
	mk5025_enable_modem_interrupt(ds);
    }
    reset_interrupt_level(SR);		/* Restore interrupt level */
}
static boolean mk5025_hw_set_config (hwidbtype *hwidb, itemlist *ilist,
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
	    mk5025_init(hwidb);
	    break;

	  case SERIAL_HW_INVERT_TXC:
	    ssb->invert_txc = item->u.l_value;
	    mk5025_init(hwidb);
	    break;

	  case SERIAL_HW_SET_CLOCKRATE:
	    mk5025_clockrate_command(item->u.buffer);
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

static boolean mk5025_hwidb_state_config (hwidbtype *hwidb, ulong opcode,
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
	ret = mk5025_hw_set_config(hwidb, ilist, ssb);
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
 * Common IDB initialization routine:
 */
void mk5025_init_idb_common (hwidbtype *idb)
{
    mk5025_instance_t *ds;
    uchar *ds_base;
    serialsb *ssb;

    les_serial_init_idb_common(idb);

    idb->type = IDBTYPE_MK5025;
    idb->typestring = "MK5025";
    idb->xmitdelay = 0;

    /*
     * Set up function calls
     */
    idb->soutput = mk5025_tx_start;
    idb->reset = mk5025_restart;
    idb->shutdown = mk5025_shutdown;
    idb->enable = mk5025_enable;
    idb->setup_bridging = mk5025_setup_bridging;
    idb->device_periodic = mk5025_periodic;
    idb->txqlength = mk5025_txqlength;
    idb->show_controller = mk5025_show_controller;
    idb->fastsend = mk5025_fastsend;

    /*
     * idb->set_maxdgram must be initialized after calling 
     * les_serial_init_idb_common() so that hdlc_setup does not attempt
     * to call mk5025_set_maxdgram() while doing system init
     */
    idb->set_maxdgram = mk5025_set_maxdgram;

    reg_add_media_transmit_delay(IDBTYPE_MK5025,
				 mk5025_set_transmit_delay,
				 "mk5025_set_transmit_delay");

    reg_add_media_set_rxoffset(IDBTYPE_MK5025, mk5025_set_rxoffset,
				"mk5025_set_rxoffset");

    
    serial_idb_enqueue(idb);
    idb_enqueue(idb);

    /*
     * Set up state config vector
     */
    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);
    ssb->hw_super_state_config = idb->state_config;
    idb->state_config = mk5025_hwidb_state_config;
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * Allocate this instance of the driver:
     */
    ds_base = malloc_named(MK5025_DS_SIZE, "MK5025 Instance");
    if (!ds_base) {
	crashdump(0);
    } else {
	ds = (mk5025_instance_t *)(((ulong)ds_base + 7) & ~7L);
	INSTANCE = ds;

	/*
	 * Allocate an Init Block for this interface:
	 */
	ds->ib = malloc_named_iomem(MK5025_IB_SIZE, "MK5025 Init");
	if (!ds->ib) {
	    crashdump(0);
	}

	/*
	 * Allocate a receive ring for this interface:
	 */
	ds->rx_base = malloc_named_iomem(MK5025_RX_SIZE, "MK5025 RX Ring");
	if (ds->rx_base)
	    ds->rx_ra = (mk5025_rx_ring_t *)(((ulong)ds->rx_base + 7) & ~7L);
	else {
	    crashdump(0);
	}

	/*
	 * Allocate a transmit ring for this interface:
	 */
	ds->tx_base = malloc_named_iomem(MK5025_TX_SIZE, "MK5025 TX Ring");
	if (ds->tx_base)
	    ds->tx_ra = (mk5025_tx_ring_t *)(((ulong)ds->tx_base + 7) & ~7L);
	else {
	    crashdump(0);
	}
	
	/*
	 * Allocate a status buffer for this interface:
	 */
	ds->sb_base = malloc_named_iomem(sizeof(mk5025_status_buffer_t) + 8,
				   "MK5025 Status");
	if (ds->sb_base)
	    ds->sb_ra = (mk5025_status_buffer_t *)
		(((ulong)ds->sb_base + 7) & ~7L);
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
    }
}
