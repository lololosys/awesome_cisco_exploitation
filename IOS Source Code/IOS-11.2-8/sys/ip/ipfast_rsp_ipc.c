/* $Id: ipfast_rsp_ipc.c,v 3.10.6.16 1996/08/16 08:02:08 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_rsp_ipc.c,v $
 *------------------------------------------------------------------
 * IPC Distributed Flow-Switching Application support
 *
 * July 1995, Don Lobete and Darren Kerr 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_rsp_ipc.c,v $
 * Revision 3.10.6.16  1996/08/16  08:02:08  dlobete
 * CSCdi59971:  DACL needs modularity work
 * Branch: California_branch
 * For the sake of future maintenance of IP access-lists, share as much
 * source code as possible between VIP and RSP.
 *
 * Revision 3.10.6.15  1996/08/12  06:12:20  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.10.6.14  1996/06/18  01:45:33  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.10.6.13  1996/06/06  21:42:36  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * Fix populate_txq_txacc_to_ips not defined in boot image
 * Branch: California_branch
 *
 * Revision 3.10.6.12  1996/06/06  12:03:24  dlobete
 * CSCdi57944:  DFS confused about rx ignores and tx drops
 * Branch: California_branch
 * Also fix up an error in NACL processing at RSP.
 *
 * Revision 3.10.6.11  1996/06/06  08:58:46  dlobete
 * CSCdi55797:  DACL ignores access-lists when switching to subinterfaces
 * Branch: California_branch
 * To have DFS handle IP output access-lists to multiple subinterfaces
 * (eg on a MIP) and to save the baggage of swidbs down at VIP,
 * the output-access-list is stashed in the DFS cache prefix entry.
 * Also don't enable DFS if multiple input ACLs exist on a VIP interface.
 *
 * Revision 3.10.6.10  1996/06/05  05:08:56  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * No cache invalidation if reset during EOIR
 * Populate txq and txacc down to all IPs after memd recarve for those
 * DFS caches pending on new txq and txacc
 * Branch: California_branch
 *
 * Revision 3.10.6.9  1996/06/04  05:04:02  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * move all DFS/IPC related variables and routines to rsp_ipc_cbus.c
 * Branch: California_branch
 *
 * Revision 3.10.6.8  1996/06/01  16:03:12  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * Flush DFS/IPC message before memd recarve to have a consistent DFS
 * cache after EOIR.
 * Branch: California_branch
 *
 * Revision 3.10.6.7  1996/05/21  09:51:25  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.6.6  1996/05/12  23:09:37  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.10.6.5  1996/05/04  01:04:09  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.10.6.4  1996/04/19  15:42:10  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.8  1996/04/09  20:37:59  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.7  1996/03/30  05:39:28  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 3.1.2.6  1996/03/30  05:19:20  carrel
 * cvs fixes
 *
 * Revision 3.1.2.5  1996/03/19  08:09:25  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.4  1996/01/31  04:45:26  xliu
 * Branch: IosSec_branch
 * Add RSP/VIP encryption feature code.
 *
 * Revision 3.1.2.3  1996/01/23  05:43:32  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.1.2.2  1996/01/04  19:03:08  xliu
 * Branch: IosSec_branch
 * Replaced the direct crypto system call as the crypto subsys registry
 * call.
 *
 * Revision 3.1.2.1  1995/12/19  03:56:32  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 3.10.6.3  1996/04/11  00:32:22  dlobete
 * CSCdi50256:  DFS displays incorrect statistics for output MIP interface
 * Branch: California_branch
 *
 * Revision 3.10.6.2  1996/04/10  03:38:27  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.10.6.1  1996/03/18  20:17:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  06:58:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/13  01:37:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.1  1996/03/07  09:46:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.10  1996/03/05  04:01:23  kao
 * CSCdi50679:  IPC fast message is broadcasted to all IPC-capable boards
 *
 * Revision 3.9  1996/03/04  15:31:11  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Better show output.  Also allow flow stats to be exported.
 *
 * Revision 3.8  1996/03/04  07:12:39  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.7  1996/02/24  02:14:33  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.6  1996/02/18  19:41:36  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.5  1996/01/19  03:02:53  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.4  1996/01/19  00:57:34  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/17  18:29:46  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:35:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:04:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/26  05:50:03  dlobete
 * Placeholders for VIP_branch development.
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <stdlib.h>
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
#include "access.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/ucode.h"
#include "../hes/if_fci.h"
#include "../hes/slot.h"


#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "../ipc/ipc_message.h"
#include "../ipc/ipc_seats.h"
#include "../ipc/ipc_ports.h"
#include "../ipc/ipc_test.h"
#include "../ipc/ipc_cbus.h"
#include "../ipc/ipc_debug.h"

#include "ip.h"
#include "ipfast.h"
#include "ipfast_ipc.h"
#include "ipfast_rsp_ipc.h"

#include "ipaccess_private.h"
#include "ipaccess.h"

#include "../src-rsp/rsp_bufhdr.h"
#include "../src-rsp/rsp_if.h"
#include "../src-rsp/rsp_qasic.h"
#include "../src-rsp/qa_hw.h"
#include "../src-rsp/rsp_fast.h"
#include "../src-rsp/rsp_pcmap.h"
#include "../src-rsp/rsp_ipc_private.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"

static ipc_port_id ipfast_rsp_stat_port;
static ipc_port_id ipfast_rsp_cfg_port;

extern boolean ipcache_debug;

/*
 * Routine to catch DFS statistics messages from RVIP.
 */
