/* $Id: vinesfast.c,v 3.3.62.5 1996/08/28 13:19:56 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/vinesfast.c,v $
 *------------------------------------------------------------------
 * vinesfast.c -- Device independent Vines Fast Switching
 *
 * July 1992, David R. Hampton
 *
 Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vinesfast.c,v $
 * Revision 3.3.62.5  1996/08/28  13:19:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.4  1996/08/13  14:34:52  rbadri
 * CSCdi64089:  Multiprotocol support fixes required
 * Branch: California_branch
 * Vines support for LANE
 *
 * Revision 3.3.62.3  1996/07/23  13:30:34  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.62.2  1996/07/17  00:02:55  ehk
 * CSCdi59059:  Out of order packets when using vines route-cache
 * Branch: California_branch
 * invalidate a fast cache entry before passing to process switching
 * a packet with metric notification bit set and relearn a new cache
 * to maintain original packet ordering.
 *
 * Revision 3.3.62.1  1996/03/18  22:32:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  02:05:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  21:47:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  17:59:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:48:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:45:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:34:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/19  23:41:36  slin
 * CSCdi41922:  Vines Router process suspends too frequently
 *
 * Revision 2.5  1995/08/09  23:30:04  slin
 * CSCdi34197:  Software forced reload/Crash in vines testing
 *
 * Revision 2.4  1995/07/25  18:54:48  slin
 * CSCdi37335: VINES: Vines cache entries contain wrong MAC address for
 *             interface
 * - Wrong interface's vencap routine called to build cache entry
 *
 * Revision 2.3  1995/07/10  05:32:41  fred
 * CSCdi36850:  commit fast path queuing for appletalk, decnet, etc.
 *
 * Revision 2.2  1995/07/07  20:40:49  hampton
 * Minor vines timer cleanup. [CSCdi36825]
 *
 * Revision 2.1  1995/06/07 23:15:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Description of Routines Exported from this module. 
 *
 * vines_cache_init:          Initializes VINES cache hash list.
 * vines_cache_update:        Updates VINES cache.
 * vinescache_ager:           Ages VINES routing cache.
 * show_vines_cache:          Displays VINES cache entries.
 * vines_flush_cache_entry    Invalidates VINES cache entry
 */

#include "master.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../vines/msg_vines.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "address.h"
#include "../if/network.h"
#include "ieee.h"
#include "../if/rif.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"
#include "../if/atm.h"
#include "vines_private.h"
#include "vinesfast.h"

/*
 * Local Storage
 */
vinescachetype *vinescache[VINES_HASHLEN];
ulong vcache_entry_count;
ulong vcache_path_count;

ulong vcache_entry_added[VF_LAST_TYPE];
ulong vcache_entry_updated[VF_LAST_TYPE];
ulong vcache_entry_aged[VF_LAST_TYPE];
ulong vcache_entry_removed[VF_LAST_TYPE];
ulong vcache_flush_nbr_cnt;
ulong vcache_flush_svr_cnt;
ulong vcache_flush_idb_cnt;
ulong vcache_flush_table_cnt;
ulong vcache_locked;

#define VINESCACHE_DEBUG_LIMIT 10
ulong vinescache_debug_count;


/**********************************************************************
 *
 *			    INITIALIZATION
 *
 **********************************************************************/

/*
 * vines_fastsetup
 *
 * Turn on the use of fast switching.  Logically AND the configuration
 * option "idb->vines_routecache" with whether or not this interface can
 * support fast switching.  If fastswitching is allowed, set the flag
 * "idb->vines_fastokay" to TRUE and set the appropriate bit in the
 * "fast_switch_flag".
 */
