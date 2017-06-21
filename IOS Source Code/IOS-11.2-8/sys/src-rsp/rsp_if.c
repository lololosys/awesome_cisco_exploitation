/* $Id: rsp_if.c,v 3.29.4.58 1996/09/10 11:10:32 kao Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_if.c,v $
 *------------------------------------------------------------------
 * Network interface device driver level and
 * decision layer for the IRSP/1
 *
 * March 1994, Chris Shaker
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_if.c,v $
 * Revision 3.29.4.58  1996/09/10  11:10:32  kao
 * CSCdi54108:  Optimum and Flow switching performance degradation
 * move one global variable to fix a rsp-boot-mz build failure
 * Branch: California_branch
 *
 * Revision 3.29.4.57  1996/09/10  02:23:38  dtaubert
 * CSCdi68381:  fs_pak referenced in debug code before stuff_fs_pak in
 * rsp_aip_fs
 * Branch: California_branch
 *
 * Revision 3.29.4.56  1996/09/07  02:18:01  kao
 * CSCdi54108:  Optimum and Flow switching performance degradation
 * Performance degraded with more and more new supported features
 * This fix actually optimizes several places in various routines to
 * increase the optimum/flow switching performance for RSP
 * Branch: California_branch
 *
 * Revision 3.29.4.55  1996/08/28  13:13:58  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.29.4.54  1996/08/26  15:11:47  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.29.4.53  1996/08/24  05:00:35  fred
 * CSCdi67070:  remove upper bound from red queues
 * Branch: California_branch
 *
 * Revision 3.29.4.52  1996/08/24  00:22:28  shaker
 * CSCdi67002:  Peek at MEMD via uncached address space when displaying
 * packets
 * Branch: California_branch
 * Implement code review comment.
 *
 * Revision 3.29.4.51  1996/08/23  21:43:59  shaker
 * CSCdi67002:  Peek at MEMD via uncached address space when displaying
 * packets
 * Branch: California_branch
 * Access memd uncached for packet dumping in error logging.
 *
 * Revision 3.29.4.50  1996/08/21  11:56:21  shaker
 * CSCdi66673:  Reserved Exception / QAERROR caused by ethernet runts
 * Branch: California_branch
 * Use retbufhdr_inline() instead of rsp_fs_free_memd_pak() when
 * dropping runt ethernet packets, because the fs_pak structure is
 * not yet filled out at this point in the code path.
 *
 * Revision 3.29.4.49  1996/08/13  02:24:20  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.29.4.48  1996/08/11  23:34:14  ppearce
 * CSCdi61195:  RSRB performance degraded 40% from 11.1(1)
 * Branch: California_branch
 *   Erroneous RSRB debug processing -- missing check for debug enabled
 *
 * Revision 3.29.4.47  1996/07/31  19:26:10  awu
 * CSCdi61629:  %SYS-2-INPUTQ: INPUTQ set, but no idb
 * Branch: California_branch
 *  -set if_input properly
 *  -use right iqueue routine to hand in a pak to process level
 *
 * Revision 3.29.4.46  1996/07/30  21:10:35  rramacha
 * CSCdi57573:  Serial interface stays line down when remote side toggles
 * Branch: California_branch
 * o Add debug support
 *
 * Revision 3.29.4.45  1996/07/25  23:53:46  motto
 * CSCdi46834:  SRB explorer counters are incr. on CIP I/F for non CIP
 * destined expl
 * Branch: California_branch
 *
 * Revision 3.29.4.44  1996/07/23  18:48:26  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.29.4.43  1996/07/22  15:24:06  psandova
 * CSCdi59090:  Non-explorer packets are being flushed with explorers
 * Branch: California_branch
 * Fix the bug-fix for the rsp ... stupid mistake.
 *
 * Revision 3.29.4.42  1996/07/22  01:33:31  psandova
 * CSCdi59090:  Non-explorer packets are being flushed with explorers
 * Branch: California_branch
 * Valid multicast packets that should be handed to the protocol stack
 * are instead being diverted to the srb module and are flushed by the
 * explorer control mechanism.
 *
 * Revision 3.29.4.41  1996/07/20  03:20:47  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.29.4.40  1996/07/19  04:47:11  shaker
 * CSCdi55978:  Runts are passed up from SEEQ on EIP
 * Branch: California_branch
 * Check for runts, because the SEEQ is passing us up runt
 * ethernet packets, which it should not be doing.
 *
 * Revision 3.29.4.39  1996/07/18  20:29:12  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.29.4.38  1996/07/17  01:45:21  fsunaval
 * CSCdi59824:  RIF classification done on the VIP for the 1F.
 * Branch: California_branch
 * 1.Off-load RIF classification from the RSP to the VIP for the 1F.
 *   What's next, fragmentation?
 *
 * Revision 3.29.4.37  1996/07/13  00:56:04  cakyol
 * CSCdi62865:  rsp_aip_fs MAY dereference NULL
 * Branch: California_branch
 *
 * Revision 3.29.4.36  1996/07/11  14:21:24  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.29.4.35  1996/07/09  06:04:53  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.29.4.34  1996/07/08  22:52:42  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.29.4.33  1996/07/04  00:14:05  rbadri
 * CSCdi61232:  IP & IPX ping packets > 1484 would fail on RSP (1483-LANE
 * Bridging)
 * Branch: California_branch
 * For ATM, check the incoming datagramsize against sub_mtu + encaps
 * overhead.
 *
 * Revision 3.29.4.32  1996/06/27  16:36:49  hampton
 * Keep your mitts off the scheduler private data structures.  [CSCdi61518]
 * Branch: California_branch
 *
 * Revision 3.29.4.31  1996/06/20  21:03:33  shaker
 * CSCdi60921:  More sanity checking needed on ReturnQ processing
 * Branch: California_branch
 * Verify that the queue pointer is non zero before using it,
 * to prevent 'reserved exception' errors from crashing the router.
 *
 * Revision 3.29.4.30  1996/06/20  03:11:29  ogrady
 * CSCdi59082:  7507 reloads with %ALIGN-1-FATAL in tbridge_rsp_forward,
 *              and SEGV Branch: California_branch
 *              Add sanity checks for packets going to backing store or
 *              queueing.
 *
 * Revision 3.29.4.29  1996/06/18  21:12:52  wilber
 * CSCdi60675:  IRB with AIP not working - Input queue stalled
 * No need to specifically copy oldpak's if_input to newpak in
 * rsp_queue_atm_for_process()
 * Branch: California_branch
 *
 * Revision 3.29.4.28  1996/06/17  23:37:17  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.29.4.27  1996/06/06  18:45:14  ssangiah
 * CSCdi59131: RSP2 crashed when passing high-rate traffic over ten vlans
 *             on vip2
 * Branch: California_branch
 *
 * Revision 3.29.4.26  1996/06/04  18:48:32  fred
 * CSCdi58659:  Wrongful decrement of TX acc. resulting in output stuck on
 * Fddi/Hssi
 * Branch: California_branch
 *
 * Revision 3.29.4.25  1996/06/01  01:32:35  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.29.4.24  1996/05/21  19:40:18  fsunaval
 * CSCdi45026:  Frames of 17 bytes (without LLC) are considered as runt
 * Branch: California_branch
 *
 * Revision 3.29.4.23  1996/05/19  21:12:10  hou
 * CSCdi57265:  Bridging broke when ISL enabled on FEIP between Catalyst
 * and 7500
 * Branch: California_branch
 * - set CRC bit for ISL vLAN flooding packet on RSP to fix missing
 *   CRC problem.
 *
 * Revision 3.29.4.22  1996/05/18  19:53:19  ppearce
 * CSCdi58099:  RSP fastswitch performance - reduce path length for non-SRB
 * Branch: California_branch
 *   Code review comments (better ordering of SRB check)
 *
 * Revision 3.29.4.21  1996/05/18  13:16:46  ppearce
 * CSCdi58099:  RSP fastswitching performance - reduce path length for non-SRB
 * Branch: California_branch
 *   Access of SRB subblock during init affects all protocols
 *
 * Revision 3.29.4.20  1996/05/18  01:25:47  fred
 * CSCdi57547:  Various crashes dequeueing packet buffers on RSP during
 *         congestion. Fix: move PAK_BUF_RSP_QUEUE to pak->flags so that
 *         it doesn't get into the free list.
 * Branch: California_branch
 *
 * Revision 3.29.4.19  1996/05/17  11:54:30  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.22.2.6  1996/05/10  15:15:20  fbordona
 * CSCdi57207:  DLSw FST broken over WAN token ring and FDDI
 *              Remove redundant check for RXTYPE_SNAP_DODIP.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.22.2.5  1996/05/05  23:53:41  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.22.2.4  1996/04/26  15:24:28  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.22.2.3  1996/04/03  21:43:03  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.22.2.2  1996/03/21  06:10:43  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.22.2.1  1996/03/17  18:31:08  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.29.4.18  1996/05/14  02:31:02  bcole
 * CSCdi57575:  CSCdi56013 causes crash in rsp_oqueue_getbuffer
 * Branch: California_branch
 *
 * Check for NULL return value from getbuffer.
 *
 * Revision 3.29.4.17  1996/05/13  14:47:42  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.29.4.16  1996/05/12  23:23:07  xliu
 * CSCdi56777:  memd buffers are queued to process level for
 * encryption/decryption
 * Branch: California_branch
 *
 * Revision 3.29.4.15  1996/05/10  19:01:39  kleung
 * CSCdi57119:  SMRP fast switching from FDDI fails.
 * Branch: California_branch
 *
 * Revision 3.29.4.14  1996/05/09  14:46:15  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.29.4.13  1996/05/07  20:05:12  xliu
 * CSCdi56777:  memd buffers are queued to process level for
 * encryption/decryption
 * Branch: California_branch
 *
 * Revision 3.29.4.12  1996/05/07  00:14:43  mbeesley
 * CSCdi55679:  Add IP flow/optimum switching engine
 * Branch: California_branch
 *
 * Revision 3.29.4.11  1996/05/06  07:15:36  schaefer
 * CSCdi49868: crash at interrupt level on rsp2 (7513)
 * Branch: California_branch
 * complain & return early if no IDB found for MIP channel
 *
 * Revision 3.29.4.10  1996/05/04  20:49:50  fred
 * CSCdi56013:  alignment problems with moving data to paks
 * Branch: California_branch
 *
 * Revision 3.29.4.9  1996/05/04  01:48:01  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Force packets with station bit set but fail to be fast-switched fall
 *   through bridging(if bridging is enabled) in order to do IRB processing
 *   (token, fddi).
 * - Bypass station bit check if IRB is enabled(ether).
 * - Restore input to the Brige-group Virtual Interface if necessary when
 *   copying packet for process-level.
 *
 * Revision 3.29.4.8  1996/05/01  00:12:54  shaker
 * CSCdi55969:  ATM OAM cells not received on 7500
 * Branch: California_branch
 * Pass frames received from AIP with a null VC up to process level.
 *
 * Revision 3.29.4.7  1996/04/29  15:49:21  fred
 * CSCdi40287:  fsip is dropping large # of packets when fair-queue is on
 * Branch: California_branch
 *
 * Revision 3.29.4.6.4.1  1996/04/27  07:25:57  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.29.4.6  1996/04/19  15:31:24  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.29.4.5  1996/04/17  13:48:17  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.29.4.4  1996/04/04  09:38:33  smackie
 * Un-obfuscate buffer copy code for prolonged simplicity. Remove
 * misguided internal particle structure from paktype. Add a discrete
 * pak_coalesce() call for particle re-assembly. (CSCdi51223)
 * Branch: California_branch
 *
 * Revision 3.29.4.3.4.1  1996/04/08  02:09:27  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.29.4.3  1996/03/29  01:57:33  mcieslak
 * CSCdi52360:  Transparent Bridging truncates packets
 * Branch: California_branch
 * The new pak's datagramsize needs to be set prior to the invokation of
 * ether_parse_packet.
 *
 * Revision 3.29.4.2  1996/03/21  23:49:59  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.29.4.1  1996/03/18  22:02:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.29.6.2  1996/03/28  17:36:23  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.29.6.1  1996/03/25  02:20:58  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.29  1996/03/11  11:09:43  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Expunge code.
 *
 * Revision 3.28  1996/03/07  11:14:22  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * TRIP rxoffset needs to be on a long word boundary.
 *
 * Revision 3.27  1996/03/05  21:18:03  hou
 * CSCdi50213:  vLAN 802.10 does not fast switch the echo return IPX
 *              ping packets.
 * - rsp_fip_fs()
 *   we need to call fddi fastswitch code to demultiplex vlan id for
 *   vlan multicast packet. Also set first software idb to vlan sub-
 *   interface instead of the default value for vlan packet going to
 *   process level
 *
 * Revision 3.26  1996/03/04  15:29:11  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Add minimal support for optimum/flow switching of tokenring (without
 * rif) and atm (aal5-mux and llc-snap only).
 *
 * Revision 3.25  1996/02/29  05:22:41  foster
 * CSCdi47230:  HSA slave should come up after crash
 *
 * Revision 3.24  1996/02/28  18:16:01  kmitchel
 * CSCdi43990:  IP packets from the CIP are not being fastswitched on 75XX
 *
 * Revision 3.23  1996/02/27  19:28:13  achopra
 * CSCdi45189:  "show atm traffic" shows different (wrong) packet counts
 *
 * Revision 3.22  1996/02/24  07:36:37  shaker
 * CSCdi49798:  tx_limit should be a short, not a ushort
 *
 * Revision 3.21  1996/02/23  07:42:08  ogrady
 * CSCdi49673:  Flushed SRB explorers also counted as input drops
 *              Stop it.
 *
 * Revision 3.20  1996/02/13  05:52:05  ogrady
 * CSCdi45887:  Router crash in rsp_fastsend
 *              Allocate memd buffers with extra padding for
 *              memdcopy() overrun.
 *
 * Revision 3.19  1996/02/13  01:34:34  motto
 * Spontaneous cbus complex restarts running CIP CSNA and
 * also out stuck messages.
 * CSCdi47536:  CIP failure in 7513 - CY-Bus problems
 *
 * Revision 3.18.2.1  1996/03/14  19:18:46  rbadri
 * Branch: LE_Cal_V111_1_0_3_branch
 * Store atm_vcid, if_input etc. in the copied packet before sending to
 * process level code.
 *
 * Revision 3.18  1996/02/08  22:22:34  wmay
 * CSCdi48458:  mip uses old scheduler - use watched queues for love
 * note handler
 *
 * Revision 3.17  1996/02/05  03:34:36  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.16  1996/02/04  12:17:34  shaker
 * CSCdi47971:  RSP-3-NORESTART error message
 * Use portable array references instead of post incremented pointer
 * dereferences when passing arguments to errmsg(), because the order
 * of function argument evaluation varies.
 *
 * Revision 3.15  1996/02/04  02:29:34  shaker
 * CSCdi47971:  RSP-3-NORESTART error message
 * Prevent this error from happening. Also enhance error logging
 * for some hard to reproduce error cases.
 *
 * Revision 3.14  1996/01/23  08:52:42  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.13.2.1  1996/01/24  23:04:03  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.13  1996/01/22  07:15:55  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.12  1996/01/05  04:57:09  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.11  1995/12/28  08:24:56  foster
 * CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
 *
 * Revision 3.10  1995/12/15  11:57:44  ogrady
 * CSCdi43520:  custom queueing locks up serial interface on 7513
 * Send buffers that restart interface output queues to the return queue
 * instead of the original free queue.  Add a few sanity checks and error
 * messages.
 *
 * Revision 3.9  1995/12/15  09:22:00  ogrady
 * CSCdi39140:  Multicast fastswitching doesnt work on an RSP box.
 * Add code to FIP interrupt handler to call fast switching for multicast
 * packets.  Port 11.0 tunnel fast switching code to RSP.  While the hood
 * is up fix missing breaks from CSCdi43015.
 *
 * Revision 3.8  1995/12/13  21:09:36  ppearce
 * CSCdi45473:  %SYS-3-SUPNONE: Registry 6 doesn't exist msg
 *   Missed file during previous commit
 *
 * Revision 3.7  1995/12/13  04:23:40  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.6  1995/12/05  15:52:18  ppearce
 * CSCdi42839:  LLC2 connection to CSNA internal LAN not established on RSP
 *
 * Revision 3.5  1995/11/28  03:31:56  getchell
 * CSCdi41544:  Changing encap on HSSI interface causes the checker board
 * to disable
 *
 * Revision 3.4  1995/11/26  08:18:55  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *              ISL vLAN support for the RSP platform.
 *
 * Revision 3.3  1995/11/17  18:48:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:33:04  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:28:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.32  1995/11/08  21:28:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.31  1995/10/25  20:16:39  ppearce
 * CSCdi42839:  LLC2 connection to CSNA internal LAN not established on RSP
 *
 * Revision 2.30  1995/10/21  19:23:02  fred
 * CSCdi40287:  fsip is dropping large # of packets when fair-queue is on
 * Correct use of idb->oqueue and idb->oqueue_dequeue. Must use both
 * together;
 * was using holdq_enqueue_fast_reserve and idb->oqueue_dequeue()
 *
 * Revision 2.29  1995/10/11  13:57:07  motto
 * RSP SRB fast-switching path would cause this errmsg if adapter
 * was removed or not ack'd by CIP when forwarding paks.
 * CSCdi39794:  %MEMD-3-BADVCN error messages after CIP mic reload
 *
 * Revision 2.28  1995/10/11  07:10:38  mcieslak
 * Reload the microcode for a slot that has failed and allow
 * for IPs that are disabled to be removed cleanly.
 * CSCdi41907:  complex restart / EOIR of disabled cards broken
 * Revision 1.2.4.16  1995/11/16  05:53:17  getchell
 * Work around while alternative solutions being explored
 * CSCdi41544:  Changing encap on HSSI interface causes the checker board
 * to disable
 *
 * Revision 2.27  1995/09/26  21:32:57  motto
 * Decouple IPC from RSP subsystem.
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.26  1995/09/17  20:55:28  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.25  1995/09/09  00:53:48  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.24  1995/09/07  19:58:27  schaefer
 * CSCdi39476: periodic cbus complex messages (%RSP-3-RESTART)
 * reset debounce not functional due to confused timer usage
 *
 * Revision 2.23  1995/08/30  18:03:03  ogrady
 * CSCdi39559:  Token ring tbridging loses broadcast packets.  Change
 * token ring interrupt handler so that broadcast packets are fast
 * tbridged when its enabled.
 *
 * Revision 2.22  1995/08/12  07:10:14  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.21  1995/08/10  20:33:58  schaefer
 * CSCdi38496: output hang timestamp used as future timer
 *
 * Revision 2.20  1995/08/10  16:38:01  ppearce
 * CSCdi38300:  Add CSNA packet switching to 75xx (RSP)
 *   Modularity problems - use srberr_debug rather than channel_debug_vlan
 *
 * Revision 2.19  1995/08/09  21:33:35  ppearce
 * CSCdi38300:  Add CSNA packet switching to 75xx (RSP)
 *
 * Revision 2.18  1995/08/09  00:22:01  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.17  1995/08/07  05:42:36  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *
 * Revision 2.16  1995/08/01  06:52:42  dkerr
 * CSCdi36127:  IP with options gets dropped as checksum error
 * Don't call flow/optimum switching code if IP has options.
 *
 * Revision 2.15  1995/07/21  19:48:40  schaefer
 * CSCdi36254: traceback displayed when changing serial encaps
 * emit traceback only if "debug cbus" when recarving
 *
 * Revision 2.14  1995/07/19  06:47:46  shaker
 * CSCdi36264:  fast and optimum switching dont prime returnq
 * Bring 11.0 code up to date with solution in 10.3 code.
 *
 * Revision 2.13  1995/07/18  04:16:17  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Use new pak_set_dirty() to only set PAK_BUF_DIRTY iff
 * PAK_BUF_USE_DIRTY is set. Set PAK_BUF_USE_DIRTY for RSP fs_pak
 *
 * Revision 2.12  1995/07/17  08:22:00  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.11  1995/07/16  22:31:01  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.10  1995/07/13  22:19:12  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Create a pool that uses the public pools for pak_dup, etc, and put
 * the RSP fast pak structure in it.
 *
 * Revision 2.9  1995/07/11  00:52:08  ogrady
 * CSCdi34908:  Process level R/SRB hangs interface (orig AT over SRB
 * fails).  Don't make fastout vector return_false to disable srb fast
 * switching since its called from srb_common() path.  Set AC byte to
 * AC_PRI4 for all srb output paths.  Don't count ignored MAC packets as
 * drops.  Ongoing SRB counter clean up.
 *
 * Revision 2.8  1995/07/10  05:40:04  fred
 * CSCdi36851:  disable fancy queuing if using autonomous, sse, flow, or
 * optimal switching - ie, only use it if process switching or fast
 * switching. Applies to AGS/7000/7500 platforms.
 *
 * Revision 2.7  1995/06/29  07:10:34  dbath
 * CSCdi35616:  x25 windows close and never reopen
 * If we need to mess with datagramstart in the output path,
 * be sure to restore it before releasing the packet.  Failure
 * to do so, will result in LAPB retransmissions corrupting
 * the layer 3 packets (in this case the X.25 RRs).
 *
 * Revision 2.6  1995/06/23  18:48:37  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.5  1995/06/21  09:00:54  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.4  1995/06/20  07:18:44  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/10  17:51:14  pitargue
 * CSCdi35638:  packets going to process level from the 4500 atm nim
 * crashes router
 * make sure to initialize fast_vc_info, before calling
 * hwidb->parse_packet
 *
 * Revision 2.2  1995/06/08  09:12:45  dbath
 * CSCdi35345:  fsip configured with x25/atalk, requires longer timeout
 * periods
 * Arm the return queue processing, so that when we leave something on
 * the output hold queue, we'll know to send it once the hardware queue
 * drains.  Otherwise, it'll wait around until someone else requests
 * us to output (in this case the LAPB t1 timer expired, shocking us
 * back into life, which is why the ping timeout had to be set to
 * something larger than the t1 timer - 3 seconds).
 *
 * Revision 2.1  1995/06/07  22:58:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "../h/fastswitch_registry.regc"
#include "media_registry.h"
#include "subsys.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "exception.h"
#include "../os/free.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

#include "ieee.h"
#include "../if/rif_util.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../tbridge/tbridge_sde.h"
#include "../if/tring.h"
#include "../if/rif_inline.h"
#include "../if/tring_common.h"
#include "../srt/srt_registry.h"
#include "../if/rif.h"
#include "../srt/srb.h"
#include "../srt/rsrb.h"
#include "../srt/srb_core.h"
#include "../srt/srb_sb_private.h"
#include "../srt/srb_multiring.h"
#include "../srt/srb_error.h"
#include "../srt/fastsrb.h"
#include "../srt/trsrb.h"
#include "../srt/fddisrb.h"
#include "../ibm/lanmgr.h"
#include "address.h"
#include "../if/atm.h"
#include "../wan/serial.h"
#include "../ibm/sdlc.h"
#include "../x25/lapb.h"
#include "../x25/x25.h"
#include "../wan/frame_relay.h"
#include "../wan/ppp.h"
#include "../wan/smds.h"
#include "../wan/atm_dxi.h"

#include "rsp_bufhdr.h"
#include "../src-4k/signal_4k.h"
#include "../src-4k/alignment.h"
#include "rsp_if.h"
#include "rsp_qasic.h"
#include "rsp_pcmap.h"
#include "lovenote.h"
#include "../hes/if_fip.h"
#include "../hes/hes_xdi.h"

#include "qa_hw.h"
#include "rsp_fast.h"
#include "rsp_cache.h"
#include "../hes/if_hes_common.h"
#include "../hes/if_fci.h"		/* This should be broken up XXX */
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/ucode.h"      
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/dbus.h"
#include "../hes/cbus_registry.regh"
#include "../hes/if_cmd.h"
#include "../hes/if_trip.h"
#include "../hes/if_cip.h"

#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "../tcp/tcpcompress.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_flow.h"

