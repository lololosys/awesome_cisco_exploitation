/* $Id: if_les_bri.c,v 3.5.12.7 1996/09/05 18:09:41 chwhite Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_bri.c,v $
 *-----------------------------------------------------------------------
 * if_les_bri.c Common functions for BRI driver for XX/low-end. 
 *
 * March 1994 Manoj Leelanivas
 * Based on "if_les_bri.c" November 1991, Greg Stovall
 * Based on "if_les_mk5025.c", March 1990,  Chris Shaker
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *-----------------------------------------------------------------------
 * $Log: if_les_bri.c,v $
 * Revision 3.5.12.7  1996/09/05  18:09:41  chwhite
 * CSCdi67117:  Multicast fastswitching BRI->anywhere is broken
 * Branch: California_branch
 *
 * Revision 3.5.12.6  1996/08/28  12:58:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.12.5  1996/08/20  18:08:52  wmay
 * CSCdi39825:  ISDN PRI, setting d-channel mtu size not reflected on
 * b-channels - set for entire dialer rotary group
 * Branch: California_branch
 *
 * Revision 3.5.12.4  1996/08/13  02:20:05  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.5.12.3  1996/06/12  16:50:51  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - put b channel idbs in the idb list.  This removes a
 * lot of special initialization code for BRI.  Also, make the names
 * of the b-channels more consisted with PRI (ie: BRI0:1 and BRI0:2).
 * Branch: California_branch
 *
 * Revision 3.5.12.2  1996/05/09  14:34:02  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.12.1.18.1  1996/04/27  07:15:23  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.12.1.8.1  1996/04/08  01:58:52  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.12.1  1996/03/18  20:42:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.16.1  1996/03/28  17:35:08  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.5  1996/02/15  18:02:25  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.4  1995/12/06  18:30:56  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  17:40:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:14:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/11  14:00:35  albright
 * CSCdi37686:  Wrong IGRP metrics over BRI
 *
 * Revision 2.3  1995/06/21  03:01:08  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/19  06:47:33  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:35:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *-----------------------------------------------------------------------
 * $Endlog$ 
 */

#include "master.h"
#include "interface_private.h"
#include "../os/pool.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "logger.h"
#include "if_les.h"
#include "../wan/isdn.h"
#include "../if/network.h"
#include "../os/buffers.h"
#include "../if/if_vidb.h"
#include "isdn_bri.h"
#include "if_les_bri.h"
#include "msg_bri.c"			/* Not a typo, see logger.h */
#include "../parser/parser_defs_parser.h"


/*
 * =====================================================================
 * bri_enq_prim -
 *
 * Description:
 * This routine is common to the C4XXX as well
 * as the low end. This routine grabs an ISDN primitive element from
 * the IsdnL1PrimPoolQ, assigns the primitive value, and enqs into the
 * IsdnL1Q using p_enqueue. The ISDN process level should use a
 * p_dequeue to deq the element. Since it is called in a level4/level3
 * interrupt, it is made inline to prevent ftion call overhead.
 *
 * Paramters: idb_unit_no -> the unit # which identifies each idb uniquely.
 * 	      prim        -> the layer 1 primitive passed to isdn.
 *
 * Returns: nothing
 *
 * Notes: none.
 * =====================================================================
 */
void bri_enq_prim (uint idb_unit_no, uint prim)
{
    isdn_l1_primitive_t *ptr;	/* ptr to L1 primitive struct */

    /*
     * Deq a prim element from the global pool
     */
    ptr = p_dequeue(IsdnL1PrimPoolQ);
    if (!ptr) {
	errmsg(&msgsym(NOMEMORY, BRI), idb_unit_no,
	       "ISDN L1 Q elements. L1 message not enqueued to L2.");
	return;
    }

    /*
     * Assign values and enq into the isdn Q to inform
     * either L2 or ME.
     */
    ptr->dchan_num = idb_unit_no;
    ptr->prim = prim;
    p_enqueue(IsdnL1Q, ptr);
}


/*
 * =====================================================================
 * bri_init_isdn_queue -
 *
 * Description:
 * Common to XX/low end. Initialise the L1 queue
 * and primitives pool on startup. Should be called only one time.
 *
 * Parameters: nothing
 *
 * Returns:   nothing
 *
 * Notes:     Called only one time when the box is initialised from
 *            the analyze routine.
 * =====================================================================
 */
