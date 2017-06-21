/* $Id: lec.c,v 3.12.6.28 1996/09/12 18:27:53 michellw Exp $
 * $Source: /release/112/cvs/Xsys/lane/lec.c,v $
 *------------------------------------------------------------------
 * lec.c -- LAN Emulation Client
 *
 * October 1994, Brad Benson
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lec.c,v $
 * Revision 3.12.6.28  1996/09/12  18:27:53  michellw
 * CSCdi61979:  ATM: mismatch of known Vcd cause applications to block
 * Branch: California_branch
 * For lec_delete_nsap_entry(), passed in the pointer to the
 * lec_nsap_info_t to be deleted instead of the pointer to
 * the nsap address.
 *
 * Revision 3.12.6.27  1996/09/09  22:49:04  michellw
 * CSCdi68089:  LANE data direct down; Resolution: backout CSCdi61979
 * Branch: California_branch
 * Backout the fix committed for CSCdi61979
 *
 * Revision 3.12.6.26  1996/08/28  12:54:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.6.25  1996/08/22  20:31:57  michellw
 * CSCdi61979:  ATM: mismatch of known Vcd cause applications to block
 * Branch: California_branch
 * For loopback VC, there will be two entries in lec_info->nasp_table
 * with the same nsap address. Use callid to uniquely locate the entry.
 *
 * Revision 3.12.6.24  1996/08/16  23:00:19  rbadri
 * CSCdi66287:  LANE: Clean up IPX/OSPF releated initializaion
 * Branch: California_branch
 * Move IPX & OSPF related registry invocations to lec_parser.c
 *
 * Revision 3.12.6.23  1996/08/12  06:13:58  bbenson
 * CSCdi65719:  Rearrange deck chairs.  create/delete client belongs in
 * lec_parser.c
 * Branch: California_branch
 *
 * Revision 3.12.6.22  1996/07/31  16:07:20  bbenson
 * CSCdi45072:  Restarting a LEC causes it to lose VLAN identity
 * Branch: California_branch
 *
 * Revision 3.12.6.21  1996/07/30  14:42:53  michellw
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
 * Revision 3.12.6.20  1996/07/26  06:04:46  ioakley
 * CSCdi63958:  lec_create_client() contains unnecessary code.
 * Branch: California_branch
 *
 * Revision 3.12.6.19  1996/07/25  00:16:41  rbadri
 * CSCdi63919:  LANE Client can invalidate fast cache entries too often
 * Branch: California_branch
 * New flag to facilitate better cache invalidation when VCs go down.
 *
 * Revision 3.12.6.18  1996/07/09  05:59:51  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.12.6.17  1996/07/02  22:19:55  michellw
 * CSCdi61813:  LEC was not invoked when the timer expired
 * Branch: California_branch
 * Changed LEC, LES/BUS and LECS process priority to NORMAL.
 *
 * Revision 3.12.6.16  1996/07/02  00:10:18  rbadri
 * CSCdi55849:  LANE: IOS still believe it is non-broadcast interface
 * Branch: California_branch
 * While configuring an LEC, make the sub-interface  broadcast capable,
 * if OSPF is configured on it.
 *
 * Revision 3.12.6.15  1996/07/01  17:10:48  michellw
 * CSCdi61778:  vlanid is cleared before the lec state machine
 * Branch: California_branch
 * Zero out vlanid after the lec state machine is completed.
 *
 * Revision 3.12.6.14  1996/06/28  23:20:26  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.12.6.13  1996/06/27  21:42:21  rbadri
 * CSCdi61523:  Can not configure MTU on a sub-if greater than hwidb (for
 * ATM)
 * Branch: California_branch
 * When LANE client is configured, change the MTU  appropriately.
 *
 * Revision 3.12.6.12  1996/06/27  17:54:34  yanke
 * Show map group that causes lane conflict.
 * Remove mapgroup if no arp server left.
 *
 * CSCdi60385:  LECs cannot rejoin ELAN after changing ilmi prefix on a
 * port
 * Branch: California_branch
 *
 * Revision 3.12.6.11  1996/06/25  18:42:05  rbadri
 * CSCdi61089:  c4500 crash upon shutdown of atm0 in LANE BATs
 * Branch: California_branch
 * When a "no int a0.3" is done, make sure to disable the client and
 * release all lsv/BUS info.
 *
 * Revision 3.12.6.10  1996/06/18  19:15:09  rbadri
 * CSCdi60067:  IPX rip TR->TR through bridged ELAN works intermittently.
 * Branch: California_branch
 * Clean up a little on the LEC side.
 *
 * Revision 3.12.6.9  1996/06/18  01:32:23  cyoung
 * CSCdi60671:  tr lane support for VTP
 * Branch: California_branch
 *
 * Revision 3.12.6.8  1996/06/06  18:39:06  michellw
 * CSCdi59652:  make generic function for creating/deleting LEC
 * Branch: California_branch
 * The functions, lec_create_client() and lec_delete_client(), are
 * created to do the job.
 *
 * Revision 3.12.6.7  1996/06/05  02:01:25  rbadri
 * CSCdi59338:  TR->TR through bridged ELAN works intermittently
 * Branch: California_branch
 * Move setting of C4_proxy to a_activate_lec. Update bridge table
 * entries more tightly. When creating le-arp entries on the fly, due
 * to an incoming le-arp req, use the src_mac as the mac in le-arp entry.
 *
 * Revision 3.12.6.6  1996/05/31  16:50:33  michellw
 * CSCdi46369:  LEC remembers ELAN name even if it was never specified
 * Branch: California_branch
 * Not to save elan_name to lane_info->name when processing the
 * join_rsp message.
 *
 * Revision 3.12.6.5  1996/05/30  21:32:55  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.12.6.4  1996/05/23  14:29:26  kjadams
 * CSCdi58174:  MTU size resets to 1500 on an atm subif configured with a
 * TR LEC
 * Branch: California_branch
 * MTU was reset to 1500 on "no shut".  If lec is a TR LEC, reset to
 * 4544.
 *
 * Revision 3.12.6.3  1996/05/17  23:26:36  rbadri
 * CSCdi56574:  LANE clients go down/up during 500 station multiping
 * Branch: California_branch
 * Flush requests are to be sent in the context of the client process
 * rather than signalling process. Throttle the number of requests to be
 * sent and be generous about giving up the CPU.
 *
 * Revision 3.12.6.2  1996/05/09  14:32:46  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.12.6.1.18.1  1996/04/27  07:06:39  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.12.6.1.8.15  1996/04/26  00:41:48  michellw
 * Branch: LE_Cal_V112_0_2_branch
 * Support "lane clear client join-cnt" command.
 *
 * Revision 3.12.6.1.8.14  1996/04/23  17:20:46  rbadri
 * CSCdi54446:  Lane static le-arps once configured cannot be removed
 * Branch: LE_Cal_V112_0_2_branch
 * Move the NOPREFIX to include the mac_address for which we are
 * deleting the le_arp entry.
 *
 * Revision 3.12.6.1.8.13  1996/04/21  01:17:11  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Create a new sub-system for TRLANE.
 *
 * Revision 3.12.6.1.8.12  1996/04/19  03:01:05  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Initialize TR-LANE from (lane)parser, if not already done.
 *
 * Revision 3.12.6.1.8.11  1996/04/18  01:53:38  cakyol
 * I forgot to add "reg_add_lec_hsrp_action" to lec.c once I
 * removed it from lane.c, during the initialization cleanup.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.12.6.1.8.10  1996/04/17  23:14:44  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Convert TokenRing LANE related functions to registries
 * Move contents of lec_srb* srt/srb_lane.[ch], to facilitate
 * creation of new sub-system.
 *
 * Revision 3.12.6.1.8.9  1996/04/16  02:44:37  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.12.6.1.8.8  1996/04/15  09:06:43  shj
 * CSCdi52532:  LANE invalidates protocol fast caches too often
 * Branch: LE_Cal_V112_0_2_branch
 * Don't do bump_cache_version for dying LE-ARP's unless data-direct path
 * was actually in use.  Also add some debugging to see dying LE-ARP's.
 *
 * Revision 3.12.6.1.8.7  1996/04/15  07:26:59  shj
 * CSCdi52538:  IP multicast packets do not fastswitch to LANE
 * Branch: LE_Cal_V112_0_2_branch
 * Allow BUS path to be added to fast cache if destination is multicast.
 * Suppress fast cache only if BUS path will be replaced later with direct
 * path.
 *
 * Revision 3.12.6.1.8.6  1996/04/13  23:13:05  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * - Don't call tr_lane_start, llc2_start or start_srb_background unless
 *   tr-lane is configured.
 * - First shot at calling lec_set_maxmtu to set the tr-lane MTU.
 * - Remove lane_type from lec_muex_config.  It's not needed.
 * - Only do the tr-lane config of srb when tr-lane is configured.
 *
 * Revision 3.12.6.1.8.5  1996/04/11  05:33:47  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Correct botched lecs.c sync
 * - Modify indentation to appease the pedantic
 *
 * Revision 3.12.6.1.8.4  1996/04/11  00:04:30  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Track manual configuration of MTU on swidb basis
 *
 * Revision 3.12.6.1.8.3  1996/04/09  02:16:38  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.12.6.1.8.2  1996/04/08  14:51:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.12.6.1.8.1  1996/04/08  01:57:03  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.12.6.1  1996/03/18  20:32:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.12.10.7  1996/04/06  02:52:22  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * In LE-ARP requests, fill in the source MAC of the originator and not
 * the MAC of client.
 *
 *
 * Revision 3.12.10.6  1996/04/03  02:39:31  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * Client state machine is event driven instead of packet driven.
 *
 * Revision 3.12.10.4  1996/03/28  23:34:29  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * Bug fix for map-group on major interface.
 *
 * Revision 3.12.10.3  1996/03/28  17:34:43  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.12.10.2  1996/03/25  02:19:58  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.12.10.1  1996/03/22  22:54:49  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.9.4.2  1996/03/22  16:18:30  cyoung
 * Add function to find LEC by vlan id
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.9.4.1  1996/02/27  21:10:27  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.9  1996/02/05  18:06:56  rbadri
 * CSCdi47734:  LEC responds to LE_ARP requests even in blocked state.
 * Add another registry call for LE_ARP responses. Respond to LE_ARP
 * requests only if LEC is in Forwarding state and the MAC is not learned
 * through the LEC.
 *
 * Revision 3.8  1996/01/22  06:35:54  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7.4.1  1996/02/29  07:48:52  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.7.2.6  1996/02/16  21:38:25  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Scheduler interface changes for LANE client code.
 *
 * Revision 3.7.2.5  1996/02/15  19:18:04  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.7.2.4  1996/02/13  00:17:55  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.7.2.3.2.7  1996/03/27  17:58:02  kjadams
 * Branch: TRlane_branch
 * use GETSHORT to pull elan ring number from config response.
 *
 * Revision 3.7.2.3.2.6  1996/03/27  00:23:14  ddecapit
 * Branch: TRlane_branch
 *
 * code review comments part II
 *
 * Revision 3.7.2.3.2.5  1996/03/23  21:13:11  kjadams
 * Branch: TRlane_branch
 * LES/BUS support for token ring LANE has been added.
 * TR LEC now has a new state for registration - to register the virtual
 *    ring number and bridge (route descriptor) with the LES.
 * BLLI code points for token ring have also been added for TR BUS
 *    connections.
 *
 * Revision 3.7.2.3.2.4  1996/03/21  03:26:19  ddecapit
 * Branch: TRlane_branch
 *
 * RSP Support
 * Code Review Comments
 *
 * Revision 3.7.2.3.2.3  1996/03/06  18:52:54  ddecapit
 * Branch: TRlane_branch
 *
 * TR LANE debug
 * Remove tr_lane_explorer
 *
 * Revision 3.7.2.3.2.2  1996/03/05  04:47:14  ddecapit
 * Branch: TRlane_branch
 * Token Ring B-LLI for LAN Emulation
 * Fix copyright
 *
 * Revision 3.7.2.3.2.1  1996/02/26  23:07:44  ddecapit
 * Branch: TRlane_branch
 *
 * Revision 3.7.2.3  1996/01/24  01:36:58  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.7.2.2  1996/01/23  23:54:15  cakyol
 * HSRP over LANE Code review changes:
 * - "interface_up" now works properly with HSRP after
 *   Badri's network.c fix.
 * - additional HSRP mac addresses implemented much
 *   faster on the LANE client.
 * - starting an already running or stopping an already
 *   stopped LANE client now behaves properly.
 * - white space cleanup
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.7.2.1  1996/01/11  01:46:28  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.7  1996/01/05  04:56:11  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.6  1995/12/16  00:26:26  pitargue
 * CSCdi45515:  ST MAC and all ones MAC are off by 2 bytes in show bridge
 * fix comments and optimize a if statement
 *
 * Revision 3.5.8.1  1995/12/29  01:06:47  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.5  1995/11/28  18:35:28  rbadri
 * CSCdi44531:  LANE client doesnt set the proxy bit due to local config
 * change.
 *
 * Revision 3.4  1995/11/23  17:32:45  bbenson
 * CSCdi43159:  Unresolved LE-ARPs never stop ARPingq
 * Removed autodecrement of arp_retry_count.
 *
 * Revision 3.3  1995/11/20  18:35:58  rbadri
 * CSCdi38662:  Configuring bridging on multiple ATM sub-interfaces causes
 *              hardware reset
 * When a sub-interface is added to the bridge-group, initiate a config
 * change rather than resetting the ATM interface.
 *
 * Revision 3.2  1995/11/17  17:50:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.17  1995/11/08  21:12:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.16  1995/09/27  00:45:14  shj
 * CSCdi38974:  reset the switch off one AIP would bring down ELANs of
 * another AIP
 * On new prefix from switch, notify only those clients associated with
 * the interface on which the prefix arrived, not all clients.
 *
 * Revision 2.15.8.3.2.1  1996/02/03  07:07:07  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.15  1995/09/08  23:45:45  bbenson
 * CSCdi36632:  LEC uses an un-initialized timer
 * Missing break in switch for LE-ARP timeouts.  Bad dog!
 *
 * Revision 2.14  1995/08/31  02:08:35  bbenson
 * CSCdi39584:  set sdu sizes per connection - part 1
 * - Staging of the LANE/LEC part of this fix.
 *
 * Revision 2.13.16.10.2.1  1996/01/19  21:56:47  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.13.16.13  1995/12/01  23:15:32  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Need to clear the earl VC-mac when a VC goes down.
 *
 * Revision 2.13.16.12  1995/12/01  16:01:17  bbenson
 * CSCdi44768:  TOPO change debug message should output on toggle
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Added new debug switch to the client for topo debugging.
 *
 * Revision 2.13.16.11  1995/11/07  01:34:21  bbenson
 * CSCdi43159:  Unresolved LE-ARPs never stop ARPingq
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Removed autodecrement of arp_retry_count.
 *
 * Revision 2.13.16.10  1995/09/27  23:26:53  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * - Further buff and polish of setting ACTIVE SAR state.
 * - Moving Topology change, SNMP, proxy, and flush code into Synergy.
 *
 * Revision 2.13.16.9  1995/09/25  19:23:13  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Removed an extra call to reg_invoke_lec_check_sar_inputQ.
 *
 * Revision 2.13.16.8  1995/09/25  15:16:29  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Restructure part I.  Remove any reference to vlanid in registry calls.
 *
 * Revision 2.13.16.7  1995/09/22  22:34:32  sakumar
 * lec_find_vc() returns the Data Direct VCD when in RESOLVED state
 * and the call is READY (instead of the BUS VCD as it was previously
 * doing).
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.13.16.6  1995/09/21  21:50:28  sakumar
 * Added registry call specific to the Catalyst 5000 ATM LAN Emulation
 * Module to ask the Transmit SAR to send up packets after the MAC
 * address has been resolved in order to continue to drive the LEC
 * state machine.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.13.16.5  1995/09/20  21:42:44  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * - Call lec_clear_mac instead of bridge_set_mac2vc whenever clearing the
 *   le_arp entry.
 * - Set the earl's mac entry only when the le-arp state is DIRECT and the
 *   call ready state is CALL_READY.
 *
 * Revision 2.13.16.4  1995/09/15  17:11:46  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Check the ready_state of the call before setting the le_arp state to
 * LEC_MAC_DIRECT in lec_find_vc.
 *
 * Revision 2.13.16.3  1995/09/15  05:27:23  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Close some conditions where the MAC is not cleared out of the CAM after
 * the data direct VC is established.  Loads of belts and suspenders.
 *
 * Revision 2.13.16.2  1995/09/13  05:10:09  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Don't free paks that are put on the bridge_lookup_holdQ.
 *
 * Revision 2.13.16.1  1995/09/11  01:56:17  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.13  1995/08/14  19:14:26  bbenson
 * CSCdi35018:  AS should be turned off when 1577 and LANE are on the same
 * AIP.
 *
 * Revision 2.12  1995/08/11  04:48:08  bbenson
 * CSCdi38623:  LEC may come up in admin down state
 * - Call reg_add_if_statechange_complete within lec_init instead of
 *   lec_input.
 *
 * Revision 2.11  1995/08/06  21:59:47  bbenson
 * CSCdi36725:  LEC needs to obtain LECS address from the switch.
 * - Locally implement LECS address determination (not using cfgacc).
 * - Use interface_admin_down instead of interface_up_simple.
 *
 * Revision 2.10  1995/07/30  01:15:49  bbenson
 * CSCdi37903:  Not necessary to invalidate caches immediately for LANE.
 * - No more calls to reg_invoke_bump_cache_version with an immediate
 *   request.
 * - Also collapsed numerous message pointers in the lec_info_t into a
 *   single union
 *
 * Revision 2.9  1995/07/12  07:28:30  bbenson
 * CSCdi36903:  LEC needs to align show lane output with other components.
 * CSCdi36945:  LEC doesn't handle RELEASE COMPLETE in idle state.
 *
 * Revision 2.8  1995/06/30  22:02:15  bbenson
 * CSCdi36650:  LEC Join Failure error code could be more descriptive
 * - Removed coercion of mallocs in the bargain.
 *
 * Revision 2.7  1995/06/23  18:48:08  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.6  1995/06/19  06:45:55  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.5  1995/06/18  06:18:45  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.4  1995/06/16 06:06:47  bbenson
 * CSCdi35603:  router hangs after displaying NO BLOCK error message
 *  - Remove unnecessary call to reg_invoke_clear_routes.
 *
 * Revision 2.3  1995/06/09  13:10:54  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.2  1995/06/08 20:31:11  bbenson
 * CSCdi35434: LEC doesnt take the new ELAN name
 * - Bring in fresh copy out of lane_info every time we come up.
 * CSCdi35445: During state LECS_CONNECT abnormally received
 *             RELEASE_COMPLETE
 * - Handle RELEASE_COMPLETE in LECS_GET_ADDR state.
 *
 * Revision 2.1  1995/06/07  21:19:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "../ui/debug.h"
#include "parser_defs_lane.h"
#include "sys_registry.h"
#include "media_registry.h"

#include "../if/static_map.h"
#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/network.h"

#include "../iprouting/route.h"
#include "../ip/ip.h"

#include "../if/arp.h"
#include "../atm/atmsig_api.h"
#include "../atm/atmsig_public.h"
#include "../atm/ilmi_api.h"
#include "../src-68-alc/alc_lec.h"

#include "lane.h"
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */
#include "lane_private.h"
#include "lec_private.h"
#include "lane_signal.h"
#include "lec_fsm.h"
#include "lec_pdu.h"
#include "lane_debug.h"
#include "lane_ilmi.h"
#include "lane_signal.h"
#include "lane_registry.h"
/*
 * For bridged ELANs support
 */
