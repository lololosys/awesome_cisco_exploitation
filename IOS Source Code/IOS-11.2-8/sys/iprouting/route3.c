/* $Id: route3.c,v 3.9.4.17 1996/08/28 20:13:55 myeung Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/route3.c,v $
 *------------------------------------------------------------------
 * route3.c -- Routing Support: Router command and related routines
 *
 * 15-June-1987, Kirk Lougheed
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: route3.c,v $
 * Revision 3.9.4.17  1996/08/28  20:13:55  myeung
 * CSCdi64310:  match internal keyword broken in OSPF redistribution
 * Branch: California_branch
 *
 * Revision 3.9.4.16  1996/08/28  12:53:49  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.4.15  1996/08/16  17:54:17  dawnli
 * CSCdi62695:  rip not advertising networks that are passive of other
 * protocols
 * Branch: California_branch
 *
 * Do not copy NETF_PASSIVE into ndb->attrflags, since it will be
 * interpreted as NET_SUPERNET.
 * And rip version 1 does not advertise any supernets
 *
 * Revision 3.9.4.14  1996/06/28  23:19:46  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.9.4.13  1996/06/28  02:49:17  dkatz
 * CSCdi61545:  EIGRP doesnt limit hop count
 * Branch: California_branch
 * If the hop count exceeds the maximum, act as though an infinite metric
 * was received.
 *
 * Revision 3.9.4.12  1996/06/28  01:21:18  snyder
 * CSCdi61601:  fix some char * that should have been char [] in iprouting
 * Branch: California_branch
 *
 *              You get space back for the pointer
 *              You get space back for each reference
 *              You get bytes out of data space
 *
 * Revision 3.9.4.11  1996/06/17  23:34:25  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.9.4.10  1996/06/03  23:24:08  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.9.4.9  1996/05/26  08:38:30  dkatz
 * CSCdi57814:  crash when no router eigrp
 * Branch: California_branch
 * Dequeue summary entries before freeing them.
 *
 * Revision 3.9.4.8  1996/05/21  09:53:37  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.9.4.7  1996/05/04  01:22:33  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.9.4.6  1996/04/16  19:02:19  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.9.4.5  1996/04/15  21:18:38  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.9.4.4  1996/04/02  07:29:27  dkatz
 * CSCdi52277:  Redistribution metric problems with IGRP/EIGRP
 * Branch: California_branch
 * Always store canonical metrics scaled for IGRP, and make EIGRP do
 * its own scaling internally.  Get the scaling right in all three
 * places.
 *
 * Revision 3.9.4.3  1996/04/02  05:37:28  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.9.4.2  1996/03/28  22:28:22  rchandra
 * CSCdi52880:  BGP: generate error message for unsupported route-map
 * options
 * Branch: California_branch
 * - generate error message when unsupported route-map option is used
 * - tread confederation EBGP neighbors similar to internal neighbors in
 *   best path selection
 * - fix the bug in bgp_compare_med, so that during MED comparison,
 *   confederation information is skipped properly
 * - confederation EBGP paths should be inserted into the IP table with
 *   internal distance
 * - do not consider common-AS neighbors as confederation neighbors
 * - during 'clear ip bgp dampening' avoid skipping paths
 * - undo the patch to the radix version walker
 *
 * Revision 3.9.4.1  1996/03/18  20:29:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/16  06:58:19  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.2  1996/03/07  09:50:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  01:04:23  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/03/09  22:56:07  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.8  1996/02/07  16:13:26  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/01  06:06:13  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/22  06:15:02  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/29  01:55:20  myeung
 * CSCdi46087:  OSPF misinterpret network 0.0.0.0 255.255.255.255
 *
 * Revision 3.4  1995/12/19  00:34:20  myeung
 * CSCdi44966:  OSPF subnets stay in the old area number upon change
 *
 * Revision 3.3  1995/12/04  06:47:15  tli
 * CSCdi43559:  IP: distribute-list 101 out should work
 *
 * Revision 3.2  1995/11/17  17:37:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:06:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/09/25  08:31:16  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.7  1995/09/23  21:41:06  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Reference renamed fields.  Clean up various interfaces to the IP
 * forwarding table to support protocol-specific functions.
 *
 * Revision 2.6  1995/08/24  22:34:43  bcole
 * CSCdi39279:  NHRP: Aggregation needed to provide purge notification
 *
 * Provide aggregated NHRP responses.
 * Cache now consists of radix tree(s) instead of hash table.
 * Allow NHRP resolution during encapsulation of NHRP purge messages.
 * For IP, NHRP now a routing process so as to receive callbacks.
 * Allow for radix tree walk where varargs have already been pre-collected.
 *
 * Revision 2.5  1995/07/07  21:52:20  rchandra
 * CSCdi36806:  BGP : Spurious memory access in bgp_nv_neighbor
 *
 * Revision 2.4  1995/07/06  23:12:31  rchandra
 * CSCdi36759:  BGP : Add conditional per neighbor default generation
 * - add 'neighbor <x.x.x.x> default-originate route-map <tag>' command.
 *
 * Revision 2.3  1995/06/19  01:08:33  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:06:47  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:10:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include "interface_private.h"
#include "packet.h"
#include "login.h"
#include "access.h"
#include "../ip/ip_registry.h"
#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "config.h"
#include "route.h"
#include "route_inlines.h"
#include "neighbor.h"
#include "access.h"
#include "../if/network.h"
#include "ipigrp2.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "parser.h"
#include "parser_defs_iprouting.h"
#include "parser_defs_router.h"
#include "logger.h"
#include "../iprouting/msg_iproute.c"	/* Not a typo, see logger.h */
#include "ipmobile.h"
#include "ospf_public.h"
#include "igrp.h"
#include "../parser/parser_actions.h"
#include "../tcp/http.h"
#include "eigrp_registry.h"
#include "../wan/ppp_registry.h"

boolean router_init_running = FALSE;
queuetype router_initQ;			/* List of network commands */
pdbtype *ip_proctable[MAX_IPPROCS];	/* table of routing processes */

/*
 *	<protocol> ::= egp <as> | igrp <as> | rip | bgp <rd>
 *					ospf <as> | static | connected
 * [no] router <protocol>
 *
 * [no] network <address> [passive|backdoor]
 * [no] distance <weight> [<address> <mask> [<list>]]
 * [no] redistribute <protocol> [metric <metric>] [tag <tag>] [subnets]
 * [no] distribute-list <access-list> in | out [<protocol>|<interface>]
 * [no] variance <number>
 * [no] default-metric <metric> |
 *		 <bandwidth> <delay> <reliability> <loading> <mtu>
 * [no] neighbor <address> [<arguments>]
 *   "    "      any [<arguments>]
 * [no] area <area id> [<arguments>]
 * [no] passive-interface <interface-name> 
 * [no] offset-list in|out <modification> [<accesslist>]
 * [no] default-information allowed in|out
 *   "    "                 originate
 * [no] timers basic <broadcast> <invalid> <holdtime> <flush>
 *   "    "    bgp <keepalive> <holdtime>
 *   "    "    egp <hello> <poll>
 * [no] metric weights <k1> <k2> <k3> <k4> <k5>
 *   "    "    holddown
 *   "    "    hopcount <count>
 *   "  source <interface>
 * [no] output-delay <delay>
 */

/*
 * Forward declarations
 */
static void neighbor_simple(pdbtype *pdb, ipaddrtype address,
		     parseinfo *csb, boolean sense);

const char showipproto_neighborheader[] = "  Neighbor(s):";


/*
 * router_parse
 * Parse a router keyword and AS, return pointer to existing pdb.
 */

#define BUFFLN 20

static const char unkpro[] = "\nUnknown routing protocol - \"%s\"";
static const char nodyn[] = "\nNon-dynamic routing protocol specified";

typedef struct protonametype_ {
    char *name;
    int type;
    int index;
} protonametype;

static const protonametype protoname[] = {
    {"static",		PDB_ALL,	PROC_STATIC},
    {"igrp",		PDB_IP,		PROC_IGRP},
    {"egp",		PDB_IP,		PROC_EGP},
    {"bgp",		PDB_IP,		PROC_BGP},
    {"rip",		PDB_IP,		PROC_RIP},
    {"ospf",		PDB_IP,		PROC_OSPF},
    {"eigrp",		PDB_IP,		PROC_IGRP2},
    {"connected",	PDB_IP,		PROC_CONNECTED},
    {"mobile",		PDB_IP,		PROC_MOBILE},
    {"nhrp",		PDB_IP,		PROC_NHRP},
    {"odr",		PDB_IP,		PROC_ODR},
    {"isis",		PDB_OSI,	PROC_ISIS},
    {"iso-igrp",	PDB_OSI,	PROC_ISOIGRP},
    {NULL, 0, 0}
};

pdbtype *router_parse (parseinfo *csb, boolean nullokay, int *pdb_type)
{
    char         *token;
    pdbtype      *pdb;
    clns_pdbtype *clns_pdb;
    char         name[BUFFLN];
    ushort       asystem;
    int		 i, index;

    asystem = 0;
    token = NULL;
    pdb = NULL;

    /*
     * Get protocol name (second keyword).
     */
    if (csb->router_name.myname &&
	(strlen(csb->router_name.myname) < BUFFLN)) {
	strcpy(name, csb->router_name.myname);
    } else {
	if ((csb->router_name.myname == NULL) && nullokay) {
	    return(NULL);
	}
	printf(unkpro, csb->line);
	return(NULL);
    }


    /*
     * Check if it's an IP protocol or an OSI protocol.
     */
    index = -1;
    for (i = 0; protoname[i].name; i++) {
	if (partialmatch(protoname[i].name, name)) {
	    index = protoname[i].index;
	    break;
	}
    }

    if (index == -1)
	goto unknown;

    /*
     * If the protcol name is not specified, then take the context from the
     * name.
     */
    if ((*pdb_type == PDB_UNKNOWN) && (protoname[i].type != PDB_ALL)) {
	*pdb_type = protoname[i].type;
    }

    if (((*pdb_type == PDB_UNKNOWN) && (protoname[i].type == PDB_ALL)) ||
	(index == PROC_ISIS)) {
 	*pdb_type = PDB_OSI;
    }
 
    /*
     * Get third parameter on command. Accept number (for IP) or tag
     * (for OSI).
     */
    switch (*pdb_type) {
    case PDB_IP:

	/*
	 * Parsing an AS number for IP routing protocols.  Accept 0 only
	 * for EGP.  RIP needs no AS number.
	 */
	asystem = (ushort) csb->router_name.as;

	/*
	 * Scan list of IP routing processes for a matching descriptor block.
	 */
	for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	    if (partialmatch(pdb->name, name)) {
		if (pdb->proctype & 
		    (PROC_IGRP|PROC_IGRP2|PROC_EGP|PROC_BGP|PROC_OSPF|
		     PROC_RIP)) {
		    if (asystem == pdb->asystem)
			return(pdb);
		} else
		    return(pdb);
	    }
	}
	
	/*
	 * No existing routing process, so maybe create it
	 */
 	if (csb->sense) {
	    pdb = reg_invoke_iprouting_pdb_init(index, asystem);
	    if (index == PROC_IGRP || index == PROC_IGRP2)
		reg_invoke_ipigrp2_set_distflags(pdb);
	}
	break;
    case PDB_OSI:

	/*
	 * Parsing string for OSI routing protocol tag or keyword
	 * on "redistribute static" command.
	 */
	token = (!*csb->router_name.tag) ? NULL : csb->router_name.tag;

	/*
	 * Go through OSI pdbs for match based on protocol and tag. 
	 */
	clns_pdb = reg_invoke_ip_clns_find_router_name(token);
	if ((index != PROC_STATIC) && clns_pdb) {
	    if (partialmatch(clns_pdb->name, name)) {
		return((pdbtype *) clns_pdb);
	    } else {
		printf("Invalid tag - it is assigned to %s",
			clns_pdb->name);
		return(NULL);
	    }
	}

	/*
	 * No existing routing process, so maybe create it
	 */
 	if (csb->sense) {
	    switch (index) {
	    case PROC_ISIS:
		pdb = (pdbtype *) reg_invoke_ip_isis_pdb_init(token);
		break;
	    case PROC_ISOIGRP:
		pdb = (pdbtype *) reg_invoke_ip_clns_igrp_pdb_initialize(token);
		break;
	    case PROC_STATIC:
		if (!GETOBJ(int,11)) {
		    pdb = (pdbtype *) reg_invoke_ip_clns_staticpdb();
		} else {
		    pdb = pdb_STATIC;
		}
		break;
	    default:
		goto unknown;
		break;
	    }
	}
	break;
    default:
	printf("\nUnknown protocol family");
	break;
    }

    return(pdb);

 unknown:
    printf(unkpro, name);
    return(NULL);
}

/*
 * ip_routing_pdb_init_default
 * Default if unregistered protocol attempted
 */
pdbtype *
iprouting_pdb_init_default (ushort asystem)
{
    printf("\nUnknown routing protocol");
    return(NULL);
}

/*
 * ip_router_erase
 * Destroy a routing process
 */

void ip_router_erase (pdbtype *pdb)
{
    pdbtype      *dist;
    idbtype      *idb;
    neighbortype *next, *np;
    int          i;
    int		 count;
    char         *str;
    summarytype  *summary;

    if (pdb->redist_callback)
	reg_invoke_iprouting_external_flush(pdb->proctype, pdb);

    count = pdb->netcount;
    for (i = 0; i < count; i++)
	network_finish(pdb, pdb->networks[0], FALSE, pdb->network_mask[0], 
		       pdb->network_info[0], OSPF_AREA_ID_FORMAT_IGNORE, 
		       FALSE);
    
    /*
     * Eliminate all neighbors if we have simple neighbors
     */
    if (!pdb->neighbor) {
	for (np = pdb->neighbors; np; np = next) {
	    next = np->next;
	    free(np);
	}
	pdb->neighbors = NULL;
    }
    
    /*
     * Clear the passive bits on all interfaces we are running on.
     * Clean up any protocol filters that exist.
     */
    FOR_ALL_SWIDBS(idb) {
	idb->ip_passive_router &= ~pdb->mask;
	idb->ip_iprotofltr[pdb->index] = 0;
	idb->ip_oprotofltr[pdb->index] = 0;
    }

    /*
     * Clear the redistribute bit in all other routers.
     */
    invalidate_redist(pdb);
    for (dist = (pdbtype *)protoQ.qhead; dist; dist = dist->next)
	if (dist != pdb)
	    dist->distflag &= ~pdb->mask;

    /*
     * Clear route-map references.
     */
    for (i = 0; i < MAX_IPPROCS; i++) {
	str = pdb->routemap[i];
	if (str) {
	    free(str);
	    pdb->routemap[i] = NULL;
	}
    }

    while (!QUEUEEMPTY(&pdb->ip_address_sumq))
	free(dequeue(&pdb->ip_address_sumq));

    if (pdb->default_routemap) {
	free(pdb->default_routemap);
	pdb->default_routemap = NULL;
    }

    /*
     * Clear summary addresses.
     */
    while ((summary = dequeue(&pdb->summaryQ))) {

	/* Don't forget to free the interface pointer entries. */

	while (summary->idbQ.qhead) {
	    free(dequeue(&summary->idbQ));
	}
	free(summary);
    }

    /*
     * Clean up the peer cache.
     */
    pdb_flushcache(pdb);
    
    /*
     * Shutdown process, maybe delete pdb.  Flush routing table.
     */
    if (pdb->running) {
	pdb->running = FALSE;
	if (pdb->pid) {
	    process_kill(pdb->pid);
	    pdb->pid = 0;
	}
	table_prune_pdb(pdb);
	table_reload(NULL);
	ip_route_new_default();
    }

    /*
     * Do any protocol specific cleanup.
     */
    if (pdb->cleanup)
	(*pdb->cleanup)(pdb);
}

