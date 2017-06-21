/* $Id: novfast.c,v 3.14.4.18 1996/08/28 00:52:37 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/novfast.c,v $
 *------------------------------------------------------------------
 * novfast.c -- Device independent XNS and Novell Fast Switching
 *
 * 29-April-1990, Phanindra Jujjavarapu
 * November 1992, Joanne Boyle - Split novfast.c from xnsfast.c
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novfast.c,v $
 * Revision 3.14.4.18  1996/08/28  00:52:37  hampton
 * Migrate Desktop Protocols away from the one minute and one second
 * registries.  [CSCdi67383]
 * Branch: California_branch
 *
 * Revision 3.14.4.17  1996/08/14  14:12:11  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * add multilink half-fastswitching support
 *
 * Revision 3.14.4.16  1996/08/14  02:37:26  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.14.4.15  1996/08/07  09:05:25  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.14.4.14  1996/07/23  18:50:06  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.14.4.13  1996/07/09  06:06:46  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.14.4.12  1996/06/28  23:33:41  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.14.4.11  1996/06/12  17:37:53  akr
 * CSCdi60228:  ipx route-cache can grow without limit
 * Branch: California_branch
 *
 * Revision 3.14.4.10  1996/06/10  22:03:57  mschaef
 * CSCdi60014:  Alignment error in nov_fastswitch when IPX multiring
 *              on token
 * Branch: California_branch
 *
 * Revision 3.14.4.9  1996/05/17  12:18:07  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.12.2.4  1996/05/06  00:12:32  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.12.2.3  1996/04/26  15:24:48  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.2  1996/04/03  22:21:38  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.14.4.8  1996/05/09  14:49:56  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.14.4.7  1996/05/07  16:43:21  mschaef
 * CSCdi56910:  Unreachable services not removed from SAP table
 * Branch: California_branch
 *
 * Revision 3.14.4.6  1996/05/04  02:18:42  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Enqueue to routable packet received on bridged interface to the
 *   appropriate queue instead of bridgeQ.
 * - Restore input to the BVI if necessary when copying packet for
 *   process-level.
 *
 * Name Access List
 *
 * Revision 3.14.4.5  1996/04/30  18:49:36  wmay
 * CSCdi54817:  Extra board encap added for ipx over frame-relay
 * Branch: California_branch
 *
 * Revision 3.14.4.4  1996/04/27  06:47:43  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.14.4.3.10.1  1996/04/27  07:39:52  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.14.4.3  1996/04/05  00:18:10  dko
 * CSCdi51183:  IPX Alignment errors
 * Branch: California_branch
 * Fixed word misalignment problem in nov_cache_update
 *
 * Revision 3.14.4.2.4.1  1996/04/08  02:44:03  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.14.4.2  1996/03/29  03:19:21  hou
 * CSCdi50046:  vLAN 802.10 supports only IPX Arpa encaps.
 * Branch: California_branch
 * - should support default novell-ether IPX encap.
 *
 * Revision 3.14.4.1  1996/03/18  22:53:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.14.6.1  1996/03/22  22:56:50  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.14  1996/03/09  18:13:16  mschaef
 * CSCdi46978:  IPX fastswitch cache can grow very large
 *
 * Revision 3.13  1996/03/01  07:34:22  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.12  1996/02/25  03:53:23  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.11  1996/02/12  22:21:40  dko
 * CSCdi48651:  no ipx routing doesnt remove ipx accounting cmd.
 * Insert generic check for if novell is running or shutting down.
 * Clean up a leftover debugging bugtrace.
 *
 * Revision 3.10.8.1  1996/03/05  05:38:28  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.10.4.1  1996/02/27  21:21:23  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.10  1996/02/05  02:07:14  mmcneali
 * CSCdi46205:  IPX packets are corrupted by vLAN 802.10 fast-switch.
 *
 * Revision 3.9  1996/01/22  07:53:01  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8.4.1  1996/02/15  19:27:39  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.8  1995/12/15  01:38:21  mschaef
 * CSCdi45394:  IPX AS and input access list restrictions not properly
 * enforced
 *
 * Revision 3.7  1995/12/13  02:54:40  mmcneali
 * CSCdi43645:  IPX pings process-switched, not fast for RFC1483 to
 * RFC1483.
 *
 * The assumption that Fast Switching between subinterfaces having the
 * same encapsulation is only allowable for vLAN subinterfaces should
 * only be applied to LAN media types.
 *
 * Revision 3.6  1995/12/07  22:34:59  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.5  1995/12/01  06:25:51  mschaef
 * CSCdi37234:  Allow fastswitching of certain directed broadcast packets
 *
 * Revision 3.4  1995/11/26  08:24:12  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *              If subinterface configured as a vLAN do not permit another
 *              encapsulation.
 *
 *              Correct counters of vLAN packets switched.
 *
 * Revision 3.3  1995/11/17  19:23:22  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:53:47  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:59:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:39:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/04  01:26:38  john
 * CSCdi41272:  SegV exception in IPX fast switch cache update
 *              Part 2: code review suggested better fix.
 *
 * Revision 2.8  1995/10/02  16:56:51  john
 * CSCdi41272:  SegV exception in IPX fast switch cache update
 *
 * Revision 2.7  1995/09/13  05:24:59  tli
 * CSCdi40250:  SSE: clean up IPX debugging
 *
 * Revision 2.6  1995/08/05  08:40:18  lbustini
 * CSCdi38136:  Incorrect output interface selected when fastswitching
 * over rotary group.
 * When a ddr link comes up, use the authenticated name along with
 * the phone number to set up the route cache entry.
 *
 * Revision 2.5  1995/07/10  05:33:08  fred
 * CSCdi36850:  commit fast path queuing for appletalk, decnet, etc.
 *
 * Revision 2.4  1995/07/07  04:30:31  bbenson
 * CSCdi35019:  Fast/SSE switching of LANE IPX SAP encaps not working.
 * - Split out writing of lane encaps from nov_write_hes_aip_header.
 * - Allow same enctype on subinterfaces of non-broadcast media (AIP).
 * - Set uninitialized pak->encsize in novell_et_novell_ether.
 *
 * Revision 2.3  1995/07/01  03:38:59  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.2  1995/06/15 06:03:51  dbath
 * CSCdi33071:  IPX fast switching, over equal cost ATM paths is broken
 * Replaced PAK_FR_CACHED flag with a more generic one, PAK_ENCAPS_ONLY,
 * so that we could also use it in ATM-land.  Made IPX ATM fastswitch
 * cache population code cognisant of multiple equal cost paths,
 * like the frame relay case.
 *
 * Revision 2.1  1995/06/07  23:28:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Description of Routines Exported from this module. 
 *
 * nov_cache_init:	  Initializes NOVELL cache hash list.
 * nov_cache_update:      Updates NOVELL cache.
 * nov_fscache_age:	  Ages NOVELL routing cache.
 * nov_fscache_flush:	  Flushes NOVELL routing cache.
 * show_nov_cache:	  Displays NOVELL cache entries.
 *
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "../hes/sse_registry.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "interface.h"
#include "media_registry.h"
#include "packet.h"
#include "access.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "config.h"
#include "../wan/serial.h"
#include "../wan/ppp.h"
#include "../wan/dialer.h"
#include "../wan/dialer_registry.h"
#include "../if/network.h"
#include "xns.h"
#include "../xns/xnstypes.h"
#include "novfast.h"
#include "novell.h"
#include "../ip/tunnel.h"
#include "../ip/gre.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/isis_ipx.h"
#include "../srt/srb_multiring.h"
#include "../srt/srt_registry.h"
#include "ipx_registry.h"
#include "logger.h"

#include "../vlan/vlan.h"
#include "../tbridge/tbridge_sde.h"


/*
 * macros
 */

#define TABLE(hash) (novcache[hash])
#define T_ADDR(hash) (&novcache[hash])

/*
 * Forward declarations:
 */
static boolean nov_flush_cache_net (ulong net);

/*
 * Local Storage
 */
novcachetype *novcache[XNS_HASHLEN];
long novcache_version, novcache_entries, novcache_valid;
ulong novcache_max_size;
ushort novcache_max_inactive_age;
ushort novcache_inactive_age_rate;
ulong novcache_max_update_age;
ushort novcache_update_age_rate;
novcache_hashtable_type *novcache_hashtable;

#undef DEBUG

#ifdef NOVCACHE_STATS
ulong novcache_reason[NOVCACHE_REASON_MAX];
ulong novcache_param[NOVCACHE_REASON_MAX];
sys_timestamp novcache_time[NOVCACHE_REASON_MAX];
ulong         novcache_history_reason[NOVCACHE_HISTORY_MAX];
ulong         novcache_history_param[NOVCACHE_HISTORY_MAX];
sys_timestamp novcache_history_time[NOVCACHE_HISTORY_MAX];
ulong         novcache_trigger_reason[NOVCACHE_HISTORY_MAX];
sys_timestamp novcache_trigger_time[NOVCACHE_HISTORY_MAX];
int novcache_history_index;
int novcache_trigger_index;
ulong novcache_trigger;
#endif

/*
 * novcache_increment_version()
 *	Increment the cache version & do same for cBus complex where
 *	necessary. Special handling for route table changes. If the
 *      default route is involved blow everything away ASAP. Otherwise,
 *      invalidate only entries which apply to the specific network.
 *      If the network is not represented in the cache at all, do nothing.
 */
