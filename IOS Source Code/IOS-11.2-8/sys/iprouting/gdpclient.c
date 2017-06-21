/* $Id: gdpclient.c,v 3.6.20.6 1996/09/10 01:07:35 snyder Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/gdpclient.c,v $
 *------------------------------------------------------------------
 * gdpclient.c -- discover routers in the world by various means.
 *
 * September 1990, By Bill Westfield
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gdpclient.c,v $
 * Revision 3.6.20.6  1996/09/10  01:07:35  snyder
 * CSCdi68568:  more constant opportuniites
 *              140 from image, 52 from data
 * Branch: California_branch
 *
 * Revision 3.6.20.5  1996/08/28  12:52:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.20.4  1996/08/07  09:00:42  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.20.3  1996/06/28  23:18:52  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.20.2  1996/04/15  21:17:51  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.6.20.1  1996/03/18  20:27:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/07  09:49:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  01:02:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/01  06:05:20  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/24  20:16:30  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4  1996/01/22  06:14:12  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/17  17:33:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:48:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:07:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/03  07:42:18  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.2  1995/06/28 03:21:10  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.1  1995/06/07 21:06:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../iprouting/gdp.h"
#include "../iprouting/route.h"
#include "../iprouting/rip.h"
#include "../iprouting/irdp.h"
#include "../iprouting/igrp.h"
#include "../iprouting/iprouting_debug.h"
#include "parser_defs_gdp.h"
#include "../os/signal.h"

/*
 * local storage
 */
static ipsocktype *ripsoc;
static sys_timestamp gdp_clienttimer;
static sys_timestamp gdp_defaulttimer;
static char *const gdpstrings[]= {"unknown", "GDP", "RIP", "IGRP", "IRDP"};
static queuetype gdp_routerQ;
pid_t gdp_client_pid;

watched_queue *gdp_clientQ;
boolean gdp_enable, gdp_igrp_enable;
boolean gdp_irdp_enable, gdp_rip_enable, gdp_gdp_enable;

/*
 * forward references
 */
static gdp_candidate *find_gdpcandidate (ipaddrtype address);
static void gdpclient_update (gdp_candidate *entry, int source, int priority,
		       idbtype *source_idb);

/*
 * gdpclient_process_input
 *
 * Process any input on either the GDP Client queue or on the RIP queue.
 */
