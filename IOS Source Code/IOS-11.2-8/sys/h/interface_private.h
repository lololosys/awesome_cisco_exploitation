/* $Id: interface_private.h,v 3.32.4.37 1996/09/07 22:36:14 jliu Exp $
 * $Source: /release/112/cvs/Xsys/h/interface_private.h,v $
 *------------------------------------------------------------------
 * interface_private.h -- header file for the privateIDB functions
 *			  and structures.
 *
 * January 1995, David S.A. Stine
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: interface_private.h,v $
 * Revision 3.32.4.37  1996/09/07  22:36:14  jliu
 * CSCdi64205:  Unprovisioned T1 plugged into AS5200 causes box to hang
 * Branch: California_branch
 *     o The fix included display warning message to console port and used
 *       SNMP notification to send the message to network management
 *       station
 *
 * Revision 3.32.4.36  1996/08/26  06:48:37  dino
 * CSCdi67098:  Do not accept DVMRP neighbor probes/reports from
 * non-pruners
 * Branch: California_branch
 *
 * Revision 3.32.4.35  1996/08/22  06:00:34  pitargue
 * CSCdi66791:  reserve fci type and idb type for Yosemite
 * Branch: California_branch
 *
 * Revision 3.32.4.34  1996/08/15  02:32:05  hampton
 * Change some function arguments to be 'const'.  [CSCdi66102]
 * Branch: California_branch
 *
 * Revision 3.32.4.33  1996/08/13  02:16:19  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.32.4.32  1996/07/26  00:07:26  hampton
 * Add some accessor routines for various IDB values, and make some
 * existing routines use const values.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.32.4.31  1996/07/23  18:44:48  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.32.4.30  1996/07/18  20:27:38  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.32.4.29  1996/07/08  22:17:10  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.32.4.28  1996/06/28  22:14:47  myeung
 * CSCdi61229:  Trying to turn on OSPF on IRB BVI interfaces crashes router
 * Branch: California_branch
 * - Remove IDB_OSPF_VL, which is not necessary, to preserve status bit
 *
 * Revision 3.32.4.27  1996/06/28  06:03:13  myeung
 * CSCdi61229:  Trying to turn on OSPF on IRB BVI interfaces crashes router
 * Branch: California_branch
 * - Make OSPF recognize BVI
 * - Improve OSPF interface detection so that it will not crash again
 *   when new type of virtual interface is added
 *
 * Revision 3.32.4.26  1996/06/27  09:26:09  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.32.4.25  1996/06/27  01:09:55  mschaef
 * CSCdi61334:  Alignment error, building IPX route cache entry
 * Branch: California_branch
 *
 * Revision 3.32.4.24  1996/06/24  09:20:25  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.32.4.23  1996/06/18  08:17:30  dawnli
 * CSCdi59054:  fddi behaves unstable when using HSRP on fddi
 * Branch: California_branch
 * Delay fddi reset to reduce state flap when multiple HSRP groups
 * are configured on fddi.
 *
 * Revision 3.32.4.22  1996/06/17  08:31:11  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.32.4.21  1996/06/16  21:11:56  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.32.4.20  1996/06/13  19:07:06  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.32.4.19  1996/06/07  20:41:47  fred
 * CSCdi58655:  turning RED off on an interface turns WFQ off on that
 * interface
 *         save default setting and restore it when folks remove RED,
 *         Custom, or Priority Queuing
 * Branch: California_branch
 *
 * Revision 3.32.4.18  1996/05/24  23:33:00  achopra
 * CSCdi56238:  Linear search for glommable VCs in the vc HASH table has
 *         been changed to a binary search in a WAVL tree maintained per
 *         swidb, which is keyed on NSAPs, AAL5 encapsulation and
 *         protocol type if encapsulation is AAL5MUX.
 * Branch: California_branch
 *
 * Revision 3.32.4.17  1996/05/22  20:54:39  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.32.4.16  1996/05/17  11:15:50  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.31.2.15  1996/05/07  21:34:14  jbalestr
 * CSCdi52473:  bsc local-ack fails link for rx frames larger than 888
 * bytes at 2400
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.31.2.14  1996/05/07  05:14:27  gcox
 * Branch: IbuMod_Calif_branch
 * Improve compile error when tangled includes; Improve comments
 *
 * Revision 3.31.2.13  1996/05/05  23:05:14  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.31.2.12  1996/05/02  22:02:06  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.31.2.11  1996/04/29  21:54:27  gcox
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.31.2.10  1996/04/26  15:17:51  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.31.2.9  1996/04/26  07:52:53  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.31.2.8  1996/04/25  19:30:47  ioakley
 * CSCdi53651:  Add support for hidden message-age-increment option in
 *              "source-bridge spanning X" interface command.  Permits
 *              AST interoperability in mixed vendor bridge environments
 *              where hard-coded MAX_AGE defaults are insufficient for
 *              the network diameter and MAX_AGE is not configurable.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.31.2.7  1996/04/11  14:37:29  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.31.2.6  1996/04/03  14:25:38  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.31.2.5  1996/03/21  06:07:06  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.31.2.4  1996/03/19  02:04:01  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Incorporate NetBios session access lists into NetBios subblock
 *
 * Revision 3.31.2.3  1996/03/17  17:47:58  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.31.2.2  1996/03/16  12:08:58  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.31.2.1  1996/03/01  16:30:46  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.32.4.15  1996/05/14  00:11:44  mleelani
 * CSCdi56461:  European cisco MBONE needs ip dvmrp metric-offset out
 * command
 * Branch: California_branch
 * Support outgoing DVMRP metric offsets.
 *
 * Revision 3.32.4.14  1996/05/13  20:08:49  mleelani
 * CSCdi57016:  Multicast helper-maps not functional
 * Branch: California_branch
 * Fix helper-maps.
 *
 * Revision 3.32.4.13  1996/05/10  01:37:29  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.32.4.12  1996/05/09  14:13:21  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.32.4.11  1996/05/04  00:42:21  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR transparent bridging support.
 * - New interafce type: BVI(Bridge-group Virtual Interface).
 * - New fast-switching flavor: FS_BVI.
 * - Introduce idb->tbridge_bridge_linktypes, idb->user_configured_mtu and
 *   pak->irb_rcvd_input.
 *
 * Constrained Multicast Flooding
 * - Declare tbifd_type.
 * - Define the CMF parser link points.
 * - Declare the return_one stub function.
 * - Declare the Transparent Bridging registry.
 * - Add tbridge_cmf to the fastswitch registry.
 *
 * Name Access List
 *
 * Revision 3.32.4.10  1996/04/27  05:47:01  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.32.4.9.4.1  1996/04/27  06:35:22  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.32.4.9  1996/04/19  15:15:22  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.32.4.8  1996/04/17  13:37:33  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.32.4.7  1996/04/04  20:45:00  dino
 * CSCdi51177:  Jhawk wants uptime and syslog of DVMRP tunnel transitions
 * Branch: California_branch
 *
 * Revision 3.32.4.6  1996/04/03  23:18:49  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 * Revision 3.32.4.5  1996/04/02  05:36:13  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.32.4.4.6.3  1996/04/11  00:03:31  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Track manual configuration of MTU on swidb basis
 *
 * Revision 3.32.4.4.6.2  1996/04/08  14:47:43  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.32.4.4.6.1  1996/04/08  01:45:55  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.32.4.4  1996/03/24  00:28:08  ppearce
 * 7000 platform build failure
 * Branch: California_branch
 *   Move definition of "encapstype" back to interface_private.h
 *     to fix 7000 build failure of assembly generated file
 *
 * Revision 3.32.4.3  1996/03/23  22:48:40  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.32.4.2  1996/03/23  01:30:10  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.32.4.1  1996/03/18  19:37:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.32.6.2  1996/03/28  17:24:16  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.32.6.1  1996/03/22  09:35:43  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.32  1996/03/07  06:33:45  kchiu
 * CSCdi40165:  Add netbios session level byte filtering support
 *
 * Revision 3.31  1996/02/21  02:43:19  hampton
 * Clean up platform specific SRB defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.30  1996/02/13  21:24:48  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.29  1996/02/13  08:11:44  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.28.6.3  1996/03/20  01:22:51  sakumar
 * Fixed merge conflicts for Cat5K BIGA idb.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.28.6.2  1996/03/05  06:10:29  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.28.6.1  1996/03/03  21:22:51  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.28.4.1  1996/03/02  01:18:49  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.28.2.1  1996/02/27  20:41:19  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.28  1996/02/09  11:15:58  dstine
 * CSCdi48527:  IOS supports too many interfaces
 *         - set the maximum number of IDB's to 300.
 *
 * Revision 3.27  1996/02/07  22:00:41  raj
 * CSCdi45762:  Warning issued when ip pim nbma-mode configured on PRI
 * links
 * Change message and make it issued for LAN interfaces instead.
 *
 * Revision 3.26  1996/02/04  02:28:53  shaker
 * CSCdi47971:  RSP-3-NORESTART error message
 * Prevent this error from happening. Also enhance error logging
 * for some hard to reproduce error cases.
 *
 * Revision 3.25  1996/01/30  16:03:05  fox
 * CSCdi47721:  Peer IP address supported on non-PPP/SLIP interfaces
 * Check for enctype of PPP or SLIP instead of for point-to-point link
 *
 * Revision 3.24  1996/01/25  20:46:33  ccpoe
 * CSCdi47341:  remove subif_tmp_state from idb. It's no longer used.
 *
 * Revision 3.23  1996/01/23  23:31:37  dko
 * CSCdi47086:  Maximum value for ipx delay cmd is set too high for 2 bytes
 * Fixed the type of novell_delay to ushort.
 *
 * Revision 3.22  1996/01/22  03:54:19  dmccowan
 * CSCdi45007:  LLC2 LCP negotiated over PPP when not wanted
 *
 * Revision 3.21  1996/01/20  19:10:55  lbustini
 * CSCdi38705:  DDR optimum switching for RSP doesnt reset idle timer.
 * Disable RSP fastswitching on DDR interfaces.
 *
 * Revision 3.20  1996/01/18  03:22:24  kchiu
 * CSCdi43616:  Router needs to set express buffer bit on RPS response
 *
 * Revision 3.19  1996/01/12  23:28:58  atsao
 * CSCdi44422:  LAPB N1 command not accepted, N1 falls back to default
 * If user has ever configured LAPB N1 value, make sure this N1 is shown
 * in the show interface output, otherwise, update this N1 value to
 * reflect the current configuration.
 *
 * Revision 3.18  1996/01/11  03:49:43  kao
 * CSCdi43878:  IS_VIP() macro should be used
 *
 * Revision 3.17.4.1  1996/02/29  07:48:13  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.17  1996/01/06  02:55:24  ronnie
 * CSCdi46517:  Cant turn off multiple arp server feature
 *
 * Revision 3.16  1996/01/04  22:14:59  lol
 * CSCdi43469:  per-user route wont use default interface
 *
 * Revision 3.15  1995/12/24  04:53:29  dwong
 * CSCdi44162:  Appletalk doesnt work between ISDN callers
 *
 * Revision 3.14  1995/12/12  07:11:28  dbath
 * CSCdi45276:  4R driver should use new idbtype instead of IDBTYPE_CTR
 * Fix hwang's previous commit which doesn't compile.
 *
 * Revision 3.13  1995/12/12  04:45:34  hwang
 * CSCdi45276:  4R driver should use new idbtype instead of IDBTYPE_CTR
 *
 * Revision 3.12  1995/12/10  23:27:06  rpratt
 * CSCdi45343:  Chassis MIB enhancements for VIP & port adapters
 * Revision 3.17.2.2  1996/02/13  00:18:07  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.17.2.1  1996/01/11  01:44:56  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.11.4.2  1996/01/04  00:37:49  dcheng
 * Delete the boolean flag "vtp_enabled" in the idb structure, clean up the
 * related code. Fix some bugs in the CLI for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 3.11.4.1  1995/12/23  03:58:47  dcheng
 * Branch: LE_Calif_branch
 * Added support for VTP.
 *
 * Revision 3.11  1995/12/01  21:38:14  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.10  1995/12/01  18:37:10  lwei
 * CSCdi44737:  IP multicast does not support administratively scoped
 * boundaries
 *
 * Revision 3.9  1995/11/30  20:38:42  dlobete
 * CSCdi43408:  two fields overlap in sho ip igmp group
 * Add IS_VIP() macro for general use.
 *
 * Revision 3.8  1995/11/30  04:40:59  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.7  1995/11/26  08:08:36  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *              ISL vLAN support for the RSP platform.
 *
 * Revision 3.6  1995/11/24  19:35:27  rbadri
 * CSCdi36121:  Line protocol is up, while LANE client is non-operational
 * subif_line_state variable will track the swidb's line state. It
 * is controlled by the protocols which determine the operational
 * state of swidb.
 *
 * Revision 3.5  1995/11/22  06:56:17  dino
 * CSCdi43574:  mtrace gets native interface instead of tunnel when native
 * not mcast
 *
 * Revision 3.4  1995/11/21  23:10:55  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.3  1995/11/17  09:12:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:24:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:35:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.41  1995/11/08  20:58:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.40  1995/10/23  20:54:10  fox
 * CSCdi42602:  Neighbor routes are lost if routing table is cleared
 * Remove ip_neighbor_address value.  It is a point-to-point interface
 * specific value.
 *
 * Revision 2.39  1995/10/20  19:03:29  asastry
 * CSCdi41453:  Request for new knob: no decnet split-horizon (for frame
 * relay)
 *
 * Revision 2.38  1995/10/13  00:59:33  ioakley
 * CSCdi35149:  Eliminate extraneous generation of AST config_bpdus
 *              within an RSRB disjoint peer context.
 *
 * Revision 2.37  1995/10/10  21:15:33  dino
 * CSCdi41878:  Monitor number of DVMRP routes received and errmsg()
 * during surges.
 *
 * Revision 2.36.2.11.2.1  1996/02/03  07:06:27  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.36  1995/09/25  08:34:18  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.35  1995/09/19  23:55:09  kleung
 * CSCdi38726:  AppleTalk RTMP Stub rtmp_stub feature - only RTMP stubs
 * sent.
 *
 * Revision 2.34  1995/09/18  19:34:34  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.33  1995/09/15  19:22:15  lmercili
 * CSCdi34816:  add dynamic mapping for ipx/smds
 *
 * Revision 2.32  1995/09/15  05:56:48  ppearce
 * CSCdi40438:  Cannot config LLC parms for RSRB/LAK on CSNA virtual i/f
 *
 * Revision 2.31  1995/09/12  21:58:00  pst
 * CSCdi39958:  ip ethernet 802.3 packets bridged when they should be
 * routed
 *
 * Revision 2.30  1995/09/12  18:46:48  gglick
 * CSCdi38693:  lapb disables priority queuing
 *
 * Revision 2.29  1995/09/09  00:51:23  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.28  1995/09/02  10:07:10  fred
 * CSCdi39438:  no fair does not disable WFQ
 *         implement idb->queue_algorithm to indicate the
 *         queuing algorithm deployed.
 *
 * Revision 2.27  1995/08/29  06:01:15  fox
 * CSCdi38536:  peer default ip address pool command does not match
 * documentation
 * Commit changes to mainline in order to match 11.0 Release
 * documentation.
 *
 * Revision 2.26.16.1  1995/09/29  18:45:26  sakumar
 * Added support for vc-per-vp command for the Catalyst 5000 ATM module..
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.26  1995/08/11  20:58:33  ronnie
 * CSCdi38022:  ATM ARP client needs to check for valid NSAP before
 * creating map
 *
 * Revision 2.25  1995/08/09  21:33:11  ppearce
 * CSCdi38300:  Add CSNA packet switching to 75xx (RSP)
 *
 * Revision 2.24  1995/08/09  00:18:58  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.23  1995/08/08  20:18:40  motto
 * CSCdi37835:  SRB fastswitching from FDDI to CIP internal LANs does not
 * work
 *
 * Revision 2.22  1995/08/04  14:08:00  motto
 * CSCdi37535:  CIP LAN SRB explorers multiplying by # of adapters defined
 * per LAN
 *
 * Revision 2.21  1995/08/02  21:38:53  sgannu
 * CSCdi37788:  bridging does not work for flex box with ppp compression
 * Add a new span_bridge_process_force FLAG for bridging over compression.
 *
 * Revision 2.20  1995/07/21  00:35:24  ioakley
 * CSCdi36717:  Ensure that FDDI high end logic is able to detect an
 *              srb/fddi bridge and thereby initialize the control
 *              register for frame stripping.  Prevents explorer storms
 *              caused by unstripped frames running around the FDDI
 *              ring at wire speeds when a wrap of the FDDI ring occurs.
 *
 * Revision 2.19  1995/07/20  19:02:06  ppearce
 * CSCdi37404:  CIP-to-RSRB with TCP/lack not working
 *
 * Revision 2.18  1995/07/19  21:01:51  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Reduce qllc data fields in the idb.  Precursor to a qllc subblock.
 *
 * Revision 2.17  1995/07/12  23:43:51  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.16  1995/07/09 21:04:32  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Introduce explicit, bridge-group specific route/bridge indicators
 * for CRB.
 *
 * Revision 2.15  1995/07/07  22:47:05  hampton
 * Convert DECNET to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi36830]
 *
 * Revision 2.14  1995/07/06 01:31:52  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.13  1995/07/05 22:16:57  gtaylor
 * CSCdi34592:  PPP Reliable Link broken. Changed lapb's attachment to PPP
 * from taking over and saving the idb->iqueue to taking over the
 * parse_packet vector instead. This also fixes bridging problems with
 * reliable link.
 *
 * Revision 2.12  1995/07/05  20:47:59  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.11  1995/07/01 07:46:00  dino
 * CSCdi27921:  Border router needs to send Registers to RP. ISPs want PIM
 *              over DVMRP unicast routing.
 *
 * Revision 2.10  1995/07/01  03:38:19  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.9  1995/06/27 00:28:46  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.8  1995/06/22  15:29:04  motto
 * CSCdi36240:  SRB fastswitching and process switching broken for CIP
 * v-interface
 *
 * Revision 2.7  1995/06/21  03:26:14  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.6  1995/06/20  06:00:25  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * Incorporate code review comments.
 *
 * Revision 2.5  1995/06/16  06:21:17  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.4  1995/06/14  22:53:01  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Pull SMRP fields out of idb and put them into idb sub-block!
 * - Add static idb sub-block type for SMRP.
 *
 * Revision 2.3  1995/06/14  20:39:01  gglick
 * CSCdi30355:  Unexpected LAPB reset under severe traffic condition
 *         Implement receiver busy (RNR) when interface is congested.
 *
 * Revision 2.2  1995/06/08  20:52:04  wmay
 * CSCdi35517: Extra field in swidb- bridge_flood_first ==>
 * tbridge_circuitQ
 *
 * Revision 2.1  1995/06/07  20:35:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __INTERFACE_PRIVATE_H__
#define __INTERFACE_PRIVATE_H__

#ifndef __INTERFACE_GENERIC_H__

/*

This describes the hierarchy of the include files surounding the idb's.

The changes introducing interface_generic.h into this hierarchy have been 
made as part of the continuing modularity effort including idb cleanup.

The goal is to arrive at an interface_generic.h which can be used to 
access the generic internal fields of the idb's.  This will hide the 
current instability associated with the continuing changes to 
interface_private.h, whether these be for the purpose of adding stuff 
to it in the old way, or for cleaning the stuff out of it.

It's now like this, where arrows point from including to included ...


                            interface.h
                                 ^
                                 |
                                 |
                       (interface_types.h)
                           ^           ^
                           |           |
                           |        interface_generic.h
                           |         ^    ^    ^    ^
            interface_private.h      |    |    |    |
              ^    ^    ^    ^       'cleansed' files 
              |    |    |    |       requiring stability
             older files where        of included files
              included files 
               not stable


The purpose of each of these files:

  interface.h
    This maintains its former role as ..
    A bag of declarations associated with idb's but with no view 
      of the struct's.

  interface_types.h
    NOT TO BE INCLUDED BY OTHER FILES
    This contains the ability to mirror the generic first portion 
      of the idbs into each of interface_generic.h and 
      interface_private.h.
    This is currently generated from interface_private.h, and is not 
      to be hand-edited.  The idea of this is to accommodate cvs 
      merges into interface_private.h for an interim period.  
      Eventually, interface_types is planned to stand alone,
      as its whole philosophy is to be independent of 
      interface_private.h.

  interface_generic.h
    This is meant to contain only generic (non hardware- or protocol- 
      specific) stuff.
    This is to achieve some stability for the life of a release, so 
      that those features which are coded on modular principles 
      (eg using idb subblocks) need not include the less stable 
      interface_private.h.  We do not expect to achieve this 
      order of stability in California, but some day ....
    Note that no file may implicitly or explicitly include both 
      interface_generic.h and interface_private.h.  A compile error
      is generated to indicate when this has occurred.

  interface_private.h
    This retains its traditional role, but is in the process of 
      being cleaned up.
    It should eventually shrink to a point where the idb's it 
       defines contain little more than those defined by 
       interface_generic.h.
    Note that it still temporariliy contains all the generic stuff, 
      which is excluded from direct compilation by a series of 
      #ifdef's.  These direct the building of interface_types.h.  
      For this reason, for the interim period until 
      interface_types.h ceases to be machine-generated, maintenance 
      to these generic fields is still done in interface_private.h.
    The tags which bracket the generic parts are 
        interface_types
        hwidb_fields_generic
        swidb_fields_generic
        interface_generic_inlines
        hwidb_fields_ibu
        swidb_fields_ibu
        interface_ibu_inlines
    Code embraced in *ibu* tags is to be deprecated in the medium term.

This approach defines a path towards modularity in the idb world, 
whereby the second step in liberation from interface_private.h 
(after the transition to the use of subblocks) is to use 
interface_generic.h instead of interface_private.h.

A picture of the structs in each of interface_generic.h and 
interface_private.h is presented below.
It shows how the mirroring of the structs works, and shows
why no compilation may see both of these included.

Here, 'generic' describes items which are utilised in a common way by
all protocols and hardwares.  'specific' describes the many and varied
bits of stuff specific to all the different worlds of hardware and
protocol.

interface_private.h:             interface_generic.h:

+--------------------------+     +--------------------------+
|  generic types, defines  |     |  generic types, defines  |
|                          |     |                          |
|  specific types, defines |     |                          |
|                          |     |                          |
|  struct hwidbtype_ {     |     |  struct hwidbtype_ {     |
|                          |     |                          |
|    generic fields        |     |    generic fields        |
|                          |     |                          |
|    specific fields       |     |                          |
|  };                      |     |  };                      |
|                          |     |                          |
|  struct idbtype_ {       |     |  struct idbtype_ {       |
|    generic fields        |     |    generic fields        |
|                          |     |                          |
|    specific fields       |     |                          |
|  };                      |     |  };                      |
|                          |     |                          |
|  generic inlines         |     |  generic inlines         |
|                          |     |                          |
|  specific inlines        |     |                          |
+--------------------------+     +--------------------------+

*/

