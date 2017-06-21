/* $Id: at_globals.c,v 3.6.20.11 1996/07/13 11:37:12 fred Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_globals.c,v $
 *------------------------------------------------------------------
 * at_globals.c -- Appletalk globals support
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_globals.c,v $
 * Revision 3.6.20.11  1996/07/13  11:37:12  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.6.20.10  1996/07/10  22:18:11  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.6.20.9  1996/06/29  00:06:41  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.6.20.8  1996/05/17  10:39:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.6.3  1996/05/05  22:46:49  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.6.6.2  1996/04/26  07:30:05  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.6.1  1996/04/03  13:43:26  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.20.7  1996/05/04  00:33:55  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Atalk protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on IRB/CRB is enabled or not.
 *
 * Revision 3.6.20.6  1996/05/02  08:39:55  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.6.20.5  1996/05/01  14:25:56  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between Appletalk network code and ATCP code.
 *
 * Revision 3.6.20.4  1996/04/29  02:41:45  bcole
 * CSCdi55590:  configuring ipx routing  breaks IP ping
 * Branch: California_branch
 *
 * Add 'ipdiag' to the list of subsystem dependencies for subsystems which
 * support the ping command.
 *
 * Revision 3.6.20.3  1996/04/17  13:35:15  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.6.20.2  1996/03/23  01:26:31  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.6.20.1  1996/03/18  18:52:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.3  1996/03/13  01:06:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.12.2  1996/03/07  08:28:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.12.1  1996/02/20  00:17:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/01/25  11:16:24  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.5  1995/12/24  04:56:17  dwong
 * CSCdi44162:  Appletalk doesnt work between ISDN callers
 *
 * Revision 3.4  1995/11/20  22:00:16  kleung
 * CSCdi42895:  Code can cause a segment violation crash
 *
 * Revision 3.3  1995/11/17  08:42:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:29  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:56:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  20:49:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/09/19  23:54:41  kleung
 * CSCdi38726:  AppleTalk RTMP Stub rtmp_stub feature - only RTMP stubs
 * sent.
 *
 * Revision 2.8  1995/09/12  19:34:48  slin
 * CSCdi39096:  Inability to NBP-register AUFS Server (Unix Host).
 * - Add support for routing between local devices
 *
 * Revision 2.7  1995/09/05  22:35:06  kleung
 * CSCdi39766:  Clean up atalk_print_network_header().
 *
 * Revision 2.6  1995/09/01  23:05:12  asastry
 * CSCdi39698:  WFQ: Display info (in "show queue") for non-IP protocols
 * as well
 *
 * Revision 2.5  1995/07/13  06:49:40  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.4  1995/07/09 21:08:14  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Derive the CRB route/bridge behaviour from explicit, per-interface
 * route/bridge indicators rather than inferring it from protocol-
 * specific configuration.
 *
 * Revision 2.3  1995/06/21  02:50:46  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/19  04:03:04  speakman
 * CSCdi35697:  General TB and SRB regression failures
 * Make protocol-specific route/bridge test explicit rather than relying
 * on per-interface rxtypes indicators to provide backward compatible
 * SRB and TB behaviour.
 *
 * Revision 2.1  1995/06/07  20:08:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "ieee.h"
#include "../if/network.h"
#include "../if/priority_private.h"
#include "../os/chunk.h"
#include "atalk_private.h"
#include "at_maint.h"
#include "at_arp.h"
#include "macip_util.h"
#include "atalk_nbp.h"
#include "at_fast.h"
#include "at_static.h"
#include "at_registry.h"
#include "../atalk/at_registry.regc"
#include "at_domain.h"

/*
 * Global AppleTalk Constants
 */

const char atalk_not_enabled[]   = "\n%% AppleTalk not enabled";
const char atalk_not_running[]   = "\n%% AppleTalk not running";
const char atalk_not_on_idb[]    = "\n%% AppleTalk not enabled on this interface";
const char atalk_badaddr[]       = "\n%% Invalid Appletalk address - %s";
const char atalk_badcmd[]        = "\n%% Invalid Appletalk command - %s";