static void gdpclient_process_input (void)
{
    paktype *pak;
    iphdrtype *ip;
    udptype *udp;
    riptype *rip;
    igrptype *igrp;
    icmprdptype *icmp;
    int i;
    int dummy;
    gdp_candidate *entry;
    irdpinfo *rinfo;
    gdptype *gdp;
    gdpinfo *info;
    
    if (gdp_rip_enable && ripsoc)
	while ((pak = read_ipsocket(ripsoc,FALSE,0, &udp,&dummy))) {
	    ip = (iphdrtype *)ipheadstart(pak);
	    rip = (riptype *) udp->udpdata;
	    if (rip->opcode == RIPCMD_REPLY) {
		entry = find_gdpcandidate(ip->srcadr);
		gdpclient_update(entry, GDP_SOURCE_RIP, GDP_RIP_PRIORITY,
				 pak->if_input);
	    }
	    datagram_done(pak);
	}
    while ((pak = process_dequeue(gdp_clientQ))) {
	ip = (iphdrtype *)ipheadstart(pak);
	switch (ip->prot) {
	  case UDP_PROT:
	    udp = (udptype *) (ipdatastart(pak));
	    if (udp->dport == GDP_PORT) {
		gdp = (gdptype *) udp->udpdata;
		if (gdp->version == GDP_VERSION &&
		    gdp->opcode == GDP_OPC_REPORT) {
		    info = (gdpinfo *) (gdp + 1);
		    for (i=0; i < gdp->count; i++) {
			if (pak->if_input == samecable(info->address)) {
			    entry = find_gdpcandidate(info->address);
			    gdpclient_update(entry, GDP_SOURCE_GDP,
					     info->priority, pak->if_input);
			    if (entry) {
				TIMER_START(entry->expiretime,
				    info->holdtime * ONESEC);
				if (entry->interval == 0)
				    entry->interval =
					(info->holdtime*ONESEC) >> 1;
			    }
			}
			info++;
		    }
		}
	    }
	    break;

	  case IGRP_PROT:
	  case NEWIGRP_PROT:
	    if (gdp_igrp_enable) {
		igrp = (igrptype *) ipdatastart(pak);
		if (igrp->opcode == IGRP_OPC_UPDATE)
		    gdpclient_update(find_gdpcandidate(ip->srcadr),
				     GDP_SOURCE_IGRP, GDP_IGRP_PRIORITY,
				     pak->if_input);
	    }
	    break;
	    
	  case ICMP_PROT:
	    icmp = (icmprdptype *) ipdatastart(pak);
	    /* ICMP type should have already been checked... */
	    if (icmp->type != ICMP_RDPADVERT || icmp->code != 0)
		break;
	    rinfo = (irdpinfo *) icmp->data;
	    for (i=0; i < icmp->num_addrs; i++) {
		if (pak->if_input == samecable(rinfo->address)) {
		    entry = find_gdpcandidate(rinfo->address);
		    gdpclient_update(entry, GDP_SOURCE_IRDP,
				     rinfo->preference, pak->if_input);
		    if (entry) {
			TIMER_START(entry->expiretime, icmp->lifetime*ONESEC);
			if (entry->interval == 0)
			    entry->interval = (icmp->lifetime*ONESEC) >> 1;
		    }
		}
		rinfo++;
	    }
	    break;
	    
	  default:
	    break;
	}
	datagram_done(pak);
    }
}

/*
 * gdpclient_process_timers
 *
 * Process the various GDP timers.
 */
static void gdpclient_process_timers (void)
{
    gdp_candidate *entry, *next;

    if (AWAKE(gdp_clienttimer)) {
	TIMER_START(gdp_clienttimer, 10*ONESEC);
	for (entry= (gdp_candidate *) gdp_routerQ.qhead; entry;
	     entry = next) {
	    next = entry->next;
	    if (entry->interval && AWAKE(entry->expiretime)) {
		unqueue(&gdp_routerQ, entry);
		if (route_debug)
		    buginf("\nGDPclient: router %i via %s, EXPIRED",
			   entry->address, gdpstrings[entry->source]);
		if (entry->address == default_gateway) {
		    default_gateway = gdp_pickrouter(default_gateway);
		    reg_invoke_ip_defaultgw_change(default_gateway);
		    if (route_debug)
			buginf("\nGDPclient: new default gateway is %i",
			       default_gateway);
		}
		free(entry);
	    }
	}
	if (AWAKE(gdp_defaulttimer) || (default_gateway == 0)) {
	    TIMER_START(gdp_defaulttimer, 5*ONEMIN);
	    default_gateway = gdp_pickrouter(0);
	    reg_invoke_ip_defaultgw_change(default_gateway);
	}
    }
}

/*
 * gdp_client_teardown 
 *
 * This process is being killed.  Remove any data structures that this
 * process set up when it started.
 */
static void gdp_client_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    gdp_candidate *entry;
    paktype *pak;

    /* free up GDP data space */
    process_watch_timer(&gdp_clienttimer, DISABLE);
    while ((entry = dequeue(&gdp_routerQ)) != NULL)
	free(entry);
    process_watch_queue(gdp_clientQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(gdp_clientQ)))
	datagram_done(pak);
    delete_watched_queue(&gdp_clientQ);
}

/*
 * gdp_client
 * Listen for GDP packets on the network
 */

