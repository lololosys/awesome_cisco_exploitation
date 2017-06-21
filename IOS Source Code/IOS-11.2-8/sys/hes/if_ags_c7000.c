/* $Id: if_ags_c7000.c,v 3.39.4.45 1996/09/07 18:57:01 xtang Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_ags_c7000.c,v $
 *------------------------------------------------------------------
 * if_ags_c7000.c -- Common RP1 (C/7000), MCI interface code
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_ags_c7000.c,v $
 * Revision 3.39.4.45  1996/09/07  18:57:01  xtang
 * CSCdi67401:  ALIGN-3-SPURIOUS: Spurious memory access made at
 * 0x60191478 reading
 * Branch: California_branch
 *         Turned off hwidb->serial_flags for HSSI and POSIP interfaces,
 *         therefore disallow clock rate (clockrate) command on these.
 *         Any serial interfaces that do not have clockrate command
 *         implemented should act similarly.
 *
 * Revision 3.39.4.44  1996/08/28  12:44:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.39.4.43  1996/08/26  15:06:51  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.39.4.42  1996/08/24  05:01:27  fred
 * CSCdi67070:  remove upper bound from red queues
 * Branch: California_branch
 *
 * Revision 3.39.4.41  1996/08/21  19:17:24  fox
 * CSCdi57429:  Router cannot distinguish between FCS bit set/not set in
 * PPP header
 * Branch: California_branch
 * Add support for RFC 1638 bridging packet header fields.  Bounce packets
 * to process level if any fields are set.
 *
 * Revision 3.39.4.40  1996/08/20  00:34:33  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.39.4.39  1996/08/13  02:17:14  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.39.4.38  1996/08/12  16:01:44  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.39.4.37  1996/08/11  23:33:39  ppearce
 * CSCdi61195:  RSRB performance degraded 40% from 11.1(1)
 * Branch: California_branch
 *   Erroneous RSRB debug processing -- missing check for debug enabled
 *
 * Revision 3.39.4.36  1996/08/02  00:26:38  rbadri
 * CSCdi63540:  Bridging NetBEUI over LANE from 4500 doesnt work
 * Branch: California_branch 
 * lane_aip_fast() should also have swidb as one of its parameters.
 *
 * Revision 3.39.4.35  1996/07/26  01:09:47  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.39.4.34  1996/07/23  18:45:11  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.39.4.33  1996/07/23  13:22:47  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.39.4.32  1996/07/22  01:32:24  psandova
 * CSCdi59090:  Non-explorer packets are being flushed with explorers
 * Branch: California_branch
 * Valid multicast packets that should be handed to the protocol stack
 * are instead being diverted to the srb module and are flushed by the
 * explorer control mechanism.
 *
 * Revision 3.39.4.31  1996/07/20  03:21:09  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.39.4.30  1996/07/19  19:13:08  ppearce
 * CSCdi62738:  rsrb over tcp connection fails on ags+ and 7k
 * Branch: California_branch
 *   Ensure pak->rxtype is initialized before reg_invoke_srt_srb_forward(
 *
 * Revision 3.39.4.29  1996/07/18  20:28:12  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.39.4.28  1996/07/12  20:46:29  weismann
 * CSCdi59647:  CRB of vines over ether-token-ether not working. Properly
 * classify bridged broadcast packets in interrupt handler so they are
 * processed at interrupt level for ALL protocols.
 * Branch: California_branch
 *
 * Revision 3.39.4.27  1996/07/11  14:18:53  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.39.4.26  1996/07/09  05:51:01  ppearce
 * CSCdi59527:  inlines wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.39.4.25  1996/07/08  22:25:12  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.39.4.24  1996/07/04  00:14:27  rbadri
 * CSCdi61232:  IP & IPX ping packets > 1484 would fail on RSP (1483-LANE
 * Bridging)
 * Branch: California_branch
 * For ATM, check the incoming datagramsize against sub_mtu + encaps
 * overhead.
 *
 * Revision 3.39.4.23  1996/07/03  17:40:19  ppearce
 * CSCdi61267:  DSPU-CSNA connection failure: Rx-type field not init'd
 * Branch: California_branch
 *   pak->rxtype not init'd when pak created for sending to process-level
 *
 * Revision 3.39.4.22  1996/06/28  21:57:27  dawnli
 * CSCdi56696:  CDP-EV: Bad checksum in header
 * Branch: California_branch
 * If a CDP packet is ISL encapsulated, datagramsize is incorrect.
 * It includes ISL header length, which cause bad checksum.
 * If a regular CDP packet, not ISL encapsulated, is received,
 * if_input may set to an ISL sub-interface.
 *
 * Adjust datagramsize to the right value, and reset fast_rcvidb to
 * firstsw, so that if_input will be set to the right swidb.
 *
 * Revision 3.39.4.21  1996/06/18  01:43:51  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.39.4.20  1996/06/07  19:03:16  kramling
 * CSCdi55236:  Problems with FEIP OIR
 * Branch: California_branch
 *
 * Revision 3.39.4.19  1996/06/01  06:15:11  hampton
 * Remove obsolete media_tring_supports_etr registry call.  [CSCdi59254]
 * Branch: California_branch
 *
 * Revision 3.39.4.18  1996/05/29  03:46:12  skufer
 * CSCdi55758:  800E on TRIP steals MEMD from global queue, tql > tx queue
 * limit
 * Branch: California_branch
 *         TRIP on C7000 may advance tql when noisy ring causes
 *         frequent beacons.  Result is performance degradation.  When
 *         resetting the trip, new code prevents advancing the tql.  It
 *         is only a problem on c7000, not c7500 series.
 *
 * Revision 3.39.4.17  1996/05/28  18:05:08  fred
 * CSCdi58819:  7000 checks wrong hardware address field in
 * set_cbus_txlimit
 * Branch: California_branch
 *
 * Revision 3.39.4.16  1996/05/25  23:37:48  shaker
 * CSCdi52227:  cbus_buffer_carve() broken on 7000
 * Branch: California_branch
 * Fix the loop that someone stepped on. Also, prevent
 * a possible divide by zero.
 *
 * Revision 3.39.4.15  1996/05/22  14:45:03  fbordona
 * CSCdi57997:  DLSw FST over WAN Frame Relay can not establish circuits
 *              Use reg_invoke_framerelay_fs() for frame-relay RXTYPE_DLSW
 *              Remove DLSw FST frame-relay code from interrupt routines.
 * Branch: California_branch
 *
 * Revision 3.39.4.14  1996/05/22  13:51:03  getchell
 * CSCdi57387:  corrupt pool pointer, VIP2 dies, box not healthy;VIP2
 * removed
 * Branch: California_branch
 *
 * Revision 3.39.4.13  1996/05/21  19:39:36  fsunaval
 * CSCdi45026:  Frames of 17 bytes (without LLC) are considered as runt
 * Branch: California_branch
 *
 * Revision 3.39.4.12  1996/05/21  09:47:47  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.39.4.11  1996/05/21  06:29:14  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.39.4.10  1996/05/17  11:17:30  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.37.2.8  1996/05/05  23:06:52  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.37.2.7  1996/05/02  22:03:19  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.37.2.6  1996/05/02  20:03:54  hampton
 * Fix AGS compilation problem.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.37.2.5  1996/04/26  15:18:35  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.37.2.4  1996/04/26  07:53:36  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.37.2.3  1996/04/03  14:28:57  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.37.2.2  1996/03/21  06:07:29  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.37.2.1  1996/03/17  17:48:54  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.39.4.9  1996/05/13  18:35:03  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *                 pick up commit error
 * Branch: California_branch
 *
 * Revision 3.39.4.8  1996/05/13  14:46:45  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.39.4.7  1996/05/09  14:28:37  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.39.4.6  1996/04/29  16:03:52  fred
 * CSCdi54721:  tql set too low on fsip when no fancy queueing configured
 * Branch: California_branch
 *                 maxke sure cbus_mci_txlimit is called after changing
 *         the link's characteristic
 *
 * Revision 3.39.4.5.4.1  1996/04/27  07:02:08  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.39.4.5  1996/04/17  13:39:01  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.39.4.4  1996/04/12  02:33:30  izhu
 * CSCdi49616:  %LINK-4-NOMAC error message during bootup
 * Branch: California_branch
 *
 * Revision 3.39.4.3  1996/04/03  22:49:04  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.39.4.2.6.1  1996/04/08  01:46:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.39.4.2  1996/03/21  22:40:27  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.39.4.1  1996/03/18  19:40:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.39.6.2  1996/03/28  17:24:39  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.39.6.1  1996/03/22  09:36:16  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.39  1996/03/07  12:30:49  dkerr
 * CSCdi50949:  Customer can crash router getting cbus lowcore dump
 * Set lowcore capture trigger by default and have results displayable
 * from "show controller cxbus memd-core".
 *
 * Revision 3.38  1996/02/28  22:42:04  motto
 * CSCdi49312:  CIP IPC not working after router reload
 *
 * Revision 3.37  1996/02/27  05:05:07  rharagan
 * CSCdi49431:  FSIP resets with error 8010 disable - fsip_reset
 * Enable ctrlr resync.
 *
 * Revision 3.36  1996/02/23  18:45:33  banderse
 * CSCdi49679:  Fix for CSCdi47707 can produce extraneous garbage in
 * config.
 * Replace qualify for newcard with !firsttime; add pre_hotstall_state
 * initialization when not oir event
 *
 * Revision 3.35  1996/02/18  19:41:11  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.34  1996/02/16  13:22:46  banderse
 * CSCdi47707:  CIP download failure should not administratively shut
 * interfaces
 * Qualify newcard insertion with "eoir_cycle" instead of "!firsttime"
 *
 * Revision 3.33  1996/02/13  21:26:39  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.32  1996/02/13  08:12:13  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.31  1996/02/13  05:50:27  ogrady
 * CSCdi45887:  Router crash in rsp_fastsend
 *              Use common routine for token ring output debug.
 *
 * Revision 3.30  1996/02/13  01:27:03  fred
 * CSCdi48680:  Zerodivide in CBUS carving
 *         install defensive code to avoid zerodivides when bandwidth is
 *         zero or when card has no useful interface
 *
 * Revision 3.29  1996/02/12  23:05:35  hwang
 * CSCdi46480:  4R failed hot standby test
 * Allow vip 4r driver to set new functional address used by
 * hsrp protocol.
 *
 * Revision 3.28.6.1  1996/03/05  06:11:05  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.28.2.1  1996/02/27  20:41:50  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.28  1996/02/07  16:11:25  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.27  1996/02/01  06:02:32  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.26  1996/01/26  21:03:22  rharagan
 * CSCdi47313:  4T love letters lost when non-default encaps used
 * Don't permit STUN to jump over sniff for IPC, use idb->rxoffset to find
 * encap.
 *
 * Revision 3.25  1996/01/24  01:39:12  rramacha
 * CSCdi46884:  vip interfaces multipy after micro reload
 *
 * Revision 3.24.2.1  1996/01/24  21:45:47  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.24  1996/01/20  03:22:28  tgrennan
 * CSCdi46884:  vip interfaces multipy after micro reload
 *
 * Missing piece of CSCdi42660.
 *
 * Revision 3.23  1996/01/19  22:14:53  wmay
 * CSCdi45344:  ACL: input access lists are ignored on SMDS subinterfaces
 * Sniff smds dest addr and switch to correct swidb.
 *
 * Revision 3.22  1996/01/15  18:04:05  schaefer
 * CSCdi45761: support RSP in new chassis
 *
 * Revision 3.21  1996/01/13  03:09:39  dlobete
 * CSCdi44696:  OIR and IPC troubles - selective sequence number synch
 * needed
 * Add slot granularity to IPC seat reset.
 *
 * Revision 3.20  1996/01/09  01:54:52  sthormod
 * CSCdi34871:  garbage interface entry in rvip configuration
 * This is not a VIP specific problem.  Any time new idbs are created as
 * a result of a "micro reload" command, their protocol fields are not
 * properly initialized.  If a new idb was created, mark it as
 * IDBS_ADMINDOWN so that the OIR code will do the right thing for this
 * new interface.
 *
 * Revision 3.19.2.2  1996/02/15  21:26:20  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.19.2.1  1996/02/15  19:12:49  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.19  1996/01/05  04:55:51  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.18  1995/12/26  23:08:19  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.17  1995/12/19  00:40:08  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.16  1995/12/16  02:29:53  banderse
 * CSCdi45721:  CSNA to CIP broken
 * change to CBUS_CHANNEL_RECEIVE macro
 *
 * Revision 3.15  1995/12/15  08:01:30  rramacha
 * CSCdi42101:  sh ver does not recognize CX-SMIP or CX-SSIP cards
 *
 * Revision 3.14  1995/12/13  04:19:52  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.13  1995/12/13  02:28:17  ppearce
 * CSCdi45473:  %SYS-3-SUPNONE: Registry 6 doesnt exist output during boot
 *   (1) Rename fddi_srb_in() to fddi_to_tr()
 *         to parallel the tr_to_fddi() routine that already exists
 *   (2) Remove srt_fddi_in() registry from srt/srt_registry.reg
 *         and add media_fddi_to_tr() registry into h/media_registry.reg
 *
 * Revision 3.12  1995/12/10  23:27:28  rpratt
 * CSCdi45343:  Chassis MIB enhancements for VIP & port adapters
 *
 * Revision 3.11  1995/12/07  22:28:36  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.10  1995/12/06  18:28:42  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.9  1995/12/02  06:55:17  tgrennan
 * CSCdi42449:  4/8e address filter improperly set by RP
 *
 * 4/8e address filter works like FE and FEIP instead of EIP/MCI.
 * Also, split large switch in cbus_mci_idb_init() into sub-functions.
 *
 * Revision 3.8  1995/12/02  05:50:07  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.7  1995/11/29  02:47:48  sthormod
 * CSCdi42503:  4r needs modularity work
 * Clean up #ifdef C7000 kludge.
 *
 * Revision 3.6  1995/11/28  00:24:10  fred
 * CSCdi43173:  Fast-switching fails over mip
 *         If Fair Queuing already configured on MIP, deconfigure it.
 *         If Fair Queuing selected but not yet configured, select FCFS
 *         instead.
 *         MIP does not support queuing that requires backpressure
 *
 * Revision 3.5  1995/11/22  21:19:36  anke
 * CSCdi41841:  Duplicate cardSlotNumber entries created when hotswapping
 * Use the slot number as the remkey when adding or removing snmp card
 * entries instead of the dci pointer.
 *
 * Revision 3.4  1995/11/20  18:56:39  gstovall
 * CSCdi43723:  FastEthernet being squirelled away
 * Oooops, idb->type should be setup before calling ether_idb_init().
 *
 * Revision 3.3  1995/11/17  09:16:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:50  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.41  1995/11/08  21:00:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.40  1995/10/25  23:09:40  vrawat
 * CSCdi41558:  STUN-basic drops HDLC frames with certain data patterns
 *
 * Revision 2.39  1995/10/24  20:19:35  kramling
 * CSCdi42750:  Fast Ethernet Interface Processors not quiesced during EOIR
 *
 * Revision 2.38  1995/10/24  20:12:33  kramling
 * CSCdi42751:  Multibus ethernet interfaces can be initialized improperly.
 * Check for NULL cdp ptr.
 *
 * Revision 2.37  1995/10/24  07:01:27  gchristy
 * CSCdi42368:  local timeout crash in cbus_mci_txlimit()
 * Copy tx_queue value into controller mci_txcount for cbus2 as well as
 * cbus1.
 *
 * Revision 2.36  1995/10/18  07:15:19  gchristy
 * CSCdi42368:  local timeout crash in cbus_mci_txlimit()
 * - Only call controller_find() with cbus interfaces.
 *
 * Revision 2.35  1995/10/13  17:48:26  fred
 * CSCdi39655:  Fast switching from ATM to MIP is broken
 *         The MIP does not refuse messages; it accepts them and then
 *         discards if it can't queue. Therefore, existing priority,
 *         customer, and fair queuing algorithms won't work with it.
 *         For the present, disable fair queuing as the default mode,
 *         and defer algorithm changes to the future.
 *
 * CSCdi39026:  TX queue limit does not change when WFQ is turned off
 *         Need to update cdb->mi_txcount when idb->mci_txcount is changed
 *
 * Revision 2.34  1995/10/03  19:30:07  speakman
 * CSCdi41429:  IP multicasts get process-switched with bridging
 * In the ether case, make sure IP multicasts get fastswitched
 * AFTER the call to tbridge.  In the fddi case, make sure IP
 * multicasts get fastswitched before the call to tbridge.
 *
 * Revision 2.33  1995/09/25  08:33:41  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.32  1995/09/20  23:27:54  wilber
 * CSCdi37174:  CRB of decnet over fddi on Highend is broken
 * Check bridging and station bit first in CBUS_FDDI_RECEIVE to prevent
 * bridgeable packets from getting into fast-switch code and thereby
 * inadvertently routed
 *
 * Revision 2.31  1995/09/12  21:09:26  pst
 * CSCdi39958:  ip ethernet 802.3 packets bridged when they should be
 * routed
 *
 * Revision 2.30  1995/09/12  00:02:53  hampton
 * Remove unnecessary include.  [CSCdi39966]
 *
 * Revision 2.29  1995/09/11 10:23:37  kmitchel
 * CSCdi25909:  Erroneous CIP configuration commands are still added to
 *              config file
 *
 * Revision 2.28  1995/08/18  23:18:46  alam
 * CSCdi38456:  SRB fails on AGS platform
 * Correct spelling mistake.
 *
 * Revision 2.27  1995/08/18  23:13:02  alam
 * CSCdi38456:  SRB fails on AGS platform
 * Only SRB frames that have RII enabled.
 *
 * Revision 2.26  1995/08/09  16:30:57  fred
 * CSCdi38126:  bandwidth command and fair-queueing result in 0 tql
 * consider the bandwidth, priority status, and use of LAPB when
 * configuring the tql from a controller block
 *
 * Revision 2.25  1995/08/09  00:19:30  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.24  1995/08/08  19:26:44  fred
 * CSCdi36704:  IP Fastswitching / WFQ cleanup for high-end
 *         assure that tx-queue-limit is always at least two in PQ/CQ/WFQ
 *         cases
 *
 * Revision 2.23  1995/08/07  05:33:51  richl
 * CSCdi38315:  makefile rework
 *              move code to tring_inline and to rif_util in libutil.a
 *
 * Revision 2.22  1995/08/06  23:00:37  bbenson
 * CSCdi36725:  LEC needs to obtain LECS address from the switch.
 *
 * Revision 2.21  1995/08/04  20:34:47  motto
 *  Removed vc variable from cbus_interrupt stack declaration.
 * CSCdi38272:  AGS image does not build anymore
 *
 * Revision 2.20  1995/08/04  14:10:04  motto
 * Changed CIP LAN interface into SRB to only have one entry per LAN
 * instead of per adapter.
 * CSCdi37535:  CIP LAN SRB explorers multiplying by # of adapters defined
 * per LAN
 *
 * Revision 2.19  1995/08/01  21:57:45  motto
 * Needed to add support in the srb-explorerfastswitching code for
 * the CIP interanl LAN interfaces.
 * CSCdi37528:  CIP internal LAN SRB fastswitch explorer does not work
 *
 * Revision 2.18  1995/07/17  21:55:20  shj
 * CSCdi37128:  Startup registry error message
 *
 * Move MAC assignment for AIP from common code (atm.c) to platform
 * specific (if_ags_c7000.c) so it doesn't fail cause registry error
 * on 4500.
 *
 * Revision 2.17  1995/07/06  02:48:07  rharagan
 * CSCdi36490:  pinging on the AIP induced a crash in the SP microcode
 * Send the version command to the IP not the SP if the tx queue appears
 * to hang.
 *
 * Revision 2.16  1995/07/06  01:32:05  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.15  1995/07/01 19:15:01  richl
 * CSCdi34435:  %SYS-3-SUPNONE: Registry 6 doesnt exist
 *              o Changed above msg so it isn't rate based
 *              o Protected call to registry with conditional statement
 *              o Resolved conflict in DRIVER/PROTOCOL class between
 *                the cbus_bridge and tbridge subsystems.
 *
 * Revision 2.14  1995/06/30  20:00:09  fred
 * CSCdi36499:  priority-group causes fddi interface down
 *              resulted from incorrect type of variable
 *
 * Revision 2.13  1995/06/23  18:47:37  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.12  1995/06/23  01:31:16  motto
 * Code review cleanup
 *   - Remove debug code.
 *   - Restore explorer fastswitch for non CIP interfaces.
 * CSCdi36240:  SRB fastswitching and process switching broken for CIP
 * v-interface
 *
 * Revision 2.11  1995/06/22  22:48:59  wmay
 * CSCdi36246:  Appletalk doesnt fast-switch smds
 *
 * Revision 2.10  1995/06/22  15:32:37  motto
 * CSCdi36240:  SRB fastswitching and process switching broken for CIP
 * v-interface
 *
 * Revision 2.9  1995/06/21  03:27:04  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.8  1995/06/19  04:06:06  speakman
 * CSCdi35697:  General TB and SRB regression failures
 * Make protocol-specific route/bridge test explicit rather than relying
 * on per-interface rxtypes indicators to provide backward compatible
 * SRB and TB behaviour.
 *
 * Revision 2.7  1995/06/18  23:16:56  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.6  1995/06/16  17:24:20  fred
 * CSCdi34574:  ip f/s serial -> fddi is broken
 * Propagate the change in the default tx-queue-limit for priority to LAPB
 * also - this significantly simplifies the code
 *
 * Make sure that CBUS controllers (cdb) use same algorithm as MCI
 * controllers (idb).
 *
 * Make displays display actual tx-queue-limit used rather than
 * peremptorily deciding it is 1 in some cases
 *
 * Revision 2.5  1995/06/10  17:50:40  pitargue
 * CSCdi35638:  packets going to process level from the 4500 atm nim
 * crashes router
 * make sure to initialize fast_vc_info, before calling
 * hwidb->parse_packet
 *
 * Revision 2.4  1995/06/09  19:50:24  rharagan
 * CSCdi34434:  changing atm mtu interrupts traffic on ISDN PRI
 * Set minimum cbus buffer pool size to ehternet MTU.
 *
 * Revision 2.3  1995/06/09  04:47:39  rharagan
 * CSCdi35371:  Divide by zero exception w/ Corona and all ether MTUs
 * Correct index used in check for next larger pool.
 *
 * Revision 2.2  1995/06/08  23:37:05  alam
 * CSCdi35548:  changes per code review comments
 *
 * Revision 2.1  1995/06/07  20:40:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "../hes/cbus_registry.h"
#include "../srt/srt_registry.h"
#include "media_registry.h"
#include "stacks.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "../if/tring.h"
#include "../if/tring_private.h"
#include "../srt/trsrb.h"
#include "../srt/rsrb.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"
#include "../srt/srb_error.h"
#include "../srt/srb_multiring.h"
#include "../if/rif_inline.h"
#include "../if/tring_common.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/ether_private.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../wan/serial_parser.h"
#include "../tbridge/tbridge_debug.h"
#include "if_mci.h"
#include "if_hes_common.h"
#include "if_fci.h"
#include "../hes/if_fip.h"
#include "dbus.h"
#include "../hes/dgbus.h"
#include "../if/fddi.h"
#include "../if/fddi_private.h"
#include "if_cmd.h"
#include "if_trip.h"
#include "../hes/if_ags_c7000_trip.h"
#include "if_cip.h"
#include "../if/channel.h"
#include "channel_sb.h"
#include "logger.h"
#include "../if/atm.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_hes.h"
#include "../ip/ip_registry.h"
#include "../wan/smds.h"
#include "../wan/ppp.h"
#include "../ibm/lanmgr.h"
#include "../ibm/sdlc.h"
#include "../snmp/chassismib_registry.h"
#include "../if/if_controller.h"
#include "hes_controller.h"
#include "../if/dsx1.h"
#include "if_mip.h"
#include "if_vip.h"
#include "../hes/cbus_test.h"
#include "parser.h"
#include "parser_defs_fci.h"
#include "../parser/parser_defs_priority.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../hes/if_aip.h"
#ifdef C7000
# include "../hes/parser_defs_ucode.h"
# include "../hes/ucode.h"
# include "../hes/sse_public.h"
# include "../ipc/ipc_cbus.h"
#endif

#include "../hes/cbus_debug.h"
#include "../if/channel_debug.h"
#include "../xdi/xdihdrsif.h"
#include "../xdi/fddihdr.h"
#include "../xdi/fbmframe.h"
#include "../if/if_fddi.h"
#include "../if/if_fddi_public.h"
#include "hes_fddi_public.h"
#include "if_hes_ether.h"
#include "if_hes_serial.h"
#include "ctrlr_cmds.h"
#include "if_feip.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "msg_cbus.c"			/* Not a typo, see logger.h */
#include "msg_dbus.c"			/* Not a typo, see logger.h */
#include "msg_oir.c"			/* Not a typo, see logger.h */
#include "msg_ucode.c"			/* Not a typo, see logger.h */
#include "../os/init.h"
#include "../pas/pas_id.h"
#ifdef C7000
#include "../hes/parser_defs_ucode.h"
#include "if_vip_4t.h"
#include "../dev/flash_dvr_spec.h"
#include "slot.h"
#endif

#include "../tbridge/tbridge_public.h"
#include "../tbridge/tbridge_hes_private.h"


/* Following includes needed by lane_aip_fast */

#include "mgd_timers.h"

#include "../atm/atmsig_api.h"
#include "../atm/ilmi_api.h"

#include "../lane/lane.h"
#include "../lane/lane_private.h"
#include "../lane/lane_signal.h"
#include "../lane/lane_cfgacc.h"
#include "../lane/lec_private.h"
#include "../lane/lsv_private.h"
#include "../if/priority_private.h"
#include "../lane/lane_aip_fast.h"
#include "../vlan/vlan.h"
#include "../iprouting/igrp.h"

#include "../tbridge/tbridge_sde.h"
#include "../if/tring_inline.h"
#include "../parser/parser_defs_parser.h"

#define	ALTERNATE	NONE
#include "cfg_int_cbus_buffers.h"
LINK_POINT(cbus_buffers_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * External variables.
 */

extern int nCTR;
extern int nMEC;
extern int nFSIP;
extern int nFIP;
extern int nHIP;
extern int nAIP;
extern int nMIP;
extern int nCIP;
extern int nCIP2;
extern int nFEIP;
extern int nRVIP;
extern int fsip_ser;
extern int nTRs;
extern int nhssinets;
extern int nposnets;
extern int natmnets;
extern int ne1ports;
extern int nt1ports;
extern int cip_channel;
extern int cip2_channel;
extern int ctr_trs;
extern int feip_feth;
extern int rvip_eth;
extern int rvip_feth;
extern int rvip_tr;
extern int rvip_ser;
extern int ncBus;
extern boolean oir_debug;
extern ulong cbus_ipcache_hits;

extern void *lev3ssubr;

static char *dci_memtest(dcitype *);



/*
 * Local Storage
 */
int nMCI;			/* number of cisco MCI controllers */

boolean zebra_debug;

static int cbus_base_fddi;
static int cbus_base_hssi;
static int cbus_base_token;
static int cbus_base_aip;
static int cbus_base_t1;
static int cbus_base_e1;
static int cbus_base_channel;
static int cbus_base_feth;
ulong cbus_cache_allocated;

queuetype cbuspreQ;
queuetype idbpreQ;
queuetype dcipreQ;
queuetype cdbpreQ;

static int dsx1_count;

int fci_max_dci_ctrlrs;


/*
 * cbus_mci_memtest
 * Run basic bus and memory tests.
 * If anything fails, return pointer to string describing test.
 * A healthy cBus/MCI returns a NULL pointer
 */
char *cbus_mci_memtest (mcitype *cbus)
{
    ushort indx, jndx, mem_size; 
    ushort sresult;
    mciregtype *regptr;
#ifdef C7000
    dgtype *dgbus;
#endif /* C7000 */

    regptr = cbus->regptr;
#ifdef C7000
/*
 * For all cards on the assumed CxBus number: reset them.
 * Then load and release the SP to run.  (Emulation of FCI_HARD_RESET.)
 * !!!!!! Don't do this for MCI cards.  Use Diag bus to determine.
 */
    if (!(cbus->flags & CXBUS)) {  /* Not a CXBUS device */
	dgbus = pslot_to_dbus(cbus->unit);
	if (touch_device(dgbus)) { /* has dbus interface, must be SIP */
	    dbus_ctrlr_reset(dgbus); /* reset but do not reload ucode */
	    dbus_slot_enable(dgbus); /* run */
	    DELAY(FCI_SOFT_RESET_TIME);
	} else {	/* must be old-style MCI board in test chassis */
	    regptr->argreg = FCI_HARD_RESET;
	    regptr->cmdreg = FCI_CMD_RESET;
	    if (cbus->flags & CBUSII) {
		DELAY(CBUSII_HARD_RESET_TIME);
	    } else {
		DELAY(FCI_HARD_RESET_TIME);
	    }
	}
    }
#else
    regptr->argreg = FCI_HARD_RESET;
    regptr->cmdreg = FCI_CMD_RESET;
    if (cbus->flags & CBUSII) {
	DELAY(CBUSII_HARD_RESET_TIME);
    } else {
	DELAY(FCI_HARD_RESET_TIME);
    }
#endif    
    /*
     * Determine the amount of cache and main memory on the cBus.
     */
    regptr->argreg = FCI_MEMSIZE_MEMA;
    regptr->cmdreg = FCI_CMD_MEMSIZE;
    cbus->mema_size = regptr->argreg;
    regptr->argreg = FCI_MEMSIZE_MEMD;
    regptr->cmdreg = FCI_CMD_MEMSIZE;
    if (cbus->flags & CBUSII)
	cbus->memd_size = regptr->largreg;
    else
	cbus->memd_size = regptr->argreg;
    
    /*
     * Test buses are free 
     */
    for (indx = 0; indx < 255; indx++) {
	regptr->argreg = indx;
	sresult = regptr->argreg;
	if (sresult != indx)
	    return("bus");
    }
    
    /*
     * Test the main buffer memory by writing 0x0000's and 0xFFFF's.
     */
    mem_size = cbus->memd_size;
    regptr->argreg = 6;
    regptr->cmdreg = FCI_CMD_MEMA_SELECT;
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_MEMA_WRITE;

    for (indx = 0; indx < 2; indx++) {
	
	/*
	 * Set all main buffer memory to zero and then 0xffff.
	 */
	if (cbus->flags & CBUSII)
	    regptr->largreg = FCI_MEMD_BASE;
	else
	    regptr->argreg = FCI_MEMD_BASE;
	regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	for (jndx = FCI_MEMD_BASE; jndx < mem_size; jndx++)
	    regptr->write0short = indx ? 0xFFFF : 0;
	
	/*
	 *  Read it back to ensure it sticks.
	 */
	if (cbus->flags & CBUSII)
	    regptr->largreg = FCI_MEMD_BASE;
	else
	    regptr->argreg = FCI_MEMD_BASE;
	regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	for (jndx = FCI_MEMD_BASE; jndx < mem_size; jndx++) {
	    sresult = regptr->readshort;
	    if (sresult != (indx ? 0xFFFF : 0)) {
		regptr->cmdreg = 17;
		buginf("\nfailed %s: read %x, expected %x, loc %x",
		       indx ? "memd SA1" : "memd SA0", sresult,
		       indx ? 0xFFFF : 0, jndx);
		return(indx ? "memd SA1" : "memd SA0");
	    }
	}
    }
    
    /*	
     * Test main memory address + 1.
     */
    if (cbus->flags & CBUSII)
	regptr->largreg = FCI_MEMD_BASE;
    else
	regptr->argreg = FCI_MEMD_BASE;
    regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    for (indx = FCI_MEMD_BASE; indx < mem_size; indx++)
	regptr->write0short = indx + 1;
    if (cbus->flags & CBUSII)
	regptr->largreg = FCI_MEMD_BASE;
    else
	regptr->argreg = FCI_MEMD_BASE;
    regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    for (indx = FCI_MEMD_BASE; indx < mem_size; indx++) {
	sresult = regptr->readshort;
	if (sresult != indx + 1)
	    return("memd addr");
    }
    
    /*
     * Test mema memory, by writing 0x0000's and the 0xFFFF's.
     */
    regptr->argreg = MCI_HW_VERSION;
    regptr->cmdreg = MCI_CMD_VERSION;
    /*
     * MCI/cBus controller MEMA diagnostic.
     */
     mem_size = cbus->mema_size;
     for (indx = 0; indx < 2; indx++) {
         regptr->argreg = FCI_MEMA_BASE;
         regptr->cmdreg = FCI_CMD_MEMA_SELECT;
         regptr->argreg = indx ? 0xffff : 0;
         for (jndx = FCI_MEMA_BASE; jndx < mem_size; jndx++)
      	     regptr->cmdreg = FCI_CMD_MEMA_WRITE;
	    
	 /* 
	  * Read the data back to ensure that it's correct.
	  */
	 regptr->argreg = FCI_MEMA_BASE;
	 regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	 for (jndx = FCI_MEMA_BASE; jndx < mem_size; jndx++) {
	     regptr->cmdreg = FCI_CMD_MEMA_READ;
	     sresult = regptr->argreg;
	     if (sresult != (indx ? 0xffff : 0))
	         return(indx ? "mema SA1" : "mema SA0");
         }
    }
    /*
     * Test cache memory address + 1.
     */
    regptr->argreg = FCI_MEMA_BASE;
    regptr->cmdreg = FCI_CMD_MEMA_SELECT;
    regptr->argreg = 0xFFFF;
    for (indx = FCI_MEMA_BASE; indx < mem_size; indx++) {
	regptr->argreg = indx + 1;
	regptr->cmdreg = FCI_CMD_MEMA_WRITE;
    }
    regptr->argreg = FCI_MEMA_BASE;
    regptr->cmdreg = FCI_CMD_MEMA_SELECT;
    for (indx = FCI_MEMA_BASE; indx < mem_size; indx++) {
	regptr->cmdreg = FCI_CMD_MEMA_READ;
	sresult = regptr->argreg;
	if (sresult != indx + 1)
	    return("mema addr");
    }
    /*
     * Return NULL pointer if no errors
     */
    return(NULL);
}

/*****************************************************************************
 * Name: cbus_mci_txlimit
 *
 * Description:  Set the TX output queue limit for an interface.
 *
 *   If priority queueing is enabled, it sets the txlimit to a small
 *   number; 1 if < 512 KBPS, 2 until 1 MBPS, 3 for T-1, r for E-1,
 *   to keep enough messages in the controller to assure full utilization
 *   but a small enough number to make sorting (priority/custom/fair
 *   queuing) effective.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface 
 *     priority_list -- indicates if priority queuing is done on this interface
 *     mci_txcount -- the value to set the TX Limit to.  Note the problem
 *       that this is sometimes called txlimit and sometimes called txcount.
 *      
 * Return Value: None
 *
 *****************************************************************************/
void cbus_mci_txlimit (hwidbtype *hwidb)
{
    mciregtype *regptr;
    mcitype *mci;
    cdbtype *cdb;
    leveltype status;
    ulong tx_queue;

    if (hwidb->mci_txcount_enabled) {
	tx_queue = hwidb->mci_txcount;
    } else if (hwidb_use_short_tx_q_inline(hwidb)) {

	/*
	 * get the bandwidth of the link from the "bandwidth" command.
	 * set tx-queue-count to a depth related to the actual bandwidth
	 */
	tx_queue = 2 + hwidb->firstsw->visible_bandwidth / MCI_DOUBLE_BUFFER;
	if (tx_queue > hwidb->mci_txcount_default)
	    tx_queue = hwidb->mci_txcount_default;
    } else {
	tx_queue = hwidb->mci_txcount_default;
    }
    hwidb->mci_txcount = tx_queue;

    mci = hwidb->devctl;

    /*
     * Controllers only exist for cbus interfaces.
     */
    if (mci->flags & (CBUS | CBUSII)) {
	cdb = controller_find(hwidb->slot, hwidb->slotunit);
	if (cdb)
	    cdb->mci_txcount = tx_queue;
    }
    status = raise_interrupt_level(NETS_DISABLE);
    regptr = hwidb->mci_regptr;
    fci_select_if(hwidb, regptr);
    regptr->argreg = tx_queue;
    regptr->cmdreg = MCI_CMD_QUEUE;
    regptr->argreg = 2;            /* 7000 supports no other value */
    regptr->cmdreg = MCI_CMD_SLIMIT;
    reset_interrupt_level(status);
}




/*
 * set_cbus_txlimit
 *
 * for interfaces with defined transmit queue limits (interfaces with
 * supporting hardwarewhich has been configured with a tql), make sure
 * the tql matches current interface characteristics.
 *
 * this is called as a registry function, as it applies only to the RSP and
 * the 7000.
 */
static void set_cbus_txlimit (hwidbtype *hwidb)
{
    if (hwidb->mci_txcount_default && hwidb->mci_regptr)
 	cbus_mci_txlimit(hwidb);
}

#define PACKET_SNIFF_WORDS   (MCI_OFFSETSIZE + 14) /* thru end of IP hdr */
#define PAGE_WORDS           0x1000
#define PAGE_ONE             0x0010
#define PAGE_255             0x0FF0
#define PAGE_SHIFT           8
#define FATAL_ERR_OFFSET     1
#define FATAL_ERR_LOCK       0x8011
#define BUFHDR_START         0x0150
#define BUFHDR_PTR_OFFSET    6
#define BUFHDR_PG_OFFSET     7
#define BUFHDR_WORDS         8
#define BUFHDR_NEXTPTR_OFFSET (BUFHDR_PTR_OFFSET + BUFHDR_WORDS)

#ifdef C7000
#define ALU_REGISTERS        64
#define BUFHDRS              ((PAGE_ZERO_WORDS - BUFHDR_START) / BUFHDR_WORDS)
#define PAGE_ZERO_WORDS      PAGE_WORDS
#define MEMA_CONTROL_WORDS   0x2000
#else
/*
 * Don't support this on ags, so try to limit memory consumption.
 */
#define BUFHDRS              1
#define PAGE_ZERO_WORDS      1
#define MEMA_CONTROL_WORDS   1
#define ALU_REGISTERS        1
#endif

boolean cbus_capture_disabled;
static ushort cbus_registers[ALU_REGISTERS];
static ushort memd_page_zero[PAGE_ZERO_WORDS];
static ushort mema_buffer[MEMA_CONTROL_WORDS];
static ushort memd_packet_buffer[BUFHDRS * PACKET_SNIFF_WORDS];
static sys_timestamp cbus_capture_timestamp;
static ulong cbus_captures;
static char *cbus_capture_msg;

  
static void memd_lowcore_capture (mcitype *cbus, boolean fatal)
{
    mciregtype *regptr = cbus->regptr;
    int i, address, sniff, end;
    ulong ptr, page, memd_pages, start;
    boolean lovenote;
    leveltype level;
    
    if (cbus_capture_disabled)
	return;

    cbus_capture_msg = (fatal) ? "Core" : "Capture";

    cbus_captures++;
    GET_TIMESTAMP(cbus_capture_timestamp);

    memd_pages = cbus->memd_size / PAGE_WORDS;
    level = raise_interrupt_level(NETS_DISABLE);
 
    /* dump SP regs first, while closest to scene of the crime */
    for (i=0; i < SP_REGS; i++) {
	regptr->argreg = i;
 	regptr->cmdreg = SP_REGS_DUMP;
 	cbus_registers[i] = regptr->argreg;
    }

    start = fatal ? 0 : SP_CCB_WORD_OFFSET;
    regptr->largreg = start;
    regptr->cmdreg = FCI_CMD_MEMD_SELECT;
    for (i=start; i < PAGE_ZERO_WORDS; i++)
	memd_page_zero[i] = regptr->readshort;
    for (i=0; i < MEMA_CONTROL_WORDS; i++) {
	regptr->argreg = i;
	regptr->cmdreg = FCI_CMD_MEMA_SELECT;
	regptr->cmdreg = FCI_CMD_MEMA_READ;
	mema_buffer[i] = regptr->argreg;
    }
    sniff = 0;
    for (i=BUFHDR_START; i<PAGE_ZERO_WORDS; i+= BUFHDR_WORDS) {
	ptr  = memd_page_zero[i + BUFHDR_PTR_OFFSET];
	page = memd_page_zero[i + BUFHDR_PG_OFFSET];
	address = (page << PAGE_SHIFT) + ptr;
	lovenote = ((address == PAGE_ZERO_WORDS) &&
		    (memd_page_zero[i+BUFHDR_NEXTPTR_OFFSET] == 0));
	if ((page >= PAGE_ONE) && (page <= PAGE_255) && (ptr < PAGE_WORDS)
	    && !lovenote) {
	    regptr->largreg = address;
	    regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	    for (end = sniff + PACKET_SNIFF_WORDS; sniff < end; sniff++)
		memd_packet_buffer[sniff] = regptr->readshort;
	} else {
	    for (end = sniff + PACKET_SNIFF_WORDS; sniff < end; sniff++)
		memd_packet_buffer[sniff] = 0;
	}
    }
	
    cbus_capture_disabled = TRUE;
    
    reset_interrupt_level(level);
}

static boolean cbus_dump_mem (ushort *sptr, int start, int length)
{
    int i, j, data;
    
    for (i=start; i < start+length; i+=8) {
	if (automore_quit()) {
	    /* automore_disable(); */
	    return(FALSE);
	}
	for (j=i, data=0; j<i+8; j++)
	  data |= sptr[j];
	
	if (data)
	  printf("%04x: %04x %04x %04x %04x  %04x %04x %04x %04x\n",
		 i, sptr[i], sptr[i+1], sptr[i+2], sptr[i+3],
		 sptr[i+4], sptr[i+5], sptr[i+6], sptr[i+7]);
    }
    return(TRUE);
}

static void show_cbus_lowcore (void)
{
    int buf, sniff;
    char time[20];

    if (!cbus_captures) {
	printf("No memd captures or cores (CBUS-3-INITERR) recorded\n");
	return;
    }
    sprint_dhms(time, cbus_capture_timestamp);
    cbus_capture_disabled = FALSE;

    automore_enable(NULL);
    printf("cBus Memd %s (#%d, age %s)\n",
	   cbus_capture_msg, cbus_captures, time);
    
    printf("ALU Registers:\n");
    cbus_dump_mem(cbus_registers, 0, ALU_REGISTERS);
    
    printf("MEMA Contents:\n");
    cbus_dump_mem(mema_buffer, 0, MEMA_CONTROL_WORDS);
    
    printf("MEMD Page Zero:\n");
    cbus_dump_mem(memd_page_zero, 0, PAGE_ZERO_WORDS);
    
    printf("MEMD Buffers:\n");

    sniff = 0;
    for (buf = BUFHDR_START; buf < PAGE_ZERO_WORDS; buf += BUFHDR_WORDS,
	 sniff += PACKET_SNIFF_WORDS ) {
	
	if (!memd_page_zero[buf + BUFHDR_PG_OFFSET])
	    continue;
	printf("Buffer %04x: \n", buf);
	if (!cbus_dump_mem(&memd_packet_buffer[sniff], 0,
			   PACKET_SNIFF_WORDS))
	    break;
    }
    automore_disable();
}

static void show_cbus_lowcore_current (void)
{
    mcitype *cbus = (mcitype *) cbusQ.qhead;

    memd_lowcore_capture(cbus, FALSE);
    show_cbus_lowcore();
}

/*
 * Ask CBUS or MCI to reserve a transmit buffer.
 * Called (and returns) with interrupts disabled.
 * Returns MCI_RSP_OKAY if reservation successful.
 */
static ushort cbus_mci_txreserve (hwidbtype *hwidb, paktype *pak, boolean tx1)
{
    mcitype *mci;
    mciregtype *regptr;
    ushort cmd, response;
    int bp, size;
	    
    regptr = hwidb->mci_regptr;
    fci_select_if(hwidb, regptr);
    cmd = tx1 ? MCI_CMD_TX1_RESERVE : MCI_CMD_TX0_RESERVE;
    if (!(hwidb->status & IDB_CBUS)) {
	/*
	 * MCI
	 */
	regptr->argreg = hwidb->buffer_pool;
	regptr->cmdreg = cmd;
	return(regptr->cmdreg);
    }
    /*
     * CBUS
     *  
     * The mci->if_sizes[] array contains the size of the
     * buffers in the pool as they were allocated.
     * It doesn't include the encapsulation overhead
     * (MCI_OFFSETSIZE, or ~100 bytes), so adjust the outbound
     * datagramsize down by the amount of overhead, allowing 8 bytes
     * to provide guard against HSSI and FDDI DMA idiosyncracies.
     */
    size = pak->datagramsize - (MCI_OFFSETSIZE*2 - 8);
    
    response = FCI_RSP_ERR_NBF;
    mci = hwidb->devctl;
    for (bp = 0; bp < mci->if_sizes_count; bp++) {
	if (size > mci->if_sizes[bp])
	    continue;
	regptr->argreg = bp;
	regptr->cmdreg = cmd;
	if ((response = regptr->cmdreg) == MCI_RSP_OKAY)
	    break;
	/*
	 * If error code is 0x8007 (ERR_NBF), then the current pool
	 * is empty and we should try the next higher sized pool.
	 * For any other error (including txqueue full), there
	 * is no point in trying other buffer pools.
	 */
	if (response != FCI_RSP_ERR_NBF)
	    break;
    }
    return(response);
}
static void cbus_ipc_txstart (hwidbtype *hwidb,
			      paktype *pak,
			      boolean tx1,
			      leveltype level)
{
    mcitype *mci = hwidb->devctl;
    mciregtype *regptr = hwidb->mci_regptr;
    ushort bytes = pak->datagramsize;
 
    /* Use offset zero unconditionally.
     */
    regptr->argreg = 0;
    if (tx1) {
    /* We're at interrupt level or are guaranteeing sequenced
     * transport, so keep interrupts disabled (latter case) and
     * use transmit window 1.
     */
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    buffer2mci(pak->datagramstart, &regptr->write1long, pak->datagramsize);
    regptr->argreg = bytes;
    regptr->cmdreg = MCI_CMD_IPC1_START;
    } else {
    /* We are being called from process level (TX0, or window 0),
     * enable interrupts while copying data into the window.
     * Interrupt level calls use window 1 to avoid tromping on process
     * level activity. We increment cBus->critical to indicate that
     * the interrupt code is not allowed to reinitialize the
     * controller.  Lock out interrupts again, reselect the outgoing
     * interface, and start transmission.
     */
    regptr->cmdreg = MCI_CMD_TX0_SELECT;
    mci->critical++;
    hwidb->buffer2mci = TRUE;
    reset_interrupt_level(level);
    buffer2mci(pak->datagramstart, &regptr->write0long, pak->datagramsize);
    level = raise_interrupt_level(NETS_DISABLE);
    hwidb->buffer2mci = FALSE;
    mci->critical--;
    regptr->argreg = hwidb->mci_index;
    regptr->cmdreg = MCI_CMD_SELECT;
    regptr->argreg = bytes;
    regptr->cmdreg = MCI_CMD_IPC0_START;
    }
    TIMER_STOP(hwidb->xmittimer);
    reset_interrupt_level(level);
    datagram_done(pak);
    GET_TIMESTAMP(hwidb->lastoutput);
}

/*
 * Called with network interrupts disabled.
 * Account for broadcasts.
 * Copy packet from system buffer down to cbus/mci controller.
 * Issue start command.
 * Restore interrupt level to previous value.
 * Account for transmit packets and bytes.
 * Return system buffer.
 */
static void cbus_mci_txstart (hwidbtype *hwidb,
			      paktype *pak,
			      boolean tx1,
			      leveltype level)
{
    mcitype *mci = hwidb->devctl;
    mciregtype *regptr = hwidb->mci_regptr;
    ushort bytes = pak->datagramsize;

    if (hwidb->status & IDB_ETHER) {
	if (*pak->datagramstart & 0x01)
	    hwidb->counters.output_broad++;
	if ((hwidb->status & IDB_HEARSELF) && (pak->flags & PAK_HEARSELF))
	    bytes |= MCI_TX_HEARSELF;
	if (IS_ISL_VLAN_SUBINTERFACE(pak->if_output))
	    bytes |= 0x2000;
    } else if (hwidb->type == IDBTYPE_FDDI) {
	if (pak->datagramstart[1] & 0x80)
	    hwidb->counters.output_broad++;
    } else if (hwidb->type == IDBTYPE_FDDIT) {
	if (pak->datagramstart[1] & 0x01) {
	    hwidb->counters.output_broad++;
	}
    } else if (hwidb->status & IDB_TR) {
	if (pak->datagramstart[2] & 0x80)
	    hwidb->counters.output_broad++;
	if (pak->flags & (PAK_SRB_OUT | PAK_SRB_EXPLORER))
	    *pak->datagramstart = AC_PRI4;
#ifndef TR_FAST
	token_output_debug(hwidb, pak);
#endif
    }
    /* If FDDI, use start offset 1 because of a problem in the FCIT DMA
     * controller.  We have a problem if only 4 long words or fewer are
     * in the TX FIFO.  To make sure this straddles and becomes 5 words
     * we start at word offset 1.
     * If HSSI, use start offset 1 for possible transmitter delay.
     * If FSIP use start offset 0 to avoid underrun on 2 byte packets.
     */
    regptr->argreg = (hwidb->status & (IDB_HSSI|IDB_FDDI)) ? 1 : 0;
    if (tx1) {
	/* We're at interrupt level or are guaranteeing sequenced
	 * transport, so keep interrupts disabled (latter case) and
	 * use transmit window 1.
	 */
	regptr->cmdreg = MCI_CMD_TX1_SELECT;
	buffer2mci(pak->datagramstart, &regptr->write1long, pak->datagramsize);
	regptr->argreg = bytes;
	regptr->cmdreg = MCI_CMD_TX1_START;
    } else {
	/* We are being called from process level (TX0, or window 0),
	 * enable interrupts while copying data into the window.
	 * Interrupt level calls use window 1 to avoid tromping on process
	 * level activity. We increment cBus->critical to indicate that
	 * the interrupt code is not allowed to reinitialize the
	 * controller.  Lock out interrupts again, reselect the outgoing
	 * interface, and start transmission.
	 */
	regptr->cmdreg = MCI_CMD_TX0_SELECT;
	mci->critical++;
	hwidb->buffer2mci = TRUE;
	reset_interrupt_level(level);
	buffer2mci(pak->datagramstart, &regptr->write0long, pak->datagramsize);
	level = raise_interrupt_level(NETS_DISABLE);
	hwidb->buffer2mci = FALSE;
	mci->critical--;
	fci_select_if(hwidb, regptr);
	regptr->argreg = bytes;
	regptr->cmdreg = MCI_CMD_TX0_START;
    }
    TIMER_STOP(hwidb->xmittimer);
    reset_interrupt_level(level);
    hwidb->counters.tx_cumbytes += pak->datagramsize;
    datagram_done(pak);
    hwidb->counters.outputs++;
    GET_TIMESTAMP(hwidb->lastoutput);
}

/*
 * Called with network interrupts disabled.
 * If cbus, check for transmitter hang via timers and checks of
 *    various error codes, responding accordingly.
 * Restore interrupt level to previous value.
 */
static void cbus_mci_txerror (hwidbtype *hwidb, ushort rsp, boolean tx1)
{
    mcitype *mci;
    mciregtype *regptr;
    ushort response, sw_ver;
    boolean resetcbus;
    char *errstr;
    
    if (!(hwidb->status & IDB_CBUS))
	return;
    
    regptr = hwidb->mci_regptr;
    mci = hwidb->devctl;
    if (hwidb->buffer2mci) {
	/*
	 * This is interrupt level, and we've just interrupted a
	 * process level copy to a transmit window for this interface.
	 * Since something positive is going on for this interface,
	 * reset the timer.
	 */
	TIMER_STOP(hwidb->xmittimer);
    } else if (!TIMER_RUNNING(hwidb->xmittimer)) {
	/*
	 * This is the first reservation failure.
	 * Start a timer to catch transmit hangs.
	 */
	TIMER_START(hwidb->xmittimer, 3*ONESEC + hwidb->keep_period*ONESEC);
    } else if (AWAKE(hwidb->xmittimer)) {
	/*
	 * Transmit hang timer expired.
	 * If the tx queue was full and the interface doesn't respond
	 * to commands, then assume the microcode is hung and change
	 * the error code to TIMEOUT.
	 */
	TIMER_STOP(hwidb->xmittimer);
	if (rsp == FCI_RSP_ERR_QFL) {
	    response = send_sw_version_cmd (hwidb, &sw_ver);
	    if (response != MCI_RSP_OKAY)
	      rsp = response;
	}
	/*
	 * Branch on error code.
	 */
	switch (rsp) {
	case FCI_RSP_ERR_QFL:
	    /* Transmit queue is full, so just reset the interface.
	     * This should get a hung transmitter to restart, and will
	     * at least return the interface's current transmit queue
	     * buffers to the free queue (unless it's old style MEC
	     * microcode, in which case the buffers stay there.
	     */
	    errstr = "queue full";
	    resetcbus = FALSE;
	    break;
	case FCI_RSP_ERR_NBF:
	    /* All free queues of size big enough for this buffer
	     * are empty over a long period of time.  This shouldn't
	     * Assume the worst (MEMD buffer corruption) and reset cbus.
	     */
	    errstr = "free queues empty";
	    resetcbus = TRUE;
	    break;
	case FCI_RSP_ERR_BFR:
	    /* Some portion of the system code forgot to issue a
	     * TX?START or AUTO_FLOOD after doing a TX?RESERVE,
	     * so the transmit window never gets flushed.
	     * This is an unrecoverable error.
	     */
	    errstr = "old buffer in window";
	    resetcbus = TRUE;
	    break;
	case FCI_RSP_ERR_TMOUT:
	    /* The microcode on the interface isn't responding to
	     * simple commands after a queue full error.  Assume
	     * it's microcode is hung and reset cbus.
	     */
	    errstr = "interface ucode timeout";
	    resetcbus = TRUE;
	    break;
	default:
	    errstr = "";
	    resetcbus = TRUE;
	    break;
	}
	errmsg(&msgsym(OUTHUNG, CBUS),
	       hwidb->hw_namestring,
	       tx1 ? '1' : '0',
	       rsp,
	       errstr,
	       resetcbus ? "cBus" : common_str_interface);
	GET_TIMESTAMP(hwidb->lastreset);
	if (resetcbus) {
 	    if (hwidb->status & IDB_CBUS)
 	      memd_lowcore_capture(mci, TRUE);
	    if (mci->critical)
		mci->restartdeferred = TRUE;
	    else
		(void) mci_init(mci, FALSE);
	    cbus_restarts_output++;
	} else {
	    (*hwidb->reset)(hwidb);
	}
    }
}

/*****************************************************************************
 * Name: cbus_mci_enable
 *
 * Description: Enable an cBus or MCI ethernet interface.
 *   This function is used in the "hwidb->enable" vector in the IDB.
 *   It was most likely shut down for input congestion. 
 *
 *   NOTE: this function must be called with NETS_DISABLED, to work correctly.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
static void cbus_mci_enable (hwidbtype *hwidb)
{
    ushort throttle_state;
    cdbtype *cdb;
    hwidbtype *temp_hwidb;
    platform_hware_handle_type *hware_handle;
    
    if (hwidb->status & (IDB_MCI | IDB_CBUS)) {
	select_if(hwidb);
	/*
	 * if this is a cBus, we send a throttle command to toggle the
	 * throttle state of the interface, and if we toggled it to the
	 * throttled state, we send another throttle command to toggle it to
	 * the UNthrottled state.
	 */
	if (hwidb->status & IDB_CBUS) {
	    send_if_throttle_cmd(hwidb,&throttle_state);
	    if (throttle_state == 0) {
		send_if_throttle_cmd(hwidb,&throttle_state);
	    }
	    cdb = controller_find(hwidb->slot, hwidb->slotunit);
	    if (cdb == NULL) {
	        /* this should never happen */
	        idb_mark_unthrottled(hwidb);
	        return;
	    }
	    cdb->input_throttled = FALSE;

	    /*
	     *  Now mark all IDBs
	     */
	    hware_handle = cdb->platform_hware_handle;
	    for (temp_hwidb = hware_handle->interface_list[cdb->unit]
		     ; temp_hwidb != NULL
		     ; temp_hwidb = temp_hwidb->next_idb_on_physint) {
	        idb_mark_unthrottled(temp_hwidb);
	
	        /*
	         * We have been throttling this interface -- fool keepalives
	         * into thinking that keepalives are still functioning.
	         */
	        GET_TIMESTAMP(temp_hwidb->mci_lineuptimer);
	    }
	} else {
	    reg_invoke_media_transmit_delay(hwidb->type, hwidb, 
					    hwidb->xmitdelay);
	    send_if_enable_cmd(hwidb);
	}
	idb_mark_unthrottled(hwidb);
	
	/*
	 * We have been throttling this interface -- fool keepalives
	 * into thinking that keepalives are still functioning.
	 */
	GET_TIMESTAMP(hwidb->mci_lineuptimer);
    }
}

/*
 * cbus_mci_output
 * Start transmission using the specified transmit window.
 * Used by cBus and MCI ethernet, serial, tokenring, and fddi interfaces.
 * Window 0 is used by process level, window 1 is used by interrupt level.
 * Keep outputting until hold queue empties or an error occurs.
 */
void cbus_mci_output (hwidbtype *hwidb)
{
    paktype *pak;
    leveltype level;
    ushort rsp;
    boolean tx1;

    if (hwidb->status & IDB_NOHARDWARE) {
	holdq_reset(hwidb);
	return;
    }

    /*
     * As long as we have tx buffers on our hold queue and there
     * is room on the outbound interface's MCI/CBUS transmit queue,
     * copy packets down to the controller.
     * We do a quick check of hold queue counter without interrupt
     * interlock to save overhead of setlevel and holdq_dequeue after
     * last transmit.
     */
    while (TRUE) {
	/*
	 * Lock out interrupts and get next pkt from tx hold queue.
	 */
        level = raise_interrupt_level(NETS_DISABLE);
        pak = (*hwidb->oqueue_dequeue)(hwidb);
        if (pak == NULL) {
            reset_interrupt_level(level);
	    break;
        }

	/*
	 * Make sure buffer has legal length.
	 */
	if (pak->datagramsize > hwidb->max_pak_size) {
  	    errmsg(&msgsym(TOOBIG, LINK), hwidb->hw_namestring,
		   pak->datagramsize);
	    datagram_done(pak);
	    reset_interrupt_level(level);
	    continue;
	}

	/*
	 * If at interrupt level, or we need to guarantee sequencing,
	 * use transit window 1, otherwise use window 0.
	 */
	tx1 = mci_cbus_intflag || (pak->linktype == LINK_RSRB) ||
	    (pak->linktype == LINK_STUN) || (pak->linktype == LINK_BRIDGE) ||
	    (pak->linktype == LINK_SRB);
	
	if ((rsp = cbus_mci_txreserve(hwidb, pak, tx1)) != MCI_RSP_OKAY) {
	    (void) pak_requeue_head(hwidb, pak);
	    cbus_mci_txerror(hwidb, rsp, tx1);
	    reset_interrupt_level(level);
	    break;
	}

	/* Copy the packet to controller and issue start command.
	 * Return packet to free queue, do accounting,
	 * re-enable interrupts (if at process level),
	 * and do modest accounting.
	 */
        if (pak->linktype != LINK_IPC)
            cbus_mci_txstart(hwidb, pak, tx1, level);
        else
            cbus_ipc_txstart(hwidb, pak, tx1, level);
    }
    
}

/*****************************************************************************
 * Name: cbus_mci_idb_typestring
 *
 * Description: Return point to type string
 *   This function is used in the "hwidb->typestring" vector in the IDB.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: 
 *   typestring -- pointer to the typestring for the interface
 *
 *****************************************************************************/
char *cbus_mci_idb_typestring (hwidbtype *idb)
{
    ulong status = idb->status;

    if ((status & (IDB_MCI|IDB_ETHER)) == (IDB_MCI|IDB_ETHER))
	return("MCI Ethernet");

#if defined(C7000)
    if ((status & (IDB_MCI|IDB_SERIAL)) == (IDB_MCI|IDB_SERIAL))
	return("SwBus Serial");

    if (status & IDB_CBUS) {
	cdbtype *cdb;
	
	if ((cdb = controller_find(idb->slot, idb->slotunit)) != NULL) {
	    switch (cdb->pif_type) {
	    case PIFT_ETHERNET:		return "cxBus Ethernet";
	    case PIFT_TOKENRING:	return "cxBus Token Ring";
	    case PIFT_SERIAL:		return "cxBus Serial";
	    case PIFT_HSSI:		return "cxBus HSSI";
	    case PIFT_T1:		return "cxBus T1";
	    case PIFT_E1:		return "cxBus E1";
	    case PIFT_FDDI:		return "cxBus FDDI";
	    case PIFT_ATM:		return "cxBus ATM";
	    case PIFT_CHANNEL:          return "cxBus IBM Channel"; /* cybus */
	    case PIFT_FASTETHER:        return "cxBus FastEthernet";/* cybus */
	    case PIFT_UNKNOWN:	/* make gcc be quiet */
		break;
	    }
	}
    }
#else /* must be an AGS */
    if ((status & (IDB_MCI|IDB_SERIAL)) == (IDB_MCI|IDB_SERIAL))
	return("MCI Serial");

    if (status & IDB_CBUS) {
	if (status & IDB_FDDI)
	    return("cBus Fddi");
	if (status & IDB_ETHER)
	    return("cBus Ethernet");
	if (status & IDB_TR)
	    return("cBus Token Ring");
	if (status & IDB_SERIAL)
	    return((status & IDB_HSSI) ? "cBus HSSI" : "cBus Serial");
    }
#endif
    return("unknown");
}

/*****************************************************************************
 * Name: dci_add_interface
 *
 * Description: Add an interface IDB to the end of a list of a dci device IDBs.
 *   If the IDB is already on the list, nothing happens.
 *
 * Input: 
 *   mci -- pointer to the mci structure for the controller
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value:  None
 *
 *****************************************************************************/
void dci_add_interface (mcitype *mci, int unit, hwidbtype *hwidb)
{
    dcitype *dci;
    int slot, slotunit;
    
    /*
     * If cBus type interface -- add idb to daughter controller structure.
     */
    if (mci->flags & (CBUS | CBUSII)) {
	cbus_unit2slotunit(unit, &slot, &slotunit);
	dci = mci->dci[slot];

	/*
	 *  The dci list is the exact same list as the mci list.  If this
	 *  is not the first item on the list, then we need do nothing more.
	 *  If it is the first, we must set the dci's list pointer to point
	 *  to it and bump the count.  (If this is not the first, then we
	 *  are automagically on the dci's list, via the next_idb_on_physint
	 *  pointer chain we set up above.
	 *
	 *  VIP TODO - change comment to reflect reality
	 */
	if (mci->interface_list[unit] == hwidb) {
	    /*
	     *	One thing to look out for--this might already be on
	     *  one of our lists
	     */
	    int ix;

	    for (ix = 0 ; ix < MAX_CTRLR_INTERFACES; ix++) {
		if (dci->interface_list[ix] == hwidb)
		    return;	/* yup, we already had it */
	    }
	    dci->interface_list[slotunit] = hwidb;
	    dci->interface_count++;
	}
    }
}

/*****************************************************************************
 * Name: mci_add_interface
 *
 * Description: Add an interface IDB to the end of a list of a mci device IDBs.
 *   If the IDB is already on the list, nothing happens.
 *
 * Input: 
 *   mci -- pointer to the mci structure for the controller
 *   unit -- unit number of the interface list
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void mci_add_interface (mcitype *mci, int unit, hwidbtype *hwidb)
{
    if (mci->interface_list[unit] == NULL) {
	mci->interface_list[unit] = hwidb;
    } else {
	hwidbtype *tmp;

	for (tmp = mci->interface_list[unit]
	     ; tmp->next_idb_on_physint != NULL && tmp != hwidb
	     ; tmp = tmp->next_idb_on_physint)
	    ;
	if (tmp == hwidb)
	    return;	/* was already on the list */
	tmp->next_idb_on_physint = hwidb;
    }
    hwidb->next_idb_on_physint = NULL;  /* this is necessary for MIP
					 after ucode reload */ 
}




static inline int cbus_get_pa_from_slotunit (int slotunit)
{
    return (slotunit/8);
}

/*
 * cbus_mci_XXX_idb_init:
 * Media specific (XXX) IDB inits.
 */
static inline void cbus_mci_ether_idb_init (mcitype	*mci,
					    hwidbtype	*idb,
					    boolean	is_vip,
					    boolean	is_fe)
{
    ethersb *esb;

    if ((mci->flags & (CBUS | CBUSII))) {
	idb->status = IDB_ETHER | FCI_IDBFLAGS;
	if (is_vip) {
	    /*
	     * VIP style ethernet interfaces are incapable
	     * of hearing themselves, so turn off IDB_HEARSELF.
	     * Also set IDB_VIP in idb->status.
	     */
	    idb->status &= ~IDB_HEARSELF;
	    idb->status |= IDB_VIP;
	}
	idb->type = is_fe ? IDBTYPE_FEIP : IDBTYPE_CBUSETHER;
	reg_add_media_transmit_delay(idb->type,
				     cbus_set_transmit_delay,
				     "cbus_set_transmit_delay");
    } else {
	idb->status = IDB_ETHER | MCI_IDBFLAGS;
	idb->type = IDBTYPE_MCIETHER;
	idb->txqlength = mci_txqcount;
	reg_add_media_transmit_delay(IDBTYPE_MCIETHER,
				     mci_set_transmit_delay,
				     "mci_set_transmit_delay");
    }
    if (is_fe) {
	mci->feip_count++;
	ether_idb_init(idb);
	idb->unit = ++cbus_base_feth;
	esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);
	esb->media_type = FEIP_RJ45;
	idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
	nfethernets++;
    } else {
	mci->ether_count++;
	ether_idb_init(idb);
	idb->unit = ++cbus_mci_base_ether;
    }
    /*
     * Count nethernets for both ethernet types (fast & non-fast)
     */
    nethernets++;
    if (is_vip || is_fe) {
	idb->reset = is_vip ? cbus_pas_ethernet_reset : cbus_feip_reset;
	idb->listen = cbus_record_pas_ethernet_address;
	idb->listen_range = cbus_record_pas_ethernet_range;
	idb->afilter_reset = pas_ethernet_af_init;
    } else {
	idb->reset = cbus_mci_ether_reset;
	idb->listen = cbus_mci_record_ether_address;
	idb->listen_range = cbus_mci_record_ether_range;
    }
    idb->firstsw->tbridge_media = TBR_ETHER;
    idb->periodic = cbus_mci_ether_periodic;
    idb->rxoffset = MCI_ETHER_OFFSET;
    idb->span_macoffset = MCI_ETHER_OFFSET;
    idb->span_encapsize = 0;	/* Pure transparent bridging */
    idb->rsrb_vringoffset = MCI_ETHER_OFFSET + ETHER_SNAP_ENCAPBYTES/2;
}


static inline void cbus_mci_serial_idb_init (mcitype	*mci,
					     hwidbtype	*idb,
					     int       capability,
					     boolean	is_vip)
{
    mciregtype *regptr;
    dcitype *dci;
    ushort result;
    serialsb *ssb;
    
    if (mci->flags & (CBUS | CBUSII)) {
	idb->status = IDB_SERIAL | IDB_SYNC | FCI_IDBFLAGS;
	dci = mci->dci[idb->cbus_ctrlr_index];
	/* Note that serial_idb_init() will set the default value
	 * for numerous idb fields, part of them might be OVERWRITTEN
	 * for specfic kind of interface in the codes that follow. 
	 * FCI_MIP_CONTROLLER below is an example.
	 * So do not move the call of serial_idb_init() without considering
	 * these or the codes will be broken.
	 */
	serial_idb_init(idb, MAXSERIALSIZE, MCI_MAXDGRAM_DEF,
			VISIBLE_MCI_SERIAL_BANDFAST, MCI_SERIAL_DELAY);
	if (dci->ctrlr_type == FCI_MIP_CONTROLLER) {
	    /*
	     * Everything was done in cdb->idb_init()
	     *
	     * Due to the fact that the MIP never refuses a message,
	     * but instead accepts it and discards it if the queue
	     * turns out to be full, existing Priority, Custom, and Fair
	     * Queuing code won't work with it. Therefore, set the
	     * queuing algorithm to First Come First Serve.
	     *
	     * note for the future: it would be nice to have a simple
	     * way to tell from the IDB what kind fo controller is in
	     * use; then we could make the queuing interface configuration
	     * commands not show up.
	     */
	    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);
	    ssb->serial_restartdelay = ssb->serial_restartdelay_def = 0;
	    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
	    delete_fair_queue(idb);
	} else {
	    idb->type = IDBTYPE_CBUSSERIAL;
	    idb->show_serial_state = fsip_show_serial_state;
	    idb->reset = cbus_mci_serial_reset;
	    idb->unit = ++cbus_mci_base_serial;
	    idb->serial_flags |= SERIAL_FLAGS_CAN_ALT_FLAG |
	      SERIAL_FLAGS_CAN_CLK_INV | SERIAL_FLAGS_CAN_NRZI |
		  SERIAL_FLAGS_CAN_IGNORE_DCD | SERIAL_FLAGS_CAN_CRC_32;
	    idb->serial_flags &= ~SERIAL_FLAGS_NO_CLOCKRATE;
	    reg_add_media_transmit_delay(IDBTYPE_CBUSSERIAL,
					 cbus_set_transmit_delay,
					 "cbus_set_transmit_delay");
	    if (is_vip) {
		idb->status |= IDB_VIP;
		reg_invoke_priv_pa_info_op(idb, TRUE, idb->slot);
	    } else {
		/* put non-vip cbus_serial specific init here */
		idb->txqlength = fsip_get_txqlength;
	    }
	}
    } else {
	idb->status = IDB_SERIAL | IDB_SYNC | MCI_IDBFLAGS;
	idb->type = IDBTYPE_MCISERIAL;
	if (capability & MCI_CAP_SPEED)
	  serial_idb_init(idb, MAXSERIALSIZE, MCI_MAXDGRAM_DEF,
			  VISIBLE_MCI_SERIAL_BANDFAST, MCI_SERIAL_DELAY);
	else
	  serial_idb_init(idb, MAXSERIALSIZE, MCI_MAXDGRAM_DEF,
			  VISIBLE_MCI_SERIAL_BANDSLOW, MCI_SERIAL_DELAY);
	idb->txqlength = mci_txqcount;
	idb->reset = cbus_mci_serial_reset;
	idb->unit = ++cbus_mci_base_serial;
	idb->serial_flags |= SERIAL_FLAGS_CAN_INT_TX |
	  SERIAL_FLAGS_CAN_CLK_INV | SERIAL_FLAGS_CAN_NRZI;
	reg_add_media_transmit_delay(IDBTYPE_MCISERIAL,
				     mci_set_transmit_delay,
				     "mci_set_transmit_delay");
	/* Figure out whether the interface is HDX capable, MCI only */
	idb->sdlc_flags = 0;
        idb->hdx_flags = 0;
	regptr = (mciregtype *) mci->regptr;
	regptr->argreg = idb->mci_index;
	regptr->cmdreg = MCI_CMD_SELECT;
	regptr->argreg = MCI_IF_HDX;
	regptr->cmdreg = MCI_CMD_STATUS;
	result = regptr->cmdreg;
	if (result == MCI_RSP_OKAY) {
	  idb->hdx_flags |= HDX_FLAG_CAPABLE;
          reg_add_media_half_or_full_duplex_cmd(IDBTYPE_MCISERIAL, 
                                                serial_half_duplex_command,
                                               "serial_half_duplex_command");
        }
	
    }

    hes_serial_idb_init(idb);

    idb->listen = (listen_t)return_nothing;
    idb->firstsw->ip_mtu = MAXSERIALSIZE;
    idb->set_maxdgram = cbus_mci_set_maxdgram;
    idb->buffersize = MCI_BUFFERSIZE_BYTES;
    idb->rxoffset = MCI_SERIAL_OFFSET;
    idb->span_macoffset = MCI_SERIAL_OFFSET + HDLC_ENCAPBYTES/2;
    idb->span_encapsize = HDLC_ENCAPBYTES;
    idb->rsrb_vringoffset = MCI_SERIAL_OFFSET + HDLC_ENCAPBYTES/2;
    TIMER_STOP(idb->sdlc_top_timer);
    idb->sdlc_slow_poll_interval = SDLC_DEFAULT_SLOW_POLL;

    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);
    ssb->serial_clockindex = 0;
    ssb->serial_protocol = capability;
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);

    nserialnets++;
}


static inline boolean cbus_mci_fddi_idb_init (mcitype	*mci,
					      hwidbtype	*idb)
{
    dcitype  *dci;
    
    nfddinets++;
    mci->fddi_count++;
    hes_fddi_idb_init(idb);
    dci = mci->dci[idb->cbus_ctrlr_index];
    idb->status = IDB_FDDI | (FCI_IDBFLAGS & ~IDB_HEARSELF);
    if (FCITYPE(idb->fci_type) == FCI_TYPE_FDDIT) {

	/*
	 * FDDIT.  Default to Translational (transparent)
	 */
	idb->type = IDBTYPE_FDDIT;
	idb->span_macoffset = FDDI_RECEIVE_OFFSET + 1;
	idb->span_encapsize = FDDI_FC; /* one byte of preamble before MAC */
	idb->firstsw->tbridge_media = TBR_FDDI;
	idb->fddi_options |= FDDI_FAST_CMT_ENB;
	idb->fddi_options |= FDDI_FAST_CMT;
    } else {
	idb->type = IDBTYPE_FDDI;
	idb->fddi_options |= FDDI_ENCAPSULATE;
	idb->span_macoffset = FDDI_RECEIVE_OFFSET + FDDI_SNAP_ENCAPWORDS;
	idb->span_encapsize = FDDI_SNAP_ENCAPBYTES;
	idb->firstsw->tbridge_media = TBR_FDDI_TRANSIT;
	if (((dci->sw_version & 0xFF00) >= 0x0200) &&
	    ((dci->sw_version & 0xFF00) != 0x8000)){
	    idb->fddi_options |= FDDI_FAST_CMT_ENB;
	    idb->fddi_options |= FDDI_FAST_CMT;
	} else {
	    return TRUE;
	}
    }
    
    /*
     * DON'T init things here that are common with RSP/FIP init.
     * put it somewhere like hes_fddi_idb_init()
     */
    idb->soutput = cbus_mci_output;
    idb->reset = fddi_reset;
    idb->system_configured = fddi_system_configured;
    idb->periodic = fddi_periodic;
    idb->unit = ++cbus_base_fddi;
    idb->rxoffset = FDDI_RECEIVE_OFFSET;
    reg_add_media_minimum_carrier_delay(idb->type, (2 * ONESEC),
					"fddi min carrier delay");

    reg_add_setup_srb(idb->type, fddi_reset, "fddi_reset");

    idb->rsrb_vringoffset = FDDI_RECEIVE_OFFSET + FDDI_SNAP_ENCAPWORDS;

    return FALSE;
}


static inline void cbus_mci_tr_idb_init (mcitype	*mci,
					 hwidbtype	*idb,
					 hwidbtype	*setup_idb,
					 boolean	is_vip)
{
    tokensb  *tsb;

    /*
     * Assume 16Mbps se we can go ahead and initialize this guy.  We
     * will set the actual ring speed once we have read it from the
     * configuration file, before we ever attempt to insert into the
     * ring.
     */
    nTRs++;
    mci->token_count++;
    idb->type = IDBTYPE_CTR;
    tring_idb_init(idb, 16);
    idb->tr_ring_speed = 0;
    idb->status = IDB_TR | FCI_IDBFLAGS;
    
    set_default_queueing(idb);
    idb->shutdown = cbus_token_shutdown;
    idb->iqueue = raw_enqueue;
    if (is_vip) {
	idb->status |= IDB_VIP;
	reg_invoke_hwidb_setup_routine(idb->type, setup_idb); 
    } else {
	if (!idb->ctr_private) {
	    idb->ctr_private = malloc_named(sizeof(spyglass_data_t), "TRIP Data");
	}
	idb->reset = (reset_t)cbus_token_reset;
	idb->system_configured = (system_configured_t)cbus_token_reset;
	idb->periodic = cbus_token_periodic;
	idb->shutdown = cbus_token_shutdown;
	idb->dot5_get_smt_stats = ctr_get_mib_stats;
	idb->dot5_add_counters  = ctr_update_prev_stats;
	idb->dot5_soft_reset    = cbus_token_reset;
	idb->show_controller = ctr_show_controller;
        idb->set_extra_addrs = ctr_set_addresses;
	tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
	tsb->tr_polling_count = TR_POLLING_PERIOD;
	tsb->tr_polling_state = TR_POLLING_OK;
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	reg_add_if_onesec(ctr_deadman_poll, "ctr_deadman_poll");
    }
    
    idb->listen = (listen_t)return_nothing;
    idb->unit = ++cbus_base_token;
    if_maxdgram_adjust(idb, CTR_BUFFERSIZE_BYTES);
    idb->buffersize = (CTR_BUFFERSIZE_WORDS << 1);
    idb->rxoffset = CTR_RECEIVE_OFFSET;
    idb->span_macoffset = CTR_RECEIVE_OFFSET + 1;	/* in shorts */
    idb->span_encapsize = TR_ACFC_SIZE;/* 2 bytes of preamble before MAC */
    idb->firstsw->tbridge_media = TBR_CTR;
    idb->cbus_burst_count = idb->cbus_burst_count_def =
      CTR_BURST_COUNT_DEF;
    reg_add_media_transmit_delay(IDBTYPE_CTR,
				 cbus_set_transmit_delay,
				 "cbus_set_transmit_delay");

    reg_add_setup_srb(idb->type, ctr_setup, "ctr_setup");
    reg_add_modify_srb(idb->type, ctr_modify, "ctr_modify");

    idb->rsrb_vringoffset = CTR_RECEIVE_OFFSET + TRING_SNAP_ENCAPBYTES/2;
    idb->srb_routecache = TRUE;
    idb->tr_srb_fastswitch = (tr_srb_fastswitch_type) return_false;

}

static inline void cbus_mci_hssi_idb_init (mcitype	*mci,
					   hwidbtype	*idb,
					   int         capability)
{
    serialsb *ssb;

    idb->status = IDB_SERIAL | FCI_IDBFLAGS | IDB_HSSI | IDB_SYNC;
    idb->type = IDBTYPE_HSSI;
    idb->serial_flags |= SERIAL_FLAGS_CAN_CRC_32;
    idb->serial_flags |= SERIAL_FLAGS_NO_CLOCKRATE;
    idb->reset = cbus_mci_serial_reset;
    idb->listen = (listen_t)return_nothing;
    serial_idb_init(idb, MAXDS3SIZE, (MAXDS3SIZE + DS3_OVERHEAD_BYTES),
		    VISIBLE_FCI_SERIAL_BANDDS3, HSSI_SERIAL_DELAY);

    hes_serial_idb_init(idb);

    /*
     * idb->set_maxdgram must be initialized after calling serial_idb_init
     * so that it does not attempt to call cbus_mci_set_maxdgram()
     * while doing system init
     */
    idb->set_maxdgram = cbus_mci_set_maxdgram;
    idb->buffersize = (MAXDS3SIZE + DS3_BAGGAGE);
    idb->unit = ++cbus_base_hssi;
    
    idb->rxoffset = MCI_SERIAL_OFFSET;
    idb->span_macoffset = MCI_SERIAL_OFFSET + HDLC_ENCAPBYTES/2;
    idb->rsrb_vringoffset = MCI_SERIAL_OFFSET + HDLC_ENCAPBYTES/2;
    idb->cbus_burst_count = idb->cbus_burst_count_def =
      HSCI_BURST_COUNT_DEF;
    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);
    ssb->hssi_lc_inhibit = TRUE;  /* Inhibit by default */
    ssb->serial_clockindex = 0;
    ssb->serial_protocol = capability;
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
    nhssinets++;
    mci->hssi_count++;
    reg_add_media_transmit_delay(IDBTYPE_HSSI,
				 cbus_set_transmit_delay,
				 "cbus_set_transmit_delay");
}


static inline void cbus_mci_aip_idb_init (mcitype	*mci,
					  hwidbtype	*idb,
					  hwidbtype	*setup_idb)
{
    idb->status = IDB_ATM | FCI_IDBFLAGS | IDB_SYNC;
    idb->status &= ~IDB_BCAST;
    idb->type = IDBTYPE_AIP;
    idb->set_maxdgram = cbus_mci_set_maxdgram;
    idb->maxmtu_def = idb->maxmtu = ATM_DEFAULT_MTU;
    idb->firstsw->sub_mtu = idb->maxmtu_def;

    idb->name = IFNAME_ATM;
    if (reg_invoke_media_atm_setup(setup_idb)) {
	reg_invoke_media_hw_setup(setup_idb);
    }
    /*
     * Always assign the MAC address for AIP.  Used by LANE.
     */
    reg_invoke_assign_mac_addr(idb);
    if (is_ieee_zero(&idb->hardware[0])) {
	ieee_copy(idb->bia, idb->hardware);
    }
    idb->rxoffset = MCI_OFFSETSIZE - btow(idb->encsize) ;
    idb->unit = ++cbus_base_aip;
    idb->span_macoffset = idb->rxoffset;
    idb->span_encapsize = idb->encsize;
    idb->firstsw->tbridge_media = TBR_ATM_TRANSIT;
    idb->bridge_transit = TRUE;
    natmnets++;
    mci->aip_count++;
}

static inline void cbus_mci_cip_idb_init (mcitype	*mci,
					  hwidbtype	*idb,
					  hwidbtype	*setup_idb)
{
    idb->type = IDBTYPE_CIP;
    idb->status = IDB_CHANNEL | FCI_IDBFLAGS;
    
    /* CHANNELTODO - find best value for this */
    idb->max_pak_size_def = FDDI_MAXDGRAM;
    if_maxdgram_adjust(idb, FDDI_MAXDGRAM); 
    idb->set_maxdgram = cbus_mci_set_maxdgram;
    idb->maxmtu_def = idb->maxmtu = CIP_DEFAULT_MTU;
    idb->firstsw->sub_mtu = idb->maxmtu_def;

    idb->name = IFNAME_CHANNEL;
    reg_invoke_hwidb_setup_routine(idb->type, setup_idb); 
    idb->rxoffset = MCI_OFFSETSIZE - btow(idb->encsize);
    idb->unit = ++cbus_base_channel;
    idb->span_macoffset = idb->rxoffset;
    idb->span_encapsize = sizeof(channel_hdr); /* only used by vlan code */
    nchannelnets++;
    mci->cip_count++;
}


static inline void cbus_mci_led_init (mcitype *mci, int unit)
{
    int u = (unit << 2);

    mci->led_mask[unit] = (0xF << u);
    mci->led_carrier[unit] = (1 << u);
    mci->led_transmit[unit] = (2 << u);
    mci->led_receive[unit] = (4 << u);
    mci->led_error[unit] = (8 << u);
}


/*****************************************************************************
 * Name: cbus_mci_idb_init
 *
 * Description: Common code to initialize a cBus or MCI interface descriptor.
 *   This function does *not* initialize the hardware.
 *
 * Input: 
 *   mci -- pointer to the mci structure for the controller
 *   unit -- unit number of the interface list (what is the "unit")
 *   interfacetype -- the interface type returned from the IP when the
 *     select command is issued (this the FCI_TYPE_xxx value)
 *   oldidb -- pointer to the old HW IDB for the interface.  This is
 *     non-null when we are re-initializing an interface that we
 *     previously removed, and has now been re-inserted.
 *   newidb -- pointer to the new HW IDB for the interface -- if this
 *     is a NULL, a new IDB is malloc'ed.
 *
 * Return Value: 
 *   boolean -- TRUE = initialization was *UNSUCCESSFUL*, otherwise FALSE.
 *     (No, that is not a typo.)
 *
 *****************************************************************************/
boolean cbus_mci_idb_init (mcitype	*mci,
			   int		unit,
			   int		interfacetype,
			   hwidbtype	*oldidb,
			   hwidbtype	*newidb)
{
    int type, capability;
    hwidbtype *idb;
    hwidbtype *setup_idb;
    cdbtype *cdb;
    dcitype *dci;
    mciregtype *regptr;
    boolean nomalloc = (get_interrupt_level() && systeminit_flag) ||
      onintstack();
    boolean is_vip = FALSE, is_fe = FALSE, is_cip = FALSE, is_feip = FALSE;
    int slot, slotunit;
#ifdef C7000
    pa_info_slots *pa_info_ptr;
#endif

    type = (interfacetype & 0xFF);
    capability = (interfacetype >> 8);
    
    /* 
     * Create an interface descriptor block
     */
    if (newidb == NULL) {
	if (nomalloc) {
	    idb = dequeue(&idbpreQ);
	} else {
	    idb = idb_create();
	}
	if (!idb) {
	    return(TRUE);
	}
    } else
	idb = newidb;
    
    /*
     * Set up common fields.
     */
    hes_idb_init_before(idb);

    /*
     * Set up fields based on input parameters.
     */
    idb->devctl = mci;
    idb->mci_index = unit;
    idb->mci_regptr = mci->regptr;
    /*
     * mci_testptr is always equal to mci_regptr, except when the interface
     * is configured for DDR, in which case it is NULL to enable DDR
     * fastswitch.
     */
    if (is_ddr(idb))
	idb->mci_testptr = NULL;
    else
	idb->mci_testptr = mci->regptr;
    idb->fci_type = type;
    if (oir_debug) {
	buginf("\nSetting pre_hotstall_state=IDBS_ADMINDOWN for unit=%d, type=%d, name=%s, slot=%d\n newidb=%#x, oldidb=%#x",
	       unit, interfacetype, idb->hw_namestring, idb->slot,
	       newidb, oldidb);
	if (oldidb) {
	    buginf(", old name=%s, old slot=%d",
		   oldidb->hw_namestring, oldidb->slot);
	}
	bugtrace();
    }
    idb->pre_hotstall_state = IDBS_ADMINDOWN;
    idb->oir_flags = (oldidb == NULL ? IDB_NEEDS_INIT : 0); 
				/* not newly inserted if already existed  */

    cbus_mci_led_init(mci, unit);

    /*
     * Setup some special cells if we are a cBus style interface.
     */
    if (mci->flags & (CBUS | CBUSII)) {
 	if (mci->flags & CBUSII) {
	    cbus_unit2slotunit(unit, &slot, &slotunit);
     	    idb->cbus_ctrlr_index = slot;
 	    regptr = (mciregtype *) mci->regptr;

	    /*
	     * Ask the ucode for the MEMD address of this interface.
	     */
 	    regptr->argreg = unit;
 	    regptr->cmdreg = FCI_CMD_GET_ICB;
 	    if (regptr->cmdreg == FCI_RSP_OKAY)
 		idb->cbus_icb_address = regptr->argreg;
 	    else
 		idb->cbus_icb_address = CBUS2_ICB_BASE + 
 		    (unit * CBUS2_ICB_SIZE);
	    idb->slot = slot;
	    idb->slotunit = slotunit;
 	} else {
 	    idb->cbus_ctrlr_index = (unit / CBUS1_MAX_CTRLR_INTERFACES);
 	    idb->cbus_icb_address = CBUS1_ICB_BASE + (unit * CBUS1_ICB_SIZE);
	    idb->slot = unit / CBUS1_MAX_CTRLR_INTERFACES;
	    idb->slotunit = unit % CBUS1_MAX_CTRLR_INTERFACES;
 	}
	/* cdb's only exist for cbus cards */
	cdb = controller_find(idb->slot, idb->slotunit);
	if (cdb) {
	    if (cdb->idb_init) {
		(cdb->idb_init)(cdb, idb);
	    }
	    /*
	     * Flags for set up of interface specific fields
	     */
	    is_cip = (cdb->idbtype == IDBTYPE_CIP);
	    is_fe = (cdb->idbtype == IDBTYPE_FEIP);
	    is_feip = (cdb->ctrlr_type == FCI_FEIP_CONTROLLER);
	    is_vip = (cdb->ctrlr_type == FCI_RVIP_CONTROLLER);
	}
    } else {
 	idb->slot = mci->unit;	/* slot number in disguise */
 	idb->slotunit = unit;	/* port number on slot */
	cdb = NULL;
    }

    setup_idb = oldidb ? oldidb : idb;

    if (is_vip || is_feip) {
	reg_invoke_initialize_pa_info(idb, mci);
#ifdef C7000
	if (is_feip) {
		if ((pa_info_ptr = slots[slot].pa_info[slotunit])) {
			convert_raw_eeprom_to_pa_info(idb, pa_info_ptr);
		}
	}
#endif
    }

    switch (FCITYPE(type)) {
    case MCI_TYPE_ETHER:
	cbus_mci_ether_idb_init(mci, idb, is_vip, is_fe);
	break;
	
    case MCI_TYPE_SERIAL:
	cbus_mci_serial_idb_init(mci, idb, capability, is_vip);
	break;

    case FCI_TYPE_FDDI:
    case FCI_TYPE_FDDIT:
	if (cbus_mci_fddi_idb_init(mci, idb)) {
	    /*
	     * Disregard this interface and also
	     * notify user that FDDI boards that do not
	     * support microcode CMT are not supported
	     * from SW release 10.2 on (the first that
	     * supports SMT 7.3).
	     */
	    dci = mci->dci[idb->cbus_ctrlr_index];
	    errmsg(&msgsym(OBSOLETE, UCODE), dci->ctrlr_unit);
	    if (newidb == NULL) {
		if (nomalloc) {
		    enqueue(&idbpreQ, idb);
		} else {
		    if (cdb)
		      controller_destroy(cdb);
		    idb_free(idb);
		}
	    }
	    return TRUE;
	}
	break;

    case FCI_TYPE_CTR:
	cbus_mci_tr_idb_init(mci, idb, setup_idb, is_vip);
	break;

    case FCI_TYPE_HSSI:
	cbus_mci_hssi_idb_init(mci, idb, capability);
	break;

    case FCI_TYPE_ATM:
	if (is_cip)
	  cbus_mci_cip_idb_init(mci, idb, setup_idb);
	else
	  cbus_mci_aip_idb_init(mci, idb, setup_idb);
	break;

    default:
	if (newidb == NULL) {
	    if (nomalloc) {
		enqueue(&idbpreQ, idb);
	    } else {
		if (cdb)
		    controller_destroy(cdb);
		idb_free(idb);
	    }
	}
	return TRUE;
    }

    /*
     * Finish set up of common fields.
     */
    hes_idb_init_after(idb);

    /* 
     * Final setup.
     */
    if (oldidb == NULL) {
	mci_add_interface(mci, unit, idb);
	dci_add_interface(mci, unit, idb);
    } else {
	/* newidb should never be non-null here, but we may as well check */
	if (newidb == NULL) {
	    if (nomalloc)
		enqueue(&idbpreQ, idb);
	    else
		idb_free(idb);
	}
	idb = oldidb;

	/*
	 *  After hot stall or microcode reload, the dci has been wiped.
	 *  Even if we are passing in an IDB, we must add it to the dci's
	 *  list.
	 */
	dci_add_interface(mci, unit, idb);
    }

    /* we must be sure to count the MIP only once */
    if (mci->interface_list[unit] == idb)
	mci->interface_count++;

    if (oldidb == NULL)
	mci_applique(idb);
    
    /*
     *	Anyone that calls this with a newidb set is responsible for seeing
     *	that it is already on the IDB Queue
     */
    if (oldidb == NULL && newidb == NULL) {
	media_idb_enqueue(idb);
    }

    return FALSE;
}

/*
 * cbus_mci_bridge_rxtype_on_ctl
 *
 * Examine every hwidb on the given MCI or cBus controller to
 * determine whether the given rxtype is being bridged.
 * Return TRUE if so, FALSE otherwise.
 */
static boolean cbus_mci_bridge_rxtype_on_ctl (hwidbtype *hwidb, ushort rxtype)
{
    mcitype     *ctl;
    dcitype	*dci;
    hwidbtype   *nhwidb;
    int         indx, jndx;

    ctl = hwidb->devctl;
    for (indx = 0; indx < fci_max_dci_ctrlrs; indx++) {
	if (!(dci = ctl->dci[indx]))
	    continue;
	for (jndx = 0; jndx < dci->interface_count; jndx++) {
	    for (nhwidb = dci->interface_list[jndx] ; nhwidb != NULL;
		 nhwidb = nhwidb->next_idb_on_physint) {
		if (((IDBS_UP == nhwidb->state) ||
		     (IDBS_INIT == nhwidb->state)) &&
		    (nhwidb->tbridge_on_hwidb) &&
		    BRIDGING_RXTYPE(nhwidb->firstsw, rxtype))
		    return(TRUE);
	    }
	}
    }
    return(FALSE);
}

/*****************************************************************************
 * Name: cbus_mci_rxtype_update
 *
 * Description: Enable/disable bridging/routing of a particular rxtype
 *		on a particular physical MCI/cBus interface.
 *
 *   When the argument "set" is TRUE, bridging will be enabled,
 *   otherwise routing will be enabled.
 *
 * Input: 
 *	hwidb	the given physical MCI/cBus interface
 *	rxtype	rxtype for which to enable/disable bridging/routing
 *	set	TRUE == enable bridging, FALSE == enable routing
 *	mema	MEMA address on the SP in which to write the bridge/route
 *		argument
 *
 * NOTA BENE:	The mema argument MUST be zero for all rxtypes greater
 *		than 0xFF.
 *
 * Return Value: None
 *
 *****************************************************************************/
static void cbus_mci_rxtype_update (hwidbtype *hwidb, ushort rxtype,
				    boolean set, ushort mema)
{
    mcitype *mci;
    mciregtype *regptr;
    leveltype status;
    ushort brg_rte_arg;

    mci = hwidb->devctl;

    /*
     * The 0xFF00 is necessary because of an early MCI bug.
     * The 0x0100 bit is accidentally cleared by the microcode.
     */
    brg_rte_arg = (set ? 0xFF00 : 0) | rxtype;
    status = raise_interrupt_level(NETS_DISABLE);

    switch (hwidb->status & (IDB_MCI | IDB_CBUS)) {

    case (IDB_MCI):
	if (!touch_device(mci->regptr))
	    break;		/* ignore removed controllers */

#ifndef C7000
	/*
	 * MCI pre 1.7 software versions only support seven (7) rxtypes. 
	 */
	if ((rxtype > 7) && ((mci->sw_version & 0xFF) < 7))
	    break;
#endif
	/*
	 * Workaround a bug in the early MCI microcode
	 */
	regptr = mci->regptr;
	if (mci->tx1bug) {
	    regptr->argreg = mema;
	    regptr->cmdreg = MCI_CMD_MEMA_SELECT;
	    regptr->argreg = set;
	    regptr->cmdreg = MCI_CMD_MEMA_WRITE;
	} else {
	    regptr->argreg = brg_rte_arg;
	    regptr->cmdreg = MCI_CMD_BRIDGEPROTOCOL;
	}
	break;

    case (IDB_CBUS):
	regptr = mci->regptr;
	regptr->argreg = brg_rte_arg;
	regptr->cmdreg = MCI_CMD_BRIDGEPROTOCOL;
	break;

    default:
	break;
    }

    reset_interrupt_level(status);
}

/*
 * The following routines enable or disable bridging for an rxtype.
 * An rxtype is bridged if it is found to be bridged anywhere on
 * the given interface's contoller.  Otherwise, it is routed.
 *
 * Note that for each protocol, just one primary rxtype is tested
 * and the related rxtypes just follow suit.
 */

static void cbus_mci_ip_rxtypes_update (hwidbtype *hwidb)
{
    boolean set;

    if (global_crb_enable) {
	set = cbus_mci_bridge_rxtype_on_ctl(hwidb, RXTYPE_DODIP);
    } else {
	set = BRIDGING_IF(hwidb->firstsw, LINK_IP);
    }

    cbus_mci_rxtype_update(hwidb, RXTYPE_DODIP, set, 061);
    cbus_mci_rxtype_update(hwidb, RXTYPE_SNAP_DODIP, set, 0);
    cbus_mci_rxtype_update(hwidb, RXTYPE_SAP_IP, set, 0);

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_crb)
	buginf("\nTB-CRB %s (%08x) %s ip", hwidb->hw_namestring,
	       hwidb->devctl, set ? "bridge" : "route");
#endif  /* TBRIDGE_DEBUG */

}

static void cbus_mci_decnet_rxtypes_update (hwidbtype *hwidb)
{
    boolean set;

    if (global_crb_enable) {
	set = cbus_mci_bridge_rxtype_on_ctl(hwidb, RXTYPE_DECNET);
    } else {
	set = BRIDGING_IF(hwidb->firstsw, LINK_DECNET);
    }

    cbus_mci_rxtype_update(hwidb, RXTYPE_DECNET, set, 062);
    cbus_mci_rxtype_update(hwidb, RXTYPE_SNAP_DECNET, set, 0);

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_crb)
	buginf("\nTB-CRB %s (%08x) %s decnet", hwidb->hw_namestring,
	       hwidb->devctl, set ? "bridge" : "route");
#endif  /* TBRIDGE_DEBUG */

}

static void cbus_mci_xns_rxtypes_update (hwidbtype *hwidb)
{
    boolean set;

    if (global_crb_enable) {
	set = cbus_mci_bridge_rxtype_on_ctl(hwidb, RXTYPE_XNS);
    } else {
	set = BRIDGING_IF(hwidb->firstsw, LINK_XNS);
    }

    cbus_mci_rxtype_update(hwidb, RXTYPE_XNS, set, 063);
    cbus_mci_rxtype_update(hwidb, RXTYPE_SNAP_XNS, set, 0);

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_crb)
	buginf("\nTB-CRB %s (%08x) %s xns", hwidb->hw_namestring,
	       hwidb->devctl, set ? "bridge" : "route");
#endif  /* TBRIDGE_DEBUG */

}

static void cbus_mci_clns_rxtypes_update (hwidbtype *hwidb)
{
    boolean set;

    if (global_crb_enable) {
	set = cbus_mci_bridge_rxtype_on_ctl(hwidb, RXTYPE_CLNS);
    } else {
	set = BRIDGING_IF(hwidb->firstsw, LINK_CLNS);
    }

    cbus_mci_rxtype_update(hwidb, RXTYPE_CLNS, set, 064);

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_crb)
	buginf("\nTB-CRB %s (%08x) %s clns", hwidb->hw_namestring,
	       hwidb->devctl, set ? "bridge" : "route");
#endif  /* TBRIDGE_DEBUG */

}


static void cbus_mci_atalk_rxtypes_update (hwidbtype *hwidb)
{
    boolean set;

    if (global_crb_enable) {
	set = cbus_mci_bridge_rxtype_on_ctl(hwidb, RXTYPE_APPLE);
    } else {
	set = BRIDGING_IF(hwidb->firstsw, LINK_APPLETALK);
    }

    cbus_mci_rxtype_update(hwidb, RXTYPE_APPLE, set, 065);
    cbus_mci_rxtype_update(hwidb, RXTYPE_SNAP_APPLE, set, 0);

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_crb)
	buginf("\nTB-CRB %s (%08x) %s apple", hwidb->hw_namestring,
	       hwidb->devctl, set ? "bridge" : "route");
#endif  /* TBRIDGE_DEBUG */

    mci_warning();
}

static void cbus_mci_novell_rxtypes_update (hwidbtype *hwidb)
{
    boolean set;

    if (global_crb_enable) {
	set = cbus_mci_bridge_rxtype_on_ctl(hwidb, RXTYPE_NOVELL1);
    } else {
	set = BRIDGING_IF(hwidb->firstsw, LINK_NOVELL);
    }

    cbus_mci_rxtype_update(hwidb, RXTYPE_NOVELL1, set, 067);
    cbus_mci_rxtype_update(hwidb, RXTYPE_SNAP_NOV1, set, 0);
    cbus_mci_rxtype_update(hwidb, RXTYPE_NOVELL2, set, 070);
    cbus_mci_rxtype_update(hwidb, RXTYPE_SAP_NOVELL, set, 0);

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_crb)
	buginf("\nTB-CRB %s (%08x) %s novell", hwidb->hw_namestring,
	       hwidb->devctl, set ? "bridge" : "route");
#endif  /* TBRIDGE_DEBUG */

    mci_warning();
}

static void cbus_mci_apollo_rxtypes_update (hwidbtype *hwidb)
{
    boolean set;

    if (global_crb_enable) {
	set = cbus_mci_bridge_rxtype_on_ctl(hwidb, RXTYPE_APOLLO);
    } else {
	set = BRIDGING_IF(hwidb->firstsw, LINK_APOLLO);
    }

    cbus_mci_rxtype_update(hwidb, RXTYPE_APOLLO, set, 0);
    cbus_mci_rxtype_update(hwidb, RXTYPE_SNAP_APOLLO, set, 0);

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_crb)
	buginf("\nTB-CRB %s (%08x) %s apollo", hwidb->hw_namestring,
	       hwidb->devctl, set ? "bridge" : "route");
#endif  /* TBRIDGE_DEBUG */

}

static void cbus_mci_vines_rxtypes_update (hwidbtype *hwidb)
{
    boolean set;

    if (global_crb_enable) {
	set = cbus_mci_bridge_rxtype_on_ctl(hwidb, RXTYPE_VINES);
    } else {
	set = BRIDGING_IF(hwidb->firstsw, LINK_VINES);
    }

    cbus_mci_rxtype_update(hwidb, RXTYPE_VINES, set, 0);
    cbus_mci_rxtype_update(hwidb, RXTYPE_SNAP_VINES, set, 0);

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_crb)
	buginf("\nTB-CRB %s (%08x) %s vines", hwidb->hw_namestring,
	       hwidb->devctl, set ? "bridge" : "route");
#endif  /* TBRIDGE_DEBUG */

}


/*
 * cbus_mci_rxtypes_update
 * Setup the cBus/MCI to know whether it is bridging or routing an rxtype.
 */
void cbus_mci_rxtypes_update (hwidbtype *hwidb)
{
    int rxtype;

    /*
     * Set bridging all rxtypes.
     */
    for (rxtype = 0; rxtype < RXTYPE_LAST; rxtype++)
	cbus_mci_rxtype_update(hwidb, rxtype, TRUE, 0);

    /*
     * Select the correct option based on the bridge flag.
     */
    cbus_mci_ip_rxtypes_update(hwidb);
    cbus_mci_decnet_rxtypes_update(hwidb);
    cbus_mci_xns_rxtypes_update(hwidb);
    cbus_mci_clns_rxtypes_update(hwidb);
    cbus_mci_atalk_rxtypes_update(hwidb);
    cbus_mci_novell_rxtypes_update(hwidb);
    cbus_mci_apollo_rxtypes_update(hwidb);
    cbus_mci_vines_rxtypes_update(hwidb);
}

/*
 * cbus_throttle_controller
 * Throttle an entire controller, given an IDB.  Cbus only
 */
static void cbus_throttle_controller (hwidbtype *idb)
{
    mciregtype *regptr;
    cdbtype *cdb;
    platform_hware_handle_type *hware_handle;

    regptr = idb->mci_regptr;
    regptr->argreg = idb->mci_index;
    regptr->cmdreg = MCI_CMD_SELECT;

    cdb = controller_find(idb->slot, idb->slotunit);
    if (cdb == NULL) {
	/*
	 *	This can never happen, but we actually have enough info
	 *	to throttle back, so we might as well.
	 */
	regptr->argreg = FCI_IF_THROTTLE;
	regptr->cmdreg = FCI_CMD_STATUS;
	if (regptr->argreg != 0) {
	    regptr->argreg = FCI_IF_THROTTLE;
	    regptr->cmdreg = MCI_CMD_STATUS;
	}
	return;
    }
	
    if (!cdb->input_throttled) {
	regptr->argreg = FCI_IF_THROTTLE;
	regptr->cmdreg = FCI_CMD_STATUS;
	if (regptr->argreg != 0) {
	    regptr->argreg = FCI_IF_THROTTLE;
	    regptr->cmdreg = MCI_CMD_STATUS;
	}
	cdb->input_throttled = TRUE;
    }

    /*
     *  Now mark all IDBs
     */

    hware_handle = cdb->platform_hware_handle;
    for (idb = hware_handle->interface_list[cdb->unit]
	     ; idb != NULL
             ; idb = idb->next_idb_on_physint) {
	idb_mark_throttled(idb);
    }
}
/*
 * cbus_mci_throttle
 * Function to discard a datagram because the scheduler has not
 * been allowed to run recently.
 */
void cbus_mci_throttle (hwidbtype *idb)
{
    mciregtype *regptr;
    mcitype *cbus;

    if (idb == NULL)
        return;

    idb->counters.input_drops++;
    regptr = idb->mci_regptr;
    regptr->cmdreg = MCI_CMD_RX_FLUSH;
    if (idb->status & IDB_CBUS) {
	/*
	 *  Test for current microcode.  This test could probably be
	 *  eliminated.
	 */
	cbus = (mcitype *) cbusQ.qhead;
	if (((cbus->sw_version >> 8) == 128) || idb_is_throttled(idb))
	    return;		/* either can't, or is already, throttled */

	cbus_throttle_controller(idb);
    }
}

/*
 * cbus_mci_idb_throttle
 * Function to throttle an interface.
 */
void cbus_mci_idb_throttle (hwidbtype *idb)
{
    mciregtype *regptr;

    regptr = idb->mci_regptr;
    regptr->cmdreg = MCI_CMD_RX_FLUSH;

    if (idb->status & IDB_CBUS) {
	cbus_throttle_controller(idb);
    } else {
	fci_select_if(idb, regptr);
	fci_cbus_cmd_arg(DCI_FROM_HWIDB(idb), regptr,
			 FCI_CMD_STATUS, MCI_IF_DISABLE,
			 CTRLR_CMD_ATTEMPTS, TRUE);
	idb_mark_throttled(idb);
    }
}


/*
 * The following set of cbus_ command routines are used by the driver
 * code that hasn't been sanitised to use the new RSP compatible 
 * command wrapper techniques.  This may be because:
 *    . we haven't got to it yet (e.g. mip)             OR
 *    . it's driver code that is not used on the RSP
 */

static long cbus_errors (
    mciregtype *regptr,
    ushort code)
{
    ushort result;

    regptr->argreg = code;
    regptr->cmdreg = FCI_CMD_ERRORS;
    result = regptr->cmdreg;
    if (result != FCI_RSP_OKAY)
	return(0);
    return(regptr->argreg);
}


static void cbus_buffersize (mciregtype *regptr, ushort size)
{
    /*
     * Tell the MALU what buffersize we will be using -- this command really
     * only stores the *size* in MEMA. Subsequent cBus commands can and do
     * reference this MEMA location. Kind of a kludge.
     */
    regptr->argreg = size;
    regptr->cmdreg = FCI_CMD_BUFFERSIZE;
}

void cbus_idb_rx_offset (mciregtype *regptr, ushort offset)
{
    /*
     * Sets the receive offset for an interface. An offset of zero (0),
     * indicates that recieved data will start immediately after the
     * microcode overhead (currently 10 words). This paramter should be
     * closely tied to the encapsulation expected on the interface.
     */
    cbus_buffersize(regptr, offset);
    regptr->argreg = FCI_RX_OFFSET;
    regptr->cmdreg = FCI_CMD_EXECUTE;
}

void cbus_rx_buffer_parking (mciregtype *regptr, int indx, ushort slots)
{
    regptr->argreg = indx;
    regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    regptr->argreg = slots;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CBUS_BUFFER_PARK;
    regptr->cmdreg = FCI_CTRLR_CMD;
}

void cbus_idb_rx_max_byte_size (mciregtype *regptr, ushort size)
{
    /*
     * Fixes the maximum number of bytes that an interface will be allowed
     * to received. *size* is in BYTES.
     */
    cbus_buffersize(regptr, size);
    regptr->argreg = FCI_MAX_BUFFERSIZE;
    regptr->cmdreg = FCI_CMD_EXECUTE;
}

static void cbus_idb_rx_buffer_limit (mciregtype *regptr, ushort limit)
{
    /*
     * Fix the maximum number of buffers that may be consumed
     * by an interface for receive data.
     */
    cbus_buffersize(regptr, limit);
    regptr->argreg = FCI_RX_BUFFER_LIMIT;
    regptr->cmdreg = FCI_CMD_EXECUTE;
}

void cbus_idb_rx_error (mciregtype *regptr, ushort pool)
{
    /*
     * Assign the interface to a pool to be used for RX_ERROR reporting.
     */
    cbus_buffersize(regptr, pool);
    regptr->argreg = FCI_RX_ERROR;
    regptr->cmdreg = FCI_CMD_EXECUTE;
}

void cbus_idb_assign_pool (mciregtype *regptr, ushort pool)
{
    /*
     * Assign an interface to a particular buffer pool -- for both
     * receive and transmit buffers.
     */
    cbus_buffersize(regptr, pool);
    regptr->argreg = FCI_ASSIGN_POOLINDEX;
    regptr->cmdreg = FCI_CMD_EXECUTE;
}

static inline int cbus_create_buffer (
    mciregtype *regptr,
    ushort pool,
    ushort size)
{
    ushort sresult;

    cbus_buffersize(regptr, size);
    regptr->argreg = pool;
    regptr->cmdreg = FCI_CMD_TX_ALLOCATE;
    sresult = regptr->cmdreg;
    return(sresult);
}


#ifdef C7000
#define CBUS_CTLR(x) (x->unit)
#define CBUS2SLOT(x) (x)
#define AGS_C7K(ags,c7k) (c7k)
#else
#define CBUS_CTLR(x) (-1)
#define CBUS2SLOT(x) cbus2slot_agsplus[(x) + 1]
static const uchar cbus2slot_agsplus[] = { 7, 6, 8, 5, 9, };
#define AGS_C7K(ags,c7k) (ags)
#endif


/*****************************************************************************
 * Name: add_cbus_dci
 *
 * Description: calls SNMP to add a card (dci) to the system
 *
 * Input: 
 *   dci -- pointer to the dci for the card to add
 *   index -- the cbus index for the card to add (slot number?)
 *
 * Return Value: None
 *
 *****************************************************************************/
static void add_cbus_dci (dcitype* dci, uint index)
{
    char* cp;
    char* swver;
    char* hwver;
    char buf[48];
    char* card_descr;
    card_type_t card_type;
    int bays = 0; 
    uint serialnum;
    boolean this_is_srswitch_pslot = FALSE;
    boolean this_is_ssip_smip_pslot = FALSE;
#ifdef C7000
    int bay;
    slottype *slot_ptr;
    pa_info_slots *pa_info_ptr;
    dgtype *dbus;
    dbus = pslot_to_dbus(dci->ctrlr_unit);
    this_is_srswitch_pslot = srswitch_pslot(dci->ctrlr_unit);
    this_is_ssip_smip_pslot = ssip_smip_pslot(dci->ctrlr_unit);
    if (dbus)
	serialnum = dbus_serial_number(dbus);
    else
#endif
	serialnum = NO_SERIALNUM;

    card_type_and_descr(dci->ctrlr_type, dci->interface_count,
			&card_type, &card_descr);
    if (this_is_srswitch_pslot || this_is_ssip_smip_pslot) {
	switch (dci->ctrlr_type) {
	  case FCI_FDDIT_CONTROLLER:
	    card_type = D_cardType_srs_fip;
	    card_descr = "SRS FDDI";
	    break;

	  case FCI_CTR_CONTROLLER:
	    card_type = D_cardType_srs_trip;
	    card_descr = "SRS token ring";
	    break;

	  case FCI_FSIP_CONTROLLER:
	    card_type = D_cardType_ssip;
	    card_descr = "Standard Serial";
	    break;

	  case FCI_MIP_CONTROLLER:
	    card_type = D_cardType_smip;
	    card_descr = "Service Provider Multichannel";
	    break;
	}
    }

    cp = buf;
    cp += sprintf(hwver = cp, "%u.%u",
		  dci->hw_version >> 8, dci->hw_version & 0xFF);
    cp += sprintf(swver = ++cp, "%u.%u",
		  dci->sw_version >> 8, dci->sw_version & 0xFF);
    switch (dci->ctrlr_type) {
	  case FCI_RVIP_CONTROLLER:
		bays = 2;
	  break;
#ifdef C7000
        case FCI_FEIP_CONTROLLER:
            slot_ptr = &slots[index];
            for (bay = 0; bay < 2 ; bay++) {
                pa_info_ptr = slot_ptr->pa_info[bay];
                if (pa_info_ptr) {
                    switch (pa_info_ptr->controller_type) {
                        /*
                         * Initial TX FEIP PA's were shipped without being
                         * programmed (0xff) so recognize them as TX PA's.
                         */
                    case PA_HARDWARE_UNKNOWN:
                    case PA_HARDWARE_1FE:
                    case PA_HARDWARE_1FE_TX_ISL:
                        card_type = D_cardType_feip;
                        /* card_descr setup in card_type_and_descr() */
                        break;
                    case PA_HARDWARE_1FE_TX_NISL:
                        card_type = D_cardType_feip_tx;
                        /* card_descr setup in card_type_and_descr() */
                        break;
                    case PA_HARDWARE_1FE_FX_ISL:
                    case PA_HARDWARE_1FE_FX_NISL:
                        card_type = D_cardType_feip_fx;
                        /* card_descr setup in card_type_and_descr() */
                        break;
                    default:
                        card_type = D_cardType_unknown;
                        break;
                    }
                    break;
                }
            }
#endif /* C7000 */

    }
    reg_invoke_chassismib_add_sub_card_entry(card_type, 
			(void*)((int) dci->ctrlr_unit + 1), card_descr, 
			serialnum, hwver, swver,  CBUS2SLOT(index), 0, bays);
}


static void add_cbus_ctlr (mcitype* cbus)
{
    char* board;
    char* cp;
    char* swver;
    char* hwver;
    char buf[48];
    card_type_t card_type;
    uint serialnum;
#ifdef C7000
    dgtype *dbus;

    dbus = pslot_to_dbus(cbus->unit);
    if (dbus)
	serialnum = dbus_serial_number(dbus);
    else
#endif
	serialnum = NO_SERIALNUM;

    board = "CiscoBus controller";
    if (cbus->flags & CBUS) 
	card_type = D_cardType_csc_cctl1;
    else if (cbus->flags & CBUSII) {
	board = "Switch Processor";
	card_type = D_cardType_sp;
	if ((cbus->hw_version >> 8) == SSP_HARDWARE) {
	    card_type = D_cardType_ssp;
	    board = "Silicon Switch Processor";
	}
	card_type = AGS_C7K(D_cardType_csc_cctl2, card_type);
	board = AGS_C7K("CiscoBusII controller", board);
    } else
	card_type = D_cardType_unknown;
    cp = buf;
    cp += sprintf(hwver = cp, "%u.%u",
		  cbus->hw_version >> 8, cbus->hw_version & 0xff);
    cp += sprintf(swver = ++cp, "%u.%u",
		  cbus->sw_version >> 8, cbus->sw_version & 0xff);

    reg_invoke_chassismib_add_card_entry(card_type, NO_HOTSWAP, board, 
					 serialnum, hwver, swver, 
					 (int) CBUS2SLOT(CBUS_CTLR(cbus))); 
}

static int vip_get_if_type (int pa_type, 
			      enum pif_types_ *pift, 
			      enum IDBTYPE *idbtype)
{
    switch (pa_type) {
    case FCI_TYPE_VIP_FETHER:
    case FCI_TYPE_VIP_FETHER_TX_NISL:
    case FCI_TYPE_VIP_FETHER_TX_ISL:
    case FCI_TYPE_VIP_FETHER_FX_ISL:
	*pift = PIFT_FASTETHER;
	*idbtype = IDBTYPE_FEIP;
	break;
    case FCI_TYPE_VIP_ETHER8:
	*pift = PIFT_ETHERNET;
	*idbtype = IDBTYPE_CBUSETHER;
	break;
    case FCI_TYPE_VIP_SERIAL4:
	*pift = PIFT_SERIAL;
	*idbtype = IDBTYPE_CBUSSERIAL;
	break;
    case FCI_TYPE_VIP_TRING4:
	*pift = PIFT_TOKENRING;
	*idbtype = IDBTYPE_CTR;	
	break;
    default:
	*pift = PIFT_UNKNOWN;
	*idbtype = IDBTYPE_UNKNOWN;
	return FCI_TYPE_UNKNOWN;
    }

    return pa_type;
}





/*
 * search idbQ for an idb that matches slot, port, and controller type.
 * This will need to be changed when we support non-homogenous IPs.
 */

static hwidbtype* idb_matching_slot (mcitype *cbus, dcitype *dci, int slot,
				     int if_unit, int dci_type) 
{
    hwidbtype *idb;
    enum IDBTYPE bits;
    enum pif_types_ pift;
    int dummy_slot, slotunit, pa;

    FOR_ALL_HWIDBS(idb) {
	if (idb->slot == slot && idb->mci_index == if_unit) {
	    switch (dci_type) {
	    case FCI_ETHER_CONTROLLER:
		bits = IDBTYPE_CBUSETHER;
		break;
 	    case FCI_LINK_CONTROLLER:
		bits = IDBTYPE_HSSI;
		break;
	    case FCI_FDDIT_CONTROLLER:
		bits = IDBTYPE_FDDIT;
		break;
	    case FCI_CTR_CONTROLLER:
		bits = IDBTYPE_CTR;
		break;
	    case FCI_FSIP_CONTROLLER:
		bits = IDBTYPE_CBUSSERIAL;
		break;
	    case FCI_AIP_CONTROLLER:
		bits = IDBTYPE_AIP;
		break;
	    case FCI_CIP_CONTROLLER:
		bits = IDBTYPE_CIP;
		break;
	    case FCI_FEIP_CONTROLLER:
		bits = IDBTYPE_FEIP;
		break;
            case FCI_RVIP_CONTROLLER:
		cbus_unit2slotunit(if_unit, &dummy_slot, &slotunit);
		pa = cbus_get_pa_from_slotunit(slotunit);
		vip_get_if_type(dci->pa_type[pa], &pift, &bits);		
                break;
	    default:
		bits = IDBTYPE_UNKNOWN;
		buginf("\nidb_matching_slot: unknown type %d", dci_type);
		break;
	    }
	    if (bits == idb->type) {
		return(idb);
	    }
	}
    }
    return(NULL);
}    

#ifdef C7000
/* sp_eoir_reset
 * fix up sp memory 
 */
static void sp_eoir_reset (mcitype* cbus) 
{
    mciregtype* regptr = cbus->regptr;

    if (oir_debug)
	buginf("\n Starting sp_eoir_reset(%#x)", cbus);

    regptr->cmdreg = FCI_AS_CAPABILITY;
    if (regptr->cmdreg == 0) {
	if (regptr->argreg & FCI_CAP_OIR_RESET) {
	    regptr->argreg = FCI_HS_OIR_RESET;
	    regptr->cmdreg = FCI_CMD_HS_OP;
	    DELAY(40);
	    return;
	}
    }

    if (oir_debug)
	buginf("\n Ending sp_eoir_reset");
}

/*
 * sp_eoir_quiescere
 * Cause a temporary cessation of activity by interface processors
 * so that MEMD may be reconfigured without forcing resets of all
 * the associated interface.
 */
void sp_eoir_quiescere (mcitype* cbus)
{
    ushort slot;
    int indx;
    mciregtype *regptr;
    dcitype *dci;    
    ushort sresult;
    dgtype *dgbus;
    ushort hwver;
    ushort ctrlr_type;
    ushort if_type;
    
    if (oir_debug)
	buginf("\n Starting sp_eoir_quiescere(), cbus=%#x", cbus);

    regptr = cbus->regptr;
    for (slot = 0; slot < fci_max_dci_ctrlrs; slot++) {

	if (!touch_device(pslot_to_dbus(slot))) {
	    if (oir_debug)
		buginf("\nskipping slot %d, no card present", slot);
	    continue;
	}

	dci = cbus->dci[slot];
	for (indx = 0; indx < CBUS2_MAX_CTRLR_INTERFACES; indx++) {
	    int if_unit;

	    dgbus = pslot_to_dbus(slot);
	    hwver = dbus_hw_version(dgbus);
	    ctrlr_type = dbus_controller_type(dgbus);
	    /*
	     * if_unit of VIP PA 1 and 2 are not contiguous
	     */
	    if_unit = cbus_slotunit2unit(slot, indx);
	    if (!fci_grab_if(dci, regptr, &sresult, if_unit, &if_type)) {
		if (sresult == FCI_RSP_ERR_TMOUT) {
		    if (oir_debug)
			buginf("\nslot %d status %#x control %#x",
			       slot, dbus_prot_rd(&dgbus->diag_status),
			       dbus_prot_rd(&dgbus->diag_control));
		    dbus_slot_reset(slot);
		    if (oir_debug) 
			buginf("\nreset non-responsive card in slot %d",
			       slot);
		    break;
		} if (sresult == FCI_RSP_ERR_NPRS) {
		    if (oir_debug)
			buginf("\nif %d/%d returned %#x, goto next",
			       slot, indx, sresult);
		    continue;
		} else {
		    if (oir_debug)
			buginf("\nif %d/%d returned %#x, skipping",
			       slot, indx, sresult);
		    break; /* some other error, probably ERR_NPRS  */
		}
	    }
	    sresult = if_type & 0xFF;
	    if (oir_debug)
		buginf("\nselect on %d/%d returned %d", slot, indx, sresult);
	    /* 
	     * Decide whether to quiesce or reset the interface
	     */
	    if (FCITYPE(sresult) == FCI_TYPE_ETHER){
		/*
		 * EIP and FEIP do not support EOIR; only OIR
		 */
		if ((ctrlr_type == FCI_ETHER_CONTROLLER) ||
		    (ctrlr_type == FCI_FEIP_CONTROLLER)) {
		    dbus_slot_reset(slot);
		    if (oir_debug) {
			if (ctrlr_type == FCI_ETHER_CONTROLLER)
			    buginf("\nreset %s rev %#x in slot %d",
				   "EIP", hwver, slot);
			if (ctrlr_type == FCI_FEIP_CONTROLLER)
			    buginf("\nreset %s rev %#x in slot %d",
				   "FEIP", hwver, slot);
		    }
		    break;
		}
	    } else if (FCITYPE(sresult) == FCI_TYPE_SERIAL) {
	        if (((ctrlr_type == FCI_FSIP_CONTROLLER) &&
		     (hwver == FSIP_NO_EOIR_VERSION)) ||
		    ((ctrlr_type == FCI_MIP_CONTROLLER) &&
		     (hwver == MIP_NO_EOIR_VERSION))) {
		    dbus_slot_reset(slot);
		    if (oir_debug) {
			if (ctrlr_type == FCI_FSIP_CONTROLLER)
			    buginf("\nreset %s rev %#x in slot %d",
				   "FSIP", hwver, slot);
			else if (ctrlr_type == FCI_MIP_CONTROLLER)
			    buginf("\nreset %s rev %#x in slot %d",
				   "MIP", hwver, slot);
		    }
		    break;
		}
	    } else if (sresult == FCI_TYPE_ATM) {
		if (hwver == AIP_NO_EOIR_VERSION) {
		    dbus_slot_reset(slot);
		    if (oir_debug) {
			buginf("\nreset AIP rev %#x in slot %d",
			       hwver, slot);
		    }
		    break;
		}
	    }

	    /*
	     * Don't try to QUIESCE boards that were just inserted
	     * and haven't been reinitialized yet:
	     */
	    if (slots[slot].flags & BOARD_RELOADED) {
		if (oir_debug) 
		    buginf("\nskipping newly (re)installed %s in slot %d",
			   slots[slot].card_name, slot);
	    } else {

		/*
		 * If we are here, quiesce the card
		 */
		fci_cbus_cmd_arg_rslt(dci, regptr, &sresult,
				      FCI_CMD_STATUS, FCI_STATUS_QUIESCE, 
				      CTRLR_CMD_ATTEMPTS, TRUE);
		if (oir_debug) {
		    buginf("\nquiesced slot/if %d/%d, type (%d), sresult (0x%-04x)",
			   slot, indx, if_type, sresult);
		}
	    }
	}
    }
    
    if (oir_debug)
	buginf("\n Ending sp_eoir_quiescere()");
}
#endif

/*
 * A platform specific routine to explicitly check for any
 * deferred interface, card, or complex restarts:
 */
static void cbus_restart_check (hwidbtype *hwidb)
{
    mcitype	*mci;

    if ((hwidb->status & (IDB_CBUS | IDB_MCI)) == 0)
	return;

    /*
     * If we have a deferred restart, make it happen now:
     */
    mci = hwidb->devctl;
    if (mci->restartdeferred)
	platform_demand_complex_restart(hwidb);
}

/*
 *	cbus_red_set_precedence
 *
 *	set precedence levels when RED is activated. RED is configured
 *	in prioritygroup_command() to manage down to a holdq that is
 *	half full. On the CBUS, "half of the queue" needs to take into
 *	account the interface card's onboard queue, which may be larger
 *	than the hold queue in main memory.
 */
static void cbus_red_set_precedence (hwidbtype *idb) {
    int total_queue;
    int half_of_queue;
    red_parameters *redp = idb->red_parameters;
    if (redp) {
	total_queue = redp->saved_threshold + idb->mci_txcount;
	half_of_queue = total_queue/2 - idb->mci_txcount;
	red_set_precedence_levels(idb, half_of_queue > 0? half_of_queue: 1);
    }
}
static int scan_slots_for_cbus_ctrlr (boolean firsttime, boolean eoir_cycle)
{
    uchar *cbusbase;
    int slot;
    mcitype *cbus;
    ushort sresult;
    int cbus_controllers;
    uchar *tptr;
    dgtype *dgbus;
    mciregtype *regptr;
    hwidbtype *idb;
    
    cbus_controllers = 0;
    cbusbase = (uchar *)FCISTART;	/* Shares multiiobus with MCI/SCIs */

    for (slot = 0; slot <= FCI_MAX_CTRLR_NUMBER; slot++, cbusbase += MCIBYTES) {
	mcitype *old_cbus;
	boolean dupcard;
	
	if (!touch_device(cbusbase)) {
	    continue;
	}

	if (firsttime) {
	    cbus = malloc_named(sizeof(mcitype), "CBUS Controller");
	} else {
	    cbus = dequeue(&cbuspreQ);
	    if (cbus)
		memset(cbus, 0, sizeof(mcitype));
	}
	if (!cbus) {
	    return(cbus_controllers);
	}
	
	
	/*
	 *  This is more than a little ugly.  Later on, if it turns
	 *  out that the cbus we just allocated is a duplicate, we
	 *  shall return it to the preQ and reuse the old one.  Which
	 *  is good, because there are things which point to the old
	 *  cbus.  The problem is that we want to set a flag to let them
	 *  know we are in the process of configuring the cbus, so we
	 *  must set it in the old one also, if it exists.
	 */
	for (old_cbus = (mcitype*)cbusQ.qhead; old_cbus; old_cbus = old_cbus->next) {
	    if (old_cbus->unit == slot) {
		old_cbus->configuring = TRUE;
		break;
	    }
	}
	cbus->configuring = TRUE;
	
	/*
	 * Need to determine if this device is a cBus controller.
	 */
	cbus->unit = slot;
	cbus->regptr = (mciregtype *)cbusbase;
	regptr = cbus->regptr;
	dgbus = NULL;		/* init this for AGS+ systems */
#ifdef C7000
	dgbus = pslot_to_dbus(slot);
	if (!(touch_device(dgbus))) {
	    if (!firsttime) {
		enqueue(&cbuspreQ, cbus);
	    } else {
		free(cbus);
	    }
	    if (old_cbus != NULL)
		old_cbus->configuring = FALSE;
	    continue;
	}
	sresult = reg_invoke_get_hw_version(dgbus, slot);
#else
	regptr->argreg = FCI_HW_VERSION;
	regptr->cmdreg = FCI_CMD_VERSION;
	sresult = regptr->argreg;
#ifdef new
	/* VIP TODO - check if this will work for ags too */
	sresult = reg_invoke_get_hw_version(regptr, slot);
#endif
#endif
	/*
	 * If the hardware type is not a cBus, free memory and cycle.
	 */
	sresult >>= 8;
	if (sresult == CBUS_HARDWARE) {
	    cbus->flags = CBUS;
	    cbus->max_interfaces = CBUS1_MAX_INTERFACES;
	    cbus_max_ctrlr_interfaces = CBUS1_MAX_CTRLR_INTERFACES;
	} else if (sresult == CBUSII_HARDWARE) {
	    cbus->flags = CBUSII;
	    cbus->max_interfaces = CBUS2_MAX_INTERFACES;
	    cbus_max_ctrlr_interfaces = CBUS2_MAX_CTRLR_INTERFACES;
	} else if (reg_invoke_switch_proc_present(dgbus)) {
	    cbus->flags = CBUSII | CXBUS;	
	    cbus->max_interfaces = CBUS2_MAX_INTERFACES;
	    cbus_max_ctrlr_interfaces = CBUS2_MAX_CTRLR_INTERFACES;
	} else {
	    if (!firsttime) {
		enqueue(&cbuspreQ, cbus);
	    } else {
		free(cbus);
	    }
	    if (old_cbus != NULL)
		old_cbus->configuring = FALSE;
	    continue;
	}
	
	/*
	 * If this is first call to cbus_analyze, test the controller.
	 * Subsequent calls will not bother, unless we find a board
	 * that does not pass.
	 * First run controller through basic bus and memory tests.
	 */
	if (firsttime) {
	    tptr = (uchar *)cbus_mci_memtest(cbus);
	    if (tptr) {
		errmsg(&msgsym(TESTFAIL, CBUS), cbus->unit, tptr);
		free(cbus);
#ifdef C7000
		reg_invoke_disable_slot(slot);
		/* boardfailed = TRUE; */
#endif
		if (old_cbus != NULL)
		    old_cbus->configuring = FALSE;
		continue;
	    }
	}

	if (cbus_controllers == 0) {
	    cbus_base_fddi = -1;
	    cbus_base_hssi = -1;
	    cbus_base_token = -1;
	    cbus_base_aip = -1;
	    cbus_base_t1 = -1;
	    cbus_base_e1 = -1;
	    cbus_base_channel = -1;
	    cbus_base_feth = -1;
	    if (nMCI == 0 && firsttime) {
#ifdef C7000
		createlevel(FCI_INTLEVEL,cbus_interrupt,"CXBus Interfaces");
#else
		createlevel(FCI_INTLEVEL,cbus_interrupt,"cBus Interfaces");
#endif
		cbus_mci_base_ether = -1;
		cbus_mci_base_serial = -1;
	    }
	    
	    /*
	     * Find starting number for each type of interface.
	     */
	    FOR_ALL_HWIDBS(idb) {
		if (idb->status & IDB_ETHER) {
		    if (idb->type == IDBTYPE_FEIP) {
			if (idb->unit > cbus_base_feth)
			    cbus_base_feth = idb->unit;
		    } else {
			if (idb->unit > cbus_mci_base_ether)
			    cbus_mci_base_ether = idb->unit;
		    }
		} else if (idb->status & IDB_SERIAL) {
		    if (idb->type == IDBTYPE_T1) {
			if (idb->unit > cbus_base_t1)
			    cbus_base_t1 = idb->unit;
		    } else if (idb->type == IDBTYPE_E1) {
			if (idb->unit > cbus_base_e1)
			    cbus_base_e1 = idb->unit;
		    } else if (idb->status & IDB_HSSI) {
			if (idb->unit > cbus_base_hssi)
			    cbus_base_hssi = idb->unit;
		    } else {
			if (idb->unit > cbus_mci_base_serial)
			    cbus_mci_base_serial = idb->unit;
		    }
		} else if (idb->status & IDB_FDDI) {
		    if (idb->unit > cbus_base_fddi)
			cbus_base_fddi = idb->unit;
		} else if (idb->status & IDB_TR) {
		    if (idb->unit > cbus_base_token)
			cbus_base_token = idb->unit;
		} else if (idb->status & IDB_ATM) {
		    if (idb->type == IDBTYPE_AIP) {
			if (idb->unit > cbus_base_aip)
			    cbus_base_aip = idb->unit;
		    }
		    else if (idb->type == IDBTYPE_CIP) {
			if (idb->unit > cbus_base_channel)
			    cbus_base_channel = idb->unit;
		    }
		}
	    }
	    /*
	     * Get hardware and software versions on the cBus.
	     */

#ifdef DEBUG_PARALLEL
	    ttyprintf (CONTTY,"cbus soft reset\n");
#endif
	    reg_invoke_cbus_soft_reset(cbus);

#ifdef C7000
	    dgbus = pslot_to_dbus(slot);
	    cbus->hw_version = reg_invoke_get_hw_version(dgbus, slot);
#else	    
	    regptr->argreg = FCI_HW_VERSION;
	    regptr->cmdreg = FCI_CMD_VERSION;
	    cbus->hw_version = regptr->argreg;
#ifdef new
	    /* VIP TODO - see if reg_invoke_get_hw_version() will work */
	    cbus->hw_version = reg_invoke_get_hw_version(regptr, slot);
#endif
#endif
	    regptr->argreg = FCI_SW_VERSION;
	    regptr->cmdreg = FCI_CMD_VERSION;
	    cbus->sw_version = regptr->argreg;
	    GET_TIMESTAMP(cbus->lastreset);
	    dupcard = FALSE;
	    for (old_cbus = (mcitype*)cbusQ.qhead; old_cbus;
		 old_cbus = old_cbus->next) {
		if (old_cbus->unit == cbus->unit) {
		    /* this card already exists, don't add it to the queue */
		    dupcard = TRUE;
		    old_cbus->sw_version = cbus->sw_version;
		    break;
		}
	    }
	    cbus_controllers++;
	    if (!dupcard) {
		enqueue(&cbusQ, cbus);
		add_cbus_ctlr(cbus);
	    } else {
		enqueue(&cbuspreQ, cbus);
	    }
	}
    }
    return cbus_controllers;
}

static inline boolean cbus_ctrlr_resyncable (int ctrlr_type)
{
    switch (ctrlr_type) {
    case FCI_CIP_CONTROLLER:
    case FCI_FSIP_CONTROLLER:
    case FCI_FEIP_CONTROLLER:
    case FCI_RVIP_CONTROLLER:  
	return TRUE;
    default:
	return FALSE;
    }
}

static void fddi_smt_init (void)
{
    
    int indx, jndx;

    /*
     * Reset FDDI SMT data structure
     */
    for (indx = 0; indx < MAX_STATION_COUNT; indx++) {
	for (jndx=0; jndx<MAX_MAC_COUNT; jndx++)
	    STADATA (indx, hwIDB[jndx]) = 0;
	for (jndx=0; jndx<MAX_PORT_COUNT; jndx++)
	    STADATA (indx, portRegs[jndx]) = 0;
	STADATA (indx, hwPresent) = FALSE;
	STADATA (indx, prevCFState) = CF_isolated;
    }
}

static inline boolean can_use_ext_int (int ctrlr_type)
{
   switch (ctrlr_type) {
    case FCI_RVIP_CONTROLLER:   
	return TRUE;
	break; 
    default:
	return FALSE;
	break;
    }
}


int cbus_get_next_if_unit (int slot, int *slotunit)
{
    *slotunit += 1;
    return cbus_slotunit2unit(slot, *slotunit);
}
/*
 * c b u s _ s h a p i n g _ s u p p o r t _ 7 0 0 0 (hwidbtype *hwidb)
 *
 * Perform platform-dependent configuration cleanup when traffic
 * shaping is enabled.  Currently called from registry within
 * shape_command().
 *
 * disable cache modes inconsistent with traffic shaping on 7000
 */
static void cbus_shaping_support_7000 (hwidbtype *hwidb)
{
    if (hwidb->firstsw) {
	if (hwidb->ip_routecache & IP_FAST_ROUTECACHE)
	    printf("   Fast Switching is now disabled on this interface\n"); 
	hwidb->ip_routecache &= ~IP_FAST_ROUTECACHE;
	hwidb->ip_routecache &= ~IP_CBUS_ROUTECACHE;
	hwidb->ip_routecache &= ~IP_SSE_ROUTECACHE;
	hwidb->ip_routecache &= ~IP_DISTRIBUTED_ROUTECACHE;
	reg_invoke_fast_setup(hwidb);
	/*
	 * Disable multicast fast switching so that this stream may be
	 * shaped.
	 */
	reg_invoke_ipmulticast_fs_setup_interface(hwidb);
    }
}

/*
 * cbus_analyze
 * Discover and initialize the cBus controller
 * Called from init_68k.c
 */
int cbus_analyze (boolean firsttime, boolean eoir_cycle)
{
    mcitype *cbus;
    dcitype *dci, *od, *nx_dcis[FCI_MAX_DCI_CTRLRS];
    mciregtype *regptr;
    dgtype *dgbus;
    hwidbtype *idb;
    cdbtype *cdb;
    int slot, slotunit, if_unit;
    int cbus_controllers, nif, typecode;
    uchar *tptr, *p;
    ushort sresult, if_type;
    boolean boardfailed, newcard;
    boolean this_is_ssip_smip_pslot = FALSE;
    boolean this_is_cip2_pslot = FALSE;
    ushort idx;
#ifdef C7000
    pa_info_slots *pa_info_ptr;
#endif

    nserialnets = 0;
    cbus_controllers = 0;
    dgbus = NULL;

    if (oir_debug) {
	buginf("\n Starting cbus_analyze(firsttime=%d, eoir_cycle=%d)",
	       firsttime, eoir_cycle);
	bugtrace();
    }

    /*
     * init the controller data structures 
     */
    controller_init(firsttime, FCI_MAX_DCI_CTRLRS, MAX_CTRLR_INTERFACES);

    if (firsttime) {
	reg_add_interface_cfg_modified(set_cbus_txlimit, "set_cbus_txlimit");
	reg_add_shape_cache(cbus_shaping_support_7000,
			    "cbus_shaping_support_7000");
	reg_add_interface_cfg_modified(cbus_red_set_precedence, 
				       "cbus_red_set_precedence");
	mec_eth = 0;
	ctr_trs = 0;
	fsip_ser = 0;
	ssip_ser = 0;
	nt1ports = 0;
	ne1ports = 0;
	nst1ports = 0;
	nse1ports = 0;
	cip_channel = 0;
	cip2_channel = 0;
	feip_feth = 0;

	/*
	 * Reset FDDI SMT data structure
	 */
	fddi_smt_init();

    } else {
	nethernets = nethernets - mec_eth - feip_feth - rvip_eth - rvip_feth;
	nTRs = nTRs - ctr_trs - rvip_tr;
        feip_feth = cip_channel = cip2_channel = nt1ports = ne1ports = fsip_ser = 
            ctr_trs = mec_eth = rvip_eth = rvip_feth =
		rvip_tr = rvip_ser = ssip_ser = 
		    nst1ports = nse1ports = 0;
    }
    nfddinets = 0;
    nhssinets = 0;
    nfethernets = 0;

    cbus = NULL;
    boardfailed = FALSE;
    nAIP = natmnets = 0;
    nMIP = nMEC = nCTR = nFSIP = nFIP = nHIP = 0;
    nCIP = nCIP2 = nFEIP = nRVIP = nchannelnets = 0;
    nSSIP = nSMIP = 0;
    dsx1_count = 0;

    if (!eoir_cycle) {
#ifdef DEBUG_PARALLEL
	ttyprintf (CONTTY,"Calling scan slots for cbus ctrlr\n");
#endif
	cbus_controllers = scan_slots_for_cbus_ctrlr(firsttime, eoir_cycle);
    } else {
	cbus_controllers = 1;	/* one guaranteed to be present for EOIR */
    }

    /*
     * Find starting number for each type of interface.
     */
    FOR_ALL_HWIDBS(idb) {
	if (idb->status & IDB_ETHER) {
	    if (idb->type == IDBTYPE_FEIP) {
		if (idb->unit > cbus_base_feth)
		    cbus_base_feth = idb->unit;
	    } else {
		if (idb->unit > cbus_mci_base_ether)
		    cbus_mci_base_ether = idb->unit;
	    }
	} else if (idb->status & IDB_SERIAL) {
	    if (idb->type == IDBTYPE_T1) {
		if (idb->unit > cbus_base_t1)
		    cbus_base_t1 = idb->unit;
	    } else if (idb->type == IDBTYPE_E1) {
		if (idb->unit > cbus_base_e1)
		    cbus_base_e1 = idb->unit;
	    } else if (idb->status & IDB_HSSI) {
		if (idb->unit > cbus_base_hssi)
		    cbus_base_hssi = idb->unit;
	    } else {
		if (idb->unit > cbus_mci_base_serial)
		    cbus_mci_base_serial = idb->unit;
	    }
	} else if (idb->status & IDB_FDDI) {
	    if (idb->unit > cbus_base_fddi)
		cbus_base_fddi = idb->unit;
	} else if (idb->status & IDB_TR) {
	    if (idb->unit > cbus_base_token)
		cbus_base_token = idb->unit;
	} else if (idb->status & IDB_ATM) {
	    if (idb->type == IDBTYPE_AIP) {
		if (idb->unit > cbus_base_aip)
		    cbus_base_aip = idb->unit;
	    }
	    else if (idb->type == IDBTYPE_CIP) {
		if (idb->unit > cbus_base_channel)
		    cbus_base_channel = idb->unit;
	    }
	}
    }

    /* fci_max_dci_ctrlrs now set in platform specific code */

    if (!cbus_controllers) {
	if (oir_debug)
	    buginf("\n Ending cbus_analyze(1)");
	return(0);
    }

    /*
     * Determine the number of controllers (interfaces) supported by cBus.
     */
    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	regptr = cbus->regptr;
	cbus->ether_count = cbus->fddi_count = 0;
	cbus->hssi_count = cbus->token_count = cbus->interface_count = 0;
	cbus->aip_count = 0;
	cbus->cip_count = 0;
	cbus->feip_count = 0;
	
        for (slot = 0; slot < fci_max_dci_ctrlrs; slot++)
            nx_dcis[slot] = NULL;
	
#ifdef C7000
	if (eoir_cycle) {
	    sp_eoir_reset(cbus);
	    cxbus_eoir_reset(cbus);
	}
#endif
	
	for (slot = 0; slot < fci_max_dci_ctrlrs; slot++) {
	    dci = NULL; dgbus = NULL; od = NULL;
	    newcard = FALSE;
	    nx_dcis[slot] = cbus->dci[slot];
#ifdef C7000
	    if (!(reg_invoke_cxbus_card_present(slot)))
		continue;
#endif
	    regptr->argreg = slot;
	    regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
	    sresult = regptr->cmdreg;
	    if (sresult != FCI_RSP_OKAY) {
		if (oir_debug) {
		    buginf("\n#Error %#x, from select slot %d",
			   sresult, slot);
		    bugtrace();
		}
		continue;
	    }
	    
	    /*
	     * The "who" command returns the type of controller present.
	     */
#ifdef C7000
	    dgbus = pslot_to_dbus(slot);
	    if (!(touch_device(dgbus))) {
		if (!firsttime && cbus->dci[slot] != NULL) {
		    reg_invoke_flush_dci(cbus, cbus->dci[slot]);
		    cbus->dci[slot] = NULL;
		}
		if (oir_debug) {
		    buginf("\n#Error, dgbus not present for slot %d", slot);
		    bugtrace();
		}
		continue;
	    }
	    if (slots[slot].flags & BOARD_DISABLED) {
		if (cbus->dci[slot] != NULL) {
		    reg_invoke_flush_dci(cbus, cbus->dci[slot]);
		    cbus->dci[slot] = NULL;
		}
		continue;
	    }
	    
	    sresult = reg_invoke_get_controller_type(dgbus);
#else
	    sresult = reg_invoke_get_controller_type(regptr);
#endif

#ifdef C7000
	    switch (sresult) {
	    case FCI_ETHER_CONTROLLER:
	    case FCI_SERIAL_CONTROLLER:
	    case FCI_LINK_CONTROLLER:
	    case FCI_CTR_CONTROLLER:
	    case FCI_FDDIT_CONTROLLER:
	    case FCI_FSIP_CONTROLLER:
	    case FCI_MIP_CONTROLLER:
	    case FCI_AIP_CONTROLLER:
	    case FCI_CIP_CONTROLLER:
	    case FCI_FEIP_CONTROLLER:
	    case FCI_RVIP_CONTROLLER:
		break;
	    default:
		errmsg(&msgsym(UNKNOWN, DBUS), slot, sresult);
		if (cbus->dci[slot] != NULL) {
		    reg_invoke_flush_dci(cbus, cbus->dci[slot]);
		    cbus->dci[slot] = NULL;
		}
		continue;
	    }
#else
	    sresult = reg_invoke_get_controller_type(regptr);

	    if ((sresult & 0x00FF) == 0x00FF)
		continue;
	    sresult = ((sresult >> 3) & 0xf);
#endif
	    /*
	     * Daughter controller exists -- create memory structure.
	     */
	    if (firsttime) {
		dci = malloc_named(sizeof(dcitype), "Daughter Card");
	    } else {
		dci = dequeue(&dcipreQ);
		if (dci) {
		    memset(dci, 0, sizeof(dcitype));
		}
	    }
	    if (!dci) {
		buginf("\nNo memory for daughter structure.");
		if (cbus->dci[slot] != NULL) {
		    reg_invoke_flush_dci(cbus, cbus->dci[slot]);
		    cbus->dci[slot] = NULL;
		}
		if (oir_debug) {
		    buginf("\n#Error, NO dci for slot %d", slot);
		    bugtrace();
		}
		continue;
	    }
	    od = cbus->dci[slot];
	    dci->ctrlr_type = sresult;
	    dci->regptr = regptr;
	    dci->fci = cbus;
	    dci->ctrlr_unit = slot;

            /*
	     * Setup for controller resync/recovery after
	     * FCI_RSP_ERR_TMOUT errors
	     */

	    dci->ctrlr_sync_count = CTRLR_RESYNC_INIT_VAL;
	    dci->ctrlr_sync_calls = 0;
	    dci->ctrlr_sync_times = 0;
	    dci->ctrlr_sync_phase = 0;
	    dci->ctrlr_sync_fails = 0;
	    dci->resyncable = cbus_ctrlr_resyncable(dci->ctrlr_type);

            for(idx=0; idx < 2; idx++)
                dci->pa_type[idx] = 0;
	    reg_invoke_priv_pa_info_op(idb, FALSE, slot);

	    /*
	     * Get the hardware/software version of the controller.
	     */
	    if (!fci_ctrlr_qry_arg_rslt(dci, regptr, &sresult, 
					FCI_CTRLR_VERSION, FCI_SW_VERSION, 
					&dci->sw_version, 
					CTRLR_CMD_ATTEMPTS, TRUE)) {
		errmsg(&msgsym(DAUGHTER_NO_RSP, CBUS), 
		       cbus->unit, dci->ctrlr_unit, sresult);
#ifdef C7000
		/*
		 * reset but do not restart the failed controller
		 */
		errmsg(&msgsym(DAUGHTER_STATS, DBUS), dci->ctrlr_unit,
		       dbus_prot_rd(&dgbus->diag_control),
		       dbus_prot_rd(&dgbus->diag_status));
		dbus_ctrlr_reset(dgbus); 
#else
		regptr->cmdreg = FCI_CMD_CTRLR_RESET;
#endif
		/*
		 * need to flush routing table, idb, etc. data
		 * structures of broken card!
		 */
		reg_invoke_flush_dci(cbus, dci);
		if (od)
		    reg_invoke_flush_dci(cbus, od);
		nx_dcis[slot] = NULL; 
		if (!firsttime && od) 
		    enqueue(&dcipreQ, od);
		else
		    free(od);
		od = NULL;
		reg_invoke_disable_slot(slot); /* disable the hardware */
		if (!firsttime)
		    enqueue(&dcipreQ, dci);
		else
		    free(dci);
		cbus->dci[slot] = 0;
		continue;
	    }

#ifdef C7000
	    dci->hw_version = reg_invoke_get_hw_version(dgbus, slot);
#else
	    regptr->argreg = FCI_HW_VERSION;
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	    regptr->argreg = FCI_CTRLR_VERSION;
	    regptr->cmdreg = FCI_CTRLR_CMD;
	    dci->hw_version = regptr->argreg;
#ifdef new
	    dci->hw_version = reg_invoke_get_hw_version(dgbus, slot);
#endif
#endif
	    /*
	     * If first time through cbus_analyze, test card.
	     * For performance reasons, don't test newly inserted cards. 
	     * Run some memory tests on the daughter controllers.
	     */
	    if (firsttime) {
		tptr = (uchar *) dci_memtest(dci); 
	    } else {
		tptr = NULL;
	    }

	    if (tptr) {
		errmsg(&msgsym(DAUGHTER, CBUS), cbus->unit,
		       dci->ctrlr_unit, tptr);
		errmsg(&msgsym(DAUGHTER_STATS, DBUS), dci->ctrlr_unit,
		       dbus_prot_rd(&dgbus->diag_control),
		       dbus_prot_rd(&dgbus->diag_status));
		reg_invoke_disable_slot(slot);
		boardfailed = TRUE; /* force retest of other controllers */
		reg_invoke_flush_dci(cbus, dci);
		if (od)
		    reg_invoke_flush_dci(cbus, od);
		if (!firsttime && od)
		    enqueue(&dcipreQ, od);
		else
		    free(od);
		od = NULL;
		if (!firsttime)
		    enqueue(&dcipreQ, dci);
		else
		    free(dci);
		cbus->dci[slot] = 0;
		continue;
	    }
#ifdef C7000
	    this_is_ssip_smip_pslot = ssip_smip_pslot(dci->ctrlr_unit);
	    this_is_cip2_pslot = is_cip2_hw(dci->ctrlr_unit);
#endif
	    switch (dci->ctrlr_type) {
	    case FCI_ETHER_CONTROLLER:	nMEC++; break;
	    case FCI_CTR_CONTROLLER:	nCTR++; break;
	    case FCI_FSIP_CONTROLLER:	
	    {
		if (this_is_ssip_smip_pslot)
		    nSSIP++;
		else
		    nFSIP++; 
	    }
	    break;
	    case FCI_FDDIT_CONTROLLER:  nFIP++; break;
	    case FCI_LINK_CONTROLLER:   nHIP++; break;
	    case FCI_AIP_CONTROLLER:	nAIP++; break;
	    case FCI_MIP_CONTROLLER:
	    {
		if (this_is_ssip_smip_pslot)
		    nSMIP++;
		else
		    nMIP++;
	    }
		break;
	    case FCI_CIP_CONTROLLER:
	    {
		if (this_is_cip2_pslot)
		    nCIP2++;
		else
		    nCIP++;
	    }
		break;
	    case FCI_FEIP_CONTROLLER:   nFEIP++; break;
	    case FCI_RVIP_CONTROLLER:   nRVIP++; break;
	    }
	    cbus->dci[slot] = dci;

	    /*
	     * Discover all interfaces supported by daughter controller.
	     */
  	    nif = 0;
	    if_unit = -1;
	    slotunit = -1;
	    while ((if_unit = 
		    cbus_get_next_if_unit(slot, &slotunit)) >= 0) {
		if (oir_debug)
		    buginf("\nselecting %d/%d = %d", slot, slotunit, if_unit);
		if (!fci_grab_if(dci, regptr, &sresult, if_unit, &if_type)) {

		    if (sresult == FCI_RSP_ERR_NPRS)
			continue;

		    if (oir_debug)
			buginf("\n#Error, select failed on if_unit %d, err %#x",
			       if_unit, sresult);

		    continue;
		}
		if (oir_debug)
		    buginf("\t0x%x 0x%x", sresult, if_type);
		if ((short)(sresult = if_type) >= 0) {
		    if (oir_debug)
			buginf("\tOK");
		    typecode = sresult & 0xFF;
		    switch (dci->ctrlr_type) {
		    case FCI_ETHER_CONTROLLER:
			if (FCITYPE(typecode) != FCI_TYPE_ETHER) {
			    nif = 0;
			    cdb = NULL;
			    break;
			}
			mec_eth++;
			cdb = controller_create(dci->ctrlr_type, PIFT_ETHERNET,
						if_unit, cbus, firsttime);
			if (cdb == NULL)
			    continue;
			cdb->idbtype = IDBTYPE_CBUSETHER;
			break;
		    case FCI_FDDI_CONTROLLER:
			cdb = controller_create(dci->ctrlr_type, PIFT_FDDI,
						if_unit, cbus, firsttime);
			if (cdb == NULL)
			    continue;
			cdb->idbtype = IDBTYPE_FDDI;
			break;
		    case FCI_FDDIT_CONTROLLER:
			cdb = controller_create(dci->ctrlr_type, PIFT_FDDI,
						if_unit, cbus, firsttime);
			if (cdb == NULL)
			    continue;
			cdb->idbtype = IDBTYPE_FDDIT;
			break;
		    case FCI_SERIAL_CONTROLLER:
			cdb = controller_create(dci->ctrlr_type, PIFT_SERIAL,
						if_unit, cbus, firsttime);
			if (cdb == NULL)
			    continue;
			cdb->idbtype = IDBTYPE_MCISERIAL;
			break;
		    case FCI_LINK_CONTROLLER:
			cdb = controller_create(dci->ctrlr_type, PIFT_HSSI,
						if_unit, cbus, firsttime);
			if (cdb == NULL)
			    continue;
			cdb->idbtype = IDBTYPE_HSSI;
			break;
                    case FCI_FSIP_CONTROLLER:
                        if (FCITYPE(typecode) != FCI_TYPE_SERIAL) {
                            nif = 0;
			    cdb = NULL;
                            break;
                        }
			if (this_is_ssip_smip_pslot)
			    ssip_ser++;
			else
			    fsip_ser++; 
			cdb = controller_create(dci->ctrlr_type, PIFT_SERIAL,
						if_unit, cbus, firsttime);
			if (cdb == NULL)
			    continue;
			cdb->idbtype = IDBTYPE_CBUSSERIAL;
			break;
		    case FCI_CTR_CONTROLLER:
			if (FCITYPE(typecode) != FCI_TYPE_CTR) {
			    nif = 0;
			    cdb = NULL;
			    break;
			}
			cdb = controller_create(dci->ctrlr_type,PIFT_TOKENRING,
						if_unit, cbus, firsttime);
			if (cdb == NULL)
			    continue;
			cdb->idbtype = IDBTYPE_CTR;
			ctr_trs++;
			break;
#ifdef C7000
		    case FCI_AIP_CONTROLLER:
			cdb = controller_create(dci->ctrlr_type,PIFT_ATM,
						if_unit, cbus, firsttime);
			if (cdb == NULL)
			    continue;
			cdb->idbtype = IDBTYPE_AIP;
			cdb->love_note = aip_love_note;
			break;
		    case FCI_MIP_CONTROLLER:
		    {
			int sv_nt1ports = nt1ports + ne1ports +
			    nst1ports + nse1ports;
			int appl_no = slotunit;
			int applique_type;
			hwidbtype *hwidb;

			/*
			 * Before calling create_context() we need to
			 * determine the applique type.
			 */
			regptr->argreg = appl_no;
			regptr->cmdreg = FCI_CMD_APPLIQUE;
			applique_type = regptr->argreg;

			cdb = mip_create_context((void *)cbus,if_unit,sresult,
						 firsttime, applique_type);
			if (cdb == NULL)
			    continue;
			dsx1_count += nt1ports + ne1ports + nst1ports +
			    nse1ports - sv_nt1ports;

			/*
			 * If the MIP hardware was reinserted, mark its
			 * hardware present in all of its IDBs, and count
			 * the serial interfaces
			 */
			FOR_ALL_HWIDBS(hwidb) {
			    if ( (hwidb->slot == slot) &&
				(hwidb->mci_index == if_unit) ) {
				if (hwidb->type == cdb->idbtype) {

				    if (oir_debug)
					buginf("\nFound hwidb %#x for MIP in slot %d, if_unit %d, idbtype %d",
					       hwidb, slot, if_unit,
					       cdb->idbtype);
				    nserialnets++;
				}
			    }
			}

			break;
		    }
		    case FCI_CIP_CONTROLLER:
			cdb = controller_create(dci->ctrlr_type,PIFT_CHANNEL, 
						if_unit, cbus, firsttime);
			cdb->idbtype = IDBTYPE_CIP;
			if (this_is_cip2_pslot)
			    cip2_channel++;
			else			
			    cip_channel++;
			break;
		    case FCI_FEIP_CONTROLLER:
			cdb = controller_create(dci->ctrlr_type,
						PIFT_FASTETHER,  
						if_unit, cbus, firsttime);
			if (cdb == NULL)
			    continue;
			cdb->idbtype = IDBTYPE_FEIP;
			feip_feth++;
			break;
		    case FCI_RVIP_CONTROLLER:
			{
   	                enum IDBTYPE idbtype;
			enum pif_types_ pift;
			int pa_type, bay;

			pa_type = if_type;
			bay = cbus_get_pa_from_slotunit(slotunit);
			dci->pa_type[bay] = pa_type;
			if_type = vip_get_if_type(pa_type, &pift, &idbtype);
			cdb = controller_create(dci->ctrlr_type, pift, 
						if_unit, cbus, firsttime);
			cdb->idbtype = idbtype;

			/* Increment the appropriate interface counter */

			switch (idbtype) {
			    case IDBTYPE_CBUSETHER:
			        rvip_eth++;
				break;
			    case IDBTYPE_CBUSSERIAL:
				rvip_ser++;
				break;
			    case IDBTYPE_CTR:
				rvip_tr++;
				break;
			    case IDBTYPE_FEIP:
				rvip_feth++;
				break;
			    default:
				break;
			    }
		        }
			break;
#endif
		    default:
			buginf("\nSlot %d/%d (unit %d): "
			       "Unknown controller type %d",
			       slot, slotunit, if_unit, dci->ctrlr_type);
			cdb = NULL;
			break;
		    }

		    if (firsttime) {
			hwidbtype *tmp;

			newcard = TRUE;
			nx_dcis[slot] = NULL;
			/*
			 *  MIP card gets its IDBs set up by parsing NVRAM
			 */
			if (dci->ctrlr_type != FCI_MIP_CONTROLLER) {
			    (void)cbus_mci_idb_init(cbus,if_unit, if_type,
						    NULL, NULL);
			    for (tmp = cbus->interface_list[if_unit] ; tmp != NULL
				 ; tmp = tmp->next_idb_on_physint)
				/* setup code will handle init */
				tmp->oir_flags &= ~IDB_NEEDS_INIT;
			}
		    } else {
			/* okay, this requires some thought */
			if (od == NULL) {
			    /* no card previously in slot, add idbs at will */
			    newcard = TRUE;
			    nx_dcis[slot] = NULL;

			    if (dci->ctrlr_type != FCI_MIP_CONTROLLER) {
				idb = idb_matching_slot(cbus, dci, slot,
							if_unit,
							dci->ctrlr_type);
				if (idb != NULL) {

				    /*
				     *  okay, we found an idb that matches
				     *  for this spot, so mark the existing
				     *  idb as not present, in case it was
				     *  for another type of card.
				     */
				    if (cbus->interface_list[if_unit] != NULL) {
					hwidbtype *ridb;
					ridb = cbus->interface_list[if_unit];
					if (oir_debug)
					    buginf("\nMarking %s (hwidb %#x) not present, slot %d, FIRST",
						   ridb->hw_namestring, ridb,
						   ridb->slot);
					mark_hwidbs_nohardware(ridb);
				    }

				    /*
				     * Save the new idb
				     */
				    cbus->interface_list[if_unit] = idb;

				    /*
				     * And mark its hardware present.
				     */
				    mark_hwidbs_hardware_present(idb);

				    (void)cbus_mci_idb_init(cbus, if_unit,
							    if_type,
							    idb, NULL);
				    idb->oir_flags &= ~IDB_NEEDS_INIT;
				                     /* not newly created */
				} else {
				    (void)cbus_mci_idb_init(cbus, if_unit,
							    if_type,
							    NULL, NULL);
				}
			    } else {

				/*
				 *  The MIP card is very different.  The
				 *  idb's will be dealt with from the driver,
				 *  we need to make sure that we don't
				 *  have them re-initialized (as that would
				 *  disable IP, among other things).  Should
				 *  probably be based on a flag in the IDB,
				 *  but as there would be a 1:1 correspondence
				 *  with FCI_MIP_CONTROLLER, we'll just use
				 *  that for now.
				 *
				 *  Also mark any idbs for other cards in the
				 *  same slot as no hardware present
				 */
				FOR_ALL_HWIDBS (idb) {
				    if ( (idb->slot == slot) &&
					 (idb->mci_index == if_unit) ) {
				        if (idb->type == cdb->idbtype) {

					    /*
					     * Found hwidb for this MIP
					     */
					    if (idb->status & IDB_NOHARDWARE)
						mark_hwidbs_hardware_present(idb);
					    idb->oir_flags &= ~IDB_NEEDS_INIT;

					    /*
					     * Save the new idb
					     */
					    cbus->interface_list[if_unit] = idb;
					} else {
					    /*
					     * Non MIP hwidb for this slot,
					     * mark it not present
					     */
					    if (oir_debug)
						buginf("\nMarking %s (hwidb %#x) not present, slot %d, SECOND",
						       idb->hw_namestring, idb,
						       idb->slot);
					    mark_hwidbs_nohardware(idb);
					}
				    }
				}
			    }
			} else {
			    /* there was some sort of card there */
			    if (od->ctrlr_type == dci->ctrlr_type) {
				nx_dcis[slot] = NULL;

				if (dci->ctrlr_type != FCI_MIP_CONTROLLER) {
				    hwidbtype *old_idb;

				    /*
				     * We don't want to add new idbs for 
				     * existing interfaces, but it is
				     * possible for "micro reload" to find
				     * new interfaces (e.g. firmware crashed
				     * on initial load).  For those cases we
				     * need  to change the state to
				     * IDBS_ADMINDOWN to cause the oir code
				     * to initialized the protocol fields
				     * for this new interface. 
				     */
				    old_idb = cbus->interface_list[if_unit];
				    (void)cbus_mci_idb_init(cbus, if_unit,
					          if_type,
						  old_idb,
						  NULL);
				    if (old_idb == NULL) {
					hwidbtype *new_idb;
					new_idb = cbus->interface_list[if_unit];
					if (new_idb) {
					    /*
					     * new idb was created
					     */
					    new_idb->state = IDBS_ADMINDOWN;
					}
				    }
					
				}
			    } else {
/*
 * this case shouldn't happen except in artificial conditions.  we would need
 * to be stalled continuously while someone pulled out an old card and put
 * in a new one.  but, be careful anyway..
 */
				/*
				 *  was a new and different card, add
				 *  interfaces
				 */
				newcard = TRUE;
				nx_dcis[slot] = od;
				if (dci->ctrlr_type != FCI_MIP_CONTROLLER) {
				    (void)cbus_mci_idb_init(cbus, if_unit,
							    if_type,
							    NULL, NULL);
				}
			    }
			}
		    }
		    nif++;
		}
	    }

	    if (od == NULL) {
		add_cbus_dci(dci, slot);
#ifdef C7000
	    	switch (dci->ctrlr_type) { 
	          case FCI_RVIP_CONTROLLER:
		    {
		        char* card_descr;
    		        card_type_t card_type;
		        int bay, pa_type;
    
		        for (bay = 0; bay < PA_MAX_BAYS ; bay++) {
			    pa_type = dci->pa_type[bay];
			    switch (pa_type) {
			      case FCI_TYPE_VIP_ETHER8:
			      case FCI_TYPE_VIP_FETHER:
			      case FCI_TYPE_VIP_FETHER_TX_NISL:
			      case FCI_TYPE_VIP_FETHER_TX_ISL:
			      case FCI_TYPE_VIP_FETHER_FX_ISL:
			      case FCI_TYPE_VIP_SERIAL4:
			      case FCI_TYPE_VIP_TRING4:
				if ((pa_info_ptr = slots[slot].pa_info[bay])) {
				    sub_card_type_and_descr(pa_type, 
					pa_info_ptr->pa_ifcount, 
					&card_type, &card_descr);
                	            reg_invoke_chassismib_add_sub_card_entry(
                                        card_type, 
					(void*)((int) dci->ctrlr_unit + 1),
					card_descr, 0, "", "", bay, 
					reg_invoke_chassismib_find_slot_card_index(slot),
					0);
				}
			        break;
			      default:
			        break;
			    }	
		        }
		    }
		    break;
		  default:
		    break;
	        }
#endif
	    }

	    if ( (nif == 0) && (dci->ctrlr_type != FCI_MIP_CONTROLLER)
#ifdef C7000
		&& (!vip_controller(dci->ctrlr_type))
#endif
		) {

		switch (dci->ctrlr_type) {
		case FCI_ETHER_CONTROLLER:
#ifdef C7000
		    p = "EIP";
#else
		    p = "MEC";
#endif
		    break;
		case FCI_FDDI_CONTROLLER:
		    p = "FDDI";
		    break;
		case FCI_CTR_CONTROLLER:
#ifdef C7000
		    p = "TRIP";
#else
		    p = "CTR";
#endif
		    break;
		case FCI_LINK_CONTROLLER:
#ifdef C7000
		    p = "HIP";
#else
		    p = "HSCI";
#endif
		    break;
		case FCI_FDDIT_CONTROLLER:
#ifdef C7000
		    p = "FIP";
#else
		    p = "FDDIT";
#endif
		    break;
#ifdef C7000
		case FCI_FSIP_CONTROLLER:
		    p = "FSIP";
		    break;

		case FCI_AIP_CONTROLLER:
		    p = "AIP";
		    break;

		case FCI_CIP_CONTROLLER:
		    p = "CIP";
		    break;
#endif

#ifdef C7000
		case FCI_FEIP_CONTROLLER:
		    p = "FEIP";
		    break;
#endif

#ifdef C7000
                case FCI_RVIP_CONTROLLER:
                    p = "VIP";
                    break;
#endif
		  default:
		    p = "unknown";
		    break;
		}
#ifdef C7000
		printf("\nSwitch Processor %d (%s-%d): no usable interfaces\n\n",
		       cbus->unit, p, dci->ctrlr_unit);
#else
		printf("\ncBus%d (%s-%d): no usable interfaces\n\n",
		       cbus->unit, p, dci->ctrlr_unit);
#endif
		errmsg(&msgsym(NOCOOKIE,CBUS), p, dci->ctrlr_unit);
		init_errors++;
		reg_invoke_flush_dci(cbus, dci);
		if (!firsttime)
		    enqueue(&dcipreQ, dci);
		else
		    free(dci);
		cbus->dci[slot] = 0;
		continue;
	    }
	    /*
	     * new card inserted, put all interfaces in "new, shutdown" state.
	     * if the cards are on configurable controllers, do nothing--
	     * the driver will handle everything.
	     */
	    if (newcard && !firsttime) {
		errmsg(&msgsym(INSCARD, OIR), slot);
#ifdef C7000
		reg_invoke_send_ipc_eoir_signal(dci->ctrlr_unit);
#endif
		slotunit = -1;
		while ((if_unit = 
			cbus_get_next_if_unit(slot, &slotunit)) >= 0) {  
		    cdb = controller_find(slot, slotunit);
		    /* hmm, isn't it an error if cdb == NULL */
		    if (cdb && cdb->cdb_status & CDB_CONFIGURE) {
			;	/* driver does all the work */
		    } else {
			for (idb = cbus->interface_list[if_unit] ; idb != NULL
			     ; idb = idb->next_idb_on_physint) {
			    idb->state = IDBS_ADMINDOWN;
			    /*
			     * It is possible that this is not really a newly
			     * inserted card, but instead, the initial 
			     * load of an IP failed, and we are now just 
			     * seeing the card via micro reload.
			     * Still mark the interfaces as shutdown, but
			     * mark the pre_hotstall_state as down, so that 
			     * start_inserted_interfaces() can bring 'em 
			     * all up.
			     */
			    if (!eoir_cycle) {
				idb->pre_hotstall_state = IDBS_DOWN;
			    }
			}
		    }
		}
	    }
	}
	/* 
	 * Existing card removed, put all its idbs in "removed, shutdown" state
	 *
	 * This code assumes that it is impossible to swap cards on a single
	 * hot stall event.
	 */
	if (!firsttime) {
	    for (slot = 0; slot < fci_max_dci_ctrlrs; slot++) {
		dcitype* dci;

		dci = nx_dcis[slot];
		if (dci != NULL) {
		    errmsg(&msgsym(REMCARD, OIR), slot);
#ifdef C7000
            	    switch (dci->ctrlr_type) {
		      case FCI_RVIP_CONTROLLER: {
			  int bay;

			  for (bay = 0; bay < PA_MAX_BAYS ; bay++) {
			      /* pa_type is maintained in the dci controller
                               * structure for 7000s, not in slots like the
                               * 7500s. pa_ifcount is kept in slots for both.
			       */
			      if ((pa_info_ptr = slots[slot].pa_info[bay]))
				  pa_info_ptr->pa_ifcount = 0;
			  }
		        } 
			break;
		      default:
		        break;
            	    }
    		    reg_invoke_send_ipc_eoir_signal(slot);
#endif
		    reg_invoke_chassismib_remove_card_entry(
					   (void*)((int) dci->ctrlr_unit + 1));

		    slotunit = -1;
		    while ((if_unit =
			    cbus_get_next_if_unit(slot, &slotunit)) >= 0) { 
			if ((cdb = controller_find_by_unit(if_unit)) != NULL)
			    controller_remove(cdb);
			idb = cbus->interface_list[if_unit];
			if (idb) {
			    if (oir_debug)
				buginf("\nMarking %s (hwidb %#x) not present, slot %d, THIRD",
				       idb->hw_namestring, idb,
				       idb->slot);
			    mark_hwidbs_nohardware(idb);
			}
			cbus->interface_list[if_unit] = NULL;
		    }
		    enqueue(&dcipreQ, dci);
		}
	    }
	}
	/* 
	 * If we found a failed board, test the others as well.
	 */
	if (boardfailed) {
	    for (slot = 0; slot < fci_max_dci_ctrlrs; slot++) {
		if (reg_invoke_slot_enabled(slot) &&
		    reg_invoke_cxbus_card_present(slot)) {
		    dci = cbus->dci[slot];

		    tptr = (uchar *)dci_memtest(dci);
		    if (tptr) {
			/* controller failed test, report why */
			errmsg(&msgsym(DAUGHTER, CBUS), cbus->unit,
			       slot, tptr);
			errmsg(&msgsym(DAUGHTER_STATS, DBUS), dci->ctrlr_unit,
			       dbus_prot_rd(&dgbus->diag_control),
			       dbus_prot_rd(&dgbus->diag_status));
			reg_invoke_flush_dci(cbus, dci);
			reg_invoke_disable_slot(slot);
		    }
		}
	    }
	}
	/*
	 * Initialize the cBus, now that we know the numbers of dci.
	 */
	if (eoir_cycle)
	    cbus->flags |= EOIR_INIT;
#ifdef DEBUG_PARALLEL
	ttyprintf (CONTTY,"Calling cbus_init\n");
#endif
	cbus_init(cbus, !firsttime ? cbus->critical : FALSE);
	cbus->flags &= ~EOIR_INIT;
	cbus->configuring = FALSE;

	/*
	 *  For controllers who need to do post hotswap resetting, but
	 *  have to wait until all variables are settled down.
	 *  Respect EOIR.
	 */
	FOR_ALL_CDBS(cdb) {
	    if (cdb->reset != NULL) {
#ifdef C7000
		slottype* sp;

		sp = &slots[cdb->slot];
		if ( (eoir_cycle) && sp && (! (sp->flags & BOARD_RELOADED) ) )
		    cdb->cdb_status |= CDB_EOIR_RESET;
#endif
		(cdb->reset)(cdb);
		cdb->cdb_status &= ~CDB_EOIR_RESET;	/* Clear it now */
	    }
	}
    }
    
    if ((nMCI) && (cbus_controllers))
	lev3ssubr = cbus_interrupt;

    /*
     * If people had slower updating screens, maybe they'd write
     * shorter subroutines.  Anyway, let's register some functions. -Kirk
     */
    if (firsttime) {

	/*
	 * Initialize cbus debugging
	 */
	cbus_debug_init();

	/*
	 * Add 'cbus commands' now; This was earlier done in hes/if_hes_serial.c
	 * * coding here eliminates the need for stubs in other platforms.
	 */
	parser_add_commands(PARSE_ADD_CFG_INT_CMD,
			&pname(cbus_buffers_interface_commands),
			"cbus buffers");

	/*
	 * periodic wedged-interface checker
	 */
	reg_add_if_onesec(cbus_restart_check, "cbus_restart_check");
    }

    if (oir_debug)
	buginf("\n Ending cbus_analyze(2)");

    return(cbus_controllers);
}


/*
 * cbus_fragment_packet
 * Given a packet up from autonomous or SSE switching, fragment it.
 */

static inline void cbus_fragment_packet (mcitype *cbus, mciregtype *regptr,
					 ushort index, ushort summary)
{
    hwidbtype *idb, *outidb;

    /*
     * Read the input interface.
     */
    idb = cbus->interface_list[index];
    if (NULL == idb)
	goto fail;
    if (idb->mci_index != index)
	goto fail;

    /*
     * Read the output interface.  We could actually defer getting the
     * output interface, but we need the cbus structure to figure out the
     * idb.  It's easier to do here. 
     */
    index = regptr->rxstatus;
    if ((short) index == -1) 
	goto fail;
    if (index > (cbus->max_interfaces - 1))
	goto fail;

    outidb = cbus->interface_list[index];
    if ((NULL == outidb) || (outidb->mci_index != index))
	goto fail;
	
    /*
     * Get the RXTYPE and length.  Kludge here because FDDI passes shit in
     * the length register.
     */
    index = regptr->rxtype;
    idb->rxlength = regptr->rxlength;
    switch (idb->type) {
    case IDBTYPE_FDDIT:
	idb->rxlength &= ~FDDIT_RIF_CAM;
	idb->rxlength++;		/* ucode bug */
	break;
    default:
	break;
    }
    if (zebra_debug)
	buginf("\nCBUS: mongogram length %#x", idb->rxlength);

    /*
     * Real routines will need to do
     *
     * idb->rxrifwordlen = regptr->rxsrbinfo;
     *
     * where the high order byte of the rxrifwordlen is the input mac
     * length and the low order byte is the output mac length.
     */

    if (reg_invoke_fast_fragmentation(index, idb, outidb))
	return;

 fail:
    regptr->cmdreg = FCI_CMD_RX_FLUSH;
/*
Useful debugging code:
  
    if (zebra_debug)
	buginf("\nCBUS: mongogram on %s to %s, len %u, rxtype %#x, "
	       "inmaclen %d, outmaclen %d, rxoffset %d",
	       idb->firstsw->namestring, 
	       outidb->firstsw->namestring, idb->rxlength,
	       idb->rxtype, idb->rxrifwordlen >> 8, idb->rxrifwordlen &
	       0xff, idb->rxoffset);
*/
}


/*
 * CBUS_MULTICAST_SRB_EXPLORER
 * Is this a multicast explorer that we want to bridge?
 */
#define CBUS_MULTICAST_SRB_EXPLORER(status,srbinfo,idb,rxtype,sap_hdr) \
      (  (status  & (FCI_MULTICAST_MASK << 8)) 				\
      && (srbinfo &  FCI_RXSRB_PRESENT)					\
      && (srbinfo &  FCI_RXSRB_EXPLORER)				\
      && (multicast_explorer_can_be_bridged(idb,rxtype,sap_hdr)) )

/*
 * CBUS_ETHER_RECEIVE
 * Receive a single datagram on an Ethernet interface.
 * This is a macro rather than a subroutine to eliminate the overhead
 * of a subroutine call.  Debug with an oscilloscope!
 */

#define CBUS_ETHER_RECEIVE						\
									\
    /*									\
     * Set fast_rcvidb to point to the main interface swidb first.	\
     * It then demux to point to the right sub-interface swidb, if an	\
     * ISL packet received, in function isl_fastswitch().		\
     */									\
									\
    idb->fast_rcvidb = idb->firstsw;					\
									\
    if (bytes >= MINETHERSIZE) {		    			\
	                                                                \
        /*								\
         * Determine whether we should bridge this datagram.		\
	 * Attempt to bridge any packet with the bridge bit set		\
	 * that does not also have the station bit set.			\
	 * This attempt will succeed if the packet was forwarded,	\
	 * flooded, or filtered by the transparent bridge code.		\
	 * This attempt will fail for a variety of packets including	\
	 * interesting multicasts (aliases will be flooded), and	\
	 * physical broadcasts in protocols we are routing (other	\
	 * physical broadcasts will be flooded).			\
         */								\
									\
        status = regptr->rxstatus;					\
	if ((status & ((FCI_BRIDGING_MASK | FCI_STATION_MASK) << 8)) ==	\
	    (FCI_BRIDGING_MASK << 8)) {					\
	    /*								\
	     * Attempt to bridge only those packets that have the	\
	     * bridge bit set but DO NOT have the station bit set.	\
	     * Anything with the station bit set falls through to the	\
	     * fast switch code.					\
	     */								\
	    if (reg_invoke_tbridge_mci_forward(idb->fast_rcvidb)) {	\
		goto rxexit;						\
	    /*								\
	     * The attempt to bridge failed.  Before assuming the packet\
	     * is to be received, let IP multicasts fall through to the	\
	     * fast switch code. (CSCdi41429)				\
	     */								\
	    } else if ((status & (FCI_MULTICAST_MASK << 8)) &&		\
		       (RXTYPE_DODIP == regptr->rxtype)) {		\
		/*							\
		 * Restore the rx pointer to the offset expected	\
		 * by the ip fastswitch code.				\
		 */							\
		regptr->argreg = MCI_OFFSETSIZE + MCI_IP_DSTADR_OFFSET;	\
		regptr->cmdreg = MCI_CMD_RX_SELECT;			\
	    } else {							\
		/*							\
		 * Receive everything else.				\
		 */							\
		goto erxread;						\
	    }								\
        }								\
									\
        /*								\
         * Very high speed switching of routed protocols	        \
         */								\
	summary = regptr->rxtype;					\
									\
	if (reg_invoke_ether_fs(summary, idb))	 			\
	    goto rxexit;						\
									\
	else if (summary == RXTYPE_UNKNOWN) {                   	\
	    if (reg_invoke_ether_sniff_encap(idb->type, cbus, idb)) {   \
		regptr->cmdreg = MCI_CMD_RX_FLUSH;		        \
		goto rxexit;		                                \
	    } else if (reg_invoke_media_sniff_ipc(cbus, idb,		\
					MCI_ETHER_OFFSET+6)) {		\
	        regptr->cmdreg = MCI_CMD_RX_FLUSH;                      \
	        goto rxexit;                                            \
	    }								\
	    goto erxread;	                                        \
	} else				        		        \
	    goto erxread;                                       	\
    } else {								\
	/*								\
	 * Datagram not within allowed size.				\
         */             						\
        regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	idb->counters.input_error++;					\
	idb->counters.runts++;						\
        goto rxexit;							\
    }									\
    /*									\
     * Can't get here because of gotos					\
     */									\
									\
    /*									\
     * Sniff start of packet						\
     */									\
									\
erxread:								\
									\
    regptr->argreg = MCI_ETHER_OFFSET + 2*IEEEWORDS;			\
    regptr->cmdreg = MCI_CMD_RX_SELECT;					\
									\
    /*									\
     * Read in the ethernet encapsulation junk.  Both			\
     * reg_invoke_hes_bridge_broadcasts() and the check before          \
     * mci_vring_input() use this.					\
     */									\
    idb->sniff[SNIFFBASE+0].d.lword = regptr->readlong;			\
    idb->sniff[SNIFFBASE+1].d.lword = regptr->readlong;			\
    idb->sniff[SNIFFBASE+2].d.lword = regptr->readlong;			\
									\
    /*									\
     * Check for fast RSRB. This is 802.2 so the offsets are not        \
     * the same as they would be if it was an ethernet frame.           \
     */									\
    if ((idb->sniff[SNIFFBASE+0].d.sword[1] == SNAPSNAP) &&		\
	(idb->sniff[SNIFFBASE+2].d.sword[0] == TYPE_RSRB))		\
	    if (reg_invoke_mci_vring_input(idb))			\
                goto rxexit;						\
									\
    if (ROUTING_RXTYPE(idb->fast_rcvidb, summary)) {			\
	/*								\
	 * Routing the protocol.  Check to see if IP and if so		\
	 * check for fast UDP flood.					\
	 * 								\
	 * We piggy packed on the last readlong above to scrape off	\
	 * the IP packet type if this is an ip/udp datagram.		\
	 * 								\
	 * Note: udp_fast_flood() relies on the rx pointer to be	\
	 * positioned at the ip checksum.				\
	 */								\
	if (summary == RXTYPE_DODIP) {					\
	    switch (idb->sniff[SNIFFBASE+2].d.byte[3]) {		\
	    case UDP_PROT:						\
		if (reg_invoke_udp_fast_flood(idb->fast_rcvidb))	\
		    goto rxexit;					\
		break;							\
	    case FST_DLSW_PROT:						\
		if (reg_invoke_mci_dlsw_fst_forward(idb))		\
		    goto rxexit;					\
		break;							\
	    case FST_RSRB_PROT:						\
		if (reg_invoke_mci_rsrb_fst_forward(idb))		\
		    goto rxexit;					\
		break;							\
	    case GRE_PROT:						\
	    case IPINIP_PROT:						\
		if (reg_invoke_tunnel_ip_hes_fs(idb))	                \
		    goto rxexit;					\
		break;							\
	    }								\
	}								\
    }									\
									\
    /*									\
     * We are going to receive this datagram.				\
     * Get a packet buffer and set it up.				\
     * If we can't get a buffer, assume some sort of congestion problem \
     * and disable the interface for a very short while so the process  \
     * level code can run and recover.					\
     */									\
									\
    /* rxlength is adjust for ISL packets */                            \
    bytes = idb->rxlength;                                              \
    pak = input_getbuffer(bytes, idb);					\
    if (!pak) {								\
        cbus_mci_idb_throttle(idb);					\
        goto rxexit;							\
    }									\
    pak->datagramsize = bytes;						\
    (*idb->parse_packet)(idb, pak, &idb->sniff[SNIFFBASE+0].d.byte[0]);	\
    if (status & idb->multibits)					\
	idb->counters.input_broad++;					\
    if (idb->mci_checked) {						\
	pak->flags |= PAK_MCICHECKED;					\
	idb->mci_checked = FALSE;					\
    }									\
    									\
    if (idb->ipmfast_switched) {				       	\
	pak->flags |= PAK_MFS_SWITCHED;    			        \
	idb->ipmfast_switched = FALSE;					\
    }	                                                                \
    /*									\
     * Set pointer to start of packet and read it into packet buffer.	\
     * Hand to raw input process and flush datagram in MCI RX buffer.	\
     */									\
    regptr->argreg = MCI_ETHER_OFFSET;					\
    regptr->cmdreg = MCI_CMD_RX_SELECT;					\
    mci2buffer(&regptr->readlong, pak->datagramstart, bytes);		\
    regptr->cmdreg = MCI_CMD_RX_FLUSH;					\
    idb->counters.rx_cumbytes += bytes;					\
    idb->counters.inputs++;						\
    if (pak->enctype == ET_LLC2)					\
        llc_input(pak);		                			\
    else								\
	(*(idb->iqueue)) (idb, pak);					\
    goto rxexit;

/*
 * CBUS_HSSI_RECEIVE
 * Receive a single datagram on a ds3 interface.
 * This is a macro rather than a subroutine to eliminate the overhead
 * of a subroutine call.  Debug with an oscilloscope!
 *
 * NOTE: any changes to this code should also be made to MIP_RECEIVE
 */

#define CBUS_HSSI_RECEIVE						\
    charlong sniffcode;							\
    /*									\
     * Very high speed switching of routed protocols			\
     */									\
    summary = regptr->rxtype;						\
    switch (idb->enctype) {						\
    case ET_SMDS:							\
       /*								\
        * Switch to correct subinterface - we need to sniff the dest	\
        * address							\
        */								\
	regptr->argreg = idb->rxoffset +				\
	    btow(idb->smds_pdb->fast_input_modelen) + 2;		\
        regptr->cmdreg = MCI_CMD_RX_SELECT;				\
        PUTLONG(idb->smds_pdb->fast_smds_dest, regptr->readlong);	\
        PUTLONG(&idb->smds_pdb->fast_smds_dest[4], regptr->readlong);	\
	idb->fast_rcvidb =						\
	    reg_invoke_media_addr_to_swidb(ET_SMDS,			\
					   idb->smds_pdb->fast_smds_dest, \
					   idb);			\
	if (idb->fast_rcvidb == NULL) goto srxprocess;			\
	if (summary == RXTYPE_UNKNOWN) {				\
           regptr->argreg = MCI_SERIAL_SMDS_OFFSET + sizeof(smdstype)/2;\
           regptr->cmdreg = MCI_CMD_RX_SELECT;				\
	   temp = regptr->readshort;					\
           switch (temp) {						\
	   case TYPE_CLNS:						\
               summary = RXTYPE_UNKNOWN;				\
	       goto srxread;						\
	   case TYPE_VINES:						\
	   case TYPE_VINES2:						\
               summary = RXTYPE_VINES;					\
	       goto ssmds_fs;						\
	   case TYPE_ETHERTALK:						\
               summary = RXTYPE_APPLE;					\
	       goto ssmds_fs;						\
	   case TYPE_NOVELL1:						\
               summary = RXTYPE_NOVELL1;				\
	       goto ssmds_fs;						\
	   }								\
	   regptr->argreg = idb->rxoffset + btow(idb->encsize) - 2;	\
	   regptr->cmdreg = MCI_CMD_RX_SELECT;				\
	   temp = regptr->readshort;					\
	   if (temp == SMDS_BRIDGE_OUI) {				\
	        idb->sniff[SNIFFBASE+0].d.sword[0] = regptr->readshort; \
	        goto srxbridge;						\
	    }								\
	    idb->sniff[SNIFFBASE+0].d.lword = 0;			\
	    goto srx_skip;						\
	} 	 							\
	ssmds_fs:							\
        regptr->argreg = MCI_OFFSETSIZE + MCI_IP_DSTADR_OFFSET;		\
        regptr->cmdreg = MCI_CMD_RX_SELECT;				\
        if (reg_invoke_smds_fs(summary, idb))				\
            goto rxexit;						\
	idb->sniff[SNIFFBASE+0].d.lword = 0;				\
	goto srx_skip;							\
	break;								\
    case ET_HDLC:							\
        if (reg_invoke_hdlc_fs(summary, idb))				\
            goto rxexit;						\
	break;								\
    case ET_PPP:							\
	if (summary == RXTYPE_UNKNOWN) {				\
	    regptr->argreg = MCI_OFFSETSIZE - 1;			\
	    regptr->cmdreg = MCI_CMD_RX_SELECT;				\
            summary = regptr->readshort;				\
	    summary = ppp_decode_rxtype_inline(summary);		\
	}								\
	if (    (summary != RXTYPE_UNKNOWN)				\
            && reg_invoke_ppp_fs(summary, idb))				\
	    goto rxexit;						\
        break;								\
    case ET_FRAME_RELAY:						\
	if (reg_invoke_fr_pvc_switch_or_fs(idb))			\
	    goto rxexit;						\
        /*								\
	 * sniff buffer already set up					\
	 */								\
	goto srx_skip;							\
	break;								\
    case ET_ATM_DXI:							\
	if (reg_invoke_atm_dxi_classify_pak_and_fs(idb))		\
	   goto rxexit;							\
	break;								\
    case ET_STUN:                                                       \
    case ET_SDLCP:                                                      \
    case ET_SDLCS:                                                      \
        /* CSCdi41558, without this some frames recognized as LEX */    \
        goto srxprocess;                                                \
        break;                                                          \
    default:								\
	goto srxread;							\
    }									\
									\
    /*									\
     * Sniff one long word to get protocol information			\
     */									\
srxread:								\
    regptr->argreg = idb->rxoffset;					\
    regptr->cmdreg = FCI_CMD_RX_SELECT;					\
    idb->sniff[SNIFFBASE+0].d.lword = regptr->readlong;			\
    sniffcode.d.lword = idb->sniff[SNIFFBASE+0].d.lword;		\
									\
    /*									\
     * Handle possible bridging functions.				\
     */									\
    if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_PPP_BRIDGE) { 	\
       summary = RXTYPE_TRANSIT;					\
       /* read in ppp bridge encaps */					\
       idb->sniff[SNIFFBASE+1].d.sword[0] = regptr->readshort;		\
       if (idb->sniff[SNIFFBASE+1].d.sword[0] != PPP_FIZC_MACTYPE) {	\
	   goto srxprocess;						\
       }								\
       goto srxbridge;							\
    }									\
    if (idb->sniff[SNIFFBASE+0].d.lword == HDLC_BRIDGECODE) {		\
srxbridge:								\
        if (idb->tbridge_on_hwidb)					\
 	    if (reg_invoke_tbridge_mci_forward(idb->fast_rcvidb))	\
	        goto rxexit;						\
        /*                                                              \
         * Get the type/length and sap (long) of this packet            \
         * for bridged packets only.  This is for                       \
         * hdlc_pakalign() to set up where pak->datagramstart           \
         * is supposed to point to.                                     \
         * We only need the type/length portion(not da and sa)          \
         * because we check for type or length.  If it is a             \
         * length, we need to check if it is snap encapsulated.         \
         *                                                              \
         *               byte1  byte2   byte3   byte4                   \
         *  SNIFFBASE+0  HDLC_BRIDGECODE(4 bytes)                       \
         *  SNIFFBASE+1  DA     DA      DA      DA                      \
         *  SNIFFBASE+2  DA     DA      SA      SA                      \
         *  SNIFFBASE+3  SA     SA      SA      SA                      \
         *  SNIFFBASE+4  type/len       (dsap)  (ssap)                  \
         *                                                              \
         *  mci_rxoffset + (HDLC_ENCAPBYTES/2) + (IEEEWORDS * 2)        \
         *  will point to sniff[SNIFFBASE+4].                           \
         */                                                             \
        regptr->argreg = idb->rxoffset + (HDLC_ENCAPBYTES/2) +          \
                        (IEEEWORDS * 2);                                \
        regptr->cmdreg = FCI_CMD_RX_SELECT;                             \
        idb->sniff[SNIFFBASE+4].d.lword = regptr->readlong;             \
     } else if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_PPP_LEX) {	\
        /*								\
         * Switch to the lex idb.  Check if we are routing the 		\
	 * protocol, if not try and bridge.				\
         */								\
	idbtype *lex_swidb = idb->fast_rcvidb->lex_swidb;		\
	if (lex_swidb == NULL) {					\
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	    goto rxexit;						\
	}								\
	/*								\
	 * lex_swidb->hwptr->rxtype is set in the lex_fs routine.	\
	 */								\
	if (BRIDGING_RXTYPE(lex_swidb->hwptr->fast_rcvidb,		\
			       lex_swidb->hwptr->rxtype)) {		\
	    if (lex_swidb->tbridge_on_swidb) {				\
		if (reg_invoke_tbridge_mci_forward(lex_swidb))		\
		    goto rxexit;					\
	    }								\
	}								\
    }									\
  srx_skip:								\
    /*									\
     * Check for fast DLSw and RSRB.					\
     */									\
    if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_DLSW) 		\
	if (reg_invoke_mci_dlsw_input(idb))				\
	    goto rxexit;						\
									\
    if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_RSRB)		\
	if (reg_invoke_mci_vring_input(idb))				\
            goto rxexit;						\
									\
    /*									\
     * Check to see if this is a udp broadcast.  If so, try to		\
     * take the fast path.						\
     * Note: udp_fast_flood() relies on the rx pointer to be		\
     * positioned at the ip checksum.					\
     */									\
    if (ROUTING_RXTYPE(idb->fast_rcvidb, RXTYPE_DODIP)) {	        \
	if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_IP10MB) {	\
	    idb->sniff[SNIFFBASE+1].d.sword[1] = regptr->readshort;	\
	    idb->sniff[SNIFFBASE+1].d.lword = regptr->readlong;		\
	    idb->sniff[SNIFFBASE+2].d.lword = regptr->readlong;		\
	    switch (idb->sniff[SNIFFBASE+2].d.byte[3]) {		\
	    case UDP_PROT:						\
		if (reg_invoke_udp_fast_flood(idb->fast_rcvidb))	\
		    goto rxexit;					\
		break;							\
	    case FST_DLSW_PROT:						\
                /*							\
                 * DLSw FST over Frame Relay handled by			\
		 * reg_invoke_fr_pvc_switch_or_fs()			\
     		 */							\
		if (idb->enctype != ET_FRAME_RELAY) {			\
		    if (reg_invoke_mci_dlsw_fst_forward(idb))		\
		        goto rxexit;					\
	        }							\
		break;							\
	    case FST_RSRB_PROT:						\
		if (reg_invoke_mci_rsrb_fst_forward(idb))		\
		    goto rxexit;					\
		break;							\
	    case GRE_PROT:						\
	    case IPINIP_PROT:						\
		if (reg_invoke_tunnel_ip_hes_fs(idb))	                \
		    goto rxexit;					\
		break;							\
	    }								\
	}								\
    }									\
                                                                        \
srxprocess:                                                             \
    if (summary == RXTYPE_UNKNOWN) {                           	        \
        if (reg_invoke_ether_sniff_encap(idb->type, cbus, idb)) {       \
            regptr->cmdreg = MCI_CMD_RX_FLUSH;                          \
            goto rxexit;                                                \
        } else if (reg_invoke_media_sniff_ipc(cbus, idb,                \
                                       idb->rxoffset+1)) {              \
            regptr->cmdreg = MCI_CMD_RX_FLUSH;                          \
            goto rxexit;                                                \
        }                                                               \
    }                                                                   \
									\
    /*									\
     * If we can't get a buffer, assume some sort of congestion problem \
     * and disable the interface for a very short while so the process  \
     * level code can run and recover.					\
     */									\
    pak = input_getbuffer(bytes, idb);					\
    if (!pak) {								\
	cbus_mci_idb_throttle(idb);					\
	goto rxexit;							\
    }									\
    /*									\
     * Do encapsulation classification, set up packet buffer.		\
     */									\
    iqueue = (*idb->parse_packet)(idb, pak, (uchar *)&idb->sniff);	\
    if (iqueue == NULL) {						\
	retbuffer(pak);							\
	cbus_mci_input_error(idb);					\
	regptr->cmdreg = FCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
    if (bytes > idb->max_pak_size) {					\
	idb->counters.giants++;						\
	retbuffer(pak);							\
	cbus_mci_input_error(idb);					\
	regptr->cmdreg = FCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
    pak->datagramsize = bytes;						\
    if (idb->mci_checked) {						\
	pak->flags |= PAK_MCICHECKED;					\
	idb->mci_checked = FALSE;					\
    }									\
    if (idb->ipmfast_switched) {				       	\
	pak->flags |= PAK_MFS_SWITCHED;  			        \
	idb->ipmfast_switched = FALSE;					\
    }	                                                                \
    /*									\
     * Set pointer to start of packet and read it into packet buffer.	\
     * Hand to raw input process and flush datagram in MCI RX buffer.	\
     */									\
    regptr->argreg = idb->rxoffset;					\
    regptr->cmdreg = FCI_CMD_RX_SELECT;					\
    mci2buffer(&regptr->readlong, pak->datagramstart, bytes);		\
    regptr->cmdreg = FCI_CMD_RX_FLUSH;					\
    idb->counters.rx_cumbytes += bytes;					\
    idb->counters.inputs++;						\
    (*iqueue) (idb, pak);						\
    goto rxexit;

/*
 * CBUS_MIP_RECEIVE
 * Receive a single datagram on a MIP interface.
 * This is a macro rather than a subroutine to eliminate the overhead
 * of a subroutine call.  Debug with an oscilliscope.
 *
 * The careful reader will no doubt see a close similarity to the HSSI
 * receive code.
 */
#define	CBUS_MIP_RECEIVE						\
                                                                        \
    /*									\
     * Very high speed switching of routed protocols			\
     */									\
                                                                        \
    /*									\
     *	If this is an IP packet, then we did a bad thing when we moved 	\
     *  the MEMD read pointer away from the destination address to	\
     *	sniff the VC.  We will put it back to 8 words (16 bytes) into	\
     *  the PDU, which is correct for IP.  Other protocols don't	\
     *  matter, as they reset it in any case.				\
     */									\
    regptr->argreg = MCI_OFFSETSIZE + MCI_IP_DSTADR_OFFSET;		\
    regptr->cmdreg = MCI_CMD_RX_SELECT;					\
                                                                        \
    summary = regptr->rxtype;						\
    switch (idb->enctype) {						\
    case ET_SMDS:							\
	/* We know we can't classify incoming smds packets - try to 	\
	 * fast switch them right away					\
	 */								\
	/* We know we can't classify incoming smds packets - try to 	\
	 * fast switch them right away					\
	 */								\
	regptr->argreg = idb->rxoffset +				\
	    btow(idb->smds_pdb->fast_input_modelen) + 2;		\
        regptr->cmdreg = MCI_CMD_RX_SELECT;				\
        PUTLONG(idb->smds_pdb->fast_smds_dest, regptr->readlong);	\
        PUTLONG(&idb->smds_pdb->fast_smds_dest[4], regptr->readlong);	\
	idb->fast_rcvidb =						\
	    reg_invoke_media_addr_to_swidb(ET_SMDS,			\
					   idb->smds_pdb->fast_smds_dest, \
					   idb);			\
	if (idb->fast_rcvidb == NULL) goto mipprocess;			\
        regptr->argreg = MCI_SERIAL_OFFSET + btow(idb->encsize) - 1;	\
        regptr->cmdreg = MCI_CMD_RX_SELECT;				\
	temp = regptr->readshort;					\
        switch (temp) {							\
	  case TYPE_CLNS:						\
            summary = RXTYPE_UNKNOWN;					\
	    goto mipprocess;						\
	  case TYPE_VINES:						\
	  case TYPE_VINES2:						\
            summary = RXTYPE_VINES;					\
	    break;							\
	  case TYPE_ETHERTALK:						\
            summary = RXTYPE_APPLE;					\
	    break;							\
	} 	 							\
        regptr->argreg = MCI_OFFSETSIZE + MCI_IP_DSTADR_OFFSET;		\
        regptr->cmdreg = MCI_CMD_RX_SELECT;				\
        if (reg_invoke_smds_fs(summary, idb))				\
            goto rxexit;						\
	bytes -= MIP_ENCAPSIZE_BYTES;					\
        pak = input_getbuffer(bytes,idb);				\
        if (!pak) goto mip_do_throttle;					\
	regptr->argreg = idb->rxoffset + MIP_ENCAPSIZE_WORDS;		\
        regptr->cmdreg = FCI_CMD_RX_SELECT;				\
        idb->sniff[SNIFFBASE+0].d.lword = regptr->readlong;		\
        iqueue = (*idb->parse_packet)(idb, pak, (uchar *)idb->sniff);	\
        pak->datagramstart += MIP_ENCAPSIZE_BYTES;			\
	pak->encsize -= MIP_ENCAPSIZE_BYTES;				\
	goto mip_copy_buffer;						\
	break;								\
    case ET_HDLC:							\
        if (reg_invoke_hdlc_fs(summary, idb))				\
            goto rxexit;						\
	break;								\
    case ET_PPP:							\
	if (summary == RXTYPE_UNKNOWN) {				\
	    regptr->argreg = MCI_OFFSETSIZE - 1;			\
	    regptr->cmdreg = MCI_CMD_RX_SELECT;				\
            summary = regptr->readshort;				\
	    summary = ppp_decode_rxtype_inline(summary);		\
	}								\
	if (    (summary != RXTYPE_UNKNOWN)				\
            && reg_invoke_ppp_fs(summary, idb))				\
	    goto rxexit;						\
        break;								\
    case ET_FRAME_RELAY:						\
	if (reg_invoke_fr_pvc_switch_or_fs(idb))			\
	    goto rxexit;						\
        /*								\
	 * sniff buffer already set up					\
	 */								\
	goto mip_bridge_skip;						\
	break;								\
    case ET_ATM_DXI:							\
	if (reg_invoke_atm_dxi_classify_pak_and_fs(idb))		\
	   goto rxexit;							\
	break;								\
    case ET_STUN:                                                       \
    case ET_SDLCP:                                                      \
    case ET_SDLCS:                                                      \
        /* CSCdi41558, without this some frames recognized as LEX */    \
        goto mipprocess;                                                \
        break;                                                          \
    default:								\
	break;								\
    }									\
									\
    /*									\
     * Sniff one long word to get protocol information			\
     */									\
    regptr->argreg = idb->rxoffset + MIP_ENCAPSIZE_WORDS; 		\
    regptr->cmdreg = FCI_CMD_RX_SELECT;					\
    idb->sniff[SNIFFBASE+0].d.lword = regptr->readlong;			\
									\
    /*									\
     * Handle possible bridging functions.				\
     */									\
    if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_PPP_BRIDGE) { 	\
       summary = RXTYPE_TRANSIT;					\
       /* read in ppp bridge encaps */					\
       idb->sniff[SNIFFBASE+1].d.sword[0] = regptr->readshort;		\
       if (idb->sniff[SNIFFBASE+1].d.sword[0] != PPP_FIZC_MACTYPE) {	\
	   goto mipprocess;						\
       }								\
       goto mipbridge;							\
    }									\
    if (idb->sniff[SNIFFBASE+0].d.lword == HDLC_BRIDGECODE) {		\
mipbridge:								\
        if (idb->tbridge_on_hwidb)			  		\
    	    if (reg_invoke_tbridge_mci_forward(idb->fast_rcvidb))	\
	        goto rxexit;						\
        /*                                                              \
         * Get the type/length and sap (long) of this packet            \
         * for bridged packets only.  This is for                       \
         * hdlc_pakalign() to set up where pak->datagramstart           \
         * is supposed to point to.                                     \
         * We only need the type/length portion(not da and sa)          \
         * because we check for type or length.  If it is a             \
         * length, we need to check if it is snap encapsulated.         \
         *                                                              \
         *               byte1  byte2   byte3   byte4                   \
         *  SNIFFBASE+0  HDLC_BRIDGECODE(4 bytes)                       \
         *  SNIFFBASE+1  DA     DA      DA      DA                      \
         *  SNIFFBASE+2  DA     DA      SA      SA                      \
         *  SNIFFBASE+3  SA     SA      SA      SA                      \
         *  SNIFFBASE+4  type/len       (dsap)  (ssap)                  \
         *                                                              \
         *  mci_rxoffset + MIP_ENCAPSIZE_WORDS + (HDLC_ENCAPBYTES/2) +  \
         *  (IEEEWORDS * 2) will point to sniff[SNIFFBASE+4].           \
         */                                                             \
        regptr->argreg = idb->rxoffset + MIP_ENCAPSIZE_WORDS +          \
                        (HDLC_ENCAPBYTES/2) + (IEEEWORDS * 2);          \
        regptr->cmdreg = FCI_CMD_RX_SELECT;                             \
        idb->sniff[SNIFFBASE+4].d.lword = regptr->readlong;             \
     } else if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_PPP_LEX) {	\
        /*								\
         * Switch to the lex idb.  Check if we are routing the 		\
	 * protocol, if not try and bridge.				\
         */								\
	idbtype *lex_swidb = idb->fast_rcvidb->lex_swidb;		\
	if (lex_swidb == NULL) {					\
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	    goto rxexit;						\
	}								\
	/*								\
	 * lex_swidb->hwptr->rxtype is set in the lex_fs routine.	\
	 */								\
	if (BRIDGING_RXTYPE(lex_swidb->hwptr->fast_rcvidb,		\
			       lex_swidb->hwptr->rxtype)) {		\
	    if (lex_swidb->tbridge_on_swidb) {				\
		if (reg_invoke_tbridge_mci_forward(lex_swidb))		\
		    goto rxexit;					\
	    }								\
	}								\
    }                                                                   \
mip_bridge_skip:							\
    /*									\
     * Check for fast RSRB.						\
     */									\
    if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_DLSW)                \
        if (reg_invoke_mci_dlsw_input(idb))                             \
            goto rxexit;                                                \
                                                                        \
    if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_RSRB)		\
	if (reg_invoke_mci_vring_input(idb))				\
            goto rxexit;						\
									\
    /*									\
     * Check to see if this is a udp broadcast.  If so, try to		\
     * take the fast path.						\
     * Note: udp_fast_flood() relies on the rx pointer to be		\
     * positioned at the ip checksum.					\
     */									\
    if (ROUTING_RXTYPE(idb->fast_rcvidb, RXTYPE_DODIP)) {		\
	if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_IP10MB) {	\
	    idb->sniff[SNIFFBASE+1].d.sword[1] = regptr->readshort;	\
	    idb->sniff[SNIFFBASE+1].d.lword = regptr->readlong;		\
	    idb->sniff[SNIFFBASE+2].d.lword = regptr->readlong;		\
	    switch (idb->sniff[SNIFFBASE+2].d.byte[3]) {		\
	    case UDP_PROT:						\
		if (reg_invoke_udp_fast_flood(idb->fast_rcvidb))	\
		    goto rxexit;					\
		break;							\
            case FST_DLSW_PROT:                                         \
                /*                                                      \
                 * DLSw FST over Frame Relay handled by                 \
                 * reg_invoke_fr_pvc_switch_or_fs()                     \
                 */                                                     \
                if (idb->enctype != ET_FRAME_RELAY) {                   \
                    if (reg_invoke_mci_dlsw_fst_forward(idb))           \
                        goto rxexit;                                    \
                }                                                       \
                break;                                                  \
	    case FST_RSRB_PROT:						\
		if (reg_invoke_mci_rsrb_fst_forward(idb))		\
		    goto rxexit;					\
		break;							\
	    case GRE_PROT:						\
	    case IPINIP_PROT:						\
		if (reg_invoke_tunnel_ip_hes_fs(idb))	                \
		    goto rxexit;					\
		break;							\
	    }								\
	}								\
    }									\
									\
mipprocess:                                                             \
    /*									\
     * If we can't get a buffer, assume some sort of congestion problem \
     * and disable the interface for a very short while so the process  \
     * level code can run and recover.					\
     */									\
    bytes -= MIP_ENCAPSIZE_BYTES;                                       \
    pak = input_getbuffer(bytes, idb);					\
    if (!pak) {								\
mip_do_throttle:                                                        \
	cbus_mci_idb_throttle(idb);					\
	goto rxexit;							\
    }									\
									\
    /*									\
     * Do encapsulation classification, set up packet buffer.		\
     */									\
    iqueue = (*idb->parse_packet)(idb, pak, (uchar *)idb->sniff);	\
mip_copy_buffer:							\
    if (iqueue == NULL) {						\
	retbuffer(pak);							\
	cbus_mci_input_error(idb);					\
	regptr->cmdreg = FCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
    if (bytes > idb->max_pak_size) {					\
	idb->counters.giants++;						\
	retbuffer(pak);							\
	cbus_mci_input_error(idb);					\
	regptr->cmdreg = FCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
    pak->datagramsize = bytes;						\
    if (idb->mci_checked) {						\
	pak->flags |= PAK_MCICHECKED;					\
	idb->mci_checked = FALSE;					\
    }									\
    if (idb->ipmfast_switched) {				       	\
	pak->flags |= PAK_MFS_SWITCHED;			                \
	idb->ipmfast_switched = FALSE;					\
    }	                                                                \
									\
    /*									\
     * Set pointer to start of packet and read it into packet buffer.	\
     * Hand to raw input process and flush datagram in MCI RX buffer.	\
     * We subtract off the MIP encapsulation bytes.			\
     */									\
    regptr->argreg = idb->rxoffset + MIP_ENCAPSIZE_WORDS; 		\
    regptr->cmdreg = FCI_CMD_RX_SELECT;					\
    mci2buffer(&regptr->readlong, pak->datagramstart, bytes);		\
    regptr->cmdreg = FCI_CMD_RX_FLUSH;					\
    idb->counters.rx_cumbytes += bytes;					\
    idb->counters.inputs++;						\
    (*iqueue) (idb, pak);						\
    goto rxexit;

/*
 * CBUS_FDDI_RECEIVE
 * Receive a single datagram on a FDDI interface.
 * This is a macro rather than a subroutine to eliminate the overhead
 * of a subroutine call.  Debug with an oscilloscope!
 */

#define CBUS_FDDI_RECEIVE	                                        \
                                                                        \
    /*									\
     * Check frame status						\
     */									\
    status = regptr->rxstatus;						\
    summary = regptr->rxtype;						\
    if (!(status & FDDI_SX_MASK)) {					\
									\
        /*								\
	 * Check frame size.						\
	 */								\
        if (bytes >= FDDI_MINDGRAM) {					\
                                                                        \
            /*                                                          \
	     * If bridging bit is set and station bit is not set,	\
	     * try to bridge it. Otherwise, very high speed switching 	\
	     * of routed protocols.           				\
	     * Adding the bridging check here is to prevent bridgeable 	\
	     * packets from getting into the fast-switch code and 	\
	     * thereby inadvertently routed since when CRB(Concurrent	\
	     * Routing and Bridging) is enabled, a specific protocol	\
	     * could be routed on some interfaces and bridged on other	\
	     * interfaces.						\
	     * In addition, check for IP multicasts, and fast switch	\
	     * them if possible. (CSCdi41429)				\
             */								\
       	    if ((((status &						\
		   ((FCI_BRIDGING_MASK | FCI_STATION_MASK |		\
                     FCI_HSRP_STATION_MASK) << 8))			\
		  != (FCI_BRIDGING_MASK << 8)) ||			\
		 ((status & (FCI_MULTICAST_MASK << 8)) &&		\
		 (RXTYPE_DODIP == regptr->rxtype))) &&			\
		reg_invoke_fddi_fs(summary,idb))			\
		goto rxexit;						\
	    else							\
		goto frxread;						\
        } else {							\
            /*								\
             * Datagram too small.					\
	     */								\
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	    if (status & FCI_STATION_MASK) {                            \
	        idb->counters.runts++;					\
	        idb->counters.input_error++;				\
	    }                                                           \
            goto rxexit;						\
	}								\
    } else {								\
									\
        /*								\
         * Error status, count em and drop the datagram.		\
         */								\
        if (status & FDDI_ERROR_THIS_NODE) {				\
	    idb->counters.input_crc++;					\
	    idb->counters.input_error++;				\
        }                                                               \
	regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
    /*									\
     * Can't get here.  Always a goto above.				\
     */									\
									\
frxread:								\
    /*									\
     * If bridging -- sniff out bridge code				\
     *									\
     * Note:  This code handles both FDDIs and FDDITs operating		\
     * in encapsulating or translational modes.  Which is denoted	\
     * by the flag FDDI_ENCAPSULATE in fddi_options which of course	\
     * we don't check here.  The reason for that is an encapsulating	\
     * interface is never put into promiscous mode and its address	\
     * filter is never set for the FCI_BRIDGING_MASK.  Also all		\
     * traffic that should be bridged will be sent directly to the	\
     * interface address (or the multicast) with the type code		\
     * RXTYPE_TRANSIT.							\
     */									\
	                                                                \
    /*									\
     * Determine whether we should bridge this datagram.		\
     * Attempt to bridge any packet with the bridge bit set		\
     * that does not also have the station bit set.			\
     * This attempt will succeed if the packet was forwarded,		\
     * flooded, or filtered by the transparent bridge code.		\
     * This attempt will fail for a variety of packets including	\
     * interesting multicasts (aliases will be flooded), and		\
     * physical broadcasts in protocols we are routing (other		\
     * physical broadcasts will be flooded).				\
     */									\
									\
    if ((status & ((FCI_BRIDGING_MASK | FCI_STATION_MASK) << 8)) ==	\
	(FCI_BRIDGING_MASK << 8)) {					\
        idb->fddi_cam_sa_miss =						\
	    regptr->rxlength & (FDDIT_CAM_VALID | FDDIT_SA_FND);	\
	if (!(srbinfo & FCI_RXSRB_PRESENT)) {                           \
	    if (reg_invoke_tbridge_mci_forward(idb->fast_rcvidb))	\
	    	goto rxexit;						\
	    else 							\
	    	goto frxread1;						\
       } else if (!(srbinfo & FCI_RXSRB_EXPLORER)) {                   	\
           /*                                                          	\
            * Source Route Bridge it.                                  	\
            */                                                         	\
             idb->rxsrbinfo = srbinfo;                                 	\
	     if ((*idb->tr_srb_fastswitch)(idb, NULL, idb->tr_bridge_idb)) {\
                 if (srb_debug) {					\
                     SrbDebug_FastForward(idb);				\
                 }							\
                 goto rxexit;                                           \
             } else                                                     \
                 goto frxread1;                                         \
       } else {								\
           if (reg_invoke_srt_flush_explorer(idb, bytes)) {		\
                     regptr->cmdreg = MCI_CMD_RX_FLUSH;                 \
                     goto rxexit;                                       \
           }								\
       }								\
    } else if (summary == RXTYPE_TRANSIT) {				\
	if (!idb->tbridge_on_hwidb || !idb->bridge_transit) {		\
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
            goto rxexit;						\
        }								\
	if (reg_invoke_tbridge_mci_forward(idb->fast_rcvidb))		\
	    goto rxexit;						\
	else {								\
	    if (!reg_invoke_tbridge_fddi_transit_receive(idb))		\
		/* Couldn't get pak buffer - need to throttle it */	\
		cbus_mci_idb_throttle(idb);				\
	    goto rxexit;						\
	}								\
    }									\
                                                                        \
    /*									\
     * Do not allow ourselves to be swamped with SMT frames.		\
     */									\
  frxread1:								\
    if (status & FDDI_SMT_FRAME) {					\
	/*								\
	 * We've heard from someone else on the ring.  If a FDDIT	\
	 * and this entry isn't in the input interface's CAM then	\
	 * insert it.							\
	 */								\
	if ((idb->type == IDBTYPE_FDDIT) &&				\
	    /*								\
	     * We do NOT check FDDI_BRIDGE_CAM_SUPPORTED here		\
	     * since these stations are unlikely to be sources		\
	     * or destinations of bridged traffic.			\
	     */								\
	    ((regptr->rxlength & (FDDIT_CAM_VALID | FDDIT_SA_FND))	\
		    == FDDIT_CAM_VALID)) {				\
	    regptr->argreg = FDDI_RECEIVE_OFFSET + 1 + IEEEBYTES/2;	\
	    regptr->cmdreg = FCI_CMD_RX_SELECT;				\
	    idb->sniff[SNIFFBASE+0].d.lword = regptr->readlong;		\
	    idb->sniff[SNIFFBASE+1].d.sword[0] = regptr->readshort;	\
	    regptr->argreg = idb->cbus_ctrlr_index;			\
	    regptr->cmdreg = FCI_CMD_CTRLR_SELECT;			\
	    regptr->argreg = idb->sniff[SNIFFBASE+0].d.sword[0];	\
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;			\
	    regptr->argreg = FCI_FDDI_CAM_WR_ARG;			\
	    regptr->cmdreg = FCI_CTRLR_CMD;				\
	    regptr->argreg = idb->sniff[SNIFFBASE+0].d.sword[1];	\
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;			\
	    regptr->argreg = FCI_FDDI_CAM_WR_ARG;			\
	    regptr->cmdreg = FCI_CTRLR_CMD;				\
	    regptr->argreg = idb->sniff[SNIFFBASE+1].d.sword[0];	\
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;			\
	    regptr->argreg = FCI_FDDI_CAM_WR_ARG;			\
	    regptr->cmdreg = FCI_CTRLR_CMD;				\
	    regptr->argreg = FDDI_CAM_INSERT;				\
	    regptr->cmdreg = FCI_CTRLR_ARGUMENT;			\
	    regptr->argreg = FCI_FDDI_CAM_CMD;				\
	    regptr->cmdreg = FCI_CTRLR_CMD;				\
	}								\
        if (!(smt_process_id) || (QUEUEFULL(&smtinputQ))) {		\
            regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
            idb->counters.input_drops++;                                \
            goto rxexit;						\
        }								\
    }									\
                                                                        \
    /*									\
     * Discard all MAC frames -- don't do anything with them.		\
     */									\
    if (status & FDDI_MAC_FRAME) {					\
        regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
                                                                        \
    /*									\
     * Sniff out some data to get protocol information.			\
     */									\
    regptr->argreg = MCI_OFFSETSIZE - 1 + idb->rxrifwordlen;		\
    regptr->cmdreg = FCI_CMD_RX_SELECT;					\
    idb->sniff[SNIFFBASE+0].d.lword = regptr->readlong;			\
    idb->sniff[SNIFFBASE+1].d.lword = regptr->readlong;			\
                                                                        \
    /*									\
     * Check for fast RSRB.						\
     */									\
    if (idb->sniff[SNIFFBASE+0].d.sword[0] == SNAPSNAP)			\
	if (idb->sniff[SNIFFBASE+1].d.sword[1] == TYPE_RSRB)		\
            if (reg_invoke_mci_vring_input(idb))			\
                goto rxexit;						\
									\
    if (ROUTING_RXTYPE(idb->fast_rcvidb, summary)) {			\
	/*								\
	 * Routing the protocol.  Check to see if IP and if so		\
	 * check for fast UDP flood.					\
	 * 								\
	 * Note: udp_fast_flood() relies on the rx pointer to be	\
	 * positioned at the ip checksum.				\
	 */								\
	if (summary == RXTYPE_DODIP) {					\
	    idb->sniff[SNIFFBASE+2].d.sword[1] = regptr->readshort;	\
	    /*                                                          \
             * Yes, this ordering is indeed correct. mci_rsrb_fst...    \
             * needs the data stored in SNIFFBASE+3 here, then will     \
             * rewrite it.                                              \
             */                                                         \
	    idb->sniff[SNIFFBASE+3].d.lword = regptr->readlong;		\
	    idb->sniff[SNIFFBASE+4].d.lword = regptr->readlong;		\
	    switch (idb->sniff[SNIFFBASE+4].d.byte[3]) {		\
	    case UDP_PROT:						\
		if (reg_invoke_udp_fast_flood(idb->fast_rcvidb))	\
		    goto rxexit;					\
		break;							\
            case FST_DLSW_PROT:                                         \
                if (reg_invoke_mci_dlsw_fst_forward(idb))               \
                    goto rxexit;                                        \
                break;                                                  \
	    case FST_RSRB_PROT:                                         \
		if (reg_invoke_mci_rsrb_fst_forward(idb))		\
		    goto rxexit;					\
		break;							\
	    case GRE_PROT:						\
	    case IPINIP_PROT:						\
		if (reg_invoke_tunnel_ip_hes_fs(idb))	                \
		    goto rxexit;					\
		break;							\
	    }								\
	}								\
    }									\
 									\
    /*                                                                  \
     * Read the datagram into memory                                    \
     */                                                                 \
    pak = input_getbuffer(bytes, idb);			        	\
    if (!pak) {								\
        cbus_mci_idb_throttle(idb);					\
	goto rxexit;							\
    }									\
    pak->datagramsize = bytes;						\
    (*idb->parse_packet)(idb, pak, &idb->sniff[SNIFFBASE+0].d.byte[0]);	\
    if (status & idb->multibits)					\
        idb->counters.input_broad++;					\
    if (idb->mci_checked) {						\
	pak->flags |= PAK_MCICHECKED;					\
	idb->mci_checked = FALSE;					\
    }									\
    if (idb->ipmfast_switched) {				       	\
	pak->flags |= PAK_MFS_SWITCHED; 			        \
	idb->ipmfast_switched = FALSE;					\
    }	                                                                \
    /*									\
     * Set pointer to start of packet and read it into packet buffer.	\
     * The arguments to mci2buffer need to be fudged because the cbus   \
     * can only be accessed in shortwords.  Hand to raw input process   \
     * and flush datagram in MCI RX buffer.				\
     */									\
    regptr->argreg = FDDI_RECEIVE_OFFSET;				\
    regptr->cmdreg = MCI_CMD_RX_SELECT;					\
    mci2buffer(&regptr->readlong, pak->datagramstart-1, bytes+1);	\
    pak->mci_status = status;						\
    idb->counters.rx_cumbytes += bytes;					\
    idb->counters.inputs++;						\
    if (pak->mci_status & FDDI_SMT_FRAMES) {				\
	uchar *p, *smtbuf;                                              \
        smtbuf = p_dequeue(&smtbufQ);                                   \
        if (smtbuf) {                                                   \
            /* note that the 3 extra FC bytes */                        \
            /* must be passed to FBM also. */                           \
            uchar eac_bits;                                             \
            p = pak->datagramstart - EXTRA_FC_BYTES;                    \
            if (idb->type != IDBTYPE_FDDI) {                            \
                ieee_swap((uchar *)(((MACHdrType *)p)->DA),             \
                          (uchar *)(((MACHdrType *)p)->DA));            \
                ieee_swap((uchar *)(((MACHdrType *)p)->SA),             \
                          (uchar *)(((MACHdrType *)p)->SA));            \
            }                                                           \
            bcopy(p, smtbuf, pak->datagramsize + EXTRA_FC_BYTES);       \
            eac_bits = (uchar)pak->mci_status;				\
            fddi_smt_frame(idb, smtbuf, pak->datagramsize, eac_bits);   \
            if ( fddi_smt_packet_debug )                                \
                fddi_smt_debug(pak,FALSE);				\
        } else {							\
            bprintf("noFBMbuf, ");					\
        }								\
        retbuffer(pak);							\
        pak = NULL;							\
    } else if ((status & 0xFF00) == (FCI_BRIDGING_MASK << 8) &&         \
             (srbinfo & FCI_RXSRB_PRESENT) && idb->rxrifwordlen) {      \
        pak->flags |= PAK_SRB_IN;					\
        pak->rxtype = idb->rxtype;					\
        reg_invoke_media_fddi_to_tr(pak, idb);				\
        reg_invoke_srt_srb_forward(pak, idb, idb->rxrifwordlen * 2);    \
    } else {								\
	if (srbinfo & FCI_RXSRB_PRESENT)				\
	    pak->flags |= PAK_SRB_IN;					\
	fddi = (fddi_hdr *)pak->datagramstart;				\
	llc1 = (sap_hdr *)pak->info_start;				\
        if ((pak->enctype == ET_LLC2) ||				\
            (srb_multiring_any_enabled(idb->firstsw))) { 		\
	    ieee_swap(fddi->saddr, bitswapped_addr);   			\
	    rif_update_llc(idb, 0, bitswapped_addr, NULL,		\
		       (srbroute_t *)(idb->rxrifwordlen ? fddi->data:NULL),\
		       llc1, RIF_AGED);					\
	}								\
	if (pak->enctype == ET_LLC2) {					\
	    fddi->saddr[0] &= ~FDDIT_RII;				\
            llc_input(pak);						\
	} else								\
            (*(idb->iqueue))(idb, pak);					\
    }									\
    /*									\
     * Re-select inbound interface, since rsrb flooding *may* have      \
     * selected some outbound interface, which would cause an RX_FLUSH  \
     * to credit the RQL of the wrong interface.                        \
     */									\
    regptr->argreg = idb->mci_index;                                    \
    regptr->cmdreg = MCI_CMD_SELECT;                                    \
    regptr->cmdreg = MCI_CMD_RX_FLUSH;                                  \
    goto rxexit;

/*
 * CBUS_TOKEN_RECEIVE
 * Receive a single datagram on a Token Ring interface.
 * This is a macro rather than a subroutine to eliminate the overhead
 * of a subroutine call.
 */

#define CBUS_TOKEN_RECEIVE						\
									\
    if ((srbinfo = regptr->rxsrbinfo) & FCI_RXSRB_PRESENT)		\
        idb->rxrifwordlen = (srbinfo & FCI_RXSRB_LENGTH);		\
	                                                                \
    /*									\
     * Determine whether we should bridge this datagram.		\
     * Attempt to bridge any packet with the bridge bit set		\
     * that does not also have the station bit set.			\
     * At the same time, check the low byte to insure that this is	\
     * not a MAC frame.							\
     * This attempt will succeed if the packet was forwarded,		\
     * flooded, or filtered by the transparent bridge code.		\
     * This attempt will fail for a variety of packets including	\
     * interesting multicasts (aliases will be flooded), and		\
     * physical broadcasts in protocols we are routing (other		\
     * physical broadcasts will be flooded).				\
     */									\
									\
    status = regptr->rxstatus;						\
									\
    /*									\
     * Catch bridging now if possible.  This check will manage to	\
     * ignore any MAC frames that may have come up with the bridging	\
     * bit.  (It is possible, but extremely rare.)			\
     */									\
    if (!(status & CTR_RXSTAT_MAC)) {					\
	if ((status & ((FCI_BRIDGING_MASK | FCI_STATION_MASK) << 8)) ==	\
	    (FCI_BRIDGING_MASK << 8)) {					\
	    if ((status & CTR_RXSTAT_BOTH) == CTR_RXSTAT_TB) {		\
		if (srbinfo & FCI_RXSRB_PRESENT) {			\
		    regptr->cmdreg = MCI_CMD_RX_FLUSH;			\
		    goto rxexit;					\
		}							\
		/*							\
		 * Transparently bridge it.				\
		 */							\
		if (reg_invoke_tbridge_mci_forward(idb->fast_rcvidb))	\
		    goto rxexit;					\
	    } else if ((status & CTR_RXSTAT_BOTH) == CTR_RXSTAT_SRB) {	\
	        if (!(srbinfo & FCI_RXSRB_EXPLORER)) {  		\
		    /*							\
		     * Source Route bridge it.				\
		     */							\
		    idb->rxsrbinfo = srbinfo;				\
		    if ((*idb->tr_srb_fastswitch)(idb, NULL, idb->tr_bridge_idb)) { \
                        if (srb_debug) {				\
                            SrbDebug_FastForward(idb);			\
                        }						\
		        goto rxexit;					\
		    }							\
	        } else {						\
		    /*							\
		     * Source Route Explorer with remote address.	\
		     * Check to see if packet needs to be flushed.	\
		     */							\
                    if (reg_invoke_srt_flush_explorer(idb, bytes)) {    \
                        regptr->cmdreg = MCI_CMD_RX_FLUSH;              \
                        goto rxexit;                                    \
	            }							\
	        }							\
            } else if ( ((status & CTR_RXSTAT_BOTH) == CTR_RXSTAT_MINE) && \
                        !(srbinfo & FCI_RXSRB_PRESENT))               { \
                /*                                                      \
                 * Attempt to transparently bridge it. SMF will         \
                 * determine if it is interesting or not. This          \
                 * could be a routing broadcast,functional              \
                 * address, or multicast that should                    \
                 * be bridged at interrupt level.                       \
                 */                                                     \
          /*                                                            \
           * Testing for CTR_RXSTAT_MINE is supposed to indicate        \
           * whether the interface recognized a multicast of some       \
           * sort, and comparing it to CTR_RXSTAT_BOTH is supposed      \
           * to guarentee that there is no rif present.  This is        \
           * only true if the protocol type is not being routed,        \
           * otherwise the interface won't set the CTR_RXSTAT_SRB       \
           * bit and SRB packets with multicast addresses will          \
           * pass this check.  However, "srbinfo" is still valid        \
           * so use it to check for packets with a rif.                 \
           */                                                           \
                if (reg_invoke_tbridge_mci_forward(idb->fast_rcvidb))   \
                    goto rxexit;                                        \
                else                                                    \
                    goto trxread;                                       \
            }                                                           \
        }								\
									\
	if (status & ((FCI_STATION_MASK | FCI_MULTICAST_MASK) << 8)) {	\
	    summary = regptr->rxtype;                                   \
	    if (reg_invoke_token_fs(summary, idb)) 			\
		goto rxexit;						\
	}								\
    }									\
									\
trxread:								\
									\
    /*									\
     * Sniff out some data to get protocol information.			\
     */									\
    regptr->argreg = CTR_RECEIVE_OFFSET + (TRING_ENCAPBYTES >> 1) +	\
	idb->rxrifwordlen;						\
    regptr->cmdreg = FCI_CMD_RX_SELECT;					\
    idb->sniff[SNIFFBASE+0].d.lword = regptr->readlong;			\
    idb->sniff[SNIFFBASE+1].d.lword = regptr->readlong;			\
									\
    /*									\
     * Check for fast RSRB.						\
     */									\
    if (idb->sniff[SNIFFBASE+0].d.sword[0] == SNAPSNAP)			\
	if (idb->sniff[SNIFFBASE+1].d.sword[1] == TYPE_RSRB)		\
	    if (reg_invoke_mci_vring_input(idb))			\
                goto rxexit;						\
									\
    if (ROUTING_RXTYPE(idb->fast_rcvidb, summary)) {			\
	/*								\
	 * Routing the protocol.  Check to see if IP and if so		\
	 * check for fast UDP flood.					\
	 *								\
	 * Note: udp_fast_flood() relies on the rx pointer to be	\
	 * positioned at the ip checksum.				\
	 */								\
	if (summary == RXTYPE_DODIP) {					\
	    idb->sniff[SNIFFBASE+2].d.sword[1] = regptr->readshort;	\
	    /*                                                          \
             * Yes, this ordering is indeed correct. mci_rsrb_fst...    \
             * needs the data stored in SNIFFBASE+3 here, then will     \
             * rewrite it.                                              \
             */                                                         \
 	     idb->sniff[SNIFFBASE+3].d.lword = regptr->readlong;	\
	     idb->sniff[SNIFFBASE+4].d.lword = regptr->readlong;	\
	    switch (idb->sniff[SNIFFBASE+4].d.byte[3]) {		\
	    case UDP_PROT:						\
		if (reg_invoke_udp_fast_flood(idb->fast_rcvidb))	\
		    goto rxexit;					\
		break;							\
            case FST_DLSW_PROT:                                         \
                if (reg_invoke_mci_dlsw_fst_forward(idb))               \
                    goto rxexit;                                        \
                break;                                                  \
	    case FST_RSRB_PROT:						\
		if (reg_invoke_mci_rsrb_fst_forward(idb))		\
		    goto rxexit;					\
		break;							\
	    case GRE_PROT:						\
	    case IPINIP_PROT:						\
		if (reg_invoke_tunnel_ip_hes_fs(idb))	                \
		    goto rxexit;					\
		break;							\
	    }								\
	}								\
    }									\
									\
    if (regptr->rxtype == RXTYPE_UNKNOWN) {                             \
        if (reg_invoke_media_sniff_ipc(cbus, idb,                       \
				       CTR_RECEIVE_OFFSET+10)) {        \
            regptr->cmdreg = MCI_CMD_RX_FLUSH;                          \
	    goto rxexit;                                                \
	}                                                               \
        if (reg_invoke_ether_sniff_encap(idb->type, cbus, idb)) { 	\
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;		                \
	    goto rxexit;		                                \
	}                                                               \
    }                                                                   \
                                                                        \
    /*									\
     * End of fast switching code.					\
     */									\
									\
    /*									\
     * Make sure that the pointer is left pointing to the first byte of	\
     * the LLC2 header in the frame.					\
     */									\
    regptr->argreg = CTR_RECEIVE_OFFSET + (TRING_ENCAPBYTES >> 1) +	\
	idb->rxrifwordlen;						\
    regptr->cmdreg = FCI_CMD_RX_SELECT;					\
									\
    /*									\
     * Do MAC frames here.						\
     */									\
    if (status & CTR_RXSTAT_MAC) {					\
	if (!(status & CTR_RXSTAT_MINE) || 				\
	    !(idb->tr_ring_mode & RINGMODE_NEED_MAC)) {			\
	    /*								\
	     * A promiscuous MAC packet, or there is no one wants	\
	     * to listen to a MAC frame on this interface.		\
	     */								\
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	    goto rxexit;						\
	}								\
	if (!(idb->tr_ring_mode & RINGMODE_RS)) {			\
	    /*								\
	     * Ignore AMP and SMP frames most of the time.		\
	     */								\
	    if ((idb->sniff[SNIFFBASE+0].d.byte[3] == MVID_AMP) || 	\
		(idb->sniff[SNIFFBASE+0].d.byte[3] == MVID_SMP)) {	\
	        regptr->cmdreg = MCI_CMD_RX_FLUSH;			\
	        goto rxexit;						\
	    }								\
	}								\
									\
	/*								\
	 * Get a packet for reading the datagram into memory		\
	 */								\
	pak = input_getbuffer(bytes, idb);				\
	if (!pak) {							\
	    cbus_mci_idb_throttle(idb);					\
	    goto rxexit;						\
	}								\
	pak->datagramstart = pak->network_start -		        \
	    (idb->rxrifwordlen * 2) - TRING_ENCAPBYTES;			\
	pak->datagramsize = bytes;					\
	if (status & idb->multibits)					\
	    idb->counters.input_broad++;				\
									\
	regptr->argreg = CTR_RECEIVE_OFFSET;				\
	regptr->cmdreg = MCI_CMD_RX_SELECT;				\
	mci2buffer(&regptr->readlong, pak->datagramstart, bytes);	\
	regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	pak->mci_status = status;					\
	pak->enctype = ET_MAC;						\
	if (tokenring_debug) {						\
	    trh = (tring_hdr *) pak->datagramstart;			\
	    llc1 = (sap_hdr *) (&trh->data[idb->rxrifwordlen*2]);	\
	    tr_print_packet(idb, "mac:", trh, (ulong *)llc1, status,	\
	        pak->datagramsize);					\
	}								\
	idb->counters.rx_cumbytes += bytes;				\
	idb->counters.inputs++;						\
	(*(idb->iqueue))(idb, pak);					\
	goto rxexit;							\
    }									\
									\
    /*									\
     * Bridge SRB packets the slow way, ie, copy packet into a system	\
     * memory buffer and forward.					\
     */									\
    if ((status & CTR_RXSTAT_BOTH) == CTR_RXSTAT_SRB) {	                \
	ctr_srb_frame(idb, regptr, srbinfo, bytes);			\
	idb->counters.rx_cumbytes += bytes;				\
	idb->counters.inputs++;						\
	goto rxexit;							\
    }									\
									\
    /*									\
     * Bridge selected Multicast Explorers as common SRB packets.  	\
     * These include illegal linktypes that are bridged by default.	\
     * This is to prevent storms of these packets from filling up the	\
     * process level queues and causing interface throttling.		\
     * See multicast_explorer_can_be_bridged().				\
     */									\
    if (CBUS_MULTICAST_SRB_EXPLORER(status, srbinfo, idb->fast_rcvidb,	\
		   idb->rxtype, (sap_hdr *)idb->sniff)) {	        \
        if (reg_invoke_srt_flush_explorer(idb, bytes)) {    		\
            regptr->cmdreg = MCI_CMD_RX_FLUSH;              		\
            goto rxexit;                                    		\
	}								\
	ctr_srb_frame(idb, regptr, srbinfo, bytes);                     \
	idb->counters.rx_cumbytes += bytes;                             \
	idb->counters.inputs++;                                         \
	goto rxexit;                                                    \
    }                                                                   \
									\
    /*									\
     * Get a packet for reading the datagram into memory		\
     */									\
    pak = input_getbuffer(bytes, idb);					\
    if (!pak) {								\
        cbus_mci_idb_throttle(idb);					\
	goto rxexit;							\
    }									\
									\
    /*									\
     * Do encapsulation classification, set up packet buffer.		\
     */									\
    idb->sniff[SNIFFBASE+0].d.lword = regptr->readlong;			\
    idb->sniff[SNIFFBASE+1].d.lword = regptr->readlong;			\
    pak->datagramsize = bytes;						\
    (*idb->parse_packet)(idb, pak, &idb->sniff[SNIFFBASE+0].d.byte[0]);	\
									\
    /*									\
     * Set pointer to start of packet and read it into packet buffer.	\
     * Hand to raw input process and flush datagram in MCI RX buffer.	\
     */									\
    regptr->argreg = CTR_RECEIVE_OFFSET;				\
    regptr->cmdreg = MCI_CMD_RX_SELECT;					\
    mci2buffer(&regptr->readlong, pak->datagramstart, bytes);		\
    regptr->cmdreg = MCI_CMD_RX_FLUSH;					\
    pak->mci_status = status;						\
    idb->counters.rx_cumbytes += bytes;					\
    idb->counters.inputs++;						\
    if (status & idb->multibits)					\
        idb->counters.input_broad++;					\
									\
    trh = (tring_hdr *) pak->datagramstart;				\
    llc1 = (sap_hdr *) pak->info_start;					\
									\
    /*                                                                  \
     * CSCdi16607                                                       \
     * Novell, NetBIOS Kludge: We want to bridge novell-ether and       \
     *   NetBIOS broadcast over token.                                  \
     *   Please reference "token_get_enctype_infosize()", there we      \
     *   classify lpak->enctype for them.                               \
     *                                                                  \
     * ET_NOVELL_ETHER and ET_UNSUPPORTED are not supported -           \
     *   Those two kinds of packets will be classified as               \
     *   LINK_ILLEGAL as linktype, and will be bridged.                 \
     */                                                                 \
     if (((idb->sniff[SNIFFBASE+0].d.byte[2] & ~LLC1_P) != LLC1_UI) &&  \
         (pak->enctype != ET_NOVELL_ETHER) &&                           \
         (pak->enctype != ET_UNSUPPORTED)) {                            \
	/*								\
	 * Is it anything other than an LLC UI				\
	 * frame.  This routine handles XID and TEST packets, as well as \
	 * all LLC Type 2 frames.					\
	 */								\
	rif_update_llc(idb, 0, trh->saddr, NULL,			\
		   (srbroute_t *)(idb->rxrifwordlen ? trh->data : NULL),  \
		   llc1, RIF_AGED);					\
	switch (llc1->control & ~LLC1_P) {				\
	  case LLC1_XID:						\
	    tr_process_xid(idb, pak, FALSE, FALSE);			\
		break;							\
	  case LLC1_TEST:						\
	    tr_process_test(idb, pak, FALSE);				\
		break;							\
	  default:							\
	    reg_invoke_llc2_input(pak);					\
	    goto rxexit;						\
	}								\
    }									\
    /*                                                                  \
     *  It's a LLC packet.  If it is a broadcast from us, drop it.      \
     */                                                                 \
    if ( (trh->daddr[0] & TR_GROUP) &&                                  \
           ieee_equal_sans_rii( trh->saddr, idb->hardware) ) {          \
         retbuffer(pak);                                                \
         goto rxexit;                                                   \
    }                                                                   \
									\
    if (idb->mci_checked) {						\
	pak->flags |= PAK_MCICHECKED;					\
	idb->mci_checked = FALSE;					\
    }									\
    if (idb->ipmfast_switched) {				       	\
	pak->flags |= PAK_MFS_SWITCHED; 			        \
	idb->ipmfast_switched = FALSE;					\
    }	                                                                \
    if (status & CTR_RXSTAT_SRB)					\
        pak->flags |= PAK_SRB_IN;					\
									\
    if (srb_multiring_any_enabled(idb->firstsw)) {			\
	rif_update(idb, 0, trh->saddr, NULL,				\
		   (srbroute_t *)(idb->rxrifwordlen ? trh->data : NULL),\
		   RIF_AGED);						\
    }									\
    if (tokenring_debug) {						\
	tr_print_packet(idb, " in:", trh, (ulong *)llc1, status,	\
	    pak->datagramsize);						\
    }									\
    (*(idb->iqueue))(idb, pak);						\
    goto rxexit;

/*
 * CBUS_AIP_RECEIVE
 * Receive a single datagram on a ds3 interface.
 * This is a macro rather than a subroutine to eliminate the overhead
 * of a subroutine call.  Debug with an oscilloscope!
 */

#define CBUS_AIP_RECEIVE						\
    charlong sniffcode;							\
    LANE_FAST_ACTION lane_result = LANE_NORMAL;                         \
    atm_db_t *aip = idb->atm_db;					\
    vc_info_t *vc = NULL;						\
    idbtype *swidb;                                             	\
                                                                        \
    /*                                                                  \
     * Sniff & save word at current read pointer in case this is        \
     * IPgram (saved word will be IP destination...see IP_FAST_STARTUP  \
     * in ipfast_mci.c).                                                \
     */                                                                 \
                                                                        \
    idb->sniff[SNIFFBASE+1].d.lword = regptr->readlong;                 \
                                                                        \
    /*									\
     * Sniff one long word to get vc information			\
     */									\
    regptr->argreg = idb->rxoffset;					\
    regptr->cmdreg = FCI_CMD_RX_SELECT;					\
    sniffcode.d.lword = regptr->readlong;				\
    idb->sniff[SNIFFBASE+0].d.lword = sniffcode.d.lword;		\
    vc = atm_getvc_inline(aip, sniffcode.d.sword[0]);			\
    aip->fast_vc_info = vc;                                             \
    if (!vc) {								\
      goto arxread;	/* punt since we can't find the vc */		\
    }   							        \
    idb->fast_rcvidb = vc->swidb;                                       \
    /*									\
     * Make sure that the received datagram doesn't exceed sub-if mtu   \
     */									\
    if (bytes > (vc->swidb->sub_mtu + ETHER_ARPA_ENCAPBYTES		\
		 + ATM_SMDS_HDRBYTES)) {              			\
	idb->counters.giants++;						\
	cbus_mci_input_error(idb);					\
	regptr->cmdreg = FCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
    summary = regptr->rxtype;                                           \
    /*                                                                  \
     * If this is LAN Emulation packet, call LANE code to see what      \
     * we should do with it.                                            \
     */                                                                 \
    if ((vc->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL5LANE) {            \
        lane_result = lane_aip_fast(idb, vc->swidb);		        \
        if (lane_result != LANE_NORMAL) {                               \
            if (lane_result == LANE_ALREADY_DONE)                       \
                goto rxexit;                                            \
            if (lane_result == LANE_DO_NOT_ROUTE)                       \
                goto no_route;                                          \
            if (lane_result == LANE_TO_PROCESS)                         \
                goto arxread;                                           \
         }                                                              \
    }                                                                   \
    /*									\
     * Very high speed switching of routed protocols			\
     */									\
    if (reg_invoke_aip_fs(summary, idb))				\
        goto rxexit;							\
									\
    /*									\
     * Check to see if this is a udp broadcast.  If so, try to		\
     * take the fast path.						\
     * Also check to see if this is an RSRB/FST frame.			\
     * Note: udp_fast_flood() relies on the rx pointer to be		\
     * positioned at the ip checksum.					\
     */									\
    if (ROUTING_RXTYPE(idb->fast_rcvidb, RXTYPE_DODIP)) {		\
	if (summary == RXTYPE_DODIP) {					\
            regptr->argreg = idb->rxoffset + btow(vc->encapsize);  	\
            regptr->cmdreg = FCI_CMD_RX_SELECT;                         \
	    idb->sniff[SNIFFBASE+1].d.sword[1] = regptr->readshort;	\
	    /*                                                          \
             * Yes, this ordering is indeed correct. mci_rsrb_fst...    \
             * needs the data stored in SNIFFBASE+3 here, then will     \
             * rewrite it.                                              \
             */                                                         \
	    idb->sniff[SNIFFBASE+3].d.lword = regptr->readlong;		\
	    idb->sniff[SNIFFBASE+4].d.lword = regptr->readlong;		\
	    switch (idb->sniff[SNIFFBASE+4].d.byte[3]) {		\
	    case UDP_PROT:						\
		if (reg_invoke_udp_fast_flood(idb->fast_rcvidb))	\
		    goto rxexit;					\
		break;							\
            case FST_DLSW_PROT:                                         \
		idb->rsrb_vringoffset = idb->rxoffset +			\
					    btow(vc->encapsize);        \
                if (reg_invoke_mci_dlsw_fst_forward(idb))               \
                    goto rxexit;                                        \
                break;                                                  \
	    case FST_RSRB_PROT:						\
		idb->rsrb_vringoffset = idb->rxoffset +			\
					    btow(vc->encapsize);        \
		if (reg_invoke_mci_rsrb_fst_forward(idb))		\
		    goto rxexit;					\
		break;							\
	    case GRE_PROT:						\
	    case IPINIP_PROT:						\
		if (reg_invoke_tunnel_ip_hes_fs(idb))	                \
		    goto rxexit;					\
		break;							\
	    }								\
	}								\
    }									\
									\
  no_route:                                                             \
    if (vc->etype == ATM_ENCTYPE_SNAP) {				\
        regptr->argreg = idb->rxoffset + (ATM_ENCAP_HDRBYTES/2);	\
        regptr->cmdreg = FCI_CMD_RX_SELECT;				\
        idb->sniff[SNIFFBASE+1].d.lword = regptr->readlong;             \
        idb->sniff[SNIFFBASE+2].d.lword = regptr->readlong;             \
    /*									\
     * Handle possible bridging functions.				\
     */									\
        if (idb->sniff[SNIFFBASE+2].d.sword[0] == TBRIDGE_OUI_LAST_SHORT && \
            idb->sniff[SNIFFBASE+1].d.byte[3] == TBRIDGE_OUI_1ST_BYTE) { \
	    swidb = vc->swidb;                                          \
            if (swidb->tbridge_on_swidb) {				\
    	        if (reg_invoke_tbridge_mci_forward(swidb))   		\
	        goto rxexit;						\
	    }								\
         }                                                      	\
     }									\
									\
    /*									\
     * Check for DLSw Direct.						\
     */									\
    if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_DLSW)		\
	if (reg_invoke_mci_dlsw_input(idb))				\
            goto rxexit;						\
									\
    /*									\
     * Check for fast RSRB.						\
     */									\
    if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_RSRB)		\
	if (reg_invoke_mci_vring_input(idb))				\
            goto rxexit;						\
									\
    /*									\
     * if bridging is configured on the interface, try to fast switch.  \
     * if that fails, try to bridge at  process level by falling thru'  \
     */                                                                 \
    if (lane_result == LANE_DO_NOT_ROUTE) {				\
	swidb = vc->swidb;                                              \
        if (swidb->tbridge_on_swidb) {	               			\
            if (reg_invoke_tbridge_mci_forward(swidb)) 		  	\
	        goto rxexit;  						\
            else { /* Process Level Bridging can be attempted */        \
              goto arxread;                                             \
            }                                                           \
        }      							        \
        else { 								\
	  regptr->cmdreg = FCI_CMD_RX_FLUSH;				\
	  goto rxexit;							\
        } 								\
    }								        \
arxread:								\
									\
    /*									\
     * If we can't get a buffer, assume some sort of congestion problem \
     * and disable the interface for a very short while so the process  \
     * level code can run and recover.					\
     */									\
    pak = input_getbuffer(bytes, idb);					\
    if (!pak) {								\
	cbus_mci_idb_throttle(idb);					\
	goto rxexit;							\
    }									\
									\
    /*									\
     * Do encapsulation classification, set up packet buffer.		\
     */									\
    pak->datagramsize = bytes;						\
    pak->rxtype = summary;						\
    iqueue = (*idb->parse_packet)(idb, pak, (uchar *)&sniffcode);	\
    if (iqueue == NULL) {						\
	retbuffer(pak);							\
	cbus_mci_input_error(idb);					\
	regptr->cmdreg = FCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
    if (bytes > idb->max_pak_size) {					\
	idb->counters.giants++;						\
	retbuffer(pak);							\
	cbus_mci_input_error(idb);					\
	regptr->cmdreg = FCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
    pak->atm_vc = vc;							\
    pak->atm_vcid = sniffcode.d.sword[0];				\
    if (idb->mci_checked) {						\
	pak->flags |= PAK_MCICHECKED;					\
	idb->mci_checked = FALSE;					\
    }									\
    if (idb->ipmfast_switched) {				       	\
	pak->flags |= PAK_MFS_SWITCHED; 			        \
	idb->ipmfast_switched = FALSE;					\
    }	                                                                \
									\
    /*									\
     * Set pointer to start of packet and read it into packet buffer.	\
     * Hand to raw input process and flush datagram in MCI RX buffer.	\
     */									\
    regptr->argreg = idb->rxoffset;					\
    regptr->cmdreg = FCI_CMD_RX_SELECT;					\
    mci2buffer(&regptr->readlong, pak->datagramstart, bytes);		\
    regptr->cmdreg = FCI_CMD_RX_FLUSH;					\
    idb->counters.rx_cumbytes += bytes;					\
    idb->counters.inputs++;						\
    (*iqueue) (idb, pak);						\
    goto rxexit;


/*
 * channel_sniff_typecode
 * 
 *
 */

static inline ushort channel_sniff_typecode (mcitype *cbus)
{
    ushort result;
    mciregtype *regptr = cbus->regptr;

    regptr->argreg = CHANNEL_RECEIVE_OFFSET+1;
    regptr->cmdreg = MCI_CMD_RX_SELECT;
    result = regptr->readshort;
    return(result);
} 

static inline ushort channel_sniff_vc (mcitype *cbus)
{
    ushort result;
    mciregtype *regptr = cbus->regptr;

    regptr->argreg = CHANNEL_RECEIVE_OFFSET;
    regptr->cmdreg = MCI_CMD_RX_SELECT;
    result = regptr->readshort;
    return(result);
}

static inline void channel_sniff_rif (mcitype *cbus, hwidbtype* hwidb,
				      ushort offset)
{
    ushort temp, bits, len;
    mciregtype *regptr = cbus->regptr;

    regptr->argreg = offset;
    regptr->cmdreg = MCI_CMD_RX_SELECT;
    temp = regptr->readshort;
    bits = FCI_RXSRB_PRESENT|FCI_RXSRB_MATCHED;
    len = hwidb->rxsrbinfo = hwidb->rxrifwordlen =
	btow((temp>>8) & RC_LTHMASK);
    
    if (temp & RC_D) {		/* direction bit set */
	bits |= FCI_RXSRB_DIR;
	bits |= (len-2)<<8;	/* RD offset - end */
    } else {			/* no direction bit set */
	bits |= 1<<8;		/* RD offset - beginning */
    }
    if (temp & ((RC_BROADBIT|RC_SPANBIT|RC_TRACE_ORIG)<<8))
	bits |= FCI_RXSRB_EXPLORER;
    hwidb->rxsrbinfo |= bits;
}

/*
 * Throttle input buffer usage using the CIP virtual interface.
 * This is better than adding overhead for every interface.
 */
static inline paktype *cip_virtual_input_getbuffer (ulong bytes, hwidbtype *idb)
{
    int input_qcount = 0;
    hwidbtype *tmp_idb = idb->output_if_hw;
    int input_defqcount = tmp_idb->input_defqcount;

    for (; tmp_idb; tmp_idb = tmp_idb->next_idb_on_physint) {
        input_qcount += tmp_idb->input_qcount;
    }
    if (input_qcount > input_defqcount) {
	idb->output_if_hw->counters.input_drops++;
	return(NULL);
    }
    idb->input_defqcount = input_defqcount;
    return input_getbuffer(bytes, idb);
}


#ifdef C7000

/*
 * Receive and enqueue a data buffer
 *   -these are data buffers coming from an IP that are
 *    not part of the interface traffic, for example
 *    love letters, configuration acknowledgements, and
 *    IPC messages.  These packets should not be reflected
 *    in the interface accounting values.
 */
static void receive_encaps_pak (
    hwidbtype *hwidb,
    ushort link_type,
    ushort bytes)
{
    mciregtype *regptr = hwidb->mci_regptr;
    paktype    *pak;
    
    pak = getbuffer(bytes);
    if (pak == NULL) {
        return;
    }
    mci2buffer(&regptr->readlong, pak->datagramstart, bytes);
    
    pak->linktype = link_type;
    pak->datagramsize = bytes;
    pak->if_input = hwidb->firstsw;
    reg_invoke_raw_enqueue(link_type, pak);
}

/*
 * channel_encaps_known
 */
static boolean channel_encaps_known (
   hwidbtype  *hwidb,
   ushort      encaps,
   ushort      bytes)
{
    ushort link_type;
    boolean debuggery;
    cip_idb_infotype *cip;

    bytes -= CHANNEL_ENCAPBYTES;
    
    /* Return FALSE if we don't understand this encapsulation */
    switch (encaps) {
    case CHANNEL_ENCAPS_LOVE:
	debuggery = channel_debug_love && channel_debug_encaps;
	link_type = LINK_LOVE;
	break;
    case CHANNEL_ENCAPS_CFGACK:
	cip = channel_sb_get_cip_info(hwidb);
	if (cip && cip->cip_encaps)
	    (*cip->cip_encaps)(hwidb, NULL, bytes);
	return TRUE;
	break;
    case CHANNEL_ENCAPS_IPC:
	debuggery = channel_debug_encaps;
	link_type = LINK_IPC;
	break;
    default:
	debuggery = TRUE;
	link_type = LINK_MAXLINKTYPE;
	break;
    }
    
    if (debuggery)
        buginf("\n%s: Encaps %04X message received, bytes %d",
	       hwidb->hw_namestring,
	       encaps,
               bytes);
    
    if (link_type == LINK_MAXLINKTYPE) {
	if (debuggery)
	    buginf("-- Unknown encapsulation -");
	return FALSE;
    }
    receive_encaps_pak(hwidb, link_type, bytes);
    return TRUE;
}
#endif

/*
 * CBUS_CHANNEL_RECEIVE
 * Receive a single datagram on a channel interface.
 * This is a macro rather than a subroutine to eliminate the overhead
 * of a subroutine call.  Any changes must be analyzed with a logic 
 * analyzer to ensure that performance isn't degraded.
 */

#define CBUS_CHANNEL_RECEIVE                                             \
    status = regptr->rxstatus;                                           \
    summary = regptr->rxtype;                                            \
    if (reg_invoke_channel_fs(summary, idb))                             \
        goto rxexit;                                                     \
    else if (summary == RXTYPE_UNKNOWN) {                                \
	summary = channel_sniff_typecode(cbus);                          \
	if (summary == CHANNEL_ENCAPS_VLAN) {                            \
            summary = regptr->readshort; /* sniff media code */          \
        } else {                                                         \
            /* Process known encapsulations; discard unknowns */         \
            channel_encaps_known(idb, summary, bytes);                   \
            regptr->cmdreg = MCI_CMD_RX_FLUSH;                           \
            goto rxexit;		                                 \
	}                                                                \
    } else                                                               \
        goto crxread; /* something else, try process level */            \
                                                                         \
    chan_sb = channel_get_sb(idb);                                       \
    if (!chan_sb) {                                                      \
        regptr->cmdreg = MCI_CMD_RX_FLUSH;                               \
        goto rxexit;                                                     \
    }                                                                    \
    idb = chan_sb->get_hwidb_from_vc(idb, channel_sniff_vc(cbus));       \
    if (!idb) {                                                          \
        regptr->cmdreg = MCI_CMD_RX_FLUSH;                               \
        goto rxexit;                                                     \
    }                                                                    \
    idb->rxlength = bytes;	/* save inbound length */                \
    idb->rxtype = RXTYPE_UNKNOWN; /* no classification done by ucode */  \
    if (summary == 1) { /* token ring, do source-route bridging */       \
        channel_sniff_rif(cbus, idb, CHANNEL_TOKEN_RIF_OFFSET);          \
        if (!(idb->rxsrbinfo & FCI_RXSRB_EXPLORER)) {                    \
            if ((*idb->tr_srb_fastswitch)(idb, NULL,                     \
                                          idb->tr_bridge_idb)) {         \
                if (srb_debug) {					 \
                    SrbDebug_FastForward(idb);				 \
                }							 \
                goto rxexit; 			                         \
	    }                                                            \
        } else {                                                         \
            /* maybe it was an explorer */                               \
            if (reg_invoke_srt_flush_explorer(idb, bytes)) {             \
                regptr->cmdreg = MCI_CMD_RX_FLUSH;                       \
                goto rxexit;                                             \
            }                                                            \
        }                                                                \
        goto crxread;                                                    \
    } else if (summary == 2) { /* fddi, do source-route bridging */      \
        channel_sniff_rif(cbus, idb, CHANNEL_TOKEN_RIF_OFFSET);          \
        if (!(idb->rxsrbinfo & FCI_RXSRB_EXPLORER)) {                    \
	    if ((*idb->tr_srb_fastswitch)(idb, NULL,                     \
                                          idb->tr_bridge_idb)) {         \
                if (srb_debug) {					 \
                    SrbDebug_FastForward(idb);				 \
                }							 \
                goto rxexit; 			                         \
	    }                                                            \
	} else {                                                         \
            /* maybe it was an explorer */                               \
            if (reg_invoke_srt_flush_explorer(idb, bytes)) {             \
                regptr->cmdreg = MCI_CMD_RX_FLUSH;                       \
                goto rxexit;                                             \
	    }                                                            \
        }                                                                \
        goto crxread;                                                    \
    } else if (summary == 0) { /* ethernet, do transparent bridging */   \
        if (reg_invoke_tbridge_mci_forward(idb->fast_rcvidb)) {          \
            goto rxexit;                                                 \
        } else                                                           \
            goto crxread; /* probably ought to flush */                  \
    } else {                                                             \
	/* something terribly wrong - don't recognize media type */      \
	errmsg(&msgsym(UNKENC, CBUS), idb->hw_namestring, summary);      \
	regptr->cmdreg = FCI_CMD_RX_FLUSH;                               \
        goto rxexit;                                                     \
    }                                                                    \
                                                                         \
                                                                         \
crxread:                                                                 \
                                                                         \
    /*                                                                   \
     * We are going to receive this datagram.                            \
     * Get a packet buffer and set it up.                                \
     * If we can't get a buffer, assume some sort of congestion          \
     * problem and disable the interface until the process level         \
     * code can run and recover.                                         \
     */                                                                  \
    if (is_cip_lan(idb) || is_cip_virtual(idb))                          \
        pak = cip_virtual_input_getbuffer(bytes, idb);                   \
    else                                                                 \
        pak = input_getbuffer(bytes, idb);                    \
    if (!pak) {                                                          \
        cbus_mci_idb_throttle(idb);                                      \
        goto rxexit;                                                     \
    }                                                                    \
    pak->datagramsize = bytes;                                           \
    (*idb->parse_packet)(idb, pak, &idb->sniff[SNIFFBASE+0].d.byte[0]);  \
    if (status & idb->multibits)                                         \
        idb->counters.input_broad++; /* CHANNELTODO - is this ever useful??? */   \
    if (idb->mci_checked) {                                              \
        pak->flags |= PAK_MCICHECKED;                                    \
        idb->mci_checked = FALSE;                                        \
    }                                                                    \
    if (idb->ipmfast_switched) {			       		 \
	pak->flags |= PAK_MFS_SWITCHED;                                  \
	idb->ipmfast_switched = FALSE;					 \
    }	                                                                 \
    /*                                                                   \
     * Set pointer to start of packet and read it into packet buffer.    \
     * Hand to raw input process and flush datagram in RX buffer.        \
     */                                                                  \
    regptr->argreg = CHANNEL_RECEIVE_OFFSET;                             \
    regptr->cmdreg = MCI_CMD_RX_SELECT;                                  \
    mci2buffer(&regptr->readlong, pak->datagramstart, bytes);            \
    idb->counters.rx_cumbytes += bytes;                                  \
    idb->counters.inputs++;                                              \
    pak->mci_status = status;                                            \
    if ((is_cip_lan(idb)) &&                                             \
        (idb->rxsrbinfo & FCI_RXSRB_PRESENT) && idb->rxrifwordlen) {     \
        pak->flags |= PAK_SRB_IN;				         \
        pak->rxtype = idb->rxtype;					 \
        pak->datagramsize -= CHANNEL_TOKEN_ENCAPBYTES;                   \
        pak->datagramstart += CHANNEL_TOKEN_ENCAPBYTES;                  \
        reg_invoke_srt_srb_forward(pak, idb, idb->rxrifwordlen * 2);     \
    } else {                                                             \
        (*(idb->iqueue))(idb, pak);                                      \
    }                                                                    \
    if (fci_select_if(idb, regptr))                                      \
        regptr->cmdreg = MCI_CMD_RX_FLUSH;                               \
    goto rxexit;


/*
 * cbus_interrupt
 * Handle an cBus controller interrupt
 */
void cbus_interrupt (void)
{
    mcitype *cbus;
    mciregtype *regptr;
    hwidbtype *idb = NULL;
    cdbtype *cdb;
    paktype *pak;
    ushort summary, bytes, status, index, throttle, temp, srbinfo = 0;
    int indx;
    long asr;
    void (*iqueue)(hwidbtype *, paktype *);
    tring_hdr *trh;
    fddi_hdr *fddi;
    sap_hdr *llc1;
    uchar bitswapped_addr[IEEEBYTES];

    cbus = (mcitype *) cbusQ.qhead;
    regptr = cbus->regptr;
    throttle = sched_run_interval;

    /*
     * Come here to service more interrupts.
     */
#ifdef C7000
 rxretry:
#endif

    /*
     * Process receive queue until empty.
     * FCI_CMD_RX_SETUP clears the interrupt request.
     */
    while (TRUE) {

	/*
	 * Come here if exiting early from a receive routine
	 */
    rxexit:
  	regptr->cmdreg = FCI_CMD_RX_SETUP;
  	index = regptr->argreg;
 	if ((summary = regptr->cmdreg) != FCI_RSP_OKAY) {
 	    goto cbus_setup_error;
	}
	if ((short) index == -1) {
 	    break;
	}
 	if (index > (cbus->max_interfaces - 1)) {
 	    goto cbus_setup_error;
	}

	/*
	 * This is not entirely correct for the MIP--but it is enough
	 * to get us to our MIP_RECEIVE routine, where we shall find the
	 * real IDB.
	 */
	idb = cbus->interface_list[index];
	if (idb == NULL || idb->mci_index != index) {
	    /* no such idb */
#ifdef C7000
	    /*
	     *	There is one condition under which this is not an error--if
	     *  this is a MIP card with no interfaces configured, sending
	     *  up a love letter.
	     */
	    cdb = controller_find_by_unit(index);
	    if (cdb) {
		if (cdb->ctrlr_type == FCI_MIP_CONTROLLER) {
		    hwidbtype *hwidb;

		    hwidb = mip_icb_to_idb(cbus, index);
		    if ( (hwidb == (void *)-1) || (hwidb == (void *)0) ) {

			/* it's a love letter */
			mip_love_letter_handler(cbus,
					(regptr->rxlength & ~FDDIT_RIF_CAM),
					index);
			regptr->cmdreg = FCI_CMD_RX_FLUSH;
			goto rxexit;
		    } else {
			if (oir_debug)
			    buginf("\n#Error: cbus_interrupt, no such hwidb for MIP, not MIP loveletter\n cdb=%#x, cdb is for %s, hwidb=%#x, hwidb is for %s",
				   cdb, cdb->cdb_namestring,
				   hwidb, hwidb->hw_namestring);
		    }
		} else {
		    if (oir_debug)
			buginf("\n#Error: cbus_interrupt, no such hwidb, not MIP cdb, cdb=%#x, cdb is for %s",
			       cdb, cdb->cdb_namestring);
		}
	    } else {
		if (oir_debug)
		    buginf("\n#Error: cbus_interrupt, NULL cdb");
	    }
#endif
	    goto cbus_setup_error;
	}

 	if (throttle)
 	    goto cbus_check_sched;

    return_cbus_check_sched:
	idb->rxlength = bytes = regptr->rxlength;
	idb->rxrifwordlen = 0;
	idb->rxtype = regptr->rxtype;

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
	if ((tbridge_debug_pak_0 && !tbridge_debug_pak_1) ||
	    (tbridge_debug_pak_0 && tbridge_debug_pak_1 &&
	     idb->tbridge_on_hwidb)) {
	    ushort af_result = regptr->rxstatus;
	    ushort srb_result = regptr->rxsrbinfo;
	    ushort lower_half = af_result & 0xFF;
	    af_result >>= 8;
	    buginf("\nTB0: %s%s%s%s %s%s%s%s ",
		   ((af_result & FCI_CLNS_PROTOCOL_MASK) ? "C" : "_"),
		   ((af_result & FCI_XNS_PROTOCOL_MASK) ? "X" : "_"),
		   ((af_result & FCI_DECNET_PROTOCOL_MASK) ? "D" : "_"),
		   ((af_result & FCI_DOD_IP_PROTOCOL_MASK) ? "I" : "_"),
		   ((af_result & FCI_MAC_MASK) ? "H" : "_"),
		   ((af_result & FCI_BRIDGING_MASK) ? "B" : "_"),
		   ((af_result & FCI_MULTICAST_MASK) ? "M" : "_"),
		   ((af_result & FCI_STATION_MASK) ? "S" : "_"));
	    switch (idb->type) {
	    case IDBTYPE_FDDIT:
		buginf("cam %s%s%s ",
		       ((bytes & FDDIT_SRB) ? "R" : "_"),
		       ((bytes & FDDIT_CAM_VALID) ? "V" : "_"),
		       ((bytes & FDDIT_SA_FND) ? "F" : "_"));
	    case IDBTYPE_FDDI:
	    case IDBTYPE_CTR:
		buginf("rif %s%s%s%s ",
		       ((srb_result & FCI_RXSRB_PRESENT) ? "R" : "_"),
		       ((srb_result & FCI_RXSRB_MATCHED) ? "M" : "_"),
		       ((srb_result & FCI_RXSRB_DIR) ? "D" : "_"),
		       ((srb_result & FCI_RXSRB_EXPLORER) ? "E" : "_"));
		if (IDBTYPE_CTR == idb->type) {
		    buginf("spy %s%s %s",
			   (((lower_half & CTR_RXSTAT_FS) == CTR_RXSTAT_FS) ?
			    "FS" : "__"),
			   ((lower_half & CTR_RXSTAT_MAC) ? "_r" : "__"),
			   (((lower_half & CTR_RXSTAT_FS) == CTR_RXSTAT_FS) ?
			    "FS" : "__"));
		    if (lower_half & CTR_RXSTAT_BOTH)
			buginf("%s%s ",
			       ((lower_half & CTR_RXSTAT_SRB) ?	"R" : "_"),
			       ((lower_half & CTR_RXSTAT_MINE) ? "S" : "_"));
		    else
			buginf("%s ", "TB");
		}
		break;
	    default:
		break;
	    }
	    buginf("%s on %s %d bytes",
		   rxtype_string(idb->rxtype),
		   idb->fast_rcvidb->namestring,
		   bytes & ~FDDIT_RIF_CAM);
	}
#endif	/* TBRIDGE_DEBUG */

#ifdef GLOBAL_DEBUG  
	if (zebra_debug) {
	    idb->rxlength = bytes & ~FDDIT_RIF_CAM; 
	    status = regptr->rxstatus;
	    buginf("\n%c%d: stat %04x, len %04x, raw len: %04x, type %04x, "
		   "srb %04x - cbus_interrupt()",
		   *idb->name, idb->unit, status, idb->rxlength, 
		   regptr->rxlength, idb->rxtype, regptr->rxsrbinfo);
	}
#endif	 /* GLOBAL_DEBUG */

	/*
	 *	The costs of implementing this as a chain of ifs versus a
	 *  switch are as follows.  For a switch, there is an overhead of
	 *  7 instructions for all paths.  For a chain of ifs, there is an
	 *  overhead of 1 instruction, plus 2 instructions for every if that
	 *  must be executed before the code.  Thus FDDI has 5 instructions
	 *  of overhead (note the second if test inside the first), ethernet
	 *  has 5, token ring has 7, etc.  One of the switch path's
	 *  instructions is pretty hairy, all others are roughly comparable.
	 *
	 *	This theory has even been checked out with a logic analyzer.
	 *  The third case in the if chain is slightly faster as a switch,
	 *  so the trade off is pretty good.
	 *
	 *	This code becomes less efficient if you add cases that are
	 *  non-contiguous.  Make sure that all cases from 0 to n, where
	 *  n is the largest possible value, are explicitly covered.  The
	 *  default case does not mean that you are generating efficient
	 *  code.
	 */
        switch (idb->type) {
	case IDBTYPE_FDDIT:
	    if ((srbinfo = regptr->rxsrbinfo) & FCI_RXSRB_PRESENT)
		idb->rxrifwordlen = (srbinfo & FCI_RXSRB_LENGTH);
	    /* fall through... */
	case IDBTYPE_FDDI:
	    idb->rxlength = bytes = bytes & ~FDDIT_RIF_CAM;
	    { CBUS_FDDI_RECEIVE; }
	    break;

	case IDBTYPE_MCIETHER:
	case IDBTYPE_CBUSETHER:
	case IDBTYPE_FEIP:
	    { CBUS_ETHER_RECEIVE; }
	    break;

	case IDBTYPE_CTR:
	    { CBUS_TOKEN_RECEIVE; }
	    break;

	case IDBTYPE_MCISERIAL:
	case IDBTYPE_CBUSSERIAL:
	case IDBTYPE_ASYNC:
	case IDBTYPE_HSSI:
	    { CBUS_HSSI_RECEIVE; }
	    break;

	case IDBTYPE_AIP:
	    { CBUS_AIP_RECEIVE; }
	    break;
	    
	case IDBTYPE_T1:
	case IDBTYPE_E1:
#ifdef C7000
	    {
		hwidbtype *first_idb = idb;

		/* find the real IDB this goes with */
		idb = mip_icb_to_idb(cbus, index);
		if (idb == (void *)-1) {
		    mip_love_letter_handler(cbus, first_idb->rxlength, index);
		    regptr->cmdreg = FCI_CMD_RX_FLUSH;
		    break;
		} else if (idb == NULL) {
		    /* some sort of error */
		    goto cbus_setup_error;
		} else if (idb == first_idb) {
		    ;	/* already set up */
		} else {
		    /* move the relevant fields */
		    idb->rxlength = first_idb->rxlength;
		    idb->rxrifwordlen = 0;
		    idb->rxtype = first_idb->rxtype;
		}
		{ CBUS_MIP_RECEIVE; }
	    }
	    break;

	case IDBTYPE_CIP:
	    {
		channel_sb_t *chan_sb;
		CBUS_CHANNEL_RECEIVE;
	    }
	    break;
#else

	    /* fall through IDBTYPE_UNKNOWN... */
#endif
	case IDBTYPE_UNKNOWN:
	    /*
	     *	You must specify this case, or the compiler generates an
	     *	additional instruction for the switch statement.
	     */
	    /* fall through... */
	default:
            regptr->cmdreg = FCI_CMD_RX_FLUSH;
            goto rxexit;
	}
    }

    /*
     * Process transmit done, interface up/down, and error counters
     */
    for (indx = 0, asr = regptr->asr; asr != 0; asr <<= 1, indx++) {
	if (asr < 0) {
	    regptr->argreg = indx;
	    regptr->cmdreg = FCI_CMD_CONDITIONS;
	    if ((status = regptr->argreg) == 0)
		continue;
	    if ((cdb = controller_find_by_unit(indx)) == NULL) {
		/*
		 * Self-protection code -- in case we get a bad value.
		 */
		errmsg(&msgsym(INTR, CBUS), indx, "conditions");
		(void) cbus_init(cbus, cbus->critical);
		cbus_restarts_error++;
		return;
	    }
	    (*cdb->love_note)(cdb, status);
	}
    } 
#ifdef C7000
    for (indx = 32, asr = regptr->asrext0; asr != 0; asr <<= 1, indx++) {
	if (asr < 0) {
	    regptr->argreg = indx;
	    regptr->cmdreg = FCI_CMD_CONDITIONS;
	    if ((status = regptr->argreg) == 0)
		continue;
	    if ((cdb = controller_find_by_unit(indx)) == NULL) {
		/*
		 * Self-protection code -- in case we get a bad value.
		 */
		errmsg(&msgsym(INTR, CBUS), indx, "conditions");
		(void) cbus_init(cbus, cbus->critical);
		cbus_restarts_error++;
		return;
	    }
	    (*cdb->love_note)(cdb, status);
	}
    }
    for (indx = 64, asr = regptr->asrext1 << 16 ; asr != 0; asr <<= 1, indx++) {
	if (asr < 0) {
	    regptr->argreg = indx;
	    regptr->cmdreg = FCI_CMD_CONDITIONS;
	    if ((status = regptr->argreg) == 0)
		continue;
	    if ((cdb = controller_find_by_unit(indx)) == NULL) {
		/*
		 * Self-protection code -- in case we get a bad value.
		 */
		errmsg(&msgsym(INTR, CBUS), indx, "conditions");
		(void) cbus_init(cbus, cbus->critical);
		cbus_restarts_error++;
		return;
	    }
	    (*cdb->love_note)(cdb, status);
	}
    }
#endif
    return;

 cbus_setup_error:
#ifdef C7000
    /*
     * SSP has a packet for us to fragment.
     */
    if (FCI_RSP_ERR_FRAG == summary) {
	cbus_fragment_packet(cbus, regptr, index, summary);

	idb = cbus->interface_list[index];
	if (idb && throttle && AWAKE(sched_run_time)) 
	  cbus_mci_throttle(idb);
	goto rxretry;
    } else
#endif

    /*
     * Switch processor has signaled to us that it is not processing packets
     * due to a hot stall event for which the process-level handler has not
     * yet been run.  Don't spit out messages to the user.
     */
    if (summary == FCI_RSP_ERR_HSA) {
	(void)cbus_init(cbus, cbus->critical);
	return;
    }

#ifdef C7000
    if (FCI_RSP_ERR_SSE == summary) {
	sse_error(cbus);
	return;
    }
#endif
    cbus_restarts_error++;
    errmsg(&msgsym(INITERR, CBUS), index, summary, idb,
	   (idb ? idb->mci_index : 0), "rx_setup"); 
    /*
     * Pull in registers, mema, and memd lowcore so we can debug
     * what the heck happened.
     */
    memd_lowcore_capture(cbus, TRUE);
    (void) cbus_init(cbus, cbus->critical);
    return;

 cbus_check_sched:
    if (AWAKE(sched_run_time)) {
	cbus_mci_throttle(idb);
	goto rxexit;
    }
    goto return_cbus_check_sched;
}



/*
 * cbus_daughter_cmd
 * send a command thru the cbus controller to the daughter.
 */
void cbus_daughter_cmd (hwidbtype *idb, uint cmd, uint arg)
{
    leveltype level;
    mciregtype *regptr;

    regptr = idb->mci_regptr;
    level = raise_interrupt_level(NETS_DISABLE);
    regptr->argreg = idb->cbus_ctrlr_index;
    regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    regptr->argreg = arg;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = cmd;
    regptr->cmdreg = FCI_CTRLR_CMD;
    reset_interrupt_level(level);
}

#define DCI_MEMTEST_INDX_PLUS 1
/*
 * dci_memtest
 * Run some memory tests (memd) on the daughter controller.
 * Basic tests:
 *      cBus writes 0x0000 to memory  -- dci reads it.
 *      cBus writes 0xffff to memory -- dci reads it.
 * 	cBus writes address + 1 to memory -- dci reads it.
 *      dci writes 0x0000 to memory -- cBus reads it.
 *      dci writes 0xFFFF to memory -- cBus reads it.
 *      dci writes address + 1 memory -- cBus reads it
 */
static char *dci_memtest (dcitype *dci)
{
    mciregtype *regptr;
    mcitype *cbus;
    int mem_size;
    int indx, jndx;
    ushort sresult;
#ifdef C7000
    dgtype *dgbus;
#endif /* C7000 */
    regptr = dci->regptr;

    /*
     * Select the appropriate controller.
     */
    regptr->argreg = dci->ctrlr_unit;
    regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
#ifdef C7000
    dgbus = pslot_to_dbus(dci->ctrlr_unit);
    /*
     * We don't load the card here, because it has already been handled
     * by dgbus_analyze(), so we just reset and restart it.
     */
    
    /*
     * As far as we can tell, there is no reason to issue this reset here.
     * In order to limit our exposure, we'll omit this reset only for VIPs
     *
     */
    if (!vip_controller(dci->ctrlr_type))
        dbus_ctrlr_reset(dgbus);

    dbus_slot_enable(dgbus);
    DELAY(FCI_SOFT_RESET_TIME);
#else
    regptr->argreg = dci->ctrlr_unit;
    regptr->cmdreg = FCI_CMD_CTRLR_LOAD;
    DELAY(FCI_SOFT_RESET_TIME);
#endif
    cbus = dci->fci;

    /*
     * Step on the complex.
     */
#ifndef C7000
    reg_invoke_cbus_hard_reset(cbus);
    reg_invoke_cbus_soft_reset(cbus);
#endif
    mem_size = (cbus->memd_size & 0xFFFF);
#define SPEEDUP_DCI_MEMTEST
#ifdef SPEEDUP_DCI_MEMTEST
    mem_size = mem_size & 0xFFF;  
#endif

    regptr->argreg = dci->ctrlr_unit;
    regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    
    /*
     * If cbusII tell the daughter card to uses page 11 (0xb).
     */
    if (cbus->flags & CBUSII) {
	fci_ctrlr_cmd_arg(dci, regptr, FCI_CTRLR_PAGE_SELECT, 11,
			  CTRLR_CMD_ATTEMPTS, TRUE);
    }

    /* 
     * Assure high bits of largreg are zeroed
     * tests are using only the low 16 bits
     */
    regptr->largreg = 0;
    
    for (indx = 0; indx < 2; indx++) {

	/*
	 * Set all main buffer memory to zero and then 0xffff.
	 */
	regptr->argreg = FCI_MEMD_BASE;
	regptr->cmdreg = FCI_CMD_MEMD_SELECT;

	for (jndx = FCI_MEMD_BASE; jndx < mem_size; jndx++)
	    regptr->write0short = indx ? 0xFFFF : 0;

	/*
	 *  Read it back via the daughter controller.
	 */
	for (jndx = FCI_MEMD_BASE; jndx < mem_size; jndx++) {
	    if (!fci_ctrlr_cmd_arg(dci, regptr, FCI_CTRLR_MEMD_SELECT, jndx, 
				   CTRLR_CMD_ATTEMPTS, TRUE))
		return("memd read");
	    if (!fci_ctrlr_qry(dci, regptr, FCI_CTRLR_MEMD_READ,
			       &sresult, CTRLR_CMD_ATTEMPTS, TRUE))
		return("memd read");
	    if (sresult != (indx ? 0xFFFF : 0)) {
		errmsg(&msgsym(CORRUPT, CBUS), dci->ctrlr_unit,
		       (indx ? 0xFFFF : 0), sresult, jndx);
		return("memd read");
	    }
	}
    }

    /*
     * Test main memory address + 1.
     */
    regptr->argreg = FCI_MEMD_BASE;
    regptr->cmdreg = FCI_CMD_MEMD_SELECT;

    for (indx = FCI_MEMD_BASE; indx < mem_size; indx++)
	regptr->write0short = indx + DCI_MEMTEST_INDX_PLUS;

    /*
     *  Read it back via the daughter controller.
     */
    for (indx = FCI_MEMD_BASE; indx < mem_size; indx++) {
	if (!fci_ctrlr_cmd_arg(dci, regptr, FCI_CTRLR_MEMD_SELECT, indx, 
			       CTRLR_CMD_ATTEMPTS, TRUE))
	    return("memd read");	    
	if (!fci_ctrlr_qry(dci, regptr, FCI_CTRLR_MEMD_READ,
			   &sresult, CTRLR_CMD_ATTEMPTS, TRUE))
	    return("memd read");
	if (sresult != indx + DCI_MEMTEST_INDX_PLUS) {
	    errmsg(&msgsym(CORRUPT, CBUS), dci->ctrlr_unit,
		   (indx + DCI_MEMTEST_INDX_PLUS), sresult, indx);
	    return("memd read");
	}
    }

    /*
     * Write memd via the daughter controller and read it via the cBus.
     */

    for (indx = 0; indx < 2; indx++) {

	/*
	 * Set all main buffer memory to zero and then 0xffff.
	 */
	for (jndx = FCI_MEMD_BASE; jndx < mem_size; jndx++) {
	    if (!fci_ctrlr_cmd_arg(dci, regptr, FCI_CTRLR_MEMD_SELECT, jndx, 
				   CTRLR_CMD_ATTEMPTS, TRUE))
		return ("memd write");
	    if (!fci_ctrlr_cmd_arg(dci, regptr, FCI_CTRLR_MEMD_WRITE, 
				   (indx ? 0xFFFF : 0x0000), 
				   CTRLR_CMD_ATTEMPTS, TRUE))
		return ("memd write");
	}
	regptr->argreg = FCI_MEMD_BASE;
	regptr->cmdreg = FCI_CMD_MEMD_SELECT;

	for (jndx = FCI_MEMD_BASE; jndx < mem_size; jndx++) {
	    sresult = regptr->readshort;
	    if (sresult != (indx ? 0xFFFF : 0)) {
		errmsg(&msgsym(CORRUPT, CBUS), dci->ctrlr_unit,
		       (indx ? 0xFFFF : 0), sresult, jndx);
		return("memd write");
	    }
	}
    }

    /*
     * Write memd with address + 1.
     */
    for (indx = FCI_MEMD_BASE; indx < mem_size; indx++) {
	if (!fci_ctrlr_cmd_arg(dci, regptr, FCI_CTRLR_MEMD_SELECT, indx, 
			       CTRLR_CMD_ATTEMPTS, TRUE))
	    return ("memd write");
	if (!fci_ctrlr_cmd_arg(dci, regptr, FCI_CTRLR_MEMD_WRITE,
			       indx + DCI_MEMTEST_INDX_PLUS, 
			       CTRLR_CMD_ATTEMPTS, TRUE))
	    return ("memd write");
    }
    regptr->argreg = FCI_MEMD_BASE;
    regptr->cmdreg = FCI_CMD_MEMD_SELECT;

    for (indx = FCI_MEMD_BASE; indx < mem_size; indx++) {
	sresult = regptr->readshort;
	if (sresult != indx + DCI_MEMTEST_INDX_PLUS) {
	    errmsg(&msgsym(CORRUPT, CBUS), dci->ctrlr_unit,
		   (indx + DCI_MEMTEST_INDX_PLUS), sresult, indx);
	    return("memd write");
	}
    }

    /*
     * Return NULL pointer if no errors.
     */
    return(NULL);
}

/*
 * sync the IDBs buffer variables to its CDBs point of view
 * Should be called after cbus_init.  Doesn't necessarilly sync all the
 * fields
 */
void load_idb_buffer_info (hwidbtype *idb, cdbtype *cdb)
{
    idb->buffer_pool = cdb->buffer_pool;
    idb->mci_txcount_default = cdb->mci_txcount_default;
    idb->mci_txcount = cdb->mci_txcount;
    idb->mci_txqloc = cdb->mci_txqloc;
}


typedef struct buffer_pool_type_ {
    int nb;         /* Number of buffers actually in the pool */
    int bw;         /* Total of all interface bandwidths in this pool */
    int bs;         /* Size in words of buffer for this pool */
    int bp;         /* Originally assigned buffer pool index */
    int interfaces; /* number of interfaces in this pool */
    boolean done;   /* Consider this pool in further FS calculations? */
} buffer_pool_type;


static void cbus_buffer_carve (mcitype *cbus)
{
    mciregtype *regptr;
    int indx, jndx, rxb_limit;
    cdbtype *cdb;
    uint sresult, overhead, memd_size;
    int i, j, n, p, sw, totalbufs;
    buffer_pool_type bp[FCI_BUF_POOL_MAX];
    buffer_pool_type bp_temp;
    int maxsize, pool;
    int bw, tbw;
    int if_temp, delta;
    ulong txcount;
    dcitype *dci;
    ushort if_type;
    ushort result;
    ulong tx_queue;
    
    regptr = cbus->regptr;
    
    /*
     * Create some RX_ERROR buffers.
     */
    regptr->argreg = 0;			/* No overhead */
    regptr->cmdreg = FCI_CMD_OVERHEAD;

    /*
     * Create three (3) love note buffers for each FDDI interface and one (1)
     * love buffer per interface for other style devices.
     */

    jndx = (cbus->fddi_count * 3) + (cbus->interface_count - cbus->fddi_count +
				     dsx1_count);

    /*
     * The cBusII is limited to 320 buffers -- attempt to allocate fewer
     * love note buffers AND more real buffers if running on the cBusII.
     */
    if (cbus->flags & CBUSII)
	if (!(jndx = (jndx >> 1)))
	    jndx = 1;

    /*
     * Create the love note buffers.
     */
    for (indx = 0; indx < jndx; indx++) {
	sresult = cbus_create_buffer(regptr, FCI_RX_ERROR_POOL, 0);
	if (sresult != FCI_RSP_OKAY)
	  errmsg(&msgsym(TXALLOC, CBUS), sresult);
    }

    /*
     * Create a beacon claim buffer for each FDDI interface.
     */
    for (indx = 0; indx < cbus->fddi_count; indx++)
	(void) cbus_create_buffer(regptr, FDDI_CLAIM_BEACON_POOL,
				  FDDI_CLAIM_BEACON_BUFFER_SIZE / 2);

    /*
     * Get total amount of memory to carve up.
     */
    regptr->argreg = MCI_OFFSETSIZE;	/* Encapsulation area (sword count) */
    regptr->cmdreg = FCI_CMD_OVERHEAD;
    overhead = regptr->argreg;		/* Total buffer overhead */

    /*
     * Setup memd_size with the number of words left after love note pool,
     * claim beacon pool and base.
     */
    memd_size = cbus->memd_size;
    if (cbus->flags & CBUSII) {
	/*
	 * In cbusII, SP, and SSP, all of the buffer overhead is in
	 * page zero (0x1000 words).
	 */
	memd_size -= 0x1000;
    } else {
	memd_size &= 0xffff;
	memd_size -= (FCI_MEMD_BASE + 1);
	/*
	 * On cbusI, love notes (error buffers) need to be accounted for.
	 * The size of love note headers is (overhead - MCI_OFFSETSIZE).
	 */
	memd_size -= ((overhead - MCI_OFFSETSIZE) *
		  ((cbus->interface_count - cbus->fddi_count) +
		   (3 * cbus->fddi_count)));
    }


/*
 * Needs to be at least 16 for TRIP and FIP to share buffers...
 * Needs to be at least 60 for FSIP with max. encapsulation (SMDS) and
 * EIP to share buffers. Now 256 to prevent more than four buffer pools
 * from being formed. 
 *  
 * Keep this define in the function, b/c, RSP folks are 
 * using a constant of the same name. 
 */
#define CBUSBUFDELTA 256
    delta = CBUSBUFDELTA;

    /*
     * FDDI claim beacon buffers use up buffer header space (cbusI) and
     * claim size space (cbusI, cbusII, SP, SSP), but no encaps space.
     */
    memd_size -= (cbus->fddi_count *
		  ((FDDI_CLAIM_BEACON_BUFFER_SIZE / 2) +
		   (overhead - MCI_OFFSETSIZE)));

    /*
     * Find out how many of each MTU in the system.
     * This should never be more than 4.  If it's greater than 4,
     * then double the size delta allowed for buffer pool sharing
     * and start over.
     * Also keep track of total interface bandwidth within buffer
     * pools and total for the system.
     */
 too_many_pools:
    memset(cbus->if_sizes, 0, sizeof(cbus->if_sizes));
    memset(bp, 0, sizeof(bp));
    cbus->if_sizes[FCI_MINI_BUF_POOL] = FCI_MINI_BUF_SIZE;
    bp[FCI_MINI_BUF_POOL].bp = FCI_MINI_BUF_POOL;
    bp[FCI_MINI_BUF_POOL].bs = FCI_MINI_BUF_SIZE >> 1;
    cbus->if_sizes[FCI_SYSTEM_BUF_POOL] = FCI_SYSTEM_BUF_SIZE;
    bp[FCI_SYSTEM_BUF_POOL].bp = FCI_SYSTEM_BUF_POOL;
    bp[FCI_SYSTEM_BUF_POOL].bs = FCI_SYSTEM_BUF_SIZE >> 1;
    cbus->if_sizes_count = 2;
    tbw = 0;
    for (cdb = (cdbtype *)cdbQ->qhead ; cdb != NULL ; cdb = cdb->next) {

	/*
	 * The ds3 interface tacks the crc onto the end of the buffer.
	 */
	maxsize = (cdb->buffersize + 3) & ~3;
	/* 
	 * Do not permit cbus buffer pools to be carved 
	 * into sizes less than the ethernet mtu. 
	 */
	if (maxsize < MAXETHERSIZE)
	    maxsize = MAXETHERSIZE;
	pool = FCI_RX_POOL_0;
	
	/*
	 * First try and find a matching entry in the bp[] table.
	 * If there is one, count it.  If there isn't one, create it.
	 */
	while (TRUE) {
	    if_temp = cbus->if_sizes[pool] - maxsize;
	    if ((if_temp >= 0) && (if_temp <= delta))
		/* Same size or slightly smaller */
		break;
	    if ((if_temp >= -delta) && (if_temp < 0)) {
		/* Slightly larger */
		cbus->if_sizes[pool] = maxsize;
		bp[pool].bs = ((maxsize + 1) >> 1);
		break;
	    }
	    /* Not within the delta either way */
	    if (cbus->if_sizes[pool] == 0) {
		cbus->if_sizes[pool] = maxsize;
		cbus->if_sizes_count++;
		bp[pool].bp = pool;
		bp[pool].bs = ((maxsize + 1) >> 1);
		break;
	    }
	    if (pool++ >= FCI_RX_POOL_3) {
		delta *= 2;
		goto too_many_pools;
	    }
	}

	/*
	 * Avoid a divide by zero error
	 */
	if (cdb->bandwidth_def)
	    bw = SCALED_BANDWIDTH(cdb->bandwidth_def);
	else
	    bw = 0;
	tbw += bw;
	bp[pool].bw += bw;
	bp[pool].interfaces++;
	cdb->buffer_pool = pool;
    }

    /*
     * Sort the counts in ascending order by buffer size.
     * Adjust bubble sort index to exclude system buffer pools 
     * not associated w/ physical interfaces from this reordering. 
     */
    for (pool = FCI_RX_POOL_1; pool < cbus->if_sizes_count; pool++) {
	for (j = (cbus->if_sizes_count - 1); j >= pool; j--) {
	    if (cbus->if_sizes[j-1] > cbus->if_sizes[j]) {
		if_temp = cbus->if_sizes[j-1];
		bcopy(&bp[j-1], &bp_temp, sizeof(buffer_pool_type));
		cbus->if_sizes[j-1] = cbus->if_sizes[j];
		bcopy(&bp[j], &bp[j-1], sizeof(buffer_pool_type));
		cbus->if_sizes[j] = if_temp;
		bcopy(&bp_temp, &bp[j], sizeof(buffer_pool_type));
	    }
	}
    }
    
    /*
     * Subtract out MEMD for mini and normal system buffers
     * (256 and 1024 bytes + encaps and buffer overhead, respectively.
     */
    memd_size -= FCI_MINI_BUF_COUNT * ((FCI_MINI_BUF_SIZE / 2) + overhead);

    memd_size -= FCI_SYSTEM_BUF_COUNT * ((FCI_SYSTEM_BUF_SIZE / 2) + overhead);

#ifdef C7000
    /*
     * Support larger MEMD sizes.  If we are limited by the number of
     * buffers, promote smaller pools into larger pools.
     */
#define SP_MAX_BUFFERS	471
    
    totalbufs = 0;
    for (pool = FCI_RX_POOL_0; pool < cbus->if_sizes_count; pool++) {
	p = ((bp[pool].bw * 1000) / tbw);
	sw = ((memd_size * p) / 1000);
	bp[pool].nb = (sw / (bp[pool].bs + overhead));
	totalbufs += bp[pool].nb;
    }
    while ((totalbufs > SP_MAX_BUFFERS) &&
	   (cbus->if_sizes_count > FCI_RX_POOL_1)) { 
     
	/*
	 * Add pool 0 into pool 1.  First up interfaces.  Then fold the
	 * smallest pool up one.
	 */
	for (cdb = (cdbtype *)cdbQ->qhead ; cdb != NULL ; cdb = cdb->next) {
	    if (cdb->buffer_pool == bp[FCI_RX_POOL_0].bp)
		cdb->buffer_pool = bp[FCI_RX_POOL_1].bp;
	}
	totalbufs -= bp[FCI_RX_POOL_0].nb;
	bp[FCI_RX_POOL_1].bw += bp[FCI_RX_POOL_0].bw;
	bp[FCI_RX_POOL_1].interfaces += bp[FCI_RX_POOL_0].interfaces;
	for (pool = FCI_RX_POOL_1; pool < cbus->if_sizes_count; pool++) {
	    bp[pool - 1] = bp[pool];
	    cbus->if_sizes[pool - 1] = cbus->if_sizes[pool];
	}
	cbus->if_sizes_count--;
	memset(&bp[cbus->if_sizes_count], 0, sizeof(buffer_pool_type));

	/*
	 * Recalculate the number of buffers we need.
	 */
	totalbufs -= bp[FCI_RX_POOL_0].nb;
	p = ((bp[FCI_RX_POOL_0].bw * 1000) / tbw);
	sw = ((memd_size * p) / 1000);
	bp[FCI_RX_POOL_0].nb = (sw / (bp[FCI_RX_POOL_0].bs + overhead));
	totalbufs += bp[FCI_RX_POOL_0].nb;
    }
#endif

    totalbufs = 0;

    /*
     * Make sure that no single buffer pool contains less than
     * about 20% of the total pool.  Since we will later use each
     * buffer pool's bandwidth to determine its fair share of MEMD,
     * we can increase a low bandwidth pool's MEMD size by artificially
     * boosting its aggregate bandwidth metric.
     */
    for (pool = FCI_RX_POOL_0; pool < cbus->if_sizes_count; pool++) {
	if ((tbw / (bp[pool].bw+1)) > 5)
	    bp[pool].bw = tbw / 5;
    }

    /*
     * Now recalculate total bandwidth for all pools using adjusted
     * aggregate pool bandwidths.
     */
    for (tbw = 0, pool = FCI_RX_POOL_0; pool < cbus->if_sizes_count; pool++)
	tbw += bp[pool].bw;

    /*
     * For first pass through, only carve up a few of the mini
     * system buffers.  This will limit MEMD waste for cbusII
     * all-ethernet configurations, where cbusII buffer headers
     * are in shorter supply than actual buffer memory.
     */
    bp[FCI_MINI_BUF_POOL].nb = FCI_MINI_BUF_COUNT_LOW;
    bp[FCI_SYSTEM_BUF_POOL].nb = FCI_SYSTEM_BUF_COUNT;

    /*
     * Divvy up memory based on bandwidth share.
     */
    for (pool = FCI_RX_POOL_0; pool < cbus->if_sizes_count; pool++) {
	p = ((bp[pool].bw * 1000) / tbw);
	sw = ((memd_size * p) / 1000);
	bp[pool].nb = (sw / (bp[pool].bs + overhead));
    }

    /*
     * Now start first pass of carving up system and interface
     * buffer pools.
     */
    for (pool = 0; pool < cbus->if_sizes_count; pool++) {
	cbus->buffers[pool] = 0;
	for (j = 0; j < bp[pool].nb; j++) {
	    sresult = cbus_create_buffer(regptr, pool, bp[pool].bs);
	    if (sresult != MCI_RSP_OKAY) { /* this should NEVER happen */
		if (!(cbus->flags & CBUSII))
		    errmsg(&msgsym(BUFFER, CBUS), cbus->unit, sresult,
			   bp[pool].bs, pool, j);
		break;
	    }
	    cbus->buffers[pool]++; /* calculate the real number carved up */
	    totalbufs++;
	}
    }

    /*
     * On second pass, use remainder of MEMD and buffer headers
     * carving out mini system buffers.
     */
    pool = FCI_MINI_BUF_POOL;
    for (j = 0; j < FCI_MINI_BUF_COUNT - FCI_MINI_BUF_COUNT_LOW; j++) {
	if (cbus_create_buffer(regptr, pool, bp[pool].bs) != MCI_RSP_OKAY)
	    break;
	cbus->buffers[pool]++; /* calculate the real number carved up */
	totalbufs++;
    }

    cbus->system_buffer_size = 0;
    cbus->system_buffer_pool = FCI_SYSTEM_BUF_POOL;

    /*
     * Set up some interface specific values.
     */

    for (cdb = (cdbtype *)cdbQ->qhead ; cdb != NULL ; cdb = cdb->next) {

#ifdef C7000
	/*
	 * If this card has been removed, don't carve buffers for it
	 */
	if (!touch_device(pslot_to_dbus(cdb->slot)))
	    continue;
#endif
	/*
	 * Fix up buffer pool indices
	 */
	for (pool = 0; pool < cbus->if_sizes_count; pool++) {
	    if (cdb->buffer_pool == bp[pool].bp) {
		cdb->buffer_pool = pool;
		break;
	    }
	}
	dci = cbus->dci[cdb->slot];    
	if (!fci_grab_if(dci, regptr, &result, cdb->unit, &if_type)) {
	    errmsg(&msgsym(INITERR, CBUS), cdb->unit, 
		   result, cdb, cdb->unit, "cmd_select");
	    continue;
	}

	pool = cdb->buffer_pool;
	
	/*
	 * Assign each serial and ethernet 
	 * interface to a buffer pool and fix each interface
	 * with a maximum buffer size and receive offset.
	 * Other interface types are initialized from their drivers.
	 */
	if ((cdb->pif_type == PIFT_ETHERNET) ||
	    (cdb->pif_type == PIFT_FASTETHER) ||
	    (cdb->pif_type == PIFT_SERIAL)) {
	    /*
	     * Setup for cbus ether or serial interfaces.
	     */
	    cbus_idb_assign_pool(regptr, cdb->buffer_pool);
	    cbus_idb_rx_max_byte_size(regptr, cdb->max_buffer_size);
	    cbus_idb_rx_error(regptr, FCI_RX_ERROR_POOL);
	    if ((cdb->pif_type ==  PIFT_ETHERNET) || 
		(cdb->pif_type == PIFT_FASTETHER))
		cbus_idb_rx_offset(regptr, MCI_ETHER_OFFSET - cdb->encsize);
	    else
		cbus_idb_rx_offset(regptr, MCI_SERIAL_OFFSET - cdb->encsize);
	}
	
	/*
	 * Compute the receive buffer limit.
	 * Give each interface equal share with no oversubscription,
	 * though take care to round up integer division.
	 * Ideally, we should take bandwidth into consideration, but
	 * that's a little radical for now :-)
	     */
	n = bp[pool].nb;
	i = bp[pool].interfaces;
	if (!n || !i) {
	    /*
	     * what? we have a card with no interfaces configurable?
	     * skip it!
	     */
	    continue;
	}
	rxb_limit = n / i;
	if ((n % i)  > (rxb_limit / 2))
	  rxb_limit++;
	
	/*
	 * Allow RX oversubscription for NFS bursts on MEC.
	 */
	if ((rxb_limit < 10) && (cdb->pif_type == PIFT_ETHERNET))
	  rxb_limit = 10;
	cbus->rxbuffers[cdb->unit] = rxb_limit;
	cbus_idb_rx_buffer_limit(regptr, rxb_limit);
	
	/*
	 * Allow up to 200% TX oversubscription for most controllers.
	 * However, limit TQL to 100% subscription for FSIP, since it's
	 * an output hog.  This shouldn't cause unreasonable tx drops,
	 * since the FSIP does local tx buffering.
	 */
	txcount = totalbufs / (cbus->interface_count + 1);
	if (cdb->pif_type != PIFT_SERIAL)
	  txcount *= 2;
	/*
	 * Make sure to round up and take SLIMIT into consideration.
	 */
	txcount += 2;
	
	/*
	 * plus claim/beacon frame
	 */
	if (cdb->pif_type == PIFT_FDDI) 
	  txcount++;
	cdb->mci_txcount_default = txcount;
	
	/*
	 * Allow override to tone down serials.
	 */
	{
	    hwidbtype *hwidb;
	    hwidb = controller_find_from_cdb (cdb);
	    if (cdb->mci_txcount_enabled) {
		tx_queue = cdb->mci_txcount;
	    } else if (hwidb && hwidb_use_short_tx_q_inline(hwidb)) {
		tx_queue = 2 + hwidb->firstsw->visible_bandwidth /
		    MCI_DOUBLE_BUFFER;
		if (tx_queue > cdb->mci_txcount_default)
		    tx_queue = cdb->mci_txcount_default;
	    } else {
		tx_queue = cdb->mci_txcount_default;
	    }
	    cdb->mci_txcount = tx_queue;
	}
	
	regptr->argreg = tx_queue;
	regptr->cmdreg = FCI_CMD_QUEUE;
	regptr->argreg = 2;
	regptr->cmdreg = MCI_CMD_SLIMIT;
	if (cdb->pif_type == PIFT_FDDI) {
	    if (cbus->interface_list[cdb->unit]->fddi_options &
		FDDI_SMT_FRAMES) {
		if (!smt_process_id) {
		    /* fddi_smt_input is going away for 9.2 */
		    queue_init(&smtinputQ, nfddinets);
		    start_smt_input_proc();
		}
	    }
	}
    }
}

/*
 * cbus_init
 * Initialize a controller.  This routine is reentrant.
 * We need to be careful of when we reinitialize a controller.
 * If we are executing critical code (in particular process level output
 * with interrupts enabled) we must not call cbus_init() at interrupt level
 * since the output window pointers in the cBus will become random.
 * In that case set the restartdeferred flag and the periodic checker will
 * take care of restarting us.
 */
 
void cbus_init (register mcitype *cbus, boolean criticalflag)
{
    mciregtype *regptr;
    int indx, jndx;
    cdbtype *cdb;
    hwidbtype *hwidb;
    uint lights;
    dcitype *dci;
    int tx_queue;
    boolean eoir;
#ifdef C7000
    slottype* sp;
#endif

    eoir = cbus->flags & EOIR_INIT;

    if (zebra_debug)
	return;

    /*
     * First check if executing in a critical section.
     * Step on the cBus controller.
     */
    if (criticalflag) {
	cbus->restartdeferred = TRUE;
	return;
    }
    cbus->restartdeferred = FALSE;
    cbus->critical = FALSE;
    GET_TIMESTAMP(cbus->lastreset);
    regptr = cbus->regptr;

    cbus->resetting = TRUE;
    
    /*
     * Step on the complex.
     */
    if (!eoir) {
	reg_invoke_cbus_hard_reset(cbus);
	reg_invoke_cbus_soft_reset(cbus);
    }
#ifdef C7000
    sse_init(cbus);			/* initialize SSE if one is present */
#endif
    /*
     * If no controllers present, exit stage left.
     */
    if (cdbQ->qhead == NULL) {
	regptr->argreg = 0;		/* all lights off -- no controllers */
	regptr->cmdreg = FCI_CMD_LIGHTS;
	cbus->resetting = FALSE;
	return;
    }

    /*
     *	Sync the controller buffer info to the IDBs
     */
    FOR_ALL_HWIDBS (hwidb) {
	if (!(hwidb->status & IDB_CBUS))
	    continue;
	if (hwidb->status & IDB_NOHARDWARE)
	    continue;
	
	if (is_atm(hwidb)) {
	    /*
	     * When there is a change in MTU, set the max_buffer_size
	     * and the buffersize to more than the new MTU.
	     */
	    if_maxdgram_adjust(hwidb, hwidb->maxmtu + ETHER_ARPA_ENCAPBYTES + 
			       ATM_SMDS_HDRBYTES);
	    hwidb->buffersize = hwidb->max_buffer_size;
	}
	
	/* skip VC IDBs: buffer management is handled by their controllers */
	if (!(hwidb->status & IDB_VIRTUAL_HW)) {
	    if (hwidb->mci_txcount_enabled) {
		tx_queue = hwidb->mci_txcount;
	    } else if (hwidb_use_short_tx_q_inline(hwidb)) {
		tx_queue = 2 + hwidb->firstsw->visible_bandwidth /
		    MCI_DOUBLE_BUFFER;
		if (tx_queue > hwidb->mci_txcount_default)
		    tx_queue = hwidb->mci_txcount_default;
	    } else {
		tx_queue = hwidb->mci_txcount_default;
	    }
	    hwidb->mci_txcount = tx_queue;

	    cdb = controller_find(hwidb->slot, hwidb->slotunit);
	    /*
	     *	We can be called before cbus_analyze() has finished finding
	     *  all the IPs, so there can be some IDBs left over from last
	     *	time without cdb's.  Thus it is not an error to fail here.
	     */
	    if (cdb == NULL)
		continue;
	    cdb->maxmtu = hwidb->maxmtu;
	    cdb->max_buffers = hwidb->max_buffers;
	    cdb->buffersize = hwidb->buffersize;
	    cdb->max_buffer_size = hwidb->max_buffer_size;
	    cdb->cbus_burst_count = hwidb->cbus_burst_count;
	    cdb->bandwidth_def = 
		SCALED_BANDWIDTH(hwidb->firstsw->visible_bandwidth_def);
	    cdb->buffer_pool = hwidb->buffer_pool;
	    cdb->mci_txcount = hwidb->mci_txcount;
	    cdb->mci_txcount_enabled = hwidb->mci_txcount_enabled;
	    cdb->mci_txcount_default = hwidb->mci_txcount_default;
	    cdb->mci_txqloc = hwidb->mci_txqloc;
	}
    }

    /*
     * Setup global bridging bits.
     */
    FOR_ALL_HWIDBS (hwidb) {
	if ((!(hwidb->status & IDB_CBUS)) || (hwidb->status & IDB_NOHARDWARE))
	    continue;
	cbus_mci_rxtypes_update(hwidb);
    }

    /*
     * Carve up MEMD and allocate buffers to interfaces
     */
    cbus_buffer_carve(cbus);

    /*
     * Program interrupt level for cbus complex
     */
    regptr->argreg = FCI_INTLEVEL;
    regptr->cmdreg = FCI_CMD_INTERRUPT;

    /*
     * Turn on the LEDs on the AGS+ cbus controllers.
     */
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CMD_LIGHTS;

    /*
     * Autonomous switch initialization:
     */
    /*
     * Setup cbus cache.
     *
     * Note that cbus_cache_init() will now call protocol specific routines
     * as appropriate before and/or after generic initialization.
     */
    cbus_cache_init();
    reg_invoke_bump_cache_version(NULL, NULL, TRUE);
    if (registry_populated(REG_SRT))
	reg_invoke_srt_setup_auto_sw_srb(NULL);

    /*
     * If a daughter controller exists
     * -- set the lights
     * -- send ipc signal if not eoir
     */
    for (jndx = 0, lights = 0; jndx < fci_max_dci_ctrlrs; jndx++) {
	dci = cbus->dci[jndx];
	if (dci) {
	    indx = jndx * fci_max_dci_ctrlrs;
	    lights |= (0x8000 >> indx);
	    reg_invoke_slot_enable_led_on(jndx);
#ifdef C7000
            if (!eoir) {
                reg_invoke_send_ipc_eoir_signal(dci->ctrlr_unit);
	    }
#endif
	}
    }
    regptr->argreg = lights;
    regptr->cmdreg = FCI_CMD_LIGHTS;

    /*
     * Tell cBus where to look for encapsulation data.
     */
    regptr->argreg = MCI_OFFSETSIZE;
    regptr->cmdreg = FCI_CMD_OVERHEAD;

    /*
     *	load up all the buffer specific stuff we have just set into the idbs
     */
    FOR_ALL_HWIDBS (hwidb) {
	if (!(hwidb->status & IDB_CBUS))
	    continue;
	if ((cdb = controller_find(hwidb->slot, hwidb->slotunit)) == NULL) {
	    /*
	     *	This is an error, but we will have annoyed them above,
	     *  so let's be quiet this time
	     */
	    continue;
	}
	load_idb_buffer_info(hwidb, cdb);
    }
    
    for (indx = 0; indx < cbus->max_interfaces; indx++) {

	cdb = controller_find_by_unit(indx);
#ifdef C7000
	sp = &slots[cdb->slot];
	if (cdb && cdb->reset &&
	    touch_device(pslot_to_dbus(cdb->slot))) {
	    if ( eoir && (! (sp->flags & BOARD_RELOADED) ) ) {
		cdb->cdb_status |= CDB_EOIR_RESET;
		if (oir_debug)
		    buginf("\nRequesting eoir reset of %s",
			   cdb->cdb_namestring);
	    } else {
		if (cbus_debug)
		    buginf("\nResetting %s, eoir=%d, sp->flags=%#x",
			   cdb->cdb_namestring, eoir, sp->flags);
	    }
	    (*cdb->reset)(cdb);
	    cdb->cdb_status &= ~CDB_EOIR_RESET;	/* Clear it now */
	}

#else
	if (cdb && cdb->reset)
	    (*cdb->reset)(cdb);
#endif
    }
  
    FOR_ALL_HWIDBS(hwidb) {

#ifdef C7000
	boolean board_reloaded;
	boolean board_eoir;

	if (hwidb->status & IDB_NOHARDWARE)
	    continue;

	if (!touch_device(pslot_to_dbus(hwidb->slot)))
	    continue;

	/*
	 * If the card was recently reinstalled, skip this interface
	 */
	if (hwidb->pre_hotstall_state != IDBS_ADMINDOWN)
	    continue;
#endif

	TIMER_STOP(hwidb->xmittimer);

#ifdef C7000
	/*
	 * Don't EOIR this interface if the slot was (re)loaded!
	 */
	board_reloaded = slots[hwidb->slot].flags & BOARD_RELOADED;
	board_eoir = eoir && (!board_reloaded);

	if (board_eoir)
	    hwidb->oir_flags |= EOIR_RESET;

	if (cbus_debug)
	    buginf("\nResetting %s (flags %x), eoir=%d, board_reloaded=%d, board_eoir=%d",
		   hwidb->hw_namestring, hwidb->oir_flags, eoir,
		   board_reloaded, board_eoir);

	/*
	 * Is the current card a MIP? If so, we have to do yet another
	 * ugly 'except for the MIP' kludge.
	 * 
	 * 
	 * The MIP interface has been reset when its controller
	 * was reset early. So when the controller is unquiesced,
	 * the interface can be ready for serving packets. This
	 * only applies to an EIORing MIP.
	 */
	if ((!board_eoir) || 
	    (slots[hwidb->slot].ctrlr_type != FCI_MIP_CONTROLLER)) {

	    hwidb->reset(hwidb);

	    if (cbus_debug)
	        buginf("\nReset %s, state %s",
		       hwidb->hw_namestring, print_hwidbstate(hwidb));
	}
	/*
	 * For VIP, get the card to send up configuration
	 */
	if (vip_controller(slots[hwidb->slot].ctrlr_type)) {
	    send_vip_request_config_cmd(hwidb);
	}
#else
	hwidb->reset(hwidb);
#endif
	hwidb->oir_flags &= ~EOIR_RESET;
	if (hwidb->counters.resets > 0)
	    hwidb->counters.resets--;
      }

#ifdef C7000
    /* Clear all BOARD_RELOADED flags */
    {
	slottype* sp;

	FOR_ALL_ENABLED_SLOTS(sp) {
	    sp->flags &= ~BOARD_RELOADED;
	}
    }
#endif

      cbus->resetting = FALSE;
}

/*
 * fci_daughter_panic
 * When an IP panics, and we don't know the IDB, print out debugging
 * information.
 * See also interface_panic(), which is used when we do know the IDB.
 */

static void fci_daughter_panic (mcitype *cbus, const char *namestring)
{
    mciregtype *regptr;
    ushort type, vals[16], i;
    char *str;

    regptr = cbus->regptr;
    type = cbus_errors(regptr, 0x8000);
    while (type) {
	for (i = 0; i < 16; i++)
	    vals[i] = 0xffff;
	if (type & 1) {			/* bad rxeof */
	    type &= ~1;
	    str = "bad rxeof vec";
	    vals[0] = cbus_errors(regptr, 0x8001);
	    vals[1] = cbus_errors(regptr, 0x8002);
	    vals[2] = cbus_errors(regptr, 0x8003);
	    vals[3] = cbus_errors(regptr, 0x8004);
	} else if (type & 2) {		/* bad tx state */
	    type &= ~2;
	    str = "bad tx state";
	    vals[0] = cbus_errors(regptr, 0x8005);
	} else if (type & 4) {		/* bad txEof */
	    type &= ~4;
	    str = "bad txEof vec";
	    vals[0] = cbus_errors(regptr, 0x8006);
	    vals[1] = cbus_errors(regptr, 0x8007);
	} else if (type & 8) {		/* tx_start/pend active */
	    type &= ~8;
	    str = "tx_start/pend active";
	    vals[0] = cbus_errors(regptr, 0x8008);
	    vals[1] = cbus_errors(regptr, 0x8009);
	    vals[2] = cbus_errors(regptr, 0x800a);
	} else {
	    type = 0;
	    str = "unknown";
	}
	buginf("\n%s ustatus: %s (%04x), %04x %04x %04x %04x", namestring,
	       str, type, vals[0], vals[1], vals[2], vals[3]);
    }
}

/*
 * show_cbus
 * Display current statistics for CBUS controllers.
 */
void show_cbus (parseinfo *csb)
{
    mcitype *cbus;
    cdbtype *cdb;
    hwidbtype *idb;
    ushort slot;
    int slotunit, i;
    dcitype *dci;
    mciregtype *regptr;
    leveltype level;
    ulong lword[2];
    ushort word;
    ushort iftype, rxqcount, txqhead, txqtail, txqcount;
    boolean snoop_memd = TRUE;
    
    if (GETOBJ(int, 1)) {
 	show_cbus_lowcore();
 	return;
    }
    
    if (GETOBJ(int, 2)) {
 	show_cbus_lowcore_current();
 	return;
    }
 
    word = iftype = rxqcount = txqhead = txqtail = txqcount = 0;
    automore_enable(NULL);

    /*
     * Scan for all cBus controllers -- really only one (1).
     */
    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	reg_invoke_print_cbus_controller(cbus);
	printf("\n  %d Kbytes of main memory, %d Kbytes cache memory",
	       (cbus->memd_size+1)/512,  (cbus->mema_size+1)/512);
	for (i = 0; i < cbus->if_sizes_count; i++) {
	    printf((i ? ", " : "\n  "));
	    printf("%d %d byte buffers", cbus->buffers[i], cbus->if_sizes[i]);
	}
	if (cbus->system_buffer_size > 0) {
	    printf(((cbus->if_sizes_count > 2) ? "\n  " : ", "));
	    printf("%d byte system buffer", cbus->system_buffer_size);
	}
	printf("\n  Restarts: %d line down, %d hung output, %d controller error",
	       cbus_restarts_lineup, cbus_restarts_output,
	       cbus_restarts_error);
	automore_conditional(0);
	
	/*
	 * Scan for all daughter controllers.
	 */
	for (slot = 0; slot < fci_max_dci_ctrlrs; slot++) {

	    dci = cbus->dci[slot];
	    if (dci == NULL)
		continue;
#ifdef C7000
	    /*
	     * Skip cards that have been removed
	     */
	    if ( (dci->ctrlr_type == FCI_UNKNOWN_CONTROLLER) ||
		(slots[dci->ctrlr_unit].flags &
		 (BOARD_NONEXIST | BOARD_INVALID) ) )
		continue;

	    if (dci->ctrlr_type == FCI_ETHER_CONTROLLER)
		printf("\n EIP");
	    if (dci->ctrlr_type == FCI_FEIP_CONTROLLER)
		printf("\n FEIP");
	    if (dci->ctrlr_type == FCI_FDDIT_CONTROLLER) {
		if (srswitch_pslot(dci->ctrlr_unit))
		    printf("\n SRS FIP");
		else
		    printf("\n FIP");
	    }
	    if (dci->ctrlr_type == FCI_LINK_CONTROLLER)
		printf("\n HIP");
	    if (dci->ctrlr_type == FCI_FSIP_CONTROLLER) {
		if (ssip_smip_pslot(dci->ctrlr_unit))
		    printf("\n SSIP");
		else
		    printf("\n FSIP");
	    }
	    if (dci->ctrlr_type == FCI_CTR_CONTROLLER) {
		if (srswitch_pslot(dci->ctrlr_unit))
		    printf("\n SRS TRIP");
		else
		    printf("\n TRIP");
	    }
	    if (dci->ctrlr_type == FCI_AIP_CONTROLLER)
		printf("\n AIP");
	    if (dci->ctrlr_type == FCI_MIP_CONTROLLER) {
		if (ssip_smip_pslot(dci->ctrlr_unit))
		    printf("\n SMIP");
		else
		    printf("\n MIP");
	    }
	    if (dci->ctrlr_type == FCI_CIP_CONTROLLER) {
		if (is_cip2_hw(dci->ctrlr_unit))
		    printf("\n CIP2");
		else
		    printf("\n CIP");
	    }
	    if (dci->ctrlr_type == FCI_RVIP_CONTROLLER)
		printf("\n VIP");
	    printf(" %d, hardware version %d.%d, microcode version %d.%d",
		   dci->ctrlr_unit,
		   (dci->hw_version >> 8), (dci->hw_version & 0xFF),
		   (dci->sw_version >> 8), (dci->sw_version & 0xFF));
	    printf("\n  Microcode loaded from %s %s", 
		   get_ucode_src(dci->ctrlr_unit), 
		   get_ucode_src_file(dci->ctrlr_unit));
#else
	    if (dci->ctrlr_type == FCI_ETHER_CONTROLLER)
		printf("\n MEC");
	    if (dci->ctrlr_type == FCI_FDDI_CONTROLLER)
		printf("\n FDDI");
	    if (dci->ctrlr_type == FCI_FDDIT_CONTROLLER)
		printf("\n FDDI-T");
	    if (dci->ctrlr_type == FCI_LINK_CONTROLLER)
		printf("\n HSCI");
	    if (dci->ctrlr_type == FCI_CTR_CONTROLLER)
		printf("\n CTR");
	    if (dci->ctrlr_type == FCI_AIP_CONTROLLER)
		printf("\n AIP");
	    if (dci->ctrlr_type == FCI_MIP_CONTROLLER)
		printf("\n MIP");
	    printf(" %d, controller type %d.%d, microcode version %d.%d",
		   dci->ctrlr_unit,
		   (dci->hw_version >> 8), (dci->hw_version & 0xFF),
		   (dci->sw_version >> 8), (dci->sw_version & 0xFF));
#endif
	    /* 
	     * Display controller resync/recovery statistics
	     * if the controller supports timeout recovery, or should
	     */ 
	    if (dci->resyncable || dci->ctrlr_sync_calls)
		printf("\n  Controller Sync: %d timeouts, %d resyncs " 
		       "%d failures, %d max phase count",
		       dci->ctrlr_sync_calls, dci->ctrlr_sync_times,
		       dci->ctrlr_sync_fails, dci->ctrlr_sync_phase);

	    reg_invoke_show_controller_specific(dci->ctrlr_type, slot);
	    /*
	     * Scan for all interface(s) on this daughter controller.
	     * N.B.: This code assumes that interfaces start at the
	     *       lowest possible number per controller.
	     * Use cdb when applicable - otherwise use idb.
	     */
	    for (slotunit = 0; slotunit < MAX_CTRLR_INTERFACES; slotunit++) {
		cdb = controller_find(slot, slotunit);
		idb = dci->interface_list[slotunit];
		if (cdb == NULL || idb == NULL)
		    continue;

                if (slotunit == 0 || slotunit == PA_VIRTUAL_SLOT_BOUNDARY)
                    reg_invoke_show_pa_specific(idb->type, idb,
                                                        dci->ctrlr_type);

#ifdef C7000
		/*
		 * We have been very careful to make the idb and the
		 * cdb information the same here.  Also the idb info
		 * used here should be the same for each idb associated
		 * with the cdb.
		 */
		printf("\n  Interface %d - %s",cdb->unit,cdb->cdb_namestring);
#else
		/*
		 * To maintain backwards compatibility, we are going to
		 * print out the interface name instead of the controller
		 * name on the AGS.  This will break if we ever get a
		 * cbus controller with multiple interfaces on the A chassis,
		 * but that will almost certainly never happen, so it's
		 * not worth the effort to be clever.
		 */
		printf("\n  Interface %d - %s",
		       idb->mci_index, idb->hw_namestring);
#endif
		if (idb->status & IDB_IEEEMASK)
		    printf(", address %e (bia %e)",
			   idb->hardware, idb->bia);
		if (idb->status & (IDB_SERIAL))
		    printf(", electrical interface is %s",
			   idb->applique_name);
		if (idb->status & IDB_ATM) {
		    switch(idb->atm_db->plimtype) {
		    default:
		    case ATM_PLIM_INVALID:
			printf(", PLIM is INVALID");
			break;
		    case ATM_PLIM_4B5B:
			printf(", PLIM is 4B5B");
			break;
		    case ATM_PLIM_SONET:
			printf(", PLIM is SONET");
			break;
		    case ATM_PLIM_E3:
			printf(", PLIM is E3");
			break;
		    case ATM_PLIM_DS3:
			printf(", PLIM is DS3");
			break;
		    }
		    printf("(%dMbps)", idb->atm_db->plimrate);
		}
		printf("\n    %d buffer RX queue threshold, %d buffer TX queue limit",
		       cbus->rxbuffers[cdb->unit],
		       cdb->mci_txcount);
		printf(", buffer size %d", cbus->if_sizes[cdb->buffer_pool]);
#ifdef C7000
		/* This is ugly, but allows txqlength for all cases
		 * but the MIP here.
		 * The mip is excluded because it has more than 1 idb
		 * per cdb.  We'll display it in show interface
		 */
		if (dci->ctrlr_type != FCI_MIP_CONTROLLER)
#endif
		    if (idb->txqlength)
			printf("\n    TX queue length is %d", 
			       (*idb->txqlength)(idb));
		level = raise_interrupt_level(NETS_DISABLE);
		regptr = idb->mci_regptr;
		fci_select_if(idb, regptr);
		regptr->argreg = FCI_ICB_IFTYPE;
		regptr->cmdreg = FCI_CMD_READ_ICB;
		if (regptr->cmdreg == FCI_RSP_OKAY) {
		    snoop_memd = FALSE;
		    iftype = regptr->argreg;
		    regptr->argreg = FCI_ICB_RXQCOUNT;
		    regptr->cmdreg = FCI_CMD_READ_ICB;
		    rxqcount = regptr->argreg;
		    regptr->argreg = FCI_ICB_TXQHEAD;
		    regptr->cmdreg = FCI_CMD_READ_ICB;
		    txqhead = regptr->argreg;
		    regptr->argreg = FCI_ICB_TXQTAIL;
		    regptr->cmdreg = FCI_CMD_READ_ICB;
		    txqtail = regptr->argreg;
		    regptr->argreg = FCI_ICB_TXQCOUNT;
		    regptr->cmdreg = FCI_CMD_READ_ICB;
		    txqcount = regptr->argreg;
		} else {
		    regptr->argreg = idb->cbus_icb_address;
		    regptr->cmdreg = FCI_CMD_MEMD_SELECT;
		    lword[0] = regptr->readlong;
		    lword[1] = regptr->readlong;
		    word = regptr->readshort;
		}
		reset_interrupt_level(level);

		if (snoop_memd)
		    printf("\n    ift %-04x, rql %d, tq %-04x %-04x, tql %d",
			   (lword[0] >> 16), (ushort) lword[0], 
			   (lword[1] >> 16), (ushort) lword[1],
			   word);
		else
		    printf("\n    ift %-04x, rql %d, tq %-04x %-04x, tql %d",
			   iftype, rxqcount, txqhead, txqtail, txqcount);
		if (!(idb->status & IDB_FDDI)) {
		    /*
		     * If there's more than 1 idb associated with the
		     * cdb, see if there is a common transmitter delay,
		     * or if is different for any of the idb's
		     * Otherwise, just display the transmit delay
		     */
		    if (idb->next_idb_on_physint) {
			boolean havediff = FALSE;
			int delayvalue = idb->xmitdelay;
						/*
			 * Search through all idbs associated with the cdb.
			 * See if any have transmitter delays
			 */
			while (idb != NULL) {
			    if (idb->xmitdelay != delayvalue) {
				havediff = TRUE;
				break;
			    }
			    idb = idb->next_idb_on_physint;
			}
			if (havediff)
			    printf("\n    Transmitter delay varies on an interface basis");
			else
			    printf("\n    Transmitter delay on all interfaces is %d flags",
				   delayvalue);
		    } else if ((idb->status & IDB_HSSI) ||
			       (idb->type == IDBTYPE_CBUSSERIAL)) 
			printf("\n    Transmitter delay is %d flags",
			       idb->xmitdelay);
		    else 
			printf("\n    Transmitter delay is %d microseconds",
			       idb->xmitdelay);
		}
	    }
	    if (cbus->next) {
		automore_conditional(0);
	    }
	}
    }
    automore_disable();
}

/*
 * cbus-buffers
 * Various commands for manipulating cbus buffers.
 * Synopsis:
 *           cbus-buffers maximum
 * Limit the number of buffers carved up on the cbus controller.
 *
 *           cbus-buffers burst-count    (HSCI only)
 * Park this many buffers under the link controller.
 */
void cbus_buffers_command (parseinfo *csb)
{
    hwidbtype *idb;
    mcitype *cbus;
    leveltype level;
    int burst, nbuf;

    idb = csb->interface->hwptr;
    cbus = idb->devctl;

    if (csb->nvgen) {
	if (!(idb->status & IDB_CBUS)) {
	    return;
	}
	switch (csb->which) {
	  case CBUS_BUFFERS_MAXIMUM:
	    nv_write(idb->max_buffers, "%s %d", csb->nv_command,
		     idb->max_buffers);
	    break;
	  case CBUS_BUFFERS_BURSTCOUNT:
	    nv_write(((idb->status & (IDB_SERIAL | IDB_TR)) &&
		      (idb->cbus_burst_count != idb->cbus_burst_count_def)),
		     "%s %d", csb->nv_command, idb->cbus_burst_count);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    if (!(idb->status & IDB_CBUS)) {
	printf("\n%%Interface %s not a cbus device.", idb->hw_namestring);
	return;
    }

    switch (csb->which) {
      case CBUS_BUFFERS_MAXIMUM:
	if (GETOBJ(int,1) == idb->max_buffers)
	    return;   /* No change, silently return */
	if (csb->sense) {
	    if (GETOBJ(int,1) < idb->cbus_burst_count) {
		printf("\n%%Maximum must not be less than burst count "
		       "-- adjusting burst count.");
		idb->cbus_burst_count = GETOBJ(int,1);
	    }
	}
	if (idb->status & (IDB_HSSI)) {
	    idb->cbus_burst_count_def = GETOBJ(int,1) < HSCI_BURST_COUNT_DEF ?
		GETOBJ(int,1) : HSCI_BURST_COUNT_DEF;
	} else if (idb->type == IDBTYPE_CTR) {
	    idb->cbus_burst_count_def = GETOBJ(int,1) < CTR_BURST_COUNT_DEF ?
		GETOBJ(int,1) : CTR_BURST_COUNT_DEF;
	}
	idb->max_buffers = GETOBJ(int,1);
	level = raise_interrupt_level(NETS_DISABLE);
	cbus_init(cbus, FALSE); /* stomp on cbus complex */
	reset_interrupt_level(level);
	return;
      case CBUS_BUFFERS_BURSTCOUNT:
	if (!(idb->status & (IDB_HSSI | IDB_TR))) {
	    printf("\n%%Burst-count not supported for interface %s",
		   idb->hw_namestring);
	    return;
	}
	nbuf = cbus->buffers[idb->buffer_pool];
	if (csb->sense) {
	    if (GETOBJ(int,1) > (nbuf - 1)) {
		printf("\n%%Burst count can not be greater than maximum %d.",
		       nbuf-1);
		return;
	    }
	    burst = GETOBJ(int,1);
	} else
	    burst = (idb->cbus_burst_count_def > nbuf) ?
		nbuf : idb->cbus_burst_count_def;
	if (burst == idb->cbus_burst_count)
	    return;    /* No change, silently return */

	idb->cbus_burst_count = burst;
	(*idb->reset)(idb);
	if (idb->counters.resets > 0)
	    idb->counters.resets--;
    }
}

/*
 * cbus_cache_init
 * Generic cbus cache initialization -- if the microcode supports it.
 */
void cbus_cache_init (void)
{
    mcitype *cbus;
    mciregtype *regptr;
    leveltype level;

    /*
     * Init some global variables:
     *
     * cbus_switching_supported contains the cbus uCode capability flags
     * and should be treated as read-only by protocol-specific code.
     *
     * cbus_cache_allocated is intended to represent the total cache
     * usage by all protocols. As such, it should be updated when
     * protocol-specific routines allocate cache memory.
     */
    cbus_switching_supported = 0;
    cbus_cache_allocated = 0;

    /*
     * If the cbus is present -- invalidate all cache entries.
     */
    if ((cbus = (mcitype *) cbusQ.qhead)) {
#ifndef C7000
        /*
         * FCI_CACHE_INVALIDATE will cause wild things to happen on the
	 * cbusI 1.0 ucode. Skip over further initialization. Once
	 * cbus_switching_supported gets reset other FCI_CACHE commands
	 * will not be issued and theres no more problem. 1.0 cbus code
	 * should be obsolete, but... 
         */
	if (cbus->sw_version >= 0x0200) {
#endif
	    regptr = cbus->regptr;
	    regptr->cmdreg = FCI_AS_CAPABILITY;
	    if (regptr->cmdreg == FCI_RSP_OKAY)
		cbus_switching_supported = regptr->argreg;
	    else
		cbus_switching_supported = FCI_CAP_IP_AUTO;

	    /*
	     * Initialize per protocol as appropriate prior to invalidation:
	     */
	    reg_invoke_auto_sw_cache_preinit_callback();

	    level = raise_interrupt_level(NETS_DISABLE);
	    regptr->argreg = 0x8000;	/* Invalidates the entire cache */
	    regptr->cmdreg = FCI_CACHE_INVALIDATE;
	    DELAY(1);			/* delay one (1) millisecond */
	    if (regptr->cmdreg != FCI_RSP_OKAY) 
		cbus_switching_supported = 0;
	    reset_interrupt_level(level);
#ifndef C7000
	}
#endif
	/*
	 * Initialize per protocol as appropriate following invalidation:
	 */
	reg_invoke_auto_sw_cache_init_callback();
    }
}	

int cbus_slotunit2unit (int slot, int slotunit)
{
    if (slot < 0 || slot >= FCI_MAX_DCI_CTRLRS)
	return -1;

    if (slotunit < 0 || slotunit >= MAX_CTRLR_INTERFACES)
	return -1;

    if (slotunit < PA_MAX_INTERFACES) {
	return slot * PA_MAX_INTERFACES + slotunit;
    } else {
	return (slot * PA_MAX_INTERFACES) + 
	       (slotunit - PA_MAX_INTERFACES) + 
	       CBUS2_NON_EXT_INTERFACES; 
    }
}

boolean cbus_unit2slotunit (int unit, int *slot, int *slotunit)
{
    if (unit < 0 || unit >= MAX_CBUS_INTERFACES)
	return FALSE;

    if (unit < CBUS2_NON_EXT_INTERFACES) {
	*slot     = unit / PA_MAX_INTERFACES;
	*slotunit = unit % PA_MAX_INTERFACES;
    } else {
	/* 
         * extended icbs, ags should never be here
	 */
	unit -= CBUS2_NON_EXT_INTERFACES;
	*slot     =  unit / PA_MAX_INTERFACES;
	*slotunit = (unit % PA_MAX_INTERFACES) + PA_MAX_INTERFACES; 
    }
    return TRUE;
}


/* process a love note for normal cbus devices */
void default_love_note_handler (cdbtype *cdb, ushort status)
{
    hwidbtype *idb;
    ushort stnID;
    platform_hware_handle_type *hware_handle;

    hware_handle = cdb->platform_hware_handle;
    
    idb = hware_handle->interface_list[cdb->unit];
    if (idb == NULL) {
	/* this must be a VC controller, with no interfaces */
	if (status & FCI_COND_PANIC)
	    fci_daughter_panic(hware_handle, cdb->cdb_namestring);
	return;
    }
    do {
	/*
	 * Unconditionally see if traffic waiting.
	 */
	(*idb->soutput)(idb);

	/*
	 *  Check other special conditions
	 */
        stnID = idb->fddi_stn_id;
	if (status & ~FCI_COND_MASK) {
	    if (cbus_debug) {
		buginf("\n%s: love note status %#x", idb->hw_namestring,
		       status);
	    }
	    if (status & FCI_COND_DCD) {
		if (idb->status & IDB_FDDI)
		    fddi_carrier(idb);
		else
		    cbus_mci_carrier(idb);
	    }
	    if (idb->status & IDB_FDDI) {
		fddi_smt_love_note_handler(idb, status);
	    }
	    if (status & FCI_COND_PANIC)
		interface_panic(idb, idb->hw_namestring);
	    if ((status & FCI_COND_LOOP) && (idb->status & IDB_HSSI))
		hssi_loop_request(idb);
	    if (idb->type == IDBTYPE_CTR) { 
		    ctr_conditions(idb, status);
	    }
	}
	idb = idb->next_idb_on_physint;
    } while (idb != NULL);
}

/*
 * interface-specific platform hooks:
 * This is a nop except for VIP style interfaces on a 7000.
 */
void eip_platform_init (subsystype* subsys)
{
#if defined(C7000)
    /*
     * Add a registry hook to suck in statistics love letters
     * from a VIP style ethernet (only applies when being compiled for
     * the 7000, does not apply to the AGS)
     */
    reg_add_ether_sniff_encap(IDBTYPE_CBUSETHER, vip_sniff_encap,
			      "vip_sniff_encap");
#endif
}

void fip_platform_init (subsystype* subsys)
{
    return;
}

/*****************************************************************************
* Name: get_cbus_ctrlr_errors
 *
 * Description: Accumulates cBus interface errors kept by the SP.
 *   Called by the get_cbus_errors.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void get_cbus_ctrlr_errors (hwidbtype  *hwidb)
{
    mcitype *mci;
    mciregtype *regptr;
    long counts;
    
    regptr = hwidb->mci_regptr;
    
    /*
     * Select the interface, if not running at interrupt level.
     */
    regptr->argreg = ((FCI_IP_RX_PKTS << 8) | hwidb->mci_index);
    regptr->cmdreg = FCI_CMD_STATISTICS;
    if (regptr->cmdreg == FCI_RSP_OKAY) {
	counts = regptr->largreg;
	hwidb->counters.inputs_auto[ACCT_PROTO_IP] += counts;
	cbus_ipcache_hits += counts;
	ip_traffic.inputs += counts;
	regptr->argreg = ((FCI_IP_RX_BYTES << 8) | hwidb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	hwidb->counters.rx_cumbytes_auto[ACCT_PROTO_IP] += regptr->largreg;
	regptr->argreg = ((FCI_IP_TX_PKTS << 8) | hwidb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	counts = regptr->largreg;
	hwidb->counters.outputs_auto[ACCT_PROTO_IP] += counts;
	ip_traffic.forwarded += counts;
	regptr->argreg = ((FCI_IP_TX_BYTES << 8) | hwidb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	hwidb->counters.tx_cumbytes_auto[ACCT_PROTO_IP] += regptr->largreg;
	regptr->argreg = ((FCI_IP_TX_DROPS << 8) | hwidb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	hwidb->counters.output_total_drops += regptr->largreg;
	
	mci = hwidb->devctl;
	if (mci->flags & CBUSII) {
	    regptr->argreg = ((FCI_IP_CKS_ERRS << 8) | hwidb->mci_index);
	    regptr->cmdreg = FCI_CMD_STATISTICS;
	    ip_traffic.checksumerr += regptr->largreg;
	    
	    /*
	     * This was created to hide some IPX data structures.
	     * The next cleanup crew through here should stuff
	     * other protocol specific code into this service point.
	     */
	    reg_invoke_cbus_get_statistics(hwidb);
	    if ((hwidb->status & IDB_TR) || (hwidb->status & IDB_FDDI)) {
                srb_vector_table_t *srbV_p;
		regptr->argreg = ((FCI_SRB_RX_PKTS << 8) | hwidb->mci_index);
		regptr->cmdreg = FCI_CMD_STATISTICS;
		counts = regptr->largreg;
		hwidb->counters.inputs_auto[ACCT_PROTO_SRB] += counts;
		regptr->argreg = ((FCI_SRB_RX_BYTES << 8) | hwidb->mci_index);
		regptr->cmdreg = FCI_CMD_STATISTICS;
		hwidb->counters.rx_cumbytes_auto[ACCT_PROTO_SRB] += regptr->largreg;
		regptr->argreg = ((FCI_SRB_TX_PKTS << 8) | hwidb->mci_index);
		regptr->cmdreg = FCI_CMD_STATISTICS;
		hwidb->counters.outputs_auto[ACCT_PROTO_SRB] += regptr->largreg;
		regptr->argreg = ((FCI_SRB_TX_BYTES << 8) | hwidb->mci_index);
		regptr->cmdreg = FCI_CMD_STATISTICS;
		hwidb->counters.tx_cumbytes_auto[ACCT_PROTO_SRB] += regptr->largreg;
		regptr->argreg = ((FCI_SRB_TX_DROPS << 8) | hwidb->mci_index);
		regptr->cmdreg = FCI_CMD_STATISTICS;
                srbV_p = reg_invoke_srb_get_vector_table();
                if (srbV_p) {
                    srbV_p->srb_sb_multi_incr_tx_drops(hwidb->firstsw, regptr->largreg);
                }
	    }
	    /*
	     * Poll the cBus-maintained autonomous bridging statistics.
	     * Note that the rx/tx packet counts are maintained separately
	     * by span_process which polls them out of the bridge cache
	     * and accumulates them into the interface statistics.
	     */
	    if (hwidb->cbus_bridge_enable) {
		regptr->argreg = ((FCI_AB_RX_BYTES << 8) | hwidb->mci_index);
		regptr->cmdreg = FCI_CMD_STATISTICS;
		hwidb->counters.rx_cumbytes_auto[ACCT_PROTO_BRIDGE] += regptr->largreg;
		
		regptr->argreg = ((FCI_AB_TX_BYTES << 8) | hwidb->mci_index);
		regptr->cmdreg = FCI_CMD_STATISTICS;
		hwidb->counters.tx_cumbytes_auto[ACCT_PROTO_BRIDGE] += regptr->largreg;
		
		regptr->argreg = ((FCI_AB_TX_DROPS << 8) | hwidb->mci_index);
		regptr->cmdreg = FCI_CMD_STATISTICS;
		counts = regptr->argreg;
		hwidb->counters.output_drops[ACCT_PROTO_BRIDGE] += counts;
		hwidb->counters.output_total_drops += counts;
	    }
	}
    }
}


/*****************************************************************************
 * Name: platform_idb_init
 *
 * Description: Platform-specific IDB initialization for c7000
 *   This function is called from common_idb_init_first() to do all the IDB
 *   initialization that is peculiar to the ags, c7000.
 *   Interface type specific initialization should go in other
 *   functions -- not here.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *****************************************************************************/
void platform_idb_init (hwidbtype *hwidb)
{
    hwidb->state = IDBS_DOWN;  /* transplant from common-idb-init()  */

    hwidb->enable = cbus_mci_enable;
    hwidb->soutput = cbus_mci_output;
    hwidb->setup_bridging = cbus_mci_rxtypes_update;

    hwidb->name = NULL;		/* make sure proper name is computed */

    return;
}

/*
 * ip_on_cbus2
 * Return true if the FDDI IP is in a CBUS II system,
 * or is in a CyBus system.
 */

boolean ip_on_cbus2 (hwidbtype *hwidb)
{
    mcitype *cbus;

    cbus = hwidb->devctl;
    if (cbus->flags & CBUSII)
	return(TRUE);
    else
	return(FALSE);
}

/*
 * platform_req_interface_restart
 * Platform specific routine to request a reset of only the
 * affected interface for an attempt at error recovery. A background
 * process honors the request at a later time.
 */

void platform_req_interface_restart (hwidbtype *hwidb)
{
    mcitype *cbus;

    /*
     * On the 7000, just use restartdeferred as before:
     */
    cbus = hwidb->devctl;
    cbus->restartdeferred = TRUE;
}

/*
 * platform_req_card_restart
 * Platform specific routine to request a reset of only the card/slot
 * which contains the affected interface for an attempt at error recovery.
 * A background process honors the request at a later time:
 */

void platform_req_card_restart (hwidbtype *hwidb)
{
    mcitype *cbus;

    /*
     * On the 7000, just use restartdeferred as before:
     */
    cbus = hwidb->devctl;
    cbus->restartdeferred = TRUE;
}

/*
 * platform_req_complex_restart
 * Platform specific routine to request a reset of the entire switching
 * complex which contains the affected interface in an attempt at error
 * recovery.  A background process honors the request at a later time.
 */

void platform_req_complex_restart (hwidbtype *hwidb)
{
    mcitype *cbus;

    /*
     * On the 7000, just use restartdeferred as before:
     */
    cbus = hwidb->devctl;
    cbus->restartdeferred = TRUE;
}

/*
 * platform_demand_complex_restart
 * Initialize a restart of the switching complex NOW:
 */

void platform_demand_complex_restart (hwidbtype *hwidb)
{
    mcitype	*mci;
    boolean	critical;
    leveltype	level;

    /*
     * If we're already in the middle of a hotswap event, let it alone
     */
    if (hotstall_flag)
	return;

    if (hwidb == NULL) 
	/*
	 * the hwidb==NULL call is only used for cbus interfaces
	 */
	mci = (mcitype*)cbusQ.qhead;
    else
	mci = hwidb->devctl;
    
    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * This routine replaces code which variously checks either of
     * these two cases for criticalness. This should be safer to check
     * them both:
     */
    critical = FALSE;
    if (mci->critical)
	critical = TRUE;
    else if (onintstack())
	critical = TRUE;
    mci_init(mci, critical);

    reset_interrupt_level(level);
}

/*
 * Platform specific routine to turn off an LED on an interface
 * in the next LED state variable:
 */

void platform_light_off (hwidbtype *hwidb)
{
    mcitype *mci;

    mci = hwidb->devctl;
    mci->lights &= ~mci->led_mask[hwidb->mci_index];
}

/*
 * Platform specific routine to turn on an LED on an interface
 * in the next LED state variable:
 */

void platform_light_on (hwidbtype *hwidb)
{
    mcitype *mci;

    mci = hwidb->devctl;
    mci->lights |= mci->led_carrier[hwidb->mci_index];
}

/*
 * Platform specific routine to update the interface LEDs from
 * the  next LED state variable:
 */

void platform_update_lights (hwidbtype *hwidb)
{
    mcitype *mci;

    mci = hwidb->devctl;
    send_lights_cmd(hwidb, mci->lights);
}

/*
 * platform_cbus_reset_in_progress
 *
 * Platform specific routine to determine whether or not we're in
 * the middle of resetting the cbus.  On the c7000, this is indicated
 * by the state of the resetting flag in the cbus structure.
 */

boolean platform_cbus_reset_in_progress (hwidbtype *hwidb)
{
    mcitype *mci;

    mci = hwidb->devctl;
    return(mci->resetting);
}

void platform_show_switch_counts (hwidbtype *hwidb)
{
    idb_si_counters_t	*sic;

    sic = idb_get_counter_info(hwidb, IDB_COUNTER_SI_CNT_PTR);
    printf("\n%-24s %-10lu %-10lu %-10lu %-10lu",
	   "Autonomous/SSE",
	   idb_auto_in(hwidb) - sic->auto_in,
	   idb_rx_auto_cumbytes(hwidb) - sic->rx_auto_cumbytes,
	   idb_auto_out(hwidb) - sic->auto_out,
	   idb_tx_auto_cumbytes(hwidb) - sic->tx_auto_cumbytes);
}
/*****************************************************************************
 * Name: trip_cbus_mci_txlimit
 *
 * Description: workaround for CSCdi55758
 *   TRIP on c7000 will steal memd as tql > txl. This is not a problem on 
 *   series 7500.
 *
 * Return Value: None
 *
 *****************************************************************************/

void trip_cbus_mci_txlimit(hwidbtype *trip_hwidb)
{
     /* CSCdi55758
      * Don't call "cbus_mci_txlimit()" on c7000 platform.
      * Do not allow txlimit to change on reset of trip (non memd carve)
      * until c7000 architecture correctly modifies tql (like 7500 series).
      */
      return;
}
