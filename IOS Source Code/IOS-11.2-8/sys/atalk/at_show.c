/* $Id: at_show.c,v 3.12.4.7 1996/07/10 22:18:54 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_show.c,v $
 *------------------------------------------------------------------
 * at_show.c -- Appletalk SHOW command support
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_show.c,v $
 * Revision 3.12.4.7  1996/07/10  22:18:54  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.12.4.6  1996/06/29  00:07:03  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.12.4.5  1996/05/21  09:42:28  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.12.4.4  1996/05/02  08:40:46  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.12.4.3  1996/03/27  18:09:07  slin
 * CSCdi52717:  show apple int brief displays nothing
 * Branch: California_branch
 *
 * - Don't skip interface if AppleTalk not configured
 *
 * Revision 3.12.4.2  1996/03/23  01:27:33  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.12.4.1  1996/03/18  18:53:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.3  1996/03/16  06:26:56  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.12.2  1996/03/07  08:29:28  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.12.1  1996/02/20  00:17:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.12  1996/03/11  22:54:37  kleung
 * CSCdi50981:  Cannot run multiple Cayman tunnels.
 *
 * Revision 3.11  1996/03/04  23:27:16  kleung
 * CSCdi50023:  Update show appletalk commands to handle VIP interfaces.
 *
 * Revision 3.10  1996/02/08  23:08:13  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.9  1996/02/08  20:09:35  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.8  1996/02/07  16:09:56  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/01  05:59:14  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  07:26:12  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1995/12/24  04:56:32  dwong
 * CSCdi44162:  Appletalk doesnt work between ISDN callers
 *
 * Revision 3.4  1995/11/22  21:03:59  slin
 * CSCdi44235:  sh appletalk route <internal arap network #> crashes router
 *
 * Revision 3.3  1995/11/17  08:43:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:57:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  20:50:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/10/10  21:51:11  slin
 * CSCdi41913:  NULL pt. dereference at show_route_detail(0x6040b9a8)+0x4d0
 * - Connected routes have no neighbors so don't dereference p->gwptr
 *
 * Revision 2.7  1995/09/19  23:54:56  kleung
 * CSCdi38726:  AppleTalk RTMP Stub rtmp_stub feature - only RTMP stubs
 * sent.
 *
 * Revision 2.6  1995/09/12  19:35:16  slin
 * CSCdi39096:  Inability to NBP-register AUFS Server (Unix Host).
 * - Add support for routing between local devices
 *
 * Revision 2.5  1995/08/22  00:52:44  slin
 * CSCdi36007:  show apple route detail halts system
 *
 * Revision 2.4  1995/08/09  00:18:23  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.3  1995/07/13  06:50:08  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.2  1995/06/22 22:47:35  wmay
 * CSCdi36246:  Appletalk doesnt fast-switch smds
 *
 * Revision 2.1  1995/06/07  20:09:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "config.h"
#include "plural.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_fast.h"
#include "../atalk/at_access.h"
#include "../atalk/macip_util.h"
#include "../atalk/atalk_nbp.h"
#include "../atalk/macip_db.h"
#include "../h/access.h"
#include "parser.h"
#include "parser_defs_atalk.h"
#include "../if/static_map.h"
#include "../os/free.h"
#include "../atalk/at_static.h"
#include "at_registry.h"
#include "../atalk/at_domain.h"


static const char aarphead[] = {
"Address      Age (min)  Type      Hardware Addr        Encap     Interface"
};

static char* atshow_PeerType (neighborentry *);
static char* atshow_RPenabled (atalkidbtype *);

/*
 * show_PrivateIF
 * Show interface fields for debugging.
 */
static void
show_PrivateIF (atalkidbtype* idb)
{
    int hlen = printf("\n       ");
    hlen += 4;
    printf(" %s: ", idb_get_namestring(idb->phys_idb));
    printf("AT encaps %s, AARP encaps %s (%s)", 
	   encstring(idb->atalk_enctype),
	   encstring(idb->aarp_enctype),
	   atalkif_addrstatemsg(idb));
    printf("\n%*ccable %#A, addr %a, state=%s, conf=%a",
	   hlen-1,' ',
	   atalkif_CableRange(idb),
	   atalkif_MyAddress(idb),
	   atutil_statusString(idb->atalk_status),
	   idb->atalk_confaddr);
    printf("\n%*cifinit %s, compat=%s, cache=%s",
	   hlen-1,' ',
	   (idb->atalk_ifinit)?"true":"false",
	   (idb->atalk_compatibility)?"true":"false",
	   (idb->phys_idb->hwptr->atalk_routecache == fsEnabled)
	   ? "true" : "false");
    printf("\n%*cLink: mcast %e, defGW %a",
	   hlen-1,' ',
	   idb->atalk_zone_multicast,idb->atalk_defaultgw);
    printf("\n%*crtmp %#x, timer %#Ta(%d secs ago), counter %d, zonecnt %d",
	   hlen-1,' ', atalkif_GetIdbPath(idb), idb->atalk_timer, 
	   ELAPSED_TIME(idb->atalk_timer)/ONESEC, idb->atalk_counter,
	   idb->atalk_zonecount);
    printf("\n%*cRP's: %s, admin distance: %d",
	   hlen-1,' ', atshow_RPenabled(idb), idb->atalk_distance);
    if (idb_is_fddi(idb->phys_idb)) {
	char *ena = (idb->phys_idb->hwptr->atalk_compat_flags == AT_COMPAT_FDDI)
	            ? " " : " not ";
	printf("\n%*cFDDI specials: Pre-FDDITalk is%senabled",
	       hlen-1,' ', ena);
    }
    reg_invoke_atalk_show_private_interface(idb);
    
}

/*
 * display_appletalk
 * display appletalk interface information
 */
