/* $Id: smrp_fast.c,v 3.3.64.6 1996/08/28 00:51:26 hampton Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_fast.c,v $
 *------------------------------------------------------------------
 * SMRP fast switching support
 *
 * August 1995, Kent Leung
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_fast.c,v $
 * Revision 3.3.64.6  1996/08/28  00:51:26  hampton
 * Migrate Desktop Protocols away from the one minute and one second
 * registries.  [CSCdi67383]
 * Branch: California_branch
 *
 * Revision 3.3.64.5  1996/06/28  23:25:57  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.64.4  1996/05/09  14:40:49  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.64.3  1996/05/02  08:44:40  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.3.64.2  1996/04/27  06:46:14  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.20.2  1996/04/26  00:50:21  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.3.20.1  1996/02/23  20:54:19  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.3.64.1  1996/03/18  21:47:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  01:59:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  10:42:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  17:10:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:53:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:23:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:12:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:25:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/05  02:19:45  kleung
 * Placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "media_registry.h"
#include "packet.h"
#include "../smrp/smrp_private.h"
#include "../atalk/at_registry.h"
#include "smrp_registry.h"
#include <ciscolib.h>

#ifdef SMRP_FAST_DEBUG
#include "ttysrv.h"
#endif

/* 
 * Private forward declarations
 */
static void smrpfast_fastsetup(hwidbtype*);
static boolean smrpfast_ResetPortForIDB(treeLink*, void*);
static boolean smrpfast_ResetCacheForIDB(treeLink*, void*);

static boolean smrpfast_AgeEntry(treeLink*, void*);
static void smrpfast_CacheAger(boolean);
static void smrpfast_ValidateFwdCache(SMRPForward*);
static void smrpfast_cache_increment_version(hwidbtype*, idbtype*, boolean);

/* 
 * Local Storage -- this is used by the fast-switching modules only.
 */
static ulong smrpfast_cacheVersion = 0L;

/* 
 *------------------------------------------------------------------
 * smrpfast_Initialize
 *
 * Initialize SMRP fast switching globals, called once at startup.
 *------------------------------------------------------------------
 */
void
smrpfast_Initialize (void)
{
    
    smrpfast_cacheVersion = 10L;
    /*
     * Register some functions
     */
    reg_add_bump_cache_version(smrpfast_cache_increment_version,
			       "smrpfast_cache_increment_version");
    reg_add_fast_setup(smrpfast_fastsetup, "smrpfast_fastsetup");

}

/*
 *------------------------------------------------------------------
 * smrpfast_InitPortMCacheData
 * 
 * Initialized the fast switching portion of the port structure.
 *------------------------------------------------------------------
 */
void
smrpfast_InitPortMCacheData (SMRPPort* port)
{
    port->RouteCache = fsUnknown;
    port->FastOkay = FALSE;
    port->Fastswitch = (smrpfastswitch_type)return_false;
    port->MCIFastswitch = (smrpmcifastswitch_type)return_false;
    port->Fastswitch_name = "Always-Fail";
}


/*
 *------------------------------------------------------------------
 * smrp_ReinitPort
 *
 * Reinitialize a port for fast switching.  This is where we generate
 * any hardware related messages which is why it is not called during startup.
 *------------------------------------------------------------------
 */
void
smrpfast_ReinitPort (SMRPPort* port)
{
    if (port == NULL)
	return;

    smrpfast_SetupPort(port);
    
    if (port->RouteCache == fsOldHardware) 
	errmsg(AT_ERR_OLDMCI,idb_get_namestring(port->NetworkReference->phys_idb));    
}

/*
 *------------------------------------------------------------------
 * smrpfast_SetupPort
 *
 * Enable fast switching if appropriate.  Called whenever conditions
 * indicate a change in status may be needed.  Note: this will
 * invalidate the current cache if configuration change occurred.
 *------------------------------------------------------------------
 */
