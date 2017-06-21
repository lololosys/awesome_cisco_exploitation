/* $Id: ipfast.c,v 3.15.6.24 1996/09/05 23:06:17 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast.c,v $
 *------------------------------------------------------------------
 * ipfast.c -- Device Independent IP Fast Switching Support
 *
 * 20-March-1988, Kirk Lougheed
 * 26-May-1994, Paul Traina
 *
 * Copyright (c) 1988-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast.c,v $
 * Revision 3.15.6.24  1996/09/05  23:06:17  snyder
 * CSCdi68126:  declare some stuff in ip const
 *              228 out of data, only 4 from image
 * Branch: California_branch
 *
 * Revision 3.15.6.23  1996/08/28  12:49:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.15.6.22  1996/08/14  14:01:32  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * enable multilink half-fastswitching on _les but disable on _mci
 * and _pas platforms
 *
 * Revision 3.15.6.21  1996/08/06  08:27:30  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.15.6.20  1996/06/29  00:11:22  snyder
 * CSCdi61694:  change some char * to char [] in ip
 * Branch: California_branch
 *     save 116 bytes
 *
 * Revision 3.15.6.19  1996/06/24  16:04:44  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.15.6.18  1996/06/17  23:32:21  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.15.6.17  1996/06/17  08:32:40  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.15.6.16  1996/06/01  00:41:42  ahh
 * CSCdi59203:  CSCdi55722 prevents flow switching from being enabled
 * Don't prevent reg_invoke_register_drivers() from being called
 * when fast-switching configuration changes.
 * Branch: California_branch
 *
 * Revision 3.15.6.15  1996/05/28  16:13:40  xliu
 * CSCdi58788:  code changes for vip named access list and encryption perf
 * Branch: California_branch
 *
 * Revision 3.15.6.14  1996/05/17  11:24:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.14.2.4  1996/05/05  23:12:12  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.14.2.3  1996/05/02  22:05:16  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.2  1996/04/03  14:41:57  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.14.2.1  1996/03/17  17:52:29  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.15.6.13  1996/05/12  23:09:27  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.15.6.12  1996/05/09  14:30:58  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.15.6.11  1996/05/04  01:03:18  wilber
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
 * Revision 3.15.6.10  1996/04/30  21:17:53  dkerr
 * CSCdi43050:  IP lookup bottleneck for RSP in ISP test setup
 * Branch: California_branch
 *
 * Revision 3.15.6.9  1996/04/30  05:44:05  gchristy
 * CSCdi56163:  SSE: move low_mem function to system registry
 * Branch: California_branch
 *
 * Revision 3.15.6.8  1996/04/29  09:20:46  gchristy
 * CSCdi50875:  SSE: handle low memory conditions more gracefully
 * Branch: California_branch
 *
 * Revision 3.15.6.7  1996/04/29  05:20:24  pst
 * CSCdi55722:  Flapping Interface causes persistent full cache
 * invalidations
 * Branch: California_branch
 *
 * Revision 3.15.6.6  1996/04/27  05:51:30  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.15.6.5.4.1  1996/04/27  07:04:47  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.15.6.5  1996/04/19  15:41:32  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.15.6.4  1996/04/15  21:16:21  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.15.6.3  1996/04/02  03:44:18  fsunaval
 * CSCdi53238:  Remove IDB_CBUS from the status field of new port adapter.
 * Branch: California_branch
 * 1.Remove IDB_CBUS from the idb->status field of port adapters.
 *
 * Revision 3.15.6.2.6.1  1996/04/08  01:55:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.15.6.2  1996/03/21  22:50:14  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.15.6.1  1996/03/18  20:16:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.15.10.1  1996/03/22  09:40:13  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.15  1996/03/04  15:30:57  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Better show output.  Also allow flow stats to be exported.
 *
 * Revision 3.14  1996/02/12  18:17:53  abaindur
 * CSCdi48693:  show process mem does not include alternate mempool mallocs
 *
 * Revision 3.13.8.1  1996/03/05  06:14:26  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.13.4.1  1996/02/27  21:09:01  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.13  1996/02/01  06:04:28  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.12  1996/01/23  08:49:46  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.11  1996/01/19  03:02:45  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.10  1996/01/19  00:57:32  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.9  1996/01/11  03:51:03  kao
 * CSCdi43878:  IS_VIP() macro should be used
 *
 * Revision 3.8.2.1  1996/02/15  21:50:28  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additonal swidb parameter to the registry bump_cache_version
 *
 * Revision 3.8  1996/01/06  08:50:26  dlobete
 * CSCdi46092:  %IPC_RSP_CBUS-3-NOBUF message on RSP with VIP under traffic
 * Only update the Distributed and Autonomous IP caches once for each
 * new prefix.
 *
 * Revision 3.7  1995/11/30  20:39:13  dlobete
 * CSCdi43408:  two fields overlap in sho ip igmp group
 * Fix up "sho ip cache" for long VIP interface names.
 *
 * Revision 3.6  1995/11/26  08:15:48  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *         "no ip address" should not be generated on subinterfaces,
 *         creates backwards compatibility problems with earlier releases.
 *
 *         Explicitly indicate cache entries that are associated with a
 *         dot10 vLAN subinterface.
 *
 * Revision 3.5  1995/11/20  01:48:56  shj
 * CSCdi43507:  do not cache IP with non-arpa (snap) encapsulation
 * Prevent non-arpa encaps from getting into route-cache for LANE
 * interfaces.
 *
 * Revision 3.4  1995/11/18  02:17:46  pst
 * CSCdi43519: Add ability to public code to query scheduler for
 * suspendability.
 *
 * Revision 3.3  1995/11/17  09:34:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:34  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:56:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.14  1995/11/08  21:04:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.13  1995/10/23  22:08:26  pst
 * CSCdi42608: improve the comments about how the ager works
 *
 * Revision 2.12  1995/10/23  18:33:31  pst
 * CSCdi42608:  allow IP cache ager to be shut down, make the
 *              "ip cache-ager" and "ip cache-invalidate-delay"
 *              commands internal only.
 *
 * Revision 2.11  1995/10/09  20:21:00  pst
 * CSCdi41527:  IP route cache code can shut down SSE
 *
 * Revision 2.10  1995/09/11  23:49:21  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.9  1995/09/06 17:28:01  pst
 * CSCdi39840:  IP route cache maintenance needs more subtle invalidation
 * code
 *
 * Revision 2.8  1995/08/23  18:17:06  fred
 * CSCdi39010:  WFQ + ip route-cache cbus on FSIP fails
 * Disable WFQ when ssp/rsp/sse switching IP
 *
 * Revision 2.7  1995/08/05  08:39:23  lbustini
 * CSCdi38136:  Incorrect output interface selected when fastswitching
 * over rotary group.
 * When a ddr link comes up, use the authenticated name along with
 * the phone number to set up the route cache entry.
 *
 * Revision 2.6  1995/07/29  01:55:52  dino
 * CSCdi36878:  Fast multicast bypasses access lists, accounting, policy
 *              routing
 *
 * Revision 2.5  1995/07/12  07:18:46  tli
 * CSCdi36426:  Policy routing will not disable on an interface
 * - Policy routed packets were creating fastswitching cache entries.
 *   That's not till _next_ release.
 * - Add counters to record packet and byte count on a per route map entry
 *   basis.
 * - Marketing identified a catastrophic deficiency.  Implemented "set ip
 *   tos" and "set ip precedence" to cover this deficiency.
 *
 * Revision 2.4  1995/06/21  08:54:58  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/21  03:28:59  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/19  01:06:32  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:58:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <stdarg.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "media_registry.h"
#include "logger.h"
#include "../ip/msg_ipfast.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "access.h"
#include "../ui/debug.h"
#include "../os/free.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../ip/ip_registry.h"
#include "../ip/ip_debug_flags.h"
#include "../iprouting/route.h"
#include "../iprouting/iprouting_debug.h"
#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../if/channel.h"
#include "../if/network.h"
#include "../hes/sse_registry.h"
#include "../lane/lane.h"
#include "../iprouting/parser_defs_iprouting.h"
#include "../wan/serial.h"
#include "../vlan/vlan.h"
#include "../wan/dialer.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"
#include "subsys.h"


/*
 * Global (ick) variables
 */
rnhtype *ipcache_head;			/* public so switch code can read it */
ulong ipcache_refcounts;                /* delete this when flow shit works. */
const char ipcache_hdr[] =
	"Prefix/Length       Age       Interface       Next Hop\n";

/*
 * Local definitions
 */
