/* $Id: if_les_bri_c1000.c,v 3.6.12.10 1996/09/05 21:46:15 chwhite Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_bri_c1000.c,v $
 *-----------------------------------------------------------------------
 * if_les_bri_c1000.c Common functions for BRI driver for C1000. 
 *
 * NOTE: This file is a temporary hack for the 10.3 release, and will 
 * go away in Arizona.
 *
 * Feb 1995, Greg Stovall
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *-----------------------------------------------------------------------
 * $Log: if_les_bri_c1000.c,v $
 * Revision 3.6.12.10  1996/09/05  21:46:15  chwhite
 * CSCdi67117:  Multicast fastswitching BRI->anywhere is broken
 * Branch: California_branch
 *
 * Revision 3.6.12.9  1996/08/28  12:58:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.12.8  1996/08/20  18:08:55  wmay
 * CSCdi39825:  ISDN PRI, setting d-channel mtu size not reflected on
 * b-channels - set for entire dialer rotary group
 * Branch: California_branch
 *
 * Revision 3.6.12.7  1996/08/13  02:20:07  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.6.12.6  1996/07/23  13:25:50  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.6.12.5  1996/06/18  01:48:19  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.6.12.4  1996/06/12  16:50:54  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - put b channel idbs in the idb list.  This removes a
 * lot of special initialization code for BRI.  Also, make the names
 * of the b-channels more consisted with PRI (ie: BRI0:1 and BRI0:2).
 * Branch: California_branch
 *
 * Revision 3.6.12.3  1996/05/17  11:26:23  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.2  1996/04/26  07:56:20  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.2.1  1996/04/03  19:58:58  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.12.2  1996/05/09  14:34:04  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.12.1.18.1  1996/04/27  07:15:25  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.12.1.8.1  1996/04/08  01:58:55  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.12.1  1996/03/18  20:42:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.14.1  1996/03/28  17:35:10  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.6  1996/02/15  18:02:29  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.5  1996/02/08  20:12:26  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.4.16.1  1996/01/24  22:24:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/06  18:30:58  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  17:40:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:14:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/22  04:36:06  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.5  1995/09/12  11:58:20  albright
 * CSCdi37686:  Wrong IGRP metrics over BRI
 *
 * Revision 2.4  1995/07/18  06:47:32  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.3  1995/06/19 06:47:34  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:11:28  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:35:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *-----------------------------------------------------------------------
 * $Endlog$ 
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "packet.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../if/bri_debug.h"
#include "../wan/ppp.h"
#include "ttysrv.h"
#include "logger.h"
#include "if_les.h"
#include "../wan/isdn.h"
#include "../if/network.h"
#include "../if/if_vidb.h"
#include "isdn_bri.h"
#include "if_les_bri_c1000.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "../ui/command.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../if/priority_private.h"
#include "msg_bri.c"			/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include "../parser/parser_defs_parser.h"

/*
 * global defines
 */

queuetype test_bri_loopbackQ; /* Queue for to store packets which were looped
                                 back on the BRI under test */
queuetype test_bri_echoQ; /* ICMP echo queue used when testing bri interface */


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
 * Set the max datagram size for the D channel and the two B 
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

   if ((idb->type != IDBTYPE_BRI) && (!ISBRILEASED(idb))){
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
 * bri_drop_pak()
 *
 * Description:
 * Clear flags and up counters for a dropped packet.
 *
 * Parameters:
 * idb - the idb for the bri channel 
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void bri_drop_pak (hwidbtype *idb, paktype *pak)
{
    clear_if_input(pak);
    idb->counters.input_drops++;
    MAYBE_DROPPED(idb);
    datagram_done(pak);
}


/*
 * =====================================================================
 * bri_setup_bridging()
 *
 * Description:
 * Setup for bridging by re-initializing the interface.
 * This is an idb vector.
 *
 * Parameters:
 * idb
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void bri_setup_bridging (hwidbtype *idb)
{
    bri_init(idb, FALSE);
    if (idb->counters.resets > 0)
        idb->counters.resets--;
}


/*
 * =====================================================================
 * isdn_bri_reload_actions -
 *
 * Description:
 * Call this to keep all bri interfaces down while a reload, so
 * that remote entities can sense the connection being torn down.
 *
 * Parameters:
 * none.
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
static void isdn_bri_reload_actions (void)
{
    hwidbtype *idb;
    
    /*
     * Disable tp3420 interrupts for all subunits in the box having
     * a BRI interface, after powering the tp3420 down.
     */
    FOR_ALL_HWIDBS(idb) 
	if ((idb->type == IDBTYPE_BRI) &&
	    (idb->state != IDBS_ADMINDOWN)) {
            bri_disable(idb);
	}
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
	    /*
	     * All this virtual idb crap is insane.  The bandwidth of the
	     * D channel is only 16k, but we fake it out to be the bandwidth
	     * of one B channel (64k) because the D channel idb is the only
	     * idb visible to igrp, which uses the interface bandwidth for
	     * its routing calculations.
	     */
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
	switch (cookie->interfaces) {
	  case INTERFACE_1E1N:
	    idb->typestring = "BRI with integrated NT1";
	    break;
	  default:
	    idb->typestring = "BRI";
	    break;
	}
	idb->name = IFNAME_BRI;
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
	idb->max_pak_size_def = MAX_SERIAL_PAK;
	if_maxdgram_adjust(idb, PAN_MAXDGRAM);
	
	serial_idb_enqueue(idb);
	idb_enqueue(idb);
	/*
	 * Make a namestring for the B channels
	 */
	if (bri_channel != BRI_D_CHAN) {
	    idb->input_defqcount = INPUTQ_DEFAULT;
	    idb->keep_period = KEEPALIVE_DEFAULT;
	    idb->keep_count = 0;
	} else {
	    idb->vc = NO_VC;
	}

	idb_init_names(swidb, FALSE);
	    
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
    /*
     * Clean up before reload.
     */
    reg_add_will_reload(isdn_bri_reload_actions, "isdn_bri_reload_actions");

    return(dchan_idb);
}