/*
 * Global AppleTalk data
 */
tinybool atalk_enable = TRUE;		/* We are allowed to run */
tinybool atalk_events_logged = FALSE;	/* if atalk_events was set */
                                        /* by apple event-logging */
tinybool atalk_eigrp_logged = FALSE;
tinybool atalk_running;			/* if running Appletalk */
static tinybool atalk_started;		/* if Atalk has ever been started */
tinybool atalk_igrp_running;		/* if running AT/IGRP */
tinybool atalk_igrp_started;		/* if AT/IGRP ever been started */
tinybool atalk_checksumming;		/* if ddp checksums desired */
tinybool atalk_strictRTMP;		/* if strict checks are used on RTMP */
tinybool atalk_testToolCompat;		/* if supporting test tool */
tinybool atalk_ignoreVerifyErrs;	/* if kludge for dumb admins is desired */
tinybool atalk_permitPartialZones;	/* If T, prop. zone if any net permits */
tinybool atalk_routeZones;		/* if enabled then routes need zones */
tinybool atalk_alternateAddrs;		/* if disp. KIP addressing as well */
tinybool atalk_extendedInternet;	/* we have a range > 1 OR >1 zone */
tinybool atalk_obsoleteRouterActive;	/* if we have a phase 1 neighbor */
tinybool atalk_obsoleteRouterDetected;	/* if we had a phase 1 neighbor */
tinybool atalk_DomainRouter;            /* if we are a domain router */
tinybool atalk_DomainRouter_running;    /* If there are active domains */
tinybool atalk_localrouting;	        /* Allow routing between local devices */
tinybool atalk_SMRP_running;            /* if we are running SMRP */

int     atalk_NBPFilters;               /* counter for NBP filters */

int     atalk_MaxPaths;                 /* max # of parallel route paths */

int     atalk_redist_status;		/* if we are re-disting routes */

ulong	atalk_rtr_id;			/* this router's global ID. */
ulong	atalk_rtmp_bw;			/* RTMP bandwidth factor */
ulong	atalk_InRouteZoneWeb;		/* a "show" command is active */
long    atalk_regcode;			/* Private AppleTalk registry number */

chunk_type *atalk_GCContexts;

/*
 * Timers used for various routing protocols.
 */
ulong atalk_rtmpUpdateTime;		/* timers used for routing */
ulong atalk_rtmpValidTime;		/* path valid interval */
ulong atalk_rtmpInvalidTime;		/* interval after which bad */

ulong atalk_rtmp_jitter;		/* jitter percentage */
ulong atalk_rtmpGCInterval;		/* RTMP path GC interval  */
ulong atalk_igrpGCInterval;		/* IGRP path GC interval  */

ulong atalk_zipQueryInterval;		/* time interval between ZIP queries */
ulong atalk_nameLookupInterval;		/* freq. to req. neighbor node names */

ulong atalk_configqueryinterval;
ulong atalk_configquerycount;
ulong atalk_restartinterval;
ulong atalk_stateinterval;

ulong *atalk_stat;			/* maintain traffic stats */

ulong atalk_equalpathFreed;             /* maintain equal-cost paths stats */
ulong atalk_equalpathMallocs;           /* maintain equal-cost paths stats */
 
static atalkidbtype *atalk_deleted_idbs;      /* list of deleted subblocks */
static watched_semaphore *atalk_deleted_idbs_sem; /* mutex semaphore */

static queuetype atalk_idbQ_space;
queuetype *atalk_idbQ;

queuetype atalkechoQ;			/* appletalk ping request queue */

atalkdebugtype *atalk_events;		/* if appletalk events are reported */
atalkdebugtype *atalktrue_debug;
atalkdebugtype *atalkfalse_debug;

