/* $Id: route2.c,v 3.7.12.4 1996/06/28 23:19:41 hampton Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/route2.c,v $
 *------------------------------------------------------------------
 * route2.c -- Routing Support: User Interface Routines
 *
 * 22-March-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: route2.c,v $
 * Revision 3.7.12.4  1996/06/28  23:19:41  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.12.3  1996/04/16  19:02:15  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.7.12.2  1996/04/15  21:18:33  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.7.12.1  1996/03/18  20:29:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/07  09:50:26  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  01:04:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/13  08:14:22  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.6  1996/02/01  06:06:08  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/22  06:14:58  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/10  00:42:44  pst
 * CSCdi38750:  Allow static routes with both next hop and interface
 *              also add "permanent" keyword for non-flapping static routes
 *
 * Revision 3.3  1995/12/02  10:11:26  pst
 * CSCdi44894:  BGP: be cool and subtle, use the hopcount metric for AS
 * path hops
 *
 * Revision 3.2  1995/11/17  17:37:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:06:54  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/02  20:45:39  pst
 * CSCdi41383:  garbage text in show ip route
 *
 * Revision 2.5  1995/09/23  21:41:03  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Reference renamed fields.  Clean up various interfaces to the IP
 * forwarding table to support protocol-specific functions.
 *
 * Revision 2.4  1995/09/11  23:50:59  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.3  1995/06/28 03:21:59  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.2  1995/06/09 13:06:44  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:10:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "route.h"
#include "route_inlines.h"
#include "iprouting_debug.h"
#include "../if/network.h"
#include "logger.h"
#include "ospf_pak.h"
#include "../clns/clns_route.h"
#include "parser_defs_router.h"
#include "../os/free.h"
#include "../ip/ip_registry.h"
#include "eigrp_registry.h"
#include "igrp.h"

/*
 * Forward declarations
 */
static ulong get_pdbmask(struct rtrname *);
static void show_network(parseinfo *);


/*
 * ip_defaultnetwork_command
 * Set up a default network for routing
 *	"[no] ip default-network <network>"
 */

void ip_defaultnetwork_command (parseinfo *csb)
{
    ipaddrtype network;
    ndbtype *ndb;
    int i, j;
    
    if (csb->nvgen) {
	for (i = 0; i < default_netcount; i++)
	    nv_write(TRUE, "%s %i", csb->nv_command, default_network[i]);
	return;
    }
    network = GETOBJ(paddr,1)->ip_addr;
    if (csb->sense == TRUE) {
	/*
	 * First check to see if it really is a default subnet, and if so,
	 * translate this into a default subnet route.  Ignore the case of
	 * a default subnet that is subnet zero.  You'll just have to enter
	 * that by hand.
	 */
	ndb = net_lookup(network, FALSE);
	if (ndb && (network != ndb->netnumber)) {
	    ip_addstatic(get_majornet(network), get_majormask(network),
			 network, NULL, 0, 0, STATIC_DISTANCE, NULL);
	    return;
	}
	for (i = 0; i < default_netcount; i++) {
	    if (default_network[i] == network)
		return;
	}
	if (default_netcount < MAXDEFNETWORKS)
	    default_network[default_netcount++] = network;
	else
	    printf("Too many default networks specified");
	return;
    } else {
	for (i = 0; i < default_netcount; i++) {
	    if (default_network[i] == network) {
		default_netcount--;
		for (j = i; j < default_netcount; j++)
		    default_network[j] = default_network[j+1];
		break;
	    }
	}
    }
}

/*
 * route_preamble
 * Print the routing table preamble.
 */

int route_preamble (void)
{
    printf("\nCodes: C - connected, S - static, I - IGRP, R - RIP, M -"
	   " mobile, B - BGP"); 
    printf("\n       D - EIGRP, EX - EIGRP external, O - OSPF, IA - OSPF inter"
	   " area "); 
    printf("\n       N1 - OSPF NSSA external type 1, N2 - OSPF NSSA external "
	   "type 2");
    printf("\n       E1 - OSPF external type 1, E2 - OSPF external type 2, E -"
	   " EGP"); 
    printf("\n       i - IS-IS, L1 - IS-IS level-1, L2 - IS-IS level-2, * -"
	   " candidate default"); 
    printf("\n       U - per-user static route, o - ODR");
    printf("\n\nGateway of last resort is ");
    
    if (default_ndb) {
	mem_lock(default_ndb);
	printf("%i to network %i\n", default_ndb->rdb[0]->gateway,
	       default_ndb->netnumber);
	ndb_free(default_ndb, FALSE);
    } else
	printf("not set\n");
    return(8);
}

/*
 * ip_ndb_pdbmask_match
 * Return an indication of whether or not the ndb is owned by one of the
 * pdbs denoted by the passed in pdbmask.
 */
static inline boolean ip_ndb_pdbmask_match (ndbtype *ndb, ulong pdbmask)
{
    return(INDEX2MASK(ndb->pdbindex) & pdbmask);
}

