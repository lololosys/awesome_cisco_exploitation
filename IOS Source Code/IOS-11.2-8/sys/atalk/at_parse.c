/* $Id: at_parse.c,v 3.8.18.11 1996/08/27 19:54:49 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_parse.c,v $
 *------------------------------------------------------------------
 * at_parse.c  -- AppleTalk parsing subroutines
 *
 * August 1990, David Edwards
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_parse.c,v $
 * Revision 3.8.18.11  1996/08/27  19:54:49  dwong
 * CSCdi67223:  Spurious access when unconfiguring an interface with atalk
 * phase 2
 * Branch: California_branch
 *
 * Revision 3.8.18.10  1996/07/10  22:18:39  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.8.18.9  1996/07/08  07:09:18  dwong
 * CSCdi62125:  CSCdi60209 broke AppleTalk
 * Branch: California_branch
 *
 * Revision 3.8.18.8  1996/06/29  00:06:59  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.8.18.7  1996/06/17  23:29:14  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.8.18.6  1996/05/02  08:40:15  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.8.18.5  1996/05/01  14:26:09  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between Appletalk network code and ATCP code.
 *
 * Revision 3.8.18.4  1996/04/27  05:44:58  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.4.8.3  1996/04/25  23:56:36  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.4.8.2  1996/04/17  00:30:37  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.4.8.1  1996/02/23  20:11:34  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.8.18.3  1996/04/02  05:35:45  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.8.18.2  1996/03/23  01:27:11  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.8.18.1  1996/03/18  18:52:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/13  01:07:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.2  1996/03/07  08:29:13  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  00:17:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/07  16:09:52  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/01  05:59:07  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  07:26:09  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/18  19:06:10  slin
 * CSCdi46748: 7513 crashes when configuring appletalk zone, when short of
 *             memory
 *
 * Revision 3.4  1995/12/24  04:56:24  dwong
 * CSCdi44162:  Appletalk doesnt work between ISDN callers
 *
 * Revision 3.3  1995/11/17  08:43:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:57:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:50:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/19  23:54:46  kleung
 * CSCdi38726:  AppleTalk RTMP Stub rtmp_stub feature - only RTMP stubs
 * sent.
 *
 * Revision 2.5  1995/09/12  19:35:09  slin
 * CSCdi39096:  Inability to NBP-register AUFS Server (Unix Host).
 * - Add support for routing between local devices
 *
 * Revision 2.4  1995/07/13  06:49:57  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.3  1995/06/21 22:21:43  bbenson
 * CSCdi36258:  ATalk phase I wont come up on LANE.
 * - Removed hwidb->enctype check for ARPA on the AIP for LANE.
 *
 * Revision 2.2  1995/06/10  12:25:09  dwong
 * CSCdi35099:  clear apple neighbor is not working
 *
 * Revision 2.1  1995/06/07  20:08:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_registry.h"
#include "../atalk/at_fast.h"
#include "../atalk/at_arp.h"
#include "parser.h"
#include "parser_defs_atalk.h"
#include "../atalk/at_static.h"
#include "../atalk/macip_util.h"
#include "../atalk/atalk_nbp.h"
#include "../if/atm.h"
#include "../lane/lane.h"



/* Public data definitions */

    /* these define the PPP net. */
ushort atalk_virtual_net;
uchar atalk_virtual_zone[ZONELEN];
int atalk_virtual_zonelen;   /* if this is set, then a network exists */

/* ----------------------- */


/*
 * atalk_parse_address
 * parse appletalk address of form net.node[:socket]
 */

boolean 
atalk_parse_address (char* buf, ushort* net, uchar* node, uchar* socket)
{
    uchar	*ptr, c;
    ushort	temp1;
    uchar	temp2;
    uchar	temp3 = 0;

    if (null(buf))
	return (FALSE);
    ptr = deblank(buf);
    c = *ptr++;
    temp1 = temp2 = 0;
    while ((c >= '0') && (c <= '9')) {
	temp1 = temp1 * 10 + ((int) c - '0');
	c = *ptr++;
    }
    if (c != '.')
	return (FALSE);
    c = *ptr++;
    while ((c >= '0') && (c <= '9')) {
	temp2 = temp2 * 10 + ((int) c - '0');
	c = *ptr++;
    }
    if (c == ':') {
	while ((c >= '0') && (c <= '9')) {
	    temp3 = temp3 * 10 + ((int) c - '0');
	    c = *ptr++;
	}
    }
    if (c != '\0')
	return (FALSE);

    *net = temp1;
    *node = temp2;
    if (socket)
	*socket = temp3;
    return (TRUE);
}

/* 
 * appletalk_command
 *
 * Parse appletalk commands.  Global commands are handled within this routine.
 * Interface configuration commands are passed down to atalkif_command().
 */