#include "../srt/span.h"
#include "../srt/srt_registry.h"
#include "../smf/smf_public.h"
#include "../tbridge/tbridge_lane.h"

/* temp...will be registry functions */
#include "lecmib_util.h"
#include "lecmib_if.h"

/*
 * G l o b a l  S t o r a g e 
 */

boolean		lec_inputQ_enabled;	/* Is LEC ready for control paks? */
boolean		lec_booted;		/* lec_input is ready */

ilmiClient	lec_ilmi_clientid;	/* ID for ILMI requests */
ulong		lec_transaction_id;	/* Globally unique transid */




/*
 * L o c a l  S t o r a g e
 */

static boolean		lec_process_started;	/* LEC process started */
static mgd_timer	lec_master;		/* Master managed timer */

/*
 * LANE client control packets come in through this queue.
 */
#define		MAX_LEC_INPUT_QLEN	40

watched_queue	*lec_inputQ = NULL;
watched_queue	*lec_bridge_inputQ = NULL;
watched_queue	*lec_flush_inputQ = NULL;
watched_queue	*lec_ipc_inputQ = NULL;

queuetype 	lec_infoQ;	/* Queue of all LEC info blocks */

/*
 * Globals
 */
int	lec_count;		/* Counts the number of LECs */
boolean tr_lane_present = FALSE;


/*
 * lec_muex_config
 *
 * Determines if there are mutually exclusive configs on the
 * subinterface that conflict with LANE.  Returns TRUE and spits out
 * the appropriate error if anything conflicted with LANE.
 */
boolean lec_muex_config (idbtype *swidb)
{
    maplist_type *list_ptr;

    if (swidb->atm_arp) {
	printf("\n%%LANE client conflicts with atm arp-server command.");
	return(TRUE);
    }

    if (swidb->nsap) {
	printf("\n%%LANE client conflicts with atm nsap command.");
	return(TRUE);
    }

    if (swidb->mapgroup) {
        list_ptr = swidb->mapgroup->maplist;
	printf("\n%%LANE client conflicts with map-group %s.",
	    list_ptr ? list_ptr->name : "command");
	return(TRUE);
    }

    return(FALSE);
}


/*
 * lec_show_dying_arp
 *
 */
static void lec_show_dying_arp (char *string, lec_le_arp_t *le_arp)
{
    char nsapstr[ATM_MAX_NSAP_STRING];

    atm_printnsap(le_arp->nsap.addr, nsapstr);
    buginf("\nLEC %s: deleting LE-ARP, state %d, for %e (%s)",
           le_arp->lec_info->swidb->namestring, le_arp->state,
           le_arp->mac, string);
    buginf("\nLEC %s: nsap %s", le_arp->lec_info->swidb->namestring, nsapstr);
}


/*
 * lec_ilmi_callback
 *
 * This is the callback that's registered with the ILMI for
 * notification of address additions/deletions and SET responses from
 * the switch.
 */
static void lec_ilmi_callback (ilmiClientReq *notify)
{
    lec_info_t	*lec_info;

    switch (notify->notify_type) {
    case ilmiResponseNotify:
	/*
	 * Find the LEC that matches the ILMI transaction ID, and
	 * send in the event.  Otherwise, return.
	 */
        FOR_ALL_LEC(lec_info) {
	    if ((ulong)lec_info == notify->transid) {
		lec_info->data.notify = notify;
		/*
		 * We bitch about not being able to register the ATM
		 * address via ILMI, but currently the state machine
		 * doesn't do anything about it.  It still attempts to
		 * make the connect, since we don't know if the switch
		 * supports SVCs without ILMI.
		 */
		if ((notify->error == ilmiResponseReceived) &&
		    (notify->response == ilmiRespNoErr)) {
		    lec_fsm(lec_info, 
			    LEC_MAJOR_CONTROL, 
			    LEC_CTL_ILMI_SET_RSP_POS);
		} else {
		    lec_fsm(lec_info, 
			    LEC_MAJOR_CONTROL, 
			    LEC_CTL_ILMI_SET_RSP_NEG);
		}
		lec_info->data.notify = NULL;
		break;
	    }
	}
	break;
    default:
	errmsg(&msgsym(BADILMI, LANE), notify->notify_type);
	break;
    }
}


/*
 * lec_ilmi_reg_service
 *
 * Register for a particular notification event from the ILMI.
 */
static void lec_ilmi_reg_service (ilmiClientNotifyType notype)
{
    ilmiClientReq ilmi_req;

    ilmi_req.client_id   = lec_ilmi_clientid;
    ilmi_req.notify_type = notype;
    ilmi_req.notifyCB    = (void (*)(void *)) lec_ilmi_callback;
    if (ILMI_OK != ilmi_register_service(&ilmi_req)) {
        errmsg(&msgsym(NOILMI, LANE));
    }
}


/*
 * lec_register_ilmi
 *
 * Do all the initialization work with the ilmi as we first come up.
 */
static void lec_register_ilmi (void)
{

    lec_ilmi_clientid = ilmi_register_client("LEC");

    if (lec_ilmi_clientid) {
	lec_ilmi_reg_service(ilmiResponseNotify);
    } else {
	errmsg(&msgsym(NOILMI, LANE));
    }
}


/*
 * lec_stay_shutdown
 *
 * Checks the interface if the user "no shuts" an interface that has
 * the client configured on it.  Resets the MTU if necessary.
 */
static boolean lec_stay_shutdown (idbtype *idb)
{
    if (idb && lec_enabled(idb)) {
	if (lec_muex_config(idb)) {
	    return(TRUE);
	} else {
	   return(FALSE);
	}
    } else {
	return(FALSE);
    }
}


/*
 * lec_compute_macaddress
 *
 * Returns an appropriate mac address to be used by
 * a LEC, taking into account the mac-address-plan 
 * information.
 *
 * NOTE: For now, all we do is take the mac out of the bia
 * or the forced_mac address of the hwidb.  Eventually, 
 * the mac-plan-base will be used.
 */
