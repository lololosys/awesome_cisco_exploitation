/* $Id: at_fast.c,v 3.7.12.10 1996/08/28 00:51:13 hampton Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_fast.c,v $
 *------------------------------------------------------------------
 * AppleTalk fast switching support
 *
 * August 1990, David Edwards
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_fast.c,v $
 * Revision 3.7.12.10  1996/08/28  00:51:13  hampton
 * Migrate Desktop Protocols away from the one minute and one second
 * registries.  [CSCdi67383]
 * Branch: California_branch
 *
 * Revision 3.7.12.9  1996/07/10  22:18:03  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.7.12.8  1996/05/10  01:36:05  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.7.12.7  1996/05/09  14:10:42  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.12.6  1996/05/04  00:33:49  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Atalk protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on IRB/CRB is enabled or not.
 *
 * Revision 3.7.12.5  1996/05/02  08:39:23  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.7.12.4  1996/04/27  05:44:43  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.7.12.3.14.1  1996/04/27  06:33:08  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.12.3.4.1  1996/04/08  01:44:09  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.7.12.3  1996/03/27  18:11:17  slin
 * CSCdi52713:  NULL pointer deference
 * Branch: California_branch
 *
 * Revision 3.7.12.2  1996/03/23  01:26:13  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.7.12.1  1996/03/18  18:52:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.16.1  1996/03/22  09:34:52  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7  1996/02/15  18:01:39  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.6  1996/02/13  08:10:43  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.5.34.1  1996/03/05  05:48:42  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.28.1  1996/02/27  20:40:18  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5.6.1  1996/02/15  19:12:13  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.5  1995/12/08  04:55:15  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.4  1995/11/20  22:00:27  kleung
 * CSCdi42895:  Code can cause a segment violation crash
 *
 * Revision 3.3  1995/11/17  08:42:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:56:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:49:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/07/13  06:49:35  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.5  1995/07/10 05:31:48  fred
 * CSCdi36850:  commit fast path queuing for appletalk, decnet, etc.
 *
 * Revision 2.4  1995/06/30  18:32:41  bbenson
 * CSCdi36642:  AppleTalk fastcache getting populated with phase1 LANE
 * - LANE doesn't support phase1 encaps at fastswitch.  Also check for
 *   encaping unicast packets on BUS.
 *
 * Revision 2.3  1995/06/22  22:46:00  wmay
 * CSCdi36246:  Appletalk doesnt fast-switch smds
 *
 * Revision 2.2  1995/06/21  02:50:37  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  20:07:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "media_registry.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "address.h"
#include "../if/fddi.h"
#include "../if/network.h"
#include "atalk_private.h"
#include "../atalk/at_fast.h"
#include "../if/atm.h"
#include "../lane/lane.h"

/* 
 * Local Storage -- this is used by the fast-switching modules only.
 */
rbTree	*atfast_cache;

ulong atfast_cacheVersion = 0L;

/*
 * Return a pointer to the fast-switching cache tree root. Everyone
 * outside the fast-switching modules uses this.
 */
rbTree *
atfast_GetCachePtr (void)
{
    return (atfast_cache);
}

ulong
atfast_CacheVersion (void)
{
    return (atfast_cacheVersion);
}

/*
 * atfast_Reason
 * Return a string describing the status of fast switching on the 
 * interface.
 */
char*
atfast_Reason (atalkidbtype* idb)
{
    char *buf;
    hwidbtype *hwidb;

    hwidb = hwidb_or_null(idb->phys_idb);
    if (!hwidb)
        return ("undefined");   /* should not happen! */

    if (!idb->atalk_enabled)
	return ("undefined");	/* should not be here! */
    if (hwidb->atalk_fastokay)
	return ("enabled");
    switch (hwidb->atalk_routecache) {
    case fsUnknown:
	return ("not initialized");
    case fsUserDisabled:
	return ("disabled by configuration");
    case fsNotSupported:
	return ("not supported by hardware");
    case fsOldHardware:
	return ("not supported by firmware");
    case fsPortDown:
	return ("disabled, port down");
    case fsNotRouting:
	return ("disabled, port initializing");
    case fsAccessControl:
	return ("disabled, port has access control");
    case fsIncompLinkProtocol:
	return ("not supported by link protocol");
    case fsPriorityList:
	return ("disabled, priority list defined");
    case fsDialOnDemand:
	return ("disabled, Dial on Demand specified");
    default:
	buf = string_getnext();
	sprintf(buf, "disabled, unknown reason = %d",
		hwidb->atalk_routecache);
	return (buf);
    }
}