#include "../xdi/xdihdrsif.h"
#include "../xdi/fddihdr.h"
#include "../xdi/csphdr.h"
#include "../xdi/cspglbl.h"
#include "../xdi/smtmsg.h"
#include "../xdi/cspproto.h"
#include "../xdi/smterror.h"
#include "../xdi/cspglbl.h"
#include "../xdi/fbmframe.h"
#include "../xdi/fbmhdr.h"
#include "../xdi/fbmglbl.h"
#include "../xdi/fbmproto.h"
#include "../xdi/mibglbl.h"
#include "../xdi/mibproto.h"
#include "../if/if_fddi.h"
#include "../if/if_fddi_public.h"
#include "../hes/hes_fip_inline.h"
#include "../if/dsx1.h"
#include "../hes/if_mip.h"
#include "ttysrv.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../if/traffic_shape.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "msg_rsp.c"			/* Not a typo, see logger.h */
#include "msg_qa.c"			/* Not a typo, see logger.h */

#ifdef DEBUG
# define inline 
# define static 
#endif

#include "../lane/lane.h"
#include "../lane/lanefast_les.h"

#include "../if/ether_inline.h"
#include "../if/fddi_inline.h"
#include "../if/tring_inline.h"
#include "../if/serial_inline.h"
#include "../if/atm_inline.h"

#include "../if/channel.h"
#include "../if/channel_inline.h"
#include "../hes/channel_sb.h"

#define RSP_MAX_BUFFERS 50
#define RSP_SMALL_BUFFER 128
#define RSP_LARGE_BUFFER 1520
static queuetype rsp_small_buffers;
static queuetype rsp_large_buffers;

/*
 * Forward declarations
 */
static void rsp_aip_fs_body (bufhdr_t *bufptr, hwidbtype *input);
static void rsp_eip_fs_body (bufhdr_t *bufptr, hwidbtype *input);
static void rsp_fip_fs_body (bufhdr_t *bufptr, hwidbtype *input);
static void rsp_trip_fs_body (bufhdr_t *bufptr, hwidbtype *input);
void rsp_qa_reuse0_diagnostics(r4k_context_t *cp, ulong info);

/*
 * Variables for interface to Low End fast switching
 */
paktype	*fs_pak;
static paktype  *rsp_flood_fs_pak;
static uint	*memd_buffer_data_start, *memd_end;

ulong global_ip_counts_cache;

/*
 * This number is large so that we can have more than one local
 * free queue for some interfaces that need it.
 */
hwidbtype	*local_queue_to_hw_idb[RSP_MAX_LOCAL_FREE_QUEUES];




/*
 * Each periodic, (i.e. each 1 second) we'll ask all the plugholes
 * in a particular slot, to return a single rx buffer to the global
 * free queue.  We cycle through each slot, each second.  This variable
 * keeps track of which slot we're up to.
 */
static int return_rx_buffer_slot;



boolean		rsp_debug;

boolean		disable_auto_interface_restart;

static tinybool	restart_complex_req,	/* If TRUE, MEMD needs to be
					   recarved, and interfaces
					   restarted */
		restart_complex_ucode;	/* if TRUE, reload ucode */

static sys_timestamp last_complex_restart;




/*
 * in_memd_buffer_data - return TRUE if an uncached MEMD address is a legal
 *           MEMD buffer address, ie - between the end of
 *           buffer header address space and the end of MEMD.
 */

static boolean
in_memd_buffer_data (uint *addr)
{
    if ( (addr >= memd_buffer_data_start) && (addr < memd_end) )
	return(TRUE);
    else
	return(FALSE);
}

/*
 * rsp_getbuffer
 */
paktype *rsp_oqueue_getbuffer (ushort size)
{
    paktype *pak;
    if (size <= RSP_SMALL_BUFFER) {
	if (rsp_small_buffers.count)
	    pak = dequeue(&rsp_small_buffers);
	else {
	    pak = getbuffer(RSP_SMALL_BUFFER);
	    if (pak)
		pak->flags |= PAK_RSP_QUEUE;
	}
    } else if (size <= RSP_LARGE_BUFFER) {
	if (rsp_large_buffers.count)
	    pak = dequeue(&rsp_large_buffers);
	else {
	    pak = getbuffer(RSP_LARGE_BUFFER);
	    if (pak)
		pak->flags |= PAK_RSP_QUEUE;
	}
    } else {
	pak = getbuffer(size);
    }
    if (pak)
	pak->datagramstart = pak->network_start = pak_center(pak);
    return pak;
}
/*
 * rsp_retbuffer
 */
static void rsp_oqueue_retbuffer(paktype *pak)
{
    if (pak->flags & PAK_RSP_QUEUE) {
	if (pak->datagramsize <= RSP_SMALL_BUFFER) {
	    if (rsp_small_buffers.count < rsp_small_buffers.maximum) {
		enqueue(&rsp_small_buffers, pak);
		return;
	    }
	} else {
	    if (rsp_large_buffers.count < rsp_large_buffers.maximum) {
		enqueue(&rsp_large_buffers, pak);
		return;
	    }
	}
    }
    /*
     * it is unnecessary to clear the PAK_RSP_QUEUE flag, as this is
     * done in datagram_done
     */
    datagram_done(pak);
}

/*
 * Ask each plughole in the 'current' slot (as indicated by
 * return_rx_buffer_slot) to return a single buffer, and
 * move the 'current' slot onto the next slot.
 */
static void
rsp_return_rx_buffer (void)
{
    slottype   *slotp;
    ushort     plughole_number;
    tinybool   crash_timer;

    /*
     * If the bus is still being reset, then we really don't want
     * to be doing this stuff just yet, so simply return, and we'll
     * start for real later.
     */
    if (cbus_being_reset)
	return;

    /*
     * First step is to grab a pointer to the current slot, and
     * move return_rx_buffer_slot onto the next slot, taking into
     * account any wrap at the end.  We do this precisely once
     * every tick, regardless of whether or not a slot has any
     * IPs in it.  This gives us a consistent return buffer interval
     * of once every MAX_SLOTS seconds.
     */
    slotp = &slots[return_rx_buffer_slot];
    return_rx_buffer_slot++;
    if (return_rx_buffer_slot == MAX_SLOTS)
	return_rx_buffer_slot = 0;

    /*
     * did this card crash long enough ago that it's time to restart?
     */
    crash_timer = (TIMER_RUNNING(slotp->crash_time) &&
		   (ELAPSED_TIME(slotp->crash_time) > (10*ONEMIN)));
    if (crash_timer)
	TIMER_STOP(slotp->crash_time);

    /*
     * If there is an IP in this slot and it's disabled,
     * check to see if it's time to restart one that crashed.  If not,
     * just quietly return.  This won't disturb our
     * scan frequency, since we've already bumped the
     * counter above.
     */
    if ((slotp->flags & (BOARD_DISABLED|BOARD_NONEXIST)) == BOARD_DISABLED) {
	if (crash_timer) {
	    GET_TIMESTAMP(slotp->crash_time);
	    /*
	     * Download new ucode to the offending board and set
	     * flag to reset cbus complex.
	     */
	    dbus_hard_ctrlr_reset(pslot_to_dbus(slotp->slot_num));
	    dbus_load_ucode(slotp->slot_num);
	    platform_req_complex_restart(NULL);
	}
	return;
    }

    /*
     * For each plughole on this IP, request it to return a buffer.
     */
    for (plughole_number = 0; plughole_number < slotp->interface_count;
	 plughole_number++) {
	hwidbtype  *hwidb;

	hwidb = slotp->interface_list[plughole_number];
	if (hwidb) {
	    leveltype  level;
	    ushort     rx_owned;

	    level = raise_interrupt_level(NETS_DISABLE);
	    
	    if ((select_if(hwidb) == CMD_RSP_OKAY) &&
		(send_return_buffer_cmd(hwidb, &rx_owned) == CMD_RSP_OKAY)) {

		hwidb->rx_currently_owned = rx_owned;
		if (rx_owned > hwidb->max_rx_currently_owned)
		    hwidb->max_rx_currently_owned = rx_owned;
	    }

	    reset_interrupt_level(level);
	}
    }
}


/**********************************************************************/
/* ReturnQ Support structures:                                        */
/**********************************************************************/

static return_data_t	return_data[RETURNQ_SIZE];

queuetype		free_retdatQ;	/* Return data struct free list */

void
initialize_retd_structs (void)
{
    int ndx;

    /*
     * Initialize ReturnQ data structure free list:
     */
    queue_init(&free_retdatQ, RETURNQ_SIZE);
    for (ndx = 0; ndx < RETURNQ_SIZE; ndx++) {
	return_data_t	*retd;

	retd = &return_data[ndx];
	retd->next = NULL;
	retd->hwidb = NULL;
	retd->type = RETQ_INVALID;
	retd->queue_ptr = 0;
	retd->bufhdr_offset = 0;
	retd->id = ndx;			/* ID for struct (constant) */
	enqueue(&free_retdatQ, retd);
    }
}

/*
 * clear cbus info
 */
void
clear_cbus_info (void)
{
    restart_complex_req = FALSE;
    restart_complex_ucode = FALSE;
    GET_TIMESTAMP(last_complex_restart);

    initialize_retd_structs();

    return;
}

static void
rsp_check_complex_restart (void)
{
    /*
     * Be able to blow out of error recovery
     */
    if (disable_auto_interface_restart || hotstall_flag)
	return;

    /*
     * Has a restart of the entire switching complex been requested?
     * reload ucode next time if a complex restart just finished.
     */
    if (restart_complex_req) {
	if (TIMER_RUNNING(last_complex_restart) &&
	    CLOCK_IN_INTERVAL(last_complex_restart, ONEMIN)) {
	    restart_complex_ucode = TRUE;
	    return;
	}
	platform_demand_complex_restart(NULL);
    } else {
	if (TIMER_RUNNING(last_complex_restart) &&
	    CLOCK_OUTSIDE_INTERVAL(last_complex_restart, ONEMIN))
	    TIMER_STOP(last_complex_restart);
    }

    return;
}

/*
 * Platform specific routine to check for interface or card/slot
 * restart requests. Called from process level
 * by periodic_activity() via reg_invoke_if_onesec():
 *
 * Check restart requests from most global to least global.
 * Stop after performing one.
 */

#define OUTHANG_TIMEOUT	(ONEMIN)

static void
rsp_check_if_restart (hwidbtype *hwidb)
{
    tinybool output_restart;

    /*
     * Be able to blow out of error recovery:
     */
    if (disable_auto_interface_restart || restart_complex_req || hotstall_flag)
	return;

    if ( (hwidb->state == IDBS_ADMINDOWN) || (!(hwidb->status & IDB_CBUS)) )
	return;

    /*
     * check for wedged output
     */
    output_restart = FALSE;
    if (hwidb->output_qcount > 0) {
	/*
	 * If there's something on the queue, give it a goose rather
	 * than relying on restart interrupts to determine whether an
	 * interface is hung.
	 */
	(*hwidb->soutput)(hwidb);

	if (ELAPSED_TIME(hwidb->lastoutput) > 2*OUTHANG_TIMEOUT) {
	    if (cbus_debug)
	      buginf("\n%s lastoutput : %08x, lastinput : %08x, output_qcount %d",
		     hwidb->hw_namestring, hwidb->lastoutput, 
		     hwidb->lastinput, hwidb->output_qcount);
	    platform_demand_complex_restart(hwidb);
	    return;
	}
	if (ELAPSED_TIME(hwidb->lastoutput) > OUTHANG_TIMEOUT)
	    output_restart = TRUE;
    }

    /*
     * Requested restart of this interface?
     */
    if ((hwidb->status & IDB_RESTART) || output_restart) {
	/*
	 * If the interface hasn't been reset within a reasonable
	 * minimum time interval, reset it now. Otherwise, wait until
	 * the minimum timer interval expires:
	 */
	if (!TIMER_RUNNING(hwidb->lastreset) ||
	    CLOCK_OUTSIDE_INTERVAL(hwidb->lastreset, OUTHANG_TIMEOUT)) {
	    GET_TIMESTAMP(hwidb->lastreset);
	    hwidb->status &= ~IDB_RESTART;
	    errvarmsg(&msgsym(RESTART, RSP),
		      "interface %s%s", hwidb->hw_namestring,
		      (output_restart) ? ", output stuck" : "");
	    if (cbus_debug)
	      buginf("\nlastoutput : %08x, lastinput : %08x, output_qcount %d",
		     hwidb->lastoutput, hwidb->lastinput, hwidb->output_qcount);
	    hwidb->reset(hwidb);
	}
    }
    return;
}

/*
 * platform_req_interface_restart
 * Platform specific routine to request a reset of only the
 * affected interface for an attempt at error recovery. A background
 * process honors the request at a later time.
 */

void
platform_req_interface_restart (hwidbtype *hwidb)
{
    if (!system_configured)
	return;

    if (cbus_debug) {
	buginf("\n%s restart request", hwidb->hw_namestring);
	bugtrace();
    }

    /*
     * Don't touch anything if IDBS_ADMINDOWN or IDBS_RESET
     * or if restart has already been requested
     */
    if ((hwidb->state == IDBS_ADMINDOWN) ||
	(hwidb->state == IDBS_RESET) ||
	(hwidb->status & IDB_RESTART))
	return;

    /*
     * Set it up so that periodic_activity() will reset this
     * interface when it next runs:
     */
    hwidb->status |= IDB_RESTART;

    /*
     * If the interface is not already down, take it down:
     */
    if (hwidb->state != IDBS_DOWN)
	net_cstate(hwidb, IDBS_DOWN);

    return;
}

/*
 * platform_req_card_restart
 * Platform specific routine to request a reset of only the card/slot
 * which contains the affected interface for an attempt at error recovery.
 * Can't currently do this without risking buffer loss, so restart the
 * world at this point.
 */

void
platform_req_card_restart (hwidbtype *hwidb)
{
    platform_req_complex_restart(hwidb);
    return;
}

/*
 * platform_req_complex_restart
 * Platform specific routine to request a reset of the entire switching
 * complex which contains the affected interface in an attempt at error
 * recovery.  A background process honors the request at a later time.
 */

void
platform_req_complex_restart (hwidbtype *hwidb)
{
    if (!system_configured)
	return;

    if (restart_complex_req)
	return;

    if (cbus_debug) {
	buginf("\nCBUS restart request %s", (hwidb) ? hwidb->hw_namestring :
            "NULL");
	bugtrace();
    }

    /*
     * Arrange for the MEMD to be recarved, and for all interfaces to
     * be reinitalized. This is roughly equivalent to a 'micro reload'.
     */
    restart_complex_req = TRUE;

    if (hwidb == NULL)
	return;

    /*
     * Don't touch anything if IDBS_ADMINDOWN or IDBS_RESET, or !IDB_CBUS
     */
    if ( (hwidb->state == IDBS_ADMINDOWN) ||
	(hwidb->state == IDBS_RESET) ||
	(!(hwidb->status & IDB_CBUS)) )
	return;

    /*
     * If the interface is not already down, take it down:
     */
    if (hwidb->state != IDBS_DOWN)
	net_cstate(hwidb, IDBS_DOWN);

    return;
}


/*
 * platform_demand_complex_restart
 * Initialize a restart of the switching complex NOW:
 */

void
platform_demand_complex_restart (hwidbtype *hwidb)
{
    slottype* sp;
    leveltype level;

    if (!system_configured || hotstall_flag)
	return;

    if (onintstack()) {
	platform_req_complex_restart(hwidb);
	return;
    }

    errvarmsg(&msgsym(RESTART, RSP), "cbus complex");

    if (cbus_debug)
	bugtrace();

    FOR_ALL_ENABLED_IP_SLOTS(sp) {
	dbus_logger_drain(sp->slot_num);
    }

    level = raise_interrupt_level(LEVEL_CONSOLE);

    FOR_ALL_ENABLED_IP_SLOTS(sp)
	disable_slot(sp->slot_num);

/* FIXME dg */
#ifdef NOTDEFINED
    if (restart_complex_ucode)
#endif
	dgbus_analyze(FALSE /* not EOIR */);

    cbus_analyze(FALSE /* not the first time */, FALSE /* not EOIR */);

    rsp_memd_carve(FALSE /* not EOIR */);

    reset_interrupt_level(level);

    return;
}

/*
 * Tie a hardware IDB to a local free queue so that we can quickly convert
 * from a local free queue pointer to a hardware IDB at interrupt level.
 *
 * The local_free_q_ptr field must already be initialized in the hardware
 * IDB.
 *
 * Remember to change LOCAL_QUEUE_TO_HW_IDB whenever you change this routine.
 * The two routines must agree on which hardware IDB is associated with
 * which local free queue!
 */

void
rsp_register_hwidb_for_local_free_queue (hwidbtype *hwidb)
{
    uint	ptr_index;
    ushort	queue_offset;

    /*
     * Set up so we can quickly convert from local free Q ptr to
     * a hardware IDB:
     */
    queue_offset = ADDR_TO_MEMD_OFFSET_16(hwidb->local_free_q_ptr);
    ptr_index = (uint)((queue_offset & LOCAL_QUEUE_MASK) >> LOCAL_QUEUE_SHIFT);
    local_queue_to_hw_idb[ptr_index] = hwidb;
}

/************************* Interrupt Throttling ******************************/

static ulong
    netint_ticks,			/* max ticks to spend in net. intr. */
    netint_mask_ticks;			/* ticks to mask network interrupts */

static ushort netint_mask_usec;		/* usecs to mask network interrupts */

static tinybool
    netint_throttle_active,		/* is network interrupt masked? */
    netint_throttle_configured;		/* is interrupt masking enabled? */

static uint
    netint_throttle_count;		/* throttle activation count */

static usec_timestamp
    netint_exit,			/* when to mask interrupts */
    netint_return;			/* last time netw. inter. returned */


/*
 * referenced by DCL timer interrupt wrapper
 */
extern void (*dcl_timer_handler)(void);	/* throttle-off interrupt handler */
extern uint dcl_timer_count;		/* count of throttle-off interrupts */

/*
 * handle "scheduler allocate" command
 *
 *	OBJ(int,1) = microseconds for network interrupt handler
 *	OBJ(int,2) = microseconds with network interrupts masked
 */
void
sched_alloc_command (parseinfo *csb)
{
    if (csb->nvgen) {
	/*
	 * emit a config line if any value differs from its default
	 */
	if ((netint_throttle_configured != NETINT_THROTTLE_DEFAULT) ||
	    (netint_ticks		!= R4K_USEC2COUNT(NETINT_DEFAULT)) ||
	    (netint_mask_usec		!= NETINT_MASK_DEFAULT)) {
	    nv_write(TRUE, "%s%s",
		     (netint_throttle_configured) ? "" : "no ",
		     csb->nv_command);
	    /*
	     * emit parameters only if it's ON
	     */
	    if (netint_throttle_configured)
		nv_add(TRUE, " %u %u",
		       R4K_COUNT2USEC(netint_ticks), netint_mask_usec);
	}
	return;
    }

    if (csb->set_to_default) {
	csb->sense = NETINT_THROTTLE_DEFAULT;
	GETOBJ(int,1) = 0;
    }
    if (csb->sense && !netint_throttle_configured) {
	/*
	 * extra init if it's being switched on
	 */
	USEC_GET_TIMESTAMP(netint_return);
	netint_exit = netint_return;
    }
    if (GETOBJ(int,1) == 0) {
	netint_ticks = R4K_USEC2COUNT(NETINT_DEFAULT);
	netint_mask_usec = NETINT_MASK_DEFAULT;
    } else {
	netint_ticks = R4K_USEC2COUNT(GETOBJ(int,1));
	netint_mask_usec = GETOBJ(int,2);
    }
    netint_mask_ticks = R4K_USEC2COUNT(netint_mask_usec);
    if (!(netint_throttle_configured = csb->sense)) {
	/*
	 * Set netint_ticks to very very long so that rsp_process_rawq()
	 * doesn't need to access global variable netint_throttle_configured
	 * in each loop
	 */
	netint_mask_ticks = 0;
	netint_ticks = 0x7FFFFFFF;
    }
    return;
}

/*
 * display network interrupt throttling stats
 */
void
netint_throttle_show (void)
{
    printf("\nThrottle count %u, DCL timer count %u",
	   netint_throttle_count, dcl_timer_count);
    printf("\nactive %u, configured %u",
	   netint_throttle_active, netint_throttle_configured);
    printf("\nnetint usec %u, netint mask usec %u",
	   R4K_COUNT2USEC(netint_ticks), netint_mask_usec);
    return;
}

/*
 * enable network interrupt throttling by masking network interrupts.
 * arrange for DCL timer2 to interrupt later when its time to reenable
 * network interrupts.
 */
static void
netint_throttle_on (void)
{
    volatile ushort *countval;
    volatile uchar *netmask, *countctl;

    netint_throttle_active = TRUE;
    ++netint_throttle_count;
    netmask = (uchar*) ADRSPC_QA_NETINT_MASK;
    *netmask = QA_INT_DISABLE;
    countval = (ushort*) ADRSPC_COUNTER2;
    *countval = netint_mask_usec;
    countctl = (uchar*) ADRSPC_COUNTCTL;
    *countctl |= COUNTCTL_C2ENABLE;
    return;
}

/*
 * handle "throttle off" timer interrupt
 * disable network interrupt throttling by unmasking network interrupts
 */
static void
netint_throttle_off (void)
{
    volatile uchar *netmask, *countctl, *clear2;

    countctl = (uchar*) ADRSPC_COUNTCTL;
    *countctl &= ~COUNTCTL_C2ENABLE;
    clear2 = (uchar*) ADRSPC_CLEAR_TIMER2;
    *clear2 = 0;
    netint_throttle_active = FALSE;
    USEC_GET_TIMESTAMP(netint_exit);
    netint_exit += netint_ticks;

    /*
     * Check if MEMD is corrupted by a parity error.
     * If it is, a complex restart has been requested and the
     * ADRSPC_QA_NETINT_MASK has already been disabled.  The
     * ADRSPC_QA_NETINT_MASK will be enabled after the complex
     * restart has completed.
     */

    if (!memd_parity_error) {
       netmask = (uchar*) ADRSPC_QA_NETINT_MASK;
       *netmask = QA_INT_ENABLE;
    }

    return;
}

/*
 * called at the bottom of the scheduler main loop
 * turns off throttling if active and nothing is happening at process level
 */
void
rsp_sched_loop_end (boolean ran_norm)
{
    if (netint_throttle_configured) {
	if (!ran_norm && netint_throttle_active)
	    netint_throttle_off();
    }
    return;
}

/*
 * set-up the exit timer on entry to the packet interrupt handler
 */
static inline usec_timestamp
netint_throttle_setup (void)
{
    ulong diff;
    usec_timestamp now;

    USEC_GET_TIMESTAMP(now);
    diff = now - netint_return;
    if (diff >= netint_mask_ticks)
	return(now + netint_ticks);
    else
	return(netint_exit + diff);
}

/*
 * initialize network interrupt throttling mechanism
 */
static void
netint_throttle_init (void)
{
    dcl_timer_count = 0;
    dcl_timer_handler = netint_throttle_off;

    USEC_GET_TIMESTAMP(netint_return);
    netint_exit = netint_return;

    netint_throttle_active = FALSE;
    netint_throttle_count = 0;
    netint_ticks = R4K_USEC2COUNT(NETINT_DEFAULT);
    netint_mask_usec = NETINT_MASK_DEFAULT;
    netint_mask_ticks = R4K_USEC2COUNT(netint_mask_usec);
    netint_throttle_configured = NETINT_THROTTLE_DEFAULT;

    reg_add_hardware_shutdown(netint_throttle_off, "netint_throttle_off");

    return;
}
/********************** End of Interrupt Throttling **************************/

/*
 * get a pak suitable _only_ for immediate fastswitching output.
 * the returned paktype hdr may be static and should always be returned via
 *   hwidb->fastsend() or datagram_done()
 *   before additional calls are made to this routine.
 *   this is an acceptable limit for low end flooding type code.
 *   the alternative - using a queue of bufferless paktypes - is probably
 *   a bit slower and more wasteful at this point.
 *
 * this buffer must NOT be locked or queued.
 * this routine should not be used from process level
 *
 * as with getbuffer() - size need not include ENCAPBYTES
 *                       size is from pak_center(pak)
 *
 * On RSP this returns a single static paktype header pointing to a
 *  memd buffer if possible - meaning you shouldn't call this routine
 *  multiple times without disposing of the packet between each call.
 *
 * On les boxes this can just return getbuffer(size)
 */
