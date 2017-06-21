/* $Id: at_maint.c,v 3.9.4.8 1996/08/28 12:36:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_maint.c,v $
 *------------------------------------------------------------------
 * at_maint.c -- Appletalk Maintenance and Neighbor Support
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_maint.c,v $
 * Revision 3.9.4.8  1996/08/28  12:36:40  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.4.7  1996/08/28  00:51:17  hampton
 * Migrate Desktop Protocols away from the one minute and one second
 * registries.  [CSCdi67383]
 * Branch: California_branch
 *
 * Revision 3.9.4.6  1996/07/10  22:18:27  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.9.4.5  1996/06/28  23:04:50  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.9.4.4  1996/05/10  01:36:08  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.9.4.3  1996/04/27  05:44:54  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.5.8.3  1996/04/25  23:56:22  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.5.8.2  1996/04/17  00:30:27  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.5.8.1  1996/02/23  20:11:30  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.9.4.2  1996/03/23  01:26:56  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.9.4.1  1996/03/18  18:52:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.4  1996/03/16  06:26:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.12.3  1996/03/13  01:06:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.12.2  1996/03/07  08:29:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.12.1  1996/02/20  00:17:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/03/11  22:54:06  kleung
 * CSCdi50981:  Cannot run multiple Cayman tunnels.
 *
 * Revision 3.8  1996/03/07  22:04:25  kleung
 * CSCdi50981:  Cannot run multiple Cayman tunnels.
 *
 * Revision 3.7  1996/02/08  23:08:04  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.6  1996/01/24  00:14:53  slin
 * CSCdi42908:  Appletalk Zip Query Broken Over Serial Links
 * - Reset ZIP Query Timer when neighbor restarts
 *
 * Revision 3.5  1995/12/15  00:55:09  kleung
 * CSCdi45426:  Appletalk non-operational over LAPB encapsulation
 *
 * Revision 3.4  1995/12/08  04:55:18  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.3  1995/11/17  08:43:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:40  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:57:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  20:50:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/20  19:04:18  slin
 * CSCdi42522:  Remove use of if_number from AppleTalk
 *
 * Revision 2.6  1995/07/24  22:30:30  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.5  1995/07/13 06:49:53  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.4  1995/06/18 06:17:41  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.3  1995/06/10 12:25:05  dwong
 * CSCdi35099:  clear apple neighbor is not working
 *
 * Revision 2.2  1995/06/09  12:58:36  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:08:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "plural.h"
#include "../os/signal.h"
#include "../os/chunk.h"
#include "atalk_private.h"
#include "at_nbp.h"
#include "at_maint.h"
#include "at_static.h"
#include "address.h"
#include "at_domain.h"
#include "at_registry.h"
#include "../if/atm.h"
#include "../lane/lane.h"


enum {
    ATTIMER_MAINT_ROUTE,
    ATTIMER_MAINT_NEIGHBOR_AGER,
    ATTIMER_MAINT_IFSTATE,
    ATTIMER_MAINT_ZONE,
    ATTIMER_MAINT_FSCACHEAGER,
    ATTIMER_MAINT_NAMEAGER,
    ATTIMER_MAINT_GC,
    ATTIMER_MAINT_NBPREQUEST,
    ATTIMER_MAINT_NAMELOOKUP,
    ATTIMER_MAINT_RUNGC
};

typedef struct maintlookuplist_ {
    struct maintlookuplist_ *next;
    str32 typeName;
} maintlookuplist;

typedef struct maintnameentry_ {
    struct treeLink_ link;
    nbpEntity nve;
    ataddress dst;
    treeKey key;
    ulong nameSum;
    sys_timestamp updateTime;
    sys_timestamp timer;
} maintnameentry, *maintNVE;

/* local storage */

static watched_queue *atalkmiscQ;

static mgd_timer atalk_maint_timer;

static sys_timestamp maint_ifStartClock;

listItem* maint_pendingZoneRequests; /* Routes awaiting zones */

static watched_queue *maint_nameQueue;
static nbpEntity maint_ciscoNVE;
static nbptimer maint_nameTimer;

static rbTree* maint_nameTable;

static maintlookuplist* maint_lookupList;

static mgd_timer maint_routeSendTimer;
static mgd_timer maint_neighborAgerTime;
static mgd_timer maint_fscacheAgerTime;
static mgd_timer maint_nameAgerTime;
static mgd_timer maint_zoneRequestTime;
static mgd_timer maint_nbpRequestTime;
static mgd_timer maint_nameLookupTime;
static mgd_timer maint_ifStateTime;
static mgd_timer maint_gcSleepTime;

static ulong maint_nameEvents;
static ulong maint_nameRequests;
static ulong maint_nameErrors;

static ATAddress maint_eolKey;

static rbTree* maint_neighbors;

static boolean maint_started;

/* forward declarations */
neighborentry* maint_GetNextNeighbor(neighborentry*);
static maintNVE maint_GetNextNVE(maintNVE);
static void maint_DeleteNVE (maintNVE n);

void
atmaint_stop_namelookup_timer (void)
{
    mgd_timer_stop(&maint_nameLookupTime);
}

void
atmaint_start_namelookup_timer (void)
{
    mgd_timer_start(&maint_nameLookupTime, 0);
}

void
atmaint_update_nbprequest_timer (void)
{
    if (!mgd_timer_running(&maint_nbpRequestTime))
	mgd_timer_start(&maint_nbpRequestTime, ONESEC);
}

/*
 * maint_GetFirstNVE
 * Get the first name entry from the tree.
 */
static inline maintNVE
maint_GetFirstNVE (void)
{ 
    return maint_GetNextNVE(NULL); 
}

/*
 * maint_GetNextRequest
 * Get the next route off the request list.
 */
static atalk_rdb_t*
maint_GetNextRequest (atalk_rdb_t *r)
{
    atalk_rdb_t* nxt = NULL;
    listItem* i = NULL;
    if (r == NULL)
	i = maint_pendingZoneRequests;
    else
	i = GetNextItem(&r->maintLink);
    if (i != NULL)
	nxt = (atalk_rdb_t *) ListItemData(i);
    return nxt;
}

/*
 * maint_GetFirstRequest
 * Get the first request off of the list
 */
static inline atalk_rdb_t*
maint_GetFirstRequest (void)
{
    return maint_GetNextRequest(NULL);
}


atalk_rdb_t *
atmaint_GetFirstRequest (void)
{
    return maint_GetNextRequest(NULL);
}

atalk_rdb_t *
atmaint_GetNextRequest (atalk_rdb_t *r)
{
    return maint_GetNextRequest(r);
}

/*
 * atmaint_Initialize
 * Initialize this modules local storage.
 */
void
atmaint_Initialize (void)
{
    mgd_timer_init_parent(&atalk_maint_timer, NULL);
    mgd_timer_init_leaf(&maint_routeSendTimer, &atalk_maint_timer,
			ATTIMER_MAINT_ROUTE, NULL, FALSE);
    mgd_timer_init_leaf(&maint_neighborAgerTime, &atalk_maint_timer,
			ATTIMER_MAINT_NEIGHBOR_AGER, NULL, FALSE);
    mgd_timer_init_leaf(&maint_ifStateTime, &atalk_maint_timer,
			ATTIMER_MAINT_IFSTATE, NULL, FALSE);
    mgd_timer_init_leaf(&maint_zoneRequestTime, &atalk_maint_timer,
			ATTIMER_MAINT_ZONE, NULL, FALSE);
    mgd_timer_init_leaf(&maint_fscacheAgerTime, &atalk_maint_timer,
			ATTIMER_MAINT_FSCACHEAGER, NULL, FALSE);
    mgd_timer_init_leaf(&maint_nameAgerTime, &atalk_maint_timer,
			ATTIMER_MAINT_NAMEAGER, NULL, FALSE);
    mgd_timer_init_leaf(&maint_gcSleepTime, &atalk_maint_timer,
			ATTIMER_MAINT_GC, NULL, FALSE);
    mgd_timer_init_leaf(&maint_nbpRequestTime, &atalk_maint_timer,
			ATTIMER_MAINT_NBPREQUEST, NULL, FALSE);
    mgd_timer_init_leaf(&maint_nameLookupTime, &atalk_maint_timer,
			ATTIMER_MAINT_NAMELOOKUP, NULL, FALSE);
        
    maint_neighbors = RBTreeCreate("Appletalk", "AT", "NeighborTable",
				   at_print_key_long_idb_address,
				   &appletalk_debug->flag);
    RBTreeNon32Bit(maint_neighbors, RBTreeKeyMaxLongLong, RBTreeKeyCompareLongLong);

    maint_pendingZoneRequests = NULL;
    maint_nameQueue = create_watched_queue("AT Name Lookup Queue", 0, 0);
    maint_lookupList = NULL;
    maint_nameTimer.maxrequests = 1;
    maint_nameTimer.interval = ((atalk_nameLookupInterval/ONESEC)/2)+1;
    maint_nameTimer.maxreplies = 100;
    maint_nameEvents = 0;
    maint_nameErrors = 0;
    maint_nameRequests = 0;
    maint_eolKey.l = 0;
    maint_nameTable = RBTreeCreate("Appletalk", "AT", "LookupTable",
				   at_print_key_address,
				   &appletalk_debug->flag);
    TIMER_STOP(maint_ifStartClock);
    maint_started = FALSE;
}

/*
 * atmaint_StatusReport
 * Generate a status report for maint.
 */
void
atmaint_StatusReport (void)
{
    int i;
    atalk_rdb_t* p;
    maintNVE nve;
    boolean hdr = FALSE;

    printf("\n  Name lookup statistics: %d request%s, %d event%s, %d error%s",
	   ArgAndPlural(maint_nameRequests,"","s"),
	   ArgAndPlural(maint_nameEvents,"","s"),
	   ArgAndPlural(maint_nameErrors,"","s"));
    RBTreeStats(maint_nameTable,"  Name Cache");
    if (at_debug(appletalk_debug, ATALK_NULL_MASK)) {
	nve = maint_GetFirstNVE();
	while (nve != NULL) {
	    printf("\n      %s, %lA, age %d, sum %d",
		   atnbp_entityStr(&nve->nve), nve->key.l,
		   ELAPSED_TIME(nve->updateTime)/ONESEC,nve->nameSum);
	    nve = maint_GetNextNVE(nve);
	}
    }
    RBTreeStats(maint_neighbors,"  Neighbors");
    /* XXX Pending GC Requests go here */
    if (!hdr)
	printf("\n  No pending GC requests.");
    i = 0;
    p = maint_GetFirstRequest();
    if (p == NULL)
	printf("\n  No pending zone requests.");
    else {
	printf("\n  Pending Zone Requests:");
	while (p != NULL) {
	    printf("\n    %#A, %s, %d retries, time %#08Ta",
		   atalk_CableRange(p),
		   p->zoneupdate?"update pending":"waiting",
		   p->zoneretries, p->zonetimer);
	    p = maint_GetNextRequest(p);
	}
    }
}

