/* $Id: ipcp.c,v 3.24.6.27 1996/09/12 13:31:59 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/ipcp.c,v $
 *------------------------------------------------------------------
 * ipcp.c - PPP IP Control Protocol.
 *
 * 02-January-1990, Vicki Ralls
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipcp.c,v $
 * Revision 3.24.6.27  1996/09/12  13:31:59  fox
 * CSCdi68759:  AAA per-user routes are not removed when connection drops
 * Branch: California_branch
 * Separate parallel connection support for per-user routes from the
 * support for neighbor routes and dynamic dialer maps.  Transfer
 * ownership of the static route to the last idb to add it (applies only
 * to per-user routes).
 *
 * Revision 3.24.6.26  1996/09/10  06:16:36  fox
 * CSCdi68456:  Parallel connection information lost if one link
 * disconnects
 * Branch: California_branch
 * Expand check for parallel connections to protect IP dynamic dialer
 * maps.
 *
 * Revision 3.24.6.25  1996/09/09  22:35:11  fox
 * CSCdi68441:  IP Address Pooling blocks use of dialer maps
 * Branch: California_branch
 * Do not require the peer to request its address by sending 0.0.0.0.
 * Support Wellfleet/DialerMap fix on deprecated IPCP Option 1.
 *
 * Revision 3.24.6.24  1996/09/06  13:28:12  fox
 * CSCdi50490:  neighbor route never installed
 * Branch: California_branch
 * Simplify neighbor route creation.  Try to add the route when IPCP or
 * SLIP are ready.  Also try to add the route if the interface comes up
 * while you know about the neighbor.
 *
 * Revision 3.24.6.23  1996/09/04  14:03:52  fox
 * CSCdi67844:  All parallel neighbor routes cleared when connection drops
 * Branch: California_branch
 * Don't remove neighbor route if shared by another hunt group member.
 *
 * Revision 3.24.6.22  1996/08/21  06:08:56  fox
 * CSCdi66613:  IPCP accepts but ignores NAK of IP address
 * Branch: California_branch
 * Never, ever, ever forget to take out test hacks.  :-(
 *
 * Revision 3.24.6.21  1996/08/18  09:07:45  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.24.6.20  1996/08/15  17:05:24  fox
 * CSCdi66132:  Neighbor routes are not created on dialer interfaces
 * Branch: California_branch
 * ip_peer (ippeerinfotype) doesn't need to exist on the dialer interface
 *
 * Revision 3.24.6.19  1996/08/15  05:17:09  fox
 * CSCdi65101:  Neighbor routes not removed when connection ends
 * Branch: California_branch
 * Clear the neighbor route before the interface is unbound from
 * the dialer interface since the neighbor routes will point ot the
 * dialer interface.
 *
 * Revision 3.24.6.18  1996/07/30  04:22:41  che
 * CSCdi62108:  authentication pitfall with XTACACS/PAP
 * Branch: California_branch
 * Check access lists returned from xtacacs.  Check return value of
 * reg_invoke_login_slipon() in ipcp_up().
 *
 * Revision 3.24.6.17  1996/07/11  19:24:13  sjackson
 * CSCdi60440:  Unable to Allocate IP Address From Local (Non-Default) Pool
 * Branch: California_branch
 * Configuration IDB containing pool information determined from physical
 * interface when required
 *
 * Revision 3.24.6.16  1996/07/03  01:38:37  albright
 * CSCdi59425:  Incorrect entry of Major Net Summary RIP/IGRP w/ ip
 * unnumbered PPP
 * Branch: California_branch
 *
 * Revision 3.24.6.15  1996/06/24  09:27:15  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.24.6.14  1996/06/21  15:31:52  sbelair
 * CSCdi58001:  debug output garbled for AV pair
 * Branch: California_branch
 *
 * Last checkin was wrong- the original problem was that the namestring was
 * reversed with the list number.
 *
 * Revision 3.24.6.13  1996/06/21  15:05:29  sbelair
 * CSCdi58001:  debug output garbled for AV pair
 * Branch: California_branch
 *
 * Remove extra (unnecessary) parameter passed to buginf.
 *
 * Revision 3.24.6.12  1996/06/19  20:38:12  fox
 * CSCdi59847:  Dynamic dialer maps are not cleared on async dialer ints
 * Branch: California_branch
 * Call reg_invoke_ipcp_cleanup_ip_peer() to remove the neighbor route,
 * the dynamic dialer map, and any other mechanisms tied to the current
 * peer address before clearing the peer address.
 *
 * Revision 3.24.6.11  1996/06/01  01:08:55  carrel
 * CSCdi51262:  xslipoff not sent when the line is renegociated for ppp
 * Branch: California_branch
 * Remove bogus check for TACACS authentication in ipcp_down()
 *
 * Revision 3.24.6.10  1996/05/28  16:20:57  vandys
 * CSCdi50153:  systat does not display ISDN access users
 * Branch: California_branch
 *
 * Revision 3.24.6.9  1996/05/21  17:26:30  syiu
 * CSCdi57970:  Installed Route remains in Dialer interface after
 * disconnected
 * Branch: California_branch
 *
 * Revision 3.24.6.8  1996/05/21  10:09:36  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.24.6.7  1996/05/10  01:44:51  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.24.6.6  1996/05/04  02:17:08  wilber
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
 * Revision 3.24.6.5  1996/04/29  09:45:28  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.24.6.4  1996/04/27  06:36:45  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.11.8.4  1996/04/25  23:58:50  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.11.8.3  1996/04/17  00:01:10  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.11.8.2  1996/02/23  21:31:37  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.11.8.1  1996/02/04  06:29:36  syiu
 * Branch: Dial1_branch
 * Support for MLP Bundle interfaces
 *
 * Revision 3.24.6.3  1996/03/27  09:24:54  billw
 * CSCdi51571:  bus error in AAA Accounting
 * Branch: California_branch
 *
 * Revision 3.24.6.2  1996/03/27  08:34:45  billw
 * CSCdi49184:  AAA accounting records do not contain IP addr if
 * autoselected
 * Branch: California_branch
 *
 * Revision 3.24.6.1  1996/03/18  22:47:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.15.2.4  1996/03/16  07:55:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.15.2.3  1996/03/13  02:12:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.15.2.2  1996/03/07  11:07:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.15.2.1  1996/02/20  21:51:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.24  1996/03/05  07:07:40  fox
 * CSCdi50489:  unnecessary neighbor route installed, backup line never
 * drops
 * Add '[no] peer neighbor-route' command
 *
 * Revision 3.23  1996/02/21  03:57:57  fox
 * CSCdi49007:  IPCP down removes all parallel neighbor routes
 * Give the neighbor route to another interface with the same
 * peer address
 *
 * Revision 3.22  1996/02/14  19:36:54  asb
 * CSCdi48113:  Should deliver NetBios NS [implement rest of RFC1877]
 *
 * Revision 3.21  1996/02/09  09:03:22  billw
 * CSCdi43680:  RADIUS doesnt support magic number ip address values
 * also add authstruct->auth_idb, and change auth->tty to auth->auth_tty
 *
 * Revision 3.20  1996/02/04  18:04:49  fox
 * CSCdi48136:  Neighbor route not installed unless IP addr is negotiated
 * Remove 'negotiated' check since some clients don't negotiate :-(
 * Process AAA/TACACS+ responses even if no address negotiated
 *
 * Revision 3.19  1996/01/26  17:25:22  asb
 * CSCdi46322:  Should return async-bootp dns-server instead of routers
 * DNS server
 *
 * Revision 3.18  1996/01/26  09:26:56  fox
 * CSCdi47481:  Can still pass IP packets after authen before author
 * Clean up comment per code review
 *
 * Revision 3.17  1996/01/25  06:28:48  fox
 * CSCdi47481:  Can still pass IP packets after authen before author
 * Don't convert between internal and external types until ACLs are
 * loaded (if necessary).
 *
 * Revision 3.16  1996/01/23  18:22:41  fox
 * CSCdi47345:  Unused functions are unneeded functions
 * ncp_closed is never referenced
 * seen_addr and seen_pool are redundant
 *
 * Revision 3.15  1996/01/22  07:45:46  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.14  1996/01/16  16:38:15  fox
 * CSCdi46355:  Can pass IP packets after authen before author
 * Don't mark port up until TACACS+ gets a chance to fix the ACL.
 *
 * Revision 3.13  1996/01/16  00:41:21  irfan
 * CSCdi46312:  Routing Protocol broadcast not sent to backup Async/PPP
 * interfaces
 * Leave Async interfaces as routing interfaces until the time they
 * are being brought up. At which time, we may mark them as
 * static_routing. Allows DDR to kick up the interface to send a
 * routing protocol update on an UP/UP Spoofing interface in async
 * backup configurations
 *
 * Revision 3.12  1996/01/15  19:24:23  misko
 * CSCdi43044:  ip address negotiation not working with DDR
 * When we assign an IP address, allow a dialer map to be dynamically
 * created so that we can map the assigned IP address to an authenticated
 * name.
 *
 * Revision 3.11  1996/01/04  22:15:17  lol
 * CSCdi43469:  per-user route wont use default interface
 *
 * Revision 3.10  1995/12/19  21:40:40  fox
 * CSCdi45830:  Neighbor route remains after interface is cleared
 * Use the correct variable when clearing the route.
 *
 * Revision 3.9  1995/12/18  23:49:32  lol
 * CSCdi43005:  route attribute is not processed for sync ISDN
 *
 * Revision 3.8  1995/12/13  22:58:08  lol
 * CSCdi39791:  access-lists applied by tacacs+ dont work when
 * fast-switched
 *
 * Revision 3.7  1995/11/29  00:16:31  lol
 * CSCdi42385:  TACACS+ crashes router in aaa_split_AV
 *
 * Revision 3.6  1995/11/26  22:13:00  fox
 * CSCdi44404:  Unacceptable slot # generates bogus Reject instead of NAK
 * Move to end of option if exiting before examining all values.
 *
 * Revision 3.5  1995/11/19  06:09:10  fox
 * CSCdi43820:  routes thrash on ipcp_up()
 * Don't remove and readd neighbor route if it is already in place.
 * Return indication from route_add_neighbor() as whether routing tables
 * were modified in order to avoid unnecessary route adjustment.
 * Remove route_remove_neighbor() as it did the same thing as
 * iprouting_delete_conn_route().
 *
 * Revision 3.4  1995/11/18  17:44:36  fox
 * CSCdi43045:  ppp and slip exec-level commands are broken
 * Reject the peer's IP address option if we have no addresses to
 * hand out.
 * Don't second guess a peer address entered with a 'ppp' command.
 *
 * Revision 3.3  1995/11/17  18:02:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:49:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.16  1995/11/08  21:35:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.15  1995/10/23  21:02:19  fox
 * CSCdi42602:  Neighbor routes are lost if routing table is cleared
 * Move neighbor address support to the WAN code.
 * Call the WAN code if an interface's routes are being modified.
 *
 * Revision 2.14  1995/10/21  22:02:23  fox
 * CSCdi42464:  PPP user is not given same IP address on next connect
 * Pass user-name from authentication to IP address pool function
 *
 * Revision 2.13  1995/10/20  20:24:54  lol
 * CSCdi42253:  callback causes IPCP authorization failures
 *              Only return callback info when doing local authorization of
 *              ppp/lcp, slip, arap or exec
 *
 * Revision 2.12  1995/10/16  21:22:36  fox
 * CSCdi42205:  Microsoft IPCP Extension options are rejected
 * Add support for primary and secondary DNS options.
 *
 * Revision 2.11  1995/10/11  06:03:26  fox
 * CSCdi40958:  PPP address negotiation doesnt work with dialer interfaces
 * Paste neighbor routes to the individual interfaces, let routing code
 *  point the route at the dialer interface, if apprpriate
 * Remove neighbor route before we possibly exit early in ipcp_down()
 *
 * Revision 2.10  1995/10/02  20:43:10  lol
 * CSCdi36244:  Console line failing authorization should be more forgiving
 *              Authenticated console users now bypass authorization.
 *
 * Revision 2.9  1995/09/18  19:39:46  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.8  1995/09/16  21:44:49  carrel
 * CSCdi40334:  Bus error crash in AAA accounting code
 * Correctly set ho->address when AAA chooses the addr
 * If pooling fails to set a non-mandatory pool, fall thru to addr checks
 * Don't put ciaddr1 in cached authors if all attempts to derive an addr
 * fail.
 * Remove an arg to aaa_dup_ifneeded().
 * Make aaa accounting calls ONLY get called for aaa new-model.
 *
 * Revision 2.7  1995/09/07  09:36:39  irfan
 * CSCdi39879:  memory leaking as AAA AV pairs from PAP/CHAP threads
 * Plug the holes by remembering to aaa_cleanup_author() for both return
 * cases of lcp_do_author().
 *
 * Revision 2.6  1995/08/31  19:48:57  lol
 * CSCdi38919:  tacacs+ ip pooling not working
 *
 * Revision 2.5  1995/08/17  02:07:56  lol
 * CSCdi37366:  Async with TACACS+ and CHAP gives wrong IP address in
 * routing table
 *
 * Revision 2.4  1995/08/08  20:50:47  lol
 * CSCdi37361:  AAA should not require auth for originating PPP connections
 *
 * Revision 2.3  1995/08/07  21:52:38  lol
 * CSCdi36743:  async PPP fails to connect with /routing and tacacs+
 * authorization
 *
 * Revision 2.2  1995/06/15  03:42:44  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 * Revision 2.1  1995/06/07  23:18:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ppp.c - Point-to-Point Protocol.
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <stdlib.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "media_registry.h"
#include "dialer_registry.h"
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "config.h"
#include "aaa.h"
#include "access.h"
#include "ttysrv.h"
#include "../ts/if_slip.h"
#include "../iprouting/route.h"
#include "ppp.h"
#include "ipcp.h"
#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaddress.h"
#include "../tcp/tcpcompress.h"
#include "ppp_public.h"
#include "ppp_registry.h"
#include "../ip/ip_registry.h"
#include "parser.h"
#include "../ip/ip_addrpool.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../parser/parser_defs_autoselect.h"
#include "../ip/ip_debug.h"
#include "name.h"
#include "dialer.h"
#include "dialer_private.h"
#include "../if/network.h"
#include "../parser/parser_actions.h"
#include "../ip/ip_actions.h"
#include "../h/cs_registry.regh"
#include "../wan/mlp.h"

static void ipcp_resetci(fsm *);	/* Reset our Configuration Information */
static int ipcp_cilen(fsm *);		/* Return length of our CI */
static void ipcp_addci(fsm *, uchar *);	/* Add our CIs */
static int ipcp_ackci(fsm *, uchar *, int);	/* Ack some CIs */
static void ipcp_nakci(fsm *, uchar *, int);	/* Nak some CIs */
static void ipcp_rejci(fsm *, uchar *, int);	/* Reject some CIs */
static uchar ipcp_reqci(fsm *, uchar *, short *);	/* Check the requested CIs */
static void ipcp_up(fsm *);	/* IPCP came up */
static void ipcp_down(fsm *);	/* IPCP is not up */
static void ipcp_closed(fsm *);	/* Link went down */

