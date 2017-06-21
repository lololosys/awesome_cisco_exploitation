/* $Id: xnsroute.c,v 3.3.60.3 1996/08/28 00:53:01 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/xnsroute.c,v $
 *------------------------------------------------------------------
 * xnsroute - support for XNS RIP
 *
 * January 1988, Greg Satz
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xnsroute.c,v $
 * Revision 3.3.60.3  1996/08/28  00:53:01  hampton
 * Migrate Desktop Protocols away from the one minute and one second
 * registries.  [CSCdi67383]
 * Branch: California_branch
 *
 * Revision 3.3.60.2  1996/06/12  19:24:00  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xxbit_word (final part)
 *
 * All good things must come to an end.  ntohl() and friends finally have
 * UNIX semantics in California releases and later.
 *
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/03/18  22:54:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.2  1996/03/07  11:23:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.24.1  1996/02/20  22:02:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  19:26:04  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:54:31  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  14:01:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/09/11  23:59:10  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.7  1995/07/11 01:24:21  dstine
 * CSCdi36892:  XNS UB hello timer set inconsistently
 *
 * Revision 2.6  1995/06/21  02:12:46  hampton
 * Wrong argument passed to xns_sendupdate.  [CSCdi36214]
 *
 * Revision 2.5  1995/06/18 06:20:20  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.4  1995/06/18 02:24:30  hampton
 * Change the name of an XNS timer variable to something that makes its
 * usage clearer.  While there, remove direct references to msclock.
 * [CSCdi36038]
 *
 * Revision 2.3  1995/06/16 17:10:41  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Eliminate obsolete macro references.
 *
 * Revision 2.2  1995/06/09  13:26:27  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:31:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "ieee.h"
#include "config.h"
#include "xns.h"
#include "xnsfast.h"
#include "xns_debug.h"
#include "../if/network.h"

boolean xns_purgeflag = FALSE;	        /* XNS route table need purging? */

sys_timestamp xns_timer;		/* Next time to send major updates */
static sys_timestamp xns_lastaged;	/* Last time XNS ran grip_ager */
static sys_timestamp xns_changed;	/* Last time XNS routing tbl changed */
sys_timestamp xns_ubhellotimer;		/* Next time to send UB XNS hellos */
static sys_timestamp xns_agecache_timer; /* Next time to age switching cache */

ulong xns_routes;
ushort xns_maxpaths;                    /* Number of paths to remember/use */
rip_routetable *xns_table;		/* routing table for XNS */

/*
 * xns_routeBLOCK
 * Block waiting for something for the XNS RIP router process to do
 */

boolean xns_routeBLOCK (queuetype *q)
{
    if (!xns_running)
	return(TRUE);			/* never if not running */
    if (q->qhead)
	return(FALSE);			/* input available */
    if (!SLEEPING(xns_timer))
	return(FALSE);			/* timer expired */
    if (xns_ub_emulation && !SLEEPING(xns_ubhellotimer))
	return (FALSE);                 /* The other timer expired */
    if (TIMER_RUNNING(xns_changed) &&
	CLOCK_OUTSIDE_INTERVAL(xns_changed, RIP_FLASHUPDATE))
	return(FALSE);			/* topology changed */
    if (!SLEEPING(xns_agecache_timer))
	return(FALSE);			/* topology changed */
    if (xns_purgeflag)		        /* table needs purging */
	return(FALSE);
    return(TRUE);
}

/*
 * xns_router
 * XNS RIP router process. Perform routing maintaince functions.
 */