void ipc_ipfast_rsp_stats (ipc_message_header *header)
{
    ipc_ipfast_stat_msg *stat_ptr = (ipc_ipfast_stat_msg *) header->data;
    ipc_ipfast_stat_unit *stat_unit;
    uint num_stat_units;
    hwidbtype *hwidb=NULL, *tmphwidb;
 
    switch (stat_ptr->hdr.ipfast_type) {
      case IPFAST_STATS_UPLOAD:

	/*
	 * Make this code index into an array of hwidb pointers
	 * indexed by hw_if_index for speed (similar to array in VIP.
	 * For now walk the hwidb list to find te right interface.
	 */
	num_stat_units = stat_ptr->num_stat_units;
	stat_unit = (ipc_ipfast_stat_unit *)stat_ptr->stats_data;
	while (num_stat_units) {
	    FOR_ALL_HWIDBS(tmphwidb) {
		if (tmphwidb->hw_if_index == stat_unit->if_index) {
		    hwidb = tmphwidb;
		    break;
		}
	    }

	    /*
	     * If no matched hwidb in FOR_ALL_HWIDBS, don't loop forever
	     */
            num_stat_units--;
	    if (!hwidb) {
		if (ipcache_debug)
		    buginf("\nipc_rsp_stat: No output idb for index 0x%x", 
		       stat_unit->if_index);
		continue;
	    }

	    /*
	     * DFS FIXME:
	     * Use auto counters for now - really need a new set of counters.
	     */
	    ip_traffic.inputs += stat_unit->rx_packets;
	    hwidb->counters.inputs_auto[ACCT_PROTO_IP] += stat_unit->rx_packets;
	    hwidb->counters.rx_cumbytes_auto[ACCT_PROTO_IP] += stat_unit->rx_bytes;

            ip_traffic.forwarded += stat_unit->tx_packets;
            hwidb->counters.outputs_auto[ACCT_PROTO_IP] += stat_unit->tx_packets;
            hwidb->counters.tx_cumbytes_auto[ACCT_PROTO_IP] += stat_unit->tx_bytes;

	    hwidb->counters.output_total_drops += stat_unit->tx_drops;
            /* need a new counter in hwidb for stat_unit->throttled */

            stat_unit++;
	}

#ifdef DFS_DEBUG
        buginf("	num_interefaces %d\n", stat_ptr->num_stat_units);
	num_stat_units = stat_ptr->num_stat_units;
	stat_unit = (ipc_ipfast_stat_unit *)stat_ptr->stats_data;
	while (num_stat_units) {
            buginf("		if_index   %d\n", stat_unit->if_index);
            buginf("		rx_bytes   %d\n", stat_unit->rx_bytes);
            buginf("		rx_packets %d\n", stat_unit->rx_packets);
            buginf("		tx_bytes   %d\n", stat_unit->tx_bytes);
            buginf("		tx_packets %d\n", stat_unit->tx_packets);
            buginf("		tx_drops   %d\n", stat_unit->tx_drops);
            buginf("		throttled  %d\n", stat_unit->throttled);
	    num_stat_units--;
	    stat_unit++;
	}
#endif

        break;
 
      default:
        buginf("\nIPFAST-DS: Unknown message received; type %x\n",
                                                   stat_ptr->hdr.ipfast_type);
        break;
    }
}