static void lec_compute_macaddress (idbtype *swidb, uchar *mac)
{
    hwidbtype	*hwidb;

    hwidb = hwidb_or_null(swidb);
    if (! hwidb) return;
    increment_macaddress_by(hwidb->hardware, mac, 0);

}


/*
 * lec_make_dest
 *
 * Given an lec_info block, which gives us the LAN type, and a
 * real (not lane) MAC address, construct the LAN emulation 
 * destination field.  
 * Note that this function will probably have more to do when 
 * the LANE_TYPEs include token ring.
 * Damn right! - DAD 8/14/95
 *
 */
void lec_make_dest (lec_info_t *lec_info, 
		    uchar const *lan_addr,
		    lane_dest_t *lane_dest,
                    ushort tag_type)
{
    switch(tag_type) {
    case LANE_TAG_MAC_ADDRESS:
	lane_dest->tag = LANE_TAG_MAC_ADDRESS;
	ieee_copy(lan_addr, lane_dest->addr.mac);
	break;
    case LANE_TAG_ROUTE_DESC:
	lane_dest->tag = LANE_TAG_ROUTE_DESC;
	lane_dest->addr.rd.reserved = 0;
	bcopy(lan_addr, &lane_dest->addr.rd.val, STATIONLEN_ROUTEDESC);
	break;
    default:
	lane_dest->tag = LANE_TAG_NOT_PRESENT;
    }
}


/*
 * lec_create_le_arp_entry
 *
 * Creates the LEC MAC<->NSAP entry.  The entry is always created in
 * the flood state, and the NSAP will be filled in when the LE_ARP req
 * returns with the answer.
 */
lec_le_arp_t *lec_create_le_arp_entry (lec_info_t *lec_info,
				       uchar *dest_mac,
				       uchar *src_mac,
				       lec_mac_state_t state,
				       lec_arp_entry_type_t entry_type)
{
    int	bucket;
    lec_le_arp_t *le_arp;

    le_arp = malloc(sizeof(lec_le_arp_t));
    if (!le_arp) {
	return(NULL);
    }

    bucket			= lane_mac_hash(dest_mac);
    le_arp->next		= lec_info->le_arp_table[bucket];
    lec_info->le_arp_table[bucket] = le_arp;

    bcopy(dest_mac, le_arp->mac, STATIONLEN_IEEE48);
    bcopy(src_mac, le_arp->src_mac, STATIONLEN_IEEE48);

    le_arp->nsap		= LANE_EMPTY_NSAP;
    le_arp->lec_info		= lec_info;
    le_arp->state		= state;
    le_arp->entry_type		= entry_type;
    le_arp->arp_retry_count	= lec_info->C13_max_retry_count;
    le_arp->unknown_count	= lec_info->C10_max_unknown_count;
    TIMER_START(le_arp->unknown_time, lec_info->C11_max_unknown_time);

    mgd_timer_init_leaf(&le_arp->arp_age_timer,
			&lec_info->lec_arp_timers,
			LEC_TIMER_TYPE_ARP_AGE,
			le_arp,
			FALSE);

    mgd_timer_init_leaf(&le_arp->control_timer,
			&lec_info->lec_arp_timers,
			LEC_TIMER_TYPE_ARP_CONTROL,
			le_arp,
			FALSE);

    mgd_timer_init_leaf(&le_arp->flush_timer,
			&lec_info->lec_arp_timers,
			LEC_TIMER_TYPE_FLUSH,
			le_arp,
			FALSE);

    return(le_arp);
}


/*
 * lec_stop_arp_timers
 *
 * Stop all the timer running on "this" le_arp entry
 */
void lec_stop_arp_timers (lec_le_arp_t *le_arp)
{

    lec_stop_arp_age_timer(le_arp);
    lec_stop_arp_control_timer(le_arp);
    lec_stop_flush_timer(le_arp);
}


/*
 * lec_delete_le_arp_entry
 *
 * Deletes the LEC MAC<->NSAP entry, if found.  Note that the deleted
 * element's next pointer gets NULLed out to protect the "show" commands
 * from chasing space.
 */
boolean lec_delete_le_arp_entry (lec_info_t *lec_info, 
				 uchar *mac)
{
    unsigned bucket;
    lec_le_arp_t *le_arp, **prev;

    bucket = lane_mac_hash(mac);
    prev = &lec_info->le_arp_table[bucket];

    for (le_arp = lec_info->le_arp_table[bucket]; 
	 le_arp;
	 prev = &le_arp->next, le_arp = le_arp->next) {
    
	/* 
	 * If the mac is found, rewire the list and delete the element.
	 */
	if (ieee_equal(le_arp->mac, mac)) { 
            *prev = le_arp->next;
	    le_arp->next = NULL;	/* "show" command protection */
 	    lec_stop_arp_timers(le_arp);
	    reg_invoke_tbridge_lane_bte_update(lec_info,
					       mac,
					       0,
					       BTE_LANE_DELETE);
	    reg_invoke_lec_clear_mac(lec_info, le_arp->mac);
	    reg_invoke_cam_clear_entry(lec_info,
				       le_arp->cam_entry,
				       le_arp->mac);
            if (lec_le_arp_debug) 
                lec_show_dying_arp("entry", le_arp);
            /*
             * flush route caches only if we were actually using direct path
             */
            switch(le_arp->state) {
            case LEC_MAC_DIRECT:
            case LEC_MAC_REVERIFYING:
            case LEC_MAC_FLUSH:
                reg_invoke_bump_cache_version(lec_info->swidb->hwptr,
                                              lec_info->swidb, FALSE);
                break;
            default:
                break;
            }
	    free(le_arp);
	    return(TRUE);
	}
    }

    return(FALSE);
}


/*
 * lec_delete_le_arp_nsap
 *
 * Delete all le_arp entries which are associated with a particular
 * nsap.  This is called when the VCD is going down.
 */
void lec_delete_le_arp_nsap (lec_info_t *lec_info, lec_nsap_info_t *nsap_info)
{
    int	index;
    lec_le_arp_t *le_arp, **prev;
    int need_flush = 0;

    reg_invoke_bridge_delete_vc(lec_info, nsap_info->vc.vcd);

    for (index = 0; index < LANE_NETHASHLEN; index++) {

	if (lec_info->le_arp_table[index]) {

	    prev = &lec_info->le_arp_table[index];
	    le_arp = lec_info->le_arp_table[index];

	    while (le_arp) {
		if ((le_arp->entry_type != LEC_LE_ARP_PERMANENT) &&
		    (lane_addr_equal(&le_arp->nsap, &nsap_info->vc.nsap))){
		    *prev = le_arp->next;
                    if (lec_le_arp_debug) {
                        lec_show_dying_arp("nsap", le_arp);
		    }
                    /*
                     * request cache flush if we were really using direct path.
                     */
                    switch(le_arp->state) {
                    case LEC_MAC_DIRECT:
                    case LEC_MAC_REVERIFYING:
                    case LEC_MAC_FLUSH:
                        need_flush++;
                        break;
                    default:
                        break;
                    }
		    lec_stop_arp_timers(le_arp);
		    le_arp->next = NULL;	/* "show" command protection */
 		    reg_invoke_cam_clear_entry(lec_info,
					       le_arp->cam_entry,
 					       le_arp->mac);
		    reg_invoke_tbridge_lane_bte_update(lec_info,
                                                        &le_arp->mac[0],
                                                        0,
                                                        BTE_LANE_DELETE);
		    free(le_arp);
		    le_arp = *prev;
		} else {
		    if (le_arp->entry_type == LEC_LE_ARP_PERMANENT) {
			le_arp->state = LEC_MAC_RESOLVED;
		    }
		    prev = &le_arp->next;
		    le_arp = le_arp->next;
		}
	    }
	}
    }

    if (need_flush)
        reg_invoke_bump_cache_version(lec_info->swidb->hwptr,
                                      lec_info->swidb, FALSE);
}


/*
 * lec_add_lecs_addr
 *
 * Add an lecs address to the lecs_addrQ.
 */
void lec_add_lecs_addr (lec_info_t *lec_info, hwaddrtype *addr)
{
    lec_lecs_addr_t **lecs_addr, *new_lecs_addr;

    new_lecs_addr = malloc(sizeof(lec_lecs_addr_t));
    if (new_lecs_addr) {
 
        for (lecs_addr = &lec_info->lecs_addrQ;
             *lecs_addr;
             lecs_addr = &(*lecs_addr)->next);
 
        *lecs_addr = new_lecs_addr;
        new_lecs_addr->next = NULL;
	new_lecs_addr->atm_addr = *addr;
    }
}


/*
 * lec_free_lecs_queue
 *
 * Remove the chain of lecs addresses from the lec_info struct.
 */
void lec_free_lecs_addrQ (lec_info_t *lec_info)
{
    lec_lecs_addr_t *lecs_addr, *next;

    lecs_addr = lec_info->lecs_addrQ;
    while(lecs_addr) {
	next = lecs_addr->next;
	free(lecs_addr);
	lecs_addr = next;
    }
    lec_info->lecs_addrQ = NULL;
}


/*
 * lec_flush_le_arp_table
 *
 * Given a subinterface, clear out the TEMPORARY entries in the 
 * le_arp cache.
 */
void lec_flush_le_arp_table (lec_info_t *lec_info)
{
    int	index;
    lec_le_arp_t *le_arp, **prev;
    
    /*
     * Cancel all outstanding ARP age timers.
     */
    mgd_timer_stop(&lec_info->lec_arp_timers);

    for (index = 0; index < LANE_NETHASHLEN; index++) {

	if (lec_info->le_arp_table[index]) {

	    prev = &lec_info->le_arp_table[index];
	    le_arp = lec_info->le_arp_table[index];

	    while (le_arp) {
		if (le_arp->entry_type != LEC_LE_ARP_PERMANENT) {
		    *prev = le_arp->next;
		    le_arp->next = NULL;	/* "show" command protection */
		    reg_invoke_tbridge_lane_bte_update(lec_info,
						       &le_arp->mac[0],
						       0,
						       BTE_LANE_DELETE);
		    reg_invoke_lec_clear_mac(lec_info, le_arp->mac);
 		    reg_invoke_cam_clear_entry(lec_info, 
					       le_arp->cam_entry,
 					       le_arp->mac);
		    free(le_arp);
		    le_arp = *prev;
		} else {
		    prev = &le_arp->next;
		    le_arp = le_arp->next;
		}
	    }
	}
    }

}


/*
 * lec_create_nsap_entry
 *
 * Creates an lec_nsap_info entry in the lec nsap table, and loads the nsap
 * into the entry.  The assumption is made that all nsaps that are
 * loaded into the table for the LEC are DATA.
 */
lec_nsap_info_t *lec_create_nsap_entry (lec_info_t *lec_info, 
					hwaddrtype *nsap)
{
    int	bucket;
    lec_nsap_info_t *nsap_info;

    nsap_info = malloc(sizeof(lec_nsap_info_t));
    if (nsap_info == NULL) {
	return(NULL);
    }

    bucket 			= lane_addr_hash(nsap);
    nsap_info->next		= lec_info->nsap_table[bucket];
    lec_info->nsap_table[bucket] = nsap_info;
    bcopy(nsap, &nsap_info->vc.nsap, sizeof(hwaddrtype));
    nsap_info->vc.swidb		= lec_info->swidb;
    nsap_info->vc.flags		= ATM_VC_AAL5LANE;

    switch (lec_info->actual_lan_type) {
    case LANE_TYPE_802_3:
        nsap_info->vc.lane_vc_type	= LANE_LEC_DATA;
        nsap_info->vc.blli		= &lec_blli_ether_data;
	break;
    case LANE_TYPE_802_5:
        nsap_info->vc.lane_vc_type	= LANE_LEC_TR_DATA;
        nsap_info->vc.blli		= &lec_blli_token_ring_data;
	break;
    }

    nsap_info->ready_state	= CALL_NOT_READY;
    nsap_info->ready_retry_count = lec_info->C13_max_retry_count;
    nsap_info->vc.sdu_size 
	= lane_frame_to_sdu_size(lec_info->lane_info->max_data_frame);

    mgd_timer_init_leaf(&nsap_info->ready_timer,
			&lec_info->lec_ready_timers,
			LEC_TIMER_TYPE_READY,
			nsap_info,
			FALSE);

    return(nsap_info);
}


/*
 * lec_delete_nsap_entry
 *
 * Removes a lec_nsap_info struct from the lec nsap table.  Note that
 * the next pointer is set to NULL so that any "show" command will not
 * chase off into space.
 */