void vines_fastsetup (hwidbtype *hwidb)
{
    hwidbtype *tmpidb;
    boolean fastflag, inuse;

    fastflag = reg_invoke_fast_supported(LINK_VINES, hwidb);
    if (!router_enable
	|| (hwidb->comp_db && (hwidb->enctype != ET_FRAME_RELAY))
	|| hwidb->ppp_compression)
        fastflag = FALSE;
    hwidb->vines_fastokay = (fastflag && hwidb->vines_routecache && vines_running);

    if (hwidb->vines_fastokay) {
	hwidb->fast_switch_flag |= FSF_VINES;
	reg_invoke_register_drivers(FSF_VINES, hwidb);
    } else {
	hwidb->fast_switch_flag &= ~FSF_VINES;
	inuse = FALSE;
	FOR_ALL_HWIDBS(tmpidb) {
	    if (tmpidb->fast_switch_flag & FSF_VINES) {
		inuse = TRUE;
		break;
	    }
	}
	if (!inuse)
	    reg_invoke_register_drivers(FSF_VINES, hwidb);
    }
    vines_flush_cache_by_idb(hwidb);
}

/*
 * vcache_clear_counters
 *
 * Clear all of the counters that track the manipulation of the fast switching
 * cache.  This does not touch the number of current entries, as those depend
 * up on the current state of the table. 
 */
void vcache_clear_counters (void)
{
    int i;

    for (i = 0; i < VF_LAST_TYPE; i++) {
	vcache_entry_added[i] = 0;
	vcache_entry_updated[i] = 0;
	vcache_entry_aged[i] = 0;
	vcache_entry_removed[i] = 0;
    }
    vcache_flush_nbr_cnt = 0;
    vcache_flush_svr_cnt = 0;
    vcache_flush_idb_cnt = 0;
    vcache_flush_table_cnt = 0;
}

/*
 * vinescache_init
 *
 * Initialize each idb to whether or it can support VINES fast switching.
 * Also initializes the hashlist for the fast switching cache table.
 */
void vines_cache_init (void)
{
    vinesidbtype *idb;
    int bucket;
    
    FOR_ALL_VIDBS(idb) {
	hwidbtype *hwidb = idb->phys_idb->hwptr;
	hwidb->vines_routecache = reg_invoke_fast_supported(LINK_VINES, hwidb);
	vines_fastsetup(hwidb);
    }

    FOR_ALL_CACHE_BUCKETS(bucket)
        vinescache[bucket] = NULL;

    vcache_clear_counters();
    vcache_entry_count = 0;
    vcache_path_count = 0;
    vcache_locked = 0;
    vinescache_debug_count = 0;
}


/**********************************************************************
 *
 *			     DEBUGGING
 *
 **********************************************************************/
static inline boolean do_vinescache_debug (void)
{
    if (!vinescache_debug)
	return(FALSE);
    return(vinescache_debug_count++ < VINESCACHE_DEBUG_LIMIT);
}

static inline vfast_entry_type vcache_entry_type (ulong net, ushort host)
{
    if (host != VINES_SERVER)
	return(VF_CLIENT);
    if (server_is_banyan(net))
	return(VF_SERVER);
    return(VF_ROUTER);
}


/**********************************************************************
 *
 *			INSERTION AND DELETION
 *
 **********************************************************************/

/*
 * vinesfast_hash_net
 *
 * hash network number and host id and return index to the cache table.
 */
static inline ushort vinesfast_hash_net (ulong net, ushort host)
{
     ushort bucket;

     bucket =  (net >> 16);
     bucket ^= (net & 0xFFFF);
     bucket ^= host;
     bucket ^= (bucket >> 8);
     bucket &=0xFF;
     return bucket;
}

/*
 * vines_free_entry
 *
 * Release all memory associated with a VINES fast switching cache entry.
 * The entry must not be on a list anywhere.
 */
void vines_free_entry (
    vinescachetype *entry,
    vinescachetype *prev)
{
    leveltype level;
    vinesfastpathtype *tmp;
    
    if (entry == NULL)
	return;

    level = raise_interrupt_level(NETS_DISABLE);
    if (prev)
	prev->next_entry = entry->next_entry;
    entry->next_entry = NULL;
    reset_interrupt_level(level);

    if (entry->all_paths) {
	vcache_entry_count--;
	vcache_path_count -= entry->path_count;
	tmp = entry->all_paths;
	entry->all_paths = NULL;
	free(tmp);
    }
    free(entry);
}

