/* $Id: network.c,v 3.32.4.68 1996/09/15 22:22:23 fox Exp $
 * $Source: /release/112/cvs/Xsys/if/network.c,v $
 *------------------------------------------------------------------
 * network.c -- device and protocol independent network interface support
 *
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: network.c,v $
 * Revision 3.32.4.68  1996/09/15  22:22:23  fox
 * CSCdi66416:  PPP MRU negotiation causes interoperability problems
 * Branch: California_branch
 * Make sure that PPP interface is in received for at least 1500 bytes.
 *
 * Revision 3.32.4.67  1996/09/06  01:11:06  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.32.4.66  1996/08/29  02:24:37  xtang
 * CSCdi60447:  FSIP max MT=14816 not 18000
 * Branch: California_branch
 *              Implemented new FSIP command 93 to pass the available
 *              buffer size in the controller to the system.
 *              The corresponding microcode change are in
 *              cvs -d /micro fsip_q (fsip187-0)
 *              and cvs -d /micro fsip_i (rsp_fsip204-0).
 *
 * Revision 3.32.4.65  1996/08/28  12:48:37  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.32.4.64  1996/08/26  15:09:01  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.32.4.63  1996/08/24  04:59:37  fred
 * CSCdi67070:  remove upper bound from red queues
 * Branch: California_branch
 *
 * Revision 3.32.4.62  1996/08/23  08:09:47  xtang
 * CSCdi60427:  parser error when set mtu size on hssi pa.
 * Branch: California_branch
 *              CSCdi60474 get negative maxdatagram when set max mtu
 *              on hssi pa. is found to be a duplicate of this bug.
 *              CSCdi39825 patch for 11.2 largely solved the
 *              problem for 11.2. Now add a recovery of the
 *              swidb->user_configured_mtu flag.
 *
 * Revision 3.32.4.61  1996/08/22  17:05:04  fox
 * CSCdi66796:  print_linktype() does not support all defined linktypes
 * Branch: California_branch
 * Add a few missing linktypes.
 *
 * Revision 3.32.4.60  1996/08/20  18:08:38  wmay
 * CSCdi39825:  ISDN PRI, setting d-channel mtu size not reflected on
 * b-channels - set for entire dialer rotary group
 * Branch: California_branch
 *
 * Revision 3.32.4.59  1996/08/19  06:36:18  asastry
 * CSCdi65585:  Priority queuing not working for bridged packets.
 * Branch: California_branch
 *
 * Revision 3.32.4.58  1996/08/19  04:01:19  cyoung
 * Add label to accounting array
 * CSCdi66424:  VTP label doesnt show on show int acc
 * Branch: California_branch
 *
 * Revision 3.32.4.57  1996/08/17  16:53:33  fox
 * CSCdi66403:  PPP on E1 Channel 30 can not be set with no keepalive
 * Branch: California_branch
 * Check should be ix <= DSX1_MAX_CHAN since DSX1_MAX_CHAN is already n-1.
 *
 * Revision 3.32.4.56  1996/08/17  00:23:25  fred
 * CSCdi61598:  spurious memory access in fair_enqueue/dequeue
 *              caused by tty interrupt interleaving with nets interrupt;
 *              need to control interrupts entirely, so use standard
 *              routines.
 * Branch: California_branch
 *
 * Revision 3.32.4.55  1996/08/16  19:16:32  fred
 * CSCdi63542:  router crashes with RED configuration
 * Branch: California_branch
 *         Control Interrupts while playing with the queue
 *
 * Revision 3.32.4.54  1996/08/15  02:32:40  hampton
 * Change some function arguments to be 'const'.  [CSCdi66102]
 * Branch: California_branch
 *
 * Revision 3.32.4.53  1996/08/13  02:18:59  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.32.4.52  1996/08/12  02:15:56  ppearce
 * CSCdi64450:  CSCdi59527 broke c4500-boot-m image
 * Branch: California_branch
 *   Add srb_core_registry that will definitely be included w/SRB subblock
 *
 * Revision 3.32.4.51  1996/08/07  23:46:40  wmay
 * CSCdi46781:  PRI load not computed correctly - compute backup load for
 * entire dialer hunt group
 * Branch: California_branch
 *
 * Revision 3.32.4.50  1996/08/07  23:38:44  fred
 * CSCdi65442:  bandwidth command doesn't change wfq state
 * Branch: California_branch
 *
 * Revision 3.32.4.49  1996/08/07  08:59:55  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.32.4.48  1996/08/03  23:27:09  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.32.4.47  1996/08/03  00:10:25  fred
 * CSCdi64876:  holdq_reset is not efficient
 *         Operationally remove a FOR_ALL_HWIDBS loop
 * Branch: California_branch
 *
 * Revision 3.32.4.46  1996/07/30  18:19:28  bbenson
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * First commit of this ddts omitted registry calls out of network.c into
 * the lane subsys.
 *
 * Revision 3.32.4.45  1996/07/30  14:42:03  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.32.4.44  1996/07/28  07:32:58  irfan
 * CSCdi60568:  v120 autodetect not working
 * Branch: California_branch
 * allow serial interfaces to autodetect encaps on the wire for incoming
 * connections.
 *
 * Revision 3.32.4.43  1996/07/27  00:39:45  schiang
 * CSCdi64264:  add NDLC to enc_keywords[]
 * Branch: California_branch
 *
 * Revision 3.32.4.42  1996/07/25  00:16:00  rbadri
 * CSCdi63919:  LANE Client can invalidate fast cache entries too often
 * Branch: California_branch
 * New flag to facilitate better cache invalidation when VCs go down.
 * When there is no change in line state, subif_line_statechange()
 * should just return.
 *
 * Revision 3.32.4.41  1996/07/21  04:33:52  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   srb_sb_set_si_counters causes build error in c100x images
 *     use SRB vector table registry instead of direct invocation
 *
 * Revision 3.32.4.40  1996/07/20  03:18:40  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.32.4.39  1996/07/19  00:48:52  irfan
 * CSCdi57534:  Load value erroneous when output bps > int bandwidth
 * Branch: California_branch
 * use the actual traffic rate for computation instead of perceived
 * bandwidth in such degenerate cases.
 *
 * Revision 3.32.4.38  1996/07/18  20:28:41  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.32.4.37  1996/07/10  09:57:30  dawnli
 * CSCdi54969:  HSRP tracking does not track Frame Relay sub-interfaces
 * Branch: California_branch
 *
 * Revision 3.32.4.36  1996/07/08  22:31:13  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.32.4.35  1996/06/27  21:41:55  rbadri
 * CSCdi61523:  Can not configure MTU on a sub-if greater than hwidb (for
 * ATM)
 * Branch: California_branch
 * Apply the mtu command on the major interface to .0 interface.
 * sub-interfaces will have an MTU based on the configuration in them or
 * the default from hwidb.
 *
 * Revision 3.32.4.34  1996/06/27  09:34:02  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.32.4.33  1996/06/19  00:26:07  carvalho
 * CSCdi33844:  X25 over B-channel broken
 * Branch: California_branch
 *
 * When changing encapsulations, revert to default queueing method if
 * current queueing method is illegal for new encapsulation.
 *
 * Revision 3.32.4.32  1996/06/18  01:44:20  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.32.4.31  1996/06/17  23:31:24  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.32.4.30  1996/06/17  08:32:08  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.32.4.29  1996/06/12  16:45:51  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - put the b channel idbs in the idb list - this removes
 * a lot of the special cases for BRI in the system code.
 * Branch: California_branch
 *
 * Revision 3.32.4.28  1996/06/11  09:30:35  dstine
 * CSCdi57115:  Counts not cleared with clear counters command
 * Branch: California_branch
 *
 * Revision 3.32.4.27  1996/06/07  20:46:21  fred
 * CSCdi58655:  turning RED off on an interface turns WFQ off on that
 *              interface. Save default in a variable and restore that
 *              when folks turn off RED, Priority, or Custom Queuing.
 * Branch: California_branch
 *
 * Revision 3.32.4.26  1996/06/05  20:56:25  wmay
 * CSCdi59462:  DSX1 remote interface loopback process needs help
 * More protection on when a remote loop can be executed.  Use
 * far_end_looped field to actually indicate if a remote loop is
 * present, and display more state info in show int while remote
 * loop request is being processed
 * Branch: California_branch
 *
 * Revision 3.32.4.25  1996/06/03  23:13:17  tylin
 * CSCdi57791:  no way to tell which queuing strategy assigned to interface
 * Branch: California_branch
 *
 * Revision 3.32.4.24  1996/05/23  22:39:54  sdurham
 * CSCdi58231:  ifLastChange does not update on line protocol change
 * Branch: California_branch
 *         update hwidb->state_time when hwidb->linestate changes.
 *
 * Revision 3.32.4.23  1996/05/21  09:50:04  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.32.4.22  1996/05/17  11:22:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.29.2.5  1996/05/05  23:09:52  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.29.2.4  1996/04/26  07:55:16  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.29.2.3  1996/04/03  14:38:23  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.29.2.2  1996/03/20  18:06:13  ppearce
 * CSCdi51441:  Local Timeout when booting router with CIP installed
 * Branch: IbuMod_Calif_branch
 *   Need fix integrated into branch for CIP/CSNA testing
 *
 * Revision 3.29.2.1  1996/03/17  17:51:18  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.32.4.21  1996/05/13  14:47:14  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.32.4.20  1996/05/10  18:17:53  hampton
 * Don't call reg_invoke_serial_cfg_modified on all interfaces, just on
 * serial interfaces.  [CSCdi56505]
 * Branch: California_branch
 *
 * Revision 3.32.4.19  1996/05/10  01:38:32  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.32.4.18  1996/05/09  14:30:09  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.32.4.17  1996/05/08  05:39:47  ccpoe
 * CSCdi56330:  serial_setencap in network.c broke modularity. Turn off
 * timers and don't change the encapsulation to default.
 * Branch: California_branch
 *
 * Revision 3.32.4.16  1996/05/08  01:46:48  lwei
 * CSCdi56122:  Tunnel interface default MTU should not show up in nvgen
 * output
 * Branch: California_branch
 *
 * Revision 3.32.4.15  1996/05/07  23:32:47  wilber
 * CSCdi56986:  Hidden command show interface <int> switching is broken
 * New command overwrote its parser variable OBJ(int, x)
 * Branch: California_branch
 *
 * Revision 3.32.4.14  1996/05/05  19:42:16  mwu
 * CSCdi56290:  channel-group command crashes 75xx routers caused
 * by CSCdi54721.
 * Branch: California_branch
 *
 * Revision 3.32.4.13  1996/05/04  00:46:42  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Restore the input to the Bridge-group Virtual Interface if necessary
 * - Support user settable mtu for the BVI.
 * - Introduce global_irb_enable, SET_BRIDGING_RXTYPE and "show idb irb"
 *
 * Revision 3.32.4.12  1996/04/30  23:16:58  sdurham
 * CSCdi46300:  Interface missing from MIB
 * Branch: California_branch
 *          add atm and aal5 layers to the ifmib
 *
 * Revision 3.32.4.11  1996/04/27  05:49:55  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.32.4.10.2.2  1996/04/30  18:25:15  rbadri
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * In sh int, show the sub MTU value for ATM interfaces only.
 *
 * Revision 3.32.4.10.2.1  1996/04/27  07:03:55  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.32.4.10  1996/04/25  23:13:33  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.32.4.9  1996/04/21  05:51:30  dkatz
 * CSCdi55237:  Net Input process stack is too small
 * Branch: California_branch
 *
 * Revision 3.32.4.8  1996/04/20  01:50:31  dino
 * CSCdi54104:  CGMP must support source-only systems in switched Cat5000
 *              network
 * Branch: California_branch
 *
 * Revision 3.20.2.4  1996/04/26  01:07:08  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.20.2.3  1996/04/17  01:59:50  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.20.2.2  1996/02/23  20:22:49  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.20.2.1  1996/02/06  20:21:03  perryl
 * Branch: Dial1_branch
 *
 * Revision 3.32.4.7  1996/04/17  13:40:14  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.32.4.6  1996/04/13  00:46:25  cheilek
 * CSCdi48224:  -FX FE Port Adapter, reported as -TX
 * Branch: California_branch
 * Change print to read TX/FX. Also added code to show diag output to
 * identify precisely which Fast Ethernet PA is installed.
 *
 * Revision 3.32.4.5  1996/04/05  22:02:59  wmay
 * CSCdi49977:  backup delay parameters reversed for subinterfaces
 * Branch: California_branch
 *
 * Revision 3.32.4.4.4.3  1996/04/22  17:37:06  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Fix the cosmetic problems related to "no mtu"
 *
 * Revision 3.32.4.4.4.2  1996/04/11  00:04:09  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Track manual configuration of MTU on swidb basis
 * Save sub-interface MTU to NVRAM
 *
 * Revision 3.32.4.4.4.1  1996/04/08  01:54:44  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.32.4.4  1996/03/27  09:02:14  speakman
 * CSCdi51444:  Blocked bridge ports do not respond to ARP broadcasts.
 * Branch: California_branch
 * Delete the list of rxtypes from 'show int crb' since it confuses
 * more people than it enlightens.
 *
 * Revision 3.32.4.3  1996/03/26  20:29:38  asastry
 * CSCdi51441:  Local Timeout when booting router with CIP installed
 * Branch: California_branch
 *
 * Revision 3.32.4.2  1996/03/21  22:49:06  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.32.4.1  1996/03/18  20:14:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.32.6.2  1996/03/28  17:25:36  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.32.6.1  1996/03/22  09:37:20  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.32  1996/03/09  22:55:15  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.31  1996/03/04  10:49:52  fred
 * CSCdi47191:  MBRI cannot disable WFQ - screws up multilink
 *         enable WFQ use by any link that cares t
 *
 * Revision 3.30  1996/02/27  18:45:07  rbatz
 * CSCdi49933:  Command "full-duplex" displayed incorrectly as
 *              "full_duplex".  Also, make 11.0, 11.1 "show int"
 *              for half-duplex the same.
 *
 * Revision 3.29  1996/02/15  18:02:00  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.28  1996/02/13  21:25:50  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.27  1996/02/13  08:13:05  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.26.6.1  1996/03/05  06:13:55  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.26.2.1  1996/02/27  21:08:25  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.26  1996/02/08  03:21:31  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.25  1996/02/07  16:12:37  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.24  1996/02/07  00:47:03  irfan
 * CSCdi48054:  async interfaces should default to no keepalive
 * Allow hwidb's to have default other than KEEPALIVE_DEFAULT.
 *
 * Revision 3.23  1996/01/31  21:15:59  motto
 * CSCdi47239:  DLSW+ doesnt forward test frames to CIP internal lan
 *
 * Revision 3.22.2.1  1996/01/24  22:18:37  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.22  1996/01/18  03:22:39  kchiu
 * CSCdi43616:  Router needs to set express buffer bit on RPS response
 *
 * Revision 3.21  1996/01/15  17:17:19  misko
 * CSCdi37400:  2511 passes packets after ipcp completes but before
 * reporting int up
 * Allow PPP to notify IP when IP comes up.  Since PPP can have NCP's that
 * come and go, and since IP routes may be installed when PPP negotiated,
 * we need to be able to tell IP to propogate those routes.  Otherwise
 * a lot of traffic gets dropped.  Take advantage of existing registrys
 * to notify IP when PPP thinks it is up.  In addition, fix network.c so
 * that when line protocol comes up, route propogation is not delayed by
 * the keepalive period.
 *
 * Revision 3.20  1996/01/13  01:34:40  irfan
 * CSCdi44881:  dialer interface dead after removal and redefine
 * disallow removal of dialer interface as long as it has members in
 * the rotary group.
 *
 * Revision 3.19.2.5  1996/02/15  19:13:54  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.19.2.4  1996/02/13  00:22:28  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Remove unnecessary include of lane.h
 *
 * Revision 3.19.2.3  1996/02/13  00:18:39  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.19.2.2  1996/01/23  17:42:46  cakyol
 * set the line state BEFORE calling everyone else
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.19.2.1  1996/01/11  01:45:27  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.19  1996/01/05  02:27:24  john
 * CSCdi44602:  xns ping fails to go through over aip (rsp1)
 *              if no mac-addr is set "no mac-addr" should do nothing
 *
 * Revision 3.18  1996/01/05  00:15:11  fred
 * CSCdi45120:  No fair-queue command does not take effect on async
 * interface
 *         fair queuing was being re-enabled during link reset. changed
 *         logic to
 *         only disable state automatically, not enable it
 *
 * Revision 3.17  1995/12/20  23:45:37  gstovall
 * CSCdi45909:  Item->handled field tested while uninitialized
 * Make sure that item->handled is initialized.  Bad programmer, no pizza.
 *
 * Revision 3.16  1995/12/17  06:03:38  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.15  1995/12/12  23:27:14  fred
 * CSCdi42501:  Weighted Fair Queuing (WFQ) uses too much memory.
 *         Remove RSVP queues and hash the link layer protocol queues.
 *         Long term fix in Californi
 *
 * Revision 3.14.4.1  1995/12/23  03:59:25  dcheng
 * Branch: LE_Calif_branch
 * Added support for VTP.
 *
 * Revision 3.14  1995/12/02  08:23:32  mschaef
 * CSCdi37520:  mac-address command not taken upon router reload or
 * initial config
 *
 * Revision 3.13  1995/11/26  10:52:22  mleelani
 * CSCdi36372:  Bootstrap image displays line up/down, changed info during
 * boot
 * Do not display line up/down messages on boot.
 *
 * Revision 3.12  1995/11/26  08:13:18  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *              Catalyst switches now support the use of the Cisco
 *              multicast 0x01000C000000 for ISL vLAN encapsulated
 *              packets.
 *
 * Revision 3.11  1995/11/24  19:38:19  rbadri
 * CSCdi36121:  Line protocol is up, while LANE client is non-operational
 * subif_line_state variable will track the swidb's line state. It
 * is not normally altered unless the protocols controlling the
 * sub-interface wish to do so. An interface routine
 * subif_line_statechange() is provided to change swidb's line state.
 *
 * Revision 3.10  1995/11/21  19:08:49  snyder
 * CSCdi44272:  more spellink errors
 *              ofhunt -> of hunt
 *
 * Revision 3.9  1995/11/20  23:35:16  wmay
 * CSCdi35668:  load-interval only affects D channel, not B channels -
 * propogate change to all hunt-group members.
 *
 * Revision 3.8  1995/11/20  23:00:53  wmay
 * CSCdi32869:  Cannot change hold-queue for BRI interfaces - update
 * all hunt group members when change for hunt group master.
 *
 * Revision 3.7  1995/11/20  22:41:03  snyder
 * CSCdi44202:  spellink errors
 *              Colour -> Color, so I am anal...
 *
 * Revision 3.6  1995/11/20  22:25:27  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.5  1995/11/20  19:35:08  turadek
 * CSCdi39149:  Some ethernet frames are not passed on HDLC when compress
 * stac used.  Bridging when using HDLC compression was not working
 * correctly.  Apply some fixes that were in original prototype but
 * didn't make it into the released version.
 * Just syncing this from 11.0.
 *
 * Revision 3.4  1995/11/18  17:24:18  fox
 * CSCdi43511:  4500 will crash when T1 controller configurations is
 * removed
 * Clear just the ip_oprotofltr array and not the next variable as well.
 * Remember, 0 to n-1, not 0 to n.
 *
 * Revision 3.3  1995/11/17  09:30:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:36  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:51:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.46  1995/11/08  21:03:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.45  1995/10/21  19:09:46  fred
 * CSCdi39516:  WFQ breaks TB flooding when congested - %SYS-2-BADSHARE
 * Consistently enable use of fast path with priority/custom/fair queuing
 * for bridging
 *
 * Revision 2.44  1995/10/18  17:19:12  wmay
 * CSCdi39845:  show interface displays serials in a non-intuitive manner
 * Sort high end idb's by slot/slotunit/vc, low end by unit/vc.
 *
 * Default slot and slotunit are changed to MAXUINT.  Virtual interfaces
 * (tunnel, loop, async) shouldn't change them, as parser is now smart
 * enough to use unit on high end.
 *
 * Sort swidbs by subinterface number, rather than alphabetically
 * (CSCdi35897).
 *
 * Revision 2.43  1995/10/13  20:27:05  shj
 * CSCdi41281:  memory leak associated with loopback interface crashes
 * router
 * Define ET_LOOP to be used in hwidb->enctype on loopback interfaces.
 * Prevents set_default_queueing from changing hwidb->oqueue on loopback
 * interfaces, which led to packets being queued forever.
 *
 * Revision 2.42  1995/09/30  05:03:25  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.41  1995/09/25  08:32:56  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.40  1995/09/23  21:26:56  gglick
 * CSCdi40746:  X.25 and LAPB not working
 *
 * Revision 2.39  1995/09/20  22:36:00  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.38  1995/09/20  16:05:25  kramling
 * CSCdi38143:  Hide Commands that do *special* Loopbacks with FastEther
 * PAs
 *
 * Revision 2.37  1995/09/16  01:07:24  fred
 * CSCdi40357:  Make WFQ configuration work correctly with Virtual and
 * Null IDBs.
 *
 * Revision 2.36  1995/09/13  13:44:37  gglick
 * CSCdi38693:  lapb disables priority queuing
 *         Remove duplicate invocation of reg_invoke_serial_cfg_modified
 *         introduced by previously applied patch.
 *
 * Revision 2.35  1995/09/12  18:47:29  gglick
 * CSCdi38693:  lapb disables priority queuing
 *
 * Revision 2.34  1995/09/11  10:24:07  kmitchel
 * CSCdi25909:  Erroneous CIP configuration commands are still added to
 *              config file
 *
 * Revision 2.33  1995/09/09  00:52:43  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.32  1995/09/08  04:57:03  fred
 * CSCdi39816:  the previous patch worked in systems it was tested on, but
 *         failed on 4500. This works on 4500 as well.
 *
 * Revision 2.31  1995/09/07  04:31:56  fred
 * CSCdi39816:  interfaces requiring secondary queuing are broken.
 *              This includes tunnels, LAPB, X.25, Loopback, and a few
 *              others
 *
 * Revision 2.30  1995/09/06  17:45:19  raj
 * CSCdi39170:  show queueing does not use flow control
 * Actually it does but the flow control is being turned on and off in
 * subroutines so much that it's not effective.  I moved all flow control
 * to the higher level routines.
 *
 * Revision 2.29  1995/09/02  10:06:41  fred
 * CSCdi39438:  no fair does not disable WFQ
 *         implement idb->queue_algorithm to indicate the
 *         queuing algorithm deployed.
 *
 * Revision 2.28  1995/09/01  22:49:01  asastry
 * CSCdi39224:  WFQ conversation counters are inaccurate on 7000.
 *
 * Revision 2.27  1995/08/25  11:43:54  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.26  1995/08/23 18:18:56  fred
 * CSCdi39010:  WFQ + ip route-cache cbus on FSIP fails
 * Code cleanups to improve user understandibility of what's happening
 *
 * Revision 2.25  1995/08/18  23:50:07  speakman
 * CSCdi39005:  CRB commands collide
 * print_linktype should return 'ipx' not 'novell'
 *
 * Revision 2.24  1995/08/18  19:39:00  achopra
 * CSCdi34426:  Failed to bring up backup link in frame-relay multipoint
 *
 * Revision 2.23  1995/08/13  19:33:46  motto
 * CSCdi36478:  Show interface statistical info is missing for Channel
 * interface
 *
 * Revision 2.22  1995/08/10  23:30:46  wfried
 * CSCdi37608:  oam f5 cells generated by 4500 are not recognized by
 * analyzer
 * Changed ATM code to pass VC pointer for OAM cells and other related
 * changes
 *
 * Revision 2.21  1995/08/09  00:20:14  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.20  1995/08/08  19:28:49  fred
 * CSCdi36704:  IP Fastswitching / WFQ cleanup for high-end
 *         enable WFQ/PQ/CQ of SMDS interfaces.
 *         adjust WFQ discard policy to permit one conversation to use the
 *         entire congestive discard threshold
 *
 * Revision 2.19  1995/08/04  06:43:16  hampton
 * sched_private.h included too many places  [CSCdi38246]
 *
 * Revision 2.18  1995/08/01 03:50:49  fred
 * CSCdi36704:  disable use of fair, custom, priority queuing on SMDS,
 *              as the way the serial number of the message is generated
 *              precludes reordering messages in the output queue.
 *
 * Revision 2.17  1995/07/27  10:54:58  irfan
 * CSCdi37751:  Weighted Fair Queueing not configurable over Async
 * interfaces
 * fix it. Also add a cumulative counter of all packets FQ thinks it
 * ever kept around and a counter of all packets FQ thinks it gave back.
 *
 * Revision 2.16  1995/07/18  06:46:11  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.15  1995/07/14 18:21:37  fowler
 * CSCdi37070:  Backup interface shouldnt be activated by a deleted subif
 *
 * Revision 2.14  1995/07/10  05:39:08  fred
 * CSCdi36851:  disable fancy queuing if using autonomous, sse, flow, or
 * optimal switching - ie, only use it if process switching or fast
 * switching. Applies to AGS/7000/7500 platforms.
 *
 * Revision 2.13  1995/07/07  05:26:17  hampton
 * Hide the definitions of watched data structures.  These data structures
 * should never be referenced directly.  [CSCdi36799]
 *
 * Revision 2.12  1995/07/05 20:48:53  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.11  1995/06/27  00:40:59  lpereira
 * CSCdi36419:  not able to set mtu on serial interface
 *
 * Revision 2.10  1995/06/27  00:22:16  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.9  1995/06/26  02:31:53  percyk
 * CSCdi35013:  a no shut on an ALREADY up atm interface, resets
 * connections.
 *
 * Revision 2.8  1995/06/24  05:30:38  fred
 * CSCdi36396:  shutdown of running system locks router
 * Analysis: shutting down and restarting at a time when there was data in
 * queue could cause a linked list to remain in an indeterminate state.
 * Control interrupts while emptying fair queue and initialize all
 * pointers
 *
 * Revision 2.7  1995/06/21  21:17:24  fred
 * CSCdi35994:  LLC2 breaks fair queuing
 *         Rewrite fair_enqueue and fair_dequeue to use pak_enqueue
 *         and related services. Augment pak_enqueue services with
 *                 pak_queue_get_head(), pak_queue_get_tail(),
 *                 pak_queue_depth(), pak_queue_max_depth(),
 *                 pak_queue_set_max_depth(), and
 *                 pak_on_data_link_queue(paktype *pak)
 *         The latter is for the IBM code, which needs a way to determine
 *         whether a message is currently on a data link secondary queue
 *
 * Revision 2.6  1995/06/21  03:28:13  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.5  1995/06/18  02:29:35  hampton
 * Net_oneminute should not run with blocking disabled.  [CSCdi36036]
 *
 * Revision 2.4  1995/06/16 04:28:07  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.3  1995/06/14  15:31:35  gglick
 * CSCdi34551:  VINES X.25 interface down causes router to hang
 *
 * Revision 2.2  1995/06/09  03:34:43  fred
 * CSCdi35326:  %ALIGN-1-FATAL: Illegal access to a low address -
 *              fair_enqueue
 * CSCdi35163:  reported hang did not occur in this code version
 * CSCdi35342:  corrected syntax error in priority-list and queue-list
 *              command in the manner suggested in the DDTs.
 * Also - modified "show queueing fair" command output per customer
 * request
 *
 * Revision 2.1  1995/06/07  20:53:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "stacks.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "network.h"
#include "logger.h"
#include "../ip/ip.h"
#include "parser.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_priority.h"
#include "../parser/parser_defs_half_duplex.h"
#include "../if/priority_private.h"
#include "tring.h"
#include "../hes/if_fci.h"
#include "../tcp/tcpcompress.h"
#include "../wan/dialer.h"
#include "../wan/ppp.h"
#include "../wan/serial.h"
#include "../wan/serial_parser.h"
#include "../wan/isdn.h"
#include "../wan/dialer_registry.h"
#include "../les/isdn_bri.h"
#include "../ibm/sdlc.h"
#include "../ui/command.h"
#include "../snmp/snmp_api.h"
#include "../if/network_debug.h"
#include "../os/clock.h"
#include "if_controller.h"
#include "dsx1.h"
#include "mgd_timers.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../ts/if_async.h"
#include "msg_clear.c"			/* Not a typo, see logger.h */
#include "msg_lineproto.c"		/* Not a typo, see logger.h */

#include "../tbridge/tbridge_sde.h"
#include "../vlan/vlan.h"
#include "../srt/srb_core.h"
#include "../srt/srt_registry.h"
#include "../srt/srb_core_registry.h"
#include "../xns/novell.h"
#include "../partner/partner_registry.h"
#include "linkdown_event.h"
#include "../util/itemlist.h"
#include "../smf/smf_public.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../os/element.h"
#include "../snmp/ifmib_registry.h"
#include "../ip/ip_registry.h"
#include "../util/random.h"
#include "traffic_shape.h"
#include "../tcp/http.h"
#include "../lane/lane_registry.h"
#include "../if/if_vtemplate.h"

/*
 * Local Storage
 */
mgd_timer if_cstate_timer;	/* time to check flapping interfaces */
idbtype *nullidb;
static watched_queue *net_inputQ;
static watched_queue *net_abortQ;
uchar default_mac_addr[IEEEBYTES]; /* Box-wide default MAC address */
watched_boolean *net_throttle_flag;
static pid_t netinput_pid;

const char set_maxdgram_failed[] =
    "\n%% Failed to change maxdgram size to %d on interface %s";

/*
 * This next item should be in if_slip.c or if_async.c, but the SVIP
 * fails to compile.  The two references to slip_traffic should be
 * removed from this file, and then the array itself should be moved out.
 */
slip_traffic_t slip_traffic;

static process compute_loads(void);
static void cstate_check(void);
static void backup_timers(hwidbtype *);
static void backup_loads(hwidbtype *);
static void bridge_adjust(hwidbtype *);
static void show_priority_queue_command (idbtype *swidb, hwidbtype *idb, 
					 int queuenum);



static char *const link2ntable[LINK_MAXLINKTYPE] = {
    /* 0*/ "ILLEGAL", "ARP", "RARP", "XARP", "PROBE",
    /* 5*/ "LOOP", "ADDRESS", "IP", "PUP", "9",
    /*10*/ "XNS", "NOVELL", "APOLLO", "VINES", "PAD",
    /*15*/ "X25", "APPLETALK", "AARP", "DEC_SPANNING", "IEEE_SPANNING",
    /*20*/ "TCP2PAD", "PAD2TCP", "DECNET", "DECNET_ROUTER-L1", "DECNET_NODE",
    /*25*/ "CLNS", "CLNS_ES", "CLNS_IS", "CLNS_BCAST", "X25TCP",
    /*30*/ "PPP", "LAT", "VINES-ECHO", "VINES_LOOP", "ATALK_SHORT",
    /*35*/ "MOP_BOOT", "MOP_CONSOLE", "RSRB", "BRIDGE", "STUN",
    /*40*/ "FR_ARP", "SMDS_ARP", "MAC", "IBMNM", "UARP",
    /*45*/ "ULTRA_HELLO", "X25SERVICE", "UNCOMPRESSEDTCP", "COMPRESSEDTCP",
	"LLC2",
    /*50*/ "CMNS", "ISIS_ALL_L1_IS", "ISIS_ALL_L2_IS", "FR_SWITCH",
	"DECNET_PRIME_ROUTER",
    /*55*/ "SRB", "QLLC", "X25-MULTIPROTOCOL", "LEX", "LEX_RCMD",
    /*60*/ "DECNET_ROUTER-L2", "LINK_CLS", "SNAPSHOT", "DLSW", "SDE", "CDP",
    /*66*/ "PPP COMPRESSION", "NMP", "BSTUN", "IPC", "LOVE", "CFGACK",
    /*72*/ "APPN_ANR", "MULTILINK", "NHRP", "MAC_EXP", "CGMP", "VTP", "CPP",
    /*79*/ "ISL"
};

/*
 * Routeable protocol link types:
 */

#define	MAX_RP_LTYPES	8

static const ushort rp_ltypes[MAX_RP_LTYPES] = {
    LINK_APPLETALK, LINK_CLNS, LINK_DECNET, LINK_IP,
    LINK_VINES, LINK_APOLLO, LINK_NOVELL, LINK_XNS
    };

/*
 * Bridging.
 * These two data structures should really be moved into srt/bridge.c,
 * but since they are referenced by a number of drivers, it is easier
 * to declare and initialize them here.
 */
queuetype bteQ;			/* queue of free bridge table entries  */
int bte_count;			/* count of entries in bteQ  */


/*
 * Mac level address
 */
uchar dec_span_address[] = { 0x09, 0x00, 0x2b, 0x01, 0x00, 0x01 };
uchar ieee_span_address[] = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x00 };
uchar esis_8023_all_es_multi[] = { 0x09, 0x00, 0x2b, 0x00, 0x00, 0x04 };
uchar esis_8023_all_is_multi[] = { 0x09, 0x00, 0x2b, 0x00, 0x00, 0x05 };
uchar esis_8025_all_es_multi[] = { 0xC0, 0x00, 0x00, 0x08, 0x40, 0x00 };
uchar esis_8025_all_is_multi[] = { 0xC0, 0x00, 0x00, 0x10, 0x80, 0x00 };
uchar isis_8025_all_is_multi[] = { 0xC0, 0x00, 0x00, 0x00, 0x80, 0x00 };
uchar isis_all_l1_is_multi[] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x14 };
uchar isis_all_l2_is_multi[] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x15 };
uchar cgmp_address[] = { 0x01, 0x00, 0x0c, 0xdd, 0xdd, 0xdd };
/*
 * The following is a block of 256 contiguous addresses for the hot standby
 * protocol.
 */