static void ipc_dfs_cfg_request (ipc_ipfast_cfgreq_msg *cfg_ptr)
{
    hwidbtype *hwidb, *tmphwidb;
    ulong *index_ptr;
    ushort i;

    index_ptr = &cfg_ptr->index_data[0];
    for (i = 0; i < cfg_ptr->num_indices; i++) {

	if ( i >= MAX_CTRLR_INTERFACES)
	    break;

	/*
	 * Make this code index into an array of hwidb pointers
	 * indexed by hw_if_index for speed (similar to array in VIP.
	 * For now walk the hwidb list to find te right interface.
	 */

	hwidb= NULL;
	FOR_ALL_HWIDBS(tmphwidb) {
	    if (tmphwidb->hw_if_index == *index_ptr) {
		hwidb = tmphwidb;
		break;
	    }
	}

	if (!hwidb) {
	    continue;
	}

	ipc_send_enable_ds(hwidb);

	index_ptr++;
    }
}

static void ipc_access_cfg_request (void)
{
    fasthash *acc;
    fastitem *item;
    fasttype *grp;
    slowtype *sitem;
    ushort i, j;
    acl_headertype *acl;

    /*
     * First do the simple access lists.
     */

    for (i = MINFASTACC; i <= MAXFASTACC; i++) {
        acl = acl_array[i];
	if (!acl)
	    continue;
	grp = (fasttype *) acl->list;
        if (grp) {
	    /*
	     * Scan the hash table of single addresses
	     */
            for (j = 0; j < NETHASHLEN; j++) {
                for (acc = grp->hashlist[j]; acc; acc = acc->next)
                    ipc_ipfast_rsp_access_change (TRUE, acl,
                            (void *)acc, NULL, NULL);
            }
	    /*
	     * Scan the table of wildcarded entries
             */
            item = grp->items;
	    while (item)  {
		ipc_ipfast_rsp_access_change (TRUE, acl, NULL,
					      (void *)item, NULL);
		item = item->next;
	    }
	}
    }
 
    /*
     * Now do the extended access lists.
     */
    for (i = MINSLOWACC; i <= MAXSLOWACC; i++) {
	acl = acl_array[i];
	if (!acl)
	    continue;
	sitem = (slowtype *) acl->list;
	while (sitem) {
	    ipc_ipfast_rsp_access_change (TRUE, acl, NULL,
					  NULL, (void *)sitem);
	    sitem = sitem->next;
	}
    }

    /*
     * Look at the interfaces for their configured ACLs.
     */
    ipc_ipfast_rsp_acl_intface_all();
}

void rp_ipc_process_permanent_port_pak (paktype *pak)
{
    ipc_message_header *header;
 
    header = (ipc_message_header *)ipcheadstart(pak);
 
    switch (header->type) {
      case IPC_TYPE_IP_STATS:
        ipc_ipfast_rsp_stats(header);
        break;
 
      case IPC_TYPE_IP_DFSCFG:
        ipc_dfs_cfg_request((ipc_ipfast_cfgreq_msg *)header->data);
        break;
 
      case IPC_TYPE_IP_ACLCFG:
        ipc_access_cfg_request();
        break;

      case IPC_TYPE_CRYPTO:
          reg_invoke_crypto_engine_rsp_ipc_callback(header);
          break;
 
      default:
        break;
    }
}

/*
 * Routine to catch DFS/ACL configuration request messages from RVIP.
 */
void ipc_ipfast_rsp_cfg_callback (ipc_message *message,
                               void *context,
                               ipc_error_code error)
{
    ipc_ipfast_cfgreq_msg *cfg_ptr = 
	(ipc_ipfast_cfgreq_msg *) message->header->data;
 
    switch (cfg_ptr->hdr.ipfast_type) {
      case IPFAST_CFG_DFS_REQUEST:
	ipc_dfs_cfg_request(cfg_ptr);
        break;

      case IPFAST_CFG_ACL_REQUEST:
	ipc_access_cfg_request();
        break;

      default:
	if (ipcache_debug)
	    buginf("\nIPFAST-DS: Unknown message received; type %x\n",
                                                   cfg_ptr->hdr.ipfast_type);
        break;
    }
 
    ipc_return_message(message);
}