void appletalk_command (parseinfo *csb)
{
    int zlen;
    ulong *interval, *count;
    uchar zbuf[ZONELEN+1];
    ushort net;
    uint t1, t2, t3;
    int def_interval, def_count;
    boolean doTimers;

    csb->interface = NULL;
    if (csb->nvgen) {
	if (!atalk_running)
	    return;

	switch (csb->which) {
	  case ATALK_EVENTS:
	    /* Store apple event-logging only if it was explicitly specified */
	    /* during configuration, since debug apple-events can also turn */
	    /* on atalk_events. */
	    if (atalk_events_logged) {	
		nv_write(atalk_events->flag != 0,  csb->nv_command);
		if (atalk_events->flag == -1) {
		    ;
		} else {
		    if (atalk_events->list.idb) {
			nv_add(atalk_events->flag & ATALK_INTERFACE_MASK,
			       " interface %s",
			       atalk_events->list.idb->namestring);
		    }
		}
	    }
	    break;

	  case ATALK_ARP_PARAM:
	    nv_write(
		     (aarp_request_interval != AARP_REQUEST_INTERVAL) && 
		     (aarp_request_interval != 0),
		     "%s request interval %d", 
		     csb->nv_command, aarp_request_interval);
	    nv_write(
		     (aarp_request_count != AARP_REQUEST_RETRANSMIT) &&
		     (aarp_request_count != 0),
		     "%s request retransmit-count %d",
		     csb->nv_command, aarp_request_count);
	    nv_write(
		     (aarp_probe_interval != AARP_PROBE_INTERVAL) && 
		     (aarp_probe_interval != 0),
		     "%s probe interval %d",
		     csb->nv_command, aarp_probe_interval);
	    nv_write(
		     (aarp_probe_count != AARP_PROBE_RETRANSMIT) &&
		     (aarp_probe_count != 0),
		     "%s probe retransmit-count %d", 
		     csb->nv_command, aarp_probe_count);
	    break;

	  case ATALK_ROUTEDIST:
	    nv_write(atalk_redist_status & ATALK_REDIST_CONFIGURED,
		     "%s", csb->nv_command);
	    break;

	  case ATALK_VIRTUAL_NET:
	     nv_write(atalk_virtual_zonelen, "%s %d %s", csb->nv_command, 
			atalk_virtual_net, atalk_virtual_zone);
	     break;

	  case ATALK_RTMP_COMMAND:
	    atrtmp_config_command(csb);
	    break;

	  case ATALK_RTMP_BW:		/* move to atrtmp_config_command */
	    nv_write((atalk_rtmp_bw != RTMP_BANDWIDTH), "%s %d",
		     csb->nv_command, atalk_rtmp_bw);
	    break;


	  case ATALK_STATIC:
	    atalk_static_command(csb);
	    break;

	  case ATALK_SERVICE:
	  case ATALK_ROUTING:
	    nv_write(atalk_running && !atalk_igrp_running,
		     "%s", csb->nv_command);
	    break;

	  case ATALK_CHECKSUM:
	    nv_write(!atalk_checksumming, "no %s",
		     csb->nv_command);
	    break;

	  case ATALK_STRICT:		/* move to atrtmp_config_command */
	    nv_write(!atalk_strictRTMP, "no %s",
		     csb->nv_command);
	    break;

	  case ATALK_ROUTEZONES:
	    nv_write(!atalk_routeZones, "no %s",
		     csb->nv_command);
	    break;

	case ATALK_LOCALROUTE:
	    nv_write(atalk_localrouting, "%s", csb->nv_command);
	    break;
	    
	  case ATALK_TESTTOOL:
	    nv_write(atalk_testToolCompat, csb->nv_command);
	    break;

	  case ATALK_PARTIALZONES:
	    nv_write(atalk_permitPartialZones, csb->nv_command);
	    break;

	  case ATALK_ALTADDRS:
	    nv_write(atalk_alternateAddrs, csb->nv_command);
	    break;

	  case ATALK_ZONEQUERY:
	    nv_write(atalk_zipQueryInterval != ZIP_QUERYTIME,
		     "%s %d", csb->nv_command,
		     atalk_zipQueryInterval/ONESEC);
	    break;

	  case ATALK_PROXYNBP:
	    atroute_WriteNV(csb->nv_command);
	    break;

	  case ATALK_IGN_VERIFY:
	    nv_write(atalk_ignoreVerifyErrs, csb->nv_command);
	    break;

	  case ATALK_TIMERS:		/* move to atrtmp_config_command */
	    doTimers = ((atalk_rtmpInvalidTime != RTMP_INVALIDTIME) ||
			(atalk_rtmpValidTime != RTMP_VALIDTIME) ||
			(atalk_rtmpUpdateTime != RTMP_UPDATETIME));
	    nv_write(doTimers,"%s %d %d %d", csb->nv_command,
		     (atalk_rtmpUpdateTime == RTMP_UPDATETIME) ?
		     0 : atalk_rtmpUpdateTime/ONESEC,
		     (atalk_rtmpValidTime == (atalk_rtmpUpdateTime*RTMP_VALIDMULT) ?
		      0 : atalk_rtmpValidTime/ONESEC),
		     (atalk_rtmpInvalidTime == (atalk_rtmpValidTime*RTMP_INVALIDMULT) ?
		      0 : atalk_rtmpInvalidTime/ONESEC));
	    break;

	  case ATALK_NAMELOOKUP:
	    nv_write(atalk_nameLookupInterval != 0, "%s %d",
		     csb->nv_command, atalk_nameLookupInterval/ONESEC);
	    break;

	  case ATALK_LOOKUPTYPE:
	    atmaint_WriteNV(csb->nv_command);
	    break;

	  case ATALK_MAXPATHS:
	    nv_write((atalk_MaxPaths != 1), "%s %d",
		     csb->nv_command, atalk_MaxPaths);
	    break;

	  default:
	    printf("\n%% NVGEN: unknown command - %s", csb->nv_command);
	    break;
	}
	return;
    }

    if (!atalk_running && csb->which != ATALK_ROUTING) {
	printf(atalk_not_running);
	return;
    }

    switch (csb->which) {
      case ATALK_STATIC:
	atalk_static_command(csb);
	break;

      case ATALK_EVENTS:
	atalk_debug_setflag(csb, atalk_events);
	atalk_events_logged = csb->sense;
	return;

      case ATALK_ARP_PARAM:
	if (!csb->sense)  {
	    aarp_probe_interval = AARP_PROBE_INTERVAL;
	    aarp_probe_count = AARP_PROBE_RETRANSMIT;
	    aarp_request_interval = AARP_REQUEST_INTERVAL;
	    aarp_request_count = AARP_REQUEST_RETRANSMIT;
	    return;
	}
	switch (GETOBJ(int,1)) {
	case ATALK_ARPTYPE_PROBE:
	    interval = &aarp_probe_interval;
	    count = &aarp_probe_count;
	    def_interval = AARP_PROBE_INTERVAL;
	    def_count = AARP_PROBE_RETRANSMIT;
	    break;
	    
	case ATALK_ARPTYPE_REQUEST:
	    interval = &aarp_request_interval;
	    count = &aarp_request_count;
	    def_interval = AARP_REQUEST_INTERVAL;
	    def_count = AARP_REQUEST_RETRANSMIT;
	    break;
	    
	default:
	    printf("%% One of \"probe\" or \"request\"");
	    return;
	}
	switch (GETOBJ(int,2)) {
	case ATALK_ARP_INTERVAL: 
	    *interval = csb->sense ? GETOBJ(int,3) : def_interval;
	    break;
	    
	case ATALK_ARP_RETRANS:
	    *count = csb->sense ? GETOBJ(int,3) : def_count;
	    break;
	    
	default:
	    printf("%% One of \"interval\" or \"retransmit-count\"");
	    return;
	}
	break;

      case ATALK_SERVICE:
      case ATALK_ROUTING:
	if (csb->sense == FALSE)
	    atalk_stop();		/* which will call atigrp2_stop */
	else
	    atalk_start();
	break;

      case ATALK_ROUTEDIST:
	/*
	 * If already enabled, only a 'turn off' (csb->sense = FALSE)
	 * has any effect.
	 *
	 * If alread disabled, only a 'turn on' (csb->sensee = TRUE)
	 * has any effect.
	 *
	 * Either way, the redist flags must be SET when calling
	 * atroute_redist_all, otherwise the call has no effect.
	 */
	if (atalk_redist_status & ATALK_REDIST_ENABLED) {
	    if (!csb->sense) {
		atroute_redist_all(TRUE); /* Unredist all */
		atalk_redist_status &=
		    ~(ATALK_REDIST_CONFIGURED | ATALK_REDIST_ENABLED);
	    }
	} else {
	    if (csb->sense) {
		atalk_redist_status |=
		    ATALK_REDIST_CONFIGURED | ATALK_REDIST_ENABLED;
		atroute_redist_all(FALSE); /* redist all */
	    }
	}
	break;

      case ATALK_VIRTUAL_NET:
        atalk_virtual_net = (ushort)GETOBJ(int,1);
        if (csb->sense) {
            atalk_virtual_zonelen = atalk_hex_filter(GETOBJ(string,1),
                            atalk_virtual_zone, ZONELEN);
            if (atalk_virtual_zonelen <= 0) {
                printf("% bad Appletalk command:""zone not specified");
                return;
            }
            atroute_AddProxyPath(atalk_virtual_net,
                        atalk_virtual_zonelen,atalk_virtual_zone, TRUE);
        } else {
	    if (atalk_virtual_zonelen) {
		atroute_DeleteProxyPath(atalk_virtual_net);
	    }
            atalk_virtual_zonelen = 0;
        }
        break;

      case ATALK_RTMP_BW:
	atalk_rtmp_bw = csb->sense ? GETOBJ(int,1) : RTMP_BANDWIDTH;
	break;

      case ATALK_RTMP_COMMAND:
	atrtmp_config_command(csb);
	break;
	
      case ATALK_CHECKSUM:
	atalk_checksumming = csb->set_to_default || csb->sense;
	break;

      case ATALK_STRICT:
	atalk_strictRTMP = csb->set_to_default || csb->sense;
	break;

      case ATALK_ROUTEZONES:
	atalk_routeZones = csb->set_to_default || csb->sense;
	break;

      case ATALK_TESTTOOL:
	atalk_testToolCompat = csb->sense;
	break;

    case ATALK_LOCALROUTE:
	atalk_localrouting = csb->sense;
	break;
	
      case ATALK_PARTIALZONES:
	atalk_permitPartialZones = csb->sense;
	break;

      case ATALK_ALTADDRS:
	atalk_alternateAddrs = csb->sense;
	break;

      case ATALK_ZONEQUERY:
	atalk_zipQueryInterval = csb->sense ?
	    (GETOBJ(int,1) * ONESEC) :
	    ZIP_QUERYTIME;
	break;

    case ATALK_PROXYNBP:
	net = (ushort)GETOBJ(int,1);
	if (csb->sense) {
	    zlen = atalk_hex_filter(GETOBJ(string,1), (char *)zbuf, ZONELEN);
	    if (zlen <= 0) {
		printf(atalk_badcmd,"zone not specified");
		return;
	    }
	} else {
	    zlen = 0;
	}
	if (csb->sense) 
	    atroute_AddProxyPath(net,zlen,zbuf,FALSE);
	else
	    atroute_DeleteProxyPath(net);
	return;

    case ATALK_IGN_VERIFY:
 	atalk_ignoreVerifyErrs = csb->sense;
 	break;

    case ATALK_TIMERS:
	if (csb->sense && GETOBJ(int,1)) {
	    t1 = GETOBJ(int,1) * ONESEC;
	} else {
	    t1 = RTMP_UPDATETIME;
	}
	if (csb->sense && GETOBJ(int,2)) {
	    t2 = GETOBJ(int,2) * ONESEC;
	} else {
	    t2 = t1 * RTMP_VALIDMULT;
	}
	if (t2 <= t1) {
	    printf("\n%% Error: Valid timer (%d) must be greater than Update timer (%d)",
		   GETOBJ(int,2), GETOBJ(int,1));
	    return;
	}
	if (csb->sense && GETOBJ(int,3)) {
	    t3 = GETOBJ(int,3) * ONESEC;
	} else {
	    t3 = t2 * RTMP_INVALIDMULT;
	}
	if (t3 < (t2 + t1)) {
	    printf("\n%% Error: Invalid timer (%d) must be greater than or equal to"
		   "\n%%        sum of Update timer and Valid timer (%d)",
		   GETOBJ(int,3), GETOBJ(int,2) + GETOBJ(int,1));
	    return;
	}
	atalk_rtmpUpdateTime = t1;
	atalk_rtmpValidTime = t2;
	atalk_rtmpInvalidTime = t3;
 	break;

    case ATALK_NAMELOOKUP:
	atalk_nameLookupInterval = csb->sense ? (GETOBJ(int,1) * ONESEC) : 0;
	if (atalk_nameLookupInterval == 0) {
	    atmaint_stop_namelookup_timer();
	    atmaint_ClearNameCache();
	} else
	    atmaint_start_namelookup_timer();
	break;

    case ATALK_LOOKUPTYPE:
	zlen = atalk_hex_filter(GETOBJ(string,1), (char *)zbuf, ZONELEN);
	if (zlen <= 0)
	    printf(atalk_badcmd,"no type specified");
	else if (csb->sense)
	    atmaint_AddLookupType(zlen,zbuf);
	else
	    atmaint_DeleteLookupType(zlen,zbuf);
	break;

    case ATALK_MAXPATHS:
	atalk_MaxPaths = csb->sense ? GETOBJ(int,1) : 1;
	if (atalk_igrp_running)
	    reg_invoke_atalk_forceupdate();
	break;

    default:
	buginf("\nAT: Unhandled configuration command");
	break;
    }
}

