/* $Id: frame_relay.c,v 3.32.4.65 1996/09/11 16:49:26 liqin Exp $
 * $Source: /release/112/cvs/Xsys/wan/frame_relay.c,v $
 *------------------------------------------------------------------
 * frame_relay.c -- Support for Frame Relay Protocol
 *
 * Summer 1990, Greg Fowler
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: frame_relay.c,v $
 * Revision 3.32.4.65  1996/09/11  16:49:26  liqin
 * CSCdi68280:  replace malloc with malloc_named in frame relay code
 * Branch: California_branch
 *
 * Revision 3.32.4.64  1996/09/04  18:25:06  liqin
 * CSCdi66691:  frame-relay interface-dlci shouldnt have broadcast option
 * Branch: California_branch
 *
 * Revision 3.32.4.63  1996/08/30  04:44:21  ccpoe
 * CSCdi67157:  2511 may reload at _bridge_enq when no bridging is
 * configured.
 * Branch: California_branch
 *
 * Revision 3.32.4.62  1996/08/28  13:21:37  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.32.4.61  1996/08/13  23:22:58  rtio
 * CSCdi59352:  Show-frame-pvc does not accurately display the packets or
 * bytes
 * Branch: California_branch
 * Added code to put dlci/dfa in swidb->p2pmac for p2p subifs so ip mcast
 * cache can pass this number to ip_pak_write_mac_inline().
 *
 * Revision 3.32.4.60  1996/08/13  21:11:33  wilber
 * CSCdi61387:  Crash in bridging when deconfigure frame-relay interface
 * dlci
 * Don't overwrite fr_pvc->idb to the main interface(firstsw)
 * Branch: California_branch
 *
 * Revision 3.32.4.59  1996/08/07  05:49:28  turadek
 * CSCdi61758:  Frame relay compression type not displayed
 * Branch: California_branch
 * Add code to show payload compression status.
 *
 * Revision 3.32.4.58  1996/08/03  23:41:39  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.32.4.57  1996/08/03  05:34:58  vrawat
 * CSCdi64662:  fr inverse arp appletalk test crashes 7000 in unconfig
 * Branch: California_branch
 *
 * Revision 3.32.4.56  1996/07/23  13:31:30  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.32.4.55  1996/07/17  06:12:11  fowler
 * CSCdi57463:  Frame relay broadcast queue should use new IDB list
 * functions
 *              Remove several instances of FOR_ALL_HWIDBS and replace
 *              with walks of the FR idb list
 * Branch: California_branch
 *
 * Revision 3.32.4.54  1996/07/15  05:38:00  ccpoe
 * CSCdi62493:  fr lmi stopped running at startup. Old state doesn't need
 * to be in IDBS_ADMINDOWN to start the lmi idb timer.
 * Branch: California_branch
 *
 * Revision 3.32.4.53  1996/07/13  11:41:55  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.32.4.52  1996/07/10  09:57:53  dawnli
 * CSCdi54969:  HSRP tracking does not track Frame Relay sub-interfaces
 * Branch: California_branch
 *
 * Revision 3.32.4.51  1996/07/10  07:37:51  chifei
 * CSCdi60658:  Clear counters clears frame relay MIB objects
 * Branch: California_branch
 *
 * Revision 3.32.4.50  1996/07/04  17:05:47  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Moved command handling out of frame_relay_command()
 *
 * Revision 3.32.4.49  1996/07/03  01:57:31  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.32.4.48  1996/07/01  17:23:13  turadek
 * CSCdi54825:  Possible memory leak during frame relay compression
 * Branch: California_branch
 * release incoming pak when recovering rare error conditions
 *
 * Revision 3.32.4.47  1996/06/28  23:32:14  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.32.4.46  1996/06/27  21:38:29  skoh
 * CSCdi59475:  Change units for Be/Bc from bytes to bits
 * Branch: California_branch
 *
 * Revision 3.32.4.45  1996/06/27  09:45:00  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.32.4.44  1996/06/26  22:40:50  turadek
 * CSCdi61379:  frame-relay packets compressed in error
 * Branch: California_branch
 * assure down-stream compress bit is initialized to prevent random
 * compression of packets when mixing IETF and CISCO encapsulation
 * across subinterfaces
 *
 * Revision 3.32.4.43  1996/06/25  14:40:03  shankar
 * CSCdi61103:  Broadcast triggering of SVCs needs a configuration hook
 * for users
 * Branch: California_branch
 *
 * Revision 3.32.4.42  1996/06/19  06:24:12  fowler
 * CSCdi55550:  Bridging over Frame Relay failed when router acting as IP
 * host
 * Branch: California_branch
 *
 * Revision 3.32.4.41  1996/06/18  23:21:49  widmer
 * CSCdi59768:  DLCI is not validated in EZSetup
 * Branch: California_branch
 *
 * Revision 3.32.4.40  1996/06/18  01:50:08  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.32.4.39  1996/06/12  14:33:37  fowler
 * CSCdi60166:  Crash at fr_enque and fr_oqueue
 *              Make sure idb initialization complete before setting
 *              up queuing
 * Branch: California_branch
 *
 * Revision 3.32.4.38  1996/06/11  23:38:30  skoh
 * CSCdi58274:  Incorrect mincir value shown in show fram svc map command
 * Branch: California_branch
 *
 * Revision 3.32.4.37  1996/06/11  06:05:14  shankar
 * CSCdi60091:  CSCdi58766 undid some changes made by CSCdi57332
 * Branch: California_branch
 *
 * Revision 3.32.4.36  1996/06/10  21:45:36  chifei
 * CSCdi55550:  ip bridging failures over frame relay
 * Branch: California_branch
 * Make pak->mac_start pointing to dlci when leaving fr_doencaps()
 *
 * Revision 3.32.4.35  1996/06/07  17:14:12  skoh
 * CSCdi57690:  Broadcasts must not trigger SVCs
 * Branch: California_branch
 *
 * Revision 3.32.4.34  1996/06/07  16:57:02  shankar
 * CSCdi58336:  Remove some legacy code from show_frame_relay_maplist
 * Branch: California_branch
 *
 * Revision 3.32.4.33  1996/06/06  22:51:59  wmay
 * CSCdi58782:  down line stays stuck looped - clear inloopback field when
 * changing state
 * Branch: California_branch
 *
 * Revision 3.32.4.32  1996/06/06  16:24:08  fowler
 * CSCdi58766:  Link errors in 11.2 build
 *              Restore nvgen code for header compression
 * Branch: California_branch
 *
 * Revision 3.32.4.31  1996/06/05  07:47:09  ccpoe
 * CSCdi58336:  Remove some legacy code from show_frame_relay_maplist
 * Branch: California_branch
 *
 * Revision 3.32.4.30  1996/06/04  13:39:26  fowler
 * CSCdi52609:  removing  priority-dlci-group does not clean up PVC status
 * Branch: California_branch
 *
 * Revision 3.32.4.29  1996/06/01  07:06:28  tylin
 * CSCdi53715:  CDP is not working over Fr-Rel, X25 and SMDS interfaces
 *    Remove multicast_status field reset for path status changes
 * Branch: California_branch
 *
 * Revision 3.32.4.28  1996/05/31  18:19:44  vrawat
 * CSCdi58846:  frame-relay debugging enhancements
 * Branch: California_branch
 *
 * Revision 3.32.4.27  1996/05/30  23:27:58  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.32.4.26  1996/05/30  18:36:16  ccpoe
 * CSCdi58545:  FR Tunneling pvc was not shown by "show frame-relay pvc"
 * command
 * Branch: California_branch
 *
 * Revision 3.32.4.25  1996/05/29  06:35:07  fowler
 * CSCdi58766:  Link errors in 11.2 build
 * Branch: California_branch
 *
 * Revision 3.32.4.24  1996/05/24  23:52:50  ccpoe
 * CSCdi58376:  spurious access in fr_interface. HUNTGROUP_LEADER() does
 * not check null parameter. Added checking.
 * Branch: California_branch
 *
 * Revision 3.32.4.23  1996/05/24  21:21:44  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.32.4.22  1996/05/23  14:32:07  chifei
 * CSCdi57332:  FR SVC initialization point needs to be re-examined
 * Branch: California_branch
 * This is layer 2 specific change.
 *
 * Revision 3.32.4.21  1996/05/22  23:57:09  pbettink
 * CSCdi57783:  Restoring Frame Relay minimum packet size to 4
 * Branch: California_branch
 *
 * Revision 3.32.4.20  1996/05/22  23:38:12  shankar
 * CSCdi57332:  FR SVCs are initialized only when user configures it.
 * Branch: California_branch
 *
 * Revision 3.32.4.19  1996/05/22  20:55:16  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.32.4.18  1996/05/22  00:39:17  skoh
 * CSCdi57751:  Encapsulation type for SVC static maps not being displayed
 * correctly
 * Branch: California_branch
 *
 * Revision 3.32.4.17  1996/05/17  21:13:33  ccpoe
 * CSCdi57877:  Frame relay traffic shaping breaks modular images
 * Branch: California_branch
 *
 * Revision 3.32.4.16  1996/05/17  12:16:26  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.29.2.5  1996/05/14  03:19:17  kpatel
 * CSCdi55075:  Cant define dlsw direct encapsulation for more that pvc
 *              dls frame-relay does not work over point to point
 *              sub-interface (CSCdi55085)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.29.2.4  1996/05/06  00:08:47  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.29.2.3  1996/04/26  07:59:46  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.29.2.2  1996/04/03  22:12:05  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.29.2.1  1996/03/17  18:54:24  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.32.4.15  1996/05/15  05:54:55  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.32.4.14  1996/05/13  23:21:50  gklee
 * CSCdi55713:  FR debugging messages should warn about unrecognized
 * bridge pkts. Added error checking of LINK_ILLEGAL in fr_raw_enqueue()
 * to handle packets w/ illegal Ethertype from 10.2.
 * Branch: California_branch
 *
 * Revision 3.32.4.13  1996/05/13  14:48:12  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.32.4.12  1996/05/09  14:49:15  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.32.4.11  1996/05/05  08:51:23  skoh
 * CSCdi55568:  The E164 address display in show-frame-svc-maplist needs
 * fixing
 * Branch: California_branch
 *
 * Revision 3.32.4.10  1996/05/05  08:18:30  skoh
 * CSCdi56167:  Need consistency in parameneter naming for SVCs and PVCs
 * Branch: California_branch
 *              Simplify and shorten the output of "show fram svc map"
 *              command.
 *
 * Revision 3.32.4.9  1996/05/04  02:17:02  wilber
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
 * Revision 3.32.4.8  1996/04/30  23:27:13  chifei
 * CSCdi52339:  Frame relay DTE/DCE switching across ip tunnel broken
 * Branch: California_branch
 * Enabled the full functionality of FR switching over IP tunnel,
 * also fixed regular switching function which was broken by improper
 * handling of the "frame intf-type" command.
 *
 * Revision 3.32.4.7  1996/04/30  21:38:51  chifei
 * CSCdi56098:  sh int shows svc info when the subsystem doesnt exist
 * Branch: California_branch
 *
 * Revision 3.32.4.6  1996/04/27  06:36:34  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.32.4.5.2.1  1996/04/27  07:39:12  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.32.4.5  1996/04/25  23:24:08  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.32.4.4  1996/04/23  01:10:16  pbettink
 * CSCdi54672:  Unable to Remove Frame-Relay DE-Group Command
 * Branch: California_branch
 *
 * Revision 3.17.8.3  1996/04/25  23:58:34  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.17.8.2  1996/04/17  00:00:53  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.17.8.1  1996/02/23  21:31:29  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.32.4.3  1996/04/17  13:51:17  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.32.4.2.6.1  1996/04/08  02:43:23  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.32.4.2  1996/03/23  01:35:06  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.32.4.1  1996/03/18  22:47:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.32.6.1  1996/03/22  09:45:32  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.32  1996/03/08  23:08:10  fowler
 * CSCdi40775:  clns fails on serial with fr-ietf encapsulation
 *
 * Revision 3.31  1996/03/07  22:41:49  ccpoe
 * CSCdi50271:  misuse of (uchar) casting in arg to fr_set_subif_state()
 *
 * Revision 3.30  1996/02/29  02:07:56  asastry
 * CSCdi49406:  Port fix (for MOP over Frame-Relay) to 10.0 and later
 * releases.
 *
 * Revision 3.29  1996/02/26  23:10:22  ccpoe
 * CSCdi48957: Frame Relay dynamic map does not disappear under multipoint
 * subinterface when "no frame interface-dlci" is issued.
 *
 * Revision 3.28  1996/02/25  06:22:27  ccpoe
 * CSCdi49573:  frame-relay interface-dlci under p2p subif is not correct.
 * PVC should go away when another interface-dlci command is issued.
 *
 * Revision 3.27  1996/02/16  21:30:15  ccpoe
 * CSCdi48228:  Spurious DLCI state change msgs on NNI interfaces with
 * ANSI LMI.
 *
 * Revision 3.26  1996/02/13  08:17:40  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.25.6.1  1996/03/05  05:38:06  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.25.2.1  1996/02/27  21:21:02  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.25  1996/02/07  16:17:20  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.24  1996/02/01  23:53:28  chifei
 * CSCdi46960:  crash in frame_relay_pvc_ie_parse
 * Added validation on IE length and range check on DLCI,
 * also replaced small size bcopy with GETSHORT and GETLONG.
 *
 * Revision 3.23  1996/02/01  19:55:38  turadek
 * CSCdi47505:  Spurious Access @ 0x604BFFF8
 * (fr_payload_compression_command+0x13c)
 * fixes potential unitialized pntr dereference in
 * fr_payload_compression_command.
 * cleans up some sloppy code elsewhere.
 *
 * Revision 3.22  1996/02/01  06:12:06  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.21  1996/01/29  07:31:12  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.20  1996/01/25  11:23:22  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.19.2.1  1996/01/24  23:31:37  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.19  1996/01/22  07:45:37  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.18  1996/01/15  19:17:50  turadek
 * CSCdi46908:  cant configure point-to-point frame-relay payload
 * compression
 *
 * permit the command "frame-relay payload-compress packet-by-packet" to
 * be parsed on a frame-relay encapsulated subinterface.
 *
 * also fixes automatic enabling of "half-compression" (i.e.,
 * decompression of
 * incoming packets without compressing outgoing packets.
 *
 * also fixes a bug relating to enabling/disabling compression on multiple
 * DLCIs via the frame-relay map command
 *
 * also removes an error message when attempt is made to compress a packet
 * with refCount > 1
 *
 * Revision 3.17.2.1  1996/02/15  19:27:07  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.17  1996/01/05  21:04:51  chifei
 * CSCdi46318:  Under some unknown reason, 7000 may reload with bus error
 *
 * Revision 3.16  1996/01/05  10:18:30  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.15  1996/01/02  20:51:38  ccpoe
 * CSCdi45548:  Alignmet error when frame  relay interface transitions
 *
 * Revision 3.14  1995/12/07  22:34:12  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.13  1995/12/06  00:47:38  ccpoe
 * CSCdi44657:  Cannot assign deleted DLCI from 1 FR subinterface to
 * another
 *
 * Revision 3.12  1995/12/04  19:45:42  chifei
 * CSCdi44627:  Configuring dialer on a frame relay link crashes box
 * Shouldn't use a NULL pointer.
 *
 * Revision 3.11  1995/12/04  19:20:54  chifei
 * CSCdi44856:  Debug framerelay packet misprint
 *
 * Revision 3.10  1995/11/30  08:20:27  ccpoe
 * CSCdi40866:  Inverse ARP broken after removal of frame-relay map
 *
 * Revision 3.9  1995/11/30  07:01:12  ccpoe
 * CSCdi43927:  Frame-relay:  IP TCP Header-Compression causes router crash
 *
 * Revision 3.8  1995/11/30  06:23:45  ccpoe
 * CSCdi44651:  ALIGN-3-SPURIOUS: Spurious memory access when tunneling
 *
 * Revision 3.7  1995/11/29  23:46:20  shankar
 * CSCdi44620:  autoinstall caused a wrong frame relay map
 *
 * Revision 3.6  1995/11/22  09:19:56  shj
 * CSCdi44330:  LANE Server stays down after <no shut> on interface
 * Keep hands off non-fr interfaces (CSCdi40880 broke ATM/LANE).
 *
 * Revision 3.5  1995/11/21  20:44:11  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * syncing code to 11.0
 *
 * Revision 3.4  1995/11/20  22:28:31  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  18:02:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:43  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:49:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.53  1995/11/08  21:35:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.52  1995/10/13  21:27:49  ccpoe
 * CSCdi40880:  Multipoint frame relay subinterfaces should be up when no
 * PVCs defin and bulid the link between physical interface and
 * subinterface.
 *
 * Revision 2.51  1995/10/12  20:59:48  ccpoe
 * CSCdi41674:  bad getbuffer, bytes= 524293
 *
 * Revision 2.50  1995/10/11  14:37:02  ccpoe
 * CSCdi41462:  no int s.x should clear associated FR commands. All pvc is
 * cleared when int s.x is deleted.
 *
 * Revision 2.49  1995/09/20  17:19:30  ccpoe
 * CSCdi40018:  Frame-relay priority-dlci-group command disapears w/ state
 * change. existing dynamic PVC should changed to static when this command
 * is configured. and it should stay static when inverse ARP creates map
 * entry.
 *
 * Revision 2.48  1995/09/15  02:14:42  dcoli
 * CSCdi39433:  Bad Encapulation Error
 * Break frame_relay and atm_dxi into separate object modules.
 * Make link2nlpid and nlpid2link inline functions so that they can be
 * shared without the overhead of a registry call.
 *
 * Revision 2.47  1995/09/15  00:12:47  ccpoe
 * CSCdi40106:  frame relay: 7010 router crashed at fr_clear_map. add a
 * check to not to break the map chain when hashing breaks.
 *
 * Revision 2.46  1995/09/13  20:59:51  fowler
 * CSCdi36199:  Frame relay IETF encapsulation for OSI doesnt comply with
 * 1490
 *
 * Revision 2.45  1995/09/13  07:02:01  hampton
 * All code should use the routine GETSHORT()/PUTSHORT() instead of using
 * the routines getshort()/putshort().  [CSCdi40249]
 *
 * Revision 2.44  1995/09/11 20:31:04  ccpoe
 * CSCdi39916:  FR point-to-point subifs not tracking DLCI state. Subint
 * should be marked down when frame_relay_periodic is marking all the PVC
 * inactive.
 *
 * Revision 2.43  1995/09/11  19:55:57  fowler
 * CSCdi39555:  FR DLCI should go away completely after the no fr interface
 *              dlci command
 *
 * Revision 2.42  1995/09/11  18:32:36  fowler
 * CSCdi40023:  Frame relay p2p subinterfaces dont work
 *
 * Revision 2.41  1995/09/10  02:33:00  shankar
 * CSCdi40046:  Attempt ANSI LMI befor CCITT LMI for Autoinstall
 * Performance improvement that'll help our US customers.
 *
 * Revision 2.40  1995/09/09  00:54:30  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.39  1995/09/01  23:06:31  asastry
 * CSCdi39698:  WFQ: Display info (in "show queue") for non-IP protocols
 * as well
 *
 * Revision 2.38  1995/09/01  05:10:54  ccpoe
 * CSCdi39496:  Deleted PVC can not come back to ACTIVE.
 *
 * Revision 2.37  1995/08/31  18:15:56  ccpoe
 * CSCdi39073:  Frame Relay: show frame relay pvc shows wrong pvc status.
 * Status received from LMI should not be altered in
 * fr_clear_dynamic_entries.
 *
 * Revision 2.36  1995/08/30  21:14:15  wmay
 * CSCdi39520:  Frame relay DLCI not set right for bridged ARPs - realign
 * the pak before adjusting datagramstart and size and passing to
 * raw_enqueue.
 *
 * Revision 2.35  1995/08/30  16:22:08  richl
 * CSCdi39553:  Modularity: FR regcode global breaks modular-dlsw
 *
 * Revision 2.34  1995/08/29  16:55:53  achopra
 * CSCdi39449:  debug frame-relay packet displays incorrect dlci number
 *
 * Revision 2.33  1995/08/28  21:32:17  shankar
 * CSCdi39401:  Autoinstall over FR broken if DLCIs are not unique
 *
 * Revision 2.32  1995/08/23  22:52:24  ronnie
 * CSCdi38804:  InARP on ATM interfaces doesnt populate ARP table
 *         Many places were passing a complete addrtype to the hashing
 *         function, when only the first few bytes were set.  This made
 *         the hash function nondeterministic.
 *
 * Revision 2.31  1995/08/23  17:14:53  ccpoe
 * CSCdi38822:  Frame relay status is not accurate in show frame PVC/map.
 * fr_set_pvc_status() is now used to change the pvc_status.
 *
 * Revision 2.30  1995/08/21  19:28:57  ccpoe
 * CSCdi37923:  possible segV exception in fr_sort_packet when router is
 * reloaded. fr_sort_packet() should check if packets tossed by tcp
 * compression code.
 *
 * Revision 2.29  1995/08/21  19:20:33  ccpoe
 * CSCdi38159:  DCE advertises wrong LMI information when DLCI removed.
 * Now we handles dte, dce with seperate
 * functions--fr_subif_dte_statechange() and fr_subif_dce_statechange().
 *
 * Revision 2.28  1995/08/17  07:15:21  ccpoe
 * CSCdi38328:  srt registry is not created if tbridge subsys doesn't
 * exist. Check srt registry's presence before do a
 * reg_add_srt_learn_source.
 *
 * Revision 2.27  1995/08/16  15:00:07  shankar
 * CSCdi38521:  router crash while testing clns isis routing,frame-relay
 * encap
 *
 * Revision 2.26  1995/08/16  06:04:06  ccpoe
 * CSCdi38475:  Parser does not accept frame-relay de-group subcommand
 * during reload.
 *
 * Revision 2.25  1995/08/14  17:16:38  fowler
 * CSCdi36923:  Frame relay inverse-arp and priorty list ordering problem.
 *
 * Revision 2.24  1995/08/11  18:34:16  wmay
 * CSCdi38645:  Invalid media message received while bridging on
 * subinterfaces - propogate tbridge_media type to subinterfaces - also
 * add point to point subints to bridge flood list
 *
 * Revision 2.23  1995/08/11  05:12:40  ccpoe
 * CSCdi37844:  alignment error in fr_is_looped. Type casting of a
 * mis-aligned pointer to a longword aligned union and its reference cause
 * the alignment error.
 *
 * Revision 2.22  1995/08/10  00:11:31  achopra
 * CSCdi37946:  conf mem issues incorrect %FR-4-MAPERROR error message
 *
 * Revision 2.21  1995/08/08  16:50:35  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.20  1995/08/04 21:30:38  fowler
 * CSCdi37783:  FR broadcast queue count incremented for for all broadcasts
 *
 * Revision 2.19  1995/08/02  21:26:42  ccpoe
 * CSCdi37746:  FR PVCs may become out of sync with subif state.
 * Subinterface comes back after it is deleted. fr_if_statechange_complete
 * is implemented to make subif state in sync with PVC state.
 *
 * Revision 2.18  1995/07/26  18:52:25  fowler
 * CSCdi32576:  "no fram route 200" command causes traceback.
 *              Correct PVC count on an interface
 *
 * Revision 2.17  1995/07/26  18:05:27  shankar
 * CSCdi37677:  Send_inarp code may access freed memory
 *
 * Revision 2.16  1995/07/26  03:26:17  ccpoe
 * CSCdi37429:  INverse arp should be enabled by default. Frame-relay PVC
 * dlci number was wrong on nv gen.
 *
 * Revision 2.15  1995/07/20  23:33:03  achopra
 * CSCdi29094:  FR LMI parsing should detect a looped line
 *
 * Revision 2.14  1995/07/11  21:03:15  ccpoe
 * CSCdi36701:  %ALIGN-3-SPURIOUS: Spurious memory access made at
 * 0x60008A50 reading
 *
 * Revision 2.13  1995/07/01  07:16:23  fowler
 * CSCdi36640:  FR interface-dlci for multipoint subifs can be lost from
 *              wr ter
 *
 * Revision 2.12  1995/06/27  16:39:12  ccpoe
 * CSCdi35924:  admin. shut subinterfaces come up when configure
 * interface-dlci. A new function fr_set_subif_state() manages all the
 * sub-interface state now.
 *
 * Revision 2.11  1995/06/26  14:03:24  fowler
 * CSCdi33765:  Frame Relay dlci prioritization broken in dynamic mapping
 *
 * Revision 2.10  1995/06/18  06:19:36  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.9  1995/06/15 06:14:54  ccpoe
 * CSCdi35764:  logging of alleged DLCI status changes. PVC needs to be
 * inactive before it is cleared in this case.
 *
 * Revision 2.8  1995/06/15  06:03:44  dbath
 * CSCdi33071:  IPX fast switching, over equal cost ATM paths is broken
 * Replaced PAK_FR_CACHED flag with a more generic one, PAK_ENCAPS_ONLY,
 * so that we could also use it in ATM-land.  Made IPX ATM fastswitch
 * cache population code cognisant of multiple equal cost paths,
 * like the frame relay case.
 *
 * Revision 2.7  1995/06/11  22:23:13  fowler
 * CSCdi35518:  Outbound frame relay DE bit counters arent correct
 *
 * Revision 2.6  1995/06/09  13:25:20  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.5  1995/06/09 05:15:03  ccpoe
 * CSCdi35078:  Admin. shut subinterfaces come up when dlci status changes
 *
 * Revision 2.4  1995/06/09  00:40:00  ccpoe
 * CSCdi34943:  Subinterfaces not deleted when switching from/back to
 * frame relay
 *
 * Revision 2.3  1995/06/09  00:00:51  ccpoe
 * CSCdi32945:  FR NV generation shouldnt generate commands for
 * non-running
 * protocols. Cache the value returns from subsys_find_by_name() at init
 * time,
 * and use it to check when we do a nvgen.
 *
 * Revision 2.2  1995/06/08  20:52:55  wmay
 * CSCdi35517: Extra field in swidb-bridge_flood_first ==> tbridge_circuitQ
 *
 * Revision 2.1  1995/06/07  23:17:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "../os/old_timer_callbacks.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "../srt/srt_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "ieee.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "config.h"
#include "../ui/debug.h"
#include "serial.h"
#include "dialer.h"
#include "address.h"
#include "frame_relay.h"
#include "fr_debug.h"
#include "serial_debug.h"
#include "logger.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../if/msg_lineproto.c"	/* Not a typo, see logger.h */
#include "../wan/msg_frame_relay.c"	/* Not a typo, see logger.h */
#include "../x25/msg_comp.c"		/* ditto */
#include "../srt/span.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "../if/arp.h"
#include "../if/network.h"
#include "../if/priority_private.h"
#include "../ui/parse_util.h"	/* for parse_proto_addr */
#include "parser.h"
#include "parser_defs_frame.h"
#include "fr_switch.h"
#include "fr_arp.h"
#include "fr_lmi.h"
#include "../wan/fr_traffic_shape.h"
#include "fr_class.h"
#include "../wan/sr_frmib.h"
#include "../xns/novell.h"
#include "../vines/vines.h"
#include "../tcp/tcp.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"
#include "../snmp/ifmib_registry.h"
#include "../llc2/llc2.h"
#include "../servers/bootp.h"
#include "../h/file.h"
#include "../ip/ip_registry.h"
#include "../wan/fr_svc.h"
#include "../wan/dialer_registry.h"
#include "../if/priority_private.h"
#include "../wan/fr_registry.h"
#include "../wan/fr_svc_registry.h"
#include "../parser/parser_defs_compress.h"
#include "../wan/comp_sys.h"
#include "../wan/fr_l2.h"
#include "../util/itemlist.h"
#include "mgd_timers.h"
#include "../wan/fr_vcq.h"


/*
 * prototypes
 */
static boolean fr_payload_compression_command(parseinfo *, frmaptype *);
static frmaptype *findMapEntry(idbtype *idb);

/*
 * Local Storage
 */
static boolean fr_started;
static ushort fr_idb_cnt = 0;
static fr_listtype *fr_listarray[FR_MAXLIST];
frmaptype *relaytable[RELAYHASH];
fr_traffic_t fr_traffic;	/* traffic statistics array */

/* DEBUG */
idbtype *fr_debug_swidb;
hwidbtype *fr_debug_hwidb;
ushort   fr_debug_dlci;

boolean fr_switching = FALSE;
extern parser_mode *fr_dlci_mode;

/*
 * fr_pvc_enqueue
 * Add a PVC to the idb list
*/
static void fr_pvc_enqueue (queuetype *qptr, fr_pvc_info_type *pvc)
{
    fr_pvc_info_type *cur_pvc;
    leveltype status;

    cur_pvc = qptr->qhead;
    status = raise_interrupt_level(NETS_DISABLE);
    insqueue(qptr,pvc,cur_pvc);
    reset_interrupt_level(status);
    return; 
}

/*
 * fr_pvc_unqueue
 * Remove a PVC from the idb list
*/
static void fr_pvc_unqueue (queuetype *q, void *pvc)
{
    leveltype status;
    status = raise_interrupt_level(NETS_DISABLE);
    unqueue(q, pvc);
    reset_interrupt_level(status);
}

/*
 * fr_set_p2pvc: Called to set the current VCC as the p2p VCC on
 *      the specified ATM interface.
 */
static boolean
fr_set_p2pvc (idbtype *idb, ushort dlci)
{
    if (idb->p2pmac) {
        return(FALSE);
    }
    idb->p2pmac = malloc_named(sizeof(hwaddrtype), "FR HW Address");
    if (!idb->p2pmac) {
        return(FALSE);
    }
    idb->p2pmac->type = STATION_FR10;
    idb->p2pmac->length = STATIONLEN_FR10;
    memcpy(idb->p2pmac->addr, &dlci, STATIONLEN_FR10);
    return (TRUE);
}

/*
 * fr_clear_p2pvc:     Called to remove a VCC from a p2p interface
 */
static void
fr_clear_p2pvc (fr_pvc_info_type* frvc)
{
    idbtype *idb;

    idb = frvc->idb;
    if(!idb->p2pmac) {
        return;
    }
    free(idb->p2pmac);
    idb->p2pmac = NULL;
}

/*
 * Locate the pvc occupying this p2p subif.
 */
static fr_pvc_info_type *find_p2p_pvc_from_swidb (idbtype *swidb)
{
    int ix;
    hwidbtype *hwidb;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *temp;

    hwidb = swidb->hwptr;
    fr_idb = hwidb->frame_relay_stuff;

    for (ix = MIN_DLCI; ix <= MAX_DLCI; ix++) {
	temp = fr_idb->fr_dlci_tab[ix];
	if (temp && temp->idb == swidb)
	    return(temp);
    }
    return(NULL);
}


/*
 * fr_idblist_add
 * Add an idb to the lsit of FR idbs after
 * creating list if necessary
 */
static void fr_idblist_add (hwidbtype *hwidb)
{
    ulong idblist_type;
    list_header *list;

    list = idb_get_listhead(HWIDBLIST_FR);
  
    if (list == NULL) {
	if (!idb_create_list(HWIDBLIST_FR, &idblist_type)) {
		idb_report_create_error("frame relay");
	}
    }
    if (!idb_add_hwidb_to_list(hwidb, HWIDBLIST_FR)) {
	if (frame_relay_debug) {
		idb_report_add_error("frame relay");
	}
    }
}

/*
 * fr_idblist_remove
 * Remove idb from the list of FR idbs
 * & destroy the list if empty
 */
static void fr_idblist_remove (hwidbtype *idb)
{

    if (!idb_remove_from_list(idb,HWIDBLIST_FR)) {
	if (frame_relay_debug)
	    idb_report_removal_error("frame relay");
    }
}

/*
 * fr_resync_pvc_count()
 *   Re-calculate num_idb_pvcs per interface to match fr_dlci_tab entries
 *   num_idb_pvcs is for buffer space estimate when generating Full Status
 *   so don't count SVCs.
 */
static void fr_resync_pvc_count (
			  fr_idb_struct_type *fr_idb,
			  hwidbtype *idb)
{
    ushort dlci, new_cnt;
    fr_pvc_info_type *fr_pvc;

    new_cnt = 0;
    for (dlci = 0; dlci < MAX_DLCI; dlci++) {
	if ((fr_pvc = fr_idb->fr_dlci_tab[dlci]) &&
    	    (fr_pvc->pvc_type != SVC_DYNAMIC))
	    new_cnt++;
    }
    if (new_cnt != fr_idb->num_idb_pvcs) {
	if (frame_relay_debug)
            buginf("\n%s: Sync #PVC - was %d, now %d", idb->hw_namestring,
		fr_idb->num_idb_pvcs, new_cnt);
	fr_idb->num_idb_pvcs = new_cnt;
    }
}

/*
 * fr_dec_pvc_count
 * - Decrement PVC count
 */
void fr_dec_pvc_count (
    fr_idb_struct_type *fr_idb,
    hwidbtype *idb)
{
    if (fr_idb->num_idb_pvcs-- == 0)
	fr_resync_pvc_count(fr_idb, idb);
}


boolean frame_relay_lmi_debug(hwidbtype *hwidb)
{
    if (!frlmi_debug)
        return(FALSE);

    if(fr_debug_hwidb && (fr_debug_hwidb != hwidb))
        return(FALSE);

    return(TRUE);
}



boolean frame_relay_packet_debug(idbtype *idb, ushort dlci)
{

    idbtype *first_sw;
     
    if (idb)
        first_sw = idb->hwptr->firstsw;
    else
        first_sw = NULL;

    if (!frpkt_debug)
        return(FALSE);

    if (fr_debug_swidb)
        if ((fr_debug_swidb != idb) && (fr_debug_swidb != first_sw))
            return(FALSE);
        else
            /* debug output, if debugging per dlci is defined on given
	       dlci OR its value is IGNORE_DLCI */
            if (fr_debug_dlci && (fr_debug_dlci != dlci) && (dlci != IGNORE_DLCI))
                return(FALSE);

    return(TRUE);
}

/*
 * Frame relay packet debug
 */
static void debug_fr_pak (
    idbtype *idb,
    paktype *pak,
    ushort dlci,
    boolean bridged,
    enum INOUT direction)
{
    fr_idb_struct_type *fr_idb;
    frame_relay_hdr *hdr;
    ether_hdr *ether;
    snap_hdr *snap;
    char display, buffer[20];
    ushort tmp_dlci, pak_dlci, type;

    fr_idb = idb->hwptr->frame_relay_stuff;
    hdr = (frame_relay_hdr *)pak->datagramstart;
    tmp_dlci = GETSHORT(&hdr->dlci);
    type = GETSHORT(&hdr->type);

    if (direction == INPUT) {
	display = 'i';
	buffer[0] = '\0';
        pak_dlci = GETSHORT(&pak->fr_dlci);
    } else {
	display = 'o';
	sprintf(buffer, "(%s)", link2name(pak->linktype));
        pak_dlci = tmp_dlci;
    }

    if (!frame_relay_packet_debug(idb,DLCI_TO_NUM(pak_dlci)))
        return;

    if (!bridged) { 
	switch (type) {
	  case FR_IETF_IP:
	  case FR_ISIS:
	  case FR_ESIS:
	  case FR_CLNP:
	    buginf("\n%s(%c): dlci %d(0x%x), NLPID 0x%2x(%s), "
		   "datagramsize %d", idb->namestring, display,
		   DLCI_TO_NUM(pak_dlci), pak_dlci, type,
		   link2name(pak->linktype), pak->datagramsize);
	    break;
	  case FR_SNAP0PAD:
	      	snap = (snap_hdr *)pak->info_start;
		buginf("\n%s(%c): dlci %d(0x%x), pkt encaps 0x%04x 0x%02x%02x "
			"0x%02x 0x%4x(%s), datagramsize %d", idb->namestring,
			display, DLCI_TO_NUM(pak_dlci), pak_dlci, type,
			snap->oui[0], snap->oui[1], snap->oui[2],
			GETSHORT(&snap->type), link2name(pak->linktype),
			pak->datagramsize);
		break;
	  case FR_SNAP1PAD:
	    snap = (snap_hdr *)pak->info_start;
	    buginf("\n%s(%c): dlci %d(0x%x), pkt encaps 0x%04x 0x%02x%02x "
		"0x%02x%02x 0x%4x(%s), datagramsize %d", idb->namestring,
		display, DLCI_TO_NUM(pak_dlci), pak_dlci, type,
		snap->control, snap->oui[0], snap->oui[1], snap->oui[2],
		snap->type, link2name(pak->linktype), pak->datagramsize);
	    break;
	  default:
	    buginf("\n%s(%c): dlci %d(0x%x), pkt type 0x%2x%s, "
		   "datagramsize %d", idb->namestring, display,
		   DLCI_TO_NUM(pak_dlci), pak_dlci, type,
		   buffer, pak->datagramsize);
	    break;
	}
    } else {
	/*
	 * Header Format:  (see if/bridge_mci.c - bridge_receive)
	 *    ushort DLCI
	 *    ushort Type (cisco)
	 *    ushort sending_DLCI  [only when send on a multicast]
	 *    uchar  ether_daddr[IEEEBYTES]
	 *    uchar  ether_saddr[IEEEBYTES]
	 *
	 *    (length > 1500)   or    (length <= 1500)
	 *    ushort type             ushort length
	 *                            uchar  snapstuff[6]
	 *                            ushort type
	 */
	if (tmp_dlci && (tmp_dlci != fr_idb->fr_multicast_dlci)) {
	    ether = (ether_hdr *)pak->mac_start;
	} else {
	    ether = (ether_hdr *)&hdr->data[2];
	}
	buginf("\n%s(%c): dlci %d(0x%x), pkt type 0x%x, datagramsize %d",
	       idb->namestring, display, DLCI_TO_NUM(tmp_dlci),
	       tmp_dlci, type, pak->datagramsize);
	buginf("\n\tbridged     dest %e, src %e", ether->daddr, ether->saddr);
	if (ether->type_or_len > MAXETHERSIZE) {
	    /* Ethernet */
	    buginf(", type %2x", ether->type_or_len);
	} else {
	    snap = (snap_hdr *)ether->data;
	    /* 802.x, SNAP */
	    buginf(", type %2x", snap->type);
	}
    }
}



/*
 * fr_pvc_stay_down
 *	If pvc is associated with a DCE interface, and the interface is
 *      not up, return TRUE, otherwise FALSE.
 */	