/*
 * =====================================================================
 * bri_tint_state_handler()
 *
 * Description:
 * Handle the state transitions on the BRI S/T interface
 *
 * Parameters:
 * dchan_idb:		the dchan idb 
 *
 * Returns:
 * The SID state is handled
 * =====================================================================
 */
uchar bri_tint_state_handler (hwidbtype *dchan_idb)
{
    uchar sid_status;
    
    /*
     * Read the sid to get the current status
     */
    sid_status = read_sid(dchan_idb);

    if (bri_debug) {
	buginf("BRI(%d): TP3420 state info, got a 0x%x\n", dchan_idb->unit,
	       sid_status);
    }
	      
    /*
     * Perform actions on the basis of SID status
     */
    switch(sid_status) {
      case TP3420_SRF_LSD:	/* line signal detect */
	/*
	 * This is the case of remote activation when a line
	 * signal is detected. This state occurs when the SID
	 * receives an INFO 0 and the isdn state transitions to
	 * F3 from F2. Note that this interrupt status is valid
	 * only in F2.
	 * Inform ISDN and power up SID. If
	 * NT loopback enabled, loopback B channels, else
	 * clear all loopbacks.
	 */
	
	if (dchan_idb->isdn_info->leased_line || dchan_idb->state != IDBS_ADMINDOWN) {
	    if (dchan_idb->loopback) {
		write_sid(dchan_idb, TP3420_CRF_B1E);
		write_sid(dchan_idb, TP3420_CRF_B2E);
		write_sid(dchan_idb, TP3420_CRF_LBL1);
		write_sid(dchan_idb, TP3420_CRF_LBL2);
	    } else {
		write_sid(dchan_idb, TP3420_CRF_CAL);
		write_sid(dchan_idb, TP3420_CRF_B1D);
	    write_sid(dchan_idb, TP3420_CRF_B2D);
	    }
	    
	    write_sid(dchan_idb, TP3420_CRF_PUP);
	    bri_enq_prim(dchan_idb->unit, ISDN_MPH_CONNECT_IND);
	    dchan_idb->isdn_info->current_bri_l1_state = l1_state_F3;
	    if (bri_debug)
		buginf("BRI: Line signal detect for unit = %d, current state is F3. \n", dchan_idb->unit);
	}
	break;
	
      case TP3420_SRF_AP:	/* Activation Pending */
	/*
	 * This indication probably implies the reception of an
	 * INFO 2 signal. Send an Activation Request to SID.
	 * if previous state was F2 or F3, start T3 timer. If it was
	 * either F7 or F8, inform isdn about the Error Indication
	 * primitive.
	 */
	write_sid(dchan_idb, TP3420_CRF_AR);

	bri_stop_t4(dchan_idb);
	
	switch (dchan_idb->isdn_info->current_bri_l1_state) {
	    
	  case l1_state_F2:
	    bri_enq_prim(dchan_idb->unit, ISDN_MPH_CONNECT_IND);
	    
	  case l1_state_F3:
	    bri_start_t3(dchan_idb);
	    if (bri_debug)
		buginf("BRI: Starting T3 timer for unit = %d.\n",
		       dchan_idb->unit);
	    break;
	    
	  case l1_state_F7:
	    bri_enq_prim(dchan_idb->unit, ISDN_MPH_EI1_IND);
	    break;
	    
	  case l1_state_F8:
	    bri_enq_prim(dchan_idb->unit, ISDN_MPH_EI2_IND);
	    break;
	    
	  case l1_state_F4:
	  case l1_state_F5:
	  case l1_state_F6:	/* do nothing */
	    break;
	    
	  default:
	    buginf("BRI: Unknown State in unit %d.\n",
		   dchan_idb->unit);
	}

	dchan_idb->isdn_info->current_bri_l1_state = l1_state_F6;
	if (bri_debug)
	    buginf("BRI: Activation Pending for unit = %d, current state is F6. \n", dchan_idb->unit);
	break;
	
      case TP3420_SRF_CON:	/* d channel collision */
	bri_dchan_coll(dchan_idb);

	dchan_idb->isdn_info->d_chan_coll++;
	break;
	
      case TP3420_SRF_AI:	/* Activation Indication*/
	/*
	 * This indicates an INFO 4 has been received from
	 * the network and the layer 1 is ready to go.
	 * Remove T3 and deact timer entries, if present.
	 */
	bri_stop_t3(dchan_idb);
	bri_stop_t4(dchan_idb);
	
	bri_enq_prim(dchan_idb->unit, ISDN_PH_ACT_IND);

	switch (dchan_idb->isdn_info->current_bri_l1_state) {
	  case l1_state_F2:
	    bri_enq_prim(dchan_idb->unit, ISDN_MPH_CONNECT_IND);
	    break;
	  case l1_state_F8:
	    bri_enq_prim(dchan_idb->unit, ISDN_MPH_EI2_IND);
	    break;
	  case l1_state_F3:
	  case l1_state_F4:
	  case l1_state_F5:
	  case l1_state_F6:
	  case l1_state_F7:	/* do nothing */
	    break;
	  default:
	    buginf("BRI: Unknown State in unit %d.\n",
		   dchan_idb->unit);
	}
	
	/*
	 * activated
	 */
	dchan_idb->isdn_info->current_bri_l1_state = l1_state_F7;
	if (bri_debug) {
	    buginf("BRI: Activation for unit = %d, current state is F7. \n",
		   dchan_idb->unit);
	}
	
	break;
	
      case TP3420_SRF_DI: /* deactivation */
	/*
	 * This indicates reception of INFO 0 after a BRI
	 * is already activated. Start deactivation timer
	 * if we are in state F7 or F8, otherwise power down
	 * and deactivate the BRI.
	 */
	switch (dchan_idb->isdn_info->current_bri_l1_state) {
	  case l1_state_F7:
	  case l1_state_F8:
	    bri_start_t4(dchan_idb);
	    dchan_idb->isdn_info->current_bri_l1_state = l1_state_F2;
	    if (bri_debug)
		buginf("BRI: Starting DEACT timer for unit = %d.\n",
		       dchan_idb->unit);
	    break;
	    
	  case l1_state_F6:
	  case l1_state_F4:
	    bri_enq_prim(dchan_idb->unit, ISDN_PH_DEACT_IND);
	    
	  default:	/* NOTE: deliberate fall thru */
	    write_sid(dchan_idb, TP3420_CRF_DR);
	    write_sid(dchan_idb, TP3420_CRF_PDN);

	    /*
	     * Turn of timer T3 and deactivation timer
	     */
	    bri_stop_t3(dchan_idb);
	    bri_stop_t4(dchan_idb);

	    /*
	     * deactivated
	     */
	    dchan_idb->isdn_info->current_bri_l1_state = l1_state_F2;
	    if (bri_debug)
		buginf("BRI: Deactivation for unit = %d, current state is F2. \n", dchan_idb->unit);

	    break;
	}
	break;
	
      case TP3420_SRF_EI:  /* error indication */
	/*
	 * Indicates lost framing. Should happen only in
	 * F6 or F7 states.
	 */
	if ((dchan_idb->isdn_info->current_bri_l1_state == l1_state_F6) ||
	    (dchan_idb->isdn_info->current_bri_l1_state == l1_state_F7)){
	    bri_enq_prim(dchan_idb->unit, ISDN_MPH_EI1_IND);
	    dchan_idb->isdn_info->current_bri_l1_state = l1_state_F8; 
	}
	if (bri_debug)
	    buginf("BRI: Lost Framing for unit = %d, current state is F8. \n",
		   dchan_idb->unit);
	break;
	
      default:	/* should never happen */
	if (bri_debug) 
	    buginf("BRI: Unexpected  SID int. status reg = %x, unit = %d \n",
		   sid_status, dchan_idb->unit);
	
    }

    return(sid_status);
}