/*
 * display_routes
 * Dump a summary of the IP routing table to the screen.
 */

extern void show_paths(parseinfo *);

void display_routes (parseinfo *csb)
{
    int i, j;
    pdbtype *pdb;
    ndbtype *ndb;
    ulong pdbmask;
    int len;
    ipaddrtype address = 0, mask = 0;
    boolean prefix = (csb->which == SHOW_IPR_PREFIX);
    struct rtrname *prname = &(csb->router_name);

    if (!ip_router_running) {
	show_paths(csb);
	return;
    }

    automore_enable(NULL);

    if (GETOBJ(paddr,1)->type == ADDR_IP) { 
        if (!prefix) {
	   show_network(csb);
	   goto punt;
	} else {
	   address = GETOBJ(paddr,1)->ip_addr;
	   mask = (GETOBJ(paddr,2)->type == ADDR_IP) ? 
	     GETOBJ(paddr,2)->ip_addr : get_majormask(address);
	}
    }

    if (prname->myname) {
	if ((pdbmask = get_pdbmask(prname)) == 0) {
	    /* The argument was specified but no match was found */
	    goto punt;
	}
    } else {
	pdbmask = ~0;
	route_preamble();
    }

    if (csb->which == SHOW_IPR_CIDR)
	i = NETHASHLEN;
    else
	i = 0;

    for (; i < NETHASHLEN<<1; i++) {
	for (j = 0; (ndb = queuepick(i < NETHASHLEN ? &nettable[i]
		      : &supernettable[i-NETHASHLEN], j)); j++) {

	    if (automore_quit())
		goto punt;

	    if (!print_this_ndb(ndb, pdbmask, address, mask, prefix))
		continue;

	    mem_lock(ndb);

	    len = printf("\n%c%c%s %i%m ", nettype(ndb),
			 ((ndb->attrflags & NET_EXTERIOR) ? '*' : ' '),
			 route_type(ndb), ndb->netnumber, ndb->netsmask);

	    /*
	     * Handle non-subnetted networks first.
	     */
	    if ((ndb->attrflags & NET_SUBNETTED) == 0) {
		if (ndb->metric == METRIC_INACCESSIBLE) {
		    printf("is possibly down, routing via %i",
			   ndb->lastgateway);
		    if (ndb->lastinterface && validlink(ndb->lastinterface))
			printf(", %s", ndb->lastinterface->namestring);
		    ndb_free(ndb, FALSE);
		    continue;
		}
		len--;
		if (!route_out(ndb, len)) {
		    ndb_free(ndb, FALSE);
		    goto punt;
		}

		/* Print protocol-specific info. */

		pdb = INDEX2PDB(ndb->pdbindex);
		if (pdb && pdb->ndb_print)
		    (*pdb->ndb_print)(pdb, ndb);
	    }
	    
	    /*
	     * Handle subnetted networks now
	     */
	    if (ndb->attrflags & NET_SUBNETTED) {
		if (ndb->attrflags & NET_VARIABLE) 
		    printf("is variably subnetted, %d subnets, %d masks",
			   ndb->subnets, ndb->maskcount);
		else 
		    printf("is subnetted, %d subnets", ndb->subnets);
		if (print_subnets(ndb, pdbmask, address, mask, prefix) == 
		    FALSE) {
		    ndb_free(ndb, FALSE);
		    goto punt;
		}
	    }
	    ndb_free(ndb, FALSE);
	}
	process_may_suspend();
    }
    
 punt:
    automore_disable();
    return;
}

/*
 * print_subnets
 * Print out the subnets of a network.
 * Returns TRUE to continue display, FALSE if user quitted
 */

boolean print_subnets (ndbtype *ndb, ulong pdbmask, ipaddrtype address, 
		       ipaddrtype mask, boolean prefix)
{
    pdbtype *pdb;
    sdbtype *sdb;
    int i, j, len;
    
    for (i = 0; i < NETHASHLEN; i++)
	for (j = 0; (sdb = queuepick(&ndb->subnettable[i], j++)); ) {

	    if (automore_quit())
		return(FALSE);

	    if (prefix & ((address & mask) != (sdb->netnumber & mask)))
	        continue;
	    if (!ip_ndb_pdbmask_match(sdb, pdbmask))
		continue;

	    mem_lock(sdb);

	    len = printf("\n%c%c%s    %i", nettype(sdb),
			 ((sdb->attrflags & NET_EXTERIOR) ? '*' : ' '),
			 route_type(sdb), sdb->netnumber);
	    if (sdb->metric == METRIC_INACCESSIBLE) {
		printf("%m is possibly down,", sdb->netsmask);
		printf("\n          routing via %i", sdb->lastgateway);
		if (sdb->lastinterface && validlink(sdb->lastinterface))
		    printf(", %s", sdb->lastinterface->namestring);
		ndb_free(sdb, FALSE);
		continue;
	    } else if (ndb->attrflags & NET_VARIABLE)
		len += printf("%m ", sdb->netsmask);
	    else
		len += printf(" ");
	    len--;
	    if (!route_out(sdb, len))
		goto punt;

	    /* Now print protocol-specific info. */

	    pdb = INDEX2PDB(sdb->pdbindex);
	    if (pdb && pdb->ndb_print)
		(*pdb->ndb_print)(pdb, sdb);

	    ndb_free(sdb, FALSE);
	}

    return(TRUE);
    
 punt:
    ndb_free(sdb, FALSE);
    return(FALSE);
}