static boolean fr_pvc_stay_down (fr_pvc_info_type *fr_pvc, ushort set_status)
{
    fr_idb_struct_type *fr_idb;

    fr_idb = fr_pvc->idb->hwptr->frame_relay_stuff;
    if ((fr_pvc->pvc_usage == PVC_LOCAL) ||
	(fr_pvc->pvc_usage == PVC_NOTSET )) {
	if (!DLCI_ACTIVE(fr_pvc->pvc_status) &&
	    DLCI_ACTIVE(set_status)) {
	    if (is_subinterface(fr_pvc->idb) &&
		fr_pvc->idb->subif_state == SUBIF_STATE_ADMINDOWN) {
		return TRUE;
	    } else if (!hw_interface_up(fr_pvc->idb->hwptr)) {
		return TRUE;
	    }
	}
    }
    return FALSE;
}


void fr_set_pvc_status (fr_pvc_info_type *fr_pvc, ushort set_status)
{
    fr_idb_struct_type *fr_idb;
    hwidbtype *netidb;
    ushort old_status;

    if ((fr_idb = fr_pvc->idb->hwptr->frame_relay_stuff) == NULL)
	return;

    if (fr_pvc_stay_down(fr_pvc, set_status)) {
	return;
    }

    netidb = get_netidb(fr_pvc->idb->hwptr);
    if (fr_pvc->pvc_status != set_status) {
	/*
	 * PVC status changed. Update the cache and record the change time
	 */
	old_status = fr_pvc->pvc_status;
	fr_pvc->pvc_status = set_status;
	GET_TIMESTAMP(fr_pvc->pvc_change_time);
	lmi_idb->fr_dlci_status_change = TRUE;
	/*
	 * We don't want to consider the LSBs when cisco LMI
	 */
	if ((old_status & DLCI_CHG_MASK) !=
	    (set_status & DLCI_CHG_MASK)) {
	    reg_invoke_frDLCIStatusChange(fr_pvc);
	    if (fr_pvc->pvc_type != SVC_DYNAMIC)
		errmsg(&msgsym(DLCICHANGE, FR),
		       netidb->hw_namestring, fr_pvc->this_dlci,
		       (IS_PVC_ACTIVE_BIT_SET(set_status) ? "ACTIVE" :
			IS_DLCI_DELETED(set_status) ? "DELETED" : "INACTIVE"));
	    if (IS_PVC_ACTIVE_BIT_SET(fr_pvc->pvc_status)) {
		reg_invoke_media_fr_pvc_active(fr_pvc->idb, fr_pvc->this_dlci);
	    } else {
		/*
		 * Only sent update of PVC down if it was UP.
		 * INACTIVE to DELETED or the other way around, we don't send
		 * update.
		 */
		if (IS_PVC_ACTIVE_BIT_SET(old_status)) {
		    reg_invoke_media_fr_pvc_inactive(fr_pvc->idb,
						     fr_pvc->this_dlci);
		}
	    }
	}
    }
}

/*
 * fr_mc_add
 * add a multicast DLCI
*/
void fr_add_mc_dlci (
		     hwidbtype *idb,
		     enum FR_MC_TYPE status,
		     ushort dlci)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc = NULL;
    ushort old_dlci;

    fr_idb = idb->frame_relay_stuff;
    if (!fr_idb)
	return;
    /*
     * Later configured multicast dlci will overwrite the existing one.
     */
    old_dlci = DLCI_TO_NUM(fr_idb->fr_multicast_dlci);
    if (old_dlci != FRAME_RELAY_DEFAULT_MULTICAST_DLCI)
	fr_mc_delete_dlci(idb, old_dlci);

    fr_idb->fr_multicast_dlci = NUM_TO_DLCI(dlci);
    fr_idb->fr_multicast_status = status;
    fr_pvc = fr_idb->fr_dlci_tab[dlci];
    if (!fr_pvc) {
	fr_pvc = fr_pvc_init(idb->firstsw,dlci,
			     PVC_LOCAL,status == FR_MULTICAST_STATIC ?
			     PVC_STATIC : PVC_DYNAMIC, FALSE, NULL);
    }
    if (fr_pvc) {
	fr_idb->map_status[dlci] = FR_ALL_MAPPED;
	fr_pvc->use_count++;
	if ( status == FR_MULTICAST_STATIC)
	    fr_set_pvc_status(fr_pvc, DLCI_ADDED);
    }
}

/*
 * fr_mc_delete_dlci
 * Delete  the multicast DLCI
*/
void fr_mc_delete_dlci (hwidbtype *idb, ushort dlci)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc = NULL;

    fr_idb = idb->frame_relay_stuff;
    if (!fr_idb)
	return;
    fr_pvc = fr_idb->fr_dlci_tab[dlci];
    if (!fr_pvc)
	return;
    fr_idb->fr_multicast_status = FR_MULTICAST_NOTSET;
    if (--fr_pvc->use_count == 0)
	fr_pvc_clear(idb, DLCI_TO_NUM(fr_idb->fr_multicast_dlci));
    fr_idb->fr_multicast_dlci =
	NUM_TO_DLCI(FRAME_RELAY_DEFAULT_MULTICAST_DLCI);
}

/*
 * fr_listdebug
 * Print a specific frame relay de list for debugging
 */
static void fr_listdebug (fr_listtype *list)
{
    if (!list) {
	buginf("\nNo list match");
    } else {
	if (list->interface)
	    buginf("\nInterface %s", list->interface->hw_namestring);
	if (list->linkcode) {
	    buginf("\nProtocol %s",print_linktype(list->linkcode));
	    if (list->gt_size)
		buginf("gt %d",list->gt_size);
	    if (list->lt_size)
		buginf(" lt %d",list->lt_size);
	    if (list->testlist)
		buginf(" list %d",list->testlist);
	    if (list->tcp_port)
		buginf(" tcp %d",list->tcp_port);
	    if (list->udp_port)
		buginf(" udp %d",list->udp_port);
	    if (list->ip_fragments)
		buginf(" fragments");
	}
    }
}

/*
 * fr_pkt_classify
 * Classify packets for setting of the DE bit
 */
static fr_listtype *fr_pkt_classify (
			      idbtype *idb,
			      paktype *pak,
			      ushort dlci)
{
    ushort sport, dport;
    fr_listtype *frlist;
    iphdrtype *ippak;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;

    fr_idb = idb->hwptr->frame_relay_stuff;
    if (!fr_idb)
	return(NULL);
    fr_pvc = fr_idb->fr_dlci_tab[dlci];
    if (!fr_pvc)
	return(NULL);
    if (fr_listarray[fr_pvc->fr_list] == NULL) {
	return(NULL);
    }

    for (frlist = fr_listarray[fr_pvc->fr_list]; frlist; frlist = frlist->next) {

	if (frlist->interface) {
	    if (frlist->interface != pak->if_input->hwptr)
		continue;
	    if (fr_de_debug) {
		fr_listdebug(frlist);
	    }
	    return(frlist);
	}

	if (frlist->linkcode != pak->linktype) {
	    continue;
	}

	if (frlist->gt_size) {
	    if (pak->datagramsize <= frlist->gt_size) {
		continue;
	    }
	    if (fr_de_debug) {
		fr_listdebug(frlist);
	    }
	    return(frlist);
	}
	if (frlist->lt_size) {
	    if (pak->datagramsize >= frlist->lt_size) {
		continue;
	    }
	    if (fr_de_debug) {
		fr_listdebug(frlist);
	    }
	    return(frlist);
	}
	if (frlist->tcp_port) {
	    ippak = (iphdrtype *)ipheadstart(pak);
	    if (ippak->prot != TCP_PROT)
		continue;
	    /*
	     * Fragmented IP packets do not have higher level header available
	     */
	    if (ippak->fo)
		continue;
	    sport = TcpPkt(pak)->sourceport;
	    dport = TcpPkt(pak)->destinationport;
	    if ((sport != frlist->tcp_port) && (dport != frlist->tcp_port)) {
		continue;
	    }
	    if (fr_de_debug) {
		fr_listdebug(frlist);
	    }
	    return(frlist);
	}
	if (frlist->udp_port) {
	    ippak = (iphdrtype *)ipheadstart(pak);
	    if (ippak->prot != UDP_PROT)
		continue;
	    /*
	     * Fragmented IP packets do not have higher level header available
	     */
	    if (ippak->fo)
		continue;
	    sport = ((udptype *)ipdatastart(pak))->sport;
	    dport = ((udptype *)ipdatastart(pak))->dport;
	    if ((sport != frlist->udp_port) && (dport != frlist->udp_port)) {
		continue;
	    }
	    if (fr_de_debug) {
		fr_listdebug(frlist);
	    }
	    return(frlist);
	}
	if (frlist->ip_fragments) {
	    ippak = (iphdrtype *)ipheadstart(pak);
	    if (!ippak->fo)
		continue;
	    if (fr_de_debug) {
		fr_listdebug(frlist);
	    }
	    return(frlist);
	}
	if (frlist->testlist) {
	    if (!reg_invoke_access_check(frlist->linkcode,
					 pak, (int) frlist->testlist)) {
		continue;
	    }
	    if (fr_de_debug) {
		fr_listdebug(frlist);
	    }
	    return(frlist);
	}
	if (fr_de_debug) {
	    fr_listdebug(frlist);
	}
	return(frlist);
    }
    if (fr_de_debug) {
	fr_listdebug(frlist);
    }
    return(NULL);
}

/*
 * fr_vencapsulate
 * frame relay encapsulation assuming datagramstart & datagramsize initialized
 */
boolean fr_vencapsulate (
			 paktype *pak, 
			 long dhost)
{
    if (pak->datagramstart == NULL)
	return(FALSE);
    return(fr_encaps(pak, dhost, NULL));
}

/*
 * frame_relay_encaps
 * Determines protocol addr -> DLCI mapping, handles replicated broadcasts,
 * & queues packet for output
 */

boolean fr_encaps (paktype *pak,
		   long address,
		   frmaptype *recursive)
{
    frmaptype *map = NULL;
    register idbtype *idb;
    fr_idb_struct_type *fr_idb;
    idbtype *netidb;
    addrtype proto_addr;
    boolean broadcast = FALSE, output = FALSE;
    ushort dlci = 0, result = FALSE;
    enum FR_ENCAPS_FORM encaps_type;
    paktype *newpak = pak, *orig_pak = pak;
    struct llc2_fr_dte_str *sptr;
    uchar l3_pid = NULL_L3_PID;
    staticmap_type 	*smap;
    
    int riflen;
    uchar *daddr = NULL;
    tring_hdr *trh;
    llctype* llc;

    idb = pak->if_output;
    fr_idb =  idb->hwptr->frame_relay_stuff;
    if (!interface_up(idb)) {
	if (frame_relay_packet_debug(idb,IGNORE_DLCI))
	    buginf("\n%s: encaps failed--line down", idb->namestring);
	return(FALSE);
    }

    /*
     * For ISDN and dialers, idb points to the physical interface (B-ch, Ser),
     * but we need the network protocol information that resides on the rotary
     * group leader (D-ch, Dialer), so get it here.
     */
    netidb = get_pak_netidb(pak);

    if (pak->linktype == LINK_FR_ARP) {
	dlci = *((ushort *)address);
	return(fr_doencaps(idb, pak, dlci, pak->linktype,
			   fr_idb->fr_ietf_encaps, NULL_L3_PID, map));
    }

    if (!recursive) {
	/*
	 * Do address resolution.  We want an encapsulation method, a
	 * destination address, and an optional pointer to any other
	 * addressing information (presently any SNAP information).  We are
	 * careful to reset pak->enctype with any new information.  The
	 * destination address can be either a dstination MAC address (in
	 * daddr) or a address structure (in proto_addr) which SMDS will use
	 * to look up a static address map.
	 */
	memset(&proto_addr, 0, sizeof(addrtype));
	if (!reg_invoke_media_frame_macaddr(pak->linktype, pak, address,
					    &proto_addr, &broadcast)) {
	    if (!reg_used_media_frame_macaddr(pak->linktype))
		errmsg(&msgsym(BADMACREG, LINK), idb->namestring,
		       pak->linktype);
	    return(FALSE);
	}
	
	if (broadcast) {
	    pak->fr_flags |= PAK_FR_BROADCAST;
	    /*
	     * Broadcast packet. Send a copy to all hosts
	     * who need broadcasts in address block. Including SVCs.
	     */
	    if (fr_idb->fr_multicast_status != FR_MULTICAST_NOTSET) {
		fr_traffic.multicasts_sent++;
		map = NULL;
		dlci = fr_idb->fr_multicast_dlci;
		encaps_type = (fr_idb->fr_ietf_encaps ? FR_ENCAPS_IETF :
			       FR_ENCAPS_CISCO);
		return(fr_doencaps(idb,pak,dlci,pak->linktype, encaps_type,
				   NULL_L3_PID, map));
	    } else {
		/*
		 * Special handling for CDP packets.
		 */
		if (pak->linktype == LINK_CDP) {
		    return(fr_broadcast_cdp(netidb, pak));
		}

		map = fr_broadcast(netidb, pak, pak->linktype);
	    	/*       
	     	 * For SVCs, broadcast on all maps configured for broadcast.
	     	 * If no such map exists, try to setup an SVC. Don't look to 
		 * see whether map is NULL, because there may be PVCs that 
		 * exist on this interface. 
	     	 */
	    	if ((fr_idb->fr_svc_enabled) && (fr_idb->lapfup)) {
		    fr_svc_broadcast_handler(idb, pak);
	    	}
		if (map == NULL) {
		    if (frame_relay_packet_debug(idb,IGNORE_DLCI))
			buginf("\n%s:encaps failed on broadcast for link %d(%s)",
			       idb->namestring, pak->linktype,
			       link2name(pak->linktype));
		    return(FALSE);
		}
		fr_idb->num_broad_total++;
		dlci = map->dlci;
	    }
	} else {
	    pak->fr_flags &= ~PAK_FR_BROADCAST;
  	    map = frame_relay_addr_lookup(netidb, &proto_addr, pak->linktype);
	     /*
	      * fr_idb->lapfup will indicate whether layer 2 is up or not.
	      */
	     if ((fr_idb->fr_svc_enabled) && (map == NULL) &&
		 (fr_idb->lapfup)) {
		/* 
		 * we know that the broadcast flag is FALSE. 
		 * because we have taken the "else" path above.
		 */
		if (is_p2p(idb))
		    smap = staticmap_lookup(idb, NULL, pak->linktype,
				ET_FRAME_RELAY, FALSE);
		else
	     	    smap = staticmap_lookup(idb, &proto_addr, pak->linktype,
	     			ET_FRAME_RELAY, FALSE);

	     	if (smap && (!smap->class))
	     	    return(FALSE);

	     	if (smap) {
	     	    /*
	      	     * encapsulate these packets before trying to setup
	     	     * an SVC. Makes the output of these packets a lot 
		     * easier after the SVC gets set up.
	     	     */
	     	    encaps_type = (smap->encap_flavor == ET_FR_IETF) ?
	     				FR_ENCAPS_IETF : FR_ENCAPS_CISCO;

	     	    fr_doencaps(idb,pak,0, pak->linktype, encaps_type, 
							FR_SVC_PID, NULL);

	     	    reg_invoke_fr_svc_l3_action(fr_idb, pak, smap, 0, 
							L3_CALL_REQ);
		    /* 
		     * This is a false error. The packet is actually queued
		     * up in a holdq. Once the SVC gets setup, the queue
		     * is emptied. 
		     */
	     	    return(FALSE);
	     	}
	    }
	    if (map == NULL) {
		if ((fr_idb->fr_multicast_status != FR_MULTICAST_NOTSET) &&
		    (fr_idb->fr_local_dlci != FR_UNDEFINED_DLCI)) {
		    reg_invoke_fr_arp_send(idb, &proto_addr,
					   pak->linktype, pak);
		    if (frame_relay_packet_debug(idb,IGNORE_DLCI))
			buginf("\n%s:encaps failed-try dynamic "
			       "addr lookup for link %d(%s)",
			       idb->namestring, pak->linktype,
			       link2name(pak->linktype));
		}
		if (frame_relay_packet_debug(idb, IGNORE_DLCI))
		    buginf("\n%s:Encaps failed--no map entry link %d(%s)",
			   idb->namestring, pak->linktype,
			   link2name(pak->linktype));
		return(FALSE);
	    }
	    dlci = map->dlci;
	}
    } else {
	map = recursive;
	dlci = map->dlci;
    }
    
    if ((fr_idb->fr_ietf_encaps && map &&
	 (map->link != LINK_COMPRESSED_TCP) &&
	 map->encaps_type == FR_ENCAPS_NOTSET ) ||
	(map && (map->encaps_type == FR_ENCAPS_IETF)))
	encaps_type = FR_ENCAPS_IETF;
    else {
	encaps_type = FR_ENCAPS_CISCO;
	newpak = fr_tcpip_hdr_compress(idb, pak, map);
    }
    /*
     * If pak->refcount is greater than 1, the pointer "newpak"
     * returned by the compression routine will not be the same as 
     * the original pointer "pak" because such packets 
     * are duplicated by the thc_compress_tcp routine.
     * In such a case, there is no mechanism to pass a pointer
     * to the duplicated packet back to the encapsulation routine. 
     * Hence Frame Relay must deal with said packet!
     */
    if (newpak != pak) {
	output = TRUE;
	pak = newpak;
    }

    /*
     * Deal with LLC2 packets.
     */
    if (pak->linktype == LINK_LLC2) {
        if (pak->llc2_enctype != ET_BRIDGE) {
            sptr = (struct llc2_fr_dte_str *) address;
            dlci = NUM_TO_DLCI(sptr->dlci);
            if (sptr->dlci != map->dlci_num) {
                if (fr_llc2_debug)
                    buginf("\nIBM: DLCI %d does not exist, found DLCI %d",
                           sptr->dlci, map->dlci_num);
                return(FALSE);
	      }

            if (fr_llc2_debug)
                buginf("\nIBM: DLCI is %d 0x%x", sptr->dlci, dlci);

            /* Determine the Level 3 PID*/
            l3_pid = sptr->l3pid;
            if (fr_llc2_debug)
                buginf("\nIBM: Using L3PID : 0x%x, DLCI:%d", l3_pid, sptr->dlci);
	}
        else {    /* BAN encapsulation */

            llc = (llctype *)address; /* for ban, address points to llctype structure */
            daddr = (uchar *)llc->llc2_dte;
            dlci  = NUM_TO_DLCI(llc->fDlci);

            riflen = RIF_LENGTH(*(daddr + RIF_DTE));
            if (riflen & 1) { /* illegal rif length */
               riflen = 0;
	     }

            pak->datagramstart -= (TRING_ENCAPBYTES + riflen ); /* 14 + riflen */
            pak->datagramsize += (TRING_ENCAPBYTES + riflen );

            trh = (tring_hdr *)pak->datagramstart;
            trh->ac = 0;              /* 0x00 pad */
            trh->fc = FC_TYPE_LLC;

            ieee_copy(daddr, trh->daddr);
            ieee_copy( (uchar *) (&(daddr[LMAC_DTE])), trh->saddr);
            if (riflen > 0) {
                trh->saddr[0] |= TR_RII;    /* indicate there is a RIF */
                memcpy(trh->data, (daddr+RIF_DTE), riflen);
	      }

            pak->mac_start  = (uchar *)trh;
            pak->addr_start = trh->daddr;
            pak->rif_start  = (riflen ? trh->data : NULL);
            pak->encsize    = TRING_SNAP_ENCAPBYTES + riflen;
	}
    }

    result = fr_doencaps(idb,pak,dlci,pak->linktype, encaps_type, l3_pid, map);

    /*
     * If compression is enabled and the packet has a linktype of LINK_IP,
     * the returned packet may have a linktype of IP, COMPRESSED_TCP or
     * UNCOMPRESSED_TCP. We only want to deal with COMPRESSED_TCP and
     * UNCOMPRESSED_TCP packets that have been duplicated within the
     * compression routine. The "ipsupport" routine will handle
     * the others.
     */
    if ((output == TRUE) && fr_pak_is_compressed(pak->linktype)) {
	if (result) {
	    ip_traffic.outputs++; 
	    tcp_traffic.outputs++; 
	    datagram_out(pak);
	    /*
	     * The ipsendnet() routine is going to try to datagram_out() 
	     * also, without knowing that the link layer has already done 
	     * so. Hence, we must keep IP from outputting by returning FALSE, 
	     * but since this is not a real failure, we must control the 
	     * IP_ENCAPSFAILED count. Kludge alert!
	     */
	    ip_traffic.encapsfailed--; 
	}
	/*
	 * datagram_done() was called for the original packet in TCP
	 * header compression code because a new packet was used after 
	 * the compression. If we return FALSE to IP either because 
	 * fr_doencaps() failed or datagram_out() will be called for
	 * the packet, we need to increase the packet reference count
	 * so that datagram_done() in ipsendnet() will not accidentally
	 * "dispose" a packet that's still in TCP retransmission queue.
	 */
	pak = orig_pak;
	pak_lock(pak);
	return(FALSE); 
    }
    return(result);
}

static boolean fr_pbp_comp_decide (paktype *pak, frmaptype *map, idbtype *idb)
{
 boolean retVal = FALSE;

 if (!map)
   return(FALSE);
  pak->fr_flags &= ~DEFERED_LATE_Q_COMPRESSION;
  if (idb->hwptr->comp_db &&
      (idb->hwptr->comp_db->comp_configed != COMP_NOT_ENABLED) &&
      (map->compression_flags & FR_MAP_COMP_PAYLOAD_STAC)) {
    pak->fr_flags |= DEFERED_LATE_Q_COMPRESSION;
    retVal = TRUE;
  }
  return retVal;
}

static paktype *fr_pbp_compress (paktype * pak)
/*
 * this is called after packet-dq,  just prior to being shoveled out to wire
 * if this seems very much like - but much more complicated than - HDLC, it's
 * because we're using the same underlying registries to do the compression,
 * but, yes, ma'am, we are re-encapsulating the packet here...
 */
{  
  frame_relay_hdr *fr_hdr;
  short saveDLCI;
  paktype *newpak =  NULL;
  short oldHeaderSize;
  
  if (pak->refcount > 1) {		/* DLSW, telnet, etc */
    /* make copy of original, call datagram_done on original, use new copy */
    newpak  = pak_duplicate(pak);
    if (!newpak) {
      errmsg(&msgsym(NOBUF, COMP), pak->if_output->hwptr->hw_namestring,
	     "Compression getbuffer failed");
      return pak;			/* have to send it uncompressed */
    }
    datagram_done(pak);
    pak = newpak;
  }

  /*
   * compressor wants to compress from info_start to end of packet.  alas, in
   * fr, info_start is not reliably set.  since this code deals with only kind
   * of encapsulation, we're going to make heinous assumptions about where things
   * are...
   */
  if (pak->info_start == NULL) {  /* who knows...some kind person may have set it */
    pak->info_start = pak->network_start;
  }
  fr_hdr = (frame_relay_hdr *)(pak->info_start - sizeof(frame_relay_hdr));
  saveDLCI = GETSHORT(&fr_hdr->dlci);
  if (pak->fr_flags &  DEFERED_LATE_Q_COMPRESSION) {
      short saveLinkType = link2serial(pak->linktype);
      boolean stat = FALSE; 
      fr_payload_compressed_hdr *fr_comp_hdr;
      oldHeaderSize =  pak->info_start - pak->datagramstart;
      pak->datagramsize -= oldHeaderSize;
      
      stat = reg_invoke_late_q_compress(COMP_STAC, pak, 0);
      
      if ( stat ) {	/* compressed and downsize...fix encaps */

	/*
	 * there may be a pre-amble on this header full of stuff we don't
	 * understand (apologies to Stevie Wonder) like board-encaps,
	 * what have you.  if it's there we need to make room for it
	 * and copy it into the new header
	 */
	short headerPreambleSize = oldHeaderSize - sizeof(frame_relay_hdr);
	short newHeaderSize = sizeof(fr_payload_compressed_hdr) + headerPreambleSize;
	unsigned char *newStart = pak->datagramstart - (newHeaderSize - oldHeaderSize);
	
	fr_comp_hdr = (fr_payload_compressed_hdr *)
	  (pak->info_start - sizeof(fr_payload_compressed_hdr));

	if (headerPreambleSize)				/* the preamble, if any */
	  bcopy(pak->datagramstart,			/* all the bcopys I've looked at... */
	      newStart,					/* ...handle overlap */
	      headerPreambleSize);
	PUTSHORT(&fr_comp_hdr->hdr.dlci, saveDLCI);
	PUTSHORT(&fr_comp_hdr->hdr.type, TYPE_COMPRESSED_FR);
	PUTSHORT(&fr_comp_hdr->trueType, saveLinkType);
	pak->datagramstart = newStart;
	pak->datagramsize += newHeaderSize;
	pak->mac_start  = pak->datagramstart + headerPreambleSize;
	pak->enctype    = FR_ENCAPS_CISCO2;
	pak->encsize    = newHeaderSize;
      } else {		/* packet is uncompressed... */
	pak->datagramsize += oldHeaderSize;
      }
    }
  return pak;
}

/*
 * fr_tcpip_hdr_compress
 *
 * Compress the packet if compression is enabled in some form, 
 * if the compression is active, and if the protocol within the
 * IP packet is of type TCP. 
 */
paktype *fr_tcpip_hdr_compress (
				idbtype *idb,
				paktype *pak,
				frmaptype *map)
{
    iphdrtype *ip;

    ip = (iphdrtype *)pak->network_start; 

    if (map->tcp_compress && (pak->linktype == LINK_IP)) {
	if ((COMPRESS_OVER_IP_LINK(map)) ||
	    (COMPRESS_OVER_COMPR_TCP_LINK(map)) ||
	    (COMPRESS_OVER_P2P_LINK(map))) {
	    if (map->tcp_compress->flags & THC_ACTIVE) {
		if ( ip->prot == TCP_PROT) {
		    pak = reg_invoke_compress_tcpip_header(LINK_IP, 
							   idb->hwptr,
							   pak,
							   FALSE, 
							   map->tcp_compress);
		    return(pak);
		}
	    }
	}
    }
    return(pak);
}

/*
 * fr_doencaps
 * Fill in the packet values
 *
 * ***** IMPORTANT NOTE: ******
 * Before return true from this routine, unless link_type is LLC2,
 * mac_start points to dlci, where addr_start is used to help setting
 * mac_start after debug msg is done; but for non-bridge cases, addr_start
 * stays null throughout this routine.
 */
boolean fr_doencaps (
		     idbtype *idb,
		     paktype *pak,
		     ushort dlci,
		     uint protocol,
		     enum FR_ENCAPS_FORM enc_ietf, 
		     uchar level3pid,
		     frmaptype *map)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    ushort num_dlci,priority_dlci;
    frame_relay_hdr *hdr = NULL;
    snap_hdr *snap;
    fr_q933_hdr *q933;
    ushort l3_pid, tmp_dlci;
    uchar fr_osi_shift;
    ushort fr_nlpid_type;
    hwidbtype *hwidb;

    fr_idb =  idb->hwptr->frame_relay_stuff;
    num_dlci = DLCI_TO_NUM(dlci);
    fr_pvc = fr_idb->fr_dlci_tab[num_dlci];
    /*
     * check if the pvc is active or not
     */
    if (!fr_pvc && (level3pid != FR_SVC_PID))
        return(FALSE);
    /*
     * Determine if we need to change to a lower priority DLCI
    */
    if (fr_pvc && fr_pvc->fr_pr_dlci_group) {
	priority_dlci = fr_pr_set_dlci(pak,fr_pvc,fr_pvc->fr_pr_dlci_group);
	/*
	 * Check to see if we changed the DLCI
	 * and if we did adjust accordingly
	*/ 
	if (num_dlci != priority_dlci) { 
	    fr_pvc = fr_idb->fr_dlci_tab[priority_dlci];
	    if (!fr_pvc)
		return(FALSE);
	    dlci = NUM_TO_DLCI(priority_dlci);
	    num_dlci = priority_dlci;
	}
    }
    if (level3pid == FR_SVC_PID) {
	/* 
	 * This is a special case of SVCs. A call  does not exist yet. 
	 * All we do is encapsulate, leave the DLCI field blank and
	 * return.
	 */
	;
    } else if (!idb->hwptr->nokeepalive && !DLCI_ACTIVE(fr_pvc->pvc_status)) {
	if (frame_relay_packet_debug(idb,num_dlci))
	    buginf("\nDLCI %d is either deleted or inactive", num_dlci);
        return(FALSE);
    }

    if ((dlci == fr_idb->fr_multicast_dlci) &&
	(pak->linktype == LINK_BRIDGE)) {
	pak->datagramstart -= ADDRLEN_FR;
	pak->datagramsize += ADDRLEN_FR;
	PUTSHORT(pak->datagramstart,
		 fr_idb->fr_local_dlci | FRAME_RELAY_EA_BITS);
    }

    if ((pak->linktype == LINK_CLNS) && (enc_ietf != FR_ENCAPS_IETF)) {
	pak->datagramstart--;
	pak->datagramsize++;
    }

    if (pak->linktype == LINK_LLC2) {

        if ( pak->llc2_enctype != ET_BRIDGE) {

            pak->datagramstart -= FRAME_RELAY_Q933_NLPID_SIZE;
            pak->datagramsize += FRAME_RELAY_Q933_NLPID_SIZE;

            /* Init hdr just to get rid of compile error */
            hdr = (frame_relay_hdr *)pak->datagramstart;
            q933 = (fr_q933_hdr *)(pak->datagramstart);
            PUTSHORT(&q933->dlci, (dlci | FRAME_RELAY_EA_BITS));
            PUTSHORT(&q933->q933_nlpid, FR_Q933_NLPID);
            PUTSHORT(&q933->l2_pid, TYPE_L2_8022_PAD);
            l3_pid = l3pid(level3pid);
            PUTSHORT(&q933->l3_pid, l3_pid);
	  }
        else {   /* BAN encapsulation , TR type encap. already done */

            pak->datagramstart -= FRAME_RELAY_SNAPENCAPBYTES; /* 10 bytes */
            pak->datagramsize += FRAME_RELAY_SNAPENCAPBYTES;
            hdr = (frame_relay_hdr *)pak->datagramstart;
            PUTSHORT(&hdr->dlci, (dlci | FRAME_RELAY_EA_BITS)); 
            PUTSHORT(&hdr->type, FR_SNAP_NLPID);  /* 03 00 */

            snap = (snap_hdr *)hdr->snap_start;
            snap->control = FR_SNAP_CONTROL;       /* 80 */
            ieee_copy_oui(tb1490_snap, snap->oui); /* 00 80 C2 */
            PUTSHORT(&snap->type, FR1490_802_5); /* 00 09 no preserved lan fcs */
            pak->info_start = (uchar *)snap;
            pak->enctype    = ET_FR_IETF;
            pak->encsize    = FRAME_RELAY_SNAPENCAPBYTES;

	  }

    } else if (enc_ietf != FR_ENCAPS_IETF) {
	fr_pbp_comp_decide(pak, map, idb); /* decide if compress downstream */

	pak->mac_start = pak->datagramstart;
	pak->datagramstart -= FRAME_RELAY_ENCAPBYTES;
	pak->datagramsize += FRAME_RELAY_ENCAPBYTES;
	if (pak->linktype == LINK_BRIDGE)
	    pak->addr_start = pak->datagramstart;
	else {
	    pak->mac_start = pak->datagramstart;
	    pak->addr_start = NULL;
	}
	hdr = (frame_relay_hdr *)pak->datagramstart;
	PUTSHORT(&hdr->dlci, (dlci | FRAME_RELAY_EA_BITS));
	PUTSHORT(&hdr->type, link2serial(pak->linktype));

	pak->rif_start  = NULL;
	pak->riflen     = 0;
	pak->info_start = NULL;
	pak->enctype    = ET_FRAME_RELAY;
	pak->encsize    = FRAME_RELAY_ENCAPBYTES;

    } else {
	if (NLPID_DEF(pak->linktype)) {
	    if (pak->linktype == LINK_CLNS) {
		fr_osi_shift = 1;
		pak->datagramstart -= (FRAME_RELAY_ENCAPBYTES-fr_osi_shift);
		pak->datagramsize += FRAME_RELAY_ENCAPBYTES-fr_osi_shift;
		hdr = (frame_relay_hdr *)pak->datagramstart;
		fr_nlpid_type = 0x0300 | hdr->osi_type[1];
	    }
	    else {
		fr_osi_shift = 0;
		pak->datagramstart -= (FRAME_RELAY_ENCAPBYTES-fr_osi_shift);
		pak->datagramsize += FRAME_RELAY_ENCAPBYTES-fr_osi_shift;
		hdr = (frame_relay_hdr *)pak->datagramstart;
		fr_nlpid_type = link2nlpid(pak->linktype);
	    }
	    PUTSHORT(&hdr->dlci, (dlci | FRAME_RELAY_EA_BITS));
	    PUTSHORT(&hdr->type,fr_nlpid_type);

	    pak->mac_start  = pak->datagramstart;
	    pak->addr_start = NULL;
	    pak->rif_start  = NULL;
	    pak->riflen     = 0;
	    pak->info_start = NULL;
	    pak->enctype    = ET_FR_IETF;
	    pak->encsize    = FRAME_RELAY_ENCAPBYTES;
	} else if ((pak->linktype == LINK_BRIDGE) ||
		   (pak->linktype == LINK_IEEE_SPANNING)) {
	    pak->datagramstart -= FRAME_RELAY_SNAPENCAPBYTES;
	    pak->datagramsize += FRAME_RELAY_SNAPENCAPBYTES;
	    hdr = (frame_relay_hdr *)pak->datagramstart;
	    PUTSHORT(&hdr->dlci, (dlci | FRAME_RELAY_EA_BITS));
	    PUTSHORT(&hdr->type, FR_SNAP_NLPID);
	    
	    snap = (snap_hdr *)hdr->snap_start;
	    snap->control = FR_SNAP_CONTROL;
	    ieee_copy_oui(tb1490_snap, snap->oui);
	    PUTSHORT(&snap->type, tb1490link2serial(pak));
	    pak->mac_start  = snap->data;
	    pak->addr_start = pak->datagramstart;
	    pak->rif_start  = NULL;
	    pak->riflen     = 0;
	    pak->info_start = (uchar *)snap;
	    pak->enctype    = ET_FR_IETF;
	    pak->encsize    = FRAME_RELAY_SNAPENCAPBYTES;
	} else {
#undef TRANSMIT_FRAME_SNAP0PAD
#ifdef TRANSMIT_FRAME_SNAP0PAD
	    pak->datagramstart -= FRAME_RELAY_SNAPENCAPBYTES - 1;
	    pak->datagramsize += FRAME_RELAY_SNAPENCAPBYTES - 1;
	    hdr = (frame_relay_hdr *)pak->datagramstart;
	    PUTSHORT(&hdr->dlci, (dlci | FRAME_RELAY_EA_BITS));
	    PUTSHORT(&hdr->type, FR_SNAP_NLPID | FR_SNAP_CONTROL);
	    snap = (snap_hdr *)((uchar *)hdr->snap_start - 1);
	    snap->control = FR_SNAP_CONTROL;
	    ieee_copy_oui(zerosnap, snap->oui);
	    PUTSHORT(&snap->type, link2serial(pak->linktype));
#else
	    pak->datagramstart -= FRAME_RELAY_SNAPENCAPBYTES;
	    pak->datagramsize += FRAME_RELAY_SNAPENCAPBYTES;

	    hdr = (frame_relay_hdr *)pak->datagramstart;
	    PUTSHORT(&hdr->dlci, (dlci | FRAME_RELAY_EA_BITS));
	    PUTSHORT(&hdr->type, FR_SNAP_NLPID);

	    snap = (snap_hdr *)hdr->snap_start;
	    snap->control = FR_SNAP_CONTROL;
	    ieee_copy_oui(zerosnap, snap->oui);
	    PUTSHORT(&snap->type, link2serial(pak->linktype));
#endif

	    pak->mac_start  = pak->datagramstart;
	    pak->addr_start = NULL;
	    pak->rif_start  = NULL;
	    pak->riflen	    = 0;
	    pak->info_start = (uchar *)snap;
	    pak->enctype    = ET_FR_IETF;
#ifdef TRANSMIT_FRAME_SNAP0PAD
	    pak->encsize    = FRAME_RELAY_SNAPENCAPBYTES - 1;
#else
	    pak->encsize    = FRAME_RELAY_SNAPENCAPBYTES;
#endif
	}
    }

    /* 
     * Special processing for SVCs. A SVC does not exist yet. 
     * encapsulation is done. The holdQ will now hold this 
     * packet. Return TRUE. Next the l3if_action routine will
     * try to set up the SVC. 
     */
    if (level3pid == FR_SVC_PID)
    	return(TRUE);

    /*
     * Note the the following way of doing board_encap is to allow 
     * FR-over-ISDN case to get to the connected channel in a generic
     * and more self-defensive way of handling.
     */
    if (pak->if_output) {
	if ((hwidb = pak->if_output->hwptr) && hwidb->board_encap &&
	    !(hwidb->board_encap)(pak, hwidb))
	    return(FALSE);
    } else {
	if ((hwidb = idb->hwptr) && hwidb->board_encap &&
	    !(hwidb->board_encap)(pak, hwidb))
	    return(FALSE);
    }

    if (!fr_pak_is_compressed(pak->linktype) && !fr_pvc->fr_list &&
	!fr_pvc->fr_pr_dlci_group && !(pak->flags & PAK_ENCAPS_ONLY))
    	reg_invoke_fs_cache_update(pak->linktype, pak, 0L);
    if (fr_pvc->fr_list) {
	if (fr_pkt_classify(idb,pak,num_dlci)) {
	    tmp_dlci = GETSHORT(&hdr->dlci);
	    PUTSHORT(&hdr->dlci, (tmp_dlci | FR_DE_BIT));
	}
    }
    	    
    if (frpkt_debug)
	debug_fr_pak(idb, pak, dlci, pak->linktype == LINK_BRIDGE, OUTPUT);
	/* for bridge mac_start points to dest. mac addr for debug msg */

    if (pak->linktype != LINK_LLC2 && (pak->addr_start))
	pak->mac_start = pak->addr_start;	/* so fr_oqueue works */

    /*
     * now update statistics, but not for the dummy encaps only packets.
     */

    if ((fr_pvc) && !(pak->flags & PAK_ENCAPS_ONLY)) {
	ushort hdr_dlci = GETSHORT(&hdr->dlci);
	fr_pvc->pvc_stats.num_output_pkts++;
	fr_pvc->pvc_stats.num_bytes_send += pak->datagramsize;
	if (ISFR_FECN(hdr_dlci))
	    fr_pvc->pvc_stats.num_sent_fecn_pkts++;
	if (ISFR_BECN(hdr_dlci))
	    fr_pvc->pvc_stats.num_sent_becn_pkts++;
	if (ISFR_DE(hdr_dlci))
	    fr_pvc->pvc_stats.num_sent_de_pkts++;
    }
    /* 
     * Reset the idle timer for every outgoing packet on an SVC.
     * Don't reset it if someone is trying to tear the svc down.
     */
    if (fr_pvc->pvc_type == SVC_DYNAMIC) {
	if (fr_pvc->svc_idle_timer) {
    	    TIMER_STOP(fr_pvc->t_last_activity);
    	    TIMER_START(fr_pvc->t_last_activity, fr_pvc->svc_idle_timer);
	    if (fr_svc_detd_debug)
	    	buginf("\nFR: fr_encaps: SVC %d Starting svc_idle_timer %d", 
					fr_pvc->this_dlci, fr_pvc->svc_idle_timer);
	    GET_TIMESTAMP(fr_pvc->svc_reset_time);
	}
    }
    return(TRUE);
}


