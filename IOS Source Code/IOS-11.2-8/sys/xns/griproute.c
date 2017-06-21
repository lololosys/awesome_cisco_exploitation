/* $Id: griproute.c,v 3.3.60.3 1996/08/28 13:24:17 thille Exp $
 * $Source: /release/112/cvs/Xsys/xns/griproute.c,v $
 *------------------------------------------------------------------
 * griproute.c - support for XNS RIP like routing protocols.
 * 
 * January 1988, Greg Satz
 * May 1989, Bill Westfield, split generic code from xnsroute.c
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Now we have the "generic" XNS RIP code, which can hopefully be used
 * by all of the different XNS-like protocols (since they deal primarilly
 * with the routing tables, which all have the same format.
 *------------------------------------------------------------------
 * $Log: griproute.c,v $
 * Revision 3.3.60.3  1996/08/28  13:24:17  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.60.2  1996/06/28  23:33:17  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/03/18  22:52:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.2  1996/03/07  11:21:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.24.1  1996/02/20  21:59:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  19:21:48  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:53:15  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:58:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/11  23:58:29  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.4  1995/08/25 11:45:56  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.3  1995/06/18 02:24:22  hampton
 * Change the name of an XNS timer variable to something that makes its
 * usage clearer.  While there, remove direct references to msclock.
 * [CSCdi36038]
 *
 * Revision 2.2  1995/06/16 17:10:05  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Eliminate obsolete macro references.
 *
 * Revision 2.1  1995/06/07  23:27:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "interface.h"
#include "packet.h"
#include "ieee.h"
#include "config.h"
#include "xns.h"
#include "apollo.h"
#include "../if/network.h"
#include "logger.h"
#include "../xns/msg_griproute.c"	/* Not a typo, see logger.h */
#include "subsys.h"
#include "../iprouting/route.h"

long xnscache_version;
boolean xns_ub_emulation = FALSE;


/*
 * grip_route_adjust
 * Fix up the routing table when an interface flaps.
 */

void
grip_route_adjust (xns_rip_t	*grip,	    /* ptr to interface */
		   rip_routetable (*table), /* pointer to routing table */
		   int		net,	    /* the network # */
		   sys_timestamp *changed,  /* when the route changed */
		   boolean	running,    /* T if caller's proto running */
		   ulong	*routecount,
		   boolean	debugflag)
{
    xns_ripentrytype *p;
    pathentrytype *pp;
    char *name = "";
    
    if ((!running) || (net == 0L) || (table == NULL) || (grip == NULL))
	return;
    p = grip_find_rt(net, table);

    if (interface_up(grip->idb)) {
	if (grip->static_rts)
	    grip_static_routes(table, grip, TRUE, changed);
	switch (grip->type) {
	  case XNS_TYPE:
	  case UBXNS_TYPE:
	    if (grip->xns_lineup == FALSE) {
		grip->xns_lineup = TRUE;
		xnscache_version++;
	    }
	    break;
	  default:
	    break;
	}
	if (p && (!(p->flags & NET_CONNECTED))) {
	    grip_del_rt(net, TRUE, table, running, debugflag);
	    grip_add_rt(grip, net, 0L, NULL, NET_CONNECTED, 
			table, running, FALSE, debugflag);
	    (*routecount)--;
	    return;
	} 
	/*
	 * In this case, NET is a locally connected network, so there can
	 * be only one path. The metric is always zero, but the route may
	 * be in holddown.
	 */
	if (p && p->paths != 1) {
	    errmsg(&msgsym(BADPATHS, GRIP), p->paths, p->nettype, net);
	    return;
	}
	if (p == NULL) {
	    grip_add_rt(grip, net, 0L, NULL, NET_CONNECTED, 
			table, running, FALSE, debugflag);
	    return;
	}
	if (TIMER_RUNNING(p->holddownuntil)) {	/* Was Down ? */
	    TIMER_STOP(p->holddownuntil);       /* Up now */
	    p->metric = 0;
	    p->hopcount = 0;
	    p->delay = 0;
	    GET_TIMESTAMP(p->changed);
	    if (!TIMER_RUNNING(*changed))
		TIMER_START(*changed, 0);
	} else {
	    return;  /* had route, connected status.  no change */
	}
    } else {				/* interface is down */
	if (p && (!(p->flags & NET_CONNECTED))) 
	    return;
	if (grip->static_rts)
	    grip_static_routes(table, grip, FALSE, changed);
	if (!p || TIMER_RUNNING(p->holddownuntil))	/* already Down ? */
	    return;

	switch (grip->type) {
	case XNS_TYPE:
	case UBXNS_TYPE:
	    if (grip->xns_lineup == TRUE) {
		grip->xns_lineup = FALSE;
		xnscache_version++;
		GET_TIMESTAMP(p->changed);
		if (!TIMER_RUNNING(*changed))
		    TIMER_START(*changed, 0);
		name = "XNS";
	    }
	    break;
	case APOLLO_TYPE:
	    GET_TIMESTAMP(p->changed);
	    if (!TIMER_RUNNING(*changed))
		TIMER_START(*changed, 0);
	    name = "Apollo";
	    break;
	default:
	    break;
	}
	if (interface_admin_down(grip->idb)) {
	   grip_prune_idb(grip->idb, table, routecount, name, debugflag);
	} else {
	    TIMER_START(p->holddownuntil,
			(RIP_HOLDDOWNTIME * grip->update_interval));
            pp = p->path_list;
    	    GET_TIMESTAMP(pp->timer);
	}
    }
}

