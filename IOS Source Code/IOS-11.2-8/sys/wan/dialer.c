/* $Id: dialer.c,v 3.35.4.32 1996/08/28 22:48:42 suresh Exp $
 * $Source: /release/112/cvs/Xsys/wan/dialer.c,v $
 *------------------------------------------------------------------
 * dialer.c -- Routines for dealing with dialers attached to a serial
 *             interface.
 *
 * 23-Sept-91 - William Miskovetz
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dialer.c,v $
 * Revision 3.35.4.32  1996/08/28  22:48:42  suresh
 * CSCdi66764:  Dialer Interface not spoofing once deleted and created
 * again
 * Branch: California_branch
 * Added dialer_coming() which seems to have been forgot in DDR
 * rearranging
 *
 * Revision 3.35.4.31  1996/08/28  13:21:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.35.4.30  1996/08/27  16:30:58  wmay
 * CSCdi67232:  dialer callback pending checks not efficient
 * Branch: California_branch
 *
 * Revision 3.35.4.29  1996/08/14  14:19:00  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * The multilink half-fastswitching performance improvement
 *
 * Revision 3.35.4.28  1996/08/13  20:02:46  wmay
 * CSCdi65253:  crash in dialer_holdq_output: dpak structure is reused for
 * broadcasts - make sure that we clear out the holdq field of that
 * structure after each call to reg_invoke_dialer_pending
 * Branch: California_branch
 *
 * Revision 3.35.4.27  1996/08/12  22:53:48  asb
 * CSCdi65784:  Hunting for free dialer when none available causes crash
 * Branch: California_branch
 *
 * Revision 3.35.4.26  1996/08/07  19:18:35  asb
 * CSCdi65391:  MBRI dialer rotary buginfs are always enabled
 * Branch: California_branch
 *
 * Revision 3.35.4.25  1996/08/07  09:04:23  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.35.4.24  1996/08/06  17:35:16  asb
 * CSCdi40543:  MBRI dialer rotary cannot detect down BRI
 * Branch: California_branch
 *
 * Revision 3.35.4.23  1996/08/06  00:37:38  asb
 * CSCdi40543:  MBRI dialer rotary cannot detect down BRI
 * Branch: California_branch
 *
 * Revision 3.35.4.22  1996/08/04  08:09:25  fox
 * CSCdi62182:  %SCHED-3-PAGEZERO: Low memory modified by ISDN (0x74 =
 * 0x0).
 * Branch: California_branch
 * Convert ddb->dial_reason to an array of char from a char *.  Modify all
 * necessary references appropriately.
 *
 * Revision 3.35.4.21  1996/08/03  23:41:14  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.35.4.20  1996/08/02  05:25:37  dblair
 * CSCdi64262:  Dial attempt on 2nd B-channel using multilink fails
 * Branch: California_branch
 * Make sure dialer map has a dial string before selecting it for
 * multilink dial out
 *
 * Revision 3.35.4.19  1996/07/25  03:33:02  dblair
 * CSCdi62999:  Multilink and Dialer causes carrier timeout
 * Branch: California_branch
 * Kill the enable timer if ddr async or sync interface changes
 * state to UP
 *
 * Revision 3.35.4.18  1996/07/23  22:04:09  suresh
 * CSCdi62055:  Dialer profile error message misleading
 * Branch: California_branch
 * Code Cleanup for change from dialer user to dialer profile.
 *
 * Revision 3.35.4.17  1996/07/23  13:31:08  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.35.4.16  1996/07/19  19:50:23  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly - do not allocate
 * holdq until needed, and pass the holdq correctly as calls and callbacks
 * are made.  Redo a little callback code to deal with the holdq better and
 * rename some of the structures.  Add a debug dialer holdq (hidden).
 * Branch: California_branch
 *
 * Revision 3.35.4.15  1996/07/12  23:00:04  suresh
 * CSCdi61907:  ISDN unbound doesnt occur on dialer profile int when ppp
 * auth fails
 * Branch: California_branch
 *
 * Dialer Profile unbinding happens only for connected links, not for
 * other kinds of failures like authentication. Fixed the problem.
 *
 * Revision 3.35.4.14  1996/07/08  22:58:48  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.35.4.13  1996/06/27  09:44:41  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.35.4.12  1996/06/19  01:47:10  dblair
 * CSCdi58636:  DDR PPP Callback not working using async interface
 * Branch: California_branch
 *
 * Revision 3.35.4.11  1996/06/18  01:39:35  suresh
 * CSCdi46872:  Dialer load balancing/multilink create to groups when 2
 * numbers dial
 * Branch: California_branch
 *
 * Revamped the way dialer groups are created to do correct
 * dialer load balancin
 *
 * Revision 3.35.4.10  1996/05/19  06:20:59  dblair
 * CSCdi57958:  Multiple Dialer Maps and MLP dont mix well
 * Branch: California_branch
 *
 * Revision 3.35.4.9  1996/05/13  14:48:00  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.35.4.8  1996/05/10  01:44:33  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.35.4.7  1996/05/09  14:49:04  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.35.4.6  1996/05/08  03:39:20  dblair
 * CSCdi56956:  tracebacks in dialer disconnect
 * Move bugtrace in dialer_disconnect out of debug dialer to hidden
 * dialer debug
 * Branch: California_branch
 *
 * Revision 3.35.4.5  1996/05/01  04:38:38  dblair
 * CSCdi55760:  7xxx crash during unconfig of Dialer maps testing over
 * ISDN PRI
 * Branch: California_branch
 *
 * Revision 3.35.4.4  1996/04/27  06:35:53  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.20.2.9  1996/04/25  23:58:04  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.20.2.8  1996/04/23  00:24:09  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.20.2.7  1996/04/17  00:00:08  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.20.2.6  1996/04/11  01:34:46  dblair
 * Branch: Dial1_branch
 * Fixed crash in dialer_disconnect_group while adding PRI to dialer
 * rotary while call is up.
 *
 * Revision 3.20.2.5  1996/03/29  15:54:54  dblair
 * Branch: Dial1_branch
 * Fix crash when clearing virtual interface and add some multilink knobs
 *
 * Revision 3.20.2.4  1996/03/19  12:42:28  dblair
 * Branch: Dial1_branch
 * use NULL string for bundle name if no authetication and no caller id.
 * free bundle after removing it in mlp_timer.
 *
 * Revision 3.20.2.3  1996/03/08  17:30:27  dblair
 * Branch: Dial1_branch
 * Change for multilink virtual bundle interfaces to support ISDN BRI,
 * PRI, and Dialer interfaces
 *
 * Revision 3.20.2.2  1996/02/23  21:31:14  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.20.2.1  1996/01/16  15:45:08  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.35.4.3  1996/04/17  13:51:03  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.35.4.2  1996/04/15  21:25:12  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.35.4.1  1996/03/18  22:47:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.28.2.5  1996/03/16  07:54:59  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.28.2.4  1996/03/13  02:11:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.28.2.3  1996/03/07  11:06:56  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.28.2.2  1996/02/20  21:50:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.35  1996/03/08  18:02:37  dblair
 * CSCdi48424:  Multilink PPP may discard fragments while processing input
 * fragments.  Do not drop fragments when unassigned threshold is reached
 * and allow lost fragments to be detected earlier.
 *
 * Revision 3.34  1996/02/07  19:46:04  dblair
 * CSCdi47994:  Router crashes when low on memory
 * Handle getbuffer fail correctly and clean up multilink data structures
 * when bundle goes down
 *
 * Revision 3.33  1996/02/07  16:17:10  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.32  1996/02/01  06:11:54  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.31  1996/01/31  02:48:14  suresh
 * CSCdi46780:  Fast idle timer not working on rotaries of rotaries
 * Fixed start_fast_idle() to recursively start the fast idle timer.
 *
 * Revision 3.30  1996/01/29  07:35:15  lbustini
 * CSCdi47701:  ISDN fastswitching broken for dial in only configs
 * If authenticated name doesn't match, don't add link to bundle.
 *
 * Revision 3.29  1996/01/23  17:02:56  fred
 * CSCdi47191:  MBRI cannot disable WFQ - screws up multilink
 *         Queue algorithm was not propogated in propogate_priority_chang
 *
 * Revision 3.28.2.1  1996/01/24  23:31:26  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.28  1996/01/20  04:44:00  dblair
 * CSCdi45363:  MLP information no longer displayed in show dialer output
 * Move multilink bundle info from show dialer to new show ppp multilink
 * command
 *
 * Revision 3.27  1996/01/20  01:57:24  lbustini
 * CSCdi44101:  c4500 crashed when attempting to configure dialer interface
 * Ensure that dialer structure exists before configuring interface.
 *
 * Revision 3.26  1996/01/20  00:21:11  irfan
 * CSCdi46951:  dialer interfaces not listed in show dialer
 * incorporating code review comments
 *
 * Revision 3.25  1996/01/18  22:04:44  dblair
 * CSCdi45298:  Clean up of DDR callback and multilink broke lost fragment
 * timer process.
 *
 * Revision 3.24  1996/01/16  20:42:08  irfan
 * CSCdi46951:  dialer interfaces not listed in show dialer
 * Relax the check for ddr links to include Dialer interfaces. Mark
 * dialer interfaces as inband-sync.
 *
 * Revision 3.23  1996/01/16  16:35:52  dblair
 * CSCdi46261:  dialer load balancing/multilink does not work without
 * dialer maps.  Check remote_name first then number_called while looking
 * for a connected group.  Prevent dialer_remote_name from overwriting
 * caller id in number_called.
 *
 * Revision 3.22  1996/01/16  00:46:32  klmj
 * CSCdi43894:  2503 crashes when adding 2 nets to a single OSPF routing
 * process.  Router_Init's stack overflowed, thus corrupting memory -
 * move a couple of string variables out of the stack.
 *
 * Revision 3.21  1996/01/15  19:24:16  misko
 * CSCdi43044:  ip address negotiation not working with DDR
 * When we assign an IP address, allow a dialer map to be dynamically
 * created so that we can map the assigned IP address to an authenticated
 * name.
 *
 * Revision 3.20  1996/01/13  01:34:52  irfan
 * CSCdi44881:  dialer interface dead after removal and redefine
 * disallow removal of dialer interface as long as it has members in
 * the rotary group.
 *
 * Revision 3.19  1996/01/12  16:48:01  rbeach
 * CSCdi46101:  ISDN 4ESS Pri doesn't correctly handle outbound NSF.
 * The correct network specific facility values are now configurable.
 * We support outbound SDN, MEGACOMM and ACCUNET.
 *
 * Revision 3.18  1995/12/22  01:28:32  suresh
 * CSCdi29641:  Dialer dialer-group accepts 0 but does not work
 * Changed the dialer-group range to 1-10; cleaned code so that
 * dialer-list and dialer-group accesses are consistent
 *
 * Revision 3.17  1995/12/21  20:06:46  dblair
 * CSCdi43794:  Clearing a dialer Async interface prevents new calls from
 * being made
 *
 * Revision 3.16  1995/12/15  17:10:54  dblair
 * CSCdi45593:  dialer load either not working properly
 *
 * Revision 3.15  1995/12/14  22:59:09  rbeach
 * CSCdi45588:  ISDN spurious low memory accesses when removing isdn
 * caller xxx or dialer caller xxx.
 *
 * Revision 3.14  1995/12/14  16:01:12  rbeach
 * CSCdi45086:  no dialer caller xxx or no isdn caller xxx may
 * crash the router. The previous pointer was not being updated
 * when removing entries.
 *
 * Revision 3.13  1995/12/12  19:58:48  lbustini
 * CSCdi45296:  DDR brings up second ISDN link when call already in
 * progress
 * Now that PPP line protocol comes up after authentication, extend
 * call pending state to cover the additional delay.
 *
 * Revision 3.12  1995/12/06  18:33:34  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.11  1995/11/29  21:59:32  gstovall
 * CSCdi44528:  encap ppp on interface dialer1 crashes router
 * Make sure the dialer idb has a serial subblock.
 *
 * Revision 3.10  1995/11/29  02:37:17  dblair
 * CSCdi42596:  Unecessary multiple calls when callback and
 * load threshold configure
 * Move TIMER_STOP(call_timer) out of dialer_link_member to prevent
 * multilink_up from stopping it and allowing calls out every load
 * time interface in when threshold is exceeded
 *
 * Revision 3.9  1995/11/29  01:58:58  dblair
 * CSCdi41333:  DDR Callback and Multilink PPP clean up
 *
 * Revision 3.8  1995/11/27  18:29:51  pst
 * CSCdi44460:  cosmetic cleanup of ppp / multilink information
 *
 * Revision 3.7  1995/11/22  20:10:08  lbustini
 * CSCdi44360:  DTR dialing broken on cisco 1005
 * Restore parts of CSCdi39576 that got lost during a talisker branch sync.
 *
 * Revision 3.6  1995/11/20  23:35:28  wmay
 * CSCdi35668:  load-interval only affects D channel, not B channels -
 * propogate change to all hunt-group members.
 *
 * Revision 3.5  1995/11/20  23:01:07  wmay
 * CSCdi32869:  Cannot change hold-queue for BRI interfaces - update
 * all hunt group members when change for hunt group master.
 *
 * Revision 3.4  1995/11/20  18:44:29  lbustini
 * CSCdi42068:  2500 crashed with Local Timeout at _ip_fastswitch
 * Fix a race condition where a cache entry is aged out at the same time
 * that a B channel is being disconnected.
 *
 * Revision 3.3  1995/11/17  18:01:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:48:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.39  1995/11/08  21:35:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.38  1995/10/26  00:56:30  dblair
 * CSCdi42200:  crash in dialer_onesec with async
 * Processing in dialer_onesec can remove member from holdq list.
 * Continue when member is remove
 *
 * Revision 2.37  1995/10/23  18:45:36  lbustini
 * CSCdi39833:  dialer priority can not be set on individual interfaces
 * Only ISDN interfaces copy the dialer priority from the rotary group
 * leader to the members.
 *
 * Revision 2.36  1995/10/21  00:40:20  dblair
 * CSCdi42505:  Invalid free memory with ppp reliable link
 *         Freed memory in protocol_up which runs from interrupt if
 *         reliable
 *         link is configure.  Added flag to data d_hql_memtype to tell
 *         dialer_onesec to free the memory
 *
 * Revision 2.35  1995/10/19  16:34:09  dblair
 * CSCdi42225:  Free async dialer members not being used
 *         Check for NULL string as well as NULL string pointer in
 *         find_hql_mem functions.
 *
 * Revision 2.34  1995/10/18  17:27:31  wmay
 * CSCdi39845:  show interface displays serials in a non-intuitive manner
 * Default slot and slotunit are changed to MAXUINT.  Virtual interfaces
 * (tunnel, loop, async) shouldn't change them, as parser is now smart
 * enough to use unit on high end.
 *
 * Revision 2.33  1995/10/17  22:18:18  rbeach
 * CSCdi36915:  voice calls are not handled correctly over ISDN.
 * Allow configuration for data over voice on both outgoing and
 * incoming calls. For outgoing there is a new option for the
 * dialer map command called class. For incoming there is a new
 * interface option "isdn incoming-voice data". This will allow
 * the isdn code to answer incoming voice calls.
 *
 * Revision 2.32  1995/10/17  13:39:04  dblair
 * CSCdi40835:  C4500 crash when no interface Dialerx entered
 * dialerdb can be NULL during reset_dialer_fields.  Need to check for
 * dialerdb == NULL.
 *
 * Revision 2.31  1995/10/06  02:26:27  lbustini
 * CSCdi39576:  dialer dtr does not leave DTR low permanently after idle
 * timeout
 * Raise DTR only when a call is placed.
 *
 * Revision 2.30  1995/09/30  05:06:09  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.29  1995/09/23  07:17:54  tkolar
 * CSCdi40082:  Dialer requires PPP subsystem hence also require CHAT
 * subsystem
 * Create registry for returning first member of an interface group
 *
 * Revision 2.28  1995/09/20  22:38:49  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.27  1995/09/18  19:39:36  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.26  1995/09/13  03:54:33  rbeach
 * CSCdi37866:  Spurious access recorded when isdn PRI deconfigured.
 * There is a race condition during PRI deconfiguration. The isdn_info
 * structure may have been freed while we cleanup after deconfig.
 * This avoids spurious accesses.
 *
 * Revision 2.25  1995/09/12  22:42:45  gglick
 * CSCdi38693:  lapb disables priority queuing
 *         Fix possible NULL dereference.
 *
 * Revision 2.24  1995/09/09  00:54:22  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.23  1995/09/07  20:33:21  tkolar
 * CSCdi39804:  group async is incompatible with dialer
 * Undo some collateral damage from CSCdi36407.
 *
 * Revision 2.22  1995/08/25  11:45:51  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.21  1995/08/24 15:55:35  lbustini
 * CSCdi38220:  SegV exception when Dialer interface is removed
 * Unlink rotary group members before removing dialer interface.
 *
 * Revision 2.20  1995/08/23  22:52:15  ronnie
 * CSCdi38804:  InARP on ATM interfaces doesnt populate ARP table
 *         Many places were passing a complete addrtype to the hashing
 *         function, when only the first few bytes were set.  This made
 *         the hash function nondeterministic.
 *
 * Revision 2.19  1995/08/18  21:42:39  lbustini
 * CSCdi37190:  X.25 encapsulation over BRI interface is failing
 * Fix queueing vectors setup.
 *
 * Revision 2.18  1995/08/08  20:50:43  lol
 * CSCdi37361:  AAA should not require auth for originating PPP connections
 *
 * Revision 2.17  1995/08/08  16:50:16  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.16  1995/08/07 20:32:24  bdas
 * CSCdi38212:  Dialing reason may be incorrect in some call history mib
 * entries
 *
 * Revision 2.15  1995/08/05  08:39:49  lbustini
 * CSCdi38136:  Incorrect output interface selected when fastswitching
 * over rotary group.
 * When a ddr link comes up, use the authenticated name along with
 * the phone number to set up the route cache entry.
 *
 * Revision 2.14  1995/07/31  09:20:12  misko
 * CSCdi37919:  DDR needs to allow protocols additional control
 * Add additional controls on a per packet basis for upper layer
 * protocols.  This is to help solve CSCdi33368.  Add new flags
 * to allow the upper layers to override the dialer access lists, so
 * they can specify that a packet is interesting or uninteresting
 * regardless of what the access lists say.  Also, add a flag so that
 * the protocols can tell the dialer not to hold the packet in the holdq
 * if a call is in progress.  Lastly, have a flag to we can tell the
 * protocols if we are dropping the packet (failing encaps) because
 * a call is in progress but not connected.
 *
 * Revision 2.13  1995/07/21  01:46:50  bdas
 * CSCdi36879:  Need to provide dialing reason in the call history mib.
 *
 * Revision 2.12  1995/07/05  19:41:16  irfan
 * CSCdi36616:  async ddr slip fails to reconnect
 * Dialer re-enable timer firing before interface has completely reset,
 * causes these problems. Disallow configuring such traps.
 *
 * Revision 2.11  1995/06/28  21:33:21  rbeach
 * CSCdi36421:  Crash when removing PRI configuration. If either the
 * D-channel or a dialer interface have an isdn caller entry and the
 * pri-group is removed the router will crash. This is caused by
 * referencing the dialer_call pointer after it has been freed.
 *
 * Revision 2.10  1995/06/28  09:32:49  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.9  1995/06/26  01:48:10  tkolar
 * CSCdi36407:  group async interfaces should not appear in dialers
 * Sileently fail to add them.
 *
 * Revision 2.8  1995/06/23  15:00:53  lbustini
 * CSCdi34276:  Traffic stops over dialer interface while second
 * connection opens
 * Fix test that determines whether a call is succesful or still pending.
 *
 * Revision 2.7  1995/06/22  20:52:27  rbeach
 * CSCdi36195:  isdn caller command causes illegal low memory accesses.
 * This is caused the first time the isdn caller or dialer caller
 * config command is added. Also added a check for isdn caller without
 * a number to be screened.
 *
 * Revision 2.6  1995/06/21  03:30:48  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.5  1995/06/20  20:38:05  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/20  16:47:21  lbustini
 * CSCdi27037:  bridged multicast doesnt initiate a call over DDR
 * Use pak->encsize rather than idb->encsize to determine offset of
 * ethertype.
 *
 * Revision 2.3  1995/06/17  00:49:46  lbustini
 * CSCdi35984:  router hangs when ddr interface goes down
 * Don't share a single WFQ between rotary group members as this can cause
 * output_qcount to go negative.
 *
 * Revision 2.2  1995/06/09  20:59:38  rbeach
 * CSCdi32791:  BRI/MBRI interfaces dont allow more than 64 isdn caller
 * entries. Make the isdn caller entries dynamically allocated. Dialer
 * and ISDN call screening now share the same mechanism and code.
 *
 * Revision 2.1  1995/06/07  23:17:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "logger.h"
#include "mgd_timers.h"
#include "parser_defs_dialer.h"
#include "../parser/parser_defs_parser.h"
#include "dialer.h"
#include "dialer_private.h"
#include "dialer_debug.h"
#include "dialer_registry.h"
#include "msg_dialer.c"			/* Not a typo, see logger.h */
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../if/network.h"
#include "../if/static_map.h"
#include "../ip/ip_registry.h"
#include "../wan/snapshot.h"
#include "../les/isdn_bri.h"

