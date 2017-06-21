/* $Id: igrp.c,v 3.5.22.6 1996/08/28 21:00:50 albright Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/igrp.c,v $
 *------------------------------------------------------------------
 * igrp.c -- Interior Gateway Routing Protocol
 *
 * 3-September-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * Patent Pending.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: igrp.c,v $
 * Revision 3.5.22.6  1996/08/28  21:00:50  albright
 * CSCdi65258:  Timers basic is not working correctly under IGRP
 * Branch: California_branch
 *
 * Revision 3.5.22.5  1996/08/28  12:52:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.22.4  1996/06/18  01:47:15  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.5.22.3  1996/05/04  01:21:33  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.5.22.2  1996/04/15  21:17:54  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.5.22.1  1996/04/02  05:36:58  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.5  1996/01/24  20:16:33  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4  1996/01/22  06:14:14  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/07  22:29:39  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  17:33:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/09/25  08:30:55  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.7  1995/09/23  21:40:34  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Reference renamed fields.  Clean up various interfaces to the IP
 * forwarding table to support protocol-specific functions.
 *
 * Revision 2.6  1995/09/13  07:01:40  hampton
 * All code should use the routine GETSHORT()/PUTSHORT() instead of using
 * the routines getshort()/putshort().  [CSCdi40249]
 *
 * Revision 2.5  1995/08/03 07:42:20  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.4  1995/06/28 09:27:48  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/19  01:07:31  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:06:16  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:06:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "login.h"
#include "../h/sys_registry.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "route.h"
#include "neighbor.h"
#include "config.h"
#include "route_inlines.h"
#include "igrp.h"
#include "../if/network.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../iprouting/msg_igrp2.c"	/* Not a typo, see logger.h */
#include "../iprouting/msg_igrp.c"	/* Not a typo, see logger.h */
#include "route_map.h"
#include "iprouting_debug.h"
#include "../h/address.h"
#include "gdp.h"


/*
 * Forward declarations (ick!)
 */
static void igrp_cleanup(pdbtype *pdb);
static process igrp_router(void);
static void igrp_sendupdate(pdbtype *pdb, ndbtype *ndb, ipaddrtype
			    destination, idbtype *interface, ulong port,
			    ipaddrtype source, ipaddrtype mask);
static boolean igrp_exterior_test(ipaddrtype mask, pdbtype *pdb, ndbtype *sdb);
static boolean igrp_noexterior_subnet_test(ipaddrtype mask, pdbtype *pdb,
				      ndbtype *sdb); 
static uint igrp_setup_metric(pdbtype *pdb, char *desc, ndbtype *ndb, idbtype
			 *interface, metrictype *ptr, ipaddrtype
			 destination, boolean debug);
static void igrp_rcvupdate(pdbtype *pdb, ndbtype *ndb, paktype *pak,
			   igrptype *igrp); 
static ulong igrp_compute_metric(metrictype *ptr, idbtype *interface, ulong
			    network, pdbtype *pdb, ulong *neighbor); 
static boolean igrp_holddown(pdbtype *pdb, ndbtype *ndb, rdbtype *rdb,
			     ulong newmetric); 
static void igrp_sendrequest(pdbtype *pdb, idbtype *sidb);
static void igrp_rcvrequest(pdbtype *pdb, ndbtype *ndb, paktype *pak);
static void igrp_rdb_update(rdbtype *rdb, metrictype *ptr,
                            idbtype *interface, ulong nexthop_metric);
static void igrp_enqueue(paktype *pak);
static ipsocktype *find_igrpsocket(ushort asystem);
static void igrp_metricdebug(char *string, ipaddrtype network, long metric,
			     long metric2);
static void igrp_show_traffic(void);
static void igrp_showipproto(pdbtype *pdb);
static enum DISTTYPE igrp_distcmp(pdbtype *, uchar, ndbtype *, ulong, ushort);

/*
 * External declarations
 */
extern ipaddrtype igrp_debug_target;

/*
 * Local storage
 */
static igrp_traffic_t igrp_traffic;


/*
 * igrp_cleanup
 * Here to cleanup after an IGRP router has been shutdown.
 * Close socket and release pdb storage.
 */

static void
igrp_cleanup (pdbtype *pdb)
{
    reg_invoke_ip_close_socket((ipsocktype *)pdb->socket);
    unqueue(&protoQ, pdb);
    free_pdbindex(pdb);
    free(pdb);
}

/*
 * igrp_enqueue
 * Here we figure out which IGRP socket gets the datagram.
 */
static void
igrp_enqueue (paktype *pak)
{
    iphdrtype *ip;
    ipsocktype *soc;
    igrptype *igrp;

    ip = (iphdrtype *)ipheadstart(pak);

    igrp_traffic.inputs++;
    igrp = (igrptype *)ipdatastart(pak);
    soc = (ipsocktype *) find_igrpsocket(igrp->asystem);
    if (soc && (soc->inallocated != soc->inallowed)) {
	soc->inallocated++;
	enqueue_ip_socket(soc, pak);
    } else {
	reg_invoke_ip_gdpclient_enqueue(pak);
    }
}

/*
 * find_igrpsocket
 * Given an IP packet, return the IGRP socket or NULL
 */

static ipsocktype *
find_igrpsocket (register ushort asystem)
{
    register ipsocktype *soc;

    for (soc = (ipsocktype *) IPsocketQ.qhead; soc; soc = soc->next) {
	if ((soc->protocol == IGRP_PROT) && (asystem == soc->lport))
	    return(soc);
    }
    return(NULL);
}

/*
 * igrp_process_packets
 *
 * Process all incoming IGRP packets from the queue.
 */
static void igrp_process_packets (pdbtype *pdb, ipsocktype *soc)
{
    paktype *pak;
    iphdrtype *ip;
    ndbtype *ndb;
    igrptype *igrp;
    int dummy;
    ipaddrtype address;

    /*
     * First empty the IGRP packet queue
     */
    while ((pak = read_ipsocket(soc,FALSE,0,&igrp,&dummy))) {

	ip = (iphdrtype *)ipheadstart(pak);

	/*
	 * Checksum datagram
	 */
	igrp_traffic.inputs++;
	if (ipcrc((ushort *)igrp,(ip->tl - (ip->ihl << 2)))) {
	    if ((igrp_debug || igrp_debug_events)
		&& (igrp_debug_addr == 0
		    || igrp_debug_addr == ip->srcadr)) 
		buginf("\nIGRP: bad checksum from %i", ip->srcadr);
	    IP_DISPOSE(igrp, checksumerr, pak);
	    continue;
	}

	/*
	 * Validate autonomous system number
	 */
	if (igrp->asystem != pdb->asystem) {
	    if ((igrp_debug || igrp_debug_events)
		&& (igrp_debug_addr == 0
		    || igrp_debug_addr == ip->srcadr)) 
		buginf("\nIGRP: system %d from %i, should be system %d",
		       igrp->asystem, ip->srcadr, pdb->asystem);
	    retbuffer(pak);
	    continue;
	}

	/*
	 * Check version number.  It is here that we should transmogrify
	 * packets from one version to another.
	 */
	if (igrp->version != IGRP_VERSION) {
	    retbuffer(pak);
	    continue;
	}

	/*
	 * Switch on packet type.  Routines must check version number.
	 */

	if (!pak->if_input->static_routing) {
	    address = pak->if_input->ip_address;
	    if (!address)
		address = pak->if_input->ip_unnumbered->ip_address;
	    ndb = (ndbtype *) net_lookup(address, FALSE);
	    switch (igrp->opcode) {
	      case IGRP_OPC_REQUEST:
		igrp_rcvrequest(pdb, ndb, pak);
		break;
	      case IGRP_OPC_UPDATE:
		igrp_rcvupdate(pdb, ndb, pak, igrp);
		break;
	      default:
		break;
	    }
	} else
	    if (igrp_debug || igrp_debug_events)
		buginf("\nIGRP: Input packet while routing disabled on %s",
		       pak->if_input->namestring);

	/*
	 * Dispose of processed datagram.
	 */
	retbuffer(pak);
	process_may_suspend();
    }
}

