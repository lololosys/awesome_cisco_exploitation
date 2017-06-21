/* $Id: if_les_lance.c,v 3.10.12.10 1996/08/28 12:58:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_lance.c,v $
 *------------------------------------------------------------------
 * if_les_lance.c  AMD Lance chip ethernet driver for low end software
 *
 * December 1989, Chris Shaker
 * Based on "if_lance.c", June 1988,  Bill Westfield
 *
 * Copyright (c) 1988-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * PLEASE NOTE: I am now using the convention that RAP must be 0 whenever
 * interrupts are enabled so that the level 4 interrupt routine does not
 * have to write RAP = 0 to save time.
 *------------------------------------------------------------------
 * $Log: if_les_lance.c,v $
 * Revision 3.10.12.10  1996/08/28  12:58:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.12.9  1996/08/27  19:18:41  sdurham
 * CSCdi62282:  Router reloads when rmon native is configured, crash at
 * _bzero
 * Branch: California_branch
 * Temp fix - remove all output packet processing in rmon - removes the
 * crash
 *
 * Revision 3.10.12.8  1996/08/13  02:20:17  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.10.12.7  1996/08/05  22:21:33  nrvankay
 * CSCdi62232:  4500 Babble errors with no cause in sniffer traces
 * Branch: California_branch
 * Ignore the babble errors in the case AMD79970 due to the chip bug.
 *
 * Revision 3.10.12.6  1996/07/20  03:19:49  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.10.12.5  1996/07/18  21:52:50  bcole
 * CSCdi57941:  Windows95 reports duplicate IP address on network
 * Branch: California_branch
 *
 * Place interface in promiscuous mode while using ARP for address
 * discovery.  This way we can completely & transparently spoof the web
 * client.
 *
 * Revision 3.10.12.4  1996/05/24  20:42:55  sdurham
 * CSCdi55847:  IOS RMON agent only monitors incoming traffic
 * Branch: California_branch
 *          add service point for passing xmit packets to rmon
 *
 * Revision 3.10.12.3  1996/04/17  13:43:45  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.10.12.2  1996/04/06  00:50:16  vdukki
 * CSCdi48472:  Sustained process switched throughput degraded 28%
 * Branch: California_branch
 * Modified the lance_stop() function to add 100 microseconds delay
 * only for 6E NIM.
 *
 * Revision 3.10.12.1  1996/03/18  20:42:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.12.3  1996/03/13  01:49:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.9.12.2  1996/02/20  01:08:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/02/13  22:10:56  fred
 * CSCdi47956:  WFQ severely degrades RSRB performance
 *              drivers: increase TQL in short queue mode for link
 *              stability
 *              fastsrb_les.c: make sure pointers are there for queuing
 *              routines
 *
 * Revision 3.9.12.1  1996/01/24  22:24:16  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.9  1996/01/06  03:22:39  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.8  1995/12/08  01:16:00  gstovall
 * CSCdi45192:  6E NIM performance less than optimal
 * Issue a lance_stop() before configuring burst mode so the chip will
 * actually see it.  Also removed a spurious decrement of idb->resets.
 *
 * Revision 3.7  1995/12/07  22:30:27  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.6  1995/12/05  20:33:42  gstovall
 * CSCdi44875:  Lance RX handling slower than it should be
 * Remove extra cruft.
 *
 * Revision 3.5  1995/11/20  22:26:04  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.4  1995/11/20  18:27:54  speakman
 * CSCdi36541:  Disabling PIM does not clear multicast filter on LANCE.
 * Explicitly clear mc_filter_bits in the reset function.
 *
 * Revision 3.3  1995/11/17  17:40:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:32  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:14:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/08/09  16:38:42  heluthra
 * CSCdi37135:  DEC BRUT Router does not recognise ethernet (10baseT) down
 * Disconnecting the 10Base2 cable on Brut gives the excessive collision
 * error and does not show the line protocol as down. Added detection of
 * RTRY error (excessive collision) to bring the line protocol down.
 *
 * Revision 2.7  1995/08/08  19:47:25  fred
 * CSCdi38414:  congestion of compressed interfaces reduces speed.
 * when compressing data AND using WFQ/PQ/CQ, use a ring size of two, not
 * one.
 *
 * Revision 2.6  1995/07/09  21:11:50  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Fix a nit in a tbridge debugging message.
 *
 * Revision 2.5  1995/06/23  16:39:44  fred
 * CSCdi36295:  Commit Queue MIB to 11.0
 *
 * Revision 2.4  1995/06/21  08:55:28  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/21  03:01:14  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/19  06:47:42  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:36:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "exception.h"
#include "../if/ether.h"
#include "../if/ether_private.h"
#include "logger.h"
#include "../les/msg_lance.c"		/* Not a typo, see logger.h */
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "if_les.h"
#include "if_les_lance.h"  
#include "../if/network.h"
#include "../ip/ip.h"
#include "../dev/monitor1.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../if/linkdown_event.h"
#include "../if/ether_inline.h"
#include "../ip/remif_proto.h"
#include "if_les_lance_inline.h"
#include "if_les_ether_inline.h"
#include "if_les_ether.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */

#include "../smf/smf_public.h"
#include "../ui/debug.h"
#include "../tbridge/tbridge_debug.h"

#include "../rmon/rmon_task.h"
#include "../rmon/parser_defs_rmon.h"
#include "../if/traffic_shape.h"

/*
 * Lance subsystem header
 */

#define LANCE_MAJVERSION   1
#define LANCE_MINVERSION   0
#define LANCE_EDITVERSION  1

SUBSYS_HEADER(lance, LANCE_MAJVERSION, LANCE_MINVERSION, LANCE_EDITVERSION,
	      lance_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);


/*
 * Stop the Lance and reinitialize its registers.
 * Must be called with network interfaces disabled.
 */
static void lance_stop (lance_instance_t *ds)
{
    ds->lance_regaddr->rap = LANCE_CSR0;
    ds->lance_regaddr->csr = LANCE_CSR0_STOP;

    /* 
     * presidio requires a delay in the stop
     */

    if (ds->delay_vector)
        (*ds->delay_vector)(100);

    ds->lance_regaddr->rap = LANCE_CSR1;
    ds->lance_regaddr->csr = (ushort)((uint)ds->ib & 0xffff);
    ds->lance_regaddr->rap = LANCE_CSR2;
    ds->lance_regaddr->csr = (ushort)((ulong)ds->ib >> 16);
    ds->lance_regaddr->rap = LANCE_CSR3;
    ds->lance_regaddr->csr = LANCE_CSR3_NORMAL;
}


/*
 * Wrapper routine for idb->reset vector:
 */
static void lance_restart (hwidbtype *idb)
{
    leveltype SR;

    SR = raise_interrupt_level(NETS_DISABLE);

    holdq_reset(idb);		/* Flush input and output queues */
	
    if (idb->state != IDBS_ADMINDOWN) {
	lance_init(idb, FALSE);	/* Reset interface */
	idb->counters.resets++;
    }
    reset_interrupt_level(SR);
}


/*
 * lance_check
 * If it should be running, read CSR0 and ensure that it is
 * running and has a normal value. Otherwise, reset it.
 */
static void lance_check (hwidbtype *idb)
{
    lance_instance_t *ds;
    leveltype SR;
    ushort csr0;
    
    if (idb_is_throttled(idb))
	return;

    ds = INSTANCE;
    SR = raise_interrupt_level(ETHER_DISABLE);
    ds->lance_regaddr->rap = LANCE_CSR0;
    csr0 = ds->lance_regaddr->csr;
    reset_interrupt_level(SR);
    if ((csr0 & LANCE_CSR0_STOP) ||
	(csr0 & LANCE_CSR0_NORMAL) != LANCE_CSR0_NORMAL) {
	if (ether_debug)
	    buginf("\nLANCE(%d): Bad state in CSR0=(0x%x)\n", idb->unit, csr0);
	lance_init(idb, FALSE);
	idb->counters.resets++;
    }
    else {
      if (TIMER_RUNNING_AND_AWAKE(idb->xmittimer)) {
	TIMER_STOP(idb->xmittimer);
	if (csr0 & LANCE_CSR0_TDMD) {
	  errmsg(&msgsym(BADCABLE, LANCE), idb->unit); /* log error */
	  if (!idb->nokeepalive) {
	    if (idb->ethernet_lineup == TRUE) {
	      idb->ethernet_lineup = FALSE; /* Cable or terminator disconnect */
	      idb->reason = "Keepalive failed";
	      reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP,idb);
	    }
	  }
	}
      }
    }

}