/*
 * forward references
 */
static dialermaptype *find_dialer_addr_name(char *, dialerdbtype *);

/*
 * Local Storage
 */

/*
 * Dialer maps table
 */
dialermaptype *dialertable[DIALERHASH];

/*
 * Dialer lists table
 */
dialerlisttype darray[DIALER_MAXLIST];

/*
 * Rotary group connected bundles free list
 */
dialergrouptype *dialergroup_free_list;

/*
 * Dialer pools head
 */
dialerpoolheadtype *dialer_pool_head;

/*
 * Odd parity character table
 */
static const uchar dialer_oddparity_table[DIALER_ODDPARITY_MAXCHAR] = {
    0x80, 0x01, 0x02, 0x83, 0x04, 0x85, 0x86, 0x07,
    0x08, 0x89, 0x8A, 0x0B, 0x8C, 0x0D, 0x0E, 0x8F,
    0x10, 0x91, 0x92, 0x13, 0x94, 0x15, 0x16, 0x97,
    0x98, 0x19, 0x1A, 0x9B, 0x1C, 0x9D, 0x9E, 0x1F,
    0x20, 0xA1, 0xA2, 0x23, 0xA4, 0x25, 0x26, 0xA7,
    0xA8, 0x29, 0x2A, 0xAB, 0x2C, 0xAD, 0xAE, 0x2F,
    0xB0, 0x31, 0x32, 0xB3, 0x34, 0xB5, 0xB6, 0x37,
    0x38, 0xB9, 0xBA, 0x3B, 0xBC, 0x3D, 0x3E, 0xBF,
    0x40, 0xC1, 0xC2, 0x43, 0xC4, 0x45, 0x46, 0xC7,
    0xC8, 0x49, 0x4A, 0xCB, 0x4C, 0xCD, 0xCE, 0x4F,
    0xD0, 0x51, 0x52, 0xD3, 0x54, 0xD5, 0xD6, 0x57,
    0x58, 0xD9, 0xDA, 0x5B, 0xDC, 0x5D, 0x5E, 0xDF,
    0xE0, 0x61, 0x62, 0xE3, 0x64, 0xE5, 0xE6, 0x67,
    0x68, 0xE9, 0xEA, 0x6B, 0xDC, 0x6D, 0x6E, 0xEF,
    0x70, 0xF1, 0xF2, 0x73, 0xF4, 0x75, 0x76, 0xF7,
    0xF8, 0x79, 0x7A, 0xFB, 0x7C, 0xFD, 0xFE, 0x7F
};

/*
 * A few useful strings
 */
static uchar dialer_dtr[] = "DTR";
const char *const dialer_keywords[] = {"NONE", "ISDN", "IN-BAND SYNC",
				 "IN-BAND ASYNC", "DTR SYNC", "DIALER PROFILE"};

static uchar dialer_header[MAXSTR];
static uchar dialer_reason[MAXSTR];

/*
 * create_dialer_holdq
 * malloc and initialize dialer holdq if configured.  If not, return NULL
 */
dialer_holdq_type *create_dialer_holdq (dialerdbtype *ddb)
{
    dialer_holdq_type *holdQ;

    holdQ = NULL;
    if (ddb->dialer_holdmax != 0) {
	holdQ = malloc_named(sizeof(dialer_holdq_type),
			     "dialer_holdq_type");
	if (holdQ) {
	    if (dialer_holdq_debug)
		buginf("\n%s:Creating holdq %x", ddb->ownidb->hw_namestring,
		       holdQ);
	    queue_init(&holdQ->queue, ddb->dialer_holdmax);
	    init_dialer_holdq_timer(holdQ);
	}
    } else if (dialer_holdq_debug) 
	buginf("\n%s:No holdq created - not configured",
	       ddb->ownidb->hw_namestring);
    return(holdQ);
}

/*
 * dialer_holdq_timer_start
 * Starts the dialer holdq timer, if it is not already running
 */
void dialer_holdq_timer_start (dialer_holdq_type *holdQ, ulong ticks)
{
    if (holdQ) {
	if (dialer_holdq_timer_running(holdQ)) {
	    
	    if (dialer_detailed_debug)
		buginf("\nHQL: Holdq timer already running");
	    return;
	}

	if (dialer_holdq_debug)
	    buginf("\nstarting dialer holdq timer");
	    
	start_dialer_holdq_timer(holdQ, ticks);
    }
}

/*
 * dialer_holdq_free
 * Frees the holdq memory and NULLs the pointer.
 * Does not discard from the queue - call dialer_holdq_discard_queue
 * for that
 */
void dialer_holdq_free (dialer_holdq_type **holdq)
{
    dialer_holdq_type *elem;
    elem = *holdq;
    if (elem != NULL) {
	stop_dialer_holdq_timer(elem);
	if (dialer_holdq_debug)
	    buginf("\nfreeing dialer holdq %x", elem);
	free(elem);
    }
    *holdq = NULL;
}    


/*
 * dialer_name_lookup
 * Given a name, look for a dialer map entry that has a "name" parameter
 * of this name.  return that map entry.
 */
dialerentrytype *dialer_name_lookup (dialerdbtype *ddb, char *name)
{
    int i;
    dialermaptype *p;
    dialerentrytype *e;
    boolean found = FALSE;
    
    ddb = GET_NETDDB(ddb);		/* Look up name in the master idb */

    for (i = 0; i < DIALERHASH; i++) {
	for (p = dialertable[i]; p; p = p->next) {
	    if (ddb == p->ddb) {
	        for (e = p->entry; e; e = e->next) {
                    if (e->hostname) {
                        found = strcmp(e->hostname, name) == 0;
			if (found)
			    return(e);
		    }
		}
	    }
	}
    }
    return(NULL);
}

/*
 * dialer_addr_lookup
 */
dialermaptype *dialer_addr_lookup (
    dialerdbtype *ddb,
    addrtype *proto_addr,
    uint link)
{
    dialermaptype *l;
    leveltype level;
    
    if (proto_addr == NULL) {
        return(NULL);
    }

    /*
     * If this is a hunt group member, look up the address in the master list.
     */
    ddb = GET_NETDDB(ddb);

    level = raise_interrupt_level(NETS_DISABLE);
    l = dialertable
            [xnethash((uchar *)&proto_addr->addr, proto_addr->length)
	    & (DIALERHASH-1)];

    while (l) {
	if ((l->link == link) && (l->ddb == ddb) &&
	    bcmp(&l->proto_addr, proto_addr, sizeof(addrtype)) == 0) {
	    reset_interrupt_level(level);
	    return(l);
	}
	l = l->next;
    }
    reset_interrupt_level(level);

    return(NULL);
}

/*
 * dialer_string_lookup
 */
dialerentrytype *dialer_string_lookup (dialermaptype *m, char *d_str)
{
    dialerentrytype *l;
    leveltype level;
 
    /*
     * Only one dialer map is allowed for bridging.  In this case return
     * right away with the first (and only) entry.
     */
    if (m->link == LINK_BRIDGE) {
	return(m->entry);
    }

    if (!d_str)
	return NULL;

    level = raise_interrupt_level(NETS_DISABLE);
    l = m->entry;
    if (!l->dialer_string) {
	reset_interrupt_level(level);
	return NULL;
    }
 
    /*
     * Search entries until we find one that matches the passed string
     */
    while (l) {
        if (strcmp(l->dialer_string, d_str) == 0) {
            reset_interrupt_level(level);
            return(l);
        }
        l = l->next;
    }
    reset_interrupt_level(level);
 
    return(NULL);
}

/*
 * dialer_goingdown
 * Here on SERVICE_HWIF_GOINGDOWN to shutdown a dialer interface.
 */  

void dialer_goingdown (hwidbtype *hwidb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(hwidb);
    if (ddb == NULL)
	return;

    ddb->dialer_spoofing_int = FALSE;		/* turn off spoofing */
    ddb->dialer_spoofing_prot = FALSE;
    ddb->dialer_placed_call = FALSE;
    TIMER_STOP(ddb->dialer_idle_timer);
    TIMER_STOP(ddb->dialer_fast_idle_timer);
    TIMER_STOP(ddb->dialer_enable_timer);
    TIMER_STOP(ddb->dialer_carrier_timer);
    if (ddb->dialer_holdQ) {
	dialer_holdq_discard_queue(&ddb->dialer_holdQ,
				   "Interface shutdown");
    }
    TIMER_STOP(ddb->dialer_load_timer);
    set_dialer_state(ddb, DIALER_SHUTDOWN);
    reg_invoke_dialer_shutdown(ddb->dialer, ddb);

}
/*
 * dialer_comingup
 * Here on SERVICE_HWIF_COMINGUP to bring up a dialer interface.
 */
static void dialer_comingup (hwidbtype *hwidb)
{

    dialerdbtype *ddb;

    ddb = get_dialerdb(hwidb);
    if (ddb == NULL)
	return;

    /*
     * Set the spoofing flags and the state of rotary group leaders.
     */
    if (hwidb->type == IDBTYPE_DIALER) {
        ddb->dialer_spoofing_int = TRUE;
        ddb->dialer_spoofing_prot = TRUE;
        hwidb->state = IDBS_UP;
    }
}
/*
 * dialer_shutdown
 * Clean up for shutdown - only for non ISDN dialers.
 */
void dialer_shutdown (dialerdbtype *ddb)
{
    dialerdbtype *member_ddb, *next_member_ddb;
    dialergrouptype *group, *next_group;
    int nmember;

    if (SLAVE_DIALER(ddb)) {
	/*
	 * If this is a huntgroup member, then disconnect *only* this link.
	 *
	 * ISDN interfaces don't come here, so we need not worry about
	 * huntgroup members that are huntgroup leads!
	 */
	dialer_disconnect(ddb->ownidb);
    } else {
	/*
	 * If this is a huntgroup leader, then disconnect
	 * *all* links started in this huntgroup.
	 *
	 * With Dialer Profiles we can share ISDN interfaces,
	 * so we can't shutdown the interface as a whole
	 * instead we should shut ONLY links started by this
	 * interface. This is also a optimum way to bring
	 * down an interface instead of traversing all links.
	 *
	 * We are not using the macros FOR_ALL_CONNECTEDGROUPS_ON_MASTER() 
	 * and FOR_ALL_MEMBERS_IN_GROUP() for a good reason, to avoid
	 * the situations where the pointers being referenced
	 * in the FOR loop get freed
	 */
	group = ddb->conn_group;
	while (group) {
	
	    next_group = group->next;
	    nmember = group->nmember;
	    member_ddb = group->firstddb;
	    
	    while (nmember && member_ddb) {

		next_member_ddb = member_ddb->next_conn;
		dialer_disconnect(member_ddb->ownidb);
		member_ddb = next_member_ddb;
		nmember--;

	    }
	    group = next_group;
	}
    }
    dialer_holdq_discard_queue(&ddb->dialer_holdQ, "Interface shutdown");

}

/*
 * dialer_reset
 * Execute real reset vector.
 */
void dialer_reset (hwidbtype *idb)
{
    dialerdbtype *ddb;
    
    ddb = get_dialerdb(idb);
    if ((ddb == NULL) || ddb->dialer_binding)
	return;

    (*(ddb->dialer_save_reset))(idb);
}


/*
 * dialer_find_group
 * Runs through all connected groups (bundles) and returns a pointer to
 * the one this idb needs to be linked to, or NULL if not found.
 */
static dialergrouptype *dialer_find_group (dialerdbtype *ddb, 
					   dialerdbtype *masterddb)
{
    dialergrouptype *grptr;
    dialermaptype *map;
    dialerentrytype *entry;
    int i;
	
    /*
     * Search through dialergroups, check if there
     * is a direct  match on remote name or number
     * called
     */

    FOR_ALL_CONNECTEDGROUPS_ON_MASTER(masterddb, grptr) {
      /*
       * The right connected group is the one that matches the remote name
       * If remote_name does not exist match on number called.
       * remote_name has more significance if it exists, so test first.
       * Must not test number_called first because multiple numbers
       *  can be used to dial the same remote AND, if the dialer map
       *  does not contain the number and no caller id is provided,
       *  all dial-in remotes will be assigned to either "" or "unknown
       *  phone number" connected group depending on authentication.
       */
      if (ddb->remote_name[0] != '\0') {
	if (strcmp(ddb->remote_name, grptr->remote_name) == 0) {
	  return(grptr);
	} else {
	  continue;
	}
      }
      if (strcmp(ddb->number_called, grptr->number_called) == 0) {
	return(grptr);
      }
    }

    /*
     * We have remote name but no matching dialergroups,
     * this means clearly there is no dialergroup to
     * this destination, so create it !
     */

    if (ddb->remote_name[0] != '\0')
      return (NULL);

    /*
     * If dialergroup matches a dialermap which also matches
     * callerid at hand return that group !
     */

    FOR_ALL_CONNECTEDGROUPS_ON_MASTER(masterddb, grptr) {
      for (i = 0; i < DIALERHASH; i++) {
	for (map = dialertable[i]; map; map = map->next) {

	  for (entry = map->entry; entry; entry = entry->next) {
	    if (strcmp(grptr->number_called, entry->dialer_string) == 0) {
	      break;
	    }
	  }

	  if (entry != NULL) {
	    for (entry = map->entry; entry; entry = entry->next) {
	      if ((strcmp(ddb->number_called, entry->dialer_string) == 0) ||
		  (strcmp(ddb->number_called, entry->hostname) == 0)) {
		return (grptr);
	      }
	    }
	  }
	}
      }
    }

    /*
     * Brute force search all dialer maps
     */
    
    for (i = 0; i < DIALERHASH; i++) {
      for (map = dialertable[i]; map; map = map->next) {

	for (entry = map->entry; entry; entry = entry->next) {
	  /*
	   * Bogus Callerids need to corrected & name set
	   */
	  if ((strcmp(ddb->number_called, entry->dialer_string) == 0) ||
	      (strcmp(ddb->number_called, entry->hostname) == 0)) {
	    if (entry->dialer_string != NULL) 
	      strcpy(ddb->number_called, entry->dialer_string);
	    if (entry->hostname != NULL) 
	      strcpy(ddb->remote_name, entry->hostname);
	    return (NULL);
	  }
	}

      }
    }

    return (NULL);
}

/*
 * dialer_link_member
 * Link a rotary group member into the connected members linked list.
 * Called when a connection is established.
 *   return FALSE = new group created
 *   return TRUE = added to existing group
 *   return first = TRUE : first link in a group
 *   return first = FALSE : not first link in a group
 */
static boolean dialer_link_member (dialerdbtype *ddb)
{
    hwidbtype *masteridb;
    dialerdbtype *masterddb, *firstddb;
    dialergrouptype *grptr;
    leveltype level;


    if (!SLAVE_DIALER(ddb) || CONNECTED_GROUP(ddb))
	return(FALSE);

    /*
     * Loop through all connected groups to see if there is one that matches 
     * this idb's connected number.
     */
    masteridb = ddb->netcfg_idb;
    masterddb = ddb->netcfg_ddb;
    grptr = dialer_find_group(ddb, masterddb);

    if (grptr == NULL) {
	/*
	 * If no group was found, get one from the free list.
	 */
	grptr = dialergroup_free_list;
	if (grptr == NULL)
	  return(FALSE); 
	else
	  dialergroup_free_list = grptr->next;

	grptr->huntddb = masterddb;
	grptr->firstddb = ddb;
	grptr->nmember = 1;
	TIMER_STOP(grptr->debounce_timer);
	TIMER_STOP(grptr->activity_timer);
	strcpy(grptr->number_called, ddb->number_called);
	strcpy(grptr->remote_name, ddb->remote_name);
	ddb->next_conn = ddb;
	ddb->prev_conn = ddb;
	ddb->conn_group = grptr;
	
	if (!CONNECTED_GROUP(masterddb)) {
	    /*
	     * This is the first connected group on this rotary group.
	     */
	    grptr->next = NULL;
	    grptr->prev = NULL;
	} else {
	    /*
	     *  Insert this group at the beginning of the list.
	     */
	    masterddb->conn_group->prev = grptr;
	    grptr->next = masterddb->conn_group;
	    grptr->prev = NULL;
	}
	masterddb->conn_group = grptr;
	reg_invoke_dialer_group_init(masterddb->ownidb, grptr);
    } else {
	/*
	 * If it already exists, insert new member in circular list.
	 */
	grptr->nmember++;
	ddb->conn_group = grptr;

	/*
	 * Now disable interrupts to insert idb into group circular list
	 * to prevent fastswitching to alter the vectors at the same time.
	 */
	level = raise_interrupt_level(NETS_DISABLE);
	firstddb = grptr->firstddb;
	ddb->next_conn = firstddb;
	ddb->prev_conn = firstddb->prev_conn;
	ddb->prev_conn->next_conn = ddb;
	ddb->next_conn->prev_conn = ddb;
	reset_interrupt_level(level);
    }

    /*
     *  Start the load timer if not already running
     */

    if (masterddb->dialer_load_threshold &&
	!TIMER_RUNNING(masterddb->dialer_load_timer))
	TIMER_START(masterddb->dialer_load_timer, DIALER_LOAD_TIME);
    return(TRUE);
}

/*
 * dialer_unlink_member
 * Unlink a rotary group member from the connected members linked list.
 * Called when removing a member from a rotary group, or when a connection 
 * is terminated.
 */
