/* $Id: if_async.c,v 3.20.4.38 1996/09/12 13:31:44 fox Exp $
 * $Source: /release/112/cvs/Xsys/ts/if_async.c,v $
 *------------------------------------------------------------------
 * if_async.c -- Support for using an async device as a network interface.
 *               This includes all async type lines, and all async protocols.
 *
 * October 1991,  Bill Westfield
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_async.c,v $
 * Revision 3.20.4.38  1996/09/12  13:31:44  fox
 * CSCdi68759:  AAA per-user routes are not removed when connection drops
 * Branch: California_branch
 * Separate parallel connection support for per-user routes from the
 * support for neighbor routes and dynamic dialer maps.  Transfer
 * ownership of the static route to the last idb to add it (applies only
 * to per-user routes).
 *
 * Revision 3.20.4.37  1996/09/10  06:16:28  fox
 * CSCdi68456:  Parallel connection information lost if one link
 * disconnects
 * Branch: California_branch
 * Expand check for parallel connections to protect IP dynamic dialer
 * maps.
 *
 * Revision 3.20.4.36  1996/09/03  22:35:07  sjackson
 * CSCdi66297:  All interfaces reset (all calls dropped) when group-range
 * applied
 * Avoid reconfiguring existing interfaces within the group range
 * Branch: California_branch
 *
 * Revision 3.20.4.35  1996/08/30  20:52:17  sjackson
 * CSCdi66301:  no fair-queue command not saved for group-async interface
 * Set the default bandwidth of the interface appropriately
 * Branch: California_branch
 *
 * Revision 3.20.4.34  1996/08/23  04:24:19  fox
 * CSCdi66589:  ppp_periodic() dereferences null pointer - idb->lcpfsm
 * during init
 * Branch: California_branch
 * Disable interrupts in isdn_setencap(), dialer_setencap() and
 * async_setencap() before changing encapsulations.  Make sure that
 * lcpfsm pointer in idb is set before idb vectors (such as periodic)
 * are set.
 *
 * Revision 3.20.4.33  1996/08/16  01:28:45  irfan
 * CSCdi65706:  CS performance: Async fastsend should clear_if_input
 * Branch: California_branch
 * allow for the drops to happen on the individual slow async hwidb
 * clogging the system, instead of happening on the small (75 paks)
 * ethernet input queue....which in any case has no chance of scaling
 * for 46 async lines (on Brasil) for downstream traffic.
 *
 * Revision 3.20.4.32  1996/08/15  02:46:21  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * remove unecessary fastokay checks
 *
 * Revision 3.20.4.31  1996/08/14  14:10:52  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * add multilink half-fastswitching support
 *
 * Revision 3.20.4.30  1996/08/14  00:03:28  fox
 * CSCdi58111:  Async mode dedicated needed after shut, no shut in PPP mode
 * Branch: California_branch
 * Start SLIP mode (packet mode) if dedicated packet mode is configured
 * but not active when async_reset() is called (after a 'no shut').
 *
 * Revision 3.20.4.29  1996/08/13  02:25:45  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.20.4.28  1996/08/11  00:22:36  irfan
 * CSCdi65705:  CS Performance: Async FS should be high priority thread
 * Branch: California_branch
 * Make Async FS helper a high priority process.
 *
 * Revision 3.20.4.27  1996/06/28  23:30:58  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.20.4.26  1996/06/28  08:53:27  elem
 * CSCdi59865:  Async lines 32-48 are not fast switching
 * Branch: California_branch
 * Fix bit masking for ttymask's upper 32 bits.
 *
 * Revision 3.20.4.25  1996/06/27  09:42:37  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.20.4.24  1996/06/22  01:36:38  irfan
 * CSCdi60159:  async interface routing config needs shut/no-shut
 * Branch: California_branch
 * For dedicated interfaces turn routing on/off immediately when asynf
 * default routing is configured.
 *
 * Revision 3.20.4.23  1996/06/19  20:36:59  fox
 * CSCdi59847:  Dynamic dialer maps are not cleared on async dialer ints
 * Branch: California_branch
 * Call reg_invoke_ipcp_cleanup_ip_peer() to remove the neighbor route,
 * the dynamic dialer map, and any other mechanisms tied to the current
 * peer address before clearing the peer address.
 *
 * Revision 3.20.4.22  1996/06/18  01:49:36  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.20.4.21  1996/06/17  08:35:53  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.20.4.20  1996/06/16  21:20:07  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.20.4.19  1996/06/12  19:57:14  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.20.4.18  1996/06/06  20:58:47  irfan
 * CSCdi57344:  Line stuck in carrier dropped, funny modem state 10600, no
 * process
 * Branch: California_branch
 * Ensure proper hangup/cleanup if CARDROPs in interface startup.
 *
 * Revision 3.20.4.17  1996/05/21  10:07:08  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.20.4.16  1996/05/21  06:41:56  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.20.4.15  1996/05/09  14:47:37  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.20.4.14  1996/05/04  02:08:21  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.20.4.13  1996/04/29  09:42:07  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.20.4.12  1996/04/27  06:28:17  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.20.4.11.4.1  1996/04/27  07:37:38  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.20.4.11  1996/04/19  17:11:54  fox
 * CSCdi54972:  lastinput is not maintained for IP traffic on async links
 * Branch: California_branch
 * Set last input before IP packets fastswitched from input path.
 *
 * Revision 3.12.8.5  1996/04/25  23:51:03  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.12.8.4  1996/04/23  00:34:01  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.12.8.3  1996/04/16  23:46:08  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.12.8.2  1996/04/06  21:25:33  dblair
 * Branch: Dial1_branch
 * Fix CSCdi53768 and added restart lcp when ppp multilink command
 * processed, and fix multilink not negotiated at reload problem.
 *
 * Revision 3.12.8.1  1996/02/23  20:59:17  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.20.4.10  1996/04/19  15:34:51  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.20.4.9  1996/04/15  21:23:45  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.20.4.8  1996/04/15  15:00:19  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.20.4.7  1996/04/10  15:38:52  ahh
 * CSCdi54196:  tty lines stuck in idle and hanging up
 * Fix spelling error.
 * Branch: California_branch
 *
 * Revision 3.20.4.6  1996/04/10  06:43:34  irfan
 * CSCdi54196:  tty lines stuck in idle and hanging up
 * Branch: California_branch
 * once the slip/ppp session has idle'd out, the session timer shouldn't
 * be running anymore..else it will cause multiple hangups.
 *
 * Revision 3.20.4.5  1996/04/08  20:11:34  syiu
 * CSCdi53895:  Async Callback broken
 * Branch: California_branch
 *
 * Revision 3.20.4.4  1996/04/04  09:39:10  irfan
 * CSCdi53185:  crash when changing encaps on group async interfaces
 * Branch: California_branch
 * most tty operations don't work on dummy tty's pointed to by
 * the async-group hwidb.
 *
 * Revision 3.20.4.3.4.1  1996/04/08  02:10:28  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.20.4.3  1996/03/27  12:25:34  irfan
 * CSCdi48100:  bus error on restart (bcopy6)
 * Branch: California_branch
 * Code changes to avoid double execs on the same tty: Don't answer
 * new incoming calls while previous process/framing is not cleared
 * up fully. Avoid dropping the line twice when an async interface
 * resets. Never return from kfork(). In effect background
 * Modem process should develop better relationship with foreground
 * process/framing.
 *
 * Revision 3.20.4.2  1996/03/23  01:34:09  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.20.4.1  1996/03/18  22:19:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.20.6.1  1996/03/28  17:36:35  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.20  1996/03/07  12:10:07  billw
 * CSCdi46565:  <uses> in  <show line> doesnt include PPP, Xremote, etc
 *
 * Revision 3.19  1996/02/22  05:20:52  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.18  1996/02/13  08:16:58  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.17  1996/02/07  16:16:25  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.16  1996/02/07  03:06:24  widmer
 * CSCdi48054:  async interfaces should default to no keepalive
 * Part of previous commit broken.
 *
 * Revision 3.15  1996/02/07  00:47:27  irfan
 * CSCdi48054:  async interfaces should default to no keepalive
 * Allow hwidb's to have default other than KEEPALIVE_DEFAULT.
 *
 * Revision 3.14  1996/02/01  06:11:02  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.13.4.1  1996/01/24  23:29:00  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.13  1996/01/16  00:41:06  irfan
 * CSCdi46312:  Routing Protocol broadcast not sent to backup Async/PPP
 * interfaces
 * Leave Async interfaces as routing interfaces until the time they
 * are being brought up. At which time, we may mark them as
 * static_routing. Allows DDR to kick up the interface to send a
 * routing protocol update on an UP/UP Spoofing interface in async
 * backup configurations
 *
 * Revision 3.12  1996/01/04  22:15:09  lol
 * CSCdi43469:  per-user route wont use default interface
 *
 * Revision 3.11  1995/12/17  18:39:47  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.10  1995/12/13  22:57:58  lol
 * CSCdi39791:  access-lists applied by tacacs+ dont work when
 * fast-switched
 *
 * Revision 3.9  1995/12/09  00:43:57  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Make it easy to claim a tty for exclusive use by a process, and add a
 * modem_busyout registry.
 *
 * Revision 3.8  1995/12/06  18:33:03  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.7  1995/11/29  01:14:36  lol
 * CSCdi42554:  SLIPOFF is missing information when accounting vty-async
 *
 * Revision 3.6  1995/11/28  03:59:05  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.5  1995/11/22  15:24:47  irfan
 * CSCdi44030:  crash when async commands applied to group-async interface
 * Only config actions allowed for group-async idbs. Avoid real action
 * on group-async idbs.
 *
 * Revision 3.4  1995/11/19  06:56:33  syiu
 * CSCdi41498:  Async PPP Callback with autoselect during-login fails
 *
 * Revision 3.3  1995/11/17  18:54:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:45:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:39:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.24  1995/11/08  21:33:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.21  1995/10/04  06:32:41  irfan
 * CSCdi39542:  show buff indicates bad input/output interface for priv
 * pools
 * Async interfaces should initialize swidb->short_namestring
 *
 * Revision 2.20  1995/09/23  07:17:48  tkolar
 * CSCdi40082:  Dialer requires PPP subsystem hence also require CHAT
 * subsystem
 * Create registry for returning first member of an interface group
 *
 * Revision 2.19  1995/09/19  22:11:13  vandys
 * CSCdi40167:  Clear counter async misses a couple counters
 * Add a "clear counter" vector for async interfaces; clear the
 * line fields if present.
 *
 * Revision 2.18  1995/09/18  19:38:48  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.17  1995/09/07  20:32:22  tkolar
 * CSCdi39804:  group async is incompatible with dialer
 * Undo some collateral damage from CSCdi36407.
 *
 * Revision 2.16  1995/08/14  15:42:46  tkolar
 * CSCdi37485:  local timeout from tcp hdr compression
 * async_fastsend needs to set pak->if_output
 *
 * Revision 2.15  1995/08/08  20:50:32  lol
 * CSCdi37361:  AAA should not require auth for originating PPP connections
 *
 * Revision 2.14  1995/08/07  20:32:53  bdas
 * CSCdi38212:  Dialing reason may be incorrect in some call history mib
 * entries
 *
 * Revision 2.13  1995/08/04  07:38:12  irfan
 * CSCdi38094:  ip routing disabled on async/ppp
 * Include interfaces dialing-out when turning off static routing flag.
 *
 * Revision 2.12  1995/07/24  21:20:24  irfan
 * CSCdi36763:  memory corruption on 2511 with async-ppp
 * Ensure we have the correct 2430 channel setup before accessing
 * 2430 registers. Protect some critical areas from interrupts.
 * Clear pending non-rxservice input when going into interrupt
 * framing mode.
 *
 * Revision 2.11  1995/07/22  06:05:26  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Fix over-zealousness in limiting output of "Building Configuration..."
 *
 * Revision 2.10  1995/07/14  07:03:15  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.9  1995/07/04 01:59:03  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.8  1995/06/22  08:24:46  irfan
 * CSCdi36206:  Null pointer derefencing in dialers caused by chat scripts
 * dialer_fail() should not be called for completion of non-dialer scripts
 * on async lines.(regress)
 *
 * Revision 2.7  1995/06/21  03:30:03  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.6  1995/06/20  04:14:38  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.  (Should have been in initial commit,
 * but something got screwed up.)
 *
 * Revision 2.5  1995/06/19  22:03:20  syiu
 * CSCdi36092:  Async callback message misleading during ARAP callback
 *
 * Revision 2.4  1995/06/16  19:34:43  irfan
 * CSCdi31975:  Async default routing should work for dedicated mode
 * use "async default routing" flag to control routing on the dedicated
 * mode async interfaces. Further, Async interfaces were routing
 * all the time...fix it.
 *
 * Revision 2.3  1995/06/15  22:58:14  syiu
 * CSCdi35686:  Async callback AAA authorization and accounting phase not
 * working
 *
 * Revision 2.2  1995/06/09  13:21:57  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:09:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "fastswitch_registry.h"
#include "stacks.h"
#include "interface_private.h"
#include "snmp_interface.h"
#include "../snmp/ifmib_registry.h"
#include "packet.h"
#include "access.h"
#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "config.h"
#include "../if/ether.h"
#include "../if/network.h"
#include "aaa.h"
#include "../ui/debug.h"
#include "../wan/ppp.h"
#include "../wan/ppp_public.h"
#include "../ts/async_ppp.h"
#include "../if/async_debug.h"
#include "../ts/if_slip.h"
#include "../os/async.h"
#include "parser.h"
#include "parser_defs_slip.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_actions.h"
#include "if_async.h"
#include "../wan/dialer.h"
#include "../wan/dialer_private.h"
#include "media_registry.h"
#include "../wan/dialer_registry.h"
#include "../h/cs_registry.regh"
#include "../ui/setup.h"
#include "../ui/command.h"
#include "../ip/ip_registry.h"
#include "../parser/parser_defs_autoselect.h"
#include "../os/nv.h"
#include "../if/if_groups.h"
#include "../ip/ip_addrpool.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../iprouting/igrp.h"
#include "../wan/ppp_registry.h"
#include "../if/traffic_shape.h"

/*
 * Some global variables
 */