/*
 * igrp_router
 * Main IGRP routing process.  One per major network.
 */

static process
igrp_router (void)
{
    ipsocktype *soc;
    boolean sendupdate;
    pdbtype *pdb;
    ulong major, minor;

    if (!process_get_arg_ptr((void**)&pdb))
	process_kill(THIS_PROCESS);
    memory_new_owner(pdb);
    process_wait_on_system_init();
    soc = find_igrpsocket(pdb->asystem);
    if (soc)				/* if socket, already routing AS */
	pdb_killp(pdb, -1);
    soc = open_ipsocket(IGRP_PROT, NULL, NULL, pdb->asystem, 0, 0);
    if (!soc) {
	errmsg(&msgsym(NOSOCKET, IGRP), pdb->asystem);
	pdb_killp(pdb, -1);
    }
    sendupdate = TRUE;
    pdb->socket = soc;

    TIMER_START(pdb->timer, 10*ONESEC);
    igrp_sendrequest(pdb, NULL);

    process_watch_queue(soc->inq, ENABLE, RECURRING);
    while (TRUE) {
	/*
         * If something changed in the last pass, send a major update now.
  	 */
	if (!TIMER_RUNNING_AND_SLEEPING(pdb->sleep_timer)) {
	    if (sendupdate || pdb->changed) {
		pdb_majorupdate(pdb);
		sendupdate = FALSE;
	    }
	    pdb->changed = 0;
	}

	/*
	 * Block until IGRP input or timer expires.  We don't want to
	 * wake up on pdb->timer unless pdb->sleep_timer is already
	 * awake.
	 */
	if (SLEEPING(pdb->sleep_timer)) {
	    process_watch_timer(&pdb->sleep_timer, ENABLE);
	} else {
	    process_watch_timer(&pdb->timer, ENABLE);
	}
	process_wait_for_event();

	/*
	 * Do whatever needs to be done.
	 */
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		igrp_process_packets(pdb, soc);
		break;

	      case TIMER_EVENT:
		/*
		 * If the update timer has expired, age entries appropriately and
		 * scan network table to determine the current closest exterior
		 * gateway.  Set "sendupdate" to force a periodic update even if
		 * nothing has changed.  Check if maybe we should suspend ourself.
		 *
		 * Do nothing if the sleep timer expired.  That timer just
		 * provides hysteresis.
		 */
		if (!TIMER_RUNNING_AND_SLEEPING(pdb->timer)) {
		    age_all_routes(pdb);
		    sendupdate = TRUE;
		    TIMER_START_JITTERED(pdb->timer, pdb->broadcasttime, IGRP_JITTER);
		    process_may_suspend();
		}
		break;

	      case DIRECT_EVENT:
		/*
		 * Currently, the only direct event is the result a topology
		 * change signal.  Ignore it for the nonce, and we'll catch it
		 * after all other wakeup events have been processed.
		 */
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}

	/*
	 * After processing the input data and doing any timeouts,
	 * see if our copy of the routing table has changed any.
	 * If so, bump the edition number.
	 * With the addition of sleep timer to the pdb the edition number
	 * could get bumped more than once while we are waiting for
	 * sleep timer to expire.  This seems harmless enough.
	 */
	if (pdb->changed) {
	    pdb->edition++;
	    if (igrp_debug || igrp_debug_events)
		buginf("\nIGRP: edition is now %d", (uint) pdb->edition);
	}
    }
}

/*
 * igrp_noexterior_test
 * Simply negation of igrp_exterior_test.
 */

static inline boolean
igrp_noexterior_test (ipaddrtype mask, pdbtype *pdb, ndbtype *sdb)
{
    return(!igrp_exterior_test(mask, pdb, sdb));
}

/*
 * igrp_sendupdate
 * Send an IGRP update.
 */

