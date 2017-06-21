/* $Id: alc_lec.c,v 3.2.62.7 1996/07/31 16:07:10 bbenson Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/alc_lec.c,v $
 *------------------------------------------------------------------
 * Synergy specific LANE Client routines
 *
 * May 1995, Brad Benson
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: alc_lec.c,v $
 * Revision 3.2.62.7  1996/07/31  16:07:10  bbenson
 * CSCdi45072:  Restarting a LEC causes it to lose VLAN identity
 * Branch: California_branch
 *
 * Revision 3.2.62.6  1996/07/02  23:44:23  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Incorporated CLI change requests for PVC config and throttle config.
 *
 * Revision 3.2.62.5  1996/06/18  16:43:38  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 3.2.62.4  1996/06/06  18:40:35  michellw
 * CSCdi59652:  make generic function for creating/deleting LEC
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/05/30  21:33:44  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.2.62.2  1996/05/17  23:27:44  rbadri
 * CSCdi56574:  LANE clients go down/up during 500 station multiping
 * Branch: California_branch
 * Be generous about giving up the CPU, while processing SAR messages.
 *
 * Revision 3.2.62.1  1996/05/09  14:40:59  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:25:13  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.5  1996/04/17  01:18:26  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * lec_find_vc now takes an extra parameter to indicate cache update.
 *
 * Revision 3.2.82.4  1996/04/15  21:21:37  sakumar
 * Support for VTP on NMP creating/destroying LECs on Cat5K ATM Module.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.82.3  1996/04/11  05:31:15  rlowe
 * Add LCP driver and upcall support for VTP messages (over SCP).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.82.2  1996/04/08  14:49:53  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.82.1  1996/04/08  02:08:46  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.5  1996/04/06  02:54:49  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * lec_find_vc should include the MAC address of the source initiating
 * the LE_ARP.
 *
 * Revision 3.2.68.4  1996/04/03  21:54:39  sakumar
 * Call to lec_earl_set_mac2vc() is now part of setting vc state to active.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.68.3  1996/03/30  02:17:10  sakumar
 * Changes to support sending of FLUSH message on Cat5K ATM LANE module.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.68.2  1996/03/22  23:40:07  rlowe
 * Give relative paths to wbu include files.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.68.1  1996/03/22  09:43:32  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.8  1996/03/20  10:48:45  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Also fold old Synergy atmtest.c content into vcdtest.c
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.52.7  1996/03/20  03:12:00  sakumar
 * Apply ATM line card 2.1 fixes to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.52.6  1996/03/14  01:40:48  sakumar
 * Roll in 2.1 bug-fix: CSCdi49930:  LEC going down can leave stale info
 * in TSAR CAM so traffic on BUS
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.52.5  1996/03/14  00:33:12  rlowe
 * Fix registry setup.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.52.4  1996/03/14  00:10:47  rbadri
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 * Add stub lec_bridge_init() for platform dependent initialization.
 *
 * Revision 3.2.52.3  1996/03/11  08:10:23  rlowe
 * More LANE merger fun and games.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.52.2  1996/03/08  01:14:02  rbadri
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 * Fix Synergy/Router conflicts for LANE.
 *
 * Revision 3.2.52.1  1996/03/03  21:24:46  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.50.1  1996/03/02  01:20:58  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.34.1  1996/02/29  07:50:08  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  18:42:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:24:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.66.1  1996/02/03  07:08:25  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.48.12.2.2  1996/01/20  08:03:32  sgarcia
 * CSCdi47232:  sync 11.1 branch to head of 11.0 branch
 * Branch: Synalc_catchup_branch
 * Sync to Synalc2_LE2_V110_1_0_3_branch_end_of_life.
 *
 * Revision 2.1.48.12.2.1  1996/01/19  21:57:25  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.1.48.16  1995/12/01  15:55:09  bbenson
 * CSCdi44807:  lec ipc and sar queues should be init with no limit
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.48.15  1995/11/29  00:47:23  sakumar
 * Fix for in certain cases where LANE could incorrectly set the TSAR's CAM
 * record pointer eventually resulting in certain destination MACs always
 * going on the BUS VC.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.48.14  1995/11/08  17:36:18  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Feed all spanning tree messages into LANE instead of just PORTBLOCK and
 * PORTUNBLOCK.
 *
 * Revision 2.1.48.13  1995/11/08  02:48:11  rlowe
 * Change to use LEIPC_PORT_STATE_CHANGE_IND for NMP spanning tree
 * messages.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.48.12  1995/10/26  02:39:18  bbenson
 * CSCdi42160:  ATM port in blocking state responds to LE-ARPs
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * - Don't respond to LE-ARP requests when the port state is BLOCKED.
 * - Roll in fix CSCdi39461 from the mainline to fix infinite loop of the
 *   config request.
 *
 * Revision 2.1.48.11  1995/10/16  23:46:05  sakumar
 * Added support for Topology Change and LES, BUS, LECS implemented
 * on the Catalyst 5000 ATM Module with RSAR->TSAR firmware
 * implementation of BUS.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.48.10  1995/09/27  23:26:43  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * - Further buff and polish of setting ACTIVE SAR state.
 * - Moving Topology change, SNMP, proxy, and flush code into Synergy.
 *
 * Revision 2.1.48.9  1995/09/25  15:16:08  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Restructure part I.  Remove any reference to vlanid in registry calls.
 *
 * Revision 2.1.48.8  1995/09/21  21:50:19  sakumar
 * Added registry call specific to the Catalyst 5000 ATM LAN Emulation
 * Module to ask the Transmit SAR to send up packets after the MAC
 * address has been resolved in order to continue to drive the LEC
 * state machine.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.48.7  1995/09/20  21:42:34  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * - Call lec_clear_mac instead of bridge_set_mac2vc whenever clearing the
 *   le_arp entry.
 * - Set the earl's mac entry only when the le-arp state is DIRECT and the
 *   call ready state is CALL_READY.
 *
 * Revision 2.1.48.6  1995/09/15  05:27:02  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Close some conditions where the MAC is not cleared out of the CAM after
 * the data direct VC is established.  Loads of belts and suspenders.
 *
 * Revision 2.1.48.5  1995/09/13  05:12:47  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Call datagram_done for IPC packs received from the EARL.
 *
 * Revision 2.1.48.4  1995/09/13  01:43:28  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Use p_unqueue instead of remqueue.
 *
 * Revision 2.1.48.3  1995/09/12  22:24:21  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * - buffer leak between the client and the EARL during LE-ARP lookups.
 *
 * Revision 2.1.48.2  1995/09/12  08:27:55  rlowe
 * control_pak (lec_private.h) is now a union member instead of a
 * separate field.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.48.1  1995/09/11  01:57:03  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.32.3  1995/09/02  01:56:41  bbenson
 * Branch: Synalc2_branch
 * Sync in LANE for the new improved Synalc branch.
 *
 * Revision 2.1.32.2  1995/08/31  14:05:11  rlowe
 * Carryover useful code from old Synergy_ATMlc_branch (10.3) to new
 * Synalc2_branch (11.0).
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1.32.1  1995/08/22  17:51:43  lcheung
 * move file from Synergy_atmlc_branch to Synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  22:54:18  hampton
 * Bump version numbers from 1.x to 2.x.
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
#include "sys_registry.h"
#include "parser.h"


#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "../atm/atmsig_api.h"
#include "../atm/atmsig_public.h"
#include "../atm/ilmi_api.h"
#include "../srt/span.h"

#include "../lane/lane.h"
#include "../lane/lane_private.h"
#include "../lane/lec_private.h"
#include "../lane/lane_signal.h"
#include "../lane/lane_cfgacc.h"
#include "../lane/lsv_private.h"

#include "../lane/lec_fsm.h"
#include "../lane/lec_pdu.h"
#include "../lane/lane_registry.h"

#include "../../wbu/cat5k/alc_cominc/cam_if.h"
#include "../../wbu/cat5k/alc_cominc/lec_if.h"
#include "../../wbu/cat5k/sysif/lec_ipc.h"
#include "../../wbu/cat5k/alc_cominc/sar_if.h" 
#include "../../wbu/cat5k/alc_cominc/atmdrv_if.h" 
#include "../../wbu/cat5k/alc_include/lcp_api.h"

#include "alc_lec.h"
#include "../../wbu/cat5k/alc_cominc/spram.h"
#include  "ios_atmdriver.h"

extern		void Syn_vtp_helper_init(void);
extern      void  ADVR_RegLearpUpCall(void (*)(void *msg));
void lec_earl_set_mac2vc (lec_info_t *lec_info, vcd_t vcd, uchar *mac);



/*
 * lec_sar_enqueue
 *
 * Synergy platform specific enqueue routine that the SAR code calls
 * to enqueue SAR messages to the LEC. 
 */