/*
 * =====================================================================
 * bri_display_layer1()
 *
 * Description:
 * Print out the layer 1 state information.
 *
 * Parameter:
 * dchan_idb:	The dchan idb.
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void bri_display_layer1 (hwidbtype *dchan_idb)
{
    /*
     * Print the layer 1 status.
     */
    switch (dchan_idb->isdn_info->current_bri_l1_state) {
      case l1_state_F1:
      case l1_state_F2:
      case l1_state_F3:
	printf("\nLayer 1 is DEACTIVATED. (ISDN L1 State F%d)\n",
	       dchan_idb->isdn_info->current_bri_l1_state);
	break;
      case l1_state_F4:
      case l1_state_F5:
      case l1_state_F6:
	printf("\nLayer 1 is PENDING ACTIVATION. (ISDN L1 State F%d)\n",
	       dchan_idb->isdn_info->current_bri_l1_state);
	break;
      case l1_state_F7:
      case l1_state_F8:
	printf("\nLayer 1 is ACTIVATED. (ISDN L1 State F%d)\n",
	       dchan_idb->isdn_info->current_bri_l1_state);
	break;
      default:
	printf("\nLayer 1 status is Unknown. (State F%d)\n",
	       dchan_idb->isdn_info->current_bri_l1_state);
    }
}


/*
 * =====================================================================
 * tp3420_enable_loopback -
 *
 * Description:
 * The function which initialises the tp3420 chip in loopback mode:
 * mode 0, disable loopback 
 * mode 1, enable  system loopback 
 * mode 2, enable  remote loopback  <--- TO BE DONE
 * mode 3, enable  plug   loopback
 *
 * Parameters:
 * dchan_idb - pointer to d channel idb
 * loopback_mode - loopback mode 
 *
 * Returns:
 * nothing
 *
 * Notes:
 * Loopback mode 2: remote loopback, i.e., NT loopback, where data recceived
 * from the network are looped back to the network was implemented before 
 * this function is created and is not implemented here. It use 'loopback'
 * field in idb structure, while system loopback and chip loopback
 * that are implemented here use 'loopback_mode' in isdn_info structure.
 * For the purpose of integrity, putting all these fields in one structure
 * is recommended.
 * =====================================================================
 */