forktype xns_router (void)
{
    paktype	*pak;
    xns_riptype *rip;
    xnshdrtype	*xns;
    boolean	full;
    boolean	part;
    idbtype	*idb;
    list_element *l_elt;

    edisms((blockproc *)systeminitBLOCK, (ulong)&xnsripQ);
    process_set_priority(PRIO_LOW);
    xns_routes = 0;
    TIMER_START(xns_timer, 0);
    TIMER_START(xns_ubhellotimer, 0);
    TIMER_STOP(xns_changed);
    TIMER_START(xns_agecache_timer, ONEMIN);
    xns_purgeflag = TRUE;	/* Will force us to send a RIP request */
    full = FALSE;
    part = FALSE;

    while (TRUE) {
	edisms((blockproc *)xns_routeBLOCK, (long)&xnsripQ);

	/* If something happened to force us to clean out our routing table,
	 * do so...
	 */
	if (xns_purgeflag) {
	    /* Empty the table */
	    grip_purgetable(xns_table);
	    /* Invalidate the cache */
	    xnscache_version++;
	    /* Tell interested users what we did */
	    if (xnsrip_debug)
		buginf("\nXNSRIP: purged dynamic entries from routing table");
	    /* Send a request so the table will get repopulated soon */
	    xns_sendrequest();
	    /* Don't do it again until we need to */
	    xns_purgeflag = FALSE;
	}

	/*
	 * First empty the RIP packet queue
	 * Packets are put on this queue only if the IDB was previously
	 * configured with a non-zero XNS network number.
	 */
	while ((pak = dequeue(&xnsripQ))) {
	    xns = (xnshdrtype *)xnsheadstart(pak);
	    if (xns->dsock == XNS_SOCK_RIP && xns->pt == XNS_TYPE_RIP &&
		interface_up(pak->if_input)) {
		rip = (xns_riptype *)(xnsheadstart(pak) + XNSHEADERBYTES);
		switch (rip->cmd) {
		case RIP_REQUEST:
		    if (xnsrip_debug)
			buginf("\nXNSRIP: received request from %u.%e.%d",
			    GET2WD(xns->ssnet), xns->shost, xns->ssock);
		    xns_rcvrequest(pak, rip);
		    break;
		case RIP_REPLY:
		    xns_riprcvupd(pak, rip);
		    break;
		case X_RIP_REQUEST:
		    if (xnsrip_debug)
			buginf("\nXNSRIP: received X-tended request from %u.%e.%d",
			    GET2WD(xns->ssnet), xns->shost, xns->ssock);
		    xns_xrcvrequest(pak, (x_riptype *)rip);
		    break;
		case X_RIP_REPLY:
		    /* Not used in this implementation */
		    break;
		}
	    }
	    if (xns_ub_emulation &&
		xns->dsock == XNS_SOCK_UBIPC && xns->pt == XNS_TYPE_UBIPC &&
		interface_up(pak->if_input)) {
		rip = (xns_riptype *)(xnsheadstart(pak) + XNSHEADERBYTES);
		if (rip->cmd == UB_RIP_REPLY)
		    xns_ubrcvupd(pak, rip);
	    }
	    retbuffer(pak);
	}
	/*
	 * Send updates, reset wakeup timer
	 */
	if (AWAKE(xns_timer)) {
	    TIMER_STOP(xns_timer);
	    full = TRUE;
	}
	if (TIMER_RUNNING(xns_changed) &&
	    CLOCK_OUTSIDE_INTERVAL(xns_changed, RIP_FLASHUPDATE))
	    part = TRUE;

	if (full && CLOCK_OUTSIDE_INTERVAL(xns_lastaged, RIP_AGEINTERVAL)) {
	    grip_ager(xns_table, &xns_routes,
		      "XNS", xns_maxpaths, &xns_changed, xnsrip_debug);
	    GET_TIMESTAMP(xns_lastaged);
	} 

	if (full || part) {
	    xns_majorupdate(&xns_changed, full, part);
	    (void) process_may_suspend();
	}
	full = part = FALSE;

	/* Send hellos if we're doing UB */
	if (xns_ub_emulation && (AWAKE(xns_ubhellotimer))) {
	    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_XNS, l_elt, idb)
		ub_hello(idb, idb_get_swsb_inline(idb, SWIDB_SB_XNS));
	    if (!TIMER_RUNNING(xns_ubhellotimer))
		TIMER_START(xns_ubhellotimer, 0);
	    TIMER_UPDATE(xns_ubhellotimer, UB_HELLO_INTERVAL);
	}
	if (!SLEEPING(xns_agecache_timer)) {
	    TIMER_START(xns_agecache_timer, ONEMIN);
	    xns_fscache_age();
	}
	(void) process_may_suspend();
    }
}

/*
 * xns_majorupdate
 * Perform an XNS RIP general broadcast.
 */
