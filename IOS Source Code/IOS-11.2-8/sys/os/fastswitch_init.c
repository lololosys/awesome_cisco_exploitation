/* $Id: fastswitch_init.c,v 3.4.20.9 1996/07/08 22:38:17 mwu Exp $
 * $Source: /release/112/cvs/Xsys/os/fastswitch_init.c,v $
 *------------------------------------------------------------------
 * Fast switching subsystem initialization
 *
 * June 1993, William H. Palmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fastswitch_init.c,v $
 * Revision 3.4.20.9  1996/07/08  22:38:17  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.4.20.8  1996/06/13  18:27:10  fbordona
 * CSCdi60152:  DLSw merge of low-end and RSP fast switch code.
 *              dlsw_srb_madge_input registry needed for hes.
 *              Fix gs7 build problem with permission from token holder.
 * Branch: California_branch
 *
 * Revision 3.4.20.7  1996/06/12  00:16:31  fbordona
 * CSCdi60152:  DLSw merge of low-end and RSP fast switch code for
 *              modularity.
 * Branch: California_branch
 *
 * Revision 3.4.20.6  1996/05/23  01:00:56  hou
 * CSCdi54936:  802.10 routing on fddi + A==B connection (loop) crashes rt.
 * Branch: California_branch
 *
 * Revision 3.4.20.5  1996/05/17  11:34:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.6.2  1996/05/05  23:40:27  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.4.6.1  1996/05/02  22:06:27  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.20.4  1996/05/11  00:48:22  hou
 * CSCdi55994:  Reload occurs after encap sde 1 command issued
 * Branch: California_branch
 * - All vLAN registry functions in fastswitch subsystem were
 *   not initialized.
 *
 * Revision 3.4.20.3  1996/05/04  01:34:33  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR bridging support.
 *
 * Constrained Multicast Flooding
 * - Add the default function return_one.
 * - Initialize the default function for tbridge_cmf to return_one.
 *
 * Name Access List
 *
 * Revision 3.4.20.2  1996/04/17  13:45:32  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/04/16  19:09:56  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.4  1996/01/25  11:20:22  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.3  1995/12/13  04:23:01  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.2  1995/11/17  18:47:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:18:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/21  00:44:27  abaindur
 * CSCdi37336:  Crash during bootup with GRE traffic
 * Ethernet i/f opened up for traffic before the tunnel_ip_les_fs
 * registry service function added. Initialize with default function
 * upfront in fastswitch init.
 * While at it, initialize some other faststub registries that were
 * not initialized.
 *
 * Revision 2.2  1995/07/20  17:55:15  wmay
 * CSCdi37067:  router crashes while fast switching ip over smds fsip to
 * lex.  add mci_testptr copy to lex idb, also default the registry that
 * was crashing.
 *
 * Revision 2.1  1995/06/07  21:54:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "../h/fastswitch_registry.regc"
#include "subsys.h"
#include "packet.h"
#include "../if/network.h"

static void discard_pak_input (paktype* pak)
{
    protocol_discard(pak, TRUE);
}


/*
 * fastswitch_init
 * Routine for filling in the fastswitching jump table
 * with the address of a function that returns FALSE:
 */