/*
 * grip_static_routes
 * Handle Static routes
 */

void
grip_static_routes (rip_routetable (*table),
		    xns_rip_t	*grip,
		    boolean	state,
		    sys_timestamp *changed)
{
    xns_ripentrytype *p;
    int i;
    pathentrytype *pp;

    if (!(table && grip))
	return;
    for (i = 0; i < NETHASHLEN; i++) {
	p = (*table)[i];
	for (; p; p = p->next) {
	    pp = p->path_list;
	    if ((pp->idb != grip->idb) || !(p->flags & NET_STATIC))
		continue;
	    if (state) {
		p->valid = TRUE;
		TIMER_STOP(p->holddownuntil);
		p->metric = 0;
		p->hopcount = 0;
		p->delay = 0;
		TIMER_START(pp->timer, 0);
	    } else {
		if (TIMER_RUNNING(p->holddownuntil))
		    continue;
		TIMER_START(p->holddownuntil,
			    (RIP_HOLDDOWNTIME * grip->update_interval));
		if ((p->nettype == XNS_TYPE) || (p->nettype == UBXNS_TYPE))  {
		    xnscache_version++;
		}
		GET_TIMESTAMP(p->changed);
		if (!TIMER_RUNNING(*changed))
		    TIMER_START(*changed, 0);
	    }
	}
    }
}


/*
 * nv_grip_route
 * Non-volatile XNS routing support
 */

void
nv_grip_route (rip_routetable (*table),
	       char *name)
{
    xns_ripentrytype *p;
    int i;

    if (!table)
	return;

    nv_write(TRUE,"!");
    for (i = 0; i < NETHASHLEN; i++) {
	p = (*table)[i];
	for (; p; p = p->next) {
	    /* Step thru the path tables for all paths */
	    nv_write(p->flags & NET_STATIC, "%s route %q %q.%e",
		     name, p->nettype, p->dstnet, p->nettype, 
		     p->path_list->srcnet, p->path_list->srchost);
	}
    }
}

/*
 * grip_ager
 * Age the RIP route table.
 */