pid_t atalk_pid;
pid_t atalk_rtmp_pid;
pid_t atalk_nbp_pid;
pid_t atalk_maint_pid;
pid_t atalk_zip_pid;
pid_t aarp_pid;


/*
 * Appletalk subsystem header
 */

#define ATALK_MAJVERSION 1
#define ATALK_MINVERSION 0
#define ATALK_EDITVERSION  1

SUBSYS_HEADER(atalk, ATALK_MAJVERSION, ATALK_MINVERSION, ATALK_EDITVERSION,
	      atalk_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, ipdiag", "req: eigrp");

/*
 * fair conversation generator
 *
 *	destination net		2
 *	destination node	1
 *	destination socket	1
 *	source net		2
 *	source node		1
 *	source socket		1
 *	message type		1
 */
static
short atalk_fair_queue_flow_id (paktype *pak)
{
    ddptype    *ddp = (ddptype*)pak->network_start;
    int  	sum;

    pak->fair_weight = FAIR_QUEUE_BASE_WEIGHT;
    sum = xnethash((uchar *) &ddp->l.dnet, 9);	/* add the bytes up */
    return(fair_queue_sum(pak, sum));
}

/*
 * atalk_print_network_header
 *
 * Print AppleTalk header.
 */
static void atalk_print_network_header (paktype *pak, ushort **data)
{
    static char ddpn[128];
    atalkidbtype* idb = atalk_getidb(pak->if_input);
    ddptype *ddp = (ddptype*)pak->network_start;
    uchar* hdr;
    
    /* 
     * atalk_pak_initdataptr() has not been called yet if pak is
     * sitting on the input queue.  So can't use utilities that
     * reference pak->atalk_dstfqa or pak->atalk_srcfqa.
     */
    atalk_ddpname(atalk_pak_getpt(pak),ddpn);

    if (pak->linktype == LINK_APPLETALK) {
	/* For extended packets, all data is obtained from the packet */
	printf("\n  source: %a(%s), destination: %a(%s), type %s",
	       atalk_address(GETSHORT(ddp->l.snet), ddp->l.snode), 
	       at_sockname(ddp->l.ssock),
	       atalk_address(GETSHORT(ddp->l.dnet), ddp->l.dnode),
	       at_sockname(ddp->l.dsock),
	       ddpn);
    } else {
	hdr = (uchar *)ddp - LLAP_ENCAPBYTES;
	/*
	 * For short headers, the net # information is obtained from the idb 
	 * and the node numbers are obtained by sniffing back into this
	 * LLAP bytes.
	 */
      idb->atalk_rtmp_stub = FALSE;
	printf("\n  source: %a(%s), destination: %a(%s), type %s",
	       atalk_address(atalkif_MyNet(idb), hdr[LLAP_SRCNODE]), 
	       at_sockname(ddp->s.ssock),
	       atalk_address(atalkif_MyNet(idb), hdr[LLAP_DSTNODE]),
	       at_sockname(ddp->s.dsock),
	       ddpn);
    }
}

/*
 * atalkif_AlwaysInit
 * Initialize transient interface variables for AppleTalk
 */
void
atalkif_AlwaysInit (atalkidbtype *idb, boolean setup_hwidb)
{
    if (setup_hwidb) {
	hwidbtype *hwidb = hwidb_or_null(idb->phys_idb);

        if (hwidb) {
	    hwidb->atalk_fastoddoutadj = 0;
	    hwidb->atalk_fastokay = FALSE;
        }
     }
    idb->atalk_probe = aarp_probe;
    idb->atalk_lookup = aarp_lookup;
    idb->atalk_periodicif = NULL;
    idb->atalk_compatibility = FALSE;
    atalkif_SetIdbPath(idb, NULL);
    idb->atalk_confaddr = 0;
    TIMER_STOP(idb->atalk_timer);
    atalkif_SetAarpTimeout(idb, AARP_ENTRY_TIMEOUT);
    idb->atalk_counter = 0;
    idb->atalk_enctype = ET_NULL;
    idb->aarp_enctype = ET_NULL;
    idb->aarp_tentative = AARP_ADDR_UNKNOWN;
}

/*
 * atalkif_init
 * Initialize interface subblock descriptor for AppleTalk.
 */
void
atalkif_init (atalkidbtype *idb, boolean clrconf, boolean setup_hwidb)
{
    hwidbtype* hwidb = hwidb_or_null(idb->phys_idb);

    if (clrconf) {
	idb->atalknet = 0;
	idb->atalknode = 0;
	idb->atalk_discovery = FALSE;
	idb->atalk_sendroutes = TRUE;
	idb->atalk_client_mode = FALSE;
	idb->atalk_allow_clients = FALSE;
	idb->atalk_glean = FALSE;
	if (setup_hwidb && hwidb) {
	    hwidb->atalk_routecache = fsUnknown;
	    hwidb->atalk_compat_flags = 0;
	}
	idb->atalk_cablestart = 0;
	idb->atalk_cableend = 0;
	idb->atalk_accesslist = 0;
	idb->atalk_route_filter = 0;
	idb->atalk_route_filter_in = 0;
	idb->atalk_gzlfilter = 0;
	idb->atalk_zipreply_filter = 0;
	idb->atalk_enabled = FALSE;
	idb->atalk_rp = 0;
	idb->atalk_rtmp_standby = 0;
	TIMER_STOP(idb->atalk_rtmp_sb_lasthrd);
	ieee_zero(idb->atalk_zone_multicast);
	idb->atalk_ifinit = FALSE;
	idb->atalk_domain = 0;
	idb->atalk_dminfo = NULL;
	idb->atalk_rtmp_split_horizon = TRUE;

    }
    if (setup_hwidb && hwidb) {
	hwidb->atalk_inputerrs = 0;
	hwidb->atalk_outputerrs = 0;
    }
    idb->zonelist = NULL;
    idb->atalk_zonecount = 0;
    idb->atalk_status = opUnknown;
    idb->atalk_nodename = NULL;
    idb->atalk_netname = NULL;
    atalkif_AlwaysInit(idb, setup_hwidb);
}

/*
 * atalkif_idbinit
 * Invoked through registry to initialize idb.
 * used to pass clrconf parameter to atalkif_init
 */
static void
atalkif_idbinit (idbtype *swidb, boolean setup_hwidb)
{
    atalkidbtype *idb = atalk_getidb(swidb);

    if (idb)
	atalkif_init(idb, TRUE, setup_hwidb);
}

void
atalk_init_idb (idbtype *idb)
{
    atalkidbtype *atalkidb;
    swidb_sb_t sbtype;
    
    if (atalk_deleted_idbs) {
	/* Recycle a subblock */
	process_lock_semaphore(atalk_deleted_idbs_sem, -1);
	atalkidb = atalk_deleted_idbs;
	atalk_deleted_idbs = atalkidb->next_deleted;
	process_unlock_semaphore(atalk_deleted_idbs_sem);
	atalkidb->next_deleted = NULL;
    } else {
	atalkidb = malloc(sizeof(atalkidbtype));
	if (atalkidb == NULL)
	    return;
    }

    /*
     * Set up pointers back and forth.
     */
    sbtype = SWIDB_SB_APPLE;
    idb_add_swsb(idb, &sbtype, atalkidb);
    atalkidb->phys_idb = idb;
    if (!atalk_idbQ) {
	atalk_idbQ = &atalk_idbQ_space;
	queue_init(atalk_idbQ, 0);
    }
    if (idb != nullidb)
	enqueue(atalk_idbQ, atalkidb);

    atalkif_init(atalkidb, TRUE, TRUE);
}    

/*
 * atalk_swif_erase
 *
 * A software interface has been removed.
 */
static void
atalk_swif_erase (idbtype *swidb)
{
    atalkidbtype *idb;
    
    /*
     * Does this exist?
     */
    idb = atalk_getidb(swidb);
    if (idb == NULL)
	return;

    /*
     * Move the idb to a "deleted" idbs queue.  It can't be freed,
     * because someone else might have a pointer to it and the code isn't
     * protected against that.  Yes, the unqueue runs the idb queue
     * again, though it updates the tail pointer properly.  Kill the
     * swidb to atalkidb association because the code is protected
     * against it.  The atalkidb to swidb association must be kept
     * because there is also no protection for that.
     */
    unqueue(atalk_idbQ, idb);
    process_lock_semaphore(atalk_deleted_idbs_sem, -1);
    idb->next_deleted = atalk_deleted_idbs;
    atalk_deleted_idbs = idb;
    process_unlock_semaphore(atalk_deleted_idbs_sem);
    idb_delete_swsb(idb->phys_idb, SWIDB_SB_APPLE);
}


/*
 * atalkif_ClearCounters
 * Clear the per-interface counters associated with AppleTalk.
 */
static void
atalkif_ClearCounters (hwidbtype* idb)
{
    idb->atalk_inputerrs = 0;
    idb->atalk_outputerrs = 0;
}

/*
 * atalk_enable_test
 * Return TRUE if we're running AppleTalk
 * Called by SERVICE_PROTO_RUNNING.
 */

static boolean
atalk_enable_test (void)
{
    return (atalk_running);
}

/*
 * atalk_routing_idb
 * Return TRUE iff Atalk packets inbound on this interface should be
 * routed.
 */

static boolean
atalk_routing_idb (idbtype *swidb)
{
    if ((global_crb_enable || global_irb_enable) && 
	swidb->tbridge_on_swidb) {
	/*
	 * For CRB, just return the appletalk route/bridge indicator
	 * if this interface is in a bridge group.  This is
	 * set by the "bridge <n> route appletalk" command.
	 */
	return(swidb->tbridge_route_linktypes & LINK_APPLETALK_RBI);
    } else {
	/*
	 * Otherwise, just verify that appletalk routing is enabled,
	 * and this interface is enabled for appletalk.  This latter
	 * is set by the "appletalk address" command.
	 */
	atalkidbtype *atalkidb = atalk_getidb(swidb);
	
	return(atalk_running && atalkidb && atalkidb->atalk_enabled);
    }
}

/*
 * atalk_bridging_idb
 * Return TRUE iff Atalk packets inbound on this interface should be
 * bridged.
 */

static boolean
atalk_bridging_idb (idbtype *swidb)
{
    if (global_crb_enable) {
	/*
	 * For CRB, just return the complement of the appletalk
	 * route/bridge indicator if this interface is in
	 * a bridge group.  This is cleared by the
	 * "no bridge <n> route appletalk" command (the default).
	 */
	return(swidb->tbridge_on_swidb &&
	       (!(swidb->tbridge_route_linktypes & LINK_APPLETALK_RBI)));
    } else if (global_irb_enable) {
	return(swidb->tbridge_on_swidb &&
	       !(swidb->tbridge_bridge_linktypes & LINK_APPLETALK_RBI));
    } else {
	/*
	 * To support SRB and the archaic transparent bridging
	 * behaviour just check for the absence of atalk routing.
	 */
	return(!atalk_running);
    }
}

/*
 * atalk_rxtypes_update
 *
 * Maintain the route/bridge state of a protocol's rxtypes.
 * Called whenever protocol-specific configuration changes
 * or transparent-bridging-specific configuration changes
 * are made.  Updates a swidb's rxtype_flags to reflect the
 * new configuration.
 *
 * Determines whether the protocol should be routed and sets/
 * clears rxtypes_flags appropriately.  If Concurrent Routing
 * and Bridging is enabled, the decision is based on the
 * protocol configuration for the given swidb.  Otherwise,
 * the decision is based on the protocol configuration for
 * the router as a whole.
 */

void atalk_rxtypes_update (idbtype *swidb)
{
    boolean rte_at, br_at;

    if (global_crb_enable || global_irb_enable) {
	rte_at = atalk_routing_idb(swidb);
	br_at = atalk_bridging_idb(swidb);
    } else {
	rte_at = atalk_running;
	br_at = !rte_at;
    }

    if (rte_at) {
	SET_ROUTING_RXTYPE(swidb, RXTYPE_APPLE);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_APPLE);
    } else {
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_APPLE);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_APPLE);
    }

    if (br_at) {
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_APPLE);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_APPLE);
    } else {
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_APPLE);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_APPLE);
    }
    
    if (swidb->hwptr->setup_bridging)
	(*(swidb->hwptr->setup_bridging))(swidb->hwptr);

    reg_invoke_bridge_setup();	/* sse_bridging_setup_enqueue() */
}