static int max_async_lines;
static ulonglong ttymask;
static int  asyncfs_fork;

/*
 * Forward declarations
 */
static void setup_async_ints(setup_struct *);
static void setup_async_ip(setup_struct *);
static process async_fs(void);
static void async_check_fs(hwidbtype *idb);
static void async_fastsend(hwidbtype *, paktype *);
static boolean async_ppp_vencap(paktype *, long);
static boolean async_ppp_setup(tt_soc *tty);
static void async_interfaceglobalsetup(setup_struct *);
void async_trigger_startoutput(hwidbtype *);

/*
 * async_framing_reset
 * Remove interrupt level async framing for async protocol
 */
static void async_framing_reset (tt_soc *tty)
{

    hwidbtype *hwidb = tty->tty_idb;

    switch (hwidb->enctype)
	{
	case ET_SLIP:
	    slip_disable(tty);
	    break;

	case ET_PPP:
	    appp_tty_shutdown(tty);
	    break;

	case ET_NULL:
	    /*
	     * due to the deencapping, we have lost the idb->enctype
	     * we will just reset both slip and ppp here. multiple
	     * resets of slip/ppp framing doesn't hurt.
	     */
	    slip_disable(tty);
	    appp_tty_shutdown(tty);
	    break;

	default:
	    if (async_framing_debug || async_state_debug)
		buginf("\n%s: Bad Encap %x on line TTY%t",
		       hwidb->hw_namestring, (uint) hwidb->enctype,
		       tty->ttynum);
	    return;
	}

    if (async_framing_debug)
	buginf("\n%s: Reset %s framing on TTY%t", hwidb->hw_namestring,
	       (hwidb->enctype)?encstring(hwidb->enctype):"",
	       tty->ttynum);

}
/*
 * async_framing_setup
 * Start interrupt level async framing for async protocol
 */
boolean async_framing_setup (tt_soc *tty)
{

    hwidbtype *hwidb = tty->tty_idb;
    boolean rc = FALSE;

    tty_clearbuffers(tty);    /* Clear any pending non rxservice input */
    switch (hwidb->enctype)
	{

	case ET_SLIP:
	    rc = slip_setup(tty);
	    break;

	case ET_PPP:
	    rc = async_ppp_setup(tty);
	    break;

	default:
	    if (async_framing_debug || async_state_debug)
		buginf("\n%s: Bad Encap %x on line TTY%t",
		       hwidb->hw_namestring, (uint) hwidb->enctype,
		       tty->ttynum);
	    return FALSE;
	}

    if (async_framing_debug)
	buginf("\n%s: %s %s framing on TTY%t", hwidb->hw_namestring,
	       (rc)?"Setup":"Failed to setup",
	       encstring(hwidb->enctype), tty->ttynum);
    return (rc);
}

/*
 * async_cleanup_int
 * Do common cleanup of async interface.
 * Called from async_reset and async_erase_idb
 * Note that if PPP callback is in progress, the callback process had issued a 
 * an interface reset (to get here) to do most of the chores except actually 
 * hanging up the line. The decision to hangup the line is deferred to the 
 * callback process.
 */

static void async_cleanup_int (hwidbtype *hwidb, idbtype *swidb, tt_soc *tty)
{
    leveltype status;
    ipaddrtype addr;

    if (swidb->ip_peer)
	addr = swidb->ip_peer->ippeer_addr;
    else
	addr = 0L;

    /*
     * We rightfully assume hwidb, swidb & tty have been sanity
     * checked by the two local callers.
     * The caller also needs to ensure proper timeout for
     * the Async device to detect a low DTR, when the line is
     * no-ModemControl, noModemDTR mode
     * Ideally this should be called only for "live" Async lines.
     */
    
    /* Signal Shutdown to tacacs */
    if (old_access_control) {
	/* 
	 * vty_async lines send their slipoffs from
	 * pt_run_vtyslip_ppp_connection 
	 */
	if (tty->tty_idb && (tty->tty_idb->type != IDBTYPE_VTY_ASYNC)) {
	    (void) reg_invoke_login_slipoff(tty, addr, tty->tty_idb, FALSE);
	}
    } else {	/* !old_access_control */
	/* Clean up per user values that might have been set. */
	idbtype *swidb;
	FOR_ALL_SWIDBS_ON_HW(tty->tty_idb, swidb) {
	    reg_invoke_ip_set_per_user_acl(swidb, IN, 
					   IP_ACL_USER_RESET);
	    reg_invoke_ip_set_per_user_acl(swidb, OUT, 
					   IP_ACL_USER_RESET);
	}
    }
    
    /*
     * The following code will remove any neighbor routes or
     * dynamic dialer maps that have been added.  We need to
     * do this now even though clear_ip_peer_addr() will do the
     * same thing because clear_ip_peer_addr() may not be called.
     */
    reg_invoke_ipcp_cleanup_ip_peer(swidb);

    /*
     * Return to default "ROUTING" state.
     * We will mark it as static_routing later as part
     * of bring-up (async_proto_mode) if need be.
     * Leaving these interfaces as ROUTING, enables DDR to kick in
     * for a routing protocol update packet. Helps in automatic and
     * immediate bring-up in Dial-backup cases.
     */
    swidb->static_routing = FALSE;

    /*
     * Return the ip address to pool except when callback is in progress.
     * The reincarnated callback connection will use that address. 
     */
    if (!((tty->acflags & ACF_CALLBACK_PPP) && (tty->statbits & DIALOUT)))
       reg_invoke_clear_ip_peer_addr(swidb);

    /*
     * Remove Async interrupt level framing
     * We may have removed it already, but it will not hurt
     */

    async_framing_reset(tty);

    /*
     * If PPP callback is in progress, avoid resetting the line
     */
    if (!((tty->acflags & ACF_CALLBACK_PPP) && (tty->statbits & DIALOUT))) {
       tty_reset(tty);		/* Clear out tty parameters */
       reg_invoke_death(tty);
    }

    status = raise_interrupt_level(NETS_DISABLE);
    hwidb->counters.resets++;
    holdq_reset(hwidb);		/* Flush the hold queue */
    reset_interrupt_level(status);

    /*
     * If PPP callback is in progress, avoid actually hanging up the 
     * line. Instead, flag that interface has been reset. The callback
     * process will pick up where it left off. 
     */
    if ((tty->acflags & ACF_CALLBACK_PPP) && (tty->statbits & DIALOUT)){
           tty->statbits &= ~SLIPMODE;
           return;
    }

    /*
     * If this line is using modem control, set the hangup flag
     * so that the modem control code will clean things up.
     * Otherwise, just clear the line out.
     */
    if (tty->capabilities & MODEMMASK) {
	if (tty->statbits & READY) {
	    /*
	     * Modem process thinks line is still in use
	     *    by some process/framing ?   We will ask
	     *    modem process to clean up for us.
	     */
	    if (modem_debug) {
		buginf("\n%s: asking modem_process to hangup TTY%t",
		       hwidb->hw_namestring, tty->ttynum);
	    }
	    tty->statbits |= HANGUP;
	    tty->statbits &= ~(READY + IDLE + CARDROP + RINGIN + AUTOBAUDING +
			       HASPROC + SLIPMODE + CALLBACK_IN_PROGRESS);
	} else {
	    /*
	     * Modem process is attempting to bounce the line back
	     * It's somewhere in the middle. Let it do its job, by
	     * not changing the modem state bits.
	     */
	    if (modem_debug) {
		buginf("\n%s: allowing modem_process to continue hangup",
		       hwidb->hw_namestring);
	    }
	    tty->statbits &= ~(HASPROC + CARDROP + SLIPMODE +
			       CALLBACK_IN_PROGRESS + AUTOBAUDING);
	}
    } else {
	tty->statbits |= READY;
	tty->statbits &= ~(HANGUP + IDLE + CARDROP + RINGIN + AUTOBAUDING +
			   HASPROC + SLIPMODE + CALLBACK_IN_PROGRESS);
	if ((tty->capabilities & MODEMDTR) == 0) {
	    if (modem_debug)
		ttybug(tty, "Async Int cleanup: Restoring DTR");
	    dtrstate(tty, TRUE);	/* Turn DTR back on */
	}
    }
}