void
display_appletalk (idbtype *phys_idb)
{
    int len;
    int zprint;
    zipifentry *zif, *zifn;
    ziptabletype *z;
    char *bp;
    atalkidbtype *idb;
    hwidbtype *hwidb;

    idb = atalk_getidb(phys_idb);

    hwidb = phys_idb->hwptr;
    
    printf(if_is_line_proto_is_msg, idb_get_namestring(phys_idb),
	   print_idbstate(phys_idb), interface_up(phys_idb) ? "up" : "down");
    if (idb && idb->atalk_enabled) {
	if (!atalkif_operational(idb))
	    printf("\n  AppleTalk %s, %s",
		   !atalkif_InterfaceUp(idb) ? "node down" : "port disabled",
		   atalkif_opstatusmsg(idb));
	
	if (reg_invoke_atalk_show_interface_special(idb)) {
	    ;				/* do nothing more */
	} else if (atalkif_HalfRouter(idb)) {
	    printf("\n  AppleTalk port is %s", "a half-router");
	} else if (idb->atalk_allow_clients) {
	    printf("\n  AppleTalk port is %s", "in client-mode");
	} else {			/* normal AppleTalk interface */
	    if (atalkif_net_extended(idb)) {
		if (idb->atalk_cableend == ATALK_RANGE_UNKNOWN)
		    printf("%snot known.", "\n  AppleTalk cable range is ");
		else
		    printf("%s%#A", "\n  AppleTalk cable range is ",
			   atalkif_CableRange(idb)); 
	    } 
	    printf("\n  AppleTalk address is %A, %s", atalkif_MyAddress(idb),
		   atalkif_addrstatemsg(idb));
	    z = atalkif_primary_zone(idb);
	    if (idb->atalk_zonecount <= 1) {
		if (z == NULL)
		    printf("%snot set.", "\n  AppleTalk zone is ");
		else
		    printf("%s\042%s\042", "\n  AppleTalk zone is ", z->pname);
	    } else {
		printf("\n  AppleTalk primary zone is \042%s\042",z->pname);
		len = printf("\n  AppleTalk additional zones: ");
		zif = atalkif_firstZone(idb); /* primary zone */
		if (zif != NULL) {
		    zifn = atalkif_nextZone(zif);
		} else {
		    zifn = NULL;
		}
		zprint = 0;
		while (zifn != NULL) {
		    zif = zifn;
		    inline_mem_lock(zif);
		    inline_mem_lock(zif->zone);
		    if ((len+strlen(zif->zone->pname)+2) > 78) {
			len = printf("\n%*c",20,' ');
			zprint = 0;
		    }
		    len += printf("%s\042%s\042", (zprint++ > 0)?", ":"",
				  zif->zone->pname);
		    zifn = atalkif_nextZone(zif);
		    inline_mem_unlock_or_free(zif->zone);
		    inline_mem_unlock_or_free(zif);
		}
	    }
	    if (atalk_igrp_running && (idb->atalk_rp & ATALK_IGRP_ENABLED)) {
		printf("\n  Routing protocols enabled: %s",
		       atshow_RPenabled(idb));
	    }
	}
	if (idb->atalk_confaddr != 0) {
	    printf("\n  AppleTalk port configuration %s %a",
		   ((atalkif_opstatus(idb) == opCableMismatch)?
		    "conflicts with":
		    ((idb->atalk_discovery)?"provided by":"verified by")),
		   idb->atalk_confaddr);
	    bp = atmaint_GetNodeName(idb->atalk_confaddr);
	    if (bp != NULL)
		printf(" (%s)",bp);
	}
	if (hwidb->atalk_inputerrs != 0)
	    printf("\n  AppleTalk discarded %d packet%s due to %s errors",
		   ArgAndPlural(hwidb->atalk_inputerrs,"","s"), "input");
	if (hwidb->atalk_outputerrs != 0)
	    printf("\n  AppleTalk discarded %d packet%s due to %s errors",
		   ArgAndPlural(hwidb->atalk_outputerrs,"","s"), "output");
	if (!idb->atalk_sendroutes)
	    printf("%swill not be generated",
		   "\n  AppleTalk routing messages ");
	else if (idb->atalk_compatibility)
	    printf("%suse compatibility mode", 
		   "\n  AppleTalk routing messages ");
	if (idb->atalk_rtmp_standby != 0) {
	    printf("\n  AppleTalk RTMP standby routing enabled using %d"
		   " second interval", idb->atalk_rtmp_standby);
	    printf("\n  We are currently %sin standby",
		   TIMER_RUNNING(idb->atalk_rtmp_sb_lasthrd) ? "" : "not ");
	}
	if (idb->atalk_discovery)
	    printf("\n  AppleTalk discovery mode is enabled");
	printf("\n  AppleTalk address gleaning is ");
	if (idb->aarp_enctype != ET_NULL)
	    printf("%sabled", idb->atalk_glean ? "en" : "dis");
	else
	    printf("not supported by hardware");
	if (!idb->atalk_rtmp_split_horizon)
	    printf("\n  AppleTalk rtmp split-horizon is disabled");
	printf("\n  AppleTalk route cache is %s",atfast_Reason(idb));
	if (idb->atalk_distance) {
	    printf("\n  Administrative distance is %d (%d hops)",
		   idb->atalk_distance, idb->atalk_distance / atalk_rtmp_bw);
	}
	if (idb->atalk_accesslist != 0)
	    printf("\n  AppleTalk access-group is %d",idb->atalk_accesslist);
	if (idb->atalk_route_filter != 0)
	    printf("\n  AppleTalk %s route filter is %d", "outgoing",
		   idb->atalk_route_filter);
	if (idb->atalk_route_filter_in != 0)
	    printf("\n  AppleTalk %s route filter is %d", "incoming",
		   idb->atalk_route_filter_in);
	if (idb->atalk_gzlfilter == ATFreeTradeZone)
	    printf("\n  AppleTalk \"Free Trade Zone\" security enabled.");
	else if (idb->atalk_gzlfilter != 0)
	    printf("\n  AppleTalk GetZoneList filter is %d",idb->atalk_gzlfilter);
	if (idb->atalk_zipreply_filter != 0)
	    printf("\n  AppleTalk Zip Reply filter is %d",
		   idb->atalk_zipreply_filter);
	if (idb_is_fddi(idb->phys_idb)) {
	    printf("\n  Interface will%sperform pre-FDDITalk compatibility",
		   (hwidb->atalk_compat_flags == AT_COMPAT_FDDI)
		   ? " " : " not ");
	    if (hwidb->atalk_compat_flags == AT_COMPAT_FDDI)
		printf("\n  Interface is in pre-FDDITalk compatibility mode");
	}

	if(idb->atalk_dminfo) {
	    printf("\n  AppleTalk domain is %d (%s)",
		   idb->atalk_dminfo->number, 
		   idb->atalk_dminfo->name);
	}
	if(idb->atalk_rtmp_stub) 
	    printf("\n  AppleTalk RTMP stub mode is enabled");
    } else 
	printf("\n  AppleTalk protocol processing disabled");
}

/*
 * show_appletalk_neighbors
 *
 * Show either a detailed dump of neighbor information (net != 0) or all
 * known neighbors (net == 0).
 */
void
show_appletalk_neighbors (ushort net,uchar node)
{
    neighborentry* r;
    ATAddress regKey;
    char* ali;
    ulong idx;
    ataddrtype addr;

    if (net != 0) {
	addr = atalk_address(net, node);
	r = atmaint_FindMatchingNeighbor(addr, NULL);
	if ((r == NULL) || (r->address != addr)) {
	    printf("No such neighbor, %a", addr);
	    if (r != NULL)
		atmaint_ReleaseNeighbor(r);
	    return;
	}
	printf("  Neighbor %a, %s, uptime %TE, updated %ld sec%s ago",
	       r->address, idb_get_namestring(r->port->phys_idb), r->restarted,
	       ArgAndPlural(ELAPSED_TIME(r->lastheard)/ONESEC,"","s"));
	printf("\n	The neighbor's address is %A",r->address);
	ali = atmaint_GetNodeName(r->address);
	if (ali != NULL)
	    printf(", and is named %s.",ali);
	if (r->oldcisco) {
 	    if (r->obsoleterouter)
 		printf("%sis a pre-8.2 cisco router.",
		       "\n        Neighbor ");
 	    else
 		printf("%soperating in compatibility mode.", 
		       "\n        Neighbor ");
	} else {
	    if (r->obsoleterouter) 
		printf("%sonly supports internets in compatibility mode.",
		       "\n        Neighbor ");
	}
	if (atalk_extendedInternet && r->obsoleterouter)
	    printf("%snot compatible with this internet.",
		   "\n        NOTE: Neighbor is ");
	printf("%s%s reachable as a %s peer", "\n        Neighbor ",
	       r->state == neighborDown ? "was" : "is", atshow_PeerType(r));
	if (r->configerror) 
	    printf("%snot properly configured for cable.",
		   "\n        NOTE: Neighbor is ");
	else if (r->state == neighborOverdue) 
	    printf("%supdate is overdue.", "\n        Neighbor ");
	else if (r->state >= neighborDown)
	    printf("%sdown, has been down for %ld sec%s", 
		   "\n        NOTE: Neighbor is ",
		   ArgAndPlural(ELAPSED_TIME(r->time_went_down)/ONESEC,"","s"));
	printf("\n        We have %s queries for %d net%s via %d packet%s.",
	       "sent", ArgAndPlural(r->qcount,"","s"),
	       ArgAndPlural(r->qpackets,"","s"));
	printf("\n        We %s %d repl%s and %d extended repl%s.", "received",
	       ArgAndPlural(r->zipreplies,"y","ies"),
	       ArgAndPlural(r->zipxreplies,"y","ies"));
	if (r->zipnetcount != 0) {
	    printf("\n        We have %s queries for %d net%s via %d packet%s.",
		   "received", ArgAndPlural(r->zipnetcount,"","s"),
		   ArgAndPlural(r->zipqueries,"","s"));
	    printf("\n        We %s %d repl%s and %d extended repl%s.", "sent",
		   ArgAndPlural(r->qreplies,"y","ies"),
		   ArgAndPlural(r->qxreplies,"y","ies"));
	}
	/*	if (r->zipnotifies != 0) */
	printf("\n        We received %d ZIP notif%s.",
	       ArgAndPlural(r->zipnotifies,"y","ies"));
	/*	if (r->zipobsolete != 0) */
	printf("\n        We received %d obsolete ZIP command%s.",
	       ArgAndPlural(r->zipobsolete,"","s"));
	/*	if (r->zipother != 0) */
	printf("\n        We received %d miscellaneous ZIP command%s.",
	       ArgAndPlural(r->zipother,"","s"));
	/*	if (r->zipunknown != 0) */
	printf("\n        We received %d unrecognized ZIP command%s.",
	       ArgAndPlural(r->zipunknown,"","s"));

	if (r->reachable_by & ATALK_RTMP_ENABLED) {
	    printf("\n        RTMP reachability statistics:");
	    if (   r->port->atalk_rtmp_standby != 0
		&& TIMER_RUNNING(r->port->atalk_rtmp_sb_lasthrd)) {
		printf("\n          RTMP updates being ignored by standby mode");
	    }
	    printf("\n          RTMP connectivity first obtained ");
	    print_dhms(r->rtmp_firstheard);
	    printf(" ago");
	    printf("%slast heard %ld sec%s ago",
		   ArgAndPlural(ELAPSED_TIME(r->rtmp_lastheard)/ONESEC,"","s"));
	    printf("\n          This RTMP neighbor is currently %s",
		   atmaint_NeighborState(r->rtmp_state));
	    printf("\n          We have %s %d routing update%s", "received",
		   ArgAndPlural(r->rtpacketsok+r->rtpacketsbad+r->rtpacketsignored,"","s"));
	    if (r->rtpacketsignored)
		printf("\n          We have %s %d routing update%s", "ignored",
		       ArgAndPlural(r->rtpacketsignored,"","s"));
	    if (r->rtpacketsok == 0)
		printf(", all invalid.");
	    else if (r->rtpacketsbad != 0)
		printf(" but %d %s invalid.",r->rtpacketsbad,
		       Plural(r->rtpacketsbad,"was","were"));
	    else
		printf(".");
	    printf("\n          Of the %d valid update%s, %d entr%s invalid.",
		   ArgAndPlural(r->rtpacketsok,"","s"),
		   ArgAndPlural(r->badroutes,"y was","ies were"));
	    printf("\n          We received %d routing update%s which "
		   "were very late.",
		   ArgAndPlural(r->rtmp_overdues,"","s"));
	    printf("\n          Last update had %d extended and %d "
		   "non-extended routes.",
		   r->rtextendedrts,r->rtnonextendedrts);
	    printf("\n          Last update detail: ");
	    if (r->rtnewrts)
		printf("%d new ",r->rtnewrts);
	    if (r->rtoldrts)
		printf("%d old ",r->rtoldrts);
	    if (r->rtignoredrts)
		printf("%d ignored ",r->rtignoredrts);
	    if (r->rtpoisonedrts)
		printf("%d poison ",r->rtpoisonedrts);
	    if (r->rtbadrts)
		printf("%d bad",r->rtbadrts);
	    if (r->restarts != 0) {
		printf("\n          The RTMP reachability has restarted %d "
		       "time%s in %TE.",
		       ArgAndPlural(r->rtmp_restarts,"","s"), r->started);
	    }
	}
	reg_invoke_atalk_show_neighbor(r);

	regKey = atmaint_FirstRegisteredKey(r,&idx);
	if (regKey.l != 0)
	    printf("\n        Cached service names for %#a:",r->address);
	while (regKey.l != 0) {
	    printf("\n           %s, socket %d",
		   atmaint_GetRegisteredName(regKey,idx), regKey.f.spec);
	    regKey = atmaint_NextRegisteredKey(regKey,&idx);
	}
	atmaint_ReleaseNeighbor(r);
	return;
    }					/* end of detailed neighbor dump */
    
    /*
     * Show all known neighbors (ie, net == 0)
     */
    if ((r = atmaint_GetNextNeighbor(NULL)) == NULL) {
	printf("%% No active neighbors");
	return;
    }
    printf("AppleTalk neighbors:");
    for (;r != NULL;r=atmaint_GetNextNeighbor(r)) {
	char* ali = atmaint_GetNodeName(r->address);
	if (ali == NULL)
	    ali = "";

	if (r->static_entry) {
	    printf("\n  %12a(Static) %s	%s, uptime ",
		   r->address, ali, idb_get_namestring(r->port->phys_idb));
	} else {
	    printf("\n  %12a %s	%s, uptime ",
		   r->address, ali, idb_get_namestring(r->port->phys_idb));
	}
	printf("%TE, %ld sec%s", r->restarted,
	       ArgAndPlural(ELAPSED_TIME(r->lastheard)/ONESEC,"","s"));
	if (CLOCK_DIFF_SIGNED(r->restarted, r->started)) {
	    printf("%shas restarted %u time%s in %TE.",
		   "\n        Neighbor ",
		   ArgAndPlural(r->restarts,"","s"), r->started);
	}
	if (r->obsoleterouter) {
	    printf("%srequires internet to be in compatibility mode.",
		   "\n        Neighbor ");
	    if (atalk_extendedInternet)
		printf("%snot compatible with this internet.",
		       "\n        NOTE: Neighbor is ");
	}
	if (atmaint_NeighborIsReachable(r))
 	   printf("%sis reachable as a %s peer", "\n        Neighbor ",
		   atshow_PeerType(r));
	
	if (r->configerror) 
	    printf("%snot properly configured for cable.",
		   "\n        NOTE: Neighbor is ");
	else if (r->state == neighborOverdue) 
	    printf("%supdate is overdue.", "\n        Neighbor ");
	else if (r->state >= neighborDown)
	    printf("%sdown.", "\n        NOTE: Neighbor is ");
    }
}