static boolean ipcp_process_author_params(hwidbtype *, char **);
static ipaddrtype ipcp_allocate_addr(fsm *, idbtype *);
static void validate_peer_ip_addr(ipaddrtype *, ipcp_options *, idbtype *);
static boolean ipcp_do_author(hwidbtype *, char ***, ipaddrtype);
static boolean ipcp_authorize_addr(fsm *, ulong, ipcp_options *, boolean *);
static void ipcp_author_set_addr(char ***, ipaddrtype);
static void ipcp_author_get_addr(char **, ipaddrtype *, enum AV_TYPE *);
static void ipcp_author_get_pool(char **, uchar **, enum AV_TYPE *);
static void ipcp_remove_p2p_neighbor(idbtype *, ipaddrtype);
static boolean store_per_user_ip_route(hwidbtype *, enum AV_TYPE, char *);

static fsm_callbacks ipcp_callbacks = { /* IPCP callback routines */
    ipcp_resetci,	/* Reset our Configuration Information */
    ipcp_cilen,		/* Length of our Configuration Information */
    ipcp_addci,		/* Add our Configuration Information */
    ipcp_ackci,		/* ACK our Configuration Information */
    ipcp_nakci,		/* NAK our Configuration Information */
    ipcp_rejci,		/* Reject our Configuration Information */
    ipcp_reqci,		/* Request peer's Configuration Information */
    ipcp_up,		/* Called when fsm reaches OPEN state */
    ipcp_down,		/* Called when fsm leaves OPEN state */
    ipcp_closed,	/* Called when fsm reaches CLOSED state */
    NULL,		/* Called when Protocol-Reject received */
    NULL,		/* Retransmission is necessary */
    NULL,		/* Reset Request */
    NULL,		/* Rest Acknowledge */
	NULL		/* Code Rejected */
};

/*
 * ipcp_init - Initialize IPCP.
 */
void ipcp_init (fsm *ipcp_fsm)
{
    ipcp_options *wo;
    ipcp_options *ao;

    if (!ipcp_fsm->wantoptions)
    	ipcp_fsm->wantoptions = malloc_named(sizeof(ipcp_options),
					     "PPP IPCP 1");
    else
	memset(ipcp_fsm->wantoptions, 0, sizeof(ipcp_options));
    if (!ipcp_fsm->wantoptions)
	return;
    if (!ipcp_fsm->allowoptions)
    	ipcp_fsm->allowoptions = malloc_named(sizeof(ipcp_options),
					      "PPP IPCP 2");
    else
	memset(ipcp_fsm->allowoptions, 0, sizeof(ipcp_options));
    if (!ipcp_fsm->allowoptions)
	return;
    if (!ipcp_fsm->heroptions)
    	ipcp_fsm->heroptions = malloc_named(sizeof(ipcp_options),
					    "PPP IPCP 3");
    else
	memset(ipcp_fsm->heroptions, 0, sizeof(ipcp_options));
    if (!ipcp_fsm->heroptions)
	return;
    ipcp_fsm->ouroptions = NULL;

    wo = ipcp_fsm->wantoptions;
    ao = ipcp_fsm->allowoptions;

    ipcp_fsm->retransmits = 0;
    TIMER_STOP(ipcp_fsm->timeouttime);
    ipcp_fsm->failures = 0;
    ipcp_fsm->callbacks = &ipcp_callbacks;
    ipcp_fsm->flags |= AOPENDING;           /* Active Open */

}


/*
 * ipcp_resetci - Reset our CI.
 */

static void ipcp_resetci (fsm *f)
{
    ipcp_options *wo;
    ipcp_options *ao;
    ipcp_options *ho;
    hwidbtype *idb;

    wo = f->wantoptions;
    ao = f->allowoptions;
    ho = f->heroptions;

    idb = f->idb;

    if (idb->tcp_compress) { /* VJ compression allowed? */
	/*
	 * Currently only if multilink has not been negotiated
	 */
	if (is_multilink(idb)) {
	    if (ppp_negotiation_debug) {
		buginf("\nPPP %s: multilink and TCP/IP header compression are "
		    "incompatible", idb->hw_namestring);
	    }
	    wo->neg_vj = 0;
	    ao->neg_vj = 0;
	} else {
	    wo->neg_vj = ao->neg_vj = 1; /* Try to negotiate VJ compression */
	    wo->vj_slots = ao->vj_slots = (idb->tcp_compress->statecount - 1);
	    wo->vj_csid = ao->vj_csid = 0; /* Don't compress session ID */
	    thc_compress_reset(idb->tcp_compress, 0, 0); /* Restore state */
	    idb->tcp_compress->flags &= ~THC_ACTIVE; /* Ensure not active */
	}
    } else {
	wo->neg_vj = 0;
	ao->neg_vj = 0;
    }

    wo->neg_addrs = 0;		/* Don't send obsolete address negotiation */
    wo->neg_address = 1;	/* Negotiate local address */
    wo->address = ip_ouraddr(idb);

    ao->neg_addrs = 1;		/* Allow peer to send obsolete addr. neg. */
    ao->neg_address = 1;	/* Allow remote to negotiate an address */
    /* Set initial remote address */
    ho->address = reg_invoke_ip_peer_addr(idb->firstsw);

    f->gotoptions = f->wantoptions;
}


/*
 * ipcp_cilen - Return length of our CI.
 */
static int ipcp_cilen (fsm *f)
{
    ipcp_options *go;

    go = f->gotoptions;

    return (LENCIADDRS(go->neg_addrs) +
	    LENCIVJHDR(go->neg_vj) +
	    LENCIADDRESS(go->neg_address));
}


/*
 * ipcp_addci - Add our desired CIs to a packet.
 */
static void ipcp_addci (fsm *f, uchar *ucp)
{
    ipcp_options *go;
    ipcp_options *ho;

    go = f->gotoptions;
    ho = f->heroptions;

    ADDCIADDRS(CI_ADDRS, go->neg_addrs, go->address, ho->address);
    ADDCIVJCOMP(CI_COMPRESSTYPE, go->neg_vj, go->vj_slots, go->vj_csid);
    ADDCIADDRESS(CI_ADDRESS, go->neg_address, go->address);

    /*
     * The Microsoft IPCP Extensions are never added to an outgoing
     * configure request.  They are implemented in the Win95 and WinNT
     * products and are only of value (you would think) to a dial-up
     * client.
     */
}


/*
 * ipcp_ackci - Ack our CIs.
 *
 * Returns:
 *	0 - Ack was bad.
 *	1 - Ack was good.
 *
 * This routine ensures that a received ACK message is valid. An ACK
 * message must contain all of the options that we specified, in the order
 * that we specified.
 */

static int ipcp_ackci (fsm *f, uchar *p, int len)
{
    ushort cilen, citype;
    ulong cilong;
    ipcp_options *go;
    ipcp_options *ho;

    go = f->gotoptions;
    ho = f->heroptions;

    /*
     * CIs must be in exactly the same order that we sent...
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */
    ACKCIADDRS(CI_ADDRS, go->neg_addrs, go->address, ho->address);
    ACKCIVJCOMP(CI_COMPRESSTYPE, go->neg_vj, go->vj_slots, go->vj_csid);
    ACKCIADDRESS(CI_ADDRESS, go->neg_address, go->address);

    /*
     * The Microsoft IPCP Extensions were never added to an outgoing
     * configure request.  They are implemented in the Win95 and WinNT
     * products and are only of value (you would think) to a dial-up
     * client.
     */

    /*
     * If there are any remaining CIs, then this packet is bad.
     */
    if (len != 0)
	goto bad;
    return (1);
bad:
    if (ppp_error_debug)
	buginf("\nppp: ipcp_ackci: received bad Ack!");
    return (0);
}


/*
 * ipcp_nakci - NAK some of our CIs.
 *
 * Returns:
 *	0 - Nak was bad.
 *	1 - Nak was good.
 *
 * This routine handles a received NAK message. The NAK message will
 * list all of the options that we specified, the peer supports, but
 * the peer would prefer a different value.
 */