/*
 * vines_flush_cache_entry
 *
 * Given a VINES IP address, invalidate that specific entry in the fast
 * switching cache.
 */
void vines_flush_cache_entry (ulong net, ushort host)
{
    vinescachetype *fast_entry;
    vinescachetype *prev_entry;
    ushort bucket;
    
    if (vcache_locked)
	return;
    if (do_vinescache_debug()) {
	buginf("\nVFAST: FS cache cleared for entry %#z.", net, host);
	bugtrace();
    }

    bucket = vinesfast_hash_net(net, host);
    prev_entry = (vinescachetype *)&vinescache[bucket];
    fast_entry = vinescache[bucket];
    for ( ; fast_entry; fast_entry = fast_entry->next_entry) {
        if ((fast_entry->net == net) && (fast_entry->host == host)) {
	    vcache_entry_removed[fast_entry->entry_type]++;
	    vines_free_entry(fast_entry, prev_entry);
	    fast_entry = prev_entry;
	}
	prev_entry = fast_entry;
    }
}

void vines_invalidate_cache_entry (ulong net, ushort host)
{
    vinescachetype *fast_entry;
    ushort bucket;
    
    if (vcache_locked)
	return;
    if (do_vinescache_debug()) {
	buginf("\nVFAST: FS cache invalidated for entry %#z.", net, host);
	bugtrace();
    }

    bucket = vinesfast_hash_net(net, host);
    fast_entry = vinescache[bucket];
    for ( ; fast_entry; fast_entry = fast_entry->next_entry) {
        if ((fast_entry->net == net) && (fast_entry->host == host)) {
	    fast_entry->flags &= ~V_FC_VALID;
	}
    }
}

/*
 * vines_flush_cache_by_server
 *
 * Invalidate a all entry in the VINES fast switching cache that are
 * associated with this server number.  It is simpler to do this and let
 * them be rebuilt as necessary than to try to remove individual paths
 * from the data structure.
 */
void vines_flush_cache_by_server (vinesnettype *server)
{
    vinescachetype *fast_entry, *prev_entry;
    int bucket;
    
    if (vcache_locked)
	return;
    if (do_vinescache_debug()) {
	buginf("\nVFAST: FS cache cleared for server %#Z.", server->net);
	bugtrace();
    }

    FOR_ALL_CACHE_BUCKETS(bucket) {
	FOR_ALL_BUCKET_ENTRIES_W_PREV(bucket, fast_entry, prev_entry) {
	    if (fast_entry->net == server->net) {
		vines_free_entry(fast_entry, prev_entry);
		fast_entry = prev_entry;
	    }
	    prev_entry = fast_entry;
	}
    }
    vcache_flush_svr_cnt++;
}

/*
 * vines_flush_cache_by_neighbor
 *
 * Invalidate a all entry in the VINES fast switching cache that use the
 * specified interface.  It is simpler to do this and let them be rebuilt
 * as necessary than to try to remove individual paths from the data
 * structure.
 */
void vines_flush_cache_by_neighbor (vinesneighbortype *neighbor)
{
    vinescachetype *fast_entry, *prev_entry;
    vinesfastpathtype *fast_path;
    int bucket;
    
    if (vcache_locked)
	return;
    vcache_flush_nbr_cnt++;
    if (!neighbor->is_router) {
	vines_flush_cache_entry(neighbor->net, neighbor->host);
	return;
    }

    if (do_vinescache_debug()) {
	buginf("\nVFAST: FS cache cleared for neighbor %#z.",
	       neighbor->net, neighbor->host);
	bugtrace();
    }

    FOR_ALL_CACHE_BUCKETS(bucket) {
	FOR_ALL_BUCKET_ENTRIES_W_PREV(bucket, fast_entry, prev_entry) {
	    fast_path = fast_entry->all_paths;
	    do {
		if (fast_path->neighbor == neighbor) {
		    vines_free_entry(fast_entry, prev_entry);
		    fast_entry = prev_entry;
		    break; /* from while loop */
		}
		fast_path = fast_path->next_path;
	    } while (fast_path != fast_entry->all_paths);
	    prev_entry = fast_entry;
	}
    }
}