void
grip_ager (rip_routetable (*table),
	   ulong	*routecount,
	   char		*name,
	   ushort	maxpaths,
	   sys_timestamp *changed,
	   boolean	debugflag)
{
    xns_ripentrytype *p, *pred;
    int		i;
    pathentrytype *pp, *last_pp;
    xnssb	*xsb;

    for (i = 0; i < NETHASHLEN; i++) {
	p = (*table)[i];
	if (p == NULL)
	    continue;
	pred = p;
	while (p) {
	    /* If this is a static route don't age, age directly connected */
	    /* if in holddown */
	    if (!(p->flags & NET_STATIC)) {
	        if (!TIMER_RUNNING(p->holddownuntil) &&
		    !(p->flags & NET_CONNECTED)) {
		    pp = p->path_list;
		    last_pp = (pathentrytype *) (&p->path_list);
		    while (pp) {
			xsb = idb_get_swsb_inline(pp->idb, SWIDB_SB_XNS);
		        if (CLOCK_OUTSIDE_INTERVAL(pp->timer,
			    pp->grip->update_interval * XNSRIP_INVALIDTIME)) {
			/*
			 * If this is the last path then mark as unreachable
			 * else remove this path now and decrement the number
			 * of paths available.
			 */
			    if ((p->nettype == XNS_TYPE) ||
				(p->nettype == UBXNS_TYPE))
			        xnscache_version++;
			    if (p->paths == 1) {
			        GET_TIMESTAMP(p->changed);
			        if (!TIMER_RUNNING(*changed))
				    GET_TIMESTAMP(*changed);
				TIMER_START(p->holddownuntil,
					    (RIP_HOLDDOWNTIME *
					     p->grip->update_interval));
			        break;
			    } else {
			        if ((last_pp->next = p->nextpath = pp->next) 
					== NULL)
				    p->nextpath = p->path_list;
			        free(pp);
			        pp = last_pp;
			        p->paths--;
			    }
		        }
		        last_pp = pp;
		        if (pp)
			    pp = pp->next;
		    } /* while pp loop */

		/*
		 * Trim down the number of paths if it exceeds the allowed 
		 * value. If the paths need trimming then invalidate the cache.
		 */
		    if ((p->paths > maxpaths) &&
		        ((p->nettype == XNS_TYPE) ||
			 (p->nettype == UBXNS_TYPE)))
		   	    xnscache_version++;
		    while (p->paths > maxpaths) {
		        p->paths--;
		        pp = p->path_list;
		        p->path_list = pp->next;
		        p->nextpath = p->path_list;
		        free(pp);
		    }
	         }   else  {
		    if (TIMER_RUNNING(p->holddownuntil) && p->valid) {
		        pp = p->path_list;
		        if (AWAKE(p->holddownuntil)) {
			    if (p->paths != 1) {
			        errmsg(&msgsym(BADPATHS, GRIP), p->paths,
				       p->nettype, p->dstnet);
 			        return;
			    }
			    if (debugflag)
			      buginf("\n%sRIP: garbage collecting entry for %q",
				   name, p->nettype, p->dstnet);
			    p->valid = FALSE;
			    if (p != pred)
			        pred->next = p->next;
			    else {
			        (*table)[i] = p->next;
			        pred = p->next;
			    }
			    if (p->flags & NET_RIP)
			        (*routecount)--;
			    free(pp);
			    free(p);
			    p = pred;
			}
		    }
		}
	    }
	    pred = p;
	    if (p != NULL) 
		p = p->next;
	}
    }
}

/*
 * grip_getnetwork
 * Scan the XNS RIP routing table feeding back entries. Also
 * do split horizon. If update is set we need to send the entire
 * routing table. Otherwise just send what's changed since the last
 * update.
 */

xns_ripentrytype *
grip_getnetwork (idbtype *idb,
		 boolean fullupdate,
		 int *index,
		 xns_ripentrytype **prev,
		 rip_routetable (*table),
		 boolean split,
		 sys_timestamp lastupdate)
{
    xns_ripentrytype *p;
    int i;

    for (i = *index; i < NETHASHLEN; i++) {
	if (*prev == NULL)
	    p = (*table)[i];
	else {
	    if (!validmem((*prev)) || !validmem((*prev)->next)) {
	        *prev = NULL;
		p = NULL;
		continue;
	    }
	    p = (*prev)->next;
	    *prev = NULL;
	}
	for (; p; p = p->next) {
	    /* If this is a partial (flash) update, only advertize things that
	     * have changed since the last full update. If this is a full
	     * update, don't advertize anything that's in holddown; we flash
	     * those once and never talk about them again.
	     */
	    if (fullupdate) {
		if (TIMER_RUNNING(p->holddownuntil))
		    continue;
	    } else if (!TIMER_EARLIER(lastupdate, p->changed))
		continue;

	    /*
	     * If this is an unnumbered interface it is a duplicate.
	     * So don't include it in the update
	     */
	    if (p->unnumbered)
		continue;
	    /*
	     * If we are doing split horizen and this 
	     * this could be sent back out one of the
	     * interfaces that we know lead to this
	     * net(because it's listed in the multiple
	     * paths) then don't send it.
	     */
	     
	    if (grip_same_idb(idb, p->path_list)) {
	    /*
	     * UB needs to hear about this net from the interface to which
	     * it is attached. Hence it's a "fuzzy" split horizen
	     */
	      if ((split == SPLIT_TRUE) || 
		  ((split == UB_FUZZY_SPLIT) && !(p->flags & NET_CONNECTED)))
		continue;
	    }
	    /*
	     * Routes that have to go through a down interface are
	     * not advertised at all. THe exeption being static and
	     * directly connected routes.
	     */
	    if (!(p->flags & (NET_CONNECTED|NET_STATIC)))
		if (!grip_interface_up(p->path_list))
		    continue;

	    /* 
	     * Static routes don't get deleted, after awhile they
	     * are marked as invalid and then we just dont say
	     * anything more about them until they come back
	     */
	    if ((p->flags & NET_STATIC) && (!(p->valid)))
		continue;
	    
	    *index = i;
	    *prev = p;
	    return(p);
	}
    }
   return(NULL);
}