#ifdef interface_types

#include "linktype.h"
#include "encapstype.h"
#include "packet.h"
#endif interface_types
#include "interface_types.h"
#ifdef interface_types
#include "interface.h"
#include "../os/list.h"
#include "../parser/parser_defs_priority.h"

/*
 * Maximum number of interfaces
 *
 * This constant defines the maximum number of "interfaces" that can be
 * defined in the system.  This includes real hardware, subinterfaces, and
 * virtual IDBs.
 *
 * Interfaces are numbered from 1;  the constant defined below is the last
 * permitted ordinal number *plus one*.  In other words, interface zero is
 * never used, and most of the time there is a wasted bit or word when
 * a table is thus indexed, and the number of usable interfaces is
 * actually *one less* than this value.  Whatever.
 *
 * This is currently set to 300 because while the Core BU builds routers
 * which are supposed to be capable of more than 256 interfaces, the 
 * software isn't supposed to be capable of more than 256 interfaces.
 */

#define MAX_INTERFACES 300		/* Last if_number == 255! */

/*
 * Bandwidth scaling.  The "visible bandwidth" (non-inverse) is counted
 * in Kbps.  This is the value that is configured and displayed.  The
 * "bandwidth" (used by IGRP and others) is effectively counted in units
 * of 100 picoseconds (10^-10) per bit.  Thus, to convert between
 * visible bandwidth and IGRP bandwidth, you divide the value into 10^7
 * (the conversion is symmetric).  The SCALED_BANDWIDTH macro below
 * can be used to convert in either direction.
 *
 * BPS_TO_IGRP_BW is used to convert a value expressed in bits per second
 * into an IGRP inverse bandwidth.  This is used with relatively small
 * bandwidth values (such as found on async lines) in order to avoid
 * the truncation error that would occur if the bps were first divided
 * by 1000.
 */
#define BANDWIDTH_SCALE 10000000	/* for scaling inverse bandwidths */
#define SCALED_BANDWIDTH(bw) (BANDWIDTH_SCALE/(bw)) /*Convert to/from visible*/
#define BPS_TO_IGRP_BW(bps) ((BANDWIDTH_SCALE / (bps)) * 1000)

#define INPUTQ_DEFAULT  75		/* default interface input queue */
#endif interface_types
#define SNIFFLONGS  10                  /* size of sniff buffer */
#ifdef interface_types

/*
 * Definitions for encapsulation types.
 */
typedef ulong encapstype;

#define IS_VIP(hwidb)  (hwidb->status & IDB_VIP)
#define IS_VIP4R(hwidb) (((hwidb)->status & IDB_VIP_4R) == IDB_VIP_4R)
#define IS_ISDN(idb) (idb && idb->isdn_info)

/*
 * Methods for resolving protocol addresses
 */


/*
 * IDBTYPE codes
 * distinct types of interface card.  These are specific types (STR, etc),
 * as opposed to generic types stored in the status word (IDB_ETHER).
 *
 * The first few have numbers hard assigned to them primarily to make sure
 * you read this comment before messing around with them.  The cbus_interrupt()
 * code does a switch on the idb type to decide which fast path to take.  If
 * you do not have all cases from 0 to n, where n is the largest value you
 * want to test, the code is less efficient.  It is always safe to add
 * definitions after the last of the cisco built interface boards.  It is
 * (at present) safe to renumber the hard definitions.
 */

/*
 * Flags in the status word of an idb
 * These are hardware related flags.  No protocol stack flags, please.
 */

# define IDB_MCI	0x00000001	/* hardware is cisco MCI */
# define IDB_VIP        0x00000002      /* VIP HW */
# define IDB_ENCRYPT    0x00000004      /* interface is set for encryption */
# define IDB_C5_ETHER	0x00000008	/* yosemite ethernet interface */
# define IDB_PCBUS      0x00000010      /* Interface for PC (ISA) bus */
# define IDB_LEX	0x00000020	/* type is Lex Interface */
# define IDB_ETHER	0x00000040	/* hdw type is Ethernet/IEEE 802.3 */
# define IDB_SERIAL	0x00000080	/* Some kind of "serial" interface */
# define IDB_CHANNEL    0x00000100      /* IBM channel */
# define IDB_TR		0x00000200	/* hdw type is Token Ring */
# define IDB_RESTART	0x00000400	/* Interface needs to be reset */
# define IDB_NO_NVGEN	0x00000800	/* Don't save interface to NVRAM */
# define IDB_FDDI	0x00001000	/* hdw type is FDDI */
# define IDB_CBUS	0x00002000	/* hdw type is cisco cBus */
# define IDB_HSSI	0x00004000	/* hdw type is hssi serial */
# define IDB_HEARSELF	0x00008000	/* can do hardware loopback */
# define IDB_BCAST	0x00010000	/* interface can do broadcasts */
# define IDB_BVI	0x00020000 	/* Bridge-group Virtual Inteface -
					 * For Integrated Routing & Bridging
					 */
# define IDB_POS        0x00040000      /* distinguish POS from HSSI */
# define IDB_DISABLED	0x00080000	/* interface disabled */
# define IDB_VIRTUAL	0x00100000	/* Interface is a software entity */
# define IDB_BRIDGE	0x00200000	/* Interface supports bridging */
# define IDB_ASYNC	0x00400000 	/* Interface is async line */
# define IDB_VTEMPLATE 	0x00800000      /* Interface is virtual template */
					/* or virtual access */
# define IDB_DELETED    0x01000000	/* Deleted dynamically created i/f, */
					/*  so it shouln't be saved to NVM */
# define IDB_TUNNEL     0x02000000	/* Interface is a virtual tunnel */
# define IDB_SYNC	0x04000000	/* Hardware is synchronous serial */
# define IDB_ISDN	0x08000000	/* Hardware type is ISDN */
# define IDB_NOHARDWARE 0x10000000	/* "vanished" interface (removed */
					/* from system by hot swap */
# define IDB_DIALER     0x20000000	/* Dynamically created dialer */
# define IDB_ATM	0x40000000	/* hdw type is ATM Interface */
# define IDB_VIRTUAL_HW	0x80000000	/* hdw is a virtual channel */

/*
 * Ephemeral interfaces are interfaces that may be marked as deleted or
 * otherwise deconfigured by the user.  They could be virual hardware
 * hardware interfaces, they could be purely software interfaces.  Doesn't
 * matter,  all this class of interfaces means is that you can mark them
 * as deleted and nuke them from the system.
 * Note that ephemeral interfaces are not actually deleted in most cases,
 * rather they are hidden from the user and shut down.  It's a VERY bad
 * idea to free an idb, since there are billions of dangling pointers all
 * over the place.  (You have been warned...)
 */
# define IDB_EPHEMERAL (IDB_ASYNC|IDB_TUNNEL|IDB_VIRTUAL|IDB_DIALER|IDB_VIRTUAL_HW|IDB_LEX)

/*
 * Some idb's don't want to be set up as part of the setup script.
 */
# define IDB_DONTSETUP (IDB_ASYNC|IDB_TUNNEL|IDB_VIRTUAL|IDB_DIALER)

# define IDB_VIP_4R (IDB_VIP|IDB_TR)

# define IDB_IEEEMASK	    (IDB_ETHER|IDB_TR|IDB_FDDI|IDB_LEX)
# define IDB_802ONLY	    (IDB_TR|IDB_FDDI)
/*
 * The bridge reset mask is used to define interfaces where a reset is desired
 * when a bridge changes.  For example, on ethernet interfaces, usually it
 * is desired to put the chip into promiscuous mode.  In general, you do not
 * want to reset serial interfaces.
 */
# define IDB_BRIDGERESET    (IDB_ETHER|IDB_TR|IDB_FDDI|IDB_LEX)


/*
 * Media type for the purposes of transparent bridging.
 *
 * These should go into a tbridge.h and what is in tbridge.h now
 * should go into tbridge_private.h
 */
enum TBR_MEDIA {
   TBR_UNKNOWN,
   TBR_ETHER,
   TBR_FDDI,
   TBR_CTR,		/* MCI/cBus Token Ring */
   TBR_MTR,		/* MultiBus Token Ring */
   TBR_TR,		/* Low-end Token Ring */
   TBR_FDDI_TRANSIT,
   TBR_HDLC_TRANSIT,
   TBR_ETHER_SDE,
   TBR_FDDI_SDE,
   TBR_CTR_SDE,
   TBR_TR_SDE,
   TBR_HDLC_TRANSIT_SDE,
   TBR_LEX_TRANSIT,
   TBR_VBRIDGE,
   TBR_ATM_TRANSIT,
   TBR_LANE_ETHER,
   TBR_FRAME_RELAY_TRANSIT,
   TBR_CPP_TRANSIT,
   TBR_CIP_VLAN,	/* Channel lan emulation (looks */
			/* like ether w/extra header) */
   TBR_SMDS_TRANSIT,
   TBR_ETHER_ISL_VLAN,  /* Bridging out to an Inter Switch Link */
                        /* (ISL) encapsulating vLAN interface */
   TBR_LAST,
   };


/*
 * IP fast flags for ip fast switching, these are a performance consideration,
 * it is faster to check one (1) short word than look at the booleans
 * ip_*_accesslist and ip_accounting.
 */
#define IP_FAST_ACCESSLISTS		0x0001
#define IP_FAST_ACCOUNTING		0x0002
#define IP_FAST_INPUTACCESSLISTS	0x0004	/* enabled on ANY idb on box */
#define IP_FAST_POLICY			0x0008  /* policy enabled i/f */
#define IP_FAST_RSP_DDR			0x0010  /* DDR enabled RSP i/f */

/*
 * IP route cache flags (idb->ip_routecache). Really bit positions.
 */
# define IP_FAST_ROUTECACHE		0x00000001
# define IP_CBUS_ROUTECACHE		0x00000002
# define IP_SSE_ROUTECACHE		0x00000004
# define IP_FLOW_ROUTECACHE		0x00000008
# define IP_OPTIMUM_ROUTECACHE		0x00000010
# define IP_DISTRIBUTED_ROUTECACHE	0x00000020

# define IP_DEFAULT_ROUTECACHE		IP_FAST_ROUTECACHE


/*
 * Output queue priority classes.  If you change PRIORITY_MAXTYPE, make
 * sure that hwidbtype is still longword aligned!
 */

#define PRIORITY_QUEUE_INC(i)           ((i & (PRIORITY_MAXTYPE - 2)) + 1)
#define DEFAULT_QUEUE(idb) (idb->custom_queuenum ? 1 : PRIORITY_NORMAL)


/*
 * Fast switching encapsulation types.
 *
 * Used in the fastswitch jump table and also for output demultiplexing
 * in the fastswitch paths that use IDBs
 */

enum FSTYPE {
    FS_UNKNOWN =	0,		/* Default Unknown Type */
    FS_ETHER =		1,		/* Ethernet - arpa encap */
    FS_FDDI =		2,		/* FDDI - snap encap */
    FS_TOKEN =		3,		/* Token ring - snap encap */
    FS_HDLC =		4,		/* Serial - hdlc encap */
    FS_FR =		5,		/* Serial - frame relay */
    FS_SMDS =		6,		/* Serial - SMDS */
    FS_PPP =		7,		/* Serial - PPP */
    FS_SLIP =           8,              /* Serial - SLIP */
    FS_AIP =		9,		/* AIP */
    FS_CHANNEL =        10,             /* CIP */
    FS_LEX =		11,		/* Lex fastswitching */
    FS_ATM_DXI =	12,		/* atm-dxi */
    FS_NULL =		13,		/* null0 */
    FS_TUNNEL =         14,             /* Tunnels */
    FS_DIALER =		15,		/* DDR */
    FS_BVI =            16,             /* Bridge-group Virtual Inteface */
    FS_C5_ETHER =       17,             /* Yosemite psuedo ether */
    FS_LAST =		18		/* one greater than last */
};


/*---- Poll protocol roles. */

#define PP_ROLE_UNDEFINED    0x00
#define PP_ACTIVE            0x01  /* PP is configured */
#define PP_ASCII             0x02  /* default EBCDIC */
#define PP_CHARINT           0x04  /* Define character interrupt mode. */

/*
 * span_bridge_process_force FLAGS
 * These flags are set for a media and force a bridged
 * datagram to go to process level.
 */

#define SPAN_BR_PROC_PRIORITY_QUEUE	0x00000001
#define SPAN_BR_PROC_X25_BRIDGE		0x00000002
#define SPAN_BR_PROC_FR_BRIDGE		0x00000004
#define SPAN_BR_PROC_DIALER		0x00000008
#define SPAN_BR_PROC_VIRTUAL_IDB	0x00000010
#define	SPAN_BR_PROC_SMDS_BRIDGE	0x00000020
#define SPAN_BR_PROC_PPP_BRIDGE         0x00000040
#define SPAN_BR_PROC_ATM_BRIDGE		0x00000080
#define SPAN_BR_PROC_MLAPB_BRIDGE       0x00000100
#define	SPAN_BR_PROC_COMPRESSION	0x00000100

/*
 * Bit definitions for idb->oir_flags
 */

#define EOIR_RESET	0x0001	/* call to idb->reset is EOIR-style "reset" */
#define IDB_NEEDS_INIT	0x0002	/* newly inserted IP, needs <proto>idb_init */

extern queuetype *swidbQ;	    /* head of list of IDB's */
extern queuetype *hwidbQ;	    /* head of list of hardware IDBs */
#endif interface_types

/******************************************************************w****
 *
 * IDB deletion notification blocks. These hold the pointer to
 * the function to be called as notification, an optional pointer to 
 * an arugment and an optional pointer to a debugging string.
 */
typedef boolean (*notify_func_t)(void *, void *);

typedef struct idb_deletion_notify_ {
    struct list_element_ l_elt;		/* Full list element for insertion */
    notify_func_t notify;		/* function to be called */
    void	*user_ptr;		/* pointer to user's block */
    uchar	*user_str;		/* pointer to option string */
} idb_notify_t;

/***********************************************************************
 *
 * IDB Sub-block datatypes. These are used to implement a registry
 * for the users of the IDB who need to keep private information directly
 * associated with the IDB (either s/w or h/w IDB).
 ***********************************************************************
 */
/*
 * Statically allocated sub-block types.
 */
typedef struct idb_sb_ {
    void	*user_ptr;		/* pointer to user's block */
    void	*idb_ptr;		/* pointer to IDB to which attached */
    ushort	idb_type;		/* type of IDB that this is from */
    ushort	type;			/* type (useless for static blocks) */
    ulong	usage;			/* usage count */
} idb_sb_t;

/*
 * Dynamically allocated sub-block types.
 */
typedef struct idb_sb_dyn_ {
    struct list_element_ l_elt;		/* Full list element for insertion */
    struct idb_sb_ 	 sb;		/* And the actual payload */
} idb_sb_dyn_t;
#ifdef interface_types