#define	WALK_CONTINUE	(0)
#define	WALK_ABORT	(-1)

/*
 * Local storage
 */
static queuetype ipcache_garbageQ;	/* a place to hang smelly underware */
static ulong ipcache_entries;		/* valid entries in cache */

static int ipcache_ager_pid;		/* pid of ager process */
static int ipcache_ager_stepover;	/* entries until next target */
static int ipcache_ager_stepinterval;	/* entries between targets */
static int ipcache_ager_examined;	/* entries examined by ager */
static int ipcache_ager_reaped;		/* entries nuked by grim entry reaper */
static int ipcache_ager_expired;	/* entries aged out */

static ulong ipcache_ager_interval;	/* Interval between ager runs */
static int ipcache_ager_age_fast;	/* fast ager age speed */
static int ipcache_ager_age_slow;	/* slow ager age speed */

static boolean delay_ipcache_inval;
static sys_timestamp ipcache_inval_req; /* time that the first request came */
static sys_timestamp ipcache_inval_last;/* time of last cache invalidation */
static ulong ipcache_inval_mintime;	/* milliseconds */
static ulong ipcache_inval_maxtime;	/* milliseconds */
static unsigned ipcache_inval_quiettime;/* seconds */
static unsigned ipcache_inval_quietthresh; /* count of requests */
static ulong *ipcache_inval_buffer;	/* circular buffer of request counts */
static ulong *ipcache_inval_current;	/* current pointer into buffer */
static ulong ipcache_inval_recent;	/* total of all buffer entries */
static boolean ipcache_inval_pending;	/* TRUE if there's a request pending */
static hwidbtype *ipcache_inval_pending_interface; /* interface to prune */
static int waiting_on_refcount;         /* # entries stuck on garbage q */
static ulong ipcache_total_adds;        /* total # cache entries since boot */
static ulong ipcache_total_invalidates; /* total # invalidates since boot */

/***
 *** IP Cache Primitives
 ************************************************************************
 ***/

/*
 * ipcache_acl_debug
 * Should a debug message be printed given the following entry?
 */

inline boolean
ipcache_acl_debug (ipaddrtype prefix, ipaddrtype mask)
{
    acl_headertype *acl;

    if (!ipcache_debug)
	return(FALSE);

    if (ipcache_debug_acl) {
	acl = acl_array[ipcache_debug_acl];
	return(route_check(acl, prefix, mask));
    }
    
    return(TRUE);
}

/*
 * ipcache_prefix_alloc
 *
 * Allocate a prefix out of any local or main memory pool as required.
 * How we do it should be invisible to the rest of the code.
 */

inline static ipcache_rntype *
ipcache_prefix_alloc (int len)
{
    ipcache_rntype *entry;
    ulong free_bytes;

    free_bytes = mempool_get_free_bytes(MEMPOOL_CLASS_LOCAL,
					MEMPOOL_ALTERNATE_INCLUDE);
    if ((free_bytes < IPCACHE_MEM_STARVE_LIMIT) ||
	reg_invoke_fs_low_mem()) /* Also check SSE memory */
	return NULL;

    entry = malloc_named(sizeof(ipcache_rntype)+len, "IP cache");
    if (!entry)
	return NULL;

    entry->prefix_keylen = IPRADIXKEYLEN;
    entry->mask_keylen   = IPRADIXKEYLEN;

    return entry;
}

/*
 * ipcache_prefix_free
 * Return a prefix back to the pool
 *
 * If we're at interrupt level, we're not allowed to do a free, so just
 * chuck it on a queue... it will get cleaned up in one second.
 *
 * Also, don't free entries with non-zero refcounts.  Put them on
 * the garbage queue until refcount goes to zero.
 */

inline static void
ipcache_prefix_free (ipcache_rntype *entry)
{
    if (entry->refcount || get_interrupt_level()) {
	*(uchar **)entry = NULL;	/* zero out the link field */
	p_enqueue(&ipcache_garbageQ, entry);

    } else
	free(entry);
}

/*
 * ipcache_prefix_insert
 * Insert a prefix entry into the trie
 */

inline static ipcache_rntype *
ipcache_prefix_insert (ipcache_rntype *entry)
{
    leveltype level;
    rntype *rn;

    entry->flags  |= IPCACHE_FLAG_VALID;

    level = raise_interrupt_level(NETS_DISABLE);
    rn = rn_addroute(&entry->prefix_keylen, &entry->mask_keylen,
		     ipcache_head, entry->rt_nodes);
    reset_interrupt_level(level);

    if (rn == (rntype *) entry) {
	ipcache_entries++;
	ipcache_total_adds++;

	if (ipcache_acl_debug(entry->prefix, entry->mask))
	    buginf("\nIP: created cache entry for %i/%d",
		   entry->prefix, ip_bitsinmask(entry->mask));
	reg_invoke_ipcache_add_callback(entry);
    } else {
	errmsg(&msgsym(RADIXINSERT, IPFAST), 
	       entry->prefix, ip_bitsinmask(entry->mask), entry, rn);
	ipcache_prefix_free(entry);
    }

    return (ipcache_rntype *) rn;
}

/*
 * ipcache_prefix_remove
 * Unlink an entry from the trie.  
 */

inline static void
ipcache_prefix_remove (ipcache_rntype *entry, const char *reason)
{
    leveltype level;
    rntype *rn;

    level = raise_interrupt_level(NETS_DISABLE);
    entry->flags &= ~IPCACHE_FLAG_VALID;
    rn = rn_delete(&entry->prefix_keylen, &entry->mask_keylen, ipcache_head);
    reset_interrupt_level(level);

    if (rn == (rntype *) entry) {
	if (ipcache_acl_debug(entry->prefix, entry->mask))
	    buginf("\nIP: deleted cache entry for %i/%d (%s)",
		   entry->prefix, ip_bitsinmask(entry->mask), reason);

	reg_invoke_ipcache_delete_callback(entry);
	ipcache_entries--;
	ipcache_total_invalidates++;
	ipcache_prefix_free(entry);

    } else
	errmsg(&msgsym(RADIXDELETE, IPFAST),
	       entry->prefix, ip_bitsinmask(entry->mask), entry, rn);
}

#ifdef	IPCACHE_SANITY
/*
 * ipcache_prefix_asspecific
 *
 * Search for any entries in the tree either as, or more specific than
 * the one we've been passed in.
 */
static ipcache_rntype *
ipcache_prefix_asspecific (ipcache_rntype *entry)
{
    rntype *rn;
    ipcache_rntype *leaf;

    rn = rn_search_m(&entry->prefix_keylen, ipcache_head->rnh_treetop,
		     &entry->mask_keylen);

    if (!rn || !(rn->rn_flags & RNF_ACTIVE))
	return NULL;

    if (rn->rn_flags & RNF_ROOT)
	return NULL;

    leaf = (ipcache_rntype *) rn;

    /* is this leaf node out of range? */
    if ((leaf->prefix & entry->mask) != entry->prefix)
	return NULL;

    return leaf;
}
#endif	/* IPCACHE_SANITY */


/*
 * ipcache_lookup
 *
 * Look up a give IP cache entry based upon prefix & mask.  If the mask
 * is zero, return the best match.
 */

static inline ipcache_rntype *
ipcache_lookup (ipaddrtype prefix, ipaddrtype mask)
{
    ipradixkeytype pfx_key, mask_key;

    if (mask)
	return (ipcache_rntype *) rn_lookup(ipaddr2radixkey(prefix, &pfx_key),
					    ipaddr2radixkey(mask, &mask_key),
					    ipcache_head);
    else
	return (ipcache_rntype *) rn_match(ipaddr2radixkey(prefix, &pfx_key),
					   ipcache_head);
}

/*
 * ipcache_add
 *
 * Do the actual add of the precomputed data to the cache.  There should
 * be no hardware specific crud in this routine or any knowledge of packet
 * formats or other such magic, since this may be called from outside this
 * file by the SSE.
 */