void dialer_unlink_member (dialerdbtype *ddb)
{
    hwidbtype *idb;
    dialerdbtype *masterddb;
    dialergrouptype *grptr;
    leveltype level;

    /*
     * If not a member of rotary group or a connected group, return.
     */
    if (!CONNECTED_GROUP(ddb))
	return;

    /*
     * Disable interrupts so that the fastswitching code doesn't try to
     * send a packet out to the interface we are just unlinking.
     */
    level = raise_interrupt_level(NETS_DISABLE);

    grptr = ddb->conn_group;
    grptr->nmember--;

    if (ddb->next_conn == ddb) {
	/*
	 * If this is the only interface in the connected group, delete
	 * the group master and invalidate the fastswitching cache.
	 */
	reg_invoke_dialer_group_destroy(grptr);
	masterddb = grptr->huntddb;
	if (grptr->prev == NULL) {
	    /*
	     * First connected group in the list: update master idb.
	     */
	    masterddb->conn_group = grptr->next;
	    if (grptr->next != NULL) {
		grptr->next->prev = NULL;
	    }
	    if (!CONNECTED_GROUP(masterddb))
		TIMER_STOP(masterddb->dialer_load_timer);
	} else {
	    /*
	     * Just unlink it.
	     */
	    grptr->prev->next = grptr->next;
	    if (grptr->next != NULL) {
		grptr->next->prev = grptr->prev;
	    }
	}

	/*
	 * Return dialergroup structure to the free list.
	 */
	bzero(grptr->number_called, MAXSTR);
	grptr->next = dialergroup_free_list;
	dialergroup_free_list = grptr;

	/*
	 * A connected group is gone: invalidate all cache entries for 
	 * the rotary group leader.  This will cause all connected groups
	 * to go back to process level until the cache is populated again,
	 * but there is currently no way of selectively invalidating cache
	 * entries for a specific connected group.
	 */
	reg_invoke_bump_cache_version(masterddb->ownidb, NULL, TRUE);
    } else {
	/*
	 * Just unlink this idb.
	 * if we were spoofing, turn it off.  If we were spoofing and
	 * we were pretending that the interface was up, set it to down.
	 */
	idb = ddb->ownidb;
	ddb->prev_conn->next_conn = ddb->next_conn;
	ddb->next_conn->prev_conn = ddb->prev_conn;
	if (grptr->firstddb == ddb) {
	    grptr->firstddb = ddb->next_conn;
	}
    }
    reset_interrupt_level(level);
    ddb->conn_group = NULL;

}

/*
 * dialer_disconnect_group
 * If the interface passed as parameter is connected, disconnect it.
 * If it is a rotary group leader, disconnect all connected members.
 * Used when adding/removing an interface to/from a rotary group to
 * disconnect all connections.
 */
void dialer_disconnect_group (dialerdbtype *ddb)
{
    dialerdbtype *tmpddb, *nextddb;

    if (HUNTGROUP_LEADER(ddb)) {
	tmpddb = ddb->hunt_group_first;
	while (tmpddb) {
	    /*
	     *  call to dialer disconnect group will free tmpddb
	     *   when tmpddb->ownidb is a virtual-access interface
	     *   that has been dynamically added to a hunt group.
	     *  Remember the next hunt group member in the list
	     *   and reassign tmpddb just in case it has been freed.
	     */

	    nextddb = tmpddb->hunt_group_next;
	    dialer_disconnect_group(tmpddb);
	    tmpddb = nextddb;
	}
    } else {
	if (CONNECTED_GROUP(ddb))
	    dialer_disconnect(ddb->ownidb);
    }
}


/*
 * Verify the calling number. Return TRUE if exact verification succeeded.
 */
boolean dialer_check_number (dialerdbtype *ddb, const char *number)
{
    dialer_call_tab *tmpcall;

    for (tmpcall = ddb->dialer_call; tmpcall; tmpcall = tmpcall->next) {
        /*
         * We have a table entry, now we need to compare it to the received 
	 * number.
         */
	if (strcmp(number, tmpcall->dialer_call_string) == 0)
	    return(TRUE);
    }

    return(FALSE);
}


/*
 * Verify the calling number. Return FALSE, if verification failed.
 */
boolean dialer_calling_num (calling_number, ddb)
    char calling_number[DIALER_MAX_CALLER];
    dialerdbtype *ddb;
{
    int y, len, rcvdlen, loopcnt, tblloop, rcvdloop;
    dialer_call_tab *tmpcall;

    /*
     * We need to see if there are any configured entries.
     * If there are none return TRUE.
     */
    if (ddb->dialer_call == NULL) {
        return(TRUE);
    }
    loopcnt = 0;
    rcvdlen = strlen(calling_number); /* len of recvd value  */
    tmpcall = ddb->dialer_call;
    while (tmpcall != NULL) {

        /*
         * We have a table entry, now we need to compare
         * it to the received entry. Remember that "x"
         * is a don't care for that position. Return
         * TRUE if we find a match.
         */
        len = strlen(tmpcall->dialer_call_string); /* len of stored value */
        tblloop = len - 1;
        rcvdloop = rcvdlen - 1;
        if (len < rcvdlen) {    /* loop for the smaller of the two */
            loopcnt = len-1;
        } else {
            loopcnt = rcvdlen-1;
        }

        /*
         * Start compare right justified and only compare against
         * the shortest length.
         */
        for (y = loopcnt; y >= 0; y--, tblloop--, rcvdloop--) {
            if (tmpcall->dialer_call_string[y] == 'x') {
                /* a don't care entry */
                if (y == 0) {
                    return(TRUE); /* found match */
                }
                continue;
            }
            if (tmpcall->dialer_call_string[tblloop] ==
                calling_number[rcvdloop]) {
                if (y == 0) {
                    return(TRUE); /* found match */
                }
                continue;
            }
            break;
        }
        tmpcall = tmpcall->next;
    }
    return(FALSE);
}

/*
 * dialer_classify
 * This routine decides if a packet is "interesting" based on the lists
 * which apply to this interface.
 */

static boolean dialer_classify (dialerdbtype *ddb, paktype *pak, char *reason)
{
    dlisttype *dlist;
    link_t linktype;

    if (reason)
	reason[0] = 0;		/* Ensure reason pointer isn't garbage */

    if (pak->dialer_flags & PAK_DDR_INTERESTING) {
	if (reason)
	    strcpy(reason, "set by protocol");
	return(TRUE);
    }

    if (pak->dialer_flags & PAK_DDR_UNINTERESTING) {
	if (reason)
	    strcpy(reason, "set by protocol");
	return(FALSE);
    }

    if (HUNTGROUP_MEMBER(ddb) &&
	pak->linktype == LINK_ARP) {
	if (reason)
	    strcpy(reason, "arp over member");
	return(TRUE);
    }
    
    if (!ddb->dialer_list) {
	if (reason)
	     strcpy(reason, "no dialer-group defined");
        return(FALSE);
    }

    if (darray[ddb->dialer_list].dlist == NULL) {
	if (reason)
	    sprintf(reason, "dialer-list %d not defined", ddb->dialer_list);
	return(FALSE);
    }

    /*
     * If a LINK_SNAPSHOT packet, just accept it...
     */
    if (pak->linktype == LINK_SNAPSHOT) {
	if (reason)
	    strcpy(reason, "set by snapshot");
	return(TRUE);
    }

    /*
     * If the packet has the 'SNAPSHOT_PAK_IS_UPDATE' flag set, this
     * means it is an update packet, and should not trigger dialing.
     * Snapshot itselfs takes care of triggering dialing.
     */
    if (pak->snapshot_flags & SNAPSHOT_PAK_IS_UPDATE) {
	if (reason)
	    strcpy(reason, "set by snapshot");
	return(FALSE);
    }


    /*
     * If the linktype is a VJ IP packet fold it into the LINK_IP type
     * for comparition purposes
     */

    linktype = pak->linktype;
    
    if ((linktype == LINK_COMPRESSED_TCP) || (linktype == LINK_UNCOMPRESSED_TCP))
      linktype = LINK_IP;



    for (dlist = darray[ddb->dialer_list].dlist; dlist; dlist = dlist->next) {
	if (dlist->linktype != linktype)
	    continue;

	if (dlist->list_type == DIALLST_PERMIT) {
	    if (reason)
		sprintf(reason, "%s PERMIT", print_linktype(pak->linktype));
	    return(TRUE);
	}

	if (dlist->list_type == DIALLST_DENY) {
	    if (reason)
		sprintf(reason, "%s DENY", print_linktype(pak->linktype));
	    return(FALSE);
	}

	if ((dlist->list_type == DIALLST_LIST) ||
	    (dlist->list_type == DIALLST_PROLIST)) {
	    if (reason)

	      if (dlist->linktype != LINK_CLNS)
		sprintf(reason, "list %d", dlist->testlist.number);
	      else
		sprintf(reason, "list %s", dlist->testlist.name);
	    if (reg_invoke_access_check(pak->linktype, pak,
					dlist->testlist.number))
		return(TRUE);
	    else
		return(FALSE);
	}
	break;
    }
    if (reason)
	strcpy(reason, "no list matched");
    return(FALSE);
}

/*
 * dialer_async_classify
 */
boolean dialer_async_classify (hwidbtype *output, paktype *pak)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(output);
    if (ddb == NULL)
	return(TRUE);

    if (ddb->dialer_list == 0)
	return(TRUE);

    return(dialer_classify(ddb, pak, NULL));
}

/*
 * dialer_get_packet_header
 * Extract packet header for debug purpose.
 */

void dialer_get_packet_header (paktype *pak, char *buffer)
{
    ushort typecode;
    char *source, *destination;

    buffer[0] = '\0';
    if (pak->linktype == LINK_BRIDGE) {
	typecode = reg_invoke_extract_typecode(pak);
	sprintf(buffer, "%s (0x%-04x)", print_linktype(pak->linktype),typecode);
    } else if (reg_used_sprintf_src_dst(pak->linktype)) {
	source = string_getnext();
	destination = string_getnext();
	reg_invoke_sprintf_src_dst(pak->linktype, source, pak, SOURCE);
	reg_invoke_sprintf_src_dst(pak->linktype,destination, pak, DESTINATION);
	sprintf(buffer, "%s (s=%s, d=%s)", print_linktype(pak->linktype),
		source, destination);
    } else {
	strcpy(buffer, print_linktype(pak->linktype));
    }
}

/*
 * dialer_packet_hexdump
 * Dump up to 48 bytes of packet.
 */
static void dialer_packet_hexdump (paktype *pak)
{
    int i, n;
    uchar *ptr;

    /*
     * Dump first xxx bytes of the packet to the screen.
     */
    n = (pak->datagramsize < 48) ? pak->datagramsize : 48;
    ptr = (uchar *)pak->datagramstart;
    for (i = 1; i <= n; i++) {
	if ((i - 1) % 24 == 0)
	    buginf("\n%02x", *ptr++);
	else if (i % 4 == 0)
	    buginf("%02x ", *ptr++);
	else
	    buginf("%02x", *ptr++);
    }
}

/*
 * dialer_connected
 * Given an idb and a dialer map, check the phone number to be called against 
 * the number currently connected.  If they are the same, return true, if they
 * are different, return false.
 */
boolean dialer_connected (dialerdbtype *ddb, dialermaptype *addr)
{
    dialerentrytype *e;
    boolean found;

    /*
     * DTR dialers are connected if the dialer state is 'pending',
     * 'connected' or 'operational'.
     */
    if (DTR_DIALER(ddb)) {
	return(DIALER_INPROGRESS(ddb));
    }

    /*
     * In band dialers are connected if the number_called is equal to
     * the map entry dial string (if a map exists), or to the dialer
     * string
     */
    if (addr != NULL) {
	found = FALSE;
	e = addr->entry;
	while (e) {
	    if (e->dialer_string)
                found = strcmp(e->dialer_string, ddb->number_called) == 0;
            if (!found && e->hostname) {
                found = strcmp(e->hostname, ddb->remote_name) == 0;
		/*
		 * To be able to handle the case where we want to use the
		 * name field of the dialer map to hold what caller id 
		 * returns, we must allow comparing of the hostname with the
		 * number called (which holds the caller id number on 
		 * incoming calls.
		 */
                if (!found)
                    found = strcmp(e->hostname, ddb->number_called) == 0;
	    }
	    if (found)
		return(TRUE);
	    e = e->next;
	}
    } else if (ddb->dialer_string != NULL) {
	return(strcmp(ddb->dialer_string->entry->dialer_string,
		      ddb->number_called) == 0);
    }
    return(FALSE);
}

/*
 * dialer_pending
 * Given an idb and an address, check the phone number to be called against the
 * pending call list.  If in the list, return true, if not, return false.
 * For serial and dialer interfaces only.
 */

dialerdbtype *dialer_pending (dialerdbtype *ddb, dialerpaktype *dpak)
{
    dialerdbtype *outddb, *tempddb;

    /*
     * If we are a dialer interface, query all members until we find one that
     * is placing the call.  If we are a serial interface return FALSE.
     */
    if (HUNTGROUP_LEADER(ddb)) {
	FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {

	   outddb = reg_invoke_dialer_pending(tempddb->dialer, tempddb, dpak);
	    if (outddb) {
		return(outddb);
	    }
	}
    } else {
	/*
	 * A non-isdn dialer has a call pending if the interface is connected
	 * to the right destination (idb->number_called matches dialer map),
	 * but is not fully up.
	 */

       if (!DIALER_UP(ddb) && dialer_connected(ddb, dpak->map)) {
	    dpak->holdQ = ddb->dialer_holdQ;
	    return(ddb);
	}
    }
    return(NULL);
}


/*
 * add_oddparity
 * Add odd parity to characters in string
 */
static void add_oddparity (char *s, int  length)
{
    int i;

    for (i = 0; i < length; i++) {
        *s = dialer_oddparity_table[*s & 0x7f];
        s++;
    }
}

/*
 * dial_v25bis
 * Issue V.25bis dialing commands on an interface.  
 * Called by SERVICE_MEDIA_DIALER_DIAL, index is DIALER_INBAND_SYNC.
 */

static boolean dial_v25bis (dialerdbtype *ddb, dialerpaktype *dpak)
{
    hwidbtype *idb;
    paktype *pak;
    dialer_hdr *dialer;
    char *cp;
    char *cp_bis;
    int len;
    leveltype status;			/* lock out interrupts */
    boolean success;

    idb = ddb->ownidb;
    success = FALSE;
    len = strlen(dpak->dial_number) + DIALER_CRN_LEN;
    if ((pak = getbuffer(len))) {
	cp = cp_bis = pak->network_start;
	bcopy("CRN ", cp, DIALER_CRN_LEN);
	cp += DIALER_CRN_LEN;
	bcopy(dpak->dial_number, cp, len - DIALER_CRN_LEN);
        if (ddb->dialer_parity == DIALER_INBAND_ODDPARITY) {
            add_oddparity(cp_bis, len);
        }
	pak->datagramstart = pak->network_start - DIALER_ENCAPBYTES;
	dialer = (dialer_hdr *)pak->datagramstart;
	dialer->flags = V25BIS_FLAGS;
	pak->datagramsize = len + DIALER_ENCAPBYTES;
	pak->enctype = ET_HDLC;	/* just fake it */
	pak->if_output = idb->firstsw;
	pak->flags |= PAK_PRIORITY;
	status = raise_interrupt_level(NETS_DISABLE);
	success = (*idb->oqueue)(idb, pak, TAIL);
	reset_interrupt_level(status);
	if (success) {
	    (*idb->soutput) (idb);	/* start another output if pending */
            TIMER_START(ddb->dialer_carrier_timer, ddb->dialer_carrier_ticks);
	    dialer_set_call_data(ddb, dpak);
        } else {
	    retbuffer(pak);
	    if (dialer_debug)
		buginf("\n%s: dialing failed, output queue full",
                       idb->hw_namestring);
	}
    }
    return(success);
}

/*
 * dial_dtr
 * raises DTR to "dial" a dtr dialer
 */
static boolean dial_dtr (dialerdbtype *ddb, dialerpaktype *dpak)
{
    hwidbtype *idb;

    idb = ddb->ownidb;
    /*
     * Before invoking the driver reset routine, it's required
     * to set call parameters.  The setting of call parameters
     * consists of dialer state change to DIALER_PENDING.
     * Then the driver reset routine will check the dialer state
     * before raising DTR to place a call.
     */
    dialer_set_call_data(ddb, dpak);

    (*(idb->reset))(idb);
    TIMER_START(ddb->dialer_carrier_timer, ddb->dialer_carrier_ticks);
    return(TRUE);
}

/*
 *  dialer_connect_name
 *  Try to dial the destination specified by name. Name may be a dialstring
 *  requesting_idb is the idb which is requesting the call be made.
 */
boolean dialer_connect_name (uchar *name, hwidbtype *requesting_idb,
			     uchar *reason)
{
    dialerdbtype *map_ddb, *dialddb, *requesting_ddb;
    dialermaptype *map;
    dialerpaktype dpak;

    requesting_ddb = get_dialerdb(requesting_idb);

    map = find_dialer_addr_name(name, requesting_ddb);
    if (!map && requesting_ddb) {
	/*
	 *  name did not match a dialer map
	 *  Try to match the dialer map phone number
	 */
	map = find_dialer_addr_name(requesting_ddb->number_called,
				    requesting_ddb);
    }
    if (!map)
	return(FALSE);
    
    map_ddb = map->ddb;
    dialddb = reg_invoke_get_free_dialer(map_ddb->dialer, map_ddb);
    if (!dialddb)
	return(FALSE);

    bzero(&dpak, sizeof(dialerpaktype));
    dpak.ddb = map_ddb;
    dpak.map = map;
    dpak.header = NULL;
    setstring(&dpak.header, reason);
    if (callback_pending(&dpak) != NULL) {
	if (dpak.header) {
	    free(dpak.header);
	}
	return(FALSE);
    }
    return(dial_if(dialddb, &dpak));
}

/*
 * find_dialer_string
 * return a pointer to the first phone number for this address/link pair.
 */
static char *find_dialer_string (dialerdbtype *ddb, dialerpaktype *dpak)
{
    dialermaptype *map;
    char *tmp;

    /*
     * Always find something for DTR dialers
     */
    if (DTR_DIALER(ddb))
	return(dialer_dtr);

    /*
     * Look up the given address and linktype in the hash table.
     * If we find it, then that is the number we dial.  If we
     * don't find it, use the default number.  
     */
    tmp = NULL;
    map = dialer_addr_lookup(ddb, dpak->proto_addr, dpak->linktype);

    if (map != NULL) {
        tmp = (map->entry)->dialer_string;
    } else if (ddb->dialer_string != NULL) {
        tmp = ddb->dialer_string->entry->dialer_string;
    }
    return(tmp);
}

/*
 * find_dialer_addr
 * Given a dial string, build an addrtype structure matching a dialer map
 * that we can use to dial a specific number.  If a dialer string is
 * configured, build a default (junk) structure.  If neither dialer map 
 * nor dialer string works, return FALSE.
 */
static dialermaptype *find_dialer_addr (dialerdbtype *ddb, char *string)
{
    dialermaptype *map;
    dialerentrytype *entry;
    int i;

    /*
     * First look for a dialer map.
     */
    for (i = 0; i < DIALERHASH; i++) {
	for (map = dialertable[i]; map; map = map->next) {
	    if (ddb == map->ddb) {
	        for (entry = map->entry; entry; entry = entry->next) {
                    if (entry->dialer_string) {
                        if (strcmp(entry->dialer_string, string) == 0) {
			    return(map);
			}
		    }
		}
	    }
	}
    }

    /*
     * We didn't find a map, try the dialer string.
     */
    if (ddb->dialer_string != NULL) {
	if (strcmp(ddb->dialer_string->entry->dialer_string, string) == 0) {
	    return(ddb->dialer_string);
	}
    }

    /*
     * No dialer string found either.  Just return FALSE.
     */
    return(NULL);
}

/*
 * find_dialer_addr_name
 * Given a name, build an addrtype structure matching a dialer map
 * that we can use to dial a specific number.  name may be a dial string
 * so check both the hostname and dialer_string fields.
 */