void xns_majorupdate (
		      sys_timestamp *changed,
		      boolean	full,
		      boolean	part)
{
    idbtype	*idb;
    boolean	updatesent;
    xnssb	*xsb;
    list_element *l_elt;

    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_XNS, l_elt, idb) {
	xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
	if (!(xsb && xsb->xnsnet))
	    continue;
	/* First do standard XNS RIP updates, both full and partial */
 	updatesent = FALSE;
	if (full) {
	    if (AWAKE(xsb->xnsrip.updatetimer)) {
		TIMER_UPDATE(xsb->xnsrip.updatetimer, xsb->xnsrip.update_interval);
		if (interface_up(idb))
		    xns_sendupdate(idb, xsb->xnsnet, baddr, 0, TRUE,
				   SPLIT_TRUE, &xsb->xnsrip);
		updatesent = TRUE;
	    }
	    /* This should be done for all interfaces not just the ones 
	     * we are sending to this iteration of the clock. Otherwise
             * we end up setting xns_timer to some time after another
             * interface's timer has expired.
             */
	    if (!TIMER_RUNNING(xns_timer) ||
		(TIMER_EARLIER(xsb->xnsrip.updatetimer, xns_timer))) {
		COPY_TIMESTAMP(xsb->xnsrip.updatetimer, xns_timer);
	    }
	} 
	if (part && !updatesent) {
	    if (interface_up(idb)) 
		xns_sendupdate(idb, xsb->xnsnet, baddr, 0, FALSE,
			       SPLIT_TRUE, &xsb->xnsrip);
	    TIMER_STOP(*changed);
	}
	/* Next do UB XNS "RIP" updates, again full and partial */
 	updatesent = FALSE;
	if (xns_ub_emulation && full) {
          /* If the timer is disabled, or too far in futre, fix it... */
          if (!TIMER_RUNNING(xsb->ubxnsrip.updatetimer) ||
              (xsb->ubxnsrip.update_interval != UB_UPDATE_INTERVAL) ||
	      (TIME_LEFT_SLEEPING(xsb->ubxnsrip.updatetimer) >
	           UB_UPDATE_INTERVAL * 2)) {
	    if (xnsrip_debug) {
		buginf("\nXNSRIP: Resynching UB update interval for "
		       "interface %s", idb->namestring);
		buginf("\nXNSRIP: Old values: Timer %#Ta, Interval %x.",
		       xsb->ubxnsrip.updatetimer, xsb->ubxnsrip.update_interval);
	    }
            GET_TIMESTAMP(xsb->ubxnsrip.updatetimer);
            xsb->ubxnsrip.update_interval = UB_UPDATE_INTERVAL;
          }
	    if (AWAKE(xsb->ubxnsrip.updatetimer)) {
		TIMER_UPDATE(xsb->ubxnsrip.updatetimer, xsb->ubxnsrip.update_interval);
		if (interface_up(idb))
		    xns_sendupdate(idb, xsb->xnsnet, baddr, 0, TRUE,
				   UB_FUZZY_SPLIT, &xsb->ubxnsrip);
		updatesent = TRUE;
		if (!TIMER_RUNNING(xns_timer) ||
		    (TIMER_EARLIER(xsb->ubxnsrip.updatetimer, xns_timer))) {
		    COPY_TIMESTAMP(xsb->ubxnsrip.updatetimer, xns_timer);
		}
	    }
	} 
	if (xns_ub_emulation && part && !updatesent) {
	    if (interface_up(idb)) 
		xns_sendupdate(idb, xsb->xnsnet, baddr, 0, FALSE,
			       UB_FUZZY_SPLIT, &xsb->ubxnsrip);
	    TIMER_STOP(*changed);
	}
    }
    /*
     * No interfaces are enabled for XNS, but allow things to age, etc.
     * This also keeps the xns_route process from constantly waking.
     */
    if (!TIMER_RUNNING(xns_timer)) {
	TIMER_UPDATE(xns_timer, ONEMIN);
    }
}

/*
 * xns_sendrequest
 * Send an XNS RIP request for routing information
 */
void xns_sendrequest (void)
{
    idbtype	*idb;
    paktype	*pak;
    paktype	*newpak;
    xnshdrtype	*xns;
    xnshdrtype	*newxns;
    int		bytes;
    xns_riptype *rip;
    xnssb	*xsb;
    list_element	*l_elt;

    bytes = XNSHEADERBYTES + sizeof(xns_riptype);
    pak = getbuffer(bytes);
    if (pak == NULL)
	return;
    pak->if_input = NULL;
    xns = (xnshdrtype *)xnsheadstart(pak);
    xns->cksum = 0;
    xns->len = bytes;
    xns->tc = 0;
    xns->pt = XNS_TYPE_RIP;
    ieee_copy(baddr, xns->dhost);
    xns->dsock = XNS_SOCK_RIP;
    xns->ssock = XNS_SOCK_RIP;
    rip = (xns_riptype *)(xnsheadstart(pak) + XNSHEADERBYTES);
    rip->cmd = RIP_REQUEST;
    PUT2WD(rip->nets[0].nnet, -1L);
    rip->nets[0].metric = XNS_INFINITY;
    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_XNS, l_elt, idb) {
	xsb = idb_get_swsb(idb, SWIDB_SB_XNS);
	if (xsb == NULL)
	    continue;
	if (interface_up(idb) && xsb->xnsnet != 0L) {
	    if (xnsrip_debug)
		buginf("\nXNSRIP: broadcasting standard general request on %s",
		    idb->namestring);
	    newpak = pak_duplicate(pak);
	    if (newpak != NULL) {
		newxns = (xnshdrtype *)xnsheadstart(newpak);
		PUT2WD(newxns->ddnet, xsb->xnsnet);
		PUT2WD(newxns->ssnet, xsb->xnsnet);
		bcopy(xsb->xns_node, newxns->shost, XNS_HOSTBYTES);
		newpak->if_output = idb;
		xns_crc(newpak);
		if (xns_send1(newpak, baddr))  /* broadcast */
		    if (xnspkt_debug)
			buginf("\nXNS: src=%u.%e, dst=%u.%e, packet sent in xns_sendrequest",
			    xsb->xnsnet, xsb->xns_node, xsb->xnsnet, baddr);
	    }
	}
    }
    retbuffer(pak);
}

/*
 * xns_sendupdate
 * Send an XNS RIP update. This also does UB updates, which is impure and
 * ugly, but probably has better immunity from future bug introductions.
 */