static void tp3420_enable_loopback (hwidbtype *dchan_idb,
				    enum Loopback loopback_mode)
{
    static boolean loop_enabled = FALSE;
    
    switch (dchan_idb->isdn_info->loopback_mode) {
      case LOOPBACK_DISABLED:
        write_sid(dchan_idb, TP3420_CRF_CAL); /* clear all loopbacks*/
	
        /* 
         * If loopback was enabled before, power down TP3420 so that it is
         * back to the state before the loopback test for which the interface 
         * is manully shut down by a user.
         */
        if (loop_enabled) { 
            write_sid(dchan_idb, TP3420_CRF_PDN); 
        }
        break;
	
      case SYSTEM_LOOPBACK_ENABLED:
        write_sid(dchan_idb, TP3420_CRF_PDN); /* power down */
        write_sid(dchan_idb, TP3420_CRF_PUP); /* power up */
        write_sid(dchan_idb, TP3420_CRF_LBS); /* system loop */
        write_sid(dchan_idb, TP3420_CRF_B1E);
        write_sid(dchan_idb, TP3420_CRF_B2E);
        loop_enabled = TRUE;  
        break;

      case REMOTE_LOOPBACK_ENABLED:
        /*
         * TO BE DONE
         */
        break;
 
      case PLUG_LOOPBACK_ENABLED:
        write_sid(dchan_idb, TP3420_CRF_PDN);
        write_sid(dchan_idb, TP3420_CRF_NTF);
        write_sid(dchan_idb, TP3420_CRF_PUP);
        DELAY(2);
        write_sid(dchan_idb, TP3420_CRF_AR);
        write_sid(dchan_idb, TP3420_CRF_B1E);
        write_sid(dchan_idb, TP3420_CRF_B2E);
        loop_enabled = TRUE;  
        break;

      default:
        break;
    } 
}


/*
 * =====================================================================
 * test_bri_init() 
 *
 * Description:
 * Initialize the loopback queue and ICMP Echo queue used for testing a
 * bri interface. Also reset the loopback flag in isdn_bri structure.
 *
 * Parameters: 
 * dchan_idb  - the pointer to d channel idb 
 *
 * Returns:   
 * nothing
 *
 * Notes: called by bri_init
 * =====================================================================
 */
void test_bri_init (hwidbtype *dchan_idb)
{
    queue_init(&test_bri_loopbackQ, 2);
    queue_init(&test_bri_echoQ, 2);
    dchan_idb->isdn_info->loopback_mode = LOOPBACK_DISABLED;
    dchan_idb->isdn_info->b1_chan_idb->isdn_info->loopback_mode =
	LOOPBACK_DISABLED;
    dchan_idb->isdn_info->b2_chan_idb->isdn_info->loopback_mode =
	LOOPBACK_DISABLED;
}


/*
 * =====================================================================
 * test_bri_echoBLOCK
 *
 * Description:
 * Wait for 
 *  - an ICMP Echo returned by loopback, or 
 *  - timeout, or
 *  - an escape sequence.
 *
 * Parameters:
 * data - data structure for ICMP type.          
 *
 * Returns:
 * TRUE or FALSE: whether the process will be kept blocking or not.
 *
 * =====================================================================
 */
static boolean test_bri_echoBLOCK (echotype *data)
{
    if (!data->active)                    /* if block flag reset, wakeup */
        return(FALSE);
    if (psipending2((tt_soc *)data->tty)) /* interrupted? */
        return(FALSE);
    if (SLEEPING(data->timeout))          /* keep blocking if timer ticking */
        return(TRUE);
    return(FALSE);
}


/*
 * =====================================================================
 * test_bri_loopback_datagram_out
 *
 * Description:
 * Queue bri loopback-test output packet for the selected net.
 * We also timeout hung transfers.
 *
 * Parameters:
 * pak - pointer to the packet to be sent 
 *
 * Returns:
 * nothing 
 *
 * =====================================================================
 */
static void test_bri_loopback_datagram_out (paktype *pak)
{
    hwidbtype *idb = pak->if_output->hwptr;	/* addr of associated idb */
    unsigned acct_proto;		/* accounting protocol type */
    leveltype status;			/* lock out interrupts */
    boolean success;
    ulong acct_bytes;
    
    /*
     * Assume the data will be enqueued and subsequently delivered.
     */
    acct_proto = pak->acct_proto = ACCT_PROTO_IP;
    idb->counters.tx_frames[acct_proto]++;
    acct_bytes = pak->datagramsize;
    idb->counters.tx_bytes[acct_proto] += acct_bytes;
    status = raise_interrupt_level(NETS_DISABLE);
    
    /*
     *  We also have to maintain the other set of counters 
     */
    if (idb->soutput == (soutput_t)return_nothing) {
        /*
         * That means that we don't have "soutput" routine which should
         * take care of "output" counter
         */
        idb->counters.outputs++;
    }
    
    success = (*idb->oqueue)(idb, pak, TAIL);	/* add to output queue */
    reset_interrupt_level(status);
    
    if (!success) {
        /*
         * Output queue has filled up, or idb is set to an encapsulation
         * that performs its own idb->soutput, or idb is a virtual idb.
         * If output queue has really filled up, kick the soutput routine as
         * self protection against deadlock.
         */
        if (idb->oqueue == holdq_enqueue ||
            idb->oqueue == priority_enqueue) {
            idb->counters.tx_frames[acct_proto]--;
            idb->counters.tx_bytes[acct_proto] -= acct_bytes;
	    /* We also have to maintain the other set of counters */
            if (idb->soutput == (soutput_t)return_nothing) {
	        /* That means that we don't have "soutput" routine which 
		 * should take care of "output" counter
		 */
                idb->counters.outputs--;
            }
            (*(idb->soutput))(idb);
        }
    } else {
	(*idb->soutput)(idb);	/* start another output if pending */
    }
}


