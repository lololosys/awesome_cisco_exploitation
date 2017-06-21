/* $Id: apollo.c,v 3.9.18.6 1996/08/30 20:44:39 dsavage Exp $
 * $Source: /release/112/cvs/Xsys/xns/apollo.c,v $
 *------------------------------------------------------------------
 * apollo.c -- handle Apollo domain protocols, a variant of XNS.
 *
 * Summer 1989, By Bill Westfield
 *
 * Copyright (c) 1989-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: apollo.c,v $
 * Revision 3.9.18.6  1996/08/30  20:44:39  dsavage
 * CSCdi61420:  ip directed-broadcast -65537
 * Branch: California_branch
 * incorrect swidb type referenced for idb_release
 *
 * Revision 3.9.18.5  1996/08/03  23:43:45  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.9.18.4  1996/07/26  00:54:19  hampton
 * Fix the automore code in show access-list.  [CSCdi61084]
 * Branch: California_branch
 *
 * Revision 3.9.18.3  1996/06/28  23:33:12  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.9.18.2  1996/05/30  23:47:34  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.9.18.1  1996/03/18  22:52:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.16.3  1996/03/13  02:14:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.16.2  1996/03/07  11:21:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.16.1  1996/02/20  21:58:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/08  20:15:40  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.8  1996/02/07  16:18:10  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/01  06:13:21  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  07:31:40  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1995/12/07  22:34:41  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/11/22  09:18:12  hampton
 * Fix the mips routine to read/write unaligned longlong words.  While
 * there, rework the entire family of routines to reduce overhead.  This
 * necessitates a change to code calling these routines.  [CSCdi44257]
 *
 * Revision 3.3  1995/11/17  19:20:25  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:53:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:56:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/09/11  23:58:18  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.6  1995/06/20 21:53:13  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.5  1995/06/18  06:19:44  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.4  1995/06/18 02:24:15  hampton
 * Change the name of an XNS timer variable to something that makes its
 * usage clearer.  While there, remove direct references to msclock.
 * [CSCdi36038]
 *
 * Revision 2.3  1995/06/16 17:10:02  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Eliminate obsolete macro references.
 *
 * Revision 2.2  1995/06/09  13:25:40  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:26:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "interface.h"

#include "packet.h"
#include "ieee.h"
#include "../if/arp.h"
#include "../if/network.h"
#include "config.h"
#include "xns.h"
#include "apollo.h"
#include "parser.h"
#include "parser_defs_apollo.h"
#include "../iprouting/route.h"
#include "../ui/common_strings.h"

static void apollo_route_command(parseinfo *csb, rip_routetable *table, 
			  int linktype, uchar *address);

queuetype apollobgQ;			/* Apollo services queue */
queuetype apollo_ripQ;			/* packet queue for apollo routing */
queuetype apolloechoQ;			/* packet queue for apollo ping */
queuetype apollo_accQ;			/* Queue of access lists */

ulong apollo_address;
uchar apollo_etheraddr[IEEEBYTES];
boolean apollo_running;			/* if running Apollo */
boolean apollo_started;			/* Apollo has been started once */
rip_routetable *apollo_table;		/* routing table for apollo */
char *apolloname = "Apollo";
/*
 * Routing variables
 */
sys_timestamp	apollo_changed;
ulong		apollo_routes;
sys_timestamp	apollo_timer;
ulong		apollo_update_interval;

static const char *apollo_badaddr = "\nIllegal Apollo Domain address - %s";


/*
 * Apollo support
 * Handle apollo's "Service advertisement protocol" - routers are suppose
 * to keep track of all the servers out on the network, as well as specially
 * handling forwarding of service advertisements.
 */

static void
apollo_routing_unconfig (void)
{
    apollo_running = FALSE;
}

static void

apollo_routing_config (parseinfo *csb)
{
    long	addr;
    ulong	idblist_type;
    idbtype	*idb;
    apollosb	*asb;
    list_element *l_elt;
    list_header	*l_hdr;

    addr = GETOBJ(int,1);
    if (addr <= 0 || addr > APOLLOMAXADDR) {
	printf(apollo_badaddr, "malformed hex address");
	return;
    }
    apollo_address = addr;
    bcopy((uchar *)&apollo_address, &apollo_etheraddr[2], APOLLOADDRBYTES);

    /*
     * See if our IDB list was ever created. If it was, then get
     * hold of it again; otherwise, create it for the first time.
     */
    l_hdr = idb_get_listhead(SWIDBLIST_APOLLO);
    if (l_hdr == NULL) {
	if ( !idb_create_list(SWIDBLIST_APOLLO, &idblist_type)
	    || idblist_type != SWIDBLIST_APOLLO) {
		    idb_report_create_error("Apollo");
	    return;
	}
    }

    apollo_running = TRUE;
    TIMER_START(apollo_timer, 0);
    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_APOLLO, l_elt, idb) {
	asb = idb_get_swsb_inline(idb, SWIDB_SB_APOLLO);
	if (asb && asb->apollonet)
	    COPY_TIMESTAMP(apollo_timer, asb->xnsrip.updatetimer);
    }

    if (!apollo_started)
	apollo_start();
}

/*
 * apollo_command
 * Process Apollo commands
 */