/*
 * show_appletalk_zones
 * print appletalk zone table
 */
void
show_appletalk_zones (void)
{
    ziptabletype *z, *z_next;
    int i, len;
    char *buff = string_getnext();
    zipnetentry* zn;

    ++atalk_InRouteZoneWeb;
    automore_header("Name                                Network(s)");
    for (i = 0; i < ZIPHASHLEN; i++) {
	z = zip_table[i];
	while (z) {
	    mem_lock(z);		/* lock this block while printing */
	    if (strlen(z->pname) >= ZONELEN) {
		printf("\n%s", z->pname);
		len = printf("\n%*c",ZONELEN+4, ' ');
	    } else 
		len = printf("\n%s%*c", z->pname,
			     (ZONELEN - strlen(z->pname))+4, ' ');
	    zn = atalk_firstNet(z);
	    while ((zn != NULL) && (zn->net && zn->net->free == NULL)) {
		if ((len + sprintf(buff,"%#A",atalk_CableRange(zn->net)))
		      > 78) {
		    len = printf("\n%*c",ZONELEN + 4, ' ');
		}
		len += printf("%s ",buff);
		zn = atalkzn_nextNet(zn);
	    }
	    z_next = z->next;
	    free(z);			/* unlock, possibly free it */
	    z = z_next;
	}
    }
    printf("\nTotal of %d zone%s", ArgAndPlural(atzip_ZoneCount(),"","s"));
    --atalk_InRouteZoneWeb;
}

void
show_appletalk_zonedetail (uchar* zonename,int zonelen)
{
    int len,hlen,lst;
    zipnetentry* zn;
    char *buff = string_getnext();
    atalkidbtype *idb;
    ziptabletype* zone = atzip_FindZone(zonename,zonelen);

    if (zone == NULL) {
	printf("%% No such zone");
	return;
    }
    ++atalk_InRouteZoneWeb;
    printf("AppleTalk Zone Information for %s:",zone->pname);
    if ((zn = atalk_firstNet(zone))!=NULL) {
	len = printf("\n  Valid for nets: ");
	hlen = len;
	while ((zn != NULL) && (zn->net && zn->net->free == NULL)) {
	    if ((len + sprintf(buff,"%#A",atalk_CableRange(zn->net))) > 78) {
		len = printf("\n%*c",hlen,' ');
	    }
	    len += printf("%s ",buff);
	    zn = atalkzn_nextNet(zn);
	}
    } else {
	len = printf("%sroutes in the current internet.",
		     "\n  Not associated with any "); 
    }
    if (zone->ifcount == 0)
	len = printf("%sinterface.", "\n  Not associated with any ");
    else {
	len = printf("\n  Interface%s (%d use%s): ",Plural(zone->ifcount,"","s"),
		     ArgAndPlural(zone->ifcount,"","s"));
	hlen = len;
	FOR_ALL_ATIDBS(idb) {
	    if (idb->atalk_enabled)
		if (atalkif_findZone(idb, zone) != NULL) {
		    if ((len+strlen(idb_get_namestring(idb->phys_idb))) > 78) {
			len = printf("\n%*c",hlen-1,' ');
		    }
		    len += printf("%s ", idb_get_namestring(idb->phys_idb));
		}
	}
    }
    if (zone->acccount == 0)
	len = printf("%saccess list.", "\n  Not associated with any ");
    else {
	len = printf("\n  Access list%s (%d use%s): ",Plural(zone->acccount,"","s"),
		     ArgAndPlural(zone->acccount,"","s"));
	hlen = len;
	for (lst = MINATALKACC; lst <= MAXATALKACC; lst++) {
	    if (atacc_ACLUsesZone(zone,lst)) {
		if ((len+4) > 78) {
		    len = printf("\n%*c",hlen-1,' ');
		}
		len += printf("%d ",lst);
	    }
	}
    }
    --atalk_InRouteZoneWeb;
}

/*
 * show_appletalk_static
 * print the whole static table 
 */
void 
show_appletalk_static (void)
{

    a_static           *as = a_static_head;
    a_static_zonelist  *z;
    char               *status;
    ataddrtype         nextIR;


    if(as == NULL) {
	printf("\n\n   Static table is empty\n");
	return;
    }

    
    
    automore_enable("\n                AppleTalk     Static      Entries:");
    printf         ("\n                ----------------------------------\n");
    printf("\n   Network      NextIR        Zone                            Status\n");


    while(as) {
	z = as->z_hdr;
	
	nextIR = 0;
	nextIR = as->nextaddr->net;
	nextIR <<= 8;
	nextIR |= as->nextaddr->node;

	if(as->end == 0)
	    printf("\n     %5d   ",as->start);
	else
	    printf("\n %5d- %5d",as->start,as->end);
	if(as->floating)
	    status = ((as->flags == AT_STATIC_ACTIVE) ? "A(Floating)" : "N/A(Floating)");
	else
	    status = ((as->flags == AT_STATIC_ACTIVE) ? "A" : "N/A");	
	printf("   %12a  %33s %s",
	       nextIR, z->zonename, status);

	/* Now print all the zones for this route */
	z = z->next;
	while(z) {
	printf("\n");
	printf("                              %s", z->zonename);
	    z = z->next;
	}
	printf("\n");
	as = as->next;
    }

}


