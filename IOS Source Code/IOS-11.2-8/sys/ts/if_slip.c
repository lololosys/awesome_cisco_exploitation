/* $Id: if_slip.c,v 3.24.4.22 1996/09/10 03:22:50 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ts/if_slip.c,v $
 *------------------------------------------------------------------
 * if_slip.c -- Berkeley compatible Serial Line Interface "Protocol"
 *
 * September 1987,  Bill Westfield
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_slip.c,v $
 * Revision 3.24.4.22  1996/09/10  03:22:50  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.24.4.21  1996/08/28  13:18:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.24.4.20  1996/08/14  06:25:48  irfan
 * CSCdi65960:  slip line command should be removed from IOS
 * Branch: California_branch
 * Remove code for obsolete UI.
 *
 * Revision 3.24.4.19  1996/08/07  09:03:12  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.24.4.18  1996/07/17  19:11:27  sjackson
 * CSCdi49571:  AAA user name is not passed in login get address requests
 * User name determined from the authentication method in use
 * Branch: California_branch
 *
 * Revision 3.24.4.17  1996/07/11  19:24:05  sjackson
 * CSCdi60440:  Unable to Allocate IP Address From Local (Non-Default) Pool
 * Branch: California_branch
 * Configuration IDB containing pool information determined from physical
 * interface when required
 *
 * Revision 3.24.4.16  1996/06/27  09:42:47  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.24.4.15  1996/06/18  01:49:46  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.24.4.14  1996/06/17  08:36:08  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.24.4.13  1996/06/06  20:58:53  irfan
 * CSCdi57344:  Line stuck in carrier dropped, funny modem state 10600, no
 * process
 * Branch: California_branch
 * Ensure proper hangup/cleanup if CARDROPs in interface startup.
 *
 * Revision 3.24.4.12  1996/05/23  07:38:30  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.24.4.11  1996/05/22  17:20:52  fox
 * CSCdi57795:  Unable to change a BRI interface from ppp to hdlc.
 * Branch: California_branch
 * Don't clear idb->enctype in ppp_deencap(), slip_deencap() or
 * bstun_deencapsulate().  Move the setting of idb->encaptype to
 * the start of the encapsulation setup functions because calls to
 * other function use this value.
 *
 * Revision 3.24.4.10  1996/05/09  14:47:40  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.24.4.9  1996/05/04  02:08:26  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.24.4.8  1996/04/29  09:42:15  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.24.4.7  1996/04/27  06:28:27  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.24.4.6.10.1  1996/04/27  07:37:41  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.24.4.6  1996/04/04  09:39:15  irfan
 * CSCdi53185:  crash when changing encaps on group async interfaces
 * Branch: California_branch
 * most tty operations don't work on dummy tty's pointed to by
 * the async-group hwidb.
 *
 * Revision 3.24.4.5  1996/04/04  06:17:20  fox
 * CSCdi53389:  SLIP address pooling doesnt use dialer configs
 * Branch: California_branch
 * Look up pool-name from top-level interface
 *
 * Revision 3.24.4.4.4.1  1996/04/08  02:10:31  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.24.4.4  1996/03/27  18:26:22  sjackson
 * CSCdi41921:  sh async statistics is inaccurate
 * Branch: California_branch
 *
 * Revision 3.24.4.3  1996/03/27  08:33:43  billw
 * CSCdi49184:  AAA accounting records do not contain IP addr if
 * autoselected
 * Branch: California_branch
 *
 * Revision 3.24.4.2  1996/03/23  01:34:15  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.24.4.1  1996/03/18  22:19:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.24.6.1  1996/03/22  09:44:27  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.24  1996/03/07  12:10:11  billw
 * CSCdi46565:  <uses> in  <show line> doesnt include PPP, Xremote, etc
 *
 * Revision 3.23  1996/03/05  01:21:35  billw
 * CSCdi49461:  autoselect during-login causes premature authorization
 * Additional commenting...
 *
 * Revision 3.22  1996/02/23  10:38:18  billw
 * CSCdi49461:  autoselect during-login causes premature authorization
 * Define a new flag AUTOSELECT_AUTHORIZE that survives across death().
 * Set it at the same time AUTOSELECTED is set, but only clear it when
 * a new autoselect starts.
 *
 * Revision 3.21.6.1  1996/03/05  05:37:02  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.21.2.1  1996/02/27  21:19:55  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.21  1996/02/10  08:47:07  billw
 * CSCdi48051:  duplicate aaa accounting entries sent for interactive PPP
 * startup
 *
 * Revision 3.20  1996/02/09  09:03:17  billw
 * CSCdi43680:  RADIUS doesnt support magic number ip address values
 * also add authstruct->auth_idb, and change auth->tty to auth->auth_tty
 *
 * Revision 3.19  1996/02/07  16:16:30  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.18  1996/02/01  06:11:05  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.17  1996/01/22  07:29:54  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.16  1996/01/19  18:45:33  fox
 * CSCdi47170:  Peer IP address from TACACS+ ignored at SLIP/PPP startup
 * Accept peer IP address from TACACS+ unless address is bogus
 * Correct detection of mandatory pool and address conflict
 *
 * Revision 3.15  1996/01/17  17:58:41  fox
 * CSCdi47013:  no async dynamic address blocks TACACS+ providing an
 * address
 * Remove checks for SLIP_DYNAMIC from slip_do_author()
 *
 * Revision 3.14  1996/01/16  03:59:47  fox
 * CSCdi46898:  New IP address from AAA ignored when ppp/slip started from
 * exec
 * Don't set the peer address until TACACS+ has spoken.
 *
 * Revision 3.13  1996/01/16  00:41:11  irfan
 * CSCdi46312:  Routing Protocol broadcast not sent to backup Async/PPP
 * interfaces
 * Leave Async interfaces as routing interfaces until the time they
 * are being brought up. At which time, we may mark them as
 * static_routing. Allows DDR to kick up the interface to send a
 * routing protocol update on an UP/UP Spoofing interface in async
 * backup configurations
 *
 * Revision 3.12  1996/01/10  23:31:20  irfan
 * CSCdi46676:  async interfaces dont respect admin-down
 * Don't put async line in protocol framing mode if admin-down'ed.
 *
 * Revision 3.11.2.1  1996/02/15  19:21:21  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.11  1996/01/04  22:15:12  lol
 * CSCdi43469:  per-user route wont use default interface
 *
 * Revision 3.10  1995/12/18  23:49:28  lol
 * CSCdi43005:  route attribute is not processed for sync ISDN
 *
 * Revision 3.9  1995/12/17  18:39:50  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.8  1995/12/13  22:58:03  lol
 * CSCdi39791:  access-lists applied by tacacs+ dont work when
 * fast-switched
 *
 * Revision 3.7  1995/12/09  00:44:00  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Make it easy to claim a tty for exclusive use by a process, and add a
 * modem_busyout registry.
 *
 * Revision 3.6  1995/11/29  00:16:22  lol
 * CSCdi42385:  TACACS+ crashes router in aaa_split_AV
 *
 * Revision 3.5  1995/11/18  17:43:17  fox
 * CSCdi43045:  ppp and slip exec-level commands are broken
 * When checking for a duplicate peer address, don't check our own
 * interface.
 *
 * Revision 3.4  1995/11/18  03:29:13  pst
 * CSCdi43467:  unumbered interfaces dont pick up changes based upon
 * numbered change
 *
 * Revision 3.3  1995/11/17  18:55:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:45:15  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:39:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.21  1995/11/08  21:33:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.20  1995/10/23  21:00:54  fox
 * CSCdi42602:  Neighbor routes are lost if routing table is cleared
 * Move neighbor address support to the WAN code.
 * Call the WAN code if an interface's routes are being modified.
 *
 * Revision 2.19  1995/10/02  20:43:05  lol
 * CSCdi36244:  Console line failing authorization should be more forgiving
 *              Authenticated console users now bypass authorization.
 *
 * Revision 2.18  1995/09/20  00:06:24  eschaffe
 * CSCdi40229:  ppp crashed on two step pt (regress)
 *
 * Revision 2.17  1995/09/18  19:38:54  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.16  1995/09/09  00:53:55  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.15  1995/09/07  09:35:06  irfan
 * CSCdi39879:  memory leaking as AAA AV pairs from PAP/CHAP threads
 * ensure we bzero the correct size.
 *
 * Revision 2.14  1995/08/31  19:48:54  lol
 * CSCdi38919:  tacacs+ ip pooling not working
 *
 * Revision 2.13  1995/08/29  06:06:14  fox
 * CSCdi38536:  peer default ip address pool command does not match
 * documentation
 * Commit changes to mainline in order to match 11.0 Release
 * documentation.
 *
 * Revision 2.12  1995/08/25  23:22:06  eschaffe
 * CSCdi39088:  No connection messages after SLIP routing issued.
 *
 * Revision 2.11  1995/08/08  20:50:36  lol
 * CSCdi37361:  AAA should not require auth for originating PPP connections
 *
 * Revision 2.10  1995/08/07  21:52:34  lol
 * CSCdi36743:  async PPP fails to connect with /routing and tacacs+
 * authorization
 *
 * Revision 2.9  1995/08/04  07:38:15  irfan
 * CSCdi38094:  ip routing disabled on async/ppp
 * Include interfaces dialing-out when turning off static routing flag.
 *
 * Revision 2.8  1995/07/28  23:19:48  syiu
 * CSCdi37843:  Fair Queuing with async fastswitch crashes box
 *
 * Revision 2.7  1995/06/30  10:10:31  irfan
 * CSCdi36611:  cleanup of sanity checks for remote SLIP/PPP parameters
 * Don't allow SLIP w/o required IP configuration. Same for PPP, but
 * don't block possible IPX or ATALK PPP clients.
 *
 * Revision 2.6  1995/06/22  03:01:46  tkolar
 * CSCdi36279:  DHCP breaks ATCP
 * Allow the normal PPP code to make the decision on rather to run or not.
 *
 * Revision 2.5  1995/06/20  04:14:44  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.  (Should have been in initial commit,
 * but something got screwed up.)
 *
 * Revision 2.4  1995/06/16  19:34:39  irfan
 * CSCdi31975:  Async default routing should work for dedicated mode
 * use "async default routing" flag to control routing on the dedicated
 * mode async interfaces. Further, Async interfaces were routing
 * all the time...fix it.
 *
 * Revision 2.3  1995/06/15  03:42:40  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 * Revision 2.2  1995/06/09  13:22:00  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:09:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdlib.h>
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "../wan/dialer_registry.h"
#include "packet.h"
#include "logger.h"
#include "../if/msg_slip.c"		/* Not a typo, see logger.h */
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include "address.h"
#include "aaa.h"
#include "login.h"
#include "parser_defs_slip.h"
#include "access.h"
#include "../parser/parser_defs_autoselect.h"
#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "../if/ether.h"
#include "../if/network.h"
#include "../os/async.h"
#include "../ts/if_slip.h"
#include "../ts/if_async.h"
#include "../tcp/tcpcompress.h"
#include "../iprouting/route.h"
#include "../wan/ppp.h"
#include "async_ppp.h"
#include "../wan/dialer.h"
#include "../ip/ip_registry.h"
#include "../h/cs_registry.regh"
#include "../ui/service.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../ip/ip_addrpool.h"
#include "../ip/ip_macros.h"
#include "../ui/command.h"
#include "../wan/ppp_registry.h"