static void ipcp_nakci (fsm *f, uchar *p, int len)
{
    ulong ciaddr1, ciaddr2;
    ipcp_options *go;
    ipcp_options *ho;

    go = f->gotoptions;
    ho = f->heroptions;

    /*
     * Any Nak'd CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */

    NAKCIADDRS(CI_ADDRS, go->neg_addrs,
	       if (!go->address)	/* Didn't know our address? */
		   go->address = ciaddr1;
	       else if (go->address != ciaddr1) {

		   /*
		    * At this point, we could check to see if this is an
		    * acceptable local address.
		    */

		   if (ppp_negotiation_debug)
		     buginf("\nppp: %s IPCP NAK of local address, peer wants %i",
			    f->idb->hw_namestring,
			    ciaddr1);
		   go->neg_addrs = 0; /* Don't try again */
	       }

	       if (!ho->address)	/* Didn't know remote address? */
	         ho->address = ciaddr2;
	       else if (ho->address != ciaddr2) {

		   /*
		    * At this point, we can check to see if this is an
		    * acceptable remote address.
		    */

		   if (ppp_negotiation_debug)
		     buginf("\nppp: %s IPCP NAK of remote address, "
			    "peer wants %i",
			    f->idb->hw_namestring,
			    ciaddr2);
		   go->neg_addrs = 0;
	       }
	       );

    NAKCIVJCOMP(CI_COMPRESSTYPE, go->neg_vj, 

		/*
		 * If the remote peer sent us a NAK, and specified
		 * more slots than we offered, or specified to use
		 * compression when we didn't, then our header compression
		 * is not compatible, so we should not negotiate it at all.
		 */

		if ((slots > go->vj_slots) || (csid && !go->vj_csid)) {
		    if (ppp_negotiation_debug) {
			buginf("\nppp: %s IPCP VJ Slots = %d/%d, "
			       "CSID = %d/%d",
			       go->vj_slots, slots, go->vj_csid, csid);
		    }
		    go->neg_vj = 0; 
		} else {
		    go->vj_slots = slots; /* Remember slots negotiated */
		    go->vj_csid = csid;	/* And compression state */
		}
		)

    NAKCIADDRESS(CI_ADDRESS, go->neg_address,
	       if (!go->address)	/* Didn't know our address? */
		   go->address = ciaddr1;
	       else if (go->address != ciaddr1) {

		   /*
		    * At this point, we could check to see if the address
		    * provided in the NAK is acceptable for us to use as
		    * a source address for this link, and attempt to
		    * re-negotiate with that address.
		    *
		    * However, if another source address is not acceptable,
		    * we should not attempt to negotiate an address at all.
		    */

		   if (ppp_negotiation_debug)
		     buginf("\nppp: %s IPCP NAK of address, peer wants %i",
			    f->idb->hw_namestring,
			    ciaddr1);
		   go->neg_address = 0;
	       }	       
		 )

    /*
     * The Microsoft IPCP Extensions were never added to an outgoing
     * configure request.  They are implemented in the Win95 and WinNT
     * products and are only of value (you would think) to a dial-up
     * client.  Thus we will ignore any proffered hints (but display
     * them for debugging purposes).
     */

    NAKCISERVER(CI_PRIMARYDNS, TRUE,)
    NAKCISERVER(CI_PRIMARYNBNS, TRUE,)
    NAKCISERVER(CI_SECONDARYDNS, TRUE,)
    NAKCISERVER(CI_SECONDARYNBNS, TRUE,)

    /*
     * If there are any remaining CIs, then this packet is bad.
     * At this point, we could check for any appended options.
     */
    if (len == 0)
	return;

    if (ppp_error_debug)
	buginf("\nppp: ipcp_nakci: ignoring unrequested options!");
}


/*
 * ipcp_rejci - Reject some of our CIs.
 */
static void ipcp_rejci (fsm *f, uchar *p, int len)
{
    ulong cilong;
    ipcp_options *go;
    ipcp_options *ho;

    go = f->gotoptions;
    ho = f->heroptions;

    /*
     * Any Rejected CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */

    REJCIADDRS(CI_ADDRS, go->neg_addrs, go->address, ho->address);

    REJCIVJCOMP(CI_COMPRESSTYPE, go->neg_vj, go->vj_slots, go->vj_csid);

    REJCIADDRESS(CI_ADDRESS, go->neg_address, go->address);

    /*
     * The Microsoft IPCP Extensions were never added to an outgoing
     * configure request.  They are implemented in the Win95 and WinNT
     * products and are only of value (you would think) to a dial-up
     * client.
     */

    /*
     * If there are any remaining CIs, then this packet is bad.
     */

    if (len == 0)
	return;
bad:
    if (ppp_error_debug)
	buginf("\nppp: ipcp_rejci: received bad Reject!");
}


/*
 * ipcp_reqci - Check the peer's requested CIs and send appropriate response.
 *
 * Returns: CONFACK, CONFNAK or CONFREJ and input packet modified
 * appropriately.
 */
