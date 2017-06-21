/* $Id: if_c3000_local_lnm.c,v 3.5.8.3 1996/08/26 15:10:12 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_local_lnm.c,v $
 *------------------------------------------------------------------
 * if_c3000_local_lnm.c - Local LNM driver  (for Fiji)
 *
 * September 1994, Johnny Chan
 *
 * Copyright (c) 1995-1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_local_lnm.c,v $
 * Revision 3.5.8.3  1996/08/26  15:10:12  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.5.8.2  1996/07/09  06:00:25  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.5.8.1  1996/05/17  11:26:08  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.14.2  1996/04/26  15:22:01  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.14.1  1996/03/17  18:06:42  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.5  1996/02/27  19:30:34  jrosen
 * CSCdi47898:  no lnm disabled crashes router if CIP internal lan exists
 * - Make sure that set_extra_addrs member of the hardware IDB is
 *   non-NULL before calling the function it points to.
 *
 * Revision 3.4  1996/01/22  06:37:30  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/17  17:38:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:38  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  03:00:58  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  21:33:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "logger.h"
#include "../partner/partner_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../wan/serial.h"
#include "ieee.h"
#include "../ibm/lanmgr.h"
#include "../srt/srb_sb.h"
#include "../ibm/ibmnm.h" 
#include "../if/tring_common.h"
#include "../srt/rsrb.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"
#include "../if/network.h"

#include "if_c3000_pcbus.h"
#include "if_c3000_local_lnm.h"


/*
 * local_lnm_get_pcbus_idb_inline
 *
 * Return IDB for the PCbus interface if one exists.
 */
static inline hwidbtype *local_lnm_get_pcbus_idb_inline (void)
{
    hwidbtype *tmpidb;

    FOR_ALL_HWIDBS(tmpidb) {
        if (tmpidb->status & IDB_PCBUS) 
                break; 
    }

   if (!tmpidb) {
       return (NULL);
   } else {
       return (tmpidb);
   }
}


/*
 * local_lnm_get_tr_idb_inline
 *
 * Return IDB for the Token Ring interface if one exists.
 */
static inline hwidbtype *local_lnm_get_tr_idb_inline (void)
{
    hwidbtype *tmpidb;
    list_element *l_elt;

    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_TOKEN, l_elt, tmpidb) {
	return(tmpidb);
    }

    return (NULL);
}


/*
 * local_lnm_set_tr_vr_bridge
 *
 * Setup the virtual bridge between the Token Ring interface and
 * the PCbus interface.
 */
static void local_lnm_set_tr_vr_bridge (hwidbtype *tr_hwidb,
					hwidbtype *pcbus_hwidb)
{
    ring_info *ring;
    idbtype *tr_swidb    = firstsw_or_null(tr_hwidb);
    idbtype *pcbus_swidb = firstsw_or_null(pcbus_hwidb);
    srb_triplet_t tr_srb_triplet;
    srb_triplet_t pcbus_srb_triplet;
    srb_vector_table_t *srbV_p;

    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {

        tr_srb_triplet.localRing  = 0;
        tr_srb_triplet.bridgeNum  = 0;
        tr_srb_triplet.remoteRing = 0;
        srbV_p->srb_sb_set_triplet(tr_swidb, &tr_srb_triplet);

        pcbus_srb_triplet.localRing  = SRB_MAX_RING_NUM + 1;
        pcbus_srb_triplet.bridgeNum  = 0;
        pcbus_srb_triplet.remoteRing = 0;
        srbV_p->srb_sb_set_triplet(pcbus_swidb, &pcbus_srb_triplet);

    }

    tr_hwidb->vidb_link = pcbus_hwidb;
    lanmgr_create_ring(tr_hwidb);
    set_ibmnm_interfaces(tr_hwidb, TRUE);
    tr_hwidb->vidb_link = NULL;
    if (srbV_p) {
        srbV_p->srb_sb_set_thisring(pcbus_swidb, 0);
    }

    /* 
     * Need to turn off RPS since virtual ring 0 will confuse 
     * the ring stations on the physical ring.
     */
    tr_hwidb->tr_ring_mode &= ~RINGMODE_RPS;
    ring = idb_to_ring(tr_hwidb);
    if (ring)
        ring->rps_state = RPS_STATE_OFF;
}