/*
 * Local storage - THis next item needs to be moved out of network.c,
 * but there are two extraneous references there causing problems.

slip_traffic_t slip_traffic;
 */

/*
 * Forward references
 */
static boolean slip_do_author(tt_soc *tty, hwidbtype *hwidb, encapstype enctype,
			      ipaddrtype *addr, boolean *routing, char **,
			      boolean *old_prompts);
static const char badslip[] = "\n%%Bad slip command - %s";
static long slip_getlink(paktype *);


/*
 * async_protocols_mode
 * Check to see if we are required to go into one of the ip or ipx mode
 * For IP,  SLIP and PPP are the two encap type for tty's idb.
 * For IPX, PPP is the only encap type for tty's idb.
 *
 */
boolean async_protocols_mode (tt_soc *tty, addrtype *proto_addr)
{
    dialerdbtype *ddb;
    boolean  success = FALSE;

    if (tty && tty->tty_idb && tty->tty_idb->firstsw && proto_addr) {

        hwidbtype *idb = tty->tty_idb;
	idbtype *swidb = idb->firstsw;

	switch (proto_addr->type) {

	case ADDR_ILLEGAL:       /* The case when a peer addr of 0 */
	    break;					 /* was passed */

	case ADDR_IP:            /* Allow IP for SLIP or PPP encap only */
	    if ((ET_SLIP == idb->enctype) || (ET_PPP == idb->enctype))
		break;

	case ADDR_NOVELL:        /* IPX works with PPP only */
	    if (ET_PPP == idb->enctype)
		break;
	default:                 /* Apparently Unsupported protocol */
	    if (async_state_debug)
		buginf("\n%s: Async encapsulation not supported for"
		       " protocol", idb->hw_namestring);
	    return FALSE;
	}

	reg_invoke_set_ip_peer_addr(swidb, proto_addr->ip_addr,
				    PEER_ADDR_FROM_CHAT);
	success = async_proto_mode(tty);
	ddb = get_dialerdb(idb);
	if (success && (ddb != NULL)) {
	    /* Fire dialer_wait_for_carrier timer for dialer idb */
	    TIMER_START(ddb->dialer_carrier_timer, ddb->dialer_carrier_ticks);
	}
    }
    return success;
}

/*
 * async_proto_mode
 * Turn async protocol on for a line.
 */

boolean async_proto_mode (tt_soc *tty)
{
    hwidbtype *idb;
    idbtype *swidb;
    ippeerinfotype *ip_peer;
    dialerdbtype *ddb;

    idb = tty->tty_idb;
    swidb = firstsw_or_null(idb);

    /*
     *   Valid Async IDB ?
     */
    if (!(idb && swidb && ((IDBTYPE_ASYNC == idb->type) ||
			   (IDBTYPE_VTY_ASYNC == idb->type)))) {
	return (FALSE);
    }

    if (tty->statbits & SLIPMODE) {
	if (async_state_debug)
	    buginf("\n%s: Protocol Mode: not restarting...already active",
		   idb->hw_namestring);
	return(TRUE);	/* Don't start if already turned on */
    }

    if (IDBS_ADMINDOWN == idb->state) {
	if (async_state_debug)
	    buginf("\n%s: Async interface is administratively down. "
		   "Not starting", idb->hw_namestring);
	return (FALSE);
    }

    /*
     * Turn on interrupt level framing on the line
     */
    if (!async_framing_setup(tty))
	return (FALSE);

    CLAIM_TTY(tty);
    tty->statbits |= SLIPMODE;

    ip_peer = swidb->ip_peer;
    /*
     * Tell IP about neighbor
     */
    if (ip_peer)
	reg_invoke_ipcp_add_p2p_neighbor(swidb);

    /*
     * Let any dialer abstraction take a shot at filling in the number
     * of the caller if we know the other side's IP address.
     */
    if (ip_peer && ip_peer->ippeer_addr) {
	addrtype proto_addr;

	memset(&proto_addr, 0, sizeof(proto_addr));
	proto_addr.ip_addr = ip_peer->ippeer_addr;
	proto_addr.type = ADDR_IP;
	proto_addr.length = ADDRLEN_IP;
	reg_invoke_dialer_lookup_caller(idb, &proto_addr, LINK_IP);
    }

    /*
     * If async_default_routing flag is not set then we
     * want to turn off routing on this interface if it is
     * being brought UP because of ddr dial-out or
     * because of its dedicated mode.
     */
    ddb = get_dialerdb(idb);
    if ((!(tty->slip_flags & SLIP_DEF_ROUTING)) &&   /* No Def_routing ? */
	((tty->capabilities & MUSTSLIP) ||            /* dedicated ? */
	 ((ddb != NULL) &&                            /* dial-out ? */
          (ddb->dialer == DIALER_INBAND_ASYNC) && 
	  ddb->dialer_placed_call))) {

	swidb->static_routing = TRUE;                 /* routing = OFF */
    }

    /*
     * It is possible that some other event has caused (idb->reset) to
     * get called within the last "pulse-interval" seconds. If this
     * has happened, then there will be a timer which will go off and
     * clear this interface out from under us. Prevent this by 
     * clearing the timer here.
     */

    TIMER_STOP(idb->serial_pulsetimer); /* Stop any possible delayed reset */

    /*
     * If interface isn't intentionally down, bring it up and do the
     * appropriate magic to routing protocols/etc...
     */
    if (idb->state != IDBS_ADMINDOWN) 
	net_cstate(idb, IDBS_UP);

    TIMER_START(tty->dallytimer, tty->tty_sessiontimeout);

    if (async_state_debug && ip_peer)
      buginf("\n%s: Async protocol mode started for %i",
	     idb->hw_namestring, ip_peer->ippeer_addr);

    return(TRUE);
}