/*
 * lance_shutdown
 * Shut down a lance interface
 */
static void lance_shutdown (hwidbtype *idb, int newstate)
{
    net_cstate(idb, newstate);

    lance_init(idb, FALSE);
    idb->counters.resets++;
}


/*
 * lance_add_multicast_address
 * Add another ethernet hardware address to the logical address filter
 */
static void lance_add_multicast_address (idbtype *swidb,
					 uchar *multicast_address,
					 uchar fsm)
{
    /*
     * Format of multicast_address on the wire:
     *
     *   47                                                   2   1   0
     *  +---+-----------------------------------------------+---+---+---+
     *  |   |                                               |   | L | M |
     *  +---+-----------------------------------------------+---+---+---+
     *		L = locally administered bit
     *		M = Multicast bit
     *
     * Format of multicast_address in 68020 memory:
     *
     *                                   L    M
     *	+----+----+----+----+----+----+----+----+
     *	| 07 | 06 | 05 | 04 | 03 | 02 | 01 | 00 |  byte 0
     *	+----+----+----+----+----+----+----+----+
     * 
     *	+----+----+----+----+----+----+----+----+
     *	| 15 | 14 | 13 | 12 | 11 | 10 | 09 | 08 |  byte 1
     *	+----+----+----+----+----+----+----+----+
     *			...
     *	+----+----+----+----+----+----+----+----+
     *	| 47 | 46 | 45 | 44 | 43 | 42 | 41 | 40 |  byte 5
     *	+----+----+----+----+----+----+----+----+
     */

    ushort *wptr, addrw, bitmask, addrbit, crcbit;
    uchar wire_address[IEEEBYTES], *cptr, hash;
    int i, j, t, bitshift, bitvalue, filw, filb;
    uint crc;
    hwidbtype *idb;
    lance_initblock_t *ib;
    lance_instance_t *ds;
    leveltype status;

    status = raise_interrupt_level(ETHER_DISABLE);
    idb = swidb->hwptr;
    ds = INSTANCE;
    ib = ds->ib;

    /*
     * Reverse the bits in each byte of the ethernet address so
     * that the bit order is the same as it will be on the wire:
     */
    cptr = multicast_address;
    ieee_swap(cptr, wire_address);

    /*
     * Implement the algorithm as published in BASIC in the Lance manual:
     */
    crc = -1;
    wptr = (u_short *)wire_address;
    for (i = 0; i < 3; i++) {
	addrw = *wptr++;
	bitmask = 0x8000;
	bitshift = 15;
	for (j = 0; j < 16; j++) {
	    crcbit = (crc & 0x80000000) >> 31; /* crcbit = c(32) */
	    crc = crc << 1;		/* left shift crc by 1 */
	    addrbit = (addrw & bitmask) >> bitshift; /* get a(n) bit */
	    t = crcbit ^ addrbit;	/* t = c(32) xor a(n) : control bit */
	    if (t) {
		crc = crc ^ 0x04C11DB6;
		crc = crc | 1;		/* c(0) = 1 */
	    }
	    bitmask = bitmask >> 1;
	    bitshift = bitshift -1;
	}
    }

    /*
     * Compute the hash number:
     */
    hash = 0;
    bitvalue = 32;
    for (i = 0; i < 6; i++) {
	if (crc & 1)
	    hash = hash + bitvalue;
	bitvalue = bitvalue >> 1;
	crc = crc >> 1;
    }

    /*
     * Set the LAF bit:
     */
    filw = hash >> 4;
    filb = hash & 0xF;
    lance_put_short(&ib->mc_filter_bits[filw],
		    (lance_get_short(&ib->mc_filter_bits[filw], ds->mode) |
		     (1 << filb)),
		    ds->mode);
    reset_interrupt_level(status);
}


/*
 * lance_add_multicast_range
 * Add multicast address range to the logical address filter. 
 *
 * NOTE: Turns out that enabling just 256 IP multicast addresses
 * (0100.5e00.0000 - 0100.5e00.00ff) enables all filter bits. Therefore,
 * just enable all multicasts to support IP multicast routing.
 */
static void lance_add_multicast_range (idbtype *swidb, uchar *address,
				       uchar *mask)
{
    lance_initblock_t *ib;
    lance_instance_t  *ds;
    hwidbtype         *idb;
    int               i;
    leveltype         status;

    idb = swidb->hwptr;
    ds = INSTANCE;
    ib = ds->ib;

    status = raise_interrupt_level(ETHER_DISABLE);

    /*
     * Need to do a stop before changing the logical address filter will work. 
     */
    lance_stop(ds);

    for (i = 0; i < 4; i++) {
	lance_put_short(&ib->mc_filter_bits[i], 0xffff, ds->mode);
    }

    /*
     * Start the Lance.
     */
    ds->lance_regaddr->csr = LANCE_CSR0_IDON | LANCE_CSR0_INEA
	                    | LANCE_CSR0_STRT;

    reset_interrupt_level(status);
}


/*
 * Set up for bridging on this interface:
 */
static void lance_setup_bridging (hwidbtype *idb)
{
    lance_init(idb, FALSE);	/* Reset code will set it up */
}


/*
 * lance_resetpak
 * Set up the Lance RX ring entry to reuse the packet buffer:
 */
static void lance_resetpak (lance_rx_descriptor_t *bd, boolean mode)
{
    /*
     * Re-Init fields in the buffer descriptor
     */
    lance_put_byte(&bd->status, LANCE_RMD_OWN, mode);   /* lance owns the
							   buffer now */
}


/*
 * lance_throttle
 * Throttle the Lance to give process level code a chance to run
 */
static void lance_throttle (hwidbtype *idb)
{
    idb_mark_throttled(idb);

    /*
     * Re-initialize the Lance and driver with the receiver disabled:
     */
    lance_init(idb, FALSE);
}


/*
 * lance_getbuf_inline
 * Return a packet buffer initialized for use as a lance receive buffer.
 * Also initializes the lance RX ring entry to point to this packet.
 * This routine is in the interrupt path and is coded for speed.
 */
inline static paktype *lance_getbuf_inline (hwidbtype             *idb,
					    lance_rx_descriptor_t *bd,
					    boolean               cache_ok)
{
    uchar *start;
    paktype *pak = NULL;
    lance_instance_t *ds;

    ds = (lance_instance_t *) INSTANCE;

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
	    buginf("\nlance_getbuf: Invalid p=%x\n", pak);
	    return(NULL);
	}
	if (pak->next != NULL) {
	    buginf("\nlance_getbuf: buffer in list, p=%x\n", pak);
	    return(NULL);
	}
	if (pak->refcount != 1) {
	    buginf("\nlance_getbuf: refcount not one, p=%x\n", pak);
	    return(NULL);
	}
#endif

	/*
	 * Initialize start
	 */
	start = pak_center(pak) - ETHER_ARPA_ENCAPBYTES;

	/*
	 * Link the packet to the Lance receive ring and give the Lance
	 * ownership of the packet.  This routine assumes that the
	 * arriving frame will be an arpa encapsulated frame, and sets up
	 * the descriptors appropriately.
	 */
	LANCE_WRITE_RMD(bd, start, ds->mode);

	/*
	 * Initialize datagramstart:
	 */
	pak->datagramstart = start;
    	return(pak);
    } else {
	idb->counters.output_nobuffers++;
	return(NULL);
    }
}


/*
 * lance_getbuf
 *
 * Function call wrapper around inline function
 */
static paktype *lance_getbuf (hwidbtype *idb, lance_rx_descriptor_t *bd)
{
    return(lance_getbuf_inline(idb, bd, FALSE));
}


/*
 * Handle Lance RX interrupts.
 * This routine is in the interrupt path and is coded for speed.
 *
 * Please note that this routine is defined after lance_getbuf()
 * so that lance_getbuf() is expanded inline, like a macro.
 */