/*
 * atamint_ShowLookupCache
 */
void
atmaint_ShowLookupCache (void)
{
    maintNVE nve;

    if (atalk_nameLookupInterval == 0)	/* don't display if not enabled */
	return;

    nve = maint_GetFirstNVE();
    if (nve == NULL)
	return;

    printf("AppleTalk Name Cache:\n%-5s%-4s%-4s %24s%16s%24s",
		"Net", "Adr", "Skt", "Name", "Type", "Zone");

    while (nve != NULL) {
	printf("\n%-5d%-4d%-4d %24s%16s%24s",
	       (nve->key.l >> 16) & 0xFFFF,
	       (nve->key.l >> 8) & 0xFF,
	       nve->key.l & 0xFF,
	       at_hex_display(str32ptr(&nve->nve.srvr_name)),
	       at_hex_display(str32ptr(&nve->nve.type_srvc)),
	       at_hex_display(str32ptr(&nve->nve.srvr_zone)));

	nve = maint_GetNextNVE(nve);
    }
}

/*
 * atmaint_ClearNameCache -
 *	clear the atalk name-cache
 *	(called from appletalk_command() in at_parse.c)
 */
void
atmaint_ClearNameCache(void)
{
    register maintNVE nve;

    /* clear any pending requests: */
    atnbp_cancel(maint_nameQueue);

    /* nuke the tree: */
    while((nve = maint_GetFirstNVE()) != NULL)
	maint_DeleteNVE(nve);
}

/*
 * atmaint_FindLookupType
 * Find a lookup type
 */
static maintlookuplist*
maint_FindLookupType (int tlen,uchar* tname)
{
    str32 tstr;
    maintlookuplist* tl = maint_lookupList;
    
    str32ninit(&tstr, (char*)tname, tlen);
    while (tl != NULL) {
	if (str32match(&tstr,&tl->typeName))
	    break;
	tl = tl->next;
    }
    return tl;
}

/*
 * atmaint_AddLookupType
 * Add a lookup type to our lookup list;
 */
void
atmaint_AddLookupType (int tlen,uchar* tname)
{
    maintlookuplist* tl = maint_FindLookupType(tlen,tname);
    if (tl != NULL)
	return;
    tl = malloc(sizeof(struct maintlookuplist_));
    if (tl == NULL)
	return;
    str32ninit(&tl->typeName, (char*)tname, tlen);
    tl->next = maint_lookupList;
    maint_lookupList = tl;
}

/*
 * atmaint_DeleteLookupType
 * Delete a lookup type to our lookup list;
 */
void
atmaint_DeleteLookupType (int tlen,uchar* tname)
{
    str32 tstr;
    maintlookuplist* tl = maint_lookupList;
    maintlookuplist* ptl = NULL;
    
    str32ninit(&tstr, (char*)tname, tlen);
    while (tl != NULL) {
	if (str32match(&tstr,&tl->typeName))
	    break;
	ptl = tl;
	tl = tl->next;
    }
    if (tl == NULL)
	return;
    if (ptl == NULL)
	maint_lookupList = tl->next;
    else
	ptl->next = tl->next;
    free(tl);
}

/*
 * atmaint_ShowLookupList
 * Show the items on the LookupList.
 */
void
atmaint_ShowLookupList (void)
{
    maintlookuplist* tl = maint_lookupList;
    int len;
    
    if (tl != NULL)
	printf("\n  Lookups will be generated for server types:");
    len = printf("\n        ");
    while (tl != NULL) {
	len += printf("%s",at_hex_display(STR32PTR(&tl->typeName)));
	tl = tl->next;
	if (tl != NULL) {
	    len += printf(", ");
	    if ((len + strlen(at_hex_display(STR32PTR(&tl->typeName)))) > 78)
		len = printf("\n        ");
	}
    }
}

/*
 * atmaint_WriteNV
 * Handle NVRAM for maint.
 */
void
atmaint_WriteNV (char *nv_command)
{
    maintlookuplist* tl = maint_lookupList;
    
    while (tl != NULL) {
	nv_write(TRUE, "%s ", nv_command);
	atalk_nv_printzone(at_hex_display(STR32PTR(&tl->typeName)));
	tl = tl->next;
    }
}

/*
 * maint_NeighborState
 * Return a string which represents the neighbors state.
 */
char*
atmaint_NeighborState (neighborStatus s)
{
    switch (s) {
    case neighborUp:
	return "Up";
    case neighborOverdue:
	return "Overdue";
    case neighborDown:
	return "Down";
    default:
	return "Unknown";
    }
}

/*
 * maint_NVEKey
 * Create a treeKey for an NVE which is always unique.
 */
static treeKey
maint_NVEKey (ataddress src)
{
    treeKey key;
    key.l = (src.net << 16) | (src.node << 8) | src.spec;
    return (key);
}

/*
 * maint_nameSum -
 *	generate a sum using nbpEntity name fields.
 *	don't include uninitialized bytes.
 */
static ulong
maint_nameSum(nbpEntity *nve)
{
    register int ii;
    register char *pp;
    register ulong nsum = 0;

    for(pp=STR32PTR(&nve->srvr_name), ii=STR32LEN(&nve->srvr_name);ii>0;ii--)
	nsum += (ulong) *pp;
    for(pp=STR32PTR(&nve->srvr_name), ii=STR32LEN(&nve->type_srvc);ii>0;ii--)
	nsum += (ulong) *pp;
    for(pp=STR32PTR(&nve->srvr_name), ii=STR32LEN(&nve->srvr_zone);ii>0;ii--)
	nsum += (ulong) *pp;

    return(nsum);
}

/*
 * maint_NVECast
 * Do data type conversion between links and NVEs.
 */
static inline maintNVE
maint_NVECast (treeLink* link) { return(maintNVE)link; }

/*
 * maint_UnprotectNVE
 * Release a name.
 */
static inline void
maint_UnprotectNVE (maintNVE n) 
{ 
    RBTreeNodeProtect(&n->link,FALSE); 
}

/*
 * maint_ProtectNVE
 * Allow an name to be protected.
 */
static inline void
maint_ProtectNVE (maintNVE n) 
{ 
    RBTreeNodeProtect(&n->link,TRUE); 
}


/*
 * maint_InsertNVE
 * Add an NVE to our table.
 */
static void
maint_InsertNVE (maintNVE n)
{
    if (maint_nameTable != NULL)
	RBTreeInsert(n->key,maint_nameTable,&n->link);
}

/*
 * maint_DeleteNVE
 * Delete a name from our table.
 */
static void
maint_DeleteNVE (maintNVE n)
{
    if ((maint_nameTable != NULL)&&(n!=NULL))
	RBTreeDelete(maint_nameTable,&n->link);
}

/*
 * GetNextNVE - Search for the next NVE and return it w/o protection
 */
static maintNVE
maint_GetNextNVE (maintNVE nve)
{
    maintNVE n = nve;

    if (maint_nameTable == NULL)
	return NULL;
    if (n == NULL) {
	n = maint_NVECast(RBTreeFirstNode(maint_nameTable));
    } else {
	n = maint_NVECast(RBTreeNextNode(&n->link));
    }
    return (n);
}

/*
 * maint_ReinitNVE
 * Release any resources in use for this NVE
 */
static void
maint_ReinitNVE (maintNVE n)
{
				/* Currently no special resources */
}

/*
 * maint_CreateNVE
 * initialize a new NBP entry
 */
static maintNVE
maint_CreateNVE (nbpEntity* nbpe, ataddress dst)
{
    maintNVE nve;
    
    if (maint_nameTable == NULL)
	return (NULL);

    nve = (maintNVE) RBTreeGetFreeNode(maint_nameTable);
    if (nve != NULL)
	maint_ReinitNVE(nve);
    else
	nve = malloc(sizeof(struct maintnameentry_));
    if (nve == NULL)
	return (NULL);
    bcopy(nbpe,&nve->nve,sizeof(nbpEntity));
    nve->dst = dst;
    GET_TIMESTAMP(nve->updateTime);
    nve->timer = TIMER_ADD_DELTA(nve->updateTime, 2*atalk_nameLookupInterval);
    nve->nameSum = maint_nameSum(&nve->nve);
    nve->key = maint_NVEKey(dst);
    if (at_debug(appletalk_debug, ATALK_NULL_MASK))
	buginf("\nAT: Added name %s to cache for %lA",
	       atnbp_entityStr(&nve->nve),
	       nve->key.l);
    maint_InsertNVE(nve);
    return (nve);
}

static boolean
maint_AddNVE (ataddress src, nbpEntity* nve)
{
    maintNVE	n;
    ulong	nsum;
    treeKey	key = maint_NVEKey(src);

    if (maint_nameTable == NULL)
	return (FALSE);
    nsum = maint_nameSum(nve);
    n = maint_NVECast(RBTreeSearch(maint_nameTable, key));
    while (n != NULL) {
	if (n->nameSum == nsum)
	    break;
	n = maint_GetNextNVE(n);
	if (n != NULL)
	    if (n->key.l != key.l)
		n = NULL;
    }
    if (n == NULL) 
	return (maint_CreateNVE(nve,src) != NULL);
    GET_TIMESTAMP(n->updateTime);
    n->timer = TIMER_ADD_DELTA(n->updateTime, 2 * atalk_nameLookupInterval);
    return (TRUE);
}

static maintNVE
maint_GetNVE (ATAddress addr)
{
    treeKey key;
    key.l = addr.l;
    if (maint_nameTable == NULL)
	return (NULL);
    
    return (maint_NVECast(RBTreeSearch(maint_nameTable,key)));
}

static maintNVE
maint_FindNVE (ataddrtype adr)
{
    treeKey key;
    maintNVE nve;
    ulong save;

    save = (atalk_netonly(adr) << 16) | (atalk_nodeonly(adr) << 8);
    key.l = save | 0xff;		/* find the maximal match */
    if (maint_nameTable == NULL)
	return (NULL);
    nve = maint_NVECast(RBTreeBestNode(maint_nameTable,key));
    if (nve != NULL)
	if ((nve->key.l & 0xFFFFFF00) == save)
	    return nve;
    return NULL;
}

ATAddress
atmaint_FirstRegisteredKey (neighborentry* n,ulong* idx)
{
    maintNVE nve;
    ATAddress a;
    nve = maint_FindNVE(n->address);
    *idx = 0;
    if (nve == NULL)
	return maint_eolKey;
    *idx = nve->nameSum;
    a.l = nve->key.l;
    return (a);
}

ATAddress
atmaint_NextRegisteredKey (ATAddress addr,ulong *idx)
{
    maintNVE nve;
    ATAddress a;
    ulong tmp;
    nve = maint_GetNVE(addr);
    while (nve != NULL) {
	tmp = (addr.f.net << 16) | (addr.f.node << 8);
	if (tmp != (nve->key.l & 0xFFFFFF00))
	    return maint_eolKey;
	if (addr.f.spec == (nve->key.l & 0xFF)) {
	    if (*idx != nve->nameSum) {
		nve = maint_GetNextNVE(nve);
		continue;
	    }
	    nve = maint_GetNextNVE(nve);
	    addr.f.spec = 0;
	    continue;
	}
	*idx = nve->nameSum;
	a.l = nve->key.l;	
	return (a);
    }
    return (maint_eolKey);
}