/*
 * Hardware Interface Description Block.
 *
 * PLEASE keep this long word aligned for speed considerations. Add padding
 * if you must to get 32 bit variables stored on long word boundaries.
 *
 * This data structure occurs once per hardware interface. It contains all
 * hardware-oriented fields, encapsulation info, function vectors, etc.
 * Basically, anything that it doesn't make sense to replicate for sub-
 * interfaces.
 *
 * These are all threaded on the hardware IDB queue (hwidbQ).  Please use
 * the macros defined below to access this queue.
 *
 * Interface function vectors.
 *****************************
 *
 *******************
 * Old IDB Vectors *
 *******************
 * *-------------------------------*
 *   Encapsulation/Deencap Vectors
 * *-------------------------------*
 *
 *     (*vencap)(paktype *pak, long addr) [required]
 *         Encapsulate the packet destined for addr assuming protocol.  Does
 *         not assume anything about the fixed encapsulations but does its
 *         encapsulation thing based on where the datagram currently starts.
 *         That is both datagramstart and datagramsize are considered to be
 *         valid. Returns true if the packet was successfully encapsulated.
 *
 *	This should now be the primary encapsulation vector and must
 *	be implemented by all drivers.
 *
 *     (*bridge_vencap)(paktype *pak, long addr) [required]
 *         Encapsulate the packet destined for addr assuming that the
 *         interface is configured for encapsulation bridging, and the
 *         destination address is in the bridge cache.  Does not assume
 *         anything about the fixed encapsulations but does its
 *         encapsulation thing based on where the datagram currently
 *         starts.  That is both datagramstart and datagramsize are
 *         considered to be valid. Returns true if the packet was
 *         successfully encapsulated.  This routine should only ever be
 *         called from the routines that are called by the vencap vector.
 *
 *     (*getlink)(paktype *pak) [required]
 *         Return the upper layer protocol link code (its link type) for
 * 	   the packet.  This is an internal code that defines what protocol
 * 	   information is contained in the packet.  Returns the link type
 * 	   if it can be be determined; otherwise returns LINK_ILLEGAL.
 *     (*extract_hwaddr)(hwidbtype *idb, hwaddrtype *dest) [required]
 *         Extract the interface address into a hwaddr structure.  This
 *	   function combined with the nex provide for an easy means of
 *	   comparing a packet source address to interface addresses.
 *	   The address is returned in canonical form (except for TR).
 *     (*extract_snpa)(paktype *pak, hwaddrtype *dest, enum SRCDST which) [required]
 *         Extract the source or destination address from this packet.
 *	   The address is returned in canonical form (except for TR).
 *     (*parse_packet)(hwidbtype *idb, paktype *pak, uchar *) [required]
 *         Incoming frame parser for this interface type.
 *     (*broadcast)(paktype *pak) [required]
 *         Determine if this packet contains a physical broadcast for the
 * 	   media.  Returns TRUE or FALSE.
 *     (*deencap)(hwidbtype *idb) [optional]
 *         Perform any work needed to undo encapsulation.
 *
 * *------------------------*
 *   Input/Output Vectors
 * *------------------------*
 * 
 *     (*soutput)(hwidbtype *idb) [required]
 *         Start output up on the interface.  Outgoing packets are enqueued
 *         on the holdq.
 *     (*sinput)(hwidbtype *idb) [optional]
 *         Start input up on the interface.  Incoming packets are processed
 *         and either queued or fast switched.  This is mainly used by the
 *         CSC-R16 driver which supports two different memory models and
 *         needs to quickly decide which is which.
 *     (*oqueue)(hwidbtype *idb, paktype *pak, enum HEADTAIL which) [required]
 *         Enqueue packet onto the interface's holdq for output.
 *     (*oqueue_dequeue)(hwidbtype *idb) [required]
 *         Dequeue packet from the interface's holdq for output.
 *     (*iqueue)(hwidbtype *idb, paktype *pak) [required]
 *         Enqueue an incoming pak.  The queue that it is enqueued upon is
 *         determined by the routine.
 *
 * *--------------------------------*
 *   Configuration-Setting Vectors
 * *--------------------------------*
 *     (*setencap)(parsetype *csb) [required]
 *         Set the encapsulation for the interface.  This vector is used by
 *         the configuration software.
 *     (*enable)(hwidbtype *idb) [optional]
 *         Enable an interface that has been shut down for input congestion.
 *         Note that this must be called with NETS_DISABLED to work correctly.
 *     (*make_snpa_native)(hwidbtype *idb, uchar *addr) [optional]
 *	   Convert SNPA from canonical to the form the device driver uses to
 *	   transmit/receive this address.  (Currently no translation for TR).
 *     (*setup_bridging)(hwidbtype *idb) [optional]
 *	   Setup bridging for this interface. If defined, this routine should
 *         be called whenever we change change a bridging structure used by
 *         the device drivers. This routine is expected to respect the
 *         following flags to properly initialize the interface for bridging:
 *              bridge_ip, bridge_decnet, bridge_xns, bridge_clns,
 *              bridge_atalk, bridge_novell, and bridge_apollo.
 *     (*set_extra_addrs)(hwidbtype *idb) [optional]
 *         Install group and functional addresses.  This is preferred over
 *         the reset vector as it is a lot less drastic.  It also take a lot
 *         less time with the token ring chipset.  (Immediately vs 20 sec.)
 *     (*set_maxdgram)(hwidbtype *idb, size) [optional]
 *         Set the maximum datagramsize for this interface to 'size' bytes
 *         and reset the interface to make this change take effect.
 *         Return TRUE if the datagram size is set successfully, FALSE if not.
 *     (*clear_support)(idbtype *i) [optional]
 *         clear hardware specific counters for an interface.
 *         initially just BRI and BSI FDDI use this.
 *     (*system_configured)(idbtype *) [optional]
 *         Called once the NVRAM has been parsed and the system is considered
 *         to be "configured". Interfaces that actually only begin to startup
 *         after NVRAM has been parsed can use this vector to begin full
 *         initialization (e.g. Token Ring and FDDI).
 *
 * *--------------------------------*
 *   Configuration-Reading Vectors
 * *--------------------------------*
 *     (*can_mcast)(hwidbtype *hwidb, enum LINK link) [optional]
 *         Determine if interface supports and is configured with any sort of
 *	   physical multicast capability for the passed in linktype.  Returns
 *	   TRUE or FALSE.
 *     (*show_support)(hwidbtype *idb) [optional]
 *         Set a helper routine for the show command.  This allows the
 *	   interface specific code to live in the interface specific file, and
 *	   cleans up a large set of if-then-else statements.
 *     (*txqlength)(hwidbtype *idb) [optional]
 *         Return the number of active buffers in the board level transmit
 *         queue, ring, etc.
 *     (*max_proto_mtu)(idbtype *idb, enum LINK linktype, boolean default) 
 *                                                                  [optional]
 *         Return the max MTU for the network protocol in question for this
 *         interface, taking into account any additional encapsulation
 *         overhead.  If default is TRUE, returns the default MTU instead of
 *         the max.
 *
 * *--------------------------------*
 *  State-Changing Vectors
 * *--------------------------------*
 *
 *     (*reset)(hwidbtype *idb) [required]
 *         Reset an interface and bring it back up.  For interfaces that do
 *         not handle their own idb->state variable it simply resets the
 * 	   interface.  For interfaces that handle their own idb->state
 * 	   variable it resets the interface and tries to bring it back up.
 * 	   If successful it will set idb->state to IDBS_UP.  This routine
 * 	   does not honor IDBS_ADMINDOWN.
 *	   This routine needs to understand EOIR, if the driver will be
 *	   used on an EOIR capable platform, like the 7000 or 7500.
 *     (*shutdown)(hwidbtype *idb, int advise_state) [optional]
 *         Do what ever it takes to shut an interface down.  advise_state is
 *         a hint about what state the caller would like the interface to be
 *         placed in.  It does not have to be honored so that if an interface
 *         must go into a particular state on shutdown it is allowed to do so.
 *         If an interface has a shutdown vector defined it MUST handle its
 *         own idb->state changes on reset.
 *     (*lineaction)(hwidbtype *idb) [optional]
 *         After interface state has changed, call this routine from process
 *         level to perform line protocol dependent actions (cache cleanups,
 *         bring up LAPB, etc).
 *	   Used by: AIP (aip_cstate), SDLC (sdlc_cstate), TR (tring_cstate),
 *		    slip, FR (fr_cstate), ISDN (isdn_sw_cstate),
 *	            HDLC serial (same as hdlc_cstate), PPP (ppp_cstate)
 *		    SMDS (smds_cstate), LAPB (lapb_cstate)
 *     (*linestate)(hwidbtype *idb) [required]
 *         Return TRUE/FALSE as to whether the line protocol is up/down.
 *     (*periodic)(hwidbtype *idb) [optional]
 *         Perform various periodic functions that are needed by an interface.
 *         Warning: This vector may be modified by encapsulation routines.
 *         This routine currently is called once every 10 secs.
 *     (*device_periodic)(hwidbtype *idb) [optional]
 *         Perform various device dependent background tasks.
 *         Only the device driver is allowed to modify this vector.
 *
 *******************
 * New IDB Vectors *
 *******************
 *
 * Encapsulation Vectors:
 *	vencap
 *	bridge_vencap
 *
 *	getlink
 *	extract_snpa
 *	parse_packet
 *	broadcast
 *	deencap
 *
 * IO Vectors
 *	soutput
 *	sinput
 *	oqueue
 *	iqueue
 *
 * Configuration Vectors, setting
 *
 *	setencap
 *	enable
 *	make_snpa_native
 *	setup_bridging
 *	set_extra_addrs
 *	set_maxdgram
 *	clear_support
 *	system_configured
 *
 * Configuration Vectors, reading
 *
 *	can_mcast
 *	show_support
 *	txqlength
 *	max_proto_mtu
 *	extract_hwaddr
 *
 * State-Changing Vectors
 *
 *	reset
 *	shutdown
 *	lineaction
 *
 * State-Reading Vectors
 *
 *	linestate
 *
 * Periodic CPU time vectors
 *
 *	periodic
 *	device_periodic
 */

/* Prototypes for IDB vectors */

typedef boolean (*vencap_t)(paktype *, long);
typedef boolean (*bencap_t)(paktype *, hwidbtype *);

typedef int (*cbencap_t)(hwidbtype *,void *,int);
typedef uchar * (*bridge_vencap_t)(paktype *, long);
typedef void (*soutput_t)(hwidbtype *);
typedef void (*sinput_t)(hwidbtype *);
typedef boolean (*oqueue_t)(hwidbtype *, paktype *, enum HEADTAIL);
typedef paktype * (*oqueue_dequeue_t)(hwidbtype *);
typedef void (*iqueue_t)(hwidbtype *, paktype *);
typedef iqueue_t (*parse_packet_t)(hwidbtype *, paktype *, uchar *);

typedef void (*reset_t)(hwidbtype *);
typedef void (*shutdown_t)(hwidbtype *, int);
typedef boolean (*extract_hwaddr_t)(hwidbtype *, hwaddrtype *);
typedef boolean (*extract_snpa_t)(paktype *, hwaddrtype *, enum SRCDST);
typedef void (*make_snpa_native_t)(hwidbtype *, uchar *);
typedef fuzzy (*broadcast_t)(hwidbtype *, paktype *);
typedef boolean (*can_mcast_t)(hwidbtype *hwidb, enum LINK link);
typedef void (*setencap_t)(parseinfo *);
typedef void (*deencap_t)(hwidbtype *);
typedef void (*lineaction_t)(hwidbtype *);
typedef boolean (*linestate_t)(hwidbtype *);
typedef void (*periodic_t)(hwidbtype *);
typedef void (*device_periodic_t)(hwidbtype *);
typedef void (*enable_t)(hwidbtype *);
typedef void (*listen_t)(idbtype *, uchar *, uchar);
typedef void (*listenrange_t)(idbtype *, uchar *, uchar *);
typedef boolean (*add_hwaddr_dynamic_t)(idbtype *, uchar *);
typedef void (*setup_bridging_t)(hwidbtype *);
typedef boolean (*set_extra_addrs_t)(hwidbtype *);
typedef boolean (*set_maxdgram_t)(hwidbtype *, int, int);
typedef void (*show_support_t)(hwidbtype *);
typedef boolean (*show_controller_t)(hwidbtype *, parseinfo *);
typedef int (*txqlength_t)(hwidbtype *);
typedef int (*max_proto_mtu_t)(idbtype *, enum LINK, boolean);
typedef void (*clear_support_t)(hwidbtype *);
typedef void (*system_configured_t)(hwidbtype *);
typedef void (*fastsend_t)(hwidbtype *, paktype *);
typedef	boolean (*tr_srb_fastswitch_type)(hwidbtype *, void *, hwidbtype *);
typedef void (*dot5_void_type)(hwidbtype *);
typedef boolean (*dot5_boolean_type)(hwidbtype *);
typedef void (*show_serial_state_t)(hwidbtype *, char *);
typedef void (*afilter_reset_t)(hwidbtype *);

typedef boolean (*fs_explorer_t)(paktype *, fs_hdrtype *, hwidbtype *);
typedef boolean (*fs_vbridge_t)(paktype *, hwidbtype *);
typedef int (*if_srb_fastout_type)(hwidbtype *, paktype *);
typedef boolean (*if_srb_les_fastout_type)(hwidbtype *, hwidbtype *, paktype *,
					   int);
typedef	boolean (*if_srb_hes_fastout_type)(hwidbtype *, hwidbtype *, uchar *,
					   int, ulong *, int, hwidbtype *);
typedef int (*rsrb_fst_fastout_type)(tr_vpeertype *, tr_ventrytype *,
				     ushort, void *, int,
				     ipcache_rntype *, hwidbtype *);

typedef int (*ip_turbo_fs_t)(void *, void *, hwidbtype *);

typedef void (*rx_char_int_t)( hwidbtype *idb, uchar input, boolean overrun);
typedef void (*rx_framer_int_t)(hwidbtype *idb, paktype **frame);
typedef void (*tx_comp_notify_t)( hwidbtype *idb );
typedef int  (*rx_in_progress_t)(hwidbtype *idb );
typedef void (*restart_rx_t)(hwidbtype *);
typedef void (*disable_rx_t)(hwidbtype *); 
typedef paktype* (*getsysbuf_t)(hwidbtype *);
#endif interface_types

struct hwidbtype_ {          /*******************************/
                             /*                             */ 
  HWIDB_FIELDS_GENERIC       /*      These statements       */ 
                             /*        must be kept         */ 
  HWIDB_FIELDS_IBU           /*          together           */ 
                             /*                             */ 
                             /*******************************/ 
#ifdef hwidb_fields_generic

        struct hwidbtype_ *next;    /* pointer to next hardware IDB in queue */

	/* 
	 * Fast switching non array variables accessed at interrupt level.
	 * Keep the following fields which are touched by the device
	 * drivers, decision layer, and fast switching code together right
	 * after the 'next' pointer at the start of the structure to
	 * minimize how many cache lines we touch.
	 */

	/*
	 * Caution:  The units of the following field vary depending
	 * on platform.  The 7000 keeps a WORD offset in this field.
	 * The RSP keeps a BYTE offset in this field.  The IP ucode on
	 * these two platforms expect WORDS and BYTES respectively.
	 */
	short		rxoffset;	/* offset from zero of RX buffer */

	ushort		ip_fast_flags;	/* used by fast switching support */

	struct idbtype_	*firstsw;	/* pointer to first software IDB */
	enum IDBTYPE    type;	        /* specific type of interface */

	ulong		fast_switch_flag;

	fastsend_t	fastsend;	/* interface fastsend routine */

	qa_hwq_t	*tx_q_ptr;	/* ptr to transmit hw Q */
	qa_acc_t	*tx_acc_ptr;	/* ptr to transmit limit accumulator */

	boolean	tbridge_on_hwidb;	/* TRUE if transparently bridging */
					/* on ANY sfw idb on this hw idb */
	short		circ_group;	/* cct group # for load distribution */
	short		circ_canforward;/* TRUE if group is PORT_FORWARDING */

	ip_turbo_fs_t	ip_turbo_fs;	/* IP turbo fs vector */
	ulong           ip_memd_offset; /* for rsp_ipflow_fs */
	enum FSTYPE fast_switch_type;	/* fastswitching media/encap flavor */
	ulong		max_pak_size;	/* Max packet size for media */
        void            *crypto_vectors;

	/*
	 * End of HW IDB variables touched by the network interface device
	 * driver, decision layer, fast switch routing code, and fast
	 * bridging code at interrupt level.
	 */
	ulong		max_pak_size_def; /* default max pak size */
  	struct hwidbtype_ *next_idb_on_physint; /* pointer to next hardware 
  				           IDB in a linked list of interfaces 
  				           on the same physical port. */
	int unit;		    /* interface unit number */
	uint hw_if_index;	    /* hardware interface index (1-n)  */
	uint snmp_if_index;	    /* snmp i/f index if registered, else 0 */
	int fci_type;	            /* the FCI_TYPE_foo of the interface */

	int vc;			    /* hardware virtual channel number */
enum { NO_VC = -1 };        
        ulong slot;                 /* slot number */
        ulong subunit;              /* subunit number */
        ulong subcont;              /* NIM sub controller number */
	char *name;		    /* pointer to device type name */
	char *typestring;	    /* Interface hardware type */
	char *hw_namestring;	    /* copy of firstsw->namestring */
	char *hw_short_namestring;  /* copy of firstsw->short_namestring */
	uchar bia[8];		    /* the manufacturer assigned MAC address */
	uchar hardware[8];	    /* the def/cur hardware address (MAC) */
	uchar forced_mac[8];	    /* forced mac if applicable (config) */
	ushort maxmtu;		    /* largest possible packet for IP (and
				     * similarly encapsulated protocols) */
	ushort maxmtu_def;          /* default maximum MTU value */
	ushort max_buffers;         /* maximum maxmtu sized buffers (cbus) */
	tinybool hw_enabled;        /* h/w interface enabled */
        tinybool newproto;          /* next state of the lineprotocol */

	ulong ALIGN_4(max_buffer_size); /* maximum buffer size needed to hold
					 * a packet (mtu + encaps + FCS) */
	ulong buffersize;           /* maximum size minus overhead */
	struct pooltype_ *pool;     /* pointer to buffer pool for interface */
	int pool_group;             /* group num of pool for interface */
	struct pooltype_ *headerpool; /* pointer to header pool for i/f */
	ulong status;		    /* hardware status bits */
	enum IDB_STATES state;	    /* current interface state */
	enum IDB_STATES oldstate;    /* state when carrier timer set */
        int newfddistat;	    /* status of cfm on fddi interfaces */
	sys_timestamp lastinput;    /* time of last input */
	sys_timestamp lastoutput;   /* time of last output */
	sys_timestamp lastreset;    /* time of last reset for output failure*/
	sys_timestamp state_time;   /* and when it was last changed */
	mgd_timer carriertimer;     /* timer for state change damping */
	ulong carrierdelay;  	    /* dwell time for carrier state changes */
	ulong carrierdelay_def;	    /* default time for carrier state change */
	char *reason;		    /* reason for last failure */
	void *devctl;		    /* ptr to device control registers */
	struct remif_* remif;	    /* remote interface control block */

	encapstype enctype;	    /* current encapsulation type */
	encapstype enctype_cfg;	    /* configured default encapsulation */
	encapstype enctype_def;	    /* default encapsulation type for media */
	int encsize;	  	    /* Sizeof the encapsulation area(BYTES) */
	int start_offset;	    /* Offset into data region of packet */
	tinybool phoney_rxtype;    /* Set true only if not supporting FS */
	tinybool inloopback;        /* interface actually is looped */
	tinybool loopback;	    /* interface is marked loopback */
	tinybool reset_request;     /* reset is requested */
	ushort ALIGN_2(ext_loop_type); /* special loop modes (HSSI and MIP) */
#endif hwidb_fields_generic

/*
 *  Here continues the hwidbtype_ struct with fields invisible 
 *  to interface_generic.h:
 */
	/*
	 * IDB user Sub-block area.
	 *
	 * The pre-allocated array is for those users of the HW IDB
	 * who we know about a priori, the dynamic list is for those
	 * who we don't know about yet.
	 */
	struct {
	    idb_sb_t		sb[HWIDB_SB_MAX];
	    list_header		sb_dynamic;
	    ulong		sb_dynamic_id;
	} subblock;
#ifdef hwidb_fields_generic
	/*
	 * Control vector. This is called with a pointer to ourself,
	 * a function opcode and a list of items to manipulate.
	 */
	hwidb_state_config_t	state_config;

