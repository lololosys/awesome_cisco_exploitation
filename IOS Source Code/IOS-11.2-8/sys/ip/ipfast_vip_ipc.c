/* $Id: ipfast_vip_ipc.c,v 3.11.6.15 1996/09/11 07:09:21 xliu Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_vip_ipc.c,v $
 *------------------------------------------------------------------
 * IPC Distributed Fast-Switching Application support
 *
 * July 1995, Don Lobete and Darren Kerr
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_vip_ipc.c,v $
 * Revision 3.11.6.15  1996/09/11  07:09:21  xliu
 * CSCdi68762:  crypto map download is missing from vip2 initialization
 * Branch: California_branch
 * Add crypto map download registry call after access list downloading.
 *
 * Revision 3.11.6.14  1996/08/31  00:20:41  dlobete
 * CSCdi66767:  clear counter wont clear the previous distributed cache
 * counter
 * Branch: California_branch
 * Uninitialized fields in statistics IPC message for DFS.
 *
 * Revision 3.11.6.13  1996/08/29  04:22:41  dlobete
 * CSCdi67053:  micro reload breaks DFS
 * Branch: California_branch
 * Wait for the index message from the RSP before asking
 * for any configuration.
 *
 * Revision 3.11.6.12  1996/08/16  08:02:12  dlobete
 * CSCdi59971:  DACL needs modularity work
 * Branch: California_branch
 * For the sake of future maintenance of IP access-lists, share as much
 * source code as possible between VIP and RSP.
 *
 * Revision 3.11.6.11  1996/08/12  06:12:28  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.11.6.10  1996/06/18  14:22:42  dlobete
 * CSCdi60686:  VIP crash when switching off-card
 * Branch: California_branch
 * Don't write or read through NULL pointers.
 *
 * Revision 3.11.6.9  1996/06/06  12:03:30  dlobete
 * CSCdi57944:  DFS confused about rx ignores and tx drops
 * Branch: California_branch
 * Also fix up an error in NACL processing at RSP.
 *
 * Revision 3.11.6.8  1996/06/06  08:58:51  dlobete
 * CSCdi55797:  DACL ignores access-lists when switching to subinterfaces
 * Branch: California_branch
 * To have DFS handle IP output access-lists to multiple subinterfaces
 * (eg on a MIP) and to save the baggage of swidbs down at VIP,
 * the output-access-list is stashed in the DFS cache prefix entry.
 * Also don't enable DFS if multiple input ACLs exist on a VIP interface.
 *
 * Revision 3.11.6.7  1996/06/05  05:09:02  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * No cache invalidation if reset during EOIR
 * Populate txq and txacc down to all IPs after memd recarve for those
 * DFS caches pending on new txq and txacc
 * Branch: California_branch
 *
 * Revision 3.11.6.6  1996/05/28  16:13:54  xliu
 * CSCdi58788:  code changes for vip named access list and encryption perf
 * Branch: California_branch
 *
 * Revision 3.11.6.5  1996/04/29  04:46:45  dlobete
 * CSCdi54163:  Distributed-switched packets discarded on ATM interface
 * Branch: California_branch
 *
 * Revision 3.11.6.4  1996/04/19  15:42:26  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.10  1996/04/11  23:15:13  carrel
 * Branch: IosSec_branch
 * IP code review comments #1
 *
 * Revision 3.1.2.9  1996/04/09  20:38:23  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.8  1996/03/30  05:39:39  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 3.1.2.7  1996/03/19  08:09:34  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.6  1996/02/21  03:30:12  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 3.1.2.5  1996/02/02  21:36:44  xliu
 * Add crypto engine ipc update msg support.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.4  1996/01/31  04:45:29  xliu
 * Branch: IosSec_branch
 * Add RSP/VIP encryption feature code.
 *
 * Revision 3.1.2.3  1996/01/23  05:43:37  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.1.2.2  1996/01/04  19:03:18  xliu
 * Branch: IosSec_branch
 * Replaced the direct crypto system call as the crypto subsys registry
 * call.
 *
 * Revision 3.1.2.1  1995/12/17  01:40:33  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.11.6.3  1996/04/11  00:32:24  dlobete
 * CSCdi50256:  DFS displays incorrect statistics for output MIP interface
 * Branch: California_branch
 *
 * Revision 3.11.6.2  1996/04/10  03:38:36  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.11.6.1  1996/03/18  20:17:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  06:59:08  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/07  09:46:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  01:00:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/04  07:12:43  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.10  1996/03/04  06:14:30  dlobete
 * CSCdi50605:  Crash on VIP in DFS init for multiple VIP configuration
 * Add some self-protection code so we don't act on an DFS enable/disable
 * directive meant for some other VIP.
 *
 * Revision 3.9  1996/03/01  20:55:48  dlobete
 * CSCdi49838:  Illegal malloc at interrupt-level may happen for DFS
 * Pre-initialize the hwidb_array[] for DFS and while we're here
 * clean up some affected DFS statistics code.
 *
 * Revision 3.8  1996/02/24  02:14:44  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.7  1996/01/24  20:23:56  dlobete
 * CSCdi47211:  IP DFS cache operations stall with traffic load on RVIP
 * Use the right callback routine configuration to truly run IPC
 * at interrupt level (and thus DFS cache updates). Now the DFS cache
 * will be updated in a snappier way when the RSP IP cache changes.
 *
 * Revision 3.6  1996/01/24  00:27:39  dlobete
 * CSCdi47213:  IP DFS statistics non-functional under traffic load
 * Don't rely on processes running on VIP - send up DFS statistics
 * when interface statistics are passed up (i.e. at interrupt level).
 *
 * Revision 3.5  1996/01/19  03:02:59  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.4  1996/01/19  00:57:36  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/01  22:21:30  dlobete
 * CSCdi43302:  Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * Temporary work-around to avoid crash while alternate solution is
 * developed.
 *
 * Revision 3.2  1995/11/17  09:35:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/26  05:50:07  dlobete
 * Placeholders for VIP_branch development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "subsys.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "interface_private.h"
#include "../if/network.h"
#include "sys_registry.h"

#include "media_registry.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ipmulticast/igmp.h"
#include "../ip/ip_registry.h"
#include "../ip/ipfast_index.h"
#include "../hes/dgbus.h"
#include "../src-vip/vip_dbus.h"
#include "../src-vip/dfs_vip_idb.h"

#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "../ipc/ipc_message.h"
#include "../ipc/ipc_seats.h"
#include "../ipc/ipc_ports.h"
#include "../ipc/ipc_test.h"
#include "../ipc/ipc_cbus.h"
#include "../ipc/ipc_debug.h"
#include "../ip/ipfast_ipc.h"
#include "../ip/ipaccess_vip.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"
#include "../pas/if_pas.h"       /* includes pas_pci.h */
#include "../src-vip/vip_memd.h"
#include "../src-vip/rsp_memd.h"
#include "../crypto/crypto_registry.h"