uchar standby_groups[] = { 0x00, 0x00, 0x0c, 0x07, 0xac, 0x00 };
uchar transit_bridge_multicast[] = { 0x03, 0x00, 0x0c, 0x00,0x00, 0x01 };
uchar bit_swapped_transit_multicast[] = { 0xc0, 0x00, 0x30, 0x00, 0x00, 0x80 };
uchar trbaddr1[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
const uchar apple_snap[SNAP_OUIBYTES]    = {0x08, 0x00, 0x07};
const uchar smds_bridge_oui[SNAP_OUIBYTES] = {0x00, 0x80, 0xC2};

/*
 * The following are the multicast DA and SNAP portion 
 * used by Inter Switch Link (ISL) vLAN frames.
 */

uchar isl_vlan_multicast[IEEEBYTES] = { 0x01,0x00,0x0C,0x00,0x00,0x00 };

uchar isl_vlan_snap[IEEEBYTES] =      { 0xAA,0xAA,0x03,0x01,0x00,0x0C };

const uchar atm_tbridge_oui[SNAP_OUIBYTES] = {0x00, 0x80, 0xc2};


/*
 * Bits used for the dial backup stuff to determine whether action should
 * be taken on kicking in or out of the secondary due to load to up/down 
 * status of the primary.  These bits are in idb->backup_state.
 */
#define BACKUP_IN_LOAD   0x01
#define BACKUP_OUT_LOAD  0x02
#define BACKUP_IN_DELAY  0x04
#define BACKUP_OUT_DELAY 0x08
#define BACKUP_LOAD_MASK (BACKUP_IN_LOAD | BACKUP_OUT_LOAD)


/*
 * linktype_is_fq_conversation
 *
 * return default fair conversation id, manufactured from link type
 */

short linktype_is_fq_conversation (paktype *pak)
{
    /*
     * produce result in (fq_hash_mask+1)..FAIR_QUEUE_DYN_FLOWS
     *
     * note that 0..255 is the hashed range, and the LINK_MAXTYPE
     * non-hashed protocols are hashed into queues
     * (fq_hash_mask+1)..(fq_hash_mask+1)+FAIR_LINK_TYPE_HASH
     */

    if (pak->flags & PAK_PRIORITY)
	pak->fair_weight = FAIR_QUEUE_PAK_PRIORITY_WEIGHT;
    else
	pak->fair_weight = FAIR_QUEUE_BASE_WEIGHT;
    return (pak->fq_hash_mask
	    + (FAIR_LINK_TYPE_HASH & pak->linktype) + 1);
}



/*
 * link2name
 * Return the 'name' of a link, given its linktype. Does bounds checking
 * as well as handle novell -> ipx name kludge (will go away in NAC1).
 */
char *link2name (int linktype)
{
    /* Force a return of 'ILLEGAL' if the link type is out of range. */
    if ((linktype < 0) || (linktype >= LINK_MAXLINKTYPE))
        linktype = 0;

    /* Handle Novell/IPX name situation */
    if (linktype == LINK_NOVELL) {
	return("NOVELL");
    } else {
	return(link2ntable[linktype]);
    }
}

/*
 * holdq_init
 * Initialize the output hold queues
 */

void holdq_init (hwidbtype *idb, int qsize)
{
    int i;

    set_default_queueing(idb);
    for (i = 0; i < PRIORITY_MAXTYPE; i++) {
	queue_init(&(idb->outputq[i]), qsize);
	idb->output_defqcount[i] = qsize;
    }
}

/*
 * holdq_resize
 * Resize the output hold queues
 */

void holdq_resize (hwidbtype *idb, int qsize)
{
    int i;

    for (i = 0; i < PRIORITY_MAXTYPE; i++) 
	pakqueue_resize(&(idb->outputq[i]), qsize);
}

/*
 * set_default_queueing
 *
 * ensure that idb->queue_algorithm is appropriate for this interface and
 * encapsulation
 *
 * disable interrupts while adjusting queuing structures, as interrupt
 * routines also play with these data structures.
 */

void set_default_queueing (hwidbtype *idb)
{
    int status;
    
    reg_invoke_for_all_dialer_members(idb, set_default_queueing);
    
    holdq_reset(idb);

    status = raise_interrupt_level(NETS_DISABLE);
    /*
     * We may be called for an encapsulation change, which may invalidate
     * the current queueing choice. If so, revert to the default: fair
     * queueing if the encapsulation/link speed  supports it, else FCFS.
     * Also set the output queue length back to the default, so we don't
     * generate a spurious "hold-queue n out" line.
     */
    if ((idb->queue_algorithm == QUEUE_CUSTOM &&
	 !custom_queue_supported(idb)) ||
	(idb->queue_algorithm == QUEUE_PRIORITY &&
	 !priority_queue_supported(idb))) {
	idb->custom_queuenum = 0;
	idb->priority_list = 0;
	if (fair_queue_bandwidth(idb)) {
	    idb->queue_algorithm = QUEUE_FAIR_DEF;
	} else {
	    idb->queue_algorithm = QUEUE_FCFS;
	    idb->outputq[DEFAULT_QUEUE(idb)].maximum =
 		 hwidb_holdqueue_size_or_default(idb);
	}
    }
    switch (idb->queue_algorithm) {
    case QUEUE_RED:
    case QUEUE_CUSTOM:
    case QUEUE_PRIORITY:
	/*
	 * these have already been initialized when the command was
	 * interpreted; don't change anything.
	 */
	break;

    case QUEUE_FCFS:
	if (queuing_can_be_modifed(idb)) {
	    idb->oqueue = holdq_enqueue;
	    idb->oqueue_dequeue = holdq_dequeue;
	}
  	break;

    case QUEUE_FAIR:
    case QUEUE_FAIR_DEF:	
        idb->custom_queuenum = 0;
        if (fair_queue_supported(idb) && priority_ip_routecache(idb)) {
            build_fair_queue_head(idb, FAIR_QUEUE_DEFAULT_QDEPTH,
                                  FAIR_QUEUE_HASH_QUEUES_DEFAULT,
                                  FAIR_RSVP_QUEUES_DEFAULT);
  	} else {
	    if (queuing_can_be_modifed(idb)) {
		idb->oqueue = holdq_enqueue;
		idb->oqueue_dequeue = holdq_dequeue;
	    }
  	    if (idb->fair_queue_head) {
  		free(idb->fair_queue_head);
		idb->oqueue = holdq_enqueue;
		idb->oqueue_dequeue = holdq_dequeue;
	    }
  	    idb->queue_algorithm = QUEUE_FCFS;
  	    idb->priority_list = 0;
  	    idb->fair_queue_head = NULL;
  	}
  	idb->old_queue_algorithm = idb->queue_algorithm;
	break;
    }

    /*
     *  invoke the configuration modified registry even if we didn't
     *  actually update the queuing vectors; configuring a protocol that
     *  uses its own vectors and the standard functions (e.g. HDLC -> LAPB)
     *  will need this notification
     */
    
    reg_invoke_interface_cfg_modified(idb);
    reset_interrupt_level(status);
}

static void reset_fair_queue(fair_queue_head *fq,
			     fair_queue *queue, 
			     int queue_count) {
    paktype *pak;
    for (; --queue_count >= 0; queue++) {
	    /*
	     *  flush queue contents
	     */
	    queue->fq_fore = NULL;
	    queue->fq_next = NULL;
	    while ((pak = pak_dequeue(&queue->q))) {
		pak->flags &= ~PAK_DLQ;
		datagram_done(pak);
	    }

	    /*
	     * initialize the queue
	     */
	    queue_init(&queue->q, pak_queue_max_depth(&queue->q));
	    queue->fq_fore = NULL;
	    queue->fq_next = NULL;
	}
}

/*
 * reset_idb_holdq
 * Make interface queue limits match the desired queue limits.
 */
static void reset_idb_holdq (
    hwidbtype *idb)
{
    int i, qsize;
    fair_queue_head *fq = idb->fair_queue_head;
    paktype *pak;
    int status;

    if (fq) {
    	status = raise_interrupt_level(NETS_DISABLE);
	fq->fq_first = NULL;
	if (fq->fq_hashed_queue)
	    reset_fair_queue(fq, fq->fq_hashed_queue, 1 + fq->fq_hash_mask);
	reset_fair_queue(fq, fq->fq_link_queue, 1 + FAIR_LINK_TYPE_HASH);
	if (fq->fq_reserved_queue)
	    reset_fair_queue(fq, fq->fq_reserved_queue, 
			     fq->fq_queue_count - fq->fq_reserved_queue_start);
	fq->fq_active_conversations = 0;        /* CSCdi39224 */
	fq->fq_allocated_conversations = 0;
	fq->fq_max_allocated_conversations = 0;
    	reset_interrupt_level(status);
    } else {
	for (i = 0; i < PRIORITY_MAXTYPE; i++) {
	    /*
	     *  flush queue contents
	     */
	    while (idb->outputq[i].qhead) {
		pak = pak_dequeue(&idb->outputq[i]);
		pak->flags &= ~PAK_DLQ;
		datagram_done(pak);
	    }

	    /*
	     *  adjust queue limit
	     */
	    if (idb->priority_list) {
		qsize = priority_qsize(idb,i);
	    } else {
		qsize =  hwidb_holdqueue_size_or_default(idb);
	    }
	    queue_init(&(idb->outputq[i]), qsize);
	}
    }
    /*
     *  clear the count of packets waiting for transmit
     */
    idb->output_qcount = 0;
}

/*
 * holdq_reset
 * Make interface queue limits match the desired queue limits.
 * Call with target == NULL to reset all interfaces.
 */

void holdq_reset (hwidbtype *target)
{
    hwidbtype *idb;
    int status;

    status = raise_interrupt_level(NETS_DISABLE);
    if (target)
	reset_idb_holdq(target);
    else {
	FOR_ALL_HWIDBS(idb) {
	    reset_idb_holdq(idb);
	    (*idb->reset)(idb);
	    if (idb->counters.resets > 0)
		idb->counters.resets--;
	}
    }
    reset_interrupt_level(status);
}

/*
 * holdq_enqueue_fail_inline
 * Failed to find room to enqueue packet on interface's output queue.
 * Prepare to drop a packet and possibly send a quench message back to
 * the source.
 */

static inline boolean holdq_enqueue_fail_inline (
    hwidbtype *output,
    paktype *pak,
    enum HEADTAIL which,
    int qnum)
{
    elementtype *q;
    paktype *tailpak;
    leveltype status;

    output->counters.output_total_drops++; /* this is the system total  */
    output->counters.output_drops[qnum]++; /* this is for this queue only */
    if (output->status & IDB_ASYNC)
	slip_traffic.dropped++;      /* count drops for SLIP */
    if (pak->if_input != NULL)
	reg_invoke_send_quench(pak->linktype, pak);
    else {
	
	/*
	 * Locally sourced, try to remove transient packets
	 * from the queue.  Lock queues against interrupt level.
	 */
	status = raise_interrupt_level(NETS_DISABLE);
	q = (elementtype *)output->outputq[qnum].qhead;
	while (q) {
	    if (q->pakptr->if_input != NULL) {
		datagram_done(pak_unqueue(&(output->outputq[qnum]),
						 q->pakptr)); 
		break;	
	    }
	    q = q->next;
	}
	
	/*
	 * If this is a priority packet, drop the packet at the tail
	 * of the queue and enqueue this at the head of the queue.
	 */
	if (pak->flags & PAK_PRIORITY) {
	    if (!q) {
		/*
		 * Try to drop the tail packet only if we have one.
		 */
		if (output->outputq[qnum].qtail) {
		    tailpak =
			((elementtype *) output->outputq[qnum].qtail)->pakptr;
		    datagram_done(pak_unqueue(&(output->outputq[qnum]),
						     tailpak));
		}
	    }
	    if (pak_requeue(&(output->outputq[qnum]), pak)) {
		pak->flags |= PAK_DLQ;
		reset_interrupt_level(status);
		return(TRUE);
	    }
	} else if (q) {
	    
	    /* 
	     * A transient packet removed -- enqueue locally sourced
	     * packet.
	     */
	    if (which == TAIL)
		(void) pak_enqueue(&(output->outputq[qnum]), pak);
	    else
		(void) pak_requeue(&(output->outputq[qnum]), pak);
	    pak->flags |= PAK_DLQ;
	    reset_interrupt_level(status);
	    return(TRUE);
	}
	reset_interrupt_level(status);
    }
    datagram_done(pak);
    return(FALSE);
}

/*
 * holdq_enqueue_fail
 * (See inline description).
 */

boolean holdq_enqueue_fail (
    hwidbtype *output,
    paktype *pak,
    enum HEADTAIL which,
    int qnum)
{
    return(holdq_enqueue_fail_inline(output, pak, which, qnum));
}

/*
 * holdq_enqueue_fast_reserve
 * Lean interrupt level version of holdq_enqueue that reserves
 * specified space on output hold queue for packets from process
 * level
 */

boolean holdq_enqueue_fast_reserve (hwidbtype *output, paktype *pak,
				    int reserve)
{
    if (pak_enqueue_reserve(&(output->outputq[PRIORITY_NORMAL]),
			    pak, reserve)) {
	output->output_qcount++;
	return(TRUE);
    }
    output->counters.output_total_drops++; /* this is the system total  */
    output->counters.output_drops[PRIORITY_NORMAL]++; /* this is for this queue only */
    if (output->status & IDB_ASYNC)
	slip_traffic.dropped++;		/* count drops for SLIP */
    datagram_done(pak);
    return(FALSE);
}

/*
 * holdq_enqueue
 * Enqueue a packet for output according to priority.
 * We are assuming an output interface is set in pak->if_output
 */

boolean holdq_enqueue (
    hwidbtype *output,
    paktype *pak,
    enum HEADTAIL which)
{
    int value;

    if (pak->flags & PAK_PRIORITY)
	which = HEAD;
    value = PRIORITY_NORMAL;

    /*
     * Hang datagram on appropriate output queue
     */
    if (which == TAIL) {
	if (pak_enqueue(&(output->outputq[value]), pak)) {
	    pak->flags |= PAK_DLQ;
	    output->output_qcount++;
	    return(TRUE);
	}
    } else {
	if (pak_requeue(&(output->outputq[value]), pak)) {
	    pak->flags |= PAK_DLQ;
	    output->output_qcount++;
	    return(TRUE);
	}
    }
    return(holdq_enqueue_fail_inline(output, pak, which, value));
}

/*
 * holdq_dequeue
 * Dequeue the next packet from one of the hold queues
 */

paktype *holdq_dequeue (hwidbtype *idb)
{
    paktype *pak;
    ushort datagramsize_old;
	
    /*
     * Handle case where no prioritization is being done.
     */
    pak = pak_dequeue(&(idb->outputq[PRIORITY_NORMAL]));
    if (pak) {
	idb->output_qcount--;
	pak->flags &= ~PAK_DLQ;

	/*
	 * Do any compression on output
  	 * Store pre-compression size for potential credit
	 * The test of "idb->tcp_compress" should be more generic.
	 */
  	datagramsize_old = pak->datagramsize;
	if ((idb->tcp_compress || 
	     ((pak->nif_output != NULL) &&
			(pak->nif_output->compression_negotiated_enabled))))
	    pak = reg_invoke_compress_header(pak->linktype, pak);
	if (idb->comp_db)
	    pak = reg_invoke_encaps_compress(idb->enctype, pak);
  	/*
  	 * Credit traffic shaping statistics for overcharging which
  	 * results from last-minute knowledge of compression savings.
  	 * Put check beyond all compression routines to insure all
  	 * bytes savings are accounted for...
  	 */
  	traffic_shape_credit(pak, datagramsize_old);
  
	pak->oqnumber = PRIORITY_NORMAL;
    }
    return(pak);
}

/*
 * red_enqueue
 *
 * Random Early Detection drops traffic periodically in an attempt to
 * diminish congestion. We assume that normal traffic is fast switched
 * and passes the transmit queue limit; congestion events are
 * recognized when traffic exceeds TQL. Then, we buffer and forward all
 * "small" messages, which are likely to be interactive traffic or
 * session keep-alive information, or single packet DNS lookups, and
 * likewise buffer and forward "large" messages, but with some
 * probability of loss.
 *
 * Per Jacobsen & Floyd (Transactions on Networking, 1993), the
 * probability of deliberate loss is calculated using the intersection
 * of two algorithms.
 *
 * We maintain mean queue depth at enqueue. If mean queue depth <=
 * min_threshold, we enqueue all traffic. If present queue depth exceeds
 * max_threshold, we discard all traffic. If mean queue depth is beween
 * them, then the probability if discard should be linearly
 * proportional to the time since last discard and randomized by a
 * random number. We acheive that by selecting a packet number in the
 * range:
 *
 *	0 <= number of packets to skip - random() <= max queue depth
 *
 *	where 0 <= random() < max queue depth
 *
 * The key points are:
 *  -  a uniform, not bursty, discard pattern probability of discard
 *  -  proportional to mean queue depth probability of discard
 *  -  proportional to time since last discard
 *
 * This will not significantly affect the rate of RTP or other stream
 * mode traffic, as they do not back off in the face of loss; however,
 * the loss level may make the data largely useless. The effect on
 * transport traffic other than TCP is application-dependent, and may
 * or may not be useful. This algorithm is designed in the context of
 * predominately TCP networks and is most effective there.
 *
 * the following inline implements an x^23+x^5+1 cyclic feedback shift
 * register as a cheap random number generator.
 */
#define BERT_DEGREE 23
#define BERT_DEGREES (BERT_DEGREE - 1)
#define BERT_BIT     5
static uint bert_random_value = 1;
static inline uint bert_random (void)
{
    int r1, r2, r3, r4, r5;

    r1 = bert_random_value;		  /* get seed value */
    r2 = r1 << (BERT_DEGREES - BERT_BIT); /* position the x^BIT bit */
    r3 = r2 << BERT_BIT;		  /* position the x^0 bit */
    r4 = (1 << BERT_DEGREES) & (r2 ^ r3); /* select x^BIT + x^0 feedback */
    r5 = r4 | (r1 >> 1);		  /* calculate new value */
    bert_random_value = r5;		  /* save new seed */
    return r5;
}
boolean red_enqueue (hwidbtype *output, paktype *pak, enum HEADTAIL which) 
{
    red_parameters *redp = output->red_parameters;
    short precedence = 0;

    if (pak->flags & PAK_PRIORITY)
	which = HEAD;

    /*
     * Hang datagram on appropriate output queue
     */
    if (which == TAIL) {
	if (redp) {
	    if (output->outputq[PRIORITY_NORMAL].count) {
		/*
		 * calculate moving average
		 */
		redp->mean_queue_depth += 
		    RED_SCALE(redp->mean_queue_depth,
			      redp->red_shift,
			      output->outputq[PRIORITY_NORMAL].count);
	    } else {
		ulong ms_elapsed;
		ulong ms_per_message;
		ulong bits_per_message;
		ulong bits_per_ms;

		/*
		 * idle line; reduce average somewhat to account for
		 * it. Rate is not well defined by Van, so we take a
		 * simple approach.
		 */
		ms_elapsed = ELAPSED_TIME(redp->idle_time);
		bits_per_message = 8*output->maxmtu;
		bits_per_ms = output->firstsw->visible_bandwidth;
		if (bits_per_message >= bits_per_ms) {
		    ms_per_message = bits_per_message/bits_per_ms;
		    redp->mean_queue_depth -= ms_elapsed/ms_per_message;
		} else {
		    ms_per_message = 1;		/* LAN rates */
		    redp->mean_queue_depth -= ms_elapsed;
		}
		if (redp->mean_queue_depth < 0)
		    redp->mean_queue_depth = 0;
		GET_TIMESTAMP(redp->idle_time);
	    }
	
	    if (redp->mean_queue_depth < redp->min_threshold[0]) {
		redp->count = -1;
	    } else {
		ulong upper;
		ulong lower;
		ulong ratio;

		/*
		 * potentially need to drop a packet. Check precedence
		 */
		precedence = 
		    reg_invoke_determine_RED_precedence(pak->linktype, pak);
		if (redp->mean_queue_depth > redp->max_threshold)
		    goto mark_packet;
		redp->count++;
		if (redp->mean_queue_depth > redp->min_threshold[precedence]) {
		    /*
		     * policy at this precedence allows a mark
		     */
		    upper =
			redp->mean_queue_depth - redp->min_threshold[precedence];
		    lower =
			redp->max_threshold - redp->min_threshold[precedence];
		    ratio = (upper << redp->red_shift) / lower;
		    if (ratio + redp->count >
			(bert_random()&((1 << redp->red_shift) - 1)))
			goto mark_packet;
		}
	    }
	}
	if (pak_enqueue(&(output->outputq[PRIORITY_NORMAL]), pak)) {
	    pak->flags |= PAK_DLQ;
	    output->output_qcount++;
	    return(TRUE);
	}
    } else {
	if (pak_requeue(&(output->outputq[PRIORITY_NORMAL]), pak)) {
	    pak->flags |= PAK_DLQ;
	    output->output_qcount++;
	    return(TRUE);
	}
    }
  mark_packet:
    if (redp) {
	/*
	 * pak_enqueue thresholding or failure, or RED threshold failure.
	 * "Mark" packet by discarding it
	 * Then calculate a random amount of time (well, packets to pass)
	 * before pitching another.
	 */
	redp->count = 0;
	redp->packets_marked[precedence]++;
    }
    return(holdq_enqueue_fail_inline(output, pak, which, PRIORITY_NORMAL));
}

/*
 * red_dequeue
 * Dequeue the next packet from one of the hold queues
 */

paktype *red_dequeue (hwidbtype *idb)
{
    paktype *pak;
    red_parameters *redp;
    ushort datagramsize_old;
	
    /*
     * Handle case where no prioritization is being done.
     */
    pak = pak_dequeue(&(idb->outputq[PRIORITY_NORMAL]));
    if (pak) {
	idb->output_qcount--;
	pak->flags &= ~PAK_DLQ;

	/*
	 * Do any compression on output
  	 * Store pre-compression size for potential credit
	 * The test of "idb->tcp_compress" should be more generic.
	 */
  	datagramsize_old = pak->datagramsize;
	if (idb->tcp_compress)
	    pak = (paktype *) reg_invoke_compress_header(pak->linktype, pak);
	if (idb->comp_db)
	    pak = (paktype *) reg_invoke_encaps_compress(idb->enctype, pak);
  	/*
  	 * Credit traffic shaping statistics for overcharging which
  	 * results from last-minute knowledge of compression savings.
  	 * Put check beyond all compression routines to insure all
  	 * bytes savings are accounted for...
  	 */
  	traffic_shape_credit(pak, datagramsize_old);
  
	pak->oqnumber = PRIORITY_NORMAL;

	/*
	 * note idle time
	 */
	if (!idb->output_qcount) {
	    redp = idb->red_parameters;
	    if (redp)
		GET_TIMESTAMP(redp->idle_time);
	}
    }
    return(pak);
}
/*
 * red_set_precedence_levels
 */
void red_set_precedence_levels (hwidbtype *idb, short managed_to)
{
    red_parameters *redp = idb->red_parameters;
    int i;
    int difference;
    if (redp) {

	/* 
	 * holdq_command calls with a parameter that says
	 * "I only changed the upper bound"
	 */
	if (managed_to)
	    redp->min_threshold[0] = managed_to << redp->red_shift;
 	else {
 	    redp->saved_threshold = idb->outputq[PRIORITY_NORMAL].maximum;
             idb->outputq[PRIORITY_NORMAL].maximum = 0;
             redp->min_threshold[0] = 
                 (redp->saved_threshold/2) << redp->red_shift;
 	}

	/*
	 * change the upper bound
	 */
	redp->max_threshold = redp->saved_threshold << redp->red_shift;

	/*
	 * calculate the bounds for different precedence levels
	 */
	difference = redp->max_threshold - redp->min_threshold[0];
	for (i = 1; i < RED_PRECEDENCE_LEVELS; i++) {
	    redp->min_threshold[i] = redp->min_threshold[0] +
		    		     (difference * i)/(RED_PRECEDENCE_LEVELS);
	}
    }
}

/*
 * holdq_display
 * Print the current input and output queue statistics.
 */

void holdq_display (hwidbtype *idb)
{
    int			i;
    fair_queue_head	*fq;
    red_parameters      *redp;
    idb_si_counters_t	*sic;

    if (!idb)
        return;
    sic = idb_get_counter_info(idb, IDB_COUNTER_SI_CNT_PTR);
    if (idb->queue_algorithm == QUEUE_FCFS) {
	printf("\n  Queueing strategy: fifo");
	printf("\n  Output queue %d/%d, %u drops; input queue %d/%d, %u drops",
	       idb->outputq[DEFAULT_QUEUE(idb)].count,
	       idb->outputq[DEFAULT_QUEUE(idb)].maximum,
	       (idb->counters.output_total_drops
		- sic->output_total_drops),
	       idb->input_qcount, idb->input_defqcount,
	       (idb->counters.input_drops - sic->input_drops));
	return;
    }
    printf("\n  Input queue: %d/%d/%u (size/max/drops); Total output drops: %u",
	   idb->input_qcount, idb->input_defqcount,
	   (idb->counters.input_drops - sic->input_drops),
	   (idb->counters.output_total_drops - sic->output_total_drops));
    switch (idb->queue_algorithm) {
    case QUEUE_FAIR:
    case QUEUE_FAIR_DEF:
	fq = idb->fair_queue_head;
	if (NULL != fq) {
	    printf("\n  Queueing strategy: weighted fair");
	    printf("\n  Output queue: %d/%d/%u (size/threshold/drops) ",
		   idb->output_qcount,
		   fq->fq_max_enqueue,
		   (idb->counters.output_drops[PRIORITY_NORMAL] -
		    sic->output_drops[PRIORITY_NORMAL]));
	    printf("\n     Conversations  %d/%d (active/max active)",
		   fq->fq_active_conversations,
		   fq->fq_max_active_conversations); /* CSCdi39224 */

	    printf("\n     Reserved Conversations %d/%d (allocated/max allocated)",
		   fq->fq_allocated_conversations,
		   fq->fq_max_allocated_conversations); /* CSCdi39224 */

	} 
	break;
    case QUEUE_RED:
	printf("\n  Queueing strategy: random early detection(RED)");
	redp = idb->red_parameters;
	if (redp) {
	    printf("\n    mean queue depth: %d", 
		   redp->mean_queue_depth>>redp->red_shift);
	    printf("\n    drops by precedence:  %5s %10s %10s",
		   "class", "drops", "threshold");
	    for (i = 0; i < 8; i++) {
		printf("\n                          %5d %10d %10d", i,
		       redp->packets_marked[i] - redp->si_packets_marked[i],
		       redp->min_threshold[i] >> redp->red_shift);
	    }
	    printf("\n                          %5s %10d %10d", "rsvp",
		   redp->packets_marked[8] - redp->si_packets_marked[8],
		   redp->min_threshold[8] >> redp->red_shift);
	}
	break;
    case QUEUE_CUSTOM:
	printf("\n  Queueing strategy: custom-list %d", idb->priority_list);
	if (idb->custom_queuenum) {
	    printf("\n  Output queues: (queue #: size/max/drops)\n     ");
	    for (i = 0; i < PRIORITY_MAXTYPE; i++) {
		printf("%d: %d/%d/%u ", i, idb->outputq[i].count,
		       idb->outputq[i].maximum,
		       (idb->counters.output_drops[i]
			- sic->output_drops[i]));
		if (i%5 == 4)
		    printf("\n     ");
	    }
	}
	break;
    case QUEUE_PRIORITY:
	printf("\n  Queueing strategy: priority-list %d", idb->priority_list);
	printf("\n  Output queue: high %d/%d/%u, medium %d/%d/%u, normal %d/%d/%u, low %d/%d/%u",
	       idb->outputq[PRIORITY_HIGH].count,
	       idb->outputq[PRIORITY_HIGH].maximum,
	       (idb->counters.output_drops[PRIORITY_HIGH] -
		sic->output_drops[PRIORITY_HIGH]),
	       idb->outputq[PRIORITY_MEDIUM].count,
	       idb->outputq[PRIORITY_MEDIUM].maximum,
	       (idb->counters.output_drops[PRIORITY_MEDIUM] -
		sic->output_drops[PRIORITY_MEDIUM]),
	       idb->outputq[PRIORITY_NORMAL].count,
	       idb->outputq[PRIORITY_NORMAL].maximum,
	       (idb->counters.output_drops[PRIORITY_NORMAL] -
		sic->output_drops[PRIORITY_NORMAL]),
	       idb->outputq[PRIORITY_LOW].count,
	       idb->outputq[PRIORITY_LOW].maximum,
	       (idb->counters.output_drops[PRIORITY_LOW] -
		sic->output_drops[PRIORITY_LOW]));
	break;
    }
}

/*
 * traffic_shape_out
 * Queue output packet for the selected net.
 * We also timeout hung transfers.
 */

void traffic_shape_out (paktype *pak)
{
    hwidbtype *idb = pak->if_output->hwptr;	/* addr of associated idb */
    unsigned acct_proto;		/* accounting protocol type */
    leveltype status;			/* lock out interrupts */
    boolean success;
    ulong acct_bytes;

    if ((idb->state != IDBS_UP) || reg_invoke_dialer_spoofing_int(idb)) {
	datagram_done(pak);		/* it's down, flush datagram */
	return;				/* all done */
    }

    /*
     * Lock out network interrupts
     */
    status = raise_interrupt_level(NETS_DISABLE);

    /*
     * Assume the data will be enqueued and subsequently delivered.
     */
    acct_proto = pak->acct_proto;
    idb->counters.tx_frames[acct_proto]++;
    acct_bytes = pak->datagramsize;
    idb->counters.tx_bytes[acct_proto] += acct_bytes;

    /* We also have to maintain the other set of counters */
    if (idb->soutput == (soutput_t)return_nothing)
      /* That means that we don't have "soutput" routine which should
       * take care of "output" counter
       */
      idb->counters.outputs++;


    /*
     * Note: X.25 and LAPB encapsulations perform their own
     * (*soutput) call with interrupts locked out to prevent out
     * of sequence delivery. They will always return FALSE here.
     */
    success = (*idb->oqueue)(idb, pak, TAIL);	/* add to output queue */

    reset_interrupt_level(status);
    if (!success) {
	/*
	 * Output queue has filled up, or idb is set to an encapsulation
	 * that performs its own idb->soutput, or idb is a virtual idb.
	 * If output queue has really filled up, kick the soutput routine as
	 * self protection against deadlock.
	 */
	if (idb->oqueue == holdq_enqueue ||
	    idb->oqueue == priority_enqueue ||
	    idb->oqueue == red_enqueue ||
	    idb->oqueue == fair_enqueue) {
	    idb->counters.tx_frames[acct_proto]--;
	    idb->counters.tx_bytes[acct_proto] -= acct_bytes;

	    /* We also have to maintain the other set of counters */
	    if (idb->soutput == (soutput_t)return_nothing)
		/* That means that we don't have "soutput" routine which
		 * should take care of "output" counter
		 */
		idb->counters.outputs--;


	    (*(idb->soutput)) (idb);
	}
    } else {
	(*idb->soutput) (idb);	/* start another output if pending */
    }
}

/*
 * datagram_out
 * Queue output packet for the selected net.
 * We also timeout hung transfers.
 */

void datagram_out (paktype *pak)
{
    traffic_shape_t *shape;
    hwidbtype *idb = pak->if_output->hwptr;	/* addr of associated idb */
    unsigned acct_proto;		/* accounting protocol type */
    leveltype status;			/* lock out interrupts */
    boolean success;
    ulong acct_bytes;

    if ((idb->state != IDBS_UP) || reg_invoke_dialer_spoofing_int(idb)) {
	datagram_done(pak);		/* it's down, flush datagram */
	return;				/* all done */
    }

    /*
     * Lock out network interrupts
     */
    status = raise_interrupt_level(NETS_DISABLE);

    /*
     * Decrement the input hold queue count on the inbound interface.
     * N.B.: Do not use clear_if_input() since it clears pak->if_input.
     */
    if (pak->flags & PAK_INPUTQ) {
	if (pak->if_input)
	    pak->if_input->hwptr->input_qcount--;
	pak->flags &= ~PAK_INPUTQ;
    }

    /*
     * determine whether there is window available to send the message with
     * in the rate control bucket
     *
     * CSCdi77501: Remove qualified check for pak->linktype.  Check
     * prevented X.25 from using traffic shaping.  Consider more
     * specific qualification checks in future which discriminates
     * between link control and (link) data packets.  For now, shape
     * all traffic types identically...
     */
    shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
    if (shape) {
	   traffic_shape_defer_sending(shape, pak);
	   reset_interrupt_level(status);
	   return;
    }
    /*
     * Assume the data will be enqueued and subsequently delivered.
     */
    acct_proto = pak->acct_proto;
    idb->counters.tx_frames[acct_proto]++;
    acct_bytes = pak->datagramsize;
    idb->counters.tx_bytes[acct_proto] += acct_bytes;

    /* We also have to maintain the other set of counters */
    if (idb->soutput == (soutput_t)return_nothing)
      /* That means that we don't have "soutput" routine which should
       * take care of "output" counter
       */
      idb->counters.outputs++;


    /*
     * Note: X.25 and LAPB encapsulations perform their own
     * (*soutput) call with interrupts locked out to prevent out
     * of sequence delivery. They will always return FALSE here.
     */
    success = (*idb->oqueue)(idb, pak, TAIL);	/* add to output queue */

    reset_interrupt_level(status);
    if (!success) {
	/*
	 * Output queue has filled up, or idb is set to an encapsulation
	 * that performs its own idb->soutput, or idb is a virtual idb.
	 * If output queue has really filled up, kick the soutput routine as
	 * self protection against deadlock.
	 */
	if (idb->oqueue == holdq_enqueue ||
	    idb->oqueue == priority_enqueue ||
	    idb->oqueue == red_enqueue ||
	    idb->oqueue == fair_enqueue) {
	    idb->counters.tx_frames[acct_proto]--;
	    idb->counters.tx_bytes[acct_proto] -= acct_bytes;

	    /* We also have to maintain the other set of counters */
	    if (idb->soutput == (soutput_t)return_nothing)
		/* That means that we don't have "soutput" routine which
		 * should take care of "output" counter
		 */
		idb->counters.outputs--;


	    (*(idb->soutput)) (idb);
	}
    } else {
	(*idb->soutput) (idb);	/* start another output if pending */
    }
}

/*
 * physical_broadcast
 * Return TRUE/FALSE/NOTSURE as to whether the datagram is a physical broadcast
 */
fuzzy physical_broadcast (paktype *pak)
{
    hwidbtype *hwidb;

    if (!pak->if_input)
	return(FUZ_FALSE);
    hwidb = pak->if_input->hwptr;
    return( hwidb->broadcast(hwidb, pak) );
}

/*
 * supports_maxdgram
 * Return true if interface supports an adjustable maximum datagram size:
 */

boolean supports_maxdgram (hwidbtype *idb)
{
    /*
     * If this interface does not support an adjustable maximum datagram
     * size, print error message and return FALSE. Else, return TRUE:
     */
    if (idb->set_maxdgram) {
	return(TRUE);
    }
    printf("\n%% Interface %s does not support adjustable "
	   "maximum datagram size\n",
	   idb->hw_namestring);
    return(FALSE);
}


/*
 * interface_command
 *
 * Command action routine for the 'interface' command.
 *
 */

void interface_command (parseinfo *csb)
{
    idbtype *idb;

    idb = GETOBJ(idb,1);

    /* NV Generation */
    if (csb->nvgen) {

	if ((idb->hwptr->status & (IDB_DELETED | IDB_NO_NVGEN)) ||
	    (idb->subif_state == SUBIF_STATE_DELETED)) {
	    return;
	}

        /* SPECIAL CASE *******
         * If this is interface Null0, check now for 'no ip unreachable'
         * and output here, then return.  The defaults for Null0
         * generally cause a divide by zero for any interface bandwidth
         * commands.  Lots of other interface subcommands are
         * also generated, due to the incomplete initialization
         * of nullidb.
         */
        if ((idb == nullidb) && nullidb) {
            if (!nullidb->ip_unreach) {
		nv_write(TRUE, "!");
                nv_write(TRUE, "%s", csb->nv_command);
                nv_write(TRUE, " no ip unreachables");
            }
            return;
        }

	nv_write(TRUE, "!");
        nv_write(TRUE, "%s", csb->nv_command);

	/* Add the point-to-point status if this is a subinterface. */

	nv_add(is_subinterface(idb) && can_select_subif_type(idb),
	       " %s", idb->subif_link_type == SUBIF_TYPE_P2P ? 
	       "point-to-point" : "multipoint");

        /* Clear the nv_command before doing interface subcommands */
        csb->interface = idb;
        *csb->nv_command = 0;
	if (is_subinterface(idb)) {
	    csb->mode = subinterface_mode;
	} else {
	    csb->mode = interface_mode;
	}
        push_node(csb, get_mode_nv(csb));
        return;
    }

    /* Check that we've been given a valid interface.  If not, then
     * complain and return.
     */
    if (idb == NULL) {
	if (GETOBJ(int,5)) {	/* point-to-point or multipoint */
	    printf("\n%% Cannot specify link type on this interface");
	    return;
	}
	if (GETOBJ(int,3) == TRUE) {
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    return;
	} else {
	    printf("\n%% No interface specified for interface_command");
	    return;
	}
    }

    if (idb && idb->hwptr &&
	!reg_invoke_interface_command_ok(idb->hwptr->type, csb)) {
	return;
    }

    /*
     * Subinterface link type processing.  If one has been specified and it
     * isn't legal, issue a warning.
     */
    if (GETOBJ(int,5)) {
	if (!can_select_subif_type(idb)) {
	    printf("%% Warning:  cannot select link type on this interface");
	} else {
	    /*
	     * If one has already been specified and this doesn't match,
	     * issue a warning.
	     */
	    if (idb->subif_link_type) {
		if (idb->subif_link_type != GETOBJ(int,5)) {
		    printf("%% Warning:  cannot change link type");
		}
	    } else {

		/* Not yet set.  Set it. */

		idb->subif_link_type = GETOBJ(int,5);

		/*
		 * Notify CDP to reset default value for FR P2P.
		 */
		if (is_frame_relay(idb->hwptr) &&
		    idb->subif_link_type == SUBIF_TYPE_P2P) {
		    reg_invoke_cdp_reset(idb->hwptr, idb);
		}

	    }
	}
    } else {

	/* Not selected.  If not yet set, set the default. */

	if (!idb->subif_link_type) {
	    set_subif_link_type(idb);
	}
    }

    csb->interface = idb;
    if (csb->sense) {
	/* Bring up the deleted interface */
	if (csb->interface->hwptr->status & IDB_DELETED)
	    shutdown_coming_up(csb->interface->hwptr);
	/* Set the top of the parse tree to the interface command set */
	csb->interface->hwptr->status &= ~IDB_DELETED;
	if (is_subinterface(csb->interface)) {
	    if (csb->interface->subif_state == SUBIF_STATE_DELETED)
		shutdown_subif(csb->interface, SUBIF_STATE_UP);
	}
	if ((idb == nullidb) && nullidb) {
	    set_mode_byname(&csb->mode, "null-interface", MODE_VERBOSE);
	} else {
	    if (is_subinterface(csb->interface)) {
		set_mode_byname(&csb->mode, "subinterface", MODE_VERBOSE);
	    } else {
		csb->mode = interface_mode;
	    }
	}
	if (is_subinterface(csb->interface)) {
	    if (csb->interface->subif_state == SUBIF_STATE_DELETED)
		shutdown_subif(csb->interface, SUBIF_STATE_UP);
		reg_invoke_ifmib_add_subif(idb->hwptr->enctype,idb);
	}
	if (csb->flags & CONFIG_HTTP) {
	    char *new_uri = string_getnext();
	    char *old_uri = csb->http_state->uri;
	    char *cp;
	    char *name;

	    /* Setup new URI with parser mode */
	    sprintf(new_uri, "/%s/", get_mode_name(csb->mode));
	    name = csb->interface->namestring;
	    cp = new_uri;

	    /* Set pointer to end of URI */
	    while (*cp) {
		cp++;
	    }

	    /* Append interface namestring to URI, escaping '/' */
	    while (*name) {
		if (*name == '/') {
		    *cp++ = '\\';
		    *cp = '/';
		} else {
		    *cp = *name;
		}
		cp++;
		name++;
	    }
	    *cp = '\0';

	    /* Call parser and generate help for new URI */
	    csb->http_state->uri = new_uri;
	    reg_invoke_http_parser(csb->http_state, csb->mode, FALSE);

	    /* Put back old URI */
	    csb->http_state->uri = old_uri;
	}
    } else {	
	if (!delete_interface(csb->interface)) {
	    printf("\n%% Removal of physical interfaces is not permitted");
	}
	exit_config_submode(csb);
    }
}


/*
 * network_fixup
 * If the datalink encapsulation has changed, clean up the network
 * layer. Called by idb->deencap routines and from the encapsulation_command.
 * We need this for CLNS which uses knowledge of SNPA and protocol addresses
 * in the network layer.  We also need this to flush the various fast switching
 * and address resolution caches.
 */

static void network_fixup (hwidbtype *hwidb, int old_encap)
{
    idbtype *swidb;
    tinybool fastflag;

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	reg_invoke_encap_fixup(swidb, old_encap);
    }

    /*
     * We have added a cell to  the idb called phoney_rxtype.  This is to
     * get around the micro code bug where it can report a packet can be
     * fastswitched when indeed we don't support it for a particular
     * encapsulation.
     */
    fastflag = FALSE;

    /*
     * This list should be a superset of all encapsulation types of all
     * routing protocols supported in the fast switching path.
     */
    if ((hwidb->status & IDB_ETHER) && ((hwidb->enctype == ET_ARPA) ||
					(hwidb->enctype == ET_NOVELL_ETHER)))  
	fastflag = TRUE;
    if ((hwidb->status & IDB_SERIAL) && ((hwidb->enctype == ET_HDLC) ||
				       (is_ppp(hwidb)) ||
				       (is_frame_relay(hwidb)) ||
				       (is_atm_dxi(hwidb)) ||
				       (is_smds(hwidb))))
	fastflag = TRUE;
    if ((hwidb->status & IDB_FDDI) && (hwidb->enctype == ET_SNAP))
	fastflag = TRUE;

    hwidb->phoney_rxtype = !fastflag;
}