	short keep_period;	    /* seconds between keepalive actions */
	short keep_count;	    /* seconds until next keepalive action */
	tinybool nokeepalive;	    /* TRUE if keepalive disabled */
	tinybool keep_current_up;   /* net_periodic() idea of line protocol */
	tinybool keep_last_up;      /* previous line protocol state */
	tinybool keep_transition;   /* was there a transition ? */
	uchar load;		    /* load (IGRP) */
	uchar reliability;	    /* reliability coefficient */
	ushort datarate_interval;   /* index for load calculation */
	sys_timestamp xmittimer;    /* timer for output */

	fair_queue_head *fair_queue_head; /* fair head data structure */
	ushort queue_algorithm;		/* queuing used on the interface */
	ushort old_queue_algorithm;	/* queuing used on the interface */
	ushort priority_list;       /* list # if priority output queuing */
	ushort custom_queuenum;     /* Current queue # if custom queuing */
	ushort output_qcount;       /* size of all priority output queues */
	ulong custqremain[PRIORITY_MAXTYPE]; /* bytes to send w/ customq */
	short old_txcount;	    /* RSP - last installed txq count */
	red_parameters *red_parameters;	/* parameters for RED */
	int output_defqcount[PRIORITY_MAXTYPE]; /* maximum queue size */
	queuetype outputq[PRIORITY_MAXTYPE];    /* output hold queues */
        int holdqueue_out;	/* configured value for size of output holdq */
	struct hwidbtype_ *output_if_hw; /* for handling simplex circuits */
	boolean xmitonly;           /* ditto */
	int input_qcount;	    /* size of input queue */
	int input_defqcount;	    /* default size of input queue */
	int xmitdelay;		    /* delay in flags or microseconds */
        int xmitdelay_default;      /* default for xmitdelay */
	afilter_reset_t afilter_reset; /* ptr to address filter rst routine */

	struct thcdbtype_ *tcp_compress; /* non-NULL if TCP compression on */

#endif hwidb_fields_generic
	/*
	 * XX stuff:
	 */
	ushort nim_type;	    /* NIM type code */
	ushort nim_version;	    /* NIM version number */

        /*
         * MCN (Multi Channel Nim) fields
         */
        ulong m32_channel;	    /* channel number for the controller */

	/*
	 * dial backup stuff
	 */
	struct hwidbtype_ *secondary; /* pointer to backup interface */
	ushort in_load;		    /* kickin load for 2ndary */
	ushort out_load;	    /* kickout load for 2ndary */
	ushort in_delay;	    /* kickin  delay for 2ndary */
	ushort out_delay; 	    /* kickout delay for 2ndary */
	ulong backup_state;         /* status bits for loads and delays */
	sys_timestamp delay_timer;  /* secondary delay timer */
	uchar rxload;		    /* receive load (dial backup) */
#ifdef hwidb_fields_generic	    /* for xx_alert.c */
        uchar failCause;            /* link down fail cause, ref. linkdown_event.h*/
        uchar lastFailCause;        /* last failure cause */
#endif hwidb_fields_generic
	tinybool standby;           /* used with status to determine status */

	/*
	 * dial on demand fields
	 */
	dialerdbtype *dialerdb;	    /* dialer structure */

  	/*
	 * Snapshot specific information
  	 */
	struct snapshot_active_dialer_hardware_block_ *snapshot_adhb;
#ifdef hwidb_fields_generic

	/*
	 * Interface function dispatches
	 */
	vencap_t vencap;	    /* variable encapsulation */
	bridge_vencap_t bridge_vencap; /* transparent bridging encapsulation */
	bencap_t board_encap;	    /* board level encapsulation - for lvl 2 */
	cbencap_t cache_board_encap; /* board encap routine to write 2 caches*/
	soutput_t soutput;	    /* output starter */
	sinput_t sinput;	    /* input starter */
	oqueue_t oqueue; 	    /* enqueue packet for output */
	oqueue_dequeue_t oqueue_dequeue; /* dequeue packet for output */
	iqueue_t iqueue;	    /* enqueue packet for input */
        oqueue_dequeue_t shape_oqueue_dequeue; /* dequeue packet for output */

	parse_packet_t parse_packet; /* parse incoming packet */
	reset_t reset;	            /* reset interface */
	shutdown_t shutdown;	    /* shut interface completely down */
	linktype_t getlink;	    /* get generic link type of input if */
	extract_hwaddr_t extract_hwaddr; /* get interface address */
	extract_snpa_t extract_snpa; /* get source or destination address */
	make_snpa_native_t make_snpa_native; /* convert SNPA to native form */
	broadcast_t broadcast;	    /* physical broadcast predicate.
				     * NB: this is not a boolean return type.
				     * It returns a fuzzy which shows true, 
				     * false, or not sure.  (Some ATM media
				     * cannot distinguish received broadcasts
				     * from other packets.)
				     */
	can_mcast_t can_mcast;	    /* media supports physical multicast */
	setencap_t setencap;	    /* set encapsulation method */
	deencap_t deencap;	    /* undo encapsulation */
	lineaction_t lineaction; /* line protocol actions for cstate */
	linestate_t linestate;	    /* line protocol up/down check */
  	periodic_t periodic;	    /* check timeouts, etc. */
  	device_periodic_t device_periodic;  /* device dependent periodic functions */
	enable_t enable;            /* enable throttled interface */
	listen_t listen;	    /* accept frames to this address */
	listenrange_t listen_range; /* accept frames to range of addresses */
	setup_bridging_t setup_bridging;   /* setup bridging on this interface */
	set_extra_addrs_t set_extra_addrs;  /* install group/functional addresses */
	set_maxdgram_t set_maxdgram;  /* set maximum datagram size */
	txqlength_t txqlength;	    /* number of buffers on txq */
	max_proto_mtu_t max_proto_mtu; /* max MTU for a particular protocol */
	system_configured_t system_configured; /* "final" init callback */

	/*
	 * Function vectors used by bstun.
	 */
        rx_char_int_t rx_char_int;      /* for each received character */
        rx_framer_int_t rx_framer_int;  /* for each received frame */
        tx_comp_notify_t tx_comp_notify; /* hook for transmit complete */

	/*
	 * Used by the low-end drivers for the HD-64570 serial chip.
	 */
        rx_in_progress_t rx_in_progress; /* middle of receiving a frame */
	restart_rx_t restart_rx;       /* flush abd restart hunt */
	disable_rx_t disable_rx;       /* stop hunt */
	getsysbuf_t  getsysbuf;	       /* get buffer from hardware interface */

	/*
	 * The following vectors are used by various drivers
	 * to extend the "show interface", "show controller" and
	 * "clear interface" commands in media-specific ways.
	 */
	show_support_t show_support;  /* "show interface" support routines */
	show_controller_t show_controller; /* "show controller" support routines */
	clear_support_t clear_support;  /* interface hw/counters clear func */
	/*
	 * Generic interface statistics. Note that these are duplicated
	 * below for the users who like to "clear counters" for interactive
	 * work. If you change/add/delete one of these, check to see if you
	 * need to do the same to one of the "si_" prefixed variables below.
	 */
	idb_current_counters_t	counters;

	/* 
	 * These are the duplicates of the above for interactive work. They
	 * contain the values of the variables as of the last time "clear
	 * counter" was done for this interface. 
	 *
	 * The struct to hold these counters is allocated only when a 
	 * "clear counters" command is issued.
	 */

	/* First, an information field */
	sys_timestamp si_last_cclear;	/* Time since last counters clearing */
	idb_si_counters_t	*si_counters;

	/* End of duplicated fields for "show interface" */

	long error_count;		/* count of errors in last period */
	long error_threshold;		/* maximum errors/period */
#endif hwidb_fields_generic

	/*
	 * Async fields
	 */
	tt_soc *idb_tty;		/* Pointer to tty data structure */

	/*
	 * Lance fields
	 */
	ulong dma_memory_error;		/* # memory errors reported by DMA device */
	ulong si_dma_memory_error;	/* # mem errors reported by DMA device */
	tinybool ethernet_lineup;	/* Protocol up flag */

	/*
	 * Serial interface dependent fields.
	 */
	tinybool dte_interface;		/* set when it is dte */
        tinybool far_end_looped;	/* far end in remote loop */
	const char ALIGN_4(*applique_name);  /* was applique in the mci struct */
	ulong serial_flags;	    /* serial interface specifig flags */
        ulong csu_dsu_type;         /* type of integrated CSU/DSU on c2524/2525 */
	
	ulong serial_pulseinterval; 		/* seconds to pulse DTR */
	ulong serial_pulseinterval_def;         /* Type dependant default */
	show_serial_state_t show_serial_state;
	sys_timestamp serial_pulsetimer;	/* DTR pulse timer */
	ushort tx_burst_deferral;	/* # of packets to defer to before xmit */
	ushort app_type;	     /* serial applique type */
	ulong unknown_encaps;	     /* No. of unknown serial encaps */	

	void *vip4t_data;	     /* Private data for 4T VIP interface */
#ifdef hwidb_fields_ibu

	/*
	 * Serial tunnel dependent fields
	 */
	queuetype stun_Q;	    /* serial tunnel pointers */
	uchar stun_group;	    /* Group for interface */
	struct hwidbtype_ ALIGN_4(*stun_if_last); /* last stun IDB used */
	sdlc_proxy_interface *stun_protocol_specific;
	                            /* Pointer to stun protocol dependant
				       definitions for interface */
	int stun_sdlc_encap;	    /* STUN SDLC encapsulation type */

	/*
	 * Block Serial tunnel dependent fields
	 */
	queuetype bstun_Q;		/* serial tunnel pointers */
	uchar bstun_group;		/* Group for interface */
        uchar syn_char;                 /* Sync character. */
        uchar poll_protocol_flags;      /* Primary/Secondary/etc. */
        uchar hdx_flags;                /* Full/Half Duplex. Aka: ignore RTS.*/
	struct hwidbtype_ *bstun_if_last; /* last stun interface used */
#endif hwidb_fields_ibu

	/*
	 * MCI, routing cache, and bridging dependent fields
	 */
	int ALIGN_4(mci_index);		/* idx to MCI controller descriptor */
	int cbus_ctrlr_index;		/* cont'r # of cBus daughter card */
	int mci_lineup;			/* proto up flag used by MCI Ethers */
	sys_timestamp mci_lineuptimer;	/* msclock since last lineup
					   verification */

        int mci_txcount_default; /* default txcount (RSP too) */
        boolean mci_txcount_enabled; /* override default tql (RSP too) */
	short mci_txcount;	/* count of available TX buffers (RSP too) */
	ushort multibits;	/* mask for testing for input multicasts */

	short mci_txqloc;	/* MEMA address of transmit queue length */
	ushort fddi_cam_sa_miss;/* SA not found in FDDI CAM */
	mciregtype *mci_regptr;	/* pointer to controller registers */
	mciregtype *mci_testptr;/* duplicate pointer to controller registers */
	boolean mci_checked;	/* TRUE if MCI_CMD_CHECK succeeded */
	boolean buffer2mci;	/* TRUE if process level writing to mci/cbus */
	ushort rxlength;	/* length of MCI/cBus rcvd datagram -  BYTES */
	ushort rxrifwordlen;	/* length of RIF - in WORDS */
	ushort rxsrbinfo;	/* srbinfo field on an incoming packet */
	ushort rxtype;		/* classification type */
	charlong sniff[SNIFFLONGS]; /* buffer for sniffing from MCI */
	int cbus_icb_address;	/* Address of ICB in MEMD */
	short span_macoffset;	/* offset of MAC header in bridged datagram */
	short span_encapsize;	/* Size in bytes of bridging encapsulation */
	long buffer_pool;	/* Buffer pool used by interface */
	int span_in_bigdrop;	/* Count of big packets dropped on input */
	int span_out_bigdrop;	/* Number of big packets dropped on output */
	boolean bridge_transit; /* Is this an encapsulating interface? */
	ulong span_bridge_process_force; /* bit array of things that can nuke
					    the fast bridge path */
	void *lex_bridge_header_cache; /* Bridging header for lex during fast
					  switching */ 

	ushort cbus_burst_count;    /* number of buffers to park under if */
	ushort cbus_burst_count_def;/* default # of buffers to park under if */
	boolean sse_bridge_forwarding;	/* TRUE if SSE bridge forwarding */
	boolean cbus_bridge_config;/* TRUE if AB is configured for this if */
	boolean cbus_bridge_enable;/* TRUE if AB is enabled for this if */

	/*
	 * Token Ring interface dependent fields.
	 * tr_multiring, mapped to a ulong of "RXTYPE" flags, determines,
	 * for a given protocol, if an interface participates in
	 * multi-ring type protocols.  If false do not generate rifs for this
	 * protocol.
	 */
        struct spyglass_data_t_ *ctr_private;
        struct vip4r_data_t_ *vip4r_info;
#ifdef hwidb_fields_ibu

	ulong collisions;
	struct hwidbtype_ *tr_bridge_idb;/* srb - if local the target */
	tr_ventrytype *tr_vring_blk;	/* srb - remote/multiport entries */
	tr_vpeertype *tr_ifpeer;	/* srb - raw interface peer ptr */
	peerentry *dlsw_ifpeer;		/* dlsw - raw interface peer ptr */

	struct dn_info_ *dn_func;       /* DECnet token ring functnl addrs */

	tinybool tr_3com_hack;		/* true if we are working around */
	ulong ALIGN_4(tr_ring_mode);	/* running REM, RPS, or CRS */
	boolean mac_use_exp_buf;		/* mac frames are express buffered */
	ulong tr_transit_oui;		/* Transit OUI for encap.  bridging */
	int srb_lsap_in;		/* srb - input list for SAPs */
	int srb_type_in;		/* srb - input list for types */
	int srb_address_in;		/* srb - input list for addresses */
	int srb_lsap_out;		/* srb - output list for SAPs */
	int srb_type_out;		/* srb - output list for types */
	int srb_address_out;		/* srb - output list for addresses*/
	int tr_ring_speed;		/* 4 or 16 Mbits */
	int srb_routecache;		/* TRUE if route cache enabled */
	tr_srb_fastswitch_type tr_srb_fastswitch; 
	                                /* srb - config specific FS routine */
	if_srb_fastout_type if_srb_fastout;  /* srb - current output routine */
	if_srb_les_fastout_type if_srb_les_fastout;  /* srb - current output routine */
	if_srb_hes_fastout_type if_srb_hes_fastout;  /* srb - current output routine */
	ring_info *ring_info;		/* srb lan manager information */
	struct hwidbtype_ *vidb_link;	/* srb-tie the virtual to a physical */
	rsrb_fst_fastout_type rsrb_fst_fastout;	
	                                /* rsrb fst fast switch routine */
	int rsrb_vringoffset;		/* vring header offset in RSRB frame */

	/*
	 * Token Ring Access list present indicators.  These are needed
	 * to indicate to the fast routines whether or not *any* srb or
	 * netbios access lists exist.
	 */
	boolean srb_access_in;
	boolean srb_access_out;
#endif hwidb_fields_ibu

	/*
	 * Hub fields.
	 */
	hub_t *hub;                     /* ptr to hub struct, if hub present */

	/*
	 * RMON fields.
	 */
	rmon_t *rmon;		/* ptr to rmon struct, if rmon enabled */

	/*
	 * Support for boolean expression filters.
	 */
	expr_tree_entry_t *access_expression_in;  /* boolean expr on input  */
	expr_tree_entry_t *access_expression_out; /* boolean expr on output */

	/*
	 * SMDS protocol dependent fields
         */
	smds_pdb_t *smds_pdb;		/* Pointer to SMDS specifics */
	tinybool smds_lineup;		/* Line up value */
	tinybool smds_bridge;		/* bridging on SMDS enable/disable */
	uchar smds_begin_tag;	        /* a unique tag on the output pkt */

	/*
	 * ATM interface specific fields
	 */
	tinybool atm_lineup;	       /* Line up value */
	atm_db_t ALIGN_4(*atm_db);

	/*
	 * HDLC protocol dependent fields (most now in serial subblock)
	 */
	tinybool hdlc_lineup;		/* current state of line protocol */

	/*
	 * SNMP information fields
	 */
	tinybool snmp_trap_nolink;	/* if TRUE, disable LINKUP/DOWN traps */
        /*
	 *  interface logging info
	 */
        ulong logging_events;

	/*
	 * PPP protocol dependent fields
	 */
	tinybool ppp_lineup;		/* current state of line protocol */
	tinybool ppp_lapb;		/* lapb being used with ppp */
	vencap_t ppp_new_vencap;	/* additional variable encapsulation */
	lineaction_t ppp_new_lineaction; /* additional enc actions for cstate */
	parse_packet_t ppp_save_parse_packet;   /* save for restore */
	show_support_t ppp_new_show_support; /* additional info for "show interface" */
	fsm *lcpfsm;
	lcp_state *lcpstate;
	auth_state *authstate;		/* state for PPP authentication */
	fsm **ncpfsmidx;		/* pointer to all ptrs to NCP fsm's */
	boolean (*ppp_compression)(paktype *pak); /* Compression Routine */
	uchar	*ppp_comp_history;	/* Pointer to compression history */
	uchar	*ppp_comp_wrkspc;	/* Pointer to workspace memory area */
	boolean (*ppp_decompression)(paktype **pak); /* Decompression Routine */
	uchar	*ppp_decomp_history;	/* Pointer to decompression history */
	tinybool ppp_bridge;
	tinybool ppp_bridge_ip;		/* True if half-bridging ip */
	tinybool ppp_bridge_ipx;	/* True if half-bridging ipx */
	tinybool ppp_bridge_atalk;	/* True if half-bridging atalk */
	encapstype ppp_bridge_ipx_enctype;
	mlplinktype *mlp_link;
	/*
	 * LAPB protocol dependent fields
	 */
	lapbidbtype *lapbidb;		/* pointer to lapb specific fields */
	uchar lapb_upstate;		/* protocol state code when up */
	uchar lapb_dce;			/* logical DTE/DCE interface */
	uchar lapb_state;		/* protocol state */
	uchar lapb_poll;		/* remember when we send a poll */
	uchar lapb_tx_state;		/* transmit engine state */
	uchar lapb_rx_state;		/* receive engine state */
	uchar lapb_exit_rnr_rej;	/* exit RNR state with REJ? */
	uchar lapb_vs;			/* send state variable */
	uchar lapb_vr;			/* receive state variable */
	uchar lapb_rvr;			/* remote receive packet count */
	uchar lapb_nr;			/* last received N(R) */
	uchar lapb_n2;			/* N2 retransmission count */
	uchar lapb_k;			/* k window size */
	uchar lapb_admin_k;		/* administrative (pending) k */
	uchar lapb_retcnt;		/* retransmission count */
	ushort ALIGN_2(lapb_modulo);	/* mode; modulo 8 or 128 */
	ushort lapb_admin_modulo;	/* administrative (pending) modulo */
	ushort lapb_t1;			/* T1 retransmission timer */
	ushort lapb_t3;			/* T3 physical outage timer */
	ushort lapb_t4;			/* T4 keepalive timer */
	ushort lapb_n1;			/* N1 number of bits for I data */
	boolean lapb_override_def_n1;	/* TRUE if user configures N1 */
	sys_timestamp lapb_t1_timer;	/* T1 timing value */
	sys_timestamp lapb_t3_timer;	/* T3 timing value */
	sys_timestamp lapb_t4_timer;	/* T4 timing value */
	sys_timestamp lapb_col_timer;	/* set mode collision timer */
	paktype	*lapb_requeue_pak;	/* packet driver was unable to send */
	queuetype lapb_u_s_q;		/* unnumbered/supervisory fr. to Tx */
	queuetype lapb_holdq;		/* hold packets not yet sent */
	queuetype lapb_retxq;		/* retransmit queue */
	queuetype lapb_unackq;		/* hold packets sent pending ack. */
	oqueue_t lapb_oqueue;		/* secondary output enqueue */
	oqueue_dequeue_t lapb_oqueue_dequeue; /* secondary output dequeue */