void lec_sar_enqueue (void *msg)
{
    paktype *pak;

    /*
     * Allocate and copy the parameters into a new record that will be
     * enqueued.
     */
    if (lec_inputQ_enabled) {
	pak = getbuffer(sizeof(tLEARP_PARAM));
	if (pak) {
	    bcopy(msg, (pak->datagramstart), sizeof(tLEARP_PARAM));
	    pak->datagramsize = sizeof(tLEARP_PARAM);
	    process_enqueue_pak(lec_bridge_inputQ, pak);
	}
    }
}



/*
 * lec_cam_clear_entry
 */
void lec_cam_clear_entry (lec_info_t *lec_info, 
			  void *cam_entry,
			  uchar *dst_mac)
{
    return((void)cam_clear_entry(cam_entry, 
				 (tpMACADDR)dst_mac, 
				 lec_info->vlanid));
}


/*
 * lec_cam_set_counter
 */
void lec_cam_set_counter (void *cam_entry)
{
    return((void)cam_set_counter(cam_entry, 2));
}


/*
 * lec_cam_set_state
 *
 * Called by the LEC to tell the CAM which state to change a MAC entry to.
 * Note that we ignore the return of the call, which usually is the
 * pointer to the cam entry.
 */
void lec_cam_set_state (void *lec_info,
                        uchar *dst_mac,
                        ushort vcd,
                        ushort state)
{
    UINT32 tsar_cam_state;

     
    switch(state) {
        case LEC_MAC_FLOOD:
        case LEC_MAC_RESOLVED:
          tsar_cam_state = LE_ARP_IN_PROGRESS_C;
          break;
        case LEC_MAC_FLUSH:
          tsar_cam_state = HOLDING_STATE_C;
          break;
        case LEC_MAC_DIRECT:
          tsar_cam_state = VC_ACTIVE_C;
	  lec_earl_set_mac2vc ((lec_info_t *)lec_info, (vcd_t)vcd, dst_mac);
          break;
        default:
          /*
	   * Return, if you don't recognize the MAC state.
	   */
	  return;
    }
    return((void)cam_set_state( (tpMACADDR)dst_mac, 
                                ((lec_info_t *)lec_info)->vlanid,
                                tsar_cam_state, 
                                vcd));
}

