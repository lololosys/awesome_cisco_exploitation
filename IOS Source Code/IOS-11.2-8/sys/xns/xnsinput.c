/* $Id: xnsinput.c,v 3.9.6.21 1996/09/11 20:09:47 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xns/xnsinput.c,v $
 *------------------------------------------------------------------
 * xnsinput.c -- handle XNS input
 *
 * November 1987, Greg Satz
 *
 * Copyright (c) 1987-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xnsinput.c,v $
 * Revision 3.9.6.21  1996/09/11  20:09:47  snyder
 * CSCdi68837:  more things const
 *              156 out of data, 64 image
 * Branch: California_branch
 *
 * Revision 3.9.6.20  1996/08/28  13:25:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.6.19  1996/08/14  02:38:15  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.9.6.18  1996/08/13  14:41:26  rbadri
 * CSCdi64089:  Multiprotocol support fixes required
 * Branch: California_branch
 * XNS support for LANE
 *
 * Revision 3.9.6.17  1996/08/07  09:05:42  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.9.6.16  1996/08/03  23:44:45  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.9.6.15  1996/07/26  00:54:30  hampton
 * Fix the automore code in show access-list.  [CSCdi61084]
 * Branch: California_branch
 *
 * Revision 3.9.6.14  1996/07/13  11:43:02  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.9.6.13  1996/07/02  23:12:08  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.9.6.12  1996/06/28  23:34:07  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.9.6.11  1996/06/24  09:30:18  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.9.6.10  1996/06/17  23:40:49  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.9.6.9  1996/05/30  23:49:58  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.9.6.8  1996/05/17  12:18:36  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.4.4  1996/05/06  00:13:31  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.8.4.3  1996/04/26  08:00:21  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.8.4.2  1996/04/03  22:23:11  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.8.4.1  1996/03/17  18:57:22  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.9.6.7  1996/05/10  01:46:25  wmay
 * CPP and ppp half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.9.6.6  1996/05/09  14:50:08  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.9.6.5  1996/05/04  02:19:03  wilber
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
 * Revision 3.9.6.4  1996/04/29  09:48:19  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.9.6.3  1996/04/29  02:42:42  bcole
 * CSCdi55590:  configuring ipx routing  breaks IP ping
 * Branch: California_branch
 *
 * Add 'ipdiag' to the list of subsystem dependencies for subsystems which
 * support the ping command.
 *
 * Revision 3.9.6.2.4.1  1996/04/27  07:40:00  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.9.6.2  1996/04/17  13:52:41  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.9.6.1.8.1  1996/04/08  02:44:13  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.9.6.1  1996/03/18  22:54:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.8.1  1996/03/22  22:57:00  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.9  1996/03/07  02:42:14  sluong
 * CSCdi50905:  show xns int command response incorrect
 *
 * Revision 3.8.8.1  1996/03/05  05:38:36  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.8.2.1  1996/02/27  21:21:33  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.8  1996/02/08  20:16:01  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.7  1996/02/07  16:18:30  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.6  1996/01/29  07:32:07  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5.6.1  1996/02/15  19:27:47  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.5  1995/12/07  22:35:09  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/11/22  09:18:39  hampton
 * Fix the mips routine to read/write unaligned longlong words.  While
 * there, rework the entire family of routines to reduce overhead.  This
 * necessitates a change to code calling these routines.  [CSCdi44257]
 *
 * Revision 3.3  1995/11/17  19:26:00  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:54:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  14:01:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.14  1995/10/03  20:13:32  john
 * CSCdi41365:  XNS fastswitching not enabled
 *
 * Revision 2.13  1995/09/11  23:59:07  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.12  1995/09/01 23:06:59  asastry
 * CSCdi39698:  WFQ: Display info (in "show queue") for non-IP protocols
 * as well
 *
 * Revision 2.11  1995/08/10  22:40:02  sluong
 * CSCdi38591:  xns static route cmd failure gives duplicate sh xns route
 * entries
 *
 * Revision 2.10  1995/07/12  08:30:18  dstine
 * CSCdi36963:  XNS network numbers print in hex
 *
 * Revision 2.9  1995/07/12  05:53:39  john
 * CSCdi36838:  4500 crashes when a xns access group is deleted
 *              part II, fix grammar and turn buginf into printf
 *
 * Revision 2.8  1995/07/12  00:32:28  john
 * CSCdi36838:  4500 crashes when a xns access group is deleted
 *
 * Revision 2.7  1995/07/09  21:10:59  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Derive the CRB route/bridge behaviour from explicit, per-interface
 * route/bridge indicators rather than inferring it from protocol-
 * specific configuration.
 *
 * Revision 2.6  1995/06/30  05:24:17  dstine
 * CSCdi35970:  router crashes. Unable to gain access to console.
 *         - XNS UB routing emulation dereferences a NULL idb pointer.
 *           Fill in the idb in the xnsubrip block.
 *
 * Revision 2.5  1995/06/20  21:54:28  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/19  04:10:13  speakman
 * CSCdi35697:  General TB and SRB regression failures
 * Make protocol-specific route/bridge test explicit rather than relying
 * on per-interface rxtypes indicators to provide backward compatible
 * SRB and TB behaviour.
 *
 * Revision 2.3  1995/06/18  06:20:16  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.2  1995/06/18 02:24:27  hampton
 * Change the name of an XNS timer variable to something that makes its
 * usage clearer.  While there, remove direct references to msclock.
 * [CSCdi36038]
 *
 * Revision 2.1  1995/06/07 23:31:15  hampton
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
#include "packet.h"
#include "linktype.h"
#include "ieee.h"
#include "config.h"
#include "../ui/debug.h"
#include "xns.h"
#include "xns_public.h"
#include "xns_debug.h"
#include "xnsfast.h"
#include "access.h"
#include "logger.h"
#include "../if/arp.h"
#include "../if/network.h"
#include "../if/priority_private.h"
#include "parser.h"
#include "parser_defs_xns_nov.h"
#include "../iprouting/route.h"
#include "media_registry.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include "../wan/ppp.h"
#include "../wan/ppp_registry.h"
#include "../lane/lane.h"


static watched_queue *xnsQ;		/* XNS input queue */
static queuetype xnsbgQ;		/* XNS background queue */
queuetype xnsripQ;			/* XNS RIP socket queue */
queuetype xnsechoQ;			/* XNS echo queue */

boolean xns_rip_delay;                  /* true if need a delay */
boolean xns_running;			/* if running XNS */
static boolean xns_started;		/* has XNS ever been started */
static uchar xns_defaddr[6];		/* if not IEEE what to use */
static int xns_forwcnt;			/* count for forwardings */
static int xns_forw_proto[XNS_FORWMAX];	/* actual types of forwardings */
static const uchar xns_allhosts[IEEEBYTES] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

ulong xns_stat[XNS_MAXTRAFFIC];	/* maintain traffic information */

static process xns_input (void);

extern int nTRs, nethernets;
extern sys_timestamp xns_timer;
extern sys_timestamp xns_ubhellotimer;


/*
 * The XNS subsystem header
 */

#define XNS_MAJVERSION 1
#define XNS_MINVERSION 0
#define XNS_EDITVERSION  1

SUBSYS_HEADER(xns, XNS_MAJVERSION, XNS_MINVERSION, XNS_EDITVERSION,
	      xns_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, ipdiag, ipx, ppp",
	      "req: griproute");



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
short xns_fair_queue_flow_id (paktype *pak)
{
    xnshdrtype *xns;
    int         sum;

    pak->fair_weight = FAIR_QUEUE_BASE_WEIGHT;
    xns = (xnshdrtype *)xnsheadstart(pak);
    /* hash protocol, source, and destination */
    sum = xnethash((uchar *)&xns->pt, 25);
    return fair_queue_sum(pak, sum);
}
/*
 * xns_print_network_header
 *
 * Print XNS header.
 */
static void xns_print_network_header (paktype *pak, ushort **data)
{
    xnshdrtype *xns;

    xns = (xnshdrtype *)xnsheadstart(pak);
    printf("\n  source:%b socket %04x destination %b socket %04x protocol %02x",
	   xns->ssnet, xns->shost, xns->ssock, 
	   xns->ddnet, xns->dhost, xns->dsock, xns->pt);
}

/*
 * xns_init_sb
 *
 * Init a SWIDB sub-block. If NULL passed, allocate a sub-block and then
 * initialize it.
 */
xnssb *xns_init_sb (xnssb *xsb)
{
    if (xsb == NULL) {
	xsb = malloc_named(sizeof(xnssb), "IDB: XNS Info");
	if (xsb == NULL)
	    return (NULL);
    }
    memset(xsb, 0, sizeof(xnssb));
    /*
     * Set "true blue" XNS-specific routing variables here.
     */
    xsb->xnsrip.type = XNS_TYPE;
    xsb->xnsrip.update_interval = RIP_DEFAULT_UPDATE;
    xsb->xnsrip.updatetimer = xns_timer;
    xsb->xnsrip.static_rts = 0;
    /*
     * Set "Urine & Bile" XNS-specific routing variables here.
     */
    xsb->ubxnsrip.type = UBXNS_TYPE;
    xsb->ubxnsrip.update_interval = RIP_DEFAULT_UPDATE;
    xsb->ubxnsrip.updatetimer = xns_timer;
    xsb->ubxnsrip.static_rts = 0;

    return (xsb);
}

/*
 * xns_init_idb
 * Split out so that we can initialize dynamicly created interfaces
 * like the tunnel.
 */
void xns_init_idb (idbtype *idb, boolean setup_hwidb)
{
    xnssb		*xsb;
    idbtype		*l_idb;
    list_element	*l_elt;
    boolean		found = FALSE;

    if (idb == NULL)
	return;

    xsb = idb_get_swsb(idb, SWIDB_SB_XNS);
    /*
     * If there isn't already an XNS sub-block on this interface,
     * there isn't any need to go farther.
     */
    if (xsb == NULL)
	return;

    xsb->idb = idb;

    if (setup_hwidb) {
	idb->hwptr->xns_fastokay = FALSE;
	idb->hwptr->xns_routecache = TRUE;
    }
    xsb->xnsrip.xns_lineup = TRUE;
    xsb->ubxnsrip.xns_lineup = TRUE;
    xsb->xnsrip.idb = xsb->ubxnsrip.idb = idb;
    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_XNS, l_elt, l_idb) {
	if (l_idb == idb) {
	    found = TRUE;
	    break;
	}
    }
    if (!found)
	idb_add_swidb_to_list(idb, SWIDBLIST_XNS);
}

/*
 * xns_on_idb
 *
 * Return TRUE if XNS is enabled on the given interface
 */
boolean xns_on_idb (idbtype *idb)
{
    xnssb	*xsb;

    if (idb == NULL)
	return (FALSE);
    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
    if (xsb == NULL)
	return (FALSE);

    if ((!xns_running) || (xsb->xnsnet == 0L))
      return (FALSE);
    else
      return (TRUE);
}

/*
 * xns_swidb_encap
 * Called when the IDB in question changes its encapsulation type.
 */