/* 
 * atfast_Initialize
 * Initialize AT fast switching globals, called once at startup.
 */
void
atfast_Initialize (void)
{
    atfast_cache = RBTreeCreate("AppleTalk", "AT", "AppleTalk Route Cache",
				at_print_key_interval, &appletalk_debug->flag);
    
    atfast_cacheVersion = 1L;
}
  
/*
 * atalk_ReinitIF
 * Reinitialize an interface for fast switching.  This is where we generate
 * any hardware related messages which is why it is not called during startup.
 */
void
atalk_ReinitIF (atalkidbtype* idb)
{
    hwidbtype* hwidb;
    
    atfast_SetupIF(idb);
    hwidb = hwidb_or_null(idb->phys_idb);
    if (!hwidb)
	return;

    if (hwidb->atalk_routecache == fsOldHardware) 
	errmsg(AT_ERR_OLDMCI, idb_get_namestring(idb->phys_idb));    
}

/*
 * atfast_SetupIF
 * Enable fast switching if appropriate.  Called whenever conditions
 * indicate a change in status may be needed.  Note: this will
 * invalidate the current cache.
 */
void
atfast_SetupIF (atalkidbtype* swidb)
{
    int fsreason = fsEnabled;
    hwidbtype *idb;
    int oldreason;

    if (!swidb)
	return;

    idb = hwidb_or_null(swidb->phys_idb);

    if (!idb)
	return;

    oldreason = idb->atalk_routecache;
    idb->fast_switch_flag &= ~(FSF_SNAP_APPLE | FSF_APPLE);
    idb->atalk_fastokay = FALSE;

				/* Check for invariant conditions */
    switch (idb->atalk_routecache) {
    case fsOldHardware:		/* Or do you like trashed data and yo-yo gws */
    case fsUserDisabled:	/* Definitely no need to check further */
	return;
    default:			/* Do more checking... */
	break;
    }

    if (!atalk_running || !swidb->atalk_enabled)
	fsreason = fsUnknown;
    else if (!atalkif_InterfaceUp(swidb))
	fsreason = fsPortDown;
    else if (!atalkif_operational(swidb))
	fsreason = fsNotRouting;
    else if (swidb->atalk_accesslist != 0)
	fsreason = fsAccessControl;
    else if (is_ddr(idb)) 
	fsreason = fsDialOnDemand;
    else if ((idb->comp_db && (idb->enctype != ET_FRAME_RELAY))
	    || idb->ppp_compression)
        fsreason = fsNotSupported;

    /*
     * If we should enable fast switching, invoke the driver registration
     * service point.  It will set a reason in idb->atalk_routecache to
     * tell us whether it succeeded or failed.  If we should not enable
     * fast switching, remember the reason why.
     */
    if (fsreason == fsEnabled) {
	reg_invoke_register_drivers((FSF_APPLE | FSF_SNAP_APPLE), idb);
	/* We are now up if fsreason == fsEnabled, therefore can now reset */
	if (idb->atalk_routecache == fsNotRouting) {
	    idb->atalk_routecache = fsEnabled;
	}
    } else {
	idb->atalk_routecache = fsreason;
    }
    idb->atalk_fastokay = (idb->atalk_routecache == fsEnabled);
    if (oldreason != idb->atalk_routecache)
	if (oldreason == fsEnabled)
	    atalk_cache_increment_version(NULL, NULL, FALSE);
}

/*
 * atalk_fastsetup
 * Reinitialize the interface for fast switching
 * Called by the SERVICE_FAST_SETUP function.
 */

void atalk_fastsetup (hwidbtype *hwidb)
{
    idbtype *swidb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	atfast_SetupIF(atalk_getidb(swidb));
    }
}