	ulong lapb_iframessent;		/* total IFRAMEs sent */
	ulong lapb_iframesrcvd;		/* total IFRAMEs received */
	ulong lapb_rnrssent;		/* total RNRs sent */
	ulong lapb_rnrsrcvd;		/* total RNRs received */
	ulong lapb_rejssent;		/* total REJs sent */
	ulong lapb_rejsrcvd;		/* total REJs received */
	ulong lapb_sabmssent;		/* total SABMs sent */
	ulong lapb_sabmsrcvd;		/* total SABMs received */
	ulong lapb_frmrssent;		/* total FRMRs sent */
	ulong lapb_frmrsrcvd;		/* total FRMRs received */
	ulong lapb_discssent;		/* total DICSs sent */
	ulong lapb_discsrcvd;		/* total DISCs received */
	ulong lapb_protocol;		/* linktype of upper level protocol
					 * when not using multi-lapb */

	ulong lapb_mib_state_changes;	/* count of MIB state changes */
	ulong lapb_tx_deferrals;	/* count of Tx deferrals due to RNR */
	ulong lapb_t1_retx;		/* count of re-Tx due to T1 timeout */
	uchar lapb_mib_state;		/* silly MIB state */
	uchar lapb_state_reason;	/* reason for MIB state change */
	uchar lapb_frmr_txdata[6];	/* last FRMR information sent */
	ushort lapb_frmr_txlen;		/* length of lapb_frmr_txdata */
	uchar lapb_frmr_rxdata[6];	/* last FRMR information received */
	ushort lapb_frmr_rxlen;		/* length of lapb_frmr_rxdata */

	/*
	 * These fields used by IGRP for load balancing over
	 * lossy serial lines.
	 */
	ulong ALIGN_4(lapb_lastiframes);/* IFRAMES sent in last interval */
	ulong lapb_lastrejs;		/* REJ's received in last interval */

	/*
	 * Transparent Bridging over Multiprotocol LAPB
	 */
	boolean mlapb_bridge;           /* Multi-lapb bridging status */

        /*
         * These are the duplicates of the above lapb statistics for
	 * interactive work. They contain the values of the variables
	 * as of the last time "clear counter" was done for this interface
	 * (the "si_" prefix stands for "show interface").
         */
        ulong si_lapb_iframessent;      /* total IFRAMEs sent */
        ulong si_lapb_iframesrcvd;      /* total IFRAMEs received */
        ulong si_lapb_rnrssent;         /* total RNRs sent */
        ulong si_lapb_rnrsrcvd;         /* total RNRs received */
        ulong si_lapb_rejssent;         /* total REJs sent */
        ulong si_lapb_rejsrcvd;         /* total REJs received */
        ulong si_lapb_sabmssent;        /* total SABMs sent */
        ulong si_lapb_sabmsrcvd;        /* total SABMs received */
        ulong si_lapb_frmrssent;        /* total FRMRs sent */
        ulong si_lapb_frmrsrcvd;        /* total FRMRs received */
        ulong si_lapb_discssent;        /* total DICSs sent */
        ulong si_lapb_discsrcvd;        /* total DISCs received */
	ulong si_lapb_mib_state_changes; /* count of MIB state changes */
	ulong si_lapb_tx_deferrals;	/* count of Tx deferrals due to RNR */
	ulong si_lapb_t1_retx;		/* count of re-Tx due to T1 timeout */

	/* COMPRESSION */ 
	compress_db *comp_db;		/* ptr to compress cntrl block  */ 

	/*
	 * X25 protocol dependent fields
	 */
	x25idbtype *x25idb;		/* x25 specific fields */
	uchar x25_version;		/* X.25 1980/1984/1944 version */
	uchar x25_modulo;		/* default sequence numbering */
	uchar x25_cud_pid_def;		/* default encapsulation method */
	uchar x25_bfe_mode;		/* BFE emergency mode type */
	ushort x25_bfe_status;		/* BFE emergency status type */
	ushort x25_bfe_decision;	/* BFE emergency decision type */
	sys_timestamp x25_timer;	/* interface timer */
	ulong x25_default;		/* default linktype */
	addrtype *x25_address;		/* X.121 address of interface */
	uchar x25_state;		/* interface state */
	uchar x25_dce;			/* logical DTE/DCE interface */
#ifdef hwidb_fields_ibu
        uchar x25_upstate;              /* interface state code when up */
#endif hwidb_fields_ibu
	uchar x25_restart;		/* LAPB requests X.25 to restart */
	ushort x25_win;			/* default window size */
	ushort x25_wout;		/* default window size */
	ushort x25_pin;			/* default packet size */
	ushort x25_pout;		/* default packet size */
	char x25_retransmit;		/* RESTART retrasmission count */
	uchar x25_cause;		/* for restart */
	uchar x25_diag;			/* also for restart */
	uchar x25_t0;			/* restart timer */
	uchar x25_t1;			/* call setup timer */
	uchar x25_t2;			/* reset indication timer */
	uchar x25_t3;			/* clear indication timer */
	uchar x25_th;			/* Ack hold timer */
	uchar x25_idle;			/* VC idle timer */
	uchar x25_nvc;			/* number of VCs */
	ushort x25_flags;		/* various support flags */
	ushort x25_lic;			/* lowest incoming channel */
	ushort x25_hic;			/* highest incoming channel */
	ushort x25_ltc;			/* lowest two-way channel */
	ushort x25_htc;			/* highest two-way channel */
	ushort x25_loc;			/* lowest outgoing channel */
	ushort x25_hoc;			/* highest outgoing channel */
	ushort x25_holdmax;		/* size of hold queue */
	ushort x25_holdtime;		/* time to ignore packets for VC */

	uchar x25_fwin;			/* global facility window size */
	uchar x25_fwout;		/* global facility window size */
	uchar x25_tput_in;		/* global facility throughput */
	uchar x25_tput_out;		/* global facility throughput */
	ushort x25_fpin;		/* global facility packet size */
	ushort x25_fpout;		/* global facility packet size */
	uchar x25_freverse;		/* global facility reverse */
	uchar x25_fcug;			/* global facility CUG */
	ushort x25_tdelay;		/* global facility transit delay */
	rpoatype *x25_rpoa;		/* global facility RPOA */
	tinybool x25_strict_ccitt_facil;/* check facility DTE/DCE source? */
	tinybool x25_bridge;		/* X25 bridging status */
	tinybool x25_tcp_compress;	/* X25 THC status */
	tinybool x25_snmp_restart_tx;	/* SNMP RESTART message sent flag */

	ushort x25_admin_modulo;	/* administrative x25_modulo */
	ushort x25_admin_lic;		/* administrative x25_lic */
	ushort x25_admin_hic;		/* administrative x25_hic */
	ushort x25_admin_ltc;		/* administrative x25_ltc */
	ushort x25_admin_htc;		/* administrative x25_htc */
	ushort x25_admin_loc;		/* administrative x25_loc */
	ushort x25_admin_hoc;		/* administrative x25_hoc */
	ushort x25_admin_pin;		/* administrative x25_pin */
	ushort x25_admin_pout;		/* administrative x25_pout */
	ushort x25_admin_win;		/* administrative x25_win */
	ushort x25_admin_wout;		/* administrative x25_wout */

	ulong ALIGN_4(x25_restartssent);/* total restarts sent */
	ulong x25_restartsrcvd;		/* total restarts received */
	ulong x25_callssent;		/* total calls sent */
	ulong x25_callssentfailed;	/* total calls send failed */
	ulong x25_callsrcvd;		/* total calls received */
	ulong x25_callsrcvdfailed;	/* total calls received failed */
	ulong x25_callsfrwd;		/* total calls forwarded */
	ulong x25_callsfrwdfailed;	/* total calls forwarded failed */
	ulong x25_diagssent;		/* total DIAGs sent */
	ulong x25_diagsrcvd;		/* total DIAGs received */
	ulong x25_provider_clears;	/* CLEARs initiated by DCE */
	ulong x25_dte_resets;		/* RESETs w/ 'DTE initiated' cause */
	ulong x25_dce_resets;		/* RESETs w/ other causes */
	ulong x25_unexp_restarts_rx;	/* unexpected RESTARTs received */
	ulong x25_data_rx;		/* DATA packets received */
	ulong x25_data_tx;		/* DATA packets sent */
	ulong x25_errors_accused;	/* 'local proc. error' causes rec'd */
	ulong x25_interrupt_rx;		/* INTERRUPT packets received */
	ulong x25_interrupt_tx;		/* INTERRUPT packets sent */
	ulong x25_t0_count;		/* T10/T20 timeouts */
	ulong x25_t1_count;		/* T11/T21 timeouts */
	ulong x25_t2_count;		/* T12/T22 timeouts */
	ulong x25_t3_count;		/* T13/T23 timeouts */
	ulong x25_retries_exhausted;	/* retries exhausted counter */
	ulong x25_clears_exhausted;	/* CLEAR retries exhausted counter */

        /*
         * These are the duplicates of the above X.25 statistics for
	 * interactive work. They contain the values of the variables
	 * as of the last time "clear counter" was done for this interface
	 * (the "si_" prefix stands for "show interface").
         */
        ulong si_x25_restartssent;      /* total restarts sent */
        ulong si_x25_restartsrcvd;      /* total restarts received */
        ulong si_x25_callssent;         /* total calls sent */
        ulong si_x25_callssentfailed;   /* total calls send failed */
        ulong si_x25_callsrcvd;         /* total calls received */
        ulong si_x25_callsrcvdfailed;   /* total calls received failed */
        ulong si_x25_callsfrwd;         /* total calls forwarded */
        ulong si_x25_callsfrwdfailed;   /* total calls forwarded failed */
        ulong si_x25_diagssent;         /* total DIAGs sent */
        ulong si_x25_diagsrcvd;         /* total DIAGs received */
	ulong si_x25_provider_clears;	/* CLEARs initiated by DCE */
	ulong si_x25_dte_resets;	/* RESETs w/ 'DTE initiated' cause */
	ulong si_x25_dce_resets;	/* RESETs w/ other causes */
	ulong si_x25_unexp_restarts_rx;	/* unexpected RESTARTs received */
	ulong si_x25_data_rx;		/* DATA packets received */
	ulong si_x25_data_tx;		/* DATA packets sent */
	ulong si_x25_errors_accused;	/* 'local proc. error' causes rec'd */
	ulong si_x25_interrupt_rx;	/* INTERRUPT packets received */
	ulong si_x25_interrupt_tx;	/* INTERRUPT packets sent */
	ulong si_x25_t0_count;		/* T10/T20 timeouts */
	ulong si_x25_t1_count;		/* T11/T21 timeouts */
	ulong si_x25_t2_count;		/* T12/T22 timeouts */
	ulong si_x25_t3_count;		/* T13/T23 timeouts */
	ulong si_x25_retries_exhausted;	/* retries exhausted counter */
	ulong si_x25_clears_exhausted;	/* CLEAR retries exhausted counter */

	/*
	 * FDDI unique variables
	 */
#ifdef hwidb_fields_ibu  /* just till we clean this better ... gcox */
	uchar bit_swapped_hardware[6];	/* bit swapped mac-level address */
#endif hwidb_fields_ibu
        struct phytype_ *phy[2];                /* Pointers to PHY records */
	int fddi_options;	 	/* Configuration option for DUP addr */
	ushort fddi_stn_id;	 	/* XDI SMT station index */
	ulong ALIGN_4(fddi_rops); 	/* no. of Ring Ops */

	/*
	 * Frame Relay specific fields
	 */
	fr_idb_struct_type *frame_relay_stuff;
	dxi_idb_struct_type *atm_dxi_stuff;
	cpp_info_type *cpp_info;

	/*
	 * frame relay defs now become part of frame_relay.h
	 * any future refs to any frame relay "idb" variables would be
	 * in frame_relay.h accessed via frame_relay_stuff
	 */

        /*---- The lcb will be overloaded. Not just BSC; as is current. */
        lcb_t* lcb;			/* Poll protocol line control block. */

	/* 
	 * Fast switching fields
	 * some of those touched during fastswitching have been grouped
	 * together at the top for more effective data caching
	 */
 	/* Explorer fastswitching fields. */

        fs_explorer_t fs_expl_func;     /* Explorer fast queue vector*/
        fs_expl_que  *fs_expl_allQ;     /* Explorer all-routes fastQ */
        fs_expl_que  *fs_expl_spanQ;    /* Explorer spann-tree fastQ */
        fs_expl_que  *fs_expl_input;    /* Explorer input - this idb */

        fs_vbridge_t  vbridge_forward;  /* Virtual bridge translator  */

	idbtype *fast_rcvidb;           /* SW idb for fast switching */
	boolean ip_fastokay;	        /* route cache enabled and possible */
	int ip_routecache;	        /* bitmask type of route cache enabled */
	tinybool ipmfast_switched;      /* for multicast fastswitching */

 	short dn_congest_limit;		/* q length cutoff for congestion */
 	tinybool dn_routecache;		/* route cache enabled */
 	tinybool dn_fastokay;		/* route cache enabled and  possible*/

	/*
	 * XNS, Apollo and Urine-Bile XNS HWIDB variabless
	 * Currently assumes this follows dn_fastokay for byte alignment.
	 * The rest of the XNS IDB variables have been moved to
	 * a XNS sub-block.
	 */
	tinybool xns_routecache;	/* XNS routing cache enable flag */
	tinybool xns_fastokay;		/* routing enabled and possible */

	/*
	 * Novell HWIDB variables
	 */
	int  novell_routecache;     	/* NOVELL routing cache enable flag */
	tinybool novell_fastokay;	/* routing enabled and possible */
	tinybool novell_ether_ok;	/* Accept Novell-ether FS input */
	tinybool novell_arpa_ok;	/* Accept Novell ARPA FS input */
	tinybool novell_iso1_ok;	/* Accept Novell ISO1 FS input */
	tinybool novell_iso2_ok;	/* Accept Novell ISO2 FS input */
	tinybool novell_spxspoof;	/* flag at hwidb for spoofing  */
 	tinybool novell_input_acl_checks; /* input acl summary for fs path */

	/*
	 * AppleTalk HWIDB variables
	 */
	tinybool atalk_fastokay;	/* TRUE if fast switching enabled */
	int atalk_routecache; 		/* !0 if fast switching desired */
	ulong atalk_outputerrs;		/* Number of interface output errors */
	ulong atalk_inputerrs;		/* Number of interface input errors */
	ulong atalk_compat_flags;	/* compatibility flags */
	ulong atalk_fastoddoutadj;	/* Adj to FS packet len if odd start */

	int clns_routecache;		/* CLNS routing cache enable flag */
	ulong clns_congestion;		/* when to set congestion bit */
					/* 2 = bad */
	tinybool clns_fastokay;		/* routing enabled and possible */

	/*
	 * Vines HWIDB variables. Assumes that this be between
	 * clns_fastokay and cmns_intfc_enabled.
	 */
	tinybool vines_fastokay;
	tinybool vines_routecache;

	/*
	 * CMNS unique variables
	 */
	tinybool cmns_intfc_enabled;	/* enable CMNS */
	/*
	 * MOP support
	 */
	tinybool mop_sysid_enabled;	/* TRUE if periodic sysids enabled */
	tinybool mop_enabled;		/* TRUE if MOP allowed at all */
	mopcon ALIGN_4(*mop_console);	/* pointer to mop console block */

        /*
         * Token Ring MIB's
         */
	dot5_void_type dot5_get_smt_stats;
	dot5_void_type dot5_add_counters;
	dot5_boolean_type dot5_soft_reset;
#ifdef hwidb_fields_ibu

	/*
	 * SDLC and SDLLC fields
	 */
	sdlc_data_block **sdlc_data;   /* Pointers to various SDLC fields */
        uint        sdlc_default;
        boolean     sdlc_have_default_partner;
        uchar       sdlc_default_partner[IEEEBYTES];
        boolean     sdlc_default_inbound;
        boolean     sdlc_default_outbound;
        uint        sdlc_dlsw_u_sap_id;

	/*
	 * Items that are common to all devices on a link
	 */
	int sdllc_largest_token_frame;
	boolean sdllc_active;
	ushort configured_role;		/* configured SDLC role */
	ushort sdlc_role;		/* running SDLC role */
	ushort sdlc_t1;			/* T1 retransmission timer */
	ushort sdlc_n1;			/* N1 number of bits for I data */
	uchar sdlc_n2;			/* N2 retransmission count */
	uchar sdlc_k;			/* k number of outstanding packets */
	ushort sdlc_poll_wait;		/* SDLC secondary poll timeout */
	sys_timestamp sdlc_top_timer;	/* top level countdown timer */
        ulong sdlc_rnrLimit;            /* SZ Remote busy timer */
        sys_timestamp sdlc_lastModifyTime; /* SZ Last time config was modified */
	boolean    sdlc_simultaneous;	/* SDLC TWS active		*/
	uchar sdlc_output_flag;		/* Flag for transmission	*/
        uchar sdlc_group_addr;          /* Group address for SDLC group poll */
        uchar sdlc_grp_start_poll;      /* first address in the group to look */
                                        /* for data to send */
	ulong ALIGN_4(sdlc_xmtptr);	/* TWS transmission leg pointer */
	uchar sdlc_vr_macaddr[6];       /* virtual tr mac addr (first five */
					/* bytes -- last is SDLC PU addr) */
        ushort sdlc_next_startup;       /* where to pick up in start up scan */
	ulong sdlc_flags;		/* SDLC support flags -- see sdlc.h */
	/* These timers are in msec */
	ulong sdlc_poll_pause_timer;	/* How long to wait until next poll */
					/* cycle */
	ulong sdlc_poll_limit_value; 	/* # of poll sequences t/f a station */
	ulong sdlc_current_polled; 	/* Who is currently being polled? */
        ulong sdlc_slow_polled;         /* current pointer at who is slow polled */
        sys_timestamp sdlc_slow_poll_timer; /* when to slow poll next */
        ulong sdlc_slow_poll_interval;  /* deltas between slow poll attempts */
        ulong sdlc_uc_delay;            /* system software to microcode delay */
        ulong sdlc_line_speed;          /* line speed to calc sdlc_uc_delay   */

	ulong rts_timeout_count;        /* For DTE interface, how many times
					   RTS has timeout, that is without CTS
					   from DCE */
	ulong si_rts_timeout_count;
	ulong half_duplex_violation;    /* For DCE half duplex, how many times
					   we see the incoming bits while we
					   are transmitting. The incoming bits
					   will be pitched */
	ulong si_half_duplex_violation;
	boolean snx_active;		/* set if sdlc-qllc conversion is 
					   running */

	/*
	 * This is a "union" of the upstate of all devices on a link
	 */
	ulong sdlc_upstate;
	/*
	 * SNA fields
	 */
	int sna_la_prilist;		/* SNA lu address priority list */
	int sap_prilist;               /* RSRB sap priority list */
	/*
	 * lnx/qllc variables
	 */
	qllc_data_t	*qllc_data;	/* Collect ALL QLLC variables */
#endif hwidb_fields_ibu

	/*
	 * ISDN fields 
	 */
	isdn_bri_t *isdn_info;		/* Pointer to ISDN info struct */

	/*
	 * Tunnel information fields
	 */
	tunnel_info *tunnel;		/* tunnel data structure */
	idbtype *tunnel_swidb;          /* Fastswitch parameter passing */