/*
 * encapsulation_command
 *
 * Set encapsulation of an interface.
 *
 * Note: encapsulation vectors are not on the software IDB therefore for
 *       802.10 SDE and ISL encapsulation on subinterfaces we need to
 *       make rather messy exceptions.
 */

void encapsulation_command (parseinfo *csb)
{
    encapstype encap;
    hwidbtype *idb;
    idbtype *swidb;
    
    if (!csb->interface)
	return;
    idb = csb->interface->hwptr;
    if (!idb)
	return;
    encap = idb->enctype_cfg;

    /* 
     * Defining an encapsulation flavour only allowed for serial interfaces
     * and subinterfaces configured for 802.10 or ISL Virtual LANs.
     */


    if (!(idb->status & IDB_SERIAL) && 
        (is_vlan_capable(csb->interface) == FALSE))
        return;

    if (csb->nvgen) {

	if  ((GETOBJ(int, 1) == ET_SDE) && 
            (csb->interface->sde_said_db != NULL)) {

            reg_invoke_encaps_nvram_write(ET_SDE, csb);
        }

        else if  ((GETOBJ(int, 1) == ET_ISL) &&
                 (csb->interface->isl_vlan != NULL)) {

            reg_invoke_encaps_nvram_write(ET_ISL, csb);
        }

        else if ((encap == GETOBJ(int,1)) && 
                 ((encap != idb->enctype_def) ||
                  ((encap == ET_STUN) || (encap == ET_BSTUN)))) {

	    reg_invoke_encaps_nvram_write(encap, csb);
	}
	return;
    }

    if (!csb->sense) {

	if (GETOBJ(int, 1) == ET_SDE)
            reg_invoke_deencap_802_10_sde(csb->interface, csb->sense);

	else if (GETOBJ(int, 1) == ET_ISL)
            reg_invoke_deencap_isl_vlan(csb->interface, csb->sense);


	GETOBJ(int,1) = idb->enctype_def;
    }

    if ((idb->enctype == GETOBJ(int,1)) &&
	(idb->enctype != ET_FRAME_RELAY) &&
	(idb->enctype != ET_X25) &&
	(idb->enctype != ET_LAPB))
	return;

    if (GETOBJ(int, 1) == ET_SDE) {
        reg_invoke_deencap_802_10_sde(csb->interface, csb->sense);
        reg_invoke_set_802_10_sde_encap(csb);
    }

    else if (GETOBJ(int, 1) == ET_ISL) {
        reg_invoke_deencap_isl_vlan(csb->interface, csb->sense);
        reg_invoke_encap_isl_vlan(csb);
    }


    else {
	if (reg_invoke_fr_encaps_check(GETOBJ(int,1), csb, idb))
	    return;
	if (idb->deencap) {
	    (*idb->deencap)(idb);
	    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
		if (is_subinterface(swidb)) {
		    if (swidb->snmp_if_index)  {
			reg_invoke_ifmib_delete_subif(swidb->hwptr->enctype,
						      swidb);
		    }
		}
	    }    
	}
        if (idb->setencap) {
            (*idb->setencap)(csb);
	    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
		if (is_subinterface(swidb)) {
		    reg_invoke_ifmib_add_subif(swidb->hwptr->enctype,swidb);
		}
	    }
	}
    }

    if (idb->enctype == ET_NULL) {
        idb->enctype = encap;
    }

    if (encap != idb->enctype) {

       /* It just was BSTUN - set default back to full duplex */
        if (encap == ET_BSTUN) {
            csb->which = CFG_HDX_RESET_PARAMS;
            csb->sense = FALSE;
            reg_invoke_media_half_or_full_duplex_cmd(idb->type, csb);
        }

        /* It is now BSTUN - set default to half-duplex */
        if (idb->enctype == ET_BSTUN) {
            csb->which = CFG_HDX_RESET_PARAMS;
            csb->sense = TRUE;
            reg_invoke_media_half_or_full_duplex_cmd(idb->type, csb);
        }

	idb->enctype_cfg = idb->enctype; /* Save new configured default */

	if (!idb_subif_allowed_here(idb))
	    FOR_ALL_SWIDBS_ON_HW(idb, swidb)
	        if (is_subinterface(swidb)) {
	            delete_interface(swidb);
		}

	reg_invoke_bump_cache_version(idb, NULL, FALSE);
	network_fixup(idb, encap);
	FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	    reg_invoke_route_adjust_msg(swidb);
	    reg_invoke_route_adjust(swidb);
	    reg_invoke_ar_cache_clear(swidb);
	}
	reg_invoke_fast_setup(idb);

	/*
	 * Notify CDP to reset the default value.
	 */
	reg_invoke_cdp_reset(idb, NULL);
    }
}

/*
 * transmitter_command
 * set the transmit delay on an interface
 */

void transmitter_command (parseinfo *csb)
{
    hwidbtype *idb;
    int	    ticks;

    idb = csb->interface->hwptr;
    if (idb == NULL) return;
    
    if (csb->nvgen) {
	if (idb->xmitdelay != idb->xmitdelay_default) {
	    nv_write(TRUE, "%s %d", csb->nv_command, idb->xmitdelay);
	}
	return;
    }

    if (csb->sense)
	ticks = GETOBJ(int,1);
    else
	ticks = idb->xmitdelay_default;

    reg_invoke_media_transmit_delay(idb->type, idb, ticks);
}


/*
 * holdqueue_command
 * Change the hold queue of an interface.
 */

void holdqueue_command (parseinfo *csb)
{
    int number;
    hwidbtype *idb;

    number = GETOBJ(int,1);
    idb = csb->interface->hwptr;

    if (reg_invoke_dialer_huntgroup_member(idb)) {
	if (!(csb->nvgen))
	    printf("\nCannot change hold-queue values of "
		   "hunt group member");
	return;
    }
    
    if (csb->nvgen) {
	nv_write((idb->input_defqcount != INPUTQ_DEFAULT),
		 "%s %d in", csb->nv_command, idb->input_defqcount);
	if (! idb->priority_list) {
	    nv_write((idb->outputq[DEFAULT_QUEUE(idb)].maximum != 
		     idb->output_defqcount[DEFAULT_QUEUE(idb)]),
		     "%s %d out", csb->nv_command,
		     idb->outputq[DEFAULT_QUEUE(idb)].maximum);
	}
	return;
    }

    switch (GETOBJ(int,2)) {
      case HOLDQUEUE_OUT:
 	   if (csb->sense) {
	       if (idb->priority_list) {
		   printf("\nCannot change hold-queue out for non FCFS queue");
		   return;
	       }
	       idb->holdqueue_out = number;
	       idb->outputq[DEFAULT_QUEUE(idb)].maximum =  number;
 	   } else {
 		  idb->holdqueue_out = 0;
 		  idb->outputq[DEFAULT_QUEUE(idb)].maximum =
 		       idb->output_defqcount[DEFAULT_QUEUE(idb)];
 	   }
	   red_set_precedence_levels(idb, 0);
	   break;
      case HOLDQUEUE_IN:
	idb->input_defqcount = csb->sense ? number : INPUTQ_DEFAULT;
	break;
      default:
	bad_parser_subcommand(csb, GETOBJ(int,2));
	break;
    }
    reg_invoke_interface_cfg_modified(idb);
}

/*
 * tx_queue_limit_command
 */

void tx_queue_limit_command (parseinfo *csb)
{
    int number;
    hwidbtype *idb;

    number = GETOBJ(int,1);
    idb = csb->interface->hwptr;
    
    if (csb->nvgen) {
	nv_write(idb->mci_txcount_enabled &&
		 (idb->mci_txcount != idb->mci_txcount_default),
                 "%s %d", csb->nv_command, idb->mci_txcount);
	return;
    }
    
    if (idb->status & (IDB_MCI | IDB_CBUS)) {
	idb->mci_txcount = csb->sense ? number : idb->mci_txcount_default;
	idb->mci_txcount_enabled = csb->sense ? TRUE: FALSE;
	(*idb->reset)(idb);
	if (idb->counters.resets > 0)
	  idb->counters.resets--;
	reg_invoke_set_cdb_txcount(idb);
    } else {
	printf("\ntx queue limit command is only supported for MCI/SCI/CBUS.");
    }
}

/*
 * keepalive_command
 * Enable or disable keepalive processing.
 * 	"keepalive"           enable keepalive, use default period.
 *	"no keepalive"        disable keepalive, reset to default period.
 *      "keepalive <period>"  enable keepalive, set new period.
 */

void keepalive_command (parseinfo *csb)
{
    int ix;
    short idb_def_keepperiod;
    hwidbtype *tempidb;
    hwidbtype *hwidb = csb->interface->hwptr;
    idbtype *swidb = csb->interface;

    /*
     * get encapsulation/type specific default keepalive period
     */
    idb_def_keepperiod = KEEPALIVE_DEFAULT;
    reg_invoke_idb_get_keep_default(hwidb, &idb_def_keepperiod);
    if (csb->nvgen) {
	if (hwidb->nokeepalive) {
	    if (idb_def_keepperiod) {
		nv_write(TRUE , "no %s", csb->nv_command);
	    }
	} else {
	    nv_write((idb_def_keepperiod != hwidb->keep_period),
		     "%s %d", csb->nv_command, hwidb->keep_period);
	}
	return;
    }
    if (csb->set_to_default) {
	csb->sense = TRUE;	/* Simulate "keepalive <default>" */
	GETOBJ(int,1) = idb_def_keepperiod;
    } else if (GETOBJ(int,1) == 0) {
	csb->sense = FALSE;	/* Simulate "no keepalive" */
	GETOBJ(int,1) = idb_def_keepperiod;
    }
    hwidb->keep_period = GETOBJ(int,1);
    if (GETOBJ(int,1) < hwidb->keep_count)
	hwidb->keep_count = GETOBJ(int,1);
    hwidb->nokeepalive = !csb->sense;
    reg_invoke_encaps_set_protocol_attr (hwidb->enctype, swidb);
    hwidb->inloopback = FALSE;

    /*
     * If this is an ISDN interface, propogate the changes to the B channel
     * idb's.
     */
    if (is_isdn(hwidb) && !ISBRILEASED(hwidb)) {
	if (IS_BRI(hwidb)) {
	    tempidb = hwidb->isdn_info->b1_chan_idb;
	    tempidb->keep_period = hwidb->keep_period;
	    tempidb->nokeepalive = hwidb->nokeepalive;
	    tempidb->inloopback = hwidb->inloopback;
	    if (GETOBJ(int,1) < tempidb->keep_count) {
		tempidb->keep_count = GETOBJ(int,1);
	    }
	    tempidb = hwidb->isdn_info->b2_chan_idb;
	    tempidb->keep_period = hwidb->keep_period;
	    tempidb->nokeepalive = hwidb->nokeepalive;
	    tempidb->inloopback = hwidb->inloopback;
	    if (GETOBJ(int,1) < tempidb->keep_count) {
		tempidb->keep_count = GETOBJ(int,1);
	    }
	} else {
	    /*
	     *  PRI rate, propogate the changes to the B channel
	     * idb's.
	     */
	    for (ix = 0; ix <= DSX1_MAX_CHAN; ix++) {
		tempidb = hwidb->isdn_info->b_chan_idb[ix];
		if (!tempidb)
		    continue;
		tempidb->keep_period = hwidb->keep_period;
		tempidb->nokeepalive = hwidb->nokeepalive;
		tempidb->inloopback = hwidb->inloopback;
		if (GETOBJ(int,1) < tempidb->keep_count) {
		    tempidb->keep_count = GETOBJ(int,1);
		}
	    }
	}
    }

    /*
     * Do encapsulation specific actions to cope with keepalive changes
     */
    reg_invoke_keepalive_changed(hwidb->enctype,hwidb);
}


/*
 * loopback_command
 * Set loopback state
 */
void loopback_command (parseinfo *csb)
{
    hwidbtype *idb;

    idb = csb->interface->hwptr;

    if (csb->nvgen) {
	if (idb->status & IDB_POS) {
	    switch (GETOBJ(int,1)) {
	      case LOOPBACK_INTERNAL:
		nv_write(idb->ext_loop_type == POSIP_LOOP_INTERNAL,
			 csb->nv_command);
		break;
	      case LOOPBACK_EXTERNAL:
		nv_write(idb->ext_loop_type == POSIP_LOOP_EXTERNAL,
			 csb->nv_command);
		break;
	    }
	} else if (idb->status & IDB_HSSI) {
	    switch (GETOBJ(int,1)) {
	      case LOOPBACK_APPLIQUE:
		nv_write(idb->loopback, csb->nv_command);
		break;
	      case LOOPBACK_DTE:
		nv_write(idb->ext_loop_type == HSSI_LOOP_DTE, csb->nv_command);
		break;
	      case LOOPBACK_LINE:
		nv_write(idb->ext_loop_type == HSSI_LOOP_LINE,
			 csb->nv_command);
		break;
	      case LOOPBACK_REMOTE:
		nv_write(idb->ext_loop_type == HSSI_LOOP_REMOTE,
			 csb->nv_command);
		break;
	      default:
		/* Ignore this for NV generation. */
		break;
	    }
	} else if ((idb->type == IDBTYPE_T1) ||
	           (idb->type == IDBTYPE_E1)) {
	    switch (GETOBJ(int,1)) {
	    case LOOPBACK_LOCAL:
		nv_write(idb->loopback, csb->nv_command);
		break;
	    case LOOPBACK_REMOTE:
		nv_write(idb->ext_loop_type == DSX1_LOOP_REMOTE,
			 csb->nv_command);
		break;
	    default:
		break;
	    }
	} else if (idb->type == IDBTYPE_FEIP) {
	    switch (GETOBJ(int,1)) {
	    case LOOPBACK_DEFAULT:
		nv_write(idb->ext_loop_type == FEIP_LOOP_MOTOROLA, 
			 csb->nv_command);
		break;
	    case LOOPBACK_MOTOROLA:
		/* No nvgen */
		break;
	    case LOOPBACK_DEC:
		nv_write(idb->ext_loop_type == FEIP_LOOP_DEC,
			 csb->nv_command);
		break;
	    case LOOPBACK_MICRO_LINEAR:
		nv_write(idb->ext_loop_type == FEIP_LOOP_MICRO_LINEAR,
			 csb->nv_command);
		break;
	    default:
		break;
	    }
	} else if (is_atm(idb)) {
	    switch (GETOBJ(int,1)) {
	    case LOOPBACK_LINE:
		nv_write(idb->ext_loop_type == ATM_LOOP_LINE, csb->nv_command);
		break;
	    case LOOPBACK_PAYLOAD:
		nv_write(idb->ext_loop_type == ATM_LOOP_PAYL, csb->nv_command);
		break;
	    case LOOPBACK_CELL:
		nv_write(idb->ext_loop_type == ATM_LOOP_CELL, csb->nv_command);
		break;
	    case LOOPBACK_APPLIQUE:
		nv_write(idb->ext_loop_type == ATM_LOOP_DIAG, csb->nv_command);
		break;
	    default:
		break;
	    }
        } else if (is_g703(idb)) {
		switch (GETOBJ(int,1)) {
                case LOOPBACK_DEFAULT:
                    nv_write(idb->loopback, csb->nv_command);
                    break;
		case LOOPBACK_LINE:
		    nv_write(idb->far_end_looped, csb->nv_command);
		    break;
                default:
                    break;
		}
	} else {
	    nv_write(idb->loopback, csb->nv_command);
	}
	return;
    }

    if (idb->type == IDBTYPE_FEIP) {
	if(csb->sense) {
	    switch (GETOBJ(int,1)) {
	    case LOOPBACK_MOTOROLA:
	    case LOOPBACK_DEFAULT:
		idb->loopback = TRUE;
		idb->ext_loop_type = FEIP_LOOP_MOTOROLA;
		break;
	    case LOOPBACK_DEC:
		idb->loopback = TRUE;
		idb->ext_loop_type = FEIP_LOOP_DEC;
		break;
	    case LOOPBACK_MICRO_LINEAR:
		idb->loopback = TRUE;
		idb->ext_loop_type = FEIP_LOOP_MICRO_LINEAR;
		break;
	    default:
		bad_parser_subcommand(csb, csb->which);
		break;
	    }
	} else {
	    idb->loopback = FALSE;
	    idb->ext_loop_type = FEIP_LOOP_NONE;
	}
	(*idb->reset)(idb);
	return;
    }
	    
    if (is_atm(idb)) {
	if(csb->sense) {
	    switch (GETOBJ(int,1)) {
	    case LOOPBACK_DEFAULT:
		idb->loopback = TRUE;
		idb->ext_loop_type = ATM_LOOP_NONE;
		break;
	    case LOOPBACK_LINE:
		idb->loopback = TRUE;
		idb->ext_loop_type = ATM_LOOP_LINE;
		break;
	    case LOOPBACK_PAYLOAD:
		idb->loopback = TRUE;
		idb->ext_loop_type = ATM_LOOP_PAYL;
		break;
	    case LOOPBACK_CELL:
		idb->loopback = TRUE;
		idb->ext_loop_type = ATM_LOOP_CELL;
		break;
	    case LOOPBACK_APPLIQUE:
		idb->loopback = TRUE;
		idb->ext_loop_type = ATM_LOOP_DIAG;
		break;
	    case LOOPBACK_TEST:
		idb->loopback = TRUE;
		idb->ext_loop_type = ATM_LOOP_TEST;
		break;
	    default:
		bad_parser_subcommand(csb, csb->which);
		break;
	    }
	} else {
	    idb->loopback = FALSE;
	    idb->ext_loop_type = ATM_LOOP_NONE;
	}
	(*idb->reset)(idb);
	return;
    }

    if ((idb->type == IDBTYPE_T1) ||
	(idb->type == IDBTYPE_E1)) {
	/*
	 * We're going to use 3 idb fields for channelized t1/e1 loopbacks
	 * These are:
	 * loopback - TRUE or FALSE for local loopback
	 * ext_loop_type - DSX1_LOOP_NONE or DSX1_LOOP_REMOTE, this is
	 *                 how the user has configured it.
	 * far_end_looped - TRUE or FALSE if the remote end is looped or not.
	 *
	 * ext_loop_type and far_end_looped combine as follows:
	 * DSX1_LOOP_REMOTE  FALSE - remote loop request pending
	 * DSX1_LOOP_REMOTE  TRUE  - remote loop established
	 * DSX1_LOOP_NONE    TRUE  - remote loop down request pending
	 * DSX1_LOOP_NONE    FALSE - remote loop down
	 */
	if (csb->sense) {
	    switch (GETOBJ(int,1)) {
	    case LOOPBACK_LOCAL:
	    case LOOPBACK_DEFAULT:
		if ((idb->ext_loop_type == DSX1_LOOP_REMOTE) ||
		    (idb->far_end_looped)) {
		    printf("\nMust remove remote loopback before local loop");
		    return;
		}
		idb->loopback = TRUE;
		break;
	    case LOOPBACK_REMOTE:
                if (idb->loopback) {
                    printf("\nMust remove local loopback before remote loop");
                    return;
                }
                if (idb->state != IDBS_UP) {
                    printf("\nInterface must be up to perform remote loop");
                    return;
                }
                if ((idb->ext_loop_type == DSX1_LOOP_NONE) &&
                    (idb->far_end_looped == TRUE)) {
                    printf("\nMust wait until remote loop down process "
                           "completes before remote loop up");
                    return;
                }
                if (idb->ext_loop_type == DSX1_LOOP_REMOTE) {
                    /* Don't start the process twice */
                    return;
                }
                if (idb->nokeepalive == FALSE) {
                    printf("\nIt is recommended to disable keepalives when "
                           "running remote loopback");
                }
		/*
		 * Indicate we have a remote loop configured, but do not
		 * have remote loop.  idb->reset will start sending it
		 */
		idb->ext_loop_type = DSX1_LOOP_REMOTE;
		idb->far_end_looped = FALSE;
		break;
	    default:
		bad_parser_subcommand(csb, csb->which);
		break;
	    }
	} else {
	    if ((idb->ext_loop_type == DSX1_LOOP_REMOTE) &&
		(idb->far_end_looped == FALSE)) {
                /*
                 * If interface is down - allow the clearing of the remote
                 * loopback state
                 */
                if (idb->state != IDBS_UP) {
                    idb->ext_loop_type = DSX1_LOOP_NONE;
                    idb->far_end_looped = FALSE;
                    return;
                }
		printf("\nMust wait until remote loop process complete"
		       " - Remote Loopback down not sent");
		return;
	    }
	    if ((idb->loopback == FALSE) &&
		(idb->ext_loop_type == DSX1_LOOP_NONE) &&
		(idb->far_end_looped == FALSE)) {
		/* Don't do it if we don't have any loopbacks */
		printf("\nNo loopback to remove");
		return;
	    }
	    idb->loopback = FALSE;
	    idb->ext_loop_type = DSX1_LOOP_NONE;
	    /*
	     * If we have a remote loop established, far_end_looped will
	     * be TRUE here.  That in itself is an indication to start
	     * the remote loopback down process
	     */
	}
	(*idb->reset)(idb);
	return;
    }

    if (is_g703(idb)) {
        if (csb->sense) {
            switch (GETOBJ(int,1)) {
            case LOOPBACK_DEFAULT:
                if (idb->far_end_looped) {
                    printf("\nMust remove line loopback before local loop");
                    return;
                }
                idb->loopback = TRUE;
                break;
            case LOOPBACK_LINE:
                if (idb->loopback) {
                    printf("\nMust remove local loopback before line loop");
                    return;
                }
                idb->far_end_looped = TRUE;
                break;
            default:
                bad_parser_subcommand(csb, csb->which);
                break;
            }
        } else {
            idb->loopback = FALSE;
            idb->far_end_looped = FALSE;
        }
        (*idb->reset)(idb);
        return;
    }

    if (GETOBJ(int,1) == LOOPBACK_DEFAULT) {
	idb->loopback = csb->sense;
	if (idb->status & IDB_POS)
	    idb->ext_loop_type = 0;
	else if (idb->status & IDB_HSSI)
	    idb->ext_loop_type = HSSI_LOOP_OFF;
	(*(idb->reset))(idb);
	return;
    }

    if (idb->status & IDB_POS) {
	if (csb->sense) {
	    switch (GETOBJ(int,1)) {
	    case LOOPBACK_INTERNAL:
		idb->loopback = TRUE;
		idb->ext_loop_type = POSIP_LOOP_INTERNAL;
		break;
	    case LOOPBACK_EXTERNAL:
		idb->loopback = TRUE;
		idb->ext_loop_type = POSIP_LOOP_EXTERNAL;
		break;
	    default:
		bad_parser_subcommand(csb, csb->which);
		break;
	    }
	} else {
	    idb->loopback = FALSE;
	    idb->ext_loop_type = 0;
	}
	(*(idb->reset))(idb);
	return;
    }

    if (!(idb->status & IDB_HSSI)) {
	printf("\nIllegal loopback command - %s.", csb->line);
	printf("\n  Not an Hssi interface.");
	return;
    }
    if (csb->sense) {
	switch (GETOBJ(int,1)) {
	  case LOOPBACK_APPLIQUE:
	    idb->loopback = csb->sense;
	    idb->ext_loop_type = HSSI_LOOP_OFF;
	    break;
	  case LOOPBACK_DTE:
	    idb->loopback = FALSE;
	    idb->ext_loop_type = HSSI_LOOP_DTE;
	    break;
	  case LOOPBACK_LINE:
	    idb->loopback = FALSE;
	    idb->ext_loop_type = HSSI_LOOP_LINE;
	    break;
	  case LOOPBACK_REMOTE:
	    idb->loopback = FALSE;
	    idb->ext_loop_type = HSSI_LOOP_REMOTE;
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
    } else {
	idb->loopback = FALSE;
	idb->ext_loop_type = HSSI_LOOP_OFF;
    }
    (*(idb->reset))(idb);
}

/* 
 * set_max_interface_mtu
 *
 * Scan through the sub-interfaces and figure out the maximum MTU configured
 * in the interface. Also, figure out if autonomous/silicon switching has
 * to be turned off.
 */
void set_max_interface_mtu(idbtype *swidb, long sub_delta)
{
    long new_maxmtu, delta;
    idbtype *idb;
    hwidbtype *hwidb;

    hwidb = swidb->hwptr;
    delta = new_maxmtu =  0;
	
    /*
     *  set the maxmtu on hwidb based on 
     * 	the sub_mtu configured on all active sub-interfaces.
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	if (interface_up_simple(idb) || idb->user_configured_mtu ) {
	    if (idb->sub_mtu >= new_maxmtu ) {
		new_maxmtu = idb->sub_mtu;
	    }
	}
    }
    
    if (new_maxmtu) {
	delta = new_maxmtu - hwidb->maxmtu;
	hwidb->maxmtu = new_maxmtu;
    } 
	  
    /*
     * Notify all protocols of irrespective of MAX MTU change. 
     * This way sub-interface specific MTUs can be
     * adjusted, if necessary.
     * Initially protocols come up with a def, when sub-interfaces are created
     * e.g. When you configure a lane client, max MTU may not change 
     * but sub-if mtu might  change. So, it is better to notify now.
     */
    reg_invoke_media_change_mtu(hwidb, swidb, sub_delta);

    if (delta != 0) {

	/*
	 * Invalidate the caches.
	 */
	reg_invoke_bump_cache_version(hwidb, swidb, FALSE);

	(*hwidb->set_maxdgram)(hwidb, hwidb->buffersize + delta,
			       hwidb->max_buffer_size + delta);
    }
    if_has_multiple_mtu(hwidb);
}	

/*
 * if_has_multiple_mtu
 *
 * Used to determine, if the interface has differrent MTUs configured on
 * the sub-interfaces. Used to turn off Silicon switching for IPX.
 */
int if_has_multiple_mtu (hwidbtype *hwidb)
{
    idbtype *idb;
    

    FOR_ALL_SWIDBS_ON_HW(hwidb,idb) {
	if (interface_up_simple(idb) && hwidb->maxmtu != idb->sub_mtu) {
	     if (hwidb->novell_routecache & NOVELL_RCACHE_SSE) {
		 hwidb->novell_routecache &= ~NOVELL_RCACHE_SSE;
		 printf("\n%%ATM interface has multiple MTUs");
		 printf("\n Turning off SSE switching for IPX");
	     }
	    return (TRUE);
	}
    }
    return (FALSE);
}

/*
 * if_change_mtu
 * do the work to set the max mtu for an interface
 */
boolean if_change_mtu (idbtype *swidb, long mtu)
{
    hwidbtype *hwidb;
    idbtype *idb;
    long delta;

    hwidb = swidb->hwptr;
    if (is_atm(hwidb)) {
	/*
	 * If sub-interface specific MTU is being changed, then take
	 * note of that here and pass it to set_max_interface_mtu
	 */
	delta = mtu - swidb->sub_mtu;
	if (delta == 0) {
	    return (FALSE);
	}
	swidb->sub_mtu = mtu;
	set_max_interface_mtu(swidb, delta);
	return (TRUE);
    } 

    /*
     * We allow sub-interface based MTU setting only for ATM. 
     */
    if (is_subinterface(swidb)) {
	return (FALSE);
    }

    delta = mtu - hwidb->maxmtu;

    if (delta == 0) {		/* no change, silently return */
	return (FALSE);
    }

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	idb->sub_mtu = mtu;  /* keep it consistent with maxmtu */
    }
    hwidb->maxmtu = mtu;

    /*
     * Notify all protocols of a MTU change.
     */
    reg_invoke_media_change_mtu(hwidb, swidb, delta);

    /*
     * Invalidate the caches.
     */
    reg_invoke_bump_cache_version(hwidb, NULL, FALSE);

    /*
     * PPP interfaces must always be in receive for at least 1500 bytes
     * as specified in RFC 1661.
     */
    if (is_ppp(hwidb) && (mtu < DEFMRU)) {
	delta += (DEFMRU - mtu);
    }

    (*hwidb->set_maxdgram)(hwidb, hwidb->buffersize + delta,
			   hwidb->max_buffer_size + delta);
    return (TRUE);
}
/*
 * max_mtu_command
 * Change the maximum MTU of an interface.
 */