inline ipcache_rntype *
ipcache_add (ipaddrtype destination, ipaddrtype nexthop,
	     idbtype *swidb, hwidbtype *hwidb, uchar *mac,
	     int maclen, ushort subint_ref_num,
	     dialergrouptype *dialergroup)
{
    ipcache_rntype *entry;
    ipaddrtype mask, c_mask;

    entry = ipcache_lookup(destination, 0);
    if (entry)			/* already have a valid entry */
	return (NULL);		/* say nothing was added */
	
    /*
     * Don't insert one of our interface addresses or aliases into the cache.
     */
    if (ip_ouraddress(destination))
        return(NULL);

    if (!iprouting_cache_mask(destination, &mask, &c_mask)) {
	if (ipcache_debug)
	    buginf("\nIP: prefix_create couldn't find mask for %i",
		   destination);
	return (NULL);
    }

    destination &= mask;

    /* belt & suspenders, no martian nets please... */
    if ((destination == 0L) || CLASSD_AND_GREATER(destination)) {
	if (ipcache_debug) 
 	    errmsg(&msgsym(INVALIDADDR, IPFAST),
		   destination, ip_bitsinmask(mask));
	return (NULL);
    }

    entry = ipcache_prefix_alloc(maclen);
    if (!entry)
	return (NULL);

    if (iprouting_recursive(destination))
	entry->flags |= IPCACHE_FLAG_RECURSIVE;

    entry->prefix = destination;
    entry->mask   = mask;
    entry->c_mask = c_mask;
    entry->nexthop= nexthop;
    entry->idb    = swidb;
    entry->hwidb  = hwidb;
    entry->length = maclen;
    entry->subint_reference_num = subint_ref_num;
    entry->dialergroup = dialergroup;

    /* Set crypto encap vector */
    reg_invoke_crypto_ipcache_add_callback(entry);

    GET_TIMESTAMP(entry->last_update);
    bcopy(mac, entry->m.macstring, maclen);

#ifdef	IPCACHE_SANITY
    /*
     * This will tell us if there is already an entry that is as-specific
     * or more specific than what we're putting in.
     */
    { ipcache_rntype *bogon;
      static const char err[] = "*error*";
      boolean cleanup = FALSE;

      bogon = ipcache_prefix_asspecific(entry);
      if (bogon) {
	  cleanup = TRUE;
	  errmsg(&msgsym(SANITY, IPFAST), "more",
		 bogon->prefix, ip_bitsinmask(bogon->mask),
				ip_bitsinmask(bogon->c_mask),
		 bogon->nexthop,
		 bogon->idb ? bogon->idb->namestring : err,
	  	 entry->prefix, ip_bitsinmask(entry->mask),
				ip_bitsinmask(entry->c_mask),
		 entry->nexthop,
		 entry->idb ? entry->idb->namestring : err);
      }

      bogon = ipcache_lookup_fast(destination);
      if (bogon) {
	  cleanup = TRUE;
	  errmsg(&msgsym(SANITY, IPFAST), "less",
		 bogon->prefix, ip_bitsinmask(bogon->mask),
				ip_bitsinmask(bogon->c_mask),
		 bogon->nexthop,
		 bogon->idb ? bogon->idb->namestring : err,
		 "less",
	  	 entry->prefix, ip_bitsinmask(entry->mask),
				ip_bitsinmask(entry->c_mask),
		 entry->nexthop,
		 entry->idb ? entry->idb->namestring : err);
      }	

      if (cleanup)
	  ipcache_invalidate_target(destination, mask, MASK_HOST);
    }
#endif	/* IPCACHE_SANITY */

    if (!ipcache_prefix_insert(entry))
	return (NULL);

    return entry;
}

/*
 * ipcache_update
 * Update the IP cache with the current IP datagram.
 * Caller checks for fast switching okay on output interface,
 * for IP protocol suite, and for allowed output encapsulation.
 * Promote entries to top of list if they've drifted down aways.
 * This is called via the registry FS_CACHE_UPDATE
 */
static void
ipcache_update (paktype *pak, ipaddrtype destination)
{
    idbtype *idb;
    hwidbtype *hwidb;
    int delta;
    int len = 4;			/* Default min! */
    trrif_t *trh;
    fddi_hdr *fddi;
    iphdrtype *ip;
    ipcache_rntype *entry;
    boolean rif_present;
    ushort subint_ref;
    dialergrouptype *dialergroup;
    rif_present = FALSE;

    /*
     * Dispatch to IP multicast cache update.
     */
    if (pak->flags & PAK_MULTICAST) {
	reg_invoke_ip_mcache_update(pak);
	return;
    }

    /*
     * Conditions for acceptance:
     * - must be transit (not sourced by us).
     * - output interface must be different that input interface,
     * - unless fastswitching on the same switch is configured (default in FR)
     * - no security in the system
     * - no multicast destinations on Ethernets.
     * - must be ARPA, SNAP(fddi), HDLC, or SMDS encapsulation.
     * - also now Fast Switch IP between ISL and IEEE 802.10 vLAN 
     *   encapsulating interfaces.
     */

    if (pak->if_input == NULL)		/* not sourced by us */
	return;

    if (pak->flags & (PAK_BROADCAST|PAK_ENCAPS_ONLY)) 
	return;				/* no multicasts or broadcasts */

    idb = pak->if_output;
    hwidb = idb->hwptr;

    /*
     * If this interface is in a dialer rotary group, get dialer interface.
     */
    if (is_ddr(hwidb)){
	/*
	 *  if this is a multilink bundle interface, we need
	 *     to check the virtual bundle interface that
	 *     is part of the dialer rotary-group here.
	 *  The dialer interface OR multilink bundle interface that
	 *     is not part of a dialer rotary-group is checked in
	 *     the normal place after this.
	 */
	if (idb->is_bundle_interface &&
	    !reg_invoke_fast_supported(LINK_IP, hwidb))
	    return;
	dialergroup = get_dialergroup(hwidb);
	idb = pak->dialer_output;
	hwidb = idb->hwptr;
    } else {
	dialergroup = NULL; 
    }

    if (!hwidb->ip_fastokay) 
	return;
    if (!interface_up(idb)) 		/* must be running */
	return;
    if (reg_invoke_fast_supported(LINK_IP, hwidb) == FALSE)
	return;

    /*
     * If fastswitching on the same interface is allowed and if redirect is
     * also enabled, do not update the cache with redirect entries.
     */
    if ((pak->if_input == idb) &&
	!idb->ip_routecache_same)
	return;

    if (ip_secopt_bytes)		/* no security anywhere */
	return;

    /*
     * Check for non-unicast and incorrect encapsulation type.
     */

    delta = 0;

    if (hwidb->status & IDB_ETHER) {

        len = pak->encsize;

	if ((pak->enctype != ET_ARPA) && (pak->enctype != ET_ISL) &&
	    (pak->enctype != ET_SDE))
	    return;

	if (((*pak->datagramstart & 0x01) == 1) &&
	    (IS_ISL_VLAN_SUBINTERFACE(idb) == FALSE)) {

            /*
             * If an outgoing cache entry is associated with a Fast Ethernet
             * interface, it may be an ISL encapsulating interface. If so allow
	     * the Cisco multicast OUI and update the encapsulation type from
	     * ARPA (Ethernet V2) to ISL.
             */
	
            return;
        }

    } else if (hwidb->status & IDB_FDDI) {

	fddi = (fddi_hdr *)pak->datagramstart;
	switch (hwidb->type) {
	  case IDBTYPE_FDDIT:
	    if (fddi->saddr[0] & FDDIT_RII)
		rif_present = TRUE;
	    break;
	  case IDBTYPE_BSI:
	  case IDBTYPE_FDDI:
	    if (fddi->saddr[0] & FDDI_RII)
		rif_present = TRUE;
	    break;
	  default:
	    errmsg(&msgsym(BADIF, IPFAST), idb->namestring);
	    return;
	}

	if (rif_present)
	    len = FDDI_SNAP_ENCAPBYTES - 1 + RIF_LENGTH(fddi->data[0]);
	else
	    len = (pak->network_start - pak->datagramstart) - FDDI_FC;
	delta = 1;

    } else if (hwidb->status & IDB_TR) {
	if (pak->enctype != ET_SNAP)
	    return;
	trh = (trrif_t *)pak->datagramstart;
	if (trh->saddr[0] & TR_RII)
	    len = TRING_SNAP_ENCAPBYTES - 2 + (trh->rc_blth & RC_LTHMASK);
	else
	    len = TRING_SNAP_ENCAPBYTES - 2;

	delta = 2;                             /* Skip over the AC/FC Bytes */

    } else if (hwidb->status & IDB_ATM) {
        if (lec_enabled(idb) && (pak->enctype != ET_ARPA))
            return;
	len = pak->encsize;
    } else if (hwidb->status & IDB_SERIAL) {
	len = pak->encsize;
    } else if (hwidb->status & IDB_CHANNEL) {
	len = CHANNEL_ENCAPBYTES;
    } else if (hwidb->status & IDB_LEX) {
	/*
	 * This should really be pak->encsize but PPP resets it to zero.
	 */
	len = pak->network_start - pak->datagramstart;

    } else if (is_tunnel(hwidb)) {
	len = pak->encsize;

    } else if (is_bvi(hwidb)) {

        /*
         * Bridge-group Virtual interface is virtual ether in terms
         * of encapsulation
         */
	
        if ((*pak->datagramstart & 0x01) == 1)  /* forbid multicast */
            return;

        /*
         * BVI is the representitive of the bridge group, so it must
	 * be flexible to accommodate any encaps
         */
        if (pak->enctype == ET_ARPA) {
            len = ETHER_ARPA_ENCAPBYTES;
        } else if (pak->enctype == ET_SNAP) {
            len = ETHER_SNAP_ENCAPBYTES;
        }
    } else
        return;

    /*
     * Find or create a cache pointer.  Since entries are dynamically
     * sized, we must also check to see if the new mac header length is
     * longer than the current length, and if so, allocate a new entry.
     */
    ip = (iphdrtype *) ipheadstart(pak);

    if (destination == 0L)
	destination = ip->dstadr;
   
    /*
     * Since we need to do mallocs & frees, don't bother with packets
     * we received at interrupt level.  This check is pretty far down
     * in the code because there are actual legitimate applications that
     * do call the vencap routines at interrupt level,  but they all
     * should hopefully get weeded out before this check.  This is not
     * an actual error, per se,  so the errmsg is conditional upon
     * ipcache debug.  The only reason to use an errmsg here is to
     * get the traceback facility.
     */
    if (get_interrupt_level()) {
	if (ipcache_debug)
	    errmsg(&msgsym(INTLEV, IPFAST), ip->srcadr,
		   pak->if_input ? pak->if_input->namestring : "local",
		   ip->dstadr,
		   pak->if_output ? pak->if_output->namestring : "local");
	return;
    }

    /*
     * Go insert the precomputed stuff onto the cache
     */
    subint_ref = reg_invoke_get_subint_ref_num_from_pak(hwidb->enctype,
							hwidb, pak);
    entry = ipcache_add(destination, pak->desthost, idb, hwidb,
			pak->datagramstart+delta, len, subint_ref,
			dialergroup);
    if (!entry) 
	return;

    /*
     * Update prefix cache on distributed L3 switchers (e.g., vip).
     * For cbus autonomous, this is a host cache, not a prefix cache.
     */

    reg_invoke_auto_sw_ip_cache_update(entry, pak, destination);

    /*
     * If this cache prefix is associated with a vLAN (ISL) subinterface
     * then we may want to inform the FEIP that this vLAN ID can be
     * switched by the SSP.
     */

    if (IS_SSE_SWITCHED_VLAN(idb) == TRUE) {

        reg_invoke_write_isl_vlan_colour_2_feip(idb, TRUE);
    }
}  