static uchar ipcp_reqci (fsm *f, uchar *inp, short *len)
{
    ipcp_options *wo;
    ipcp_options *ho;
    ipcp_options *ao;
    ipcp_options *go;
    uchar *cip;		/* Pointer to Current CI */
    ushort cilen, citype;	/* Parsed len, type */
    ushort cishort;		/* Parsed short value */
    uchar cichar;		/* Parsed char value */
    ulong tl, ciaddr1, ciaddr2;	/* Parsed address values */
    int rc = CONFACK;		/* Final packet return code */
    int orc;			/* Individual option return code */
    uchar *p = inp;		/* Pointer to next char to parse */
    uchar *ucp = inp;		/* Pointer to current output char */
    int l = *len;		/* Length left */
    addrtype *addr;		/* Remote IP address */
    idbtype *swidb = firstsw_or_null(f->idb);
    ippeerinfotype *ip_peer = swidb->ip_peer;
    boolean approved = FALSE;	/* AAA has not approved the peer address */
    ipaddrtype server;		/* Address of Name Server to return */
    char *server_type;		/* Type of server requested */
    dialerdbtype *ddb;

    wo = f->wantoptions;
    ao = f->allowoptions;
    ho = f->heroptions;
    go = f->gotoptions;

    /*
     * Reset all her options.
     */
    ho->neg_addrs = 0;
    ho->neg_vj = 0;
    ho->neg_address = 0;

    /*
     * Process all her options.
     */
    while (l) {
	orc = CONFACK;			/* Assume success */
	cip = p;			/* Remember begining of CI */
	if (l < 2 ||			/* Not enough data for CI header or */
	    p[1] < 2 ||			/*  CI length too small or */
	    p[1] > l) {			/*  CI length too big? */
	    if (ppp_error_debug)
		buginf("\nppp: ipcp_reqci: bad CI length!");
	    orc = CONFREJ;		/* Reject bad CI */
	    cilen = l;			/* Reject till end of packet */
	    l = 0;			/* Don't loop again */
	    goto endswitch;
	}
	GETCHAR(citype, p);		/* Parse CI type */
	GETCHAR(cilen, p);		/* Parse CI length */
	l -= cilen;			/* Adjust remaining length */
	cilen -= 2;			/* Adjust cilen to just data */

	switch (citype) {		/* Check CI type */
	  case CI_ADDRS:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate addresses (obsolete)",
		     f->idb->hw_namestring);
	    if (!ao->neg_addrs ||
		cilen != 2 * sizeof(long)) { /* Check CI length */
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }

	    /*
	     * Handle the remote address.
	     */

	    PPP_GETLONG(tl, p);		/* Parse source address (her) */
	    ciaddr1 = h2nl(tl);
	    if (ppp_negotiation_debug)
		buginf(": her address %i", ciaddr1);

	    /*
	     * If we dialed a wellfleet router we need to provide them with
	     * their IP address, so look it up in the dialer map table.
	     */
	    if (!ho->address && is_ddr(f->idb)) {
		ddb = get_dialerdb(f->idb);
		addr = reg_invoke_dialer_remote_addr(f->idb, LINK_IP,
						     ddb->number_called);
		if (addr != NULL) {
		    ho->address = addr->addr.ip_address;
		    reg_invoke_set_ip_peer_addr(swidb, addr->addr.ip_address,
						PEER_ADDR_FROM_MAP);
		}
	    }

	    /*
	     * Now do AAA authorization.
	     *
	     * AAA will set the 'approved' flag if it has validated the
	     * IP address in ciaddr1 or provided an address to use instead.
	     * In that case, we would not attempt to validate the address
	     * using our own requirements.
	     */
	    f->ncp_did_author = TRUE;
	    if (!old_access_control) {
		if (!ipcp_authorize_addr(f, ciaddr1, ho, &approved)) {
		    return(TERMREQ);
		}
	    }
	    
	    /*
	     * Sanity check the incoming address.  Unless AAA has already
	     * approved the address, we need to verify that the address meets
	     * the configured requirements.
	     */
	    if (ciaddr1 && !approved)
		validate_peer_ip_addr(&ciaddr1, ho, swidb);

	    /*
	     * If the remote does not know its address, and we do not
	     * yet have a remote address (from AAA for example) then make
	     * one last attempt to acquire an address.
	     */
	    
	    if (!ciaddr1 && !ho->address) {
		ho->address = ipcp_allocate_addr(f, swidb);
		/*
		 * If we still don't have an address, send a REJ for the
		 * address option.
		 */
	        if (!ho->address) {
		    orc = CONFREJ;
		    if (ppp_negotiation_debug)
		      buginf("\nppp %s: Neither side knows remote address",
			     f->idb->hw_namestring);
		    INCPTR(sizeof(long), p); /* Point past second address */
		    break;
	        }
	    }

	    /*
	     * If the remote address is different, NAK with the correct
	     * address.
	     */

	    if (ho->address && (ciaddr1 != ho->address)) {
		orc = CONFNAK;	/* Send a NAK */
		ciaddr1 = ho->address; /* Send our remote address */
	    } else {
		/*
		 * Otherwise, accept her address and keep it.  Just
		 * setting the ho->address takes less effort then seeing
		 * if it needs to be saved.  Save her address as the peer
		 * address iff it differs from the current peer address.
		 */
		ho->address = ciaddr1;
		if (!ip_peer || (ip_peer->ippeer_addr != ciaddr1))
		    reg_invoke_set_ip_peer_addr(swidb, ciaddr1,
						PEER_ADDR_FROM_PEER);
	    }

	    /*
	     * Handle the local address
	     */

	    PPP_GETLONG(tl, p);		/* Parse desination address (ours) */
	    ciaddr2 = h2nl(tl);
	    if (ppp_negotiation_debug)
		buginf(", my address %i", ciaddr2);

	    /*
	     * If the remote does not know our address, and we do not
	     * know our address, reject the option.
	     */
	    
	    if (!ciaddr2 && !go->address) {
		orc = CONFREJ;
		if (ppp_negotiation_debug)
		  buginf("\nppp %s; Neither side knows local address",
			 f->idb->hw_namestring);
		break;
	    }

	    /*
	     * If the local address is different, NAK with the correct
	     * address.
	     */

	    if (go->address && ciaddr2 != go->address) {
		orc = CONFNAK;	/* Send a NAK */
		ciaddr2 = go->address; /* Send our local address */
	    }

	    /*
	     * If we are sending a NAK, update the parameters
	     */

	    if (orc == CONFNAK) {
		DECPTR(2 * sizeof(long), p);
		PPP_PUTLONG(ciaddr1, p); /* Send remote address */
		PPP_PUTLONG(ciaddr2, p); /* And local address */
		if (ppp_negotiation_debug)
		  buginf("\nppp %s: Sending IPCP obsolete address NAK, "
			 "local = %i, remote = %i",
			 f->idb->hw_namestring,
			 ciaddr2, ciaddr1);
		break;
	    }

	    /*
	     * We got this far, so we have success.
	     */

	    ho->neg_addrs = 1;
	    go->address = ciaddr2;
	    break;

	  case CI_COMPRESSTYPE:
	    if (ppp_negotiation_debug)
		buginf("\nppp %s: ipcp_reqci: rcvd COMPRESSTYPE",
			f->idb->hw_namestring);
	    if (!ao->neg_vj ||
		cilen != (LEN_CI_VJHDR-2)) {
		INCPTR(cilen, p);
		orc = CONFREJ;
		if (ppp_negotiation_debug)
		  buginf(" (rejected)");
		break;
	    }
	    PPP_GETSHORT(cishort, p);

	    /*
	     * Compresstype must be TYPE_PPP_VJCOMP
	     */
	    if (cishort != TYPE_PPP_VJCOMP) {
		DECPTR(sizeof(short), p);
		orc = CONFNAK;
		PPP_PUTSHORT(TYPE_PPP_VJCOMP, p);
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		/*
		 * Now we need to skip over the last two characters in the
		 * field since we are exiting early.
		 */
		INCPTR(2*sizeof(char), p);
		break;
	    }

	    GETCHAR(cichar, p);	/* Get the number of slots */
	    if (cichar > ao->vj_slots) { /* More slots than we can handle? */
		DECPTR(sizeof(char), p); /* Point back to slots */
		PUTCHAR(ao->vj_slots, p); /* Set our slot maximum */
		orc = CONFNAK;	/* Send a NAK */
		if (ppp_negotiation_debug)
		  buginf(" (negotiated from %d to %d slots)",
			 cichar, ao->vj_slots);
		/*
		 * Now we need to skip over the last character in the
		 * field since we are exiting early.
		 */
		INCPTR(sizeof(char), p);
		break;
	    }
	    ho->vj_slots = cichar;

	    GETCHAR(cichar, p);	/* Get the "compress session ID field */
	    if (ao->vj_csid)
	      ho->vj_csid = cichar;

	    ho->neg_vj = 1;
	    break;

	  case CI_ADDRESS:

	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate IP address", f->idb->hw_namestring);

	    if (!ao->neg_address ||
		cilen != sizeof(long)) { /* Check CI length */ 
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }

	    PPP_GETLONG(tl, p);		/* Parse remote address */
	    ciaddr1 = h2nl(tl);
	    if (ppp_negotiation_debug)
		buginf(": her address %i", ciaddr1);

	    /*
	     * If we dialed a wellfleet router we need to provide them with
	     * their IP address, so look it up in the dialer map table.
	     */
	    if (!ho->address && is_ddr(f->idb)) {
		ddb = get_dialerdb(f->idb);
		addr = reg_invoke_dialer_remote_addr(f->idb, LINK_IP,
						     ddb->number_called);
		if (addr != NULL) {
		    ho->address = addr->addr.ip_address;
		    reg_invoke_set_ip_peer_addr(swidb, addr->addr.ip_address,
						PEER_ADDR_FROM_MAP);
		}
	    }

	    /*
	     * Now do AAA authorization.
	     *
	     * AAA will set the 'approved' flag if it has validated the
	     * IP address in ciaddr1 or provided an address to use instead.
	     * In that case, we would not attempt to validate the address
	     * using our own requirements.
	     */
	    f->ncp_did_author = TRUE;
	    if (!old_access_control) {
		if (!ipcp_authorize_addr(f, ciaddr1, ho, &approved)) {
		    return(TERMREQ);
		}
	    }

	    /*
	     * Sanity check the incoming address.  Unless AAA has already
	     * approved the address, we need to verify that the address meets
	     * the configured requirements.
	     */
	    if (ciaddr1 && !approved)
		validate_peer_ip_addr(&ciaddr1, ho, swidb);

	    /*
	     * If the remote does not know its address, and we do not
	     * yet have a remote address (from AAA for example) then make
	     * one last attempt to acquire an address.
	     */
	    
	    if (!ciaddr1 && !ho->address) {
		ho->address = ipcp_allocate_addr(f, swidb);
		/*
		 * If we still don't have an address, send a REJ for the
		 * address option.
		 */
	        if (!ho->address) {
		    orc = CONFREJ;
		    if (ppp_negotiation_debug)
		      buginf("\nppp %s: Neither side knows remote address",
			     f->idb->hw_namestring);
		    break;
	        }
	    }

	    /*
	     * If we don't have an address, and they do, or we both do
	     * and they are the same, then this is success.  Save her
	     * address as the peer address iff it differs from the
	     * current peer address.
	     */

	    if (!ho->address || (ciaddr1 == ho->address)) {
		ho->neg_address = 1; /* Successfully negotiated address */
		ho->address = ciaddr1;
		if (!ip_peer || (ip_peer->ippeer_addr != ciaddr1))
		    reg_invoke_set_ip_peer_addr(swidb, ciaddr1,
						PEER_ADDR_FROM_PEER);
		break;
	    }

	    /*
	     * Different address, send a NAK with the correct one
	     */

	    orc = CONFNAK;
	    DECPTR(sizeof(long), p); /* Point back to remote address */
	    tl = ho->address;
	    PPP_PUTLONG(tl, p);
	    if (ppp_negotiation_debug)
	      buginf(" (NAK with address %i)", tl);
	    break;

	  case CI_PRIMARYDNS:
	    reg_invoke_bootp_ns_addr(DNS_PORT, TRUE, &server);
	    server_type = "Primary DNS";
	    goto server_common;		/* Use common code */

	  case CI_PRIMARYNBNS:
	    reg_invoke_bootp_ns_addr(NBNS_PORT, TRUE, &server);
	    server_type = "Primary NBNS";
	    goto server_common;		/* Use common code */

	  case CI_SECONDARYDNS:
	    reg_invoke_bootp_ns_addr(DNS_PORT, FALSE, &server);
	    server_type = "Secondary DNS";
	    goto server_common;		/* Use common code */

	  case CI_SECONDARYNBNS:
	    reg_invoke_bootp_ns_addr(NBNS_PORT, FALSE, &server);
	    server_type = "Secondary NBNS";
	    goto server_common;		/* Use common code */

	  server_common:
	    if (ppp_negotiation_debug)
		buginf("\nppp %s: Negotiate %s address",
		       f->idb->hw_namestring,
		       server_type);

	    if (cilen != sizeof(long)) { /* Check CI length */ 
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }

	    PPP_GETLONG(tl, p);		/* Parse remote address */
	    ciaddr1 = h2nl(tl);
	    if (ppp_negotiation_debug)
		buginf(": her address %i", ciaddr1);

	    /*
	     * If we don't have an address, send a REJ for this
	     * server option.
	     */
	    
	    if (!ciaddr1 && !server) {
		orc = CONFREJ;
		if (ppp_negotiation_debug)
		    buginf("\nppp %s: %s address unknown",
			   f->idb->hw_namestring,
			   server_type);
		break;
	    }

	    /*
	     * If we have an address, and it doesn't match theirs
	     * then NAK with the correct one.
	     */

	    if (server && (ciaddr1 != server)) {
	        orc = CONFNAK;
	        DECPTR(sizeof(long), p); /* Point back to remote address */
	        tl = server;
	        PPP_PUTLONG(tl, p);
	        if (ppp_negotiation_debug)
	            buginf(" (NAK with address %i)", tl);
	    }

	    break;

	  default:
	    if (ppp_error_debug)
	      buginf("\nppp %s: Rejecting unknown IPCP option %d",
		     f->idb->hw_namestring, citype);
	    INCPTR(cilen, p);
		orc = CONFREJ;
	    break;
	}
	cilen += 2;			/* Adjust cilen whole CI */

endswitch:

	if (ppp_negotiation_debug)
	    buginf(" (%s)",
		   orc == CONFACK ? "ACK" : (orc == CONFNAK ? "NAK" : "REJ"));
	if (orc == CONFACK &&		/* Good CI */
	    rc != CONFACK)		/*  but prior CI wasnt? */
	    continue;			/* Don't send ther one */

	if (orc == CONFNAK) {		/* Nak this CI? */
	    if (rc == CONFREJ)		/* Rejecting prior CI? */
		continue;		/* Don't send this one */
	    if (rc == CONFACK) {	/* Ack'd all prior CIs? */
		rc = CONFNAK;		/* Not anymore... */
		ucp = inp;		/* Backup */
	    }
	}
	if (orc == CONFREJ &&		/* Reject this CI */
	    rc != CONFREJ) {		/*  but no prior ones? */
	    rc = CONFREJ;
	    ucp = inp;			/* Backup */
	}
	if (ucp != cip)			/* Need to move CI? */
	    bcopy(cip, ucp, cilen);	/* Move it */
	INCPTR(cilen, ucp);		/* Update output pointer */
    }

    /*
     * Now do AAA authorization iff we haven't done it till now.
     *
     * We do this primarily to load any suggested IP addresses that
     * have not been processed because the peer did not provide an
     * IPCP Address (or Addresses) option.
     * IP address in ciaddr1 or provided an address to use instead.
     * In that case, we would not attempt to validate the address
     * using our own requirements.
     */
    if (!f->ncp_did_author) {
	f->ncp_did_author = TRUE; /* set to avoid failure in fsm_check_author */
	ciaddr1 = 0;		/* yep, the peer is requesting an address :-) */
	if (!old_access_control &&
	    !ipcp_authorize_addr(f, ciaddr1, ho, &approved)) {
	    return(TERMREQ);
	}
    }

    /*
     * XXX If we wanted to send additional NAKs (for unsent CIs), the
     * code would go here.  This must be done with care since it might
     * require a longer packet than we received.
     */

    *len = ucp - inp;			/* Compute output length */
    if (ppp_negotiation_debug)
	buginf("\nppp: ipcp_reqci: returning %s.",
	      rc == CONFACK ? "CONFACK" :
	      rc == CONFNAK ? "CONFNAK" : "CONFREJ");
    return (rc);			/* Return final code */
}


/*
 * ipcp_add_p2p_neighbor
 *
 * If necessary (no other route to peer exists), tell IP routing to add
 * a neighbor route.
 *
 * Note that if the interface is a member of a rotary group, we
 * look at the top interface to see if a route exists, but we
 * store the route in the low-level interface's ip_peer structure.
 */

static void ipcp_add_p2p_neighbor (idbtype *swidb)
{
    ippeerinfotype *ip_peer;
    ipaddrtype neighbor;
    idbtype *top_swidb;
    userstruct *user;

    /*
     * Sanity checks before we continue
     */
    if (!swidb || !swidb->ip_peer || !swidb->ip_peer->ippeer_addr)
	return;

    ip_peer = swidb->ip_peer;
    neighbor = ip_peer->ippeer_addr;

    /*
     * IP only knows and cares about the dialer interface on top
     * and that is where we keep our permanent configuration.
     */
    top_swidb = get_netcfg_idb(swidb);

    /*
     * record address in accounting tables.  Note that we do this even
     * if we don't need to install a route, since it's our only chance
     */
    if (swidb->hwptr->lcpstate) {
	if (is_multilink(swidb->hwptr)) {
	    user = swidb->hwptr->mlp_link->bundle->first_link->idb->lcpstate->ustruct;
	} else {
	    user = swidb->hwptr->lcpstate->ustruct;
	}
	reg_invoke_aaa_acct_netinfo(user, AAA_ATTR_PROTO,
				    "ip", AAA_TYPE_STR, 2, FALSE);
	reg_invoke_aaa_acct_netinfo(user, AAA_ATTR_ADDR, &neighbor,
				    AAA_TYPE_IP, sizeof neighbor, TRUE);
    }

    /*
     * Flush any old routes.
     */
    if (ip_peer->neighbor && (ip_peer->neighbor != neighbor)) {
	ipcp_remove_p2p_neighbor(top_swidb, ip_peer->neighbor);
    }

    /*
     * This serves as a flag for a lot of different reasons.
     */
    ip_peer->neighbor = neighbor;

    /*
     * Maybe we don't want a neighbor route!
     * Use top-level interface's configuration (if it exists) as a guide.
     */
    if (top_swidb->ip_peer && top_swidb->ip_peer->no_neighbor_route)
	return;

    if (reg_invoke_ip_add_p2p_neighbor_route(top_swidb, neighbor)) {
	if (peer_debug || ppp_negotiation_debug)
	    buginf("\n%s: install route to %i",
		   top_swidb->namestring, neighbor);
    }
}