static void
maint_NVENodeName (maintNVE nve, char* buff)
{
    int i;
    char* cp = buff;
    char* np = str32ptr(&nve->nve.srvr_name);
    buff[0] = 0;
    for (i=0;i < str32len(&nve->nve.srvr_name);i++) {
	if (ISDISPLAY_ASCII(np[i]))
	    *cp++ = np[i];
    }
    *cp = 0;
}

char*
atmaint_GetNodeName (ataddrtype addr)
{
    maintNVE nve;
    char *buff = string_getnext();
    nve = maint_FindNVE(addr);
    if (nve == NULL)
	return (NULL);
    maint_NVENodeName(nve,buff);
    return (buff);
}

char*
atmaint_GetRegisteredName (ATAddress addr, ulong idx)
{
    maintNVE nve;
    nve = maint_GetNVE(addr);
    while (nve != NULL) {
	if (nve->nameSum != idx) {
	    nve = maint_GetNextNVE(nve);
	    if (nve->key.l != addr.l)
		return (" ");
	    continue;
	}
	break;
    }
    if (nve == NULL)
	return (" ");
    return (atnbp_entityStr(&nve->nve));
}

/*
 * maint_NeighborKey
 * Create a treeKey for a neighbor which is always unique.
 * Note that tunnels require idb/addr identifier
 * since tunnels are unnumbered configurations.
 */
static inline treeKey
maint_NeighborKey (atalkidbtype* idb, ataddrtype src)
{
    treeKey key;
    key.ll = idb->phys_idb->if_number;
    key.ll <<= 24;
    key.ll |= (src & ATALK_ADDR_MASK);
    return (key);
}

/*
 * maint_NeighborCast
 * Do data type conversion between links and neighbors.
 */
static inline neighborentry*
maint_NeighborCast (treeLink* link) { return(neighborentry*)link; }

/*
 * maint_UnprotectNeighbor
 * Allow a neighbor to be freed or reused.
 */
static inline void
maint_UnprotectNeighbor (neighborentry* n) 
{ 
    RBTreeNodeProtect(&n->link,FALSE); 
}

/*
 * maint_ProtectNeighbor
 * Protect a neighbor from being freed.
 */
static inline void
maint_ProtectNeighbor (neighborentry* n) 
{
    if (!RBTreeNodeProtected(&n->link)) 
        RBTreeNodeProtect(&n->link,TRUE); 
}

/*
 * maint_NeighborProtected
 * Return protection status of neighbor treeLink entry.
 */
static inline boolean
maint_NeighborProtected (neighborentry* n)
{
    return RBTreeNodeProtected(&n->link);
}

/*
 * maint_GetFirstNeighbor
 * Get the first neighbor from the tree.
 */
neighborentry*
maint_GetFirstNeighbor (void)
{ 
    return maint_GetNextNeighbor(NULL); 
}

/*
 * maint_InsertNeighbor
 * Add a neighbor to our table.
 */
static void
maint_InsertNeighbor (neighborentry* n) 
{
    treeKey key = maint_NeighborKey(n->port,n->address);
    if (maint_neighbors != NULL)
	RBTreeInsert(key,maint_neighbors,&n->link);
}

/*
 * atmaint_DeleteNeighbor
 * Delete a neighbor from our table.
 */
void
atmaint_DeleteNeighbor (neighborentry* n)
{
    if (!n)
	return;
    if (maint_NeighborProtected(n)) { /* if it's still locked, bail */
	if (at_debug(atalk_events, ATALK_NULL_MASK))
	    buginf("\nAT: atmaint_DeleteNeighbor: AT neighbor %a is busy",
		   n->address);
	return;
    }

    if (maint_neighbors != NULL)  {
	atdomain_DeleteNeighbor(n);
	RBTreeDelete(maint_neighbors,&n->link);
    }
}

/*
 * GetNextNeighbor - Search for the next neighbor and return it w/o protection
 */
neighborentry*
maint_GetNextNeighbor (neighborentry* neighbor)
{
    neighborentry* n = neighbor;

    if (maint_neighbors == NULL)
	return NULL;
    if (n == NULL) {
	n = maint_NeighborCast(RBTreeFirstNode(maint_neighbors));
    } else {
	n = maint_NeighborCast(RBTreeNextNode(&n->link));
    }
    return (n);
}

/*
 * maint_ReinitNeighbor
 * Release any resources in use for this neighbor
 */
static void
maint_ReinitNeighbor (neighborentry* n)
{
				/* Currently no special resources */
}

/*
 * atmaint_CreateNeighbor
 * initialize a new neighbor
 */
neighborentry*
atmaint_CreateNeighbor (ataddrtype address, atalkidbtype* idb)
{
    neighborentry* n;
    sys_timestamp now;
    
    if (maint_neighbors == NULL)
	return NULL;

    GET_TIMESTAMP(now);
    
    if (!atalkif_CaymanTunnel(idb) && !atalkif_AURPTunnel(idb))
	ATASSERT(address != 0);
    n = (neighborentry*) RBTreeGetFreeNode(maint_neighbors);
    if (n != NULL)
	maint_ReinitNeighbor(n);
    else
	n = malloc(sizeof(struct neighborentry_));
    if (n == NULL)
	return NULL;
    memset(n, 0, sizeof(neighborentry)); /* clear node */
    n->address = address;
    n->port = idb;
    COPY_TIMESTAMP(now, n->started);
    COPY_TIMESTAMP(now, n->restarted);
    COPY_TIMESTAMP(now, n->lastheard);
    n->state = neighborUp;		/* protocols set their own state */
    n->configerror = FALSE;
    n->oldcisco = FALSE;
    n->obsoleterouter = FALSE;		/* Mark it on next pass with msgs */
    if (at_debug(atalk_events, ATALK_INTERFACE_MASK, n->port->phys_idb))
	errmsg(AT_ERR_NEWNEIGHBOR, idb_get_namestring(n->port->phys_idb),
	       n->address);
    maint_InsertNeighbor(n);

    atdomain_InsertNeighbor(n);

    return (n);
}

/*
 * atmaint_FreeAllNeighbors
 *
 * This function is called only after AppleTalk routing has been disabled
 * to free up the memory used by the neighbor table.
 */
void
atmaint_FreeAllNeighbors (void)
{
    neighborentry	*n;

    atalk_clear_neighbor(0, TRUE, FALSE);
    if (!RBTreeValid(maint_neighbors, FALSE, "atmaint_FreeAllNeighbors"))
	return;
    while ((n = maint_NeighborCast(RBTreeGetFreeNode(maint_neighbors))) != NULL) {
	free(n);			/* free the route */

    }
}

/*
 * atmaint_FindNeighbor
 *
 * Find a neighbor for the given idb and address. No NULL parameters
 * allowed.
 */
neighborentry*
atmaint_FindNeighbor (ataddrtype address, atalkidbtype* idb)
{
    neighborentry* n;
    treeKey key;

    if (!idb)
	return (NULL);
    key = maint_NeighborKey(idb,address);
    if (maint_neighbors == NULL)
	return (NULL);
    n = maint_NeighborCast(RBTreeSearch(maint_neighbors,key));
    return (n);
}

/*
 * maint_NeighborAger
 * Age neighbor entries to reflect their goodness.
 *
 * NOTE: This function is now RTMP-centric. IGRP/DUAL does the aging of
 * its own neighbor database and will tell us when a neighbor is no
 * longer a feasible successor. But we do aging of IGRP neighbor status
 * in here as well.  An IGRP neighbor which hasn't been heard from in
 * 3 hello intervals is gone.
 */
static void
maint_NeighborAger (void)
{
    neighborentry* n;
    int old_state;

    atalk_obsoleteRouterDetected |= atalk_obsoleteRouterActive;
    atalk_obsoleteRouterActive = FALSE;

    for (n=maint_GetFirstNeighbor(); n!=NULL; n=maint_GetNextNeighbor(n)) {
	/*
	 * If this neighbor is already down, skip and go onto the
	 * next neighbor entry.
	 */
	if (n->state == neighborDown)
	    continue;

	if (n->obsoleterouter)
	    atalk_obsoleteRouterActive = TRUE;

	reg_invoke_atalk_age_neighbor(n);
	
	/* If neighbor is no longer reachable by anyone then mark it down */
	if (!atmaint_NeighborIsReachable(n) && !n->snapshot_no_age &&
	    !n->static_entry) {
	    old_state = n->state;
	    n->state = neighborDown;
	    GET_TIMESTAMP(n->time_went_down);
	    if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, n->port->phys_idb))
		buginf("\nAT: State change for neighbor %a: %s -> %s",
		       n->address,
		       atmaint_NeighborState(old_state),
		       atmaint_NeighborState(neighborDown));
	}
    }
}

/*
 * atmaint_RestartNeighbor
 * Clear counters and flags which may change after a restart.
 */
void
atmaint_RestartNeighbor (neighborentry* n, sys_timestamp when)
{
    n->restarts++;
    COPY_TIMESTAMP(when, n->restarted);
    n->obsoleterouter = FALSE; /* Maybe it was upgraded */
    n->configerror = FALSE;
    n->oldcisco = FALSE;
    TIMER_STOP(n->qtimer);
    if (at_debug(atalk_events, ATALK_INTERFACE_MASK, n->port->phys_idb))
	errmsg(AT_ERR_NEIGHBORUP, idb_get_namestring(n->port->phys_idb),
	       n->address);
}

void
atmaint_MarkNeighborObsolete (neighborentry* n) 
{
    if (atalk_extendedInternet && !n->obsoleterouter) 
	errmsg(AT_ERR_COMPATERR1, idb_get_namestring(n->port->phys_idb),
	       n->address);
    n->obsoleterouter = TRUE;
    atalk_obsoleteRouterActive = TRUE;
    n->port->atalk_compatibility = TRUE;
}

void
atmaint_NeighborConfigBad (neighborentry* n) 
{
    if (!n->configerror && at_debug(atalk_events, ATALK_INTERFACE_MASK, n->port->phys_idb)) 
	errmsg(AT_ERR_BADNEIGHBOR, idb_get_namestring(n->port->phys_idb),
	       n->address);
    n->configerror = TRUE;
}


/*
 * maint_RequestMyZones
 * Ask for my zones for aquisition or verification.
 */
