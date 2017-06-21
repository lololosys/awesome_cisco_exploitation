/* $Id: lane_bridge.c,v 3.6.20.1 1996/05/09 14:32:15 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_bridge.c,v $
 *------------------------------------------------------------------
 * lane/lane_bridge.c
 *
 * Sep, 1995, Badri Ramaswamy
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Registry functions to be provided to tbridge code.
 *------------------------------------------------------------------
 * $Log: lane_bridge.c,v $
 * Revision 3.6.20.1  1996/05/09  14:32:15  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.46.1  1996/04/27  07:06:09  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.36.5  1996/04/21  01:17:08  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Create a new sub-system for TRLANE.
 *
 * Revision 3.6.36.4  1996/04/17  23:14:35  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Convert TokenRing LANE related functions to registries
 * Move contents of lec_srb* srt/srb_lane.[ch], to facilitate
 * creation of new sub-system.
 *
 * Revision 3.6.36.3  1996/04/15  07:26:57  shj
 * CSCdi52538:  IP multicast packets do not fastswitch to LANE
 * Branch: LE_Cal_V112_0_2_branch
 * Allow BUS path to be added to fast cache if destination is multicast.
 * Suppress fast cache only if BUS path will be replaced later with direct
 * path.
 *
 * Revision 3.6.36.2  1996/04/08  14:51:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.6.36.1  1996/04/08  01:56:36  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.26.2  1996/04/06  02:52:19  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * In LE-ARP requests, fill in the source MAC of the originator and not
 * the MAC of client.
 *
 * Revision 3.6.26.1  1996/03/22  22:53:54  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6.10.3  1996/03/08  01:10:13  rbadri
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 * Fix router/Synergy conflicts.
 *
 * Revision 3.6.10.2  1996/03/05  06:42:38  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.6.10.1  1996/03/03  21:23:22  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.6.4.1  1996/02/27  21:09:59  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6  1996/02/05  18:06:52  rbadri
 * CSCdi47734:  LEC responds to LE_ARP requests even in blocked state.
 * Add another registry call for LE_ARP responses. Respond to LE_ARP
 * requests only if LEC is in Forwarding state and the MAC is not learned
 * through the LEC.
 *
 * Revision 3.5.18.1  1996/02/29  07:48:44  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.5.2.1  1996/02/16  21:38:20  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Scheduler interface changes for LANE client code.
 *
 * Revision 3.5  1996/01/05  04:56:05  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.4  1995/12/16  00:26:22  pitargue
 * CSCdi45515:  ST MAC and all ones MAC are off by 2 bytes in show bridge
 * fix comments and optimize a if statement
 *
 * Revision 3.3  1995/11/27  17:59:03  rbadri
 * CSCdi44443:  Bridged datagrams do not learn data direct VC all the time
 * While checking for flooding BTE entries, don't look at the interface.
 * The interface is learned properly only when dara direct is setup.
 *
 * Revision 3.2  1995/11/17  17:50:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.4.2.2.1  1996/02/03  07:07:00  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.4.2  1995/10/21  05:00:44  rbadri
 * CSCdi42387:  tbridge_lane.o is included in all platforms
 * Remove tbridge_lane.o from all makefiles.
 * Move functions previously in tbridge_lane.c to lane_bridge.c
 * Remove reg_add from tbridge_xxx.c and consolidate it in lec.c
 * Add new registry definitions in srt_registry.reg.
 * Branch: Arkansas_branch
 *
 * Revision 2.1.4.1  1995/10/12  03:23:18  rbadri
 * Branch: Arkansas_branch
 * Commiting LE_Ark features to Arkansas_branch
 *
 * Revision 2.1.8.2  1995/10/04  22:41:44  rbadri
 * Branch: LE2_Ark_V110_2_0_2_branch
 * change the parameter in tbridge_lane_bte_update to
 * use BTE states rather than LEC states.
 *
 * Revision 2.1.8.1  1995/09/29  22:57:23  shj
 * Branch: LE2_Ark_V110_2_0_2_branch
 * Non-sync of LE_Ark from V110_1_0_3 to V110_2_0_2.
 *
 * Revision 2.1.2.2  1995/09/27  06:52:08  bbenson
 * Branch: LE_Ark_V110_1_0_3_branch
 * Add check for bridging enabled through a registry since it's configured
 * on the routers and hardcoded on the ACP.
 *
 * Revision 2.1.2.1  1995/09/26  01:28:45  rbadri
 * Branch: LE_Ark_V110_1_0_3_branch
 * restructuring changes for synergy
 *
 * Revision 2.1  1995/09/26  00:45:51  rbadri
 * Placeholder file for Arkansas bridged ELANs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "../ui/debug.h"
#include "parser_defs_lane.h"
#include "sys_registry.h"
#include "media_registry.h"

#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "../atm/atmsig_api.h"
#include "../atm/atmsig_public.h"
#include "../atm/ilmi_api.h"

#include "lane.h"
#include "lane_private.h"
#include "lec_private.h"
#include "lane_signal.h"
#include "lec_fsm.h"
#include "lec_pdu.h"
#include "lane_debug.h"
#include "lane_ilmi.h"
#include "lane_signal.h"
#include "lane_registry.h"


#include "../smf/smf_public.h"
#include "../srt/span.h"
#include "../srt/srt_registry.h"
#include "../tbridge/tbridge_common_inline.h"

/* 
 * Defined within this file
 */