static void
create_per_user_info(idbtype *swidb)
{
    if (swidb->per_user_info) {
	return;
    }
    swidb->per_user_info = malloc_named(sizeof(per_user_info), "T+ params");
}

/* Get the peer address (if any) for an interface. */
static ipaddrtype
get_peer_addr (hwidbtype *idb)
{
    idbtype *swidb = firstsw_or_null(idb);
    if (swidb && swidb->ip_peer) {
	return(swidb->ip_peer->ippeer_addr);
    }
    return(IPADDR_ZERO);
}

/*
 * Install a per-user route (if any) stored in the T+ info portion of an idb. 
 */
static void
install_per_user_route(hwidbtype *idb)
{
    idbtype *swidb = firstsw_or_null(idb);
    ipaddrtype gateway;
    per_user_info *info;

    if (!(swidb && swidb->per_user_info && swidb->per_user_info->network)) {
	return;
    }
    
    info = swidb->per_user_info;
    gateway = info->gateway;
    if (!gateway) {
	gateway = get_peer_addr(idb);
    }

    if (aaa_author_debug) {
	buginf("\nAAA/AUTHOR/%s: %s add ip route=%i%m %i", 
	       encstring(idb->enctype), idb->hw_namestring,
	       info->network, info->mask, gateway);
    }
   
    reg_invoke_ip_add_static_route(info->network, 
				   info->mask, 
				   gateway, 
				   NULL, /* idb */
				   0, /* tag */
				   NET_PER_USER, /* flags */
				   STATIC_DISTANCE, 
				   idb); /* per-user hwidb */
}

/*
 * ipcp_up - FSM has reached up state
 */

static void ipcp_up (fsm *f)
{
    hwidbtype *idb;
    idbtype *swidb;
    ipcp_options *go;
    ipcp_options *ho;
    addrtype addr;
    ipaddrtype peer_address;

    idb = f->idb;		/* Get the IDB */

    go = f->gotoptions;
    ho = f->heroptions;

    swidb = idb->firstsw;
    if (swidb && swidb->ip_peer) {
        peer_address = swidb->ip_peer->ippeer_addr;
    } else {
        peer_address = (ipaddrtype)0L;
    }

    if (old_access_control) {
	/*
	 * KLUDGE Alert:  Play with fire!!!!
	 * The following call to reg_invoke_login_slipon() may dismiss
	 * during XTACACS authorization.  This may allow some traffic to
	 * fastswitch before the Access Control Lists are in place.  In
	 * order to block that traffic, we change the state temporarily.
	 * This SHOULD be OK because all PPP/IPCP processing is done at
	 * process level.
	 */
	f->state = ACKRCVD;		/* Block IP traffic */
	if (reg_invoke_login_slipon(idb->lcpstate->user,
				    NULL, NULL, idb->idb_tty,
				    peer_address, TRUE, FALSE) == FALSE) {
	    /*
	     * Authorization failed so bring down the entire PPP Link.
	     * This is somewhat harsh, but since XTACACS can only authorize
	     * IP (Not IPX or ...) then we must assume the intent was to
	     * deny PPP altogether.  If you want finer grained control,
	     * use AAA and TACACS+.
	     */
	    f->state = OPEN;            /* Let IP traffic pass */
	    lcp_close(idb->lcpfsm);
	    return;
	}

	/*
	 * XTACACS should have loaded the ACLs by now so reenable IP
	 * [CSCdi46355])
	 */
	f->state = OPEN;		/* Let IP traffic pass */
    } else { /* ! old_access_control */
	/* Dont do authorization here.  It has already been done */
    }

    reg_invoke_media_protocol_up(idb, LINK_IP);

    if (peer_address) {
        /*
         * Tell IP routing about our new neighbor
         *
         * IPCP may have renegotiated the address and the previous
         * installed route may be incorrect.  Make sure the correct
         * route is installed here after all address negotiation is done.
         */
	ipcp_add_p2p_neighbor(swidb);
	/*
	 * Send gratuitous ARPs so other know we have this address
	 */
	reg_invoke_ip_arp_send_gratuitous_arps(swidb, peer_address);
	/*
	 * Add a dynamic dialer map (if possible and necessary)
	 */
        memset(&addr, 0, sizeof(addrtype));
        addr.type = ADDR_IP;
        addr.length = ADDRLEN_IP;
        addr.ip_addr = idb->firstsw->ip_peer->ippeer_addr;
        reg_invoke_dialer_create_dynamic_map(LINK_IP, idb, &addr);
	if (vty_logging && (idb->type == IDBTYPE_VTY_ASYNC)) {
	    buginf("\nVTY-ASYNC%t: IP addr %i", idb->idb_tty->ttynum, 
		   peer_address);
	}
    }

    /*
     * Set up the proper state for VJ compression
     */

    if (idb->tcp_compress) {
	thc_compress_reset(idb->tcp_compress,
			   ho->neg_vj ? ho->vj_slots + 1 : 0,
			   go->neg_vj ? go->vj_slots + 1 : 0);
	if (ho->neg_vj)		/* Can we compress packets? */
	  idb->tcp_compress->flags |= THC_ACTIVE; /* Yes, start doing so */
    }

    /* 
     * Install a T+ per-user ip route if it was specified. 
     */
    install_per_user_route(idb);

}

/*
 * ipcp_check_parallel_neighbor
 *
 * Recursively (if necessary) check for a parallel connection.  Call this each
 * time with an IDB.  If the IDB is a huntgroup master, it will call itself
 * for each member.  To have a parallel connection, IPCP must be Open, the
 * neighbor address must be the same.
 *
 * Returns NULL if this IDB or any below are not parallel, else
 * returns a pointer to the swidb that meets all of the criteria.
 */

static idbtype *ipcp_check_parallel_neighbor (hwidbtype *hwidb,
					      ipaddrtype neighbor)
{
    idbtype *swidb;
    dialerdbtype *ddb, *member_ddb;
    dialergrouptype *grptr;
    int i;

    /*
     * Start with the sanity checks
     */
    if (!hwidb || !neighbor)
	return NULL;

    /*
     * Check all connected members under the master ddb
     */
    ddb = get_dialerdb(hwidb);
    if (ddb) {
	FOR_ALL_CONNECTEDGROUPS_ON_MASTER(ddb, grptr) {
	    FOR_ALL_MEMBERS_IN_GROUP(grptr, i, member_ddb) {
		swidb = member_ddb->ownidb->firstsw;
		if (swidb &&
		    swidb->ip_peer &&
		    (swidb->ip_peer->neighbor == neighbor)) {
		    return swidb;
		}
	    }
	}
    }

    return NULL;
}

/*
 * ipcp_check_parallel_per_user
 *
 * Returns NULL if this IDB or any below are not parallel, else
 * returns a pointer to the swidb that meets all of the criteria.
 */

static idbtype *ipcp_check_parallel_per_user (idbtype *swidb,
					      idbtype *top_swidb,
					      per_user_info *info)
{
    idbtype *tmp_swidb;
    dialerdbtype *ddb, *member_ddb;
    dialergrouptype *grptr;
    per_user_info *tmp_info;
    ipaddrtype gateway;
    int i;

    /*
     * Start with the sanity checks
     */
    if (!swidb || !top_swidb || !info)
	return NULL;

    gateway = get_peer_addr(swidb->hwptr);
    /*
     * Check all connected members under the master ddb
     */
    ddb = get_dialerdb(top_swidb->hwptr);
    if (ddb) {
	FOR_ALL_CONNECTEDGROUPS_ON_MASTER(ddb, grptr) {
	    FOR_ALL_MEMBERS_IN_GROUP(grptr, i, member_ddb) {
		tmp_swidb = member_ddb->ownidb->firstsw;
		if (tmp_swidb &&
		    (tmp_swidb != swidb) &&
		    tmp_swidb->per_user_info) {
		    tmp_info = tmp_swidb->per_user_info;
		    if ((info->network == tmp_info->network) &&
			(info->mask == tmp_info->mask) &&
			(info->gateway == tmp_info->gateway) &&
			(info->gateway ||
			 (gateway == get_peer_addr(tmp_swidb->hwptr)))) {
			return tmp_swidb;
		    }
		}
	    }
	}
    }

    return NULL;
}

/*
 * ipcp_remove_p2p_neighbor
 * Tell IP routing that our neighbor went away
 */

static void ipcp_remove_p2p_neighbor (idbtype *top_swidb, ipaddrtype neighbor)
{
    reg_invoke_iprouting_delete_conn_route_int(neighbor, MASK_HOST, top_swidb);

    if (peer_debug || ppp_negotiation_debug)
	buginf("\n%s: remove route to %i", top_swidb->namestring, neighbor);
}

/*
 * ipcp_cleanup_ip_peer
 * Cleanup IP interface because it is down, going down, being unbundled, etc.
 * Return the IP peer address so that it can be used by XTACACS.
 */

static ipaddrtype ipcp_cleanup_ip_peer (idbtype *swidb)
{
    idbtype *parallel_idb, *top_swidb;
    addrtype addr;
    ipaddrtype neighbor;

    /*
     * Assume that we will want to remove any route and dialer maps.
     */
    parallel_idb = NULL;

    /*
     * start with the obvious sanity check
     */
    if (!swidb)
	return (ipaddrtype)0L;

    /*
     * We will need to know who the route was pointing to.
     */
    top_swidb = get_netcfg_idb(swidb);

    /*
     * Get the peer IP address (if it exists) from the IP peer address
     * structure.
     */
    if (swidb->ip_peer) {
	neighbor = swidb->ip_peer->neighbor;
    } else {
	neighbor = (ipaddrtype)0L;
    }

    /*
     * We are clear to remove references to this address.  So first, the
     * address must exist and second, the address must have been used for
     * a dynamic dialer map and/or a neighbor route.
     */
    if (neighbor) {
	swidb->ip_peer->neighbor = 0L;

	/*
	 * But first we have to check for a parallel route.  We will not
	 * remove the route or the dynamic dialer map or per-user route
	 * if a parallel route exists under the same dialer interface.
	 */
	if (swidb != top_swidb) {
	    parallel_idb = ipcp_check_parallel_neighbor(top_swidb->hwptr,
							neighbor);
	}

	if (parallel_idb) {
	    /*
	     * Leave everything be since removing our route will remove hers
	     * (we all appear to be one interface to the IP routing code).
	     */
	    if (peer_debug || ppp_negotiation_debug)
		buginf("\n%s: route to %i still needed by %s",
		       swidb->namestring, neighbor,
		       parallel_idb->namestring);
	} else {
	    /*
	     * Tell IP routing that our neighbor went away
	     */
	    ipcp_remove_p2p_neighbor(top_swidb, neighbor);
	    /*
	     * If we should have removed a neighbor route, then remove
	     * any dynamic dialer map that was created.  This covers the
	     * parallel connection case where a dialer will have one
	     * neighbor route AND one dynamic dialer map for two parallel
	     * connections.
	     */
	    memset(&addr, 0, sizeof(addrtype));
	    addr.type = ADDR_IP;
	    addr.length = ADDRLEN_IP;
	    addr.ip_addr = neighbor;
	    reg_invoke_dialer_delete_dynamic_map(LINK_IP, swidb->hwptr, &addr);
	}
    }

    /* 
     * De-install a T+ stored per-user ip route if one exists. 
     * First look for any other interfaces with the same per-user
     * route, since clearing ours will clear theirs as well.
     */
    if (swidb->per_user_info) {
	/*
	 * But first we have to check for a parallel route.  We will not
	 * remove the route or the dynamic dialer map or per-user route
	 * if a parallel route exists under the same dialer interface.
	 */
	if (swidb != top_swidb) {
	    parallel_idb = ipcp_check_parallel_per_user(swidb,
							top_swidb,
							swidb->per_user_info);
	}
	/*
	 * If a parallel connection was found, assign the route to that idb.
	 * Else delete all static routes owned by this idb.
	 */
	if (parallel_idb) {
	    install_per_user_route(parallel_idb->hwptr);
	    if (aaa_author_debug)
		buginf("\n%s: per-user route to %i%m still needed by %s",
		       swidb->namestring, swidb->per_user_info->network,
		       swidb->per_user_info->mask, parallel_idb->namestring);
	} else {
	    reg_invoke_ip_delete_per_user_route(swidb->hwptr);
	}
	free(swidb->per_user_info);
	swidb->per_user_info = NULL;
    }

    return (neighbor);
}