static ipc_port_id ipfast_vip_pl_port; /* process-level DFS port (temporary) */
static ipc_port_id ipfast_vip_il_port; /* interrupt-level DFS port */
static boolean ipc_ipfast_init_flag;
static boolean dfs_cfg, dfs_got_rsp_indices;

void ipfast_idb_update (ipc_ipfast_update_idb_msg *update_ptr)
{
    dfsidbtype *hwidb_dfs;

    if (ipc_debug_packets) {
	buginf("\nIPFAST_DS:	 IPFAST_UPDATE_IDB received\n");
	buginf("	hw_if_index      %x\n", update_ptr->hw_if_index);
	buginf("	txq_ptr        %x\n", update_ptr->txq_ptr);
	buginf("	tql_ptr        %x\n", update_ptr->tql_ptr);
    }
    
    hwidb_dfs = dfs_vip_get_hwidb(update_ptr->hw_if_index);
    if (!hwidb_dfs)
	return;
    
    /*
     * The pointer comes down with the RSP's memory mapping.
     * First convert it into flat 16-bit addressing (VIRTUAL2MEMD),
     * then convert to the VIP's virtual memd address mapping.
     */
    hwidb_dfs->tx_q_ptr = (qa_hwq_t *)
	RSP_MEMD2VIRTUAL(RSP_VIRTUAL2MEMD(update_ptr->txq_ptr));
    hwidb_dfs->tx_acc_ptr = (qa_acc_t *)
	RSP_MEMD2VIRTUAL(RSP_VIRTUAL2MEMD(update_ptr->tql_ptr));
    hwidb_dfs->hw_if_index = update_ptr->hw_if_index;
}

