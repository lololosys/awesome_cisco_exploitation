/* $Id: egp.c,v 3.6.20.5 1996/08/28 12:52:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/egp.c,v $
 *------------------------------------------------------------------
 * egp.c -- Exterior Gateway Protocol
 *
 * 22-April-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: egp.c,v $
 * Revision 3.6.20.5  1996/08/28  12:52:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.20.4  1996/06/28  23:18:46  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.20.3  1996/06/28  01:20:46  snyder
 * CSCdi61601:  fix some char * that should have been char [] in iprouting
 * Branch: California_branch
 *
 *              You get space back for the pointer
 *              You get space back for each reference
 *              You get bytes out of data space
 *
 * Revision 3.6.20.2  1996/06/18  01:47:07  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.6.20.1  1996/05/04  01:21:27  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.6  1996/02/01  06:05:15  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/22  06:14:10  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/11  02:29:05  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . merged registry functions ip_tuba_best_local_address and
 *     ip_choose_saddr to function ip_best_local_address().
 *
 * Revision 3.3  1995/11/17  17:32:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:48:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:06:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/09/11  23:50:19  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.5  1995/06/28 09:27:45  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.4  1995/06/19  01:07:28  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/15  22:06:51  hampton
 * Fix 24.5 day bugs in EGP support.  While there, eliminate
 * direct references to msclock.  [CSCdi35916]
 *
 * Revision 2.2  1995/06/09 13:06:13  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:06:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "login.h"
#include "config.h"
#include "subsys.h"
#include "../ui/debug.h"
#include "route.h"
#include "iprouting_debug.h"
#include "neighbor.h"
#include "../ip/ip.h"
#include "route_inlines.h"
#include "../ip/ip_registry.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "egp.h"
#include "logger.h"
#include "../iprouting/msg_egp.c"	/* Not a typo, see logger.h */
#include "../os/buffers.h"
#include "../if/network.h"
#include "parser_defs_router.h"
#include "route_map.h"

/*
 * Forward definitions
 */
static pdbtype *egp_pdb_init(ushort asystem);
static void egp_process(paktype *pak);
static forktype egp_upd_process(void);
static void egp_input(paktype *pak);
static neighbortype *egp_promiscuous_lookup(ipaddrtype address,
					    ipaddrtype destination,
					    ushort asystem); 
static neighbortype *egp_lookup(ipaddrtype address, ipaddrtype myaddress,
				pdbtype *pdb); 
static ipaddrtype egp_lookup_local_address(ipaddrtype naddress);
static void egp_neighbor(pdbtype *pdb, ipaddrtype naddress, 
			 parseinfo *csb, boolean sense); 
static void input_idle(neighbortype *en, egptype *egp);
static void input_acquistion(neighbortype *en, egptype *egp);
static void input_down(neighbortype *en, egptype *egp);
static void input_up(neighbortype *en, egptype *egp);
static void input_cease(neighbortype *en, egptype *egp);
static void na_negotiate(neighbortype *en, egptype *egp);
static void reachable(neighbortype *en, uchar status);
static void reachable_check(neighbortype *en);
static void fsm_error(neighbortype *en, char *s, int arg);
static void egp_router(pdbtype *pdb);
static void egp_cleanup(pdbtype *pdb);
static void timer_hello(neighbortype *en);
static void timer_poll(neighbortype *en);
static void egp_stop(neighbortype *en);
static void egp_start(neighbortype *en);
static void sendreply(neighbortype *en, paktype *pak, int len);
static void egp_na_message(neighbortype *en, int code, int status, int
			   hello, int poll); 
static void egp_nr_message(neighbortype *en, int code);
static void egp_poll_request(neighbortype *en);
static void egp_update(neighbortype *en, egptype *egp, boolean unsolicited);
static boolean checksend(neighbortype *en, paktype *replypak, uchar *cp, int
			 bytesleft); 
static void route_update(neighbortype *en, egptype *egp, boolean debug);
static int host2egp(uchar *cp, ipaddrtype address, int *count);
static int net2egp(uchar *cp, ipaddrtype address, int *count);
static int egp2host(uchar *cp, ipaddrtype *address);
static int egp2net(uchar *cp, ipaddrtype *address);
static void egp_print(egptype *egp, ipaddrtype source, ipaddrtype destination);
static const char *typestring(int type);
static const char *codestring(int type, int code);
static const char *statusstring(int type, int status);
static void egp_changestate(neighbortype *en, int newstate);
static const char *egp_statestring(int state);
static void egp_show_traffic(void);
static void egp_showipproto(pdbtype *pdb);

#define RETRYTIME (5*ONEMIN)		/* try restarting a deader in 5 min */


/*
 * Semi-public (subsystem) storage
 */
ushort egp_asystem;			/* default autonomous system number */
neighbortype *egp_neighbors[NETHASHLEN];/* pointer to neighbor table */
/*
 * Local Storage
 */
static boolean egp_update_running;     	/* TRUE if input process running */
static queuetype egpQ;			/* EGP input queue */
static queuetype deadneighborQ;		/* EGP dead neighbor queue */
static queuetype deadtpQ;		/* EGP dead third party info queue */
static queuetype deadpdbQ;		/* EGP dead pdbs */
egp_traffic_t egp_traffic;

static const char egpmust[] = "Must give an \"autonomous-system <LAS>\" command first";

/*
 * egp_process
 * Handle input of EGP packets
 */

static void
egp_process (paktype *pak)
{
    egp_traffic.inputs++;
    if (!egp_update_running)
	retbuffer(pak);
    else
	enqueue(&egpQ, pak);
}

/*
 * egp_upd_process
 * Routine to handle EGP input processing
 */

static forktype
egp_upd_process (void)
{
    paktype *pak;
    neighbortype *egpn;
    thirdpartytype *tp;
    pdbtype *pdb;

    while (TRUE) {
	edisms((blockproc *)queueBLOCK, (ulong) &egpQ);
	while ((pak = dequeue(&egpQ))) {
	    if (ip_router_running && systeminit_flag && !system_loading) {
		egp_input(pak);
	    } else
		retbuffer(pak);
	}

	/*
	 * Clean up dead neighbors, pdbs and obsolete third party
	 * information.  Do this here so that we're not processing updates
	 * while playing with this info.
	 */
	while ((egpn = dequeue(&deadneighborQ))) {
	    while (egpn->thirdp) {
		tp = egpn->thirdp;
		egpn->thirdp = tp->next;
		free(tp);
	    }
	    free(egpn);
	}
	while ((pdb = dequeue(&deadpdbQ))) {
	    free_pdbindex(pdb);
	    free(pdb);
	}
	while ((tp = dequeue(&deadtpQ))) {
	    free(tp);
	}
    }
}

/*
 * egp_input
 * Handle an EGP datagram
 */

static void
egp_input (register paktype *pak)
{
    iphdrtype *ip;
    register egptype *egp;
    register neighbortype *en;

    ip = (iphdrtype *)ipheadstart(pak);

    /* 
     *  Obtain pointer to beginning of EGP packet.
     */
    egp = (egptype *) ipdatastart(pak);
    if (egp_debug
	&& (egp_debug_addr == 0 || egp_debug_addr == ip->srcadr)) 
	egp_print(egp, ip->srcadr, ip->dstadr);

    /* 
     *  Find the EGP table entry corresponding to this gateway.
     *  If none found, this isn't a registered peer, so drop.
     */
    en = egp_lookup(ip->srcadr, ip->dstadr, NULL);
    if (!en)
	en = egp_promiscuous_lookup(ip->srcadr, ip->dstadr, egp->asystem);
    if (!en) {
	retbuffer(pak);
	return;
    }
    /*
     * It's possible that we haven't figured out which AS our neighbor is
     * in.  Double check it.
     */
    if (en->yoursystem == 0) 
	en->yoursystem = egp->asystem;

    /* 
     *  Check version number.
     */
    if (egp->version != EGP_VERSION) {
	if ((egp_debug || egpevent_debug)
	    && (egp_debug_addr == 0 || egp_debug_addr == ip->srcadr))
	    buginf("\nEGP: wrong EGP version (%d) from %i", egp->version,
			ip->srcadr);
	en->inerrs++;
	IP_DISPOSE(egp, formaterr, pak);
	return;
    }

    /* 
     *  Check checksum.
     */
    if (ipcrc((ushort *)egp, (ip->tl - (ip->ihl << 2)))) {
	if ((egp_debug || egpevent_debug)
	    && (egp_debug_addr == 0 || egp_debug_addr == ip->srcadr))
	    buginf("\nEGP: checksum error from %i", ip->srcadr);
	en->inerrs++;
	IP_DISPOSE(egp, checksumerr, pak);
	return;
    }

    en->inmsgs++;
    /*
     * Check FAS number if debugging.  We don't enforce this check for
     * reasons of backwards compatibility.  For core gateways, this isn't
     * relevant. 
     */
    if ((egp_debug && en->yoursystem && (en->yoursystem != egp->asystem))
	&& (egp_debug_addr == 0 || egp_debug_addr == ip->srcadr))
	buginf("\nEGP: %i configured as FAS %d, but claims FAS %d",
			en->address, en->yoursystem, egp->asystem);

    if (egp->type == EGP_TYPE_ERROR)
	en->errsrcvd++;
    /*
     * Switch on current state.  First record input interface.
     */
    en->idb = pak->if_input;
    switch (en->state) {
	case IDLE_STATE:
	    input_idle(en, egp);
	    break;
	case ACQUISITION_STATE:
	    input_acquistion(en, egp);
	    break;
	case DOWN_STATE:
	    input_down(en, egp);
	    break;
	case UP_STATE:
	    input_up(en, egp);
	    break;
	case CEASE_STATE:
	    input_cease(en, egp);
	    break;
	default:
	    fsm_error(en, "bad state in input", 0);
	    break;
    }	
    retbuffer(pak);
}