/*
 * router_command
 * Select, create, or destroy a routing process
 */

void router_command (parseinfo *csb)
{
    pdbtype     *pdb;
    int	 	pdb_type;
    spawntype	*args;


    if (csb->nvgen) {

	csb->protocol = GETOBJ(pdb,1);
	pdb = csb->protocol;

	if ((pdb->family == PDB_IP) && (pdb->proctype & PROC_MOBILE))
	    return;

	/*
	 * NVGEN of a specific router command.  The pdb's were selected
	 * by router_action().  Treat GETOBJ(pdb,1) as opaque.
	 */
	nv_write(TRUE, "%s", csb->nv_command);
	csb->prototype = GETOBJ(int,1);
	*csb->nv_command = '\0';
	csb->mode = router_mode;
	push_node(csb, get_mode_nv(csb));
	return;
    }

    /*
     * Parse a routing process name.
     */
    csb->protocol = NULL;
    pdb_type = PDB_UNKNOWN;
    pdb = router_parse(csb, FALSE, &pdb_type);
    if (!pdb) {
	return;
    }

    /* 
     * pdb returned is an OSI one.
     */
    if (pdb_type == PDB_OSI) {
	if (!csb->sense) {
	    reg_invoke_ip_clns_delete_router(((clns_pdbtype *) pdb)->pname);
	} else {
	    csb->protocol = pdb;
	    csb->mode = router_mode;
	}
	return;
    }

    /*
     * IP pdb - check if we're actually a gateway and if IP routing is enabled.
     */
    if (!router_enable || pdb_type != PDB_IP)
	return;
    if (!ip_router_running) {
	printf("IP routing not enabled");
	return;
    }

    /* If it was a legitimate protocol pointer, router_parse has done the work
     * of finding/creating the process for us. The only thing we have left to
     * worry about is deleting processes.
     */
    if (csb->sense) {
	csb->protocol = pdb;
	csb->mode = router_mode;

	if (pdb->proctype & PROC_NONETWORKCMD) {
	    args = malloc(sizeof(spawntype));
	    if (args) {
		args->protocol = pdb;
		args->address = -1;
		args->mask = -1;
		args->sense = csb->sense;
		enqueue(&router_initQ, args);
		if (!router_init_running) {
		    router_init_running = TRUE;
		    process_create(router_init, "Router Init", NORMAL_STACK,
				   PRIO_NORMAL);
		}
	    }
	}
	if (csb->flags & CONFIG_HTTP) {
	    char *new_uri = string_getnext();
	    char *old_uri = csb->http_state->uri;
	    routerprotocolinfo *pi;

	    for (pi = routerprotocols; pi->str; pi++) {
		if (strcasecmp(pi->str, csb->router_name.myname) == 0) {
		    break;
		}
	    }

	    if (!pi->str){
		return;
	    }

	    /* Setup new URI */
	    switch (pi->type) {
	      case ROARG_PID:
	      case ROARG_AS:
		sprintf(new_uri, "/%s/%s/%u", get_mode_name(csb->mode),
			pi->str, csb->router_name.as);
		break;
	      case ROARG_NONE:
		sprintf(new_uri, "/%s/%s", get_mode_name(csb->mode), pi->str);
		break;
	      case ROARG_TAG:
		sprintf(new_uri, "/%s/%s/%s", get_mode_name(csb->mode),
			pi->str, csb->router_name.tag);
		break;
	      default:
		/* Wrong answer */
		return;
	    }

	    /* Call parser and generate help for new URI */
	    csb->http_state->uri = new_uri;
	    reg_invoke_http_parser(csb->http_state, csb->mode, FALSE);

	    /* Put back old URI */
	    csb->http_state->uri = old_uri;
	}
    } else {
	ip_router_erase(pdb);
    }
}

/*
 * router_init
 * A process created by the "network" router subcommand that hangs until
 * the the system has been initialized.  It then starts a router and sets up
 * the networks for which the router process is responsible.
 */

process router_init (void)
{
    spawntype *args;
    pdbtype *pdb;

    process_wait_on_system_init();
    while (!QUEUEEMPTY(&router_initQ)) {
	args = dequeue(&router_initQ);
	pdb = args->protocol;
	if ((pdb->proctype & PROC_NONETWORKCMD) &&
	    (args->address == -1) && (args->mask == -1) && (args->sense)) {
	    if (!pdb->running) {
		if (pdb->proctype & PROC_NEWSCHEDULER) {
		    pdb->pid = process_create((process_t*)pdb->process,
					      pdb->pname, IPROUTING_STACK,
					      PRIO_NORMAL);
		    if (pdb->pid != NO_PROCESS)
			process_set_arg_ptr(pdb->pid, pdb);
		} else {
		    pdb->pid = cfork((forkproc *)pdb->process, (long)pdb, 1500, 
				  pdb->pname, 0);
		}
	    }
	    pdb->running = TRUE;
	} else
	    network_finish(args->protocol, args->address, args->networkflags,
			   args->mask, args->area_id, args->area_id_format,
			   args->sense);
	free(args);
    }
    router_init_running = FALSE;
    process_kill(THIS_PROCESS);
}

/*
 * summary_command
 * Configure summary routes
 * Syntax: "[no] summary-address <address> <mask>
 */

void summary_command (parseinfo *csb)
{
    addrsumtype *sum;
    ipaddrtype address, mask;
    pdbtype *pdb;
    boolean not_advertise;
    ulong tag;

    pdb = csb->protocol;

    if (pdb->family == PDB_OSI) {
	reg_invoke_ip_isis_command_parse(csb);
	return;
    }

    if (csb->nvgen) {
	for (sum = (addrsumtype *) pdb->ip_address_sumq.qhead; sum;
	     sum = sum->next)
	    if ((sum->flags & (ADDRSUM_VALID|ADDRSUM_HIDDEN)) ==
		ADDRSUM_VALID) {
		nv_write(TRUE, "%s %i %i", csb->nv_command, sum->network,
			 sum->mask);
		nv_add(sum->flags & ADDRSUM_NOT_ADV, " not-advertise");
		nv_add(sum->tag, " tag %u", sum->tag);
	    }
	return;
    }

    address = GETOBJ(paddr, 1)->ip_addr;
    mask = GETOBJ(paddr,2)->ip_addr;
    address &= mask;

    for (sum = (addrsumtype *)pdb->ip_address_sumq.qhead; sum; sum = sum->next)
	if ((sum->network == address) && (sum->mask == mask))
	    break;

    not_advertise = GETOBJ(int,2);
    tag = GETOBJ(int,3);
    if (csb->sense) {
	if (!sum) {
	    sum = malloc(sizeof(addrsumtype));
	    if (!sum)
		return;
	    enqueue(&pdb->ip_address_sumq, sum);
	}
	sum->network = address;
	sum->mask = mask;
	sum->metric = METRIC_INACCESSIBLE;
	sum->tag = tag;
	sum->flags = ADDRSUM_VALID;
	if (not_advertise)
	    sum->flags |= ADDRSUM_NOT_ADV;
    } else {
	if (sum) {
	    if (not_advertise)
		sum->flags &= ~ADDRSUM_NOT_ADV;
	    else	
		sum->flags &= ~ADDRSUM_VALID;
	}
    }

    if (sum && pdb->summary_address_cb)
	(*pdb->summary_address_cb)(pdb, csb->sense);
}

/*
 * network_command
 * Set up a particular routing protocol for a network.
 * Syntax: "[no] network <prefix> <mask> [passive]"
 *         "[no] network <prefix> <mask> area <area-id> [passive] [backdoor]"
 */

void network_command (parseinfo *csb)
{
    spawntype *args;
    pdbtype *pdb;

    pdb = csb->protocol;

    /*
     * Command is valid for IP routing protocols only.
     */
    if (pdb->family != PDB_IP) return;

    if (csb->nvgen) {
	int i;

	for (i = 0; i < pdb->netcount; i++) {
	    nv_write(TRUE,"%s %i", csb->nv_command, pdb->networks[i]);
	    if (pdb->proctype & PROC_OSPF)
		nv_add(TRUE," %i area %s",
		       /* print the OSPF mask backwards, until 11.1 */
		       ~pdb->network_mask[i], 
		       reg_invoke_iprouting_area_id(pdb->proctype,
						    pdb, pdb->network_info[i]));
	    else
		nv_add(pdb->network_mask[i] != get_majormask(pdb->networks[i]),
		       " mask %i", pdb->network_mask[i]);
	    nv_add((pdb->networkflags[i] & NETF_PASSIVE)," passive");
	}
	return;
    }

    args = malloc(sizeof(spawntype));
    if (args) {
	args->protocol = csb->protocol;
	args->address = GETOBJ(paddr,1)->ip_addr;
	args->networkflags = 0;
	if (GETOBJ(paddr,2)->type == ADDR_IP)
	    args->mask = GETOBJ(paddr,2)->ip_addr;
	else
	    args->mask = get_majormask(args->address);

	/*
	 * Zero address should only goes with either
	 * zero or all 1's mask, which specify all
	 * interfaces in either new and old way respectively,
	 * other masks do not make any sense.
	 */
	if (!args->address && args->mask && (args->mask != 0xFFFFFFFF)) {
	    printf("\nOSPF: Invalid address/mask combination");
	    free(args);
	    return;
	}
	/*
	 * If someone entered the mask with first bit zero,
	 * and the address is non-zero, treat it as old OSPF
	 * way and fix it by inverting it.
	 * If address is zero, the mask can only be zero
	 * or all 1's.
	 * Zero-address/zero mask is new way to specify 
	 * all interfaces, so don't invert mask.
	 * Zero-address/all 1's mask is old way to 
	 * specify all interfaces, invert the mask.
	 * Note that we treat non-zero address/all 1's mask
	 * as new way to specify single host network instead
	 * of old way to specify all interfaces.
	 */
	if ((args->address && !(args->mask & 0x80000000)) ||
	    (!args->address && (args->mask == 0xFFFFFFFF)))
	    args->mask = ~args->mask;

	args->address &= args->mask;

	if (args->protocol->proctype & PROC_OSPF) {
	    if (GETOBJ(paddr,8)->type == ADDR_IP) {
	        args->area_id_format = OSPF_AREA_ID_FORMAT_IP;
		args->area_id = (ulong) GETOBJ(paddr,8)->ip_addr;
	    } else {
	        args->area_id_format = OSPF_AREA_ID_FORMAT_INT;
		args->area_id = (ulong) GETOBJ(int,1);
	    }
	}

	if (GETOBJ(int,3))
	    args->networkflags |= NETF_PASSIVE;

	args->sense = csb->sense;
	enqueue(&router_initQ, args);
	if (!router_init_running) {
	    router_init_running = TRUE;
	    process_create(router_init, "Router Init", NORMAL_STACK,
			   PRIO_NORMAL);
	}
    }
}

/*
 * network_finish
 * Common code for "network" and commands.
 */

void network_finish (
    register pdbtype *pdb,
    ipaddrtype address,
    uchar networkflags,
    ipaddrtype mask,
    ulong area_id,
    int area_id_format,
    boolean sense)
{
    ndbtype *ndb;
    int i, j;
    boolean deleted_entry;

    /*
     * Check if we're actually a gateway and if this protocol is dynamic.
     */
    if (!router_enable)
	return;
    if (pdb->process == NULL) {
	printf(nodyn);
	return;
    }
    /*
     * Lookup net.  We may get a NULL pointer if the network isn't yet
     * in the route table or has been deleted.  Callers should handle
     * NULL ndb pointers.
     * In case of OSPF, do not do a netsuper_lookup as we might try to 
     * get a supernet due to the presence of mask with the network
     * command.
     */
    if (pdb->proctype & PROC_OSPF)
	ndb = net_lookup(address, FALSE);
    else
	ndb = netsuper_lookup(address, mask, FALSE);

    deleted_entry = FALSE;
    if (sense) {
	if (pdb->netcount >= MAXNETWORKS) {
	    errmsg(&msgsym(TOOMANYNETS, IPRT), pdb->name, pdb->asystem,
		   MAXNETWORKS);
	    return;
	}
	set_ndb_flags(ndb, pdb, networkflags, TRUE);
	for (i = 0; i < pdb->netcount; i++)
	    if ((pdb->networks[i] == address) &&
		(pdb->network_mask[i] == mask)) {
		pdb->networkflags[i] = networkflags;
		if (pdb->proctype & PROC_OSPF)
		    set_ndb_range_flags(ndb, pdb, address, mask, FALSE, sense);
		return;
	    }

	pdb->networks[pdb->netcount] = address;
	pdb->network_mask[pdb->netcount] = mask;
	pdb->networkflags[pdb->netcount] = networkflags;
	pdb->network_info[pdb->netcount] = area_id;
	pdb->netcount++;
	if (pdb->running == FALSE) {
	    if (pdb->proctype & PROC_NEWSCHEDULER) {
		pdb->pid = process_create((process_t*)pdb->process,
					  pdb->pname, IPROUTING_STACK,
					  PRIO_NORMAL);
		if (pdb->pid != NO_PROCESS)
		    process_set_arg_ptr(pdb->pid, pdb);
	    } else {
		pdb->pid = cfork((forkproc *)pdb->process, (long)pdb, 1500, 
				 pdb->pname, 0);
	    }
	    pdb->running = TRUE;
	}
    } else {
	set_ndb_flags(ndb, pdb, FALSE, FALSE);
	for (i = 0; i < pdb->netcount; i++)
	    if ((pdb->networks[i] == address) &&
		(pdb->network_mask[i] == mask)) {
		/*
		 * We only remove the OSPF network command if the
		 * area ID matched.
		 */
		if ((pdb->proctype & PROC_OSPF) && 
		    (pdb->network_info[i] != area_id))
		    continue;
		pdb->netcount--;
		for (j = i; j < pdb->netcount; j++) {
		    pdb->networks[j] = pdb->networks[j+1];
		    pdb->networkflags[j] = pdb->networkflags[j+1];
		    pdb->network_mask[j] = pdb->network_mask[j+1];
		    pdb->network_info[j] = pdb->network_info[j+1];
		}
		deleted_entry = TRUE;
		break;
	    }
    }

    if ((sense == FALSE) && !deleted_entry)
	return;

    if (pdb->network_cb)
	(*pdb->network_cb)(pdb, address, mask, networkflags, area_id, 
			   area_id_format, sense);
}

/*
 * set_ndb_flags_subr
 * Update ndb's attrflags and pdb_overmask
 */
static inline void set_ndb_flags_subr (ndbtype *ndb, pdbtype *pdb,
				       ulong attrflags, boolean sense)
{
    ulong old_pdbmask;

    old_pdbmask = ndb->pdb_overmask;
    if (sense == TRUE) {
	ndb->pdb_overmask |= pdb->mask;
	ndb->attrflags |= attrflags;
    } else {
	ndb->pdb_overmask &= ~pdb->mask;
	ndb->attrflags &= ~attrflags;
    }
    if (old_pdbmask != ndb->pdb_overmask)
	network_redist(pdb, ndb, sense ? ROUTE_UP : ROUTE_MODIF);
}

/*
 * set_ndb_flags
 * Set or clear the pdb type flag for a network and its subnets
 * Routing flags are needed only for the major network itself.
 */

