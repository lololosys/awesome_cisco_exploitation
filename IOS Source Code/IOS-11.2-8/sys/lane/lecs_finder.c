/* $Id: lecs_finder.c,v 3.1.68.6 1996/08/28 12:55:28 thille Exp $
 * $Source: /release/112/cvs/Xsys/lane/lecs_finder.c,v $
 *------------------------------------------------------------------
 *
 * the lecs finder C file
 *
 * Nov 22, 1995 C. M. Akyol (gee)
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lecs_finder.c,v $
 * Revision 3.1.68.6  1996/08/28  12:55:28  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.68.5  1996/08/26  19:46:13  cakyol
 * CSCdi66421:  the well known LECS address needs to follow the master LECS
 * Branch: California_branch
 *
 * Revision 3.1.68.4  1996/06/18  20:39:34  cakyol
 * CSCdi60677:  more than 16 LECS addresses in the switch crashes the
 * finder
 * Branch: California_branch
 *
 * Revision 3.1.68.3  1996/05/30  07:07:53  cakyol
 * CSCdi58979:  Process thrashing on watched queue - lecsFinderInputQ
 * Branch: California_branch
 *
 * Revision 3.1.68.2  1996/05/25  22:23:46  cakyol
 * CSCdi58700:  start the lecs finder ONLY when a relevant request comes
 * in.
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/05/09  14:33:17  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.94.1  1996/04/27  07:07:07  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.84.3  1996/04/25  22:12:48  cakyol
 * - rename lecsFinderModuleInit to lecs_finder_init, similar
 *   to all the other LANE module init routines.
 * - call it from the appropriate place (lane.c: lane_init).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.1.84.2  1996/04/09  07:18:25  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Incorporate code review comments regarding if/else reign of terror.
 *
 * Revision 3.1.84.1  1996/04/08  01:57:30  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.74.3  1996/04/05  03:20:46  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_ATMNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_ATMNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.74.2  1996/03/25  02:20:08  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.74.1  1996/03/22  22:55:33  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.46.11  1996/03/21  20:29:05  cakyol
 * Pass the transaction id the finder returns to the user, also
 * as the transaction id to the ILMI getnext request.  It makes
 * life much simpler during finder and ILMI debugging when these
 * numbers are the same.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.46.10  1996/03/19  20:41:24  cakyol
 * resurrect but do NOT activate the LECS Finder test process,
 * which both pounds on the Finder and the ILMI.  It proved very
 * useful having it in there.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.46.9  1996/03/15  08:00:09  cakyol
 * - take out some very misleading old comments.
 * - change some variable names to be more obvious.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.46.8  1996/03/14  08:20:15  cakyol
 * Added in a monotonically increasing ILMI transaction id, even tho
 * the Finder does not need it.  Apparently, it helps ILMI debug better.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.46.7  1996/03/12  23:23:14  cakyol
 * Use more consistent function names for ALL the ilmi and
 * signalling callback routines.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.46.6  1996/03/12  18:54:31  cakyol
 * use better strings to reflect the "FinderRequestType" in
 * debug messages.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.46.5  1996/03/11  23:03:55  cakyol
 * make the registration with ILMI bullet proof:
 * try again until successfull, if the attempt fails.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.46.4  1996/03/06  21:54:33  cakyol
 * do not use a %d to print a string, in the debugging.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.46.3  1996/03/04  23:01:16  cakyol
 * pull in the dual OID fix for getting LECS addresses using
 * the ILMI (bug CSCdi50537) manually.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.46.2  1996/02/29  00:00:34  cakyol
 * - check the validity of the LECS address once delivered
 *   from the ILMI.
 * - prefix the error messages with "lecs finder" to denote
 *   where they are coming from.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.46.1  1996/02/27  21:10:56  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.20.14  1996/02/24  01:36:08  cakyol
 * - much better ordering of debug messages.
 * - use the new "atmSig_printOpcode" api function.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.13  1996/02/22  21:08:29  cakyol
 * new scheduling paradigm expects a process to watch only
 * on a single managed timer.  So, convert multiple timers
 * to work under one master timer.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.12  1996/02/22  09:05:39  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.11  1996/02/15  00:47:56  cakyol
 * correct misleading debugging messages giving the impression
 * that they are clearing a vc, whereas what they are in fact
 * doing is acknowledging a release received from the network.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.10  1996/02/12  21:32:12  cakyol
 * convert "malloc" to "malloc_named" for better tracability
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.9  1996/02/08  00:04:13  cakyol
 * save a lot of string space by assigning __FILE__ to a
 * static char* at the beginning of the file and use that
 * throughout instead.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.8  1996/02/06  22:19:39  cakyol
 * cleanup for the last code review:
 * - get rid of "#ifdef PARANOID...#endif".  Leave some in
 *   permanently, take some out completely.
 * - get rid of all "if 0....endif" type constructs.
 * - debug messages speeded up by using variable argument
 *   macro expansion.
 * - most fatal type errors are converted to crashdump with
 *   __FILE__ and __LINE__ printing.
 * - in the LECS, do *NOT* update the table of global LECS
 *   addresses, if any outstanding signalling requests remain.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.7  1996/02/02  22:59:11  cakyol
 * - more robust locking while global LECS addresses are
 *   being updated
 * - better debugging messages
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.6  1996/02/02  02:16:28  cakyol
 * - much more robust procesing of calling party address on
 *   the LECS now.  If a call from an already existing caller
 *   is received, the old one is maintained, the new one is
 *   rejected.  Before, the new one bumped the old one off.
 * - More efficient lecs finder:
 *         - if noone wants to know the master LECS address,
 *           this step is completely eliminated saving cycles.
 *         - as soon as the global LECS addresses are determined,
 *           the users awaiting only for those are responded to
 *           immediately.  Before, they had to wait until the
 *           master LECS was determined as well.  This could take
 *           a long time if signalling was slow.
 *         - safety timer to protect against ILMI hangups is
 *           added for debugging only.
 *         - the "#ifdef TESTING" section is now taken out.
 * - long printf string cleanup done, concatenation used now.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.5  1996/01/26  02:02:49  cakyol
 * more code review mods (still more to come):
 *
 * - "show lane config" now shows all the details of the
 *   connected LESs, such as elan name, relative priority
 *   and whether active or not.
 * - in the lecs finder, protect against getnext going past
 *   the end of the lecs addresses table, by checking the
 *   incoming oid every time.
 * - in the lecs finder, add a parameter to the user interface
 *   limiting the number of lecs addresses the caller is
 *   interested in.
 * - use a static global buffer for debugging messages; saves
 *   stack space.
 * - make "les-timeout" and "lane global-lecs-address" commands
 *   hidden.
 * - prepend "atmSig_" to the 2 new atm signalling api function
 *   names.
 * - prepend "lsv_" to some LES functions for consistency.
 * - more white space, if...else formatting cleanup in some
 *   of the files.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.4  1996/01/20  02:13:12  cakyol
 * - debug message in "sendRelease" was erroneous.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.3  1996/01/18  22:46:04  cakyol
 * - active LES may very briefly be NULL while an active and
 *   a backup LES are switched over.
 * - add comments to remind me to add a safety timer to the
 *   LECS finder, in case ILMI or signalling screws up.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.2  1996/01/11  22:03:34  cakyol
 * - warn against duplicate LECS addresses from the switch
 * - update from the switch once a minute
 * - correct more out of date comments
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.1  1996/01/11  01:46:54  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.8.2  1996/01/03  22:52:56  cakyol
 * - divided up debug categories to: all, events and packets
 *   for the LECS.  There were too many debugging messages
 *   for a single debug flag.
 *
 * - updated some of the outdated comments.
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.1.8.1  1995/12/29  01:07:08  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.1  1995/11/22  21:02:25  cakyol
 *------------------------------------------------------------------
 * $Endlog$
 */