/* 
 * add_ip_route_stats
 * Add statistics on supplied ndb to table of statistics for
 * all ndb's.
 */

static void add_ip_route_stats (ndbtype *ndb, boolean is_net_route,
			 ndbstattype *ndb_stat)
{
    ndbstattype *stat;
    int procnum, i;
    backuptype *backup;
    
    procnum = ndb->pdbindex;
    stat = &ndb_stat[procnum];
    if (is_net_route)
	stat->nets++;
    else
	stat->subnets++;
    stat->mem += get_block_size(ndb, NULL);
    
    /* Compute overhead */
    for (i = 0; i < ndb->routecount; i++)
	stat->overhead += sizeof(rdbtype);
    if (ndb->maskcount)
	stat->overhead += get_block_size(ndb->masktable, NULL);
    if (ndb->backup_info)
	for (backup = ndb->backup_info; backup; backup = backup->next)
	    stat->overhead += get_block_size(backup, NULL);

    switch (ndb->route_type) {
    case RTYPE_INTRA:
    case RTYPE_L1:
    case RTYPE_BGP_LOC:
	stat->rtype[0]++;
	break;
    case RTYPE_INTER:
    case RTYPE_L2:
    case RTYPE_BGP_INT:
	stat->rtype[1]++;
	break;
    case RTYPE_EXTERN1:
    case RTYPE_BGP_EXT:
	stat->rtype[2]++;
	break;
    case RTYPE_EXTERN2:
	stat->rtype[3]++;
	break;
    }
}

/*
 * ip_stats_subnet
 * Loop through subnets of an ndb to collect statistics on all subnet routes.
 */

static inline void ip_stats_subnet (ndbtype *ndb, ndbstattype *ndb_stat)
{
    sdbtype *sdb;
    int i;
    
    for (i = 0; i < NETHASHLEN; i++)
	for (sdb = ndb->subnettable[i]; sdb; sdb = sdb->next)
	    add_ip_route_stats(sdb, FALSE, ndb_stat);
}

/*
 * display_route_summary
 * Summarize to screen resource usage statistics on IP routing table.
 */
void display_route_summary (parseinfo *csb)
{
    int i, len;
    ndbtype *ndb;
    ndbstattype ndb_stat[MAX_IPPROCS], tot_stats;
    ulong nets_subnetted;
    pdbtype *pdb;
    
    /* Initialize stats to zero */
    memset(ndb_stat, 0, MAX_IPPROCS * sizeof(ndbstattype));
    memset(&tot_stats, 0, sizeof(tot_stats));
    nets_subnetted = 0;
    
    for (i = 0; i < NETHASHLEN<<1; i++) {
	for (ndb = (i < NETHASHLEN ? nettable[i] :
		    supernettable[i-NETHASHLEN]);
	     ndb; ndb = ndb->next) {
	    if (ndb->attrflags & NET_SUBNETTED) {
		ip_stats_subnet(ndb, ndb_stat);
		nets_subnetted++;
	    } else
		add_ip_route_stats(ndb, TRUE, ndb_stat);
	}
	process_may_suspend();
    }

    /*
     * Now print results
     */
    printf("\nRoute Source    Networks    Subnets     "
	   "Overhead    Memory (bytes)");
    for (i = 0; i < MAX_IPPROCS; i++) {
	pdb = INDEX2PDB(i);
	if (pdb) {
	    if (!pdb->running && 
		!(pdb->proctype & (PROC_STATIC|PROC_CONNECTED))) 
		continue;
	    
	    if (pdb->asystem ||	(pdb->proctype & PROC_EGP)) {
		len = min(10, strlen(pdb->name));
		printf("\n%*s %*d", len, pdb->name, 15 - len,
		       pdb->asystem);
	    } else
		printf("\n%16s", pdb->name);
	    printf("%12d%12d%12d%d", ndb_stat[i].nets,
		   ndb_stat[i].subnets, ndb_stat[i].overhead,
		   ndb_stat[i].mem);
	    tot_stats.nets += ndb_stat[i].nets;
	    tot_stats.subnets += ndb_stat[i].subnets;
	    tot_stats.overhead += ndb_stat[i].overhead;
	    tot_stats.mem += ndb_stat[i].mem;
	    if (pdb->proctype & PROC_OSPF)
		printf("\n  Intra-area: %d Inter-area: %d "
		       "External-1: %d External-2: %d",
		       ndb_stat[i].rtype[0], ndb_stat[i].rtype[1],
		       ndb_stat[i].rtype[2], ndb_stat[i].rtype[3]);
	    else if (pdb->proctype & PROC_ISIS)
		printf("\n  Level 1: %d Level 2: %d",
		       ndb_stat[i].rtype[0], ndb_stat[i].rtype[1]);
	    else if (pdb->proctype & PROC_BGP)
		printf("\n  External: %d Internal: %d Local: %d",
		       ndb_stat[i].rtype[2], ndb_stat[i].rtype[1],
		       ndb_stat[i].rtype[0]);
	}
    }
    if (nets_subnetted) {
	printf("\ninternal        %16d                    %d",
	       nets_subnetted, nets_subnetted * sizeof(ndbtype));
	tot_stats.nets += nets_subnetted;
	tot_stats.mem += nets_subnetted * sizeof(ndbtype);
    }
    printf("\nTotal           %12d%12d%12d%d", tot_stats.nets,
	   tot_stats.subnets, tot_stats.overhead, tot_stats.mem);
}

