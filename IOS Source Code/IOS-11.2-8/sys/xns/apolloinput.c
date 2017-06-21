/* $Id: apolloinput.c,v 3.3.60.9 1996/08/28 13:24:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/xns/apolloinput.c,v $
 *------------------------------------------------------------------
 * apolloinput.c -- handle Apollo domain input
 *
 * July 1989, Bill Westfield.  Largely a copy of xnsinput.c
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: apolloinput.c,v $
 * Revision 3.3.60.9  1996/08/28  13:24:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.60.8  1996/08/03  23:43:53  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.3.60.7  1996/07/13  11:42:51  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.3.60.6  1996/07/02  23:11:59  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.3.60.5  1996/05/17  12:17:43  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.46.3  1996/05/06  00:11:32  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.3.46.2  1996/04/26  08:00:13  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.46.1  1996/04/03  22:20:20  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.60.4  1996/05/04  02:18:25  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Enqueue to routable packet received on bridged interface to the
 *   appropriate queue instead of bridgeQ.
 * - Restore input to the BVI if necessary when copying packet for
 *   process-level.
 *
 * Name Access List
 *
 * Revision 3.3.60.3  1996/04/29  02:42:28  bcole
 * CSCdi55590:  configuring ipx routing  breaks IP ping
 * Branch: California_branch
 *
 * Add 'ipdiag' to the list of subsystem dependencies for subsystems which
 * support the ping command.
 *
 * Revision 3.3.60.2  1996/04/17  13:52:20  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/03/18  22:52:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.3  1996/03/13  02:14:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.24.2  1996/03/07  11:21:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.24.1  1996/02/20  21:58:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  19:20:49  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:53:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:57:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/09/11  23:58:27  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.6  1995/09/01 23:06:40  asastry
 * CSCdi39698:  WFQ: Display info (in "show queue") for non-IP protocols
 * as well
 *
 * Revision 2.5  1995/07/09  21:10:53  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Derive the CRB route/bridge behaviour from explicit, per-interface
 * route/bridge indicators rather than inferring it from protocol-
 * specific configuration.
 *
 * Revision 2.4  1995/06/19  04:10:05  speakman
 * CSCdi35697:  General TB and SRB regression failures
 * Make protocol-specific route/bridge test explicit rather than relying
 * on per-interface rxtypes indicators to provide backward compatible
 * SRB and TB behaviour.
 *
 * Revision 2.3  1995/06/18  06:19:48  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.2  1995/06/18 02:24:20  hampton
 * Change the name of an XNS timer variable to something that makes its
 * usage clearer.  While there, remove direct references to msclock.
 * [CSCdi36038]
 *
 * Revision 2.1  1995/06/07 23:26:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "interface.h"
#include "packet.h"
#include "linktype.h"
#include "../if/ether.h"
#include "../if/network.h"
#include "../if/priority_private.h"
#include "config.h"
#include "xns.h"
#include "config.h"
#include "../ui/debug.h"
#include "apollo.h"
#include "apollo_debug.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

/*
 * External symbols
 */
extern sys_timestamp apollo_timer;

/*
 * Storage
 */
static watched_queue *apolloQ;
ushort apollo_maxpaths;
ushort apollo_transid;
ulong apollo_stat[APOLLO_MAXTRAFFIC];


/*
 * The Apollo subsystem header
 */
#define APOLLO_MAJVERSION 1
#define APOLLO_MINVERSION 0
#define APOLLO_EDITVERSION  1

/*
 * AppleTalk must be initialized before Novell so that AppleTalk's ping
 * function gets a chance to test any address given as an argument
 * to the command first. This preserves pre-modular behavior.
 */