extern boolean          lec_bridge_lookup_mac(lec_info_t *lec_info);
extern void		lec_process_bridge_messages(void);
extern void		lec_bridge_init(void);
extern boolean		lec_bridging_enabled(idbtype *swidb);


/*
 * lec_bridging_enabled
 *
 * Checks to see if bridging is configured on the subinterface.
 * Returns TRUE/FALSE.
 */
boolean lec_bridging_enabled (idbtype *swidb)
{

    if (swidb->tbridge_on_swidb) {
        return(TRUE);
    }
    return (FALSE);

    
}

/*
 * lec_process_bridge_messages
 *
 * Routine to dequeue packets from the queue to initiate LE_ARP requests
 */
void lec_process_bridge_messages (void)
{
    reg_invoke_lec_trigger_le_arp();
}




/*
 * tbridge_lane_bte_update
 *
 * Registry function to update the bridge table entry, called from
 * Lane Client code. 
 */

void 
tbridge_lane_bte_update (void *lec_info, 
			 uchar *mac_addr,
			 ushort vcd, 
			 ushort bte_state)
{
    spantype *span;
    btetype  *bte;
    btetype  *last_bte;
    leveltype level ;
    idbtype  *swidb;

    /* Initialize variables */
    last_bte = NULL;
    swidb    = NULL;
    bte      = NULL;

    if (lec_info) {
       swidb = ((lec_info_t *)lec_info)->swidb;
    } else {
        return;
    }

    level = raise_interrupt_level(NETS_DISABLE);

    span = swidb->span_ptr;
    if (!span) {
	reset_interrupt_level(level);
	return;
    }
    bte = reg_invoke_tbridge_bte_entry_locate(span, mac_addr, &last_bte);
    if (!bte) {
	reset_interrupt_level(level);
	return;
    }

    switch (bte_state) {
        /*
         * update the BTE based on the state of MAC address.
         */
    case BTE_READY:
    case BTE_LANE_FLUSH:
    case BTE_LANE_FLOOD:
    case BTE_LANE_RESOLVED:
    case BTE_LANE_REVERIFYING:
        tbridge_lane_learn_source_inline(bte, swidb, vcd, bte_state);
	break;
 
    case BTE_LANE_DELETE:
	if (bte) {
            /*
	     * To remove the bte, previous entry has already been found in
             * last_bte, using bridge_entry_locate() above.
             */
	    reg_invoke_tbridge_bte_entry_remove(span, bte, last_bte);
	}
	break;
    default:
	buginf("Illegal lec state %d\n", bte_state);
	break;
    }
    reset_interrupt_level(level);
    return;
}



/*
 * Given a MAC address, this function when invoked by a registry call, would
 * try to locate a BTE for the MAC. If found, returns TRUE only if the BTE
 * entry has a non-null interface.
 */


boolean 
tbridge_lane_bte_ready (void *lec_info, uchar *mac)
{

    spantype *span;
    btetype  *bte;
    btetype  *last_bte;
    idbtype  *swidb;
    leveltype level;

    bte = NULL;

    if (lec_info) {
        swidb = ((lec_info_t *)lec_info)->swidb;
    } else { 
       return (FALSE);
    }
    level = raise_interrupt_level(NETS_DISABLE);
    span = swidb->span_ptr;
    if (!span){
	reset_interrupt_level(level);
	return (FALSE);
    }
    bte =  reg_invoke_tbridge_bte_entry_locate(span, mac, &last_bte);

    /* 
     * BTE should exist and the forwarding interface must have been learnt 
     */
    if (bte && bte->bte_interface && (bte->bte_state == BTE_READY)) {

	reset_interrupt_level(level);
	return (TRUE);
    }
    reset_interrupt_level(level);
    return (FALSE);
}