/*
 * show_appletalk_traffic
 * print appletalk traffic statistics 
 */
void
show_appletalk_traffic (void)
{
    automore_enable("AppleTalk statistics:");
    printf("\n  Rcvd:  %d total, %d checksum errors, %d bad hop count",
	   atalk_stat[ATALK_INPUT],
	   atalk_stat[ATALK_CKSUMERR], atalk_stat[ATALK_HOPCOUNT]);
    printf("\n	 %d local destination, %d access denied",
	   atalk_stat[ATALK_LOCALDEST],
	   atalk_stat[ATALK_NOACCESS]);
    printf("\n	 %d for MacIP, %d bad MacIP, %d no client",
	   atalk_stat[MACIP_DDP_IN],
	   atalk_stat[MACIP_DDP_BAD_DDP_TYPE],
	   atalk_stat[MACIP_DDP_NO_CLIENT_SERVICE]);
    printf("\n	 %d port disabled, %d no listener",
	   atalk_stat[ATALK_NOT_OPERATIONAL],atalk_stat[ATALK_NO_HANDLER]);
    printf("\n	 %d ignored, %d martians",
	   atalk_stat[ATALK_IGNORED],
	   atalk_stat[ATALK_MARTIAN]);
    printf("\n  Bcast: %d received, %d sent",
	   atalk_stat[ATALK_BCASTIN], atalk_stat[ATALK_BCASTOUT]);
    printf("\n  Sent:  %d generated, %d forwarded, %d fast forwarded, %d loopback",
	    atalk_stat[ATALK_OUTPUT], atalk_stat[ATALK_FORWARDED],
	    atalk_stat[ATALK_FASTFORWARD] + atalk_stat[ATALK_XFASTFORWARD],
	    atalk_stat[ATALK_LOOPBACK]);
    printf("\n	 %d forwarded from MacIP, %d MacIP failures", 
	   atalk_stat[MACIP_IP_DDP_OUT],
	   atalk_stat[MACIP_IP_DDP_SEND_FAIL]);
    printf("\n	 %d encapsulation failed, %d no route, %d no source",
	   atalk_stat[ATALK_ENCAPSFAILED], atalk_stat[ATALK_NOROUTE],
	   atalk_stat[ATALK_NOSOURCE]);
    printf("\n  DDP:   %d long, %d short, %d macip, %d bad size",
	   atalk_stat[ATALK_DDP_LONG], atalk_stat[ATALK_DDP_SHORT],
	   atalk_stat[MACIP_DDP_IN],
	   atalk_stat[ATALK_DDP_WRONGSIZE]);
    printf("\n  NBP:   %d received, %d invalid, %d proxies", 
	   atalk_stat[ATALK_NBP_IN], 
	   atalk_stat[ATALK_NBP_BAD],
	   atalk_stat[ATALK_NBP_PROXY]);
    printf("\n	 %d replies sent, %d forwards, %d lookups, %d failures",
	   atalk_stat[ATALK_NBP_OUT],
	   atalk_stat[ATALK_NBP_FWD], atalk_stat[ATALK_NBP_LKUP],
	   atalk_stat[ATALK_NBP_FAILED]);
    printf("\n  RTMP:  %d received, %d requests, %d invalid, %d ignored", 
	   atalk_stat[ATALK_RTMP_IN],
	   atalk_stat[ATALK_RTMP_REQUEST], atalk_stat[ATALK_RTMP_BAD],
	   atalk_stat[ATALK_RTMP_IGNORED]);
    printf("\n	 %d sent, %d replies",
	   atalk_stat[ATALK_RTMP_OUT],
	   0);
    printf("\n  ATP:   %d received", atalk_stat[ATALK_ATP]);
#ifdef ATALK_SNMP_ENABLE
    printf("\n  SNMP:  %d received, %d sent", atalk_stat[ATALK_SNMP_IN],
	   atalk_stat[ATALK_SNMP_OUT]);
#endif
    printf("\n  ZIP:   %d received, %d sent, %d netinfo",
	   atalk_stat[ATALK_ZIP_IN], atalk_stat[ATALK_ZIP_OUT],
	   atalk_stat[ATALK_ZIP_NETINFO]);
    printf("\n  Echo:  %d received, %d discarded, %d illegal", 
	   atalk_stat[ATALK_ECHO],
	   atalk_stat[ATALK_ECHO_DROPPED],
	   atalk_stat[ATALK_ECHO_ILL]);
    printf("\n	 %d generated, %d replies sent",
	   atalk_stat[ATALK_ECHO_SENT],
	   atalk_stat[ATALK_ECHO_REPLIES]);
    printf("\n  Responder:  %d received, %d illegal, %d unknown", 
	   atalk_stat[AT_RESPONDER_IN],
	   atalk_stat[AT_RESPONDER_BAD_ATP],
	   atalk_stat[AT_RESPONDER_BAD_OPCODE]);
    printf("\n	 %d replies sent, %d failures",
	   atalk_stat[AT_RESPONDER_OUT],
	   atalk_stat[AT_RESPONDER_SEND_FAIL]+
	   atalk_stat[AT_RESPONDER_CREATE_ERROR]);
    printf("\n  AARP:  %d requests, %d replies, %d probes",
	   atalk_stat[ATALK_ARP_REQUEST], atalk_stat[ATALK_ARP_REPLY],
	   atalk_stat[ATALK_ARP_PROBE]);
    printf("\n	 %d martians, %d bad encapsulation, %d unknown",
	   atalk_stat[ATALK_ARP_BOGON], atalk_stat[ATALK_ARP_BADENCS],
	   atalk_stat[ATALK_ARP_UNKNOWN]);
    printf("\n	 %d sent, %d failures, %d delays, %d drops",
	   atalk_stat[ATALK_ARP_SENT], atalk_stat[ATALK_ARP_FAILED],
	   atalk_stat[ATALK_ARP_DELAYS], atalk_stat[ATALK_ARP_DROPS]);
    printf("\n  Lost: %d no buffers",
	   atalk_stat[ATALK_NO_BUFFERS]);
    printf("\n  Unknown: %d packets", atalk_stat[ATALK_UNKNOWN]);
    printf("\n  Discarded: %d wrong encapsulation, %d bad SNAP discriminator",
	   atalk_stat[ATALK_ROUTER_BADVERS], atalk_stat[ATALK_BAD_ISO2OUI]);
    reg_invoke_atalk_show_traffic();
    automore_disable();
}

void
show_appletalk_globals (void)
{
    printf("AppleTalk global information:");
    if(atalk_DomainRouter_running)
	printf("\n  The router is a domain router.");
    if (atalk_extendedInternet)
	printf("\n  Internet is incompatible with older, AT Phase1, routers.");
    else
	printf("\n  Internet is compatible with older, AT Phase1, routers.");
    if (atalk_obsoleteRouterActive)
	printf("\n  Some neighbor(s) require the internet to use "
	       "compatibility mode.");
    else if (atalk_obsoleteRouterDetected)
	printf("\n  Some neighbor(s) needed the internet to use "
	       "compatibility mode.");
    printf("\n  There %s %d route%s in the internet.",
	   Plural(atroute_RouteCount(),"is","are"),
	   ArgAndPlural(atroute_RouteCount(),"","s"));
    printf("\n  There %s %d zone%s defined.",
	   Plural(atzip_ZoneCount(),"is","are"),
	   ArgAndPlural(atzip_ZoneCount(),"","s"));
    if (at_debug(atalk_events, ATALK_NULL_MASK))
	printf("\n  All significant events will be logged.");
    else
	printf("\n  Logging of significant AppleTalk events is disabled.");
    printf("\n  ZIP resends queries every %d seconds.",
	   atalk_zipQueryInterval/ONESEC);
    if (atalk_rtmp_jitter) {
	printf("\n  RTMP updates are sent every %d seconds with a jitter of"
	       " %d percent.", atalk_rtmpUpdateTime/ONESEC, atalk_rtmp_jitter);
    } else {
	printf("\n  RTMP updates are sent every %d seconds.",
	       atalk_rtmpUpdateTime/ONESEC);
    }
    printf("\n  RTMP entries are considered BAD after %d seconds.",
 	   atalk_rtmpValidTime/ONESEC);
    printf("\n  RTMP entries are discarded after %d seconds.",
 	   atalk_rtmpInvalidTime/ONESEC);
    printf("\n  AARP probe retransmit count: %d, interval: %d msec.",
	   aarp_probe_count, aarp_probe_interval);
    printf("\n  AARP request retransmit count: %d, interval: %d msec.",
	   aarp_request_count, aarp_request_interval);
    printf("\n  DDP datagrams will%s be checksummed.",
	   (atalk_checksumming)?"":" not");
    printf("\n  RTMP datagrams will%s be strictly checked.",
	   (atalk_strictRTMP)?"":" not");
    printf("\n  RTMP routes may%s be propagated without zones.",
	   (!atalk_routeZones)?"":" not");
    printf("\n  Routes will %sbe distributed between routing protocols.",
	   atalk_redist_status & ATALK_REDIST_ENABLED ? "" : "not ");
    printf("\n  Routing between local devices on an interface will"
	   "%s be performed.", atalk_localrouting ? "" : " not");
    
    /* Display all other subsystem's globals here. */
    reg_invoke_atalk_show_globals();

    printf("\n  Alternate node address format will%s be displayed.",
	   (atalk_alternateAddrs)?"":" not");
    if (!atalk_permitPartialZones)
	printf("\n  Access control of any networks of a zone hides the zone.");
    else
	printf("\n  All networks of a zone must be denied to hide the zone.");
    if (atalk_ignoreVerifyErrs)
 	printf("\n  Configuration verification errors at startup are ignored.");
    if (atalk_testToolCompat)
	printf("\n  TestTool compatibility mode is enabled.");
    if (atalk_nameLookupInterval != 0) {
	printf("\n  Names of local servers will be queried every %d seconds.",
	       atalk_nameLookupInterval/ONESEC);
	atmaint_ShowLookupList();
    }
    if (atalk_MaxPaths > 1)
        printf("\n  Up to %d parallel paths allowed.", atalk_MaxPaths);
}