static boolean
maint_RequestMyZones (atalkidbtype* idb)
{
    atalk_rdb_t* r;
    r = atroute_GetIdbRoute(idb);
    if (r == NULL) {
	if (at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\nmaint_RequestMyZones: NULL returned from GetIdbRoute");
	return (FALSE);
    }
    atalkif_clear_route_zones(idb);
    atzip_SendQuery(r);
    return (TRUE);
}

/*
 * maint_RequestMyConfig
 * Ask for my configuration via appropriate method.
 */
static boolean
maint_RequestMyConfig (atalkidbtype* idb)
{
    paktype* pak;

    if ((idb->atalk_cablestart == 0) ||
	(atalkif_ErrForPeers(idb) == NULL) || 
	(!atalkif_net_extended(idb))) {
	pak = atalk_pak_create(idb, 
			       atalk_address(ATALK_NET_UNKNOWN,
					     ATALK_BROAD_ADDR),
			       ATALK_SOCK_RTMP, ATALK_SOCK_RTMP, 
			       DDP_TYPE_RTMPReq,  RTMPREQBYTES);
	if (pak == NULL)
	    return FALSE;

	if ((!atalkif_net_extended(idb) && 
	     (atalkif_ErrForPeers(idb) != NULL)) ||
	    atalkif_HalfRouter(idb))
	    atalk_pak_setsnet(pak,ATALK_NET_UNKNOWN);
	
	*((uchar*)pak->transport_start) = RTMPReq_NetRequest;
	atalk_DDPcksum(pak);
	if (atalk_send(pak)) {
	    atalk_stat[ATALK_RTMP_OUT]++;
	    if (at_debug(atalkrtmp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		buginf("\nAT: Sent net query to %s",
		       idb_get_namestring(idb->phys_idb));
	    return TRUE;
	}
    } else {
	if (atalkif_net_extended(idb)) {
	    if (atzip_SendGetNetInfo(idb))
		return TRUE;
	} else {
	    if (maint_RequestMyZones(idb))
		return TRUE;
	}
    }
    return FALSE;
}

/*
 * maint_NextNeighborZipQuery
 * Gather pending requests for a neighbor.
 */
atalk_rdb_t*
atmaint_NextNeighborZipQuery (neighborentry* n,atalk_rdb_t* op)
{
    atalk_rdb_t* r = op;

    while ((r = maint_GetNextRequest(r)) != NULL) {
	if (r->zoneupdate)
	    continue;
	/* Note that zonetimer is 0 when a request is first scheduled.
         * In this case, we will assume that it's time to query this route.
         */
	if (TIMER_RUNNING_AND_SLEEPING(r->zonetimer))
	    continue;
	if (r->rpath.gwptr != n)
	    continue;
	if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, n->port->phys_idb)) {
	    buginf("\nAT: NextNbrZipQuery: [%#A] zoneupdate %d gw: %a n: %a",
		   atalk_CableRange(r), r->zoneupdate,
		   r->rpath.gwptr->address, n->address);
	    buginf("\nAT: NextNbrZipQuery: r->rpath.gwptr: %08x, n: %08x",
		   r->rpath.gwptr, n);
	}
	return (r);
    }
    return (NULL);
}

/*
 * maint_SetRequestTime
 * Set the request time based upon retries.
 */
static void
maint_SetRequestTime (atalk_rdb_t* r)
{
    int t;
    
    t = ++r->zoneretries;
    if (t > MAX_ZIP_RETRY_MULTIPLIER)
	t = MAX_ZIP_RETRY_MULTIPLIER;
    TIMER_START(r->zonetimer, t * atalk_zipQueryInterval);
    mgd_timer_set_soonest(&maint_zoneRequestTime, r->zonetimer);
}

/*
 * maint_SendNeighborQueries
 * Build and send a zip query packet to a neighbor.
 */
static int
maint_SendNeighborQueries (neighborentry* n)
{
    paktype *pak;
    ziptype *zpkt;
    uchar *zip;
    atalk_rdb_t* p;
    sys_timestamp timeo;
    int qcnt;
    int bytes;
    ushort domain_net;



    p = atmaint_NextNeighborZipQuery(n,NULL);
    if (p == NULL) {
	if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, n->port->phys_idb))
	    buginf("\nAT: maint_SendNeighborQueries, no pending queries");
	return (FALSE);
    }

    pak = atalk_pak_create(n->port, n->address,
			   ATALK_SOCK_ZIP, ATALK_SOCK_ZIP, DDP_TYPE_ZIP, 
			   DDP_MAX_DATABYTES);
    if (pak == NULL) {
	if (   at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, n->port->phys_idb)
	    || at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, n->port->phys_idb)) {
	    buginf("\nAT: maint_SendNeighborQueries getbuffer() failure");
	}
	atalk_stat[ATALK_NO_BUFFERS]++;
	return (FALSE);
    }
    pak->if_output = n->port->phys_idb;
    zpkt = (ziptype *)pak->transport_start;
    zpkt->cmd = ZIP_Query;
    zip = ((uchar *)zpkt)+ZIPTYPEBYTES;
    bytes = pak->atalk_datalen - ZIPTYPEBYTES;
    qcnt = 0;
    timeo = n->qtimer;
    while ((p != NULL) && (qcnt++ < NEIGHBOR_QUERIES_PER_PACKET)) {
	PUTSHORT((uchar *)zip, p->dstrngstr);

	/* If it is a domain idb, 
         * then substitute this net for a real domain net 
         */

	if(p->rpath.idb && p->rpath.idb->atalk_dminfo) {
	     domain_net = atdomain_FindBackwardRemap(p->rpath.idb->atalk_dminfo->in,
						     p->dstrngstr);
	     if (domain_net)
	    	PUTSHORT((uchar *)zip, domain_net);
	 }

	zip += DDPNETBYTES;
	bytes -= DDPNETBYTES;
	maint_SetRequestTime(p);
	timeo = TIMER_SOONEST(timeo, p->zonetimer);
	p = atmaint_NextNeighborZipQuery(n,p);
    }
    if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, n->port->phys_idb)) {
	buginf("\nAT: maint_SendNeighborQueries, sending %d queries to %#a",
	       qcnt, n->address);
    }
    atalk_pak_setlength(pak,ZIPTYPEBYTES + (DDPNETBYTES * qcnt));
    zpkt->cnt = qcnt;
    atalk_DDPcksum(pak);
    if (atalk_send(pak)) {
	n->qcount += qcnt;
	n->qtimer = timeo;
	atalk_stat[ATALK_ZIP_OUT]++;
	return (p == NULL) ? -1 : 1; /* Indicate if scan completed */
    }
    
    return (FALSE);
}

/*
 * maint_HandleMiscQ
 */
static void
maint_HandleMiscQ (void)
{
    paktype* pak;

    /* while more packets... */
    while ((pak = process_dequeue(atalkmiscQ)) != NULL) {
	/* dequeue then checksum, but no other error checks: */
	if (!pak || !atalk_check(pak, DDP_TYPE_Invalid, -1, FALSE))
	    continue;			/* noone home, checksum, other error */
	if (!atalk_running)
	    atalk_dispose(-1, pak);
	    
	switch (atalk_pak_dsock(pak)) {
	case ATALK_SOCK_PING:
	    atalk_PingHandler(pak);
	    break;
	case ATALK_SOCK_OURNAME:
	    atalk_Responder(pak);
	    break;
	case ATALK_SOCK_ECHO:
	    atalk_EchoHandler(pak);
	    break;
	default:
	    atalk_pak_printfail(pak, atalktrue_debug, "no packet handler");
	    atalk_dispose(ATALK_UNKNOWN,pak);
	    break;
	}
    }
}

/*
 * maint_SchedIFMaint
 * Schedules an IF maintenance run and returns a time to sleep until
 */
static sys_timestamp
maint_SchedIFMaint (ulong interval)
{
    sys_timestamp now, timval;

    if (!TIMER_RUNNING(maint_ifStartClock)) {
	GET_TIMESTAMP(now);
    } else
	now = maint_ifStartClock;

    timval = TIMER_ADD_DELTA(now, max(interval, atalk_stateinterval));
    mgd_timer_set_soonest(&maint_ifStateTime, timval);
    return (timval);
}

/*
 * atmaint_ScheduleIFWakeup - Reset IF state timers/counters and signal a
 * wakeup.
 */
void
atmaint_ScheduleIFWakeup (atalkidbtype* idb)
{
    ATASSERT(idb != NULL);
    TIMER_STOP(idb->atalk_timer);
    idb->atalk_counter = 0;
    maint_SchedIFMaint(0);
}

/*
 * maint_GotoState - Set the ports current operational state
 */
static void
maint_GotoState (atalkidbtype* idb, opStatus stat)
{
    char *olds;
    char const *idb_name;
    
    ATASSERT(idb != NULL);

    idb_name = idb_get_namestring(idb->phys_idb);
    
    atmaint_ScheduleIFWakeup(idb);
    if (stat == idb->atalk_status)
	return;
    olds = atalkif_ShortStatus(idb);
    if (at_debug(atalk_events, ATALK_INTERFACE_MASK, idb->phys_idb)) {
	if ((idb->aarp_tentative == AARP_ADDR_SET) && (stat == opNetDown))
	    errmsg(AT_ERR_INTDOWN, idb_name);
	else if ((idb->atalk_status == opNetDown) && (stat == opProbing))
	    errmsg(AT_ERR_INTUP, idb_name);
    }
    idb->atalk_status = stat;
    if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	buginf("\n%s: AppleTalk state changed; %s -> %s",
	       idb_name, olds, atalkif_ShortStatus(idb));
}

/*
 * atalkif_disms - Sleep for the specified period of time
 */
static sys_timestamp
maint_disms (atalkidbtype* idb, int period)
{
    sys_timestamp then = maint_SchedIFMaint(period);
    if (period == 0) {		/* special case, clear timer */
	TIMER_STOP(idb->atalk_timer);
	return then;
    }
    idb->atalk_timer = then;
    return then;
}

/*
 * maint_RestartPort
 * Handle the restarting of the port in the state machine
 */
static void
maint_RestartPort (atalkidbtype* idb) 
{

    atalkif_ResetMulticasts(idb);
    if ((!idb->atalk_enabled)||!atalk_running) {
	maint_GotoState(idb,opUnknown);
	return;
    }
    if (!interface_up(idb->phys_idb)) {
	atalkif_DisablePort(idb);
	maint_GotoState(idb,opNetDown);
	maint_disms(idb,atalk_restartinterval);
	return;
    }
    if (!atalkif_InterfaceConfigured(idb)) 
	maint_GotoState(idb,opConfigBad);
    else if (!atalkif_InternetCompatible(idb)) 
	maint_GotoState(idb,opCompatBad);
    else if (!atalkif_InterfaceCompatible(idb)) 
	maint_GotoState(idb,opLineProtoErr);
    else {
	maint_GotoState(idb,opProbing);
	maint_disms(idb,atalk_configqueryinterval);
    }
}

/*
 * maint_ProbePort
 */