void novcache_increment_version (hwidbtype* hwidb, int reason, ulong param,
				 ulong netmask)
{
    int i;

    if (novell_debug) {
	buginf("\nIPX: cache flush");
    }
    if (netmask == NOV_NETMASK_ALL) {
	switch (reason) {
	  case NOVCACHE_RIP_AGER_STALE:
	  case NOVCACHE_RIP_AGER_TRIM:
	  case NOVCACHE_RIP_EQ_PATH_STALE:
	  case NOVCACHE_RIP_EQ_PATH_FASTER1:
	  case NOVCACHE_RIP_ADD_EQ_PATH:
	  case NOVCACHE_RIP_ADD_EQ_PATH_FASTER:
	  case NOVCACHE_RIP_BETTER_METRIC:
	  case NOVCACHE_RIP_WORSE_METRIC_LAST:
	  case NOVCACHE_RIP_WORSE_METRIC:
	  case NOVCACHE_RIP_BETTER_XROUTE:
	  case NOVCACHE_NLSP_PATH_CHANGE:
	  case NOVCACHE_NLSP_BETTER_METRIC:
	  case NOVCACHE_NLSP_WORSE_METRIC:
	  case NOVCACHE_NLSP_UNREACHABLE:
	    /*
	     * Attempt to invalidate a single network. However, if
	     * unsuccessful, or the default route has flapped then
	     * blow everything away.
	     */
	    if ((param == NOV_RIP_DEFAULT_ROUTE) || 
		!nov_flush_cache_net(param)) {
		novcache_version++;
		novcache_valid = 0;
		reg_invoke_auto_sw_cache_version(LINK_NOVELL);
	    }
	    break;
	case NOVCACHE_RIP_NEW_ROUTE:
	case NOVCACHE_IGRP_NEW_ROUTE:
	case NOVCACHE_NLSP_NEW_ROUTE:
	    /*
	     * May need to flush existing cache entries if an explicit
	     * route is learned and a default route is active.
	     */
	    if (!novell_default_route || (novell_default_ndb == NULL)) {
		/*
		 * Immediate return if default route not active or available.
		 */
		return;
	    }
	    if (novell_default_route && (param == NOV_RIP_DEFAULT_ROUTE)) {
		/*
		 * Immediate return if we just learned default route. Nobody
		 * is using it yet.
		 */
		return;
	    }
	    /*
	     * A default route is active and the new route is not default.
	     * Attempt to flush only this specific network. Dump everything
	     * if this cannot be done for whatever reason.
	     */
	    if (!nov_flush_cache_net(param)) {
		novcache_version++;
		novcache_valid = 0;
		reg_invoke_auto_sw_cache_version(LINK_NOVELL);
	    }
	    break;
	    
	default:
	  if ((hwidb == NULL) || (hwidb->novell_routecache)) {
	      novcache_version++;
	      novcache_valid = 0;
	      reg_invoke_auto_sw_cache_version(LINK_NOVELL);
	  }
	  break;
	}
    } else {
	/*
	 * Blow away the entire cache if we're invalidating an aggregated
	 * route.
	 */
	if ((hwidb == NULL) || hwidb->novell_routecache) {
	    novcache_version++;
	    novcache_valid = 0;
	    reg_invoke_auto_sw_cache_version(LINK_NOVELL);
	}
    }    
#ifdef NOVCACHE_STATS
    /*
     * Record reason for flush:
     */
    if ((reason >= 0) && (reason < NOVCACHE_REASON_MAX)) {
	novcache_reason[reason]++;
	novcache_param[reason] = param;
	GET_TIMESTAMP(novcache_time[reason]);
	i = novcache_history_index++;
	novcache_history_index %= NOVCACHE_HISTORY_MAX;
	novcache_history_reason[i] = reason;
	novcache_history_param[i] = param;
	GET_TIMESTAMP(novcache_history_time[i]);
	if (novcache_trigger && (param == novcache_trigger)) {
	    i = novcache_trigger_index++;
	    novcache_trigger_index %= NOVCACHE_HISTORY_MAX;
	    novcache_trigger_reason[i] = reason;
	    GET_TIMESTAMP(novcache_trigger_time[i]);
	}
    }
#endif
}

/*
 * novcache_external_increment_version()
 *	Increment the cache version & do same for cBus complex where
 *	necessary.  Called from outside of novell without the reason codes.
 */
void novcache_external_increment_version(hwidbtype* hwidb, idbtype *swidb,
					 boolean immediately)
{
    novcache_increment_version(hwidb, NOVCACHE_EXTERNAL_EVENT, 0, 
			       NOV_NETMASK_ALL);
}

/*
 * novcache_init
 * Initializes Novell cache hashlist.
 */

void nov_cache_init (void)
{
    int i;
    
    for (i=0; i < XNS_HASHLEN; i++)
	novcache[i] = NULL;
    novcache_version = 0L;
    novcache_entries = 0L;
    novcache_valid = 0;
    novcache_max_inactive_age = NOVRCACHE_DEFAULT_MAX_INACTIVE_AGE;
    novcache_inactive_age_rate = NOVRCACHE_DEFAULT_INACTIVE_AGE_RATE;
    novcache_max_update_age = (NOVRCACHE_DEFAULT_MAX_UPDATE_AGE*ONEMIN);
    novcache_update_age_rate = NOVRCACHE_DEFAULT_UPDATE_AGE_RATE;
    
    novcache_hashtable =
 	malloc_named(sizeof(novcache_hashtable_type), "IPX FS Cache");
    for (i = 0; i < NOV_NETHASHLEN; i++)
 	(*novcache_hashtable)[i] = NULL;
    
    reg_invoke_auto_sw_cache_init(LINK_NOVELL);

#ifdef NOVCACHE_STATS
   for (i = 0; i < NOVCACHE_REASON_MAX; i++) {
       novcache_reason[i] = 0;
       novcache_param[i] = 0;
       TIMER_STOP(novcache_time[i]);
   }
   for (i = 0; i < NOVCACHE_HISTORY_MAX; i++) {
       novcache_history_reason[i] = 0;
       novcache_history_param[i] = 0;
       TIMER_STOP(novcache_history_time[i]);
       novcache_trigger_reason[i] = 0;
       TIMER_STOP(novcache_trigger_time[i]);
   }
   novcache_history_index = novcache_trigger_index = novcache_trigger = 0;
#endif

    /*
     * Register some functions
     */
    reg_add_fs_cache_update(LINK_NOVELL, nov_cache_update, "nov_cache_update");
    reg_add_ipx_novcache_increment_version(novcache_increment_version, 
					   "novcache_increment_version");
}

/*
 * novcache_hash_table_delete
 *
 */
static void novcache_hash_table_delete (novcachetype *entry)
{
    int i;
    novcachetype *hash, *prev;

    i = NOV_NETHASH(entry->novc_dest_net);
    for (prev = NULL, hash = (*novcache_hashtable)[i]; hash;
	 prev = hash, hash = hash->novc_hash_next) {
	if (hash == entry) {
	    if (prev)
	      prev->novc_hash_next = hash->novc_hash_next;
	    else
	      (*novcache_hashtable)[i] = hash->novc_hash_next;
	    hash->novc_hash_next = NULL;
	    break;
	}
    }
}

/*
 * novcache_hash_table_insert
 *
 */
static void novcache_hash_table_insert (novcachetype *entry)
{
    int i;

    if (entry->novc_hash_next != NULL)
	return;

    i = NOV_NETHASH(entry->novc_dest_net);
    entry->novc_hash_next = (*novcache_hashtable)[i];
    (*novcache_hashtable)[i] = entry;
}

/*
 * nov_free_entry
 * Release an entry
 */

static void nov_free_entry (novcachetype *entry_ptr,
			    novcachetype *prev_ptr)
{
    int status;

    if (novcache_version == entry_ptr->novc_version) {
	novcache_valid--;
    }
    status = raise_interrupt_level(NETS_DISABLE);
    if (prev_ptr != NULL) {
	prev_ptr->novc_next_ptr = entry_ptr->novc_next_ptr;
    }
    if (entry_ptr->novc_ext_cache) {
	reg_invoke_auto_sw_ipx_cache_invalidate(entry_ptr);
    }
    if (entry_ptr->novc_path_list_ptr != NULL) {
	free(entry_ptr->novc_path_list_ptr);
	entry_ptr->novc_path_list_ptr = NULL;
    }
    novcache_hash_table_delete(entry_ptr);
    free(entry_ptr);
    novcache_entries--;
    if (novcache_entries < 0)
	novcache_entries = 0;
    reset_interrupt_level(status);
}

/*
 * nov_flush_cache_net
 *
 * Invalidate the novell cache entries for "net" if possible:
 *
 *     - If all cache entries are individually invalidated (or none exist
 *       for the target network), return TRUE. No further action required
 *       by caller.
 *
 *     - A request for full cache invalidation is signalled by returning
 *       FALSE if individual cache entry invalidation is impossible or
 *       impractical.
 *
 * Note: INVALIDATION_LIMIT is the maximum number of individual invalidations
 *       to attempt. Beyond this point, invalidate the entire cache.
 */

#define INVALIDATION_LIMIT 3

static boolean nov_flush_cache_net (ulong net)
{
    int i;
    novcachetype *hash, *prev;
    boolean novell_auto_switching;
    int invalidation_count;
    novcachetype *invalidation_hash_array[INVALIDATION_LIMIT];


    invalidation_count = 0;
    novell_auto_switching = novell_auto_sw_active();
    i = NOV_NETHASH(net);

    for (prev = NULL, hash = (*novcache_hashtable)[i]; hash;
	 prev = hash, hash = hash->novc_hash_next) {

	if ((hash->novc_dest_net == net) &&
	    (hash->novc_version == novcache_version)) {
	    
	    hash->novc_version--;
	    novcache_valid--;

	    if (novell_auto_switching) {
		if (invalidation_count == INVALIDATION_LIMIT) {
		    return(FALSE);
		}
		invalidation_hash_array[invalidation_count] = hash;
		invalidation_count++;
	    }
	}
    }

    if (novell_auto_switching)
	for (i = 0; i < invalidation_count; i++) {
	    reg_invoke_auto_sw_ipx_cache_invalidate
		(invalidation_hash_array[i]);
	}
    
    return(TRUE);
}