static void
atalk_show_pvt_globals (void)
{
    printf("\n  Fast switching breakdown: %d within controller, "
	   "%d cross controller",
	   atalk_stat[ATALK_FASTFORWARD], atalk_stat[ATALK_XFASTFORWARD]);
    printf("\n  Debugging statistics: %d bugs, %d messages",
	   atalk_stat[ATALK_BUGS], atalk_stat[ATALK_DEBUGMSGS]);
    atutil_StatusReport();
    atmaint_StatusReport();
    atzip_StatusReport();
    atroute_StatusReport();
    aarp_StatusReport();
}

static void 
atalk_show_pvt_interfaces (void)
{
    atalkidbtype *idb;
    
    printf("\n  AT Interface fields:");
    FOR_ALL_ATIDBS(idb) {
	if (idb->atalk_enabled) {
	    show_PrivateIF(idb);
	}
    }
}

static void
atalk_show_pvt_old (void)
{
    printf("\n  Fast switching breakdown: %d within controller, "
	   "%d cross controller",
	   atalk_stat[ATALK_FASTFORWARD], atalk_stat[ATALK_XFASTFORWARD]);
    printf("\n  Debugging statistics: %d bugs, %d messages",
	   atalk_stat[ATALK_BUGS], atalk_stat[ATALK_DEBUGMSGS]);
    printf("\n  Addr Types: %d NotMyBA, %d BadSA, %d BadDA, %d IgnDA, %d ToDefGW",
	   atalk_stat[ATALK_BCAST_NOTMINE],
	   atalk_stat[ATALK_SADDR_INVALID],
	   atalk_stat[ATALK_DADDR_INVALID],
	   atalk_stat[ATALK_DADDR_IGNORED],
	   atalk_stat[ATALK_USED_DEFGW]);
    atutil_StatusReport();
    atmaint_StatusReport();
    atzip_StatusReport();
    atroute_StatusReport();
    atalk_show_pvt_interfaces();
}

/*
 * show_route_detail -- show detailed & gory information about a 
 *			route or the routing tree.
 *
 * If 'net' is 0, we simply dump the entire routing tree in R/B form.
 */
static void
show_route_detail (int net)
{
    zipnetentry* zn;
    int zcount, i;
    int len, hlen;
    atalk_rdb_t *r;
    atalk_pdb_t *p;
    dqueue_t	*dq;
    rbTree	*tree;
    boolean	best_path;
    uint DisplayBufSize;
    uint *DisplayBufSizePtr;
    char *displaybuf;
    char **displaybufptr;
    char *workbuf;
    short count;

    displaybuf = malloc(WIDTH_DEFAULT);
    if (displaybuf == NULL)
	return;
    DisplayBufSize = WIDTH_DEFAULT;
    displaybufptr = &displaybuf;
    DisplayBufSizePtr = &DisplayBufSize;
    
    tree = atroute_GetTree();
    ++atalk_InRouteZoneWeb;
    r = atroute_FindRoute(tree,net, TRUE);
    if (r == NULL) {
	printf("%% No net %d",net);
	--atalk_InRouteZoneWeb;
	return;
    }
    zcount = atalk_CountAndFilterZones(r, 0); /* No filtering done. */
    safe_sprintf(displaybufptr, DisplayBufSizePtr, "\n  Route installed ");
    workbuf = string_getnext();
    sprint_dhms(workbuf, r->started);
    strcat_with_realloc(displaybufptr, DisplayBufSizePtr, workbuf);
    if (!atalk_net_connected(r)	&& (r->rpath.gwptr != NULL)) {
	safe_sprintf(displaybufptr, DisplayBufSizePtr,
		     ", updated %d sec%s ago",
		     ArgAndPlural(ELAPSED_TIME(r->timer)/ONESEC,"","s"));
	if (r->rpath.metric.metric == METRIC_INACCESSIBLE)
	    safe_sprintf(displaybufptr, DisplayBufSizePtr,
			 "\n  Route poisoned, to notify neighbors, by %#a",
			 r->rpath.gwptr->address);
	else
	    safe_sprintf(displaybufptr, DisplayBufSizePtr,
			 "\n  Next hop: %#a, %d hop%s away",
			 r->rpath.gwptr->address,
			 ArgAndPlural(atrtmp_GetRouteMetric(r),"","s"));
    } else if (r->rpath.metric.metric == METRIC_INACCESSIBLE) {
	safe_sprintf(displaybufptr, DisplayBufSizePtr,
		     "\n  Route is poisoned to notify neighbors");
    } else if (!atalk_net_proxy(r)) {
	safe_sprintf(displaybufptr, DisplayBufSizePtr,
		     ", directly connected via %s",
		     idb_get_namestring(r->rpath.idb->phys_idb));
    }
    if (r->holddown > 0) {
	safe_sprintf(displaybufptr, DisplayBufSizePtr,
		     ", hcount at %d", r->holddown);
    }
    if (r->zoneupdate) {
	safe_sprintf(displaybufptr, DisplayBufSizePtr,
		     "\n  Zone update in progress");
	if (r->zonecnt != 0)
	    safe_sprintf(displaybufptr, DisplayBufSizePtr,
			 "\n  Partial update received from %#a,"
			 " %d zone%s outstanding",
			 r->zoneaddr,ArgAndPlural(r->zonecnt,"","s"));
    } else if ((r->zoneaddr != 0) && (zcount > 0)) {
	safe_sprintf(displaybufptr, DisplayBufSizePtr,
		     "\n  Zone list provided by %#a",r->zoneaddr);
    }
    if (r->zoneretries != 0) {
	safe_sprintf(displaybufptr, DisplayBufSizePtr,
		     "\n  Zone list request retries: %d", r->zoneretries);
    }
    if (r->changed) {
	safe_sprintf(displaybufptr, DisplayBufSizePtr,
		     "\n  Route has been updated since last routing update "
		     "was sent");
    }
    if (zcount > 0) {
	len = safe_sprintf(displaybufptr, DisplayBufSizePtr,
			   "\n  Valid zones: ");
	hlen = len;
	for (i = 0, zn = atalk_firstZone(r); zn != NULL && i < zcount;
	     i++, zn = atalkzn_nextZone(zn)) {
	    char* pn = zn->zone->pname;
	    if ((len+strlen(pn)+3) > 78) {
		len = safe_sprintf(displaybufptr, DisplayBufSizePtr,
				   "\n");
		len += safe_sprintf(displaybufptr, DisplayBufSizePtr,
				    "%*c",hlen,' ');
	    }
	    len += safe_sprintf(displaybufptr, DisplayBufSizePtr,
				"\"%s\"%c", pn,((i + 1) < zcount)?',':' ');
	}
    }
    count = atroute_GetEqualPathCount(r->best_path);
    safe_sprintf(displaybufptr, DisplayBufSizePtr,
		 "\n  There %s %d path%s for this route",
		 ((r->pathcount+count) > 1 || r->pathcount == 0)?"are":"is",
		 ArgAndPlural(r->pathcount+count,"","s"));
    for (dq = r->rpath.dqLink.flink; dq != &r->rpath.dqLink; dq = dq->flink) {
	p = (atalk_pdb_t*)dq->parent;
	best_path = p == p->rdb->best_path;

	safe_sprintf(displaybufptr, DisplayBufSizePtr,
		     "\n%c %s path, ", (best_path) ? '*' : ' ',
		     atroute_rp_type(p->type));
	if (!(p->type == AT_PATH_CONNECTED || p->type == AT_PATH_PROXY))
	    safe_sprintf(displaybufptr, DisplayBufSizePtr,
			 "to neighbor %#a, ", p->gwptr->address);
	safe_sprintf(displaybufptr, DisplayBufSizePtr,
		     "installed ");
	sprint_dhms(workbuf, p->timer);
	strcat_with_realloc(displaybufptr, DisplayBufSizePtr, workbuf);
	if (p->idb)
	    safe_sprintf(displaybufptr, DisplayBufSizePtr,
			 " via %s", idb_get_namestring(p->idb->phys_idb));
	safe_sprintf(displaybufptr, DisplayBufSizePtr,
		     "\n   Composite metric is %u, %u hop%s",
		     p->metric.metric,
		     ArgAndPlural(p->metric.u.rtmp.hops,"","s"));
	if (p->type == AT_PATH_IGRP) {
	    safe_sprintf(displaybufptr, DisplayBufSizePtr,
			 "\n    Delay is %u microseconds, "
			 "minimum bandwidth is %u Kbit",
			 p->metric.u.igrp.delay,
			 p->metric.u.igrp.bandwidth);
	    safe_sprintf(displaybufptr, DisplayBufSizePtr,
			 "\n    Reliability %u/255, minimum MTU %u bytes",
			 p->metric.u.igrp.reliability, p->metric.u.igrp.mtu);
	    safe_sprintf(displaybufptr, DisplayBufSizePtr,
			 "\n    Loading %u/255, %u EIGRP hop%s",
			 p->metric.u.igrp.load,
			 ArgAndPlural(p->metric.u.igrp.hops,"","s"));
	    if (p->metric.u.igrp.external) {
		safe_sprintf(displaybufptr, DisplayBufSizePtr,
			     "\n    Path is derived from %s from %d",
			     atroute_rp_type(p->metric.u.igrp.ext_proto_id), 
			     p->metric.u.igrp.ext_source);
		safe_sprintf(displaybufptr, DisplayBufSizePtr,
			     "\n    Path's external metric is %u hop%s",
			     ArgAndPlural(p->metric.u.igrp.ext_hops,"","s"));
	    }
	}
    }
    
    ReleaseRoute(r);
    --atalk_InRouteZoneWeb;
    printf("%s", displaybuf);
    free(displaybuf);
}