/*
 * same_idb
 * If this IDB matches any idb in the path list then return TRUE.
 */

boolean
grip_same_idb (idbtype *idb, pathentrytype *pp)
{
    if (idb) 
	for (; pp; pp = pp->next) 
	    if (idb == pp->idb)
		return (TRUE);

    return (FALSE);
}

/*
 * grip_interface_up
 * If any interface is up return TRUE
 */

boolean
grip_interface_up (pathentrytype *pp)
{
    for (; pp; pp = pp->next) 
	if (interface_up(pp->idb))
	    return (TRUE);

    return (FALSE);
}

/*
 * grip_update
 * Perform updating of the XNS RIP routing tables
 */

void
grip_update (xns_rip_t	*grip,
	     ulong	dstnet,
	     ushort	metric,
	     ulong	snet,
	     uchar	*shost,
	     rip_routetable (*table),
	     ulong	*routecount,
	     char	*name,
	     sys_timestamp *changed,
	     ushort	maxpaths,
	     ushort	hopcount,
	     ushort	delay,
	     boolean	debugflag)
{
    xns_ripentrytype	*p;
    int			i;
    pathentrytype	*pp, *prev, *last_pp, *temp_pp;

    i = nethash(dstnet);
    p = (*table)[i];
    for (; p; p = p->next) 
	if (p->dstnet == dstnet)
	    break;
    /*
     * Not found, enter it.
     */
    if (p == NULL) {
        if (hopcount >= XNS_INFINITY)
            return;
	p = malloc(sizeof(xns_ripentrytype));
	if (p == NULL) 
	    return;
	pp = malloc(sizeof(pathentrytype));
	if (pp == NULL) 
	    return;
	p->path_list = p->nextpath = pp;
	TIMER_START(p->started, 0);
	p->dstnet = dstnet;
	TIMER_STOP(p->holddownuntil);
	p->metric = metric;
	p->hopcount = hopcount;
	p->delay = delay;
	p->nettype = grip->type;
	p->grip = grip;
	GET_TIMESTAMP(p->changed);
	if (!TIMER_RUNNING(*changed))
	    GET_TIMESTAMP(*changed);
	p->valid = TRUE;
	pp->idb = grip->idb;
	pp->grip = grip;
	pp->srcnet = snet;
	ieee_copy(shost, pp->srchost);
	TIMER_START(pp->timer, 0);
	p->next = (*table)[i];
	p->paths++;
	(*table)[i] = p;
	if (debugflag)
	    buginf("\n%sRIP: create route to %q via %e, metric %d, hops %d, dly %d",
		   name, p->nettype, dstnet, shost, metric, hopcount, delay);
	(*routecount)++;
	p->flags |= NET_RIP;		/* used as dynamic */
	return;
    } 
    if (p->flags & (NET_STATIC|NET_CONNECTED))
	return;

    /*
     * Equal metric, add multiple path. Also, update hopcount, delay and net
     * type fields, since they could theoretically fluctuate independent
     * of the metric. This means that in a UB environment the hopcount could
     * theoretically oscillate with each update from a UB host that had
     * two paths with the same delay but different hopcounts. Tough.
     * A metric equal to or less than the metric we had when we put a
     * route into holddown will resurrect it. Also, if we only have a
     * single path to a destination, and we haven't heard from the next
     * hop for a while, we'll let the new path overwrite the old, on
     * the theory that the old may have gone away or gotten flaky.
     */
    if (hopcount < XNS_INFINITY && metric == p->metric) {
	TIMER_STOP(p->holddownuntil);
	p->hopcount = hopcount;
	p->delay = delay;
	p->nettype = grip->type;
	p->grip = grip;
	last_pp = pp = p->path_list;
	while (pp) {
	    last_pp = pp;
	    /* One host, one vote */
	    if (pp->srcnet == snet && (bcmp(pp->srchost, 
					    shost, IEEEBYTES) ==0)) {
		GET_TIMESTAMP(pp->timer);
		return;
	    } else if ((maxpaths == 1) &&    /* No vote if you don't use it. */
		       CLOCK_OUTSIDE_INTERVAL(pp->timer,
				  (RIP_STALETIME * pp->grip->update_interval))) {
		/* We're only allowed one path, and we haven't heard from
		 * the one we've been using in a while. Change to a new one.
		 * Reasonably oscillation-safe if RIP_STALETIME is large
		 * enough...
		 */
		if (debugflag)
		    buginf("\n%sRIP: net %q path via %q.%e (%s) is stale; "
			   "now using %q.%e (%s)",  
			   name, p->nettype, p->dstnet,
			   grip->type, pp->srcnet, pp->srchost,
			   pp->idb->namestring,
			   grip->type, snet, shost, grip->idb->namestring);
		pp->srcnet = snet;
		ieee_copy(shost, pp->srchost);
		pp->idb = grip->idb;
		pp->grip = grip;
		GET_TIMESTAMP(pp->timer);
		return;
	    }

	    pp = pp->next;
	}
	if (p->paths < maxpaths) {
	    pp = malloc(sizeof(pathentrytype));
	    if (pp == NULL) 
		return;
	    last_pp->next = pp;
	    p->paths++;
	    p->flags |= NET_RIP;		/* used as dynamic */
	    if (debugflag)
		buginf("\n%sRIP: Adding %d multiple paths for Net %q",  
		       name, p->paths, p->nettype, dstnet);
	    pp->idb = grip->idb;
	    pp->grip = grip;
	    pp->srcnet = snet;
	    ieee_copy(shost, pp->srchost);
	    GET_TIMESTAMP(pp->timer);
	    if ((grip->type == XNS_TYPE) || (grip->type == UBXNS_TYPE))
		xnscache_version++;
	} else {
	    pp = p->path_list;
	    while (pp) {
		/* if this interface is faster then use it instead */
		if (pp->idb->delay > grip->idb->delay) {
		    p->flags |= NET_RIP;		/* used as dynamic */
		    if (debugflag)
			buginf("\n%sRIP: Replacing %d  path for Net %q",  
			       name, p->paths, p->nettype, dstnet);
		    pp->idb = grip->idb;
		    pp->grip = grip;
		    pp->srcnet = snet;
		    ieee_copy(shost, pp->srchost);
		    GET_TIMESTAMP(pp->timer);
		    if (   (grip->type == XNS_TYPE)
			|| (grip->type == UBXNS_TYPE))
			xnscache_version++;
		    return;
		}
		pp = pp->next;
	    }
	}
    }

    /*
     * Better metric
     */
    if (metric < p->metric) {
	if (debugflag)
	    buginf("\n%sRIP: better [%d] route for %q from %x.%e,"
		   " flushing old [%d] route/paths",
		   name, metric, p->nettype, dstnet, snet, shost, p->metric);
	TIMER_STOP(p->holddownuntil);
	p->metric = metric;
	p->hopcount = hopcount;
	p->delay = delay;
	p->nettype = grip->type;
	p->grip = grip;
	pp = malloc(sizeof(pathentrytype));
	if (pp == NULL) 
	    return;
	last_pp = temp_pp = p->path_list;
	while (last_pp) {
	    last_pp = temp_pp->next;
	    free(temp_pp);
	    temp_pp = last_pp;
	}
	if ((grip->type == XNS_TYPE) || (grip->type == UBXNS_TYPE))
	    xnscache_version++;
	p->path_list = p->nextpath = pp;
	p->paths = 1;
	p->valid = TRUE;
	p->flags |= NET_RIP;		/* used as dynamic */
	pp->idb = grip->idb;
	pp->grip = grip;
	pp->srcnet = snet;
	ieee_copy(shost, pp->srchost);
	GET_TIMESTAMP(pp->timer);
	GET_TIMESTAMP(p->changed);
	if (!TIMER_RUNNING(*changed))
	    GET_TIMESTAMP(*changed);
    }
    /*
     * Only hear poison from same protocol
     */
    if (   (p->nettype == grip->type)
	&& ((metric > p->metric) || (hopcount == XNS_INFINITY))) {
	prev = (pathentrytype *) (&p->path_list);
	pp = p->path_list;
	while (pp) {
	    /*
	     * Worse metric from same host with same protocol
	     */
	    if (   (grip->type == p->nettype)
		&& (snet == pp->srcnet) && ieee_equal(pp->srchost, shost)) {
		if (p->paths == 1) {
		    if (TIMER_RUNNING(p->holddownuntil))
			return;
		    if (   (grip->type == XNS_TYPE)
			|| (grip->type == UBXNS_TYPE))
			xnscache_version++;
		    TIMER_START(p->holddownuntil,
		      (RIP_HOLDDOWNTIME * pp->grip->update_interval));
		    GET_TIMESTAMP(p->changed);
		    if (!TIMER_RUNNING(*changed))
			GET_TIMESTAMP(*changed);
		    if (debugflag)
			buginf("\n%sRIP: poison route to %q via %e, metric %d delay %d", 
			       name, p->nettype, dstnet, shost, metric, delay);
		} else {
		    if (   (grip->type == XNS_TYPE)
			|| (grip->type == UBXNS_TYPE))
			xnscache_version++;
		    p->paths--;
		    if (debugflag)
			buginf("\n%sRIP: delete path to %q via %e,"
			       " paths remaining %d", 
			       name, p->nettype, dstnet, shost, p->paths);
		    if ((prev->next = p->nextpath = pp->next) == NULL)
			p->nextpath = p->path_list;
		    free(pp);
		}
		return;
	    }
	    prev = pp;
	    pp = pp->next;
	}
    }
}