/*
 * ipcache_invalidate_subrange_walker
 *
 * Examine a cache entry, does it fall within the specified address
 * and mask range?  Was it also created by a route that was <=
 * than the one that is requesting its demise? If so, unlink it
 * from the tree and free all resources.
 *
 * Called by rn_walktree in ipcache_invalidate_subrange
 */

static int
ipcache_invalidate_subrange_walker (rntype *rn, va_list ap)
{
    ipcache_rntype *entry = rn2entry(rn);
    ipaddrtype prefix;
    ipaddrtype mask, c_mask;

    prefix = va_arg(ap, ipaddrtype);
    mask   = va_arg(ap, ipaddrtype);
    c_mask = va_arg(ap, ipaddrtype);

    if ((entry->prefix & mask) == prefix) {
	if ((entry->mask >= mask) && (entry->c_mask <= c_mask))
	    ipcache_prefix_remove(entry, "range invalidation");
	return WALK_CONTINUE;
    }

    return WALK_ABORT;	/* outside range, all done */
}

/*
 * ipcache_invalidate_subrange
 *
 * Selectively invalidate parts of the IP caches (external routine)
 */

inline void
ipcache_invalidate_subrange (ipaddrtype prefix, ipaddrtype mask,
			     ipaddrtype c_mask)
{
    rntype *rn;
    ipradixkeytype key, key2;

    if (!ipcache_entries)
	return;

    /*
     * Don't bother doing anything if we're going down anyway...
     */
    if (ipcache_inval_pending && (ipcache_inval_pending_interface == NULL))
	return;

    prefix &= mask;

    if (ipcache_acl_debug(prefix, mask)) {
	buginf("\nIP: destroying cache entries covering %i/%d..%d %s",
	       prefix, ip_bitsinmask(mask), ip_bitsinmask(c_mask),
	       (onintstack() != 0) ? " at interrupt level" : "");
    }
    rn = rn_search_m(ipaddr2radixkey(prefix, &key),
		     ipcache_head->rnh_treetop,
		     ipaddr2radixkey(mask, &key2));

    rn_walktree(rn, ipcache_invalidate_subrange_walker, prefix, mask, c_mask);
}

/*
 * ipcache_invalidate_range
 *
 * Same as ipcache_invalidate_subrange, but pay no attention to the length
 * of the route that created this entry.
 */

void
ipcache_invalidate_range (ipaddrtype prefix, ipaddrtype mask)
{
    ipcache_invalidate_subrange(prefix, mask, MASK_HOST);
}

/*
 * ipcache_invalidate_target
 *
 * Remove all less specific cache entries that would cover a given
 * destination range.  This does almost exactly the opposite of
 * ipcache_invalidate_range and is necessary to invalidate less specific
 * cache entries when a new network or subnet is added.
 */

void
ipcache_invalidate_target (ipaddrtype prefix, ipaddrtype mask,
			   ipaddrtype c_mask)
{
    ipcache_rntype *entry;

    if (!ipcache_entries)
	return;

    if (ipcache_inval_pending && (ipcache_inval_pending_interface == NULL))
	return;

    prefix &= mask;

    ipcache_invalidate_subrange(prefix, mask, c_mask);

    while ((entry = ipcache_lookup_fast(prefix)))
	ipcache_prefix_remove(entry, "target invalidation");
}

/*
 * ipcache_invalidate_nexthop_walker
 *
 * Unlinks a prefix entry with the given nexthop from the tree
 * and frees resources.
 */

static int
ipcache_invalidate_nexthop_walker (rntype *rn, va_list ap)
{
    ipcache_rntype *entry = rn2entry(rn);
    ipaddrtype nexthop;

    nexthop = va_arg(ap, ipaddrtype);

    if (nexthop == entry->nexthop)
	ipcache_prefix_remove(entry, "nexthop invalidation");

    return WALK_CONTINUE;
}

/*
 * ipcache_invalidate_nexthop
 *
 * Remove all prefix entries that use a given nexthop
 */

void
ipcache_invalidate_nexthop (ipaddrtype nexthop)
{
    if (!ipcache_entries)
	return;

    if (ipcache_debug) {
	buginf("\nIP: pruning prefix cache entries of next hop %i",
	       nexthop);
    }

    rn_walktree(ipcache_head->rnh_treetop,
                ipcache_invalidate_nexthop_walker, nexthop);
}

/*
 * ipcache_invalidate_interface_walker
 *
 * This is the work-horse routine that actually unlinks a prefix
 * entry from the tree and frees resources.
 */

static int
ipcache_invalidate_interface_walker (rntype *rn, va_list ap)
{
    ipcache_rntype *entry = rn2entry(rn);
    hwidbtype *hwidb;

    hwidb = va_arg(ap, hwidbtype *);

    if (!hwidb || (hwidb == entry->hwidb))
	ipcache_prefix_remove(entry, "interface invalidation");

    return WALK_CONTINUE;
}

/*
 * ipcache_invalidate_interface
 *
 * Prune all prefix entries that use a given interface.
 * If idb is NULL, prune everything.
 */

static void
ipcache_invalidate_interface (hwidbtype *hwidb)
{
    if (ipcache_inval_pending && (ipcache_inval_pending_interface != hwidb))
	hwidb = NULL;

    ipcache_inval_pending = FALSE;
    ipcache_inval_pending_interface = NULL;

    if (!ipcache_entries)
	return;

    if (!hwidb)
	GET_TIMESTAMP(ipcache_inval_last);

    if (ipcache_debug) {
	buginf("\nIP: pruning prefix cache entries for %s%s",
	       hwidb ? hwidb->hw_namestring : "all interfaces",
	       (onintstack() != 0) ? " at interrupt level" : "");
	bugtrace();
    }

    /*
     * If we're at raised IPL or someone (E-OIR) has disabled blocking,
     * we do NOT want to block.
     */
    if (process_suspends_allowed())
	rn_walktree_timed(ipcache_head, ipcache_invalidate_interface_walker,
			  ip_radix_successor, hwidb);
    else
	rn_walktree(ipcache_head->rnh_treetop,
		    ipcache_invalidate_interface_walker, hwidb);
    
}