static process gdp_client (void)
{
    ulong major, minor;
    
    TIMER_START(gdp_clienttimer, 30*ONESEC);
    COPY_TIMESTAMP(gdp_clienttimer, gdp_defaulttimer);
    
    signal_permanent(SIGEXIT, gdp_client_teardown);
    gdp_clientQ = create_watched_queue("GDP Packets", 0, 0);
    process_watch_queue(gdp_clientQ, ENABLE, RECURRING);
    process_watch_timer(&gdp_clienttimer, ENABLE);
    if (ripsoc)
	process_watch_queue(ripsoc->inq, ENABLE, RECURRING);

    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		gdpclient_process_input();
		break;

	      case TIMER_EVENT:
		gdpclient_process_timers();
		break;

	      case DIRECT_EVENT:
		/*
		 * Configuration changed.  This code only needs to be
		 * worried about the RIP socket being added.  If it was
		 * removed, then close_ipsocket call would automatically
		 * remove the watch.
		 */
		if (ripsoc)
		    process_watch_queue(ripsoc->inq, ENABLE, RECURRING);
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

#define TOSEC(x) (((x)+ONESEC/2)/ONESEC)

static void gdpclient_update (gdp_candidate *entry, int source, int priority,
		       idbtype *source_idb)
{
    int dummy;
    int time_interval = 0;
    
    if (!entry)
	return;
    
    if (entry->interval == 0) {
	if (TIMER_RUNNING(entry->lastupdate)) {
	    time_interval = ELAPSED_TIME(entry->lastupdate);
	    if (time_interval > GDP_MIN_INTERVAL) {
		entry->interval = time_interval;
		TIMER_START(entry->expiretime, (entry->interval<<1) +
		    (entry->interval>>1));
	    } else
		time_interval = 1;
	} else if (route_debug)
	    buginf("\nGDPclient: %i initial message", entry->address);
    } else {
	if (entry->source != source && source != GDP_SOURCE_IRDP)
	    return;
	/*
	 * Smooth the expected inter-update interval
	 * interval = .875 * oldinterval + .125*newinterval
	 */
	dummy = entry->interval;
	time_interval = ELAPSED_TIME(entry->lastupdate);
	if (time_interval > GDP_MIN_INTERVAL) {
	    entry->interval = (dummy * 7 + time_interval)>>3;
	    TIMER_START(entry->expiretime, (entry->interval<<1) +
		(entry->interval>>1));
	} else time_interval = 1;
    }
    GET_TIMESTAMP(entry->lastupdate);
    entry->priority = priority;
    entry->source = source;
    entry->source_idb = source_idb;
    
    if (route_debug)
	buginf("\nGDPclient: %i via %s, interval %d, exp %#Ta, priority %d %s",
	       entry->address, gdpstrings[source], TOSEC(entry->interval),
	       entry->expiretime, priority,
	       time_interval == 1 ? "(ignored)" : "");
}

/*
 * find_gdpcanditate
 * Look in our list of candidates to see if we already know about this
 * address.
 */

static gdp_candidate *find_gdpcandidate (ipaddrtype address)
{
    gdp_candidate *entry;
    
    for (entry= (gdp_candidate *) gdp_routerQ.qhead; entry; entry= entry->next)
	if (entry->address == address)
	    break;
    if (!entry) {
	entry = malloc(sizeof (struct gdp_candidate));
	if (entry) {
	    entry->address = address;
	    enqueue(&gdp_routerQ, entry);
	}
    }
    return(entry);
}


/*
 * Pick a new default router based on GDP.
 * Try to pick something other than the supplied address, which is what
 * we are using currently (eg if a TCP connection is about to time out)
 */
ipaddrtype gdp_pickrouter (ipaddrtype address)
{
    gdp_candidate *entry, *current = NULL, *best = NULL;
    
    for (entry= (gdp_candidate *)gdp_routerQ.qhead; entry;
	 entry= entry->next) {
	if (entry->address == address) {
	    current = entry;
	    continue;
	}
	
	if (gdp_irdp_enable) {
	    if (best && (best->source == GDP_SOURCE_IRDP) &&
		(entry->source != GDP_SOURCE_IRDP))
		continue;
	    if (entry->priority == MININT)	/* -2^32 = black hole */
		continue;
	    if (!best) 
		best = entry;
	    else if (entry->source == GDP_SOURCE_IRDP) {

		/*
		 * Both entry and best are IRDP.  Lower wins.
		 */
		if (entry->priority < best->priority)
		    best = entry;
		if (best->source != GDP_SOURCE_IRDP)
		    best = entry;
	    } else if (entry->priority > best->priority)
		best = entry;
	} else
	    if (!best || entry->priority > best->priority)
		best = entry;
    }

    if (!best) {
	if (current)
	    return (current->address);
	else return (0L);
    }

    if (route_debug)
	buginf("\nGDPclient: picked new default router %i", best->address);

    return(best->address);    
}

/*
 * ip_gdp_command  (for non-routers)
 * Parse the GDP keyword of the ip subcommand of the interface command
 *    [no] ip gdp
 *	   ip gdp rip
 *	   ip gdp igrp
 *	   ip gdp gdp
 */

void ip_gdpclient_command (parseinfo *csb)
{
    if (csb->nvgen) {
	switch (csb->which) {
	case GDP_CMD_GDP:
	    nv_write(gdp_gdp_enable, csb->nv_command);
	    break;
	case GDP_CMD_IGRP:
	    nv_write(gdp_igrp_enable, csb->nv_command);
	    break;
	case GDP_CMD_RIP:
	    nv_write(gdp_rip_enable, csb->nv_command);
	    break;
	case GDP_CMD_IRDP:
	    nv_write(gdp_irdp_enable, csb->nv_command);
	    break;
	case GDP_CMD_IRDP_MULTICAST:
	    nv_write(gdp_irdp_multicast, csb->nv_command);
	    break;
	}
	return;
    }
    
    /* Don't allow us to enable a GDP client if we're routing */
    if (ip_router_running) {
	printf("%%Cannot start GDP client; system is routing IP");
	return;
    }
    
    if (csb->sense && !gdp_enable)
	gdp_client_pid = process_create(gdp_client, "Router Discovery",
					NORMAL_STACK, PRIO_NORMAL);
    
    switch (csb->which) {
	
    case GDP_CMD_GLOBAL:
	/*
	 * For backwards compatibility, 'no ip gdp' turns off all gdp 
	 * processing and 'ip gdp' acts like 'ip gdp gdp'.
	 */
	if (!csb->sense)
	    gdp_gdp_enable = gdp_igrp_enable = 
		gdp_rip_enable  = gdp_irdp_enable = gdp_irdp_multicast = FALSE;
	else
	    gdp_gdp_enable = csb->sense;
	return;
	
    case GDP_CMD_RIP:
	gdp_rip_enable = csb->sense;
	if (csb->sense) {
	    ripsoc = open_ipsocket(UDP_PROT, NULL, NULL, RIP_PORT, RIP_PORT, 
				   SOCK_ANYSRC);
	    if (!ripsoc) {
		if (route_debug)
		    buginf("\nGDP client: Can't open RIP socket");
	    } else {
		process_wakeup(gdp_client_pid);
	    }

	} else if (ripsoc)
	    close_ipsocket(ripsoc);
	break;
	
    case GDP_CMD_IGRP:
	gdp_igrp_enable = csb->sense;
	break;
	
    case GDP_CMD_GDP:
	gdp_gdp_enable = csb->sense;
	break;
	
    case GDP_CMD_IRDP:
	if (csb->sense && !gdp_irdp_enable)
	    send_irdpsolicit(csb->interface);
	gdp_irdp_enable = csb->sense;
	if (!csb->sense)
	    gdp_irdp_multicast = FALSE;
	break;
	
    case GDP_CMD_IRDP_MULTICAST:
	gdp_irdp_multicast = csb->sense;
	break;
	
    default:
	break;
    }
    gdp_enable = gdp_gdp_enable || gdp_igrp_enable || gdp_rip_enable ||
	gdp_irdp_enable;
    if (!gdp_enable)
	process_kill(gdp_client_pid);
}

/*
 * show_gdpclient
 * Show the gateways we have learned about
 */

static void show_gdpclient (void)
{
    gdp_candidate *entry;
    static const char gdphead[] =
	"Gateway         Using  Interval  Priority   Interface";
    /*   111.111.111.111 IGRP+++...123++++...500 */
    
    if (!gdp_enable)
	return;
    automore_header(gdphead);
    for (entry= (gdp_candidate *)gdp_routerQ.qhead; entry;
	 entry= entry->next) {
	
	printf("\n%15i %4s   %-6d    %-6d     %15s", entry->address,
	       gdpstrings[entry->source], (entry->interval+ONESEC/2)/1000,
	       entry->priority,
	       entry->source_idb ? entry->source_idb->namestring : "Unknown");
    }
    automore_header(NULL);
    printf("\n");
}

/*
 * gdpclient_goingdown
 * Here from SERVICE_SWIF_GOINGDOWN
 */

static void gdpclient_goingdown (idbtype *idb)
{
    gdp_candidate *entry, *next;
    
    for (entry= (gdp_candidate *) gdp_routerQ.qhead; entry; entry = next) {
	next = entry->next;
	if (entry->source_idb == idb) {
	    unqueue(&gdp_routerQ, entry);
	    if (route_debug)
		buginf("\nGDPclient: router %i via %s, %s interface down",
		       entry->address, gdpstrings[entry->source],
		       idb->namestring);
	    if (entry->address == default_gateway) {
		default_gateway = gdp_pickrouter(default_gateway);
                reg_invoke_ip_defaultgw_change(default_gateway);
		if (route_debug)
		    buginf("\nGDPclient: new default gateway is %i",
			   default_gateway);
	    }
	    free(entry);
	}
    }
}

/*
 * gdpclient_comingup
 * Here from SERVICE_SWIF_COMINGUP
 */

static void gdpclient_comingup (idbtype *idb)
{
    if (interface_up_simple(idb)) {
	if (gdp_irdp_enable)
	    send_irdpsolicit(idb);
    }
}

/*
 * gdpclient_enqueue
 * Queue packet for processing by GDP client process.
 */

static void gdpclient_enqueue (paktype *pak)
{
    if (!gdp_irdp_enable) {
	retbuffer(pak);
	return;
    }
    process_enqueue_pak(gdp_clientQ, pak);
}

/*
 * gdpclient_enable
 * Is the GDP client process enabled?
 */

static boolean gdpclient_enable (void)
{
    return(gdp_enable);
}

/*
 * gdpclient_init
 * Initialize gdp client support
 */

void gdpclient_init (void)
{
    /*
     * Initialize host side
     */
    gdp_clientQ = NULL;
    queue_init(&gdp_routerQ, 0);
    gdp_rip_enable = gdp_igrp_enable  = FALSE;
    gdp_irdp_enable = gdp_enable = FALSE;
    reg_add_swif_comingup(gdpclient_comingup, "gdpclient_comingup");
    reg_add_swif_goingdown(gdpclient_goingdown, "gdpclient_goingdown");
    reg_add_ip_gdpclient_enqueue(gdpclient_enqueue, "gdpclient_enqueue");
    reg_add_ip_gdpclient_enable(gdpclient_enable, "gdpclient_enable");
    reg_add_ip_gdpclient_show(show_gdpclient, "show_gdpclient");
    reg_add_ip_gdpclient_pick_defaultgw(gdp_pickrouter, "gdp_pickrouter");
    gdpclient_parser_init();
}