/*
 * local_lnm_vr_bridge
 *
 * Check if the virtual bridge has been built.
 *    Flag = TRUE if the virtual bridge exists
 *           FALSE otherwise
 */
static boolean local_lnm_vr_bridge (hwidbtype *hwidb)
{
    srb_triplet_t srb_triplet;
    srb_vector_table_t *srbV_p;

    srbV_p = reg_invoke_srb_get_vector_table();
    if (!srbV_p) {
        return(FALSE);
    } 
    srbV_p->srb_sb_get_triplet(firstsw_or_null(hwidb), &srb_triplet);
    if ((srb_triplet.bridgeNum == 0) && (srb_triplet.localRing == 0)) 
        return (TRUE);
    else
	return (FALSE);
}


/* 
 * local_lnm_unset_tr_vr_bridge
 *
 * Remove the virtual bridge between the Token Ring interface and
 * the PCbus interface.
 */
static void local_lnm_unset_tr_vr_bridge (hwidbtype *tr_hwidb,
					  hwidbtype *pcbus_hwidb)
{
    idbtype *tr_swidb;
    idbtype *pcbus_swidb;
    srb_vector_table_t *srbV_p;
    srb_triplet_t tr_srb_triplet;
    srb_triplet_t pcbus_srb_triplet;

    srbV_p = reg_invoke_srb_get_vector_table();
    if (!srbV_p) {
        return;
    }

    tr_swidb    = firstsw_or_null(tr_hwidb);
    pcbus_swidb = firstsw_or_null(pcbus_hwidb);

    srbV_p->srb_sb_get_triplet(tr_swidb, &tr_srb_triplet);
    srbV_p->srb_sb_get_triplet(pcbus_swidb, &pcbus_srb_triplet);
    if ((tr_srb_triplet.bridgeNum == 0) && (tr_srb_triplet.localRing == 0)) {

        pcbus_srb_triplet.remoteRing = SRB_MAX_RING_NUM + 1;
        pcbus_srb_triplet.bridgeNum = SRB_INVALID_BRIDGE_NUM;
        srbV_p->srb_sb_set_triplet(pcbus_swidb, &pcbus_srb_triplet);

        srbV_p->srb_sb_set_bridge_num(tr_swidb, SRB_INVALID_BRIDGE_NUM);

        tr_hwidb->vidb_link = NULL;;
        set_ibmnm_interfaces(tr_hwidb, FALSE);

        tr_srb_triplet.localRing  = 0;
        tr_srb_triplet.remoteRing = 0;
        srbV_p->srb_sb_set_triplet(tr_swidb, &tr_srb_triplet);

        pcbus_srb_triplet.localRing  = 0;
        pcbus_srb_triplet.remoteRing = 0;
        srbV_p->srb_sb_set_triplet(pcbus_swidb, &pcbus_srb_triplet);

        lanmgr_destroy_ring(tr_hwidb);
    }
}


/*
 * local_lnm_incr_claim_tok_count
 *
 * Increment the claim token count received for the Token Ring interface. 
 */
static void local_lnm_incr_claim_tok_count (hwidbtype *hwidb)
{
	hwidb->local_lnm_claim_count++;
}


/*
 * local_lnm_incr_purge_tok_count
 *
 * Increment the purg token count received for the Token Ring interface. 
 */
static void local_lnm_incr_purge_tok_count (hwidbtype *hwidb)
{
	hwidb->local_lnm_purge_count++;
}