/*
 * slip_setup - Set up SLIP context
 */

boolean slip_setup (tt_soc *tty)
{
    hwidbtype *idb;

    idb = tty->tty_idb;

    /*
     * Re-initialize TCP header compression
     */

    if (idb->tcp_compress) {
	thc_compress_reset(idb->tcp_compress, 0, 0);
	if (idb->tcp_compress->flags & THC_SESSION)
	  idb->tcp_compress->flags |= THC_ACTIVE;
    }
    tty->slip_istate = SLIP_RXIDLE;
    tty->slip_ostate = SLIP_TXIDLE;
    if (!tty_servicehook(tty, slip_sndchar, slip_gotchar)) {
	if (async_framing_debug || async_state_debug)
	    buginf("\n%s: Unable to set SLIP service hook on TTY%t",
		   idb->hw_namestring, tty->ttynum);
	printf(nomemory);
	return(FALSE);
    }

    slip_suspend_swflow(tty);     /* Suspend sw flow-c on physical line */
    return(TRUE);
}

/*
 * slip_disable - Remove SLIP interrupt handler
 */

void slip_disable (tt_soc *tty)
{
    /*
     * If fast switching is on, obsolete the route cache
     */
    if ((tty->tty_idb) && (tty->tty_idb->ip_fastokay)) 
	reg_invoke_bump_cache_version(NULL, NULL, TRUE);

    slip_restore_swflow(tty);     /* Restore sw flow-c if we suspended */
    tty_noservicehook(tty);
    tty->slip_istate = SLIP_RXIDLE;
    tty->slip_ostate = SLIP_TXIDLE;
    if (tty->slip_inpak)
	retbuffer((paktype *)tty->slip_inpak);
    if (tty->slip_outpak)
	datagram_done((paktype *)tty->slip_outpak);
    tty->slip_outpak = NULL;
    tty->slip_inpak = NULL;
}

static const char slip_toolong[] = "\nSLIP%t: packet too long";

/*
 * slip_gotchar
 * Process input characters for a terminal line running slip.
 */
void slip_gotchar (tt_soc *tty, uchar *p, int len)
{
    leveltype status;
    paktype *pak;
    hwidbtype *idb;
    uchar c;
    
    idb = tty->tty_idb;
    pak = tty->slip_inpak;
    while (len-- > 0) {
	c = *p++;
	switch (tty->slip_istate) {
	case SLIP_RXIDLE:
	    if (c != SLIP_FRAME_END) {
		if (async_framing_debug)
		    buginf("\nSLIP%t: spurious %x.", tty->ttynum, c);
		break;		/* ignore spurious characters */
	    }
	    /*
	     * We've seen an end-of-packet come in, so get ready for
	     * the next packet.  Count any data that we've seen up till
	     * now as an error.  Note that a failure to get a buffer is
	     * not an error until we actually have a byte to put in the
	     * next packet (and can't.)
	     */
	    if (pak == NULL) {
		pak = input_getbuffer(idb->maxmtu, idb);
		if (pak) {
		    pak->datagramsize = 0;
		    pak->dataptr = ipheadstart(pak);
		    tty->slip_inpak = pak;
		}
	    }
	    tty->slip_istate = SLIP_RECEIVE;
	    idb->counters.input_frame++;
	    idb->counters.input_error++;
	    slip_traffic.fmterrors++;
	    break;

	case SLIP_RECEIVE:
	    switch (c) {
	    case SLIP_FRAME_ESC:
		tty->slip_istate = SLIP_RXESCAPE;
		break;
	    case SLIP_FRAME_END:
		if (!pak || !pak->datagramsize) {
		    break;
		}
		pak->datagramstart = pak->network_start;
		pak->enctype = ET_SLIP;
		pak->encsize = 0;

		if (async_packet_debug) {
		  buginf("\nSLIP%t: Input packet, len %d, "
			 "header = %x %x %x %x",
			 tty->ttynum,
			 pak->datagramsize,
			 pak->datagramstart[0],
			 pak->datagramstart[1],
			 pak->datagramstart[2],
			 pak->datagramstart[3]);
		}

		status = raise_interrupt_level(NETS_DISABLE);
		slip_traffic.inpacks++;
		slip_traffic.inbytes += pak->datagramsize;

		/*
		 * If interface is not fastswitch enabled, process
		 * switch it. Otherwise, try to fastswitch it (IP only)
		 * If the fastswitch fails, process switch it.    
		 */
		if ( (!idb->ip_fastokay) || 
		     (!reg_invoke_slip_fs(RXTYPE_DODIP, pak))){
                   idb->counters.rx_cumbytes += pak->datagramsize;
                   idb->counters.inputs++;
		   (*(idb->iqueue)) (idb, pak);
		} else {
		    /*
		     * Set linktype for packet classification if the packet
		     * was successfully fastswitched. (it's still around)
		     */		       
		    pak->linktype = slip_getlink(pak);
		}
		/*
		 * Check if this affects idle time AFTER iqueue has set
		 * pak->linktype appropriately.  In theory, the packet is
		 * GONE, but in reality it's still on some queue somewhere.
		 * (even if the packet was fastswitched)
		 */
		if (dialer_async_classify(idb, pak)) {
		    TIMER_START(tty->dallytimer, tty->tty_sessiontimeout);
		}
		reset_interrupt_level(status);

		pak = input_getbuffer(idb->maxmtu, idb);
		tty->slip_inpak = pak;
		if (pak) {
		    pak->datagramsize = 0;
		    pak->dataptr = (char *)ipheadstart(pak);
		}
		break;
	    default:
		if (pak == NULL) {
		    pak = input_getbuffer(idb->maxmtu, idb);
		    if (pak) {
			pak->datagramsize = 0;
			pak->dataptr = ipheadstart(pak);
			tty->slip_inpak = pak;
		    } else {
			tty->slip_istate = SLIP_RXERROR;
			idb->counters.input_error++;
			++slip_traffic.nobuf;
			return;
		    }
		}
		*pak->dataptr++ = c;
		if (++pak->datagramsize > idb->maxmtu) {
		    tty->slip_istate = SLIP_RXERROR;
		    idb->counters.giants++;
		    idb->counters.input_error++;
		    slip_traffic.badlen++;
		    if (async_framing_debug)
			buginf(slip_toolong,tty->ttynum);
		}
		break;
	    }
	    break;
	case SLIP_RXESCAPE:
	    if (pak == NULL) {
		pak = input_getbuffer(idb->maxmtu, idb);
		if (pak) {
		    pak->datagramsize = 0;
		    pak->dataptr = ipheadstart(pak);
		    tty->slip_inpak = pak;
		} else {
		    tty->slip_istate = SLIP_RXERROR;
		    idb->counters.input_error++;
		    ++slip_traffic.nobuf;
		    return;
		}
	    }
	    tty->slip_istate = SLIP_RECEIVE;
	    if (c == SLIP_TRANS_END) {
		*pak->dataptr++ = SLIP_FRAME_END;
		if (++pak->datagramsize > idb->maxmtu) {
		    tty->slip_istate = SLIP_RXERROR;
		    ++slip_traffic.badlen;
		    idb->counters.giants++;
		    idb->counters.input_error++;
		    if (async_framing_debug)
			buginf(slip_toolong,tty->ttynum);
		}
		break;
	    }
	    else if (c == SLIP_TRANS_ESC) {
		*pak->dataptr++ = SLIP_FRAME_ESC;
		if (++pak->datagramsize > idb->maxmtu) {
		    tty->slip_istate = SLIP_RXERROR;
		    ++slip_traffic.badlen;
		    idb->counters.giants++;
		    idb->counters.input_error++;
		    if (async_framing_debug)
			buginf(slip_toolong,tty->ttynum);
		}
		break;
	    }
	    else if (c != SLIP_FRAME_END)
		break;
	    tty->slip_istate = SLIP_RXERROR;
	    /* Fall through */
	case SLIP_RXERROR:
	    if (c == SLIP_FRAME_END) {
		/*
		 * We've gotten to the end of a packet that we couldn't
		 * receive for one reason or another.  Set up to receive
		 * the next packet (if we can.)
		 */
		if (pak == NULL) {
		    pak = input_getbuffer(idb->maxmtu, idb);
		    if (pak) {
			tty->slip_inpak = pak;
		    }
		}
		if (pak) {
		    pak->datagramsize = 0;
		    pak->dataptr = (char *)ipheadstart(pak);
		}
		tty->slip_istate = SLIP_RECEIVE;
		/*
		 * Also, if we are doing tcp header compression, inform
		 * that code that we know a link-level packet has been dropped
		 */
		MAYBE_DROPPED(idb);
	    }
	    break;
	default:
	    errmsg(&msgsym(BADSTATE, SLIP), tty->slip_istate);
	} /* switch */
    }
}