void
xns_sendupdate (
		idbtype		*idb,
		ulong		net,
		uchar const	*host,
		ushort		sock,
		boolean		update,
		boolean		split,
		xns_rip_t	*sbrip)
{
    paktype	*pak;
    xnshdrtype	*xns;
    xns_riptype *rip;
    xns_ripinfotype *p;
    xns_ripentrytype *entry, *temp;
    int		nbytes, bucket;
    boolean	isub, done;
    sys_timestamp lastupdate;
    xnssb	*xsb;


    isub = (sbrip->type == UBXNS_TYPE);
    if (xnsrip_debug)
	buginf("\nXNSRIP: sending %s %s to %u.%e via %s",
	       (isub ? "UB" : "standard"),
	       (update ? (ieee_equal(host, baddr) ?
			  "periodic update" : "response") :
		"flash update"), net, host, idb->namestring);
    xsb = idb_get_swsb(idb, SWIDB_SB_XNS);
    if (xsb == NULL)
	return;
    /* Don't set interface timestamps here as this is called by
     * request handling.
     */
    COPY_TIMESTAMP(sbrip->lastupdate, lastupdate);
    GET_TIMESTAMP(sbrip->lastupdate);
    done = FALSE;
    bucket = 0;
    temp = NULL;
    while (!done) {
	/* &&&&& I wonder what the real maximum size for UB is.... */
	nbytes = RIP_MAXSIZE;
	pak = getbuffer(nbytes);
	if (!pak)
	    return;
	pak->if_input = NULL;
	pak->if_output = idb;
	xns = (xnshdrtype *)xnsheadstart(pak);
	xns->cksum = 0;
	nbytes -= (XNSOVERHEADBYTES + sizeof(rip->cmd));
	xns->tc = 0;
	xns->pt = isub ? XNS_TYPE_UBIPC : XNS_TYPE_RIP;
	PUT2WD(xns->ssnet, xsb->xnsnet);
	PUT2WD(xns->ddnet, (isub ? 0 : net));
	ieee_copy(host, xns->dhost);
	xns->dsock = sock ? sock : (isub ? XNS_SOCK_UBIPC : XNS_SOCK_RIP);
	ieee_copy(xsb->xns_node, xns->shost);
	xns->ssock = isub ? XNS_SOCK_UBIPC : XNS_SOCK_RIP;
	rip = (xns_riptype *)(xnsheadstart(pak) + XNSOVERHEADBYTES);
	rip->cmd = isub ? UB_RIP_REPLY : RIP_REPLY;
	p = rip->nets;
	while (nbytes > 0) {
	    entry = grip_getnetwork(idb, update, &bucket, &temp,
				     xns_table, split, lastupdate);
	    if (entry == NULL) {
		done = TRUE;
		break;
	    }
	    PUT2WD(p->nnet, entry->dstnet);
	    /* Say anything about this net? */
	    if ((xns_filtercheck(entry->dstnet, xsb->xns_out_filterlist))){
		if (isub) {
		    /* UB update; need to include both delay and hop count,
		     * neither of which can be greater than its "infinite"
		     * value. If the entry is in holddown, we need to *make*
		     * the metric "infinite". Note that we'll only talk about
		     * nets that are in holddown if this is a flash update;
		     * that's the only time grip_getnetwork will return such
		     * an entry.
		     */
		    if (!TIMER_RUNNING(entry->holddownuntil))
			p->metric =
			    ub_packmetric (
			        min(entry->hopcount + 1, XNS_INFINITY), 
				min(((long) entry->delay +
				   ub_vis_bw_to_delay(idb->visible_bandwidth)),
				    UB_MAXDELAY));
		    else
			p->metric = ub_packmetric (XNS_INFINITY, UB_MAXDELAY);
		} else {
		    /* Standard update; no more than infinity, but always
		     * infinite for a network in holddown. Again, nets in
		     * holddown only get sent in flash updates.
		     */
		    if (!TIMER_RUNNING(entry->holddownuntil))
			p->metric = min(entry->hopcount + 1, XNS_INFINITY);
		    else
			p->metric = XNS_INFINITY;
		}
	    	if (xnsrip_debug) {
		    buginf ("\n    network %u", GET2WD(p->nnet));
		    if (isub)
		    	buginf(", delay %d, hops %d, UB metric 0x%04x",
			   ub_getdelay(p->metric), ub_gethops(p->metric),
			   p->metric);
		    else
		    	buginf (", hop count %u", p->metric);
	    	}
	    	nbytes -= sizeof(xns_ripinfotype);
	    	p++;
	    	(void) process_may_suspend();
	    }
	}
	xns->len = RIP_MAXSIZE - nbytes;
	if (xns->len > XNSOVERHEADBYTES + sizeof(rip->cmd)) {
	    if (!isub)
		xns_crc(pak);
	    else
		xns->cksum = 0xffff;
	    if (xns_send1(pak, host)) {
		if (xnspkt_debug) {
		    buginf("\nXNS: src=%u.%e, dst=%u.%e, packet sent in xns_sendupdate",
			   xsb->xnsnet, xsb->xns_node, net, host);
		}
		if (xns_rip_delay) {
		    process_sleep_for(XNS_RIP_DELAY_TIME);
		}
	    }
	} else {
	    if (xnsrip_debug)
		buginf("\nXNSRIP: suppressing null %s update on %s",
		       isub ? "UB" : "standard RIP", idb->namestring);
	    retbuffer(pak);
	}
    }
}