/* 
 * local_lnm_terminate_lnm_session
 *
 * Send LRM-terminate to all the linked sessions thru the virtual
 * bridge.
 */ 
static void local_lnm_terminate_lnm_session (hwidbtype *hwidb)
{
    bridge_info *bridge;
    hwidbtype *tmpidb;
    int link;

   /* 
    * Make check to make sure the PCbus interface exists in this system 
    * before continue on.
    */
   tmpidb = local_lnm_get_pcbus_idb_inline();
   if (!tmpidb) 
       return;

   /*  
    * Only send the LRM-terminate when the local lnm feature is active,
    * and there are LRM sessions on going.
    */
   if ((tmpidb->local_lnm_enabled) &&
       (tmpidb->lrm_session_est)) {
       if ((bridge = idb_to_bridge(hwidb))) {
            for (link = 0; link < MAXLINKS; link++) {
                if (bridge->links[link].cookie == (llctype *) tmpidb) {
                    lrm_terminating(bridge, LRM_TERM_NORMAL); 
		    destroy_link(bridge, link);
		    lrm_find_controlling(bridge, LRM_CNTL_CLOSE);
                }
            }
       }
   }
}


/*
 * local_lnm_reset_count
 *
 * Reset the claim and purge token count to 0.
 */
static void local_lnm_reset_count (hwidbtype *hwidb)
{
    /* 
     * Make check to make sure the PCbus interface exists in this system 
     * before continue on.
     */
    if (!local_lnm_get_pcbus_idb_inline()) 
        return;

    hwidb->local_lnm_purge_count = 0;
    hwidb->local_lnm_claim_count = 0;
}

/*
 * local_lnm_process_link_nmp_packets
 *
 * Process the LINK_NMP packet so it can be sent to the
 * IBM subsystem.
 */
static void local_lnm_process_link_nmp_packets (paktype *pak)
{
    paktype *lan_pak;
    ushort count;
    hwidbtype *hwidb;
    ibmnm_vector *ptr, *vec;
    bridge_info *bridge;
    int link;

    hwidb = pak->if_input->hwptr;
    
    /*
     * If the LAN manager process is UP and the local lmn
     * is enabled for the interface, then send it.
     */
    if ((lanmgr_pid) && (hwidb->local_lnm_enabled)) {
        lan_pak = pak_duplicate(pak);
        if (lan_pak)
        {
	    vec = (ibmnm_vector *)ibmnm_vector_headstart(lan_pak);
            count = pak->datagramsize - HDLC_ENCAPBYTES;
            lan_pak->datagramstart = (uchar *) vec;
	    ptr = (ibmnm_vector *)(pak->datagramstart + HDLC_ENCAPBYTES);
	    vec->length = ptr->length;
	    vec->command = ptr->command;
            bcopy(pak->datagramstart + (HDLC_ENCAPBYTES * 2), 
		  lan_pak->datagramstart + HDLC_ENCAPBYTES,  
		  (uint)vec->length);
            lan_pak->datagramsize = count;

	    /* 
	     * If there is a LRM session going on, then always
	     * set up the llc2 cookie value.
	     */
	    if (hwidb->lrm_session_est) { 
		lan_pak->llc2_cb = (llctype *) hwidb;
 	    } else {
		/*
		 * If there is no LRM session going on, but there
		 * is a link in the bridge table, allow LRM_CLOSE_LINK
		 * packet to go thru to clean it up. 
		 */
    		if ((cookie_to_link((llctype *) hwidb, &bridge, &link)) &&
		    (vec->command == LRM_CLOSE_LINK))
		    lan_pak->llc2_cb = (llctype *) hwidb;
		else
		    lan_pak->llc2_cb = NULL;
	    }

   	    if (pcbus_debug)
       		buginf("\n%%%s: NMP packet received: 0x%x", 
		       hwidb->hw_namestring, ptr->command);

            retbuffer(pak);
            p_enqueue(&ibmnmQ, lan_pak);

        }
    } else {
	datagram_done(pak);
    }
}