void set_ndb_flags (
    ndbtype *ndb,
    pdbtype *pdb,
    ulong attrflags,
    boolean sense)
{
    int i;
    ndbtype *sdb;

    if (ndb == NULL)
	return;
    if ((ndb->attrflags & NET_ATTACHED) == 0)
	return;

    if (attrflags & NETF_PASSIVE) {
        /*
         * don't copy NETF_PASSIVE into ndb->attrflags
         * it has the same value as NET_SUPERNET
         */
        attrflags &= ~NETF_PASSIVE;
        ndb->passive |= pdb->mask;
    } else {
	ndb->passive &= ~pdb->mask;
    }
    set_ndb_flags_subr(ndb, pdb, attrflags, sense);
    /*
     * If we are subnetted and if the routing protocol knows about subnets,
     * flag the directly connected subnets.
     */
    if ((ndb->attrflags & NET_SUBNETTED) &&
	!(pdb->proctype & (PROC_EGP+PROC_BGP+PROC_OSPF))) { 
	for (i = 0; i < NETHASHLEN; i++) {
	    for (sdb = ndb->subnettable[i]; sdb; sdb = sdb->next) {
		if (sdb->attrflags & NET_ATTACHED) {
		    set_ndb_flags_subr(sdb, pdb, attrflags, sense);
		}
	    }
	}
    }
}

/*
 * set_ndb_range_flags
 * Set or clear the pdb type flag for a range of network and its subnets
 */

void set_ndb_range_flags (
    ndbtype *ndb,
    pdbtype *pdb,
    ipaddrtype address,
    ipaddrtype mask,
    ulong attrflags,
    boolean sense)
{
    int i;
    ndbtype *sdb;
    idbtype *idb;

    if (ndb == NULL) {
	/*
	 * look up all interface addresses and
	 * setup the flag.
	 */
	FOR_ALL_SWIDBS(idb) {
	    if (idb->ip_address 
		&& ((idb->ip_address & mask) == (address & mask))) {
		if ((ndb = net_lookup(idb->ip_address, FALSE)) == NULL)
		    continue;
	    } else 
		if (idb->ip_unnumbered
		    && ((idb->ip_unnumbered->ip_address & mask)
			== (address & mask))) {
		    if ((ndb = net_lookup(idb->ip_unnumbered->ip_address,
					  FALSE)) == NULL)
			continue;
		} else {
		    continue;
		}
	    if (!(ndb->attrflags & NET_ATTACHED))
		continue;
	    set_ndb_flags_subr(ndb, pdb, attrflags, sense);
	    if (!(ndb->attrflags & NET_SUBNETTED))
		continue;
	    for (i = 0; i < NETHASHLEN; i++) {
		for (sdb = ndb->subnettable[i]; sdb; sdb = sdb->next) {
		    if ((sdb->attrflags & NET_ATTACHED)
			&& ((sdb->netnumber & mask) == (address & mask))) {
			set_ndb_flags(sdb, pdb, attrflags, sense);
		    }
		}
	    }
	}
	return;
    }
    if (ndb == NULL)
	return;

    set_ndb_flags_subr(ndb, pdb, attrflags, sense);
    /*
     * If we are subnetted and if the routing protocol knows about subnets,
     * flag the directly connected subnets.
     */
    if (ndb->attrflags & NET_SUBNETTED) {
	for (i = 0; i < NETHASHLEN; i++) {
	    for (sdb = ndb->subnettable[i]; sdb; sdb = sdb->next) {
		if ((sdb->attrflags & NET_ATTACHED)
		    && ((sdb->netnumber & mask) == (address & mask))) {
		    set_ndb_flags_subr(sdb, pdb, attrflags, sense);
		}
	    }
	}
    }
}

/*
 * network_revise
 * Given a network, make sure the appropriate ndb/sdb information is
 * updated in accordance with the routing protocol information.
 * This makes the right routing things happen with static routes.
 */

void network_revise (register ipaddrtype network)
{
    register pdbtype *pdb;
    register int i;

    for (pdb = protoQ.qhead; pdb; pdb = pdb->next)
	for (i = 0; i < pdb->netcount; i++) {
	    if (pdb->networks[i] == (network & pdb->network_mask[i]))
		network_finish(pdb, pdb->networks[i],pdb->networkflags[i], 
			       pdb->network_mask[i], pdb->network_info[i], 
			       OSPF_AREA_ID_FORMAT_IGNORE, TRUE);
	}
}

/*
 * maxpaths_command
 * [no] maximum-paths <number>
 */
void maxpaths_command (parseinfo *csb)
{
    pdbtype *pdb;
    int paths;

    if (!router_enable)
	return;

    pdb = csb->protocol;
    
    if (pdb->family == PDB_OSI) {
	pdb = ((clns_pdbtype *)csb->protocol)->ip_pdb;
	if (!pdb)
	    return;
    }

    if (csb->nvgen) {
	nv_write((pdb->multipath != pdb->def_multipath), "%s %d", 
		 csb->nv_command, pdb->multipath);
	return;
    }

    paths = csb->sense ? GETOBJ(int,1) : pdb->def_multipath;

    if (paths != pdb->multipath) {
	pdb->multipath = paths;
	if (pdb->running) {
	    table_prune_pdb(pdb);
	    if (pdb->query)
		(*pdb->query)(pdb, NULL);
	}
    }
}

/*
 * iprouting_version_command
 * [no] version <n>
 */
 
void iprouting_version_command (parseinfo *csb)
{
    pdbtype *pdb;

    /*
     * Check if we're actually a gateway
     */
    if (!router_enable)
	return;

    pdb = csb->protocol;

    if (csb->nvgen) {
	if (pdb->family == PDB_OSI)
	    return;
	nv_write(pdb->version != 0, "%s %d", csb->nv_command, pdb->version);
	return;
    }

    pdb->version = csb->sense ? GETOBJ(int,1) : 0;
}

/*
 * distance_command
 * "distance <weight> [ip | clns] [<address> <mask> [<list>]]"
 */
 
void distance_command (parseinfo *csb)
{
    pdbtype *pdb;
    int distance, list;
    char *name;
    ipaddrtype address, mask;
    int i, j;
    boolean gotaddress = FALSE;
    clns_pdbtype *c_pdb;
    acl_headertype *acl = NULL;

    /*
     * Check if we're actually a gateway
     */
    if (!router_enable)
	return;

    pdb = csb->protocol;

    if (csb->nvgen) {
	if (pdb->family == PDB_OSI) {
	    c_pdb = (clns_pdbtype *)pdb;
	    /* Write "distance" command. */
	    if (c_pdb->typeflag & ISO_IGRPMASK) {
		nv_write((c_pdb->distance != ISO_IGRP_DIST),
			 "%s %d", csb->nv_command, c_pdb->distance);
	    } else if (c_pdb->typeflag & ISO_ISISMASK) {
		nv_write((c_pdb->distance != ISO_ISIS_DIST),
			 "%s %d", csb->nv_command, c_pdb->distance);
		pdb = c_pdb->ip_pdb;
		if (pdb) {
		    nv_write(pdb->distance != pdb->def_distance,
			     "%s %d ip", csb->nv_command, pdb->distance);
		    for (i = 0; i < pdb->peercount; i++) {
			nv_write(TRUE, "%s %d %i %i", csb->nv_command,
				 pdb->peers[i].distance, pdb->peers[i].address,
				 pdb->peers[i].mask);
			if (pdb->peers[i].acl)
			    nv_add(TRUE, " %s", pdb->peers[i].acl->name);
		    }
		}
	    }
	} else {
	    if (csb->which == DISTANCE_IGRP2) {
		if (pdb->proctype != PROC_IGRP2) 
		    return;
		if (pdb->distance != pdb->def_distance ||
		    pdb->distance2 != pdb->def_distance2) {
		    nv_write(TRUE, "%s %d %d", csb->nv_command, pdb->distance,
			     pdb->distance2);
		}
	    } else {
		if (pdb->proctype & PROC_OSPF) {
		    /*
		     * NVGEN the normal distance command iff
		     * all 3 distances are the same and are
		     * different from the default.
		     */
		    if ((pdb->distance  == pdb->distance2) &&
			(pdb->distance2 == pdb->distance3) &&
			(pdb->distance  != pdb->def_distance)) {
			nv_write(TRUE, "%s %d", csb->nv_command, 
				 pdb->distance);
		    }
		} else {
		    nv_write(!(pdb->proctype & (PROC_BGP|PROC_IGRP2)) &&
			     (pdb->distance != pdb->def_distance),
			     "%s %d", csb->nv_command, pdb->distance);
		}
		for (i = 0; i < pdb->peercount; i++) {
		    nv_write(TRUE, "%s %d %i %i", csb->nv_command,
			     pdb->peers[i].distance, pdb->peers[i].address, 
			     pdb->peers[i].mask);
		    if (pdb->peers[i].acl)
			nv_add(TRUE, " %s", pdb->peers[i].acl->name); 
		}
	    }
	}
	return;
    }

    if (pdb->family == PDB_IP && pdb->process == NULL) {
	printf(nodyn);
	return;
    }

    distance = GETOBJ(int,1);

    /*
     * Get the optional address and mask.
     */
    address = mask = list = 0;
    if (GETOBJ(paddr,1)->type == ADDR_IP) {
	gotaddress = TRUE;
	address = GETOBJ(paddr,1)->ip_addr;
	mask = GETOBJ(paddr,2)->ip_addr;
	address &= ~mask;

	/*
	 * Get an optional access list number
	 */
	list = GETOBJ(int,2);
	name = GETOBJ(string,1);
	if (list || strlen(name))
	    acl = access_parse_acl(list, name, (list == 0),
				   ACL_IP_NAMED_SIMPLE);
    }
    /*	
     * If CLNS pdb, process distance and return.
     */
    if (pdb->family == PDB_OSI) {
	if ((GETOBJ(int,3) == 1) || gotaddress) {
	    pdb = reg_invoke_ip_isis_get_pdb((clns_pdbtype *) pdb);
	    if (!pdb) {
		return;
	    }
	}
	else if (!GETOBJ(int,3)) {
	    reg_invoke_ip_clns_distance((clns_pdbtype *) pdb, distance);
	    return;
	} 
    }

    /*
     * If no address and mask specified, treat distance as the default
     * value for this routing protocol.  Set new default, flush cache,
     * and return.
     */
    if (!gotaddress) {
	if (csb->sense)
	    pdb->distance = distance;
	else
	    pdb->distance = pdb->def_distance;

	if (pdb->proctype == PROC_IGRP2 && csb->which == DISTANCE_IGRP2) {
	    pdb->distance2 = (csb->sense) ? GETOBJ(int,2) : pdb->def_distance2;
	}
	if (pdb->proctype & PROC_OSPF) {
	    if (csb->sense) {
		pdb->distance2 = pdb->distance3 = distance;
	    } else {
		pdb->distance2 = pdb->def_distance2;
		pdb->distance3 = pdb->def_distance3;
	    }
	}
	pdb_flushcache(pdb);
	return;
    }

    /*
     * Address and mask specified.  Linear scan table of table to find
     * any matches.  We may flush an entry or may just update it.
     * Flush cache and return in either case.
     */
    for (i = 0; i < pdb->peercount; i++) {
	if ((pdb->peers[i].address == address)&&(pdb->peers[i].mask == mask)) {
	    if (csb->sense == FALSE) {
		pdb->peercount--;
		for (j = i; j < pdb->peercount; j++) {
		    pdb->peers[j].address = pdb->peers[j+1].address;
		    pdb->peers[j].mask = pdb->peers[j+1].mask;
		    pdb->peers[j].distance = pdb->peers[j+1].distance;
		    pdb->peers[j].acl = pdb->peers[j+1].acl;
		}
	    } else {
		pdb->peers[i].distance = distance;
		pdb->peers[i].acl = acl;
	    }
	    pdb_flushcache(pdb);
	    return;
	}
    }

    /*
     * No matches.  Install a new entry in table if there is room.
     */
    if (pdb->peercount >= MAXPEERS) {
	printf("\nData structure full");
	return;
    }
    i = pdb->peercount++;
    pdb->peers[i].address = address & ~mask;
    pdb->peers[i].mask = mask;
    pdb->peers[i].acl = acl;
    pdb->peers[i].distance = distance;
    pdb_flushcache(pdb);
}

/*
 * ip_get_def_metric_scalar
 * Return the value of the configured default metric for the selected
 * redistribution.  Caller should first check ip_def_metric_configured()
 * before attempting to use this routine to ensure that a metric is
 * actually configured.
 */

ulong ip_get_def_metric_scalar (pdbtype *pdb, int index)
{
    if (pdb->metric[index].flag)
	return(pdb->metric[index].metric.scalar);
    return(pdb->default_metric);
}

/*
 * ip_get_def_metric_vector
 * Pass back the value of the configured default metric for the selected
 * redistribution.  Caller should first check ip_def_metric_configured()
 * before attempting to use this routine to ensure that a metric is
 * actually configured.
 */

void ip_get_def_metric_vector (pdbtype *pdb, int index, struct vector *def)
{
    if (pdb->metric[index].flag) {
	*def = pdb->metric[index].metric.vector;
    } else {
	def->bandwidth = pdb->def_bandwidth;
	def->reliability  = pdb->def_reliability;
	def->delay = pdb->def_delay;
	def->loading = pdb->def_loading;
	def->mtu = pdb->def_mtu;
    }
}

/*
 * redistribute_command
 * Set flag to cause wholesale redistribution of routes
 */

static const char router_ospf_only[] =
    "\n%% Only valid for 'router ospf' or 'router isis'";
static const char redist_ospf_only[] =
    "\n%% Only valid for OSPF redistribution";