/*
 * is_fr_osi_nlpid
 * Return true if a valid OSI NLPID
 */
static boolean is_fr_osi_nlpid (
				    ushort type)
{
    switch (type) {
    case FR_CLNP:
    case FR_ESIS:
    case FR_ISIS:
    case FR_ISO_IGRP:
	return(TRUE);
    default:
	return(FALSE);
    }
}


/*
 * frame_relay_parse_packet
 *
 * Given a packet buffer and the first four bytes of a serial interface
 * datagram, determine the value of pak->datagramstart so the datagram's
 * network level protocols are aligned on a long word boundary.
 * Returns pointer to function to enqueue the datagram, NULL if datagram
 * should be discarded.
 *
 * This now will sniff past the first longword.  This is okay because
 * the low end should pass in the whole pak, and the high end will pass
 * in the sniff buffer, which will have the correct amount of stuff
 * loaded in it from fr_fast_pvc_switch_or_fs
 */
static iqueue_t frame_relay_parse_packet (
					  hwidbtype *idb,
					  paktype *pak,
					  uchar *data)
{
    fr_idb_struct_type *fr_idb;
    frame_relay_hdr *frame;
    snap_hdr *snap;
    fr_q933_hdr *q933;
    ether_hdr *ether;
    charlong firstlong;
    trrif_t *trh;
    srbrif_t *shdr;
    uchar fr_osi_shift;
    uchar *sniffdata;

    /*
     * Check to see if building a frame in a virgin packet, or whether
     * we're just setting up media pointers into the frame
     */
    if (data) {
	/*
	 * We've got a virgin packet. 
	 */
	firstlong.d.lword = GETLONG(data);
    } else {
	/*
	 * Soiled packet. Start from datagramstart
	 */
	firstlong.d.lword = GETLONG(pak->datagramstart);
    }

    /*
     * Grab the encapsulation type, and figure out the encapsulation size.
     */
    fr_idb = idb->frame_relay_stuff;
    pak->fr_flags = 0;
    if (firstlong.d.sword[1] == TYPE_BRIDGE) { /* Bridge packet? */
	pak->enctype = ET_BRIDGE; /* Mark as bridge encapsulation */
	pak->flags |= PAK_TBRIDGE_IN;

	if (data) {
	    frame = (frame_relay_hdr *)(pak_center(pak) -
					FRAME_RELAY_ENCAPBYTES -
					ETHER_ARPA_ENCAPBYTES);
	    pak->datagramstart = (uchar *)frame;
	    sniffdata = data;
	} else {
	    frame = (frame_relay_hdr *)pak->datagramstart;
	    sniffdata = pak->datagramstart;
	}	    
	ether              = (ether_hdr *)frame->data;
	pak->mac_start     = (uchar *)ether;
	pak->addr_start    = ether->daddr;
	pak->rif_start     = NULL;
	pak->riflen	   = 0;
	pak->info_start    = NULL;
	pak->network_start = ether->data;
	
	/*
	 * if this packet input i/f has been changed by
	 * IRB code. We have to adjust the network_start so
	 * it can point to the right location such that 
	 * it will not be rejected by upper layer processes.
	 * The packet is to be routed even it got into the
	 * system through the bridged i/f
	 */
	if (is_bvi(pak->if_input->hwptr)) {
	    /*
	     * We just need to deal with non-ARPA Novell type since
	     * for ARPA type packet the network_start already
	     * points to the right place
	     */
	    if (GETSHORT(&sniffdata[ARPATYPE + FRAME_RELAY_ENCAPBYTES])
				<= MAXETHERSIZE) {
		switch(GETSHORT(&sniffdata[ETHER_8023_ENCAPBYTES + 
					   FRAME_RELAY_ENCAPBYTES])) {
		case SNAPSNAP:
			pak->network_start = (uchar *)ether + 
					     ETHER_SNAP_ENCAPBYTES;
			break;
		case SAPSAP_NOVELL:
			pak->network_start = (uchar *)ether + 
					     ETHER_SAP_ENCAPBYTES;
			break;
		default:
			break;
		}
	    }
	}
	/*
	 * Record just how big this encapsulation was rather than every
	 * protocol have it's own fun and wacky way to work it out.
	 */
	pak->encsize = pak->network_start - pak->datagramstart;
	return(frame_relay_input);
    }

    pak->enctype = ET_FRAME_RELAY;
    pak->flags &= ~PAK_TBRIDGE_IN;
    if ((firstlong.d.sword[1] == FR_SNAP0PAD) ||
	(firstlong.d.sword[1] == FR_SNAP1PAD)) {
	int pad_adjust;
	frame_relay_hdr *fr_read;
	snap_hdr *snap_read;

        pad_adjust = (firstlong.d.sword[1] == FR_SNAP0PAD) ? 1 : 0;

	if (data) {
	    frame = (frame_relay_hdr *)(pak_center(pak) -
					FRAME_RELAY_SNAPENCAPBYTES +
					pad_adjust);
	    pak->datagramstart = (uchar *)frame;
	    fr_read = (frame_relay_hdr *)data;
	} else {
	    fr_read = frame = (frame_relay_hdr *)pak->datagramstart;
	}
	snap_read = (snap_hdr *)((uchar *)fr_read->snap_start - pad_adjust);

        if ((GETSHORT((ushort *)&snap_read->oui[1]) == FR_OUI23_BRIDGE) &&
            ((GETSHORT(&snap_read->type) == FR1490_802_5) ||
             (GETSHORT(&snap_read->type) == FR1490_802_5_FCS))) {


            pak->linktype = LINK_LLC2;
            pak->llc2_enctype = ET_BRIDGE;

            pak->fr_flags |= PAK_FR_IETF;
            snap = (snap_hdr *)((uchar *)frame->snap_start - pad_adjust);

            trh                 = (trrif_t *)snap->data;
            shdr                = (srbrif_t *)trh->daddr;
            pak->rif_start = snap->data + TRING_ENCAPBYTES;
            pak->mac_start      = (uchar *)trh;
            pak->addr_start     = trh->daddr;
            return(frame_relay_input);
        }


	/*
	 * See if we have a bridge OIU, and we are don't have a BPDU
	 * If so, set up for 1490 bridging; otherwise set up for a
	 * normal SNAP packet
	 */
	if ((GETSHORT((ushort *)&snap_read->oui[1]) == FR_OUI23_BRIDGE) &&
	    (GETSHORT(&snap_read->type) != FR1490_SPANNING)) {
	    pak->enctype = ET_BRIDGE;
	    pak->flags |= PAK_TBRIDGE_IN;
	    pak->fr_flags |= PAK_FR_IETF;
	    snap = (snap_hdr *)((uchar *)frame->snap_start - pad_adjust);
	    ether		= (ether_hdr *)snap->data;
	    pak->mac_start	= (uchar *)ether;
	    pak->addr_start	= ether->daddr;
	    pak->rif_start	= NULL;
	    pak->riflen	        = 0;
	    pak->info_start	= (uchar *)snap;
	    pak->network_start	= ether->data;
	} else {
	    /* Snap header starts 1 byte earlier for SNAP0PAD */
	    snap = (snap_hdr *)((uchar *)frame->snap_start - pad_adjust);
	    pak->mac_start     = NULL;
	    pak->addr_start    = NULL;
	    pak->rif_start     = NULL;
	    pak->riflen	       = 0;
	    pak->info_start    = (uchar *)snap;
	    pak->network_start = snap->data;
	}
	/*
	 * Record just how big this encapsulation was rather than every
	 * protocol have it's own fun and wacky way to work it out.
	 */
	pak->encsize = pak->network_start - pak->datagramstart;
	return(frame_relay_input);
    }

    if (firstlong.d.sword[1] == FR_Q933_NLPID) {
	if (data) {
	    q933 = (fr_q933_hdr *)(pak_center(pak) -
				   FRAME_RELAY_Q933_NLPID_SIZE);
	    pak->datagramstart = (uchar *)q933;
	} else {
	    q933 = (fr_q933_hdr *)pak->datagramstart;
	}
	q933 = (fr_q933_hdr *)pak->datagramstart;
	pak->mac_start     = NULL;
	pak->addr_start    = NULL;
	pak->rif_start     = NULL;
	pak->riflen	   = 0;
	pak->info_start    = (uchar *)q933->data;
	pak->network_start = pak->info_start + SAP_LONGHDRBYTES;
    } else {
	if (is_fr_osi_nlpid(firstlong.d.sword[1])) 
	    fr_osi_shift = 1;
	 else
	    fr_osi_shift = 0;
	if (data) {
	    frame = (frame_relay_hdr *)(pak_center(pak) -
					FRAME_RELAY_ENCAPBYTES-fr_osi_shift);
	    pak->datagramstart = (uchar *)frame;
	} else {
	    frame = (frame_relay_hdr *)pak->datagramstart;
	}
	pak->mac_start     = NULL;
	pak->addr_start    = NULL;
	pak->rif_start     = NULL;
	pak->riflen	   = 0;
	pak->info_start    = NULL;
	pak->network_start = frame->data-fr_osi_shift;
    }

    /*
     * Record just how big this encapsulation was rather than every protocol
     * have it's own fun and wacky way to work it out.
     */
    pak->encsize = pak->network_start - pak->datagramstart;
    return(frame_relay_input);
}


/*
 * fr_raw_enqueue
 * Called from interrupt level to enqueue an input packet
 * If the protocol specific routine doesn't absorb the packet, possibly because
 * the protocol is not enabled on that interface, the packet must be enqueued
 * on netinputQ for later disposal.  Interrupt level disposal of a steady
 * stream of packets might other lock up the system.  Unbridgable packets are
 * marked with PAK_ABORT.
 */

static void fr_raw_enqueue (
		     hwidbtype *idb, 
		     paktype *pak)
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
	pak->flags |= PAK_ABORT;
	netinput_enqueue(pak);
	return;
    }

    /*
     * Snap the frame to alignment if required
     */
    ALIGN_DATAGRAM_CMD(pak_realign(pak, NULL));

    pak->linktype = (*idb->getlink)(pak);
    
    if (pak->linktype == LINK_ILLEGAL) {
       if (frpkt_debug) 
	   debug_fr_pak(pak->if_input, pak, 0, FALSE, INPUT);
       datagram_done(pak);
       return;  
    } 

    if (fr_rsrb_debug && (pak->linktype == LINK_RSRB))
	fr_debug_pak(pak, "Incoming RSRB frame");

    if (fr_dlsw_debug && (pak->linktype == LINK_DLSW))
        fr_debug_pak(pak, "Incoming DLSW frame");

    if (frpkt_debug)
	debug_fr_pak(idb->firstsw, pak, 0, FALSE, INPUT);

    if (pak->linktype == LINK_ARP) {
	reg_invoke_raw_enqueue(LINK_FR_ARP, pak);
	return;
    }
    reg_invoke_raw_enqueue(pak->linktype, pak);
}

/*
 * fr_translate_1490_to_ether
 * the below is a very ugly routine that convert token/fddi
 * bridged packets received over ietf links to ethernet format
 * for the bridging code
 */
static boolean fr_translate_1490_to_ether (paktype *pak)
{
    snap_hdr *snap;
    ether_hdr *ether;

    snap = (snap_hdr *)pak->info_start;
    switch (GETSHORT(&snap->type)) {
    case FR1490_SPANNING:
	break;
    case FR1490_ETHER_FCS:
	pak->datagramsize -= FCS8023_SIZE;
	/* fall into */
    case FR1490_ETHER:
	break;
    case FR1490_802_5_FCS:
    case FR1490_FDDI_FCS:
	pak->datagramsize -= FDDI_FCS;
	/* fall into */
    case FR1490_802_5:
    case FR1490_FDDI: {
	uchar *dgstart, *dgend;
	int ix;
	
	dgstart = pak->datagramstart + FRAME_RELAY_SNAPENCAPBYTES;
	dgend = dgstart + TR_ACFC_SIZE;

	/*
	 * Move the dest and src addresses to where they'd be for ether
	 * Must swap the bits since token is backwards
	 */
	for (ix = 0; ix < (IEEEBYTES * 2); ix++)
	     *dgstart++ = BITSWAP(*dgend++);
	/* change snap type to look like ether */
	PUTSHORT(&snap->type, FR1490_ETHER);
	ether = (ether_hdr *)snap->data;
	PUTSHORT(&ether->type_or_len,
		 pak->datagramsize - FRAME_RELAY_SNAPENCAPBYTES);
    }
	break;
    default:
	if (frpkt_debug && pak->if_input) {
	    buginf("\n%%Illegal rfc 1490 packet type 0x%x, %s",
		   GETSHORT(&snap->type), pak->if_input->namestring);
	}
	return(FALSE);
    }
    return(TRUE);
}

/*
 * frame_relay_input
 * Handle input packets with Frame Relay format.
 */

void frame_relay_input (
			hwidbtype *idb, 
			paktype *pak)
{
    hwidbtype *netidb;
    ushort temp_dlci, dlci;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc = NULL;
    ulong link;
    boolean is_mop, irb_route_packet;

    /* For IRB(Integrated Routing and Bridging */
    irb_route_packet = FALSE;

    /*
     * If interface is in a dialer rotary group, restore pak->if_input
     * as it has been changed by fr_fast_pvc_switch_or_fs() from
     * a serial (or B channel) interface to a dialer (or D channel)
     * interface, without calling change_if_input().  This can cause
     * bogus input queue counts when the packet is discarded.
     */
    if (reg_invoke_dialer_huntgroup_member(idb)) {
	pak->if_input = idb->firstsw;
    }

    fr_idb = idb->frame_relay_stuff;
    if (!fr_idb) {
	datagram_done(pak);
        return;		/* paranoia check, should never really happen */
    }
    
    SET_PAK_DIALER_INPUT(pak);		/* Set real interface, for snapshot */

    /*
     * For ISDN and dialers, idb points to the physical interface
     * (B-ch, Ser), but we need the network protocol information that
     * resides on the rotary group leader (D-ch, Dialer), so get it here.
     */
    netidb = get_netidb(idb);	/* Get configuration idb */

    temp_dlci = GETSHORT(pak->datagramstart);
    pak->fr_dlci = temp_dlci;
    if ((temp_dlci & DLCI0_MASK) == FR_SIGNALING_DLCI) {
    	if (reg_invoke_Lapf_input(fr_idb, pak))
	    return;             /* packet queued for lapf process */
    }

    dlci = DLCI_TO_NUM(temp_dlci);
    if (dlci < MAXDLCI)
	fr_pvc = fr_idb->fr_dlci_tab[dlci];

    if ((temp_dlci != (NUM_TO_DLCI((ushort)0) | FRAME_RELAY_EA_BITS)) &&
        (temp_dlci != (NUM_TO_DLCI((ushort)1023) | FRAME_RELAY_EA_BITS))) {
	if (!fr_pvc) {
	    fr_idb->fr_err_dlci = dlci;
	    datagram_done(pak);
	    return;
	} 
	fr_update_ctrl_bits(pak, fr_pvc, temp_dlci);
	if (fr_pvc->fr_pr_enabled) {
	    dlci = fr_pvc->primary_dlci;
	    fr_pvc = fr_idb->fr_dlci_tab[dlci];
	    if (!fr_pvc) {
		datagram_done(pak);
		return;
	    }
	    PUTSHORT(pak->datagramstart,NUM_TO_DLCI(dlci)|FRAME_RELAY_EA_BITS);
	}
    }
    /*
     * Determine incoming dlci and update 
     */
    if ((temp_dlci == (NUM_TO_DLCI((ushort)0) | FRAME_RELAY_EA_BITS)) || 
		(temp_dlci == (NUM_TO_DLCI((ushort)1023) | FRAME_RELAY_EA_BITS))) {
	if (pak->datagramsize < FR_MIN_PKT_SIZE) {
	    datagram_done(pak);
	    return;
	}
	process_enqueue(fr_lmiQ, pak);
	return;
    } else {

	if (fr_pvc) {
	    if (is_subinterface(fr_pvc->idb) &&
		(fr_pvc->idb->subif_state == SUBIF_STATE_ADMINDOWN)) {
		datagram_done(pak);
		return;
	    }
	}
	/*
	 * check to see if this dlci can be switched over IP.
	 * If it is the one that exists, then switch it and return
	 * added by sanjeev for fr switching over an IP backbone.
	 * Do this only on leased lines.
	 */

	if (!is_ddr(idb)) {
	    if (fr_switch_process(idb, pak))
		return;
	}

	/*
	 * Overwrite pak->if_input with the IDB pointer from the PVC table
	 * in order to associate the packet with the appropriate subinterface.
	 * Use change_if_input() to make sure the queue counts are correctly
	 * handled (takes care of dialer rotary groups), see the comment at 
	 * the top of this routine.
	 */
	if (fr_pvc && fr_pvc->idb) {
	    /*
	     * If IRB(Integrated Routing and Bridging) is enabled,
	     * there is a possibility that we overwrote the input 
	     * interface to let it looks like it is coming from the BVI
	     * (Bridge-group Virtual Interface) when making bridge/route
	     * decision in tbridge fast-switching code; If these are
	     * true, don't bother to overwrite pak->if_input!
	     */
	    if (global_irb_enable && is_bvi(pak->if_input->hwptr)) {
		/* 
		 * Record the interface and remember that we want to 
		 * route this packet so we don't need to do this expensive
		 * check again later.
		 */
		pak->irb_rcvd_input = fr_pvc->idb;
		irb_route_packet = TRUE;
	    } else {
		if (!change_if_input(pak, fr_pvc->idb)) {
		    datagram_done(pak);
		    return;
		}
	    }
	} else if (reg_invoke_dialer_change_pak_if(idb, pak)) {
	    datagram_done(pak);
	    return;
	}

	/*
	 * fr_sort_packet() needs netidb to access network protocol data,
	 * fr_raw_enqueue() only needs physical idb.
	 */
	if (!(pak->flags & PAK_TBRIDGE_IN)) {
	    if (fr_sort_packet(netidb, &pak, temp_dlci)) {
	    	fr_raw_enqueue(idb,pak);
	    }
	    return;
	}
	/*
	 * Okay - we're dealing with bridging here
	 */
	if (pak->fr_flags & PAK_FR_IETF) {
	    if (fr_translate_1490_to_ether(pak) == FALSE) {
		datagram_done(pak);
		return;
	    }
	}
    	if (frpkt_debug)
	    debug_fr_pak(idb->firstsw, pak, 0, TRUE, INPUT);
	/*
	 * don't corrupt bridged pkts !!!
	 */
	pak->datagramsize -= FR_TRANSIT_ENCSIZE(pak);
	pak->datagramstart += FR_TRANSIT_ENCSIZE(pak);
	if ( fr_idb->fr_multicast_status != FR_MULTICAST_NOTSET ) {
	    if ( (*((ushort *)(pak->datagramstart-FRAME_RELAY_ENCAPBYTES)) ==
		  (ushort )(fr_idb->fr_multicast_dlci | FRAME_RELAY_EA_BITS))
		&& (*((ushort *)(pak->datagramstart-ADDRLEN_FR)) ==
		    (ushort )( TYPE_BRIDGE ))) {
		temp_dlci = GETSHORT(pak->datagramstart);
		pak->datagramsize -= ADDRLEN_FR;
		pak->datagramstart += ADDRLEN_FR;
		PUTSHORT(((uchar *)pak->datagramstart-FRAME_RELAY_ENCAPBYTES),
			 temp_dlci);
	    }
	}
	pak->enctype = ET_ARPA;
	link = frame_relay_getlink(pak);
	is_mop = (link == LINK_MOP_BOOT) || (link == LINK_MOP_CONSOLE);
	if ((link == LINK_ARP) || 
	    ((link == LINK_IP) && reg_invoke_ip_forus(pak)) ||
	    irb_route_packet) {
	    /*
	     * irb_route_packet:
	     * If the IRB logic in tbridge fast-switching code has decided
	     * that we should route this packet off the bridged interface,
	     * raw_enqueue it to the appropriate queue instead of enqueue
	     * it to bridgeQ!
	     */

	    /*
	     * Okay - granted.  This is ugly.  But it is needed
	     * for the 4K platforms.  The 4K platforms will realign the
	     * packet, which will copy the packet.  This will not copy
	     * the DLCI, which the bridging code copies.
	     */
	    ALIGN_DATAGRAM_CMD( {
		pak->datagramsize += FR_TRANSIT_ENCSIZE(pak);
		pak->datagramstart -= FR_TRANSIT_ENCSIZE(pak);
		pak_realign(pak, NULL);
		pak->datagramsize -= FR_TRANSIT_ENCSIZE(pak);
		pak->datagramstart += FR_TRANSIT_ENCSIZE(pak);
	    });
	    raw_enqueue(idb,pak);
	} else if (is_mop && reg_invoke_mop_forus(pak)) {
	    raw_enqueue(idb,pak);
	} else {
	    net_bridge_enqueue(pak);
	}
    }
}

/* 
 * Categorize the received packets. Set the IETF flag for those 
 * with SNAP and NLPID headers. Uncompress the packets with 
 * compressed headers so that they can be enqueued. 
 */
boolean fr_sort_packet (
			hwidbtype *idb, 
			paktype **pakptr, 
			ushort temp_dlci)
{
  ushort dlci = 0;
  frmaptype *map = NULL;
  vhdlctype *hdlc;
  paktype *newpak = NULL;
  snap_hdr *snap_read;
  paktype *pak;
  
  fr_idb_struct_type *fr_idb = idb->frame_relay_stuff;
  pak = *pakptr;
  dlci = DLCI_TO_NUM(temp_dlci); 
  switch (GETSHORT((uchar *)pak->datagramstart+ADDRLEN_FR)) {
  case FR_Q933_NLPID:
  case FR_IETF_IP:
  case FR_ISIS:
  case FR_ESIS:
  case FR_CLNP:
  case FR_ISO_IGRP:
  case FR_SNAP0PAD:
    pak->fr_flags |= PAK_FR_IETF;
    break;
  case FR_SNAP1PAD:
    pak->fr_flags |= PAK_FR_IETF;
    snap_read = (snap_hdr *)((uchar *)pak->datagramstart+ADDRLEN_FR); 
    if ((GETSHORT((ushort *)&snap_read->oui[1]) == FR_OUI23_BRIDGE) &&
	((GETSHORT(&snap_read->type) == FR1490_802_5) ||
	 (GETSHORT(&snap_read->type) == FR1490_802_5_FCS))) {
      
      pak->riflen = getRifLenPak(pak->addr_start, TR_RII);
      pak->info_start = pak->network_start = pak->rif_start + pak->riflen;
      pak->encsize = pak->network_start - pak->datagramstart;
    }
    break;
  case TYPE_COMPRESSED_FR:
    if ( !reg_used_late_q_uncompress(COMP_STAC) ) {
      datagram_done(pak);
      return FALSE;
    }

    /* tell frame_relay_periodic to create compression context for incoming
     * compressed packets
     */
    if ( !idb->comp_db ) {
      fr_idb->needsCompressionContext = TRUE;
      datagram_done(pak);
      return FALSE;
    }

    if ( TRUE ) {			/* dummy scope for var-decls below */
      ushort replType = GETSHORT( &((fr_payload_compressed_hdr *)pak->datagramstart)->trueType );
      ushort replAddr = GETSHORT( &((fr_payload_compressed_hdr *)pak->datagramstart)->hdr.dlci );
      pak->datagramstart += sizeof(fr_payload_compressed_hdr);
      pak->network_start += sizeof(fr_payload_compressed_hdr) - FRAME_RELAY_ENCAPBYTES; 
      pak->datagramsize -= sizeof(fr_payload_compressed_hdr);
      newpak = reg_invoke_late_q_uncompress(COMP_STAC, pak, 0);
      
      /* the pak returned will either be decompressed or aborted ... */
      if ( (newpak->flags & PAK_ABORT) == 0 ) {	/* it decomp'd */
	newpak->datagramstart -= FRAME_RELAY_ENCAPBYTES; 
	newpak->network_start = newpak->datagramstart + FRAME_RELAY_ENCAPBYTES; 
	newpak->datagramsize += FRAME_RELAY_ENCAPBYTES; 
	PUTSHORT( &((frame_relay_hdr *)newpak->datagramstart)->dlci, replAddr );
	PUTSHORT( &((frame_relay_hdr *)newpak->datagramstart)->type, replType );
	if (frame_relay_parse_packet(idb, newpak, NULL)) {
	  frame_relay_input(idb, newpak);             /* start over */
	}
      }
    }
    /* original packet is useless regardless of what happened */
    return FALSE;
    break;
  case TYPE_UNCOMPRESSED_TCP:
    hdlc = (vhdlctype *)(pak->datagramstart);
    hdlc->var_hdlctype = TYPE_IP10MB; 
    pak->linktype = LINK_UNCOMPRESSED_TCP;
    map = fr_verify_compression(idb, pak->linktype, dlci);
    if (!map) {
      if (fr_tcpip_hdr_comp)
	buginf("\nConfiguration Error: Dropping UNCOMPRESSED TCP packet");
      datagram_done(pak);
      return(FALSE);
    }
    pak = *pakptr = reg_invoke_uncompress_tcpip_header(LINK_IP, pak, idb, 
						       map->tcp_compress);
    if (pak == NULL) {
      return (FALSE);
    }
    break;
  case TYPE_COMPRESSED_TCP:
    hdlc = (vhdlctype *)(pak->datagramstart);
    hdlc->var_hdlctype = TYPE_IP10MB; 
    pak->linktype = LINK_COMPRESSED_TCP;
    map = fr_verify_compression(idb, pak->linktype, dlci);
    if (!map) {
      if (fr_tcpip_hdr_comp)
	buginf("\nConfiguration Error: Dropping COMPRESSED TCP packet");
      datagram_done(pak);
      return(FALSE);
    }
    pak = *pakptr = reg_invoke_uncompress_tcpip_header(LINK_IP, pak, idb, 
						       map->tcp_compress);
    /* 
     * Account for the Frame Relay Header. 
     * since the pak->datagramsize was recomputed within 
     * the uncompress routine (only for COMPRESSED_TCP packets). 
     */
    if (pak == NULL) {
      return (FALSE);
    }
    pak->datagramstart -= FRAME_RELAY_ENCAPBYTES; 
    pak->datagramsize += FRAME_RELAY_ENCAPBYTES;
    break;
    /*
     * Fudgebyte revenge. We have to skip a packing byte between
     * the end of the type field and the start of the CLNS fixed
     * header. This is required for backwards compatibility.
     */
  case TYPE_CLNS:
    pak->network_start++;
    pak->encsize++;
  }
  return(TRUE); 
}

/* 
 * fr_update_ctrl_bits
 * for the received packet, update the FECN/BECN and DE bit counters
 */
void fr_update_ctrl_bits (paktype *pak, fr_pvc_info_type *fr_pvc, ushort dlci)
{
    fr_pvc->pvc_stats.num_input_pkts++;
    fr_pvc->pvc_stats.num_bytes_rcv += pak->datagramsize;
 
    if (ISFR_FECN(dlci)) {
	fr_pvc->pvc_stats.num_rcvd_fecn_pkts++;
	pak->fr_flags |= PAK_FR_FECN;
	if (reg_invoke_generic_shaping_fecn_received(fr_pvc->idb, dlci))
	    fr_pvc->pvc_stats.num_sent_becn_pkts++;
    }
    if (ISFR_BECN(dlci)) {
	fr_pvc->pvc_stats.num_rcvd_becn_pkts++;
	pak->fr_flags |= PAK_FR_BECN;
	reg_invoke_generic_shaping_becn_received(fr_pvc->idb, fr_pvc);
    }
    if (ISFR_DE(dlci)) {
	fr_pvc->pvc_stats.num_rcvd_de_pkts++;
    }
}

/*
 * frame_relay_getlink
 * Get link type for a Frame Relay datagram.
 */

long frame_relay_getlink (paktype *pak)
{
    frame_relay_hdr *frame;
    snap_hdr *snap;
    ether_hdr *ether;
    fr_q933_hdr *q933;

    if (pak->flags & PAK_TBRIDGE_IN) {
	/*
	 * For packet with non-arpa encap
	 * we just make use of the inherited
	 * rxtype since the type2link() will
	 * just parse the arpa type
	 */
	switch(pak->rxtype) {
	case RXTYPE_NOVELL1:
	case RXTYPE_NOVELL2:
	case RXTYPE_SAP_NOVELL:
	case RXTYPE_SNAP_NOV1:
		return(LINK_NOVELL);
	default:
		break;
	}
	ether = (ether_hdr *)pak->mac_start;
	return(type2link(ether->type_or_len));
    }
    if (pak->enctype != ET_FRAME_RELAY)
	return(LINK_ILLEGAL);

    frame = (frame_relay_hdr *)pak->datagramstart;
    if (pak->fr_flags & PAK_FR_IETF ) {
	if (GETSHORT(&frame->type) == FR_SNAP1PAD) {
	    snap = (snap_hdr *)frame->snap_start;
	    if (GETSHORT(&snap->type) == FR1490_SPANNING)
		return(LINK_IEEE_SPANNING);
            if (GETSHORT(&snap->type) == FR1490_802_5)
                return(LINK_LLC2);
	    return(type2link(snap->type));
	} else if (GETSHORT(&frame->type) == FR_SNAP0PAD) {
	    snap = (snap_hdr *)(frame->snap_start - 1);
	    return(type2link(snap->type));
	} else if (GETSHORT(&frame->type) == FR_Q933_NLPID) {
	    q933 = (fr_q933_hdr *)(pak->datagramstart);
	    return(type2link(q933->l2_pid));
	}
	return(nlpid2link(GETSHORT(&frame->type)));
    }
    return(serial2link(GETSHORT(&frame->type)));
}

/* fr_can_mcast
 * Return TRUE if interface supports and is configured with any sort of
 * physical multicast capability.
 */

static boolean fr_can_mcast (hwidbtype *hwidb, enum LINK link)
{
    fr_idb_struct_type *fr_idb;

    fr_idb = hwidb->frame_relay_stuff;
    if (fr_idb->fr_multicast_status != FR_MULTICAST_NOTSET)
	return(TRUE);
    return(FALSE);
}

/*
 * frame_relay_bcast
 * Return FUZ_TRUE if broadcast packet, FUZ_FALSE if not
 */

static fuzzy
frame_relay_bcast (hwidbtype *hwidb, paktype *pak)
{
    fr_idb_struct_type *fr_idb;
    frame_relay_hdr *hdr;

    fr_idb = hwidb->frame_relay_stuff;

    hdr = (frame_relay_hdr *)pak->datagramstart;
    if ((GETSHORT(&hdr->dlci) &
	 ~FRAME_RELAY_EA_BITS) == fr_idb->fr_multicast_dlci)
	return(FUZ_TRUE);
    else
	return(FUZ_FALSE);
}


/*
 * frame_relay_check_idb
 *
 * returns TRUE if at least one DLCI is configured on this subinterface
 */
static boolean frame_relay_check_idb(idbtype *idb)
{
    int i;
    frmaptype *p;

    for (i=0; i<RELAYHASH; i++) {
	for (p = relaytable[i]; p; p = p->next) {
	    if (((p->map_type == FR_MAP_INT_DLCI) ||
		(p->map_type == FR_MAP_INT_DLCI_SVC) ||
		p->map_type == FR_MAP_STATIC) && idb == p->idb)
		return(TRUE);
	}
    }
    return(FALSE);
}


static void fr_add_bridge_flood (idbtype *swidb, fr_pvc_info_type *fr_pvc,
			  enum FR_ENCAPS_FORM encaps_ietf)
{
    queuetype *head;
    
    if (fr_pvc == NULL) {
	/*
	 * we don't have a PVC - must be a P2P
	 * search through the PVC table until we find the correct 1
	 */
	if (!(is_p2p(swidb))) {
	    if (frame_relay_debug)
		buginf("\n%%PVC is null, not P2P subint on %s",
		       swidb->namestring);
	    return;
	}
	fr_pvc = find_p2p_pvc_from_swidb(swidb);
	if (fr_pvc == NULL) {
	    if (frame_relay_debug)
		buginf("\n%%No frame-relay PVC defined for %s",
		       swidb->namestring);
	    return;
	}
	encaps_ietf =  swidb->hwptr->frame_relay_stuff->fr_ietf_encaps ?
	    FR_ENCAPS_IETF : FR_ENCAPS_CISCO;
    } else if (fr_pvc->idb != swidb) {
	if (frame_relay_debug)
	    buginf("\nPVC idb not same as swidb");
	return;
    }
    fr_pvc->tbridging_on_pvc = TRUE;
    fr_pvc->bridge_encap = encaps_ietf;
    fr_pvc->bridge_flood_next = NULL;
    head = &swidb->tbridge_circuitQ;
    if (head->qhead == NULL) {
	head->qhead = fr_pvc;
	head->qtail = fr_pvc;
    } else {
	fr_pvc_info_type *p, *q;
	q = NULL;
	p = head->qhead;
	while (p != NULL && fr_pvc->this_dlci >= p->this_dlci) {
	    q = p;
	    p = p->bridge_flood_next;
	}
	if (p != NULL && p->this_dlci == fr_pvc->this_dlci) {
	    if (frame_relay_debug)
		buginf("\nDLCI number %d already in bridge flood list",
		       p->this_dlci);
	} else {
	    if (q == NULL)
		head->qhead = fr_pvc;
	    else {
		q->bridge_flood_next = fr_pvc;
		if (head->qtail == q)
		    head->qtail = p;
	    }
	    fr_pvc->bridge_flood_next = p;
	}
    }
}

static void fr_delete_bridge_flood (fr_pvc_info_type *fr_pvc)
{
    fr_pvc_info_type *p, *q;
    queuetype *head;
    leveltype status;
    
    if (fr_pvc->tbridging_on_pvc == FALSE) {
	if (frame_relay_debug)
	    buginf("\nbridge flood not on pvc");
	return;
    }
    fr_pvc->tbridging_on_pvc = FALSE;
    status = raise_interrupt_level(ALL_DISABLE);
    head = &fr_pvc->idb->tbridge_circuitQ;
    p = head->qhead;
    q = NULL;
    while (p && p != fr_pvc) {
	q = p;
	p = p->bridge_flood_next;
    }
    if (p == NULL) {
	if (frame_relay_debug)
	    buginf("\ndlci not on bridge flood list");
	reset_interrupt_level(status);
	return;
    }
    if (q == NULL) {
	head->qhead = fr_pvc->bridge_flood_next;
    } else {
	q->bridge_flood_next = fr_pvc->bridge_flood_next;
    }
    if (head->qtail == fr_pvc)
	head->qtail = q;
    fr_pvc->bridge_flood_next = NULL;
    reset_interrupt_level(status);
}


static void fr_bridge_add_p2p_flood (idbtype *swidb)
{
    fr_add_bridge_flood(swidb, NULL, FR_ENCAPS_NULL);
}

static void fr_bridge_clear_p2p_flood (idbtype *swidb)
{
    fr_pvc_info_type *fr_pvc;

    if (!(is_p2p(swidb)))
	return;
    
    fr_pvc = find_p2p_pvc_from_swidb(swidb);
    if (fr_pvc)
	fr_delete_bridge_flood(fr_pvc);
}

/*
 * fr_pri_delete_bridge_flood
 *   If the bridge_fr_pvc is a member of the priority-group-dlci,
 * then this pvc may not be the one that is actually stored on
 * tbridge_curcuitQ list. Therefore, we need to search for the
 * one stored on this list and remove the correct one (via
 * fr_delete_bridge_flood).
 */
 
void fr_pri_delete_bridge_flood (fr_pvc_info_type *bridge_fr_pvc,
                                 fr_idb_struct_type *fr_idb)
{
    int i;
    ushort dlci;
    fr_pvc_info_type *primary_fr_pvc;
 
    primary_fr_pvc = bridge_fr_pvc;
    if (bridge_fr_pvc->fr_pr_enabled)
        primary_fr_pvc = fr_idb->fr_dlci_tab[bridge_fr_pvc->primary_dlci];
 
    if (primary_fr_pvc->fr_pr_dlci_group) {
        for (i = PRIORITY_HIGH; i <= PRIORITY_LOW; i++) {
            dlci = primary_fr_pvc->fr_pr_dlcis[i];
            bridge_fr_pvc = fr_idb->fr_dlci_tab[dlci];
            if (bridge_fr_pvc->tbridging_on_pvc)
                break;
        }
    }
 
    reg_invoke_tbridge_clear_cache(bridge_fr_pvc->idb->span_index);
    fr_delete_bridge_flood(bridge_fr_pvc);

}

/*
 * fr_pri_delete_add_bridge_flood
 *   This function will delete the orig_fr_pvc from the bridging table
 * (via fr_delete_bridge_flood) and adds new_fr_pvc to the bridging
 * table (via fr_add_bridge_flood).
 */
 
void fr_pri_delete_add_bridge_flood (fr_pvc_info_type *orig_fr_pvc,
                                     fr_pvc_info_type *new_fr_pvc,
                                     fr_idb_struct_type *fr_idb)
{
    enum FR_ENCAPS_FORM bridge_encaps_type;
 
    bridge_encaps_type = orig_fr_pvc->bridge_encap;
    reg_invoke_tbridge_clear_cache(orig_fr_pvc->idb->span_index);
    fr_delete_bridge_flood(orig_fr_pvc);
    fr_add_bridge_flood(new_fr_pvc->idb, new_fr_pvc, bridge_encaps_type);
 
}


/* reset_fr_pvc_from_priority_list
 *   Before removing the non-primary dlci, we need to reset the bridging
 * dlci to the primary dlci.
 */
void reset_fr_pvc_from_priority_list (ushort dlci,
                                      fr_pvc_info_type *primary_fr_pvc,
                                      fr_pvc_info_type *current_fr_pvc,
                                      fr_idb_struct_type *fr_idb)
{
 
    /*
     * if "frame-relay map bridge" is associated to this dlci, then we don't
     * want to reset the bridge cache because it already contains the correct
     * information.
     */
    if (fr_idb->map_status[dlci] & FR_BR_MAPPED)
        return;
 
    /*
     * same as above but for point-to-point interfaces
     */
    if (is_p2p(primary_fr_pvc->idb) && (fr_idb->map_status[dlci] & FR_ALL_MAPPED))
        return;
 
    if (current_fr_pvc->tbridging_on_pvc)
        fr_pri_delete_add_bridge_flood(current_fr_pvc, primary_fr_pvc, fr_idb);
  
}