/*
 * atalk_clear_route
 *
 * using a network number, look up a route in the route table. if found,
 * delete the route, performing housekeeping necessary. (deletion of zones,
 * etc)
 *
 */
static void
atalk_clear_route (ushort net)
{
    atalk_rdb_t *r = NULL;
    rbTree	*tree;

    tree = atroute_GetTree();
    if (net && (r = atroute_FindRoute(tree,net, TRUE))) {
	if (atalk_net_connected(r)) {
	    printf("\nCan't clear directly connected interface routes.");
	    ReleaseRoute(r);
	} else if (r->rpath.type == AT_PATH_STATIC) {
	    printf("\nThis command can not be used to clear AT static routes");
	    ReleaseRoute(r);
	} else {
	    atroute_ClearAllPaths(r);
	    ReleaseRoute(r);
	    atroute_DeleteRoute(r);
	}
    } else {
	errmsg(AT_ERR_NOSUCHROUTE, net);
    }
}

/*
 * atalk_clear_neighbors
 *
 * Clear all neighbor entries which are marked 'down'.
 */
void
atalk_clear_neighbor (ataddrtype nbr_addr, boolean all, boolean state)
{
    neighborentry	*n = NULL;
    neighborentry	*prev_n = NULL;
    ataddrtype          address;

    if (all) {
	do {
	    prev_n = n;
	    n = atmaint_GetNextNeighbor(prev_n);
	    if (prev_n != NULL && prev_n->state == neighborDown) {	
		address = prev_n->address;
		atmaint_ReleaseNeighbor(prev_n);
		if (!state ) {
		    atmaint_DeleteNeighbor(prev_n);
		} else {
		    prev_n->reachable_by = 0;
		    TIMER_STOP(prev_n->igrp_lastheard);
		    TIMER_STOP(prev_n->rtmp_lastheard);
		}
		errmsg(AT_ERR_NBRDELETED, address);
	    }
	} while (n != NULL);
    } else {
	n = atmaint_FindMatchingNeighbor(nbr_addr, NULL);
	atmaint_ReleaseNeighbor(n);
	if (n->state == neighborDown) {
	    address = n->address;
	    if (!state ) {
		atmaint_DeleteNeighbor(n);
	    } else {
		n->reachable_by = 0;
		TIMER_STOP(n->igrp_lastheard);
		TIMER_STOP(n->rtmp_lastheard);
	    }
	    errmsg(AT_ERR_NBRDELETED, address);
	} 
    }
}