SUBSYS_HEADER(apollo, APOLLO_MAJVERSION, APOLLO_MINVERSION, APOLLO_EDITVERSION,
	      apollo_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, ipdiag, ipx, atalk", "req: griproute");

/*
 * fair conversation generator
 *
 *      level 2 protocol        1
 *      destination net         4
 *      destination host        6
 *      destination socket      2
 *      source net              4
 *      source host             6
 *      source socket           2
 */

static
short apollo_fair_queue_flow_id (paktype *pak)
{
    apollohdrtype *apollo;
    int         sum;

    pak->fair_weight = FAIR_QUEUE_BASE_WEIGHT;
    apollo = (apollohdrtype *)apolloheadstart(pak);
    /* hash protocol, source, and destination */
    sum = xnethash((uchar *)&apollo->pt, 25);
    return(fair_queue_sum(pak, sum));
}

/*
 * apollo_print_network_header
 *
 * Print Apollo header.
 */
static void apollo_print_network_header (paktype *pak, ushort **data)
{
    apollohdrtype *apollo;

    apollo = (apollohdrtype *)apolloheadstart(pak);
    printf("\n  source:%b socket %04x destination %b socket %04x protocol %02x",
	   apollo->snet, apollo->shost, apollo->ssock,
	   apollo->dnet, apollo->dhost, apollo->dsock,
	   apollo->pt);
}

/*
 * apollo_enable_test
 * Return TRUE if we're running Apollo
 * Called by SERVICE_PROTO_RUNNING.
 */

static boolean apollo_enable_test (void)
{
    return (apollo_running);
}

/*
 * apollo_routing_idb
 * Return TRUE iff apollo packets inbound on this interface should be
 * routed.
 */

static boolean apollo_routing_idb (idbtype *swidb)
{
    if ((global_crb_enable || global_irb_enable) && swidb->tbridge_on_swidb) {
	/*
	 * For CRB, just return the apollo route/bridge indicator
	 * if this interface is in a bridge group.  This is
	 * set by the "bridge <n> route apollo" command.
	 */
	return(swidb->tbridge_route_linktypes & LINK_APOLLO_RBI);
    } else {

	apollosb	*asb;

	asb = idb_get_swsb_inline(swidb, SWIDB_SB_APOLLO);

	/*
	 * Otherwise, just verify that apollo routing is enabled,
	 * and this interface is enabled for apollo.  This latter
	 * is set by the "apollo network" command.
	 */
	return (apollo_running && asb && asb->apollonet);
    }
}

/*
 * apollo_bridging_idb
 * Return TRUE iff apollo packets inbound on this interface should be
 * bridged.
 */

static boolean apollo_bridging_idb (idbtype *swidb)
{
    if (global_crb_enable) {
	/*
	 * For CRB, just return the complement of the apollo
	 * route/bridge indicator if this interface is in
	 * a bridge group.  This is cleared by the
	 * "no bridge <n> route apollo" command (the default).
	 */
	return (swidb->tbridge_on_swidb &&
		(!(swidb->tbridge_route_linktypes & LINK_APOLLO_RBI)));
    }else if (global_irb_enable) {
	return (swidb->tbridge_on_swidb &&
		!(swidb->tbridge_bridge_linktypes & LINK_APOLLO_RBI));
    } else {
	/*
	 * To support SRB and the archaic transparent bridging
	 * behaviour just check for the absence of apollo routing.
	 */
	return(!apollo_running);
    }
}
/*
 * apollo_rxtypes_update
 *
 * Maintain the route/bridge state of a protocol's rxtypes.
 * Called whenever protocol-specific configuration changes
 * or transparent-bridging-specific configuration changes
 * are made.  Updates a swidb's rxtype_flags to reflect the
 * new configuration.
 *
 * Determines whether the protocol should be routed and sets/
 * clears rxtypes_flags appropriately.  If Concurrent Routing
 * and Bridging is enabled, the decision is based on the
 * protocol configuration for the given swidb.  Otherwise,
 * the decision is based on the protocol configuration for
 * the router as a whole.
 */

void apollo_rxtypes_update (idbtype *swidb)
{
    boolean rte_ap, br_ap;

    if (global_crb_enable || global_irb_enable) {
	rte_ap = apollo_routing_idb(swidb);
	br_ap = apollo_bridging_idb(swidb);
    } else {
	rte_ap = apollo_running;
	br_ap = !rte_ap;
    }

    if (rte_ap) {
	SET_ROUTING_RXTYPE(swidb, RXTYPE_APOLLO);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_APOLLO);
    } else {
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_APOLLO);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_APOLLO);
    }

    if (br_ap) {
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_APOLLO);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_APOLLO);
    } else {
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_APOLLO);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_APOLLO);
    }

    if (swidb->hwptr->setup_bridging)
	(*(swidb->hwptr->setup_bridging))(swidb->hwptr);

    reg_invoke_bridge_setup();	/* sse_bridging_setup_enqueue() */
}

/*
 * apollo_showproto
 * Interface specific portion of "show protocols" commands
 * Called by SERVICE_SHOWPROTO
 */