static void
maint_ProbePort (atalkidbtype* idb) 
{
    ataddrtype	probeseed = atalk_address(idb->atalknet,idb->atalknode);
    hwidbtype	*hwidb;
    char const *idb_name;
    


    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == NULL)
	return;
    idb_name = idb_get_namestring(idb->phys_idb);
    
    /*
     * At this point, we don't need to have a route installed in the
     * routing table for the connected interface; we're just checking
     * interface configuration and address.
     */
    if (!atalkif_InternetCompatible(idb) ||
	(!idb->atalk_discovery && !atalkif_InterfaceValid(idb))) {
	maint_GotoState(idb,opCompatBad);
	maint_disms(idb,atalk_restartinterval);
	return;
    }
    if (idb->atalk_discovery)
	atalkif_clear_zones(idb);

    /*
     * Now we're going to pick an interface address. Again, we don't need
     * the route to be installed yet.
     */
    if (!aarp_pick_addr(idb)) {
	if (atalkif_InterfaceUp(idb)) {
	    errmsg(AT_ERR_NOADDRSAVAIL, idb_name);
	    maint_GotoState(idb,opNoAddress);
	} else {
	    atalkif_DisablePort(idb);
	    maint_GotoState(idb,opNetDown);
	}
	maint_disms(idb,atalk_restartinterval);
	return;
    }

    /*
     * If the address changed from what the user configured, then
     * emit a message notifying him.
     */
    if ((probeseed != atalkif_MyAddress(idb))
	|| at_debug(atalk_events, ATALK_INTERFACE_MASK, idb->phys_idb))
	errmsg(AT_ERR_ADDRUSED, idb_name,
	       atalkif_CaymanTunnel(idb) || atalkif_AURPTunnel(idb)
	       ? 0 : atalkif_MyAddress(idb));


    if (idb->atalk_discovery || (atalkif_ErrForPeers(idb) == NULL)) {
	/*
	 * In discovery mode or peer mode (ie, serial line peer),
	 * we'll create the route in atalkif_NetAcquired.
	 */
	
	/* check for DDR */
	if (is_ddr(hwidb)) {
	    /* 
	     * With DDR F/S should be disabled
	     */
	    hwidb->atalk_fastokay  = FALSE;
	    idb->atalk_operational = FALSE;
	    atfast_SetupIF(idb);
	    /*
	     * When DDR is on we go right to opValid state
	     */
	    maint_GotoState(idb,opValid);		    
	    return;
	}
	
	if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    errmsg(AT_ERR_ACQUIREMODE, idb_name,
		   idb->atalk_discovery?"network configuration":"remote peer");
	maint_GotoState(idb,opAcquire);
    } else {
	if (atalkif_InterfaceValid(idb)) {
	    /*
	     * no need to verify port config for Cayman Tunnels,
	     * since tunnels are unnumbered configurations.
	     */
	    if (atalkif_CaymanTunnel(idb))
		maint_GotoState(idb, opValid);
	    else {
		if (atalkif_CreateConnectedPath(idb) == NULL) {
		    if (at_debug(atalkerr_debug, ATALK_NULL_MASK))
			buginf("\nAT: maint_probeport, failed to add route"
			       " in maint_ProbePort");
		    maint_GotoState(idb, opConfigBad);
		    return;
		}
		maint_GotoState(idb, opVerify);
	    }
	} else {
	    /*
	     * Dismiss for a couple of minutes while we wait for
	     * misconfiguration to clear up.
	     */
	    maint_GotoState(idb, opRestartPort);
	}
    }
}

/*
 * maint_VerifyPort
 */
static void
maint_VerifyPort (atalkidbtype* idb)
{
    if (idb->atalk_counter > atalk_configquerycount) {
	if (!idb->atalk_discovery) 
	    maint_GotoState(idb,opValid);
	else
	    maint_GotoState(idb,opAcquire);
	return;
    }
    maint_RequestMyConfig(idb);
    maint_disms(idb,atalk_configqueryinterval);
}

static void
maint_AcquirePort (atalkidbtype* idb)
{
    if (idb->atalk_counter > atalk_configquerycount) {
	maint_GotoState(idb,opAcquire);
	maint_RequestMyConfig(idb); /* Gee, its been a while, retry */
    }
				/* We sit in acquire waiting for an RTMP */
    maint_disms(idb,atalk_restartinterval);
}

static void
maint_AcquirePeer (atalkidbtype* idb)
{
   if (idb->atalk_counter > atalk_configquerycount) {
	maint_GotoState(idb,opAcquirePeer);
	maint_RequestMyConfig(idb); /* Gee, its been a while, retry */
    }
				/* We sit in aquire waiting for an RTMP */
   atrtmp_SendUpdate(idb,atalkif_MyNet(idb),0,0,TRUE);
   maint_disms(idb,atalk_configqueryinterval);
}

/*
 * General theory, set desired state and alert Maintenance.  This
 * procedure performs the action required by the state and handles
 * retransmission etc.  Maintenance invokes StateHandler for each port
 * in a state other than state is either opUnknown OR opValid.
 * Whenever the state is changed, the atalk timer and counter
 * variables are reset to zero.  Counter is used to determine the
 * number of StateHandler invocations since a state change and timer
 * allows sleep/wait periods to be automated.  
 *
 * Returns time for next invocation, zero if no more calls are needed
 */

/*
 * maint_StateHandler - Port finite state machine for AppleTalk.
 */
static sys_timestamp
maint_StateHandler (atalkidbtype* idb)
{
    opStatus oldstate = idb->atalk_status;

    if (TIMER_RUNNING(idb->atalk_timer)
	&& XSLEEPING(idb->atalk_timer, ATALK_RESTART_FREQ))
	return (idb->atalk_timer); 	/* Continue waiting for a while */
    
    switch (idb->atalk_status) {

    case opValid:			/* AT is operational */
	(void) atalkif_InternetCompatible(idb);	/* issue warnings */
	atalkif_SetOperational(idb);
	/* now collect all the non-active statics
         * pointing to this idb
	 */
	if (atalk_static)
	    at_static_collect_routes(idb);
	/* If it is a domain router,
	 * Now we have to adjust all the domain stuff 
	 */
	if(idb->atalk_dminfo)
	    atdomain_InitialAdjust(idb);
	return (timer_no_time);

    case opConfigBad:			/* Configuration info incomplete */
    case opLineProtoErr:		/* Line proto incompatible w/ config */
    case opInternalBug:			/* Unrecoverable situation */
	atalkif_DisablePort(idb);
	return(timer_no_time);

    case opUnknown:			/* AT is not enabled on this port */
	if (idb->atalk_enabled) {
	    atalkif_RestartPort(idb);
	    break;
	}
	return(timer_no_time);

    case opProbing:			/* Attempt to obtain a node address */
	maint_ProbePort(idb);
	break;

    case opVerify:			/* Verifying cable information */
	idb->atalk_confaddr = 0;	/* reset when trying anyone */
	maint_VerifyPort(idb);
	break;

    case opAcquire:			/* Acquiring port info */
	if (idb->atalk_discovery) {
	    maint_AcquirePort(idb);
	    return idb->atalk_timer;
	    break;
	}
	/* FALL THROUGH */
    case opAcquirePeer:
	maint_AcquirePeer(idb);
	return idb->atalk_timer;
	break;

    case opAcquireZones:		/* Acquiring zone information */
	if (idb->atalk_counter > atalk_configquerycount) {
	    maint_GotoState(idb,opAcquire);
	} else {
	    maint_RequestMyZones(idb);
	    maint_disms(idb,atalk_configqueryinterval);
	}
	break;
    case opVerifyZones:			/* Cable info OK - Check zones */
	if (idb->atalk_counter > atalk_configquerycount) {
	    if (atalkif_net_extended(idb) && 
		(atalkif_ErrForPeers(idb) != NULL))
		maint_GotoState(idb,opVerify);
	    else
		maint_GotoState(idb,opValid);
	    break;
	}
	maint_RequestMyZones(idb);
	maint_disms(idb,atalk_configqueryinterval);
	break;
    case opValidate:			/* Validate final port configuration */
	if (idb->atalk_counter > atalk_configquerycount) {
	    maint_GotoState(idb,opVerify);
	    break;
	}
	if (!atalkif_net_extended(idb)) {
	    maint_GotoState(idb,opValid);
	    break;
	}
	maint_RequestMyConfig(idb);
	maint_disms(idb,atalk_configqueryinterval);
	break;
    case opNoAddress:			/* Unable to obtain a node address */
	atalkif_DisablePort(idb);
	maint_GotoState(idb,opRestartPort);
        return (maint_disms(idb,atalk_restartinterval));
	break;
    case opNetDown:			/* Line protocol down, check if back */
	if (!interface_up(idb->phys_idb)) {
	    maint_GotoState(idb,opNetDown);
	    return (maint_disms(idb,atalk_restartinterval));
	} else
	    maint_GotoState(idb,opRestartPort);
	break;

    case opResExhausted:
	maint_GotoState(idb,opRestartPort);
	break;

    case opRemotePeerErr:		/* Remote peer incorrectly configed */
	maint_GotoState(idb,opAcquirePeer);
	maint_disms(idb,atalk_restartinterval);
	break;

    case opCompatBad:
    case opNetConflict:			/* port parameters conflict */
	maint_GotoState(idb,opRestartPort);
	maint_disms(idb,atalk_restartinterval);
	break;

    case opCableMismatch:		/* Our config doesn't match nbr's */
	if (idb->atalk_discovery)  {
	    atalkif_ClearLocalConfig(idb);
	    maint_GotoState(idb,opAcquire);
	    maint_disms(idb,atalk_configqueryinterval);
	} else {
	    if (idb->atalk_counter > 0) {
		/*
		 * You need to find out, again, if EITHER nets or zones
		 * are bad. Re-start the world for this interface.
		 */
		maint_GotoState(idb,opRestartPort);
	    }
	    atalkif_DisablePort(idb);
	    maint_disms(idb,atalk_restartinterval);
	}
	break;

    case opHRSupportErr:		/* Half-router's config error */
	maint_GotoState(idb,opLineProtoErr);
	break;

    case opRestartPort:			/* Startup an AT port */
	atalkif_DisablePort(idb);
	maint_RestartPort(idb);
	break;

    case opInternalError:
	atalkif_RestartPort(idb);
	maint_disms(idb,atalk_restartinterval);
	break;

    default:
	atalkif_DisablePort(idb);
	if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\nAT: Unexpected state for port %s FSM (%d)",
		   idb_get_namestring(idb->phys_idb), idb->atalk_status);
	maint_GotoState(idb,opInternalError);
	break;
    }

    if (idb->atalk_status != oldstate)
	return (maint_disms(idb,0)); /* Special indicator for stateinterval */

    if (!idb->atalk_enabled || !atalk_running) {
	maint_GotoState(idb,opUnknown);
	return(timer_no_time);
    }
    if (!interface_up(idb->phys_idb)) {
	atalkif_DisablePort(idb);
	maint_GotoState(idb,opNetDown);
	return (maint_disms(idb,atalk_restartinterval));
    }

    if (idb->atalk_counter++ > ATALK_STATE_RETRIES) {
	if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\nAT: Port %s stuck in state %d (%s)",
		   idb_get_namestring(idb->phys_idb), idb->atalk_status,
		   atalkif_opstatusmsg(idb));
	maint_GotoState(idb,opInternalError);
	return (maint_disms(idb,atalk_restartinterval));
    }

    if (!TIMER_RUNNING_AND_SLEEPING(idb->atalk_timer))
	return (maint_disms(idb, atalk_stateinterval));
    
    return (idb->atalk_timer);
}

