/* $Id: dfs_vip_idb.c,v 3.10.6.6 1996/08/12 06:12:43 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/dfs_vip_idb.c,v $
 *------------------------------------------------------------------
 * DFS routines for manipulating hwidbs.
 *
 * Aug 1995, Don Lobete
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dfs_vip_idb.c,v $
 * Revision 3.10.6.6  1996/08/12  06:12:43  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.10.6.5  1996/05/21  10:04:57  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.6.4  1996/05/08  01:10:03  gchristy
 * CSCdi56944:  Compilation errors from IP named access list commit
 * Branch: California_branch
 * - Modularize new ACL code for inclusion in VIP images.
 *
 * Revision 3.10.6.3  1996/05/07  22:57:26  hampton
 * CSCdi52864:  Make 11.2 compile with the Cygnus progressive-96q1 release
 * Branch: California_branch
 *
 * Revision 3.10.6.2  1996/04/12  17:44:34  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.10.6.1  1996/04/10  03:39:00  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.10  1996/03/04  07:14:37  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.9  1996/03/01  20:55:55  dlobete
 * CSCdi49838:  Illegal malloc at interrupt-level may happen for DFS
 * Pre-initialize the hwidb_array[] for DFS and while we're here
 * clean up some affected DFS statistics code.
 *
 * Revision 3.8  1996/02/24  02:12:22  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.7  1996/02/18  19:42:37  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.6  1996/01/24  20:24:02  dlobete
 * CSCdi47211:  IP DFS cache operations stall with traffic load on RVIP
 * Use the right callback routine configuration to truly run IPC
 * at interrupt level (and thus DFS cache updates). Now the DFS cache
 * will be updated in a snappier way when the RSP IP cache changes.
 *
 * Revision 3.5  1996/01/24  00:27:49  dlobete
 * CSCdi47213:  IP DFS statistics non-functional under traffic load
 * Don't rely on processes running on VIP - send up DFS statistics
 * when interface statistics are passed up (i.e. at interrupt level).
 *
 * Revision 3.4  1996/01/19  03:03:34  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.3  1995/12/17  18:38:35  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:51:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/26  05:44:01  dlobete
 * DFS support.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "config.h"
#include "../wan/serial.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../ip/ipfast.h"
#include "../if/tring.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../if/network.h"
#include "../if/atm.h"

#include "../src-rsp/rsp_bufhdr.h"
#include "../src-rsp/rsp_if.h"
#include "../src-rsp/rsp_qasic.h"
#include "../src-rsp/qa_hw.h"
#include "../src-rsp/rsp_fast.h"
#include "../src-rsp/rsp_pcmap.h"
#include "../src-rsp/rsp_cache.h"
/* #include "../ip/ipfast_rsp.h" */

#include "sys_registry.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ip/ip_registry.h"

#include "dfs_vip_idb.h"
#include "../ip/ipfast_index.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"
#include "../ip/ipaccess_vip.h"


dfsidbtype *hwidb_array[MAX_INTERFACES];
dfsidbtype *hwidb_list;
ushort hwidb_counter;
static usec_timestamp dfs_stats_timer;

void dfs_vip_quiesce (void)
{
    dfsidbtype *hwidb_dfs;
    int i;
 
    for(i=0; i < MAX_INTERFACES; i++) {
        hwidb_dfs = hwidb_array[i];
        if (hwidb_dfs == NULL)
            continue;
        hwidb_dfs->tx_q_ptr = NULL;
        hwidb_dfs->tx_acc_ptr = NULL;
    }
}

/*
 * If a hwidb exists for the given index
 * go get it otherwise create one before returning it.
 */