/*
 * grip_find_rt
 * Return ptr to routing table based on destination network
 */

xns_ripentrytype *
grip_find_rt (ulong net, rip_routetable (*table))
{
    xns_ripentrytype *p;

    p = (*table)[nethash(net)];
    for (; p; p = p->next) 
	if (net == p->dstnet)
	    break;
    return (p);
}

/*
 * grip_add_rt
 * Add an XNS RIP route to the table
 */

void
grip_add_rt (xns_rip_t	*grip,
	     ulong	dstnet,
	     ulong	net,
	     uchar	*host,
	     ulong	flags,
	     rip_routetable (*table),
	     boolean	running,
	     boolean	unnumbered,
	     boolean	debugflag)
{
    int		bucket;
    xns_ripentrytype *p;
    pathentrytype *pp;

    if (!table) {
	if (running)
	    errmsg(&msgsym(BADROUTE, GRIP), "adding");
	return;
    }
    p = malloc(sizeof(xns_ripentrytype));
    if (p == NULL) 
	return;
    pp = malloc(sizeof(pathentrytype));
    if (pp == NULL) 
	return;
    p->valid = TRUE;
    p->unnumbered = unnumbered;
    GET_TIMESTAMP(p->changed);
    GET_TIMESTAMP(p->started);
    p->path_list = p->nextpath = pp;
    p->paths = 1;
    p->delay = 0;
    p->nettype = grip->type;
    p->grip = grip;
    pp->idb = grip->idb;
    pp->grip = grip;
    p->flags = flags;
    TIMER_STOP(p->holddownuntil);
    p->hopcount = (flags & NET_CONNECTED) ? 0 : 1;
    if (((grip->type == XNS_TYPE) || (grip->type == UBXNS_TYPE)) &&
	(xns_ub_emulation)) {
	p->metric = ub_hops_to_delay (p->hopcount);
	p->delay = p->metric;
    } else
	p->metric = p->hopcount;
	
    TIMER_START(pp->timer, 0);
    p->dstnet = dstnet;
    pp->srcnet = net;
    if (host)
	ieee_copy(host, pp->srchost);
    pp->uses = 0L;
    bucket = nethash(dstnet);
    p->next = (*table)[bucket];
    (*table)[bucket] = p;
}