/*
 * ipcache_invalidate_interface_request
 * Function to invalidate the IP fast cache
 *
 * The cache is flushed only if no deferred invalidation is in effect.
 * Otherwise, we note the fact that an invalidation request has been made
 * and note the current time if no previous request was pending.  Eventually,
 * the cache will actually be flushed when (a) the minimum interval has
 * passed, and (b) the number of cache invalidation requests has quieted down
 * (defined as a certain number of requests over a certain interval).
 * The invalidation is guaranteed to happen by the time the maximum interval
 * has passed.
 */

static void
ipcache_invalidate_interface_request (hwidbtype *hwidb)
{
    /* Just do it if we're not fancy. */
    if (!delay_ipcache_inval) {
	ipcache_invalidate_interface(hwidb);
	return;
    }

    /* Count the activity. */
    if (ipcache_inval_current) {
	(*ipcache_inval_current)++;
	ipcache_inval_recent++;
    }

    /* If no request is pending, note the time. */
    if (!ipcache_inval_pending) {
	GET_TIMESTAMP(ipcache_inval_req);
	ipcache_inval_pending = TRUE;
	ipcache_inval_pending_interface = hwidb;
	if (ipcache_debug)
	    errmsg(&msgsym(INVALREQ, IPFAST), 
		   hwidb ? hwidb->hw_namestring : "all interfaces");
    } else {
	/*
	 * If we're waiting to nuke a different interface already
	 * then just nuke ALL interfaces.
	 */
	if (ipcache_inval_pending_interface != hwidb) {
	    ipcache_inval_pending_interface = NULL;
	    if (ipcache_debug)
		errmsg(&msgsym(INVALREQ, IPFAST), 
		       hwidb ? hwidb->hw_namestring : "all interfaces");
	}
    }
}

/*
 * ipcache_invalidate
 *
 * Function to invalidate the IP route cache.
 * This function is called by REGISTRY_BUMP_CACHE_VERSION
 * and also directly by any code in the IP or IP routing modules.
 */

void
ipcache_invalidate (hwidbtype *hwidb, idbtype *swidb,
		    boolean immediately)
{
    if ((hwidb == NULL) || (hwidb->ip_routecache)) {
	if (immediately)
	    ipcache_invalidate_interface(hwidb);
	else
	    ipcache_invalidate_interface_request(hwidb);
    }
}

/*
 * ipcache_free_resources
 * Flush IP routing cache (called from command line)
 */

void
ipcache_free_resources (parseinfo *csb)
{
    ipaddrtype prefix, mask;
    
    if ((GETOBJ(paddr,1)->type == ADDR_IP) &&
	(GETOBJ(paddr,2)->type == ADDR_IP)) {

	prefix = GETOBJ(paddr,1)->ip_addr;
	mask   = GETOBJ(paddr,1)->ip_addr;
	prefix &= mask;

	ipcache_invalidate_range(prefix, mask);
    } else
	ipcache_invalidate_interface(NULL);
}

/*
 * ipcache_ager_walker
 *
 * Check to see how we're doing on memory,  if we're kinda low, try
 * killing off old cache entries at the rate of 1/STEPINTERVAL each
 * ager run.
 */

static int
ipcache_ager_walker (rntype *rn, va_list ap)
{
    ipcache_rntype *entry = rn2entry(rn);

    boolean last_link;
    ipaddrtype prefix;

    prefix = entry->prefix;
    last_link = !rn->rn_dupedkey;

    ipcache_ager_examined++;

    if (entry->flags & IPCACHE_FLAG_RECURSIVE) {
	if (!iprouting_destination_valid(prefix, entry->nexthop,
					 entry->idb)) {
	    ipcache_ager_reaped++;
	    ipcache_prefix_remove(entry, "stale recursive");
	    return WALK_CONTINUE;
	}
    }

    if (ipcache_ager_stepover-- == 0) {
	ipcache_ager_stepover = ipcache_ager_stepinterval;
	ipcache_ager_expired++;
	ipcache_prefix_remove(entry, "aged out");
    }

    return WALK_CONTINUE;
}

/*
 * ipcache_ager
 * Age routing cache, also heal stale recursive routes.
 *
 * The ager runs once every ipcache_ager_interval ms (once a minute by
 * default).  It checks every entry to make sure that a recursive routing
 * chage (which we can't detect directly) hasn't made the entry invalid.
 * This process is called reaping and the number of entries reaped is reported
 * due to this check.
 *
 * The second function this ager does is arbitrarily delete one out of every
 * "ipcache_ager_stepover" entries.  (By default, this is 1 out of every 20
 * or 1 out of every 4 entries based on whether we have lots or little memory.)
 * The number of entries "aged" is based on this check.
 *
 * The idea here is we refresh the entire cache approximately every 20
 * minutes.  This isn't exact due to slop, but that's the basic goal.
 * We're doing some magic here to make sure that each time we run, we start
 * at a different entry (stepover's initialized value) so that we always
 * try to reap the first few entries every stepinterval iterations.
 *
 * We used to spare entries from being reaped if they were in use
 * (if an entry had been used in the last n seconds, we wouldn't consider
 * it for aging).  This seemed to be an obvious win,  however, in real life,
 * it was a bad idea because load-sharing ended up breaking.  If you had a
 * constantly popular entry, such as to a popular WWW site on the net, it
 * would never get aged.  Now, if you have two links in parallel and one link
 * flaps, the entry migrates to (or stays on) the stable link.  Everyone
 * migrates to that link and never has a reason to migrate back to the other
 * link.  You end up with one link heavily congested and the other almost
 * empty.
 *
 */

static process
ipcache_ager (void)
{
    int run;
    ulong starttime, curtime;
    ulong entries;
    ulong free_bytes;

    run = 0;

    /* don't muck with cache if we're crashing */
    process_set_crashblock(THIS_PROCESS,TRUE);

    while (ipcache_ager_interval) {
	process_sleep_for(ipcache_ager_interval);

	if (!ipcache_entries)
	    continue;

	process_get_runtime(&starttime);

	free_bytes = mempool_get_free_bytes(MEMPOOL_CLASS_LOCAL,
					    MEMPOOL_ALTERNATE_INCLUDE);
	if ((free_bytes < IPCACHE_MEMORY_LIMIT) ||
	    reg_invoke_fs_low_mem()) /* Also check SSE memory */
	    ipcache_ager_stepinterval = ipcache_ager_age_fast;
	else
	    ipcache_ager_stepinterval = ipcache_ager_age_slow;

	run = (run++ % ipcache_ager_stepinterval);
	ipcache_ager_stepover = run;

	ipcache_ager_reaped = 0;
	ipcache_ager_expired = 0;
	ipcache_ager_examined = 0;

	entries = ipcache_entries;

	rn_walktree_timed(ipcache_head,
			  ipcache_ager_walker, ip_radix_successor);

	if (ipcache_debug) {
	    process_get_runtime(&curtime);
	    buginf("\nIP: ager ran for %dms, %d entries, "
		   "%d examined, %d aged, %d reaped",
		   curtime - starttime, entries,
		   ipcache_ager_examined, ipcache_ager_expired,
		   ipcache_ager_reaped);
	}
    }

    ipcache_ager_pid = 0;

    process_kill(-1);			/* die */
}

/*
 * ipcache_is_quiet
 *
 * Return TRUE if ip cache has been "quiet" (not too many invalidation
 * requests recently).
 */

static boolean
ipcache_is_quiet(void)
{
    /* Say it's quiet if we're not tracking activity. */
    if (ipcache_inval_buffer == NULL)
	return(TRUE);

    return (ipcache_inval_recent <= ipcache_inval_quietthresh);
}

/*
 * ipcache_onesec
 *
 * (a) clean up any memory that was scheduled to be freed at interrupt level
 * (b) test to see if the route cache should be invalidated.
 *
 * Called periodically from the system once-a-second registry.
 */