/*
 * print_this_ndb
 * Return true if any subnets of this ndb match the given address/mask pair
 * and pdbmask.
 * Or, if not subnetted, return true if the ndb matches the given address/mask
 * pair and pdbmask.  Otherwise return false.
 * The address/mask pair test is ignore only if prefix is false.
 */

boolean print_this_ndb (ndbtype *ndb, ulong pdbmask, ipaddrtype address,
			ipaddrtype mask, boolean prefix)
{
    sdbtype *sdb;
    int i;
    boolean match = TRUE;

    if (prefix)
        match = ((address & mask) == (ndb->netnumber & mask));

    if (pdbmask == ~0 && match)
	return(TRUE);

    if (ndb->attrflags & NET_SUBNETTED) {
	/*
	 * If this network is subnetted, we want to print its route iff we
	 * want to print one of the subnet routes
	 */
	for (i = 0; i < NETHASHLEN; i++)
	    for (sdb = ndb->subnettable[i]; sdb; sdb = sdb->next) {
	        match = prefix ? 
		  ((address & mask) == (sdb->netnumber & mask)) : TRUE;
		if (match && ip_ndb_pdbmask_match(sdb, pdbmask))
		    return(TRUE);
	    }
	return(FALSE);
    } else
	return(match && ip_ndb_pdbmask_match(ndb, pdbmask));
}

/*
 * nettype
 * Return a type code for a network
 */

char nettype (ndbtype *ndb)
{
    if (ndb->attrflags & NET_SUBNETTED)
	return(' ');
    switch (NDB2PROCTYPE(ndb)) {
    case PROC_CONNECTED: return('C');
    case PROC_STATIC: 
	if (ndb->attrflags & NET_PER_USER) {
	    return('U');
	}
	return('S');
    case PROC_EGP: return('E');
    case PROC_BGP: return('B');
    case PROC_RIP: return('R');
    case PROC_IGRP: return('I');
    case PROC_IGRP2: return('D');
    case PROC_OSPF:  return('O');
    case PROC_ISIS:  return('i');
    case PROC_MOBILE: return('M');
    case PROC_ODR: return('o');
    default: return('?');
    }
}

/*
 * route_type
 * returns route type string.
 */
char *route_type (ndbtype *ndb)
{
    if (ndb->attrflags & NET_SUBNETTED)
	return("  ");
    if (NDB2PROCTYPE(ndb) & PROC_OSPF) {
	switch (ndb->route_type) {
	case RTYPE_INTRA: return("  ");
	case RTYPE_INTER: return("IA");
	case RTYPE_EXTERN1: return("E1");
	case RTYPE_EXTERN2: return("E2");
	case RTYPE_NSSA1: return("N1");
	case RTYPE_NSSA2: return("N2");
	}
    } else if (NDB2PROCTYPE(ndb) & PROC_ISIS) {
	if (ndb->route_type == RTYPE_L1) return("L1");
	if (ndb->route_type == RTYPE_L2) return("L2");
    } else if (NDB2PROCTYPE(ndb) & PROC_IGRP2) {
	if (ndb->route_type == RTYPE_IGRP2_EXT) return("EX");
    }
    
    return("  ");
}

/*
 * route_out
 * Print the routes for a network or subnet
 * Returns TRUE if we should keep on displaying, FALSE otherwise.
 */