void apollo_command (parseinfo *csb)
{
    long	addr;
    apollosb	*asb;
    list_element	*l_elt;
    idbtype	*idb;

    if (csb->nvgen) {
	int i;
	ulong host;
	xns_ripentrytype *p;
	pathentrytype *pp;
	apollo_acclisttype *acclist;
	apollo_acctype *acc;

	if (!apollo_running)
	    return;

	switch (csb->which) {

	  case APOLLO_ACCESSL:	/* apollo access-list */
	    for (acclist = (apollo_acclisttype *) apollo_accQ.qhead; acclist;
		 acclist = acclist->next) {
		    for (acc = (apollo_acctype *) acclist->q.qhead; 
			 acc; 
			 acc = acc->next)
		    nv_write(TRUE, "%s %s %s %x-%x.%x %x",
			    csb->nv_command,
			    acclist->name, acc->result? "permit" : "deny",
			    acc->firstnet, acc->lastnet, acc->host, acc->hostmask);
	    }
	    break;

	  case APOLLO_PATHS:	/* apollo maximum-paths */
	    nv_write((apollo_maxpaths != RIP_MAXPATHS), "%s %d",
		     csb->nv_command, apollo_maxpaths);
	    break;

	  case APOLLO_ROUTE:	/* apollo route */
	    for (i = 0; i < NETHASHLEN; i++) {
		p = (*apollo_table)[i];

		for (; p; p = p->next) {
		    pp = p->path_list;
		    /* Step thru the path tables for all paths */
		    bcopy(&pp->srchost[2], (uchar *)&host, 4);
		    nv_write(p->flags & NET_STATIC,
			    "%s %x %x.%x", csb->nv_command,
			     p->dstnet, pp->srcnet, host);
		}
	    }
	    break;

	  case APOLLO_ROUTING:	/* apollo routing */
	    nv_write(TRUE, "%s %x", 
		     csb->nv_command, apollo_address);
	    break;


	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    /*
     * Process given args
     */
    switch (csb->which) {

      case APOLLO_ACCESSL: {
	apollo_acclisttype *acclist;
	apollo_acctype *acc;
	idbtype *idb;
	ulong firstnet, lastnet;
	ulong host;
	boolean result;

	acclist = find_namedthing(&apollo_accQ, GETOBJ(string,1));
	if (acclist == NULL) {
	    acclist = malloc(sizeof(apollo_acclisttype));
	    if (acclist == NULL)
		return;
	    sstrncpy(acclist->name, GETOBJ(string,1), NAMEDTHINGNAMELEN);
	    enqueue(&apollo_accQ, acclist);
	}
	if (csb->sense == FALSE) {
	    unqueue(&apollo_accQ, acclist);
	    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_APOLLO, l_elt, idb) {
		asb = idb_get_swsb_inline(idb, SWIDB_SB_APOLLO);
		if (asb && asb->apollo_acclist == acclist)
		    asb->apollo_acclist = NULL;
	    }
	    for (acc = (apollo_acctype *)acclist->q.qhead; acc;) {
		addr = (ulong) acc->next;
		free(acc);
		acc = (apollo_acctype *)addr;
	    }
	    return;
	}
	
	/* GETOBJ(int,1) == TRUE for 'permit', FALSE for 'deny' */
	result = GETOBJ(int,1);

	/*
	 * Get apollo address
	 */
	lastnet = GETOBJ(paddr,1)->apollo_addr.net;	/* net part */

	/* If firstnet was specified, use it, otherwise make it
	 * the same as lastnet.
	 */
	if (GETOBJ(int,3)) {
	    firstnet = GETOBJ(int,2);
	} else {
	    firstnet = lastnet;
	}
	/* host part */
	bcopy(&GETOBJ(paddr,1)->apollo_addr.host[2], (uchar *)&host, 4);

	if (GETOBJ(int,5)) {				/* mask */
	    addr = GETOBJ(int,4);
	} else {
	    addr = 0;
	}

	acc = malloc(sizeof(apollo_acctype));
	if (acc) {
	    acc->result = result;
	    acc->firstnet = firstnet;
	    acc->lastnet = lastnet;
	    acc->host = host;
	    acc->hostmask = addr;
	    enqueue(&acclist->q, acc);
	}
	break;
      }	

      case APOLLO_ROUTING:
	if (!csb->sense) {
	    apollo_routing_unconfig();
	} else {
	    apollo_routing_config(csb);
	}
	FOR_ALL_SWIDBS(idb)
	    apollo_rxtypes_update(idb);
	break;

    case APOLLO_ROUTE:
	if (!apollo_running)
	    return;
	apollo_route_command(csb, apollo_table, LINK_APOLLO, apollo_etheraddr);
	break;

    case APOLLO_PATHS:
	if (!csb->sense) {
	    apollo_maxpaths = RIP_MAXPATHS;
	    return;
	}
	if (GETOBJ(int,1))
	    apollo_maxpaths = GETOBJ(int,1);
	else 
	    apollo_maxpaths = RIP_MAXPATHS;

	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * apollo_route_command
 * Parse a command for a static apollo routuing command, eg:
 *    apollo route CAFE COFFEE.12345
 */

static void apollo_route_command (parseinfo *csb, rip_routetable *table, 
			   int linktype, uchar *address)
{
    idbtype		*idb;
    list_element	*l_elt;
    uchar		host[IEEEBYTES];
    ulong		dstnet;
    ulong		net;
    apollosb		*asb;

    if (!table) {
	printf(apollo_badaddr, "non-existent routing table");
	return;
    }
    dstnet = GETOBJ(int,1);
    if (dstnet == 0) {
	printf(apollo_badaddr, "bad network number");
	return;
    }
    if (!csb->sense) {
	grip_del_rt(dstnet, FALSE, table, apollo_running, 
		    apollorip_debug);
	return;
    }

    net = GETOBJ(paddr,1)->apollo_addr.net;
    bcopy((char *)(GETOBJ(paddr,1)->apollo_addr.host),
	  (char *)(host), ADDRLEN_APOLLO);

    if (ieee_equal(address, host))
	return;
    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_APOLLO, l_elt, idb) {
	asb = idb_get_swsb_inline(idb, SWIDB_SB_APOLLO);
	if (asb && asb->apollonet == net)
	    break;
    }
    if (idb == NULL) {
	printf(apollo_badaddr, "must be on connected network");
	return;
    }
    asb = idb_use_swsb_inline(idb, SWIDB_SB_APOLLO);
    COPY_TIMESTAMP(apollo_timer, asb->xnsrip.updatetimer);
    grip_add_rt(&asb->xnsrip, dstnet, net, host, NET_STATIC, table, 
		apollo_running, FALSE, apollorip_debug);
    asb->xnsrip.static_rts++;
    idb_release_swsb_inline(idb, SWIDB_SB_APOLLO);
}

static void
apollo_if_net_deconfig (parseinfo *csb)
{
    apollosb	*asb;

    if (!csb || csb->interface == NULL)
	return;

    asb = idb_get_swsb(csb->interface, SWIDB_SB_APOLLO);
    if (!asb)
	return;

    grip_del_rt(asb->apollonet, TRUE, apollo_table,
		apollo_running,  apollorip_debug);
    if (!idb_delete_swsb(csb->interface, SWIDB_SB_APOLLO)) {
	printf("\nApollo: error removing sb from IDB %s", 
	       csb->interface->namestring);
	return;
    } else {
	free(asb);
    }
    if (!idb_remove_swidb_from_list(csb->interface, SWIDBLIST_APOLLO)) {
	printf("\nAPOLLO: failed to remove APOLLO idb %s from APOLLO list",
	       csb->interface->namestring);
    }
    /*
     * If this was a 802.foobar interface, remove the Apoloo<>MAC
     * mapping from the ARP table.
     */
    if (csb->interface->hwptr->status & IDB_IEEEMASK)
	arp_table_remove(apollo_address, LINK_APOLLO);
}

/*
 * apollo_if_net_config
 *
 * Configure an Apollo network # on an interface.
 */
static void
apollo_if_net_config (parseinfo *csb)
{
    apollosb	*asb1;			/* used in config check loop */
    apollosb	*asb;			/* sb from csb->interface */
    idbtype	*idb1;			/* user in config check loop */
    idbtype	*idb;
    ulong	network;
    list_element *l_elt;
    list_header *list_head;		/* ptr to head of Apollo idb list */
    swidb_sb_t	type = SWIDB_SB_APOLLO;

    if (csb == NULL || csb->interface == NULL)
	return;

    if (csb->interface->hwptr->status & IDB_TR) {
	printf("\nApollo routing is not supported on Token Ring interfaces.");
	return;
    }
    network = GETOBJ(int,1);
    idb = csb->interface;
    list_head = idb_get_listhead(SWIDBLIST_APOLLO);
    if (list_head == NULL) {
	printf("\nApollo: Could not find IDB listhead"); /* ??? */
	return;				/* we have a _big_ problem */
    }
    /*
     * See if there is a Apollo sub-block on this idb;
     * if there is, we most likely have a current configuration
     * on the interface.
     */
    asb = idb_get_swsb(idb, SWIDB_SB_APOLLO);
    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_APOLLO, l_elt, idb1) {
	asb1 = idb_get_swsb_inline(idb, SWIDB_SB_APOLLO);
	if (!asb1 || asb->apollonet == 0)
	    continue;
	/*
	 * If we find an interface with the network # in question
	 * configured on an interface other than the one we're
	 * trying to configure, report a conflict.
	 */
	if ((asb1->apollonet == network) && (idb1 != idb)) {
	    printf("\nApollo network %x already exists on interface %s",
		   asb1->apollonet, idb1->namestring);
	    return;
	}
    }

    /*
     * Delete any route which is currently configured on the interface
     * we wish to configure. Then delete any routing entry for the
     * network we wish to add to the configuration.
     */
    if (asb) {
	grip_del_rt(asb->apollonet, TRUE, apollo_table, apollo_running,
		    apollorip_debug);
    } else {
	asb = malloc_named(sizeof(apollosb), "IDB: Apollo Info");
	if (asb == NULL) {
	    printf(nomemory);
	    return;
	}
	/*
	 * There was no block on the idb, so add the newly allocated one.
	 */
	if (!idb_add_swsb(idb, &type, asb)) {
	    printf("\nApollo: failed to add sub-block to %s", idb->namestring);
	    free(asb);
	    return;
	}
	/*
	 * Add this interface to the Apollo-private list of interfaces.
	 */
	if (!idb_add_swidb_to_list(idb, SWIDBLIST_APOLLO)) {
	    idb_report_add_error("Apollo");
	    return;
	}
    }
    asb->apollonet = network;
    asb->xnsrip.update_interval = RIP_DEFAULT_UPDATE;
    asb->xnsrip.static_rts = 0;
    asb->xnsrip.idb = idb;
    asb->xnsrip.type = APOLLO_TYPE;

    grip_del_rt(network, TRUE, apollo_table, apollo_running, apollorip_debug);
    grip_add_rt(&asb->xnsrip, network, 0L, NULL, NET_CONNECTED, 
		apollo_table, apollo_running, FALSE,
		apollorip_debug);
    COPY_TIMESTAMP(apollo_timer, asb->xnsrip.updatetimer);
    if (idb->hwptr->status & IDB_IEEEMASK) {
	hwidbtype	*hwidb = idb->hwptr;
	hwaddrtype	hwaddr;

	(*(hwidb->reset))(hwidb);
	hwaddr.type = STATION_IEEE48;
	hwaddr.length = STATIONLEN_IEEE48;
	bcopy(hwidb->hardware, &hwaddr.addr, hwaddr.length);
	arp_table_add(idb, apollo_address, LINK_APOLLO,
		      &hwaddr, ET_ARPA, ARP_INTERFACE);
    }
}

/*
 * apolloif_command
 * Process apollo interface keywords
 */

void apolloif_command (parseinfo *csb)
{
    idbtype	*idb;
    idbtype	*xfc;
    hwidbtype	*hwidb;
    apollosb	*asb;
    list_element *l_elt;
    int		time;
    apollo_acclisttype *acc;

    idb = csb->interface;
    if (idb == NULL)
	return;
    hwidb = hwidb_or_null(idb);
    asb = idb_get_swsb(idb, SWIDB_SB_APOLLO);

    if (csb->nvgen) {
	if (!apollo_running) {
	    return;
	}

	if (asb == NULL) {
	    return;
	}

	switch (csb->which) {
	  case APOLLOIF_ACC:
	    nv_write(apollo_running && asb->apollonet != 0L &&
		     asb->apollo_acclist,
		     "%s %s", csb->nv_command,
		     asb->apollo_acclist->name);
	    break;

	  case APOLLOIF_NETWORK:
	    nv_write(apollo_running && asb->apollonet != 0L, 
		     "%s %x", csb->nv_command,
		     asb->apollonet);
	    break;

	  case APOLLOIF_UPDATETIME:
	    nv_write(apollo_running && 
		     asb->xnsrip.update_interval != RIP_DEFAULT_UPDATE,
		     "%s %d", csb->nv_command,
		     asb->xnsrip.update_interval/ONESEC);
	    break;

	}
	return;
    }

    /*
     * Process given args
     */
    if (!apollo_running) {
	printf("%% Apollo not enabled");
        return;
    }

    switch (csb->which) {

      case APOLLOIF_ACC:
	if (csb->sense) {
	    acc = find_namedthing(&apollo_accQ, GETOBJ(string,1));
	    if (acc) {
		asb->apollo_acclist = acc;
	    } else {
		printf("\nBad Apollo access-group \"%s\"", csb->line);
	    }
	} else {
	    asb->apollo_acclist = NULL;
	}
	break;

      case APOLLOIF_NETWORK:
	if (csb->sense == FALSE) {
	    apollo_if_net_deconfig(csb);
	} else {
	    apollo_if_net_config(csb);
	}
	apollo_rxtypes_update(idb);
	break;

      case APOLLOIF_UPDATETIME:
	if (!csb->sense)
	    time = RIP_DEFAULT_UPDATE;
	else
	    time = GETOBJ(int,1) * ONESEC;
	asb->xnsrip.update_interval = time;
	/*
	 * Recompute new global update interval.  Take the minimum.
	 */
	apollo_update_interval = -1L;
	FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_APOLLO, l_elt, xfc) {
	    asb = idb_get_swsb_inline(xfc, SWIDB_SB_APOLLO);
	    if (asb && asb->apollonet)
		apollo_update_interval = min(apollo_update_interval,
					     asb->xnsrip.update_interval);
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	return;
	break;
    }

    return;
}

/*
 * apollo_start
 * Start apollo processing
 */
void apollo_start (void)
{
    apollo_table = malloc_named(sizeof(rip_routetable), "Apollo RIP Table");

    if (!apollo_table) {
	printf(nomemory);
	return;
    }
    apollo_started = TRUE;

    process_create(apollo_input, "Apollo Input", NORMAL_STACK, PRIO_HIGH);
    (void)cfork(apollo_router, 0L, 0, "Apollo Router", 0);
    (void)cfork(apollo_fork, 0L, 0, "Apollo Protocols", 0);
}

/*
 * apollo_fork
 * Process/routine to handle apollo RIP packets.
 */

forktype apollo_fork (void)
{
    paktype *pak;
    apollohdrtype *apollo;
    apolloechotype *ep;
    apollo_cisco_ping *cp;
    sys_timestamp time_sent;

    edisms((blockproc *)systeminitBLOCK, (ulong)&apollobgQ);
    process_set_priority(PRIO_LOW);

    queue_init(&apollo_ripQ,0);
    while (TRUE) {
	edisms((blockproc *)queueBLOCK, (long)&apollobgQ);
	while ((pak = dequeue(&apollobgQ))) {
	    if (!interface_up(pak->if_input)) {
		retbuffer(pak);
		continue;
	    }

	    apollo = (apollohdrtype *)apolloheadstart(pak);

	    switch (apollo->dsock) {

	      case APOLLO_SOCK_ECHO:
		if (apollo->pt & APOLLO_ECHO_REQUEST) {
		    apollo_echo(pak);
		    break;
		} else 
		    if (!QUEUEEMPTY(&apolloechoQ)) {		
			apollo_stat[APOLLO_ECHOREPIN]++;
			for (ep = (apolloechotype *)apolloechoQ.qhead;
			     ep;
			     ep = ep->next) {

			    if (ep->socket == apollo->dsock) {
				if (apollo->len >= APOLLO_MINECHOSIZE) {
				    cp = (apollo_cisco_ping *)APOLLODATASTART(pak);
				    PAK_GET_TS(&cp->time_sent, &time_sent);
				    ep->deltatime = ELAPSED_TIME(time_sent);
				} else
				    ep->deltatime = 0;
				ep->how = '!';
				ep->active = FALSE;
				p_unqueue(&apolloechoQ, ep);
				break;
			    }
			}
		    }
		retbuffer(pak);
		break;

	      case APOLLO_SOCK_RIP:
		if (apollo->pt == APOLLO_TYPE_RIP) {
		    enqueue(&apollo_ripQ, pak);
		    break;
		}
		/* Fall thru to error code, if packet mismatch */

	      default:
		if (apollo_debug)
		    buginf("\nApollo: unknown packet from"
			   "%x:%e::%x to %x:%e::%x typ %x",
			   GETLONG(&apollo->snet), apollo->shost, apollo->ssock,
			   GETLONG(&apollo->dnet), apollo->dhost, apollo->dsock,
			   apollo->pt);
		apollo_stat[APOLLO_UNKNOWN]++;
		retbuffer(pak);
		break;
	    }
	}
    } /* while TRUE */
}

/*
 * apollo_routeBLOCK
 * Block waiting for something for the apollo RIP router process to do
 */

boolean apollo_routeBLOCK (queuetype *q)
{
    if (!apollo_running)
	return(TRUE);			/* never if not running */
    if (q->qhead)
	return(FALSE);			/* input available */
    if (AWAKE(apollo_timer))
	return(FALSE);			/* timer expired */
    if (TIMER_RUNNING(apollo_changed) &&
	CLOCK_OUTSIDE_INTERVAL(apollo_changed, RIP_FLASHUPDATE))
	return(FALSE);			/* topology changed */
    return(TRUE);
}

/*
 * apollo_router
 * Apollo RIP router process. Perform routing maintaince functions.
 */

forktype apollo_router (void)
{
    paktype *pak;
    apollohdrtype *apollo;
    xns_riptype *rip;
    boolean full;
    boolean part;

    edisms((blockproc *)systeminitBLOCK, (ulong)&apollo_ripQ);
    process_set_priority(PRIO_LOW);

    full = FALSE;
    part = FALSE;
    apollo_routes = 0;
    TIMER_STOP(apollo_changed);

    while (TRUE) {

	edisms((blockproc *)apollo_routeBLOCK, (long)&apollo_ripQ);
	/*
	 * First empty the RIP packet queue
	 */
	while ((pak = dequeue(&apollo_ripQ))) {

	    apollo = (apollohdrtype *)apolloheadstart(pak);
	    rip = (xns_riptype *)APOLLODATASTART(pak);
	    switch (rip->cmd) {
	    case RIP_REQUEST:
		apollo_rcvriprequest(pak, rip);
		break;
	    case RIP_REPLY:
		apollo_riprcvupd(pak, rip, apollo_table, apollorip_debug);
		break;
	    }
	    retbuffer(pak);
	}

	/*
	 * Send updates, reset wakeup timer
	 */
	if (AWAKE(apollo_timer))
	    full = TRUE;
	if (TIMER_RUNNING(apollo_changed) &&
	    CLOCK_OUTSIDE_INTERVAL(apollo_changed, RIP_FLASHUPDATE))
	    part = TRUE;

	if (full || part) {
	    if (full) {
		TIMER_UPDATE(apollo_timer, apollo_update_interval);
		grip_ager(apollo_table, &apollo_routes,
			  "Apollo", apollo_maxpaths, &apollo_changed,
			  apollorip_debug);
	    } 
	    apollo_majorupdate(&apollo_changed, full, part);
	    (void) process_may_suspend();
	}
	full = part = FALSE;
    }
}

/*
 * apollo_majorupdate
 * Perform an XNS RIP general broadcast.
 * If update is set we need to send the entire routing table and
 * not just what changed.
 */

void apollo_majorupdate (
    sys_timestamp *changed,
    boolean full,
    boolean part)
{
    idbtype	*idb;
    apollosb	*asb;
    list_element *l_elt;
    boolean	updatesent;

    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_APOLLO, l_elt, idb) {
	asb = idb_get_swsb_inline(idb, SWIDB_SB_APOLLO);
	if ((asb == NULL) || (asb->apollonet == 0L))
	    continue;
 	updatesent = FALSE;
	if (full) {
	    if (AWAKE(asb->xnsrip.updatetimer)) {
		TIMER_UPDATE(asb->xnsrip.updatetimer, asb->xnsrip.update_interval);
		if (interface_up(idb))
		    apollo_sendripupdate(idb, asb->apollonet, apollo_bdcast,
					 0, TRUE);
		updatesent = TRUE;
	    }
	    if (TIMER_EARLIER(asb->xnsrip.updatetimer, apollo_timer))
 	        COPY_TIMESTAMP(asb->xnsrip.updatetimer, apollo_timer);
	}

	if (part && !updatesent) {
	    if (interface_up(idb)) 
		apollo_sendripupdate(idb, asb->apollonet, apollo_bdcast,
				     0, FALSE);
	    TIMER_STOP(*changed);
	}
    }
}