void ipfast_index_update (ipc_ipfast_update_hwindex_msg *update_ptr)
{
    dfsidbtype *hwidb_dfs;
    ipc_ipfast_hwindex_elt *data = update_ptr->data;
    int i;
 
    if (ipc_debug_packets) {
        buginf("\nIPFAST_DS:     IPFAST_UPDATE_INDEX received\n");
	buginf("        num_indices      %d\n", update_ptr->num_indices);
    }
 
    /*
     * If output interface is local then set the link for the
     * dfsidb to the real local hwidb and also set the local_sw_vector
     * if the local output dfs interface has not been throttled.
     *
     * This is done on each memd recarve since the VIP interfaces don't know
     * their hw_if_index values (according to the RSP) at VIP init time.
     */
    for(i=0; i < update_ptr->num_indices; i++, data++) {

	/*
	 * Not for this slot; let's go on.
	 */
	if (data->slot != vip_logical_slot())
	    continue;
	
	hwidb_dfs = dfs_vip_get_hwidb(data->hw_if_index);
	if (!hwidb_dfs) {
	    if (ipc_debug_packets)
		buginf("\nNo matching hwidb for hw_if_index %d", 
		       data->hw_if_index);
	    return;
	}
	hwidb_dfs->hw_if_index = data->hw_if_index;

	if ((hwidb_dfs->hwidb = 
	     dfs_get_local_hwidb(data->slotunit, data->vc))) {
	    if (!hwidb_dfs->dfs_throttled) {
		if (ipc_debug_packets) 
		    buginf("\nipfast_index_update(): local_sw_vector to %x",
			   hwidb_dfs->hwidb->fastsend);
		hwidb_dfs->local_sw_vector = hwidb_dfs->hwidb->fastsend;
	    }
        } else {
	    if (ipc_debug_packets)
		buginf("\nNo local hwidb for slotunit %d vc %d",
		       data->slotunit, data->vc);
	}
    }
    dfs_got_rsp_indices = TRUE;
}