/*
 * vines_flush_cache_by_idb
 *
 * Invalidate a all entry in the VINES fast switching cache that use the
 * specified interface.  It is simpler to do this and let them be rebuilt
 * as necessary than to try to remove individual paths from the data
 * structure.
 */
void vines_flush_cache_by_idb (hwidbtype *idb)
{
    vinescachetype *fast_entry, *prev_entry;
    vinesfastpathtype *fast_path;
    int bucket;
    
    if (vcache_locked)
	return;
    if (do_vinescache_debug()) {
	buginf("\nVFAST: FS cache cleared for idb %s.", idb->hw_namestring);
	bugtrace();
    }

    FOR_ALL_CACHE_BUCKETS(bucket) {
	FOR_ALL_BUCKET_ENTRIES_W_PREV(bucket, fast_entry, prev_entry) {
	    fast_path = fast_entry->all_paths;
	    do {
		if (hwidb_or_null(fast_path->idb) == idb) {
		    vines_free_entry(fast_entry, prev_entry);
		    fast_entry = prev_entry;
		    break; /* from while loop */
		}
		fast_path = fast_path->next_path;
	    } while (fast_path != fast_entry->all_paths);
	    prev_entry = fast_entry;
	}
    }
    vcache_flush_idb_cnt++;
}

/*
 * vines_bump_cache
 *
 * This routine invokes vines_flush_cache_by_idb for ATM interfaces.
 * We want to flush the cache whenever a data VC goes down
 */

void vines_bump_cache (hwidbtype *idb, idbtype *swidb,
		       boolean immediately)
{
    if (is_atm(idb) &&  idb->vines_routecache) {
	vines_flush_cache_by_idb(idb);
    }
}

/**********************************************************************
 *
 *				UPDATE
 *
 **********************************************************************/

/*
 * et_to_vines_fs_code
 *
 * Convert the driver's encapsulation type into a local type.  The only
 * problem here is ET_SNAP because it is used on multiple interfaces.
 * Other than that, the encapsulation routines tell us exactly what we
 * need to know.  The main purpose of this switch is to convert from the
 * ET enum (48 values) to a smaller, vines specific enum (11 values).
 */
static ulong et_to_vines_fs_code (hwidbtype *hwidb, ulong enctype)
{
    if (is_tunnel(hwidb))
	return(V_FS_TUNNEL);

    switch (enctype) {
      case ET_SNAP:
	if (hwidb->status & IDB_ETHER)
	    return(V_FS_ETHER_SNAP);
	if (hwidb->status & IDB_TR)
	    return(V_FS_TOKEN_SNAP);
	if (hwidb->status & IDB_FDDI)
	    return(V_FS_FDDI);
	if (is_atm(hwidb))
	    return(V_FS_ATM);
	return(V_FS_UNKNOWN);
      case ET_ARPA: 
	if (is_atm(hwidb))
            return(V_FS_ATM);
       return(V_FS_ETHER_ARPA); 
      case ET_VINES_TR:    return(V_FS_TOKEN_VINES);
      case ET_HDLC:	   return(V_FS_HDLC);
      case ET_FR_IETF:     return(V_FS_FRAME_IETF);
      case ET_FRAME_RELAY: return(V_FS_FRAME);
      case ET_SMDS_DXI:    return(V_FS_SMDS_DXI);
      case ET_SMDS:	   return(V_FS_SMDS_CISCO);
      case ET_PPP:	   return(V_FS_PPP);
      case ET_ATM:	   return(V_FS_ATM);
      case ET_ATM_DXI:	   return(V_FS_ATM_DXI);
      default:		   return(V_FS_UNKNOWN);
    }
}

/*
 * vines_cache_add_paths
 *
 * Add all best cost paths to the specified neighbor into the given fast
 * switching cache entry for a server.  This routine may be called
 * multiple times if multiple neighbors can be used as the next hop to a
 * server.
 */