static dialermaptype *find_dialer_addr_name (
    char *name,
    dialerdbtype *requesting_ddb)
{
    dialermaptype *map;
    dialerentrytype *entry;
    int i;
    dialerdbtype *net_ddb, *profileddb;
    
    net_ddb = NULL;

    /*
     *  requesting_ddb will be NULL if requesting idb was not
     *    a ddr interface, ie serial.
     */
    if (requesting_ddb) {
	net_ddb = GET_NETDDB(requesting_ddb);
    }

    /*
     * First look for a dialer map.
     */
    for (i = 0; i < DIALERHASH; i++) {
	for (map = dialertable[i]; map; map = map->next) {
	    /*
	     *  If the requesting idb was a dialer, make sure
	     *    the map->ddb matches the net_ddb.
	     *  if the requesting idb was not a ddr interface
	     *     check all maps configured on the router.
	     */
	    if ((!net_ddb) || (net_ddb == map->ddb)) {
		for (entry = map->entry; entry; entry = entry->next) {
		    /*
		     *  Dynamic dialer map creates a map with no dialer_string
		     *  so choose the dialer map if: 
		     *  a) it has a valid dialer_string AND
		     *  either:
		     *  b) the hostname is valid and matches name OR
		     *  c) the dialer_string matches name
		     */
		    if (!null(entry->dialer_string) &&
			((!null(entry->hostname) && 
			  (strcmp(entry->hostname, name) == 0)) || 
			 (strcmp(entry->dialer_string, name) == 0))) {
			/*
			 *  Make sure there is a free dialer.  There
			 *   is no use in returning a map that does
			 *   not have a free dialer.
			 */
			if (reg_invoke_dialer_is_free(map->ddb->dialer,
						      map->ddb)) {
			    return(map);
			}
		    }
		}
	    }
	}
    }

    /*
     * We didn't find a map.
     * requesting idb could be one of the following:
     * Leased, legacy ddr, or dialer pool-member
     * If the requesting idb is a dialer check for the dialer_string
     * This will only check the dialer interface which the requesting
     *   idb is a part of.
     *
     * A better way to check would be to search all interfaces with
     * dialer configured for a matching dialer string and set *map_idb
     *  to that interface.
     */
    if (requesting_ddb) {
	/*
	 *  requesting idb is a ddr interface.
	 */
	if (requesting_ddb->dialer_bound) {
	    /*
	     *  requesting idb is a dialer pool-member and is bound to 
	     *    to a dialer profile.
	     *    return the dialer_string for the dialer profile
	     *  netcfg_ddb should be the dialer profile ddb.
	     */
	    return(requesting_ddb->netcfg_ddb->dialer_string);

	} else {
	    
	    /*
	     *  THis is legacy ddr - NOT dialer profiles
	     *  
	     */
	    
	    if (HUNTGROUP_MEMBER(requesting_ddb)) {
		/*
		 *  Legacy ddr and requesting_ddb is in a hunt group
		 *  Get the dialer string from the net_ddb if it exists
		 */
		return(requesting_ddb->netcfg_ddb->dialer_string);
	    } else {
		/*
		 *  requesting idb is ddr but not a hunt group 
		 *    member
		 *  search for a dialer profile with the same remote_name
		 *   as name
		 */
		profileddb = dialer_find_profile_by_name(name);
		if (profileddb)
		    return(profileddb->dialer_string);
	    }
	} 
    } else {
	/*
	 *  This is the leased line case
	 *  search all profiles for one with the same remote_name as
	 *    name.  Return the dialer string.
	 */
	profileddb = dialer_find_profile_by_name(name);
	if (profileddb)
	    return(profileddb->dialer_string);
    }
    /*
     * No dialer string found either.  Just return FALSE.
     */
    return(NULL);
}
/*
 * dialer_set_number_called
 * Let anyone interested know that the number called has been
 * set (not cleared) on the interface. What happens is that
 * all of the map statements associated with the interface are
 * scanned, for any type. For those that match with their
 * 'number to dial' string, the addrtype structure is passed in to
 * a registry indexed by link type, to let them know this address
 * can now be reached (or is 'pending to be reached').
 *
 * Pass in 'NULL' as new_number_location to clear the idb->number_called.
 * For Snapshot's help, if new_number_location is NULL, and no Snapshot
 * map entries are found, call with an 'empty' addr, LINK_SNAPSHOT. This
 * will allow server side Snapshot to detect down links without needing
 * map statements.
 */
void dialer_set_number_called (dialerdbtype *ddb, char *new_number_location)
{
    int i;
    hwidbtype *real_idb, *master_idb;
    dialerdbtype *master_ddb;
    dialermaptype *p;
    dialerentrytype *e;
    int new_numberlen, old_numberlen;
    addrtype addr;

    if (HUNTGROUP_LEADER(ddb))
        return;

    /* Get hunt group master, which may be this interface itself */
    real_idb = ddb->ownidb;
    master_idb = GET_NETIDB(ddb);
    master_ddb = GET_NETDDB(ddb);

    old_numberlen = strlen(ddb->number_called);
    if (new_number_location != NULL) {
	new_numberlen = strlen(new_number_location);
        if (new_numberlen > 0)
	    strcpy(ddb->number_called, new_number_location);
    } else {
	new_numberlen = 0;
    }

    for (i = 0; i < DIALERHASH; i++) {
	for (p = dialertable[i]; p; p = p->next) {
	    if (master_ddb != p->ddb)
		continue;
	    for (e = p->entry; e; e = e->next) {
		if (e->dialer_string == NULL)
		    continue;
		/* Old number going DOWN... */
		if ((old_numberlen > 0) &&
		    (strcmp(e->dialer_string, ddb->number_called) == 0)) {
		    reg_invoke_dialer_number_called_changed
			(p->link, real_idb, master_idb, &p->proto_addr, FALSE);
		}
		/* New number going UP... */
		if ((new_numberlen > 0) &&
		    (strcmp(e->dialer_string, new_number_location) == 0)) {
		    reg_invoke_dialer_number_called_changed
			(p->link, real_idb, master_idb, &p->proto_addr, TRUE);
		}
	    }
	}
    }

    /* Actually copy the thing... */
    if (new_numberlen <= 0) {
	ddb->number_called[0] = '\0';
	/*
	 * For Snapshot's server side needs, indicate a down interface
	 * with a 'SNAPSHOT_SERVER_DIALER_ADDR' map address.
	 */
	addr.snapshot_addr = SNAPSHOT_SERVER_DIALER_ADDR;
	addr.type = ADDR_SNAPSHOT;
	addr.length = ADDRLEN_SNAPSHOT;
	reg_invoke_dialer_number_called_changed
	    (LINK_SNAPSHOT, real_idb, master_idb, &addr, FALSE);
    }
}

/*
 * dialer_cannot_dial
 * Called when we've reached the end of the list of dialer strings without
 * connecting a call.
 */
void dialer_cannot_dial (dialerdbtype *ddb, dialerpaktype *dpak)
{
    dialerdbtype *profileddb;

    if (ddb->dialer_call_group) {
	ddb->dialer_call_group->cip = NULL;
	ddb->dialer_call_group = NULL;
    }
    ddb->dial_reason[0] = '\0';
    if (DIALPOOL_MEMBER(ddb)) {
	profileddb = ddb->dialing_ddb;
	if ((profileddb != NULL) ||
	    (dpak && (profileddb = dpak->ddb))) { 
	    dialer_call_failed(ddb, profileddb);
	}
	
    }
    ddb->dialing_ddb = NULL;
    if (dpak) {
	if (dpak->map) {
	    dpak->map->cip = NULL;
	    dpak->map = NULL;
	}
	if (dpak->header != NULL) {
	    free(dpak->header);
	}
	dpak->header = NULL;
	/* The below if should never, ever be true - but keep it anyway */
	if (dpak->holdQ && (dpak->holdQ != ddb->dialer_holdQ))
	    dialer_holdq_discard_queue(&dpak->holdQ, "Dialing failed");
    }
    dialer_holdq_discard_queue(&ddb->dialer_holdQ, "Dialing failed");
}

/*
 * dialer_set_call_data
 * Set call parameters after placing a call, but before it is up.
 */
void dialer_set_call_data (dialerdbtype *ddb, dialerpaktype *dpak)
{
    dialer_set_number_called(ddb, dpak->dial_number);
    set_dialer_state(ddb, DIALER_PENDING);
    ddb->dialer_placed_call = TRUE;
    ddb->dialing_ddb = dpak->ddb;
    ddb->dialer_call_group = dpak->map;
    ddb->dialer_stats = dpak->stats;
    if (ddb->dialer_holdQ) {
	if (dialer_debug || dialer_holdq_debug)
	    buginf("\n%s - already had holdq - overwriting",
		   ddb->ownidb->hw_namestring);
    }
    ddb->dialer_holdQ = dpak->holdQ;
    if (!*ddb->dial_reason) {
	sstrncpy(ddb->dial_reason, dpak->header, MAXSTR);
    } else if (dpak->header) {
	free(dpak->header);
    }
    dpak->header = NULL;
}


/*
 * dial_if
 * Dial an interface.  
 *  dialed_str = the number found found in the dialer config that was dialed.
 */
boolean dial_if (dialerdbtype *ddb, dialerpaktype *dpak)
{
    boolean success;
    dialermaptype *map;
    char *dial_number;
    dialer_stat *ds;
    hwidbtype *idb;
    dialerdbtype *netddb;
    mapclass_type *mapclass;
    dialer_params_type *params;

    idb = ddb->ownidb;

    /*
     * First check that we are not attempting to place more calls than we
     * are allowed to.  X.25 and FR limit to 1 the number of links that can
     * be active simultaneously in a rotary group.
     */

    netddb = dpak->ddb;
    if (IN_HUNTGROUP(ddb) || DIALPOOL_MEMBER(ddb)) {
      if (!IS_CALL_ALLOWED(netddb)) {
	    if (dialer_debug) {
		buginf("\n%s: Already %d call(s) in progress on %s, dialing not"
		       " allowed", idb->hw_namestring, netddb->dialer_num_call,
		       netddb->ownidb->hw_namestring);
	    }
	    dialer_cannot_dial(ddb, dpak);
	    return(FALSE);
	}
    }

    map = dpak->map;
    dial_number = NULL;
    ds = NULL;

    /*
     * Find the dial string, and the associated statistics.
     */
    if (DTR_DIALER(ddb)) {
	dial_number = dialer_dtr;
	ds = &ddb->dialer_dtr_stats;
    } else {
	if (map == NULL) {
	    map = netddb->dialer_string;
	}

	if (map != NULL) {
	    /*
	     * Get the next phone number in the list and make it 
	     * "call-in-progress"
	     */
	  if (map->cip == NULL)
		map->cip = map->entry;
	    else 
		map->cip = (map->cip)->next;

	    /*
	     * If there is a next phone number initialize data structures,
	     * else make sure we exit right after this test and reset the idb 
	     * call group pointer.
	     */
	    if (map->cip) {
        	dial_number = (map->cip)->dialer_string;
		ds = &(map->cip)->stat;
	    }
	} else if (dialer_debug) {
	    buginf("\n%s: No dialer string, dialing cannot occur",
		   idb->hw_namestring);
	}
    }

    /*
     * If no dialer_string is defined, then bail out now as things are
     * obviously misconfigured.
     */
    if (!dial_number) {
      dialer_cannot_dial(ddb, dpak);
	return(FALSE);
    }

    /*
     * Set the map class parameters
     */
    if (map && map->cip && (map->cip)->class) {
        mapclass = mapclass_lookup((map->cip)->class, ET_DIALER);
        if (mapclass) {
            if (mapclass->params) {
                params = (dialer_params_type *)mapclass->params;

                if (params->idle_ticks)	/* idle timer */
		  ddb->dialer_idle_ticks = params->idle_ticks;

                if (params->fast_idle_ticks) /* fast idle timer */
		  ddb->dialer_fast_idle_ticks = params->fast_idle_ticks;

                if (params->enable_ticks) /* enable timer */
		  ddb->dialer_enable_ticks = params->enable_ticks;

                if (params->carrier_ticks) /* carrier timer */
		  ddb->dialer_carrier_ticks = params->carrier_ticks;
            }
        }
    }

    /*
     * Now actually dial the number
     */

    dpak->dial_number = dial_number;
    dpak->stats = ds;
    /*
     * If we don't have a holdQ already in the dpak (which would have
     * been transferred from a previous call or for a callback), create
     * a new holdQ - the create_dialer_holdq will only malloc one if
     * one is configured
     */
    if (dpak->holdQ == NULL) {
	dpak->holdQ = create_dialer_holdq(ddb);
    } else {
	if (dialer_holdq_debug)
	    buginf("\n%s: took holdq from dpak %x",
		   ddb->ownidb->hw_namestring, dpak->holdQ);
    }
		
    if (dialer_debug)
        buginf("\n%s: Attempting to dial %s", 
	       idb->hw_namestring, dial_number);

    success = reg_invoke_dialer_dial(ddb->dialer, ddb, dpak);

    if (success) {
        if (ddb->ownidb->type != IDBTYPE_VACCESS)
           dialer_increment_call(ddb);	   /* one more line/Bchan in use */
	GET_TIMESTAMP(ds->last_call_time);
	GET_TIMESTAMP(ds->curr_call_time);
    } else {
	if (dialer_debug)
            buginf("\n%s: Unable to dial %s.", idb->hw_namestring, 
		   dial_number);
	ds->num_failures++;
	ds->last_call_success = FALSE;
	dialer_fail(ddb);
	dialer_cannot_dial(ddb, dpak);
    }
    return(success);
}

/*
 * dialer_fail - Signal the failure of a dialing attempt
 *
 * This routine is used to clean up after a dialing attempt that has failed.
 * Also called through the dialer_dial_failed  service point.
 * Most other media are handled through wait-carrier-timeout for
 * asynchronous failures, but Asynch media would like to have a faster
 * reporting interface.
 */

void dialer_fail (dialerdbtype *ddb)
{
    hwidbtype *idb;
    dialer_stat *ds;
    callback_type *call;

    dialer_set_number_called(ddb, NULL);

    call = find_callback_by_dial_string(ddb->number_called);
    if (call)
	remove_callback(call);
    
    GET_TIMESTAMP(ddb->mru_lastfailure);

    if (HUNTGROUP_LEADER(ddb))
	return;

    ddb->dialer_spoofing_prot = FALSE;
    ddb->dialer_spoofing_int = FALSE;
    ddb->dialer_placed_call = FALSE;
    TIMER_STOP(ddb->dialer_idle_timer);
    TIMER_STOP(ddb->dialer_fast_idle_timer);
    TIMER_STOP(ddb->dialer_carrier_timer);
    TIMER_STOP(ddb->dialer_enable_timer);   /* Possibly restarted below */

    ds = ddb->dialer_stats;
    if (ds) {
	ds->num_failures++;
	ds->last_call_success = FALSE;
    }
    ddb->dialer_stats = NULL;
    idb = ddb->ownidb;
    if (idb->state != IDBS_ADMINDOWN) {
	idb->state = IDBS_DOWN;
	set_dialer_state(ddb, DIALER_WAITING);
	TIMER_START(ddb->dialer_enable_timer, ddb->dialer_enable_ticks);
    }
}

/*
 * dialer_failed
 * Let dialer_fail be called from a registry.
 */
static void dialer_failed (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb == NULL)
	return;

    dialer_disconnect(idb);
    dialer_decrement_call(ddb);
    dialer_fail(ddb);
}


/*
 * pick_best_dialer_idb
 * Compare two interfaces, and return the "best" one for transmission.
 */
dialerdbtype *pick_best_dialer_idb (
    dialerdbtype *testddb,
    dialerdbtype *bestddb,
    ulong *queuelen,
    sys_timestamp *lastoutput)
{
    hwidbtype *testidb;
    ulong test_queuelen;
    sys_timestamp test_lastoutput;

    /*
     * Get data from the test idb.
     */
    testidb = testddb->ownidb;
    test_queuelen = testidb->output_qcount;
    if (testidb->txqlength)
	test_queuelen += (testidb->txqlength)(testidb);
    test_lastoutput = testidb->lastoutput;

    /*
     * bestddb is the current best idb, compare it to testddb.
     */
    if (bestddb) {
        /*
         * Don't use this interface if the fast idle timer
         * is running, and we have found an interface that
         * is not running the fast idle timer.
         *
         */
        if (TIMER_RUNNING(testddb->dialer_fast_idle_timer) &&
	    !TIMER_RUNNING(bestddb->dialer_fast_idle_timer))
            return (bestddb);

        /*
         * Don't use this interface if the queue is longer.
         */
        if (test_queuelen > *queuelen)
            return (bestddb);

	/*
	 * It's possible that we don't really have enough info
	 * above to make a good decision.  That's because some
	 * devices don't support txqlength(), so there may be
	 * data waiting to go out we don't know about.  So, if
	 * txqlength() doesn't exist, try to fake it by looking
	 * at the last output time.  If the last output time of
	 * this interface is later than the best one, skip
	 * this one.   Only do this if the length of the two
	 * queues are equal.
	 *
	 * This has the side effect of doing a round robin when
	 * the interfaces are basically idle.
	 */
	if ((!testidb->txqlength) && 
	    (test_queuelen == *queuelen) &&
	    (TIMER_LATER(test_lastoutput, *lastoutput)))
	    return (bestddb);

        /*
         * Don't use this interface if the queue is the same
         * length, but the interface priority is lower.
         */
        if ((*queuelen == test_queuelen) &&
	    (testddb->dialer_priority < bestddb->dialer_priority))
            return (bestddb);
    }

    /*
     * By now, we have found a new interface to consider the best.  Return
     * everything for the next comparison.
     */
    *queuelen = test_queuelen;
    *lastoutput = test_lastoutput;
    return (testddb);
}


/*
 * find_best_output_idb
 * This interface is the hunt group lead. Look through all
 * interfaces, and find the connected interface with the
 * shortest queue.
 */
static dialerdbtype *find_best_output_idb (
    dialerdbtype *ddb, 
    dialerpaktype *dpak)
{
    dialerdbtype *bestddb, *tempddb, *testddb;
    ulong queuelen;
    sys_timestamp lastoutput;

    bestddb = NULL;		/* No best IDB so far */
    queuelen = 0;		/* No queue length so far */
    GET_TIMESTAMP(lastoutput);
    FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
        /*
         * Get all interfaces for comparison.  If a rotary group leader, get
	 * the best interface among the members.
         */
        if (HUNTGROUP_LEADER(tempddb)) {

	    testddb = find_best_output_idb(tempddb, dpak);
	} else if (DIALER_UP(tempddb) && dialer_connected(tempddb, dpak->map)) {
            testddb = tempddb;
	} else {
	    continue;
	}
	if (testddb) {
	  bestddb = pick_best_dialer_idb(testddb, 
					 bestddb, &queuelen, &lastoutput);
	}
    }
    return(bestddb);
}

/* 
 * connected_to
 * This routine checks to see if any of the dialer interfaces are connected
 * to the given address.
 */

static dialerdbtype *connected_to (dialerdbtype *ddb, dialerpaktype *dpak)
{

    if (HUNTGROUP_LEADER(ddb)) {
	/*
	 * This interface is the hunt group lead. Look through all
	 * interfaces, and find the connected interface with the
	 * shortest queue.
	 */
	return(find_best_output_idb(ddb, dpak));
    } else if (DIALER_UP(ddb) && dialer_connected(ddb, dpak->map)) {
	return(ddb);
    }

    return(NULL);
}

/*
 * dialer_connected_to_inline
 * Implements function below.
 */
static inline boolean dialer_connected_to_inline (
    dialerdbtype *ddb,
    dialermaptype *map)
{
    dialerdbtype *tempddb;

    if (HUNTGROUP_LEADER(ddb)) {
	/*
	 * This interface is the hunt group lead. Look through the
	 * interfaces in the hunt group, and accept the first that
	 * is connected as proving the point. Recursive to cover
	 * rotaries of rotaries...
	 */
	FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
	    if (dialer_connected_to_inline(tempddb, map)) {
		return(TRUE);
	    }
	}
	return(FALSE);
    } else {
	return(dialer_connected(ddb, map));
    }
}