void show_appletalk (parseinfo *csb)
{
    uchar	zonename[ZONELEN];
    int		zonelen;
    ushort	net;
    uchar	node;
    uchar	pvt_header[80];

    if (!atalk_running) {
#ifdef ERRMSG_FOR_ERRORS
	errmsg(AT_ERR_NOTRUNNING);
#else
	printf(atalk_not_running);
#endif
	return;
    }

    automore_enable(NULL);
    switch (csb->which) {
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
      case SHOW_APPLE_ARP:
	net = node = 0;
	if (GETOBJ(int,1)  == TRUE) {	/* there's an optional address */
	    net  = GETOBJ(paddr,1)->apple_addr.net;
	    node = GETOBJ(paddr,1)->apple_addr.host;
	}
	automore_header(aarphead);
	aarp_show_table(net, node);
	automore_header(NULL);
	break;
      case SHOW_APPLE_ZONES:
	if (GETOBJ(int,1) == TRUE) {
	    zonelen = encode_zonename(GETOBJ(string,1),zonename);
	    show_appletalk_zonedetail(zonename,zonelen);
	} else {
	    show_appletalk_zones();
	}
	break;
      case SHOW_APPLE_CACHE:
	show_atcache();
	break;
      case SHOW_APPLE_GLOBALS:
	show_appletalk_globals();
	break;
      case SHOW_APPLE_PRIVATE:
	net = node = 0;
	if (GETOBJ(int,2)) {
	    net  = GETOBJ(paddr,1)->apple_addr.net;
	    node = GETOBJ(paddr,1)->apple_addr.host;
	}
	sprintf(pvt_header, "AppleTalk internal stats");
	automore_header(pvt_header);
	switch (GETOBJ(int, 1)) {
	  case ATALK_SHOW_PVT_GLOBALS:
	    atalk_show_pvt_globals();
	    break;
	  case ATALK_SHOW_PVT_INTERFACES:
	    atalk_show_pvt_interfaces();
	    break;
	  case ATALK_SHOW_PVT_ROUTES:
	    atroute_printtrees();
	    break;
	  case ATALK_SHOW_PVT_AARP:
	    atalk_show_pvt_aarp(atalk_address(net,node));
	    break;
	  case ATALK_SHOW_PVT_RTMP:
	    atrtmp_show_private();
	    break;
	  case ATALK_SHOW_PVT_IGRP:
	    reg_invoke_atalk_show_private();
	    break;
	  case ATALK_SHOW_PVT_CACHE:
	    atfast_show_pvt_cache(atalk_address(net,node));
	    break;
	  case ATALK_SHOW_PVT_REMAP:
	    atalk_show_pvt_remap();
	    break;
	  case ATALK_SHOW_PVT_ALL:
	    atalk_show_pvt_globals();
	    atalk_show_pvt_interfaces();
	    atroute_printtrees();
	    atalk_show_pvt_aarp(0);
	    atfast_show_pvt_cache(0);
	    atrtmp_show_private();
	    reg_invoke_atalk_show_private();
	    atalk_show_pvt_remap();
	    break;
	  case ATALK_SHOW_PVT_DEFAULT:
	    atalk_show_pvt_old();	/* compatibility for the CE's */
	    break;
	  default:
	    break;
	}
	automore_header(NULL);
	break;
      case SHOW_APPLE_ROUTE:
	if (GETOBJ(int,1) != 1) {
	    if (GETOBJ(int,1) == 2) {
		show_swidb_chain(GETOBJ(idb,1), atalkif_print_routes, 0);
		return;
	    } else {
		net = 0;
	    }
	} else {
	    net = GETOBJ(int,2);
	}
	show_appletalk_route(net,0);
	if (net != 0) {
	    show_route_detail(net);
	}
	break;
      case SHOW_APPLE_SOCKETS:
	if (GETOBJ(int,1) == TRUE) {
	    atalk_PrintSocket(GETOBJ(int,2));
	} else {
	    atalk_PrintActiveSockets();
	}
	break;
      case SHOW_APPLE_NEIGHBORS:
	if (GETOBJ(int,1) == TRUE) {
	    net = GETOBJ(paddr,1)->apple_addr.net;
	    node = GETOBJ(paddr,1)->apple_addr.host;
	} else {
	    net = 0;
	    node = 0;
	}
	show_appletalk_neighbors(net,node);
	break;
      case SHOW_APPLE_ADJACENT:
	if (GETOBJ(int,1)) {
	    net = GETOBJ(int,2);
	} else {
	    net = 0;
	}
	show_appletalk_route(net, 1);
	break;
      case SHOW_APPLE_ACCESS:
	show_atalkaccess_shell(GETOBJ(int, 1));
	break;
      case SHOW_APPLE_TRAFFIC:
	show_appletalk_traffic();
	break;
      case SHOW_APPLE_STATIC:
	show_appletalk_static();
	break;
      case SHOW_APPLE_INTERFACE:
	if (GETOBJ(int,2)) {
	    atalk_show_int_brief(GETOBJ(idb,1));
	} else {
	    show_swidb_chain(GETOBJ(idb,1), display_appletalk, 0);
	}
	break;
      case SHOW_APPLE_NBP:
	/* dump the NBP names table: */
	atnbp_nve_dump();
	break;
      case SHOW_APPLE_NAMES:
	atmaint_ShowLookupCache();
	break;
      case SHOW_APPLE_MAP:
#if ATALK_NOT_USED_YET
	parse_interface(args, &target);
	show_swidb_chain(target, display_appletalk_map, 0);
#endif
	break;
    }
    automore_disable();
}

/*
 * atalk_print_route
 *
 * Print detailed information about a route. Invoked by the 
 * "show appletalk route <nnnnn>" command.
 */