void lance_RX_interrupt (hwidbtype *idb)
{
    lance_instance_t *ds;
    lance_rx_descriptor_t *rmd;
    paktype **pak_ptr;
    ushort rmd_status;
    paktype *pak, *replacement;
    ulong count;
    uchar packets = 1;

    ds = INSTANCE;

    /*
     * Receive all of the input frames that we own:
     */
    if (sched_run_interval && AWAKE(sched_run_time)) {
	/*
	 * the scheduler has not run recently enough,
	 * so, throttle the Lance:
	 */
	lance_throttle(idb);
    } else
	for (rmd = ds->rx_head;
	    (LANCE_READ_RMD(rmd, rmd_status, count, ds->mode) &&
	    (packets++ < LANCE_MAX_SPIN));
	    rmd = ds->rx_head) {

	    /*
	     * Do this so that the descriptor byte count does not
	     * screw up.
	     */
	    lance_put_short(&rmd->max_buffer_size, -(idb->max_buffer_size),
			    ds->mode);
	    
	    pak_ptr = ds->head_rxp;	/* Save pointer to packet pointer */
	    pak = *ds->head_rxp++; /* Get packet pointer */
	    if (++ds->rx_head >= ds->rx_max) {
		ds->rx_head = ds->rx_ra->ring_entry;
		ds->head_rxp = ds->rx_p;
	    }

	/*
	 * Process only good packets which are not chained.
	 * If packets are chained, they are giants.
	 */
	pak->datagramsize = count;
	if (((rmd_status & (LANCE_RMD_ENP | LANCE_RMD_ERRBITS)) ==
	     LANCE_RMD_ENP) && (count <= idb->max_pak_size)) {

	    /*
	     * Attempt to get a new packet buffer to replace
	     * this packet buffer on the RX ring:
	     */
	    replacement = lance_getbuf_inline(idb, rmd, TRUE);
	    if (replacement) {
		*pak_ptr = replacement; /* save packet ptr */
		
		/*
		 * Default the input software IDB to the major
		 * interface. Subsequent tbridge code may determine
		 * a different value if sub-interface bridging is
		 * configured.
		 */
		set_if_input_inline(pak, idb->firstsw);
		
		les_ethernet_RX_interrupt(idb, pak);

		continue; /* next packet */
		
	    } else {
		
		/*
		 * We could not get a new buffer to replace this one on
		 * the lance receiver ring, so drop the frame instead of
		 * processing it.
		 */
		clear_if_input(pak);
		lance_resetpak(rmd, ds->mode);
		idb->counters.input_drops++;
		lance_throttle(idb);
		continue; /* next packet */
	    }
  	} else {
	    boolean error;
	    uchar rmd1;

	    /*
	     * We encountered an error while receiving this frame.
  	     * Was this buffer the last in a chain?
  	     */
	    error = FALSE;
  	    rmd1 = lance_get_byte(&rmd->status, ds->mode);
  	    if (rmd1 & LANCE_RMD_ENP) {
		if ((count >= MAX_LANCE) && (!(rmd1 & LANCE_RMD_ERR))) {
		    idb->counters.giants++; /* 2 */
		    error = TRUE;
		}
		if ((rmd1 & LANCE_RMD_FRAM) && (!(rmd1 & LANCE_RMD_OFLO))) {
		    idb->counters.input_frame++;
		    error = TRUE;
		}
		if ((rmd1 & LANCE_RMD_CRC) && (!(rmd1  & LANCE_RMD_OFLO))) {
		    idb->counters.input_crc++;
		    error = TRUE;
		}
		if (rmd1 & LANCE_RMD_BUFF) {
			idb->counters.input_overrun++;
		        ds->buffer_err++;
		        error = TRUE;
		}
	    } else {
		ds->no_enp++;
		if (rmd1 & LANCE_RMD_OFLO) {
		    idb->counters.input_overrun++;
		    ds->overflow_err++;
		    error = TRUE;
		}
		if (rmd1 & LANCE_RMD_BUFF) {
		    idb->counters.input_overrun++;
		    ds->buffer_err++;
		    error = TRUE;
		}
	    }

	    if (error)
		idb->counters.input_error++; /* Only 1 input error per packet */
	    clear_if_input(pak);
	    lance_resetpak(rmd, ds->mode); /* Give packet back to the Lance */

	    continue; /* next packet */
	}
    } /* End of 'for' loop */
}


/*
 * Check the given output ring entry for transmit errors.
 * Take the correct action for each error:
 */
static void lance_check_tx_errors (
    lance_tx_descriptor_t *tmd,
    hwidbtype *idb)
{
    lance_instance_t		*ds;
    uchar			status;
    ushort			status2;
    ethersb			*esb;

    ds = INSTANCE;
    esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);

    status = lance_get_byte(&tmd->status, ds->mode);
    status2 = lance_get_short(&tmd->status2, ds->mode);
    
    if ((status & LANCE_TMD_ONE) && (!(status2 & LANCE_TMD3_LCOL))) {
	 esb->output_one_collision++;
    }
    if (status & LANCE_TMD_MORE) {
	esb->output_more_collision++;
    }
    if (status & LANCE_TMD_DEF) {
        esb->output_deferred++;
    }
    if (status & LANCE_TMD_ERR) {
	idb->counters.output_error++;
	if ((status2 & LANCE_TMD3_BUFF) &&
	    (!(status2 & (LANCE_TMD3_LCOL | LANCE_TMD3_RTRY)))) {
	    ds->tx_buff++;
	    /* The Lance disables the transmitter when this error occurs */
	    ds->fatal_tx_err = TRUE;
            /* NmEvent Hook 3 */
            idb->failCause = Ether_FatalTxError;
	}
	if (status2 & LANCE_TMD3_UFLO) {
	    idb->counters.output_underrun++;
	    errmsg(&msgsym(UNDERFLO, LANCE), idb->unit);
	    /* The Lance disables the transmitter when this error occurs */
	    ds->fatal_tx_err = TRUE;
            /* NmEvent Hook 3 */
            idb->failCause = Ether_FatalTxError;
	}
	if (status2 & LANCE_TMD3_LCOL) {
	    esb->output_late_collision++;
	    errmsg(&msgsym(LATECOLL, LANCE), idb->unit);
            /* NmEvent Hook 5 */
            idb->failCause = Ether_LateCollision;
	}
	if (status2 & LANCE_TMD3_LCAR) {
	    esb->output_lost_carrier++;
	    if (!idb->nokeepalive) {
		if (idb->ethernet_lineup == TRUE) {
		    idb->ethernet_lineup = FALSE; /* Lost carrier, not up */
		}
	    }    
	    if (ether_debug)
		errmsg(&msgsym(LOSTCARR, LANCE), idb->unit);
            /* NmEvent Hook 2 */
           idb->failCause = Ether_LostCarrier;
	}
	if (status2 & LANCE_TMD3_RTRY) {
	    esb->output_excessive_collision++;
	    ds->tdr = status2 & LANCE_TMD3_TDR;
	    errmsg(&msgsym(COLL, LANCE), idb->unit, ds->tdr);

            /* NmEvent Hook 4 */
            idb->failCause = Ether_ExcessiveCongestion;
            if (!idb->nokeepalive) {
                if (idb->ethernet_lineup == TRUE) {
                    if (++ds->rtry_error_count > 9) {
                        idb->ethernet_lineup = FALSE; /* excessive rtry */
		    }
		}
	    }
	}
	lance_put_short(&tmd->status2, 0, ds->mode);
    } else {
	les_ethernet_line_up(idb);
	ds->rtry_error_count = 0;
    }

    idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
} 


/*
 * Start or restart output.
 */