static void
ipcache_onesec (void)
{
    ipcache_rntype *entry;
    int waiting, count;
    
    /*
     * Get rid of most memory scheduled to be returned to the pool
     * that isn't still referenced elsewhere (e.g., cache front end).
     * Note that if/when someone adds to the queue while we're here,
     * it won't get freed until next time around.
     */
    waiting = 0;
    count = QUEUESIZE(&ipcache_garbageQ);
    while (count-- > 0) {
	entry = p_dequeue(&ipcache_garbageQ);
	if (!entry)
	  break;
	if (!entry->refcount)
	  free(entry);
	else {
	    p_enqueue(&ipcache_garbageQ, entry);
	    waiting++;
	}
    }
    waiting_on_refcount = waiting;

    /* If the max interval has been exceeded, do it now. */
    if (ipcache_inval_pending) {
	if (CLOCK_OUTSIDE_INTERVAL(ipcache_inval_req, ipcache_inval_maxtime)) {
	    ipcache_invalidate_interface(ipcache_inval_pending_interface);
	    
	/*
	 * If things have quieted down and the min interval has been
	 * exceeded, do it now.
	 */
	} else
	    if (CLOCK_OUTSIDE_INTERVAL(ipcache_inval_req,
				       ipcache_inval_mintime) && 
	        ipcache_is_quiet()) {
	    ipcache_invalidate_interface(ipcache_inval_pending_interface);
	}
    }

    /* Circularly advance the current pointer around the buffer. */
    if (ipcache_inval_current != NULL) {
	ipcache_inval_current++;
	if (ipcache_inval_current >= 
	    (ipcache_inval_buffer + ipcache_inval_quiettime)) {
	    ipcache_inval_current = ipcache_inval_buffer;
	}

	/* Adjust the running total appropriately. */
	ipcache_inval_recent -= *ipcache_inval_current;
	*ipcache_inval_current = 0;
    }

    /*
     * This is used to flush summarized flow switching stats to the
     * appropriate idb's and global counters on the RSP.
     */
    reg_invoke_flush_ip_stats();
}

/*
 * ipcache_calc_walker
 *
 * Compute statistics about a single prefix entry
 * Called via rn_walktree in ipfast_prefix_show.
 */

static int
ipcache_show_calc_walker (rntype *rn, va_list ap)
{
    ulong *memory;

    memory  = va_arg(ap, ulong *);
    *memory  += get_block_size(rn, NULL);
    return WALK_CONTINUE;
}

void
ipcache_show_entry (ipcache_rntype *entry, boolean verbose)
{
    int len, j, start;
    char time[20], *name, *type;

    sprint_dhms(time, entry->last_update);

    if (IS_VLAN_CONFIGURED_SUBINTERFACE(entry->idb) ||
	is_valid_vip(entry->hwidb))
	name = entry->idb->short_namestring;
    else
	name = entry->idb->namestring;

    type = "";
    start= 0;

    if (IS_ISL_VLAN_SUBINTERFACE(entry->idb) &&
	ieee_equal(entry->m.macstring, isl_vlan_multicast)) {
	type = "ISL";
	start= ISL_HEADER_LENGTH;
    } else if (IS_DOT10_VLAN_SUBINTERFACE(entry->idb))
	type = "802.10";

    if (verbose) {
	len = printf("%i/%d-%d", entry->prefix,
		     ip_bitsinmask(entry->mask),
		     ip_bitsinmask(entry->c_mask));

	printf("%*c %9s %15s ", 23-len, ' ', time, name);

	if (entry->nexthop)
	    printf("%i", entry->nexthop);

        printf("\n%10c %7s %3d ", ' ', type, entry->length);

	for (j = start; j < entry->length; j++) {
	    if ((j % 24 == 0) && j)
		printf("\n%23c", ' ');
	    printf("%-02x", entry->m.macstring[j]);
	}

    } else {

	len = printf("%i/%d", entry->prefix, ip_bitsinmask(entry->mask));
	printf("%*c %9s %15s ", 23-len, ' ', time, name);
	if (entry->nexthop)
	    printf("%i", entry->nexthop);
    }
    printf("\n");
}

/*
 * ipcache_show_walker
 *
 * Display information about a single prefix entry
 * Called via rn_walktree_blocking in ipfast_prefix_show.
 */

static int
ipcache_show_walker (rntype *rn, va_list ap)
{
    ipcache_rntype *entry = rn2entry(rn);
    ipaddrtype prefix, mask;
    idbtype *idb;
    boolean verbose;

    prefix = va_arg(ap, ipaddrtype);
    mask   = va_arg(ap, ipaddrtype);
    idb    = va_arg(ap, idbtype *);
    verbose= va_arg(ap, boolean);

    if ((entry->prefix & mask) != prefix)
        return WALK_ABORT;

    if (!idb || (entry->idb == idb))
        ipcache_show_entry(entry, verbose);

    if (automore_quit())
        return WALK_ABORT;

    process_may_suspend();

    return WALK_CONTINUE;
}


/*
 * ipcache_show
 *
 * Tell the world about everything in the prefix cache.
 */

void
ipcache_show (parseinfo *csb)
{
    ulong memory;
    char *buffer;
    ipaddrtype prefix, mask;
    ipradixkeytype prefix_key, mask_key;
    idbtype *idb;
    boolean verbose, flow, crypto, opt_cbus;
    rntype *rn;
    static const char hdr_verbose[] =
	"Prefix/Length          Age       Interface       Next Hop\n";
    
    if (GETOBJ(paddr,1)->type == ADDR_IP) {
	prefix = GETOBJ(paddr,1)->ip_addr;
	mask   = GETOBJ(paddr,2)->ip_addr;
	prefix &= mask;
    } else
	prefix = mask = 0;

    idb = GETOBJ(idb, 1);
    verbose = GETOBJ(int, 1);
    flow = GETOBJ(int, 2);
    opt_cbus = GETOBJ(int, 3) | GETOBJ(int, 4);
    crypto = GETOBJ(int, 5);

    if (flow && crypto) {
        reg_invoke_crypto_show_flow_info(prefix, mask, idb);
        return;
    }
    
    if (flow) {
	reg_invoke_ip_cache_flow_show(prefix, mask, idb);
	return;
    }

    if (opt_cbus) {
	reg_invoke_auto_sw_ip_cache_show(prefix, mask);
	return;
    }

    automore_enable(NULL);
    memory = 0;
    rn_walktree(ipcache_head->rnh_treetop, ipcache_show_calc_walker, &memory);

    printf("IP routing cache %lu entr%s, %lu bytes\n",
	   ipcache_entries, ipcache_entries == 1 ? "y" : "ies",
	   memory);
    printf("   %lu adds, %lu invalidates, %lu refcounts\n",
	   ipcache_total_adds, ipcache_total_invalidates,
	   ipcache_refcounts);



    if (waiting_on_refcount)
	printf("   %d invalidated entr%s waiting for flow cache\n",
	       waiting_on_refcount, waiting_on_refcount == 1 ? "y" : "ies");

    if (ipcache_ager_interval) {
	if ((ipcache_ager_age_fast != IPCACHE_AGER_DEF_AGE_FAST) ||
	    (ipcache_ager_age_slow != IPCACHE_AGER_DEF_AGE_SLOW) ||
	    (ipcache_ager_interval != IPCACHE_AGER_DEF_INTERVAL))
	    printf("Cache aged by 1/%d every %d seconds "
		   "(1/%d when memory is low).\n", ipcache_ager_age_slow,
		   ipcache_ager_interval / ONESEC, ipcache_ager_age_fast);
    } else
	printf("Cache ager disabled by user configuration\n");

    if (delay_ipcache_inval) {
	printf("Minimum invalidation interval %d seconds, "
	       "maximum interval %d seconds,\n",
	       ipcache_inval_mintime / ONESEC, ipcache_inval_maxtime / ONESEC);
	printf("   quiet interval %d seconds, threshold %d requests\n",
	       ipcache_inval_quiettime,
	       ipcache_inval_quietthresh);
	if (ipcache_inval_buffer != NULL) {
	    printf("Invalidation rate %d in last second", 
		   *ipcache_inval_current);
	    if (ipcache_inval_quiettime > 1)
		printf(", %d in last %d seconds", ipcache_inval_recent,
		       ipcache_inval_quiettime);
	    printf("\n");
	}
	if (ipcache_inval_pending) {
	    printf("Cache invalidation for %s pending for %d seconds\n",
		   (ipcache_inval_pending_interface ?
		       ipcache_inval_pending_interface->hw_namestring :
		       "all interfaces"),
		   ELAPSED_TIME(ipcache_inval_req) / ONESEC);
	}
    }

    if (TIMER_RUNNING(ipcache_inval_last)) {
	buffer = string_getnext();
	sprint_dhms(buffer, ipcache_inval_last);
	printf("Last full cache invalidation occurred %s ago\n", buffer);
    }

    /*
     * If a mask was specified, traverse to the edge of that range first.
     */
    if (mask) {
	rn = rn_search_m(ipaddr2radixkey(prefix, &prefix_key),
			 ipcache_head->rnh_treetop,
			 ipaddr2radixkey(mask, &mask_key));
    } else
	rn = ipcache_head->rnh_treetop;

    automore_header(verbose ? ipcache_hdr : hdr_verbose);
    rn_walktree_blocking(rn, ipcache_show_walker, prefix, mask, idb, verbose);
    automore_disable();
}