void redistribute_command (parseinfo *csb)
{
    pdbtype      *dist, *pdb;
    clns_pdbtype *c_pdb;
    int          pdb_type;
    int          flags = 0;
    char         *str;
    int          len;

    pdb = csb->protocol;

    if (csb->nvgen) {
	if (pdb->family == PDB_OSI) {
	    c_pdb = (clns_pdbtype *) pdb;
	    if (c_pdb->redistallowed != c_pdb->typeflag) {
		reg_invoke_ip_clns_nv_write_redist(c_pdb, csb->nv_command);
	    }
	    if (!(pdb = ((clns_pdbtype *) pdb)->ip_pdb)) {
		return;
	    }
	}
	print_redistribute_command(pdb);
	return;
    }

    pdb_type = (pdb->family == PDB_OSI) ? PDB_UNKNOWN : pdb->family;

    /*
     * Check if we're actually a gateway
     */
    if (pdb_type == PDB_IP && pdb->process == NULL) {
	printf(nodyn);
	return;
    }

    /*
     * Get pdb for routing process from redistribute command.
     */
    if (!(dist = router_parse(csb, FALSE, &pdb_type))) return;

    /*
     * This is the case when processing an (pdb) OSI routing protocol.
     */
    if (pdb->family == PDB_OSI) {
	c_pdb = (clns_pdbtype *) pdb;

	/*
	 * Redistributing a OSI routing protocol into another OSI routing
	 * protocol.
	 */
	if (dist->family == PDB_OSI) {
	    reg_invoke_ip_clns_redistribute(c_pdb, (clns_pdbtype *) dist,
					    csb->sense, GETOBJ(string,1));
	    return;
	}

	/*
	 * Redistributing an IP routing protocol into IS-IS.
	 */
	if (!(c_pdb->typeflag & ISO_ISISMASK)) {
	    printf("redistribution of \"%s\" via \"%s\" not allowed",
		   dist->name, pdb->name);
	    return;
	}
	pdb = reg_invoke_ip_isis_get_pdb(c_pdb);
	if (!pdb) {
	    return;
	}
    }

    /*
     * Potentially redistributing IS-IS into an IP routing protocol.
     */
    if (dist->family == PDB_OSI) {
	c_pdb = (clns_pdbtype *) dist;
	if (!(c_pdb->typeflag & ISO_ISISMASK)) {
	    printf("redistribution of \"%s\" via \"%s\" not allowed",
		   dist->name, pdb->name);
	    return;
	}
	dist = reg_invoke_ip_isis_get_pdb(c_pdb);
	if (!dist) {
	    return;
	}
	pdb_type = PDB_IP;
    }

    /* 
     * pdb_type is an IP routing protocol - check if we're actually a gateway. 
     */
    if (!router_enable)	return;

    if ((pdb == dist) || !(pdb->redistallowed & dist->proctype)) {
	printf("redistribution of \"%s\" via \"%s\" not allowed",
		dist->name, pdb->name);
	return;
    }

    /* GETOBJ(int,1) is a flag word indicating what options were specified */

    if (GETOBJ(int,1) & REDIST_METRIC) {
	if (csb->sense) {
	    if (pdb->proctype & (PROC_IGRP | PROC_IGRP2)) {
		pdb->metric[dist->index].metric.vector.user_bandwidth = 
		    GETOBJ(int,5);
		pdb->metric[dist->index].metric.vector.bandwidth = 
		    SCALED_BANDWIDTH(GETOBJ(int,5));
		pdb->metric[dist->index].metric.vector.delay = GETOBJ(int,6);
		pdb->metric[dist->index].metric.vector.reliability =
		    GETOBJ(int,7);
		pdb->metric[dist->index].metric.vector.loading = GETOBJ(int,8);
		pdb->metric[dist->index].metric.vector.mtu = GETOBJ(int,9);
	    } else {
		pdb->metric[dist->index].metric.scalar = GETOBJ(int,5);
	    }
	    pdb->metric[dist->index].flag = TRUE;
	} else {
	    pdb->metric[dist->index].flag = FALSE;
	}
    }

    if (GETOBJ(int,1) & REDIST_WEIGHT) {
	if (csb->sense)
	    pdb->metric[dist->index].weight = *((int *)&GETOBJ(int,12));
        else
	    pdb->metric[dist->index].weight = 0;
    }

    if (GETOBJ(int,1) & REDIST_METRICTYPE) {
	if (pdb->proctype & PROC_OSPF) {
	    pdb->metric[dist->index].type = csb->sense ? GETOBJ(int,4) :
		DEFAULT_METRIC_TYPE;
	} else if (pdb->proctype & PROC_ISIS) {
	    if (GETOBJ(int,4) == 3) {

		/*
		 * Redistibuting with internal metrics into IS-IS.	
		 */
		pdb->metric[dist->index].metric.scalar &= ~0x80;
	    } else if (GETOBJ(int,4) == 4) {  

		/*
		 * Redistibuting with external metrics into IS-IS.	
		 */
		pdb->metric[dist->index].metric.scalar |= 0x80;
	    }
	} else {
	    printf(router_ospf_only);
	    return;
	}

    }

    /*
     * Redistributing into IS-IS, get level to redistributed into.
     */
    if (pdb->proctype & PROC_ISIS) {
	reg_invoke_ip_isis_redistribute(pdb, dist, FALSE);
	pdb->metric[dist->index].type = GETOBJ(int,10);
    }

    /*
     * If redistributing from IS-IS into another protocol, get level. This
     * configures if either level-1, level-2, or both types of IS-IS
     * routes should be considered.
     */
    if (dist->proctype & PROC_ISIS) {
	flags = 0;
	reg_invoke_ip_isis_redistribute_command_exception(csb);
	if (GETOBJ(int,10) & ISIS_CIRCUIT_L1) {
	    flags |= RTYPE_L1;
	}

	if (GETOBJ(int,10) & ISIS_CIRCUIT_L2) {
	    flags |= RTYPE_L2;
	}
	if (csb->sense) {
	    pdb->criteria[dist->index].routetype |= flags;
	} else {
	    pdb->criteria[dist->index].routetype &= ~flags;
	}
    }

    if (GETOBJ(int,1) & REDIST_TAG) {
	if ((pdb->proctype & PROC_OSPF) == 0) {
	    printf(router_ospf_only);
	    return;
	}
	if (csb->sense) {
	    pdb->metric[dist->index].tag = GETOBJ(int,3);
	    pdb->metric[dist->index].tag_set = TRUE;
	} else {
	    pdb->metric[dist->index].tag = pdb->metric_def[dist->index].tag;
	    pdb->metric[dist->index].tag_set = FALSE;
	}
    }

    if (GETOBJ(int,1) & REDIST_SUBNET) {
	if ((pdb->proctype & PROC_OSPF) == 0) {
	    printf(router_ospf_only);
	    return;
	}
	pdb->metric[dist->index].subnets = csb->sense ?
				    TRUE :
				    pdb->metric_def[dist->index].subnets;
    }

    if (GETOBJ(int,1) & REDIST_MATCH) {
	if ((dist->proctype & PROC_OSPF) == 0) {
	    printf(redist_ospf_only);
	    return;
	}
	if (csb->sense) {
	    pdb->criteria[dist->index].routetype |= GETOBJ(int,2);
	} else {
	    pdb->criteria[dist->index].routetype &= ~GETOBJ(int,2);
	}
    }

    if (pdb->criteria[dist->index].routetype == 0) {
 	if (csb->sense) {
 	    if (dist->proctype & PROC_OSPF) {
 		pdb->criteria[dist->index].routetype = OSPF_IROUTE_TYPES;
 		pdb->criteria_def[dist->index].routetype = OSPF_IROUTE_TYPES;
 		if ((pdb->proctype & (PROC_BGP|PROC_EGP)) == 0) {
 		    pdb->criteria[dist->index].routetype |= OSPF_EROUTE_TYPES;
 		    pdb->criteria_def[dist->index].routetype 
 			|= OSPF_EROUTE_TYPES;
 		}
 	    }
 	}
    }
    if (GETOBJ(int,1) == REDIST_DEFAULT) {    /* No options specified */
	pdb->metric[dist->index].flag = FALSE;
	if (csb->sense) {
	    if (dist->proctype & PROC_OSPF) {
		pdb->criteria[dist->index].routetype = OSPF_IROUTE_TYPES;
		pdb->criteria_def[dist->index].routetype = OSPF_IROUTE_TYPES;
		if ((pdb->proctype & (PROC_BGP|PROC_EGP)) == 0) {
		    pdb->criteria[dist->index].routetype |= OSPF_EROUTE_TYPES;
		    pdb->criteria_def[dist->index].routetype |=
			OSPF_EROUTE_TYPES;
		}
	    }
	} else {
	    pdb->distflag &= ~dist->mask;
	    pdb->metric[dist->index].tag = pdb->metric_def[dist->index].tag;
	    pdb->metric[dist->index].tag_set = FALSE;
	    pdb->metric[dist->index].type = DEFAULT_METRIC_TYPE;
	    pdb->metric[dist->index].subnets 
		= pdb->metric_def[dist->index].subnets;
	    pdb->metric[dist->index].weight = 0;
	    pdb->criteria[dist->index].routetype = 0;
	    pdb->criteria_def[dist->index].routetype = 0;
	    str = pdb->routemap[dist->index];
	    if (str) {
		free(str);
		pdb->routemap[dist->index] = NULL;
	    }
	}
    }

    /*
     * Process route-map.
     */
    if (GETOBJ(int,1) & REDIST_ROUTEMAP) {
	str = pdb->routemap[dist->index];
	if (csb->sense) {
	    if (str)
		free(str);
	    len = strlen(GETOBJ(string,1)) + 1;
	    str = malloc(len);
	    if (!str) {
		printf(nomemory);
		return;
	    }
	    bcopy(GETOBJ(string,1), str, len);
	} else {
	    free(str);
	    str = NULL;
	}
	pdb->routemap[dist->index] = str;
    }
    
    if (csb->sense) {
	/* Mark the redistributed protocol */
	pdb->distflag |= dist->mask;
    }

    /* Doit */
    if (pdb->redistribute)
	(*(pdb->redistribute))(pdb, dist, csb->sense ? TRUE : FALSE);
}

/*
 * distributelist_command
 * Set up access lists to govern the contents of routing updates
 * 	"distribute-list <list> in|out [<router>|<interface>]"
 */

void distributelist_command (parseinfo *csb)
{
    idbtype *idb;
    pdbtype *pdb, *dist;
    int     number, direction;
    int	    pdb_type, i;
    char    *name;
    acl_headertype *acl;

    pdb = csb->protocol;

    /*
     * Command is valid for IP routing protocols only.
     */
    if (pdb->family != PDB_IP)
	return;

    if (csb->nvgen) {
	for (i = 0; i < MAX_IPPROCS; i++) {
	    if (!pdb->ofltr_proto[i].acl)
		continue;
	    dist = INDEX2PDB(i);
	    if (dist) {
		nv_write(TRUE, "distribute-list %s out %s",
			 pdb->ofltr_proto[i].acl->name, dist->name);
		nv_add(dist->asystem, " %d", dist->asystem);
	    }
	}
	FOR_ALL_SWIDBS(idb) {
	    acl = idb->ip_oprotofltr[pdb->index];
	    if (acl)
		nv_write(TRUE,"distribute-list %s out %s",
			 acl->name, idb->namestring); 
	    acl = idb->ip_iprotofltr[pdb->index];
	    if (acl)
		nv_write(TRUE, "distribute-list %s in %s",
			 acl->name, idb->namestring); 
	}
	if (pdb->ofltr_all.acl)
	    nv_write(TRUE,"distribute-list %s out",
		     pdb->ofltr_all.acl->name);
	if (pdb->ifltr_all.acl)
	    nv_write(TRUE,"distribute-list %s in",
		     pdb->ifltr_all.acl->name);
	return;
    }

    if (pdb->process == NULL) {
	printf(nodyn);
	return;
    }
    pdb_type = pdb->family;
    number = GETOBJ(int,1);
    name = GETOBJ(string,1);
    direction = GETOBJ(int,2);
    idb = GETOBJ(idb,1);
    if (!idb)
	dist = router_parse(csb, TRUE, &pdb_type);
    else
	dist = NULL;

    if (csb->sense) {
	acl = access_parse_acl(number, name, (number == 0),
			       ACL_IP_NAMED_SIMPLE);
    } else {
	acl = NULL;
    }

    switch (direction) {
      case IN:
	if (idb)
	    idb->ip_iprotofltr[pdb->index] = acl;
	else
	    pdb->ifltr_all.acl = acl;
 	if (pdb->proctype & PROC_OSPF && pdb->ospf_area_list) {
 	    table_prune_pdb(pdb);
 	    if (pdb->query)
 		(*pdb->query)(pdb, NULL);
 	}
	break;

      case OUT:
	if (dist)
	    pdb->ofltr_proto[dist->index].acl = acl;
	else if (idb) {
	    if (pdb->proctype & PROC_OSPF) {
		printf("\nInterface not allowed with OUT in case of OSPF");
		return;
	    }
	    idb->ip_oprotofltr[pdb->index] = acl;
	} else
	    pdb->ofltr_all.acl = acl;
	if (pdb->proctype & PROC_OSPF && pdb->ospf_area_list)
	    reg_invoke_ospf_perform_asbr_duty(pdb);
	break;
    }

    /* Alert the protocol. */

    if (pdb->acl_changed) {
	(*pdb->acl_changed)(pdb, idb, (direction == OUT), csb->sense);
    }
}

/*
 * nvgen_offsetlist_command
 */
static void nvgen_offsetlist_command (parseinfo *csb)
{
    pdbtype *pdb;
    idbtype *idb;
    offsettype offset;

    pdb = csb->protocol;

    if (pdb->mmetric_list_in)
	nv_write(pdb->mmetric_in, "%s %s in %u", 
		 csb->nv_command, pdb->mmetric_list_in->name,
		 pdb->mmetric_in); 

    if (pdb->mmetric_list_out)
	nv_write(pdb->mmetric_out, "%s %s out %u", 
		 csb->nv_command,  pdb->mmetric_list_out->name,
		 pdb->mmetric_out); 

    FOR_ALL_SWIDBS(idb)
	if (idb->ip_info) {
	    offset = idb->ip_info->in_offlist[pdb->index];
	    if (offset.acl)
		nv_write(offset.metric, "offset-list %s in %u %s",
			 offset.acl->name, offset.metric,
			 idb->namestring); 
	    offset = idb->ip_info->out_offlist[pdb->index];
	    if (offset.acl)
		nv_write(offset.metric, "offset-list %s out %u %s",
			 offset.acl->name, offset.metric,
			 idb->namestring); 
	}
}

/*
 * offsetlist_command
 * Add a constant offset to a metric
 */

void offsetlist_command (parseinfo *csb)
{
    pdbtype *pdb;
    idbtype *idb;
    short   list;
    ulong   metric;
    offsettype *offset;
    char *name;

    pdb = csb->protocol;

    if (csb->nvgen) {
	nvgen_offsetlist_command(csb);
	return;
    }

    if (pdb->process == NULL) {
	printf(nodyn);
	return;
    }

    /*
     * Metric == 0 indicates that offset is not set
     */
    list = GETOBJ(int, 1);
    name = GETOBJ(string,1);

    metric = csb->sense ? GETOBJ(int, 3) : 0;
    idb = GETOBJ(idb, 1);
    
    switch (GETOBJ(int,2)) {		/* IN or OUT */
    case IN:
	if (idb) {
	    if (idb->ip_info){
		offset = &idb->ip_info->in_offlist[pdb->index];
		offset->metric = metric;
		offset->acl = access_parse_acl(list, name, (name[0]),
					       ACL_IP_NAMED_SIMPLE);
	    }
	} else {
	    pdb->mmetric_in = metric;
	    pdb->mmetric_list_in = access_parse_acl(list, name, (name[0]),
						    ACL_IP_NAMED_SIMPLE);
	}
	break;
    case OUT:
	if (idb) {
	    if (idb->ip_info){
		offset = &idb->ip_info->out_offlist[pdb->index];
		offset->metric = metric;
		offset->acl = access_parse_acl(list, name, (name[0]),
					       ACL_IP_NAMED_SIMPLE);
	    }
	} else {
	    pdb->mmetric_out = metric;
	    pdb->mmetric_list_out = access_parse_acl(list, name, (name[0]),
						     ACL_IP_NAMED_SIMPLE);
	}
	break;
    }
}

/*
 * defaultinf_command
 * Control the acceptance ([no] default-information in) and spread ([no]
 * default-information out) of default information.  Mostly for controlling
 * the interaction of defaults for multiple IGRP processes.
 *
 * default-information originate [metric <value>] [metric-type <1 or 2>]
 * default-information allowed [in | out]
 *
 * The RIP and IS-IS form is:
 * [no] default-information originate [route-map <map-name>]
 */

static const char invcmd[] = "%% Invalid command for protocol";