/*
 * =====================================================================
 * ping_test_for_bri
 *
 * Description:
 * Send a number of ICMP Echo Requests out an BRI interface and return
 * count of failures.  Prints a 'B1', 'B2', or 'D' on stdio on success of
 * B1, B2, and D channel loopback, respectively.
 *
 * Parameters:
 * idb - pointer to d channel idb                     
 *
 * Returns:
 * drops - count of failures
 *
 * Notes:
 * Will run until the repeat count reached or is aborted by the user.   
 * =====================================================================
 */
static int ping_test_for_bri (hwidbtype *idb)
{
    idbtype *swidb;
    echotype data;
    ulong size, repeat, seconds, maxsize;
    ulong hits, ix, jx;
    boolean hits_flag;
    ulong bri_channel; 
    int databytes;
    ushort *dataptr;
    
    isdn_bri_t *isdn_bri;
    
    iphdrtype *ip;
    icmptype *icmp;
    ipparamtype params;
    ushort pattern = 0xAA55;
    paktype *pak;                       /* packet to send */
    paktype *pak_copy;                  /* save a copy of the packet sent */   
    paktype *pak_in;                    /* paket received */
    int data_offset = ENCAPBYTES;
    int drops = 0; 
    
    /*
     * Set up pointers    
     */
    isdn_bri = idb->isdn_info;
    
    /*
     * Set default parameters.
     */
    size = BRI_TEST_DATA_SIZE;           /* default data size */
    repeat = BRI_TEST_DATA_COUNT;        /* default repeat count */
    seconds = BRI_TEST_DATA_GAP;         /* pause between packets in seconds */

    /*
     * For now, set default max datagram size to  be 1500.
     * It can be larger because the test
     * packet (ICMP echo request) will never get
     * fragmented with the current implementation
     * of loopback code.
     */
    maxsize = BRI_TEST_DATA_MAX_SIZE;
    
    /*
     * Ask for repeat count, size, and pause interval
     */
    if (!ping_common(&repeat, &size, (IPHEADERBYTES(NOPT) +
				      sizeof(icmptype)), maxsize, &seconds)) {
        printf("\nCan't get repeat count, size, or pause interval. Test aborted.");
        return(drops);
    }
    
    /*
     * Go for it.
     */
    printf(type_escape_seq_abort_msg);
    printf("\nSending %d, %d-byte ICMP Echos, timeout is %d seconds:",
	   repeat, size, seconds);
    printf("\n `%s' means `one loopback test passed on %s channel'", "B1", "B1");
    printf("\n `%s' means `one loopback test passed on %s channel'", "B2", "B2");
    printf("\n `%s' means `one loopback test passed on %s channel'", "D", "D");
    printf("\n `.' means `transmitted packet NOT equivalent to received packet on B1, B2, or D channel'"
           "\n `T' means `one timeout on receiving test packet on B1,B2, or D channel"
           "\nTest in progress. Please be patient.\n");
    process_sleep_for(100);	/* let output out */
    psion(stdio);
    
    for (ix = 0, hits = 0; (ix < repeat) && (psipending() == FALSE); ix++) {
        hits_flag = TRUE; 
	for (bri_channel=1; bri_channel<=BRI_MAX_CHAN; bri_channel++) {
            data.sequence = 0;
            data.tos = 0;
            data.options = NULL;
            
            if (!(pak = getbuffer(ENCAPBYTES + size))) {
                if (ix) {
                    ix--;
                } 
                goto test_done;
            }
	    
            pak->datagramsize = size; 
            pak->datagramstart = pak->data_area + data_offset;
	    
            data.id = ix % (MAXUSHORT + 1);
            ip = (iphdrtype *)ipheadstart(pak);
            /*
             * We initialize the packet buffer by zeroing the IP
	     * and ICMP header
             * and putting whatever the desired pattern
	     * in the rest of the packet.
             */
            databytes = size - IPHEADERBYTES(data.options) - sizeof(icmptype);
            if (databytes < 0)
                databytes = 0;
            icmp = (icmptype *)(ipheadstart(pak) +
				IPHEADERBYTES(data.options));
            dataptr = (ushort *)(icmp + 1);
            for (jx = 0; jx < btow(databytes) - 1; jx++)
                *dataptr++ = ~(pattern);
            memset(pak_center(pak), 0, size - databytes);
	    
            /*
             * Fill in the ICMP and IP portion of the datagram
             */
            icmp->type = ICMP_ECHO;
            icmp->code = 0;
            icmp->checksum = 0;
            icmp->identifier = data.id; 
            icmp->sequence = data.sequence; 
            icmp->checksum = ipcrc((ushort *)icmp,
				   (size - IPHEADERBYTES(data.options)));
	    
            params = ipdefaultparams;           /* struct copy */
            params.ident = (short)icmp_traffic.echosent;/* use for unique id*/
	    
            /*
             * Build the header.
             */
            ip->version = IPVERSION;        /* IP version number */
            ip->tos = params.tos;          /* type of service */
            ip->tl = size;                  /* length of packet */
 
            ip->id = params.ident;         /* packet ID */
            ip->fo = 0;                     /* no fragment offset */
            ip->ipreserved = 0;             /* reserved field */
            ip->dontfragment = params.dontfragment; /* may fragment? */
            ip->morefragments = FALSE;      /* no more fragments */
            ip->prot = ICMP_PROT;           /* protocol type */
            ip->ihl = btol(IPHEADERBYTES(data.options)); /* header length */
	    
            swidb = idb->firstsw;
            ip->srcadr = swidb->ip_address;     /* source address */
            ip->dstadr = swidb->ip_address;     /* destination address */
	    
            pak->if_output = swidb;
            pak->if_input = swidb;
            switch (bri_channel) {
	      case 1:
		pak->if_output->hwptr = isdn_bri->b1_chan_idb;
		break;
	      case 2:
		pak->if_output->hwptr = isdn_bri->b2_chan_idb;
		break;
	      case 3:
		pak->if_output->hwptr = isdn_bri->d_chan_idb;
		break;
	      default: 
		printf("\nWrong BRI channel. Test aborted.");
		datagram_done(pak);
		return(++drops);
            }
	    
            switch (pak->if_output->hwptr->enctype) {
	      case ET_HDLC:
		pak->datagramsize += HDLC_ENCAPBYTES;
		pak->datagramstart -= HDLC_ENCAPBYTES;
		if (pak->datagramstart < pak->data_area) {
		    printf("\nInvalid datagram start address. Test aborted.");
		    datagram_done(pak);
		    return(++drops);
		}
		pak->enctype = ET_HDLC;
		pak->refcount = 1;       
		break;
	      case ET_PPP:
		pak->datagramsize += PPP_ENCAPBYTES;
		pak->datagramstart -= PPP_ENCAPBYTES;
		if (pak->datagramstart < pak->data_area) {
		    printf("\nInvalid datagram start address. Test aborted.");
		    datagram_done(pak);
		    return(++drops);
		}
		pak->enctype = ET_PPP;
		pak->refcount = 1;       
		break;
	      default:
		printf("\nInvalid encapsulation. Test aborted.");
		datagram_done(pak);
		return(++drops);
            }
	    
            pak_copy = pak_duplicate(pak);
            if (!pak_copy) {
                datagram_done(pak);
                drops++;
                if (ix) {
                    ix--;
                }
                goto test_done;
            }
            icmp_traffic.echosent++;
	    
            data.next = NULL;
            data.active = TRUE;
            data.tty = stdio;
            TIMER_START(data.timeout, (ulong) seconds * ONESEC);
	    
            /*
	     * send ICMP echo packet out of the BRI interface under test
             */
            test_bri_loopback_datagram_out(pak);
	    
            /*
             * go to sleep and wait for ICMP Echo returned via loopback 
             */
            enqueue(&test_bri_echoQ, &data);
            edisms((blockproc *)test_bri_echoBLOCK, (ulong)&data);
            unqueue(&test_bri_echoQ, &data);
            if (!data.active) {
                /*
                 * It's either Echo packet or escape sequence has
		 * been received.
                 */
                if ((pak_in = pak_dequeue(&test_bri_loopbackQ))) {
                    /*
                     * Echo packet has been looped back and received
                     */
                    ip_traffic.inputs++;
                    icmp_traffic.echorcv++; /* echo received via loopback */
                    swidb = pak_in->if_input;
                    if (swidb) {
                        swidb->hwptr->counters.rx_frames[ACCT_PROTO_IP]++;
                        swidb->hwptr->counters.rx_bytes[ACCT_PROTO_IP] +=
			    pak->datagramsize;
                    }
                    /*
                     * compare transmitted packet and received packet. If they
                     * are the same, mark test successful.
                     */
                    if (bcmp(pak_copy->datagramstart, pak_in->datagramstart, 
			     pak_copy->datagramsize) == 0) {
                        hits_flag &= TRUE;      
                        printf("%s", (bri_channel == 1 ? "B1" :
				      (bri_channel == 2 ? "B2" : "D")));
                    } else {
                        hits_flag &= FALSE;      
                        drops++;
                        printf(".");
                    }
                    datagram_done(pak_in);
                    datagram_done(pak_copy);
                } else {
                    /*
                     * escape sequence received 
                     */
                    datagram_done(pak_copy);
                    ix--;  
                    goto test_done;
                } 
            } else {
                /*
                 * timeout on receiving
                 */
                hits_flag &= FALSE;      
                drops++;
	        printf("T");
                datagram_done(pak_copy);
            } 
        }   /* end of B1, B2, D loop */
        if (hits_flag == TRUE) {
            hits++;
        } 
    }   /* end of test loop */ 
  test_done:    
    psioff(stdio);
    printf("\nSuccess rate of %s loopback test is %d percent (%u/%u)", 
        isdn_bri->loopback_mode == SYSTEM_LOOPBACK_ENABLED ? 
            "internal" : "external",
        hits > MAXULONG/100 ? hits / (ix/100) : (hits*100) / ix, hits, ix);
    /*
     * To mark test sucessful, we allows maximumally 2 drops which may be 
     * caused by
     * 1) aborting the test using escape sequence,
     * 2) real timeout on receiving a test packet, or
     * 3) unequivalent transmitted and received packets.
     */
    if (drops < 3) {
        drops = 0;
    }
    return(drops); 
}   /* end of ping_test_for_bri */