static void apollo_showproto (idbtype *idb)
{
    apollosb	*asb;

    if (idb == NULL) {
	if (RUNNING(LINK_APOLLO))
	    printf("\n  Apollo routing is enabled");
	return;
    }
    asb = idb_get_swsb(idb, SWIDB_SB_APOLLO);
    if (asb) {
	if (RUNNING_IF(LINK_APOLLO, idb))
	    printf("\n  Apollo address is %x.%x",
		   asb->apollonet, apollo_address);
    }
}

/*
 * apollo_init_idb
 * Initialize interface for apollo defaults
 */

static void
apollo_init_idb (idbtype *idb, boolean setup_hwidb)
{
    apollosb		*asb;
    list_element	*l_elt;
    idbtype		*l_idb;
    boolean		found = FALSE;

    asb = idb_get_swsb(idb, SWIDB_SB_APOLLO);
    /*
     * If there isn't already an Apollo sub-block on this interface,
     * there isn't any need to go farther.
     */
    if (asb == NULL)
	return;

    asb->apollonet = 0L;
    asb->xnsrip.update_interval = RIP_DEFAULT_UPDATE;
    asb->xnsrip.static_rts = 0;
    asb->xnsrip.idb = idb;
    asb->xnsrip.type = APOLLO_TYPE;
    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_APOLLO, l_elt, l_idb) {
	if (l_idb == idb) {
	    found = TRUE;
	    break;
	}
    }
    if (!found)
	idb_add_swidb_to_list(idb, SWIDBLIST_APOLLO);
    apollo_rxtypes_update(idb);
}

/*
 * apollo_register_thyself
 */
static void apollo_register_thyself (void)
{
    apollo_parser_init();
    apollo_media_registry();

    /*
     * Register some functions
     */
    reg_add_route_adjust(apollo_route_adjust, "apollo_route_adjust");
    reg_add_sw_idb_init(apollo_init_idb, "apollo_init_idb");
    reg_add_proto_on_swidb(LINK_APOLLO, apollo_on_idb,  "apollo_on_idb");
    reg_add_routed_proto_on_swidb(LINK_APOLLO, apollo_on_idb, "apollo_on_idb");
    reg_add_ping_run(LINK_APOLLO, apollo_ping,  "apollo_ping");
    reg_add_ping_parse(apollo_ping_parse,  "apollo_ping_parse");
    reg_add_proto_running(LINK_APOLLO, apollo_enable_test,
			  "apollo_enable_test");
    reg_add_proto_bridging_on_swidb(LINK_APOLLO, apollo_bridging_idb,
				    "apollo_bridging_idb"); 
    reg_add_proto_routing_on_swidb(LINK_APOLLO, apollo_routing_idb,
				   "apollo_routing_idb");
    reg_add_determine_fair_queue_flow_id(LINK_APOLLO,
					 apollo_fair_queue_flow_id,
					 "apollo_fair_queue_flow_id");
    reg_add_print_network_header(LINK_APOLLO, apollo_print_network_header,
				 "apollo_print_network_header");
    reg_add_rxtypes_update(apollo_rxtypes_update,"apollo_rxtypes_update");
    reg_add_access_show(show_apolloaccess, "show_apolloaccess");
    reg_add_showproto(apollo_showproto, "apollo_showproto");
    reg_add_setup_global(apollo_setup_global, "apollo_setup_global");
    reg_add_setup_global2(apollo_setup_global2, "apollo_setup_global2");
    reg_add_setup_int(apollo_setup_int, "apollo_setup_int");
    reg_add_setup_end(apollo_setup_end, "apollo_setup_end");
}

/*
 * apollo_init
 * Initialize APOLLO protocol machine.
 * Allow bridging of APOLLO by MCI hardware.
 */

void apollo_init (subsystype *subsys)
{
    int		i;
    ulong	typeout;
    idbtype	*idb;

    apollo_register_thyself();
    apollo_debug_init();
    idb_create_list(SWIDBLIST_APOLLO, &typeout);
    for (i = 0; i < APOLLO_MAXTRAFFIC; i++)
	apollo_stat[i] = 0;
    apolloQ = NULL;
    queue_init(&apollobgQ, 0);
    apollo_update_interval = RIP_DEFAULT_UPDATE;
    TIMER_START(apollo_timer, 0);
    apollo_maxpaths = RIP_MAXPATHS;
    apollo_running = FALSE;
    apollo_started = FALSE;
    apollo_address = 0L;
    ieee_zero(apollo_etheraddr);
    apollo_table = NULL;
    FOR_ALL_SWIDBS(idb)
	apollo_rxtypes_update(idb);
    apollo_transid = 1;
}