/*
 * apollo_sendupdate
 * Send an apollo RIP update.
 */

uchar unk1_data[] = { 0,0,0,0, 0x90, 0,0,0,0, 1, 0x23, 0x45,
		      02, 0, 0x90, 0 };

void apollo_sendripupdate (
    idbtype	*idb,
    ulong	net,
    uchar	*host,
    ushort	sock,
    boolean	fullupdate)
{
    paktype		*pak;
    apollohdrtype	*apollo;
    xns_riptype		*rip;
    xns_ripinfotype	*p;
    xns_ripentrytype	*entry, *temp;
    apollosb		*asb;
    int			nroutes, bucket;
    boolean		done;
    sys_timestamp	lastupdate;

    if (apollorip_debug)
	buginf("\nApolloRIP: sending update to %x.%e via %s",
	    net, host, idb->namestring);

    asb = idb_get_swsb_inline(idb, SWIDB_SB_APOLLO);
    COPY_TIMESTAMP(asb->xnsrip.lastupdate, lastupdate);
    GET_TIMESTAMP(asb->xnsrip.lastupdate);
    done = FALSE;
    bucket = 0;
    temp = NULL;
    while (!done) {
	nroutes = APOLLO_RIP_MAXROUTES;
	pak = getbuffer(APOLLO_RIP_MAXSIZE);
	if (!pak)
	    return;
	pak->if_output = idb;

	apollo = (apollohdrtype *)apolloheadstart(pak);

	memset(apolloheadstart(pak), 0,APOLLOOVERHEADBYTES);
	apollo->pkttype = apollo->apollolink = 0x90;
	apollo->srcnode = apollo_address;
	apollo->version = 2;
	apollo->transid = apollo_transid++;
	apollo->routelen = 40;
	apollo->curpos = 4;
	apollo->tc = 0;
	apollo->pt = 1;
	PUTLONG(&apollo->dnet, net);
	pak->linktype = LINK_APOLLO;
	ieee_copy(host, apollo->dhost);
	apollo->dsock = sock ? sock : APOLLO_SOCK_RIP;
	PUTLONG(&apollo->snet, asb->apollonet);
	ieee_copy(apollo_etheraddr, apollo->shost);
	apollo->ssock = APOLLO_SOCK_RIP;
	rip = (xns_riptype *) APOLLODATASTART(pak);
	rip->cmd = RIP_REPLY;
	p = rip->nets;
/*
 *   p->net = idb->apollonet;
 *   p->metric = 1;
 *   nroutes--;
 *   p++;
 */
	while (nroutes > 0) {
	    entry = grip_getnetwork(idb, fullupdate, &bucket, &temp,
				    apollo_table, SPLIT_TRUE, lastupdate);
	    if (entry == NULL) {
		done = TRUE;
		break;
	    }
	    PUT2WD(p->nnet, entry->dstnet);
	    if (!TIMER_RUNNING(entry->holddownuntil))
		p->metric = min(entry->hopcount + 1, APOLLO_INFINITY);
	    else
		p->metric = APOLLO_INFINITY;   /* Only happens when flashing */
	    if (apollorip_debug)
		buginf("\n    network %x, metric %d", entry->dstnet,p->metric);
	    nroutes--;
	    p++;
	    (void) process_may_suspend();
	}
	if (nroutes < APOLLO_RIP_MAXROUTES) {
	    apollo->hdrdatalen = 2 +
		((APOLLO_RIP_MAXROUTES - nroutes)*sizeof(xns_ripinfotype));
	    apollo->hdrlen = apollo->hdrdatalen + XNSOVERHEADBYTES +
		APOLLOOVERHEADBYTES;
	    apollo->datalen = 0;
	    apollo->len = XNSOVERHEADBYTES + apollo->hdrdatalen;
	    apollo->cksum = 0xFFFF;
	    if (apollo_send1(pak, apollo_bdcast))
		if (apollo_debug) {
		    buginf("\nApollo: src=%x.%e, dst=%x.%e, packet sent",
			   asb->apollonet, apollo_etheraddr, net, host);
	    }
	} else {
	    if (apollorip_debug)
		buginf("\nApolloRIP: suppressing null update");
	    retbuffer(pak);
	}
    }
}