void max_mtu_command (parseinfo *csb)
{
    ushort mtu;
    hwidbtype *hwidb;
    idbtype *swidb, *firstsw;
    long delta;
    boolean sav_user_configured;

    swidb = csb->interface;
    hwidb = swidb->hwptr;
    firstsw = hwidb->firstsw;

    if (csb->nvgen) {
	if (is_atm(hwidb) ) {
	    nv_write(swidb->user_configured_mtu,"%s %d",
		     csb->nv_command, swidb->sub_mtu);
	} else {
	    nv_write((hwidb->maxmtu != hwidb->maxmtu_def),
                     "%s %d", csb->nv_command, hwidb->maxmtu);
        }
        return;
    }

    if (!(hwidb->status & (IDB_SERIAL | IDB_TUNNEL | IDB_ATM | IDB_BVI)) &&
	(hwidb->type != IDBTYPE_TMS380) && !(IS_VIP4R(hwidb))
	&& (hwidb->type != IDBTYPE_IBM2692)) {
	printf("\n%% Interface %s does not support user settable mtu.",
	       hwidb->hw_namestring);
	return;
    }

    if (!supports_maxdgram(hwidb)) {
	return;
    }

    sav_user_configured = swidb->user_configured_mtu;
    if (csb->sense) {
	mtu = GETOBJ(int,1);
	swidb->user_configured_mtu = TRUE;
    } else {
	swidb->user_configured_mtu = FALSE;
	mtu = hwidb->maxmtu_def;
    }

    if (is_atm(hwidb)) {

	/*
	 * If LANE is configured on this interface,
	 * make sure that the MTU is valid before proceeding.
	 */
	if (if_lane_exist(swidb)) {
	    if (csb->sense) {
		if (!reg_invoke_is_valid_lane_mtu(swidb, mtu)) {
		    printf("%% Invalid LANE MTU %d.", mtu);
		    printf("\n  Valid sizes are %d, %d and %d for Ethernet "
			   "and %d and %d for TR.",
			   LANE_MTU_ETHER_1516, LANE_MTU_ETHER_4544,
			   LANE_MTU_ETHER_9234, LANE_MTU_TR_4544, 
			   LANE_MTU_TR_9234);	
		    swidb->user_configured_mtu = sav_user_configured;
		    return;
		}
	    } else {
		/*
		 * Use the default mtu size 
		 * if lane is running on the sub-interface
		 */
		mtu = reg_invoke_lane_default_mtu(swidb);
	    }
	}
	delta = mtu - swidb->sub_mtu;
    } else {
	/*
	 * We allow sub-interface based MTU setting only for ATM. 
         */
	if (is_subinterface(swidb)) {
	    printf("\n%% setting mtu on sub-interface allowed for ATM only");
	    swidb->user_configured_mtu = sav_user_configured;
	    return;
	}
	delta = mtu - hwidb->maxmtu;
    }

    if (mtu < 64 || !pak_pool_find_by_size(hwidb->max_buffer_size + delta )) {
	printf("\n%% Invalid MTU specified");
	swidb->user_configured_mtu = sav_user_configured;
	return;
    }

    if (hwidb->status & IDB_SERIAL) {
        if (!reg_invoke_media_check_mtu(hwidb->type, hwidb, mtu))
            return;
    }

    if (!if_change_mtu(swidb, mtu)) {
        swidb->user_configured_mtu = sav_user_configured;
	return;
    }
      
    reg_invoke_interface_cfg_modified(hwidb);
}

/*
 * set_interface_bandwidth
 *
 * Set the interface bandwidth.  If it is changing, invoke the registry.
 */

static void set_interface_bandwidth (idbtype *idb, ulong new_visible_bandwidth)
{
    hwidbtype *hwidb;
    if (idb->visible_bandwidth != new_visible_bandwidth) {
        idb->visible_bandwidth = new_visible_bandwidth;
        reg_invoke_bandwidth_changed(idb);
	if (!is_subinterface(idb)) {
	    hwidb = hwidb_or_null(idb);
	    /*
	     * When Bandwidth is set > 2048Kbps, change queuing
	     * algorithm from wfq to fifo only if wfq is on by default
	     */
	    if ((hwidb->queue_algorithm == QUEUE_FAIR_DEF) && 
		(new_visible_bandwidth > FAIR_QUEUE_DEFAULT_BW)) {
		delete_fair_queue(hwidb);
	    }
	}   
    }
}

/*
 * set_default_interface_bandwidth
 *
 * Updates the default bandwidth setting.
 *
 * Updates the current bandwidth setting if the bandwidth hasn't been
 * set manually.
 *
 * Sets the queue type based on the bandwidth setting; this may be
 * changed later by configuration or mitigating circumstances.
 *
 * The parameter is in Kbps.
 */
void set_default_interface_bandwidth (idbtype *idb, ulong visible_bandwidth)
{
    hwidbtype *hwidb;

    set_vaccess_prevent_reset(idb);

    idb->visible_bandwidth_def = visible_bandwidth;
    if (!idb->manual_bandwidth) {
	set_interface_bandwidth(idb, visible_bandwidth);
    }
    if (!is_subinterface(idb)) {
	hwidb = hwidb_or_null(idb);
	if (hwidb && visible_bandwidth > FAIR_QUEUE_DEFAULT_BW) {
	    delete_fair_queue(hwidb);
	}
    }

    clear_vaccess_prevent_reset(idb);
}

/*
 * bandwidth_command
 * Set the channel's inverse bandwidth parameter for IGRP.
 * Argument must be in kilobits/sec.
 */

void bandwidth_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    ulong new_visible_bandwidth;

    if (csb->nvgen) {
	nv_write(idb->manual_bandwidth,
	         "%s %d", csb->nv_command,
	         idb->visible_bandwidth);
	return;
    }

    if (csb->sense) {
	new_visible_bandwidth = GETOBJ(int,1);
	idb->manual_bandwidth = TRUE;
	
    } else {
	new_visible_bandwidth = idb->visible_bandwidth_def;
	idb->manual_bandwidth = FALSE;
    }
    set_interface_bandwidth(idb, new_visible_bandwidth);
}

/*
 * logging_interface_command
 * Disable/Enable log messages on for an interface.
 */

void logging_interface_command (parseinfo *csb)
{
    hwidbtype *idb = csb->interface->hwptr;

    if (csb->nvgen) {
	nv_write(!(LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(idb)),
	         "no %s", csb->nv_command);
	return;
    }

    if (csb->sense) {
	idb->logging_events &= ~LOG_INTF_FLAGS_LINK_STATUS;
    } else {
	idb->logging_events |= LOG_INTF_FLAGS_LINK_STATUS;
    }
}

/*
 * set_interface_delay
 *
 * If anything changed, update the idb delay parameters and call the
 * registry entry.
 */

static void set_interface_delay (idbtype *idb, ulong new_delay)
{
    if (idb->delay != new_delay) {
	idb->delay = new_delay;
	reg_invoke_delay_changed(idb);
    }
}

/*
 * set_default_interface_delay
 *
 * Updates the default delay settings.
 *
 * Updates the current delay settings if the delay hasn't been set manually.
 */
void set_default_interface_delay (idbtype *idb, ulong delay)
{
    idb->delay_def = delay;
    if (!idb->manual_delay) {
	set_interface_delay(idb, delay);
    }
}

/*
 * delay_command
 * Set the station delay parameter
 * Argument must be in units of 10 microseconds.
 * The parser won't allow delay values of zero, which would screw
 * up IGRP.
 */
void delay_command (parseinfo *csb)
{
    idbtype *idb;
    ulong new_delay;

    idb = csb->interface;

    if (csb->nvgen) {
	nv_write(idb->manual_delay,
	         "%s %d", csb->nv_command, idb->delay);
	return;
    }

    if (csb->sense) {
	new_delay = GETOBJ(int,1);
	idb->manual_delay = TRUE;
	
    } else {
	new_delay = idb->delay_def;
	idb->manual_delay = FALSE;
    }
    set_interface_delay(idb, new_delay);
}

/*
 * description_command
 * Parse a descriptive name for a interface
 */

void description_command (parseinfo *csb)
{
    char *tmp, *tmp2;

    if (csb->interface == NULL)
	return;

    if (csb->nvgen) {
	nv_write((csb->interface->description != NULL),
	         "%s %s", csb->nv_command, csb->interface->description);
	return;
    }

    if (!csb->sense) {
	tmp = NULL;
    } else {
	tmp = strdup(GETOBJ(string,1));
	if (tmp == NULL) {
	    printf("\n%% No memory for description");
	    return;
	}
    }
    tmp2 = csb->interface->description;
    csb->interface->description = tmp;
    if (tmp2 != NULL) {
	free(tmp2);
    }
}

/*
 * transmitif_command
 * This is used to support dynamic routing over simplex interfaces.
 * This command specifies an output interface to be used in conjunction
 * with a receive only interface.
 */

void transmitif_command (parseinfo *csb)
{
    idbtype *idb;

    if (csb->nvgen) {
	idb = csb->interface;
	nv_write(idb != idb->output_if,
		 "%s %s", csb->nv_command,
		 idb->output_if->namestring);
	return;
    }

    if (!csb->sense) {
	idb = csb->interface;
	idb->output_if->hwptr->xmitonly = FALSE;
	idb->output_if = idb;
	idb->hwptr->output_if_hw = idb->hwptr;
    } else {
	idb = GETOBJ(idb,1);
	csb->interface->output_if = idb;
	csb->interface->hwptr->output_if_hw = idb->hwptr;
	if (idb != csb->interface) {
	    idb->hwptr->xmitonly = TRUE;
	    reg_invoke_clear_routes();
	}
    }
}

/*
 * pulsetime_command
 * Specify an interval in seconds to keep DTR down when resetting
 * a serial interface
 */

void pulsetime_command (parseinfo *csb)
{
    hwidbtype *hwidb = csb->interface->hwptr;

    /*
     * Check to see if the interface is valid.  The parser already checked
     * to see if it was a serial interface.
     */
    if (!(hwidb->status & IDB_ISDN) && !(hwidb->status & IDB_PCBUS) &&
	!is_g703(hwidb) && (hwidb->type != IDBTYPE_T1) &&
	(hwidb->type != IDBTYPE_E1)) {
	if (csb->nvgen) {
	    nv_write((hwidb->serial_pulseinterval !=
		      hwidb->serial_pulseinterval_def),
		     "%s %d", csb->nv_command,
		     hwidb->serial_pulseinterval/ONESEC);
	    return;
	}
	
	if (csb->sense) {
	    hwidb->serial_pulseinterval = GETOBJ(int,1)*ONESEC;
	} else {
	    hwidb->serial_pulseinterval = hwidb->serial_pulseinterval_def;
	}
	
	reg_invoke_interface_cfg_modified(hwidb);
    }
}

void media_type_command (parseinfo *csb)
{
    reg_invoke_media_type_cmd(csb->interface->hwptr->type, csb);
}

/*
 * default_media_half_or_full_duplex_params_command
 */
static void default_media_half_or_full_duplex_params_command (parseinfo* csb)
{
    hwidbtype *idb  = csb->interface->hwptr;

    if (csb->nvgen || (csb->which == CFG_HDX_RESET_PARAMS))
        return;

    printf("\nThis command is not supported for %s.", idb->hw_namestring);
    return;
}

void media_half_or_full_duplex_params_command (parseinfo *csb)
{
    reg_invoke_media_half_or_full_duplex_cmd(csb->interface->hwptr->type, csb);
}

/*
 * backup_set_interface will turn the idb given into a backup interface
 */
static void backup_set_interface (hwidbtype *idb)
{
    if (idb->state != IDBS_ADMINDOWN) {
	idb->standby = TRUE;
	shutdown_going_down(idb);
	/* idb->state is now set to be admin-down */
    }
}

/*
 * backup_clear_interface will clear the idb of being a backup interface
 */
static void backup_clear_interface (hwidbtype *idb)
{
    if (idb == NULL)
	return;

    if (idb->standby) {
	shutdown_coming_up(idb);
    }
    idb->standby = FALSE;	/* this idb is no more a backup */
    TIMER_STOP(idb->delay_timer);
}

/*
 * backup_check_interface
 * Check if checkidb can be a backup interface of either the swidb
 * or the hwidb of thisswidb.
 */
static boolean backup_check_interface (hwidbtype *checkidb,
				       idbtype *thisswidb)
{
    hwidbtype *idbck, *thishwidb = thisswidb->hwptr;
    idbtype *swidbck;

    if (checkidb == thishwidb) {
	printf("\nMust specify a different interface");
	return(FALSE);
    }
    if (checkidb->status & IDB_DELETED) {
	printf("\nInvalid backup interface");
	return(FALSE);
    }
    if (thisswidb->backup == checkidb)	/* redundent config */
	return(FALSE);
    if (thishwidb->secondary == checkidb) {
       if (is_subinterface(thisswidb)) {
	   printf("\n%s is already a backup for %s.",
		  checkidb->hw_namestring, thishwidb->hw_namestring);
       }
       return(FALSE);
    }
    if (is_ddr(thishwidb)) {
	printf("\n%s Interface already configured as dialer",
	       thishwidb->hw_namestring);
	return(FALSE);
    }
    FOR_ALL_HWIDBS(idbck) {
	if (idbck != thishwidb && idbck->secondary == checkidb) {
	    printf("\n%s is already a backup for %s.",
		   checkidb->hw_namestring, idbck->hw_namestring);
	    return(FALSE);
	}
	FOR_ALL_SWIDBS_ON_HW(idbck, swidbck) {
	    if (swidbck != thisswidb && swidbck->backup == checkidb) {
		printf("\n%s is already a backup for %s.",
		       checkidb->hw_namestring,swidbck->namestring);
		return(FALSE);
	    }
	}
    }
    return(TRUE);
}


static void bring_up_subif_backup (
    idbtype *swidb,
    hwidbtype *bkupidb,
    boolean delay)
{
    swidb->prev_state = FALSE;	/* so that backup_timers doesn't start timer */
    TIMER_STOP(bkupidb->delay_timer); /* safety */
    if (delay && (swidb->down_delay > 0)) {
	TIMER_START(bkupidb->delay_timer, swidb->down_delay * ONESEC);
	swidb->backup_active = FALSE;	/* not yet active */
    } else {
	shutdown_coming_up(bkupidb);
	swidb->backup_active = TRUE;
    }
}

static void bring_down_subif_backup (
    idbtype *swidb,
    hwidbtype *bkupidb,
    boolean delay)
{
    TIMER_STOP(bkupidb->delay_timer); /* safety */
    if (delay && (swidb->up_delay > 0)) {
	TIMER_START(bkupidb->delay_timer, swidb->up_delay * ONESEC);
    } else {
	shutdown_going_down(bkupidb);
	swidb->backup_active = FALSE;
    }
    swidb->prev_state = interface_up(swidb);
}

static inline boolean backup_is_shut (
    hwidbtype *bkupidb)
{
    return(!bkupidb->standby && bkupidb->state == IDBS_ADMINDOWN);
}

static void subif_backup_adjust(
    idbtype *idb,
    boolean delay)
{
    hwidbtype *bkupidb;
    boolean subif_up;

    if ((bkupidb = idb->backup) && !backup_is_shut(bkupidb)) {
	subif_up = interface_up(idb);
	if (subif_up && bkupidb->state == IDBS_UP) {
	    bring_down_subif_backup(idb, bkupidb, delay);
	} else if (!subif_up && !idb->backup_active) {
	    bring_up_subif_backup(idb, bkupidb, delay);
	}
    }
}


static void backup_set_default_delay (
    hwidbtype *primeidb)
{
    if (!(primeidb->backup_state & BACKUP_IN_DELAY)) {
	primeidb->backup_state |= BACKUP_IN_DELAY;
	primeidb->in_delay = 0;
    }
    if (!(primeidb->backup_state & BACKUP_OUT_DELAY)) {
	primeidb->backup_state |= BACKUP_OUT_DELAY;
	primeidb->out_delay = 0;
    }
}


/*
 * backup_command
 * Enable/Disable an interface for dial backup (dial backup may be the
 * wrong term as this is a more generic form)
 */

void backup_command (parseinfo *csb)
{
    hwidbtype *hwidb, *csbidb;
    idbtype *swidb = csb->interface;
    boolean primary_up;

    /*
     * backup command for a subinterface
     */
    if (is_subinterface(swidb)) {
	if (csb->nvgen) {
	    if (swidb->backup == NULL)
		return;
	    switch (csb->which) {
	    case BACKUP_DELAY:
		nv_write(TRUE,"%s %d %d", csb->nv_command,
			swidb->down_delay, swidb->up_delay);
		break;
	    case BACKUP_INTERFACE:
		nv_write(TRUE, "%s %s", csb->nv_command,
			 swidb->backup->hw_namestring);
		break;
	    }
	    return;
	}
	if (!(is_p2p(swidb)) &&
	    !is_frame_relay(swidb->hwptr)) {
	    printf("\nCan only backup point to point interfaces");
	    return;
	}
	/*
	 * Register backup timer routine
	 */
	reg_add_if_onesec(backup_timers, "backup_timers");

	switch (csb->which) {
	case BACKUP_INTERFACE:
	    hwidb = hwidb_or_null(GETOBJ(idb,1));
	    if (csb->sense) {
		if (backup_check_interface(hwidb, swidb) == FALSE)
		    return;
		backup_clear_interface(swidb->backup);
		swidb->backup = hwidb;
		if (interface_up(swidb)) {
		    TIMER_STOP(hwidb->delay_timer); /* safety */
		    backup_set_interface(hwidb);  /* put in stand-by mode */
		    swidb->backup_active = FALSE;
		    swidb->prev_state = TRUE;

		} else {	/* try to bring up this backup if we can */
		    switch (hwidb->state) {
		    case IDBS_UP:
			swidb->prev_state = FALSE;
			swidb->backup_active = TRUE;
			hwidb->standby = TRUE;
			break;
		    case IDBS_ADMINDOWN:
			swidb->prev_state = FALSE;
			swidb->backup_active = FALSE;
			hwidb->standby = FALSE;
			break;
		    case IDBS_DOWN:
		    case IDBS_GOINGDOWN:
			hwidb->standby = TRUE;
			bring_up_subif_backup(swidb, hwidb, FALSE); /* no delay*/
			break;
		    case IDBS_INIT:
		    case IDBS_RESET:
		    case IDBS_TESTING:
			swidb->backup_active = FALSE;
			hwidb->standby = TRUE;
			swidb->prev_state = TRUE;
			/* so that 1 sec. later we'll try to bring it up */
			break;
		    default:
			break;
		    }
		}
	    } else {
		/* "no backup" command */
		if (swidb) {
		    backup_clear_interface(swidb->backup);
		    swidb->backup = NULL;
	            swidb->backup_active = FALSE;
	            swidb->prev_state = FALSE;
		    swidb->up_delay = swidb->down_delay = 0;
		}
	    }
	    break;
	case BACKUP_DELAY:
	    if (csb->sense) {
		swidb->down_delay = GETOBJ(int,1);
		swidb->up_delay = GETOBJ(int,2);
	    } else {
		swidb->up_delay = swidb->down_delay = 0;
		return;
	    }
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;	/* done with subif level backup command handling */
    }

    /*
     * backup command for hardware interfaces
     */
    csbidb = hwidb_or_null(csb->interface);
    if (csb->nvgen) {
	if (csbidb->secondary == NULL) {
	    return;
	}
	switch (csb->which) {
	  case BACKUP_DELAY:
	    if (csbidb->backup_state & BACKUP_IN_DELAY) {
		if (csbidb->backup_state & BACKUP_OUT_DELAY) {
		    nv_write(TRUE, "%s %d %d", csb->nv_command,
			     csbidb->in_delay, csbidb->out_delay);
		} else {
		    nv_write(TRUE, "%s %d never", csb->nv_command,
			     csbidb->in_delay);
		}
	    } else {
		nv_write((csbidb->backup_state & BACKUP_OUT_DELAY),
			 "%s never %d", csb->nv_command,
			 csbidb->out_delay);
	    }
	    break;
	  case BACKUP_INTERFACE:
	    nv_write(TRUE, "%s %s", csb->nv_command,
		     csbidb->secondary->hw_namestring);
	    break;
	  case BACKUP_LOAD:
	    if (csbidb->backup_state & BACKUP_IN_LOAD) {
		if (csbidb->backup_state & BACKUP_OUT_LOAD) {
		    nv_write(TRUE, "%s %d %d", csb->nv_command,
			     (csbidb->in_load * 100 + 128) / 255,
			     (csbidb->out_load * 100 + 128) / 255);
		} else {
		    nv_write(TRUE, "%s %d never", csb->nv_command,
			     (csbidb->in_load * 100 + 128) / 255);
		}
	    } else {
		nv_write((csbidb->backup_state & BACKUP_OUT_LOAD),
			 "%s never %d", csb->nv_command,
			 (csbidb->out_load * 100 + 128) / 255);
	    }
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }
    /*
     * Register some functions.  Call backup_timers first, then backup_loads.
     */
    reg_add_if_onesec(backup_timers, "backup_timers");
    reg_add_if_onesec(backup_loads, "backup_loads");

    switch (csb->which) {
      case BACKUP_INTERFACE:
	hwidb = hwidb_or_null(GETOBJ(idb,1));
	if (csb->sense) {
	    if (backup_check_interface(hwidb, csbidb->firstsw) == FALSE)
		return;
	    backup_clear_interface(csbidb->secondary);
	    csbidb->secondary = hwidb;
	    backup_set_default_delay(csbidb);  /* use default if not yet set */
	    if (csbidb->keep_transition || hw_interface_up(csbidb)) {
		/* put the secondary in stand-by mode if it's not shut */
		backup_set_interface(hwidb);

	    } else {	/* primary not in transition mode and primary's down */
		if (hwidb->state == IDBS_ADMINDOWN) { /* backup's shut already*/
		    hwidb->standby = FALSE;	/* safety */
		} else {
		    hwidb->standby = TRUE;
                    /* bring up the backup */
		    if ((hwidb->state != IDBS_UP) &&
			(csbidb->backup_state & BACKUP_IN_DELAY)) {
			if (csbidb->in_delay > 0) {
			    TIMER_START(hwidb->delay_timer,
			    csbidb->in_delay * ONESEC);
			} else {
			    TIMER_STOP(hwidb->delay_timer);
			    shutdown_coming_up(hwidb);
			}
		    }
		}
	    }
	} else {	/* 'no backup int' */
	    if (csbidb) {
		backup_clear_interface(csbidb->secondary); /* bring it up */
		csbidb->secondary = NULL;
		csbidb->backup_state &= ~(BACKUP_IN_LOAD | BACKUP_OUT_LOAD |
					  BACKUP_IN_DELAY | BACKUP_OUT_DELAY);
	    }
	}
	break;

    case BACKUP_LOAD:
	if (csb->sense) {
	    if (GETOBJ(int,1) == -1) {	/* "never" comes up due to load */
		csbidb->backup_state &= ~BACKUP_IN_LOAD;

	    } else {	/* comes up when primary exceeds this much load */
		csbidb->in_load = (GETOBJ(int,1) * 255 + 50) / 100;
		csbidb->backup_state |= BACKUP_IN_LOAD;
	    }
	    if (GETOBJ(int,2) == -1) {	/* never goes down due to load */
		csbidb->backup_state &= ~BACKUP_OUT_LOAD;

	    } else {	/* goes down when load on primary below this much */
		csbidb->out_load = (GETOBJ(int,2) * 255 + 50) / 100;
		csbidb->backup_state |= BACKUP_OUT_LOAD;
	    }
	} else {	/* default- don't come up or go down due to the load */
	    csbidb->backup_state &= ~(BACKUP_IN_LOAD | BACKUP_OUT_LOAD);
	}

	if (!(csbidb->backup_state & BACKUP_LOAD_MASK) &&
		(hwidb = csbidb->secondary) &&
		!csbidb->keep_transition &&
		!backup_is_shut(hwidb)) {
	    primary_up = hw_interface_up(csbidb);
	    if (primary_up && hwidb->state == IDBS_UP && 
		(csbidb->backup_state & BACKUP_OUT_DELAY))
		shutdown_going_down(hwidb);
	    else if (!primary_up && hwidb->state != IDBS_UP &&
		(csbidb->backup_state & BACKUP_IN_DELAY))
		shutdown_coming_up(hwidb);
	}
	break;

    case BACKUP_DELAY:
	if (csb->sense) {
	    if (GETOBJ(int,1) == -1) {	/* never activate the backup */
		if (GETOBJ(int,2) == -1) { /* AND never deactive the backup */
		    /* "backup delay never never" doesn't make sense */
		    printf("\n%% Invalid backup command");
 		    return;
		}
		csbidb->backup_state &= ~BACKUP_IN_DELAY;
		if ((hwidb = csbidb->secondary) && !csbidb->keep_transition
			&& hwidb->state == IDBS_UP)
		    shutdown_going_down(hwidb);

	    } else {			/* comes up with delay */
		csbidb->in_delay = GETOBJ(int,1);
		csbidb->backup_state |= BACKUP_IN_DELAY;
	    }
	    if (GETOBJ(int,2) == -1) {	/* never deactivate the backup */
		csbidb->backup_state &= ~BACKUP_OUT_DELAY;
		if ((hwidb = csbidb->secondary) && !csbidb->keep_transition
			&& hwidb->state != IDBS_UP && hwidb->standby == TRUE)
		    shutdown_coming_up(hwidb);

	    } else {			/* goes down with delay */
		csbidb->out_delay = GETOBJ(int,2);
		csbidb->backup_state |= BACKUP_OUT_DELAY;
	    }
	} else {	/* no form brings default setting-- delay=(0, 0) */
	    csbidb->backup_state |= (BACKUP_IN_DELAY | BACKUP_OUT_DELAY);
	    csbidb->in_delay = csbidb->out_delay = 0;
	}
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
    /* done with delay config command of interface level */
}

/*
 * delete_interface
 * Delete an interface
 * Note: the idb still exists after this.
 */

boolean delete_interface (idbtype *idb)
{
    hwidbtype *hwidb;
    boolean deleted;

    /*
     *  This can never happen
     */
    if (idb == NULL || idb->hwptr == NULL)
	return(FALSE);
    
    hwidb = idb->hwptr;
    deleted = FALSE;

    /*
     *	Is the underlying hardware delete-able?  It is if it was created
     *	by the software.
     *	
     *	Do NOT delete main interface when frame-relay subinterface is 
     *	removed (CSCdj05415).
     */
    if ((hwidb->status & IDB_EPHEMERAL) &&
	(!is_frame_relay(hwidb) || is_first_swidb(idb))) {
	hwidb->status |= IDB_DELETED;
	reg_invoke_ifmib_deregister_subif(hwidb->snmp_if_index);
	shutdown_going_down(hwidb);
	deleted = TRUE;
    }

    /*
     *	What to do with the software IDB?  Subinterfaces must be shutdown
     *	explicitly, as they probably didn't get shut down above.  Virtual
     *	channels must just erase.
     */
    if (is_subinterface(idb)) {
	/* These are subinterfaces... */
	shutdown_subif(idb, SUBIF_STATE_DELETED);
	reg_invoke_ifmib_deregister_subif(idb->snmp_if_index);
	deleted = TRUE;
    } else if (hwidb->status & IDB_VIRTUAL_HW) {
	/* This is a dsx1 interface with virtual interfaces */
	deleted = TRUE;
    }
    if (deleted) {
	reg_invoke_swif_erase(idb);
	memset(idb->ip_iprotofltr, 0, sizeof (idb->ip_iprotofltr));
	memset(idb->ip_oprotofltr, 0, sizeof (idb->ip_oprotofltr));
    }
    return(deleted);
}

/*
 * shutdown_command
 * Enable/Disable a network interface
 * If this is a secondary interface and the user issues a "no shutdown"
 * then leave it up to the backup code to call shutdown_going_down() or
 * shutdown_coming_up().
 */

void shutdown_command (parseinfo *csb)
{
    hwidbtype *csbidb;
    idbtype *swidb;

    swidb = csb->interface;
    csbidb = csb->interface->hwptr;

    if (csb->nvgen) {
	if (is_subinterface(swidb)) {
	    nv_write(swidb->subif_state == SUBIF_STATE_ADMINDOWN,
		     csb->nv_command);
	} else {
	    nv_write(((!csbidb->standby) &&
		      (csbidb->state == IDBS_ADMINDOWN)),
		     csb->nv_command);
	}
    } else
	shutdown_interface(swidb, csb->sense);
}

void shutdown_interface (idbtype *swidb, boolean sense)
{
    hwidbtype *idb, *csbidb;
    idbtype *swidbck;
    boolean prime_if_up;
       
    csbidb = swidb->hwptr;
    
    if (csbidb->status & IDB_VTEMPLATE) {
	printf("\n%% Shutdown not allowed on virtual template interface.");
 	return;
    }
 
    if (csbidb->status & IDB_DIALER) {
        printf("\n%% Shutdown not allowed on rotary group lead.");
	return;
    }

    if (ISBRILEASED(csbidb)) {
	if(sense)
	    (*csbidb->isdn_info->cstate_bri_leased)(csbidb, IDBS_ADMINDOWN);
	else
	    (*csbidb->isdn_info->cstate_bri_leased)(csbidb, IDBS_UP);
	return;
    }

    /*
     * Loop through hwidb and swidb lists to check for backup.
     * Only an interface can be a backup, not a subinterface.
     */
    if (is_subinterface(swidb) == FALSE) {
	FOR_ALL_HWIDBS(idb) {
	    if (idb->secondary == csbidb) {
		if (sense) {	/* shut the backup interface */
		    if (!backup_is_shut(csbidb)) {
			csbidb->standby = FALSE;
			TIMER_STOP(csbidb->delay_timer);
			shutdown_going_down(csbidb);
		    }
		} else {	/* unshut the backup interface */
		    csbidb->standby = TRUE;
		    prime_if_up = hw_interface_up(idb);
		    if (prime_if_up && (csbidb->state != IDBS_ADMINDOWN) &&
			    (idb->backup_state & BACKUP_OUT_DELAY) &&
			    !(idb->backup_state & BACKUP_OUT_LOAD)) {
			shutdown_going_down(csbidb);
		    } else if (!prime_if_up && (csbidb->state != IDBS_UP) &&
			    (idb->backup_state & BACKUP_IN_DELAY)) {
			shutdown_coming_up(csbidb);
		    }
		}
		return;
	    }
	    FOR_ALL_SWIDBS_ON_HW(idb, swidbck) {
		if (swidbck->backup == csbidb) {
		    if (sense) {	/* shut the backup */
			if (!backup_is_shut(csbidb)) {
			    csbidb->standby = FALSE;
			    bring_down_subif_backup(swidbck, csbidb, FALSE);
			}
		    } else {	/* unshut the backup */
			csbidb->standby = TRUE;
			subif_backup_adjust(swidbck, FALSE); /* no delay */
		    }
		    return;
		}
	    }
	}
    }

    if (csbidb->status & IDB_FDDI) {
	ONE_ITEM_LIST(tmp, FDDI_HW_SHUTDOWN_REASONS, sizeof(ushort), 0);
	
	(*csbidb->state_config)(csbidb, IDB_CONTROL_SET_STATE, &tmp_list);
    }

    if (is_subinterface(swidb))
	shutdown_subif(swidb, sense ? SUBIF_STATE_ADMINDOWN : SUBIF_STATE_UP);
    else {
	if (sense)
	    shutdown_going_down(csbidb);
	else
	    shutdown_coming_up(csbidb);
    }
}

/*
 * shutdown_going_down
 * Take an interface into the administratively down state.
 *
 * Some interfaces have delayed start up properties.
 * For these interfaces idb->cstate is not called.  This
 * would cause a state transition before it actually
 * occurs.
 *
 * For interfaces that handle their own state transitions
 * because of say delayed start idb->reset and idb->shutdown
 * handle the state transitions.
 */

void shutdown_going_down (hwidbtype *hwidb)
{
    idbtype *swidb;

    /*
     * Do pre-shutdown activities
     */
    reg_invoke_hwif_goingdown(hwidb);
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb)
        reg_invoke_swif_goingdown(swidb);
    
    /*
     * Invoke the state change vector.
     * If this is a no-op and a specific shutdown vector exists, call it
     */

    /* NmEvent Hook 1 */
    hwidb->failCause = AdminDownCommand;

    net_cstate(hwidb, IDBS_ADMINDOWN);
    if (hwidb->shutdown && !(hwidb->status & IDB_NOHARDWARE))
	(*hwidb->shutdown)(hwidb, IDBS_ADMINDOWN);

    /*
     * Fixup our fastswitching caches and network topology databases
     */
    reg_invoke_bump_cache_version(hwidb, NULL, FALSE);

    transition_adjust(hwidb);
    route_adjust(hwidb);
    bridge_adjust(hwidb);   
    if (hwidb->keep_transition)
	backup_adjust(hwidb);
    reg_invoke_if_statechange_complete(NULL, hwidb);
}

/*
 * shutdown_coming_up
 * Take an interface out of the administratively down state
 */

void shutdown_coming_up (hwidbtype *hwidb)
{
    int newstate;
    idbtype *swidb;

    /*
     * Make sure our configuration allows us to remove this interface
     * from the shutdown state.
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	if (reg_invoke_stay_shutdown(swidb))
	    return;

    }

    /*
     * Prevent the "no shut" command from toggling an
     * FDDI or an ATM interface that is already up.
     */
    if ( ( (hwidb->status & IDB_FDDI) || (hwidb->status & IDB_ATM) ) 
                                && (hwidb->state == IDBS_UP) )
        return;

    /*
     * Set new state based on hardware type.
     * Serial and TR make their own decisions about being up or down.
     */
    if ((hwidb->status & (IDB_SERIAL|IDB_TR|IDB_FDDI)) ||
	(hwidb->type == IDBTYPE_ATMZR))
	newstate = IDBS_DOWN;
    else 
	newstate = IDBS_UP;

    /*
     * Cause a state transition.
     * Then issue a hardware reset.
     */
    net_cstate(hwidb, newstate);
    (*(hwidb->reset))(hwidb);

    /*
     * Fixup our fastswitching caches and network topology databases
     */
    reg_invoke_bump_cache_version(hwidb, NULL, FALSE);
    transition_adjust(hwidb);
    route_adjust(hwidb);
    bridge_adjust(hwidb);
    reg_invoke_hwif_comingup(hwidb);
    if (hwidb->keep_transition)
	backup_adjust(hwidb);
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb)
        reg_invoke_swif_comingup(swidb);
    reg_invoke_if_statechange_complete(NULL, hwidb);
}

/*
 * shutdown_subif
 * Shutdown command for subinterfaces.
 */

void shutdown_subif (idbtype *idb, ulong newstate)
{
    switch (newstate) {
    case SUBIF_STATE_UP:
	/*
	 * Make sure our configuration allows us to remove this interface
	 * from the shutdown state.
	 */
	if (reg_invoke_stay_shutdown(idb))
	    return;
        idb->subif_state = SUBIF_STATE_UP;
        idb->subif_line_state = SUBIF_STATE_UP;
	reg_invoke_swif_comingup(idb);
  	/*
  	 * When the interface is coming up. make sure that the sub_mtu and
  	 * max_mtu is in sync
  	 */
  	set_max_interface_mtu(idb, 0);
        break;

    case SUBIF_STATE_ADMINDOWN:
    case SUBIF_STATE_DELETED:
    case SUBIF_STATE_DOWN:
	reg_invoke_swif_goingdown(idb);
        /*
         * FALL THROUGH
         */
    default:
        idb->subif_state = newstate;
        idb->subif_line_state = newstate;
        break; 
    }
    reg_invoke_bump_cache_version(idb->hwptr, NULL, FALSE);
    reg_invoke_route_adjust_msg(idb);
    reg_invoke_route_adjust(idb);
    reg_invoke_bridge_adjust(idb);
    reg_invoke_vlan_adjust(idb);
    reg_invoke_if_statechange_complete(idb, NULL);
    subif_backup_adjust(idb, TRUE); /* take care the backup accordingly */
}


/*
 * This function is here to set the operational state of sub-if
 * up or down. 
 * 
 * Parameters:
 *
 * idb : Pointer to swidb
 * newstate: The desired new line state for swidb
 * notify:   A boolean to indicate if the protocols have to be notified.
 *
 * Return value:
 * None
 *
 * This is an interface for the lower layer protocols, which 
 * control the operational state of the subif. The motivation for this
 * function is to effect changes to other protocols / tables, which 
 * depend on the operational state of subif. This function does NOT 
 * alter the hardware line protocol state.
 */
void subif_line_statechange (idbtype *idb, ulong newstate, boolean notify)
{
    if (newstate !=  idb->subif_line_state) {
	idb->subif_line_state = newstate;
	switch (newstate) {
	case SUBIF_STATE_UP:
	    /*
	     * Make sure our configuration allows us to remove this interface
	     * from the shutdown state.
	     */
	    if (reg_invoke_stay_shutdown(idb))
		return;
	    if (notify)
		reg_invoke_swif_comingup(idb);
	    break;

	case SUBIF_STATE_ADMINDOWN:
	case SUBIF_STATE_DELETED:
	case SUBIF_STATE_DOWN:
	    if (notify)
		reg_invoke_swif_goingdown(idb);
	    break;

	default:
	    break;
	}
	reg_invoke_bump_cache_version(idb->hwptr, NULL, FALSE);
	reg_invoke_route_adjust_msg(idb);
	reg_invoke_route_adjust(idb);
	reg_invoke_bridge_adjust(idb);
	reg_invoke_vlan_adjust(idb);
	subif_backup_adjust(idb, TRUE); /* take care the backup accordingly */
    }
}


/*
 * reset_if
 * Here to reset an interface.
 * Used by SNMP and by EXEC "clear interface"
 */

void reset_if (hwidbtype *idb)
{
    idbtype *swidb;

    if (!idb)
	return;
    /*
     * Currently the only interface that maintains its
     * own idb state information is the token ring.
     * This is a kludge to avoid changing all of the
     * driver code.  When the other drivers have
     * reset modified to support their own interface state
     * then the following should become:
     */
    if (!(idb->status & IDB_TR) || (idb->state != IDBS_ADMINDOWN))
	(*idb->reset)(idb);
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	reg_invoke_reset_if(swidb);
    }
}