paktype *
platform_fastsend_getbuffer (int size)
{
    paktype *pak = rsp_flood_fs_pak;
    bufhdr_t  *bufptr;

    /*
     * check that the static flood pak is not in use
     *  (as indicated by dirty flag) and
     * that we have a memd buffer
     */
    if ( !(pak->buffer_flags & PAK_BUF_DIRTY) &&
         (bufptr = get_memd_buffer( size + RSP_FLOOD_PAD + MEMDCOPY_PAD))) {

        pak->data_area = (uchar *)MEMD_OFFSET_TO_ADDR(bufptr->bf_buffer_ptr);
        pak->buffer_header = (uchar *)bufptr;
        pak->datagramstart = pak->data_area + RSP_FLOOD_PAD;
        pak->network_start = pak->data_area + RSP_FLOOD_PAD;

    } else {
        /*
         * otherwise get a system buffer
         */

        pak = getbuffer(size);
    }
    return(pak);
}


/*
 * get an input buffer if the interface isn't throttled
 */
static inline paktype*
rsp_input_getbuffer (uint size, hwidbtype* hwidb)
{
    if (idb_is_throttled(hwidb)) {
	++hwidb->counters.input_drops;
	return NULL;
    }
    return input_getbuffer(size, hwidb);
}

/*
 * Throttle the specified interface, since we could not get another
 * buffer for input packets passed up to process level.
 */
static inline void
rsp_idb_throttle (hwidbtype *hwidb)
{
    if (!idb_is_throttled(hwidb))
	idb_mark_throttled(hwidb);
    return;
}

/*
 * rsp_fs_free_memd_pak
 * Invalidate the buffer header and the packet data that we touched
 * in our attempts at fast switching this input packet, and then free
 * the buffer header and MEMD buffer that it points to.
 *
 * Called when we drop an input packet, and also called after we copy
 * a packet out of MEMD into DRAM.
 *
 * NOTE that this routine does NOT Writeback any dirty packet data from
 * the data cache. So do NOT use it in the transmit path.
 */

void
rsp_fs_free_memd_pak (paktype *pak)
{
    bufhdr_t	*bufptr;
    bufptr = (bufhdr_t *) pak->buffer_header;
    if (bufptr) {

	/*
	 * Invalidate the entire packet data that we might
	 * have read or written while fast switching.
	 */
	invalidate_data_inline(pak->data_area,
			       (pak->datagramsize +
				(pak->datagramstart - pak->data_area)));
	pak->buffer_flags &= ~PAK_BUF_DIRTY; /* Clean now */

	/*
	 * Free the buffer header and the MEMD buffer it points to:
	 */
	retbufhdr_inline(bufptr);
	pak->buffer_header = NULL;

    } else {
	errmsg(&msgsym(BADBUFHDR, RSP), "freeing MEMD pak", (ulong) bufptr);
    }
}

/*
 * rsp_queue_ether_for_process
 * Copy an ethernet input packet from MEMD into DRAM and
 * then queue it for process level
 */

boolean rsp_queue_ether_for_process (hwidbtype *hwidb, paktype *oldpak,
				     ether_hdr *ether)
{
    paktype	*newpak;
    ushort	size;

    /*
     * What we need to do:
     * o Get a packet buffer of the appropriate size, via input_getbuffer().
     * o Initialize the pointers into the packet in the packet structure.
     * o Copy the packet data to the new packet buffer.
     * o Pass packet buffer up to process level via (hwidb->iqueue)().
     * o Free the input buffer header (and associated packet buffer).
     */
    size = oldpak->datagramsize;
    newpak = rsp_input_getbuffer(size, hwidb);
    if (!newpak) {

	rsp_idb_throttle(hwidb);		/* Out of input buffers */

	/*
	 * We are done with the packet data and buffer header,
	 * so invalidate and then free it
	 */
	rsp_fs_free_memd_pak(oldpak);
	return(TRUE);
    }

    /*
     * Set up the pointers into the new packet properly so that the
     * network layer begins on 'the line' for process level routines.
     * Then copy the packet data from MEMD into newpak->datagramstart:
     *
     * The bcopy should really be replaced with a 'bcopy-source-invalidate',
     * or some such, to invalidate the source from the data cache as it is
     * copied. It could also do some whizzy 'create-dirty-exclusive' cache
     * magic to prevent us from doing a 'read/modify/write' on the
     * destination in DRAM, to save time. XXX
     */
    newpak->flags |= (oldpak->flags & (PAK_MCICHECKED | PAK_MFS_SWITCHED));
    newpak->rxtype = oldpak->rxtype;
    newpak->datagramsize = size;
    hwidb->parse_packet(hwidb, newpak, (uchar*) &ether->type_or_len);
    bcopy(ether, newpak->datagramstart, size);

    /*    
     * If subinterface routing/bridging we may have overwritten
     * "firstsw" therefore preserve the correct software IDB.
     *
     * If IRB(Integrated Routing and Bridging) is enabled, 
     * the input interface *may* have been overwritten to the BVI
     * (Bridge-group Virtual Interface) in tbridging fast-switching
     * code where it makes route/bridge decision. Take car of
     * input_qcount instead of just overwriting it.
     */
    if (global_irb_enable && is_bvi(oldpak->if_input->hwptr)) {
	change_if_input(newpak, oldpak->if_input);
	newpak->irb_rcvd_input = oldpak->irb_rcvd_input;
    } else {
	newpak->if_input = oldpak->if_input;
    }

    /*
     * Do some accounting
     */
    hwidb->counters.rx_cumbytes += size;
    hwidb->counters.inputs++;

    /*
     * Have to check for LLC2 on ethernet here:
     */
    if (newpak->enctype == ET_LLC2) {
	llc_input(newpak);
    } else {
	hwidb->iqueue(hwidb, newpak);
    }

    rsp_fs_free_memd_pak(oldpak);	/* Done with MEMD buffer */
    return(TRUE);
}

/*
 * rsp_queue_atm_for_process
 * Copy an atm input packet from MEMD into DRAM and
 * then queue it for process level
 */

static void
rsp_queue_atm_for_process (hwidbtype *hwidb, paktype *oldpak,
			   atm_encap_t *atm)
{
    paktype	*newpak;
    ushort	size;
    iqueue_t    iqueue;


    /*
     * What we need to do:
     * o Get a packet buffer of the appropriate size, via input_getbuffer().
     * o Initialize the pointers into the packet in the packet structure.
     * o Copy the packet data to the new packet buffer.
     * o Pass packet buffer up to process level via (hwidb->iqueue)().
     * o Free the input buffer header (and associated packet buffer).
     */
    size = oldpak->datagramsize;
    newpak = rsp_input_getbuffer(size, hwidb);
    if (!newpak) {

	rsp_idb_throttle(hwidb);		/* Out of input buffers */

	/*
	 * We are done with the packet data and buffer header, so
	 * invalidate and free it
	 */
	rsp_fs_free_memd_pak(oldpak);

	return;
    }

    /*
     * Set up the pointers into the new packet properly so that the
     * network layer begins on 'the line' for process level routines.
     * Then copy the packet data from MEMD into newpak->datagramstart:
     *
     * The bcopy should really be replaced with a 'bcopy-source-invalidate',
     * or some such, to invalidate the source from the data cache as it is
     * copied. It could also do some whizzy 'create-dirty-exclusive' cache
     * magic to prevent us from doing a 'read/modify/write' on the
     * destination in DRAM, to save time. XXX
     */
    newpak->flags |= (oldpak->flags & (PAK_MCICHECKED | PAK_MFS_SWITCHED));    
    newpak->rxtype = oldpak->rxtype;
    newpak->datagramsize = size;
    newpak->atm_vcid = oldpak->atm_vcid;
    newpak->atm_enctype = oldpak->atm_enctype;
    iqueue = hwidb->parse_packet(hwidb, newpak, (uchar*) atm);
     /*
      * in case we got a NULL vector, which should never happen
      * normally, we'd better drop the packet and return.
      */
     if (iqueue == NULL) {
       hwidb->counters.input_error++;
       MAYBE_DROPPED(hwidb);
       retbuffer(newpak);
       rsp_fs_free_memd_pak(oldpak);
       return;
     }

    bcopy(atm, newpak->datagramstart, size);

    /*
     * If IRB(Integrated Routing and Bridging) is enabled, 
     * the input interface *may* have been overwritten to the BVI
     * (Bridge-group Virtual Interface) in tbridging fast-switching
     * code where it makes route/bridge decision. Set newpak's 
     * if_input to bvi if it's true.
     */
    if (global_irb_enable && is_bvi(oldpak->if_input->hwptr)) {
	change_if_input(newpak, oldpak->if_input);
	newpak->irb_rcvd_input = oldpak->irb_rcvd_input;
    }

    /*
     * Do some accounting
     */
    hwidb->counters.rx_cumbytes += size;
    hwidb->counters.inputs++;

    /*
     * Queue the packet for process level:
     */
    iqueue(hwidb, newpak);

    rsp_fs_free_memd_pak(oldpak);	/* Done with MEMD buffer */

    return;
}

/*
 * rsp_queue_channel_for_process
 * Copy a channel input packet from MEMD into DRAM and
 * then queue it for process level
 */

static void
rsp_queue_channel_for_process (hwidbtype *hwidb, paktype *oldpak,
			       channel_hdr *chan)
{
    paktype	*newpak;
    ushort	size;

    /*
     * What we need to do:
     * o Get a packet buffer of the appropriate size, via input_getbuffer().
     * o Initialize the pointers into the packet in the packet structure.
     * o Copy the packet data to the new packet buffer.
     * o Pass packet buffer up to process level via (hwidb->iqueue)().
     * o Free the input buffer header (and associated packet buffer).
     */
    size = oldpak->datagramsize;
    newpak = rsp_input_getbuffer(size, hwidb);
    if (!newpak) {

	rsp_idb_throttle(hwidb);		/* Out of input buffers */

	/*
	 * We are done with the packet data and buffer header, so
	 * invalidate and free it
	 */
	rsp_fs_free_memd_pak(oldpak);

	return;
    }

    /*
     * When linktype is LINK_SRB (i.e. packet is from CIP ILAN)
     *   copy info from oldpak into newpak
     *
     */
    if (oldpak->linktype == LINK_SRB) {

        newpak->rxtype = RXTYPE_UNKNOWN;

        newpak->if_input = oldpak->if_input;

        newpak->linktype = LINK_SRB;
        newpak->flags |= PAK_SRB_IN;

        size = newpak->datagramsize = oldpak->datagramsize;
        bcopy(oldpak->datagramstart, newpak->datagramstart, size);

        newpak->riflen = oldpak->riflen;
        newpak->rif_match = NULL;
        newpak->fcs_type = FCS_TYPE_TR;

    } else {

        /*
         * Set up the pointers into the new packet properly so that the
         * network layer begins on 'the line' for process level routines.
         * Then copy the packet data from MEMD into newpak->datagramstart:
         *
         * The bcopy should really be replaced with a 'bcopy-source-invalidate',
         * or some such, to invalidate the source from the data cache as it is
         * copied. It could also do some whizzy 'create-dirty-exclusive' cache
         * magic to prevent us from doing a 'read/modify/write' on the
         * destination in DRAM, to save time. XXX
         */
        newpak->flags |= (oldpak->flags & (PAK_MCICHECKED | PAK_MFS_SWITCHED));

        newpak->rxtype = oldpak->rxtype;
        newpak->datagramsize = size;
        hwidb->parse_packet(hwidb, newpak, (uchar*) chan);
        bcopy(chan, newpak->datagramstart, size);

    }

    /*
     * Do some accounting
     */
    hwidb->counters.rx_cumbytes += size;
    hwidb->counters.inputs++;

    /*
     * Queue the packet for process level:
     */
    hwidb->iqueue(hwidb, newpak);

    rsp_fs_free_memd_pak(oldpak);	/* Done with MEMD buffer */

    return;
}


static inline boolean
rsp_multicast_srb_explorer (ushort status, idbtype *swidb, paktype *pak)
{
    if ((status & RSP_MULTICAST_MASK)
    &&  (status & CTR_RXSTAT_SRB) 
    && is_tr_explorer_pak(pak)
    && multicast_explorer_can_be_bridged(swidb, pak->rxtype, 
				       (sap_hdr *)pak->info_start))
          return TRUE;

    return FALSE;
}

/*
 * rsp_queue_trip_for_process
 *
 * Copy a tokenring input packet from MEMD into DRAM 
 * for code paths that might queue trip packet to process level.
 * srb_common() expects TR FCS at end of pak which TRIP leaves in MEMD
 * but doesn't count in byte counts
 */

boolean rsp_queue_trip_for_process (hwidbtype *ihwidb, paktype *fspak,
				    ushort status)
{
    paktype	*pak; 
    tring_hdr	*trh;
    sap_hdr	*llc1;
    ushort	dgsize;
    ushort	size_with_fcs;
    boolean     multicast_to_srb_common = FALSE;

    dgsize = fspak->datagramsize;
    size_with_fcs = fspak->datagramsize + TR_FCS_SIZE;
	/*
	 * must increase fspak->datagramsize to include fcs so that the
	 * cache invalidate in rsp_fs_free_memd_pak() flushes the fcs.
	 */
    fspak->datagramsize = size_with_fcs;

    if (status & CTR_RXSTAT_MAC) {
	if (!(status & CTR_RXSTAT_MINE) || 
	    !(ihwidb->tr_ring_mode & RINGMODE_NEED_MAC)) {

	    /*
	     * A promiscuous MAC packet, or there is no one wants
	     * to listen to a MAC frame on this interface.
	     */
            rsp_fs_free_memd_pak(fspak);
	    return(TRUE);
	}
	if (!(ihwidb->tr_ring_mode & RINGMODE_RS)) {

	    /*
	     * Ignore AMP and SMP frames most of the time.
	     */
	    if ( ((tring_mac_frame *)
		  fspak->datagramstart)->command == MVID_AMP || 
		((tring_mac_frame *)
		 fspak->datagramstart)->command == MVID_SMP) {
                rsp_fs_free_memd_pak(fspak);
	        return(TRUE);
	    }
	}

        pak = rsp_input_getbuffer(size_with_fcs, ihwidb);
        if (!pak) {
            rsp_idb_throttle(ihwidb);
            rsp_fs_free_memd_pak(fspak);
            return(TRUE);
        }

        pak->riflen = fspak->riflen;
        pak->datagramstart =
	    pak->network_start - fspak->riflen - TRING_ENCAPBYTES;
        pak->datagramsize = dgsize;
        pak->info_start = pak->network_start;

        /*
	 * xxx replace with rsp copy using cache ops
	 * copy fcs into pak too
	 */
        bcopy(fspak->datagramstart, pak->datagramstart, size_with_fcs);

	pak->enctype = ET_MAC;
        if (status & ihwidb->multibits)
            ihwidb->counters.input_broad++;

	ihwidb->counters.rx_cumbytes += dgsize;
	ihwidb->counters.inputs++;

	if (tokenring_debug) {
	    tr_print_packet(ihwidb, "mac:", (tring_hdr *) pak->datagramstart,
                (ulong *)pak->info_start, status, dgsize);
	}

	(*(ihwidb->iqueue))(ihwidb, pak);
        rsp_fs_free_memd_pak(fspak);
	return(TRUE);
    }

    /*
     * Bridge selected Multicast Explorers as common SRB packets.
     * These include illegal linktypes that are bridged by default.
     * This is to prevent storms of these packets from filling up the
     * process level queues and causing interface throttling.
     * See multicast_explorer_can_be_bridged().
     */
    if (rsp_multicast_srb_explorer(status, ihwidb->firstsw, fspak)) {
        if (reg_invoke_srt_flush_explorer(ihwidb, fspak->datagramsize)) {
            rsp_fs_free_memd_pak(fspak);
            return(TRUE);
        }
        multicast_to_srb_common = TRUE;
    }

    pak = rsp_input_getbuffer(size_with_fcs , ihwidb);
    if (!pak) {
        rsp_idb_throttle(ihwidb);
        rsp_fs_free_memd_pak(fspak);
        return(TRUE);
    }

    /*
     * If IRB(Integrated Routing and Bridging) is enabled, 
     * the input interface *may* have been overwritten to the BVI
     * (Bridge-group Virtual Interface) in tbridging fast-switching
     * code where it makes route/bridge decision. Take car of
     * input_qcount instead of just overwriting it.
     * copy the real received input interface as well.
     */
    if (global_irb_enable && is_bvi(fspak->if_input->hwptr)) {
	change_if_input(pak, fspak->if_input);
	pak->irb_rcvd_input = fspak->irb_rcvd_input;
    }

    /*
     * Set up pointers into new system packet buffer, then copy the packet
     */
    pak->riflen = fspak->riflen;
    pak->flags |= (fspak->flags & (PAK_MCICHECKED | PAK_MFS_SWITCHED));    
    pak->rxtype = fspak->rxtype;
    ihwidb->rxrifwordlen = fspak->riflen/2;
    /* parse should use pak->riflen */
    (*ihwidb->parse_packet)(ihwidb, pak, fspak->info_start);
    pak->datagramsize = dgsize;

    ihwidb->counters.rx_cumbytes += dgsize;
    ihwidb->counters.inputs++;

    /*
     * Send SRB packets to srb_common().
     */
    if ((status & CTR_RXSTAT_BOTH) == CTR_RXSTAT_SRB || multicast_to_srb_common) {
        /*
         * tring_parse_packet() may not set datagramstart far enough into
         * buffer to allow prepending RSRB/TCP header?
         * xxx replace with rsp copy using cache ops
         * copy fcs into pak too - srb (at least) code expects it
         */
        pak->datagramstart = pak->network_start;
        bcopy(fspak->datagramstart, pak->datagramstart, size_with_fcs);
        srb_common_setup(ihwidb, pak, status);
	rsp_fs_free_memd_pak(fspak);
        return(TRUE);
    }

        /*
         * xxx replace with rsp copy using cache ops
         */
    bcopy(fspak->datagramstart, pak->datagramstart, size_with_fcs);


    trh = (tring_hdr *) pak->datagramstart;
    llc1 = (sap_hdr *) pak->info_start;

    if (status & ihwidb->multibits)
        ihwidb->counters.input_broad++;




    /*                                                                  
     * CSCdi16607                                                       
     * Novell, NetBIOS Kludge: We want to bridge novell-ether and       
     *   NetBIOS broadcast over token.                                  
     *   Please reference "token_get_enctype_infosize()", there we      
     *   classify lpak->enctype for them.                               
     *                                                                  
     * ET_NOVELL_ETHER and ET_UNSUPPORTED are not supported -           
     *   Those two kinds of packets will be classified as               
     *   LINK_ILLEGAL as linktype, and will be bridged.                 
     */                                                                 
     if (((llc1->control & ~LLC1_P) != LLC1_UI) &&  
         (pak->enctype != ET_NOVELL_ETHER) &&                           
         (pak->enctype != ET_UNSUPPORTED)) {                            
	/*
	 * Is it anything other than an LLC UI
	 * frame.  This routine handles XID and TEST packets, as well as 
	 * all LLC Type 2 frames.
	 */
	rif_update_llc(ihwidb, 0, trh->saddr, NULL,
		   (srbroute_t *)(pak->riflen ? trh->data : NULL),
		   llc1, RIF_AGED);

	switch (llc1->control & ~LLC1_P) {

	  case LLC1_XID:
	    tr_process_xid(ihwidb, pak, FALSE, FALSE);
            break;
	  case LLC1_TEST:
	    tr_process_test(ihwidb, pak, FALSE);
            break;
	  default:
	    reg_invoke_llc2_input(pak);
	    rsp_fs_free_memd_pak(fspak);
	    return(TRUE);
	}
    }

    /*                                                                  
     *  It's a LLC packet.  If it is a broadcast from us, drop it.      
     */                                                                 
    if ( (trh->daddr[0] & TR_GROUP) &&                                  
           ieee_equal_sans_rii( trh->saddr, ihwidb->hardware) ) {          
         retbuffer(pak);                                                
	 rsp_fs_free_memd_pak(fspak);
	 return(TRUE);
    }                                                                   

    /*
     * this would be spyglass srb match and addr match bits set
     * typically broadcast/exploreres (like ARPs)
     */
    if (status & CTR_RXSTAT_SRB)
        pak->flags |= PAK_SRB_IN;

    if (srb_multiring_any_enabled(ihwidb->firstsw)) {
	rif_update(ihwidb, 0, trh->saddr, NULL,
		   (srbroute_t *)(pak->riflen ? trh->data : NULL), RIF_AGED);
    }
    if (tokenring_debug) {
	tr_print_packet(ihwidb, " in:", trh, (ulong *)llc1, status, dgsize);
    }
    (*(ihwidb->iqueue)) (ihwidb, pak);
    rsp_fs_free_memd_pak(fspak);
    return(TRUE);
}

/*
 * rsp_queue_serial_for_process
 * Copy a serial input packet from MEMD into DRAM and
 * then queue it for process level

 */

static void
rsp_queue_serial_for_process (hwidbtype *hwidb, paktype *oldpak)
{
    paktype	*newpak;
    void	(*siqueue)(hwidbtype *, paktype *);
    uchar	*testlong;
    ushort	size;


    /*
     * What we need to do:
     * o Get a packet buffer of the appropriate size, via input_getbuffer().
     * o Initialize the pointers into the packet in the packet structure.
     * o Copy the packet data to the new packet buffer.
     * o Pass packet buffer up to process level via (hwidb->iqueue)().
     * o Free the input buffer header (and associated packet buffer).
     */
    size = oldpak->datagramsize;
    newpak = rsp_input_getbuffer(size, hwidb);
    if (!newpak) {

	MAYBE_DROPPED(hwidb);

	rsp_idb_throttle(hwidb);		/* Out of input buffers */

	/*
	 * We are done with the packet data and buffer header,
	 * so invalidate and then free it
	 */
	rsp_fs_free_memd_pak(oldpak);

	return;
    }

    /*
     * Do special work for SMDS frames:
     */
    testlong = oldpak->datagramstart;
    if (is_smds(hwidb)) {
	/*
	 * Snip the last 4 bytes of the encapsulation
	 */
	testlong = oldpak->network_start - sizeof(ulong);
    }

    /*
     * Set up the pointers into the new packet properly so that the
     * network layer begins on 'the line' for process level routines.
     * Then copy the packet data from MEMD into newpak->datagramstart:
     *
     * The bcopy should really be replaced with a 'bcopy-source-invalidate',
     * or some such, to invalidate the source from the data cache as it is
     * copied. It could also do some whizzy 'create-dirty-exclusive' cache
     * magic to prevent us from doing a 'read/modify/write' on the
     * destination in DRAM, to save time. XXX
     */
    newpak->flags |= (oldpak->flags & (PAK_MCICHECKED | PAK_MFS_SWITCHED));    
    newpak->rxtype = oldpak->rxtype;
    newpak->datagramsize = size;
    /*
     * If IRB(Integrated Routing and Bridging) is enabled, 
     * the input interface *may* have been overwritten to the BVI
     * (Bridge-group Virtual Interface) in tbridging fast-switching
     * code where it makes route/bridge decision. Restore it!
     */
    if (is_bvi(oldpak->if_input->hwptr)) {
	change_if_input(newpak, oldpak->if_input);
	newpak->irb_rcvd_input = oldpak->irb_rcvd_input;
    }

    siqueue = (*hwidb->parse_packet)(hwidb, newpak, testlong);
    if (siqueue == NULL) {
	hwidb->counters.input_error++;
	MAYBE_DROPPED(hwidb);
	if (serial_debug)
	    buginf("\nRSP(%s): Illegal packet encapsulation = 0x%04x",
		   hwidb->hw_namestring, *((long*) testlong));
	retbuffer(newpak);
	rsp_fs_free_memd_pak(oldpak);
	return;
    }

    /*
     * Don't need to check for giants here because FSIP, HIP,
     * and MIP all check for giants for us:
     */

    bcopy(oldpak->datagramstart, newpak->datagramstart, size);


    /*
     * Do some accounting
     */
    hwidb->counters.rx_cumbytes += size;
    hwidb->counters.inputs++;

    /*
     * Queue packetfor process level:
     */
    (*siqueue)(hwidb, newpak);

    rsp_fs_free_memd_pak(oldpak);	/* Done with MEMD buffer */

    return;
}