/*
 * ipcache_inval_init - initialize ipcache invalidation
 *
 * If maxtime == 0, cache invalidation is disabled.
 */

static void
ipcache_inval_init (ulong mintime, ulong maxtime,
		    ulong quiettime, ulong quietthresh)
{
    /* Free any existing memory blocks. */
    if (ipcache_inval_buffer) {
	free(ipcache_inval_buffer);
	ipcache_inval_buffer = NULL;
    }

    ipcache_inval_pending = FALSE;
    ipcache_inval_pending_interface = NULL;
    ipcache_inval_recent = 0;
    ipcache_inval_current = NULL;

    if (maxtime) {		/* Enabled */
	ipcache_inval_mintime = mintime;
	ipcache_inval_maxtime = maxtime;
	ipcache_inval_quiettime = quiettime;
	ipcache_inval_quietthresh = quietthresh;
	/* Note that things run (albeit crippled) if the malloc fails. */
	ipcache_inval_buffer = 
	    malloc(ipcache_inval_quiettime * sizeof(ulong));
	ipcache_inval_current = ipcache_inval_buffer;
	ipcache_inval_recent = 0;
	if (!ipcache_inval_buffer) {
	    printf(nomemory);
	}
	delay_ipcache_inval = TRUE;
    } else {			/* disabled */
	ipcache_inval_mintime = ipcache_inval_maxtime = 0;
	ipcache_inval_quiettime = ipcache_inval_quietthresh = 0;
	delay_ipcache_inval = FALSE;
    }
}

/*
 * ipcache_command
 * Parse IP cache commands.
 */
void ipcache_command (parseinfo *csb)
{
    if (csb->nvgen) {
	switch (csb->which) {
	case IPCACHE_AGER:
	    if ((ipcache_ager_interval != IPCACHE_AGER_DEF_INTERVAL) ||
		(ipcache_ager_age_fast != IPCACHE_AGER_DEF_AGE_FAST) ||
		(ipcache_ager_age_slow != IPCACHE_AGER_DEF_AGE_SLOW)) {
		nv_write(TRUE, "%s %d %d %d", csb->nv_command,
			 ipcache_ager_interval / ONESEC,
			 ipcache_ager_age_fast,
			 ipcache_ager_age_slow);
	    }
	    break;
	case IPCACHE_INVALIDATE:
	    /* If not running, issue the "no" command. */
	    if (!delay_ipcache_inval) {
		nv_write(TRUE, "no %s", csb->nv_command);
	    } else {
		/* If not using defaults, issue the command. */
		if (ipcache_inval_mintime != IPCACHE_MIN_INVAL_DEFAULT ||
		    ipcache_inval_maxtime != IPCACHE_MAX_INVAL_DEFAULT ||
		    ipcache_inval_quiettime != IPCACHE_QUIET_IVL_DEFAULT ||
		    ipcache_inval_quietthresh != IPCACHE_QUIET_THRESH_DEFAULT){
		    nv_write(TRUE, "%s %d %d %d %d",
			     csb->nv_command,
			     ipcache_inval_mintime / ONESEC,
			     ipcache_inval_maxtime / ONESEC,
			     ipcache_inval_quiettime,
			     ipcache_inval_quietthresh);
		}
	    }
	    break;
	}
	return;
    }

    switch (csb->which) {
    case IPCACHE_AGER:
	if (csb->sense) {
	    ipcache_ager_interval = GETOBJ(int, 1) * ONESEC;
	    ipcache_ager_age_fast = GETOBJ(int, 2);
	    ipcache_ager_age_slow = GETOBJ(int, 3);
	} else {
	    ipcache_ager_interval = IPCACHE_AGER_DEF_INTERVAL;
	    ipcache_ager_age_fast = IPCACHE_AGER_DEF_AGE_FAST;
	    ipcache_ager_age_slow = IPCACHE_AGER_DEF_AGE_SLOW;
	}
	/*
	 * Setting the ager interval to 0 will allow it to die gracefully.
	 * If the pid is currently 0, it means we killed it previously and
	 * need to restart it.
	 */
	if (ipcache_ager_interval && !ipcache_ager_pid)
	    ipcache_ager_pid = process_create(ipcache_ager, "IP Cache Ager",
					      NORMAL_STACK, PRIO_LOW);
	break;

    case IPCACHE_INVALIDATE:
	if (csb->set_to_default || csb->sense) {
	    /* If no parameters, use the defaults. */
	    if (!GETOBJ(int,2)) {
		ipcache_inval_init(IPCACHE_MIN_INVAL_DEFAULT,
				   IPCACHE_MAX_INVAL_DEFAULT,
				   IPCACHE_QUIET_IVL_DEFAULT,
				   IPCACHE_QUIET_THRESH_DEFAULT);
	    } else {
		ulong min = GETOBJ(int,1) * ONESEC;
		ulong max = GETOBJ(int,2) * ONESEC;

		if (min > max) {
		    printf("\nIP:  Max cannot be less than min");
		    return;
		}
		ipcache_inval_init(min, max, GETOBJ(int,3), GETOBJ(int,4));
	    }
	} else
	    ipcache_inval_init(0,0,0,0);	/* Turn it off */
	break;
    }
}

/*
 * idb_can_fastswitch_ip
 *
 * This is a hardware dependant routine for determining if a given
 * hardware idb is capable of fast switching IP.  This is one of those
 * modularity damed-if-you-do and damned-if-you-don't routines because
 * either the driver or IP needs to have incestuious knowledge of the
 * other's capabilities.
 */
static boolean
idb_can_fastswitch_ip (hwidbtype *hwidb)
{
    if ((hwidb->status & (IDB_MCI | IDB_CBUS)) ||
	(hwidb->type == IDBTYPE_TMS380) || (hwidb->type == IDBTYPE_BSI) ||
        (hwidb->type == IDBTYPE_HD64570) || (hwidb->type == IDBTYPE_PCBUS) ||
	(hwidb->type == IDBTYPE_E1) || (hwidb->type == IDBTYPE_T1) ||
	(hwidb->type == IDBTYPE_ASYNC) || (hwidb->type == IDBTYPE_VTY_ASYNC) ||
	(hwidb->type == IDBTYPE_LEX) || (hwidb->type == IDBTYPE_ATMZR) ||
	(hwidb->type == IDBTYPE_BRI) || (hwidb->type == IDBTYPE_DIALER)||
	(hwidb->type == IDBTYPE_MK5025) || (hwidb->type == IDBTYPE_LANCE) ||
	(hwidb->type == IDBTYPE_QUICC_ETHER) || 
	(hwidb->type == IDBTYPE_QUICC_SERIAL) ||
	(hwidb->type == IDBTYPE_AMDP2) ||
	(hwidb->type == IDBTYPE_IBM2692) ||
	(hwidb->type == IDBTYPE_S4T68360) ||
	(hwidb->type == IDBTYPE_FDDIT) || 
	(hwidb->type == IDBTYPE_FEIP) ||  
        (hwidb->type == IDBTYPE_CD2430) ||
	(hwidb->type == IDBTYPE_BVI)) { 

	if ((hwidb->status & IDB_ETHER) && (hwidb->enctype == ET_ARPA))
	    return (TRUE);

	if (hwidb->status & IDB_SERIAL) {
	    if ((hwidb->enctype == ET_HDLC) || is_ppp(hwidb) || 
		is_smds(hwidb) || is_atm_dxi(hwidb))
		return(TRUE);
	    if (!is_ddr(hwidb)) {
		if (is_frame_relay(hwidb) || is_slip(hwidb))
		    return (TRUE);
	    }

	}

	/*
	 * SNAP is the "native" encapsulation for ring devices like the
	 * FDDI and token-ring.
	 */
	if (hwidb->enctype == ET_SNAP) {
	    if (hwidb->status & IDB_FDDI)
		return (TRUE);

	    switch (hwidb->type) {
		case IDBTYPE_CTR:
		case IDBTYPE_TMS380:
	        case IDBTYPE_IBM2692:
		    return (TRUE);
		default:
		    break;
	    }
	}

	if ((hwidb->enctype == ET_ATM) || (hwidb->status & IDB_ATM))
	    return (TRUE);

	if (hwidb->type == IDBTYPE_CIP)
	    return (TRUE);

	if ((hwidb->status & IDB_LEX) && (hwidb->enctype == ET_ARPA))
	    return (TRUE);

	if (hwidb->type == IDBTYPE_BVI)
            return (TRUE);
    }
    if (is_tunnel(hwidb))
	return(reg_invoke_fast_supported(LINK_IP, hwidb));
    return (FALSE);
}