/*
 * egp_promiscuous_lookup
 * Look up an EGP neighbor and see if we will accept that neighbor under
 * our promiscuous access list.  If so, set up and return the neighbor data
 * structure. 
 */

static neighbortype *
egp_promiscuous_lookup (ipaddrtype address, ipaddrtype destination, 
		        ushort asystem)
{
    pdbtype *pdb;
    neighbortype *en;
    int bucket;
    thirdpartytype *tp, *ntp;

    for (pdb = (pdbtype *) protoQ.qhead; pdb; pdb = pdb->next) {
	if ((pdb->proctype & PROC_EGP) && pdb->promiscuous) {

	    /*
	     * First, check to see if this is allowed.
	     */
	    if (fast_check(pdb->pfilter, address))
		break;
	}
    }

    /*
     * Found nothing, bail out.
     */
    if (!pdb)
	return(NULL);

    /*
     * Go ahead and clone the neighbor from the pdb->promiscuous neighbor.
     * Be sure to clone the third party list.  Return the newly created
     * neighbor structure. 
     */
    en = malloc(sizeof(neighbortype));
    if (!en)
	return(NULL);
    if ((egp_debug || egpevent_debug)
	&& (egp_debug_addr == 0 || egp_debug_addr == address))
	buginf("\nEGP: %i added as new neighbor", address);
    bucket = nethash(address);
    en->next = egp_neighbors[bucket];
    egp_neighbors[bucket] = en;
    en->address = address;
    if (ip_ouraddress(destination))
	en->myaddr = destination;
    else
	en->myaddr = egp_lookup_local_address(address);
    en->mysystem = egp_asystem;
    en->yoursystem = asystem;
    egp_changestate(en, IDLE_STATE);
    en->flags &= ~EN_DEAD;
    en->flags |= EN_TEMP;
    en->mode = EGP_NAS_UNSPEC;
    en->pdb = pdb;
    for (tp = (thirdpartytype *) pdb->promiscuous->thirdp;
	 tp; tp = tp->next) {
	ntp = malloc(sizeof(thirdpartytype));
	if (!ntp)
	    break;
	ntp->address = tp->address;
	ntp->flags = tp->flags;
	enqueue((queuetype *)&en->thirdp, ntp);
    }
    return(en);		
}

/*
 * egp_lookup
 * Lookup an EGP gateway in a table, add it if necessary.
 * Returns neighbortype pointer or NULL
 */

static neighbortype *
egp_lookup (ipaddrtype address, ipaddrtype myaddress, pdbtype *pdb)
{
    neighbortype *en;
    int bucket;

    bucket = nethash(address);
    for (en = egp_neighbors[bucket]; en; en = en->next) {
	if (en->address == address) {
	    if (en->state == IDLE_STATE) {
		en->myaddr = myaddress;
		en->flags &= ~EN_FLAGMASK;
		en->mode = EGP_NAS_UNSPEC;
		en->sndseq = 0;
		TIMER_STOP(en->hello_timer);
		TIMER_STOP(en->poll_timer);
		TIMER_STOP(en->abort_timer);
	    }
	    en->flags &= ~EN_DEAD;
	    return(en);
	}
    }

    /*
     * Create neighbor only if a pdb was specified.
     * This means only pre-specified neighbors are created.
     */
    if (pdb == NULL)
	return(NULL);
    if (egp_asystem == 0) {
	printf(egpmust);
	return(NULL);
    }
    en = malloc(sizeof(neighbortype));
    if (!en)
	return(NULL);
    en->next = egp_neighbors[bucket];
    egp_neighbors[bucket] = en;
    en->address = address;
    en->myaddr = myaddress;
    en->mysystem = egp_asystem;
    en->yoursystem = pdb->asystem;
    en->pdb = pdb;
    egp_changestate(en, IDLE_STATE);
    en->flags &= ~EN_DEAD;
    en->mode = EGP_NAS_UNSPEC;
    return(en);		
}

/*
 * egp_lookup_local_address
 * For a particular neighbor, look up our best local address.
 */

static ipaddrtype
egp_lookup_local_address (ipaddrtype naddress)
{
    ipaddrtype myaddress, snaddress = 0;
    idbtype *idb;
    
    /*
     * Correlate the EGP neighbor address to a major network.
     * If major network is subnetted, try for best subnet.
     * We can't simply call ip_best_local_address() since the routing lookup
     * code might try to use an indirect path if the "best" interface is down.
     * Preferences:
     *   1 - an interface address that's on our neighbors subnet.
     *   2 - a "best" local address that's on our neighbors major net.
     *   3 - any subnet that's on our neighbors major net.
     *   4 - our "best" local address
     */
    myaddress = 0;
    FOR_ALL_SWIDBS(idb) {
	if (idb->ip_enabled && interface_up(idb) &&
	    (idb->ip_majornet == get_majornet(naddress))) {
	    snaddress = idb->ip_address;
	    if ((idb->ip_address & idb->ip_nets_mask) ==
		(naddress & idb->ip_nets_mask)) {
		myaddress = idb->ip_address;
		break;
	    }
	}
    }
    if (myaddress == 0) {
	myaddress = ip_best_local_address(naddress, FALSE);
	if ((get_majornet(myaddress) != get_majornet(naddress)) &&
	    (snaddress != 0))
	    myaddress = snaddress;
    }
    return(myaddress);
}

/*
 * egp_neighbor
 * Process egp neighbor commands.  An address of 0.0.0.0 indicates that
 * we've been given a "neighbor any" command and we're going to be
 * promiscuous. 
 */

static void
egp_neighbor (pdbtype *pdb, ipaddrtype naddress, parseinfo *csb, boolean sense)
{
    neighbortype *egpn;
    thirdpartytype *tp;
    ipaddrtype myaddress = 0;
    ipaddrtype tpaddress = 0;
    int i;

    if (csb->nvgen) {
	for (i = 0; i < NETHASHLEN; i++) {
	    for (egpn = egp_neighbors[i]; egpn; egpn = egpn->next) {
		if ((egpn->flags & EN_TEMP)
		    || !((egpn->flags & EN_KEEP)
			 && (pdb == egpn->pdb)))
		    continue;
		nv_write(TRUE, "%s %i", 
			 csb->nv_command, egpn->address);
		for (tp = egpn->thirdp; tp; tp = tp->next) {
		    nv_write(TRUE, "%s %i third-party %i",
			     csb->nv_command, egpn->address, tp->address);
		    nv_add(tp->flags & TP_EXTERNAL, " external");
		}
	    }
	}

	if (pdb->promiscuous) {
	    nv_write(TRUE, "%s any", csb->nv_command);
	    nv_add(pdb->pfilter, " %d", pdb->pfilter);
	    for (tp = pdb->promiscuous->thirdp; tp; tp = tp->next) {
		nv_write(TRUE, "%s any third-party %i",
			 csb->nv_command, tp->address);
		nv_add(tp->flags & TP_EXTERNAL, " external");
	    }
	}
	return;
    }

    if (csb->which != NEIGHBOR_ANY &&
	csb->which != NEIGHBOR_EGP_3PARTY &&
	csb->which != NEIGHBOR_STD) {
	printf("\n%% EGP: Invalid command - %s", csb->line);
	return;
    }

    /* neighbor any ... */
    if (naddress == 0L) {
	if (pdb->asystem != 0) {
	    printf("\n%% Only valid for 'egp 0' - %s", csb->line);
	    return;
	}
	pdb->pfilter = GETOBJ(int,2);
	if (!pdb->promiscuous) {
	    pdb->promiscuous = (neighbortype *)
		malloc(sizeof(neighbortype));
	    if (!pdb->promiscuous)
		return;
	}
	egpn = pdb->promiscuous;
    } else {	        /* neighbor <addr> third-party ... */
	myaddress = egp_lookup_local_address(naddress);
	egpn = (neighbortype *) egp_lookup(naddress, myaddress, pdb);
	if (!egpn)
	    return;
    }

    if (csb->which == NEIGHBOR_EGP_3PARTY) {
	tpaddress = GETOBJ(paddr,2)->ip_addr;
	if (naddress &&
	    (get_majornet(myaddress) != get_majornet(tpaddress))) {
	    printf("\nEGP third party address must be on same major"
		   " network."); 
	    return;
	}
	for (tp = egpn->thirdp; tp; tp = tp->next)
	    if (tp->address == tpaddress) {
		unqueue((queuetype *)&egpn->thirdp, tp);
		break;
	    }
	if (csb->sense) {
	    if (!tp) {
		tp = malloc(sizeof(thirdpartytype));
		if (!tp)
		    return;
		tp->address = tpaddress;
	    }
	    if (GETOBJ(int,1))	    /* TRUE if external, FALSE if internal */
		tp->flags = TP_EXTERNAL;
	    else
		tp->flags = TP_INTERNAL;
	    enqueue((queuetype *)&egpn->thirdp, tp);
	} else {
	    if (tp)
		free(tp);
	}
	return;
    }
    
    /*
     * If we're flushing an entry, declare a stop event and clear
     * the permanent flag.
     */
    if (!sense) {
	if (egpn == pdb->promiscuous) {
	    while (egpn->thirdp) {
		tp = egpn->thirdp;
		egpn->thirdp = tp->next;
		free(tp);
	    }
	    pdb->promiscuous = NULL;
	    free(egpn);
	    pdb->pfilter = 0;
	} else {
	    egp_stop(egpn);
	    egpn->flags &= ~EN_KEEP;
	    egpn->flags |= EN_DEAD;
	    return;
	}
    }
    /*
     * If the entry is idle, mark it kept and inactive.
     * The egp router process will possibly try to acquire it.
     */
    if (egpn->state == IDLE_STATE)
	egpn->flags |= (EN_KEEP + EN_DEAD);
    TIMER_STOP(egpn->retrytime);
    egpn->pdb = pdb;
}