/*
 * atalk_clear_command -- clear appletalk specific parameters.
 *
 * This command is designed to act as much as possible as the
 * "show appletalk" exec command, so as to minimise confusion.
 */
void clear_appletalk (parseinfo *csb)
{
    ushort	net = 0, i;
    uchar	node = 0;
    idbtype	*idb;
    ataddrtype	addr;
    ataddress	fqa_addr;

    if (!atalk_running) {
	printf(atalk_not_running);
	return;
    }

    csb->interface = NULL;

    switch (csb->which) {
      case CLEAR_APPLE_ARP:
	if (!GETOBJ(int,1)) {
	    net = node = 0;
	} else {
	    net = GETOBJ(paddr,1)->apple_addr.net;
	    node = GETOBJ(paddr,1)->apple_addr.host;
	}
	if (node != 0)
	    aarp_remove(atalk_address(net, node), NULL);
	else
	    flush_all_aarp(net);
	/*
	 * There should be a "clear apple route-cache" command to do this.
	 * Don't overload Fast Switching and ARP cache handling.
	 * 3/15/93, - Kirk L.
	 *
	 * Done, see CLEAR_APPLE_ROUTE_CACHE below. 940929 dsa
	 */
	atalk_fscache_flush();
	break;

      case CLEAR_APPLE_INTERFACE:
	idb = GETOBJ(idb,1);
	if (idb != NULL) {
	    atalkif_clear(idb);
	}
	break;

      case CLEAR_APPLE_ROUTE:
	net = GETOBJ(int,1);
	atalk_clear_route(net);
	break;

      case CLEAR_APPLE_ROUTE_CACHE:
	atalk_fscache_flush();
	break;

      case CLEAR_APPLE_NBP:
	if (GETOBJ(int,1)) {		/* was address spec'd? */
	    fqa_addr.net  = GETOBJ(paddr,1)->apple_addr.net;
	    fqa_addr.node = GETOBJ(paddr,1)->apple_addr.host;
	    fqa_addr.spec = 0;
	} else {			/* no address */
	    fqa_addr.net = fqa_addr.node = fqa_addr.spec = 0;
	}
	atnbp_DDPunregister(&fqa_addr);
	break;

      case CLEAR_APPLE_NEIGHBOR:
	if (GETOBJ(int,1)) {		/* was address spec'd? */
	    addr = atalk_address(GETOBJ(paddr,1)->apple_addr.net,
				 GETOBJ(paddr,1)->apple_addr.host);
	} else {			/* no address */
	    addr = 0;
	}
	atalk_clear_neighbor(addr, GETOBJ(int,2), GETOBJ(int,3));
	break;

      case CLEAR_APPLE_TRAFFIC:
	for (i = 0; i < ATALK_MAXTRAFFIC; ++i)
	    atalk_stat[i] = 0;
	break;

      default:
	buginf("\nclear_appletalk; bad subcommand");
	break;
    }
}







void
atalkif_shutdown (atalkidbtype* idb)
{
    if (idb == NULL)
	return;
    atalkif_ClearLocalConfig(idb);
    atalkif_AlwaysInit(idb, TRUE);
    atalkif_ClearOurMulticasts(idb);
    atmaint_ScheduleIFWakeup(idb);
}

/*
 * atalk_network_fixup
 * Wrapper around atalkif_clear for reg_invoke_encap_fixup.
 */
void
atalk_network_fixup (idbtype *idb, int old_encap)
{
    return(atalkif_clear(idb));
}


/*
 * atalkif_clear
 * Supports the appletalk clear interface command.
 *
 * Any interface-specific, dynamically discovered information should
 * be cleared here.
 */
void
atalkif_clear (idbtype* idb)
{
    atalkidbtype *atalkidb;
    ziptabletype *zone;

    if (!atalk_running || idb == NULL)
	return;
    atalkidb = atalk_getidb(idb);
    if (atalkidb == NULL || atalkidb->phys_idb->hwptr == NULL)
	return;
    flush_aarp(atalkidb);
    atalk_fscache_flush();
    if (!atalkidb->atalk_enabled)
	return;
    errmsg(AT_ERR_INTCLEARED, idb_get_namestring(idb));
    if (atalkidb->atalk_discovery)
	atalkif_ClearLocalConfig(atalkidb);
    else {
        zone = atalkif_primary_zone(atalkidb);
	if (zone)
	    ieee_copy(atalk_zone_multicast(atalkidb,
					   atalk_zonesum(zone->name, zone->length)),
		      atalkidb->atalk_zone_multicast);

	atalkif_SetOurMulticasts(atalkidb, atalkidb->atalk_zone_multicast);
    }
    atalkif_RestartPort(atalkidb);
    return;
}    

/*
 * atalkif_InterfaceConfigured
 * Check that the interface is fully configured for current mode.
 */
boolean
atalkif_InterfaceConfigured (atalkidbtype* idb)
{
    if (!atalk_running || !idb->atalk_enabled)
	return (FALSE);
    if (idb->atalk_discovery)
	return (TRUE);
    if (atalkif_HalfRouter(idb))
	return (TRUE);
    if (idb->atalk_cablestart == 0) 
	return (FALSE);
    if (idb->atalk_zonecount == 0)
	return (FALSE);
    return (TRUE);
}

/*
 * atalkif_Compatible - Check interface parameters for compatibility
 * with AT configuration.
 */
boolean
atalkif_InterfaceCompatible (atalkidbtype* idb)
{
    hwidbtype	*hwidb;

    if (idb == NULL)
	return (FALSE);
    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == NULL)
	return (FALSE);
    if (hwidb->vencap == NULL) {
	errmsg(AT_ERR_NOTSUPPORTED, idb_get_namestring(idb->phys_idb));
	return (FALSE);
    }
    if (hwidb->status & (IDB_ETHER|IDB_LEX)) {
	if (!atalkif_net_extended(idb)) {
	    if (hwidb->enctype != ET_ARPA) {
		errmsg(AT_ERR_NOTSUPPORTED, idb_get_namestring(idb->phys_idb));
		return (FALSE);	/* too dangerous */
	    }
	}
    }
    return (TRUE);
}

/*
 * atalkif_InternetCompatible
 * Check if interface violates compatibility mode.
 */