/*
 * rsp_queue_fddi_for_process
 * Copy an fddi input packet from MEMD into DRAM and
 * then queue it for process level
 *
 * This is expected to be called after fddi_decode_encapsulation_inline()
 * has set up: oldpak->riflen, oldpak->info_start, oldpak->network_start,
 * and oldpak->rxtype for us.
 */

boolean rsp_queue_fddi_for_process (hwidbtype *hwidb, paktype *oldpak)
{
    paktype	*newpak;
    iqueue_t	iqueue;
    fddi_hdr    *fddi;
    ether_hdr	*ether;
    bufhdr_t	*bufptr;
    ushort	size;

    fddi = (fddi_hdr *)oldpak->datagramstart;
    ether = (ether_hdr *)oldpak->datagramstart;

    /*
     * What we need to do:
     * o Get a packet buffer of the appropriate size, via input_getbuffer().
     * o Initialize the pointers into the packet in the packet structure.
     * o Copy the packet data to the new packet buffer.
     * o Pass packet buffer up to process level via (hwidb->iqueue)().
     * o Free the input buffer header (and associated packet buffer).
     */
    size = oldpak->datagramsize;
    newpak = rsp_input_getbuffer(size, hwidb);
    if (!newpak) {

	rsp_idb_throttle(hwidb);		/* Out of input buffers */

	/*
	 * We are done with the packet data and buffer header,
	 * so invalidate and then free it
	 */
	rsp_fs_free_memd_pak(oldpak);

	return(TRUE);
    }
    newpak->flags |= (oldpak->flags & (PAK_MCICHECKED | PAK_MFS_SWITCHED));    
    newpak->rxtype = oldpak->rxtype;
    newpak->datagramsize = size;
    newpak->riflen = oldpak->riflen;
    hwidb->rxrifwordlen = newpak->riflen/2;

    /*
     * Set up the pointers into the new packet properly so that the
     * network layer begins on 'the line' for process level routines.
     * Then copy the packet data from MEMD into newpak->datagramstart:
     *
     * The bcopy should really be replaced with a 'bcopy-source-invalidate',
     * or some such, to invalidate the source from the data cache as it is
     * copied. It could also do some whizzy 'create-dirty-exclusive' cache
     * magic to prevent us from doing a 'read/modify/write' on the
     * destination in DRAM, to save time. XXX
     */

    /*
     * Check to see if we have an encapsulated bridge frame
     */
    if (oldpak->flags & PAK_TBRIDGE_IN) {
	newpak->flags |= PAK_TBRIDGE_IN;

	/*
	 * As we have, in effect, an Ethernet frame addressed to us,
	 * parse the ethernet frame within. Whoever set PAK_TBRIDGE_IN
	 * already lopped off the FDDI encapsulation
	 */
	iqueue = ether_parse_packet(hwidb, newpak,
				    (uchar *)&ether->type_or_len);
    } else {

	/*
	 * See which FDDI encapsulation the packet is - ISO1, ISO2, or ISO3,
	 * and set up the pointers in the newpak so that we get the start
	 * of the packet in the correct place:
	 */
	iqueue = (*hwidb->parse_packet)(hwidb, newpak, oldpak->info_start);
    }
    bcopy(fddi, newpak->datagramstart, size);

    /*    
     * If subinterface routing/bridging we may have overwritten
     * "firstsw" therefore preserve the correct software IDB.
     *
     * If IRB(Integrated Routing and Bridging) is enabled, 
     * the input interface *may* have been overwritten to the BVI
     * (Bridge-group Virtual Interface) in tbridging fast-switching
     * code where it makes route/bridge decision. Take car of
     * input_qcount instead of just overwriting it.
     */
    if (global_irb_enable && is_bvi(oldpak->if_input->hwptr)) {
	change_if_input(newpak, oldpak->if_input);
	newpak->irb_rcvd_input = oldpak->irb_rcvd_input;
    } else {
	newpak->if_input = oldpak->if_input;
    }

    hwidb->counters.rx_cumbytes += size;
    hwidb->counters.inputs++;

    fddi = (fddi_hdr *)newpak->datagramstart;
    bufptr = (bufhdr_t *)oldpak->buffer_header;

    /*
     * Pass the frame up to process level in an appropriate way:
     */
    if (oldpak->flags & PAK_SRB_IN) {

        /*
         * srb packet to be bridged - dest is not this router
         * mac addrs should already be bitswapped
         */
        newpak->datagramstart--;
        newpak->datagramsize++;
        ((tring_hdr *)newpak->datagramstart)->ac = AC_PRI0;
        ((tring_hdr *)newpak->datagramstart)->fc = FC_TYPE_LLC;
        newpak->flags |= PAK_SRB_IN;
        srb_common_setup(hwidb, newpak, bufptr->bf_fip_status);

    } else {

      if (bufptr->bf_fip_status & FDDI_SMT_FRAME) {
	uchar *smtbuf;

	smtbuf = p_dequeue(&smtbufQ);
        if (smtbuf) {
            uchar	eac_bits;

            /*
	     * Note that the 3 extra FC bytes must be passed to
	     * the XDI FBM code also.
	     */
            bcopy( (oldpak->datagramstart - EXTRA_FC_BYTES), smtbuf,
                  (size + EXTRA_FC_BYTES) );

	    /*
	     * Swap the MAC encapsulation back to wire order. The FIP
	     * cononicalizes the FDDI MAC address that it gives to us.
	     * The XDI code expects it to be in native FDDI wire order:
	     */
	    ieee_swap((uchar *)(((MACHdrType *)smtbuf)->DA),
		      (uchar *)(((MACHdrType *)smtbuf)->DA));
	    ieee_swap((uchar *)(((MACHdrType *)smtbuf)->SA),
		      (uchar *)(((MACHdrType *)smtbuf)->SA));
	    eac_bits = (uchar)bufptr->bf_fip_status;
	    fddi_smt_frame(hwidb, smtbuf, size, eac_bits);
	    if (fddi_smt_packet_debug)
		fddi_smt_debug(newpak, FALSE);
        } else {
            bprintf("noFBMbuf, ");
        }
        retbuffer(newpak);

      } else {
	
            if (is_fddi_srb_pak(newpak, FDDIT_RII))
                newpak->flags |= PAK_SRB_IN;   /* srb for this router */
 
            if ((newpak->enctype == ET_LLC2) ||
                (srb_multiring_any_enabled(hwidb->firstsw))) {
 
                sap_hdr *llc1;
                uchar bitswapped_addr[IEEEBYTES];
 
                llc1 = (sap_hdr *)newpak->info_start;
                ieee_swap(fddi->saddr, bitswapped_addr);
                rif_update_llc(hwidb, 0, bitswapped_addr, NULL,
                       (srbroute_t *)((newpak->flags & PAK_SRB_IN) ?
                       fddi->data : NULL), llc1, RIF_AGED);
            }
 
            if (newpak->enctype == ET_LLC2) {
                fddi->saddr[0] &= ~FDDIT_RII;
		llc_input(newpak);
	    } else {
	      (*iqueue) (hwidb, newpak);
	    }
      }
    }
    rsp_fs_free_memd_pak(oldpak);	/* Done with MEMD buffer */

    return(TRUE);
}


/*
 * process rsrb direct input
 */
static boolean
rsp_fs_rsrb_direct_input (hwidbtype *ihwidb, paktype *pak)
{
    pak_set_dirty(pak);
    return(reg_invoke_fs_rsrb_direct_input(ihwidb, pak));
}

/*
 * process rsrb fst input
 */
static boolean
rsp_fs_rsrb_fst_input (hwidbtype *ihwidb, paktype *pak)
{
    pak_set_dirty(pak);
    return(reg_invoke_fs_rsrb_fst_input(ihwidb, pak));
}

/*
 * process dlsw direct input
 */
static boolean
rsp_fs_dlsw_direct_input (hwidbtype *ihwidb, paktype *pak)
{
    pak_set_dirty(pak);
    return(reg_invoke_nim_dlsw_input(ihwidb, pak));
}

/*
 * process udp broadcasts - if its enabled
 */
static inline boolean
rsp_udp_flood_fs(idbtype *fast_rcvidb, paktype *pak)
{
    if (ip_udp_flood_it) {
        return(reg_invoke_udp_flood_fs(fast_rcvidb, pak));
    }
    return (FALSE);
}

/*
 * copy data from fastswitch input buffer (only) to fastsend output buffer
 * only call this from fastswitching code at interrupt level
 */
void
platform_fs_pak_data_copy (paktype *srcpak,
			   paktype *dstpak,
			   uchar *srcptr,
			   uchar *dstptr,
			   int size)
{
   /*
    * should do everything uncached to avoid misses and to
    * save filling cache lines more important data.
    */

    if (dstpak->buffer_header) {
        memdcopy(srcptr, ADDR_MEMD_TO_UNCACHED(dstptr), size);
    } else {
	bcopy(srcptr, dstptr, size);
	pak_set_dirty(dstpak);
    }
    pak_set_dirty(srcpak);
}


/*
 * If we're not fast-flow-switching, then stuff the pak struct with
 * commonly needed fields.
 * Unfortunately, this is somewhat wasteful of code space compared
 * to putting this in the common path :-(
 */
inline void
    stuff_fs_pak (bufhdr_t *bufptr, hwidbtype *input, paktype *pak)
{
    uchar *data_area = MEMD_OFFSET_TO_ADDR(bufptr->bf_buffer_ptr);
    
    pak->flags = 0;
    pak->enctype = 0;		/* Needed after encapsulated bridging */
    pak->data_area = data_area;
    pak->buffer_header = (uchar *)bufptr;
    pak->datagramsize = bufptr->bf_rx_bytes;
    pak->datagramstart = data_area + input->rxoffset;
    pak->if_input = input->firstsw;
}

char runt_msg[] = "\nReceived %s runt from %s, %d bytes";

static inline void
rsp_eip_fs (bufhdr_t *bufptr, hwidbtype *input)
{
    /*
     * The SEEQ chip is sending us up runt packets that it should
     * instead be dropping, so check for them first. See CSCdi55978.
     */
    if (bufptr->bf_rx_bytes < MINETHERSIZE) {
	input->counters.runts++;
	input->counters.input_error++;

	/*
	 * Free the input MEMD buffer
	 * No cache invalidation is needed, because no one has yet touched
	 * the MEMD buffer that the buffer header points at, and the fs_pak
	 * structure is not yet filled out. Simply return the buffer
	 * header to the local free queue it came in from.
	 */
	if (rsp_debug)
	    buginf(runt_msg, "MAC", input->hw_namestring,
		   bufptr->bf_rx_bytes);
	retbufhdr_inline(bufptr);
	return;
    }

    /*
     * Check EIP address filter status to see if this is a Unicast
     * station matched IP datagram with a TTL greater than 1.
     */
    if (bufptr->bf_rx_status & RSP_DOD_IP_PROTOCOL_MASK) {
	iphdrtype   *ip;
	
	ip = (iphdrtype *)(bufptr->bf_buffer_ptr + input->ip_memd_offset);

	if (input->ip_turbo_fs(bufptr, ip, input) == IPFLOWOK)
	    return;
    }
    rsp_eip_fs_body(bufptr, input);
}

static void
rsp_eip_fs_body (bufhdr_t *bufptr, hwidbtype *input)
{
    ether_hdr	*ether;
    ushort	rxtype;
    ushort	status;

    status = bufptr->bf_rx_status;
    stuff_fs_pak(bufptr, input, fs_pak);

#ifdef DEBUG
    if (fs_pak->datagramsize > (input->max_pak_size)) {
	input->counters.giants++;
	input->counters.input_error++;
	if (rsp_debug)
	    buginf("\nReceived giant from EIP %s, %d bytes",
		   input->hw_namestring, fs_pak->datagramsize);
	rsp_fs_free_memd_pak(fs_pak);
	return;
    }
#endif /* DEBUG */

    ether = (ether_hdr *)fs_pak->datagramstart;
    rxtype = fs_pak->rxtype =
	ether_decode_encapsulation_inline(fs_pak, ether);

    /*
     * Attempt to bridge any packet with the bridge bit set that does
     * not also have the station bit set.  This attempt will succeed if
     * the packet was forwarded, flooded, or filtered by the transparent
     * bridge code.  This attempt will fail for a variety of packets
     * including interesting multicasts (aliases will be flooded), and
     * physical broadcasts in protocols we are routing (other physical
     * broadcasts will be flooded)
     */
    status = (bufptr->bf_rx_status & IPSTAT_ADDR_MASK);
    if (status & IPSTAT_BRIDGING_MASK) {
	if (!(status & IPSTAT_STATION_MASK) || global_irb_enable) {
	    /*
	     * If IRB(Integrated Routing and Bridging is enabled,
	     * bybass the station bit check since we want to 
	     * overwrite the input interface to the BVI
	     * (Bridge-group Virtual Interface) for the routable
	     * packets in tbridge fast-switching code when we make
	     * the bridge/route decision.
	     */
	    if (reg_invoke_tbridge_forward(fs_pak))
		return;
	}
    }

    /*
     * Attempt to fast switch.
     */
    if (reg_invoke_ether_fs(rxtype, fs_pak))
      return;

    /*
     * check for RSRB packets
     */
    if (rxtype == RXTYPE_SRB) {
        if (rsp_fs_rsrb_direct_input(input, fs_pak))
	    return;
    } else if (rxtype == RXTYPE_DLSW) {
        if (rsp_fs_dlsw_direct_input(input, fs_pak))
            return;
    }

    if (ROUTING_RXTYPE(input->firstsw, rxtype) &&
        (rxtype == RXTYPE_DODIP  || rxtype == RXTYPE_SNAP_DODIP)) {
        iphdrtype *ip;

        ip = (iphdrtype *) fs_pak->network_start;
        switch (ip->prot) {

            case UDP_PROT:
                if (rsp_udp_flood_fs(input->fast_rcvidb, fs_pak)) {
                     return;
                }
		break;

            case FST_RSRB_PROT:
                if (rsp_fs_rsrb_fst_input(input, fs_pak)) {
                    return;
                }
                break;

            case GRE_PROT:
            case IPINIP_PROT:
                if (reg_invoke_tunnel_ip_les_fs(fs_pak)) {
                    return;
                }
                break;
        }
    }

    /*
     * Count input broadcasts
     */
    if (status & IPSTAT_MULTICAST_MASK)
	input->counters.input_broad++;

    /*
     * We did not fast switch the datagram.
     */
    rsp_queue_ether_for_process(input, fs_pak, ether);
    return;
}

static inline void
rsp_fip_fs (bufhdr_t *bufptr, hwidbtype *input)
{
    /*
     * Check FIP address filter status to see if this is a Unicast
     * station matched IP datagram with mac length greater than 21,
     * and a TTL greater than 1, with no errors and LLC detected
     * by the Formac
     */
    if (input->status & IDB_VIP) {
	bufptr->bf_fip_srb = bufptr->bf_rxstatus2_or_txbytes_or_fipstatus;
	bufptr->bf_fip_status = bufptr->bf_rx_status;
    }

    /* Address filter results in MSB */
    /* Formac status bits in LSB */
    
    if ((bufptr->bf_fip_status & (RSP_PROTOCOL_BIT_MASK | FDDI_SX_MASK)) ==
			  RSP_DOD_IP_PROTOCOL_MASK) {
	iphdrtype   *ip;
	
	ip = (iphdrtype *)(bufptr->bf_buffer_ptr + input->ip_memd_offset);

	if (input->ip_turbo_fs(bufptr, ip, input) == IPFLOWOK)
	    return;
    }
    rsp_fip_fs_body(bufptr, input);
}

static void
rsp_fip_fs_body (bufhdr_t *bufptr, hwidbtype *input)
{
    ushort	cam_bits;		/* Save CAM bits from size field */
    ushort	size;
    ushort	status;			/* FIP address filter status */

    status = bufptr->bf_fip_status;
    stuff_fs_pak(bufptr, input, fs_pak);
    
    /*
     * Seperate the FDDIT information bits from datagramsize:
     *
     * cam_bits holds FDDIT_SRB, FDDIT_CAM_VALID, and FDDIT_SA_FND.
     *  FDDIT_SRB       means that the frame has a RIF.
     *  FDDIT_CAM_VALID means that the CAM search was valid.
     *  FDDIT_SA_FND    means that the source address was found in the CAM.
     */
    cam_bits = fs_pak->datagramsize & FDDIT_RIF_CAM;
    input->fddi_cam_sa_miss = cam_bits & (FDDIT_CAM_VALID | FDDIT_SA_FND);
    size = fs_pak->datagramsize & ~FDDIT_RIF_CAM;
    fs_pak->datagramsize = size;

#ifdef DEBUG
    if (size > input->max_pak_size) {
	input->counters.giants++;
	input->counters.input_error++;
	if (rsp_debug)
	    buginf("\nReceived giant from FIP %s, %d bytes",
		   input->hw_namestring, fs_pak->datagramsize);
	rsp_fs_free_memd_pak(fs_pak);
	return;
    }
#endif

    /*
     * Check for runts:
     */
    if (size >= FDDI_MINDGRAM) {

	ushort		rxtype;		/* Packet fast switch class */

	/*
	 * Only accept what the Formac says are good FDDI frames.
	 * It is faster for us to check for this than for the FIP to read
	 * the FC byte back from MEMD and check it for us:
	 */
	if ( ! (status & FDDI_SX_MASK) ) {

	    /*
	     * We only route or bridge LLC frames:
	     * Use Formac frame status bits to determine this. There are
	     * only four types of frames. If not MAC, SMT, or IMPLEMENTOR,
	     * it must be LLC:
	     */
	    if (! (status &
		   (FDDI_MAC_FRAME | FDDI_SMT_FRAME | FDDI_IMPLEMENTER_FRAME))
		) {

		/*
		 * This only classifies ISO2 FDDI frames.
		 * The rest are classified on the trip up to process level.
		 */
		rxtype = fs_pak->rxtype =
		    fddi_decode_encapsulation_inline(fs_pak, (fddi_hdr *)
					     fs_pak->datagramstart, FDDIT_RII);

  		/*
  		 * Was this frame sent directly to us?
		 */
		if (status & (IPSTAT_STATION_MASK | IPSTAT_HSRP_STATION_MASK)) {

		    /*
		     * Attempt to fast switch.
                     * Frame sent directly to our unicast address
                     * (handle multicasts later)
		     *
		     * Protect against forwarding killer packet fragments
		     * smaller than the encapsulation size
  		     */
		    if (fs_pak->datagramsize > fs_pak->encsize) {
			if (reg_invoke_fddi_fs(rxtype, fs_pak))
			    return;
		    }
		}

  		/*
  		 * Check for SRB packets.
		 */
        if ((status & (IPSTAT_STATION_MASK | IPSTAT_HSRP_STATION_MASK
                         | IPSTAT_BRIDGING_MASK)) == IPSTAT_BRIDGING_MASK) {

                    if (is_fddi_srb_pak(fs_pak, FDDIT_RII)) {

                        fddi_hdr *fddi = (fddi_hdr *) fs_pak->datagramstart;
 
                        /*
                         * SRB frame - no unicast mac address.  If it's not an
                         * explorer try to fastswitch the packet.  The field
                         * "fs_pak->riflen" has been set already by decode encaps.
                         */
                        if (!is_fddi_explorer_pak(fs_pak)) {
                            srb_sb_t *srb_sb = idb_get_swsb_inline(input->firstsw, 
                                                                   SWIDB_SB_SRB);
                            fs_pak->rif_match = fs_rif_match( 
                                      (srbroute_t *) fddi->data, fs_pak->riflen, 
                                      srb_sb->rng_br_rng_f, srb_sb->rng_br_rng_r);

                            if (fs_pak->rif_match) {

                                reg_invoke_media_fddi_to_tr(fs_pak, input);
                                fs_pak->fcs_type = FCS_TYPE_FDDI;

                                /* try to fastswitch it */
                                if ((*input->tr_srb_fastswitch)(input, fs_pak,
                                                         input->tr_bridge_idb))
                                      return;

			        /*
                                 * Fastswitching failed for some reason so mark 
                                 * this packet for process switching with the 
                                 * PAK_SRB_IN flag.  Leave mac addrs twisted but
                                 * readjust dgstart & dgsize for fddi_parse_packet(). 
				 */
                                fs_pak->flags |= PAK_SRB_IN;
                                fs_pak->datagramstart++;
                                fs_pak->datagramsize--;

			    } else {
				
				/*
				 * No rif match.  If not a multicast discard it.
				 */
                                if (!(status & IPSTAT_MULTICAST_MASK)) {
			           rsp_fs_free_memd_pak(fs_pak);
                                   return;
				}
				/*
                                 * An SRB with multicast address.  Fall thru to
                                 * protocol switching.  Someone may want it ...
				 */
			    }

                        } else {
                            /*
                             * It's an explorer so first check for congestion.
                             * If it's not a multicast, or is one that can be
                             * source-route-bridged, set it up for SRB process
                             * switching, else it goes to protocol switching.
                             */
                            if (reg_invoke_srt_flush_explorer(input,
                                                fs_pak->datagramsize)) {
                                rsp_fs_free_memd_pak(fs_pak);
                                return;
                            }

			    if (!(status & IPSTAT_MULTICAST_MASK)
                            ||  multicast_explorer_can_be_bridged(input->firstsw,
                                   rxtype, ((sap_hdr *)fs_pak->info_start))) {
                                ieee_swap(fddi->saddr, fddi->saddr);
                                ieee_swap(fddi->daddr, fddi->daddr);
                                fs_pak->flags |= PAK_SRB_IN;
			    }
                        }
                        rsp_queue_fddi_for_process(input, fs_pak);
                        return;
		    }
		}

		/*
                 * If tbridge is enabled try to forward the packet.
                 * (the bridging bits don't work for encapsulation bridging).
		 */
		if (input->tbridge_on_hwidb) {

                    /*
                     * Before trying to tbridge make sure the packet does
                     * not have a rif.  This could happen if srb is not
                     * configured and unicast or multicast packet is
                     * received with a rif.  Also, protect against killer
                     * packet fragments smaller than encapsulation size.
                     */
                    if (!is_fddi_srb_pak(fs_pak, FDDIT_RII))
			if (fs_pak->datagramsize >= fs_pak->encsize)
			    if (reg_invoke_tbridge_forward(fs_pak))
				return;
		}

                if (status & IPSTAT_MULTICAST_MASK) {
 
                    /*
                     * Multicast frame not bridged.
                     * Attempt to fast switch things like IP Multicast
                     * xxx should probably reject unwanted address filter
                     * aliases here.
                     */
                    if ((rxtype == RXTYPE_DODIP) ||
			(rxtype == RXTYPE_APPLE) ||
			(rxtype == RXTYPE_IEEE_802_10)) {
                        if (reg_invoke_fddi_fs(rxtype, fs_pak))
                            return;
	            }
		}

		/*
		 * Check for RSRB, UDP flood or tunnel
		 */
		if (rxtype == RXTYPE_SRB) {
                    if (rsp_fs_rsrb_direct_input(input, fs_pak))
	                return;
                } else if (rxtype == RXTYPE_DLSW) {
                    if (rsp_fs_dlsw_direct_input(input, fs_pak))
                  return;
                }

                if (ROUTING_RXTYPE(input->firstsw, rxtype) &&
		    (rxtype == RXTYPE_DODIP) ) {
		    iphdrtype *ip;

		    ip = (iphdrtype *) fs_pak->network_start;
		    switch (ip->prot) {

                        case UDP_PROT:
                            if (rsp_udp_flood_fs(input->fast_rcvidb, fs_pak)) {
                                return;
                            }
			    break;

                        case FST_RSRB_PROT:
                            if (rsp_fs_rsrb_fst_input(input, fs_pak)) {
                                return;
			    }
			    break;

                        case GRE_PROT:
                        case IPINIP_PROT:
                            if (reg_invoke_tunnel_ip_les_fs(fs_pak)) {
                                return;
                            }
                            break;
		    }
                }

		/*
		 * Count input broadcasts:
		 */
		if (status & IPSTAT_MULTICAST_MASK)
		    input->counters.input_broad++;

		/*
		 * Pass the input packet up to process
		 * level, first doing any necessary packet
		 * origin changes necessary for the different
		 * FDDI encapsulations:
		 */
		rsp_queue_fddi_for_process(input, fs_pak);
		return;

	    } else {
	    	    
		/*
		 * Not LLC frame. Is it SMT, MAC, or IMPLEMENTOR?
		 */
		if (status & FDDI_SMT_FRAME) {
		    fddi_hdr *fddi;	/* FDDI frame structure in MEMD */

		    fddi = (fddi_hdr *)fs_pak->datagramstart;

		    /*
		     * Prevent SMT Frame swamping.
		     * Is the source station already in the CAM? If not,
		     * insert it in the CAM now, so that we don't see
		     * packets sent directly to this destination station:
		     *
		     * We do NOT check FDDI_BRIDGE_CAM_SUPPORTED here
		     * since these stations are unlikely to be sources
		     * or destinations of bridged traffic.
		     */
		    if ( (cam_bits & (FDDIT_CAM_VALID | FDDIT_SA_FND) ) ==
			FDDIT_CAM_VALID) {

			/*
			 * Load the source MAC address into the CAM:
			 */
			hes_fddi_cam_populate_inline(input,
						     (uchar *)&fddi->saddr);
		    }

		    /*
		     * Discard all SMT frames if we don't have
		     * an SMT process running:
		     */
		    if (!(smt_process_id)) {
			rsp_fs_free_memd_pak(fs_pak);
			input->counters.input_drops++;
			return;
		    }

		    /*
		     * Input the SMT frame, after setting up
		     * fs_pak appropriately:
		     */
		    fs_pak->rxtype = RXTYPE_UNKNOWN;
		    fs_pak->info_start = fddi->data;
		    fs_pak->riflen = 0;
		    rsp_queue_fddi_for_process(input, fs_pak);
		    return;

		} else {

		    /*
		     * Discard all MAC frames, IMPLEMENTOR frames,
		     * tokens, voids, etc:
		     */
		    rsp_fs_free_memd_pak(fs_pak);
		    return;
		}
	    }
	} else {

	    /*
	     * Error status, count em and drop the datagram:
	     */
	    if (status & FDDI_ERROR_THIS_NODE) {
		input->counters.input_crc++;
		input->counters.input_error++;
	    }
	    rsp_fs_free_memd_pak(fs_pak);
	    return;
	}
    } else {

	/*
	 * Count the Runt and free up the input MEMD buffer:
	 */
        if (status & RSP_STATION_MASK) {
	    input->counters.runts++;
	    input->counters.input_error++;
        }
  	if (rsp_debug)
	    buginf(runt_msg, "MAC", input->hw_namestring,
		   fs_pak->datagramsize);
	rsp_fs_free_memd_pak(fs_pak);
    }
}