/*
 * async_reset
 * Initialize an async interface.
 * Note that PPP callback will call this to reset the incoming async 
 * interface to do most of the chores as would a normal interface reset
 * except actually hanging up of the line. The actual line hangup decision
 * (not necessary if outgoing callback on the same incoming line) is 
 * deferred to the callback process.
 */

static void async_reset (hwidbtype *idb)
{
    tt_soc *tty;
    idbtype *swidb = firstsw_or_null(idb);

    /* NB: Not subinterface ready! */

    /* Some sanity checks */

    if ((NULL == idb) || (NULL == swidb))
	return;
    tty = idb->idb_tty;
    
    if (((IDBTYPE_ASYNC != idb->type) && (IDBTYPE_VTY_ASYNC != idb->type)) ||
	(NULL == tty))
	return;

    /*
     * Reset the bandwidth based on baud rate if we are still using
     * the default bandwidth, and set this as a new default bandwidth.
     */
    set_default_interface_bandwidth(swidb, (tty->tty_txspeed ?
                                            tty->tty_txspeed :
                                            ASYNC_DEFSPEED) / 1000);

    /*
     * Dialing in progress? 
     * Except when PPP callback is in progress, kill the line
     */
    if (tty->statbits & DIALOUT) { 
 	if (!(tty->acflags & ACF_CALLBACK_PPP)){
           tty->statbits |= CARDROP; /* Yes, kill the dialing */
  	   return;			/* And we're done here */
  	}
    }

    if (!(tty->statbits & SLIPMODE)) {	/* Not in async mode? */
	if (!TIMER_RUNNING(idb->serial_pulsetimer)) {
	    /*
	     * Attempt a restart, if a dedicated mode, non-modem Async line
	     *
	     * We get here when a 'no shut' is done after a 'shut' has taken
	     * us out of 'SLIPMODE'.  Thus async_reset() has been done once
	     * and does not need to be done again, but somehow we do need to
	     * get to async_proto_mode().
	     */
	    if ((tty->capabilities & MUSTSLIP) &&
		((tty->capabilities & MODEMMASK) == 0)) {
		async_proto_mode(tty);
	    }
	} else {
	    TIMER_STOP(idb->serial_pulsetimer); /* kill the timer that */
	}
	return;                             /* may call us back again */
    }

    if (TIMER_RUNNING(idb->serial_pulsetimer)) {
	if (SLEEPING(idb->serial_pulsetimer)) {
	    return;
	} else {
	    TIMER_STOP(idb->serial_pulsetimer);
	}
    } else {
	if (modem_debug)
	    ttybug(tty, "Async Int reset: Dropping DTR");
	dtrstate(tty, FALSE);	/* Ensure that DTR is low */

	if (idb->state != IDBS_ADMINDOWN) /* Set interface to reset state */
	    net_cstate(idb, IDBS_RESET);

	/* Note starting a timer with 0, will make it AWAKE 'now' */
	TIMER_START(idb->serial_pulsetimer, idb->serial_pulseinterval);
	return;
    }

    if (async_state_debug) {
	buginf("\n%s: Async protocol mode stopped", idb->hw_namestring);
	if (swidb->ip_peer)
	    buginf(" for %i", swidb->ip_peer->ippeer_addr);
    }

    /*
     * Common cleanup of Async interfaces
     */

    async_cleanup_int(idb, swidb, tty);

    /*
     * Restore the default encapsulation
     */

    if (idb->enctype != idb->enctype_cfg)
      async_new_encap(idb, idb->enctype_cfg);
    /*
     * Mark as a down interface
     */

    if (idb->state != IDBS_ADMINDOWN)
      net_cstate(idb, IDBS_DOWN);

    /*
     * Attempt a restart, if a dedicated mode, non-modem Async line
     */

    if ((tty->capabilities & MUSTSLIP) &&
	((tty->capabilities & MODEMMASK) == 0)) {
	async_proto_mode(tty);
    }

}

/*
 * Kill Async Interface capability from the tty line.
 * Called when Interface is removed. Also performs cleanups
 */

static void async_erase_idb (idbtype *swidb)
{
    tt_soc *tty;
    hwidbtype * hwidb = hwidb_or_null(swidb);

    if (hwidb == NULL)
	return;

    tty = hwidb->idb_tty;
    if ((IDBTYPE_ASYNC != hwidb->type) || (!tty))
	return;

    tty->capabilities &= ~(MUSTSLIP|MAYSLIP);
    tty->def_capabilities &= ~(MUSTSLIP|MAYSLIP);

    if (!(tty->statbits & SLIPMODE))    /* Nothing more if not in proto mode */
	return;

    if (tty->statbits & DIALOUT) { /* Dialing in progress? */
	tty->statbits |= CARDROP;  /* Yes, kill the dialing */
	return;                    /* And we're done here */
    }

    if (modem_debug)
	ttybug(tty, "Async Int removal: Dropping DTR");
    dtrstate(tty, FALSE);	/* Pulse DTR for PULSEINTERVAL */
    /*
     * This is the least amount of time we should wait so that the
     * down DTR can be noticed. This is going to block the parser,
     * but there is no easy workaround.
     */
    process_sleep_for(ASYNC_MIN_PULSEINTERVAL);

    async_cleanup_int(hwidb, swidb, tty);
}

/*
 * async_reset_tty - Reset an async interface, passing the TTY pointer
 *
 * This is a convenience routine for parts of the system that deal with
 * tty pointers and not IDB pointers.
 * 
 * Compare with tty_if_reset(tt_soc *) in os/ttysrv.c
 */

void async_reset_tty (tt_soc *tty)
{
    hwidbtype *idb;

    idb = tty->tty_idb;
    if (idb && ((IDBTYPE_ASYNC == idb->type) ||   /* Valid Async IDB ? */
		(IDBTYPE_VTY_ASYNC == idb->type))) {
	(*(idb->reset))(idb);	/* Reset the async IDB */
    }
}

/*
 * Start pending async output
 */

static void async_soutput (
    hwidbtype *idb)
{
    tt_soc *tty = (tt_soc *)idb->idb_tty;

    if (!tty) {
	buginf("\nasync_startoutput with null idb_tty");
	return;
    }

    if (!(tty->statbits & SLIPMODE))
      return;			/* Do nothing if line not running now */

    if (idb->enctype == ET_SLIP)
      startoutput(tty);
    else
      appp_startoutput(tty);

}


/*
 * async_ppp_iqueue - Handle a received packet
 */

static inline void async_ppp_iqueue (
     paktype *pak,
     tt_soc *tty)
{
    hwidbtype *idb;
    leveltype status;

    idb = tty->tty_idb; /* Get the IDB pointer */
    if (!idb) {
	datagram_done(pak);	/* Nothing to do if shutting down */
	return;
    }	

    GET_TIMESTAMP(idb->lastinput);
    pak->enctype = ET_PPP;	/* Set encapsulation type */
    pak->if_input = idb->firstsw;/* Set input interface */
    status = raise_interrupt_level(NETS_DISABLE); /* Disable net interrupts */
    slip_traffic.inpacks++;
    slip_traffic.inbytes += pak->datagramsize;

    /*
     *  Try to fastswitch IP or IPX packets.
     */
    pak->linktype = (*idb->getlink)(pak);
    if (dialer_async_classify(idb, pak))
	TIMER_START(tty->dallytimer, tty->tty_sessiontimeout);
    /*
     *  Test fastswitch on the input interface before fastswitching
     *    to prevent fastswitching from the AUX port.  fastokay is
     *    always false on the AUX port
     */
    switch(pak->linktype) {
      case LINK_IP:
	if (reg_invoke_ppp_from_process_fs(RXTYPE_DODIP, pak)) {
	    reset_interrupt_level(status);
	    return;
	}
	break;   
      case LINK_NOVELL:
	if (reg_invoke_ppp_from_process_fs(RXTYPE_NOVELL1, pak)) {
	    reset_interrupt_level(status);
	    return;
	}
	break;
    }
    
    idb->counters.rx_cumbytes += pak->datagramsize;
    idb->counters.inputs++;
    (*(idb->iqueue)) (idb, pak); /* Enqueue the packet */
    /*
     * Check if this affects idle time AFTER iqueue has set
     * pak->linktype appropriately.  In theory, the packet is
     * GONE, but in reality it's still on some queue somewhere.
     * (even if the packet was fastswitched)
     */
    if (dialer_async_classify(idb, pak))
	TIMER_START(tty->dallytimer, tty->tty_sessiontimeout);/*Note not idle*/

    reset_interrupt_level(status);		/* Lower priority */
}

/*
 * async_ppp_setup - Set up a line for async PPP
 */

static boolean async_ppp_setup (tt_soc *tty)
{
    uchar ch;
    hwidbtype *idb;
    int mru;

    idb = tty->tty_idb;

    /*
     * A PPP link must always be able to receive at least 1500 bytes,
     * so make sure we are always in receive for at least that amount.
     */
    mru = max(idb->maxmtu, DEFMRU);

    if (appp_tty_startup(tty, async_ppp_iqueue,
			 PPP_ENCAPBYTES,
			 mru + PPP_ENCAPBYTES,
			 idb)) {

	for (ch = 0; ch < 0x20; ch++)
	  appp_add_quoted_char(tty, ch); /* Set initial escape map */
	return(TRUE);
    }
    if (tty == stdio)
	printf("%%Unable to start async PPP: %s", nomemory);
    return(FALSE);
}

/*
 * async_setmap - Set the control character map
 *
 * This routine is used to set the control character map. We are
 * passed a map as a 32-bit integer, with bit zero meaning character 0,
 * and bit 31 meaning character 31.
 *
 * If a bit is set in the map, we quote it on transmission. If the bit
 * is not set, we don't quote it.
 *
 */

void async_setmap (hwidbtype *idb, ulong map)
{
    tt_soc *tty;
    int ch;

    if (idb->enctype != ET_PPP)
      return;			/* Nothing to do if not PPP */

    tty = idb->idb_tty;		/* Get the TTY pointer */

    if (!tty)
      return;			/* Nothing to do if no tty pointer */

    for (ch = 0; ch < 0x20; ch++) {
	if (map & (1L << ch))
	  appp_add_quoted_char(tty, ch);
	else
	  appp_remove_quoted_char(tty, ch);
    }

    /*
     * Update driver support, if any.  The async HDLC standard for PPP
     * requires distinct TX and RX maps, but we don't do that (yet).
     * Thus, we send with the given map and receive with the most
     * generous one.
     */
    tty_pppmap(tty, map, 0L);
}

