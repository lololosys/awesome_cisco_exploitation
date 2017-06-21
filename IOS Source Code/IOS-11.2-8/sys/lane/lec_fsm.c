/* $Id: lec_fsm.c,v 3.13.6.25 1996/09/12 18:27:59 michellw Exp $
 * $Source: /release/112/cvs/Xsys/lane/lec_fsm.c,v $
 *------------------------------------------------------------------
 * lec_fsm.c -- LAN Emulation Client Finite State Machine
 *
 * September 1994, Brad Benson
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lec_fsm.c,v $
 * Revision 3.13.6.25  1996/09/12  18:27:59  michellw
 * CSCdi61979:  ATM: mismatch of known Vcd cause applications to block
 * Branch: California_branch
 * For lec_delete_nsap_entry(), passed in the pointer to the
 * lec_nsap_info_t to be deleted instead of the pointer to
 * the nsap address.
 *
 * Revision 3.13.6.24  1996/09/09  22:49:18  michellw
 * CSCdi68089:  LANE data direct down; Resolution: backout CSCdi61979
 * Branch: California_branch
 * Backout the fix committed for CSCdi61979
 *
 * Revision 3.13.6.23  1996/09/05  21:03:25  ppearce
 * CSCdi68216:  alc-a-m does not build due to undefined references to srbV
 * Branch: California_branch
 *   Access srbV from registry rather than directly
 *
 * Revision 3.13.6.22  1996/08/28  12:55:00  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.6.21  1996/08/26  15:09:37  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.13.6.20  1996/08/22  20:32:02  michellw
 * CSCdi61979:  ATM: mismatch of known Vcd cause applications to block
 * Branch: California_branch
 * For loopback VC, there will be two entries in lec_info->nasp_table
 * with the same nsap address. Use callid to uniquely locate the entry.
 *
 * Revision 3.13.6.19  1996/08/16  23:00:25  rbadri
 * CSCdi66287:  LANE: Clean up IPX/OSPF releated initializaion
 * Branch: California_branch
 * Move IPX & OSPF related registry invocations to lec_parser.c
 *
 * Revision 3.13.6.18  1996/08/12  06:26:32  bbenson
 * CSCdi64929:  Need to remove extraneous PVC code.
 * Branch: California_branch
 *
 * Revision 3.13.6.17  1996/07/30  14:42:57  michellw
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
 * Revision 3.13.6.16  1996/07/19  22:46:16  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.13.6.15  1996/07/17  18:26:40  ioakley
 * CSCdi62950:  LEC does not re-register its VRING rd after it is cycled.
 * Branch: California_branch
 *
 * Revision 3.13.6.14  1996/07/11  20:34:42  cyoung
 * Deregister RcvAddresses when client shutsdown/tearsdown.
 * CSCdi61990:  LANE CLIENT: memory leak
 * Branch: California_branch
 *
 * Revision 3.13.6.13  1996/07/09  05:59:57  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.13.6.12  1996/07/06  20:35:20  bbenson
 * CSCdi62141:  Flush timer mod (CSCdi50636) put event in active state
 * only.
 * Branch: California_branch
 *
 * Revision 3.13.6.11  1996/06/18  19:15:14  rbadri
 * CSCdi60067:  IPX rip TR->TR through bridged ELAN works intermittently.
 * Branch: California_branch
 * Clean up a little on the LEC side.
 *
 * Revision 3.13.6.10  1996/06/07  16:29:50  rbadri
 * CSCdi59709:  ATM / LANE subinterf are up even with LEC down when boot:
 * routing pb
 * Bring the line state down, when LEC is about to register its address.
 *
 * Revision 3.13.6.9  1996/06/05  02:01:31  rbadri
 * CSCdi59338:  TR->TR through bridged ELAN works intermittently
 * Branch: California_branch
 * Move setting of C4_proxy to a_activate_lec. Update bridge table
 * entries more tightly. When creating le-arp entries on the fly, due
 * to an incoming le-arp req, use the src_mac as the mac in le-arp entry.
 *
 * Revision 3.13.6.8  1996/05/31  16:50:37  michellw
 * CSCdi46369:  LEC remembers ELAN name even if it was never specified
 * Branch: California_branch
 * Not to save elan_name to lane_info->name when processing the
 * join_rsp message.
 *
 * Revision 3.13.6.7  1996/05/30  21:33:04  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.13.6.6  1996/05/28  19:02:04  michellw
 * CSCdi58712:  LEC starts next initial state when termination is not
 * completed
 * Branch: California_branch
 * The LEC entered TERMINATING state when receiving LOCAL_DEACTIVATE
 * event.  It then changed to IDLE state when all the VCs were down.
 * Also, re-indented codes in a_process_cfg_rsp().
 *
 * Revision 3.13.6.5  1996/05/17  23:26:41  rbadri
 * CSCdi56574:  LANE clients go down/up during 500 station multiping
 * Branch: California_branch
 * Flush requests are to be sent in the context of the client process
 * rather than signalling process. Throttle the number of requests to be
 * sent and be generous about giving up the CPU.
 *
 * Revision 3.13.6.4  1996/05/17  11:25:31  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.13.6.3  1996/05/09  14:32:51  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.13.6.2.2.3  1996/05/03  21:45:27  kjadams
 * CSCdi56624:  Incorrect BLLI for Token Ring LANE Data Direct VCC
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * confine blli change to lane.  Use LANE_BLLI_MAX_PID when looping
 * thru PIDs to see if there is a valid match on the blli.  Don't use
 * the ATM_SIG_MAX_BLLI unless you're really looking at the max number of
 * blli ie's in a setup message.
 *
 * Revision 3.13.6.2.2.2  1996/05/03  18:36:13  kjadams
 * CSCdi56624:  Incorrect BLLI for Token Ring LANE Data Direct VCC
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * Add switches to determine what vc type to pass into lane_sig_blli_match
 * instead of assuming EN.
 *
 * Revision 3.13.6.2.2.1  1996/04/27  07:06:42  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.13.6.2  1996/04/25  23:14:44  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.13.6.1  1996/04/05  18:51:27  michellw
 * CSCdi43013:  LEC state machine expects control dist. to be setup.
 * Branch: California_branch
 * Changed the LEC state machine so that the ctl distribute
 * vc is optional. Also the LEC will not accept the VC
 * after processing the join response.  When control or mcast
 * vc won't come up due to incorrect blli or wrong calling party,
 * LEC will tear down all the associated VCs and return to
 * the IDLE state.
 *
 * Revision 3.13.20.7  1996/04/26  00:46:06  michellw
 * Branch: LE_Cal_V112_0_2_branch
 * Support two new commands "lane client idle IDLE-TIME" and
 * "clear lane client join-cnt".  Print the LEC uptime and
 * lec join-attempt-count in the "show lan client" display.
 *
 * Revision 3.13.20.6  1996/04/17  23:14:51  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Convert TokenRing LANE related functions to registries
 * Move contents of lec_srb* srt/srb_lane.[ch], to facilitate
 * creation of new sub-system.
 *
 * Revision 3.13.20.5  1996/04/16  02:44:44  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.13.20.4  1996/04/11  05:34:02  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Correct botched lecs.c sync
 * - Modify indentation to appease the pedantic
 *
 * Revision 3.13.20.3  1996/04/09  02:16:49  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.13.20.2  1996/04/08  14:52:02  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.13.20.1  1996/04/08  01:57:06  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.13.12.4  1996/04/06  02:52:27  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * In LE-ARP requests, fill in the source MAC of the originator and not
 * the MAC of client.
 *
 * Revision 3.13.12.3  1996/04/03  02:39:35  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * Client state machine is event driven instead of packet driven.
 *
 * Revision 3.13.12.2  1996/03/25  02:20:02  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.13.12.1  1996/03/22  22:54:56  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.9.4.2  1996/03/16  19:54:55  bbenson
 * CSCdi51791:  Need fsm fix in LEC, LS2020 sets up mcast send before
 * forward
 * Branch: LE_Cal_V111_1_0_3_branch
 * When mcast send gets connected _after_ mcast forward, need to go active
 *
 * Revision 3.9.4.1  1996/02/27  21:10:30  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.9  1996/02/05  18:07:00  rbadri
 * CSCdi47734:  LEC responds to LE_ARP requests even in blocked state.
 * Add another registry call for LE_ARP responses. Respond to LE_ARP
 * requests only if LEC is in Forwarding state and the MAC is not learned
 * through the LEC.
 *
 * Revision 3.8  1996/01/17  23:17:19  cyoung
 * Fill in the correct config source
 * CSCdi46903:  lecConfigSource is incorrect
 *
 * Revision 3.7.4.1  1996/02/29  07:48:55  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.7.2.2  1996/01/23  23:54:22  cakyol
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
 * Revision 3.7.2.1  1996/01/11  01:46:31  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.7  1996/01/05  04:56:14  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.6.8.1  1995/12/29  01:06:49  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.6  1995/11/24  19:39:43  rbadri
 * CSCdi36121:  Line protocol is up, while LANE client is non-operational
 * subif_line_state variable will track the swidb's line state.
 * Use subif_line_statechange() to control the operational state of
 * the sub-interface. In LECS/LES use interface_admin_down instead of
 * interface up to determine the adminstrative nature of the sub-interface.
 *
 * Revision 3.5  1995/11/23  18:59:53  bbenson
 * CSCdi43592:  Topo changes should only touch LE-ARP cache on change
 * Only hit the timers on the LE-ARP cache when the incoming TC bit has
 * toggled state.
 *
 * Revision 3.4  1995/11/23  17:32:49  bbenson
 * CSCdi43159:  Unresolved LE-ARPs never stop ARPingq
 * Removed autodecrement of arp_retry_count.
 *
 * Revision 3.3  1995/11/20  17:47:16  rbadri
 * CSCdi43483:  Change in network topology is not picked up by ATM card
 * When the ARP response comes back, update the le_arp cache entry based
 * on the NSAP address in the reply and change the MAC state accordingly.
 *
 * Revision 3.2  1995/11/17  17:51:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.19  1995/11/08  21:12:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.18  1995/10/13  01:03:54  shj
 * CSCdi41571:  atm interface uses wrong ipx node address
 * Backout CSCdi38984, which caused wrong ipx address on non-LANE ATM
 * interfaces.  Instead, LANE now changes ipx address if ipx is already
 * configured when LANE Client configured.
 *
 * Revision 2.17.8.2.2.1  1996/02/03  07:07:09  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.17  1995/08/30  00:50:51  bbenson
 * CSCdi39461:  LEC gets into an infinite loop trying to get LES addr
 * - Add an action function to handle resending of the config request to
 *   the LECS on control timeouts.
 *
 * Revision 2.16.16.7.2.1  1996/01/19  21:56:49  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.16.16.11  1995/12/01  16:01:21  bbenson
 * CSCdi44768:  TOPO change debug message should output on toggle
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Added new debug switch to the client for topo debugging.
 *
 * Revision 2.16.16.10  1995/11/08  20:47:18  bbenson
 * CSCdi43483:  Change in network topology is not picked up by ATM card
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.16.16.9  1995/11/08  07:38:10  bbenson
 * CSCdi43592:  Topo changes should only touch LE-ARP cache on change
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Only hit the timers on the LE-ARP cache when the incoming TC bit has
 * toggled state.
 *
 * Revision 2.16.16.8  1995/11/07  01:34:25  bbenson
 * CSCdi43159:  Unresolved LE-ARPs never stop ARPingq
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Removed autodecrement of arp_retry_count.
 *
 * Revision 2.16.16.7  1995/10/26  02:39:30  bbenson
 * CSCdi42160:  ATM port in blocking state responds to LE-ARPs
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * - Don't respond to LE-ARP requests when the port state is BLOCKED.
 * - Roll in fix CSCdi39461 from the mainline to fix infinite loop of the
 *   config request.
 *
 * Revision 2.16.16.6  1995/10/16  23:45:50  sakumar
 * Added support for Topology Change and LES, BUS, LECS implemented
 * on the Catalyst 5000 ATM Module with RSAR->TSAR firmware
 * implementation of BUS.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.16.16.5  1995/09/27  23:26:57  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * - Further buff and polish of setting ACTIVE SAR state.
 * - Moving Topology change, SNMP, proxy, and flush code into Synergy.
 *
 * Revision 2.16.16.4  1995/09/25  15:16:33  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Restructure part I.  Remove any reference to vlanid in registry calls.
 *
 * Revision 2.16.16.3  1995/09/21  21:50:33  sakumar
 * Added registry call specific to the Catalyst 5000 ATM LAN Emulation
 * Module to ask the Transmit SAR to send up packets after the MAC
 * address has been resolved in order to continue to drive the LEC
 * state machine.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.16.16.2  1995/09/15  05:27:26  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Close some conditions where the MAC is not cleared out of the CAM after
 * the data direct VC is established.  Loads of belts and suspenders.
 *
 * Revision 2.16.16.1  1995/09/11  01:56:20  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.16  1995/08/14  19:14:32  bbenson
 * CSCdi35018:  AS should be turned off when 1577 and LANE are on the same
 * AIP.
 *
 * Revision 2.15  1995/08/11  00:04:45  bbenson
 * CSCdi38612:  LEC should check vblist type of the returned LECS address
 * - Check for OCTET_PRIM_TYPE and STATIONLEN_ATMNSAP in returned vblist.
 *
 * Revision 2.14  1995/08/09  18:59:16  bbenson
 * CSCdi36725:  LEC needs to obtain LECS address from the switch.
 * - Original fix doesn't completely shut down config vc on failure to
 *   connect to LECS.
 *
 * Revision 2.13  1995/08/06  21:59:50  bbenson
 * CSCdi36725:  LEC needs to obtain LECS address from the switch.
 * - Locally implement LECS address determination (not using cfgacc).
 * - Use interface_admin_down instead of interface_up_simple.
 *
 * Revision 2.12  1995/07/30  01:15:51  bbenson
 * CSCdi37903:  Not necessary to invalidate caches immediately for LANE.
 * - No more calls to reg_invoke_bump_cache_version with an immediate
 *   request.
 * - Also collapsed numerous message pointers in the lec_info_t into a
 *   single union
 *
 * Revision 2.11  1995/07/22  00:02:20  bbenson
 * CSCdi37390:  LANE Client not deregisterring
 * - Needed to call lec_send_ilmi_unreg_addr before cancelling the listen
 *   on the ATM address.
 *
 * Revision 2.10  1995/07/13  21:39:38  cakyol
 * CSCdi37017:  LECS should advertise its coming up and going down
 * fixed.
 *
 * CSCdi37071:  lecs should migrate to "malloc_named"
 * fixed.
 *
 * Revision 2.9  1995/07/12  07:28:32  bbenson
 * CSCdi36903:  LEC needs to align show lane output with other components.
 * CSCdi36945:  LEC doesn't handle RELEASE COMPLETE in idle state.
 *
 * Revision 2.8  1995/07/11  05:58:52  bbenson
 * CSCdi36837:  LEC doesnt recycle when LES/BUS/LECS address changes
 *
 * Revision 2.7  1995/07/11  05:27:28  bbenson
 * CSCdi36864:  No need to display UPDOWN message periodically.
 * CSCdi36904:  LEC should retransmit JOIN requests C13_max_retry_count
 * times.
 * CSCdi36837:  LEC doesnt recycle when LES/BUS/LECS address changes.
 *
 * Revision 2.6  1995/07/07  05:42:33  bbenson
 * CSCdi35012:  Server and Client should errmsg when operState goes down.
 * - Consolidated message to conform to the link and lineproto "UPDOWN"s.
 *   As Norm would say, slicker than...
 *
 * Revision 2.5  1995/07/01  00:40:23  shj
 * CSCdi36443:  show lane name <ELAN-NAME> doesnt display ELAN info
 *
 * Copy elan name from join response to lane_info if name not previously
 * defined.
 *
 * Revision 2.4  1995/06/30  22:02:17  bbenson
 * CSCdi36650:  LEC Join Failure error code could be more descriptive
 * - Removed coercion of mallocs in the bargain.
 *
 * Revision 2.3  1995/06/16  06:06:50  bbenson
 * CSCdi35603:  router hangs after displaying NO BLOCK error message
 *  - Remove unnecessary call to reg_invoke_clear_routes.
 *
 * Revision 2.2  1995/06/08  20:31:14  bbenson
 * CSCdi35434: LEC doesnt take the new ELAN name
 * - Bring in fresh copy out of lane_info every time we come up.
 * CSCdi35445: During state LECS_CONNECT abnormally received
 *             RELEASE_COMPLETE
 * - Handle RELEASE_COMPLETE in LECS_GET_ADDR state.
 *
 * Revision 2.1  1995/06/07  21:20:02  hampton
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
#include "logger.h"
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */
#include "ieee.h"
#include "sys_registry.h"

#include "../ui/debug.h"
#include "../util/fsm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../if/network.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"
#include "../srt/span.h"
#include "../atm/atmsig_api.h"
#include "../atm/ilmi_api.h"


#include "lane.h"
#include "lane_private.h"
#include "lec_private.h"
#include "lec_fsm.h"
#include "lec_pdu.h"
#include "lane_debug.h"
#include "lane_registry.h"
#include "lane_signal.h"

#include "lecmib_util.h"
#include "lecmib_if.h"

lec_fail_reason_t lec_fail_reason = LEC_FAIL_NOTAPPLY;

/*
 * Looks like this is the factory norm for casting action functions.
 * This makes the state tables look nice.
 */
#define A_ACTIVATE_LEC	 		FSM_ACTION_ROUTINE(a_activate_lec)
#define A_ALLOC_LECS_ADDR		FSM_ACTION_ROUTINE(a_alloc_lecs_addr)
#define A_POST_LISTEN 			FSM_ACTION_ROUTINE(a_post_listen)
#define A_POST_LISTEN_NO_ILMI		FSM_ACTION_ROUTINE(a_post_listen_no_ilmi)
#define A_PROCESS_ADD_PARTY		FSM_ACTION_ROUTINE(a_process_add_party)
#define A_PROCESS_ARP_AGE_TIMER		FSM_ACTION_ROUTINE(a_process_arp_age_timer)
#define A_PROCESS_ARP_CONTROL_TIMER 	FSM_ACTION_ROUTINE(a_process_arp_control_timer)
#define A_PROCESS_FLUSH_TIMER		FSM_ACTION_ROUTINE(a_process_flush_timer)
#define A_PROCESS_ARP_RSP 		FSM_ACTION_ROUTINE(a_process_arp_rsp)
#define A_PROCESS_NARP_REQ		FSM_ACTION_ROUTINE(a_process_narp_req)
#define A_PROCESS_FLUSH_RSP             FSM_ACTION_ROUTINE(a_process_flush_rsp)
#define A_PROCESS_BUS_CONNECT 		FSM_ACTION_ROUTINE(a_process_bus_connect)
#define A_PROCESS_CFG_RSP 		FSM_ACTION_ROUTINE(a_process_cfg_rsp)
#define A_PROCESS_CONNECT 		FSM_ACTION_ROUTINE(a_process_connect)
#define A_PROCESS_CONNECT_ACK		FSM_ACTION_ROUTINE(a_process_connect_ack)
#define A_PROCESS_CTL_DIST_SETUP	FSM_ACTION_ROUTINE(a_process_ctl_dist_setup)
#define A_PROCESS_CTL_DIST_SETUP_SEND_REG_REQ   \
                    FSM_ACTION_ROUTINE(a_process_ctl_dist_setup_send_reg_req)