void
atalk_print_route (atalk_rdb_t* r)
{
    int len = 0;
    int hlen = 0;
    zipnetentry* zn;

    /* Allow stuff to change underneath us without us crashing. */
    /* Pointer to current zone */
    ziptabletype *zone;
    /*
     * A table of pointers to zones, each mem_locked. The constant is
     * a quick guess at the typical maximum zone list for any given
     * cablerange. If current route exceeds it, a new table is malloc'ed.
     */
    ziptabletype *on_stack_zone_table[AT_PRINT_ROUTE_ON_STACK_ZONES];
    /*
     * If more than the 'guessed' number of zones exist for the cablerange,
     * then a new temporary table is malloced. This points to it. By
     * default, it is the on-stack zone table (did_malloc == FALSE.)
     */
    ziptabletype **zone_table = on_stack_zone_table;
    boolean did_malloc = FALSE;
    int zcount, i;

    if (r == NULL)
	return;
    if (automore_quit())
	return;
    /*
     * See how many zones there are. If fewer than
     * AT_PRINT_ROUTE_ON_STACK_ZONES, then you can use the stack variable
     * to store the zone list. If not you have to malloc one.
     */
    zcount = atalk_CountAndFilterZones(r, 0); /* No filtering done. */
    if (zcount > AT_PRINT_ROUTE_ON_STACK_ZONES) {
	zone_table = malloc(sizeof(ziptabletype *) * zcount);
	if (zone_table == NULL) {
	    printf("\nNo memory available to display zones of next route!");
	    zcount = 0;
	} else {
	    did_malloc = TRUE;
	}
    }
    /* Copy pointers into temporary zone table. Lock zones themselves. */ 
    for (i = 0, zn = atalk_firstZone(r); ((zn != NULL) && (i < zcount));
	 i++, zn = atalkzn_nextZone(zn)) {
	zone_table[i] = zn->zone;
	inline_mem_lock(zn->zone);
    }
    zcount = min(i, zcount);
    len = printf("\n%c Net %#A ", atroute_TypeInitial(r),atalk_CableRange(r)); 
    if (!atalk_net_connected(r)) {

	if(atalk_DomainRouter && r->domain) {
	    at_Domain *dm_ptr;
	    dm_ptr = atdomain_FindDomain_by_number(r->domain);
	    if(dm_ptr && dm_ptr->hop_rdctn) 
	    	len += printf("[1/%c] ",atroute_PathState(r->best_path));
	    else
		len += printf("[%d/%c] ", atrtmp_GetRouteMetric(r),
			      atroute_PathState(r->best_path));
	}
	else
	    len += printf("[%d/%c] ", atrtmp_GetRouteMetric(r),
			  atroute_PathState(r->best_path));
    }
    hlen = len;
    if (atalk_net_proxy(r)) {
	len += printf("NBP proxy");
    } else if (atalk_net_connected(r)) {
	len += printf("directly connected");
	len += printf(", %s", idb_get_namestring(r->rpath.idb->phys_idb));
    } else {
	if (r->rpath.gwptr != NULL) 
	    len += printf("via %#a", r->rpath.gwptr->address);
	len += printf(", %ld sec", ELAPSED_TIME(r->timer)/ONESEC);
	if (r->rpath.idb != NULL)
	    len += printf(", %s", idb_get_namestring(r->rpath.idb->phys_idb));
    }

    printf(",");
    if (!atroute_GetEqualPathCount(r->best_path)) {
        if (zcount > 0) {
	    zone = zone_table[0];
	    if ((strlen((char*)zone->pname)+len+9) > 78) {
	        if (zcount > 1)
		    printf("\n%*cPrimary zone: \042%s\042",hlen-1, ' ',
		           zone->pname);
	        else
		    printf("\n%*cZone: \042%s\042",hlen-1,' ',zone->pname);
	    }
	    else
	        printf(" zone %s",zone->pname);

	    if (zcount > 1) {
	        len = printf("\n%*cAdditional zones: ",hlen-1,' ');
	        hlen = len;
	        for (i = 1; i < zcount; i++) {
		    zone = zone_table[i];
		    if ((strlen(zone->pname)+len+2) > 78) {
		        len = printf("\n%*c",hlen-1,' ');
		    }
		    len += printf("\047%s\047%c",zone->pname,
			         ((i + 1 < zcount) ? ',' : ' '));
		}
	    }
	} 
        else {
	    printf(" no zone set");
	}
    }
    /*
     * Free the zones that were locked before. Free the 'malloced'
     * table if one was created
     */
    for (i = 0; i < zcount; i++)
	inline_mem_unlock_or_free(zone_table[i]);
    if (did_malloc)
        free(zone_table);
    return;
}

static void
atalk_print_equalroute (atalk_rdb_t *r, atalk_pdb_t *p)
{
    char buf[100];
    int len = 0;
    int hlen = 0;
    zipnetentry* zn;
    boolean lastone;

    /* Allow stuff to change underneath us without us crashing. */
    /* Pointer to current zone */
    ziptabletype *zone;
    /*
     * A table of pointers to zones, each mem_locked. The constant is
     * a quick guess at the typical maximum zone list for any given
     * cablerange. If current route exceeds it, a new table is malloc'ed.
     */
    ziptabletype *on_stack_zone_table[AT_PRINT_ROUTE_ON_STACK_ZONES];
    /*
     * If more than the 'guessed' number of zones exist for the cablerange,
     * then a new temporary table is malloced. This points to it. By
     * default, it is the on-stack zone table (did_malloc == FALSE.)
     */
    ziptabletype **zone_table = on_stack_zone_table;
    boolean did_malloc = FALSE;
    int zcount, i;
 
    if (automore_quit())
	return;
    /*
     * See how many zones there are. If fewer than
     * AT_PRINT_ROUTE_ON_STACK_ZONES, then you can use the stack variable
     * to store the zone list. If not you have to malloc one.
     */
    zcount = atalk_CountAndFilterZones(r, 0); /* No filtering done. */
    if (zcount > AT_PRINT_ROUTE_ON_STACK_ZONES) {
	zone_table = malloc(sizeof(ziptabletype *) * zcount);
	if (zone_table == NULL) {
	    printf("\nNo memory available to display zones of next route!");
	    zcount = 0;
	} 
	else
	    did_malloc = TRUE;
    }
    /* Copy pointers into temporary zone table. Lock zones themselves. */ 
    for (i = 0, zn = atalk_firstZone(r); ((zn != NULL) && (i < zcount));
	 i++, zn = atalkzn_nextZone(zn)) {
	zone_table[i] = zn->zone;
	inline_mem_lock(zn->zone);
    }
    zcount = min(i, zcount);
    len = sprintf(buf,"%c Net %#A ", atroute_TypeInitial(r),atalk_CableRange(r)); 

    if (atalk_DomainRouter && r->domain) {
	at_Domain *dm_ptr;
	dm_ptr = atdomain_FindDomain_by_number(r->domain);
	if (dm_ptr && dm_ptr->hop_rdctn) 
	    len += sprintf(buf, "[1/%c] ",atroute_PathState(r->best_path));
	else
	    len += sprintf(buf, "[%d/%c] ", atrtmp_GetRouteMetric(r),
			   atroute_PathState(r->best_path));
    }
    else
	len += sprintf(buf, "[%d/%c] ", atrtmp_GetRouteMetric(r),
		       atroute_PathState(r->best_path));

    hlen = len;
    printf("\n%*c", len, ' ');
    lastone = (p->equalpath == r->best_path);

    if (p->gwptr != NULL) 
	len += printf("via %#a", p->gateway);
    len += printf(", %ld sec", ELAPSED_TIME(p->timer)/ONESEC);
    if (p->idb)
        len += printf(", %s", idb_get_namestring(p->idb->phys_idb));

    printf(",");
    if (lastone) {
        if (zcount > 0) {
	    zone = zone_table[0];
	    if ((strlen(zone->pname)+len+9) > 78) {
	        if (zcount > 1)
		    printf("\n%*cPrimary zone: \042%s\042",hlen, ' ',
		           zone->pname);
	        else
		    printf("\n%*cZone: \042%s\042",hlen,' ',zone->pname);
	    }
	    else
	        printf(" zone %s",zone->pname);

	    if (zcount > 1) {
	        len = printf("\n%*cAdditional zones: ",hlen,' ');
	        hlen = len;
	        for (i = 1; i < zcount; i++) {
		    zone = zone_table[i];
		    if ((strlen(zone->pname)+len+2) > 78)
		        len = printf("\n%*c",hlen,' ');
		    len += printf("\047%s\047%c",zone->pname,
			         ((i + 1 < zcount) ? ',' : ' '));
	        }
	    }
        } 
        else
	    printf(" no zone set");
    }
    /*
     * Free the zones that were locked before. Free the 'malloced'
     * table if one was created
     */
    for (i = 0; i < zcount; i++)
	inline_mem_unlock_or_free(zone_table[i]);
    if (did_malloc)
        free(zone_table);
    return;
}

static void
show_equalroutes (atalk_rdb_t *r)
{
    atalk_pdb_t *next;
    short count;
    
    if (!r->best_path)
        return;
    next = r->best_path->equalpath;
    count = atroute_GetEqualPathCount(r->best_path);
    while (count-- && next) {
      mem_lock(next);
      atalk_print_equalroute(r, next);
      free(next);
      next = next->equalpath;
    }
}