/*
 * async_new_encap - Set a new encapsulation
 *
 */

void async_new_encap (hwidbtype *idb, encapstype encap)
{
    lcp_options *wo;
    int saved_neg_auth = 0;
    uchar saved_authtypes[PPP_AUTH_NUM] = { 0 };
    boolean saved_params = FALSE;

    if ((encap == ET_PPP) && (idb->lcpfsm != NULL) && 
	(idb->lcpfsm->wantoptions != NULL)) { 
	saved_params = TRUE;
	wo = (lcp_options *)idb->lcpfsm->wantoptions;
	saved_neg_auth = wo->neg_auth;
	bcopy(wo->authtypes, saved_authtypes, PPP_AUTH_NUM);
    }

    if (idb->deencap)
      (*idb->deencap)(idb);	/* Shut down current encapsulation */

    switch (encap) {
      case ET_SLIP:
	slip_encap_setup(NULL, idb);
	break;

      case ET_PPP:
	ppp_setup(NULL, idb);

	/*
	 * replace normal ppp encapsulate routines with ones that
	 * also adjust our idle time based on dialer-group
	 */
	idb->vencap = async_ppp_vencap;
	if ((saved_params) && (idb->lcpfsm != NULL) && 
	    (idb->lcpfsm->wantoptions != NULL)) { 
	    wo = (lcp_options *)idb->lcpfsm->wantoptions;
	    wo->neg_auth = saved_neg_auth;
	    bcopy(saved_authtypes, wo->authtypes, PPP_AUTH_NUM);
	}
	break;

      case ET_BSTUN:
         if (reg_invoke_media_serial_setup(encap, NULL, idb))
            break;

      default:
	if (async_framing_debug || async_state_debug)
	    buginf("\n%s: Bad Encap %x on line TTY%t",
		   idb->hw_namestring, (uint) idb->enctype,
		   idb->idb_tty->ttynum);
	return;			/* Unknown encapsulation */
    }
    if (async_state_debug)
	buginf("\n%s: Set up %s encapsulation on TTY%t",
	       idb->hw_namestring, encstring(idb->enctype),
	       idb->idb_tty->ttynum);
    reg_invoke_media_serial_encap_changed(idb, encap);
}

/*
 * async_setencap - Interface to encapsulation command for async interfaces
 */
static void async_setencap (parseinfo *csb)
{
    hwidbtype *idb;
    tt_soc *tty;
    int enctype;
    leveltype level;

    idb = csb->interface->hwptr;       /* Get interface pointer */

    /*
     * Shouldn't allow configuring encap on rotary group members
     */
    if (reg_invoke_dialer_huntgroup_member(idb)) {
	printf("\n%% Cannot change encapsulation of rotary group member");
	return;
    }

    enctype = GETOBJ(int,1);

    if (enctype == idb->enctype)
	return;			/* Nothing to do */

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * For group interfaces, just set the correct
     * encap and quit. Members will be dealt with
     * separately.
     */
    if (IDBTYPE_ASYNC_GROUP == idb->type) {
	async_new_encap(idb, enctype);
	reset_interrupt_level(level);
	return;
    }

    tty = idb->idb_tty;		/* Get tty pointer */

    if (tty->statbits & SLIPMODE) {      /* Using SLIP mode now? */
	async_framing_reset(tty);        /* Reset current framing */
	(*(idb->reset))(idb);	         /* Reset the async IDB */
    }

    async_new_encap(idb, enctype); /* Set the desired encapsulation */

    /*
     * If the line is set to SLIP dedicated, and this is a hard-wired
     * line, turn SLIP on.
     */

    if ((tty->capabilities & MUSTSLIP) &&
	((tty->capabilities & MODEMMASK) == 0))
      async_proto_mode(tty);

    reset_interrupt_level(level);
}

/*
 * async_set_maxdgram - Set the maximum datagram size
 */

static boolean async_set_maxdgram (
     hwidbtype *idb,
     int buffersize,
     int maxdgram)
{
    return(FALSE);		/* Not supported yet */
}

/*
 * async_shutdown
 * Shut down an async interface.  note that this can be quite compicated,
 * since it may involve hanging up the phone, returning the line to a
 * non-interface mode (eg for terminal server use), etc. 
 */
static void async_shutdown (hwidbtype *idb, int newstate)
{
    idb->state = newstate;
    async_reset(idb);
}

/*
 * find_async_group_leader -- given an async idb, return its 
 *      			group leader if any
 */
hwidbtype *find_async_group_leader (hwidbtype *async_idb)
{
    hwidbtype *hwidb;
    async_member_info *member;
    int queue_size, count;
    queuetype *memberQ;

#ifdef DEBUGGROUP
    buginf("\nFindGroupLeader %s", async_idb->hw_namestring);
#endif
    FOR_ALL_HWIDBS(hwidb) {
        if (hwidb->type == IDBTYPE_ASYNC_GROUP) {
	    /*
	     * If we find a async group interface, walk through its
	     *   member queue, comparing against our target.  If we find
	     *   a match, return the group leader idb.
	     */
	    memberQ = &hwidb->firstsw->async_groupQ;
	    queue_size = QUEUESIZE(memberQ);
            member = memberQ->qhead;
	    for (count = 0; member && (count < queue_size); count++) {
		if (member->hwidb == async_idb)
                	return(hwidb);
                member = member->next;
	    }
	}
    }
    return(NULL);

}

/*
 * async_config_changed
 *
 * This function is called from the parser every time a command is 
 *   executed.  If the command refers to an async group leader idb,
 *   the groupQ is walked and the command is executed against
 *   each member idb.
 */
static void async_config_changed (parseinfo *csb)
{
    hwidbtype *hwidb;
    idbtype *original_idb, *member_idb;
    async_member_info *member;
    int queue_size, count;
    queuetype *memberQ;


    if ((hwidb = hwidb_or_null(csb->interface))) {
        if (hwidb->type == IDBTYPE_ASYNC_GROUP) {
    	    original_idb = csb->interface;
	    memberQ = &hwidb->firstsw->async_groupQ;
	    queue_size = QUEUESIZE(memberQ);
            member = memberQ->qhead;
	    for (count = 0; member && (count < queue_size); count++) {
		member_idb = firstsw_or_null(member->hwidb);
		/* 
		 * If the function is _not_ an async group command then
		 *  execute it against the member idb.  If it's an async
		 *  group command then it would never be executed against
		 *  an async member anyways.
		 */
		if ((member_idb) && 
		    (csb->action_func != async_group_config_command)){
		    csb->interface = member_idb;
		    csb->action_func(csb);
		}
                member = member->next;
	    }
            csb->interface = original_idb;
	}
    }
}



/*
 * async_default_keepperiod
 * the default keepperiod for async (slip/ppp) encapsulations.
 */
static boolean async_default_keepperiod (hwidbtype *hwidb, short *keepperiod)
{
    if (hwidb && (hwidb->status & IDB_ASYNC)) {
	*keepperiod = (ASYNC_DEF_KEEPPERIOD);  /* Our default is none */
	return (TRUE);
    }
    return (FALSE);
}

/*
 * async_periodic - Perform periodic async functions
 *
 * This routine performs the once a second async functions. Currently,
 * this just consists of checking session timeouts.
 */

static void async_periodic (hwidbtype *idb)
{
    tt_soc *tty = idb->idb_tty;

    if (!(tty->statbits & SLIPMODE))
      return;			/* Nothing to do if not in SLIP mode */

    /*
     * Nothing to do if PPP callback is in progress
     */
    if ((tty->acflags & ACF_CALLBACK_PPP) && (tty->statbits & DIALOUT))
      return;

    /*
     * if a session-timeout has occured, stop the timer
     * and trigger the cleanup of the interface.
     */
    if (tty->tty_sessiontimeout &&
	TIMER_RUNNING_AND_AWAKE(tty->dallytimer)) {

	TIMER_STOP(tty->dallytimer);
	if (async_state_debug)
	    buginf("\n%s: Protocol mode idle timeout", idb->hw_namestring);

	if (tty->capabilities & MODEMMASK) {
	      tty->statbits |= HANGUP; /* Hang up the line! */
	      tty->statbits &= ~(READY | IDLE | RINGIN);
	} else
	    async_reset_tty(tty);
    }
}

/*
 * async_clear_counters - clear async portion of counters
 */
static void
async_clear_counters (hwidbtype *hwidb)
{
    tt_soc *tty;

    tty = hwidb->idb_tty;
    if (tty == NULL) {
	return;
    }
    tty->totalin = 0;
    tty->totalout = 0;
    tty->linenoise = 0;
    tty->overrun = 0;
    tty->overflow = 0;
    tty->usagecount = 0;
}


/*
 * async_group_clear_counters - clear async portion of counters for all
 * members of a group
 */
static void
async_group_clear_counters (hwidbtype *gr_hwidb)
{
    async_member_info *member;
    int queue_size, count;
    queuetype *memberQ;
 
    if (gr_hwidb->type == IDBTYPE_ASYNC_GROUP) {
        /*   walk through its
         *   member queue, and clear all their counters as well
         */
        memberQ = &gr_hwidb->firstsw->async_groupQ;
        queue_size = QUEUESIZE(memberQ);
        member = memberQ->qhead;
        for (count = 0; member && (count < queue_size); count++) {
            reg_invoke_consolidate_counters(member->hwidb);
            clear_idb(member->hwidb);
            if (member->hwidb->clear_support) {
                (*member->hwidb->clear_support)(member->hwidb);
            }
            member = member->next;
        }
    }
    return;
}



/*
 * init_async_idb_common()
 * Init the common fields in the idb. Enqueue the IDB in the hardware
 * IDB queue if qflag is TRUE.
 */