void
smrpfast_SetupPort (SMRPPort* port)
{
    int fsreason = fsEnabled;
    int old_fastokay;
    SMRPIdbsb *idbsb;
    idbtype* idb;
    hwidbtype* hwidb;
    
    idbsb = port->NetworkReference;
    if (idbsb == NULL)
	return;
    
    idb = idbsb->phys_idb;
    hwidb = hwidb_or_null(idb);
    if (hwidb == NULL)
	return;
    
    old_fastokay = port->FastOkay;
    port->Fastswitch = (smrpfastswitch_type)return_false;
    port->MCIFastswitch = (smrpmcifastswitch_type)return_false;
    port->Fastswitch_name = "Always-Fail";
    port->FastOkay = FALSE;

    switch (port->RouteCache) {
    case fsOldHardware:		/* Or do you like trashed data and yo-yo gws */
    case fsUserDisabled:	/* Definitely no need to check further */
	/* If configuration changed, invalidate all cache entries */
	if (old_fastokay)
	    smrpfast_InvalidateAllCache("smrpfast_SetupPort", port);
	return;
    default:			/* Do more checking... */
	break;
    }

    /* Check for invariant conditions */
    if (!smrp_running)
	fsreason = fsUnknown;
    else if (!smrp_IsPortActive(port))
	fsreason = fsPortDown;
    else if (is_ddr(hwidb)) 
	fsreason = fsDialOnDemand;
    else if ((hwidb->comp_db && (hwidb->enctype != ET_FRAME_RELAY))
	     || hwidb->ppp_compression)
        fsreason = fsNotSupported;

    /*
     * If we should enable fast switching, invoke the driver registration
     * service point for the network type.  It will set a reason in port->RouteCache 
     * to tell us whether it succeeded or failed.  If we should not enable
     * fast switching, remember the reason why.
     */
    if (fsreason == fsEnabled) {
	reg_invoke_smrp_fs_register_ports(port->NetworkType, idbsb->phys_idb);
    } else {
	port->RouteCache = fsreason;
    }

    port->FastOkay = (port->RouteCache == fsEnabled);

    /* If configuration changed, invalidate all cache entries */
    if (old_fastokay ^ port->FastOkay)
	smrpfast_InvalidateAllCache("smrpfast_SetupPort", port);
}

/*
 *------------------------------------------------------------------
 * smrpfast_fastsetup
 *
 * Reinitialize the interface for fast switching
 * Called by the SERVICE_FAST_SETUP function.
 *------------------------------------------------------------------
 */

static void 
smrpfast_fastsetup (hwidbtype *hwidb)
{
#ifdef SMRP_FAST_DEBUG 
    /* mcache debugging */
    if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
	buginf("\nDEBUG: smrpfast_fastsetup");
#endif

    smrp_ForAllPortsDo(smrpfast_ResetPortForIDB, (void*)hwidb);
}

/*
 *------------------------------------------------------------------
 * smrpfast_ResetPortForIDB
 * 
 * Reset SMRP fast switching control setup for port on the interface.
 *------------------------------------------------------------------
 */
static boolean
smrpfast_ResetPortForIDB(treeLink* link, void* pdata)
{
    SMRPPort* port = (SMRPPort*) link;
    hwidbtype* hwidb;
    
    hwidb = hwidb_or_null(port->NetworkReference->phys_idb);
    
    if (hwidb != NULL && hwidb == (hwidbtype*) pdata)
	smrpfast_SetupPort(port);

    return (TRUE);
}

/*
 *------------------------------------------------------------------
 * smrpfast_CreateCache	
 *
 * Create memory for a cache entry.
 *------------------------------------------------------------------
 */
SMRPCache*
smrpfast_CreateCache(void)
{
    SMRPCache* cptr;
    
    /* Create cache */
    if ((cptr = malloc(sizeof(SMRPCache))) == NULL)
	return (NULL);
    else
	memset(cptr, 0, sizeof(SMRPCache));
    
    /* mcache debugging */
    if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
        buginf("\nSMRP: Cache memory allocated");
    
    return (cptr);
}