/*
 * grip_del_rt
 * Remove an XNS RIP route from the table
 */

void 
grip_del_rt (ulong net, boolean	force, rip_routetable (*table),
	     boolean running, boolean debugflag)
{
    xns_ripentrytype	*p;
    xns_ripentrytype	*pred;
    pathentrytype	*pp;
    pathentrytype	*next_pp;
    int			bucket;

    if (!table) {
	if (running)
	    errmsg(&msgsym(BADROUTE, GRIP), "deleting");
	return;
    }
    bucket = nethash(net);
    p = (*table)[bucket];
    pred = p;
    while (p && p->dstnet != net) {
	pred = p;
	p = p->next;
    }
    if (p) {
	if (!force && p->flags == NET_CONNECTED) {
	    printf("\nCan't delete interface route to network %q",
		   p->nettype, p->dstnet);
	    return;
	}
	for (pp = p->path_list; pp; pp = next_pp) {
	    next_pp = pp->next;
	    if (p->flags & NET_STATIC)
		if (pp->idb != NULL && pp->grip != NULL) {
		    pp->grip->static_rts--;
		}
	    free(pp);
	}
	if (p != pred)
	    pred->next = p->next;
	else
	    (*table)[bucket] = p->next;
	free(p);
    }
}

/*
 * grip_prune_idb
 * Delete all paths associated with an idb.
 */