/*
 * dialer_connected_to
 * registry invoked function to return TRUE if we are connected to this
 * address.
 */
static boolean dialer_connected_to (hwidbtype *output, addrtype *proto_addr, uint link)
{
    dialerdbtype *ddb;
    dialermaptype *map;

    ddb = get_dialerdb(output);
    map = dialer_addr_lookup(ddb, proto_addr, link);
    return(dialer_connected_to_inline(ddb, map));
}

/*
 * in_transition()
 *	Tell if we think our IDB is still flapping state
 */
inline static boolean in_transition (hwidbtype *idb)
{
    if (!(mgd_timer_running(&idb->carriertimer))) {
	return(FALSE);
    }
    return(idb->state != idb->oldstate);
}

/*
 * dialer_is_free
 * Returns TRUE if this interface is available to place a call.
 */
boolean dialer_is_free (dialerdbtype *ddb)
{

    return(DIALER_FREE(ddb));
}

/* 
 * get_free_dialer
 * This routine checks to see if any of the dialer interfaces are free.
 * If so, that interface is returned.
 */

dialerdbtype *
get_free_dialer (dialerdbtype *ddb)
{
    hwidbtype *idb;
    dialerdbtype *bestddb, *tempddb;
    dialerdbtype    *worstddb, *leastworstddb,
		    *nextworstddb, *zeroddb,
		    *priorityddb;
    sys_timestamp time_zero;

    memset(&time_zero, 0, sizeof time_zero);
    worstddb=leastworstddb=nextworstddb=zeroddb=priorityddb = NULL;

    idb = ddb->ownidb;

    if (HUNTGROUP_LEADER(ddb)) {

	/*
	 * Dialer group. Look for a free interface with the highest
	 * priority.
	 */

	bestddb = NULL;		/* No best IDB so far */

        FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
	    /*
	     * Don't try to use a dialer which is administratively down, 
	     * or in transition.
	     */
	    if (DIALER_SHUT(tempddb) || in_transition(tempddb->ownidb)) {
		continue;
	    }

	    /*
	     * Don't try to use a dialer that is not free.
	     */
	    if (!reg_invoke_dialer_is_free(tempddb->dialer, tempddb)) {
		continue;
	    }

	    if (!priorityddb ||
		(priorityddb->dialer_priority < tempddb->dialer_priority)) {
		priorityddb = tempddb;	/* save best ddb [by priority] so far */
	    }

	    if (ddb->dialer_rotor_style != DIALER_ROTOR_BEST) {
		continue;   /* skip un-necessary stuff */
	    }

	    if ((zeroddb == NULL) && TIMERS_EQUAL(time_zero, tempddb->mru_lastfailure)) {
		zeroddb = tempddb;  /* contingencies */
	    }

	    if (zeroddb &&
		(zeroddb->dialer_priority < tempddb->dialer_priority)) {
		zeroddb = tempddb;  /* save the best unused ddb so far */
	    }

	    if (worstddb == NULL) {
		leastworstddb = worstddb = tempddb;
	    } else {
		if (TIMER_EARLIER(worstddb->mru_lastfailure, tempddb->mru_lastfailure)) {
		    nextworstddb = worstddb;
		    worstddb = tempddb;
		}
		if (TIMER_LATER(leastworstddb->mru_lastfailure, tempddb->mru_lastfailure)) {
		    leastworstddb = tempddb;
		}
	    }

	    if (!bestddb ||
		(bestddb->dialer_priority < tempddb->dialer_priority)) {
		bestddb = tempddb; /* Save best DDB so far */
	    }
	}

	/*
	 * if there are ever more than two cases, this should be a switch()
	 */
	if (ddb->dialer_rotor_style == DIALER_ROTOR_BEST) {
	    if (leastworstddb == worstddb) {
		if (dialer_debug) {
		    buginf("\n%s: rotor dialout [best] "
			   "least recent failure is also most recent failure",
			   worstddb->ownidb->hw_namestring);
		}
		if (zeroddb != NULL) {
		    if (dialer_debug) {
			buginf("\n%s: rotor dialout [best] "
			       "trying untried dialout",
			       zeroddb->ownidb->hw_namestring);
		    }
		    leastworstddb = zeroddb;
		}
	    }

	    if (bestddb == worstddb) {
		if (dialer_debug) {
		    buginf("\n%s: rotor dialout [best] "
			   "also has most recent failure",
			   bestddb->ownidb->hw_namestring);
		}
		bestddb = leastworstddb;
	    }
	    if (dialer_debug) {
		buginf("\n%s: rotor dialout [best]", 
		       bestddb->ownidb->hw_namestring);
	    }
	} else {
	    if (priorityddb != NULL) {
		bestddb = priorityddb;
		if (dialer_debug) {
		    buginf("\n%s: rotor dialout [priority]",
			   bestddb->ownidb->hw_namestring);
		}
	    }
	}

	if (bestddb != NULL) {
	    GET_TIMESTAMP(bestddb->last_getdialer);
	}

	return(bestddb);
    } else if (DIALER_FREE(ddb)) {
	return(ddb);             /* return this ddb, if not in use */
    }

    return(NULL);
}

/* 
 * start_fast_idle
 * This routine starts the fast idle timer for all dialers in a group.
 * For an isdn interface, that means start the fast idle timer for both
 * B channels if they are in use.  For a regular interface, just start
 * the fast idle timer for that interface only.
 */
void start_fast_idle (dialerdbtype *ddb)
{
    dialerdbtype *tempddb;
    sys_timestamp temptime;

    if (HUNTGROUP_LEADER(ddb)) {
	FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
	    start_fast_idle(tempddb);
	}
    } else {
	/*
	 * Only start the fast idle timer if it isn't already running
	 */
	if (TIMER_RUNNING(ddb->dialer_fast_idle_timer))
	    return;
	/*
	 * Only start the fast idle timer on interfaces that are in use
	 * and are not currently in the process of placing a call.
	 */
	if (DIALER_CALLUP(ddb)) {
	    /*
	     * Check to see if the line has been idle for the fast idle
	     * period.  If so, set the fast idle timer to now, so the line
	     * gets hung up right away.
	     */
	    temptime = TIMER_SUB_DELTA(ddb->dialer_idle_timer,
				       ddb->dialer_idle_ticks);
            if (CLOCK_OUTSIDE_INTERVAL(temptime, ddb->dialer_fast_idle_ticks)) {
		GET_TIMESTAMP(ddb->dialer_fast_idle_timer);
	    } else {
        	TIMER_START(ddb->dialer_fast_idle_timer, 
			    ddb->dialer_fast_idle_ticks);    
	    }
	}
    }
}

/*
 * dialer_execute_save_vencap
 * Send a packet to its ultimate destination, with saved encapsulation.
 * Routine also checks for SNAPSHOT_PAK_IS_UPDATE flag, and if set,
 * asks Snapshot if the packet can be sent out this hwidb.
 */
static boolean dialer_execute_save_vencap (dialerdbtype *ddb, paktype *pak,
				    long addr)
{
    hwidbtype *output;

    output = ddb->ownidb;
    if (pak->snapshot_flags & SNAPSHOT_PAK_IS_UPDATE) {
	if (reg_invoke_snapshot_dialer_update_denied(output)) {
	    return(FALSE);
	}
    }
    
    if (!(*ddb->dialer_save_vencap)(pak, addr))
	return(FALSE);

    return(TRUE);
}

/*
 * dialer_holdq_output
 * Empty the dialer hold queue, transmit all packets.
 */
static void dialer_holdq_output (dialerdbtype *ddb, uint link)
{
    queuetype *queue;
    paktype *pak;
    elementtype *elem, *nextelem;
    hwidbtype *idb;
    uint dcount, tcount;
    uint count;

    if (ddb->dialer_holdQ == NULL)
	return;

    queue = &ddb->dialer_holdQ->queue;
    if (DIALER_HOLDQ_EMPTY(queue)) {
	dialer_holdq_free(&ddb->dialer_holdQ);
	return;
    }

    if (ddb->dialer_save_vencap == NULL) {
        dialer_holdq_discard_queue(&ddb->dialer_holdQ, "Encapsulation error");
        return;
    }

    idb = ddb->ownidb;
    dcount = 0;
    tcount = 0;
    if (link == LINK_ALL) {
	/*
	 * For all packets in the queue: unqueue, encapsulate and transmit.
	 */
      while ((pak = pak_dequeue(queue))) {
	    pak->if_output = idb->firstsw;
            if (dialer_execute_save_vencap(ddb, pak, pak->dialer_save_address)){
        	datagram_out(pak);
        	tcount++;
            } else {
        	datagram_done(pak);
        	dcount++;
            }
	}
    } else {
	/*
	 * For all packets in the queue, unqueue and transmit those that
	 * match the given protocol type.  This is to accomodate those
	 * encapsulations (PPP) for which protocols are enabled one at a time.
	 */
        elem = queue->qhead;
	while (elem != NULL) {
	    nextelem = elem->next;
	    pak = elem->pakptr;
	    if (pak != NULL) {
		if (pak->dialer_save_type == link) {
		  pak = pak_unqueue(queue, pak);
		    pak->if_output = idb->firstsw;
		    if (dialer_execute_save_vencap(ddb, pak, 
						   pak->dialer_save_address)) {
			datagram_out(pak);
			tcount++;
		    } else {
			datagram_done(pak);
			dcount++;
		    }
		}
	    }
	    elem = nextelem;
	}
    }

    if (DIALER_HOLDQ_EMPTY(queue))
	dialer_holdq_free(&ddb->dialer_holdQ);

    if (dialer_debug || dialer_holdq_debug) {
        count = dcount + tcount;
        buginf("\n%s: %s, %d packets unqueued, %d transmitted, %d discarded",
                idb->hw_namestring, "Call connected", count, tcount, dcount);
    }

    /*
     * If we have transmitted any packets, reset idle timer
     */
    if (tcount > 0) {
	START_IDLE_TIMER(ddb);
    }
}

/*
 * dialer_holdq_discard_queue
 * Empty the dialer hold queue, discard all packets, free the holdq
 * note that we pass in the address of the pointer to the holdq - so
 * that we can NULL it out.
 */
void dialer_holdq_discard_queue (dialer_holdq_type **holdq_ptr, char *reason)
{
    dialer_holdq_type *holdq;
    paktype *pak;
    uint count;
    
    if (holdq_ptr == NULL)
	return;

    holdq = *holdq_ptr;
    if (holdq == NULL)
	return;

    count = 0;
    
    if (!(QUEUEEMPTY(&holdq->queue))) {
	while ((pak = pak_dequeue(&holdq->queue))) {
	    datagram_done(pak);
	    count++;
	}
    }

    dialer_holdq_free(holdq_ptr);
    if (dialer_debug || dialer_holdq_debug)
        buginf("\n: %s, %d packets unqueued and discarded",
	       reason, count);
    
}

/*
 * dialer_enqueue_pak
 * Enqueue packet into the dialer hold queue
 * holdq -> points to the holdq
 * pak -> packet to queue
 * address = protocol address of packet
 * reason -> char string why packet enqueued
 */
void dialer_enqueue_pak (dialer_holdq_type *dq, paktype *pak, long address)
{
    queuetype *holdq;
    paktype *newpak;

    if (dq == NULL)
	return;
    holdq = &dq->queue;
    if ((!holdq) ||
	QUEUEFULL(holdq) ||
	(QUEUEMAXIMUM(holdq) == 0)) 
	return;
    
    /*
     * For CLNS packet, need to allocate bigger buffer to copy address
     * over because clns passes a address pointer which points to its
     * process local stack, we shouldn't reuse it.
     */
    if (DIALER_NEW_ADDR_LINKTYPE(pak)) {
	reg_invoke_clns_pak_duplicate(pak, &newpak, &address);
    } else {
	newpak = pak_duplicate(pak);
    }
    if (newpak != NULL) {
        /*
         * Queue packet "as is", save protocol address for when we perform
         * encapsulation.  If packet is unqueueable, discard it.
         */
	newpak->dialer_save_address = address;
	if (pak_enqueue(holdq, newpak) != NULL) {
	    if (dialer_packet_debug)
		buginf("\nPacket queued");
	    return;
	} else {
	    datagram_done(newpak);
	}
    }
    return;
}

/*
 * dialer_protocol_up
 * Take actions necessary when the specified protocol goes up.  For HDLC
 * and X.25 this is when line protocol goes up; for PPP it is when an NCP 
 * is OPEN.
 *
 * ***** CAUTION:  this function is called at interrupt level in the following
 *    cases:
 *             X.25, ppp reliable-link (lapb)
 */
static void dialer_protocol_up (hwidbtype *idb, uint link)
{
    dialerdbtype *ddb, *profileddb;
    callback_type *call;

    /*
     * Clear the spoofing flag if we are a dialer.  This way we now
     * track the real state of line protocol.  Also set 'lineup' to
     * indicate that the line is open for transmission (as opposed
     * to down or call pending), link member into a call group, and 
     * flush hold queue.
     */
    if (idb == NULL) {
	return;
    }

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {

	/*
	 * Go through here only if the call is active.  This could happen
	 * if the call is being disconnected before being fully up.
	 */
	if (!DIALER_INPROGRESS(ddb))
	    return;

	/*
	 * If this interface is a member of a rotary group, add it to the
	 * connected member linked list.
	 */
	if (SLAVE_DIALER(ddb)) {
	    if (dialer_link_member(ddb)) {
		/*
		 * Stop call pending timer.
		 */
		
		TIMER_STOP(ddb->conn_group->call_timer);
	    }
	}

        ddb->dialer_spoofing_prot = FALSE;
        ddb->dialer_spoofing_int = FALSE;
	set_dialer_state(ddb, DIALER_OPERATIONAL);

	/*
	 * If this is a dialer pool member, set new state on the profile.
	 */
	if (DIALPOOL_MEMBER(ddb)) {
	    profileddb = ddb->netcfg_ddb;
	    if (!DIALER_UP(profileddb)) {
		set_dialer_state(profileddb, DIALER_OPERATIONAL);
		profileddb->dialer_fsm = dialer_fsm_up;
	    }
	}

	ddb->callback_type = CALLBACK_NONE;
	call = find_callback_by_call(ddb);
	if (call) {
	    /*
	     * Callback structure found for this call - copy
	     * the holdq and remove the callback.
	     * We do this no matter if we were the client, or the
	     * server.  If we were the server, something very
	     * interesting happened.  We don't want to make another
	     * call, so just go with the flow.
	     */
	    if (call->callback_type == CALLBACK_SERVER) {
		if (dialer_debug) {
		    buginf("\n%s callback server and dialer protocol up ?"
			   "- must be mlp",
			   ddb->ownidb->hw_namestring);
		}
		return;
	    }
			   
	    if (ddb->dialer_holdQ) {
		dialer_holdq_discard_queue(&ddb->dialer_holdQ,
					   "Already had a callback holdQ");
	    }
	    ddb->dialer_holdQ = call->holdQ;
	    call->holdQ = NULL;
	    if (dialer_debug) {
		buginf("\nCallback received on %s from %s %s",
		       ddb->ownidb->hw_namestring,
		       call->remote_name,
		       call->number_called);
	    }
	    remove_callback(call);
	}
	dialer_holdq_output(ddb, link);
    }
}


/*
 *
 */