/*
 * Initialize RSP IPC support.
 */
void ipc_ipfast_rsp_init (void)
{
    ipc_error_code error;

    if (!ipfast_rsp_stat_port)
        ipc_create_named_port(IPFAST_RSP_STATISTICS_NAME, 
	&ipfast_rsp_stat_port, IPC_PORT_OOB);

    ipc_platform_struct.platform_permanent_port_process_vector =
	rp_ipc_process_permanent_port_pak;

    if (!ipfast_rsp_cfg_port)
        ipc_create_named_port(IPFAST_RSP_CONFIG_NAME,
        &ipfast_rsp_cfg_port, IPC_PORT_OOB);
 
    error = ipc_set_receive_callback(ipfast_rsp_cfg_port, 0,
                                     ipc_ipfast_rsp_cfg_callback);
 
    if (error != IPC_OK) {
        if (ipcache_debug)
            buginf("\nCan't set callback for %s\n", IPFAST_RSP_CONFIG_NAME);
    }

}

/*
 * Cleanup the packet we've sent down to VIP.
 */
void rsp_ipc_callback (ipc_message *message,
                               void *context,
                               ipc_error_code error)
{
    if (message)
        ipc_return_message(message);
}

#define DFS_TEST /* Hook to allow test path along with mainstream */
#ifdef DFS_TEST
ipcache_rntype bogus_entry;
#endif

/*
 *
 */
void ipc_ipfast_rsp_send_add (ipcache_rntype *entry, paktype *pak,
			      ipaddrtype destination)
{
    ipc_ipfast_add_msg *add_msg;
    hwidbtype *hwidb;
    uchar *mac;
    ushort ip_mtu;
    ushort mac_len;
    int size;
#ifdef DFS_TEST
    uchar macbuf[4];
    
    if (!entry) {
	*(ulong *)macbuf = 0x0F000800;
	mac = macbuf;
	mac_len = 4;
	ip_mtu = 1500;
	hwidb = (hwidbtype *) hwidbQ->qhead;
	entry = &bogus_entry;
	entry->prefix   = 0x0A0B0C00;
	entry->mask     = 0xFFFFFF00;
	entry->nexthop  = 0x0A0B0D01;
    } else {
	hwidb = entry->hwidb;
	mac = pak->datagramstart;
	ip_mtu = entry->idb->ip_mtu;
	mac_len = pak->network_start - pak->datagramstart;
    }
#else
    hwidb = entry->hwidb;
    mac = pak->datagramstart;
    ip_mtu = entry->idb->ip_mtu;
    mac_len = pak->network_start - pak->datagramstart;
#endif

    /*
     * Punt if there is a input named ACL defined anyplace.
     */
 
    if (hwidb->ip_fast_flags & IP_FAST_INPUT_NACL)
        return;
 
    /*
     * Only accept cache entries if the output list (if it exists) is
     * is one of the types grokked by DFS.
     */
 
    if (entry->idb->ip_output_accesslist) {
        switch (entry->idb->ip_output_accesslist->type) {
	  case ACL_IP_SIMPLE:
	  case ACL_IP_EXTENDED:
            break;
 
	  default:
            return;
        }
    }

    /*
     * Build the add message.
     */
    size = sizeof(ipc_ipfast_add_msg) + mac_len;
    add_msg = ip_ipc_get_msg(size);
    add_msg->hdr.ipfast_type = IPFAST_UPDATE_PREFIX;
    add_msg->hdr.msg_size = size;
    add_msg->hw_if_index = hwidb->hw_if_index;
    add_msg->slot_and_unit = ((hwidb->slot & 0xFF) << 24) | 
	(hwidb->slotunit & 0xFFFFFF);
    add_msg->is_vip  = IS_VIP(hwidb); 
    add_msg->idbtype = hwidb->type;
    add_msg->txq_ptr = (ulong)hwidb->tx_q_ptr;
    add_msg->tql_ptr = (ulong)hwidb->tx_acc_ptr;
    add_msg->flags = hwidb->ip_fast_flags;
    add_msg->prefix = entry->prefix;
    add_msg->mask = entry->mask;
    add_msg->nexthop = entry->nexthop;
    add_msg->encaps_length = mac_len;
    add_msg->encaps_type = hwidb->fast_switch_type;
    add_msg->ip_mtu = ip_mtu;
    add_msg->output_acl = (entry->idb->ip_output_accesslist) ?
	atoi(entry->idb->ip_output_accesslist->name) : 0;
    add_msg->queue_algorithm = hwidb->queue_algorithm;
    bcopy(mac, add_msg->mac_rewrite, mac_len);
    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, FALSE);
}