static void lance_tx_start (hwidbtype *idb)
{
    lance_tx_descriptor_t *tmd;
    lance_instance_t *ds;
    paktype **pak_ptr;
    uchar *start;
    paktype *pak;
    leveltype SR;
    ushort bytes;
    

    ds = INSTANCE;
    SR = raise_interrupt_level(ETHER_DISABLE);
    
    /*
     * While there is another packet to output and room for it in the
     * Lance TX queue, dequeue the packet:
     */
    while ((ds->tx_count < ds->tx_size) &&
	   (pak = (*idb->oqueue_dequeue)(idb))) {

	/*
	 * Is the packet too big to transmit?
	 */
	bytes = pak->datagramsize;
	if (bytes > idb->max_pak_size) {
	    idb->counters.output_total_drops++;
	    errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, bytes);
	    datagram_done(pak);
	    continue;
	}

	if (bytes < MINETHERSIZE) {
	    pak->datagramsize = MINETHERSIZE;
	    bytes = MINETHERSIZE;
	}

	/*
	 * Get pointers for the TX ring entry at tx_tail:
	 */
	tmd = ds->tx_tail;
	if ((lance_get_byte(&tmd->status, ds->mode) & LANCE_TMD_OWN) == 0) {

	    pak_ptr = ds->tail_txp;
	    start = pak->datagramstart;

	    *pak_ptr = pak;
	    if (*start & 1)
		idb->counters.output_broad++;

	    /*
	     * Set up this TX ring entry for this packet and force TX:
	     */
	    lance_put_short(&tmd->buffer_low,
			    LANCE_BUFF_LO(pak->datagramstart),
			    ds->mode);
	    lance_put_byte(&tmd->buffer_high,
			   LANCE_BUFF_HI(pak->datagramstart),
			   ds->mode);
	    lance_put_short(&tmd->packet_size, -bytes,
			    ds->mode);
	    lance_put_byte(&tmd->status,
			   LANCE_TMD_OWN | LANCE_TMD_STP | LANCE_TMD_ENP,
			   ds->mode);

	    /*
	     * Force the Lance to poll the TX ring and see this packet:
	     */
	    ds->lance_regaddr->csr =
		LANCE_CSR0_STRT | LANCE_CSR0_INEA | LANCE_CSR0_TDMD;

	    /*
	     * Do packet accounting after starting the Lance:
	     */
	    ds->tx_count++;
	    idb->counters.tx_cumbytes += bytes;
	    if (!TIMER_RUNNING(idb->xmittimer))
	        TIMER_START(idb->xmittimer, TIMEOUT); /* set timeout timer */
	    idb->counters.outputs++;

	    /*
	     * Is remote monitoring enabled?  If so, analyze the packet.
	     */
	    if (idb->rmon) {
		reg_invoke_rmon_output(idb, pak);
	    }

	    ds->tx_tail++;	/* Point to next ring entry */
	    ds->tail_txp++;	/* Point to next packet */
	    if (ds->tx_tail >= ds->tx_max) {
		ds->tx_tail = ds->tx_ra->ring_entry;
		ds->tail_txp = ds->tx_p;
	    }
	} else {

	    /*
	     * Error - we don't own the ring entry, try again later:
	     */
	    errmsg(&msgsym(OWNERR, LANCE), idb->unit, pak);
	    pak_requeue_head(idb, pak);
	    break;
	}
    }

    reset_interrupt_level(SR);
}


/*
 * Handle Lance TX interrupts:
 */
void lance_TX_interrupt (hwidbtype *idb)
{
    lance_tx_descriptor_t *tmd;
    lance_instance_t *ds;
    paktype **pak_ptr;
    paktype *pak;
    uchar tmd_status;

    /*
     * Process all transmitted packets:
     */
    ds = INSTANCE;

    for (tmd = ds->tx_head, pak_ptr = ds->head_txp, pak = *pak_ptr;
	 pak && (tmd_status = lance_get_byte(&tmd->status, ds->mode),
		 (tmd_status & LANCE_TMD_OWN) == 0);
	 tmd = ds->tx_head, pak_ptr = ds->head_txp, pak = *pak_ptr) {

	/*
	 * Check for transmit errors:
	 */
	if (tmd_status & (LANCE_TMD_ONE | LANCE_TMD_MORE |
		           LANCE_TMD_DEF | LANCE_TMD_ERR)) {
	    lance_check_tx_errors(tmd, idb);
	} else {
	    les_ethernet_line_up(idb);
	    ds->rtry_error_count = 0;
	}
	
	/*
	 * Do packet TX accounting and clean up:
	 */
	datagram_done(pak); /* Release the output packet */
	GET_TIMESTAMP(idb->lastoutput);
	*pak_ptr = 0;	/* Not pointing to a packet anymore */
	ds->tx_count--;
	
	ds->tx_head++;	/* Point to next ring entry */
	ds->head_txp++;	/* Point to next packet */
	if (ds->tx_head >= ds->tx_max) {
	    ds->tx_head = ds->tx_ra->ring_entry;
	    ds->head_txp = ds->tx_p;
	}
    }

    TIMER_STOP(idb->xmittimer); /* Reset TDMD timer used to ddetect cable disconnect */

    /*
     * Attempt to recover from any fatal TX errors we noticed:
     */
    if (ds->fatal_tx_err) {
	if (ether_debug)
	    buginf("\nLANCE(%d): Fatal transmit error on %s. Resetting...\n",
		   idb->unit, idb->hw_namestring);
	lance_init(idb, FALSE);
	idb->counters.resets++;
    }

    lance_tx_start(idb); /* Possibly restart output */
}


/*
 * Handle Lance Error interrupts:
 */
void lance_E_interrupt (ulong csr0_status, hwidbtype *idb)
{
    lance_instance_t *ds;
    ethersb *esb;
    
    /*
     * Process Error interrupts:
     */
    ds = INSTANCE;

    if (csr0_status & LANCE_CSR0_ERR) {
	esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);
        /*
         * Ignore babble errors for AMD79970. Check for Lance.
         */
	if (!ds->mode && (csr0_status & LANCE_CSR0_BABL)) {
	    idb->counters.output_error++;
	    esb->output_babbles++;
	    errmsg(&msgsym(BABBLE, LANCE), idb->unit, csr0_status);
	}
	if (csr0_status & LANCE_CSR0_MISS) {
	    idb->counters.input_error++;
	    idb->counters.input_resource++;
	}
	if (csr0_status & LANCE_CSR0_MERR) {
	    idb->dma_memory_error++;
	    errmsg(&msgsym(MEMERR, LANCE), idb->unit, 0, csr0_status);
	    lance_shutdown(idb, IDBS_DOWN);
	    ds->fatal_tx_err = TRUE;
	}
	idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
    }
    if (csr0_status & LANCE_CSR0_IDON) {
	ds->spurious_idon++;
	errmsg(&msgsym(SPURIDON, LANCE), idb->unit, 0, csr0_status);
	lance_shutdown(idb, IDBS_DOWN);
	ds->fatal_tx_err = TRUE;
    }
}


/*
 * lance_fastsend
 * Transmit this packet immediately or drop it
 * Called from interrupt level.
 */