/*
 * local_lnm_set_rep_link_mac_address
 *
 * For the SET_REP_LINK packet, build the packet with the mac address
 * of the Token Ring interface.
 */
static void local_lnm_set_rep_link_mac_address (uchar *vec, hwidbtype *hwidb)
{

    ibmnm_vector *sv;
    uchar *to, *from;
    hwidbtype *tmpidb;
 
    /* 
     * Make check to make sure the PCbus interface exists in this system 
     * before continue on.
     */
    tmpidb = local_lnm_get_pcbus_idb_inline();
    if (!tmpidb) 
        return;

    /* If PCbus does not exist or the packet is not for it, then do nothing */
    if (tmpidb != hwidb) 
        return;

   /* 
    * Make check to make sure the Token Ring interface exists in this system 
    * before continue on.
    */
    tmpidb = local_lnm_get_tr_idb_inline();
    if (!tmpidb)
	return;

    sv = (ibmnm_vector *)vec;
    to = (uchar *)&sv->data;
    from = (uchar *)&tmpidb->hardware;
    *(ulong  *)&to[0] = *(ulong  *)&from[0];
    *(ushort *)&to[4] = *(ushort *)&from[4];
}


/*
 * local_lnm_slarp
 *
 * Called to set up the local lnm related information for the
 * slarp special reply packet if Local LNM is enabled.
 */
static boolean local_lnm_slarp (uchar *ptr, hwidbtype *pcbus_hwidb,
				hwidbtype *tr_hwidb)
{
    slarp_sp_reply_t *sp_ptr;

    /*
     * Send slarp information for local lnm only if
     * it is enabled.
     */
    if (pcbus_hwidb->local_lnm_enabled) 
    { 
        sp_ptr = (slarp_sp_reply_t *)ptr;
        sp_ptr->purge_ring_counter = tr_hwidb->local_lnm_purge_count; 
        sp_ptr->claim_ring_counter = tr_hwidb->local_lnm_claim_count; 
        sp_ptr->LNM_support = 1;
	sp_ptr->adapter_status = ((*(tr_hwidb->linestate))(tr_hwidb) ? 0 : 1);
        return (TRUE);
    } 
    return(FALSE);
}


/*
 * local_lnm_llc2_oqueue_status
 *
 * Interpret the cookie value to determine if it is a PCbus
 * link thru the virtual bridge. Return TRUE to cause a bypass
 * to the regular llc2_oqueue_status function.
 */
static boolean local_lnm_llc2_oqueue_status (llctype *cookie)
{
    hwidbtype *hwidb;

    /* 
     * Make check to make sure the PCbus interface exists in this system 
     * before continue on.
     */
    hwidb = local_lnm_get_pcbus_idb_inline();
    if (!hwidb) 
        return (FALSE);
 
    
    if (cookie != (llctype *) hwidb) 
        return (FALSE);

    return (TRUE);
}


/* 
 * local_lnm_llc2_send
 *
 * Interpret the cookie value to determine if it is a PCbus
 * Packet. Return TRUE to cause a bypass to the regular llc2_send 
 * function and send the packet to the PCbus output queue.
 */