/*
 *------------------------------------------------------------------
 * smrpfast_AddEntry
 *
 * Adds the forward entry to the SMRP fast switching cache if not exist 
 * already.  Validate the cache.
 *------------------------------------------------------------------
 */
void
smrpfast_AddEntry (SMRPForward* fwd)
{
    if (fwd == NULL) return;
	
    if (!fwd->FastEnabled) {
	
	/* mcache debugging */
	if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
	    buginf("\nSMRP: Cache created for group  %s", 
		   smrp_DisplayGroupAddr(fwd->GroupAddress));

	fwd->FastEnabled = TRUE;
    }
    fwd->CacheUpdated = FALSE;            /* Reset */
    smrpfast_ValidateFwdCache(fwd);
}

/*
 *------------------------------------------------------------------
 * smrpfast_DeleteEntry
 * 
 * Delete cache entry by marking fwd->FastEnabled off.
 * Invalidate the entry as well.  Free up memory.
 *------------------------------------------------------------------
 */
void
smrpfast_DeleteEntry (SMRPForward *entry)
{
    SMRPChildPortLink *cur, *next;
    
    if (entry == NULL || !entry->FastEnabled)
	return;
    
    smrpfast_InvalidateFwdCache((void*)entry, NULL);

    /* Release and free up SMRP cache entry */
    next = cur = entry->ChildPorts;
    while (next) {
	next = cur->Next;
	if (cur->FastCache) {
	    free(cur->FastCache);
	    cur->FastCache = NULL;
	}
	cur = next;
    }

    /* mcache debugging */
    if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
	buginf("\nSMRP: Cache deleted for group  %s", 
            smrp_DisplayGroupAddr(entry->GroupAddress));

    entry->FastEnabled = FALSE;
}

/*
 *------------------------------------------------------------------
 * smrpfast_AgeEntry
 * 
 * Age out SMRP cache entries.
 *------------------------------------------------------------------
 */
static boolean
smrpfast_AgeEntry (treeLink* link, void* pdata)
{
    SMRPForward* fwd = (SMRPForward*)link;
    boolean	flushflag = (boolean)pdata;
    SMRPPort* port;
    
    port = smrp_FindPort(fwd->ParentPortID);
    
    if (!fwd->FastEnabled)
	return (TRUE);
    
    if (!fwd->CacheValid || flushflag) {
	if (   flushflag
	    || CLOCK_OUTSIDE_INTERVAL(fwd->CacheUpdateTime, SMRPMCACHE_TIMEOUT)
	    || !smrp_IsPortOperational(port)) {
	    smrpfast_DeleteEntry(fwd);
	}
    } else
	GET_TIMESTAMP(fwd->CacheUpdateTime);

    return (TRUE);
}

/*
 *------------------------------------------------------------------
 * smrpfast_CacheAger
 *
 * SMRP fast switching cache ager.
 *------------------------------------------------------------------
 */
static void
smrpfast_CacheAger (boolean flushflag)
{
    int	status;

    /*
     * If 0, cache has never been used. If it is 'loop time' and we
     * loop back to zero, then the cache must be changing so frequently,
     * it won't be zero for long
     */
    if (smrpfast_cacheVersion == 0L)
        return;
    status = raise_interrupt_level(NETS_DISABLE);
    smrp_ForAllForwardsDo(smrpfast_AgeEntry, (void*)flushflag);
    reset_interrupt_level(status);
}

/*
 *------------------------------------------------------------------
 * smrpfast_fscache_age
 *
 * Age the fast switching cache entries.
 *------------------------------------------------------------------
 */

void 
smrpfast_fscache_age (void)
{
    smrpfast_CacheAger(FALSE);
}

/*
 *------------------------------------------------------------------
 * smrpfast_fscache_flush
 *
 * Flush the fast switching cache entries.
 *------------------------------------------------------------------
 */

void 
smrpfast_fscache_flush (void)
{
    smrpfast_CacheAger(TRUE);
}