/*
 * autonomous_command
 * Set default autonous system number
 */

void
autonomous_command (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write(egp_asystem, "%s %d\n!", csb->nv_command, egp_asystem);
	return;
    }

    egp_asystem = (ushort) GETOBJ(int,1);
}

/*
 * egp_timers
 * Set hello and poll timers for this EGP process
 * Called after parsing router subcommand "timers egp"
 */

void
egp_timers_command (parseinfo *csb)
{
    pdbtype *pdb = csb->protocol;

    if (!router_enable)
	return;

    if (!(pdb->proctype & PROC_EGP))
	return;

    if (csb->nvgen) {
	nv_write(((pdb->egp_hello != P1) || (pdb->egp_poll != P2)),
		 "%s %d %d", csb->nv_command, pdb->egp_hello, pdb->egp_poll);
	return;
    }

    if (csb->sense == FALSE) {
	pdb->egp_hello = P1;
	pdb->egp_poll = P2;
    } else {
	pdb->egp_hello = GETOBJ(int,1);
	pdb->egp_poll = GETOBJ(int,2);
    }
}

/*
 * input_idle
 * Received a packet while in the idle state
 */

static void
input_idle (register neighbortype *en, register egptype *egp)
{
    if (egp->type != EGP_TYPE_NA) {
	if (egp->type != EGP_TYPE_ERROR)
	    egp_na_message(en, EGP_NA_CEASE, EGP_NAS_PROTO, 0, 0);
	en->flags |= EN_DEAD;
	return;
    }
    switch (egp->code) {
	case EGP_NA_REQUEST:
	    en->rcvseq = egp->sequence;
	    if (en->pdb) {
		na_negotiate(en, egp);
		egp_changestate(en, DOWN_STATE);
		egp_na_message(en, EGP_NA_CONFIRM, en->mode,
						en->hello_int, en->poll_int);
		egp_nr_message(en, EGP_NR_HELLO);
		en->flags &= ~EN_DEAD;
	    } else {
		egp_na_message(en,EGP_NA_REFUSE, EGP_NAS_ADMIN, 0, 0);
		en->flags |= EN_DEAD;
	    }
	    break;
	case EGP_NA_CEASE:
	    en->rcvseq = egp->sequence;
	    egp_na_message(en, EGP_NA_CEASEACK, en->mode, 0, 0);
	    en->flags |= EN_DEAD;
	    break;
	case EGP_NA_REFUSE:
	case EGP_NA_CONFIRM:
	    egp_na_message(en,EGP_NA_CEASE,EGP_NAS_PROTO,0,0);
	    en->flags |= EN_DEAD;
	    break;
	case EGP_NA_CEASEACK:
	    en->flags |= EN_DEAD;
	    break;
	default:
	    fsm_error(en, "bad NA code %d when idle", egp->code);
	    en->flags |= EN_DEAD;
	    break;
    }
}

/*
 * input_acquisition
 * Received a packet while in the acquisition state
 */

static void
input_acquistion (register neighbortype *en, register egptype *egp)
{
    if (egp->type != EGP_TYPE_NA) {
	fsm_error(en, "not NA type %d when input acq.", egp->type);
	return;
    }
    switch (egp->code) {
        case EGP_NA_REQUEST:
	    en->rcvseq = egp->sequence;
	    na_negotiate(en,egp);
	    egp_changestate(en,DOWN_STATE);
	    egp_na_message(en, EGP_NA_CONFIRM, en->mode,
					en->hello_int,en->poll_int);
	    egp_nr_message(en,EGP_NR_HELLO);
	    break;
	case EGP_NA_CONFIRM:
	    na_negotiate(en, egp);
	    egp_changestate(en,DOWN_STATE);
	    reachable(en, egp->status);
	    egp_nr_message(en,EGP_NR_HELLO);
	    break;
	case EGP_NA_REFUSE:
	    egp_changestate(en,IDLE_STATE);
	    TIMER_START(en->retrytime, RETRYTIME);
	    break;
	case EGP_NA_CEASE:
	    en->rcvseq = egp->sequence;
	    egp_na_message(en,EGP_NA_CEASEACK, en->mode,0,0);
	    egp_changestate(en,IDLE_STATE);
	    TIMER_START(en->retrytime, RETRYTIME);
	    break;
	default:
	    fsm_error(en, "bad code %d when input acq.", egp->code);
	    break;
    }
}

/*
 * input_down
 * Received a packet while in the down state
 */

static void
input_down (register neighbortype *en, egptype *egp)
{
    switch (egp->type) {
	case EGP_TYPE_NA:
	    switch (egp->code) {
		case EGP_NA_REQUEST:
		    en->rcvseq = egp->sequence;
	            na_negotiate(en,egp);
	            egp_changestate(en,DOWN_STATE);
		    egp_na_message(en,EGP_NA_CONFIRM, en->mode,
						en->hello_int, en->poll_int);
		    egp_nr_message(en,EGP_NR_HELLO);
		    break;
		case EGP_NA_CEASE:
		    en->rcvseq = egp->sequence;
		    egp_na_message(en,EGP_NA_CEASEACK, en->mode,0,0);
		    egp_changestate(en, IDLE_STATE);
		    break;
		default:
		    fsm_error(en, "bad code %d in type NA when down",
			egp->code);
		    break;
	    }
	    break;

	case EGP_TYPE_NR:
	    switch (egp->code) {
		case EGP_NR_HELLO:
		    en->rcvseq = egp->sequence;
		    reachable(en, egp->status);
		    egp_nr_message(en, EGP_NR_IHU);
		    break;
		case EGP_NR_IHU:
		    reachable(en, egp->status);
		    break;
		default:
		    fsm_error(en, "bad code %d in type NR when down",
			egp->code);
		    break;
	    }
	    break;

	case EGP_TYPE_POLL:
	case EGP_TYPE_UPDATE:
	    if ((en->flags & EN_PASSIVE) && (egp->status == EGP_NRS_UP)) {
		egp_changestate(en,UP_STATE);
		input_up(en, egp);
		egp_poll_request(en);
		break;
	    }
	    /* else fall through */

	default:
	    fsm_error(en, "bad type %d when down", egp->type);
	    break;
    }
}

/*
 * input_up
 * Received a packet while in the up state
 */

static void
input_up (register neighbortype *en, register egptype *egp)
{
    switch (egp->type) {

	/*
	 * Neighbor Acquistion activity
	 */
	case EGP_TYPE_NA:
	    switch (egp->code) {
		case EGP_NA_REQUEST:
		    en->rcvseq = egp->sequence;
	            na_negotiate(en,egp);
	            egp_changestate(en,DOWN_STATE);
		    egp_na_message(en,EGP_NA_CONFIRM, en->mode,
						en->hello_int,en->poll_int);
		    egp_nr_message(en,EGP_NR_HELLO);
		    break;
		case EGP_NA_CEASE:
		    en->rcvseq = egp->sequence;
		    egp_na_message(en,EGP_NA_CEASEACK, en->mode,0,0);
		    egp_changestate(en, IDLE_STATE);
		    break;
		default:
		    fsm_error(en, "bad code %d in type NA when up",
			egp->code);
		    break;
	    }
	    break;

	/*
	 * Neighbor Reachability activity
	 */
	case EGP_TYPE_NR:
	    switch (egp->code) {
		case EGP_NR_HELLO:
		    en->rcvseq = egp->sequence;
		    reachable(en, egp->status);
		    egp_nr_message(en,EGP_NR_IHU);
		    break;
		case EGP_NR_IHU:
		    reachable(en, egp->status);
		    break;
		default:
		    fsm_error(en, "bad code %d in type NR when up",
			egp->code);
		    break;
	    }
	    break;

	/*
	 * Polling activity
	 */
	case EGP_TYPE_POLL:
	    en->rcvseq = egp->sequence;
	    reachable(en, egp->status);
	    egp_update(en,egp,FALSE);
	    break;
	case EGP_TYPE_UPDATE:
	    reachable(en, egp->status);
	    route_update(en,egp,FALSE);
	    break;

	/*
	 * Something bizarre
	 */
	default:
	    fsm_error(en, "bad type %d when up", egp->type);
	    break;
    }
}

/*
 * input_cease
 * Received a packet while in the ceased state
 */

static void
input_cease (register neighbortype *en, register egptype *egp)
{
    if (egp->type != EGP_TYPE_NA) {
	fsm_error(en, "not NA type %d when cease", egp->type);
	return;
    }
    switch (egp->code) {
	case EGP_NA_REQUEST:
	    en->rcvseq = egp->sequence;
	    egp_na_message(en,EGP_NA_CEASE,en->mode,0,0);
	    break;
	case EGP_NA_CEASE:
	    en->rcvseq = egp->sequence;
	    egp_na_message(en,EGP_NA_CEASEACK,en->mode,0,0);
	    egp_changestate(en, IDLE_STATE);
	    break;
	case EGP_NA_CEASEACK:
	    egp_changestate(en, IDLE_STATE);
	    break;
	default:
	    fsm_error(en, "bad code %d when cease", egp->code);
	    break;
    }
}

/*
 * na_negotiate
 * Initialize state variables subject to negotiation
 */