/*
 * nov_cache_update
 * Updates the Novell cache with the current Novell datagram.
 * Caller checks for fast switching okay on output interface,
 * for Novell protocol, and for correct output encapsulation.
 * Promote entries to top of list if they've drifted down aways.
 */

void nov_cache_update (paktype *pak_ptr, ulong address_tmp)
    			/* pointer to outgoing Datagram */
{
    ipxhdrtype *ipx;
    novcachetype *cache_ptr, *prev_ptr;
    idbtype *swinput;
    idbtype *swoutput;
    hwidbtype *hwinput;
    hwidbtype *hwoutput, *nouthwidb;
    nidbtype *noutput;
    boolean newflag;
    int len, status, depth;
    ulong bucket;
    novndbtype *rip_rt;
    novpathtype	*cache_pptr = 0, *prev_pptr;
    novrdbtype *path_ptr;
    ushort pathcount;
    charlong dnet;
    uchar *daddr, dsap;
    int extra, riflen = 0;
    int delta, total_encap_bytes = 0;
    trrif_t *trh;
    fddirif_t *fhdr;
    riftype *rif;
    ulong *ptr = (ulong *) NULL;
    leveltype level;
    paktype *fr_pak = NULL;
    paktype *atm_pak = NULL;
    dialergrouptype *dialergroup;
    boolean dialer_interface = FALSE;
    addrtype proto_addr;
    uchar rii_mask;
    boolean bcast = FALSE;
    encapstype enctype;

    ipx = (ipxhdrtype *)ipxheadstart(pak_ptr);
    /*
     * Conditions for acceptance:
     * - must be transit (not sourced by us).
     * - output network/encap must be valid/different from input network/encap.
     * - output must be via an MCI interface.
     * - must not be node address of input/output interface.
     * - no network layer broadcast destinations.
     * - no MAC layer multicast destinations (with the exception of the ISL
     *   vLAN Cisco multicast).
     * - must be ETHER, TOKEN, FDDI, or serial HDLC/PPP/FR.
     * - must be ARPA, NOVELL-ETHER, HDLC, ISO1/2 (FDDI/CTR) or IEEE 802.10 or
     *   ISL (vLAN) encapsulation.
     */

    swinput  = pak_ptr->if_input;
    swoutput = pak_ptr->if_output;

    if (swinput != NULL && swoutput != NULL) { /* avoid potential bus errors */
	hwinput  =  swinput->hwptr;
	hwoutput = swoutput->hwptr;
	noutput = pak_ptr->nif_output;
    } else {
	/* make the compiler shut up */
	hwinput = NULL;
	hwoutput = NULL;
	noutput = NULL;
    }
    
    dialer_interface = if_is_ddr(hwoutput);

    /*
     * If this interface is in a dialer rotary group, get dialer interface.
     */
    if (dialer_interface) {
	/*
	 *  if this is a multilink bundle interface, we need
	 *     to check the virtual bundle interface that
	 *     is part of the dialer rotary-group here.
	 *  The dialer interface or multilink bundle interface that
	 *     is not part of a dialer rotary-group is checked in
	 *     the normal place after this.
	 */
	if (swoutput->is_bundle_interface &&
	    !reg_invoke_fast_supported(LINK_NOVELL, hwoutput))
	    return;
	dialergroup = get_dialergroup(hwoutput);
	if (!dialergroup) {
#ifdef DEBUG
	    if (novell_debug)
	        buginf("\nInterface %s has no dialergroup. Cannot fastswitch",
		       hwoutput->hw_short_namestring);
#endif
	    return;
	}
	swoutput = pak_ptr->dialer_output;
	hwoutput = swoutput->hwptr;
    } else {
	dialergroup = NULL;
    }

    /*
     * Dump non-transit/loopback packets:
     */
    if (swinput == NULL)
	return;

    /*
     * Bail if anything is wierd:
     */
    if ((swoutput == NULL) || (!validmem(swoutput)) ||
	(hwoutput == NULL) || (!validmem(hwoutput)) ||
	(noutput == NULL) || (!validmem(noutput)) ||
	(hwinput == NULL) || (!validmem(hwinput)) || (!validmem(swinput))) {
#ifdef DEBUG
	if (novell_debug) 
	    buginf("\nnov_cache_update: bad packet");
#endif
	return;
    }
	
    /*
     * Is fast switching permitted?
     */
    if (!hwoutput->novell_fastokay)
	return;

    /*
     * Allow packets to be sent back out input interface if and only
     * if different network/encapsulation and all sanity checks check.
     *
     * Allow identical encaps on different subinterfaces of
     * non-broadcast media.
     *
     * Also if routing between IPX vLANs then may cache same interface.
     */

    if (hwinput == hwoutput)
	if ((pak_ptr->nif_input == NULL) ||
	    (!swoutput->novellnets) ||
	    (pak_ptr->nif_input == noutput) ||
	    (pak_ptr->nif_input->novellnet == noutput->novellnet) ||
	    ((pak_ptr->nif_input->enctype == noutput->enctype) &&
	    (((hwoutput->status & IDB_IEEEMASK) &&
	     IS_VLAN_CONFIGURED_SUBINTERFACE(swinput) == FALSE))))
	    {
		return;
	    }

    /*
     * Make sure our own node address never gets cached. This replaces a
     * check formerly in the fastswitch path. Local packets will be bumped
     * up to process level since the cache lookup is guaranteed to fail.
     */
    if (((GETSHORT(&ipx->dhost[4]) == *(ushort*)&swinput->novell_node[4]) &&
	 (GETLONG(&ipx->dhost[0])  == *(ulong *)&swinput->novell_node[0])) ||
	((GETSHORT(&ipx->dhost[4]) == *(ushort*)&swoutput->novell_node[4]) &&
	 (GETLONG(&ipx->dhost[0]) == *(ulong *)&swoutput->novell_node[0])))
	{
	    return;
	}

    /*
     * Forbid network layer broadcast unless kosher AND allowed
     * by config. Routing/SAP updates, conformant type 20, and
     * non-specific destination networks are NEVER kosher.
     */
    if ((GETLONG(ipx->dhost) == 0xffffffffL) &&
	(GETSHORT(&ipx->dhost[4]) == 0xffff)) {
  	if (novell_fastswitch_bcast &&
  	    (GETLONG(ipx->ddnet) != 0) &&
  	    (GETLONG(ipx->ddnet) != -1L) &&
  	    ((ipx->pt != NOVELL_TYPE_NETBIOS) || novell_type20_helpered) &&
  	    (ipx->dsock != NOVELL_SOCK_RIP) &&
  	    (ipx->dsock != NOVELL_SOCK_SAP) &&
  	    (ipx->dsock != NOVELL_SOCK_NLSP) &&
  	    (ipx->dsock != NOVELL_SOCK_IGRP)) {
  	    bcast = TRUE;
  	} else {
  	    return;
  	}
    }
    
    /*
     * Forbid MAC layer multicast (unless entry is kosher directed broadcast
     * or Cisco's vLAN multicast) and improper encapsulation types:
     */
    if (hwoutput->status & (IDB_ETHER|IDB_BVI)) {
	if ((pak_ptr->datagramstart[0] & NOVELL_MULTICAST_ETHER) &&
  	    !bcast && (IS_ISL_VLAN_SUBINTERFACE(swoutput) == FALSE))
	    return;

	if ((pak_ptr->enctype != ET_ARPA) &&
	    (pak_ptr->enctype != ET_NOVELL_ETHER) &&
	    (pak_ptr->enctype != ET_SAP) &&
	    (pak_ptr->enctype != ET_SNAP) &&
            (pak_ptr->enctype != ET_SDE) &&
	    (pak_ptr->enctype != ET_ISL))
	    return;

    } else if (hwoutput->status & IDB_TR) {
	if ((pak_ptr->datagramstart[2] & NOVELL_MULTICAST_TR) && !bcast) {
	    return;
	}
	if ((pak_ptr->enctype != ET_SAP) &&
	    (pak_ptr->enctype != ET_SNAP) &&
            (pak_ptr->enctype != ET_SDE))
	    return;

    } else if (hwoutput->status & IDB_FDDI) {
	if (hwoutput->type == IDBTYPE_FDDIT) {
	    if ((pak_ptr->datagramstart[1] & NOVELL_MULTICAST_ETHER) &&
  		!bcast) {
		return;
	    }
	} else if ((pak_ptr->datagramstart[1] & NOVELL_MULTICAST_TR) &&
		   !bcast) {
	    return;
	}
	
	if ((pak_ptr->enctype != ET_SAP) &&
	    (pak_ptr->enctype != ET_SNAP) &&
            (pak_ptr->enctype != ET_NOVELL_FDDI) &&
	    (pak_ptr->enctype != ET_SDE))
	    return;

    } else if (hwoutput->status & IDB_SERIAL) {
	/*
	 * Serial encaps must be at least 4 bytes due
	 * to optimizations in fast path code.
	 */
	if (pak_ptr->encsize < HDLC_ENCAPBYTES)
	    return;
	if (pak_ptr->enctype == ET_HDLC) {
	    vhdlctype *hdlc = (vhdlctype *)pak_ptr->datagramstart;
	    if (hdlc->var_hdlcflags & HDLC_BCAST)
		return;
	} else if (is_frame_relay(hwoutput)) {
	    if ((pak_ptr->enctype != ET_FRAME_RELAY) &&
		(pak_ptr->enctype != ET_FR_IETF))
		return;
	} else if ((!(is_ppp(hwoutput))) &&
		   (!(is_atm_dxi(hwoutput))) &&
		   (!(is_smds(hwoutput))))
	    return;
    } else if (hwoutput->status & IDB_LEX) {
	if ((pak_ptr->enctype != ET_ARPA) &&
	    (pak_ptr->enctype != ET_NOVELL_ETHER) &&
	    (pak_ptr->enctype != ET_SAP) &&
	    (pak_ptr->enctype != ET_SNAP))
	    return;
    } else if (!(hwoutput->status & IDB_ATM) &&
	       !is_tunnel(hwoutput))
  	return; /* Bail if not IEEE or serial hdlc or ppp or ATM or
		   tunnel*/

    /*
     * Look for interfaces that do not work correctly when an odd byte
     * start is used.
     */
    if ((hwoutput->status & (IDB_ETHER|IDB_LEX)) &&
	ether_odd_byte_start_bug(hwoutput))
	extra = 1;
    else
        extra = 0;

    /*
     * Find or create a cache pointer
     */
    newflag = FALSE;
    depth = 0;
    dnet.d.lword = GET2WD(ipx->ddnet);
    bucket =  dnet.d.lword;
    bucket ^= GET2WD(ipx->dhost);
    bucket ^= GETSHORT(ipx->dhost+4);
    bucket ^= (bucket >> 16);
    bucket ^= (bucket >> 8);
    bucket &=0xFF;
    prev_ptr = (novcachetype *)T_ADDR(bucket);
    for (cache_ptr = TABLE(bucket);
	   cache_ptr; cache_ptr = cache_ptr->novc_next_ptr, depth++){
	if (ieee_equal(ipx->dhost, cache_ptr->novc_dest_address) &&
	    (cache_ptr->novc_dest_net == dnet.d.lword))
	    break;
	prev_ptr = cache_ptr;
    }
    if (cache_ptr == NULL) {
	/* If we already have max_size cache entries we can't add this.
         * Just leave quietely. Take care of 0 as no limit.
 	 */
	if (novcache_max_size && 
	    ((novcache_entries + 1) > novcache_max_size))
	    return;
	cache_ptr = malloc(sizeof(novcachetype));
	if (cache_ptr == NULL)
	    return;
	newflag = TRUE;
	cache_ptr->novc_path_count = 0;
	cache_ptr->novc_path_list_ptr = NULL;
	cache_ptr->novc_hash_next = NULL;
	prev_ptr = NULL;
	depth = 0;
    } else
	/* check if this entry is already in synch with new info  */
        if (cache_ptr->novc_version == novcache_version)
	    return;
    /*
     * Update cache, link a new one into hash table
     */
    cache_ptr->novc_bcast = bcast;
    cache_ptr->novc_ext_cache = FALSE;
    cache_ptr->novc_dest_net = dnet.d.lword;
    ieee_copy(ipx->dhost, cache_ptr->novc_dest_address);
    GET_TIMESTAMP(cache_ptr->novc_lastupdate);
    cache_ptr->novc_active = cache_ptr->novc_inactive = 0;
    cache_ptr->novc_connected = (dnet.d.lword == noutput->novellnet);
    if (cache_ptr->novc_path_count == 0)
	novcache_hash_table_insert(cache_ptr);
    /*
     * check if we are dealing with directly connected net.
     */
    if (cache_ptr->novc_connected) {

	if (cache_ptr->novc_path_count != 1) {
	    level = raise_interrupt_level(NETS_DISABLE);
	    if (cache_ptr->novc_path_list_ptr != NULL) {
		free(cache_ptr->novc_path_list_ptr);
		cache_ptr->novc_path_list_ptr = NULL;
	    }
	    if (!(cache_ptr->novc_path_list_ptr =  
		  malloc(sizeof(novpathtype)))) {
		nov_free_entry(cache_ptr, prev_ptr);
		reset_interrupt_level(level);
		return;
	    }
	    reset_interrupt_level(level);
	}
#ifdef DEBUG
	if (novell_debug)
	    buginf("\nPopulate conn cache entry:");
#endif
	dsap = 0;

	if (hwoutput->status & (IDB_ETHER|IDB_BVI)) {
	    total_encap_bytes = ETHER_ARPA_ENCAPBYTES;
	    if (noutput->enctype == ET_SAP)
		total_encap_bytes = ETHER_SAP_ENCAPBYTES;
	    else if (noutput->enctype == ET_SNAP)
		total_encap_bytes = ETHER_SNAP_ENCAPBYTES;

	/* Also allow for ISL on Fast Ethernet here */

	    else if (noutput->enctype == ET_ISL)
		total_encap_bytes = ISL_HEADER_LENGTH + ETHER_ARPA_ENCAPBYTES;

	    delta = 0;

	} else if (hwoutput->status & IDB_FDDI) {

	    if (hwoutput->type == IDBTYPE_FDDIT)
		rii_mask = FDDIT_RII;
	    else
		rii_mask = FDDI_RII;

	    if (noutput->enctype == ET_SNAP)
		total_encap_bytes = FDDI_SNAP_ENCAPBYTES;
	    else if (noutput->enctype == ET_NOVELL_FDDI)
		total_encap_bytes = FDDI_ENCAPBYTES;
	    else if (noutput->enctype == ET_SAP)
		total_encap_bytes = FDDI_SAP_ENCAPBYTES;
	    /*
	     * Compute RIF.
	     */
	    fhdr = (fddirif_t *)pak_ptr->datagramstart;

	    if (fhdr->saddr[0] & rii_mask) {
		riflen = (fhdr->rc_blth & RC_LTHMASK);
		total_encap_bytes += riflen;
	    }

	    /* 
	     * No RIF present, so check if we're an IEEE 802.10 vLAN interface,
	     * IPX routing between vLANs supported on Ethernet, FDDI and TRing,
	     * 802.2 (SAP) and SNAP, without the Frame Control byte.
	     */

	    else if (noutput->enctype == ET_SDE) {

		if (GETSHORT(pak_ptr->datagramstart + FDDI_ENCAPBYTES +
			     SDE_ENCAPBYTES) == SNAPSNAP) {
		    total_encap_bytes = FDDI_SNAP_ENCAPBYTES +
		      SDE_ENCAPBYTES;

		} else if (GETSHORT(pak_ptr->datagramstart + FDDI_ENCAPBYTES +
				    SDE_ENCAPBYTES) == NOVELL_NO_CHECKSUM) {

		  /* set raw fddi packet to support Novell-Ether */
		    total_encap_bytes = FDDI_SNAP_ENCAPBYTES +
		      SDE_ENCAPBYTES - SNAP_HDRBYTES;

		} else {
		    total_encap_bytes = FDDI_SAP_ENCAPBYTES +
		      SDE_ENCAPBYTES - SAP_HDRBYTES;
		}
	    }
	    
	    delta = 1;

	} else if (hwoutput->status & IDB_TR) {
	    /*
	     * Compute RIF.
	     */
	    trh = (trrif_t *)pak_ptr->datagramstart;
	    total_encap_bytes = TRING_ENCAPBYTES;
	    if (trh->saddr[0] & TR_RII) {
		riflen = (trh->rc_blth & RC_LTHMASK);
		total_encap_bytes += riflen;
		dsap = *((uchar *)&trh->rc_blth + riflen);

	    } else {

                if (noutput->enctype == ET_SDE)
                    total_encap_bytes += SDE_ENCAPBYTES;

		dsap = *((uchar *)&trh->rc_blth);
	    }
	    /*
	     * Now look for SAP vs SNAP. Gah.
	     */
	    total_encap_bytes += ((dsap == SAP_SNAP) ? SNAP_HDRBYTES : SAP_HDRBYTES);
	    delta = 2;

	} else if (is_tunnel(hwoutput)) {
	    total_encap_bytes = 0;
	    delta = 0;
	} else if (hwoutput->status & IDB_ATM) {
	    total_encap_bytes = pak_ptr->encsize;
	    delta = 0;
	} else if (hwoutput->status & IDB_LEX) {
	    total_encap_bytes =
		pak_ptr->network_start - pak_ptr->datagramstart;
	    delta = 0;
	} else {
	    /*
	     * Serial
	     */
	    delta = 0;
	    total_encap_bytes = pak_ptr->encsize;
	}
	len = total_encap_bytes - delta;
	cache_pptr = cache_ptr->novc_path_next_ptr = 
	  cache_ptr->novc_path_list_ptr;
	cache_ptr->novc_path_next_ptr->novp_idb_ptr = swoutput;
	cache_ptr->novc_path_next_ptr->novp_nidb_ptr = noutput;
	cache_ptr->novc_path_next_ptr->novp_next_ptr = 
	                                  cache_ptr->novc_path_list_ptr;
	bcopy(pak_ptr->datagramstart + delta, 
	      cache_ptr->novc_path_list_ptr->novp_mh.mac_bytes, len);
	bcopy(pak_ptr->datagramstart + delta, 
	      cache_ptr->novc_path_list_ptr->novp_mh_oa.mac_bytes+1, len);
	/*
	 * Preset "wasted" odd-aligned byte to boost FS speed:
	 */
	if (hwoutput->status & IDB_FDDI)
	    cache_ptr->novc_path_list_ptr->novp_mh_oa.mac_bytes[0] = FDDI_LLC_FC_BYTE;
	else if (hwoutput->status & IDB_TR)
	    cache_ptr->novc_path_list_ptr->novp_mh_oa.mac_bytes[0] = FC_TYPE_LLC;
	/*
	 * Init descriptor portion of cache entry:
	 */
	cache_ptr->novc_path_list_ptr->rifwords = (riflen/2);
	cache_ptr->novc_path_list_ptr->total_encap_bytes = total_encap_bytes;
	cache_ptr->novc_path_list_ptr->length = len;
	cache_ptr->novc_path_list_ptr->total_encap_words =
	    btow(total_encap_bytes);
	cache_ptr->novc_path_list_ptr->extra_bytes = extra;
	cache_ptr->novc_path_list_ptr->enctype = noutput->enctype;
	cache_ptr->novc_path_list_ptr->igrp_route = FALSE;
	cache_ptr->novc_path_list_ptr->subint_reference_num =
	    reg_invoke_get_subint_ref_num_from_pak(hwoutput->enctype,
						   hwoutput, pak_ptr);
	cache_pptr->dialergroup    = dialergroup;
	cache_ptr->novc_path_count = 1;

    } else {

        /*
         * Here we are dealing with a remote net.
         * first_hop has set ipx_pak_ndb_for_cache to the ndb.
         */
        rip_rt = ipx_pak_ndb_for_cache;

	if (rip_rt) {
	    /*
	     * If we are doing per_host load balancing, we just keep one 
	     * path. Also Use the lesser of the available paths and 
	     * maximum allowable paths, because NLSP route descriptors 
	     * keep all valid paths output by SPF.
	     */
	    pathcount = nov_load_share_per_host ? 
	         1 : min(rip_rt->paths, novell_maxpaths);
	    /*
	     * If cache cannot keep all the paths we have, free whatever we
	     * have and build the cache from scratch.
	     */ 
	    if (cache_ptr->novc_path_count < pathcount) {
		level = raise_interrupt_level(NETS_DISABLE);
		if (cache_ptr->novc_path_list_ptr != NULL) {
		    free(cache_ptr->novc_path_list_ptr);
		    cache_ptr->novc_path_list_ptr = NULL;
		}
		if (!(cache_ptr->novc_path_list_ptr =
		      malloc((sizeof(novpathtype) * pathcount)))) {
		    nov_free_entry(cache_ptr, prev_ptr);
		    reset_interrupt_level(level);
		    return;
		}
		reset_interrupt_level(level);
	    }

	    cache_ptr->novc_path_count = 0;
            cache_ptr->novc_path_next_ptr = NULL;
	    /*
	     * If doing per-host load-sharing, utilize the path selected
	     * round-robin at process level. If not, select first path
	     * in list and prepare to traverse.
	     */
	    if (nov_load_share_per_host) {
		path_ptr = rip_rt->nextpath;
	    } else {
		path_ptr = rip_rt->path_list;
	    }
	    prev_pptr = cache_pptr = cache_ptr->novc_path_list_ptr;
	    cache_ptr->novc_path_next_ptr = cache_ptr->novc_path_list_ptr;
	    while (pathcount && (path_ptr != NULL)) {
		noutput = cache_pptr->novp_nidb_ptr = path_ptr->nidb;
		swoutput = cache_pptr->novp_idb_ptr = path_ptr->nidb->idb;
		enctype = noutput->enctype;
		nouthwidb = noutput->idb->hwptr;
		/*
		 * IPX has no knowledge of B channels on BRI/PRI interrfaces
		 * but is supposed to pass a good dialergroup during 
		 * DDR_FASTSWITCH. Get the right dialergroup and install.
		 */
		if ((dialer_interface = if_is_ddr(nouthwidb))) {
		    memset(&proto_addr, 0, sizeof(addrtype));
		    proto_addr.type = ADDR_NOVELL;
		    proto_addr.length = ADDRLEN_NOVELL;
		    proto_addr.nov_addr.net = path_ptr->srcnet;
		    ieee_copy(path_ptr->srchost, proto_addr.nov_addr.host);
		    dialergroup = 
		        reg_invoke_dialer_get_connected_group(nouthwidb, 
							      &proto_addr, 
							      LINK_NOVELL);
		}
		/*
		 * Evaluate eligibility for cache entry creation:
		 *
		 * - output interface cache enabled
		 * - output interface up
		 * - ddr interfaces must have a valid dialergroup
		 * - permitted if output filter present
		 *   - if normal traffic use novell_accesslist
		 */
		if ((!swoutput->hwptr->novell_fastokay) ||
		    (!interface_up(swoutput)) ||
 		    (dialer_interface && !dialergroup) ||
		    ((swoutput->novell_accesslist &&
		      (ipx_get_ext_acl(swoutput->novell_accesslist)) &&
		      (!nov_accesscheck(pak_ptr,
					swoutput->novell_accesslist))))) {
#ifdef DEBUG
		    if (novell_debug) {
			buginf("\nSkipping remote cache entry: %s [%x]",
 			       nouthwidb->hw_short_namestring, 
 			       noutput->novellnet);
 			if (dialer_interface && !dialergroup)
 			    buginf("\nInterface %s has no dialergroup, "
 				   "cannot fastswitch",
 				   nouthwidb->hw_short_namestring);
		    }
#endif
		    path_ptr = path_ptr->next;
		    if (path_ptr == NULL) {
			path_ptr = rip_rt->path_list;
		    }
		    pathcount--;
		    continue;
		}

		/*
		 * Populate each cache entry:
		 */
#ifdef DEBUG
		if (novell_debug) 
		    buginf("\nPopulate remote cache entry: %s [%x]",
			   nouthwidb->hw_short_namestring, 
			   noutput->novellnet);
#endif
		cache_pptr->dialergroup = dialergroup;
		fr_pak = NULL;

		if (nouthwidb->status & (IDB_ETHER|IDB_BVI)) {

		    if (noutput->enctype == ET_ISL) {

                        total_encap_bytes = ISL_HEADER_LENGTH +
			    ETHER_ARPA_ENCAPBYTES;

			/*
			 * Use the MAC address associated with the egressing 
			 * interface. Length field is computed by the FEIP 
			 * ucode upon transmission.
			 */
			ieee_copy(isl_vlan_multicast,
				  cache_pptr->novp_mh.mac_bytes);
			ieee_copy(path_ptr->nidb->idb->hwptr->hardware, 
				  &cache_pptr->novp_mh.mac_bytes[IEEEBYTES]);

			ieee_copy(isl_vlan_snap,
				  &cache_pptr->novp_mh.mac_bytes
				  [ETHER_8023_ENCAPBYTES]);
			/*
			 * Shift the colour field one bit position since ISL 
			 * defines the LSB to indicate whether or not the 
			 * packet is a Spanning Tree BPDU or Cisco Discovery 
			 * packet!
			 */
			PUTSHORT(&cache_pptr->novp_mh.mac_bytes
				 [ISL_VLAN_ID_OFFSET], 
				 (swoutput->isl_vlan->colour << 1));

			/*
			 * Ethernet: Set MAC destination/source fields:
			 */
			ieee_copy(path_ptr->srchost,
				  &cache_pptr->novp_mh.mac_bytes
				  [ISL_HEADER_LENGTH]);
			ieee_copy(path_ptr->nidb->idb->hwptr->hardware,
				  &cache_pptr->novp_mh.mac_bytes
				  [ISL_HEADER_LENGTH+IEEEBYTES]);
			/*
			 * Set MAC type/length field and append LLC header
			 * per the encapsulation of the output network.
			 */
			cache_pptr->novp_mh.mac_shorts
			    [total_encap_bytes/sizeof(short)-1] =
			    pak_ptr->datagramsize;		    
		    } else {
			/*
			 * Ethernet: Set MAC destination/source fields:
			 */
			ieee_copy(path_ptr->srchost,
				  cache_pptr->novp_mh.mac_bytes);
			ieee_copy(path_ptr->nidb->idb->hwptr->hardware,
				  &cache_pptr->novp_mh.mac_bytes[IEEEBYTES]);
			/*
			 * Set MAC type/length field and append LLC header
			 * per the encapsulation of the output network.
			 */
			cache_pptr->novp_mh.mac_shorts[6] = pak_ptr->datagramsize;
			
			if (noutput->enctype == ET_SAP) {
			    cache_pptr->novp_mh.mac_shorts[7] = 0xe0e0;
			    cache_pptr->novp_mh.mac_shorts[8] = 0x0300;
			    total_encap_bytes = ETHER_SAP_ENCAPBYTES;
			} else if (noutput->enctype == ET_SNAP) {
			    cache_pptr->novp_mh.mac_shorts[7]  = 0xaaaa;
			    cache_pptr->novp_mh.mac_longs[4]   = 0x03000000;
			    cache_pptr->novp_mh.mac_shorts[10] = TYPE_NOVELL1;
			    total_encap_bytes = ETHER_SNAP_ENCAPBYTES;
			} else if (noutput->enctype == ET_ARPA) {
			    cache_pptr->novp_mh.mac_shorts[6] = TYPE_NOVELL1;
			    total_encap_bytes = ETHER_ARPA_ENCAPBYTES;
			    
			} else { /* ET_NOVELL_ETHER */
			    
			    total_encap_bytes = ETHER_ARPA_ENCAPBYTES;
			}
		    }
		    delta = 0; /* Nothing to prepend on ethernet */

		} else if (nouthwidb->status & IDB_FDDI) {
		    level = raise_interrupt_level(NETS_DISABLE);
		    if (noutput->idb->hwptr->type == IDBTYPE_FDDIT)
			rii_mask = FDDIT_RII;
		    else
			rii_mask = FDDI_RII;

		    if (srb_multiring_novell_enabled(noutput->idb)) {
			rif = reg_invoke_media_rif_lookup(path_ptr->srchost,
					 hwidb_or_null(path_ptr->nidb->idb), 
					 NULL, 0);
			if (!rif) {
			    reset_interrupt_level(level);
			    path_ptr = path_ptr->next;
			    if (path_ptr == NULL) {
				path_ptr = rip_rt->path_list;
			    }
			    pathcount--;
			    continue;
			}
			riflen = rif->length;
		    } else {
			rif = NULL;
			riflen = 0;
  		    }
		    /*
		     * FDDI: Set MAC destination/source fields:
		     */
		    daddr = path_ptr->srchost;
		    if (nouthwidb->type == IDBTYPE_FDDIT) {
			ieee_copy(path_ptr->srchost, cache_pptr->novp_mh.mac_bytes);
			ieee_copy(path_ptr->nidb->idb->hwptr->hardware, 
			      &cache_pptr->novp_mh.mac_bytes[IEEEBYTES]);
		    } else {
			ieee_swap(daddr, cache_pptr->novp_mh.mac_bytes);
			ieee_copy(path_ptr->nidb->idb->hwptr->bit_swapped_hardware, 
			      &cache_pptr->novp_mh.mac_bytes[IEEEBYTES]);
		    }

		    if (riflen)
			bcopy(rif->rif, &cache_pptr->novp_mh.mac_bytes[12], riflen);
		    ptr = (ulong *)(&cache_pptr->novp_mh.mac_bytes[12] + riflen);
		    /*
		     * Set MAC type/length field and append LLC header:
		     */
		    if (enctype == ET_SNAP) {
		        PUTLONG(ptr++, 0xaaaa0300);
			PUTLONG(ptr++, TYPE_NOVELL1); /* 00008137 */
			total_encap_bytes = FDDI_SNAP_ENCAPBYTES + riflen;
		    } else if (enctype == ET_NOVELL_FDDI) {
			total_encap_bytes = FDDI_ENCAPBYTES + riflen;

		    } else if (enctype == ET_SDE) {

		      /* send raw fddi pacet to support the default
		       * Novell-Ether on ether interface.
		       */
			total_encap_bytes = FDDI_SNAP_ENCAPBYTES +
			  SDE_ENCAPBYTES - SNAP_HDRBYTES;

		        (uchar *) ptr = (&cache_pptr->novp_mh.mac_bytes[12]);

                        /* Dot10 LLC Type 1, SAID, Station ID */

                        PUTSHORT(ptr, (SAP_SDE | SAP_SDE << 8));
                        (uchar *) ptr += 2;
                        PUTSHORT(ptr, (LLC1_UI << 8));
                        (uchar *) ptr += 1;
                        PUTLONG(ptr, noutput->idb->sde_said_db->sdb_said);
                        (uchar *) ptr += 4;
                        ieee_copy(path_ptr->nidb->idb->hwptr->hardware,
				  (uchar *)ptr);

		    } else { /* ET_SAP */

		        PUTLONG(ptr, 0xe0e00300);
			total_encap_bytes = FDDI_SAP_ENCAPBYTES +riflen;
		    }
		    /*
		     * Account for prepended FDDI frame control byte:
		     */
		    delta = 1;
		    reset_interrupt_level(level);	
		    if (riflen)
			cache_pptr->novp_mh.mac_bytes[IEEEBYTES] |= rii_mask;

		} else if (nouthwidb->status & IDB_TR) {

		    /*
		     * Token Ring: Possibly lookup RIF:
		     */

		    level = raise_interrupt_level(NETS_DISABLE);
		    /*
		     * If multiring is enabled on this interface for the
		     * protocol in question, look up a RIF. If it is not,
		     * then don't look up the RIF.
		     */
		    if (srb_multiring_novell_enabled(noutput->idb)) {
			rif = reg_invoke_media_rif_lookup(
                                         path_ptr->srchost,
					 hwidb_or_null(path_ptr->nidb->idb), 
					 NULL, 0);
			if (!rif) {
			    reset_interrupt_level(level);
			    path_ptr = path_ptr->next;
			    if (path_ptr == NULL) {
				path_ptr = rip_rt->path_list;
			    }
			    pathcount--;
			    continue;
			}
			riflen = rif->length;
		    } else {
			rif = NULL;
			riflen = 0;
  		    }
		    /*
		     * Set MAC destination/source field and "lay down a RIF",
		     * if present. Update cache entry with appropriate
		     * rif length.
		     */
		    ieee_copy(path_ptr->srchost, cache_pptr->novp_mh.mac_bytes);
		    ieee_copy(path_ptr->nidb->idb->hwptr->hardware,
			  cache_pptr->novp_mh.mac_bytes+IEEEBYTES);
		    if (riflen)
			bcopy(rif->rif, &cache_pptr->novp_mh.mac_bytes[12], riflen);
		    ptr = (ulong *)(&cache_pptr->novp_mh.mac_bytes[12] + riflen);
		    /*
		     * Append LLC header:
		     */
		    if (enctype == ET_SAP) {
			/*
			 * Novell. 802.2 with Novell's SAP code.
			 */
			ieee_copy(path_ptr->srchost, cache_pptr->novp_mh.mac_bytes);
			ieee_copy(path_ptr->nidb->idb->hwptr->hardware,
			      cache_pptr->novp_mh.mac_bytes+IEEEBYTES);
			if (riflen)
			  bcopy(rif->rif, &cache_pptr->novp_mh.mac_bytes[12], riflen);

			/* Note possible short word alignment depending on RIF */
			PUTLONG(ptr, ((SAP_NOVELL << 24) | (SAP_NOVELL << 16)  | (LLC1_UI << 8)));
			total_encap_bytes = TRING_SAP_ENCAPBYTES + riflen;
		    } else {
		        /*
			 * Novell 802.2 snap
			 *
			 * Note possible short word alignment depending on RIF
			 */
		        PUTLONG(ptr++, 0xaaaa0300);
			PUTLONG(ptr++, TYPE_NOVELL1);
			total_encap_bytes = TRING_SNAP_ENCAPBYTES + riflen;
		    }
		    /*
		     * Account for prepended TR AC/FC bytes:
		     */
		    delta = 2;
		    reset_interrupt_level(level);
		    if (riflen)
			cache_pptr->novp_mh.mac_bytes[IEEEBYTES] |= TR_RII;
		} else if (nouthwidb->status & IDB_ATM) {
 		    /*
 		     * If the outgoing packet corresponds to this path
 		     * entry, then go ahead and use the encaps in the
 		     * packet.  If not, then like frame relay below,
 		     * we need to do something pretty kludgey.  Allocate
 		     * a dummy packet and fake atm into encapsulating
 		     * it.
 		     */
 		    if (pak_ptr->nif_output == noutput) {
 			total_encap_bytes = pak_ptr->encsize;
 			bcopy(pak_ptr->datagramstart, cache_pptr->novp_mh.mac_bytes,
 			      total_encap_bytes);
 			delta = 0;
 		    } else {
 			atm_pak = pak_clip(pak_ptr, 32);  /* arbitrary len */
 			if (atm_pak == NULL) {
 			    path_ptr = path_ptr->next;
 			    pathcount--;
 			    continue;
 			}
 			/*
 			 * Make the new packet look like we're going to output
 			 * it on this path, so that we'll generate the correct
 			 * encaps, and lookup the correct static maps etc.
 			 */
 			atm_pak->if_output = noutput->idb;
 			atm_pak->nif_output = noutput;
 
 			/*
 			 * The ENCAPS_ONLY flag tells the encaps routine that this
 			 * is a dummy packet.  We want it to go ahead and encapsulate
 			 * it, but we don't it want it to add the results to the
 			 * fastswitch cache (otherwise we'll end up back in this
 			 * routine in a recursion loop), and we don't want it to
 			 * count the packet in any stats, since we never intend
 			 * outputting it.
 			 */
 			atm_pak->flags |= PAK_ENCAPS_ONLY;
 			if (!pak_encap(noutput->idb->hwptr, atm_pak,
				       SMALLDATA, (long)path_ptr->srchost)) {
 			    retbuffer(atm_pak);
 			    path_ptr = path_ptr->next;
 			    pathcount--;
 			    continue;
 			}
 			total_encap_bytes = atm_pak->encsize;
 			bcopy(atm_pak->datagramstart, cache_pptr->novp_mh.mac_bytes,
 			      total_encap_bytes);
 			delta = 0;
 			retbuffer(atm_pak);
 		    }
		} else if (nouthwidb->status & IDB_LEX) {
		    total_encap_bytes = pak_ptr->network_start -
			pak_ptr->datagramstart;
		    bcopy(pak_ptr->datagramstart, cache_pptr->novp_mh.mac_bytes,
			  total_encap_bytes);
		    delta = 0;
		} else {
		    /*
		     * Serial HDLC/PPP/FR:
		     */
		    total_encap_bytes = delta = 0;
		    if (pak_ptr->nif_output == noutput) {
			/*
			 * Trigger packet - just copy the encap from the
			 * output packet
			 */
			total_encap_bytes = pak_ptr->encsize;
			bcopy(pak_ptr->datagramstart,
			      cache_pptr->novp_mh.mac_bytes,
			      total_encap_bytes);
		    } else {
			if (is_frame_relay(nouthwidb) ||
			    is_atm_dxi(nouthwidb) ||
			    is_smds(nouthwidb)) {
			    /*
			     * Output hwidb is frame relay. If the outgoing
			     * packet corresponds to this path entry, use
			     * the encapsulation present in this packet.
			     * If not then we have a problem since we have
			     * no *good* way to generate it unless we import
			     * details of frame relay encapsulation into
			     * this unit. Get around this via a cumbersome
			     * but reliable method ... allocate a dummy
			     * packet and fake fr into encapsulating it.
			     *
			     * Gross, but do it for atm_dxi also
			     * and smds
			     */
			    fr_pak = pak_clip(pak_ptr,32);
			    if (fr_pak == NULL) {
				path_ptr = path_ptr->next;
				if (path_ptr == NULL) {
				    path_ptr = rip_rt->path_list;
				}
				pathcount--;
				continue;
			    }
			    fr_pak->if_output = noutput->idb;
			    fr_pak->flags |= PAK_ENCAPS_ONLY;
			    if (!pak_encap(nouthwidb, fr_pak, SMALLDATA,
					   (long)path_ptr->srchost)) {
				retbuffer(fr_pak);
				path_ptr = path_ptr->next;
				pathcount--;
				fr_pak = NULL;
				continue;
			    }
			    total_encap_bytes = fr_pak->encsize;
			    bcopy(fr_pak->datagramstart,
				  &cache_pptr->novp_mh.mac_bytes[delta],
				  total_encap_bytes);
			    enctype = fr_pak->enctype;
			} else {
			    if (nouthwidb->cache_board_encap) {
				delta =
				    (*nouthwidb->cache_board_encap)
					(nouthwidb,
					 cache_pptr->novp_mh.mac_bytes,
					 MAXMAC_BYTES - total_encap_bytes);
#ifdef DEBUG
				if (delta == -1) {
				    buginf("illegal encap size");
				    delta = 0;
				    total_encap_bytes = 0;
				} 
#endif
			    }
			    if (nouthwidb->enctype == ET_HDLC) {
				total_encap_bytes = HDLC_ENCAPBYTES;
				PUTLONG(&cache_pptr->novp_mh.mac_bytes[delta], 
					HDLC_NOVCODE);
			    } else if (nouthwidb->enctype == ET_PPP) {
				total_encap_bytes = PPP_ENCAPBYTES;
				PUTLONG(&cache_pptr->novp_mh.mac_bytes[delta], 
					PPP_NOVCODE);
			    }
			    total_encap_bytes += delta;
			    delta = 0;
			}
		    }
		}

		len = total_encap_bytes - delta;
		bcopy(cache_pptr->novp_mh.mac_bytes,
		      cache_pptr->novp_mh_oa.mac_bytes+1, len);

		/*
		 * Preset "wasted" odd-aligned byte to boost FS speed:
		 */
		if (nouthwidb->status & IDB_FDDI)
		    cache_pptr->novp_mh_oa.mac_bytes[0] = FDDI_LLC_FC_BYTE;
		else if (nouthwidb->status & IDB_TR)
		    cache_pptr->novp_mh_oa.mac_bytes[0] = FC_TYPE_LLC;
		/*
		 * Init descriptor portion of cache entry:
		 */
		cache_pptr->rifwords = (riflen/2);
		cache_pptr->length = len;
		cache_pptr->total_encap_words = btow(total_encap_bytes);
		cache_pptr->total_encap_bytes = total_encap_bytes;
		cache_pptr->extra_bytes = extra;
		cache_pptr->enctype = enctype;
		cache_pptr->igrp_route = (rip_rt->flags & NET_IGRP);
		cache_pptr->subint_reference_num =
		    reg_invoke_get_subint_ref_num_from_pak(nouthwidb->enctype,
							   nouthwidb,
							   fr_pak == NULL ?
							   pak_ptr : fr_pak);
 		/* Make the cache next ptr match the rt next ptr */
		if (rip_rt->nextpath == path_ptr)
		    cache_ptr->novc_path_next_ptr = cache_pptr;
		cache_ptr->novc_path_count++;
 		if (nov_load_share_per_host) {
 		    pathcount = 0;
 		} else {
 		    pathcount--;
 		}
		prev_pptr->novp_next_ptr = cache_pptr;
		prev_pptr = cache_pptr;
		cache_pptr++;
		path_ptr = path_ptr->next;
		if (fr_pak)
		    retbuffer(fr_pak);
	    }
	    prev_pptr->novp_next_ptr = cache_ptr->novc_path_list_ptr;
            if (cache_ptr->novc_path_next_ptr == NULL)
	        cache_ptr->novc_path_next_ptr = cache_ptr->novc_path_list_ptr;
	}
    }
    if (!(cache_ptr->novc_path_count)) {
	nov_free_entry(cache_ptr, prev_ptr);
	return;
    }
    if (newflag) {
	status = raise_interrupt_level(NETS_DISABLE);
        cache_ptr->novc_next_ptr = novcache[bucket];
	novcache[bucket] = cache_ptr;
	reset_interrupt_level(status);
    } else if (depth > NOVRCACHE_DEPTH) {
	status = raise_interrupt_level(NETS_DISABLE);
	prev_ptr->novc_next_ptr = cache_ptr->novc_next_ptr;
	cache_ptr->novc_next_ptr = novcache[bucket];
	novcache[bucket] = cache_ptr;
	reset_interrupt_level(status);
    }
    /*
     * Call cbus_ipxcache_update() to enter this entry into the cBus
     * cache for autonomous switching.		rlfs	2/17/93
     *
     * Make sure that the input interface is AS capable and the
     * output interface is AS enabled. NEVER upload a broadcast
     * entry, since at present, AS does not screen out packets
     * that should be sent to process level (helpered traffic,
     * bogus routing updates, etc.)
     */
    if ((hwinput->status & IDB_CBUS) &&
	(pak_ptr->if_output->hwptr->novell_routecache & NOVELL_RCACHE_CBUS) &&
  	(!cache_ptr->novc_bcast)) {
  	pak_ptr->cacheptr = cache_ptr;
	reg_invoke_auto_sw_ipx_cache_update(pak_ptr);

#ifdef DEBUG
    if (novell_debug) 
	buginf("\nnov_cache_update: cbus update %s[%x]->%s[%x]",
	       swinput->short_namestring,
	       pak_ptr->nif_input->novellnet,
	       pak_ptr->if_output->short_namestring,
	       pak_ptr->nif_output->novellnet);
#endif

    }
    cache_ptr->novc_version = novcache_version;
    novcache_valid++;
    if (newflag) {
	novcache_entries++;
    }
}