/*
 * slip_sndchar
 * Process a output characters to be sent to the slip line.
 * Do any necessary quoting framing, etc.
 */
uint slip_sndchar (tt_soc *tty, uchar *p, int max)
{
    uchar c;
    paktype *pak = tty->slip_outpak;
    hwidbtype *idb = tty->tty_idb;
    int len = 0;
    leveltype status;


    while (len < max) {
	if (pak == NULL) {
	    /*
	     * An output interrupt has occurred, and there is no packet
	     * waiting to be sent.  Check if there are more packets queued,
	     * and if not, disable the transmitter to prevent more interrupts
	     */
            status = raise_interrupt_level(NETS_DISABLE);
	    pak = (*idb->oqueue_dequeue)(idb);
            reset_interrupt_level(status);
	    tty->slip_outpak = pak;
	    if (!pak) {
		tty->slip_ostate = SLIP_TXIDLE;
		return(len);
	    }

	    slip_traffic.outpacks++;
	    slip_traffic.outbytes += pak->datagramsize;

	    if (async_packet_debug)
	      buginf("\nSLIP%t: Output packet, len = %d, header = %x %x %x %x",
		     tty->ttynum,
		     pak->datagramsize,
		     pak->datagramstart[0],
		     pak->datagramstart[1],
		     pak->datagramstart[2],
		     pak->datagramstart[3]);

	    pak->dataptr = pak->datagramstart;
	    pak->slip_length = pak->datagramsize;
            if (pak->acct_proto != ACCT_PROTO_LAST){
	       idb->counters.tx_cumbytes += pak->datagramsize;
               idb->counters.outputs++;
	    }
            GET_TIMESTAMP(idb->lastoutput);
	    tty->slip_ostate = SLIP_XMIT;
	    *p++ = SLIP_FRAME_END;
	    len += 1;
	    continue;
	}

	/*
	 * Now we know that a packet exists, and can procede to send the
	 * next character, doing quoting and such as we go along...
	 */
	switch (tty->slip_ostate) {

	case SLIP_XMIT:
	    c = *pak->dataptr;
	    switch (c) {
	    case SLIP_FRAME_END:
	    case SLIP_FRAME_ESC:
		tty->slip_ostate = SLIP_TXESCAPE;
		c = SLIP_FRAME_ESC;
		break;
		
	    default:
		pak->dataptr++;
		if (--pak->slip_length == 0)
		  tty->slip_ostate = SLIP_NEEDSEND;
		break;
	    }

	    break;

	case SLIP_TXESCAPE:
	    c = *pak->dataptr++;
	    switch (c) {
	    case SLIP_FRAME_END:
		c = SLIP_TRANS_END;
		break;

	    case SLIP_FRAME_ESC:
		c = SLIP_TRANS_ESC;
		break;

	    default:
		errmsg(&msgsym(BADQUOTE, SLIP), c);
	    }
	    if (--pak->slip_length == 0) {
		tty->slip_ostate = SLIP_NEEDSEND;
	    } else {
		tty->slip_ostate = SLIP_XMIT;
	    }
	    break;

	case SLIP_TXIDLE:
	    /*
	     * In theory, we should never get here, since the transmitter
	     * will not be idle unless there is no output packet, and that
	     * condition is handled earlier.  However, assume that something
	     * has started the transmitter slightly at the wrong time, and
	     * start sending a packet hoping it is where it belongs 
	     */
	    tty->slip_ostate = SLIP_XMIT;
	    /* Fall through */

	case SLIP_NEEDSEND:
	    c = SLIP_FRAME_END;
	    tty->slip_ostate = SLIP_XMIT;
	    datagram_done(pak);
	    pak = tty->slip_outpak = NULL;
	    break;

	default:
	    c = 0xFF;		/* Should never happen */
	}
	*p++ = c;
	len += 1;
    }
    return(len);
}

/* get_unnum_for_async - Walk the hardware IDB queue, look for an
 * interface that we can use to do unnumbered off of.  If we find one,
 * return it, else return NULL. 
 */
static idbtype *get_unnum_for_async (tt_soc *tty)

{
    hwidbtype *hwidb;
    idbtype *idb;

    FOR_ALL_HWIDBS(hwidb) {
	idb = hwidb->firstsw;
	if (!idb->ip_enabled)		/* Don't use if not running IP */
	    continue;
	if (idb->ip_unnumbered)		/* Can't use unnumbered i/f */
	    continue;
	if (tty->tty_idb == idb->hwptr)	/* Can't use self */
	    continue;
	return(idb);
    }
    return(NULL);    
}


/*
 * get_slip_idb - Get an idb pointer, and handle backwards compatibility
 *
 * This routine is used to get an IDB pointer for various SLIP configuration
 * commands.
 *
 * In new configuration files, there will always be an explicit reference
 * to an async interface, so by the time a SLIP configuration command is
 * seen, the async IDB will already exist.
 *
 * In an old configuration file, or when the user is entering things
 * manually, an async IDB will not yet exist. In this case, we create
 * one, unnumbered, pointing to the first interface on the system.
 * Additionally, if IP routing has not been enabled, then we turn on
 * routing with host mode fallback. This allows the old configuration
 * file (which won't enable any routing protocols) to still pass traffic
 * to and from the async interfaces.
 */

idbtype *get_slip_idb (tt_soc *tty)
{
    idbtype *idb;

    idb = firstsw_or_null(tty->tty_idb);
    if (idb)
      return(idb);

    idb = firstsw_or_null(init_async_idb(tty, TRUE));
    if (!idb) {
	printf(nomemory);
	return(NULL);
    }

    idb->ip_unnumbered = get_unnum_for_async(tty);
    /*
     * If we got an idb, set IP up.  Else, there is no valid interface
     * to point at, so leave IP down.
     */
    if (idb) {
	idb->ip_enabled = TRUE;
	ip_address_change(idb, 0, 0, FALSE, TRUE);
	reg_invoke_ipcache_setup_interface(idb->hwptr);
    } 

    /*
     * Enable IP routing with host-style fallback if IP routing hasn't
     * yet been enabled.
     */

    if (router_enable && (!ip_router_running && 
			  (!(BRIDGING_IF(idb, LINK_IP)))))
      ip_routing(TRUE, TRUE);

    return(idb);
}

/*
 * show_slipndata
 * Show relevant SLIP data for the SHOW LINE command.
 */

void show_slipdata (tt_soc *tty)
{
    hwidbtype *hwidb = tty->tty_idb;
    idbtype *idb;

    if (!(tty->statbits & SLIPMODE))
	return;
    idb = hwidb->firstsw;
    printf("\nLine is running %s", encstring(hwidb->enctype));
    if (!idb->static_routing)
	printf(" routing");
    if (idb->ip_peer && idb->ip_peer->ippeer_addr)
	printf(" for address %i",idb->ip_peer->ippeer_addr);
    printf(".\n%d output packets queued, %d input packets.\n",
	   hwidb->output_qcount, hwidb->input_qcount);
    if (hwidb->enctype == ET_PPP && tty->appp_context) {
	int i;
	printf(" Async Escape map is ");
	for (i=0; i < 32; i++)
	    printf("%c", tty->appp_context->EscapeCharMap[i] ? '1':'0');
    }
}

/*
 * show_slip
 * Show a bunch of info about SLIP connections, including SLIP Traffic.
 */

static const char sliphead[] = 
{" Int           Local          Remote Qd     InPack     OutPac Inerr  Drops  MTU"};
/*
 *  96 192.123.456.789 100.123.456.789  N AAAAAAAAAA YYYYYYYYYY CCCCC DDDDDD MMMM
 */