static void
igrp_sendupdate (pdbtype *pdb, ndbtype *ndb, ipaddrtype destination,
		 idbtype *interface, ulong port, ipaddrtype source,
		 ipaddrtype mask)
{
    paktype *pak;
    igrptype *igrp;
    ndbtype *net;
    metrictype *ptr;
    int prev, nbytes;
    boolean done, exteriordone, interiordone, systemdone, debug, first;
    uchar edition;
    ipaddrtype sourcemask, primary;
    ipaddtype *secptr;
    int subnet_route_count, network_route_count, exterior_route_count;
    pdbtype *src_pdb;

    if (!pdb->running)
	return;

    prev = 0;
    net = NULL;
    done = FALSE;
    edition = pdb->edition;
    systemdone = FALSE;
    exteriordone = FALSE;
    interiordone = TRUE;
    first = TRUE;

    /*
     * If the interface is not to receive updates because of a snapshot
     * configuration, don't bother to send any further updates.
     */
    if (reg_invoke_snapshot_update_denied(interface, FALSE))
	return;

    /*
     * If unnumbered interface, use pointer interface for
     * source address.
     */
    if (interface->ip_unnumbered) {
	primary = interface->ip_unnumbered->ip_address;
	sourcemask = interface->ip_unnumbered->ip_nets_mask;
    } else {
	primary = interface->ip_address;
	sourcemask = interface->ip_nets_mask;
    }
    if (!source) {
	source = primary;
	mask = sourcemask;
    }

    /*
     * If we have an ndb, we may be sending subnet information.
     * IGRP unfortunately sends the last three octets of a subnet
     * address, assuming the full four octets can be reconstructed
     * using the first octet of the datagram's source address.
     * If we have a subnetted ndb that does not correspond to the
     * primary address of the outgoing interface, search the secondary
     * list for an appropriate source address.  If we can't find a good
     * source address, suppress the update, otherwise send out just the
     * subnet information for that network.  Don't forget about un-
     * numbered serial interfaces.
     */
    if (ndb) {
	if (ndb->attrflags & NET_SUBNETTED)
	    interiordone = FALSE;

  	if (!interface->ip_split_disabled &&
 	    (get_majornet(primary) != ndb->netnumber)) {
  	    for (secptr = (ipaddtype *) interface->ip_addressQ.qhead; secptr;
  		 secptr = secptr->next) {
  		if (get_majornet(secptr->address) == ndb->netnumber) {
  		    if (!interface->ip_split_disabled) {
  			systemdone = TRUE;
  			exteriordone = TRUE;
			break;
		    }
		}
	    }
	    if (!secptr)
		return;
	}
    }
    debug = ((igrp_debug || igrp_debug_events)
	     && (igrp_debug_addr == 0
		 || igrp_debug_addr == destination
		 || (igrp_debug_addr & mask) == (source & mask)));

    if (interiordone && systemdone && exteriordone) {
	if (reg_invoke_snapshot_enabled(interface)) {
	    /* Snapshot. Send out a null packet */
	    pak = getbuffer(IPHEADERBYTES(NOPT) + IGRPHEADERBYTES); 
	    if (!pak)
	      goto done;
	    pak->if_output = interface;
	    igrp = (igrptype *)(ipheadstart(pak) + IPHEADERBYTES(NOPT));
	    igrp->ninterior = igrp->nsystem = igrp->nexterior =
	      igrp->checksum = 0;
	    igrp->version = IGRP_VERSION;
	    igrp->opcode = IGRP_OPC_UPDATE;
	    igrp->edition = edition;
	    igrp->asystem = pdb->asystem;
	    igrp->checksum = ipcrc((ushort *)igrp, IGRPHEADERBYTES);
	    if (debug) 
	      buginf("\nIGRP: sending NULL update (for Snapshot) to %i via %s (%i)",
		     (((long)destination == -1L) ? interface->ip_broadcast :
		      destination), interface->namestring, source);
	    reg_invoke_snapshot_notice_update_pak(pak, interface);
	    ipwrite(pak, (IGRPHEADERBYTES + IPHEADERBYTES(NOPT)), IGRP_PROT,
			  source, destination, interface, NULL,
			  &iprouteparams);
	}
	return;
    }
    if (debug)
	buginf("\nIGRP: sending update to %i via %s (%i)",
	       (((long)destination == -1L) ? interface->ip_broadcast :
		destination), interface->namestring, source);


    /*
     * Don't lose the ndb or pdb if we suspend.
     */
    if (ndb)
	mem_lock(ndb);
    mem_lock(pdb);

    while (!done) {
	nbytes = MAXIGRPBYTES;
	pak = getbuffer(IPHEADERBYTES(NOPT) + IGRPHEADERBYTES + nbytes); 
	if (!pak)
	    goto done;
	pak->if_output = interface;
	igrp = (igrptype *)(ipheadstart(pak) + IPHEADERBYTES(NOPT));
	igrp->ninterior = 0;
	igrp->nsystem = 0;
	igrp->nexterior = 0;
	subnet_route_count = 0; 
	network_route_count = 0; 
	exterior_route_count = 0;
	ptr = (metrictype *)((char *)igrp + IGRPHEADERBYTES);

	/*
	 * Run over the interior routes
	 */
	while (nbytes && !interiordone) {
	    net = (ndbtype *) getnextnetwork(pdb, ndb, net, &prev,
		interface, mask, destination, igrp_noexterior_subnet_test,
		TRUE, TRUE, &src_pdb);
	    if (!net) {
		interiordone = TRUE;
		prev = 0;
		if (systemdone && exteriordone)
		    done = TRUE;
		break;
	    }
	    (void) igrp_setup_metric(pdb, "subnet", net, interface, ptr,
				     destination, (debug && igrp_debug));
	    if (igrp_debug_events)
	        subnet_route_count++;
	    bcopy(((uchar *)&net->netnumber) + 1, ptr->number, 3);
	    nbytes -= METRICSIZE;
	    ptr = (metrictype *) ((char *) ptr + METRICSIZE);
	    igrp->ninterior++;
	}

	/*
	 * Suspend here to let process switching run.
	 * If we lose our place, just punt until the next update.
	 */
	if (net) {
	    mem_lock(net);
	    process_may_suspend();
	    if (mem_refcount(net) == 1) {
		ndb_free(net, FALSE);
		interiordone = TRUE;
		net = NULL;
		prev = 0;
	    } else
		ndb_free(net, FALSE);
	} else
	    process_may_suspend();

	/*
	 * Run over the autonomous system routes
	 */
	while (nbytes && !systemdone) {
	    net = (ndbtype *) getnextnetwork(pdb, ndb, net, &prev,
		interface, mask, destination, igrp_noexterior_test, FALSE,
		TRUE, &src_pdb);
	    if (!net) {
		systemdone = TRUE;
		prev = 0;
		break;
	    }
	    (void) igrp_setup_metric(pdb, "network", net, interface, ptr,
				     destination, (debug && igrp_debug));
	    if (igrp_debug_events)
	 	network_route_count++;
	    bcopy((uchar *)&net->netnumber, ptr->number, 3);
	    nbytes -= METRICSIZE;
	    ptr = (metrictype *) ((char *) ptr + METRICSIZE);
 	    igrp->nsystem++;
	}

	/*
	 * Suspend here to let process switching run.
	 * If we lose our place, just punt until the next update.
	 */
	if (net) {
	    mem_lock(net);
	    process_may_suspend();
	    if (mem_refcount(net) == 1) {
		ndb_free(net, FALSE);
		systemdone = TRUE;
		net = NULL;
		prev = 0;
	    } else
		ndb_free(net, FALSE);
	} else
	    process_may_suspend();

	/*
	 * Run over the exterior system routes
	 */
	while (nbytes && !exteriordone) {
	    pdb->route_map = NULL;
	    net = (ndbtype *) getnextnetwork(pdb, ndb, net, &prev,
		interface, mask, destination, igrp_exterior_test, FALSE,
		TRUE, &src_pdb);
	    if (!net) {
		exteriordone = TRUE;
		done = TRUE;
		break;
	    }
	    (void) igrp_setup_metric(pdb, "exterior", net, interface, ptr,
				     destination, (debug && igrp_debug));
            if (igrp_debug_events)
                exterior_route_count++;
	    bcopy((uchar *)&net->netnumber, ptr->number, 3);
	    nbytes -= METRICSIZE;
	    ptr = (metrictype *) ((char *) ptr + METRICSIZE);
 	    igrp->nexterior++;
	}
        if (debug && igrp_debug_events) {
	    buginf("\nIGRP: Update contains %d interior, %d system, "
		   "and %d exterior routes.", subnet_route_count,
		   network_route_count, exterior_route_count);
	    buginf("\nIGRP: Total routes in update: %d",
		   subnet_route_count+network_route_count+
		   exterior_route_count); 
	}
	/*
	 * Suppress null messages except on snapshot interfaces
	 */
	if (nbytes == MAXIGRPBYTES) {
	    if (!reg_invoke_snapshot_enabled(interface)) {
		if (debug && first)
		    buginf(" - suppressing null update");
		retbuffer(pak);
		goto done;
	    } else {
		done = TRUE;
	    }
	}

	/*
	 * Finish up the datagram and ship it.
	 */
	igrp->version = IGRP_VERSION;
	igrp->opcode = IGRP_OPC_UPDATE;
	igrp->edition = edition;
	igrp->asystem = pdb->asystem;
	igrp->checksum = 0;
	nbytes = IGRPHEADERBYTES + MAXIGRPBYTES - nbytes;
        igrp->checksum = ipcrc((ushort *)igrp,nbytes);
	nbytes += IPHEADERBYTES(NOPT);
	reg_invoke_snapshot_notice_update_pak(pak, interface);
	ipwrite(pak,nbytes,IGRP_PROT,source,
	    destination,interface,NULL,&iprouteparams);
	first = FALSE;
    }
 done:
    TIMER_START(pdb->sleep_timer, pdb->sleeptime);
    free(pdb);
    ndb_free(ndb, FALSE);
}

/*
 * igrp_exterior_out_allowed
 * Given a route which has been determined to be exterior in the routing
 * table, decide if we should advertise it as exterior or not.  Return
 * TRUE if route should be advertised as exterior, FALSE otherwise.
 */