/*
 * xns_xsendupdate
 * Send an extended XNS RIP update.
 */

void
xns_xsendupdate (idbtype *idb,
		 ulong net,
		 uchar const *host,
		 ushort sock,
		 boolean update,
		 boolean split)
{
    paktype	*pak;
    xnshdrtype	*xns;
    x_riptype	*xrip;
    x_ripinfotype *p;
    xns_ripentrytype *entry, *temp;
    pathentrytype *pp;
    int		nbytes, bucket;
    boolean	done;
    sys_timestamp lastupdate;
    xnssb	*xsb;

    if (xnsrip_debug)
	buginf("\nXNSRIP: sending X-tended update to %u.%e via %s",
	    net, host, idb->namestring);
    xsb = idb_get_swsb(idb, SWIDB_SB_XNS);
    if (xsb == NULL)
	return;
    /* Don't set interface timestamps here as this is called by
     * request handling.
     */
    COPY_TIMESTAMP(xsb->xnsrip.lastupdate, lastupdate);
    GET_TIMESTAMP(xsb->xnsrip.lastupdate);
    done = FALSE;
    bucket = 0;
    temp = NULL;
    while (!done) {
	nbytes = RIP_MAXSIZE;
	pak = getbuffer(nbytes);
	if (!pak)
	    return;
	pak->if_input = NULL;
	pak->if_output = idb;
	xns = (xnshdrtype *)xnsheadstart(pak);
	xns->cksum = 0;
	nbytes -= (XNSOVERHEADBYTES + sizeof(xrip->cmd));
	xns->tc = 0;
	xns->pt = XNS_TYPE_RIP;
	PUT2WD(xns->ddnet, net);
	ieee_copy(host, xns->dhost);
	xns->dsock = sock ? sock : XNS_SOCK_RIP;
	PUT2WD(xns->ssnet, xsb->xnsnet);
	ieee_copy(xsb->xns_node, xns->shost);
	xns->ssock = XNS_SOCK_RIP;
	xrip = (x_riptype *)(xnsheadstart(pak) + XNSOVERHEADBYTES);
	xrip->cmd = X_RIP_REPLY;
	p = xrip->nets;
	while (nbytes > 0) {
	    entry = grip_getnetwork(idb, update, &bucket, &temp, xns_table, 
				    split, lastupdate);
	    if (entry == NULL) {
		done = TRUE;
		break;
	    }
	    /* Say anything about this net? */
	    if (!xns_filtercheck(entry->dstnet, xsb->xns_out_filterlist))
		continue;
	    pp = entry->path_list;
	    while (pp && (nbytes > 0)) {
		PUT2WD(p->nnet, entry->dstnet);
		PUT2WD(p->ggwnet, pp->srcnet);
		ieee_copy(pp->srchost, p->gwhost);
		if (TIMER_RUNNING(entry->holddownuntil))
		    p->metric = XNS_INFINITY;
		else
		    p->metric = min(entry->hopcount + 1, XNS_INFINITY);
		if (xnsrip_debug)
		    buginf("\n    network %u, metric %d via %u.%e", 
			   GET2WD(p->nnet), p->metric);
		nbytes -= sizeof(x_ripinfotype);
		p++;
		pp = pp->next;
	    }
	    (void) process_may_suspend();
	}
	xns->len = RIP_MAXSIZE - nbytes;
	if (xns->len > XNSOVERHEADBYTES + sizeof(xrip->cmd)) {
	    xns_crc(pak);
	    if (xns_send1(pak, host)) {
		if (xnspkt_debug) {
		    buginf("\nXNS: src=%u.%e, dst=%u.%e, packet sent in xns_xsendupdate",
			   xsb->xnsnet, xsb->xns_node, net, host);
		}
		if (xns_rip_delay) {
		    process_sleep_for(XNS_RIP_DELAY_TIME);
		}
	    }
	} else {
	    if (xnsrip_debug)
		buginf("\n    null update suppressed");
	    retbuffer(pak);
	}
    }
}

/*
 * xns_rcvrequest
 * Receive and process a RIP route request
 */

