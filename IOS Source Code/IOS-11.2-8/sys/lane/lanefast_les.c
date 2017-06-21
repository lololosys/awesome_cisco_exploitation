 /* $Id: lanefast_les.c,v 3.2.62.3 1996/08/27 02:07:38 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lanefast_les.c,v $
 *------------------------------------------------------------------
 * LANE fastswitching code for low-end
 *
 * July, 1995       Steve Jay
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lanefast_les.c,v $
 * Revision 3.2.62.3  1996/08/27  02:07:38  rbadri
 * CSCdi67231:  Topology change messages hinder LANE client debugging
 * Branch: California_branch
 * Dispose Topology change messages at fast path, if there is no
 * change or if it is from ourself.
 *
 * Revision 3.2.62.2  1996/05/09  14:32:43  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.62.1.4.1  1996/04/27  07:06:36  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.62.1  1996/04/17  13:42:46  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.2.84.2  1996/04/09  02:16:30  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.2.84.1  1996/04/08  14:51:53  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.82.1  1996/04/08  08:06:36  ddecapit
 * Sync to LE_Syn_Cal_V111_1_3_branch
 * Branch: TRlane_LE_Syn_1_3_branch
 *
 * Revision 3.2.78.1  1996/04/06  21:20:07  ddecapit
 * Sync to LE_Cal_V111_1_0_3_branch
 * Branch: TRlane_branch_1_0_3
 *
 * Revision 3.2.16.4  1996/03/27  18:01:44  kjadams
 * Branch: TRlane_branch
 * Fix the check for counting unicast forward frames for TR.
 *
 * Revision 3.2.16.3  1996/03/24  08:13:09  ddecapit
 * Branch: TRlane_branch
 *
 *
 * Fix compilation errors with regards to lane_data_t
 *
 * Revision 3.2.16.2  1996/03/21  03:26:09  ddecapit
 * Branch: TRlane_branch
 *
 * RSP Support
 * Code Review Comments
 *
 * Revision 3.2.16.1  1996/03/05  04:47:05  ddecapit
 * Branch: TRlane_branch
 * Token Ring B-LLI for LAN Emulation
 * Fix copyright
 *
 * Revision 3.2  1995/11/17  17:50:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:12:26  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/26  22:06:00  shj
 * Placeholders for LANE fastswitching on low-end.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "mgd_timers.h"

#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../atm/atmsig_api.h"
#include "../atm/ilmi_public.h"

#include "lane.h"
#include "lane_private.h"
#include "lec_private.h"
#include "lane_signal.h"
#include "lane_cfgacc.h"
#include "lsv_private.h"
#include "lanefast_les.h"


static inline void
lane_reflect_inline (paktype *pak, vcd_t vcd)
{
    atm_encap_t         *hdr = (atm_encap_t *)pak->datagramstart;
    hwidbtype           *output = pak->if_input->hwptr;

    hdr->vcnum = pak->atm_vcid =  vcd;
    hdr->enctype = pak->atm_enctype = ATM_DM_PKT_AAL5 | ATM_DM_EOM_EN;
    pak->acct_proto = ACCT_PROTO_OTHER;

    /* output is same as input hwidb - reflect is always on same interface */

    pak->if_output = pak->if_input;
    (*output->fastsend)(output, pak);
    
} 