/*
 * at_showproto
 * Interface specific portion of "show protocols" commands
 * Called by SERVICE_SHOWPROTO
 */

static void
at_showproto (idbtype *idb)
{
    atalkidbtype *atalkidb;
    
    if (idb == NULL) {
	if (RUNNING(LINK_APPLETALK))
	    printf("\n  Appletalk routing is enabled");
	return;
    }
    atalkidb = atalk_getidb(idb);
    if (RUNNING_IF(LINK_APPLETALK, idb)) {
	if (!atalkif_HalfRouter(atalkidb))
	    printf("\n  AppleTalk address is %a, zone %s",
		   atalkif_MyAddress(atalkidb),
		   atalkif_PrimaryZoneName(atalkidb));
    }
}

/*
 * atalk_register_thyself
 */
static void atalk_register_thyself (void)
{
    atalk_parser_init();
    atalk_media_registry();
    reg_add_reset_if(atalkif_clear, "atalkif_clear");
    reg_add_route_adjust(atalk_route_adjust, "atalk_route_adjust");
    reg_add_encap_fixup(atalk_network_fixup, "atalk_network_fixup");
    reg_add_bump_cache_version(atalk_cache_increment_version,
		      "atalk_cache_increment_version");
    reg_add_ar_cache_clear(aarp_flush_cache, "aarp_flush_cache");
    reg_add_fast_setup(atalk_fastsetup, "atalk_fastsetup");
    reg_add_sw_idb_init(atalkif_idbinit, "atalkif_idbinit");
    reg_add_clear_counters(atalkif_ClearCounters, "atalkif_ClearCounters");
    reg_add_proto_on_swidb(LINK_APPLETALK, atalk_on_idb,  "atalk_on_idb");
    reg_add_routed_proto_on_swidb(LINK_APPLETALK, atalk_on_idb,
				  "atalk_on_idb");
    reg_add_printf((long) 'A', at_printf_biga, "at_printf_biga");
    reg_add_printf((long) 'a', at_printf_littlea, "at_printf_littlea");
    reg_add_ping_run(LINK_APPLETALK, atalk_ping, "atalk_ping");
    reg_add_ping_parse(atalk_ping_parse, "atalk_ping_parse");
    reg_add_trace_run(LINK_APPLETALK, atalk_trace, "atalk_trace");
    reg_add_proto_running(LINK_APPLETALK,
		      atalk_enable_test, "atalk_enable_test");
    reg_add_proto_bridging_on_swidb(LINK_APPLETALK, atalk_bridging_idb,
				    "atalk_bridging_idb"); 
    reg_add_proto_routing_on_swidb(LINK_APPLETALK, atalk_routing_idb,
				   "atalk_routing_idb"); 
    reg_add_rxtypes_update(atalk_rxtypes_update,"atalk_rxtypes_update");
    reg_add_access_show(show_atalkaccess, "show_atalkaccess");
    reg_add_showproto(at_showproto, "at_showproto");
    reg_add_setup_global(atalk_setup_global, "atalk_setup_global");
    reg_add_setup_int(atalk_setup_int, "atalk_setup_int");
    reg_add_snapshot_start_aging_routes(LINK_APPLETALK, atalk_start_aging,
					"atalk_start_aging");
    reg_add_determine_fair_queue_flow_id(LINK_APPLETALK,
					 atalk_fair_queue_flow_id,
					 "atalk_fair_queue_flow_id");
    reg_add_print_network_header(LINK_APPLETALK, atalk_print_network_header,
				 "atalk_print_network_header");
    reg_add_swif_erase(atalk_swif_erase, "atalk_swif_erase");
    register_snmp_atalk_echo();
}