        /*
         * SSCOP information fields
         */
        sscopType *sscop;                /* pointer to SSCOP struct */

	boolean	serial_autoinstall; 	/* autoinstall status */

	/*
	 * Cisco 7000 and 7500 unique variables
	 */
	ulong slotunit;			/* port/unit number for this slot */
	enum IDB_STATES pre_hotstall_state; /* idb state before removal */
	ulong oir_flags;		/* OIR state flags */

	boolean pak_backing_store; /* Swap packets out of fast MEMD buffers */
				/* onto holdq in DRAM when HW TX queue full */
	ulong pak_swapped_out;		/* # packets we've swapped to DRAM */
	ulong si_pak_swapped_out;	/* # packets we've swapped to DRAM */

	/*
	 * RSP-specific elements
	 */
	slottype	*slot_ptr;
	qa_hwq_t	*raw_q_ptr;
	uint		global_free_q_size;
	qa_hwq_t	*global_free_q_ptr;
	qa_hwq_t	*local_free_q_ptr;
	uint		rx_limit_lo;
	uint		rx_limit_hi;
	uint            rx_currently_owned;
	uint            max_rx_currently_owned;

	boolean		tx_restart_active; /* A TX restart is scheduled */
	sys_timestamp	tx_restart_time; /* When TX restart was scheduled */

	uchar		*claimbuf;	/* Ptr to beacon/claim buffer */

	void            (*love_note)(struct hwidbtype_ *, lovenotetype *);

        /*
         * Cisco Access Pro (2500) LNM system flag and variables
         */
        boolean local_lnm_enabled;
        boolean lrm_session_est;
        ulong local_lnm_purge_count;
        ulong local_lnm_claim_count;

        boolean tr_lane_active;

	/* Virtual LAN attributes */

	boolean	dot10_vlan_on_hwidb;	/* TRUE if IEEE 802.10 vLAN encap
					   configured on ANY software IDB */
	boolean	isl_vlan_on_hwidb;	/* TRUE if ISL vLAN encapsulation is */
					/* configured on ANY software IDB */


};

/*
 * "Software" IDB
 *
 * This data structure appears once per sub-interface;  there may be many of
 * them associated with a single interface (and hardware IDB).  It contains
 * network layer protocol and routing information, as well as transparent
 * bridging fields.  Basically, anything that makes sense to replicate in the
 * face of multiple subinterfaces.
 *
 * All software IDBs are threaded onto the software IDB queue (swidbQ);  
 * please use the macros below for manipulating this queue.
 *
 * Each software IDB points at its associated hardware IDB;  this is a many-
 * to-one mapping.  Each hardware IDB points back at the first software IDB
 * associated with it (which will have a value of 0 in sub_number).  All
 * software IDBs associated with a particular hardware IDB are linked
 * together (via idb->nextsub) for easy traversal.
 *
 * When writing code that accesses these structures, please keep in mind
 * the presence (current or future) of multiple software IDBs.  This is
 * particularly important when it is necessary to access software IDB fields
 * from a procedure that has a hardware IDB pointer as its handle, as well
 * as protocol routines that manipulate the hardware.
 *
 * These fields are longword-aligned;  keep them that way.
 */
struct idbtype_ {            /*******************************/
                             /*                             */ 
  SWIDB_FIELDS_GENERIC       /*      These statements       */ 
                             /*        must be kept         */ 
  SWIDB_FIELDS_IBU           /*          together           */ 
                             /*                             */ 
                             /*******************************/ 
#ifdef swidb_fields_generic

        struct idbtype_ *next;      /* pointer to next IDB in queue */

	/*
	 * Fields used in fast switch routing and bridging at interrupt level.
	 * Keep them together right after the 'next' pointer at the start
	 * of the structure to minimize how many cache lines we have to
	 * touch during fast switching with Orion R4600 processors
	 */
	ulong	ip_mtu;		    /* current IP MTU */
	ulong   clns_mtu;	    /* CLNS specific MTU */
	ulong   sub_mtu;	    /* sub-interface specific MTU */

	boolean ip_routecache_same; /* TRUE if fastswitch ok on same intf */
	boolean tbridge_on_swidb;   /* TRUE if transparently bridging on */
				    /* THIS software idb */
	smf_entry_type **sfw_mac_filter;
				    /* software MAC address filter for */
				    /* promiscuous listeners */
	ushort	*rxtype_flags;	    /* route/bridge indication for rxtypes */
	enum TBR_MEDIA tbridge_media;
				    /* determines bridging translation */
        /*
         * Virtual LANs support.
         */

        said_descriptor *sde_said_db;    /* The 802.10 vLAN structure */

        isl_vlan_descriptor *isl_vlan;   /* The Inter Switch Link
                                            (ISL) vLAN structure */

        ulong cache_version;        /* Version number for all protocols */
	/*
	 * Traffic Shaping
	 */
	traffic_shape_t *traffic_shape;  /* pointer to rate limits */

	/*
	 * End of variables touched during fast switching
	 */

        boolean user_configured_mtu; /* User typed in MTU manually */


	struct hwidbtype_ *hwptr;   /* pointer to hardware IDB */
	struct idbtype_ *nextsub;   /* pointer to next subinterface */
	ulong if_number;	    /* number of interface, unit 1 based */
	ulong sub_number;	    /* number of subinterface (0 if master) */
	enum SUBIDB_STATES subif_state;	/* up/down/deleted */
	ulong subif_line_state;	    /* Operational state */
	ulong subif_link_type;	    /* point-to-point or multipoint? (valid */
				    /* on non-subinterfaces as well) */
	char *namestring;	    /* pointer to interface unit name */
	char *short_namestring;	    /* pointer to short interface unit name */
	char *description;	    /* User given description of device */
        char lanName[8];            /* name used by netview for displaying */
	uint snmp_if_index;	    /* snmp i/f index if registered, else 0 */
	boolean static_routing;	    /* TRUE if only static routing allowed */
	struct idbtype_ *output_if; /* for handling simplex circuits */
#endif swidb_fields_generic

/*
 *  Here continues the idbtype_ struct with fields invisible 
 *  to interface_generic.h:
 */
/*
	 * IDB user Sub-block area.
	 *
	 * The pre-allocated array is for those users of the SW IDB
	 * who we know about a priori, the dynamic list is for those
	 * who we don't know about yet.
	 */
	struct {
	    idb_sb_t		sb[SWIDB_SB_MAX];
	    list_header		sb_dynamic;
	    ulong		sb_dynamic_id;
	} subblock;
	/*
	 * Control vector. This is called with a pointer to ourself,
	 * a function opcode and a list of items to manipulate.
	 */
	swidb_state_config_t	state_config;

	int resolvemethod;	    /* RES_xxx method we used to resolve */

	ulong delay;		    /* delay, 10usec units (used mainly by IGRP) */
	ulong delay_def;	    /* default delay value */
	ulong visible_bandwidth_def;/* default bandwidth (Kbps) */
	ulong visible_bandwidth;    /* bandwidth (Kbps) */
 	ulong manual_delay:1;	    /* TRUE if delay set manually */
 	ulong manual_bandwidth:1;   /* TRUE if bandwidth set manually */

        mapgroup_type *mapgroup;    /* pointer to static map groups */

	/*
	 * ARP specific fields
	 */
	ulong arp_timeout;	    /* arp entry timeout value */
	ulong arp_gra_time;	    /* time out value for gratuitous arp
				     * in seconds */
	tinybool arp_arpa;	    /* true if using "arpa" arps */
	tinybool arp_probe;	    /* true if using hp's probe */
	tinybool arp_snap;	    /* true if using 802.2 snap_ip arps */
	tinybool arp_smds;           /* true if using SMDS arps */
	tinybool arp_arpa_def;	    /* default ether arp for this if */
	tinybool arp_probe_def;	    /* default probe arp for this if */
	tinybool arp_snap_def;	    /* default snap arp for this if */

	tinybool fr_arp_enable;
	tinybool fr_inarp_enable;
#ifdef swidb_fields_generic  /* just till we clean this better  */
        tinybool fr_tcp_hdr_comp;
#endif swidb_fields_generic
	tinybool fr_hdr_comp_passive;

	/*
	 * Internet protocol specific fields
	 * ip_enabled assumes it follows fr_hdr_comp_passive for
	 * the ip_address to be long-aligned.
	 */
	tinybool ip_enabled;	    /* TRUE if IP enabled on this interface */
        ipaddrtype ip_address;	    /* IP interface address */
	ipaddrtype ip_broadcast;    /* IP broadcast address */
	ipaddrtype ip_majornet;	    /* major IP network number */
	ipaddrtype ip_majormask;    /* major IP network mask */
	ipaddrtype ip_nets_mask;    /* mask of network and subnet fields */
	ipaddrtype ip_resolvehost;  /* host that provided address info */
 	ipaddrtype ip_rarp_server_addr; /* for RARP server spoofing */
	queuetype ip_helpernetQ;    /* for multiple helper-address entries */
	queuetype ip_addressQ;      /* list of secondary IP addresses */
	boolean ip_unreach;	    /* TRUE if send ICMP unreachables */
	boolean ip_redirect;	    /* TRUE if send ICMP redirects */
	boolean ip_maskreply;	    /* TRUE if send ICMP mask reply */
	int ip_directbcast;	    /* Non-zero if allow directed broadcasts */
	boolean ip_output_acctg;    /* TRUE if doing packet/byte accounting */
	boolean ip_violation_acctg; /* TRUE if doing violations accounting */
	ulong ip_maskid;	    /* special ICMP mask code for valid. */
	sys_timestamp ip_quench_timer; /* time of last ICMP source quench */
	ulong ip_quench_interval;   /* minimum interval between quenchs */
	sys_timestamp ip_unreach_timer; /* time of last ICMP unreachable */
	ulong ip_unreach_interval;  /* minimum interval between unreach. */
	acl_headertype *ip_output_accesslist;/* access list for output */
	acl_headertype *ip_input_accesslist; /* access list for input */
	acl_headertype *ip_output_accesslist_def; /* default output ACL */
	acl_headertype *ip_input_accesslist_def; /* default input ACL */
	char *crypto_map;	    /* Crypto map (in and out) */
	boolean ip_proxy_disabled;  /* true if proxy arps not allowed. */
	boolean ip_sec_default;	    /* TRUE if security is at the defult */
	boolean ip_sec_multilevel;  /* mode of security operation */
	boolean ip_sec_implicit;    /* implicit security labelling allowed */
	boolean ip_sec_cipso;       /* How to process commercial IPSO */
	ulong ip_sec_extended;	    /* How to process extended IPSO? */
	boolean ip_sec_allow_rsrvd; /* allow reserved security levels */
	uchar ip_sec_instyle;	    /* style of security on interface */
	uchar ip_sec_outstyle;      /* style of security on interface */
	uchar ip_sec_low;	    /* lower bound security level */
	uchar ip_sec_high;	    /* upper bound security level */
	uchar ip_sec_def_class;     /* use this if not supplied */
	uchar ip_sec_auth_low;      /* authority matrix low (required) */
	uchar ip_sec_auth_high;     /* authority matrix high (optional) */
	uchar ip_sec_def_auth;      /* use this if not supplied */
	queuetype ip_sec_nleso;	    /* extended IPSO info */
	queuetype ip_sec_aeso;	    /* extended IPSO info */
	ulong ip_passive_router;    /* router flags.  See route.h */
	int ip_enctype;		    /* default IP encapsulation type */
	boolean ip_gdp_enabled;	    /* TRUE if running Gateway Discovery */
	boolean ip_gdp_default;	    /* TRUE if mapping default route */
	ushort ip_gdp_priority;	    /* default GDP priority */
	ushort ip_gdp_holdtime;	    /* default GDP hold time */
	int ip_gdp_count;	    /* seconds until GDP report issued */
	int ip_gdp_countreset;	    /* seconds between GDP updates */
	queuetype ip_gdp_proxyQ;    /* list of GDP proxy addresses */
	struct idbtype_ *ip_unnumbered; /* pointer to a numbered interface */
	tinybool ip_irdp_enabled;   /* true if running router discovery */
	tinybool ip_irdp_multicast; /* true of adverts go to 224.0.0.1 */
	ushort ip_irdp_holdtime;    /* default IRDP hold time (report ok) */
	int ip_irdp_preference;	    /* default IRDP preference */
	int ip_irdp_count;	    /* seconds until IRDP report issued */
	int ip_irdp_initial_count;  /* initial interface bringup mode */
	int ip_irdp_minadvert;	    /* minimum seconds between broadcasts */
	int ip_irdp_maxadvert;	    /* maximum seconds between broadcasts */
	queuetype ip_irdp_proxyQ;   /* list of IRDP proxy addresses */
	tinybool ip_probe_name;	    /* TRUE if answering HP proxy-name */
	tinybool ip_split_disabled;  /* TRUE if split horizon is disabled */
	tinybool ip_split_disabled_def; /* default split horizon state */
	tinybool ip_routecache_same_def;/* def. for fastswitch on same intf */
	acl_headertype *ip_iprotofltr[MAX_IPPROCS]; /* lists for
							filtering input */ 
	acl_headertype *ip_oprotofltr[MAX_IPPROCS]; /* lists for
						       filtering output */ 
	ippeerinfotype *ip_peer;    /* peer address (IP) information */

	/*
	 * IP multicast fields.
	 */
	boolean   ip_mrouting;         /* If IP multicast router enable */
	boolean   ip_multicast_fs;     /* IP multicast fastswitching is on */
	boolean   ip_multicast_dr;     /* If set, IGMP host queries are sent */
        boolean   ip_multicast_use_functional; /* Use tring functional */
	queuetype ip_multiaddrQ;       /* Multicast groups router joined */
	ulong     ip_multicast_threshold;  /* TTL threshold for forwarding */
	ratetype  *ip_multicast_rate_list; /* Configured rate-limits */
	ulong     ip_multicast_out;    /* Total mcasts sent counted by IP */
	ulong     ip_multicast_in;     /* Total mcasts rcvd counted by IP */
	queuetype ip_mhelpermapQ;      /* Multiple helper-maps */
	boolean   igmp_enabled;        /* If IGMP is enabled on interface */
	boolean   cgmp_enabled;        /* If CGMP is enabled on interface */
	boolean   cgmp_proxy;          /* CGMP proxying for non-cisco routers*/
	boolean   cgmp_process_leaves; /* Send CGMP Leave when IGMP leaving */
	boolean   cgmp_do_batch_joins; /* Send CGMP batch Join messages */
	boolean   pim_enabled;         /* If PIM is enabled on interface */
	ulong     pim_mode;            /* Configured mode, dense or sparse */
	ulong     pim_query_interval;  /* How often Router Queries are sent */
	sys_timestamp pim_query_timer; /* Running PIM query timer */
	ulong     pim_dr;              /* Elected designated router */
	pim_nbrtype *pim_nbrs;         /* PIM neighbors on this interface */
	boolean   pim_nbma_mode;       /* Treat interface in NBMA mode */
	sys_timestamp dvmrp_nbr_timer; /* If DVMRP neighbors present */
	sys_timestamp dvmrp_nbr_uptime; /* How long tunnel nbr has been up */
	ulong     dvmrp_nbr_version;   /* DVMRP neighbor version */
	ulong     dvmrp_send_default;  /* If DVMRP default should be sent */
	ulong     dvmrp_metric_offset_in; /* Metric additive(incoming routes)*/
        ulong     dvmrp_metric_offset_out;/* Metric additive(outgoing)*/  
	boolean   dvmrp_unicast;       /* DVMRP unicast routing on interface */
	ulong     dvmrp_routes_rcvd;   /* Counter to monitor MBONE surges */
	ulong     dvmrp_proutes_rcvd;  /* Poison-reversed routes received */
	sys_timestamp dvmrp_routes_rcvd_timer; /* To clear dvmrp_routes_rcvd */
	ulong     dvmrp_unicast_out_count; /* # routes advertised last time */
	ulong     dvmrp_dvmrp_out_count;   /* # routes advertised last time */
	boolean   dvmrp_reject_non_pruners; /* Don't peer with non pruners */
 	boolean   dvmrp_auto_summary;      /* Auto summarization flag for if */
	acl_headertype *ipmulticast_boundary;/* Boundary multicast addresses */
#ifdef swidb_fields_ibu
	
        /* 
         * SRB fields
         */
        ulong srb_multiring_bits;
        ulong srb_allroutes_bits;


	/*
	 * bridging fields
	 */
	int	span_index;	/* index into spanning tree array */
	ulong	tbridge_route_linktypes;	/* C/IRB routing indicators */
	ulong   tbridge_bridge_linktypes;       /* IRB bridging indicators */
	int span_type_in;	/* input access list for Ethernet types */
	int span_lsap_in;	/* input access list for 802.3 LSAPs */
	int span_address_in;	/* input access list for Ethernet addresses */
	int span_pattern_in;	/* extended access list */
	int span_type_out;	/* output access list for Ethernet types */
	int span_lsap_out;	/* out access list for 802.3 LSAPs */
	int span_address_out;	/* output access list for Ethernet addresses*/
	int span_pattern_out;	/* extended access list */
	ulong number_bit_mask;  /* a bit map corresponding to unit number */
	ulong bit_mask_word;	/* word offset for bit map */
	spantype *span_ptr;	/* pointer to spanning tree structure */
	tinybool span_flag_in;   /* T if doing input type or LSAP checking */
	tinybool span_flag_out;  /* T if doing output type or LSAP checking*/
	tinybool span_latcompress;/* T if compressing LAT packets */
	tinybool span_disabled;	/* T if not xmitting spanning updates */
	int span_latsize;	/* bytes in a compressed LAT packet */
	uchar span_lat_in_svc_deny[33]; /* Input LAT group deny mask */
	uchar span_lat_in_svc_permit[33]; /* Input LAT group permit mask */
	uchar span_lat_out_svc_deny[33]; /* Output LAT group deny mask */
	uchar span_lat_out_svc_permit[33]; /* Output LAT group permit mask */
	short circ_group;	/* cct group number for load distribution */
	short circ_canforward;	/* TRUE if group is == PORT_FORWARDING */
	short circ_canlearn;	/* TRUE if group is >= PORT_LEARNING */
	short circ_inserted;	/* TRUE if interface is in flood list */
	queuetype tbridge_circuitQ; /* all the vc's that correspond to this
				       swidb gets linked here */
#endif swidb_fields_ibu

