/* $Id: if_les_serial.c,v 3.3.32.2 1996/08/28 12:58:51 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_serial.c,v $
 *------------------------------------------------------------------
 * if_les_serial.c - Generic LES serial driver routines
 *
 * July 1995, Greg Stovall
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_les_serial.c,v $
 * Revision 3.3.32.2  1996/08/28  12:58:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.32.1  1996/03/18  20:42:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.6.2  1996/03/13  01:50:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.6.1  1996/01/24  22:24:32  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1996/01/10  06:48:56  syiu
 * CSCdi41302:  Concurrent LAT & bridging for HDLC and PPP
 *
 * Revision 3.2  1995/11/17  00:00:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/15  18:41:38  gstovall
 * Adding placholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "media_registry.h"
#include "interface_private.h"
#include "../os/pool.h"
#include "if_les.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../os/buffers.h"
#include "if_les_serial.h"
#include "logger.h"
#include "packet.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */



/*
 * les_serial_show_errors()
 * Shwo errors for the les serial interfaces
 */
void les_serial_show_errors (hwidbtype *idb)
{
    printf("%d missed datagrams, %d overruns\n",
	   idb->counters.input_resource, idb->counters.input_overrun);

    printf("%d bad datagram encapsulations, %d memory errors\n", 
	   idb->unknown_encaps, idb->dma_memory_error);

    printf("%d transmitter underruns\n", idb->counters.output_underrun);

}


/*
 * les_serial_init_idb_common ()
 * Init the les serial stuff in the idb
 */
void les_serial_init_idb_common (hwidbtype *idb)
{
    idbtype *swidb = idb->firstsw;

    idb->status = IDB_SERIAL|IDB_BCAST|IDB_HEARSELF|IDB_BRIDGE|IDB_SYNC;

    serial_idb_init(idb, MAXSERIALSIZE, PAN_MAXDGRAM,
		    VISIBLE_LES_SERIAL_BANDWIDTH, LES_SERIAL_DELAY);

    idb->state = IDBS_INIT;
    GET_TIMESTAMP(idb->state_time);
    idb->error_threshold = 5000;
    idb->error_count = 0;
    idb->fast_switch_flag = 0;    
    idb->serial_flags &= ~SERIAL_FLAGS_NO_CLOCKRATE;
    idb->pool = NULL;
    idb->pool_group = POOL_GROUP_INVALID;

    swidb->span_disabled = FALSE;

    /*
     * Set up how arping is done.  This isn't used for
     * serial links but is here for consistency.
     */
    swidb->arp_arpa = swidb->arp_arpa_def = FALSE;
    swidb->arp_probe = swidb->arp_probe_def = FALSE;
    swidb->arp_snap = swidb->arp_snap_def = FALSE;

    idb->reliability = 255;
    idb->load = 1;
    idb->rxload = 1;
}


/*
 * les_serial_setup_bridging
 * Configure the software multicast address filter for serial interfaces.
 */
void les_serial_setup_bridging (hwidbtype *hwidb)
{
    idbtype *swidb;

    if (hwidb->enctype == ET_HDLC || hwidb->enctype == ET_PPP) {
      if (!system_loading && !bootstrap_enable) {
        FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
        reg_invoke_media_interesting_addresses(swidb);
      }
      }
    }
}

/* 
 * les_serial_init_offset()
 * Initialize the start_offset for packet handling
 */
void les_serial_init_offset (hwidbtype *hwidb)
{
    /*
     * Set up start_offset
     */
    switch (hwidb->enctype) {
      case ET_HDLC:
      case ET_PPP:
	les_serial_setup_bridging(hwidb);
      case ET_SMDS:
      case ET_FRAME_RELAY:
      case ET_SDLCP:
      case ET_SDLCS:
      case ET_SDLC:
      case ET_STUN:
      case ET_BSTUN:
      case ET_LAPB:
      case ET_X25:
      case ET_ATM_DXI:
	hwidb->start_offset = ENCAPBYTES - hwidb->encsize;
	break;
      default:
	errmsg(&msgsym(BOGUSENCAP, LINK), hwidb->hw_namestring,
	       hwidb->enctype);
	hwidb->enctype = ET_HDLC;
	hwidb->start_offset = ENCAPBYTES - HDLC_ENCAPBYTES;
    }
}