void lec_delete_nsap_entry (lec_info_t *lec_info,
			    lec_nsap_info_t *nsap_entry)
{
    int	bucket;
    hwaddrtype *nsap = &nsap_entry->vc.nsap;
    lec_nsap_info_t *nsap_info;
    lec_nsap_info_t **pred;

    bucket = lane_addr_hash(nsap);
    pred = &lec_info->nsap_table[bucket];

    for (nsap_info = lec_info->nsap_table[bucket];
	 nsap_info;
	 pred = &nsap_info->next, nsap_info = nsap_info->next) {

	/* If nsap is found, rewire the list and delete the element */
	if (nsap_info == nsap_entry) {
	    *pred = nsap_info->next;
	    nsap_info->next = NULL;	/* protection for "show" functions */
	    mgd_timer_stop(&nsap_info->ready_timer);
	    free(nsap_info);
	    return;
	}
    }
}


/*
 * lec_lookup_nsap
 *
 * Given a destination nsap, returns a pointer to the lec_nsap entry.
 */
lec_nsap_info_t *lec_lookup_nsap (lec_info_t *lec_info, 
				  hwaddrtype *nsap)
{
    int bucket;
    lec_nsap_info_t *nsap_info;

    bucket = lane_addr_hash(nsap);

    for (nsap_info = lec_info->nsap_table[bucket]; 
	 nsap_info; 
	 nsap_info = nsap_info->next) {

	if (lane_addr_equal(&nsap_info->vc.nsap, nsap)) {
	    return(nsap_info);
	}
    }

    return(NULL);
}


/*
 * lec_lookup_vcd
 *
 * Look up the nsap entry based on the vcd.
 */
lec_nsap_info_t *lec_lookup_vcd (lec_info_t *lec_info, 
				 vcd_t vcd)
{
    int bucket;
    lec_nsap_info_t *nsap_info;

    for (bucket = 0; bucket < LANE_NETHASHLEN; bucket++) {
	for (nsap_info = lec_info->nsap_table[bucket]; 
	     nsap_info; 
	     nsap_info = nsap_info->next) {
	    if (nsap_info->vc.vcd == vcd) {
		return(nsap_info);
	    }
	}
    }

    return(NULL);
}


/*
 * lec_lookup_callid
 *
 * Look up the nsap entry based on callid.
 */
lec_nsap_info_t *lec_lookup_callid (lec_info_t *lec_info,
				    void *call_id)
{
    int	bucket;
    lec_nsap_info_t *nsap_info;

    for (bucket = 0; bucket < LANE_NETHASHLEN; bucket++) {
	for (nsap_info = lec_info->nsap_table[bucket];
	     nsap_info;
	     nsap_info = nsap_info->next) {
	    if (nsap_info->vc.call_id == call_id) { 
		return(nsap_info);
	    }
	}
    }
    
    return(NULL);
}


/*
 * lec_lookup_le_arp
 *
 * Hash into the le_arp table for a given mac.  Return a pointer to the
 * entry, else NULL.
 */
lec_le_arp_t *lec_lookup_le_arp (lec_info_t *lec_info, 
				 uchar *mac)
{
    int bucket;
    lec_le_arp_t *le_arp;

    bucket = lane_mac_hash(mac);

    for (le_arp = lec_info->le_arp_table[bucket];
         le_arp;
         le_arp = le_arp->next) {

        if (ieee_equal(le_arp->mac, mac)) {
	    return(le_arp);
        }
    }

    return(NULL);
}


/*
 * lec_find_arp_ptr
 *
 * Hash into the le_arp table for a mac, for which we issued a flush.
 * If we find it in the cache, then it is OK to use the entry.
 */
boolean lec_find_arp_ptr (lec_info_t *lec_info, lec_le_arp_t *le_arp_entry)
{
    int index;
    lec_le_arp_t *le_arp;

    for (index = 0; index < LANE_NETHASHLEN; index++) {

	if (lec_info->le_arp_table[index]) {
	    le_arp = lec_info->le_arp_table[index];
	    while (le_arp) {
		if (le_arp == le_arp_entry) {
		    return(TRUE);
		}
		le_arp = le_arp->next;
	    }
	}
    }

    return(FALSE);
}


/*
 * lec_throttle_flood
 *
 * Check to see if we've sent more than C10 frames per C11 seconds.
 * Defaults to 1 per second.
 */
static boolean lec_throttle_flood (lec_info_t *lec_info, 
			    lec_le_arp_t *le_arp)
{
    /*
     * If we're still within the time window, continue to decrement
     * the unknown counter.  When it reaches zero, start to throttle
     * packets.  When the window passes, reset both the timer and 
     * the counter.
     */
    if (TIMER_RUNNING_AND_SLEEPING(le_arp->unknown_time)) {
	if (le_arp->unknown_count) {
	    le_arp->unknown_count--;
	    return(FALSE);
	} else {
	    return(TRUE);
	}
    } else {
	le_arp->unknown_count = lec_info->C10_max_unknown_count;
	TIMER_START(le_arp->unknown_time,
		    lec_info->C11_max_unknown_time);
        return(FALSE);
    }

    return(FALSE);
}


/*
 * lec_create_glare_entry
 *
 * Allocate a glared VC element on the glare list.  These VCs are to
 * be taken down because they lost in the rules of glare resolution.
 * If SETUPS between two clients cross in the network fabric, the
 * smaller NSAP is the one that is kept.  Yes, this is a case where 
 * the little guy wins.
 */
void lec_create_glare_entry (lec_info_t *lec_info, lec_nsap_info_t *glare_vc)
{
    lec_nsap_info_t *nsap_info;

    nsap_info = malloc(sizeof(lec_nsap_info_t));
    if (nsap_info) {
	nsap_info->vc.swidb	= glare_vc->vc.swidb;
	nsap_info->vc.vcd	= glare_vc->vc.vcd;
	nsap_info->vc.call_id	= glare_vc->vc.call_id;
	nsap_info->vc.svc       = glare_vc->vc.svc;
	nsap_info->vc.call_state= glare_vc->vc.call_state;
	nsap_info->next		= lec_info->glare_list;
	lec_info->glare_list	= nsap_info;
    }
}


/*
 * lec_lookup_glare
 *
 * Used by the LEC to find call_ids that may be on it's glare list.
 */
lec_nsap_info_t *lec_lookup_glare (lec_info_t *lec_info, void *call_id)
{
    lec_nsap_info_t *nsap_info;

    for (nsap_info = lec_info->glare_list;
	 nsap_info;
	 nsap_info = nsap_info->next) {

	if (nsap_info->vc.call_id == call_id) {
	    return(nsap_info);
	}

    }

    return(NULL);
}


/*
 * lec_delete_glare_entry
 *
 * Delete a glared VC out of the glare list.
 */
void lec_delete_glare_entry (lec_info_t *lec_info,
			     lec_nsap_info_t *nsap_info)
{
    lec_nsap_info_t *entry;
    lec_nsap_info_t **pred;

    pred = &lec_info->glare_list; 

    for (entry = lec_info->glare_list;
	 entry;
	 pred = &entry->next, entry = entry->next) {

	if (entry == nsap_info) {
	    *pred = entry->next;
	    free(entry);
	    return;
	}
    }
}


/*
 * lec_find_control
 *
 * Find one of the five control vc structs associated with a callid.
 */
lec_vc_t *lec_find_control (lec_info_t *lec_info, void *call_id)
{
    if (lec_info->configure.call_id == call_id) {
	return(&lec_info->configure);
    } else if (lec_info->ctl_direct.call_id == call_id) {
	return(&lec_info->ctl_direct);
    } else if (lec_info->ctl_distribute.call_id == call_id) {
	return(&lec_info->ctl_distribute);
    } else if (lec_info->mcast_send.call_id == call_id) {
	return(&lec_info->mcast_send);
    } else if (lec_info->mcast_forward.call_id == call_id) {
	return(&lec_info->mcast_forward);
    }
    return(NULL);
}


/*
 * lec_release_all_data
 *
 * Send a RELEASE for all of the data SVCs.  We're shutting down.
 * Notice that we don't care about receiving release complete for
 * data; we just remove the nsap_info and get the hell out.
 */
void lec_release_all_data (lec_info_t *lec_info)
{
    int	bucket;
    lec_nsap_info_t *nsap_info;
    lec_nsap_info_t **pred;

    /*
     * Can ALL of the active nsap ready timers by stopping the master
     * entry in the lec_info.
     */
    mgd_timer_stop(&lec_info->lec_ready_timers);

    for (bucket = 0; bucket < LANE_NETHASHLEN; bucket++) {
	nsap_info = lec_info->nsap_table[bucket];
	pred = &lec_info->nsap_table[bucket];
	while (nsap_info) {
	    if (nsap_info->vc.svc) {
		lec_send_release(&nsap_info->vc, ATM_NORMAL_UNSPECIFIED);
		reg_invoke_bridge_delete_vc(lec_info, nsap_info->vc.vcd);
		*pred = nsap_info->next;
		free(nsap_info);
		nsap_info = *pred;
	    } else {
		pred = &nsap_info->next;
		nsap_info = nsap_info->next;
	    }
	}
    }
}


/*
 * lec_release_all_vccs
 *
 * Utility function which leaves death in its wake.  Use with appropriate
 * discretion. 
 */
void lec_release_all_vccs (lec_info_t *lec_info)
{
    /*
     * Release the LECS SVC.
     */
    if ((lec_info->configure.svc) &&
	((lec_info->configure.call_state == CALL_STATE_UP) ||
	 (lec_info->configure.call_state == CALL_STATE_PROCEEDING))){
	lec_send_release(&lec_info->configure, ATM_NORMAL_UNSPECIFIED);
    }

    /*
     * Release the Control Direct SVC.  This should result in the
     * Control Distribute SVC getting taken down by the LES.
     */
    if ((lec_info->ctl_direct.svc) &&
	((lec_info->ctl_direct.call_state == CALL_STATE_UP) ||
	 (lec_info->ctl_direct.call_state == CALL_STATE_PROCEEDING))) {
	lec_send_release(&lec_info->ctl_direct, ATM_NORMAL_UNSPECIFIED);
    }


    /*
     * Release the Control Distribute SVC.
     */
    if ((lec_info->ctl_distribute.svc) &&
	((lec_info->ctl_distribute.call_state == CALL_STATE_UP) ||
	 (lec_info->ctl_distribute.call_state == CALL_STATE_PROCEEDING))) {
	lec_send_release(&lec_info->ctl_distribute, ATM_NORMAL_UNSPECIFIED);
    }

    /*
     * Release the Multicast Send SVC.  This should result in the
     * Multicast Forward SVC getting taken down by the BUS.
     */
    if ((lec_info->mcast_send.svc) &&
	((lec_info->mcast_send.call_state == CALL_STATE_UP) ||
	 (lec_info->mcast_send.call_state == CALL_STATE_PROCEEDING))){
	lec_send_release(&lec_info->mcast_send, ATM_NORMAL_UNSPECIFIED);
    }

    /*
     * Release the Multicast Forward SVC.
     */
    if ((lec_info->mcast_forward.svc) &&
	((lec_info->mcast_forward.call_state == CALL_STATE_UP) ||
	 (lec_info->mcast_forward.call_state == CALL_STATE_PROCEEDING))){
	lec_send_release(&lec_info->mcast_forward, ATM_NORMAL_UNSPECIFIED);
    }

    lec_release_all_data(lec_info);

}


/*
 * lec_lookup_connection
 *
 * Given a le_arp entry, check to see if a connection using the nsap
 * has been set up.  If there is a connection, move 
 * this entry from Resolved to flush. When the flush response comes
 * back, we will move to Data Direct
 * If there is no connection, start it up.
 */
void
lec_lookup_connection (lec_le_arp_t *le_arp)
{
    lec_nsap_info_t	*lec_nsap_info;
    lec_info_t		*lec_info;
    uchar		*mac;

    lec_info = le_arp->lec_info;
    mac	     = &le_arp->mac[0];

    lec_nsap_info = lec_lookup_nsap(lec_info, &le_arp->nsap);
    if (!lec_nsap_info) {
	lec_nsap_info = lec_create_nsap_entry(lec_info, &le_arp->nsap);
	if (lec_nsap_info) {
	    /*
	     * Start the Call!
	     */
	    lec_send_setup(lec_info, &lec_nsap_info->vc);
	}
    } else {
	if ((lec_nsap_info->vc.call_state == CALL_STATE_UP) &&
	     (lec_nsap_info->ready_state == CALL_READY)) {
	    if (lec_info->C4_proxy) {
		if (le_arp->state == LEC_MAC_RESOLVED) {
		    reg_invoke_tbridge_lane_bte_update(lec_info,
						       mac,
						       lec_nsap_info->vc.vcd,
						       BTE_LANE_FLUSH);
		    le_arp->state = LEC_MAC_FLUSH;
		    lec_send_flush_req(lec_info, le_arp);
		    lec_start_flush_timer(le_arp);
		    TIMER_STOP(le_arp->unknown_time);
		}
		return;
	    }
	    /* If bridging is not configured, just go to MAC_DIRECT */
	    le_arp->state = LEC_MAC_DIRECT;
	    TIMER_STOP(le_arp->unknown_time);
	}
    }
}