void
xns_rcvrequest (paktype *pak, xns_riptype *rip)
{
    paktype	*newpak;
    xnshdrtype	*xns;
    xnshdrtype	*newxns;
    xns_riptype *newrip;
    xns_ripinfotype *p;
    xns_ripentrytype *entry;
    int		bytes;
    xnssb	*xsb;


    xns = (xnshdrtype *)xnsheadstart(pak);
    xsb = idb_get_swsb(pak->if_input, SWIDB_SB_XNS);
    if (xsb == NULL)
	return;
    if (GET2WD(xns->ssnet) == 0L)
	PUT2WD(xns->ssnet, xsb->xnsnet);

    /*
     * In this case we do not do split horizon
     */
    if ((xns->len == (XNSHEADERBYTES + sizeof(xns_riptype))) &&
	(GET2WD(rip->nets[0].nnet) == -1L)) {
	xns_sendupdate(pak->if_input, GET2WD(xns->ssnet), xns->shost,
		       xns->ssock, TRUE, SPLIT_FALSE, &xsb->xnsrip);
	return;
    }
    newpak = pak_duplicate(pak);
    if (newpak == NULL)
	return;
    newxns = (xnshdrtype *)xnsheadstart(newpak);
    newpak->if_output = pak->if_input;
    newxns->cksum = 0;
    newxns->pt = XNS_TYPE_RIP;
    newxns->tc = 0;
    PUT2WD(newxns->ddnet, GET2WD(xns->ssnet));
    ieee_copy(xns->shost, newxns->dhost);
    newxns->dsock = xns->ssock;
    PUT2WD(newxns->ssnet, xsb->xnsnet);
    ieee_copy(xsb->xns_node, newxns->shost);
    newxns->ssock = XNS_SOCK_RIP;
    newrip = (xns_riptype *)(xnsheadstart(newpak) + XNSOVERHEADBYTES);
    newrip->cmd = RIP_REPLY;
    p = newrip->nets;
    bytes = newxns->len;
    while (bytes > XNSOVERHEADBYTES + sizeof(rip->cmd)) {
	entry = grip_find_rt(GET2WD(p->nnet), xns_table);
	p->metric = XNS_INFINITY;
	/* Say anything about this net? */
	if (entry != NULL)
	    if (xns_filtercheck(entry->dstnet, xsb->xns_out_filterlist)
		&& !TIMER_RUNNING(entry->holddownuntil))
		p->metric = min(entry->hopcount + 1, XNS_INFINITY);
	bytes -= sizeof(xns_ripinfotype);
	p++;
    }
    xns_crc(newpak);
    (void)xns_send(newpak);
}


/*
 * xns_xrcvrequest
 * Receive and process a extendted RIP route request
 */

void
xns_xrcvrequest (paktype *pak, x_riptype *xrip)
{
    paktype	*newpak;
    xnshdrtype	*xns;
    xnshdrtype	*newxns;
    x_riptype	*newrip;
    x_ripinfotype *p;
    xns_ripentrytype *entry;
    pathentrytype *pp;
    int		bytes;
    ushort	metric;
    idbtype	*idb;
    xnssb	*xsb_in;


    xns = (xnshdrtype *)xnsheadstart(pak);
    idb = pak->if_input;
    if (idb == NULL)
	return;
    xsb_in = idb_get_swsb(idb, SWIDB_SB_XNS);
    if (xsb_in == NULL)
	return;

    if (GET2WD(xns->ssnet) == 0L)
	PUT2WD(xns->ssnet, xsb_in->xnsnet);
    if ((xns->len == (XNSHEADERBYTES + sizeof(x_riptype))) &&
	(GET2WD(xrip->nets[0].nnet) == -1L)) {
	xns_xsendupdate(idb, GET2WD(xns->ssnet), xns->shost, xns->ssock,
			TRUE, SPLIT_FALSE);
	return;
    }
    newpak = pak_duplicate(pak);
    if (newpak == NULL)
	return;
    newxns = (xnshdrtype *)xnsheadstart(newpak);
    newpak->if_output = pak->if_input;
    newxns->cksum = 0;
    PUT2WD(newxns->ddnet, GET2WD(xns->ssnet));
    ieee_copy(xns->shost, newxns->dhost);
    newxns->dsock = xns->ssock;
    PUT2WD(newxns->ssnet, xsb_in->xnsnet);
    ieee_copy(xsb_in->xns_node, newxns->shost);
    newxns->ssock = XNS_SOCK_RIP;
    newxns->tc = 0;
    newrip = (x_riptype *)(xnsheadstart(newpak) + XNSOVERHEADBYTES);
    newrip->cmd = X_RIP_REPLY;
    p = newrip->nets;
    bytes = newxns->len;
    while (bytes > XNSOVERHEADBYTES + sizeof(xrip->cmd)) {
	metric = XNS_INFINITY;
	entry = grip_find_rt(GET2WD(p->nnet), xns_table);
	if (entry != NULL)
	    if (xns_filtercheck(entry->dstnet,
				xsb_in->xns_out_filterlist) &&
		!TIMER_RUNNING(entry->holddownuntil))
		metric = min(entry->hopcount + 1, XNS_INFINITY);
	pp = entry->path_list;
	p->metric = metric;
	PUT2WD(p->ggwnet, pp->srcnet);
	ieee_copy(pp->srchost, p->gwhost);
	bytes -= sizeof(x_ripinfotype);
	p++;
    }
    xns_crc(newpak);
    (void)xns_send(newpak);
}

/*
 * xns_riprcvupd
 * Process an XNS RIP packet
 */