static void fastswitch_init(subsystype *subsys)
{
    int proto;
    int level;

    registry_create(REG_FS, SERVICE_FS_MAX, "Fastswitch");
    create_registry_fs();

    level = raise_interrupt_level(NETS_DISABLE);

    create_queue_registry();
    for (proto = 0; proto < RXTYPE_LAST; proto++) {
	reg_add_fast_fragmentation(proto,
			      (service_fast_fragmentation_type)return_false,
				   "return_false");
	reg_add_ether_fs(proto,
			 (service_ether_fs_type)return_false,
			 "return_false");
	reg_add_fddi_fs(proto,
			(service_fddi_fs_type)return_false,
			"return_false");
	reg_add_hdlc_fs(proto,
			(service_hdlc_fs_type)return_false,
			"return_false");
	reg_add_ppp_fs(proto,
		       (service_ppp_fs_type)return_false,
		       "return_false");
	reg_add_ppp_from_process_fs(proto,
				    (service_ppp_from_process_fs_type)
				    return_false,
				    "return_false");
	reg_add_slip_fs(proto,
		       (service_slip_fs_type)return_false,
		       "return_false");
	reg_add_lex_fs(proto,
		       (service_lex_fs_type)return_false,
		       "return_false");
	reg_add_framerelay_fs(proto,
			(service_framerelay_fs_type)return_false,
			"return_false");
	reg_add_smds_fs(proto,
			(service_smds_fs_type)return_false,
			"return_false");
	reg_add_token_fs(proto,
			 (service_token_fs_type)return_false,
			 "return_false");
	reg_add_aip_fs(proto,
			    (service_aip_fs_type)return_false,
			    "return_false");
	reg_add_channel_fs(proto,
			   (service_channel_fs_type)return_false,
			   "return_false");
	reg_add_tunnel_fs(proto,
			    (service_aip_fs_type)return_false,
			    "return_false");
	reg_add_atm_dxi_fs(proto,
			   (service_atm_dxi_fs_type)return_false,
			   "return_false");
	reg_add_fast_translate_before_routing(proto,
		(service_fast_translate_before_routing_type)return_false,
					      "return_false");
	reg_add_fast_translate_after_routing(proto,
		(service_fast_translate_after_routing_type)return_false,
					     "return_false");
    }

    reg_add_write_dot10_encapsulation(
	       (service_write_dot10_encapsulation_type)return_false,
	       "return_false");			  

    reg_add_write_ether_dot10_encapsulation(
		(service_write_ether_dot10_encapsulation_type)return_false,
		"return_false");
 
    reg_add_write_isl_vlan_colour_2_feip(
                (service_write_isl_vlan_colour_2_feip_type)return_false,
                "return_false");
    
    reg_add_tbridge_forward((service_tbridge_forward_type)return_false,
                                "return_false");
    reg_add_tbridge_mci_forward((service_tbridge_mci_forward_type)return_false,
                                "return_false");
    reg_add_tbridge_cmf((service_tbridge_cmf_type)return_one,
			"return_one");
    reg_add_tbridge_fddi_transit_receive(
    		(service_tbridge_fddi_transit_receive_type)return_false,
					 "return_false");
    reg_add_nim_vring_input((service_nim_vring_input_type)return_false,
				"return_false");
    reg_add_tms380_vring_input((service_tms380_vring_input_type)return_false,
				"return_false");
    reg_add_mci_vring_input((service_mci_vring_input_type)return_false,
				"return_false");
    reg_add_mci_rsrb_fst_forward((service_mci_rsrb_fst_forward_type)
				 return_false, "return_false");
    reg_add_nim_rsrb_fst_forward((service_nim_rsrb_fst_forward_type)
				 return_false, "return_false");
    reg_add_vrfst_input(discard_pak_input, "discard_pak_input");

    reg_add_dlsw_srb_input((service_dlsw_srb_input_type)
				 return_false, "return_false");
    reg_add_nim_dlsw_fst_forward((service_nim_dlsw_fst_forward_type)
				 return_false, "return_false");
    reg_add_nim_dlsw_input((service_nim_dlsw_input_type)
				 return_false, "return_false");
    reg_add_dlsw_srb_mci_input((service_dlsw_srb_mci_input_type)
				 return_false, "return_false");
    reg_add_mci_dlsw_fst_forward((service_mci_dlsw_fst_forward_type)
				 return_false, "return_false");
    reg_add_mci_dlsw_input((service_mci_dlsw_input_type)
				 return_false, "return_false");
    reg_add_dlsw_srb_madge_input((service_dlsw_srb_madge_input_type)
				 return_false, "return_false");

    reg_add_udp_fast_flood((service_udp_fast_flood_type)return_false,
			   "return_false");
    reg_add_udp_flood_fs((service_udp_flood_fs_type)return_false,
                         "return_false");
    reg_add_llc2_input(discard_pak_input, "discard_pak_input");
    reg_add_fr_pvc_switch_or_fs((service_fr_pvc_switch_or_fs_type)
				    return_false, "return_false");
    reg_add_atm_dxi_classify_pak_and_fs((service_atm_dxi_classify_pak_and_fs_type)
				    return_false, "return_false");
    reg_add_atm_get_nlpidtype((service_atm_get_nlpidtype_type)
			      return_null, "return_null");

    /*
     * DLSw fast switching defaults.
     */
    reg_add_dlswfst_input(discard_pak_input, "discard_pak_input");

    /*
     * FS_RSRB default
     */
    reg_add_fs_rsrb_direct_input((service_fs_rsrb_direct_input_type)
				 return_false,
				 "return_false");
    reg_add_fs_rsrb_fst_input((service_fs_rsrb_fst_input_type) return_false,
				 "return_false");

    /*
     * Tunneling
     */
    reg_add_tunnel_ip_les_fs((service_tunnel_ip_les_fs_type)return_false,
			      "return_false");
    reg_add_tunnel_ip_hes_fs((service_tunnel_ip_hes_fs_type)return_false,
			     "return_false"); 

    reset_interrupt_level(level);
}



/*
 * fs_table_fill
 * Generic table fill - used by fast switching routines that don't take
 * full advantage of fast_switch table (currently non IP protocols).
 */
void fs_table_fill (fastswitch_type fs_rtn, char* fs_rtn_name, int rxtype)
{
    int fsi;

    for (fsi = SERVICE_ETHER_FS; fsi < SERVICE_LAST_FS; fsi++) {
	registry_add_case(REG_FS, fsi, rxtype, fs_rtn, fs_rtn_name);
    }
}


/*
 * Fastswitching subsystem header
 */

#define FS_MAJVERSION 1
#define FS_MINVERSION 0
#define FS_EDITVERSION  1

SUBSYS_HEADER(fastswitch, FS_MAJVERSION, FS_MINVERSION, FS_EDITVERSION,
	      fastswitch_init, SUBSYS_CLASS_REGISTRY,
	      NULL,
	      NULL);