static void
xns_swidb_encap (idbtype *idb, int enctype)
{
    xnssb	*xsb;
    ulong	status;
    encapstype	computed_xns_enctype;

    if (idb == NULL)
	return;

    xsb = idb_get_swsb(idb, SWIDB_SB_XNS);
    if (xsb == NULL)
	return;

    status = idb->hwptr->status;
    if (status & IDB_ETHER)
	computed_xns_enctype = ET_ARPA;
    else if (status & IDB_TR)
	computed_xns_enctype = ET_SNAP;
    else if (status & IDB_FDDI)
	computed_xns_enctype = ET_SNAP;
    else if (status & IDB_TUNNEL)
	computed_xns_enctype = idb->hwptr->enctype;
    else if (status & IDB_SERIAL) {
	/*
	 * Serial line encapsulations are such a party.
	 */
	switch (enctype) {
	case ET_HDLC:
	case ET_BSTUN:
	    computed_xns_enctype = ET_HDLC;
	    break;
	    
	case ET_SLIP:
	    computed_xns_enctype = ET_SLIP;
	    break;
	    
	case ET_PPP:
	    computed_xns_enctype = ET_PPP;
	    break;
	    
	/*
	 * All "null" serial line encapsulations for XNS come
	 * here. There are more than a couple...
	 */
	case ET_LAPB:
	case ET_X25:
	case ET_FRAME_RELAY:
	case ET_STUN:
	case ET_SMDS:
	default:
	    computed_xns_enctype = ET_NULL;
	    break;
	}
    } else if (status & IDB_ASYNC) {
	computed_xns_enctype = idb->hwptr->enctype;
    } else if (status & IDB_ATM) {
	if (is_atm_lane(idb)) {
	    /*
	     * set the enctype to what ever the lane client said it
	     * is supporting (for now it is ET_ARPA)
             */
	    computed_xns_enctype = enctype;
	} else {
	    computed_xns_enctype = ET_NULL;
	}
    } else {
	computed_xns_enctype = ET_NULL;
    }
    xsb->xns_enctype = computed_xns_enctype;
}

/*
 * xns_enable_test
 * Return TRUE if we're running XNS
 * Called by SERVICE_PROTO_RUNNING.
 */

static boolean xns_enable_test (void)
{
    return (xns_running);
}

/*
 * xns_routing_idb
 * Return TRUE iff XNS packets inbound on this interface should be
 * routed.
 */

static boolean xns_routing_idb (idbtype *swidb)
{
    if ((global_crb_enable || global_irb_enable) && swidb->tbridge_on_swidb) {
	/*
	 * For CRB, just return the xns route/bridge indicator
	 * if this interface is in a bridge group.  This is
	 * set by the "bridge <n> route xns" command.
	 */
	return(swidb->tbridge_route_linktypes & LINK_XNS_RBI);
    } else {

	xnssb	*xsb;

	xsb = idb_get_swsb_inline(swidb, SWIDB_SB_XNS);

	/*
	 * Otherwise, just verify that xns routing is enabled,
	 * and this interface is enabled for xns.  This latter
	 * is set by the "xns network" command.
	 */
	return (xns_running && (xsb != NULL && xsb->xnsnet));
    }
}

/*
 * xns_bridging_idb
 * Return TRUE iff XNS packets inbound on this interface should be
 * bridged.
 */

static boolean xns_bridging_idb (idbtype *swidb)
{
    if (global_crb_enable || global_irb_enable) {
	/*
	 * For CRB, just return the complement of the xns
	 * route/bridge indicator if this interface is in
	 * a bridge group.  This is cleared by the
	 * "no bridge <n> route xns" command (the default).
	 */
	return (swidb->tbridge_on_swidb &&
		(!(swidb->tbridge_route_linktypes & LINK_XNS_RBI)));
    } else if (global_irb_enable) {
	return (swidb->tbridge_on_swidb &&
		!(swidb->tbridge_bridge_linktypes & LINK_XNS_RBI));
    } else {
	/*
	 * To support SRB and the archaic transparent bridging
	 * behaviour just check for the absence of xns routing.
	 */
	return(!xns_running);
    }
}
/*
 * xns_rxtypes_update
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

static void xns_rxtypes_update (idbtype *swidb)
{
    boolean rte_xn, br_xn;

    if (global_crb_enable || global_irb_enable) {
	rte_xn = xns_routing_idb(swidb);
	br_xn = xns_bridging_idb(swidb);
    } else {
	rte_xn = xns_running;
	br_xn = !rte_xn;
    }

    if (rte_xn) {
	SET_ROUTING_RXTYPE(swidb, RXTYPE_XNS);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_XNS);
    } else {
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_XNS);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_XNS);
    }

    if (br_xn) {
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_XNS);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_XNS);
    } else {
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_XNS);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_XNS);
    }

    if (swidb->hwptr->setup_bridging)
	(*(swidb->hwptr->setup_bridging))(swidb->hwptr);

    reg_invoke_bridge_setup();	/* sse_bridging_setup_enqueue() */
}

/*
 * xns_showproto
 * Interface specific portion of "show protocols" commands
 * Called by SERVICE_SHOWPROTO
 */

static void xns_showproto (idbtype *idb)
{
    xnssb	*xsb;

    if (idb == NULL) {
	if (RUNNING(LINK_XNS))
	    printf("\n  XNS routing is enabled");
	return;
    }
    if (RUNNING_IF(LINK_XNS, idb)) {
	xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
	if (xsb)
	    printf("\n  XNS address is %u.%e", xsb->xnsnet, xsb->xns_node);
    }
}

/*
 * xns_register_thyself
 */
static void xns_register_thyself (void)
{
    xns_parser_init();
    xns_media_registry();

    /*
     * Register some functions
     */
    reg_add_bump_cache_version(xnscache_increment_version,
			       "xnscache_increment_version");
    reg_add_route_adjust(xns_route_adjust, "xns_route_adjust");
    reg_add_fast_setup(xns_fastsetup, "xns_fastsetup");
    reg_add_sw_idb_init(xns_init_idb, "xns_init_idb");
    reg_add_proto_on_swidb(LINK_XNS, xns_on_idb, "xns_on_idb");
    reg_add_routed_proto_on_swidb(LINK_XNS, xns_on_idb, "xns_on_idb");
    reg_add_ping_run(LINK_XNS, xns_ping, "xns_ping");
    reg_add_ping_parse(xns_ping_parse, "xns_ping_parse");
    reg_add_proto_running(LINK_XNS, xns_enable_test, "xns_enable_test");
    reg_add_proto_bridging_on_swidb(LINK_XNS, xns_bridging_idb,
				    "xns_bridging_idb"); 
    reg_add_proto_routing_on_swidb(LINK_XNS, xns_routing_idb,
				   "xns_routing_idb");
    reg_add_determine_fair_queue_flow_id(LINK_XNS, xns_fair_queue_flow_id,
					 "xns_fair_queue_flow_id");
    reg_add_print_network_header(LINK_XNS, xns_print_network_header,
				 "xns_print_network_header");
    reg_add_rxtypes_update(xns_rxtypes_update,"xns_rxtypes_update");
    reg_add_access_show(show_xnsaccess, "show_xnsaccess");
    reg_add_showproto(xns_showproto, "xns_showproto");
    reg_add_setup_global(xns_setup_global, "xns_setup_global");
    reg_add_setup_global2(xns_setup_global2, "xns_setup_global2");
    reg_add_setup_int(xns_setup_int, "xns_setup_int");
    reg_add_setup_end(xns_setup_end, "xns_setup_end");
    reg_add_media_enctype_setup(xns_swidb_encap, "xns_swidb_encap");
}

/*
 * xns_init
 * Initialize XNS protocol machine.
 * Allow bridging of XNS by MCI hardware.
 */

void xns_init (subsystype *subsys)
{
    int		i;
    ulong	typeout;
    idbtype	*idb;

    xns_register_thyself();
    xns_debug_init();
    idb_create_list(SWIDBLIST_XNS, &typeout);
    for (i = 0; i < XNS_MAXTRAFFIC; i++)
	xns_stat[i] = 0;
    xnsQ = NULL;
    queue_init(&xnsbgQ, 0);
    xns_maxpaths = RIP_MAXPATHS;
    TIMER_START(xns_timer, 0);
    xns_running = FALSE;
    xns_started = FALSE;
    xns_rip_delay = FALSE;
    xns_forwcnt = 0;
    FOR_ALL_SWIDBS(idb)
	xns_rxtypes_update(idb);
    xns_cache_init();
    xns_test_init();

    /*
     * Register XNSCP with PPP
     */
    reg_invoke_ncp_register(LINK_XNS, TYPE_PPP_XNSCP,
			    NULL, NULL, "XNSCP");
}

static const char xns_badaddr[] = "\n%%Illegal XNS address - %s";
static const char xns_notenabled[] = "\n%%XNS not enabled";