boolean
atalkif_InternetCompatible (atalkidbtype* idb)
{
    if (idb->atalk_compatibility || atalk_obsoleteRouterActive) {
	if ((idb->atalk_cableend != 0) &&
	    (idb->atalk_cableend != ATALK_RANGE_UNKNOWN) &&
	    (idb->atalk_cablestart != idb->atalk_cableend)) {
	    errmsg(AT_ERR_COMPATERR3,atalkif_netname(idb));
	    return (FALSE);
	} 
	if (idb->atalk_zonecount > 1) {
	    errmsg(AT_ERR_COMPATERR2,atalkif_netname(idb));
	    return (FALSE);
	}
    }
    return (TRUE);
}

#define ATALK_MAX_XADDR (ATALK_ETHER_BADADDR - 1)
#define ATALK_MAX_ADDR (ATALK_BROAD_ADDR - 1)
/*
 * at_ifHint
 * Validate or default the hint addess
 */
static ataddrtype
at_ifHint (ushort rstart, ushort rend, ataddrtype hint, uchar maxnode)
{
    ushort net = atalk_netonly(hint);
    uchar node = atalk_nodeonly(hint);

    if ((node == 0) || (node > maxnode))
	node = atalk_random(maxnode - 1) + 1;

    if ((net < rstart) || (net > rend))
	net = rstart + (atalk_random(rend - rstart));

    return (atalk_address(net,node));
}

/*
 * atalkif_setup - Validate and set various port parameters
 *
 * Also will set up any data structures which require knowledge of the
 * idb before starting.
 */
boolean
atalkif_setup (atalkidbtype* idb,ushort cablestart, ushort cableend,
	       ushort mynet, uchar mynode, boolean force_restart)
{
    char* err = ((cableend != 0) ? 
		 atalkif_ErrForExtended(idb) : 
		 atalkif_ErrForNonExtended(idb));
    ataddrtype hint = atalk_address(mynet, mynode);

    if (err) {
	printf(atalk_badaddr, err);
	return (FALSE);
    }
    if ((cablestart == 0)
	&& ((cableend == 0) || (cableend == ATALK_RANGE_UNKNOWN))
	&& (err = atalkif_ErrForDiscovery(idb))) {
	printf(atalk_badaddr, err);
	return (FALSE);
    }

    /* 
     * Now do configuration specific testing and setup the node address
     */
    if (cableend == 0) {
	/* 
	 * Non-extended configuration testing
	 */
	if (cablestart >= ATALK_STARTUP_START) {
	    printf(atalk_badaddr, "network is reserved");
	    return (FALSE);
	}
	hint = at_ifHint(cablestart,cablestart,hint,ATALK_MAX_ADDR);

    } else if (cableend == ATALK_NET_UNNUMBERED) {
	/* 
	 * Unnumbered configuration testing
	 */
	hint = at_ifHint(cablestart,cableend,hint,ATALK_MAX_XADDR);

    } else if (cablestart == 0) {
	/* 
	 * Extended discovery configuration testing
	 */
	if (cableend != ATALK_STARTUP_END) {
	    printf(atalk_badaddr, "discovery range must be 0-0");
	    return (FALSE);
	}
	hint = at_ifHint(ATALK_STARTUP_START,ATALK_STARTUP_END,hint,
			 ATALK_MAX_XADDR);

    } else {
	/*
	 * Extended seed configuration testing
	 */
	if (cablestart > cableend) {
	    printf(atalk_badaddr, "invalid range (start > end)");
	    return (FALSE);
	}
	if (cableend >= ATALK_STARTUP_START) {
	    printf(atalk_badaddr, "end of range exceeds ");
	    printf("%d", ATALK_STARTUP_START);
	    return (FALSE);
	}
	hint = at_ifHint(cablestart,cableend,hint,ATALK_MAX_XADDR);
    }

    if ((cablestart == idb->atalk_cablestart) &&
	(cableend == idb->atalk_cableend) &&
	(atalkif_InterfaceUp(idb)) && (!force_restart))
	return (TRUE);			/* we're already set up */

    /*
     * Take shut down AppleTalk routing on the interface, reset the
     * configuration and re-start.
     */
    atalkif_shutdown(idb);

    if (cablestart == 0)
	idb->atalk_discovery = TRUE;
    idb->atalknode = atalk_nodeonly(hint);
    idb->atalknet = atalk_netonly(hint);
    idb->atalk_cablestart = cablestart;
    idb->atalk_cableend = cableend;

    /*
     * If the routing protocol selector is currently 0, then no RP was
     * chosen.  Assume RTMP.
     */
    idb->atalk_enabled = TRUE;
    if (idb->atalk_rp == 0) 		/* just initialized */
	    idb->atalk_rp = ATALK_RTMP_ENABLED;	/* Default protocol */
    atalkif_RestartPort(idb);
    return (TRUE);
}

int
encode_zonename (char* inbuff,uchar *buff)
{
    ushort zonelen, index;

    index = 0;
    zonelen = 0;
    while (index < strlen((char *)inbuff)) {
	buff[zonelen] = inbuff[index++];
	if (buff[zonelen] == ':') {
	    uchar c = toupper(inbuff[index++]);
	    if ((c >= '0') && (c <= '9')) 
		buff[zonelen] = c - '0';
	    else if (c >= 'A' && c <= 'F')
		buff[zonelen] = c - '7';
	    c = toupper(inbuff[index++]);
	    if ((c >= '0') && (c <= '9')) 
		buff[zonelen] =
		    buff[zonelen]*16 + (c - '0');
	    else if (c >= 'A' && c <= 'F')
		buff[zonelen] =
		    buff[zonelen]*16 + (c - '7');
	}
	if (++zonelen >= ZONELEN)
	    break;
    }
    return (zonelen);
}

/*
 * atalkif_cmdzone
 * process AT interface zone commands
 */
boolean
atalkif_cmdzone (parseinfo* csb, atalkidbtype* idb)
{
    ziptabletype* zt;
    ushort zonelen;
    uchar zonename[ZONELEN];

    if (!idb->atalk_enabled) {
	printf(atalk_badcmd,"AppleTalk not enabled");
	return (FALSE);
    }
    if (idb->atalk_discovery || atalkif_HalfRouter(idb)) {
	printf(atalk_badcmd,
	       (idb->atalk_discovery) ?
	       "ZONE command invalid in discovery mode" :
	       "ZONE command invalid for half routers");
	return (FALSE);
    }
    if (!csb->sense) {
	zonelen = atalk_hex_filter(GETOBJ(string,1), (char *)zonename, ZONELEN);
	if (zonelen == 0) {
	    atalkif_clear_zones(idb);
	} else {
	    zt = atzip_FindZone(zonename,zonelen);
	    if (zt != NULL)
		atalkif_clear_zone(idb,zt);
	}
    } else {
	if (!atalkif_net_extended(idb))
	    atalkif_clear_zones(idb);
	zonelen = atalk_hex_filter(GETOBJ(string,1), (char *)zonename, ZONELEN);
	if (zonelen != 0) {
	    zt = atzip_CreateZone(zonename, zonelen);
	    if (zt == NULL || !atalkif_addZone(idb,zt))
		printf("\n%% AppleTalk zone %s could not be added to %s",
		       zonename, idb_get_namestring(idb->phys_idb));
	}
    }
    atalkif_RestartPort(idb);
    return (TRUE);
}