/* 
 * ipcache_setup_interface
 *
 * Turn on use of fast switching.  Logically AND configuration option
 * "idb->ip_routecache" with whether or not this interface can support
 * fast switching.  Fast switching code checks "idb->ip_fastokay".
 * Call when:
 *    - IP initialization.
 *    - changing serial encapsulation.
 *    - "[no] ip route-cache" configuration command given.
 */

void
ipcache_setup_interface (hwidbtype *idb)
{
    hwidbtype *tmpidb = idb;
    boolean fastflag;
    
    fastflag = idb_can_fastswitch_ip(idb);
    if (!router_enable || idb->tcp_compress
	|| (idb->comp_db && (idb->enctype != ET_FRAME_RELAY))
        || idb->comp_db || idb->ppp_compression)
	fastflag = FALSE;

    idb->ip_fastokay = (fastflag && idb->ip_routecache);

    /*
     * Check for async fastswitching prohibition on AUX port
     */
    reg_invoke_async_check_fs(idb);

    /*
     * Check to see if SSE switching can still be enabled.
     */
    if (idb->ip_routecache & IP_SSE_ROUTECACHE) {
	if (!reg_invoke_sse_idb_capable(idb, idb->ip_fastokay))
	    idb->ip_routecache &= ~IP_SSE_ROUTECACHE;
    }

    if (idb->ip_fastokay) {
	idb->fast_switch_flag |= FSF_DODIP;
	reg_invoke_register_drivers(FSF_DODIP, idb);
	set_default_queueing(idb);
    } else {
	idb->fast_switch_flag &= ~FSF_DODIP;
	FOR_ALL_HWIDBS(tmpidb)
	    if (tmpidb->ip_fastokay)
		break;
	if (!tmpidb) {
	    reg_invoke_register_drivers(FSF_DODIP, idb);
	    set_default_queueing(idb);
	}
    }
    ipcache_invalidate_interface(idb);
}

/*
 * ipcache_init
 * Initialize the IP cache data structure.
 */

static void ipcache_init (subsystype *dummy)
{
    /* Create some free lists for commonly used sizes. */

    mempool_add_free_list(MEMPOOL_CLASS_LOCAL,
		  sizeof(ipcache_rntype) + HDLC_ENCAPBYTES);
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL,
		  sizeof(ipcache_rntype) + ETHER_ARPA_ENCAPBYTES);
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL,
		  sizeof(ipcache_rntype) + FDDI_SNAP_ENCAPBYTES - 1);

    ipcache_entries = 0L;
    ipcache_ager_interval = IPCACHE_AGER_DEF_INTERVAL;
    ipcache_ager_age_slow = IPCACHE_AGER_DEF_AGE_SLOW;
    ipcache_ager_age_fast = IPCACHE_AGER_DEF_AGE_FAST;

    reg_invoke_auto_sw_cache_init(LINK_IP);

    /* Initialize the invalidation delay stuff. */

    ipcache_inval_buffer = NULL; /* Make sure it's clean */
    ipcache_inval_init(IPCACHE_MIN_INVAL_DEFAULT, 
		       IPCACHE_MAX_INVAL_DEFAULT,
		       IPCACHE_QUIET_IVL_DEFAULT, 
		       IPCACHE_QUIET_THRESH_DEFAULT);

    /* Initialize IP fast prefix cache */
    if (!rn_inithead((void **) &ipcache_head, IPRADIXDATAOFFSET))
	errmsg(&msgsym(RADIXINIT, IPFAST));

    /* hang up the clotheswire */
    queue_init(&ipcache_garbageQ, 0);

    /*
     * Register some functions
     */
    reg_add_onesec(ipcache_onesec, "ipcache_onesec");
    reg_add_fs_cache_update(LINK_IP, ipcache_update, "ipcache_update");
    reg_add_bump_cache_version(ipcache_invalidate, "ipcache_invalidate");
    reg_add_ipcache_setup_interface(ipcache_setup_interface,
				    "ipcache_setup_interface");
    reg_add_ipcache_invalidate_range(ipcache_invalidate_range,
				     "ipcache_invalidate_range");
    reg_add_ipcache_invalidate_target(ipcache_invalidate_target,
				     "ipcache_invalidate_target");
    reg_add_ipcache_invalidate_nexthop(ipcache_invalidate_nexthop,
				     "ipcache_invalidate_nexthop");
    reg_add_ipcache_invalidate_subrange(ipcache_invalidate_subrange,
				     "ipcache_invalidate_subrange");
    reg_add_ipcache_invalidate(ipcache_invalidate, "ipcache_invalidate");
    reg_add_fast_setup(ipcache_setup_interface, "ipcache_setup_interface");

    /*
     * Initialize IP fast cache parser support
     */
    ipfast_parser_init();

    /*
     * Start the ager process
     */
    ipcache_ager_pid = process_create(ipcache_ager, "IP Cache Ager",
				      NORMAL_STACK, PRIO_LOW);
}

/* sp - add fastswitch accounting */
/*
 * ip_fast_accumulate_acctg
 * Called from ip_fastswitch() and ipflow_flush_stats()
 */

boolean ip_fast_accumulate_acctg (ipaddrtype srcadr, ipaddrtype dstadr,
				  ulong bytes, ulong packets)
{
    ip_acctg_entry_type *ipae;
    long slot;
    uint i;
    ipaddrtype mask;
    ip_acctg_acc_entry_type *acc_entry;
    
    if (ip_acctg_acc != NULL) {
	for (i = 0, acc_entry = &ip_acctg_acc->list[0]; 
	     i < ip_acctg_acc->count; acc_entry++, i++) {
	    mask = acc_entry->mask;
	    if (((srcadr & mask) == acc_entry->addr) ||
		((dstadr & mask) == acc_entry->addr)) {
		goto hit;
	    }
	}
	if (ip_acctg_acc->count == 0)
	  goto hit;
	if (ip_acctg_acc->transits == 0)
	  return(TRUE);
	ip_acctg_acc->transits--;
    }
hit:
    /* determine hash table entry */
    slot = srcadr;
    slot ^= dstadr;
    slot ^= (slot >>16);
    slot ^= (slot >>8);
    slot &= 0xFF;
    ipae = ip_acctg_info.current->store[slot];

    /* first look for this entry in the hash table */
    while (ipae) {
	if ((ipae->srcadr == srcadr) && (ipae->dstadr == dstadr)) {
	    /* found it - update counts & get out of here */
	    ipae->packets += packets;
	    ipae->bytes += bytes;
	    return(TRUE);
	}
	/* let's try the next item */
	ipae = ipae->next;
    }
    
    if (ip_acctg_info.count < ip_acctg_info.threshold) {
	/* not found in hash table - get an entry from the avail acct Q */
	ipae = dequeue(&ipacctQ);
	if (ipae) {
	    /* free entry available - fill it in */
	    ipae->srcadr = srcadr;
	    ipae->dstadr = dstadr;
	    ipae->bytes = bytes;
	    ipae->packets = packets;

	    /* update hash table for this entry */
	    ipae->next = ip_acctg_info.current->store[slot];
	    ip_acctg_info.current->store[slot] = ipae;

	    ip_acctg_info.count++;
	    return(TRUE);
	}
	/*
	 * We need to get more memory -- do it at process level.
	 */
	return(FALSE);
    }
    /* over accounting threshold - let's just get out */
    ip_acctg_info.current->packets_tossed += packets;
    ip_acctg_info.current->bytes_tossed += bytes;
    return(TRUE);
}

/*
 * IPCACHE subsystem header
 */

#define IPCACHE_MAJVERSION 1
#define IPCACHE_MINVERSION 0
#define IPCACHE_EDITVERSION  1

SUBSYS_HEADER(ipcache, IPCACHE_MAJVERSION, IPCACHE_MINVERSION,
	      IPCACHE_EDITVERSION,
	      ipcache_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iprouting",
	      "req: iprouting");