void ipfast_prefix_update (ipc_ipfast_add_msg *update_ptr)
{
    dfsidbtype *hwidb_dfs;
    uchar *mac;
    leveltype level;
    ulong flags;

    if (ipc_debug_packets) {
	buginf("\nIPFAST_DS:	 IPFAST_UPDATE_PREFIX received\n");
	buginf("	hw_if_index    %x\n", update_ptr->hw_if_index);
	buginf("	txq_ptr        %x\n", update_ptr->txq_ptr);
	buginf("	tql_ptr        %x\n", update_ptr->tql_ptr);
	buginf("	flags          %x\n", update_ptr->flags);
	buginf("	prefix         %x\n", update_ptr->prefix);
	buginf("	mask           %x\n", update_ptr->mask);
	buginf("	nexthop        %x\n", update_ptr->nexthop);
	buginf("	ip_mtu         %x\n", update_ptr->ip_mtu);
	buginf("	encaps_length  %x\n", update_ptr->encaps_length);
	buginf("	encaps_type    %x\n", update_ptr->encaps_type);
    }
    
    hwidb_dfs = dfs_vip_get_hwidb(update_ptr->hw_if_index);
    if (!hwidb_dfs)
	return;
    
    /*
     * The pointer comes down with the RSP's memory mapping.
     * First convert it into flat 16-bit addressing (VIRTUAL2MEMD),
     * then convert to the VIP's virtual memd address mapping.
     */
    hwidb_dfs->tx_q_ptr = (qa_hwq_t *)
	RSP_MEMD2VIRTUAL(RSP_VIRTUAL2MEMD(update_ptr->txq_ptr));
    hwidb_dfs->tx_acc_ptr = (qa_acc_t *)
	RSP_MEMD2VIRTUAL(RSP_VIRTUAL2MEMD(update_ptr->tql_ptr));
    hwidb_dfs->hw_if_index = update_ptr->hw_if_index;
    hwidb_dfs->type    = update_ptr->idbtype;
    hwidb_dfs->ip_fast_flags = flags = update_ptr->flags;
    hwidb_dfs->slot_and_unit = update_ptr->slot_and_unit;
    hwidb_dfs->queue_algorithm = update_ptr->queue_algorithm;

    assign_encaps_and_hwstring(hwidb_dfs, update_ptr->encaps_type,
			       update_ptr->is_vip);

    /*
     * Currently we don't support distributed IP accounting, DDR
     * interfaces, policy routing or output encryption to non-VIP
     * interfaces. Also fancy-queueing is unsupported.
     */
    if (DFS_SUPPORTED_OUTPUT_FLAGS(flags) 
	&& DFS_SUPPORTED_CRYPTO(flags, update_ptr->is_vip) 
        && !update_ptr->queue_algorithm) {

	mac = (uchar *)update_ptr + sizeof(ipc_ipfast_add_msg);

	level = raise_interrupt_level(NETS_DISABLE);
	add_ipcache_entry(update_ptr->prefix, update_ptr->mask,
		      update_ptr->nexthop, update_ptr->ip_mtu,
		      update_ptr->output_acl, update_ptr->encaps_length, 
		      mac, (void *)hwidb_dfs);
	reset_interrupt_level(level);
    }

}

void ipc_ipfast_print_routecache (ulong route_cache)
{
    buginf("\n");

    if (route_cache & IP_FAST_ROUTECACHE)
            buginf("IP_FAST_ROUTECACHE ");

    if (route_cache & IP_FLOW_ROUTECACHE)
            buginf("IP_FLOW_ROUTECACHE ");

    if (route_cache & IP_OPTIMUM_ROUTECACHE)
            buginf("IP_OPTIMUM_ROUTECACHE ");

    if (route_cache & IP_DISTRIBUTED_ROUTECACHE)
            buginf("IP_DISTRIBUTED_ROUTECACHE ");

    buginf("\n");
}