/*
 *------------------------------------------------------------------
 * smrpfast_UpdateMCache
 *
 * Do some sanity checks before updating the cache.  Add forwarding
 * entry to the cache by setting fwd->FastEnabled flag.  
 *------------------------------------------------------------------
 */
boolean
smrpfast_UpdateMCache (paktype* pak, SMRPForward** fwd, SMRPChildPortLink** child)
{
    SMRPGroupID gid;
    MCHeader *hdr;
    SMRPPort* port;
    idbtype* idb;
    SMRPIdbsb *idbsb;
    
    SMRPASSERT(pak != NULL);
    
    hdr = (MCHeader *)pak->transport_start;
    /* Handle data packets only */
    if (smrp_GetPacketType(hdr->PacketType) != MCData) 
	return (FALSE);

    gid = GETLONG(&hdr->GroupAddress);

    *fwd = smrp_FindForward(gid);
    if (*fwd == NULL) 
	return (FALSE);

    /* Cache is already up-to-date */
    if ((*fwd)->CacheValid)
	return (FALSE);
    
    /* 
     * Don't bother populating cache unless parent port is f/s enabled 
     */
    port = smrp_FindPort((*fwd)->ParentPortID);
    if (port == NULL || !port->FastOkay)
	return (FALSE);
    
    idb = pak->if_output;
    idbsb = smrp_GetIdbsb(idb);
    if (idbsb == NULL)
	return (FALSE);

    port = smrp_FindPortFromIdb(idbsb, pak->linktype);
    if (port == NULL)
	return (FALSE);
    
    /* Look up child entry for outgoing port */
    *child = smrp_FindForwardChildPort(*fwd, port->PortID);
    if (*child == NULL)
	return (FALSE);

    /* Don't bother populating cache unless child port is f/s enabled */
    if (!port->FastOkay) {
	if ((*child)->FastCache) {
	    /* Fast switching disabled on the port, so delete cache entry. */
	    free((*child)->FastCache);
	    (*child)->FastCache = NULL;
	}
	return (FALSE);
    }

    return (TRUE);
}

/*
 *------------------------------------------------------------------
 * smrpfast_ValidateFwdCache
 *
 * Validates the forward's SMRP fast switching cache entry.
 *------------------------------------------------------------------
 */
static void
smrpfast_ValidateFwdCache (SMRPForward* fwd)
{
    if (fwd == NULL) return;
	
    /* mcache debugging */
    if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
	buginf("\nSMRP: Cache validated for group  %s", 
            smrp_DisplayGroupAddr(fwd->GroupAddress));

    fwd->CacheValid = TRUE;
    GET_TIMESTAMP(fwd->CacheUpdateTime);
}

/*
 *------------------------------------------------------------------
 * smrp_InvalidateFwdCache
 *
 * Invalidates the forward's SMRP fast switching cache entry.
 * The entry is invalidated if it contains the specified port.
 *------------------------------------------------------------------
 */
boolean
smrpfast_InvalidateFwdCache (treeLink* link, void* pdata)
{
    SMRPForward* fwd = (SMRPForward*) link;
    SMRPPort* match = (SMRPPort*) pdata;
    
    if (!fwd->CacheValid)
	return (TRUE);
    
    if (match != NULL && 
	match->PortID != fwd->ParentPortID && 
	!smrp_FindForwardChildPort(fwd, match->PortID)) {
	    return (TRUE);
	}

    /* mcache debugging */
    if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
	buginf("\nSMRP: Cache invalidated for group  %s", 
	       smrp_DisplayGroupAddr(fwd->GroupAddress));

    fwd->CacheValid = FALSE;

    return (TRUE);
}

/*
 *------------------------------------------------------------------
 * smrpfast_InvalidateAllCache
 *
 * Invalidates the entire SMRP fast switching cache.
 *------------------------------------------------------------------
 */
void
smrpfast_InvalidateAllCache (char *rname, SMRPPort* port)
{
#ifdef SMRP_FAST_DEBUG 
    /* mcache debugging */
    if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
	buginf("\nDEBUG: Fast switching cache invalidated by %s", rname);
#endif

    smrp_ForAllForwardsDo(smrpfast_InvalidateFwdCache, port);
}