boolean lec_cam_entry_exists (void *lec_info,
	    		      uchar      *mac)
{
	 /*
	  * Since active cam entries are deleted by the TSAR
          * this look up does not apply to cat5K
          */
          return TRUE;
}

boolean lec_cam_check_flooding (void *lec_info,
	    			uchar      *mac)
{
      UINT32 state = 0;
      if (NULLP != cam_get_state((tpMACADDR)mac, ((lec_info_t *)lec_info)->vlanid, &state) &&
          state == LE_ARP_IN_PROGRESS_C) {
          return TRUE;
      } 
      return FALSE;
}

boolean lec_cam_check_ready (void *lec_info,
	    			uchar      *mac)
{
      UINT32 state = 0;
      if (NULLP == cam_get_state((tpMACADDR)mac, ((lec_info_t *)lec_info)->vlanid, &state)) {
          /* 
           * If entry does not exist, then it means it is active becos the
           * TSAR deletes active CAM entries 
           */
          return TRUE;
      }
      /* Or if TSAR has not deleted the CAM entry yet */
      if (state == VC_ACTIVE_C) {
          return TRUE;
      } 
      return FALSE;
}

/*
 * lec_earl_set_mac2vc
 *
 * Send a message to the EARL telling it mac to vc mapping.
 */
void lec_earl_set_mac2vc (lec_info_t *lec_info, vcd_t vcd, uchar *mac)
{
    leipc_send_earl_req(0, 
			LEIPC_SET_MAC_REQ, 
			(tpMACADDR)mac, 
			lec_info->vlanid, 
			vcd);
}


/*
 * lec_earl_clear_mac
 *
 * Send a message to the EARL to clear a mac entry.
 */
void lec_earl_clear_mac (lec_info_t *lec_info, uchar *mac)
{
    leipc_send_earl_req(0, 
			LEIPC_CLEAR_MAC_REQ, 
			(tpMACADDR)mac, 
			lec_info->vlanid,
			0);
}


/*
 * lec_earl_delete_vc
 *
 */
void lec_earl_delete_vc (lec_info_t *lec_info, vcd_t vcd)
{
    leipc_send_earl_req(0, LEIPC_REMOVE_MACS_ON_VCD, 0, lec_info->vlanid, vcd);
}


/*
 * lec_earl_lookup_mac
 *
 * Send a message to the EARL requesting mac information.  Check the
 * bridge_lookup_holdQ to see if we already have an outstanding
 * request for the particular MAC lookup.  If not, duplicate the pak
 * and stow the copy away on the queue for the response.
 *
 * The parameter for this is lane_info because this function is
 * registered.
 */