/*
 * Do the front-end work of sending a cache-invalidate to VIPs.
 */
void ipc_ipfast_rsp_send_invalid (ipcache_rntype *entry)
{
    ipc_ipfast_inv_msg *inv_msg;
    int size = sizeof(ipc_ipfast_inv_msg);

    inv_msg = ip_ipc_get_msg(size);
    inv_msg->hdr.ipfast_type = IPFAST_INVALIDATE_PREFIX_PR;
    inv_msg->hdr.msg_size = size;
    inv_msg->prefix = entry->prefix;
    inv_msg->mask = entry->mask;
    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, FALSE);
}

static boolean hwidb_has_miacls (hwidbtype *hwidb)
{
    idbtype *idb;
    acl_headertype *first_list = (acl_headertype *) NULL;
    
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
        if (idb->ip_input_accesslist) {
            if (!first_list) {
                first_list = idb->ip_input_accesslist;
                continue;
            } else if (first_list != idb->ip_input_accesslist) {
		/*
		 * Multiple input acls so let's leave.
		 */
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}

/*
 * Enable/Disable DFS for a particular interface on a particular VIP.
 */
void ipc_send_enable_ds (hwidbtype *hwidb)
{
    ipc_ipfast_en_dis_msg *enable_msg;
    int size = sizeof(ipc_ipfast_en_dis_msg);

    if ( !is_vip(hwidb))
        return;

    /*
     * If multiple input access-lists exist for this
     * interface (i.e. we're seeing traffic from multiple
     * sub-interfaces) don't turn on DFS on this interface because
     * it can't handle this - so security hole would be opened.
     */
    if (hwidb_has_miacls(hwidb))
	return;
    
    enable_msg = ip_ipc_get_msg(size);

    /*
     * Build the enable message.
     */
    if (hwidb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE)
	enable_msg->hdr.ipfast_type = IPFAST_ENABLE_RC_DIST;
    else
	enable_msg->hdr.ipfast_type = IPFAST_DISABLE_RC_DIST;

    enable_msg->hdr.msg_size    = size;
    enable_msg->hw_if_index       = hwidb->hw_if_index;
    enable_msg->ip_routecache   = hwidb->ip_routecache;
    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, FALSE);
}

/*
 * Add/Delete an entry to/from an access list.
 */
void ipc_ipfast_rsp_access_change (ushort add_notdelete,
				   acl_headertype *acl, 
				   void *fh_p, void *fi_p, void *si_p) 
{
    fasthash *fh = fh_p;
    fastitem *fi = fi_p;
    slowtype *si = si_p;
    ipc_ipfast_acc_fasthash *add_hash_msg;
    ipc_ipfast_acc_fastitem *add_fitem_msg;
    ipc_ipfast_acc_slowtype *add_sitem_msg;
    int number;
    ushort size;

    /*
     * Only accept cache entries if the output list (if it exists) is
     * is one of the types grokked by DFS.
     */

    if (!acl)
	return;

    switch (acl->type) {
    case ACL_IP_SIMPLE:
    case ACL_IP_EXTENDED:
	break;
	
    default:
	return;
    }
    
    number = atoi(acl->name);
    size = (fh) ? sizeof(ipc_ipfast_acc_fasthash)  :
      ((fi) ? sizeof(ipc_ipfast_acc_fastitem) : 
       sizeof(ipc_ipfast_acc_slowtype));

    /*
     * Build the fasthash_add message.
     */
    if (fh) 	/* fasthash add/del */ {
	add_hash_msg = ip_ipc_get_msg(size);
	if (add_notdelete)
	    add_hash_msg->hdr.ipfast_type = IPFAST_ACCESS_ADD_FASTHASH;
	else
	    add_hash_msg->hdr.ipfast_type = IPFAST_ACCESS_DEL_FASTHASH;
	add_hash_msg->hdr.msg_size = size;
	add_hash_msg->list_number = number;
	add_hash_msg->address = fh->address;
	add_hash_msg->grant = fh->grant;
	
    } else if (fi)	/* fastitem add/del */ {
	
	add_fitem_msg = ip_ipc_get_msg(size);
	if (add_notdelete)
	    add_fitem_msg->hdr.ipfast_type = IPFAST_ACCESS_ADD_FASTITEM;
	else
	    add_fitem_msg->hdr.ipfast_type = IPFAST_ACCESS_DEL_FASTITEM;
	add_fitem_msg->hdr.msg_size = size;
	add_fitem_msg->list_number = number;
	add_fitem_msg->permitmask = fi->permitmask;
	add_fitem_msg->wildmask = fi->wildmask;
	add_fitem_msg->grant = fi->grant;
	
    } else if (si)	/* slowitem add/del */ {
	
	add_sitem_msg = ip_ipc_get_msg(size);
	if (add_notdelete)
	    add_sitem_msg->hdr.ipfast_type = IPFAST_ACCESS_ADD_SLOWITEM;
	else
	    add_sitem_msg->hdr.ipfast_type = IPFAST_ACCESS_DEL_SLOWITEM;
	add_sitem_msg->hdr.msg_size = size;
	add_sitem_msg->list_number = number;
	add_sitem_msg->grant = si->grant;
	add_sitem_msg->source = si->source;
	add_sitem_msg->srcmask = si->srcmask;
	add_sitem_msg->destination = si->destination;
	add_sitem_msg->dstmask = si->dstmask;
	add_sitem_msg->protocol = si->protocol;
	add_sitem_msg->established = si->established;
	add_sitem_msg->dstopr = si->dstopr;
	add_sitem_msg->dport = si->dport;
	add_sitem_msg->dport2 = si->dport2;
	add_sitem_msg->srcopr = si->srcopr;
	add_sitem_msg->sport = si->sport;
	add_sitem_msg->sport2 = si->sport2;
	add_sitem_msg->precedence = si->precedence;
	add_sitem_msg->precedence_value = 
	    si->precedence_value >> IP_PRECEDENCE_SHIFT;
	add_sitem_msg->tos = si->tos;
	add_sitem_msg->tos_value = si->tos_value >> IP_TOS_SHIFT;
	add_sitem_msg->log = si->log;
	add_sitem_msg->fancy = si->fancy;
	
    } else
	return;
    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, FALSE);
}