static boolean local_lnm_llc2_send (llctype *cookie, paktype *pak)
{
    hwidbtype *hwidb;
    paktype *lan_pak;
    int count;
    pcbus_frame_t *pcbus_frame;
    ibmnm_vector *vec;
   
    /* 
     * Make check to make sure the PCbus interface exists in this system 
     * and it is equal to the cookie before continue on.
     */
    hwidb = local_lnm_get_pcbus_idb_inline(); 
    if ((!hwidb) || 
        (cookie != (llctype *) hwidb)) 
        return (FALSE);

    /* 
     * Want to make sure the PCbus interface is up 
     */
    if (!hw_interface_up(hwidb)) {
        datagram_done(pak);
        return (TRUE);
    }

    /* 
     * Transform the packet from the IBM format to the one for the
     * PCbus.
     */
    lan_pak = pak_duplicate(pak);
    if (lan_pak)
    {
        vec = (ibmnm_vector *)ibmnm_vector_headstart(lan_pak);
        count = vec->length;
        lan_pak->datagramstart = lan_pak->data_area + ENCAPBYTES -
	    HDLC_ENCAPBYTES;
	pcbus_frame = (pcbus_frame_t *) lan_pak->datagramstart;
	pcbus_frame->pcbus_station = HDLC_STATION;
        bcopy(pak->datagramstart, lan_pak->datagramstart + HDLC_ENCAPBYTES,
	      (uint)count);

        lan_pak->if_output = hwidb->firstsw;
        lan_pak->enctype = ET_HDLC;
        lan_pak->linktype = LINK_NMP;

        if (pcbus_debug)
            buginf("\n%%%s: NMP packet transmitted: 0x%x", 
                   hwidb->hw_namestring, vec->command);
 

        if (pak_encap(hwidb, lan_pak, count, 0)) 
            datagram_out(lan_pak);
    }

    datagram_done(pak);
    return(TRUE);
}


/* 
 * local_lnm_set_802addr
 *
 * Set the 802 address for the PCbus interface based packet with
 * the mac address of the Token Ring interface.
 */
static void local_lnm_set_802addr (paktype *pak, uchar *dest, boolean which)
{
    hwidbtype *hwidb, *tmpidb; 

    hwidb = pak->if_input->hwptr;

    if (hwidb->status & IDB_PCBUS) {
        tmpidb = local_lnm_get_tr_idb_inline();
        if (tmpidb)
	    ieee_copy((uchar *)&tmpidb->hardware, dest);
    }
}


/*
 * local_lnm_reset_tr_vr_bridge
 * 
 * Rebuild the PCbus virtual bridge.
 */
static void local_lnm_reset_tr_vr_bridge (hwidbtype *hwidb)
{
    hwidbtype *tmpidb;

    /* 
     * Make check to make sure the PCbus interface exists in this system 
     * before continue on.
     */
    tmpidb = local_lnm_get_pcbus_idb_inline();
    if (!tmpidb) 
        return;
 
    if (tmpidb->local_lnm_enabled)
        local_lnm_set_tr_vr_bridge(hwidb, tmpidb);
}


/*
 * local_lnm_llc2_open
 *
 * Interpret the IDB type of the request link to determine if it 
 * is a PCbus link thru the virtual bridge. Return TRUE to cause a bypass
 * to the regular llc2_open function. It also set the LRM session
 * established flag for the PCbus interface IDB and pass back the
 * IDB pointer as the cookie value for the link.
 */
static boolean local_lnm_llc2_open (idbtype *idb, llctype **cookie)
{
    hwidbtype *tmpidb;

    *cookie = NULL;

    /* 
     * Make check to make sure the PCbus interface exists in this system 
     * before continue on.
     */
    tmpidb = local_lnm_get_pcbus_idb_inline();
    if (!tmpidb)
        return (FALSE);

    /* 
     * If it is for a PCbus packet, set the flag and return
     * the cookie value with the IDB pointer.
     */
    if (tmpidb == idb->hwptr) {
	tmpidb->lrm_session_est = TRUE;
        *cookie = (llctype *) tmpidb;
        return (TRUE);
    } else {
        return (FALSE);
    }
}


/*
 * local_lnm_llc2_close
 *
 * Interpret the cookie value of the request link to determine if it 
 * is a PCbus link thru the virtual bridge. Return TRUE to cause a bypass
 * to the regular llc2_close function. It also reset the LRM session
 * established flag for the PCbus interface IDB.
 */