void show_slip (parseinfo *csb)
{
    int i,len;
    tt_soc *tty;
    boolean printflag;
    hwidbtype *hwidb;
    idbtype *swidb;

    automore_enable(NULL);
    printf("\nAsync protocol statistics:");
    printf("\n  Rcvd: %lu packets, %lu bytes",
	slip_traffic.inpacks, slip_traffic.inbytes);
    printf("\n        %lu format errors, %lu checksum errors, %lu overrun, %lu no buffer",
	slip_traffic.fmterrors, slip_traffic.crcerr, slip_traffic.badlen,
	slip_traffic.nobuf);
    printf("\n  Sent: %lu packets, %lu bytes, %lu dropped",
	   slip_traffic.outpacks,  slip_traffic.outbytes,
	   slip_traffic.dropped);
    printf("\n\n\n");
    automore_conditional(0);
    printc('\n');

    printflag = FALSE;
    for (i = 0; i < ALLlines; i++) {
	tty = MODEMS[i];
	hwidb = tty->tty_idb;
	/*
	 * Print, if we have an async interface and it is not
	 * deleted. Just testing MAYSLIP | MUSTSLIP will miss
	 * no-mode Async, commonly used by outgoing DDR.
	 */
	if (hwidb && (hwidb->status & IDB_ASYNC) &&
	    !(hwidb->status & IDB_DELETED)) {
	    if (!printflag) {
		automore_header(sliphead);
		printflag = TRUE;
	    }

	    /*
	     * Get the software IDB pointer. Note Async interfaces
	     * don't exist w/o a SW idb.
	     */

	    swidb = firstsw_or_null(hwidb);

	    /*
	     * Handle the active character and the line number
	     */

	    printf("\n%c%-3d ",((tty->statbits & SLIPMODE) ? '*' : ' '),i);

	    /*
	     * Handle the local address
	     */

	    len = printf("%-15i", swidb->ip_unnumbered ?
			 swidb->ip_unnumbered->ip_address :
			 swidb->ip_address);

	    /*
	     * Handle the remote address
	     */

	    if ((tty->slip_flags & SLIP_DYNAMIC) &&
		((tty->statbits & SLIPMODE) == 0))
	      printf(" %-15s", "Dynamic");
	    else
	      if (swidb->ip_peer && swidb->ip_peer->ippeer_addr)
		printf(" %-15i", swidb->ip_peer->ippeer_addr);
	      else
		printf(" %-15s", "None");

	    /*
	     * Handle the other remaining statistics
	     */

 	    printf(" %-2d%-11lu%-11lu%-6d%-7d%-5d",
		   hwidb->output_qcount,
		   idb_inputs(hwidb),
		   idb_outputs(hwidb),
		   hwidb->counters.input_error,
		   hwidb->counters.output_total_drops,
		   hwidb->maxmtu);
	}
    }
    if (!printflag)
      printf("\nNo lines currently configured for async protocol usage");
    automore_disable();
}

/*
 * check_other_asyncs()
 *	Make sure no other async port is doing SLIP at this IP address
 *
 * Returns TRUE for OK, FALSE if the address is already in use
 */