/*
 * maint_HandlePorts
 * Adjust any ports which need service
 */
static void
maint_HandlePorts (void)
{
    atalkidbtype *idb;
    sys_timestamp nextrun;

    if (!atalk_running)
      return;
    TIMER_STOP(nextrun);
    mgd_timer_stop(&maint_ifStateTime);
    GET_TIMESTAMP(maint_ifStartClock);

    FOR_ALL_ATIDBS(idb) {
	if (idb->atalk_counter >= 0) {
	    nextrun = maint_StateHandler(idb);
	    if (!TIMER_RUNNING(nextrun)) {
		idb->atalk_counter = -1;
	    } else
		mgd_timer_set_soonest(&maint_ifStateTime, nextrun);
	}
    }
    TIMER_STOP(maint_ifStartClock);
}

/*
 * atalkif_RestartPort - General routine to restart an AT port
 */
void
atalkif_RestartPort (atalkidbtype* idb)
{
    hwidbtype	*hwidb;

    ATASSERT(idb != NULL);
    hwidb = hwidb_or_null(idb->phys_idb);
    ATASSERT(hwidb != NULL);

    if (!(maint_started && idb->atalk_enabled))
	return;

    if (is_tunnel(hwidb)) {
	idb->atalk_enctype = ET_TUNNEL;
	idb->aarp_enctype = ET_NULL;
	/* In the future, change this to handle Cayman Tunnels, too. */
	if (atalkif_AURPTunnel(idb))
	    return;
    } else if (hwidb->status & IDB_SERIAL) {
	if (atalkif_net_extended(idb)) {
	    switch (hwidb->enctype) {
	    case ET_SMDS:
		idb->atalk_enctype = hwidb->enctype;
		idb->aarp_enctype = ET_SMDS;
		break;
	    case ET_CPP:
		idb->atalk_enctype = ET_SNAP;
		idb->aarp_enctype = ET_SNAP;
		break;
	    case ET_PPP:
		if (hwidb->ppp_bridge_atalk) {
		    idb->atalk_enctype = ET_SNAP;
		    idb->aarp_enctype = ET_SNAP;
		    break;
		}
		/* else fall through */
	    case ET_FRAME_RELAY:
	    case ET_FR_IETF:
	    case ET_HDLC:
	    case ET_X25:
	    case ET_LAPB:
	    case ET_ATM_DXI:
		idb->atalk_enctype = hwidb->enctype;
		idb->aarp_enctype = ET_NULL;
		break;

	    default:			/* anything else */
		idb->atalk_enctype = ET_NULL;
		idb->aarp_enctype = ET_NULL;
		break;
	    }
 	} else {			/* non-extended serial */
	    switch (hwidb->enctype) {
	    case ET_SMDS:
		idb->atalk_enctype = ET_SMDS;
		idb->aarp_enctype = ET_NULL;
		break;
	    case ET_FRAME_RELAY:
		idb->atalk_enctype = ET_FRAME_RELAY;
		idb->aarp_enctype = ET_NULL;
		break;
	    case ET_FR_IETF:
		idb->atalk_enctype = ET_FR_IETF;
		idb->aarp_enctype  = ET_NULL;
		break;
	    case ET_ATM_DXI:
		idb->atalk_enctype = ET_ATM_DXI;
		idb->aarp_enctype  = ET_NULL;
		break;
	    case ET_CPP:
		idb->atalk_enctype = ET_ETHERTALK;
		idb->aarp_enctype = ET_ARPA;
		break;
	    case ET_PPP:
		if (hwidb->ppp_bridge_atalk) {
		    idb->atalk_enctype = ET_ETHERTALK;
		    idb->aarp_enctype = ET_ARPA;
		    break;
		}
		/* else fall through */
	    default:
		/*
		 * All others, including HDLC, come here.
		 */
 		idb->atalk_enctype = ET_ETHERTALK;
 		idb->aarp_enctype = ET_NULL;
		break;
	    }
  	}
    } else if (is_atm_nolane(idb->phys_idb)) {
	idb->atalk_enctype = ET_ATM;
	idb->aarp_enctype = ET_ATM;
    } else if (atalkif_net_extended(idb)) {
	idb->atalk_enctype = ET_SNAP;
	idb->aarp_enctype = ET_SNAP;
    } else {
	idb->atalk_enctype = ET_ETHERTALK;
	idb->aarp_enctype = ET_ARPA;
    }

    atmaint_ScheduleIFWakeup(idb);
    atalkif_ResetMulticasts(idb);
    if (!interface_up(idb->phys_idb)) {
	atalkif_DisablePort(idb);
	maint_GotoState(idb,opNetDown);
    } else
	maint_GotoState(idb,opRestartPort);
    maint_disms(idb,atalk_configqueryinterval);
}

boolean 
atmaint_ZonesMatch (atalkidbtype *idb, atalk_rdb_t *r)
{
    zipnetentry *zn;
    int cnt = 0;

    zn = atalk_firstZone(r);
    while (zn != NULL) {
	cnt++;
	if (atalkif_findZone(idb, zn->zone) == NULL)
	    return FALSE;
	zn = atalkzn_nextZone(zn);
    }
    return (cnt == idb->atalk_zonecount);
}

/*
 * maint_ZonesMatch - Check that the route and if zones are the same.
 */
static boolean
maint_ZonesMatch (atalkidbtype* idb)
{
    atalk_rdb_t* r = atroute_GetIdbRoute(idb);

    ATASSERT(idb != NULL);
    ATASSERT(r != NULL);
    if (idb->atalk_zonecount < 1)
	return FALSE;
    
    return atmaint_ZonesMatch(idb, r);
}

/*
 * atalkif_RouteZonesReady - Zone update finished successfully
 */
void 
atalkif_RouteZonesReady (atalkidbtype* idb)
{
    switch (atalkif_opstatus(idb)) {
    case opAcquireZones:
	atalkif_AcquireZones(idb);
	maint_GotoState(idb,opVerify);
	return;
    case opVerifyZones:
	if (maint_ZonesMatch(idb))
	    if (idb->atalk_zonecount == 1)
		maint_GotoState(idb,opValid);
	    else
		maint_GotoState(idb,opValidate);
	else
	    if (atalk_ignoreVerifyErrs)
		maint_GotoState(idb,opValid);
	    else
		maint_GotoState(idb,opCableMismatch);
	return;
    default:
	break;
    }
}

/*
 * atalkif_CableInfoReport - Heard a cable configuration.
 *
 * This 
 */
void
atalkif_CableInfoReport (atalkidbtype *idb,ushort snet,ushort enet,
			 ataddrtype src)
{
    uchar tnode;
    ushort tnet;
    atalk_rdb_t* r = NULL;
    atalk_pdb_t* idb_path;
    atalk_rdb_t* idb_route;
    rbTree 	*tree;
    char const *idb_name;

    idb_name = idb_get_namestring(idb->phys_idb);
    
    tree = atroute_GetTree();
    if (atalkif_opstatus(idb) != opAcquire) {
	if (!atroute_IsRouteValid(tree,snet,enet) && !idb->atalk_discovery) {
	    if (at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		buginf("\nAT: Bad cable report for %d-%d via %s",
		       snet,enet, idb_name);
	    return;
	}

	/*
	 * See if we have a route with a conflicting config on the net.
	 */
	r = atroute_LookupRoute(tree,snet, FALSE);
	if (r != NULL) {
	    idb_path = atalkif_GetIdbPath(idb);
	    idb_route = atroute_GetIdbRoute(idb);
	    if (idb_route != NULL && r != idb_route) {
		errmsg(AT_ERR_NETINVALID, idb_name);
		atalkif_ConfigError(idb,"cable conflicts with internet");
		return;
	    }
	    if (atalk_net_connected(r) && (r->rpath.idb != idb)) {
		errmsg(AT_ERR_IFCONFLICT, idb_name,
		       idb_get_namestring(r->rpath.idb->phys_idb));
		atalkif_ConfigError(idb, "port clash");
		return;
	    }
	}
    }

    switch (atalkif_opstatus(idb)) {
    case opAcquire:
	tnode = idb->atalknode;
	tnet = idb->atalknet;
	if ((tnet < snet) || ((tnet > enet) && (enet != 0)))
	    tnet = 0;
	if ((atalkif_ErrForPeers(idb) == NULL) && (tnode == 0)) {
	    tnode = atalk_nodeonly(src);
	    tnode ^= tnode;
	}
	if (atalkif_setup(idb,snet,enet,tnet,tnode, FALSE)) {
	    idb->atalk_confaddr = src;
	    atalkif_NetAcquired(idb);
	}
	return;
    case opAcquirePeer:
    case opVerify:
	if ((idb->atalk_cablestart != snet) || (idb->atalk_cableend != enet)){
	    idb->atalk_confaddr = src;
	    errmsg(AT_ERR_NETDISAGREES, idb_name,
		   src);
	    atalkif_ConfigError(idb,"net configuration disagrees");
	    return;
	}
	if (!atalkif_net_extended(idb)) {
	    idb->atalk_confaddr = src;
	    atalkif_NetConfirmed(idb);
	}
	return;
    default:
	break;
    }
    return;
}	

/*
 * atalkif_RouterProbe - Heard a router probe
 */
void
atalkif_RouterProbe (atalkidbtype* idb, ataddrtype adr)
{
    switch (atalkif_opstatus(idb)) {
    case opValidate:
    case opVerifyZones:
    case opVerify:
    case opAcquireZones:
	break;
    default:
	return;
    }
    if (idb->atalk_discovery) {
	maint_GotoState(idb,opAcquire);
	maint_disms(idb,atalk_restartinterval);
	if (   at_debug(appletalk_debug, ATALK_INTERFACE_MASK, idb->phys_idb)
	    || at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\n%s: AppleTalk port startup delayed; probe from %a",
		   idb_get_namestring(idb->phys_idb),adr);
	return;
    }
    if (adr < atalkif_MyAddress(idb)) {
	maint_GotoState(idb,opVerify);
	maint_disms(idb,atalk_random(atalk_configqueryinterval)+
		    atalk_configqueryinterval);
	if (   at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb)
	    || at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\n%s: AppleTalk port startup delayed; probe from %a",
		   idb_get_namestring(idb->phys_idb), adr);
	return;
    }
}

void
atalkif_ConfigError (atalkidbtype* idb,char* msg)
{
    if (atalk_ignoreVerifyErrs)
	return;
    if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	buginf("\nAT: Config error for %s, %s",
	       idb_get_namestring(idb->phys_idb),msg);
    if (atalkif_ErrForPeers(idb) == NULL)
	maint_GotoState(idb,opRemotePeerErr);
    else
	maint_GotoState(idb,opCableMismatch);
}