void ipc_process_permanent_port_ip_cache_pkt (ipc_message_header *header)
{
    ipc_ipfast_msg_hdr *msg, *lastmsg;
    ipc_ipfast_inv_msg *invalidate_ptr;
    ipc_ipfast_en_dis_msg *en_dis_ptr;
    dfsidbtype *hwidb_dfs;
    leveltype level;
    int size;

    msg = (ipc_ipfast_msg_hdr *)&header->data[0];
    lastmsg = (ipc_ipfast_msg_hdr *)((u_char *)msg + header->size);
    while (msg < lastmsg) {
	size = (msg->msg_size + 3) & ~3;
	if (size == 0) {
	    buginf("\n IPFAST_DS: bad msg size %d", size);
	    break;
	}
	ipc_globals->fast_dfs_rx++;

	if (ipc_debug_packets) {
	  buginf("\nIPFAST_DS:    Int-lvl callback! size=%d\n",
		msg->msg_size);
	}
	
	switch (msg->ipfast_type) {
	  case IPFAST_UPDATE_IDB:
	    ipfast_idb_update((ipc_ipfast_update_idb_msg *) msg);
	    break;

	  case IPFAST_UPDATE_INDEX:
	    ipfast_index_update((ipc_ipfast_update_hwindex_msg *) msg);
	    break;

	  case IPFAST_UPDATE_PREFIX:
	    ipfast_prefix_update((ipc_ipfast_add_msg *) msg);
	    
	    break;
	    
	  case IPFAST_INVALIDATE_PREFIX_PR:
	    
	    invalidate_ptr = (ipc_ipfast_inv_msg *) msg;
	    
	    if (ipc_debug_packets) {
		buginf("\nIPFAST-DS: IPFAST_INVALID_PREFIX_PR received\n");
		buginf("	prefix  0x%x\n", invalidate_ptr->prefix);
		buginf("	mask    0x%x\n", invalidate_ptr->mask);
	    }
	    
	    level = raise_interrupt_level(NETS_DISABLE);
	    ipfast_prefix_delete(invalidate_ptr->prefix,
				 ip_bitsinmask(invalidate_ptr->mask), NULL);
	    reset_interrupt_level(level);
	    
	    break;
	    
	  case IPFAST_ENABLE_RC_DIST:

	    en_dis_ptr = (ipc_ipfast_en_dis_msg *) msg;
	    if (ipc_debug_packets) {
		buginf("\nIPFAST-DS: IPFAST_ENABLE_RC_DIST received\n");
		buginf("	hw_if_index      %x\n", 
		       en_dis_ptr->hw_if_index);
		ipc_ipfast_print_routecache(en_dis_ptr->ip_routecache);
	    }
	    
	    level = raise_interrupt_level(NETS_DISABLE);

	    /*
	     * The RSP gives us all the ENABLE messages for the box,
	     * so weed out the ones not relevant for us.
	     */
	    hwidb_dfs = dfs_vip_get_hwidb(en_dis_ptr->hw_if_index);
	    if (!hwidb_dfs)
	      break;
	    
	    if (hwidb_dfs->hwidb)
		hwidb_dfs->hwidb->ip_routecache |= IP_DISTRIBUTED_ROUTECACHE;
	    hwidb_dfs->ip_routecache  |= IP_DISTRIBUTED_ROUTECACHE;
	    
	    /*
	    * Mimic the RSP's fast-switching routecache config.
	    */
	    if (en_dis_ptr->ip_routecache & IP_FLOW_ROUTECACHE) {
		if (hwidb_dfs->hwidb) {
		    hwidb_dfs->hwidb->ip_routecache |= IP_FLOW_ROUTECACHE;
		    hwidb_dfs->hwidb->ip_routecache &= ~IP_OPTIMUM_ROUTECACHE;
		}
		hwidb_dfs->ip_routecache  |= IP_FLOW_ROUTECACHE;
		hwidb_dfs->ip_routecache &= ~IP_OPTIMUM_ROUTECACHE;
	    } else {
		if (hwidb_dfs->hwidb) {
		    hwidb_dfs->hwidb->ip_routecache |= IP_OPTIMUM_ROUTECACHE;
		    hwidb_dfs->hwidb->ip_routecache &= ~IP_FLOW_ROUTECACHE;
		}
		hwidb_dfs->ip_routecache  |= IP_OPTIMUM_ROUTECACHE;
		hwidb_dfs->ip_routecache  &= ~IP_FLOW_ROUTECACHE;
	    }
	    reset_interrupt_level(level);
	    
	    break;
	    
	  case IPFAST_DISABLE_RC_DIST:
	    
	    en_dis_ptr = (ipc_ipfast_en_dis_msg *) msg;
	    if (ipc_debug_packets) {
		buginf("\nIPFAST-DS: IPFAST_DISABLE_RC_DIST received\n");
		buginf("	hw_if_index      %x\n", 
		       en_dis_ptr->hw_if_index);
		ipc_ipfast_print_routecache(en_dis_ptr->ip_routecache);
	    }
	    
	    level = raise_interrupt_level(NETS_DISABLE);

	    hwidb_dfs = dfs_vip_get_hwidb(en_dis_ptr->hw_if_index);
	    if (!hwidb_dfs)
	      break;
	    
	    hwidb_dfs->ip_routecache &= ~IP_DISTRIBUTED_ROUTECACHE;
	    hwidb_dfs->ip_routecache &= ~IP_FLOW_ROUTECACHE;
	    
	    if (hwidb_dfs->hwidb) {
		hwidb_dfs->hwidb->ip_routecache &= ~IP_DISTRIBUTED_ROUTECACHE;
		hwidb_dfs->hwidb->ip_routecache &= ~IP_FLOW_ROUTECACHE;
	    }
	    reset_interrupt_level(level);
	    
	    break;
	    
          case IPFAST_CRYPTO_ENGINE_VIP_UPD_IDB:
	   /* do nothing */
            break;

          case IPFAST_CRYPTO_ENGINE_VIP_KEY_REQUEST:
            reg_invoke_crypto_engine_recv_key_request((void *)msg); 
            break;

          case IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP:
            reg_invoke_crypto_engine_vip_upd_map((uchar *)msg);
            break;
            
	  case IPFAST_ACCESS_ADD_FASTHASH:
	  case IPFAST_ACCESS_DEL_FASTHASH:
	  case IPFAST_ACCESS_ADD_FASTITEM:
	  case IPFAST_ACCESS_DEL_FASTITEM:
	  case IPFAST_ACCESS_ADD_SLOWITEM:
	  case IPFAST_ACCESS_DEL_SLOWITEM:
	  case IPFAST_IP_ACL_INTFACE:
	    if (ipc_debug_packets)
	      buginf("\nDACL_MESSAGE type %x received\n", msg->ipfast_type);
	    level = raise_interrupt_level(NETS_DISABLE);
	    if (dfs_got_rsp_indices)
		ipaccess_vip_queue_to_pl(msg);
	    reset_interrupt_level(level);
	    break;
	    
	  default:
	    if (ipc_debug_packets) {
		buginf("\nIPFAST-DS: Unknown message received type\n");
		buginf("    type      %x\n", msg->ipfast_type);
	    }
	    break;
	    
	}
	msg = (ipc_ipfast_msg_hdr *)((u_char *)msg + size);
    }
}