void ipc_ipfast_rsp_acl_intface_all (void)
{
    ipc_ipfast_acl_ena_msg *acl_msg;
    ipc_ipfast_acl_inout *intface_ipc_msg;
    hwidbtype *tmphwidb;
    acl_headertype *acl;
    ulong acl_message_sz = sizeof(ipc_ipfast_acl_ena_msg) +
                                sizeof(ipc_ipfast_acl_inout);

    /*
     * Gather all the interfaces that have access lists configured
     * and send a message for each interface/ip access-group pair.
     */
    FOR_ALL_HWIDBS(tmphwidb) {
	if (tmphwidb->firstsw->ip_output_accesslist) {

            acl_msg = ip_ipc_get_msg(acl_message_sz);
            acl_msg->hdr.ipfast_type = IPFAST_IP_ACL_INTFACE;
            acl_msg->hdr.msg_size = acl_message_sz;
            acl_msg->num_interfaces = 1;
            intface_ipc_msg = (ipc_ipfast_acl_inout *) &acl_msg->acl_data[0];

	    acl = tmphwidb->firstsw->ip_output_accesslist;
	    switch (acl->type) {
	      case ACL_IP_SIMPLE:
	      case ACL_IP_EXTENDED:
		intface_ipc_msg->ip_out_accesslist = atoi(acl->name);
		break;
	
	      default:
		intface_ipc_msg->ip_out_accesslist = 0;
		break;
	    }
	    intface_ipc_msg->hw_if_index = tmphwidb->hw_if_index;	
	    intface_ipc_msg->in_not_out = FALSE;
	    intface_ipc_msg->no_inacls = FALSE;
	    
	    ip_ipc_send_msg(acl_message_sz, IPC_TYPE_IP_CACHE, FALSE);
	}

	if (tmphwidb->firstsw->ip_input_accesslist) {

            acl_msg = ip_ipc_get_msg(acl_message_sz);
            acl_msg->hdr.ipfast_type = IPFAST_IP_ACL_INTFACE;
            acl_msg->hdr.msg_size = acl_message_sz;
            acl_msg->num_interfaces = 1;
            intface_ipc_msg = (ipc_ipfast_acl_inout *) &acl_msg->acl_data[0];

	    acl = tmphwidb->firstsw->ip_input_accesslist;
	    switch (acl->type) {
	      case ACL_IP_SIMPLE:
	      case ACL_IP_EXTENDED:
		intface_ipc_msg->ip_in_accesslist = atoi(acl->name);
		break;
		
	      default:
		intface_ipc_msg->ip_in_accesslist = 0;
		continue;
	    }
	    intface_ipc_msg->hw_if_index = tmphwidb->hw_if_index;
	    intface_ipc_msg->in_not_out = TRUE;
	    intface_ipc_msg->no_inacls = (tmphwidb->ip_fast_flags & 
				  IP_FAST_INPUTACCESSLISTS) ? FALSE : TRUE;
	    
	    ip_ipc_send_msg(acl_message_sz, IPC_TYPE_IP_CACHE, FALSE);
	}
    }
}