void xns_command (parseinfo *csb)
{
    hwidbtype *idb = NULL;
    idbtype *swidb = NULL;
    uchar host[IEEEBYTES];
    ulong dstnet, net;
    uchar *defaddr;
    int i;
    list_element	*l_elt;
    xnssb		*xsb;

    if (csb->nvgen) {
	if (!xns_running) {
	    return;
	}
	switch (csb->which) {
	  case XNS_ADDRESS:
	    /* Alias for XNS_ROUTING; no NV generation to be done. */
	    break;
	  case XNS_ROUTING:
	    nv_write(TRUE, "%s %e", csb->nv_command, xns_defaddr);
	    break;
	  case XNS_ROUTE:
	    nv_grip_route(xns_table ,"xns");
	    break;
	  case XNS_FORWARD:
	    for (i = 0; i < xns_forwcnt; i++) {
		nv_write(TRUE, "%s %d", csb->nv_command, xns_forw_proto[i]);
	    }
	    break;
	  case XNS_UBROUTE:
	    /* Alias for XNS_UBEMULATION; do not do NV generation */
	    break;
	  case XNS_UBEMULATION:
	    nv_write(xns_ub_emulation, csb->nv_command);
	    break;
	  case XNS_PATHS:
	    nv_write((xns_maxpaths != RIP_MAXPATHS), "%s %d",
		     csb->nv_command, xns_maxpaths);
	    break;
	  case XNS_RIP_DELAY:
	    nv_write(xns_rip_delay, csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    if (!xns_running &&
	csb->which != XNS_ROUTING &&
	csb->which != XNS_ADDRESS) {

	printf(xns_notenabled);
	return;
    }

    switch (csb->which) {
      case XNS_ADDRESS:
      case XNS_ROUTING:
	if (!csb->sense) {
	    xnscache_increment_version(NULL, NULL, FALSE);
	    FOR_ALL_SWIDBS(swidb)
		xns_rxtypes_update(swidb);
	    xns_running = FALSE;
	    FOR_ALL_HWIDBS(idb) {
		xns_fastsetup(idb);
	    }
	    return;
	}
	if (GETOBJ(int,1)) {
	    ieee_copy(GETOBJ(paddr,1)->xns_addr.host, host);
	    if ((GETSHORT(&host[4]) == 0 && GETSHORT(&host[2]) == 0
                 && GETSHORT(&host[0]) == 0)) {
                printf(xns_badaddr, "malformed hex address");
                return;
	    }
	    if (nethernets > 0 && (host[0] & XNS_MULTICAST_ETHER)) {
		printf(xns_badaddr,  "multicast not permitted");
                return;
	    }
            defaddr = host;
	} else {
	    /*
             * No address spec was given.  Use a default.  This default
             * will be an IEEE interface with a valid address.  It
             * is used for non-IEEE interfaces if present.
             */
            defaddr = default_mac_addr;
	}
	xns_running = TRUE;
	FOR_ALL_SWIDBS(swidb)
	    xns_rxtypes_update(swidb);
	GET_TIMESTAMP(xns_timer);
	ieee_copy(defaddr, xns_defaddr);
	FOR_ALL_HWIDBS(idb) {
	    /*
	     * If DECNET is running we have already coerced.  If
	     * the ATG is running all ETHERs won't be coerced to
	     * the same, but we live with this.  The new XNS code
	     * handles this situation.
	     * Lex is a virtual ethernet.
	     */
	    if (((idb->status & IDB_ETHER) || (idb->status & IDB_LEX) ||
		 (idb->status & IDB_ATM)) && !RUNNING(LINK_DECNET)) {
		coerce_hwaddr(defaddr, idb);
	    }
	    xns_fastsetup(idb);
	}
	arp_table_reload();
	if (!xns_started) {
	    xns_start();
	}
	break;

      case XNS_ROUTE:
        dstnet = GETOBJ(int,1);
        if (dstnet == 0L || dstnet == -1L) {
            printf(xns_badaddr, "bad network number");
            return;
        }
        if (!csb->sense) {
	    xnscache_increment_version(NULL, NULL, FALSE);
            grip_del_rt(dstnet, FALSE, xns_table, xns_running,
			xnsrip_debug);
            return;
        }
        net = GETOBJ(paddr,1)->xns_addr.net;
        ieee_copy(GETOBJ(paddr,1)->xns_addr.host, host);

        FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_XNS, l_elt, swidb) {
	    xnssb	*xsb1;
	    if ((xsb1 = idb_get_swsb(swidb, SWIDB_SB_XNS)) != NULL)
		if (ieee_equal(xsb1->xns_node, host)) {
		    printf(xns_badaddr,  "can't be local host");
		    return;
		}
        }
	xsb = NULL;
        FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_XNS, l_elt, swidb) {
	    xnssb	*xsb1;
	    if ((xsb1 = idb_get_swsb_inline(swidb, SWIDB_SB_XNS)) != NULL)
		if (xsb1->xnsnet == net) {
		    xsb = xsb1;
		    break;
		}
	}
        if (xsb == NULL) {
            printf(xns_badaddr, "must be on connected network");
            return;
        }

	idb = swidb->hwptr;
        if (((idb->status & IDB_ETHER) && (host[0] & XNS_MULTICAST_ETHER)) ||
            ((idb->status & IDB_TR)    && (host[0] & XNS_MULTICAST_TR))) {
            printf(xns_badaddr,  "multicast not permitted");
            return;
        }
        grip_del_rt(dstnet, FALSE, xns_table, xns_running,
		    xnsrip_debug);
	if (grip_find_rt(dstnet, xns_table))
	    return;
        grip_add_rt(&xsb->xnsrip, dstnet, net, host, NET_STATIC, xns_table,
                    xns_running, FALSE, xnsrip_debug);
        xsb->xnsrip.static_rts++;
	break;

      case XNS_FORWARD:
	if (!int_array_op(xns_forw_proto, &xns_forwcnt, XNS_FORWMAX,
			  GETOBJ(int,1), csb->sense)) {
	    if (csb->sense) {
		printf("\n%%Maximum XNS forwarding protocols are in use.");
	    } else {
		printf("\n%%XNS protocol %d not found to delete",
		       GETOBJ(int,1));
	    }
	}
	break;

      case XNS_UBROUTE:
	if (csb->sense) {
	    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_XNS, l_elt, swidb) {
		xnssb	*xsb1;

		xsb1 = idb_get_swsb_inline(swidb, SWIDB_SB_XNS);
		if (xsb1 == NULL)
		    continue;
		xsb1->xns_hear_rip = TRUE;
		xsb1->xns_fld_spec_allnets = TRUE;
		xsb1->xns_fld_bcast_allnets = TRUE;
		xsb1->xns_fld_bcast_netzero = FALSE;
	    }
	}
	/* Fall through to UB emulation setup */
      case XNS_UBEMULATION:
	/*
	 * This can have nasty effects on the state of the XNS routing
         * table, so it's a no-op if it's setting things back the way
         * they already are.
         */
        if (csb->sense && !xns_ub_emulation) {
            xns_ubhellotimer = xns_timer;
            FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_XNS, l_elt, swidb) {
		xnssb	*xsb1;

		xsb1 = idb_get_swsb_inline(swidb, SWIDB_SB_XNS);
		if (xsb1 == NULL)
		    continue;
		xsb1->ubxnsrip.type = UBXNS_TYPE;
                xsb1->ubxnsrip.update_interval  = UB_UPDATE_INTERVAL;
                COPY_TIMESTAMP(xns_timer, xsb1->ubxnsrip.updatetimer);
                TIMER_STOP(xsb1->ubxnsrip.lastupdate);
            }
            xns_ub_emulation = TRUE;
            /* Let the XNS routing process clean out the routing table
             * next time it runs...
             */
            xns_purgeflag = TRUE;
        } else {
	    if (!csb->sense && xns_ub_emulation) {
		xns_ub_emulation = FALSE;
		/* Clean out the routing table in the XNS routing process... */
		xns_purgeflag = TRUE;
	    }
        }
        break;
      case XNS_PATHS:
	if (csb->sense) {
	    xns_maxpaths = GETOBJ(int,1);
	} else {
	    xns_maxpaths = RIP_MAXPATHS;
	}
	break;
      case XNS_RIP_DELAY:
	xns_rip_delay = csb->sense;
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * xns_if_deconfig
 *
 * Turn off XNS routing on a specified interface.
 */
static void
xns_if_deconfig (parseinfo *csb)
{
    xnssb	*xsb;
    idbtype	*idb ;

    if (csb == NULL)
	return;
    idb = csb->interface;
    if (idb == NULL)
	return;
    /*
     * Disabling XNS on an interface. If we're missing the
     * xsb here, bail.
     */
    xsb = idb_get_swsb(idb, SWIDB_SB_XNS);
    if (xsb == NULL) {
	printf("\nXNS: is not enabled on %s", idb->namestring);
	return;
    }
    xnscache_increment_version(NULL, NULL, FALSE);
    grip_del_rt(xsb->xnsnet, TRUE, xns_table, xns_running, 
		xnsrip_debug);
    grip_prune_idb(idb, xns_table, &xns_routes, "XNS", xnsrip_debug);
    xns_fastsetup(idb->hwptr);
    xns_rxtypes_update(idb);
    /*
     * Delete the XNS swidb sub-block and remove this IDB from the
     * XNS idblist.
     */
    if (!idb_delete_swsb(idb, SWIDB_SB_XNS)) {
	buginf("\nXNS: failed to delete XNS sub-block on idb %s",
	       idb->namestring);
    }
    if (!idb_remove_swidb_from_list(idb, SWIDBLIST_XNS)) {
	buginf("\nXNS: failed to remove XNS idb %s from XNS list",
	       idb->namestring);
    }
}

/*
 * xns_if_config
 *
 * enables XNS routing on a specified interface. Called as a result of the
 * "xns network <n>" interface command.
 */
static void
xns_if_config (parseinfo *csb)
{
    ulong	net_number;
    idbtype	*idb;
    hwidbtype	*hwidb;
    idbtype	*xfc;
    xnssb	*xsb;
    xnssb	*xsb1;			/* for use in idb loops */
    list_element *l_elt;
    ulong	status;
    swidb_sb_t	type = SWIDB_SB_XNS;

    if (csb == NULL || csb->interface == NULL)
	return;
    idb = csb->interface;
    hwidb = idb->hwptr;
    net_number = GETOBJ(int,1);
    /*
     * If we're adding an "numbered" interface, make sure that
     * the same network number is not configured on any other
     * interface on this router. If so, whine and return.
     */
    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_XNS, l_elt, xfc) {
	xsb1 = idb_get_swsb(xfc, SWIDB_SB_XNS);
	if (   (xsb1 && xsb1->xnsnet != 0) && (xfc != idb)
	    && (xsb1->xnsnet == net_number)) {
	    printf("\n%%XNS network %u already exists on"
		   " interface %s", xsb1->xnsnet, xfc->namestring);
	    return;
	}
    }
    /*
     * Get any sub-block currently allocated on the idb.
     * If one is already there, it means that someone previously
     * configured "xns network" on this interface and now they
     * are repeating the command, possibly with a different
     * network number.
     */
    xsb = idb_get_swsb(idb, SWIDB_SB_XNS);
    if (xsb) {
	if (xnspkt_debug)
	    buginf("\nXNS: pre-existing xsb");
	grip_del_rt(xsb->xnsnet, TRUE, xns_table, xns_running, xnsrip_debug);
	grip_prune_idb(idb, xns_table, &xns_routes, "XNS", xnsrip_debug);
    } else {
	xsb = xns_init_sb(NULL);
	if (xsb == NULL) {
	    return;
	}
	if (!idb_add_swsb(idb, &type, xsb)) {
	    if (xnspkt_debug)
		buginf("\nXNS: failed to add sub-block to %s",
		       idb->namestring);
	    free(xsb);
	    return;
	}
	if (!idb_add_swidb_to_list(idb, SWIDBLIST_XNS)) {
	    if (xnspkt_debug)
		idb_report_add_error("XNS");
	    idb_delete_swsb(idb, SWIDB_SB_XNS);
	    free(xsb);
	    return;
	}
    }
    /*
     * Determine and set the XNS encapsulation type here.
     * This sort of stuff has no business being in the drivers.
     */
    status = hwidb->status;
    xns_swidb_encap(idb, ET_NULL);

    /* Initialize the xns idb */
    xns_init_idb(idb, TRUE);

    /*
     * this should probably go into the clause where we're configuring
     * a network on an interface which previously had a network.
     */
    grip_del_rt(net_number, TRUE, xns_table, xns_running, xnsrip_debug);

    /* XXX unnumbered interfaces execute the following logic */

    xnscache_increment_version(NULL, NULL, FALSE);
    xsb->idb = xsb->xnsrip.idb = xsb->ubxnsrip.idb = idb;
    xsb->xnsnet = net_number;
    grip_add_rt(&xsb->xnsrip, net_number, 0L, NULL, NET_CONNECTED,
		xns_table, xns_running, FALSE, xnsrip_debug);
    if (hwidb->status & IDB_IEEEMASK || hwidb->status & IDB_ATM) {/* a LAN? */
	/* Force the hardware to have a decent address! */
	/*
	 * ???-dsa OK, this sort of nonsense must really stop.
	 * resetting the interface just to get it to have a canonical
	 * address is not a way to make a network stable.
	 */
	(*hwidb->reset)(hwidb);
	ieee_copy(hwidb->hardware, xsb->xns_node);
    } else {
	ieee_copy(xns_defaddr, xsb->xns_node);
    }
    GET_TIMESTAMP(xns_timer);
    xsb->xnsrip.updatetimer = xns_timer;
    xsb->ubxnsrip.updatetimer = xns_timer;
    xns_fastsetup(hwidb);
    xns_rxtypes_update(idb);
}

/*
 * xnsif_command
 * Process XNS interface keywords
 */
static const char flooderr[] =
    "\n%%Use \"xns flood [broadcast|specific] [net-zero|allnets]\"";