/*
 * periodic_activity
 * Called every second from the tty_background process.
 *
 * Decrement idb->keep_count and perform periodic actions if it goes to zero.
 *  - reset periodic counter.
 *  - clear input error rate counter
 *  - check serial lines for restarting from glitches
 *  - perform any periodic keepalive functions
 *  - check protocol routing table consistency (route_adjust)
 *  - do bridge state consistency checking (bridge_adjust)
 */

static void periodic_activity (hwidbtype *idb)
{
    boolean linestate;
    hwidbtype *netidb;

    /*
     * Execute the registered functions first, then do the
     * platform, media, protocol independent functions.
     */
    reg_invoke_if_onesec(idb);

    /*
     * Do periodic keepalive and routing table actions here.
     */
    if (--idb->keep_count <= 0) {
        idb->keep_count = idb->keep_period;
        idb->error_count = 0L;
	/*
	 * device_periodic needs to be called before idb->periodic.
	 * This is because device_periodic gets information from the
	 * IPs on 7000 type devices.  This information contains things
	 * like packets transmitted counts.  Some of this information
	 * may be used by routines called by idb->periodic, like PPP LQM.
	 * Calling this after periodic causes data to be out of date
	 * by one keepalive period, which may be sufficient to cause
	 * a line to flap.
	 */
        if (idb->device_periodic)
            (*(idb->device_periodic))(idb);
        if (idb->periodic)
            (*(idb->periodic))(idb);

        /*
         * If the MCI disabled us, then undo the  bit that said this.
         * If no keepalives are active or there is no periodic function,
         * then reset the interface to get things starts. This fixes the
         * problem with LAPB and other protocols which may not always
         * catch MCI disabled interfaces.
         */
        if (idb->status & IDB_DISABLED) {
	    idb->status &= ~IDB_DISABLED;
	    if (idb->periodic == NULL || idb->nokeepalive)
	        (*idb->reset)(idb);
        }
    }

    /*
     * Announce line protocol up/down events.
     * Ignore interface if we are spoofing.
     *
     * Note that this code use to be inside the above if statement 
     * for keepalive period.  This had the effect of postponing line
     * state changes for the keepalive period.  For interfaces with 
     * long keepalive periods, this meant that installed routes would
     * not be available for a significant amount of time.  So now, if
     * the linestate has changed, do the adjust routines immediately.
     */
    if (!reg_invoke_dialer_spoofing_prot(idb))  {
        linestate = hw_interface_up(idb);
        if (linestate != idb->newproto) {
	    if (!system_loading) {
		netidb = get_netidb(idb);
		if (LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(netidb)) {
		    errmsg(&msgsym(UPDOWN, LINEPROTO), idb->hw_namestring, 
			   linestate ? "up" : "down");
		}
	    }
	    idb->newproto = linestate;
	    GET_TIMESTAMP(idb->state_time);
            if (linestate)
                idb->lastFailCause = 0;    /* clear last fail cause */
            else if (idb->failCause)
                LinkDownEvent(idb);      /* link down event logger */
        }
    }

    /*
     * Readjust things in case protocol line state changes have happened.
     * If carriertimer is active that says an interface state change is in
     * the works (we are potentially flapping) so defer any adjustments
     * until things settle down.
     *
     * We only do route_adjust and bridge_adjust if there was a protocol
     * line transition.  This gives us line state changes.  Interface state
     * changes are picked up by cstate_check.
     */
    if (!(mgd_timer_running(&idb->carriertimer))) {
        transition_adjust(idb);
	route_adjust(idb);
        if (idb->keep_transition) {
	    bridge_adjust(idb);
	    backup_adjust(idb);
	    reg_invoke_if_statechange_complete(NULL, idb);
        }
    }
}

/*
 * net_periodic
 *
 * Perform once a second interface periodic functions
 */
process net_periodic (void)
{
    hwidbtype *idb;

    while (TRUE) {
	process_sleep_periodic(ONESEC);
	FOR_ALL_HWIDBS(idb) {
	    periodic_activity(idb);
	}
    }
}

/*
 * net_onesecond
 *
 * Perform a variety of global system tasks once a second
 */
process net_onesecond (void)
{
    while (TRUE) {
	process_sleep_periodic(ONESEC);
	reg_invoke_onesec();
    }
}

/*
 * net_oneminute
 * Perform a variety of tasks once a minute.
 */

static process net_oneminute (void)
{
    while (TRUE) {
	process_sleep_periodic(ONEMIN);

	/*
	 * The following are kernel functions that are always run.
	 * We do not need to register them.
	 */
	process_stacks();		 /* 1 - analyze stack usage */
	stack_check();			 /* 2 - announce low stacks */

	/*
	 * Execute registered functions
	 */
	reg_invoke_onemin();
    }
}

/*
 * net_restart_throttle
 * We are called from net_background() to restart any throttle interfaces.
 */

static inline void net_restart_throttle (void)
{ 
    list_header *throttled_idbs;
    list_element *element, *next_element;
    hwidbtype *hwidb;
    leveltype status;

    throttled_idbs = idb_get_listhead(HWIDBLIST_THROTTLED);
    FOR_ALL_DATA_IN_LIST_W_NEXT(throttled_idbs, element, next_element, hwidb) {
	if (hwidb->enable) {
	    status = raise_interrupt_level(NETS_DISABLE);
	    (*(hwidb->enable))(hwidb);
	    reset_interrupt_level(status);
	}
    }
}

void idb_mark_throttled (hwidbtype *hwidb)
{
    hwidb->counters.throttle_count++;
    idb_add_hwidb_to_list(hwidb, HWIDBLIST_THROTTLED);
    process_set_boolean(net_throttle_flag, TRUE);
}

void idb_mark_unthrottled (hwidbtype *hwidb)
{
    idb_remove_from_list(hwidb, HWIDBLIST_THROTTLED);
}

boolean idb_is_throttled (hwidbtype *target)
{
    list_element *element;
    hwidbtype *hwidb;

    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_THROTTLED, element, hwidb)
	if (hwidb == target)
	    return(TRUE);
    return(FALSE);
}


/*
 * net_input
 * A process to handle otherwise unknown packets.  We do this at process
 * level so our input queueing comes into play.  If we operate at
 * interrupt level we could very easily lock up the router.  We also
 * handle some known protocols that we might decide at interrupt level
 * should be offered to bridging.  Packets flagged with PAK_ABORT must
 * never be offered to bridging since we are routing that protocol on
 * other interfaces.
 */
static process net_input (void)
{
    paktype *pak;
    ulong major, minor;

    process_watch_queue(net_inputQ, ENABLE, RECURRING);
    process_watch_queue(net_abortQ, ENABLE, RECURRING);
    while (TRUE) {
	process_safe_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		switch (minor) {
		  case ABORT_QUEUE:
		    while ((pak = process_dequeue(net_abortQ)) != NULL)
			datagram_done(pak);
		    break;
		    
		  case INPUT_QUEUE:
		    while ((pak = process_dequeue(net_inputQ)) != NULL) {
			if (pak->linktype == LINK_ADDRESS)
			    reg_invoke_media_serial_hdlc_address(pak);
			else if (pak->linktype == LINK_NMP)
			    reg_invoke_local_lnm_link_nmp(pak);
			else if (pak->flags & PAK_ABORT)
			    datagram_done(pak);
			else
			    net_bridge_enqueue(pak);
		    }
		    break;

		  default:
		    errmsg(&msgsym(UNEXPECTEDQUEUE,SCHED), minor);
		    break;
		}
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}


/*
 * net_background
 * Perform a variety of network related background tasks.
 * These tasks must be performed quickly and may not block for
 * any reason.
 *
 * The tasks that are called in the net-background process are
 * time critical (e.g. interface dethrottling).
 */

process net_background (void)
{
    ulong major, minor;

    netinput_pid = process_create(net_input, "Net Input", NORMAL_STACK,
				  PRIO_HIGH);
    process_create(compute_loads, "Compute load avgs", SMALL_STACK,
		   PRIO_CRITICAL);
    process_create(net_oneminute, "Per-minute Jobs", NORMAL_STACK,
		   PRIO_NORMAL);

    process_watch_boolean(net_throttle_flag, ENABLE, RECURRING);
    process_watch_mgd_timer(&if_cstate_timer, ENABLE);
    while (TRUE) {
	/*
	 * Sleep until an interface up/down status change.  This sleep
	 * may be interrupted early if an interface is throttled or if
	 * network buffers need attention.
	 */
	process_safe_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
		/*
		 * Scan interfaces for carrier transition damping.
		 */
		cstate_check();
		break;

	      case BOOLEAN_EVENT:
		/*
		 * Restart any throttled interfaces. This process is
		 * only watching one boolean, so no demux is needed.
		 */
		process_set_boolean(net_throttle_flag, FALSE);
		net_restart_throttle();
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * raw_enqueue
 * Called from interrupt level to enqueue an input packet
 * If the protocol specific routine doesn't absorb the packet, possibly because
 * the protocol is not enabled on that interface, the packet must be enqueued
 * on netinputQ for later disposal.  Interrupt level disposal of a steady
 * stream of packets might other lock up the system.  Unbridgable packets are
 * marked with PAK_ABORT.
 */

void raw_enqueue (hwidbtype *idb, paktype *pak)
{
    if (!pak->if_input) {
	errmsg(&msgsym(NOSOURCE, LINK));
	retbuffer(pak);
	return;
    }
    GET_TIMESTAMP(idb->lastinput);	/* remember time of last input */

    /*
     * If hardware is not up, discard packet and return
     */
    if (idb->state != IDBS_UP) {
	protocol_discard(pak, TRUE);
	return;
    }

    pak->linktype = (*idb->getlink)(pak);

    /*
     * Snap the frame to alignment if required
     */
    ALIGN_DATAGRAM_CMD(pak_realign(pak, NULL));

    reg_invoke_raw_enqueue(pak->linktype, pak);
}

/*
 * netinput_enqueue
 * Enqueue a packet for the Net Input process
 */

void netinput_enqueue (paktype *pak)
{
    if (!process_enqueue(net_inputQ, pak))
	crashdump(2);
}

/*
 * protocol_discard
 * Enqueue a packet for the Net Input process after possible marking
 * it to be flushed.
 */

void protocol_discard (paktype *pak, boolean abort)
{
    if (abort) {
	if (!process_enqueue(net_abortQ, pak))
	    crashdump(2);
	return;
    }
    if (!process_enqueue(net_inputQ, pak))
	crashdump(2);
}

/*
 * process_enqueue_pak
 *
 * Enqueue an element onto a queue, and then notify any processes that are
 * interested in the contents of this queue.  This routine uses
 * protocol_discard instead of retbuffer because it is commonly called
 * from interrupt level, and needs to maintain the existing throttling
 * model.
 */
void process_enqueue_pak (watched_queue *event, paktype *pak)
{
    if (process_enqueue(event, pak))
	return;
    protocol_discard(pak, TRUE);
}

/*
 * process_requeue_pak
 *
 * Requeue an element onto a queue, and then notify any processes that are
 * interested in the contents of this queue.  This routine uses
 * protocol_discard instead of retbuffer because it is commonly called
 * from interrupt level, and needs to maintain the existing throttling
 * model.
 */
void process_requeue_pak (watched_queue *event, paktype *pak)
{
    if (process_requeue(event, pak))
	return;
    protocol_discard(pak, TRUE);
}


/*
 * loopback_enqueue
 * Enqueue a loopback packet
 */

static void loopback_enqueue (paktype *pak)
{
    ulong code;

    code = (pak->if_input->hwptr->status & (IDB_ETHER|IDB_TR|IDB_FDDI));
    reg_invoke_loop_enqueue(code, pak);
}

/*
 * net_bridge_enqueue
 * Common routine to pass an unwanted packet to the bridging code.
 * You probably should think about using datagram_done() instead.
 */

void net_bridge_enqueue (paktype *pak)
{
    (void) reg_invoke_bridge_enqueue(pak, FALSE, TRUE);
}

/*
 * net_bridge_copy
 * Make a copy of a packet and give it to the bridging software.
 * Caller intends to use the original packet for its own purposes.
 */

void net_bridge_copy (paktype *pak)
{
    (void) reg_invoke_bridge_enqueue(pak, TRUE, TRUE);
}

/*
 * net_bridge_enqueue_broadcast
 * Like net_bridge_enqueue but first ensures that the packet is a broadcast
 * or multicast.
 */

void net_bridge_enqueue_broadcast (paktype *pak)
{
    (void) reg_invoke_bridge_enqueue(pak, FALSE, FALSE);
}

/*
 * net_bridge_copy_broadcast
 * Like net_bridge_copy but first ensures that the packet is a broadcast
 * or multicast.
 */

void net_bridge_copy_broadcast (paktype *pak)
{
    (void) reg_invoke_bridge_enqueue(pak, TRUE, FALSE);
}

/*
 * net_bridge_enqueue_discard
 * 
 * Ugly default service routine to discard frames that would be passed to a
 * bridging process, if it existed
 */

static void net_bridge_enqueue_discard (paktype *pak,
					boolean copyflag,
					boolean unicast_ok)
{
    /*
     * If we were s'pposed to just copy the frame, return
     */
    if (copyflag)
	return;

    /*
     * Death to unloved frames!
     */
    datagram_done(pak);
}


/*
 * parse_interface
 * Parse interface name of the form "Ethernet 0" or "Ethernet0".
 * Recognize special interface "Null 0" or "Null0".
 */

char *parse_interface (char *arg, idbtype **idb)
{
    char *p;
    char name[TOKLEN];
    int unit;
    idbtype *ifp;
    boolean parsed = FALSE;

    /*
     * First try parsing a string like "Ethernet0"
     */
    *idb = NULL;
    p = gettoken(arg, name, TOKLEN, WORD);
    FOR_ALL_SWIDBS(ifp) {
	if (!strcasecmp(ifp->namestring, name))
	    break;
    }
    if (!ifp && !strcasecmp("null0", name))
	ifp = nullidb;

    /*
     * If no luck, try parsing for something like "Ethernet 0"
     * If this parse succeeds, advance the byte pointer beyond the unit number.
     */
    if (!ifp && p) {
	unit = parse_unsigned(p, &parsed);
	FOR_ALL_SWIDBS(ifp) {
	    if ((ifp->hwptr->unit == unit) && 
		partialmatch(ifp->hwptr->name, name))
	        break;
        }		
	if (!ifp && (unit == 0) && partialmatch("null", name))
	    ifp = nullidb;
	if (ifp) {
	    p = deblank(p);
	    while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n')
	        p++;
	}
    }

    /*
     * If we found an interface, return the idbtype pointer.
     * Return pointer to current position in parse string.
     */
    if (ifp) {
	*idb = ifp;
	return(p);
    } else
	return(arg);
}

/*
 * nullidb_init - Initialize the NULL idb structure
 *  . No input - no real hardware
 *  . No output - encapsulation always fails.
 *  . Never found in the system idb list (idbQ).
 * Can be used for routing tricks, e.g. by setting an IP static route such
 * as "ip route 127.0.0.0 null 0" we can route all traffic for that common
 * martian to the null interface.
 */

void nullidb_init (void)
{
    hwidbtype *hwidb = idb_create();

    if (!hwidb)
	return;

    nullidb = hwidb->firstsw;
    hwidb->state = IDBS_UP;		/* hardware is always up */
    nullidb->subif_state = SUBIF_STATE_UP;
    nullidb->subif_line_state = SUBIF_STATE_UP;
    hwidb->name = IFNAME_NULL;
    hwidb->unit = 0;
    idb_init_names(nullidb, TRUE);
    hwidb->enctype = ET_ARPA;		/* some sort of dumb Ethernet */
    hwidb->fast_switch_type = FS_NULL;  /* go nowhere fast */
    hwidb->soutput =(soutput_t) return_nothing;
    hwidb->reset = (reset_t) return_nothing;
    hwidb->vencap = NULL;		/* no variable encapsulator */
    hwidb->deencap = NULL;
    hwidb->getlink = (linktype_t) return_zero;/* link type always illegal */

    /*
     * Output always fails
     */
    hwidb->oqueue = (oqueue_t) return_false;
    hwidb->oqueue_dequeue = (oqueue_dequeue_t) return_null;

    hwidb->iqueue = (iqueue_t) return_nothing;
    hwidb->extract_hwaddr = (extract_hwaddr_t)return_false;
    hwidb->extract_snpa = (extract_snpa_t)return_false;
    hwidb->broadcast = (broadcast_t) return_fuz_true;
    hwidb->setencap = (setencap_t) return_nothing;
    hwidb->linestate = (linestate_t) return_true;
    hwidb->maxmtu = hwidb->maxmtu_def = MAXETHERSIZE; /* maximum MTU */
    nullidb->ip_unreach = TRUE;		/* send unreachables */
}

/*
 * show_swidb_chain
 *
 * Walk the swidb chain, showing all interfaces or a particular
 * interface.  This is an interactive routine intended for "show"
 * commands.  We walk each software IDB attached to each hardware IDB. The
 * size passed in is the maximum size of the display for the interface so
 * that we can block if necessary.  Passing a size of 0 causes a "more"
 * prompt after every interface.
 */

void show_swidb_chain (idbtype *target, void (*rtn)(idbtype *), int size)
{
    boolean onceflag;
    idbtype *swidb;
    hwidbtype *hwidb;

    onceflag = FALSE;
    FOR_ALL_HWIDBS(hwidb) {
	if (hwidb->status & IDB_DELETED)
	    continue;
        reg_invoke_consolidate_counters(hwidb);
	FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	    if (target && (target != swidb))
		continue;
	    if (onceflag) {
		if (target != NULL)
		    return;
		automore_conditional(size);
	    } else
		onceflag = TRUE;
	    (*rtn)(swidb);
	    if (automore_quit())
		return;
	}
    }
}

/*
 * show_hwidb_chain
 *
 * Walk the hwidb chain, showing all interfaces or a particular
 * interface.  This is an interactive routine intended for "show"
 * commands.
 */

void show_hwidb_chain (hwidbtype *target, void (*rtn)(hwidbtype *))
{
    boolean onceflag;
    hwidbtype *hwidb;

    onceflag = FALSE;
    FOR_ALL_HWIDBS(hwidb) {
	if (hwidb->status & IDB_DELETED)
	    continue;
	if (target && (target != hwidb))
	    continue;
	if (onceflag) {
	    if (target != NULL)
		return;
	    automore_conditional(0);
	} else
	    onceflag = TRUE;
        reg_invoke_consolidate_counters(hwidb);
	(*rtn)(hwidb);
	if (automore_quit())
	    break;
    }
}

/*
 * print_idbflags
 * display flag information for an interface.
 */

char *print_idbflags (hwidbtype *idb)
{
    if (idb->inloopback && idb->status & IDB_DISABLED)
	return("(disabled, looped)");
    else if (idb->inloopback)
	return("(looped)");
    else if (idb->status & IDB_DISABLED)
	return("(disabled)");
    else if (reg_invoke_dialer_spoofing_prot(idb))
        return("(spoofing)");
    return("");
}

/*
 * clear_counters
 * Clear counters associated with an interface, as shown by "show interface"
 */

void clear_counters (parseinfo *csb)
{
    hwidbtype *idb = hwidb_or_null(GETOBJ(idb,1));

    if (idb)  {
	if (confirm("Clear \"show interface\" counters on this interface ")) {
            reg_invoke_consolidate_counters(idb);
	    clear_idb(idb);
	    if ( idb->clear_support )
	       (*idb->clear_support)(idb);
	    errmsg(&msgsym(COUNTERS, CLEAR), common_str_interface,
		   idb->hw_namestring, user_on_stdio_str());
	}
    } else {
        if (confirm("Clear \"show interface\" counters on all interfaces ")) {
            FOR_ALL_HWIDBS(idb) {
                reg_invoke_consolidate_counters(idb);
		clear_idb(idb);
		if ( idb->clear_support )
		    (*idb->clear_support)(idb);
	    }
	    reg_invoke_cdb_clear_counters(idb);
	    errmsg(&msgsym(COUNTERS, CLEAR), "all", "interfaces", 
		   user_on_stdio_str()); 
	}
    }
}

/*
 * clear_idb
 * Does the work for "clear counters", through "show chain".
 * Note that the formatting of the lines below (in setting values
 * to 0) follows the formatting of the lines in "show_idb". This
 * is intentional and makes comaprisions for changes between the
 * two simpler. Keep it that way.
 * 
 * Note that variables that could affect routing (load, reliability, etc)
 * or "non-cumulative" (last output, etc.) are *not* affected.
 *
 * Also called by "show_idb" if it is suspected that counters have wrapped.
 */

void clear_idb (hwidbtype *idb)
{
    int			i;
    idb_si_counters_t	*sic;
    idbtype             *swidb;
    srb_vector_table_t  *srbV_p;


    if (!idb)			/* Sanity check, but return quietly */
	return;
    /*
     * If the "show interface" counters for saving the stats over a
     * "clear counters" command is NULL, allocate it here.
     */
    if (idb->si_counters == NULL) {
	idb->si_counters = malloc(sizeof(idb_si_counters_t));
	if (idb->si_counters == NULL)
	    return;
    } 
    
    GET_TIMESTAMP(idb->si_last_cclear); /* Set time of last clear */
    sic = idb->si_counters;
    /*
     * First, set up the computed fields and such.
     */
    sic->slow_in = idb->counters.inputs;
    sic->fast_in = idb_fast_in(idb);
    sic->auto_in = idb_auto_in(idb);

    sic->slow_out = idb->counters.outputs;
    sic->fast_out = idb_fast_out(idb);
    sic->auto_out = idb_auto_out(idb);

    sic->rx_fast_cumbytes = idb_rx_fast_cumbytes(idb);
    sic->rx_auto_cumbytes = idb_rx_auto_cumbytes(idb);
    sic->tx_fast_cumbytes = idb_tx_fast_cumbytes(idb);
    sic->tx_auto_cumbytes = idb_tx_auto_cumbytes(idb);
    sic->rx_slow_cumbytes = idb->counters.rx_cumbytes;
    sic->tx_slow_cumbytes = idb->counters.tx_cumbytes;

    sic->inputs = idb_inputs(idb);
    sic->input_broad = idb->counters.input_broad;
    sic->input_error = idb->counters.input_error;
    sic->input_crc = idb->counters.input_crc;
    sic->input_frame = idb->counters.input_frame;
    sic->input_dribble = idb->counters.input_dribble;
    sic->input_overrun = idb->counters.input_overrun;
    sic->input_resource = idb->counters.input_resource;
    sic->input_drops = idb->counters.input_drops;
    sic->input_abort = idb->counters.input_abort;

    sic->outputs = idb_outputs(idb);
    sic->output_error = idb->counters.output_error;
    sic->output_collision = idb->counters.output_collision;
    sic->output_underrun = idb->counters.output_underrun;
    sic->output_resource = idb->counters.output_resource;
    sic->output_nobuffers = idb->counters.output_nobuffers;
    sic->output_total_drops = idb->counters.output_total_drops;
    
    for (i = 0; i < PRIORITY_MAXTYPE; i++) {
	sic->output_drops[i] = idb->counters.output_drops[i];
	sic->output_dequeued[i] = idb->counters.output_dequeued[i];
    }

    sic->resets = idb->counters.resets;
    sic->runts = idb->counters.runts;
    sic->giants = idb->counters.giants;
    sic->transitions = idb->counters.transitions;
    sic->parity = idb->counters.parity;

    sic->rx_cumbytes = idb_rx_cumbytes(idb);
    sic->tx_cumbytes = idb_tx_cumbytes(idb);

    for (i = 0; i < ACCT_PROTO_LAST; i++) {
        sic->rx_frames[i]
	    = idb->counters.rx_frames[i] + idb->counters.inputs_fast[i] + 
	      idb->counters.inputs_auto[i];
	sic->tx_frames[i]
	    = idb->counters.tx_frames[i] + idb->counters.outputs_fast[i] +
		idb->counters.outputs_auto[i];
	sic->rx_bytes[i]
	    = idb->counters.rx_bytes[i]  + idb->counters.rx_cumbytes_fast[i] +
		idb->counters.rx_cumbytes_auto[i];
	sic->tx_bytes[i]
	    = idb->counters.tx_bytes[i]  + idb->counters.tx_cumbytes_fast[i] +
		idb->counters.tx_cumbytes_auto[i];
    }

    sic->throttle_count = idb->counters.throttle_count; 

    /*
     * Serial-interfaces specials. These will eventually be moved into
     * serial-specific blocks, someday, somewhere.
     */
    idb->si_rts_timeout_count = idb->rts_timeout_count;
    idb->si_half_duplex_violation = idb->half_duplex_violation;
    idb->si_dma_memory_error = idb->dma_memory_error;
    idb->si_pak_swapped_out = idb->pak_swapped_out;

    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
            srbV_p->srb_sb_set_si_counters(swidb);
        }
    }

    /*
     * Clear ether fields
     */
    if (idb->status & IDB_ETHER) {
	ONE_ITEM_LIST(tmp, ETHER_HW_CLEAR_STATS, 1, 0);
	(void)idb_hw_state_config(idb, IDB_CONTROL_SET_STATE, &tmp_list);
    }

    /*
     * Clear serial fields
     */
    if (idb->status & IDB_SERIAL) {
	ONE_ITEM_LIST(tmp, SERIAL_HW_CLEAR_STATS, 1, 0);
	(void)idb_hw_state_config(idb, IDB_CONTROL_SET_STATE, &tmp_list);
    }

    /*
     * Clear FDDI fields
     */
    if (idb->status & IDB_FDDI) {
	ONE_ITEM_LIST(tmp, FDDI_HW_CLEAR_STATS, 1, 0);
	(void)idb_hw_state_config(idb, IDB_CONTROL_SET_STATE, &tmp_list);
    }

    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);

    reg_invoke_clear_counters(idb);
}

/*
 * show_isdn_idb
 * Print out information about the B Channels.  The D channel idb is
 * passed in along with the starting B channel number.  Need to parse
 * the ending channel number (if there is one), and display the B idbs
 * using show_idb.
 */

static void show_isdn_idb (idbtype *d_swidb, int start, int end)
{
    hwidbtype *d_idb;
    int i;

    d_idb = d_swidb->hwptr;
    if (end == 0)
        end = start;

    /*
     * If no bound is specified, display D channel idb.
     */
    if (start == 0) {
	show_idb(d_swidb);
	return;
    }

    /*
     * The isdn_info structure contains the B channel idb pointers 
     * in an array which can is indexed off of the D-channel.
     */

    automore_enable(NULL);
    for (i = (start-1); i < end; i++) {
        if (d_idb->isdn_info->b_chan_idb[i] == NULL) {
            /*
             * B16 does not exist for E1 but I still need to
             * be able to support "show interface sx/y:15 bchan 1 31".
             * In order to support this I have to skip B16 without
             * returning an error. If the user attempts to view
             * B16 alone there is nothing returned except the prompt.
             */
            if ((d_idb->type == IDBTYPE_E1) && (i == E1_D_CHAN_PRI))
                continue;
            printf("\n%%Attempting to access a non-ISDN interface, aborting.");
            break;
        }
        show_idb(d_idb->isdn_info->b_chan_idb[i]->firstsw);
    }

    automore_disable();
}

/*
 * show_idb_crb
 * Display routing and bridging information for one network interface.
 */