/*
 * lec_start_flush_requests
 *
 * Once an incoming call is accepted, we have to move the MAC states
 * to either flush or direct, depending on whether bridging is configured
 * Go thru' all the le_arp entries and identify the MACs for which the NSAP
 * address matches the current call.
 */
boolean
lec_start_flush_requests (lec_info_t *lec_info,
			  lec_nsap_info_t *nsap_info)
{
    int index;
    lec_le_arp_t *le_arp;
    uchar   *mac;
    int count;

    count = 0;
    for (index = 0; index < LANE_NETHASHLEN; index++) {

	if (lec_info->le_arp_table[index]) {
	    le_arp = lec_info->le_arp_table[index];
	    while (le_arp) {
		if (lane_addr_equal (&le_arp->nsap, &nsap_info->vc.nsap)) {
		    /*
		     * We have found a MAC which has been resolved to this
		     * nsap address. Move the mac state of the le_arp entry
		     * to flush/direct based on the configuration.
		     */
		    if (lec_info->C4_proxy) {
			if (le_arp->state == LEC_MAC_RESOLVED ) {
			    mac = le_arp->mac;
			    reg_invoke_tbridge_lane_bte_update(lec_info,
							       mac,
							       nsap_info->vc.vcd,
							       BTE_LANE_FLUSH);
			    le_arp->state = LEC_MAC_FLUSH;
			    lec_send_flush_req(lec_info, le_arp);
			    lec_start_flush_timer(le_arp);
			    count++;
			}
		    } else {
			le_arp->state = LEC_MAC_DIRECT;
		    }
		    /* 
		     * This means about 60 flush requests are to be sent before
		     * giving up the processor
		     */
		       
		    if (count >= 3*PAK_SWITCHCOUNT) {
			return (FALSE);
		    }
		}
		le_arp = le_arp->next;
	    }
	}
    }
    return (TRUE);
}


/*
 * lec_find_vc
 *
 * Given a MAC address, find the vcnum and return TRUE if found.
 * Initially look through the le_arp table to see if the mac is in the
 * midst of getting resolved via an le_arp.  If the lookup brings back
 * an ATM address, lookup the vcd in the nsap table.
 */
boolean lec_find_vc (lane_info_t *lane_info,
		     uchar *dest_mac,
		     uchar *src_mac,
		     vcd_t *vcnum,
                     boolean *cache_ok)
{
    lec_info_t		*lec_info;
    lec_le_arp_t	*le_arp;
    lec_nsap_info_t	*lec_nsap_info;

    
    /*
     * Check to see if the LANE Client is configured and active.
     */
    if ((!lane_info) ||
	(!lane_info->lec_info) ||
	(lane_info->lec_info->state != LEC_ST_ACTIVE)) {
	return(FALSE);
    }
    lec_info = lane_info->lec_info;

    /*
     * If this MAC is multicast, just return the multicast send vcc.
     */
    if (ISMULTICAST(dest_mac)) {
	*vcnum = lec_info->mcast_send.vcd;
        *cache_ok = TRUE;
	return(TRUE);
    }

    /*
     * For a unicast MAC, lookup the entry from the le-arp table.
     */
    *cache_ok = FALSE;
    le_arp = lec_lookup_le_arp(lec_info, dest_mac);
    if (le_arp) {
	switch (le_arp->state) {
	case LEC_MAC_FLOOD:
	case LEC_MAC_RESOLVED:

	    *vcnum = lec_info->mcast_send.vcd;
	    reg_invoke_tbridge_lane_bte_update(lec_info,
					       dest_mac,
					       *vcnum,
					       le_arp->state);

	    if (le_arp->state == LEC_MAC_RESOLVED ||
		le_arp->entry_type == LEC_LE_ARP_PERMANENT) {
		lec_lookup_connection(le_arp);
	    }
	    /*
	     * Continue to send out mcast vc, we're flooding this mac.
	     * Check to see if the destination should be throttled, if 
	     * so, drop the packet on the floor.
	     */
	    if (lec_throttle_flood(lec_info, le_arp)) {
		return(FALSE);
	    }
	    return(TRUE);

	case LEC_MAC_FLUSH:
	    /*
	     * Drop the packet.
	     */
	    reg_invoke_tbridge_lane_bte_update(lec_info,
					       dest_mac,
					       *vcnum,
					       le_arp->state);
	    return(FALSE);

	case LEC_MAC_DIRECT:
	case LEC_MAC_REVERIFYING:
	    lec_nsap_info = lec_lookup_nsap(lec_info, &le_arp->nsap);
	    if (lec_nsap_info) {
		/*
		 * If bridging is configured, then send a flush before
		 * switching to data direct path.
		 */
		if (lec_info->C4_proxy && 
		    reg_invoke_tbridge_lane_bte_flooding(lec_info, dest_mac)) {
		    reg_invoke_tbridge_lane_bte_update(lec_info,
						       dest_mac,
                                                       lec_nsap_info->vc.vcd,
						       BTE_LANE_FLUSH);
		    lec_send_flush_req(lec_info, le_arp);
		    lec_start_flush_timer(le_arp);
		    TIMER_STOP(le_arp->unknown_time);
		    return(FALSE);
		}
		*vcnum = lec_nsap_info->vc.vcd;
                *cache_ok = TRUE;
		return(TRUE);
	    } else {
		return (FALSE);
	    }
	}
    } else {
	/*
	 * Create an le-arp entry in the table and send off the first le-arp
	 * request.
	 * Note that this should be fixed in the future to hesitate
	 * for a few packets before firing off the first one to avoid
	 * setting up an unnecessary SVC.
	 * Also, DO NOT set the aging timer until the LE-ARP comes back.
	 */
	if (!onintstack()) {
	    le_arp = lec_create_le_arp_entry(lec_info, 
					     dest_mac,
					     src_mac,
					     LEC_MAC_FLOOD,
					     LEC_LE_ARP_LOCAL);
	    if (le_arp) {
		lec_start_arp_control_timer(le_arp);
		lec_send_arp_req(lec_info, dest_mac, src_mac);
	    }
	}
	*vcnum = lec_info->mcast_send.vcd;
	return(TRUE);
    }

    return(FALSE);
}


/*
 * lec_cpcs_lane
 *
 * Called to encapsulate data path LAN emulation packets.
 * Only thing for LANE to lay down in the encap is the LECID.
 */
void lec_cpcs_lane (paktype *pak)
{
    lec_info_t	*lec_info;
    
    lec_info = lec_info_from_swidb(pak->if_output);
    if (!lec_info) {
	return;
    }

    pak->datagramstart -= ATM_LANE_LECID_ENCAPBYTES;
    pak->datagramsize += ATM_LANE_LECID_ENCAPBYTES;

    PUTSHORT(pak->datagramstart, lec_info->lecid);

    pak->encsize += ATM_LANE_LECID_ENCAPBYTES;
}


/*
 * lec_load_config
 *
 * Just before the predicate vector for the LEC state machine is
 * initialized, we reinitialize all of the config overrides that might
 * redirect any of the control nsaps (lecs, les, or bus).
 */
void lec_load_config (lec_info_t *lec_info)
{
    lane_info_t *lane_info;

    lane_info = lec_info->lane_info;
    if (!lane_info) {
	return;
    }

    /*
     * Copy the elan name from lane_info.  Do this fresh every time,
     * since previous invocations of the client may have negotiate
     * another elan name.
     */
    lec_info->actual_name_size = lane_info->name_size;
    bcopy(lane_info->name,
	  lec_info->actual_elan_name,
	  MAX_ELAN_NAME_LENGTH);

    /*
     * Check for a configured LEC nsap on this subinterface.
     */
    switch (lane_ilmi_compute_atmsap(lane_info->swidb,
				     LANE_ILMI_LE_CLIENT,
				     &lec_info->masked_addr,
				     &lec_info->nsap)) {
    case LANE_ILMI_COMPUTE_SUCCESS:
	break;
    case LANE_ILMI_COMPUTE_BAD_INPUTS:
    case LANE_ILMI_COMPUTE_NO_PREFIX:
	lec_info->nsap = LANE_NO_NSAP;
	break;
    }

    /*
     * Check for a configured LECS nsap on this subinterface.  If
     * nothing is configured in the masked nsap, we zero out the
     * lec_info copy.
     */
    if (lane_info->lecs_nsap.length) {
	switch (lane_ilmi_compute_atmsap(lane_info->swidb, 
					 LANE_ILMI_CONFIG_SERVER,
					 &lane_info->lecs_nsap,
					 &lec_info->configure.nsap)) {
	case LANE_ILMI_COMPUTE_SUCCESS:
	    lec_info->configure.svc = TRUE;
	    break;
	case LANE_ILMI_COMPUTE_BAD_INPUTS:
	case LANE_ILMI_COMPUTE_NO_PREFIX:
	    lec_info->configure.nsap = LANE_NO_NSAP;
	    break;
	}
    } else {
	lec_info->configure.nsap = LANE_NO_NSAP;
    }

    /*
     * Check for a configured LES nsap on this subinterface.  If
     * nothing is configured in the masked nsap, we zero out the
     * lec_info copy.
     */
    if (lane_info->lsv_nsap.length) {
	switch(lane_ilmi_compute_atmsap(lane_info->swidb, 
					LANE_ILMI_LE_SERVER,
					&lane_info->lsv_nsap,
					&lec_info->ctl_direct.nsap)) {
	case LANE_ILMI_COMPUTE_SUCCESS:
	    lec_info->ctl_direct.svc = TRUE;
	    break;
	case LANE_ILMI_COMPUTE_BAD_INPUTS:
	case LANE_ILMI_COMPUTE_NO_PREFIX:
	    lec_info->ctl_direct.nsap = LANE_NO_NSAP;
	    break;
	}
    } else {
	lec_info->ctl_direct.nsap = LANE_NO_NSAP;
    }

    /*
     * Check for a configured BUS nsap on this subinterface.  If
     * nothing is configured in the masked nsap, we zero out the
     * lec_info copy.
     */
    if (lane_info->bus_nsap.length) {
	switch(lane_ilmi_compute_atmsap(lane_info->swidb, 
					LANE_ILMI_BUS,
					&lane_info->bus_nsap,
					&lec_info->mcast_send.nsap)) {
	case LANE_ILMI_COMPUTE_SUCCESS:
	    lec_info->mcast_send.svc = TRUE;
	    break;
	case LANE_ILMI_COMPUTE_BAD_INPUTS:
	case LANE_ILMI_COMPUTE_NO_PREFIX:
	    lec_info->mcast_send.nsap = LANE_NO_NSAP;
	    break;
	}
    } else {
	lec_info->mcast_send.nsap = LANE_NO_NSAP;
    }

    lec_info->ctl_distribute.nsap = LANE_NO_NSAP;
    lec_info->mcast_forward.nsap = LANE_NO_NSAP;
}


/*
 * lec_config_change
 *
 * Called by the les/bus or lecs whenever any configuration of theirs
 * changes which may have an affect on the LEC.  
 */
void lec_config_change (void *voidp)
{
    lane_info_t *lane_info = (lane_info_t*) voidp;
    lec_info_t *lec_info;

    if (!lane_info) {
	return;
    }

    lec_info = lane_info->lec_info;
    if (lec_info && lec_enabled(lec_info->swidb)) {
	/*
	 * Make sure to set C4_proxy field, if bridging is enabeld
	 */
	if (reg_invoke_lec_bridging_enabled(lec_info->swidb)) {
	    lec_info->C4_proxy = TRUE;
	} else {
	    lec_info->C4_proxy = FALSE;
	}

	lec_fsm(lec_info, LEC_MAJOR_LOCAL, LEC_LOCAL_CONFIG_CHANGE);
    }
}


/*
** we are restarting, so reset the idle timeouts 
** back to the beginning originals.
*/
static void reset_lec_idle_timer (lec_info_t *lec_info)
{
    if (!lec_info) return;
    if (lec_info->config_idle_time)
	lec_info->idle_time = lec_info->config_idle_time;
    else
	lec_info->idle_time = LEC_IDLE_TIMEOUT;
}


/*
 * lec_addr_recycle.  call this when either a mac
 * address have changed or a prefix have changed.
 */