/*
 * fast bridge or fastswitch a datagram received on a TRIP.
 * this code is based on C7000 CBUS_TOKEN_RECEIVE macro
 * (this could probably be made to work with les hw with some clever macros)
 *
 * inputs: memd bufhdr, input hwidb 
 */
static inline void
rsp_trip_fs (bufhdr_t* bufhdr, hwidbtype* ihwidb)
{
    /*
     * VIP/4R port adapter uses TRIP's receive logic.  TRIP leaves
     * the token ring FCS in the MEMD buffer but does not count it
     * in bf_rx_bytes.  The 4R *does* count the FCS in the bf_rx_bytes.
     * Here we must adjust the length coming from the 4R so the rest of
     * the logic will work correctly (see CSCdj21539).
     */
    if (IS_VIP4R(ihwidb)) {
        bufhdr->bf_rx_bytes -= TR_FCS_SIZE;
    }

    /*
     * Determine if we should bridge this datagram.  Check the high
     * byte of AF status and see if only the bridge bit is set.
     * Bridging is checked before routing to enhance perceived
     * performance.  At the same time, check the low byte to insure
     * that this is not a MAC frame.  This check will manage to
     * ignore any MAC frames that may have come up with the bridging
     * bit.  (It is possible, but extremely rare.)
     */

    /*
     * Check TRIP address filter status to see if this is a Unicast
     * station matched IP datagram with mac length greater than 21,
     * and a TTL greater than 1, and no RIF is present.
     */
    if (bufhdr->bf_rx_status & RSP_DOD_IP_PROTOCOL_MASK) {
	iphdrtype   *ip;
	
	ip = (iphdrtype *)(bufhdr->bf_buffer_ptr + ihwidb->ip_memd_offset);

	if (ihwidb->ip_turbo_fs(bufhdr, ip, ihwidb) == IPFLOWOK)
	    return;
    }
    rsp_trip_fs_body(bufhdr, ihwidb);
}

static void
rsp_trip_fs_body (bufhdr_t* bufhdr, hwidbtype* ihwidb)
{
    ushort status;
    paktype *pak = fs_pak;

    status = bufhdr->bf_rx_status;
    stuff_fs_pak(bufhdr, ihwidb, pak);


    if ((status & (RSP_BRIDGING_MASK | RSP_STATION_MASK | RSP_MULTICAST_MASK |
	           CTR_RXSTAT_MAC | CTR_RXSTAT_BOTH)) ==
	 (RSP_BRIDGING_MASK | CTR_RXSTAT_SRB) ) {

        /*
	 * srb rif match (or explorer), bridging is on,
	 * no station match, no multicast match, not a mac frame:
	 * candidate for SRB fastswitching
	 */

	if (!is_tr_explorer_pak(pak)) {
            /*
             * should program an extra AF bit to detect explorers
             *
             * DON'T touch mac source addr, which reads in another
             *  cache line, unless necessary
             */
            pak->riflen = ((trrif_t *)pak->datagramstart)->rc_blth & RC_LTHMASK;
            pak->rif_match = NULL;
            pak->fcs_type = FCS_TYPE_TR;

            /*
             * call fastsrb routines
             */
            if ((*ihwidb->tr_srb_fastswitch)(ihwidb, (void *)pak,
                                             ihwidb->tr_bridge_idb)) 
                return;
        } else {
            /*
             * if its an explorer and we're in a storm - drop it
             */
	    if (reg_invoke_srt_flush_explorer(ihwidb, pak->datagramsize)) {
                rsp_fs_free_memd_pak(pak);
                return;
            }
        }
	/*
	 * its an explorer or SRB and not for our mac addrs
	 * but couldn't fast switch - send to srb_common()
	 */
	pak->rxtype = tring_decode_encaps(pak); /* ??? */

    } else if ((status & (CTR_RXSTAT_MAC | CTR_RXSTAT_SRB)) == 0) {

        /*
	 * not a TR mac frame, no srb _match_ (could still have a rif though)
	 */
	pak->rxtype = tring_decode_encaps(pak);

        if (status & RSP_STATION_MASK) {
	    /*
	     * packet for our mac unicast addr (with or without rif):
	     * fastswitch
	     * (don't allow multicasts/broadcasts in here - otherwise
	     *  srb stuff could will fall in)
	     */
	    if (reg_invoke_token_fs(pak->rxtype, pak)) 
		return;
	}

	/*
	 * bridge, broadcast and multicast traffic
	 */
	if (is_tr_srb_pak(pak)) {
	    /*
	     * srb frame (but no rif match)
	     */
	    if ( !((status & (RSP_STATION_MASK | RSP_MULTICAST_MASK)) || 
		   ( (is_tr_explorer_pak(pak)) && 
                     (srbV.srb_sb_get_thisring(ihwidb->firstsw)) )) ) {
		/*
		 * no mac addr or rif match - not explorer 
		 * drop it  (we don't tbride srb packets)
		 */
		rsp_fs_free_memd_pak(pak);
		return;
	    }
	    /* send any srb for us to process level */
	    
	} else {
	    
	    if (ihwidb->tbridge_on_hwidb) {
		/*
		 * Transparent bridging.
		 *
		 * Attempt to bridge any packet with the bridge bit set
		 * that does not also have the rif set.
		 * This attempt will succeed if the packet was forwarded,
		 * flooded, or filtered by the transparent bridge
		 * code.  This attempt will fail for a variety of packets
		 * including interesting multicasts (aliases will be
		 * flooded), and physical broadcasts in protocols we
		 * are routing (other physical broadcasts will be
		 * flooded)
		 */
		if (reg_invoke_tbridge_forward(pak))
		    return;
		/* send to process level */
	    } else {
		/*
		 * if not for us - drop it
		 */
		if ( !(status & (RSP_STATION_MASK | RSP_MULTICAST_MASK))) {
		    rsp_fs_free_memd_pak(pak);
		    return;
		}
	    }
	}

	/*
	 * if (status & RSP_MULTICAST_MASK)
	 *    {handle multicasting here: could have a rif on it}
	 */
	if (status & RSP_MULTICAST_MASK) {
	    /*
	     * Attempt to fastswitch IP multicasts.
	     */
	    if (pak->rxtype == RXTYPE_SNAP_DODIP) {
		if (reg_invoke_token_fs(pak->rxtype, pak))
		    return;
	    }
	}

        /*
         * check for RSRB, etc
         */
        if (pak->rxtype == RXTYPE_SRB) {
            if (rsp_fs_rsrb_direct_input(ihwidb, pak))
                return;
        } else if (ROUTING_RXTYPE(ihwidb->firstsw, pak->rxtype) &&
            pak->rxtype == RXTYPE_SNAP_DODIP) {

    	    /*
	     * if IP do RSRB, DLSw, or UDP flood
	     */
	        switch (((iphdrtype *)ipheadstart(pak))->prot) {
	            case UDP_PROT:
                        if (rsp_udp_flood_fs(ihwidb->fast_rcvidb, pak)) {
                            return;
                        }
		        break;

	            case FST_RSRB_PROT:
		        if (rsp_fs_rsrb_fst_input(ihwidb, pak)) {
		            return;
			}
		        break;	

                    case GRE_PROT:
                    case IPINIP_PROT:
                        if (reg_invoke_tunnel_ip_les_fs(pak)) {
                            return;
                        }
                        break;
                }
        }

    } else if (!(status & CTR_RXSTAT_MAC)) {
	/*
	 * if not a mac packet classify it
	 * send to process level
	 */
	pak->rxtype = tring_decode_encaps(pak); /* ??? */
    }

    pak->fcs_type = FCS_TYPE_TR;
    rsp_queue_trip_for_process(ihwidb, pak, status);
    return;
}

/*
 * rsp_serial_fs
 * Used directly for the FSIP and HIP
 * Called from rsp_mip_fs() for the MIP
 */

static void
rsp_serial_fs (bufhdr_t *bufptr, hwidbtype *input)
{
    ushort rxtype;

#ifdef DEBUG
    if (fs_pak->datagramsize > input->max_pak_size) {
	input->counters.giants++;
	input->counters.input_error++;
	if (rsp_debug)
	    buginf("\nReceived giant from FSIP/HIP %s, %d bytes",
		   input->hw_namestring, fs_pak->datagramsize);
	rsp_fs_free_memd_pak(fs_pak);
	return;
    }
#endif /* DEBUG */

    switch (input->enctype) {

    case ET_HDLC:
	rxtype = fs_pak->rxtype =
	    hdlc_decode_encapsulation_inline(fs_pak, (vhdlctype *)
					     fs_pak->datagramstart);

	/*
	 * Attempt to fast switch the packet.
	 * Protect against fast switching killer packet fragments
	 * smaller than the encapsulation size
	 */
	if (fs_pak->datagramsize > fs_pak->encsize) {
	    if (reg_invoke_hdlc_fs(rxtype, fs_pak))
		return;
	}
	break;

    case ET_FRAME_RELAY:
	if (reg_invoke_fr_pvc_switch_or_fs(fs_pak))
	    return;
	rxtype = fs_pak->rxtype;
	break;

    case ET_SMDS:
	rxtype = fs_pak->rxtype =
	    smds_decode_encapsulation_inline(fs_pak, fs_pak->datagramstart);

	/*
	 * Attempt to fast switch the packet
	 * Protect against fast switching killer packet fragments
  	 * smaller than the encapsulation size
	 */
	if (fs_pak->datagramsize > fs_pak->encsize) {
	    if (reg_invoke_smds_fs(rxtype, fs_pak)) {
		return;
	    }
	}
	break;

    case ET_PPP:
	rxtype = fs_pak->rxtype =
	    ppp_decode_encapsulation_inline(fs_pak, (ppp_hdr *)
					    fs_pak->datagramstart);

	/*
	 * Attempt to fast switch the packet
	 * Protect against fast switching killer packet fragments
  	 * smaller than the encapsulation size
	 */
	if (fs_pak->datagramsize > fs_pak->encsize) {
	    if (reg_invoke_ppp_fs(rxtype, fs_pak)) {
		return;
	    }
	}
	break;

    case ET_ATM_DXI:
	if (atm_dxi_classify_pak_and_fs_les(fs_pak))
	    return;
	rxtype = fs_pak->rxtype;
	break;

    default:
	rxtype = fs_pak->rxtype = RXTYPE_UNKNOWN;
	break;
    }

    /*
     * Handle receive frame special cases
     */
    switch (rxtype) {

    case RXTYPE_TRANSIT:
	/*
	 * Consider a frame for bridging
	 */
	if (input->tbridge_on_hwidb) {

	    /*
	     * Mark as bridge encapsulation
	     */
	    fs_pak->enctype = ET_BRIDGE;

	    /*
	     * Protect against bridging killer packet fragments
	     * smaller than the encapsulation size
	     */
	    if (fs_pak->datagramsize >= input->encsize) {
		if (reg_invoke_tbridge_forward(fs_pak)) {
		    return;
		}
	    }
	}
	break;

    case RXTYPE_SRB:
	/*
	 * Consider a frame for direct RSRB
	 */
        if (rsp_fs_rsrb_direct_input(input, fs_pak))
	    return;
	break;

    case RXTYPE_DLSW:
        /*
         * Consider a frame for direct DLSW
         */
        if (rsp_fs_dlsw_direct_input(input, fs_pak))
            return;
        break;

    case RXTYPE_LEX:
	/*
	 * Consider a frame from lex
	 */
	{ 
	    idbtype *lex_swidb = input->firstsw->lex_swidb;

	    /*
	     * Lex not bound to serial
	     */
	    if (!lex_swidb) {
		rsp_fs_free_memd_pak(fs_pak);
		return;
	    }

	    /*
	     * Only bridge protocols that we are not routing.
	     */
	    if (BRIDGING_RXTYPE(input->firstsw, fs_pak->rxtype)) {

		/*
		 * Is there a bridge group on the lex interface.
		 */
		if (RUNNING_IF(LINK_BRIDGE, lex_swidb)) {

		    /*
		     * Mark as bridge encapsulation
		     */
		    fs_pak->enctype = ET_BRIDGE;

		    /*
		     * Change the input interface so that it points to
		     * the lex interface.
		     */
		    fs_pak->if_input = lex_swidb;

		    if (reg_invoke_tbridge_forward(fs_pak))
			return;

		    fs_pak->if_input = input->firstsw;
		}
	    }
	}
	break;

    case RXTYPE_DODIP:
	/*
	 * Consider a frame for RSRB, etc
	 */
	if (ROUTING_IF(input->firstsw, LINK_IP)) {
	    iphdrtype *ip = (iphdrtype *)ipheadstart(fs_pak);
	    switch (ip->prot) {

                case UDP_PROT:
                    if (rsp_udp_flood_fs(input->fast_rcvidb, fs_pak)) {
                        return;
                    }
		    break;

	        case FST_RSRB_PROT:
		    if (input->enctype == ET_HDLC  &&
			rsp_fs_rsrb_fst_input(input, fs_pak))
		        return;
		    break;
                case GRE_PROT:
                case IPINIP_PROT:
                    if (reg_invoke_tunnel_ip_les_fs(fs_pak)) {
                        return;
                    }
		    break;
	    }
	}
	break;
		
    default:
	break;
    }
	    
    /*
     * We did not fast switch the serial datagram.
     * Queue the packet for process level:
     */

    rsp_queue_serial_for_process(input, fs_pak);
    return;
}

/*
 * Quick check to see if we can optimum/flowswitch this puppy.
 * This is non-optimal for other protocols, but this is high
 * priority for HSSI in an Internet Service Provider environment.
 */
static inline int
rsp_serial_ip_turbo_fs (bufhdr_t *bufptr, hwidbtype *input)
{
    ushort etype, *proto_ptr;
    iphdrtype *ip;

    proto_ptr = (ushort *)(bufptr->bf_buffer_ptr + input->ip_memd_offset);
    etype = proto_ptr[-1];

    /*
     * Check to see if we can optimum switch the input packet,
     * verifying input interface encapsulation and packet
     * encapsulation agree, and verifying minimum input length
     */
    if (((input->fast_switch_type == FS_HDLC) && (etype == TYPE_IP10MB) &&
	 (bufptr->bf_rx_bytes > (HDLC_ENCAPBYTES + MINIPHEADERBYTES)) ) ||

	((input->fast_switch_type == FS_PPP) && (etype == TYPE_PPP_IP) &&
	 (bufptr->bf_rx_bytes > (PPP_ENCAPBYTES + MINIPHEADERBYTES)) ) ||

	((input->fast_switch_type == FS_SMDS) && (etype == TYPE_IP10MB) &&
	 (bufptr->bf_rx_bytes > (SMDS_SNAP_ENCAPBYTES + MINIPHEADERBYTES)) &&
	 (!(input->ip_fast_flags & IP_FAST_INPUTACCESSLISTS)) ) ||

	((input->fast_switch_type == FS_FR) &&
	 (etype == FR_IETF_IP || etype == TYPE_IP10MB) &&
	 (bufptr->bf_rx_bytes > (FRAME_RELAY_ENCAPBYTES + MINIPHEADERBYTES)) &&
	 (!(input->ip_fast_flags & IP_FAST_INPUTACCESSLISTS)) ) )
      {
	ip = (iphdrtype *)proto_ptr;
	if ((ip->ttl > 1) && (*(uchar *)ip == IPV4_LEN5))
	    return(input->ip_turbo_fs(bufptr, ip, input));
      }
    return(IPFLOWERR_NOTCONF);
}

static inline iphdrtype *
rsp_atm_turbo_iphdr (bufhdr_t *bufptr, hwidbtype *input)
{
    iphdrtype   *ip;
    atm_encap_t	*atm;
    
    atm = (atm_encap_t *) (bufptr->bf_buffer_ptr + input->ip_memd_offset);
    if (atm->enctype == TYPE_IP10MB) {
	ip = (iphdrtype *)atm->data;
	if ((ip->ttl > 1) && (*(uchar *)ip == IPV4_LEN5) &&
	    (bufptr->bf_rx_bytes >
	     (ATM_ENCAP_HDRBYTES + MINIPHEADERBYTES) ) )
	  return(ip);
    } else if (atm->enctype == ATM_ENCTYPE_SNAP) {
	atm_aal5snap_t *atm_snap = (atm_aal5snap_t *) atm;
	snap_hdr *snap = &atm_snap->snap;
	ushort dsapssap = *(ushort *)&snap->dsap;

	if ((dsapssap == SNAPSNAP) &&
	    (snap->type == TYPE_IP10MB) &&
	    (bufptr->bf_rx_bytes >
	     (sizeof(atm_aal5snap_t) + MINIPHEADERBYTES) ) ) {
	    ip = (iphdrtype *)snap->data;
	    if ((ip->ttl > 1) && (*(uchar *)ip == IPV4_LEN5))
	      return(ip);
	}
    }
    return(NULL);
}


static void check_ip_invalidate (ulong ip)
{
    ulong       status;

    if ((ip & (R4K_DLINE_SHIFT - 1)) <= (R4K_DLINE_SHIFT - MINIPHEADERBYTES))
	return;

    ip += R4K_DLINE_SHIFT;
    ip &= ~(R4K_DLINE_SHIFT - 1);

    /*
     * Efficiently mask all interrupts to prevent timer interrupts
     * between 'cache' instruction bug workaround and cache instruction!
     * Protect against 2.0 Orion 'cache' instruction bug.
     */
    GLOBAL_INTERRUPT_BLOCK(status);
    r4k_CACHE_instruction_fix();

    asm volatile(".set noreorder");
    asm volatile(".set mips3");
 
    asm volatile(" nop");   /* Ensure 4 cycles w/o data cache reference */
    asm volatile(" cache "HIT_INVALIDATE_DATA",0(%0)"
	: /* no output */
	: "r" (ip));
 
    asm volatile(".set mips2");
    asm volatile(".set reorder");

    /*
     * Restore interrupt enable state:
     */
    GLOBAL_INTERRUPT_UNBLOCK(status);
}

static inline void
rsp_aip_fs (bufhdr_t *bufptr, hwidbtype *input)
{
    iphdrtype   *ip;

    if ((ip = rsp_atm_turbo_iphdr(bufptr, input)) != NULL) {
	int status;

	status = input->ip_turbo_fs(bufptr, ip, input);

	/*
	 * optimum switching only invalidate first cache line if
	 * no access list is set to get the best performance.
	 * Certain encap such as ATM LANE have longer encap which
	 * make part of IP header in the second cache line without
	 * being invalidated. 
	 * check_ip_invalidate checks whether part of IP header is
	 * in the second cache and invalidate it if that is the case.
	 */
	check_ip_invalidate((ulong)ip);
	if (status == IPFLOWOK)
	    return;
    }
    rsp_aip_fs_body(bufptr, input);
}
    