static void show_idb_crb (hwidbtype *hwidb)
{
    int indx, printed;
    idbtype *swidb;

    if (!hwidb->standby && hwidb->state == IDBS_ADMINDOWN)
	return;

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	printf("\n\n%s", swidb->namestring, swidb->description);

	printed = 0;
	for (indx = 0; indx < MAX_RP_LTYPES; indx++) {
	    if (ROUTING_IF(swidb, rp_ltypes[indx])) {
		if (printed == 0) {
		    printf("\n\n Routed protocols on %s:", swidb->namestring);
		    printf("\n  %9s", print_linktype(rp_ltypes[indx]));
		    printed++;
		} else if (printed == 4) {
		    printf("\n  %9s", print_linktype(rp_ltypes[indx]));
		    printed = 1;
		} else {
		    printf("  %9s", print_linktype(rp_ltypes[indx]));
		    printed++;
		}
	    }
	}

	if (hwidb->tbridge_on_hwidb) {
	    if (swidb->tbridge_on_swidb) {

		printed = 0;
		for (indx = 0; indx < MAX_RP_LTYPES; indx++) {
		    if (BRIDGING_IF(swidb, rp_ltypes[indx])) {
			if (printed == 0) {
			    printf("\n\n Bridged protocols on %s:",
				   swidb->namestring);
			    printf("\n  %9s", print_linktype(rp_ltypes[indx]));
			    printed++;
			} else if (printed == 4) {
			    printf("\n  %9s", print_linktype(rp_ltypes[indx]));
			    printed = 1;
			} else {
			    printf("  %9s", print_linktype(rp_ltypes[indx]));
			    printed++;
			}
		    }
		}
	    } else {
		printf("\n\n Not bridging this sub-interface.");
	    }
	} else if (swidb->tbridge_on_swidb) {
	    printf(" *ERROR* bridging on sub-interface but NOT on interface");
	}

	reg_invoke_tbridge_smf_show(swidb);
    }
}

/*
 * show_idb_irb
 * Display routing and bridging information for one network interface.
 */

static void show_idb_irb (hwidbtype *hwidb)
{
    int indx, printed;
    idbtype *swidb;
    if (!hwidb->standby && hwidb->state == IDBS_ADMINDOWN)
	return;
    
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	printf("\n\n%s", swidb->namestring, swidb->description);
	
	printed = 0;
	for (indx = 0; indx < MAX_RP_LTYPES; indx++) {
	    if (ROUTING_IF(swidb, rp_ltypes[indx])) {
		if (printed == 0) {
		    printf("\n\n Routed protocols on %s:",swidb->namestring);
		    printf("\n  %9s", print_linktype(rp_ltypes[indx]));
		    printed++;
		} else if (printed == 4) {
		    printf("\n  %9s", print_linktype(rp_ltypes[indx]));
		    printed = 1;
		} else {
		    printf("  %9s", print_linktype(rp_ltypes[indx]));
		    printed++;
		}
	    }
	}
	
	if (hwidb->tbridge_on_hwidb) {
	    if (swidb->tbridge_on_swidb) {
		
		printed = 0;
		for (indx = 0; indx < MAX_RP_LTYPES; indx++) {
		    /*
		     * Note that since we reg_invoke protcol specific
		     * routine to check if we are bridging, we may
		     * fail to verify it for protocols not include
		     * in the subset image, even we in fast are
		     * bridging this protocol.
		     */
		    if (BRIDGING_IF(swidb, rp_ltypes[indx])) {
			if (printed == 0) {
			    printf("\n\n Bridged protocols on %s:",
				   swidb->namestring);
			    printf("\n  %9s", print_linktype(rp_ltypes[indx]));
			    printed++;
			} else if (printed == 4) {
			    printf("\n  %9s", print_linktype(rp_ltypes[indx]));
			    printed = 1;
			} else {
			    printf("  %9s", print_linktype(rp_ltypes[indx]));
			    printed++;
			}
		    }
		}
	    } else {
		printf("\n\n Not bridging this sub-interface.");
	    }
	} else if (swidb->tbridge_on_swidb) {
	    printf(" *ERROR* bridging on sub-interface but NOT on interface");
	}
	
	reg_invoke_tbridge_smf_show(swidb);
    }
}


/*
 * show_idb_accounting
 * Display link level statistics for one network interface
 */

    static char *const acct_proto_name[ACCT_PROTO_LAST] = {
	"Other", "IP", "DECnet", "XNS", "CLNS", "Appletalk",
	"IPX", "Apollo", "Vines", "Trans. Bridge", "SR Bridge",
	"DEC MOP", "LAN Manager", "Serial Tunnel",
	"Spanning Tree", "ARP", "HP Probe", "FR Switching", "CDP",
        "Block Serial Tunnel", "VTP"
      };

static void show_idb_accounting (hwidbtype *idb)
{
    int			i;
    boolean		shown_any;
    idbtype		*swidb;
    idb_si_counters_t	*sic;

    if (!idb->standby && idb->state == IDBS_ADMINDOWN) {
	printf("\nInterface %s is disabled\n", idb->hw_namestring);
	return;
    }

    shown_any = FALSE;

    /*
     * Now before reading the count of autonomous datagrams, poll the
     * bridge cache entries on the cbus, to include their counts
     */
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
        reg_invoke_bridge_poll_interface_stats(swidb);
    }

    printf("\n%s %s\n%-24s %-10s %-10s %-10s %-10s", idb->hw_namestring,
	   idb->firstsw->description,
	   "Protocol", "Pkts In", "Chars In", "Pkts Out", "Chars Out");
    sic = idb_get_counter_info(idb, IDB_COUNTER_SI_CNT_PTR);
    for (i = pkt_debug ? 0 : 1; i < ACCT_PROTO_LAST; i++)
	if (    (idb->counters.rx_frames[i]+idb->counters.inputs_fast[i]+
		idb->counters.inputs_auto[i])-sic->rx_frames[i]+
		(idb->counters.rx_bytes[i]+idb->counters.rx_cumbytes_fast[i]+
		idb->counters.rx_cumbytes_auto[i])-sic->rx_bytes[i]+
		(idb->counters.tx_frames[i]+idb->counters.outputs_fast[i]+
		idb->counters.outputs_auto[i])-sic->tx_frames[i]+
		(idb->counters.tx_bytes[i]+idb->counters.tx_cumbytes_fast[i]+
		idb->counters.tx_cumbytes_auto[i])-sic->tx_bytes[i] ) {
	    shown_any = TRUE;
	    printf("\n%-24s %-10lu %-10lu %-10lu %-10lu",
		acct_proto_name[i],
		(idb->counters.rx_frames[i]+idb->counters.inputs_fast[i]+
		idb->counters.inputs_auto[i])-sic->rx_frames[i],
		(idb->counters.rx_bytes[i]+idb->counters.rx_cumbytes_fast[i]+
		idb->counters.rx_cumbytes_auto[i])-sic->rx_bytes[i],
		(idb->counters.tx_frames[i]+idb->counters.outputs_fast[i]+
		idb->counters.outputs_auto[i])-sic->tx_frames[i],
		(idb->counters.tx_bytes[i]+idb->counters.tx_cumbytes_fast[i]+
		idb->counters.tx_cumbytes_auto[i])-sic->tx_bytes[i]);
	}
    if (!shown_any) 
      printf("\nNo traffic sent or received on this interface.");
}

/*
 * show_idb_switching
 * Display link level switching statistics for one network interface
 */

static void
show_idb_switching (hwidbtype *idb)
{
    int			i;
    boolean		shown_any;
    idbtype		*swidb;
    idb_si_counters_t	*sic;

    if (!idb->standby && idb->state == IDBS_ADMINDOWN) {
	printf("\nInterface %s is disabled\n", idb->hw_namestring);
	return;
    }

    shown_any = FALSE;

    /*
     *  Now before reading the count of autonomous datagrams, poll the
     *  bridge cache entries on the cbus, to include their counts.
     */
    FOR_ALL_SWIDBS_ON_HW (idb, swidb) {
        reg_invoke_bridge_poll_interface_stats (swidb);
    }

    sic = idb_get_counter_info(idb, IDB_COUNTER_SI_CNT_PTR);
    printf("\n%s %s", idb->hw_namestring, idb->firstsw->description);
    printf("\n%-24s %-10lu", "Throttle count:",
	   idb->counters.throttle_count - sic->throttle_count);
    printf("\n%-13s %-10s %-10s %-10s %-10s %-10s", 
	   "Protocol", "Path", "Pkts In", "Chars In", "Pkts Out",
	   "Chars Out"); 
    for (i = 0; i < ACCT_PROTO_LAST; i++) {
	if (    (idb->counters.rx_frames[i]+idb->counters.inputs_fast[i]+
		idb->counters.inputs_auto[i])-sic->rx_frames[i]+
		(idb->counters.rx_bytes[i]+idb->counters.rx_cumbytes_fast[i]+
		idb->counters.rx_cumbytes_auto[i])-sic->rx_bytes[i]+
		(idb->counters.tx_frames[i]+idb->counters.outputs_fast[i]+
		idb->counters.outputs_auto[i])-sic->tx_frames[i]+
		(idb->counters.tx_bytes[i]+idb->counters.tx_cumbytes_fast[i]+
		idb->counters.tx_cumbytes_auto[i])-sic->tx_bytes[i] ) {
	    shown_any = TRUE;
	    printf("\n%-13s %-10s %-10lu %-10lu %-10lu %-10lu",
		   acct_proto_name[i], "Process",
		   idb->counters.rx_frames[i], idb->counters.rx_bytes[i],
		   idb->counters.tx_frames[i], idb->counters.tx_bytes[i]);
	    printf("\n%-24s %-10lu", "Cache misses",
		   idb->counters.cache_miss[i]);
	    printf("\n%-24s %-10lu %-10lu %-10lu %-10lu",
		   "Fast",
		   idb->counters.inputs_fast[i],
		   idb->counters.rx_cumbytes_fast[i],
		   idb->counters.outputs_fast[i],
		   idb->counters.tx_cumbytes_fast[i]);
	    printf("\n%-24s %-10lu %-10lu %-10lu %-10lu",
		   "Auton/SSE",
		   idb->counters.inputs_auto[i],
		   idb->counters.rx_cumbytes_auto[i],
		   idb->counters.outputs_auto[i],
		   idb->counters.tx_cumbytes_auto[i]);
	}
    }
    if (!shown_any) 
	printf("\nNo traffic sent or received on this interface.");
}

/*
 * show_idb_stats
 * Display link level statistics for one network interface
 */

static void
show_idb_stats (hwidbtype *idb)
{
    idbtype		*swidb;
    idb_si_counters_t	*sic;


    if (!idb->standby && idb->state == IDBS_ADMINDOWN) {
	printf("\nInterface %s is disabled\n", idb->hw_namestring);
	return;
    }
    sic = idb_get_counter_info(idb, IDB_COUNTER_SI_CNT_PTR);
    printf("\n%s\n%-24s %-10s %-10s %-10s %-10s", idb->hw_namestring,
	   "Switching path", "Pkts In", "Chars In", "Pkts Out", "Chars Out");
    printf("\n%-24s %-10lu %-10lu %-10lu %-10lu",
		"Processor",
		idb->counters.inputs - sic->slow_in, 
		idb->counters.rx_cumbytes - sic->rx_slow_cumbytes,
		idb->counters.outputs - sic->slow_out, 
		idb->counters.tx_cumbytes - sic->tx_slow_cumbytes);
    printf("\n%-24s %-10lu %-10lu %-10lu %-10lu",
		"Route cache",
		idb_fast_in(idb) - sic->fast_in,
		idb_rx_fast_cumbytes(idb) - sic->rx_fast_cumbytes,
		idb_fast_out(idb) - sic->fast_out,
		idb_tx_fast_cumbytes(idb) - sic->tx_fast_cumbytes);

    /*
     * Now before reading the count of autonomous datagrams,
     * poll the bridge cache entries, to include their counts.
     */

    FOR_ALL_SWIDBS_ON_HW (idb, swidb) {
        reg_invoke_bridge_poll_interface_stats (swidb);
    }

    /*
     * Invoke routine to display auxilary switched traffic counts.
     * Autonomous switching counts and SSE switched counts show up here.
     * Also, IP flow switching counts show up here.
     */
    reg_invoke_show_auxiliary_switch_counts(idb);

    printf("\n%-24s %-10lu %-10lu %-10lu %-10lu",
		"Total",
		idb_inputs(idb) - sic->inputs,
		idb_rx_cumbytes(idb) - sic->rx_cumbytes,
		idb_outputs(idb) - sic->outputs,
		idb_tx_cumbytes(idb) - sic->tx_cumbytes);
}

/*
 * show_interface
 * Print out information on the network interfaces
 */

void show_interface (parseinfo *csb)
{
    idbtype *swidb;
    hwidbtype *hwidb;

    swidb = GETOBJ(idb,1);
    hwidb = swidb ? swidb->hwptr : NULL;

    automore_enable(NULL);
    if (GETOBJ(int,13)) {
	show_hwidb_chain(hwidb, show_idb_irb);
    } else if (GETOBJ(int,11)) {
	show_hwidb_chain(hwidb, show_idb_crb);
    } else if (GETOBJ(int,10)) {
	show_hwidb_chain(hwidb, show_idb_accounting);
    } else if (GETOBJ(int,9)) {
	show_hwidb_chain(hwidb, show_idb_stats);
    } else if (GETOBJ(int, 12)) {
	show_hwidb_chain(hwidb, show_idb_switching);
    } else {
	if (hwidb && hwidb->isdn_info) {
	    if ((hwidb->type == IDBTYPE_BRI) && (GETOBJ(int,2)))
	        show_isdn_idb(swidb, GETOBJ(int,2), GETOBJ(int,3));
	    else
	        show_isdn_idb(swidb, GETOBJ(int,4), GETOBJ(int,5));
	} else{
	    show_swidb_chain(swidb, show_idb, 0);
	}
    }
    automore_disable();
}

/*
 * show_idb
 * Display link level statistics for one network interface
 */

void show_idb (idbtype *idb)
{
    char		sepchar;
    char		tempbuff[40];
    hwidbtype		*hwidb = idb->hwptr;
    idb_si_counters_t	*sic;

    printf("\n%s is %s", idb->namestring, print_idbstate(idb));
    printf("%s", reg_invoke_dialer_spoofing_int(hwidb) ? 
		 " (spoofing), " : ", ");
    printf("line protocol is %s %s", interface_up(idb) ? "up" : "down",
	      print_idbflags(hwidb));
    reg_invoke_show_int(hwidb);
    if (hwidb->status & IDB_NOHARDWARE) 
	printf("\n  Hardware has been removed");
    printf("\n  Hardware is %s", hwidb->typestring);
    if (hwidb->status & (IDB_IEEEMASK | IDB_BVI))
	printf(", address is %e (bia %e)", hwidb->hardware, hwidb->bia);
    if (idb->description)
	printf("\n  Description: %s", idb->description);

    if (idb->ip_address)
	printf("\n  Internet address is %i%m",
	       idb->ip_address, idb->ip_nets_mask);
    if (idb->ip_unnumbered)
	printf("\n  Interface is unnumbered.  Using address of %s (%i)",
	       idb->ip_unnumbered->namestring, idb->ip_unnumbered->ip_address);

    if (!is_subinterface(idb)) {
	if (hwidb->secondary) {
	    printf("\n  Backup interface %s, kickin load",
		   hwidb->secondary->hw_namestring);
	    if (hwidb->backup_state & BACKUP_IN_LOAD) {
		printf(" %d%%", (hwidb->in_load*100+128)/255);
	    } else {
		printf(" not set");
	    }
	    printf(", kickout load");
	    if (hwidb->backup_state & BACKUP_OUT_LOAD) {
		printf(" %d%%", (hwidb->out_load*100+128)/255);
	    } else {
		printf(" not set");
	    }
	    printf("\n      failure delay");
	    if (hwidb->backup_state & BACKUP_IN_DELAY) {
		printf(" %d sec", hwidb->in_delay);
	    } else {
		printf(" not set");
	    }
	    printf(", secondary disable delay");
	    if (hwidb->backup_state & BACKUP_OUT_DELAY) {
		printf(" %d sec", hwidb->out_delay);
	    } else {
		printf(" not set");
	    }
	}
    } else {
	if (idb->backup) {
	    printf("\n  Backup interface %s, failure delay %u sec, "
		   "restore delay %u sec",
		   idb->backup->hw_namestring, idb->down_delay, idb->up_delay);
	}
    }
    if (!is_subinterface(idb)) {
        if (is_atm(hwidb)) {
            printf("\n  MTU %d bytes, sub MTU %d, BW %ld Kbit, DLY %ld usec, "
                   "rely %d/255, load %d/255",
                   hwidb->maxmtu, idb->sub_mtu, idb->visible_bandwidth,
                   10*idb->delay,hwidb->reliability, hwidb->load);
        } else {
            printf("\n  MTU %d bytes, BW %ld Kbit, DLY %ld usec, "
                   "rely %d/255, load %d/255",
                   hwidb->maxmtu, idb->visible_bandwidth,
                   10*idb->delay,hwidb->reliability, hwidb->load);
 
        }

    } else {
	printf("\n  MTU %d bytes, BW %ld Kbit, DLY %ld usec, "
	       "rely %d/255, load %d/255",
	       idb->sub_mtu, idb->visible_bandwidth,
	       10*idb->delay,hwidb->reliability, hwidb->load);
    }

    /* 
     * Display the encapsulation, along with any 
     * protocol specific attributes. i.e IETF etc.
     */
    reg_invoke_encaps_show_protocol_attr(hwidb->enctype, idb);

    if (!is_subinterface(idb)) {
	boolean hssi_loop_req;
	ONE_ITEM_LIST(tmp, SERIAL_HW_HSSI_LOOP_REQ, sizeof(boolean), 0);

	(*hwidb->state_config)(hwidb, IDB_CONTROL_READ_CONFIG, &tmp_list);
	hssi_loop_req = tmp_item.u.l_value;
	
	if (hwidb->ext_loop_type && hssi_loop_req)
	    /*
	     * Make it fit on a line.
	     */
	    printf("\n   ");
	else
	    printf(",");
	switch (hwidb->type) {
	case IDBTYPE_HSSI:
	    printf(" loopback %sset", hwidb->loopback ? "" :
		   hwidb->ext_loop_type ? "" :
		   hssi_loop_req ? "" : "not ");
	    switch (hwidb->ext_loop_type) {
	    case HSSI_LOOP_OFF:    break;
	    case HSSI_LOOP_DTE:    printf(" (dte)"); break;
	    case HSSI_LOOP_LINE:   printf(" (line)"); break;
	    case HSSI_LOOP_REMOTE: printf(" (remote)"); break;
	    }
	    if (hssi_loop_req)
		printf(" (external request)");
	    break;
	case IDBTYPE_T1:
	case IDBTYPE_E1:
	    if (hwidb->ext_loop_type == DSX1_LOOP_REMOTE)
		printf(" loopback set (remote%s)",
		       hwidb->far_end_looped ? "" : " pending");
	    else if (hwidb->far_end_looped)
		printf(" loopback not set (remote removal pending)");
	    else 
		printf(" loopback %sset", hwidb->loopback ? "" :
		       hwidb->loopback ? "" : "not ");
	    break;
	case IDBTYPE_FEIP:
	    printf(" loopback %sset", hwidb->loopback ? "" : "not ");
	    switch (hwidb->ext_loop_type) {
	    case FEIP_LOOP_DEC:   printf(" (dec)"); break;
	    case FEIP_LOOP_MICRO_LINEAR: printf(" (micro linear)"); break;
	    }
	    break;
	default:
	    printf(" loopback %sset", hwidb->loopback ? "" : "not ");
	    break;
	}
	if (hwidb->periodic) {
	    printf(", keepalive ");
	    if (hwidb->nokeepalive)
		printf("not set");
	    else
		printf("set (%d sec)", hwidb->keep_period);
	}
	if (hwidb->type == IDBTYPE_FEIP) {
	    N_ITEM_LIST(tmp, 2);

	    item_desc_init(&tmp_item[0], ETHER_HW_FEIP_FULLDUPLEX,
			   sizeof(ulong));

	    item_desc_init(&tmp_item[1], ETHER_HW_MEDIA_TYPE, sizeof(ulong)); 

	    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_CONFIG, &tmp_list);

	    if (tmp_item[0].u.l_value)
		printf(", fdx");
	    else
		printf(", hdx");
	    if (tmp_item[1].u.l_value == FEIP_RJ45)
		printf(", 100BaseTX/FX");
	    else if (tmp_item[1].u.l_value == FEIP_MII)
		printf(", MII");
	}
	if (hwidb->carrierdelay != hwidb->carrierdelay_def) {
	    printf("\n  Carrier delay is ");
            if (hwidb->carrierdelay < ONESEC)
                printf("%d msec", hwidb->carrierdelay);
            else
                printf("%d sec", (hwidb->carrierdelay / ONESEC));
        }

	if (hwidb->status & IDB_SERIAL) {
	    N_ITEM_LIST(tmp, 6);

	    item_desc_init(&tmp_item[0], SERIAL_HW_SERIAL_RESTARTDELAY,
			   sizeof(ulong));

	    item_desc_init(&tmp_item[1], SERIAL_HW_SERIAL_RESTARTDELAY_DEF,
			   sizeof(ulong));

            if (hwidb->dte_interface) {
                item_desc_init(&tmp_item[2], SERIAL_HW_RTS_TIMEOUT,
                                sizeof(ulong));
                item_desc_init(&tmp_item[3], SERIAL_HW_RTS_DROP_DELAY,
                                sizeof(ulong));
                item_desc_init(&tmp_item[4], SERIAL_HW_CTS_DROP_TIMEOUT,
                                sizeof(ulong));
                item_desc_init(&tmp_item[5], SERIAL_HW_TRANSMIT_DELAY,
                                sizeof(ulong));
            } else {
                item_desc_init(&tmp_item[2], SERIAL_HW_CTS_DELAY,
                                sizeof(ulong));
                item_desc_init(&tmp_item[3], SERIAL_HW_DCD_TXSTART_DELAY,
                                sizeof(ulong));
                item_desc_init(&tmp_item[4], SERIAL_HW_DCD_DROP_DELAY,
                                sizeof(ulong));
                item_desc_init(&tmp_item[5], SERIAL_HW_TRANSMIT_DELAY,
                                sizeof(ulong));
            }

	    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_CONFIG, &tmp_list);

            /* Print out serial half-duplex stuff */
            if (hwidb->hdx_flags & HDX_FLAG_ON) {
                printf("\n    Half-duplex enabled.");
                if (hwidb->dte_interface) {
                    printf("\n      %s %u milliseconds", "rts-timeout", tmp_item[2].u.l_value);
                    if (hwidb->type == IDBTYPE_CD2430) {
                        printf("\n      %s %u milliseconds", "rts-drop-delay", tmp_item[3].u.l_value);
                        printf("\n      %s %u milliseconds", "cts-drop-timeout", tmp_item[4].u.l_value);
                        printf("\n      %s %u milliseconds", "transmit-delay", tmp_item[5].u.l_value);
                    }

                    if (hwidb->state != IDBS_ADMINDOWN) {
                        printf("\n      Errors - %lu rts timeout, "
			       "%lu half duplex violation",
                            (hwidb->rts_timeout_count -
			     hwidb->si_rts_timeout_count),
                            (hwidb->half_duplex_violation -
			     hwidb->si_half_duplex_violation));
                    }
                }
                else {
                    printf("\n      %s %u milliseconds", "cts-delay", tmp_item[2].u.l_value);
                    if (hwidb->type == IDBTYPE_CD2430) {
                        printf("\n      %s %u milliseconds", "dcd-txstart-delay", tmp_item[3].u.l_value);
                        printf("\n      %s %u milliseconds", "dcd-drop-delay", tmp_item[4].u.l_value);
                        printf("\n      %s %u milliseconds", "transmit-delay", tmp_item[5].u.l_value);
                    }
                    if (hwidb->state != IDBS_ADMINDOWN) {
                        printf("\n      Errors - %lu half duplex violation",
                            (hwidb->half_duplex_violation -
			     hwidb->si_half_duplex_violation));
                    }
                }
            }
            else if (hwidb->enctype == ET_BSTUN) {
                printf("\n    Full-duplex enabled.");
            }

	    if ((hwidb->serial_pulseinterval) ||
		(tmp_item[0].u.l_value != tmp_item[1].u.l_value)) {
		boolean disp = FALSE;
		printf("\n  ");
		if (hwidb->serial_pulseinterval) {
		    printf("DTR is pulsed for %d seconds on reset",
			   hwidb->serial_pulseinterval/ONESEC);
		    disp = TRUE;
		}
		if (tmp_item[0].u.l_value != tmp_item[1].u.l_value) {
		    if (disp) printf(", ");
		    printf("Restart-Delay is %d secs",
			   tmp_item[0].u.l_value/ONESEC);
		}
	    }
	}
	if (hwidb->output_if_hw != hwidb)
	    printf("\n  Transmit interface is %s", 
		   idb->output_if->namestring);
    }
    if (idb->arp_arpa || idb->arp_probe || idb->arp_snap || idb->arp_smds) {
	printf("\n  ARP type");
	sepchar = ':';
	if (idb->arp_arpa) {
	    printf("%c ARPA",sepchar);
	    sepchar = ',';
	}
	if (idb->arp_snap) {
	    printf("%c SNAP",sepchar);
	    sepchar = ',';
	}
	if (idb->arp_probe) {
	    printf("%c PROBE",sepchar);
	    sepchar = ',';
	}
	
  	if (idb->arp_smds) {
  	    printf("%c SMDS",sepchar);
	    sepchar = ',';
	}
	if (idb->arp_snap || idb->arp_arpa || idb->arp_smds) {
	    printf("%c ARP Timeout ",sepchar);
	    print_ticks_in_dhms(idb->arp_timeout);
	}
    }
    sic = idb_get_counter_info(hwidb, IDB_COUNTER_SI_CNT_PTR);
    if (!is_subinterface(idb)) {
	if (hwidb->show_support)
	    (*hwidb->show_support)(hwidb);
	printf("\n  Last input "); 
	print_dhms(hwidb->lastinput);
	printf(", output ");
	print_dhms(hwidb->lastoutput);
	printf(", output hang ");
	print_dhms(hwidb->lastreset);
	printf("\n  Last clearing of \"show interface\" counters ");
	print_dhms(hwidb->si_last_cclear);
	holdq_display(hwidb);
	if (hwidb->datarate_interval == 0)
	    sprintf(tempbuff, "30 second");
	else if (hwidb->datarate_interval & 1)
	    sprintf(tempbuff, "%d minute", (hwidb->datarate_interval + 1) / 2);
	else
	    sprintf(tempbuff, "%d minutes, 30 seconds",
		    (hwidb->datarate_interval + 1) / 2);
	printf("\n  %s input rate %lu bits/sec, %lu packets/sec", tempbuff,
	       hwidb->counters.rx_datarate*1000, hwidb->counters.rx_packetrate);
	printf("\n  %s output rate %lu bits/sec, %lu packets/sec", tempbuff,
	       hwidb->counters.tx_datarate*1000, hwidb->counters.tx_packetrate);
	printf("\n     %lu packets input, %lu bytes, %lu no buffer",
	       (idb_inputs(hwidb) - sic->inputs),
	       (idb_rx_cumbytes(hwidb) - sic->rx_cumbytes),
	       (hwidb->counters.output_nobuffers
		-  sic->output_nobuffers));
	printf("\n     Received %lu broadcasts, %lu runts, %lu giants",
	       (hwidb->counters.input_broad - sic->input_broad),
	       (hwidb->counters.runts - sic->runts),
	       (hwidb->counters.giants - sic->giants));
	printf(", %d%s throttles", 
               (hwidb->counters.throttle_count - sic->throttle_count),
	       (idb_is_throttled(hwidb) ? "*" : ""));
	if (hwidb->status & IDB_HSSI) {
	    printf("\n              %lu parity",
		   (hwidb->counters.parity - sic->parity));
	}
	printf("\n     %lu input errors, %lu CRC, %lu frame, %lu overrun,",
               (hwidb->counters.input_error - sic->input_error),
               (hwidb->counters.input_crc - sic->input_crc),
               (hwidb->counters.input_frame - sic->input_frame),
	       (hwidb->counters.input_overrun - sic->input_overrun));
	/* HIP cannot report ignore correctly */
	if (!((hwidb->status&IDB_HSSI) && (hwidb->status&IDB_CBUS))
	    || IS_VIP(hwidb))
	  printf(" %lu ignored,",
		 (hwidb->counters.input_resource - sic->input_resource));
	printf(" %lu abort", (hwidb->counters.input_abort - sic->input_abort));
	if (hwidb->type == IDBTYPE_FEIP) {
	    N_ITEM_LIST(tmp, 2);

	    item_desc_init(&tmp_item[0], ETHER_HW_FEIP_WATCHDOG_STAT,
			   sizeof(ulong));

	    item_desc_init(&tmp_item[1], ETHER_HW_FEIP_MULTICAST_STAT,
			   sizeof(ulong));

	    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_STATE, &tmp_list);

	    printf("\n     %lu watchdog, %lu multicast",
		   tmp_item[0].u.l_value,  /* watchdog */
		   tmp_item[1].u.l_value); /* multicast */
	}
	if (hwidb->status & IDB_ETHER)
	    printf("\n     %lu input packets with dribble condition detected",
		   (hwidb->counters.input_dribble - sic->input_dribble));
	printf("\n     %lu packets output, %lu bytes, %lu underruns",
	       (idb_outputs(hwidb) - sic->outputs),
	       (idb_tx_cumbytes(hwidb) - sic->tx_cumbytes),
	       (hwidb->counters.output_underrun - sic->output_underrun));
	if (hwidb->status & IDB_HSSI) {
	    ONE_ITEM_LIST(tmp, SERIAL_HW_APPLIQUE_STAT, sizeof(ulong), 0);

	    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_STATE, &tmp_list);
	    printf("\n     %lu output errors, %lu applique, "
		   "%lu interface resets",
		   (hwidb->counters.output_error - sic->output_error),
		   tmp_item.u.l_value,
		   (hwidb->counters.resets - sic->resets));
	} else {
	    int coll_count;
	    coll_count = hwidb->counters.output_collision -
			  sic->output_collision;

	    if (hwidb->status & IDB_ETHER) {
		N_ITEM_LIST(tmp, 3);
		
		item_desc_init(&tmp_item[0],
			       ETHER_HW_OUTPUT_ONE_COLLISION_STAT,
			       sizeof(ulong));

		item_desc_init(&tmp_item[1],
			       ETHER_HW_OUTPUT_MORE_COLLISION_STAT,
			       sizeof(ulong));

		item_desc_init(&tmp_item[2],
			       ETHER_HW_OUTPUT_EXCESSIVE_COLLISION_STAT,
			       sizeof(ulong));

		(*hwidb->state_config)(hwidb, IDB_CONTROL_READ_STATE,
				       &tmp_list);

		coll_count += tmp_item[0].u.l_value + /* one_coll */
		              tmp_item[1].u.l_value + /* more_coll */
			      tmp_item[2].u.l_value;  /* excess_coll */
	    }
	    
	    printf("\n     %lu output errors, %lu collisions, "
		   "%lu interface resets",
		   (hwidb->counters.output_error - sic->output_error),
		   coll_count,
		   (hwidb->counters.resets - sic->resets));
	}
	
	if (hwidb->status & IDB_ETHER) {
	    N_ITEM_LIST(tmp, 5);

	    item_desc_init(&tmp_item[0], ETHER_HW_OUTPUT_BABBLES_STAT,
			   sizeof(ulong));

	    item_desc_init(&tmp_item[1], ETHER_HW_OUTPUT_LATE_COLLISION_STAT,
			   sizeof(ulong));

	    item_desc_init(&tmp_item[2], ETHER_HW_OUTPUT_DEFERRED_STAT,
			   sizeof(ulong));

	    item_desc_init(&tmp_item[3], ETHER_HW_OUTPUT_LOST_CARRIER_STAT, 
			   sizeof(ulong));

	    item_desc_init(&tmp_item[4], ETHER_HW_FEIP_NO_CARRIER_STAT,
			   sizeof(ulong));

	    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_STATE,
				       &tmp_list);

	    printf("\n     %lu babbles, %lu late collision, %lu deferred",
		   tmp_item[0].u.l_value,  /* output_bables */
		   tmp_item[1].u.l_value,  /* late_coll */
		   tmp_item[2].u.l_value); /* deferred */
	    printf("\n     %lu lost carrier, %lu no carrier",
		   tmp_item[3].u.l_value,  /* lost_carrier */
		   tmp_item[4].u.l_value); /* no_carrier */
	}
	printf("\n     %lu output buffer failures, "
	       "%lu output buffers swapped out",
	       (hwidb->counters.output_resource - sic->output_resource),
	       (hwidb->pak_swapped_out - hwidb->si_pak_swapped_out));

	if (hwidb->status & IDB_SERIAL) {
	    printf("\n     %lu carrier transitions",
		   (hwidb->counters.transitions - sic->transitions));
	    if (is_g703(hwidb)) {
		N_ITEM_LIST(tmp, 5);

		item_desc_init(&tmp_item[0], SERIAL_HW_START_SLOT,
			       sizeof(ushort));  

		item_desc_init(&tmp_item[1], SERIAL_HW_RX_AIS_STAT,
			       sizeof(ulong));

		item_desc_init(&tmp_item[2], SERIAL_HW_RX_RAI_STAT, 
			       sizeof(ulong));

		item_desc_init(&tmp_item[3], SERIAL_HW_RX_LOF_STAT,
			       sizeof(ulong));

		item_desc_init(&tmp_item[4], SERIAL_HW_RX_LOS_STAT,
			       sizeof(ulong));

		(*hwidb->state_config)(hwidb, IDB_CONTROL_READ_STATE,
				       &tmp_list);

		if (tmp_item[0].u.s_value[0]) { /* check for start slot */
		    printf("\n     %lu alarm indications, %lu remote alarms, %lu rx LOF, %lu rx LOS ",
			   tmp_item[1].u.l_value,  /* rx_ais */
			   tmp_item[2].u.l_value,  /* rx_rai */
			   tmp_item[3].u.l_value,  /* rx_lof */
			   tmp_item[4].u.l_value); /* rx_los */
		}
	    }
	    if (hwidb->show_serial_state)
		(*hwidb->show_serial_state)(hwidb, " ");
	}

	if (hwidb->status & (IDB_TR | IDB_FDDI))
	    printf("\n     %lu transitions",
		   (hwidb->counters.transitions - sic->transitions));
	if (hwidb->status & IDB_FDDI) {
	    ONE_ITEM_LIST(tmp, FDDI_HW_TRACE_COUNT_STAT, sizeof(boolean), 0);

	    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_STATE, &tmp_list);

	    printf(", %lu traces", tmp_item.u.l_value);
	}
	if (hwidb->type == IDBTYPE_FDDIT) {
	    N_ITEM_LIST(tmp, 2);

	    item_desc_init(&tmp_item[0], FDDI_HW_CLAIMS_STAT, sizeof(ulong));

	    item_desc_init(&tmp_item[1], FDDI_HW_BEACONS_STAT, sizeof(ulong));

	    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_STATE, &tmp_list);

	    printf(",  %lu claims, %lu beacon",
		   tmp_item[0].u.l_value,   /* fddi_claims */
		   tmp_item[1].u.l_value);  /* fddi_beacons */
	}
    }
}