/*
 * atalk_SetCacheDelay()
 *
 *   This routine is used to set the delay counter
 *   for load balancing.  Cache entries will only be
 *   used if (delaycount == 0).
 */
static void
atalk_SetCacheDelay (atcachetype *primary, ushort net)
{
    atalk_rdb_t *rdb;
    atalk_pdb_t *next_pdb;
    hwidbtype *hwidb;
    short count;

    rdb = atroute_LookupRoute(atroute_GetTree(), net, TRUE);
    if (rdb && rdb->best_path)
	primary->delaycount = atroute_GetEqualPathCount(rdb->best_path);
    else {
        primary->delaycount = 0;
	if (at_debug(atalklb_debug, ATALK_NULL_MASK))
	    buginf("atalk_SetCacheDelay::net = %d, delay = %d\n", 
                   net, primary->delaycount);
	return;
    }
   
    /* 
     * adjust for interfaces that are cache disabled 
     */
    next_pdb = rdb->best_path;
    count = primary->delaycount + 1;
    while (count--) {
	hwidb = next_pdb->idb->phys_idb->hwptr;
	if (!hwidb->atalk_fastokay)
	    primary->delaycount--;
	next_pdb = next_pdb->equalpath;
    }

    if (at_debug(atalklb_debug, ATALK_NULL_MASK))
        buginf("atalk_SetCacheDelay::net = %d, delay = %d\n", 
               net, primary->delaycount);
}

/*
 * atalk_DeleteEqualCacheList()
 *
 *   Remove equal cost cache entries that
 *   are linked to the primary cache entry.
 */
static void 
atalk_DeleteEqualCacheList (atcachetype *primary)
{
    atcachetype *next, *prev;

    if (at_debug(atalklb_debug, ATALK_NULL_MASK))
        buginf("atalk_DeleteEqualCacheList\n");
     
    next = primary->equalcache;
    while (primary != next) {
        prev = next;
        next = next->equalcache;
	prev->equalcache = NULL;
        free (prev);
    }
    primary->delaycount = 0;
    primary->equalcount = 0;
    primary->equalcache = primary;
    primary->next_equalcache = primary;
}

/*
 * atalk_AddEqualCache()
 *
 *   Return a newly created cache entry or
 *   the existing cache entry
 */
static atcachetype*
atalk_AddEqualCache (atcachetype *primary, idbtype *idb, ushort net)
{
    atcachetype *cptr, *ncptr;
    short count;

    /* if the cache entry is out of date, delete 
     * all equal cost entries that are linked
     * to the primary cache entry and return
     * the primary cache entry
     */
    if (primary->version != atfast_cacheVersion) {
	atalk_DeleteEqualCacheList(primary);
	atalk_SetCacheDelay(primary, net);
        return (primary);
    }

    if (primary->idb == idb)
        return (primary);

    /* scan through the equal cache list */
    cptr = primary;
    count = primary->equalcount;

    while (count) {
        cptr = cptr->equalcache;
	if (cptr->idb == idb)
            return (cptr);
        count--;
    }

    /* create a new one if not in the list */
    ncptr = malloc(sizeof(atcachetype));
    if (ncptr == NULL) {
        if (at_debug(atalklb_debug, ATALK_NULL_MASK))
	    buginf("Unable to create equal cache entry.\n");
        return (cptr);
    }

    if (at_debug(atalklb_debug, ATALK_NULL_MASK))
        buginf("atalk_AddEqualCache::idb = %s, net = %d\n", 
               cptr->idb->namestring, net);

    bzero(ncptr, sizeof(atcachetype));
    cptr->equalcache = ncptr;
    ncptr->equalcache = primary;
    primary->equalcount++;
    primary->delaycount--;
    return (ncptr);
}


static void
atfast_DeleteEntry (atcachetype *entry)
{
    if (entry->equalcount)
        atalk_DeleteEqualCacheList(entry);

    RBTreeDelete(atfast_cache,(treeLink *)entry);
}