void lec_addr_recycle (hwidbtype *hwidb,
    lane_addr_recycle_reason reason)
{
    idbtype	*swidb;
    lec_info_t  *lec_info;

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {

	/* if no lec configured here, move to next one */
	if (!lec_enabled(swidb)) continue;
	lec_info = lec_info_from_swidb(swidb);
	if (!lec_info) continue;

	switch (reason) {
	case MAC_ADDRESS_CHANGE_EVENT:
	    LEC_STATE_DEBUG("\nLEC %s: mac address changed, recycling",
		swidb->namestring);
	    lec_compute_macaddress(swidb, lec_info->normalMac);
	    reset_lec_idle_timer(lec_info);
	    lec_config_change(lec_info->lane_info);
	    break;
	case PREFIX_ADD_EVENT:
	case PREFIX_DELETE_EVENT:
	    LEC_STATE_DEBUG("\nLEC %s: atm address changed, recycling",
		swidb->namestring);
	    reset_lec_idle_timer(lec_info);
	    lec_fsm(lec_info, LEC_MAJOR_LOCAL, LEC_LOCAL_PREFIX_SET);
	    break;
	default:
	    break;
	}
    }
}


/*
 * lec_start_process
 *
 * Start the LEC process.  It should only be started if there is 
 * a client enabled on any interface on the system.  This function 
 * should be called from the lec_command function when an interface 
 * is detected with an lec enabled.
 */
void lec_start_process (void)
{
    int pid;

    if (!lec_process_started) {
	pid = process_create(lec_input, "LANE Client", 
			     LARGE_STACK, PRIO_NORMAL);
	if ( pid > 0 ) {
	    lec_process_started = TRUE;
	}
    }
}


/*
 * lec_swif_erase
 *
 * Called by lane_swif_erase to cleanup any LEC that might be hanging
 * around on a subinterface.  Note that all of the timers and the
 * state machine have been shutdown by the callback lec_interface_updown
 * that has been reg_invoked earlier.
 */
void lec_swif_erase (lane_info_t *lane_info)
{
    lec_info_t *lec_info;

    lec_info = lane_info->lec_info;
    if (!lec_info) {
	return;
    }

    unqueue(&lec_infoQ, lec_info);

    lec_info->next = NULL;	/* Protect any show commands */
    free(lec_info);
    lane_info->lec_info = NULL;
    lane_info->lec_enabled = FALSE;
    
    lane_info_destroy(lane_info);
}


/*
 * lec_interface_updown
 *
 * A registered function that is called whenever either the hwidb or
 * swidb changes state.  At the time that this happens, we check the
 * new state of the interface and, if an LEC is configured on the
 * interface, either bring it up or down.
 */
void lec_interface_updown (idbtype *swidb, hwidbtype *hwidb)
{

    lec_info_t	*lec_info;

    if (hwidb) {
	FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	    if (lec_enabled(swidb)) {
		lec_info = lec_info_from_swidb(swidb);
		if (interface_admin_down(swidb)) {
		    lec_info->admin_status = LANE_INACTIVE;
		    lec_fsm(lec_info, LEC_MAJOR_LOCAL, LEC_LOCAL_DEACTIVATE);
		} else {
		    lane_set_maxmtu(swidb, swidb->sub_mtu);
		    lec_info->admin_status = LANE_ACTIVE;
		    lec_fsm(lec_info, LEC_MAJOR_LOCAL, LEC_LOCAL_ACTIVATE);
		}
	    }
	}
	return;
    }

    if (swidb && lec_enabled(swidb)) {
	lec_info = lec_info_from_swidb(swidb);
	if (interface_admin_down(swidb)) {
	    lec_info->admin_status = LANE_INACTIVE;
	    lec_fsm(lec_info, LEC_MAJOR_LOCAL, LEC_LOCAL_DEACTIVATE);
	} else {
	    lane_set_maxmtu(swidb, swidb->sub_mtu);
	    lec_info->admin_status = LANE_ACTIVE;
	    lec_fsm(lec_info, LEC_MAJOR_LOCAL, LEC_LOCAL_ACTIVATE);
	}
    }

}


/*
 * A registered function that is called by VTP to 
 * create a LEC on a sub-interface.
 */
boolean lane_configure_client (idbtype *swidb, boolean create_lec, 
			       int lane_type, uchar *elan_name, 
			       int name_size, ulong vlan_id, 
			       int vr_rn, int srb_bn) 
{  
    boolean		result;
    
    if (!swidb) {
	return FALSE;
    }

    if (create_lec)
	result = lec_create_client(swidb, LANE_CALLER_SNMP, lane_type,
				   elan_name, vr_rn, srb_bn, vlan_id);
    else
	result = lec_delete_client(swidb, LANE_CALLER_SNMP);

    return result;
}


/*
 * Routine that handles what an LEC should do
 * with HSRP behavior.  If "activate" is set,
 * the LEC should start listening on the specified 
 * group mac address.  If set to FALSE, it should 
 * stop listening on that group number.
 */
static void lec_hsrp_action (idbtype *idb, boolean activate, 
    uchar hsrp_group)
{
    lane_info_t *lane_info;
    lec_info_t *lec_info;
    char *ifname;
    uchar hsrpMacAddress [IEEEBYTES];

    if (! idb) return;
    ifname = idb->namestring;
    lane_info = idb->lane_info;
    if (! lane_info) return;
    lec_info = lane_info->lec_info;
    if (! lec_info) return;

    if (lec_state_debug) {
	buginf("\nLEC %s: became hsrp %s client for group %u",
	    ifname, 
	    activate ? "ACTIVE" : "standby",
	    hsrp_group);
    }

    /* start/stop listening on specified group number */
    lec_info->lecHsrpMacs [hsrp_group] = activate;

    /* now send an le-narp involving this mac address */
    bcopy(standby_groups, hsrpMacAddress, (IEEEBYTES-1));
    hsrpMacAddress [IEEEBYTES-1] = hsrp_group;
    lec_send_narp_req(lec_info, hsrpMacAddress);
}


/*
 * find_lec_by_name_on_interface
 *
 */
static idbtype* find_lec_by_name_on_interface (uchar* elan_name,
					uchar elan_name_length, 
					idbtype*  idb)
{ 
    lec_info_t* lec_info;

    FOR_ALL_LEC(lec_info) 
	if ((lec_info->actual_name_size == elan_name_length) &&
	    (!memcmp(elan_name, lec_info->actual_elan_name,
		     elan_name_length)) && 
	    (lec_info->swidb) && (lec_info->swidb->hwptr) &&
	    (lec_info->swidb->hwptr->firstsw == idb))
	    return lec_info->swidb;
    return NULL;
}


/*
 * find_lec_by_vlanid_on_interface
 *
 */
static idbtype* find_lec_by_vlanid_on_interface (unsigned short vlan_id,
					  idbtype* idb) {
    lec_info_t* lec_info;

    FOR_ALL_LEC(lec_info) 
	if ((lec_info->vlanid == vlan_id) && (lec_info->swidb) &&
	    (lec_info->swidb->hwptr) &&
	    (lec_info->swidb->hwptr->firstsw == idb)) 
	    return lec_info->swidb;
    return NULL;
}


/*
 * lec_bridge_enqueue
 *
 * Function to enque packets to arp_triggerQ, reg_invoked from bridge code.
 */
static void lec_bridge_enqueue (uchar *addr, idbtype *swidb) 
{
    paktype *pak_lane;
    lec_info_t *lec_info;

    lec_info = swidb->lane_info->lec_info;

    if (lec_info->state == LEC_ST_ACTIVE) {

	/* Copy both the Destination MAC address & the source MAC address */
	pak_lane = getbuffer(IEEEBYTES*2);
	if (pak_lane) {
	    pak_lane->if_output = swidb;
	    bcopy(addr, pak_lane->datagramstart, IEEEBYTES*2);
	    process_enqueue_pak(lec_bridge_inputQ, pak_lane);
	}
    }
    return;
}


/*
 * lec_flush_enqueue
 *
 * Function to enque packets to flushQ, so that flush can be initiated
 * later on.
 */
void lec_flush_enqueue (lec_info_t *lec_info, lec_nsap_info_t *lec_nsap)
{
    paktype *pak_lane;

    if (lec_info->state == LEC_ST_ACTIVE) {

	/* Copy both the nsap info */
	pak_lane = getbuffer(sizeof(lec_nsap_info_t));
	if (pak_lane) {
	    pak_lane->if_output = lec_info->swidb;
	    bcopy(lec_nsap, pak_lane->datagramstart, sizeof(lec_nsap_info_t));
	    process_enqueue_pak(lec_flush_inputQ, pak_lane);
	}
    }
    return;
}


/*
 * lec_trigger_le_arp
 *
 * This routine dequeues messages from the bridg_inputQ and triggers
 * an LE_ARP, based on the MAC state
 */
static void lec_trigger_le_arp (void)
{
    lane_info_t	*lane_info;
    vcd_t       vcd;
    uchar       *dest_mac, *src_mac;
    paktype     *pak;
    int		count;
    boolean     cache_ok;

    count = PAK_SWITCHCOUNT;
    while (TRUE) {
	/*
	 * Process only a limited number of packets per call
	 */
	if (--count == 0) {
	    break;
	}
    
	pak = process_dequeue(lec_bridge_inputQ);
	if (!pak) {
	    break;
	}
	lane_info = pak->if_output->lane_info;
	if (!lane_info) {
	    datagram_done(pak);
	    continue;
	}
	dest_mac = pak->datagramstart;
	src_mac  = pak->datagramstart+IEEEBYTES;

	/*
	 * Don't bother to look at the return code from lec_find_vc.
	 */
	lec_find_vc(lane_info, dest_mac, src_mac, &vcd, &cache_ok);
	datagram_done(pak);
    }
}


/*
 * lec_tr_lane_present
 *
 * When TRLANE comes up, set this value so that parser can understand
 * the TRLANE specific stuff. Kludge due to modularization
 */
static void lec_tr_lane_present (boolean value)
{
    tr_lane_present = value;
}

/*
 * A registry function to handle the mtu change on a sub-interface.
 * This routine is invoked in set_max_interface_mtu()
 * when lec is created or the sub-interface mtu is changed.
 * We only want to take action in the case of the mtu change.
 */
static void
lec_change_mtu (hwidbtype *hwidb, idbtype *swidb, int delta)
{
    lec_info_t  *lec_info;
    int		max_data_frame;

    if (delta == 0) 
	return;

    if (swidb && lec_enabled(swidb)) {
	max_data_frame = lane_mtu_to_frame(swidb); 	

	lec_info = lec_info_from_swidb(swidb);
	lec_info->actual_max_frame_size =
	lec_info->lane_info->max_data_frame = max_data_frame;

	lec_info->mcast_send.sdu_size =
	lec_info->mcast_forward.sdu_size =
	    lane_frame_to_sdu_size(lec_info->lane_info->max_data_frame);
       
	lec_fsm(lec_info, LEC_MAJOR_LOCAL, LEC_LOCAL_CONFIG_CHANGE);
    }
    return;
}


/*
 * lec_init
 *
 * Called from lane_init when the subsystem is initialized.
 */
void lec_init (void)
{

    reg_add_media_change_mtu(lec_change_mtu, "lec_change_mtu");
    reg_add_lane_configure_client(lane_configure_client,
				  "lane_configure_client");
    reg_add_find_lec_by_name_on_interface(find_lec_by_name_on_interface, 
					  "find_lec_by_name_on_interface");
    reg_add_find_lec_by_vlanid_on_interface(find_lec_by_vlanid_on_interface, 
					    "find_lec_by_vlanid_on_interface");
    reg_add_lec_cpcs_lane(lec_cpcs_lane, "lec_cpcs_lane");
    reg_add_lec_config_change(lec_config_change, "lec_config_change");
    reg_add_lec_enqueue(lec_enqueue, "lec_enqueue");

    reg_add_lec_send_topo_change(lec_send_topo_change,
				 "lec_send_topo_change");
    reg_add_stay_shutdown(lec_stay_shutdown, "lec_stay_shutdown");

    reg_add_lec_hsrp_action(lec_hsrp_action, "lec_hsrp_action");
    reg_add_lec_trigger_le_arp(lec_trigger_le_arp, "lec_trigger_le_arp");
    reg_add_lec_bridge_enqueue(lec_bridge_enqueue, "lec_bridge_enqueue");
    reg_add_lec_init_idb(lec_init_idb, "lec_init_idb");
    reg_add_lec_tr_lane_present(lec_tr_lane_present, "lec_tr_lane_present");
    /* 
     * Common input queue for the LAN Emulation Client - 
     * handles the LE Control path for all emulated interfaces.
     */
    lec_inputQ = create_watched_queue("lec_inputQ", 0, 0);

    lec_flush_inputQ = create_watched_queue("lec_flush_inputQ", 0, 0);
    /*
     * Queue of all lec_info structs.
     */
    queue_init(&lec_infoQ, 0);

    /*
     * Queue of MAC addresses for which LE_ARP may need to be issued
     */

    lec_ipc_inputQ = create_watched_queue("lec_ipc_inputQ", 0, 0);
    lec_bridge_inputQ = create_watched_queue("lec_bridge_inputQ", 0, 0);
    
    /*
     * Root LEC timer.
     */
    mgd_timer_init_parent(&lec_master, NULL);

    lec_booted		= FALSE;	/* lec state machines ready */
    lec_process_started	= FALSE;	/* Is the LEC process up? */

    lec_inputQ_enabled	= FALSE;	/* disable inputQ */
    lec_transaction_id	= 0L;
    /*
     * When lec_init is called record if TRLANE is also configured
     * used by parser
     */
    if (reg_invoke_tr_lane_lec_srb_running())
	tr_lane_present = TRUE;
}


