/* $Id: if_c1000_bri.c,v 3.6.12.9 1996/09/07 02:08:01 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-36-c1000/if_c1000_bri.c,v $
 *------------------------------------------------------------------
 * if_c1000_bri.c -- The BRI driver code for the cisco 1xxx.
 *
 * November 1994, Greg Stovall
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c1000_bri.c,v $
 * Revision 3.6.12.9  1996/09/07  02:08:01  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 3.6.12.8  1996/08/28  13:10:42  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.12.7  1996/08/13  02:22:39  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.6.12.6  1996/07/20  03:20:18  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.6.12.5  1996/06/17  17:33:54  wmay
 * CSCdi60601:  show controller bri crashes box - only allow for d-chans
 * Branch: California_branch
 *
 * Revision 3.6.12.4  1996/06/12  19:57:01  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.6.12.3  1996/06/12  16:51:30  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - put b channel idbs in the idb list.  This removes a
 * lot of special initialization code for BRI.
 * Branch: California_branch
 *
 * Revision 3.6.12.2  1996/04/17  13:47:31  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.6.12.1  1996/03/18  21:57:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/16  07:32:10  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.1  1996/03/13  02:00:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6  1996/02/15  18:03:00  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.5  1996/01/19  22:15:51  wmay
 * CSCdi45344:  ACL: input access lists are ignored on SMDS subinterfaces
 * Add extra include file so serial_inline is happy
 *
 * Revision 3.4  1996/01/05  01:04:41  sluong
 * CSCdi42391:  Configuring a protocol to be routed brings down B-channel
 *
 * Revision 3.3  1995/11/17  18:40:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:38  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:26:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/22  04:36:43  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.4  1995/08/10  20:28:00  agh
 * CSCdi38551:  Adding BRI fastswitching to sapphire
 *
 * Revision 2.3  1995/06/21  03:14:58  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/20  07:18:15  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:52:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "subsys.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../wan/serial.h"
#include "../wan/ppp.h"
#include "../if/ether.h"
#include "../wan/smds.h"
#include "../wan/atm_dxi.h"
#include "logger.h"
#include "../wan/isdn_debug.h"
#include "../if/bri_debug.h"
#include "../les/if_les.h"
#include "../les/isdn_bri.h"
#include "../les/if_les_bri_c1000.h"
#include "../src-36/quicc_driver_lib.h"
#include "../src-36/if_bri_quicc.h"
#include "if_c1000_bri.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "media_registry.h"
#include "../if/serial_inline.h"
#include "../les/if_les_bri_inline.h"
#include "../wan/serial_parser.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../if/traffic_shape.h"

/* local storage */
static boolean isdn_process_up = FALSE;
static void bri_system_configured(hwidbtype *); /* set ISDN cstate */


/*
 * void bri_show_int_enhanced
 *
 * display any enhancements to the interface. In this case we indicate
 * that we have an integrated nt1
 */

void bri_show_int_enhanced(void)
{
    printf("Integrated NT1 for ISDN Basic Rate interface\n");
}

/*
 * =================================================================
 * Name: quicc_bri_check_tx_errors
 *
 * Description:
 *
 * Parameters:
 *	hwidbtype * - idb for channel that transmitted.
 *	ushort - Buffer descriptor status.
 *
 * Returns:
 * =================================================================
 */
static void quicc_bri_check_tx_errors (hwidbtype *idb, ushort status)
{
    bri_instance_t *ds;

    ds = INSTANCE;

    if (status & QUICC_BDSTAT_TX_CTSLOS)
	ds->fatal_tx_err = TRUE;
	
    if (status & QUICC_BDSTAT_TX_UNRRUN) {
	/*
	 * Underrun error. FATAL. TXE set in SCCE.
	 */
	idb->counters.output_underrun++;
	ds->fatal_tx_err = TRUE;
    }
}


/*
 * =====================================================================
 * tp3420_init -
 *
 * Description:
 * This function initialises  a specific tp3420 chip, ie per
 * bri interface. Invoked for each dchannel idb by bri_init ().
 * Note that we do not enable interrupts.
 *
 * Parameters:
 * dchannel_idb: 	hwidbtype ptr to the bri interface.
 *
 * Returns:
 * Nothing
 * =====================================================================
 */
void tp3420_init (hwidbtype *dchan_idb) 
{
    /*
     * write commands to the SID to set it in the approp. mode
     */
    write_sid(dchan_idb, TP3420_CRF_TEM); /* we are a TE master */
    write_sid(dchan_idb, TP3420_CRF_DIF4);/* use digital subunit 4 */
    write_sid(dchan_idb, TP3420_CRF_EID); /* disable end of message interrupt*/
    write_sid(dchan_idb, (TP3420_CRF_PINDEF | TP3420_P18_DENR));
                                                            /* setup pin 18 */ 
    write_sid(dchan_idb, TP3420_CRF_DCKE);  /* Enable the D chan clock */
    write_sid(dchan_idb, TP3420_CRF_BCLK4); /* Set BCLK to 2.56MHz */
    write_sid(dchan_idb, TP3420_CRF_DACCE); /* Enable Dchan access */
    
    /*
     * Check for the NT loopback mode, ie loopback type2. Note that the
     * driver does not directly support the other loopback types. They
     * are supported by the test_subunit module by test commands.
     */
    if (dchan_idb->loopback) {
        write_sid(dchan_idb, TP3420_CRF_B1E);
        write_sid(dchan_idb, TP3420_CRF_B2E);
	write_sid(dchan_idb, TP3420_CRF_LBL1);
	write_sid(dchan_idb, TP3420_CRF_LBL2);
	write_sid(dchan_idb, TP3420_CRF_PUP);
    } else {
	write_sid(dchan_idb, TP3420_CRF_CAL); /* clear all loopbacks*/
        write_sid(dchan_idb, TP3420_CRF_B1D);
        write_sid(dchan_idb, TP3420_CRF_B2D);
    }

	/*
	 * Leased Line initialisation registry
	 */
	reg_add_isdn_leased_line(init_bri_leased_line, "init_bri_leased_line");
}


/*
 * =====================================================================
 * write_sid()
 *
 * Description:
 * Write info to the TP3420 SID
 *
 * Parameters:
 * dchan_idb:      The dchannel idb
 * command:        The command to send
 *
 * Returns:
 *
 * =====================================================================
 */
void write_sid (hwidbtype *dchan_idb, uchar command)
{
    bri_instance_t *ds;
    uchar sid_status;

    ds = DCHAN_INSTANCE;

    if (bri_debug) {
	buginf("BRI: write_sid: command = 0x%x\n", command);
    }

    sid_status = quicc_access_sid(ds->dprbase, command);
 
    if (sid_status) {
	ds->last_sid = sid_status;
    }
}


/*
 * =====================================================================
 * read_sid()
 *
 * Description:
 * Read info from the TP3420 SID
 *
 * Parameters:
 * dchan_idb:      The dchannel idb
 *
 * Returns:
 * Contents of SID
 * =====================================================================
 */
uchar read_sid (hwidbtype *dchan_idb)
{
    uchar sid_status;
    bri_instance_t *ds;

    ds = DCHAN_INSTANCE;

    write_sid(dchan_idb, TP3420_CRF_NOP);
    sid_status = ds->last_sid;
    ds->last_sid = 0;
    return(sid_status);
}