static hwidbtype *init_async_idb_common (hwidbtype *hwidb, boolean qflag)
{
    idbtype *swidb;
    char buffer[30];

    nnets++;			/* New net created */
    swidb = hwidb->firstsw;
    create_serial_sb(hwidb);

    /*
     * Set up generic driver information
     */
    hwidb->status = IDB_ASYNC | IDB_SERIAL;
    hwidb->state = IDBS_DOWN;
    GET_TIMESTAMP(hwidb->state_time);
    hwidb->hw_if_index = idb_get_next_hw_index(); /* SNMP needs this */
    if (hwidb->type != IDBTYPE_ASYNC_GROUP) {
	hwidb->snmp_if_index = swidb->snmp_if_index =
	    reg_invoke_ifmib_register_hwidb(hwidb);
    }
    hwidb->vc = NO_VC;
    hwidb->reliability = 255;
    hwidb->load = 1;
    holdq_init(hwidb, ASYNC_HOLDQ);

    sprintf(buffer, "%s%d", hwidb->name, hwidb->unit);
    setstring(&swidb->namestring, buffer);
    hwidb->hw_namestring = swidb->namestring;

    sprintf(buffer, "%c%c%d", hwidb->name[0], hwidb->name[1], hwidb->unit);
    setstring(&swidb->short_namestring, buffer);
    hwidb->hw_short_namestring = swidb->short_namestring;

    /*
     * Set up function calls
     */
    hwidb->setencap = async_setencap; /* Handle "encapsulation" command */
    hwidb->soutput = async_soutput; /* Start pending output */
    hwidb->reset = async_reset;
    hwidb->set_maxdgram = async_set_maxdgram;
    hwidb->device_periodic = async_periodic;
    hwidb->linestate = (linestate_t) return_true;
    hwidb->shutdown = async_shutdown;
    hwidb->fastsend = async_fastsend;

    /*
     * Encapsulation-specific vectors
     */
    hwidb->enctype_def = hwidb->enctype_cfg = ET_SLIP;
				/* Default encapsulation is SLIP */
    slip_encap_setup(NULL, hwidb);	/* Set up the IDB */

    /*
     * Other stuff not yet cleaned up
     */
    hwidb->input_defqcount = INPUTQ_DEFAULT;
    async_default_keepperiod(hwidb, &hwidb->keep_period);
    if (!hwidb->keep_period) {
	hwidb->nokeepalive = TRUE;
    }
    hwidb->maxmtu = hwidb->maxmtu_def = MAXETHERSIZE; /* set the maximum MTU */
    hwidb->firstsw->sub_mtu = hwidb->maxmtu_def;

    hwidb->max_pak_size_def = MAXETHERSIZE;
    if_maxdgram_adjust(hwidb, MAXETHERSIZE);
    hwidb->buffersize = MAXETHERSIZE;

    /*
     * Initialize protocol state
     */
    reg_invoke_sw_idb_init(swidb, TRUE);
    serial_idb_enqueue(hwidb);
    if (qflag)
        idb_enqueue(hwidb);

    return(hwidb);
}


/*
 * init_async_group_idb - Get an async IDB to use as a group leader
 */

static hwidbtype *init_async_group_idb (uint unit)
{
    hwidbtype *hwidb;
    idbtype *swidb;

#ifdef DEBUGGROUP
    buginf("\ninit_async_group_idb %d");
#endif
    /* 
     *  If the idb already exists, return it;
     */
    FOR_ALL_HWIDBS(hwidb) {
	if ((hwidb->type == IDBTYPE_ASYNC_GROUP) && (hwidb->unit == unit))
	    return(hwidb);
    }

    /*
     * We're going to have to make one 
     */

    /*
     * Set up storage, add IDB to interface queue
     */
    hwidb = idb_create();
    if (!hwidb)
      return(NULL);

    swidb = hwidb->firstsw;

    /*
     * Set up generic driver information
     */

    swidb->async_group_leader = TRUE;

    hwidb->type = IDBTYPE_ASYNC_GROUP;
    hwidb->typestring = "Async Group Serial";
    hwidb->name = IFNAME_ASYNC_GROUP;	/* set preferred device name */

    /*
     * Allocate a TTY structure to hold 'async xxx' command configs.
     * Initialize the TTY vectors to something safe because none of the
     * functions that dereference the vectors, check for their existence
     * first.  They assume that if the TTY exists, then the vectors are
     * correctly set.
     */
    hwidb->idb_tty = malloc_named(sizeof(tt_soc), "Async Group TTY");
    if (hwidb->idb_tty) {
	tt_soc *tty = hwidb->idb_tty;
	tty->ivector = tty->ovector = tty->nvector = &null_io_vector;
    }

    hwidb->slotunit = hwidb->unit = unit;

    set_default_interface_bandwidth(swidb, BANDWIDTH_SCALE/ 10000);
    set_default_interface_delay(swidb, 10000);


    /*
     * Our Async specific DTR pulse time
     */
    hwidb->serial_pulseinterval = ASYNC_DEFAULT_PULSEINTERVAL;
    hwidb->serial_pulseinterval_def = ASYNC_DEFAULT_PULSEINTERVAL;

    /*
     * Set up function calls
     */
    hwidb->clear_support = async_group_clear_counters;


    init_async_idb_common(hwidb, TRUE);
    /*
     * default to sync/async.  In NVRAM, the "physical-layer xxx" command
     * shows up long before the "group-range" command, and we want to
     * accept it.  We'll remove it later (so "physical-later" doesn't
     * get generated for real async interfaces.
     */
    swidb->hwptr->serial_flags |= SERIAL_FLAGS_SYNCASYNC;

    return(hwidb);
}
/*
 * init_async_idb - Get an async IDB for a TTY
 * Queue it in the hwidbQ if qflag is TRUE.
 */

hwidbtype *init_async_idb (tt_soc *tty, boolean qflag)
{
    hwidbtype *hwidb;
    idbtype *swidb;

    hwidb = tty->tty_idb;	/* See if we already have an idb pointer */
    if (hwidb)
      return(hwidb);		/* Yes, return it */

    /*
     * Add buffers to the system for our usage
     */

    /*
     * Set up storage, add IDB to interface queue
     */
    hwidb = idb_create();
    if (!hwidb)
      return(NULL);

    swidb = hwidb->firstsw;

    /*
     *  THINGS THAT SHOULD BE DESTROYED WITH THE INTERFACE
     */
    swidb->async_member_differences = 
		malloc(sizeof(async_member_info));

    /*
     * Set up generic driver information
     */

    if (tty->type == VTY_LINE) {
	hwidb->type = IDBTYPE_VTY_ASYNC;
	hwidb->typestring = "Virtual Async Serial";
	hwidb->name = IFNAME_VTY_ASYNC;
    }
    else {
	hwidb->type = IDBTYPE_ASYNC;
	hwidb->typestring = "Async Serial";
	hwidb->name = IFNAME_ASYNC;		/* set preferred device name */
    }

    hwidb->idb_tty = tty;
    tty->tty_idb = hwidb;

    hwidb->slotunit = hwidb->unit = tty->ttynum;

    set_default_interface_bandwidth(swidb, (tty->tty_txspeed ?
					    tty->tty_txspeed :
					    ASYNC_DEFSPEED) / 1000);
    set_default_interface_delay(swidb, ASYNC_IGRP_DELAY);
    swidb->static_routing = FALSE;

    swidb->async_member_differences->hwidb = hwidb;

    /*
     * Set up function calls
     */
    hwidb->clear_support = async_clear_counters;

    /*
     * Our Async specific DTR pulse time
     */
    hwidb->serial_pulseinterval = hwidb->serial_pulseinterval_def =
	tty->dtr_pulseinterval;

    init_async_idb_common(hwidb, qflag);

    /*
     * Initialize any modem hardware management information
     */
    reg_invoke_modem_async_idb_inits(hwidb);
 
    return(hwidb);
}

/*
 * async_group_createidb
 */
static idbtype *async_group_createidb (uint unit)
{
    hwidbtype *idb;

    if (unit <MAXLINES) { 
	idb = init_async_group_idb(unit);
	if (idb) {
            reg_invoke_modem_group(idb);
	    return(idb->firstsw);
	} else {
	    printf(nomemory);
	    return(NULL);
	}
    }
    printf("\n%%Unit number must be less than %d", MAXLINES);
    return(NULL);
}
/*
 * async_createidb
 * Create an IDB on the fly when it is first referenced by an 
 * "interface async N" command.  We need to do this since the interface
 * commands will all appear in the config files before line commands, which
 * are where the system should really allocated IDBs for async ports.
 * It is assumed that the called has already parsed the unit number.
 */
static idbtype *async_createidb (uint unit)
{
    tt_soc *tty;
    hwidbtype *idb;

    if (unit < MAXLINES) {
	tty = MODEMS[unit];
	if (tty && ((tty->type == TTY_LINE) || (tty->type == AUX_LINE))) {
	    idb = init_async_idb(tty, TRUE);
	    if (idb) {
		return(idb->firstsw);
	    } else {
		printf(nomemory);
		return(NULL);
	    }
	} else {
	    printf("\n%%Line %d is not a TTY line", unit);
	    return(NULL);
	}
    }
    printf("\n%%Unit number must be less than %d", MAXLINES);
    return(NULL);
}

static boolean async_group_parser_createidb (uint64 *ifcode, parseinfo *csb)
{
    if (*ifcode == IFTYPE_ASYNC_GROUP) {
	csb->createidb = async_group_createidb;
	return(TRUE);
    }
    return(FALSE);
}
static boolean async_parser_createidb (uint64 *ifcode, parseinfo *csb)
{
    if (*ifcode == IFTYPE_ASYNC) {
	csb->createidb = async_createidb;
	return(TRUE);
    }
    return(FALSE);
}


/*
 * find_async_group_range
 */
static void find_async_group_range (queuetype *memberQ, int *low, int *high)
{
    idbtype *member_idb;
    async_member_info *member;
    int queue_size, count;

    *low = 1000; 
    *high = 0;

    queue_size = QUEUESIZE(memberQ);
    member = memberQ->qhead;
    for (count = 0; member && (count < queue_size); count++) {
	member_idb = firstsw_or_null(member->hwidb);
	if (member_idb) {
	   if (member_idb->hwptr->unit < *low) 
		*low = member_idb->hwptr->unit;
	   if (member_idb->hwptr->unit > *high) 
		*high = member_idb->hwptr->unit;
	    
	}
        member = member->next;
    }
}

static void async_group_first_member (idbtype *idb, hwidbtype **first_member)
{
    async_member_info *member;

#ifdef DEBUGGROUP
    buginf("\ngroup_first_member %s", idb->namestring);
#endif
    *first_member = NULL;
    if (QUEUESIZE(&idb->async_groupQ)) {
	member = peekqueuehead(&idb->async_groupQ);
	*first_member = member->hwidb;
    }

}

/*
 * async_group_config_command - Handle async group interface configuration
 */