#define A_PROCESS_FAILED_LISTEN 	FSM_ACTION_ROUTINE(a_process_failed_listen)
#define A_PROCESS_IDLE_REL_COMP 	FSM_ACTION_ROUTINE(a_process_idle_rel_comp)
#define A_PROCESS_JOIN_RSP_SEND_REQ   \
                    FSM_ACTION_ROUTINE(a_process_join_rsp_send_req)
#define A_PROCESS_READY_IND 		FSM_ACTION_ROUTINE(a_process_ready_ind)
#define A_PROCESS_READY_QUERY 		FSM_ACTION_ROUTINE(a_process_ready_query)
#define A_PROCESS_READY_TIMER 		FSM_ACTION_ROUTINE(a_process_ready_timer)
#define A_PROCESS_REG_RSP     FSM_ACTION_ROUTINE(a_process_reg_rsp)
#define A_PROCESS_RELEASE 		FSM_ACTION_ROUTINE(a_process_release)
#define A_PROCESS_REL_COMP 		FSM_ACTION_ROUTINE(a_process_rel_comp)
#define A_PROCESS_SETUP 		FSM_ACTION_ROUTINE(a_process_setup)
#define A_PROCESS_TERM_RELEASE 		FSM_ACTION_ROUTINE(a_process_term_release)
#define A_PROCESS_TERM_REL_COMP 	FSM_ACTION_ROUTINE(a_process_term_rel_comp)
#define A_PROCESS_TOPO_CHANGE	 	FSM_ACTION_ROUTINE(a_process_topo_change)
#define A_REGISTER_ADDR			FSM_ACTION_ROUTINE(a_register_addr)
#define A_RESEND_CFG_REQ 		FSM_ACTION_ROUTINE(a_resend_cfg_req)
#define A_RESEND_JOIN_REQ 		FSM_ACTION_ROUTINE(a_resend_join_req)
#define A_RESEND_REG_REQ      FSM_ACTION_ROUTINE(a_resend_reg_req)
#define A_RESEND_REG_REQ      FSM_ACTION_ROUTINE(a_resend_reg_req)

#define A_SEND_ARP_RSP 			FSM_ACTION_ROUTINE(a_send_arp_rsp)
#define A_SEND_BUS_ARP 			FSM_ACTION_ROUTINE(a_send_bus_arp)
#define A_SEND_BUS_CONNECT 		FSM_ACTION_ROUTINE(a_send_bus_connect)
#define A_SEND_BUS_SETUP 		FSM_ACTION_ROUTINE(a_send_bus_setup)
#define A_SEND_CFG_REQ 			FSM_ACTION_ROUTINE(a_send_cfg_req)
#define A_SEND_FLUSH_RSP	 	FSM_ACTION_ROUTINE(a_send_flush_rsp)
#define A_SEND_JOIN_REQ 		FSM_ACTION_ROUTINE(a_send_join_req)
#define A_SEND_LECS_SETUP		FSM_ACTION_ROUTINE(a_send_lecs_setup)
#define A_SEND_LES_SETUP 		FSM_ACTION_ROUTINE(a_send_les_setup)
#define A_SEND_NEXT_LECS_SETUP		FSM_ACTION_ROUTINE(a_send_next_lecs_setup)
#define A_SET_LEC_ACTIVE		FSM_ACTION_ROUTINE(a_set_lec_active)
#define A_SHUTDOWN_LEC			FSM_ACTION_ROUTINE(a_shutdown_lec)
#define A_TEARDOWN_LEC			FSM_ACTION_ROUTINE(a_teardown_lec)
#define A_SEND_REL_COMP			FSM_ACTION_ROUTINE(a_send_rel_comp)


/*
 * check_predicate
 *
 * Check the predicate_vector in the lec_info struct against a
 * particular predicate.  Return TRUE if the predicate in the vector
 * is set, FALSE if not set.
 */
static inline 
boolean check_predicate (lec_info_t *lec_info, ulong predicate)
{
    if (lec_info->predicate_vector & predicate) {
	LEC_STATE_DEBUG ("\nLEC %s: predicate %s TRUE",
			 lec_info->swidb->namestring,
			 lec_prpred(predicate));
	return(TRUE);
    } else {
	LEC_STATE_DEBUG ("\nLEC %s: predicate %s FALSE",
			 lec_info->swidb->namestring,
			 lec_prpred(predicate));
	return(FALSE);
    }
}

/*
 * set_predicate
 *
 * Set bits in the predicate vector.
 */
void set_predicate (lec_info_t *lec_info, ulong predicate)
{
    lec_info->predicate_vector |= predicate;
}

/*
 * reset_predicate
 *
 * Reset bits in the predicate vector.
 */
static inline
void reset_predicate (lec_info_t *lec_info, ulong predicate)
{
    lec_info->predicate_vector &= ~predicate;
}

/* 
 * lec_save_config_vals
 *
 */
static inline 
void lec_save_config_vals (lec_info_t *lec_info,
			   lec_config_source_t cfg_source)
{
    /* temp...probably won't stay this way..should set the
     * config vals in the state machine--not the actual vars
     */
    if (!lec_info->lec_snmp)
	return;

    lec_info->lec_snmp->lec_config_source = cfg_source;
}

/*
 * a_send_rel_comp
 *
 * The LEC does not expect a SETUP message at this state.
 * Send a RELEASE_COMPLETED to reject the call.
 */
static ushort a_send_rel_comp (lec_info_t *lec_info)
{
    lec_vc_t    released_vc;
 
    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }
 
    LEC_STATE_DEBUG ("\nLEC %s: action A_SEND_REL_COMP",
		     lec_info->swidb->namestring);
 
    released_vc.swidb 	= lec_info->data.sig_msg->idb;
    released_vc.call_id = lec_info->data.sig_msg->u.setup.call_id;
    lec_send_release_comp(&released_vc,
			  ATM_NORMAL_UNSPECIFIED);
 
    return(FSM_OK);
}
  



/*
 * lec_idle
 *
 * The lec is waiting for an event from the ILMI or the idle timer.
 * When the LEC wakes, it will reload the predicate vector based on it's
 * configuration information, and if possible, construct an ATM address
 * based upon the ATM prefix.  Once the address is constructed, the LEC
 * will SET it in the switch, before beginning JOIN the procedure.
 */