void defaultinf_command (parseinfo *csb)
{
    pdbtype *pdb;
    boolean default_route;
    ulong default_metric;
    uchar default_type;
    boolean default_always;
    boolean changed = FALSE;
    uchar *str;
    int len;
    acl_headertype *acl;

    pdb = csb->protocol;

    /*
     * Don't do IP pdbs that didn't start their process yet.
     */
    if (pdb->family == PDB_IP && pdb->process == NULL) {
	if (!csb->nvgen) {
	    printf(nodyn);
	}
	return;
    }

    /*
     * Might be IS-IS pdb. This is checked in the parser chain.
     */
    if (pdb->family == PDB_OSI) {
	if (!(pdb = ((clns_pdbtype *) pdb)->ip_pdb)) return;
    }

    if (csb->nvgen) {
	switch (GETOBJ(int,1)) {
	  case DEFAULTINF_ORIGINATE:
	    if (!pdb->default_originate)
		break;
	    nv_write(TRUE, csb->nv_command);
	    if (pdb->proctype & PROC_OSPF) {
	      nv_add(pdb->default_always, " always");
	      nv_add(pdb->ospf_default_metric, 
		     " metric %d", pdb->ospf_default_metric);
	      nv_add((pdb->ospf_default_type != METRIC_TYPE2),
		     " metric-type %d", pdb->ospf_default_type);
	    }
	    nv_add(pdb->default_routemap != NULL, " route-map %s", 
		   pdb->default_routemap);
	    break;

	  case DEFAULTINF_IN:
	    if (pdb->proctype & (PROC_IGRP | PROC_IGRP2)) {
		if (!pdb->exteriors_in)
		    nv_write(TRUE, "no %s", csb->nv_command);
		else {
		    if (pdb->exteriors_in_acl)
			nv_write(TRUE, "%s %s", csb->nv_command,
				 pdb->exteriors_in_acl->name);
		}
	    }
	    break;
	  case DEFAULTINF_OUT:
	    if (pdb->proctype & (PROC_IGRP | PROC_IGRP2)) {
		if (!pdb->exteriors_out)
		    nv_write(TRUE, "no %s", csb->nv_command);
		else {
		    if (pdb->exteriors_out_acl)
		    nv_write(TRUE, "%s %s", csb->nv_command, 
			     pdb->exteriors_out_acl->name);
		}
	    }
	    break;
	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    break;
	}
	return;
    }

    switch (GETOBJ(int,1)) {
      case DEFAULTINF_ORIGINATE:
	if (pdb->proctype & (PROC_RIP|PROC_OSPF|PROC_ISIS)) {
	    default_route = pdb->default_originate;
	    default_metric = pdb->ospf_default_metric;
	    default_type = pdb->ospf_default_type;
	    default_always = pdb->default_always;

	    /* metric <value> */
	    if (GETOBJ(int,2)) {
		if (csb->sense) {
		    pdb->ospf_default_metric = GETOBJ(int,3);
		} else
		    pdb->ospf_default_metric = 0;
	    }

	    /* metric-type { 1 | 2 } */
	    if (GETOBJ(int,4)) {
		if (csb->sense) {
		    pdb->ospf_default_type = GETOBJ(int,5);
		} else
		    pdb->ospf_default_type = METRIC_TYPE2;
	    }
		
	    /* always */
	    if (GETOBJ(int,6)) {
		if (csb->sense)
		    pdb->default_always = TRUE;
		else
		    pdb->default_always = FALSE;
	    }
	    
	    /* 
	     * route-map keyword was supplied.
	     */
	    str = NULL;
	    if (*GETOBJ(string,1)) {
		str = pdb->default_routemap;
		if (csb->sense) {
		    if (strcmp(str, GETOBJ(string,1))) {
			len = strlen(GETOBJ(string,1)) + 1;
		        str = malloc(len);
			if (!str) {
			    printf(nomemory);
			    return;
			}
			bcopy(GETOBJ(string,1), str, len);
			changed = TRUE;
		    }
		} else {
		    if (str) {
		        str = NULL;
			changed = TRUE;
		    }
		}
		/*
		 * Remove old default route before the new route map
		 * is applied.
		 */
		if (changed) { 
		    if (default_route && pdb->redist_callback) 
		        (*pdb->redist_callback)(pdb, default_ndb, ROUTE_DOWN,
						DEFAULT_ROUTE);
		    if (pdb->default_routemap)
		        free(pdb->default_routemap);
		    pdb->default_routemap = str;
		}
	    }

	    changed = changed || 
	              (default_metric != pdb->ospf_default_metric) ||
		      (default_type != pdb->ospf_default_type) ||
		      (default_always != pdb->default_always);

	    if (csb->sense || changed) {
		pdb->default_originate = TRUE;
		/*
		 * Call pdb->redistribute first so it will setup
		 * pdb->redist_callback properly.
		 */
		if (pdb->redistribute)
		    if (pdb->default_originate != default_route)
			(*(pdb->redistribute))(pdb, NULL, TRUE);
		if (pdb->redist_callback) {
		    if (default_route != pdb->default_originate || changed) {
		        (*pdb->redist_callback)(pdb, default_ndb, ROUTE_UP,
					        DEFAULT_ROUTE); 
		    } 
		}
	    } else {
		pdb->default_originate = FALSE;
		pdb->default_always = FALSE;
		pdb->ospf_default_metric = 0;
		pdb->ospf_default_type = METRIC_TYPE2;
		if (pdb->default_routemap) {
		    free(pdb->default_routemap);
		    pdb->default_routemap = NULL;
		}
		/*
		 * Call pdb->redist_callback to bring down the default
		 * first. It is because pdb->redistribute could set 
		 * pdb->redist_callback to NULL and causes the default
		 * not removed.
		 */
		if (default_route != pdb->default_originate) {
		    if (pdb->redist_callback) {
		        (*pdb->redist_callback)(pdb, default_ndb, ROUTE_DOWN,
					        DEFAULT_ROUTE); 
		    } 
		    if (pdb->redistribute)
		        (*(pdb->redistribute))(pdb, NULL, FALSE);
		}
	    }
	} else{
	    if (pdb->proctype & (PROC_EGP|PROC_BGP)) {
		pdb->default_originate = csb->sense;
	    } else {
		/*
		 * Not OSPF or EGP.
		 */
		printf(invcmd);
	    }
	}
	return;
	break;
	
      case DEFAULTINF_IN:
	if ((pdb->proctype & (PROC_IGRP | PROC_IGRP2)) == 0) {
	    printf(invcmd);
	}
	pdb->exteriors_in = csb->set_to_default || csb->sense;
	if (csb->sense) {
	    int list;
	    char *name;

	    list = GETOBJ(int,2);
	    name = GETOBJ(string,2);
	    acl = access_parse_acl(list, name, (list == 0),
				   ACL_IP_NAMED_SIMPLE);

	    pdb->exteriors_in_acl = acl;
	} else {
	    pdb->exteriors_in_acl = NULL;
	}
	break;

      case DEFAULTINF_OUT:
	if ((pdb->proctype & (PROC_IGRP | PROC_IGRP2)) == 0) {
	    printf(invcmd);
	}
	pdb->exteriors_out = csb->set_to_default || csb->sense;
	if (csb->sense) {
	    int list;
	    char *name;

	    list = GETOBJ(int,2);
	    name = GETOBJ(string,2);
	    
	    pdb->exteriors_out_acl = access_parse_acl(list, name, (list == 0),
						      ACL_IP_NAMED_SIMPLE);

	} else {
	    pdb->exteriors_out_acl = NULL;
	}
	break;

      default:
	bad_parser_subcommand(csb, GETOBJ(int,1));
	break;
    }
}
  

/*
 * timers_command
 * Set timers for various routing protocols
 */