/*
 * =====================================================================
 * tp3420_deactivate_line -
 *
 * Description:
 * This function is called during a bri_reset or a bri_shutdown to
 * make sure that the subunit TP3420 is in a known state.
 *
 * Parameters:
 * dchannel_idb: 	hwidbtype ptr to the bri subunit.
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
static void tp3420_deactivate_line (hwidbtype *dchan_idb)
{
    leveltype SR;	/* interrupt level */
    bri_instance_t *ds;

    SR = raise_interrupt_level(NETS_DISABLE);

    /*
     * Power down the SID.
     */
    write_sid(dchan_idb, TP3420_CRF_DR);
    write_sid(dchan_idb, TP3420_CRF_PDN);

    /*
     * Put out the lights
     */
    ds = DCHAN_INSTANCE;
    C1000_ISDN_B1_LED_OFF(ds->dprbase);
    C1000_ISDN_B2_LED_OFF(ds->dprbase);

    /*
     * Inform ISDN about this only if we were in a state
     * other than F1 or F2. (ie, only if we are either
     * activated or in an activation sequence). Also
     * remove possible outstanding timers.
     */
    if ((isdn_process_up) && (dchan_idb->isdn_info->current_bri_l1_state
			      > l1_state_F2)) {
	/*
	 * Turn off T3.
	 */
	bri_stop_t3(dchan_idb);

	/*
	 * Stop the deactivation timer
	 */
	bri_stop_t4(dchan_idb);

	/*
	 * Send deact primitives
	 */
  	bri_enq_prim(dchan_idb->unit, ISDN_PH_DEACT_IND);
	bri_enq_prim(dchan_idb->unit, ISDN_MPH_DEACT_IND);
    }

    /*
     * Set the appropriate current state variable.
     */
    dchan_idb->isdn_info->current_bri_l1_state = l1_state_F2;
    
    reset_interrupt_level(SR);
}


/*
 * =====================================================================
 * bri_reset()
 *
 * Description:
 * Deactivate the tp3420 chip for an interface.
 *
 * Parameters:
 * dchan_idb - the idb for the bri D channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_reset (hwidbtype *idb) 
{
    hwidbtype *dchan_idb;

    dchan_idb = idb->isdn_info->d_chan_idb;	/* make sure it is dchannel*/

    /*
     * put the chip in a known state and inform the process
     * level code that things are closing down
     */
    tp3420_deactivate_line(dchan_idb); 
}


/*
 * =====================================================================
 * bri_init -
 * 
 * Description:
 * Initailize a BRI interface. this includes the QUICC SCC
 * channels and the tp3420 chip.
 *
 * Parameters:
 * hwidbtype *dchan_idb
 * maxdgram_change :	boolean to indicate if max dgram size changed.
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
void bri_init (hwidbtype *in_idb, boolean maxdgram_change)
{
    hwidbtype *dchan_idb;
    hwidbtype *idb = NULL;
    leveltype SR;
    uint bri_channel;
    bri_instance_t *ds;
    pooltype *old_pool;
    isdn_bri_t *isdn_info;
    
    dchan_idb = in_idb->isdn_info->d_chan_idb;	/* make sure it is dchannel*/
    isdn_info = dchan_idb->isdn_info;

    /*
     * down the channels at the hdlc controller level
     */
    bri_down_channels(dchan_idb);
    
    /*
     * If we are administratively down, dont re-enable.
     */
    if (!ISBRILEASED(dchan_idb) && (dchan_idb->state == IDBS_ADMINDOWN))
	return;
    
    SR = raise_interrupt_level(NETS_DISABLE);	/* disable interrupts */
    dchan_idb->counters.resets++;		/* bump count */
    
    /*
     * Put the SID in a known down state.
     */
    bri_reset(dchan_idb);

    for (bri_channel = 0; bri_channel < BRI_MAX_CHAN; bri_channel++) {
	/* Set up IDB channel pointers */
	switch (bri_channel) {
	  case BRI_D_CHAN:
	    idb = dchan_idb;
	    break;
	    
	  case BRI_B1_CHAN:
	    idb = dchan_idb->isdn_info->b1_chan_idb;
	    break;
	    
	  case BRI_B2_CHAN:
	    idb = dchan_idb->isdn_info->b2_chan_idb;
	    break;
	}

	/*
	 * Setup start_offset
	 */
	switch (idb->enctype) {
	  case ET_HDLC:
	  case ET_FRAME_RELAY:
	  case ET_PPP:
	  case ET_SMDS:
	  case ET_SDLCP:
	  case ET_SDLCS:
	  case ET_STUN:
	  case ET_LAPB:
	  case ET_X25:
	    idb->start_offset = ENCAPBYTES - idb->encsize;
	    break;
	  default:
	    errmsg(&msgsym(BOGUSENCAP, LINK), idb->hw_namestring,
		   idb->enctype);
	    idb->start_offset = ENCAPBYTES - HDLC_ENCAPBYTES;
	}	
	
	/*
	 * If a private buffer pool for this interface exists AND there
	 * has been an MTU change, discard the pool with the old-sized
	 * buffers. 
	 */
	
	if (idb->pool && maxdgram_change) {
	    old_pool = idb->pool;
	    pool_destroy(old_pool);
	    idb->pool = NULL;
	}
	
	ds = INSTANCE;

	/* 
	 * If the private buffer pool has not been allocated yet, or if
	 * it was destroyed due to an MTU change above, allocate it now.
	 * Populate the pool, initialize a private buffer cache,
	 * and populate the buffer cache with buffers from the newly
	 * created private pool.  Also set up the fallback public buffer
	 * pool. 
	 */
	
	if (!idb->pool) {
	    if (idb->pool_group == POOL_GROUP_INVALID)
		idb->pool_group = pool_create_group();
	    
	    idb->pool = pak_pool_create(idb->hw_namestring,
					    idb->pool_group,
					    idb->max_buffer_size, POOL_SANITY,
					    NULL); 
	    if (!idb->pool) {
		crashdump(0);
	    }
	    
	    idb->pool->fallback = pak_pool_find_by_size(idb->max_buffer_size);
	    pool_adjust(idb->pool, 0, PRIV_NUM_BUF(ds->max_rx_ring_size),
			PRIV_NUM_BUF(ds->max_rx_ring_size), TRUE); 
	    
	    pak_pool_create_cache(idb->pool,
				      PRIV_NUM_CACHE(ds->max_rx_ring_size));
	    pool_adjust_cache(idb->pool,
			      PRIV_NUM_CACHE(ds->max_rx_ring_size)); 
	}
	
	/*
	 * Initalize buffer descriptor rings.
	 */
	quicc_scc_bd_ring_init(idb, maxdgram_change);

	/*
	 * Init and reset the controller channels and the holdQ.
	 * Note that this should clear all the ring variables and
	 * such.
	 */
	holdq_reset(idb);
    }

    /*
     * Initialize all the channels
     */
    quicc_init_bri(dchan_idb);

    /*
     * Initialise the SID.
     */
    tp3420_init(dchan_idb);

    /* Make sure we are using the D Channel instance structure
     * since qinfo will need to point to specific port to enable.
     */
    ds = DCHAN_INSTANCE;
    quicc_scc_start(ds->dprbase, ds->qinfo, RX_ENABLED, TX_ENABLED);

    if (! dchan_idb->loopback) {
        if ((dchan_idb->state != IDBS_ADMINDOWN) &&
            (!ISBRILEASED(dchan_idb) ||
            (is_layer1_current_state_activated(dchan_idb))))
	        net_cstate(dchan_idb, IDBS_UP);
    }

    reset_interrupt_level(SR);

    /*
     * Initialize the loopback queue used in bri loopback test
     * Initialize the ICMP echo queue used in bri loopback test
     */
    test_bri_init(dchan_idb);

	/*
	 * If this interface has been configure for leased line ISDN then enable
	 * the interface and both B channels.
	 */
	if(ISBRILEASED(dchan_idb)){
          if(dchan_idb->isdn_info->b1_chan_idb->state==IDBS_UP)
		(*isdn_info->enable_bri_channel)(dchan_idb->isdn_info->b1_chan_idb);
          if(dchan_idb->isdn_info->b2_chan_idb->state==IDBS_UP)
		(*isdn_info->enable_bri_channel)(dchan_idb->isdn_info->b2_chan_idb);
	}
}