static inline boolean
igrp_exterior_out_allowed (pdbtype *pdb, ndbtype *sdb)
{
    /*
     * Exteriors unconditionally disallowed
     */
    if (!pdb->exteriors_out)
	return(FALSE);
    /*
     * Exteriors unconditionally allowed
     */
    if (!pdb->exteriors_out_acl)
	return(TRUE);
    /*
     * Exteriors allowed if they match access list, otherwise disallowed
     */
    return(nfast_check(pdb->exteriors_out_acl, sdb->netnumber));
}

/*
 * igrp_exterior_test
 * See if we want to include this network in the IGRP exterior list
 * The network must be flagged as exterior and it must be a network
 * we learned about using this IGRP process or a network that is
 * directly connected that we are routing.
 * Note that the network may be flagged exterior either in the dummy ndb or
 * in the majornet route hung off the dummy ndb.
 */

static boolean
igrp_exterior_test (ipaddrtype mask, pdbtype *pdb, ndbtype *sdb)
{
    if (sdb->attrflags & NET_EXTERIOR)
	return(igrp_exterior_out_allowed(pdb, sdb));
    if (sdb->attrflags & NET_SUBNETTED) {
	sdb = subnet_lookup(sdb, sdb->netnumber, sdb->netsmask);
	if (sdb && (sdb->attrflags & NET_EXTERIOR))
	    return(igrp_exterior_out_allowed(pdb, sdb));
    }
    /*
     * Route is not exterior
     */
    return(FALSE);
}

/*
 * igrp_noexterior_subnet_test
 * Return TRUE if route is subnet of interface out which update is being
 * sent, and shouldn't be advertised as exterior.
 */

static boolean
igrp_noexterior_subnet_test (ipaddrtype mask, pdbtype *pdb, ndbtype *sdb)
{
    if (sdb->netsmask != mask)
	return(FALSE);
    if (sdb->attrflags & NET_EXTERIOR)
	return(!igrp_exterior_out_allowed(pdb, sdb));
    /*
     * Route is not exterior
     */
    return(TRUE);
}


/*
 * igrp_setup_metric
 * Given network and interface descriptors, setup the IGRP path data fields.
 * that we are going to advertise.  Split horizon checking has already
 * been done, i.e. none of the rdb's match the outgoing interface.
 */