#define PRINT_ROUTETABLE_HEADER() 					\
    printf("Codes: R - RTMP derived, E - EIGRP derived, C - connected, " \
	   "A - AURP\n       S - static " \
	   " P - proxy \n%d route%s in internet",			\
	   ArgAndPlural(atroute_RouteCount(),"","s"));                  \
    if (atalk_MaxPaths > 1)                                             \
        printf(". Up to %d parallel paths allowed.", atalk_MaxPaths);   \
    printf("\n\nThe first zone listed for each entry is its default (primary) zone.\n");

    
/*
 * show_appletalk_route
 * display appletalk routing information
 *
 * This function will show the route/path currently in use.
 * To see the paths, use the "show appletalk paths [net#]" exec command.
 */
void
show_appletalk_route (ushort net, int limit)
{
    atalk_rdb_t *r = NULL;		/* route entry */
    rbTree	*tree;

    tree = atroute_GetTree();

    ++atalk_InRouteZoneWeb;
    PRINT_ROUTETABLE_HEADER();
    if (net != 0) {
	r = atroute_FindRoute(tree,net,TRUE);
	if (r != NULL) {
	    atalk_print_route(r);
	    if (atroute_GetEqualPathCount(r->best_path))
	        show_equalroutes(r);
	    ReleaseRoute(r);
	}
	--atalk_InRouteZoneWeb;
	return;
    }
    for (r = KeepFirstRoute(tree); r != NULL; r = KeepNextRoute(tree, r)) {
	if ((limit > 0) && (atrtmp_GetRouteMetric(r) > limit))
	    continue;
	atalk_print_route(r);
	if (atroute_GetEqualPathCount(r->best_path))
	    show_equalroutes(r);
    }
    --atalk_InRouteZoneWeb;
}

/*
 * atalkif_print_routes
 * display appletalk routing information by interface
 */
void
atalkif_print_routes (idbtype* idb)
{
    atalkidbtype *atalkidb;
    atalk_rdb_t *r;
    rbTree	*tree;

    if (idb == NULL)
	return;

    atalkidb = atalk_getidb(idb);
    if (atalkidb == NULL)
	return;
    
    tree = atroute_GetTree();
    ++atalk_InRouteZoneWeb;
    PRINT_ROUTETABLE_HEADER();
    for (r = KeepFirstRoute(tree); r != NULL; r = KeepNextRoute(tree, r)) {
	if (r->rpath.idb != atalkidb)
	    continue;
	atalk_print_route(r);
    }
    --atalk_InRouteZoneWeb;
}

/*
 * show_at_equalcache()
 *
 *   show equal cost cache entries that are
 *   linked to the primary cache entry
 */
static void
show_at_equalcache (atcachetype *primary)
{
   int jx;
   atcachetype *ecptr;

   ecptr = primary->equalcache;
   while (ecptr && ecptr != primary) {
       mem_lock(ecptr);
       printf("\n");
       printf("%13c %18s ", ' ', idb_get_namestring(ecptr->idb));

       jx = 0;
       do {
	   printf("%-02x", ecptr->m.macstring[jx]);
	   jx++;
	   if ((jx % 16) == 0)
	       printf("\n%27c", ' ');
       } while (jx < ecptr->byte_len);

       free(ecptr);
       ecptr = ecptr->equalcache;
   }
}


void
show_atcache (void)
{
    static const char rstring[] = " Destination  Interface          MAC Header";

    atcachetype *ptr;
    int		jx;
    boolean	onceflag = FALSE;
    char	flag;
    rbTree	*tree;

    tree = atfast_GetCachePtr();
    printf("AppleTalk Routing Cache, * = active entry, cache version is %lu", 
	   atfast_CacheVersion());
    for (ptr = KeepFirstEntry(tree);
	 ptr;
	 ptr = KeepNextEntry(tree, ptr)) {
	if (!onceflag) {
	    automore_header(rstring);
	    onceflag = TRUE;
	}
	if (atfast_cacheVersion == ptr->version)
	    flag = '*';
	else
	    flag = ' ';
	printf("\n%c %11a %18s ",
	       flag, atalk_address(ptr->net,ptr->node),
	       idb_get_namestring(ptr->idb));
	jx = 0;
	do {
	    printf("%-02x", ptr->m.macstring[jx]);
	    jx++;
	    if ((jx % 16) == 0)
		printf("\n%27c", ' ');
	} while (jx < ptr->byte_len);

	if (ptr->equalcount && (atalk_MaxPaths > 1))
	    show_at_equalcache(ptr);
    }
}

static char*
atshow_PeerType (neighborentry *n)
{
    if (atalkif_HalfRouter(n->port)) {
	if (atalkif_CaymanTunnel(n->port))
	    return ("Cayman Tunnel");
	else if (atalkif_AURPTunnel(n->port))
	    return ("AURP Tunnel");
	else
	    return ("half-router");
    }
    if (atmaint_NeighborIsReachable(n)) {
	if ((n->reachable_by & (ATALK_IGRP_ENABLED|ATALK_RTMP_ENABLED))
	    == (ATALK_IGRP_ENABLED|ATALK_RTMP_ENABLED))
	    return ("RTMP & EIGRP");
	if (n->reachable_by & ATALK_IGRP_ENABLED)
	    return ("EIGRP");
	if (n->reachable_by & ATALK_RTMP_ENABLED)
	    return ("RTMP");
	if (n->reachable_by & ATALK_AURP_ENABLED)
	    return ("AURP");
    }
    return ("unknown");			/* shouldn't get here */
}

static char*
atshow_RPenabled (atalkidbtype *idb)
{
    if (idb->atalk_rp & (ATALK_IGRP_ENABLED|ATALK_RTMP_ENABLED)) {
	if ((idb->atalk_rp & (ATALK_IGRP_ENABLED|ATALK_RTMP_ENABLED))
	    == (ATALK_IGRP_ENABLED|ATALK_RTMP_ENABLED))
	    return ("RTMP & EIGRP");
	if (idb->atalk_rp & ATALK_IGRP_ENABLED)
	    return ("EIGRP");
	if (idb->atalk_rp & ATALK_RTMP_ENABLED)
	    return ("RTMP");
    }
    if (idb->atalk_rp & ATALK_AURP_ENABLED)
	return ("AURP");
    return ("unknown");			/* shouldn't get here */
}

/*
 * atalk_show_int_brief
 *
 * Quick one-line summaries of the AppleTalk status of an idb.
 *
 */
void
atalk_show_int_brief (idbtype *idb)
{
    char	addrbuf[10];
    boolean	once = FALSE;		/* TRUE if we print one idb */
    idbtype	*iidb;
    atalkidbtype *atalkidb;
    hwidbtype   *hwidb;
    
    automore_enable(NULL);
    printf("\n%16s %10s  %12s  %21s %14s", "Interface", "Address",
	"Config", "Status/Line Protocol", "Atalk Protocol");

    iidb = (idb != NULL) ? idb : SWIDBQ_HEAD;
    for ( ; !once && iidb; iidb = IDB_NEXT(iidb)) {
	atalkidb = atalk_getidb(iidb);
	hwidb = hwidb_or_null(iidb);
	
	/* Deleted interfaces include tunnels that have been deleted. Ugh! */
	if (hwidb && (hwidb->status & IDB_DELETED))
	    continue;

	once = (idb != NULL);
	if (is_vip(hwidb))
	    printf("\n%16s ", iidb->short_namestring);
        else
	    printf("\n%16s ", idb_get_namestring(iidb));
	if (atalkidb && atalkidb->atalk_enabled) {
	    if (atalkif_AURPTunnel(atalkidb) ||
		atalkif_CaymanTunnel(atalkidb)) {
		printf("%10s  ", "unassigned");
	    } else {
		sprintf(addrbuf, "%a", atalkif_MyAddress(atalkidb));
		printf("%10s  ", addrbuf);
	    }
	    if (!atalkif_net_extended(atalkidb))
		printf("%12s  ", "Non-Extended");
	    else
		printf("%12s  ", "Extended");
	    printf("%21s ", print_idbstate(iidb));
	    printf("%14s", atalkif_operational(atalkidb) ? "up" : "down");
	} else {
	    printf("%10s  %12s  ", "unassigned", "not config'd");
	    printf("%21s n/a", print_idbstate(iidb));
	    continue;
	}
    }
    automore_disable();
}

/*
 *
 */
void
at_print_key_address (char *text, treeKey key)
{
    sprintf(text, "%la", key.l);
}

/*
 *
 */
void
at_print_key_idb_address (char *text, treeKey key)
{
    sprintf(text, "hb:#%d A:%a",
	    ((key.l & 0xff000000) >> 24), key.l & 0x00ffffff);
}

/*
 *
 */
void
at_print_key_long_idb_address (char *text, treeKey key)
{
    sprintf(text, "hb:#%lu A:%a",
	    ((key.ll >> 24) & 0xffffffff), (key.ll & 0xffffff));
}

/*
 *
 */
void
at_print_key_interval (char *text, treeKey key)
{
    sprintf(text, "l:#%d h:%#d", key.i.low, key.i.high);
}