/*
 * print_hwidbstate
 * Return pointer to a string describing the interface state
 */
char *print_hwidbstate (hwidbtype *idb)
{
    switch (idb->state) {
	case IDBS_DOWN: return("down");
	case IDBS_GOINGDOWN: return("going down");
	case IDBS_INIT: return("initializing");
	case IDBS_TESTING: return("testing");
	case IDBS_UP: return("up");
	case IDBS_RESET: return("reset");
	case IDBS_DELETED: return("delete pending");
	case IDBS_ADMINDOWN:
	  if (idb->standby)
	      return("standby mode");
	  else if (is_fddi(idb)) {
	      ONE_ITEM_LIST(tmp, FDDI_HW_SHUTDOWN_REASONS, sizeof(ushort), 0);
	      ushort fddi_shutdown_reasons;

	      (*idb->state_config)(idb, IDB_CONTROL_SET_STATE, &tmp_list);

	      fddi_shutdown_reasons = tmp_item.u.s_value[0];

	      if (fddi_shutdown_reasons & WRAP_A_FAIL )
		 return("WRAP A failure");
	      if (fddi_shutdown_reasons & WRAP_B_FAIL)
		 return("WRAP B failure");
	      if (fddi_shutdown_reasons & THRU_A_FAIL)
		 return("THRU A failure");
	  }
	  return("administratively down");

      	default: return("????");
    }
}

/*
 * print_idbstate
 * Same as above, but dereference the IDB pointer first.
 * For subinterfaces, note the local up/down state first.
 */

char *print_idbstate (idbtype *idb)
{
    if (is_subinterface(idb)) {
	switch (idb->subif_state) {
	case SUBIF_STATE_DOWN:
	    return("down");
	case SUBIF_STATE_DELETED:
	    return("deleted");
	case SUBIF_STATE_ADMINDOWN:
	    return("administratively down");
	default:		/* Fall through for "UP" */
	    break;
	}
    }
    return(print_hwidbstate(idb->hwptr));
}


/*
 * Here are the encapsulation keywords. The index should be the same
 * as the ET_ manifest constants (now in h/encapstype.h).
 */

static const char *const enc_keywords[ET_MAX] = {

"UNKNOWN", "ARPA", "SAP", "SNAP", "1822", "HDLC", "3MB", "HDH", "LAPB",
"X25", "HUB", "NOVELL-ETHER", "NOVELL-TR", "3COM-TR", "UB-TR", "APOLLO",
"PPP", "ISO3", "VINES-TR", "ETHERTALK", "FRAME-RELAY", "SMDS", "MAC",
"ULTRANET", "ULTRANET-HELLO", "STUN", "BRIDGE", "LLC2", "SDLC-PRIMARY",
"SDLC-SECONDARY", "SLIP", "TUNNEL", "BRIDGE_ENCAPS", "ATM", "ATM-DXI",
"FRAME-RELAY-IETF", "SMDS DXI", "ATM-DXI-IETF", "CHANNEL", "SDLC",
"802.10 vLAN", "BSTUN", "DIALER", "NOVELL-FDDI", "V.120", "ISL vLAN",
"LOOPBACK", "CHANNEL-ILAN", "SERIAL-AUTODETECT", "CPP", "NDLC"
};


/*
 * encstring
 * Return pointer to a string describing the encapsulation type
 */
const char *encstring (uint enctype)
{
    if (enctype >= ET_MAX)
	return(enc_keywords[ET_NULL]);
    return(enc_keywords[enctype]);
}

/*
 * encaptype
 * Return the encapsulation type given string describing encapsulation type
 */
uint encaptype (const char *encstring)
{
    uint i;

    for (i=0; i<=ET_MAX; i++) {
	if (strcasecmp(encstring, enc_keywords[i]) == 0) {
	    return(i);
	}
    }
    return(ET_NULL);
}

/*
 * print_linktype
 * Return packet link type as a string
 */
char *print_linktype (int link)
{
    switch (link) {
    case LINK_ILLEGAL:		return("illegal");
    case LINK_ARP:		return("arp");
    case LINK_RARP:		return("rarp");
    case LINK_XARP:		return("xarp");
    case LINK_PROBE:		return("probe");
    case LINK_LOOP:		return("loop");
    case LINK_ADDRESS:		return("address");
    case LINK_IP:		return("ip");
    case LINK_PUP:		return("pup");
    case LINK_XNS:		return("xns");
    case LINK_NOVELL:		return("ipx");
    case LINK_APOLLO:		return("apollo");
    case LINK_VINES:		return("vines");
    case LINK_PAD:		return("pad");
    case LINK_X25:		return("x25");
    case LINK_APPLETALK:	return("appletalk");
    case LINK_AARP:		return("apple-arp");
    case LINK_DEC_SPANNING:	return("dec-st");
    case LINK_IEEE_SPANNING:	return("ieee-st");
    case LINK_NBF:		return("netbios");
    case LINK_DECNET:		return("decnet");
    case LINK_DECNET_ROUTER_L1:	return("decnet_router-l1");
    case LINK_DECNET_NODE:   	return("decnet_node");
    case LINK_CLNS:		return("clns");
    case LINK_CLNS_ALL_ES:	return("clns_es");
    case LINK_CLNS_ALL_IS:	return("clns_is");
    case LINK_CLNS_BCAST:	return("clns-bcast");
    case LINK_X25TCP:		return("x25-tcp");
    case LINK_PPP:              return("ppp");
    case LINK_LAT:		return("lat");
    case LINK_VINES_ECHO:	return("vines-echo");
    case LINK_VINES_LOOP:    	return("vines-loop");
    case LINK_ATALK_SHORT:	return("appletalk-short");
    case LINK_MOP_BOOT:		return("mop-boot");
    case LINK_MOP_CONSOLE:	return("mop-console");
    case LINK_RSRB:             return("rsrb");
    case LINK_BRIDGE:           return("bridge");
    case LINK_STUN:		return("stun");
    case LINK_FR_ARP:		return("frame-relay-arp");
    case LINK_SMDS_ARP:		return("smds-arp");
    case LINK_MAC:		return("mac");
    case LINK_IBMNM:		return("ibmnm");
    case LINK_UARP:		return("uarp");
    case LINK_ULTRA_HELLO:	return("ultra-hello");
    case LINK_X25SERVICE:	return("x25-service");
    case LINK_UNCOMPRESSED_TCP:	return("uncompressed-tcp");
    case LINK_COMPRESSED_TCP:	return("compressed-tcp");
    case LINK_LLC2:		return("llc2");
    case LINK_CMNS:		return("cmns");
    case LINK_ISIS_ALL_L1_IS:   return("isis-level1-is");
    case LINK_ISIS_ALL_L2_IS:   return("isis-level2-is");
    case LINK_FR_SWITCH:	return("fr-switch");
    case LINK_DECNET_PRIME_ROUTER: return("decnet-prime-router");
    case LINK_SRB:		return("srb");
    case LINK_QLLC:		return("qllc");
    case LINK_X25_MULTI_ENC:	return("x25-multiprotocol");
    case LINK_LEX:		return("lex");
    case LINK_LEX_RCMD:		return("lex-rcommand");
    case LINK_DECNET_ROUTER_L2:	return("decnet_router-l2");
    case LINK_CLS:		return("cls");
    case LINK_SNAPSHOT:		return("snapshot");
    case LINK_DLSW:		return("dlsw");
    case LINK_SDE:		return("sde");
    case LINK_CDP:		return("cdp");
    case LINK_PPP_COMPRESSION:	return("ccp");
    case LINK_NMP:              return("nmp");
    case LINK_BSTUN:            return("bstun");
    case LINK_IPC:              return("ipc");
    case LINK_LOVE:             return("love");
    case LINK_CFGACK:           return("cfgack");
    case LINK_APPN_ANR:         return("appn-anr");
    case LINK_MULTILINK:        return("multilink");
    case LINK_NHRP:		return("nhrp");
    case LINK_MAC_EXP:		return("mac-use-express-buffer");
    case LINK_CGMP: 		return("cgmp");
    case LINK_VTP: 		return("vtp");
    case LINK_CPP:		return("cpp");
    case LINK_ISL:		return("isl");
    case LINK_FR_SWITCH_SERIAL:	return("fr-switch");
    }
    return("????");
}

/*
 * compute_idb_load
 * Compute the five minute, exponentially-decayed output bit rate of a
 * network interface and the loading factor of the entire system.
 * We are called from the scheduler every 5 seconds.
 *
 *	average = ((average - interval) * e exp (-t/C)) + interval
 * 
 *  where t is five seconds and C is 5 minutes.  exp(-5/(60*5)) == .983
 *
 * The below 3 arrays (datarate_mult_1 and datarate_mult_2) will contain
 * the e exp (-t/C) factor.  datarate_mult_1 is the amount to multiply
 * to divide by 100, and datarate_mult_2 is the amount to multiply to
 * divide by 1000.  So for 5 minutes, the value is .983, which works
 * out to a mult_1 value of 98 and a mult_2 value of 3.
 * The datarate_mult_comb array is the combination (.983 becomes 983)
 * The C values are 30 sec, 1 min, 1 1/2 min ... 10 min.
 * The lower the value, the faster we will converge to the actual rates.
 *
 * These arrays are accessed by the datarate_interval of the hwidb.  This
 * is set using the load-interval command.
 */
#define MAX_DATARATE_MULT_INDEX 20
static const long datarate_mult_1[MAX_DATARATE_MULT_INDEX] =
{ 84, 92, 94, 95, 96,
  97, 97, 97, 98, 98,
  98, 98, 98, 98, 98,
  99, 99, 99, 99, 99 };

static const long datarate_mult_2[MAX_DATARATE_MULT_INDEX] =
{ 6, 0, 6, 9, 7,
  3, 6, 9, 2, 3,
  5, 6, 7, 8, 9,
  0, 0, 1, 1, 2 };

static const long datarate_mult_comb[MAX_DATARATE_MULT_INDEX] =
{ 846, 920, 946, 959, 967,
  973, 976, 979, 982, 983,
  985, 986, 987, 988, 989,
  990, 990, 991, 991, 992 };

#define SAMPLESECS 5

static void compute_idb_load (hwidbtype *idb)
{
    ulong bitssec, frames, errors, rx_cumbytes, tx_cumbytes,scaled_bandwidth;
    ulong inputs, outputs;
    uchar interval;
    thcdbtype *thcdb;
    long mult1, mult2, mult_combined;

    /*
     * Grab all the values used in this routine.  If some cares, they
     * can put a nets_disable around this.
     */
    inputs = idb_inputs(idb);
    outputs = idb_outputs(idb);
    rx_cumbytes = idb_rx_cumbytes(idb);
    tx_cumbytes = idb_tx_cumbytes(idb);
    
    mult1 = datarate_mult_1[idb->datarate_interval];
    mult2 = datarate_mult_2[idb->datarate_interval];
    mult_combined = datarate_mult_comb[idb->datarate_interval];
    /*
     * Calculate kilobits/sec transmitted and the loading coefficient.
     * be cafeful of a long overflow, as well as bandwidths so low that
     * they might result in a 0 denominator.
     *
     * bitssec is the number of kilobits in 1 second.  (125 * SAMPLESEC)
     * is really the conversion from bytes in 5 seconds, to bits in 1
     * 125 = (8 bits/bytes / 1000 bits/kilobits)
     */
    bitssec = tx_cumbytes - idb->counters.tx_intbytes;
    if ((long)bitssec < 0)
        bitssec = 0;
    bitssec = bitssec/(125*SAMPLESECS);

    scaled_bandwidth = idb->firstsw->visible_bandwidth;
    if (scaled_bandwidth == 0)
	scaled_bandwidth = 1;
    /*
     * if interface is clocking faster than the bandwidth,
     * use the max bitrate reached as bandwidth.
     */
    if (scaled_bandwidth < bitssec) {
	scaled_bandwidth = bitssec;
    }
    idb->counters.tx_intbytes = tx_cumbytes;
    idb->counters.tx_datarate = bitssec +
	((((long)(idb->counters.tx_datarate - bitssec)) * mult1) / 100);
    idb->counters.tx_datarate +=
	((((long)(idb->counters.tx_datarate - bitssec))* mult2) / 1000);
    
    idb->load = (idb->counters.tx_datarate*255) / scaled_bandwidth;
    if (idb->load == 0)
        idb->load = 1;
    
    /*
     * Calculate kilobits/sec received
     * be cafeful of a long overflow
     */
    bitssec = rx_cumbytes - idb->counters.rx_intbytes;
    if ((long)bitssec < 0)
        bitssec = 0;
    bitssec = bitssec/(125*SAMPLESECS);
    idb->counters.rx_intbytes = rx_cumbytes;
    idb->counters.rx_datarate = bitssec +
	((((long)(idb->counters.rx_datarate - bitssec)) * mult1) / 100);
    idb->counters.rx_datarate +=
	((((long)(idb->counters.rx_datarate - bitssec)) * mult2) / 1000);
    
    scaled_bandwidth = idb->firstsw->visible_bandwidth;
    if (scaled_bandwidth == 0)
	scaled_bandwidth = 1;
    /*
     * if interface is clocking faster than the bandwidth,
     * use the max bitrate reached as bandwidth.
     */
    if (scaled_bandwidth < bitssec) {
	scaled_bandwidth = bitssec;
    }
    idb->rxload = (idb->counters.rx_datarate*255) / scaled_bandwidth;
    if (idb->rxload == 0)
        idb->rxload = 1;

    /*
     * Calculate packets/second transmitted
     * be cafeful of a long overflow
     */
    frames = outputs - idb->counters.tx_intoutputs;
    if ((long)frames < 0)
        frames = 0;
    frames = frames/SAMPLESECS;
    idb->counters.tx_packetrate = frames +
	((((long)(idb->counters.tx_packetrate-frames)) * mult1) / 100);
    idb->counters.tx_packetrate +=
	((((long)(idb->counters.tx_packetrate-frames))*mult2)/1000);
    
    /*
     * Calculate packets/second received
     * be careful of a long overflow
     */
    frames = inputs - idb->counters.rx_intinputs;
    if ((long)frames < 0)
        frames = 0;
    frames = frames/SAMPLESECS;
    idb->counters.rx_packetrate = frames +
	((((long)(idb->counters.rx_packetrate-frames)) * mult1) / 100);
    idb->counters.rx_packetrate +=
	((((long)(idb->counters.rx_packetrate-frames)) * mult2) / 1000);
    
    /*
     * Calculate link reliability coefficient.  This is the likelihood
     * that a packet will be successfully transmitted or received,
     * expressed as a fraction of 255, i.e.  a reliability of 255
     * indicates a total reliable link.  For now we do calculations
     * differently for LAPB or SDLC-based encapsulations. 
     */
    if (is_lapb(idb)) {
        frames = idb->lapb_iframessent - idb->lapb_lastiframes;
        errors = idb->lapb_rejsrcvd - idb->lapb_lastrejs;
        idb->lapb_lastiframes = idb->lapb_iframessent;
        idb->lapb_lastrejs = idb->lapb_rejsrcvd;
    } else {
        frames = (inputs + outputs) - 
    	        (idb->counters.rx_intinputs + idb->counters.tx_intoutputs);
        errors = (idb->counters.input_error + idb->counters.output_error) -
    		idb->counters.rxtx_interrors;
        idb->counters.rxtx_interrors
	    = idb->counters.input_error + idb->counters.output_error;
    
    }
    if (frames > 0) {
        interval = 255 - ((255*errors)/frames);
        if (interval < 128)
    	    interval = 128;
        idb->reliability = interval + (uchar)((((int)idb->reliability - 
	        (int)interval) * mult_combined) / 1000);
    }
    
    /*
     * Reset total packets tx/rx in last interval
     */
    idb->counters.rx_intinputs = inputs;
    idb->counters.tx_intoutputs = outputs;
    
    /*
     * If we're doing header compression, calculate the cache miss rate
     * and max cache miss rate.
     */
    thcdb = idb->tcp_compress;
    if (thcdb) {
        frames = (thcdb->misses - thcdb->intmisses)/SAMPLESECS;
        if ((long) frames < 0)
    	    frames = 0;
        thcdb->missrate = frames +
    	((((long) (thcdb->missrate - frames)) * mult_combined) / 1000); 
        thcdb->maxmissrate = max(thcdb->maxmissrate, thcdb->missrate);
        thcdb->intmisses = thcdb->misses;
    }
}

/*
 * compute_loads
 * Compute the five minute, exponentially-decayed output bit rate of each
 * network interface and the loading factor of the entire system.
 *	average = ((average - interval) * exp (-t/C)) + interval
 * 
 *  where t is five seconds and C is 5 minutes.  exp(-5/(60*5)) == .983
 */

static process compute_loads (void)
{
    hwidbtype *idb;

    process_wait_on_system_init();

    /*
     * Stagger this process from system start by two seconds.
     */
    process_sleep_periodic(2*ONESEC);

    while (TRUE) {
	/*
	 * Do interface specific averages
	 */ 
	FOR_ALL_HWIDBS(idb) {
	    compute_idb_load(idb);
	}

	/*
	 * Compute load averages every five seconds.
	 */
	process_sleep_periodic(5*ONESEC);
    }
}

/*
 * load_interval_command
 * Set datarate_interval value in idb.  This is an index into 2 arrays,
 * defined above.  Valid values are 30, 60, 90, ... 600 seconds.
 */

void load_interval_command (parseinfo *csb)
{
    hwidbtype *idb;
    ushort new;

    idb = csb->interface->hwptr;
    if (reg_invoke_dialer_huntgroup_member(idb)) {
	if (!(csb->nvgen))
	    printf("\nCannot change load-interval values of "
		   "hunt group member");
	return;
    }
    if (csb->nvgen) {
	nv_write((idb->datarate_interval != DATARATE_INTERVAL_DEF),
		 "%s %d",
		 csb->nv_command, (idb->datarate_interval + 1) * 30);
	return;
    }

    if (!csb->sense) {
	/* reset to default */
	idb->datarate_interval = DATARATE_INTERVAL_DEF;
	return;
    }

    new = GETOBJ(int,1);
    if ((new % 30) != 0) {
	printf("\nLoad interval must be in increments of 30 seconds");
	return;
    }
    /*
     * Get an index of 0 to MAX_DATARATE_MULT_INDEX - 1
     */
    idb->datarate_interval = (new / 30) - 1;
    if (idb->datarate_interval >= MAX_DATARATE_MULT_INDEX)
	idb->datarate_interval = DATARATE_INTERVAL_DEF;

    reg_invoke_interface_cfg_modified(idb);

}

/*
 * net_cstate
 * Perform functions necessary when interface changes state.
 * If we haven't seen a transition for a while, set a timer and remember
 * old state.  Later the net background process will compare old state
 * with current state to see if anything really changed.
 * 
 * Functions in SERVICE_NET_CSTATE are those that need to be invoked
 * whenever a line even thinks about flapping.  Those that can wait until
 * the line is declared stone cold dead should use SERVICE_HWIF_STATE_CHANGE.
 */

void net_cstate (hwidbtype *idb, int newstate)
{

    /*
     * Check to see if it's OK to run net_cstate
     */
    if (reg_invoke_pre_cstate(idb, newstate)) {
	return;
    }

    if (!(mgd_timer_running(&idb->carriertimer))) {
	/*
	 * Note that a 0 carriertime will not invoke the state transition
	 * immediately - this routine could be called in the interrupt
	 * routine, which could malloc, which is bad.  TIMER_START
	 * will make it go on the next tick, which will be the next
	 * time that cstate_check runs.
	 */
	mgd_timer_start(&idb->carriertimer, idb->carrierdelay);

	/*
	 * The only time we are spoofing is when the interface is really
	 * down, and we are pretending it is up.  So, if we are spoofing,
	 * we know it was down so set the oldstate to DOWN.  Otherwise, 
	 * if we are not spoofing, then it's possible the interface was
	 * up so just set it to the current state.
	 */
	if (reg_invoke_dialer_spoofing_int(idb) && !is_isdn(idb))
	    idb->oldstate = IDBS_DOWN;
	else
	    idb->oldstate = idb->state;
    }

    /*
     * If we are not spoofing, then the interface is up and carrier
     * just dropped.  So we want to pass the fact that carrier dropped
     * along.  But, if we are spoofing, that means that either carrier
     * bounced down before the timer expired, or carrier just came up.
     * In either case we don't want to change the state which should be
     * IDBS_UP.
     */
    if (newstate == IDBS_ADMINDOWN || !reg_invoke_dialer_spoofing_int(idb) ||
	is_isdn(idb)) {
	idb->state = newstate;
    }
    GET_TIMESTAMP(idb->state_time);
    reg_invoke_net_cstate(idb);
}

/*
 * net_sw_cstate
 * Perform protocol related tasks when an interface changes state.
 * Broke this out from cstate_check for ease of plugging in other
 * portions later, plus for sharing the cstate_check code.
 */

static void net_sw_cstate (hwidbtype *hwidb)
{
    idbtype *swidb;

    /*
     * Notify those routines that want to know when the
     * physical layer has changed.  Different from 
     * lineaction, which is usually for line protocol
     */
    reg_invoke_hwif_state_change(hwidb);
    
    /*
     * Perform line protocol dependent actions upon
     * state change, e.g. LAPB brings up a link, Token
     * Ring adjusts ARP and RIF caches, etc.
     */
    if (hwidb->lineaction)
        (*(hwidb->lineaction))(hwidb);
    
    /*
     * Perform higher level protocol dependent actions.
     * For example, check LLC2 connections, adjust routing
     * and bridging topologies, poke the dialer and backup
     * routines, etc.  transition_adjust() sets up variables
     * used by these routines.
     */
    transition_adjust(hwidb);
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
        reg_invoke_route_adjust_msg(swidb);
        reg_invoke_route_adjust(swidb);
        reg_invoke_bridge_adjust(swidb);
    }
    if (hwidb->keep_transition) {
        backup_adjust(hwidb);
        reg_invoke_if_statechange_complete(NULL, hwidb);
    }
}

/*
 * cstate_check
 * Check for carrier transitions on interfaces.
 * These are major interface state transitions.
 * Line Protocol changes are checked for in net_periodic
 * via periodic_activity.
 * This is the default routine.
 */

static void cstate_check (void)
{
    mgd_timer *expired_timer;
    hwidbtype *hwidb, *netidb;
    
    while (mgd_timer_expired(&if_cstate_timer)) {
	expired_timer = mgd_timer_first_expired(&if_cstate_timer);
	hwidb = mgd_timer_context(expired_timer);
	mgd_timer_stop(&hwidb->carriertimer);
	netidb = get_netidb(hwidb);
	if (hwidb->state != hwidb->oldstate) {
	    hwidb->reason = print_hwidbstate(hwidb);
	    switch (hwidb->state) {
	      case IDBS_DOWN: 
	      case IDBS_UP:
		if (!system_loading) {
		    if (LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(netidb)) {
			errmsg(&msgsym(UPDOWN, LINK), hwidb->hw_namestring,
			       hwidb->reason);
		    }
		}
		break;
	      default:
		if (!system_loading) {
		    if (LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(netidb)) {
			errmsg(&msgsym(CHANGED, LINK), hwidb->hw_namestring,
			       hwidb->reason);
		    }
		}
		break;
	    }
	    if (hwidb->state != IDBS_UP && hwidb->failCause != 0)
		LinkDownEvent(hwidb);     /* generate specific trap & alert */
	    else if (hwidb->state == IDBS_UP){
		hwidb->lastFailCause = 0;
		reg_invoke_ifmib_link_trap(LINK_UP_TRAP, hwidb);
	    }
	    else if (hwidb->lastFailCause == 0)   /* the link was up */
		/* The following routine needn't be called if all the linkdown
		 * event is implemented. 
		 */
		reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, hwidb);
	    net_sw_cstate(hwidb);
	}
    }
}

/*
 * carrier_delay_command
 * Set carrier_delay value in idb
 */

void carrier_delay_command (parseinfo *csb)
{
    hwidbtype *idb;
    ulong min,new;

    idb = csb->interface->hwptr;
    if (csb->nvgen) {
        if (idb->carrierdelay != idb->carrierdelay_def) {
            nv_write(TRUE, csb->nv_command);
            if (idb->carrierdelay < ONESEC) {
                nv_add(TRUE, " msec %d", idb->carrierdelay);
            } else {
                nv_add(TRUE, " %d", idb->carrierdelay / ONESEC);
            }
        }
	return;
    }

    if (!csb->sense) {
	/* reset to default */
	idb->carrierdelay = idb->carrierdelay_def;
	return;
    }

    new = GETOBJ(int,1);
    /*
     * New value is not checked against the minimum carrier delay
     * if it is specified in milliseconds.
     */
    if (GETOBJ(int, 2) == FALSE) {
        new *= ONESEC;
        min = reg_invoke_media_minimum_carrier_delay(idb->type);
        if (min != 0 && new <= min) {
            printf("\n%d is below minimum carrier delay - set to %d sec",
                   new / ONESEC, min / ONESEC);
            new = min;
        }
    }
    idb->carrierdelay = new;
}


/*
 * interface_up
 * Returns TRUE if line protocol is up, FALSE otherwise.
 */

boolean interface_up (idbtype *idb)
{
    /*
     * Special check for subinterfaces, otherwise, return the actual
     * hardware status.
     */
    if (idb->subif_state != SUBIF_STATE_UP)
	return(FALSE);
    
    if (idb->subif_line_state == SUBIF_STATE_UP) {
	return (hw_interface_up(idb->hwptr));
    }
	
    return (FALSE);

}

/*
 * hw_interface_up
 * Returns TRUE if line protocol is up, FALSE otherwise.
 */

boolean hw_interface_up (hwidbtype *hwidb)
{
   /*
    * Handle the case of simplex interfaces
    */
    if (hwidb != hwidb->output_if_hw)
	hwidb = hwidb->output_if_hw;

    /*
     * If hardware is down, line protocol must be down too.
     */
    if (hwidb->state != IDBS_UP)
	return(FALSE);

    /*
     * Check the line protocol.
     * Complain if someone forgot to define a line protocol handler.
     * Make sure we return TRUE or FALSE since some callers expect
     * those values whereas some of the  hwidb->linestate functions may
     * return non-zero or zero values.
     */
    if (hwidb->linestate)
	return((*(hwidb->linestate))(hwidb) ? TRUE : FALSE);
    else {
	errmsg(&msgsym(LINEST, LINK), hwidb->hw_namestring);
	return(FALSE);
    }
}


/*
 * backup_timers
 * This routine gets called from net_periodic() if idb->secondary
 * is no NULL.  If the secondary is in standby mode then
 * bring UP the secondary.  If the secondary
 * is already UP then we probably need to bring it down.  But we
 * rely on the backup_loads() code to take care of that if the
 * aggregate load between primary and secondary will fit into the
 * primary only if backup loads are enabled, otherwise we take care of
 * it.  In this case (backup loads enabled) then we just clear the timer.
 */

static void backup_timers (hwidbtype *idb)
{
    hwidbtype *sec_idb;
    idbtype *swidb;
    boolean now_state;

    /*
     * Handle the hardware interface backup case
     */
    sec_idb = idb->secondary;       /* pointer to secondary interface */
    if ((sec_idb != NULL) && (validmem(sec_idb))) {
	if (TIMER_RUNNING_AND_AWAKE(sec_idb->delay_timer)) {
	    TIMER_STOP(sec_idb->delay_timer);
	    if (!backup_is_shut(sec_idb)) {
		if (hw_interface_up(idb)) {
		    /*
		     * don't know why a backup goes down should bring its
		     * backup interfaces to go down as well. 
		     * please explain the do loop if know.
		     */
		    do {
			if (!(idb->backup_state & BACKUP_LOAD_MASK) &&
			    (idb->backup_state & BACKUP_OUT_DELAY))
			    shutdown_going_down(sec_idb);
			sec_idb = sec_idb->secondary;
		    } while (sec_idb != NULL);
		} else {
		    if (idb->backup_state & BACKUP_IN_DELAY)
			shutdown_coming_up(sec_idb);
		}
	    }
	}
    }
    /*
     * Handle the subinterface backup case
     */
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	/*
	 * Ignore deleted subifs
	 * Deleted because of encaps change or user config
	 */
	if (swidb->subif_state == SUBIF_STATE_DELETED)
	    continue;
	sec_idb = swidb->backup;
	now_state = interface_up(swidb);
	if (sec_idb != NULL) {
	    if (now_state == swidb->prev_state) {
		/*
		 * if the hardware interface is down, and it has
		 * a secondary - just restart the timer
		 */
		if (!(hw_interface_up(idb)) && idb->secondary) {
		    TIMER_START(sec_idb->delay_timer,
				swidb->down_delay * ONESEC);
		    continue;
		}
		/*
		 * No state change.  If the timer expires, change
		 * the state of the backup interface
		 */
		if (TIMER_RUNNING_AND_AWAKE(sec_idb->delay_timer)) {
		    TIMER_STOP(sec_idb->delay_timer);
 		    if (sec_idb->standby) { /* Can we use this interface ? */
			if (now_state) {
			    if (sec_idb->state != IDBS_ADMINDOWN) {
				shutdown_going_down(sec_idb);
				swidb->backup_active = FALSE;
			    }
			} else {
			    if (sec_idb->state == IDBS_ADMINDOWN) {
				/* in true stand-by mode */
				shutdown_coming_up(sec_idb);
				swidb->backup_active = TRUE;
			    }
			}
		    }
		} 
	    } else {
		/*
		 * We have a state change for the swidb.
		 *
		 * now_state being TRUE means the subinterface is up.
		 * backup_active is TRUE if the backup interface is active.
		 * If they are not the same, meaning either subif has just
		 * come up while backup is down or subif has just gone down
		 * while backup is up, then no action required except to
		 * stop the delay timer.
		 * If they are the same, then we bring the backup up, if
		 * the subif is down, or else bring it down.
		 */
		if (now_state != swidb->backup_active) {
		    /* we returned to the same state before timer expired */
		    TIMER_STOP(sec_idb->delay_timer);
		} else {
		    /* have a different state - start the timer if not shut */
		    if (!backup_is_shut(sec_idb))
			TIMER_START(sec_idb->delay_timer, ONESEC *
				(now_state ? swidb->up_delay
				 : swidb->down_delay));
		}
	    }
	}
	swidb->prev_state = now_state;
    }
}

/*
 * compute_load_from_dialer_member
 * Since we no longer know about dialer members in this file,
 * we need a way to get the total load from the dialer members.
 * This routine, combined with the reg_invoke_all_dialer_members
 * call will do that, and add the loads to the temp_rxload and
 * temp_txload variables.
 */

static ulong temp_rxload, temp_txload;

static void compute_load_from_dialer_member (hwidbtype *idb)
{
    temp_txload += idb->counters.tx_datarate;
    temp_rxload += idb->counters.rx_datarate;
}

/*
 * compute_backup_loads
 * Compute the transmit and receive load of both the primary and
 * secondary interfaces used for backup.
 * (Backup-loads does not apply to the case of subinterface backup.)
 */

static void compute_backup_loads (hwidbtype *idb, ulong *tx_load,
				  ulong *rx_load)
{
    hwidbtype *sec_idb;
    ulong bandwidth;

    sec_idb = idb->secondary;       /* pointer to secondary interface */

    temp_txload = idb->counters.tx_datarate;
    temp_rxload = idb->counters.rx_datarate;
    if (is_ddr(sec_idb)) {
	reg_invoke_for_all_dialer_members(sec_idb,
					  compute_load_from_dialer_member);
    } else {
	temp_txload += sec_idb->counters.tx_datarate;
	temp_rxload += sec_idb->counters.rx_datarate;
    }

    bandwidth = idb->firstsw->visible_bandwidth;
    if (bandwidth == 0)
	bandwidth = 1;

    if (temp_txload > bandwidth)
	*tx_load = 255;
    else
	*tx_load = (temp_txload * 255) / bandwidth;
    
    if (temp_rxload > bandwidth)
	*rx_load = 255;
    else
	*rx_load = (temp_rxload * 255) / bandwidth;
}

/*
 * backup_loads
 * Check loads for backup interface.  If the primary is overloaded
 * and the secondary is in standby mode then enable secondary.
 * If primary and secondary are both up and the aggregate load is
 * less than a preset load percentage of the primary then put
 * the secondary back into standby.  Don't do it if the delay
 * timer is running as this is the only place an interface can
 * be put back into standby, even if the secondary was started
 * due to failure of the primary.  Mind the "valid" bits in backup_state
 * for the {in,out}_load vars in the idb.  Assume the caller has
 * checked the that idb->secondary != NULL.
 */

static void backup_loads (hwidbtype *idb)
{
    hwidbtype *sec_idb;
    ulong tx_load;
    ulong rx_load;

    sec_idb = idb->secondary;       /* pointer to secondary interface */
    if (!sec_idb || !validmem(sec_idb))
	return;
    if ((hw_interface_up(idb)) && !(mgd_timer_running(&idb->carriertimer))) {
	if ((idb->backup_state & BACKUP_IN_LOAD) &&
	    (sec_idb->standby) &&
	    (sec_idb->state == IDBS_ADMINDOWN)) {
	    if ((idb->load > idb->in_load) ||
		(idb->rxload > idb->in_load)) { /* iface ok,check load */
		shutdown_coming_up(sec_idb);
		return;
	    }
	}
	if (idb->backup_state & BACKUP_OUT_LOAD) {
	    if ((sec_idb->state != IDBS_ADMINDOWN) &&
		(!(TIMER_RUNNING(sec_idb->delay_timer)))) {
		/* interface states ok, check load */
		compute_backup_loads(idb, &tx_load, &rx_load);
		if ((tx_load < idb->out_load) && (rx_load < idb->out_load))
		    shutdown_going_down(sec_idb);
	    }
	}
    }
}