/*
 * lec_init_idb
 *
 * Init the swidb with an lec_info block.  If the lane info struct
 * hasn't been allocated yet, go do it too.
 */
lec_info_t *lec_init_idb (idbtype *swidb)
{
    lane_info_t	*lane_info;
    lec_info_t	*lec_info;
    hwidbtype	*hwidb;

    hwidb = hwidb_or_null(swidb);
    if (!hwidb) {
	return(NULL);
    }

    /*
     * Allocate a lane_info struct if it hasn't been allocated yet
     * for the swidb.
     */
    if (!swidb->lane_info) {
	lane_info = lane_init_idb(swidb);
	if (!lane_info) {
	    return(NULL);
	}
    } else {
	lane_info = swidb->lane_info;
    }

    /*
     * If there is already an lec_info allocated, just return it.
     * Otherwise, allocate one and drop through to initialize it.
     */
    if (!lane_info->lec_info) {
	lec_info = malloc(sizeof(lec_info_t));
	if (!lec_info) {
	    return(NULL);
	}
    } else {
	return(lane_info->lec_info);
    }

    mgd_timer_init_leaf(&lec_info->lec_timer,
			&lec_master,
			LEC_TIMER_TYPE_IDLE,
			lec_info,
			FALSE);

    mgd_timer_init_parent(&lec_info->lec_ready_timers, &lec_master);
    mgd_timer_init_parent(&lec_info->lec_arp_timers, &lec_master);

    queue_init(&lec_info->bridge_lookup_holdQ, 0);

    lane_info->lec_info	= lec_info;
    lec_info->lane_info	= lane_info;
    lec_info->swidb	= swidb;

    /*
     * Check if bridging enabled on the subinterface.
     */
    if (reg_invoke_lec_bridging_enabled(swidb)) {
	lec_info->C4_proxy = TRUE;
    } else {
	lec_info->C4_proxy = FALSE;	
    }

    /* set the "standard" mac address for this lec */
    lec_compute_macaddress(lec_info->swidb, 
	lec_info->normalMac);

    lec_info->lecid			= C14_DEFAULT;
    lec_info->vlanid			= 0;
    lec_info->transaction_id		= 0L;
    lec_info->state			= LEC_ST_IDLE;
    lec_info->predicate_vector		= 0L;
    lec_info->admin_status		= LANE_INACTIVE;
    lec_info->oper_status		= LANE_INACTIVE;

    lec_info->lecs_addrQ		= NULL;

    lec_info->configure.swidb		= swidb;
    lec_info->configure.svc		= FALSE;
    lec_info->configure.lane_vc_type	= LANE_LEC_CONFIGURE;
    lec_info->configure.call_state	= CALL_STATE_DOWN;
    lec_info->configure.flags		= (VC_FLAG_NOIDLETIME | 
					   VC_FLAG_NOBUMP_CACHE |
					   ATM_VC_AAL5LANE);
    lec_info->configure.nsap		= LANE_NO_NSAP;
    lec_info->configure.blli		= &lec_blli_control;
    lec_info->configure.sdu_size	= LANE_SDU_SIZE_1516;

    lec_info->ctl_direct.swidb		= swidb;
    lec_info->ctl_direct.svc		= FALSE;
    lec_info->ctl_direct.lane_vc_type	= LANE_LEC_DIRECT;
    lec_info->ctl_direct.call_state	= CALL_STATE_DOWN;
    lec_info->ctl_direct.flags		= (VC_FLAG_NOIDLETIME | 
					   VC_FLAG_NOBUMP_CACHE |
					   ATM_VC_AAL5LANE);
    lec_info->ctl_direct.nsap		= LANE_NO_NSAP;
    lec_info->ctl_direct.blli		= &lec_blli_control;
    lec_info->ctl_direct.sdu_size	= LANE_SDU_SIZE_1516;

    lec_info->ctl_distribute.swidb	= swidb;
    lec_info->ctl_distribute.svc 	= FALSE;
    lec_info->ctl_distribute.lane_vc_type = LANE_LEC_DISTRIBUTE;
    lec_info->ctl_distribute.call_state = CALL_STATE_DOWN;
    lec_info->ctl_distribute.flags	= (VC_FLAG_NOIDLETIME |
					   VC_FLAG_NOBUMP_CACHE |
					   ATM_VC_AAL5LANE);
    lec_info->ctl_distribute.nsap	= LANE_NO_NSAP;
    lec_info->ctl_distribute.blli	= &lec_blli_control;
    lec_info->ctl_distribute.sdu_size	= LANE_SDU_SIZE_1516;

    lec_info->mcast_send.swidb		= swidb;
    lec_info->mcast_send.svc		= FALSE;
    lec_info->mcast_send.lane_vc_type	= LANE_LEC_SEND;
    lec_info->mcast_send.call_state	= CALL_STATE_DOWN;
    lec_info->mcast_send.flags		= (VC_FLAG_NOIDLETIME |
					   VC_FLAG_NOBUMP_CACHE |
					   ATM_VC_AAL5LANE);
    lec_info->mcast_send.nsap		= LANE_NO_NSAP;
    lec_info->mcast_send.blli		= &lec_blli_ether_mcast;
    lec_info->mcast_send.sdu_size	= LANE_SDU_SIZE_1516;
    lec_info->mcast_forward.swidb	= swidb;
    lec_info->mcast_forward.svc		= FALSE;
    lec_info->mcast_forward.lane_vc_type = LANE_LEC_FORWARD;
    lec_info->mcast_forward.call_state	= CALL_STATE_DOWN;
    lec_info->mcast_forward.flags	= (VC_FLAG_NOIDLETIME |
					   VC_FLAG_NOBUMP_CACHE |
					   ATM_VC_AAL5LANE);
    lec_info->mcast_forward.nsap	= LANE_NO_NSAP;
    lec_info->mcast_forward.blli	= &lec_blli_ether_mcast;
    lec_info->mcast_forward.sdu_size	= LANE_SDU_SIZE_1516;

    lec_info->glare_list		= NULL;

    lec_info->C7_control_timeout	= C7_MINIMUM;
    lec_info->C10_max_unknown_count	= C10_MAXIMUM;
    lec_info->C11_max_unknown_time	= C11_DEFAULT;
    lec_info->C12_vcc_timeout		= C12_DEFAULT;
    lec_info->C13_max_retry_count	= C13_DEFAULT;
    lec_info->C17_aging_time		= C17_DEFAULT;
    lec_info->C18_forward_delay_time	= C18_DEFAULT;
    lec_info->C19_topology_change       = FALSE;
    lec_info->C20_arp_response_time	= C20_DEFAULT;
    lec_info->C21_flush_time		= C21_DEFAULT;
    lec_info->C22_path_switching_delay  = C22_DEFAULT;
    lec_info->C25_mcast_ave_rate        = C25_DEFAULT;
    lec_info->C26_mcast_peak_rate       = C26_DEFAULT;

    lec_info->C28_connect_comp_time	= C28_DEFAULT;

    lec_info->idle_time			= LEC_IDLE_TIMEOUT;
    lec_info->config_idle_time		= 0;
    lec_info->join_attempt_cnt		= 0;
    lec_info->done_first_idle_timeout	= FALSE; 

    lec_info->last_fail_reason		= LEC_FAIL_INIT;
    lec_init_info_counters(lec_info);

    p_enqueue(&lec_infoQ, lec_info);

    /*
     * Set up the ARP stuff in the swidb
     */
    swidb->arp_arpa 	= swidb->arp_arpa_def = TRUE;
    swidb->arp_probe	= swidb->arp_probe_def = FALSE;
    swidb->arp_snap	= swidb->arp_snap_def = FALSE;
    swidb->arp_smds	= FALSE;
    swidb->arp_timeout	= ARP_MAXAGE;
    
    /*
     * Set all of the enctypes for the subinterface.
     */
    swidb->ip_enctype	= ET_ARPA;
    reg_invoke_media_enctype_setup(swidb, ET_ARPA);

    /*
     * To support transparent bridging,  update
     * software mac filter with our own MAC address. 
     */
    
    reg_invoke_tbridge_smf_update(lec_info->normalMac, 
				  lec_info->swidb,
				  SMF_MACTYP_OWN_UNICAST | SMF_ACTION_RECEIVE);
    return(lec_info);
}


/*
 * lec_init_info_counters
 *
 * Initialize the LEC Interface Statistics.
 *
 * ;;; might have to put in the saved information (si) counters when I
 * understand the necessity of them...
 */
void lec_init_info_counters (lec_info_t *lec_info)
{ 
    lec_info->lecARPrequestsOut		= 0;
    lec_info->lecARPrequestsIn		= 0;
    lec_info->lecARPrepliesOut		= 0;
    lec_info->lecARPrepliesIn		= 0;
    lec_info->lecControlFramesOut       = 0;
    lec_info->lecControlFramesIn        = 0;
    lec_info->lecSVCFailures            = 0;
}


/*
 * lec_enqueue
 *
 * Enqueue a packet for later processing by the LANE Client.
 * If LANE is not running, as determined by the lec_inputQ_enabled
 * variable, then throw all packets away.
 *
 * Always return TRUE so that the registry return can be checked to
 * see if LANE is linked into the system.
 */
boolean lec_enqueue (paktype *pak)
{
    if (lec_inputQ_enabled) {
        process_enqueue_pak(lec_inputQ, pak);
    } else {
	pak->flags |= PAK_ABORT;
        netinput_enqueue(pak);
    }

    return(TRUE);
}


/*
 * lec_process_messages
 *
 * Handle incoming LAN emulation control packets for the LEC.  Snarf 
 * packets off of the lec_inputQ and stuff them through the lec
 * state machine.
 * abb--added support for LEC MIB error reporting
 *    --error code 'timeout(2)' is not supported at this time.
 */