static boolean 
atfast_AgeEntry (treeLink *link, void *pdata)
{
    atcachetype *ptr = (atcachetype *)link;
    boolean	flushflag = (boolean)pdata;

    if ((ptr->version != atfast_cacheVersion) || flushflag) {
	if (   flushflag
	    || CLOCK_OUTSIDE_INTERVAL(ptr->lastupdate, ATRCACHE_TIMEOUT)
	    || !atalkif_InterfaceUp(ptr->atalkidb)) {
	    atfast_DeleteEntry(ptr);
	}
    } else
	GET_TIMESTAMP(ptr->lastupdate);

    return (TRUE);
}

/*
 * atalk_CacheAger
 * Age the fast switching cache entries.
 */
static void
atalk_CacheAger (boolean flushflag)
{
    int	status;

    /*
     * If 0, cache has never been used. If it is 'loop time' and we
     * loop back to zero, then the cache must be changing so frequently,
     * it won't be zero for long
     */
    if (atfast_cacheVersion == 0L)
        return;
    status = raise_interrupt_level(NETS_DISABLE);
    RBTreeForEachNode(atfast_AgeEntry, (void *)flushflag, atfast_cache, FALSE);
    reset_interrupt_level(status);
}

/*
 * atalk_fscache_age
 * Age the fast switching cache entries.
 */

void atalk_fscache_age (void)
{
    atalk_CacheAger(FALSE);
}

/*
 * atalk_fscache_flush
 * Flush the fast switching cache entries.
 */

void atalk_fscache_flush (void)
{
    atalk_CacheAger(TRUE);
}


/*
 * atalk_UpdateCache
 * Update our fast switching cache based upon an outgoing packet.  This is
 * a public routine.
 */