void bri_init_isdn_queue (void)
{
    isdn_l1_primitive_t *ptr, *ptr_next, *pool;
    int i = 0;
    
    /*
     * Allocate and init the L1 primitive Pool .
     */
    IsdnL1PrimPoolQ = malloc(sizeof(queuetype));
    if (!IsdnL1PrimPoolQ) {
	crashdump(0);
    }

    queue_init(IsdnL1PrimPoolQ, BRI_MAX_L1_PRIM);

    /*
     * Allocate memory for BRI_MAX_L1_PRIM entries in the IsdnL1PrimPoolQ.
     * Malloc a single chunk to reduce overhead from multiple mallocs.
     * Then enq each Q element into the IsdnL1PrimPoolQ.
     */
    pool = malloc(sizeof(isdn_l1_primitive_t) * BRI_MAX_L1_PRIM);
    if (!pool) {
	crashdump(0);
    }
    for (i = 0, ptr = pool; i < BRI_MAX_L1_PRIM; i++, ptr = ptr_next) {
	ptr_next = (isdn_l1_primitive_t *) ((char *)ptr +
					    sizeof(isdn_l1_primitive_t));
	/*
	 * Initialise values and enqueue into the Pool
	 */
	ptr->source = ISDN_BRI_L1;
	enqueue(IsdnL1PrimPoolQ, ptr);
    }

    /*
     * Now initialise the IsdnL1Q for passing primitives up to ISDN.
     */
    IsdnL1Q = malloc(sizeof(queuetype));

    if (!IsdnL1Q) {
	crashdump(0);
    }
    queue_init(IsdnL1Q, 0);
}


/*
 * =====================================================================
 * bri_set_maxdgram()
 *
 * Description:
 * Set the max datagram size for the D channel oand the two B 
 * channels of a BRI interface.
 *
 * Parameters:
 * idb - the D channel idb for the bri interface
 * maxdgram - the new max datagram size for this interface
 *
 * Returns:
 * a boolean value reflecting the success of the change
 * =====================================================================
 */
static boolean bri_set_maxdgram (hwidbtype *idb, int buffersize, int maxdgram)
{
   hwidbtype *dchan_idb;

   if ((idb->type != IDBTYPE_BRI) && (!ISBRILEASED(idb))) {
      printf(set_maxdgram_failed, maxdgram, idb->hw_namestring);
      return(FALSE);
   }

   /*
    * Whatever be the idb passed, find the dchannel idb and
    * then find the appropriate b-channel.
    */
   dchan_idb = idb->isdn_info->d_chan_idb;
   
   /*
    * change the maximum expected frame size for the B channels
    * of this interface. if the size didn't really change
    * this won't hurt.
    */

   if_maxdgram_adjust(dchan_idb->isdn_info->d_chan_idb, maxdgram);
   if_maxdgram_adjust(dchan_idb->isdn_info->b1_chan_idb, maxdgram);
   if_maxdgram_adjust(dchan_idb->isdn_info->b2_chan_idb, maxdgram);

   dchan_idb->isdn_info->d_chan_idb->maxmtu = idb->maxmtu;
   dchan_idb->isdn_info->b1_chan_idb->maxmtu = idb->maxmtu;
   dchan_idb->isdn_info->b2_chan_idb->maxmtu = idb->maxmtu;

   /*
    * Initialize the interface.
    */
   if (dchan_idb == idb)
       bri_init(idb, TRUE);
   return(TRUE);
}


/*
 * =====================================================================
 * init_bri_idb_common -
 *
 * Description:
 * The common idb initialisations for c4000 and low-end. Creates
 * dchannel and b channel idbs, stores the dchannel in idb Q.
 *
 * Parameters:
 * unit # of the interface.
 *
 * Returns:
 * idb ptr to dchan idb.
 * =====================================================================
 */