static void
rsp_aip_fs_body (bufhdr_t *bufptr, hwidbtype *input)
{
    atm_encap_t	*atm;
    vcd_t	vcnum;
    vc_info_t	*vc;
    ushort	rxtype;
    boolean     lane_flush;
    boolean     try_tbridge;

    stuff_fs_pak(bufptr, input, fs_pak);
#ifdef DEBUG
    if (fs_pak->datagramsize > input->max_pak_size) {
	input->counters.giants++;
	input->counters.input_error++;
	if (rsp_debug)
	    buginf("\nReceived giant from AIP %s, %d bytes",
		   input->hw_namestring, fs_pak->datagramsize);
	rsp_fs_free_memd_pak(fs_pak);
	return;
    }
#endif /* DEBUG */
    atm = (atm_encap_t *) fs_pak->datagramstart;
    vcnum = atm->vcnum;
    vc = atm_getvc_inline(input->atm_db, vcnum);
    
    /* Save for fs & lane */
    input->atm_db->fast_vc_info = fs_pak->atm_vc = vc;	

    fs_pak->atm_vcid = vcnum;

    /*
     * Try to fast switch or bridge the packet only if there is a valid VC
     * OAM frames come in with VC==0, and need to be passed up to process
     * level
     */
    if (vc) {
	
 	if (fs_pak->datagramsize > (vc->swidb->sub_mtu +
				    ETHER_ARPA_ENCAPBYTES +
				    ATM_SMDS_HDRBYTES)) {
	    datagram_done(fs_pak);
	    return;
 	}
	fs_pak->if_input = vc->swidb;
        lane_flush  = FALSE;
        try_tbridge = FALSE;
	rxtype = fs_pak->rxtype =
	    atm_decode_encapsulation_inline(fs_pak, atm, vc,
                                            &lane_flush, &try_tbridge);

        /*
         * Decode will set try_tbridge if this is a LANE data packet to
         * multicast MAC or unicast MAC other than me.  If not LANE, or
         * packet is sent to our MAC, we can skip bridging & go straight
         * to fastswitch routing.  This optimizes for most common routing
         * cases.  This code is patterned after rsp_eip_fs() in
         * src-rsp/rsp_if.c, with the answer from atm_decode taking the
         * place of the hardware address filter.
         * 
         * See comments in atm_decode for how rxtype & booleans are set.
         */
        if (try_tbridge) {
            if (!vc->swidb->tbridge_on_swidb) {
                if (lane_flush) {
                    /*
                     * Is LANE with dst unicast MAC other than me.  When
                     * bridging not configured, must toss.
                     */
                    datagram_done(fs_pak);
                    return;
                }
                /*
                 * bridging not configured, was LANE with dst multicast MAC,
                 * fall through to fastswitching.
                 */
            } else {
                /*
                 * Is LANE with dst multicast MAC or unicast other than me.
                 * Let bridging figure out what to do with it.
                 */
                fs_pak->atm_vcid    = vcnum;
                fs_pak->atm_enctype = vc->etype;

		/*
		 * Protect against forwarding killer packet fragments
		 * smaller than the encapsulation size
		 */
		if (fs_pak->datagramsize >= fs_pak->encsize) {
		    if (reg_invoke_tbridge_forward(fs_pak)) {
			return;
		    }
                }
                /*
                 * fall through to fast-switching if bridging didn't do
                 * something with packet.
                 */
            }
        }

	/*
	 * Attempt to fast switch the packet:
	 * Protect against forwarding killer packet fragments
	 * smaller than the encapsulation size
	 */
	if (fs_pak->datagramsize > fs_pak->encsize) {
	    if (reg_invoke_aip_fs(rxtype, fs_pak)) {
		vc->infast++;
		ATM_INFAST(input);
		return;
	    }
	}
	
        /*
         * If LANE data packet & decode said throw it away, toss it.
         * Fastswitching attempt will have failed, because decode sets
         * RXTYPE_LANE_CONTROL when it sets lane_flush, and lanefast
         * doesn't do anything with LANE data packets.
         */
        if (lane_flush && !try_tbridge) {
            datagram_done(fs_pak);
            return;
        }

	/*
	 * Check for direct encaps RSRB:
	 */
	if (rxtype == RXTYPE_SRB) {
	    if (rsp_fs_rsrb_direct_input(input, fs_pak)) {
		vc->infast++;
		ATM_INFAST(input);
	        return;
	    }
	}
	/*
	 * If we are routing IP, check to see if this frame is
	 * an UDP broadcast, RSRB, etc.
	 */
	if (ROUTING_RXTYPE(input->firstsw, RXTYPE_DODIP) &&
	    (rxtype == RXTYPE_DODIP)) {
	    iphdrtype *ip;
	    ip = (iphdrtype *) fs_pak->network_start;
	    switch (ip->prot) {

	    case FST_RSRB_PROT:
		if (rsp_fs_rsrb_fst_input(input, fs_pak)) {
		    vc->infast++;
		    ATM_INFAST(input);
		    return;
		}
		break;

            case GRE_PROT:
            case IPINIP_PROT:
                if (reg_invoke_tunnel_ip_les_fs(fs_pak)) {
                    vc->infast++;
                    ATM_INFAST(input);
                    return;
                }
                break;
	    }
	}	    

	/*
	 * Is this a bridgeable packet? If so, try to bridge the input packet:
	 */
	if (vc->etype == ATM_ENCTYPE_SNAP) {
	    if (vc->swidb->tbridge_on_swidb) {
                atm_bridge_ether_hdr *bridge_hdr = (atm_bridge_ether_hdr *)
                    (fs_pak->datagramstart);
 
                if (ieee_equal_oui(bridge_hdr->oui, atm_tbridge_oui)) {
 
                    if (reg_invoke_tbridge_forward(fs_pak)) {
                        vc->infast++;
                        return;
                    }
		    else if (is_bvi(fs_pak->if_input->hwptr)) {
		   	/*
			 * give a second chance for IRB routable
			 * packets to do fast switch to lessen the 
			 * process switch load. Those pkts never
			 * get successfully fast switched above
			 * since the rxtype is not classified
			 * until they go through the bridging
			 * fast path. That's the reason we use
			 * fs_pak->rxtype as the argument instead of
			 * the local rxtype
			 */
			if (reg_invoke_aip_fs(fs_pak->rxtype, fs_pak)) {
	    		    vc->infast++;
	    		    ATM_INFAST(input);
	    		    return; 
			}
		    }
                }
            }
        }
    } else {
	fs_pak->if_input = input->firstsw;
    }

    /*
     * Else, pass up to process level:
     */
    input->atm_db->fast_vc_info = vc;
    rsp_queue_atm_for_process(input, fs_pak, atm);
    return;
}

static void
rsp_mip_fs (bufhdr_t *bufptr, hwidbtype *input)
{
    hwidbtype *actual_hwidb;
    ushort  vc;

    /*
     * Packets from the MIP have two bytes worth of vc, and then
     * look just like they came from an FSIP.  The first step is
     * to extract the vc, and then skip past it by adjusting
     * datagramstart and datagramsize.
     *
     * Note, all switching modes will invalidate the line
     * holding the vc field, so it's safe to access it via
     * cached space, safe in the knowledge, that the switching
     * paths will invalidate it for us when they're done.
     */
    vc = *(ushort *)fs_pak->datagramstart;

    /*
     * N.B. On incoming data, the MIP places  some encaps info for the SSE
     * into the top byte of the vc number.  So we mask this out for the
     * purpose of locating the hwidb.
     */
    vc &= 0xff;
    fs_pak->datagramstart += MIP_ENCAPSIZE_BYTES;
    fs_pak->datagramsize -= MIP_ENCAPSIZE_BYTES;

    /*
     * Note that the hwidb we were passed, is the 'base' hwidb,
     * not the actual hwidb associated with this channel group.
     * We now use the vc, to locate the real hwidb associated
     * with this channel group.
     */
    actual_hwidb = mip_list[input->mci_index]->hwidb[vc];
    if (actual_hwidb == NULL) {
	errmsg(&msgsym(NOIDB, RSP), vc, input->hw_namestring);
	rsp_fs_free_memd_pak(fs_pak);
	return;
    }

    /*
     * Now we need to fix some of the fields in the fs_pak,  that
     * were incorrectly set by rsp_input_raw just before it called us.
     * The reason it got these wrong, was because it was using the
     * 'base' hwidb, not  the real one that we've just located.
     */
    fs_pak->if_input = actual_hwidb->firstsw;


    /*
     * Finally, we have the real hwidb, we've adjusted the packet to
     * 'remove' the MIP vc number, and so we finally look just like
     * any other serial interface.  We can now launch straight into
     * the fastswitch code for the FSIP and HSSI.
     *
     * XXX You may need to clone this if you want to count
     * the vc bytes in your stats? Either that, or just inc the
     * counts here, if that is feasable? XXX
     *
     * Also, try to turbo IP switch it before calling generic serial code.
     */
    if (rsp_serial_ip_turbo_fs(bufptr, actual_hwidb) != IPFLOWOK)
	rsp_serial_fs(bufptr, actual_hwidb);
}

static void
rsp_cip_fs (bufhdr_t *bufptr, hwidbtype *input)
{
    channel_hdr	*chan;
    ushort	rxtype;

#ifdef DEBUG
    if (fs_pak->datagramsize > input->max_pak_size) {
	input->counters.giants++;
	input->counters.input_error++;
	if (rsp_debug)
	    buginf("\nReceived giant from CIP %s, %d bytes",
		   input->hw_namestring, fs_pak->datagramsize);
	rsp_fs_free_memd_pak(fs_pak);
	return;
    }
#endif /* DEBUG */
    
    chan = (channel_hdr *) fs_pak->datagramstart;

    rxtype = fs_pak->rxtype =
	channel_decode_encapsulation_inline(fs_pak, chan);

    /* 
     * If (rxtype == RXTYPE_UNKNOWN)
     *   check for CHANNEL_ENCAPS_VLAN and fastswitch for CIP VLANs
     * Else 
     *   Attempt to fastswitch packet based on "rxtype"
     *
     */
    if (rxtype == RXTYPE_UNKNOWN) {

        if (chan->type == CHANNEL_ENCAPS_VLAN) {
            hwidbtype *idb = NULL;
            channel_sb_t *chan_sb = channel_get_sb(input);
            if (chan_sb) {
                idb = chan_sb->get_hwidb_from_vc(input, chan->vc);
            }
            if (idb) {
                input = idb;       /* Save CIP ILAN hwidb */
            } else {
                if (srberr_debug) {
                  buginf("\nCIP pak dropped: no hwidb_from_vc");
                }
                input->counters.input_drops++;
                rsp_fs_free_memd_pak(fs_pak);
                return;
            }

            if (chan->media == CIP_VLAN_TOKEN) {

                fs_pak->if_input = idb->firstsw;

                fs_pak->flags |= PAK_SRB_IN;
                fs_pak->datagramsize -= CHANNEL_TOKEN_ENCAPBYTES;
                fs_pak->datagramstart += CHANNEL_TOKEN_ENCAPBYTES;
                fs_pak->network_start = fs_pak->datagramstart;

                fs_pak->riflen = get_tr_riflen_pak(fs_pak);
                fs_pak->rif_match = NULL;
                fs_pak->fcs_type = FCS_TYPE_NONE;

                if (!is_tr_explorer_pak(fs_pak)) {

                    /* 
                     * Since packet is not an explorer,
                     *   try to fastswitch the packet
                     * 
                     * If fastswitch fails, 
                     *   packet will booted up to be process switched
                     * 
                     */
                    if ((*idb->tr_srb_fastswitch)(idb, (void *)fs_pak, 
                                                  idb->tr_bridge_idb)) {
                        if (srb_debug) {
                            SrbDebug_FastForward(idb);
                        }
                        return;
                    } else {
                       /*
                        * Set linktype to LINK_SRB as indication
                        *   to rsp_queue_channel_for_process() that pak is 
                        *   from CSNA ILAN which requires special processing
                        *   
                        */
                        fs_pak->linktype = LINK_SRB;
                    }

                } else {

                    /* 
                     * Explorer packet will eventually be booted up 
                     *   to process level for handling
                     *
                     */
                    if (reg_invoke_srt_flush_explorer(idb, 
                                                      fs_pak->datagramsize)) {
                        rsp_fs_free_memd_pak(fs_pak);
                        return;
                    }

                    fs_pak->linktype = LINK_SRB;
                } /* End else - Handle the explorer packet */

            } else if (chan->media == CIP_VLAN_FDDI) {

                if (srberr_debug) {
                  buginf("\nCIP pak dropped: CIP ILAN(FDDI) not supported");
                }
                idb->counters.input_drops++;
                rsp_fs_free_memd_pak(fs_pak);
                return;

            } else if (chan->media == CIP_VLAN_ETHER) {

                if (srberr_debug) {
                  buginf("\nCIP pak dropped: CIP ILAN(Ethernet) not supported");
                }
                idb->counters.input_drops++;
                rsp_fs_free_memd_pak(fs_pak);
                return;

            } else {    /* Internal CIP LAN is Unknown */

                if (srberr_debug) {
                  buginf("\nCIP pak dropped: CIP ILAN(Unknown) not supported");
                }
                idb->counters.input_drops++;
                rsp_fs_free_memd_pak(fs_pak);
                return;

            } /* End else - internal CIP LAN is Unknown */

        } else {     /* Unknown ENCAPS type */

            errmsg(&msgsym(UNKENC, CBUS), input->hw_namestring, chan->media);
            input->counters.input_drops++;
            rsp_fs_free_memd_pak(fs_pak);
            return;

        } /* End else - Unknown ENCAPS type */

    } else {

        /*
         * "rxtype" is known
         *   attempt to fast switch the packet based on "rxtype"
         */
        if (reg_invoke_channel_fs(rxtype, fs_pak)) {
            return;
        }

    } 

    /*
     * Pass packet up to process level 
     *
     */
    rsp_queue_channel_for_process(input, fs_pak, chan);

}

/*
 * handle corrupt love notes
 */
static void
null_lovenote_handler (hwidbtype *hwidb, lovenotetype *note)
{
    ulong *ptr;

    ptr = (ulong*) note;
    errmsg(&msgsym(LOVENOTE, RSP),
	   ADDR_TO_MEMD_OFFSET_16(note), ptr[0], ptr[1], ptr[2], ptr[3]);
    return;
}

/*
 * rsp_pool_item_ret
 *
 * Return a MEMD buffer from a retbuffer() call
 */

static void rsp_pool_item_ret (pooltype *pool, void *item)
{
    paktype *pak = item;

    /*
     * Hand the attached data back to MEMD
     */
    rsp_fs_free_memd_pak(pak);
}

/*
 * rsp_pool_item_get
 *
 * Stub function that'll crash if ever someone does a getbuffer()
 * on this pool.
 */

static void *rsp_pool_item_get (pooltype *pool)
{
    /*
     * Bobby don't dance.
     */
    crashdump(0);

    /*
     * We never make it here...
     */
    return(NULL);
}

/*
 * Initialize the static parts of the
 * fast switching packet structure:
 */

static paktype dummy;

static void
pak_init (void)
{
    int pool_group;
    pooltype *pool;

    /*
     * Set up pointers for MEMD bounds checking
     */
    memd_buffer_data_start =
	((uint *) (ADRSPC_MEMD_UNCACHED + MEMD_BUFDATA_OFFSET));
    memd_end =
	((uint *) (ADRSPC_MEMD_UNCACHED + memd_size));

    /*
     * Allocate our static fast switching packet structure:
     */
    fs_pak = malloc_aligned(sizeof(paktype), RSP_DCACHE_LINE_SIZE);
    rsp_flood_fs_pak = malloc_aligned(sizeof(paktype), RSP_DCACHE_LINE_SIZE);
    if (!fs_pak || !rsp_flood_fs_pak) {
	crashdump(2);
    }

    /*
     * We need to be the only one holding onto this buffer:
     */
    fs_pak->refcount = 1;
    rsp_flood_fs_pak->refcount = 1;

    /*
     * Flag this packet as not doing input queue accounting, and
     * enable use of the DIRTY flag on this packet structure
     */
    fs_pak->buffer_flags = PAK_BUF_IGNOREQ | PAK_BUF_USE_DIRTY |
	PAK_BUF_DO_NOT_Q;

    /*
     * Ensure that other software cannot successfully enqueue our input
     * MEMD buffer onto an input queue, or onto an output hold queue.
     * (This is used in combination with the PAK_BUF_DO_NOT_Q flag.)
     */
    fs_pak->next = &dummy;
    dummy.next = &dummy;
    dummy.datagramsize = 0;
    dummy.datagramstart = 0;
    dummy.flags = 0;
    dummy.buffer_flags = PAK_BUF_PERMANENT;

    rsp_flood_fs_pak->buffer_flags = PAK_BUF_IGNOREQ | PAK_BUF_USE_DIRTY |
	PAK_BUF_DO_NOT_Q;
    rsp_flood_fs_pak->next = &dummy;

    /*
     * Initialize a packet pool for this static fast switched packet struct:
     */
    pool_group = pool_create_group();
    pool = pak_pool_create("fs_pak", pool_group, 0,
			       POOL_PUBLIC_DUPLICATE | POOL_INVISIBLE, NULL);
    if (!pool)
	crashdump(0);

    /*
     * Modify the default vectors
     */
    pool->item.get = rsp_pool_item_get;
    pool->item.ret = rsp_pool_item_ret;
    pool->encapsize = ENCAPBYTES;

    fs_pak->pool = pool;
    rsp_flood_fs_pak->pool = pool;
}

/*
 *	rsp_red_set_precedence
 *
 *	set precedence levels when RED is activated. RED is configured
 *	in prioritygroup_command() to manage down to a holdq that is
 *	half full. On the RSP, "half of the queue" needs to take into
 *	account the interface card's onboard queue, which may be larger
 *	than the hold queue in main memory.
 */
static void rsp_red_set_precedence (hwidbtype *idb) {
    int total_queue;
    int half_of_queue;
    red_parameters *redp = idb->red_parameters;
    if (redp) {
	total_queue = redp->saved_threshold + idb->mci_txcount;
	half_of_queue = total_queue/2 - idb->mci_txcount;
	red_set_precedence_levels(idb, half_of_queue > 0? half_of_queue: 1);
    }
}
/*
 * Initialize RSP specific info needed for fast switching:
 */
#ifdef DEBUG
ulong	invalid_qa_interrupts;	/* Times interrupted with nothing to do */
ulong	full_txq_count;		/* Times rsp_tx_start() found full TX queue */
ulong	full_returnq_data;	/* Times we could get no return q struct */
#endif

void
rsp_fs_init (void)
{
#ifdef DEBUG
    disable_auto_interface_restart = TRUE;
    rsp_debug = TRUE;
    invalid_qa_interrupts = 0;
#endif

    pak_init();

    nullidb->hwptr->love_note = null_lovenote_handler;

    queue_init(&rsp_small_buffers, RSP_MAX_BUFFERS);
    queue_init(&rsp_large_buffers, RSP_MAX_BUFFERS);

    netint_throttle_init();

    reg_add_if_onesec(rsp_check_if_restart, "rsp_check_restart_req");

    TIMER_STOP(last_complex_restart);
    reg_add_onesec(rsp_check_complex_restart, "rsp_check_complex_restart");

    /*
     * Set up the 'return rx buffer' mechanism.  This is used to
     * periodically request that each plughole returns an rx buffer
     * from it's local free queue back to the global free queue.
     */
    return_rx_buffer_slot = 0;             /* Start at the beginning */
    reg_add_onesec(rsp_return_rx_buffer, "rsp_return_rx_buffer");
    reg_add_interface_cfg_modified(rsp_red_set_precedence, "rsp_red_set_precedence");
}

/*
 * rsp_dump_memd_buffer
 * Print out the first MEMD_BUF_INTS of the MEMD buffer,
 * if the buffer header appears to be valid
 *
 * Always access MEMD via uncached address space to avoid
 * any cache coherency considerations.
 * Do bounds checking to ensure that we do not try to go off the
 * end of MEMD while dumping the MEMD buffer contents.
 */

#define	MEMD_BUF_LINES		6
#define MEMD_INT_PER_LINE	6

void
rsp_dump_memd_buffer (bufhdr_t *bufptr)
{
    uint	*loc;
    int		lc;

    loc = MEMD_OFFSET_TO_ADDR(bufptr->bf_buffer_ptr);
    loc = ADDR_MEMD_TO_UNCACHED(loc);
    if (in_memd_buffer_data(loc)) {
	for (lc = 0; lc < MEMD_BUF_LINES; lc++) {

	    /*
	     * Ensure that entire line is within MEMD
	     * before trying to print it out
	     */
	    if (!in_memd_buffer_data(&loc[MEMD_INT_PER_LINE])) {
		errmsg(&msgsym(BADBUFHDR, RSP), "Past end of MEMD",
		       (ulong) &loc[MEMD_INT_PER_LINE]);
		return;
	    }

	    errmsg(&msgsym(DMPMEM, SYS), loc,
		   loc[0], loc[1], loc[2], loc[3], loc[4], loc[5]);
	    loc += MEMD_INT_PER_LINE;
	}
    } else {
	errmsg(&msgsym(BADBUFHDR, RSP), "Invalid buffer ptr",
	       (ulong) loc);
    }
}

static inline void
rsp_bad_fs (bufhdr_t *bufptr, hwidbtype *input)
{
    if (input == nullidb->hwptr) {
	ulong* ulp;

	/* buffer header likely trashed, complain and drop it */
	ulp = (ulong*) bufptr;
	errmsg(&msgsym(XBUFHDR, RSP),
	       (ulong) bufptr, ulp[0], ulp[1], ulp[2], ulp[3]);
	rsp_dump_memd_buffer(bufptr);
	if (++lost_memd_buffers > MAX_LOST_MEMD_BUFFERS)
	    platform_req_complex_restart(NULL);
    } else {
	errvarmsg(&msgsym(ERROR, RSP), 
	          "unknown input interface type %d (%s)",
	          input->type, input->hw_namestring);
	retbufhdr_inline(bufptr);
    }
}

/*
 * rsp_input_raw
 * Input packets from the RawQ
 */

static inline void
rsp_input_raw (bufhdr_t *bufptr)
{
    hwidbtype	*input;
    
    /*
     * Find out what kind of interface the input interface is
     */
    input = LOCAL_QUEUE_TO_HW_IDB(bufptr->bf_queue_ptr);
#ifdef DEBUG
    /*
     * Sanity check buffer data MEMD offset:
     */
    if ( (bufptr->bf_buffer_ptr >= MEMD_BUFDATA_OFFSET) &&
	(bufptr->bf_buffer_ptr < memd_size) ) {
#endif /* DEBUG */

	/*
	 * Switch on 'input->type' since we will need to support different
	 * fast switching code for the classic 7000 IPs, and for the VIPer
	 * interfaces:
	 */
	switch (input->type) {

	case IDBTYPE_CBUSETHER:
	case IDBTYPE_FEIP:
	    rsp_eip_fs(bufptr, input);
	    break;

	case IDBTYPE_FDDIT:
	    rsp_fip_fs(bufptr, input);
	    break;

	case IDBTYPE_CTR:
	    rsp_trip_fs(bufptr, input);
	    break;

	case IDBTYPE_CBUSSERIAL:
	case IDBTYPE_HSSI:
	    if (rsp_serial_ip_turbo_fs(bufptr, input) == IPFLOWOK)
	      break;
	    stuff_fs_pak(bufptr, input, fs_pak);
	    rsp_serial_fs(bufptr, input);
	    break;

	case IDBTYPE_AIP:
	    rsp_aip_fs(bufptr, input);
	    break;

	case IDBTYPE_T1:
	case IDBTYPE_E1:
	case IDBTYPE_BRI:
	    stuff_fs_pak(bufptr, input, fs_pak);
	    rsp_mip_fs(bufptr, input);
	    break;

	case IDBTYPE_CIP:
	case IDBTYPE_CIP_LAN_ADAPTER:
	    stuff_fs_pak(bufptr, input, fs_pak);
	    rsp_cip_fs(bufptr, input);
	    break;

	default:
	    rsp_bad_fs(bufptr, input);
	    break;
	}

#ifdef DEBUG
    } else {

	ulong* ulp;

	/*
	 * Bad MEMD offset for start of buffer data,
	 * so complain and dump part of MEMD buffer for debugging
	 */
	buginf("\nrsp_input_raw: bad buffer MEMD offset in buffer header:");
	/* buffer header likely trashed, complain and lose it */
	ulp = (ulong*) bufptr;
	errmsg(&msgsym(XBUFHDR, RSP),
	       (ulong) bufptr, ulp[0], ulp[1], ulp[2], ulp[3]);
	rsp_dump_memd_buffer(bufptr);
	if (++lost_memd_buffers > MAX_LOST_MEMD_BUFFERS)
	  platform_req_complex_restart(NULL);
    }
#endif /* DEBUG */
    return;
}

/*
 * Arrange for the IP to return this buffer header/memd buffer
 * via the ReturnQ, so that we have an opportunity to restart
 * output on this interface when it is returned to us
 * NOTE: must be called with net interrupts disabled or from net intr level
 *       also once called the bufhdr _must_ be enqueued or resources
 *       will be lost
 */

void
rsp_tx_restart_via_returnq (hwidbtype *hwidb, bufhdr_t *bufptr)
{
    return_data_t	*retd;
    ushort		bcast_id;

    /*
     * Only allow one outstanding restart request at a time per interface
     */
    if (hwidb->tx_restart_active)
	return;

    /*
     * Sanity checking on input buffer header
     */
    if (bufptr->bf_queue_ptr == 0) {
	ulong	*ulp;
	ulp = (ulong*) bufptr;
	errmsg(&msgsym(INVQPTR, RSP), bufptr->bf_queue_ptr,
	       (ulong) bufptr, ulp[0], ulp[1], ulp[2], ulp[3]);
	rsp_dump_memd_buffer(bufptr);
    }

    /*
     * Doesn't need protected dequeue because rsp_tx_restart_via_returnq()
     * must be called from NETS_DISABLE context
     */
    retd = dequeue(&free_retdatQ);	/* Get free return data struct */
    if (retd) {

	/*
	 * Save our broadcast ID in the buffer header,
	 * and get our return queue data struct
	 */
	bcast_id = retd->id;

	/*
	 * save original q ptr and bufhdr in local struct
	 */
	retd->queue_ptr = bufptr->bf_queue_ptr;
	retd->bufhdr_offset = ADDR_TO_MEMD_OFFSET_16(bufptr);

	/*
	 * put return id into bufhdr (and original free q ptr - not needed
	 *   but may help in debugging dump)
	 *
	 * change bufhdr queue ptr to return queue
	 */
	bufptr->bf_return_info = ((bufptr->bf_queue_ptr << 16) | bcast_id);
	bufptr->bf_queue_ptr = ADDR_TO_MEMD_OFFSET_16(ReturnQ);
  
	/*
	 * Set the type value to RETQ_TX_RESTART
	 */
	retd->type = RETQ_TX_RESTART;

	/*
	 * Save the HW IDB:
	 */
	retd->hwidb = hwidb;

	/*
	 * We've now got a TX restart request outstanding
	 */
	hwidb->tx_restart_active = TRUE;
	GET_TIMESTAMP(hwidb->tx_restart_time);

    } else {
	errmsg(&msgsym(NORESTART, RSP), hwidb->hw_namestring);
    }
}