static void lance_fastsend (hwidbtype *idb, paktype *pak)
{
    lance_tx_descriptor_t *tmd;
    lance_instance_t *ds;
    uchar *start;
    paktype *tx_pak;
    ushort bytes;
    ulong acct_proto;
    paktype **txp;
    traffic_shape_t *shape;

    if (idb->state != IDBS_UP) {
        idb->counters.output_total_drops++;
        datagram_done(pak);
        return;
    }

    bytes = pak->datagramsize;
    acct_proto = (ulong)pak->acct_proto;
    ds = INSTANCE;
    
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
     * If no packets are queued for output on this interface,
     * immediately transmit the packet:
     */
    if (!idb->output_qcount) {

	/*
	 * Get pointer to ring entry and packet pointer at tx_tail.
	 */ 
	tmd = ds->tx_tail;
	start = pak->datagramstart;
	
	/*
	 * Is there room for another packet in the output queue?
	 */
	if (ds->tx_count < ds->tx_size) {
	    if ((lance_get_byte(&tmd->status, ds->mode) &
		 LANCE_TMD_OWN) == 0) {
		
		/*
		 * Transmit our packet:
		 */
		if (*start & 1)
		    idb->counters.output_broad++; /* count broadcasts */
		
		/*
		 * Give the Lance ownership of the buffer now to reduce
		 * latency:
		 */
		lance_put_short(&tmd->buffer_low, LANCE_BUFF_LO(start),
				ds->mode);
		lance_put_byte(&tmd->buffer_high, LANCE_BUFF_HI(start),
				ds->mode);
		lance_put_short(&tmd->packet_size, -bytes,
				ds->mode);
		lance_put_byte(&tmd->status,
			       LANCE_TMD_OWN | LANCE_TMD_STP | LANCE_TMD_ENP,
			       ds->mode);
		/*
		 * Force the Lance to poll the TX ring and see this packet:
		 */
		ds->lance_regaddr->csr =
		    LANCE_CSR0_STRT | LANCE_CSR0_INEA | LANCE_CSR0_TDMD;
		
		/*
		 * Do packet accounting after starting the Lance:
		 */
		ds->tx_count++;
		idb->counters.tx_cumbytes_fast[acct_proto] += bytes;
		idb->counters.outputs_fast[acct_proto]++;
		if (!TIMER_RUNNING(idb->xmittimer))
		    TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */

		/*
		 * Is remote monitoring enabled?  If so, analyze the packet.
		 */
		if (idb->rmon) {
		    reg_invoke_rmon_output(idb, pak);
		}

		/*
		 * Update the tail pointer for the next packet we want to TX:
		 */
		txp = ds->tail_txp;
		*txp++ = pak;
		if (++ds->tx_tail >= ds->tx_max) { /* End of ring? */
		    ds->tx_tail = ds->tx_ra->ring_entry;
		    txp = ds->tx_p;
		}
		ds->tail_txp = txp;
		return;

	    } else {
		
		/*
		 * Error, we don't own the buffer:
		 */
		retbuffer(pak);
		idb->counters.output_total_drops++;
		return;
	    }
	} else if (ds->tx_tail == ds->tx_head) {
	    
	    /*
	     * We have a full TX queue. Is the first packet in the queue done?
	     * If so, we can service the TX interrupt on this packet and use
	     * the ring entry for our new packet:
	     */
	    if ((lance_get_byte(&tmd->status, ds->mode) &
		 LANCE_TMD_OWN) == 0) {
		tx_pak = *ds->tail_txp;

		/*
		 * Was this ring entry transmitting a packet?
		 */
		if (tx_pak) {
		    		    
		    /*
		     * Check for TX packet errors:
		     */
		    if (lance_get_byte(&tmd->status, ds->mode) &
			(LANCE_TMD_ONE | LANCE_TMD_MORE |
			 LANCE_TMD_DEF | LANCE_TMD_ERR)) {
			lance_check_tx_errors(tmd, idb);
		    } else {
			les_ethernet_line_up(idb);
			ds->rtry_error_count = 0;
		    }
		    /*
		     * Do TX packet accounting and clean up:
		     */
		    datagram_done(tx_pak); /* Release output packet*/
		}

		/*
		 * Now, we can transmit our new packet:
		 */
		if (*start & 1)
		    idb->counters.output_broad++; /* count broadcasts */
		
		/*
		 * Give the Lance ownership of the buffer and force TX:
		 */
		lance_put_short(&tmd->buffer_low, LANCE_BUFF_LO(start),
				ds->mode);
		lance_put_byte(&tmd->buffer_high, LANCE_BUFF_HI(start),
				ds->mode);
		lance_put_short(&tmd->packet_size, -bytes,
				ds->mode);
		lance_put_byte(&tmd->status,
			       LANCE_TMD_OWN | LANCE_TMD_STP | LANCE_TMD_ENP,
			       ds->mode);

		/*
		 * Force the Lance to poll the TX ring and see this packet:
		 */
		ds->lance_regaddr->csr =
		    LANCE_CSR0_STRT | LANCE_CSR0_INEA | LANCE_CSR0_TDMD;
		
		/*
		 * Do packet accounting after starting the Lance:
		 */

		/* Don't change ds->tx_count, it should stay the same */
		idb->counters.tx_cumbytes_fast[acct_proto] += bytes;
		idb->counters.outputs_fast[acct_proto]++;
		GET_TIMESTAMP(idb->lastoutput);
		if (!TIMER_RUNNING(idb->xmittimer))
		    TIMER_START(idb->xmittimer, TIMEOUT); /* set timeout timer */
		
		/*
		 * Is remote monitoring enabled?  If so, analyze the packet.
		 */
		if (idb->rmon) {
		    reg_invoke_rmon_output(idb, pak);
		}

		/*
		 * Update the tail pointer for the next packet we want to TX:
		 */
		txp = ds->tail_txp;
		*txp++  = pak;
		if (++ds->tx_tail >= ds->tx_max) {
		    ds->tx_tail = ds->tx_ra->ring_entry;
		    txp = ds->tx_p;
		}
		ds->tail_txp = txp;
		
		/*
		 * Have the next TX interrupt bypass this packet
		 * since we just took care of it:
		 */
		ds->head_txp++;	/* Point to next packet */
		if (++ds->tx_head >= ds->tx_max) {
		    ds->tx_head = ds->tx_ra->ring_entry;
		    ds->head_txp = ds->tx_p;
		}
		return;

	    } else {
		
		/*
		 * We have a full TX queue, so queue the packet in the output
		 * queue.
		 */
		holdq_enqueue_fast(idb, pak);
		lance_tx_start(idb);
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
	 * There are already packets on the output queue,
	 * so add this one to the end of the queue to minimize
	 * out of order situations:
	 */
	holdq_enqueue_fast(idb, pak);

	/*
         * If the software thinks the TX ring is full, go check with
         * the Lance (and in particular, the ownership bits, and see
         * if we can clear some space before pulling frames off of
         * the holdq by faking a TX interrupt. The TX interrupt
         * routine will call lance_tx_start() for us.
         *
         * This causes a smoother saturation roll-off
         */
        if (ds->tx_count == ds->tx_size)
            lance_TX_interrupt(idb);
        else
            lance_tx_start(idb);
    }
}


/*
 * lance_enable
 * Enable a Lance interface, most likely shut down for input congestion.
 * Please note; this function must be called with NETS_DISABLED, to 
 * work correctly.
 */
static void lance_enable (hwidbtype *idb)
{
    lance_instance_t *ds;
    
    idb_mark_unthrottled(idb);
    
    ds = INSTANCE;
    ds->enabled++;		/* Count number of times enabled */

    /*
     * Reset the lance with the receiver enabled:
     */
    lance_init(idb, FALSE);
}


/*
 * lance_periodic
 * periodically check whether the chip seems to be working.
 */
static void lance_periodic (hwidbtype *idb)
{
    les_ethernet_periodic(idb);

    if (idb->state == IDBS_UP)
	lance_check(idb);	/* Sanity check CSR0 */

    /*
     * Provide bells and whistle vendor support
     */
    lance_update_vendor_states(idb);
}


/*
 * Show a receive ring entry:
 */
static void lance_show_rmd (paktype *pak,
			    lance_rx_descriptor_t *rmd,
			    int n,
			    uint iobase,
			    boolean mode)
{
    uint data_start;

    data_start = iobase +
	(uint)LANCE_MAKE_PTR(lance_get_byte(&rmd->buffer_high, mode),
			     lance_get_short(&rmd->buffer_low, mode));
    
    printf("%02d pak=0x%06x ds=0x%06x status=0x%02x max_size=%d pak_size=%d\n",
           n, pak, data_start, lance_get_byte(&rmd->status, mode),
           LANCE_GET_NEG_W(lance_get_short(&rmd->max_buffer_size, mode)),
	   lance_get_short(&rmd->packet_size, mode));
    
    if (pak && (pak->datagramstart == NULL)) {
        printf("Error: datagramstart == NULL!!!\n");
    }
}


/*
 * Show a transmit ring entry:
 */
static void lance_show_tmd (paktype *pak,
			    lance_tx_descriptor_t *tmd,
			    int n,
			    uint iobase,
			    boolean mode)
{
    uint data_start;
    
    data_start = iobase +
	(uint)LANCE_MAKE_PTR(lance_get_byte(&tmd->buffer_high, mode),
			     lance_get_short(&tmd->buffer_low, mode));
    
    printf("%02d pak=0x%06x ds=0x%06x status=0x%02x status2=0x%04x pak_size=%d\n",
           n, pak, data_start, lance_get_byte(&tmd->status, mode),
	   lance_get_short(&tmd->status2, mode),
           LANCE_GET_NEG_W(lance_get_short(&tmd->packet_size, mode)));
    
    if (pak && (pak->datagramstart == NULL)) {
        printf("Error: datagramstart is NULL!!!\n");
    }
}


/*
 * Return the index into the array that the pointer accesses:
 */
static int lance_get_index (void *ptr, void *array, int element_size)
{
    if (ptr == array)
	return(0);
    return((((char *)ptr) - ((char *)array)) / element_size);
}


/*
 * Show Lance errors:
 */
static boolean lance_show_errors (hwidbtype *idb)
{
    lance_instance_t *ds = INSTANCE;

    les_ethernet_show_errors(idb);

    printf("%d tdr, %d spurious initialization done interrupts\n",
	   ds->tdr, ds->spurious_idon);
    printf("%d no enp status, %d buffer errors, %d overflow errors\n",
	   ds->no_enp, ds->buffer_err, ds->overflow_err);
    printf("%d tx_buff, %d throttled, %d enabled\n",
	   ds->tx_buff, idb->counters.throttle_count, ds->enabled);
    return(FALSE);
}


/*
 * Show Lance status. Return true if we quit the display.
 */
static boolean lance_show_controller (hwidbtype *idb, parseinfo *csb)
{
    lance_instance_t *ds = INSTANCE;
    uchar eth_addr[IEEEBYTES];
    lance_initblock_t *ib;
    ushort bs_hw_addr[3], hw_addr[3];
    int i;
    uchar *rx_head, *rxp, *tx_head, *tx_tail, *txp_head, *txp_tail;
    lance_rx_ring_t *lance_rx_ring;
    lance_tx_ring_t *lance_tx_ring;
    paktype *rx_p[MAX_RING];
    paktype *tx_p[MAX_RING];
    char tx_count;
    leveltype SR;
    uint iobase;
    
    /* 
     * use the mode boolean to determine whether to print Presidio or 
     * Lance info.  Makes this routine hardware dependent.... 
     */
    if (ds->mode) 
      printf("AM79970 unit %d ", idb->unit);
   else
      printf("LANCE unit %d, ", idb->unit);
    
    ib = ds->ib;

    /*
     * Get the base of IO memory. If the return code is -1, then
     * the function is unsupported and iobase taken to be zero.
     */
    iobase = (int)mon_get_iobase();
    
    if ((int)iobase == -1)
	iobase = 0;
    else
	iobase = LANCE_16M_PAGE(iobase);
    
    (*ds->show_structure_info) (idb);
    
    /*
     * Compute the current station address and default station address:
     */
    hw_addr[0] = lance_get_short((ushort *) &ib->hw_station_address[0],
				 ds->mode);
    hw_addr[1] = lance_get_short((ushort *) &ib->hw_station_address[1],
				 ds->mode);
    hw_addr[2] = lance_get_short((ushort *) &ib->hw_station_address[2],
				 ds->mode);
    
    bscopy((uchar *) hw_addr, bs_hw_addr, IEEEBYTES);
    lance_get_default_hw_address(idb, eth_addr);
    
    printf("IB at 0x%lx: mode=0x%04x, mcfilter %04x/%04x/%04x/%04x\n",
	   ib, lance_get_short(&ib->mode, ds->mode),
	   lance_get_short(&ib->mc_filter_bits[0], ds->mode),
	   lance_get_short(&ib->mc_filter_bits[1], ds->mode),
	   lance_get_short(&ib->mc_filter_bits[2], ds->mode),
	   lance_get_short(&ib->mc_filter_bits[3], ds->mode));
    
    printf("station address %e  default station address %e\n",
	   bs_hw_addr, eth_addr);
    
    printf("buffer size %d\n", idb->max_buffer_size);
    
    /*
     * Allocate memory for the RX and TX rings:
     */
    lance_rx_ring = malloc(LANCE_RX_SIZE);
    if (!lance_rx_ring) {
	return(TRUE);
    }
    lance_tx_ring = malloc(LANCE_TX_SIZE);
    if (!lance_tx_ring) {
	free(lance_rx_ring);
	return(TRUE);
    }
    
    /*
     * Disable network interrupts and take a snapshot of the ring state:
     */
    SR = raise_interrupt_level(ETHER_DISABLE);
    rx_head = (uchar *)ds->rx_head;
    rxp = (uchar *)ds->head_rxp;
    bcopy(ds->rx_ra, lance_rx_ring,
	  (MAX_RING * sizeof(lance_rx_descriptor_t)));
    for (i = 0; i < ds->rx_size; i++)
	rx_p[i] = ds->rx_p[i];
    tx_count = ds->tx_count;
    tx_head = (uchar *)ds->tx_head;
    tx_tail = (uchar *)ds->tx_tail;
    txp_head = (uchar *)ds->head_txp;
    txp_tail = (uchar *)ds->tail_txp;
    bcopy(ds->tx_ra, lance_tx_ring,
	  (MAX_RING * sizeof(lance_tx_descriptor_t)));
    for (i = 0; i < ds->tx_size; i++)
	tx_p[i] = ds->tx_p[i];
    reset_interrupt_level(SR);
    
    printf("RX ring with %d entries at 0x%x\nRxhead = 0x%x (%d), Rxp = 0x%x (%d)\n",
	   ds->rx_size, ds->rx_ra, rx_head,
	   lance_get_index(rx_head, ds->rx_ra, sizeof(lance_rx_descriptor_t)), 
	   rxp, lance_get_index(rxp, &ds->rx_p, sizeof(uchar *)));
    
    for (i = 0; i < ds->rx_size; i++) {
	lance_show_rmd(rx_p[i],
		       (lance_rx_descriptor_t *)
		       &lance_rx_ring->ring_entry[i], i, iobase, ds->mode);
    }
    free(lance_rx_ring);
    
    printf("TX ring with %d entries at 0x%x, tx_count = %d\n", 
	   ds->tx_size, ds->tx_ra, tx_count);
    printf("tx_head = 0x%x (%d), head_txp = 0x%x (%d)\n",
	   tx_head,
	   lance_get_index(tx_head, ds->tx_ra, sizeof(lance_tx_descriptor_t)),
	   txp_head,
	   lance_get_index(txp_head, &ds->tx_p, sizeof(uchar *)));
    printf("tx_tail = 0x%x (%d), tail_txp = 0x%x (%d)\n",
	   tx_tail,
	   lance_get_index(tx_tail, ds->tx_ra,sizeof(lance_tx_descriptor_t)),
	   txp_tail,
	   lance_get_index(txp_tail, &ds->tx_p, sizeof(uchar *)));
    
    for (i = 0; i < ds->tx_size; i++) {
	lance_show_tmd(tx_p[i],
		       (lance_tx_descriptor_t *)&lance_tx_ring->ring_entry[i],
		       i, iobase, ds->mode);
    }
    free(lance_tx_ring);
    
    if (lance_show_errors(idb))
	return(TRUE);
    
    SR = raise_interrupt_level(ETHER_DISABLE);
    ds->lance_regaddr->rap = LANCE_CSR0;
    i = ds->lance_regaddr->csr;
    reset_interrupt_level(SR);
    if (ds->mode)
	printf("Am79970 csr0 = 0x%x\n", i);
    else
	printf("Lance csr0 = 0x%x\n", i);
    
    return(FALSE);
}


/*
 * lance_txqlength
 * Returns the number of active packets in the transmit ring.
 */
static int lance_txqlength (hwidbtype *idb)
{
    lance_instance_t *ds = INSTANCE;

    return (ds->tx_count);
}

/*
 * Set the vectors in the instance to point to the approproiate
 * functions. 
 */
static void lance_set_default_vectors (hwidbtype *idb)
{
    lance_instance_t *ds;

    ds = (lance_instance_t *)INSTANCE;

    ds->get_regaddr = lance_get_regaddr;
    ds->reset = lance_reset;
    ds->set_vectors = lance_set_vectors;
    ds->show_structure_info = lance_show_structure_info;
    ds->ctrlr_pre_init = NULL;
    ds->ctrlr_post_init = NULL;
    ds->delay_vector = NULL;
}


/*
 * Initialize the Lance interface:
 * 
 * Warning!  The lance_init() routine is littered with calls to lance_stop(),
 * don't mess with them unless you know what you are doing.  They're needed
 * to make the Lance and Presidio happy.
 * 
 */
void lance_init (hwidbtype *idb, boolean maxdgram_change)
{
    lance_rx_descriptor_t *rmd;
    lance_tx_descriptor_t *tmd;
    lance_rx_ring_t *rxring;
    lance_tx_ring_t *txring;
    lance_initblock_t *ib;
    lance_instance_t *ds;
    paktype *pak = NULL;
    leveltype SR;
    int i;
    idbtype *swidb;
    ushort hw_station_address[3]; /* Hardware Enet address */
    int rx_ring_size, tx_ring_size;
    pooltype *lance_buffer_pool = NULL;
    boolean prom;

#ifdef DEBUG
    SAVE_CALLER();
#endif

    SR = raise_interrupt_level(ETHER_DISABLE);
    
    /*
     * stop the Lance and count the reset:
     */
    ds = INSTANCE;
    ds->fatal_tx_err = FALSE;
    
    lance_stop(ds);

    if (idb->state != IDBS_ADMINDOWN) {
	/*
	 * Show that we are being reset:
	 */
	net_cstate(idb, IDBS_RESET);
    }

#ifdef DEBUG
    buginf("\nLANCE(%d): Reset from 0x%x", idb->unit, caller());
#endif

    /*
     * Flush the hold queue:
     */
    holdq_reset(idb);

    /*
     * If interface is administratively down, don't re-enable it.
     * Note that we do this check after shutting down the Lance
     */
    if (idb->state == IDBS_ADMINDOWN) {
	idb->ethernet_lineup = TRUE;
	reset_interrupt_level(SR);
	return;
    }

    lance_stop(ds);
    (*ds->reset)(idb);	/* After stop, so we don't mess up a DMA */

    /*
     * Clear the variables used for RX and TX ring access:
     */
    select_ring_size(&rx_ring_size, &tx_ring_size, idb->max_buffer_size);

    ds->rx_size = make_ring_size(rx_ring_size);
    ds->rx_head = ds->rx_ra->ring_entry;
    ds->head_rxp = ds->rx_p;
    ds->rx_max = &ds->rx_ra->ring_entry[ds->rx_size];
    ds->rtry_error_count = 0;

    if (hwidb_use_short_tx_q_inline(idb)) {
	/*
	 * shorten TQL to make reponsive to prioritization
	 * but not so short it leaves air gaps on the wire
	 * when compressing or line is high speed
	 */
	ds->tx_size = make_ring_size(4);
    } else {
	ds->tx_size = make_ring_size(tx_ring_size);
    }
    idb->mci_txcount = ds->tx_size;
    ds->tx_head = ds->tx_tail = ds->tx_ra->ring_entry;
    ds->head_txp = ds->tail_txp = ds->tx_p;
    ds->tx_count = 0;
    ds->tx_max = &ds->tx_ra->ring_entry[ds->tx_size];

    ib = ds->ib;

    /*
     * The chip reset does NOT clear the mc_filter_bits.
     * This needs to be done explicitly here so that they
     * are clear after deconfiguring ip multicasting
     * or transparent bridging.
     */

    for (i = 0; i < 4; i++) {
	lance_put_short(&ib->mc_filter_bits[i], 0x0000, ds->mode);
    }

    /*
     * initialize the hardware addresses
     * If our station address has not already been set, default it to the
     * address given in the MCI cookie:
     */
    lance_get_default_hw_address(idb, idb->bia);
    if (is_ieee_zero(idb->hardware))
	lance_get_default_hw_address(idb, idb->hardware); 
    /* Swap bytes */
    bscopy(idb->hardware, (uchar *)hw_station_address, IEEEBYTES);

    /*
     * Now make sure the word swap gets done for the presidio.
     */
    lance_put_short((ushort *) &ib->hw_station_address[0],
		    hw_station_address[0], ds->mode);
    lance_put_short((ushort *) &ib->hw_station_address[1],
		    hw_station_address[1], ds->mode);
    lance_put_short((ushort *) &ib->hw_station_address[2],
		    hw_station_address[2], ds->mode);

    /*
     * Initialize private buffer pool if there isn't one.
     * Populate the pool, initialize a private buffer cache,
     * and populate the buffer cache with buffers from the newly
     * created private pool.  Also set up the fallback public buffer
     * pool. 
     */
    if (!idb->pool) {
        if (idb->pool_group == POOL_GROUP_INVALID)
	    idb->pool_group = pool_create_group();
	
	if (!idb->hw_namestring)
  	    idb_init_names(idb->firstsw, FALSE);

	lance_buffer_pool = pak_pool_create(idb->hw_namestring,
					    idb->pool_group,
					    idb->max_buffer_size, POOL_SANITY |
					    POOL_CACHE, NULL); 
	if (!lance_buffer_pool) {
	    crashdump(0);
	} 
	lance_buffer_pool->fallback =
	    pak_pool_find_by_size(idb->max_buffer_size);
	pool_adjust(lance_buffer_pool, 0, PRIV_NUM_BUF(ds->rx_size),
		    PRIV_NUM_BUF(ds->rx_size), TRUE);
	
	pak_pool_create_cache(lance_buffer_pool, PRIV_NUM_CACHE(ds->rx_size));
	pool_adjust_cache(lance_buffer_pool, PRIV_NUM_CACHE(ds->rx_size)); 
	
	idb->pool = lance_buffer_pool;
	
    }   

    /*
     * Set up the RX ring in the initialization block and ensure
     * that it is full of buffers owned by the lance:
     */
    rxring = ds->rx_ra;
    lance_put_short((ushort *) &ib->lance_rx_ring[0], LANCE_BUFF_LO(rxring),
		    ds->mode);
    lance_put_short((ushort *) &ib->lance_rx_ring[1],
		    (LANCE_BUFF_HI(rxring) | (log2n(ds->rx_size) << 13)),
		    ds->mode);

    for (i = 0; i < ds->rx_size; i++) {
	rmd = &rxring->ring_entry[i];
	lance_put_short(&rmd->packet_size, 0,
			ds->mode); /* packet size is zero */
	lance_put_short(&rmd->max_buffer_size, -(idb->max_buffer_size),
			ds->mode);

	if (ds->rx_p[i]) {
	    pak = ds->rx_p[i];
	    /*
	     * Give this packet to the lance
	     */
	    lance_put_short(&rmd->buffer_low,
			    LANCE_BUFF_LO(pak->datagramstart),
			    ds->mode);
	    lance_put_byte(&rmd->buffer_high,
			   LANCE_BUFF_HI(pak->datagramstart),
			   ds->mode);
	    lance_put_byte(&rmd->status,
			   LANCE_RMD_OWN,
			   ds->mode);/* LANCE owns the buffer now */
	}

        /*
	 * If this ring doesn't have a packet yet, get one:
	 */
        if (!ds->rx_p[i]) {
	    pak = lance_getbuf(idb, rmd);
	    if (pak) {
		ds->rx_p[i] = pak;
            } else {
	        /*
	         * If we could not get a packet, mark our interface as
	         * down so we try again later:
	         */
#ifdef DEBUG
	        buginf("\nLANCE(%d): RXRING lance_getbuf failed", idb->unit);
#endif
	        net_cstate(idb, IDBS_DOWN);
	        idb->ethernet_lineup = FALSE;
	        reset_interrupt_level(SR);
	        return;
	    }
	}
    }

    /*
     * Set up the TX ring and ensure that we own all of the packet
     * buffers so that the lance does not try to transmit any of them:
     */
    txring = ds->tx_ra;
    lance_put_short((ushort *) &ib->lance_tx_ring[0], LANCE_BUFF_LO(txring),
		    ds->mode);
    lance_put_short((ushort *) &ib->lance_tx_ring[1],
		    (LANCE_BUFF_HI(txring) | (log2n(ds->tx_size) << 13)),
		    ds->mode);
    for (i = 0; i < ds->tx_size; i++) {
	/*
	 * Initialize this TX ring entry:
	 */
	tmd = &txring->ring_entry[i];
	lance_put_byte(&tmd->status, LANCE_TMD_STP | LANCE_TMD_ENP,
		       ds->mode);
	lance_put_short(&tmd->buffer_low, 0, ds->mode);
	lance_put_byte(&tmd->buffer_high, 0, ds->mode);
	lance_put_short(&tmd->packet_size, 0, ds->mode);
	lance_put_short(&tmd->status2, 0, ds->mode);

	/*
	 * If this ring had a packet, free it:
	 */
	if (ds->tx_p[i]) {
	    pak = ds->tx_p[i];
	    datagram_done(pak);
	    ds->tx_p[i] = NULL;
	}
    }

    /*
     * Enable Reception for appropriate addresses and address groups.
     */

    /*
     * Set the Lance mode register:
     */
    lance_put_short(&ib->mode, 0, ds->mode);

    /*
     * If the interface is throttled, disable the receiver:
     */
    if (idb_is_throttled(idb))
	lance_put_short(&ib->mode,
			lance_get_short(&ib->mode, ds->mode)
			| LANCE_MODE_DRX,
			ds->mode);

    prom = FALSE;
    /*
     * Enable promiscuous listening if bridging is configured
     * on this interface and all those other caveats apply.
     */
    if (idb->tbridge_on_hwidb && bridge_enable &&
	!(system_loading | bootstrap_enable))
	prom = TRUE;
    
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	/*
	 * Refresh any MAC filters.
	 */
	reg_invoke_tbridge_smf_update(idb->hardware,
				      swidb,
				      SMF_MACTYP_OWN_UNICAST |
				      SMF_ACTION_RECEIVE);
	if (!(system_loading | bootstrap_enable))
	    reg_invoke_media_interesting_addresses(swidb);
    }
    /*
     * If rmon is enabled.
     */
    if (idb->rmon && idb->rmon->rmon_mode == RMON_PROMISCUOUS)
	prom = TRUE;

    /*
     * NOTE:  
     * This checks to see if we're tunneling this interface to a virtual
     * router running on a SGI workstation.  If we are, and we're bridging
     * then go into promiscuous mode.  We really aren't bridging on THIS
     * interface, but on the virtual interface.
     */
    if ((idb->remif != NULL) && (idb->remif->bridging_enabled))
	prom = TRUE;
    if (reg_invoke_media_if_should_use_promiscuous(idb))
	prom = TRUE;
    if (prom)
        lance_put_short(&ib->mode,
			lance_get_short(&ib->mode, ds->mode)
			| LANCE_MODE_PROM,
			ds->mode);
    lance_stop(ds);

    /*
     * If this is a presidio chip, do the presidio specific
     * reinitializations.
     */
    if (ds->ctrlr_pre_init)
	(*ds->ctrlr_pre_init) (idb);

    /*
     * Tell the Lance to read the initblock and initialize:
     */
    ds->lance_regaddr->rap = LANCE_CSR0;
    ds->lance_regaddr->csr = LANCE_CSR0_INIT;

    /* 
     * Poll 10 times the LANCE to check that init is complete
     */
    for (i = 10; i > 0; i--) {
	if (ds->lance_regaddr->csr & LANCE_CSR0_IDON)
	    break;
	/*
	 * Wait 1 msec between polls
	 */
	DELAY(1);
    }
    if (i > 0) {
	/*
	 * Do the Presidio specific post init sequence.
	 */

	if (ds->ctrlr_post_init) {
	    lance_stop(ds);
	    (*ds->ctrlr_post_init) (idb);
	}
	/*
	 * Start the Lance.
	 */
	ds->lance_regaddr->csr = LANCE_CSR0_IDON | LANCE_CSR0_INEA |
	    LANCE_CSR0_STRT;

	(*ds->set_vectors)(idb);
	net_cstate(idb, IDBS_UP);
	if (ether_debug)
	    buginf("\n%s: Interface is alive, phys. addr %e",
		   idb->hw_namestring, idb->hardware);
	reset_interrupt_level(SR);		/* restore interrupt level */
	return;
    }
    errmsg(&msgsym(INITFAIL, LANCE), idb->unit, 0, ds->lance_regaddr->csr);
    lance_shutdown(idb, IDBS_DOWN);
    idb->ethernet_lineup = FALSE;
    reset_interrupt_level(SR);		/* restore interrupt level */
}


/*
 * Common IDB initialization routine:
 */
void lance_init_idb_common (hwidbtype *idb, lance_vectors_t *vec)
{
    lance_instance_t *ds;
    uchar *ds_base;
    static first_time = TRUE;
    ulong idblist_type;

    if (first_time) {
	lance_parser_init();
	first_time = FALSE;
    }

    idb->type = IDBTYPE_LANCE;
    idb->typestring = "Lance";

    /*
     * Set up function calls
     */
    idb->soutput = lance_tx_start;
    idb->reset = lance_restart;
    idb->shutdown = lance_shutdown;
    idb->periodic = lance_periodic;
    idb->listen = lance_add_multicast_address;
    idb->listen_range = lance_add_multicast_range;
    idb->enable = lance_enable;
    idb->setup_bridging = lance_setup_bridging;
    idb->txqlength = lance_txqlength;
    idb->show_controller = lance_show_controller;
    idb->fastsend = lance_fastsend;

    les_ethernet_idb_init(idb);

    /*
     * Check for lance hwidb list if not there, create it.
     */
    if (idb_get_listhead(HWIDBLIST_LANCE) == NULL) {
	if (!idb_create_list(HWIDBLIST_LANCE, &idblist_type) ||
	    idblist_type != HWIDBLIST_LANCE) {
	    if (ether_debug) {
		idb_report_create_error("lance");
	    }
	    return;
	}
    }

    /*
     * Add the idb to the ether idb list
     */
    if (!idb_add_hwidb_to_list(idb, HWIDBLIST_LANCE)) {
	if (ether_debug) {
	    idb_report_add_error("lance");
	}
    }

    /*
     * Allocate this lance_instance of the driver:
     */
    ds_base = malloc_named_fast(LANCE_DS_SIZE, "Lance Instance");
    if (!ds_base) {
	crashdump(0);
    } else {
	ds = (lance_instance_t *)(((ulong)ds_base + 7) & ~7L);
	INSTANCE = ds;
	
 	/*
 	 * Set the proper vectors in the instance. 
 	 * Set to default lance vectors if the vector
	 * pointer is NULL. Otherwise, copy vectors into the
	 * instance.
 	 */
 	if (vec) {
	    ds->mode = TRUE; /* flag for presidio*/
	    ds->get_regaddr = vec->get_regaddr;
	    ds->reset = vec->reset;
	    ds->set_vectors = vec->set_vectors;
	    ds->show_structure_info = vec->show_structure_info;
	    ds->ctrlr_pre_init = vec->ctrlr_pre_init;
	    ds->ctrlr_post_init = vec->ctrlr_post_init;
	    ds->delay_vector = vec->delay_vector;
	} else
	    lance_set_default_vectors(idb);

 	(*ds->get_regaddr)(idb);
	if (ds->lance_regaddr == NULL) {
	    errmsg(&msgsym(BADUNIT, LANCE), idb->unit);
	    crashdump(0);
	}

        /*
         * Allocate an initialization block:
         */
        ds->ib = malloc_named_iomem(LANCE_IB_SIZE, "Lance Init");
        if (!ds->ib) {
	    crashdump(0);
        }

        /*
         * Allocate a receive ring:
         */
	ds->rx_base = malloc_named_iomem(LANCE_RX_SIZE, "Lance RX Ring");
        if (ds->rx_base)
	    ds->rx_ra = (lance_rx_ring_t *)(((ulong)ds->rx_base + 7) & ~7L);
        else {
	    crashdump(0);
        }

        /*
         * Allocate a transmit ring:
         */
        ds->tx_base = malloc_named_iomem(LANCE_TX_SIZE, "Lance TX Ring");
        if (ds->tx_base)
            ds->tx_ra = (lance_tx_ring_t *)(((ulong)ds->tx_base + 7) & ~7L);
        else {
	    crashdump(0);
        }
    }
}

/*
 * Set up for remote monitoring on this interface:
 */

void
lance_setup_rmon (hwidbtype *idb)
{
    lance_init(idb, FALSE);	/* Reset code will set it up */
}

      