boolean check_other_asyncs (idbtype *swidb, ipaddrtype addr)
{
    idbtype *idb;

    /*
     * Sanity
     */
    if (!addr) {
	return(TRUE);
    }

    /*
     * Check each swidb, and look for address conflicts
     *   Use current peer address as opposed to ip_neighbor_address because
     *   not all peer addresses are added as 'neighbor' routes
     */
    FOR_ALL_SWIDBS(idb) {
	if (ip_ifaddress(idb, addr) ||
	    ((reg_invoke_ip_peer_addr(idb) == addr) && (idb != swidb))) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * slip_exec_command (previously slip_on)
 * Implement the SLIP & PPP exec commands.
 *
 * The SLIP command has the following arguments:
 *
 *	/compressed - Enable VJ header compression
 *	/routing    - Use routing support
 *
 * And an optional trailing IP address or the word 'default' or the word
 * 'negotiate'..
 */

#define MAXCHARS 200

void slip_exec_command (parseinfo *csb)
{
    char host[MAXCHARS];
    char *reason, *compr_string_ptr;
    char *atsign_ptr = NULL;
    int timeout;
    hwidbtype *hwidb;
    idbtype *idb;
    idbtype *cfg_idb;
    idbtype *tempidb = NULL;
    int dynamic_default;
    ipaddrtype addr;
    enum PEER_ADDR_SOURCES addr_source;
    encapstype enctype;
    boolean async_routing_flag;
    boolean will_negotiate;
    boolean ip_on_interface;
    char *poolname = NULL;

    if (!reg_invoke_async_line_get_idb(stdio->type, stdio, &idb, &hwidb))
	return;

    if (!(stdio->capabilities & MAYSLIP)) {
	printf("\nThis line may not run %s.",
	       GETOBJ(int,3) ? "PPP" : "SLIP");
	return;
    }

    /*
     * ensure that a peer info structure exists for the current interface
     */
    if (!idb->ip_peer && !create_ippeerinfo(idb)) {
	printf(nomemory);
	return;
    }

    /*
     * find top-level configuration IDB and ensure that a peer info structure
     * exists for it. The configuration IDB may be associated with a
     * different type of interface altogether e.g. V.120. Should that be the
     * case then tempidb will contain the appropriate IDB.
     */
    if (stdio->cfgidb) {
	tempidb = (*stdio->cfgidb)(stdio);
    }
    cfg_idb = get_netcfg_idb(tempidb ? tempidb : idb);
    if (!cfg_idb->ip_peer && !create_ippeerinfo(cfg_idb)) {
	printf(nomemory);
	return;
    }

    /*
     * Determine the correct encapsulation
     */
    if (GETOBJ(int, 3)) {
	enctype = ET_PPP;
    } else {
	enctype = ET_SLIP;
    }

    /*
     * Set negotiate flag from the parse info
     */
    will_negotiate = GETOBJ(int, 5);

    /* Refuse if IDBS_ADMINDOWN */

    if (IDBS_ADMINDOWN == hwidb->state) {
	printf("\n%s: administratively down, %s not allowed",
	       hwidb->hw_namestring, GETOBJ(int,3) ? "PPP" : "SLIP");
	return;
    }

    /* Refuse if line has open network connections */

    if (!QUEUEEMPTY(&stdio->connhead)) {
	printf("\n%%You have open connections"
	       " - Close them before proceeding ");
	return;
    }

    addr = idb->ip_peer->ippeer_addr_def;	/* Pick up default address */
    addr_source = PEER_ADDR_FROM_CONFIG; /* No special source if default addr */

    /*
     * We will allow routing protocols on the line if
     * either the user entered /route option or (if we
     * don't have async_dynamic_routing but do have
     * async_default_routing flags.
     */
    async_routing_flag = (GETOBJ(int, 2) ||
			  ((stdio->slip_flags & SLIP_DEF_ROUTING) &&
			  !(stdio->slip_flags & SLIP_DYN_ROUTING)));

    /*
     * If PPP is requested and IP is not defined, then don't require an IP
     * address or 'negotiate' to be entered.  Just 'ppp' should work.
     */
    ip_on_interface = reg_invoke_proto_on_swidb(LINK_IP, cfg_idb);
    if ((ET_PPP == enctype) && !ip_on_interface) {
	will_negotiate = TRUE;
    }
    /*
     * Dynamic address assignment ?
     *
     * If address will be negotiated by IPCP, no need to do all of this
     * processing.
     */
    if (!will_negotiate && (stdio->slip_flags & SLIP_DYNAMIC)) {
	if (GETOBJ(int, 4)) {	/* Default address? */
	    dynamic_default = TRUE;
	} else {
	    dynamic_default = FALSE;
	    if ((GETOBJ(int, 8) & SLIP_OPTION_IPADDR) != 0) {
		sstrncpy(host, GETOBJ(string,1), MAXCHARS);
	    } else {
		if (old_slip_prompts) {
		    /*
		     * Never change the way this looks, to avoid breaking
		     * scripts that start up slip/ppp automatically
		     */
		    printf("IP address or hostname: ");
		} else {
		    printf("IP address or hostname: ");
		}
		if (!rdtty(host, MAXCHARS)) {
		    printf("\n%% Bad IP address");
		    return;
		}
	    }

	    /*
	     * If the user says "default", see if we have configured a
	     * default dynamic address or have pooling available.
	     */

	    if (!strcasecmp(host, "negotiate")) {
		will_negotiate = TRUE;
	    } else if (!strcasecmp(host, "default") && ip_pooling_available(idb)) {
		dynamic_default = TRUE;
	    } else {
		/* 
		 * If there is an '@' in the hostname, null it out and
		 * keep the ptr 
		 */
		for (atsign_ptr = host; *atsign_ptr; atsign_ptr++)
		    if (*atsign_ptr == '@')
			break;
		if (*atsign_ptr) {
		    *atsign_ptr++ = 0;
		} else
		    atsign_ptr = NULL;

		uppercase(host);
		if (!parse_hostname(host, &addr)) {
		    printf("\n%% Bad IP address");
		    return;
		}

		if (atsign_ptr != NULL) {	/* If there was an '@', */
		    *--atsign_ptr = '@';	/*    then put it back. */
		}
	    }
	}


	if (!will_negotiate && !dynamic_default && addr) {
	    addr_source = PEER_ADDR_FROM_EXEC;
	    if (!check_other_asyncs(idb, addr)) {
		printf("\n%% Address %i in use on another interface", addr);
		return;
	    }
	    if (!check_slip_addr(addr)) {
		printf("\n%% Address %i is already in use", addr);
		return;
	    }
	    if (old_access_control &&
		!reg_invoke_login_slipaddr(stdio, host, &reason, addr,
					   async_routing_flag)) {
		if (reason)
		  printf("%% %s", reason);
		return;
	    }
	}
    }

    if (old_access_control) {
	/* Check if this user is allowed to use this address */
	if (!check_slip_on_access(idb, stdio->user, NULL, &reason, stdio, addr, 
				  async_routing_flag, TRUE)) {
	    if (reason)
		printf(" - %s", reason);
	    goto out;
	}
    } else {
        ipaddrtype old_addr;
	int old_xtacacs_prompts = FALSE;

	old_addr = addr;	/* record it so we can see if it changed */

	/*
	 * Ask AAA about this attempted session.  It can modify the
	 * requested address, as well as specify a pool
	 */
	if (!slip_do_author(stdio, hwidb, enctype, &addr,
			    &async_routing_flag, &poolname,
			    &old_xtacacs_prompts)) {
	    /* Already printed a message if there was one. */
	    return;
	}

	if (old_xtacacs_prompts) {
	    /* If we are emulating old (x)tacacs style prompts we must
	     * prompt for an additional password which we won't
	     * actually look at, whenever the user enters "slip" or
	     * "slip <ipaddress>" 
	     */

	    if (((GETOBJ(int, 8) & 
		  (SLIP_OPTION_IPADDR|
		   SLIP_OPTION_NEGOTIATE|
		   SLIP_OPTION_DEFAULT)) == 0) ||
		(GETOBJ(int, 8) & SLIP_OPTION_IPADDR)) {

		char buff[40];

		askstring(stdio, buff, sizeof(buff), "Password: ", 30*ONESEC, 
			  1, TRUE);
	    }
	}

	/*
	 * If the address changed, then we need to use a different 'source'
	 */
	if (addr != old_addr)
	    addr_source = PEER_ADDR_FROM_AAA;
    }

    if (addr)
        reg_invoke_set_ip_peer_addr(idb, addr, addr_source);

    /*
     * Still haven't found a peer address. If we don't have a poolname
     * either (e.g. from tacacs+) see if there's a configured poolname
     * we can use. If not, use poolname "default".
     *
     * Invoke IP Pooling mechanisms to find an address for the
     * peer.
     */
    if (!addr) {
	if (!poolname) {
	    /*
	     * poolname not provided by TACACS+ so see if the configuration
	     * interface has a special pool name to use
	     */
	    poolname = cfg_idb->ip_peer->ippeer_poolname_def ?
		       cfg_idb->ip_peer->ippeer_poolname_def :
		       DEFAULT_LOCAL_POOL_NAME;
	}
	reg_invoke_ip_get_pool_addr(&addr, idb, get_user_name(stdio),
				    poolname, 4*ONESEC);
    }

    /*
     * Sanity check address parameters.
     * SLIP should be allowed only if IP parameters are good.
     */
    if (ET_SLIP == enctype) {
	if (!ip_on_interface) {
	    printf("\n%% IP not configured on this interface");
	    if (async_state_debug)
		buginf("\n%s: IP not configured. Not starting SLIP",
		       hwidb->hw_namestring);
	    goto out;
	} else if (!addr) {
	    printf("\n%% No address for remote."
		   " Check 'Peer Default IP Address a.b.c.d'");
	    printf("\n%%  or configure IP pooling support.");
	    if (async_state_debug)
		buginf("\n%s: No remote IP address. Not starting SLIP",
		       hwidb->hw_namestring);
	    goto out;
	}
    }

    /* Allow different encap only if not a rotary group member */

    if ((hwidb->enctype != enctype) &&
	!reg_invoke_dialer_huntgroup_member(hwidb))
	async_new_encap(hwidb, enctype);

    if (hwidb->enctype != enctype) {
	printf("\n%% Unable to set encapsulation");
	goto out;
    }

    /* 
     * NOTE: hwidb->enctype is set to ET_SLIP by default.  Therefor, 
     * reg_invoke_async_handle_option() should not get called unless
     * the encapsualtion type is set first.
     * 
     * Hook for PPP option handling
     */
    if (enctype == ET_PPP) {
	reg_invoke_async_handle_options(stdio->type, stdio);
    }

    /*
     * set VJ compression flags as needed
     * also build the VJ info prompt, here
     */

    compr_string_ptr = NULL;

    if (hwidb->tcp_compress) {

	compr_string_ptr = &host[0];      /* Reuse host[] for VJ info prompt */

	compr_string_ptr += sprintf(compr_string_ptr,
				    "\nHeader compression ");

	hwidb->tcp_compress->flags &= ~THC_SESSION;
	if (hwidb->tcp_compress->flags & THC_PERMACTIVE) {

		compr_string_ptr += sprintf(compr_string_ptr, "is on.");

	} else if (GETOBJ(int, 1)) {
	    compr_string_ptr += sprintf(compr_string_ptr,
					"forced by /compress.");
	    hwidb->tcp_compress->flags |= THC_SESSION;
	} else {
	    compr_string_ptr += sprintf(compr_string_ptr,
					"will match your system.");
	}
    }

    if (old_slip_prompts) {
 	/*
 	 * Never change the way this looks, to avoid breaking
 	 * scripts that start up slip/ppp automatically
 	 */
 	printf("Entering %s mode.\n",  encstring(enctype));
 	if (addr)
 	    printf("Your IP address is %i. ", addr);
 	printf("MTU is %d bytes\n\n", hwidb->maxmtu);
	tempidb = NULL;
    } else {
	printf("Entering %s%s mode.", 
	       encstring(enctype),
	       async_routing_flag ? " routing" : NULL);
	printf("\n%s interface address is ",
	       reg_invoke_dialer_huntgroup_member(hwidb) ? "Dialer" : "Async");
	hwidb = get_netidb(hwidb);
	tempidb = hwidb->firstsw;
	
	if (tempidb->ip_unnumbered)
	    printf("unnumbered (%s)\n", tempidb->ip_unnumbered->namestring);
	else
	    printf("%i\n", tempidb->ip_address);

	if (addr)
	    printf("Your IP address is %i. ", addr);

	printf("MTU is %d bytes", hwidb->maxmtu);

	if (compr_string_ptr && host[0]) {
	    /*
	     * we have the compression prompt written on host[] array
	     */
	    printf("%s", &host[0]);
	}
    }
    printf("\n\n");

    /*
     * Set the routing flag for the interface
     */
    idb->static_routing = ! async_routing_flag;

    /*
     * Notify accounting
     * If we're starting PPP, this will be done when IPCP negotiates,
     * so we don't have to do it here.
     */
    if (enctype != ET_PPP) {
	if (addr || !tempidb) {
	    reg_invoke_aaa_acct_netinfo(stdio->ustruct, AAA_ATTR_ADDR, &addr,
					AAA_TYPE_IP, sizeof addr, TRUE);
	} else {
	    reg_invoke_aaa_acct_netinfo(stdio->ustruct, AAA_ATTR_ADDR,
			&tempidb->ip_address, AAA_TYPE_IP, sizeof addr, TRUE);
	}
    }

    /*
     * Wait for any output to finish
     */
    startoutput(stdio);
    timeout = 20;
    /* In the case of a TCP connection on a vty-async line, we want to make
     * sure that the connection messages are printed.  TCP can do
     * lots of things that will hold it up for more that half a second
     */
    do {
        process_sleep_for(ONESEC/2);
    } while ((!modemcheck(stdio)) && outputpending(stdio) && timeout--);

    async_proto_mode(stdio);		/* Turn on the protocol */
    stdio->usagecount++;
    if (enctype != ET_PPP) {
	/*
	 * Note that the accounting start message for a PPP connection
	 * will be sent when LCP comes up, so don't send it here.
	 */
	reg_invoke_aaa_acct_net(stdio->ustruct, csb, idb);
    }

    if (modemcheck(stdio)) {
	/*
	 * If the modem hung up somewhere above, set HANGUP
	 * to let modem process clear the interface, instead of
	 * waiting for the process to die on its own.  Also set
	 * SLIPMODE, so that tty_if_reset doesn't just bail out if
	 * async_proto_mode has failed. We really need it to go
	 * through async_reset, to do an inorder cleanup of the
	 * interface, now.
	 *
	 * Cover the non-modem lines by directly calling tty_if_reset()
	 */
	if (async_state_debug) {
	    buginf("\n%s: Carrier dropped while interface starting up",
		   hwidb->hw_namestring);
	}
	stdio->statbits |= (SLIPMODE + HANGUP);
	tty_if_reset(stdio); /* redundancy to cover non-modem lines */
    }

    /*
     * We're finished. Handle the process depending on the line type.
     * If its a vty-async connection, then its a translation type,
     * start the vtyasync helper process.  Otherwise, its a tty line,
     * and we need to kill the process.
     */
    reg_invoke_async_handle_process(stdio->type, stdio);
    return;

    /*
     * Failure; clean up and return
     */
out:
    reg_invoke_clear_ip_peer_addr(idb);
}

static boolean slip_do_author (tt_soc *tty, hwidbtype *hwidb,
			       encapstype enctype, ipaddrtype *addr,
			       boolean *routing, char **poolname,
			       boolean *old_prompts)
{
    int avail = 0, i, mandpool = 0, mandaddr = 0;
    boolean ret;
    char buf[20], *attr, *val;
    enum AV_TYPE type;
    authorstruct a;
    idbtype *swidb;
    char *route_val = NULL; /* set only if there is a route attribute */
    enum AV_TYPE route_type = AAA_AV_MAND;
    const char *encstr = encstring(enctype);

    /*
     * authorize at exec level if autoselected - "real" authorization will
     * be done from lcp/ipcp/etc.  AUTOSELECT_AUTHORIZE is set by a previous
     * autoselect process (or exec for autosel during login) and is sticky
     * past that process being killed, so that we can now notice that we
     * should temporarilly bypass (exec-level) authorization.
     */
    if  ((enctype == ET_PPP) && (tty->autoselectmask & AUTOSELECT_AUTHORIZE))
	return(TRUE);

    if (!aaa_author_needed(AAA_AUTHOR_TYPE_NET, 0, NULL, tty))
	return(TRUE);

    memset(&a, 0, sizeof(a));
    a.auth_idb = tty->tty_idb;
    a.auth_tty = tty;

    ret = aaa_add_AV(&a.out_args, &avail, AAA_ATTR_service,
		     ((ET_SLIP == enctype) ? AAA_VAL_SVC_slip :
		     AAA_VAL_SVC_ppp), AAA_AV_MAND);
    if (ret) {
	ret = aaa_add_AV(&a.out_args, &avail, AAA_ATTR_protocol,
			 AAA_VAL_PROT_ip, AAA_AV_MAND);
    }
    if (ret && (!addr || !*addr)) {
	ret = aaa_add_AV(&a.out_args, &avail, AAA_ATTR_addr_pool,
			AAA_ATTR_addr_pool_default, AAA_AV_OPT);
    }
    if (ret && addr && *addr) {
	sprintf(buf, "%i", *addr);
	ret = aaa_add_AV(&a.out_args, &avail, AAA_ATTR_addr,
			 buf, AAA_AV_OPT);
    }
    if (ret && *routing)
	ret = aaa_add_AV(&a.out_args, &avail, AAA_ATTR_routing,
			 AAA_VAL_true, AAA_AV_OPT);

    if (ret) {
	ret = aaa_do_author(tty->ustruct, &a, AAA_AUTHOR_TYPE_NET, 0);

	switch(a.status) {
	case AAA_AUTHOR_STATUS_ERROR:
	case AAA_AUTHOR_STATUS_FAIL:
	    ret = FALSE;
	    break;
	case AAA_AUTHOR_STATUS_PASS_ADD:
	    if (!a.in_args) {
		break;
	    }
	    aaa_merge_attrs(a.out_args, a.in_args, &a.in_args);
	    /* FALLTHRU */
	case AAA_AUTHOR_STATUS_PASS_REPL:
	    if (!a.in_args) {
		break;
	    }
	    for (i = 0; ret && a.in_args[i]; i++) {
		if (aaa_author_debug) {
		    buginf("\nAAA/AUTHOR/%s: %s: Processing AV %s", 
			   hwidb->hw_namestring, 
			   (enctype == ET_SLIP ? "SLIP" : "PPP"),
			   a.in_args[i]);
		}
		if (aaa_split_AV(a.in_args[i], &attr, &val, &type)) {
		    if (!strcmp(attr, AAA_ATTR_service)) {
			if (strcmp(val, AAA_VAL_SVC_slip) &&
			    strcmp(val, AAA_VAL_SVC_ppp)) {
			    if (type == AAA_AV_MAND)
				ret = FALSE;
			}
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_protocol)) {
			if (strcmp(val, AAA_VAL_PROT_ip)) {
			    if (type == AAA_AV_MAND)
				ret = FALSE;
			    else
				if (aaa_author_debug)
				    buginf("\nAAA/AUTHOR/%s: "
					   "tried to change \"protocol\". "
					   " Ignoring.", encstr);
			}
			continue;
		    }

		    if (!strcmp(attr, AAA_ATTR_addr_pool)) {
			/*
			 * Select pool to use.  If it's mandatory
			 * to use the pool, zero out any address
			 * the user might have requested.
			 */
			*poolname = strdup(val);
			if (*poolname == 0) {
			    *poolname = "<nomem>";
			}
			if (type == AAA_AV_MAND) {
			    mandpool = 1;
			    if (!reg_invoke_ip_pool_exists(*poolname)) {
				ret = FALSE;
				if (aaa_author_debug)
				    buginf("\nAAA/AUTHOR/SLIP: %s: mandatory "
					   "pool '%s' does not exist",
					   hwidb->hw_namestring, *poolname);
			    }
			}
			continue;
		    }

		    if (!strcmp(attr, AAA_ATTR_addr)) {
			ipaddrtype tmpaddr;
			boolean good;

			good = parse_ip_address(val, &tmpaddr);
			if (!good) {
			    if (aaa_author_debug) {
				buginf("\nAAA/AUTHOR/SLIP: "
				       "address denied %i", tmpaddr);
			    }
		            if (type == AAA_AV_MAND) {
			        ret = FALSE;
			    }
		        } else {
			    *addr = tmpaddr;
		            if (type == AAA_AV_MAND) {
			        mandaddr = 1;
			    }
		        }
			continue;
		    }

		    if (!strcmp(attr, AAA_ATTR_routing)) {
			if (!strcmp(val, AAA_VAL_true)) {
			    if ((type == AAA_AV_MAND) &&
				!(tty->slip_flags & 
				  (SLIP_DYN_ROUTING|SLIP_DEF_ROUTING))) {
				if (aaa_author_debug) {
				    buginf("\nAAA/AUTHOR/%s: routing denied"
					   " -- tty not configured to route",
					   encstr);
				}
				ret = FALSE;
			    }
			    if (tty->slip_flags & 
				(SLIP_DYN_ROUTING|SLIP_DEF_ROUTING)) {
				if (aaa_author_debug) {
				    buginf("\nAAA/AUTHOR/SLIP: set routing"
					   "=TRUE");
				}
				*routing = TRUE;
			    }
			} else {
			    if (aaa_author_debug) {
				buginf("\nAAA/AUTHOR/SLIP: set routing=FALSE");
			    }
			    *routing = FALSE;
			}
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_inacl)) {
			FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
			    reg_invoke_ip_set_per_user_acl(swidb, IN, 
							   atoi(val));
			}
			continue;
		    }
		    if (!strcmp(attr, AAA_ATTR_outacl)) {
			FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
			    reg_invoke_ip_set_per_user_acl(swidb, OUT, 
							   atoi(val));
			}
			continue;
		    }

		    if (!strcmp(attr, AAA_ATTR_route)) {
			if (enctype == ET_SLIP) {
			    /* If there is a route attribute, note it here.  We
			     * will install this route only if we are doing SLIP
			     * and authorization is successful.  For PPP, the
			     * route attribute is processed in ipcp.c 
			     */
			    route_type = type;
			    route_val = val;
			}
			continue;
		    }

		    if (!strcmp(attr, AAA_ATTR_old_prompts)) {
			*old_prompts = (strcmp(val, AAA_VAL_true) == 0);
		    }
		    continue;

		    /* Other attributes to think about for the future
		     *	timeout
		     *	idletime
		     *	compressed
		     */
		    /* if unknown and mandatory, then bail */
		    if (type == AAA_AV_MAND) {
			if (aaa_author_debug)
			    buginf("\nAAA/AUTHOR/%s: received unknown "
				   "mandatory AV: %s", encstr, a.in_args[i]);
			ret = FALSE;
		    }
		}
	    }
	    break;
	}
    }

    if (ret) {
	if (mandpool && mandaddr) {
	    if (aaa_author_debug) {
		buginf("\nAAA/AUTHOR/SLIP: "
		       "mandatory address %i conflicts with "
		       "mandatory pool %s", *addr, *poolname);
	    }
	    ret = FALSE;
	} else if (mandpool) {
	    /*
	     * If it's mandatory to use the pool, zero out any address
	     * the user might have requested.  This will force an attempt
	     * to load from the named pool.
	     */
	    *addr = 0;
	} 
    }

    if (ret && route_val) {
	/* A successful authorization. Install a route if required */
	ret = reg_invoke_store_per_user_ip_route(hwidb, route_type, route_val);
	if (ret) {
	    reg_invoke_install_per_user_route(hwidb);
	}
    }

    if (aaa_author_debug) {
	buginf("\nAAA/AUTHOR/SLIP: %s: %s",
	       hwidb->hw_namestring, ret ? "succeeded" : "denied");
    }

    if (a.server_msg)
	printf("%s\n", a.server_msg);
    if (!ret && !a.server_msg) {
	printf("%s authorization failed.\n", encstr);
    }
    aaa_cleanup_author(&a);
    return(ret);
}