void async_group_config_command (parseinfo *csb)
{

    idbtype *swidb;
    hwidbtype *idb, *group_leader_hwidb;
    async_member_info *member;
    int start, end, i, unit, oldstart=10000, oldend=0;
    uchar *config_buf;
    uint config_bufsize;
    uint nv_size;
    char *ifname;

    if (csb->nvgen) {
	/* if there isn't an async_group here then go away */
	if (!QUEUESIZE(&csb->interface->async_groupQ))
	    return;

	find_async_group_range(&csb->interface->async_groupQ, &start, &end);


	switch(csb->which) {
	  case ASYNC_GROUP_RANGE:
	    nv_write(TRUE, "group-range %d %d", start, end);
	    break;
	  case ASYNC_GROUP:
	    /* cycle through all our members, printing "member [n] command" */
	    for (i = start; i <= end; i++) {
		swidb = MODEMS[i]->tty_idb->firstsw;
		if ((swidb->async_member_differences->difference_mask 
			& GROUPMASK_DEFAULT_IP_ADDRESS) && swidb->ip_peer) {
		    nv_write(swidb->ip_peer->ippeer_addr_def, 
			"member %d peer default ip address %i", 
				i, swidb->ip_peer->ippeer_addr_def);
		}
		if (swidb->async_member_differences->difference_mask 
			& GROUPMASK_DESCRIPTION) {
		    nv_write((swidb->description != NULL),
		     "member %d description %s", i, swidb->description);
		}
	    }
	    break;
	  default:
	    buginf("NVGEN UNKNOWN");
	    break;
	}

	return;
    }



    switch(csb->which) {
      case ASYNC_GROUP_RANGE:

	swidb = csb->interface;

	if (!csb->sense) {
	    /* clean out the async_groupQ, leaving individual interface
		differences intact */
	    while ((member = dequeue(&swidb->async_groupQ))) {
		member->next = NULL;
		member->hwidb->status &= ~(IDB_NO_NVGEN); 
	    }
	    /* that's it */
	    break;
	}

        if (QUEUESIZE(&csb->interface->async_groupQ) > 0) {
           /* Queue already existed.  We don't want to reinitialize
               interfaces that were already members */
           find_async_group_range(&csb->interface->async_groupQ,
               &oldstart, &oldend);
        }

	/* temporary until we get parser ranges working */
	start = GETOBJ(int,1);
	end = GETOBJ(int,2);

        if ((start == oldstart) && (end == oldend)) {
            /* ranges match, no reason to do anything */
            return;
        }

	if (start > end) {
	    printf("Beginning of range must be less than end.\n");
	    return;
	}

	if (start < 1) {
	    printf("Range must start with interface Async 1 or higher.\n");
	    return;
	}

	if (end > nttylines) {
	    printf("Range end must be less than or equal to %d\n", nttylines);
	    return;
	}

	/* 
	 * create the async interfaces if they don't exist 
	 */
	for (i = start; i <= end; i++) {
	    if (MODEMS[i]->tty_idb == NULL) {
		async_createidb(i);
	    } 
	}

	/* 
	 * now see if the interfaces are already member of another 
	 * idb group
	 */
	for (i = start; i <= end; i++) {
	    group_leader_hwidb = find_async_group_leader(MODEMS[i]->tty_idb);
	    if ((group_leader_hwidb) && (group_leader_hwidb != swidb->hwptr)) {
		buginf("\nAsync interface %d already belongs to" 
			" Async-Group %d", i, group_leader_hwidb->unit);
		return;
	    }
	}

	/*
	 * Blow away the existing configuration on the members if 
	 *  necessary.  "If necessary" is defined as "If any of them
	 *  existed before we started this config command".
	 */
	config_bufsize = nv_get_config_size();
	nv_size = config_bufsize;
	while (((config_buf = malloc(nv_size)) == NULL) 
		  && (nv_size > 0x3fff)){
	    nv_size = nv_size>>1;
	}

	if (config_buf == NULL) {
	    buginf("couldn't get buffer");
	    return;
	}

	/*
	 * check whether these are dual-mode (sync/async) interfaces.
	 * If so, we'll want the group interface to be dual mode as well.
	 * We'll also need to use the appropriate name...
	 */
	if ((MODEMS[start]->tty_idb->serial_flags &
	    SERIAL_FLAGS_SYNCASYNC) == 0 )
	    swidb->hwptr->serial_flags &= ~SERIAL_FLAGS_SYNCASYNC;
	ifname = MODEMS[start]->tty_idb->name;

	if (nv_current_to_buffer(config_buf, nv_size, TRUE,
				 PRIV_ROOT, FALSE)) {
	    unconfigure_interfaces(config_buf, nv_size, ifname,
					start, end, oldstart, oldend);
	}

	/* 
	 * If this group interface already has a list, clear it.
	 *   Leave the differences on the individuals IDBs intact.
	 */
	while ((member = dequeue(&swidb->async_groupQ))) {
	    member->next = NULL;
	    member->hwidb->status &= ~(IDB_NO_NVGEN); 
	}

	/* 
	 * make the new list 
	 */
	for (i = start; i <= end; i++) {
	    idb = MODEMS[i]->tty_idb;
	    idb->status |= IDB_NO_NVGEN; 
	    enqueue(&swidb->async_groupQ, 
		idb->firstsw->async_member_differences);
	}

	/*
	 * bring everyone up to date with the group leader
	 */
	sync_to_master_interface(config_buf, nv_size, 
				  IFNAME_ASYNC_GROUP, swidb->hwptr->unit,
				  ifname, start, end, oldstart, oldend);

   	break;

      case GROUP_DESCRIPTION:
	unit = GETOBJ(int, 22);
	find_async_group_range(&csb->interface->async_groupQ, &start, &end);
	if ((unit < start) || (unit > end)) {
	    printf("Async interface %d is not a member of Async-Group %d\n",
			unit, csb->interface->hwptr->unit);
	    return;
	}

	member = MODEMS[unit]->tty_idb->firstsw->async_member_differences;
	if (csb->sense) {
	   member->difference_mask |= GROUPMASK_DESCRIPTION;
	} else {
	   member->difference_mask &= ~GROUPMASK_DESCRIPTION;
	}
	
	swidb = csb->interface;
	csb->interface = MODEMS[unit]->tty_idb->firstsw;
	csb->which = 0;
	description_command(csb);
	csb->which = GROUP_DESCRIPTION;
	csb->interface = swidb;
	break;
      case GROUP_PEER_DEFAULT_IP_ADDRESS:
        unit = GETOBJ(int, 22);
	find_async_group_range(&csb->interface->async_groupQ, &start, &end);
	if ((unit < start) || (unit > end)) {
	    printf("Async interface %d is not a member of Async-Group %d\n",
			unit, csb->interface->hwptr->unit);
	    return;
	} 

	member = MODEMS[unit]->tty_idb->firstsw->async_member_differences;
	if (csb->sense) {
	   member->difference_mask |= GROUPMASK_DEFAULT_IP_ADDRESS;
	} else {
	   member->difference_mask &= ~GROUPMASK_DEFAULT_IP_ADDRESS;
	}

	swidb = csb->interface;
	csb->interface = MODEMS[unit]->tty_idb->firstsw;
	csb->which = PEER_ADDR_CONFIG;
	peer_config_command(csb);
	csb->which = GROUP_PEER_DEFAULT_IP_ADDRESS;
	csb->interface = swidb;
	break;
      default:
	buginf("\nUnknown command %d in async_group_config_command",
			csb->which);
    } 
}

/*
 * async_group_physical_la<yer
 * handle "physical layer" command for group-async commands.  This doesn't
 * actually DO anything, but it makes it possible for the group-async command
 * to specify "physical-layer async" for dual-mode interfaces...
 */
boolean
async_group_physical_layer (hwidbtype *idb, parseinfo *csb, int type)
{
    if (idb->type == IDBTYPE_ASYNC_GROUP && (type == PHY_LAYER_ASYNC ||
					    type == PHY_LAYER_SYNC))
	return(TRUE);
    else
	return(FALSE);	
}


/*
 * async_config_command - Handle async specific line configuration
 */