void vip_ipc_process_permanent_port_pak (paktype *pak)
{
    ipc_message_header *header;

    header = (ipc_message_header *)ipcheadstart(pak);

    switch (header->type) {
    case IPC_TYPE_IP_CACHE:
	ipc_process_permanent_port_ip_cache_pkt(header);
	break;

    default:
	break;
    }
}

void ipc_ipfast_vip_control_pl_callback (ipc_message *message,
				      void *context,
				      ipc_error_code error)
{
    ipc_return_message(message);
}

void ipc_ipfast_do_vip_init (void)
{
 
    ipc_error_code error;
    uchar *control_port_name;
 
    /*
     * Create process-level port and setup callback.
     */
    control_port_name = generate_ds_ctrl_port_name(vip_logical_slot(), FALSE);

    if (control_port_name == NULL)
	return;

    if (!ipfast_vip_pl_port)
        ipc_create_named_port(control_port_name, &ipfast_vip_pl_port, 
			      IPC_PORT_OOB);
    error = ipc_set_receive_callback(ipfast_vip_pl_port, 0,
                                     ipc_ipfast_vip_control_pl_callback);
    if (error != IPC_OK) {
	if (ipc_debug_packets)
	    buginf("Can't set callback for %s\n", control_port_name);
    }

    /*
     * Create interrupt-level port and setup callback.
     */
    control_port_name = generate_ds_ctrl_port_name(vip_logical_slot(), TRUE);

    if (control_port_name == NULL)
        return;

    if (!ipfast_vip_il_port)
        ipc_create_named_port(control_port_name, &ipfast_vip_il_port,
                              IPC_PORT_OOB);
    if (error != IPC_OK) {
	if (ipc_debug_packets)
	    buginf("Can't set callback for %s\n", control_port_name);
    }

    ipc_ipfast_init_flag = TRUE;
}