static void vines_cache_add_paths (
    paktype *original_pak,
    ulong dstnet,
    ushort dsthost,
    vinesneighbortype *gw,
    vinescachetype *fast_entry)
{
    vinesfastpathtype *fast_path, *prev_path;
    vinespathtype *path;
    vinesidbtype *vidb;
    vinesiptype *vip;
    hwidbtype *hwidb;
    idbtype *swidb;
    paktype *newpak;
    srbroute_t *rif;
    int i;

    newpak = pak_duplicate(original_pak);
    if (newpak == NULL) {
	return;
    }
    vip = (vinesiptype *)newpak->network_start;

    prev_path = NULL;
    fast_path = fast_entry->all_paths;
    for (i = 0; i < fast_entry->path_count; i++)
	prev_path = fast_path++;

    path = gw->best_path;
    do {
	if (path->metric != gw->metric) {
	    path = path->next_best;
	    continue;
	}
	vidb = path->idb;
	swidb = vidb->phys_idb;
	hwidb = swidb->hwptr;

	if (!vines_interface_up(vidb) || !hwidb->vines_fastokay) {
	    path = path->next_best;
	    continue;
	}

	/*
	 * Call the encapsulation routine to build the header.
	 */
	newpak->datagramstart = newpak->network_start;
	newpak->datagramsize = GETSHORT(&vip->len);
	newpak->if_output = swidb;
	if (!hwidb->vencap(newpak, (long)path)) {
	    errmsg(&msgsym(ENCAPFAILED,VINES), dstnet, dsthost,
		   gw->net, gw->host, swidb->namestring);
	    path = path->next_best;
	    continue;
	}

	/*
	 * Copy the data out.  FDDI and TR don't start with the first
	 * byte.  Make sure to use datagramstart and not mac_start so
	 * that any MIP card overhead will be included.
	 */
	if ((!is_tunnel(hwidb) && newpak->encsize == 0) ||
	    (newpak->encsize > MAXMAC_BYTES)) { 
	    errmsg(&msgsym(CACHEFAILED,VINES), newpak->encsize, dstnet,
		   dsthost, swidb->namestring);
	    path = path->next_best;
	    continue;
	}
	fast_path->reallength = newpak->encsize;
	if (hwidb->status & (IDB_TR | IDB_FDDI)) {
	    fast_path->length = fast_path->reallength -
		(newpak->addr_start - newpak->mac_start);
	    bcopy(newpak->addr_start, fast_path->vinesp_mh.mac_bytes,
		  fast_path->length);
	} else {
	    fast_path->length = fast_path->reallength;
	    bcopy(newpak->datagramstart, fast_path->vinesp_mh.mac_bytes,
		  fast_path->length);
	}

	/*
	 * Check to see if there is a RIF embedded in the encapsulation.
	 */
	if (newpak->rif_start) {
	    rif = (srbroute_t *)newpak->rif_start;
	    fast_path->rifwordlength = ((rif->rc_blth & RC_LTHMASK) / 2);
	} else {
	    fast_path->rifwordlength = 0;
	}
	
	/*
	 * Record the encapsulation used.
	 */
	fast_path->encaptype = et_to_vines_fs_code(hwidb, newpak->enctype);
	if (fast_path->encaptype == V_FS_UNKNOWN) {
	    errmsg(&msgsym(CACHEUNSUPP,VINES), encstring(newpak->enctype),
		   swidb->namestring);
	    path = path->next_best;
	    continue;
	}

	/*
	 * Finish up with the housekeeping
	 */
	fast_path->idb = swidb;
	fast_entry->path_count++;
	fast_path->wordlength = (fast_path->reallength + 1) >> 1;
	fast_path->subint_reference_num =
	    reg_invoke_get_subint_ref_num_from_pak(hwidb->enctype,
						   hwidb,
						   newpak);
	fast_path->neighbor = gw;
	if (prev_path)
	    prev_path->next_path = fast_path;
	prev_path = fast_path;
	fast_path++;
	path = path->next_best;
    } while (path != gw->best_path);
    if (prev_path)
	prev_path->next_path = fast_entry->all_paths;
    fast_entry->next_path = fast_entry->all_paths;
    datagram_done(newpak);
}