boolean route_out (ndbtype *ndb, int len)
{
    int k;
    pdbtype *pdb;
    rdbtype *rdb;
    char buf[82], *bp;
    
    for (k = 0; k < ndb->routecount; k++) {
	bp = buf;
	rdb = ndb->rdb[k];
	rdb_lock(rdb);
	if (ndb->attrflags & NET_ATTACHED) 
	    bp += sprintf(bp, "is directly connected");
	else if (ndb->attrflags & NET_SUMMARY)
	    bp += sprintf(bp, "is a summary");
	else
	    bp += sprintf(bp, "[%d/%u] via %i", ndb->distance,
			  rdb->metric, rdb->gateway);  
 	if ((!(rdb->rdb_flags & IP_RDB_ISIS)) &&
 	    ((NDB2PROCTYPE(ndb) &
 	      (PROC_STATIC|PROC_CONNECTED)) == 0)) { 
  	    bp += sprintf(bp, ", ");
 	    bp += sprint_ticks_in_dhms(bp, ELAPSED_TIME(rdb->rdb_routetimer));
	}
	if (rdb->idb && validlink(rdb->idb))
	    bp += sprintf(bp, ", %s", rdb->idb->namestring);

 	/* Do protocol-specific printing. */

 	pdb = INDEX2PDB(rdb->pdbindex);
 	if (pdb && pdb->rdb_print)
 	    bp += (*pdb->rdb_print)(pdb, rdb, bp);

	if (k) {
 	    printf("\n%*c%s", len, ' ', buf);
	} else if ((bp - buf + len) > 78) {
	    printf("\n           %s", buf);
	    len = 11;
	} else
	    printf("%s", buf);
	rdb_free(rdb, FALSE);
    }
    return(TRUE);
}

/* show_network_redistribute
 * Display redistribute parameter.
 * Similar to print_redistribute_command(), except this route use 
 * printf() rather than nv_write().
 * Called by show_network().
 */
static void show_network_redistribute (pdbtype *pdb_src, pdbtype *pdb_dst) {
    char *str;
    int   i;
    
    if (pdb_src == pdb_dst) {
        printf(" (self originated)");
        return;
    }
    reg_invoke_ip_clns_redistribute_level_printf(pdb_src, pdb_dst);

    /* 
     * Redistributing into IGRP.
     */
    if (pdb_dst->proctype & (PROC_IGRP | PROC_IGRP2)) {
	if (pdb_dst->metric[pdb_src->index].flag == TRUE)
	    printf(" metric %d %d %d %d %d",
	        pdb_dst->metric[pdb_src->index].metric.vector.user_bandwidth, 
	        pdb_dst->metric[pdb_src->index].metric.vector.delay,
	        pdb_dst->metric[pdb_src->index].metric.vector.reliability,
		pdb_dst->metric[pdb_src->index].metric.vector.loading,
	        pdb_dst->metric[pdb_src->index].metric.vector.mtu);
    } else {
	/*
	 * Generalize for any protocol except IGRP. This prints
	 * the metric information.
	 */
	if (pdb_dst->metric[pdb_src->index].flag == TRUE)
	    if ((pdb_dst->proctype & PROC_ISIS) ||
		pdb_dst->metric[pdb_src->index].flag)
		printf(" metric %d",
		       (pdb_dst->proctype & PROC_ISIS) ?
		       pdb_dst->metric[pdb_src->index].metric.scalar & 0x7f :
		       pdb_dst->metric[pdb_src->index].metric.scalar);
    }

    if (pdb_dst->proctype == PROC_BGP && 
	pdb_dst->metric[pdb_src->index].weight)
	printf(" weight %d", pdb_dst->metric[pdb_src->index].weight);

    /*
     * Redistributing INTO OSPF 
     */
    if (pdb_dst->proctype & PROC_OSPF) {
	if ((pdb_dst->metric[pdb_src->index].type != 
	     pdb_dst->metric_def[pdb_src->index].type)
	    && (pdb_dst->metric_def[pdb_src->index].type))
		printf(" metric-type 1");
	if (pdb_dst->metric[pdb_src->index].subnets !=
	    pdb_dst->metric_def[pdb_src->index].subnets)
	    printf(" subnets");
	if (pdb_dst->metric[pdb_src->index].tag_set)
	    printf(" tag %d", pdb_dst->metric[pdb_src->index].tag);
    }
    /*
     * Redistributing FROM OSPF
     */
    if (pdb_src->proctype & PROC_OSPF) {
	i = pdb_dst->criteria[pdb_src->index].routetype;
	if (i != pdb_dst->criteria_def[pdb_src->index].routetype) {
	    if (i) printf(" match");
	    if (i & (RTYPE_INTER|RTYPE_INTRA)) printf(" internal"); 
	    if (i & RTYPE_EXTERN1) printf(" external 1");
	    if (i & RTYPE_EXTERN2) printf(" external 2");
        }
    }

    /*
     * Redistributing into IS-IS.
     */

    reg_invoke_ip_clns_redistribute_metric_printf(pdb_src, pdb_dst);

    /*
     * Print route-map keyword and value. 
     */
    str = pdb_dst->routemap[pdb_src->index];
    if (str)
	printf(" route-map %s", str);

}

/*
 * show_network
 * Display details of a net/subnet routing table entry
 */