void async_config_command (parseinfo *csb)
{
    idbtype *idb;
    hwidbtype *hwidb;
    tt_soc *tty;
    dialerdbtype *ddb;

    idb = csb->interface;               /* Get Software IDB */
    hwidb = idb->hwptr;                 /* Get Hardware IDB */
    tty = idb->hwptr->idb_tty;          /* Get TTY pointer */

    if (csb->nvgen) {
	switch (csb->which) {

	  case ASYNC_MODE_INT:
	    nv_write(tty->def_capabilities & MAYSLIP, "%s", csb->nv_command);
	    break;

	  case ASYNC_MODE_DED:
	    nv_write(tty->def_capabilities & MUSTSLIP, "%s", csb->nv_command);
	    break;

	  case ASYNC_MODE_NONE:
	    break;

	  case ASYNC_DEFAULT_ROUTING:
	    nv_write(tty->slip_flags & SLIP_DEF_ROUTING, "%s",
		     csb->nv_command);
	    break;

	  case ASYNC_DYNAMIC_ROUTING:
	    nv_write(tty->slip_flags & SLIP_DYN_ROUTING, "%s",
		     csb->nv_command);
	    break;

	  case ASYNC_DYNAMIC_ADDRESS:
	    nv_write(tty->slip_flags & SLIP_DYNAMIC, "%s", csb->nv_command);
	    break;

	  case ASYNC_DEFAULT_IP_ADDRESS:
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {

      case ASYNC_MODE_INT:

	/*
	 * If the line was previously set async mode dedicated, then
	 * reset the line state now.
	 */
	  
	if (tty->capabilities & MUSTSLIP) {
	    tty->def_capabilities &= ~MUSTSLIP;
	    tty->capabilities &= ~MUSTSLIP;
	    if ((tty->capabilities & MODEMMASK) == 0 &&
		(tty->statbits & SLIPMODE)) {
		async_reset_tty(tty);
	    }
	}

	/*
	 * Mark the line as async mode interactive
	 */

	tty->def_capabilities |= MAYSLIP;
	tty->capabilities |= MAYSLIP;

	break;

      case ASYNC_MODE_DED:

	/*
	 * Mark the line as async mode dedicated
	 */

	tty->def_capabilities &= ~MAYSLIP;
	tty->capabilities &= ~MAYSLIP;
	tty->def_capabilities |= MUSTSLIP;
	tty->capabilities |= MUSTSLIP;

	/*
	 * If this is a hard-wired line, set into protocol mode now
	 */

	if ((tty->capabilities & MODEMMASK) == 0)
	    async_proto_mode(tty);
	break;

      case ASYNC_MODE_NONE:

	/*
	 * Turn off both async mode bits
	 */

	tty->capabilities &= ~(MUSTSLIP|MAYSLIP);
	tty->def_capabilities &= ~(MUSTSLIP|MAYSLIP);

	/*
	 * If this is not a modem control line, take the line out of
	 * protocol mode.
	 */
	if (tty->statbits & SLIPMODE && !(tty->capabilities & MODEMMASK))
	    async_reset_tty(tty);
	break;

      case ASYNC_DEFAULT_ROUTING:

	/*
	 * Turn on Async Default routing
	 */
	ddb = get_dialerdb(hwidb);
	if (csb->sense) {

	    tty->slip_flags |= SLIP_DEF_ROUTING;

	    if ((tty->capabilities & MUSTSLIP) ||
		(ddb && (DIALER_INBAND_ASYNC == ddb->dialer) &&
		 ddb->dialer_placed_call)) {

		idb->static_routing = FALSE;     /* Turn on routing */
	    }

	} else {

	    tty->slip_flags &= ~SLIP_DEF_ROUTING;

	    if ((tty->capabilities & MUSTSLIP) ||
		(ddb && (DIALER_INBAND_ASYNC == ddb->dialer) &&
		 ddb->dialer_placed_call)) {

		idb->static_routing = TRUE;     /* Turn off routing */
	    }
	}

	break;

      case ASYNC_DYNAMIC_ROUTING:

	/*
	 * Turn on Async Dynamic routing
	 */

	if (csb->sense) {
	    tty->slip_flags |= SLIP_DYN_ROUTING;
	} else {
	    tty->slip_flags &= ~SLIP_DYN_ROUTING;
	}
	
	break;

      case ASYNC_DYNAMIC_ADDRESS:

	/*
	 * Set the bit based on the NO flag
	 */

	if (csb->sense)
	  tty->slip_flags |= SLIP_DYNAMIC;
	else
	  tty->slip_flags &= ~SLIP_DYNAMIC;

	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

}

#define SLIP_AUTOSELECT_STRING 0x0000C045
#define SLIP_AUTOSELECT_MASK   0x0000FFFF /* Match on 2 bytes */
#define PPP_AUTOSELECT_STRING  0x00FF7d23 /* bytes 2..4 of initial PPP frame */
#define PPP_AUTOSELECT_MASK    0x00FFFFFF /* Match on 3 bytes */

static boolean autoselect_slip(uint sample)
{
    return((sample & SLIP_AUTOSELECT_MASK) == SLIP_AUTOSELECT_STRING);
}


static boolean autoselect_ppp(uint sample)
{
    return ((sample & PPP_AUTOSELECT_MASK) == PPP_AUTOSELECT_STRING);
}

/*
 * async_start_process
 * Return TRUE if we started an async protocol process, FALSE otherwise.
 * Called by SERVICE_EXEC_PROCESS
 */

static boolean async_start_process (tt_soc *tty, uint sample)
{
    char *cmd = NULL;
    ulong stacksize = EXEC_STACK_SIZE;

    /*
     * If this is a callback PPP line, don't process it here
     */
    if (tty->acflags & ACF_CALLBACK_PPP)
        return(FALSE);

    if (tty->capabilities & MUSTSLIP) {
	async_proto_mode(tty);
	return(TRUE);
    }
    if ((tty->capabilities & MAYSLIP) == 0)
	return(FALSE);

    if ((tty->autoselectmask & PARSER_AUTOSELECT_SLIP) && 
	autoselect_slip(sample)) {
	cmd = "slip default";
    } else 
    if ((tty->autoselectmask & PARSER_AUTOSELECT_PPP) &&
	autoselect_ppp(sample)) {
	cmd = "ppp negotiate";
    }

    if (cmd) {
	if (modem_debug)
	    buginf("\nTTY%t Autoselect cmd: %s", tty->ttynum, cmd);
	tty->tty_autocommand = cmd;
	if (reg_invoke_kerberos_is_running()) {
	    stacksize = KERBEROS_STACK_SIZE;
	}
	tty->tiptop = cfork ((forkproc *)exec, (long)tty, stacksize,
			     "Exec", tty->ttynum);
	if (tty->tiptop == -1) {
	    quick_puts(tty,"\r\n%% Unable to start up SLIP/PPP"
		       "- no memory or too many processes\r\n");
	} else {
	    tty->statbits &= ~CARDROP;
	    CLAIM_TTY(tty);
	}
	return(TRUE);
    }

    return(FALSE);
}

/*
 * async_start_ppp
 * Return TRUE if we started an async callback PPP process, FALSE otherwise.
 * Called by SERVICE_EXEC_PROCESS
 *
 * Note: sample is not used at all.
 */
static boolean async_start_ppp (tt_soc *tty, uint sample)
{
    hwidbtype *hwidb;
    idbtype *idb;

    /*
     * If this isn't callback PPP line, don't process it here
     */
    if (!(tty->acflags & ACF_CALLBACK_PPP))
        return(FALSE);
  
    /*
     * Don't start the process here because there isn't a process
     * yet (tiptop=-1). Let the exec process handle it 
     */
    if (tty->tiptop < 0)
        return(FALSE);

    /*
     * Get the async software and hardware idbs
     */
    if (!reg_invoke_async_line_get_idb(stdio->type, tty, &idb, &hwidb))
	return(FALSE);

    /*
     * If not an async mode interactive interface or 
     * interface is IDBS_ADMINDOWN or line has open network connections 
     *   return FALSE
     */
     if (!(tty->capabilities & MAYSLIP) || 
         (IDBS_ADMINDOWN == hwidb->state) || 
         !QUEUEEMPTY(&tty->connhead))
       return(FALSE);

    /*
     * Ensure that we have some kind of an address to use, or the interface
     * is unnumbered.
     */
     if (!idb->ip_address && !idb->ip_unnumbered &&
        !reg_invoke_dialer_huntgroup_member(hwidb) &&
	!idb_sw_get_atalkclientmode(idb)) {
       return(FALSE);
    }
    
    /* 
     * Allow different encap only if not a dialer rotary group member 
     */
    if ((hwidb->enctype != ET_PPP ) &&
	 !reg_invoke_dialer_huntgroup_member(hwidb))
	async_new_encap(hwidb, ET_PPP);

    if (hwidb->enctype != ET_PPP) 
	return(FALSE);


    /* 
     * Set the routing flag for the interface 
     */
    idb->static_routing = !(tty->slip_flags & SLIP_DEF_ROUTING);

    /* 
     * Turn on the protocol using the current address.
     * This address was the current address on the incoming line accepting
     * the callback request from the remote end. When we are now calling
     * back, we should use that same address (propagated by callback). 
     */ 
    async_proto_mode(stdio);		

    /*
     * Start net accounting
     */    
    reg_invoke_aaa_acct_net(stdio->ustruct, NULL, idb);

    /* 
     * Did modem hang up while we were parsing ?
     */
    if (modemcheck(tty)) 
        tty_if_reset(tty);    

    /*
     * We're finished. Old processes don't die, they just kfork.
     */
    reg_invoke_async_handle_process(tty->type, tty);
    return(TRUE);
}


/* 
 * Indicate if we are answering an incoming call on an async interface
 * Invoked by reg_invoke_media_dialer_is_answer 
 */
static boolean 
async_dialer_is_answer (dialerdbtype *ddb)
{
    return(!ddb->dialer_placed_call);
}

/*
 * async_dialer
 * Use chat support to callout on an inband dialer.
 * Called by SERVICE_MEDIA_DIALER_DIAL, index is DIALER_INBAND_ASYNC.
 */


static boolean async_dialer (dialerdbtype *ddb, dialerpaktype *dpak)
{
    boolean success;

    success = reg_invoke_chat_dial(ddb->ownidb, dpak->dial_number, dpak->map);
    
    if (success) {
      dialer_set_call_data(ddb, dpak);
    }

    return(success);
}

/*
 * async_tty_death
 * In case we were dialing out, notify dialer that we failed
 * Called by reg_invoke_death
 */
static void async_tty_death (tt_soc *tty)
{

    hwidbtype *hwidb = tty->tty_idb;

    /*
     * In case we are dialing out, notify dialer that we failed. 
     * Don't notify dialer if this is not part of the dialer sequence 
     * but async callback in its interim stage
     */
    if ((tty->statbits & DIALOUT) && hwidb &&
	reg_invoke_dialer_is_in_use(hwidb) &&
	!(tty->acflags & ACF_CALLBACK_MASK)) {

	/* Signal failure to dialer module */
	if (async_state_debug)
	    buginf("\n%s: Async Interface dial-out failure",
		   hwidb->hw_namestring);
	reg_invoke_dialer_dial_failed(hwidb);
    }
}

/*
 * show_contr_async()
 *	Invoke all the show commands under our list
 */
void show_contr_async (parseinfo *pi)
{
    registry_list(REG_CS, SERVICE_ASYNC_SHOW_CONTROLLERS);
}


/*
 * async_line_get_idb_default
 * Default case to get an idb for a TTY line type
 */
static boolean async_line_get_idb_default (tt_soc *tty,
					   idbtype **idb,
					   hwidbtype **hwidb)
{
    *hwidb = tty->tty_idb;
    *idb   = firstsw_or_null(*hwidb);
    if(!idb)
	return(FALSE);
    return(TRUE);
}
/*
 * async_handle_process_default
 * Default case to handle the process for a TTY line type
 */
static void async_handle_process_default (tt_soc *tty)
{
    kfork(tty);
}
/*
 * async_return_nothing_default
 * PPP options are configured from "interface async N"
 */
static void async_return_nothing_default (tt_soc *tty)
{
    return;
}
/*
 * async_heraddr
 * Return peer address for async interface
 */
static ipaddrtype async_heraddr(hwidbtype *hwidb)
{
    if (hwidb && is_async(hwidb) && hwidb->firstsw)
	return reg_invoke_ip_peer_addr(hwidb->firstsw);
    else
	return 0;
}

/*
 * async_init
 * Initialize async routing support
 */

void async_init (void)
{
    /*
     * Clear the slip traffic array.
     */
    memset(&slip_traffic, 0, sizeof(slip_traffic));

    slip_parser_init();
    async_debug_init();
    reg_invoke_init_async_fs();

    /*
     * Register some functions
     */
    reg_add_create_idb(async_parser_createidb, "async_parser_createidb");
    reg_add_create_idb(async_group_parser_createidb, "async_group_parser_createidb");
    reg_add_tty_show(show_slipdata, "show_slipdata");
    reg_add_exec_process(async_start_process, "async_start_process");
    reg_add_exec_process(async_start_ppp, "async_start_ppp");
    reg_add_media_serial_setup(ET_SLIP, slip_encap_setup, "slip_encap_setup");
    reg_add_dialer_is_answer(DIALER_INBAND_ASYNC, async_dialer_is_answer,
			     "async_dialer_is_answer");
    reg_add_dialer_dial(DIALER_INBAND_ASYNC,async_dialer,"async_dialer");
    reg_add_dialer_drop(DIALER_INBAND_ASYNC,async_reset,"async_reset");
    reg_add_dialer_remote_ip_addr(DIALER_INBAND_ASYNC,async_heraddr,"async_heraddr");
    reg_add_alllines_setup(async_interfaceglobalsetup, "async_interfacesetup");
    reg_add_swif_erase(async_erase_idb, "async_erase_idb");

    reg_add_async_protocol_mode(async_protocols_mode, "async_protocols_mode");
    reg_add_async_trigger_startoutput(async_trigger_startoutput, 
                                      "async_trigger_startoutput");

    /*
     * Register the special async fastswitch check for AUX ports
     */
    reg_add_async_check_fs(async_check_fs, "async_check_fs");

    /*
     * possibly configure ephemeral async interface during SETUP
     */
    reg_add_setup_ephemeral_int(setup_async_ints, "Async dynamic interfaces");

    /*
     * Async systems don't have any process on the tty, except for
     * in DIALOUT mode. Have a hook on death() to do cleanup.
     */

    reg_add_death(async_tty_death, "async_tty_death");
    
    reg_add_default_async_line_get_idb(async_line_get_idb_default,
				       "async_line__get_idb_default");

    reg_add_default_async_handle_process(async_handle_process_default,
					 "async_handle_process_default");

    reg_add_default_async_handle_options(async_return_nothing_default, 
					 "async_return_nothing_default");
    reg_add_config_changed(async_config_changed, "async_config_changed");

    reg_add_if_group_first_member(IDBTYPE_ASYNC_GROUP,
				  async_group_first_member,
				  "async_group_first_member");
    reg_add_idb_get_keep_default(async_default_keepperiod,
 				 "async_default_keepperiod");
    reg_add_serial_physical_layer(async_group_physical_layer,
				  "async_group_physical_layer");
}

typedef struct asyncip_setup_struct_ {
    ulong flags;
    ipaddrtype default_ipaddr;
} asyncip_setup_struct;

static asyncip_setup_struct asyncip_setup_info;

#define SETUP_ASYNC_SLIP	2
#define SETUP_ASYNC_DYNAMICIP	4
#define SETUP_ASYNC_DEFAULTIP	8
#define SETUP_ASYNC_HDRCOMP	16
#define SETUP_ASYNC_DYNAMICROUTES 32


static void setup_async_ints (setup_struct *sp)
{
    int i;
    asyncip_setup_struct *info = (asyncip_setup_struct *) sp->args[LINK_PPP];

    if (!info || !info->flags)
	return;

    for (i = 0; i < ALLlines; i++) {
	if (stdio->statbits & CONTROLC)
	    break;
        if (reg_invoke_invalid_line_specified(i, FALSE))
            break;
	if (MODEMS[i] && MODEMS[i]->type == TTY_LINE) {
	    sp->idb = MODEMS[i]->tty_idb ? MODEMS[i]->tty_idb->firstsw : NULL;
	    setup_printf(sp, "\n!\nInterface Async%d", i);
	    printf("\nConfiguring interface Async%d:", i);
	    reg_invoke_setup_async_int(sp);
	}
    }
}

/*
 * Setup functions for async interfaces.  A lot of this is very ip-centric,
 * and arguably belongs over in ip.  On the other hand, this whole file is
 * pretty ip-centric anyway...
 */
	    
static void async_interfaceglobalsetup (setup_struct *sp)
{
    /*
     * Use LINK_PPP to store information about the setup answers
     * for async IP.  This is somewhat bogus, but should be safe.
     */
    
    int flags = 0;

    if (sp->args[LINK_IP] &&
	(yes_or_no("    Configure for Dial-in IP SLIP/PPP access",
		  FALSE, TRUE))) {
	flags |= SETUP_ASYNC_SLIP;
	if (yes_or_no("      Configure for Dynamic IP addresses",
		      TRUE, TRUE))
	    flags |= SETUP_ASYNC_DYNAMICIP;
	if (yes_or_no("      Configure Default IP addresses",
		      FALSE, TRUE))
	    flags |= SETUP_ASYNC_DEFAULTIP;
	if (yes_or_no("      Configure for TCP Header Compression",
		      TRUE, TRUE))
	    flags |= SETUP_ASYNC_HDRCOMP;
	if (yes_or_no("      Configure for routing updates on async links",
		      FALSE, TRUE))
	    flags |= SETUP_ASYNC_DYNAMICROUTES;
    }
    asyncip_setup_info.flags = flags;
    
    sp->args[LINK_PPP] = (void *) & asyncip_setup_info;
    if (flags != 0) {
	if (!sp->args[LINK_IP])	{ /* make sure we have SOME IP routing */
	    setup_printf(sp, "\nip host-routing");
	}
	reg_add_setup_async_int(setup_async_ip, "setup_async_ip");
    }
}

static void setup_async_ip (setup_struct *sp)
{
    hwidbtype *idb = NULL;		/* IDB used for ip unnumbered */
    int flags;
    asyncip_setup_struct *info = (asyncip_setup_struct *) sp->args[LINK_PPP];
    flags = info->flags;
 
    if (!(flags & SETUP_ASYNC_SLIP))
	return;

    FOR_ALL_HWIDBS(idb) {
	if ((idb->status & IDB_IEEEMASK) ||
	    (idb->firstsw->ip_enabled && idb->firstsw->ip_address))
	    break;
    }
    if (!idb) {
	printf("%% Can't find master interface for async IP support"
	       "\n%% Aborting Setup command");
	stdio->statbits |= CONTROLC;
    }
    setup_printf(sp, "\nip unnumbered %s", idb->hw_namestring);

    if (flags & SETUP_ASYNC_DEFAULTIP) {
	char buffer[MAXBUF], *cp;
	char ipaddr[20];
	ipaddrtype address;
	int status;

	/*
	 * Check if there is something reasonable to use as a default.
	 * use either existing slip address, or save default
	 */
	if (sp->idb && sp->idb->ip_peer && sp->idb->ip_peer->ippeer_addr_def)
	    sprintf(ipaddr, "%i", sp->idb->ip_peer->ippeer_addr_def);
	else if (reg_invoke_ip_good_address(info->default_ipaddr, 0L, FALSE))
	    sprintf(ipaddr, "%i", info->default_ipaddr);
	else
	    sprintf(ipaddr, "none");

	cp = setup_string(buffer,
			  "    Default client IP address for this interface",
			  ipaddr);

	if (0 == strcmp(cp, "none"))
	    *cp = '\000';

	if (!null(cp)) {
	    status = parse_hostname(cp, &address);
	    if (!status || !address ||
		!reg_invoke_ip_good_address(address, 0L, FALSE)) {
		printf("\n%% Bad Internet address.");
	    } else {
		setup_printf(sp, "\nasync default ip address %i", address);
		info->default_ipaddr = 1 + (ulong) address; /* Next default */
	    }
	}
    }
    if (flags & SETUP_ASYNC_HDRCOMP) {
	setup_printf(sp, "\nip tcp header-compression passive");
    }
    setup_printf(sp, "\nasync mode interactive");
    if (flags & SETUP_ASYNC_DYNAMICIP) {
	setup_printf(sp, "\nasync dynamic address");
    }
    if (flags & SETUP_ASYNC_DYNAMICROUTES) {
	setup_printf(sp, "\nasync dynamic routing");
    }
}


/*
 * Async fastswitch initilization routine to fork the fastswitch helper
 * process and initialize the tty mask value
 */
void init_async_fs (void)
{
    /*
     * ttymask bit-position == absolute tty number
     * We do not actually use the bit position corresponding to the 
     * consoles line(s) (usually there is only one), but corresponding
     * bit positions to tty numbers are used. 
     * No. of bits in ttymask must be large enough for total number of 
     * physical async serial lines. Check this and return if not.
     */
    if ( (nttylines+nauxlines+nconlines) > (8 * sizeof(ttymask))){
	buginf("\n % Async Serial ttymask too small \n");
	return;
    }

    /* 
     * Initialize the max async lines that we will use to scan the 
     * ttymask (since the AUX port is not supported and it is 
     * always after the async ports, the the total number of async
     * lines is also the maximum lines we will scan for)
     */
    max_async_lines = nttylines;

    /*
     * Clear mask and fork the helper process
     */
    ttymask = 0;
    asyncfs_fork = process_create(async_fs, "Asy FS Helper", NORMAL_STACK,
				  PRIO_HIGH);
}


/*
 * async_fastsend - Fast switch routine that receives a packet at interrupt
 * level
 */
static void async_fastsend (
     hwidbtype *idb,
     paktype *pak)
{
   traffic_shape_t *shape;   
 
   /*
    * determine whether there is window available to send the message with
    * in the rate control bucket
    */
   shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
   if (shape) {
       traffic_shape_defer_sending(shape, pak);
       return;
   }

   /*
    * Queue the packet to the idb's hold queue
    * If TTY is idle, notify helper process to startoutput
    */
   pak->if_output = idb->firstsw;
   /*
    * Async interfaces are slow compared to Ether/Token/Serial etc.
    * Inbound interface's input queue will quickly max-out if
    * inputQ is not cleared. This is specially true of platforms
    * with many async interfaces. e.g. AS5200 with 46 asyncs.
    */
   clear_if_input(pak);
   holdq_enqueue_fast(idb, pak);
   if ((idb->idb_tty->type != VTY_LINE) && (idb->output_qcount == 1) && 
       (idb->idb_tty->outpak == NULL)) {
       ttymask = ttymask | ( ((ulonglong)1) << idb->unit );
       process_wakeup(asyncfs_fork);
   }
}


/*
 * async_trigger_startoutput - Enable startoutput from Interrupt level
 */
void async_trigger_startoutput (hwidbtype *idb)
{
    ttymask = ttymask | ( ((ulonglong)1) << idb->unit );
    process_wakeup(asyncfs_fork);  
}


/*
 * Async Fastswitch helper process. 
 * This process helps kickstart the transmiiter when there is data
 * to be output on a particular TTY 
 */    
static process async_fs (void)
{
    int i;
    ulonglong temp_ttymask;
    leveltype status;
  
    while (TRUE) {
        /*
         * Block waiting for non-zero mask
         */
	while (ttymask == 0)
	    process_safe_wait_for_event();
 
        /*
         * Critical section the save and clear on the tty mask
         * The async_fastsend, at interrupt level, sets the tty mask at
         * the bit position corresponding to the TTY number.
         */
        status = raise_interrupt_level(NETS_DISABLE);
        temp_ttymask = ttymask;
        ttymask = 0;
        reset_interrupt_level(status);

        /*
         * Sweep thru temporary bit mask, where the bit-position set 
         *is* the tty number. Since we only care about non-console 
         * physical tty lines, start at bit position after the console 
         * line(s) (at least 1).
         */
        for (i=nconlines; i <= max_async_lines; i++){
          if ( (((ulonglong)1) << i) & temp_ttymask )
             startoutput(MODEMS[i]);
        }
     }
} 



/*
 * Check for Async idb fastswitching prohibition - 
 * Fastswitching on the AUX port is not supported.
 */
static void async_check_fs (hwidbtype *idb)
{
  /* 
   * If this is an async interface and it's tty is an AUX port,
   * make sure to disable fastswitching. 
   */
  if ((idb->status & IDB_ASYNC) && 
      (idb->idb_tty) && (idb->idb_tty->type == AUX_LINE)) {
           idb->ip_fastokay = FALSE;
	   idb->novell_fastokay = FALSE;
       }

}

static boolean async_ppp_vencap (paktype *pak, long address)
{
    hwidbtype *idb;
    tt_soc *tty;
    boolean result;

    result = ppp_vencapsulate(pak, address);
    if (result) {
	idb = pak->if_output->hwptr;
	tty = idb->idb_tty;
	if ((tty->capabilities & OUTIDLE) && dialer_async_classify(idb, pak)) {
	    TIMER_START(tty->dallytimer, tty->tty_sessiontimeout);
	}
    }
    return(result);
}
    