boolean dialer_place_call (dialerdbtype *outddb, dialerpaktype *dpak)
{
    hwidbtype *outidb;
    paktype *pak;

    pak = dpak->pak;
    outidb = outddb->ownidb;
    dpak->header = malloc_named(MAXSTR, "Dialer Header");
    if (dpak->header != NULL) {
	dialer_get_packet_header(pak, dpak->header);
	if (dialer_debug) {
	    buginf("\n%s: Dialing cause %s", outidb->hw_namestring, 
		   dpak->header);
	}
    }
    pak->if_output = outidb->firstsw;
    if (dial_if(outddb, dpak)) {
	/*
	 * enqueue the packet in holdQ - holdq will be set up during dial_if
	 */
	dialer_holdq_enqueue(dpak);
	pak->dialer_flags |= PAK_DDR_CALL_PENDING;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * dialer_process_packet
 * This routine checks to see if there is a connected interface for this
 * packet and possibly place a call to the required destination.
 */
static dialerdbtype *dialer_process_packet (dialerdbtype *ddb, dialerpaktype *dpak) 
{
    paktype *pak;
    hwidbtype *outidb;
    dialerdbtype *outddb;
    callback_type *callback;
    /*
     * If this packet is a type for TCP/IP header compression, then
     * simply indicate that the packet should be output - the initial
     * check has already been done when the packet was first encapsulated
     * as a regular IP packet.
     */
    pak = dpak->pak; 
    if ((pak->linktype == LINK_UNCOMPRESSED_TCP) ||
	(pak->linktype == LINK_COMPRESSED_TCP))
	return(ddb);
    
    /*
     * First check for a passive interface, i.e. an interface with neither
     * a dialer string, nor a dialer-group.
     */
    if (!ddb->dialer_list) {
	/*
	 * If this is a rotary group leader, just return as those packets 
	 * don't travel this path.  This case could occur if there is no 
	 * dialer list on the interface.
	 * This code is now redundant since the next test is always 
	 * FALSE for rotary group leaders.
	if (HUNTGROUP_LEADER(ddb)) {
	    if (dialer_debug) {
		buginf("\n%s: Passive interfaces not supported on rotary group",
		       ddb->ownidb->hw_namestring);
	    }
	    return(NULL);
	}
	 */
	
	/*
	 * If the line is connected, reset the timers.
	 * NOTE: if no dialer-list is defined for an interface, we will
	 * always keep the interface up.  This is to assist "passive"
	 * interfaces in ease of configuration.  This is the opposite of
	 * dialing though, where dialing will never occur.
	 */
	if (DIALER_UP(ddb)) {
	    START_IDLE_TIMER(ddb);
	    return(ddb);
	}
	return(NULL);
    }

    /*
     * Check to see if we are connected to the destination of this
     * packet.  If so, then if it is interesting, reset the timers.
     */
    dpak->map = dialer_addr_lookup(ddb, dpak->proto_addr, dpak->linktype);
    if (dpak->map == NULL) {
	dpak->map = ddb->dialer_string;
    }
    outddb = connected_to(ddb, dpak);
    if (outddb != NULL) {
	/*
	 * We are connected to the destination: get ready for transmission.
	 */
	outidb = outddb->ownidb;
	pak->if_output = outidb->firstsw;	/* physical output idb */

	/*
	 * If this is an interesting packet, then reset the timers.
	 */
        if (dpak->interesting) {
	    START_IDLE_TIMER(outddb);
        }
	return(outddb);
    } else if (!dpak->interesting) {
	/*
	 * We don't have a link connected to the destination, and the
	 * packet is not interesting: return now.
	 */
	return(NULL);	/* uninteresting and not connected */
    }

    /*
     * We are not connected, but the packet is interesting: check whether
     * we have a call pending, or whether we should place a call.
     */
    if ((outddb = reg_invoke_dialer_pending(ddb->dialer, ddb, dpak))) {
	/*
	 * If there is a call pending to this destination, attempt to
	 * queue the packet in the dialer hold queue.  The pending routines
	 * need to set up dpak->holdQ.
	 */
	dialer_holdq_enqueue(dpak);
        pak->dialer_flags |= PAK_DDR_CALL_PENDING;
    } else if ((callback = callback_pending(dpak))) {
	/*
	 * Do not make a call if callback is pending, either waiting to dial
	 * back, or waiting to be dialed.  Callback timer expiration will
	 * dial the return call (server) or time out (client) or we will
	 * answer the return call (server).
	 * If a callback is pending, enqueue the packet so that it can
	 * be sent when the return call is connected (if holdq is allocated).
	 */
	dialer_enqueue_pak(callback->holdQ, pak, dpak->address);
    } else if ((outddb = reg_invoke_get_free_dialer(ddb->dialer, ddb))) {
	/*
	 * We aren't connected to the destination and there is no call pending:
	 * we have a candidate for placing a call.  So, check to see if
	 * there is a free dialer to place a call on.  If so, place the
	 * call.  If not, then start the fast idle timers on all interfaces.
	 */
       dialer_place_call(outddb, dpak);
    } else if (find_dialer_string(ddb, dpak)) {
  	/*
 	 * Before we start the fast idle timer, make sure that there
 	 * is a phone number to be called for this packet.  That way
 	 * if we get an interesting packet that doesn't have a dialer
 	 * string, we won't bother to start the fast idle timer(s).
  	 */
	if (dialer_debug)
	    buginf("\n%s: No free dialer - starting fast idle timer",
		   ddb->ownidb->hw_namestring);
		
        start_fast_idle(ddb);
    }
    return(NULL);	/* interesting but not connected */
}

/*
 * dialer_process_broadcast
 * Deal with an individual map and broadcast packet
 * Using the passed in dialer map, see if we should/can send this packet
 * out.  Returns first_idb if this was the first success
 */

static void dialer_process_broadcast (
    dialerdbtype *ddb,
    dialerpaktype *dpak, 
    idbtype **first_idb) 
{
    paktype *pak, *newpak;

    pak = dpak->pak;
    ddb = dialer_process_packet(ddb, dpak);
    if (ddb != NULL) {
	/*
	 * Save the first successful destination away for
	 * later.  This will be transmitted by the caller
	 * so that we don't erronously have an encapsulation
	 * failure when we really sent something.
	 */
        if (*first_idb == NULL) {
	    *first_idb = pak->if_output;
	} else {
            if (ddb->dialer_save_vencap) {
	        newpak = pak_duplicate(pak);
		if (newpak) {
                    if (!dialer_execute_save_vencap(ddb, newpak, dpak->address)) {
                        if (dialer_packet_debug)
		            buginf(" -- failed");
	                datagram_done(newpak);
	            } else {
	                datagram_out(newpak);
                        if (dialer_packet_debug)
		            buginf(" -- succeeded");
	            }
	        }
	    }
	}
    } else {
        if (dialer_packet_debug)
	    buginf(" -- failed, not connected");
    }
}

/*
 * dialer_broadcast
 * Deal with a broadcast packet sent to a DDR interface.  This means
 * sending it out if dialer string or the broadcast flag on the dialer
 * map if already connected, or placing a call if interesting and if
 * dialer string  or the broadcast flag on the dialer map.
 */

static dialerdbtype *dialer_broadcast (dialerdbtype *ddb, dialerpaktype *dpak)
{
    idbtype *tmpidb;
    hwidbtype *tmphwidb;
    paktype *pak;
    int i;
    dialermaptype *p;
    dialerentrytype *e;
    char *buffer;
    idbtype *first_idb;

    /*
     * If this packet is a type for TCP/IP header compression, then
     * simply indicate that the packet should be output - the initial
     * check has already been done when the packet was first encapsulated
     * as a regular IP packet.
     */
    pak = dpak->pak;
    if ((pak->linktype == LINK_UNCOMPRESSED_TCP) ||
        (pak->linktype == LINK_COMPRESSED_TCP)) {
	return(ddb);
    }

    buffer = string_getnext();
    tmpidb = pak->if_output;
    tmphwidb = ddb->ownidb;
    first_idb = NULL;

    /*
     * First loop through all of the maps and send this out to any that
     * are currently connected, and have the broadcast flag set.
     */

    for (i = 0; i < DIALERHASH; i++) {
        for (p = dialertable[i]; p; p = p->next) {
    	    for (e = p->entry; e; e = e->next) {
		/* 
		 * If this packet is for this map, and the map has 
		 * broadcast set then process it.  There is a break
		 * inside this if as if we find one map with the
		 * broadcast keyword, then we send it out to this
		 * next hop.
		 */
		if ((pak->dialer_save_type == p->link) && 
		     (ddb == p->ddb) &&
		      (e->broadcast)) {
		    /* 
		     * Since we are passing pak to dialer_process_packet,
		     * which can mangle pak->if_output, restore it to what
		     * was passed into the routine.  Then call
		     * dialer_process_packet to output this broadcast packet
		     * if we are connected, or to place a call if not and
		     * interesting.
		     *
		     * We call dialer_process_broadcast with the "next hop"
		     * address taken from the map so we are guaranteed that
		     * dialer_process_packet will match a dialer map and not
		     * the dialer string.  Then all dialer_process_packet
		     * has to do is decide if we are connected or if
		     * we have to dial.
		     */
		    pak->if_output = tmpidb;
		    if (dialer_packet_debug) {
			dialer_print_proto_addr(tmpidb, &p->proto_addr, 
						p->link, buffer);
			buginf("\n%s: sending broadcast to %s", 
			        tmphwidb->hw_namestring, buffer);
		    }
		    dpak->proto_addr = &p->proto_addr;
		    dialer_process_broadcast(ddb, dpak, &first_idb);
		    break;
		}
	    }
	}
    }

    /*
     * We've checked all the dialer maps, so now check the dialer string.
     * We do this by calling dialer_process_packet with a NULL for the
     * next hop address, which will only match the dialer string
     *
     * We also need to call dialer_process_packet in the case of "passive"
     * interfaces, which should pass all traffic including broadcasts.
     *
     * As an optimization, we don't do this call if dialer_string is
     * not initialized...
     */
    if ((ddb->dialer_string != NULL) || 
	 (DTR_DIALER(ddb)) ||
	  (!ddb->dialer_list)) {
        pak->if_output = tmpidb;
        if (dialer_packet_debug) {
	    buginf("\n%s: sending broadcast to default destination", 
	            tmphwidb->hw_namestring);
	}
	dpak->proto_addr = NULL;
	dialer_process_broadcast(ddb, dpak, &first_idb);
    }

    /*
     * If we found a place to send this broadcast, put it in the pak so the
     * caller can send it out.  Otherwise, just restore it to what it was
     * when we were called (for safety).
     */
    if (first_idb != NULL) {
        pak->if_output = first_idb;
	return(get_dialerdb(first_idb->hwptr));
    } else {
	pak->if_output = tmpidb;
	return(NULL);
    }
}

/*
 * dialer_disconnect
 * Depending on the type of dialer that is in use, hang up the call.
 *
 * If we are in the middle of an EOIR event, do not hang up the call.
 * This is necessary because part of the hotswap process involves
 * calling hwidb->reset for all hwidbs, and on ISDN interfaces, this
 * calls dialer_disconnect()!
 */

void dialer_disconnect (hwidbtype *hwidb)
{
    dialerdbtype	*ddb;
    boolean		eoir;

    ddb = get_dialerdb(hwidb);
    if (ddb == NULL)
	return;

/* This may be needed for dialer profiles. 3/29/96
    if (ddb->dialer_binding)
	return;
*/

    eoir = (hwidb->oir_flags & EOIR_RESET);
    if (eoir) {

      /*
       * Do not disconnect the call during EOIR
       */
      if (dialer_debug)
	buginf("\n%s: EOIR - no dialer disconnect", hwidb->hw_namestring);
      return;

    } else {

      /*
       * If this interface is part of a connected group, unlink it from there.
       */
      if (CONNECTED_GROUP(ddb)) {
	dialer_unlink_member(ddb);
      }

      /*
       * If this interface was bound to a profile, unbinds it.
       */
      if (DIALPOOL_MEMBER(ddb) && ddb->dialer_bound) {
	dialer_unbind_profile(ddb);
      }

      if (dialer_debug) {
        buginf("\n%s: disconnecting call", hwidb->hw_namestring);
      }

      if (dialer_detailed_debug) {
	bugtrace();
      }

      TIMER_STOP(ddb->dialer_idle_timer);

      reg_invoke_dialer_drop(ddb->dialer, hwidb);
      if (DIALER_INPROGRESS(ddb))
	set_dialer_state(ddb, DIALER_DISCONNECTING);
    }
}

/*
 * dialer_set_subif_state
 */
static void dialer_set_subif_state (hwidbtype *idb)
{
    idbtype *swidb;

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	swidb->subif_state = SUBIF_STATE_UP;
	reg_invoke_route_adjust_msg(swidb);
	reg_invoke_route_adjust(swidb);
    }
}

/*
 * dialer_keep_dtr_low
 * Used by the reset/init vectors of serial interfaces to test whether DTR
 * should be kept low, i.e return early from the routine.  This is because
 * the same routine may be called to either dial a DTR dialer (by raising
 * DTR), or because of some other reasons for which we want to take or keep
 * DTR low.
 */
boolean dialer_keep_dtr_low (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);

    if (DTR_DIALER(ddb)) {
	/*
	 * We are "dialing" a DTR dialer when the dialer state is 'pending',
	 * so return FALSE (don't keep DTR low) in this case.
	 */
	if (!DIALER_CALLING(ddb)) {
            if (!IS_SPOOFING_INT(ddb) &&
		 (idb->state != IDBS_ADMINDOWN) && 
		  (idb->state != IDBS_DOWN)) {
		net_cstate(idb, IDBS_DOWN);
	    }
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * dtr_dialer_return
 * Called from net_cstate to return early in some conditions.
 */
static boolean dtr_dialer_return (hwidbtype *idb, int newstate)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);

    if (ddb == NULL)
	return(FALSE);

    if (DTR_DIALER(ddb)) {
	/*
	 * If we are spoofing and the line went down the modem is still
	 * attempting to dial the remote side, ignore line state change
	 * (i.e. continue spoofing).
	 * If we are not spoofing and the line came up return as this is
	 * because the low end driver detected that DCD is up.
	 */
	if (IS_SPOOFING_INT(ddb)) {
	    if ((newstate == IDBS_DOWN) ||
		(newstate == IDBS_RESET) ||
		((newstate == IDBS_UP) && !DIALER_INPROGRESS(ddb))) {
		return(TRUE);
	    }
	} else if (newstate == IDBS_UP) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * is_spoofing_int
 */
static boolean is_spoofing_int (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
	 return(IS_SPOOFING_INT(ddb));
    }
    return(FALSE);
}

/*
 * is_spoofing_prot
 */
static boolean is_spoofing_prot (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
	 return(IS_SPOOFING_PROT(ddb));
    }
    return(FALSE);
}

/*
 * huntgroup_member
 */
boolean huntgroup_member (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
	return(HUNTGROUP_MEMBER(ddb));
    }
    return(FALSE);
}

/*
 * huntgroup_leader
 */
boolean huntgroup_leader (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
	return(HUNTGROUP_LEADER(ddb));
    }
    return(FALSE);
}

/*
 * dialer_is_in_use
 * Return TRUE if dialer is in use.
 */
static boolean dialer_is_in_use (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
        return(DIALER_INPROGRESS(ddb));
    }
    return(FALSE);
}

/*
 * for_all_huntgroup_members
 * Run through all rotary group members.
 */
static void for_all_huntgroup_members (hwidbtype *idb, void *rtn)
{
    dialerdbtype *ddb, *tmpddb;
    void (*func) (hwidbtype *);

    ddb = get_dialerdb(idb);
    if ((ddb == NULL)  || !HUNTGROUP_LEADER(ddb))
	return;

    func = rtn;
    FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tmpddb) {
	(*func)(tmpddb->ownidb);
    }
}

/*
 * get_dialer_string
 * Returns pointer to dialer string
 */
static char *get_dialer_string (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if ((ddb == NULL) || (ddb->dialer_string == NULL))
	return(NULL);

    return(ddb->dialer_string->entry->dialer_string);
}

/*
 * dialer_compute_load
 * Return the total load on the given connected group.
 */
static ulong dialer_compute_load (dialergrouptype *grptr, hwidbtype **lowidb)
{
    hwidbtype *idb, *lowrxidb;
    dialerdbtype *ddb;
    ulong load, sumload, lowload;
    ulong rxload, sumrxload, lowrxload;
    int i;
    enum DIALER_THRESHOLD_DIRECTION direction;

    sumload = 0;
    sumrxload = 0;
    lowload = MAXLONG;
    lowrxload = MAXLONG;
    *lowidb = NULL;
    lowrxidb = NULL;

    ddb = grptr->firstddb;
    direction = ddb->thresh_direction;

    /*
     * Go through all connected members, add up load and determine least
     * loaded interface for possible disconnect.
     */
    if ((direction == DIALER_THRESH_EITHER) || 
	(direction == DIALER_THRESH_OUTBOUND)) {
	FOR_ALL_MEMBERS_IN_GROUP(grptr, i, ddb) {
	    idb = ddb->ownidb;
	    load = idb->load;
	    sumload += load;
	    if (load < lowload){
		lowload = load;
		*lowidb = idb;
	    }
	}
    }

    if ((direction == DIALER_THRESH_EITHER) || 
	(direction == DIALER_THRESH_INBOUND)) {
	FOR_ALL_MEMBERS_IN_GROUP(grptr, i, ddb) {
	    idb = ddb->ownidb;
	    rxload = idb->rxload;
	    sumrxload += rxload;
	    if (rxload < lowrxload){
		lowrxload = rxload;
		lowrxidb = idb;
	    }
	}
    }
    
    switch(direction) {
      case DIALER_THRESH_EITHER:
	if (sumrxload > sumload) {
	    sumload = sumrxload;
	    *lowidb = lowrxidb;
	}
	break;
      case DIALER_THRESH_INBOUND:
	sumload = sumrxload;
	*lowidb = lowrxidb;
	break;
      case DIALER_THRESH_OUTBOUND:
      default:
	break;
    }
    return(sumload);
}

/*
 * dialer_load_analyze
 * Determine if a connected group is over- or under- loaded, and take
 * appropriate actions, placing or disconnecting call(s).
 */
static void dialer_load_analyze (dialerdbtype *ddb, dialergrouptype *grptr)
{
    hwidbtype *discidb;
    dialerdbtype *dialddb;
    dialermaptype *map;
    dialerpaktype dpak;
    hwidbtype *idb;
    ulong avgload, sumload, nmember;

    idb = ddb->ownidb;
    /*
     * Compute total load on all connected lines, and determine average.  
     */
    sumload = dialer_compute_load(grptr, &discidb);
    nmember = grptr->nmember;
    avgload = sumload / nmember;
    if (avgload >= ddb->dialer_load_threshold) {
	/*
	 * The load threshold is exceeded.  If the load activity timer
	 * was running, it means that the load has gone back up after 
	 * a dip below the threshold, so stop it.
	 */
	TIMER_STOP(grptr->activity_timer);

	/*
	 * If we are still within the line down debounce period, this means
	 * that we hung up a line very recently.  Just return to keep 
	 * oscillations (dial/hang up) frequency at a minimum.
	 */
	if (TIMER_RUNNING(grptr->debounce_timer)) {
	    if (SLEEPING(grptr->debounce_timer))
		return;
	    else
		TIMER_STOP(grptr->debounce_timer);
	}

	/*
	 * Look up a map that matches this group's number, get a dialer,
	 * and place a call.
	 */
	map = find_dialer_addr(ddb, grptr->number_called);
	if (map != NULL) {
	    dialddb = reg_invoke_get_free_dialer(ddb->dialer, ddb);
	    if (dialddb) {
		if (dialer_debug) {
		    buginf("\n%s: rotary group to %s overloaded (%d)",
			    idb->hw_namestring, grptr->number_called,
			    avgload);
		}
		bzero(&dpak, sizeof(dialerpaktype));

		dpak.ddb = ddb;
		dpak.map = map;
		dpak.header = NULL;
		setstring(&dpak.header, "Dialing on overload");
		dial_if(dialddb, &dpak);
		TIMER_START(grptr->call_timer, ddb->dialer_carrier_ticks);
	    }
	}
    } else {
	if (TIMER_RUNNING(grptr->activity_timer)) {
	    /*
	     * The load activity timer is running because the load went
	     * under the threshold some time ago.  If it is expired,
	     * determine if a line should be disconnected, and
	     * disconnect it.
	     */
	    if (AWAKE(grptr->activity_timer)) {
		/*
		 * If there is n (n > 1) lines in the connected group, and
		 * the total load over (n - 1) lines would be less than
		 * the threshold, disconnect the least loaded line and
		 * start the debounce timer to prevent immediate redial
		 * of the same number, but don't stop the load activity 
		 * timer so that another link may be disconnected the 
		 * next time we analyze the load.
		 */
		if ((nmember > 1) &&
		    ((sumload / (nmember - 1)) < ddb->dialer_load_threshold)) {
		    if (dialer_debug)
			buginf("\n%s: load activity timeout for %s (%d)", 
				idb->hw_namestring, grptr->number_called,
				avgload);
		    dialer_disconnect(discidb);
		    TIMER_START(grptr->debounce_timer, DIALER_DEBOUNCE_TIME);
		} else {
		    TIMER_STOP(grptr->activity_timer);
		}
	    }
	} else {
	    /*
	     * The load activity timer is not running, this must be the 
	     * first time the load goes below the threshold, start
	     * the load activity timer here if there is more than one 
	     * member in the group.  If not don't bother, only the
	     * idle or fast idle timers can disconnect a single line. 
	     */
	    if (nmember > 1) {
		TIMER_START(grptr->activity_timer, ddb->dialer_idle_ticks);
		if (dialer_debug)
		    buginf("\n%s: rotary group to %s underloaded (%d), "
			   "starting load activity timer", idb->hw_namestring, 
			    grptr->number_called, avgload);
	    }
	}
    }
}



/*
 * dialer_timers
 * This routine is called from net_periodic if the interface is a dialer.
 * A dialer interface has several timers associated with it.  If the idle
 * timer expires, the interface is taken down.  If the re-enable timer
 * expires, then spoofing is re-enabled for the interface.  If the wait
 * for carrier timer expires and carrier is not up, then the interface is
 * taken down and the enable timer is started.  
 */