static boolean local_lnm_llc2_close (llctype *cookie)
{
    hwidbtype *tmpidb;

    /* 
     * Make check to make sure the PCbus interface exists in this system 
     * before continue on.
     */
    tmpidb = local_lnm_get_pcbus_idb_inline();
    if (!tmpidb) 
        return (FALSE);

    /* 
     * If it is a PCbus link, reset the flag.
     */
    if ((llctype *) tmpidb == cookie) {
	tmpidb->lrm_session_est = FALSE;
        return (TRUE);
    } else {
        return (FALSE);
    }
}


/* 
 * local_lnm_ringno
 *
 * Return TRUE if the ring number is a virtual ring (0).
 */
static boolean local_lnm_ringno (ushort ringno)
{
    hwidbtype *tmpidb;
 
    /* 
     * Make check to make sure the PCbus interface exists in this system 
     * before continue on.
     */
    tmpidb = local_lnm_get_pcbus_idb_inline();
    if (!tmpidb) 
        return (FALSE);

    if (tmpidb->lrm_session_est && (!ringno)) {
	return (TRUE);
    } else {
	return (FALSE);
    }
}


/*
 * local_lnm_cstate
 *
 * Close the active LRM link if the PCbus interface went down.
 * Called when the net_background detects carrier transitions on 
 * interfaces. 
 */
static void local_lnm_cstate (hwidbtype *idb)
{
    bridge_info *bridge;
    int link;

    /*
     * Close the link if the interface state is not UP
     */
    if (idb->state != IDBS_UP) {
        if (cookie_to_link((llctype *)idb, &bridge, &link)) {
            unset_rif(bridge->links[link].idb, 0,
                      bridge->links[link].address, NULL, FALSE);
            destroy_link(bridge, link);
            local_lnm_llc2_close((llctype *)idb);
            lrm_find_controlling(bridge, LRM_CNTL_ABORT);
        }
    }
}


/*
 * local_lnm_enable_command
 *
 * Enable or disable local_lnm processing.
 *	"local-lnm"	enable local lnm feature.
 *  	"no local-lnm"  disable local lnm feature.
 */
void local_lnm_enable_command (parseinfo *csb)
{
    hwidbtype *hwidb,  *tmpidb;
    srb_vector_table_t *srbV_p;
    srb_triplet_t       srb_triplet;

    hwidb = csb->interface->hwptr;
    tmpidb = local_lnm_get_tr_idb_inline();
    if (!tmpidb) {
 	printf("Unsupported command for non token ring platform");
    }

    if (csb->nvgen) { 
        nv_write(hwidb->local_lnm_enabled, csb->nv_command);
	return;
    }

    /*
     * If the choice is ""local_lnm"" ,then
     * set enable flag and start the LAN manager
     * process just in case it is no up.
     */
    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        srbV_p->srb_sb_get_triplet(tmpidb->firstsw, &srb_triplet);
    } else {
        srb_triplet.localRing  = SRB_INVALID_RING_NUM;
        srb_triplet.bridgeNum  = SRB_INVALID_BRIDGE_NUM;
        srb_triplet.remoteRing = SRB_INVALID_RING_NUM;
    }
    if (csb->sense) {
        hwidb->local_lnm_enabled = TRUE;

        /*
         * Configure the token ring interface
         * with virtual bridge if it does not have
         * bridging configured.
         */
        if (srb_triplet.bridgeNum == SRB_INVALID_BRIDGE_NUM) {

	    local_lnm_set_tr_vr_bridge(tmpidb, hwidb);

            if ((tmpidb->set_extra_addrs != NULL) &&
		(tmpidb->state != IDBS_ADMINDOWN))
	        (*(tmpidb->set_extra_addrs))(tmpidb);
        }
    } else {
	if (hwidb->lrm_session_est) 
	    local_lnm_terminate_lnm_session(tmpidb);

	/*
 	 * Remove Virtual Ring only if it is set.
	 */
        if ((srb_triplet.bridgeNum == 0) && 
            (srb_triplet.localRing == 0)) {
            local_lnm_unset_tr_vr_bridge(tmpidb, hwidb);
            if ((tmpidb->set_extra_addrs != NULL) &&
		(tmpidb->state != IDBS_ADMINDOWN))
	        (*(tmpidb->set_extra_addrs))(tmpidb);
        }

       	hwidb->local_lnm_enabled = FALSE;
    }
}