const static struct fsm_state lec_idle[] =
{
/*  Action        Arguments  Next State         Decoded Input          		*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_REGISTER_ADDR, FSM_NOARG, FSM_NOCHANGE},  /* LOCAL, ATM Prefix set		*/
{A_REGISTER_ADDR, FSM_NOARG, FSM_NOCHANGE},  /* LOCAL, Config Change		*/
{A_REGISTER_ADDR, FSM_NOARG, FSM_NOCHANGE},  /* LOCAL, Activate			*/
{A_SHUTDOWN_LEC, FSM_NOARG, FSM_NOCHANGE},   /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req      		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_REL_COMP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP                      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK      		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, RELEASE     		*/
{A_PROCESS_IDLE_REL_COMP, FSM_NOARG, FSM_NOCHANGE}, /* SIG, RELEASE_COMP	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{A_REGISTER_ADDR, FSM_NOARG, FSM_NOCHANGE},  /* TIMER, IDLE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_register_addr
 *
 * The LEC has sent the SNMP SET request to the switch to set the LEC
 * ATM address, and is waiting for the SNMP GET response that
 * indicates that the SET happened.  Once it's done, the LEC must get the
 * LECS ATM address and begin connection setup to it.
 * Note that even if the ILMI comes back with a negative response, we
 * continue since there may be switches out there that don't speak ILMI.
 */
const static struct fsm_state lec_register_addr[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set  	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{A_POST_LISTEN, FSM_NOARG, LEC_ST_POSTING_LISTEN}, /* CTL, ILMI Get Rsp+  	*/
{A_POST_LISTEN_NO_ILMI, FSM_NOARG, LEC_ST_POSTING_LISTEN}, /* CTL, ILMI Get Rsp- */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_REL_COMP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP                      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK      		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, RELEASE      		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, RELEASE_COMP  	    	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_posting_listen
 *
 * When the LEC first climbs out of the muck, it must re-activate the
 * signaling by posting a listen on the NSAP that it is supposed to use.
 * It waits in this state until signaling comes back with the right answer.
 */
const static struct fsm_state lec_posting_listen[] =
{
/*  Action        Arguments  Next State         Decoded Input          		*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING},/* LOCAL, ATM Prefix set  	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING},/* LOCAL, Config Change		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING},/* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING},/* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req      		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_REL_COMP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP                      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK      		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, RELEASE     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, RELEASE_COMP 		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{A_ACTIVATE_LEC, FSM_NOARG, FSM_NOCHANGE},   /* SIG, LISTEN+			*/
{A_PROCESS_FAILED_LISTEN, FSM_NOARG, LEC_ST_IDLE}, /* SIG, LISTEN-		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_get_lecs_addr
 *
 * The LEC has sent the ILMI GET to the switch for the LECS address
 * and is waiting for the ILMI GET Rsp with the answer.
 */
const static struct fsm_state lec_get_lecs_addr[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set  	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{A_ALLOC_LECS_ADDR, FSM_NOARG, FSM_NOCHANGE}, /* CTL, ILMI Get Rsp+  		*/
{A_SEND_LECS_SETUP, FSM_NOARG, LEC_ST_LECS_CONNECT}, /* CTL, ILMI Get Rsp-	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_REL_COMP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP                      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK      		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, RELEASE      		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, RELEASE_COMP      		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_lecs_connect
 *
 * The LEC obtained the LECS ATM address, sent the SETUP
 * request, and is now waiting for the CONNECT back from the LECS.  
 * When it receives the CONNECT, the LEC will send the LEC Config Req
 * to the LECS to get the address of the LES.  If a
 * release/release_comp comes back, the LEC looks up the next atm
 * address to use.
 */
const static struct fsm_state lec_lecs_connect[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req     	  	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_REL_COMP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP                      */
{A_SEND_CFG_REQ, FSM_NOARG, LEC_ST_GET_LES_ADDR}, /* SIG, CONNECT		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK      		*/
{A_SEND_NEXT_LECS_SETUP, FSM_NOARG, FSM_NOCHANGE}, /* SIG, RELEASE     		*/
{A_SEND_NEXT_LECS_SETUP, FSM_NOARG, FSM_NOCHANGE}, /* SIG, RELEASE_COMP 	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_get_les_addr
 *
 * The LEC successfully connected to the LECS and sent the
 * LECS_CONFIG_REQ.  It's waiting for the LECS_CONFIG_RSP.  If it 
 * gets a positive Rsp, it saves the information, and closes the VC.
 */
const static struct fsm_state lec_get_les_addr[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+   	  	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{A_PROCESS_CFG_RSP, FSM_NOARG, LEC_ST_LECS_RELEASE}, /* CTL, LECS CFG Rsp+   	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* CTL, LECS CFG Rsp- 		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_REL_COMP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP                      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK   	   	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE_COMP   	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{A_RESEND_CFG_REQ, FSM_NOARG, FSM_NOCHANGE}, /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_lecs_release
 *
 * Wait for the release complete coming back from the LECS before
 * opening up the VC to the LES.
 */
const static struct fsm_state lec_lecs_release[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set  	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+   	  	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_REL_COMP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP                      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK   	   	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, RELEASE      		*/
{A_SEND_LES_SETUP, FSM_NOARG, LEC_ST_CTL_DIRECT_CONN},/* SIG, RELEASE_COMP    	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_ctl_direct_conn
 *
 * The LEC has obtained the ATM address of the LES from the LECS, and
 * sent the SETUP to create the control direct connection to the
 * LES.  It's now waiting for the CONNECT to come back.  When it
 * receives the CONNECT, the LEC immediately sends the JOIN request.
 */
const static struct fsm_state lec_ctl_direct_conn[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp- 	    	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_REL_COMP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP                      */
{A_SEND_JOIN_REQ, FSM_NOARG, LEC_ST_JOIN_CTL_DIST_CONN}, /* SIG, CONNECT	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE_COMP 	     	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/* 
 * lec_join_ctl_dist_conn
 *
 * The LEC has a control direct connection up and has sent the
 * JOIN_REQ to the LES.  In this state, the LEC waits for:
 *
 * 1) The LE_JOIN_RSP.
 * 2) The incoming control distribute SETUP from the LES.
 *
 * According to the spec, the LES may establish the control
 * distribute VC. If it does, it should establish the VC before
 * sending LEC the LE_JOIN_RSP.
 * The LEC will not accept the incoming control distribute SETUP
 * after receiving the LE_JOIN_RSP.
 *
 * Upon receipt of the incoming SETUP request, the LEC will go 
 * into the LEC_ST_JOIN state, waiting for the JOIN_RSP.
 */
const static struct fsm_state lec_join_ctl_dist_conn[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set  	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+   	  	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{A_SEND_BUS_ARP, FSM_NOARG, LEC_ST_GET_BUS_ADDR},/* CTL, LES Join Rsp+          */
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* CTL, LES Join Rsp-	    	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req    	   	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_PROCESS_CTL_DIST_SETUP, FSM_NOARG, LEC_ST_JOIN}, /* SIG, SETUP              	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE_COMP  	    	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{A_RESEND_JOIN_REQ, FSM_NOARG, FSM_NOCHANGE},  /* TIMER, LES Join 		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_ctl_dist_conn
 *
 * The LEC has sent and received the JOIN PDU and is now waiting for:
 *
 * 1) The SETUP from the LES to open the control distribute connection.
 */
const static struct fsm_state lec_ctl_dist_conn[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set  	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-  		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req      	 	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_PROCESS_CTL_DIST_SETUP_SEND_REG_REQ,
 FSM_NOARG, FSM_NOCHANGE},                   /* SIG, SETUP        		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK	      	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE     	 	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE_COMP  		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_join
 *
 * The LEC has opened the control direct and control distribute
 * connections to the LES and is now waiting on the LES to send back the
 * JOIN response.  If the LEC receives a positive JOIN, it will send
 * any necessary registrations to the LES.  If none are required, it 
 * sends the le-arp to the BUS.
 */
const static struct fsm_state lec_join[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set  	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{A_PROCESS_JOIN_RSP_SEND_REQ, FSM_NOARG, FSM_NOCHANGE}, /* CTL, LES Join Rsp+	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* CTL, LES Join Rsp-   	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_REL_COMP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP                      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE_COMP 	     	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{A_RESEND_JOIN_REQ, FSM_NOARG, FSM_NOCHANGE},  /* TIMER, LES Join		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_registration
 *
 * The LEC has opened the control direct and control distribute
 * connections to the LES and has received the JOIN response.  The
 * registration of any unregistered LAN Destinations has been sent to
 * LES and we're waiting for a response.
 */
const static struct fsm_state lec_registration [] =
{
/*  Action        Arguments  Next State          Decoded Input			*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events			*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{A_SEND_BUS_ARP, FSM_NOARG, LEC_ST_GET_BUS_ADDR}, /*LOCAL, registration done	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change		*/
{A_PROCESS_REG_RSP, FSM_NOARG, LEC_ST_GET_BUS_ADDR}, /* CTL, LE_REG Rsp+	*/
{A_RESEND_REG_REQ, FSM_NOARG, FSM_NOCHANGE}, /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, SETUP			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE_COMP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{A_RESEND_REG_REQ, FSM_NOARG, FSM_NOCHANGE},  /* TIMER, Register Req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/* 
 * lec_get_bus_addr
 *
 * After receiving the positive JOIN response and getting both control
 * direct and distribute VCCs up, the LEC has to LE-ARP to get the BUS
 * ATM address and wait in this state to receive the response. 
 */
const static struct fsm_state lec_get_bus_addr[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set 	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req       		*/
{A_SEND_BUS_SETUP, FSM_NOARG, LEC_ST_MCAST_SEND_FORWARD_CONN},/* CTL, LE-ARP Rsp*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_REL_COMP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP                      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK  	    	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE_COMP  		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{A_SEND_BUS_ARP, FSM_NOARG, LEC_ST_GET_BUS_ADDR},/* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_mcast_send_forward_conn
 *
 * The LEC received the response from the LE-ARP, and sent the UNI
 * SETUP request.  It's now waiting on the BUS to respond with the
 * CONNECT for the multicast send vc.  There is also the possiblity 
 * that the SETUP for the multicast forward vc may
 * arrive from the BUS before the send VC gets completed.
 */
const static struct fsm_state lec_mcast_send_forward_conn[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set  	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_BUS_CONNECT, FSM_NOARG, LEC_ST_MCAST_SEND_CONN}, /* SIG, SETUP         	*/
{A_PROCESS_BUS_CONNECT, FSM_NOARG, LEC_ST_MCAST_FORWARD_CONN}, /* SIG, CONNECT	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE_COMP 		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_mcast_send_conn
 *
 * The LEC is still waiting for the send VC to get connected.  It's
 * already sent back the CONNECT in response to the multicast forward
 * getting opened.
 */
const static struct fsm_state lec_mcast_send_conn[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set  	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+   	  	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-   	  	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_REL_COMP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP                      */
{A_PROCESS_BUS_CONNECT, FSM_NOARG, LEC_ST_ACTIVE},/* SIG, CONNECT		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE_COMP  		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY     		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join   	 	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_mcast_forward_conn
 *
 * The LEC already has a connection to the BUS through the multicast send VC
 * and is waiting for the multicast forward VC to be set up.  The LEC
 * will wait here for either an ADD_PARTY or a SETUP from the BUS.
 */
const static struct fsm_state lec_mcast_forward_conn[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set  	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_BUS_CONNECT, FSM_NOARG, LEC_ST_ACTIVE}, /* SIG, SETUP              	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE      		*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* SIG, RELEASE_COMP  	 	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join   	 	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};



/*
 * lec_active
 *
 * We've made it to active (!).  Now the LEC state machine
 * has to handle ARPs, FLUSH, and connections getting torn down.
 */
const static struct fsm_state lec_active[] =
{
/*  Action        Arguments  Next State          Decoded Input          	*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, ATM Prefix set 	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Config Change	*/
{A_TEARDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Activate		*/
{A_SHUTDOWN_LEC, FSM_NOARG, LEC_ST_TERMINATING}, /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete	*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+   	  	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{A_SEND_ARP_RSP, FSM_NOARG, FSM_NOCHANGE},   /* CTL, LE-ARP Req       		*/
{A_PROCESS_ARP_RSP, FSM_NOARG, FSM_NOCHANGE}, /* CTL, LE-ARP Rsp       		*/
{A_PROCESS_NARP_REQ, FSM_NOARG, FSM_NOCHANGE}, /* CTL, LE-NARP    		*/
{A_PROCESS_READY_QUERY, FSM_NOARG, FSM_NOCHANGE}, /* CTL, READY QUERY  		*/
{A_PROCESS_READY_IND, FSM_NOARG, FSM_NOCHANGE}, /* CTL, READY IND    		*/
{A_SEND_FLUSH_RSP, FSM_NOARG, FSM_NOCHANGE}, /* CTL, Flush Req    		*/
{A_PROCESS_FLUSH_RSP, FSM_NOARG, FSM_NOCHANGE}, /* CTL, Flush Rsp    		*/
{A_PROCESS_TOPO_CHANGE, FSM_NOARG, FSM_NOCHANGE}, /* CTL, TOPOLOGY Change   	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-		*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_PROCESS_SETUP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP              	*/
{A_PROCESS_CONNECT, FSM_NOARG, FSM_NOCHANGE}, /* SIG, CONNECT			*/
{A_PROCESS_CONNECT_ACK, FSM_NOARG, FSM_NOCHANGE}, /* SIG, CONNECT_ACK   	*/
{A_PROCESS_RELEASE, FSM_NOARG, FSM_NOCHANGE}, /* SIG, RELEASE 	     		*/
{A_PROCESS_REL_COMP, FSM_NOARG, FSM_NOCHANGE}, /* SIG, RELEASE_COMP    		*/
{A_PROCESS_ADD_PARTY, FSM_NOARG, FSM_NOCHANGE}, /* SIG, ADD_PARTY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, IDLE 			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{A_PROCESS_READY_TIMER, FSM_NOARG, FSM_NOCHANGE}, /* TIMER, READY		*/
{A_PROCESS_ARP_AGE_TIMER, FSM_NOARG, FSM_NOCHANGE}, /* TIMER, ARP AGE		*/
{A_PROCESS_ARP_CONTROL_TIMER, FSM_NOARG, FSM_NOCHANGE}, /* TIMER, ARP CONTROL   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{A_PROCESS_FLUSH_TIMER, FSM_NOARG, FSM_NOCHANGE}, /* TIMER, FLUSH		*/
};



/*
 * lec_terminating
 *
 * LEC waits in this state until all control VCs are cleared.
 */
const static struct fsm_state lec_terminating[] =
{
/*  Action        Arguments  Next State         Decoded Input          		*/
/*------------------------------------------------------------------------------*/
                                                /* Local Events         	*/
{A_TEARDOWN_LEC, FSM_NOARG, FSM_NOCHANGE},   /* LOCAL, ATM Prefix set	  	*/
{A_TEARDOWN_LEC, FSM_NOARG, FSM_NOCHANGE},   /* LOCAL, Config Change		*/
{A_TEARDOWN_LEC, FSM_NOARG, FSM_NOCHANGE},   /* LOCAL, Activate			*/
{A_SHUTDOWN_LEC, FSM_NOARG, FSM_NOCHANGE},   /* LOCAL, Deactivate		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* LOCAL, register complete        */
/*------------------------------------------------------------------------------*/
                                                /* LEC Control Events         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, ILMI Get Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LECS CFG Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp+     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LES Join Rsp-     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Req      		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-ARP Rsp       		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE-NARP    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY QUERY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, READY IND    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Req    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, Flush Rsp    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, TOPOLOGY Change   		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp+           */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* CTL, LE_REG Rsp-           */
/*------------------------------------------------------------------------------*/
                                                /* LEC Signalling Events      	*/
{A_SEND_REL_COMP, FSM_NOARG, FSM_NOCHANGE},  /* SIG, SETUP                      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CONNECT_ACK      		*/
{A_PROCESS_TERM_RELEASE, FSM_NOARG, FSM_NOCHANGE}, /* SIG, RELEASE     		*/
{A_PROCESS_TERM_REL_COMP, FSM_NOARG, FSM_NOCHANGE}, /* SIG, RELEASE_COMP 	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, ADD_PARTY     		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, DROP_PARTY         	*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN+			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, LISTEN-			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* SIG, CANCEL			*/
/*------------------------------------------------------------------------------*/
                                                /* LEC Timer Events             */
{A_REGISTER_ADDR, FSM_NOARG, LEC_ST_REGISTER_ADDR}, /* TIMER, IDLE 		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LECS CFG req		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, LES Join    		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, BUS LE-ARP		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, READY			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP AGE			*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, ARP CONTROL		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, Register Request		*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},     /* TIMER, FLUSH			*/
};


#define LEC_FSM_DECODE FSM_DECODE_ROUTINE(lec_fsm_decode)


const static struct finite_state_machine lec_fsm_table[] =
{
/*  State       		Decoder			State Value     	*/
{   lec_idle,			LEC_FSM_DECODE}, /* LEC_ST_IDLE			*/
{   lec_register_addr,	 	LEC_FSM_DECODE}, /* LEC_ST_REGISTER_ADDR	*/
{   lec_posting_listen,		LEC_FSM_DECODE}, /* LEC_ST_POSTING_LISTEN	*/
{   lec_get_lecs_addr,		LEC_FSM_DECODE}, /* LEC_ST_GET_LECS_ADDR	*/
{   lec_lecs_connect,		LEC_FSM_DECODE}, /* LEC_ST_LECS_CONNECT		*/
{   lec_get_les_addr,		LEC_FSM_DECODE}, /* LEC_ST_GET_LES_ADDR		*/
{   lec_lecs_release,		LEC_FSM_DECODE}, /* LEC_ST_LECS_RELEASE		*/
{   lec_ctl_direct_conn,	LEC_FSM_DECODE}, /* LEC_ST_CTL_DIRECT_CONN 	*/
{   lec_join_ctl_dist_conn,	LEC_FSM_DECODE}, /* LEC_ST_JOIN_CTL_DIST_CONN 	*/
{   lec_ctl_dist_conn,		LEC_FSM_DECODE}, /* LEC_ST_CTL_DIST_CONN 	*/
{   lec_join,			LEC_FSM_DECODE}, /* LEC_ST_JOIN			*/
{   lec_registration,           LEC_FSM_DECODE}, /* LEC_ST_REGISTRATION */
{   lec_get_bus_addr,		LEC_FSM_DECODE}, /* LEC_ST_GET_BUS_ADDR		*/
{   lec_mcast_send_forward_conn,LEC_FSM_DECODE}, /* LEC_ST_MCAST_SEND_FORWARD_CONN */
{   lec_mcast_send_conn,	LEC_FSM_DECODE}, /* LEC_ST_MCAST_SEND_CONN 	*/
{   lec_mcast_forward_conn,	LEC_FSM_DECODE}, /* LEC_ST_MCAST_FORWARD_CONN 	*/
{   lec_active,			LEC_FSM_DECODE}, /* LEC_ST_ACTIVE		*/
{   lec_terminating,		LEC_FSM_DECODE}, /* LEC_ST_TERMINATING		*/
};


/*
 * lec_fsm_decode
 * 
 * Given major and minor event inputs, produce the magic number that
 * indexes into each of the state arrays.
 */
int
lec_fsm_decode (lec_info_t *lec_info, int major, int minor)
{
    switch (major) {
    case LEC_MAJOR_LOCAL:
	if (minor < MAX_LEC_LOCAL_EVENT) {
	    return(LEC_LOCAL_EVENT_BASE + minor);
	} else {
	    return(FSM_FAULT);
	}
	break;
    case LEC_MAJOR_CONTROL:
	if (minor < MAX_LEC_CTL_EVENT) {
	    return(LEC_CTL_EVENT_BASE + minor);
	} else {
	    return(FSM_FAULT);
	}
	break;
    case LEC_MAJOR_SIGNALLING:
	if (minor < MAX_LEC_SIG_EVENT) {
	    return(LEC_SIG_EVENT_BASE + minor);
	} else {
	    return(FSM_FAULT);
	}
	break;
    case LEC_MAJOR_TIMER:
	if (minor < MAX_LEC_TIMER_EVENT) {
	    return(LEC_TIMER_EVENT_BASE + minor);
	} else {
	    return(FSM_FAULT);
	}
	break;
    default:
	return(FSM_FAULT);
    }
}

/*
 * a_process_flush_timer
 *
 * This routine is called after sending an LE-ARP and not getting a
 * response after the control timeout has expired.
 */
ushort a_process_flush_timer (lec_info_t *lec_info)
{
    lec_le_arp_t *le_arp_entry;
    lec_nsap_info_t	*lec_nsap_info;


    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_FLUSH_TIMER",
		     lec_info->swidb->namestring);

    le_arp_entry = lec_info->data.le_arp_entry;
    if (le_arp_entry && 
	le_arp_entry->entry_type != LEC_LE_ARP_PERMANENT) {

	switch (le_arp_entry->state) {
	case LEC_MAC_DIRECT:
	    /*
	     * If the entry state was in DIRECT, we've already got the
	     * response, so turn off the flush timer.
	     */

	    break;

	case LEC_MAC_FLUSH:
	    /*
	     * If the le_arp entry is in LEC_MAC_FLUSH state, we haven't
	     * received the flush response. Since we have already waited for
	     * C22_path_switching delay, go to data direct.
	     */

	    break;

	default:
	    /*
	     * Nothing much can be done, can we?
	     */
	    break;
	}
	lec_stop_flush_timer(le_arp_entry);
	lec_nsap_info = lec_lookup_nsap(lec_info, &le_arp_entry->nsap);
	if (lec_nsap_info) {
	    if ((lec_nsap_info->vc.call_state == CALL_STATE_UP) &&
		 (lec_nsap_info->ready_state    == CALL_READY)) {
		if (le_arp_entry->state != LEC_MAC_REVERIFYING) {
		    le_arp_entry->state = LEC_MAC_DIRECT;
		    reg_invoke_tbridge_lane_bte_update(lec_info,
						       &le_arp_entry->mac[0],
						       lec_nsap_info->vc.vcd,
						       BTE_READY);
		}
		TIMER_STOP(le_arp_entry->unknown_time);
		return(FSM_OK);
	    }
	} else {
	    le_arp_entry->state = LEC_MAC_RESOLVED;
	    reg_invoke_tbridge_lane_bte_update(lec_info,
					       &le_arp_entry->mac[0],
					       lec_info->mcast_send.vcd,
					       BTE_LANE_RESOLVED);
	    lec_lookup_connection(le_arp_entry);	
	}
    }
    
    return(FSM_OK);
}

/*
 * lec_init_pred_vector
 *
 * Check configured options such as overridden NSAPs and set 
 * up the predicate vector.
 */
static void
lec_init_pred_vector (lec_info_t *lec_info)
{
    if (lec_info->nsap.length) {
	set_predicate(lec_info, PRED_LEC_NSAP);
    } else {
	reset_predicate(lec_info, PRED_LEC_NSAP);
    }

    if ((lec_info->configure.svc) &&
	(lec_info->configure.nsap.length != 0)) {
	set_predicate(lec_info, PRED_LECS_NSAP);
    } else {
	reset_predicate(lec_info, PRED_LECS_NSAP);
    }

    if ((lec_info->ctl_direct.svc) &&
	(lec_info->ctl_direct.nsap.length != 0)) {
	set_predicate(lec_info, PRED_CTL_DIRECT_NSAP);
    } else {
	reset_predicate(lec_info, PRED_CTL_DIRECT_NSAP);
    }

    if ((lec_info->mcast_send.svc) &&
	(lec_info->mcast_send.nsap.length != 0)) {
	set_predicate(lec_info, PRED_MCAST_SEND_NSAP);
    } else {
	reset_predicate(lec_info, PRED_MCAST_SEND_NSAP);
    }

}


/*
 * lec_save_join_resp_params
 *
 * helper routine for a_process_join_rsp & a_send_bus_arp.
 */
static void
lec_save_join_resp_params (lec_info_t *lec_info, lane_ctl_msg_u *msg)
{
    lane_info_t         *lane_info = lec_info->lane_info;

    lec_info->lecid = GETSHORT(&msg->hdr.requestor_lecid);
    lec_info->actual_lan_type       = msg->join.lan_type;
    lec_info->actual_max_frame_size = msg->join.max_frame_size;
    lec_info->actual_name_size      = msg->join.name_size;
    bcopy(msg->join.elan_name, lec_info->actual_elan_name,
          msg->join.name_size);
    /*
     * Put name into lane_info if we didn't have one before.
     */ 
    if (lane_info->name_size == 0) {
        lane_info->name_from_lec = TRUE;
    }           
}


/*
 * lec_clear_vc
 *
 * Utility function to clear out the items in the lec_vc_t struct that
 * need to be reinitialized when the vc is finally dead.
 */
void lec_clear_vc (lec_info_t *lec_info, lec_vc_t *vc)
{
    reg_invoke_lecmib_count_deleted_SVC(lec_info, vc);
    vc->vcd		= 0;
    vc->call_state	= CALL_STATE_DOWN;
    vc->call_id		= 0;
}


/*
 * lec_check_idle
 *
 * Used in terminating the LEC to check if all of the control VCCs are
 * down, and if so, transition to the idle state.  Also sets the IDLE
 * timer if the transition is made.
 */
static void lec_check_idle (lec_info_t *lec_info)
{
    if ((lec_info->configure.call_state == CALL_STATE_DOWN) &&
	(lec_info->ctl_direct.call_state == CALL_STATE_DOWN) &&
	(lec_info->ctl_distribute.call_state == CALL_STATE_DOWN) &&
	(lec_info->mcast_send.call_state == CALL_STATE_DOWN) &&
	(lec_info->mcast_forward.call_state == CALL_STATE_DOWN)) {
 
	/* 
	 * LEC is set to IDLE state only when all the VCs are down.
	 * This prevents the lec from entering next setup phase,
	 * when some of the VCs are still in disconnecting state.
	 */
	if (lec_info->admin_status == LANE_INACTIVE) {
	    lec_info->state = LEC_ST_IDLE;
	    return;
	}
	/*
	 * Minor kludge alert.  The idle timer is used to determine if
	 * this routine has been called the first time in a teardown
	 * scenario.
	 */
	if (!lec_info->done_first_idle_timeout) {
	    lec_info->done_first_idle_timeout = TRUE;
	    errmsg(&msgsym(UPDOWN, LANE),
		   lec_info->swidb->namestring,
		   "elan",
		   lec_info->lane_info->name,
		   "LE Client", "down");
	    /*
	     * Change the line state to down
	     */
	    lec_line_statechange(lec_info->swidb, 
				 SUBIF_STATE_ADMINDOWN);

	    /* if the user does not configure the idle timer,
	     * we will take the system default idle timer.
	     */
	    if (!lec_info->config_idle_time)
		lec_info->idle_time = LEC_IDLE_TIMEOUT;
	    else
		lec_info->idle_time = lec_info->config_idle_time;

	} else {
	    if (!lec_info->config_idle_time) {
		lec_info->idle_time += lec_info->idle_time;
		if (lec_info->idle_time >= LEC_IDLE_TIMEOUT_MAX) {
		    lec_info->idle_time = LEC_IDLE_TIMEOUT_MAX;
		}
	    }
	}

	lec_start_timer(lec_info, lec_info->idle_time, LEC_TIMER_TYPE_IDLE);
	lec_info->state = LEC_ST_IDLE;
    }
}


/*
 * a_set_lec_active
 *
 * Generic function to set the operational state to ACTIVE.  This is
 * defined as an action function, but it can actually be called out
 * of the state machine in addition to being called from within other action
 * functions.
 */
ushort a_set_lec_active (lec_info_t *lec_info)
{
    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    lec_stop_timer(lec_info);

    lec_free_lecs_addrQ(lec_info);

    lec_info->done_first_idle_timeout = FALSE;

    lec_info->oper_status = LANE_ACTIVE;

    GET_TIMESTAMP(lec_info->time_when_active);
    reg_invoke_lec_port_create(lec_info);

    errmsg(&msgsym(UPDOWN, LANE),
	   lec_info->swidb->namestring,
	   "elan",
	   lec_info->lane_info->name,
	   "LE Client", "up");

    lec_line_statechange(lec_info->swidb, 
			 SUBIF_STATE_UP);

    return(FSM_OK);
}



/*
 * a_activate_lec
 *
 * This action function is called whenever the LEC should be activated
 * (no shit).  At this point, we should have registered with the ILMI
 * and calculated our own ATM address.
 *
 * The LEC received the ILMI Get_Response+ which acknowledges the SET
 * of it's ATM address to the switch succeeded and the SETUP has to be
 * sent to the LECS.  If there is an NSAP to the LECS, then don't
 * bother querying the switch, but send the SETUP request and go to
 * the LECS connect state. 
 */
ushort a_activate_lec (lec_info_t *lec_info)
{
    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_ACTIVATE_LEC",
		     lec_info->swidb->namestring);

    lec_line_statechange(lec_info->swidb, 
			 SUBIF_STATE_ADMINDOWN);

    /*
     * Make sure to set C4_proxy field, if bridging is enabeld
     */
    if (reg_invoke_lec_bridging_enabled(lec_info->swidb)) {
	lec_info->C4_proxy = TRUE;
    } else {
	lec_info->C4_proxy = FALSE;
    }

    /*
     * If we already have a control direct NSAP to use, go ahead and
     * try joining.
     */
    if (check_predicate(lec_info, PRED_CTL_DIRECT_NSAP)) {
	/* save the config values for SNMP */
	lec_save_config_vals(lec_info, LEC_CONFIG_TYPE_NO_LECS);

	/*
	 * Send a SETUP to the LES
	 */
	lec_send_setup(lec_info, &lec_info->ctl_direct);
	lec_info->state = LEC_ST_CTL_DIRECT_CONN;
	return (FSM_OK);
    }

    /*
     * If there is an LECS NSAP configured, go ahead and connect to
     * it.  Note that we return into the LECS_CONNECT state.
     */
    if (check_predicate(lec_info, PRED_LECS_NSAP)) {
	/* save the config values for SNMP...
	 * ??? don't know if this is the 'well-known' addr or
	 * ??? just an configured addr received via ILMI
	 */
	lec_save_config_vals(lec_info, LEC_CONFIG_TYPE_KNOWN_ADDR);

	/*
	 * Using the SigAPI, send the SETUP request to the LECS.
	 */
	lec_send_setup(lec_info, &lec_info->configure);
	lec_info->state = LEC_ST_LECS_CONNECT;
	return(FSM_OK);
    }

    /*
     * Query the switch if no ATM address 
     * configured to the LECS.
     */
    lec_send_get_lecs_addr(lec_info);
    lec_info->state = LEC_ST_GET_LECS_ADDR;

    return(FSM_OK);
}


/*
 * a_alloc_lecs_addr
 *
 * While the getNext continues to find more LECS addresses in the switch
 * address table, we allocate lecs_addr structs for later use.
 */
ushort a_alloc_lecs_addr (lec_info_t *lec_info)
{
    hwaddrtype addr;
    ilmiClientReq *notify;
    
    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_ALLOC_LECS_ADDR",
		     lec_info->swidb->namestring);
    
    
    notify = lec_info->data.notify;
    if (notify) {
	if ((notify->vblist->value.type == OCTET_PRIM_TYPE) &&
	    (notify->vblist->value.os_value->length == STATIONLEN_ATMNSAP)){
	    lec_make_hwaddr(&addr,
			    notify->vblist->value.os_value->octet_ptr,
			    STATION_ATMNSAP,
			    STATIONLEN_ATMNSAP);
	    lec_add_lecs_addr(lec_info, &addr);
	}
    }

    /* save the config values for SNMP 
     * ??? don't know what the configSource is...
     */
    lec_save_config_vals(lec_info, LEC_CONFIG_TYPE_ILMI);

    return(FSM_OK);
}


/*
 * a_post_listen
 *
 * Called out of the idle state.  Have to tell signaling which
 * NSAP we wish to initiate/receive connections on.
 */
ushort a_post_listen (lec_info_t *lec_info)
{
    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_POST_LISTEN",
		     lec_info->swidb->namestring);

    lec_listen(lec_info);

    return(FSM_OK);
}


/*
 * a_post_listen_no_ilmi
 *
 * If ILMI won't let us register the ATM address, go ahead and listen
 * on the address, but tell the user about it.
 */
ushort a_post_listen_no_ilmi (lec_info_t *lec_info)
{
    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_POST_LISTEN_NO_ILMI",
		     lec_info->swidb->namestring);

    errmsg(&msgsym(NOREGILMI, LANE),
	   lec_info->swidb->namestring, "LEC", lec_info->nsap.addr);

    lec_listen(lec_info);

    return(FSM_OK);
}


/*
 * a_process_add_party
 *
 * If the LEC receives an ADD_PARTY, it will return an ADD_PARTY_REJ,
 * since this implies that we already have a connection from the same
 * root to this interface.  At this time, we can't see any reason to
 * configure two LECs on the same ELAN on the same AIP.
 */
ushort a_process_add_party (lec_info_t *lec_info)
{
    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_ADD_PARTY",
		     lec_info->swidb->namestring);

    lec_send_add_party_rej(lec_info);

    return(FSM_OK);

}


/*
 * a_process_arp_age_timer
 *
 * An LE-ARP entry has just timed out.  It's time to reverify the
 * entry by sending an LE-ARP request.
 */
ushort a_process_arp_age_timer (lec_info_t *lec_info)
{
    lec_le_arp_t	*le_arp_entry;
    lec_nsap_info_t	*lec_nsap_info;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_ARP_AGE_TIMER",
		     lec_info->swidb->namestring);

    le_arp_entry = lec_info->data.le_arp_entry;
    if (le_arp_entry && 
	le_arp_entry->entry_type != LEC_LE_ARP_PERMANENT) {

	switch (le_arp_entry->state) {
	case LEC_MAC_DIRECT:
	    /*
	     * If the entry state was in DIRECT and the VCC is still
	     * there, we're going to reverify the entry.  Change the 
	     * state to indicate this, and send out an arp request 
	     * to start the reverification.
	     */
	    lec_nsap_info = lec_lookup_nsap(lec_info, 
					    &le_arp_entry->nsap);
	    if (!lec_nsap_info) {
		lec_delete_le_arp_entry(lec_info, le_arp_entry->mac);
	    } else {
		le_arp_entry->state = LEC_MAC_REVERIFYING;
		le_arp_entry->arp_retry_count = lec_info->C13_max_retry_count;
		bzero(le_arp_entry->src_mac, IEEEBYTES);
		lec_send_arp_req(lec_info, le_arp_entry->mac,
				 le_arp_entry->src_mac);
		lec_stop_arp_age_timer(le_arp_entry);
		lec_start_arp_control_timer(le_arp_entry);
	    }
	    break;
	case LEC_MAC_REVERIFYING:
	case LEC_MAC_FLOOD:
	case LEC_MAC_RESOLVED:
	case LEC_MAC_FLUSH:
	    /*
	     * For all others, simply delete the le_arp_entry.
	     */
	    lec_delete_le_arp_entry(lec_info, &le_arp_entry->mac[0]);
	    break;
	}

    }
    
    return(FSM_OK);
}


/*
 * a_process_arp_control_timer
 *
 * This routine is called after sending an LE-ARP and not getting a
 * response after the control timeout has expired.
 */
ushort a_process_arp_control_timer (lec_info_t *lec_info)
{
    lec_le_arp_t *le_arp_entry;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_ARP_CONTROL_TIMER",
		     lec_info->swidb->namestring);

    le_arp_entry = lec_info->data.le_arp_entry;
    if (le_arp_entry && 
	le_arp_entry->entry_type != LEC_LE_ARP_PERMANENT) {

	switch (le_arp_entry->state) {
	case LEC_MAC_DIRECT:
	case LEC_MAC_RESOLVED:
	case LEC_MAC_FLUSH:
	    /*
	     * If the entry state was in DIRECT, we've already got the
	     * response, so turn off the control timer.
	     */
	    lec_stop_arp_control_timer(le_arp_entry);
	    break;
	case LEC_MAC_REVERIFYING:
  	    bzero(le_arp_entry->src_mac, IEEEBYTES);
	case LEC_MAC_FLOOD:
	    /*
	     * If the arp control timer pops while in this state,
	     * we've got to retransmit the LE-ARP, until retry_count
	     * is gone.
	     */
	    if (le_arp_entry->arp_retry_count) {
		le_arp_entry->arp_retry_count--;
		lec_start_arp_control_timer(le_arp_entry);
		lec_send_arp_req(lec_info, le_arp_entry->mac,
				 le_arp_entry->src_mac);
	    } else {
		/*
		 * Delete the entry.  Seems that nobody is answering.
		 */
		lec_delete_le_arp_entry(lec_info, &le_arp_entry->mac[0]);
	    }
	    break;
	}

    }
    
    return(FSM_OK);
}

/*
 * process a narp request.  Typically from an HSRP operation
 */
ushort a_process_narp_req (lec_info_t *lec_info)
{
    lane_ctl_msg_u	*msg;
    uchar		*target_mac;
    target_mac = NULL;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_NARP_REQ",
		     lec_info->swidb->namestring);

    msg = (lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart;
    if (msg) 
    {
	target_mac = msg->arp.target_dest.addr.mac;
	lec_delete_le_arp_entry(lec_info, target_mac);
    }
    return FSM_OK;
}

/*
 * a_process_arp_rsp
 *
 * Check to see if we have an outstanding le_arp request for the lan
 * destination address in the message.
 */
ushort a_process_arp_rsp (lec_info_t *lec_info)
{
    lane_ctl_msg_u	*msg;
    uchar		*target_mac;
    hwaddrtype		target_nsap;
    lec_le_arp_t	*le_arp_entry;


    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_ARP_RSP",
		     lec_info->swidb->namestring);

    msg = (lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart;
    if (msg) {
	
	/*
	 * Check LECID...is it mine?
	 * Check source lane destination...is it mine?
	 * Check source atm address...is it mine?
	 */
	if (msg->hdr.requestor_lecid != lec_info->lecid) {
	    return(FSM_OK);
	}
	if (msg->arp.src_lan_dest.tag != LANE_TAG_MAC_ADDRESS) {
	    return(FSM_OK);
	}
	if (memcmp(msg->arp.src_atm_addr, 
		   lec_info->nsap.addr,
		   STATIONLEN_ATMNSAP)) {
	    return(FSM_OK);
	}

	/*
	 * Lookup the le_arp entry.  If the le-arp entry doesn't exist, 
	 * ignore the le-arp rsp packet and move on.  If there is an le-arp entry, 
	 * fill out the information received from the arp rsp.
	 */
	target_mac = msg->arp.target_dest.addr.mac;
	le_arp_entry = lec_lookup_le_arp(lec_info, target_mac);
	if (le_arp_entry &&
	    le_arp_entry->entry_type != LEC_LE_ARP_PERMANENT) {

	    switch (le_arp_entry->state) {
	    case LEC_MAC_FLOOD:
		/*
		 * Fill in the le_arp table entry with the resolved nsap
		 * address and set the entry state to LEC_MAC_RESOLVED.  
		 * A call to lec_lookup_connection  will result in a connection
		 * getting setup if it doesn't already exist.
		 */
		le_arp_entry->state = LEC_MAC_RESOLVED;

		lec_make_hwaddr(&target_nsap,
				msg->arp.target_atm_addr,
				STATION_ATMNSAP,
				STATIONLEN_ATMNSAP);
		bcopy(&target_nsap, &le_arp_entry->nsap, sizeof(hwaddrtype));
		if (msg->arp.hdr.flags == LANE_PROT_FLAG_REMOTE_ADDR) {
		    le_arp_entry->entry_type = LEC_LE_ARP_REMOTE;
		} else {
		    le_arp_entry->entry_type = LEC_LE_ARP_LOCAL;
		}
		le_arp_entry->arp_retry_count = lec_info->C13_max_retry_count;
		lec_start_arp_age_timer(le_arp_entry);
		lec_stop_arp_control_timer(le_arp_entry);
	    case LEC_MAC_RESOLVED:
		if (lec_info->C4_proxy) {
		    reg_invoke_tbridge_lane_bte_update(lec_info,
						       &le_arp_entry->mac[0],
					             lec_info->mcast_send.vcd,
						       BTE_LANE_RESOLVED);
		}
		lec_lookup_connection(le_arp_entry);
		break;
	    case LEC_MAC_REVERIFYING:
		/*
		 * If the target ATM address has changed, update the
		 * entry and put it in LEC_MAC_RESOLVED state.  If the
		 * target ATM address matches that of the LE_ARP
		 * entry, the reverification is complete, so set it back
		 * to LEC_MAC_DIRECT. 
		 */
		if (memcmp(msg->arp.target_atm_addr, 
			   le_arp_entry->nsap.addr,
			   STATIONLEN_ATMNSAP)) {
		    lec_make_hwaddr(&target_nsap,
				    msg->arp.target_atm_addr,
				    STATION_ATMNSAP,
				    STATIONLEN_ATMNSAP);
		    bcopy(&target_nsap, &le_arp_entry->nsap, sizeof(hwaddrtype));
		    le_arp_entry->state = LEC_MAC_RESOLVED;
		    reg_invoke_tbridge_lane_bte_update(lec_info,
						       &le_arp_entry->mac[0],
					             lec_info->mcast_send.vcd,
						       BTE_LANE_RESOLVED);
		    reg_invoke_bump_cache_version(lec_info->swidb->hwptr,
						  lec_info->swidb, FALSE);

		    lec_lookup_connection(le_arp_entry);
		} else {
		    le_arp_entry->state = LEC_MAC_DIRECT;
		}
		le_arp_entry->arp_retry_count = lec_info->C13_max_retry_count;
		lec_start_arp_age_timer(le_arp_entry);
		lec_stop_arp_control_timer(le_arp_entry);
		break;
	    case LEC_MAC_FLUSH:
	    case LEC_MAC_DIRECT:
		break;
	    }
	}
    }

    return(FSM_OK);
}


/*
 * a_process_flush_rsp
 *
 * Insure that this flush response it intended for a particular MAC and change
 * state accordingly. The MAC entry in le_arp cache has to be in LEC_MAC_FLUSH
 * Move it to LEC_MAC_DIRECT.
 */
ushort a_process_flush_rsp (lec_info_t *lec_info)
{
    lane_ctl_msg_u	*msg;
    lec_le_arp_t	*le_arp_entry;
    lec_nsap_info_t	*lec_nsap_info;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_FLUSH_RSP",
		     lec_info->swidb->namestring);

    if (lec_info->data.control_pak) {

	msg = (lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart;

        /*
	 * If the flush response came back to us, ignore if target_atm address
	 * matches ours.
	 */
	if (!memcmp(msg->flush.target_atm_addr, 
		    lec_info->nsap.addr,
		    STATIONLEN_ATMNSAP)) {
	    return(FSM_OK);
	}
        /*
	 * Go through the LE_ARP cache and insure that this entry is for real.
	 */  
	if (lec_find_arp_ptr(lec_info, 
			     (lec_le_arp_t *)msg->hdr.transaction_id)) {
	    le_arp_entry = (lec_le_arp_t *)msg->hdr.transaction_id;
	} else {
	    return(FSM_OK);
	}
	
	lec_nsap_info = lec_lookup_nsap(lec_info, &le_arp_entry->nsap);
	if (lec_nsap_info) {
	    if ((lec_nsap_info->vc.call_state == CALL_STATE_UP) &&
		 (lec_nsap_info->ready_state    == CALL_READY)) {
		if (le_arp_entry->state != LEC_MAC_REVERIFYING) {
		    le_arp_entry->state = LEC_MAC_DIRECT;
		    reg_invoke_tbridge_lane_bte_update(lec_info,
						       &le_arp_entry->mac[0],
						       lec_nsap_info->vc.vcd,
						       BTE_READY);
		}
		TIMER_STOP(le_arp_entry->unknown_time);
		lec_stop_flush_timer(le_arp_entry);
		return(FSM_OK);
	    }
	}
	/*
	 * Really to insure that we can go back to flooding and go through
	 * LE_ARP process. Otherwise, this MAC is dead as far as Bridge
	 * Processing is concerned (state of BTE indicates BTE_LANE_FLUSH)
	 */
	le_arp_entry->state = LEC_MAC_RESOLVED;
	reg_invoke_tbridge_lane_bte_update(lec_info,
					   &le_arp_entry->mac[0],
 					   lec_info->mcast_send.vcd,
					   BTE_LANE_RESOLVED);
	lec_lookup_connection(le_arp_entry);
    }
    return(FSM_OK);
}


/*
 * a_process_bus_connect
 *
 * Processes incoming bus mcast_send connect.  This had better be a bus
 * connect, else we flame all over the place and drop the JOIN.
 */
ushort a_process_bus_connect (lec_info_t *lec_info)
{
    sig_api_msg	*msg;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_BUS_CONNECT",
		     lec_info->swidb->namestring);

    msg = lec_info->data.sig_msg;
    if (msg) {
	/*
	 * Should also check blli.
	 */
	if (msg->u.connect.blli.length &&
	    (!lane_sig_blli_match(1, &msg->u.connect.blli,
				  lec_info->mcast_send.lane_vc_type))) {
 
	    LEC_STATE_DEBUG("\nLEC %s: wrong blli in connect, teardown LEC",
			    lec_info->swidb->namestring);

	    lec_fail_reason = LEC_FAIL_BLLI_SEND;
	    return(a_teardown_lec(lec_info));
	}

	lec_info->mcast_send.vcd = msg->u.connect.vcd;
	lec_info->mcast_send.call_state = CALL_STATE_UP;
	/*
	 * In the event that the mcast_forward got set up before the
	 * send, we pop out of here and set the LEC active since it's
	 * all done now.
	 */
	if (lec_info->mcast_forward.call_state == CALL_STATE_UP) {
	    a_set_lec_active(lec_info);
	}
    }

    return(FSM_OK);
}


/*
 * a_process_cfg_rsp
 */
ushort a_process_cfg_rsp (lec_info_t *lec_info)
{
    lane_ctl_msg_u *msg;
    hwidbtype      *hwptr = lec_info->swidb->hwptr;
    srb_vector_table_t *srbV_p;
  
    if (!lec_info) {
     return(FSM_RC_DATA_ERROR);
    }
  
    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_CFG_RSP",
		     lec_info->swidb->namestring);

    /*
     * Stop the CFG RSP timer.
     */
    lec_stop_timer(lec_info);

    /*
     * Teardown the LECS SVC.  We don't need it anymore.
     */
    lec_send_release(&lec_info->configure, ATM_NORMAL_UNSPECIFIED);
  
    /*
     * Save the information returned from the LECS in the config response.
     */
    msg = (lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart;
    if (msg) {
  
	lec_info->actual_lan_type 		= msg->cfg.lan_type;
        lec_info->actual_max_frame_size		= msg->cfg.max_frame_size;
	lec_info->actual_name_size		= msg->cfg.name_size;
	bcopy(msg->cfg.elan_name, lec_info->actual_elan_name, msg->cfg.name_size);

	lec_info->elan_rn = 0;
        if ((lec_info->lec_is_srb) &&
	    (msg->cfg.lan_type == LANE_TYPE_802_5) &&
	    (msg->cfg.num_tlvs > 0)) {

	    if (msg->cfg.tlvs[3] == LANE_LOCAL_SEG_TLV_ID) {
		lec_info->elan_rn = GETSHORT(&msg->cfg.tlvs[5]);
	    }
 
            srbV_p = reg_invoke_srb_get_vector_table();
            if (srbV_p) {
	        if (srbV_p->srb_sb_get_thisring(lec_info->swidb) == 0) {

		    srbV_p->srb_sb_set_thisring(lec_info->swidb, 
						lec_info->elan_rn);
		    reg_invoke_tr_lane_add_vring_interface(hwptr,
		    					   lec_info->swidb,
		       					   lec_info->elan_rn,
							   lec_info->srb_bn,
							   lec_info->vr_rn);
	        }
	    }
	}
  
	lec_make_hwaddr(&lec_info->ctl_direct.nsap,
			msg->cfg.target_atm_addr,
			STATION_ATMNSAP,
			STATIONLEN_ATMNSAP);

	if (!lec_info->ctl_direct.svc) {
	    lec_info->ctl_direct.svc = TRUE;
	}
  
    }

    return(FSM_OK);
}



/*
 * a_resend_reg_req
 *
 * This action function resends the register request upon the REGISTER
 * timer popping.  Since the LANE spec doesn't specifically say how many
 * attempts to make, we use the control retries count.
 */
ushort a_resend_reg_req (lec_info_t *lec_info)
{

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_RESEND_REG_REQ",
           lec_info->swidb->namestring);

    lec_stop_timer(lec_info);

    /* sanity check */
    if (lec_info->lec_is_srb) {
	if (lec_info->control_retrys-- > 0) {
	    lec_send_reg_req(lec_info);
	} else {
	    return(a_teardown_lec(lec_info));
	}
    } else {  /* ethernet ? */
	a_send_bus_arp(lec_info);
    }
    return(FSM_OK);
}



/*
 * a_process_reg_rsp
 * we received a register response from LES
 */
ushort a_process_reg_rsp (lec_info_t *lec_info)
{
    lane_ctl_msg_u *msg;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_REG_RSP",
           lec_info->swidb->namestring);

    /*
     * Stop the REG REQ timer.
     */
    lec_stop_timer(lec_info);

    msg = (lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart;
    if (msg) {
	switch(msg->reg.hdr.status) {
	case LANE_STATUS_SUCCESS:
	    /*
             * sanity check
             */
	    if (lec_info->actual_lan_type == LANE_TYPE_802_5) {
                lec_info->registered_w_les = TRUE;
	    }

	    LEC_STATE_DEBUG("\nDEBUG received reg rsp.  Send bus arp");
	    lec_info->state = LEC_ST_GET_BUS_ADDR;
	    a_send_bus_arp(lec_info);
	    break;

	    /* 
	     * sanity check - shouldn't be in this state unless I got
	     * a SUCCESSFUL response 
	     */
	case LANE_STATUS_DUP_LAN_DEST:
	case LANE_STATUS_DUP_ATM_ADDR:
	case LANE_STATUS_INVALID_LAN_DEST:
	case LANE_STATUS_INVALID_ATM_ADDR:
	case LANE_STATUS_INVALID_REQ_LECID:
	default:
	    a_resend_reg_req(lec_info);
	}
    }
    return(FSM_OK);
}


/*
 * a_process_connect
 *
 * Processes incoming data connect.  This had better be a data
 * connect, else we drop it.
 */
ushort a_process_connect (lec_info_t *lec_info)
{
    lec_nsap_info_t *nsap_info;
    sig_api_msg	*msg;
    lec_vc_t    released_vc;
    lane_vc_type_t  vc_type;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_CONNECT",
		     lec_info->swidb->namestring);

    /*
     * Look up the data direct nsap struct based on incoming call_id.
     * Assign the newly allocated vcd, and send back a ready_ind
     * message to the called party.
     *
     * Should also check blli.
     */
    msg = lec_info->data.sig_msg;
    if (msg) {
       switch(lec_info->lane_info->type) {
       case LANE_TYPE_802_3:
          vc_type = LANE_LEC_DATA;
          break;
       case LANE_TYPE_802_5:
          vc_type = LANE_LEC_TR_DATA;
          break;
       default:
          vc_type = LANE_LEC_DATA;
       }
       if (msg->u.connect.blli.length &&
	   !lane_sig_blli_match(1, &msg->u.connect.blli,
				vc_type)) {
	   LEC_STATE_DEBUG("\nLEC %s: wrong blli in connect, teardown vc",
			   lec_info->swidb->namestring);
	   released_vc.swidb       = msg->idb;
	   released_vc.call_id     = msg->u.setup.call_id;
	   lec_send_release(&released_vc,
			    ATM_NORMAL_UNSPECIFIED);
	   return(FSM_OK);
       }

	nsap_info = lec_lookup_callid(lec_info, msg->u.connect.call_id);
	if (nsap_info) {
	    nsap_info->vc.vcd = msg->u.connect.vcd;
	    nsap_info->vc.call_state = CALL_STATE_UP;
	    nsap_info->ready_state = CALL_READY;
 	    reg_invoke_lec_bind_vlan_to_vc(lec_info, msg->u.connect.vcd);
	    lec_send_ready_ind(lec_info, msg->u.connect.vcd);
	    /*
	     * At this point, we need to initiate a flush, if bridging is
	     * configured on this interface. Otherwise, move the MAC state
	     * to LEC_MAC_DIRECT. Should be done for all the MACs that have
	     * been mapped to this NSAP address. The function will take
	     * care of enqueuing the information necessary to initiate the
	     * flush process
	     */
	    lec_flush_enqueue(lec_info, nsap_info);
	    
	} else if (lec_info->glare_list) {
	    nsap_info = lec_lookup_glare(lec_info, msg->u.connect.call_id);
	    if (nsap_info) {
		lec_send_release(&nsap_info->vc, ATM_NORMAL_UNSPECIFIED);
	    }
	}
    }

    return(FSM_OK);
}


/*
 * a_process_connect_ack
 *
 * The connection is set to CALL_STATE_UP at this point only if it 
 * is a Control VC.  We toss this bogus, useless, ragamuffin of a 
 * message if it is for a Data VC and  wait for the READY_IND before 
 * setting the connection to CALL_STATE_UP.
 *
 * The UNI CONNECT_ACK is generated by the network, therefore there is no
 * guarantee that the channel is ready for data to be sent when we
 * receive it.  Thus LANE defined the READY QUERY/IND message protocol.
 * When we recieve the READY_IND, the data connection is set to CALL_STATE_UP.
 *
 * Having said that it's a useless message, it turns out that the
 * Synergy box can't have reg_invoke_aalsap_map_bus_vcds called until
 * the VC is CONNECTed at the driver.  So we have to wait until the
 * CONNECT_ACK to map the bus VC.
 */
ushort a_process_connect_ack (lec_info_t *lec_info)
{
    lec_vc_t *control_vc;
    sig_api_msg *msg;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_CONNECT_ACK",
		     lec_info->swidb->namestring);

    /*
     * If the callid is not associated with a control VC, just drop
     * the message.  It's not needed.
     */
    msg = lec_info->data.sig_msg;
    if (msg) {
	control_vc = lec_find_control(lec_info, msg->u.connect_ack.call_id);
	if (control_vc) {
	    control_vc->call_state = CALL_STATE_UP;
 	    if (lec_info->mcast_send.call_state == CALL_STATE_UP &&
 		lec_info->mcast_forward.call_state == CALL_STATE_UP) {
 		reg_invoke_lec_map_bus_vcds(lec_info); 
 	    }
	}
    }

    return(FSM_OK);
}


/*
 * a_process_ctl_dist_setup
 *
 * After receiving the setup for the LES control distribute, send the
 * connect and wait for the join.
 */
ushort a_process_ctl_dist_setup (lec_info_t *lec_info)
{
    sig_api_msg	*msg;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_CTL_DIST_SETUP",
		     lec_info->swidb->namestring);

    /*
     * Check the information coming in on the setup.
     */
    msg = lec_info->data.sig_msg;
    if (msg) {
	lec_info->ctl_distribute.call_id = msg->u.setup.call_id;
	lec_info->ctl_distribute.vcd = msg->u.setup.vcd;

	/*
	 * Make sure that the setup is from LES and with correct blli.
	 */
	if (!lane_addr_equal(&lec_info->ctl_direct.nsap,
			     &msg->u.setup.calling_party)) {
	    LEC_STATE_DEBUG("\nLEC %s: wrong calling party, teardown LEC",
			    lec_info->swidb->namestring);
	    lec_fail_reason = LEC_FAIL_ORIG_DISTRIBUTE;
	    return(a_teardown_lec(lec_info));
	}

	if (!lane_sig_blli_match(ATM_MAX_BLLI_NUM, &msg->u.setup.blli[0],
				 lec_info->ctl_distribute.lane_vc_type)) {
	    LEC_STATE_DEBUG("\nLEC %s: wrong blli, teardown LEC",
			    lec_info->swidb->namestring);
	    lec_fail_reason = LEC_FAIL_BLLI_DISTRIBUTE;
	    return(a_teardown_lec(lec_info));
	}
 
	bcopy(&msg->u.setup.calling_party,
	      &lec_info->ctl_distribute.nsap,
	      sizeof(hwaddrtype));
	lec_send_connect(&lec_info->ctl_distribute);
    }
    
    return(FSM_OK);
}