static void
na_negotiate (register neighbortype *en, register egptype *egp)
{
    pdbtype *pdb;

    pdb = en->pdb;
    en->hello_int = max(egp->t.na.helloint, pdb->egp_hello);
    en->poll_int = max(egp->t.na.pollint, pdb->egp_poll);
    switch (egp->status) {
	case EGP_NAS_UNSPEC:
	    if (egp_asystem <= egp->asystem)
		en->flags &= ~EN_PASSIVE;
	    else
		en->flags |= EN_PASSIVE;
	    break;
	case EGP_NAS_ACTIVE:
	    en->flags |= EN_PASSIVE;
	    break;
	case EGP_NAS_PASSIVE:
	    en->flags &= ~EN_PASSIVE;
	    break;
	default:
	    en->flags &= ~EN_PASSIVE;
	    break;	
    }
    en->mode = (en->flags & EN_PASSIVE) ? EGP_NAS_PASSIVE : EGP_NAS_ACTIVE;
}

/*
 * reachable
 * Process neighbor reachability indications
 */

static void
reachable (neighbortype *en, uchar status)
{
    TIMER_START(en->abort_timer, P4*ONESEC);
    if ((en->flags & EN_PASSIVE) && (status != EGP_NRS_UP))
	return;
    en->reachcount++;
}

/*
 * reachable_check
 * Analyze the reachablity information gather in the last hello (T1)
 * interval.  Decide whether to change state or not.  This code implicitly
 * assumes that the reachability window T3 is 4 times T1, the hello
 * retransmit window that we negotiated.
 */