/*
 * get_pri_from_pri_list
 *  Searches the priority list for a match on linkcode.
 * It will return the priority of the pvc if it finds a match,
 * otherwise, it returns -1.
 */
int get_pri_from_priority_list (fr_pvc_info_type *fr_pvc,
                           uchar linkcode)
{
    plisttype *plist;
 
    if (parray[fr_pvc->fr_pr_dlci_group]) {
        plist = parray[fr_pvc->fr_pr_dlci_group]->plist;
        while (plist) {
            if (plist->linkcode == linkcode)
                return(plist->priority);
            plist = plist->next;
        }
 
    }
    return(-1);
}

/*
 * set_fr_pvc_from_priority_list
 * Sets the bridging pvc to the correct pvc given a priority.
 * This function may be later modify to support other protocols
 * than bridging
 */
 
void set_fr_pvc_from_priority_list (fr_pvc_info_type *primary_pvc,
                                    fr_idb_struct_type *fr_idb)
{
    int priority;
    ushort new_bridge_dlci;
    fr_pvc_info_type *new_bridge_pvc;
 
    if (primary_pvc->tbridging_on_pvc) {
        priority = get_pri_from_priority_list(primary_pvc, LINK_BRIDGE);
 
        /*
         * priority is negative is the priority list does not have a
         * element with LINK_BRIDGE type.
         */
        if (priority > 0) {
            new_bridge_dlci = primary_pvc->fr_pr_dlcis[priority];
            if (new_bridge_dlci != primary_pvc->this_dlci) {
                new_bridge_pvc = fr_idb->fr_dlci_tab[new_bridge_dlci];
                fr_pri_delete_add_bridge_flood(primary_pvc, new_bridge_pvc, fr_idb);
            }
        }
    }
}
static void fr_set_idbmap (fr_idb_struct_type *fr_idb, ushort dlci, 
	            enum MAP_TYPE maptype, uint link)
{
    if ( maptype == FR_MAP_STATIC )
	switch ( link ) {
	case LINK_COMPRESSED_TCP:
	case LINK_IP:
	    fr_idb->map_status[dlci] |= FR_IP_MAPPED;
	    fr_idb->map_dynamic[dlci] &= ~FR_IP_DYNAMIC;
	    break;
	case LINK_NOVELL:
	    fr_idb->map_status[dlci] |= FR_IPX_MAPPED;
	    fr_idb->map_dynamic[dlci] &= ~FR_IPX_DYNAMIC;
	    break;
	case LINK_APPLETALK:
	    fr_idb->map_status[dlci] |= FR_AT_MAPPED;
	    fr_idb->map_dynamic[dlci] &= ~FR_AT_DYNAMIC;
	    break;
	case LINK_XNS:
	    fr_idb->map_status[dlci] |= FR_XNS_MAPPED;
	    fr_idb->map_dynamic[dlci] &= ~FR_XNS_DYNAMIC;
	    break;
	case LINK_DECNET:
	    fr_idb->map_status[dlci] |= FR_DNET_MAPPED;
	    fr_idb->map_dynamic[dlci] &= ~FR_DNET_DYNAMIC;
	    break;
	case LINK_VINES:
	    fr_idb->map_status[dlci] |= FR_VINES_MAPPED;
	    fr_idb->map_dynamic[dlci] &= ~FR_VINES_DYNAMIC;
	    break;
	case LINK_CLNS:
	    fr_idb->map_status[dlci] |= FR_CLNS_MAPPED;
	    break;
	case LINK_BRIDGE:
	    fr_idb->map_status[dlci] |= FR_BR_MAPPED;
	    break;
	case FR_RSRB_MAPPED:
	    fr_idb->map_status[dlci] |= FR_RSRB_MAPPED;
	    break;
	case FR_STUN_MAPPED:
	    fr_idb->map_status[dlci] |= FR_STUN_MAPPED;
	    break;
	case FR_LLC2_MAPPED:
	    fr_idb->map_status[dlci] |= FR_LLC2_MAPPED;
	    break;
	default:
	    break;
	}
    if ( maptype == FR_MAP_DYNAMIC )
	switch ( link ) {
	case LINK_COMPRESSED_TCP:
	case LINK_IP:
	    fr_idb->map_dynamic[dlci] |= FR_IP_DYNAMIC;
	    break;
	case LINK_NOVELL:
	    fr_idb->map_dynamic[dlci] |= FR_IPX_DYNAMIC;
	    break;
	case LINK_APPLETALK:
	    fr_idb->map_dynamic[dlci] |= FR_AT_DYNAMIC;
	    break;
	case LINK_XNS:
	    fr_idb->map_dynamic[dlci] |= FR_XNS_DYNAMIC;
	    break;
	case LINK_DECNET:
	    fr_idb->map_dynamic[dlci] |= FR_DNET_DYNAMIC;
	    break;
	case LINK_VINES:
	    fr_idb->map_dynamic[dlci] |= FR_VINES_DYNAMIC;
	    break;
	default:
	    break;
	}
    if (maptype == FR_MAP_INT_DLCI)
	fr_idb->map_status[dlci] = FR_ALL_MAPPED;
}

static void fr_unset_idbmap (fr_idb_struct_type *fr_idb, ushort dlci, 
	              enum MAP_TYPE maptype, uint link)
{
    fr_pvc_info_type *fr_pvc;
    
    fr_pvc = fr_idb->fr_dlci_tab[dlci];
    if (maptype == FR_MAP_STATIC)
	switch (link ) {
	case LINK_COMPRESSED_TCP:
	case LINK_IP:
	    fr_idb->map_status[dlci] &= ~FR_IP_MAPPED;
	    break;
	case LINK_NOVELL:
	    fr_idb->map_status[dlci] &= ~FR_IPX_MAPPED;
	    break;
	case LINK_APPLETALK:
	    fr_idb->map_status[dlci] &= ~FR_AT_MAPPED;
	    break;
	case LINK_XNS:
	    fr_idb->map_status[dlci] &= ~FR_XNS_MAPPED;
	    break;
	case LINK_DECNET:
	    fr_idb->map_status[dlci] &= ~FR_DNET_MAPPED;
	    break;
	case LINK_VINES:
	    fr_idb->map_status[dlci] &= ~FR_VINES_MAPPED;
	    break;
	case LINK_CLNS:
	    fr_idb->map_status[dlci] &= ~FR_CLNS_MAPPED;
	    break;
	case LINK_BRIDGE:
	    fr_pri_delete_bridge_flood(fr_pvc, fr_idb);
	    fr_idb->map_status[dlci] &= ~FR_BR_MAPPED;
	    break;
	case FR_RSRB_MAPPED:
	    fr_idb->map_status[dlci] &= ~FR_RSRB_MAPPED;
	    break;
	case FR_STUN_MAPPED:
	    fr_idb->map_status[dlci] &= ~FR_STUN_MAPPED;
	    break;
	case FR_LLC2_MAPPED:
	    fr_idb->map_status[dlci] &= ~FR_LLC2_MAPPED;
	    break;
	default:
	    break;
	}
    if (maptype == FR_MAP_DYNAMIC )
	switch (link) {
	case LINK_COMPRESSED_TCP:
	case LINK_IP:
	    fr_idb->map_dynamic[dlci] &= ~FR_IP_DYNAMIC;
	    break;
	case LINK_NOVELL:
	    fr_idb->map_dynamic[dlci] &= ~FR_IPX_DYNAMIC;
	    break;
	case LINK_APPLETALK:
	    fr_idb->map_dynamic[dlci] &= ~FR_AT_DYNAMIC;
	    break;
	case LINK_XNS:
	    fr_idb->map_dynamic[dlci] &= ~FR_XNS_DYNAMIC;
	    break;
	case LINK_DECNET:
	    fr_idb->map_dynamic[dlci] &= ~FR_DNET_DYNAMIC;
	    break;
	case LINK_VINES:
	    fr_idb->map_dynamic[dlci] &= ~FR_VINES_DYNAMIC;
	    break;
	default:
	    break;
	}
    if ((maptype == FR_MAP_INT_DLCI) ||
			(maptype == FR_MAP_INT_DLCI_SVC))
	fr_idb->map_status[dlci] = FR_NOT_MAPPED;
}


/*
 * frame_relay_parse_map
 * Parse a map command
 */

int frame_relay_parse_map (
			   parseinfo *csb, 
			   idbtype *idb)
{
  addrtype proto_addr;
  ushort dlci;
  boolean broadcast = FALSE;
  uchar encaps_ietf;
  frmaptype *addr;
  fr_idb_struct_type *fr_idb;
  fr_pvc_info_type *fr_pvc;
  boolean parse_rtn = FALSE;
  uint compr_options = FR_MAP_INIT_OPTIONS;
  uint linktype, temp_link, temp_ietf;
  ushort local_pvc; 
  
  fr_idb =  idb->hwptr->frame_relay_stuff;
  
  memset(&proto_addr, 0, sizeof(addrtype));
  dlci = GETOBJ(int,1);
  linktype = GETOBJ(int,10);
  
  /* Complain if this DLCI is assigned to a different interface. */
  
  fr_pvc = fr_idb->fr_dlci_tab[dlci];
  if (fr_pvc && fr_pvc->idb && fr_pvc->idb != idb &&
      fr_pvc->pvc_usage != PVC_NOTSET && (fr_pvc->pvc_type != PVC_AUTOINSTALL)) {
    printf("\n%%PVC already assigned to interface %s",
	   fr_pvc->idb->namestring);
    return(NULL_DLCI);
  }
  
  if (!LINK_LEARN_OK(linktype))
    parse_rtn = parse_proto_addr(csb, NONULL, &proto_addr, GETOBJ(paddr,1),
				 linktype);
  else
    parse_rtn = parse_proto_addr(csb, NONULL, &proto_addr,
				 (addrtype *) &dlci,
				 linktype); 
  
  if (!parse_rtn) {
    printf("\n%%Unable to parse map address");
    return(NULL_DLCI);
  }
  addr = frame_relay_addr_lookup(idb, &proto_addr, linktype);
  if (!csb->sense) {
    if (addr) {
	reg_invoke_bump_cache_version(idb->hwptr, NULL, FALSE);
      local_pvc = addr->dlci_num;
      fr_payload_compression_command(csb, addr);
      frame_relay_map_delete(addr);
      return(local_pvc);
    } else {
      char buf[64];
      print_proto_addr(idb, &proto_addr, linktype, buf);
      printf("\n%%Can't find address map for %s", buf);
      return(NULL_DLCI);
    }
  }
  broadcast = GETOBJ(int,3);
  encaps_ietf = GETOBJ(int,4);
  
  /*
   * Fetch the "active/passive" option. Also fetch the "nocompress"
   * option. 
   */
  compr_options = GETOBJ(int,2) | GETOBJ(int,5);
  compr_options = fr_inherit_compr_attributes(idb, compr_options, 
					      linktype, encaps_ietf);
  /* 
   * For one special case, force the encapsulation type.
   * This allows us to configure header compression on a particular 
   * CISCO PVC when the interface is configured as IETF. Particularly
   * when the user forgets to specify the PVC as being of type CISCO.
   * note: header compression will only work over CISCO encapsulation. 
   */
  if ((linktype == LINK_COMPRESSED_TCP) && 
      (fr_idb->fr_ietf_encaps == TRUE) &&
      (encaps_ietf == FR_ENCAPS_NOTSET))
    encaps_ietf = FR_ENCAPS_CISCO;
  
  /*
   * Check to see if this entry is in the map in its
   * entirety. If so then update with newer information
   * but don't complain. Otherwise check each part of the
   * map for redundancy.
   */
  if (addr == NULL) {
    if ((idb->subif_link_type == SUBIF_TYPE_P2P) && 
	frame_relay_check_idb(idb)) {
      printf("\n%%Can't add VC to point-to-point subinterface");
      return(NULL_DLCI);
      
    } else if (!(addr = fr_addr_enter(idb, &proto_addr, dlci,  broadcast,
				      encaps_ietf, FR_MAP_STATIC,
				      PVC_STATIC, linktype, 
				      compr_options, NULL))) {
      printf("\n%%Error entering address map");
      return(NULL_DLCI);
    } else if ( encaps_ietf == FR_ENCAPS_CISCO2 ) {
      fr_payload_compression_command(csb, addr);
    } 
  } else {
    if ((bcmp(&addr->proto_addr, &proto_addr, sizeof(addrtype)) == 0) &&
	((addr->link == linktype)  ||
	 fr_valid_link_linktype_combo(linktype, addr->link)) &&
	(NUM_TO_DLCI(dlci) == addr->dlci)) {
      temp_link = addr->link;
      temp_ietf = addr->encaps_type;
      addr->link = linktype;
      addr->broadcast = broadcast;
      addr->encaps_type = encaps_ietf;
      addr->map_type = FR_MAP_STATIC;
      addr->compression_flags = compr_options;
      fr_set_idbmap(fr_idb, dlci, addr->map_type, addr->link);
      if ((linktype == LINK_BRIDGE) || (linktype == LINK_ILLEGAL))
	fr_pvc->bridge_encap = encaps_ietf;
      
      /*
       * If "nocompress" was specified or if a COMPRESSED_TCP link
       * is being converted to a IP link, get rid of the compression
       * related memory. 
       */
      if ((compr_options & FR_MAP_NO_COMPRESS) || 
	  ((linktype == LINK_IP) &&
	   (temp_link == LINK_COMPRESSED_TCP)) ||
	  (encaps_ietf == FR_ENCAPS_IETF)) {
	free(addr->tcp_compress);
	addr->tcp_compress = NULL;
	addr->compression_flags |= FR_MAP_NO_COMPRESS;	/* signal to payload-compress */
	fr_payload_compression_command(csb, addr);
      }
      /*
       * Case I: 
       * If an IP link is being converted to a COMPRESSED_TCP link, 
       * delete the existing map and enter a new one. This will allow
       * the compression related memory to be allocated.
       * 
       * Case II: 
       * Or if the interface is set for compression and 
       * an IP map originally configured as IETF is 
       * converted to a CISCO or NOTSET encaps, then this 
       * IP map needs to inherit compression characteristics.
       */
      else if (fr_valid_linktype_change(temp_link, linktype) ||
	       (fr_valid_encaps_change(idb, encaps_ietf, temp_ietf))) {
	frame_relay_map_delete(addr);
	addr = fr_addr_enter(idb, &proto_addr, dlci,  broadcast,
			     encaps_ietf, FR_MAP_STATIC,
			     PVC_STATIC, linktype, 
			     compr_options, NULL);
	if (!addr)
	  printf("\n%%Error converting map from IP to COMPRESSED_TCP");
      } else if ( encaps_ietf == FR_ENCAPS_CISCO2 ) {
	if ( !fr_payload_compression_command(csb, addr) ) {
	  printf("\nPayload compression not enabled!");
	}
      }
      return(NULL_DLCI);
    } else {
      printf("\n%%Address already in map");
      return(NULL_DLCI);
    }
  }
  return(addr->dlci_num);
}

static frmaptype *findMapEntry (idbtype *idb)
{
  frmaptype *p;
  int i;  
  
  for (i=0; i<RELAYHASH; i++)
    for (p = relaytable[i]; p; p = p->next)
      if (p->idb == idb) {
	return p;
      }
  return NULL;
}

/*
 * fr_payload_compression_command(csb, addr);
 * Configure that payload compression
 */
static boolean fr_payload_compression_command (parseinfo *csb,
					       frmaptype *addr)
{
  hwidbtype *hwidb;
  frmaptype *p = NULL;
  
  if ((hwidb = hwidb_or_null(csb->interface)) == NULL) {
    printf("\ncouldn't get hwidb!  compression state not changed.");
    return FALSE;
  }
  
  if (addr)  {					/* map command */
    p = addr;
  } else if (is_p2p(csb->interface)) {		/* p2p */
    p = findMapEntry( csb->interface );
  } else
    p =  NULL;
  
  if (!p) {
    printf("\ncouldn't find map! compression state not changed.");
    return FALSE;
  }
  
  if (csb->sense) {				/* enabling... */
    boolean context_okay = TRUE;
    if (!hwidb->comp_db) {
      context_okay =  reg_invoke_init_stac_context(hwidb);
    }
    if (context_okay && 
	!(p->compression_flags & FR_MAP_COMP_PAYLOAD_STAC)) {
      p->compression_flags = p->compression_flags | FR_MAP_COMP_PAYLOAD_STAC;
      hwidb->comp_db->vc_contexts++;
    }
  } else { 					/* 'no' prefix...disabling */
    if (hwidb->comp_db && (p->compression_flags & FR_MAP_COMP_PAYLOAD_STAC)) {
      p->compression_flags = p->compression_flags & ~FR_MAP_COMP_PAYLOAD_STAC;
      hwidb->comp_db->vc_contexts--;
      if (hwidb->comp_db->vc_contexts <= 0) {
	reg_invoke_remove_comp_context(hwidb);
      }
    }
  }
  return TRUE;
}

/*
 * frame_relay_bridge_address
 *
 * Called by the bridge code when a new static address is added
 */
static void frame_relay_bridge_address (parseinfo *csb,
					spantype *span,
					boolean *success)
{
    idbtype *interface;
    int dlci;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;

    /*
     * If someone's already rained on our parade, exit now
     */
    if (!*success)
	return;

    interface = GETOBJ(idb,1);

    if (interface && is_frame_relay(interface->hwptr)) {
	if (!(dlci = GETOBJ(int,3))) {
            printf ("\nThis interface requires a DLCI number");
	    *success = FALSE;
            return;
	} else {
            /* Check to see if we already have a PVC struc.
             * If not, Create and Initialize a PVC structure
             */
            fr_idb = interface->hwptr->frame_relay_stuff;
            fr_pvc = fr_idb->fr_dlci_tab[dlci];
            if (!fr_pvc) {
		fr_pvc_init(interface, dlci, PVC_LOCAL, PVC_STATIC,
			    FALSE, NULL);
	    }
	}
    }
}


/*
 * fr_find_multistate
 *	find out multi point what subinterface status supposed to be
 */
static uchar fr_find_multistate (idbtype *swidb)
{

    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    int i, pvc_count;
    uchar multi_status;

    fr_idb = swidb->hwptr->frame_relay_stuff;

    if (!fr_idb)
	return SUBIF_STATE_UP;
    
    pvc_count = 0;
    multi_status = SUBIF_STATE_DOWN;
    for (i = 0; i < MAXDLCI; i++) {
	if (!(fr_pvc = fr_idb->fr_dlci_tab[i]) || fr_pvc->idb != swidb)
	    continue;
	/*
	 * For multipoint, we change to up only when there is a
	 *     PVC status up.
	 */
	pvc_count++;
	if (IS_PVC_ACTIVE_BIT_SET(fr_pvc->pvc_status)) {
	     multi_status = SUBIF_STATE_UP;
	}
    }
    if (pvc_count) {
	return (multi_status);
    } else {
	return (SUBIF_STATE_UP);
    }
}


/*
 * fr_subif_stay_down
 *	
 *     check to see if subinterface should stay down
 */
static boolean fr_subif_stay_down (idbtype *swidb)
{
    fr_idb_struct_type *fr_idb;

    fr_idb = swidb->hwptr->frame_relay_stuff;
    
    if (hw_interface_up(swidb->hwptr)) {
	return FALSE;
    }
    if (swidb->subif_state != SUBIF_STATE_DOWN) {
	return FALSE;
    }
    if (fr_idb->fr_intf_type == FR_DTE && !(swidb->hwptr->nokeepalive)) {
	return FALSE;
    }

    swidb->subif_state = SUBIF_STATE_DOWN;
    return TRUE;
}


/*
 * fr_set_subif_state
 *
 * Set the subinterface state (p2p) or subinterface tmp_state (multipoint) by
 * 1. forced assign
 * if it is not administrative down
 *   2. hwidb state
 *   3. status
 *      This can be pvc_status or pvc_tmp_status or subif_state used to set
 *      subif_state.
 *
 */
void fr_set_subif_state (idbtype *idb, int flag, uchar status)
{
    uchar o_status;

    if (!is_subinterface(idb)) {
	return;
    }
    o_status = idb->subif_state;
    if (flag == FR_FORCE_STATE) {
	idb->subif_state = status;
	goto out;
    }
    
    if (idb->subif_state != SUBIF_STATE_ADMINDOWN &&
	idb->subif_state != SUBIF_STATE_DELETED) {
	if (fr_subif_stay_down(idb)) {
	    return;
	}
	switch (flag) {
	case FR_ASSIGN_STATE:
	    idb->subif_state = status;
	    break;
	case FR_HWIF_STATE:
	    if (hw_interface_up(idb->hwptr)) {
		idb->subif_state = SUBIF_STATE_UP;
	    }
	    break;
	case FR_PVC_STATE:
	    if (is_p2p(idb)) {
		idb->subif_state =
		    (IS_PVC_ACTIVE_BIT_SET(status) ?
		     SUBIF_STATE_UP : SUBIF_STATE_DOWN);
	    } else {
		idb->subif_state = fr_find_multistate(idb);
	    }
	    break;
	}
	
    }
out:
    if (o_status != idb->subif_state) {
	    errmsg(&msgsym(UPDOWN, LINEPROTO), idb->namestring,
		   (idb->subif_state == SUBIF_STATE_UP) ? "up" : "down");
            subif_line_statechange(idb, idb->subif_state, TRUE);
    }
}


/*
 * Parse components of a DE list
*/
static void fr_de_parse(parseinfo *csb,
		 int number)
{
    int option,linkcode;
    int testlist, lt_size, gt_size;
    int udp_port, tcp_port;
    fr_listtype *tmplist,*pred,*list;
    boolean ip_fragments;
    hwidbtype *interface;
    
    option = GETOBJ(int,2);
    list = fr_listarray[number];
    switch (option) {
    case FR_PRICMD_PROTOCOL:
	linkcode = GETOBJ(int,3);
	/*
	 * Get priority type
	 */
	/*
	 * Parse further conditions:
	 *  - "list <list-number>"
	 *  - "gt <size>"
	 *  - "lt <size>"
	 *  - "tcp" <port>"
	 *  - "udp"  <port>"
	 *  - "fragments"
	 */
	testlist = 0;
	gt_size = 0;
	lt_size = 0;
	udp_port = 0;
	tcp_port = 0;
	ip_fragments = FALSE;
	switch (GETOBJ(int,5)) {
	case FR_PR_LIST:
	    testlist = GETOBJ(int,6);
	    break;
	case FR_PR_LT:
	    lt_size = GETOBJ(int,6);
	    break;
	case FR_PR_GT:
	    gt_size = GETOBJ(int,6);
	    break;
	case FR_PR_TCP:
	    if (linkcode != LINK_IP) {
		printf("tcp arg valid only for IP");
		return;
	    }
	    tcp_port = GETOBJ(int,6);
	    break;
	case FR_PR_UDP:
	    if (linkcode != LINK_IP) {
		printf("udp arg valid only for IP");
		return;
	    }
	    udp_port = GETOBJ(int,6);
	    break;
	case FR_PR_FRAGS:
	    ip_fragments = TRUE;
	    break;
	default:
	    /* Do Nothing */
	    break;
	}
	
	/*
	 * Parsing done. Verify protocol and list range.
	 */
	if (testlist &&
	    !reg_invoke_valid_access_list(linkcode, testlist)) {
	    printf("bad list number");
	    return;
	}
	
	/*
	 * Search the priority array for an item that matches on link and
	 * priority. If we find an item, update or delete it and return.
	 * "pred" is the pointer to the last item in the list.
	 */
	pred = NULL;
	for (tmplist = list; tmplist;
	     pred = tmplist, tmplist = tmplist->next) {
	    if (tmplist->linkcode == linkcode) {
		if (tmplist->testlist != testlist) {
		    continue;
		}
		if ((tmplist->gt_size != gt_size) ||
		    (tmplist->lt_size != lt_size)) {
		    continue;
		}
		if ((tmplist->tcp_port != tcp_port) ||
		    (tmplist->udp_port != udp_port)) {
		    continue;
		}
		if (tmplist->ip_fragments != ip_fragments)
		    continue;
		if (!csb->sense) {
		    if (pred) {
			pred->next = tmplist->next;
		    } else {
			fr_listarray[number] = tmplist->next;
		    }
		    free(tmplist);
		}
		return;
	    }
	}
	
	/*
	 * Item not in list.  Create it at end of list.
	 */
	if (!csb->sense) {
	    return;
	}
	tmplist = malloc_named(sizeof(fr_listtype), "FR Protocol DE List");
	if (!tmplist) {
	    printf(nomemory);
	    return;
	}
	tmplist->linkcode = linkcode;
	tmplist->testlist = testlist;
	tmplist->gt_size = gt_size;
	tmplist->lt_size = lt_size;
	tmplist->tcp_port = tcp_port;
	tmplist->udp_port = udp_port;
	tmplist->ip_fragments = ip_fragments;
	if (pred) {
	    pred->next = tmplist;
	} else {
	    fr_listarray[number] = tmplist;
	}
	break;
    case FR_PRICMD_INTERFACE:
	interface = hwidb_or_null(GETOBJ(idb,1));
	/*
	 * Search the priority array for an item that matches on interface
	 * and priority. If we find an item, possibly delete it, then return.
	 * "pred" is the pointer to the last item in the list.
	 */
	pred = NULL;
	for (tmplist = list; tmplist;
	     pred = tmplist, tmplist = tmplist->next) {
	    if (tmplist->interface && (tmplist->interface == interface)) {
		if (!csb->sense) {
		    if (pred) {
			pred->next = tmplist->next;
		    } else {
			fr_listarray[number] = tmplist->next;
		    }
		    free(tmplist);
		}
		return;
	    }
	}
	
	/*
	 * Item not in list.  Create it at end of list.
	 */
	if (!csb->sense) {
	    return;
	}
	tmplist = malloc_named(sizeof(fr_listtype), "FR Interface DE List");
	if (!tmplist) {
	    printf(nomemory);
	    return;
	}
	tmplist->interface = interface;
	if (pred) {
	    pred->next = tmplist;
	} else {
	    fr_listarray[number] = tmplist;
	}
	break;
    }
}

/*
 * nv_fr_listwrite
 * Write out the classification lists
 */
static void nv_fr_listwrite (fr_listtype *list,
		      parseinfo *csb,
		      int number,
		      int type)
{
    fr_listtype *tmplist;
    
    switch (type) {
    case FR_PRICMD_INTERFACE:
	if (!list)
	    return;
	for (tmplist = list; tmplist; tmplist = tmplist->next) {
	    if (tmplist->interface != NULL) {
		nv_write(TRUE, "%s %s", csb->nv_command, 
			 tmplist->interface->hw_namestring);
	    }
	}
	break;
    case FR_PRICMD_PROTOCOL:
	if (!list)
	    return;
	for (tmplist = list; tmplist; tmplist = tmplist->next) {
	    if (tmplist->interface)
		continue;
	    nv_write(TRUE, "%s %s", csb->nv_command,
		     print_linktype(tmplist->linkcode));
	    nv_add(tmplist->gt_size, " gt %d", tmplist->gt_size);
	    nv_add(tmplist->lt_size, " lt %d", tmplist->lt_size);
	    nv_add(tmplist->testlist, " list %d", tmplist->testlist);
	    if (tmplist->tcp_port) {
		nv_add(TRUE, " tcp ");
		reg_invoke_nv_add_tcp_port(tmplist->tcp_port);
	    }
	    if (tmplist->udp_port) {
		nv_add(TRUE, " udp ");
		reg_invoke_nv_add_udp_port(tmplist->udp_port);
	    }
	    nv_add(tmplist->ip_fragments, " fragments");
	}
	break;
    }
}

/*
 * fr_dlci_group_clear
 * Clear the DLCI prioritization configuration
*/
static void fr_dlci_group_clear (fr_idb_struct_type *fr_idb,
			  fr_pvc_info_type *fr_pvc)
{
    fr_pvc_info_type *tmp_pvc;
    ushort dlci,i;
    

    fr_pvc->fr_pr_dlci_group = 0;
    fr_pvc->fr_pr_enabled = FALSE;
    for (i = PRIORITY_MEDIUM; i <= PRIORITY_LOW; i++) {
	dlci = fr_pvc->fr_pr_dlcis[i];
	fr_pvc->fr_pr_dlcis[i] = 0;
	if (dlci == fr_pvc->fr_pr_dlcis[PRIORITY_HIGH])
	    continue;
	tmp_pvc = fr_idb->fr_dlci_tab[dlci];
	if (!tmp_pvc)
	    continue;
	reset_fr_pvc_from_priority_list(dlci, fr_pvc, tmp_pvc, fr_idb);
	fr_pvc_clear(fr_pvc->idb->hwptr, dlci);
    }
}    

/*
 * frame_relay_global_command
 *
 * parse frame relay global configuration command
 */