/*
 * a_process_ctl_dist_setup_send_reg_req
 *
 * We've received the setup of the control distribute and already have
 * the join response.  Check the setup parms and send a Register to the
 * LES for all unregistered LAN Destinations.
 * In the case of 802.3 LEC, there is nothing else to register so send
 * registration complete
 */
ushort a_process_ctl_dist_setup_send_reg_req (lec_info_t *lec_info)
{
    sig_api_msg	*msg;

    if (!lec_info) {
     return(FSM_RC_DATA_ERROR);
    }
  
    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_CTL_DIST_SETUP_SEND_BUS_ARP",
		     lec_info->swidb->namestring);
  
    /*
     * Check the information coming in on the setup.
     */
    msg = lec_info->data.sig_msg;
    if (msg) {
	lec_info->ctl_distribute.call_id = msg->u.setup.call_id;
	lec_info->ctl_distribute.vcd = msg->u.setup.vcd;
 
	bcopy(&msg->u.setup.calling_party,
        &lec_info->ctl_distribute.nsap,
  	      sizeof(hwaddrtype));
        /*
         * Need to check msg->u.setup.blli[0] here...
         */
        if (!lane_sig_blli_match(ATM_MAX_BLLI_NUM, msg->u.setup.blli,
				 LANE_LEC_DISTRIBUTE)) {
	    errmsg(&msgsym(LEC_BAD_CTL_DIST_TYPE, LANE),
		   lec_info->swidb->namestring);
	} else {
	    lec_send_connect(&lec_info->ctl_distribute);
	}
    }
 
    /*
     * if this is an 802.5 LEC, then register the route
     * descriptor (we only have 1) with the LES.
     */
    if (lec_info->lec_is_srb) {
        lec_send_reg_req(lec_info);
        lec_info->state = LEC_ST_REGISTRATION;
    } else {
    /*
     * else this is an 802.3 LEC, nothing to register so send
     * the arp for the BUS.
     */
	lec_start_timer(lec_info,
			lec_info->C20_arp_response_time,
  			LEC_TIMER_BUS_ARP);
 	lec_send_arp_req(lec_info, baddr, lec_info->normalMac);
        lec_info->state = LEC_ST_GET_BUS_ADDR;
    }
      
    return(FSM_OK);
}
  