/*
 * novcache_ager
 * Age Novell routing cache, or flush it entirely.
 */

static void novcache_ager (boolean flushflag)
{
    novcachetype *cache_ptr, *pred;
    int i, status;
    ulong inactive_rate, update_rate;
    boolean sse_active;

    if (!novell_running)
	return;

    /* Prepare to rate limit valid entry aging: */
    inactive_rate = novcache_inactive_age_rate;
    if (inactive_rate == 0) {
	inactive_rate = MAXUINT;
    }
    update_rate = novcache_update_age_rate;
    if (update_rate == 0) {
	update_rate = MAXUINT;
    }
    sse_active = (registry_populated(REG_SSE) &&
		  reg_invoke_sse_present() &&
		  novell_sse_sw_active());

    status = raise_interrupt_level(NETS_DISABLE);
    for (i = 0; i < XNS_HASHLEN; i++) {
	pred = (novcachetype *)&novcache[i];
	cache_ptr = pred->novc_next_ptr;
	while (cache_ptr) {
	    if (flushflag ||
		((novcache_version != cache_ptr->novc_version) &&
		 (CLOCK_OUTSIDE_INTERVAL(cache_ptr->novc_lastupdate, NOVRCACHE_TIMEOUT)))){
		nov_free_entry(cache_ptr, pred);
		cache_ptr = pred;
	    } else {
		/*
		 * Update inactivity flag based on activity in last minute:
		 */
		if (!cache_ptr->novc_active) {
		    cache_ptr->novc_inactive++;
		} else {
		    cache_ptr->novc_inactive = 0;
		}
		/*
		 * Cache entry valid. Invalidate if inactive (unless entry
		 * in use by autonomous switching or silicon switching
		 * active). Note that in the autonomous switching case
		 * the fast cache entry is always inactive, but must be
		 * present to allow for autonomous cache maintenance.
		 */
		if (inactive_rate && novcache_max_inactive_age &&
		    (novcache_version == cache_ptr->novc_version) &&
		    (cache_ptr->novc_inactive >= novcache_max_inactive_age) &&
		    !cache_ptr->novc_ext_cache && !sse_active) {
		    cache_ptr->novc_version--;
		    if (novcache_valid > 0)
			novcache_valid--;
		    inactive_rate--;
		}
		/*
		 * Cache entry valid, but aging. Invalidate inactive
		 * entries if sufficiently old. This helps to release
		 * RP memory when SP entry is inactive, since the
		 * autonomous cache is not aged otherwise.
		 */
		if (update_rate && novcache_max_update_age &&
		    (novcache_version == cache_ptr->novc_version) &&
		    cache_ptr->novc_inactive &&
		    CLOCK_OUTSIDE_INTERVAL(cache_ptr->novc_lastupdate,
					   novcache_max_update_age)) {
		    cache_ptr->novc_version--;
		    if (novcache_valid > 0) {
			novcache_valid--;
		    }
		    if (cache_ptr->novc_ext_cache) {
			reg_invoke_auto_sw_ipx_cache_invalidate(cache_ptr);
		    }
		    update_rate--;
		}
		cache_ptr->novc_active = 0; /* Reset activity flag */
	    }
	    pred = cache_ptr;
	    cache_ptr = cache_ptr->novc_next_ptr;
	}
    }
    /*
     * Force cbus flush if this is a user initiated clear command
     * and IPX AS configured on any active interface.
     */
    if (flushflag) {
	novcache_entries = 0L;
	novcache_valid = 0;
	if (novell_auto_sw_active())
	    reg_invoke_auto_sw_cache_init(LINK_NOVELL);
    }
    reset_interrupt_level(status);
}