/*
 * =====================================================================
 * bri_shutdown()
 *
 * Description:
 * Shut a BRI interface down by downing the SCC
 * channels and disabling the tp3420.
 *
 * Parameters:
 * dchan_idb - the idb for the bri D channel
 * newstate - the state this interface is being put into
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_shutdown (hwidbtype *idb, int newstate)
{
    leveltype SR;
    hwidbtype *dchan_idb;

    dchan_idb = idb->isdn_info->d_chan_idb;	/* make sure it is dchannel*/

    SR = raise_interrupt_level(NETS_DISABLE);

    dchan_idb->state = newstate;
    GET_TIMESTAMP(dchan_idb->state_time);

    /*
     * deactivate the bri interface chip
     */
    bri_reset(dchan_idb);

    /*
     * down the channels at the hdlc controller level
     */
    bri_down_channels(dchan_idb);

    reset_interrupt_level(SR);
}


/*
 * =====================================================================
 * bri_disable()
 *
 * Description:
 * Disable the tp3420 chip for an interface.
 *
 * Parameters:
 * dchan_idb - the idb for the bri D channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void bri_disable (hwidbtype *idb) 
{
    hwidbtype *dchan_idb;

    dchan_idb = idb->isdn_info->d_chan_idb;	/* make sure it is dchannel*/

    /*
     * put the chip in a known state and inform the process
     * level code that things are closing down
     */
    tp3420_deactivate_line(dchan_idb); 
}


/*
 * =====================================================================
 * bri_down_channels()
 *
 * Description:
 * Shut down the channels
 *
 * Parameters:
 * idb - the idb for the bri D channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void bri_down_channels (hwidbtype *idb)
{
    bri_instance_t *ds;

    ds = INSTANCE;

    
    /* Stop D channel */
    quicc_scc_stop(ds->dprbase, ds->qinfo, RX_DISABLED, TX_DISABLED);
    
    quicc_scc_bd_ring_init(idb, FALSE);

    /* This is only done to use the INSTANCE macro to access each
     * channel's instance structure.
     */
    
    /* Stop B1 channel */
    ds  = B1CHAN_INSTANCE;
    quicc_scc_stop(ds->dprbase, ds->qinfo, RX_DISABLED, TX_DISABLED);

    /* Stop B2 channel */
    ds  = B2CHAN_INSTANCE;
    quicc_scc_stop(ds->dprbase, ds->qinfo, RX_DISABLED, TX_DISABLED);
}


/*
 * =====================================================================
 * bri_up_channels()
 *
 * Description:
 * Bring up the channels
 *
 * Parameters:
 * idb - the idb for the bri D channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void bri_up_channels (hwidbtype *idb)
{
    bri_instance_t *ds;
    boolean enable_rx = idb_is_throttled(idb) ? FALSE : TRUE;

    ds = INSTANCE;

    /* Start D Channel */
    quicc_scc_start(ds->dprbase, ds->qinfo, enable_rx, TRUE);

    
    /* Start B1 channel */
    ds  = B1CHAN_INSTANCE;
    quicc_scc_start(ds->dprbase, ds->qinfo, enable_rx, TRUE);

    /* Start B2 channel */
    ds  = B2CHAN_INSTANCE;
    quicc_scc_start(ds->dprbase, ds->qinfo, enable_rx, TRUE);
}


/*
 * =====================================================================
 * c1000_set_bri_bandwidth()
 *
 * Description:
 * Set the bandwidth of a BRI B channel by setting a field
 * in the isdn_info struct and re-initializing the channel.
 *
 * Parameters:
 * idb - the idb for the bri channel 
 * bandwidth - new bandwidth of the channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void c1000_set_bri_bandwidth (hwidbtype *idb, uint requested_bw)
{
    enum Chan channel_type;
    hwidbtype *dchan;
    bri_instance_t *ds;
    ulong b1_bw;
    ulong b2_bw;

    channel_type = idb->isdn_info->chantype;
    ds = INSTANCE;
    
    /* 
     * only the B channels can have their bandwidths changed
     */
    if (channel_type != ISDN_Dchan) {
	/*
	 * Set up the new values for this idb.
	 */
        if (requested_bw == VISIBLE_BRI_BANDWIDTH_56) {
	    set_default_interface_bandwidth(idb->firstsw,
					    VISIBLE_BRI_BANDWIDTH_56);
        } else if (requested_bw == VISIBLE_BRI_BANDWIDTH_64) {
	    set_default_interface_bandwidth(idb->firstsw,
					    VISIBLE_BRI_BANDWIDTH_64);
        }

	/*
	 * Now that both have been changed let the quicc build the
	 * map on the fly.
	 */
	dchan = idb->isdn_info->d_chan_idb;
	b1_bw = dchan->isdn_info->b1_chan_idb->firstsw->visible_bandwidth;
	b2_bw = dchan->isdn_info->b2_chan_idb->firstsw->visible_bandwidth;

	quicc_bri_set_bandwidth(ds->dprbase, b1_bw, b2_bw);

	if (isdn_debug)
	    buginf("\nBRI %d %s: Set bandwidth to %dKb\n", idb->unit,
		   (channel_type == B1) ? "B1" : "B2", requested_bw);
    }
}