static char *ThisFileName = __FILE__;

#include "lecs.h"
#include "logger.h"
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */
#include "lecs_finder.h"

/*
** we will NOT query the ilmi more than once
** per so many seconds.  We will instead use
** the cached values.
*/
#define THRESHOLD_TIME	0	/* was: (10 * ONESEC) */

/*********************************************************
**
** these structures are private to the lecs finder module 
** and therefore are not exposed outside of this file.
**
**********************************************************/

typedef struct _userRequestBlock
{
    /* 
    ** make it part of a dll list.  Note that
    ** the first word of "links" is shared with
    ** "process_enqueue" as well.  Be careful.
    */
    ListEntry links;

    /* need I say what this bugger is */
    idbtype *swidb;

    /* to match up */
    uint transactionId;
    void *userContext;

    /* get LECS addresses OR get master LECS address */
    FinderRequestType reqType;

    /* call this when done with it, user supplied */
    FinderCallback callback;

    /* maximum limiting requested for returned values */
    uint limit;

} UserRequestBlock;

/*
** this structure represents an LECS address obtained from
** the ATM switch.  Many of these are obtained from the
** switch using the ILMI.  They are used in LECS redundancy
** implementation.  
** 
** They are stored in the list "switchLecsAddressesList" 
** contained in the lane_info_t structure.
**
** If a SETUP has already been sent on this guy's behalf,
** its "callId" will be NON NULL but "connected" will 
** still be FALSE.  When "connected" is set as well, then
** the connexion is assumed to be fully established.
*/
typedef struct switchLecsAddress
{
    /* make it part of a dll list */
    ListEntry links;

    /* address value itself as received from the ILMI */
    hwaddrtype address;

    /* which lane info I belong to */
    struct lane_info_t_ *laneInfo;

    /* callId if I am connecting or connected */
    void *callId;

    /* TRUE if connected completely and successfully */
    boolean connected;

    /* set if currently the master */
    boolean master;

} SwitchLecsAddress;

/*
** the structure that hangs off of ONLY the main 
** subinterface, which is involved in determining 
** the master LECS.  On all other subinterfaces,
** this will not exist.  This contains all the
** crap that we need to find out the master LECS.
** This stuff is heavily used by the LECS finder
** process.
*/
typedef struct _lecsFinderData
{
    /* cross pointer to subif 0's laneInfo */
    lane_info_t *laneInfo;

    /*
    ** This list holds the manually entered LECS
    ** addresses for the LECS redundancy implementation.
    ** These addresses always take priority over the
    ** ILMI obtained ones.
    */
    ListPtr manualSLecsAddressesList;

    /* 
    ** list of all lecs addresses in the entire system.
    ** These are obtained from the ATM switch using ILMI.
    ** If any are manually configured (any in the list above),
    ** instead of obtaining them from the ILMI, they are
    ** just copied into this one.
    */
    ListPtr switchLecsAddressesList;

    /* queued up user requests */
    ListPtr urbList;

    /* at least one user requested the master LECS address */
    boolean masterRequested;

    /*
    ** the LECS address which is currently being tried 
    ** for mastership suitability.  This always points
    ** to an element of the "switchLecsAddressesList"
    ** list.
    */
    SwitchLecsAddress *lecsBeingTested;

    /* last time the stuff was done */
    ulong timeComputed;

    /* safety timer */
    mgd_timer timer;

} LecsFinderData;

/* LECS finder process id */
static int G_lecsFinderPid = NO_PROCESS;

/* queue that holds the user requests */
static watched_queue *finderUserRequestQ = NULL;

/* transaction id generators */
static uint transId = 0;

/* global safety timer */
static mgd_timer finderSafetyTimer;
#define ILMI_HUNG_TIMEOUT	(60*ONESEC)

/* ILMI client id */
static int lecsFinderClientId = ILMI_ERROR;

static void registerWithIlmiServices (void);
static void lecsFinderSignallingCB (void *sigmsg);
static void getAllSwitchLecsAddresses (lane_info_t *laneInfo);
static process lecsFinder (void);

#define NO_PROBLEM	0

/******************************************************************
*******************************************************************
**		helpers & initialization procedures
*******************************************************************
******************************************************************/

/*
** get a client user id from the ILMI.
*/
static boolean finderGetIlmiClientId (void)
{
    /* already registered */
    if (lecsFinderClientId != ILMI_ERROR) return TRUE;

    /* register now */
    lecsFinderClientId = ilmi_register_client("LECS_FINDER");
    if (lecsFinderClientId == ILMI_ERROR) {
	LANE_ERR("lecs finder: finderGetIlmiClientId: "
	    "ilmi_register_client failed");
	return FALSE;
    }
    return TRUE;
}

/*
** initialize master LECS finder process variables 
*/
void lecs_finder_init (void)
{
    lane_lecsFinder_debug = FALSE;
    transId = 0;
    G_lecsFinderPid = NO_PROCESS;
    mgd_timer_init_parent(&finderSafetyTimer, NULL);
    
    /* initialize the queues */
    initWatchedQueue(&finderUserRequestQ, "lecsFinderInputQ");

    /* register with ilmi */
    finderGetIlmiClientId();
}

/*
** parser helper:
** regenerate the global LECS addresses if any
*/
void globalLecsAddresses_regenerate (idbtype *swidb, 
    char *command)
{
    LecsFinderData *lfd;
    SwitchLecsAddress *slecs;

    if (! is_first_swidb(swidb)) return;
    if (! swidb->lane_info) return;
    if (! swidb->lane_info->lecsFinderContext) return;
    lfd = swidb->lane_info->lecsFinderContext;
    if (! lfd->manualSLecsAddressesList) return;
    slecs = List_head(lfd->manualSLecsAddressesList);
    while (slecs) {
	nv_write(TRUE, "%s %Ch", command, &slecs->address);
	slecs = List_next(slecs);
    }
}

/*
** force the next request to actually go and get
** the LECS addresses from the ILMI, do NOT use
** the already cached in values.
*/
static void forceGetNext_nextTime (LecsFinderData *lfd)
{ lfd->timeComputed = - (THRESHOLD_TIME + 100); }

/*
** user request interface.  Take the request, make a struct
** from it and enqueue it on the Finder user request q.
** These are malloc'd here and freed when the state machine 
** runs to completion.  A key id of >= 0 will be returned to
** the user for tracking purposes.  A return value of < 0
** indicates failure.
*/

#define MAX_TRANSID	1000000