/*
 * nov_fscache_age
 * Age NOVELL routing cache
 */

void nov_fscache_age (void)
{
    novcache_ager(FALSE);
}

/*
 * nov_fscache_flush
 * Flush NOVELL routing cache
 */

void nov_fscache_flush (void)
{
    novcache_ager(TRUE);
}

/*
 * show_nov_cache
 * Display contents of NOVELL routing cache
 */

static const char nov_xstring[] = "\n              Destination  Port     MAC Header\n";

void show_nov_cache (void)
{
    novcachetype *cache_ptr;
    int i, j, linect,depth, length;
    boolean onceflag;
    ushort count;
    novpathtype *path_ptr, *tmp_ptr, *first_ptr;

    automore_enable(NULL);
    printf("\nIPX routing cache version is %lu, %lu entries, %lu valid",
	   novcache_version, novcache_entries, novcache_valid);

    if (nov_load_share_per_host && (novell_maxpaths > 1)) {
	printf("\n  Per-host load sharing is in effect");
    }

    automore_header(nov_xstring);
    for (i = 0, linect = 2, onceflag = FALSE; i < XNS_HASHLEN; i++) {
	for (cache_ptr = novcache[i], depth = 0; cache_ptr; depth++, 
	     cache_ptr = cache_ptr->novc_next_ptr) {
	    if (!validmem(cache_ptr))
	        goto done;
	    mem_lock(cache_ptr);
            printf("%c%-9q.%e ",
	     ((cache_ptr->novc_version == novcache_version) ? '*' : ' '),
	      NOVELL_TYPE, cache_ptr->novc_dest_net, cache_ptr->novc_dest_address);
	    first_ptr = path_ptr = cache_ptr->novc_path_list_ptr;
	    if (!validmem(first_ptr)) {
		free(cache_ptr);
		goto done;
	    }
	    mem_lock(first_ptr);
	    tmp_ptr = cache_ptr->novc_path_next_ptr;
	    for (count = cache_ptr->novc_path_count; count;
		 count--, path_ptr++) {
		if (cache_ptr->novc_path_count != 1)
		    printf("%c%8s ", ((path_ptr == tmp_ptr) ? '@' : ' '),
		                      path_ptr->novp_idb_ptr->short_namestring);
		else
		    printf(" %8s ",path_ptr->novp_idb_ptr->short_namestring);
		length = path_ptr->length;

		/*
		 * If this IPX cache entry has a lengthy vLAN encapusulation
		 * then save some real estate on display!
		*/

		if (IS_ISL_VLAN_SUBINTERFACE(path_ptr->novp_idb_ptr) &&
                    ieee_equal(path_ptr->novp_mh.mac_bytes,
			       isl_vlan_multicast)) {

                    printf("ISL ");
                    for (j = ISL_HEADER_LENGTH; j < length; j++) {

                        printf("%-02x", path_ptr->novp_mh.mac_bytes[j]);
                    }

		} else if (IS_DOT10_VLAN_SUBINTERFACE(path_ptr->novp_idb_ptr)) {

                    printf("802.10 ");
                    for (j = 0; j < (2 * IEEEBYTES); j++) {

                        printf("%-02x", path_ptr->novp_mh.mac_bytes[j]);
                    }

	        } else {

  		    for (j = 0; j < length; j++) {
		        printf("%-02x", path_ptr->novp_mh.mac_bytes[j]);
		        if (j == 23)
			    printf("\n%36c",' ');
		    }
	        }

		printf("\n");
		if (count - 1)
		    printf("%26c",' ');
		if (automore_quit()) {
		    free(first_ptr);
		    free(cache_ptr);
		    goto done;
		}
	    }
	    free(first_ptr);
	    free(cache_ptr);
	}
    }
  done:
    automore_disable();
}