/*
 * =====================================================================
 * test_bri_interface_loopback -
 *
 * Description:
 * This function tests a bri interface by enabling loopback on the
 * TP3420 chip (system loopback), and/or by enabling the external loopback
 * (plug loopback), which requires a loopback plug.
 *
 * Parameters:
 * idb - hwidbtype ptr to the bri interface 
 * bri_skippred - flag that indicates whether one or both of the loopback 
 *                tests (chip or plug) is skipped.
 *
 * Returns:
 * drops - number of failures 
 *
 * Notes:
 *
 * =====================================================================
 */
static int test_bri_interface_loopback (hwidbtype *idb, ushort *bri_skipped)
{
    
    int drops = 0; 
    ulong bri_channel; 
    hwidbtype *dchan_idb,
              *b1chan_idb,
              *b2chan_idb;
    isdn_bri_t *isdn_bri;
    enum Loopback looptest;
    boolean test_aborted = FALSE;
    
    /*
     * Set all idb variables
     */
    isdn_bri = idb->isdn_info;
    dchan_idb = isdn_bri->d_chan_idb;     /* make sure it is d channel*/
    b1chan_idb = isdn_bri->b1_chan_idb;
    b2chan_idb = isdn_bri->b2_chan_idb;
    
    /*
     * check to see if remote loopback is enabled.
     * Note that remote loopback was implemented before and use 'loopback'
     * field in idb structure, while system loopback and chip loopback
     * that are implemented now use 'loopback_mode' in isdn_info structure.
     * For the purpose of integrity, putting all these fields in one structure
     * is recommended.
     */
    if (dchan_idb->loopback) { 
        printf("\nMust remove remote loopback before testing");
        return(drops);
    }
    
    if (isdn_bri->loopback_mode == LOOPBACK_DISABLED) {
        for (looptest = SYSTEM_LOOPBACK_ENABLED; looptest !=
	     LOOPBACK_DISABLED;) {
            /*
             * no loopback test has currently being conducted. So go ahead.
             */
            clearinput(stdio);
            printf("\n%s loopback test [y/n] ?",
		   (looptest == SYSTEM_LOOPBACK_ENABLED ? "internal" :
		    "external"));
            if (doit()) {
                if (looptest == PLUG_LOOPBACK_ENABLED) {
                    printf("\nUnplug %s cable and insert the loopback plug.",
			   idb->hw_namestring);
                    printf("\nThe procedure to build such a plug:");
                    printf("\n1. connect pin 3 and 4, and pin 5 and 6, respectively, of a RJ45 plug.");
                    printf("\n2. put a 50 Ohm terminating resistor between pin 4 and 5.");
                    printf("\nWarning: executing a plug loopback test without unplugging the existing %s cable and plugging a loopback plug is not a valid test and may cause SERIOUS problems.", idb->hw_namestring);
                    printf("\n**** Make sure you unplug %s cable and insert the loopback plug. ****\n", idb->hw_namestring); 
                    printf("\n external loopback test [y/n] ?");
                    if (!doit()) {
                        (*bri_skipped)++;
                        looptest = LOOPBACK_DISABLED;
                        break;   
                    }
                }
                /*
                 * - set loopback flag for interface under test.
                 * - enable loopback
                 */
                isdn_bri->loopback_mode = b1chan_idb->isdn_info->loopback_mode
		    = b2chan_idb->isdn_info->loopback_mode = looptest;
                tp3420_init(dchan_idb);
                tp3420_enable_line(dchan_idb);
                tp3420_enable_loopback(dchan_idb, looptest);

                /*
                 * do a full initialization on the associated BRI channel
		 */
		for (bri_channel = 1; bri_channel <= BRI_MAX_CHAN;
		     bri_channel++) {
		    switch (bri_channel) {
		      case 1:
			idb = isdn_bri->b1_chan_idb;
			break;
		      case 2:
			idb = isdn_bri->b2_chan_idb;
			break;
		      case 3:
			
			idb = isdn_bri->d_chan_idb;
			break;
		      default:
			printf("\nWrong BRI channel. Test aborted.");
			drops++;
			test_aborted = TRUE;
			break;
		    }
		    bri_chan_init(idb);
		}
		if (!test_aborted) {
		    test_aborted = FALSE;
		    bri_up_channels(dchan_idb);
		    drops += ping_test_for_bri(dchan_idb);
		}
		/*
		 * remove loopback setup when test is done.
		 */                           
                isdn_bri->loopback_mode = LOOPBACK_DISABLED;
                b1chan_idb->isdn_info->loopback_mode = LOOPBACK_DISABLED;
                b2chan_idb->isdn_info->loopback_mode = LOOPBACK_DISABLED;
                tp3420_enable_loopback(dchan_idb, LOOPBACK_DISABLED);
                /*
                 * deactivate the bri interface chip and disable
                 * its interrupts
                 */
                bri_disable(dchan_idb);
                bri_down_channels(dchan_idb);
                switch (looptest) {
		  case SYSTEM_LOOPBACK_ENABLED:
		    looptest = PLUG_LOOPBACK_ENABLED; 
		    break;
		  default:
		    looptest = LOOPBACK_DISABLED; 
		    break;
                }
            } else {
                (*bri_skipped)++; 
                switch (looptest) {
		  case SYSTEM_LOOPBACK_ENABLED:
		    looptest = PLUG_LOOPBACK_ENABLED;
		    break;
		  default:
		    looptest = LOOPBACK_DISABLED;
		    break;
                }
            }
        }   /* end of looptest loop */
        return(drops);
    } else if ((isdn_bri->loopback_mode == SYSTEM_LOOPBACK_ENABLED) ||
               (isdn_bri->loopback_mode == PLUG_LOOPBACK_ENABLED)) {
        printf("\n%s is being tested. Try later.", idb->hw_namestring);
        *bri_skipped = 2; /*skip both chip and plug loopback test */
        return(drops);
    } 
    printf("\nTesting BRI interface is not yet supported on this platform.");
    return(drops);
}