static boolean
lane_les_aip_fs (paktype *pak)
{
    vc_info_t           *vc_info = pak->atm_vc;
    lane_info_t         *lane_info = vc_info->swidb->lane_info;
    lsv_info_t          *lsv_info;
    union {
        lane_data_hdr_t     *data_hdr;
        lane_ctl_hdr_t      *ctl_hdr;
        lane_arp_pdu_t      *le_arp;
    } hdr;

    if (!lane_info)
	return (FALSE);
    lsv_info = lane_info->lsv_info;
    hdr.data_hdr = (lane_data_hdr_t *)(pak->datagramstart +
                                       ATM_ENCAP_HDRBYTES);

    switch(vc_info->user_vc_type) {
    case LANE_BUS_SEND:
    case LANE_BUS_TR_SEND:
        if (!lsv_info) {
            datagram_done(pak);
            return TRUE;
        }
        /*
         * For all packets arriving at BUS on multicast send vcc, reflect
         * out on multicast forward vcc.  This implements a very dumb BUS.
         * If a smart BUS is ever implemented, this code must change.
         *
         * Count BUS data packets sent to unicast MAC, because too many
         * of these could mean some client is abusing the BUS.
         *
         * For tokenring - must check for unicast address AND not an ri
         * broadcast (explorer) to count is as a Unicast frame
         */
        if (!IS_LANE_CTL(GETSHORT(&hdr.data_hdr->lec_id))) {
	    switch(lane_info->type) {
	    case(LANE_TYPE_802_3): 
                if (IS_ETHER_UNICAST(hdr.data_hdr->lan_type.ether.daddr))
		    lsv_info->buss.busUnicastForwards++;
                break;
 
	    case(LANE_TYPE_802_5):
                if (IS_TRING_UNICAST(hdr.data_hdr->lan_type.tring.daddr) && 
                    (!IS_TRING_RI_BCAST(hdr.data_hdr->lan_type.tring.saddr,
					hdr.data_hdr->lan_type.tring.data[0])))
		    lsv_info->buss.busUnicastForwards++;
                break;
	    default:
                crashdump(0);
	    }
        }
        lsv_info->buss.busBytes += pak->datagramsize;
        lsv_info->buss.busInFrames++;
        lsv_info->buss.busOutFrames++;

        lane_reflect_inline(pak, lsv_info->forward_vcc.vcd);
        return TRUE;

    case LANE_LES_DIRECT:
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
                switch(lane_info->type) { 
		case(LANE_TYPE_802_3):
		    if (IS_ETHER_UNICAST(hdr.le_arp->target_dest.addr.mac)) {
                        lsv_info->lsvs.lsvFastFwdArpReqs++;
                        lane_reflect_inline(pak, lsv_info->distribute_vcc.vcd);
                        return TRUE;
		    } else {
                        return FALSE;
		    }
		    break;

		case(LANE_TYPE_802_5):
		    if (((hdr.le_arp->target_dest.tag == LANE_TAG_MAC_ADDRESS) && 
			 IS_TRING_UNICAST(hdr.le_arp->target_dest.addr.mac)) ||
			(hdr.le_arp->target_dest.tag == LANE_TAG_ROUTE_DESC)) {
                        lsv_info->lsvs.lsvFastFwdArpReqs++;
                        lane_reflect_inline(pak, lsv_info->distribute_vcc.vcd);
                        return TRUE;
		    } else { 
                        return FALSE;
		    }
		    break;
		default:
		    crashdump(0);
		    return FALSE;
                }

            case LANE_ARP_RSP:
                lsv_info->lsvs.lsvFastFwdArpResps++;
                lane_reflect_inline(pak, lsv_info->distribute_vcc.vcd);
                return TRUE;

            case LANE_FLUSH_RSP:
                lsv_info->lsvs.lsvFastFwdFlushResps++;
                lane_reflect_inline(pak, lsv_info->distribute_vcc.vcd);
                return TRUE;
	    case LANE_TOPO_CHANGE:
                lsv_info->lsvs.lsvOutTopReqs++;
		lane_reflect_inline(pak, lsv_info->distribute_vcc.vcd);
		return TRUE;
            default:
                /* other opcodes go to LES process */
                return FALSE;
            }
                    
        } else {

            /* dump data packet on control vcc. */
            datagram_done(pak);
            return TRUE;

        }
        break;
    case LANE_LEC_DISTRIBUTE:
	/*
	 * Handle Topology change messages at the fast level
	 */
        if (IS_LANE_CTL(GETSHORT(&hdr.ctl_hdr->marker))) {
           if ((GETSHORT(&hdr.ctl_hdr->opcode) == LANE_TOPO_CHANGE) &&
	       is_atm_lane(vc_info->swidb)) {
	       lec_info_t *client;
	       ushort flags;
	       client = lane_info->lec_info;

	       /*
		* Toss away the packets from ourself
		*/
	       if (GETSHORT(&hdr.ctl_hdr->requestor_lecid) == client->lecid) {
		   datagram_done(pak);
		   return TRUE;
	       }
	       /*
		* Pass the message to process level, if the flag in the
		* message and the topology_chage flag in lec_info are
		* different.
		*/
	       if (hdr.ctl_hdr->flags & LANE_PROT_FLAG_TOPO_CHANGE) {
		   flags = TRUE;
	       } else {
		   flags = FALSE;
	       }

	       if (flags != client->C19_topology_change) {
		   return FALSE;
	       } else {
		   datagram_done(pak);
		   return TRUE;
	       }
	   }
       } 
        return FALSE;

    default:
        /* not a user_vc_type we care about. */
        return FALSE;
    
    }  

}

static void
lane_les_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    reg_add_aip_fs(RXTYPE_LANE_CONTROL,
			 (service_aip_fs_type)lane_les_aip_fs,
			 "lane_les_aip_fs");
}

void lane_les_fs_init (subsystype* subsys)
{
    reg_add_register_drivers(lane_les_fs_register_drivers,
			     "lane_les_fs_register_drivers");
}


/*
 * LANE LES FS subsystem header
 */

#define LANELESFS_MAJVERSION 1
#define LANELESFS_MINVERSION 0
#define LANELESFS_EDITVERSION  1

SUBSYS_HEADER(lane_les_fs,
              LANELESFS_MAJVERSION, LANELESFS_MINVERSION,
              LANELESFS_EDITVERSION,
	      lane_les_fs_init, SUBSYS_CLASS_PROTOCOL,
              "req: fastswitch",
              NULL);