static void lec_process_messages (void)
{
    int			count;
    paktype		*pak;
    lane_ctl_msg_u	*msg = NULL;
    lec_info_t		*lec_info;

    count = PAK_SWITCHCOUNT;
    while (TRUE) {
        /*
         * Process only a limited number of incoming packets per invocation.
         */
        if (--count == 0) {
            break;
	}

	pak = process_dequeue(lec_inputQ);
	if (!pak) {
	    break;
	}

	lec_info = lec_info_from_swidb(pak->if_input);
	if (lec_info) {
	    lec_info->lecControlFramesIn++;
	    lec_info->data.control_pak = pak;
	} else {
	    datagram_done(pak);
	    continue;
	}

	msg = (lane_ctl_msg_u *)pak->datagramstart;

	if (!lane_hdr_check(&msg->hdr)) {
	    errmsg(&msgsym(LEC_CONTROL_MSG, LANE),
		   pak->if_input->namestring);

	    /* bump the inErrors counter if SNMP enabled..
	     * should be able to tell diff between inUnknownProtos 
	     * and inErrors, but can't right now
	     */
	    if (lec_info->lec_snmp)
		lec_info->lec_snmp->in_errors++;

	    datagram_done(pak);
	    continue;
	}

	switch (msg->hdr.opcode) {
	case LANE_CONFIG_RSP:
	    LEC_PACKET_DEBUG (RECV, pak->if_input, pak, pak->vcd);
            if (msg->hdr.status == LANE_STATUS_SUCCESS) {
		if (lec_info->lec_is_srb){
		    if (msg->cfg.num_tlvs > 0){
			if (lec_state_debug)
			    buginf("\nTR LANE - tlv found on LE_CONFIGURE_RESPONSE");
			lec_info->elan_rn = GETSHORT(&msg->cfg.tlvs[5]);
		    }
		}  
  		lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_CONFIG_RSP_POS);
	    } else {
		errmsg(&msgsym(LEC_CFG, LANE),
		       pak->if_input->namestring,
		       lane_pr_status(msg->hdr.status),
		       msg->hdr.status);

		/* some SNMP bookkeeping */
		if (lec_info->lec_snmp)  {
		    lec_info->lec_snmp->last_fail_resp_code =
			msg->hdr.status;
		    lec_info->lec_snmp->last_fail_state =
			lec_info->state;
		}
		lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_CONFIG_RSP_NEG);
	    }
 	    break;
	case LANE_JOIN_RSP:
	    LEC_PACKET_DEBUG (RECV, pak->if_input, pak, pak->vcd);
	    if (msg->hdr.status == LANE_STATUS_SUCCESS) {
		lec_fsm(lec_info, LEC_MAJOR_CONTROL,
			LEC_CTL_JOIN_RSP_POS);
		/* complete reg. of this LEC with the LEC MIB */
		reg_invoke_lecmib_complete_lec_info(lec_info);
	    } else {
		errmsg(&msgsym(LEC_JOIN, LANE),
		       pak->if_input->namestring,
		       lane_pr_status(msg->hdr.status),
		       msg->hdr.status);
		/* some SNMP bookkeeping */
		if (lec_info->lec_snmp)  {
		    lec_info->lec_snmp->last_fail_resp_code =
			msg->hdr.status;
		    lec_info->lec_snmp->last_fail_state =
			lec_info->state;
		}
		lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_JOIN_RSP_NEG);
	    }
	    break;
	case LANE_ARP_REQ:
	    LEC_PACKET_DEBUG (RECV, pak->if_input, pak, pak->vcd);
	    lec_info->lecARPrequestsIn++;
	    lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_ARP_REQ);
	    break;
	case LANE_NARP_REQ:
	    LEC_PACKET_DEBUG (RECV, pak->if_input, pak, pak->vcd);
	    lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_NARP_REQ);
	    break;
	case LANE_ARP_RSP:
	    LEC_PACKET_DEBUG (RECV, pak->if_input, pak, pak->vcd);
	    lec_info->lecARPrepliesIn++;
	    lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_ARP_RSP);
	    break;
	case LANE_FLUSH_REQ:
	    LEC_PACKET_DEBUG (RECV, pak->if_input, pak, pak->vcd);
	    lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_FLUSH_REQ);
	    break;
	case LANE_FLUSH_RSP:
	    LEC_PACKET_DEBUG (RECV, pak->if_input, pak, pak->vcd);
	    lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_FLUSH_RSP);
	    break;
	case LANE_TOPO_CHANGE:
	    LEC_TOPO_DEBUG ("\nLEC %s: received LANE_TOPO_CHANGE on VCD %u",
			    lec_info->swidb->namestring,
			    pak->vcd);
	    lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_TOPO_CHANGE);
	    break;
	case LANE_READY_QUERY:
	    LEC_PACKET_DEBUG (RECV, pak->if_input, pak, pak->vcd);
	    lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_READY_QUERY);
	    break;
	case LANE_READY_IND:
	    LEC_PACKET_DEBUG (RECV, pak->if_input, pak, pak->vcd);
	    lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_READY_IND);
	    break;
 	case LANE_REG_RSP: 
            if (msg->hdr.status == LANE_STATUS_SUCCESS) {
	        lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_REG_RSP_POS);
            } else {
	        lec_fsm(lec_info, LEC_MAJOR_CONTROL, LEC_CTL_REG_RSP_NEG);
	    }
       break;

	default:
	    errmsg(&msgsym(LEC_CONTROL_MSG, LANE), 
		   pak->if_input->namestring);
	    break;
	}

	/*
	 * In synergy, if this pak is enqueued to the holdQ, the
	 * control_pak field is NULL.  The pak will be freed after the
	 * EARL comes back with the result of the MAC lookup for the
	 * LE_ARP.
	 */
	if (lec_info->data.control_pak) {
	    datagram_done(lec_info->data.control_pak);
	    lec_info->data.control_pak = NULL;
	}
	process_may_suspend();
    }
}


/*
 * lec_process_flushQ
 *
 * send flush requests for all the MAC addresses that have been resolved to
 * this NSAP address. Pace it by sending 60 flush messages at a time.
 * This number (60) came by trial & error and needs tuning only if the
 * MACs represented by the NSAP are just too many.
 */
void lec_process_flushQ (void)
{
    paktype		*pak;
    lec_info_t		*lec_info;
    lec_nsap_info_t 	*lec_nsap_info;

    /*
     * Each time process only 1 item from the flush Q
     * That will in turn cause more flush packets to be sent out.
     */
    pak = process_dequeue(lec_flush_inputQ);
    if (pak) {
	
	lec_info = lec_info_from_swidb(pak->if_output);
    
	if (lec_info) {
	    lec_nsap_info = (lec_nsap_info_t *)pak->datagramstart;
	    if (!lec_start_flush_requests(lec_info, lec_nsap_info)) {
		process_enqueue_pak(lec_flush_inputQ, pak);
	    } else {
		datagram_done(pak);
	    }
	} else {
	    datagram_done(pak);
	}
    }

    /*
     * If the lec process has taken too much time, then suspend now.
     */
    process_may_suspend();

}


/*
 * lec_sig_callback
 *
 * This callback is registered with the SigAPI both via the listen and
 * setup requests in order to receive incoming UNI signalling
 * primitives.  It does NOT run on the lec process, but
 * is invoked in the context of the signalling code.
 */
void lec_sig_callback (void *msg)
{
    lec_info_t *lec_info;
    sig_api_msg *sig_msg;

    sig_msg = (sig_api_msg *)msg;

    lec_info = lec_info_from_swidb(sig_msg->idb);
    if (!lec_info) {
	return;
    }

    lec_info->data.sig_msg = sig_msg;

    LEC_SIGNAL_DEBUG (RECV, sig_msg);

    switch(sig_msg->opcode) {
    case ATM_SETUP:
	lec_fsm(lec_info, LEC_MAJOR_SIGNALLING, LEC_SIG_SETUP);
	break;
    case ATM_CONNECT:
	lec_fsm(lec_info, LEC_MAJOR_SIGNALLING, LEC_SIG_CONNECT);
	break;
    case ATM_CONNECT_ACK:
	lec_fsm(lec_info, LEC_MAJOR_SIGNALLING, LEC_SIG_CONNECT_ACK);
	break;
    case ATM_RELEASE:
	lec_fsm(lec_info, LEC_MAJOR_SIGNALLING, LEC_SIG_RELEASE);
	break;
    case ATM_RELEASE_COMPLETE:
	lec_fsm(lec_info, LEC_MAJOR_SIGNALLING, LEC_SIG_RELEASE_COMP);
	break;
    case ATM_ADD_PARTY:
	lec_fsm(lec_info, LEC_MAJOR_SIGNALLING, LEC_SIG_ADD_PARTY);
	break;
    case ATM_DROP_PARTY:
	lec_fsm(lec_info, LEC_MAJOR_SIGNALLING, LEC_SIG_DROP_PARTY);
	break;
    case ATM_LISTEN_NSAP:
	if (sig_msg->u.listen.result == ATM_SIGAPI_OK) {
	    lec_fsm(lec_info, LEC_MAJOR_SIGNALLING, LEC_SIG_LISTEN_POS);
	} else {
	    lec_fsm(lec_info, LEC_MAJOR_SIGNALLING, LEC_SIG_LISTEN_NEG);
	}
	break;
    case ATM_CANCEL_NSAP:
	lec_fsm(lec_info, LEC_MAJOR_SIGNALLING, LEC_SIG_CANCEL);
	break;
    default:
	errmsg(&msgsym(LEC_SIGNAL, LANE), sig_msg->opcode);
	break;
    }

    /* Reset the sig_msg pointer. */
    lec_info->data.sig_msg = NULL;
}


/*
 * lec_process_timers
 *
 * Checks for expired timers on all LECs and sends the appropriate
 * event into the state machine.
 */
static void lec_process_timers (void)
{
    lec_info_t		*lec_info;
    lec_nsap_info_t	*nsap_info;
    lec_le_arp_t	*le_arp;

    while (mgd_timer_expired(&lec_master)) {
	mgd_timer *expired_timer;
	
	expired_timer = mgd_timer_first_expired(&lec_master);

	switch (mgd_timer_type(expired_timer)) {
	case LEC_TIMER_TYPE_IDLE:
	    lec_info = mgd_timer_context(expired_timer);
	    lec_fsm(lec_info, LEC_MAJOR_TIMER, LEC_TIMER_IDLE);
	    break;
	case LEC_TIMER_TYPE_CONFIG:
	    lec_info = mgd_timer_context(expired_timer);
	    lec_fsm(lec_info, LEC_MAJOR_TIMER, LEC_TIMER_CONFIG);
	    break;
	case LEC_TIMER_TYPE_JOIN:
	    lec_info = mgd_timer_context(expired_timer);
	    lec_fsm(lec_info, LEC_MAJOR_TIMER, LEC_TIMER_JOIN);
	    break;
	case LEC_TIMER_TYPE_BUS_ARP:
	    lec_info = mgd_timer_context(expired_timer);
	    lec_fsm(lec_info, LEC_MAJOR_TIMER, LEC_TIMER_BUS_ARP);
	    break;
	case LEC_TIMER_TYPE_READY:
	    nsap_info = mgd_timer_context(expired_timer);
	    lec_info = lec_info_from_swidb(nsap_info->vc.swidb);
	    lec_info->data.nsap_entry = nsap_info;
	    lec_fsm(lec_info, LEC_MAJOR_TIMER, LEC_TIMER_TYPE_READY);
	    lec_info->data.nsap_entry = NULL;
	    break;
	case LEC_TIMER_TYPE_ARP_AGE:
	    le_arp = mgd_timer_context(expired_timer);
	    lec_info = le_arp->lec_info;
	    lec_info->data.le_arp_entry = le_arp;
	    lec_fsm(lec_info, LEC_MAJOR_TIMER, LEC_TIMER_TYPE_ARP_AGE);
	    lec_info->data.le_arp_entry = NULL;
	    break;
	case LEC_TIMER_TYPE_ARP_CONTROL:
	    le_arp = mgd_timer_context(expired_timer);
	    lec_info = le_arp->lec_info;
	    lec_info->data.le_arp_entry = le_arp;
	    lec_fsm(lec_info, LEC_MAJOR_TIMER, LEC_TIMER_TYPE_ARP_CONTROL);
	    lec_info->data.le_arp_entry = NULL;
	    break;
	case LEC_TIMER_TYPE_FLUSH:
	    le_arp = mgd_timer_context(expired_timer);
	    lec_info = le_arp->lec_info;
	    lec_info->data.le_arp_entry = le_arp;
	    lec_fsm(lec_info, LEC_MAJOR_TIMER, LEC_TIMER_TYPE_FLUSH);
	    lec_info->data.le_arp_entry = NULL;
	    break;
        case LEC_TIMER_TYPE_REG_REQ:
            lec_info = mgd_timer_context(expired_timer);
            lec_fsm(lec_info, LEC_MAJOR_TIMER, LEC_TIMER_TYPE_REG_REQ);
            break;
	default:
	    mgd_timer_stop(expired_timer);
	    break;
	}

    }
}


/*
 * lec_line_statechange
 *
 * A wrapper for calling subif_line_statechange and also to initialize
 * CDP and add interesting addresses to SMF
 */
void lec_line_statechange (idbtype *swidb, ulong new_state)
{
    /* 
     * Set the line state according to the new_state 
     */
    subif_line_statechange(swidb, new_state, TRUE);

    if (new_state == SUBIF_STATE_UP) {
	swidb->cdp_enabled = TRUE;
	reg_invoke_media_interesting_addresses(swidb);
	/*
	 * Set the bridge media type based on what the LAN type is
	 */
	if ((swidb->tbridge_on_swidb) && 
	    (swidb->lane_info->lec_info->actual_lan_type == LANE_TYPE_802_3)) {
	    swidb->tbridge_media = TBR_LANE_ETHER;
	}

    } else {
	swidb->cdp_enabled = FALSE;
    }
}


/*
 * lec_boot_all
 *
 * Since the state machine has been quenched through the
 * lec_booted flag, no event has been sent through the state
 * machine to activate any LECs.  We do this here for only the LECs
 * that were configured at systeminit.
 */
static void lec_boot_all (void)
{
    lec_info_t *lec_info;

    FOR_ALL_LEC(lec_info) {
	lec_fsm(lec_info, LEC_MAJOR_LOCAL, LEC_LOCAL_ACTIVATE);
    }
    reg_invoke_lec_bridge_init();
}


/*
 * lec_input
 *
 * Handles all incoming LAN emulation control messages coming in from
 * the network, and timer events.
 */
process lec_input (void)
{

    process_wait_on_system_config();
    process_watch_queue(lec_inputQ, ENABLE, RECURRING);
    process_watch_queue(lec_ipc_inputQ, ENABLE, RECURRING);
    process_watch_queue(lec_bridge_inputQ, ENABLE, RECURRING);
    process_watch_queue(lec_flush_inputQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&lec_master, ENABLE);

    lec_register_ilmi();

    lec_booted = TRUE;
    lec_boot_all();


    while (TRUE) {

        process_wait_for_event();

	/*
	 * Check the message queue first.
	 */
	lec_process_messages();
	lec_process_flushQ();

	/*
	 * Check the Synergy SAR queue / bridge_inputQ as the case may be.
	 */
	reg_invoke_lec_process_bridge_messages();

	/*
	 * Have any timers expired?
	 */
	if (mgd_timer_expired(&lec_master)) {
	    lec_process_timers();
	}
    }
}