/*
 * a_process_failed_listen
 *
 * In the event that the listen fails, we emit a cute errmsg.
 */
ushort a_process_failed_listen (lec_info_t *lec_info)
{
    uchar addr[ATM_MAX_NSAP_STRING];

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }
    
    atm_printnsap(lec_info->nsap.addr, addr);
    
    errmsg(&msgsym(LEC_LISTEN, LANE), 
	   lec_info->swidb->namestring,
	   addr);

    return(FSM_OK);
}


/*
 * a_process_idle_rel_comp
 *
 * Handle incoming release complete while in the IDLE state.
 */
ushort a_process_idle_rel_comp (lec_info_t *lec_info)
{
    sig_api_msg *msg;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_IDLE_REL_COMP",
		     lec_info->swidb->namestring);

    msg = lec_info->data.sig_msg;
    if (msg) {
	lec_vc_t	*control_vc;
	lec_nsap_info_t	*nsap_info;

	/*
	 * Check to see if this is a control VC coming down.
	 */
	control_vc = lec_find_control(lec_info, 
				      msg->u.release.call_id);
	if (control_vc) {
	    lec_clear_vc(lec_info, control_vc);
	    return(FSM_OK);
	}

	/*
	 * If this is a data VC, just make sure that the nsap entry is
	 * taken out of the table.  Also take out all le_arp entries
	 * that are using the vc associated with the nsap.
	 */
	nsap_info = lec_lookup_callid(lec_info,
				      msg->u.release_comp.call_id);
	if (nsap_info) {
	    lec_delete_le_arp_nsap(lec_info, nsap_info);
	    lec_delete_nsap_entry(lec_info, nsap_info);
	}
    }
	
    return (FSM_OK);
}

/*
 * a_process_join_rsp_send_req
 *
 * We've received the JOIN rsp over the control direct and are
 * ready to register any lan destinations that have not been registered
 * In the case of 802.3, no additional registration is needed; therefore,
 * send bus_arp request and transition to BUS_ADDR.
 */
ushort a_process_join_rsp_send_req (lec_info_t *lec_info)
{
    lane_ctl_msg_u *msg;
    
    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_JOIN_RSP_SEND_REQ",
           lec_info->swidb->namestring);
    /*
     * stop the JOIN timeout.
     */
    lec_stop_timer(lec_info);

    /*
     * if we are handling an incoming join message, save the
     * result and send the register request
     */
    if (lec_info->data.control_pak) {
	msg = (lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart;
	/* 
	 * If receiving a different max_data_frame_size back,
	 * we treat this an error situation.
	 */
	if (lec_info->lane_info->max_data_frame != 
	    msg->join.max_frame_size) {
	    LEC_STATE_DEBUG("\nLEC %s: receive different frame_size (%d),"
			    "teardown LEC",
			    lec_info->swidb->namestring,
			    msg->join.max_frame_size);
	    LEC_STATE_DEBUG("\nLEC %s: change the mtu size to match"
			    " the one for LES",
			    lec_info->swidb->namestring);

	    lec_fail_reason = LEC_FAIL_FRAME_SIZE_MISMATCH;
	    return(a_teardown_lec(lec_info));
	}

	lec_save_join_resp_params(lec_info,
		(lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart);
     }

     /*
      * if the LES sent us an lecid of 0, complain.
      */
     if (!lec_info->lecid) {
	 return(FSM_RC_INVALID_INPUT);
     }

     /*
      * if this is an 802.5 LEC, then register the route
      * descriptor (we only have 1) with the LES.
      * Spec doesn't specify timeout for registration specifically
      * so we'll use the c7 control timeout
      */
     if ((lec_info->lec_is_srb) && (!lec_info->registered_w_les)) {
	 LEC_STATE_DEBUG("\nDEBUG send registration request");
	 lec_send_reg_req(lec_info);
	 lec_info->state = LEC_ST_REGISTRATION;
     } else {
	 /*
	  * for 802.5 that's already registered everything or
	  * for 802.3, send the bus arp request and skip to the
	  * bus connect state.
	  */
	 lec_send_arp_req(lec_info, baddr, lec_info->normalMac);
	 lec_start_timer(lec_info,
			 lec_info->C20_arp_response_time,
			 LEC_TIMER_BUS_ARP);
	 lec_info->state = LEC_ST_GET_BUS_ADDR;
     }
     return(FSM_OK);
}


/*
 * a_process_ready_ind
 *
 * Valid ONLY for data VCs.  Performs a lookup in the nsap cache for
 * the callid, and if found, resets timer C28 on the nsap entry, and
 * sets the nsap ready state to CALL_READY.
 */
ushort a_process_ready_ind (lec_info_t *lec_info)
{
    lec_nsap_info_t	*nsap_info;
    paktype		*pak;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_READY_IND",
		     lec_info->swidb->namestring);

    pak = lec_info->data.control_pak;
    if (pak) {
	nsap_info = lec_lookup_vcd(lec_info, pak->vcd);
	if (nsap_info) {
	    nsap_info->ready_state = CALL_READY;
	    lec_flush_enqueue(lec_info, nsap_info);
	    mgd_timer_stop(&nsap_info->ready_timer);
 	    reg_invoke_lec_bind_vlan_to_vc(lec_info, pak->vcd);
	}
    }

    return(FSM_OK);
}


/*
 * a_process_ready_query
 *
 * This is simple.  Whenever the LEC receives a READY_QUERY, it
 * returns a READY_IND.  Set the nsap_info->ready flag just for good
 * measure.  It shouldn't be necessary, since we should already
 * sent the READY_IND back when the connection first came up.
 */
ushort a_process_ready_query (lec_info_t *lec_info)
{
    lec_nsap_info_t	*nsap_info;
    paktype		*pak;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_READY_QUERY",
		     lec_info->swidb->namestring);

    pak = lec_info->data.control_pak;
    if (pak) {
	nsap_info = lec_lookup_vcd(lec_info, pak->vcd);
	if (nsap_info) {
	    nsap_info->ready_state = CALL_READY;
 	    reg_invoke_lec_bind_vlan_to_vc(lec_info, pak->vcd);
	    lec_send_ready_ind(lec_info, pak->vcd);
	}
    }

    return(FSM_OK);
}


/*
 * a_process_ready_timer
 *
 * If a called LEC holds a data direct VCC for more than C28_DEFAULT
 * (4 sec) without receiving a READY_IND, it will timeout and send a
 * READY_QUERY.  It will retry C13_DEFAULT (1) times, at which time
 * the VCC will be released.
 */
ushort a_process_ready_timer (lec_info_t *lec_info)
{
    lec_nsap_info_t	*nsap_info;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_READY_TIMER",
		     lec_info->swidb->namestring);

    nsap_info = lec_info->data.nsap_entry;
    if (nsap_info) {
	if (nsap_info->ready_retry_count--) {
	    lec_send_ready_query(lec_info, nsap_info->vc.vcd);
	    mgd_timer_start(&nsap_info->ready_timer, 
			    lec_info->C28_connect_comp_time);
	} else {
	    mgd_timer_stop(&nsap_info->ready_timer);
	    lec_send_release(&nsap_info->vc, ATM_NORMAL_UNSPECIFIED);
	}
    }

    return(FSM_OK);
}


/*
 * a_process_release
 *
 * Checks to see if the RELEASE is coming from a control VC or data VC
 * and takes appropriate action.
 */