void
xns_riprcvupd (paktype *pak, xns_riptype *rip)
{
    xnshdrtype	*xns;
    xns_ripinfotype *p;
    idbtype	*idb;
    xnssb	*xsb;
    int		nentries;
    ulong	thenet;

    xns = (xnshdrtype *)xnsheadstart(pak);

    nentries = (xns->len - XNSHEADERBYTES - sizeof(rip->cmd)) /
	sizeof(xns_ripinfotype);
    idb = pak->if_input;
    if (!idb)
	return;
    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
    if (!xsb)
	return;
    if (xnsrip_debug)
	buginf("\nXNSRIP: got standard update from %u.%e socket %d via %s",
	       GET2WD(xns->ssnet), xns->shost, xns->ssock, idb->namestring);

    /* Are we listening to standard RIP on this interface? */
    if (xns_ub_emulation && !xsb->xns_hear_rip) {
	if (xnsrip_debug)
	    buginf("\n    Update ignored; xns hear-rip not set in UB mode");
	return;
    }

    /* Is this packet from off the local net? (should we check snet here?) */
    if ((xns->tc & XNS_TC_HOP_MASK) != 0) {
	if (xnsrip_debug)
	    buginf("\n    Update ignored; not from local net (packet hops > 0)");
	return;
    }

    /* Do we believe anything from this router? */
    if (!xns_accesscheck(pak, xsb->xns_router_filterlist)) {
	if (xnsrip_debug)
	    buginf("\n    Update ignored; source failed input router-filter");
	return;
    }

    p = rip->nets;
    if (xns->ssock == XNS_SOCK_RIP)
	while (nentries-- > 0) {
	    /* Do we want to hear anything about this destination net ? */
	    thenet = GET2WD(p->nnet);
	    if (xns_filtercheck(thenet, xsb->xns_in_filterlist) &&
		(!xns_ub_emulation ||
		 xns_filtercheck(thenet, xsb->xns_hear_rip_list))) {
		if (p->metric > 0 && thenet != 0L && thenet != -1L)
		    grip_update(&xsb->xnsrip, thenet,
				xns_ub_emulation ?
				   ub_hops_to_delay(p->metric) : p->metric,
				GET2WD(xns->ssnet), xns->shost, xns_table,
				&xns_routes, "XNS", &xns_changed,
				xns_maxpaths, p->metric,
				ub_hops_to_delay(p->metric), xnsrip_debug);
		if (xnsrip_debug)
		    buginf("\n    net %u: %d hops", thenet, p->metric);
	    }
	    p++;
	    (void) process_may_suspend();
	}
}

/*
 * xns_route_adjust
 * Called by the SERVICE_ROUTE_ADJUST function.
 */

void xns_route_adjust (idbtype *swidb)
{
    xnssb	*xsb;

    if (swidb == NULL)
	return;
    xsb = idb_get_swsb_inline(swidb, SWIDB_SB_XNS);
    /*
     * We now have to call grip_route_adjust for both native XNS and UB
     * emulations since the routing metric, idb and lineup state variables
     * are kept in independent grip blocks off the XNS sub-block.
     */
    if (xsb) {
        if (swidb->hwptr->status & IDB_IEEEMASK)
	    ieee_copy(swidb->hwptr->hardware, xsb->xns_node);
	grip_route_adjust(&xsb->xnsrip, xns_table, xsb->xnsnet, &xns_changed, 
			  xns_running, &xns_routes, xnsrip_debug);
	grip_route_adjust(&xsb->ubxnsrip, xns_table, xsb->xnsnet, &xns_changed,
			  xns_running, &xns_routes, xnsrip_debug);
    }
}

/*
 * xns_ubrcvupd
 * Process a UB routing update packet
 */

void
xns_ubrcvupd (paktype *pak, xns_riptype *rip)
{
    xnshdrtype	*xns;
    xns_ripinfotype *p;
    idbtype	*idb;
    xnssb	*xsb;
    int		nentries;
    ulong	thenet;
    ulong	sourcenet;

    xns = (xnshdrtype *)xnsheadstart(pak);

    nentries = (xns->len - XNSHEADERBYTES - sizeof(rip->cmd)) /
	sizeof(xns_ripinfotype);
    sourcenet = GET2WD(xns->ssnet);
    idb = pak->if_input;
    if (!idb)
	return;
    xsb = idb_get_swsb(idb, SWIDB_SB_XNS);
    if (!xsb)
	return;
    if (xnsrip_debug)
	buginf("\nXNSRIP: got UB update from %u.%e socket %d via %s",
	       sourcenet, xns->shost, xns->ssock, idb->namestring);

    /* Is this packet from off the local net? (should we check snet here?) */
    if ((xns->tc & XNS_TC_HOP_MASK) != 0) {
	if (xnsrip_debug)
	    buginf("\n    Update ignored; not from local net (packet hops > 0)");
	return;
    }

    /* Do we believe anything from this router? */
    if (!xns_accesscheck(pak, xsb->xns_router_filterlist)) {
	if (xnsrip_debug)
	    buginf("\n    Update ignored due to \"xns router-filter\" command");
	return;
    }

    p = rip->nets;

    while (nentries-- > 0) {
	/* Do we want to hear anything about this destination net ? */
	thenet = GET2WD(p->nnet);
	if (xns_filtercheck(thenet, xsb->xns_in_filterlist)) {
	    if (p->metric != 0 && thenet != 0L && thenet != -1L)
		grip_update(&xsb->ubxnsrip, thenet, ub_getdelay(p->metric),
			    sourcenet, xns->shost, xns_table, &xns_routes,
			    "XNS", &xns_changed, xns_maxpaths,
			     ub_gethops(p->metric), ub_getdelay(p->metric),
			     xnsrip_debug);
	    if (xnsrip_debug)
		buginf("\n    %u in %d hops, UB delay %d, UB metric 0x%04x",
		       thenet, ub_gethops(p->metric), ub_getdelay(p->metric),
		       p->metric);
	}
	p++;
	(void) process_may_suspend();
    }
}