void frame_relay_global_command (parseinfo *csb)
{
    fr_listtype *frlist;
    int number;
    
    if (csb->nvgen) {
	switch (csb->which) {
	case FRAME_RELAY_SWITCHING:
	    nv_write(fr_switching != FALSE, "%s", csb->nv_command);
	    break;
	case FR_LISTNUM:
	    number = GETOBJ(int,1);
	    frlist = fr_listarray[number];
	    nv_fr_listwrite(frlist,csb,number,GETOBJ(int,2));
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }
    switch (csb->which) {
    case FRAME_RELAY_SWITCHING:
	if (!csb->sense) {
	    fr_switching = FALSE;
	} else {
	    fr_switching = TRUE;
	}
	break;
    case FR_LISTNUM:
	number = GETOBJ(int,1);
	fr_de_parse(csb,number);
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * fr_dlci_exit_command
 */
void fr_dlci_exit_command (parseinfo *csb)
{
    idbtype *idb;
    
    idb = csb->interface;
    
    if (is_subinterface(idb)) {
	csb->mode = subinterface_mode;
    } else {
	csb->mode = interface_mode;
    }
}


/*
 * frame_relay_command
 * Parse the "frame-relay" command
 */

void frame_relay_command (parseinfo *csb)
{
    idbtype *swidb = csb->interface;
    hwidbtype *idb = swidb->hwptr;
    unsigned tmp_mc, tmp_lcl, tmp_lmi;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc = NULL;
    fr_idb =  idb->frame_relay_stuff;

    if (idb->enctype != ET_FRAME_RELAY) {
	if (!csb->nvgen) {
		printf("\n%%%s :Frame Relay not configured on interface", 
					idb->hw_namestring);
	}
	return;
    }

    if (reg_invoke_dialer_huntgroup_member(idb)) {
	if (!csb->nvgen) {
	    printf("\nCannot change frame relay configuration of hunt group"
		   " member");
	}
	return;
    }

    if (csb->nvgen) {
	switch (csb->which) {
	case FR_TCP_COMPRESS:
	    if (swidb->fr_tcp_hdr_comp) {
		nv_write(TRUE, csb->nv_command);
		if (swidb->fr_hdr_comp_passive)
		    nv_add(TRUE, " passive");
	    }
	    break;
	case FR_DEGROUP:
	    {
		int i;
		
		for (i = MIN_DLCI; i <= MAX_DLCI; i++) {
		    if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
			continue;
		    if (!fr_pvc->fr_list)
			continue;
		    if (fr_pvc->idb != swidb)
			continue;
		    if (fr_pvc->route_flag)
			continue;
		    nv_write(TRUE, "%s %d %d", csb->nv_command,
			     fr_pvc->fr_list, i);
		}
	    }
	    break;
	case FR_DLCI_GROUP:
	    {
		int i;
		
		for (i = MIN_DLCI; i <= MAX_DLCI; i++) {
		    if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
			continue;
		    if (!fr_pvc->fr_pr_dlci_group)
			continue;
		    if (fr_pvc->idb != csb->interface)
			continue;
		    if (fr_pvc->route_flag)
			continue;
		    nv_write(TRUE, "%s %d %d %d %d %d", csb->nv_command,
			     fr_pvc->fr_pr_dlci_group,
			     fr_pvc->fr_pr_dlcis[PRIORITY_HIGH],
			     fr_pvc->fr_pr_dlcis[PRIORITY_MEDIUM],
			     fr_pvc->fr_pr_dlcis[PRIORITY_NORMAL],
			     fr_pvc->fr_pr_dlcis[PRIORITY_LOW]);
		}
	    }
	    break;
	  case FRAME_RELAY_DLCI_BITS:
	    nv_write((fr_idb->fr_dlci_bits !=
		      FRAME_RELAY_DEFAULT_DLCI_BITS) &&
		     (GETOBJ(int,1) == fr_idb->fr_dlci_bits),
		     "%s", csb->nv_command,
		     fr_idb->fr_dlci_bits);
	    break;
	  case FRAME_RELAY_LMI_TYPE:
	    if ((!idb->nokeepalive) &&
		(lmi_idb->fr_lmi_type_configured)) {
		nv_write(TRUE, csb->nv_command);
		switch (lmi_idb->fr_lmi_type) {
	        case FR_ANSI_D:
		    nv_add(TRUE, " ansi");
		    break;
	        case FR_CCITT_LMI:
		    nv_add(TRUE, " q933a");
		    break;
	        case FR_CISCO_LMI:
		    nv_add(TRUE, " cisco");
		    break;
		default:
		    break;
		}
	    }
	    break;
	  case FRAME_RELAY_LOCAL_DLCI:
	    if (fr_idb->fr_multicast_status != FR_MULTICAST_DYNAMIC) {
		nv_write(fr_idb->fr_local_dlci !=
			 FR_UNDEFINED_DLCI,
			 "%s %d", csb->nv_command,
			 DLCI_TO_NUM(fr_idb->fr_local_dlci));
	    }
	    break;
	  case FRAME_RELAY_KEEPALIVE:
	    /*
	     * write only keepalive and not fr keepalive
	     */
/*
	    if (! idb->nokeepalive) {
		if (fr_idb->fr_intf_type != FR_DTE)
		    nv_write(idb->keep_period != KEEPALIVE_DEFAULT,
			     "%s %d", csb->nv_command, idb->keep_period);
	    }
*/
	    break;
	case FR_DLCI:
	    {
		int i;
		frmaptype *p;
		char	*str1, *str2;
		char    mapaddr[64];
		
		for (i=0; i<RELAYHASH; i++) {
		    for (p = relaytable[i]; p; p = p->next) {
			if (!(fr_pvc = fr_idb->fr_dlci_tab[DLCI_TO_NUM(p->dlci)]))
			    continue;
		    	if (fr_pvc->pvc_type == SVC_DYNAMIC)
			    continue;
			if (((p->map_type == FR_MAP_INT_DLCI) ||
				(p->map_type == FR_MAP_INT_DLCI_SVC)) 
				&& (swidb == p->idb) &&
			    is_p2p(swidb)) {
			    str1 = "";
			    if ( p->encaps_type == FR_ENCAPS_IETF )
				str1 = " IETF";
			    if  ( p->encaps_type == FR_ENCAPS_CISCO )
				str1 = " CISCO";
			    str2 = " protocol";
			    print_proto_addr(swidb, &p->p2p_proto_addr, 
							p->p2p_link, mapaddr);
			    nv_write(TRUE, "%s %d%s%s %s", csb->nv_command,
				     DLCI_TO_NUM(p->dlci), str1, 
				     p->p2p_link ? str2 : " ", p->p2p_link ? mapaddr : " ");
			    if (fr_pvc->pvc_class &&
				fr_pvc->pvc_inht == FR_VC_CLASS) {
				nv_write(TRUE, " class %s",
					 fr_pvc->pvc_class->name);
			    }
			}
		    }
		}
		for (i = MIN_DLCI; i < MAX_DLCI; i++) {
		    if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
			continue;
		    if (fr_pvc->pvc_type == SVC_DYNAMIC)
			continue;
		    if (!(swidb == fr_pvc->idb))
			continue;
		    if (fr_pvc->fr_if_set) {
			nv_write(TRUE, "%s %d", csb->nv_command,i);
			if (fr_pvc->pvc_class &&
			    fr_pvc->pvc_inht == FR_VC_CLASS) {
			    nv_write(TRUE, " class %s",
				     fr_pvc->pvc_class->name);
			}
		    }
		}
	    }
	    break; 
	  case FRAME_RELAY_MAP:
	    {
	      int i;
	      frmaptype *p;
	      char    *str1,*str2, *str3, *str4, *str5;
	      char    mapaddr[64];
	      
	      for (i=0; i<RELAYHASH; i++) {
		for (p = relaytable[i]; p; p = p->next) {
		  if ((p->map_type == FR_MAP_STATIC) && (swidb == p->idb)) {
		    str1 = (p->broadcast ? " broadcast" : "");
		    str2 = "";
		    if ( p->encaps_type == FR_ENCAPS_IETF )
		      str2 = " IETF";
		    if  ( p->encaps_type == FR_ENCAPS_CISCO )
		      str2 = " CISCO";
		    str3 = "";
		    str4 = "";
		    str5 = "";
		    if (p->link == LINK_COMPRESSED_TCP)
		      str5 = " tcp header-compression";
		    if ((p->compression_flags & FR_MAP_COMP_PAYLOAD_STAC) &&
			(idb->comp_db && idb->comp_db->comp_configed == COMP_STAC) ) {
		      str3 = " payload-compression";
		      if ( p->encaps_type == FR_ENCAPS_CISCO2 )
			str4 = " packet-by-packet";
		    }
		    if ((p->compression_flags & FR_MAP_PASSIVE) &&
			(p->link == LINK_COMPRESSED_TCP))
		      str4 = " passive";
		    if (p->compression_flags & FR_MAP_ACTIVE)
		      str4 = " active";
		    if (p->compression_flags & FR_MAP_NO_COMPRESS)
                       if (p->encaps_type==FR_ENCAPS_IETF)
		           str3="";
		       else
		           str3 = " nocompress";
		    if (p->link == LINK_COMPRESSED_TCP)
		      print_proto_addr(swidb, &p->proto_addr,
				       LINK_IP, mapaddr);
		    else
		      print_proto_addr(swidb, &p->proto_addr,
				       p->link, mapaddr);
		    nv_write(TRUE, "%s %s %d%s%s%s%s%s", csb->nv_command,
			     mapaddr, DLCI_TO_NUM(p->dlci),
			     str1, str2, str3, str5, str4);
		  }
		}
	      }
	    }
	    break;
	  case FRAME_RELAY_MULTICAST:
	    if  (fr_idb->fr_multicast_status != FR_MULTICAST_DYNAMIC) {
		nv_write(fr_idb->fr_multicast_dlci !=
			 NUM_TO_DLCI(FRAME_RELAY_DEFAULT_MULTICAST_DLCI),
			 "%s %d", csb->nv_command,
			 DLCI_TO_NUM(fr_idb->fr_multicast_dlci));
	    }
	    break;
	  case FRAME_RELAY_ZAP:
	    /* No NV gen for this command */
	    break;
	  case FRAME_RELAY_INTF :
	    nv_write(fr_idb->fr_intf_type != FR_DTE, "%s", csb->nv_command);
	    switch (fr_idb->fr_intf_type) {
	        case FR_DCE:
		    nv_add(TRUE, " dce");
		    break;
	        case FR_NNI:
		    nv_add(TRUE, " nni");
		    break;
		default:
		    break;
	    }
	    break;
	  case FRAME_RELAY_ROUTE :
	    nv_fr_route(swidb, csb);
	    break;
	  case FR_LMI_N391DTE :
            if (!idb->nokeepalive) {
                if (fr_idb->fr_intf_type != FR_DCE)
	    	    nv_write(lmi_idb->fr_lmi_n391dte != DEFAULT_LMI_N391DTE,
			 "%s %d", csb->nv_command, lmi_idb->fr_lmi_n391dte);
	    }
	    break;
	  case FR_LMI_N392DTE :
            if (!idb->nokeepalive) {
                if (fr_idb->fr_intf_type != FR_DCE)
	    	    nv_write(lmi_idb->fr_lmi_n392dte != DEFAULT_LMI_N392DTE,
			 "%s %d", csb->nv_command, lmi_idb->fr_lmi_n392dte);
	    }
	    break;
	  case FR_LMI_N393DTE :
            if (!idb->nokeepalive) {
                if (fr_idb->fr_intf_type != FR_DCE)
	    	    nv_write(lmi_idb->fr_lmi_n393dte != DEFAULT_LMI_N393DTE,
			 "%s %d", csb->nv_command, lmi_idb->fr_lmi_n393dte);
	    }
	    break;
	  case FR_LMI_T392DCE :
            if (!idb->nokeepalive) {
	        if (fr_idb->fr_intf_type != FR_DTE)
	            nv_write(lmi_idb->fr_lmi_t392dce != DEFAULT_LMI_T392DCE,
			 "%s %d", csb->nv_command, lmi_idb->fr_lmi_t392dce);
	    }
	    break;
          case FR_LMI_N392DCE :
            if (!idb->nokeepalive) {
                if (fr_idb->fr_intf_type != FR_DTE)
            	    nv_write(lmi_idb->fr_lmi_n392dce != DEFAULT_LMI_N392DCE,
                         "%s %d", csb->nv_command, lmi_idb->fr_lmi_n392dce);
	    }
            break;
          case FR_LMI_N393DCE :
            if (!idb->nokeepalive) {
	      if (fr_idb->fr_intf_type != FR_DTE)
		nv_write(lmi_idb->fr_lmi_n393dce != DEFAULT_LMI_N393DCE,
                         "%s %d", csb->nv_command, lmi_idb->fr_lmi_n393dce);
	    }
	    break;
	  case FR_STAC_COMPRESS:
	    {
	      frmaptype *p;
	      if (idb->comp_db != NULL 
		  && idb->comp_db->comp_configed != COMP_NOT_ENABLED
		  && is_p2p(swidb) 
		  && ( p = findMapEntry( csb->interface ) )
		  && p->compression_flags & FR_MAP_COMP_PAYLOAD_STAC ) {
		nv_write(TRUE, "%s", csb->nv_command);
		nv_add(TRUE, " packet-by-packet");
	      }
	    }
	    break;
	  case FRAME_RELAY_ENQUEUE:
	      break;
	  case FR_SVC_IDLE_TIMER_SET: 
		break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
    /* 
     * A hidden command to be used for clearing SVCs gracefully. 
     * This is only for tesing purposes. Used to changed idle timer 
     * values after the call has been set up. 
     */
    case FR_SVC_IDLE_TIMER_SET:
    {
	ushort dlci;
	uint timer;

	dlci = GETOBJ(int,1);
	timer = GETOBJ(int,2);
	if (dlci && (fr_pvc = fr_idb->fr_dlci_tab[dlci])) {
	    if (fr_pvc->pvc_type == SVC_DYNAMIC) {
		fr_pvc->svc_idle_timer = timer * ONESEC;
	    	TIMER_STOP(fr_pvc->t_last_activity);
	    	TIMER_START(fr_pvc->t_last_activity, fr_pvc->svc_idle_timer);
	    	GET_TIMESTAMP(fr_pvc->svc_reset_time);
	    }
	}
    }
    break;
	
	case FR_TCP_COMPRESS:
		fr_tcpip_header_compression_command(csb);
		break;
    case FR_DEGROUP:
	{
	    ushort group,dlci;

	    dlci = GETOBJ(int,3);
	    group = GETOBJ(int,2);

            fr_pvc = fr_idb->fr_dlci_tab[dlci];

	    if (csb->sense) {
		if (!fr_pvc) {
		    fr_pvc = fr_pvc_init(swidb, dlci, PVC_NOTSET, PVC_STATIC,
					 FALSE, NULL);
		    if (!fr_pvc) {
			return;
		    }
		} else {
		    fr_pvc->fr_list = group;
		    reg_invoke_bump_cache_version(idb, NULL, FALSE);
		}
		fr_pvc->fr_list = group;
	    } else {
		if (!fr_pvc) {
		    return;
		}
		fr_pvc->fr_list = 0;
		fr_pvc_clear(idb, dlci);
	    }
	}
	break;
	case FR_DLCI_GROUP:
		{
		    ushort dlci_high = GETOBJ(int,1);
		    ushort dlci_medium = GETOBJ(int,2);
		    ushort dlci_normal = GETOBJ(int,3);
		    ushort dlci_low = GETOBJ(int,4);
		    ushort group = GETOBJ(int,5);

		    fr_pvc = fr_idb->fr_dlci_tab[dlci_high];
		    if (!csb->sense) {
			if (!fr_pvc)
			    return;
			fr_dlci_group_clear(fr_idb, fr_pvc);
		    } else {
			if (!fr_pvc) {
			    fr_pvc = fr_pvc_init(csb->interface, dlci_high,
					PVC_LOCAL,PVC_STATIC, FALSE, NULL);
			    if (fr_pvc) {
				fr_idb->map_dynamic[dlci_high] = FR_NOT_DYNAMIC;
				fr_set_pvc_status(fr_pvc, DLCI_ADDED);
			    }
			} else {
			    fr_pvc->idb = swidb;
			    fr_pvc->pvc_usage = PVC_LOCAL;
 			    fr_pvc->pvc_type = PVC_STATIC;
			    fr_set_pvc_status(fr_pvc, DLCI_ADDED);
			}
			fr_pvc->fr_pr_dlci_group = group;
			fr_pvc->fr_pr_dlcis[PRIORITY_MEDIUM] = dlci_medium;
			fr_pvc->fr_pr_dlcis[PRIORITY_NORMAL] = dlci_normal;
			fr_pvc->fr_pr_dlcis[PRIORITY_LOW] = dlci_low;
			fr_pr_init_dlci_group(fr_idb,fr_pvc);
			reg_invoke_bump_cache_version(idb, NULL, TRUE); 
		    }
		}
		break;
      case FRAME_RELAY_DLCI_BITS:
	if (csb->sense) {
	    fr_idb->fr_dlci_bits = GETOBJ(int,1);
	} else {
	    fr_idb->fr_dlci_bits = FRAME_RELAY_DEFAULT_DLCI_BITS;
	}
	switch (GETOBJ(int,1)) {
	  case 10:
	    tmp_mc = DLCI_TO_NUM(fr_idb->fr_multicast_dlci);
	    tmp_lcl = DLCI_TO_NUM(fr_idb->fr_local_dlci);
	    tmp_lmi = DLCI_TO_NUM(lmi_idb->fr_lmi_dlci);
	    fr_idb->fr_lsb_to_dlci_mask = 0x0f;
	    fr_idb->fr_lsb_to_num_mask = 0xf0;
	    fr_idb->fr_lsb_dlci_shift = 4;
	    fr_idb->fr_msb_to_dlci_mask = 0x03f0;
	    fr_idb->fr_msb_to_num_mask = 0xfc00;
	    fr_idb->fr_msb_dlci_shift = 6;
	    fr_idb->fr_local_dlci = NUM_TO_DLCI(tmp_lcl);
	    fr_idb->fr_multicast_dlci = NUM_TO_DLCI(tmp_mc);
	    lmi_idb->fr_lmi_dlci = NUM_TO_DLCI(tmp_lmi);
	    break;
	  case 11:
	    tmp_mc = DLCI_TO_NUM(fr_idb->fr_multicast_dlci);
	    tmp_lcl = DLCI_TO_NUM(fr_idb->fr_local_dlci);
	    tmp_lmi = DLCI_TO_NUM(lmi_idb->fr_lmi_dlci);
	    fr_idb->fr_lsb_to_dlci_mask = 0x1f;
	    fr_idb->fr_lsb_to_num_mask = 0xf8;
	    fr_idb->fr_lsb_dlci_shift = 3;
	    fr_idb->fr_msb_to_dlci_mask = 0x07e0;
	    fr_idb->fr_msb_to_num_mask = 0xfc00;
	    fr_idb->fr_msb_dlci_shift = 5;
	    fr_idb->fr_local_dlci = NUM_TO_DLCI(tmp_lcl);
	    fr_idb->fr_multicast_dlci = NUM_TO_DLCI(tmp_mc);
	    lmi_idb->fr_lmi_dlci = NUM_TO_DLCI(tmp_lmi);
	    break;
	  case 13:
	    tmp_mc = DLCI_TO_NUM(fr_idb->fr_multicast_dlci);
	    tmp_lcl = DLCI_TO_NUM(fr_idb->fr_local_dlci);
	    tmp_lmi = DLCI_TO_NUM(lmi_idb->fr_lmi_dlci);
	    fr_idb->fr_lsb_to_dlci_mask = 0x7f;
	    fr_idb->fr_lsb_to_num_mask = 0xfe;
	    fr_idb->fr_lsb_dlci_shift = 1;
	    fr_idb->fr_msb_to_dlci_mask = 0x1f80;
	    fr_idb->fr_msb_to_num_mask = 0xfc00;
	    fr_idb->fr_msb_dlci_shift = 3;
	    fr_idb->fr_local_dlci = NUM_TO_DLCI(tmp_lcl);
	    fr_idb->fr_multicast_dlci = NUM_TO_DLCI(tmp_mc);
	    lmi_idb->fr_lmi_dlci = NUM_TO_DLCI(tmp_lmi);
	    break;
	}
	break;
      case FRAME_RELAY_LMI_TYPE:
	if (!csb->sense) 
	    fr_lmi_set(idb,fr_idb,FR_CISCO_LMI,FALSE, FALSE);
	else
	    fr_lmi_set(idb,fr_idb,GETOBJ(int,1),TRUE, TRUE);
	break;
      case FRAME_RELAY_LOCAL_DLCI:
	if (csb->sense) {
	    ushort local_dlci;
	    ushort inarp_status;
	    
	    local_dlci = GETOBJ(int,1);
	    fr_idb->fr_local_dlci = NUM_TO_DLCI(GETOBJ(int,1));
	    fr_pvc = fr_idb->fr_dlci_tab[local_dlci];
	    if ( ! fr_pvc ) {
		fr_pvc = fr_pvc_init(swidb,local_dlci,PVC_LOCAL,PVC_STATIC,
				     FALSE, NULL);
		if (!fr_pvc)
		    break;
	    }
	    inarp_status = ~FR_ALL_IN_ARP_ENABLED;
	    reg_invoke_fr_inarp_dlci(FR_INARP_DLCI_SET, fr_idb, local_dlci,
				     &inarp_status);
	} else {
	    fr_pvc_clear(idb, DLCI_TO_NUM(fr_idb->fr_local_dlci));
	    fr_idb->fr_local_dlci =
		FR_UNDEFINED_DLCI;
	}
	break;
      case FRAME_RELAY_KEEPALIVE:
        idb->keep_period = GETOBJ(int,1);
        if (idb->keep_period < idb->keep_count)
            idb->keep_count = idb->keep_period;
        idb->nokeepalive = !csb->sense;
        if (idb->nokeepalive) {
            if (!fr_idb->fr_lineup && idb->state == IDBS_UP) { 
                fr_idb->fr_lineup = TRUE;
		idb->reason = "LMI disabled";
		reg_invoke_Lapf_sys_event(FR_MDL_ASSIGN, fr_idb);
		reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
	    }
            fr_reset_lmi_events(fr_idb, FR_INTF_DTE, FR_LMI_PASS,
                                    0, lmi_idb->fr_lmi_n393dte);
            fr_reset_lmi_events(fr_idb, FR_INTF_DCE, FR_LMI_PASS,
                                    0, lmi_idb->fr_lmi_n393dce);
        }
	break;
	    case FR_DLCI:
		{
		    ushort dlci = GETOBJ(int,1);
		    
		    if (csb->sense) {
			if (is_p2p(swidb))
			    fr_parse_p2p_dlci(swidb, dlci, 
				TRUE, GETOBJ(int,4), GETOBJ(int,10),
				NULL, *GETOBJ(paddr,1));
			else
			    fr_parse_multi_dlci(swidb, dlci);
			csb->mode = fr_dlci_mode;
			csb->line1 = dlci;
		    } else
			fr_parse_no_if_dlci(swidb, dlci);
		    if (csb->flags & CONFIG_HTTP) {
			char *new_uri = string_getnext();
			/*	
			 * char *old_uri = csb->http_state->uri;
			 */
			/* Setup new URI */
			sprintf(new_uri, "/%s/%s/%s", get_mode_name(csb->mode),
				encstring(idb->enctype), idb->hw_namestring);
			
			set_mode_byname(&csb->mode, "fr-dlci", MODE_VERBOSE);
		    }
		}
		break;
      case FRAME_RELAY_MAP:
      {
	int result;
	ushort dlci = GETOBJ(int,1);
	int pvc_usage = PVC_LOCAL;
	fr_pvc = fr_idb->fr_dlci_tab[dlci];
	if (fr_pvc) {
	    pvc_usage = fr_pvc->pvc_usage;
	}
	if ( is_p2p(swidb) ) {
	    printf("\nFRAME-RELAY INTERFACE-DLCI command should be used on "
		   "point-to-point interfaces");
	    break;
	}
	result = frame_relay_parse_map(csb, swidb);
	}
	break;
	
      case FRAME_RELAY_MULTICAST:
	if (! csb->sense) {
	    fr_mc_delete_dlci(swidb->hwptr,
			      DLCI_TO_NUM(fr_idb->fr_multicast_dlci));
	} else 
	    fr_add_mc_dlci(swidb->hwptr, FR_MULTICAST_STATIC, GETOBJ(int,1));
	break;
      case FRAME_RELAY_ZAP:
	if (csb->sense) {
	    fr_clear_map(swidb);
	}
	break;
      case FRAME_RELAY_INTF:
      {
	int i;
        if (!csb->sense || (GETOBJ(int, 1) == FR_DTE)) {
            fr_idb->fr_intf_type = FR_DTE;
            fr_lmi_timer_delete(idb, LMI_ERR_TIMER);
	    fr_lmi_timer_delete(idb, LMI_ROUTE_TIMER);
            fr_reset_lmi_events(fr_idb, FR_INTF_DCE, FR_LMI_PASS,
                                0, lmi_idb->fr_lmi_n393dce);
	    fr_lmi_timer_create(idb, LMI_IDB_TIMER);

        } else if (!fr_switching) {
            printf ("\nMust enable frame-relay switching to configure DCE/NNI");

        } else {
	    if (fr_idb->fr_intf_type == GETOBJ(int, 1))
		break;		/* no change */
	    /*
	     * changing interface type to be either DCE or NNI
	     */
            fr_idb->fr_intf_type = GETOBJ(int, 1);
            /*
             * DCE and NNI interfaces need timers, start them here
             * route_status_timer is used to update pvc_status from
             * the other side's nni_pvc_status
             */
            if (!(fr_lmi_timer_create(idb, LMI_ROUTE_TIMER)
		  && fr_lmi_timer_create(idb, LMI_ERR_TIMER))) {
		printf("\n%% error on LMI timer allocation");
	    }
            if (fr_idb->fr_intf_type == FR_DCE) {
		fr_lmi_timer_delete(idb, LMI_IDB_TIMER);
                /*
                 * Set A bit to 1 in nni_pvc_status as DCE never
                 * receives LMI status message
                 */
                for (i = MIN_DLCI; i < MAX_DLCI; i++)
                    if ((fr_pvc = fr_idb->fr_dlci_tab[i]))
                        SET_PVC_ACTIVE(fr_pvc->nni_pvc_status);

                fr_reset_lmi_events(fr_idb, FR_INTF_DTE, FR_LMI_PASS,
				    0, lmi_idb->fr_lmi_n393dte);
	    } else {
		/* For NNI make sure the LMI_IDB_TIMER is running. */
		if (lmi_idb->idb_timer == NULL)
		    fr_lmi_timer_create(idb, LMI_IDB_TIMER);
	    }
	    if (csb->sense) {
		fr_resync_pvc_count(fr_idb, idb);   /* re-calculate #pvc */
	    }
	  }
        }
	break;

      case FRAME_RELAY_ENQUEUE:
      /* 
       * Purely for the initial release of SVCs. Used to queue messages
       * on this router, not to send them out!!! Only for testing 
       * purposes. 
       */
      {
      	fr_svc_info_type   *svc;
      	uchar           msg_type;
      	fr_params_type  *fr_params;
	struct fr_svc_params *svc_params;
      	paktype         *pak;
      	uchar           *cptr;

	svc = malloc_named(sizeof(fr_svc_info_type), "FR SVC Descriptor");
	fr_params = malloc_named(sizeof(fr_params_type), "FR Parameters");
	svc_params = malloc_named(sizeof(struct fr_svc_params), "FR SVC Parameters");
	
	if (!svc || !fr_params || !svc_params)
	    return;

	switch (GETOBJ(int,1)) {
	    case 1:
		msg_type = MSG_SETUP;
		svc_params->msg_type = msg_type;
		svc_params->call_ref = GETOBJ(int,3);
		svc_params->dlci = GETOBJ(int,2);
		pak = getbuffer(sizeof(svc_setup_msg));
		if (!pak)
		    return;
		pak->datagramstart = pak->network_start;
		pak->datagramsize = sizeof(svc_setup_msg);
		/* 3 = 2 byte dlci header + 1 byte numbered frame ID */
      		pak->if_input = swidb;
      		PUTSHORT(pak->datagramstart, (0 | FRAME_RELAY_EA_BITS));
      		cptr = (uchar *)(pak->datagramstart);
      		reg_invoke_form_setup_msg(svc, &cptr, svc_params);
		process_enqueue_pak(iframeQ, pak);
      		break;
      	    case 2:
      		msg_type = MSG_CALL_PROCEEDING;
      		svc_params->msg_type = msg_type;
		svc_params->call_ref = GETOBJ(int,3);
		svc_params->dlci = GETOBJ(int,2);
      		pak = getbuffer(sizeof(svc_call_proc_msg));
      		if (!pak)
		    return;
      		pak->datagramstart = pak->network_start;
      		pak->datagramsize = sizeof(svc_call_proc_msg);
      		pak->if_input = swidb;
		PUTSHORT(pak->datagramstart, (0 | FRAME_RELAY_EA_BITS));
		cptr = (uchar *)(pak->datagramstart);
		reg_invoke_form_call_proc_msg(svc, &cptr, svc_params);
		process_enqueue_pak(iframeQ, pak);
		break;
	    case 3:
		msg_type = MSG_CONNECT;
		svc_params->msg_type = msg_type;
		svc_params->call_ref = GETOBJ(int,3);
		svc_params->dlci = GETOBJ(int,2);
		pak = getbuffer(sizeof(svc_connect_msg));
		if (!pak)
		    return;
		pak->datagramstart = pak->network_start;
		pak->datagramsize = sizeof(svc_connect_msg);
		pak->if_input = swidb;
		PUTSHORT(pak->datagramstart, (0 | FRAME_RELAY_EA_BITS));
		cptr = (uchar *)(pak->datagramstart);
		reg_invoke_form_connect_msg(svc, &cptr, svc_params);
		process_enqueue_pak(iframeQ, pak);
		break;
	    case 4:
		msg_type = MSG_DISCONNECT;
		svc_params->msg_type = msg_type;
		svc_params->call_ref = GETOBJ(int,3);
		svc_params->dlci = GETOBJ(int,2);
		pak = getbuffer(sizeof(svc_disconnect_msg));
		if (!pak)
		    return;
		pak->datagramstart = pak->network_start;
		pak->datagramsize = sizeof(svc_disconnect_msg);
		pak->if_input = swidb;
		PUTSHORT(pak->datagramstart, (0 | FRAME_RELAY_EA_BITS));
		cptr = (uchar *)(pak->datagramstart);
		reg_invoke_form_disc_msg(svc, &cptr , svc_params);
		process_enqueue_pak(iframeQ, pak);
		break;
	    case 5:
		msg_type = MSG_RELEASE;
		svc_params->msg_type = msg_type;
		svc_params->call_ref = GETOBJ(int,3);
		svc_params->dlci = GETOBJ(int,2);
		pak = getbuffer(sizeof(svc_release_msg));
		if (!pak)
		    return;
		pak->datagramstart = pak->network_start;
		pak->datagramsize = sizeof(svc_release_msg);
		pak->if_input = swidb;
		PUTSHORT(pak->datagramstart, (0 | FRAME_RELAY_EA_BITS));
		cptr = (uchar *)(pak->datagramstart);
		reg_invoke_form_release_msg(svc, &cptr, svc_params);
		process_enqueue_pak(iframeQ, pak);
		break;
	    case 6:
		msg_type = MSG_RELEASE_COMPLETE;
		svc_params->msg_type = msg_type;
		svc_params->call_ref = GETOBJ(int,3);
		svc_params->dlci = GETOBJ(int,2);
		pak = getbuffer(sizeof(svc_release_msg));
		if (!pak)
		    return;
		pak->datagramstart = pak->network_start;
		pak->datagramsize = sizeof(svc_release_msg);
		pak->if_input = swidb;
		PUTSHORT(pak->datagramstart, (0 | FRAME_RELAY_EA_BITS));
		cptr = (uchar *)(pak->datagramstart);
		reg_invoke_form_release_comp_msg(svc, &cptr, svc_params);
		process_enqueue_pak(iframeQ, pak);
		break;
	    case 7:
		msg_type = MSG_STATUS_ENQUIRY;
		svc_params->msg_type = msg_type;
		svc_params->call_ref = GETOBJ(int,3);
		svc_params->dlci = GETOBJ(int,2);
		pak = getbuffer(sizeof(svc_status_enq));
		if (!pak)
		    return;
		pak->datagramstart = pak->network_start;
		pak->datagramsize = sizeof(svc_status_enq);
		pak->if_input = swidb;
		PUTSHORT(pak->datagramstart, (0 | FRAME_RELAY_EA_BITS));
		cptr = (uchar *)(pak->datagramstart);
		reg_invoke_form_status_enq_msg(svc, &cptr, svc_params);
		process_enqueue_pak(iframeQ, pak);
		break;
	    case 8:
		msg_type = MSG_STATUS;
		svc_params->msg_type = msg_type;
		svc_params->call_ref = GETOBJ(int,3);
		svc_params->dlci = GETOBJ(int,2);
		pak = getbuffer(sizeof(svc_status_msg));
		if (!pak)
		    return;
		pak->datagramstart = pak->network_start;
		pak->datagramsize = sizeof(svc_status_msg);
		pak->if_input = swidb;
		PUTSHORT(pak->datagramstart, (0 | FRAME_RELAY_EA_BITS));
		cptr = (uchar *)(pak->datagramstart);
		reg_invoke_form_status_msg(svc, &cptr, svc_params);
		process_enqueue_pak(iframeQ, pak);
		break;
	    default:
		break;
	}
	}
	break;
      case FRAME_RELAY_ROUTE:
	parse_fr_route(csb, swidb);
	break;
      case FR_LMI_N391DTE:
        if (fr_idb->fr_intf_type == FR_DCE) {
            printf("\nCan't set DTE values for a FR DCE");
            return;
        }
        if (!csb->sense)
            lmi_idb->fr_lmi_n391dte = DEFAULT_LMI_N391DTE;
        else
	    lmi_idb->fr_lmi_n391dte = GETOBJ(int,1);
        break;
      case FR_LMI_N392DTE:
        if (fr_idb->fr_intf_type == FR_DCE) {
            printf("\nCan't set DTE values for a FR DCE");
            return;
        }
        if (!csb->sense)
            lmi_idb->fr_lmi_n392dte = DEFAULT_LMI_N392DTE;
	else
            lmi_idb->fr_lmi_n392dte = GETOBJ(int,1);
        break;            
      case FR_LMI_N393DTE: {
	ushort new_n393;

        if (fr_idb->fr_intf_type == FR_DCE) {
            printf("\nCan't set DTE values for a FR DCE");
            return;
        }
        if (!csb->sense)
            new_n393 = DEFAULT_LMI_N393DTE;
	else
            new_n393 = GETOBJ(int,1);

	/*
	 * If n393 has increased extend delay line by bumping up lmi_new
	 * pointer and adding PASS events
         */
        fr_update_n393(fr_idb, new_n393, &(lmi_idb->fr_lmi_n393dte),FR_INTF_DTE);
        break;            
      }
      case FRAME_RELAY_SVC_DIAG_SETUP:
	break;
      case FR_STAC_COMPRESS:
	fr_payload_compression_command(csb, NULL);
	break;
      case FR_LMI_T392DCE:
	    {
	    ulong interval;
	if (fr_idb->fr_intf_type == FR_DTE) {
	    printf("\nCan't set DCE/NNI values for a FR DTE");
	    return;
	}
	if (!csb->sense)
	    lmi_idb->fr_lmi_t392dce = DEFAULT_LMI_T392DCE;
	else
	    lmi_idb->fr_lmi_t392dce = GETOBJ(int,1);

        interval =  lmi_idb->fr_lmi_t392dce*ONESEC;
	reg_invoke_fr_mgd_timer_interval(FR_LMI_TIMER, idb,
					 &interval,
					 LMI_ERR_TIMER);
	}
        break;            
      case FR_LMI_N392DCE:
        if (fr_idb->fr_intf_type == FR_DTE) {
            printf("\nCan't set DCE/NNI values for a FR DTE");
            return;
        }
        if (!csb->sense)
            lmi_idb->fr_lmi_n392dce = DEFAULT_LMI_N392DCE;
        else
            lmi_idb->fr_lmi_n392dce = GETOBJ(int,1);
        break;
      case FR_LMI_N393DCE: {
        ushort new_n393;
 
        if (fr_idb->fr_intf_type == FR_DTE) {
            printf("\nCan't set DCE/NNI values for a FR DTE");
            return;
        }
        if (!csb->sense)
            new_n393 = DEFAULT_LMI_N393DCE;
        else
            new_n393 = GETOBJ(int,1);
	fr_update_n393(fr_idb, new_n393, &(lmi_idb->fr_lmi_n393dce),FR_INTF_DCE);
        break;
      }

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * fr_pvc_init
 * Create and initialize a PVC structure
 */
fr_pvc_info_type *fr_pvc_init (
			       idbtype *idb,
			       ushort dlci,
			       enum PVC_USAGE_TYPE usage,
			       enum PVC_TYPE pvc_type,
			       boolean map_flag, 
			       mapclass_type *mapclass)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc = NULL;
    struct fr_svc_params *svc_params;
    ushort status;

    fr_idb =  idb->hwptr->frame_relay_stuff;
    fr_pvc = fr_idb->fr_dlci_tab[dlci];
    if (!fr_pvc) {
	if ((fr_pvc = create_fr_pvc())) {
	    fr_pvc->fr_pr_enabled = FALSE;
	    fr_pvc->pvc_congestion = NOT_CONGESTED;
	    fr_pvc->pvc_usage = usage;
	    fr_pvc->pvc_type = pvc_type;
	    fr_pvc->route_flag = FALSE;
	    fr_pvc->map_flag = map_flag;
	    fr_pvc->idb = idb;	/* MIB and subinterfaces */
	    fr_pvc->this_dlci = dlci;
	    fr_pvc->convert_dlci = NUM_TO_DLCI(dlci) | FRAME_RELAY_EA_BITS; 
	    fr_pvc->use_count = 0;
	    fr_pvc_enqueue(&fr_idb->fr_pvcQ,fr_pvc);
	    if (fr_idb->fr_traffic_shape)
		reg_invoke_fr_dlci_create(fr_pvc->idb, fr_pvc, SHAPE_CREATE);
	    fr_pvc->svc_idle_timer = FR_SVC_IDLE_TIMER; 
	    if ((pvc_type == SVC_DYNAMIC) && mapclass && 
		(mapclass->params)) {
		svc_params = ((fr_params_type *)mapclass->params)->svc_params;
		fr_pvc->svc_idle_timer = svc_params->idle_timer * ONESEC;
	    } 
	    if (pvc_type == SVC_DYNAMIC) {
	    	TIMER_START(fr_pvc->t_last_activity, fr_pvc->svc_idle_timer);
	    	GET_TIMESTAMP(fr_pvc->svc_reset_time);
	    }

	    fr_idb->fr_dlci_tab[dlci] = fr_pvc;
    	    if (fr_pvc->pvc_type != SVC_DYNAMIC)
	    	fr_idb->num_idb_pvcs++;
	    if (is_p2p(idb)) {
		fr_set_p2pvc(idb, dlci);
	    }
	    reg_invoke_fr_dlci_class_inht(idb, dlci, mapclass,
					  ADD_CLASS, fr_pvc->pvc_inht);
	}
    }
    
    status = FR_ALL_IN_ARP_ENABLED;
    reg_invoke_fr_inarp_dlci(FR_INARP_DLCI_SET, fr_idb, dlci, &status);
    return(fr_pvc);
}

/*
 * fr_pvc_clear
 * Free the PVC struct and stats info
*/
void fr_pvc_clear (hwidbtype *idb, ushort dlci)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc = NULL;

    if (frame_relay_debug)
        buginf("\n%s:clearing %d", idb->hw_namestring, dlci);
    fr_idb =  idb->frame_relay_stuff;
    fr_pvc = fr_idb->fr_dlci_tab[dlci];
    if (fr_pvc) {
	if (fr_pvc->use_count) {
	    if (L3_DBG)
		buginf("\nfr_pvc_clear, usecount is %d, not cleared", fr_pvc->use_count);
	    return;
	}
	if (fr_pvc->tbridging_on_pvc) {
	    fr_delete_bridge_flood(fr_pvc);
	}
	
	/*
	 * VC queueing
	 */
	reg_invoke_fr_dlci_class_inht(fr_pvc->idb, dlci, fr_pvc->pvc_class,
				      REMOVE_CLASS, 0);
	/* 
	 * SVC is history. Stop the idle timer. 
	 */
	if (fr_pvc->pvc_type == SVC_DYNAMIC) {
	    TIMER_STOP(fr_pvc->t_last_activity);
	    fr_idb->fr_dlci_tab[dlci] = NULL;
	} else {
	    fr_idb->fr_dlci_tab[dlci] = NULL;
	    fr_dec_pvc_count(fr_idb, idb);  /* exclude svc in Status msg */
	}
	if (fr_idb->fr_traffic_shape)
	    reg_invoke_fr_dlci_destroy(fr_pvc->idb, fr_pvc, SHAPE_DELETE);
	fr_pvc_unqueue(&fr_idb->fr_pvcQ,fr_pvc);
	if (fr_pvc->stats_save)
	    free((frmib_pvc_stats_type *)fr_pvc->stats_save);
	if (is_p2p(fr_pvc->idb)) {
	    fr_clear_p2pvc(fr_pvc);
	}
	free(fr_pvc);
    }
}

/*
 * get_pri_fr_pvc_bridging
 * If fr_pvc is a member of a priority-dlic-group then we check
 * what priority bridging has on the priority list and returns
 * the pvc that corresponds to this priority.
 * If fr_pvc is not a member of a priority-dlci-group, then
 * fr_pvc itself is returned.
 */
 
fr_pvc_info_type *get_pri_fr_pvc_bridging (fr_pvc_info_type *fr_pvc,
                                         ushort dlci,
                                         fr_idb_struct_type *fr_idb)
{
    ushort priority_dlci;
    plisttype *plist;
    fr_pvc_info_type *tmp_fr_pvc;
 
    if ((!fr_pvc->fr_pr_dlci_group) ||
        (!parray[fr_pvc->fr_pr_dlci_group])) {
        return(fr_pvc);
    }
 
    plist = parray[fr_pvc->fr_pr_dlci_group]->plist;
    while (plist) {
        if (plist->linkcode == LINK_BRIDGE) {
            priority_dlci = fr_pvc->fr_pr_dlcis[plist->priority];
            if (dlci != priority_dlci) {
                tmp_fr_pvc = fr_idb->fr_dlci_tab[priority_dlci];
                if (!tmp_fr_pvc) {
                    return(fr_pvc);
                }
                return(tmp_fr_pvc);
            }
            return(fr_pvc);
        }
        plist = plist->next;
    }
    return(fr_pvc);
}

/*
 * frame_relay_addr_enter
 */
frmaptype *fr_addr_enter (
			  idbtype *idb, 
			  addrtype *proto_addr, 
			  ushort dlci, 
			  boolean broadcast, 
			  enum FR_ENCAPS_FORM encaps_ietf,
			  enum MAP_TYPE maptype, 
			  enum PVC_TYPE pvctype, 
			  uint link, 
			  uint options,
			  mapclass_type *mapclass)
{
    int hash;
    frmaptype *l;
    leveltype level;
    fr_pvc_info_type *fr_pvc;
    fr_idb_struct_type *fr_idb;
    addrtype *p2p_proto_addr = NULL;
    uint p2p_linktype = 0;

    if (is_p2p(idb) && (link != LINK_ILLEGAL)) {
	p2p_proto_addr = malloc_named(sizeof(addrtype), "FR Protocol Address");
	if (!p2p_proto_addr)
	    return (NULL);
	memcpy((uchar *)p2p_proto_addr, (uchar *)proto_addr, sizeof(addrtype));
	p2p_linktype = link;
	memset(proto_addr, 0, sizeof(addrtype));
	memcpy((uchar *)&proto_addr->addr, (uchar *)&idb, sizeof(idb));
	proto_addr->length = sizeof(idb); 
	link = LINK_ILLEGAL;
    }
    
    fr_idb = idb->hwptr->frame_relay_stuff;
    fr_pvc = fr_idb->fr_dlci_tab[dlci];
    /*
     * also we need to create a PVC for this if the entry is null
     */
    if (!fr_pvc) {
	fr_pvc = fr_pvc_init(idb,dlci,PVC_LOCAL,pvctype, TRUE, mapclass);
	if (!fr_pvc) {
	    if (p2p_proto_addr)
		free (p2p_proto_addr);
	    return(NULL);
	}
	fr_set_pvc_status(fr_pvc, DLCI_ADDED);
	fr_set_subif_state(idb, (int)FR_HWIF_STATE, fr_pvc->pvc_status);
	fr_idb->map_dynamic[dlci] = FR_NOT_DYNAMIC;
    } else {
	if (fr_pvc->route_flag) {
	    printf ("\nCan't use DLCI for map as route exists");
	    if (p2p_proto_addr)
		free (p2p_proto_addr);
	    return(NULL);
	}
 	if (pvctype != PVC_DYNAMIC) {
 	    /*
 	     * If the pvc is not created dynamic, we don't want to changed it
 	     * to dynamic, so it won't be removed later by clear dynamic
 	     */
 	    fr_pvc->pvc_type = pvctype;
 	}
	fr_pvc->pvc_usage = PVC_LOCAL;
	fr_pvc->idb = idb; 
    }
    fr_pvc->use_count++;
    fr_set_idbmap(fr_idb, dlci, maptype, link);
    if (link == LINK_BRIDGE
	|| ((link == LINK_ILLEGAL) &&
	    (is_p2p(idb)) &&
	    (idb->tbridge_on_swidb))) {
	fr_pvc = get_pri_fr_pvc_bridging (fr_pvc, dlci, fr_idb);
        fr_add_bridge_flood(idb, fr_pvc, encaps_ietf);
    }
    level = raise_interrupt_level(NETS_DISABLE);
    l = malloc_named(sizeof(frmaptype), "FR Map Class");
    if (l) {
	memset(&l->proto_addr, 0, sizeof(addrtype));
	memcpy(&l->proto_addr, proto_addr, sizeof(addrtype));
	l->idb = idb;
	l->broadcast = broadcast;
	l->map_type = maptype;
	if (maptype != FR_MAP_STATIC)
	    GET_TIMESTAMP(l->time);
	l->link = link;
	l->dlci = NUM_TO_DLCI(dlci);
	l->dlci_num = dlci;
	l->encaps_type = encaps_ietf;
	hash = xnethash((uchar *)&proto_addr->addr, proto_addr->length)
	    & (RELAYHASH-1);
	l->next = relaytable[hash];
	relaytable[hash] = l;
	l->tcp_compress = (struct thcdbtype_ *)NULL;
	l->compression_flags = options;
	if (is_p2p(idb) && p2p_linktype != LINK_ILLEGAL) {
	    l->p2p_link = p2p_linktype;
	    memset(&l->p2p_proto_addr, 0, sizeof(addrtype));
	    memcpy(&l->p2p_proto_addr, p2p_proto_addr, sizeof(addrtype));
	}
	if (COMPRESS_OVER_IP_LINK(l) ||
	    COMPRESS_OVER_COMPR_TCP_LINK(l) ||
	    COMPRESS_OVER_P2P_LINK(l)) {
	    l->tcp_compress=malloc_named(sizeof(thcdbtype), "FR TCP Compression");
	    if (l->tcp_compress) {
	    	thcdbtype *thcdb = (thcdbtype *)l->tcp_compress;
		reg_invoke_hdr_compress_init(LINK_IP, thcdb, 
					     DEFAULT_THC_STATES);

	    	if ((options & FR_MAP_PASSIVE) ||
		    (options & FR_MAP_INHERIT_PASSIVE))
		    thcdb->flags &= ~(THC_PERMACTIVE|THC_ACTIVE);
	    	else
		    thcdb->flags |= THC_PERMACTIVE|THC_ACTIVE;
	    } else {
		if (p2p_proto_addr)
		    free (p2p_proto_addr);
		relaytable[hash] = l->next;
		free(l);
		reset_interrupt_level(level);
		return (NULL);
	    }
	}
    }

    reset_interrupt_level(level);
    if (p2p_proto_addr)
	free (p2p_proto_addr);
    return(l);
}

/*
 * frame_relay_addr_lookup
 */

frmaptype *frame_relay_addr_lookup (
				    idbtype *idb,
				    addrtype *proto_addr,
				    register uint link)
{
    register frmaptype *l;
    leveltype level;

    if (is_p2p(idb)) {
	memset((uchar *)proto_addr, 0, sizeof(addrtype));
	memcpy((uchar *)&proto_addr->addr, (uchar *)&idb, sizeof(idb));
	proto_addr->length = sizeof(idb); 
    }
    /*
     * Handle sets of link types that should match an FR map
     * e.g., LINK_DECNET_ROUTER_L1 -> LINK_DECNET
     */
    switch (link) {
    case LINK_DECNET_ROUTER_L1:
    case LINK_DECNET_ROUTER_L2:
    case LINK_DECNET_PRIME_ROUTER:
    case LINK_DECNET_NODE:
	link = LINK_DECNET;
	break;
    default:
	break;
    }
    level = raise_interrupt_level(NETS_DISABLE);
    l = relaytable[xnethash((uchar *)&proto_addr->addr, proto_addr->length) &
		   (RELAYHASH-1)];
    while (l) {
  	if (((l->link == link) ||
	     (l->link == LINK_ILLEGAL) ||
	     (fr_valid_link_linktype_combo(link, l->link))) &&
	    (l->idb == idb) &&
	    ((bcmp((uchar *)&l->proto_addr, (uchar *)proto_addr,
		   sizeof(addrtype)) == 0) ||
	     (link == LINK_ILLEGAL))) {
	    reset_interrupt_level(level);
	    return(l);
	}
	l = l->next;
    }
    reset_interrupt_level(level);
    return(NULL);
}

/*
 * frame_relay_map_delete
 */

void frame_relay_map_delete (register frmaptype *hp)
{
    register uint hash;
    register frmaptype **map;
    leveltype level;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    boolean map_err = TRUE;
    
    level = raise_interrupt_level(NETS_DISABLE);
    /*
     * Find and delink the address map from the address hash table.
     */
    hash = xnethash((uchar *)&hp->proto_addr.addr, hp->proto_addr.length) &
	(RELAYHASH-1);
    map = &relaytable[hash];
    while (*map) {
	if (*map == hp) {
	    *map = hp->next;
	    hp->next = NULL;
	    map_err = FALSE;
	    break;
	}
	map = &((*map)->next);
    }		
    reset_interrupt_level(level);
    
    if (map_err == TRUE) {
	/*
	 * if there is an error finding the map from the hash chain, we don't
	 * remove anything and let frame relay do it best.
	 */
	return;
    }

    /*
     * Decrement use_count
     *	 Remove the map
     *   If no other static map associated with this pvc, no int-dlci, 
     *      If the pvc is dynamic mapped under subinterface 
     *         print warning message and remove the pvc.
     *      If the pvc is dynamic mapped under main interface
     *         change the pvc type to DYNAMIC. So when PVC will be removed
     *	       when the DLCI is deleted from the switch.
     *      If there is no dynamic mapping exist on this pvc, remove pvc.
     */
    fr_idb = hp->idb->hwptr->frame_relay_stuff;
    if (!fr_idb)
	return;
    fr_pvc = fr_idb->fr_dlci_tab[hp->dlci_num];
    if (!fr_pvc || (hp->dlci_num > MAXDLCI))
	return;
    fr_pvc->use_count--;
    fr_unset_idbmap(fr_idb, hp->dlci_num, hp->map_type, hp->link);
    
    if ((hp->link == LINK_ILLEGAL) &&
	(is_p2p(fr_pvc->idb)) &&
	(fr_pvc->idb->tbridge_on_swidb)) {
	fr_pri_delete_bridge_flood(fr_pvc, fr_idb);
    }

    if (fr_idb->map_status[hp->dlci_num] == FR_NOT_MAPPED) {
	if (fr_pvc->fr_if_set == FALSE) {
	    if (fr_pvc->use_count != 0) {
		if (is_subinterface(fr_pvc->idb)) {
		    if (fr_pvc->pvc_type != SVC_DYNAMIC)
		    	printf("\n%% Remove PVC DLCI %d while it is being used",
			   hp->dlci_num);
		    fr_pvc_clear(hp->idb->hwptr, hp->dlci_num);
		} else {
		    if (fr_pvc->pvc_type != SVC_DYNAMIC)
		    	fr_pvc->pvc_type = PVC_DYNAMIC;
		}
	    } else {
		/*
                 * don't delete the pvc if this pvc is a member of a
                 * priority-dlci-group.
                 */
                if ((!fr_pvc->fr_pr_dlci_group) && (!fr_pvc->fr_pr_enabled))
                    fr_pvc_clear(hp->idb->hwptr, hp->dlci_num);
	    }
	}
    }
    if (hp->tcp_compress)
	free(hp->tcp_compress);
    free(hp);
}

/*
 * frame_relay_deencapsulate
 * Remove frame relay parameters from the idb when changing encapsulations
 * on the interface.
 */

static void frame_relay_deencapsulate (
    hwidbtype *idb)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    int i;
    idbtype *swidb;
    boolean arp_status;

    idb->status |= IDB_BCAST;
    fr_idblist_remove(idb);
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
        if (is_first_swidb(swidb))
	    swidb->ip_split_disabled = swidb->ip_split_disabled_def = FALSE;
	arp_status = FALSE;
	reg_invoke_fr_arp_int(FR_ARP_INT_SET, swidb,
			      FR_CISCO_ARP, &arp_status);
	fr_clear_map(swidb);
	if (swidb->tbridge_on_swidb) {
	    idb->frame_relay_stuff->fr_bridge = FALSE;
	    idb->span_bridge_process_force &= ~SPAN_BR_PROC_FR_BRIDGE;
	}
	if (is_subinterface(swidb))
	    shutdown_subif(swidb, SUBIF_STATE_DELETED);
	queue_init(&swidb->tbridge_circuitQ, 0);
    }
    idb->fast_rcvidb = idb->firstsw;	/* may have changed in fr_fast_mci.c */
    /*
     * Don't clear the arp table here anymore - it gets done correctly
     * in encapsulation_command()
     */
    idb->lineaction = NULL;
    idb->linestate = NULL;
    fr_idb =  idb->frame_relay_stuff;
    if (!fr_idb)
        return;
    fr_idb->fr_ietf_encaps = FALSE;
    /*
     * clean up stats and pvc tables
     */
    for (i = 0; i < MAXDLCI; i++) {
	if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
	    continue;
	fr_idb->fr_dlci_tab[i] = NULL;
	if (fr_idb->fr_traffic_shape)
	    reg_invoke_fr_dlci_destroy(fr_pvc->idb, fr_pvc, SHAPE_DELETE);
	fr_pvc_unqueue(&fr_idb->fr_pvcQ,fr_pvc);
	if (fr_pvc->stats_save)
	    free((frmib_pvc_stats_type *)fr_pvc->stats_save);
	free(fr_pvc);
    }
    reg_invoke_fr_idb_uninit(idb);
    idb->frame_relay_stuff = NULL;
    idb->can_mcast = NULL;
    /*
     * Clear any memory allocated for autoinstall
     */
    if (fr_idb->fr_bootp) {
	free(fr_idb->fr_bootp);
	fr_idb->fr_bootp = NULL;
    }
    reg_invoke_Lapf_sys_event(LAPF_TEARDOWN, fr_idb);
    free(fr_idb);
    idb->frame_relay_stuff = NULL;
    idb->bridge_transit = FALSE;
}
/*
 * fr_verify_compression
 *
 * returns a pointer to a map entry if compression is configured.
 *
 */
frmaptype *fr_verify_compression (
				  hwidbtype *idb, 
				  uint link, 
				  ushort dlci)
{
    int i;
    frmaptype *p;

    for (i=0; i<RELAYHASH; i++) {
	for (p = relaytable[i]; p; p = p->next) {
	    if ((p->tcp_compress) &&
		(idb == p->idb->hwptr) &&
		(p->dlci_num == dlci) && 
		((COMPRESS_OVER_IP_LINK(p)) ||
		 (COMPRESS_OVER_COMPR_TCP_LINK(p)) ||
		 (COMPRESS_OVER_P2P_LINK(p)))) {
		return(p);
	    }
	}
    }
    return(NULL);
}
/* fr_inherit_compr_attributes
 *
 * Last will and testament, decide who gets Grandma's jewels!
 *
 * IP maps can inherit compression/passive/active characteristics.
 * COMPRESSED_TCP maps can inherit passive/active characteristics. 
 */
uint fr_inherit_compr_attributes (
				  idbtype *idb, 
				  uint compr_options,
				  uint linktype, 
				  uchar encaps_ietf)
{
    /*
     * Inherit passive/active characteristics based on how the interface
     * is configured unless the user specifically says otherwise!
     */
    if ((idb->fr_hdr_comp_passive == TRUE) &&
	!(compr_options & FR_MAP_ACTIVE) &&
	!(compr_options & FR_MAP_PASSIVE) &&
	(!(compr_options & FR_MAP_NO_COMPRESS)))
    	compr_options |= FR_MAP_INHERIT_PASSIVE;
    else 
    	compr_options &= ~FR_MAP_INHERIT_PASSIVE;

    /*
     * Inherit compression options based on how the interface
     * is configured, unless the user specifically says NO!
     */
    if ((linktype == LINK_IP) &&
	(idb->fr_tcp_hdr_comp == TRUE) &&
	(encaps_ietf != FR_ENCAPS_IETF)) {
    	if (!(compr_options & FR_MAP_NO_COMPRESS))
	    compr_options |= FR_MAP_INHERIT_COMPRESSION;
    } 
    return(compr_options);
}

/*
 * frame_relay_linestate
 * Return current line protocol status
 */

static boolean frame_relay_linestate (hwidbtype *hwidb)
{
    if (reg_invoke_dialer_spoofing_prot(hwidb))
	return(TRUE);
    else
	return(hwidb->frame_relay_stuff->fr_lineup);
}

/*
 * frame_relay_extract_addr
 *
 * Given a Frame Relay packet, extract the requested 'MAC' level address
 * into a hwaddrtype structure.
 */
static boolean frame_relay_extract_addr (
				  paktype *pak,
				  hwaddrtype *address,
				  enum SRCDST which)
{
    fr_idb_struct_type *fr_idb;
    frame_relay_hdr *hdr;
    ushort dlci;

    hdr = (frame_relay_hdr *)pak->datagramstart;
    address->type = STATION_FR10;
    address->length = STATIONLEN_FR10;
    address->target_type = TARGET_UNICAST;
    switch (which) {
    case GET_SOURCE:
	fr_idb = pak->if_input->hwptr->frame_relay_stuff;
	dlci = DLCI_TO_NUM(GETSHORT(&hdr->dlci));
	memcpy(address->addr, &dlci, STATIONLEN_FR10);
	break;
    case GET_DEST:
	return(FALSE);
    }

    return(TRUE);
}

/*
 * fr_copy_snpa
 * Used by autoinstall over frame-relay
 */
ushort fr_copy_snpa (hwaddrtype *address) 
{
    ushort pvc;
    memcpy((uchar *)&pvc, (uchar *)&address->addr, STATIONLEN_FR10);
    return(pvc);
}

/*
 * frame_relay_setup
 * Initialize frame relay encapsulation
 */
static boolean frame_relay_setup (parseinfo *csb, hwidbtype *idb)
{
    fr_idb_struct_type *fr_idb;
    int i;
    idbtype *swidb;
    dialerdbtype *ddb;

    if (idb->frame_relay_stuff != NULL)
	return(FALSE);
    fr_idb = malloc_named(sizeof(fr_idb_struct_type), "IDB: FR Info");
    if (!fr_idb)
        return(FALSE);
    
    fr_idb_cnt++;
    fr_idb->hwidb = idb;
    fr_idblist_add(idb);
    fr_idb->needsCompressionContext = FALSE;

    /* 
     * Allocate storage for any bootp related administrative stuff
     */
    fr_idb->fr_bootp = malloc_named(sizeof(fr_bootp_stuff), "FR Bootp");
    if (!fr_idb->fr_bootp) {
	free(fr_idb);
	return(FALSE);
    }
    fr_idb->fr_bootp->fr_ai_need_fs = FALSE;
    fr_idb->fr_bootp->fr_ai_initiated = FALSE;

    idb->status &= ~IDB_BCAST;
    if(idb->enctype == ET_FR_IETF)
    	fr_idb->fr_ietf_encaps = TRUE;
    else
	fr_idb->fr_ietf_encaps = FALSE;

    fr_idb->num_idb_pvcs = 0;
     

    /* (if interface status is up)
     * Set fr_idb_is_up to TRUE to start off with. 
     * This will update the status of all PVCs within lmi_idb_timeout()
     */
    fr_idb->fr_idb_is_up = FALSE; /* set it true when fr idb init is done */

    /*
     * initialize the pvc tab
     */
     for (i = 0; i <= MAXDLCI; i++)
          fr_idb->fr_dlci_tab[i] = NULL;

    if (! fr_started) {
	fr_started = TRUE;
	reg_invoke_fr_proc_create();
    }
    /*
     * We don't want to have the case of line status down or admin down but
     * line protocol status up.
     */
    fr_idb->fr_lineup = idb->state == IDBS_UP;
    fr_idb->fr_lsb_to_dlci_mask = 0x0f;
    fr_idb->fr_lsb_to_num_mask = 0xf0;
    fr_idb->fr_lsb_dlci_shift = 4;
    fr_idb->fr_msb_to_dlci_mask = 0x03f0;
    fr_idb->fr_msb_to_num_mask = 0xfc00;
    fr_idb->fr_msb_dlci_shift = 6;
    fr_idb->fr_multicast_dlci = NUM_TO_DLCI(FRAME_RELAY_DEFAULT_MULTICAST_DLCI);
    fr_idb->fr_local_dlci = FR_UNDEFINED_DLCI; 
    fr_idb->fr_dlci_bits = FRAME_RELAY_DEFAULT_DLCI_BITS;
    fr_idb->fr_multicast_status = FR_MULTICAST_NOTSET;
    fr_idb->fr_traffic_shape = FALSE;

    /*
     * Some SVC stuff.
     */
    fr_idb->fr_svc_b2b = FALSE;
    fr_idb->fr_svc_enabled = FALSE;
    fr_idb->fr_svc_configured = FALSE;
    fr_idb->nli_flink = NULL;
    fr_idb->nli_blink = NULL;
    fr_idb->svc_head = NULL;
    fr_idb->svc_tail = NULL;
    fr_idb->fr_idb_classQ = NULL;

    /*
     * Set the fastswitch flavor
     */
    idb->fast_switch_type = FS_FR;

    idb->inloopback = FALSE;
    idb->deencap = frame_relay_deencapsulate;
    idb->vencap = fr_vencapsulate;
    idb->bridge_vencap = (bridge_vencap_t)return_null;
    idb->getlink = frame_relay_getlink;
    idb->iqueue = frame_relay_input;
    idb->parse_packet = frame_relay_parse_packet;
    idb->periodic = frame_relay_periodic;
    idb->keep_count = 0;
    idb->extract_hwaddr = invalidate_hwaddr;
    idb->extract_snpa = frame_relay_extract_addr;
    idb->broadcast = frame_relay_bcast;
    idb->can_mcast = fr_can_mcast;
    idb->show_support = fr_interface;
    idb->encsize = FRAME_RELAY_ENCAPBYTES;
    idb->span_encapsize = idb->encsize;
    idb->lineaction = fr_cstate;
    idb->linestate = frame_relay_linestate;
    idb->frame_relay_stuff = fr_idb;
    idb->enctype = ET_FRAME_RELAY;	/* default encapsulation */

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	reg_invoke_media_enctype_setup(swidb, ET_FRAME_RELAY);
	swidb->ip_enctype = ET_FRAME_RELAY;
	if (is_first_swidb(swidb))
	    swidb->ip_split_disabled = swidb->ip_split_disabled_def = TRUE;
	swidb->ip_routecache_same = swidb->ip_routecache_same_def = TRUE;
	swidb->fr_tcp_hdr_comp = FALSE;
	swidb->fr_hdr_comp_passive = FALSE;
	swidb->tbridge_media = TBR_FRAME_RELAY_TRANSIT;
	idb->bridge_transit = TRUE;
	reg_invoke_fr_idb_class_alloc(swidb);
    }
    
    reg_invoke_fr_idb_init(idb);
    set_default_queueing (idb);

    /*
     * If this interface is a rotary group lead, set call limit.
     */
    if (reg_invoke_dialer_huntgroup_leader(idb)) {
        ddb = get_dialerdb(idb);
        ddb->dialer_max_call = DIALER_MAX_FR_CALLS;
    }

    if (supports_maxdgram(idb)) {
        if (!(idb->status & IDB_HSSI))
            (*idb->set_maxdgram)(idb, idb->maxmtu + ETHER_BAGGAGE, 
	        idb->maxmtu + ETHER_BAGGAGE + HDLC_ENCAPBYTES);
        else /* HSSI */
          (*idb->set_maxdgram)(idb, 
	    idb->maxmtu + ETHER_BAGGAGE + DS3_OVERHEAD_BYTES, 
	    idb->maxmtu + ETHER_BAGGAGE + DS3_BAGGAGE);
    }
    (*(idb->reset))(idb);
    /*
     * if circuit group was previously enabled on this interface and then the
     * encaps was chaned then remove the circuit group
     *
     * To disable fast bridging:
     * idb->span_bridge_process_force |= SPAN_BR_PROC_FR_BRIDGE;
     */
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	queue_init(&swidb->tbridge_circuitQ, 0);
	if (swidb->tbridge_on_swidb) {
	    reg_invoke_bridge_circuit_remove(swidb);
	    fr_idb->fr_bridge = TRUE;
	}
    }
    fr_idb->fr_idb_is_up = TRUE; /* set it true when fr idb init is done */

    return(TRUE);
}