/*
 * show_nov_cache_hash
 * Display contents of NOVELL routing cache hash table
 */

static const char nov_hashstring[] = "\nhash/index   version      destination          paths\n";

void show_nov_cache_hash_table (void)
{
    novcachetype *hash;
    int i, count;

    automore_enable(NULL);
    printf("\nIPX routing cache version is %lu, %lu entries",
	   novcache_version, novcache_entries);

    automore_header(nov_hashstring);
    for (i = 0; i < XNS_HASHLEN; i++) {
	for (hash = (*novcache_hashtable)[i], count = 1; hash;
	     hash = hash->novc_hash_next, count++) {
	    if (!validmem(hash))
		goto done;
	    mem_lock(hash);
	    printf("\n%c%-3d/%-5d   %-7d%-10q.%e    %d",
		   ((hash->novc_version == novcache_version) ? '*' : ' '),
		   i, count, hash->novc_version,
		   NOVELL_TYPE, hash->novc_dest_net, hash->novc_dest_address,
		   hash->novc_path_count);
	    if (automore_quit()) {
		free(hash);
		goto done;
	    }
	    free(hash);
	}
    }
  done:
    automore_disable();
}

/*
 * show_nov_cache_history
 *
 * Display IPX cache invalidation history internals
 *
 */
void
show_nov_cache_history (parseinfo *csb)
{
    int i,j;

#ifdef NOVCACHE_STATS
    if (GETOBJ(int,1)) {
	novcache_trigger = GETOBJ(int,1);
	novcache_trigger_index = 0;
	for (i = 0; i < NOVCACHE_HISTORY_MAX; i++) {
	    novcache_trigger_reason[i] = 0;
	    TIMER_STOP(novcache_trigger_time[i]);
	}
	return;
    } else {
	automore_enable(NULL);
	printf("\nCache invalidation summary at time %TN, version = %lu:",
	       novcache_version);
	for (i = 0; i < NOVCACHE_REASON_MAX; i++) {
	    if (novcache_reason[i] == 0)
		continue;
	    printf("\nReason %-02d: count = %d, param = %-08x, time = %TN",
		   i, novcache_reason[i],
		   novcache_param[i],
		   novcache_time[i]);
	}
	printf("\n---");
	printf("\nhistory:");
	for (i = 0; i < NOVCACHE_HISTORY_MAX; i++) {
	    j = ((novcache_history_index + i) % NOVCACHE_HISTORY_MAX);
	    if (!TIMER_RUNNING(novcache_history_time[j]))
		continue;
	    printf("\nparam = %-08x, reason = %d, time = %TN",
		   novcache_history_param[j],
		   novcache_history_reason[j],
		   novcache_history_time[j]);
	}
	printf("\n---");
	printf("\nspecific history, trigger = %-08x:", novcache_trigger);
	for (i = 0; i < NOVCACHE_HISTORY_MAX; i++) {
	    j = ((novcache_trigger_index + i) % NOVCACHE_HISTORY_MAX);
	    if (!TIMER_RUNNING(novcache_trigger_time[j]))
		continue;
	    printf("\nreason = %d, time = %TN",
		   novcache_trigger_reason[j],
		   novcache_trigger_time[j]);
	}
	automore_disable();
    }
#endif
}