/*
 * =====================================================================
 * bri_tx_start -
 * 
 * Description:
 * Starts tx on the BRI
 *
 * Parameters:
 * idb:		The channel idb
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
static void bri_tx_start (hwidbtype *idb)
{
    bri_instance_t *ds = INSTANCE;  /* instance */
    hwidbtype *dchan_idb;
    uint bytes;                     /* counters */
    paktype *pak;                   /* packet */
    leveltype SR;                   /* Interrupt level */
    

    /*
     * Only send frames in state F7
     */
    dchan_idb = idb->isdn_info->d_chan_idb;
    if (dchan_idb->isdn_info->current_bri_l1_state == l1_state_F7) {

	SR = raise_interrupt_level(NETS_DISABLE);
	
	/*
	 * While there is another packet to output and room for it in the
	 * TX queue, enqueue the packet:
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
	     * send the packet on it's way
	     */
	    quicc_scc_transmit((quicc_instance_t *)ds, pak,
			       QUICC_BRI_TX_FLAGS);
	    
	    /*
	     * Do the accounting
	     */
	    idb->counters.tx_cumbytes += bytes;
	    TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */
	    idb->counters.outputs++;
	    
	    /*
	     * Count broadcasts
	     */
	    if ((pak->enctype == ET_HDLC) &&
		((*(ushort *)pak->datagramstart) & HDLC_BCAST))
		idb->counters.output_broad++;
	}
	
	/*
	 * If D chan, write SID to start output.
	 */
	if ((idb->isdn_info->chan_num == BRI_D_CHAN) && (ds->tx_count)) {
	    write_sid(idb, TP3420_CRF_DREQ1);
	}
	
	reset_interrupt_level(SR);    /* Restore interrupt level */
    }
}


/*
 * =====================================================================
 * bri_TX_interrupt()
 *
 * Description:
 * Handle a just transmitted packet notification.
 *
 * Parameters:
 * idb - the idb of the bri channel 
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_TX_interrupt (hwidbtype *idb, paktype *pak)
{
    GET_TIMESTAMP(idb->lastoutput);
    bri_tx_start(idb);
}


/*
 * =====================================================================
 * bri_restart()
 *
 * Description:
 * Restart a BRI interface by re-initializing the interface
 * and enabling the tp3420.
 *
 * Parameters:
 * idb - the idb for the bri channel 
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void bri_restart (hwidbtype *idb)
{
    hwidbtype *dchan_idb;

    dchan_idb = idb->isdn_info->d_chan_idb;	/* make sure it is dchannel*/

    /*
     * Make sure the L2 knows we are down.
     */
    bri_disable(dchan_idb);

    /*
     * re-initialize the interface
     */
    bri_init(dchan_idb, TRUE);

    /*
     * enable the bri interface chip only if we are up. 
     */
    if (ISBRILEASED(dchan_idb) || dchan_idb->state != IDBS_ADMINDOWN) {
        tp3420_enable_line(dchan_idb);

		if(ISBRILEASED(dchan_idb))
			(*dchan_idb->isdn_info->activate_line)(dchan_idb);
    }
}


/*
 * =====================================================================
 * bri_enable()
 *
 * Description:
 * routine to clear throttling on a bri channel.
 *
 * Parameters:
 * idb
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_enable (hwidbtype *idb)
{
    if (idb->type == IDBTYPE_BRI)
	idb_mark_unthrottled(idb);
}


/*
 * =====================================================================
 * tp3420_interrupt -
 *
 * Description:
 * This handler is called to deal with interrupts from the tp3420
 *
 * Paramters:
 * dchan_idb:	The dchan idb
 *
 * Returns:
 * nothing.
 *
 * Notes: The ISDN states F1 to F8 are the I.430 finite
 * states recommended by ANSI/NET3 specification for the layer 1. 
 * =====================================================================
 */
void tp3420_interrupt (hwidbtype *dchan_idb)
{
    bri_instance_t *ds;
    uchar sid_status = 0;	/* TP3420 status */
    quicc_dpr_t *dpr = (quicc_dpr_t *) getmbar();

    ds = DCHAN_INSTANCE;

    /* 
     * clear out intr 
     */
    quicc_3420_intr_clear(dpr);
    sid_status = bri_tint_state_handler(dchan_idb);

    /*
     * If we are activated, fire up the controller for the dchan
     */
    if ((sid_status == TP3420_SRF_AI) &&
	(dchan_idb->state != IDBS_ADMINDOWN)) {
	bri_tx_start(dchan_idb);
    } 
}


/*
 * =====================================================================
 * bri_t3_handler -
 * 
 * Description:
 * Handle a T3 timer interrupt
 *
 * Parameters:
 * idb:		The D-channel idb
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
void bri_t3_handler (hwidbtype *idb)
{
    bri_instance_t *ds;

    ds = INSTANCE;

    quicc_t3_timer_clear(ds->dprbase);

    /*
     * Stop trying to activate and power down, but only if not in State F6.
     * Determine the state of Layer 1 with the ENST and DISST commands.
     */
    write_sid(idb, TP3420_CRF_ENST); 
    write_sid(idb, TP3420_CRF_DISST);
    if(ds->last_sid == TP3420_F6_STATE) {
	bri_enq_prim(idb->unit, ISDN_PH_DEACT_IND);

	/*
	 * Clear out the pending packets
	 */
	holdq_reset(idb);
	if(bri_debug)
	    buginf("\nBRI: Timer T3 expired in State F6, not deactivating ");
    } else {
	bri_enq_prim(idb->unit, ISDN_PH_DEACT_IND);
	idb->isdn_info->current_bri_l1_state = l1_state_F2;
	write_sid(idb, TP3420_CRF_DR);
	write_sid(idb, TP3420_CRF_PDN);

	bri_stop_t4(idb);

	/*
	 * re-initialize the interface
	 */
	bri_init(idb, FALSE);

	if (bri_debug) 
	    buginf("\nBRI: Timer T3 expired. Activation halted ");
    }
}


/*
 * =====================================================================
 * bri_t4_handler -
 * 
 * Description:
 * Handle a T4 timer interrupt
 *
 * Parameters:
 * idb:		The d-channel idb
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
void bri_t4_handler (hwidbtype *idb)
{
    bri_instance_t *ds;

    ds = INSTANCE;

    quicc_t4_timer_clear(ds->dprbase);

    /*
     * Check the FState of the 3420 to see if must deactivate the line.
     * Do not deactivate if in State F7.
     */
    write_sid(idb, TP3420_CRF_ENST);
    write_sid(idb, TP3420_CRF_DISST);
    if(ds->last_sid != TP3420_F7_STATE) {
	if(bri_debug) 
	    buginf("\nBRI: Deactivation timer expired. Activation halted ");
	bri_enq_prim(idb->unit, ISDN_PH_DEACT_IND);
	if (idb->isdn_info->current_bri_l1_state == l1_state_F8)
	    bri_enq_prim(idb->unit, ISDN_MPH_EI2_IND);
	write_sid(idb, TP3420_CRF_DR);
	write_sid(idb, TP3420_CRF_PDN);

	/*
	 * re-initialize the interface
	 */
	bri_init(idb, FALSE);
    } else {
	idb->isdn_info->current_bri_l1_state = l1_state_F7;
	if(bri_debug)
	    buginf("\nBRI: Deactivation timer expired, staying in State F7 ");
    }
}


/*
 * =====================================================================
 * bri_2ms_handler -
 * 
 * Description:
 * Handle the TP3420 2ms timer interrupt.
 *
 * Parameters:
 * idb:		The d-channel idb
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
void bri_2ms_handler (hwidbtype *idb)
{
    bri_instance_t *ds;

    ds = INSTANCE;

    quicc_2ms_timer_clear(ds->dprbase);

    if (idb->isdn_info->current_bri_l1_state == l1_state_F3) {
	write_sid(idb, TP3420_CRF_AR);       /* Activation Request */
	idb->isdn_info->current_bri_l1_state = l1_state_F4;
	
	bri_start_t3(idb);
	
	if (bri_debug)
	    buginf("BRI: Starting T3 timer after expiry of Power Up timeout for unit = %d, current state is F4. \n",
		   idb->unit);
    }
}
 