uint lecsFinderRequest (idbtype *swidb, FinderRequestType reqType, 
    void *userContext, FinderCallback callback, uint max)
{
    UserRequestBlock *urb;
    char *reqString = "UNKNOWN";

    /* sanity checks */
    if (!swidb || !callback) return INTERNAL_ERROR;
    switch (reqType) {
	case GET_MASTER_LECS_ADDRESS:
	    reqString = "GET_MASTER_LECS_ADDRESS";
	    max = 1;
	    break;
	case GET_ALL_GLOBAL_LECSS:
	    reqString = "GET_ALL_GLOBAL_LECSS";
	    break;
	default:
	    return INTERNAL_ERROR;
    }
    if (!max) return INTERNAL_ERROR;

    /* clip to in built limit */
    if (max > MAX_GLOBAL_LECS_COUNT) max = MAX_GLOBAL_LECS_COUNT;

    /*
    ** if this swidb does NOT yet have lane info, create it.
    ** this also automatically starts the Finder process etc.
    */
    if (! swidb->lane_info) {
	swidb->lane_info = lane_init_idb(swidb);
	if (! swidb->lane_info) return INTERNAL_ERROR;
    }

    /* allocate block */
    urb = malloc_named(sizeof(UserRequestBlock), 
	"UserRequestBlock");
    if (!urb) return INTERNAL_ERROR;

    /*
    ** now start the actual lecs finder
    ** process itself, if not yet started.
    */
    if (G_lecsFinderPid <= NO_PROCESS) {
	G_lecsFinderPid = 
	    process_create(lecsFinder, "LECS Finder",
			NORMAL_STACK, PRIO_LOW);
	if (G_lecsFinderPid <= NO_PROCESS) {
	    LANE_ERR("lecs finder: process_create for "
		"lecsFinder failed");
	    free(urb);
	    return INTERNAL_ERROR;
	}
	LECSF_DBG(NULL, "lecsFinder process started (pid=%d)\n",
	    G_lecsFinderPid);
    }

    /* generate the next transaction id */
    if (transId > MAX_TRANSID)
	transId = 0;
    else
	transId++;

    LECSF_DBG(swidb->namestring,
	"user request %d of type %s queued up", transId, reqString);

    List_clearLinks(&urb->links);
    urb->swidb = swidb;
    urb->reqType = reqType;
    urb->transactionId = transId;
    urb->userContext = userContext;
    urb->callback = callback;
    urb->limit = max;
    process_enqueue(finderUserRequestQ, urb);

    /* key to the user */
    return transId;
}

/*
** we have all the ILMI obtained LECS addresses, now
** let the user know about it.  Copy them into a temp
** storage area, call his callback and free the temp 
** storage.  The user is expected to take his/her own 
** local copy.
*/
static void tellUserAllLecsAddresses (UserRequestBlock *urb, 
    LecsFinderData *lfd, int errcode)
{
    ListPtr lptr = NULL;
    hwaddrtype *all = NULL;
    SwitchLecsAddress *slecs = NULL;
    int count, i, tid;
    void *ctx;
    idbtype *swidb;

    if (! urb) LECS_CRASH();
    swidb = urb->swidb;
    tid = urb->transactionId;
    ctx = urb->userContext;

    if (! lfd) {
	(*urb->callback)(swidb, tid, ctx, INTERNAL_ERROR, NULL);
	return;
    }
    lptr = lfd->switchLecsAddressesList;
    if (! lptr) {
	(*urb->callback)(swidb, tid, ctx, INTERNAL_ERROR, NULL);
	return;
    }

    /*
    ** if a higher level error occured, use that.
    ** if this is zero, proceed.
    */
    if (errcode) {
	(*urb->callback)(swidb, tid, ctx, errcode, NULL);
	return;
    }

    /* get and clip the count */
    count = List_size(lptr);
    if (count > urb->limit) count = urb->limit;

    /* now give them to the user */
    if (count <= 0) {
	(*urb->callback)(swidb, tid, ctx, 0, NULL);
    } else {
	all = malloc_named(count * sizeof(hwaddrtype),
		"Temp Urb Response Block");
	if (! all) {
	    (*urb->callback)(swidb, tid, ctx, 0, NULL);
	} else {
	    i = 0;
	    slecs = List_head(lptr);
	    while (slecs && (i<count)) {
		all [i++] = slecs->address;
		slecs = List_next(slecs);
	    }
	    (*urb->callback)(swidb, tid, ctx, count, all);
	    free(all);
	}
    }
}

/*
** tell the user the master LECS address by calling
** the specified callback routine supplied.
** This is always computed and placed into the
** "masterLecsAddress" field of the lane-info,
** therefore pick it up directly from there.
**
** Note that if LANE is NOT configured on an interface,
** return 0 and NULL for count and master LECS address
** respectively.
*/
static void tellUserMasterLecsAddress (UserRequestBlock *urb,
    int errcode)
{
    lane_info_t *linfo = urb->swidb->lane_info;
    idbtype *swidb = urb->swidb;
    int tid = urb->transactionId;
    void *ctx = urb->userContext;
    int count;

    /* a higher level error occured */
    if (errcode) {
	(*urb->callback)(swidb, tid, ctx, errcode, NULL);
	return;
    } else {
	/* 
	** there is only one master LECS address
	** unless the user has specified even less
	** than that.
	*/
	count = 1;
	if (urb->limit < count) count = urb->limit;
	(*urb->callback)
	    (swidb, tid, ctx, count, &linfo->masterLecsAddress);
    }
}

/*
** go thru the entire chain of stored urb's
** and answer them all one at a time.  If
** "getLecsAddressesOnly" is set, this routine processes
** ONLY the requests that match "GET_ALL_GLOBAL_LECSS",
** and leaves the others (GET_MASTER_LECS_ADDRESS)
** alone.  Those will be taken care of later.
*/
static void answerAllUrbs (LecsFinderData *lfd, int errcode,
    boolean getLecsAddressesOnly)
{
    UserRequestBlock *urb, *this;
    boolean remove;
    char *ifname;

    if (! lfd) return;
    ifname = lfd->laneInfo->swidb->namestring;
    LECSF_DBG(ifname, 
	"answering %s requests now",
	getLecsAddressesOnly ? "only GET_ALL_GLOBAL_LECSS" : "all");
    urb = List_head(lfd->urbList);
    while (urb) {
	this = urb;
	urb = List_next(urb);
	remove = TRUE;
	switch (this->reqType) {
	case GET_ALL_GLOBAL_LECSS:
	    tellUserAllLecsAddresses(this, lfd, errcode);
	    break;

	case GET_MASTER_LECS_ADDRESS:
	    if (getLecsAddressesOnly) 
		remove = FALSE;
	    else
		tellUserMasterLecsAddress(this, errcode);
	    break;

	default:
	    LECS_CRASH();
	}

	/* is it to be removed ? */
	if (remove) {
	    LECSF_DBG(ifname, "responded to user request %d",
		this->transactionId);
	    List_remove(lfd->urbList, this);
	    free(this);
	}
    }
    LECSF_DBG(ifname, 
	"number of remaining requests still to be processed: %d",
	List_size(lfd->urbList));
}

/*
** determine if an LECS address has been manually 
** configured by an operator on this subinterface.
** If it has, return a pointer to it, otherwise
** return NULL.
*/
static MaskedAddress *manuallySpecifiedLecsAddress (idbtype *swidb)
{
    lane_info_t *linfo;
    MaskedAddress *manual;

    if (! swidb) return NULL;
    linfo = swidb->lane_info;
    if (! linfo) return NULL;
    manual = &linfo->lecs_nsap;
    if ((manual->type == STATION_ATMNSAP) && 
	(manual->length == STATIONLEN_ATMNSAP))
	return manual;
    else
	return NULL;
}