boolean 
tbridge_lane_bte_exists (void *lec_info, uchar *mac)
{

    spantype *span;
    btetype  *bte;
    btetype  *last_bte;
    idbtype  *swidb;
    leveltype level;

    if (lec_info) {
        swidb = ((lec_info_t *)lec_info)->swidb;
    } else { 
       return (FALSE);
    }
    level = raise_interrupt_level(NETS_DISABLE);
    span = swidb->span_ptr;
    if (!span) {
	reset_interrupt_level(level);
	return (FALSE);
    }
    bte =  reg_invoke_tbridge_bte_entry_locate(span, mac, &last_bte);

    if (bte ) {
	reset_interrupt_level(level);
	return (TRUE);
    }
    reset_interrupt_level(level);
    return (FALSE);
}


/*
 * Locate the BTE. If present, make sure that we didn't learn 
 * the MAC addess on this client's interface. If so, it is OK
 * to send LE_ARP responses.
 */
static boolean 
tbridge_lane_arp_respond (void *lec_info, uchar *mac)
{

    spantype *span;
    btetype  *bte;
    btetype  *last_bte;
    idbtype  *swidb;
    leveltype level;

    if (lec_info) {
        swidb = ((lec_info_t *)lec_info)->swidb;
    } else { 
       return (FALSE);
    }
    level = raise_interrupt_level(NETS_DISABLE);
    span = swidb->span_ptr;
    if (!span) {
	reset_interrupt_level(level);
	return (FALSE);
    }
    bte =  reg_invoke_tbridge_bte_entry_locate(span, mac, &last_bte);

    if (bte && bte->bte_interface && bte->bte_interface != swidb &&
	bte->bte_state == BTE_READY) {
	reset_interrupt_level(level);
	return (TRUE);
    }
    reset_interrupt_level(level);
    return (FALSE);
}


/*
 * To move a bridge table entry to BTE_READY state, LANE client code
 * needs to know, if the entry is in flooding state. This function
 * returns a boolean to confirm the flooding state.
 */


boolean 
tbridge_lane_bte_flooding (void *lec_info, uchar *mac)
{

    spantype *span;
    btetype  *bte;
    btetype  *last_bte;
    idbtype  *swidb;
    leveltype level;

    if (lec_info) {
        swidb = ((lec_info_t *)lec_info)->swidb;
    } else { 
       return (FALSE);
    }
    level = raise_interrupt_level(NETS_DISABLE);
    span = swidb->span_ptr;
    if (!span) {
	reset_interrupt_level(level);
	return (FALSE);
    }

    bte =  reg_invoke_tbridge_bte_entry_locate(span, mac, &last_bte);

    /* 
     * BTE should exist and be in the flooding state.
     */
    if (bte &&  bte->bte_state == BTE_LANE_FLOOD ) {
	reset_interrupt_level(level);
	return (TRUE);
    }
    reset_interrupt_level(level);
    return (FALSE);
}

/*
 * Wrapper function for tbridge_lane_arp_respond to make sure that
 * we call the same registry for both Synergy and the Bridge.
 */
boolean lec_bridge_lookup_mac (lec_info_t *lec_info)
{
    lane_ctl_msg_u *msg;

    /*
     * Get the destination mac address from the LE_ARP request and
     * determine, if we can send an LE_ARP response for that MAC.
     */
    msg = (lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart;
    if (msg) {
	if (tbridge_lane_arp_respond(lec_info, 
				     &msg->arp.target_dest.addr.mac[0])) { 
	    return (TRUE);
	}
    }
    return (FALSE);

}


/*
 * lec_bridge_init
 *
 * called by lec_init, to initialize platform dependent registry functions.
 */
void lec_bridge_init (void) 
{
    /*
     * Adds the registry stuff for bridge
     */
    reg_add_tbridge_lane_bte_exists(tbridge_lane_bte_exists,
			            "tbridge_lane_bte_exists");
    
    reg_add_tbridge_lane_bte_ready(tbridge_lane_bte_ready,
				   "tbridge_lane_bte_ready");
    reg_add_tbridge_lane_bte_update(tbridge_lane_bte_update,
				    "tbridge_lane_bte_update");
    reg_add_tbridge_lane_bte_flooding(tbridge_lane_bte_flooding,
				      "tbridge_lane_bte_flooding");
    reg_add_lec_bridging_enabled(lec_bridging_enabled, 
				 "lec_bridging_enabled");
    reg_add_lec_bridge_lookup_mac(lec_bridge_lookup_mac,
                                  "lec_bridge_lookup_mac");
    reg_add_lec_process_bridge_messages(lec_process_bridge_messages,
                                        "lec_process_bridge_messages");
}