ushort a_process_release (lec_info_t *lec_info)
{
    sig_api_msg *msg;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_RELEASE",
		     lec_info->swidb->namestring);

    msg = lec_info->data.sig_msg;
    if (msg) {
		lec_vc_t	*control_vc;
		lec_vc_t	data_vc;
		lec_nsap_info_t	*nsap_info;

	/*
	 * Check to see if this is a control VC coming down.  If so,
	 * we teardown the entire LEC.
	 */
	control_vc = lec_find_control(lec_info, 
				      msg->u.release.call_id);
	if (control_vc) {
	    lec_fail_reason = LEC_FAIL_CTRL_VC_RELEASED;
	    return(a_teardown_lec(lec_info));
	}

	/*
	 * If this is a data VC going down, find the nsap entry, send
	 * the release complete and delete the entry.  If an entry
	 * wasn't found in the nsap cache, check the glare queue and
	 * delete the entry.  If still no entry, just send a release 
	 * complete to free up signaling resources.  This is just
	 * belts and suspenders.
	 * 
	 * Also, when deleting the nsap entry, delete all le_arp
	 * entries that are associated with the nsap.
	 */
	nsap_info = lec_lookup_callid(lec_info,
				      msg->u.release.call_id);
	if (nsap_info) {
	    lec_delete_le_arp_nsap(lec_info, nsap_info);
	    lec_send_release_comp(&nsap_info->vc, ATM_NORMAL_UNSPECIFIED);
	    lec_delete_nsap_entry(lec_info, nsap_info);
	} else if (lec_info->glare_list) {
	    nsap_info = lec_lookup_glare(lec_info,
					 msg->u.release.call_id);
	    if (nsap_info) {
		lec_send_release_comp(&nsap_info->vc,
				      ATM_NORMAL_UNSPECIFIED);
		lec_delete_glare_entry(lec_info, nsap_info);
	    }
	} else {
	    data_vc.swidb = msg->idb;
	    data_vc.call_id = msg->u.release.call_id;
	    lec_send_release_comp(&data_vc, ATM_NORMAL_UNSPECIFIED);
	}
    }
	
    return (FSM_OK);
}


/*
 * a_process_rel_comp
 *
 * Check to see if this is a control vc going down.  If so, tear the
 * whole LEC down.  Otherwise, find the nsap info struct and
 * delete it from the hash table.
 */
ushort a_process_rel_comp (lec_info_t *lec_info)
{
    sig_api_msg *msg;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_REL_COMP",
		     lec_info->swidb->namestring);

    msg = lec_info->data.sig_msg;
    if (msg) {
	lec_vc_t	*control_vc;
	lec_nsap_info_t	*nsap_info;

	/*
	 * If this is a control VC coming down, teardown the whole
	 * LEC.
	 */
	control_vc = lec_find_control(lec_info, 
				      msg->u.release_comp.call_id);
	if (control_vc) {
	    lec_fail_reason = LEC_FAIL_CTRL_VC_RELEASED;
	    return(a_teardown_lec(lec_info));
	}

	/*
	 * If this is a data VC, just make sure that the nsap entry is
	 * taken out of the table.  Also take out all le_arp entries
	 * that are using the vc associated with the nsap.
	 */
	nsap_info = lec_lookup_callid(lec_info,
				      msg->u.release_comp.call_id);
	if (nsap_info) {
	    lec_delete_le_arp_nsap(lec_info, nsap_info);
	    lec_delete_nsap_entry(lec_info, nsap_info);
	    lec_info->lecSVCFailures++;
        } else if (lec_info->glare_list) {
	    nsap_info = lec_lookup_glare(lec_info,
					 msg->u.release_comp.call_id);
	    if (nsap_info) {
		lec_delete_glare_entry(lec_info, nsap_info);
	    }
	}
    }
	
    return(FSM_OK);
}


/*
 * a_process_setup
 *
 * Handle an incoming SETUP request from signaling.
 */
ushort a_process_setup (lec_info_t *lec_info)
{
    int			result;
    lec_nsap_info_t	*nsap_info;
    lec_vc_t		released_vc;
    sig_api_msg		*msg;
    lane_vc_type_t   	vc_type;
    int			sdu_size;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_SETUP",
		     lec_info->swidb->namestring);

    msg = lec_info->data.sig_msg;
    if (msg) {
       switch(lec_info->lane_info->type) {
       case LANE_TYPE_802_3:
          vc_type = LANE_LEC_DATA;
          break;
       case LANE_TYPE_802_5:
          vc_type = LANE_LEC_TR_DATA;
          break;
       default:
          vc_type = LANE_LEC_DATA;
       }
	if (!lane_sig_blli_match(ATM_MAX_BLLI_NUM, &msg->u.setup.blli[0],
				 vc_type)) {
	    LEC_STATE_DEBUG("\nLEC %s: wrong blli in setup, send rel_comp",
			    lec_info->swidb->namestring);
	    released_vc.swidb       = msg->idb;
	    released_vc.call_id     = msg->u.setup.call_id;
	    lec_send_release_comp(&released_vc,
				  ATM_NORMAL_UNSPECIFIED);
 
	    return(FSM_OK);
	}

       /*
	* If there is a SDU size mis match, reject the call
	*/
       sdu_size = lane_frame_to_sdu_size(lec_info->lane_info->max_data_frame);
       if ((sdu_size != msg->u.setup.fwd_sdu_size) ||
	   (sdu_size != msg->u.setup.bwd_sdu_size)) {

	   if (msg->idb->hwptr->atm_db->mtu_reject_call) {
	       errmsg(&msgsym(LANE_SIG_INV_SDU, LANE), 
		      &msg->u.setup.calling_party,
		      msg->u.setup.fwd_sdu_size,
		      msg->u.setup.bwd_sdu_size,
		      lec_info->swidb->namestring, sdu_size);
	       released_vc.swidb       = msg->idb;
	       released_vc.call_id     = msg->u.setup.call_id;
	       lec_send_release_comp(&released_vc,
				     ATM_AAL_PARAMS_NOT_SUPP);
	       return(FSM_OK);
 
	   }
       }
	/*
	 * Lookup the incoming atm address in the nsap table.
	 * If it doesn't exist, create one.  If it does exist, check
	 * for glare.  The LE spec says that the lowest nsap address
	 * wins.  We only put a call on the glare_queue if we want 
	 * to take it down.  We only want to take it down
	 * if it is in CALL_STATE_PROCEEDING.  If it's already up, too
	 * bad.  We'll turn down the setup.
	 */
	nsap_info = lec_lookup_nsap(lec_info, &msg->u.setup.calling_party);
	if (!nsap_info) {
	    /*
	     * Need to check msg->u.setup.blli[0] here before creating
	     * the nsap entry.
	     */
	    nsap_info = lec_create_nsap_entry(lec_info, 
					      &msg->u.setup.calling_party);
	    if (nsap_info) {

		nsap_info->vc.vcd 		= msg->u.setup.vcd;
		nsap_info->vc.call_id 		= msg->u.setup.call_id;
		nsap_info->vc.svc		= TRUE;
		nsap_info->vc.call_state	= CALL_STATE_PROCEEDING;

		mgd_timer_start(&nsap_info->ready_timer, 
				lec_info->C28_connect_comp_time);
		lec_send_connect(&nsap_info->vc);
	    }
	} else {
	    switch (nsap_info->vc.call_state) {
	    case CALL_STATE_PROCEEDING:
		/*
		 * Compare the incoming nsap against our own.
		 * The numerically smaller nsap wins in glare
		 * conditions.  
		 */
		result = memcmp(lec_info->nsap.addr,
				nsap_info->vc.nsap.addr,
				STATIONLEN_ATMNSAP);
		if (result < 0) {
		    released_vc.swidb	= msg->idb;
		    released_vc.call_id	= msg->u.setup.call_id;
		    lec_send_release_comp(&released_vc,
					  ATM_NORMAL_UNSPECIFIED);
		} else if (result > 0) {
		    /*
		     * Swap connections...the incoming connection
		     * wins.  Allocate a glare VC, which will be taken
		     * down AFTER it comes up.  We do this to avoid 
		     * having to trip over network bugs
		     * when we chase a setup with an immediate release.
		     */
		    lec_create_glare_entry(lec_info, nsap_info);

		    nsap_info->vc.vcd 		= msg->u.setup.vcd;
		    nsap_info->vc.call_id 	= msg->u.setup.call_id;
		    nsap_info->vc.svc		= TRUE;
		    nsap_info->vc.call_state	= CALL_STATE_PROCEEDING;

		    lec_send_connect(&nsap_info->vc);
		} else {
		    /*
		     * We're setting up a loopback connect.  Pinging
		     * ourself.
		     */
		    nsap_info 
			= lec_create_nsap_entry(lec_info, 
						&msg->u.setup.calling_party);
		    if (nsap_info) {
			nsap_info->vc.vcd	= msg->u.setup.vcd;
			nsap_info->vc.call_id	= msg->u.setup.call_id;
			nsap_info->vc.svc	= TRUE;
			nsap_info->vc.call_state= CALL_STATE_PROCEEDING;

			mgd_timer_start(&nsap_info->ready_timer, 
					lec_info->C28_connect_comp_time);
			lec_send_connect(&nsap_info->vc);
		    }
		}
		break;
	    case CALL_STATE_UP:
		/*
		 * If there is already a connection
		 * up, the incoming connection loses.
		 */
		released_vc.swidb	= msg->idb;
		released_vc.call_id	= msg->u.setup.call_id;
		lec_send_release_comp(&released_vc, 
				      ATM_NORMAL_UNSPECIFIED);
		lec_info->lecSVCFailures++;
		break;
	    default:
		break;
	    }
	}
    }

    return(FSM_OK);
}


/*
 * a_process_term_release
 *
 * Handle incoming releases while in the TERMINATING state.
 */
ushort a_process_term_release (lec_info_t *lec_info)
{
    sig_api_msg *msg;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_TERM_RELEASE",
		     lec_info->swidb->namestring);

    msg = lec_info->data.sig_msg;
    if (msg) {
	lec_vc_t	*control_vc;
	lec_vc_t	data_vc;
	lec_nsap_info_t	*nsap_info;

	/*
	 * Check to see if this is a control VC coming down.
	 */
	control_vc = lec_find_control(lec_info, 
				      msg->u.release.call_id);
	if (control_vc) {
	    lec_send_release_comp(control_vc, ATM_NORMAL_UNSPECIFIED);
	    lec_check_idle(lec_info);
	    return(FSM_OK);
	}

	/*
	 * Check if we have this cached in the nsap table.  Even if we
	 * don't, send a gratuitous release_comp to clear up signaling.
	 */
	nsap_info = lec_lookup_callid(lec_info,
				      msg->u.release.call_id);
	if (nsap_info) {
	    lec_delete_le_arp_nsap(lec_info, nsap_info);
	    lec_send_release_comp(&nsap_info->vc, ATM_NORMAL_UNSPECIFIED);
	    lec_delete_nsap_entry(lec_info, nsap_info);
	} else {
	    data_vc.swidb = msg->idb;
	    data_vc.call_id = msg->u.release.call_id;
	    lec_send_release_comp(&data_vc, ATM_NORMAL_UNSPECIFIED);
	}
    }
	
    return (FSM_OK);
}


/*
 * a_process_term_rel_comp
 *
 * Handle incoming release complete while in the TERMINATING state.
 * Looks for control VCs first, and then any rogue data VCs.  Once all
 * control VCs are down, it transitions to the IDLE state.
 */
ushort a_process_term_rel_comp (lec_info_t *lec_info)
{
    sig_api_msg *msg;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_PROCESS_TERM_REL_COMP",
		     lec_info->swidb->namestring);

    msg = lec_info->data.sig_msg;
    if (msg) {
	lec_vc_t	*control_vc;
	lec_nsap_info_t	*nsap_info;

	/*
	 * Note that we shouldn't rely on the control plane only.  In
	 * the future, a count of SVCs in the nsap table will be kept
	 * that should also be checked before transitioning to the IDLE
	 * state.
	 */
	control_vc = lec_find_control(lec_info, 
				      msg->u.release_comp.call_id);
	if (control_vc) {
	    lec_clear_vc(lec_info, control_vc);
	    lec_check_idle(lec_info);
	    return(FSM_OK);
	}

	/*
	 * If this was a data VC going down, just delete the entry out
	 * of the nsap cache.
	 */
	nsap_info = lec_lookup_callid(lec_info,
				      msg->u.release_comp.call_id);
	if (nsap_info) {
	    lec_delete_le_arp_nsap(lec_info, nsap_info);
	    lec_delete_nsap_entry(lec_info, nsap_info);
	}

    }
	
    return(FSM_OK);
}


/*
 * a_process_topo_change
 *
 * Topology change messages are generate when bridging is enabled
 * whenever configuration BPDU is generated.  The change flag in the
 * topology change message will be set to the same value as the change
 * bit in the configuration BPDU.  When the change bit is set, all
 * non-local le-arp entries will be reset to use the forward delay
 * timer.
 */
ushort a_process_topo_change (lec_info_t *lec_info)
{
    int			index;
    lane_ctl_msg_u	*msg;
    lec_le_arp_t	*le_arp;
    sys_timestamp	next_time;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    msg = (lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart;
    if (msg) {

	/*
	 * Check the TC flag in the incoming msg.
	 */
	if (msg->topo.hdr.flags == LANE_PROT_FLAG_TOPO_CHANGE) {
	    /*
	     * If incoming TC is set, and our state is FALSE, change
	     * all of the le_arp timers to the forward delay time.
	     */
            if (!lec_info->C19_topology_change) {

		LEC_TOPO_DEBUG ("\nLEC %s: action A_PROCESS_TOPO_CHANGE, TC bit true",
				lec_info->swidb->namestring);

		lec_info->C19_topology_change = TRUE;

                /*
                 * topology_changed is set, the first time LEC gets a
                 * message from the spanning tree, as we want to update
                 * rx_sar only then.
                 */
                reg_invoke_lec_change_vcd_clbit(lec_info, FALSE);

		for (index = 0; index < LANE_NETHASHLEN; index++) {
		    if (lec_info->le_arp_table[index]) {
			for (le_arp = lec_info->le_arp_table[index];
			     le_arp;
			     le_arp = le_arp->next) {
			    if (le_arp->entry_type == LEC_LE_ARP_REMOTE) {
				TIMER_START(next_time, 
					    lec_info->C18_forward_delay_time);
				mgd_timer_set_soonest(&le_arp->arp_age_timer,
						      next_time);
			    }
			}
		    }
		}
            }
	} else {
	    if (lec_info->C19_topology_change) {

		lec_info->C19_topology_change = FALSE;

		LEC_TOPO_DEBUG ("\nLEC %s: action A_PROCESS_TOPO_CHANGE, TC bit false",
				lec_info->swidb->namestring);

                /*
                 * When the topology change bit is not set in the message,
                 * set topology changed to TRUE only for the first message.
                 * Then update rx_sar
                 */
                reg_invoke_lec_change_vcd_clbit(lec_info, TRUE);
            }
	}
    }

    return(FSM_OK);
}


/*
 * a_resend_cfg_req
 *
 * Resend the config request to the LECS after the control timeout.
 */
ushort a_resend_cfg_req (lec_info_t *lec_info)
{

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_RESEND_CFG_REQ",
		     lec_info->swidb->namestring);
    
    if (lec_info->control_retrys-- > 0) {
	lec_send_cfg_req(lec_info);
    } else {
	return(a_teardown_lec(lec_info));
    }

    return (FSM_OK);
}


/*
 * a_resend_join_req
 *
 * This action function resends the join request upon the JOIN timer
 * popping.  Since the LANE spec doesn't specifically say how many
 * attempts to make, we use the C13 maximum retry count.
 */
ushort a_resend_join_req (lec_info_t *lec_info)
{

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_RESEND_JOIN_REQ",
		     lec_info->swidb->namestring);

    lec_stop_timer(lec_info);

    if (lec_info->control_retrys-- > 0) {
	lec_send_join_req(lec_info);
    } else {
	return(a_teardown_lec(lec_info));
    }

    return (FSM_OK);
}


/*
 * a_send_arp_rsp
 *
 * Respond to an incoming LE-ARP request.
 * Note that as a result of HSRP implementation, 
 * the client can have MORE than one mac address.
 *
 */
ushort a_send_arp_rsp (lec_info_t *lec_info)
{
    lane_ctl_msg_u *msg;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_SEND_ARP_RSP",
		     lec_info->swidb->namestring);

    /*
     * Confirm that the target destination is for us, and send a
     * response.  Echo suppress these by checking the lecid.
     */
    msg = (lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart;
    if (msg) {
	if (lec_mac_valid(lec_info, msg->arp.target_dest.addr.mac)) {
	    lec_send_arp_rsp(lec_info, LEC_LOCAL_ADDR);
	}
 	/*
 	 * In the event that we are a Synergy box or bridge, send the
 	 * query to find out if we can bridge this MAC.  Note that the
 	 * Synergy call is non-blocking.  The answer will come back
 	 * asynchronously.  Make sure we echo suppress these requests.
	 *
	 * Also, don't respond to LE_ARPs if the port is blocked or
	 * disabled.
	 */

        if (lec_info->lecid != GETSHORT(&msg->hdr.requestor_lecid) &&
	    ((lec_info->C4_proxy) &&
              lec_info->swidb->port_state == PORT_FORWARDING)) {
             if (reg_invoke_lec_bridge_lookup_mac(lec_info)) {
	        lec_send_arp_rsp(lec_info, LEC_REMOTE_ADDR);
             }
	 }
    }

    return(FSM_OK);
}


/*
 * a_send_bus_arp
 */
ushort a_send_bus_arp (lec_info_t *lec_info)
{
    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_SEND_BUS_ARP",
		     lec_info->swidb->namestring);

    /*
     * Stop the JOIN timeout.
     */
    lec_stop_timer(lec_info);
    lec_send_arp_req(lec_info, baddr, lec_info->normalMac);

    lec_start_timer(lec_info,
		    lec_info->C20_arp_response_time,
		    LEC_TIMER_BUS_ARP);

    return(FSM_OK);
}


/*
 * a_send_bus_connect
 *
 * Respond to the BUS setup with a connect.
 */