/*
 * ipcp_remove_per_user_access_lists
 *
 * Remove any per-user authorisaton applied to the specified interface
 */
static void ipcp_remove_per_user_access_lists (hwidbtype *idb)
{
    idbtype *swidb;

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	reg_invoke_ip_set_per_user_acl(swidb, IN, IP_ACL_USER_RESET);
	reg_invoke_ip_set_per_user_acl(swidb, OUT, IP_ACL_USER_RESET);
    }
}

/*
 * ipcp_down - FSM has left the Open state
 */

static void ipcp_down (fsm *f)
{
    hwidbtype *idb;
    idbtype *swidb;
    ipaddrtype ippeer_addr;
    ipaddrtype neighbor;

    idb = f->idb;			/* Get the IDB */
    swidb = firstsw_or_null(idb);

    if (old_access_control) {
	 neighbor = (ipaddrtype)0L;
	 if (swidb->ip_peer) {
	      neighbor = swidb->ip_peer->neighbor;
	 }
	 reg_invoke_login_slipoff(idb->idb_tty, neighbor, idb, TRUE);
    }

    /*
     * start with the obvious sanity check
     */
    if (!swidb)
	return;

    /*
     * Cleanup any use of the ip peer address including the neighbor route
     * and the dynamic dialer map (if they were created).
     */
    ippeer_addr = ipcp_cleanup_ip_peer(swidb);

    if (!old_access_control) {
	/* Async idb's default to static routing. Sync lines default
	 * to dynamic */
	swidb->static_routing = FALSE;

	/* Undo any per user authorization settings */
	ipcp_remove_per_user_access_lists(idb);
    }
}

/*
 * ipcp_closed - FSM has reached the Closed state
 */
static void ipcp_closed (fsm *f)
{
    hwidbtype *idb;

    idb = f->idb;			/* Get the IDB */

    /*
     * at the present time (11/1/95) there is nothing registered
     * for this routine, but for completeness include the call.
     */
    reg_invoke_media_protocol_down(idb, LINK_IP);
}

/*
 * ipcp_net_cstate
 *
 * Called the moment that the interface changes state, we will use the
 * new state as an indication to add or remove a neighbor route provided
 * that the neighbor address is set.  This will be a NOP for many calls
 * since PPP lines come up before IPCP negotiation.  However, there are
 * enough cases where the call to reg_invoke_ip_add_p2p_neighbor_route()
 * will be ignored that we are better off just repeating the call so
 * that we ensure that the route is added.
 *
 * Note that we ignore the case where the interface has gone down as there
 * other places in the code that will clean up.  Also note that we will
 * recursively call ourselves for all member interfaces if this a hunt group.
 */

static void ipcp_net_cstate (hwidbtype *hwidb)
{
    idbtype *swidb, *top_swidb;
    dialerdbtype *ddb, *tmp_ddb;
    ipaddrtype neighbor;

    /*
     * Start with the sanity checks
     */
    if (!hwidb || (hwidb->state != IDBS_UP)) {
	return;
    }

    /*
     * Recursive calls to handle rotary of rotaries
     */
    ddb = get_dialerdb(hwidb);
    if ((ddb != NULL) && HUNTGROUP_LEADER(ddb)) {
        FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tmp_ddb) {
	    ipcp_net_cstate(tmp_ddb->ownidb);
        }
	return;
    }

    swidb = hwidb->firstsw;

    /*
     * Continue with the sanity checks
     */
    if (!swidb || !swidb->ip_peer || !swidb->ip_peer->neighbor) {
	return;
    }

    /*
     * For every UP interface with a neighbor, try to add a route.
     * Always point the route at the top-level dialer
     */
    top_swidb = get_netcfg_idb(swidb);
    neighbor = swidb->ip_peer->neighbor;

    /*
     * Maybe we don't want a neighbor route!
     * Use top-level interface's configuration (if it exists) as a guide.
     */
    if (top_swidb->ip_peer && top_swidb->ip_peer->no_neighbor_route)
	return;

    if (reg_invoke_ip_add_p2p_neighbor_route(top_swidb, neighbor)) {
	if (peer_debug || ppp_negotiation_debug)
	    buginf("\n%s: install route to %i",
		   top_swidb->namestring, neighbor);
    }
}

/*
 *   Last ditch attempt to find an address for a peer who claims to have none.
 *
 *   Note that we got here because the remote did not have an address nor
 *   did we.
 *
 *   Note that AAA is not applicable as it would already have been called by
 *   this point without providing nor requiring an IP address.
 */

static ipaddrtype ipcp_allocate_addr (fsm *f, idbtype *swidb)
{
    idbtype *cfg_swidb;
    idbtype *idb = NULL;
    char *uname;

    /*
     * Hunt groups (incl ISDN) store their configuration in a common spot.
     * The configuration IDB may be associated with a different type of
     * interface altogether e.g. V.120. Should that be the case then
     * idb will contain the appropriate IDB.
     */
    if (swidb && swidb->hwptr && swidb->hwptr->idb_tty &&
	swidb->hwptr->idb_tty->cfgidb) {
	idb = (*swidb->hwptr->idb_tty->cfgidb)(swidb->hwptr->idb_tty);
    }
    cfg_swidb = get_netcfg_idb(idb ? idb : swidb);

    if (!swidb->ip_peer && !create_ippeerinfo(swidb))
	return 0L;

    if (!cfg_swidb->ip_peer && !create_ippeerinfo(cfg_swidb))
	return 0L;

    /*
     * At this point, we have only one possibility.  If pooling is enabled
     * (the default case), then attempt to allocate an address from the pool.
     */

    if (cfg_swidb->ip_peer->ippeer_pool_type != PEER_ADDR_NONE) {

        ipaddrtype tmpaddr = 0;
        uchar *poolname = cfg_swidb->ip_peer->ippeer_poolname_def;
        int ret;

        if (ppp_negotiation_debug) {
	    if (poolname)
	        buginf("\nppp: %s: using pool '%s'",
		       swidb->namestring, poolname);
	    else
	        buginf("\nppp: %s: using the default pool",
		       swidb->namestring);
	}

	uname = NULL;
	(void)reg_invoke_get_authname(swidb->hwptr, &uname);
        ret = reg_invoke_ip_get_pool_addr(&tmpaddr, swidb, uname,
	    poolname, 4 * ONESEC);

        if (ret) {
	    if (ppp_negotiation_debug)
	        buginf("\nppp: %s: pool returned %i",
		       swidb->namestring, tmpaddr);
	    return(tmpaddr);
        } else {
	    if (ppp_negotiation_debug)
	        buginf("\nppp: %s: cannot satisfy pool request",
		       swidb->namestring);
	}
    } else {
	if (ppp_negotiation_debug)
	    buginf("\nppp: %s: no peer address configured",
		   swidb->namestring);
    }

    /*
     * Well, we looked everywhere.  Give up and return nothing.  User
     * should have configured a default!
     */

    return(0L);
}

/*
 * Validate incoming IP address against the interface configuration.  If the
 * administrator has specified a pool name then the peer's address must be a
 * member of that pool and available for assignment.  If so, mark the pool
 * address in use, else attempt to allocate a different address from the same
 * pool.
 *
 * If an address has been configured thru one of the many mechanisms, eg
 * from an exec-level 'ppp hostname' command, then we want to ensure only
 * that the peer's address matches the current peer address.  In a way, you
 * could think of these mechanisms as temporarily setting the default address.
 */
static void validate_peer_ip_addr (ipaddrtype *ciaddr1,
				   ipcp_options *ho,
				   idbtype *swidb)
{
    ipaddrtype tmpaddr = *ciaddr1;
    int ret;
    idbtype *cfg_swidb;
    idbtype *idb = NULL;
    char *uname;

    /*
     * Hunt groups (incl ISDN) store their configuration in a common spot.
     * The configuration IDB may be associated with a different type of
     * interface altogether e.g. V.120. Should that be the case then
     * idb will contain the appropriate IDB.
     */
    if (swidb && swidb->hwptr && swidb->hwptr->idb_tty &&
	swidb->hwptr->idb_tty->cfgidb) {
	idb = (*swidb->hwptr->idb_tty->cfgidb)(swidb->hwptr->idb_tty);
    }
    cfg_swidb = get_netcfg_idb(idb ? idb : swidb);

    if (!swidb->ip_peer && !create_ippeerinfo(swidb))
	return;

    if (!cfg_swidb->ip_peer && !create_ippeerinfo(cfg_swidb))
	return;

    /*
     * Time to see whether we need to use pooling.  If we have a current
     * peer address for any reason other pooling then we want to use that
     * address no matter what.  The peer does not get to say 'ppp host1'
     * and then negotiate for 'host2'.
     */

    switch (swidb->ip_peer->ippeer_addr_source) {
    case PEER_ADDR_FROM_AAA:
    case PEER_ADDR_FROM_EXEC:
    case PEER_ADDR_FROM_VTY:
    case PEER_ADDR_FROM_MAP:
    case PEER_ADDR_FROM_CHAT:
	return;		/* ho->address is already the current peer addr */

    case PEER_ADDR_FROM_CONFIG:
    case PEER_ADDR_FROM_DHCP:
    case PEER_ADDR_FROM_POOL:
    case PEER_ADDR_FROM_PEER:
	break;		/* let the IP Pooling code do the validation */
    }

    uname = NULL;
    (void)reg_invoke_get_authname(swidb->hwptr, &uname);
    ret = reg_invoke_ip_get_pool_addr(&tmpaddr, swidb, uname,
	cfg_swidb->ip_peer->ippeer_poolname_def, 4*ONESEC);

    if (ret)
	ho->address = tmpaddr;		/* make sure we accept his addr */
    else if (swidb->ip_peer->ippeer_addr)
	ho->address = swidb->ip_peer->ippeer_addr; /* use the best we have */
    else
	*ciaddr1 = 0L;		/* make sure that we reject his address */
}


/*
 *   Set parameters for ppp address negotiation from AAA. The fsm will
 *   already have made one authorization call for ipcp and those
 *   values will be in the cache.
 *
 *   ciaddr1 contains the address proposed by the remote.
 *   ho->address will contain the remote address we propose
 *
 *   Return FALSE if authorization denies the connection
 */

