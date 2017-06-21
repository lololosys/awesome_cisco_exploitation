/* $Id: at_globals.h,v 3.2.60.4 1996/06/29 00:06:44 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_globals.h,v $
 *------------------------------------------------------------------
 * at_globals.h -- definitions for Appletalk globals 
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_globals.h,v $
 * Revision 3.2.60.4  1996/06/29  00:06:44  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.2.60.3  1996/05/04  00:33:57  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Atalk protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on IRB/CRB is enabled or not.
 *
 * Revision 3.2.60.2  1996/05/02  08:39:58  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.2.60.1  1996/03/23  01:26:34  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:42:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:49:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/12  19:34:53  slin
 * CSCdi39096:  Inability to NBP-register AUFS Server (Unix Host).
 * - Add support for routing between local devices
 *
 * Revision 2.2  1995/07/13  06:49:43  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.1  1995/06/07 20:08:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_GLOBALS_H__
#define __AT_GLOBALS_H__

extern const char atalk_not_enabled[];
extern const char atalk_not_running[];
extern const char atalk_not_on_idb[];
extern const char atalk_badaddr[];
extern const char atalk_badcmd[];

extern ulong	atalk_rtr_id;
extern uchar    apple_old_fddi_broadcast[];
extern uchar	apple_tr_broadcast[];
extern listItem *maint_pendingZoneRequests;

/*
 * AppleTalk debug flags. Note that "atalk_events" is set from the config file,
 * ie, "appletalk event-logging".
 */
extern atalkdebugtype *atalk_events;	/* rtmp, zip, neigbor, idb events */
extern atalkdebugtype *atalkrsp_debug;	/* Responder events */
extern atalkdebugtype *atalkrtmp_debug;	/* routing updates, packets, errors */
extern atalkdebugtype *atalkzip_debug;	/* ZIP changes, packets, errors */
extern atalkdebugtype *appletalk_debug;	/* "debug apple-event" */
extern atalkdebugtype *atalkpkt_debug;	/* reams of wallpaper */
extern atalkdebugtype *atalkerr_debug;	/* all packet, config, internal err */
extern atalkdebugtype *atalkiptalk_debug; /* iptalk noise */
extern atalkdebugtype *aarp_debug;	/* AARP packets, events, errors */
extern atalkdebugtype *atalknbp_debug;
extern atalkdebugtype *atalktrue_debug;
extern atalkdebugtype *atalkfalse_debug;

/*
 * Global configuration flags.
 */
extern tinybool atalk_running;		/* if running Appletalk */
extern tinybool atalk_igrp_running;	/* if AT/IGRP running */
extern tinybool atalk_enable;		/* if Appletalk is allowed to run */
extern tinybool atalk_events_logged;	/* if apple event-logging was used */
extern tinybool atalk_igrp_started;	/* if atalk has ever been started */
extern tinybool atalk_eigrp_logged;

extern tinybool atalk_checksumming;	/* if ddp checksums desired */
extern tinybool atalk_strictRTMP;	/* if strict checks are used on RTMP */
extern tinybool atalk_testToolCompat;	/* if supporting test tool */
extern tinybool atalk_ignoreVerifyErrs;	/* T for kludge for mis-config */

extern tinybool atalk_permitPartialZones; /* If enabled then propogate a zone if any net permits */
extern tinybool atalk_routeZones;	/* if enabled then routes need zones */

extern tinybool atalk_alternateAddrs;	/* if displaying KIP addressing as well */
extern tinybool atalk_extendedInternet;	/* if we have seen range > 1 OR >1 zone */
extern tinybool atalk_obsoleteRouterActive; /* if we have a phase 1 neighbor */
extern tinybool atalk_obsoleteRouterDetected; /* if we had a phase 1 neighbor */

extern tinybool atalk_DomainRouter;     /* if we are a domain router */

extern tinybool atalk_DomainRouter_running; /* If there are active domains */
extern tinybool atalk_localrouting;

extern tinybool atalk_SMRP_running;     /* SMRP routing */

extern int atalk_NBPFilters;            /* counter for nbp filters */

extern int atalk_MaxPaths;              /* max # of equal cost paths */
extern ulong atalk_equalpathFreed;      /* equal-cost paths stats */
extern ulong atalk_equalpathMallocs;    /* equal-cost paths stats */

extern int atalk_redist_status;
extern chunk_type *atalk_GCContexts;

extern long  atalk_iptalk_baseport;
extern ulong atalk_configquerycount;
extern ulong atalk_rtmp_bw;
extern ulong atalk_InRouteZoneWeb;

/*
 * Timer intervals.
 */
extern ulong atalk_rtmpUpdateTime;	/* interval between RTMP updates */
extern ulong atalk_rtmpValidTime;	/* time for RTMP path to go stale*/
extern ulong atalk_rtmpInvalidTime;	/* interval for "    " to go bad */

extern ulong atalk_rtmp_jitter;		/* percentage of jitter */
extern ulong atalk_rtmpGCInterval;	/* interval for RTMP path GC */
extern ulong atalk_igrpGCInterval;	/* interval for IGRP path GC */
extern ulong atalk_zipQueryInterval;	/* interval between ZIP queries */
extern ulong atalk_nameLookupInterval;	/* interval between NBP lookups */

extern ulong atalk_configqueryinterval;	/* interval to query for cable config*/
extern ulong atalk_restartinterval;
extern ulong atalk_stateinterval;

extern ulong aarp_probe_count;		/* count of ARP's to send */
extern ulong aarp_probe_interval;	/* delay between each ARP sent */
extern ulong aarp_request_count;	/* count of ARP's to send */
extern ulong aarp_request_interval;	/* delay between each ARP sent */

/* 
 * Virtual network info 
 */
extern ushort atalk_virtual_net;
extern uchar atalk_virtual_zone[ZONELEN];
extern int atalk_virtual_zonelen;

/*
 * Traffic counting
 */
extern ulong *atalk_stat;		/* maintain traffic stats */

/*
 * various AppleTalk packet queues.
 */
extern queuetype atalkechoQ;		/* appletalk ping request queue */


void 		atalkif_AlwaysInit(atalkidbtype *, boolean);
void 		atalkif_init(atalkidbtype *, boolean, boolean);
void 		atalk_start(void);
void 		atalk_stop(void);
void		atalk_rxtypes_update(idbtype *);
void            atalk_init_idb(idbtype *);

#endif __AT_GLOBALS_H__