/*
 * frame_relay_set_dialer
 * Perform frame relay specific actions when a FR interface is configured
 * for dial on demand.
 */
static void frame_relay_set_dialer (dialerdbtype *ddb, boolean setup)
{
    hwidbtype *idb;

    idb = ddb->ownidb;

    if (setup) {
	/*
	 * Adding DDR : save fr_idb_struct pointer and replace with pointer
	 * to fr_idb_struct of rotary group leader.
	 */
	if (ddb->dialer_encap_ptr == NULL) {
	    ddb->dialer_encap_ptr = idb->frame_relay_stuff;
	}
	idb->frame_relay_stuff = ddb->netcfg_idb->frame_relay_stuff;
        ddb->dialer_max_call = DIALER_MAX_FR_CALLS;
    } else {
	/*
	 * Removing DDR : restore original fr_idb_struct pointer
	 */
	idb->frame_relay_stuff = ddb->dialer_encap_ptr;
	ddb->dialer_encap_ptr = NULL;
    }
}


/*
 * frame_relay_periodic
 */
void frame_relay_periodic (hwidbtype *idb)
{
    fr_idb_struct_type *fr_idb;

   if (reg_invoke_dialer_spoofing_prot(idb)) {
	return;
    }

    fr_idb =  idb->frame_relay_stuff;
    if (!fr_idb)
        return;

    if (idb->enctype != ET_FRAME_RELAY)
	return; 

    /*
     * when sort_packet sees a compressed payload packet but there's no context
     * to decompress it in, it sets this flag so the context can be created
     * and subsequent compressed packets can be decompressed
     */
    if ( fr_idb->needsCompressionContext &&
	 !idb->comp_db &&
	 reg_invoke_init_stac_context( idb ) ) {
	  fr_idb->needsCompressionContext = FALSE;
	  idb->comp_db->vc_contexts++;
	}
    if (fr_idb->fr_svc_enabled) {
	reg_invoke_fr_svc_periodic(idb);
    }
}

/*
 * fr_interface
 * Display frame relay information related to a specific interface
 */
void fr_interface (hwidbtype *idb)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    ushort mc_dlci;
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    fr_idb =  idb->frame_relay_stuff;
    if (!fr_idb)
        return;

    if (fr_idb->fr_multicast_dlci !=
	NUM_TO_DLCI(FRAME_RELAY_DEFAULT_MULTICAST_DLCI)) {
	mc_dlci = DLCI_TO_NUM(fr_idb->fr_multicast_dlci);
	fr_pvc = fr_idb->fr_dlci_tab[mc_dlci];
	printf("\n  multicast DLCI %d, ",mc_dlci);
	printf(" status");
	print_dlci_status(fr_pvc->pvc_status, idb);
	printf("\n  source DLCI    %d",
	       DLCI_TO_NUM(fr_idb->fr_local_dlci));
    }
    if (!idb->nokeepalive) {
	if (!(ddb && HUNTGROUP_LEADER(ddb))) {
	    printf("\n  LMI enq sent  %d, LMI stat recvd %d, LMI upd recvd %d",
		   lmi_idb->lmi_stats->num_enquiries_sent,
		   lmi_idb->lmi_stats->num_status_rcvd,
		   lmi_idb->lmi_stats->num_upd_status_rcvd);
	    if (fr_idb->fr_intf_type != FR_DCE)
		printf(", DTE LMI %s",
                        (lmi_idb->lmi_lineup[FR_INTF_DTE] &&
			   (idb->state == IDBS_UP)) ? "up" : "down");
	    printf("\n  LMI enq recvd %d, LMI stat sent  %d, LMI upd sent  %d",
		   lmi_idb->lmi_stats->num_enquiries_rcvd,
		   lmi_idb->lmi_stats->num_status_sent,
		   lmi_idb->lmi_stats->num_upd_status_sent);
	    if (fr_idb->fr_intf_type != FR_DTE)
		printf(", DCE LMI %s", 
			(lmi_idb->lmi_lineup[FR_INTF_DCE] &&
			  (idb->state == IDBS_UP)) ? "up" : "down");
	}
	printf("\n  LMI DLCI %d",
	       DLCI_TO_NUM(lmi_idb->fr_lmi_dlci));
	switch(lmi_idb->fr_lmi_type) {
	case FR_CISCO_LMI:
	    printf("  LMI type is CISCO");
	    break;
	case FR_ANSI_D:
	    printf("  LMI type is ANSI Annex D");
	    break;
	case FR_CCITT_LMI:
	    printf("  LMI type is CCITT");
	    break;
	}
	switch(fr_idb->fr_intf_type) {
	case FR_DTE:
	    printf("  frame relay DTE");
	    break;
	case FR_DCE:
	    printf("  frame relay DCE");
	    break;
	case FR_NNI:
	    printf("  frame relay NNI");
	    break;
	}
    }
    /* now show info on svc */
    reg_invoke_Lapf_sys_event(LAPF_SHOW_IF, fr_idb);
    reg_invoke_fr_interface_display(fr_idb,ddb);
}

/*
 * print_dlci_status
 * Print the status of a DLCI as provided by the switch
 */
void print_dlci_status (
    uchar status,
    hwidbtype *hwidb)
{

    switch (status & DLCI_STATUS_MASK ) {
    case DLCI_PRESENT:
	printf(" defined");
	break;
    case DLCI_DELETED:
	printf(" deleted");
	return;
    case DLCI_ADDED:
	printf(" added");
	break;
    default:
    }
    printf(", %s%s", DLCI_ACTIVE(status) ? "active" : "inactive",
	(is_ddr(hwidb) && !reg_invoke_dialer_get_conn_member(hwidb)) ?
	" (spoofing)" : "");
    if (DLCI_CONGESTED(status))
	printf(", congested");
}


/*
 * frame_relay_hash_init
 * Initialize the hash table
 */

void frame_relay_hash_init (void)
{
    int i;
    
    for (i = 0; i < RELAYHASH; i++)
	relaytable[i] = NULL;
}

static ushort fr_get_subint_ref_num_from_pak (hwidbtype *hwidb, paktype *pak)
{
    fr_idb_struct_type *fr_idb;
    fr_idb = hwidb->frame_relay_stuff;
    return(DLCI_TO_NUM(GETSHORT(pak->mac_start)));
}


/*
 * fr_clear_subif_dynamic_entries
 *	clear all the dynamic entries that belong to a specific subinterface
 */
static void fr_clear_subif_dynamic_entries (idbtype *swidb)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    frmaptype *p, *ptemp;
    int i;
    ushort map_dlci;

    fr_idb = swidb->hwptr->frame_relay_stuff;
    
    for (i = 0; i < RELAYHASH; i++) {
	p = relaytable[i];
        while (p) {
	    ptemp = p->next;
	    if ((p->idb == swidb) && (p->dlci_num < MAXDLCI_VALUES)) {
                map_dlci = p->dlci_num; 
		if (p->map_type == FR_MAP_DYNAMIC)
		    frame_relay_map_delete(p);
		fr_pvc = fr_idb->fr_dlci_tab[map_dlci];
		if (fr_pvc) {
		    if (fr_pvc->pvc_type == PVC_DYNAMIC)
			fr_pvc_clear(swidb->hwptr, map_dlci);
		}
            }
	    p = ptemp;
	}	
    }	    
}


/*
 * fr_clear_subif_pvc
 *	clear all the pvc under a subinterface.
 */
static void fr_clear_subif_pvc (idbtype *swidb)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    int i;

    fr_idb = swidb->hwptr->frame_relay_stuff;
    
    for (i = 0; i < MAXDLCI; i++) {
	if (!(fr_pvc = fr_idb->fr_dlci_tab[i]) || fr_pvc->idb != swidb)
	    continue;
	if ((fr_pvc->pvc_usage == PVC_LOCAL) ||
	    (fr_pvc->pvc_usage == PVC_NOTSET)) {
	    fr_set_pvc_status(fr_pvc, DLCI_DELETED);
	    fr_clear_map(swidb);	
	    fr_pvc_clear(swidb->hwptr, i);
	}
    }
}

/*
 * fr_subif_pvcchange
 *      Set the pvc status according change to subinterface state.
 */
static void fr_subif_pvcchange (idbtype *swidb)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    int i;

    if ((fr_idb = swidb->hwptr->frame_relay_stuff) == NULL)
        return;

    for (i = 0; i < MAXDLCI; i++) {
        if (!(fr_pvc = fr_idb->fr_dlci_tab[i]) || fr_pvc->idb != swidb)
            continue;
        fr_pvc->pvc_upd_seq = 0;
        /*
         * Only handle PVCs terminated on this interface
         */
        if ((fr_pvc->pvc_usage == PVC_LOCAL) ||
            (fr_pvc->pvc_usage == PVC_NOTSET)) {
            switch(swidb->subif_state) {
            case SUBIF_STATE_UP:
                fr_set_pvc_status(fr_pvc, DLCI_ADDED);
                break;
            case SUBIF_STATE_DELETED:
                    fr_set_pvc_status(fr_pvc, DLCI_DELETED);
                break;
            case SUBIF_STATE_DOWN:
            case SUBIF_STATE_ADMINDOWN:
                if (!IS_DLCI_DELETED(fr_pvc->pvc_status)) {
                    fr_set_pvc_status(fr_pvc, DLCI_PRESENT);
                }
                break;
            }
        }
    }
}


/*
 * fr_subif_dte_statechange
 *      Subinterfaces state change over DTE
 */
static void fr_subif_dte_statechange(idbtype *swidb)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    int i; 

    fr_idb = swidb->hwptr->frame_relay_stuff;
    if (swidb->hwptr->nokeepalive) {
        fr_subif_pvcchange(swidb);
    }

    switch (swidb->subif_state) {
    case SUBIF_STATE_DOWN:
    case SUBIF_STATE_ADMINDOWN:
        fr_clear_subif_dynamic_entries(swidb);
        break;

    case SUBIF_STATE_DELETED:
        fr_clear_subif_pvc(swidb);
        break;

    case SUBIF_STATE_UP:
	for (i = MIN_DLCI; i <= MAX_DLCI; i++) {
	    if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
		continue;
	    if ((fr_pvc->idb != swidb))
		continue;
	    fr_set_pvc_status(fr_pvc, PVC_ACTIVE_BIT);
	}
        lmi_idb->lmi_n391dte = lmi_idb->fr_lmi_n391dte; /* req full status */
        fr_reset_lmi_events(fr_idb, FR_INTF_DTE, FR_LMI_PASS,
                            0, lmi_idb->fr_lmi_n393dte);
        break;
    }
}

/*
 * fr_subif_dce_statechange
 *      Subinterfaces state change over DCE
 */
static void fr_subif_dce_statechange(idbtype *swidb)
{
    fr_idb_struct_type *fr_idb;

    fr_idb = swidb->hwptr->frame_relay_stuff;
    fr_subif_pvcchange(swidb);

    switch(swidb->subif_state) {
    case SUBIF_STATE_DELETED:
        fr_clear_subif_pvc(swidb);
        break;

    case SUBIF_STATE_UP:
        lmi_idb->full_status_req = TRUE;               /* send full status */
        fr_reset_lmi_events(fr_idb, FR_INTF_DCE, FR_LMI_PASS,
                            0, lmi_idb->fr_lmi_n393dce);
        break;

    case SUBIF_STATE_DOWN:
    case SUBIF_STATE_ADMINDOWN:
        fr_clear_subif_dynamic_entries(swidb);
        break;
    }
}

/*
 * fr_subif_statechange
 *	Subinterfaces state change over DCE
 */
static void fr_subif_statechange (idbtype *swidb)
{
    fr_idb_struct_type *fr_idb;
    
    if (swidb == NULL || !is_subinterface(swidb)) {
	return;
    }

    fr_idb = swidb->hwptr->frame_relay_stuff;
    if (!fr_idb) {
	return;
    }
    
    if (swidb->subif_state == SUBIF_STATE_UP &&
	!hw_interface_up(swidb->hwptr)) {
	swidb->subif_state = SUBIF_STATE_DOWN;
    }
    
    switch(fr_idb->fr_intf_type) {
    case FR_DTE:
	fr_subif_dte_statechange(swidb);
	break;
    case FR_DCE:
	fr_subif_dce_statechange(swidb);
	break;
    /*
     * NNI is not available over subinterface
     */
    default:
	printf(" Unknown subinterface type 0x%x \n", fr_idb->fr_intf_type);
    }
}

/*
 * fr_list_list
 * Initialize classification lists used by frame relay
*/
static void fr_list_init (void)
{
    int i;

    for (i = 0; i <= FR_MAXLIST;  i++)
	fr_listarray[i] = NULL;
}
/*
 * frame_relay_get_srt_source
 */
static boolean frame_relay_srt_learn_source (
    idbtype *idb,
    paktype *pak,
    btetype *mac)
{
    fr_idb_struct_type *fr_idb;
    ushort dlci;
    
    if (!is_frame_relay(idb->hwptr))
	return(FALSE);
    fr_idb = idb->hwptr->frame_relay_stuff;

    dlci = DLCI_TO_NUM(*(ushort *)(pak->datagramstart-FR_TRANSIT_ENCSIZE(pak)));
    memcpy((uchar *)&mac->bte_proxy_addr.fr_dlci, (uchar *)&dlci, ADDRLEN_FR);
    if (frame_relay_debug)
	buginf("\n%s: Re-learning DLCI-input from %e "
	       "on DLCI %2d", idb->namestring, &mac->bte_mac_addr, mac->bte_proxy_addr.fr_dlci);
    return(TRUE);
}


static void fr_if_statechange_complete (idbtype *swidb, hwidbtype *hwidb) 
{
    idbtype *hw_swidb;
  
    /*
     * hw interface state change. We need to synchronize subinterface
     */
    if (swidb == NULL && hwidb) {
        if (!hwidb->frame_relay_stuff)
            return;
	FOR_ALL_SWIDBS_ON_HW(hwidb, hw_swidb) {
	    if (hw_interface_up(hwidb)) {
		fr_set_subif_state(hw_swidb, FR_ASSIGN_STATE,
				  SUBIF_STATE_UP);
	    } else {
		fr_set_subif_state(hw_swidb, FR_ASSIGN_STATE,
				  SUBIF_STATE_DOWN);
	    }
	    fr_subif_statechange(hw_swidb);
	}
	return;
    }
    /*
     * Subinterface state change only. Note: subinterface has been
     * assigned to the new state by network interface already.
     */
    fr_subif_statechange(swidb);
}


/*
 * fair conversation generator
 */
static short fr_fair_queue_flow_id (paktype *pak)
{
    ushort weight = FAIR_QUEUE_BASE_WEIGHT;
    ushort raw_dlci;
    ushort num_dlci;
    fr_idb_struct_type *fr_idb;
    idbtype *swidb = pak->if_output;
    hwidbtype *hwidb = swidb->hwptr;
    fr_idb = hwidb->frame_relay_stuff;
    raw_dlci = GETSHORT(pak->mac_start);

    if (ISFR_BECN(raw_dlci))
	weight <<= 1;	/* if BECN set, slow down by delaying delivery */
    if (ISFR_FECN(raw_dlci))
	weight <<= 1;	/* if FECN set, slow down by delaying delivery */
    if (ISFR_DE(raw_dlci))
	weight <<= 2;	/* if DE set, slow down by delaying delivery,
			   and make it yield place to committed traffic */
    pak->fair_weight = weight;
    num_dlci = DLCI_TO_NUM (raw_dlci);
    return(fair_queue_sum(pak, num_dlci));
}

/*
 * fr_print_network_header
 *
 * Print Frame Relay header.
 */
static void fr_print_network_header (paktype *pak, ushort **data)
{
    ushort raw_dlci;
    fr_idb_struct_type *fr_idb;
    idbtype *swidb = pak->if_output;
    hwidbtype *hwidb = swidb->hwptr;
    fr_idb = hwidb->frame_relay_stuff;
    raw_dlci = GETSHORT(pak->mac_start);

    printf("\n  DLCI: %d",  DLCI_TO_NUM (raw_dlci));
    if (ISFR_BECN(raw_dlci))
	printf(" + BECN");
    if (ISFR_FECN(raw_dlci))
	printf(" + FECN");
    if (ISFR_DE(raw_dlci))
	printf(" + DE");
}


/*
 * fr_reset_pvc_stats
 *	- called to handle clear counters command
 */
static void fr_reset_pvc_stats (fr_idb_struct_type *fr_idb)
{
    fr_pvc_info_type *fr_pvc;
    int i;

    for (i = 0; i < MAXDLCI; i++) {
	if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
	    continue;
	reg_invoke_fr_mib_save_stats(fr_pvc);
	clear_fr_pvc_stats(&fr_pvc->pvc_stats);
    }
}

/*
 * fr_init_idb_stats
 * Initialize/reset idb specific counters 
 */
static void fr_init_idb_stats (fr_idb_struct_type *fr_idb)
{
    if (!fr_idb)
	return;
    fr_idb->num_broad_frbq = 0;
    fr_idb->num_broad_total = 0;
    fr_idb->num_drop = 0;
}

/*
 * fr_clear_counters
 *	- correspond to the clearcounters user command
 */
static void fr_clear_counters (hwidbtype *idb)
{
    fr_idb_struct_type *fr_idb;

    if (is_frame_relay(idb) && (fr_idb = idb->frame_relay_stuff)) {
	fr_reset_pvc_stats(fr_idb);
	fr_init_idb_stats(fr_idb);
	reg_invoke_Lapf_sys_event(LAPF_CLR_COUNTERS, fr_idb);
    }
}

/*
 * fr_bridging_pri_list_changed
 * This function is called from frame_relay_priority_list_changed.
 * It will update the fr bridging pvc if bridging is configured
 * on it's swidb.
 */
void fr_bridging_pri_list_changed (fr_idb_struct_type *fr_idb,
                                  boolean pri_list_scb_sense,
                                  ulong priority_list_number,
                                  idbtype *swidb)
{
    fr_pvc_info_type *bridging_pvc, *new_bridging_pvc, *primary_fr_pvc;
    plisttype *plist;
    ushort new_priority_dlci;

    bridging_pvc = swidb->tbridge_circuitQ.qhead;
 
    while (bridging_pvc) {
 
        primary_fr_pvc = bridging_pvc;
 
        if (bridging_pvc->fr_pr_enabled) {
            primary_fr_pvc = fr_idb->fr_dlci_tab[bridging_pvc->primary_dlci];
        }
 
        if (primary_fr_pvc->fr_pr_dlci_group != priority_list_number) {
           bridging_pvc = bridging_pvc->bridge_flood_next;
           break;
        }
 
        /*
         * we only support priority setting if the map command is associated with
         * the primary dlci on multipoint OR in p2p, if the primary dlci is associated
         * with interface-dlci.
         */
 
        if (!(((fr_idb->map_status[primary_fr_pvc->this_dlci] & FR_BR_MAPPED)) ||
            (is_p2p(primary_fr_pvc->idb) &&
             (fr_idb->map_status[primary_fr_pvc->this_dlci] & FR_ALL_MAPPED))))
            break;
	/* deals with "no priority-list" command */
	if (!pri_list_scb_sense) {
		
            /*
             * the primary pvc is associated with the frame-relay map command
             * therefore it is the primary pvc that will be added to the
             * briding queue.
             */
 
            fr_pri_delete_add_bridge_flood(bridging_pvc, primary_fr_pvc, fr_idb);
        }
        else {
            /*
             * traverses all elements of this particular priority
             * list (parray[priority_list_number]) and handles
             * those elements whose linktype is LINK_BRIDGE. By
             * handle I mean, clear the bridge cache, remove the
             * pvc from bridge entry and re-add the correct pvc.
             */
 
            plist = parray[priority_list_number]->plist;
            while (plist){
                if (plist->linkcode == LINK_BRIDGE) {
                    new_priority_dlci = primary_fr_pvc->fr_pr_dlcis[plist->priority];
                    if (new_priority_dlci != bridging_pvc->this_dlci) {
                        new_bridging_pvc = fr_idb->fr_dlci_tab[new_priority_dlci];
                        fr_pri_delete_add_bridge_flood (bridging_pvc, new_bridging_pvc, fr_idb);
                    }
		}
                plist = plist->next;
            } 
        }
        bridging_pvc = bridging_pvc->bridge_flood_next;
    }
}
 
/*
 * frame_relay_priority_list_changed
 * This function will update FrameRelay pvc on different applications depending
 * on the priority-list changes. Currently it only supports FrameRelay bridging.
 */
 
void frame_relay_priority_list_changed (ulong priority_list_number,
                                        uchar linktype,
                                        boolean pri_list_scb_sense)
{
    hwidbtype *idb;
    idbtype *swidb;
    fr_idb_struct_type *fr_idb;
    list_element *element;

    /*
     * Deal with configuration that have linktype of either:
     *     LINK_BRIDGE or
     *     LINK_ILLEGAL (when dealing with "no priority-list x protocol")
     *  otherwise it just returns.
     */
    if (linktype != LINK_ILLEGAL) {
        if (linktype != LINK_BRIDGE)
            return;
    }
    else if (pri_list_scb_sense)
        return;
    
    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, idb) {
        FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
            fr_idb = swidb->hwptr->frame_relay_stuff;
 
            if ((linktype == LINK_BRIDGE) ||
		(linktype == LINK_ILLEGAL))
                fr_bridging_pri_list_changed(fr_idb, pri_list_scb_sense, priority_list_number, swidb);
        }
    }
}