static boolean do_ipcp_authorize_addr (fsm *f, ulong ciaddr1, ipcp_options *ho,
				       boolean *approved)
{
    ipaddrtype tmpaddr = 0;
    enum AV_TYPE type = AAA_AV_OPT;
    uchar *poolname;
    int ret;
    hwidbtype *idb = f->idb;
    idbtype *swidb = firstsw_or_null(idb);
    char *uname;

    if (!ciaddr1) {
	ipcp_author_get_addr(f->cached_author, &tmpaddr, &type);
	if (tmpaddr) {
	    /* Remote proposed no address. AAA proposed address A */
	    ho->address = tmpaddr;
	    *approved = TRUE;
	    reg_invoke_set_ip_peer_addr(swidb, tmpaddr, PEER_ADDR_FROM_AAA);
	    return(TRUE);
	}

	/*
	 * Remote did not propose an address. AAA says use pool S.
	 * Try to get any address A from S, recording it in the
	 * authorization cache for accounting routines */

	ipcp_author_get_pool(f->cached_author, &poolname, &type);
	if (poolname) {

	    if (ppp_negotiation_debug || aaa_author_debug) {
		buginf("\nAAA/AUTHOR/IPCP: %s: says use pool %s",
		       idb->hw_namestring, poolname);
	    }

	    uname = NULL;
	    (void)reg_invoke_get_authname(idb, &uname);
	    ret = reg_invoke_ip_get_pool_addr(&tmpaddr, swidb, uname,
		poolname, 4 * ONESEC);

	    if (ret) {
		ho->address = tmpaddr;
	        *approved = TRUE;

		/* install this address in the cache for accounting */
		ipcp_author_set_addr(&f->cached_author, ho->address);

		if (ppp_negotiation_debug || aaa_author_debug) {
		    buginf("\nAAA/AUTHOR/IPCP: %s: pool returned %i",
			   idb->hw_namestring, tmpaddr);
		}
		return(TRUE);
	    }

	    /* Failed to get a pool address. If it was mandatory to do
	     * so, deny the request */

	    if (type == AAA_AV_MAND) {
		if (ppp_negotiation_debug || aaa_author_debug) {
		    buginf("\nAAA/AUTHOR/IPCP: %s: cannot satisfy pool request",
			   idb->hw_namestring);
		}
		return(FALSE);
	    }
	}

	/* Remote did not propose an address. AAA did not propose an
	   address or a pool. */
	return(TRUE);
    }

    /*
     * Remote proposes R, AAA said use pool P.  If we can get R from
     * P, that's fine. Otherwise, try to get any address A from P. */
 
    ipcp_author_get_pool(f->cached_author, &poolname, &type);

    if (poolname) {

	if (ppp_negotiation_debug || aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/IPCP: %s: request %i from pool %s",
		   idb->hw_namestring, ciaddr1, poolname);
	}

	/*
	 * Try to acquire  the address R or any other from the pool P.
	 * Fail only if no addresses are left in pool P.
	 */
	uname = NULL;
	(void)reg_invoke_get_authname(idb, &uname);
	ret = reg_invoke_ip_get_pool_addr(&ciaddr1, swidb, uname,
	    poolname, 4 * ONESEC);

	if (ret) {
	    if (ppp_negotiation_debug || aaa_author_debug) {
		buginf("\nAAA/AUTHOR/IPCP: %s: pool grants %i",
		       idb->hw_namestring, ciaddr1);
	    }

	    /* Success. R or another is now marked in-use for us from pool P */
	    ho->address = ciaddr1;
	    *approved = TRUE;
	    ipcp_author_set_addr(&f->cached_author, ho->address);
	    return(TRUE);
	}

	/* No address available from pool. Deny request outright if
           pooling it was mandatory */

	if (ppp_negotiation_debug || aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/IPCP: %s: no address granted from pool",
		   idb->hw_namestring);
	}

	if (type == AAA_AV_MAND) {
	    if (ppp_negotiation_debug || aaa_author_debug) {
		buginf("\nAAA/AUTHOR/IPCP: %s: Unsatisfiable mandatory "
		       "pool request", idb->hw_namestring);
	    }
	    return(FALSE);
	}

	/* Remote proposed R, AAA said use pool P, we couldn't get
	 * anything from the pool but it wasn't mandatory */

	/* Fall out of this here.  Go check for specific addrs below. */
    }

    /* Remote proposed address R. AAA does not return a pool.
     * We need to reauthorize this address if it's new */

    /* Has AAA already okayed R? */
    ipcp_author_get_addr(f->cached_author, &tmpaddr, &type);
    if (tmpaddr && (tmpaddr == ciaddr1)) {
	/* AAA wants this address */
	ho->address = tmpaddr;
	*approved = TRUE;
	reg_invoke_set_ip_peer_addr(swidb, tmpaddr, PEER_ADDR_FROM_AAA);
	return(TRUE);
    }

    /* See if R is acceptable to AAA. Propose AAA's answer
       as the address we want */

    if (!ipcp_do_author(idb, &f->cached_author, ciaddr1)) {
	return(FALSE); /* permission denied */
    }

    ipcp_author_get_addr(f->cached_author, &tmpaddr, &type);
    if (tmpaddr) {
	/* AAA wants this address */
	ho->address = tmpaddr;
	*approved = TRUE;
	reg_invoke_set_ip_peer_addr(swidb, tmpaddr, PEER_ADDR_FROM_AAA);
	return(TRUE);
    }

    /* If we come here, AAA has no opinion about R so we'll accept it */

    if (ppp_negotiation_debug || aaa_author_debug) {
	buginf("\nAAA/AUTHOR/IPCP: %s: Reject %i, using %i", idb->hw_namestring,
	       ciaddr1, ho->address);
    }

    ipcp_author_set_addr(&f->cached_author, ho->address);
    return(TRUE);
}

static boolean ipcp_authorize_addr (fsm *f, ulong ciaddr1, ipcp_options *ho,
				    boolean *approved)
{
    boolean ret;

    if (!aaa_author_needed(AAA_AUTHOR_TYPE_NET, 0, f->idb, f->idb->idb_tty)) {
	return(TRUE);
    }

    if (aaa_author_debug || ppp_negotiation_debug) {
	buginf("\nAAA/AUTHOR/IPCP: %s: start: her address %i, we want %i",
	       f->idb->hw_namestring, ciaddr1, ho->address);
    }

    ret = do_ipcp_authorize_addr(f, ciaddr1, ho, approved);

    if (ret) {
	ret = ipcp_process_author_params(f->idb, f->cached_author);
    }

    if (ppp_negotiation_debug || aaa_author_debug) {
	if (ret) {
	    buginf("\nAAA/AUTHOR/IPCP: %s: done: her address %i, we want %i",
		   f->idb->hw_namestring, ciaddr1, ho->address);
	}
	buginf("\nAAA/AUTHOR/IPCP: %s: authorization %s",
	       f->idb->hw_namestring,  ret ? "succeeded" : "denied");
    }
    return(ret);
}

/*
 * Authorization code for IPCP. */
static void ipcp_author_get_pool (char **a, uchar **poolname,
				  enum AV_TYPE *type)
{
    int i;
    char *attr, *val;
    enum AV_TYPE t;

    if (!a) {
	*poolname = NULL;
	return;
    }
    for (i = 0; a[i]; i++) {
	if (aaa_split_AV(a[i], &attr, &val, &t)) {
	    if (!strcmp(attr, AAA_ATTR_addr_pool)) {
		*poolname = val;
		*type = t;
		return;
	    }
	}
    }
    *poolname = NULL;
}

static void ipcp_author_get_addr (char **a, ipaddrtype *addr,
				  enum AV_TYPE *type)
{
    int i;
    char *attr, *val;
    enum AV_TYPE t;

    if (!a) {
	*addr = '\0';
	return;
    }
    for (i = 0; a[i]; i++) {
	if (aaa_split_AV(a[i], &attr, &val, &t)) {
	    if (!strcmp(attr, AAA_ATTR_addr)) {
		ipaddrtype tmp;
		if (parse_ip_address(val, &tmp)) {
		    *addr = tmp;
		    *type = t;
		}
		return;
	    }
	}
    }
    *addr = '\0';
}

static void ipcp_author_set_addr (char ***a, ipaddrtype addr)
{
    char buf[32];

    sprintf(buf, "%i", addr);
    aaa_set_attr(a, AAA_ATTR_addr, buf, AAA_AV_OPT);
}

/* Do ipcp authorization with a proposed address */
static boolean ipcp_do_author (hwidbtype *idb, char ***attrs,
			       ipaddrtype addr)
{
    boolean ret = TRUE;
    int avail = 0;
    char buf[64];
    authorstruct a;


    if (!aaa_author_needed(AAA_AUTHOR_TYPE_NET, 0, idb, idb->idb_tty))
	return(TRUE);   /* Authorization not required. */

    memset(&a, 0, sizeof(authorstruct));
    a.auth_idb = idb;
    a.auth_tty = idb->idb_tty;

    /* First step.  Build outgoing A-V lists. */
    if (!aaa_add_AV(&a.out_args, &avail, AAA_ATTR_service, AAA_VAL_SVC_ppp,
		    AAA_AV_MAND) ||
	!aaa_add_AV(&a.out_args, &avail, AAA_ATTR_protocol, AAA_VAL_PROT_ip,
		    AAA_AV_MAND))
	ret = FALSE;

    if (ret && addr) {
	sprintf(buf, "%i", addr);
	if (!aaa_add_AV(&a.out_args, &avail, AAA_ATTR_addr, buf, AAA_AV_OPT))
	    ret = FALSE;
    }

    if (ret) {
	/* Second step.  Ask AAA. */
	ret = aaa_do_author(idb->lcpstate->ustruct, &a, AAA_AUTHOR_TYPE_NET, 0);

	/* Third step.  Deal with response. */
	switch(a.status) {
	case AAA_AUTHOR_STATUS_ERROR:
	case AAA_AUTHOR_STATUS_FAIL:
	    ret = FALSE;
	    break;
        case AAA_AUTHOR_STATUS_PASS_ADD:
	    aaa_clear_attrs(attrs);
	    if (!a.in_args) {
		*attrs = aaa_copy_attrs(a.out_args);
	    } else {
		aaa_merge_attrs(a.out_args, a.in_args, attrs);
	    }
	    break;
	case AAA_AUTHOR_STATUS_PASS_REPL:
	    aaa_clear_attrs(attrs);
	    *attrs = aaa_copy_attrs(a.in_args);
	    break;
	}
    }
    aaa_cleanup_author(&a);
    return(ret);
}

/* set the routing flag in the IDB from Tacacs+. Return FALSE if
 * authorization should be denied because of this request.
 */
static boolean
ipcp_set_routing_attr (hwidbtype *idb, enum AV_TYPE type, char *val)
{
    tt_soc *tty = idb->idb_tty;    
    idbtype *swidb = firstsw_or_null(idb);

    if (!swidb) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/IPCP: routing denied-- no idb");
	}
	return(FALSE);
    }

    if ((type == AAA_AV_MAND) &&
	tty &&
	!(tty->slip_flags & (SLIP_DYN_ROUTING|SLIP_DEF_ROUTING))) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/IPCP: %s: routing denied"
		   " -- tty not configured to route",
		   idb->hw_namestring);
	}
	return(FALSE);
    }
    
    if (!strcmp(val, AAA_VAL_false)) {

	if (!tty) { 
	    /* this is not an async line */
	    if (type == AAA_AV_MAND) {
		if (aaa_author_debug) {
		    buginf("\nAAA/AUTHOR/IPCP: %s: cannot turn off routing"
			   " for sync lines", idb->hw_namestring);
		}
		return(FALSE);
	    } else {
		/* ignore optional request for sync line */
		return(TRUE);
	    }
	}

	swidb->static_routing = TRUE;
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/IPCP: set routing to FALSE");
	}
	return(TRUE);
    }

    if (!strcmp(val, AAA_VAL_true)) {
	swidb->static_routing = FALSE;
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/IPCP: %s: set routing to TRUE",
		   idb->hw_namestring);
	}
	return(TRUE);
    }

    if (aaa_author_debug) {
	buginf("\nAAA/AUTHOR/IPCP: %s: illegal value %s for routing attr",
	       idb->hw_namestring, val);
    }
    return(FALSE);
}