ushort a_send_bus_connect (lec_info_t *lec_info)
{
    sig_api_msg	*msg;

    if (!lec_info) {
		return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_SEND_BUS_CONNECT",
		     lec_info->swidb->namestring);

    /*
     * Check the information coming in on the setup.
     */
    msg = lec_info->data.sig_msg;
    if (msg) {
	lec_info->mcast_forward.call_id = msg->u.setup.call_id;
	lec_info->mcast_forward.vcd = msg->u.setup.vcd;

	/*
	 * Make sure the setup is from BUS and with correct BLLI.
	 */
	if (!lane_addr_equal(&lec_info->mcast_send.nsap,
			     &msg->u.setup.calling_party)) {
	    LEC_STATE_DEBUG("\nLEC %s: wrong calling party, teardown LEC",
			    lec_info->swidb->namestring);
	    lec_fail_reason = LEC_FAIL_ORIG_FORWARD;
	    return(a_teardown_lec(lec_info));
	}
	 
	if (!lane_sig_blli_match(ATM_MAX_BLLI_NUM, &msg->u.setup.blli[0],
				 lec_info->mcast_forward.lane_vc_type)) {
	    LEC_STATE_DEBUG("\nLEC %s: wrong blli in SETUP, teardown LEC",
			    lec_info->swidb->namestring);
	    lec_fail_reason = LEC_FAIL_BLLI_FORWARD;
	    return(a_teardown_lec(lec_info));
	}

	bcopy(&msg->u.setup.calling_party,
	      &lec_info->mcast_forward.nsap,
	      sizeof(hwaddrtype));
	lec_send_connect(&lec_info->mcast_forward);
	/*
	 * This action function is used whenever a setup for the mcast
	 * forward comes in from the BUS.  This can happen before or
	 * after the mcast send is actually built up.  We only want to
	 * go ACTIVE when the mcast send is complete.
	 */
	if (lec_info->mcast_send.call_state == CALL_STATE_UP) {
	    a_set_lec_active(lec_info);
	}
    } else {
	return(FSM_RC_INVALID_INPUT);
    }

    return(FSM_OK);
}


/*
 * a_send_bus_setup
 *
 * This routine is called with the returned le-arp for the bus nsap.
 */
ushort a_send_bus_setup (lec_info_t *lec_info)
{
    lane_ctl_msg_u	*msg;

    if (!lec_info) {
		return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_SEND_BUS_SETUP",
		     lec_info->swidb->namestring);

    /*
     * Stop the BUS ARP timer.
     */
    lec_stop_timer(lec_info);

    /*
     * Stow away the target nsap returned in the bus arp message.
     * However, if a bus nsap has been locally configured, throw
     * the information away.
     */
    if (lec_info->data.control_pak &&
	!check_predicate(lec_info, PRED_MCAST_SEND_NSAP)) {
	msg = (lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart;
	lec_make_hwaddr(&lec_info->mcast_send.nsap,
			msg->arp.target_atm_addr,
			STATION_ATMNSAP,
			STATIONLEN_ATMNSAP);
    }

    /*
     * send the setup to open the mcast send SVC.
     */
    lec_info->mcast_send.svc = TRUE;
    lec_send_setup(lec_info, &lec_info->mcast_send);

    return(FSM_OK);
}


/*
 * a_send_cfg_req
 *
 * The LEC has received the CONNECT back from the LECS (or has the vc
 * hard configured).  Send the CFG request to the LECS.
 */
ushort a_send_cfg_req (lec_info_t *lec_info)
{
    sig_api_msg *msg;

    if (!lec_info) {
		return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_SEND_CFG_REQ",
		     lec_info->swidb->namestring);
    
    /*
     * check the blli to make sure this is what we are waiting for
     */
    msg = lec_info->data.sig_msg;
    if (msg) {
	if (msg->u.connect.blli.length &&
	    (!lane_sig_blli_match(1, &msg->u.connect.blli,
				  lec_info->configure.lane_vc_type))) {
	    LEC_STATE_DEBUG("\nLEC %s: wrong blli in connect, teardown LEC",
			    lec_info->swidb->namestring);

	    lec_fail_reason = LEC_FAIL_BLLI_CFG;
	    return(a_teardown_lec(lec_info));
	}
    }

    /*
     * sig_msg pointer is valid if there is an incoming connect
     * message that has to be handled.  This routine is also called
     * when the LEC_TIMER_CONFIG timer pops.  In either case, just
     * keep trying.
     */
    if (lec_info->data.sig_msg) {
	lec_info->configure.vcd = lec_info->data.sig_msg->u.connect.vcd;
	lec_info->configure.call_state = CALL_STATE_UP;
	lec_info->control_retrys = lec_info->C13_max_retry_count;
	lec_send_cfg_req(lec_info);
    }

    return (FSM_OK);
}


/*
 * a_send_flush_rsp
 *
 * Upon receipt of a flush request over either a data direct or mcast
 * forward, we return a flush response back over the control direct,
 * which gets forwarded to the sender over the control distribute.
 */
ushort a_send_flush_rsp (lec_info_t *lec_info)
{
    lane_ctl_msg_u *msg;

    if (!lec_info) {
		return(FSM_RC_DATA_ERROR);
    }


    /*
     * Only send the response if the flush request was addressed to us.
     */
    msg = (lane_ctl_msg_u *)lec_info->data.control_pak->datagramstart;
    if (msg) {
	if (!memcmp(msg->flush.target_atm_addr,
		    lec_info->nsap.addr,
		    STATIONLEN_ATMNSAP)) {
	    LEC_STATE_DEBUG ("\nLEC %s: action A_SEND_FLUSH_RSP",
			     lec_info->swidb->namestring);
	    lec_send_flush_rsp(lec_info);
	}
    }
    
    return(FSM_OK);
}


/*
 * a_send_join_req
 *
 * Now that the control direct VC is setup, send the JOIN REQ 
 * to the LES.
 */
ushort a_send_join_req (lec_info_t *lec_info)
{

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_SEND_JOIN_REQ",
		     lec_info->swidb->namestring);

    /*
     * The sig_msg pointer is valid if we are handling an incoming
     * connect for a SVC.  Just go ahead and send the join.
     */
    if (lec_info->data.sig_msg) {
 
	/*
	 * check whether the blli is correct
	 */
	if (lec_info->data.sig_msg->u.connect.blli.length &&
	    (!lane_sig_blli_match(1, &lec_info->data.sig_msg->u.connect.blli,
				  lec_info->ctl_direct.lane_vc_type))) {

	    LEC_STATE_DEBUG("\nLEC %s: wrong blli in connect, teardown LEC",
			    lec_info->swidb->namestring);

	    lec_fail_reason = LEC_FAIL_BLLI_DIRECT;
	    return(a_teardown_lec(lec_info));
	}
 
	/*
	 * Confirm that the incoming connect matches the callid that
	 * we sent on the setup.
	 */
	if (lec_info->ctl_direct.call_id ==
	    lec_info->data.sig_msg->u.connect.call_id) {
	    lec_info->ctl_direct.vcd = lec_info->data.sig_msg->u.connect.vcd;
	    lec_info->ctl_direct.call_state = CALL_STATE_UP;
	} else {
	    return (FSM_OK);
	}
    }

    lec_info->control_retrys = lec_info->C13_max_retry_count;
    lec_send_join_req(lec_info);

    return (FSM_OK);
}


/*
 * a_send_lecs_setup
 *
 * Received all of the positive ILMI GetNext Responses from the
 * switch.  Now pick one off of the head of the list and connect to
 * the LECS.  Note that we don't have to check the predicate vector
 * for a configured LECS NSAP, since we already checked before getting
 * to the GET_LECS_ADDR state.
 */
ushort a_send_lecs_setup (lec_info_t *lec_info)
{
    hwaddrtype addr;
    lec_lecs_addr_t *next;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_SEND_LECS_SETUP",
		     lec_info->swidb->namestring);

    /*
     * Add the default LECS atm address to the end of the queue.
     */
    addr = LANE_LECS_FIXED_NSAP;
    lec_add_lecs_addr(lec_info, &addr);
    
    /*
     * Pull an LECS address from the head of the queue.
     */
    bcopy(&lec_info->lecs_addrQ->atm_addr,
	  &lec_info->configure.nsap,
	  sizeof(hwaddrtype));

    next = lec_info->lecs_addrQ->next;
    free(lec_info->lecs_addrQ);
    lec_info->lecs_addrQ = next;

    /*
     * Send the setup request to the LECS.
     */
    if (lec_send_setup(lec_info, &lec_info->configure)) {
	return(FSM_OK);
    }

    return (FSM_OK);
}


/*
 * a_send_les_setup
 *
 * The LEC is in the GET_LES_ADDR state and just received the
 * release_complete from the LECS.  We assume that the predicate has
 * already been checked for the LES NSAP, so go ahead and set up the
 * control direct VC using the returned NSAP.
 */
ushort a_send_les_setup (lec_info_t *lec_info)
{
    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_SEND_LES_SETUP",
		     lec_info->swidb->namestring);

    /*
     * This had better be the LECS control vc coming down...
     */
    lec_clear_vc(lec_info, &lec_info->configure);

    lec_send_setup(lec_info, &lec_info->ctl_direct);

    return (FSM_OK);
}


/*
 * a_send_next_lecs_setup
 *
 * After sending the setup to the first lecs address in the list of
 * lecs atm addresses, the LEC keeps attempting until it either runs
 * out of addresses (in which case it goes back to idle) or it
 * successfully connects and sends the config request.
 */
ushort a_send_next_lecs_setup (lec_info_t *lec_info)
{
    lec_lecs_addr_t *next;
    sig_api_msg *msg;
    lec_vc_t    *control_vc;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_SEND_NEXT_LECS_SETUP",
		     lec_info->swidb->namestring);

    /*
     * Do the right thing with the release/release complete that we
     * just received. 
     */
    msg = lec_info->data.sig_msg;
    if (msg) {
	switch (msg->opcode) {
	case ATM_RELEASE:
	    control_vc = 
		lec_find_control(lec_info,
				 msg->u.release.call_id);
	    if (control_vc) {
		lec_send_release_comp(control_vc,
				      ATM_NORMAL_UNSPECIFIED);
	    }
	    break;
	case ATM_RELEASE_COMPLETE:
	    control_vc = 
		lec_find_control(lec_info,
				 msg->u.release_comp.call_id);
	    if (control_vc) {
		lec_clear_vc(lec_info, control_vc);
	    }
	    break;
	default:
	    break;
	}
    }

    /*
     * If no other LECSs are queued, teardown back to IDLE.
     */
    if (!lec_info->lecs_addrQ) {
	lec_fail_reason = LEC_FAIL_REL_CONFIG;
	return(a_teardown_lec(lec_info));
    }

    /*
     * Pull another LECS address from the head of the queue.
     */
    bcopy(&lec_info->lecs_addrQ->atm_addr,
	  &lec_info->configure.nsap,
	  sizeof(hwaddrtype));

    next = lec_info->lecs_addrQ->next;
    free(lec_info->lecs_addrQ);
    lec_info->lecs_addrQ = next;

    /*
     * Send the setup request to the LECS.
     */
    if (lec_send_setup(lec_info, &lec_info->configure)) {
	return(FSM_OK);
    }

    return (FSM_OK);
}


/*
 * a_register_addr
 *
 * Check the LEC predicate vector and determine if the state machine
 * is being overridden with configured options (e.g. NSAPs).  If the
 * ILMI is available, attempt to set the LEC ATM address into the
 * switch.
 */
ushort a_register_addr (lec_info_t *lec_info)
{
    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_REGISTER_ADDR",
		     lec_info->swidb->namestring);

    lec_stop_timer(lec_info);

    /*
     * If we're not administratively ACTIVE, just return to the
     * IDLE state.  We just received a set prefix from the ILMI.
     */
    if (lec_info->admin_status != LANE_ACTIVE) {
	return(FSM_OK);
    }

    lec_line_statechange(lec_info->swidb, 
			 SUBIF_STATE_ADMINDOWN);

    /*
     * Get fresh copies of the control nsaps...they can be changed out
     * from under us with config commands.
     */
    lec_load_config(lec_info);

    /*
     * Set the predicate vector to the current configured conditions.
     */
    lec_init_pred_vector(lec_info);

    lec_info->join_attempt_cnt++;

    /*
     * If no LEC ATM address has come out of config, or auto
     * assignment has failed to find an ATM Prefix, go back to IDLE
     * and wait for either the switch to set the prefix, or 
     * the user to reconfig and give us something to work with.
     */
    if (!check_predicate(lec_info, PRED_LEC_NSAP)) {
	return(FSM_OK);
    }	

    /*
     * Always attempt to register the LEC address via ILMI.
     *
     * If the ILMI is NOT active, then we'll get an error.
     * And then moan about it, of course.
     */
    if (lec_send_ilmi_reg_addr(lec_info) != ILMI_OK) {
	errmsg(&msgsym(NOREGILMI, LANE),
	       lec_info->swidb->namestring, "LEC", lec_info->nsap.addr);
    }

    lec_info->state = LEC_ST_REGISTER_ADDR;
    return(FSM_OK);

}


/*
 * a_shutdown_lec
 *
 * Permanently shutdown the LEC.  This is basically the same as the
 * a_teardown_lec, except that we go right back to IDLE, with NO idle
 * timer and no waithing for .  It is NEVER called as a result of
 * receiving an incoming RELEASE or RELEASE complete.  It's only 
 * called due to locally initiated events.
 */
ushort a_shutdown_lec (lec_info_t *lec_info)
{

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_SHUTDOWN_LEC",
		     lec_info->swidb->namestring);

    /*
     * Stop all outstanding LEC timers.
     */
    lec_stop_timer(lec_info);

    /*
     * Reset anything that matters in the lec_info.
     */
    lec_info->lecid		= 0;
    lec_info->admin_status	= LANE_INACTIVE;
    lec_info->oper_status	= LANE_INACTIVE;
    lec_info->done_first_idle_timeout = FALSE;

    /*
     * Shutdown the Synergy data path.
     */
    reg_invoke_lec_port_destroy(lec_info);

    /*  
     * Synergy: for the mac addresses learned from 
     * the mcast_forward vc are programmed in the 
     * cam table with the mcast_send VC since it is 
     * the connection for LEC to send packets to BUS. 
     */
    reg_invoke_bridge_delete_vc(lec_info,
				lec_info->mcast_send.vcd);

    errmsg(&msgsym(UPDOWN, LANE),
	   lec_info->swidb->namestring,
	   "elan",
	   lec_info->lane_info->name,
	   "LE Client", "down");

    lec_flush_le_arp_table(lec_info);
    lec_free_lecs_addrQ(lec_info);
    lec_release_all_vccs(lec_info);

    /*
     * Reset all ARP caches on this interface.  Note that we DON'T do
     * this when we flush the LE-ARP table.  It's OK to maintain
     * network to MAC address arp entries even though there is no VCC or
     * LE-ARP entry.  But when the interface shuts down, we have no
     * use for this information.
     */

    reg_invoke_ar_cache_clear(NULL);
    reg_invoke_arp_reload(lec_info->swidb);
    reg_invoke_reset_if(lec_info->swidb);

    /* Tell interface MIB that this MAC address is no longer valid for
     * receiving traffic.
     */

    reg_invoke_lecmib_deregister_rcv_addr(lec_info);

    /*
     * Finally, cancel the listen, and unregister the address.
     */
    lec_cancel_listen(lec_info);
    lec_send_ilmi_unreg_addr(lec_info);

    /*
     * Record when and why the failure occurs.
     */
    if (lec_fail_reason != LEC_FAIL_ACTIVATE) {	
	lec_info->last_fail_reason = lec_fail_reason;
    }
    
    /*
     * To check whether all the control VCs are down.
     * If yes, set the state to IDLE.
     */
    lec_check_idle(lec_info);

    return (FSM_OK);
}


/*
 * a_teardown_lec
 *
 * Generic teardown of the lec.  Check to see if there are any
 * SVCs on the control plane, and send releases.  Then go through the
 * entire nsap table and tear down all data direct SVCs.
 */
ushort a_teardown_lec (lec_info_t *lec_info)
{
    lec_vc_t	*control_vc = NULL;
    sig_api_msg *msg;

    if (!lec_info) {
	return(FSM_RC_DATA_ERROR);
    }

    LEC_STATE_DEBUG ("\nLEC %s: action A_TEARDOWN_LEC",
		     lec_info->swidb->namestring);

    /*
     * Stop any outstanding LEC timer.
     */
    lec_stop_timer(lec_info);

    /*
     * No need for this anymore.  Note that the state hardcoded to return out of
     * this action function is LEC_ST_TERMINATING, unless all of the
     * control VCCs are down.
     */
    lec_info->lecid		= 0;
    lec_info->oper_status	= LANE_INACTIVE;
    lec_info->state		= LEC_ST_TERMINATING;
    lec_info->registered_w_les  = FALSE;
    
    /* Tell interface MIB that this MAC address is no longer valid for
     * receiving traffic.
     */

    reg_invoke_lecmib_deregister_rcv_addr(lec_info);

    /*
     * Shutdown the Synergy data path.
     */
    reg_invoke_lec_port_destroy(lec_info);

    /* 
     * Synergy: for the mac addresses learned from
     * the mcast_forward vc are programmed in the
     * cam table with the mcast_send VC since it is 
     * the connection for LEC to send packets to BUS.
     */
    reg_invoke_bridge_delete_vc(lec_info,
				lec_info->mcast_send.vcd);

    lec_flush_le_arp_table(lec_info);

    /*
     * First check to see if we are tearing down the LEC due to
     * receipt of either a RELEASE or a RELEASE COMPLETE on one of the
     * control VCs.  If it's a RELEASE_COMPLETE, we don't acknowledge.
     */
    msg = lec_info->data.sig_msg;
    if (msg) {

	switch (msg->opcode) {
	case ATM_SETUP:
	    control_vc =
		lec_find_control(lec_info,
				 msg->u.setup.call_id);
	    if (control_vc) {
		lec_send_release_comp(control_vc,
				      ATM_NORMAL_UNSPECIFIED);
	    }
	    break;
	case ATM_RELEASE:
	    control_vc = 
		lec_find_control(lec_info,
				 msg->u.release.call_id);
	    if (control_vc) {
		lec_send_release_comp(control_vc,
				      ATM_NORMAL_UNSPECIFIED);
	    }
	    break;
	case ATM_RELEASE_COMPLETE:
	    control_vc = 
		lec_find_control(lec_info,
				 msg->u.release_comp.call_id);
	    if (control_vc) {
		lec_clear_vc(lec_info, control_vc);
	    }
	    break;
	default:
	    break;
	}
    }

    lec_free_lecs_addrQ(lec_info);
    lec_release_all_vccs(lec_info);
    lec_cancel_listen(lec_info);
    lec_send_ilmi_unreg_addr(lec_info);

    /*
     * Record when and why the failure occurs.
     */
    if (lec_fail_reason != LEC_FAIL_ACTIVATE) {
	if (lec_fail_reason == LEC_FAIL_CTRL_VC_RELEASED && control_vc)
	    lec_fail_reason = lec_get_ctrl_vc_fail(control_vc->lane_vc_type);
	lec_info->last_fail_reason = lec_fail_reason;
    }

    /*
     * This will set the state to idle if all control VCCs are down.
     */
    lec_check_idle(lec_info);

    /*
     * Change the line state to down
     */
    lec_line_statechange(lec_info->swidb, 
			 SUBIF_STATE_ADMINDOWN);
        
    return (FSM_OK);
}