/*
 * check_slip_on_access
 * check if user is allowed to use this address
 */
boolean check_slip_on_access (idbtype *idb, char *user_name, char *password, 
			      char **reason, tt_soc *tty, ipaddrtype address, 
			      boolean routing, boolean exec_cmd)
{
    if (reason != NULL)
	*reason = NULL;
    /* Ensure that this user is allowed to use this address. Note
     * that the address may be zero at this point - this should still
     * be checked.
     */

    if (!reg_invoke_login_slipon(user_name, password, reason, tty, address,
				 routing, exec_cmd)) {
	printf ("\n%% Network access denied by TACACS Server");
	return(FALSE);
    }
    return(TRUE);
}

/*
 * Routines to handle the SLIP encapsulation
 */

/*
 * slip_vencap - Encapsulate a SLIP packet
 */

boolean slip_vencap (
    paktype *pak,
    long address)
{
    iphdrtype *ip;
    hwidbtype *idb;
    tt_soc *tty;

    ip = (iphdrtype *)ipheadstart(pak);

    switch (pak->linktype) {
      case LINK_IP:
	break;

      case LINK_UNCOMPRESSED_TCP:
	ip->version = 7;
	break;

      case LINK_COMPRESSED_TCP:
	ip->version |= 8;
	break;

      default:
	return(FALSE);		/* Don't know any other protocols */
    }

    pak->enctype = ET_SLIP;	/* Mark as SLIP packet */
    pak->encsize = 0;           /* No encap */
    /*
     * Make sure routing cache is populated by transit traffic.
     */
    reg_invoke_fs_cache_update(pak->linktype, pak, 0L);

    idb = pak->if_output->hwptr;
    tty = idb->idb_tty;
    if ((tty->capabilities & OUTIDLE) && dialer_async_classify(idb, pak)) {
	TIMER_START(tty->dallytimer, tty->tty_sessiontimeout);
    }

    return(TRUE);
}