/*
** Set the master lecs address on the specified subinterface,
** to the given value.  If no corresponding lane info exists
** on this subinterface, just return.
**
** If a manually configured one is available, it ALWAYS prevails,
** otherwise the specified address is used.  If the computation 
** of the manual address fails, then the "fixed" LECS address is
** used.  If manual address is NOT specified and if the specified 
** address is NULL, the "fixed" LECS address is used.  
**
** Whatever the final calculated value is, it is placed into
** the "masterLecsAddress" field of the corresponding lane info
** structure.
*/
static void setOneMasterLecsAddress (idbtype *swidb,
    hwaddrtype *hwaddr, MaskedAddress *manualKnown)
{
    lane_ilmi_compback_t result;
    lane_info_t *linfo;
    MaskedAddress *manual = manualKnown;
    char *type = "";

    /* if LANE is NOT configured on this subif, just return */
    linfo = swidb->lane_info;
    if (! linfo) return;

    /* manually configured LECS address ALWAYS overrides */
    if (manual == NULL)
	manual = manuallySpecifiedLecsAddress(swidb);
    if (manual) {
	type = "(manual)";
	hwaddr = &linfo->masterLecsAddress;
	result = lane_ilmi_compute_atmsap(swidb, 
		    LANE_ILMI_CONFIG_SERVER, manual, hwaddr);

	/* if computation failed, set it to fixed address */
	if (result != LANE_ILMI_COMPUTE_SUCCESS) {
	    setToFixedLecsAddress(hwaddr);
	    type = "(fixed)";
	}
    } else {
	if (hwaddr == NULL) {
	    hwaddr = &linfo->masterLecsAddress;
	    setToFixedLecsAddress(hwaddr);
	    type = "(fixed)";
	} else {
	    linfo->masterLecsAddress = *hwaddr;
	}
    }
    LECSF_DBG(swidb->namestring,
	"master LECS address set to %Ch %s", hwaddr, type);
}

/*
** set/calculate the master LECS address on ALL of the
** subinterfaces of this interface.  At the end of the 
** transaction, "masterLecsAddress" field of ALL the 
** lane infos are set to the correct/requested value.
*/
static void setAllMasterLecsAddresses (lane_info_t *linfo,
    hwaddrtype *hwaddr)
{
    hwidbtype *hwidb;
    idbtype *swidb;

    hwidb = linfo->swidb->hwptr;
    if (! hwidb) return;
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb)
	setOneMasterLecsAddress(swidb, hwaddr, NULL);
}

/*
** attempt to connect to an LECS thru ATM signalling.
*/
static void connectToLecs (lane_info_t *laneInfo,
    SwitchLecsAddress *slecs, boolean firstAddress)
{
    char *ifname = laneInfo->swidb->namestring;
    LecsFinderData *lfd = laneInfo->lecsFinderContext;

    /* 
    ** either no addresses exists to try, or none 
    ** of the existing ones were successfull.
    ** In both cases, master LECS address is NULL.
    */
    if (slecs == NULL) {
	if (firstAddress)
	    LECSF_DBG(ifname, 
		"no global LECS addresses exist in the system")
	else
	    LECSF_DBG(ifname, 
		"could NOT connect to ANY of the LECSs")
	setAllMasterLecsAddresses(laneInfo, NULL);
	answerAllUrbs(lfd, NO_PROBLEM, FALSE);
	return;
    }

    /* this LECS should NOT already be connected */
    if (slecs->connected) {
	LANE_ERR("lecs finder: connectToLecs: already "
	    "connected to LECS %Ch", &slecs->address);
	answerAllUrbs(lfd, INTERNAL_ERROR, FALSE);
	return;
    }

    /* connect to the desired LECS */
    slecs->callId = connect_to_target(laneInfo->swidb, NULL,
	&slecs->address, LANE_CONFIGURE, lecsFinderSignallingCB);
    if (slecs->callId == NULL) {
	LANE_ERR("lecs finder: connectToLecs: connect_to_target "
	    "failed for %Ch", &slecs->address);
	answerAllUrbs(lfd, INTERNAL_ERROR, FALSE);
	return;
    }

    LECSF_DBG(ifname, "connectING to %Ch", &slecs->address);

    /******** MORE TO DO ********/
    /*
    ** should we set a safety timer here 
    ** in case signalling never came back ?
    */
}

/*
** try connecting to the next lecs if available
*/
static void tryNextLecs (LecsFinderData *lfd)
{
    SwitchLecsAddress *slecs = lfd->lecsBeingTested;
    SwitchLecsAddress *nextOne = NULL;

    if (slecs) nextOne = List_next(slecs);
    lfd->lecsBeingTested = nextOne;
    connectToLecs(lfd->laneInfo, nextOne, FALSE);
}

/*
** start connecting to all the LECSs in turn, 
** starting from the very first one.  Note that,
** if noone requested the master LECS address
** (this implies that every one wanted only the
** global LECS addresses), there is no need to
** proceed to finding the master LECS address.
** Just respond with the global LECS addresses
** and finish up.
*/
static void startConnecting (lane_info_t *laneInfo)
{
    LecsFinderData *lfd;
    
    lfd = laneInfo->lecsFinderContext;

    /* 
    ** if we are here, we MUST have succeeded in getting
    ** all the global LECS addresses one way or another.
    ** However, before going ahead trying to determine
    ** the master, we can answer all the clients who are
    ** interested only in the LECS addresses list.  Those
    ** clients do NOT need to wait until a master LECS is
    ** found, since they dont care about it.
    */
    answerAllUrbs(lfd, NO_PROBLEM, TRUE);

    /* if no user requested the master LECS address, we are done */
    if (! lfd->masterRequested && (List_size(lfd->urbList) <= 0)) 
	return;

    /*
    ** if we are here, at least one user wants the master
    ** LECS address.  So, we start the ball rolling
    */
    lfd->lecsBeingTested = List_head(lfd->switchLecsAddressesList);
    connectToLecs(laneInfo, lfd->lecsBeingTested, TRUE);
}

/*
** release with a debugging message
*/
static void finderRelease (boolean releaseComplete, void *callId,
    sig_api_cause_code cause, idbtype *swidb)
{
    LECSF_DBG(swidb->namestring,
	"sending RELEASE%s for call 0x%x cause %d",
	releaseComplete ? "_COMPLETE" : "",
	callId, cause);
    sendRelease(releaseComplete, callId, cause, swidb);
}

/******************************************************************
*******************************************************************
**		Switch Lecs Address object routines
*******************************************************************
******************************************************************/

/*
** search for a matching switch lecs address in the
** specified list of all global switch lecs addresses.
*/
static SwitchLecsAddress *findSLecsAddress_byAddress (ListPtr list,
    hwaddrtype *hwaddr)
{
    SwitchLecsAddress *slecs;

    slecs = List_head(list);
    while (slecs != NULL) {
	if (byteStreamCompare(hwaddr->addr, slecs->address.addr, 
		STATIONLEN_ATMNSAP) == 0)
		    return slecs;
	slecs = List_next(slecs);
    }
    return NULL;
}

/*
** find a switch lecs address by callId.  This is used 
** when signalling comes back with a response.  Always 
** searches in the list "switchLecsAddressesList" since 
** all the call information is stored in addresses in 
** this list.
*/
static SwitchLecsAddress *findSLecsAddress_byCallId (lane_info_t 
    *laneInfo, void *callId)
{
    SwitchLecsAddress *slecs;
    LecsFinderData *lfd;

    if (! callId) return NULL;
    lfd = laneInfo->lecsFinderContext;
    slecs =  List_head(lfd->switchLecsAddressesList);
    while (slecs) {
	if (slecs->callId == callId) return slecs;
	slecs = List_next(slecs);
    }
    return NULL;
}