/* 
 * atalk_init
 * Initialize Appletalk protocol machine
 */
void
atalk_init (subsystype *subsys)
{
    idbtype 	*idb;
    ushort	i;

    atalk_register_thyself();

    /* Set up AppleTalk private function registry */
    atalk_regcode = registry_create(REG_UNKNOWN, SERVICE_ATALK_MAX,
				    "AppleTalk");
    if (atalk_regcode != REG_ILLEGAL)
	create_registry_atalk();

    /* Set up debugging... */
    atalk_debug_init();

    atalk_stat = malloc(sizeof(ulong) * ATALK_MAXTRAFFIC);
    if (atalk_stat == NULL) {
	printf("\n%%No memory to initialize AppleTalk");
	return;
    }

    for (i = 0; i < ATALK_MAXTRAFFIC; i++)
	atalk_stat[i] = 0;
    
    queue_init(&atalkechoQ, 0);		/* initialize echo processing queue */

    atalk_running = FALSE;
    atalk_started = FALSE;
    atalk_igrp_started = FALSE;
    atalk_static  = FALSE;
    atalk_configqueryinterval = ATALK_CONFIG_FREQ;
    atalk_configquerycount = ATALK_CONFIG_RETRIES;
    atalk_zipQueryInterval = ZIP_QUERYTIME;
    atalk_stateinterval = ATALK_STATE_FREQ;
    atalk_restartinterval = ATALK_RESTART_FREQ;
    atalk_rtmpUpdateTime = RTMP_UPDATETIME;
    atalk_rtmpValidTime = RTMP_VALIDTIME;
    atalk_rtmpInvalidTime = RTMP_INVALIDTIME;
    atalk_rtmp_jitter = 0;
    atalk_rtmpGCInterval = ONEMIN;
    atalk_igrpGCInterval = ONEMIN;
    atalk_nameLookupInterval = 0;
    atalk_ignoreVerifyErrs = FALSE;
    atalk_checksumming = TRUE;
    atalk_strictRTMP = TRUE;
    atalk_routeZones = TRUE;
    atalk_permitPartialZones = FALSE;
    atalk_testToolCompat = FALSE;
    atalk_alternateAddrs = FALSE;
    atalk_DomainRouter = FALSE;
    atalk_DomainRouter_running = FALSE;
    atalk_SMRP_running = FALSE;
    atalk_NBPFilters = 0;
    atalk_MaxPaths = 1;
    atalk_equalpathFreed = 0;
    atalk_equalpathMallocs = 0;
    atalk_localrouting = FALSE;

    atalk_extendedInternet = FALSE;
    atalk_obsoleteRouterDetected = FALSE;
    atalk_obsoleteRouterActive = FALSE;
    atalk_redist_status = ATALK_REDIST_UNCONFIGURED;
    atalk_rtmp_bw = RTMP_BANDWIDTH;
    atalk_InRouteZoneWeb = 0;

    atalk_deleted_idbs = NULL;
    atalk_deleted_idbs_sem =
	create_watched_semaphore("atalk_deleted_idbs_sem", 0);
    atalk_init_idb(nullidb);

    FOR_ALL_SWIDBS(idb)
	atalk_rxtypes_update(idb); /* init bridging */
    
    atalk_GCContexts = chunk_create(sizeof(gccontext_t), processIDSize,
				    CHUNK_FLAGS_DYNAMIC, NULL, 0,
				    "AT GC Context Chunk");
    
    /* Initialize maint module first */
    atmaint_Initialize();		/* initialize maintenance module */

    atutil_Initialize();		/* initialize utility module */
    atroute_Initialize();		/* initialize routing module */
    atstatic_Initialize();              /* initialize static module */
    atrtmp_Initialize();		/* initialize rtmp module  */
    atzip_Initialize();			/* initialize zip module */
    atfast_Initialize();		/* initialize fast switching module */
    atsnmp_Initialize();		/* initialize snmp module */
    atnbp_Initialize();			/* initialize NBP module */
    atdomain_Initialize();              /* initialize Domain module */
    aarp_init();
}