void lec_earl_lookup_mac (lec_info_t *lec_info)
{
    lane_ctl_msg_u	*msg;
    lane_ctl_msg_u	*holdQ_entry;
    paktype		*pak;

    msg = (lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart;

    leipc_send_earl_req(0, 
			LEIPC_GET_MAC_REQ, 
			(tpMACADDR)msg->arp.target_dest.addr.mac, 
			lec_info->vlanid, 
			0);

    pak = (paktype *)lec_info->bridge_lookup_holdQ.qhead;

    while (pak) {
	holdQ_entry = (lane_ctl_msg_u *)pak->datagramstart;
	if (!memcmp(msg->arp.target_dest.addr.mac,
		    holdQ_entry->arp.target_dest.addr.mac,
		    STATIONLEN_IEEE48)) {
	    return;
	}
	pak = pak->next;
    }

    /*
     * If we don't already have an outstanding request for this
     * MAC/vlanid out to the EARL, save the LE_ARP_REQ pak in the holdq
     * for later use in the LE_ARP_RSP.  NULLing out the
     * lec_info->data pointer will prevent the process_messages routine
     * from deallocating the pak.
     */
    pak = lec_info->data.control_pak;
    p_enqueue(&lec_info->bridge_lookup_holdQ, pak);
    lec_info->data.control_pak = NULL;
}


/*
 * lec_bridge_lookup_mac
 *
 * A registry function called by client state machine to respond
 * to LE_ARP requests. Always return FALSE, as the look up response
 * is asynchronous from Earl.
 */
boolean lec_bridge_lookup_mac (lec_info_t *lec_info)
{
    lec_earl_lookup_mac(lec_info);
    return(FALSE);
}
 

/*
 * lec_port_destroy
 */
void lec_port_destroy (lec_info_t *lec_info)
{
    leipc_send_spt_req(0, LEIPC_PORT_DESTROY_REQ, lec_info->vlanid);

    /* Now inform TX SAR that this VLAN is no longer active */
    acp_pTxSpram->LecTable[lec_info->vlanid].lece_lecid = 0;
    acp_pTxSpram->LecTable[lec_info->vlanid].lece_busvpc = 0;

}

/*
 * lec_port_create
 */
void lec_port_create (lec_info_t *lec_info)
{
    leipc_send_spt_req(0, LEIPC_PORT_CREATE_REQ, lec_info->vlanid);
}


/*
 * lec_bind_vlan_to_vc
 * 
 * Wrapper function to call into the TXSAR interface defined in atmdrv_if.h
 * Associate VLAN with non-bus data VC.
 */
void lec_bind_vlan_to_vc (lec_info_t *lec_info, ushort vcd)
{
    aalsap_bind_vlan_to_vc(lec_info->swidb->hwptr->unit, 
			   vcd, 
			   lec_info->vlanid, VPC_DEST_BIGA);
}


/*
 * lec_map_bus_vcd
 * 
 * Wrapper function to call the TXSAR interface defined in
 * atmdrv_if.h.  Maps the incoming BUS VC to the outgoing BUS VC.
 */
void lec_map_bus_vcds (lec_info_t *lec_info)
{
    aalsap_lec_map_bus_vcds(lec_info->swidb->hwptr->unit,
			lec_info->mcast_forward.vcd,
			lec_info->mcast_send.vcd,
			lec_info->vlanid,
			lec_info->lecid);
    /* also for BUS's that send on the point-to-point to LEC */
    aalsap_lec_map_bus_vcds(lec_info->swidb->hwptr->unit,
			lec_info->mcast_send.vcd,
			lec_info->mcast_send.vcd,
			lec_info->vlanid,
			lec_info->lecid);
}


/*
 * bus_map_bus_vcd
 *
 * Wrapper function to call the TXSAR interface defined in
 * atmdrv_if.h.  Maps the incoming TO BUS VC to the outgoing FROM BUS VC.
 */
void bus_map_bus_vcds (lsv_client_t *lsv_client)
{
    aalsap_bus_map_bus_vcds(0,
        lsv_client->send_vcc.vcd,       /* Incoming VCD To BUS From Network */
        lsv_client->forward_vcc.vcd);   /* Outgoing VCD From BUS To Network */
}


/*
 * lec_change_vcd_clbit
 *
 * Wrapper function to clear the CL BIT for RSAR BUS VC during
 * topology change.
 */
void lec_change_vcd_clbit (lec_info_t *lec_info, boolean clbit)
{
    aalsap_change_vcd_clbit(lec_info->swidb->hwptr->unit,
            lec_info->mcast_forward.vcd,
            lec_info->mcast_send.vcd,
            lec_info->vlanid,
            (ushort) clbit);
}


/*
 * lec_process_ipc_messages
 *
 * Process IPC messages that have come from the EARL.  Note that the
 * messages are in pak buffers.
 */
void lec_process_ipc_messages (void)
{
    int			count;
    paktype		*pak;
    tLEC_IPCMSG		*ipc_msg;
    lec_info_t		*lec_info;
    lec_le_arp_t	*le_arp;
    lane_ctl_msg_u	*holdQ_arp_req;
    paktype		*holdQ_pak;

    count = PAK_SWITCHCOUNT;
    while (TRUE) {
        /*
         * Process only a limited number of incoming packets 
	 * per invocation.
         */
        if (--count == 0) {
            break;
        }

	pak = process_dequeue(lec_ipc_inputQ);
	if (!pak) {
	    break;
	}

	ipc_msg = (tpLEC_IPCMSG)(pak->datagramstart + sizeof(IBIPC_PKT_HDR_T));
	switch (ipc_msg->leipc_msgtype) {
	case LEIPC_SET_MAC_RESP:
	    lec_info = lec_info_from_vlanid(ipc_msg->param.earl_msg.color);

	    if (lec_info && (ipc_msg->param.earl_msg.errcode == EARLIPC_OK)) {

		le_arp = lec_lookup_le_arp(lec_info, 
					   ipc_msg->param.earl_msg.macaddr);
		if (le_arp) {
		    reg_invoke_cam_clear_entry(lec_info,
					       le_arp->cam_entry,
					       le_arp->mac);
		}
	    }
	    break;
	case LEIPC_GET_MAC_RESP:
	    lec_info = lec_info_from_vlanid(ipc_msg->param.earl_msg.color);

	    if (lec_info) {

		holdQ_pak = (paktype *)lec_info->bridge_lookup_holdQ.qhead;

		/*
		 * Search for the pak in the holdQ which matches the
		 * MAC address of the response.  Use it to send the
		 * response back.
		 */
		while (holdQ_pak) {
		    holdQ_arp_req = (lane_ctl_msg_u *)holdQ_pak->datagramstart;
		    if (!memcmp(ipc_msg->param.earl_msg.macaddr,
				holdQ_arp_req->arp.target_dest.addr.mac,
				STATIONLEN_IEEE48)) {
			break;
		    }
		    holdQ_pak = holdQ_pak->next;
		}

		if (holdQ_pak) {
		    p_unqueue(&lec_info->bridge_lookup_holdQ, holdQ_pak);
		    if (ipc_msg->param.earl_msg.errcode == EARLIPC_OK) {
			lec_info->data.control_pak = holdQ_pak;
			lec_send_arp_rsp(lec_info, LEC_REMOTE_ADDR);
			lec_info->data.control_pak = NULL;
		    }
		    datagram_done(holdQ_pak);
		}
	    }
	    break;
	case LEIPC_PORT_STATE_CHANGE_IND:
	    lec_info = lec_info_from_vlanid(ipc_msg->param.spanning_msg.port_color);
	    if (lec_info) {
		/*
		 * Note that the default is to do nothing...if a state
		 * comes in that we don't know about what are you
		 * supposed to do?
		 */
		switch (ipc_msg->param.spanning_msg.port_state) {
		case LEC_PORTUNKNOWN:
		case LEC_PORTBLOCK:
		case LEC_PORTDISABLD:
		case LEC_PORTLISTEN:
		case LEC_PORTLEARN:
		    lec_info->swidb->port_state = PORT_BLOCKING;
		    break;
		case LEC_PORTUNBLOCK:
		    lec_info->swidb->port_state = PORT_FORWARDING;
		    break;
		}
	    }
	    break;
	case LEIPC_TOPCHANGE_IND:
	    lec_info = lec_info_from_vlanid(ipc_msg->param.TopChange_msg.Vlan);
	    if (lec_info) {
		reg_invoke_lec_send_topo_change(lec_info,
						(boolean)ipc_msg->param.TopChange_msg.TopologyState);
	    }
	    break;
	}

	datagram_done(pak);
    }
}


/*
 * lec_process_sar_messages
 *
 * Process messages that have come up from the SAR.  Note that the
 * messages are copied into pak buffers.
 */
void lec_process_sar_messages (void)
{
    int			count;
    vcd_t		vcd;
    lec_info_t		*lec_info;
    lec_nsap_info_t     *lec_nsap_info;
    lec_le_arp_t	*le_arp;
    tLEARP_PARAM	*msg;
    paktype		*pak;
    boolean		cache_ok;

    count = PAK_SWITCHCOUNT;
    while (TRUE) {
        /*
         * Process only a limited number of incoming packets 
	 * per invocation.
         */
        if (--count == 0) {
            break;
        }

	pak = process_dequeue(lec_bridge_inputQ);
	if (!pak) {
	    break;
	}

	msg = (tpLEARP_PARAM) (pak->datagramstart);

	switch (msg->opcode) {
	case TXSAR_START_LEARP_IND_C:
	    lec_info = lec_info_from_vlanid(msg->learpp_vlanid);
	    if (lec_info) {

		le_arp = lec_lookup_le_arp(lec_info, 
					   msg->learpp_dmacaddr.maddr);
		if (le_arp) {
		    le_arp->cam_entry = msg->learp_came;
                    if (lec_info->oper_status == LANE_INACTIVE) {
                        reg_invoke_cam_clear_entry(lec_info,
						   le_arp->cam_entry,
						   le_arp->mac);
                    }
		}

		lec_find_vc(lec_info->lane_info, 
			    msg->learpp_dmacaddr.maddr,
			    msg->learpp_smacaddr.maddr,
			    &vcd, 
			    &cache_ok);
		/*
		 * Could have freshly allocated an le_arp entry in the
		 * lec_find_vc call. 
		 */
		le_arp = lec_lookup_le_arp(lec_info, 
					   msg->learpp_dmacaddr.maddr);
		if (le_arp) {
		    le_arp->cam_entry = msg->learp_came;
		    lec_nsap_info = lec_lookup_nsap(lec_info, &le_arp->nsap);
		    /*
		     * If we're still getting queue'd up LEARP
		     * requests from the cam, make certain that the
		     * CAM is cleared out.
		     */
		    if (lec_nsap_info &&
			(le_arp->state == LEC_MAC_DIRECT) &&
			(lec_nsap_info->ready_state == CALL_READY)) {
			reg_invoke_tbridge_lane_bte_update(lec_info,
                                                 le_arp->mac,
						 VC_ACTIVE_C,
						 vcd);
		    }
		}
	    }
	    break;
	case TXSAR_CLEAR_MAC_IND_C:
	    lec_info = lec_info_from_vlanid(msg->learpp_vlanid);
	    if (lec_info) {
		reg_invoke_lec_clear_mac(lec_info,
					 msg->learpp_dmacaddr.maddr);
		lec_find_vc(lec_info->lane_info, 
			    msg->learpp_dmacaddr.maddr,
			    msg->learpp_smacaddr.maddr,
			    &vcd, 
			    &cache_ok);
		le_arp = lec_lookup_le_arp(lec_info, 
					   msg->learpp_dmacaddr.maddr);
		if (le_arp) {
		    le_arp->cam_entry = msg->learp_came;
		}
	    }
	    break;
	default:
	    break;
	}
	datagram_done(pak);
    }
}


/*
 * lec_process_bridge_messages
 *
 * This is the Synergy version of process_bridge_messages.  As opposed
 * to the router version, is has to check both queues from the EARL an
 * the queue from the TXSAR.
 */
void lec_process_bridge_messages (void)
{
    lec_process_sar_messages();
    lec_process_ipc_messages();
}


/*
 * lec_bridging_enabled
 *
 * Since we're on Synergy, this is *always* TRUE.
 */
boolean lec_bridging_enabled (idbtype *swidb)
{
    return(TRUE);
}


/*
 * lec_cfg_vlanid
 *
 * Synergy specific function to check/configure the vlanid for the
 * client.  Note that we always return TRUE, so that the parser code
 * can determine if the registry is present.
 */
boolean lec_cfg_vlanid (idbtype *swidb, int vlanid)
{
    hwidbtype	*hwidb;
    idbtype	*tmp_swidb;
    lec_info_t	*lec_info;
    boolean	found = FALSE;

    hwidb = hwidb_or_null(swidb);
    if (!hwidb) {
	return(FALSE);
    }

    /*
     * Search for a previously configured VLANID on this hwidb. 
     * Can't do that.
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, tmp_swidb) {
	lec_info = lec_info_from_swidb(tmp_swidb);
	if ((lec_info->vlanid == vlanid) &&
	    (lec_info->admin_status == LANE_ACTIVE)) {
	    found = TRUE;
	    break;
	}
    }

    /*
     * Reuse lec_info to get the client that's being configured, and
     * set vlanid appropriately.  Also initialize the port state for
     * bridging.
     */
    lec_info = lec_info_from_swidb(swidb);
    if (found) {
	lec_info->vlanid = 0;
	printf("\n%%Duplicate VLANID on this interface");
	return(TRUE);
    } else {
	lec_info->vlanid = vlanid;
	lec_info->swidb->port_state = PORT_BLOCKING;
	return(TRUE);
    }

}


/*
 * synalc_lane_reflect
 */
void synalc_lane_reflect (paktype *pak, 
			  lsv_info_t *lsv_info, 
			  lane_info_t *lane_info)
{
    /* output is same as input hwidb - reflect is always on same interface */
    pak->if_output = pak->if_input;
    if (!lane_control_encap(pak, lsv_info->distribute_vcc.vcd)) {
	buginf("\nLES %s elan %s vcd %d encaps failure",
               lane_info->swidb->namestring,
               lane_info->name, pak->vcd);
        datagram_done(pak);
    } else {
        datagram_out(pak);
    }
} 


/*
 * synalc_les_check_for_reflect
 */
boolean synalc_les_check_for_reflect (paktype *pak)
{
    idbtype		*swidb = pak->if_input;
    lane_info_t         *lane_info = swidb->lane_info;
    lsv_info_t          *lsv_info;
    union {
        lane_ctl_hdr_t      *ctl_hdr;
        lane_arp_pdu_t      *le_arp;
    } hdr;

    lsv_info = lane_info->lsv_info;
    hdr.ctl_hdr = (lane_ctl_hdr_t *)pak->datagramstart;

    if (!lsv_info) {
       datagram_done(pak);
       return TRUE;
    }
    /*
     * For packets arriving at LES on control direct vc:
     * a. If le-arp resp, flush-resp, or le-arp req for other than
     *    mcast addr, count & reflect out control distribute.
     * b. Other control packets to process level.
     * c. Toss data packets.
     */
    if (IS_LANE_CTL(GETSHORT(&hdr.ctl_hdr->marker))) {
        switch(GETSHORT(&hdr.ctl_hdr->opcode)) {
        case LANE_ARP_REQ:
            /*
             * For le-arp's, check requested address.  For multicast
             * addresses, let process level LES handle.  Otherwise,
             * reflect out control distribute for LEC's.
             */
            if (IS_ETHER_UNICAST(hdr.le_arp->target_dest.addr.mac)) {
                lsv_info->lsvs.lsvFastFwdArpReqs++;
		synalc_lane_reflect(pak, lsv_info, lane_info);
                return TRUE;
	    } else {
                return FALSE;
            }

        case LANE_ARP_RSP:
            lsv_info->lsvs.lsvFastFwdArpResps++;
	    synalc_lane_reflect(pak, lsv_info, lane_info);
            return TRUE;

        case LANE_FLUSH_RSP:
            lsv_info->lsvs.lsvFastFwdFlushResps++;
	    synalc_lane_reflect(pak, lsv_info, lane_info);
            return TRUE;

        default:
            /* other opcodes handled by LES process */
            return FALSE;
        }
                    
    }  

    return FALSE;

}


/*
 * lec_alc_init
 *
 * Initialize any Synergy LEC relevant stuff here.
 */
void lec_alc_init (void)
{

    reg_add_bridge_delete_vc(lec_earl_delete_vc,
			      "lec_earl_delete_vc");
    reg_add_lec_bridge_lookup_mac (lec_bridge_lookup_mac, 
				   "lec_earl_get_mac");
    reg_add_bridge_set_mac2vc(lec_earl_set_mac2vc,
			      "lec_earl_set_mac2vc");
    reg_add_cam_clear_entry(lec_cam_clear_entry, 
			    "lec_cam_clear_entry");
    reg_add_tbridge_lane_bte_update(lec_cam_set_state,
			  "lec_cam_set_state");
    reg_add_tbridge_lane_bte_flooding(lec_cam_check_flooding,
			  "lec_cam_check_flooding");
    reg_add_tbridge_lane_bte_ready(lec_cam_check_ready,
			  "lec_cam_check_ready");
    reg_add_tbridge_lane_bte_exists(lec_cam_entry_exists,
			  "lec_cam_entry_exists");
    reg_add_cam_set_counter(lec_cam_set_counter, 
			    "lec_cam_set_counter");
    reg_add_lec_bridging_enabled(lec_bridging_enabled,
				 "lec_bridging_enabled");
    reg_add_lec_bridge_lookup_mac(lec_bridge_lookup_mac,
                                  "lec_bridge_lookup_mac");
    reg_add_lec_process_bridge_messages(lec_process_bridge_messages,
                                        "lec_process_bridge_messages");

    reg_add_lec_clear_mac(lec_earl_clear_mac,
			  "lec_clear_earl_mac");
    reg_add_lec_cfg_vlanid(lec_cfg_vlanid, "lec_cfg_vlanid");
    reg_add_lec_map_bus_vcds(lec_map_bus_vcds,
			     "lec_map_bus_vcds");
    reg_add_lec_bind_vlan_to_vc(lec_bind_vlan_to_vc,
				"lec_bind_vlan_to_vc");
    reg_add_lec_port_create(lec_port_create, "lec_port_create");
    reg_add_lec_port_destroy(lec_port_destroy, "lec_port_destroy");

    reg_add_les_check_for_reflect(synalc_les_check_for_reflect,
				 "synalc_les_check_for_reflect"); 
    reg_add_bus_map_bus_vcds(bus_map_bus_vcds, "bus_map_bus_vcds");
    reg_add_lec_change_vcd_clbit(lec_change_vcd_clbit, 
				 "lec_change_vcd_clbit");

    /*
     * Register the start le arp event handler with the atm driver
     * Ditto for the Set Port State upcall from the LCP driver.
     */
    ADVR_RegLearpUpCall(lec_sar_enqueue);
    LCPl_RegPortStateInd(leipc_portstate_ind) ;
    LCPl_RegVlanParamCmd(vtpipc_vlanparams_cmd) ;
  
    /* Startup the VTP Helper Task */
    Syn_vtp_helper_init();


#ifdef IPCTEST

#else 
    leipc_init();
#endif

}

#ifdef printf
#undef printf
#endif
/*
 * parser commands for VLAN creation/deletion from PVC
 */

/*
 * cat5k_interface_commands:	Called to do all ATM command processing.
 * OUTPUT/RETURN:
 *	None.
 * INPUT:
 *	csb = Ptr. to the Parse block with command & arguments.
 *	csb->which = ATM interface Command type
 *	csb->sense = [NO] value(0 = NO), (1 = !NO)
 */
void
cat5k_interface_commands (parseinfo *csb)
{
    idbtype *idb, *firstsw;
    hwidbtype *hwidb;
    ushort   vlanid;
    ushort   vcd;
    vcd_t    vcnum;
    vc_info_t *vc;
    atm_db_t *atm;
    unsigned int req_speed;
    unsigned int clks_per_cell = 1;
    

    idb = csb->interface;
    vlanid  = 0;

    if (idb) {
	hwidb = idb->hwptr;
    } else {
	printf("\nerror: source idb not set");
	return;
    }
    atm = hwidb->atm_db;
    firstsw = hwidb->firstsw;
/*
 * csb->nvgen == TRUE when the 'write [term || mem || net]' command is
 * entered.  Otherwise, it's false to indicate that a command is being
 * specified by the user.
 */
    if (csb->nvgen) {
	switch(csb->which) {

	case ATM_CMD_BIND_PVC_TO_VLAN:

	    for(vcnum = 1; vcnum < atm->config.max_intf_vcs; vcnum++) {
		if ((vc = atm_getvc_all(atm, vcnum))) {
		    if(IS_ATMVC_SVC(vc) || (vc->flags & VC_FLAG_TEMP))
			continue;
		    if (vc->swidb != idb) /* Only do this SWIDB */
			continue;
		    switch (vc->flags & VC_FLAG_AAL_MASK) {
		    case ATM_VC_AAL5SNAP:
			if ((vlanid = alc_get_vlan_from_PVC(vcnum))) {
			    nv_write (TRUE, "%s %d %d ", csb->nv_command,
				      vcnum, vlanid);
			}
			break;
		    default:
			break;
		    }
		}
	    }
	    break;
	case  ATM_CMD_REMOVE_VLAN:
	    break;
        case ATM_CMD_THROTTLE_TSAR:
	    if (pAtmdrvGcb->throttle_parm != MAX_OC3_MBITS_PER_SEC) {
		nv_write (TRUE, "%s %d", csb->nv_command, 
                          pAtmdrvGcb->throttle_parm);
	    }
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

/*
 * csb->nvgen == FALSE.  Go process user specific command
 */
    vcd = 0;
    switch(csb->which) {
    case ATM_CMD_BIND_PVC_TO_VLAN:
	/* Bind the VC to a VLAN */
	vlanid = GETOBJ(int, 2);
	vcd = GETOBJ(int, 1);
	if (csb->sense) {
	    idb = reg_invoke_find_lec_by_vlanid_on_interface(vlanid, firstsw);
	    if (idb) {
		/*
		 * This vlan id is  bound to an LEC. Go ahead and Delete it.
		 * and then bind the PVC to VLAN
		 */
		lec_delete_client(idb, LANE_CALLER_CLI);
		buginf("Deleting LEC with VLAN ID %d\n", vlanid);
	    }
	    switch (alc_bind_PVC_to_vlan(vlanid, vcd)) {
		case ALC_RET_OK:
		    break;
		case ALC_ERR_BAD_COLOR_RANGE:
	            printf("\natm bind failed: vlan %d out of range ", vlanid); 
		    break;
		case ALC_ERR_PVC_NOT_AAL5SNAP:
	            printf("\natm bind failed: vcd %d not aal5snap", vcd); 
		    break;
		case ALC_ERR_VLAN_ON_LANE:
	            printf("\natm bind failed: vlan %d is configured as LEC", 
                            vlanid); 
		    break;
		case ALC_ERR_PVC_ON_OTHER_VLAN:
	            printf("\natm bind failed: vcd %d already bound to another vlan", vcd);
		    break;
		case ALC_ERR_OTHER:
	            printf("\natm bind failed");
		    break;
                default:
	            printf("\ncat5k_interface_commands: Illegal return code from alc_bind_PVC_to_vlan()");
                    break;
	    }
	} else {
	    switch (alc_unbind_PVC_from_vlan(vlanid, vcd)) {
                case  ALC_RET_OK:
                    break;
		case ALC_ERR_BAD_COLOR_RANGE:
	            printf("\natm unbind failed: vlan %d out of range", vlanid);
                    break;
		case ALC_ERR_VLAN_NOT_PVC_SUPP:
	            printf("\natm unbind failed: vlan %d not bound to PVCs",
                            vlanid);
                    break;
		case ALC_ERR_VLAN_PVC_MISMATCH:
	            printf("\natm unbind failed: vlan %d not bound to PVC %d",
                            vlanid, vcd);
                    break;
                default:
	            printf("\ncat5k_interface_commands: Illegal return code from alc_unbind_PVC_to_vlan()");
                    break;
            }
	}
	break;
    case ATM_CMD_REMOVE_VLAN:
	/* Remove VLAN and all associated PVCs */
	vlanid = GETOBJ(int, 1);
	while (TRUE) {
	    vcd = alc_get_PVC_from_vlan(vlanid, vcd);
	    if (!vcd) {
		break;
	    } else {
		alc_unbind_PVC_from_vlan(vlanid, vcd);
	    }
	}
	break;
    case ATM_CMD_THROTTLE_TSAR:
        if (csb->sense) {
	    req_speed = GETOBJ(int, 1);
        } else {
	    req_speed = MAX_OC3_MBITS_PER_SEC; /* if no throttling */
        }
        pAtmdrvGcb->throttle_parm = req_speed;
	if (req_speed == MAX_OC3_MBITS_PER_SEC) {
	    clks_per_cell = 1; /* This effectively removes throttling */
	} else {
	    /* Note: Clocks per cell at max rate is 141.56, so integer approx*/ 
            clks_per_cell = (14156 * MAX_OC3_MBITS_PER_SEC) / (100 * req_speed);
	}
        aalsap_set_tsar_throttle(clks_per_cell);
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
	
    }
}

static const char *vlan_info = "\tVCD   \tVLAN-ID";

/*
 * cat5k_print_vlan_info
 *
 * Given a vlanid, this function prints all the PVCs associated with
 * the vlan.
 */
void
cat5k_print_vlan_info (ushort vlanid)
{
    ushort vcd;
    boolean header = FALSE;

    vcd = 0;

    while (TRUE) {
	vcd = alc_get_PVC_from_vlan(vlanid, vcd);
	if (vcd) {
	    if (!header) {
		automore_enable(NULL);
		automore_header(vlan_info);
		header = TRUE;
	    }
	    printf("\n\t%d   \t%6d", vcd, vlanid); 
	} else {
	    break;
	}
    }
    
}

void
cat5k_show_commands (parseinfo *csb)
{
    ushort vlanid;
    ushort vcd;
    vlanid = 0;
    vcd = 0;

    switch(csb->which) {
    case SHOW_ATM_VLAN:			/* Show PVCs in this VLAN*/
	vlanid = GETOBJ(int,1);
	/*
	 * When "show atm vlan" is called without the vlanid, all the
	 * available vlans are probed. PVCs associated with each VLAN
	 * will be printed.
	 */
	if (!vlanid) {
	    while (TRUE) {
		vlanid = alc_get_PVC_vlan(vlanid);
		if (vlanid) {
		    cat5k_print_vlan_info(vlanid);
		} else {
		    break;
		}
	    }
	} else {
	    cat5k_print_vlan_info(vlanid);
	}
	automore_disable();
	break;
    default:
	break;
    }

}