/*
** Obliterate a switch lecs address.  Note that the switch
** lecs address MUST have been removed already from the
** relevant list before this is called.  This is just the
** final step in releasing its connection if any exists, 
** and releasing the associated storage.
*/
static void switchLecsAddress_destroy (SwitchLecsAddress *slecs)
{
    LecsFinderData *lfd;
    
    /* if connected, tear it down first */
    if (slecs->connected && slecs->callId)
	finderRelease(FALSE, slecs->callId, ATM_NORMAL_UNSPECIFIED, 
	    slecs->laneInfo->swidb);

    /* if this was the current one, it is no more */
    lfd = slecs->laneInfo->lecsFinderContext;
    if (lfd->lecsBeingTested == slecs)
	lfd->lecsBeingTested = NULL;

    /* free it up */
    LECSF_DBG(slecs->laneInfo->swidb->namestring,
	"LECS %Ch deleted",
	 &slecs->address);
    free(slecs);
}

/*
** this routine is called from the parser, to remove a manually 
** entered switch LECS address from the global list.
*/
void switchLecsAddress_delete (idbtype *swidb, hwaddrtype *hwaddr)
{
    lane_info_t *laneInfo;
    LecsFinderData *lfd;
    SwitchLecsAddress *slecs;
    ListPtr list;

    if (! swidb) {
	printf("%% switchLecsAddress_delete: swidb is NULL\n");
	return;
    }
    if (! is_first_swidb(swidb)) return;
    laneInfo = swidb->lane_info;
    if (! laneInfo) return;
    lfd = laneInfo->lecsFinderContext;
    if (! lfd) return;
    list = lfd->manualSLecsAddressesList;
    if (! list) return;

    /* first find it */
    slecs = findSLecsAddress_byAddress(list, hwaddr);
    if (slecs == NULL) return;

    /* now remove it */
    List_remove(list, slecs);
    switchLecsAddress_destroy(slecs);

    /*
    ** if all manual switch addresses are gone, the next
    ** request SHOULD BE FORCED to go to the ILMI to get
    ** the LECS addresses.
    */
    if (List_size(list) <= 0) forceGetNext_nextTime(lfd);
}

/*
** destroy ALL the switch obtained LECS addresses
*/
static void switchLecsAddress_deleteAll (ListPtr list)
{
    SwitchLecsAddress *slecs;
    
    if (! list) return;
    while ((slecs = List_read(list)))
	switchLecsAddress_destroy(slecs);
}

/*
** add a "new" switch lecs address to the list.  Make sure
** that it is not already included in the list first.  If
** "manual" is set, the address is being added manually
** and therefore has to be added to the "manualSLecsAddressesList"
** list.  Otherwise it is being added thru the ILMI getnext
** response and needs to be added into the 
** "switchLecsAddressesList" list.
*/
void switchLecsAddress_add (idbtype *swidb, boolean manual, 
    hwaddrtype *hwaddr)
{
    lane_info_t *laneInfo;
    SwitchLecsAddress *slecs;
    LecsFinderData *lfd;
    ListPtr list;

    if (! is_first_swidb(swidb)) {
	printf("%% cannot be done on a subinterface\n");
	return;
    }
    laneInfo = lane_init_idb(swidb);
    if (! laneInfo) {
	LANE_ERR("lecs finder: switchLecsAddress_add: "
	    "lane_init_idb failed");
	return;
    }
    lfd = laneInfo->lecsFinderContext;
    if (! lfd) LECS_CRASH();

    /* select which list */
    list = manual ?
	    lfd->manualSLecsAddressesList :
	    lfd->switchLecsAddressesList;

    /* see if the address is already in the list */
    slecs = findSLecsAddress_byAddress(list, hwaddr);
    if (slecs != NULL) {
	if (manual)
	    printf("%% specified LECS address is already in list\n");
	else
	    LECS_WARN("duplicate LECS address %Ch from the ATM switch",
		&slecs->address);
	return;
    }

    /****************** MORE TO DO ******************/
    /*
    ** make sure that the LECS address is also NOT 
    ** an LES address or a client address.  How can
    ** we check this anyway ?  Not quite possible.
    */

    /*
    ** the well known LECS address is special, do
    ** not allow that to be entered.  If it is 
    ** obtained from the switch simply ignore it.
    ** If it is being entered manually, warn against 
    ** it as well.
    */
    if (byteStreamCompare(hwaddr->addr, FIXED_LECS_NSAP, 
	    STATIONLEN_ATMNSAP) == 0) {
	LECSF_DBG(laneInfo->swidb->namestring,
	    "well known LECS address: ignored");
	if (manual)
	    printf("%% the well known LECS address is "
		   "treated specially; ignored\n"); 
	return;
    }

    /* now create and store */
    slecs = malloc_named
	    (sizeof(SwitchLecsAddress), "SwitchLecsAddress");
    if (slecs != NULL) {
	/* set these properly, just in case */
	hwaddr->length = STATIONLEN_ATMNSAP;
	hwaddr->type = STATION_ATMNSAP;

	/* initialize fields */
	slecs->laneInfo = laneInfo;
	slecs->address = *hwaddr;
	slecs->callId = NULL;
	slecs->connected = slecs->master = FALSE;
	List_clearLinks(slecs);

	/* enter into list */
	List_addToEnd(list, slecs);
	LECSF_DBG(laneInfo->swidb->namestring,
	     "LECS %Ch added", hwaddr);
    }
}

/******************************************************************
*******************************************************************
**		   ILMI Get LecsAddresses stuff
*******************************************************************
******************************************************************/

/************* MORE TO DO **************/
/*
** the ls100 works without the "10" in atmf_LECS_addr_oid
** below.  However, the 10 should be there for correct
** operation normally.
*/

/*
** all the oid stuff to do getnexts on switch lecs addresses
*/
static ulong atmf_LECS_addr_oid [] =
{
    atmfSrvcRegATMAddress, 

    /* port number */
    0
};
static OID atmf_LECS_addr =
{
    length:	sizeof(atmf_LECS_addr_oid)/sizeof(ulong),
    oid_ptr:	atmf_LECS_addr_oid
}; 
static VarBind getLecsAddress =
{
    data_length:        0,
    name:               &atmf_LECS_addr,
    name_size:          0,
    value:              { type: NULL_TYPE },
    value_size:         0,
    next_var:           NULL
};

/*
** copies an ATM address from a varbind 
** octet pointer to a hwaddrtype object.
*/
static boolean vblist2hwaddrtype (VarBind *vblist, 
    hwaddrtype *hwaddr)
{
    if (vblist &&
	(vblist->value.type == OCTET_PRIM_TYPE) &&
	(vblist->value.os_value->length == STATIONLEN_ATMNSAP)) {
	hwaddr->type = STATION_ATMNSAP;
	hwaddr->length = STATIONLEN_ATMNSAP;
	bcopy(vblist->value.os_value->octet_ptr, hwaddr->addr, 
	    STATIONLEN_ATMNSAP);
	return TRUE;
    }

    /* invalid LECS address */
    return FALSE;
}

/*
** this routine determines if the error that is returned 
** from the ilmi sbsystem can be considered as end of
** a getnext operation or not.
*/
static boolean endOfGetNext (ilmiClientReq *resp)
{
    return 
	(resp->error == ilmiResponseReceived) &&
        (resp->response == ilmiRespNoSuchNameErr);
}