/*
 * atalk_start
 * start appletalk processing
 */
void
atalk_start (void)
{
    atalkidbtype* idb;
    
    if (!atalk_running) {
	atalk_running = TRUE;

	FOR_ALL_ATIDBS(idb) {
	    idbtype *swidb;

	    swidb = idb->phys_idb;
	    atalk_rxtypes_update(swidb);
	    if (hwidb_or_null(swidb))
		hwidb_or_null(swidb)->atalk_routecache = fsUnknown;
	    idb->atalk_ifinit = FALSE;
	    atfast_SetupIF(idb);
	    if (atalk_started && idb->atalk_enabled)
		atalkif_RestartPort(idb);
	}
    }

    if (atalk_pid || atalk_rtmp_pid || atalk_nbp_pid || atalk_maint_pid ||
	atalk_zip_pid || aarp_pid) {
	printf("\n%%The AppleTalk processes that were previously running "
	       "have not stopped\nyet.  Please wait a few seconds and "
	       "reissue this command.");
	return;
    }
    atalk_pid = process_create(atalk_input, "AT Input", NORMAL_STACK,
			       PRIO_HIGH);
    atalk_rtmp_pid = process_create(atalk_RtmpRouter, "AT RTMP", NORMAL_STACK,
				    PRIO_NORMAL);
    atalk_nbp_pid = process_create(atalk_nbp, "AT NBP", NORMAL_STACK,
				   PRIO_LOW);
    atalk_maint_pid = process_create(atalk_maint, "AT Maint", LARGE_STACK,
				     PRIO_LOW);
    atalk_zip_pid = process_create(atalk_zip, "AT ZIP", NORMAL_STACK,
				   PRIO_LOW);
    aarp_pid = process_create(aarp_background, "AT ARP", NORMAL_STACK,
			      PRIO_NORMAL);
    if (atalk_started)
	return;

    /*
     * anything beyond here should only be started once
     */
    atalk_started = TRUE;
    aarp_init();
}