void
atalk_UpdateCache (paktype* pak)
{
    atcachetype *ptr;
    idbtype	*idb;
    atalkidbtype *atalkidb;
    hwidbtype	*hwidb;
    boolean	newflag;
    ushort	net;
    uchar	node;
    uchar 	*ds;
    netTypes	nettype;
    int		len;
    treeKey	key;
    trrif_t	*trh = NULL;		/* pointer to a TR rif field */
    uchar	riflen = 0;		/* length of rif, in bytes */
    uchar	phaseOne;		/* != 0 when it's a PhI entry */
    ushort	encaptype = AT_FS_MIN;
#ifdef NOT_IMPLEMENTED_YET
    ushort	*enclen = (ushort *)(pak->datagramstart - sizeof(ushort));
#endif

    /*
     * If either input or output interface is null, or if they are the
     * same (ie, a loopback packet), then don't bother.
     */
    if (   (pak->if_input == NULL)
	|| (pak->if_output == NULL)
	|| (pak->if_output == pak->if_input))
	return;
    idb = pak->if_output;
    hwidb = idb->hwptr;
    if (!hwidb->atalk_fastokay)
	return;
    atalkidb = atalk_getidb(idb);

    phaseOne = FALSE;
    ds = pak->datagramstart;

    if (pak->atalk_dstmcast != ATALK_UNICAST)
	return;

    if (hwidb->status & (IDB_ETHER|IDB_BVI)) {
	if (ds[ETALKETHERDST] & ETHER_MAC_MULTICAST)
	    return;			/* don't populate cache with m-cast */
	if (atalkif_net_extended(atalkidb)) {
	    len = ETHER_ARPA_ENCAPBYTES - 2; /* Omit the length bytes */
	    encaptype = AT_FS_ETHER_SNAP;
	} else {
	    len = ETALK_ENCAPBYTES; /* Include the AppleTalk lap bytes */
	    phaseOne = TRUE;
	    encaptype = AT_FS_ETHER_ARPA;
	}
    } else if (hwidb->status & IDB_FDDI) {
	switch (hwidb->type) {
	  case IDBTYPE_FDDIT:
	    if (ds[1] & ETHER_MAC_MULTICAST)
		return;
	    break;
	  case IDBTYPE_BSI:
	  case IDBTYPE_FDDI:
	    if (ds[1] & TRING_MAC_MULTICAST)
		return;
	    break;
	  default:
	    return;
	}
	encaptype = AT_FS_FDDI;
	len = FDDI_ENCAPBYTES - 1;	/* omit the LLC_FC byte */
	ds++;
    } else if (   (hwidb->status & IDB_SERIAL)
	       && (   hwidb->enctype == ET_HDLC
		   || hwidb->enctype == ET_FRAME_RELAY
		   || hwidb->enctype == ET_FR_IETF
		   || hwidb->enctype == ET_ATM_DXI
		   || (hwidb->enctype == ET_PPP && !is_ddr(hwidb))
		   || hwidb->enctype == ET_SMDS)) {
	if (!atalkif_net_extended(atalkidb))
	    return;

	len = pak->encsize;
	if (len == 0 || len > MAXATMACSTRING) {
	    buginf("\nAT: encapsulation failed, pak->encsize = %d",len);
	    return;
	}
	switch (pak->enctype) {
	  case ET_PPP:
	      if (hwidb->ppp_bridge_atalk)
		  return;
	      encaptype = AT_FS_PPP;
	      break;
	  case ET_HDLC:		encaptype = AT_FS_HDLC;		break;
	  case ET_FR_IETF:	encaptype = AT_FS_FRAME_IETF;	break;
	  case ET_FRAME_RELAY:	encaptype = AT_FS_FRAME;	break;
	  case ET_ATM_DXI:	encaptype = AT_FS_ATM_DXI;	break;
	  case ET_SMDS:		encaptype = AT_FS_SMDS;		break;
	  default:
	    return;
	}
    } else if (is_atm(hwidb)) {
	if (is_atm_lane(idb)) {
	    if ((atalkif_net_extended(atalkidb) == FALSE) ||
		(is_atm_lane(idb) && (pak->flags & PAK_ENCAPS_ONLY))) {
		return;
	    }
	}
	len = pak->encsize;
	encaptype = AT_FS_ATM;
    } else if (hwidb->status & IDB_TR) {
	if (pak->enctype != ET_SNAP)
	    return;
	trh = (trrif_t *)pak->datagramstart;
	len = TRING_8025_ENCAPBYTES - 2;/* don't copy the AC and FC bytes */
	ds  += 2;			/* shuffle over the AC and FC bytes */
	if (trh->saddr[0] & TR_RII) {
	    riflen = trh->rc_blth & RC_LTHMASK;
	    len   += riflen;
	}
	encaptype = AT_FS_TOKEN_SNAP;
    } else if (hwidb->status & IDB_LEX) {
	len = pak->network_start - pak->datagramstart;
	if (atalkif_net_extended(atalk_getidb(idb)) == FALSE) {
	    phaseOne = TRUE;
	    encaptype = AT_FS_LEX_ARPA;
	} else {
	    encaptype = AT_FS_LEX_SNAP;
	}
    } else if (is_tunnel(hwidb)) {
	len = pak->encsize;
    } else {				/* an interface we don't fast-switch */
	return;
    }

    ATASSERT(len <= MAXATMACSTRING);
    newflag = FALSE;

    /*
     * Cache consistency checks.
     * Don't insert entries for the following addresses:
     * Network #'s which are: 0 or in startup range
     * Node #'s which are:    0 or 255 (broadcast).
     */
    if ((net = atalk_pak_dnet(pak)) == 0)
	return;
    nettype = atalkif_dstnettype(atalkidb,net);
    if (nettype == netStartupRange)
	return;
    node = atalk_pak_dnode(pak);
    if (node == ATALK_BROAD_ADDR || node == 0)
	return;

    key.l = atalk_address(net,node);
    ptr = (atcachetype *)RBTreeSearch(atfast_cache, key);
    if (ptr == NULL) {
	atalkidbtype *ifxidb;
	
	ifxidb = atalk_findIDB(net);
	if (ifxidb != NULL)
	    if (atalkif_InterfaceUp(ifxidb) && 
		atalkif_IsMyAddress(ifxidb, atalk_address(net,node)))
		return;
	ptr = (atcachetype *)RBTreeGetFreeNode(atfast_cache);
	if (ptr == NULL) {
	    ptr = malloc(sizeof(atcachetype));
	    if (ptr == NULL)
		return;
	} else
	    memset(ptr, 0, sizeof(atcachetype));
	newflag = TRUE;

	ptr->equalcount = 0;
	ptr->equalcache = ptr;
	ptr->next_equalcache = ptr;

	if (atalk_MaxPaths > 1) {
	    /*
	     * For load balancing, the first parallel path # of
	     * packets will be handled by the *process* switch,
	     * so we are going to set the delaycount here.
	     */
	    atalk_SetCacheDelay(ptr, net);
	}
    }
    else {
        if (atalk_MaxPaths > 1) 
	    ptr = atalk_AddEqualCache(ptr, idb, net);
    }

    ptr->net        = net;
    ptr->node       = node;
    ptr->flags      = 0;
    ptr->encaptype  = encaptype;
    ptr->subint_reference_num =
	reg_invoke_get_subint_ref_num_from_pak(hwidb->enctype, hwidb, pak);

    /*
     * Small explanation about RIF lengths:
     * In the MCI/CTR/TRIP/cBus fast-switching code, we need the
     * length of the RIF in both bytes and words. Rather than
     * use only bytes or words and then convert from one to the other
     * in the fast-switch code, we will pre-compute what is needed
     * in the f/s path and use it directly.
     */
    ptr->b_riflen   = riflen;		/* RIF length, in bytes */
    ptr->w_riflen   = riflen/2;		/* RIF length, in 16-bit words */
    ptr->phaseOne   = phaseOne;
    ptr->byte_len   = len;		/* total length of entry, rif & all */
    ptr->word_len   = btow(len);	/* do this once, and not at f/s time */
    ptr->idb        = idb;
    ptr->hwif       = hwidb;
    ptr->atalkidb   = atalkidb;
    GET_TIMESTAMP(ptr->lastupdate);
    bcopy(ds, ptr->m.macstring, len);
    bcopy(ds+1, ptr->s.macstring, len-1);
    ptr->version    = atfast_cacheVersion; /* do this last, enables cache */
    if (newflag)
	RBTreeInsert(key, atfast_cache, (treeLink *)ptr);
}