/*
 * nov_adjust_fastflags
 *
 * Set fastswitch input flags associated with the encaps present on a
 * particular (software) interface. The flag indicates whether incoming
 * packets with the specified encapsulation may be fastswitched.
 *
 * When a valid network with a particular encapsulation is configured on the
 * interface, the corresponding flag is set to TRUE. Conversely, if no network
 * with the encapsulation, the flag will be FALSE. This prevents leakage of
 * such packets even when the interface is up and valid networks exist. Note
 * that if a network is set to down either as a result of the "novell down"
 * command, or "shut" command, the flag will become FALSE.
 *
 * If we have a vLAN encapsulation present, then allow *any* (interior)
 * encapsulation, to avoid a specific check when Fast Switching.
 */

void nov_adjust_fastflags (idbtype *swidb)
{
    hwidbtype *hwidb;
    nidbtype *nidb;
    boolean inflag;

    hwidb = swidb->hwptr;
    for (nidb = (nidbtype *) novidbQ.qhead; nidb != NULL; nidb = nidb->next) {
	if (nidb->idb == swidb) {
	    inflag = (novell_running && !novell_shutting_down &&
		      interface_up(swidb) && NOVELL_NIDB_ISUP(nidb));
	    switch (nidb->enctype) {
	      case ET_NOVELL_ETHER:
	      case ET_NOVELL_FDDI:
		hwidb->novell_ether_ok = inflag;
		break;
	      case ET_ARPA:
		hwidb->novell_arpa_ok = inflag;
		break;
	      case ET_SAP:
		hwidb->novell_iso1_ok = inflag;
		break;
	      case ET_SNAP:
		hwidb->novell_iso2_ok = inflag;
		break;

	      case ET_SDE:
	      case ET_ISL:
		hwidb->novell_ether_ok = inflag;
		hwidb->novell_arpa_ok = inflag;
		hwidb->novell_iso1_ok = inflag;
		hwidb->novell_iso2_ok = inflag;
		break;

	      default:
		break;
	    }
	}
    }
}
    