	/*
	 * Spanning tree protocol variables for a port
	 */
	int port_state;
	uchar designated_root[8];
	int designated_cost;
	uchar designated_bridge[8];
	ushort designated_port;
#ifdef swidb_fields_ibu			  /* dlclack.c and others need this */
        int path_cost;
	ushort port_id;           /* For IEEE/DEC also used for port priority*/
	/* SRB AST support */
	boolean disjoint_peers;           /* indication of mismatched peers  */
	boolean disjoint_bpdu_generation; /* TRUE if special bpdu generation */
                                          /* resulting from the detection of */
                                          /* mismatched peers is in effect   */
        ushort ibm_port_priority;         /* IBM port priority maintained    */
                                          /* here since port_id unavailable  */
	ushort ibm_message_age_increment; /* Used to fine-tune AST in mixed- */
                                          /* bridge topology where non-Cisco */
                                          /* vendors don't allow user to     */
	                                  /* manually configure ROOT MAX_AGE */
#endif swidb_fields_ibu
        tinybool path_cost_explicit;    /* path cost not based on bandwidth */
        tinybool topology_change_acknowledge;
        tinybool config_pending;
        tinybool message_age_timer_active;
        ulong message_age_timer;
        ulong forward_delay_timer;
        ulong hold_timer;
        ulong forward_transition_count;
 	/*
 	 * DECnet protocol dependent fields
 	 */
	boolean dn_designated;		 /* TRUE if we're designated router */
	uchar dn_desig_router[2];	 /* designated router */
	short dn_dr_prio;                /* DR's priority */
	boolean dn_lineup;		 /* TRUE if DECnet is up */
 	sys_timestamp dn_desig_timer; 	 /* time last saw a designed router */
 	short dn_cost;			 /* cost for routing layer */
	short dn_prio;			 /* router priority */
	int dn_accesslist;		 /* access list */
	int dn_in_filter;		 /* routing input filter */
	int dn_out_filter;		 /* routing output filter */
	int dn_route_time;
	int dn_hello_time;
	sys_timestamp dn_hello_timer;
	sys_timestamp dn_route_timer;
        ulong dn_network;                /* DECnet network number */
	int dn_maxblksize;               /* smallest blocksize seen */
	int dn_hismtu;                   /* blksize of other side - p2p only */
	short dn_startnode;		 /* start node for route round-robin */
        ushort dn_L1_adj_count;          /* # of L1 adjacencies on
					    interface */
 	ushort dn_old_phaseIV_adj_count;  /* # of Non-Phase IV+ adjacencies */
 	ushort dn_phaseIV_plus_adj_count; /* # of Phase IV+ adjacencies */
	boolean dn_split_horizon;         /* is split horizon on/off on i/f? */

	/*
	 * CDP support
	 */
	boolean cdp_enabled;            /* TRUE if CDP configured on i/f */
	sys_timestamp cdp_update_timer;  /* when to send updates on this i/f */

	/*
 	 * TARP support
 	 */
 	boolean tarp_enabled;            /* TRUE if TARP configured on i/f */
 	boolean tarp_propagate;          /* if TRUE, propagate TARP info out on
					    this i/f */

	/*
	 * Novell
	 */
	ulong novellnets;		/* novell net(s) on this interface */
	uchar ALIGN_4(novell_node[6]);	/* node address for this interface */
	tinybool novell_deleted_nidb;    /* Was an NIDB deleted on here? */
	tinybool novell_reserved;       /* Available */
	nidbtype ALIGN_4(*primary_nidb);/* primary network (for IGRP) */
	nidbtype *owner_nidb;		/* which nidb owns this client */
	int novell_static_routes;       /* active static routes */
	queuetype nov_helpernetQ;	/* novell helper address queue */
	int novell_helperlist;		/* access list for helper address */
	int novell_accesslist;		/* " " " for novell */
 	int novell_inaccesslist;	/* acl for novell data pkts (input) */
	int novell_accesslist_def;	/* default access list for novell */
	tinybool novell_output_acl_checks; /* output acl summary for fs path */
	tinybool novell_type20;         /* Forward type 20 propagation pkts */
	tinybool novell_evenize;        /* Pad odd-length (slow-switch) pkts */
	tinybool novell_permit_default; /* Override access list if dflt net */
	ulong nov_update_interval;	/* time to send out updates */
	ulong novell_sap_delay;         /* Delay between SAP update packets */
	ulong novell_rip_delay;         /* Delay between RIP update packets */
	ulong nov_in_filterlist;        /* don't listen to any routing info */
	ulong nov_gns_outfilterlist;    /* don't pass on any service info */
	ulong nov_out_filterlist;       /* don't pass on any routing info */
	ulong nov_router_filterlist;    /* don't listen to this guy */
	ulong nov_sap_infilterlist;     /* don't listen to any routing info */
	ulong nov_sap_outfilterlist;    /* don't pass on any service info */
	ulong nov_sap_routerfilterlist; /* don't listen to this guy */
	ulong nov_rip_multiplier; 	/* RIP age multiplier */
	ulong nov_sap_multiplier; 	/* SAP age multiplier */
	ulong nov_rip_maxpacketsize; 	/* RIP max packet size */
	ulong nov_sap_maxpacketsize; 	/* SAP max packet size */
	boolean nov_respond_to_gns; 	/* Respond to GNS queries? */
	ulong nov_gns_delay;            /* Time to Delay to to GNS queries */
	int novell_interval;		/* time novell incremental updates */
	ushort novell_delay;		/* Novell delay (ticks) */
	ushort reserved_ipx_1;          /* Reserved for next ipx */

	/* Support for IPX accounting */
	boolean ipx_accounting;		/* Running IPX accounting here? */
	/* Support for Novell IPXWAN */
	boolean nov_ipxwan_enabled; 	/* IPXWAN enabled for interface */

	/* Per Interface Novell IPX Counters RIP/SAP MIB */
	ulong nov_sap_out_packets;      /* Number of SAPs sent */
	ulong nov_sap_in_packets;       /* Number of SAPs received */
	ulong nov_rip_out_packets;      /* Number of RIPs sent */
	ulong nov_rip_in_packets;       /* Number of RIPs received */

	/* Snapshot protocol dependent fields */
	struct snapshot_interface_info_ *sii; /* Full descriptor block */
	   
	/* 
	 * CLNS protocol dependent fields
	 */
	boolean clns_enabled;
	uchar   *clns_net;            /* Interface specific NET. */
	tinybool clns_cluster_alias;  /* Cluster aliases enabled. */
	tinybool clns_lineup;         /* was line up last time we checked */
	tinybool clns_checksum;	      /* generate checksum? */
	tinybool clns_send_rdpdus;    /* whether to send redirects */
	mgd_timer clns_hello_timer;   /* when to send an ESH or ISH next */
	sys_timestamp clns_sched_hello_time;   /* our next scheduled time */
	uchar   *clns_hello_addr;     /* non-null if sent to one station */
	uchar   clns_hello_addrlen;
 	tinybool clns_split_enabled;   /* TRUE if split horizon is enabled */
 	tinybool clns_split_enabled_def; /* default split horizon state */
	tinybool clns_send_rdpdumasks; /* include addr masks ? */
	tinybool clns_send_erpdus;     /* whether to send error reports */
	tinybool clns_dec_compatible;  /* clns_dec_compatible */
	ushort  clns_hello_type;      /* snpa.type */
	sys_timestamp clns_rd_timer;  /* msclock of last redirect sent */
	ulong   clns_rd_interval;     /* min interval between redirects */
	sys_timestamp clns_er_timer;  /* msclock of last error report */
	ulong   clns_er_interval;     /* min interval for error reports */
	int     clns_level;           /* level 2 only ? */
	ulong   clns_filter_in;	      /* Index of input filter, or 0 */
	ulong   clns_filter_out;      /* Index of output filter, or 0 */
	ulong	clns_es_adj_filter;   /* Index of ES adjacency filter, or 0 */
	ulong	clns_is_adj_filter;   /* Index of IS adjacency filter, or 0 */
	ulong   clns_esct_time;       /* ES-IS ES suggested config time */

	/*
	 * ISIS protocol dependent fields
	 */
	isisidbtype **isis_idb;		/* For IS-IS and NLSP */

	/*
	 * ISO-IGRP fields
	 */
	ulong	   iso_igrp_adj_filter;	/* Adjacency filter index or 0 */

	/*
	 * OSPF protocol dependent fields
	 */
	ospf_idb_info *ospf_info;	/* all ospf related data */

	/*
	 * LAT support
	 */
	boolean lat_running;		/* TRUE if lat enabled */

#ifdef swidb_fields_ibu
        sap_registry_t *qsap_registry;  /* QLLC SAP registry for CLSI */
#endif swidb_fields_ibu

	/*
	 * NTP information fields
	 */
	struct ntp_idb_info *ntp_info;	/* Pointer to NTP stuff */

	/*
	 * IP host information fields
	 */
	iphostinfotype *iphost_info;

	/*
	 * IP routing information
	 */
	ipinfotype *ip_info;

	/*
	 * EIGRP protocol-independent fields
	 */
	igrp2_idblink *eigrp_info;

	/*
	 * Hot standby protocol information fields
	 */
	standbyidbtype *standbyidb;	/* Pointer to standby stuff */

	/*
	 * Virtual IDB Virtual MAC addr.
	 */
	hwaddrtype	*vaddr;		/* Virtual Interface address */
	hwaddrtype	*p2pmac;		/* P2P MAC(VCD for ATM) */

	/*
	 * LEX information fields
	 */
	struct lex_idb_info *lex_info; /* Pointer to LEX stuff */
	struct idbtype_ *lex_swidb;    /* Lex software idb, bound dynamically */
	/*
	 * Subinterface backup
	 */
	struct hwidbtype_ *backup; 	/* pointer to backup interface */
	ushort up_delay, down_delay;	/* up / down state delay for backup */
	tinybool prev_state;		/* swidb state last time through */
	tinybool backup_active;		/* True if backup is active */

        /*
         * ATM stuff -- alignment assumes this comes after 'backup_active'
         */
	tinybool atm_nsap_valid;        /* Flag for valid ATM NSAP Address */
	tinybool atmsig_mtp_flag;       /* Flag for multipoint signalling */
	hwaddrtype ALIGN_4(*nsap);	/* ATM Signalling NSAP Address */
    	int rfc1577_flavor;		/* Which type of 1577 we are running */
	struct atm_arp_config_ *atm_arp;/* RFC 1577 ATM ARP data */
	/* 
	 * Tree of p2p VCs keyed on NSAPs that could be used instead 
	 * of opening another connection to those NSAPs 
	 */ 
	struct wavl_handle_ *atm_glommable_vc_tree;	

        /*
         * SMDS protocol dependent information
         */
        smds_gleantype *smds_glean;

  	/*
	 * LAN emulation over ATM
	 */
	struct lane_info_t_ *lane_info;

	/*
	 * Commserver specific fields
	 */
	boolean async_group_leader;	/* Is an async master */
	queuetype async_groupQ;		/*  ...list of slaves */
	async_member_info		/* Internal state under master */
	*async_member_differences;

	/*
	 * NHRP
	 */
	nhrpinfotype *nhrp_info;

	/*
	 * RSVP
	 */
	rsvp_info_type *rsvp_info;              /* rsvp interface state */

	/*
	 * Tacacs+ info
	 */
	struct per_user_info_ *per_user_info;

 	/*
 	 * Virtual Template and Access specific fields
 	 */
 	queuetype vtemplate_memberQ;   	/* Q of virtual idbs cloned after it */
 	char *command_list;		/* up-to-date command list */
  	vaccess_info_block *vaccess_info; 
	vaccess_info_block *vaccess_dirtyQ_info;
	char vaccess_status;

        /*
         * VPN 
         */
	l2f_midtype *l2f_mid;
	hwidbtype *other_link;    /* for testing purposes */
	int vpn_flags;
	iqueue_t vpn_save_iqueue;
	oqueue_t vpn_save_oqueue;
       	oqueue_dequeue_t vpn_save_oqueue_dequeue;

        /*
         * MLP Bundle interface
         */
        boolean is_bundle_interface;

};

INTERFACE_GENERIC_INLINES

INTERFACE_IBU_INLINES

#ifdef interface_types

/*
 * Support for fast switching code
 */

/*
 * RXTYPE codes for packet clasification.
 *
 * Type codes returned in rxtype register on MCI/cBus.
 */

typedef enum RXTYPE {
    RXTYPE_UNKNOWN =         0,		/* no special protocol handling      */
    RXTYPE_DODIP =           1,		/* IP on Ethernet             (0x01) */
    RXTYPE_DECNET =          2,		/* DECnet Phase IV	      (0x02) */
    RXTYPE_XNS =             3,		/* XNS			      (0x03) */
    RXTYPE_CLNS =            4,		/* ISO CLNS		      (0x04) */
    RXTYPE_APPLE =           5,		/* AppleTalk version 1	      (0x05) */
    RXTYPE_NOVELL1 =	     6,		/* Novell 1                   (0x06) */
    RXTYPE_NOVELL2 =         7,		/* Novell 2                   (0x07) */
    RXTYPE_SAP_IP =          8,		/* IP on IEEE 802.3 aka HPIP  (0x08) */
    RXTYPE_APOLLO =          9,		/* Apollo Domain              (0x09) */
    RXTYPE10_UNUSED =        10,	/* unused                     (0x0A) */
    RXTYPE_VINES =           11,	/* Banyan Vines               (0x0B) */
    RXTYPE_TRANSIT =         12,	/* FDDI transit bridging      (0x0C) */
    RXTYPE_SRB =             13,	/* Token ring SRB             (0x0D) */
    RXTYPE_ISL =             14,        /* Inter Switch Link - vLANs  (0x0E) */
    RXTYPE_IEEE_802_10 =     15,        /* IEEE 802.10 - vLANs        (0x0F) */
    RXTYPE_SNAP_UNK =        16,	/* Unknown SNAP               (0x10) */
    RXTYPE_SNAP_DODIP =      17,	/* IP on SNAP on Ethernet     (0x11) */
    RXTYPE_SNAP_DECNET =     18,	/* DECnet Phase IV on SNAP    (0x12) */
    RXTYPE_SNAP_XNS =        19,	/* XNS on SNAP                (0x13) */
    RXTYPE20_UNUSED =        20,	/* unused                     (0x14) */
    RXTYPE_SNAP_APPLE =      21,	/* AppleTalk on SNAP          (0x15) */
    RXTYPE_SNAP_NOV1 =       22,	/* Novell 1 on SNAP           (0x16) */
    RXTYPE23_UNUSED =        23,	/* unused                     (0x17) */
    RXTYPE24_UNUSED =        24,	/* unused                     (0x18) */
    RXTYPE_SNAP_APOLLO =     25,	/* Apollo Domain on SNAP      (0x19) */
    RXTYPE26_UNUSED =        26,	/* unused                     (0x1A) */
    RXTYPE_SNAP_VINES =      27,	/* Banyan Vines on SNAP       (0x1B) */
    RXTYPE28_UNUSED =        28,	/* unused                     (0x1C) */
    RXTYPE_LEX =             29,	/* Lan Extension Protocol     (0x1D) */
    RXTYPE_JUMBO_CLNP =      30,	/* CLNP in need of fragmenting(0x1E) */
    RXTYPE_JUMBO_IP =        31,	/* IP in need of fragmenting  (0x1F) */
    RXTYPE_SAP_UNKN =        32,	/*  			      (0x20) */
    RXTYPE33_UNUSED =        33,        /* used to be SAP_IP	      (0x21) */
    RXTYPE_IPUNC =	     34,	/* IP unclassifed by hardware (0x22) */
    RXTYPE_IP_TUNNEL =	     35,	/* For tunnel fastswitching   (0x23) */
    RXTYPE_SAP_CLNS =	     36,	/*  			      (0x24) */
    RXTYPE37_UNUSED =	     37,	/*  			      (0x25) */
    RXTYPE_SAP_NOVELL =      38,	/*  			      (0x26) */
    RXTYPE39_UNUSED =        39,        /*                            (0x27) */
    RXTYPE40_UNUSED =        40,        /*                            (0x28) */
    RXTYPE_LANE_BUS =        41,	/*  			      (0x29) */
    RXTYPE_LANE_CONTROL =    42,	/* ATM LAN emulation          (0x2A) */
    RXTYPE_SAP_VINES =       43,	/*  			      (0x2B) */
    RXTYPE44_UNUSED =	     44,	/*  			      (0x2C) */
    RXTYPE_RFC826_ARP =	     45,	/* IP ARP		      (0x2D) */
    RXTYPE_DLSW =	     46,	/* Token Ring DLSw	      (0x2E) */
    RXTYPE_REVERSE_ARP =     47,	/* IP RARP		      (0x2F) */
    RXTYPE_LAST =	     48,	/*			      (0x30) */

    /*
     * This constant is used to dimension the per-interface rxtype_flags
     * arrays.  It replaces RPF_MAX_ROUTING_PROTOCOLS which was 256 and
     * used to dimension the now obsolete routing_protocols array.
     */
    MAX_RXTYPES	 =	     49
} rxtype_t;



/*
 * Bits to indicate whether we are fastswitching a specific 
 * protocol. These bits should be set in "fast_switch_flag" field 
 * in interface block.  
*/
#define FSF_DODIP	(1<<RXTYPE_DODIP)
#define FSF_DECNET	(1<<RXTYPE_DECNET)
#define FSF_XNS		(1<<RXTYPE_XNS)
#define FSF_CLNS	(1<<RXTYPE_CLNS)
#define FSF_APPLE	(1<<RXTYPE_APPLE)
#define FSF_NOVELL1	(1<<RXTYPE_NOVELL1)
#define FSF_NOVELL2	(1<<RXTYPE_NOVELL2)
#define FSF_APOLLO	(1<<RXTYPE_APOLLO)
#define FSF_SNAP_APPLE	(1<<RXTYPE_SNAP_APPLE)
#define FSF_VINES	(1<<RXTYPE_VINES)
#define FSF_SRB		(1<<RXTYPE_SRB)
#define FSF_LEX		(1<<RXTYPE_LEX)

#define NONSLOTTED_IDB_NAMES		1
#define SLOTTED_IDB_NAMES		2
#define EXTENDED_SLOTTED_IDB_NAMES	3

#define PA_VIRTUAL_SLOT_BOUNDARY	8
#define PA_VIRTUAL_SLOT_BITS		3
#define PA_BAY_0			0
#define PA_BAY_1			1
#endif interface_types

/*
 * Interesting macros/inlines
 *
 * A number of the following were converted from macros to inline functions
 * because they inadvertently evaluated a parameter more than once.  In
 * general, it is dangerous to refer to a parameter in a macro more than
 * once, since the reference can have side-effects (such as a procedure
 * call that allocates a data structure).  Inline functions are guaranteed
 * to evaluate the parameter only once.
 *
 * Macros (with names in all caps) are defined for backward compatibility.
 * Please use the inline name (lower case) for new/changed code.
 */

/*
 *  Inline to determine if an idb is using LAPB
 */
static inline boolean is_lapb (const hwidbtype *hwidb)
{
    return (hwidb
	    && (   (hwidb->enctype == ET_LAPB) || (hwidb->enctype == ET_X25)
		|| ((hwidb->enctype == ET_PPP) && (hwidb->ppp_lapb))));
}

/*
 *  Inline to determine if an idb is using Single protocol LAPB only
 */
static inline boolean is_slapb_only (const hwidbtype *hwidb)
{
    return (hwidb
	    && (   (hwidb->enctype == ET_LAPB)
		&& (hwidb->lapb_protocol != LINK_ILLEGAL)));
}

/*
*  Inline to determine if an idb is using Multiprotocol LAPB only
*/
static inline boolean is_mlapb_only (const hwidbtype *hwidb)
{
    return (hwidb
	    && (   (hwidb->enctype == ET_LAPB)
		&& (hwidb->lapb_protocol == LINK_ILLEGAL)));
}
#ifdef interface_generic_inlines

/*
 *  Inline to determine if an idb is using X.25
 */
static inline boolean is_x25 (const hwidbtype *hwidb)
{
    return (hwidb && (hwidb->enctype == ET_X25));
}

static inline boolean is_smds (const hwidbtype *hwidb)
{
    return (hwidb && (hwidb->enctype == ET_SMDS));
}
#endif interface_generic_inlines

static inline boolean is_isdn (const hwidbtype *hwidb)
{
    return (hwidb && (hwidb->isdn_info));
}
#ifdef interface_generic_inlines

/*
 * Inline to requeue a packet at the front of the output queue it came
 * from.
 */