hwidbtype *init_bri_idb_common (uint unit)
{
    hwidbtype *idb;
    idbtype *swidb;
    hwidbtype *dchan_idb = NULL;
    isdn_bri_t *isdn_info;
    int bri_channel;
    
    for (bri_channel = 0; bri_channel < BRI_MAX_CHAN; bri_channel++) {
	
	/*
	 * Set up Storage, add IDB to interface queue
	 */
	idb = idb_create();
	if (!idb) {
	    return(NULL);
	}
	idb->status = 0;

	swidb = idb->firstsw;

	create_serial_sb(idb);
	
	/*
	 * Allocate memory for ISDN struct.
	 */
	isdn_info = malloc(sizeof(isdn_bri_t));
	if (!isdn_info) {
	    return(NULL);
	}
	idb->isdn_info = isdn_info;
	idb->isdn_info->primary_rate = FALSE;
	idb->isdn_info->leased_line = FALSE;
	
	/* Set up IDB channel pointers */
	switch (bri_channel) {
	case BRI_D_CHAN:
	    dchan_idb = idb;
	    idb->isdn_info->chantype = ISDN_Dchan;
	    idb->vc = idb->isdn_info->chan_num = BRI_D_CHAN;
	    break;
	    
	case BRI_B1_CHAN:
	    dchan_idb->isdn_info->b1_chan_idb = idb;
	    idb->isdn_info->chantype = B1;
	    idb->vc = idb->isdn_info->chan_num = BRI_B1_CHAN;
	    idb->status |= IDB_NO_NVGEN | IDB_VIRTUAL_HW;
	    break;
	    
	case BRI_B2_CHAN:
	    dchan_idb->isdn_info->b2_chan_idb = idb;
	    idb->isdn_info->chantype = B2;
	    idb->vc = idb->isdn_info->chan_num = BRI_B2_CHAN;
	    idb->status |= IDB_NO_NVGEN | IDB_VIRTUAL_HW;
	    break;
	}
	
	set_default_interface_bandwidth(swidb, VISIBLE_BRI_BANDWIDTH_64);
	/*
	 * Init the idb
	 */
	idb->isdn_info->d_chan_idb = dchan_idb;
	
	idb->status |= IDB_SERIAL | IDB_BCAST | IDB_HEARSELF | IDB_BRIDGE |
	    IDB_ISDN;
	idb->type = IDBTYPE_BRI;
	idb->typestring = idb->name = IFNAME_BRI;
	idb->state = IDBS_DOWN;
	GET_TIMESTAMP(idb->state_time);
	idb->error_threshold = 5000;
	idb->error_count = 0;
	idb->fast_switch_flag = 0;
	idb->pool = NULL;
	idb->pool_group = POOL_GROUP_INVALID;

	swidb->tbridge_media = TBR_HDLC_TRANSIT;

	/*
	 * Set up function calls
	 */
	bri_init_idb_vectors(idb);
	
	swidb->span_disabled = FALSE;
	
	/* 
	 * Set up arping (actually, turn off all arping)
	 */
	swidb->arp_arpa = swidb->arp_arpa_def = FALSE;
	swidb->arp_probe = swidb->arp_probe_def = FALSE;
	swidb->arp_snap = swidb->arp_snap_def = FALSE;
	
	idb->maxmtu = idb->maxmtu_def = MAXSERIALSIZE;
	idb->firstsw->sub_mtu = idb->maxmtu_def;

	holdq_init(idb, HDLC_HOLDQ);
	
	set_default_interface_delay(swidb, BRI_SERIAL_DELAY);
	idb->reliability = 255;
	idb->load = 1;
	idb->rxload = 1;
	
	/* 
	 * Save our unit number and a pointer to our IDB and interrupt routine:
	 */
	idb->unit = unit;
	idb->max_buffer_size = PAN_MAXDGRAM;
	idb->max_pak_size = PAN_MAXDGRAM;
	idb->max_pak_size_def = MAX_SERIAL_PAK;

	/*
	 * enqueue the idbs before creating the namestrings.  This is
	 * so we can use the VC when enqueueing, but take it away
	 * for the rest of the operation
	 */
	serial_idb_enqueue(idb);
	idb_enqueue(idb);
	/*
	 * Set some defaults for the b channels.  Set the idb->vc for
	 * the d-channel to NO_VC, so the name is bri 0 instead of
	 * bri 0:0
	 */
	if (bri_channel != BRI_D_CHAN) {
	    idb->input_defqcount = INPUTQ_DEFAULT;
	    idb->keep_period = KEEPALIVE_DEFAULT;
	    idb->keep_count = 0;
	} else {
	    idb->vc = NO_VC;
	}
	idb_init_names(swidb, FALSE);
	/*
	 * We can call hdlc_setup and isdn_setup as part of this
	 * loop now.
	 */
	hdlc_setup(NULL, idb);
	isdn_setup(idb);
    }
    
    dchan_idb->set_maxdgram = bri_set_maxdgram;
    dchan_idb->isdn_info->b1_chan_idb->set_maxdgram = bri_set_maxdgram;
    dchan_idb->isdn_info->b2_chan_idb->set_maxdgram = bri_set_maxdgram;
    
    /*
     * Tell ISDN code about D chan idb.The 2nd parameter basically
     * is a dchannel id being passed to the upper layer.
     */
    isdn_init_core(dchan_idb, dchan_idb->unit);
    return(dchan_idb);
}