/*
 * apollo_riprcvupd
 * Process an Apollo RIP packet.
 */

void apollo_riprcvupd (
		       paktype		*pak,
		       xns_riptype	*rip,
		       rip_routetable	(*table),
		       boolean		debug)
{
    apollohdrtype	*apollo;
    xns_ripinfotype	*p;
    idbtype		*idb;
    int			nentries;
    apollosb		*asb;

    apollo = (apollohdrtype *)apolloheadstart(pak);

    nentries = (apollo->len - XNSHEADERBYTES - sizeof(rip->cmd)) /
	sizeof(xns_ripinfotype);
    idb = pak->if_input;
    if (idb == NULL)
	return;
    asb = idb_get_swsb(idb, SWIDB_SB_APOLLO);
    if (asb == NULL)
	return;
    p = (xns_ripinfotype *)rip->nets;
    if (debug)
	buginf("\nApolloRIP: update from %x.%e",
	    GETLONG(&apollo->snet), apollo->shost);
    while (nentries-- > 0) {
	if (debug)
	    buginf("\n    %x in %d hops",
		GET2WD(p->nnet), p->metric);
	grip_update(&asb->xnsrip, GET2WD(p->nnet), p->metric,
		    GETLONG(&apollo->snet),
		    apollo->shost, table, &apollo_routes, apolloname,
		    &apollo_changed, apollo_maxpaths, p->metric,
		    0, apollorip_debug);
	p++;
	(void) process_may_suspend();
    }
}