/*
 * backup_adjust
 * Determine if an interface has transitioned.  If so, do appropriate
 * magic to initiate kicking of secondaries.
 */
void backup_adjust (hwidbtype *idb)
{
    hwidbtype *sec_idb;

    /* Check to see if we need to set or reset the
     * secondary delay timer.
     */
    sec_idb = idb->secondary;

    if ((sec_idb != NULL) && (idb->keep_transition)) {
	if (idb->keep_current_up) {	/* Primary just went up */
	    if ((idb->backup_state & BACKUP_OUT_DELAY) &&
		!(idb->backup_state & BACKUP_OUT_LOAD)) {
		if ((sec_idb->standby) && (sec_idb->state != IDBS_ADMINDOWN)) {
		    TIMER_START(sec_idb->delay_timer,
				(idb->out_delay * ONESEC));
		} else {
		    TIMER_STOP(sec_idb->delay_timer);
		}
	    }
	} else {      /* Primary just went down */
	    if (idb->backup_state & BACKUP_IN_DELAY) {
		if ((sec_idb->standby) && (sec_idb->state == IDBS_ADMINDOWN)) {
		    TIMER_START(sec_idb->delay_timer, 
				(idb->in_delay * ONESEC));
		} else {
		    TIMER_STOP(sec_idb->delay_timer);
		}
	    }
	}
    }
}

/*
 * transition_adjust
 * This routine sets the transition variables in the idb for use by
 * backup_adjust() and the routing routines called by route_adjust().
 */

void transition_adjust (hwidbtype *idb)
{
    idbtype *swidb;

    idb->keep_current_up = hw_interface_up(idb);
    idb->keep_transition = (idb->keep_current_up != idb->keep_last_up);
    idb->keep_last_up = idb->keep_current_up;
    if (idb->keep_transition) {
	FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	    if (idb->keep_current_up)
		reg_invoke_swif_comingup(swidb);
	    else
		reg_invoke_swif_goingdown(swidb);
	}
    }
}

/*
 * route_adjust
 * Jacket routine to call topology adjustment routines for various protocols
 */
void route_adjust (hwidbtype *hwidb)
{
    idbtype *idb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
        reg_invoke_route_adjust_msg(idb);
	reg_invoke_route_adjust(idb);
    }
}

/*
 * bridge_adjust
 * Adjust the bridging database when a bridge port goes down.
 * Call transparent and source route specific routines.
 */

static void bridge_adjust (hwidbtype *hwidb)
{
    idbtype *idb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	reg_invoke_bridge_adjust(idb);
    }
}

/*
 * setup_all_bridging
 * Execute all bridge initialization funtions on all network interfaces:
 * This will redundantly setup bridging on Multibus systems, but this should
 * not be a problem or performance concern. Bridging is not changed often.
 */

void setup_all_bridging (void)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb)
	if (idb->setup_bridging)
	    (*(idb->setup_bridging))(idb);

    reg_invoke_bridge_setup();
}

/*
 * coerce_hwaddr
 * Coerce an interface to its given hw address.  This routine
 * will make sure the current approrpiate hw address is being used
 * and anything else that depends on it remains consistant.
 *
 * Do the reset last!  Otherwise the reset wipes out idb->hardware
 * with the previous value of xns_node or novell_node.  Which is
 * then used to wipe out xns_node or novell_node.
 *
 * For a given interface the appropriate hardware address should
 * always be the same regardless of what protocol is enabled.  In
 * other words xns_node and novell_node should be the same for a
 * given interface.  Note further that canonical screws this up but
 * there is still a simple transformation.
 */

void coerce_hwaddr (uchar *addr, hwidbtype *idb)
{
    idbtype *subidb;

    reg_invoke_tbridge_smf_delete(idb->hardware, idb->firstsw,
				  SMF_MACTYP_OWN_UNICAST |
				  SMF_ACTION_RECEIVE);
    ieee_copy(addr, idb->hardware);
    (*idb->reset)(idb);
    if (idb->counters.resets > 0)
	idb->counters.resets--;
    FOR_ALL_SWIDBS_ON_HW(idb, subidb) {
	/* The change to subidb->novell_node has been moved */
	/*  to the mac_address_change callback in novell.c  */
	reg_invoke_media_hwaddr_notify(subidb, idb->hardware, IEEEBYTES);
    }

    /*
     * Inform those who care that the MAC address changed.  Follow this by a
     * general address resolution cache clear.  This ensures gratuitous ARPs
     * are done.
     */
    reg_invoke_mac_address_change(idb);
    reg_invoke_ar_cache_clear(NULL);
}

/*
 * macaddr_command
 * Force an interface's mac address to a particular value.
 */

void macaddr_command (parseinfo *csb)
{
    uchar *newmac;
    hwidbtype *idb;

    idb = csb->interface->hwptr;
    if (csb->nvgen) {
	nv_write((((ushort *)idb->forced_mac)[0] |
		  ((ushort *)idb->forced_mac)[1] |
		  ((ushort *)idb->forced_mac)[2]),
		 "%s %e", csb->nv_command,
		 idb->forced_mac);
	return;
    }

    if (!(idb->status & (IDB_IEEEMASK|IDB_BRIDGE|IDB_BVI))) {
	printf("\n%% \"mac-address\" not valid on this interface");
	return;
    }
    if (!csb->sense) {
      if (((ushort *)idb->forced_mac)[0] |
	  ((ushort *)idb->forced_mac)[1] |
	  ((ushort *)idb->forced_mac)[2]) {
	ieee_zero(idb->forced_mac);
	coerce_hwaddr(idb->bia, idb);
      }
      return;
    }

    /*
     * Check for reasonable MAC address.  Don't allow Multicasts
     */
    newmac = GETOBJ(hwaddr,1)->addr;
    if (((idb->status & (IDB_ETHER|IDB_SERIAL)) && (newmac[0] & 0x01)) || 
	((idb->status & IDB_TR) && (newmac[0] & 0x80))) {
	printf("\n%% Malformed hex mac address");
	return;
    }
    ieee_copy(newmac, idb->forced_mac);
    coerce_hwaddr(newmac, idb);
}

/*
 * find_802addr
 *
 * *** This routine is obsolete. Please use the idb vector ***
 * *** extract_snpa in order to get an data link address   ***
 * *** out of a packet.                                    ***
 *
 * Given a packet, find the 802 MAC level destination or source address.
 *
 * If this packet has the transparent bridge in flag on, then it
 * came in encapsulated, so treat it as ethernet. Otherwise,
 * find the appropriate media-specific address.
 */

void find_802addr (paktype *pak, uchar *dest, boolean which)
{
    int offset, i;
    uchar *from;
    hwidbtype *hwidb = pak->if_input->hwptr;

    offset = (which == GET_SOURCE) ? IEEEBYTES : 0;

    if ((hwidb->status & IDB_ETHER) || (pak->flags & PAK_TBRIDGE_IN))
	ieee_copy(pak->datagramstart+offset, dest);
    else if (hwidb->status & IDB_TR) {
	ieee_copy(pak->datagramstart+2+offset, dest);
	if ((which == GET_SOURCE))
	    dest[0] &= ~TR_RII;
    } else if (hwidb->status & IDB_FDDI) {
	if (hwidb->type == IDBTYPE_FDDIT) {
	    ieee_copy(pak->datagramstart + 1 + offset, dest);
	} else {
	    from = pak->datagramstart + 1 + offset;
	    for (i = 0; i < IEEEBYTES; i++)
		*dest++ = bit_swaps[*from++];
	}
    } else
        reg_invoke_local_lnm_find_802addr(pak, dest, which);
}

/* net_get_first_ieee_hardware_address
 *
 * Get hardware address of first ethernet (or first IEEE) interface
 */
static uchar *net_get_first_ieee_hardware_address (void)
{
    hwidbtype *idb;
    uchar *ptr;
    list_element *l_elt;
    
    /*
     * Return the first ethernet if present otherwise return first IEEE.
     * The first ethernet is important if we have xns turned on. Both of
     * these checks make sure that the address chosen, if any, is non-zero.
     */
    ptr = NULL;
    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_ETHER, l_elt, idb) {
	if (!is_ieee_zero(idb->hardware)) {
	    ptr = idb->hardware;
	    break;
	}
    }

    /*
     * If no ethernet address picked -- check for IEEE style interface.
     */
    if (ptr == NULL) {
	FOR_ALL_HWIDBS(idb) {
	    if ((idb->status & IDB_IEEEMASK) &&
		!is_ieee_zero(idb->hardware)) {
		ptr = idb->hardware;
		break;
	    }
	}
    }
    
    return(ptr);
}

/*
 * net_default_addr_init
 * Choose the hardware address to use when a default box-wide IEEE address
 * is needed.  It will return the first non-zero ethernet address in
 * the system. If there are no ethernets, it will pick the first address
 * of any IEEE device in the system. If none can be found, we use the time
 * to construct an address.
 */

static void net_default_addr_init (void)
{
    hwidbtype *idb;
    uchar *ptr;
    
    /*
     * Get the hardware address of first ethernet, if present, or hardware
     * address of first IEEE interface.
     */
    ptr = net_get_first_ieee_hardware_address();

    /*
     * Make up an address using the exact time, if we haven't found one that is
     * non-zero.
     */
    if (ptr == NULL) {
	clock_epoch exact_time;
	clock_get_time_exact(&exact_time);
	default_mac_addr[0] = 0x00;
	default_mac_addr[1] = 0x00;
	default_mac_addr[2] = 0x0c;
	default_mac_addr[3] = exact_time.epoch_secs & 0xff;
	PUTSHORT(&default_mac_addr[4], exact_time.epoch_frac >> 16);
        errmsg(&msgsym(NOMAC, LINK), default_mac_addr);
    } else 
        ieee_copy(ptr, default_mac_addr);
    
    /*
     * Set our default address on all non-IEEEMASK interfaces.
     */
    FOR_ALL_HWIDBS(idb) {
	if (!(idb->status & IDB_IEEEMASK) &&
	    !(idb->status & IDB_ATM)) {
	    ieee_copy(default_mac_addr, idb->hardware);
	    ieee_copy(default_mac_addr, idb->bia);
	}
    }

    /*
     * Register 'get_first_ieee_hardware_address' after final initialization
     */
    reg_add_media_get_first_ieee_hardware_address(
	                                net_get_first_ieee_hardware_address,
				        "net_get_first_ieee_hardware_address");
}

/*
 * This will create hopefully a unique number.  It is based on
 * the clock time, the mac addr of the system, and a local static
 * value.
 */
ulong create_unique_number (void)
{
    static ulong temp;
    ulong curtime;
    GET_TIMESTAMP32(curtime);
    return(curtime + temp++ +
	   GETLONG(&default_mac_addr[(IEEEBYTES - 1) - sizeof(ulong)]));
    /* The above index gets the last 4 bytes of the default_mac_addr */
}

/*
 * new_physical_mtu
 *
 * The MTU of an interface has changed.  This is necessary because a 16M
 * token ring has a higher capability than a 4M ring, and we'd like to
 * tap some of the extra.  All routes must be cleared because IGRP
 * maintains the MTU as part of the routing metric.  If the initial flag
 * is set, the routes will not be cleared.  This flag should be true *ONLY*
 * if this is a newly created idb from the hotswap code!
 */
void new_physical_mtu (
    hwidbtype *hwidb,
    int mtu,
    boolean initial)
{
    idbtype *idb;

    hwidb->maxmtu_def = mtu;
    hwidb->maxmtu = mtu;

    reg_invoke_media_default_mtu(hwidb);
    if (initial)
	return;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	reg_invoke_reset_if(idb);
    }
    reg_invoke_clear_routes();
}

/*
 * can_select_subif_type
 *
 * Returns TRUE if the parameter is a subinterface IDB and it is legal to
 * configure whether or not it is point-to-point or not.
 */
boolean can_select_subif_type (idbtype *idb)
{
    if (idb) {
	if (is_subinterface(idb)) {
	    if (idb->hwptr->status & IDB_SERIAL)
		if (is_frame_relay(idb->hwptr) || is_x25(idb->hwptr) ||
		(is_atm_dxi(idb->hwptr))) {
		    return(TRUE);
		}
	    if (is_smds(idb->hwptr)) {
		return(TRUE);
	    }
	    if (idb->hwptr->status & IDB_ATM) {
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}

/*
 * set_subif_link_type
 *
 * Set idb->subif_link_type for a subinterface when it is not specified
 * by the user.   This gets called for all software IDBs, whether they
 * be subinterfaces or not.
 *
 * We set subinterfaces on Frame Relay, X.25 & AIP to be multipoint by
 * default, and all others to be the same as the hardware.
 */
void set_subif_link_type (idbtype *idb)
{
    if (is_subinterface(idb)) {	/* Only for subinterfaces! */
	if (idb->hwptr->status & IDB_SERIAL) {
	    if (is_frame_relay(idb->hwptr) || is_x25(idb->hwptr)
	      || is_smds(idb->hwptr) || is_atm_dxi(idb->hwptr)) {
		idb->subif_link_type = SUBIF_TYPE_MULTI;
		return;
	    }
	    if (idb->hwptr->status & IDB_ATM) {
		idb->subif_link_type = SUBIF_TYPE_MULTI;
		return;
	    }
	}
    }
    idb->subif_link_type = (is_p2p_hw(idb->hwptr) ? 
			    SUBIF_TYPE_P2P : SUBIF_TYPE_MULTI);
}


/*
 * main_if_or_vc_or_vlan ()
 *
 * Returns TRUE if:-
 *
 * The interface is the first (i.e. major) Software IDB or
 *
 * The interface is a subinterface of a Virtual Ciruit based protocol,
 * i.e. ATM. Frame Relay or X.25 or
 * 
 * The interface is capable of supporting IEEE 802.10 based or Inter Switch
 * Link (ISL) Virtual LAN encapsulation.
 */

boolean main_if_or_vc_or_vlan (idbtype *swidb)
{
    if (!is_subinterface(swidb))
        return(TRUE);

    if (is_frame_relay(swidb->hwptr) || is_x25(swidb->hwptr) ||
	is_atm_dxi(swidb->hwptr) || (is_atm(swidb->hwptr) || 
	is_smds(swidb->hwptr)))
        return(TRUE);

    if (is_vlan_capable(swidb))
	return (TRUE);

    return(FALSE);
}


/*
 * main_if_or_vc
 *
 * Returns true if the IDB is either not a subinterface, or it's a subinterface
 * on a virtual circuit network (X.25 or frame relay).
 */
boolean main_if_or_vc (idbtype *idb)
{
    if (!is_subinterface(idb))
	return(TRUE);
    if (is_frame_relay(idb->hwptr) || is_x25(idb->hwptr) || is_atm_dxi(idb->hwptr))
	return(TRUE);
    if (is_atm(idb->hwptr))
	return(TRUE);
    if (is_smds(idb->hwptr))
	return(TRUE);
    return(FALSE);
}

/*
 * main_if_or_iftype
 *
 * Returns true if the IDB is either not a subinterface, or it's a subinterface
 * and the hardware IDB type matches the supplied flag.
 */
boolean main_if_or_iftype (idbtype *idb, ulong idbstatus)
{
    if (!is_subinterface(idb))
	return(TRUE);
    if (idb->hwptr->status & idbstatus)
	return(TRUE);
    return(FALSE);
}

/*
 * routed_protocols_on_idb
 * Returns TRUE if any of the routed protocols are active on the given
 * interface
 */

boolean routed_protocols_on_idb (idbtype *idb)
{
    int linktype;

    for (linktype = 0; linktype < LINK_MAXLINKTYPE; linktype++)
	if (reg_invoke_routed_proto_on_swidb(linktype, idb))
	    return(TRUE);
    return(FALSE);
}


/*
 * invalidate_snpa
 */

boolean invalidate_snpa (paktype *pak, hwaddrtype *address, enum SRCDST which)
{
    address->type = STATION_ILLEGAL;
    address->length = 0;
    address->addr[0] = 0;
    return(FALSE);
}

/*
 * invalidate_proto_address
 */

static void invalidate_proto_address (
    idbtype *swidb,
    paktype *pak,
    addrtype *addr)
{
    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_ILLEGAL;
    addr->length = 0;
}

/*
 * sprintf_unknown_address
 *
 * Take an addrtype data structure and print it as an IP address.
 */
static void sprintf_unknown_address (
    char *string,
    addrtype *addr)
{
    sprintf(string, "UNKNOWN");
}

static void sprintf_unknown_src_dst (
    char *string,
    paktype *pak,
    enum SRCDST which)
{
    sprintf(string, "UNKNOWN");
}

/*
 * ieee_extract_hwaddr
 */
boolean ieee_extract_hwaddr (hwidbtype *hwidb, hwaddrtype *hwaddr)
{
    memset(hwaddr, 0, sizeof(hwaddrtype));
    hwaddr->type = STATION_IEEE48;
    hwaddr->length = STATIONLEN_IEEE48;
    hwaddr->target_type = TARGET_UNICAST;
    ieee_copy(hwidb->hardware, hwaddr->addr);
    return(TRUE);
}

/*
 * invalidate_hwaddr
 */
boolean invalidate_hwaddr (hwidbtype *hwidb, hwaddrtype *hwaddr)
{
    memset(hwaddr, 0, sizeof(hwaddrtype));
    hwaddr->type = STATION_ILLEGAL;
    return(TRUE);
}


/*
 * l l c _ m t u
 * Return the LLC layer payload value given an interface.
 * For non-llc2 media return what it would be (regular
 * payload).  For llc2 media, modify the interface's mtu
 * value to include the llc2 overhead.
 */

int llc_mtu (hwidbtype *hwidb)
{
    if (hwidb->status & IDB_802ONLY)
	return(hwidb->maxmtu + LLC_DATA_OFFSET);
    else
	return(hwidb->maxmtu);
}

/*
 * usable_interfaces
 * Predicate for determining if any IP interfaces are usable, i.e.,
 * they have an address and are in an up state.  Although this properly
 * belongs with IP, this function is used for various system initialization
 * functions and so is included here.
 */

boolean usable_interfaces (void)
{
    idbtype *idb;

    FOR_ALL_SWIDBS(idb) {
	if (idb->ip_enabled && idb->ip_address && idb->ip_nets_mask
		 && interface_up(idb))
	    return(TRUE);
    }
    return(FALSE);
} 

/*
 * return_link_illegal
 */

static ulong return_link_illegal (idbtype *idb, paktype *pak)
{
    return(LINK_ILLEGAL);
}

/*
 * Write the encapsulation into NVRAM
 */
static void default_encaps_nv_write (parseinfo *csb)
{
    nv_write(TRUE, csb->nv_command);
}

/*
 * display the encapsulation when requested.
 *
 * If this is a subinterface used to for Virtual LAN switching/routing,
 * let that be known.
 */

void
default_encaps_display (idbtype *idb)
{
    hwidbtype *hwidb = idb->hwptr;

    if (idb->sde_said_db != NULL)
	printf("\n  Encapsulation Virtual LAN 802.10 SDE, SAID %d.",
	       idb->sde_said_db->sdb_said);
    else if (idb->isl_vlan != NULL)
	printf("\n  Encapsulation ISL Virtual LAN, Color %d.",
	       idb->isl_vlan->colour);
    else
	printf("\n  Encapsulation %s", encstring(hwidb->enctype));
}


/*
 * default case for transmitter_command registry
 */
static void default_command (hwidbtype *idb, int value)
{
    printf("\n%%Unsupported command for this interface type");
}

/*
 * default case for header compression.  Just return the pak
 */
static paktype *default_compress_header(paktype *pak)
{
    return(pak);
}

/*
 * network_init
 * General Media layer initialization.
 *
 * This function is called after Kernel startup and before Driver startup.
 * Make sure that anything that's added in here doesn't expect -
 *
 *  1. IDBs
 *  2. Drivers
 *  3. Registries other than REG_SYS and REG_MEDIA
 *
 */

void network_init (void)
{
    ulong dummy;

    /*
     * Reset the global throttle flag to FALSE. This flag must not be set
     * to FALSE anywhere else other than net_background() from this point
     * on during the uptime of the box.
     */
    net_inputQ = create_watched_queue("Net Input", 0, INPUT_QUEUE);
    net_abortQ = create_watched_queue("Net Abort", 0, ABORT_QUEUE);
    net_throttle_flag = create_watched_boolean("Net Throttle", 0);
    idb_create_list(HWIDBLIST_THROTTLED, &dummy);

    /*
     * Init some bridging data structures
     */
    queue_init(&bteQ, 0);
    bte_count = 0;

    /*
     * Register some default functions
     */
    reg_add_if_final_init(net_default_addr_init, "net_default_addr_init");
    reg_add_raw_enqueue(LINK_ILLEGAL, netinput_enqueue,"netinput_enqueue");
    reg_add_raw_enqueue(LINK_ADDRESS, netinput_enqueue,"netinput_enqueue");
    reg_add_raw_enqueue(LINK_LOOP, loopback_enqueue,"loopback_enqueue");
    reg_add_raw_enqueue(LINK_LLC2, llc_input, "llc_input");
    reg_add_default_raw_enqueue(netinput_enqueue,"netinput_enqueue");
    reg_add_default_loop_enqueue(netinput_enqueue,"netinput_enqueue");
    reg_add_default_proto_on_swidb((service_proto_on_swidb_type )
				   return_false, "return_false");
    reg_add_default_proto_running((service_proto_running_type)
				  return_false, "return_false");
    reg_add_default_proto_bridging_on_swidb
	((service_proto_bridging_on_swidb_type) return_false,
	 "return_false");  
    reg_add_default_proto_routing_on_swidb
	((service_proto_routing_on_swidb_type) return_false,
	 "return_false");  
    reg_add_default_proto_address(invalidate_proto_address,
				  "invalidate_proto_address");
    reg_add_default_sprintf_address_link(sprintf_unknown_address,
					 "sprintf_unknown_address");
    reg_add_default_sprintf_address_addr(sprintf_unknown_address,
					 "sprintf_unknown_address");
    reg_add_default_sprintf_src_dst(sprintf_unknown_src_dst,
				    "sprintf_unknown_src_dst");
    reg_add_default_access_check((service_access_check_type)
				 return_true, "return_true");
    reg_add_default_valid_access_list((service_valid_access_list_type)
				      return_false, "return_false");
    reg_add_default_media_getlink((service_media_getlink_type) 
				  return_link_illegal, "return_link_illegal");
    reg_add_default_media_type_to_link(LINK_ILLEGAL, "link_illegal");
    reg_add_default_media_link_to_type(0L, "illegal type");
    reg_add_default_media_link_to_snaptype(0L, "illegal type");
    reg_add_default_media_sap_to_link(LINK_ILLEGAL, "link_illegal");
    reg_add_default_media_transmit_delay(default_command,
					 "transmit_delay_default_command");
    reg_add_default_media_minimum_carrier_delay(0,"minimum carrier delay");

    reg_add_default_compress_header(default_compress_header,
				    "default_compress_header");
    reg_add_default_encaps_nvram_write(default_encaps_nv_write, 
				"default_encaps_nv_write");
    reg_add_default_encaps_show_protocol_attr(default_encaps_display, 
					" default_encaps_display");
    reg_add_default_get_subint_ref_num_from_pak(
	(service_get_subint_ref_num_from_pak_type)return_false,"return_false");
	   
    /*
     * Add a bridge "stub" default to discard frames we'd normally
     * consider game for bridging
     */
    reg_add_bridge_enqueue(net_bridge_enqueue_discard,
			   "net_bridge_enqueue_discard");   
    reg_add_default_clns_parse_addr_map((service_clns_parse_addr_map_type)
					return_true, "return_true");
    reg_add_default_interface_command_ok((service_interface_command_ok_type)
					 return_true, "return_true");
    reg_add_default_media_half_or_full_duplex_cmd(
            default_media_half_or_full_duplex_params_command,
           "default_media_half_or_full_duplex_params_command");
}

void create_queue_registry (void)
{
    int i;

    /*
     * Using fast switch registry, so there is no
     * "reg_default_fair_queue_flow_id"
     */
    for (i = LINK_ILLEGAL; i < LINK_MAXLINKTYPE; i++) {
	reg_add_determine_RED_precedence(i,
			(service_determine_RED_precedence_type) return_false,
					 "RED_precedence_0");
	reg_add_determine_fair_queue_flow_id (i, linktype_is_fq_conversation,
					      "linktype_is_fq_conversation");
    }
}

/*
 * show_queue_command
 *
 * Display queue contents. This function displays the type of packets that
 * are queued on an inteface output queue. In some cases, the network layer
 * contents are displayed.
 *
 * This function should be modified by anyone that wants to see other
 * network layer headers.
 */
void show_queue_command (parseinfo *csb)
{

    idbtype     *swidb;
    hwidbtype   *idb;
    int         queuenum;

    swidb = GETOBJ(idb,1);
    if (!swidb)
	return;
    idb = swidb->hwptr;
    if (!idb)
	return;

    automore_enable(NULL);
    /*
     * Determine which queue to look at. If none specified, use
     * default queue.
     */
    queuenum = GETOBJ(int,1);
    if (queuenum == -1)
	queuenum = DEFAULT_QUEUE(idb);

    if (idb->fair_queue_head) {
	show_fair_queue_command(idb);
    } else {
	show_priority_queue_command(swidb, idb, queuenum);
    }
    automore_disable();
}

static void
snapshot_fair_queue (queuetype *messages, 
		     fair_queue *queue, 
		     int queue_count) 
{
    paktype *pak;
    for (;--queue_count >= 0; queue++) {

	/*
	 * Don't print empty conversations.
	 */
	pak = pak_queue_get_head(&queue->q);
	if (pak) {
	    pak_lock(pak);
	    pak->fair_conversation = pak_queue_depth(&queue->q);
	    pak_enqueue(messages, pak);
	}
    }
	    
}
void show_fair_queue_command (hwidbtype   *idb)
{

    ushort status;
    paktype     *pak;
    ushort      *ptr;
    fair_queue_head *fq = idb->fair_queue_head;
    fair_queue  *queue;
    int 	conversation;
    queuetype   messages;

    if (fq) {
	queue_init(&messages, 0);
	holdq_display(idb);
	printf("\n");
	if ((0 != fq->fq_active_conversations) ||
	    (0 != fq->fq_allocated_conversations)) {

	    /*
	     * determine what messages to display
	     */
	    status = raise_interrupt_level(NETS_DISABLE);
	    if (fq->fq_hashed_queue)
		snapshot_fair_queue(&messages, 
				    fq->fq_hashed_queue, 
				    1 + fq->fq_hash_mask);
	    snapshot_fair_queue(&messages, 
				fq->fq_link_queue, 
				1 + FAIR_LINK_TYPE_HASH);
	    if (fq->fq_reserved_queue)
		snapshot_fair_queue(&messages, 
				    fq->fq_reserved_queue, 
				    fq->fq_queue_count - fq->fq_reserved_queue_start);
	    reset_interrupt_level(status);

	    /*
	     * display the messages
	     */
	    while ((pak = pak_dequeue(&messages))) {
		if (automore_quit())
	    	    break;

		conversation = pak->oqnumber;
		queue = fair_queue_address(fq, conversation);
		printf("\n  (depth/weight/discards) %d/%d/%d\n",
		       pak->fair_conversation, pak->fair_weight,
		       queue->fq_discards - queue->si_fq_discards);

		printf("  Conversation %d, linktype: %s, length: %d",
		       conversation, print_linktype(pak->linktype),
		       pak->datagramsize);

		/*
		 * Call registry to display network layer header. Protocol
		 * specific function returns pointer to data.
		 */
		ptr = NULL;
		reg_invoke_print_network_header(pak->linktype, pak, &ptr);

		/*
		 * Display 32 bytes of data after network layer header.
		 */
		datagram_done(pak);
		printf("\n");
	    }
	    while ((pak = pak_dequeue(&messages)))
		datagram_done(pak);
	}
	printf("\n");
    }
}

static void show_priority_queue_command (idbtype     *swidb,
					 hwidbtype   *idb, 
					 int queuenum)
{

    paktype     *pak;
    elementtype *element;
    int         i, j, count, max;
    ushort      *ptr;
    ushort      status;
    queuetype   queue;

    /*
     * control interrupts while we make a copy of the queue
     */
    status = raise_interrupt_level(NETS_DISABLE);

    count = idb->outputq[queuenum].count;
    max = idb->outputq[queuenum].maximum;
    queue_init(&queue, max);

    if (count == 0) {
	reset_interrupt_level(status);
	return;
    }

    element = idb->outputq[queuenum].qhead;
    for (i = 0; i < count; i++, element = element->next) {
	if (!element)
	    break;
	pak = element->pakptr;		/* get the buffer */
	if (!pak) 
	    break;
	pak_lock(pak);			/* lock the buffer */
	pak_enqueue(&queue, pak);	/* save it for display */
    }
    reset_interrupt_level(status);

    /*
     * identify the queue being displayed
     */
    printf("\nOutput queue for %s is %d/%d\n", swidb->namestring, count, max);

    /*
     * peel paks off the copy of the queue and display them.
     *
     * the main queue, of course, continues to be transmitted
     */
    i = 1;
    while ((pak = pak_dequeue(&queue))) {
	if (automore_quit())
	    break;
	printf("\nPacket %d, linktype: %s, length: %d, flags: 0x%x", i, 
	       print_linktype(pak->linktype), pak->datagramsize, pak->flags);

	/*
	 * Call registry to display network layer header. Protocol specific
	 * function returns pointer to data.
	 */
	ptr = NULL;
	reg_invoke_print_network_header(pak->linktype, pak, &ptr);

	/*
	 * Display 32 bytes of data after network layer header.
	 */
	if (ptr) {
	    printf("\n    data: ");
	    for (j = 0; j < 7; j++, ptr++)
		printf("0x%04x ", *ptr);
	    printf("\n          ");
	    for (j = 0; j < 7; j++, ptr++)
		printf("0x%04x ", *ptr);
	}
	datagram_done(pak);
	printf("\n");
	i++;
    }

    /*
     * if auto-more popped us out, discard any remaining paks
     */
    while ((pak = pak_dequeue(&queue))) {
	datagram_done(pak);
    }
}


/*
 * fair_clear_counters
 *
 * Clear interface counters having to do with fair queuing.
 * (Called via registry from clear_idb.)
 */
static void
snapshot_fair_queue_counters (fair_queue *queue, int queue_count) {
    for (;--queue_count >= 0; queue++)
        queue->si_fq_discards = queue->fq_discards;
}

void fair_clear_counters (hwidbtype *idb) {
    fair_queue_head     *fq;

    if((fq = idb->fair_queue_head) != NULL) {

        if (fq->fq_hashed_queue)
            snapshot_fair_queue_counters(fq->fq_hashed_queue,
                                         1 + fq->fq_hash_mask);
        snapshot_fair_queue_counters(fq->fq_link_queue,
                                     1 + FAIR_LINK_TYPE_HASH);
        if (fq->fq_reserved_queue)
            snapshot_fair_queue_counters(fq->fq_reserved_queue,
			   fq->fq_queue_count - fq->fq_reserved_queue_start);
    }
}

/*
 * clear RED counters
 */
void red_clear_counters (hwidbtype *idb) {
    red_parameters      *redp;
    short i;

    if((redp = idb->red_parameters) != NULL) {
        for (i = RED_PRECEDENCE_LEVELS; --i >= 0;)
            redp->si_packets_marked[i] = redp->packets_marked[i];
    }
}



/*
 * Function to return the max MTU for a particular protocol.  Returns the
 * interface MTU if the IDB vector is uninitialized.
 */
ulong 
max_proto_mtu (idbtype *idb, enum LINK proto)
{
    return (idb->hwptr->max_proto_mtu ?
	    (*idb->hwptr->max_proto_mtu)(idb,proto, FALSE) : 
	    idb->hwptr->maxmtu);
}


/*
 * def_proto_mtu
 *
 * Function to return the default MTU for a particular protocol.  Returns
 * the interface MTU if the IDB vector is uninitialized.  Returns
 * the correct tunnel interface MTU, if a call back function for
 * the tunnel is registered.
 */
ulong def_proto_mtu (idbtype *idb, enum LINK proto)
{
    int def_mtu;

    if (idb->hwptr && (idb->hwptr->status & IDB_TUNNEL) &&
	 reg_used_media_tunnel_mtu(proto)) {
        def_mtu = reg_invoke_media_tunnel_mtu(proto, idb);
    } else {
	def_mtu = idb->hwptr->max_proto_mtu ?
		  (*idb->hwptr->max_proto_mtu)(idb, proto, TRUE) :
		  idb->hwptr->maxmtu;
    }

    return(def_mtu);
}

/*
 * Function to add a multicast address to the interface's MAC address filter.
 */
void
if_multi_add (hwidbtype *hwidb, idbtype *swidb, uchar *mac)
{
    if (hwidb->listen)
	(*hwidb->listen)(swidb, mac, 1 /*FSM_BROADCAST*/);
}

/*
 * if_maxdgram_adjust()
 * Adjust the maxdgram size, also make sure max_pak_size is setup to the
 * correct value
 */
void if_maxdgram_adjust (hwidbtype *hwidb, uint maxdgram)
{

    hwidb->max_buffer_size = maxdgram;

    /*
     * Adjust the max packet size, if needed
     */
    if ((hwidb->type & IDB_SERIAL) || is_atm(hwidb)) {
	hwidb->max_pak_size = maxdgram;
    } else {
	if (maxdgram < hwidb->max_pak_size_def) {
	    hwidb->max_pak_size = maxdgram;
	} else {
	    hwidb->max_pak_size = hwidb->max_pak_size_def;
	}
    }
}
      