/*
 * local_lnm_subsys_init
 *
 * LOCAL LNM subsystem initialisation entry point
 */
static void local_lnm_subsys_init (subsystype *subsys)
{
    /* 
     * Continue to initialize this subsystem if this is
     * a Token Ring based card.
     */
    if (!reg_invoke_if_pcbus_with_tr())
	return;

    /*
     * Now register all the access point routines. 
     */
    reg_add_media_serial_to_link(TYPE_NMP, LINK_NMP, "local_lnm type");
 
    reg_add_media_link_to_type(LINK_NMP, TYPE_NMP, "local_lnm type");
 
    reg_add_inc_local_lnm_claim_token_counter(local_lnm_incr_claim_tok_count, 
					      "local_lnm_incr_claim_tok_count");
 
    reg_add_inc_local_lnm_purge_token_counter(local_lnm_incr_purge_tok_count, 
					      "local_lnm_incr_purge_tok_count");
 
    reg_add_local_lnm_disable(local_lnm_terminate_lnm_session, 
			      "local_lnm_terminate_lnm_session");
 
    reg_add_local_lnm_reset_count(local_lnm_reset_count, 
				  "local_lnm_reset_count");
 
    reg_add_local_lnm_link_nmp(local_lnm_process_link_nmp_packets, 
			       "local_lnm_process_link_nmp_packets");
 
    reg_add_local_lnm_set_mac_address(local_lnm_set_rep_link_mac_address, 
				      "local_lnm_set_rep_link_mac_address");
 
    reg_add_local_lnm_slarp(local_lnm_slarp, "local_lnm_slarp");
 
    reg_add_local_lnm_llc2_send(local_lnm_llc2_send, "local_lnm_llc2_send");

    reg_add_local_lnm_pcbus_send(local_lnm_llc2_send, "local_lnm_llc2_send");
 
    reg_add_local_lnm_reset_vr_bridge(local_lnm_reset_tr_vr_bridge, 
				      "local_lnm_reset_tr_vr_bridge");

    reg_add_local_lnm_llc2_open(local_lnm_llc2_open, "local_lnm_llc2_open");

    reg_add_local_lnm_llc2_close(local_lnm_llc2_close, "local_lnm_llc2_close");

    reg_add_if_pcbus_down(local_lnm_cstate, "local_lnm_cstate");

    reg_add_local_lnm_llc2_oqueue_status(local_lnm_llc2_oqueue_status, 
					 "local_lnm_llc2_oqueue_status");

    reg_add_local_lnm_find_802addr(local_lnm_set_802addr,
				   "local_lnm_set_802addr");

    reg_add_raw_enqueue(LINK_NMP, netinput_enqueue,"netinput_enqueue");

    reg_add_local_lnm_ringno(local_lnm_ringno, "local_lnm_ringno");

    reg_add_local_lnm_vr_bridge(local_lnm_vr_bridge, "local_lnm_vr_bridge");

    /*
     * Set up the parser stuff
     */
    local_lnm_parser_init();
}

#define LOCAL_LNM_MAJVERSION   1
#define LOCAL_LNM_MINVERSION   0
#define LOCAL_LNM_EDITVERSION  1

SUBSYS_HEADER (local_lnm,
 	       LOCAL_LNM_MAJVERSION, LOCAL_LNM_MINVERSION,
 	       LOCAL_LNM_EDITVERSION,
 	       local_lnm_subsys_init, SUBSYS_CLASS_DRIVER,
 	       NULL,
 	       NULL);
 