/*
** see if the object represented in this response
** is or is not an lecs address object identification.
*/
static boolean invalidLecsAddrOid (ilmiClientReq *resp)
{
    int len1, len2;
    ulong *u1, *u2;
    int i;

    /* the ILMI "guarantees" this now, with the dual OID scheme */
    return FALSE;

    len1 = sizeof(atmf_LECS_addr_oid)/sizeof(ulong);
    len2 = resp->vblist->name->length;
    if (len1 >= len2) return TRUE;
    u1 = atmf_LECS_addr_oid;
    u2 = resp->vblist->name->oid_ptr;
    for (i=0; i<len1; i++)
	if (*u1++ != *u2++) return TRUE;
    return FALSE;
}

/*
** ilmi callback handler.
** This code analyzes the responses from the ilmi
** getnext request and enters the lecs addresses
** into the relevant list.  When the end of list
** is reached, it wakes up the lecs finder process.
** So, when the lecs Finder is woken up, the list
** is fully populated and ready to be used.
*/
static void lecsFinderIlmiCB (void *arg)
{
    hwidbtype *hwidb;
    idbtype *swidb;
    ilmiClientReq *resp = (ilmiClientReq*) arg;
    lane_info_t *laneInfo;
    hwaddrtype hwaddr;
    char *ifname;
    LecsFinderData *lfd;
    ulong now;

    if (! resp) LECS_CRASH();
    hwidb = (hwidbtype*) resp->transid;
    if (! hwidb) LECS_CRASH();

    /* do we need this much paranoia ?
    if (hwidb->hw_if_index != resp->intf_id) LECS_CRASH();
    */

    swidb = firstsw_or_null(hwidb);
    if (! swidb) LECS_CRASH();
    ifname = swidb->namestring;
    laneInfo = swidb->lane_info;
    if (! laneInfo) LECS_CRASH();
    lfd = laneInfo->lecsFinderContext;
    if (! lfd) LECS_CRASH();

    LECSF_DBG(ifname,
	"ilmi callback activated; error=%d response=%d",
	resp->error, resp->response);

    /* if we are here, it MUST be a getnext response */
    if (resp->notify_type != ilmiResponseNotify) {
	LANE_ERR("lecs finder: lecsFinderIlmiCB: "
	    "unexpected notify_type %d", resp->notify_type);
	return;
    }

    /* if safety timer has expired, this is too little too late */
    if (! mgd_timer_running(&lfd->timer)) {
	LECSF_DBG(ifname, "ILMI callback is too late");
	return;
    }

    /*
    ** if an error occured, this could be one of two
    ** conditions.  Either the end of the list is reached,
    ** or some other unknown error occured.  In both cases
    ** we need to try and connect to whatever we have.
    */
    if (ILMI_Error(resp)) {
	/* this needs to be stopped */
	mgd_timer_stop(&lfd->timer);

	/*
	** if ended respectably, proceed to actually finding
	** the master LECS, otherwise just flag everyone
	** with the ILMI error.
	*/
	if (endOfGetNext(resp)) {
	    get_timestamp32(&now);
	    lfd->timeComputed = now;
	    
	    /* ok try and connect to whatever we have */
	    startConnecting(laneInfo);
	} else {
	    LECSF_DBG(ifname, "possible ILMI getnext timeout");
	    answerAllUrbs(lfd, ILMI_PROBLEM, FALSE);
	    forceGetNext_nextTime(lfd);
	    return;
	}
    } else {
    /*
    ** no error occured.  Ilmi getnext response came in ok.
    ** If it brought an LECS address, add it to our list,
    ** otherwise, ignore it.  This can happen if getnext
    ** continues to get the rest of the objects from the
    ** mib, past the end of the lecs addresses table.
    */
	/* if NOT an LECS address, ignore it */
	if (invalidLecsAddrOid(resp)) return;

	/* extract the address and enter into the list */
	if (vblist2hwaddrtype(resp->vblist, &hwaddr))
	    switchLecsAddress_add(swidb, FALSE, &hwaddr);
	else
	    LANE_ERR("lecs finder: vblist2hwaddrtype failure");
    }
}

/*
** register for ilmi getNext responses, ilmi prefix additions
** and ilmi prefix deletions with the ilmi subsystem.
*/
static void registerWithIlmiServices (void)
{
    ilmiClientReq req;

    /* are we properly registered with the ilmi ? */
    if (! finderGetIlmiClientId()) return;

    req.client_id = lecsFinderClientId;
    req.notifyCB = lecsFinderIlmiCB;

    /* register for get next responses */
    req.notify_type = ilmiResponseNotify;
    if (ilmi_register_service(&req) != ILMI_OK)
	LANE_ERR("lecsFinder: registerWithIlmiServices: "
	    "ilmi registration for ilmiResponseNotify failed");

    /* we are not interested in these for the time being */
    /**********
    req.notify_type = ilmiAddressAdd;
    if (ilmi_register_service(&req) != ILMI_OK)
	LANE_ERR("lecs finder: ilmi registration for "
	    "ilmiAddressAdd failed");
    req.notify_type = ilmiAddressDel;
    if (ilmi_register_service(&req) != ILMI_OK)
	LANE_ERR("lecs finder: ilmi registration for "
	    "ilmiAddressDel failed");
    ***********/
}

/*
** sends the getNext lecs address request to the ILMI.
** If any manual addresses are configured, then these will
** ALWAYS prevail.  In this case they will simply be
** copied from the manual list to the ilmi obtained list,
** to give the apparent impression that they were indeed
** obtained from the ILMI.  If no manual addresses are
** configured, then ILMI will really be consulted.
*/
static void getAllSwitchLecsAddresses (lane_info_t *laneInfo)
{
    ilmiClientReq req;
    idbtype *swidb = laneInfo->swidb;
    hwidbtype *hwidb = swidb->hwptr;
    LecsFinderData *lfd = laneInfo->lecsFinderContext;
    SwitchLecsAddress *slecs;
    int ifIndex = hwidb->hw_if_index;
    boolean atLeastOneConnected = FALSE;
    ulong now, timeDiff;

    LECSF_DBG(swidb->namestring,
	"finder state machine started");

    /* get the current time */
    get_timestamp32(&now);

    /* adjust for wrapover */
    if (now <= lfd->timeComputed)
	forceGetNext_nextTime(lfd);

    /* get the time elapsed since the last ILMI service */
    timeDiff = now - lfd->timeComputed;

    /*
    ** if manually configured addresses exist in the list 
    ** "manualSLecsAddressesList", take them from there.
    ** Also, force the "ilmi getnext" so that they are
    ** taken from there EVERY time, since this now only
    ** "simulates" the getnext and it actually gets the
    ** addresses from the internal list.
    */
    if (List_size(lfd->manualSLecsAddressesList) > 0) {
	LECSF_DBG(swidb->namestring,
	    "copying global LECS addresses to ILMI list");

	/* delete (and hence disconnect) the old ones */
	switchLecsAddress_deleteAll(lfd->switchLecsAddressesList);

	/* now copy from manual to ilmi list */
	slecs = List_head(lfd->manualSLecsAddressesList);
	while (slecs != NULL)
	{
	    switchLecsAddress_add(swidb, FALSE, &slecs->address);
	    slecs = List_next(slecs);
	}
	forceGetNext_nextTime(lfd);

	/* we have all the LECS addresses, so start testing */
	startConnecting(laneInfo);

    } else if (timeDiff > THRESHOLD_TIME) {
    /*
    ** go out to the ILMI if never been before OR
    ** a certain time has elapsed since we last did it.
    */
	LECSF_DBG(swidb->namestring,
	    "time to perform a getNext on the ILMI");

	/* get rid of all addresses */
	switchLecsAddress_deleteAll(lfd->switchLecsAddressesList);

	/* send the getnext request */
	req.client_id = lecsFinderClientId;
	req.intf_id = ifIndex;
	req.oper_type = ilmiGetNext;
	req.transid = (ulong) hwidb;
	req.vblist = &getLecsAddress;

	/*
	** if this fails, there is nothing that will kick
	** us into life from ILMI, therefore, answer all
	** the requests and finish up.
	*/
	if (ilmi_client_request(&req) != ILMI_OK) {
	    LECSF_DBG(swidb->namestring,
		"ilmi_client_request failed, answering all users");
	    answerAllUrbs(lfd, ILMI_PROBLEM, FALSE);
	    forceGetNext_nextTime(lfd);
	} else {
	    /*
	    ** Set a safety timer here in case ILMI never comes 
	    ** back.  It has been known to happen frequently.
	    ** Otherwise we will go to sleep forever.
	    */
	    mgd_timer_start(&lfd->timer, ILMI_HUNG_TIMEOUT);
	    LECSF_DBG(swidb->namestring,
		"ilmi getNext request for hwidb 0x%x placed",
		hwidb);
	}
    } else {
    /*
    ** no need to ask the ILMI yet, just release all
    ** the already connected circuits and use already
    ** cached addresses.
    */
	slecs = List_head(lfd->switchLecsAddressesList);
	while (slecs) {
	    slecs->master = FALSE;
	    if (slecs->connected) {
		atLeastOneConnected = TRUE;
		finderRelease(FALSE, slecs->callId, 
		    ATM_NORMAL_UNSPECIFIED, laneInfo->swidb);
	    }
	    slecs->connected = FALSE;
	    slecs->callId = NULL;
	    slecs = List_next(slecs);
	}

	/* give signalling a chance to work */
	if (atLeastOneConnected) process_sleep_for(2*ONESEC);

	/* we have all the new LECS addresses, so start testing */
	startConnecting(laneInfo);
    }
}