/*
 * atalk_InvalidateCache
 * Invalidates the entire AT fast switching cache.
 */
void
atalk_InvalidateCache (char *rname)
{
    if (at_debug(aarp_debug, ATALK_NULL_MASK)
	&& at_debug(appletalk_debug, ATALK_NULL_MASK))
	buginf("\nAT: Cache invalidation by %s", rname);
    atfast_cacheVersion++;
}

void atalk_cache_increment_version (hwidbtype *hwidb, idbtype *swidb, 
				    boolean immediately)
{
    /*
     * invalidate cache only if fs enabled for proto on this interface,
     * or if no interface supplied
     */

    if (!hwidb)			/* old behavior, invalidate cache */
	atalk_InvalidateCache("registry");
    else if (hwidb->atalk_fastokay)
	atalk_InvalidateCache("registry");
}

static void
atfast_show_pvt_entry (atcachetype *entry)
{
    ushort	ix;

    printf("\n  Cache Entry for %d.%d, Ph%c %s RIF flags %02x ver %d len %d",
	   entry->net, entry->node, (entry->phaseOne ? '1':'2'),
	   (entry->b_riflen ? "  " : "no"), entry->flags, entry->version,
	   entry->byte_len);
    printf("\n    Interface: %s, time of last update: ",
	   idb_get_namestring(entry->idb));
    print_dhms(entry->lastupdate);
    printf("\n    Entry: ");
    ix= 0;
    do {
	printf("%02X", entry->m.macstring[ix]);
	ix++;
	if ((ix % 32) == 0) {
	    printf("\n%11c", ' ');
	}
    } while (ix < entry->byte_len);
}
void
atfast_show_pvt_cache (ataddrtype addr)
{
    treeKey	key;
    atcachetype	*entry;
    atcachetype	*next_entry;

    printf("\n  Cache's current version: %lu", atfast_CacheVersion());
    if (addr != 0) {
	key.l = addr;
	entry = (atcachetype *)RBTreeSearch(atfast_cache, key);
	if (entry)
	    atfast_show_pvt_entry(entry);
	else
	    printf("\n%%No cache entry for %a", addr);
    } else {
	entry = KeepFirstEntry(atfast_cache);
	while (entry) {
	    next_entry = KeepNextEntry(atfast_cache, entry);
	    atfast_show_pvt_entry(entry);
	    entry = next_entry;
	}
    }
}