static inline void pak_requeue_head(hwidbtype *idb, paktype *pak)
{
    (*idb->oqueue)(idb, pak, HEAD);
}
#endif interface_generic_inlines
#ifdef interface_types

/*
 * Macro to traverse all hardware IDBs
 */

#define FOR_ALL_HWIDBS(hw) \
    for ((hw) = (hwidbtype *) hwidbQ->qhead; (hw); (hw) = (hw)->next)

/*
 * Macro to traverse all software IDBs
 */

#define FOR_ALL_SWIDBS(idb) \
    for ((idb) = (idbtype *) swidbQ->qhead; (idb); (idb) = (idb)->next)

/*
 * Macro to traverse all software IDBs associated with a hardware IDB
 */

#define FOR_ALL_SWIDBS_ON_HW(hwidb, idb) \
    for ((idb) = (hwidb)->firstsw; (idb); (idb) = (idb)->nextsub)
#endif interface_types
#ifdef interface_generic_inlines

/*
 * Inline to return hwidb or null, given swidb or null
 */
static inline hwidbtype *  hwidb_or_null (idbtype *idb)
{
    return ((idb != NULL) ? idb->hwptr : NULL);
}

/*
 * Inline to return master swidb or null, given hwidb or null
 */
static inline idbtype *firstsw_or_null (hwidbtype *idb)
{
    return ((idb != NULL) ? idb->firstsw : NULL);
}

/*
 * Inline to return hwidb or null, given index (hw_if_index)
 */
static inline hwidbtype *hwidb_or_null_from_index (ulong index)
{
    hwidbtype *hwidb;

    FOR_ALL_HWIDBS(hwidb)
	if (hwidb->hw_if_index == index)
	    return (hwidb);
    return (NULL);
}
#endif interface_generic_inlines
#ifdef interface_types

/*
 * Macro to return head of swidbQ
 */
#define SWIDBQ_HEAD ((idbtype *)swidbQ->qhead)

/*
 * Macro to return head of hwidbQ
 */
#define HWIDBQ_HEAD ((hwidbtype *)hwidbQ->qhead)
#endif interface_types
#ifdef interface_generic_inlines

/*
 * vLAN routeable/switchable protocol types for per-vLAN accounting.
 *
 * This identifies the protocols we handle, no more, no less ...
 */

enum VLAN_PROTOCOLS {

    VLAN_ROUTE_IP,
    VLAN_ROUTE_NOVELL,
    VLAN_ROUTE_APPLE,
    VLAN_TBRIDGE,
    VLAN_ROUTE_DECNET,
    VLAN_ROUTE_XNS,
    VLAN_ROUTE_CLNS,
    VLAN_ROUTE_APOLLO,
    VLAN_ROUTE_VINES,
    VLAN_PROTOCOLS_MAX
};

/*
 * Returns TRUE if the idb is a subinterface IDB (not the first 
 * software IDB on the hardware interface)
 */
static inline boolean is_subinterface (const idbtype *idb)
{
    return (idb && (idb->sub_number != 0));
}

/*
 * Returns TRUE if the swidb specified is the first
 * of possibly more than one swidb associated with a
 * hardware idb.
 */
static inline boolean is_first_swidb (const idbtype *idb)
{
    return (idb && (idb->sub_number == 0));
}
#endif interface_generic_inlines

/*
 * There follows definitions specific to Virtual LANs.
 */

/*
 * Return TRUE is this is an interface that can support
 * 802.10 SDE Encapsulation.
 */
static inline boolean is_sde_capable (const idbtype *idb)
{
    hwidbtype	*hwidb;

    if (idb && is_subinterface(idb)) {
	if (idb->hwptr) {
	    hwidb = idb->hwptr;
	    if (   (hwidb->status & (IDB_ETHER | IDB_TR))
		|| ((hwidb->status & IDB_FDDI) && !hwidb->bridge_transit)
		|| ((hwidb->status & IDB_SERIAL) && hwidb->enctype == ET_HDLC))
		return (TRUE);
	}
    }
    return (FALSE);
}

/*
 * Is this an interface (Fast Ethernet only)
 * capable of supporting ISL vLAN encapsulation?
 */
static inline boolean is_isl_vlan_capable (const idbtype *idb)
{
    return (idb && is_subinterface(idb)
	    && (idb->hwptr && idb->hwptr->type == IDBTYPE_FEIP));
}

/*
 * Is this an interface that can support vLAN encapsulation?
 */
static inline boolean is_vlan_capable (const idbtype *idb)
{
    return ((idb && is_sde_capable(idb)) || is_isl_vlan_capable(idb));
}

#ifdef interface_generic_inlines

/*
 * Inlines to determine point-to-pointedness of an interface.
 *
 * In general, the inline forms should not be used;  compiled versions
 * can be found in network.c (and are called from the corresponding
 * macros).
 */

/*
 * is_p2p_hw_inline
 *
 * Returns TRUE if the hardware IDB is point-to-point.
 */
static inline boolean is_p2p_hw_inline (const hwidbtype *hwidb)
{
    return (hwidb &&
	    ((hwidb->status & IDB_TUNNEL) ||
	     (hwidb->status & IDB_VTEMPLATE) ||
	     ((hwidb->status & IDB_SERIAL) &&
	      ((hwidb->enctype == ET_HDLC) ||
	       (hwidb->enctype == ET_PPP) ||
	       (hwidb->enctype == ET_LAPB) ||
	       (hwidb->enctype == ET_STUN) ||
	       (hwidb->enctype == ET_SLIP)))));
}

/*
 * is_p2p_inline
 *
 * Returns TRUE if the software IDB is point-to-point.
 */
static inline boolean is_p2p_inline (const idbtype *idb)
{
    return (idb &&
	    ((idb->subif_link_type == SUBIF_TYPE_P2P) ||
	     (is_p2p_hw_inline(idb->hwptr))));
}

/*
 * Inline returns TRUE if the idb is the only one on the hardware interface
 */
static inline boolean is_only_swidb (const idbtype *idb)
{
    return((is_first_swidb(idb) && (idb->nextsub == NULL)));
}
#endif interface_generic_inlines

extern void idb_mark_throttled(hwidbtype *idb);
extern void idb_mark_unthrottled(hwidbtype *idb);
extern boolean idb_is_throttled(hwidbtype *idb);

/*
 *  Inline to determine if an idb should use a short transmit queue
 *
 *  Some features and protocols require the transmit buffer queue size
 *  be sharply limited for best performance.
 *  Priority queueing needs to judge the best packet to send when it has
 *  an opportunity to send.  LAPB needs to acknowledge incoming frames
 *  quickly for best throughput.
 */
static inline boolean hwidb_use_short_tx_q_inline (hwidbtype *hwidb)
{
    if (QUEUE_RED == hwidb->queue_algorithm)
	return FALSE;
    if (hwidb->priority_list)
	return TRUE;
    if (is_lapb(hwidb))
	return TRUE;
    return FALSE;
}
#ifdef interface_generic_inlines

/*
 * Inline to determine if an interface uses IEEE 802 style headers 
 * and addresses
 */
static inline boolean is_ieee802 (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->status & IDB_IEEEMASK));
}

/*
 * Inline to determine if an interface is an ethernet
 */
static inline boolean is_ethernet (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->status & IDB_ETHER));
}

/*
 * Inline to determine if an interface is a token ring
 */
static inline boolean is_tokenring (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->status & IDB_TR));
}

/*
 * Inline to determine if an interface is a serial line
 */
static inline boolean is_serial (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->status & IDB_SERIAL));
}

static inline boolean is_hdlc (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->enctype == ET_HDLC));
}

static inline boolean is_async (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->status & IDB_ASYNC));
}

static inline boolean is_sync (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->status & IDB_SYNC));
}

static inline boolean is_ppp (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->enctype == ET_PPP));
}

static inline boolean is_slip (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->enctype == ET_SLIP));
}

static inline boolean is_ppp_or_slip (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (   (hwidb->enctype == ET_PPP)
			      || (hwidb->enctype == ET_SLIP)));
}

static inline boolean is_cpp (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->enctype == ET_CPP));
}

/*
 * Inline to determine if an interface is a FDDI
 */
static inline boolean is_fddi (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->status & IDB_FDDI));
}
#endif interface_generic_inlines

static inline boolean is_fddi_br_encap (const hwidbtype *hwidb)
{
    return (   hwidb != NULL
	    && (hwidb->status & IDB_FDDI) && (hwidb->bridge_transit));
}
#ifdef interface_generic_inlines

static inline boolean is_lan (const hwidbtype *hwidb)
{
    return (   hwidb != NULL
	    && (hwidb->status & (IDB_FDDI | IDB_ETHER | IDB_TR)));
}

/*
 * Inline to determine if an interface is running frame relay
 */
static inline boolean is_frame_relay (const hwidbtype* idb)
{
    return (idb != NULL && (idb->enctype == ET_FRAME_RELAY));
}
#endif interface_generic_inlines

/*
 * Inline to determine if an interface is doing DDR.  Assumes non null argument.
 */
static inline boolean is_ddr (const hwidbtype* idb)
{
    if (idb->dialerdb != NULL)
	return(TRUE);
    else
	return(FALSE);
}
/*
 * Inline to determine if an interface is doing DDR.  Deals with null argument.
 */
static inline boolean if_is_ddr (const hwidbtype* idb)
{
    return (idb != NULL && is_ddr(idb));
}

/*
 * is_multilink
 * return TRUE if multilink enabled on a hwidb.
 */
static inline boolean is_multilink (const hwidbtype *idb)
{
  if (idb->mlp_link)
      return(TRUE);
  else
      return(FALSE);
}

/*
 * if_is_nbma
 * Return TRUE if interface connects to a non-broadcast multi-access (NBMA)
 * network.
 */
static inline boolean if_is_nbma (const idbtype *swidb, enum LINK link)
{
    hwidbtype *hwidb;

    hwidb = swidb->hwptr;
    /*
     * Only consider certain WAN media
     */
    if (!(is_x25(hwidb) || is_frame_relay(hwidb) || is_smds(hwidb)))
	return (FALSE);
    /*
     * If media supports multicast, then not NBMA
     */
    if ((hwidb->can_mcast != NULL) && (hwidb->can_mcast)(hwidb, link))
	return (FALSE);
    if (is_p2p(swidb))
	return (FALSE);
    return (TRUE);
}

#ifdef interface_generic_inlines

extern boolean is_stun_sdlc_encap(const hwidbtype *hwidb);
/* implementation temporarily in if/interface.c till more subblocks done */

static inline boolean is_sdlc (const hwidbtype *hwidb)
{
    return (hwidb != NULL
	    && (   (   (hwidb->enctype == ET_SDLCP)
		    || (hwidb->enctype == ET_SDLCS)
		    || (hwidb->enctype == ET_SDLC))
		|| (   (hwidb->enctype == ET_STUN)
                    && is_stun_sdlc_encap(hwidb))));
}


static inline boolean is_lex (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->status & IDB_LEX));
}

static inline boolean is_cip (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->type == IDBTYPE_CIP));
}

static inline boolean is_cip_lan (const hwidbtype *hwidb)
{
    return (hwidb && (hwidb->type == IDBTYPE_CIP_LAN_ADAPTER));
}
#endif interface_generic_inlines

static inline boolean is_cip_virtual (const hwidbtype *hwidb)
{
    return (hwidb != NULL && is_cip(hwidb) && (hwidb->slotunit == 2));
}
#ifdef interface_generic_inlines

static inline boolean is_srb_hw (const hwidbtype *hwidb)
{
    return (   hwidb != NULL
	    && (   (hwidb->status & IDB_TR)
		      || (hwidb->status & IDB_FDDI)
		      || is_cip_lan(hwidb)));
}

static inline boolean is_bstun (const hwidbtype *hwidb)
{
    return (hwidb != NULL && hwidb->enctype == ET_BSTUN);
}
#endif interface_generic_inlines

static inline boolean is_g703 (const hwidbtype *hwidb)
{
    return (hwidb != NULL
	    && 	(   (hwidb->app_type == APPTYPE_G703_BAL)
		 || (hwidb->app_type == APPTYPE_G703_UNBAL)));
}
#ifdef interface_generic_inlines

static inline boolean is_tunnel (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->status & IDB_TUNNEL));
}

static inline boolean is_vip (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->status & IDB_VIP) != 0);
}

static inline boolean is_valid_vip (const hwidbtype *hwidb)
{
    return (hwidb != NULL && is_vip(hwidb));
}

static inline boolean is_atmhw (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->status & IDB_ATM));
}

static inline boolean is_atm (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->enctype == ET_ATM));
}

static inline boolean is_atm_dxi (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->enctype == ET_ATM_DXI));
}

static inline boolean is_wan (const hwidbtype *hwidb)
{
    return (is_x25(hwidb) || is_frame_relay(hwidb) || is_smds(hwidb)
	    || is_atm(hwidb) || is_atm_dxi(hwidb));
}
#endif interface_generic_inlines

static inline boolean is_bvi (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->status & IDB_BVI));
}

static inline boolean is_ospf_vl (const hwidbtype *hwidb)
{
    return (hwidb != NULL && (hwidb->type == IDBTYPE_OSPF_VL));
}


/*
 * IDB sub-block inlines.
 *
 * There is no range checking here and no support for dynamic
 * sub-blocks.
 *
 * These functions are to be used ONLY in those places where
 * the performance concerns justify doing so. Examples of
 * such usage include the fast-switching paths and other code
 * which runs in the context of an interrupt.
 *
 * For documentation on these functions, see sys/if/interface.c and
 * search for the function which has the same name as the function
 * below, but without the "_inline" suffix.
 */

static inline void *
idb_get_swsb_inline (const idbtype *idb, const swidb_sb_t type)
{
    return (idb->subblock.sb[type].user_ptr);
}

static inline void *
idb_get_hwsb_inline (const hwidbtype *idb, const hwidb_sb_t type)
{
    return (idb->subblock.sb[type].user_ptr);
}

static inline void *
idb_use_swsb_inline (idbtype *idb, swidb_sb_t type)
{
    idb->subblock.sb[type].usage++;
    return (idb->subblock.sb[type].user_ptr);
}

static inline void *
idb_use_hwsb_inline (hwidbtype *idb, hwidb_sb_t type)
{
    idb->subblock.sb[type].usage++;
    return (idb->subblock.sb[type].user_ptr);
}

static inline boolean
idb_release_swsb_inline (idbtype *idb, swidb_sb_t type)
{
    idb->subblock.sb[type].usage--;
    return (TRUE);
}

static inline boolean
idb_release_hwsb_inline (hwidbtype *idb, hwidb_sb_t type)
{
    idb->subblock.sb[type].usage--;
    return (TRUE);
}
#ifdef interface_types

extern char *rxtype_string(rxtype_t);
extern char *print_snpa_string (char *buffer, hwaddrtype *snpa);

/* from if/tring.h:  destined for interface_types  */
iqueue_t tring_parse_packet(hwidbtype *iadb, paktype *pak, uchar *data);


#define RXF_ROUTING     0x01
#define RXF_BRIDGING    0x02

#define SET_ROUTING_RXTYPE(swidb, a) \
    ((swidb)->rxtype_flags[(a)] |= RXF_ROUTING)
#define CLEAR_ROUTING_RXTYPE(swidb, a) \
    ((swidb)->rxtype_flags[(a)] &= ~RXF_ROUTING)
#define ROUTING_RXTYPE(swidb, a) \
    ((swidb)->rxtype_flags[(a)] & RXF_ROUTING)

/*
 * For a given protocl, bridging and routing used to share a common
 * indicator since they are in contradiction to each other. In
 * other words, routing a protocol meant we were not bridging it
 * and vice versa,
 *
 *  #define BRIDGING_RXTYPE(swidb, a) (!ROUTING_RXTYPE(swidb, a))
 *
 * With the introduction of IRB(Integrated Routinga and Bridging),
 * bridging or routing a given protocol is independent; So now
 * it has been generalized that bridging and routing use different
 * indicator regardless IRB is enabled or not.
 *
 * The default when bridging is configured is to bridge the given
 * protocol by not setting it. In other words, for a given RXTYPE,
 * CLEAR means bridged, SET means not bridged.
 *
 */

#define SET_BRIDGING_RXTYPE(swidb, a) \
((swidb)->rxtype_flags[(a)] |= RXF_BRIDGING)
#define CLEAR_BRIDGING_RXTYPE(swidb, a) \
((swidb)->rxtype_flags[(a)] &= ~RXF_BRIDGING)
#define BRIDGING_RXTYPE(swidb, a) \
!((swidb)->rxtype_flags[(a)] & RXF_BRIDGING)
 
#endif interface_types
#ifdef interface_generic_inlines

/* interface_up_simple - quickly determine if the interface is up.  This
 * is intended to replace checks for "idb->state != IDBS_UP" in the brave
 * new world of subinterfaces.  Whether or not those references should
 * actually call interface_up is beyond the scope of the project...
 */
static inline boolean interface_up_simple (const idbtype *idb)
{
    if (idb->subif_state != SUBIF_STATE_UP)
	return(FALSE);
    if (idb->hwptr->state != IDBS_UP)
	return(FALSE);
    return(TRUE);
}

/*
 * interface_admin_down - determine if the interface is administratively
 * down.  This replaces checks for idb->state == IDBS_ADMINDOWN.
 */

static inline boolean interface_admin_down (const idbtype *idb)
{
    if ((idb->subif_state == SUBIF_STATE_DELETED) ||
	(idb->subif_state == SUBIF_STATE_ADMINDOWN))
	return(TRUE);
    if (idb->hwptr->state == IDBS_ADMINDOWN)
	return(TRUE);
    return(FALSE);
}

static inline boolean holdq_enqueue_fast (hwidbtype *idb, paktype *pak) {
   
    ushort bytes;			/* No. of bytes in a packet */
    ushort acct_proto;			/* Accounting protocol type */
    bytes = pak->datagramsize;
    acct_proto = pak->acct_proto;	
    if (idb->oqueue(idb, pak, TAIL)) {
	/*
	 * Do Accounting for successfully enqueued packets
	 * only if the accounting protocol is valid.
	 */ 
        if (acct_proto != ACCT_PROTO_LAST) { 
        idb->counters.tx_cumbytes_fast[acct_proto] += bytes;
        idb->counters.outputs_fast[acct_proto]++;
        }
     return(TRUE);
     }
     return(FALSE);
}

/*
 * Return the pre-configured holdqueue length or the appropriate
 * default value (appropriate based upon configured queueing model).
 * holdqueue_out values:
 *     Null      - value not configured
 *     !Null     - indicates configured value
 */
static inline int hwidb_holdqueue_size_or_default(hwidbtype *idb)
{
     return ( (idb->holdqueue_out) ? (idb->holdqueue_out) :
	      (idb->output_defqcount[DEFAULT_QUEUE(idb)]) );
}

/* definition of these is in network.c !! gcox 
 * these decl's can't go in network.h without including linktype.h there
 *  .. so maybe that's what we should do...
 */
ulong max_proto_mtu(idbtype *, enum LINK);
ulong def_proto_mtu(idbtype *, enum LINK);
void  if_multi_add(hwidbtype *, idbtype *, uchar *);

#endif interface_generic_inlines

#else

struct hwidbtype_ { int dummy; } deliberate_compile_error;
/*
 * If you see an error here, the real cause of the problem is that 
 * you have included both interface_generic.h and interface_private.h.
 * These are incompatible, and only one can be included.
 * Therefore .h files which include one cannot be included 
 * in .c files which include the other or which include a .h file  
 * which includes the other.
 */

#endif __INTERFACE_GENERIC_H__
#endif __INTERFACE_PRIVATE_H__