void grip_prune_idb (
    idbtype	*idb,
    rip_routetable (*table),
    ulong	*routecount,
    char	*name,
    boolean	debugflag)
{
    xns_ripentrytype *p, *pred;
    int i;
    pathentrytype *pp, *last_pp;

    for (i = 0; i < NETHASHLEN; i++) {
        p = (*table)[i];
        if (p == NULL)
            continue;
        pred = p;
        while (p) {
            /* If this is a static route don't delete it*/
            if (!(p->flags & NET_STATIC)) {
                last_pp = (pathentrytype *) (&p->path_list);
		pp = p->path_list;
                while (pp) {
                    if (pp->idb == idb) {
			last_pp->next = pp->next;
			p->paths--;
			free(pp);
			pp = last_pp->next;
		    } else { 
			last_pp = pp;
			pp = pp->next;
		    } 
                } 
                p->nextpath = p->path_list;
		if (p->paths <= 0) {
                    if (debugflag)
                            buginf("\n%sRIP: garbage collecting entry for %q",
                                   name, p->nettype, p->dstnet);
                    pp = p->path_list;
                    p->valid = FALSE;
                    if (p != pred)
                        pred->next = p->next;
                    else {
                        (*table)[i] = p->next;
			pred = p->next;
                    }
                    if (p->flags & NET_RIP)
                        (*routecount)--;
                    free(p);
                    p = pred;
		}
	    }
	    pred = p;
	    if (p != NULL)
		p = p->next;
	}
    }
}

/*
 * show_griproutes
 * Display XNS routing information for any XNS-like protocol
 */

void show_griproutes (
    long	net,
    rip_routetable (*table),
    int		count,
    ushort	maxpaths,
    int 	type)
{
    xns_ripentrytype *p = NULL;
    int i;
    pathentrytype *pp;

    automore_enable(NULL);
    if (type == XNS_TYPE)
        printf("\nCodes: R - RIP, U - UB RIP, C - connected, S - static, "
	       "%d learned routes\n",  count);
    else
        printf("\nCodes: R - RIP, C - connected, S - static, %d learned"
	       " routes\n",  count);

    if (maxpaths == 1)
	printf("\nNo");
    else
	printf("\nUp to %d", maxpaths);
    printf(" parallel paths allowed\n");
    for (i = 0; i < NETHASHLEN; i++) {
	p = (*table)[i];
	for (; p; free(p), p = p->next) {
	    if (!validmem(p)) {
		automore_disable();
		break;
	    }
	    /* AFTER THIS POINT, FREE p BEFORE RETURN (BUT NOT CONTINUE) */
	    mem_lock(p);
	    if (net != 0 && net != p->dstnet) 
		continue;
	    pp = p->path_list;
	    if (!validmem(pp)) {
	        free(p);
		automore_disable();
		return;
	    }
	    /* AFTER THIS POINT, FREE pp BEFORE RETURN OR THE 'p' LOOP */
	    mem_lock(pp);
	    if (p->flags & NET_CONNECTED) {
		printf("\nC Net %q is directly connected", 
		       p->nettype, p->dstnet);
		if (TIMER_RUNNING_AND_SLEEPING(p->holddownuntil)) {
		    printf(", in holddown (%u sec remain)",
			   TIME_LEFT_SLEEPING(p->holddownuntil) / ONESEC);
		}
		printf(", %ld uses, %s%s", pp->uses, pp->idb->namestring,
		       (interface_up(pp->idb) ? "" : " (down)"));
		free(pp);
	    }
	    if (p->flags & NET_STATIC) {
		printf("\nS Net %q ",
		       p->nettype, p->dstnet);
		if (TIMER_RUNNING_AND_SLEEPING(p->holddownuntil)) {
		    printf(", in holddown (%d sec remain)",
			   TIME_LEFT_SLEEPING(p->holddownuntil) / ONESEC);
		}
		printf(" via %q.%b, %ld uses, %s%s",
		       p->nettype, pp->srcnet, p->nettype, pp->srchost, 
		       pp->uses, pp->idb->namestring,
		       (interface_up(pp->idb) ? "" : " (down)"));
		free(pp);
	    }
	    if (p->flags & NET_RIP) {
		printf("\n%c Net %q [%d/%d/%d] ",
		       (p->nettype == UBXNS_TYPE) ? 'U' : 'R',
		       p->nettype, p->dstnet, p->metric, p->hopcount,
		       p->delay);
		if (TIMER_RUNNING_AND_SLEEPING(p->holddownuntil)) {
		    printf(", in holddown (%d sec remain) ",
			   TIME_LEFT_SLEEPING(p->holddownuntil) / ONESEC);
		}
		printf("via %q.%b,  %ld sec, %ld uses, %s%s",
		       p->nettype, pp->srcnet, p->nettype, pp->srchost, 
		       ELAPSED_TIME(pp->timer)/ONESEC, pp->uses, 
		       pp->idb->namestring,
		       (interface_up(pp->idb) ? "" : " (down)"));
		free(pp);
		pp = pp->next;
		/* pp IS FREED IN THE FOR LOOP */
		for (; pp; free(pp), pp = pp->next) {
		    if (!validmem(pp)) {
			automore_disable();
			free(p);
			return;
		    }
		    mem_lock(pp);
		    printf("\n%-18s %q.%b,  %ld sec, %ld uses, %s%s",
			   "via", p->nettype, pp->srcnet,
			   p->nettype, pp->srchost, 
			   ELAPSED_TIME(pp->timer)/ONESEC, pp->uses, 
			   pp->idb->namestring,
			   (interface_up(pp->idb) ? "" : " (down)"));
		}
	    }
	}
    }
    automore_disable();
}