/*
 * apollo_route_adjust
 * Called by SERVICE_ROUTE_ADJUST
 */

void apollo_route_adjust (idbtype *swidb)
{
    apollosb	*asb;

    asb = idb_get_swsb(swidb, SWIDB_SB_APOLLO);
    if (asb)
	grip_route_adjust(&asb->xnsrip, apollo_table, asb->apollonet, 
			  &apollo_changed, apollo_running, 
			  &apollo_routes, apollorip_debug);
}

/*
 * show_apollo
 * Display apollo Protocol information
 */

void show_apollo (parseinfo *csb)
{
    if (!apollo_running) {
	printf("%% Apollo not enabled");
	return;
    }
    switch (csb->which) {
      case SHOW_APOLLO_INTERFACE:
	automore_enable(NULL);
	show_swidb_chain(GETOBJ(idb,1), display_apollo, 0);
	automore_disable();
	break;
      case SHOW_APOLLO_ROUTE:
	show_griproutes(GETOBJ(int,1), apollo_table, apollo_routes,
			apollo_maxpaths, APOLLO_TYPE);
	break;
      case SHOW_APOLLO_TRAFFIC:
	show_apollotraffic();
	break;
      case SHOW_APOLLO_ARP:
	apollo_arp_display();
	break;
      case SHOW_APOLLO_ACCESS:
	show_apolloaccess(0);
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * display_apollo
 * Display APOLLO interface information
 */

void display_apollo (idbtype *idb)
{
    apollosb	*asb;

    if (idb == NULL)
	return;

    printf(if_is_line_proto_is_msg, idb->namestring,
          print_idbstate(idb), interface_up(idb) ? "up" : "down");
    asb = idb_use_swsb_inline(idb, SWIDB_SB_APOLLO);
    if (asb == NULL || asb->apollonet == 0L) {
	printf("\n  Apollo protocol processing disabled");
	if (asb != NULL)
	    idb_release_swsb_inline(idb, SWIDB_SB_APOLLO);
	return;
    }
    printf("\n  Apollo address is %x.%x", asb->apollonet, apollo_address);
    printf("\n  Update time is %d seconds", 
	   asb->xnsrip.update_interval/ONESEC);
    printf("\n  Outgoing access list is %s", asb->apollo_acclist ?
	   asb->apollo_acclist->name : "not set");
    idb_release_swsb_inline(idb, SWIDB_SB_APOLLO);
}

void show_apollotraffic (void)
{
    printf("\nRcvd:   %d total, %d format errors, %d checksum errors, %d bad hop count,",
	    apollo_stat[APOLLO_INPUT], apollo_stat[APOLLO_FORMATERROR],
	    apollo_stat[APOLLO_CKSUMERR], apollo_stat[APOLLO_HOPCOUNT]);
    printf("\n        %d local destination, %d multicast",
	    apollo_stat[APOLLO_LOCALDEST], apollo_stat[APOLLO_INMULT]);
    printf("\nBcast:  %d received, %d sent",
	    apollo_stat[APOLLO_BCASTIN], apollo_stat[APOLLO_BCASTOUT]);
    printf("\nSent:   %d generated, %d forwarded",
	    apollo_stat[APOLLO_OUTPUT], apollo_stat[APOLLO_FORWARDED]);
    printf("\n        %d encapsulation failed, %d no route",
	    apollo_stat[APOLLO_ENCAPSFAILED], apollo_stat[APOLLO_NOROUTE]);
    printf("\n Echo: Rcvd %d requests, %d replies",
	   apollo_stat[APOLLO_ECHOREQIN], apollo_stat[APOLLO_ECHOREPIN]);
    printf("\n       Sent %d requests, %d replies",
	   apollo_stat[APOLLO_ECHOREQOUT], apollo_stat[APOLLO_ECHOREPOUT]);
    printf("\n        %d unknown", apollo_stat[APOLLO_UNKNOWN]);
}

/*
 * show_apolloaccess
 * pretty-print the apollo access lists.
 */

void show_apolloaccess (int listnumber_tmp)
{
    apollo_acclisttype *acclist;
    apollo_acctype *acc;

    if (listnumber_tmp)
       return;

    for (acclist = (apollo_acclisttype *) apollo_accQ.qhead; acclist;
	 acclist = acclist->next) {
	if (automore_quit())
	    return;
	printf("\nApollo Domain access list %s", acclist->name);
	for (acc = (apollo_acctype *) acclist->q.qhead; acc; acc = acc->next)
	    printf("\n    %s %x-%x.%x %x", acc->result? "permit" : "deny",
		     acc->firstnet, acc->lastnet, acc->host, acc->hostmask);
    }
}


/*
 * apollo_rcvriprequest
 * Process an incomming route request.  We don't actually know that
 * apollo systems send any of these, but...
 */

void apollo_rcvriprequest (
			   paktype *pak,
			   xns_riptype *rip)
{
    apollohdrtype *apollo, *newapollo;
    paktype *newpak;
    xns_riptype *newrip;
    xns_ripinfotype *p;
    xns_ripentrytype *entry;
    apollosb	*asb;
    int bytes;

    apollo = (apollohdrtype *)apolloheadstart(pak);

    if (apollorip_debug)
	buginf("\nApolloRIP: received request from %x.%e",
		GETLONG(&apollo->snet), apollo->shost);
    if (apollo->len == XNSHEADERBYTES + sizeof(xns_riptype) &&
	GET2WD(rip->nets[0].nnet) == -1) {
	apollo_sendripupdate(pak->if_input, GETLONG(&apollo->snet),
			     apollo->shost, apollo->ssock, TRUE);
	return;
    }
    newpak = pak_duplicate(pak);
    if (newpak == NULL)
	return;
    newpak->if_output = pak->if_input;

    newapollo = (apollohdrtype *)apolloheadstart(newpak);

    newapollo->tc = 0;
    PUTLONG(&newapollo->dnet, GETLONG(&apollo->snet));
    ieee_copy(apollo->shost, newapollo->dhost);
    newapollo->dsock = apollo->ssock;

    asb = idb_get_swsb(pak->if_input, SWIDB_SB_APOLLO);
    PUTLONG(&newapollo->snet, asb->apollonet);
    ieee_copy(apollo_etheraddr, newapollo->shost);
    newapollo->ssock = APOLLO_SOCK_RIP;
    newrip = (xns_riptype *) APOLLODATASTART(newpak);
    newrip->cmd = RIP_REPLY;
    p = newrip->nets;
    bytes = newapollo->len;
    while (bytes >=  XNSOVERHEADBYTES + sizeof(rip->cmd)) {
	entry = grip_find_rt(GET2WD(p->nnet), apollo_table);
	if ((entry != NULL) && !TIMER_RUNNING(entry->holddownuntil)) {
	    p->metric = min(entry->hopcount + 1, APOLLO_INFINITY);
	} else
	    p->metric = APOLLO_INFINITY;
	if (apollorip_debug)
	    buginf("\n    Net %x reachable in %d hops", GET2WD(p->nnet),
		p->metric);
	bytes -= sizeof(xns_ripinfotype);
	p++;
    }
    apollo->cksum = 0xFFFF;
    if (apollo_send1(newpak, apollo->shost)) {
	if (apollo_debug)
	    buginf("\nApollo: src=%x.%e, dst=%x.%e, packet sent",
		asb->apollonet, apollo_etheraddr,
		GETLONG(&apollo->snet), apollo->shost);
    }
}

/*
 * parse_apolloaddress
 * Parse and return an apollo address (32bit_hex_netnumber.20bit_hex_host)
 */

boolean parse_apolloaddress (
    char *buf,
    ulong *net,
    uchar *host)
{
    ulong myhost;

    buf = deblank(buf);
    if (buf[0] == '-' && buf[1] == '1' && buf[2] == '.') {
	*net = -1L;
	buf = &buf[2];
    } else
	*net = parse_hex_long(buf);
    while (*buf != '.' && *buf != '\0')
	buf++;
    if (*buf++ != '.' || *net == 0L || *net > APOLLOMAXNET)
	return(FALSE);
    myhost = parse_hex_long(buf);
    while (*buf != '.' && *buf != '\0')
	buf++;
    if (myhost == 0 || myhost > APOLLOMAXADDR || *buf != '\0')
	return(FALSE);
    bcopy((uchar *)&myhost, host+2, 4);
    host[0] = 0;
    host[1] = 0;
    return(TRUE);
}


/*
 * apollo_echo
 * Handle echo protocol
 */

void apollo_echo (paktype *pak)
{
    ulong dnet, tnet, tnode;
    ushort tsock;
    uchar thost[IEEEBYTES];
    idbtype *idb;
    apollohdrtype *apollo;
    apollosb	*asb;
    apollo_cisco_ping *cp;

    apollo = (apollohdrtype *)apolloheadstart(pak);

    idb = pak->if_input;
    asb = idb_get_swsb(idb, SWIDB_SB_APOLLO);
    apollo_stat[APOLLO_ECHOREQIN]++;
    tnet = GETLONG(&apollo->snet);
    ieee_copy(apollo->shost, thost);
    tsock = apollo->ssock;
    tnode = apollo->srcnode;

    apollo->pkttype = SETECHO_REPLY(apollo->pkttype);
    apollo->srcnode = apollo_address;
    apollo->transid = apollo_transid++;
    apollo->pt = SETECHO_REPLY (apollo->pt);

    apollo->destnode = tnode;
    dnet = GETLONG(&apollo->dnet);
    PUTLONG(&apollo->snet,
	    ((dnet != -1L) && (dnet != 0L)) ? dnet : asb->apollonet);
    ieee_copy(apollo_etheraddr, apollo->shost);
    apollo->ssock = apollo->dsock;
    
    PUTLONG(&apollo->dnet, tnet);
    ieee_copy(thost, apollo->dhost);
    apollo->dsock = tsock;
    apollo->tc = 0;
    cp = (apollo_cisco_ping *)APOLLODATASTART(pak);
    cp->type = APOLLO_ECHO_REPLY;
    
    apollo->cksum = 0xFFFF;
    if (apollo_send(pak))
	apollo_stat[APOLLO_ECHOREPOUT]++;
}

/*
 * apollo_on_idb
 *
 * Return TRUE if Apollo Domain is enabled on the given interface
 */
boolean apollo_on_idb (idbtype *idb)
{
    apollosb	*asb;

    if (   (!apollo_running)
	|| ((asb = idb_get_swsb_inline(idb, SWIDB_SB_APOLLO)) == NULL)
	|| (asb->apollonet == 0L))
      return(FALSE);
    else
      return(TRUE);
}