/*
 * vines_cache_update
 *
 * Updates the VINES cache with the current VINES datagram.  The calling
 * must check for whether or not fast switching is okay on the output
 * interface, and for the correct output encapsulation.  This routine
 * will promote entries to top of the hash list if they've drifted down
 * aways.
 */
void vines_cache_update (paktype *pak)
{
    vinesiptype *vip;
    vinescachetype *fast_entry, *prev_entry;
    leveltype status;
    ushort bucket;
    ushort pathcount, dhost;
    charlong dnet;
    vinesneighbortype *gw;
    vinesnettype *server;
    vinesroutetype *route;
    
    /*
     * Conditions for acceptance:
     * - must be transit (not sourced by this router).
     * - output interface must be different that input interface.
     * - output must be via an MCI, SCI, Cbus, or 16/4 token ring interface.
     * - serial output must use supported encapsulation
     * - no multicast destinations (handled by the caller)
     *
     * Also don't learn if the input interface doesn't support fast
     * switching.  This prevents building a cache entry unless someone
     * can use it.
     */
    if ((pak->if_input == NULL) || (pak->if_output == NULL) ||
	(pak->if_input == pak->if_output))
        return;
    if (!pak->if_output->hwptr->vines_fastokay ||
	!reg_invoke_fast_supported(LINK_VINES, pak->if_output->hwptr))
	return;
    if (!pak->if_input->hwptr->vines_fastokay ||
	!reg_invoke_fast_supported(LINK_VINES, pak->if_input->hwptr))
	return;
    if (pak->flags & PAK_ENCAPS_ONLY) {
	return;
    }
    /*
     * Find or create a cache pointer.
     */
    vip = (vinesiptype *)pak->network_start;
    dnet.d.lword = GET2WD(vip->ddstnet);
    dhost = vip->dsthost;
    bucket =  dhost;
    bucket ^= dnet.d.sword[0];
    bucket ^= dnet.d.sword[1];
    bucket ^= (bucket >> 8);
    bucket &=0xFF;
    prev_entry = (vinescachetype *)&vinescache[bucket];
    FOR_ALL_BUCKET_ENTRIES_W_PREV(bucket, fast_entry, prev_entry) {
        if ((fast_entry->net == dnet.d.lword) &&
	    (fast_entry->host == dhost))
            break;
    }

    /*
     * Get a new entry or unlink a current entry.  Unlinking an
     * existing entry prevents the fast switching code from trying to
     * use it while it is being modified.
     */
    if (fast_entry == NULL) {
        fast_entry = malloc(sizeof(vinescachetype));
        if (fast_entry == NULL)
            return;
	fast_entry->entry_type = vcache_entry_type(dnet.d.lword, dhost);
	vcache_entry_added[fast_entry->entry_type]++;
    } else {
	status = raise_interrupt_level(NETS_DISABLE);
	prev_entry->next_entry = fast_entry->next_entry;
	fast_entry->next_entry = NULL;
	reset_interrupt_level(status);
	vcache_entry_count--;
	vcache_path_count -= fast_entry->path_count;
	vcache_entry_updated[fast_entry->entry_type]++;
    }

    /*
     * Update the cache entry.
     */
    fast_entry->flags |= V_FC_VALID;
    fast_entry->net = dnet.d.lword;
    fast_entry->host = dhost;
    GET_TIMESTAMP(fast_entry->lastupdate);
    gw = vneigh_FindEntry(dnet.d.lword, dhost);
    if (gw && !(gw->newflags & V_N_USE_ROUTE)) {
        /*
	 * Here we are dealing with a station that is directly accessible
	 * to this router.  Add all of the paths to this neighbor into
	 * the cache entry.  First, insure that there is enough space in
	 * the data structure.
	 */
	pathcount = gw->path_count;
	if (pathcount == 0) {
	    errmsg(&msgsym(CONSISTENCY, VINES), "populating fast cache from neighbor entry");
	    vines_free_entry(fast_entry, NULL);
	    return;
	}
        if (fast_entry->path_count != pathcount) {
	    if (fast_entry->all_paths != NULL)
		free(fast_entry->all_paths);
	    fast_entry->all_paths = malloc(pathcount * sizeof(vinesfastpathtype));
            if (fast_entry->all_paths == NULL) {
                vines_free_entry(fast_entry, NULL);
                return;
            }
	}

	fast_entry->path_count = 0;
	fast_entry->next_path = NULL ;
	vines_cache_add_paths(pak, dnet.d.lword, dhost, gw, fast_entry);
    } else {
        /*
	 * Here we are dealing with a station that is not directly
	 * accessible to this router.  Look for the corresponding
	 * server's entry, count up all of the best paths to that server,
	 * and then insure that there is enough space in the data
	 * structure.
	 */
        if (!(server = vroute_FindEntry(dnet.d.lword))) {
            vines_free_entry(fast_entry, NULL);
            return;
        }
	pathcount = vines_server_total_paths(server);
	if (pathcount == 0) {
	    errmsg(&msgsym(CONSISTENCY, VINES), "populating fast cache from server entry");
	    vines_free_entry(fast_entry, NULL);
	    return;
	}
        if (fast_entry->path_count != pathcount) {
	    if (fast_entry->all_paths != NULL)
		free(fast_entry->all_paths);
	    fast_entry->all_paths = malloc(pathcount * sizeof(vinesfastpathtype));
            if (fast_entry->all_paths == NULL) {
		vines_free_entry(fast_entry, NULL);
		return;
            }
        }
	
	/*
	 * Now cycle through all of the best cost next hops, adding in
	 * entries for each of the best paths for reaching them.
	 */
        fast_entry->path_count = 0;
        fast_entry->next_path = NULL;
	route = server->best_route;
	do {
	    vines_cache_add_paths(pak, dnet.d.lword, dhost, route->gw, fast_entry);
	    route = route->next_best;
	} while (route != server->best_route);
    }

    /*
     * All done.  Make sure that we actually ended up with an entry, and
     * then link it into the cache.
     */
    if (!fast_entry->path_count) {
        vines_free_entry(fast_entry, NULL);
        return;
    }
    if (!fast_entry->all_paths || !fast_entry->next_path) {
	errmsg(&msgsym(CONSISTENCY, VINES), "creating fast cache path entries");
	vines_free_entry(fast_entry, NULL);
        return;
    }
    status = raise_interrupt_level(NETS_DISABLE);
    fast_entry->next_entry = vinescache[bucket];
    vinescache[bucket] = fast_entry;
    reset_interrupt_level(status);
    vcache_entry_count++;
    vcache_path_count += fast_entry->path_count;
}