/*
 * grip_first_hop
 * Determine first hop for a datagram based on destination address
 */

uchar *
grip_first_hop (ulong dnet,
		uchar *dhost,
		idbtype **if_output,
		xns_ripentrytype *p)
{
    pathentrytype *pp, *ppp;

    if (dnet == -1)
	return (dhost);

    /* Don't use a route that is been poisoned */
    while (p) {
	if ((p->dstnet == dnet) && !TIMER_RUNNING(p->holddownuntil))
	    break;
	p = p->next;
    }
    if (p == NULL)
	return(NULL);
    if (!(pp = ppp = p->nextpath))
	return(NULL);

    /*
     * If the interface is not up then find one that is
     */
    for (;;) {
        if (interface_up(pp->idb))
	    break;
        if (!(pp = pp->next))
            pp = p->path_list;
        if (pp == ppp)
            return(NULL);
    }
    if (!(p->nextpath = pp->next))
	p->nextpath = p->path_list;
    pp->uses++;
    *if_output = pp->idb;
    return(p->flags == NET_CONNECTED ? dhost : pp->srchost);
}

/* grip_purgetable
 * Delete all learned entries in a routing table
 */
void grip_purgetable (rip_routetable (*table))
{
    int i;			/* Loop index */
    xns_ripentrytype *p;	/* Route data for a network */
    xns_ripentrytype *lastp;	/* Save last valid p for unlinking */
    pathentrytype *pp;		/* Path entry in chain attached to a "p" */
    pathentrytype *lastpp;	/* Save pp for deletion */

    if (table == NULL) {
	return;
    }

    /* Loop over all the hash chains */
    for (i=0; i < NETHASHLEN; i++) {
	lastp = NULL;
	p = (*table)[i];
	while (p != NULL)
	    /* Only do routes that were learned dynamically */
	    if (p->flags & NET_RIP) {
		/* Start by throwing away all the path entries */
		pp = p->path_list;
		while (pp != NULL) {
		    lastpp = pp;
		    pp = pp->next;
		    free (lastpp);
		}
		/* Unlink this network from the hash chain */
		if (lastp == NULL) {
		    (*table)[i] = p->next;
		    free(p);
		    p = (*table)[i];
		} else {
		    lastp->next = p->next;
		    free(p);
		    p = lastp->next;
		}
	    } else {
		/* Not unlinking; this net is valid and we keep it */
		lastp = p;
		p = p->next;
	    }
    }
}

/*
 * griproute_init
 * Currently, does nothing, as nothing needs to be done to initialize this
 * library.
 */
static void griproute_init (subsystype *subsys)
{
    return;
}
 
/*
 * Griproute subsystem header
 */
#define GRIPROUTE_MAJVERSION 1
#define GRIPROUTE_MINVERSION 0
#define GRIPROUTE_EDITVERSION 1

SUBSYS_HEADER (griproute, GRIPROUTE_MAJVERSION, GRIPROUTE_MINVERSION,
	       GRIPROUTE_EDITVERSION, griproute_init, SUBSYS_CLASS_LIBRARY,
	       NULL, NULL);