static void dialer_timers (hwidbtype *idb)
{
    dialerdbtype *ddb;
    dialergrouptype *grptr;
    boolean mlp_is_master_link;

    mlp_is_master_link = FALSE;

    ddb = get_dialerdb(idb);
    if (ddb == NULL)
	return;

    if (is_multilink(idb) && (idb->firstsw->is_bundle_interface))
	mlp_is_master_link = TRUE;

    if (TIMER_RUNNING_AND_AWAKE(ddb->dialer_idle_timer) || 
	TIMER_RUNNING_AND_AWAKE(ddb->dialer_fast_idle_timer)) {
        if (dialer_debug)
	    buginf("\n%s: idle timeout", idb->hw_namestring);

        TIMER_STOP(ddb->dialer_idle_timer);
        TIMER_STOP(ddb->dialer_fast_idle_timer);

	/*
	 * It is possible we started the idle timer, but carrier never
	 * came up.  So, check to make sure that the interface really
	 * is up, and we aren't spoofing before disconnecting the modem.
	 */
	
	if (DIALER_CALLUP(ddb)) {
	    ddb->disconnect_reason = DISCONNECT_IDLE;
	    dialer_disconnect(idb);
	    /*
	     *  If idb is the master link of a multilink bundle, do not
	     *    process the rest of dialer_timers because ddb is no
	     *    longer valid.  dialer_disconnect calls mlp_bundle_reset
	     *    which removes dialerdb from the virtual bundle interface.
	     */
	    if (mlp_is_master_link)
		return;
	}
	ddb->dialer_placed_call = FALSE;
    }
    
    if (TIMER_RUNNING_AND_AWAKE(ddb->dialer_enable_timer)) { 
	TIMER_STOP(ddb->dialer_enable_timer);
	if (dialer_debug)
	    buginf("\n%s: re-enable timeout", idb->hw_namestring);
	if ((idb->state == IDBS_DOWN) && !SLAVE_DIALER(ddb)) {
	    ddb->dialer_spoofing_int = TRUE;
	    ddb->dialer_spoofing_prot = TRUE;
	    dialer_set_subif_state(idb);
	    ddb->remote_name[0] = '\0';
	    idb->state = IDBS_UP;
        }
	set_dialer_state(ddb, DIALER_READY);
        /*
         * If this number is in a call group, the enable timer was set
	 * below when the carrier timer expired: dial the next number
	 * in the group.
         */
	if (ddb->dialer_call_group) {
	    dialerpaktype dpak;
	    bzero(&dpak, sizeof(dialerpaktype));
	    dpak.ddb = ddb->dialer_call_group->ddb;
	    dpak.map = ddb->dialer_call_group;
	    dpak.header = NULL;
	    /*
	     * Transfer the holdQ from the interface to the dpak.
	     * It will get transferred back, most likely
	     */
	    dpak.holdQ = ddb->dialer_holdQ;
	    ddb->dialer_holdQ = NULL;
	    
	    dial_if(ddb, &dpak);
	} else {
	    callback_type *call;
 	    /*
 	     *  Check if we need to dial a callback return call.
 	     *  For dialer in-band async or Sync interface that are
 	     *  not part of a dialer rotary-group dial the callback
 	     *  return call when the enable timer expires.
 	     */
	    call = find_callback_by_idb(idb);
	    if (call) {
		if (call->callback_type == CALLBACK_SERVER) {
		    if (dialer_debug)
			buginf("\ncallback triggered by dialer_timers");
		    callback_dial(call);
		}	
 	    } else {
		/* no more calls pending - clear out the ddb */
		dialer_cannot_dial(ddb, NULL);
 	    }
	}
    }

    if (TIMER_RUNNING_AND_AWAKE(ddb->dialer_carrier_timer)) { 
        TIMER_STOP(ddb->dialer_carrier_timer);
        if (dialer_debug)
	    buginf("\n%s: wait for carrier timeout", idb->hw_namestring);
	if (DIALER_CALLING(ddb)) {
	    dialer_disconnect(idb);
	    dialer_decrement_call(ddb);
	    dialer_fail(ddb);
            /*
             * If there is another number in the call group, set
	     * enable timer to (pulsetimer + 1) so that when 
	     * dialer_disconnect is done resetting the interface,
	     * we can try the next number.
             */
            if (ddb->dialer_call_group &&
		ddb->dialer_call_group->cip &&
		ddb->dialer_call_group->cip->next) {
		TIMER_START(ddb->dialer_enable_timer,
			    idb->serial_pulseinterval + ONESEC);
	    } else {
	        dialer_cannot_dial(ddb, NULL);
	    }
	}
    }

    if (ddb->dialer_holdQ) {
	if (dialer_holdq_timer_running_and_awake(ddb->dialer_holdQ)) {
	    stop_dialer_holdq_timer(ddb->dialer_holdQ);
	    if (dialer_debug)
		buginf("\n%s: Holdq timer error", idb->hw_namestring);
	    dialer_holdq_discard_queue(&ddb->dialer_holdQ,
				       "Hold queue purge timeout");
	}
    }

    if (TIMER_RUNNING_AND_AWAKE(ddb->dialer_load_timer)) {
	if (CONNECTED_GROUP(ddb)) {
	    TIMER_START(ddb->dialer_load_timer, DIALER_LOAD_TIME);
	} else {
	    TIMER_STOP(ddb->dialer_load_timer);
	}

	FOR_ALL_CONNECTEDGROUPS_ON_MASTER(ddb, grptr) {
	    /*
	     * For all connected groups, determine if the load is such that
	     * a line should be dialed or disconnected.  Skip process if a
	     * call is pending.
	     */
	    if (TIMER_RUNNING(grptr->call_timer)) {
		if (SLEEPING(grptr->call_timer))
		    continue;
		else
		    TIMER_STOP(grptr->call_timer);
	    }
	    /*
	     *  if interface is part of a multilink bundle, multilink
	     *    will do load analysis and balancing
	     */
	    if (!is_multilink(grptr->firstddb->ownidb))
		dialer_load_analyze(ddb, grptr);
	}
    }
}

/*
 * dialer_change_pak_if
 * Change pak->if_input from physical interface to rotary group leader.
 * Returns FALSE if not a DDR interface, not a rotary group leader, or 
 * change is succesful.  This is counter intuitive, but is consistent
 * in the absence of registry.
 */
static boolean dialer_change_pak_if (hwidbtype *idb, paktype *pak)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);

    if ((ddb == NULL) || !SLAVE_DIALER(ddb))
	return(FALSE);

    ddb = GET_NETDDB(ddb);
    if (change_if_input(pak, ddb->ownidb->firstsw)) {
	return(FALSE);
    }
    return(TRUE);
}

/*
 * dialer_raw_enqueue
 * Called from interrupt level to enqueue an input packet
 * If the protocol specific routine doesn't absorb the packet, possibly because
 * the protocol is not enabled on that interface, the packet must be enqueued
 * on netinputQ for later disposal.  Interrupt level disposal of a steady
 * stream of packets might other lock up the system.  Unbridgable packets are
 * marked with PAK_ABORT.
 *
 * ISDN requires its own enqueue routine because it fiddles with the interface
 * the packet is from.  If the packet arrives on a B channel, and it isn't
 * a low level packet for HDLC or PPP, then pak->if_input needs to be changed
 * to make it appear as if the packet came from the D channel idb instead of
 * the B.  This is due to the way grouping works.
 */

void dialer_raw_enqueue (hwidbtype *idb, paktype *pak)
{
    dialer_hdr *dialer;

    if (!pak->if_input) {
	errmsg(&msgsym(NOSOURCE, LINK));
	retbuffer(pak);
	return;
    }
    SET_PAK_DIALER_INPUT(pak);
    GET_TIMESTAMP(idb->lastinput);	/* remember time of last input */
    pak->inputtime = idb->lastinput;	/* save it in packet too */
    if (idb->state != IDBS_UP) {	/* if hardware not up */
	retbuffer(pak);			/* drop the packet now */
	return;				/* and return */
    }
    pak->linktype = (*idb->getlink)(pak);

    /*
     * Need to uncompress any header compression prior to changing the
     * idbs around, since the header compression db is attached to the
     * actual incoming/outgoing interface.
     * We are fortunate here.  Only PPP and HDLC packets will get here.
     * Frame relay and X.25 header compressed packets will already have
     * been handled by the appropriate input routine.
     */
    if (pak->linktype == LINK_COMPRESSED_TCP ||
	pak->linktype == LINK_UNCOMPRESSED_TCP) {
        pak = reg_invoke_uncompress_tcpip_header(LINK_IP, pak, idb, 
		idb->tcp_compress);
	if (pak == NULL) {
	    return;
	}
    }

    /*
     *  Data packets (IP, IPX, ...) receceived on dialer rotary-group
     *   members (inluding ISDN B-channels) need to appear of to
     *   be received of the dialer rotary-group leader ( Including
     *   (ISDN D-channels).  So, change the if_input for those
     *   types of packets.
     *
     *  LINK_MULTILINK and LINK_COMPRESSION
     *  Since multilink and compression do not have the data
     *    in the final form, keep the if_input the same until
     *    the data has been reassembled or decompressed
     *
     *  LINK_PPP
     *  ppp negoations take place over the rotary-group group member
     *   so don't change the if_input.
     *
     *  LINK_ADDRESS
     *  serial keepalives originate and terminate on the rotary-group member
     *    so don't change if_input.
     *    
     */
    if ((pak->linktype != LINK_MULTILINK) &&
	(pak->linktype != LINK_PPP_COMPRESSION) &&
	(pak->linktype != LINK_PPP) &&
	(pak->linktype != LINK_ADDRESS)) {
	if (dialer_change_pak_if(idb, pak)) {
	    retbuffer(pak);
	    return;
	}
    }

    dialer = (dialer_hdr *)pak->datagramstart;

    /*
     * If this is a packet from a V.25bis modem,
     * display its contents and dispose of it.
     */
    if ((pak->linktype == LINK_ILLEGAL) &&
	(GETSHORT(&dialer->flags)  == V25BIS_FLAGS)) {
	if (dialer_debug) {
	    char *tmp = string_getnext();
	    if ((pak->datagramsize - 1) <= sizeof tmp) {
		sstrncpy(tmp, pak->datagramstart, pak->datagramsize-1);
	    } else {
		sstrncpy(tmp, pak->datagramstart, sizeof(tmp));
	    }
	    buginf("\n%s: Dialer result = %s", 
		   pak->if_input->namestring, tmp);
	}
	datagram_done(pak);
    } else {
	/*
	 * Snap the frame to alignment if required
	 */
	ALIGN_DATAGRAM_CMD(pak_realign(pak, NULL));
	reg_invoke_raw_enqueue(pak->linktype, pak);
    }
}

/*
 * dialer_remote_name
 * If we are a dial on demand interface, save the name of the remote
 * that just authenticated.  This way when an outbound packet has a 
 * destination for the remote router, we know what interface we should
 * use.
 */
void dialer_remote_name (hwidbtype *idb, char *name)
{
    dialerdbtype *ddb;
    dialerentrytype *entry;

    ddb = get_dialerdb(idb);
    if (ddb == NULL)
        return;

    /*
     * If we placed the call, don't overwrite the number called.  This
     * allows us to authenticate with dialer string, plus speeds up
     * comparisons.  Just to be nice, save the same elsewhere so we
     * can display it later.
     */
    strcpy(ddb->remote_name, name);
    /*
     * If this is an unbound pool member, attempt to bind it to a profile
     * now.  If we can't find one, disconnect the call.
     */
    if (DIALPOOL_MEMBER(ddb)) {
	if (!ddb->dialer_bound) {
	    if (!dialer_match_name(ddb, name)) {
        	if (dialer_debug)
		    buginf("\n%s: authenticated host %s with no matching",
			   " dialer profile", idb->hw_namestring, name);
		dialer_disconnect(idb);
	    }
	}
	return;
    }
    entry = dialer_name_lookup(ddb, name);

    /*
     * if we have a dialer map with a "name" parameter of what we just
     * authenticated, then if that map has a phone number, save it as
     * the "number called" so that later checks are quicker, and so
     * that the show displays look correct.  If there is no matching
     * dialer map statement, then we trust that either caller id
     * set the number called to the correct value, or that the "dialer string"
     * called in. Otherwise, the phone number displayed and the hostname
     * displayed will not match.
     */

    if (entry) {
        if (entry->dialer_string) {
	    dialer_set_number_called(ddb, entry->dialer_string);
	} else {
	    /*
	     *  test idb->number_called to prevent ovwerwriting the
	     *    caller id
	     */
	    if (ddb->number_called[0] == '\0')
	        strcpy(ddb->number_called, "<unknown phone number>");
	}
    } else {
        if (dialer_debug)
	    buginf("\n%s: authenticated host %s with no matching dialer map",
	        idb->hw_namestring, name);
    }
}


/*
 * dialer_remote_addr
 * Get the first address in the map matching idb, linktype and dial string.
 */
addrtype *dialer_remote_addr (hwidbtype *idb, uint link, char *number)
{
    dialerdbtype *ddb;
    dialermaptype *map;
    int i;

    ddb = get_dialerdb(idb);
    if (ddb == NULL)
	return(NULL);

    idb = GET_NETIDB(ddb);		/* Look up address in the master idb */
    ddb = GET_NETDDB(ddb);		/* Look up address in the master idb */

    for (i = 0; i < DIALERHASH; i++) {
	map = dialertable[i];
	while (map != NULL) {
	    /*
	     * If idb and linktype match, try to match dial string, unless
	     * this is a DTR dialer, in which case the first map will do.
	     */
	    if ((ddb == map->ddb) && (link == map->link)) {
		if ((DTR_DIALER(ddb)) ||
			dialer_string_lookup(map, number) != NULL) {
		    return(&map->proto_addr);
		}
	    }
	    map = map->next;
	}
    }

    return(NULL);
}

/*
 * dialer_lookup_caller()
 *	Try to figure out a phone number based on IP address attached
 */
static void
dialer_lookup_caller (hwidbtype *idb, addrtype *proto_addr, int link)
{
    dialerdbtype *ddb;
    dialermaptype *map;

    /*
     * Don't touch the IDB unless it is currently a dialer
     */
    ddb = get_dialerdb(idb);
    if (ddb == NULL)
	return;

    /*
     * Try to figure out a phone number to use as the number_called.
     * We need this so that we can properly handle return traffic
     * on this interface.
     */
    if (ddb->number_called[0] == '\0') {
	
	/*
	 * Look up based on address/link type for the IDB
	 */
	map = dialer_addr_lookup(ddb, proto_addr, link);

	/*
	 * Get the first number
	 */
	if (map) {
	    dialer_set_number_called(ddb, (map->entry)->dialer_string);
	}

	/*
	 * If we still don't have any idea, set the default number
	 */
	if ((ddb->number_called[0] == '\0') && ddb->dialer_string) {
	    dialer_set_number_called(ddb,
				     ddb->dialer_string->entry->dialer_string);
	}

	/*
	 * Log the call
	 */
	if (dialer_debug) {
	  buginf("\n%s: Dialer received incoming call from %s",
		 idb->hw_namestring, 
		 ddb->number_called[0] ? ddb->number_called : "<unknown>");
	}
    }
}

/*
 * dialer_out_call_connected
 * An outgoing call just connected.
 */
void dialer_out_call_connected (dialerdbtype *ddb)
{
    dialer_stat *ds;

    set_dialer_state(ddb, DIALER_CONNECTED);
    ddb->callback_type = CALLBACK_NONE;
    ddb->dialer_placed_call = TRUE;
    ds = ddb->dialer_stats;
    if (ds) {
	ds->num_successes++;
	ds->last_call_success = TRUE;
    }
    if (ddb->dialer_call_group)  {
	(ddb->dialer_call_group)->cip = NULL;
	ddb->dialer_call_group = NULL;
    }
    if (ddb->dialer_holdQ != NULL) {
	dialer_holdq_timer_start(ddb->dialer_holdQ, ddb->dialer_holdq_ticks);
    }

     /*
      * Connect this dialer pool member to its profile.
      */
     if (DIALPOOL_MEMBER(ddb)) {
       dialer_bind_profile(ddb, ddb->dialing_ddb);
     }

    TIMER_START(ddb->dialer_idle_timer, ddb->dialer_idle_ticks);
    GET_TIMESTAMP(ddb->mru_callsuccess);	/* for get_free_dialer */
}

/*
 * dialer_in_call_connected
 * An incoming call just connected.
 */
void dialer_in_call_connected (dialerdbtype *ddb)
{  
dialerdbtype *d_ddb;

    set_dialer_state(ddb, DIALER_CONNECTED);
    ddb->callback_type = CALLBACK_NONE;
    ddb->dialer_placed_call = FALSE;
    /*
     * ISDN keeps this number on the D channel idb
     */
    if (ddb->dialer == DIALER_ISDN) {
       d_ddb = get_dialerdb(ddb->ownidb->isdn_info->d_chan_idb);
       dialer_increment_call(d_ddb);
    } else {
       if (ddb->ownidb->type != IDBTYPE_VACCESS)
          dialer_increment_call(ddb);		 /* one more line in use */
    }
    TIMER_START(ddb->dialer_idle_timer, ddb->dialer_idle_ticks);
}

/*
 * dialer_call_disconnected
 * Clean up after a call just disconnected.
 */
void dialer_call_disconnected (dialerdbtype *ddb, enum DIALER_STATE state)
{
dialerdbtype *d_ddb;

    if (ddb->callback_type == CALLBACK_CLIENT) {
	callback_type *call;
	/*
	 * we are a callback client - create callback structure
	 *
	 * start a callback timer, so that if we time out, we can
	 * restart another call.
	 */
	call = create_callback(ddb->dialer_holdQ,
			       CALLBACK_CLIENT,
			       ddb->remote_name,
			       ddb->ownidb,
			       ddb->number_called,
			       NULL);
	if (call) {
	    start_callback_timer(call, 
				 ddb->dialer_enable_ticks +
				 ddb->dialer_carrier_ticks);
	    ddb->dialer_holdQ = NULL;
	    ddb->callback_type = CALLBACK_NONE;
	    if (dialer_debug) {
		buginf("\nCallback client for %s %s created",
		       ddb->remote_name, ddb->number_called);
	    }
	}
    } 
    if (ddb->dialer_holdQ) {
	dialer_holdq_discard_queue(&ddb->dialer_holdQ, "Call disconnected");
    }
    /*
     * If this interface is part of a connected group, unlink it from there.
     */
    if (CONNECTED_GROUP(ddb)) {
	dialer_unlink_member(ddb);
    }

    /*
     * If this interface was bound to a profile, unbinds it.
     */
    if (DIALPOOL_MEMBER(ddb) && ddb->dialer_bound) {
      dialer_unbind_profile(ddb);
    }

    if ((ddb->ownidb->state != ddb->ownidb->oldstate) &&
        (ddb->ownidb->oldstate == IDBS_UP)) {
       /*
        * ISDN keeps this number on the D channel idb
        */
       if (ddb->dialer == DIALER_ISDN) {
          d_ddb = get_dialerdb(ddb->ownidb->isdn_info->d_chan_idb);
          dialer_decrement_call(d_ddb);
       } else {
          if (ddb->ownidb->type != IDBTYPE_VACCESS)
             dialer_decrement_call(ddb);   /* one more line available */
       }
    }
    ddb->dialer_spoofing_int = FALSE;
    ddb->dialer_spoofing_prot = FALSE;
    ddb->dialer_placed_call = FALSE;
    ddb->dialer_stats = NULL;
    TIMER_STOP(ddb->dialer_idle_timer);
    TIMER_STOP(ddb->dialer_fast_idle_timer);
    if (!HUNTGROUP_LEADER(ddb))
	set_dialer_state(ddb, state);
    dialer_set_number_called(ddb, NULL);
    ddb->remote_name[0] = '\0';
    ddb->dial_reason[0] = '\0';
}

/*
 * dialer_statechange - A dialer interface has changed state
 *
 * We come here when a dialer interface has an actual state transition as
 * determined by cstate_check().
 *
 * If the carrier came up on a dialer, then turn spoofing off.  Also
 * start the idle timer.  If carrier dropped on a dialer, then set the
 * timer to start spoofing again.
 */