/*
 * =====================================================================
 * bri_start_t3()
 *
 * Description:
 * Start the T3 timer
 *
 * Parameters:
 * dchan_idb:	The dchan idb
 *
 * Returns:
 * Nothing
 * =====================================================================
 */
void bri_start_t3 (hwidbtype *dchan_idb)
{
    quicc_init_timer(QUICC_TMR_3, QUICC_TMR_MAX_PRESCALE, TRUE,
		     QUICC_TMR_ICLK_INTCLK_DIV16, QUICC_TIME_8S);
    quicc_start_timer(QUICC_TMR_3);
}


/*
 * =====================================================================
 * bri_stop_t3()
 *
 * Description:
 * Stop the T3 timer
 *
 * Parameters:
 * dchan_idb:	The dchan idb
 *
 * Returns:
 * Nothing
 * =====================================================================
 */
void bri_stop_t3 (hwidbtype *dchan_idb)
{
    quicc_stop_timer(QUICC_TMR_3);
}


/*
 * =====================================================================
 * bri_start_t4()
 *
 * Description:
 * Start the T4 (deactivation) timer.
 *
 * Parameters:
 * dchan_idb:	The dchan idb
 *
 * Returns:
 * Nothing
 * =====================================================================
 */
void bri_start_t4 (hwidbtype *dchan_idb)
{
    quicc_init_timer(QUICC_TMR_4, QUICC_TMR_MAX_PRESCALE, TRUE,
		     QUICC_TMR_ICLK_INTCLK_DIV16, QUICC_TIME_600MS);
    quicc_start_timer(QUICC_TMR_4);
}


/*
 * =====================================================================
 * bri_stop_t4()
 *
 * Description:
 * Stop the T4 (deactivation) timer.
 *
 * Parameters:
 * dchan_idb:	The dchan idb
 *
 * Returns:
 * Nothing
 * =====================================================================
 */
void bri_stop_t4 (hwidbtype *dchan_idb)
{
    quicc_stop_timer(QUICC_TMR_4);
}


/*
 * =====================================================================
 * bri_start_pup_timer()
 *
 * Description:
 * Start the TP3420 PUP timer.
 *
 * Parameters:
 * None.
 *
 * Returns:
 * Nothing
 * =====================================================================
 */
static void bri_start_pup_timer (void)
{
    quicc_init_timer(QUICC_TMR_2, QUICC_TMR_MAX_PRESCALE, TRUE,
		     QUICC_TMR_ICLK_INTCLK_DIV16, QUICC_TIME_2MS);
    quicc_start_timer(QUICC_TMR_2);
}


/*
 * =====================================================================
 * bri_dchan_coll()
 *
 * Description:
 * Handle a dchan collision
 *
 * Parameters:
 * dchan_idb:	The dchan idb
 *
 * Returns:
 * Nothing
 * =====================================================================
 */
void bri_dchan_coll (hwidbtype *idb)
{
    /* ### clean up ring ### */
}


/*
 * =====================================================================
 * bri_show_errors()
 *
 * Description:
 * Print BRI errors.
 *
 * Parameters:
 * idb - the idb for the bri channel 
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_show_errors (hwidbtype *idb)
{
    printf("  %d missed datagrams, %d overruns\n",
      idb->counters.input_resource, idb->counters.input_overrun);
    printf("  %d bad datagram encapsulations, %d memory errors\n",
      idb->unknown_encaps, idb->dma_memory_error);
    printf("  %d transmitter underruns",idb->counters.output_underrun);
    printf("\n\n");
}


/*
 * =====================================================================
 * bri_show_controller()
 *
 * Description:
 * Print controller stats for a BRI interface.
 *
 * Parameters:
 * idb - idb of the bri channel 
 * parseinfo *
 *
 * Returns:
 * nothing
 * =====================================================================
 */

static const char chan_info[] = "Channel Information:\n";

static boolean bri_show_controller (hwidbtype *in_idb, parseinfo *csb)
{
    hwidbtype *dchan_idb;
    hwidbtype *idb = NULL;
    ulong bri_channel;
    bri_instance_t *ds;

    dchan_idb = in_idb->isdn_info->d_chan_idb;	/* make sure it is dchannel*/
    if (dchan_idb != in_idb)
	return(FALSE);
    
    ds = DCHAN_INSTANCE;
        

    printf("BRI unit %d%s:", dchan_idb->unit,
	   (ds->nt_one ? " with integrated NT1" : ""));

    /*
     * Print the layer 1 status.
     */
    bri_display_layer1(dchan_idb);

    /* 
     * loop thru all 3 BRI channels and list the controller
     * information for each one.
     */
    for (bri_channel=0; bri_channel<BRI_MAX_CHAN; bri_channel++) {
        /* 
         * get the idb of the BRI channel.
         */
        switch (bri_channel) {
        case BRI_D_CHAN:
            idb = dchan_idb->isdn_info->d_chan_idb;
            printf("D %s", chan_info);
            break;
        case BRI_B1_CHAN:
            idb = dchan_idb->isdn_info->b1_chan_idb;
            printf("B1 %s", chan_info);
            break;
        case BRI_B2_CHAN:
            idb = dchan_idb->isdn_info->b2_chan_idb;
            printf("B2 %s", chan_info);
            break;
        }

	quicc_bri_show_controller(idb, bri_channel);

        bri_show_errors(idb);
    } /* end of for loop */

    printc('\n');
    /*
     * always return false because this is what the 
     * calling function is expecting.
     */
    return(FALSE);
}


/*
 * =====================================================================
 * tp3420_enable_line -
 *
 * Description:
 * This function is called when switch type is first configured
 * and whenever the BRI comes out of shutdown.
 *
 * Parameters:
 * dchannel_idb: 	hwidbtype ptr to the bri.
 *
 * Returns:
 * nothing
 *
 * =====================================================================
 */
void tp3420_enable_line (hwidbtype *dchan_idb)
{

}


/*
 * =====================================================================
 * bri_chan_init()
 *
 * Description:
 * Init the BRI channels
 * 
 * Parameters
 * idb:
 *
 * Returns:
 * nothing
 * =====================================================================
 */
void bri_chan_init (hwidbtype *idb)
{
    bri_init(idb, TRUE);
}


/*
 * =====================================================================
 * c1000_isdn_activate_line -
 *
 * Description:
 * For isdn initiated activation.
 *
 * Parameters:
 * dchan_idb:	ptr to dchannel idb.
 *
 * Returns:
 * nothing.
 *
 * Notes: Some switch types might not need isdn to do this.
 * so critical during homologation.
 * =====================================================================
 */