/*
 * ub_hello
 *
 * Send UB-XNS hello packets.
 */
void
ub_hello (idbtype *idb, xnssb *xsb)
{
    paktype	*pak;
    xnshdrtype	*xns;
    xns_riptype *rip;
    int		nbytes;
    boolean	done;
    idbtype	*interface;
    xnssb	*xsb1;
    xns_ripinfotype *p;
    list_element *l_elt;

    if (!(idb && xsb))
	return;
    if (xsb->xnsnet == 0)
	return;

    done = FALSE;
    l_elt = LIST_HEAD_ELEMENT(idb_get_listhead(SWIDBLIST_XNS));
    interface = LIST_GET_DATA(l_elt);
    while (!done) {
	nbytes = RIP_MAXSIZE;
	pak = getbuffer(nbytes);
	if (!pak)
	    return;
	pak->if_input = NULL;
	nbytes -= (XNSOVERHEADBYTES + sizeof(rip->cmd));
	pak->if_output = idb;
	xns = (xnshdrtype *)xnsheadstart(pak);
	xns->tc = 0;
	xns->pt = XNS_TYPE_UBIPC;
	PUT2WD(xns->ddnet, 0L);
	ieee_copy(baddr, xns->dhost);
	xns->dsock =  XNS_SOCK_UBIPC;
	PUT2WD(xns->ssnet, xsb->xnsnet);
	ieee_copy(xsb->xns_node, xns->shost);
	xns->ssock = XNS_SOCK_UBIPC;
	rip = (xns_riptype *)(xnsheadstart(pak) + XNSOVERHEADBYTES);
	rip->cmd = UB_RIP_HELLO;
	p = rip->nets;
	while (nbytes > 0) {
	    xsb1 = idb_get_swsb(interface, SWIDB_SB_XNS);
	    if (xsb1 && xsb1->xnsnet) {
		PUT2WD(p->nnet, xsb1->xnsnet);
		p->metric = 1;
		nbytes -= sizeof(xns_ripinfotype);
		p++;
	    }
	    (void) process_may_suspend();
	    l_elt = LIST_NEXT_ELEMENT(l_elt);
	    if (l_elt == NULL) {
		done = TRUE;
		break;
	    } else {
		interface = LIST_GET_DATA(l_elt);
	    }
	}
	xns->len = RIP_MAXSIZE - nbytes;
	if (xns->len > XNSOVERHEADBYTES + sizeof(rip->cmd)) {
	    xns->cksum = 0xFFFF;
	    if (xns_send1(pak, baddr))
		if (xnspkt_debug) {
		    buginf("\nXNS: src=%u.%e, dst=%u.%e, packet sent in ub_hello",
			xsb->xnsnet, xsb->xns_node, 0, baddr);
	    }
	} else {
	    if (xnsrip_debug)
		buginf("\nNull UB hello suppressed on %s", idb->namestring);
	    retbuffer(pak);
	}
    }
}

/* Little routines to pack and extract hopcounts and delays into ushorts
 * for UB metrics. The field is byte swapped, and the top 6 bits of the
 * byte-swapped value are used for the hop count, while the lower 10 are
 * the delay.
 */

ushort ub_gethops (ushort metric)
{
    return((ushort)(swap_16bit_word(metric) & 0xfc00) >> 10);
}

ushort ub_getdelay (ushort metric)
{
    return((ushort)swap_16bit_word(metric) & 0x3ff);
}

ushort ub_packmetric (ushort hops, ushort delay)
{
    return((ushort)swap_16bit_word(((hops & 0x3f) << 10) | (delay & 0x3ff)));
}

/* ub_vis_bw_to_delay converts the bandwidth in the IDB visible_bandwidth
 * field to a UB delay value, using UB's conversion table. This isn't strictly
 * portable, but it shouldn't matter on processor with 32-bit or wider longs.
 */
ushort ub_vis_bw_to_delay (ulong bw)
{
    /* Translation tables */
    static ulong thresholds[] = {UB_SPEEDS};
    static ushort delays[] = {UB_DELAYS};

    int tblpos;			/* Position in lookup table */

    if (bw >= 0xffffffffL/1000)		/* Avoid overflows */
	bw = 0xffffffffL;
    else
	bw = 1000 * bw;    		/* Convert to plain BPS */

    /* Scan through the table; we want the first entry that's <= bw. */
    for (tblpos = 0;
	 (thresholds[tblpos] != 0) && (bw < thresholds[tblpos]);
	 tblpos++);

    return (delays[tblpos]);
}