/*
 * lec_prstate
 *
 * Returns the state string given the LEC state variable.
 */
static char *lec_prstate (int state)
{
    switch (state) {
    case LEC_ST_IDLE:			return("IDLE");
    case LEC_ST_REGISTER_ADDR: 	 	return("REGISTER_ADDR");
    case LEC_ST_POSTING_LISTEN:		return("POSTING_LISTEN");
    case LEC_ST_GET_LECS_ADDR:       	return("GET_LECS_ADDR");
    case LEC_ST_LECS_CONNECT:       	return("LECS_CONNECT");
    case LEC_ST_GET_LES_ADDR:      	return("GET_LES_ADDR");
    case LEC_ST_LECS_RELEASE:		return("LECS_RELEASE");
    case LEC_ST_CTL_DIRECT_CONN:    	return("CTL_DIRECT_CONN");
    case LEC_ST_JOIN_CTL_DIST_CONN:     return("JOIN_CTL_DIST_CONN");
    case LEC_ST_CTL_DIST_CONN:        	return("CTL_DIST_CONN");
    case LEC_ST_JOIN:        		return("JOIN");
    case LEC_ST_REGISTRATION:    return("REGISTRATION");
    case LEC_ST_GET_BUS_ADDR:        	return("GET_BUS_ADDR");
    case LEC_ST_MCAST_SEND_FORWARD_CONN: return("MCAST_SEND_FORWARD_CONN");
    case LEC_ST_MCAST_SEND_CONN:        return("MCAST_SEND_CONN");
    case LEC_ST_MCAST_FORWARD_CONN:	return("MCAST_FORWARD_CONN");
    case LEC_ST_ACTIVE:			return("ACTIVE");
    case LEC_ST_TERMINATING:		return("TERMINATING");
    default:     			return("UNKNOWN");
    }
}


/*
 * lec_pr_atmf_state
 *
 * Returns the "official" ATM Forum version of the LEC protocol state.
 * Choices are:
 *
 *       initialState (1)
 *       lecsConnect (2)
 *       configure (3)
 *       join (4)
 *       initialRegistration (5)
 *       busConnect (6)
 *       operational (7)
 */
char* lec_pr_atmf_state (int state) {

    switch (state) {
    case LEC_ST_IDLE:			return("initialState");
    case LEC_ST_REGISTER_ADDR:		return("waitingIlmi");
    case LEC_ST_POSTING_LISTEN:		return("waitingListen");
    case LEC_ST_GET_LECS_ADDR:
    case LEC_ST_LECS_CONNECT:		return("lecsConnect");
    case LEC_ST_GET_LES_ADDR:
    case LEC_ST_LECS_RELEASE:		return("configure");
    case LEC_ST_CTL_DIRECT_CONN:
    case LEC_ST_JOIN_CTL_DIST_CONN:
    case LEC_ST_CTL_DIST_CONN:
    case LEC_ST_JOIN:			return("join");
    case LEC_ST_REGISTRATION:		return("initialRegistration");
    case LEC_ST_GET_BUS_ADDR:
    case LEC_ST_MCAST_SEND_FORWARD_CONN:
    case LEC_ST_MCAST_SEND_CONN:
    case LEC_ST_MCAST_FORWARD_CONN:	return("busConnect");
    case LEC_ST_ACTIVE:			return("operational");
    case LEC_ST_TERMINATING:		return("terminating");
    default:				return("unknown");
    }
}


/*
 * lec_prinput
 *
 * Given the major and minor elements that make up an event, return
 * the character string that represents it.
 */
static char *lec_prinput (int major, int minor)
{
    switch (major) {
    case LEC_MAJOR_LOCAL:
        switch (minor) {
	case LEC_LOCAL_PREFIX_SET:	return("LEC_LOCAL_PREFIX_SET");
	case LEC_LOCAL_CONFIG_CHANGE:	return("LEC_LOCAL_CONFIG_CHANGE");
	case LEC_LOCAL_ACTIVATE:	return("LEC_LOCAL_ACTIVATE");
	case LEC_LOCAL_DEACTIVATE:	return("LEC_LOCAL_DEACTIVATE");
	default:			return("UNKNOWN");
        }
    case LEC_MAJOR_CONTROL:
        switch (minor) {
	case LEC_CTL_ILMI_SET_RSP_POS:	return("LEC_CTL_ILMI_SET_RSP_POS");
	case LEC_CTL_ILMI_SET_RSP_NEG:	return("LEC_CTL_ILMI_SET_RSP_NEG");
	case LEC_CTL_CONFIG_RSP_POS:	return("LEC_CTL_CONFIG_RSP_POS");
	case LEC_CTL_CONFIG_RSP_NEG:	return("LEC_CTL_CONFIG_RSP_NEG");
	case LEC_CTL_JOIN_RSP_POS:    	return("LEC_CTL_JOIN_RSP_POS");
	case LEC_CTL_JOIN_RSP_NEG:    	return("LEC_CTL_JOIN_RSP_NEG");
	case LEC_CTL_ARP_REQ:    	return("LEC_CTL_ARP_REQ");
	case LEC_CTL_ARP_RSP:    	return("LEC_CTL_ARP_RSP");
	case LEC_CTL_NARP_REQ:    	return("LEC_CTL_NARP_REQ");
	case LEC_CTL_READY_QUERY:    	return("LEC_CTL_READY_QUERY");
	case LEC_CTL_READY_IND:    	return("LEC_CTL_READY_IND");
	case LEC_CTL_FLUSH_REQ:    	return("LEC_CTL_FLUSH_REQ");
	case LEC_CTL_FLUSH_RSP:    	return("LEC_CTL_FLUSH_RSP");
	case LEC_CTL_TOPO_CHANGE:    	return("LEC_CTL_TOPO_CHANGE");
	default:			return("UNKNOWN");
        }
    case LEC_MAJOR_SIGNALLING:
        switch (minor) {
	case LEC_SIG_SETUP:    		return("LEC_SIG_SETUP");
	case LEC_SIG_CONNECT:	    	return("LEC_SIG_CONNECT");
	case LEC_SIG_CONNECT_ACK:    	return("LEC_SIG_CONNECT_ACK");
	case LEC_SIG_RELEASE:    	return("LEC_SIG_RELEASE");
	case LEC_SIG_RELEASE_COMP:    	return("LEC_SIG_RELEASE_COMP");
	case LEC_SIG_ADD_PARTY:    	return("LEC_SIG_ADD_PARTY");
	case LEC_SIG_DROP_PARTY:    	return("LEC_SIG_DROP_PARTY");
	case LEC_SIG_LISTEN_POS:	return("LEC_SIG_LISTEN_POS");
        case LEC_SIG_LISTEN_NEG:	return("LEC_SIG_LISTEN_NEG");
	case LEC_SIG_CANCEL:		return("LEC_SIG_CANCEL");
	default:           		return("UNKNOWN");
        }
    case LEC_MAJOR_TIMER:
        switch (minor) {
	case LEC_TIMER_IDLE:		return("LEC_TIMER_IDLE");
	case LEC_TIMER_CONFIG:	    	return("LEC_TIMER_CONFIG");
	case LEC_TIMER_JOIN:	    	return("LEC_TIMER_JOIN");
	case LEC_TIMER_BUS_ARP:    	return("LEC_TIMER_BUS_ARP");
	case LEC_TIMER_READY:    	return("LEC_TIMER_READY");
	case LEC_TIMER_TYPE_ARP_AGE:	return("LEC_TIMER_TYPE_ARP_AGE");
	case LEC_TIMER_TYPE_ARP_CONTROL:return("LEC_TIMER_TYPE_ARP_CONTROL");
	case LEC_TIMER_TYPE_FLUSH:	return("LEC_TIMER_TYPE_FLUSH");
	default:          		return("UNKNOWN");
        }
    default:
	return("UNKNOWN");
    }
}


/*
 * lec_prpred
 *
 * Returns a string representing the predicate.
 */
char *lec_prpred (ulong predicate)
{
    switch (predicate) {
    case PRED_LEC_NSAP:			return("PRED_LEC_NSAP");
    case PRED_LECS_NSAP: 		return("PRED_LECS_NSAP");
    case PRED_CTL_DIRECT_NSAP:		return("PRED_CTL_DIRECT_NSAP");
    case PRED_MCAST_SEND_NSAP: 		return("PRED_MCAST_SEND_NSAP");
    default: return("UNKNOWN");
    }
}
/*
 * lec_get_fail_reason
 *
 * This function is to map the major and minor events
 * to a fail reason.  This applies to certain events only.
 */
static lec_fail_reason_t lec_get_fail_reason (lec_major_t major, int minor)
{
    switch (major) {
    case LEC_MAJOR_LOCAL:
        switch (minor) {
	case LEC_LOCAL_PREFIX_SET:	return(LEC_FAIL_PREFIX_SET);
	case LEC_LOCAL_CONFIG_CHANGE:	return(LEC_FAIL_CONFIG_CHANGE);
	case LEC_LOCAL_ACTIVATE:	return(LEC_FAIL_ACTIVATE);
	case LEC_LOCAL_DEACTIVATE:	return(LEC_FAIL_DEACTIVATE);
	default:			return(LEC_FAIL_NOTAPPLY);
        }
    case LEC_MAJOR_CONTROL:
        switch (minor) {
	case LEC_CTL_ILMI_SET_RSP_NEG:	return(LEC_FAIL_ILMI_SET_RSP_NEG);
	case LEC_CTL_CONFIG_RSP_NEG:	return(LEC_FAIL_CONFIG_RSP_NEG);
	case LEC_CTL_JOIN_RSP_NEG:    	return(LEC_FAIL_JOIN_RSP_NEG);
	case LEC_CTL_REG_RSP_NEG:	return(LEC_FAIL_REG_RSP_NEG);
	default:			return(LEC_FAIL_NOTAPPLY);
	}
    case LEC_MAJOR_SIGNALLING:
        switch (minor) {
	case LEC_SIG_RELEASE:
	case LEC_SIG_RELEASE_COMP:
	case LEC_SIG_ADD_PARTY:
	case LEC_SIG_DROP_PARTY:    	return(LEC_FAIL_CTRL_VC_RELEASED);
	default:			return(LEC_FAIL_NOTAPPLY);
	}
    case LEC_MAJOR_TIMER:
        switch (minor) {
	case LEC_TIMER_CONFIG:	    	return(LEC_FAIL_TIMER_CONFIG);
	case LEC_TIMER_JOIN:	    	return(LEC_FAIL_TIMER_JOIN);
	case LEC_TIMER_BUS_ARP:    	return(LEC_FAIL_TIMER_BUS_ARP);
	case LEC_TIMER_REG_REQUEST:	return(LEC_FAIL_TIMER_REG_REQ);
	default:          		return(LEC_FAIL_NOTAPPLY);
        }
    default:
	return(LEC_FAIL_NOTAPPLY);
    }
}

/* 
 * lec_pr_fail_reason
 *
 * Returns a character string which prepresents the LEC fail reason.
 */
char * lec_pr_fail_reason (lec_fail_reason_t reason)
{
    switch (reason) {
    case LEC_FAIL_PREFIX_SET:		return("ATM prefix being changed");
    case LEC_FAIL_CONFIG_CHANGE:	return("Local config change");
    case LEC_FAIL_ACTIVATE:
    case LEC_FAIL_DEACTIVATE:		return("Locally deactivate");
    case LEC_FAIL_ILMI_SET_RSP_NEG:	return("Fail to register address via ILMI");
    case LEC_FAIL_CONFIG_RSP_NEG:	return("Receiving negative config response");
    case LEC_FAIL_JOIN_RSP_NEG:		return("Receiving negative join response");
    case LEC_FAIL_REG_RSP_NEG:		return("Receiving negative register response");
    case LEC_FAIL_TIMER_CONFIG:		return("Timeout on config request");
    case LEC_FAIL_TIMER_JOIN:		return("Timeout on join request");
    case LEC_FAIL_TIMER_BUS_ARP:	return("Timeout on requesting BUS address");
    case LEC_FAIL_TIMER_REG_REQ:       	return("Timeout on register request");
    case LEC_FAIL_BLLI_CFG:		return("Wrong BLLI for configure VC");
    case LEC_FAIL_BLLI_DIRECT:		return("Wrong BLLI for control direct VC");
    case LEC_FAIL_BLLI_DISTRIBUTE:	return("Wrong BLLI for control distribute VC");
    case LEC_FAIL_BLLI_SEND:		return("Wrong BLLI for multicast send VC");
    case LEC_FAIL_BLLI_FORWARD:		return("Wrong BLLI for multicast forward VC");
    case LEC_FAIL_ORIG_DISTRIBUTE:	return("Wrong originator for control distribute VC");
    case LEC_FAIL_ORIG_FORWARD:		return("Wrong originator for multicast forward VC");
    case LEC_FAIL_FRAME_SIZE_MISMATCH:	return("MaxFrameSize mismatch with LES");
    case LEC_FAIL_REL_CONFIG:		return("Config VC being released");
    case LEC_FAIL_REL_CTRL_DIR:		return("Control Direct VC being released");
    case LEC_FAIL_REL_CTRL_DISTR:	return("Control Distribute VC being released");
    case LEC_FAIL_REL_MCAST_SEND:	return("Multicast Send VC being released");
    case LEC_FAIL_REL_MCAST_FORWARD:	return("Multicast Forward VC being released");
    case LEC_FAIL_CTRL_VC_RELEASED:	return("Control VC being released");
    case LEC_FAIL_INIT:			return("Initializaton");
    case LEC_FAIL_SETUP_LECS_VC:	return("Fail to set up config VC");
    default:				return("Not applicable");
    }
}


/*
 * lec_fsm
 *
 * Given a major and minor event, call the fsm_execute and run the
 * state machine.
 */
ushort
lec_fsm (lec_info_t *lec_info, lec_major_t major, int minor)
{
    int         fsm_rc;
    lec_state_t	old_state;

    /*
     * Simply return if the router is still booting.  This flag is set
     * once inside the lec_input process.
     */
    if (!lec_booted) {
		return(FSM_OK);
    }

    lec_fail_reason = lec_get_fail_reason(major, minor);
    /*
     *  Execute the LE Client State Machine.
     */
    old_state = lec_info->state;
    fsm_rc = (int) fsm_execute("LEC", 
			       lec_fsm_table,
                               ((int *) &(lec_info->state)),
			       MAX_LEC_FSM_STATE,
                               (void *) lec_info, 
			       major, 
			       minor,
			       FALSE);

    if (fsm_rc < 0) {
        switch (fsm_rc) {
	case FSM_RC_UNKNOWN_STATE:
	    LEC_STATE_DEBUG ("\nLEC %s: unknown state: state %s event %s",
			     lec_info->swidb->namestring,
			     lec_prstate(old_state), 
			     lec_prinput(major, minor));
	    break;	
	case FSM_RC_UNKNOWN_INPUT:
	    LEC_STATE_DEBUG ("\nLEC %s: unknown input: %s %s input= %d %d",
			     lec_info->swidb->namestring,
			     lec_prstate(old_state), 
			     lec_prinput(major, minor),
			     major,
			     minor);
	    LEC_STATE_DEBUG ("\nLEC %s: major=%d minor=%d",
			     lec_info->swidb->namestring,
			     major,
			     minor);
	    break;
	    /*
	     * If someone sent us bogus information on the control
	     * plane, this return will result in the teardown of the LEC
	     * and return to IDLE.
	     */
	case FSM_RC_INVALID_INPUT:
	    LEC_STATE_DEBUG ("\nLEC %s: invalid input: state %s event %s",
			     lec_info->swidb->namestring,
			     lec_prstate(old_state),
			     lec_prinput(major, minor));
	    LEC_STATE_DEBUG ("\nLEC %s: major=%d minor=%d",
			     lec_info->swidb->namestring,
			     major, minor);
	    a_teardown_lec(lec_info);
	    break;
	case FSM_RC_ERROR_INPUT:
	    LEC_STATE_DEBUG ("\nLEC %s: error input: state %s event %s",
			     lec_info->swidb->namestring,
			     lec_prstate(old_state),
			     lec_prinput(major, minor));
	    LEC_STATE_DEBUG ("\nLEC %s: major=%d minor=%d",
			     lec_info->swidb->namestring,
			     major, minor);
	case FSM_RC_DATA_ERROR:
	    LEC_STATE_DEBUG ("\nLEC: data error: %s %s %s input= %d %d",
			     lec_info->swidb->namestring,
			     lec_prstate(old_state), 
			     lec_prinput(major, minor),
			     major,
			     minor);
	    LEC_STATE_DEBUG ("\nLEC %s: major=%d minor=%d",
			     lec_info->swidb->namestring,
			     major,
			     minor);
            break;
	}
    } else {
	LEC_STATE_DEBUG ("\nLEC %s: state %s event %s => %s",
			 lec_info->swidb->namestring,
			 lec_prstate(old_state),
			 lec_prinput(major, minor),
			 lec_prstate(lec_info->state));
    }
    return(fsm_rc);
}


/*
 * lec_get_ctrl_vc_fail 
 *
 * If an LEC is brought down due to the release of a control vc,
 * based on the type of the control vc, further break down 
 * the fail reason into more detailed categories.
 */ 
lec_fail_reason_t
lec_get_ctrl_vc_fail (lane_vc_type_t lane_vc_type)
{
    switch (lane_vc_type) {
    case LANE_LEC_CONFIGURE:	return(LEC_FAIL_REL_CONFIG);
    case LANE_LEC_DIRECT: 	return(LEC_FAIL_REL_CTRL_DIR);
    case LANE_LEC_DISTRIBUTE:	return(LEC_FAIL_REL_CTRL_DISTR);
    case LANE_LEC_SEND:
    case LANE_LEC_TR_SEND:	return(LEC_FAIL_REL_MCAST_SEND);
    case LANE_LEC_FORWARD:	
    case LANE_LEC_TR_FORWARD:	return(LEC_FAIL_REL_MCAST_FORWARD);
    default:			return(LEC_FAIL_NOTAPPLY);
    }
}