/* Walk through the AV pairs, processing them */

static boolean
ipcp_process_author_params (hwidbtype *idb, char **args)
{
    int i;
    boolean ret = TRUE;
    boolean mand_addr = FALSE, mand_pool = FALSE;
    char *attr, *val;
    enum AV_TYPE type;
    hwidbtype *netidb;
    idbtype *swidb;
    int list;
    char *route_val = NULL; /* set only if there is a route attribute */
    enum AV_TYPE route_type = AAA_AV_MAND; /* keep gcc quiet: use default */

    if (!args) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/IPCP: %s: No AV pairs", 
		   idb->hw_namestring);
	}
    }

    for (i = 0; ret && args && args[i]; i++) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/IPCP: %s: Processing AV %s", 
		   idb->hw_namestring, args[i]);
	}
	if (aaa_split_AV(args[i], &attr, &val, &type)) {
	    if (!strcmp(attr, AAA_ATTR_service)) {
		if (strcmp(val, AAA_VAL_SVC_ppp)) {
		    if (type == AAA_AV_MAND)
			ret = FALSE;
		    else
			if (aaa_author_debug)
			    buginf("\nAAA/AUTHOR/IPCP: %s: "
				   "tried to change \"service\".  Ignoring.",
				   idb->hw_namestring);
		}
		continue;
	    }
	    if (!strcmp(attr, AAA_ATTR_protocol)) {
		if (strcmp(val, AAA_VAL_PROT_ip)) {
		    if (type == AAA_AV_MAND)
			ret = FALSE;
		    else
			if (aaa_author_debug)
			    buginf("\nAAA/AUTHOR/IPCP: %s: "
				   "tried to change \"protocol\".  Ignoring.",
				   idb->hw_namestring);
		}
		continue;
	    }

 	    if (!strcmp(attr, AAA_ATTR_old_prompts)) {
 		continue;	/* processed in ts/if_slip.c */
 	    }
	    if (!strcmp(attr, AAA_ATTR_addr)) {
		mand_addr = (type == AAA_AV_MAND);
		continue;	/* Dealt with this elsewhere */
	    }
	    if (!strcmp(attr, AAA_ATTR_addr_pool)) {
		mand_pool = (type == AAA_AV_MAND);
		continue;	/* Dealt with this elsewhere */
	    }
	    if (!strcmp(attr, AAA_ATTR_inacl)) {
		list = atoi(val);
		if ( !list || !access_find_acl(list) ) {
		    if (aaa_author_debug) {
			buginf("\nAAA/AUTHOR/IPCP: bad inacl access list "
			       "value <%s>", val);
		    }
		    if (type == AAA_AV_MAND) {
			ret = FALSE;
		    }
		} else {
		    netidb = reg_invoke_dialer_pool_acl_idb(idb);
		    FOR_ALL_SWIDBS_ON_HW(netidb, swidb) {
			reg_invoke_ip_set_per_user_acl(swidb, IN, list);
		    }
		    if (aaa_author_debug) {
			buginf("\nAAA/AUTHOR/IPCP: %s: install inacl %d on %s",
			    idb->hw_namestring, list, netidb->hw_namestring);
		    }
		}
		continue;
	    }
	    if (!strcmp(attr, AAA_ATTR_outacl)) {
		list = atoi(val);
		if ( !list || !access_find_acl(list) ) {
		    if (aaa_author_debug) {
			buginf("\nAAA/AUTHOR/IPCP: bad outacl access list "
			       "value <%s>", val);
		    }
		    if (type == AAA_AV_MAND) {
			ret = FALSE;
		    }
		} else {
		    netidb = reg_invoke_dialer_pool_acl_idb(idb);
		    FOR_ALL_SWIDBS_ON_HW(netidb, swidb) {
			reg_invoke_ip_set_per_user_acl(swidb, OUT, list);
		    }
		    if (aaa_author_debug) {
			buginf("\nAAA/AUTHOR/IPCP: %s: install outacl %d on "
			    "%s", idb->hw_namestring, list,
			    netidb->hw_namestring);
		    }
		}
		continue;
	    }

	    /* Do we allow routing updates on this interface? */
	    if (!strcmp(attr, AAA_ATTR_routing)) {
		ret = ipcp_set_routing_attr(idb, type, val);
		continue;
	    }

	    if (!strcmp(attr, AAA_ATTR_route)) {
		/*
		 * we'll only install this route if authorization
		 * succeeds 
		 */
		route_type = type;
		route_val = val;
		continue;
	    }

	    /* if unknown and mandatory, then bail */
	    if (type == AAA_AV_MAND) {
		if (aaa_author_debug)
		    buginf("\n%% AAA/AUTHOR/IPCP: %s: unknown mandatory "
			   "attribute %s denied", 
			   idb->hw_namestring, attr);
		ret = FALSE;
	    }
	}
    }

    if (mand_addr && mand_pool) {
	ret = FALSE;
	if (aaa_author_debug)
	    buginf("\n%% AAA/AUTHOR/IPCP: %s: attributes "
		   "addr and addr-pool are mutually exclusive",
		   idb->hw_namestring);
    }

    if (ret && route_val) {
	/* A successful authorization. Store a route to be installed
	 * by ipcp_up.
	 */
	ret = store_per_user_ip_route(idb, route_type, route_val);
    }

    if (aaa_author_debug) {
	buginf("\nAAA/AUTHOR/IPCP: %s: authorization %s", 
	       idb->hw_namestring,
	       ret ? "succeeded" : "denied");
    }

    return(ret);
}


/* Parse and store the value of a "route" attribute into an idb. The
 * attribute is a string of the form:
 *
 *	 <network> <mask> [ <gateway> ]
 *
 * If gateway is omitted, it means use the current peer address as the
 * gateway. Since the peer may not be known yet, or may change during
 * negotiation, we store this information in the idb and actually
 * apply the route in ipcp_up.
 *
 * Return FALSE if authorization should be denied.
 *
 * This routine is also called from slip_do_author() to handle SLIP
 * routes.
 * 
 * For PPP we are called from ipcp_authorize_addr which stores values
 * to be processed later by ipcp_up. 
 */
static boolean
store_per_user_ip_route (hwidbtype *idb, enum AV_TYPE type, 
				 char *val)
{
    idbtype *swidb = firstsw_or_null(idb);
    int offset = 0;
    ipaddrtype network, gateway, mask;
    addrtype addr;
    const char *encstr = encstring(idb->enctype);
    per_user_info *info;

    if (!swidb) {
	return(FALSE);
    }
    
    if (!router_enable)	{
	/* not a router */
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/%s: route=%s -- NAS is not a router",
		   encstr, val);
	}
	return(type == AAA_AV_MAND ? FALSE : TRUE);
    }

    match_whitespace2(val, &offset, FALSE);

    /* Lol -- consider setting ADDR_NAME_LOOKUP in flags field */
    if (!match_ip_addr(NULL, val, &offset, &addr, 0, 0, NULL)) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/%s: invalid dst_address in route=%s",
		   encstr, val);
	    return(type == AAA_AV_MAND ? FALSE : TRUE);
	}
    }

    network = addr.addr.ip_address;

    if (CLASSD_AND_GREATER(network)) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/%s: route=%s Invalid network number", 
		   encstr, val);
	}
	return(type == AAA_AV_MAND ? FALSE : TRUE);
    }

    match_whitespace2(val, &offset, FALSE);

    if (!match_ip_addr(NULL, val, &offset, &addr, 0, 0, NULL)) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/%s: invalid mask in route=%s", 
		   encstr, val);
	    return(type == AAA_AV_MAND ? FALSE : TRUE);
	}
    }
    mask = addr.addr.ip_address;

    /*
     * Sanity check mask and address.
     */
    if (!good_route_mask(network, mask, subnetzero_enable)) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/%s: route=%s Inconsistent address and mask",
		   encstr, val);
	}
	return(type == AAA_AV_MAND ? FALSE : TRUE);
    }
    
    match_whitespace2(val, &offset, FALSE);

    /* Assume no gateway specified. */
    gateway = IPADDR_ZERO;

    /* See if we have a gateway? */
    if (val[offset]) {
	if (!match_ip_addr(NULL, val, &offset, &addr, 0, 0, NULL)) {
	    if (aaa_author_debug) {
		buginf("\nAAA/AUTHOR/%s: invalid gateway in route=%s",
		       encstr, val);
	    }
	    return(type == AAA_AV_MAND ? FALSE : TRUE);
	}
	
	/* Successfully parsed a gateway. Check it */
	gateway = addr.addr.ip_address;
	if ((gateway != 0L) && martian(gateway, FALSE)) {
	    if (aaa_author_debug) {
		buginf("\nAAA/AUTHOR/%s route=%s Invalid next hop address",
		       encstr, val);  
	    }
	    return(type == AAA_AV_MAND ? FALSE : TRUE);
	}

	/*
	 * Make sure we don't add a static route whose first hop is one of
	 * our addresses.
	 */
	if (ip_ouraddress(gateway)) {
	    if (aaa_author_debug) {
		buginf("\nAAA/AUTHOR/%s: route=%s First hop is ourself",
		       encstr, val);
	    }
	    return(type == AAA_AV_MAND ? FALSE : TRUE);
	}
    }

    create_per_user_info(swidb);
    info = swidb->per_user_info;
    if (!info) {
	return(FALSE);
    }
    info->network = network;
    info->mask = mask;
    info->gateway = gateway;
    return(TRUE);
}

static boolean ppp_ip_protocheck (hwidbtype *hwidb,
				  hwidbtype *ignore,
				  int linktype)
{
    /* Don't open NCP for ip if we want to psuedo-bridge it */
    if (hwidb->ppp_bridge_ip)
	return(FALSE);
    return(reg_invoke_proto_on_swidb(linktype, hwidb->firstsw));
}

/*
 * ipcp_init_subsys
 * Initialization of IPCP subsystem
 */

static void ipcp_init_subsys (subsystype *subsys)
{
    reg_add_hwif_state_change(ipcp_net_cstate, "ipcp_net_cstate");
    reg_add_ipcp_add_p2p_neighbor(ipcp_add_p2p_neighbor,
				  "ipcp_add_p2p_neighbor");
    reg_add_ipcp_cleanup_ip_peer(ipcp_cleanup_ip_peer,
				 "ipcp_cleanup_ip_peer");
    reg_add_store_per_user_ip_route(store_per_user_ip_route,
				    "store_per_user_ip_route");
    reg_add_install_per_user_route(install_per_user_route,
				    "install_per_user_route");
    reg_add_author_remove_per_user(ipcp_remove_per_user_access_lists,
				    "ipcp_remove_per_user_access_lists");
    reg_invoke_ncp_register(LINK_IP, TYPE_PPP_IPCP,
			    ipcp_init, ppp_ip_protocheck, "IPCP");
    reg_invoke_ncp_register_link(LINK_ARP, 0, LINK_IP);
    reg_invoke_ncp_register_link(LINK_COMPRESSED_TCP, TYPE_PPP_VJCOMP,
				 LINK_IP);
    reg_invoke_ncp_register_link(LINK_UNCOMPRESSED_TCP, TYPE_PPP_VJUNCOMP,
				 LINK_IP);
}

/*
 * The IPCP subsystem header
 */

#define IPCP_MAJVERSION 1
#define IPCP_MINVERSION 0
#define IPCP_EDITVERSION  1

SUBSYS_HEADER(ipcp, IPCP_MAJVERSION, IPCP_MINVERSION, IPCP_EDITVERSION,
	      ipcp_init_subsys, SUBSYS_CLASS_PROTOCOL,
	      "seq: ppp",
	      "req: iphost, ppp");