static void show_network (parseinfo *csb)
{
    ndbtype *ndb;
    rdbtype *rdb;
    ipaddrtype address, mask;
    boolean subnetflag, headerflag;
    pdbtype *pdb;
    int i;

    subnetflag = FALSE;
    address = GETOBJ(paddr,1)->ip_addr;
    if (GETOBJ(paddr,2)->type == ADDR_IP)
	mask = GETOBJ(paddr,2)->ip_addr;
    else 
	mask = 0;

    ndb = netsuper_lookup(address, mask, FALSE);
    if (!ndb) {
	printf("%% Network not in table");
	return;
    }

    /*
     * If the ndb is subnetted, try getting the correct subnet. If the subnet
     * is not found, there could be a supernet covering the majornet. Try
     * getting that supernet. Make sure that the supernet was learned
     * via a mask carrying protocol and check if class routing is enabled.
     */
    if ((ndb->attrflags & NET_SUBNETTED) &&
	((address != ndb->netnumber) || mask)) {
	ndb = subnet_lookup(ndb, address, mask);
	if (ndb) {
	    subnetflag = TRUE;
	} else {
	    ndb = supernet_lookup(address, 0, FALSE);
	    if (!ndb || !(ndb && ((NDB2PROCTYPE(ndb) & PROC_DISCSUBNETMASK)
				  || iprouting_classless))) {
		printf("%% Subnet not in table");
		return;
	    }
	}
    }

    pdb = INDEX2PDB(ndb->pdbindex);
    if (!pdb) {
	printf("%% Network not in table");
	return;
    }
    mem_lock(ndb);
    printf("\nRouting entry for %i%m", ndb->netnumber,
	   ndb->netsmask);
    if (subnetflag == FALSE) {
	if (ndb->attrflags & NET_SUBNETTED) {
	    printf(", %d known subnets", ndb->subnets);
	    if (ndb->attrflags & NET_ATTACHED) 
		printf("\n  Attached (%d connections)", ndb->attachcount);
	    if (ndb->attrflags & NET_VARIABLE) 
	        printf("\n  Variably subnetted with %d masks", ndb->maskcount);
	}
	if (ndb->attrflags & NET_SUPERNET)
		printf(", supernet");
    }
    if (!(ndb->attrflags & NET_SUBNETTED)) {
	printf("\n  Known via \"%s", pdb->name);
	if (pdb->asystem)
	    printf(" %d", pdb->asystem);
	if (!(ndb->attrflags & NET_SUBNETTED)) {
	    printf("\", distance %d, metric %u", ndb->distance, ndb->metric);
	    if (ndb->metric == METRIC_INACCESSIBLE)
		printf(" (inaccessible)");
	    if (ndb->metric == METRIC_UNKNOWN)
		printf(" (unknown)");
	    if (ndb->attrflags & NET_ATTACHED)
		printf(" (connected)");
	}

	if (ndb->attrflags & NET_EXTERIOR)
	    printf(", candidate default path");

	if (ndb->tag) {
	    printf("\n  Tag ");
	    if ((NDB2PROCTYPE(ndb) & PROC_OSPF) &&
		(ndb->tag & TAG_AUTOMATIC)) {
		if (ndb->tag & TAG_COMPLETE)
		    printf("Complete, ");
		switch (ndb->tag & TAG_PATHLENPART) {
		case TAG_PATHLEN_0:
		    printf("Path Length == 0, ");
		    break;
		case TAG_PATHLEN_1:
		    printf("Path Length == 1, ");
		    break;
		case TAG_PATHLEN_GE1:
		    printf("Path Length >= 1, ");
		    break;
		}
		if (ndb->tag & TAG_ASPART)
		    printf("AS %u, ", ndb->tag & TAG_ASPART);
		if (ndb->tag & TAG_ARBPART)
		    printf("%u", (ndb->tag & TAG_ARBPART) >> TAG_ARBSHIFT);
	    } else
		printf("%u", ndb->tag);
	}

	if (ndb->route_type) {
	    printf(", type ");
	    switch (ndb->route_type) {
	    case RTYPE_INTRA:
		printf("intra area");
		break;
	    case RTYPE_INTER:
		printf("inter area");
		break;
	    case RTYPE_EXTERN1:
		printf("extern 1");
		break;
	    case RTYPE_EXTERN2:
		printf("extern 2, forward metric %d", ndb->forw_dist);
		break;
	    case RTYPE_L1:
		printf("level-1");
		break;
	    case RTYPE_L2:
		printf("level-2");
		break;
	    case RTYPE_IGRP2_INT:
	    case RTYPE_BGP_INT:
		printf("internal");
		break;
	    case RTYPE_IGRP2_EXT:
	    case RTYPE_BGP_EXT:
		printf("external");
		break;
	    case RTYPE_BGP_LOC:
		printf("locally generated");
		break;
	    case RTYPE_NSSA1:
		printf("NSSA extern 1");
		break;
	    case RTYPE_NSSA2:
		printf("NSSA extern 2, forward metric %d", ndb->forw_dist);
		break;
	    default:
		printf("unknown");
		break;
	    }
	} 
    }

    /* Do protocol-specific printing. */
    
    if (pdb && pdb->ndb_print)
	(*pdb->ndb_print)(pdb, ndb);

    headerflag = FALSE;
    for (pdb = (pdbtype *) protoQ.qhead; pdb; pdb = pdb->next) {
	if (ip_redist_flag(ndb, pdb)) {
	    printf("%s%s", (headerflag ? ", " : "\n  Redistributing via "),
		   pdb->name);
	    if (pdb->asystem)
		printf(" %d", pdb->asystem);
	    headerflag = TRUE;
	}
    }
    headerflag = FALSE;
    for (pdb = (pdbtype *) protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->index == pdb_CONNECTED->index)
	    continue;
	if (ndb->advertisingmask & pdb->mask) {
	    printf("%s%s", (headerflag ? "\n                " : 
		   "\n  Advertised by "),
		   pdb->name);
	    if (pdb->asystem)
		printf(" %d", pdb->asystem);
	    headerflag = TRUE;
	    show_network_redistribute(INDEX2PDB(ndb->pdbindex), pdb);
	}
    }

    if (!(ndb->attrflags & NET_SUBNETTED)) {
	pdb = INDEX2PDB(ndb->pdbindex);
	if (!(pdb->proctype & PROC_STATIC) && ndb->lastgateway) {
	    printf("\n  Last update from %i ", ndb->lastgateway);
	    if (ndb->lastinterface)
		printf("on %s, ", ndb->lastinterface->namestring);
	    print_ticks_in_dhms(ELAPSED_TIME(ndb->nettimer));
	    printf(" ago");
	}
	if (TIMER_RUNNING(ndb->blindtimer))
	    printf("\n  Hold down timer expires in %d secs",
		   TIME_LEFT_SLEEPING(ndb->blindtimer)/ONESEC);
	if (ndb->routecount)
	    printf("\n  Routing Descriptor Blocks:");
	for (i = 0; i < ndb->routecount; i++) {
	    rdb = ndb->rdb[i];
	    printf("\n  %c ", (i == ndb->lastindex) ? '*' : ' ');
	    if (rdb->gateway) {
		printf("%i", rdb->gateway);
  		if ((pdb->proctype & (PROC_STATIC+PROC_CONNECTED)) == 0) {
		    printf(", from %i",
			   rdb->infosource);
		    if (!(rdb->rdb_flags & IP_RDB_ISIS)) {
			printf(", ");
			print_ticks_in_dhms(ELAPSED_TIME(rdb->rdb_routetimer));
			printf(" ago");
		    }
		}
	    }
	    else
		printf("directly connected");
	    if (rdb->idb)
		printf(", via %s", rdb->idb->namestring);
	    if (rdb->rdb_flags & IP_RDB_PERMANENT)
		printf(", permanent");
	    if (pdb->rdb_print) {
		char *proto_specific = string_getnext();
		if (proto_specific &&
		    (*pdb->rdb_print)(pdb, rdb, proto_specific))
		    printf(proto_specific);
	    }
	    printf("\n      Route metric is %u, traffic share count is %d",
		   rdb->metric, rdb->robin_reset);
	    if ((pdb->proctype & (PROC_CONNECTED+PROC_STATIC)) == 0)
		if (pdb->proctype & PROC_IGRP) {
		    printf("\n      Total delay is %ld microseconds, "
			   "minimum bandwidth is %ld Kbit",
			   rdb->rdb_delay*10,
			   rdb->rdb_bandwidth ?
			   SCALED_BANDWIDTH(rdb->rdb_bandwidth) : 0); 
		    printf("\n      Reliability %d/255, minimum MTU %d bytes", 
			   rdb->reliability, rdb->rdb_mtu);
		    printf("\n      Loading %d/255, Hops %d",
			   rdb->load, rdb->hopcount);
		}
	    if (pdb->proctype & PROC_BGP)
		printf("\n      AS Hops %d", rdb->hopcount);
	    if (pdb->proctype & PROC_IGRP2) {
		printf("\n      Total delay is %u microseconds, "
		       "minimum bandwidth is %ld Kbit",
		       reg_invoke_igrp2_cdelay(rdb->rdb_delay),
		       rdb->rdb_bandwidth ?
		       IGRP2_BANDWIDTH_SCALE/rdb->rdb_bandwidth : 0);
		printf("\n      Reliability %d/255, minimum MTU %d bytes", 
		       rdb->reliability, rdb->rdb_mtu);
		printf("\n      Loading %d/255, Hops %d",
		       rdb->load, rdb->hopcount);
	    }
	}
    }
    printc('\n');
	    
    /*
     * Show subnet summary for subnetted networks
     */
    if (ndb->attrflags & NET_SUBNETTED) {
	automore_conditional(0);
	(void) print_subnets(ndb, ~0, 0, 0, FALSE);
    }
    ndb_free(ndb, FALSE);
}