void xnsif_command (parseinfo *csb)
{
    idbtype	*idb;
    hwidbtype	*hwidb = NULL;
    xnssb	*xsb = NULL;


#ifdef NOT_YET
/* XXX XNSIF_UNNUMBERED not yet supported; See note in cfg_int_xns.h */
    xns_ripentrytype *entry;
#endif NOT_YET
    boolean unnumbered;
    
    unnumbered = FALSE;
    idb = csb->interface;
    if (idb) {
	hwidb = hwidb_or_null(idb);
	xsb = idb_get_swsb(idb, SWIDB_SB_XNS);
    } else {
	return;
    }
    if (!xns_running)
      return;
    /*
     * It is possible to proceed beyond this point with a NULL xsb.
     * Check below where assumptions are made that xsb must be != NULL.
     */
    if ((csb->nvgen || (csb->which != XNSIF_NETWORK)) &&
			 ((xsb == NULL) || (xsb && xsb->xnsnet == 0))) {
      return;
    }
      
    if (csb->nvgen) {
	switch (csb->which) {

	  case XNSIF_ACCESS:
	    nv_write(xsb->xns_accesslist != 0, "%s %d",
		     csb->nv_command, xsb->xns_accesslist);
	    break;

	  case XNSIF_ENCTYPE:
	    if (hwidb->status & IDB_ETHER) {
		nv_write(xsb->xns_enctype != ET_ARPA, "%s %s",
			 csb->nv_command, encstring(xsb->xns_enctype));
	    } else {
		if (hwidb->status & IDB_802ONLY) {
		    nv_write(xsb->xns_enctype != ET_SNAP,
			     "%s %s", csb->nv_command,
			     encstring(xsb->xns_enctype));
		}
	    }
	    break;

	  case XNSIF_HELPER:
	    if (xsb->xns_helpernet == -1L) {
		nv_write(TRUE, "%s -1.%e", csb->nv_command,
			 xsb->xns_helperaddr);
	    } else {
		nv_write(xsb->xns_helpernet,
			 "%s %u.%e", csb->nv_command, xsb->xns_helpernet,
			 xsb->xns_helperaddr);
	    }
	    break;

#ifdef NOT_YET
/* XXX XNSIF_UNNUMBERED not yet supported; See note in cfg_int_xns.h
 */
	  case XNSIF_UNNUMBERED:
	    if (xsb->xnsnet) {
                entry = grip_find_rt(xsb->xnsnet, xns_table);
		nv_write(entry && entry->unnumbered, csb->nv_command);
            }
	    break;
#endif NOT_YET

	  case XNSIF_NETWORK:
	    nv_write(TRUE, "%s %u", csb->nv_command, xsb->xnsnet);
	    break;

	  case XNSIF_ROUTEC:
	    nv_write(!hwidb->xns_routecache, "no %s", csb->nv_command);
	    break;

	  case XNSIF_IN_FILTER:
	    nv_write(xsb->xns_in_filterlist != 0, "%s %d",
		     csb->nv_command, xsb->xns_in_filterlist);
	    break;

	  case XNSIF_OUT_FILTER:
	    nv_write(xsb->xns_out_filterlist != 0, "%s %d",
		     csb->nv_command, xsb->xns_out_filterlist);
	    break;

	  case XNSIF_ROUTER_FILTER:
	    nv_write(xsb->xns_router_filterlist != 0, "%s %d",
		     csb->nv_command, xsb->xns_router_filterlist);
	    break;

	  case XNSIF_HEARRIP:
	    if (xns_ub_emulation && xsb->xns_hear_rip) {
		if (xsb->xns_hear_rip_list != 0) {
		    nv_write(TRUE, "%s %d", csb->nv_command,
			     xsb->xns_hear_rip_list);
		} else {
		    nv_write(TRUE, csb->nv_command);
		}
	    }
	    break;

	  case XNSIF_UPDATETIME:
	    nv_write(xsb->xnsrip.update_interval != RIP_DEFAULT_UPDATE, "%s %d",
		     csb->nv_command, xsb->xnsrip.update_interval/ONESEC);
	    break;

	  case XNSIF_OLD3COM:
	    if (hwidb->status & IDB_TR) {
		nv_write(hwidb->tr_3com_hack, csb->nv_command);
	    }
	    break;

	  case XNSIF_FLOOD:
	    nv_write(xsb->xns_fld_bcast_netzero, "%s broadcast net-zero",
		     csb->nv_command);
	    nv_write(xsb->xns_fld_bcast_allnets, "%s broadcast allnets",
		     csb->nv_command);
	    nv_write(xsb->xns_fld_spec_allnets, "%s specific allnets",
		     csb->nv_command);
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    if (!xns_running) {
	printf(xns_notenabled);
	return;
    }

    switch (csb->which) {
      case XNSIF_ACCESS:
	if (csb->sense) {
	    xsb->xns_accesslist = GETOBJ(int,1);
	} else {
	    xsb->xns_accesslist = 0;
	}
	break;

      case XNSIF_ENCTYPE:
	if (!(hwidb->status & IDB_IEEEMASK)) {
	    printf("\n%%XNS encapsulation may be specified only for"
		   " 802.x interfaces");
	    return;
	}
        if (csb->sense) {
	    if (((hwidb->status & IDB_ETHER) && GETOBJ(int,1) == ET_ARPA) ||
		((hwidb->status & IDB_TR) && (GETOBJ(int,1) == ET_3COM_TR ||
					    GETOBJ(int,1) == ET_UB_TR || 
					    GETOBJ(int,1) == ET_SNAP))) {
		xsb->xns_enctype = GETOBJ(int,1);
	    } else {
		printf("\n%%Bad XNS encapsulation type - %s",csb->line);
	    }
        } else {
            if (hwidb->status & IDB_ETHER) {
                xsb->xns_enctype =  ET_ARPA;
            } else {
		if (hwidb->status & IDB_802ONLY) {
		    xsb->xns_enctype = ET_SNAP;
		}
	    }
	}
        break;

      case XNSIF_HELPER:
	if (csb->sense) {
	    xsb->xns_helpernet = GETOBJ(paddr,1)->xns_addr.net;
            ieee_copy(GETOBJ(paddr,1)->xns_addr.host, xsb->xns_helperaddr);
            if (xsb->xns_helpernet == -1L) {
                /* This is a special case; it really turns on flooding */
                if (!ieee_equal(xsb->xns_helperaddr, xns_allhosts)) {
                    xsb->xns_helpernet = 0L;
                    printf(xns_badaddr, "malformed address");
                    return;
                }
                xsb->xns_helpernet = 0L;
                xsb->xns_fld_bcast_allnets = TRUE;
            }
	} else {
	    xsb->xns_helpernet = 0L;
	}
	xnscache_increment_version(NULL, NULL, FALSE);
	break;

#ifdef NOT_YET
/* XXX XNSIF_UNNUMBERED not yet supported; See note in cfg_int_xns.h
 * XXX This code needs to be checked for correctness.
 */
      case XNSIF_UNNUMBERED:
	if (!is_p2p(idb)) {
            printf("\n%%Point-to-Point (non-multi-access) interfaces only");
            return;
	}
	if (!csb->sense) {
	    if (xsb->xnsnet) {
                entry = grip_find_rt(xsb->xnsnet, xns_table);
                entry->unnumbered = FALSE;
            }
	    return;
	}
	if (xsb->xnsnet) {
	    printf("\n%%Xns net exists, please remove before making unnumbered");
	    return;
	}
	unnumbered = TRUE;
	/* Fall thru to add a new net as unnumbered */
#endif NOT_YET

      case XNSIF_NETWORK:
        if (csb->sense == FALSE) {
	    xns_if_deconfig(csb);
        } else {
	    xns_if_config(csb);
	}
        break;

      case XNSIF_ROUTEC:
	hwidb->xns_routecache = csb->set_to_default || csb->sense;
	xns_fastsetup(hwidb);
	break;

      case XNSIF_IN_FILTER:
	if (csb->sense) {
	    xsb->xns_in_filterlist = GETOBJ(int,1);
	} else {
	    xsb->xns_in_filterlist = 0;
	}
	break;

      case XNSIF_OUT_FILTER:
	if (csb->sense) {
	    xsb->xns_out_filterlist = GETOBJ(int,1);
	} else {
	    xsb->xns_out_filterlist = 0;
	}
	break;

      case XNSIF_ROUTER_FILTER:
	if (csb->sense) {
	    xsb->xns_out_filterlist = GETOBJ(int,1);
	} else {
	    xsb->xns_out_filterlist = 0;
	}
	break;

      case XNSIF_HEARRIP:
	if (csb->sense) {
	    xsb->xns_hear_rip_list = GETOBJ(int,1);
	    xsb->xns_hear_rip = TRUE;
	} else {
	    xsb->xns_hear_rip_list =0;
	    xsb->xns_hear_rip = FALSE;
	}
	break;

      case XNSIF_UPDATETIME:
	if (csb->sense) {
	    xsb->xnsrip.update_interval = GETOBJ(int,1) * ONESEC;
	} else {
	    xsb->xnsrip.update_interval = RIP_DEFAULT_UPDATE;
	}
	break;

      case XNSIF_OLD3COM:
	if (!(hwidb->status & IDB_TR)) {
	    printf("\n%%old3com only allowed on Token Rings");
	    return;
	}
	hwidb->tr_3com_hack = csb->sense;
	break;

      case XNSIF_FLOOD:
        if (GETOBJ(int,1) == XNSIF_BROADCAST) {
            if (GETOBJ(int,2) == XNSIF_NETZERO) {
                xsb->xns_fld_bcast_netzero = csb->sense;
            } else {
		if (GETOBJ(int,2) == XNSIF_ALLNETS) {
		    xsb->xns_fld_bcast_allnets = csb->sense;
		} else {
		    printf(flooderr);
		}
	    }
        } else {
	    if (GETOBJ(int,1) == XNSIF_SPECIFIC) {
		if (GETOBJ(int,2) == XNSIF_NETZERO) {
		   printf("\n%%\"xns flood specific net-zero\" not supported");
		} else {
		    if (GETOBJ(int,2) == XNSIF_ALLNETS) {
			xsb->xns_fld_spec_allnets = csb->sense;
		    } else {
			printf(flooderr);
		    }
		}
	    } else {
		printf(flooderr);
	    }
	}
        break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * xnsnaccess_command
 * Parse xns normal access list command.
 * access-list <number> <permit | deny> <source network | -1><.><source addr>
 *             <source mask> <destination network | -1><.><destination addr>
 *             <destination mask>
 *
 * The only required parmeter is the source network, which may be -1 -- 
 * indicates ALL networks. A value of zero (0) is disallowed
 * 		access-list 400 deny -1
 * The source address parameter is an optional parmeter. If specified, it
 * be separated from the source network by a period (.).
 *              access-list 400 deny -1.0000.0c00.1111
 * The source mask is an optional parameter, but it may only be specified if
 * a source address was specfied.
 *              access-list 400 deny -1.0000.0c00.1111 0000.00ff.ffff
 *              access-list 400 deny -1 0000.00ff.ffff (is illegal)
 * The rules for  the destination parameters are identical -- except that
 * the destination network is optional.
 *              access-list 400 deny -1 2 
 *              access-list 400 deny -1 2.0000.0c00.1111
 *              access-list 400 deny -1 2.0000.0c00.1111 0000.00ff.ffff
 * The maximum definition would result in
 *              access-list 400 deny 1.1111.1111.1111 0000.00ff.ffff
 *                                   2.2222.2222.2222 0000.00ff.ffff
 */
void xnsnaccess_command (parseinfo *csb)
{
    xnsnacctype *item, *wrk_item;
    xnsnacctype *entry;
    int number;
    boolean grant;

    number = GETOBJ(int,1);
    grant = GETOBJ(int,2);

    if (csb->nvgen) {
	nv_xnsaccess(csb);
	return;
    }

    if (csb->sense == FALSE) {
        /*
         * No access-list command. Delink entries and free memory.
         */
        item = xns_get_std_acl(number);
        xns_set_std_acl(number, NULL);
        while (item) {
            wrk_item = item->next;
            free(item);
            item = wrk_item;
        }
        return;
    }

    entry = malloc(sizeof(xnsnacctype));
    if (!entry) {
	return;
    }
    entry->grant = grant;
    entry->flags = GETOBJ(int,3);

    /*
     * Network/host masks are always applied. Init to 0 in case
     * they are not specified:
     */
    entry->srcnetmask = entry->dstnetmask = 0;
    entry->srchostmask.u.lword[0] = entry->dsthostmask.u.lword[0] = 0;
    entry->srchostmask.u.sword[2] = entry->dsthostmask.u.sword[2] = 0;

    /*
     * Parse the required source network address.
     */
    if (entry->flags & XNS_SRCNET) {
	if (entry->flags & XNS_SRCHOST) {
	    entry->srcnet = GETOBJ(paddr,1)->xns_addr.net;
	} else {
	    entry->srcnet = GETOBJ(int,4);
	}
    } else {
	printf("\n%%XNS network address required");
	free(entry);
	return;
    }
    if (entry->flags & XNS_SRCHOST) {
	ieee_copy(GETOBJ(paddr,1)->xns_addr.host,
	      (uchar *)&(entry->srchost));
    }
    if (entry->flags & XNS_SRCHOSTMASK) {
	ieee_copy(GETOBJ(paddr,2)->xns_addr.host,
	      (uchar *)&(entry->srchostmask));
    }
    if (entry->flags & XNS_DSTNET) {
	if (entry->flags & XNS_DSTHOST) {
	    entry->dstnet = GETOBJ(paddr,3)->xns_addr.net;
	} else {
	    entry->dstnet = GETOBJ(int,5);
	}
    }
    if (entry->flags & XNS_DSTHOST) {
	ieee_copy(GETOBJ(paddr,3)->xns_addr.host,
	      (uchar *)&(entry->dsthost));
    }
    if (entry->flags & XNS_DSTHOSTMASK) {
	ieee_copy(GETOBJ(paddr,4)->xns_addr.host,
	      (uchar *)&(entry->dsthostmask));
    }
    
    /*
     * Check to ensure that we don't add a duplicate entry.
     */
    wrk_item = xns_get_std_acl(number);
    for (item = wrk_item; item; wrk_item = item, item = item->next) {
	if (bcmp((uchar *)&entry->grant, (uchar *)&item->grant,
		 sizeof(xnsnacctype) - sizeof(item->next)) == 0) {
	    free(entry);
	    return;
	}
    }
    
    /*
     * Finally add the new item to the list.
     */
    if (wrk_item == NULL) {
	xns_set_std_acl(number, entry);
    } else {
	wrk_item->next = entry;
    }
}

/*
 * xnseaccess_command
 * Parse xns extended access list command.
 * access-list [number] [permit | deny] [protocol] [source network]<.>
 *             <source address> <source address mask> <source socket>
 *             <destination network><.><destination address.> 
 *	       (destination address mask> <destination socket>
 *
 * The only required parmeter is the protocol.
 * 		access-list 500 deny 1
 */
void xnseaccess_command (parseinfo *csb)
{
    xnseacctype *item, *wrk_item;
    xnseacctype *entry;
    int number;
    boolean grant;
    
    number = GETOBJ(int,1);
    grant = GETOBJ(int,2);

    if (csb->nvgen) {
	/* NV generation is taken care of in xnsnaccess_command  
	 *   nv_xnsaccess(csb);
	 */
	return;
    }

    if (csb->sense == FALSE) {
	xnseacctype *itemp, *wrk_itemp;

        /*
         * No access-list command. Delink entries and free memory.
         */
        itemp = xns_get_ext_acl(number);
        xns_set_ext_acl(number, NULL);
        while (itemp) {
            wrk_itemp = itemp->next;
            free(itemp);
            itemp = wrk_itemp;
        }
        return;
    }

    entry = malloc(sizeof(xnseacctype));
    if (!entry) {
	return;
    }
    entry->grant = grant;
    entry->flags = GETOBJ(int,3);

    /*
     * Network/host masks are always applied. Init to 0 in case
     * they are not specified:
     */
    entry->srcnetmask = entry->dstnetmask = 0;
    entry->srchostmask.u.lword[0] = entry->dsthostmask.u.lword[0] = 0;
    entry->srchostmask.u.sword[2] = entry->dsthostmask.u.sword[2] = 0;

    if (entry->flags & XNS_PROTOCOL) {
	entry->protocol = GETOBJ(int,6);
    }
    if (entry->flags & XNS_SRCNET) {
	if (entry->flags & XNS_SRCHOST) {
	    entry->srcnet = GETOBJ(paddr,1)->xns_addr.net;
	} else {
	    entry->srcnet = GETOBJ(int,4);
	}
    }
    if (entry->flags & XNS_SRCHOST) {
	ieee_copy(GETOBJ(paddr,1)->xns_addr.host,
	      (uchar *)&(entry->srchost));
    }
    if (entry->flags & XNS_SRCHOSTMASK) {
      if (entry->flags & XNS_SRCNETMASK) {
	entry->srcnetmask = GETOBJ(paddr,2)->xns_addr.net;
	ieee_copy(GETOBJ(paddr,2)->xns_addr.host,
		  (uchar *)&(entry->srchostmask));
	} else {
	entry->srcnetmask = (ulong) 0x0000;
	ieee_copy(GETOBJ(paddr,2)->xns_addr.host,
		  (uchar *)&(entry->srchostmask));
      }
    }
    if (entry->flags & XNS_SRCSOCKET) {
	entry->srcsocket = GETOBJ(int,7);
    }
    if (entry->flags & XNS_DSTNET) {
	if (entry->flags & XNS_DSTHOST) {
	    entry->dstnet = GETOBJ(paddr,3)->xns_addr.net;
	} else {
	    entry->dstnet = GETOBJ(int,5);
	}
    }
    if (entry->flags & XNS_DSTHOST) {
	ieee_copy(GETOBJ(paddr,3)->xns_addr.host,
	      (uchar *)&(entry->dsthost));
    }
    if (entry->flags & XNS_DSTHOSTMASK) {
      if (entry->flags & XNS_DSTNETMASK) {
	entry->dstnetmask = GETOBJ(paddr,4)->xns_addr.net;
	ieee_copy(GETOBJ(paddr,4)->xns_addr.host,
	      (uchar *)&(entry->dsthostmask));
      } else {
	entry->dstnetmask = (ulong) 0x0000;
	ieee_copy(GETOBJ(paddr,4)->xns_addr.host,
	      (uchar *)&(entry->dsthostmask));
      }
    }
    if (entry->flags & XNS_DSTSOCKET) {
	entry->dstsocket = GETOBJ(int,8);
    }
    if (entry->flags & XNS_PEPCLIENT) {
	entry->pep_clienttype = GETOBJ(int,9);
    }
    
    /*
     * Check to ensure that we don't add a duplicate entry.
     */
    wrk_item = xns_get_ext_acl(number);
    for (item = wrk_item; item; wrk_item = item, item = item->next) {
	if (bcmp((uchar *)&entry->grant, (uchar *)&item->grant
		 , sizeof(xnseacctype) - sizeof(item->next)) == 0) {
	    free(entry);
	    return;
	}
    }
    
    /*
     * Finally add the new item to the list.
     */
    if (wrk_item == NULL) {
	xns_set_ext_acl(number, entry);
    } else {
	wrk_item->next = entry;	    
    }
}

void clear_xns_command (parseinfo *csb)
{
    ulong net;

    if (!xns_running) {
	printf(xns_notenabled);
	return;
    }

    switch (GETOBJ(int,10)) {
      case CLEAR_XNS_CACHE:
	xns_fscache_flush();
	break;
      case CLEAR_XNS_ROUTE:
	if (GETOBJ(int,2)) {
	    grip_purgetable(xns_table);
	    xns_sendrequest();	    
	} else {
	    if (GETOBJ(int,1)) {
		net = GETOBJ(int,1);
	    }else{
		net = GETOBJ(paddr,1)->xns_addr.net;
	    }
	    grip_del_rt(net, FALSE, xns_table, xns_running, 
			xnsrip_debug);
	}
	break;
      default:
	bad_parser_subcommand(csb, GETOBJ(int,10));
	break;
    }
}

/*
 * nv_xnsaccess
 * Function to write out the XNS access list(s).
 */
void nv_xnsaccess (parseinfo *csb)
{
    int i;
    xnseacctype *item;
    
    for (i = MINXNSACC; i <= MAXEXNSACC; i++) {
	for (item = xns_get_ext_acl(i); item; item = item->next) {
	    nv_write(TRUE,"%s %d %s", csb->nv_command, i,
		     item->grant ? "permit" : "deny ");
	    nv_add((item->flags & XNS_PROTOCOL)," %d", item->protocol);
	    nv_add((item->flags & XNS_SRCNET)," %d", item->srcnet);
	    nv_add((item->flags & XNS_SRCHOST),".%e", &item->srchost);
	    if (item->flags & XNS_SRCNETMASK) {
	      nv_add((item->flags & XNS_SRCNETMASK)," %d", item->srcnetmask);
	      nv_add((item->flags & XNS_SRCHOSTMASK),".%e", &item->srchostmask);
	    } else {
	      nv_add((item->flags & XNS_SRCHOSTMASK)," %e", &item->srchostmask);
	    }
	    nv_add((item->flags & XNS_SRCSOCKET)," 0x%-04x", item->srcsocket);
	    nv_add((item->flags & XNS_DSTNET)," %d", item->dstnet);	
	    nv_add((item->flags & XNS_DSTHOST),".%e", &item->dsthost);
	    if (item->flags & XNS_DSTNETMASK) {
	      nv_add((item->flags & XNS_DSTNETMASK)," %d", item->dstnetmask);
	      nv_add((item->flags & XNS_DSTHOSTMASK),".%e", &item->dsthostmask);
	    } else {
	      nv_add((item->flags & XNS_DSTHOSTMASK)," %e", &item->dsthostmask);
	    }
	    nv_add((item->flags & XNS_DSTSOCKET), " 0x%-04x", item->dstsocket);
	    nv_add((item->flags & XNS_PEPCLIENT), " %d", item->pep_clienttype);
	}
    }
}

/*
 * xns_start
 * Start XNS processing
 */

void xns_start (void)
{
    xns_table = malloc_named(sizeof(rip_routetable), "XNS RIP Table");
    if (!xns_table) {
	printf(nomemory);
	return;
    }
    xns_started = TRUE;
    (void)process_create(xns_input, "XNS Input", NORMAL_STACK, PRIO_HIGH);
    (void)cfork(xns_background, 0L, 0, "XNS Protocols", 0);
    (void)cfork(xns_router, 0L, 0, "XNS Router", 0);
}

/*
 * xns_enqueue
 * Pass XNS packet to XNS handler
 */

void xns_enqueue (paktype *pak)
{
    xnssb	*xsb;

    xsb = idb_get_swsb_inline(pak->if_input, SWIDB_SB_XNS);
    if (systeminit_flag && xns_running && (xsb && xsb->xnsnet)) {
	process_enqueue_pak(xnsQ, pak);
    } else {
	protocol_discard(pak, systeminit_flag && xns_running);
    }
}

/*
 * xns_input
 * All of XNS begins here
 */

/*
 * values that xns_forus returns
 */
#define XNS_ROUTEIT 0			/* route it */
#define XNS_CONSUMEIT 1			/* its for us */
#define XNS_PITCHIT 2			/* when we hear ourself */

static process xns_input (void)
{
    int		error;
    int		which;
    int		pak_count;
    int		len;
    paktype	*pak;
    xnshdrtype	*xns;
    idbtype	*idb;
    xnssb	*xsb;

    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    xnsQ = create_watched_queue("XNS Input", 0, 0);
    process_watch_queue(xnsQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_XNS, xns_enqueue, "xns_enqueue");

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

	    pak = process_dequeue(xnsQ);
	    if (pak == NULL) {
		break;
	    }
	    xns = (xnshdrtype *)xnsheadstart(pak);

	    idb = pak->if_input;
	    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
	    if ((xsb == NULL) || (xsb->xnsnet == 0)) {
		retbuffer(pak);
		continue;
	    }
	    if (xnspkt_debug)
	      buginf("\nXNS: src=%u.%e, dst=%u.%e, rcvd. on %s",
		     GET2WD(xns->ssnet), xns->shost, GET2WD(xns->ddnet),
		     xns->dhost, idb->namestring);

	    xns_stat[XNS_INPUT]++;
	    idb_increment_rx_proto_counts(pak, ACCT_PROTO_XNS);
	    
	    len = xns->len;
	    if (len & 1)
		len++;			/* odd packets preserve garbage */
	    /*
	     * Sanity check header fields
	     */
	    if ((len < XNS_MINSIZE) ||
		(len > XNS_MAXSIZE) ||
		(len > pak_get_pool_size((paktype*)pak))) {
		xns_dispose(XNS_FORMATERROR, pak);
		continue;
	    }
	    
	    /*
	     * Check checksum for validity
	     */
	    if (!xns_validcrc(pak)) {
		if (xns_forme(pak))
		    error = XNS_ERR_BADSUM;
		else
		    error = XNS_ERR_TBADSUM;
		xns_error(pak, error, 0, XNS_CKSUMERR);
		continue;
	    }
	    /*
	     * Is this packet aimed at us, or should it be forwarded, or
	     * pitched?
	     */
	    which = xns_forus(pak);
	    switch (which) {
	    case XNS_CONSUMEIT:
		if (xnspkt_debug)
		    buginf("\nXNS: src=%u.%e, dst=%u.%e, local processing",
			   GET2WD(xns->ssnet), xns->shost, GET2WD(xns->ddnet),
			   xns->dhost);
		p_enqueue(&xnsbgQ, pak);
		break;
	    case XNS_PITCHIT:
		if (xnspkt_debug)
		    buginf("\nXNS: src=%u.%e, dst=%u.%e, pitching!",
			   GET2WD(xns->ssnet), xns->shost, GET2WD(xns->ddnet),
			   xns->dhost);
		retbuffer(pak);
		break;
	    case XNS_ROUTEIT:
		xns_forward(pak);
		break;
	    }
	}
    }
}

/*
 * xns_forme
 * Determine if a packet is meant for one this hosts interfaces.
 */

boolean xns_forme (paktype *pak)
{
    xnshdrtype	*xns;
    idbtype	*idb;
    ulong	dnet;
    xnssb	*xsb;
    list_element *l_elt;

    xns = (xnshdrtype *)xnsheadstart(pak);

    dnet = GET2WD(xns->ddnet);
    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_XNS, l_elt, idb) {
	xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
	if (xsb == NULL)
	    continue;
	if (xsb->xnsnet == 0)
	    continue;
	if ((dnet != 0L) && (dnet != -1L) && (dnet != xsb->xnsnet))
	    continue;
	if (ieee_equal(xns->dhost, xsb->xns_node))
	    return (TRUE);
	
    }
    return (FALSE);
}

/*
 * xns_forus
 * Determine if a packet is meant for this host
 */

int xns_forus (paktype *pak)
{
    xnshdrtype	*xns;
    idbtype	*idb;
    xns_ripentrytype *p;
    ulong	snet, dnet;
    xnssb	*xsb;

    xns = (xnshdrtype *)xnsheadstart(pak);
    idb = pak->if_input;
    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
    snet = GET2WD(xns->ssnet);
    dnet = GET2WD(xns->ddnet);

    /* Only hear all nets packets on the primary interface of the path set
     * we'd use to get back to the source net. This should prevent undue
     * packet duplication. If the source net is direct-connected, we assume
     * that the input interface is the primary interface, since we don't
     * support two interfaces with the same network number. The same applies
     * to net-zero packets, since they may get flooded in some configurations.
     */
    if (   ((dnet == -1L) || (dnet == 0L))
	&& (snet != 0) && (snet != xsb->xnsnet)) {
	p = grip_find_rt(snet ? snet : xsb->xnsnet, xns_table);
	if ((p != NULL) && (idb != p->path_list->idb))
	    return (XNS_PITCHIT);
    }

    /*
     * Check for XNS broadcasts.
     * XNS broadcasts have to be MAC-level multicasts as well
     * as protocol broadcasts.
     */
    if (((idb->hwptr->status & IDB_TR) &&
	 (xns->dhost[0] & XNS_MULTICAST_TR)) ||
	(!(idb->hwptr->status & IDB_TR) &&
	 (xns->dhost[0] & XNS_MULTICAST_ETHER))) {
	/*
	 * Forward broadcast if the destination network is not
	 * broadcast or unspecified and the source network is not
	 * unspecified and the destination network does not
	 * match the input interface network.
	 */
	if (dnet != XNS_BROADCAST_NET && dnet != 0L &&
	    snet != 0L &&  xsb->xnsnet != dnet)
	    return (XNS_ROUTEIT);

	xns_stat[XNS_BCASTIN]++;
	/*
	 * Ignore our own broadcasts
	 */
	if (ieee_equal(xsb->xns_node, xns->shost))
	    return (XNS_PITCHIT);
	pak->flags |= PAK_BROADCAST;
	xns_stat[XNS_LOCALDEST]++;
	return (XNS_CONSUMEIT);
    }
    if (xns_forme(pak)) {
	xns_stat[XNS_LOCALDEST]++;
	return (XNS_CONSUMEIT);
    }
    /* If it's an all-nets specific-host packet, send it to the process
     * level for flooding. Not strictly speaking a packet for consumption,
     * but we'll stretch the point.
     */
    return ((dnet == -1L) ? XNS_CONSUMEIT : XNS_ROUTEIT);
}

/*
 * xns_forward
 * Forward an XNS gram
 */

void xns_forward (paktype *pak)
{
    xnshdrtype	*xns;
    ushort	hops, oldt;
    
    xns = (xnshdrtype *)xnsheadstart(pak);

    oldt = (xns->tc << 8) | xns->pt;
    hops = (xns->tc & XNS_TC_HOP_MASK) + 1; /* Increment only hop count bits */
    if (hops >= XNS_INFINITY) {
	if (xnspkt_debug)
	    buginf("\nXNS: src=%u.%e, dst=%u.%e, hop count exceeded",
		   GET2WD(xns->ssnet), xns->shost, GET2WD(xns->ddnet),
		   xns->dhost);
	xns_error(pak, XNS_ERR_2OLD, 0, XNS_HOPCOUNT);
	return;
    }
    if (xnspkt_debug)
	buginf("\nXNS: src=%u.%e, dst=%u.%e, forwarding...",
	       GET2WD(xns->ssnet), xns->shost, GET2WD(xns->ddnet),
	       xns->dhost);
    /* Update hops field within tc, preserving reserved bits */
    xns->tc = (hops |= (xns->tc & XNS_TC_RSV_MASK));
    xns_qcsum(xnsheadstart(pak), oldt, 2);
    if (xns_send(pak))
	xns_stat[XNS_FORWARDED]++;
}

/*
 * xns_send
 * send an XNS gram
 */

boolean xns_send (paktype *pak)
{
    xnshdrtype	*xns;
    uchar	*dhost;
    xns_ripentrytype *p;
    ulong	dnet;
    xnssb	*xsb;

    xns = (xnshdrtype *)xnsheadstart(pak);

    dnet = GET2WD(xns->ddnet); 
    p = (*xns_table)[nethash(dnet)];
    dhost = grip_first_hop(dnet, &(xns->dhost[0]), &pak->if_output, p);
    if ((dhost == NULL) || (pak->if_output == NULL)) {	/* no route */
	if (xnspkt_debug)
	    buginf("\nXNS: src=%u.%e, dst=%u.%e, no route",
		   GET2WD(xns->ssnet), xns->shost, dnet, xns->dhost);
	xns_error(pak, XNS_ERR_UNREACH, 0, XNS_NOROUTE);
	return (FALSE);
    }
    xsb = idb_get_swsb_inline(pak->if_output, SWIDB_SB_XNS);
    /*
     * Before sending the packet, see if access lists are defined.
     * Only do access checks for routed traffic (if_input != NULL).
     */
    if (pak->if_input) {
	if (xsb->xns_accesslist) {
	    if (!xns_accesscheck(pak, xsb->xns_accesslist)) {
		retbuffer(pak);
		if (xnspkt_debug)
		    buginf("\nXNS: src=%u.%e, dst=%u.%e, filtered",
			   GET2WD(xns->ssnet), xns->shost, dnet, xns->dhost);
		return (FALSE);
	    }
	}
    }
    if (xnspkt_debug)
	buginf("\nXNS: src=%u.%e, dst=%u.%e, gw=%u.%e",
	       GET2WD(xns->ssnet), xns->shost, dnet, xns->dhost,
	       xsb->xnsnet, dhost);
    
    return (xns_send1(pak, dhost));
}

/*
 * xns_send1
 * Other half of sending XNS grams
 */

boolean xns_send1 (register paktype *pak, uchar const *dhost)
{
    xnshdrtype	*xns;
    ulong	len;
    idbtype	*idb;
    hwidbtype	*hwidb;
    xnssb	*xsb;

    xns = (xnshdrtype *)xnsheadstart(pak);
    idb = pak->if_output;
    pak->acct_proto = ACCT_PROTO_XNS;
    if (idb == NULL) {
	errmsg(&msgsym(NULLIDB, SYS), "xns_send1");
	retbuffer(pak);
	return (FALSE);
    }
    if (!interface_up_simple(idb)) {
	retbuffer(pak);
	return (FALSE);
    }
    hwidb = idb->hwptr;
    if (((hwidb->status & IDB_TR) && (dhost[0] & XNS_MULTICAST_TR)) ||
	(!(hwidb->status & IDB_TR) && (dhost[0] & XNS_MULTICAST_ETHER)))
	xns_stat[XNS_BCASTOUT]++;
    len = xns->len;
    if (len & 1)			/* if len odd, round up */
	len++;
    pak->linktype = LINK_XNS;
    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
    pak->enctype = xsb->xns_enctype;
    if (pak_encap(hwidb, pak, len, (long)dhost)) {
	xns_stat[XNS_OUTPUT]++;
	datagram_out(pak);		/* queue it for output */
	return (TRUE);
    }
    if (xnspkt_debug)
	buginf("\nXNS: src=%u.%e, dst=%u.%e, encapsulation failed",
	       GET2WD(xns->ssnet), xns->shost, GET2WD(xns->ddnet),
	       xns->dhost);
    xns_error(pak, XNS_ERR_UNREACH, 0, XNS_ENCAPSFAILED);
    return (FALSE);
}

/*
 * xns_background
 * Perform XNS background handling
 */

forktype xns_background (void)
{
    paktype	*pak;
    xnshdrtype	*xns;
    xnsechotype	*ep;
    xns_cisco_ping *cp;
    sys_timestamp time_sent;
    
    edisms((blockproc *)systeminitBLOCK, (ulong)&xnsbgQ);
    process_set_priority(PRIO_LOW);
    queue_init(&xnsripQ, 0);
    while (TRUE) {
	edisms((blockproc *)queueBLOCK, (long)&xnsbgQ);
	while ((pak = dequeue(&xnsbgQ))) {
	    xns = (xnshdrtype *)xnsheadstart(pak);

	    switch (xns->pt) {
	    case XNS_TYPE_ECHO:		/* echo protocol */
		if (xns->dsock == XNS_SOCK_ECHO) {
		    xns_echo(pak);
		    break;
		} else if (!QUEUEEMPTY(&xnsechoQ)) {		
		    xns_stat[XNS_ECHOREPIN]++;
		    for (ep = (xnsechotype *)xnsechoQ.qhead; ep;
			 ep = ep->next) {
			if (ep->socket == xns->dsock) {
			    if (xns->len >= XNS_MINECHOSIZE) {
				cp = (xns_cisco_ping *)XNSDATASTART(pak);
				PAK_GET_TS(&cp->time_sent, &time_sent);
				ep->deltatime = ELAPSED_TIME(time_sent);
			    } else
				ep->deltatime = 0;
			    ep->how = '!';
			    ep->active = FALSE;
			    p_unqueue(&xnsechoQ, ep);
			    break;
			}
		    }
		}
		retbuffer(pak);
		break;
	    case XNS_TYPE_ERROR:	/* error protocol */
		xns_stat[XNS_ERRIN]++;
		retbuffer(pak);
		break;
	    case XNS_TYPE_RIP:		/* routing information protocol */
	    case XNS_TYPE_UBIPC:        /* UB routing... if socket matches */
		if ((xns->pt == XNS_TYPE_RIP) || 
		    ((xns->pt == XNS_TYPE_UBIPC) &&
		     (xns->dsock == XNS_SOCK_UBIPC))) {
		    enqueue(&xnsripQ, pak);
		    break;
		}

	    default:
		if (xns_forwarding(pak))
		    break;
		else {
		    if (xnspkt_debug)
			buginf("\nXNS: unknown from %u.%e(%u) to %u.%e(%u) type %u",
			       GET2WD(xns->ssnet), xns->shost, xns->ssock,
			       GET2WD(xns->ddnet), xns->dhost, xns->dsock,
			       xns->pt);
		    xns_stat[XNS_UNKNOWN]++;
		    retbuffer(pak);
		    break;
		}
	    }
	}
    }
}


/*
 * xns_echo
 * Handle echo protocol
 */

void xns_echo (paktype *pak)
{
    xnshdrtype	*xns;
    ulong	tnet, dnet;
    ushort	tsock;
    uchar	thost[IEEEBYTES];
    hwidbtype	*hwidb;
    xnssb	*xsb;
    xns_cisco_ping *cp;

    xns = (xnshdrtype *)xnsheadstart(pak);
    xsb = idb_get_swsb(pak->if_input, SWIDB_SB_XNS);
    hwidb = pak->if_input->hwptr;
    xns_stat[XNS_ECHOREQIN]++;
    tnet = GET2WD(xns->ssnet);
    dnet = GET2WD(xns->ddnet);
    ieee_copy(xns->shost, thost);
    tsock = xns->ssock;
    PUT2WD(xns->ssnet,((dnet != -1L && dnet) ? dnet : xsb->xnsnet));
    if (GET2WD(xns->ssnet) == 0L)
	PUT2WD(xns->ssnet, xsb->xnsnet);
    if (((hwidb->status & IDB_TR) && (xns->dhost[0] & XNS_MULTICAST_TR)) ||
	(!(hwidb->status & IDB_TR) && (xns->dhost[0] & XNS_MULTICAST_ETHER)))
	ieee_copy(xsb->xns_node, xns->shost);
    else
	ieee_copy(xns->dhost, xns->shost);
    xns->ssock = xns->dsock;
    PUT2WD(xns->ddnet, tnet);
    ieee_copy(thost, xns->dhost);
    xns->dsock = tsock;
    xns->tc = 0;
    cp = (xns_cisco_ping *)XNSDATASTART(pak);
    cp->type = XNS_ECHO_REPLY;
    pak->flags |= PAK_HEARSELF;
    xns->cksum = 0;
    xns_crc(pak);
    if (xns_send(pak))
	xns_stat[XNS_ECHOREPOUT]++;
}


/*
 * xns_error
 * Handler sending an XNS error packet
 */

void xns_error (
    paktype *pak,
    int type,
    int code,
    int local)
{
    xnshdrtype	*xns;
    xnshdrtype	*errpak_xns;
    paktype	*errpak;
    xnserrtype	*errtype;
    idbtype	*idb;
    xnssb	*xsb = NULL;

    xns = (xnshdrtype *)xnsheadstart(pak);
    /*
     * Don't generate responses to multicast, or a packet with
     * bad length or packets that we source, or the source packet type is 
     * error protocol.
     */
    idb = pak->if_input;
    if (idb)
	xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
    if (idb == NULL || xsb == NULL || xns->len < XNS_MINSIZE ||
	ieee_equal(xns->dhost, xns_allhosts) || xns->pt == XNS_TYPE_ERROR)
	goto done;
    /* 
     * This should never happen. If we source the packet then the idb
     * should be null. However don't send an error packets for errors
     * that we source.
     */
    if (GET2WD(xns->ssnet) == xsb->xnsnet &&
	ieee_equal(xns->shost, xsb->xns_node))
	goto done;
    errpak = getbuffer(XNSHEADERBYTES + sizeof(xnserrtype));
    if (!errpak)
	goto done;
    errpak_xns = (xnshdrtype *)xnsheadstart(errpak);

    errpak_xns->cksum = 0;
    errpak_xns->len = XNSHEADERBYTES + sizeof(xnserrtype);
    errpak_xns->pt = XNS_TYPE_ERROR;
    errpak_xns->tc = 0;
    PUT2WD(errpak_xns->ssnet, xsb->xnsnet);
    ieee_copy(xsb->xns_node, errpak_xns->shost);
    bcopy(xns->ssnet, errpak_xns->ddnet, XNS_ADDRBYTES);
    errtype = (xnserrtype *)XNSDATASTART(errpak);
    errtype->errnum = type;
    errtype->errparam = code;
    bcopy(xnsheadstart(pak), errtype->errpak, XNSHEADERBYTES + 12);
    xns_crc(errpak);
    if (xns_send(errpak))
	xns_stat[XNS_ERROUT]++;
done:
    if (local >= 0)
	xns_dispose(local, pak);
    else
	retbuffer(pak);
}

/*
 * xns_dispose
 * discard an XNS input packet
 */

void xns_dispose (register int code, register paktype *pak)
{
    if (code >= XNS_MAXTRAFFIC)
      errmsg(&msgsym(BADDISP, SYS), code, "xns_dispose");
    else
	xns_stat[code]++;
    retbuffer(pak);
}

/* 
 * xns_crc - checksum XNS packet
 */

void xns_crc (paktype *pak)
{
    xnshdrtype *xns;

    xns = (xnshdrtype *)xnsheadstart(pak);

    /* 
     * checksum on FULL word boundary - use appropriate pointer 
     * since char stuff sometimes clears rest WORD, eg, on 11s 
     */
    if (xns->cksum == 0xFFFF)
	return;
    /* check sum the packet */
    xns->cksum = xns_crc1(xnsheadstart(pak));
}

/*
 * xns_validcrc
 * Compute checksum - return TRUE if good, else FALSE
 */

boolean xns_validcrc (paktype *pak)
{
    xnshdrtype *xns;

    xns = (xnshdrtype *)xnsheadstart(pak);

    if (xns->cksum == 0xFFFF)
	return(TRUE);			/* Not checksummed */
    return(xns->cksum == xns_crc1(xnsheadstart(pak)));
}

/*
 * show_xns
 * Display XNS Protocol information
 */
void show_xns (parseinfo *csb)
{
    if (!xns_running) {
	printf(xns_notenabled);
	return;
    }
    switch (csb->which) {

      case SHOW_XNS_INTERFACE:
	automore_enable(NULL);
	show_swidb_chain(GETOBJ(idb,1), display_xns, 0);
	automore_disable();
	break;

      case SHOW_XNS_ROUTE:
 	show_griproutes(GETOBJ(int,1), xns_table, xns_routes, xns_maxpaths, 
	XNS_TYPE);
	break;

      case SHOW_XNS_TRAFFIC:
	show_xnstraffic();
	break;

      case SHOW_XNS_CACHE:
        show_xns_cache(XNS_TYPE);
	break;

      case SHOW_XNS_ACCESS:
        show_xnsaccess(GETOBJ(int,1));
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * display_xns
 * Display XNS interface information
 */

void display_xns (idbtype *idb)
{
    xnssb	*xsb;
    hwidbtype	*hwidb;

    if (idb == NULL)
	return;
    xsb = idb_use_swsb_inline(idb, SWIDB_SB_XNS);
    printf(if_is_line_proto_is_msg, idb->namestring,
	   print_idbstate(idb), interface_up(idb) ? "up" : "down");
    if (xsb == NULL || xsb->xnsnet == 0) {
	printf("\n  XNS protocol processing disabled");
	return;
    }
    printf("\n  XNS address is %u.%e", xsb->xnsnet, xsb->xns_node);
    hwidb = idb->hwptr;
    if (hwidb == NULL) {
	printf("\n  No hardware interface");
	idb_release_swsb_inline(idb, SWIDB_SB_XNS);
	return;
    }
    if (hwidb->status & IDB_IEEEMASK) {
	printf("\n  XNS encapsulation is %s", xsb->xns_enctype == ET_NULL ?
	       encstring(hwidb->enctype) : encstring(xsb->xns_enctype));
	if ((hwidb->status & IDB_TR) && hwidb->tr_3com_hack)
	    printf("  (old3com)");
    }
    printf("\n  Helper address is ");
    if (xsb->xns_helpernet != 0L) {
	if (xsb->xns_helpernet == -1L)
	    printf("-1.%e", xsb->xns_helperaddr);
	else
	    printf("%u.%e", xsb->xns_helpernet, xsb->xns_helperaddr);
    }
    else
	printf("not set");
    printf("\n  Outgoing access list is "); notset(xsb->xns_accesslist);
    printf("\n  Input filter list is "); notset(xsb->xns_in_filterlist);
    printf("\n  Output filter list is "); notset(xsb->xns_out_filterlist);
    printf("\n  Router filter list is "); notset(xsb->xns_router_filterlist);
    printf("\n  Update time is %d seconds",
	   xsb->xnsrip.update_interval/ONESEC);
    if (hwidb->fast_switch_flag & FSF_XNS)
	printf("\n  XNS Fast switching enabled");
    else
	printf("\n  XNS Fast switching disabled");

    idb_release_swsb_inline(idb, SWIDB_SB_XNS);
}

/*
 * xns_traffic
 * XNS statistics
 */

void show_xnstraffic (void)
{
    printf("\nRcvd:  %d total, %d format errors, %d checksum errors,"
	   " %d bad hop count,",
	   xns_stat[XNS_INPUT], xns_stat[XNS_FORMATERROR],
	   xns_stat[XNS_CKSUMERR], xns_stat[XNS_HOPCOUNT]);
    printf("\n       %d local destination, %d not a gateway, %d multicast",
	   xns_stat[XNS_LOCALDEST], xns_stat[XNS_NOTGATEWAY],
	   xns_stat[XNS_INMULT]);
    printf("\nBcast: %d received, %d sent",
	   xns_stat[XNS_BCASTIN], xns_stat[XNS_BCASTOUT]);
    printf("\n Sent: %d generated, %d forwarded",
	   xns_stat[XNS_OUTPUT], xns_stat[XNS_FORWARDED]);
    printf("\n       %d encapsulation failed, %d no route",
	   xns_stat[XNS_ENCAPSFAILED], xns_stat[XNS_NOROUTE]);
    printf("\nError: %d received, %d sent",
	   xns_stat[XNS_ERRIN], xns_stat[XNS_ERROUT]);
    printf("\n Echo: Rcvd %d requests, %d replies",
	   xns_stat[XNS_ECHOREQIN], xns_stat[XNS_ECHOREPIN]);
    printf("\n       Sent %d requests, %d replies",
	   xns_stat[XNS_ECHOREQOUT], xns_stat[XNS_ECHOREPOUT]);
    printf("\nUnknown: %d packets", xns_stat[XNS_UNKNOWN]);
}


/*
 * show_xnsaccess
 * Function to display the XNS access lists.
 */
void show_xnsaccess (int listnumber)
{
    int i;
    xnseacctype *item;
    
    for (i = MINXNSACC; i <= MAXEXNSACC; i++) {
	if (listnumber && (listnumber != i))
	    continue;
	item = xns_get_ext_acl(i);
	if (item) {
	    if (automore_quit())
		return;
	    printf("\nXNS access list %d", i);
	    while (item) {
		printf("\n    %s", item->grant ?
		       "permit" : "deny ");
		if (item->flags & XNS_PROTOCOL)
		    printf(" %d", item->protocol);
		if (item->flags & XNS_SRCNET)
		    printf(" %d", item->srcnet);
		if (item->flags & XNS_SRCHOST)
		    printf(".%e", &item->srchost);
		if (item->flags & XNS_SRCNETMASK) {
		  printf(" %d", item->srcnetmask);
		  if (item->flags & XNS_SRCHOSTMASK)
		    printf(".%e", &item->srchostmask);
		} else {
		  if (item->flags & XNS_SRCHOSTMASK)
		    printf(" %e", &item->srchostmask);
		}
		if (item->flags & XNS_SRCSOCKET)
		    printf(" 0x%-04x", item->srcsocket);
		if (item->flags & XNS_DSTNET)
		    printf(" %d", item->dstnet);
		if (item->flags & XNS_DSTHOST)
		    printf(".%e", &item->dsthost);
		if (item->flags & XNS_DSTNETMASK) {
		  printf(" %d", item->dstnetmask);
		  if (item->flags & XNS_DSTHOSTMASK)
		    printf(".%e", &item->dsthostmask);
		} else {
		  if (item->flags & XNS_DSTHOSTMASK)
		    printf(" %e", &item->dsthostmask);
		}
		if (item->flags & XNS_DSTSOCKET)
		    printf(" 0x%-04x", item->dstsocket);
		if (item->flags & XNS_PEPCLIENT)
		    printf(" %d", item->pep_clienttype);
		item = item->next;
	    }	    
	}
    }
}


/*
 * xns_forwarding
 * see if we should forward a packet to a helper address, or flood it, or
 * ignore it.
 */
boolean xns_forwarding (paktype *pak)
{
    xnshdrtype	*xns;
    idbtype	*idb;
    xnssb	*xsb;
    ulong	dnet;
    int		i;

    xns = (xnshdrtype *)xnsheadstart(pak);

    idb = pak->if_input;
    if (idb == NULL)
	return (FALSE);
    xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
    if (xsb == NULL)
	return (FALSE);
    dnet = GET2WD(xns->ddnet);
    if (GET2WD(xns->ssnet) == 0)
	PUT2WD(xns->ssnet, xsb->xnsnet);

    /* First we check to see if the packet is a helpering candidate. It
     * will be helpered if:
     *
     *      Its network-layer destination host address is the broadcast address
     *
     *      We have a helper address to send it to
     *
     *      Its protocol type is one we've been configured to helper. All-nets
     *      broadcasts aren't subject to this check.
     *
     * Note that XNS uses the PAK_BROADCAST flag for the network layer, rather
     * than the link layer...
     */
    if ((pak->flags & PAK_BROADCAST) && xsb->xns_helpernet)
	for (i = 0; i < xns_forwcnt; i++) 
	    if ((xns->pt == xns_forw_proto[i]) || (dnet == XNS_BROADCAST_NET)){
		if (xnspkt_debug)
		    buginf("\nXNS: src=%u.%e, dst=%u.%e, helper=%u.%e",
			   GET2WD(xns->ssnet), xns->shost, dnet,
			   xns->dhost, xsb->xns_helpernet,xsb->xns_helperaddr);
		PUT2WD(xns->ddnet, xsb->xns_helpernet);
		ieee_copy(xsb->xns_helperaddr, xns->dhost);
		xns_crc(pak);
		xns_forward(pak);
		return (TRUE);
	    }

    /*
     * Check to see if this packet matches one of the types we flood.
     * If it does, flood it out all the interfaces.
     */
    if (((pak->flags & PAK_BROADCAST) && (dnet == 0L) &&
	 xsb->xns_fld_bcast_netzero) ||
	((pak->flags & PAK_BROADCAST) && (dnet == XNS_BROADCAST_NET) &&
	 xsb->xns_fld_bcast_allnets) ||
	(!(pak->flags & PAK_BROADCAST) && (dnet == XNS_BROADCAST_NET) &&
	 xsb->xns_fld_spec_allnets))
	return (xns_flood(pak));

    /* Wasn't a candidate for helpering or flooding... */
    return (FALSE);
}

/*
 * xns_flood
 *
 * Flood a XNS broadcast datagram out all XNS-enabled interfaces.
 * This is done with the usual "duplicate and loop" method.
 */
boolean xns_flood (paktype *pak)
{
    xnshdrtype	*xns;
    paktype	*newpak;
    idbtype	*idb;
    xnssb	*xsb;
    xns_ripentrytype *p;
    ushort	hops;
    list_element *l_elt;

    xns = (xnshdrtype *)xnsheadstart(pak);

    /* Look up the path list back to the source. If there's no path, take
     * the conservative tack and don't flood the thing.
     */
    if (!(p = grip_find_rt(GET2WD(xns->ssnet), xns_table)))
	return (FALSE);

    /* We no longer use xns_forward for flooding, because, to put
     * it simply, it didn't work. We can't let the target net get
     * looked up in the routing table, because it would overwrite our
     * setting of if_output. Unfortunately, this means that much of the
     * functionality of both xns_forward and xns_send gets sucked into
     * this routine. Sigh.
     */
    hops = (xns->tc & XNS_TC_HOP_MASK) + 1; /* Increment only hop count bits */
    if (hops >= XNS_INFINITY) {
	if (xnspkt_debug)
	    buginf("\nXNS: src=%u.%e, dst=%u.%e, hop count exceeded on flood",
		   GET2WD(xns->ssnet), xns->shost, GET2WD(xns->ddnet),
		   xns->dhost);
	xns_error(pak, XNS_ERR_2OLD, 0, XNS_HOPCOUNT);
	return (TRUE);
    }
    /* Update hops field within tc, preserving reserved bits */
    xns->tc = (hops |= (xns->tc & XNS_TC_RSV_MASK));

    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_XNS, l_elt, idb) {
	xsb = idb_get_swsb_inline(idb, SWIDB_SB_XNS);
	/* Don't flood out the source interface, or out any path that
	 * leads back to the of the datagram.
	 */
	if (   interface_up(idb) && xsb && xsb->xnsnet
	    && (idb != pak->if_input) && !grip_same_idb(idb, p->path_list)) {

	    /* If access lists would prevent us from sending the packet
	     * out this interface, there's no reason to go through
	     * all the effort of duplicating or rechecksumming it.
	     * Just go on to the next IDB.
	     */
	    /* Note: use idb since pak->if_output is not valid at this point.*/
	    if (xsb->xns_accesslist &&
		!xns_accesscheck(pak, xsb->xns_accesslist))
		continue;

	    /* Don't do pak_duplicate on the last packet */
	    if (IDB_NEXT(idb)) {
		newpak = pak_duplicate(pak);
		if (newpak == NULL)
		    return (FALSE);
	    } else {
		newpak = pak;
		pak = NULL;
	    }
	    newpak->if_output = idb;
	    if (xnspkt_debug)
		buginf("\nXNS: flooding out %s from %u.%e(%u)",
		       idb->namestring, 
		       GET2WD(xns->ssnet), xns->shost, xns->ssock);
	    xns_crc(newpak);
	    /* We always send to the broadcast address when flooding */
	    (void) xns_send1(newpak, baddr);
	}
    }
    if (pak != NULL)
	retbuffer(pak);
    return (TRUE);
}


/*
 * xns_accesscheck
 * Apply a XNS access list to a packet.
 */
boolean xns_accesscheck (paktype *pak, int accesslist)
{
    xnseacctype	*item;
    xnshdrtype	*xns;
    xnspephdrtype *pep;
    ulong	flags;
    mac_address_type host;
    
    if (!accesslist)
	return (TRUE);
    item = xns_get_ext_acl(accesslist);
    if (item == NULL) 
	return (TRUE);

    xns = (xnshdrtype *)xnsheadstart(pak);

    do {
	flags = item->flags;
	if (flags & XNS_PROTOCOL) {
	    if (item->protocol != xns->pt) {
		continue;
	    } else {
		if (flags & XNS_PEPCLIENT) {
		    if (xns->pt == XNS_TYPE_PEP) { 
			pep = (xnspephdrtype *)xnsheadstart(pak);
			if (pep->pep_client_type != item->pep_clienttype)
			  continue;
		    }
		}
	    }
	}

	/*
	 * A network of -1 indicates all XNS networks.
	 * A socket of 0 indicates all XNS sockets.
	 */
	if (flags & XNS_SRCNET) {
	   if ((item->srcnet != XNS_BROADCAST_NET) && 
	       (item->srcnet != (GET2WD(xns->ssnet) & ~item->srcnetmask)))
		continue;
	}
	if (flags & XNS_SRCHOST) {
	    /*
	     * Check for match on source host address
	     */
	    ieee_copy(xns->shost, (uchar *)&host);
	    if ((host.u.lword[0] & ~item->srchostmask.u.lword[0])
		!= (item->srchost.u.lword[0]))
		continue;
	    if ((host.u.sword[2] & ~item->srchostmask.u.sword[2])
		!= item->srchost.u.sword[2])
		continue;
	}
	if (flags & XNS_SRCSOCKET) {
	    if (item->srcsocket && (item->srcsocket != xns->ssock))
		continue;
	}
	if (flags & XNS_DSTNET) {
	    if ((item->dstnet != -1L) && 
		(item->dstnet != (GET2WD(xns->ddnet) & ~item->dstnetmask)))
		continue;
	}
	if (flags & XNS_DSTHOST) {
	    /*
	     * Check for match on destination host address
	     */
	    ieee_copy(xns->dhost, (uchar *)&host);
	    if ((host.u.lword[0] & ~item->dsthostmask.u.lword[0])
		!= (item->dsthost.u.lword[0]))
		continue;
	    if ((host.u.sword[2] & ~item->dsthostmask.u.sword[2]) 
		!= item->dsthost.u.sword[2])
		continue;
	}
	if (flags & XNS_DSTSOCKET) {
	    if (item->dstsocket && (item->dstsocket != xns->dsock))
		continue;
	}
	return(item->grant);
    } while ((item = item->next));
    return (FALSE);
}

/*
 * xns_filtercheck
 * Check if the specified network number is permitted.
 */
boolean xns_filtercheck (ulong net, int accesslist)
{
    xnseacctype *item;
    ulong	flags;

    if (!accesslist)
	return (TRUE);
    item = xns_get_ext_acl(accesslist);
    if (item == NULL) 
	return (TRUE);
    do {
	flags = item->flags;
	if (flags & XNS_SRCNET)
	    if ((item->srcnet != -1L) &&
		(item->srcnet != (net & ~item->srcnetmask)))
		continue;
	return (item->grant);
    } while ((item = item->next));
    return (FALSE);
}
