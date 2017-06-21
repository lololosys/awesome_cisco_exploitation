/* $Id: if_les_leased_bri.c,v 3.1.22.4 1996/06/12 16:50:57 wmay Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_leased_bri.c,v $
 *------------------------------------------------------------------
 * Routines specific to the Leased Line ISDN interface feature
 * (D64S/I-interface)
 *
 * February 1996, George M Taylor
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_leased_bri.c,v $
 * Revision 3.1.22.4  1996/06/12  16:50:57  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - put b channel idbs in the idb list.  This removes a
 * lot of special initialization code for BRI.  Also, make the names
 * of the b-channels more consisted with PRI (ie: BRI0:1 and BRI0:2).
 * Branch: California_branch
 *
 * Revision 3.1.22.3  1996/04/29  18:48:43  gtaylor
 * CSCdi55144:  "isdn leased-line BRI0" command removes shutdown.
 * Branch: California_branch
 *
 * Revision 3.1.22.2  1996/04/27  06:33:04  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.22.1  1996/04/22  18:14:39  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.1.54.1  1996/04/17  03:35:02  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.1  1996/02/15  18:02:32  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "../wan/serial.h"
#include "../wan/serial_debug.h"
#include "ttysrv.h"
#include "logger.h"
#include "subsys.h"
#include "if_les.h"
#include "../wan/isdn.h"
#include "../if/network.h"
#include "../os/buffers.h"
#include "../if/if_vidb.h"
#include "packet.h"
#include "address.h"
#include "../wan/dialer.h"
#include "../wan/dialer_private.h"
#include "isdn_bri.h"
#include "if_les_bri.h"
#include "../parser/parser_defs_parser.h"
#include "../wan/serial_parser.h"


/*
 * =====================================================================
 * isdn_cstate_leased_bchan
 *
 * Description:
 *   Manages physicial & configured line state changes for B channels
 *   configured as leased lines.
 *
 * Parameters:
 *   hwidb    - hardware IDB for a B channel.
 *   newstate - The state to which this interface is intending to change to.
 *
 * Returns:
 *   Nothing.
 *
 * =====================================================================
 */
void isdn_cstate_leased_bchan (hwidbtype *hwidb, int newstate)
{
    isdn_bri_t *isdn_info;

    isdn_info = hwidb->isdn_info;
    if (newstate != IDBS_ADMINDOWN) {
	if(!is_layer1_current_state_activated(hwidb))
	    newstate = IDBS_DOWN;
    }
    if(newstate == IDBS_UP)
	(*isdn_info->enable_bri_channel)(hwidb);
    else
	(*isdn_info->disable_bri_channel)(hwidb);
    net_cstate(hwidb, newstate);
}

/*
 * =====================================================================
 * isdn_cstate_leased_dchan
 *
 * Description:
 *   Manages physicial & configured line state changes for the
 *   D Channel when it is configured as a leased line.
 *
 * Parameters:
 *   hwidb    - hardware IDB for the D Channel.
 *   newstate - The state to which this interface is intending to change to.
 *
 * Returns:
 *   Nothing.
 *
 * =====================================================================
 */
void isdn_cstate_leased_dchan (hwidbtype *hwidb, int newstate)
{
    isdn_bri_t *isdn_info;

    isdn_info = hwidb->isdn_info;
    /*
     * Need to do something about shutting down the D channel physically.
     * This is difficult since most drivers and hardware controllers assume
     * that if you have no D then you don't need B's... :-(
     */
    if (newstate != IDBS_ADMINDOWN) {
	if(!is_layer1_current_state_activated(hwidb))
	    newstate = IDBS_DOWN;
    }
    net_cstate(hwidb, newstate);
}

/*
 * =====================================================================
 * init_bchan_leased_line
 *
 * Description:
 *   Reconfigures the hwidb & swidb's for a B channel to transform it from
 *   being a dialer slave to being a individual interface in it's own right.
 *
 * Parameters:
 *   hwidb  - hardware IDB for a B channel.
 *   b_num  - The B Channel number (either 1 or 2)
 *   lineup - Is the linestate currently ACTIVATED.
 *
 * Returns:
 *   Nothing.
 *
 * =====================================================================
 */