/*
 * show_ipmasks(args)
 * Display netmasks and reference counts.
 */

void show_ipmasks (parseinfo *csb)
{
    ipaddrtype address;
    ndbtype *ndb;
    int i;
    masktype *mask;

    if (GETOBJ(paddr,1)->type != ADDR_IP) {
	if (supernetmaskcount) {
	    automore_enable("Supernet masks  Reference count");
	    for (i = 0; i < supernetmaskcount; i++)
		printf("\n%15i %d", supernetmasks[i].mask,
		       supernetmasks[i].count);
	    automore_disable();
	    return;
	}
	printf("%% No supernet masks found");
	return;
    }

    address = GETOBJ(paddr,1)->ip_addr;
    ndb = net_lookup(address, FALSE);
    if (!ndb) {
	printf("%% Network not found");
	return;
    }

    if (!(ndb->attrflags & NET_SUBNETTED)) {
	printf("%% Network not subnetted");
	return;
    }

    if (!(ndb->attrflags & NET_VARIABLE)) {
	printf("Network %i%m", ndb->netnumber, ndb->netsmask); 
    } else {
	mask = ndb->masktable;
	mem_lock(mask);
	automore_enable("Mask            Reference count");
	for (i = 0; i < ndb->maskcount; i++) {
	    printf("\n%15i %d", mask[i].mask, mask[i].count);
	}
	automore_disable();
	free(mask);
    }
}