/*
 * rsp_input_return
 * Input packets from the ReturnQ
 */

static inline void
rsp_input_return (bufhdr_t *bufptr)
{
    return_data_t	*retd;
    hwidbtype		*hwidb;
    ushort		bcast_id;

    /*
     * Rather than invalidating the buffer header's cache line,
     * it's usually more efficient to access uncached.
     * This is because we can get all the info we need in
     * a single read.
     */
    bufptr = ADDR_MEMD_TO_UNCACHED(bufptr);

    /*
     * Read all potential return queue info as uncached long word read,
     * so we can avoid having to deal with invalidating a cache line,
     * or doing multiple uncached reads.
     */
    bcast_id = (bufptr->bf_return_info) & 0xFFFF;

    if (bcast_id < RETURNQ_SIZE) {
	ushort              freeq;

	/*
	 * Find the return queue data structure
	 */
	retd = &return_data[bcast_id];
	freeq = retd->queue_ptr;

	/*
	 * Sanity check the buffer header and
	 * the retd structure before using them
	 *
	 * Ensure that the retd structure and the buffer header match,
	 * verify that the saved freeq pointer is not NULL, and
	 * make sure that the retd structure isn't on the retd free list
	 */
	if ( (retd->bufhdr_offset == ADDR_TO_MEMD_OFFSET_16(bufptr)) &&
	     (freeq != 0) &&
	     (retd->next == NULL) ) {

	    /*
	     * Check the return queue action code
	     */
	    switch(retd->type) {

	    case RETQ_TX_RESTART:
		/*
		 * Restart output from holdq on specified interface
		 */
		hwidb = retd->hwidb;
		if (hwidb) {
		    hwidb->tx_restart_active = FALSE;
		    rsp_tx_start(hwidb);
		} else
		    break;	/* Fall into error case for debug */

		/*
		 * Clear and return the retd structure
		 */
		retd->type = RETQ_INVALID;
		retd->bufhdr_offset = 0;
		enqueue(&free_retdatQ, retd);

		/*
		 * Restore the queue ptr in the buffer header,
		 * then free the buffer header and attached buffer
		 */
		bufptr->bf_queue_ptr = freeq;
		bufptr->bf_bcast_id = 0;
		hwq_enqueue_inline(QASIC_OFFSET_TO_ADDR(freeq), bufptr);

		return;

	    case RETQ_INVALID:
	    default:
		break;
	    }
	}

	/*
	 * Dump debug information in case of error
	 * Do not free the buffer header or the retd structure,
	 * to minimize further errors
	 */
	{
	    ulong* ulp;
	    ulp = (ulong*) bufptr;
	    errmsg(&msgsym(INVRTN, RSP),
		   retd->next, retd->hwidb, retd->type, freeq,
		   retd->bufhdr_offset, retd->id, bcast_id,
		   (ulong) bufptr, ulp[0], ulp[1], ulp[2], ulp[3]);
	    rsp_dump_memd_buffer(bufptr);
	    if (++lost_memd_buffers > MAX_LOST_MEMD_BUFFERS)
	      platform_req_complex_restart(NULL);
	}

    } else {
	ulong* ulp;

	/*
	 * Invalid broadcast ID, so buffer header is really trashed, or
	 * else is in the return queue by mistake.
	 *
	 * Complain and leave it off the queues for debugging
	 */
	ulp = (ulong*) bufptr;
	errmsg(&msgsym(INVRTNBCASTID, RSP),
	       bcast_id, (ulong) bufptr, ulp[0], ulp[1], ulp[2], ulp[3]);
	rsp_dump_memd_buffer(bufptr);
	if (++lost_memd_buffers > MAX_LOST_MEMD_BUFFERS)
	  platform_req_complex_restart(NULL);
    }
}

/*
 * inval_buf_hdr
 * Do a HIT_INVALIDATE_DATA on the supplied buffer header or CCB
 *
 * The input buffer header pointer must be at least 32 bit word aligned.
 */

void
inval_buf_hdr (bufhdr_t *bufptr)
{
    inval_buf_hdr_inline(bufptr);
}

/*
 * writeback_inval_data
 * Do a HIT_WRITEBACK_INVALIDATE_DATA on the specified memory region:
 *
 * The region of memory affected by this routine is larger than that
 * specified, since all cached memory is accessed in units of entire
 * cache lines.
 */

void
writeback_inval_data (uchar *ptr, int bytes)
{
    writeback_inval_data_inline(ptr, bytes);
}

/*
 * rsp_qa_error_interrupt
 * Handle QA error interrupts
 *
 * On the real RSP hardware, this routine should only be called if the
 * 'Q/A ASIC Error' bit is set in the 'Interrupt Level 6 Status Register'
 * in the MEMD controller:
 */ 

int
rsp_qa_error_interrupt (r4k_context_t *cp)
{
    ushort	address;
    char	*error_str, *select_str, *access_str;
    ulong       error_log0_3;
    ulong       error_log4_7;
    ulong       error_log8_11;
    uchar	error_field;
    char* const select_strings[] = { "NOP", "status", "QA", "diag", };
    boolean     reuse0_link = FALSE;

    /*
     * Disable CyBuses and leave them disabled:
     */
    disable_cbus_grant();

    /*
     * Read the QA Error log
     */
    error_log0_3 = read_qa_diag(&qasic_diag_base->error0_3);
    error_log4_7 = read_qa_diag(&qasic_diag_base->error4_7);
    error_log8_11 = read_qa_diag(&qasic_diag_base->error8_11);

    rsp_save_qadiag(&qasic_diag_copy);

    /*
     * Decode the Error field
     */
    error_str = "unknown";
    error_field = error_log0_3 >> 24;

    select_str = select_strings[error_field & QA_DIAG_SELECT_MASK];
    if (error_field & QA_DIAG_INPUT_PARITY_ERROR) {
	error_str = "input data parity";
    } else if (error_field & QA_DIAG_REUSE0_LINK_ERROR) {
	error_str = "reused or zero link";
	reuse0_link = TRUE;
    } else if (error_field & QA_DIAG_MEM_PARITY_ERROR) {
	error_str = "memory array parity";
    } else if (error_field & QA_DIAG_ALT_QWRITE_ERROR) {
	error_str = "write to alternate queue";
    } else if (error_field & QA_DIAG_BAD_ADD_ERROR) {
	error_str = "bad buffer header address";
    }
    if (QA_ERR_READWRITE(error_log0_3) == QA_ERR_READ)
	access_str = "read";
    else
	access_str = "write";

    /*
     * If an error was found, decode the rest of the information,
     * print it out, and set a global variable to alert other software:
     */

    address = (error_log0_3 >> 8) & QA_DIAG_ADDRESS_MASK;
    errmsg(&msgsym(QAERROR, RSP),
	   error_str, access_str, address, select_str,
	   error_log0_3, error_log4_7, error_log8_11);

    if (reuse0_link)
	rsp_qa_reuse0_diagnostics(cp, error_log4_7);
    
    rsp_qa_init();		/* Reset QA ASIC state and clear QA error */

    platform_req_complex_restart(NULL);

    return 0;			/* crash not necessary */
}


/*
 * Handle event interrupts:
 */

void
rsp_process_event (bufhdr_t *bufptr)
{
    hwidbtype		*hwidb;
    lovenotetype        *lovenote;

    /*
     * Cast the bufptr to a lovenote ptr so that we can access
     * it for what it really is.
     */
    lovenote = (lovenotetype *)bufptr;
    
    /*
     * Find the hwidb associated with this interface.
     * It is a requirement that IPs place a local free q ptr
     * into the love_id field.
     */
    hwidb = LOCAL_QUEUE_TO_HW_IDB(lovenote->love_id);

    if (hwidb) {
	if (cbus_debug)
	    buginf("\n%s: love note received, %08x %08x %08x %08x",
		   hwidb->hw_namestring, ((ulong*) lovenote)[0],
		   ((ulong*) lovenote)[1], ((ulong*) lovenote)[2],
		   ((ulong*) lovenote)[3]);
	/*
	 * Next call the love note handler for this particular
	 * interface.
	 */
	hwidb->love_note(hwidb, lovenote);

	/*
	 * If it's a love letter (i.e. has some data associated
	 * with it) then we need to invalidate any cache lines
	 * that may be holding that data.
	 */
	if (lovenote->data_length) {
	    invalidate_data_inline(MEMD_OFFSET_TO_ADDR(lovenote->buffer_ptr),
				   lovenote->data_length);
	}

    } else {
	null_lovenote_handler(NULL, lovenote);
    }

    /*
     * Now we go ahead and return the buffer to the free q
     * stored in the header by the IP.
     */
    retbufhdr_inline(bufptr);
}

#ifdef DEBUG
/*
 * Count and Recover from an Invalid QA Interrupt:
 */

void
rsp_invalid_qa_interrupt (void)
{
    invalid_qa_interrupts++;

    rsp_qa_init();		/* Reset QA ASIC state */

    platform_req_complex_restart(NULL);
}
#endif /* DEBUG */

static inline void
clean_global_ip_counts_cache (void)
{
    /*
     * global_ip_counts_cache contains the IP counts that are accumulated
     * in rsp_check_rawq() but not added to ip_traffic yet
     */
    if (global_ip_counts_cache) {
	ip_traffic.inputs += global_ip_counts_cache;
	ip_traffic.forwarded += global_ip_counts_cache;
	global_ip_counts_cache = 0;
    }
}

/*
 * rsp_check_rawq
 * Check the RawQ for input packets
 * Process up to NETINT_PKTS_PER_POLL packets
 * Return boolean indicating whether more work remains
 */

static inline boolean
rsp_check_rawq (boolean more)
{
    bufhdr_t	*rawq_bp;
    ulong	status;
    int         pkts_per_poll = NETINT_PKTS_PER_POLL;

    while (pkts_per_poll--) {

	/*
	 * Mask all interrupts to prevent timer interrupts between
	 * 'cache' instruction bug workaround and cache instruction!
	 * In this case, we workaround the Orion 2.0 bug by using
	 * the uncached read of the RawQ to flush the refill buffer.
	 * As long as we have interrupts disabled around this, we
	 * can be assured that the cache invalidation will work.
	 */
	GLOBAL_INTERRUPT_BLOCK(status);
	rawq_bp = hwq_dequeue_inline(RawQ);
	if (!rawq_bp) {		/* Empty raw queue? */
	    GLOBAL_INTERRUPT_UNBLOCK(status);
	    /*
	     * don't need to change 'more'
	     */
	    return(more);
	}
	hit_invalidate_cache_line_inline(rawq_bp);
	GLOBAL_INTERRUPT_UNBLOCK(status);

#ifdef DEBUG
	if ( (rawq_bp < bufhdr_base) ||
	    (rawq_bp >= (bufhdr_base + MEMD_BUFDATA_OFFSET) ) ) {
	    
	    /*
	     * Bad buffer header address!
	     * so complain and drop:
	     */
	    buginf("\nrsp_check_rawq: bad buffer header address");
	    buginf("=%#x\nshould be >= %#x and < %#x",
		   rawq_bp, bufhdr_base,
		   (bufhdr_base + MEMD_BUFDATA_OFFSET) );
	    continue;
	}
#endif /* DEBUG */

	/*
	 * Process or drop this input packet
	 */
	rsp_input_raw(rawq_bp);
    }

    /*
     * 'more' work to do
     */
    return(TRUE);
}

/*
 * rsp_check_returnq
 * Check the ReturnQ for input packets
 * Process up to NETINT_PKTS_PER_POLL packets
 * Return boolean indicating whether more work remains
 */

static inline boolean
rsp_check_returnq (void)
{
    int		pkts_per_poll;

    pkts_per_poll = NETINT_PKTS_PER_POLL;
    while(pkts_per_poll--) {
	bufhdr_t	*returnq_bp;

	returnq_bp = hwq_dequeue_inline(ReturnQ);
	if (returnq_bp == NULL)	{	/* Empty return queue? */
	    return(FALSE);;
	}

#ifdef DEBUG
	    if ( (returnq_bp < bufhdr_base) ||
		(returnq_bp >= (bufhdr_base + MEMD_BUFDATA_OFFSET) ) ) {

		/*
		 * Bad buffer header address!
		 * so complain and drop:
		 */
		buginf("\nrsp_check_returnq: bad buffer header address=%#x\nshould be >= %#x and < %#x",
		       returnq_bp, bufhdr_base,
		       (bufhdr_base + MEMD_BUFDATA_OFFSET) );
		continue;
	    }
#endif /* DEBUG */

	/*
	 * Process or drop this input packet
	 */
	rsp_input_return(returnq_bp);
    }
    return(TRUE);
}

/*
 * Handle network packet interrupts from the Raw queue
 * and from the Return queue in the QA ASIC
 */

void
rsp_process_rawq (void)
{
    boolean	more;
    usec_timestamp netint_exit_cache;

    /*
     * keep global value of netint_exit in local cache
     * The current code compiler keeps it in a register thru the entire
     * routine without loading global variable in each loop. 
     */
    netint_exit_cache = netint_throttle_setup();

    while (1) {
	more = rsp_check_returnq();
	more = rsp_check_rawq(more);

	/*
	 * See if we need to throttle network interrupts
	 */
	if (USEC_AWAKE(netint_exit_cache)) {
	    netint_throttle_on();
	    break;
	}

	/*
	 * We drained the queues, so we are done
	 */
	if (!more)
	    break;
    }
    netint_exit = netint_exit_cache;
    USEC_GET_TIMESTAMP(netint_return);

    /*
     * call clean_global_ip_counts_cache to update global_ip_counts_cache 
     * which contains the IP counts that are accumulated
     * in rsp_check_rawq() but not added to ip_traffic yet
     */
    clean_global_ip_counts_cache();
    return;
}

/*
 * Handle network status interrupts from the Event queue in the QA ASIC
 */

void
rsp_process_eventq (void)
{
    reg_invoke_ipc_cbus_input();

    for (;;) {
	bufhdr_t	*eventq_bp;

	/*
	 * Check the command/event queue for love missives
	 */
	eventq_bp = hwq_dequeue(EventQ);
	if (!eventq_bp)
	    return;

#ifdef DEBUG
	if ( (eventq_bp < bufhdr_base) ||
	    (eventq_bp >= (bufhdr_base + MEMD_BUFDATA_OFFSET) ) ) {

	    /*
	     * Bad buffer header address!
	     * so complain and drop:
	     */
	    buginf("\nrsp_process_eventq: bad buffer header address=%#x\nshould be >= %#x and < %#x",
		   eventq_bp, bufhdr_base,
		   (bufhdr_base + MEMD_BUFDATA_OFFSET) );
	    continue;
	}
#endif /* DEBUG */

	/*
	 * Invalidate the buffer header here, so that we don't reuse
	 * stale cached data from when we read the other buffer header
	 * in this cache line
	 */
	inval_buf_hdr(eventq_bp);
	rsp_process_event(eventq_bp);
	eventq_bp->bf_tx_bytes = 0xffff; /* XXX - debug love note length */
    }
}

/*
 * rsp_tx_start
 * Output any packets sitting on the output hold queue
 *
 * Arrange to have buffers returned via the ReturnQ so that we will
 * attempt to restart output with every packet that is successfully
 * output from the output hold queue. This allows us to restart output
 * more aggressively when output fails due to temporary MEMD buffer
 * shortages, or when full hw TX queues are encountered.
 */
void
rsp_tx_start (hwidbtype *hwidb)
{
    paktype	*pak;
    bufhdr_t	*bufptr;
    uchar	*tx_data_ptr;
    ulong	tx_memd_offset;
    leveltype	SR;
    ushort	size;
    short	tx_limit;

    /* when tx_acc_ptr == NULL, the interface has been NOed */
    if ((hwidb->status & IDB_NOHARDWARE) || (hwidb->tx_acc_ptr == NULL)) {
	holdq_reset(hwidb);
	return;
    } 

    if (cbus_being_reset)
	return;

    while (TRUE) {

	SR = raise_interrupt_level(NETS_DISABLE);
    	if (!hwidb->output_qcount) {
	    reset_interrupt_level(SR);
	    break;
	}

	/*
	 * Ensure that there was space in the HW TX queue before
	 * attempting to transmit a packet.
	 *
	 * Note that hwacc_read_dec_inline() returns the value
	 * that was in the accumulator BEFORE it was decremented.
	 */
	tx_limit = hwacc_read_dec_inline(hwidb->tx_acc_ptr);
	if (!tx_limit) {
#ifdef DEBUG
	    full_txq_count++;
#endif
	    reset_interrupt_level(SR);
	    break;
	}

	/*
	 * While there is another packet to output and room for it in the
	 * outgoing IP's TX queue, dequeue the next packet and output it
	 */
	pak = hwidb->oqueue_dequeue(hwidb);
	if (!pak) {
	    hwacc_inc_inline(hwidb->tx_acc_ptr); /* Restore TX limit */
	    reset_interrupt_level(SR);
	    break;
	}

	/*
	 * Is the packet too big to transmit?
	 */
	size = pak->datagramsize;
	if (size > hwidb->max_pak_size) {
	    hwidb->counters.output_total_drops++;
	    errmsg(&msgsym(TOOBIG, LINK), hwidb->hw_namestring, size);
	    
	    rsp_oqueue_retbuffer(pak);
	    hwacc_inc_inline(hwidb->tx_acc_ptr); /* Restore TX limit */
	    reset_interrupt_level(SR);
	    continue;
	}

        if (hwidb->status & IDB_TR) {
		/*
		 * process level doesn't necessarily change AC on srb
		 *  input frame - input AC may not be suitable for output
		 *  on trip.
		 */
	    if (pak->flags & (PAK_SRB_OUT|PAK_SRB_EXPLORER))
		    *pak->datagramstart = AC_PRI4;

            token_output_debug(hwidb, pak);
        }

	/*
	 * Get a MEMD data buffer, with buffer header,
	 * large enough to hold the output packet, some prefix pad
	 * and some memdcopy() trailing pad.
	 */
	bufptr = get_memd_buffer(size + RSP_BUFSTART_PAD + MEMDCOPY_PAD);

	if (!bufptr) {
	    
	    /*
	     * No MEMD buffer available. Count error and requeue
	     * packet. If requeue fails, code called by
	     * pak_requeue_head() does datagram_done():
	     *
	     * XXX (DK) - This should not happen too many times.
	     * If it does, we should check a timer and reset the
	     * complex.
	     */
	    hwidb->counters.output_resource++;
	    pak_requeue_head(hwidb, pak);
	    hwacc_inc_inline(hwidb->tx_acc_ptr); /* Restore TX limit */
	    
	    reset_interrupt_level(SR);
	    break;
	}

	/*
	 * Set up the output buffer header, and copy the output packet
	 * from DRAM into MEMD:
	 *
	 * We include an extra 2 bytes of pad prepending the packet
	 * start for outbound HIP, to support HIP transmitter delay,
	 * which is written to MEMD by HIP microcode.  However, we
	 * round up to an 8-byte boundary to get optimal memd_copy
	 * speed, since we can copy 50% faster to memd using "sd"
	 * than "sdl/sdr" MIPS instructions.
	 *
	 * We also need to make sure that minimum sized FDDI packet takes
	 * at least 5 MEMD reads to avoid some FIP dma problem.  So if
	 * the outbound packet is 16 bytes, start on an odd word boundary
	 * (but only for small packets, since we copy slower for odd
	 * boundaries).
	 */
	if (size <= RSP_FIP_DMA_UNDERRUN_SIZE)
	  tx_memd_offset = bufptr->bf_buffer_ptr + RSP_FIP_BUFSTART_PAD;
	else
	  tx_memd_offset = bufptr->bf_buffer_ptr + RSP_BUFSTART_PAD;
	
	tx_data_ptr = MEMD_OFFSET_TO_UNCACHED_ADDR(tx_memd_offset);

	if (pak->flags & PAK_HEARSELF) {
	    /*
	     * Potentially set HEARSELF bit for EIP.
	     */
	    if ((hwidb->type == IDBTYPE_CBUSETHER) || 
		(hwidb->type == IDBTYPE_FEIP)) 
		tx_memd_offset |= TX_PTR_HEARSELF_EIP;
	}

	/*
	 * There follows a check to determine if this is an ISL vLAN packet
	 * egressing an FEIP interface, in which case we may have to indicate
	 * to the Fast Ethernet ucode for the RSP that it needs to generate
	 * an interior CRC for the encapsulated packet. Once the FEIP Port
	 * Adapter incorporates the MICE (Mini ISL CRC Engine) hardware assist
	 * this will be redundant.
	 */

	if (pak->flags & PAK_DO_ISL_CRC) {

	    tx_memd_offset |= TX_PTR_DO_ISL_INNER_CRC;
	}


	bufptr->bf_tx_ptr = tx_memd_offset;
	bufptr->bf_tx_bytes = size;	/* packet length */

 	/*
 	 * Arrange for the IP to return this buffer header/memd buffer
 	 * via the ReturnQ, so that we have an opportunity to restart
 	 * output on this interface when it is returned to us
 	 */
 	rsp_tx_restart_via_returnq(hwidb, bufptr);
  
	memdcopy(pak->datagramstart, tx_data_ptr, size);

	/*
	 * Enqueue the packet on the IP's transmit queue:
	 */
	hwq_enqueue_inline(hwidb->tx_q_ptr, bufptr);
	
	/*
	 ********************************************************
	 * Do packet accounting after giving the packet to the IP.
	 *
	 * However, don't do accounting if it's been done already
	 * from fastswitching, if we're just here due to congestion
	 * and backing-store.
	 ********************************************************
	 */

	if (pak->buffer_flags & PAK_BUF_ACCT_DONE)
	  pak->buffer_flags &= ~PAK_BUF_ACCT_DONE;
	else {
	    boolean its_a_mip = rsp_hwidb_has_board_encap(hwidb);

	    /*
	     * Have to dork with pak->datagramstart to skip over the VC
	     * for the broadcast check when outputting to the MIP.
	     */
	    if (its_a_mip)
		pak->datagramstart += MIP_ENCAPSIZE_BYTES;
	    
	    /*
	     * See if the output packet is a broadcast packet. Use
	     * the broadcast check implementation pointed at via the
	     * hwidb vector:
	     */
	    if (hwidb->broadcast(hwidb, pak) == FUZ_TRUE)
		hwidb->counters.output_broad++;
	    
	    /*
	     * If we dorked with datagramstart be sure to return
	     * it to its previous glory.  LAPB may well be keeping
	     * a copy of this packet, and will get most confused if
	     * anyone tinkers with datagramstart.
	     */
	    if (its_a_mip)
		pak->datagramstart -= MIP_ENCAPSIZE_BYTES;

	    hwidb->counters.tx_cumbytes += size;
	    hwidb->counters.outputs++;
	}
	
	/*
	 * Free the process level packet buffer now that we're
	 * done touching it:
	 */
	rsp_oqueue_retbuffer(pak);
	
	GET_TIMESTAMP(hwidb->lastoutput);
	
	reset_interrupt_level(SR);
    }
}

/*
 * rsp_replicate_pak
 *
 * pull code aside so it can be called from elsewhere
 */