static void c1000_isdn_activate_line (hwidbtype *dchan_idb)
{
    leveltype SR;

    /*
     * Try activation only if we have not started the 
     * activation sequence.
     * Send PUP and start PUP timer.
     */
    if (dchan_idb->isdn_info->current_bri_l1_state < l1_state_F4) { 
	SR = raise_interrupt_level(NETS_DISABLE);

	/*
	 * If loopback enabled, then loop away
	 */
	if (dchan_idb->loopback) {
            write_sid(dchan_idb, TP3420_CRF_B1E);
            write_sid(dchan_idb, TP3420_CRF_B2E);
	    write_sid(dchan_idb, TP3420_CRF_LBL1);
	    write_sid(dchan_idb, TP3420_CRF_LBL2);
	} else {
	    write_sid(dchan_idb, TP3420_CRF_CAL);
            write_sid(dchan_idb, TP3420_CRF_B1D);
            write_sid(dchan_idb, TP3420_CRF_B2D);
	    bri_init(dchan_idb, FALSE);
	}

	write_sid(dchan_idb, TP3420_CRF_PUP);

	/*
	 * Start the PUP 2ms timer
	 */
	bri_start_pup_timer();

	if (bri_debug) {
	    buginf("BRI: Starting Power Up timer for unit = %d. \n",
		   dchan_idb->unit);
	}

	dchan_idb->isdn_info->current_bri_l1_state = l1_state_F3;

	reset_interrupt_level(SR);	
    }       
}


/*
 * =====================================================================
 * c1000_isdn_disable_bri_channel -
 *
 * Description:
 * called by isdn code to disable individual B channels to reduce CRC
 * and abort errors before connection.
 *
 * Parameters:
 * bchan_idb:	idb ptr to the bchannel.
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
static void c1000_isdn_disable_bri_channel (hwidbtype *bchan_idb)
{
    leveltype SR;
    enum Chan bchan;
    hwidbtype *dchan_idb;
    bri_instance_t *ds;
    hwidbtype *idb;

    /*
     * Get needed info
     */
    idb = dchan_idb = bchan_idb->isdn_info->d_chan_idb;
    bchan = bchan_idb->isdn_info->chantype;
    ds = DCHAN_INSTANCE;

    if (bchan == ISDN_Dchan)
	return;

    if (bri_debug)
	buginf("BRI: disable channel %s \n", (bchan == B1) ? "B1": "B2");

    SR = raise_interrupt_level(NETS_DISABLE);

    /*
     * Disable the particular B channel by writing SID command
     */
    if (bchan == B1) {
	write_sid(dchan_idb, TP3420_CRF_B1D);
	ds = B1CHAN_INSTANCE;
	quicc_scc_stop(ds->dprbase, ds->qinfo, RX_DISABLED, TX_DISABLED);
	
	/*
	 * Turn off the LED
	 */
	C1000_ISDN_B1_LED_OFF(ds->dprbase);
    }else if (bchan == B2) {
	write_sid(dchan_idb, TP3420_CRF_B2D);
	ds = B2CHAN_INSTANCE;
	quicc_scc_stop(ds->dprbase, ds->qinfo, RX_DISABLED, TX_DISABLED);

	/*
	 * Turn off the LED
	 */
	C1000_ISDN_B2_LED_OFF(ds->dprbase);
    }

    /*
     * Clean up any packets in the receive rings.
     */
    quicc_scc_bd_ring_init(bchan_idb, FALSE);

    reset_interrupt_level(SR);
}


/*
 * =====================================================================
 * c1000_isdn_enable_bri_channel -
 *
 * Description:
 * called by isdn code to enable individual B channels to reduce CRC
 * and abort errors before connection.
 *
 * Parameters:
 * bchan_idb:	idb ptr to the bchannel.
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
static void c1000_isdn_enable_bri_channel (hwidbtype *bchan_idb)
{
    leveltype SR;
    enum Chan bchan;
    hwidbtype *dchan_idb;
    bri_instance_t *ds;
    hwidbtype *idb;

    /*
     * Get needed info
     */
    idb = dchan_idb = bchan_idb->isdn_info->d_chan_idb;
    bchan = bchan_idb->isdn_info->chantype;
    ds = DCHAN_INSTANCE;

    if (bri_debug)
	buginf("BRI: enable channel %s \n", (bchan == B1) ? "B1": "B2");
    
    SR = raise_interrupt_level(NETS_DISABLE);

    /*
     * Enable the particular B channel by writing SID command
     */
    if (bchan == B1) {
	write_sid(dchan_idb, TP3420_CRF_B1E);

	ds = B1CHAN_INSTANCE;
	quicc_scc_start(ds->dprbase, ds->qinfo, RX_ENABLED, TX_ENABLED);

	/*
	 * Turn on the LED
	 */
	C1000_ISDN_B1_LED_ON(ds->dprbase);
    }
    
    else if (bchan == B2) {
	write_sid(dchan_idb, TP3420_CRF_B2E);

	ds = B2CHAN_INSTANCE;
	quicc_scc_start(ds->dprbase, ds->qinfo, RX_ENABLED, TX_ENABLED);

	/*
	 * Turn on the LED
	 */
	C1000_ISDN_B2_LED_ON(ds->dprbase);
    }

    reset_interrupt_level(SR);
}

/*
 * =====================================================================
 * isdn_bri_enable_layer1_Q - This function is called by isdn once a
 * switch type is configured. This will make sure that the layer1 will
 * start queueing primitives only after isdn process is spawned. The
 * interrupts are enabled here for the first time.
 *
 * Parameters: none.
 * Returns:    nothing.
 * Notes:      The interrupts for the MC68302 are enabled here for the
 *             first time. This function will be  called only one time.
 *             Do not make assumptions about shutdown/reset etc.
 * =====================================================================
 */
void isdn_bri_enable_layer1_Q (void)
{
    hwidbtype *idb;

    /*
     * flag for other init/restart routines which need to know about
     * the existence of isdn process.
     */
    isdn_process_up = TRUE;

    /*
     * Enable tp3420 interrupts
     */
    FOR_ALL_HWIDBS(idb) {
	if ((idb->type == IDBTYPE_BRI) &&
	    (idb->isdn_info->chantype == ISDN_Dchan) &&
	    (idb->state != IDBS_ADMINDOWN))
	    tp3420_enable_line(idb);
    }
}


/*
 * =================================================================
 * Name: quicc_bri_fastsend
 *
 * Description:
 *	Called by the fastswitching code to transmit a packet when
 *	being switched to bri.
 *
 * Parameters:
 *	hwidbtype * - hwidb for this interface.
 *	paktype * - pak structure describing packet to be switched
 *
 * Returns:
 *      void - none.
 * =================================================================
 */