void timers_command (parseinfo *csb)
{
    pdbtype      *pdb;
    clns_pdbtype *cpdb;

    if (!router_enable)
	return;

    pdb = csb->protocol;

    if (csb->nvgen) {
	if (pdb->family == PDB_OSI) {
	    cpdb = (clns_pdbtype *) pdb;
	    if ((cpdb->typeflag & ISO_IGRPMASK) && 
		csb->which == P_TIMERS_BASIC) {
		nv_write(cpdb->def_broadcasttime != cpdb->broadcasttime ||
			 cpdb->def_invalidtime != cpdb->invalidtime ||
			 cpdb->def_blindtime != cpdb->blindtime,
			 "timers basic %d %d %d", cpdb->broadcasttime/ONESEC,
			 cpdb->invalidtime/ONESEC, cpdb->blindtime/ONESEC);
	    }
	    return;
	}

	switch (csb->which) {
	  case P_TIMERS_BASIC:
	    nv_write(pdb->def_broadcasttime != pdb->broadcasttime ||
		     pdb->def_invalidtime != pdb->invalidtime ||
		     pdb->def_holdtime != pdb->holdtime ||
		     pdb->def_flushtime != pdb->flushtime ||
		     pdb->sleeptime,
		     "%s %d %d %d %d", csb->nv_command,
		     pdb->broadcasttime / ONESEC,
		     pdb->invalidtime / ONESEC,
		     pdb->holdtime / ONESEC,
		     pdb->flushtime / ONESEC);
	    nv_add((boolean)pdb->sleeptime, " %d", pdb->sleeptime);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    /*
     * Command is valid for IP routing protocols and ISO-IGRP only.
     */
    if (pdb->family == PDB_OSI) {
	if (csb->which == P_TIMERS_BASIC) {
	    reg_invoke_ip_clns_basic_timers((clns_pdbtype *) pdb, csb);
	}
	return;
    }

    switch (csb->which) {
      case P_TIMERS_BASIC:
	/* Whenever the timers are changed, we change sleep_timer.
	 * This prevents it from retaining an old, large value which
	 * may cause temporary routing failures due to timing differences
	 * in processing routing information between adjacent routers.
	 */
	if (csb->sense) {
	    pdb->broadcasttime = GETOBJ(int,1) * ONESEC;
	    pdb->invalidtime = GETOBJ(int,2) * ONESEC;
	    pdb->holdtime = GETOBJ(int,3) * ONESEC;
	    pdb->flushtime = GETOBJ(int,4) * ONESEC;
	    pdb->sleeptime = GETOBJ(int,5); /* Specified in MS */
	    if (pdb->sleeptime > pdb->broadcasttime) {
		printf("Warning: sleep time reduced to match broadcast time");
		pdb->sleeptime = pdb->broadcasttime;
		TIMER_START(pdb->sleep_timer, pdb->sleeptime);
	    }
	} else {
	    pdb->broadcasttime = pdb->def_broadcasttime;
	    pdb->invalidtime = pdb->def_invalidtime;
	    pdb->holdtime = pdb->def_holdtime;
	    pdb->flushtime = pdb->def_flushtime;
	    pdb->sleeptime = 0;
	    TIMER_STOP(pdb->sleep_timer);
	}
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * metric_command
 * Set metric parameters for a routing protocol.
 */

void metric_command (parseinfo *csb)
{
    pdbtype      *pdb;
    clns_pdbtype *cpdb;

    if (!router_enable)
	return;

    pdb = csb->protocol;

    if (csb->nvgen) {
	if (pdb->family == PDB_OSI) {
	    cpdb = (clns_pdbtype *) pdb;
	    if ((cpdb->typeflag & ISO_IGRPMASK) 
		&& csb->which == P_METRIC_WEIGHTS) {
		nv_write((cpdb->igrp_k1 != cpdb->igrp_k1_def ||
			  cpdb->igrp_k2 != cpdb->igrp_k2_def ||
			  cpdb->igrp_k3 != cpdb->igrp_k3_def ||
			  cpdb->igrp_k4 != cpdb->igrp_k4_def ||
			  cpdb->igrp_k5 != cpdb->igrp_k5_def),
			 "metric weights 0 %u %u %u %u %u",
			 cpdb->igrp_k1, cpdb->igrp_k2, cpdb->igrp_k3,
			 cpdb->igrp_k4, cpdb->igrp_k5);
	    }
	    return;
	}

	switch (csb->which) {
	  case P_METRIC_WEIGHTS:
	    nv_write((pdb->igrp_k1 != pdb->igrp_k1_def ||
		      pdb->igrp_k2 != pdb->igrp_k2_def ||
		      pdb->igrp_k3 != pdb->igrp_k3_def ||
		      pdb->igrp_k4 != pdb->igrp_k4_def ||
		      pdb->igrp_k5 != pdb->igrp_k5_def),
		     "%s 0 %u %u %u %u %u", csb->nv_command,
		     pdb->igrp_k1, pdb->igrp_k2, pdb->igrp_k3,
		     pdb->igrp_k4, pdb->igrp_k5);
	    break;
	  case P_METRIC_HOPCOUNT:
	    nv_write(pdb->igrp_maxhops != pdb->igrp_def_maxhops,
		     "%s %d", csb->nv_command, pdb->igrp_maxhops);
	    break;
	  case P_METRIC_HOLDDOWN:
	    nv_write(pdb->def_holdtime && !pdb->holdokay, "no %s",
		     csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    if (pdb->family == PDB_IP && !(pdb->proctype & (PROC_IGRP | PROC_IGRP2))) {
	printf("metric command supported only for IGRP");
	return;
    }

    /*
     * Process "metric weights" command for ISO-IGRP.
     */
    if (pdb->family == PDB_OSI) {
	if (((clns_pdbtype *) pdb)->typeflag & ISO_IGRPMASK) {
	    if (csb->which == P_METRIC_WEIGHTS) {
		reg_invoke_ip_clns_metric_command((clns_pdbtype *) pdb, csb);
	    }
	} else {
	    printf("Metric command supported only for ISO-IGRP");
	}
 	return;
    }

    /*
     * Since all our dynamic information will be inconsistent, we erase
     * all our dynamic information right now with table_prune_pdb().  After we
     * have set up the new metric values, we call table_reload() to get
     * new information.
     */
    switch (csb->which) {
    case P_METRIC_HOPCOUNT:
	if (!csb->sense) {
	    SETOBJ(int,1) = pdb->igrp_def_maxhops;
	}
	if (pdb->igrp_maxhops == GETOBJ(int,1)) {
	    /* Nothing changed */
	    return;
	}

	/* Notify the protocol, if it so wishes. */

	if (pdb->hopcount_change) {
	    (*pdb->hopcount_change)(pdb, GETOBJ(int,1));
	}

	table_prune_pdb(pdb);
	pdb->igrp_maxhops = GETOBJ(int,1);
	break;
    case P_METRIC_HOLDDOWN:
	if (csb->set_to_default)
	    csb->sense = TRUE;
	if (pdb->holdokay == csb->sense) {
	    /* Nothing changed */
	    return;
	}
	table_prune_pdb(pdb);
	pdb->holdokay = csb->sense;
	break;
    case P_METRIC_WEIGHTS:
	table_prune_pdb(pdb);
	if (!csb->sense) {
	    pdb->igrp_k1 = pdb->igrp_k1_def;
	    pdb->igrp_k2 = pdb->igrp_k2_def;
	    pdb->igrp_k3 = pdb->igrp_k3_def;
	    pdb->igrp_k4 = pdb->igrp_k4_def;
	    pdb->igrp_k5 = pdb->igrp_k5_def;
	} else {
	    pdb->igrp_k1 = GETOBJ(int,1);
	    pdb->igrp_k2 = GETOBJ(int,2);
	    pdb->igrp_k3 = GETOBJ(int,3);
	    pdb->igrp_k4 = GETOBJ(int,4);
	    pdb->igrp_k5 = GETOBJ(int,5);
	    /* TOS = GETOBJ(int,6), which the parser forces to be zero */
	}
	if (pdb->metric_cmd)
	    (*pdb->metric_cmd)(pdb);
	break;
    }

    /*
     * Done changing parameters.  Reload the routing table.
     */
    table_reload(NULL);
    ip_route_new_default();
}

/*
 * variance_command
 * Control the amount of multipathing that we're willing to do.
 */

void variance_command (parseinfo *csb)
{
    pdbtype *pdb;

    pdb = csb->protocol;

    /*
     * Command is valid for IP routing protocols only.
     */
    if (pdb->family != PDB_IP) return;

    if (csb->nvgen) {
	if (pdb->proctype & (PROC_IGRP | PROC_IGRP2)) {
	    nv_write(pdb->igrp_variance != IGRP_DEFVARIANCE, 
		     "%s %d", csb->nv_command, pdb->igrp_variance); 
	}
	return;
    }

    if ((pdb->proctype & (PROC_IGRP | PROC_IGRP2)) == 0) {
	printf("Variance only supported for IGRP");
	return;
    }
    if (csb->sense) {
	pdb->igrp_variance = GETOBJ(int,1);
    } else 
	pdb->igrp_variance = IGRP_DEFVARIANCE;
}

/*
 * defaultmetric_command
 * Set a default metric for readvertisement.
 *	For IGRP:
 *	   "default-metric <bandwidth> <delay> <reliability> <loading>"
 *	For other protocols:
 * 	   "default-metric <scalar>" for most protocols.
 */

void defaultmetric_command (parseinfo *csb)
{
    register pdbtype *pdb;

    if (!router_enable) {
	return;
    }

    pdb = csb->protocol;

    /*
     * Command is valid for IP routing protocols only.
     */
    if (pdb->family != PDB_IP)
	return;

    if (csb->nvgen) {
	if ((pdb->proctype & (PROC_IGRP | PROC_IGRP2)) && pdb->def_bandwidth) {
	    nv_write(pdb->defaultmetricflag,
		     "%s %u %u %d %d %d", csb->nv_command,
		     pdb->user_bandwidth,
		     pdb->def_delay, pdb->def_reliability,
		     pdb->def_loading, pdb->def_mtu);
	} else {
	    nv_write(pdb->defaultmetricflag,
		     "%s %d", csb->nv_command,
		     pdb->default_metric);
	}
	return;
    }

    if (csb->sense == FALSE) {
	pdb->defaultmetricflag = FALSE;
    } else {
	pdb->defaultmetricflag = TRUE;
	if (pdb->proctype & (PROC_IGRP|PROC_IGRP2)) {
	    pdb->user_bandwidth = GETOBJ(int,1);
	    pdb->def_bandwidth = SCALED_BANDWIDTH(GETOBJ(int,1));
	    pdb->def_delay = GETOBJ(int,2);
	    pdb->def_reliability = GETOBJ(int,3);
	    pdb->def_loading = GETOBJ(int,4);
	    pdb->def_mtu = GETOBJ(int,5);
	} else {
	    pdb->default_metric = GETOBJ(int,1);
	}
    }
    if (pdb->defmetric_cmd) {
	(*pdb->defmetric_cmd)(pdb, csb->sense);
    }
}

/*
 * area_command
 * configure area 
 */

void area_command (parseinfo *csb)
{
    pdbtype *pdb;
	
    pdb = csb->protocol;

    /*
     * Command is valid for IP routing protocols only.
     */
    if (pdb->family != PDB_IP) return;

    if (pdb->area)
	(*pdb->area)(pdb, csb, csb->sense);
    else
	printf("\nInvalid command for %s", pdb->name);
}

/*
 * neighbor_command
 * Set/clear a neighbor.
 */

void neighbor_command (parseinfo *csb)
{
    pdbtype *pdb;
	
    pdb = csb->protocol;

    /*
     * Command is valid for IP routing protocols only.
     */
    if (pdb->family != PDB_IP)
	return;

    /*
     * Each neighbor function must be prepared to handle other
     * routing protocols' commands and report errors.
     */

    if (pdb->neighbor)
	(*pdb->neighbor)(pdb, GETOBJ(paddr,1)->ip_addr, csb, csb->sense);
    else
	neighbor_simple(pdb, GETOBJ(paddr,1)->ip_addr, csb, csb->sense);
}

/*
 * neighbor_simple
 * Handle a simple (non-EGP, non-BGP) neighbor
 */

static void neighbor_simple (pdbtype *pdb, ipaddrtype address,
		      parseinfo *csb, boolean sense)
{
    neighbortype **neighp;
    neighbortype *neigh;

    if (csb->nvgen) {
	for (neigh = pdb->neighbors; neigh; neigh = neigh->next) {
	    nv_write(TRUE, "%s %i", 
		     csb->nv_command, neigh->address);
	}
	return;
    }

    /*
     * Scan list for match.  Delete if sense == FALSE.
     * Return now if item already in list
     */ 
    neighp = &(pdb->neighbors);
    while ((*neighp)) {
	if ((*neighp)->address == address) {
	    if (!sense) {
		neigh = (*neighp);
		(*neighp) = (*neighp)->next;
		free(neigh);
	    }
	    return;
	}
	neighp = &((*neighp)->next);
    }

   /*
    * Allocate a new neighbor record if adding
    */
    if (!sense)
	return;
    neigh = malloc(sizeof(neighbortype));
    if (neigh) {
	neigh->address = address;
	neigh->next = pdb->neighbors;
	neigh->pdb = pdb;
	pdb->neighbors = neigh;
    } else {
	printf(nomemory);
    }
}

/*
 * passive_interface_command
 * Set/Clear an interface [Serial 0 etc.] as a passive or listen only
 * interface for the associated router process. This command is a
 * further refinement of the "[no] network xx.xx.00.00 passive" router
 * sub-command. This router sub-command makes the interface passive.
 */

void passive_interface_command (parseinfo *csb)
{
    pdbtype *pdb;
    idbtype *idb;

    pdb = csb->protocol;

    idb = GETOBJ(idb,1);
    if (csb->nvgen) {
	if (pdb->family == PDB_OSI) {
	    if (!(pdb = ((clns_pdbtype *) pdb)->ip_pdb)) return;
	}

	/*
	 * Write out passive interfaces 
	 */
	FOR_ALL_SWIDBS(idb) {
	    if ((idb->ip_passive_router & pdb->mask) &&
		(idb->hwptr->type != IDBTYPE_VACCESS)) {
		nv_write(TRUE, "%s %s", csb->nv_command, idb->namestring);
	    }
	}
	return;
    }

    if (!idb) {
	printf("\nBad interface name");
	return;
    }

    /*
     * Command is valid for IP routing protocols only, IS-IS is
     * sometimes considered an IP routing protocol.
     */
    if (pdb->family == PDB_OSI) {
	pdb = reg_invoke_ip_isis_get_pdb((clns_pdbtype *) pdb);
	if (!pdb)
	    return;
    }

    if (csb->sense)
	idb->ip_passive_router |= pdb->mask;
    else
	idb->ip_passive_router &= ~pdb->mask;

    reg_invoke_ip_passive_cmd_route_adjust(pdb->proctype, pdb, idb, 
						   csb->sense);

    /*
     * If there is a change to virtual template interface
     * propagate the changes to its active virtual access interfaces
     */
    if (idb->hwptr->type == IDBTYPE_VTEMPLATE)
    reg_invoke_vtemplate_passive_interface(idb->hwptr, pdb, csb->sense);

}

/*
 * outdelay_command
 * Control the inter-frame gap on RIP updates.
 */
void outdelay_command (parseinfo *csb)
{
    pdbtype *pdb;

    pdb = csb->protocol;
    if (csb->nvgen) {
        nv_write(pdb->output_delay, "%s %d",
                 csb->nv_command, pdb->output_delay);
        return;
    }
    if (csb->sense) {
	pdb->output_delay = GETOBJ(int,1);
    } else 
	pdb->output_delay = 0;
}

/*
 * synchronization_command
 * Enable or disable IGP synchronization for a protocol.  
 */

void synchronization_command (parseinfo *csb)
{
    pdbtype *pdb;

    pdb = csb->protocol;
    if (csb->nvgen) {
	nv_write(pdb->sync_disabled, "no %s", csb->nv_command);
	return;
    }

    if (pdb->family != PDB_IP) 
	return;
    if (!(pdb->proctype & PROC_BGP)) {
	printf("\nInvalid command for %s", pdb->name);
	return;
    }
    pdb->sync_disabled = !(csb->set_to_default || csb->sense);
}

/*
 * table_map_command
 * Set a table map for a router
 */

void table_map_command (parseinfo *csb)
{
    pdbtype *pdb;
    char *new_map;

    pdb = csb->protocol;
    if (csb->nvgen) {
	if (pdb->table_map)
	    nv_write(TRUE, "%s %s",
		 csb->nv_command, pdb->table_map->mapname);
	return;
    }

    if (pdb->family != PDB_IP) 
	return;

    if (!(pdb->proctype & PROC_BGP)) {
	printf("\nInvalid command for %s", pdb->name);
	return;
    }

    new_map = GETOBJ(string,1);

    if (csb->sense) {
	if (!route_map_config(&pdb->table_map, new_map, NULL))
	    return;
    } else
	route_map_deconfig(&pdb->table_map);
}

/*
 * pdb_distance
 * Return distance associated with this gateway and protocol.
 * If there is an access list associated with that source of information,
 * apply the list. 
 */

int pdb_distance (
    register pdbtype *pdb,
    ipaddrtype source,
    ipaddrtype target,
    ipaddrtype mask,
    int flags,
    ushort route_type)
{
    peertype *peer;
    int i, peerdistance[2], pdb_peerindex, pdb_def_distance;
    acl_headertype *acl = NULL;

    if (!pdb)
	return(UNKNOWN_DISTANCE);

    /*
     * Support floating static routes by retrieving distance from the
     * pdb were we stuck it just before calling network_update().
     * More obvious argument passing could be wished for.
     */
    if (pdb == pdb_STATIC)
	return(pdb->static_distance);
	
    peerdistance[0] = pdb->distance;
    peerdistance[1] = pdb->distance2;

    pdb_def_distance = pdb->distance;
    pdb_peerindex = 0;
    if (pdb->distance_cb) {
	/*
	 * Protocols which maintain multiple administrative distances
	 * wish to select the default distance dynamically.
	 */
	pdb_def_distance = (*pdb->distance_cb)(pdb, target, route_type,
					       flags, &pdb_peerindex);
	peerdistance[pdb_peerindex] = pdb_def_distance;
    }

    /*
     * Usually we've heard from this peer.  Check in our peer cache
     * and return the appropriate distance metric.
     */
    for (peer = pdb->peercache[nethash(source)]; peer; peer = peer->next) {
	if (peer->source == source) {
	    GET_TIMESTAMP(peer->timer);
	    acl = peer->acl;
	    peerdistance[0] = peer->distance;
	    peerdistance[1] = peer->distance2;
	    goto checklist;
	}
    }

    /*
     * Make sure the source of information is not self generated.
     */ 
    if (pdb != pdb_CONNECTED)
	if (ip_up_ouraddress(source))
	    return(UNKNOWN_DISTANCE);

    /*
     * We've never peered with this source before.  First we see if
     * there is a specific neighbor entry for this source.  If not, we
     * use the defaults for the given protocol.
     */
    for (i = 0; i < pdb->peercount; i++) {
	if ((source & ~pdb->peers[i].mask) == pdb->peers[i].address) {
	    acl = pdb->peers[i].acl;
	    peerdistance[0] = pdb->peers[i].distance;
	    if (pdb->peers[i].distance2)
		peerdistance[1] = pdb->peers[i].distance2;
	    break;
	}
    }

    /*
     * We have a distance.  Construct a new cache entry.
     */
    peer = malloc(sizeof(peertype));
    if (peer) {
	peer->source = source;
	peer->distance = peerdistance[0];
	peer->distance2 = peerdistance[1];
	peer->acl = acl;
	GET_TIMESTAMP(peer->timer);
	i = nethash(source);
	peer->next = pdb->peercache[i];
	pdb->peercache[i] = peer;
    }

 checklist:

    /*
     * For BGP/OSPF, the cached distance is probably wrong, because 
     * the correct distance depends upon the route type.
     * See if cached distance is an explicit distance.
     */
    if (pdb->proctype & (PROC_BGP|PROC_OSPF)) {
	for (i = 0; i < pdb->peercount; i++) {
	    if ((source & ~pdb->peers[i].mask) == pdb->peers[i].address) {
		acl = pdb->peers[i].acl;
		peerdistance[0] = peer->distance;
		break;
	    }
	}
	/* 
	 * No explicit distance has been specified for this source, hence
	 * use the distance returned by bgp_distance()/ospf_distance().
	 */
	if (i == pdb->peercount)
	    return(pdb_def_distance);
    }

    /*
     * If there's no list, or if the address is accepted by the list, then
     * use the peer distance.
     */
    if (!acl || nfast_check(acl, target))
	return(peerdistance[pdb_peerindex]);
    /*
     * Use pdb's default distance
     */
    return(pdb_def_distance);
}

/*
 * offset_metric
 * Return the offset to be added to the metric of this entry.
 */
ulong offset_metric (pdbtype *pdb, idbtype *idb, ipaddrtype network, 
		     int direction)
{
    offsettype offset;
    
    switch(direction) {
    case IN:
	if (idb && idb->ip_info) {
	    offset = idb->ip_info->in_offlist[pdb->index];
	    if (offset.metric && ((offset.acl == NULL) ||
				  nfast_check(offset.acl, network)))
		return(offset.metric);
	}
	if (pdb && pdb->mmetric_in && 
	    ((pdb->mmetric_list_in == NULL) ||
	     nfast_check(pdb->mmetric_list_in, network)))
	    return (pdb->mmetric_in);
	break;
    case OUT:
	if (idb && idb->ip_info) {
	    offset = idb->ip_info->out_offlist[pdb->index];
	    if (offset.metric && ((offset.acl == NULL) ||
				  nfast_check(offset.acl, network)))
		return(offset.metric);
	}
 	if (pdb && pdb->mmetric_out && 
	    ((pdb->mmetric_list_out == NULL) ||
	     nfast_check(pdb->mmetric_list_out, network)))
	    return (pdb->mmetric_out);
	break;
    }
    return(0);
}

/*
 * pdb_flushcache
 * Flush the cache of peers we have talked to recently.
 * Do this whenever we change a neighbor parameter
 */

void pdb_flushcache (pdbtype *pdb)
{
    int i;
    peertype *peer, *next;

    for (i = 0; i < NETHASHLEN; i++) {
	peer = pdb->peercache[i];
	pdb->peercache[i] = NULL;
	while (peer) {
	    next = peer->next;
	    peer->next = NULL;
	    free(peer);
	    peer = next;
	}
    }
}

/*
 * pdb_sawmajor
 * Returns TRUE if the major net of the argument address already
 * appeared in the list of IP addresses for this interface.  Used
 * to make sure we don't send extra updates on an interface with
 * secondary addresses
 */

boolean pdb_sawmajor (idbtype *idb, ipaddrtype address)
{
    ipaddtype *ptr;
    ipaddrtype majornet;

    if (idb->ip_split_disabled)
	return(FALSE);
    if (address == idb->ip_address)
	return(FALSE);
    majornet = get_majornet(address);
    if (majornet == get_majornet(idb->ip_address))
	return(TRUE);
    for (ptr = (ipaddtype *) idb->ip_addressQ.qhead; ptr; ptr = ptr->next) {
	if (address == ptr->address)
	    return(FALSE);
	if (majornet == get_majornet(ptr->address))
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * pdb_majorupdate
 * Broadcast a general routing update for a specific routing protocol
 * Interface must have line protocol up.  Network must not be marked passive
 * AND the interface must not be passive.
 * If neighbors are defined, ship a routing update to them; regardless
 * of the passivity of the network or interface.
 */

void pdb_majorupdate (register pdbtype *pdb)
{
    idbtype *idb;
    ndbtype *ndb;
    neighbortype *neigh;
    ipaddrtype address, source, dummy, mask;
    ipaddrtype idbaddress;
    boolean idbfound;
    ipaddtype *ptr;
    int i;

    /*
     * Don't send updates if we are loading up.
     */
    if (system_loading)
	return;
    
    FOR_ALL_SWIDBS(idb) {
	if (idb->ip_enabled && !idb->static_routing && interface_up(idb)) {

	    /*
	     * First send updates on the primary address.
	     * This includes the major net information for the
	     * secondary addresses.
	     */
	    source = idb->ip_address;
	    mask = idb->ip_nets_mask;
	    if (!source && idb->ip_unnumbered) {
		source = idb->ip_unnumbered->ip_address;
		mask = idb->ip_unnumbered->ip_nets_mask;
	    }
	    if (ip_pdb_net_advertise(pdb, source, &ndb)) {
		if (!(ndb->passive & pdb->mask) &&
		    !(idb->ip_passive_router & pdb->mask))
		    (*pdb->majorupdate)(pdb, ndb, -1, idb, 0L, source, mask);
	    }

	    /*
	     * Now send updates on the secondary addresses.
	     * We do this to disseminate the subnet information for the
	     * secondary.  Major net information went out with the primary
	     * address update.  Avoid multiple updates on a network.
	     */
	    for (ptr = (ipaddtype *) idb->ip_addressQ.qhead; ptr;
		 ptr = ptr->next) {
		if (pdb_sawmajor(idb, ptr->address))
		    continue;
		if (ip_pdb_net_advertise(pdb, ptr->address, &ndb)) {
		    if (!(ndb->passive & pdb->mask) &&
			!(idb->ip_passive_router & pdb->mask))
			(*pdb->majorupdate)(pdb, ndb, -1, idb, 0L,
					    ptr->address, ptr->mask); 
		}
	    }
	}
    }

    /*
     * The following code will attempt to send updates to specific
     * neighbor addresses -- it initially makes the assumption that
     * the neighbor address is probably directly connected.
     *
     * In the presence of unnumbered serial interfaces it would be
     * wrong here to use the unnumbered interface's pointer.  This
     * would cause updates for the unnumbered's partner net to
     * go out the wrong side.  Use it's ip_address which will be zero
     * and cause this check to fail.  This is subtle.
     */
    for (neigh = pdb->neighbors; neigh; neigh = neigh->next) {
	address = neigh->address;
	idbfound = FALSE;
	FOR_ALL_SWIDBS(idb) {
	    if (idb->ip_enabled && !idb->static_routing && interface_up(idb)) {
		idbaddress = (idb->ip_address & idb->ip_nets_mask);
		if (idb->ip_address &&
		    (idbaddress == (address & idb->ip_nets_mask))) {
		    if (ip_pdb_net_advertise(pdb, address, &ndb)) {
			idbfound = TRUE;
			(*pdb->majorupdate)(pdb, ndb, address, idb, 0L,
					    idb->ip_address,
					    idb->ip_nets_mask);
		    }
		}
		for (ptr = (ipaddtype *) idb->ip_addressQ.qhead; ptr;
		     ptr = ptr->next) {
		    idbaddress = (ptr->address & ptr->mask);
		    if (idbaddress == (address & ptr->mask)) {
			if (ip_pdb_net_advertise(pdb, address, &ndb)) {
			    idbfound = TRUE;
			    (*pdb->majorupdate)(pdb, ndb, address, idb, 0L,
						ptr->address,
						ptr->mask);
			}
		    }
		    
		}
	    }
	}

	/*
	 * If the neighbor address was not located on a connected idb
	 * -- try to determine where to ship the routing update.
	 */
	if (!idbfound) {
	    dummy = address;
	    idb = reg_invoke_ip_first_hop(&dummy, dummy, IP_PATH_DEFFLAGS);
	    if (idb && !idb->static_routing) {
		for (i = 0; i < pdb->netcount; i++) {
		    ndb = net_lookup(pdb->networks[i], FALSE);
		    if (ndb)
			(*pdb->majorupdate)(pdb, ndb, address, idb, 0, 0, 0);
		}
	    }
	}
    }
}

/*
 * pdb_topology
 * Signal a routing process that a change in network topology has
 * occurred, i.e. an interface has come up or gone down.
 *
 * In the case of an unnumbered interface we consider it part
 * of it's numbered interface's network.  So it the unnumbered
 * interface has changed state we will generate updates out
 * both sides.
 */

void pdb_topology (idbtype *idb)
{
    register pdbtype *pdb;
    register ndbtype *ndb;
    ipaddrtype address;
    ipaddtype *ptr;
    ulong pdbmask;

    if (!router_enable || !idb->ip_enabled)
	return;
    address = idb->ip_address;
    if (!address && idb->ip_unnumbered)
	address = idb->ip_unnumbered->ip_address;
    ndb = net_lookup(address, FALSE);
    if (ndb) {
	pdbmask = ip_ndb_pdbmask(ndb);
	ptr = (ipaddtype *) idb->ip_addressQ.qhead;
	for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	    /*
	     * If the pdbmask does not appear to include this pdb, start
	     * walking through the secondary address list until it does, or
	     * until we finish checking secondary addresses.
	     * This way we minimize calls to net_lookup() when the pdb matches,
	     * and we don't miss a pdb mask match which only occurs via a
	     * secondary address.
	     */
	    if ((pdbmask & pdb->mask) == 0) {
		while (ptr) {
		    ndb = net_lookup(ptr->address, FALSE);
		    if (ndb) {
			pdbmask |= ip_ndb_pdbmask(ndb);
			if (pdbmask & pdb->mask)
			    break;
		    }
		    ptr = ptr->next;
		}
	    }
	    if (pdbmask & pdb->mask) {
		pdb->changed++;
		process_wakeup(pdb->pid);
		if (pdb->route_adjust) {
		    (*pdb->route_adjust)(pdb, idb);
		}
	    }
	}
    } else 
	/*
	 * if we cannot find the ndb, i.e. the connected interface is down,
	 * returns immediately unless the interface is unnumbered.   
	 * We should notify the routing protocols when an unnumbered 
	 * interface changes state even when the numbered interface it 
	 * refers to is still down.
	 */
	if (!idb->ip_unnumbered)
	    return;

    /*
     * Notify other protocols about this interface event.
     */
    reg_invoke_ip_interface_route_adjust(idb, FALSE);
}

/*
 * pdb_outfilter
 * Apply our output access lists before including a network in an update
 */

boolean pdb_outfilter (
    pdbtype *pdb,
    idbtype *interface,
    pdbtype *src_pdb,
    ipaddrtype address)
{
    acl_headertype *acl;

    if (interface) {
	acl = interface->ip_oprotofltr[pdb->index];
	if (acl && !network_check(acl, 0, address))
	    return(FALSE);
    }
    if (src_pdb) {
	acl = pdb->ofltr_proto[src_pdb->index].acl;
	if (acl && !network_check(acl, 0, address))
	    return(FALSE);
    }
    acl = pdb->ofltr_all.acl;
    if (acl && !network_check(acl, 0, address))
	return(FALSE);
    return(TRUE);
}

/*
 * pdb_routemap
 *
 * If a route-map is configured for the protocol being redistributed,
 * check the match conditions and return result. Otherwise return TRUE.
 */
boolean pdb_routemap (pdbtype *pdb, pdbtype *src_pdb, ndbtype *db)
{
    if (pdb->routemap[src_pdb->index]) {
	pdb->route_map = route_map_ip_check(pdb->routemap[src_pdb->index],
					    db,
					    src_pdb);
	if (!pdb->route_map)
	    return(FALSE);
    } else
	pdb->route_map = NULL;
    return(TRUE);
}

/*
 * pdb_routemap_for_default
 *
 * If a route-map is configured for default origination, check the match
 * parameters for an ndb in the routing table. Save set conditions.
 */
boolean pdb_routemap_for_default (pdbtype *pdb)
{

    pdb->route_map = NULL;
    if (!pdb->default_routemap) {
	return(TRUE);
    }

    pdb->route_map = route_map_default_check(pdb->default_routemap);

    if (!pdb->route_map) 
	return(FALSE);

    return(TRUE);
}

/*
 * pdb_interface_for_default
 * Check if default route can be originated and sent out of an interface
 * return TRUE if there is no "set interface" clause at all in the routemap;
 * otherwise, return TRUE only if the interface is in the "set interface" 
 * clause.   Currently used by RIP. 
 */
boolean
pdb_interface_for_default (pdbtype *pdb, idbtype *interface)
{
    route_map_interface **iptr;

    if (!pdb->default_originate)
	return (FALSE);
    if (!pdb->default_routemap)
	return (TRUE);

    /* It allows default, and has a routemap */
    pdb->route_map = route_map_default_check(pdb->default_routemap);
    if (pdb->route_map) {
	if (!(pdb->route_map->set_list.set_flags & ROUTE_MAP_INTERFACE_SET))
	    return (TRUE);
	iptr = &(pdb->route_map->set_list.interface);
	for (; *iptr; iptr = &(*iptr)->next) {
	    if ((*iptr)->idb == interface)
		return (TRUE);
	}
    }
    return (FALSE);
}

/*
 * show_ipproto_summary
 * Display a table of IP protocol information.
 */
void show_ipproto_summary (parseinfo *csb)
{
    pdbtype *dist;

    printf("\nIndex Process Name");
    for (dist = (pdbtype *) protoQ.qhead; dist; dist = dist->next) {
	printf("\n%6d%s", dist->index, dist->name);
	if (dist->asystem || (dist->proctype & PROC_EGP))
	    printf(" %d", dist->asystem);
	if (dist->proctype == PROC_ISIS)
	    printf(" %s", dist->osi_pdb->pname);
    }
}

/*
 * show_ipprotocol
 * Show the various IP routing protocols that are running.
 */

static const char peerheader[] = 
                        "    Gateway         Distance      Last Update";
static const char distheader[] = 
                        "    Address         Wild mask       Distance  List";
static const char passiveheader[] = "  Passive Interface(s):";

void show_ipprotocol (parseinfo *csb)
{
    pdbtype *pdb, *last_pdb;
    peertype *peer, *last_peer;
    int i;
    pdbtype *proto;
    int headerflag;
    idbtype *idb;
    addrsumtype *sum;
    offsettype offset;
    acl_headertype *acl;

    i = 0;				/* stupid compiler */
    automore_enable(NULL);

    pdb = protoQ.qhead;
    while (pdb) {
	if (pdb->running == FALSE) {
	    pdb = pdb->next;
	    continue;
	}
	mem_lock(pdb);
	headerflag = FALSE;
	printf("\nRouting Protocol is \"%s", pdb->name);
	if (pdb->asystem)
	    printf(" %d", pdb->asystem);
	if (pdb->proctype == PROC_ISIS && pdb->osi_pdb->pname)
	    printf(" %s", pdb->osi_pdb->pname);
	printc('"');
	if (!(pdb->proctype & PROC_NONPERIODIC)) {
	    printf("\n  Sending updates every %d seconds",
		   pdb->broadcasttime/ONESEC);
	    if (TIMER_RUNNING(pdb->timer))
		printf(", next due in %d seconds",
		    TIME_LEFT_SLEEPING(pdb->timer)/ONESEC);
	}
	if (!(pdb->proctype & PROC_NOHOLDDOWN)) {
	    printf("\n  Invalid after %d seconds, hold down %d, flushed"
		   " after %d", 
		   pdb->invalidtime/ONESEC,
		   (pdb->holdtime && pdb->holdokay) ?
		   pdb->holdtime/ONESEC : 0, 
		   pdb->flushtime/ONESEC);
	}
        if (pdb->output_delay) {
            printf("\n  Output delay %d milliseconds between packets",
                   pdb->output_delay);
        }
	if (!(pdb->proctype & (PROC_MOBILE|PROC_NHRP))) {
	    printf("\n  Outgoing update filter list for all interfaces is "); 
	    access_print_name(pdb->ofltr_all.acl);
	    for (i = 0; i < MAX_IPPROCS; i++) {
		if (!pdb->ofltr_proto[i].acl)
		    continue;
		proto = INDEX2PDB(i);
		if (!proto)
		    continue;
		printf("\n    Redistributed %s ", proto->name);
		if (proto->asystem)
		    printf("%d ", proto->asystem);
		printf("filtered by %s", pdb->ofltr_proto[i].acl->name);
	    }
	    FOR_ALL_SWIDBS(idb) {
		acl = idb->ip_oprotofltr[pdb->index];
		if (acl) {
		    printf("\n    %s filtered by %s", idb->namestring,
			   acl->name);
		}
	    }
	    printf("\n  Incoming update filter list for all interfaces is ");
	    access_print_name(pdb->ifltr_all.acl);
	    FOR_ALL_SWIDBS(idb) {
		acl = idb->ip_iprotofltr[pdb->index];
		if (acl) {
		    printf("\n    %s filtered by %s", idb->namestring,
			   acl->name);
		}
	    }
	}
	if (pdb->mmetric_out) {
	    printf("\n  Outgoing routes will have %d added to metric",
		   pdb->mmetric_out);
	    if (pdb->mmetric_list_out)
	        printf(" if on list %s", pdb->mmetric_list_out->name);
	}
	FOR_ALL_SWIDBS(idb) {
	    if (idb->ip_info) {
		offset = idb->ip_info->out_offlist[pdb->index];
		if (offset.metric) {
		    printf("\n  Outgoing routes in %s will have %d "
			   "added to metric", idb->namestring, offset.metric);
		    if (offset.acl)
			printf(" if on list %s", offset.acl->name);
		}
	    }
	}
	if (pdb->mmetric_in) {
	    printf("\n  Incoming routes will have %d added to metric",
		   pdb->mmetric_in);
	    if (pdb->mmetric_list_in)
	        printf(" if on list %s", pdb->mmetric_list_in->name);
	}
	FOR_ALL_SWIDBS(idb) {
	    if (idb->ip_info) {
		offset = idb->ip_info->in_offlist[pdb->index];
		if (offset.metric) {
		    printf("\n  Incoming routes in %s will have %d "
			   "added to metric", idb->namestring, offset.metric);
		    if (offset.acl)
			printf(" if on list %s", offset.acl->name);
		}
	    }
	}
	if (pdb->table_map)
	    printf("\n  Mapping external information into routing "
		   "table via '%s'", pdb->table_map->mapname);

	if (pdb->showipproto)
	    (*pdb->showipproto)(pdb);
	else
	    default_showipproto(pdb);

 	/*
 	 * Display summarization configuration.
 	 */
	if (pdb->show_summary) {
	    (*pdb->show_summary)(pdb);
	}
	if (!QUEUEEMPTY(&pdb->ip_address_sumq)) {
	    printf("\n  Address Summarization:");
	    while ((sum = queuepick(&pdb->ip_address_sumq, i++)))
		if ((sum->flags & (ADDRSUM_VALID|ADDRSUM_HIDDEN)) ==
		    ADDRSUM_VALID)
		    printf("\n    %i%m", sum->network, sum->mask);
	}
 
	if (!(pdb->proctype & (PROC_MOBILE|PROC_NHRP|PROC_ODR))) {
	    printf("\n  Routing for Networks:");
	    if (pdb->proctype != PROC_ISIS) {
		for (i = 0; i < pdb->netcount; i++) {
		    printf("\n    %i", pdb->networks[i]);
		    if (pdb->network_mask[i] !=
			get_majormask(pdb->networks[i]))
			printf("%m", pdb->network_mask[i]);
		    if (pdb->networkflags[i] & NETF_PASSIVE)
			printf(" passive");
		    if (pdb->networkflags[i] & NETF_BACKDOOR)
			printf(" backdoor");
		}
	    } else {
		reg_invoke_ip_isis_show_interfaces(pdb);
	    }
	    headerflag = TRUE;
	    FOR_ALL_SWIDBS(idb) {
		if (idb->ip_passive_router & pdb->mask) {
		    if (headerflag) {
			automore_header(passiveheader);
			headerflag = FALSE;
		    }
		    printf("\n    %s", idb->namestring);
		}
	    }
	}
	automore_header(NULL);

	printf("\n  Routing Information Sources:");
	for (i = 0, headerflag = TRUE; i < NETHASHLEN; i++) {
	    if (headerflag) {
		automore_header(peerheader);
		headerflag = FALSE;
	    }
	    peer = pdb->peercache[i];
	    while (peer) {
		mem_lock(peer);
		if (peer->source)
		    printf("\n    %15i", peer->source);
		else
		    printf("\n    (this router)  ");
		printf(" %-8d      ", peer->distance);
		print_dhms(peer->timer);
		last_peer = peer;
		peer = peer->next;
		free(last_peer);
	    }
	}
	automore_header(NULL);
	printf("\n  Distance: ");
	if (pdb->proctype & PROC_BGP) {
	    printf("external %d internal %d local %d", pdb->distance,
		   pdb->distance2, pdb->distance3); 
	} else if (pdb->proctype & PROC_IGRP2) {
	    printf("internal %d external %d", pdb->distance, pdb->distance2);
	} else {
	    printf("(default is %d)", pdb->distance);
	}

	for (i = 0, headerflag = TRUE; i < pdb->peercount; i++) {
	    if (headerflag) {
		automore_header(distheader);
		headerflag = FALSE;
	    }
	    printf("\n    %15i",  pdb->peers[i].address);
	    printf(" %-15i %-8d", pdb->peers[i].mask,
		   pdb->peers[i].distance); 
	    if (pdb->peers[i].acl)
		printf("  %s", pdb->peers[i].acl->name);
	}
	automore_header(NULL);
	printc('\n');
	last_pdb = pdb;
	pdb = pdb->next;
	free(last_pdb);
    }
    automore_disable();
}
/*
 * showipproto_redistribute
 * show the redistribute information
 */
void showipproto_redistribute (pdbtype *pdb)
{
    pdbtype *proto;
    int headerflag, i;

    headerflag = FALSE;
    for (proto = (pdbtype *) protoQ.qhead; proto; proto = proto->next) {
	if (proto->mask & pdb->distflag) {
	    if (!headerflag)
		printf("\n  Redistributing: ");
	    printf("%s%s", (headerflag ? ", " : NULL), proto->name);
	    if (proto->asystem || (proto->proctype & PROC_EGP))
		printf(" %d", proto->asystem);
	    headerflag = TRUE;
	    if ((pdb != proto)
		&& (proto->proctype & PROC_OSPF)
		&& (i = pdb->criteria[proto->index].routetype)) {
		printf(" (");
		if (i & (RTYPE_INTER|RTYPE_INTRA)) {
		    printf("internal");
		    if (i & ~(RTYPE_INTER|RTYPE_INTRA))
			printf(", ");
		}	
		if (i & RTYPE_EXTERN1) {
		    printf("external 1");
		    if (i & RTYPE_EXTERN2)
			printf(", ");
		}
		if (i & RTYPE_EXTERN2)
		    printf("external 2");
		printf(")\n                  ");
		headerflag = FALSE;
	    }
	}
    }
}

/* 
 * default_showipproto
 * Show protocol information for protocols other than BGP, IGRP/IGRP2, EGP
 */
void default_showipproto (pdbtype *pdb) 
{
    neighbortype *neigh;
    int headerflag;

    if (pdb->defaultmetricflag)
	printf("\n  Default redistribution metric is %d", pdb->default_metric);
    showipproto_redistribute(pdb);
    for (neigh = pdb->neighbors, headerflag = TRUE;
	 neigh; neigh = neigh->next) {
	if (headerflag) {
	    automore_header(showipproto_neighborheader);
	    headerflag = FALSE;
		}
	printf("\n    %i",neigh->address);
    }
    automore_header(NULL);
}


/*
 * clear_routecommand_func
 * Clear ip route action routine.
 */

void clear_routecommand_func (ipaddrtype address, ipaddrtype mask)
{
    pdbtype *pdb;
    ndbtype *ndb;
    ushort route_type;
    backuptype local_backup;
    
    if (!address) {
	table_init();
    } else {
	ndb = ip_rtlookup(address, mask);
	if (!ndb) {
	    return;
	}
	pdb = INDEX2PDB(ndb->pdbindex);
	route_type = ndb->route_type;

	if (!mask) {
	    mask = ndb->netsmask;
            address &= mask;
        }
	if (pdb && pdb->add_route) {
	    memset(&local_backup, 0, sizeof(local_backup));
	    if (pdb->build_backupinfo) {
		(*pdb->build_backupinfo)(pdb, ndb, &local_backup, route_type,
					 TRUE);
	    }
	    local_backup.route_type = route_type;
	}
	ip_route_delete(pdb, address, ndb->netsmask, IPADDR_ZERO,
			IPADDR_ZERO, NULL, 0);
	if (pdb && pdb->add_route) {
	    (*pdb->add_route)(pdb, address, mask, &local_backup);
	}
    }
}

/*
 * clear_routecommand
 * Support EXEC commands "clear ip route <address>", "clear ip route
 * <address> <mask>" or "clear route *".
 */
void clear_routecommand (parseinfo *csb)
{
    if (GETOBJ(int,1)) {
	clear_routecommand_func((ipaddrtype)GETOBJ(paddr,1)->ip_addr,
				(ipaddrtype)GETOBJ(paddr,2)->ip_addr);
    } else {
	clear_routecommand_func(0, 0);
    }
}

/*
 * is_id_used
 *
 * check if an ip address is used as OSPF router id.
 */
boolean is_id_used (ulong address)
{
    pdbtype *pdb;

    for (pdb = protoQ.qhead; pdb; pdb = pdb->next) {
	if ((pdb->proctype & PROC_OSPF) && (pdb->ospf_rtr_id == address)) {
		return(TRUE);
	}
    }

    return(FALSE);
}

/*
 * allocate_rtr_id
 *
 * Allocated a new router ID.
 * Returns 0.0.0.0 on error.
 */
ipaddrtype allocate_rtr_id (boolean need_unique)
{
    ipaddrtype rtr_id = 0;
    boolean loopback_found = FALSE;
    idbtype *idb;

    /*
     * If possible Loopback interface address
     * is prefered as the Router ID. 
     */
    FOR_ALL_SWIDBS(idb) {
	if ((!idb->ip_enabled) 
	    || (!idb->ip_address)
	    || interface_admin_down(idb))
	    continue;
	if (need_unique && is_id_used(idb->ip_address))
	    continue;
	if (idb->hwptr->type == IDBTYPE_LB) {
	    if (!loopback_found)
		rtr_id = idb->ip_address;
	    loopback_found = TRUE;
	} else if (loopback_found)
	    continue;
	if (!rtr_id)
	    rtr_id = idb->ip_address;
	else if (rtr_id < idb->ip_address)
	    rtr_id = idb->ip_address;
    }
    return(rtr_id);
}

/*
 * allocate_pdbindex
 * Allocate a pdb index dynamically.  Sets it and returns it.  Returns -1
 * on error.
 */
int allocate_pdbindex (pdbtype *pdb)
{
    int index;

    for (index = 0; index < MAX_IPPROCS; index++)
	if (!ip_proctable[index]) {
	    ip_proctable[index] = pdb;
	    pdb->index = index;
	    pdb->mask = INDEX2MASK(index);
	    return(index);
	}
    printf("too many IP routing processes");
    return(-1);
}

/*
 * free_pdbindex
 * Free a pdb index dynamically.
 */
void free_pdbindex (pdbtype *pdb)
{
    ip_proctable[pdb->index] = NULL;
    pdb->mask = 0;
    pdb->index = -1;
}

/*
 * print_redistribute_command
 *
 * Display "redistribute ..." command for given pdb.
 */
void print_redistribute_command (pdbtype *pdb)
{
    pdbtype *proto;
    char    *str;
    int     i;

    for (proto = (pdbtype *) protoQ.qhead; proto; proto = proto->next) {
	if ((proto == pdb) ||
	    (!(pdb->redistallowed & proto->proctype))) {
	    continue;
	}

	/*
	 * IGRP1 and IGRP2 for the same AS auto-redistribute by default.
	 * Don't NVgen commands.
	 */
	if (pdb->asystem == proto->asystem &&
	    (pdb->proctype & (PROC_IGRP | PROC_IGRP2)) &&
	    (proto->proctype & (PROC_IGRP | PROC_IGRP2))) {
	    if (pdb->distflag & proto->mask) continue;
	    nv_write(TRUE,"no redistribute %s", proto->name);
	} else {
	    if (!(pdb->distflag & proto->mask)) continue;
	    nv_write(TRUE,"redistribute %s", proto->name);
	}

	if (proto->proctype == PROC_ISIS && proto->osi_pdb) {
	    nv_add((boolean) proto->osi_pdb->pname, 
		   " %s", proto->osi_pdb->pname);
	} else {
	    nv_add(proto->asystem || (proto->proctype & PROC_EGP),
		   " %d", proto->asystem);
	}

	nv_add((pdb->proctype == PROC_ISIS) &&
	       (proto->proctype == PROC_STATIC),
	       " ip");

	/*
	 * Redistributing IS-IS into this pdb, print level.
	 */
	reg_invoke_ip_clns_redistribute_level_nvgen(proto, pdb);


	/* 
	 * Redistributing into IGRP.
	 */
	if (pdb->proctype & (PROC_IGRP | PROC_IGRP2)) {
	    if (pdb->metric[proto->index].flag == TRUE)
		nv_add(TRUE, " metric %d %d %d %d %d",
		       pdb->metric[proto->index].metric.vector.user_bandwidth, 
		       pdb->metric[proto->index].metric.vector.delay,
		       pdb->metric[proto->index].metric.vector.reliability,
		       pdb->metric[proto->index].metric.vector.loading,
		       pdb->metric[proto->index].metric.vector.mtu);
	} else {

	    /*
	     * Generalize for any protocol except IGRP. This prints
	     * the metric information.
	     */
	    if (pdb->metric[proto->index].flag == TRUE)
		if ((pdb->proctype & PROC_ISIS) ||
		    pdb->metric[proto->index].flag)
		    nv_add(TRUE, " metric %d",
			   (pdb->proctype & PROC_ISIS) ?
			   pdb->metric[proto->index].metric.scalar & 0x7f :
			   pdb->metric[proto->index].metric.scalar);
	}

	if (pdb->proctype == PROC_BGP)
	    nv_add(pdb->metric[proto->index].weight, " weight %d",
		pdb->metric[proto->index].weight);

	/*
	 * Redistributing INTO OSPF 
	 */
	if (pdb->proctype & PROC_OSPF) {
	    if ((pdb->metric[proto->index].type != 
		 pdb->metric_def[proto->index].type)
		&& (pdb->metric_def[proto->index].type))
		nv_add(TRUE, " metric-type 1");
	    if (pdb->metric[proto->index].subnets !=
		pdb->metric_def[proto->index].subnets)
		nv_add(TRUE, " subnets");
	    if (pdb->metric[proto->index].tag_set)
		nv_add(TRUE, " tag %d", pdb->metric[proto->index].tag);
	}
	/*
	 * Redistributing FROM OSPF into this pdb.
	 */
	if (proto->proctype & PROC_OSPF) {
	    i = pdb->criteria[proto->index].routetype;
	    if (i != pdb->criteria_def[proto->index].routetype) {
		nv_add(i, " match");
		nv_add(i & (RTYPE_INTER|RTYPE_INTRA),
		       " internal"); 
		nv_add(i & RTYPE_EXTERN1, " external 1");
		nv_add(i & RTYPE_EXTERN2, " external 2");
	    }
	}

	/*
	 * Print route-map keyword and value. 
	 */
	str = pdb->routemap[proto->index];
	if (str) {
	    nv_add(TRUE, " route-map %s", str);
	}

	/*
	 * Redistributing into IS-IS.
	 */
	reg_invoke_ip_clns_redistribute_metric_nvgen(proto, pdb);
    }
}

/*
 * traffic_share_command
 * Set the traffic share algorithm for this pdb.
 */
void traffic_share_command (parseinfo *csb)
{
    pdbtype *pdb;
    uint algorithm;

    pdb = csb->protocol;

    if (csb->nvgen) {
	nv_write(pdb->traffic_share == TRAFFIC_MIN, "%s min", csb->nv_command);
	return;
    }

    algorithm = GETOBJ(int,1);

    if (csb->sense)
	pdb->traffic_share = algorithm;
    else
	pdb->traffic_share = TRAFFIC_BALANCED;
}

/*
 * auto_summary_command
 *
 * Set flag to indicate to aggregate on network number boundary. This command
 * is supported only for IP-IGRP2 and BGP.
 */
void auto_summary_command (parseinfo *csb)
{

    pdbtype *pdb;

    pdb = csb->protocol;

    if (pdb->family != PDB_IP)
	return;

    if (csb->nvgen) {
	nv_write(!pdb->auto_summary, "no %s", csb->nv_command);
	return;
    }
    if (csb->set_to_default)
	csb->sense = TRUE;
    if (pdb->auto_summary == csb->sense) {
	/*
	 * No work needed.  Avoid trying to delete summaries which
	 * are already gone.
	 */
	return;
    }
    pdb->auto_summary = csb->sense;

    /*
     * For each configured network, add/delete the auto-summarization entries.
     */
    if (pdb->auto_sum_cmd) {
	(*pdb->auto_sum_cmd)(pdb);
    }
}

/*
 * iprouting_validate_command
 * Let user decide whether or not system should do a samecable()
 * check on the source IP address of incoming routing updates.
 */

void iprouting_validate_command (parseinfo *csb)
{
    pdbtype *pdb;

    pdb = csb->protocol;

    if (pdb->family != PDB_IP)
	return;

    if (csb->nvgen) {
	nv_write(!pdb->validate_source, "no %s", csb->nv_command);
	return;
    }
    pdb->validate_source = csb->set_to_default || csb->sense;
}

/*
 * iprouting_command
 *
 * Generic manipulation of flags of interest to IP routing.
 *
 */
void iprouting_command (parseinfo *csb)
{
    if (csb->nvgen) {
	switch (csb->which) {
	case IPR_SLOW_CONVERGE:
	    nv_write(iprouting_slow_converge, csb->nv_command);
	    break;
	case IPR_CLASSLESS:
	    nv_write(iprouting_classless,  "%s", csb->nv_command);
	    nv_write(!iprouting_classless, "no %s", csb->nv_command);
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case IPR_SLOW_CONVERGE:
	iprouting_slow_converge = csb->sense;
	break;
    case IPR_CLASSLESS:
	iprouting_classless = csb->sense;
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

    return;
}

/*
 * pdb_killp
 * Kill the process number passed in (unually us) and turn off running flag.
 */
void pdb_killp (pdbtype *pdb, int pid)
{
    pdb->running = FALSE;
    process_kill(pid);
}