static void dialer_statechange (hwidbtype *idb)
{
    dialerdbtype *ddb, *netddb;
    enum DIALER_STATE state;

    ddb = get_dialerdb(idb);

    if ((ddb == NULL) || 
	(ddb->dialer == DIALER_NONE) ||
	(idb->type == IDBTYPE_DIALER) ||
	ISDN_DIALER(ddb))
	return;

    if (idb->state == IDBS_UP) {
	ddb->dialer_spoofing_int = FALSE;
	TIMER_STOP(ddb->dialer_carrier_timer);
	/*
	 *  Call may have been answered while the enable timer was running
	 *    The enable timer will set the interface to DIALER_READY.
	 *    If the interface is up then there is no need for enable
	 *    clean up and do not want to make an UP interface ready 
	 *    for dialing.
	 */
	TIMER_STOP(ddb->dialer_enable_timer);
	ddb->disconnect_reason = NO_DISCONNECT;
	if (DIALER_CALLING(ddb)) {
	    /*
	     * We were placing a call, and carrier came up. We will assume
	     * that this is the call that we placed, although it may
	     * be possible that this is a call collision.
	     */
	    dialer_out_call_connected(ddb);
	} else {
	    addrtype proto_addr;
	    ipaddrtype ipaddr;

	    /*
	     * Incoming call, handle it
	     */


	    dialer_in_call_connected(ddb);
	    netddb = GET_NETDDB(ddb);

	    if (DIALPOOL_MEMBER(ddb)) {
		/*
	 	 * If a dialer pool member, attempt to match up and bind a 
		 * profile based on the pool.  If we have no hope of doing 
		 * it, disconnect the call.
		 */
		if (!(dialer_match_pool(ddb) || dialer_may_match_name(idb))) {
		    if (dialer_debug) {
			buginf("\n%s: Can not bind incoming call", 
				idb->hw_namestring);
		    }
		    dialer_disconnect(idb);
		    return;
		}
	    } else if (!IS_CALL_ALLOWED(netddb)) {
		/*
		 * Here we verify that this incoming call does not exceed our
		 * limit on the rotary group.  First increment the number of
		 * calls, then check new value against maximum.  In case of
		 * excess disconnect this call.
		 */
		if (dialer_debug) {
		    buginf("\n%s: Already %d call(s) in progress on %s",
			   idb->hw_namestring, netddb->dialer_num_call,
			   netddb->ownidb->hw_namestring);
		}
		dialer_disconnect(idb);
		return;
	    }

	    /*
	     * If we are an async dialer and we have a neighbor IP address, 
	     * see if we have an entry in the dialer map for this destination.
	     * Sync dialers always get the IP address 0 here and end up with
	     * the dialer string, if configured.
	     */
	    ipaddr = reg_invoke_ip_peer_addr(idb->firstsw);
	    if (is_async(idb) && ipaddr) {
		memset(&proto_addr, 0, sizeof(addrtype));
		proto_addr.ip_addr = ipaddr;
		proto_addr.type = ADDR_IP;
		proto_addr.length = ADDRLEN_IP;
		dialer_lookup_caller(idb, &proto_addr, LINK_IP);
	    } else {
		dialer_lookup_caller(idb, NULL, LINK_IP);
	    }
	}
	reg_invoke_dialer_active(idb, ddb->netcfg_idb, TRUE);
    } else {
	state = (idb->state == IDBS_ADMINDOWN) ? 
					DIALER_SHUTDOWN : DIALER_WAITING;
	dialer_call_disconnected(ddb, state);

	if (!HUNTGROUP_LEADER(ddb) && !DIALER_SHUT(ddb)) {
	    TIMER_START(ddb->dialer_enable_timer, ddb->dialer_enable_ticks);
	} 

        /*
         * DTR dialers drop DTR when the line goes down
         */
        if (DTR_DIALER(ddb) && (idb->state == IDBS_DOWN))
            dialer_dtr_drop(idb);

	reg_invoke_dialer_active(idb, ddb->netcfg_idb, FALSE);
    }
}

/*
 * dialer_vencapsulate - Encapsulate a packet using a dialer
 *
 * When an interface has a dialer set, we set ourselves to be the
 * encapsulation routine, instead of the real handler. We first attempt
 * to find a destination for the packet, and then we encapsulate it using
 * the real routine.
 */

boolean dialer_vencapsulate (paktype *pak, long address)
{
    dialerdbtype *ddb;
    addrtype proto_addr;
    dialerpaktype dpak;
    boolean broadcast;
    char *reason;

    bzero(&dpak, sizeof(dialerpaktype));

    /*
     * We should not get here if DDR is not configured on this interface,
     * but double check and return if this is the case.
     */
    ddb = get_dialerdb(pak->if_output->hwptr);
    if (ddb == NULL) {
	return(FALSE);
    }

    /* Novell and possibly other protocols do not pass a
     * complete "next hop" address in the "address" parameter to the
     * encapsulation routine.  In novell's case, they pass the host, but
     * not the network number.  Without a complete address, lookups
     * for dialer maps will fail.  So fix up the address here if necessary.
     *
     * Do address resolution.  
     * The destination address is a address structure (in proto_addr) 
     * which DDR will use to look up a static address map.
     */

    memset(&proto_addr, 0, sizeof(addrtype));
    if (!reg_invoke_media_ddr_macaddr(pak->linktype, pak, address, &proto_addr,
	&broadcast)) {
        if (!reg_used_media_ddr_macaddr(pak->linktype))
            errmsg(&msgsym(BADMACREG, LINK), pak->if_output->namestring,
                    pak->linktype);
        return(FALSE);
    }
    pak->encsize = 0;                   /* no encapsulation yet */
    pak->dialer_output = pak->if_output;  /* keep track of this idb */

    /*
     * Populate dpak structure and classify packet.
     */

     dialer_reason[0] = '\0';
     reason = dialer_packet_debug ? &dialer_reason[0] : NULL;
     dpak.ddb = ddb;
    dpak.pak = pak;
    dpak.linktype = pak->dialer_save_type;
    dpak.address = address;
    dpak.proto_addr = address ? &proto_addr : NULL;
    dpak.map = NULL;
    dpak.interesting = dialer_classify(ddb, pak, reason);
    dpak.header = NULL;

    /*
     * Per packet debug display.
     */
    if (dialer_packet_debug) {
        dialer_header[0] = '\0';
        dialer_get_packet_header(pak, &dialer_header[0]);
	buginf("\n%s: %s, %d bytes, %sinteresting (%s)", 
                ddb->ownidb->hw_namestring, dialer_header, pak->datagramsize,
		dpak.interesting ? "" : "un", reason);
	if (dialer_detailed_debug) {
	    dialer_packet_hexdump(pak);
	}
    }

    /*
     * Get output ddb.
     */

    if (IS_PROFILE(ddb))
	ddb = (*ddb->dialer_fsm)(ddb, (void *)&dpak);	/* dialer profile */
    else if (broadcast)
	ddb = dialer_broadcast(ddb, &dpak);		/* legacy DDR */
    else
	ddb = dialer_process_packet(ddb, &dpak);	/* legacy DDR */
    if (ddb == NULL)
	return(FALSE);

    /*
     * And perform encapsulation.
     */
    if (ddb->dialer_save_vencap)
        return(dialer_execute_save_vencap(ddb, pak, address));
    else
        return(FALSE);
}


/*
 * dialer_sync_drop
 * Called by SERVICE_MEDIA_DIALER_DROP, index is DIALER_INBAND_SYNC
 */

static void dialer_sync_drop (hwidbtype *hwidb)
{
    (*hwidb->reset)(hwidb);
}

/*
 * dialer_dtr_drop
 * Called by SERVICE_MEDIA_DIALER_DROP, index is DIALER_DTR_SYNC
 */

void dialer_dtr_drop (hwidbtype *hwidb)
{
    (*hwidb->reset)(hwidb);
}

/*
 * dialer_initiate_connection
 * Send a 'dialer kicker' packet out an interface - will kick dialer into
 * gear if need be....
*/
void dialer_initiate_connection (hwidbtype *hwidb, long addr, ulong link)
{
    paktype *pak;

    /* If not attached to a dialer, don't bother... */
    if (!is_ddr(hwidb)) {
        return;
    }

    /* Get a buffer... */
    if ((pak = getbuffer(2)) == NULL) {
	return;
    }

    /* Set up output interface */
    pak->if_output = firstsw_or_null(hwidb);

    if (pak->if_output == NULL) {
	errmsg(&msgsym(NOSWIDB, DIALER), hwidb->hw_namestring);
    } else {
	pak->linktype = link;
	pak->enctype = ET_HDLC; /* Why not? It's not going anywhere... */
	(void) pak_encap(hwidb, pak, 2, addr);
    }

    retbuffer(pak);
    return;
}

/*
 * dialer_reset_timers_inline
 * Implement below function.
 */
static inline void dialer_reset_timers_inline (dialerdbtype *ddb)
{
    dialerdbtype *tempddb;

    if (DIALER_UP(ddb)) {
	START_IDLE_TIMER(ddb);
    }

    if (!HUNTGROUP_LEADER(ddb))
        return;

    /* Recursive to allow rotaries of rotaries... */
    FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
	dialer_reset_timers_inline(tempddb);
    }
}
/*
 * dialer_reset_timers
 * Reset the idle timers of the given interface, and all interfaces
 * in the hunt group...Only do this on interfaces 'fully up'
 */
void dialer_reset_timers (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb == NULL)
	return;

    dialer_reset_timers_inline(ddb);
}

/*
 * dialer_return_connected_hwidbs_inline
 * Implements function below.
 */
static void dialer_return_connected_hwidbs_inline (
    hwidbtype *top, 
    dialerdbtype *ddb,
    dialermaptype *map)
{
    dialerdbtype *tempddb;
    boolean isup;

    if (HUNTGROUP_LEADER(ddb)) {
	/*
	 * This interface is the hunt group lead. Look through the
	 * interfaces in the hunt group. Recursive to cover rotaries
	 * of rotaries...
	 */
	FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
	    dialer_return_connected_hwidbs_inline(top, tempddb, map);
	}
    } else {
	isup = dialer_connected(ddb, map);
	if (isup) {
	    reg_invoke_dialer_number_called_changed(map->link, ddb->ownidb,
						   top, &map->proto_addr, TRUE);
	}
    }
}

/*
 * dialer_return_connected_hwidbs
 * Will call dialer_number_called_changed for all hwidbs associated
 * with the passed-in idb, for all of those hwidbs that are connected to same
 * dial-string that the dialer map entry of the address passed in has as its
 * dialer string. In other words "Which hw interfaces are attached to the phone
 * number associated with, say, 'dialer map snapshot 10 12345?' Original
 * caller should have top == output.
 *
 * You will notice that it looks a lot like dialer_connected_to.
 */
void dialer_return_connected_hwidbs (hwidbtype *output, hwidbtype *top,
				     addrtype *proto_addr, uint link)
{
    dialerdbtype *ddb;
    dialermaptype *map;

    ddb = get_dialerdb(output);
    map = dialer_addr_lookup(ddb, proto_addr, link);
    dialer_return_connected_hwidbs_inline(top, ddb, map);
}

static boolean dialer_serial_is_answer (dialerdbtype *ddb)
{
    return(!ddb->dialer_placed_call);
}

/*
 * invoked via the reg_invoke_media_ddr_macaddr call.  The default is
 * for packets to not be considered broadcast packets.
 * Cases this covers includes SLARP packets (yuck, shouldn't even
 * get here), and LINK_RSRB packets.
 */
static boolean dialer_bcast_default (paktype *pak, long address,
				 addrtype *proto_addr, boolean *broadcast)
{
    *broadcast = FALSE;
    return(TRUE);
}

/*
 * dialer_pool_acl_idb
 *
 * For the interface in question, determine if it is part of a dialer pool
 * and return the idb of where an access list should be applied.
 */
static hwidbtype *dialer_pool_acl_idb (hwidbtype *idb)
{
    dialerdbtype *ddb;
    hwidbtype *netidb = NULL;
    
    ddb = get_dialerdb(idb);
    if (ddb && DIALPOOL_MEMBER(ddb)) {
	netidb = GET_NETIDB(ddb);
    }
    return (netidb ? netidb : idb);
}


/*
 * dialer_get_conn_member - returns the hwidb of the connected channel,
 *	returns NULL if no channel is up.
 */
static hwidbtype *dialer_get_conn_member (hwidbtype *hwidb)
{
    dialerdbtype *ddb;
    dialergrouptype *grptr;

    if (!(ddb = get_dialerdb(hwidb)))	/*safety*/
	return(NULL);
    if (HUNTGROUP_LEADER(ddb)) {
	FOR_ALL_CONNECTEDGROUPS_ON_MASTER(ddb, grptr) {
	    if (grptr->firstddb && (hwidb = grptr->firstddb->ownidb) &&
		hwidb->state == IDBS_UP)
		return(hwidb);
	}
	return(NULL);
    } else {
	if (HUNTGROUP_MEMBER(ddb) && (hwidb = ddb->ownidb) &&
	    hwidb->state == IDBS_UP)
	    return(hwidb);
    }
    return(NULL);
}

/*
 * dialer_get_connected_group
 * Given a ddb, protocol address return the connected group if connected
 */
static dialergrouptype *dialer_get_connected_group (hwidbtype *hwidb,
						    addrtype *proto_addr,
						    uint linktype)
{
    dialerpaktype dpak;
    dialerdbtype *ddb, *outddb;

    ddb = get_dialerdb(hwidb);

    dpak.proto_addr = proto_addr;
    dpak.linktype = linktype;

    /*
     * Find a dialer map for this protocol address, if not 
     * get the default dialer string
     */

    dpak.map = dialer_addr_lookup(ddb, dpak.proto_addr, dpak.linktype);

    if (dpak.map == NULL)
	dpak.map = ddb->dialer_string;

    /*
     * Are we connected to this destination ?
     * If yes, return the connected group else NULL
     */
    outddb = connected_to(ddb, &dpak);

    if (outddb)
	return(outddb->conn_group);
    
    return NULL;
}

/*
 * dialer_init
 * Initialize and register dialer support
 */

static void dialer_init (subsystype *subsys)
{
    /*
     * Set up debugging flags
     */
    dialer_debug_init();
    callback_system_init();
    dialer_pool_head = NULL;

    /*
     * Register some functions
     */
    reg_add_hwif_goingdown(dialer_goingdown, "dialer_goingdown");
    reg_add_hwif_comingup(dialer_comingup, "dialer_comingup");
    reg_add_if_onesec(dialer_timers, "dialer_timers");
    reg_add_onesec(callback_onesec, "callback_onesec");
    reg_add_hwif_state_change(dialer_statechange, "dialer_statechange");
    reg_add_swif_erase(dialer_deleteidb, "dialer_deleteidb");
    reg_add_create_idb(dialer_parser_createidb, "dialer_parser_createidb");
    reg_add_interface_cfg_modified(
		      propagate_hunt_changes, "propagate_hunt_changes");
    reg_add_default_dialer_dial((service_dialer_dial_type)return_false,
				"return_false");
    reg_add_dialer_dial(DIALER_INBAND_SYNC, dial_v25bis, "dial_v25bis");
    reg_add_dialer_dial(DIALER_DTR_SYNC, dial_dtr, "dial_dtr");
    reg_add_dialer_drop(DIALER_INBAND_SYNC, dialer_sync_drop,
			      "dialer_sync_drop");
    reg_add_dialer_drop(DIALER_DTR_SYNC, dialer_dtr_drop, "dialer_dtr_drop");
    reg_add_dialer_dial_failed(dialer_failed, "dialer_failed");
    reg_add_media_serial_encap_changed(dialer_adjust_encap_vectors, 
    				      "dialer_adjust_encap_vectors");
    reg_add_pri_member_adjust_encap(dialer_adjust_encap_vectors, 
    				    "dialer_adjust_encap_vectors");
    reg_add_dialer_pri_shutdown(dialer_pri_shutdown, "dialer_pri_shutdown");
    reg_add_media_protocol_up(dialer_protocol_up, "dialer_protocol_up");
    reg_add_default_dialer_remote_ip_addr(
	(service_dialer_remote_ip_addr_type)return_zero, "return_zero");
    reg_add_dialer_lookup_caller(dialer_lookup_caller, "dialer_lookup_caller");
    reg_add_dialer_initiate_connection(dialer_initiate_connection,
                                       "dialer_initiate_connection");
    reg_add_dialer_connected(dialer_connected_to, "dialer_connected_to");
    reg_add_dialer_retrieve_maps(dialer_retrieve_maps, "dialer_retrieve_maps");
    reg_add_dialer_reset_timers(dialer_reset_timers, "dialer_reset_timers");
    reg_add_dialer_return_connected_hwidbs
	(dialer_return_connected_hwidbs, "dialer_return_connected_hwidbs");
    reg_add_dialer_remote_name(dialer_remote_name, "dialer_remote_name");
    reg_add_dialer_remote_addr(dialer_remote_addr, "dialer_remote_addr");
    reg_add_default_dialer_shutdown(dialer_shutdown, "dialer_shutdown");
    reg_add_default_dialer_clear_group(dialer_clear_group, 
				       "dialer_clear_group");
    reg_add_default_dialer_pending(dialer_pending, "dialer_pending");
    reg_add_default_get_free_dialer(get_free_dialer, "get_free_dialer");
    reg_add_get_free_dialer(DIALER_PROFILE, get_free_dialer_in_pool, 
			    "get_free_dialer_in_pool");
    reg_add_default_dialer_is_free(dialer_is_free, "dialer_is_free");
    reg_add_default_dialer_show_info(show_dialer_info, "dialer_show_info");
    reg_add_default_dialer_is_answer(
            (service_dialer_is_answer_type)return_false, "return_false");
    reg_add_dialer_is_answer(DIALER_INBAND_SYNC, dialer_serial_is_answer,
			     "dialer_serial_is_answer");
    reg_add_dialer_is_answer(DIALER_DTR_SYNC, dialer_serial_is_answer,
			     "dialer_serial_is_answer");
    reg_add_default_dialer_setup(default_set_dialer, "default_set_dialer");
    reg_add_default_media_ddr_macaddr(dialer_bcast_default, 
    				      "dialer_bcast_default");
    reg_add_dialer_callback_start(callback_start, "dialer_callback_start"); 
    reg_add_mapclass_nvgen(ET_DIALER, dialer_mapclass_nvgen,
			   "dialer_mapclass_nvgen");
    reg_add_dialer_keep_dtr_low(dialer_keep_dtr_low, "dialer_keep_dtr_low");
    reg_add_dialer_create_dynamic_map(dialer_create_dynamic_map, 
    				      "dialer_create_dynamic_map");
    reg_add_dialer_delete_dynamic_map(dialer_delete_dynamic_map, 
    				      "dialer_delete_dynamic_map");
    reg_add_interface_command_ok(IDBTYPE_DIALER,
				 dialer_interface_command_ok,
				 "dialer_interface_command_ok");
    reg_add_dialer_connect_name(dialer_connect_name,
 				"dialer_connect_name");
    reg_add_dialer_spoofing_int(is_spoofing_int, "is_spoofing_int");
    reg_add_dialer_spoofing_prot(is_spoofing_prot, "is_spoofing_prot");
    reg_add_dialer_huntgroup_member(huntgroup_member, "huntgroup_member");
    reg_add_dialer_huntgroup_leader(huntgroup_leader, "huntgroup_leader");
    reg_add_dialer_is_in_use(dialer_is_in_use, "dialer_is_in_use");
    reg_add_pre_cstate(dtr_dialer_return, "dtr_dialer_return");
    reg_add_for_all_dialer_members(for_all_huntgroup_members,
				   "for_all_huntgroup_members");
    reg_add_dialer_change_pak_if(dialer_change_pak_if, "dialer_change_pak_if");
    reg_add_get_dialer_string(get_dialer_string, "get_dialer_string");
    reg_add_default_dialer_save_config(
               (service_dialer_save_config_type)return_null, "return_null");
    reg_add_default_dialer_restore_config(
              (service_dialer_restore_config_type)return_null, "return_null");
    reg_add_dialer_pool_acl_idb(dialer_pool_acl_idb, "dialer_pool_acl_idb");
    reg_add_dialer_get_conn_member(dialer_get_conn_member, "dialer_get_conn_member");
    reg_add_dialer_get_connected_group(dialer_get_connected_group,
				       "dialer_get_connected_group");
    /*
     * Initialize Dialer parser support
     */
    dialer_parser_init();
}

/*
 * DIALER subsystem header
 */

#define DIALER_MAJVERSION 1
#define DIALER_MINVERSION 0
#define DIALER_EDITVERSION  1

SUBSYS_HEADER(dialer, DIALER_MAJVERSION, DIALER_MINVERSION, DIALER_EDITVERSION,
	      dialer_init, SUBSYS_CLASS_LIBRARY,
	      NULL,
	      NULL);