/*
 *------------------------------------------------------------------
 * smrpfast_cache_increment_version
 * 
 * Invalidate cache only if f/s enabled for any network 
 * protocol on this SMRP interface, or if no interface supplied.
 *------------------------------------------------------------------
 */
static void 
smrpfast_cache_increment_version (hwidbtype *hwidb, idbtype *swidb,
				  boolean immediately)
{
    
    if (hwidb == NULL)	
	smrpfast_InvalidateAllCache("registry", NULL);
    else
	smrp_ForAllPortsDo(smrpfast_ResetCacheForIDB, (void*)hwidb);
}

/*
 *------------------------------------------------------------------
 * smrpfast_ResetCacheForIDB
 * 
 * Invalidate the cache if the port is on the interface.
 *------------------------------------------------------------------
 */
static boolean
smrpfast_ResetCacheForIDB(treeLink* link, void* pdata)
{
    SMRPPort* port = (SMRPPort*) link;
    hwidbtype* hwidb;
    
    hwidb = hwidb_or_null(port->NetworkReference->phys_idb);
    
    if (hwidb != NULL && hwidb == (hwidbtype*) pdata)
	smrpfast_InvalidateAllCache("registry", port);

    return (TRUE);
}


/*
 *------------------------------------------------------------------
 * smrpfast_RBTreeSearch_inline - return a ptr if a valid forward exists.
 *
 * This is a special version of RBTreeSearch(), completely
 * inlined to remove as many function calls as possible.
 *------------------------------------------------------------------
 */
inline treeLink*
smrpfast_RBTreeSearch_inline (rbTree* T, treeKey key)
{
    register treeLink* x = T->root;
    treeLink	 *nilnode = RBTreeGetNIL();
    ulong	 cnt = 0;

    if (key.l == 0)
	return (NULL);

    T->findRequests++;
    while (x != nilnode) {
	++cnt;
	if (x->key.l == key.l) {
	    /*
	     * Keys match, so find first so GetNext will iterate
	     */
	    if (x->left->key.l == key.l) {
		while (x->right != nilnode)
		    x = x->right;
	    } else if (x->right->key.l == key.l) {
		while (x->left != nilnode)
		    x = x->left;
	    }
	    break;			/* leave while loop, we have a node */
	}
	if (key.l < x->key.l)
	    x = x->left;
	else
	    x = x->right;
    }
    T->findAccessed += cnt;
    if (cnt > T->findDeepest) 
	T->findDeepest = cnt;
    if (cnt > T->maxNodeDepth)
	T->maxNodeDepth = cnt;

    if (x != nilnode) {
	return (x);
    }
    return (NULL);
}

/*
 *------------------------------------------------------------------
 * smrpfast_Reason
 *
 * Return a string describing the status of fast switching on the 
 * port.
 *------------------------------------------------------------------
 */
char*
smrpfast_Reason (SMRPPort* port)
{
    char *buf;

    switch (port->RouteCache) {
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
		port->RouteCache);
	return (buf);
    }
}

/*
 *------------------------------------------------------------------
 * smrpfast_DumpCacheMacHdr
 * 
 * Print out the MAC header in the cache.
 *------------------------------------------------------------------
 */
uchar*
smrpfast_DumpCacheMacHdr (SMRPCache* entry, uchar* data)
{
    if (!entry)
	return (NULL);
    
    return (smrp_DumpBuffer(entry->mac_ptr, entry->mac_len, data));
}

/*
 *------------------------------------------------------------------
 * smrpfast_DumpCacheNetHdr
 * 
 * Print out the network header in the cache.
 *------------------------------------------------------------------
 */
uchar*
smrpfast_DumpCacheNetHdr (SMRPCache* entry, uchar* data)
{

    if (!entry)
	return NULL;
    
    return(smrp_DumpBuffer((uchar *)&entry->net, entry->net_len, 
			   data));
}