/*
 * apollo_input Processing.
 * All of Apollo begins here
 */

/*
 * apollo_enqueue
 * Pass Apollo packet to APOLLO handler for forwarding, etc.
 */

void apollo_enqueue (paktype *pak)
{
    apollosb	*asb;

    asb = idb_get_swsb_inline(pak->if_input, SWIDB_SB_APOLLO);
    if (systeminit_flag && apollo_running && (asb != NULL) &&
	(asb->apollonet != 0)) {
	process_enqueue_pak(apolloQ, pak);
    } else {
	protocol_discard(pak, apollo_running);
    }
}

/*
 * values that apollo_forus returns
 */
#define APOLLO_ROUTEIT 0			/* route it */
#define APOLLO_CONSUMEIT 1			/* its for us */
#define APOLLO_PITCHIT 2			/* when we hear ourself */

process apollo_input (void)
{
    int			which;
    int			pak_count;
    int			len;
    paktype		*pak;
    apollohdrtype	*apollo;
    apollosb		*asb;

    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    apolloQ = create_watched_queue("Apollo Input", 0, 0);
    process_watch_queue(apolloQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_APOLLO, apollo_enqueue, "apollo_enqueue");

    while (TRUE) {
	process_wait_for_event();

	pak_count = PAK_SWITCHCOUNT;

	while (TRUE) {
	    /*
 	     * Process only a limited number of packets per pass
	     */
	    if (pak_count-- == 0) {
		break;
	    }

	    pak = process_dequeue(apolloQ);
	    if (pak == NULL) {
		break;
	    }

	    apollo = (apollohdrtype *)apolloheadstart(pak);
	    asb = idb_get_swsb_inline(pak->if_input, SWIDB_SB_APOLLO);
	    if (asb == NULL || asb->apollonet == 0L) {
		retbuffer(pak);
		continue;
	    }

	    apollo_stat[APOLLO_INPUT]++;

	    idb_increment_rx_proto_counts(pak, ACCT_PROTO_APOLLO);

	    len = apollo->len;
	    if (len & 1)		/* is the length odd? */
		len++;			/* odd packets preserve garbage */
	    /*
	     * Sanity check header fields
	     */
	    if ((len < APOLLO_MINSIZE) ||
		(len > APOLLO_MAXSIZE) ||
		(len > pak_get_pool_size((paktype*)pak))) {
		apollo_dispose(APOLLO_FORMATERROR, pak);
		continue;
	    }

	    /*
	     * Check checksum for validity
	     */
	    if (!apollo_validcrc(pak)) {
		apollo_dispose(APOLLO_CKSUMERR, pak);
		continue;
	    }

	    /*
	     * Is this packet aimed at us, or should it be forwarded, or
	     * pitched?
	     */
	    which = apollo_forus(pak);
	    switch (which) {
	      case APOLLO_CONSUMEIT:
		  if (apollo_debug)
		      buginf("\nApollo: src=%x.%e, dst=%x.%e, local dst",
			     GETLONG(&apollo->snet), apollo->shost,
			     GETLONG(&apollo->dnet), apollo->dhost);
		  apollo_stat[APOLLO_LOCALDEST]++;
		  p_enqueue(&apollobgQ, pak);
		  break;
	      case APOLLO_PITCHIT:
		  retbuffer(pak);
		  break;
	      case APOLLO_ROUTEIT:
		  apollo_forward(pak);
		  break;
	      }
	}
    }
}

/*
 * apollo_validcrc
 * Check whether the packet has a valid CRC.  DomainOS always generates
 * an 0xFFFF.  Apollo NCS, however, puts something into this field.
 * Whether this is a checksum is anyone's guess.  Even Apollo doesn't
 * seem to know.  We will always ignore the checksum here.
 */

boolean apollo_validcrc (paktype *pak)
{
    return(TRUE);
}

/*
 * apollo_forus
 * Determine if a packet is meant for this host
 */