/******************************************************************
*******************************************************************
**			signalling stuff
*******************************************************************
******************************************************************/

static void lecsFinderSignallingCB (void *sigmsg)
{
    sig_api_msg *msg = (sig_api_msg*) sigmsg;
    char *ifname = msg->idb->namestring;
    SwitchLecsAddress *slecs;
    lane_info_t *laneInfo = msg->idb->lane_info;
    LecsFinderData *lfd = laneInfo->lecsFinderContext;
    void *callId = NULL;
    boolean success = TRUE;
    char *opcodeStr = atmSig_printOpcode(msg->opcode);

    LECSF_DBG(ifname, 
	"lecsFinderSignallingCB activated with opcode %s",
	opcodeStr);
    switch (msg->opcode) {
	/* our setup request just got accepted */
	case ATM_CONNECT:
	    callId = msg->u.connect.call_id; 
	    break;
	case ATM_CONNECT_ACK:
	    callId = msg->u.connect_ack.call_id; 
	    break;
	/* always acknowledge this */
	case ATM_RELEASE:
	    success = FALSE;
	    callId = msg->u.release.call_id;
	    LECSF_DBG(ifname, 
		"acknowledging RELEASE for callId 0x%x", callId);
	    finderRelease(TRUE, callId, 
		ATM_NORMAL_UNSPECIFIED, msg->idb);
	    break;
	case ATM_RELEASE_COMPLETE:
	    success = FALSE;
	    callId = msg->u.release_comp.call_id; 
	    break;
	default:
	    LANE_ERR("lecs finder: lecsFinderSignallingCB: "
		"unexpected opcode %s", opcodeStr);
	    return;
    }

    /* find the corresponding switch lecs address */
    slecs = findSLecsAddress_byCallId(laneInfo, callId);
    if (slecs == NULL) {
	LECSF_DBG(ifname, 
	    "sig message %s is UN-related to any lecs address",
	    opcodeStr);
	if (success)
	    finderRelease(FALSE, callId, ATM_NORMAL_UNSPECIFIED, 
		msg->idb);
	return;
    }

    /* perform the remaining actions */
    if (success) {
	/* connection accepted, this MUST be a master */
	LECSF_DBG(ifname, "connected to %Ch", &slecs->address);
	slecs->connected = TRUE;
	slecs->master = TRUE;
	setAllMasterLecsAddresses(laneInfo, &slecs->address);
	answerAllUrbs(lfd, NO_PROBLEM, FALSE);

	/* once master is found, dont keep this vc around */
	finderRelease(FALSE, callId, ATM_NORMAL_UNSPECIFIED,
	    laneInfo->swidb);
    } else {
	/* connection torn down, try the next one ? */
	slecs->connected = FALSE;
	slecs->callId = NULL;

	/* 
	** if this was the master, we just got our own 
	** intended release acknowledgement, otherwise
	** the LECS we are trying to connect to failed.
	*/
	if (slecs->master) {
	    LECSF_DBG(ifname, 
		"released from master %Ch", &slecs->address)
	} else {
	    LECSF_DBG(ifname, "rejected by %Ch", &slecs->address);
	    tryNextLecs(lfd);
	}
    }
}

/******************************************************************
*******************************************************************
**		Actual Lecs Finder process routines
*******************************************************************
******************************************************************/

/*
** this routine initializes stuff that needs to be
** initialized ONLY on the main subif.  This is all
** to do with the LECS mastership determination.
*/
boolean prepareLecsFinderModule (lane_info_t *laneInfo)
{
    LecsFinderData *lfd;
    idbtype *firstsw = NULL;
    char *ifname;

    if (! laneInfo) LECS_CRASH();
    if (! laneInfo->swidb) LECS_CRASH();

    /* 
    ** if called from a subinterface, translate to major 
    ** interface.  If this is being called, the lane_info 
    ** on the main interface SHOULD exist by now.
    */
    if (! is_first_swidb(laneInfo->swidb)) {
	firstsw = firstsw_or_null(laneInfo->swidb->hwptr);
	if (! firstsw) LECS_CRASH();
	laneInfo = firstsw->lane_info;
	if (! laneInfo) LECS_CRASH();
    }
    ifname = laneInfo->swidb->namestring;

    lfd = laneInfo->lecsFinderContext;
    if (! lfd) {
	lfd = malloc_named(sizeof(LecsFinderData), "LecsFinderData");
	if (! lfd) return FALSE;
	bzero(lfd, sizeof(LecsFinderData));

	/* set up the cross pointers */
	laneInfo->lecsFinderContext = lfd;
	lfd->laneInfo = laneInfo;

	/* init the timestamp stuff */
	forceGetNext_nextTime(lfd);

	/* fill in the rest */
	lfd->masterRequested = FALSE;
	lfd->lecsBeingTested = NULL;
	mgd_timer_init_leaf(&lfd->timer, &finderSafetyTimer,
	    0, lfd, FALSE);

	LECSF_DBG(ifname, "lecsFinderContext created");
    }

    /* create the manual list, if not yet created */
    if (lfd->manualSLecsAddressesList == NULL) {
	lfd->manualSLecsAddressesList = List_alloc();
	if (lfd->manualSLecsAddressesList == NULL) {
	    LANE_ERR("lecs finder: interface %s: List_alloc of "
		"manualSLecsAddressesList failed", ifname);
	    return FALSE;
	}
	LECSF_DBG(ifname, "manualSLecsAddressesList created");
    }

    /* create the dynamic ilmi list, if not yet created */
    if (lfd->switchLecsAddressesList == NULL) {
	lfd->switchLecsAddressesList = List_alloc();
	if (lfd->switchLecsAddressesList == NULL) {
	    LANE_ERR("lecs finder: interface %s: List_alloc of "
		"switchLecsAddressesList failed", ifname);
	    return FALSE;
	}
	LECSF_DBG(ifname, "switchLecsAddressesList created");
    }

    if (lfd->urbList == NULL) {
	lfd->urbList = List_alloc();
	if (lfd->urbList == NULL) {
	    LANE_ERR("lecs finder: interface %s: List_alloc of "
		"urbList failed", ifname);
	    return FALSE;
	}
	LECSF_DBG(ifname, "urbList created");
    }

    return TRUE;
}