static const short egpbits[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

static void
reachable_check (register neighbortype *en)
{
    register int indications, upthreshold, downthreshold;

    en->reachshift <<= 1;		/* shift once every T1 interval */
    if (en->reachcount) {		/* any reachability events? */
	en->reachshift += 1;		/* yes, "shift" in a 1 bit */
	en->reachcount = 0;		/* reset reachability event counter */
    }
    indications = (int) egpbits[(en->reachshift & 0xF)];
    if (en->flags & EN_PASSIVE) {
	downthreshold = DOWN_PASS_THRESHOLD;
	upthreshold = UP_PASS_THRESHOLD;
    }
    else {
	downthreshold = DOWN_ACT_THRESHOLD;
	upthreshold = UP_ACT_THRESHOLD;
    }
    if ((indications >= upthreshold) && (en->state == DOWN_STATE)) {
	egp_changestate(en,UP_STATE);
	egp_poll_request(en);
	egp_update(en, NULL, TRUE);
    }
    else if ((indications <= downthreshold) && (en->state == UP_STATE))
	egp_changestate(en,DOWN_STATE);
}

/*
 * fsm_error
 * Report a strange occurence.
 * The FSM error is most likely due to other guy....
 */

static void
fsm_error (neighbortype *en, char *s, int arg)
{
    char buffer[50];
    int len;

    if ((egp_debug || egpevent_debug)
	&& (egp_debug_addr == 0 || egp_debug_addr == en->address)) {
	len = sprintf(buffer, s, arg);
	buffer[len] = '\0';
	buginf("\nEGP: %i state is %s, %s",
	    en->address, egp_statestring(en->state), buffer);
    }
}

/*
 * egp_router
 * The process that handles EGP's time driven events.
 */

static void
egp_router (pdbtype *pdb)
{
    neighbortype *en, *prev;
    int i, delta_invalidtime, delta_flushtime, interval;
    sys_timestamp now, wakeup, tmp_time;
    peertype *peer;

    TIMER_START(pdb->timer, pdb->invalidtime);
    while (TRUE) {
	process_sleep_for(ONESEC >> 1);
	for (i = 0; i < NETHASHLEN; i++) {
	    prev = (neighbortype *) & egp_neighbors[i];
	    en = prev->next;
	    while (en) {
		if (en->pdb && en->pdb != pdb)
		    goto egploop;

		/*
		 * Flush non-permanent dead entries ones.
		 * Try restarting permanent neighbors.
		 */
		if (en->flags & EN_DEAD) {
		    if (en->flags & EN_KEEP) {
			if (TIMER_RUNNING_AND_AWAKE(en->retrytime))
			    TIMER_STOP(en->retrytime);
			if (!TIMER_RUNNING(en->retrytime))
			    egp_start(en);
		    } else {
			prev->next = en->next;
			en->next = NULL;
			enqueue(&deadneighborQ, en);
			en = prev;
		    }
		    goto egploop;
		}

		/*
		 * Do time dependent actions.  Order of Poll and Hello
		 * is important for suppressing HELLO's after POLL's.
		 */
		if (TIMER_RUNNING_AND_AWAKE(en->poll_timer))	/* t2 */
		    timer_poll(en);
		if (TIMER_RUNNING_AND_AWAKE(en->hello_timer))	/* t1 */
		    timer_hello(en);
		if (TIMER_RUNNING_AND_AWAKE(en->abort_timer))	/* t3 */
		    egp_stop(en);

		/*
		 * The bottom of the while loop
		 */
egploop:
		prev = en;
		en = en->next;	
	    }
	}

	/*
	 * Age old entries if timer has expired
	 */
	if (!TIMER_RUNNING_AND_SLEEPING(pdb->timer)) {
	    TIMER_STOP(pdb->timer);
	    age_all_routes(pdb);
	    (void) process_may_suspend();
	    /*
	     * Figure out when we should wake up again.
	     * To do this: we've recorded update times in peer->timer
	     * Calculate invalid times and flushtimes for each neighbor and
	     * find the earliest one which hasn't already passed.
	     */
	    GET_TIMESTAMP(now);
	    TIMER_STOP(wakeup);
	    for (i = 0; i < NETHASHLEN; i++) {
		peer = pdb->peercache[i];
		prev = (neighbortype *) & egp_neighbors[i];
		while (peer) {
		    for (en = prev->next; en; en = en->next)
			if (en->address == peer->source)
			    break;
		    if (!en) {
			peer = peer->next;
			continue;
		    }
		    interval = en->poll_int * ONESEC;
		    delta_invalidtime = pdb->invalidtime % interval;
 		    delta_flushtime = pdb->flushtime % interval;
		    tmp_time = TIMER_ADD_DELTA(peer->timer, delta_invalidtime);
		    if (TIMER_LATER(tmp_time, now)){
			wakeup = TIMER_SOONEST(wakeup, tmp_time);
		    } else {
			tmp_time = TIMER_ADD_DELTA(peer->timer,
						   interval + delta_invalidtime);
			if (TIMER_LATER(tmp_time, now))
			    wakeup = TIMER_SOONEST(wakeup, tmp_time);
		    }

		    tmp_time = TIMER_ADD_DELTA(peer->timer, delta_flushtime);
		    if (TIMER_LATER(tmp_time, now)) {
			wakeup = TIMER_SOONEST(wakeup, tmp_time);
		    } else {
			tmp_time = TIMER_ADD_DELTA(peer->timer,
						  interval + delta_flushtime);
			if (TIMER_LATER(tmp_time, now))
			    wakeup = TIMER_SOONEST(wakeup, tmp_time);
		    }
		    peer = peer->next;
		}
	    }

	    /*
	     * Wakeups must be at least five seconds apart.
	     */
	    tmp_time = TIMER_ADD_DELTA(now, 5*ONESEC);
	    if (TIMER_EARLIER(wakeup, tmp_time)) {
		COPY_TIMESTAMP(tmp_time, pdb->timer);
	    } else {
		COPY_TIMESTAMP(wakeup, pdb->timer);
	    }
	}
    }
}

/*
 * egp_cleanup
 * Cease and force to IDLE all our neighbors if we've been killed.
 */

static void
egp_cleanup (pdbtype *pdb)
{
    neighbortype *en, *pred;
    int i;

    for (i = 0; i < NETHASHLEN; i++) {
	pred = (neighbortype *) & egp_neighbors[i];
	for (en = pred->next; en; pred = en, en = en->next) {
	    if (en->pdb == pdb) {
		egp_stop(en);
		egp_changestate(en, IDLE_STATE);
		pred->next = en->next;
		en->next = NULL;
		enqueue(&deadneighborQ, en);
		en = pred;
	    }
	}
    }
    unqueue(&protoQ, pdb);
    enqueue(&deadpdbQ, pdb);
}

/*
 * timer_hello
 * The hello timer (t1) expired.
 */

static void
timer_hello (register neighbortype *en)
{
    TIMER_UPDATE(en->hello_timer, en->hello_int*ONESEC);
    switch (en->state) {
	case ACQUISITION_STATE:
	    egp_na_message(en,EGP_NA_REQUEST,en->mode,
					en->hello_int,en->poll_int);
	    break;
	case DOWN_STATE:
	case UP_STATE:
	    reachable_check(en);
	    if (!(en->flags & EN_NOHELLO))
		egp_nr_message(en,EGP_NR_HELLO);
	    en->flags &= ~EN_NOHELLO;
	    break;
	case CEASE_STATE:
	    egp_na_message(en,EGP_NA_CEASE,en->mode,0,0);
	    break;
	case IDLE_STATE:
	default:
	    TIMER_STOP(en->hello_timer);
	    return;
    }
}

/*
 * timer_poll
 * The poll timer (t2) expired.
 * Meaningful only in UP_STATE.
 */

static void
timer_poll (register neighbortype *en)
{
    if (en->state == UP_STATE) {
	egp_poll_request(en);
	TIMER_UPDATE(en->poll_timer, en->poll_int*ONESEC);
	en->flags |= EN_NOHELLO;	/* suppress next HELLO */
    }
}

/*
 * egp_stop
 * The abort timer (t3) expired, or a stop event has been requested
 */

static void
egp_stop (register neighbortype *en)
{
    TIMER_START(en->retrytime, RETRYTIME);
    if (en->flags & EN_STARTED)
	en->flags &= ~EN_STARTED;
    switch (en->state) {
	case DOWN_STATE:
	case UP_STATE:
	    egp_changestate(en, CEASE_STATE);
	    egp_na_message(en,EGP_NA_CEASE,EGP_NAS_HALTING,0,0);
	    break;
	case IDLE_STATE:
	case ACQUISITION_STATE:
	case CEASE_STATE:
	    egp_changestate(en, IDLE_STATE);
	    break;
    }
}

/*
 * egp_start
 * A start event has been requested for a neighbor
 */

static void
egp_start (neighbortype *en)
{
    TIMER_STOP(en->retrytime);		/* clear retry timer */
    switch (en->state) {
	case IDLE_STATE:		/* Take care of local AS no. change &*/
	    en->mysystem = egp_asystem; /* fall thru to continue processing */
	case ACQUISITION_STATE:
	case DOWN_STATE:
	case UP_STATE:
	    en->flags &= ~EN_DEAD;
	    en->myaddr = egp_lookup_local_address(en->address);
	    egp_changestate(en, ACQUISITION_STATE);
	    egp_na_message(en,EGP_NA_REQUEST,en->mode,
					en->hello_int,en->poll_int);
	    break;
	case CEASE_STATE:
	default:
	    break;
    }
}

/*
 * sendreply
 * Send an EGP reply message
 */

static void
sendreply (neighbortype *en, paktype *pak, int len)
{
    register egptype *egp;

    en->outmsgs++;
    egp = (egptype *) ip2egp(pak);
    if (egp->type == EGP_TYPE_ERROR)
	en->errssent++;
    egp->version = EGP_VERSION;
    egp->asystem = en->mysystem;
    egp->checksum = 0;
    egp->checksum = ipcrc((ushort *)egp,len);
    len += IPHEADERBYTES(NOPT);
    if (egp_debug
	&& (egp_debug_addr == 0 || egp_debug_addr == en->address))
	egp_print(egp, en->myaddr, en->address);
    ipwrite(pak, len, EGP_PROT, en->myaddr, en->address, NULL, NULL,
							&ipdefaultparams);
}

/*
 * egp_na_message
 * Send a Neighbor Acquisition message
 */

#define NASIZE 14

static void
egp_na_message (neighbortype *en, register int code, int status,
		int hello, int poll)
{
    paktype *replypak;
    register egptype *re;
    int size;

    replypak = getbuffer(IPHEADERBYTES(NOPT) + NASIZE);
    if (!replypak) {
	en->outerrs++;
	return;	
    }
    re = (egptype *) ip2egp(replypak);
    re->type = EGP_TYPE_NA;
    re->code = code;
    re->status = status;
    if ((code == EGP_NA_CONFIRM) || (code == EGP_NA_CEASEACK))
	re->sequence = en->rcvseq;
    else
	re->sequence = en->sndseq;
    size = NASIZE;
    if ((code == EGP_NA_REQUEST) || (code == EGP_NA_CONFIRM)) {
 	re->t.na.pollint = poll;
	re->t.na.helloint = hello;
    }
    else
	size -= 4;
    sendreply(en, replypak, size);
}

/*
 * egp_nr_message
 * Send an EGP "HELLO" or "IHU" message.
 * Don't send HELLO if we are passive wrt the neighbor. 
 */

#define NRSIZE 10

static void
egp_nr_message (neighbortype *en, int code)
{
    paktype *replypak;
    register egptype *re;
    register int status;

    if ((code == EGP_NR_HELLO) && (en->flags & EN_PASSIVE))
	return;
    replypak = getbuffer(IPHEADERBYTES(NOPT) + NRSIZE);
    if (!replypak) {
	en->outerrs++;
	return;
    }
    re = (egptype *) ip2egp(replypak);
    re->type = EGP_TYPE_NR;
    re->code = code;
    switch (en->state) {
	case UP_STATE:
	    status = EGP_NRS_UP;
	    break;
	case DOWN_STATE:
	    status = EGP_NRS_DOWN;
	    break;
	default:
	    status = EGP_NRS_UNKNOWN;
	    break;
    }
    re->status = status;
    if (code == EGP_NR_HELLO)
	re->sequence = en->sndseq;
    else
	re->sequence = en->rcvseq;
    sendreply(en, replypak, NRSIZE);
}

/*
 * egp_poll_request
 * Ask for EGP routing information
 */

#define POLLREQSIZE 16

static void
egp_poll_request (neighbortype *en)
{
    register egptype *re;
    register paktype *replypak;
    register int status;

    replypak = getbuffer(IPHEADERBYTES(NOPT) + POLLREQSIZE);
    if (!replypak) {
	en->outerrs++;
	return;
    }
    re = (egptype *) ip2egp(replypak);
    re->type = EGP_TYPE_POLL;
    re->code = EGP_POLL_CODE;
    switch (en->state) {
	case UP_STATE:
	    status = EGP_POLLS_UP;
	    break;
	case DOWN_STATE:
	    status = EGP_POLLS_DOWN;
	    break;
	default:
	    status = EGP_POLLS_INDET;
	    break;
    }
    re->status = status;
    re->sequence = en->sndseq++;	/* send sequence updated only here */
    re->t.pl.net = get_majornet(en->address);
    sendreply(en, replypak, POLLREQSIZE);
}

/*
 * egp_update
 * Send a routing update in response to a poll request
 */

/*
 * Some notes on the updates we send:
 *
 * We send a null update if we are not on the same net as the requestor,
 * that is, neither internal nor external gateways are mentioned.
 *
 * We advertise only internal gateways and networks.
 *
 * We advertise only one gateway, ourself.  Distance 0 networks are
 * specified by the "network" command and need not be directly connected.
 * A configurable non-zero distance is used for redistributed networks.
 *
 * A design problem:  updates are by organized by gateways.  Our data
 * structure is organized by networks.
 */

static void
egp_update (neighbortype *en, egptype *egp, boolean unsolicited)
{
    paktype *replypak;
    egptype *re;
    ndbtype *ndb;
    int i, j;
    uchar *cp;
    int len, status, bytesleft;
    boolean reject, metric_changed;
    uchar *distancecount, *netcount, defaultmetric, new_defmetric;
    pdbtype *pdb;
    thirdpartytype *tp, *found;
    idbtype *idb = NULL;
    ipaddrtype naddress;
    route_maptype *map;
    pdbtype *src_pdb;

    pdb = en->pdb;
    if (!pdb) {
	errmsg(&msgsym(NOPDB, EGP), en->address);
	return;
    }
    replypak = getbuffer(MAXEGPDATASIZE);
    if (!replypak) {
	en->outerrs++;
	return;
    }

    re = (egptype *) ip2egp(replypak);
    re->type = EGP_TYPE_UPDATE;
    re->code = EGP_POLL_CODE;
    switch (en->state) {
	case UP_STATE:
	    status = EGP_POLLS_UP;
	    break;
	case DOWN_STATE:
	    status = EGP_POLLS_DOWN;
	    break;
	default:
	    status = EGP_POLLS_INDET;
	    break;
    }
    if (unsolicited)
	status += EGP_POLLS_UNSOLICITED;
    re->status = status;
    re->t.pl.nintgw = 0;		/* no interior gws, yet */
    re->t.pl.nextgw = 0;		/* no exterior gws, yet */
    if (egp) {
	re->t.pl.net = get_majornet(egp->t.pl.net);/* reply for same net */
	re->sequence = egp->sequence;	/* use same sequence in replying */
    } else {
	re->t.pl.net = get_majornet(en->myaddr); /* reply for this net */
	re->sequence = en->rcvseq;
    }
    len = 16;				/* header is 16 bytes so far */
    cp = re->t.pl.data;			/* data starts here */
    bytesleft = MAXEGPDATASIZE - IPHEADERBYTES(NOPT) - len; /* data bytes */

    /*
     * If we're not on the same network that the request was made for,
     * send a null update message back.
     */
    if (get_majornet(en->myaddr) != re->t.pl.net) {
	sendreply(en, replypak, len);
	return;
    }

    /*
     * Figure out if there is an output filter on the outbound interface.
     * Minor kludge here since first_hop overwrites neighbor in the case
     * that there's more than one hop.  Don't overwrite en->address.
     */
    naddress = en->address;
    idb = reg_invoke_ip_first_hop(&naddress, naddress, IP_PATH_DEFFLAGS);

    /*
     * Now that we are actually going to send a reply, allocate space for
     * the third party temprorary data structures.  While running the
     * third party tree, go ahead and set up the headers for directly
     * connected networks.
     */
    for (tp = en->thirdp; tp; tp = tp->next) {
	if ((tp->buffer = malloc(THIRDDATASIZE)) == NULL) {
	    goto egpupdateabort;
	}
	tp->p = tp->buffer;
	tp->bytesleft = THIRDDATASIZE - 3;

	tp->p += host2egp(tp->p, tp->address, &tp->bytesleft);
	tp->distances = tp->p;
	*tp->p++ = 1;
	*tp->p++ = 0;
	tp->netcount = tp->p;
	*tp->p++ = 0;
	tp->nnets = 0;
    }

    /*
     * List the interior gateways.  All networks specified by the
     * "network" command are advertised as distance zero, even if they
     * aren't directly connected.  We don't access check these networks
     * since they've been explicitly specified.  For each entry we find
     * that should appear in the update, check to see if the route is
     * through one of the routers we are third partying for.  If so,
     * write into it's scratch area, instead of into the working update
     * for this process.  When we are finished, we'll copy the
     * third-party stuff back onto the end of the update.
     */
    cp += host2egp(cp, en->myaddr, &bytesleft);
    re->t.pl.nintgw += 1;		/* count an interior gateway */
    distancecount = cp;			/* remember this location */
    *cp++ = 1;				/* number of distances so far */
    *cp++ = 0;				/* distance == 0 */
    netcount = cp;			/* count of nets at this distance */
    *cp++ = 0;				/* #nets == 0 */
    bytesleft -= 3;
    for (i = 0; i < pdb->netcount; i++) {
	ndb = (ndbtype *) net_lookup(pdb->networks[i], FALSE);
	if (ndb) {
	    src_pdb = ip_get_src_pdb(pdb, ndb, FALSE);
	    if (martian(ndb->netnumber, FALSE))
		continue;
	    if (!pdb_outfilter(pdb, idb, src_pdb, ndb->netnumber))
		continue;
	    if (!pdb_routemap(pdb, src_pdb, ndb))
		continue;
	    for (found = NULL, tp = en->thirdp; tp && !found; tp = tp->next)
		for (j = 0; (j < ndb->routecount) && !found; j++)
		    if (tp->address == ndb->rdb[j]->gateway)
			found = tp;
	    if (found) {
		(*found->netcount)++;
		found->nnets++;
		found->p += net2egp(found->p, ndb->netnumber,
				    &found->bytesleft);
	    } else {
		(*netcount)++;
		cp += net2egp(cp, ndb->netnumber, &bytesleft);
		if (checksend(en, replypak, cp, bytesleft)) 
		    goto egpupdateabort;
	    }
	}
    }

    /*
     * If we're supposed to originate a default route, add it to the
     * update.  Use the default-metric as the metric.
     */
    if (pdb->default_originate && pdb_outfilter(pdb, idb, NULL, 0)) {
        defaultmetric = pdb->defaultmetricflag ? pdb->default_metric : 3;
	(*distancecount)++;		/* another distance */
	*cp++ = defaultmetric;
	netcount = cp;			/* count of nets at this distance */
	*cp++ = 0;			/* #nets == 0 */
	bytesleft -=2;
	for (tp = en->thirdp; tp; tp = tp->next) {
	    (*tp->distances)++;		/* another distance */
	    *tp->p++ = defaultmetric;
	    tp->netcount = tp->p;	/* count of nets at this distance */
	    *tp->p++ = 0;		/* #nets == 0 */
	    tp->bytesleft -=2;
	}
	/*
	 * If we have a default ndb, consider it for third partying.
	 */
	found = NULL;
	if (default_ndb) {
	    if (default_ndb->rdb[0]->gateway != en->address) {
		for (tp = en->thirdp; tp && !found; tp=tp->next)
		    if (tp->address == default_ndb->rdb[0]->gateway)
			found = tp;
		if (found) {
		    if (*found->netcount == 255) {
			(*found->distances)++;		/* another distance */
			*found->p++ = defaultmetric;
			found->netcount = found->p;	/* count of nets */
			*found->p++ = 0;		/* #nets == 0 */
			found->bytesleft -=2;
		    }
		    (*found->netcount)++;
		    found->nnets++;
		    found->p += net2egp(found->p, 0,
					&found->bytesleft);
		}
	    } else
		found = en->thirdp;
	}
	if (!found) {
	    if (*netcount == 255) {
		(*distancecount)++;			/* another distance */
		*cp++ = defaultmetric;
		netcount = cp;				/* count of nets */
		*cp++ = 0;				/* #nets == 0 */
		bytesleft -=2;
	    }
	    (*netcount)++;
	    cp += net2egp(cp, 0, &bytesleft);
	    if (checksend(en, replypak, cp, bytesleft))
		goto egpupdateabort;
	}
    }

    /*
     * Now advertise any networks we are redistributing (distflag != 0).
     * We explicitly do *not* readvertise EGP derived routes back into the
     * same autonomous system.  We *can* mix EGP routes between different
     * autonomous systems, however.  Be very careful to not advertise any
     * network we've already advertised by the "network" command.
     * EXCEPTION: If we're running as EGP AS 0, we're a core gateway and we
     * echo back to ourselves.  In either case, DON'T advertise the same
     * route back to the particular gateway.
     */
    if (pdb->distflag) {
        defaultmetric = pdb->defaultmetricflag ? pdb->default_metric : 3;
	(*distancecount)++;		/* another distance */
	*cp++ = defaultmetric;
	netcount = cp;			/* count of nets at this distance */
	*cp++ = 0;			/* #nets == 0 */
	bytesleft -=2;
	for (tp = en->thirdp; tp; tp = tp->next) {
	    (*tp->distances)++;		/* another distance */
	    *tp->p++ = defaultmetric;
	    tp->netcount = tp->p;	/* count of nets at this distance */
	    *tp->p++ = 0;		/* #nets == 0 */
	    tp->bytesleft -=2;
	}
	for (i = 0; i < NETHASHLEN; i++) {
	    for (ndb = (ndbtype *) nettable[i]; ndb; ndb = ndb->next) {
	        if (!ip_redist_flag(ndb, pdb))
		    continue;
		if (martian(ndb->netnumber, FALSE))
		    continue;
		src_pdb = ip_get_src_pdb(pdb, ndb, FALSE);
		if (src_pdb->proctype & PROC_OSPF)
		    if (!pdb->routemap[src_pdb->index] && 
			!(ospf_check_match_routetype(pdb, src_pdb, ndb)))
		        continue;
		if (!pdb_outfilter(pdb, idb, src_pdb, ndb->netnumber))
		    continue;
		if (!pdb_routemap(pdb, src_pdb, ndb))
		    continue;
		if (ndb->pdbindex != pdb->index)
		    ndb->advertisingmask |= pdb->mask;

		for (j = 0, reject = FALSE; j < pdb->netcount; j++) {
		    if (ndb->netnumber == pdb->networks[j]) {
			reject = TRUE;
			break;
		    }
		}
		for (j = 0; j < ndb->routecount; j++) {
		    if (ndb->rdb[j]->gateway == en->address) {
			reject = TRUE;
			break;
		    }
		}
		if (reject || (ndb->metric == METRIC_INACCESSIBLE))
		    continue;

		metric_changed = FALSE;
		if ((map = pdb->route_map) &&
		    (map->set_list.set_flags & ROUTE_MAP_METRIC_SET))
		    new_defmetric = map->set_list.metric.value.scalar;
		else
		    new_defmetric = pdb->defaultmetricflag ?
			pdb->default_metric : 3;
		if (new_defmetric != defaultmetric) {		    
		    defaultmetric = new_defmetric;
		    metric_changed = TRUE;
		}
		for (found = NULL, tp = en->thirdp; tp && !found; tp=tp->next)
		    for (j = 0; (j < ndb->routecount) && !found; j++)
			if (tp->address == ndb->rdb[j]->gateway)
			    found = tp;
		if (found) {
		    if (*found->netcount == 255 || metric_changed) {
			if (*found->distances == 255) {
			    if (egp_debug)
				buginf("\nEGP: Too many metrics in update");
			    goto egpupdateabort;
			}
			(*found->distances)++;		/* another distance */
			*found->p++ = defaultmetric;
			found->netcount = found->p;	/* count of nets */
			*found->p++ = 0;		/* #nets == 0 */
			found->bytesleft -=2;
		    }
		    (*found->netcount)++;
		    found->nnets++;
		    found->p += net2egp(found->p, ndb->netnumber,
					&found->bytesleft);
		} else {
		    if (*netcount == 255 || metric_changed) {
			if (*distancecount == 255) {
			    if (egp_debug)
				buginf("\nEGP: Too many metrics in update");
			    goto egpupdateabort;
			}
			(*distancecount)++;		/* another distance */
			*cp++ = defaultmetric;
			netcount = cp;			/* count of nets */
			*cp++ = 0;			/* #nets == 0 */
			bytesleft -=2;
		    }
		    (*netcount)++;
		    cp += net2egp(cp, ndb->netnumber, &bytesleft);
		    if (checksend(en, replypak, cp, bytesleft))
			goto egpupdateabort;
		}
	    }
	}
    }

    /*
     * Tack the third party data onto the end of the update.
     */
    for (tp = en->thirdp; tp; tp = tp->next) {
	if ((tp->nnets > 0) && (tp->flags & TP_INTERNAL)) {
	    i = THIRDDATASIZE - tp->bytesleft;
	    if (checksend(en, replypak, cp, bytesleft - i)) 
		goto egpupdateabort;
	    bcopy(tp->buffer, cp, i);
	    re->t.pl.nintgw++;
	    bytesleft -= i;
	    cp += i;
	}
    }
    for (tp = en->thirdp; tp; tp = tp->next) {
	if ((tp->nnets > 0) && (tp->flags & TP_EXTERNAL)) {
	    i = THIRDDATASIZE - tp->bytesleft;
	    if (checksend(en, replypak, cp, bytesleft - i)) 
		goto egpupdateabort;
	    bcopy(tp->buffer, cp, i);
	    re->t.pl.nextgw++;
	    bytesleft -= i;
	    cp += i;
	}
    }

    /*
     * Calculate packet length.
     */
    len = cp - re->t.pl.data + 16;

    /*
     * Send packet off.
     */
    sendreply(en, replypak, len);

 egpupdateabort:
    for (tp = en->thirdp; tp; tp = tp->next) {
	if (tp->buffer) {
	    free(tp->buffer);
	    tp->buffer = NULL;
	}
    }
}

/*
 * checksend
 * Check if we should send the update message yet
 */

static boolean
checksend (neighbortype *en, paktype *replypak, uchar *cp, int bytesleft)
{
    int len;

    if (bytesleft > 10)
	return(FALSE);
    errmsg(&msgsym(TOOBIG, EGP), bytesleft);
    len = MAXEGPDATASIZE - IPHEADERBYTES(NOPT) - 16 - bytesleft;
    if (len < 0)
	len = MAXEGPDATASIZE-IPHEADERBYTES(NOPT);
    sendreply (en, replypak, len);
    return(TRUE);
}

/*
 * route_update
 * Update our routing tables with the current EGP NR packet
 */

static void
route_update (neighbortype *en, register egptype *egp, boolean debug)
{
    ipaddrtype gateway, network;
    int gws, distances, addresses, metric;
    register uchar *cp;
    pdbtype *pdb;
    int count = 0, num = 0;

    /*
     * Look up pdb if we're not debugging.
     */
    if (!debug) {
	pdb = en->pdb;
	if (!pdb) {
	    if (debug
		&& (egp_debug_addr == 0 || egp_debug_addr == en->address))
	        buginf("\nEGP: no AS for NR from %i, ignoring", en->address);
	    return;
	}
    } else
	pdb = NULL;
    cp = egp->t.pl.data;
    gws = egp->t.pl.nintgw + egp->t.pl.nextgw;
    if (pdb)
	mem_lock(pdb);
    while (gws--) {
	gateway = egp->t.pl.net;		/* set up net number */
	cp += egp2host(cp, &gateway);		/* create gateway address */
	distances = (int) *cp++;		/* total count of distances */
	while (distances--) {
	    metric = (int) *cp++;		/* current distance */
	    addresses = (int) *cp++;		/* nets at that distance */
	    while (addresses--) {
		cp += egp2net(cp, &network);
		if (!debug && en) {
						/* only update if not */
						/* default or if we don't */
						/* gen default */
		    if (network || !pdb->default_originate)
			(void) network_update(pdb, network,
					      IPADDR_ZERO, gateway,
					      en->idb, metric+1, 0,
					      en->address, en->yoursystem,
					      NULL, 0);
		    num++;
		}
		else {
		    if ((egp_debug_addr == 0
			 || egp_debug_addr == en->address))
			buginf("\n     Network %i via %i%s in %d hops",
			       network, gateway,
			       gws < egp->t.pl.nextgw ? "(e)" : "",
			       metric);
		}
 		count = (count + 1) % EGPROUTEDISMS;
 		if (!process_may_suspend() && count == 0) {
		    process_suspend();
		    if (pdb && mem_refcount(pdb) == 1)
			goto done;
		}
	    }
	}
	if (debug) {
	    process_suspend();			/* let others run */
	    if (pdb && mem_refcount(pdb) == 1)
		goto done;
	}
    }
    if (!debug && en && egp_debug
	&& (egp_debug_addr == 0 || egp_debug_addr == en->address))
	buginf("\nEGP: %i updated %d routes", en->address, num);
 done:
    if (pdb) 
	free(pdb);
}

/*
 * host2egp
 * Given an address and a destination byte pointer, copy the host portion
 * of the address to the destination and return count of bytes copied.
 */

static int
host2egp (register uchar *cp, ipaddrtype address, int *count)
{
    register uchar *dp;
    register int i, j;

    dp = (uchar *) & address;
    if (!(address & CLASS_A_BIT))
	i = 1;
    else if (!(address & CLASS_B_BIT))
	i = 2;
    else
	i = 3;
    dp += i;
    for (j = i; j < 4; j++)
	*cp++ = *dp++;
    *count -= (4-i);
    return(4-i);
}

/*
 * net2egp
 * Given an address and a destination byte pointer, copy the net portion
 * of the address to the destination and return count of bytes copied.
 */

static int
net2egp (register uchar *cp, ipaddrtype address, int *count)
{
    register uchar *dp;

    dp = (uchar *) & address;
    *cp++ = *dp++;
    if (!(address & CLASS_A_BIT)) {
	*count -= 1;
	return(1);
    }
    *cp++ = *dp++;
    if (!(address & CLASS_B_BIT)) {
	*count -= 2;
	return(2);
    }
    *cp++ = *dp++;
    *count -= 3;
    return(3);
}	

/*
 * egp2host
 * Given a pointer into an EGP packet, and a pointer to a long containing
 * the network portion of an address, return a host (gateway) address and
 * the count of bytes the EGP pointer advanced.
 */

static int
egp2host (register uchar *cp, ipaddrtype *address)
{
    register uchar *dp;
    register int i, j;

    dp = (uchar *) address;
    if (!(*address & CLASS_A_BIT))
	i = 1;
    else if (!(*address & CLASS_B_BIT))
	i = 2;
    else
	i = 3;
    dp += i;
    for (j = i; j < 4; j++)
	*dp++ =	*cp++;
    return(4-i);
}

/*
 * egp2net
 * Given a pointer into an EGP packet, and a pointer to a long in which
 * to deposit a network address, update the network address and return the
 * number of bytes the pointer advanced.
 */

static int
egp2net (register uchar *cp, ipaddrtype *address)
{
    register uchar *dp;
    register uchar first;

    *address = 0;
    dp = (uchar *) address;
    first = *cp++;
    *dp++ = first;
    if (!(first & CLASSA_BIT))
	return(1);
    *dp++ = *cp++;
    if (!(first & CLASSB_BIT))
	return(2);
    *dp++ = *cp++;
    return(3);
}	

/*
 * egp_print
 * Given an EGP datagram, print it
 */

static void
egp_print (register egptype *egp, ipaddrtype source, ipaddrtype destination)
{
    char buffer[100];
    int len;

    buginf("\nEGP: from %i to %i, version=%d, asystem=%u, sequence=%u",
	source, destination, egp->version, egp->asystem, egp->sequence);
    len = sprintf(buffer, "\n     Type=%s, Code=%s, Status=%d (%s)",
		typestring(egp->type), codestring(egp->type,egp->code),
		egp->status,statusstring(egp->type,egp->status));
    switch (egp->type) {
    case EGP_TYPE_NA:
	if ((egp->code == EGP_NA_REQUEST) || (egp->code == EGP_NA_CONFIRM))
	    len += sprintf(buffer+len, ", Hello=%d, Poll=%d",
				egp->t.na.helloint, egp->t.na.pollint);
	break;
    case EGP_TYPE_POLL:
	len += sprintf(buffer+len, ", Net=%i",egp->t.pl.net);
	break;
    case EGP_TYPE_ERROR:
	len += sprintf(buffer+len, ", Reason=%d", egp->t.err.reason);
	break;
    case EGP_TYPE_UPDATE:
	len += sprintf(buffer+len, ", IntGW=%d, ExtGW=%d, Net=%i",
		egp->t.pl.nintgw,egp->t.pl.nextgw,egp->t.pl.net);
	buffer[len] = '\0';
	buginf(buffer);
	if (!egpevent_debug)
	    route_update(NULL,egp,TRUE);
    	return;
    default:
	break;
    }
    buffer[len] = '\0';
    buginf(buffer);
}

/*
 * typestring
 * Return a pointer to a type name string
 */

static const char bogus[] = "????";

static const char *
typestring (int type)
{
    switch (type) {
	case EGP_TYPE_NA: return("ACQUIRE");
	case EGP_TYPE_NR: return("REACH");
	case EGP_TYPE_UPDATE: return("UPDATE");
	case EGP_TYPE_POLL: return("POLL");
	case EGP_TYPE_ERROR: return("ERROR");
	default: return(bogus);
    }
}

/*
 * codestring
 * Given a type and a code, return a pointer to a code namestring
 */

static const char *
codestring (int type, int code)
{
    switch (type) {
	case EGP_TYPE_NA:
	    switch (code) {
		case EGP_NA_REQUEST: return("REQUEST");
		case EGP_NA_CONFIRM: return("CONFIRM");
		case EGP_NA_REFUSE: return("REFUSE");
		case EGP_NA_CEASE: return("CEASE");
		case EGP_NA_CEASEACK: return("CEASE-ACK");
		default: return(bogus);    
	    }
	case EGP_TYPE_NR:
	    switch (code) {
		case EGP_NR_HELLO: return("HELLO");
		case EGP_NR_IHU: return("I-HEARD-YOU");
		default: return(bogus);    
	    }
	case EGP_TYPE_UPDATE:
	case EGP_TYPE_POLL:
	case EGP_TYPE_ERROR:
	    return("0");
	default: return(bogus);
    }
}

/*
 * statusstring
 * Return a pointer to the name of a status code
 */

static const char *
statusstring (int type, int status)
{
    switch (type) {
	case EGP_TYPE_NA:
	   switch (status) {
		case 0: return("UNSPECIFIED");
		case 1: return("ACTIVE-MODE");
		case 2: return("PASSIVE-MODE");
		case 3: return("NO-RESOURCES");
		case 4: return("PROHIBITED");
		case 5: return("HALTING");
		case 6: return("BAD-PARAMS");
		case 7: return("BAD-PROTOCOL");
		default: return(NULL);
	    }
	case EGP_TYPE_NR:
	case EGP_TYPE_UPDATE:
	case EGP_TYPE_POLL:
	    status &= ~128;		/* clear unsolicited bit */
	    switch (status) {
		case 0: return("INDETERMINATE");
		case 1: return("UP");
		case 2: return("DOWN");
		default: return(NULL);
	    }
	case EGP_TYPE_ERROR:
	    switch (status) {
		case 0: return("UNSPECIFIED");
		case 1: return("BAD-HEADER");
		case 2: return("BAD-DATA");
		case 3: return("NO-INFO");
		case 4: return("BAD-POLL-RATE");
		case 5: return("NO-RESPONSE");
		default: return(NULL);
	    }
	default:
	    return(NULL);
    }
}

/*
 * egp_changestate
 * Change a neighbor state, possibly printing debugging output.
 * Reset timers appropriately.
 */

static void
egp_changestate (neighbortype *en, int newstate)
{
    register ulong hello, poll, abort;
    int oldstate;
    pdbtype *pdb;    
    
    pdb = en->pdb;
    oldstate = en->state;
    en->state = newstate;
    if ((egp_debug || egpevent_debug)
	&& (egp_debug_addr == 0 || egp_debug_addr == en->address))
	if (oldstate != newstate)
	    buginf("\nEGP: %i going from %s to %s", en->address,
			egp_statestring(oldstate), egp_statestring(newstate));
    if ((newstate == IDLE_STATE) && (oldstate != IDLE_STATE)) {
	if (en->flags & EN_STARTED)
	    en->flags &= ~EN_STARTED;
    }
    hello = poll = abort = 0;
    if ((newstate != UP_STATE) && (oldstate == UP_STATE)) {
	reg_invoke_egp_neighbor_lost(en->address);
	en->wentdn++;
    }
    switch (newstate) {
	case ACQUISITION_STATE:
	    hello = P3;
	    poll = 0;
	    abort = P5;
	    break;
	case DOWN_STATE:
	    hello = en->hello_int;
	    poll = 0;
	    abort = P5;
	    en->reachcount = 0;		/* reset reachability window ... */
	    en->reachshift = 0;		/* .. when entering down state. */
	    break;
	case UP_STATE:
	    poll = en->poll_int;
	    en->cameup++;
	    GET_TIMESTAMP(en->started);
	    break;
	case CEASE_STATE:
	    hello = P3;
	    poll = 0;
	    abort = P5;
	    break;
	case IDLE_STATE:
	    en->hello_int = pdb->egp_hello;
	    en->poll_int = pdb->egp_poll;
	    en->flags |= EN_DEAD;
	    break;
	default:
	    en->flags |= EN_DEAD;
	    break;
    }
    if (hello) 
	TIMER_START(en->hello_timer, hello*ONESEC);
    if (poll)
	TIMER_START(en->poll_timer, poll*ONESEC);
    if (abort)
 	TIMER_START(en->abort_timer, abort*ONESEC);
}

/*
 * egp_statestring
 * Given a state code, return a pointer to a name string
 */

static const char *
egp_statestring (int state)
{
    switch (state) {
	case IDLE_STATE: return("IDLE");
	case ACQUISITION_STATE: return("ACQUIRE");
	case DOWN_STATE: return("DOWN");  
	case UP_STATE: return("UP");
	case CEASE_STATE: return("CEASE");
	default: return(bogus);
    }
}

/*
 * show_neighbors
 * Dump contents of neighbor table to screen
 */

static const char egphead[] =
" EGP Neighbor     FAS/LAS  State    SndSeq RcvSeq Hello  Poll j/k Flags";
static const char egp3phead[] = " EGP Neighbor     Third Party";

void
egp_show_neighbors (parseinfo *csb)
{
    register int i, len, header;
    register neighbortype *en;
    register thirdpartytype *tp;
    int ticks;

    automore_enable(NULL);
    header = FALSE;
    for (i = 0; i < NETHASHLEN; i++) {
	en = egp_neighbors[i];
	while (en) {
	    if (!header) {
		printf("\nLocal autonomous system is %d\n",
			egp_asystem);
		automore_header(egphead);
		header = TRUE;
	    }
	    printf("\n%c%16i%-4d/%4d ",
			((en->flags & EN_DEAD) ? ' ' : '*'),
			 en->address, en->yoursystem, en->mysystem);
	    len = printf(egp_statestring(en->state));
	    if (en->state == UP_STATE)
		len += printf(" %#-5TE ", en->started);
	    len = 9-len;
  	    if (len > 0)
	        printf("%*c", len, ' ');
	    len = egpbits[(en->reachshift & 0xF)];
	    printf("%-6u %-6u %-5d %-5d %-3d ", en->sndseq, en->rcvseq,
			en->hello_int, en->poll_int, len);
	    if ((en->flags & EN_KEEP) && !(en->flags & EN_TEMP))
		printf("Perm");
	    else
		printf("Temp");
	    if (en->flags & EN_DEAD) {
		if (TIMER_RUNNING(en->retrytime)) {
		    ticks = (TIME_LEFT_SLEEPING(en->retrytime) / ONESEC);
		} else
		    ticks = 0;
		printf(", %d sec", ticks);
	    } else {
		if (en->flags & EN_PASSIVE)
		    printf(", Pass"); else printf(", Act");
	    }
	    en = en->next;
	}
    }
    
    /*
     * Display any third party information
     */
    header = FALSE;
    for (i = 0; i < NETHASHLEN; i++) {
	for (en = egp_neighbors[i]; en; en = en->next) {
	    if (!en->thirdp)
		continue;
	    if (!header) {
		automore_header(egp3phead);
		header = TRUE;
	    }		
	    printf("\n%c%16i", ((en->flags & EN_DEAD) ? ' ' : '*'),
		   en->address);
	    for (tp=en->thirdp; tp; tp=tp->next) {
		printf(" %i", tp->address);
		if (tp->flags & TP_EXTERNAL)
		    printf("(e)");
	    }
	}
    }
    automore_disable();
}

/*
 * egp_show_traffic
 * EGP statistics
 */

static void
egp_show_traffic (void)
{
    printf("\n\nEGP statistics:");
    printf("\n  Rcvd: %lu total, %lu format errors, %lu checksum errors, %lu no listener",
 		egp_traffic.inputs, egp_traffic.formaterr,
		egp_traffic.checksumerr, egp_traffic.noport);
    printf("\n  Sent: %lu total", egp_traffic.outputs);
}

/* 
 * egp_showipproto
 * Show egp specific information
 */
static void
egp_showipproto (pdbtype *pdb) 
{
    neighbortype *neigh;
    int i, headerflag;

    printf("\n  Default network %sgenerated",
	   pdb->default_originate ? "" : "not ");
    if (pdb->defaultmetricflag)
	printf("\n  Default redistribution metric is %d", pdb->default_metric);
    showipproto_redistribute(pdb);

    for (i = 0, headerflag = TRUE; i < NETHASHLEN; i++) {
	if (headerflag) {
	    automore_header(showipproto_neighborheader);
	    headerflag = FALSE;
	}
	neigh = egp_neighbors[i];
	while (neigh) {
	    if (neigh->pdb == pdb)
		printf("\n    %i",neigh->address);
	    neigh = neigh->next;
	}
    }
    automore_header(NULL);
}

/*
 * egp_pdb_init
 * Initialize an EGP Protocol Descriptor Block
 */

static pdbtype *
egp_pdb_init (ushort asystem)
{
    pdbtype *pdb;

    /*
     * Sanity check to ensure LAS is already known.
     */
    if (egp_asystem == 0) {
	printf(egpmust);
	return(NULL);
    }

    /*
     * Create the pdb.
     */
    pdb = malloc(sizeof(pdbtype));
    if (pdb == NULL)
	return(NULL);
    /*
     * Allocate an unused router process flag
     */
    if (allocate_pdbindex(pdb) == -1) {
	free(pdb);
	return(NULL);
    }

    pdb->name = "egp";
    pdb->family = PDB_IP;
    pdb->pname = "EGP Router";
    pdb->process = egp_router;
    pdb->cleanup = egp_cleanup;
    pdb->neighbor = egp_neighbor;
    pdb->showipproto = egp_showipproto;
    pdb->majorupdate = NULL;
    pdb->holddown = simple_holddown;
    if (asystem)
	pdb->distflag = 0;
    else
	pdb->distflag = pdb->mask;
    pdb->proctype = PROC_EGP;
    pdb->redistallowed = PROC_REDISTMASK;
    pdb->broadcasttime = pdb->def_broadcasttime = EGP_CYCLETIME;
    pdb->invalidtime = pdb->def_invalidtime = EGP_ROUTEFLUSHTIME;
    pdb->holdtime = pdb->def_holdtime =  EGP_BLINDTIME;
    pdb->flushtime = pdb->def_flushtime = EGP_NETFLUSHTIME;
    pdb->distance = pdb->def_distance = EGP_DISTANCE;
    pdb->distance_cmp = iprouting_distcmp_generic;
    pdb->multipath = pdb->def_multipath = 1;
    pdb->egp_hello = P1;
    pdb->egp_poll = P2;
    pdb->asystem = asystem;
    enqueue(&protoQ, pdb);
    if (!egp_update_running) {
	(void) cfork(egp_upd_process,0L,0,"EGP Updates", 0);
	egp_update_running = TRUE;
    }
    return(pdb);
}

/*
 * Initialize EGP global data structures.
 */
static void
egp_init (subsystype *subsys)
{
    int i;

    queue_init(&egpQ, 0);
    queue_init(&deadneighborQ, 0);
    queue_init(&deadtpQ, 0);
    egp_update_running = FALSE;
    egp_asystem = 0;			/* don't know autonomous system */
    memset(&egp_traffic, 0, sizeof(egp_traffic));
    for (i = 0; i < NETHASHLEN; i++)
	egp_neighbors[i] = NULL;	/* no neighbors yet */

    /*
     * Parser chain initialization
     */
    egp_parser_init();

    /*
     * Register some functions
     */
    ip_set_proto_outcounter(EGP_PROT, &egp_traffic.outputs);
    reg_add_ip_enqueue(EGP_PROT, egp_process, "egp_process");
    reg_add_ip_show_traffic(egp_show_traffic, "egp_show_traffic");
    reg_add_iprouting_pdb_init(PROC_EGP, egp_pdb_init, "egp_pdb_init");
}

/*
 * EGP subsystem header
 */

#define EGP_MAJVERSION 1
#define EGP_MINVERSION 0
#define EGP_EDITVERSION  1

SUBSYS_HEADER(egp, EGP_MAJVERSION, EGP_MINVERSION,
	      EGP_EDITVERSION, egp_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iprouting", "req: iprouting");