/*
 * nov_fastsetup
 *
 * Turn on use of fast switching. If fastswitching is allowed, set
 * "idb->nov_fastokay" to TRUE and the appropriate bit(s) in the
 * "fast_switch_flag". Note that novcache_increment_version() is called
 * unconditionally.
 *
 * Note: fast/auto switch "configured" state is "idb->novell_routecache"
 *                           "enabled" state is "idb->novell_fastokay"
 */

void nov_fastsetup (hwidbtype *idb)
{
    boolean fastflag;
    hwidbtype *hwidb;
    idbtype *swidb;

    fastflag = reg_invoke_fast_supported(LINK_NOVELL, idb);

    if (!router_enable || idb->comp_db || idb->ppp_compression)
	fastflag = FALSE;

    idb->novell_fastokay = (fastflag && idb->novell_routecache && novell_running && !novell_shutting_down);
    
    if (idb->novell_fastokay) {
	idb->fast_switch_flag |= (FSF_NOVELL1 | FSF_NOVELL2);
	reg_invoke_register_drivers((FSF_NOVELL1 | FSF_NOVELL2), idb);
     } else {
	idb->fast_switch_flag &= ~(FSF_NOVELL1 | FSF_NOVELL2);
	FOR_ALL_HWIDBS(hwidb)
	    if (hwidb->novell_fastokay)
		break;
	if (!hwidb) {
	    reg_invoke_register_drivers((FSF_NOVELL1 | FSF_NOVELL2), idb);
	}
    }
    /*
     * Clear out then recompute FS input flags on this interface:
     */
    idb->novell_ether_ok = FALSE;
    idb->novell_arpa_ok  = FALSE;
    idb->novell_iso1_ok  = FALSE;
    idb->novell_iso2_ok  = FALSE;

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	if (swidb->novellnets)
	    nov_adjust_fastflags(swidb);
    }
    novcache_increment_version(NULL, NOVCACHE_FAST_SETUP, (ulong)idb, 
			       NOV_NETMASK_ALL);
}