/*
 * atalk_stop
 * stop appletalk routing.
 *
 * Because there are many dynamically allocated structures in AppleTalk,
 * we have to schedule the processes to run to deallocate memory.
 */
void
atalk_stop (void)
{
    atalkidbtype* idb;
    hwidbtype* hwidb;

    /*
     * This function will need to check to see that there are no AppleTalk
     * routing protocols running before it enables bridging of AppleTalk
     * and thereby disables all AppleTalk routing.
     */
    atalk_running = FALSE;

    atmaint_FreeAllNeighbors();
    FOR_ALL_ATIDBS(idb) {
	atalk_rxtypes_update(idb->phys_idb); /* allow MAC bridging of AT */
	idb->atalk_ifinit = FALSE;
	atfast_SetupIF(idb);
    }
    FOR_ALL_HWIDBS(hwidb) {
	atalkif_ClearCounters(hwidb);
    }

    /* GC all memory */

    reg_invoke_atalk_shutdown();

    if (atalk_pid)
	process_kill(atalk_pid);
    if (atalk_rtmp_pid)
	process_kill(atalk_rtmp_pid);
    if (atalk_nbp_pid)
	process_kill(atalk_nbp_pid);
    if (atalk_maint_pid)
	process_kill(atalk_maint_pid);
    if (aarp_pid)
	process_kill(aarp_pid);
    if (atalk_zip_pid)
	process_kill(atalk_zip_pid);
}