/**********************************************************************
 *
 *			     HOUSEKEEPING
 *
 **********************************************************************/

/*
 * vinescache_ager
 *
 * Age VINES routing cache, or flush it entirely.
 */
void vinescache_ager (boolean flushflag)
{
    vinescachetype *fast_entry, *prev_entry;
    int bucket;
    boolean table_changed;

    if (!vines_running)
        return;
    if (flushflag)
	vcache_flush_table_cnt++;
    table_changed = FALSE;
    FOR_ALL_CACHE_BUCKETS(bucket) {
        prev_entry = (vinescachetype *) &vinescache[bucket];
        fast_entry = prev_entry->next_entry;
        while (fast_entry) {
            if (flushflag ||
		CLOCK_OUTSIDE_INTERVAL(fast_entry->lastupdate, RCACHE_TIMEOUT)){
		if (do_vinescache_debug()) {
		    buginf("\nVFAST: FS cache cleared by ager for %#z.",
			   fast_entry->net, fast_entry->host);
		    bugtrace();
		}
		if (!flushflag)
		    vcache_entry_aged[fast_entry->entry_type]++;
                vines_free_entry(fast_entry, prev_entry);
                fast_entry = prev_entry;
		table_changed = TRUE;
            }
            prev_entry = fast_entry;
            fast_entry = fast_entry->next_entry;
        }
	process_suspend_if_req();
    }
}

/**********************************************************************
 *
 *			       DISPLAY
 *
 **********************************************************************/

/*
 * show_vines_cache
 *
 * Display contents of VINES routing cache
 */