static void quicc_bri_fastsend (hwidbtype *idb, paktype *pak)
{
    TXBD_T *tmd;
    bri_instance_t *ds;
    uchar *start;
    ushort bytes;
    ulong acct_proto;
    paktype **txp;
    traffic_shape_t *shape;

    if (idb->state != IDBS_UP) {
        idb->counters.output_total_drops++;
        datagram_done(pak);
        return;
    }

    acct_proto = (ulong)pak->acct_proto;
    ds = (bri_instance_t *)INSTANCE;
    
    bytes = pak->datagramsize;

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
     * try to immediately transmit the packet:
     */
    if (!idb->output_qcount) {

	/*
	 * Get pointer to ring entry and packet pointer at tx_head.
	 */ 
        txp = ds->head_txp;
	tmd = ds->tx_head;
	start = pak->datagramstart;
	
	/*
	 * Is there room for another packet in the output queue?
	 */
	if (ds->tx_count < ds->tx_size) {  

            /* Room in TX ring */

	    if ((tmd->status & QUICC_BDSTAT_TX_RDY) == 0) {
		
		/*
		 * Count broadcasts
		 */
		if ((pak->enctype == ET_HDLC) &&
		    ((*(ushort *)pak->datagramstart) & HDLC_BCAST))
		    idb->counters.output_broad++;
		
		/*
		 * Give the quicc ownership of the buffer now to reduce
		 * latency
		 */
                tmd->buf_ptr = start;
		tmd->length = bytes;
		tmd->status = QUICC_BDSTAT_TX_DEFAULT | 
		    (tmd->status & QUICC_BDSTAT_TX_WRAP);

		/*
		 * Force the quicc to poll the TX ring and see this packet:
		 */
                if (ds->tx_count == 0)
		    *ds->tx_todr = QUICC_TX_KICK;
		ds->tx_count++;
		
		/*
		 * Update the head pointer for the next packet we want to TX:
		 */
                *txp++ = pak;
		if (++ds->tx_head > ds->tx_max) { /* End of ring? */
		    ds->tx_head = ds->tx_ra;
                    txp = &ds->tx_p[0];
		}
                ds->head_txp = txp;

		/*
		 * Do packet accounting after starting the quicc:
		 */
		idb->counters.tx_cumbytes_fast[acct_proto] += bytes;
		idb->counters.outputs_fast[acct_proto]++;

		TIMER_START(idb->xmittimer, TIMEOUT);

	    } else {
                /*
                 * We don't own the buffer
                 */
		retbuffer(pak);
		idb->counters.output_total_drops++;
	    }
	    return;
	} else {			/* TX ring full */
	    /*
	     * We have a full TX ring. Is the first packet in the queue done?
	     * If so, we can service the TX interrupt on this packet and use
	     * the ring entry for our new packet:
	     */
	    if ((tmd->status & QUICC_BDSTAT_TX_RDY) == 0) {
		/*
                 * This packet has been transmitted. Does the BD
		 * need cleaning up ?
		 */
		if (*txp) {

		    /*
                     * BD needs cleaning up.
		     * Check for TX  errors, then release packet buffer.
		     */
	 	    (*ds->TX_errors)(idb, tmd->status);

		    /*
		     * Do TX packet accounting and clean up:
		     */
		    datagram_done(*txp); /* Release output packet*/
		}

		/*
		 * Count broadcasts
		 */
		if ((pak->enctype == ET_HDLC) &&
		    ((*(ushort *)pak->datagramstart) & HDLC_BCAST))
		    idb->counters.output_broad++;

		/*
		 * Give the quicc ownership of the buffer.
		 */
                tmd->buf_ptr = start;
		tmd->length = bytes;
		tmd->status = QUICC_BDSTAT_TX_DEFAULT | 
		    (tmd->status & QUICC_BDSTAT_TX_WRAP);

		/*
		 * Update the head pointer for the next packet we want to TX:
		 */
                *txp++ = pak;
		if (++ds->tx_head > ds->tx_max) {
		    ds->tx_head = ds->tx_ra;
                    txp = &ds->tx_p[0];
		}
                ds->head_txp = txp;
		
		/*
		 * Have the next TX interrupt bypass this packet
		 * since we just took care of it:
		 */
                ds->tail_txp++;
		if (++ds->tx_tail > ds->tx_max) {
		    ds->tx_tail = ds->tx_ra;
                    ds->tail_txp = &ds->tx_p[0];
		}

		/*
		 * Do packet accounting after starting the quicc:
		 */

		/* Don't change ds->tx_count, it should stay the same */
		idb->counters.tx_cumbytes_fast[acct_proto] += bytes;
		idb->counters.outputs_fast[acct_proto]++;
		GET_TIMESTAMP(idb->lastoutput);
		TIMER_START(idb->xmittimer, TIMEOUT); /* set timeout timer */
		
	    } else {			/* BD still in use by QUICC */
		
		/*
		 * We have a full TX queue, so queue the packet in the output
		 * queue. Note that holq_enqueue requires the if_output and
		 * linktype fields to be initialized:
		 */
		holdq_enqueue_fast(idb, pak);
		(*idb->soutput)(idb);
	    }

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
         * the controller (and in particular, the ownership bits, and see
         * if we can clear some space before pulling frames off of
         * the holdq by faking a TX interrupt. The TX interrupt
         * routine will call (*idb->soutput)() for us.
         *
         * This causes a smoother saturation roll-off
         */
        if (ds->tx_count == ds->tx_size)
            quicc_scc_tx_intr(idb);
        else
            bri_tx_start(idb);
    }
}


/*
 * =====================================================================
 * bri_init_idb_vectors -
 *
 * Description:
 * Set the idb function pts. This is called from init_bri_idb_common().
 * This function is separate for 4000 and low-end since the idb
 * vectors are different for the two.
 *
 * Parameters:
 * hwidbtype *idb, could be d channel or b channel.
 *
 * Returns:
 * nothing.
 * =====================================================================
 */
void bri_init_idb_vectors (hwidbtype *idb)
{
    /*
     * Set up function calls
     */
    idb->setencap = serial_encapsulation;
    idb->soutput = bri_tx_start;
    idb->reset = bri_restart;
    idb->shutdown = bri_shutdown;
    idb->enable = bri_enable;
    idb->setup_bridging = NULL;
    idb->periodic = NULL;

    idb->device_periodic = NULL;
    idb->clear_support = NULL;
    idb->show_controller = bri_show_controller;
    idb->txqlength = les_driver_txqlength;
    idb->system_configured = bri_system_configured;
    idb->fastsend = quicc_bri_fastsend;

    /*
     * Init all BRI specific isdn function vectors.
     * This is necessary to support BRI/PRI in the same image.
     */
    idb->isdn_info->activate_line = c1000_isdn_activate_line;
    idb->isdn_info->disable_bri_channel = c1000_isdn_disable_bri_channel;
    idb->isdn_info->enable_bri_channel = c1000_isdn_enable_bri_channel;
    idb->isdn_info->set_bri_bandwidth = c1000_set_bri_bandwidth;
}


/*
 * =====================================================================
 * bri_system_configured()
 *
 * Description:
 * Reinitialize the BRI interfaces:  Since the ISDN process is
 * started until after the driver process the isdn_cstate does not
 * get set since the cstate registry has not been added yet.
 *
 * Parameters:
 * hwidbtype *idb
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_system_configured (hwidbtype *idb)
{
   if (idb->enctype == ET_HDLC)
      bri_init(idb, FALSE);
}


/*
 * =====================================================================
 * init_bri_idb -
 *
 * Description:
 * Initialise the idbs and call init_bri_idb_common which enQs
 * the dchannel idb into the idbQ.
 *
 * Parameters:
 * none
 * 
 * Returns: nothing.
 * =====================================================================
 */
static void init_bri_idb (void)
{
    hwidbtype *dchan_idb = NULL,
              *idb = NULL;
    uint bri_channel;
    bri_instance_t *ds;
    int rx_ring_size, tx_ring_size;

    /*
     * Set all idb variables.
     */
    dchan_idb = init_bri_idb_common(0);
    if (dchan_idb == NULL)
	return;

    for (bri_channel = 0; bri_channel < BRI_MAX_CHAN; bri_channel++) {
	switch (bri_channel) {
	  case BRI_D_CHAN:
	    idb = dchan_idb;
	    break;
	    
	  case BRI_B1_CHAN:
            idb = dchan_idb->isdn_info->b1_chan_idb;
	    break;
	    
	  case BRI_B2_CHAN:
            idb = dchan_idb->isdn_info->b2_chan_idb;
	    break;
	}

        /* 
         * Get an instance
         */
	ds = malloc(sizeof(bri_instance_t));
	if (ds) {
	    INSTANCE = ds;

	    ds->dprbase = (quicc_dpr_t *)getmbar();

	    ds->qinfo = malloc(sizeof(quicc_info_t));
	    
	    if (!ds->qinfo) {
		/* Should probably free ds */
		idb->state = IDBS_DOWN;
		GET_TIMESTAMP(idb->state_time);
		return;
	    }
	    
	    /*
	     * Find the ring sizes
	     */
	    select_ring_size(&rx_ring_size, &tx_ring_size,
			     idb->max_buffer_size);

	    /*
	     * Set up some channel specific parameters.
	     */
	    switch (bri_channel) {
	    case BRI_D_CHAN:
		ds->qinfo->port_no = ds->port_no = QUICC_PORT_SCC3;
		ds->parmbase = (void *)&ds->dprbase->scc3.hdlc;
		ds->max_rx_ring_size = min(QUICC_RXBD_NUM_SCC3, rx_ring_size);
		ds->max_tx_ring_size = min(QUICC_TXBD_NUM_SCC3, tx_ring_size);
		break;
	    
	    case BRI_B1_CHAN:
		ds->qinfo->port_no = ds->port_no = QUICC_PORT_SCC2;
		ds->parmbase = (void *)&ds->dprbase->scc2.hdlc;
		ds->max_rx_ring_size = min(QUICC_RXBD_NUM_SCC2, rx_ring_size);
		ds->max_tx_ring_size = min(QUICC_TXBD_NUM_SCC2, tx_ring_size);
		break;
	    
	    case BRI_B2_CHAN:
		ds->qinfo->port_no = ds->port_no = QUICC_PORT_SCC4;
		ds->parmbase = (void *)&ds->dprbase->scc4.hdlc;
		ds->max_rx_ring_size = min(QUICC_RXBD_NUM_SCC4, rx_ring_size);
		ds->max_tx_ring_size = min(QUICC_TXBD_NUM_SCC4, tx_ring_size);
		break;
	    }

	    ds->rx_handler = bri_RX_interrupt;
	    ds->tx_handler = bri_TX_interrupt;
	    ds->TX_errors  = quicc_bri_check_tx_errors;

	    ds->rx_bd_mask = QUICC_BRI_RX_MASK;
	    ds->rx_bd_good_mask = QUICC_BDSTAT_RX_PKT;

	    ds->qinfo->rx_enable_sccm_mask   = QUICC_RX_BRI_ENABLE_SCCM_MASK;
	    ds->qinfo->tx_enable_sccm_mask   = QUICC_TX_BRI_ENABLE_SCCM_MASK;
    
	    ds->qinfo->rx_clear_scce_event   = QUICC_RX_BRI_CLEAR_SCCE_EVENT;
	    ds->qinfo->tx_clear_scce_event   = QUICC_TX_BRI_CLEAR_SCCE_EVENT;

	    ds->qinfo->rx_disable_sccm_mask  = QUICC_RX_BRI_DISABLE_SCCM_MASK;
	    ds->qinfo->tx_disable_sccm_mask  = QUICC_TX_BRI_DISABLE_SCCM_MASK;

	    ds->crc_length = QUICC_HDLC_CRC_SIZE;
	    
	    quicc_scc_driver_setup(idb);
	    quicc_scc_handler_install(idb, ds->port_no);

	    ds->nt_one = (cookie->interfaces == INTERFACE_1E1N);
	} else {
	    idb->state = IDBS_DOWN;
	    GET_TIMESTAMP(idb->state_time);
	    return;
	}
    }

    /*
     * Intialize the interface
     */
    quicc_init_bri(dchan_idb);
    bri_init(dchan_idb, FALSE);

    /*
     * don't count this init as a reset
     */
    dchan_idb->counters.resets = 0;
    dchan_idb->isdn_info->b1_chan_idb->counters.resets = 0;
    dchan_idb->isdn_info->b2_chan_idb->counters.resets = 0;
}
 

/*
 * =====================================================================
 * analyze_bri -
 *
 * Description: Look for all the BRI interfaces, and initialize them
 *
 * Parameters:
 * nothing
 *
 * Returns:
 * The number for BRI interfaces found
 *
 * =====================================================================
 */
static uint analyze_bri (void)
{
    if ((cookie->interfaces == INTERFACE_1E1B) ||
	(cookie->interfaces == INTERFACE_1E1N)) {
	quicc_dpr_t *dpr;

	bri_init_isdn_queue();	/* Enable layer 1 Q to isdn */
	init_bri_idb();
	bri_parser_init();
	dpr = (quicc_dpr_t *) getmbar();
	/*
	 * Init the BRI timer interrupts
	 */
	quicc_install_cpm_exception(dpr, quicc_timer_t3,
				    QUICC_CPIC_INTERRUPT_TIMER3);
	quicc_install_cpm_exception(dpr, quicc_timer_t4,
				    QUICC_CPIC_INTERRUPT_TIMER4);
	quicc_install_cpm_exception(dpr, quicc_timer_2ms,
				    QUICC_CPIC_INTERRUPT_TIMER2);
	
	return(1);
    }
    
    return(0);
}

/*
 * =====================================================================
 * bri_subsys_init -
 *
 * Description: BRI subsystem initialisation entry point.
 *
 * Parameters:
 * subsys	: ptr to subsystem type.
 *
 * Returns:
 * nothing.
 * 
 * =====================================================================
 */
static void bri_subsys_init (subsystype *subsys)
{
    quicc_dpr_t *dpr;
    
    dpr = (quicc_dpr_t *) getmbar();
    
    /*
     * Turn off the BRI related LED's
     */
    C1000_ISDN_B1_LED_OFF(dpr);
    C1000_ISDN_B2_LED_OFF(dpr);
    
    /*
     * Discover all the BRI serial interfaces in system
     */
    nbrinets += analyze_bri();

    /*
     * If no interfaces don't do any more work.
     */
    if (nbrinets == 0)
	return;

    /*
     * Add in the test interface
     */
    reg_add_test_specific_interface(IDBTYPE_BRI, test_bri_interface,
				    "test_bri_interface");
    if (cookie->interfaces == INTERFACE_1E1N) {
	reg_add_show_int_enhanced(bri_show_int_enhanced,
				  "bri_show_int_enhanced");
    }
}

/*
 * BRI subsystem header
 */
#define BRI_MAJVERSION   1
#define BRI_MINVERSION   0
#define BRI_EDITVERSION  1

SUBSYS_HEADER(bri, BRI_MAJVERSION, BRI_MINVERSION, BRI_EDITVERSION,
              bri_subsys_init, SUBSYS_CLASS_DRIVER,
              NULL,
              NULL);
 