int apollo_forus (paktype *pak)
{
    apollohdrtype *apollo;
    apollosb	*asb;

    apollo = (apollohdrtype *)apolloheadstart(pak);

    if (ieee_equal(apollo->dhost, apollo_bdcast)) {
	asb = idb_get_swsb_inline(pak->if_input, SWIDB_SB_APOLLO);
	if (!asb)
	    return(APOLLO_PITCHIT);
	/*
	 * Forward broadcast if the destination network is not
	 * broadcast or unspecified and the source network is not
	 * unspecified and the destination network does not
	 * match the input interface network.
	 */
	if ((GETLONG(&apollo->dnet) != -1L) &&
	    (GETLONG(&apollo->dnet) != 0L) &&
	    (GETLONG(&apollo->snet) != 0L) &&
	    (GETLONG(&apollo->dnet) != asb->apollonet))
	    return (APOLLO_ROUTEIT);

	apollo_stat[APOLLO_BCASTIN]++;
	/*
	 * Ignore our own broadcasts
	 */
	if (ieee_equal(apollo_etheraddr, apollo->shost))
	    return (APOLLO_PITCHIT);
	pak->flags |= PAK_BROADCAST;
	return (APOLLO_CONSUMEIT);
    }
    if (ieee_equal(apollo_etheraddr, apollo->dhost))
	return (APOLLO_CONSUMEIT);

    return (APOLLO_ROUTEIT);
}

/*
 * apollo_forward
 * Forward a Apollo datagram
 */

void apollo_forward (paktype *pak)
{
    apollohdrtype *apollo;
    register ushort hops;

    apollo = (apollohdrtype *)apolloheadstart(pak);

    if (apollo->shost[0] || apollo->shost[1]) {
	if (apollo_debug)
	    buginf("\nApollo: src=%x.%e, dst=%x.%e, bad src addr",
		   GETLONG(&apollo->snet), apollo->shost,
		   GETLONG(&apollo->dnet), apollo->dhost);
	apollo_dispose(APOLLO_INMULT, pak);
	return;
    }
    hops = (apollo->tc & XNS_TC_HOP_MASK) + 1; /* Increment only hop count bits */
    if (hops >= APOLLO_INFINITY) {
	if (apollo_debug)
	    buginf("\nApollo: src=%x.%e, dst=%x.%e, hop count exceeded",
		   GETLONG(&apollo->snet), apollo->shost,
		   GETLONG(&apollo->dnet), apollo->dhost);
	apollo_dispose(APOLLO_HOPCOUNT, pak);
	return;
    }
    /* Update hops field within tc, preserving reserved bits */
    apollo->tc = (hops |= (apollo->tc & XNS_TC_RSV_MASK));
    apollo->cksum = 0xFFFF;
    if (apollo_send(pak))
	apollo_stat[APOLLO_FORWARDED]++;
}

/*
 * apollo_send
 * send an Apollo gram
 */

boolean apollo_send (paktype *pak)
{
    apollohdrtype	*apollo;
    register uchar	*dhost;
    xns_ripentrytype	*p;
    apollosb		*asb;

    apollo = (apollohdrtype *)apolloheadstart(pak);

    p = (*apollo_table)[nethash(GETLONG(&apollo->dnet))];
    dhost = grip_first_hop(GETLONG(&apollo->dnet),
			   &(apollo->dhost[0]),
			   &(pak->if_output), p);
    if (dhost == NULL) {		/* no route */
	if (apollo_debug)
	    buginf("\nApollo: src=%x.%e, dst=%x.%e, no route",
		   GETLONG(&apollo->snet), apollo->shost,
		   GETLONG(&apollo->dnet), apollo->dhost);
	apollo_dispose(APOLLO_NOROUTE, pak);
	return(FALSE);
    }
    asb = idb_get_swsb_inline(pak->if_output, SWIDB_SB_APOLLO);
    if (asb && asb->apollo_acclist) {
	apollo_acclisttype *acclist = asb->apollo_acclist;
	apollo_acctype *acc = (apollo_acctype*) acclist->q.qhead;
	boolean grant = FALSE;
	
	for (; acc; acc = acc->next) {
	    if (GETLONG(&apollo->snet) <= acc->lastnet &&
		GETLONG(&apollo->snet) >= acc->firstnet &&
		(*(long*)(apollo->shost+2) & ~acc->hostmask) == acc->host) {
		grant = acc->result;
		break;
	    }
	}
	if (grant == FALSE) {
	    if (apollo_debug)
		buginf("\nApollo: src=%x.%e, dst=%x.%e, access denied",
		       GETLONG(&apollo->snet), apollo->shost,
		       GETLONG(&apollo->dnet), apollo->dhost);
	    apollo_dispose(APOLLO_NOROUTE, pak);
	    return(FALSE);
	}
    }
    if (apollo_debug)
	buginf("\nApollo: src=%q.%b, dst=%q.%b, send packet",
	       APOLLO_TYPE, GETLONG(&apollo->snet), APOLLO_TYPE, 
	       apollo->shost, APOLLO_TYPE, GETLONG(&apollo->dnet),
	       APOLLO_TYPE, apollo->dhost);
    return (apollo_send1(pak, dhost));
}