void
atalkif_NetAcquired (atalkidbtype* idb)
{
    if (!atalkif_InterfaceUp(idb)) {
	atalkif_RestartPort(idb);
	maint_GotoState(idb,opNetDown);
	return;
    }
    if (!atalkif_InternetCompatible(idb) ||
	((idb->atalk_cablestart != 0) && 
	 !atalkif_InterfaceValid(idb))) {
	maint_GotoState(idb,opCompatBad);
	maint_disms(idb,atalk_restartinterval);
	return;
    }
    atalkif_clear_route_zones(idb);

    /*
     * Create the route
     */
    if (atalkif_CreateConnectedPath(idb) == NULL) {
	buginf("\nAT: failed to add route in atalkif_NetAcquired");
	return;
    }
    
    if (atalkif_ErrForPeers(idb) == NULL) {
	maint_GotoState(idb,opVerifyZones);
	maint_disms(idb,atalk_stateinterval);
	return;
    }
    maint_GotoState(idb,opAcquireZones);
    maint_disms(idb,atalk_stateinterval);
}

void
atalkif_NetConfirmed (atalkidbtype* idb)
{
    if (atalkif_opstatus(idb)==opVerify) {
	atalkif_clear_route_zones(idb);
	maint_GotoState(idb,opVerifyZones);
    }
}

void
atalkif_PrimaryConfirmed (atalkidbtype* idb)
{
    if (atalkif_opstatus(idb)==opVerify) {
	if (idb->atalk_zonecount == 1)
	    maint_GotoState(idb,opValid);
	else
	    maint_GotoState(idb,opVerifyZones);
    } else if (atalkif_opstatus(idb)==opVerifyZones) {
	if (idb->atalk_zonecount == 1)
	    maint_GotoState(idb,opValid);
	else
	    maint_GotoState(idb,opValidate);
    } else if (atalkif_opstatus(idb)==opValidate)
	maint_GotoState(idb,opValid);
}

/*
 * atmaint_ResetRequestInfo
 * Reset zone request fields of routing entry
 */
void
atmaint_ResetRequestInfo (atalk_rdb_t* p)
{
    if (p->zonelist != NULL) {	/* Reset and retry */
	atalk_clear_zones(p);
	atalk_FreeZoneEntries(p);
    }
    p->zoneupdate = FALSE;
    TIMER_STOP(p->zonetimer);
    p->zonecnt = 0;
}

/*
 * maint_SendQuery
 * Send a query and handle request fields
 */
static void
maint_SendQuery (atalk_rdb_t* p)
{
    atzip_SendQuery(p);
    maint_SetRequestTime(p);
}

/*
 * maint_SendQueries
 * scan routing table, looking for networks without a name
 * sending out ZIP queries for all anonymous nets
 */
static void
maint_SendQueries (void)
{
    neighborentry* n;
    atalk_rdb_t* r;
    atalk_rdb_t* nr;
    int pcnt;
    int stat;

    /*
     * First, clean up the request list and generate special requests
     */
    pcnt = 0;
    mgd_timer_stop(&maint_zoneRequestTime);
    r = maint_GetFirstRequest();
    while (r != NULL) {
	nr = maint_GetNextRequest(r);
	
	/* Check if request will be handled by AURP. */
	if (atalkif_AURPTunnel(r->rpath.gwptr->port)) {
	    r = nr;
	    continue;
	}

	/* Check if request has been completed */
	if ((!r->zoneupdate && ((atalk_firstZone(r) != NULL)))) {
	    atmaint_CancelZoneRequest(r);
	    r = nr;
	    continue;
	}

	/*
	 * If update in progress but timed out
	 */
	if (r->zoneupdate && !SLEEPING(r->zonetimer)) {
	    /*
	     * If multipacket response was in progress
	     */
	    if ((r->zonecnt != 0)&&(r->zoneretries <= 1)) {
		maint_SendQuery(r);
		r = nr;
		continue;
	    }
	    atmaint_ResetRequestInfo(r);
	}
	if (!r->zoneupdate && (r->zoneretries > MAX_ZIP_GWTRIES)) 
	    maint_SendQuery(r);
	else if (!r->zoneupdate)
	    pcnt++;
	r = nr;
    }
    if (pcnt == 0)
	return;
    /*
     * Now, for each neighbor, scan the request list for processing
     */
    for (n = maint_GetFirstNeighbor();n!=NULL;n = maint_GetNextNeighbor(n)) {
	if (n->state != neighborUp)
	    continue;
	if (atalkif_AURPTunnel(n->port)) /* AURP handles its own ZIP Reqs */
	    continue;
	if (TIMER_RUNNING_AND_SLEEPING(n->qtimer))
	    continue;
	TIMER_STOP(n->qtimer);
	pcnt = 0;
	maint_ProtectNeighbor(n);
	while (pcnt < NEIGHBOR_PACKET_LIMIT) {
	    stat = maint_SendNeighborQueries(n);
	    if (stat != 0)
		pcnt++;
	    if (stat <= 0)
		break;
	}
	if (pcnt > 0) {
	    if (at_debug(atalkzip_debug, ATALK_INTERFACE_MASK, n->port->phys_idb)) 
		buginf("\nAT: %d query packet%s sent to neighbor %a",
		       ArgAndPlural(pcnt,"","s"),n->address);
	    n->qpackets += pcnt;
	}
	maint_UnprotectNeighbor(n);
    }
    if (maint_pendingZoneRequests &&
	!mgd_timer_running_and_sleeping(&maint_zoneRequestTime))
	mgd_timer_start(&maint_zoneRequestTime, atalk_zipQueryInterval);
}

/*
 * atmaint_FindMatchingNeighbor
 * Find a neighbor which best matches the specified criteria.
 * Any or all of the parameters may be zero which matches any.  If node is
 * specified, net must be specified as well.
 *
 * This routine has been modified to return neighbor entries even on down
 * idb's. Do not call this routine if you expect that what it returns
 * will always be an active neighbor.
 */
neighborentry*
atmaint_FindMatchingNeighbor (ataddrtype addr, atalkidbtype* pidb)
{
    atalkidbtype* idb = pidb;
    neighborentry* n;
    if (maint_neighbors == NULL)
	return NULL;

    /*
     * Loop across all neighbors in the neighbor database; check the
     * address first. If that matches, then check the idb. If both match,
     * protect & return the entry. Note that we can be passed a NULL idb,
     * so if we were, we match on address only.
     */
    n = maint_GetFirstNeighbor();
    while (n != NULL) {
	if (n->address == addr) {
	    if (idb) {
		if (idb == n->port) {
		    maint_ProtectNeighbor(n);
		    return (n);
		}
	    } else {
		maint_ProtectNeighbor(n);
		return (n);
	    }
	}
	n = maint_GetNextNeighbor(n);
    }
    return (NULL);
}

/*
 * atmaint_ReleaseNeighbor
 * Unprotect this neighbor (caller may not use this neighbor after calling)
 */
void
atmaint_ReleaseNeighbor (neighborentry* n)
{
    if (n == NULL)
	return;
    maint_UnprotectNeighbor(n);
}

/*
 * atmaint_GetNextNeighbor
 * Get and protect the next neighbor from the current one after releasing it.
 */
neighborentry*
atmaint_GetNextNeighbor (neighborentry* n)
{
    if (maint_neighbors == NULL)
	return (NULL);
    if (n != NULL)
	atmaint_ReleaseNeighbor(n);
    return (maint_GetNextNeighbor(n));
}

/*
 * atmaint_GetNeighbor
 * Return a neighbor for the specified interface and address. Can create it.
 */
neighborentry*
atmaint_GetNeighbor (ataddrtype addr, atalkidbtype* idb, boolean create)
{
    neighborentry* n = atmaint_FindNeighbor(addr,idb);
    if ((n == NULL) && create)
	n = atmaint_CreateNeighbor(addr,idb);
    if (n != NULL)
	maint_ProtectNeighbor(n);
    return (n);
}

void
atmaint_ProtectNeighbor (neighborentry* n)
{
    if (n != NULL)
	maint_ProtectNeighbor(n);
}

void
atmaint_UnprotectNeighbor (neighborentry* n)
{
    if (n != NULL)
	maint_UnprotectNeighbor(n);
}


void
atmaint_ScheduleZoneRequestShell (atalk_rdb_t *r, listItem **list_ptr)
{
    listItem* i = NULL;

    if (r == NULL)
	return;

    /* If it is a bad route, then don't bother putting it 
     * on the zone request list.
     * We do it only for _good_ routes.
     * Otherwise it will be stuck there forever
     */
    if(r->rpath.state >= at_state_bad)
	return;

    r->zoneretries = 0;
    atmaint_ResetRequestInfo(r);
    if (*list_ptr != NULL)
	i = SListSearch(r->dstrngstr, *list_ptr);
    if (i != NULL)
	return;
    atutil_ProtectEntry(&r->link);
    InsertListItem(r->dstrngstr, &r->maintLink, list_ptr);
    SetListItemData(&r->maintLink,r);
}

/*
 * atmaint_ScheduleZoneRequest
 * Schedule a route for periodic processing (to obtain its zones).
 * Note: routes are protected while they are scheduled
 */
void
atmaint_ScheduleZoneRequest (atalk_rdb_t* r)
{
    atmaint_ScheduleZoneRequestShell(r, &maint_pendingZoneRequests);
    if (!mgd_timer_running(&maint_zoneRequestTime))
	mgd_timer_start(&maint_zoneRequestTime, 0);
}

/*
 * atmaint_CancelZoneRequest
 *
 * Remove a route from the periodic maintenance processing queue.
 */
void
atmaint_CancelZoneRequest (atalk_rdb_t* r)
{
    listItem* i = NULL;
    if (r == NULL)
	return;
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("\nAT: in CancelZoneRequest, cancelling req on %#A...",
	       atutil_CableRange(r->dstrngstr,r->dstrngend));
    if (maint_pendingZoneRequests  != NULL)
	i = SListSearch(r->dstrngstr, maint_pendingZoneRequests);
    if (i == NULL) {
	if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	    buginf("failed; not on list");
	return;

    }
    UnlinkListItem(i, &maint_pendingZoneRequests);
    r->zoneupdate = FALSE;
    atutil_UnprotectEntry(&r->link);
    if (at_debug(atalkzip_debug, ATALK_NULL_MASK))
	buginf("succeeded");
}

void
atmaint_ScheduleGCRun (gccontext_t *gccontext)
{
    mgd_timer *timer = &gccontext->timer;
    
    if (!mgd_timer_running_and_sleeping(timer)) {
	mgd_timer_start(timer, ONEMIN);
    }
}


/*
 * maint_RegisterNames
 * Register any names which were discovered.
 */