static uint
igrp_setup_metric (pdbtype *pdb, char *desc, ndbtype *ndb, idbtype *interface,
		   metrictype *ptr, ipaddrtype dest, boolean debug)
{
    register int i;
    register rdbtype *rdb, *bestrdb;
    register idbtype *idb;
    idbtype *best;
    ulong temp, bestmetric;
    pdbtype *dist;
    ipaddrtype dummy;
    ulong dummy_neighbor;
    boolean match, bestset, subnetted;
    ipaddtype *sec;
    route_maptype *map;
    boolean map_sets_metric;
    ndbtype *sdb;
    struct vector def_metric;
    pdbtype *src_pdb;
    int offset;

    /*
     * First find the idb whose bandwidth and delay info we will use.
     * If this is a directly connected network,
     * find the first interface to that network that is up.
     * For other networks, search the route descriptor list for an interface.
     */
    best = NULL;
    rdb = NULL;
    bestrdb = NULL;
    bestmetric = METRIC_INACCESSIBLE;
    bestset = FALSE;
    dist = INDEX2PDB(ndb->pdbindex);
    subnetted = FALSE;
    if (ndb->attrflags & NET_SUBNETTED) {
	subnetted = TRUE;
	FOR_ALL_SWIDBS(idb) {

	    /*
	     * Check interfaces to find ones that are directly connected to
	     * the major net.
	     */
	    match = FALSE;
	    if (!idb->ip_enabled)
		continue;
	    if (idb->ip_majornet == ndb->netnumber)
		match = TRUE;
	    else {
		for (sec = (ipaddtype *) idb->ip_addressQ.qhead; sec;
		     sec = sec->next) {
		    if (get_majornet(sec->address) == ndb->netnumber) {
			match = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * If the interface is a contender for that network, then
	     * calculate its metric and see if this is the best interface
	     * that we have to that major net.
	     */
	    if (match) {
		if (!interface_up(idb))
		    temp = (unsigned) -1;
		else
		    temp = (idb->delay << 8);
		bcopy((uchar *)&temp, ptr->delay, 3);
		temp = SCALED_BANDWIDTH(idb->visible_bandwidth) << 8;
		bcopy((uchar *)&temp, ptr->bandwidth, 3);
		ptr->load = idb->hwptr->load;
		ptr->reliability = idb->hwptr->reliability;
		ptr->hopcount = 0;
		PUTSHORT(ptr->mtu,idb->ip_mtu);
		temp = igrp_compute_metric(ptr, NULL, 0, pdb, &dummy_neighbor);
		if (temp < bestmetric) {
		    best = idb;
		    bestmetric = temp;
		    bestset = TRUE;
		}
	    }	
	}
    } else {
	for (i = 0, idb = NULL; i < ndb->routecount; i++) {
	    rdb = ndb->rdb[i];
	    idb = rdb ? rdb->idb : NULL;
	    if (idb && (rdb->metric <= bestmetric)) {
		best = idb;
		bestmetric = rdb->metric;
		bestrdb = rdb;
		if (interface_up(idb) && rdb->metric == ndb->metric)
		    break;
	    }
	}
	if (!best) {
	    dummy = ndb->netnumber;
	    best = route_lookup(ndb, &dummy, dummy, ROUTINGDEPTH,
			        IP_PATH_USE_IF_GUESS, NULL);
	}
    }

    idb = best;

    /*
     * If the route to that network has vanished or if the interface
     * we would use is not up, say network is unreachable.
     * Otherwise, compute a metric based on type of network entry
     */
    if ((!subnetted && (ndb->metric == METRIC_INACCESSIBLE)) ||
	!idb || !interface_up(idb)) {
	temp = (unsigned) -1;
	bcopy((uchar *)&temp, ptr->delay, 3);
	bestset = FALSE;
    } else if (!subnetted && (NDB2PROCTYPE(ndb) & (PROC_IGRP | PROC_IGRP2))) {
	if ((bestrdb == NULL) || (bestrdb->hopcount > pdb->igrp_maxhops)) {
	    temp = (unsigned) -1;
	    bcopy((uchar *)&temp, ptr->delay, 3);
	    bestset = FALSE;
	} else

	    /*
	     * Look at the rdb for the pdbindex instead of the ndb.  This
	     * is done because we could have more than a single source 
	     * of information in the rdb's.  We don't want to confuse the
	     * metrics.
	     */
	    if (INDEX2PDB(bestrdb->pdbindex)->proctype & PROC_IGRP2) {
		/*
		 * Lose the least significant byte of the EIGRP bandwidth
		 * and delay.
		 */
		bcopy((uchar *)&bestrdb->rdb_delay, ptr->delay, 3);
		bcopy((uchar *)&bestrdb->rdb_bandwidth, ptr->bandwidth, 3);

		/*
		 * This probably shouldn't be a hard coded constant
		 */
		if (bestrdb->rdb_mtu > IGRP_MAXMTU)
		    PUTSHORT(ptr->mtu, IGRP_MAXMTU);
		else
		    PUTSHORT(ptr->mtu, bestrdb->rdb_mtu);
		ptr->reliability = bestrdb->reliability;
		ptr->load = bestrdb->load;
		ptr->hopcount = bestrdb->hopcount + 1;
		bestset = FALSE;
	    } else {

		/*
		 * The igrp1 case
		 */
		temp = (bestrdb->rdb_delay << 8);
		bcopy((uchar *)&temp, ptr->delay, 3);
		temp = (bestrdb->rdb_bandwidth << 8);
		bcopy((uchar *)&temp, ptr->bandwidth, 3);
		PUTSHORT(ptr->mtu, bestrdb->rdb_mtu);
		ptr->reliability = bestrdb->reliability;
		ptr->load = bestrdb->load;
		ptr->hopcount = bestrdb->hopcount + 1;
	    }
    } else {
	map = pdb->route_map;
	map_sets_metric = map &&
	    (map->set_list.set_flags & ROUTE_MAP_METRIC_SET) &&
		map->set_list.metric.igrp_metric;
	if (!ip_def_metric_configured(pdb, dist->index) && !map_sets_metric) {
	    if (ndb->attrflags & NET_ATTACHED) {
		temp = (idb->delay << 8);
		bcopy((uchar *)&temp, ptr->delay, 3);
		temp = SCALED_BANDWIDTH(idb->visible_bandwidth) << 8;
		bcopy((uchar *)&temp, ptr->bandwidth, 3);
		ptr->load = idb->hwptr->load;
		ptr->reliability = idb->hwptr->reliability;
		ptr->hopcount = 0;
		PUTSHORT(ptr->mtu,idb->ip_mtu);
		bestset = FALSE;
	    } else if (NDB2PROCTYPE(ndb) & PROC_STATIC) {
		temp = (idb->delay << 8);
		bcopy((uchar *)&temp, ptr->delay, 3);
		temp = SCALED_BANDWIDTH(idb->visible_bandwidth) << 8;
		bcopy((uchar *)&temp, ptr->bandwidth, 3);
		ptr->load = 1;
		ptr->reliability = 255;
		ptr->hopcount = 0;
		PUTSHORT(ptr->mtu,0);
		bestset = FALSE;
	    } else {
		temp = (unsigned) -1;
		bcopy((uchar *)&temp, ptr->delay, 3);
		bestset = FALSE;
	    }
	} else {
	    if (!bestset && ip_def_metric_configured(pdb, dist->index)) {
		ip_get_def_metric_vector(pdb, dist->index, &def_metric);
		temp = (def_metric.delay << 8);
		bcopy((uchar *)&temp, ptr->delay, 3);
		temp = (def_metric.bandwidth << 8);
		bcopy((uchar *)&temp, ptr->bandwidth, 3);
		ptr->load = def_metric.loading;
		ptr->reliability = def_metric.reliability;
		ptr->hopcount = 0;
		PUTSHORT(ptr->mtu, def_metric.mtu);
	    }
	    if (map_sets_metric) {
		temp = (map->set_list.metric.value.vector.delay << 8);
		bcopy((uchar *)&temp, ptr->delay, 3);
		temp = (map->set_list.metric.value.vector.bandwidth << 8);
		bcopy((uchar *)&temp, ptr->bandwidth, 3);
		ptr->load = map->set_list.metric.value.vector.loading;
		ptr->reliability = 
		    map->set_list.metric.value.vector.reliability;
		ptr->hopcount = 0;
		PUTSHORT(ptr->mtu, map->set_list.metric.value.vector.mtu);
		bestset = FALSE;
	    }
	}
    }

    /*
     * Do metric munging on output
     */
    offset = offset_metric(pdb, interface, ndb->netnumber, OUT);
    if (offset) {
	bcopy(ptr->delay, (uchar *)&temp, 3);
	temp >>= 8;
	temp += offset;
	if (temp >= IGRP_METRIC_INACCESSIBLE)
	    temp = (unsigned) -1;
	temp <<= 8;
	bcopy((uchar *)&temp, ptr->delay, 3);
	bestset = FALSE;
    }

    /*
     * If we've already calculated the best metric, don't recalculate it.
     */
    if (!bestset)
	bestmetric = igrp_compute_metric(ptr, NULL, 0, pdb, &dummy_neighbor);

    /*
     * If we're looking at a major ndb, which is NOT redistributed into
     * IGRP, and it's variably subnetted and we learned a summary route via
     * another routing protocol, we would generate an infinite metric.  In
     * this case, find the summary route and recurse.
     */
    if ((bestmetric == METRIC_INACCESSIBLE) &&
	(ndb->attrflags & NET_SUBNETTED) &&
	(ndb->attrflags & NET_VARIABLE)) {
	sdb = subnet_lookup(ndb, ndb->netnumber,
			    get_majormask(ndb->netnumber));
	if (sdb) 
	    bestmetric = igrp_setup_metric(pdb, desc, sdb, interface, ptr, dest,
					   FALSE);
    }
	
    /*
     * If we're looking at a dummy ndb and still don't have something, 
     * we're in a world of hurt.  Walk all subnets looking for
     * something, anything that would give us a good metric.
     */
    if ((bestmetric == METRIC_INACCESSIBLE) &&
	(ndb->attrflags & NET_SUBNETTED)) {
	for (i = 0; i < NETHASHLEN; i++) {
	    for (sdb = ndb->subnettable[i]; sdb; sdb = sdb->next) {
		if (!validnextnetwork(pdb, ndb, sdb, interface, 0, dest,
				      NULL, FALSE, &src_pdb))
		    continue;
		bestmetric = igrp_setup_metric(pdb, desc, sdb, interface, ptr,
					       dest, FALSE); 
		if (bestmetric != METRIC_INACCESSIBLE)
		    break;
	    }
	}
    }

    /*
     * Now say what we're sending
     */
    if (debug && ((igrp_debug_target == 0) ||
		  (igrp_debug_target == ndb->netnumber)))
	buginf("\n      %s %i, metric=%d", desc,
	       ndb->netnumber, bestmetric);
    return(bestmetric);
}

/*
 * igrp_validate_network
 *
 * Validate the network associated with the received interface to see
 * if pdb is configured to run over it.
 */

static boolean igrp_validate_network (pdbtype *pdb, ndbtype *ndb,
				      paktype *pak)
{
    ipaddtype *secptr;
    boolean valid;
    
    valid = FALSE;
    if (ndb && (ndb->pdb_overmask & pdb->mask)) {
        valid = TRUE;
    } else {
        for (secptr = (ipaddtype *) pak->if_input->ip_addressQ.qhead;
             secptr; secptr = secptr->next) {
            if (ip_pdb_net_advertise(pdb, secptr->address, &ndb)) {
                valid = TRUE;
                break;
            }
        }
    }
    return (valid);
}

/*
 * igrp_rcvupdate
 * Receive an IGRP update.
 */

static void
igrp_rcvupdate (pdbtype *pdb, ndbtype *ndb, paktype *pak, igrptype *igrp)
{
    iphdrtype *ip;
    int i, limit;
    metrictype *ptr;
    rdbtype *rdb;
    ipaddrtype source, network;
    ulong metric;
    idbtype *interface;
    
    int subnet_route_count, network_route_count, exterior_route_count;
    long flags;

    ip = (iphdrtype *)ipheadstart(pak);

    ptr = (metrictype *) ((char *)igrp + IGRPHEADERBYTES);
    interface = pak->if_input;
    source = ip->srcadr;

    /*
     * If split horizon is disabled, or update source validation is enabled,
     * check to make sure that the source address is on one of our nets.
     */
    if (interface->ip_split_disabled || pdb->validate_source) {
	if (!ip_onifsubnet(interface, source)) {
	    if ((igrp_debug || igrp_debug_events) &&
		(igrp_debug_addr == 0 || igrp_debug_addr == source))
		buginf("\nIGRP: received update from invalid source %i on %s",
		       source, interface->namestring);
	    return;
	}
    }
    if (!igrp_validate_network(pdb, ndb, pak))
	return;
 
    if ((igrp_debug || igrp_debug_events)
	&& (igrp_debug_addr == 0
	    || igrp_debug_addr == ip->srcadr)) 
	buginf("\nIGRP: received update from %i on %s", source,
	       pak->if_input->namestring);
    
    
    reg_invoke_snapshot_update_received(pak->if_input, (paktype *) pak);
    if (!pdb->exteriors_in) {
        igrp->nsystem += igrp->nexterior;
	igrp->nexterior = 0;
    }
    subnet_route_count = 0;
    network_route_count = 0;
    exterior_route_count = 0;

    /*
     * Process interior routes.
     */
    for (i = 0, limit = igrp->ninterior; i < limit; i++) {
	network = source;
	bcopy(ptr->number,(((uchar *) &network) + 1), 3);
	metric = igrp_compute_metric(ptr, interface, network, pdb, &pdb->arg1);
	if ((igrp_debug) 
	    && (igrp_debug_addr == 0
		|| igrp_debug_addr == ip->srcadr)
	    && (igrp_debug_target == 0
		|| igrp_debug_target == network)) 
	    igrp_metricdebug("subnet", network, metric, pdb->arg1);
	if (igrp_debug_events)
	    subnet_route_count++;
	pdb->igrp_hopcount = ptr->hopcount;
	/*
	 * The following line is because pre-8.0 software would occasionally
	 * send out a hop count of 255 rather than 0.  Remove when we feel
	 * there are no more pre-8.0 systems in the field.  Note we do this
	 * for interior, system, and exterior routes.
	 */
	if (pdb->igrp_hopcount == 255) {
	    pdb->igrp_hopcount = 0;
	    ptr->hopcount = 0;
        }
	rdb = network_update(pdb, network, IPADDR_ZERO, source,
			     interface, metric, NET_INTERIOR, source, 0,
			     NULL, 0);
	igrp_rdb_update(rdb,ptr,interface,pdb->arg1);
	ptr = (metrictype *) ((char *) ptr + METRICSIZE);
	(void) process_may_suspend();
    }

    /*
     * Process system routes.
     */
    for (i = 0, limit = igrp->nsystem; i < limit; i++) {
	network = 0;
	bcopy(ptr->number, (uchar *)&network, 3);
	metric = igrp_compute_metric(ptr, interface, network, pdb, &pdb->arg1);
	if ((igrp_debug)
	    && (igrp_debug_addr == 0
		|| igrp_debug_addr == ip->srcadr)
	    && (igrp_debug_target == 0
		|| igrp_debug_target == network)) 
	    igrp_metricdebug("network", network, metric, pdb->arg1);
	if (igrp_debug_events)
	    network_route_count++;
	pdb->igrp_hopcount = ptr->hopcount;
	if (pdb->igrp_hopcount == 255) {
	    pdb->igrp_hopcount = 0;
	    ptr->hopcount = 0;
        }
	rdb = network_update(pdb, network, IPADDR_ZERO, source,
			     interface, metric, NET_SYSTEM, source, 0,
			     NULL, 0);
	igrp_rdb_update(rdb,ptr,interface,pdb->arg1);
	ptr = (metrictype *) ((char *) ptr + METRICSIZE);
	(void) process_may_suspend();
    }

    /*
     * Process exterior routes.
     */
    for (i = 0, limit = igrp->nexterior; i < limit; i++) {
	network = 0;
	bcopy(ptr->number, (uchar *)&network, 3);
	metric = igrp_compute_metric(ptr, interface, network, pdb, &pdb->arg1);
	if ((igrp_debug)
	    && ((igrp_debug_addr == 0)
		|| igrp_debug_addr == ip->srcadr)
	    && ((igrp_debug_target == 0)
		|| (igrp_debug_target == network))) 
	    igrp_metricdebug("exterior network", network, metric, pdb->arg1);
	if (igrp_debug_events)
 	    exterior_route_count++;
	pdb->igrp_hopcount = ptr->hopcount;
	if (pdb->igrp_hopcount == 255) {
	    pdb->igrp_hopcount = 0;
	    ptr->hopcount = 0;
        }
	flags = NET_DEXTERIOR;
	if (pdb->exteriors_in_acl) {
	    /*
	     * If network number fails to match access list, don't consider it
	     * as exterior, even though it was advertised as such.
	     */
	    if (!nfast_check(pdb->exteriors_in_acl, network))
		flags = 0;
	}
	rdb = network_update(pdb, network, IPADDR_ZERO, source,
			     interface, metric, flags, source, 0, NULL, 0);
	igrp_rdb_update(rdb,ptr,interface,pdb->arg1);
	ptr = (metrictype *) ((char *) ptr + METRICSIZE);
	(void) process_may_suspend();
    }
    if ((igrp_debug_events
	 && (igrp_debug_addr == 0
	     || igrp_debug_addr == ip->srcadr))) {
        buginf("\nIGRP: Update contains %d interior, %d system, "
               "and %d exterior routes.", subnet_route_count,
	       network_route_count, exterior_route_count);
	buginf("\nIGRP: Total routes in update: %d",
	       subnet_route_count+network_route_count+exterior_route_count);
    }
}

/*
 * igrp_metricdebug
 * Print out a debugging message about a received IGRP metric.
 */

static void
igrp_metricdebug (char *string, ipaddrtype network, long metric, long metric2)
{
    if (metric != METRIC_INACCESSIBLE)
	 buginf("\n      %s %i, metric %d (neighbor %d)", string, network,
		metric, metric2); 
    else
	 buginf("\n      %s %i, metric %d (inaccessible)",
					string, network, metric);
}

/*
 * igrp_compute_metric
 * Give a metric entry and the incoming interface, return the composite metric
 * If interface is NULL, compute metric from just the metric entry data.
 *
 *  metric = [K1*bandwidth + (K2*bandwidth)/(256 - load) + K3*delay] *
 *				[K5/(reliability + K4)]
 *
 *  If K5 == 0, then there is no reliability term.
 *
 *  The default version of IGRP has K1 == K3 == 1, K2 == K4 == K5 == 0
 */

static ulong
igrp_compute_metric (metrictype *ptr, idbtype *interface, ulong network,
		     pdbtype *pdb, ulong *neighbor)
{
    ulong delay, bandwidth;
    ulong reliability, metric;

    /*
     * Combine factors from update and interface.
     * Return the update factors in neighbor.
     */
    bcopy(ptr->delay, (uchar *)&delay, 3);
    delay >>= 8;
    delay += offset_metric(pdb, interface, network, IN);
    if (delay >= IGRP_METRIC_INACCESSIBLE) {
	*neighbor = METRIC_INACCESSIBLE;
	return(METRIC_INACCESSIBLE);
    }
    bcopy(ptr->bandwidth, (uchar *)&bandwidth, 3);
    bandwidth >>= 8;

    /*
     * Compute the metric that our neighbor sees.
     */
    *neighbor = 0;
    if (pdb->igrp_k1)
	*neighbor += bandwidth*pdb->igrp_k1;
    if (pdb->igrp_k2)
	*neighbor += (bandwidth*pdb->igrp_k2)/(256-(ulong)ptr->load);
    if (pdb->igrp_k3)
	*neighbor += delay*pdb->igrp_k3;
    if (pdb->igrp_k5) {
	*neighbor *= pdb->igrp_k5;
	*neighbor /= (ptr->reliability + pdb->igrp_k4);
    }
    /*
     * Keep routing metric from getting too large
     */
     if (*neighbor > IGRP_METRIC_INACCESSIBLE)
	*neighbor = METRIC_INACCESSIBLE;

    if (interface != NULL) {
	delay = delay + interface->delay;
	bandwidth = max(bandwidth, 
			SCALED_BANDWIDTH(interface->visible_bandwidth));     
	reliability = min((ulong)ptr->reliability, 
			  interface->hwptr->reliability);
    } else
	reliability = (ulong)ptr->reliability;

    /*
     * Compute the scalar metric for the current Type of Service.
     * We are currently assuming a default TOS of zero.
     */
    metric = 0;
    if (pdb->igrp_k1)
	metric += bandwidth*pdb->igrp_k1;
    if (pdb->igrp_k2)
	metric += (bandwidth*pdb->igrp_k2)/(256-(ulong)ptr->load);
    if (pdb->igrp_k3)
	metric += delay*pdb->igrp_k3;
    if (pdb->igrp_k5) {
	metric *= pdb->igrp_k5;
	metric /= (reliability + pdb->igrp_k4);
    }
    if (metric > IGRP_METRIC_INACCESSIBLE)
	metric = METRIC_INACCESSIBLE;
    return(metric);
}

/*
 * igrp_rdb_update
 * Given a routing block, update the IGRP specific data
 */

static void
igrp_rdb_update (register rdbtype *rdb, register metrictype *ptr,
		 register idbtype *interface, ulong nexthop_metric)
{
    ulong temp;

    if (!rdb)
	return;
    bcopy(ptr->delay, (uchar *)&temp, 3);
    temp >>= 8;
    rdb->rdb_delay = temp + interface->delay;
    bcopy(ptr->bandwidth, (uchar *)&temp, 3);
    temp >>= 8;
    rdb->rdb_bandwidth =
	max(temp, SCALED_BANDWIDTH(interface->visible_bandwidth));
    temp = GETSHORT(ptr->mtu);
    if (temp)
	rdb->rdb_mtu = (ushort) (min(temp, interface->ip_mtu));
    else
	rdb->rdb_mtu = interface->ip_mtu;
    if (ptr->reliability)
	rdb->reliability = min(ptr->reliability, 
			       interface->hwptr->reliability);
    else
	rdb->reliability = interface->hwptr->reliability;
    rdb->load = max(ptr->load, interface->hwptr->load);
    rdb->hopcount = ptr->hopcount;
    rdb->rdb_nexthop_metric = nexthop_metric;
}

/*
 * igrp_holddown
 * We have received worse information about a path.  Determine if we
 * should flush the path (possibly resulting in a poison reverse) or
 * accept the worse information.  Return TRUE to flush the path;
 * return FALSE to accept the new, worse information.
 */

static boolean
igrp_holddown (pdbtype *pdb, ndbtype *ndb, rdbtype *rdb, ulong newmetric)
{
    /*
     * If we have new information, or this information is about a route
     * that is not optimal, then perform a feasible successor check.
     * If it's a feasible successor, return FALSE.  It's feasible if:
     * - our neighbor's metric is better than our best metric, and
     * - our new metric is less than the variance times the best metric
     * If it is not feasible (presumably it was at some point) then return
     * TRUE. 
     */ 
    if (!rdb || rdb->metric != ndb->metric) {
	if ((pdb->arg1 < ndb->metric)
	    && (newmetric < pdb->igrp_variance * ndb->metric))
	    return(FALSE);
	return(TRUE);
    }

    /*
     * If holddowns are off, accept new information with a worse metric
     * only if its hopcount is the same or less than the old information.
     * The +1 is probably right, but in mixed 7.1 and 8.0 networks, not all
     * routers increment the hopcount, so this prevents loops in some cases.
     */
    if (!pdb->holdokay)
	return((pdb->igrp_hopcount > (rdb->hopcount /* +1 */)) ? TRUE : FALSE);

    /*
     * If holddowns are on, allow a 10% variance in the metric information
     * If it is within 10% and the hopcount has increased go into holddown.
     * This allows fast media like FDDI to work with this algorithm.
     */
    return(((ndb->metric + ndb->metric/10) < newmetric) ? TRUE :
	   pdb->igrp_hopcount > (rdb->hopcount /* +1 */) ? TRUE : FALSE);
}

/*
 * igrp_sendrequest
 * Broadcast an IGRP request for information.  We send two broadcasts
 * to cope with the fact that the gateways will drop the first reply
 * when they attempt to ARP us.  The second reply will get through to us.
 */

static void
igrp_sendrequest (pdbtype *pdb, idbtype *sidb)
{
    paktype *pak, *copy;
    ipaddrtype address;
    igrptype *igrp;
    idbtype *idb;
    ndbtype *ndb;
    int bytes;
    boolean send;
    ipaddtype *secptr;

    if (!pdb->running)
	return;

    bytes = IPHEADERBYTES(NOPT) + IGRPHEADERBYTES;
    pak = getbuffer(bytes);
    if (!pak)
	return;
    igrp = (igrptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igrp->version = IGRP_VERSION;
    igrp->opcode = IGRP_OPC_REQUEST;
    igrp->edition = 0;
    igrp->asystem = pdb->asystem;
    igrp->ninterior = 0;
    igrp->nsystem = 0;
    igrp->nexterior = 0;
    igrp->checksum = 0;
    igrp->checksum = ipcrc((ushort *)igrp, IGRPHEADERBYTES);
    FOR_ALL_SWIDBS(idb) {
	if (idb->ip_enabled 
	    && !(idb->ip_passive_router & pdb->mask)
	    && interface_up_simple(idb)
	    && !(idb->static_routing)) {
	    send = FALSE;
	    address = idb->ip_address;
	    if (!address && idb->ip_unnumbered)
		address = idb->ip_unnumbered->ip_address;
	    if (ip_pdb_net_advertise(pdb, address, &ndb)) {
		send = TRUE;
	    } else {
		for (secptr = (ipaddtype *) idb->ip_addressQ.qhead; secptr;
		     secptr = secptr->next) {
		    if (ip_pdb_net_advertise(pdb, secptr->address, &ndb)) {
			address = secptr->address;
			send = TRUE;
			break;
		    }		    
		}
	    }
	    if (send) {
		if (igrp_debug || igrp_debug_events)
		    buginf("\nIGRP: broadcasting request on %s",
			   idb->namestring);
		copy = pak_duplicate(pak);
		if (copy)
		    ipwrite(copy, bytes, IGRP_PROT, address, -1,
			    idb, NULL, &iprouteparams);
	    }
	}
    }
    retbuffer(pak);
}

/*
 * igrp_rcvrequest
 * Receive a request and maybe send a reply.
 */

static void
igrp_rcvrequest (pdbtype *pdb, ndbtype *ndb, paktype *pak)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    if (pak->if_input->ip_passive_router & pdb->mask)
	return;
    if (!igrp_validate_network(pdb, ndb, pak))
	return;
    if ((igrp_debug || igrp_debug_events)
	&& (igrp_debug_addr == 0
	    || igrp_debug_addr == ip->srcadr)) 
	buginf("\nIGRP: received request from %i on %s", ip->srcadr,
	       pak->if_input->namestring);
    igrp_sendupdate(pdb, ndb, ip->srcadr, pak->if_input,
		    0L, 0L, 0L);
}

/*
 * igrp_show_traffic
 * IGRP statistics
 */

static void
igrp_show_traffic (void)
{
    printf("\n\nIGRP statistics:");
    printf("\n  Rcvd: %lu total, %lu checksum errors",
 		igrp_traffic.inputs, igrp_traffic.checksumerr);
    printf("\n  Sent: %lu total", igrp_traffic.outputs);
}

/*
 * igrp_showipproto
 * Show igrp, igprp2 specific information
 */
static void
igrp_showipproto (pdbtype *pdb)
{
    neighbortype *neigh;
    int headerflag;

    printf("\n  Default networks %sflagged in outgoing updates",
	   pdb->exteriors_out ? "": "not ");
    if (pdb->exteriors_out_acl)
	printf(", if matching access list %s",
	       pdb->exteriors_out_acl->name); 
    printf("\n  Default networks %saccepted from incoming updates",
	   pdb->exteriors_in ? "": "not ");
    if (pdb->exteriors_in_acl)
	printf(", if matching access list %s",
	       pdb->exteriors_in_acl->name); 
    printf("\n  IGRP metric weight K1=%u, K2=%u, K3=%u, K4=%u, K5=%u",
	   pdb->igrp_k1, pdb->igrp_k2, pdb->igrp_k3, pdb->igrp_k4,
	   pdb->igrp_k5);
    printf("\n  IGRP maximum hopcount %d", pdb->igrp_maxhops);
    printf("\n  IGRP maximum metric variance %d", pdb->igrp_variance);

    if (pdb->defaultmetricflag) {
	printf("\n  Default redistribution metric is ");
	printf("%u %u %d %d %d", pdb->user_bandwidth,
	       pdb->def_delay, pdb->def_reliability,
	       pdb->def_loading, pdb->def_mtu);
    }

    showipproto_redistribute(pdb);
	   
    for (neigh = pdb->neighbors, headerflag = TRUE; neigh; 
	 neigh = neigh->next) {
	if (headerflag) {
	    automore_header(showipproto_neighborheader);
	    headerflag = FALSE;
	}
	printf("\n    %i",neigh->address);
    }
    automore_header(NULL);
}

/*
 * igrp_distcmp
 * Compare administrative distances.
 * Return an indication of whether or not 'our' distance is better than
 * 'their' distance.  Normally, this is the traditional distance check.
 * But, if IGRP<->EIGRP auto-redistribution is in effect and the EIGRP
 * route is external, consider route metrics instead.
 */

static enum DISTTYPE
igrp_distcmp (pdbtype *pdb, uchar our_dist, ndbtype *db,
	      ulong metric, ushort route_type)
{
    pdbtype *their_pdb;
    ulong their_metric;

    their_pdb = INDEX2PDB(db->pdbindex);
    if (their_pdb && (their_pdb->proctype & PROC_IGRP2) &&
	their_pdb->asystem == pdb->asystem) {
	/*
	 * Same AS as us, so automatic redistribution is occuring.
	 * If comparing IGRP route against external EIGRP route,
	 * compare metrics and ignore distance.
	 */
	if (db->route_type == RTYPE_IGRP2_EXT) {
	    /*
	     * Shift EIGRP metric for comparison purposes
	     */
	    their_metric = (db->metric >> 8);
	    if (metric < their_metric)
		return(IP_DIST_BETTER);
	    return(IP_DIST_WORSE);
	}
    }
    return(iprouting_distcmp_inline(our_dist, db->distance));
}

/*
 * igrp_pdb_init
 * Initialize an IGRP Protocol Descriptor Block
 */
static pdbtype *
igrp_pdb_init (ushort asystem)
{
    pdbtype *pdb;

    /*
     * Create the pdb.
     */
    pdb = malloc_named(sizeof(pdbtype), ip_pdb_name);
    if (pdb == NULL) {
	printf(nomemory);
	return(NULL);
    }
    /*
     * Allocate an unused router process flag
     */
    if (allocate_pdbindex(pdb) == -1) {
	free(pdb);
	return(NULL);
    }	
    pdb->name = "igrp";
    pdb->family = PDB_IP;
    pdb->pname = "IGRP Router";
    pdb->atomic_update = TRUE;
    pdb->process = (processcbtype)igrp_router;
    pdb->cleanup = igrp_cleanup;
    pdb->majorupdate = igrp_sendupdate;
    pdb->is_feasible = simple_variance;
    pdb->holddown = igrp_holddown;
    pdb->query = igrp_sendrequest;
    pdb->showipproto = igrp_showipproto;
    pdb->holdokay = TRUE;
    pdb->validate_source = TRUE;
    pdb->proctype = PROC_IGRP;
    pdb->distflag = pdb->mask;
    pdb->redistallowed = PROC_REDISTMASK;
    pdb->exteriors_out = TRUE;
    pdb->exteriors_in = TRUE;
    pdb->broadcasttime = pdb->def_broadcasttime = IGRP_CYCLETIME;
    pdb->invalidtime = pdb->def_invalidtime = IGRP_ROUTEFLUSHTIME;
    pdb->holdtime = pdb->def_holdtime =  IGRP_BLINDTIME;
    pdb->flushtime = pdb->def_flushtime = IGRP_SUBNETFLUSHTIME;
    pdb->multipath = pdb->def_multipath = DEFMAXROUTES;
    pdb->distance = pdb->def_distance = IGRP_DISTANCE;
    pdb->distance_cmp = igrp_distcmp;
    pdb->igrp_variance = IGRP_DEFVARIANCE;
    pdb->asystem = asystem;
    pdb->igrp_k1 = pdb->igrp_k1_def = IGRP_K1_DEFAULT;
    pdb->igrp_k2 = pdb->igrp_k2_def = IGRP_K2_DEFAULT;
    pdb->igrp_k3 = pdb->igrp_k3_def = IGRP_K3_DEFAULT;
    pdb->igrp_k4 = pdb->igrp_k4_def = IGRP_K4_DEFAULT;
    pdb->igrp_k5 = pdb->igrp_k5_def = IGRP_K5_DEFAULT;
    pdb->igrp_maxhops = pdb->igrp_def_maxhops = IGRP_MAXHOPS;
    TIMER_STOP(pdb->sleep_timer);
    pdb->supports_snapshot = TRUE;
    enqueue(&protoQ, pdb);
    return(pdb);
}

/*
 * igrp_init
 * initialize IGRP subsystem
 */
static void
igrp_init (subsystype *subsys)
{
    memset(&igrp_traffic, 0, sizeof(igrp_traffic));

    /*
     * Register some functions
     */
    ip_set_proto_outcounter(IGRP_PROT, &igrp_traffic.outputs);
    reg_add_ip_enqueue(IGRP_PROT, igrp_enqueue, "igrp_enqueue");
    reg_add_ip_show_traffic(igrp_show_traffic, "igrp_show_traffic");
    reg_add_iprouting_pdb_init(PROC_IGRP, igrp_pdb_init, "igrp_pdb_init");
}

/*
 * IGRP subsystem header
 */

#define IGRP_MAJVERSION 1
#define IGRP_MINVERSION 0
#define IGRP_EDITVERSION  1

SUBSYS_HEADER(igrp, IGRP_MAJVERSION, IGRP_MINVERSION,
	      IGRP_EDITVERSION, igrp_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iprouting", "req: iprouting");