/*
 * slip_getlink - Get the link type of a SLIP frame
 */

static long slip_getlink (paktype *pak)
{
    thcdbtype *tcp_compress;
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    switch (pak->enctype) {
      case ET_SLIP:
	if (ip->version & 8)
	    return(LINK_COMPRESSED_TCP);
	if (ip->version >= 7)
	    return(LINK_UNCOMPRESSED_TCP);
	if (pak->if_input) {
	    tcp_compress = pak->if_input->hwptr->tcp_compress;
	    if ((tcp_compress) && (tcp_compress->flags & THC_ACTIVE) &&
		thc_verify_compress (pak))
		/*
		 * We have a compressible packet that isn't
		 * compressed. If we're in dynamic mode, the other
		 * end seems not to be compressing. (Maybe we got
		 * put in compressing mode by an accident of line noise.
		 */
		if ((tcp_compress->flags&(THC_PERMACTIVE|THC_SESSION)) == 0)
		    tcp_compress->flags &= ~THC_ACTIVE;
	}	    
	return(LINK_IP);
    }
    return(LINK_ILLEGAL);
}

/*
 * Actions to take when interface state changes.
 * Similar to hdlc_cstate, only the only thing we worry about is dialers.
 */
static void slip_lineaction (hwidbtype *idb)
{
    /*
     * SLIP should not run on a Group Async interface.
     */
    if (IDBTYPE_ASYNC_GROUP == idb->type) {
	return;
    }

    /*
     * Clear the spoofing flag if we are a dialer.  This way we now
     * track the real state of line protocol.
     */
    if ( idb->state == IDBS_UP)
	reg_invoke_media_protocol_up(idb, LINK_IP);
}

/*
 * Remove SLIP encapsulation from a line
 */
static void slip_deencap (hwidbtype *idb)
{
    idb->vencap = NULL;
    idb->deencap = NULL;
}

/*
 * slip_extract_addr
 *
 * Given a SLIP packet, extract the requested 'MAC' level address
 * into a hwaddrtype structure.
 */
boolean slip_extract_addr (
    paktype *pak,
    hwaddrtype *address,
    enum SRCDST which)
{
    address->type = STATION_SLIP;
    address->length = STATIONLEN_SLIP;
    address->target_type = TARGET_UNICAST;
    bzero(address->addr, IEEEBYTES);
    return(TRUE);
}

/*
 * set up an interface to run SLIP 
 */

boolean slip_encap_setup (parseinfo *csb, hwidbtype *idb)
{
    idbtype *swidb = idb->firstsw;

    idb->enctype = ET_SLIP;	/* default encapsulation */
    /*
     * Set the fastswitch flavor
     */
    idb->fast_switch_type = FS_SLIP;

    /*
     * SLIP specific vectors
     */
    idb->vencap = slip_vencap;
    idb->deencap = slip_deencap;
    idb->lineaction = slip_lineaction;
    idb->getlink = slip_getlink;
    idb->linestate = (linestate_t) return_true;
    idb->bridge_vencap = (bridge_vencap_t)return_null;
    set_default_queueing(idb);
    idb->extract_snpa = slip_extract_addr;
    idb->iqueue = raw_enqueue;
    idb->broadcast = (broadcast_t)return_fuz_false;/*No link-level broadcast */
    idb->periodic = NULL;
    idb->show_support = NULL;	/* Not yet */
    swidb->ip_enctype = ET_SLIP;
    reg_invoke_media_enctype_setup(swidb, ET_SLIP);
    return(TRUE);
}

/*
 * If this is a physical line, suspend sw flow control for the session
 * and make a note of it in tty->slip_flags
 */
void slip_suspend_swflow (tt_soc *tty)
{
/* I beleive we are only going to allow slip on aux/tty/vty lines */
/* for vty lines (future) this will be a waste */

    if(tty->type!=VTY_LINE){
	if(tty->capabilities & SFLOWIN)                /* Suspend incoming */
	    tty->slip_flags |= SLIP_SFLOWIN_SUSPENDED;
	if(tty->capabilities & SFLOWOUT)               /* Suspend outgoing */
	   tty->slip_flags |= SLIP_SFLOWOUT_SUSPENDED;
        tty->capabilities &= ~(SFLOWIN | SFLOWOUT);
	setsflow(tty);
   }
}
/*
 * Restore sw flow control that we (might have) disabled
 *  on physical lines only
 */
void slip_restore_swflow (tt_soc *tty)
{
                                                  /* Did we disable sw flow */
    if(tty->type!=VTY_LINE){
	if(tty->slip_flags & SLIP_SFLOWIN_SUSPENDED) 
	    tty->capabilities |= SFLOWIN;             /* incoming side */
	if(tty->slip_flags & SLIP_SFLOWOUT_SUSPENDED)
	    tty->capabilities |= SFLOWOUT;            /* outgoing side */
	tty->slip_flags &= ~(SLIP_SFLOWIN_SUSPENDED | SLIP_SFLOWOUT_SUSPENDED);
	setsflow(tty);
    }
}