static void
maint_RegisterNames (void)
{
    nbpQelem * ee;
    if (process_is_queue_empty(maint_nameQueue))
	return;
    while ((ee = process_dequeue(maint_nameQueue)) != NULL) {
	maint_nameEvents++;
	switch(ee->msg) {
	case NBP_REPLY_LAST:
	case NBP_REPLY:
	    if (!maint_AddNVE(ee->srvr_addr, &ee->nve)) {
		if (at_debug(appletalk_debug, ATALK_NULL_MASK))
		    buginf("\nAT: Unable to add name %s",
			   atnbp_entityStr(&ee->nve));
	    }
	    break;
	case NBP_TIMEOUT:
	    break;
	default:
	    if (at_debug(appletalk_debug, ATALK_NULL_MASK))
		buginf("\nAT: Unexpected event during name lookup - %s",
		       atnbp_msgStr(ee->msg));
	    break;
	}
	atnbp_event_free(ee);
    }
}

/*
 * maint_SendNeighborLookups -
 *	send an NBP lookup for each configured lookup type in zone <zname>
 */
static void
maint_SendNeighborLookups (atalkidbtype* idb, str32 *zname)
{
    maintlookuplist*	tl;
    nbpEntity		nve;
    ataddress		src;
    nbp_result		err;

    /* always look for ciscoRouter's: */
    atnbp_mkname(&maint_ciscoNVE, "=", cisco_NBP_type, STR32PTR(zname));
    src.net  = idb->atalk_cablestart;
    src.node = 0;
    src.spec = 0;
    if ((err = atnbp_find(&maint_ciscoNVE, &src, &maint_nameTimer, 
			  maint_nameQueue)) != NBP_OK) {
	if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\nAT: Failed to send name query - %s",
		   atnbp_resultStr(err));
	maint_nameErrors++;
    } else
	maint_nameRequests++;

    /* look for other entities as specified in config: */
    tl = maint_lookupList;
    while (tl != NULL) {
	atnbp_mkname(&nve, "=", STR32PTR(&tl->typeName), STR32PTR(zname));
	if ((err = atnbp_find(&nve, &src, &maint_nameTimer, 
			      maint_nameQueue)) != NBP_OK) {
	    if (at_debug(appletalk_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		buginf("\nAT: Failed to send query - %s",
		       atnbp_resultStr(err));
	    maint_nameErrors++;
	} else {
	    maint_nameRequests++;
	}
	tl = tl->next;
    }
}

/*
 * maint_LookupNeighborNames -
 *	perform name-cache lookups in each directly connected zone,
 *	maintain lookup interval timer.
 */
static void
maint_LookupNeighborNames (void)
{
    register ziptabletype *zz;
    register int ii;
    str32 zname;
    atalkidbtype* idb;

    maint_nameTimer.interval = ((atalk_nameLookupInterval/ONESEC)/2)+1;
    if (!mgd_timer_running(&maint_nameAgerTime))
	mgd_timer_start(&maint_nameAgerTime,
			ONEMIN + (3*atalk_nameLookupInterval));
    if (!process_is_queue_empty(maint_nameQueue))
	maint_RegisterNames();
    atnbp_cancel(maint_nameQueue);

    /* foreach slot in zone hash table... */
    for (ii = 0; ii < ZIPHASHLEN; ii++) {
	/* foreach zip entry in slot... */
	for (zz = zip_table[ii]; zz != NULL; zz = zz->next) {
	    /* make a str32: */
	    str32ninit(&zname,(char *) zz->name,zz->length);

	    /* pick an interface in the zone, if any: */
	    if ((idb = pickIDBinZone(&zname, NULL)) != NULL) {
		/* send lookups: */
		maint_SendNeighborLookups(idb, &zname);
	    }
	}
    }
}

boolean
atmaint_NeighborIsReachable (neighborentry *n)
{
    return ((n->reachable_by & (ATALK_IGRP_ENABLED | ATALK_RTMP_ENABLED
				| ATALK_AURP_ENABLED)) != 0);
}

static void
maint_AgeNameCache (void)
{
    maintNVE nve = maint_GetFirstNVE();
    while (nve != NULL) {
	if (!SLEEPING(nve->timer)) {
	    maint_DeleteNVE(nve);
	    nve = maint_GetFirstNVE();
	    continue;
	}
	nve = maint_GetNextNVE(nve);
    }
}

gccontext_t *
atmaint_AddGCContext (boolproc proc, int resched_int)
{
    gccontext_t *gccontext;

    gccontext = chunk_malloc(atalk_GCContexts);
    if (gccontext == NULL)
	return (NULL);
    
    mgd_timer_init_leaf(&gccontext->timer, &atalk_maint_timer,
			ATTIMER_MAINT_RUNGC, gccontext, FALSE);
    gccontext->proc = proc;
    gccontext->resched_int = resched_int;
    return gccontext;
}

/*
 * maint_RunGC
 *
 * Run the GC routine associated with this timer.
 *
 * Check the boolean return value from the GC function.  If true,
 * then reschedule.  Otherwise, stop the timer.
 */
static void
maint_RunGC (mgd_timer *gctimer)
{
    gccontext_t *gc_context;
    boolproc gcproc;
    int resched_int;

    gc_context = mgd_timer_context(gctimer);
    gcproc = gc_context->proc;
    
    if ((*gcproc)() == TRUE) {	/* Run the gc routine */
	mgd_timer_stop(gctimer);
    } else {			/* gc not done, so reschedule */
	resched_int = gc_context->resched_int;
	
	if (resched_int > 0)
	    mgd_timer_start(gctimer, resched_int * ONESEC);
	else
	    mgd_timer_start(gctimer, ONEMIN);
    }
}

static void
atalk_maint_do_timers (void)
{
    mgd_timer *expired_timer;

    while (mgd_timer_expired(&atalk_maint_timer)) {
	expired_timer = mgd_timer_first_expired(&atalk_maint_timer);
	switch (mgd_timer_type(expired_timer)) {
	case ATTIMER_MAINT_ROUTE:
	    mgd_timer_start_jittered(expired_timer, atalk_rtmpUpdateTime,
				     atalk_rtmp_jitter);
	    if (atalk_running) {
		atrtmp_SendUpdates();
		process_may_suspend();
		atroute_Ager();
		process_may_suspend();
	    }
	    break;
	case ATTIMER_MAINT_NEIGHBOR_AGER:
	    mgd_timer_start(expired_timer, atalk_rtmpValidTime);
	    maint_NeighborAger();
	    break;
	case ATTIMER_MAINT_IFSTATE:
	    maint_HandlePorts();
	    break;
	case ATTIMER_MAINT_ZONE:
	    maint_SendQueries();
	    break;
	case ATTIMER_MAINT_NBPREQUEST:
	    mgd_timer_stop(expired_timer);
	    atnbp_background();
	    break;
	case ATTIMER_MAINT_NAMELOOKUP:
	    mgd_timer_start(&maint_nameLookupTime, atalk_nameLookupInterval);
	    maint_LookupNeighborNames();
	    break;
	case ATTIMER_MAINT_FSCACHEAGER:
	    mgd_timer_start(expired_timer, ONEMIN);
	    atalk_fscache_age();
	    break;
	case ATTIMER_MAINT_NAMEAGER:
	    mgd_timer_start(expired_timer,
			    ONEMIN + (3 * atalk_nameLookupInterval));
	    maint_AgeNameCache();
	    break;
	case ATTIMER_MAINT_GC:
	    atroute_pathGC(AT_PATH_CONNECTED, "Connected");
	    mgd_timer_start(&maint_gcSleepTime, ONEMIN);
	    break;
	case ATTIMER_MAINT_RUNGC:
	    maint_RunGC(expired_timer);
	    break;
	default:
	    mgd_timer_stop(expired_timer);
	    break;
	}
    }
}

static void
atalk_maint_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;
    
    process_watch_mgd_timer(&atalk_maint_timer, DISABLE);
    process_watch_queue(atalkmiscQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(atalkmiscQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&atalkmiscQ);
    atalk_maint_pid = 0;
}

/*
 * atalk_maint - Handle AppleTalk table maintenance and ager operations
 */
process
atalk_maint (void)
{
    atalkidbtype* idb;
    ulong major, minor;
    
    process_wait_on_system_init();
    
    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, atalk_maint_teardown);
    atalkmiscQ = create_watched_queue("AT Misc packets", 0, 0);
    process_watch_queue(atalkmiscQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&atalk_maint_timer, ENABLE);
    
    atalk_BindSocket(atalkmiscQ,"AEP",ATALK_SOCK_ECHO, NULL);
    atalk_BindSocket(atalkmiscQ,"PingServer",ATALK_SOCK_PING, NULL);
    atalk_BindSocket(atalkmiscQ,"Responder",ATALK_SOCK_OURNAME, NULL);

    mgd_timer_start(&maint_nbpRequestTime, ONESEC);
    mgd_timer_start(&maint_neighborAgerTime, atalk_rtmpValidTime);
    mgd_timer_start(&maint_ifStateTime, 0);
    mgd_timer_start(&maint_routeSendTimer, 0);
    mgd_timer_start(&maint_zoneRequestTime, 0);
    mgd_timer_start(&maint_fscacheAgerTime, 0);
    mgd_timer_start(&maint_nameAgerTime, 0);
    mgd_timer_start(&maint_gcSleepTime, 0);
    
    maint_started = TRUE;
    FOR_ALL_ATIDBS(idb) {
	if (idb->atalk_enabled)
	    atalkif_RestartPort(idb);
    }
    while (TRUE) {
	process_wait_for_event();
	
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case TIMER_EVENT:
		atalk_maint_do_timers();
		break;
	    case QUEUE_EVENT:
		maint_HandleMiscQ();
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * atalkif_DisablePort
 * Disable route & paths for a connected interface.
 * Here we will trod the list of paths kept in an idb and mark all of them
 * as bad or disabled. In the aging function, we will rehabilitate routes
 * which have been affected by their [current] path being marked bad.
 *
 */
void
atalkif_DisablePort (atalkidbtype *idb)
{
    hwidbtype	*hwidb;
    atalk_pdb_t* p;

    if (idb == NULL)
	return;
    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == NULL)
	return;

    p = atalkif_GetIdbPath(idb);
    if (p != NULL) {
	atalkif_SetIdbPath(idb, NULL);
	atroute_DeletePath(p);
    }
    idb->aarp_tentative = AARP_ADDR_INVALID;
    hwidb->atalk_fastokay = FALSE;
    if (idb->atalk_operational) {
	idb->atalk_operational = FALSE;
	atfast_SetupIF(idb);		/* turn off fast-switching */
	atalkif_unregister(idb);	/* unregister NBP port name: */
    }
    idb->atalk_confaddr = 0;
    flush_aarp(idb);			/* remove our AARP entries */
    /*
     * Make sure that this is AFTER the logic which sets the IDB to
     * be non-operational.
     */
    reg_invoke_atalk_if_disable(idb);
    /* 
     * Make sure now that all the static routes
     * pointing to this idb are marked
     * as NON-ACTIVE
     */
    if (atalk_static)
	at_static_nonactive(idb);

    if(atalk_DomainRouter)
	atdomain_DisablePort(idb);
}

/*
 * atalk_start_aging
 * Start the aging of RTMP paths and neighbors on the idb specified
 */
void
atalk_start_aging (idbtype *idb, paktype *pak)
{
    atrtmp_start_aging(atalk_getidb(idb), pak);
}