/*
 * atalkif_cmdrp
 * process AT interface routing-protocol commands.
 *
 * Note: the 'routing-protocol' command must be entered after any
 * 'address', 'cable-range' or 'tunnel' commands.
 */
static boolean
atalkif_cmdrp (parseinfo* csb, atalkidbtype* idb)
{
    int		which;			/* which routing protocol */

    which     = GETOBJ(int,1);
    if (csb->nvgen) {
	switch (which) {
	case ATALK_RP_RTMP:
	    if (!(idb->atalk_rp & ATALK_RTMP_ENABLED) &&
		!(idb->atalk_rp & ATALK_AURP_ENABLED) &&
		!(idb->atalk_allow_clients))
		nv_write(TRUE, "no %s", csb->nv_command);
	    break;

	case ATALK_RP_ISIS:		/* not implemented yet */
	    break;
	default:
	    break;
	}
	return (TRUE);
    }

    switch (which) {
      case ATALK_RP_RTMP:
	if (csb->sense) {
	    if (atalkif_AURPTunnel(idb)) {
		printf(atalk_badcmd, "RTMP not permitted on AURP tunnel");
		break;
	    }
	    idb->atalk_rp |= ATALK_RTMP_ENABLED;
	} else {
	    if (atalk_igrp_running && (idb->atalk_rp & ATALK_IGRP_ENABLED))
		idb->atalk_rp &= ~ATALK_RTMP_ENABLED;
	    else
		printf(atalk_badcmd, "Disabling RTMP without enabling EIGRP"
		       " not permitted");
	}
	break;

      case ATALK_RP_ISIS:		/* not implemented yet */
	break;
      default:
	printf("\n%%Error in route-protocol subcommand in %s @ %d",
	       __FILE__,__LINE__);
	return (FALSE);
    }
    return (TRUE);
}

/*
 * atalkif_ErrForExtended / atalkif_ErrForNonExtended
 * NOTE!! Returns NULL if interface supports specified nets
 */
char*
atalkif_ErrForExtended (atalkidbtype* idb)
{
    char	*msg = NULL;
    hwidbtype	*hwidb;

    if (idb == NULL)
	return (NULL);
    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == NULL)
	return ("hardware interface missing");
    if ((hwidb->status & ATALK_VALID_IDBS) == 0)
	msg = "interface does not support AppleTalk";
    else if ((hwidb->status & ATALK_EXTENDED_IDBS) == 0) {
	if ((hwidb->enctype != ET_SMDS) &&
	    (hwidb->enctype != ET_HDLC) &&
	    (hwidb->enctype != ET_PPP)  &&
	    !is_frame_relay(hwidb) &&
	    !is_atm_dxi(hwidb))
	  msg = "interface does not support extended nets, use ADDRESS";
    }
    return (msg);
}	

char*
atalkif_ErrForNonExtended (atalkidbtype* idb)
{
    char *msg = NULL;
    hwidbtype *hwidb;

    if (idb == NULL)
	return (NULL);
    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == NULL)
	return ("hardware interface is missing");
    if ((hwidb->status & ATALK_VALID_IDBS) == 0)
	msg = "interface does not support AppleTalk";
    else if ((hwidb->status & ATALK_NONEXTENDED_IDBS) == 0)
	msg = "interface does not support non-extended nets, use CABLE-RANGE";
    return (msg);
}	

char*
atalkif_ErrForPeers (atalkidbtype* idb)
{
    char	*msg = NULL;
    hwidbtype	*hwidb;

    if (idb == NULL)
	return (NULL);
    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == NULL)
	return ("hardware interface is missing");
    if ((hwidb->status & ATALK_VALID_IDBS) == 0)
	msg = "interface does not support AppleTalk";
    else if (!(hwidb->status & IDB_SERIAL) || atalkif_IPTalkTunnel(idb))
	msg = "interface does not support half-router";
    else if (is_ddr(hwidb))
	msg = "DDR interface";
    else if (!idb->atalk_sendroutes) 
        msg = "RTMP disabled on interface"; 

    return (msg);
}	

/*
 * atalkif_ErrForDiscovery
 * Return a message if interface cannot support discovery.
 */
char*
atalkif_ErrForDiscovery (atalkidbtype* idb)
{
    char	*msg = NULL;
    hwidbtype	*hwidb;

    if (idb == NULL)
	return (NULL);
    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == NULL)
	return ("hardware interface is missing");
    if ((hwidb->status & ATALK_VALID_IDBS) == 0)
	msg = "interface does not support AppleTalk";
    else if (hwidb->status & ATALK_PEER_IDBS)
	msg = "interface does not support discovery";
    return (msg);
}

/*
 * atalkif_command
 * process Appletalk interface keywords
 */