static ulong get_pdbmask (struct rtrname *prname)
{
    pdbtype *pdb;
    ulong pdbmask;
    clns_pdbtype *cpdb;
    char *token;

    pdbmask = 0;
    /*
     * Find the pdb if we successfully parsed an IP router name
     */
    if (prname->myname) {
	for (pdb = (pdbtype *) protoQ.qhead; pdb; pdb = pdb->next) {
	    if (partialmatch(pdb->name, (char *)prname->myname)) {
		/* Special processing required to match an ISIS tag */
		if (pdb->proctype & PROC_ISIS) {
		    token = (!*(prname->tag)) ? NULL : prname->tag;
		    if (token) {
			cpdb = pdb->osi_pdb;
			if (strlen(token) != strlen(cpdb->pname)) {
			    continue;
			}	
			if (strcasecmp(token, cpdb->pname)) {
			    continue;
			}
		    }
		} else {
		    if (prname->arg_specified &&
			(prname->as != pdb->asystem)) {
			continue;
		    }
		}
		/* Add the current pdb mask to the mask of matching pdbs */
		pdbmask |= pdb->mask;
	    }
	}
    }
    return(pdbmask);
}

/*
 * ip_get_src_pdb
 *
 * Get pdb for IP route that we will use to filter on.  This is either 
 * if this ndb is not a connected route, it is INDEX2PDB(ndb->pdbindex)
 *  else if the redistributing our_pdb runs on this ndb, it is our_pdb
 *    else this ndb is connected route, it is the pdb which has the lowest 
 *         admin distance among protocols that are configured to be 
 *         redistributed by our_pdb
 */
pdbtype *ip_get_src_pdb (pdbtype *our_pdb, ndbtype *ndb,
			 boolean ignore_ownership)
{
    pdbtype *pdb, *best_pdb;

    /*
     * Route is not attached, return owner's pdbindex.
     */
    if ((ndb->attrflags & NET_ATTACHED) == 0)
	return(INDEX2PDB(ndb->pdbindex));

    /*
     * if we have a share of this ndb,
     * we want to advertise it as our own route.   This eliminates
     * the special case for redist connected when we don't want to 
     * return a route as connected if we in fact are running on that 
     * network (with network command).
     */
    if (!ignore_ownership && ip_pdb_on_ndb(ndb, our_pdb))
	return(our_pdb);

    /*
     * Route is attached, if there is more than one routing protocol 
     * configured on the interface, return the one with the better
     * administrative distance.
     */
    best_pdb = NULL;
    for (pdb = (pdbtype *) protoQ.qhead; pdb; pdb = pdb->next) {
	if (pdb == our_pdb) 
	    continue;
	/*
	 * ignore pdb that doesn't (partially) own this ndb
	 */
	if (!ip_pdb_on_ndb(ndb, pdb)) 
	    continue;
	/*
	 * ignore pdb that our_pdb is not config to redistributed
	 */
	if (!(our_pdb->distflag & pdb->mask)) 
	    continue;

	if (!best_pdb || (pdb->distance < best_pdb->distance)) {
	    best_pdb = pdb;
	}
    }

    return(best_pdb);
}

/*
 * ip_get_summary
 *
 * Get summary entry for given address. A longest match will be performed,
 * therefore if the following summaries exist:
 *
 *   1) 10.0.0.0 255.0.0.0
 *   2) 10.1.0.0 255.255.0.0
 *
 *   10.1.2.3 matches 2)
 *   10.2.2.3 matches 1)
 *
 * Returns NULL if there is no summary match.
 */
addrsumtype *ip_get_summary (pdbtype *pdb, ulong address)
{
    addrsumtype *sum, *save_sum;

    save_sum = NULL;
    for (sum = (addrsumtype *) pdb->ip_address_sumq.qhead; sum; 
	 sum = sum->next) {
	if (!(sum->flags & ADDRSUM_VALID))
	    continue;
	if ((address & sum->mask) == sum->network) {

	    /*
	     * Get best match.
	     */
	    if (!save_sum || 
		sum->mask == LONGEST_MASK(sum->mask, save_sum->mask)) {
		save_sum = sum;
	    }
	}
    }
    return(save_sum);
}