void ipc_access_group_callback (idbtype *idb, boolean groupin,
                                acl_headertype *acl)
{
    ipc_ipfast_acl_ena_msg *acl_msg;
    ipc_ipfast_en_dis_msg *enable_msg;
    ipc_ipfast_acl_inout *intface;
    hwidbtype *hwidb = idb->hwptr;
    uint listnumber;
    ulong acl_message_sz = sizeof(ipc_ipfast_acl_ena_msg) +
	                        sizeof(ipc_ipfast_acl_inout);
    int size;

    if (acl) {
	/*
	 * Only numbered ACLs are supported for now.
	 */
	switch (acl->type) {
	  case ACL_IP_SIMPLE:
	  case ACL_IP_EXTENDED:
	    break;
 
	  default:
	    return;
	}
	listnumber = atoi(acl->name);
    } else {
	/*
	 * We are processing a "no" event.
	 */
	listnumber = 0;
    }

    acl_msg = ip_ipc_get_msg(acl_message_sz);
    acl_msg->hdr.ipfast_type = IPFAST_IP_ACL_INTFACE;
    acl_msg->hdr.msg_size = acl_message_sz;
    acl_msg->num_interfaces = 1;

    intface = (ipc_ipfast_acl_inout *)(&acl_msg->acl_data);
    intface->hw_if_index = hwidb->hw_if_index;
    if (groupin) {
	intface->ip_in_accesslist = listnumber;
	intface->ip_out_accesslist = 0;
	intface->in_not_out = TRUE;
	intface->no_inacls = (hwidb->ip_fast_flags & IP_FAST_INPUTACCESSLISTS)
	    ? FALSE : TRUE;
    } else {
	intface->ip_out_accesslist = listnumber;
	intface->ip_in_accesslist = 0;
	intface->in_not_out = FALSE;
	intface->no_inacls = FALSE;
    }

    ip_ipc_send_msg(acl_message_sz, IPC_TYPE_IP_CACHE, FALSE);

    /*
     * For subinterfaces input ACL changes do some further checking.
     * We have been called because an ACL has changed.
     * If distributed switching is intended to be on for this
     * interface (or subinterface), then enable or disable DFS
     * down at VIP accordingly.
     */
    if (groupin && (hwidb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE)) {
 
        size = sizeof(ipc_ipfast_en_dis_msg);
        enable_msg = ip_ipc_get_msg(size);
 
        if (hwidb_has_miacls(hwidb))
            enable_msg->hdr.ipfast_type = IPFAST_DISABLE_RC_DIST;
        else
            enable_msg->hdr.ipfast_type = IPFAST_ENABLE_RC_DIST;
 
        enable_msg->hdr.msg_size    = size;
        enable_msg->hw_if_index     = hwidb->hw_if_index;
        enable_msg->ip_routecache   = hwidb->ip_routecache;
        ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, FALSE);
    }
}