void atalkif_command (parseinfo *csb)
{
    idbtype *phys_idb;
    hwidbtype	*hwidb;
    atalkidbtype *idb;
    ushort net;
    char* why = NULL;
    ushort rngstart, rngend;
    uchar host;

#define	CHECK_ATALK_ENABLED\
    if (!idb->atalk_enabled) {\
	printf(atalk_badcmd, atalk_not_enabled);\
	return;\
    }

    /* If AppleTalk not running in the router, then bail out now. */
    if (!atalk_running) {
	if (!csb->nvgen) {
	    printf(atalk_not_running);
	}
	return;
    }

    phys_idb = csb->interface;
    idb = atalk_getidb(phys_idb);
    hwidb = hwidb_or_null(phys_idb);
    if (hwidb == NULL) {
	printf("\n%% Hardware interface is missing");
	return;
    }
    if (csb->nvgen) {
	zipifentry* z;
	
	/*
	 * if atalk never configured on this interface, or is simply
	 * not running on this interface, then bail out now.
	 */
	if (!(idb && idb->atalk_enabled)) {
	    return;
	}

	switch (csb->which) {
	  case ATALK_ACCESS:
	    nv_write(idb->atalk_accesslist != 0, "%s %d", 
		     csb->nv_command, idb->atalk_accesslist);
	    break;
	    
	  case ATALK_ADDRESS:
	    if (!atalkif_HalfRouter(idb) && !atalkif_net_extended(idb)
		&& !idb->atalk_allow_clients && !atalkif_IPTalkTunnel(idb)) {
		nv_write((idb->atalknet || idb->atalknode),
			 "%s %d.%d", csb->nv_command,
			 idb->atalknet, idb->atalknode);
	    }
	    break;
	    
	  case ATALK_ARP_TIMEOUT:
	    nv_write((atalkif_ArpTimeout(idb) != AARP_ENTRY_TIMEOUT),
		     "%s %d", csb->nv_command, atalkif_ArpTimeout(idb)/ONEMIN);
	    break;
	    
	  case ATALK_CABLE_RANGE:
	    if (!atalkif_HalfRouter(idb) && atalkif_net_extended(idb)
			&& !idb->atalk_allow_clients) {
		nv_write(TRUE,"%s %d-%d %d.%d",
			 csb->nv_command, idb->atalk_cablestart,
			 (idb->atalk_cablestart == 0) ? 0 : idb->atalk_cableend,
			 idb->atalknet, idb->atalknode);
	    }
	    break;
	    
	  case ATALK_DISTANCE:
	    nv_write(idb->atalk_distance,"%s %d",csb->nv_command,
		     idb->atalk_distance);
	    break;

	  case ATALK_DISCOVERY:
	    nv_write(idb->atalk_discovery, csb->nv_command);
	    break;
	    
	  case ATALK_FTZ_SECURITY:
	    nv_write((idb->atalk_gzlfilter == ATFreeTradeZone), csb->nv_command);
	    break;

	  case ATALK_CLIENT_MODE:
	    nv_write(idb->atalk_allow_clients, csb->nv_command);
	    break;
	    
	  case ATALK_GLEAN:
	    nv_write(idb->aarp_enctype != ET_NULL && idb->atalk_glean,
		     csb->nv_command);
	    break;
	    
	  case ATALK_HALFROUTER:
	    if (atalkif_HalfRouter(idb) && (!atalkif_CaymanTunnel(idb)
					    && !atalkif_AURPTunnel(idb))) {
		nv_write(TRUE, csb->nv_command);
	    }
	    break;

	  case ATALK_PORTDISABLE:
	    /* Nothing is generated if a port is disabled */
	    break;
	    
	  case ATALK_PREFDDITALK:
	    if (hwidb->status & IDB_FDDI && 
	       (hwidb->atalk_compat_flags == AT_COMPAT_FDDI)) {
	          nv_write(TRUE, "%s", csb->nv_command); 
	    }
	    break;

	  case ATALK_ROUTEC:
	    nv_write((hwidb->atalk_routecache == fsUserDisabled),
		     "no %s", csb->nv_command);
	    break;
	    
	  case ATALK_ROUTE_PROTOCOL:	/* do this for IGRP only? */
	    atalkif_cmdrp(csb, idb);
	    break;

	  case ATALK_ROUTEFILTER:
	    nv_write(idb->atalk_route_filter != 0,
		     "%s %d out", csb->nv_command,
		     idb->atalk_route_filter);
	    nv_write(idb->atalk_route_filter_in != 0,
		     "%s %d in", csb->nv_command,
		     idb->atalk_route_filter_in);
	    break;

	  case ATALK_RTMP_SPLIT:
	    /*
	     * Write out this interface command only if split-horizon
	     * processing is turned off (on is default)
	     */
	    if (idb->atalk_rp & ATALK_RTMP_ENABLED) {
		nv_write(!idb->atalk_rtmp_split_horizon, "no %s",
			 csb->nv_command);
	    }
	    break;

	  case ATALK_RTMP_STANDBY:
	    nv_write((idb->atalk_rtmp_standby != 0), "%s %d", csb->nv_command,
		     idb->atalk_rtmp_standby);
	    break;

	  case ATALK_RTMP_STUB:
	    nv_write((idb->atalk_rtmp_stub != 0), "%s", csb->nv_command);
	    break;

	  case ATALK_SENDROUTES:
	    nv_write(!idb->atalk_sendroutes, "no %s", csb->nv_command);
	    break;

	  case ATALK_ZIPFILTER:
	    if (idb->atalk_gzlfilter != ATFreeTradeZone)
		nv_write((idb->atalk_gzlfilter != 0), "%s %d", 
			 csb->nv_command, idb->atalk_gzlfilter);
	    break;
	    
	  case ATALK_ZIPREPLYFILTER:
	    nv_write(idb->atalk_zipreply_filter, "%s %d", csb->nv_command,
		     idb->atalk_zipreply_filter);
	    break;

	  case ATALK_ZONE:
	    if (!idb->atalk_discovery && !atalkif_HalfRouter(idb)
		&& !idb->atalk_allow_clients && !atalkif_IPTalkTunnel(idb)) {
		z = atalkif_firstZone(idb);
		while (z != NULL) {
		    nv_write(TRUE, "%s ", csb->nv_command);
		    atalk_nv_printzone(z->zone->pname);
		    z = atalkif_nextZone(z);
		}
	    }
	    break;
	    
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    /*
     * Need to insure that the atalk idb for this specific interface
     * exists.
     */
    if (atalk_getidb(phys_idb) == NULL) {
	atalk_init_idb(phys_idb);
	if (atalk_getidb(phys_idb) == NULL) {
	    printf("\n%% Cannot create data structures for atalk interface "
		   "information.");
	    return;
	}
	atalk_rxtypes_update(phys_idb);
    }
    idb = atalk_getidb(phys_idb);
    
    /*
     * Group all commands which turn off AppleTalk on an interface here.
     */
    if (csb->sense == FALSE) {
	CHECK_ATALK_ENABLED;
	switch (csb->which) {
	case ATALK_CABLE_RANGE:
	case ATALK_ADDRESS:
	    idb->atalk_enabled = FALSE;
	    atalk_rxtypes_update(phys_idb);
	    atalkif_clear_zones(idb);	/* Fall through! */
	case ATALK_HALFROUTER:
	case ATALK_PORTDISABLE:
	    atalkif_shutdown(idb);      /* Fall through! */
 	    reg_invoke_atalk_if_config(phys_idb, FALSE); /* Tell EIGRP. */
	    atalkif_init(idb, TRUE, TRUE);
	    return;
	default:
	    /* Fall through to switch below */
	    break;
	}
    }

    switch (csb->which) {
      case ATALK_ROUTEFILTER:
	CHECK_ATALK_ENABLED;
	{ 
	    int direction;
	    int value;
	    
	    value = csb->sense ? GETOBJ(int,1) : 0;
	    direction = GETOBJ(int,2);
	    
	    if (direction == IN)
		idb->atalk_route_filter_in = value;
	    else if (direction == OUT)
  		idb->atalk_route_filter = value;
	    atfast_SetupIF(idb);
	    break;
	}
	break;
	
      case ATALK_ZIPFILTER:
	CHECK_ATALK_ENABLED;
	{ 
	    idb->atalk_gzlfilter = csb->sense ? GETOBJ(int,1) : 0;
	    break;
	}
	break;
	
      case ATALK_ZIPREPLYFILTER:
	CHECK_ATALK_ENABLED;
	idb->atalk_zipreply_filter = csb->sense ? GETOBJ(int,1) : 0;
	break;

      case ATALK_SENDROUTES:
	CHECK_ATALK_ENABLED;
	idb->atalk_sendroutes = csb->set_to_default || csb->sense;
	break;

      case ATALK_RTMP_SPLIT:
	atalk_InvalidateCache("atalkif_command");
	idb->atalk_rtmp_split_horizon = csb->sense;
	break;

      case ATALK_RTMP_STANDBY:
	CHECK_ATALK_ENABLED;
	if (idb->atalk_rp & ATALK_RTMP_ENABLED) {
	    TIMER_STOP(idb->atalk_rtmp_sb_lasthrd);
	    idb->atalk_rtmp_standby = csb->sense ? GETOBJ(int,1) : 0;
	} else {
	    printf("\n%% AppleTalk RTMP is not enabled on this interface");
	}
	break;
	
      case ATALK_RTMP_STUB:
	CHECK_ATALK_ENABLED;
	if (!(   atalk_igrp_running 
	      && (idb->atalk_rp & ATALK_IGRP_ENABLED))) 
	    printf("\n%%Warning: AppleTalk EIGRP is not enabled on this interface");

	idb->atalk_rtmp_stub = csb->sense;	

	break;
	
      case ATALK_CLIENT_MODE:
	idb->atalk_allow_clients = csb->sense;
	idb->atalk_enabled = csb->sense;
        idb->atalk_rp = 0;   /* shut off any routing */
	break;
	
      case ATALK_GLEAN:
	CHECK_ATALK_ENABLED;
	if (csb->sense) {
	    if (!is_ieee802(hwidb)) {
		printf(atalk_badcmd,
		       "gleaning not allowed on non-LAN interfaces");
		idb->atalk_glean = FALSE;
	    } else {
		idb->atalk_glean = TRUE;
	    }
	} else {
	    idb->atalk_glean = FALSE;
	}
	break;
	
      case ATALK_DISCOVERY:
	CHECK_ATALK_ENABLED;
	if (csb->sense && (why = atalkif_ErrForDiscovery(idb)) != NULL) {
	    printf(atalk_badcmd, why);
	    break;
	}
	idb->atalk_discovery = csb->sense;
	atalkif_RestartPort(idb);
	break;

      case ATALK_FTZ_SECURITY:
	CHECK_ATALK_ENABLED;
	if (csb->sense)
	    idb->atalk_gzlfilter = ATFreeTradeZone;
	else if (idb->atalk_gzlfilter == ATFreeTradeZone)
	    idb->atalk_gzlfilter = 0;
	atfast_SetupIF(idb);
	if (idb->atalk_gzlfilter == ATFreeTradeZone)
	    atalk_InvalidateCache("atalkif_command");
	break;

      case ATALK_DISTANCE:
	CHECK_ATALK_ENABLED;
	idb->atalk_distance = csb->sense ? GETOBJ(int,1) : 0;
	break;

      case ATALK_ROUTEC:
	CHECK_ATALK_ENABLED;
	hwidb->atalk_routecache = fsUnknown;
	if (csb->sense)
	    atalk_ReinitIF(idb);	/* Give any hardware warnings */
	else
	    hwidb->atalk_routecache = fsUserDisabled;
	atfast_SetupIF(idb);
	atalk_InvalidateCache("atalkif_command");
	break;

      case ATALK_ROUTE_PROTOCOL:
	CHECK_ATALK_ENABLED;
	atalkif_cmdrp(csb, idb);
	break;

      case ATALK_PREFDDITALK:
  	if (csb->sense)
 	    hwidb->atalk_compat_flags = AT_COMPAT_FDDI;
  	else
 	    hwidb->atalk_compat_flags = 0;
 	
 	if (idb != NULL && (hwidb->status & IDB_FDDI)){
 	    ziptabletype *zone;
 	    uchar const *mc;
 
 	    zone = atalkif_primary_zone(idb);
 	    if (zone != NULL)
 	       mc = atalk_zone_multicast(idb, atalk_zonesum(zone->name,zone->length));
 	    else
 	       mc = atalk_zone_multicast(idb, 0);
 
 	    ieee_copy(mc, idb->atalk_zone_multicast);
 	    atalkif_clear(phys_idb);
 	}
	break;

      case ATALK_ACCESS:
	CHECK_ATALK_ENABLED;
	idb->atalk_accesslist = csb->sense ? GETOBJ(int,1) : 0;
	atfast_SetupIF(idb);
	if (idb->atalk_accesslist != 0)
	    atalk_InvalidateCache("atalkif_command");
	break;

      case ATALK_ZONE:
	if (idb->atalk_allow_clients) {
	    printf(atalk_badcmd, "Cannot set zone for an interface",
			" in client-mode.");
	    break;
	}
	atalkif_cmdzone(csb, idb);
	break;

      case ATALK_CABLE_RANGE:
	if (idb->atalk_allow_clients) {
	    printf(atalk_badcmd, "Cannot set cable-range for an interface",
			" in client-mode.");
	    break;
	}
	why = atalkif_ErrForExtended(idb);
	if (why != NULL) {
	    printf(atalk_badcmd,why);
	    break;
	}
	rngstart = (ushort)GETOBJ(int,1);
	rngend = (ushort)GETOBJ(int,2);
	
	if (GETOBJ(paddr,1)->type == ADDR_APPLE) {
	    net = GETOBJ(paddr,1)->apple_addr.net;
	    host = GETOBJ(paddr,1)->apple_addr.host;
	} else {
	    net = 0;
	    host = 0;
	}
	if (rngend == 0)
	    rngend = ATALK_RANGE_UNKNOWN; 
	if (!atalkif_net_extended(idb))
	    atalkif_shutdown(idb);
	if (atalkif_setup(idb,rngstart,rngend,net,host, TRUE))
	    atalkif_clear_zones(idb);
	break;

      case ATALK_ADDRESS:
	if (idb->atalk_allow_clients) {
	    printf(atalk_badcmd, "Cannot set address for an interface",
			" in client-mode.");
	    break;
	}
	why = atalkif_ErrForNonExtended(idb);
	if (why != NULL) {
	    printf(atalk_badcmd,why);
	    break;
	}
	net = GETOBJ(paddr,1)->apple_addr.net;
	host = GETOBJ(paddr,1)->apple_addr.host;
	if (net == 0) 
	    idb->atalk_discovery = TRUE;
	atalkif_setup(idb,net,0,net,host, TRUE);
	atalk_rxtypes_update(phys_idb);
	break;

      case ATALK_HALFROUTER:
	if (csb->sense == FALSE) {
	    atalkif_shutdown(idb);
	    break;
	}
	why = atalkif_ErrForPeers(idb);
	if (why != NULL) {
	    printf(atalk_badcmd,why);
	    break;
	}
	atalkif_shutdown(idb);
	atalkif_setup(idb,ATALK_NET_UNNUMBERED,
		      ATALK_NET_UNNUMBERED,ATALK_NET_UNNUMBERED,0, TRUE);
	break;

      case ATALK_ARP_TIMEOUT:
	if (csb->sense == FALSE) {
	    atalkif_SetAarpTimeout(idb, AARP_ENTRY_TIMEOUT);
	} else {
	    atalkif_SetAarpTimeout(idb, (GETOBJ(int, 1)*ONEMIN));
	}
	break;

	
      case ATALK_PORTDISABLE:
	/* Fall thru */
	;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}