void ipc_ipfast_vip_init (void)
{
    int i;

    ipc_register_delayed_func(IPC_LEVEL_OTHERS, ipc_ipfast_do_vip_init);

    ipc_platform_struct.platform_permanent_port_process_vector =
	vip_ipc_process_permanent_port_pak;

    /*
     * Populate the hwidb_array[] fully because
     * DFS cache operations are coming down at
     * interrupt level now making the malloc in
     * dfs_vip_get_hwidb() illegal.
     */
    for(i=0; i < MAX_INTERFACES; i++)
	dfs_vip_get_hwidb(i);
}

static boolean dfs_init_ok (void)
{
    /*
     * For EOIR, the RSP should re-send the same hw_if_index
     * values thus there's no need to reset these booleans. 
     * Newly inserted or removed VIPs are taken care
     * of with other mechanisms.
     */
    if (!dfs_cfg) {
	if (dfs_got_rsp_indices) {
	    dfs_cfg = TRUE;

	    /*
	     * Go ask the RSP for our DFS and DACL config.
	     */
	    ipc_ipfast_get_dfs_cfg();
	    ipc_ipfast_get_acl_cfg();
            reg_invoke_crypto_engine_vip_get_maps();
	}
	return(FALSE);
    } else {
	 return(TRUE);
   }
}

/*
 * Take in a hwidb_dfs's worth of statistics at a time
 * and build an IPC message.
 * Future: Handle going over one packet's worth of data.
 */ 
void ipc_ipfast_vip_send_stats (dfsidbtype *hwidb_dfs, ushort hwidb_counter,
				ushort protocol_type)
{
    ipc_ipfast_stat_msg stats_msg;
    ipc_ipfast_stat_unit *int_stats;
    ipc_message *message;
    dfsidbtype *curr_idb = hwidb_dfs;
    ulong dfs_hwidb_counter=0;
    ulong local_driver_drops;
    ulong local_input_stats;

    if (!ipc_ipfast_init_flag)
	return;

    if (!dfs_init_ok())
	return;

    /*
     * Ask for the worst-case message size.
     */
    message = ipc_get_message(sizeof(stats_msg) +
		(hwidb_counter * sizeof(ipc_ipfast_stat_unit)), 
			      0, 0);
    if (message == NULL) {
	if (ipc_debug_packets)
	    buginf("\nIPFAST-DS: ipc_get_message() FAIL.\n");
	return;
    }

    /*
     * Point into the data area leaving room for the stats_msg header.
     */
    int_stats = (ipc_ipfast_stat_unit *)(message->header->data 
						+ sizeof(stats_msg));

    while (curr_idb) {

	/*
	 * Check cautiously for local inputs.
	 */
	if (curr_idb->hwidb)
	    local_input_stats = 
		curr_idb->hwidb->counters.rx_cumbytes_auto[protocol_type];
	else
	    local_input_stats = 0;
		    
	/*
	 * Only send stats for an dfsibdtype if there is
	 * something to report.
	 */
	if (!local_input_stats 
	    && !curr_idb->tx_cumbytes_auto[protocol_type]
	    && !curr_idb->outputdrops
	    && !curr_idb->input_drops) {
	    curr_idb = curr_idb->next;
	    continue;
	}

	/*
	 * The MIPs deal with their own statistics so don't
	 * confuse things by sending them up to the RSP
	 * when these IPs are involved.
	 */
	if (curr_idb->type == IDBTYPE_T1 || curr_idb->type == IDBTYPE_E1) {
	    curr_idb = curr_idb->next;
	    continue;
	}

	/*
	 * Load 'em up.
	 */
	int_stats->if_index = curr_idb->hw_if_index;
	if (curr_idb->hwidb) {
	    int_stats->rx_bytes = 
		curr_idb->hwidb->counters.rx_cumbytes_auto[protocol_type];
	    int_stats->rx_packets = 
		curr_idb->hwidb->counters.inputs_auto[protocol_type];
	    local_driver_drops = curr_idb->hwidb->counters.output_total_drops;
	} else {
	    int_stats->rx_bytes = 0;
	    int_stats->rx_packets = 0;
	    local_driver_drops = 0;
	}
	int_stats->tx_bytes = curr_idb->tx_cumbytes_auto[protocol_type];
	int_stats->tx_packets = curr_idb->outputs_auto[protocol_type];
	int_stats->tx_drops = curr_idb->outputdrops + local_driver_drops;
	int_stats->throttled = curr_idb->input_drops;

	/*
	 * Clear 'em out.
	 */
	curr_idb->tx_cumbytes_auto[protocol_type] = 0;
	curr_idb->outputs_auto[protocol_type] = 0;
	curr_idb->outputdrops = 0;
	if (curr_idb->hwidb) {
	    curr_idb->hwidb->counters.output_total_drops = 0;
	    curr_idb->hwidb->counters.rx_cumbytes_auto[protocol_type] = 0;
	    curr_idb->hwidb->counters.inputs_auto[protocol_type] = 0;
	}
	curr_idb->input_drops = 0;

	int_stats++;
	curr_idb = curr_idb->next;
	dfs_hwidb_counter++;
    }

    /*
     * Build the stats IPC message.
     */
    stats_msg.hdr.ipfast_type = IPFAST_STATS_UPLOAD;
    stats_msg.num_stat_units = dfs_hwidb_counter;
    bcopy(&stats_msg, message->header->data, sizeof(stats_msg));

    /*
     * Change the IPC message size to accurately reflect
     * what statistics we're sending.
     */
    message->header->size = sizeof(stats_msg) +
	(dfs_hwidb_counter * sizeof(ipc_ipfast_stat_unit));

    /*
     * Send IP stats via PVC port for best performance
     * Or a no ack, no retry light way IPC msg, ipc_send_message_noack(message)
     */
    ipc_send_message_permanent_port(message, IPC_TYPE_IP_STATS);
}