dfsidbtype * dfs_vip_get_hwidb (ushort hw_if_index)
{
    dfsidbtype *hwidb_dfs;
    dfsidbtype *old_start;

    hwidb_dfs = hwidb_array[hw_if_index];

    if (!hwidb_dfs) {
	hwidb_dfs = malloc_named(sizeof(dfsidbtype), "IP DFS cache");

	if (!hwidb_dfs)
	    return NULL;

	/* init the thing */
	hwidb_dfs->hw_if_index = hw_if_index;
	hwidb_dfs->hw_namestring = malloc(DFS_NAMESTRING_LGTH);
	hwidb_dfs->next = NULL;
	hwidb_dfs->ip_output_accesslist = 0;

	hwidb_array[hw_if_index] = hwidb_dfs;

        /*
         * Re-arrange the start of the list.
         */
        old_start = hwidb_list;
        hwidb_list = hwidb_dfs;
        hwidb_dfs->next = old_start;

        hwidb_counter++;
    }

    return hwidb_dfs;
}

/*
 * Erase the existence of hwidb.
 */
void dfs_vip_delete_hwidb (ushort hw_if_index)
{
    dfsidbtype *hwidb_dfs, *prev_hwidb, *curr_hwidb;
 
    hwidb_dfs = hwidb_array[hw_if_index];

   if (!hwidb_list || !hwidb_dfs)
	return;

    /*
     * Walk the list to remove this guy cleanly.
     */
    prev_hwidb = curr_hwidb = hwidb_list;

    if (curr_hwidb == hwidb_dfs) {

	hwidb_list = curr_hwidb->next;

    } else {
	
	while (curr_hwidb != hwidb_dfs) {
	    prev_hwidb = curr_hwidb;
	    curr_hwidb = curr_hwidb->next;	
	}

	prev_hwidb->next = curr_hwidb->next;
    }

    free(hwidb_dfs);
    hwidb_counter--;
    hwidb_array[hw_if_index] = NULL;
}


void dfs_vip_delete_hwidb_all (void)
{

    ushort i;

    for (i = 0; i < (MAX_INTERFACES); i++) {
	if (hwidb_array[i])
	    free(hwidb_array[i]);
    }

    hwidb_counter = 0;
    hwidb_list = NULL;
}

/* 
 * Collect statistics for every interface.
 */
void dfs_check_periodic_tasks (void)
{
    ushort protocol_type = ACCT_PROTO_IP; /* hardcode for now */
    ulong time;

    if (dfs_stats_timer) {
	/*
         * See if it's time to send up dfs stats
         */
	time = USEC_ELAPSED_TIME(dfs_stats_timer);
	if (time < DFS_STATS_INTERVAL) {
	    return;
	}
    }
    USEC_GET_TIMESTAMP(dfs_stats_timer);

    ipc_ipfast_vip_send_stats(hwidb_list, hwidb_counter, protocol_type);
    
    ipflow_tensec();
}

char *dfs_get_namestring (void *hwidb)
{
    dfsidbtype *hwidb_dfs=hwidb;
    
    return(hwidb_dfs->hw_namestring);
}

int dfs_get_slotunit (void *hwidb)
{
    dfsidbtype *hwidb_dfs=hwidb;
 
    return(hwidb_dfs->slot_and_unit);
}

/*
 * Convert RSP's notion of this interface into terms the
 * VIP's hwidb understands.
 */
hwidbtype * dfs_get_local_hwidb (ushort slot_unit, short vc)
{
    hwidbtype *hwidb;
    int slotunit = slot_unit & 0xFFFFFF;
    int pa_bay = (slotunit >= PA_VIRTUAL_SLOT_BOUNDARY) ? 1 : 0;
    int port = (pa_bay) ? (slotunit - PA_VIRTUAL_SLOT_BOUNDARY) : slotunit;

    FOR_ALL_HWIDBS(hwidb)
	if (hwidb->slot == pa_bay && hwidb->slotunit == port &&
	    hwidb->vc == vc) {
	    return(hwidb);
	}
    return(NULL);
}

/*
 * switch vector of DFS in pascb->pak_dfs
 */
boolean vip_pak_dfs(hwidbtype *hwidb, paktype *pak)
{
    if (hwidb->ip_routecache & IP_FLOW_ROUTECACHE)
	return(vip_ip_flow_fs(hwidb, pak));
    else                   /* IP_OPTIMUM_ROUTECACHE */
	return(vip_ip_prefix_fs(hwidb, pak));
}