/*
 * apollo_send1
 * Other half of sending APOLLO grams
 */

boolean apollo_send1 (paktype *pak, uchar *dhost)
{
    uint len;
    idbtype *idb;
    apollohdrtype *apollo;

    apollo = (apollohdrtype *)apolloheadstart(pak);

    idb = pak->if_output;
    pak->acct_proto = ACCT_PROTO_APOLLO;
    if (idb == NULL) {
	errmsg(&msgsym(NULLIDB, SYS), "apollo_send1");
	retbuffer(pak);
	return(FALSE);
    }
    if (!interface_up_simple(idb)) {
	retbuffer(pak);
	return(FALSE);
    }
    if (pak->if_input == NULL) {	/* fill in "private" data */
	ether_apollo *ether = (ether_apollo *)(apolloheadstart(pak) -
					       ETHER_APOLLO_ENCAPBYTES);
	memset(ether->private, 0, APOLLOPRIVATELEN);
    }
    bcopy(&dhost[2], (uchar *)&apollo->destnode, 4); /* update next hop node id */
    if (dhost[0] & APOLLO_MULTICAST)
	apollo_stat[APOLLO_BCASTOUT]++;
    len = apollo->hdrlen + apollo->datalen;
    if (len & 1)
	len++;
    if (pak_encap(idb->hwptr, pak, len, (long)dhost)) {
	apollo_stat[APOLLO_OUTPUT]++;
	datagram_out(pak);		/* queue it for output */
	return(TRUE);
    }
    if (apollo_debug)
	buginf("\nApollo: src=%q.%b, dst=%q.%b, encapsulation failed",
	       APOLLO_TYPE, GETLONG(&apollo->snet), APOLLO_TYPE, 
	       apollo->shost, APOLLO_TYPE, GETLONG(&apollo->dnet),
	       APOLLO_TYPE, apollo->dhost);
    apollo_dispose(APOLLO_ENCAPSFAILED, pak);
    return(FALSE);
}

/*
 * apollo_dispose
 * discard an APOLLO input packet.  Apollo doesn't have an error protocol,
 * so we just count the packet and maybe print a message.
 */

void apollo_dispose (int code, paktype *pak)
{
    if (code >= APOLLO_MAXTRAFFIC)
      errmsg(&msgsym(BADDISP, SYS), code, "apollo_dispose");
    else
	apollo_stat[code]++;
    retbuffer(pak);
}

/*
 * apollo_printpak
 * print the relevant contents of an apollo packet.
 */
#ifdef NOBODY_CALLS_ME
void apollo_printpak (paktype *pak)
{
    apollohdrtype *apollo;
    ether_apollo  *ether;

    ether = (ether_apollo *)(apolloheadstart(pak) - ETHER_APOLLO_ENCAPBYTES);
    apollo = (apollohdrtype *)apolloheadstart(pak);

    buginf("\nApollo packet received from %e, dest %e, type %#x",
	   ether->saddr, ether->daddr, ether->type);

    buginf("\n   dnode: %x, type %x, snode %x, version %d, link %x",
	   apollo->destnode, apollo->pkttype, apollo->srcnode,
	   apollo->version, apollo->apollolink);
    buginf("\n   Hdrlen %d  Hdrdatalen %d, datalen %d, id %x, rl %d, pos %d",
	   apollo->hdrlen, apollo->hdrdatalen, apollo->datalen, apollo->transid,
	   apollo->curpos);
    buginf("\n   Unknown (14): %e.%e.%e", apollo->unknown, &apollo->unknown[6],
	   &apollo->unknown[12]);
    buginf("\n   cksum %#x,  len %d,  tc %#x,  pt %#x", apollo->cksum,
	   apollo->len, apollo->tc, apollo->pt);
    buginf("\n   Dest %x.%e::%x,  Src: %x.%e::%x",
	   GETLONG(&apollo->dnet), apollo->dhost, apollo->dsock,
	   GETLONG(&apollo->snet), apollo->shost, apollo->ssock);
}
#endif