void ipc_ipfast_get_dfs_cfg (void)
{
    ipc_message *message;
    ipc_ipfast_cfgreq_msg *cfg_msg;
    ushort num_indices = 0;
    ulong *index_ptr;
    hwidbtype *hwidb;
    dfsidbtype *hwidb_dfs;
    int i;

    FOR_ALL_HWIDBS(hwidb) {
        num_indices++;
    }
 
    message = ipc_get_message(sizeof(ipc_ipfast_cfgreq_msg) +
			      sizeof(ulong) * num_indices, 0, 0);
 
    if (message == NULL) {
        return;
    }

    cfg_msg = (ipc_ipfast_cfgreq_msg *) message->header->data;
    cfg_msg->hdr.ipfast_type = IPFAST_CFG_DFS_REQUEST;
    cfg_msg->num_indices = num_indices;
    index_ptr = (ulong *)&cfg_msg->index_data[0];

    for(i = 0; i < MAX_INTERFACES; i++) {
	hwidb_dfs = hwidb_array[i];
	if (hwidb_dfs && hwidb_dfs->hwidb)
	    *index_ptr++ = hwidb_dfs->hw_if_index;
    }

    ipc_send_message_permanent_port(message, IPC_TYPE_IP_DFSCFG);
}

void ipc_ipfast_get_acl_cfg (void)
{
    ipc_message *message;
    ipc_ipfast_cfgreq_msg cfg_msg;
 
    message = ipc_get_message(sizeof(ipc_ipfast_cfgreq_msg), 0, 0);
 
    if (message == NULL) {
        return;
    }
 
    cfg_msg.hdr.ipfast_type = IPFAST_CFG_ACL_REQUEST;
    bcopy(&cfg_msg, message->header->data, sizeof(cfg_msg));

    ipc_send_message_permanent_port(message, IPC_TYPE_IP_ACLCFG);
}