/*
 * frame_relay_init
 */

static void frame_relay_init (subsystype *subsys)
{
    fr_debug_init();
    frame_relay_hash_init();
    fr_lmi_init();
    fr_started = FALSE;
    fr_list_init();
    memset(&fr_traffic, 0, sizeof(fr_traffic));

    /*
     * Register some functions
     */
    reg_add_keepalive_changed(ET_FRAME_RELAY,
		      set_fr_keepalive, "set_fr_keepalive");
    reg_add_determine_fair_queue_flow_id(LINK_FR_SWITCH_SERIAL,
					  fr_fair_queue_flow_id,
					 "fr_fair_queue_flow_id");
    reg_add_print_network_header(LINK_FR_SWITCH, fr_print_network_header,
				 "fr_print_network_header");
    reg_add_clear_counters(fr_clear_counters, "fr_clear_counters");
    reg_add_media_serial_setup(ET_FR_IETF, frame_relay_setup,
			   "frame_relay_ietf");
    reg_add_media_serial_setup(ET_FRAME_RELAY, frame_relay_setup,
			       "frame_relay_setup");
    reg_add_swif_erase(fr_clear_map, "fr_clear_map");
    reg_add_proto_on_swidb(LINK_FR_SWITCH, fr_switching_idb,
			   "fr_switching_idb");
    reg_add_sw_idb_encaps_init(ET_FRAME_RELAY, fr_init_encaps_idb,
                               "fr_init_encaps_idb");
    if (registry_populated(REG_SRT)) {
	reg_add_srt_learn_source(frame_relay_srt_learn_source,
				 "frame_relay_srt_learn_source");
    }
    reg_add_bridge_address(frame_relay_bridge_address,
			   "frame_relay_bridge_address");
    reg_add_encaps_nvram_write(ET_FRAME_RELAY, fr_encaps_nvram_write,
			       "fr_encaps_nvram_write");
    reg_add_encaps_show_protocol_attr(ET_FRAME_RELAY, 
				      fr_encaps_show_protocol_attr, 
		      		      "fr_encaps_show_protocol_attr");
    reg_add_serial_autoinstall(ET_FRAME_RELAY, fr_bootp_setup, 
					"fr_bootp_setup");
    reg_add_serial_lineup(ET_FRAME_RELAY, fr_bring_line_up, 
					"fr_bring_line_up");
    reg_add_autoinstall_addr_search(ET_FRAME_RELAY, fr_addr_search, 
					"fr_addr_search");
    reg_add_autoinstall_static_map(ET_FRAME_RELAY, fr_enter_static_map, 
					"fr_enter_static_map");
    reg_add_autoinstall_complete(ET_FRAME_RELAY, fr_bootp_complete, 
					"fr_bootp_complete");
    reg_add_autoinstall_cleanup(ET_FRAME_RELAY, fr_cleanup_after_ai,
					"fr_cleanup_after_ai");
    reg_add_fr_tcpip_header_compression_command(
				fr_tcpip_header_compression_command, 
				"fr_tcpip_header_compression_command"); 
    reg_add_media_frame_relay_parse(frame_relay_global_command,
				    "frame_relay_global_command");
    reg_add_extract_l3pid_value(fr_extract_l3pid_value, 
				"fr_extract_l3pid_value");
    reg_add_autoinstall_copy_snpa(ET_FRAME_RELAY, fr_copy_snpa, 
				"fr_copy_snpa");
    reg_add_dialer_setup(ET_FRAME_RELAY, frame_relay_set_dialer,
			 "frame_relay_set_dialer");
    reg_add_get_subint_ref_num_from_pak(ET_FRAME_RELAY,
					fr_get_subint_ref_num_from_pak,
					"fr_get_subint_ref_num_from_pak");
    reg_add_if_statechange_complete(fr_if_statechange_complete,
				    "Sync if state with PVC state");
    reg_add_fr_encaps_check(ET_FRAME_RELAY, fr_encaps_check, "fr_encaps_check");
    reg_add_fr_encaps_check(ET_FR_IETF, fr_encaps_check, "fr_encaps_check");
    reg_add_media_tbridge_swidb_vc_queue_init(ET_FRAME_RELAY,
					      fr_bridge_add_p2p_flood,
					      "fr_bridge_add_p2p_flood");
    reg_add_media_tbridge_swidb_vc_queue_clear(ET_FRAME_RELAY,
					      fr_bridge_clear_p2p_flood,
					      "fr_bridge_clear_p2p_flood");
    reg_add_encaps_compress(ET_FRAME_RELAY, fr_pbp_compress, "fr_pbp_compress");
    reg_add_priority_list_changed(frame_relay_priority_list_changed,
                                  "frame_relay_priority_list_changed");

    /*
     * Initialize Frame-Relay support
     */
    frame_relay_parser_init();

    /*
     * These two must be added after the Frame Relay private function registry
     * has been created.
     */
    reg_add_fr_addr_enter(fr_addr_enter, "fr_addr_enter");
    reg_add_fr_addr_lookup(frame_relay_addr_lookup, "frame_relay_addr_lookup");
    reg_add_fr_addr_remove(frame_relay_map_delete, "frame_relay_map_delete");
    reg_add_fr_p2p_intf_dlci(fr_parse_p2p_dlci, "fr_parse_p2p_dlci");
    reg_add_fr_p2p_intf_dlci_remove(fr_parse_no_if_dlci, "fr_parse_no_if_dlci");
    reg_add_mapclass_nvgen(ET_FRAME_RELAY, fr_mapclass_nvgen_func,
			   "fr_mapclass_nvgen_func");
    reg_add_show_fr_tcpip_header_compression(show_frame_relay_compression,
				"show_frame_relay_compression");
}
/* 
 * fr_encaps_check
 * If encapsulation is being changed from "frame-relay" to 
 * "frame-relay ietf" or vice versa, do not rip everything 
 * down. Leave the already defined frame relay stuff untouched. 
 */
boolean fr_encaps_check(
	parseinfo *csb, 
	hwidbtype *hwidb)
{
    fr_idb_struct_type *fr_idb;
    fr_idb = hwidb->frame_relay_stuff;

    if (!fr_idb)
	return (FALSE);

    /* 
     * If the command "encaps frame ietf" is invoked and the
     * interface is already configured so, make no changes. 
     * If the interface is not configured as ietf, invalidate
     * cache entries, remove dynamic maps, clear compression
     */
    if (GETOBJ(int,1) == ET_FR_IETF) {
	if (fr_idb->fr_ietf_encaps == TRUE)
	    return(TRUE);
    	fr_idb->fr_ietf_encaps = TRUE;
	reg_invoke_bump_cache_version(hwidb, NULL, FALSE);
	fr_clear_dynamic_entries(hwidb, LINK_ILLEGAL, MAXDLCI_VALUES, FR_BOTH);
	fr_header_compression_set(csb->interface, FALSE, FALSE);
	return(TRUE);	
    }
    /* 
     * If the command "encaps frame" is invoked and the
     * interface is configured as ietf, do some routine cleanup. 
     */
    if (fr_idb->fr_ietf_encaps == TRUE) {
	reg_invoke_bump_cache_version(hwidb, NULL, FALSE);
	fr_clear_dynamic_entries(hwidb, LINK_ILLEGAL, MAXDLCI_VALUES, FR_BOTH);
    	fr_idb->fr_ietf_encaps = FALSE;
	return(TRUE);
    }
    /* 
     * If the command "encaps frame" is invoked and the interface 
     * is already configured so, make no changes. 
     */
    if (fr_idb->fr_ietf_encaps == FALSE) {
	return(TRUE);
    }
return(FALSE);
}

/*
 * return the l3pid value when requested by the 
 * llc2 code. 
 */
uchar fr_extract_l3pid_value(paktype *pak)
{
    fr_q933_hdr *hdr;
    uchar *l3pid_ptr;

    hdr = (fr_q933_hdr *)pak->datagramstart;
    l3pid_ptr = (uchar *)&hdr->l3_pid;
    /* return the lower byte only */
    return(*(l3pid_ptr+1));
}
/*
 * The BOOTP client receives a reply from a BOOTP server to its request
 * for an address. When the client end receives the reply, it needs to 
 * configure a static map back to the server so that a TFTP request
 * can get through.
 */
boolean fr_enter_static_map (
	hwidbtype *hwidb,
	addrtype *fr_bootp_server_addr, 
	ushort dlci)
{
    fr_addr_enter(hwidb->firstsw, fr_bootp_server_addr, dlci, TRUE,
	FR_ENCAPS_CISCO, FR_MAP_AUTOINSTALL, PVC_AUTOINSTALL, LINK_IP, 
				NO_OPTIONS, NULL);
    return(TRUE);
}
/*
 * fr_bootp_setup
 * Basic startup routine for bootp
 */
void fr_bootp_setup (parseinfo *csb)
{
    int cpid;
    hwidbtype *idb = csb->interface->hwptr;

    /* 
     * Just in case someone has decided to configure the interface, 
     * after autoinstall started .....
     * check to make sure that the interface doesn't have a valid IP 
     * address. If it does, abort autoinstall over FR. 
     */
    if (reg_invoke_ip_good_address(csb->interface->ip_address, 
			csb->interface->ip_nets_mask,
			(csb->interface->ip_nets_mask ? TRUE : FALSE))) {
	return;
    }

    if (!idb->frame_relay_stuff || !idb->frame_relay_stuff->fr_bootp
		|| !idb->serial_autoinstall) {
	return;
    }

    /* Save bootp_client PID in case we wanna kill it! */
    cpid = idb->frame_relay_stuff->fr_bootp->bootp_client_pid=
		reg_invoke_fork_bootp_client("FR BOOTP Process");

}
/*
 * fr_bring_line_up
 * A process, whereby ANSI, CCITT and CISCO LMI are configured.
 * A full status report is then requested from the switch
 * and when a full status message is received, the configured 
 * LMI is the correct LMI. The BOOTP part of the autoinstall 
 * is then initiated.
 */
int fr_bring_line_up (parseinfo *csb)
{
    hwidbtype *hwidb;
    fr_idb_struct_type *fr_idb;
    int i;
    hwidb = csb->interface->hwptr;
    if (hwidb->enctype != ET_FRAME_RELAY)
	return(FALSE);
    
    fr_idb = hwidb->frame_relay_stuff;

    fr_idb->fr_bootp->fr_ai_initiated = TRUE;
    fr_idb->fr_bootp->fr_ai_need_fs = TRUE;
    for (i = FR_ANSI_D; i > 0 ; i--) {
   	lmi_idb->lmi_n391dte = FR_N391_COUNTER_MAX;
	fr_lmi_set(hwidb,fr_idb,i,TRUE, TRUE);
	fr_wait(10);
	lmi_idb->fr_dte_rpt_type_sent = FR_REPORT_FULL_STATUS;
	send_frame_relay_keepalive(hwidb);
	fr_wait(2);
	if (fr_idb->fr_bootp->fr_ai_need_fs == FALSE) {
	    fr_wait(5);
	    if (!hwidb->serial_autoinstall)
		return(FALSE);
	    return(TRUE);
	}
    }
return(FALSE);
}
/* 
 * fr_cleanup_after_ai
 * Remove the maps that we set up to allow autoinstall to succeed.
 * This must be done at the very end so that we do not lose connectivity
 * to the next hop.
 */
void fr_cleanup_after_ai(hwidbtype *idb)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    int i;
    frmaptype *p, *ptemp;

    if (!fr_started)
	return;
    if (idb->enctype != ET_FRAME_RELAY)
	return;
    fr_idb = idb->frame_relay_stuff;
    for (i=0; i<RELAYHASH; i++) {
    	for (p = relaytable[i]; p; ) {
	    if ((idb == p->idb->hwptr) && (p->link == LINK_IP)) {
		if (p->dlci_num) {
		    fr_pvc = fr_idb->fr_dlci_tab[p->dlci_num];
		    if (fr_pvc && (p->map_type == FR_MAP_AUTOINSTALL)) {
		    	ptemp = p->next;
		    	frame_relay_map_delete(p);
		    	p = ptemp;
		    	continue;
		    }
		}
	    }
	    p = p->next;
	}
    }
}
/*
 * fr_bootp_complete
 * Basic administrative cleanup routine for autoinstall
 */
void fr_bootp_complete (hwidbtype *idb)
{
    fr_idb_struct_type *fr_idb;
    fr_bootp_stuff *fr_bootp;
    int i;
    long blankaddr;
    frmaptype *p, *ptemp;

    if (!fr_started)
	return;
    if (idb->enctype != ET_FRAME_RELAY)
	return;
    fr_idb = idb->frame_relay_stuff;
    fr_bootp = fr_idb->fr_bootp;
    fr_bootp->fr_ai_initiated = FALSE;
    idb->serial_autoinstall = FALSE;
    /* Remove any bogus IP maps*/
    blankaddr = 0;
    for (i=0; i<RELAYHASH; i++) {
    	for (p = relaytable[i]; p; ) {
	    if ((idb == p->idb->hwptr) && (p->link == LINK_IP)) {
		if (bcmp(((uchar *)&((p->proto_addr).addr)),
			(uchar *)&blankaddr, sizeof(ipaddrtype)) == 0) {
		    ptemp = p->next;
		    frame_relay_map_delete(p);
		    p = ptemp;
		    continue;
    		}
	    }
	    p = p->next;
	}
    }
}
/*
 * fr_addr_search
 * Search the frame relay map table to find an appropriate entry.
 * If an corresponding the DLCI is found, send it back.
 */
addrtype *fr_addr_search (
    hwidbtype *idb, 
    ushort dlci, 
    paktype *pak)
{
    frmaptype *p;
    int i;
    addrtype *proto_addr;

    for (i = 0; i < RELAYHASH; i++) {
	for (p = relaytable[i]; p; p = p->next) {
	    if (is_p2p(p->idb) && (pak->if_input == p->idb)) {
		if (p->p2p_link == LINK_IP) {
		    return(&p->p2p_proto_addr);
		}
		continue;
	    }
	    if ((p->link == LINK_IP) && (pak->if_input == p->idb) &&
				p->dlci_num == dlci) {
              	proto_addr = &p->proto_addr;
		return(proto_addr);
	    }
      	}
    }
return(NULL);
}


/*
 * show_frame_relay
 */

void show_frame_relay (parseinfo *csb)
{
    switch (csb->which) {
      case SHOW_FR_SVC_MAPLIST:
	show_frame_relay_maplist(csb);
	break;

      case SHOW_FRAME_RELAY_COMP:
	show_frame_relay_compression(csb);
	break;

      case SHOW_FRAME_RELAY_MAP:
	show_frame_relay_map();
	break;

      case SHOW_FRAME_RELAY_TRAFFIC:
	show_fr_traffic();
	break;

      case SHOW_FRAME_RELAY_PVC:
	show_pvc_stats(csb);
	break;

      case SHOW_FRAME_RELAY_LMI:
	show_lmi_stats(csb);
	break;

      case SHOW_FRAME_RELAY_LAPF:
	reg_invoke_Lapf_showCmd(csb);
	break;

      case SHOW_FRAME_RELAY_ROUTE:
	show_fr_route();
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

void show_frame_relay_maplist (parseinfo *csb)
{
    maplist_type      		*list_ptr;
    char              		*name;
    char              		buf[64];
    int               		iy;
    staticmap_type    		*map_ptr;
    staticmap_type    		*tmp_map_ptr;
    fr_params_type   		*fr_params;
    struct fr_svc_params    	*map_params;
    struct fr_svc_params    	*in_params;
    fr_svc_nli_type		*nli;

    nli = NULL;
    map_params = NULL;
    in_params = NULL;
    name = GETOBJ(string, 1);
    list_ptr = maplist_lookup(name);

    if (!list_ptr) {
      return;
    }

    printf("\nMap List : %s", name);
    FOR_ALL_MAPS_IN_LIST(iy, list_ptr, map_ptr) {
	printf("\nAddress : Source %s %s <----> Destination %s %s",
	       (list_ptr->addr.saddr.num_plan == FR_E164_ADDR) ? "E164" : "X121", 
	       (char *)&list_ptr->addr.saddr.addr,
	       (list_ptr->addr.daddr.num_plan == FR_E164_ADDR) ? "E164" : "X121",
	       (char *)&list_ptr->addr.daddr.addr);
	
	printf("\n");
	goto jump;		/* ugly but necessary */
    }

jump:

    if (map_ptr && (map_ptr->class)) {
	if ((fr_params = (fr_params_type *)map_ptr->class->params)) {
	    map_params = fr_params->svc_params;
	}
	if (!map_params)
	    return;
    } else {
	return;
    }

    nli = map_ptr->nli;

    if (nli && nli->in_params)
	in_params = nli->in_params;

    FOR_ALL_MAPS_IN_LIST(iy, list_ptr, tmp_map_ptr) {
	print_proto_addr(NULL, &tmp_map_ptr->addr, tmp_map_ptr->protocol, buf);
	printf("\nProtocol : %s", buf);

	if (tmp_map_ptr->encap)
	    printf("\t\t\t\tEncapsulation : %s", 
		   (tmp_map_ptr->encap_flavor == ET_FR_IETF) ? "IETF" : "CISCO");
    }

    if (nli && nli->dlci) {
	if (in_params->call_ref)
	    printf("\nCall Reference : %x, Call %s Side", in_params->call_ref,
		   (in_params->call_ref & 0x8000) ? "Destination" : "Origination");
	printf("\tDLCI : %d", nli->dlci);
    }


    printf("\n");
    printf("\nFMIF (Frame Mode Information Field Size), bytes");
    printf("\nConfigured : In = %d,", map_params->inc_max_fmif);
    printf("\tOut = %d", map_params->out_max_fmif);
    if (in_params) {
	printf("\nNegotiated : In = %d,", in_params->inc_max_fmif);
	printf("\tOut = %d", in_params->out_max_fmif);
    }
    printf("\n");

    printf("\nCIR (Committed Information Rate), bits/sec");
    printf("\nConfigured : In = %d,", 
	   fr_svc_get_exponential(map_params->inc_cir_mult,
				  map_params->inc_cir_mag));
    printf("\tOut = %d,",
	   fr_svc_get_exponential(map_params->out_cir_mult,
				  map_params->out_cir_mag));
    if (in_params) {
	printf("\nNegotiated : In = %d,",
	       fr_svc_get_exponential(in_params->inc_cir_mult,
				      in_params->inc_cir_mag));
	printf("\tOut = %d,",
	       fr_svc_get_exponential(in_params->out_cir_mult,
				      in_params->out_cir_mag));
    }
    printf("\n");

    printf("\nMinimum Acceptable CIR, bits/sec");
    printf("\nConfigured : In = %d,",
	   fr_svc_get_exponential(map_params->min_inc_cir_mult,
				  map_params->min_inc_cir_mag));
    printf("\tOut = %d,",
	   fr_svc_get_exponential(map_params->min_out_cir_mult,
				  map_params->min_out_cir_mag));
    if (in_params && (in_params->call_ref & 0x8000)) {
 	printf("\nNegotiated : In = %d,",
	       fr_svc_get_exponential(in_params->min_inc_cir_mult,
				      in_params->min_inc_cir_mag));
	printf("\tOut = %d,",
	       fr_svc_get_exponential(in_params->min_out_cir_mult,
				      in_params->min_out_cir_mag));
    }
    printf("\n");
    printf("\nBc (Committed Burst Size), bits");
    printf("\nConfigured : In = %d,", 
	   fr_svc_get_exponential((map_params->inc_cbs << 3),
				  map_params->inc_cbs_mag));
    printf("\tOut = %d,", 
	   fr_svc_get_exponential((map_params->out_cbs << 3),
				  map_params->out_cbs_mag));
    if (in_params) {
	printf("\nNegotiated : In = %d,", 
	       fr_svc_get_exponential((in_params->inc_cbs << 3),
				      in_params->inc_cbs_mag)); 
	printf("\tOut = %d,",
	       fr_svc_get_exponential((in_params->out_cbs << 3),
				      in_params->out_cbs_mag));
    }
    printf("\n");
    printf("\nBe (Excess Burst Size), bits");
    printf("\nConfigured : In = %d,",
	   fr_svc_get_exponential((map_params->inc_ebs << 3),
				  map_params->inc_ebs_mag));
    printf("\tOut = %d,",
	   fr_svc_get_exponential((map_params->out_ebs << 3),
				  map_params->out_ebs_mag));
    if (in_params) {
	printf("\nNegotiated : In = %d,",
	       fr_svc_get_exponential((in_params->inc_ebs << 3),
				      in_params->inc_ebs_mag));
	printf("\tOut = %d,", 
	       fr_svc_get_exponential((in_params->out_ebs << 3),
				      in_params->out_ebs_mag));
    }
    printf("\n");
}
void show_frame_relay_compression (parseinfo *csb)
{
    fr_idb_struct_type *fr_idb = NULL;
    hwidbtype *idb;
    boolean screenfull;
    int i;
    list_element *element;
    ushort dlci;
    frmaptype *p, *ptemp;
    char buf[64];

    screenfull = FALSE;

    automore_enable(NULL);
    if (GETOBJ(idb,1)) {
	idb = GETOBJ(idb,1)->hwptr;
	    if (is_frame_relay(idb)) {
		fr_idb =  idb->frame_relay_stuff;
		for (dlci = 0; dlci < MAX_DLCI; dlci++) {
		  for (i = 0; i < RELAYHASH; i++) {
		    p = relaytable[i];
		    while (p != NULL) {
			if (DLCI_TO_NUM(p->dlci) != dlci) {
			    p = p->next;
			    continue;
			}
			if (!COMPRESS_OVER_IP_LINK(p) && 
			  	!COMPRESS_OVER_COMPR_TCP_LINK(p) &&
				!COMPRESS_OVER_P2P_LINK(p)) {
			    p = p->next;
			    continue;
			}
			if (idb != p->idb->hwptr) {
			    p = p->next;
			    continue;
			}
			mem_lock(p);
			if (p->link == LINK_COMPRESSED_TCP)
			    print_proto_addr(p->idb, &p->proto_addr, LINK_IP, buf);
			else
	  		    print_proto_addr(p->idb, &p->proto_addr, p->link, buf);
			printf("\n  DLCI %d \t Link/Destination info: %s ", 
			DLCI_TO_NUM(p->dlci), p->link == LINK_ILLEGAL ?
					" point-to-point dlci" : buf);
			reg_invoke_show_compression_stats(LINK_IP, 
						idb, p->tcp_compress);
			ptemp = p->next;
			free(p);
			p = ptemp;
		    }
		  }
		}
	    }
    } else {
    	FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, idb) {
	    fr_idb =  idb->frame_relay_stuff;
	    for (dlci = 0; dlci < MAX_DLCI; dlci++) {
		for (i = 0; i < RELAYHASH; i++) {
		    p = relaytable[i];
		    while (p != NULL) {
			if (DLCI_TO_NUM(p->dlci) != dlci) {
			    p = p->next;
			    continue;
			}
			if (!COMPRESS_OVER_IP_LINK(p) && 
			  	!COMPRESS_OVER_COMPR_TCP_LINK(p) &&
				!COMPRESS_OVER_P2P_LINK(p)) {
			    p = p->next;
			    continue;
			}
			if (idb != p->idb->hwptr) {
			    p = p->next;
			    continue;
			}
			mem_lock(p);
			if (p->link == LINK_COMPRESSED_TCP)
			    print_proto_addr(p->idb, &p->proto_addr, LINK_IP, buf);
			else
	  		    print_proto_addr(p->idb, &p->proto_addr, p->link, buf);
			printf("\n  DLCI %d \t Link/Destination info: %s ", 
			DLCI_TO_NUM(p->dlci), p->link == LINK_ILLEGAL ?
					" point-to-point dlci" : buf);
			reg_invoke_show_compression_stats(LINK_IP, 
						idb, p->tcp_compress);
			ptemp = p->next;
			free(p);
			p = ptemp;
		    }
		}
	    }
	}
    }
    automore_disable();
}

/*
 * show_fr_traffic
 * display the traffic statistics for Frame Relay
 */
void show_fr_traffic (void)
{
    printf("\nFrame Relay statistics:");
    printf("\n\tARP requests sent %lu, ARP replies sent %lu",
	   fr_traffic.arpreqsent, fr_traffic.arprepsent);
    printf("\n\tARP request recvd %lu, ARP replies recvd %lu",
	   fr_traffic.arpreqrcvd, fr_traffic.arpreprcvd);

}

/*
 * show_frame_relay_map
 */

void show_frame_relay_map (void)
{
    frmaptype *p, *ptemp;
    int i;
    hwidbtype *idb;
    list_element *element;
    
    automore_enable(NULL);
    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FR, element, idb) {
	if (frame_relay_debug) {
	    idbtype *swidb;
	    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
		if (swidb->tbridge_circuitQ.qhead) {
		    fr_pvc_info_type *fr_pvc;
		    printf("\nInterface %s - bridge dlcis:",
			   swidb->namestring);
		    fr_pvc = swidb->tbridge_circuitQ.qhead;
		    do {
			printf("%d ", fr_pvc->this_dlci);
			fr_pvc = fr_pvc->bridge_flood_next;
		    } while (fr_pvc != NULL);
		}
	    }
	}
	for (i = 0; i < RELAYHASH; i++) {
	    p = relaytable[i];
	    while (p != NULL) {
		if ( p->link == LINK_ILLEGAL ) {
		    p = p->next;
		    continue;
		}
		if (idb != p->idb->hwptr) {
		    p = p->next;
		    continue;
		}
		mem_lock(p);
		frame_relay_display_map(p);
		ptemp = p->next;
		free(p);
		p = ptemp;
	    }
	}
	for (i = 0; i < RELAYHASH; i++) {
	    p = relaytable[i];
	    while (p != NULL) {
		if (idb != p->idb->hwptr) {
		    p = p->next;
		    continue;
		}
		mem_lock(p);
		if ( p->link == LINK_ILLEGAL )
		    frame_relay_display_int_dlci(p);
		ptemp = p->next;
		free(p);
		p = ptemp;
	    }
	}
    }
    automore_disable();
}


/*
 * frame_relay_display_map
*/
void frame_relay_display_map (frmaptype *p)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    
    fr_idb =  p->idb->hwptr->frame_relay_stuff;
    
    printf("\n%s (%s): ", p->idb->namestring, print_idbstate(p->idb));
    if (p->link == LINK_CLNS) {
	printf("CLNS ");
    } else {
	char buf[64];
	if (p->link == LINK_COMPRESSED_TCP)
	    print_proto_addr(p->idb, &p->proto_addr, LINK_IP, buf);
	else
	    print_proto_addr(p->idb, &p->proto_addr, p->link, buf);
	printf("%s ", buf);
    }
    if (is_subinterface(p->idb)) {
    	if (p->idb->subif_state == SUBIF_STATE_ADMINDOWN)
	    printf("\n              ");
    } else {
	if (p->idb->hwptr->state == IDBS_ADMINDOWN)
	    if (!p->idb->hwptr->standby)
	        printf("\n              ");
    }
    printf("dlci %d(0x%x,0x%x)",p->dlci_num, p->dlci_num, p->dlci);
    if (p->map_type == FR_MAP_STATIC)
	printf(", static,");
    if (p->map_type == FR_MAP_DYNAMIC)
	printf(", dynamic,");
    if (p->map_type == FR_MAP_SVC_DYNAMIC) {
    	printf(", SVC,");
    	fr_pvc = fr_idb->fr_dlci_tab[p->dlci_num];
    }
    if (p->broadcast)
	printf("\n              broadcast,");
    if (p->encaps_type == FR_ENCAPS_IETF ||
	(fr_idb->fr_ietf_encaps &&
	 			p->encaps_type == FR_ENCAPS_NOTSET))
	printf("\n              IETF");
    if (p->encaps_type == FR_ENCAPS_CISCO || 
	(!fr_idb->fr_ietf_encaps &&
				p->encaps_type == FR_ENCAPS_NOTSET))
	printf("\n              CISCO");
    if (!p->idb->hwptr->nokeepalive) {
	fr_pvc = fr_idb->fr_dlci_tab[p->dlci_num];
	if (fr_pvc) {
	    if (fr_pvc->pvc_bw > 0l)
	        printf(", BW = %U", fr_pvc->pvc_bw);
	    printf(", status");
	    print_dlci_status(fr_pvc->pvc_status, p->idb->hwptr);
	}
    }

    if (p->tcp_compress) {
        printf("\n              ");
	if (p->map_type == FR_MAP_STATIC) {
	printf("TCP/IP Header Compression %s",
		(p->compression_flags & FR_MAP_INHERIT_COMPRESSION) ?
			"(inherited)" : "(enabled)");
	printf("%s%s ",
	(p->compression_flags & FR_MAP_INHERIT_PASSIVE ?
				", passive (inherited)" : ""),
		(p->compression_flags & FR_MAP_PASSIVE ?
				", passive (enabled)" : ""));
	} else if (p->map_type == FR_MAP_DYNAMIC) {
		printf("TCP/IP Header Compression (dynamic)");
		if ((p->compression_flags & FR_MAP_INHERIT_PASSIVE)  ||
			(p->compression_flags & FR_MAP_PASSIVE))
			printf(", passive (dynamic)");
	}
    }

    if (p->compression_flags & FR_MAP_COMP_PAYLOAD_STAC) {
        printf("\n              Payload Compression packet-by-packet");
    }
    
    fr_pvc = fr_idb->fr_dlci_tab[p->dlci_num];
    if (!fr_pvc)
	return;
    if (fr_pvc->fr_pr_dlci_group)
	printf("\n  Priority DLCI Group %1d, DLCI %4d(HIGH), DLCI %4d(MEDIUM)"
	       "\n  DLCI %4d(NORMAL), DLCI %4d(LOW)",fr_pvc->fr_pr_dlci_group,
	       fr_pvc->fr_pr_dlcis[PRIORITY_HIGH],
	       fr_pvc->fr_pr_dlcis[PRIORITY_MEDIUM],
	       fr_pvc->fr_pr_dlcis[PRIORITY_NORMAL],
	       fr_pvc->fr_pr_dlcis[PRIORITY_LOW]);
}

/*
 * frame_relay_display_int_dlci
 * Display DLCIs attached to a subinterface
*/
void frame_relay_display_int_dlci (frmaptype *p)
{
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    
    fr_idb =  p->idb->hwptr->frame_relay_stuff;
    printf("\n%s (%s): ", p->idb->namestring, print_idbstate(p->idb));
    printf("point-to-point dlci, ");
    printf("dlci %d(0x%x,0x%x)",p->dlci_num, p->dlci_num, p->dlci);
    if (p->broadcast)
	printf(", broadcast");
    if (p->encaps_type == FR_ENCAPS_IETF)
	printf(", IETF");
    if (p->encaps_type == FR_ENCAPS_CISCO)
	printf(", CISCO");
    if (p->map_type == FR_MAP_INT_DLCI_SVC)
	printf(", SVC");
    if (! p->idb->hwptr->nokeepalive) {
	fr_pvc = fr_idb->fr_dlci_tab[p->dlci_num];
	if (fr_pvc) {
	    if (fr_pvc->pvc_bw > 0l)
	        printf(", BW = %U", fr_pvc->pvc_bw);
	    printf("\n          status");
	    print_dlci_status(fr_pvc->pvc_status, p->idb->hwptr);
	}
    }

    if (p->tcp_compress) {
	printf(",\n              TCP/IP Header Compression%s", 
		(p->compression_flags & FR_MAP_INHERIT_COMPRESSION ?
						" (inherited)" : ""));
	printf("%s ",
		(p->compression_flags & FR_MAP_INHERIT_PASSIVE ?
				", passive (inherited)" : ""));
    }
    fr_pvc = fr_idb->fr_dlci_tab[p->dlci_num];
    if (!fr_pvc)
	return;
    if (fr_pvc->fr_pr_dlci_group)
	printf("\n  Priority DLCI Group %1d, DLCI %4d(HIGH), DLCI %4d(MEDIUM)"
	       "\n  DLCI %4d(NORMAL), DLCI %4d(LOW)",fr_pvc->fr_pr_dlci_group,
	       fr_pvc->fr_pr_dlcis[PRIORITY_HIGH],
	       fr_pvc->fr_pr_dlcis[PRIORITY_MEDIUM],
	       fr_pvc->fr_pr_dlcis[PRIORITY_NORMAL],
	       fr_pvc->fr_pr_dlcis[PRIORITY_LOW]);
}

/*
 * fr_svc_broadcast_handler
 * Routine that'll handle broadcast packets in an SVC environment.
 */
void fr_svc_broadcast_handler(
        idbtype *idb,
        paktype *pak)
{
    staticmap_type      *smap;
    maplist_type        *list_ptr;
    mapgroup_type       *group_ptr;
    int                 i;
    uint                link;
    idbtype             *netidb;
    fr_idb_struct_type  *fr_idb;
    enum FR_ENCAPS_FORM encaps_type;

    link = LINK_MAXLINKTYPE;
    fr_idb =  idb->hwptr->frame_relay_stuff;

    netidb = get_pak_netidb(pak);

        FOR_ALL_MAPGROUPS_ON_IDB(idb, group_ptr) {
            list_ptr = group_ptr->maplist;
                FOR_ALL_MAPS_IN_LIST(i, list_ptr, smap) {
                    /*
                     * DECNET/bridging need to support routing updates
                     * and spanning tree updates. If a DECNET/BRIDGE map does
                     * not exist, we'll need to create one.
                     */
                    switch (pak->linktype) {
                        case LINK_DECNET_ROUTER_L1:
                        case LINK_DECNET_ROUTER_L2:
                        case LINK_DECNET_PRIME_ROUTER:
                        case LINK_DECNET_NODE:
                            link = LINK_DECNET;
                            break;
                        case LINK_DEC_SPANNING:
			case LINK_IEEE_SPANNING:
                            link = LINK_BRIDGE;
                            break;
                        default:
                            break;
                    }
                    /*
                     * Trigger an SVC if "broadcast" is TRUE.
                     * If a Call already exists for this maplist,
                     * l3if_action will not trigger a call.
                     * Fall through to normal processing of broadcast
                     * packets.
                     */
                    if ((smap->protocol != pak->linktype) &&
                                (link == LINK_MAXLINKTYPE))
                        continue;

                    if (smap->broadcast && smap->broadcast_trigger) {
                        encaps_type = (smap->encap_flavor == ET_FR_IETF) ?
                                FR_ENCAPS_IETF : FR_ENCAPS_CISCO;
                        fr_doencaps(idb,pak,0, pak->linktype, encaps_type,
                                FR_SVC_PID, NULL);
                        reg_invoke_fr_svc_l3_action(fr_idb, pak, smap,
                                0, L3_CALL_REQ);

                        if (L3_DBG)
                            buginf("\nFR:%s: maplist:%s SVC Broadcast, linktype %d",
                                idb->namestring, list_ptr->name, pak->linktype);
                            break;
                    }
                }
        }
}


/*
 * Support Higher level broadcast over DLCIs.
 * fr_broadcast
 */
frmaptype *fr_broadcast (
    register idbtype *idb,
    register paktype *pak,
    uint link)
{
    frmaptype *first_addr = NULL;
    fr_idb_struct_type *fr_idb;

    fr_idb = idb->hwptr->frame_relay_stuff;
/*
 * Handle sets of link types that should match an FR map
 * e.g., LINK_DECNET_ROUTER_L1 -> LINK_DECNET
*/
    	switch (link) {
	case LINK_DECNET_ROUTER_L1:
	case LINK_DECNET_ROUTER_L2:
    	case LINK_DECNET_PRIME_ROUTER:
    	case LINK_DECNET_NODE:
	    link = LINK_DECNET;
	    break;
    	case LINK_DEC_SPANNING:
    	case LINK_IEEE_SPANNING:
	    link = LINK_BRIDGE;
	    break;
    	default:
	    break;
    	}
    	first_addr = fr_broadcast_on_all_eligible_pvcs(idb, pak,link);
    	return (first_addr);
}

/*
 * fr_broadcast_on_all_eligible_pvcs
 * This routine came about because of the original thinking 
 * that a BOOTP autoinstall request must be directed out on a
 * specific PVC and not on all PVCs. This resulted in two routines,
 * fr_broadcast_on_all_eligible_pvcs and fr_broadcast_on_specified_pvc.
 * However, currently, BOOTP requests are broadcast on all PVCs 
 * and I decided to leave this routine as is instead of restoring 
 * it back within the fr_broadcast routine. shankar. 
 */
frmaptype *fr_broadcast_on_all_eligible_pvcs (
	register idbtype *idb,
	register paktype *pak,
	uint link)
{
    fr_idb_struct_type *fr_idb; 
    int i; 
    frmaptype *first_addr = NULL;
    register paktype *newpak;
    register frmaptype *addr = NULL;
    boolean result;
    frame_relay_hdr *hdr;
    ushort num_dlci;
    fr_pvc_info_type *fr_pvc;

    fr_idb = idb->hwptr->frame_relay_stuff;
    if (frame_relay_packet_debug(idb, IGNORE_DLCI))
	buginf("\n%s: broadcast search", idb->namestring);
    for (i = 0; i < RELAYHASH; i++) {
	for (addr = relaytable[i]; addr; addr = addr->next) {
	    if (!(addr->broadcast) && !is_p2p(addr->idb))
		continue;
	    if (addr->idb != idb)
		continue;
	    if ((addr->link != link) && (!((link == LINK_IP) &&
				(addr->link == LINK_COMPRESSED_TCP)))
				&& (addr->link != LINK_ILLEGAL))
		continue;
	    if (first_addr == NULL) {
		first_addr = addr;
		continue;
	    }
	    newpak = pak_duplicate(pak);
	    if (newpak) {
		if (frame_relay_packet_debug(idb, DLCI_TO_NUM(addr->dlci)))
		    buginf("\n%s: Broadcast on DLCI %d  link %d",
			   idb->namestring, DLCI_TO_NUM(addr->dlci),
			   addr->link);
		result = fr_encaps(newpak, 0L, addr);
		
		/* need to prevent compression of broadcasts packets?
		if ( pak->fr_flags )
		  pak->fr_flags = pak->fr_flags & ~ DEFERED_LATE_Q_COMPRESSION;
		*/

		hdr = (frame_relay_hdr *)pak->datagramstart;
		num_dlci = DLCI_TO_NUM(GETSHORT(&hdr->dlci));
		fr_pvc = fr_idb->fr_dlci_tab[num_dlci];
		if (result)
		    if (!system_configured || (link == LINK_BRIDGE) ||
			(link == LINK_DEC_SPANNING) ||
			(link == LINK_IEEE_SPANNING))
			datagram_out(newpak);
		    else
			if (!QUEUEFULL(&fr_idb->fr_br_packetQ)) {
			    p_enqueue(&fr_idb->fr_br_packetQ, newpak);
			    fr_idb->num_broad_frbq++;
			    continue;
			} else {
			    fr_idb->num_drop++;
			    datagram_done(newpak);
			}
		else
		    retbuffer(newpak);
	    }
	}
    }
    return(first_addr);
}