paktype *rsp_replicate_pak(paktype *pak)
{
    paktype	*newpak;
    uchar	*newstart;
    int		size;
    idbtype     *output;
    hwidbtype   *hwidb;
    int		 offset;

    output = pak->if_output;
    hwidb = hwidb_or_null(output);
    size = pak->datagramsize;

    /*
     * Packet in MEMD.
     * Copy packet out of MEMD into a system buffer
     * in DRAM and enqueue it on the end of the hold queue.
     */

    newpak = rsp_oqueue_getbuffer(size);
    if (!newpak) {
	rsp_fs_free_memd_pak(pak);
	hwidb->counters.output_total_drops++;
	return NULL;
    }

    /*
     * Set up the new packet structure:
     */
    hwidb->pak_swapped_out++;
    offset = pak->network_start - pak->datagramstart;
    newstart = newpak->datagramstart - offset;
    newpak->datagramstart = newstart;
    newpak->datagramsize = size;
    newpak->rxtype = pak->rxtype;
    newpak->linktype = pak->linktype;
    newpak->if_input = pak->if_input;
    newpak->if_output = pak->if_output;
    newpak->flags = pak->flags;
    newpak->traffic_shape = pak->traffic_shape;
    newpak->mac_start = 
	newstart + (pak->mac_start - pak->datagramstart);

    /*
     * Move accounting-done indicator from old pak to new one,
     * and clear it in old pak, since this field doesn't get
     * reset from packet to packet if it's the RSP's fs_pak.
     */
    newpak->buffer_flags |= pak->buffer_flags & PAK_BUF_ACCT_DONE;
    pak->buffer_flags &= ~PAK_BUF_ACCT_DONE;
	
    /*
     * Copy packet into DRAM, then invalidate original packet in
     * MEMD.
     * Invalidate entire packet buffer that might have been
     * touched, from data_area to end of packet. Takes care of
     * cases like SMDS, etc.
     * Don't have to invalidate new packet in DRAM. No DMA in DRAM.
     *
     */
    bcopy(pak->datagramstart, newstart, size);
	
    /*
     * Free the input MEMD buffer:
     */
    if (pak->buffer_header) {
        rsp_fs_free_memd_pak(pak);
    } else {
        datagram_done(pak);
    }
    return newpak;
}
/*
 * rsp_move_pak_to_holdq
 * Move this TX packet onto the output hold queue or drop it.
 * If this packet is sitting in a MEMD buffer, copy it out of MEMD
 * into a system buffer in DRAM first.
 *
 * Requires pak->if_input to be set to the input interface so we can
 * do an input_getbuffer(), which respects the input queue count.
 *
 * Called at interrupt level from rsp_fastsend().
 */

static void
rsp_move_pak_to_holdq (hwidbtype *hwidb, paktype *pak)
{
    paktype	*newpak;
    uchar	*newstart;
    int		size;
    int		offset;

    /*
     * do not check for queue full if fair queuing present
     *
     * The reason, simply, is that while FCFS, Priority, and Custom
     * Queuing have hard upper bounds on packet depth, WFQ has a soft
     * upper bound.  If the message is for a queue that has other
     * traffic in it and the depth limit is reached, WFQ will toss it.
     * However, if the queue is empty - because the traffic is
     * interactive or is for a flow which has been granted precedence
     * via RSVP fo example - WFQ will queue it anyway and refuse
     * traffic later. Call this "intelligent early drop" if you like
     * :^)
     */

    /*
     * Output queueing and the newpak init below expect network_start and
     * mac_start to be init.
     * It can't be guaranteed that all current and future fs paths will
     * do this.  Since the effect of these fields uninit is catastrophic,
     * both via cache corruption and the data copy below,
     * these checks are a must.
     *
     * In a few cases output queueing may not work well with these fields set
     * to datagramstart but its much better than crashing.  The fix 
     * would be to correct the fs path.
     *
     * A better design might be to correctly init these fields here 
     * rather than requiring all fs paths to init them whether or not they
     * are needed.
     */

    if ( (pak->network_start < pak->datagramstart) || 
         (pak->network_start >= (pak->datagramstart+pak->datagramsize))) {
       pak->network_start = pak->datagramstart;
    }
    if ( (pak->mac_start < pak->datagramstart) || 
       (pak->mac_start >= (pak->datagramstart+pak->datagramsize))) {
       pak->mac_start = pak->datagramstart;
    }

    size = pak->datagramsize;
    
    /*
     * Is the output packet sitting in MEMD or in a DRAM buffer.
     */
    if (!pak->buffer_header) {
	/*
	 * Packet not in MEMD, already sitting in a system buffer.
	 * Don't count as pak_swapped_out.
	 */
	
    } else {
	/*
	 * Packet in MEMD.
	 * Copy packet out of MEMD into a system buffer
	 * in DRAM and enqueue it on the end of the hold queue.
	 */
	newpak = rsp_oqueue_getbuffer(size);
	if (!newpak) {
	    rsp_fs_free_memd_pak(pak);
	    hwidb->counters.output_total_drops++;
	    return;
	}

	/*
	 * Set up the new packet structure:
	 */
	hwidb->pak_swapped_out++;
	offset = pak->network_start - pak->datagramstart;
	newstart = newpak->datagramstart - offset;
	newpak->datagramstart = newstart;
	newpak->datagramsize = size;
	newpak->rxtype = pak->rxtype;
	newpak->linktype = pak->linktype;
	newpak->if_input = pak->if_input;
	newpak->if_output = pak->if_output;
	newpak->flags = pak->flags;
	newpak->traffic_shape = pak->traffic_shape;
	newpak->mac_start =		/* save offsets to headers */
	    newstart + (pak->mac_start - pak->datagramstart);

	/*
	 * Move accounting-done indicator from old pak to new one,
	 * and clear it in old pak, since this field doesn't get
	 * reset from packet to packet if it's the RSP's fs_pak.
	 */
	newpak->buffer_flags |= pak->buffer_flags & PAK_BUF_ACCT_DONE;
	pak->buffer_flags &= ~PAK_BUF_ACCT_DONE;
	
	/*
	 * Copy packet into DRAM, then invalidate original packet in
	 * MEMD.
	 * Invalidate entire packet buffer that might have been
	 * touched, from data_area to end of packet. Takes care of
	 * cases like SMDS, etc.
	 * Don't have to invalidate new packet in DRAM. No DMA in DRAM.
	 *
	 */
	bcopy(pak->datagramstart, newstart, size);
	
	/*
	 * Free the input MEMD buffer:
	 */
	rsp_fs_free_memd_pak(pak);	/* Done with MEMD buffer */

	pak = newpak;
    }

    /*
     * We are confident that the packet will eventually be sent
     * due to previous QUEUEFULL check.  Therefore, account
     * as a fastswitched packet if we haven't done so previously.
     *
     * Note that we are not doing broadcast accounting.  We need
     * to document that fastswitched broadcast packets aren't
     * accounted for by txstart code.
     */
    if (!(pak->buffer_flags & PAK_BUF_ACCT_DONE)) {
	short	acct_proto;
	
	acct_proto = pak->acct_proto;
	hwidb->counters.outputs_fast[acct_proto]++;
	hwidb->counters.tx_cumbytes_fast[acct_proto] += size;
	pak->buffer_flags |= PAK_BUF_ACCT_DONE;
    }
	
    /*
     * Put the new packet on the TX hold queue now:
     *
     * Holdq_enqueue_fast_reserve will enqueue the packet
     * on the end of the output hold queue, or will count
     * the TX drop and free the packet buffer.
     * queuing routine counts drop and
     * frees input buffer on enqueue failure.
     */
    hwidb->oqueue(hwidb, pak, TAIL);
    if (1 == hwidb->output_qcount)
	rsp_tx_start(hwidb);
}


/*
 * rsp_check_for_pak_backing_store
 * Common routine used to check for pak_backing_store and queue
 * packet on output hold queue or free it if not.
 * Used by rsp_fastsend() when an error occurs that prevents
 * using the immediate transmit path.
 */

void
rsp_check_for_pak_backing_store (hwidbtype *hwidb, paktype *pak) {

    /*
     * Is packet backing store enabled?
     */
    if (hwidb->pak_backing_store || hwidb->priority_list) {

	/*
	 * Yes, enqueue this packet on the end of the
	 * output hold queue or drop and count it:
	 */
	rsp_move_pak_to_holdq(hwidb, pak);

    } else {

	/*
	 * No. Drop this TX packet.
	 * If the output packet is sitting in MEMD, free
	 * the MEMD buffer, else free the system buffer:
	 */
	if (pak->buffer_header) {
	    rsp_fs_free_memd_pak(pak);
	} else {
	    datagram_done(pak);
	}
	hwidb->counters.output_total_drops++;
    }
}

/*
 * rsp_fastsend
 * Output a packet that was switched at interrupt level
 *
 * Handles packets sitting in MEMD (pak->buffer_header is non NULL),
 * and packets sitting in system buffers.
 */

void
rsp_fastsend (hwidbtype *hwidb, paktype *pak)
{
    bufhdr_t	*bufptr;
    int		size;
    uchar	*start;
    short	tx_limit;
    short	acct_proto;
    boolean     crc_required = FALSE;
    traffic_shape_t *shape;

    /*
     * determine whether there is window available to send the message with
     * in the rate control bucket
     */
    shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
    if (shape) {
	pak = rsp_replicate_pak(pak);
	if (pak)
	    traffic_shape_defer_sending(shape, pak);
	return;
    }

    /*
     * Check to see if any packets are sitting on the process level
     * output hold queue before transmitting this packet to minimize
     * packets sent out of order.
     */
    if (hwidb->output_qcount) {

	/*
	 * Send as many packets from the hold queue as can fit on
	 * the MEMD transmit queue.
	 */
	rsp_tx_start(hwidb);

	if (hwidb->output_qcount) {
	    /*
	     * TX Hold Queue still not empty!
	     * Is TX buffer backing store enabled?  If so, copy packet
	     * to DRAM, put on holdq, and free the MEMD buffer.
	     * Otherwise just drop the packet.
	     */
	    rsp_check_for_pak_backing_store(hwidb, pak);
	    return;
	}
    }

    /*
     * Is there room for this packet on the interface's TX queue?
     *
     * Note that hwacc_read_dec_inline() returns the value
     * that was in the accumulator BEFORE it was decremented.
     * Fastswitched packets aren't allowed to take the last
     * tx slot, since this could prevent us from priming the
     * pump for tx interrupts to drain the hold queue.
     */
    tx_limit = hwacc_read_dec_inline(hwidb->tx_acc_ptr);
    if (tx_limit >= RSP_RES_PROC_TX) {

	/*
	 * Yes, room to transmit.
	 */
	size = pak->datagramsize;
	acct_proto = pak->acct_proto;
	start = pak->datagramstart;

	/*
	 * Interface specific code!
	 */
	if (hwidb->fast_switch_type == FS_AIP) {
	    vc_info_t	*output_vc;

	    /*
	     * Find the output VC and count fast packets that went out.
	     * Pull the VC num from the first short of the output packet:
	     */
	    output_vc = atm_getvc_inline(hwidb->atm_db,
					 GETSHORT(pak->datagramstart));
	    if (output_vc)
		output_vc->outfast++;
	    ATM_OUTFAST(hwidb);

        } /* End if - output interface is AIP */

	hwidb->counters.outputs_fast[acct_proto]++;
	hwidb->counters.tx_cumbytes_fast[acct_proto] += size;

	if (pak->flags & PAK_DO_ISL_CRC) {
   
	    crc_required = TRUE;
        }
	/*
	 * Set up the transmit buffer header
	 */
	bufptr = (bufhdr_t *)pak->buffer_header;
	if (!bufptr) {
	    /*
	     * The packet is sitting in a system buffer in DRAM,
	     * so we need to copy the packet into a MEMD buffer.
	     * Get a MEMD buffer we can copy the packet into:
	     * This routine returns a pointer to an uncached buffer
	     * header, so we don't need to invalidate the cache line.
	     */
	    bufptr = get_memd_buffer(size + RSP_BUFSTART_PAD + MEMDCOPY_PAD);
	    if (!bufptr) {
	    
		/*
		 * We didn't get a MEMD buffer for the output frame,
		 * so, count the error, clean up and return:
		 */
		hwidb->counters.output_resource++;
	    
		/*
		 * Restore tql.
		 */
		hwacc_inc_inline(hwidb->tx_acc_ptr);

		/*
		 * We've already accounted for fastswitching pkts, bytes,
		 * vc, and broadcast.  Tell backing store code not to count
		 * again.
		 */
		pak->buffer_flags |= PAK_BUF_ACCT_DONE;
		rsp_check_for_pak_backing_store(hwidb, pak);
		return;
	    }
	    /*
	     * Set up the output buffer header, and copy the output packet
	     * from DRAM into MEMD. Start now points at the packet in the
	     * new MEMD buffer. The packet is copied into the first available
	     * byte in the new MEMD buffer.
	     *
	     * Use uncached MEMD to avoid cache fills and invalidation stalls.
	     * Use memdcopy to get minimal write stalls.   This should copy
	     * in 10nS per byte when the MEMD address is dword aligned,
	     * or 15nS per byte if MEMD is unaligned.
	     * (Note that the previous version took ~30nS per byte due to
		* cache fill, cache invalidate, and cache write).
		*/
	    start = MEMD_OFFSET_TO_UNCACHED_ADDR(bufptr->bf_buffer_ptr +
						 RSP_BUFSTART_PAD);
	    memdcopy(pak->datagramstart, start, size);
	    
	    /*
	     * If the packet was originally in DRAM, return now.
	     */
	    datagram_done(pak);
	    
	} else if (pak->buffer_flags & PAK_BUF_DIRTY) {
	    /*
	     * This was always a MEMD buffer.
	     * Writeback invalidate the packet data that we might
	     * have read or written while fast switching. If the
	     * dirty flag is set, writeback invalidate the entire
	     * packet, including max possible encaps
	     */
	    writeback_inval_data_inline(pak->data_area,
					(size + (start - pak->data_area)));
	    pak->buffer_flags &= ~PAK_BUF_DIRTY; /* Clean now */
	} else {
	    /*
	     * This was always a MEMD buffer.
	     * Invalidate all cache lines from the start of the buffer
	     * through the maximum fastswitchable network header.
	     */
	    fs_writeback_inval_data_inline(pak->data_area);
	}
	
	/*
	 * Write the transmit pointer, ensuring that the hearself
	 * and unused status bits are cleared:
	 */
	bufptr->bf_tx_ptr = ADDR_TO_MEMD_OFFSET_24(start);
	bufptr->bf_tx_bytes = size;

        /*
         * There follows an ISL vLAN specific check :-( to determine if this
	 * is an ISL packet egressing an FEIP interface, in which case we may
         * have to indicate to the Fast Ethernet ucode for the RSP that it
	 * needs to generate an interior CRC for the encapsulated packet.
         * Once the FEIP Port Adapter incorporates the MICE (Mini ISL CRC
         * Engine) hardware assist this will be redundant.
         */

	if (crc_required == TRUE) {
   
	    bufptr->bf_tx_ptr |= TX_PTR_DO_ISL_INNER_CRC;
        }
	
	/*
	 * Code arranged to minimize jumps for the normal, not full
	 * hw TX queue case. The fast case is when we're not using
	 * the last fast switching hw TX queue slot:
	 */
	if (tx_limit != RSP_RES_PROC_TX) {

	queue_tx_pkt:

	    /*
	     * Enqueue the packet in MEMD on the hardware queue:
	     */
	    hwq_enqueue_inline(hwidb->tx_q_ptr, bufptr);
	
	
	    GET_TIMESTAMP(hwidb->lastoutput);
	    return;

	} else {

	    /*
	     * If we taking the last transmit slot allowed for fast
	     * switching, arrange for the TX IP to return this MEMD
	     * buffer to us via the ReturnQ, so that we always have an
	     * opportunity to restart output from the output hold queue
	     * for this interface when the MEMD buffer is returned to us
	     * after transmission even if no more traffic comes in:
	     */
	    rsp_tx_restart_via_returnq(hwidb, bufptr);
	    goto queue_tx_pkt;

	}

    } else {

	/*
	 * No more fast switched packets allowed on the hw TX queue.
	 * Restore tql if it wasn't zero.
	 */
	if (tx_limit)
	    hwacc_inc_inline(hwidb->tx_acc_ptr);
	
	/*
	 * Is TX buffer backing store enabled?  If so, copy packet
	 * to DRAM, put on holdq, and free the MEMD buffer.
	 * Otherwise just drop the packet.
	 */
	rsp_check_for_pak_backing_store(hwidb, pak);
	return;
    }
}




/*
 * turbo_disabled
 */
int turbo_disabled (void)
{
    return(IPFLOWERR_NOTCONF);
}

/*
 * stub to complain about old MCI cards
 */
void
mci_warning (void)
{
    return;
}

/*
 * stub to disable MCI cards seeing lots of input errors
 */
void
mci_input_error (hwidbtype* hwidb)
{
    return;
}

/*
 * rsp_qacopy_read
 */
static inline ulong
rsp_qacopy_read (ulong *addr)
{
    return(*addr);
}

/*
 * rsp_qacopy_is_queue_valid
 *
 * Seaches through a queue by reading the header, tail and body arrays in
 * a copy of QA diag space.  Returns TRUE if the 
 * queue is valid, and FALSE otherwise.     If bp is not NULL, then it also
 * checks to see if the specified buffer is on this queue. 
 *
 * buf is a 16-bit number, because QA only uses the bottom 16 bits of the
 * buffer header pointer when adding bufers to queues (the low 4 bits are
 * always zero)
 *
 * This function assumes that the tail of a queue is determined by the 
 * contents of the tail array.   It does not expect the last element of the
 * body array to be set to zero.
 */
static boolean
rsp_qacopy_is_queue_valid (int qnum,  ushort buf)
{
    ushort first, last, next;
    uint lsabit;
    int el;
    
    boolean valid = TRUE;
    
    /* if queue is empty, return immediately */
    if (QA_READ_STATUS(qnum, rsp_qacopy_read, &qasic_diag_copy) == 0)
	return (valid);
    
    el = 1;
    first = QA_READ_HEAD(qnum, rsp_qacopy_read, &qasic_diag_copy);
    last = QA_READ_TAIL(qnum, rsp_qacopy_read, &qasic_diag_copy);
    next = first;
    
    while (TRUE) {

	if (next == 0) {
	    errvarmsg(&msgsym(DIAG, QA),
		      "Buffer 0x%04x, element %u on queue %#x is NULL",
		      next, el, qnum);
	    valid = FALSE;
	}

	lsabit = QA_READ_LSA( next, rsp_qacopy_read, &qasic_diag_copy);
	if (lsabit == 0) {
	    errvarmsg(&msgsym(DIAG, QA), 
		      "Buffer 0x%04x, element %u on queue %#x has clear LSA",
		      next, el, qnum);
	    valid = FALSE;
	}
	

	if (!is_bufhdr_off(next)) {
	    errvarmsg(&msgsym(DIAG, QA), 
		      "Illegal buffer 0x%04x found on queue %#x, element %u",
		      next, qnum, el);
	    valid = FALSE;
	    break;
	}
    
	if (next == buf) {
	    errvarmsg(&msgsym(DIAG, QA),
		      "Buffer 0x%04x is element %u on queue %#x",
		      buf, el, qnum);
	}
	
	if (el > QA_NQ_ELEMENTS) {
	    errvarmsg(&msgsym(DIAG, QA), 
		      "No NULL terminator for queue %#x?", qnum);
	    valid = FALSE;
	    break;
	}
    
	if (next == last) 	/* reached end of queue */
	    break;
	
	el++;
	next = QA_READ_BODY(next, rsp_qacopy_read, &qasic_diag_copy);
	
    }
    errvarmsg(&msgsym(DIAG, QA), "    Queue %#x (%x) has %d elements", 
	      qnum, queue_ptr(qnum), el); 
    return (valid);
}

/*
 * rsp_qacopy_are_queues_valid
 *
 * Returns TRUE if all queues are valid in the copy of QA diag space.
 * Returns FALSE otherwise.
 *
 * While checking queue validity, it also searches for  a specified buffer,
 * and prints a message about where the buffer was found.
 *
 * buf is a 16-bit number, because QA only uses the bottom 16 bits of the
 * buffer header pointer when adding bufers to queues (the low 4 bits are
 * always zero).
 */
static boolean
rsp_qacopy_are_queues_valid (ushort buf)
{
    boolean valid = TRUE;
    int qnum;

    for (qnum = QA_FIRST_QNUM; qnum <= QA_LAST_QNUM; ++qnum) {
	if (rsp_qacopy_is_queue_valid(qnum, buf) == FALSE) {
	    valid = FALSE;
	} 
    }
    return (valid);
}


/* rsp_qa_reuse0_diagnostics
 *
 * These diagnostics are intended to help diagnose the root cause of
 * reuse link error, when an attempt is made to enqueue a buffer which
 * is already on a queue (its link status bit is already set to 1).
 * After a zero link error, buf will usually be set to 0, and so this
 * code will normally be skipped.
 *
 * Note that a reuse link error leaves the tail pointer of the affected
 * queue pointing to the buffer which could not be properly enqueued. 
 * This may appear as a shortening of the queue - buffers after that
 * item on the queue will no longer appear to be on the queue, and the
 * queue may appear to be shorter than it should be.
 *
 * Set context_p to 0 if a stack backtrace is not wanted.
 */

void
rsp_qa_reuse0_diagnostics(r4k_context_t *context_p, ulong error_log4_7)
{
    ulong bufhdr;

    bufhdr = (error_log4_7 >> QA_DIAG_BUF_SHIFT) & QA_DIAG_BUF_MASK;
    errvarmsg(&msgsym(DIAG, QA),
	      "Failed to enqueue buffer header %#x", bufhdr);
	
    /* print stack backtrace */
    if (context_p != NULL)
    {
	ulong trace[LOG_TRACE_MAX];
	int   frame;
	char  buf[TRACEBACK_SIZE];
	char *cp;

	errvarmsg(&msgsym(DIAG, QA), "Approximate stack backtrace prior to interrupt:");

	cp = &buf[0];
	frame = 0;
	cp += sprintf(cp, "%s", traceback_prefix);

	r4k_pc_trace(trace, LOG_TRACE_MAX, context_p);

	while (frame < LOG_TRACE_MAX && trace[frame] != 0)
	{
	    cp += sprintf(cp, "%x ", trace[frame]);
	    frame++;
	}
	*--cp = '\0';		/* dump trailing space */
	errvarmsg(&msgsym(DIAG, QA), "%s", &buf[0]);
    }
    
    /* verify QA queues and search for bad buffer */
    if (bufhdr != 0) {	   /* reuse link error, not zero link error */

	ulong *ulp;

	if (rsp_qacopy_are_queues_valid((ushort) bufhdr )) 
	    errvarmsg(&msgsym(DIAG, QA), "All QA queues are consistent");
	else 
	    errvarmsg(&msgsym(DIAG, QA),"At least one QA queue is broken");
	    
	/*
	 * Print the buffer header and associated buffer contents.  First
	 * sanity check the buffer header offset.  Should be on a 16 byte
	 * boundary, and entirely within the buffer header part of MEMD. 
	 */

	if (((bufhdr & 0xf) == 0) &&
	    (bufhdr >= MEMD_BUFHDR_OFFSET) &&
	    (bufhdr < (MEMD_BUFHDR_OFFSET + MEMD_BUFHDR_SIZE))) {

	    ulp = MEMD_OFFSET_TO_ADDR(bufhdr);
	    errvarmsg(&msgsym(DIAG, QA), "Buffer header at %#x: %x %x %x %x",
		      ulp, ulp[0], ulp[1], ulp[2], ulp[3]);
	
	    errvarmsg(&msgsym(DIAG, QA), "Buffer contents:");
	
	    rsp_dump_memd_buffer(MEMD_OFFSET_TO_ADDR(bufhdr));

	} else {
	    errvarmsg(&msgsym(DIAG, QA), "Buffer header offset %#x is invalid", 
		      bufhdr);
	}
	
	rsp_print_global_queues();
	rsp_print_if_queues();
    }
}