void init_bchan_leased_line (hwidbtype *hwidb, int b_num, boolean lineup)
{
    idbtype *swidb;

    /*
     * Remove from D channels hunt group.
     */
    if (hwidb->dialerdb)
	add_to_hunt_group(hwidb->dialerdb, hwidb->unit, FALSE, NULL);

    /*
     * Remove the dialer from the hardware idb
     */
    dialer_remove(hwidb);

    swidb = hwidb->firstsw;
    hwidb->type = IDBTYPE_UNKNOWN;
    hwidb->isdn_info->leased_line = TRUE;
    hwidb->setencap = serial_encapsulation;
    hwidb->reset = bri_restart;
    hdlc_setup(NULL,hwidb);
    hwidb->show_controller = NULL;

    /*
     * Setup the protocols on this new interface
     */
    reg_invoke_sw_idb_init(swidb, TRUE);

    /*
     * We don't want to appear to have B Channels attached to B channels.
     */
    hwidb->isdn_info->b1_chan_idb = NULL;
    hwidb->isdn_info->b2_chan_idb = NULL;

    /*
     * This deals with interface state changes due to shut down etc..
     */
    hwidb->isdn_info->cstate_bri_leased = isdn_cstate_leased_bchan;

    /*
     * Let the NV gen uses this idb
     */
    hwidb->status &= ~IDB_NO_NVGEN;

    /*
     * Leased lines over BRI are always UP when interface is connected.
     */
    if(lineup)
	isdn_cstate_leased_bchan(hwidb, IDBS_UP);
    else
	isdn_cstate_leased_dchan(hwidb, IDBS_DOWN);
}

/*
 * =====================================================================
 * init_dchan_leased_line
 *
 * Description:
 *   Performs defaults settings and line state changes in preparation for the
 *   D Channels new role as a serial leased line.
 *
 * Parameters:
 *   hwidb  - hardware IDB for the D Channel.
 *   lineup - Is the linestate currently ACTIVATED.
 *
 * Returns:
 *   Nothing.
 *
 * =====================================================================
 */
void init_dchan_leased_line (hwidbtype *hwidb, boolean lineup)
{
    /*
     * Prevent the dialer from trying to use the D channel to set up calls
     */
    dialer_remove(hwidb);

    hwidb->type = IDBTYPE_BRI;
    hwidb->setencap = serial_encapsulation;
    hwidb->isdn_info->cstate_bri_leased = isdn_cstate_leased_dchan;
    hwidb->reset = bri_restart;

    hdlc_setup(NULL,hwidb);

    if (lineup)
	isdn_cstate_leased_dchan(hwidb, IDBS_UP);
    else
	isdn_cstate_leased_dchan(hwidb, IDBS_DOWN);
}

/*
 * =====================================================================
 * init_bri_leased_line
 *
 * Description:
 *  This function is the front end to the routines which configure a given
 *  ISDN BRI interface to work as a leased line interface instead of the
 *  normal dialer interface opertation.
 *
 * Parameters:
 *   hwidb  - hardware IDB for the BRI interface (normally the D Channel).
 *
 * Returns:
 *   Nothing.
 *
 * =====================================================================
 */
void init_bri_leased_line (hwidbtype *hwidb)
{
    isdn_bri_t *isdn_info;
    idbtype *swidb;
    boolean lineup, shutdown;

    isdn_info = hwidb->isdn_info;
    
    /*
     * Check that this isn't being attempted on a PRI interface
     */
    if(IS_PRI(hwidb)){
	printf("\n%%This command is for Basic Rate Interfaces only");
	return;
    }

    /*
     * If no switch type has been configured then start the BRI hardware.
     */
    if(!isdn_fork)
	isdn_bri_enable_layer1_Q();

    /*
     * Make sure we're looking at the D channel info.
     */
    if (isdn_info->chantype != ISDN_Dchan)
	isdn_info = isdn_info->d_chan_idb->isdn_info;

    /*
     * If the interface is shutdown, bring it up before configuring it.
     */
    if (hwidb->state == IDBS_ADMINDOWN){
	shutdown = TRUE;
	shutdown_coming_up (hwidb);
    } else
	shutdown = FALSE;

    /*
     * Reset BRI interface
     */
    bri_init(hwidb, FALSE);

    /*
     * D channel is now marked as Leased Line and in so doing so marks the
     * interface as Leased line.
     */
    isdn_info->leased_line = TRUE;

    /*
     * Tell the protocols that the D channel is out of the picture.
     */
    reg_invoke_hwif_goingdown(hwidb);
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb)
	reg_invoke_swif_goingdown(swidb);

    /*
     * Activate the BRI interface.
     */
    (*isdn_info->activate_line)(hwidb);

    /*
     * We need to know the current status of the line for initialisation.
     */
    lineup = is_layer1_current_state_activated(hwidb);

    /*
     * Setup the B channel hwidb's as individual serial line interfaces
     */
    init_bchan_leased_line(hwidb->isdn_info->b1_chan_idb, 1, lineup);
    init_bchan_leased_line(hwidb->isdn_info->b2_chan_idb, 2, lineup);

    /*
     * Setup the D channel hwidb as individual serial line interface.
     * Must do this after the B channel leased initiate, since they refer
     * back to the D channel when removing themselves from it's hunt group.
     */
    init_dchan_leased_line(hwidb, lineup);
    
    /*
     * If the interface was administratively down then we must preserve this
     * admin state for all 3 interfaces (BRI#, BRI#:1 & BRI#:2).
     */
    if(shutdown){
	shutdown_going_down(hwidb);
	shutdown_going_down(hwidb->isdn_info->b1_chan_idb);
	shutdown_going_down(hwidb->isdn_info->b2_chan_idb);
    }
}