/*
** clean out and delete everything about
** the lecs finder module.  This should
** really NOT be called since the only
** time it can possibly be called is when
** subinterface 0 is being deleted, and
** that should NOT happen.
*/
void clearLecsFinderModule (lane_info_t *laneInfo)
{
    LecsFinderData *lfd;
    char *ifname;

    LECS_CRASH();
    if (! laneInfo) return;
    ifname = laneInfo->swidb->namestring;
    lfd = laneInfo->lecsFinderContext;
    if (! lfd) return;

    /* respond to all outstanding user requests */
    answerAllUrbs(lfd, NO_PROBLEM, FALSE);
    List_destroy(lfd->urbList);

    switchLecsAddress_deleteAll(lfd->switchLecsAddressesList);
    List_destroy(lfd->switchLecsAddressesList);
    lfd->switchLecsAddressesList = NULL;

    switchLecsAddress_deleteAll(lfd->manualSLecsAddressesList);
    List_destroy(lfd->manualSLecsAddressesList);
    lfd->manualSLecsAddressesList = NULL;
    mgd_timer_stop(&lfd->timer);

    free(lfd);
    laneInfo->lecsFinderContext = NULL;
    LECSF_DBG(ifname, "cleared module variables");

    process_kill(G_lecsFinderPid);
    LECSF_DBG(ifname, "killed the finder process");
    G_lecsFinderPid = NO_PROCESS;
}

/*
** process the user request block.
*/
static void processUserRequest (UserRequestBlock *urb)
{
    idbtype *swidb;
    hwidbtype *hwidb;
    lane_info_t *laneInfo = NULL;
    LecsFinderData *lfd;
    MaskedAddress *manual = NULL;

    if (!urb) return;
    swidb = urb->swidb;
    if (!swidb) LECS_CRASH();

    /*
    ** we can short circuit here in cases where the user
    ** is requesting the master LECS address AND a manual 
    ** one is already specified on the corresponding
    ** subinterface.  We can calculate that immediately 
    ** and respond, WITHOUT having to go thru the entire 
    ** state machine.
    */
    if (urb->reqType == GET_MASTER_LECS_ADDRESS) {
	manual = manuallySpecifiedLecsAddress(swidb);
	if (manual != NULL) {
	    LECSF_DBG(swidb->namestring,
		"manual LECS address, short circuiting");
	    setOneMasterLecsAddress(swidb, NULL, manual);
	    tellUserMasterLecsAddress(urb, NO_PROBLEM);
	    free(urb);
	    return;
	}
    }

    /*
    ** Cannot short circuit, so proceed on.
    ** Get the Finder descriptor....eventually.
    */

    hwidb = hwidb_or_null(swidb);
    if (!hwidb) LECS_CRASH();
    swidb = firstsw_or_null(hwidb);
    if (!swidb) LECS_CRASH();
    laneInfo = swidb->lane_info;
    if (!laneInfo) LECS_CRASH();
    lfd = laneInfo->lecsFinderContext;
    if (!lfd) LECS_CRASH();

    /* 
    ** if this is the first new request, 
    ** clean out the master requested flag.
    */
    if (List_size(lfd->urbList) <= 0)
	lfd->masterRequested = FALSE;

    /* if the request is getMasterLECSaddress, mark it */
    if (urb->reqType == GET_MASTER_LECS_ADDRESS)
	lfd->masterRequested = TRUE;

    /*
    ** we finally got the lfd, now tack the urb to
    ** the END of the list of all urb's to be handled.
    ** If this happens to be the first urb, then kick the
    ** state machine into action, otherwise it is queued
    ** up to be handled later.
    */
    List_clearLinks(&urb->links);
    List_addToEnd(lfd->urbList, urb);
    if (List_size(lfd->urbList) == 1) {
	lfd->lecsBeingTested = NULL;
	getAllSwitchLecsAddresses(laneInfo);
    }
}

/* define this if stress testing the finder module */
#undef TESTING_FINDER

#ifdef TESTING_FINDER
static process finderStressTest (void);
#endif TESTING_FINDER

/*
** main process that handles all the requests.
*/
static process lecsFinder (void)
{
    UserRequestBlock *urb;
    mgd_timer *expired;
    LecsFinderData *lfd;
    int processed;

#define COUNT	10

    process_wait_on_system_config();
    process_watch_queue(finderUserRequestQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&finderSafetyTimer, ENABLE);

    /*
    ** now that the process is on its way, register with
    ** all the ilmi services that we are interested in.
    */
    registerWithIlmiServices();
    
    /* give everyone some time to settle */
    process_sleep_for(5*ONESEC);

#ifdef TESTING_FINDER
    process_create(finderStressTest, "LECS FINDER TEST", 
	NORMAL_STACK, PRIO_LOW);
#endif TESTING_FINDER

    processed = 0;
    while (TRUE) {

	/* ILMI fucked up again ? */
	while (mgd_timer_expired(&finderSafetyTimer))
	{
	    expired = mgd_timer_first_expired(&finderSafetyTimer);
	    if (! expired) break;
	    mgd_timer_stop(expired);
	    lfd = mgd_timer_context(expired);
	    LANE_ERR("lecs finder: ILMI hung on interface %s",
		lfd->laneInfo->swidb->namestring);
	    answerAllUrbs(lfd, ILMI_PROBLEM, FALSE);
	}

	/* user request came in */
	urb = process_dequeue(finderUserRequestQ);
	if (urb) 
	    processUserRequest(urb);
	if ((processed++ > COUNT) || (!urb))
	{
	    processed = 0;
	    process_wait_for_event();
	}
    }
}

#ifdef TESTING_FINDER

/**********************************************************
***********************************************************
**
**		FINDER STRESS TEST PROCESS
**
***********************************************************
**********************************************************/

/*
** callback handler
*/
static void allLecssCallback (idbtype *swidb, int tid, 
    void *context, int count, hwaddrtype *lecs_addresses)
{ }

/*
** fires a GET_ALL_GLOBAL_LECSS request to the finder for
** every swidb of all the ATM hwidb's, once every 3 seconds.
*/
static process finderStressTest (void)
{
    hwidbtype *hw;
    idbtype *sw;

    /* wait until things settle down */
    process_wait_on_system_config();
    process_sleep_for(20*ONESEC);

    while (TRUE) {
	FOR_ALL_HWIDBS(hw) {
	    /* do it only on ATM interfaces of course */
	    if (is_atm(hw)) {
		FOR_ALL_SWIDBS_ON_HW(hw, sw) {
		    /* perform the request */
		    lecsFinderRequest(sw, GET_ALL_GLOBAL_LECSS, 
			NULL, (FinderCallback) allLecssCallback, 
			1000);
		}
	    }
	}

	/* sleep a bit */
	process_sleep_for(3*ONESEC);
    }
}

#endif TESTING_FINDER