/*
 * =====================================================================
 * test_bri_interface -
 *
 * Description:
 * This function is invoked by the test_interface registry for BRI.
 * The BRI test interface function is different from other interfaces.
 * It does an internal loopback and an external loopback test.
 *
 * Parameters:
 * hwidbtype *,
 * int *failed cases
 * int *passed cases
 * int *skipped cases
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
void test_bri_interface (hwidbtype *idb, int *nfailed, int *npassed,
			 int *nskipped)
{
    int drops = 0, ierrors, oerrors;
    isdn_bri_t *isdn_bri;
    ushort test_bri_skipped = 0;
    char *reason = NULL;
    
    if (idb->state != IDBS_ADMINDOWN) {
	printf("SHUTDOWN required when testing BRI.\nWarning: make sure there is no existing traffic on %s.\nSkipping...", idb->hw_namestring);
	(*nskipped)++;
	return;                          
    } else {
	isdn_bri = idb->isdn_info;
	ierrors = idb->counters.input_error + 
	    isdn_bri->b1_chan_idb->counters.input_error +
		isdn_bri->b2_chan_idb->counters.input_error;
	oerrors = idb->counters.output_error + 
	    isdn_bri->b1_chan_idb->counters.output_error +
		isdn_bri->b2_chan_idb->counters.output_error;
	
	/*
	 * Perform loopback tests and get the drops count
	 */
	drops = test_bri_interface_loopback(idb, &test_bri_skipped);
	
	/*
	 * check to see if loopback test is skipped and adjust
	 * the ierrors value accordingly. This is because 
	 * after setting up the loopback (chip or plug), one input 
	 * error for each channel (B1, B2, and D) on the interface 
	 * under is always detected by Munich 32 chip, which may be 
	 * the limitation of the chip. We need to adjust
	 * the value of ierrors so that when it is used later
	 * to compare with the input errors after the test (
	 * aftertest_ierrors), the true errors detected during the
	 * test can be reflected.
	 */
	if (test_bri_skipped == BRI_NO_LOOP_SKIPPED) {
	    /* neither chip nor plug loopback test skipped */
	    ierrors += BRI_MAX_CHAN * 2; /* chip and loop; B1, B2, and D */ 
	} else if (test_bri_skipped == BRI_ONE_LOOP_SKIPPED) {
	    /* either chip or plug loopback test skipped */
	    ierrors += BRI_MAX_CHAN; /* chip or loop; B1, B2, and D */ 
	} else if (test_bri_skipped == BRI_BOTH_LOOP_SKIPPED) { 
	    /* both chip and plug loopback test skipped */
	    (*nskipped)++;
	    return;  
	} else {
	    /*
	     * Should never comes here. Should it do,
	     * abort the test.
	     */
	    printf("\nInvalid value of `test_bri_skipped'. Test aborted.\n");
	    return; 
	}
	
	if (drops == 0) {
	    int aftertest_ierrors, aftertest_oerrors;
	    
	    isdn_bri = idb->isdn_info;
            aftertest_ierrors = idb->counters.input_error +
		isdn_bri->b1_chan_idb->counters.input_error +
		    isdn_bri->b2_chan_idb->counters.input_error;
            aftertest_oerrors = idb->counters.output_error +
		isdn_bri->b1_chan_idb->counters.output_error +
		    isdn_bri->b2_chan_idb->counters.output_error;
	    /*
             * tolerate one input error on each channel (B1, B2, and D) which 
             * occurs when setting up loopback.
             */
            if (aftertest_ierrors > ierrors + 1) {
                reason = "reception";
            } else if (aftertest_oerrors > oerrors + 1) {
                reason = "transmission";
            } else {
                reason = NULL;
            }
	} else {
	    reason = "timeout OR `transmitted vs received packet mismatch'";
	}  /* drops not equal to 0 */
	
	if (reason) {
	    printf("\nTest result for %s: Failed - %s problem.", 
		   idb->hw_namestring, reason);
	    (*nfailed)++;
	    if (reason == "reception") {
		printf(" Input errors on %s.", idb->hw_namestring);
	    } else if (reason == "transmission") {
		printf(" Output errors on %s.", idb->hw_namestring);
	    }
	} else {
	    printf("\nTest result for %s: Passed", idb->hw_namestring);
	    (*npassed)++;
	}
    }
}

/*
 * Return TRUE is the BRI interface is in state ACTIVATED. FALSE is in any
 * other state. Added for Leased Line ISDN feature.
 */
boolean is_layer1_current_state_activated (hwidbtype *hwidb)
{
	isdn_bri_t *isdn_info;

	isdn_info = hwidb->isdn_info;
	if(isdn_info->chantype != ISDN_Dchan)
		isdn_info = isdn_info->d_chan_idb->isdn_info;	 
	return((isdn_info->current_bri_l1_state >= l1_state_F7) ? TRUE : FALSE);
}