void show_vines_cache (
    ulong net,
    ushort host,
    vinesneighbortype *neighbor,
    vinesidbtype *target)
{
    vinescachetype *fast_entry, *next_entry;
    int bucket, i, depth, length, count;
    vinesfastpathtype *path_ptr, *next_ptr;
    boolean first;

    automore_enable(NULL);
    printf("\nVINES fast switching cache information:");
    printf("\n  Current: %u entries, %u paths", vcache_entry_count,
	   vcache_path_count);
    if (vcache_locked)
	printf(", locked (%d)", vcache_locked);
    printf("\n  History:");
    printf("\n      Added: %-10u server, %-10u router, %-10u client",
	   vcache_entry_added[VF_SERVER], vcache_entry_added[VF_ROUTER],
	   vcache_entry_added[VF_CLIENT]);
    printf("\n    Updated: %-10u server, %-10u router, %-10u client",
	   vcache_entry_updated[VF_SERVER], vcache_entry_updated[VF_ROUTER],
	   vcache_entry_updated[VF_CLIENT]);
    printf("\n    Expired: %-10u server, %-10u router, %-10u client",
	   vcache_entry_aged[VF_SERVER], vcache_entry_aged[VF_ROUTER],
	   vcache_entry_aged[VF_CLIENT]);
    printf("\n    Removed: %-10u server, %-10u router, %-10u client",
	   vcache_entry_removed[VF_SERVER], vcache_entry_removed[VF_ROUTER],
	   vcache_entry_removed[VF_CLIENT]);
    printf("\n    Flushes: %-10u by neighbor,  %-10u by server",
	   vcache_flush_nbr_cnt, vcache_flush_svr_cnt);
    printf("\n             %-10u by interface, %-10u entire table",
	   vcache_flush_idb_cnt, vcache_flush_table_cnt);
    automore_header("\nHash   Destination       Int     Age  Length  Type  MAC Header\n");
    FOR_ALL_CACHE_BUCKETS(bucket) {
	if (!vinescache[bucket])
	    continue;
	next_entry = NULL;
        for (fast_entry = vinescache[bucket], depth = 0; fast_entry; depth++, 
	     fast_entry = next_entry) {
	    if (net) {
		if (net != fast_entry->net) {
		    next_entry = fast_entry->next_entry;
		    continue;
		}
		if (host && (host != fast_entry->host)) {
		    next_entry = fast_entry->next_entry;
		    continue;
		}
	    }
	    mem_lock(fast_entry);
	    mem_lock(fast_entry->all_paths);
	    first = TRUE;
            path_ptr = fast_entry->all_paths;
            next_ptr = fast_entry->next_path;
            for (count = fast_entry->path_count; count; count--, path_ptr++) {
		/*
		 * Check command line qualifiers
		 */
		if (target && (target->phys_idb != path_ptr->idb))
		    continue;
		if (neighbor && (neighbor != path_ptr->neighbor))
		    continue;

		/*
		 * Print common information.
		 */
		if (first) {
		    printf("%02x/%02x  %#16z", bucket, depth, fast_entry->net,
			   fast_entry->host);
		    first = FALSE;
		} else {
		    printf("%23c",' ');
		}

		/*
		 * Print one line for this path in the cache entry.
		 */
		printf(" %c%5s %-5d   %02d/%02d  %4d  ",
		       ((path_ptr == next_ptr) ? '*' : ' '),
		       path_ptr->idb->short_namestring,
		       ELAPSED_TIME(fast_entry->lastupdate)/ONESEC,
		       path_ptr->length, path_ptr->reallength,
		       path_ptr->encaptype);
                length = path_ptr->length;
                for (i = 0; i < length; i++) {
                    if ((i != 0) && ((i % 14 /* 16 */) == 0))
                        printf("\n%52c",' '); /* 48 */
                    printf("%-02x", path_ptr->vinesp_mh.mac_bytes[i]);
                }
                printf("\n");
            }
	    next_entry = fast_entry->next_entry;
	    free(fast_entry->all_paths);
	    free(fast_entry);
        }
	if (automore_quit())
	    break;
    }
    automore_disable();
}