/*
 * fr_broadcast_cdp
 *
 * Duplicate packet out per each PVC associated with the underlying swidb.
 */
boolean fr_broadcast_cdp (idbtype *idb, paktype *pak)
{
    fr_idb_struct_type *fr_idb;
    int dlci;
    ushort saved_dlci = 0;
    fr_pvc_info_type *pvc;
    paktype *newpak;
    ushort linktype;

    fr_idb = idb->hwptr->frame_relay_stuff;
    linktype = pak->linktype;

    for (dlci = MIN_DLCI; dlci <= MAX_DLCI; dlci++) {
        pvc = fr_idb->fr_dlci_tab[dlci];
        if (pvc && pvc->idb == idb) {
            /*
             * If this is the first dlci to send on, save this one for later
             * processing.
             */
            if (!saved_dlci) {
                saved_dlci = dlci;
		/*
		 * If point-to-point, stop the searching.
		 */
		if (is_p2p(idb))
		    break;
		else
		    continue;
            }

            /*
             * Duplicate the packet and put into broadcast queue.
             */
            newpak = pak_duplicate(pak);
            if (newpak) {
		if (frame_relay_packet_debug(idb, dlci)) {
		    buginf("\n%s: Broadcast on DLCI %d link %d(%s)",
		       idb->namestring, dlci, linktype, link2name(linktype));
		}

                if (fr_doencaps(idb, newpak, NUM_TO_DLCI(dlci), linktype,
                                FR_ENCAPS_CISCO, NULL_L3_PID, NULL)) {
		    datagram_out(newpak);
                } else {
		    datagram_done(newpak);
                }
            }
        }
    }

    if (saved_dlci) {
	if (frame_relay_packet_debug(idb, saved_dlci)) {
	    buginf("\n%s: Broadcast on DLCI %d link %d(%s)",
	       idb->namestring, saved_dlci, linktype, link2name(linktype));
	}

        if (fr_doencaps(idb, pak, NUM_TO_DLCI(saved_dlci), linktype,
			FR_ENCAPS_CISCO, NULL_L3_PID, NULL)) {
            return(TRUE);
        }
    }
    return(FALSE);
}

void hexdump_packet (
	paktype *pak, 
	ulong status)
{
    uchar *ptr;
    int i, j;
    
    ptr = (uchar *)pak->datagramstart;
    buginf("\ndatagramstart = 0x%x, datagramsize = %d",
	   pak->datagramstart, pak->datagramsize);
    if (status & IDB_ETHER) {
	buginf("\nEther encap = 0x");
	for (i = 0; i < ((IEEEBYTES * 2) + 2); i++)
	    buginf("%02x ", *ptr++);
    } else if (status & IDB_SERIAL) {
	buginf("\nFR encap = 0x%08X",*(ulong *)pak->datagramstart);
	ptr += 4;
	i = 4;
    } else {
	buginf("\nhexdump_packet: Bad status = 0x%x", status);
	return;
    }
    buginf("\n");
    for (j = 0; i < pak->datagramsize; i++) {
	buginf("%02x ", *ptr++);
	if (j++ == 15) {
	    buginf("\n");
	    j = 0;
	}
    }
    buginf("\n");
}

/*
 * fr_clear_map
 * Re-initialize the Frame Relay map
 */

void fr_clear_map (idbtype *idb)
{
    frmaptype *p, *ptemp;
    int i;

    if (idb->hwptr->enctype_cfg != ET_FRAME_RELAY)
	return;

    for (i = 0; i < RELAYHASH; i++) {
	p = relaytable[i];
	while (p) {
	    ptemp = p->next;
	    if (p->idb == idb) {
		frame_relay_map_delete(p);
	    }
	    p = ptemp;
	}
    }
}

/*
  * print_pvc_ie
  * Print a PVC information element
  */
void print_pvc_ie (
    uchar *ptr,
    uchar lmi_type)
{
    ushort dlci;

    switch (lmi_type) {
    case FR_ANSI_D:
    case FR_CCITT_LMI:
	memcpy((uchar *) &dlci, ptr+2, sizeof(dlci));
	buginf("\nPVC IE 0x%2x, length 0x%2x, dlci %d, status 0x%2x",
	       *ptr, *(ptr+1), ANSI_DLCI_TO_NUM(dlci),(*(ptr+4) & 0x7F));
	break;
    case FR_CISCO_LMI:
	buginf("\nPVC IE 0x%2x, length 0x%2x, dlci %d, status 0x%2x, bw %d",
	       *ptr, *(ptr+1), GET_PVC_DLCI(ptr),
	       *(ptr+4), ((*(ptr+5) << 16) + (*(ptr+6) << 8) + *(ptr+7)));
	break;
    }
}

/*
  * print_mc_ie
  * Print multicast information element
  */
void print_mc_ie (uchar *ptr)
{
    buginf("\nMC IE %1x, length %2x, MC group %2x, MC DLCI %4u, SRC DLCI %4u, MC status %2x",
	   *ptr, *(ptr+1), *(ptr+2), GETSHORT(ptr+3), GETSHORT(ptr+5),
	   *(ptr+7));
}

/*
  * print_ka_ie
  * Print keepalive information element
  */
void print_ka_ie (uchar *ptr)
{
    buginf("\nKA IE %1x, length %1x, yourseq %2u, myseq %2u",
	   *ptr, *(ptr+1), *(ptr+2), *(ptr+3));
}

/*
  * print_report_ie
  * Print report type information element
  */
void print_report_ie (uchar *ptr)
{
    buginf("\nRT IE %1x, length %1x, type %1x", *ptr, *(ptr+1), *(ptr+2));
}


/*
 * l3pid
 * given a level 3 pid by llc2, return a ushort
 * that looks like 0x70xx where xx is the l3 pid.
 */
ushort l3pid(uchar level3pid) 
{
    ushort l3_pid;
	switch (level3pid) {
	    case L3_SNA:
		l3_pid = FR_L3_SNA;
		break;
	    case L3_SNA_PERIPHERAL:
		l3_pid = FR_L3_SNA_PERIPHERAL;
		break;
	    case L3_APPN:
		l3_pid = FR_L3_APPN;
		break;
	    case L3_NETBIOS:
		l3_pid = FR_L3_NETBIOS;
		break;
	    default:
		buginf("\nIBM: Unknown L3 PID, fr_doencap failed");  
		return(FALSE);
	}
return(l3_pid);
}

/*
 * tb1490link2serial
 * Given a linktype return the RFC1490 bridge type
*/
ushort tb1490link2serial(paktype *pak)
{
    if (pak->linktype == LINK_IEEE_SPANNING)
	return(FR1490_SPANNING);
    else
	return(FR1490_ETHER);
}

/*
 * fr_cstate
 * Handle interface state change
*/
void fr_cstate (hwidbtype *idb)
{
    int i;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    
    fr_idb = idb->frame_relay_stuff;

    idb->inloopback = FALSE;
    if (idb->state != IDBS_UP) {
	lmi_idb->fr_dte_mineseen = 0;
	lmi_idb->fr_dte_yoursseen = 0;
	lmi_idb->fr_dte_myseq = 0;
	lmi_idb->fr_dce_mineseen = 0;
	lmi_idb->fr_dce_yoursseen = 0;
	lmi_idb->fr_dce_myseq = 0;

	reg_invoke_fr_mgd_timer_onoff(idb, FALSE); /* turn off lmi/arp timers */

	/*
	 * No need to reset the multicast status here.
	 * If the current status is 'DYNAMIC', it will be reset later when
	 *    calls the function fr_mc_delete_dlci().
	 * If the current status is 'STATIC', this reset will prevent data
	 *    packets going out once the interface comes up again.
	 * fr_idb->fr_multicast_status = FR_MULTICAST_NOTSET;
	*/

	/*
	 * Make sure that SVCs are torn down.
	 */
	 if (fr_idb->fr_svc_configured == TRUE) {
	     fr_idb->fr_svc_enabled = FALSE;
	     fr_idb->fr_svc_b2b = FALSE;
	     reg_invoke_fr_svc_teardown_calls(fr_idb);
	}
	/*
	 * do this only if we are a dte
	 */
	if (fr_idb->fr_intf_type == FR_DTE || fr_idb->fr_intf_type == FR_DCE) {
	    /*
	     * Clear all dynamically entered PVCs for ALL
	     * link types and for ALL dlci, because the line is 
	     * down. 
	     *
 	     * note: 
 	     * 	link of LINK_ILLEGAL implies ALL link types
 	     * 	dlci of MAX_DLCIVALUES implies ALL dlci. 
	     */ 
	    fr_clear_dynamic_entries(idb, LINK_ILLEGAL, 
				  MAXDLCI_VALUES, FR_BOTH);
	    if (fr_idb->fr_multicast_status == FR_MULTICAST_DYNAMIC)
		fr_mc_delete_dlci(idb,DLCI_TO_NUM(fr_idb->fr_multicast_dlci));
	    for (i = 0; i < MAXDLCI; i++) {
		if (!(fr_pvc = fr_idb->fr_dlci_tab[i]) ||
		    fr_pvc->idb != idb->firstsw)
		    continue;
		fr_pvc->pvc_upd_seq = 0;
		/*
		 * Don't tag switched PVCs as inactive
		 */
		if ((fr_pvc->pvc_usage == PVC_LOCAL) ||
		     (fr_pvc->pvc_usage == PVC_NOTSET)) {
		    if (!IS_DLCI_DELETED(fr_pvc->pvc_status)) {
			fr_set_pvc_status(fr_pvc, DLCI_DELETED);
		    }
		    /*
		     * If this interface is a rotary group member (Serial
		     * or B channel), the call is being disconnected.
		     * We need to set the subinterfaces state on the group
		     * leader (Dialer, D channel) to UP (spoofing) so that 
		     * we can place calls for any subinterface.
		     * For dialers on plain serial interfaces, the same
		     * thing is done in dialer.c when the enable timer
		     * expires.
		     * Note that idb is the real (physical) interface idb,
		     * but fr_pvc->idb is the group leader idb.
		     */
		    if (reg_invoke_dialer_huntgroup_member(idb)) {
			fr_set_subif_state(fr_pvc->idb, FR_ASSIGN_STATE,
					   SUBIF_STATE_UP);
		    } else {	
			fr_set_subif_state(fr_pvc->idb, FR_ASSIGN_STATE,
					   SUBIF_STATE_DOWN);
		    }
	        }
	    }
	}
	reg_invoke_Lapf_sys_event(FR_PH_DEACT, fr_idb);
	TIMER_START(fr_idb->timer_309, T309_DEFAULT);
    }
    if (idb->state == IDBS_UP) {
	lmi_idb->fr_dte_mineseen = 0;
	lmi_idb->fr_dte_yoursseen = 0;
	lmi_idb->fr_dte_myseq = 0;
	lmi_idb->fr_dce_mineseen = 0;
	lmi_idb->fr_dce_yoursseen = 0;
	lmi_idb->fr_dce_myseq = 0;
	/*
	 * Reset LMI when the interface is back up 
	 */
	if (fr_idb->fr_intf_type != FR_DTE) {
	    lmi_idb->full_status_req = TRUE;             /* send full status */
            fr_reset_lmi_events(fr_idb, FR_INTF_DCE, FR_LMI_PASS,
                                0, lmi_idb->fr_lmi_n393dce);
	}
        if (fr_idb->fr_intf_type != FR_DCE) {
	    lmi_idb->lmi_n391dte = lmi_idb->fr_lmi_n391dte; /*req full status*/
            fr_reset_lmi_events(fr_idb, FR_INTF_DTE, FR_LMI_PASS,
                                0, lmi_idb->fr_lmi_n393dte);
        }

	reg_invoke_fr_mgd_timer_onoff(idb, TRUE);  /* turn on lmi/arp timers */

	if (fr_idb->fr_multicast_dlci) {
	    fr_pvc = fr_idb->fr_dlci_tab[DLCI_TO_NUM(fr_idb->fr_multicast_dlci)];
	    fr_set_pvc_status(fr_pvc, DLCI_ADDED);
	}
	for (i = 0; i < MAXDLCI; i++) {
	    if (!(fr_pvc = fr_idb->fr_dlci_tab[i]))
		continue;
	    fr_pvc->pvc_upd_seq = 0;
	    /*
	     * Only handle PVCs terminated on this interface
	     */
	    if ((fr_pvc->pvc_usage == PVC_LOCAL) ||
		(fr_pvc->pvc_usage == PVC_NOTSET)) {
		fr_set_pvc_status(fr_pvc, DLCI_ADDED);
		fr_set_subif_state(fr_pvc->idb, FR_ASSIGN_STATE, SUBIF_STATE_UP);
	    }
	}
	TIMER_STOP(fr_idb->timer_309);

	if (fr_idb->fr_svc_configured == TRUE)
	    fr_idb->fr_svc_enabled = TRUE;

	reg_invoke_Lapf_sys_event(FR_PH_ACT, fr_idb);
	reg_invoke_media_protocol_up(idb, LINK_ALL);
	if ((lmi_idb->fr_lmi_type_configured == FALSE) &&
			(fr_idb->fr_intf_type == FR_DTE)) {
 	    fr_initiate_autosense(idb);
        }

    }
}
/*
 * fr_clear_dynamic_entries
 *
 * The link has gone down or conditions have changed. 
 * Clear all the dynamic PVCs and/or maps and update the status for others. 
 *
 * note: 
 * 	link of LINK_ILLEGAL implies ALL link types
 * 	dlci of MAX_DLCIVALUES implies ALL dlci.
 *      flag indicates whether to clear maps, PVCs, or both
 *
 * Also NOTE that at entry, the hw-idb argument is the interface for in/out
 * packets. For the case of FR-over-ISDN, given that FR only uses one channel,
 * beware this hw-idb doesn't have the real subif stuff for the dlci.
 */
void fr_clear_dynamic_entries (
	hwidbtype *idb, 
	long link, 
	ushort dlci,
	enum FR_CLEAR flag)
{
    int i;
    frmaptype *p, *ptemp;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *fr_pvc;
    
    fr_idb = idb->frame_relay_stuff;
    
    /* 
     * Only a FR_DTE will be acted upon. 
     */
    if(fr_idb->fr_intf_type != FR_DTE)
	return;
    
    for (i = 0; i < RELAYHASH; i++) {
	p = relaytable[i];
	while (p) {
	    
	    /*
	     * Only delete the specified link or delete ALL
	     * if LINK_ILLEGAL is specified. 
	     */
	    if ((link != LINK_ILLEGAL) && (p->link != link)) {
		p = p->next;
		continue; 
	    }
	    
	    /*
	     * Only delete the specified dlci or delete ALL
	     * if MAXDLCI_VALUES is specified. 
	     */
	    if ((dlci != MAXDLCI_VALUES) && (p->dlci_num != dlci)) {
		p = p->next;
		continue;
	    }

	    if (!(hwidb_or_null(p->idb) == idb)) {
	    	p = p->next;
	    	continue;
	    }

	    ptemp = p->next;
	    if (flag & FR_PVC) {
		fr_pvc = fr_idb->fr_dlci_tab[p->dlci_num];
		if (fr_pvc) {
		    if (fr_pvc->pvc_type == PVC_DYNAMIC)
			fr_pvc_clear(idb, p->dlci_num);
		    else if (!(flag & FR_LMI_CLEAR)) 
			fr_set_pvc_status(fr_pvc, DLCI_DELETED);
		}
	    }
	    if (flag & FR_MAP) { 
		if (p->map_type == FR_MAP_DYNAMIC)
		    frame_relay_map_delete(p);
	    }
	    p = ptemp;
	}	    
    }
}


/*
 * fr_parse_p2p_dlci
 * Add a DLCI to the current point-to-point subinterface
 */
boolean fr_parse_p2p_dlci (
    idbtype *idb,
    ushort dlci,
    boolean broadcast,
    enum FR_ENCAPS_FORM fr_encaps_type,
    uint linktype,
    mapclass_type *class_ptr, 
    addrtype addr)
{
    frmaptype *map = NULL, *l;
    addrtype proto_addr;
    fr_pvc_info_type *fr_pvc, *old_fr_pvc;
    fr_idb_struct_type *fr_idb;
    int options = 0;
    frmaptype *addr_enter;
    leveltype level;
    ushort		temp_pvc_attribute;
    
    temp_pvc_attribute = 0;
    if (linktype == FR_LINK_ILLEGAL_SVC) {
	linktype = LINK_ILLEGAL;
	temp_pvc_attribute = SVC_DYNAMIC;
    }
    memset(&proto_addr, 0, sizeof(addrtype));
    memcpy(&proto_addr.addr, (uchar *)&idb, sizeof(idb));
    proto_addr.length = sizeof(idb); 
    
    fr_idb = idb->hwptr->frame_relay_stuff;
    fr_pvc = fr_idb->fr_dlci_tab[dlci];
    if (linktype) {
    	level = raise_interrupt_level(NETS_DISABLE);
    	l = relaytable[xnethash((char *)&proto_addr.addr,
    				proto_addr.length) & (RELAYHASH-1)];
    	while (l) {
    	    if ((l->p2p_link == linktype) && (l->idb == idb) &&
		address_equ(&l->p2p_proto_addr, &proto_addr)) {
    		reset_interrupt_level(level);
    		map = l;
    		break;
    	    }
    	    l = l->next;
    	}
    	reset_interrupt_level(level);
    } else {
    	map = frame_relay_addr_lookup(idb, &proto_addr, LINK_ILLEGAL);
    }

	if (fr_encaps_type != FR_ENCAPS_IETF) {
    	    if (idb->fr_tcp_hdr_comp == TRUE)
	    	options |= FR_MAP_INHERIT_COMPRESSION;

    	    if (idb->fr_hdr_comp_passive == TRUE)
	    	options |= FR_MAP_INHERIT_PASSIVE;
	} 

	/* 
	 * First make sure that the DLCI isn't assigned to
	 * another subinterface. 
	 */
	if (fr_pvc) {
	    if (fr_pvc->idb && (fr_pvc->idb != idb) &&
		(fr_pvc->idb != fr_pvc->idb->hwptr->firstsw) && 
		(fr_pvc->pvc_type != PVC_AUTOINSTALL)) {
                printf("\n%%PVC already assigned to sub-interface %s",
                fr_pvc->idb->namestring);
		return(FALSE);
	    }
	    /*
	     * Verify no static maps have been entered for this PVC
	    */
	    if (fr_idb->map_status[dlci] != FR_NOT_MAPPED) {
                printf("\n%%PVC is already defined");
		return(FALSE);
	    }
	    /*
	     * If any dynamic maps exists clear them and install p2p subif
	    */
	    if (fr_idb->map_dynamic[dlci] != FR_NOT_DYNAMIC)
		fr_clear_dynamic_entries(fr_pvc->idb->hwptr, LINK_ILLEGAL
					 ,dlci,FR_MAP);
	}
	
	if ( !map ) {
	    /* Don't allow two PVCs on a point-to-point subinterface. */
	new_map:	    
	    if ((idb->subif_link_type == SUBIF_TYPE_P2P) && 
		frame_relay_check_idb(idb) ) {
		printf("\n%%Can't add another VC to point-to-point subinterface");
	    } else {
		if (!linktype) {
		    if (!temp_pvc_attribute)
		    addr_enter = fr_addr_enter(idb, &proto_addr, dlci, broadcast,
			fr_encaps_type, FR_MAP_INT_DLCI, PVC_STATIC,
				LINK_ILLEGAL, options, NULL);
		    else
		    	addr_enter = fr_addr_enter(idb, &proto_addr, dlci, broadcast,
				fr_encaps_type, FR_MAP_INT_DLCI_SVC, SVC_DYNAMIC,
				LINK_ILLEGAL, options, class_ptr);
		} else {
		    memcpy( &proto_addr, &addr, sizeof(addrtype));
		    addr_enter = fr_addr_enter(idb, &proto_addr, dlci, broadcast,
			fr_encaps_type, FR_MAP_INT_DLCI, PVC_STATIC,
			linktype, options, NULL);
		}
		if (addr_enter) {
		    /* Set PVC value initialized by the map creation */
		    fr_pvc = fr_idb->fr_dlci_tab[dlci];
		    if (fr_pvc) {
			fr_set_subif_state(fr_pvc->idb, FR_PVC_STATE,
					   fr_pvc->pvc_status);
		    }
		}
	    }
	} else { /* Update an existing map */
	    old_fr_pvc = fr_idb->fr_dlci_tab[map->dlci_num];
	    /* Re-assign the PVC interface to the main interface only if
	     * old and new aren't the same
	     */  
	    if (old_fr_pvc && (dlci != map->dlci_num)) {
		frame_relay_map_delete(map);
		goto new_map;
	    }
	}
return(TRUE);
}


/*
 * fr_clear_dlci_map
 *      Clear the map associates with a dlci under a interface
 */
static void fr_clear_dlci_map(idbtype *swidb, ushort dlci)
{
    frmaptype *p, *ptemp;
    int i;
    fr_idb_struct_type *fr_idb;

    fr_idb = swidb->hwptr->frame_relay_stuff;

    /*
     * Search for the map associates with the dlci and remove it
     */

    for (i = 0; i < RELAYHASH; i++) {
	p = relaytable[i];
	while (p) {
	    ptemp = p->next;
	    if (p->idb == swidb && p->dlci_num == dlci) {
		frame_relay_map_delete(p);
	    }
	    p = ptemp;
	}
    }
}


/*
 * fr_parse_multi_dlci
 *	-- handle "fr interface-dlci" for multipoint interface/subinterface
 */
void fr_parse_multi_dlci (idbtype *idb,
			  ushort dlci)
{
    fr_pvc_info_type *fr_pvc;
    fr_idb_struct_type *fr_idb;
    
    fr_idb = idb->hwptr->frame_relay_stuff;
    fr_pvc = fr_idb->fr_dlci_tab[dlci];
    
    /* 
     * First make sure that the DLCI isn't assigned to
     * another subinterface and is not used in a map configuration 
     */
    if (fr_pvc) {
	if (fr_pvc->idb && (fr_pvc->idb != idb)) {
	    if (fr_pvc->idb != fr_pvc->idb->hwptr->firstsw) {
	        /*
	         * PVC already assigned to a sub-interface
	         * Check to see if it is used in any maps
	         */
	        if (is_p2p(fr_pvc->idb) ||
		(fr_idb->map_dynamic[dlci] || fr_idb->map_status[dlci])) {
                    printf("\n%%PVC already in use on sub-interface %s",
                    fr_pvc->idb->namestring);
		    return;
	        }
	    } else {
                if (fr_idb->map_status[dlci] != FR_NOT_MAPPED) {
                    printf("\n%%PVC already in use by static frame-relay map");
                    return;
                }
            }
	    if (fr_idb->map_dynamic[dlci] != FR_NOT_DYNAMIC)
		fr_clear_dynamic_entries(fr_pvc->idb->hwptr, LINK_ILLEGAL,
					 dlci, FR_MAP);
	}
	fr_pvc->idb = idb;
	fr_pvc->pvc_type = PVC_STATIC;
	fr_pvc->pvc_usage = PVC_LOCAL;
	fr_pvc->fr_if_set = TRUE;
    } else {
	fr_pvc = fr_pvc_init(idb,dlci,PVC_LOCAL,PVC_STATIC, TRUE, NULL);
	if (!fr_pvc)
	    return;
	fr_set_pvc_status(fr_pvc, DLCI_ADDED);
	fr_pvc->fr_if_set = TRUE;
	fr_idb->map_dynamic[dlci] = FR_NOT_DYNAMIC;
    }
}

/*
 * fr_parse_no_if_dlci
 *	-- handles "no fr interface-dlci" command
 */
void fr_parse_no_if_dlci (idbtype *idb,
			  ushort dlci)
{
    fr_idb_struct_type *fr_idb = NULL;
    fr_pvc_info_type *fr_pvc = NULL;
    idbtype *swidb = NULL;
    addrtype proto_addr;
    frmaptype *map;

    if (idb) {
	fr_idb = idb->hwptr->frame_relay_stuff;
	if (fr_idb) {
	    fr_pvc = fr_idb->fr_dlci_tab[dlci];
	    if (!fr_pvc)
		return;
	    swidb = fr_pvc->idb;
	}
    }
    if (!swidb)
	return;

    if (swidb != idb) {
	printf("\n%%error on NO FR INTERFACE-DLCI cmd - no DLCI %d on interface %s",
		dlci, idb->namestring);
	return;
    }
    if (fr_pvc->route_flag) {
	printf("\n%%command NO FR INTERFACE-DLCI inapplicable for "
	       "static route DLCI %d", dlci);
	return;
    }
    /*
     * Three basic cases:
     * p2p subinterface where we must clear a map
     * multi-point subinterface but the DLCI is referenced in maps
     * multipoint subinterfaces the DLCI isn't referenced by any maps
     */

    if (is_p2p(swidb)) {
	map = frame_relay_addr_lookup(idb, &proto_addr, LINK_ILLEGAL);
	if (map) {
	    frame_relay_map_delete(map);
	    return;
	}
    }
    
    if (swidb->subif_link_type == SUBIF_TYPE_MULTI) {
	if (fr_idb->map_status[dlci] != FR_NOT_MAPPED) {
            printf("\n%%Cannot remove PVC as being referenced by map statement");
	    return;
	} else {
	    fr_pvc->fr_if_set = FALSE;
	    fr_clear_dlci_map(swidb, dlci);
	    fr_pvc_clear(swidb->hwptr, dlci);
	    return;
	}
    }
}

/*
 * fr_switching_idb
 * Return TRUE if DLCI's on this interface should be switched.  This is
 * primarily for external access.
 */

boolean fr_switching_idb (idbtype *idb)
{
    fr_idb_struct_type *fr_idb;

    if (!idb || !fr_switching)
	return(FALSE);
    fr_idb = idb->hwptr->frame_relay_stuff;
    if (!fr_idb || (fr_idb->fr_intf_type == FR_DTE))
	return(FALSE);
    return(TRUE);
}
void fr_encaps_nvram_write (parseinfo *csb)
{
    idbtype *swidb = csb->interface;
    nv_write(TRUE, "encapsulation frame-relay");

    if (swidb->hwptr->frame_relay_stuff->fr_ietf_encaps) {
	nv_add (TRUE, " IETF");
    }
}
void fr_encaps_show_protocol_attr (idbtype *swidb)
{
    printf("\n  Encapsulation FRAME-RELAY");

    if (swidb->hwptr->frame_relay_stuff->fr_ietf_encaps) {
	printf(" IETF");
    }
}


/*
 * fr_tcpip_header_compression_command
 * Config command to enable/disable compression on the interface.
 */
void fr_tcpip_header_compression_command (parseinfo *csb)
{
    hwidbtype *idb;
    idbtype *swidb;

    idb = hwidb_or_null(csb->interface);
    swidb = csb->interface;

    if (!csb->nvgen) {
	if (idb->frame_relay_stuff->fr_ietf_encaps == TRUE) {
	   printf("\n%%\007Interface is set for IETF encapsulation! TCP/IP");
	   printf("\n%%header compression is supported only over CISCO ");
	   printf("\n%%encapsulation! ");
	return;
	}
    }
    if (!csb->sense) {
	fr_header_compression_set(swidb, FALSE, FALSE);
	return;
    }

    if (!fr_header_compression_set(swidb,
	GETOBJ(int,1) == FR_INTF_PASSIVE ? TRUE : FALSE, TRUE)) {
	    printf(nomemory);
	    return;
    }
}

boolean fr_header_compression_set (
    idbtype *swidb,
    boolean passive, 
    boolean sense)
{
    thcdbtype *thcdb = NULL;
    frmaptype *p;
    int i;

    if (sense) {

    	if (passive)
	    swidb->fr_hdr_comp_passive = TRUE;
    	else
	    swidb->fr_hdr_comp_passive = FALSE;

	swidb->fr_tcp_hdr_comp = TRUE;

	for (i = 0; i < RELAYHASH; i++) {
	    for (p = relaytable[i]; p; p = p->next) {
		if (p->idb != swidb)
		    continue;
		if (p->encaps_type == FR_ENCAPS_IETF)
		    continue;
		if (p->link != LINK_COMPRESSED_TCP  && p->link != LINK_IP
				&& p->link != LINK_ILLEGAL)
		    continue;
		if ((p->link == LINK_IP) && 
				(p->compression_flags & FR_MAP_NO_COMPRESS)) 
		    continue;
		if (p->map_type == FR_MAP_DYNAMIC)
		    continue;

		if (p->link == LINK_ILLEGAL) {
		    p->compression_flags |= FR_MAP_INHERIT_COMPRESSION;
		    if (passive)
			p->compression_flags |= FR_MAP_INHERIT_PASSIVE;
		} else if (p->link == LINK_IP) {
		    p->compression_flags |= FR_MAP_INHERIT_COMPRESSION;
		    if (!(p->compression_flags & FR_MAP_PASSIVE)) {
		    	if (passive && 
				!(p->compression_flags & FR_MAP_ACTIVE))
		    	    p->compression_flags |= FR_MAP_INHERIT_PASSIVE;
			else {
		   	    p->compression_flags &= ~FR_MAP_PASSIVE;
			    p->compression_flags &= ~FR_MAP_INHERIT_PASSIVE;
		   	}
		    }
		}
		thcdb = (thcdbtype *)p->tcp_compress;
		if (thcdb) {
		    /*
		     * Compression is already configured for this PVC.
		     * Just update the "passive" option.
		     */
		    if (p->compression_flags & FR_MAP_PASSIVE || 
			p->compression_flags & FR_MAP_INHERIT_PASSIVE)
		    	thcdb->flags &= ~(THC_PERMACTIVE|THC_ACTIVE);
		    else 
		    	thcdb->flags |= THC_PERMACTIVE|THC_ACTIVE;
		} else {
		    /*
		     * Compression is NOT configured for this PVC.
		     * Initialize the compression structure, set flags.
		     */
		    p->tcp_compress =
				malloc_named(sizeof(thcdbtype), "FR TCP
Header Compression");
		    if (!p->tcp_compress)
			return(FALSE);
		    thcdb = (thcdbtype *)p->tcp_compress;
		    reg_invoke_hdr_compress_init(LINK_IP,
				thcdb, DEFAULT_THC_STATES);
		    if (p->compression_flags & FR_MAP_PASSIVE || 
			p->compression_flags & FR_MAP_INHERIT_PASSIVE)
		    	thcdb->flags &= ~(THC_PERMACTIVE|THC_ACTIVE);
		    else 
		    	thcdb->flags |= THC_PERMACTIVE|THC_ACTIVE;
		}
	    }
	}
	reg_invoke_bump_cache_version(swidb->hwptr, NULL, FALSE);
    } else {
	swidb->fr_tcp_hdr_comp = FALSE;
	swidb->fr_hdr_comp_passive = FALSE;
	for (i = 0; i < RELAYHASH; i++) {
	    for (p = relaytable[i]; p; p = p->next) {
		if (p->idb != swidb)
		    continue;
		if ((p->link != LINK_IP) && (p->link != LINK_ILLEGAL))
		    continue;
		if (p->encaps_type == FR_ENCAPS_IETF)
		    continue;
		if (p->map_type == FR_MAP_DYNAMIC)
		    continue;
		p->compression_flags &= ~FR_MAP_INHERIT_PASSIVE;
		p->compression_flags &= ~FR_MAP_INHERIT_COMPRESSION;
		thcdb = (thcdbtype *)p->tcp_compress;
		if (thcdb) {
		    free(p->tcp_compress);
		    p->tcp_compress = NULL;
		}
	    }
	}	
    }
return(TRUE);
}

/*
 * fr_init_encaps_idb
 * Initialize frame relay related data structure in swidb
 */
void fr_init_encaps_idb(idbtype *swidb, boolean setup_hwidb)
{
    hwidbtype *idb;
    fr_idb_struct_type *fr_idb;
    boolean arp_status;
    
    idb = swidb->hwptr;
    fr_idb = idb->frame_relay_stuff;

    arp_status = TRUE;
    reg_invoke_fr_arp_int(FR_ARP_INT_SET, swidb, FR_CISCO_ARP, &arp_status);
    reg_invoke_fr_arp_int(FR_ARP_INT_SET, swidb, FR_INARP, &arp_status);
    reg_invoke_media_enctype_setup(swidb, ET_FRAME_RELAY);
    swidb->ip_enctype = ET_FRAME_RELAY;
    if (is_first_swidb(swidb))
	swidb->ip_split_disabled = swidb->ip_split_disabled_def = TRUE;
    swidb->ip_routecache_same = swidb->ip_routecache_same_def = TRUE;
    swidb->fr_tcp_hdr_comp = FALSE;
    swidb->fr_hdr_comp_passive = FALSE;
    if (swidb->span_ptr) {
	reg_invoke_bridge_circuit_remove(swidb);
	fr_idb->fr_bridge = TRUE;
	if (setup_hwidb)
	    idb->span_bridge_process_force |= SPAN_BR_PROC_FR_BRIDGE;
    }
    reg_invoke_fr_idb_class_alloc(swidb);
}    

/* 
 * fr_wait
 * Wait for specified amount of time
 */
void fr_wait(int seconds)
{
    process_sleep_for(ONESEC * seconds);
}
void fr_debug_pak (paktype *pak, char *string)
{
  short *temp1;
  int i, size;

  buginf("\n%s", string);
  buginf("\n\tdatagramstart 0x%x, size %d, network_start 0x%x\n\trefcount %d linktype %s, pak=%x", 
	pak->datagramstart, pak->datagramsize, pak->network_start,
	pak->refcount, link2name(pak->linktype), pak);
  buginf("\n");
  size = pak->datagramsize;
  temp1 = (short *)pak->datagramstart;
  /* convert short to char */
  if (size > 128)
      size = 128;
  size = size >> 1;
  for (i = 0; i < size; i++, temp1++)
  {
      buginf(" 0x%04x", GETSHORT(temp1));
      if ( (i>0) && !((i+1) % 8))
      buginf("\n");
  }
}

/*
 * fr_pr_set_dlci
 * Return the DLCI to be used from a priority DLCI group
*/
ushort fr_pr_set_dlci (paktype *pak,
		       fr_pvc_info_type *fr_pvc,
		       ushort group)
{
    ushort tmp_group;
    int pri;
    hwidbtype *tmp_idb;

    /*
     * Handle the case where priority queuing is also enabled
    */
    tmp_idb = pak->if_output->hwptr;
    tmp_group = tmp_idb->priority_list;
    tmp_idb->priority_list = group;
    pri = priority_classify(tmp_idb,pak);
    tmp_idb->priority_list = tmp_group;
    return(fr_pvc->fr_pr_dlcis[pri]);
}

/*
  * fr_pr_init_dlci_group
 * Set up a priority DLCI group
*/ 
void fr_pr_init_dlci_group (fr_idb_struct_type *fr_idb,
			    fr_pvc_info_type *fr_pvc)
{
    ushort dlci;
    fr_pvc_info_type *tmp_pvc;
    int i;
    
    /*
     * Define DLCI if needed and initialize as part of group
     * High priority DLCI has been initialized prior to this call
     */
    
    for (i = PRIORITY_MEDIUM; i <= PRIORITY_LOW; i++) {
	dlci = fr_pvc->fr_pr_dlcis[i];
	/*
	 * If no DLCI provided replicate the last value given
	 */
	
	if (dlci == 0) {
	    dlci = fr_pvc->fr_pr_dlcis[i-1];
	    fr_pvc->fr_pr_dlcis[i] = fr_pvc->fr_pr_dlcis[i-1];
	}
	if (dlci == fr_pvc->fr_pr_dlcis[PRIORITY_HIGH])
	    continue;

	tmp_pvc = fr_idb->fr_dlci_tab[dlci];
	if (!tmp_pvc) 
	    tmp_pvc = fr_pvc_init(fr_pvc->idb,dlci,PVC_LOCAL,PVC_STATIC,
							FALSE, NULL);
	if (!tmp_pvc)
	    return;
	fr_set_pvc_status(tmp_pvc, DLCI_ADDED);
	tmp_pvc->fr_pr_enabled = TRUE;
	tmp_pvc->primary_dlci = fr_pvc->fr_pr_dlcis[PRIORITY_HIGH];
	tmp_pvc->idb = fr_pvc->idb;
	tmp_pvc->pvc_usage = PVC_LOCAL;
    }

    set_fr_pvc_from_priority_list(fr_pvc, fr_idb);

}

void fr_mapclass_nvgen_func(mapclass_type *class_ptr)
{
    reg_invoke_fr_mapclass_nvgen(class_ptr);
}


  /*
   *  To enable user to filter frame-relay  packets on per interface
   *  or a per dlci basis.
   *
   */
void  fr_debug_command (parseinfo *csb)
{

    debug_command(csb);


    /* most debug command processing is done in debug_command
     * but to allow filtering on a per-interface/subinterface
     * or per dlci basis we must provide additional handling for
     * the commands. 
     */

    switch(csb->which) {
    case DEBUG_FRAME_RELAY_PACKET:
      fr_debug_swidb = GETOBJ(idb,1);
      fr_debug_dlci = GETOBJ(int,1);

      if (fr_debug_swidb) {
        printf ("\nDisplaying packets on interface %s ",
                fr_debug_swidb->namestring);
        if (fr_debug_dlci)
          printf (" dlci %u only",fr_debug_dlci);
        else
          printf ("only");
      }
      break;

    case DEBUG_FRAME_RELAY_LMI:
      if (GETOBJ(idb,1))
          fr_debug_hwidb = GETOBJ(idb,1)->hwptr;
      else
          fr_debug_hwidb = NULL;

      if (fr_debug_hwidb)
        printf ("\nDisplaying lmi data from interface %s only",
              fr_debug_hwidb->hw_namestring);
      else 
        printf ("\nDisplaying all Frame Relay LMI data");
      break;

    default:

   }
}



/*
 * FR subsystem header
 */

#define FR_MAJVERSION 1
#define FR_MINVERSION 0
#define FR_EDITVERSION  1

SUBSYS_HEADER(frame_relay, FR_MAJVERSION, FR_MINVERSION, FR_EDITVERSION,
	      frame_relay_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: tbridge",
	      NULL);
      
