/* $Id: srb.c,v 3.34.4.26 1996/09/15 13:16:59 gcox Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb.c,v $
 *------------------------------------------------------------------
 * srb.c -- Token Ring Source Route Bridge Support
 *
 * May 1, 1989, Eric B. Decker
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srb.c,v $
 * Revision 3.34.4.26  1996/09/15  13:16:59  gcox
 * CSCdi67372:  Vip interface name get truncated on show source-bridge int
 * Branch: California_branch
 *
 * Revision 3.34.4.25  1996/09/14  01:04:54  ppearce
 * CSCdi68885:  SRB subblock not setup properly for VIP interfaces
 * Branch: California_branch
 *   Previous commit breaks build for c7200- images
 *     Backout previous changes
 *     Create SRB_SB subsystem to init SRB vector table
 *       rather than init in  VIP reset routines
 *
 * Revision 3.34.4.24  1996/09/11  04:46:00  kchiu
 * CSCdi68756:  DLSw+/fddi is not working
 * Branch: California_branch
 *
 * Revision 3.34.4.23  1996/09/07  02:08:18  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 3.34.4.22  1996/08/26  15:13:07  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.34.4.21  1996/08/03  23:38:20  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.34.4.20  1996/07/31  17:28:44  gcox
 * CSCdi64717:  Remove doubly-defined macros - SCALED_BANDWIDTH etc
 * Branch: California_branch
 *
 * Revision 3.34.4.19  1996/07/25  23:54:23  motto
 * CSCdi46834:  SRB explorer counters are incr. on CIP I/F for non CIP
 * destined expl
 * Branch: California_branch
 *
 * Revision 3.34.4.18  1996/07/23  13:29:34  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.34.4.17  1996/07/22  18:37:53  ppearce
 * CSCdi59090:  Non-explorer packets are being flushed with explorers
 * Branch: California_branch
 *   compile failure -- srb_rx_drops field no longer in hwidb
 *
 * Revision 3.34.4.16  1996/07/22  01:33:53  psandova
 * CSCdi59090:  Non-explorer packets are being flushed with explorers
 * Branch: California_branch
 * Valid multicast packets that should be handed to the protocol stack
 * are instead being diverted to the srb module and are flushed by the
 * explorer control mechanism.
 *
 * Revision 3.34.4.15  1996/07/18  20:29:44  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.34.4.14  1996/07/18  04:26:25  ppearce
 * CSCdi56398:  Router (sdlc-secondary) fails to respond to SNRM inputs
 * Branch: California_branch
 *   Ensure SDLLC process is re-started when SDLLC is configured
 *
 * Revision 3.34.4.13  1996/07/09  06:06:07  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.34.4.12  1996/07/02  23:11:47  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.34.4.11  1996/06/27  09:40:52  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.34.4.10  1996/06/24  17:59:03  ioakley
 * CSCdi58625:  In srb_common make lnm/vdlc checking conditional upon
 *              dlsw_running status to ensure that existing lnm over
 *              rsrb processing is not circumvented.
 * Branch: California_branch
 *
 * Revision 3.34.4.9  1996/06/17  23:38:02  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.34.4.8  1996/05/21  16:56:15  ppearce
 * CSCdi56732:  IP & Vines ping fails over SRB
 * Branch: California_branch
 *
 * Revision 3.34.4.7  1996/05/17  12:14:22  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.29.2.20  1996/05/06  00:01:52  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.29.2.19  1996/05/03  21:08:19  ppearce
 * CSCdi56298:  Error messages are seen during RSRB configuration
 * Branch: IbuMod_Calif_branch
 *   Only print the error message is SRB is NOT configured on the interface
 *     (logic was reversed, duh)
 *
 * Revision 3.29.2.18  1996/05/02  22:47:55  kchiu
 * CSCdi56517:  Image build error for c1005 images in IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.29.2.17  1996/05/02  22:07:22  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.29.2.16  1996/05/02  20:04:55  gcox
 * Branch: IbuMod_Calif_branch
 * IBU Modularity - add ibu_int subsystem
 *
 * Revision 3.29.2.15  1996/05/02  14:23:38  rbatz
 * CSCdi51754:  Output Access Lists broken again for Fast Explorers.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.29.2.14  1996/04/26  15:23:44  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.29.2.13  1996/04/26  07:58:57  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.29.2.12  1996/04/25  19:31:41  ioakley
 * CSCdi53651:  Add support for hidden message-age-increment option in
 *              "source-bridge spanning X" interface command.  Permits
 *              AST interoperability in mixed vendor bridge environments
 *              where hard-coded MAX_AGE defaults are insufficient for
 *              the network diameter and MAX_AGE is not configurable.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.29.2.11  1996/04/18  19:18:52  sbales
 * CSCdi50712:  DLSw+ does not perform RIF check on incoming explorers
 * Branch: IbuMod_Calif_branch
 *              Required check for high-end platforms.
 *
 * Revision 3.29.2.10  1996/04/15  00:09:30  kchiu
 * Branch: IbuMod_Calif_branch
 * Add dlsw+/fddi support
 *
 * Revision 3.29.2.9  1996/04/14  19:28:38  motto
 * CSCdi53896:  CSNA: Channel is process switching on input
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.29.2.8  1996/04/12  21:13:54  sbales
 * CSCdi50717:  DLSw+ - port-to-port explorers processed by DLSw+
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.29.2.7  1996/04/03  21:59:53  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.29.2.6  1996/03/28  08:21:09  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 * Revision 3.34.4.6  1996/04/30  17:53:24  hwang
 * CSCdi47567:  rvip interface name get truncated in show source-bridge
 * Reserve 12 spaces for idb->hw_short_namestring.
 * Branch: California_branch
 *
 * Revision 3.34.4.5  1996/04/02  17:55:41  psandova
 * CSCdi53367:  Fast sr/tlb defaults to the wrong OUI.
 * Branch: California_branch
 *
 * The default OUI was changed in 11.2 but was not
 * reflected in the sr/tlb process.
 *
 * Revision 3.34.4.4  1996/03/28  16:47:39  psandova
 * CSCdi47456:  All srb explorers being flushed on one tok interface
 * Branch: California_branch
 *
 * Make the flush timer interface specific.  Also, count flush
 * data rate after packet has passed all filtering checks.
 *
 * Revision 3.34.4.3  1996/03/23  22:49:24  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.34.4.2  1996/03/22  00:13:52  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.34.4.1  1996/03/18  22:11:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.5  1996/03/18  23:36:20  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.4.2.4  1996/03/06  19:53:20  sthormod
 * CSCdi50859:  add srb switching path for predator 4R
 * Branch: ELC_branch
 * Add case for IBDTYPE_IBM2692.
 *
 * Revision 3.4.2.3  1996/02/08  09:49:27  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.4.2.2  1996/01/16  02:37:36  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.4.2.1  1995/12/11  19:25:34  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.34  1996/03/11  17:24:55  ppearce
 * CSCdi47301:  CLS/RSRB link activation may fail when LAK enabled
 *   Cannot depend on pak->peer_ptr to represent a valid peer entry
 *     use a different method to find valid peer entry
 *
 * Revision 3.33  1996/03/11  14:05:58  rbatz
 * CSCdi45116:  Duplicate All-Routes Explorers unnecessarily
 *              forwarded in redundant topologies.
 *
 * Revision 3.32  1996/03/07  23:32:44  speakman
 * CSCdi48222:  SMF creates chunk storage even when unconfigured
 * Replace SMF chunk memory with managed memory and fix the original
 * problem with a simple check for onintstack.
 *
 * Revision 3.31  1996/03/07  06:34:29  kchiu
 * CSCdi40165:  Add netbios session level byte filtering support
 *
 * Revision 3.30  1996/02/29  00:19:40  kchiu
 * CSCdi46631:  lanmgr reporting erroneous Non-Broadcast frame counters
 *
 * Revision 3.29  1996/02/22  01:46:09  ioakley
 * CSCdi48632:  Bridge Group 0 is running the IBM protocol already.
 * Prevent user from deconfiguring IBM spanning tree on an interface when
 * it's not configured.
 *
 * Revision 3.28  1996/02/13  22:12:31  fred
 * CSCdi47956:  WFQ severely degrades RSRB performance
 *              move fair queue discriminator from srb.c to rsrb.c
 *              make sure pointers are st up for queueing routine
 *
 * Revision 3.27  1996/02/13  08:16:34  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.26  1996/02/13  04:26:23  psandova
 * CSCdi48685:  Fast explorers still miss-handles max-hops
 *
 * Fix typo.
 *
 * Revision 3.25  1996/02/12  16:13:25  psandova
 * CSCdi48685:  Fast explorers still miss-handles max-hops (see CSCdi47196)
 *
 * The complete fix for max-hops processing.
 *
 * Revision 3.24  1996/02/07  16:15:53  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.23  1996/02/06  03:02:41  szdravko
 * CSCdi37685:  Access-expression parsing is incosistent
 *
 * Revision 3.22  1996/02/01  06:10:36  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.21.2.1  1996/01/24  23:28:11  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.21  1996/01/22  07:16:50  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.20  1996/01/19  20:58:51  psandova
 * CSCdi47196:  Fast explorers miss-handles max-hops.
 *
 * Reject packets with hops less than max-hops value, not equal-to.
 *
 * Revision 3.19  1996/01/18  03:23:18  kchiu
 * CSCdi43616:  Router needs to set express buffer bit on RPS response
 *
 * Revision 3.18  1996/01/17  21:04:19  jrosen
 * CSCdi46182:  reconfiguring access-list breaks access-expression
 * - On outbound when fast switching explorers, we were incorrectly
 *   checking the inbound idb instead of the outbound idb to determine
 *   if there were any access lists applied.
 *
 * Revision 3.17  1996/01/17  04:55:19  kchiu
 * CSCdi18993:  Crash in rsrb_netbios
 *
 * Revision 3.16  1996/01/09  21:04:19  fred
 * CSCdi43225:  ipx ping fails with autonomous switching enabled on mip
 *              need to disable fair queuing on any interface that is
 *              autonomous switched, as SP/SSE don't do fancy queuin
 *
 * Revision 3.15  1996/01/05  10:25:15  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.14  1995/12/30  00:29:24  hou
 * CSCdi37413:  RTBR bridging fails for Ether->Token->Ether on vines and
 * ip.
 * - Set the default OUI for SNAP on token ring interface as 0x000000
 *   instead of 0x0000f8.
 * - Make low-end "ethernet-transit-oui" fully functional.
 *
 * Revision 3.13  1995/12/18  15:03:06  motto
 * When multiple i/f's are in same ring-group and do not share
 * the same bridge number, then explorers can get forwarded
 * with incorrect RIFs.
 * CSCdi45373:  Explorers not forwarded to target ring
 *
 * Revision 3.12  1995/12/15  22:43:26  rbatz
 * CSCdi43378:  Explorers destined for SDLLC Virtual Ring forwarded to
 *              wrong ring.  Rework of vr_virtualp required.
 *
 * Revision 3.11  1995/12/13  04:24:15  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.10  1995/12/13  02:28:42  ppearce
 * CSCdi45473:  %SYS-3-SUPNONE: Registry 6 doesnt exist output during boot
 *   (1) Rename fddi_srb_in() to fddi_to_tr()
 *         to parallel the tr_to_fddi() routine that already exists
 *   (2) Remove srt_fddi_in() registry from srt/srt_registry.reg
 *         and add media_fddi_to_tr() registry into h/media_registry.reg
 *
 * Revision 3.29.2.5  1996/03/20  17:55:11  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comment - name changes only
 *
 * Revision 3.29.2.4  1996/03/19  02:05:10  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Incorporate NetBios session access lists into NetBios subblock
 *
 * Revision 3.29.2.3  1996/03/17  18:50:13  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.29.2.2  1996/03/16  12:10:06  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.29.2.1  1996/02/28  21:24:58  ppearce
 * CSCdi50230:  IBU modularity - Remove unnecessary header file includes
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.2.1  1995/12/17  09:50:43  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.8  1995/12/11  10:20:37  ppearce
 * CSCdi44095:  SRB proxy-explorer causes FDDI packet drops
 *   rif_explore() should call tr_to_media_out() rather than datagram_out()
 *     so that packets can be correctly formatted for output to FDDI
 *
 *     - Rename srb_out() to tr_media_out()
 *     - Change srb_out() to invoke tr_fddi_out() for FDDI output
 *     - Move adjust_pak() code into tr_fddi_out()
 *
 * Revision 3.7  1995/12/07  22:33:00  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.6  1995/12/07  22:09:13  gschleic
 * CSCdi43561:  TR inputq wedge with specifically routed SRB frames
 * towards LNM MAC.  dlsw_demux should classify SRB packets with RIF which
 * end at source-bridge ring-group ring as D_DLSW only if DLSW is active.
 *
 * Revision 3.5  1995/12/06  19:28:03  monical
 * CSCdi42740:  Duplicate Bridge Definition Should Not Be Permitted
 *
 * Revision 3.4  1995/11/20  18:37:39  rbadri
 * CSCdi38662:  Configuring bridging on multiple ATM sub-interfaces causes
 *              hardware reset
 * Make span_fixup() recognize sub-interfaces, so that for ATM interfaces,
 * we can initiate a config change rather than a hardware reset.
 *
 * Revision 3.3  1995/11/17  18:57:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:42:52  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:32:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.36  1995/11/08  21:30:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.35  1995/10/26  00:30:02  speakman
 * CSCdi41837:  SMF calls malloc and free at interrupt (SYS-3-INVMEMINT)
 * Convert tbridge and srb to use chunk-based SMF.
 *
 * Revision 2.34  1995/09/11  20:23:31  richl
 * CSCdi39276:  AGS+ crashes in CLS, disable FTCP peertype
 *
 * Revision 2.33  1995/09/09  03:34:53  ppearce
 * CSCdi40032:  SRB discards non-SNAP encap'd frames
 *                 with 0x0806 data at type offset
 *
 * Revision 2.32  1995/09/08  22:14:19  kchiu
 * CSCdi38368:  ibm lnm cannot query the remote bridge adapter
 *
 * Revision 2.31  1995/09/02  22:28:23  alam
 * CSCdi39293:  SRB TR<->FDDI not forwarding Netbios UA frame
 * Fix explorer processing for SRB/FDDI.
 *
 * Revision 2.30  1995/09/01  23:06:10  asastry
 * CSCdi39698:  WFQ: Display info (in "show queue") for non-IP protocols
 * as well
 *
 * Revision 2.29  1995/09/01  06:02:18  ppearce
 * CSCdi39705:  MEMD-3-BADVCN when using CSNA with RSRB/TCP local-ack
 *   Re-work how CSNA header is added before sending pak to CIP interface
 *
 * Revision 2.28  1995/08/23  04:25:50  richl
 * CSCdi37283:  Add the interface description for duplicate bridge
 *
 * Revision 2.27  1995/08/12  07:40:42  fred
 * CSCdi38525:  rewrite fair queue conversation discriminator using IEEE
 * definitions
 *
 * Revision 2.26  1995/08/10  14:44:05  ppearce
 * CSCdi38300:  Add CSNA packet switching to 75xx (RSP)
 *   Code review comments
 *
 * Revision 2.25  1995/08/09  21:33:46  ppearce
 * CSCdi38300:  Add CSNA packet switching to 75xx (RSP)
 *
 * Revision 2.24  1995/08/08  21:52:44  jacobt
 * CSCdi38416:  Dont probe the destination by sending directed test frames
 * (proxy)
 *
 * Revision 2.23  1995/08/08  20:24:23  motto
 * CSCdi37835:  SRB fastswitching from FDDI to CIP internal LANs does not
 * work
 *
 * Revision 2.22  1995/08/08  16:49:59  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.21  1995/08/07 07:33:21  richl
 * CSCdi37283:  Duplicate Bridge definition on diabled port causes
 * bridging failures
 *              Issue warning for unsupported function, but do not block
 *              it.
 *
 * Revision 2.20  1995/08/07  05:44:11  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.19  1995/08/01  22:06:30  motto
 * Changed fast explorer vector routine to keep track of drops instead
 * of the caller.
 * Added support for CIP internal LAN fast explorers.
 * CSCdi37528:  CIP internal LAN SRB fastswitch explorer does not work
 *
 * Revision 2.18  1995/08/01  09:22:55  ogrady
 * CSCdi32342:  update TR_MAXPORTS.
 * Remove constant TR_MAXPORTS.  Determine max number of possible SRB
 * ports dynamically.
 *
 * Revision 2.17  1995/07/31  06:55:54  szdravko
 * CSCdi36139:  SRB doesnt check malloc return
 *
 * Revision 2.16  1995/07/29  05:49:28  ioakley
 * CSCdi35866:  Correct problem with "source-bridge spanning"
 *              configuration when bridge defined for IEEE/DEC
 *              spanning tree operation and both source-bridge
 *              and bridge-group specified on same interface.
 *
 * Revision 2.15  1995/07/25  18:55:18  ioakley
 * CSCdi37556:  During source-bridge command processing initialize
 *              srb_for_fddi status regardless of interface state.
 *
 * Revision 2.14  1995/07/24  13:08:18  fbordona
 * CSCdi34580:  DLS show circuit displays HALT PENDING NOACK after
 *              successful disc
 *
 * Revision 2.13  1995/07/21  00:35:59  ioakley
 * CSCdi36717:  Ensure that FDDI high end logic is able to detect an
 *              srb/fddi bridge and thereby initialize the control
 *              register for frame stripping.  Prevents explorer storms
 *              caused by unstripped frames running around the FDDI
 *              ring at wire speeds when a wrap of the FDDI ring occurs.
 *
 * Revision 2.12  1995/07/20  21:34:37  jacobt
 * CSCdi36718:  Explorers to non-existent destination cause broadcast
 * storms
 *
 * Revision 2.11  1995/07/13  20:16:23  kchiu
 * CSCdi36376:  Change bridge number from OS/2 LanManager, router reboots
 *
 * Revision 2.10  1995/06/26  20:45:17  rbatz
 * CSCdi30326:  Bytes and Packet information INVALID(sho int token ring)
 * When fast srb explorers were transmitted, outputs_fast[ACCT_PROTO_SRB]
 * and tx_cumbytes_fast[ACCT_PROTO_SRB] were not being incremented.  This
 * resulted in incorrect reporting of output stats for "show int tokX".
 *
 * Revision 2.9  1995/06/22  15:36:58  motto
 * CSCdi36240:  SRB fastswitching and process switching broken for CIP
 * v-interface
 *
 * Revision 2.8  1995/06/21  18:56:37  alam
 * CSCdi36191:  IBM spanning tree doesnt form with SRB/FDDI
 * Add sbridge_smf_update registry in srb_init.
 *
 * Revision 2.7  1995/06/21  02:06:50  alam
 * CSCdi36191:  IBM spanning tree doesnt form with SRB/FDDI
 * Insert IEEE spanning tree multicast address into SMF when AST over
 * SRB/FDDI is enabled.
 *
 * Revision 2.6  1995/06/20  07:19:13  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.5  1995/06/18  06:19:16  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.4  1995/06/15 23:00:44  ppearce
 * CSCdi35921:  CLS/RSRB virtual ring not removed after "appn stop"
 *
 * Revision 2.3  1995/06/13  00:02:00  rnaderi
 * CSCdi35729:  No check for LNM in demuxer
 *
 * Revision 2.2  1995/06/12  21:34:46  rnaderi
 * CSCdi30264:  DLSw Local peer causes router to reboot.
 * Handling the border condition when dealing with seven hops caused the
 * problem.
 * In the process made the following improvements.
 * 1. No more demuxing when dealing with explorers.
 * 2. Rif handling was moved to CSM entirely.
 * 3. dlsw_demux cleaned up to the deal with I frames and much shorter
 * path.
 *
 * Revision 2.1  1995/06/07  23:02:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "subsys.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "srt_registry.h"
#include "../cls/dlc_registry.h"
#include "../partner/partner_registry.h"
#include "../hes/sse_registry.h"
#include "interface_private.h" /* -> _generic when subblock work done */
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "ieee.h"
#include "address.h" /* Defines hwaddrtype for clsi_msg.h    */
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "../if/network.h"
#include "../if/priority.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../llc2/llc2.h"
#include "parser_defs_srb.h"
#include "rsrb.h"
#include "srb.h"
#include "srt_debug.h"
#include "lack_externs.h"
#include "lack2fsm.h"
#include "lack2_externs.h"
#include "access.h"
#include "../tcp/tcpdriver.h"
#include "span.h"
#include "../ibm/lanmgr.h"
#include "../ibm/ibmnm.h"
#include "../ibm/lanmgr_registry.regh"
/* need to remove this one...*/
#include "../ibm/netbios_sb.h"

#include "../srt/netbios_srb.h"  /* loosely coupled netbios - srb interface */
#include "access_expr.h"
#include "../cls/clsi_msg.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_dlu.h"
#include "../dlsw/dlsw_peer_externs.h" /* peek in lpeer for Ethernet support */
#include "../cls/dlc.h"
#include "../cls/cls_assert.h"
#include "bridgeaccess.h"
#include "../ibm/libibm_externs.h"
#include "../if/rif_util.h"
#include "../smf/smf_public.h"
#include "srb_core.h"
#include "srb_sb_private.h"
#include "../hes/channel_sb.h"

/*
 * Externals
 */
/*DLSw externs*/
extern void lack_start(void);

extern ipaddrtype rsrb_fst_name;
extern boolean enable_80d5;
extern boolean sap_8209_table[256];
extern boolean default_sap_8209_table[256];
extern boolean global_netbios_name_cache_enable;
extern int lack_passthru_rings[];
/* SRB AST support */
extern void rsrb_add_spanidb(spantype *, int, int);
extern void add_all_spanidb(int, spantype *);
extern boolean rsrb_permit_vidb_span(idbtype *);
extern short rsrb_fair_queue_flow_id(paktype *pak);

extern ulong vring_tcpqmax;

/*
 * Forward declarations
 */
static void srb_span_enable(idbtype *, spantype *, int);
static void srb_span_disable(idbtype *, spantype *);
static void srb_input(paktype *pak);

/*
 * Global Storage
 */

watched_queue *srb_explorerQ;
fs_explorer_cb fs_explorer;

boolean srb_running;
boolean lnx_running;

static pid_t srb_pid;
int sdllc_pid;

boolean sdllc_lak_enable;
boolean lnx_lak_enable;
boolean rsrb_cos_enable;
boolean srb_proxy_netbios_only;




/*
 * Source Route Bridging subsystem header
 */

#define SRB_MAJVERSION 1
#define SRB_MINVERSION 0
#define SRB_EDITVERSION  1
#define FAST_EXPLORER


SUBSYS_HEADER(srb, SRB_MAJVERSION, SRB_MINVERSION, SRB_EDITVERSION,
	      srb_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: srbcore, iphost, tbridge",
	      "req: srbcore, tbridge, ibu_int");


/*
 * srb_clear_counters
 *
 * clear counters associated with SRB. Also, call routine to do it for RSRB
 */
void srb_clear_counters (parseinfo *csb)
{
    hwidbtype *hwidb;
    idbtype   *swidb;
    srb_sb_t  *srb_sb;

    FOR_ALL_HWIDBS(hwidb) {
       FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
           srb_sb = srb_get_sb_inline(swidb);
           if (!srb_sb) {
               continue;
           }
           srb_sb->srb_rx_count     = 
           srb_sb->srb_rx_bytes     = 
           srb_sb->srb_tx_nb_count  = 
           srb_sb->srb_tx_b_count   =
           srb_sb->srb_tx_nb_bytes  = 
           srb_sb->srb_tx_b_bytes   =
           srb_sb->srb_tx_drops     = 
           srb_sb->srb_rx_drops     =
           srb_sb->srb_span_in      = 
           srb_sb->srb_allrte_in    =
           srb_sb->srb_span_out     = 
           srb_sb->srb_allrte_out   = 0;

           srb_sb->si_inputs_fast_srb
               = hwidb->counters.inputs_fast[ACCT_PROTO_SRB];
           srb_sb->si_rx_cumbytes_fast_srb 
               = hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB];
           srb_sb->si_outputs_fast_srb     
               = hwidb->counters.outputs_fast[ACCT_PROTO_SRB];
           srb_sb->si_tx_cumbytes_fast_srb 
               = hwidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB];
           srb_sb->si_inputs_auto_srb      
               = hwidb->counters.inputs_auto[ACCT_PROTO_SRB];
           srb_sb->si_rx_cumbytes_auto_srb 
               = hwidb->counters.rx_cumbytes_auto[ACCT_PROTO_SRB];
           srb_sb->si_outputs_auto_srb      
               = hwidb->counters.outputs_auto[ACCT_PROTO_SRB]; 
           srb_sb->si_tx_cumbytes_auto_srb 
               = hwidb->counters.tx_cumbytes_auto[ACCT_PROTO_SRB];
       }
       fs_clear_counts(hwidb);
    }
    rsrb_clear_counters();
}


/*
 * rsrb_print_network_header
 *
 * Print Source Route Bridged header(generalized)
 */
static void rsrb_print_network_header (paktype *pak, ushort **data)
{
    mac_hdr     *header;
    snap_hdr    *snap;

    header = (mac_hdr*) pak->mac_start;
    snap = (snap_hdr *) (header->data + getRifLenPak(header->daddr, RC_BROADBIT));
    /* unicast: source and destination */
    printf("\n  source: %e", header->saddr);
    printf(" destination: %e", header->daddr);
    if (SAP_SNAP == header->data[0]) {
	printf(" SNAP OUI: %02x-%02x-%02x PID %04x",
	       snap->oui[0], snap->oui[1], snap->oui[2], snap->type);
    } else {
	printf(" 802.2 DSAP: %02x SSAP: %02x CNTL %02x",
	       snap->dsap, snap->ssap, snap->control);
    }
}

/*
 * srb_background
 *
 * This process handles background tasks for source route bridging.
 * Included are the following:
 *
 * 	 1) Process delayed ring exchange requests.
 * 	 2) Draining and processing of the exchange queue.  This
 * 	    queue holds packets that will possibly change a remote
 * 	    peer's ring/bridge configuration.  These paks are ring
 * 	    exchange requests or replies from the other side.
 * 	 3) Draining and processing of the connection queue.  These
 * 	    are packets that need to be handed to the Connector after
 * 	    it is started up.  This start up can't happen at interrupt
 * 	    level.
 * 	 4) Draining and processing of the explorer queue.
 *
 * The first three items are handled by vring_background.
 *
 * Explorer packets on the explorer queue are processed in the last section.
 * If the packet came from a direct (dual port) half bridge it gets
 * munged directly and pushed out.  If it is from a multi-port virtual
 * ring half-bridged the munged packet is handed to the virtual ring
 * code for flooding.
 *
 * It is assumed that one or the other of the ways of handling this pak
 * will happen.  That is that either vring_blk or bridge_idb will be set.
 */

static process srb_background (void)
{
    paktype *pak, *newpak;
    hwidbtype *idb, *outidb;
    trrif_t *trh;
    int len, rd_cnt, insert_size, maxrif, mtu_lf;
    boolean spanning, flush, flush_dls;
    srbrd_t *trrd;
    char rif_string[SRB_MAXRIF*2+SRB_MAXRIF/2];
    tring_hdr *trframe;
    ushort riflen;
    tr_ventrytype *vre;
    sys_timestamp srb_bg_timer;
    sys_timestamp srb_timestamp;
    snap_hdr *llc1;
    srb_sb_t *srb_sb;

    process_wait_on_system_init();

    /*
     * Set up this process' world.
     *
     * Note that this process does not take advantage of the wakeup reasons
     * that the scheduler can provide.  Instead, as it did with the old
     * scheduler, it simply looks at all possible input sources.
     */
    process_watch_queue(vrxchgQ, ENABLE, RECURRING);
    process_watch_queue(vrconn_pakQ, ENABLE, RECURRING);
    process_watch_queue(srb_explorerQ, ENABLE, RECURRING);
    process_watch_boolean(vr_wake_up, ENABLE, RECURRING);
    TIMER_START(srb_bg_timer, ONESEC);
    process_watch_timer(&srb_bg_timer, ENABLE);

    srb_running = TRUE;

    while (TRUE) {

	process_wait_for_event();

	if (TIMER_RUNNING_AND_AWAKE(srb_bg_timer)) {
	    process_set_boolean(vr_wake_up, TRUE);
	    TIMER_UPDATE(srb_bg_timer, ONESEC);
	}

 	vring_background();

	GET_TIMESTAMP(srb_timestamp);

	while (TRUE) {
	    /*
	     * limit the time spent here
	     */
	    if (ELAPSED_TIME(srb_timestamp) > MAX_SRB_EXPLORERQ_TIME) {
		break;
	    }
	    pak = process_dequeue(srb_explorerQ);
	    if (!pak)
		break;
            idb = pak->if_input->hwptr;

            trh = (trrif_t *) pak->datagramstart;
            riflen = trh->rc_blth & RC_LTHMASK;

            /*
             * Explorers coming in RSRB Direct are now enqueued.
             */
            if (pak->flags & PAK_RSRB_EXPLORER) {
                int offset = (int) (pak->rif_match - pak->datagramstart);
                vrforward_explorer(pak, offset, idb->tr_ifpeer, 99);
                continue;
            }

            srb_sb = srb_get_sb_inline(idb->firstsw);
	    if (!srb_sb) {
                if (srberr_debug) {
                    buginf("SRB subblock does not exist\n");
	        }
                retbuffer(pak);
                continue;
	    }
	    
	    idb_increment_rx_proto_counts(pak, ACCT_PROTO_SRB);
	    pak->acct_proto = ACCT_PROTO_SRB;

	    if ((srb_sb->srb_bridge_num > SRB_MAX_BRIDGE_NUM) ||
		(idb->tr_bridge_idb == NULL && idb->tr_vring_blk == NULL) ) { /* rug yanked */
		if (srb_debug || srbevent_debug)
		  buginf("\nSRB%d: dropping bridged packet, disabled, srb_bridge_num %d tr_bridge_idb %d\n",
			 idb->unit, srb_sb->srb_bridge_num, idb->tr_bridge_idb);
		retbuffer(pak);
		continue;
	    }
	    if (!(pak->flags & PAK_SRB_IN) || !(trh->saddr[0] & TR_RII) ||
		  !(trh->rc_blth & RC_BROADBIT)) {
		if (srb_debug || srbevent_debug)
		    buginf("\nSRB%d: illegal explorer, s: %e d: %e rif: %s",
			   idb->unit, trh->saddr, trh->daddr,
			   ((trh->saddr[0] & TR_RII) ?
			    get_rif_str((uchar *) &trh->rc_blth, rif_string)
			      : ""));
                srb_sb_incr_rx_drops(idb->firstsw);
		retbuffer(pak);
		continue;
	    }
	    /*
	     * The following line was originally
	     * if ((trh->rc_dlf & RC_D) || (trh->rc_blth & 1))
	     * It has been modified to fix a misalignment caused 
	     * by trh being byte aligned. The compiler is taking
	     * the short word from trh->rc_blth to do the logic
	     * operation since the two byte fields are consecutive.
	     * The fix privileges code speed to readibility.
	     */
	    if (GETSHORT(&trh->rc_blth) & (0x100 | RC_D)) {
		if (srb_debug || srbevent_debug)
		    buginf("\nSRB%d: bad explorer control, D set or odd",
			   idb->unit);
                srb_sb_incr_rx_drops(idb->firstsw);
		retbuffer(pak);
		continue;
	    }
	    
	    /*
	     * Spanning or All Routes?
	     */
            len = trh->rc_blth & RC_LTHMASK;
	    spanning = ((trh->rc_blth & RC_RTMASK) == RC_B_SPAN);
	    if (spanning) {
		if (!srb_sb->srb_spanning_explorer) {
		    if (srb_debug || srbevent_debug)
			buginf("\nSRB%d: span dropped, input off, s: %e d: %e rif: %s",
			       idb->unit, trh->saddr, trh->daddr,
			       get_rif_str((uchar *)&trh->rc_blth, rif_string));
                    srb_sb_incr_rx_drops(idb->firstsw);
		    retbuffer(pak);
		    continue;
		}
                if ( len > ( (srb_sb->srb_maxin_hops + 1) * 2 + 2 ) ) {
                    srb_sb_incr_rx_drops(idb->firstsw);
                    retbuffer(pak);
                    continue;
                } 
		maxrif = SRB_MAXRIF;  /* I.E. No limit. */
	    } else {
		maxrif = (srb_sb->srb_max_hops + 1) * 2 + 2;
	    }

	    /*
	     * Knock down the maximum size based upon the input ring.
	     */
	    mtu_lf = bytes_to_rc_code(llc_mtu(idb));
	    if ((trh->rc_dlf & RC_LFMASK) > mtu_lf)
		trh->rc_dlf = mtu_lf;

            /*
             * If DLSw+ running, make copy of explorer and pass to DLSw+
             */
            if (reg_invoke_dlsw_running(0,0,0)) {
              if ((idb != NULL) && (idb->tr_vring_blk != NULL)) {

                  trrd = (srbrd_t *) &trh->rd[0];
                  flush_dls = FALSE;
                  for (rd_cnt = len - 2; rd_cnt > 0; rd_cnt -= 2) {
		    if ((GETSHORT(trrd) >> 4) == srb_sb->srb_targetring) {
                      if (srb_debug) 
                        buginf ("\nDLSW: explorer duplicate ring violation %s",
                             get_rif_str((uchar *) &trh->rc_blth, rif_string));
                      flush_dls = TRUE;
                      break;
                    }
                    trrd++;
                  }

                  if (!flush_dls) {
                    if ((newpak = pak_duplicate(pak))) {
                      if (is_fddi(idb)) {  /* already in token ring format */
                        newpak->enc_flags |= ENC_DLSW;
                        tring_parse_packet(idb, newpak, NULL);
                      }
                      else 
                        (*idb->parse_packet)(idb, newpak, NULL);
                      reg_invoke_dlsw_work(newpak, idb, riflen);
                    }
                  }
              }
            }

	    /*
	     * Virtual ring traversals consume 2 route descriptors (4 bytes)
	     * in the normal case.  Dual port traversals consume 1 RD (2
	     * bytes).
	     */
             vre = vr_virtualp(NULL, srb_sb->srb_targetring, trh->daddr);
             if (vre) {
 		insert_size = (vre->type & (RSRB_ET_SDLLC | RSRB_ET_QLLC | 
                                             RSRB_ET_LOCAL_VIRTUAL)) ? 4 : 2;
             }
	     else {
		insert_size = (idb->tr_vring_blk ? 4 : 2);
             }
	     if ((len == 2) && ((len + insert_size + 2) <= maxrif)) {
		insert_size += 2;	/* add 1st descriptor too */
		bcopy((uchar *)trh, (uchar *) trh - insert_size,
		      TRING_ENCAPBYTES + 2);
		(uchar *) trh -= insert_size;
		trh->rc_blth += insert_size;
		pak->datagramstart = (uchar *) trh;
		pak->datagramsize += insert_size;
		trrd = (srbrd_t *) &trh->rd[0];
		PUTSHORT(trrd, ((srb_sb->srb_thisring << 4) |
				(srb_sb->srb_bridge_num & 0x0f)));
		trrd++;		/* point at where target goes */
		PUTSHORT(trrd, ((srb_sb->srb_targetring << 4) & 0xfff0));
		if (insert_size > 4) {
		    if ((vre) && 
                        (vre->type & (RSRB_ET_SDLLC | RSRB_ET_QLLC | 
                                      RSRB_ET_LOCAL_VIRTUAL)) ) {
                        idbtype *swidb = firstsw_or_null(vre->outhole.idb);
                        srb_triplet_t srb_triplet;
                        srb_sb_get_triplet(swidb, &srb_triplet);
			PUTSHORT(++trrd, ((srb_triplet.localRing
				 << 4) & 0xfff0));
			trrd--;
			PUTSHORT(trrd, ((GETSHORT(trrd) & 0xfff0) | 
				    (srb_triplet.bridgeNum & 0x0f)));
			(++trrd)->ring_num = srb_triplet.localRing;
			(--trrd)->bridge_num = srb_triplet.bridgeNum;
		    } else {
			PUTSHORT(++trrd, 0);
			trrd--;
		    }
		}
	    } else if ((len >= 6) && ((len + insert_size) <= maxrif)) {
		/*
		 * We don't check for TR duplicate paths because the interface
		 * does this for the target.  Note that the virtual ring
		 * propagator must check this.
		 */
		if (spanning || is_fddi(idb)) {
		    trrd = (srbrd_t *) &trh->rd[0];
		    flush = FALSE;
		    for (rd_cnt = len - 2; rd_cnt > 0; rd_cnt -= 2) {
			if ((GETSHORT(trrd) >> 4) == srb_sb->srb_targetring) {
			    if (srb_debug || srbevent_debug)
				buginf("\nSRB%d: duplicate ring violation, s: %e d: %e rif: %s",
				       idb->unit, trh->saddr, trh->daddr,
				       get_rif_str((uchar *) &trh->rc_blth,
						   rif_string));
                            srb_sb_incr_rx_drops(idb->firstsw);
			    flush = TRUE;
			    break;
			}
			trrd++;
		    }
		    if (flush) {
			retbuffer(pak);
			continue;
		    }
		}
		if (!vre || (RIF_DEST_RING(trh) != srb_sb->srb_targetring)) {
		  bcopy((uchar *)trh, (uchar *) trh - insert_size,
			TRING_ENCAPBYTES + len);
		  (uchar *) trh -= insert_size;
		  trh->rc_blth += insert_size;
		  pak->datagramstart = (uchar *) trh;
		  pak->datagramsize += insert_size;
		  trrd = (srbrd_t *) ((uchar *) &trh->rd[0] + len - 4);
		  PUTSHORT(trrd, ((GETSHORT(trrd) & 0xfff0) | 
				  (srb_sb->srb_bridge_num & 0x0f)));
		  trrd++;
		  PUTSHORT(trrd, ((srb_sb->srb_targetring << 4) & 0xfff0));
		  if (insert_size > 2) {
		      if ((vre) &&
                          (vre->type & (RSRB_ET_SDLLC | RSRB_ET_QLLC | 
                                        RSRB_ET_LOCAL_VIRTUAL))) {
                          idbtype *swidb = firstsw_or_null(vre->outhole.idb);
                          srb_triplet_t srb_triplet;
                          srb_sb_get_triplet(swidb, &srb_triplet);
			  PUTSHORT(++trrd, ((srb_triplet.localRing
					     << 4) & 0xfff0));
			  trrd--;
			  PUTSHORT(trrd, ((GETSHORT(trrd) & 0xfff0) | 
				    (srb_triplet.bridgeNum & 0x0f)));
		      } else {
			  PUTSHORT(++trrd, 0);
			  trrd--;
		      }
		  }
		} else {
		  trrd = (srbrd_t *) ((uchar *) &trh->rd[0] + len - 4);
		}
	    } else {
		/*
		 * No room or yet another weird length.  Punt.
		 */
		if (srb_debug || srbevent_debug)
		    buginf("\nSRB%d: max hops reached - %d, s: %e d: %e rif: %s",
			   idb->unit, 
                           srb_sb->srb_max_hops, trh->saddr, trh->daddr,
			   get_rif_str((uchar *) &trh->rc_blth, rif_string));
                srb_sb_incr_rx_drops(idb->firstsw);
		retbuffer(pak);
		continue;
	    }
	    /*
	     * Explorer modified.  trrd points at where next bridge
	     * modification would go.  If direct (dual port) punch it and
	     * send it out.  If virtual hand off to the virtual explorer.
	     */
	    pak->flags |= PAK_SRB_EXPLORER;
	    if (vre) {
		/* make LNM happy by counting */
                if (idb->vidb_link && (idb->vidb_link != RSRB_INVALID_ADDRESS))
                {
                    srb_sb_incr_tx_b_counters(idb->vidb_link->firstsw,
                                              pak->datagramsize);
                }
                srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, 
                                        pak->datagramsize);
	        if (spanning)
                    srb_sb_incr_span_in_counter(idb->firstsw);
		else
                    srb_sb_incr_allrte_in_counter(idb->firstsw);
                vr_vre_input(vre, pak, FALSE, RSRB_OP_EXPLORER);
		continue;
	    }
	    if (idb->tr_vring_blk) {
		/* make LNM happy by counting */
                if (idb->vidb_link && (idb->vidb_link != RSRB_INVALID_ADDRESS))
                {
                    srb_sb_incr_tx_b_counters(idb->vidb_link->firstsw,
                                              pak->datagramsize);
                }
		vrforward_explorer(pak, (uchar *) trrd - pak->datagramstart,
				   NULL, idb->unit);
	    } else {			/* direct */
		outidb = idb->tr_bridge_idb;
                riflen = trh->rc_blth & RC_LTHMASK;
		llc1 = (snap_hdr *)((uchar *)&trh->rc_blth + riflen);

	        /* Check to see if this packet has already been fast 
		 * forwarded from interrupt level.  If not add this oidb 
		 * to the idb explorer fast forward queue and continue.  
	         * Only packets from an interface that can fastswitch 
		 * explorers are considered.  Note that checking 
		 * PAK_SRB_EXP_SENT after function call assures all
		 * enqueued packets are sent. */

                if((pak->flags & PAK_SRB_EXP_FAST) 
                && fs_explorer_check(spanning, idb, outidb)
		&& (pak->flags & PAK_SRB_EXP_SENT)) {
                    retbuffer(pak);
	            continue;
	            }

                if(spanning) {
                    ushort maxout_hops;
                    srb_sb_incr_span_in_counter(idb->firstsw);
		    if (!srb_sb_get_spanning_explorer(outidb->firstsw)) {
			  if (srb_debug || srbevent_debug)
			       buginf("\nSRB%d: span dropped, output off, s: %e d: %e rif: %s",
			       outidb->unit, trh->saddr, trh->daddr,
			       get_rif_str((uchar *)&trh->rc_blth, rif_string));
                          srb_sb_incr_tx_drops(outidb->firstsw);
			  retbuffer(pak);
			  continue;
		    }
                    /* check if pack exceeds output hop count limit. */
                    maxout_hops = srb_sb_get_maxout_hops(outidb->firstsw);
                    if ( riflen > ( (maxout_hops + 1) * 2 + 2 )) {
                        srb_sb_incr_tx_drops(outidb->firstsw);
                        retbuffer(pak);
                        continue;
                    }
                }
		else
                    srb_sb_incr_allrte_in_counter(idb->firstsw);

		/*
		 * Also knock down the maximum size based upon the output ring.
		 */
		mtu_lf = bytes_to_rc_code(llc_mtu(outidb));
		if ((trh->rc_dlf & RC_LFMASK) > mtu_lf)
		    trh->rc_dlf = mtu_lf;
		trframe = (tring_hdr *) pak->datagramstart;
		riflen = trframe->data[0] & RC_LTHMASK;

		if (!srb_out_check((uchar *) pak->datagramstart,
				  pak->datagramsize, outidb, riflen)) {
		    retbuffer(pak);
		    continue;
		}

                 /*  CIP internal LAN process-swtiched explorers filter */
                if (is_cip_lan(outidb)) {
                    channel_sb_t *chan_sb = channel_get_sb(outidb);
                    if ((chan_sb) && 
                        (chan_sb->get_vc_from_macaddr(outidb, trframe->daddr,
                                                trframe->saddr) == (ushort)NO_CHANNEL_VC)) {
                        srb_sb_incr_tx_drops(outidb->firstsw);
		        retbuffer(pak);
		        continue;
                    }
                }

		pak->if_output = outidb->firstsw;
		pak->acct_proto = ACCT_PROTO_SRB;
                srb_sb_incr_tx_b_counters(outidb->firstsw, pak->datagramsize);
		if (spanning)
                    srb_sb_incr_span_out_counter(outidb->firstsw);
		else
                    srb_sb_incr_allrte_out_counter(outidb->firstsw);

		if (srb_debug) {
		    buginf("\nSRB%d: sent explorer from %d to %d [%s]",
			   idb->unit, 
                           srb_sb->srb_thisring,
			   srb_sb->srb_targetring,
			   get_rif_str((uchar *) &trh->rc_blth, rif_string));
                }
		tr_to_media_out(pak);
		continue;
	    }
	}
    }
}

static ushort lackfsm(lak_t *lackie, int major, int minor)
{
    return(lak2_fsm(lackie, major, minor));
}

/*
 * srb_enabled
 * Return TRUE if SRB is running.
 */

boolean srb_enabled(void)
{
    return(srb_running);
}

/*
 * srb_on_idb
 * Return TRUE if SRB is configured on an interface.
 */

static boolean srb_on_idb (idbtype *idb)
{
    srb_triplet_t srb_triplet;
    srb_sb_get_triplet(idb, &srb_triplet);
    if (((srb_triplet.localRing > SRB_MAX_RING_NUM) || 
	 (srb_triplet.localRing <= 0)) ||
	((srb_triplet.bridgeNum > SRB_MAX_BRIDGE_NUM) || 
	 (srb_triplet.bridgeNum <= 0)) ||
	((srb_triplet.remoteRing > SRB_MAX_RING_NUM) || 
	 (srb_triplet.remoteRing <= 0))) {
	return(FALSE);
    }
    return(TRUE);
}

/*
 * return max possible number of srb interfaces in the chassis
 */
int get_max_srb_ports (void)
{
    hwidbtype *idb;
    int num_srb_ports = nTRs+nfddinets;

    FOR_ALL_HWIDBS(idb) {
        if (is_cip_lan(idb))
            num_srb_ports++;
    }
    return(num_srb_ports);
}


boolean
sbridge_smf_update (uchar *mac_addr, idbtype *swidb, ulong result)
{
    if (srb_on_idb(swidb))
	return(smf_insert_address(mac_addr, swidb, result));
    else
	return(smf_delete_address(mac_addr, swidb, result));
}


/*
 * tr_set_access_flags
 * Logical OR of all possible input and output access filtering
 */
static boolean idb_srb_access_in( hwidbtype *hwidb) {
  return (hwidb->srb_lsap_in || 
    hwidb->srb_type_in ||
    hwidb->srb_address_in || 
    hwidb->access_expression_in ||
    (*netbios_sb_ibm_iacf_is_enabled_p)(hwidb->firstsw));
}
static boolean idb_srb_access_out( hwidbtype *hwidb) {
  return (hwidb->srb_lsap_out || 
    hwidb->srb_type_out ||
    hwidb->srb_address_out || 
    hwidb->access_expression_out ||
    (*netbios_sb_ibm_oacf_is_enabled_p)(hwidb->firstsw));
}
static void tr_set_access_flags (hwidbtype *hwidb)
{
    hwidb->srb_access_in = idb_srb_access_in( hwidb);
    hwidb->srb_access_out = idb_srb_access_out(hwidb);
}


void srb_init (subsystype *subsys)
{
    srb_pid = NO_PROCESS;
    srb_running = FALSE;		/* no process yet */
    vring_init();
    lack2_init();
    global_proxy_explorer_enable = FALSE;
    srb_proxy_netbios_only = FALSE;

    /*
     * Setup explorer fastswitching stuff.
     */
    srb_explorerQ = create_watched_queue("SRB Explorer pkts",
					 DEFAULT_EXPLORERQ_DEPTH, 0);
    fs_explorer.newpak = NULL;
    fs_explorer.hcb = NULL;
    fs_explorer.enabled = TRUE;             /* Default to ON */
    fs_explorer.ARE_dup_filter = FALSE;     /* Default to OFF */
    fs_explorer_setup();

    /*
     * Set up debugging flags
     */
    srt_debug_init();

    /*
     * Register some functions
     */
    srb_media_registry();
    reg_add_raw_enqueue(LINK_SRB, srb_input, "srb_input");
    reg_add_raw_enqueue(LINK_RSRB, vrif_input, "vrif_input");
    reg_add_onemin(rsrb_periodic, "rsrb_periodic");
    reg_add_setup_global(bridge_setup_global, "bridge_setup_global");
    reg_add_setup_int(bridge_setup_int, "bridge_setup_int");
    reg_add_setup_end(bridge_setup_end, "bridge_setup_end");
    reg_add_srb_enabled(srb_enabled, "srb_enabled");
    reg_add_media_bia_discovered(rsrb_newbia, "rsrb_newbia");
    reg_add_tcp_listen(TR_RSRB_P0_PORT, vrtcpd_listen, "vrtcpd_listen");
    reg_add_tcp_listen(TR_RSRB_P1_PORT, vrtcpd_listen, "vrtcpd_listen");
    reg_add_tcp_listen(TR_RSRB_P2_PORT, vrtcpd_listen, "vrtcpd_listen");
    reg_add_tcp_listen(TR_RSRB_P3_PORT, vrtcpd_listen, "vrtcpd_listen");
    reg_add_tcp_access_check(TR_RSRB_P0_PORT, rsrb_active, "rsrb_active");
    reg_add_tcp_access_check(TR_RSRB_P1_PORT, rsrb_active, "rsrb_active");
    reg_add_tcp_access_check(TR_RSRB_P2_PORT, rsrb_active, "rsrb_active");
    reg_add_tcp_access_check(TR_RSRB_P3_PORT, rsrb_active, "rsrb_active");
    reg_add_srt_srb_multicast(srb_multicast_enq, "srb_multicast_enq");
    reg_add_bridge_enqueue(bridge_routefailed, "bridge_routefailed");
    reg_add_srt_find_virtual_port(rsrb_find_virtual_bridge,
				  "rsrb_find_virtual_bridge");
    reg_add_srt_srb_forward(srb_common, "srb_common");
    reg_add_srt_flush_explorer(srb_flush_explorer, "srb_flush_explorer");
    reg_delete_vrfst_input();
    reg_add_vrfst_input(vrfst_input, "vrfst_input");
    reg_add_srt_lackfsm(lackfsm, "lackfsm");
    reg_add_proto_on_swidb(LINK_SRB, srb_on_idb, "srb_on_idb");
    reg_add_srt_permit_vidb_span(rsrb_permit_vidb_span, "rsrb_permit_vidb_span");
    reg_add_srt_add_spanidb(rsrb_add_spanidb, "rsrb_add_spanidb");
    reg_add_srt_add_all_spanidb(add_all_spanidb, "add_all_spanidb");
    reg_add_srt_span_index(get_srb_span_index, "get_srb_span_index");
    reg_add_srt_remove_netbios_wan_filter(rsrb_netbios_delete_list,
                                          "rsrb_netbios_delete_list");
    /*	note that *if* WFQ is configured on a token ring interface,
	srb will use the same discriminator as rsrb uses */
    reg_add_determine_fair_queue_flow_id(LINK_SRB, rsrb_fair_queue_flow_id,
					 "srb_fair_queue_flow_id");    
    reg_add_determine_fair_queue_flow_id(LINK_RSRB, rsrb_fair_queue_flow_id,
					 "rsrb_fair_queue_flow_id");    
    reg_add_print_network_header(LINK_RSRB, rsrb_print_network_header,
				 "rsrb_print_network_header");
    reg_add_xmacaccesscheck_gen(xmacaccesscheck_gen, "xmacaccesscheck_gen");
    reg_add_cls_CreateLak(cls_CreateLak, "cls_CreateLak");
    reg_add_cls_remove_vring(cls_remove_vring_interface, "cls_remove_vring");
    reg_add_trmac2lak(trmac2lak, "trmac2lak");
    reg_add_srb_activate_ring(activate_ring, "activate_ring");
    reg_add_srb_deactivate_ring(deactivate_ring, "deactivate_ring");
    reg_add_sbridge_smf_update(sbridge_smf_update, "sbridge_smf_update");

    reg_add_rsrb_is_pak_from_lak_peer(rsrb_is_pak_from_lak_peer, 
                                      "rsrb_is_pak_from_lak_peer");
    reg_add_srb_out_check(srb_out_check, "srb_out_check");

    tr_set_access_flags_p = & tr_set_access_flags;
    srb_access_in_p   = & idb_srb_access_in;
    srb_access_out_p  = & idb_srb_access_out;
}

boolean srb_multicast_enq (
    idbtype *swidb,
    paktype *pak,
    boolean makeowncopy)
{
    trrif_t *trh;
    char rif_string[SRB_MAXRIF*2+SRB_MAXRIF/2];
    int riflen;
    tring_hdr *trframe;
    hwidbtype *idb = swidb->hwptr;
    hwidbtype *oidb;

    if (idb->status & IDB_FDDI) {
	fddi_to_tr(pak, idb);
    }

    trh = (trrif_t *) pak->datagramstart;
    trframe = (tring_hdr *) pak->datagramstart;
    riflen = trframe->data[0] & RC_LTHMASK;

    if (!srb_in_check((uchar *) pak->datagramstart, pak->datagramsize, idb, riflen)) {
	if (!makeowncopy)
	    retbuffer(pak);
	return(TRUE);
    }
    if ((idb->tr_bridge_idb == NULL && idb->tr_vring_blk == NULL) ||
	!(trh->saddr[0] & TR_RII)) {
        if (srb_debug || srberr_debug)
	    buginf("\nSRB%d: no path multi, s: %e d: %e rif: %s",
		   idb->unit, trh->saddr, trh->daddr,
		   ((trh->saddr[0] & TR_RII) ?
		    get_rif_str((uchar *) &trh->rc_blth, rif_string) : ""));
        srb_sb_incr_rx_drops(idb->firstsw);
	if (!makeowncopy)
	    datagram_done(pak);
	return(TRUE);
    }

    pak = makeowncopy ? pak_duplicate(pak) : pak;

    if (pak) {
    	oidb  = idb->tr_bridge_idb;
	pak->flags |= PAK_SRB_IN;

	if (trh->rc_blth & RC_BROADBIT) {	/* an explorer? */
	    srb_enq(srb_explorerQ, pak);
	    if (srb_debug) {
              srb_triplet_t srb_triplet;
              srb_sb_get_triplet(swidb, &srb_triplet);
	      buginf("\nSRB%d: explorer enqueued (srn %d bn %d trn %d)",
		     idb->unit,
                     srb_triplet.localRing,
                     srb_triplet.bridgeNum,
                     srb_triplet.remoteRing);
            }
	} else if (oidb) {
	    if (!srb_out_check(pak->datagramstart, pak->datagramsize,
			      oidb, riflen)) {
		retbuffer(pak);
		return(TRUE);
	    }
	    pak->if_output = oidb->firstsw;
	    pak->flags |= PAK_SRB_OUT;
            srb_sb_incr_tx_nb_counters(oidb->firstsw, pak->datagramsize);
	    pak->acct_proto = ACCT_PROTO_SRB;
	    if (srb_debug) {
              srb_triplet_t srb_triplet;
              srb_sb_get_triplet(swidb, &srb_triplet);
	      buginf("\nSRB%d: direct multi (srn %d bn %d trn %d)",
		     idb->unit,
                     srb_triplet.localRing,
                     srb_triplet.bridgeNum,
                     srb_triplet.remoteRing);
            }
	    datagram_out(pak);
	} else
	  vrforward_pak(pak, FALSE, 0, RSRB_RIFLF_LEAVEALONE);
    }
    return(TRUE);
}

void srb_enq (watched_queue *qptr, paktype *pak)
{
    if (!srb_running) {
        retbuffer(pak);
        return;
    }

    if (qptr == srb_explorerQ){
        if (!explorer_enq(pak)) {
            srb_sb_incr_rx_drops(pak->if_input);
            retbuffer(pak);
        }
        return;
    }

    process_enqueue_pak(qptr, pak);
}

/*
 * srb_command: parse source-bridge configuration commands.
 *
 * Summary:
 *
 *   Global:
 *
 * 	[no] source-bridge ring-group  <ring-group>
 * 	[no] source-bridge remote-peer <ring-group> <addr type> <addr>
 *      [no] source-bridge remote-peer-keepalive <secs>
 * 	[no] source-bridge connection-timeout <secs>
 *	[no] source-bridge transparent <ring-group> <pseudo-ring> <bridge-num>
 *				       <transparent-bridge-group> <oui-keyword>
 *				       {enable-name-cache}
 *      [no] source-bridge enable-80d5
 *      [no] source-bridge old-oui
 *      [no] source-bridge sdllc-local-ack
 *      [no] source-bridge qllc-local-ack
 *      [no] source-bridge cos-enable
 *	[no] source-bridge proxy-netbios-only
 *	[no] source-bridge explorerQ-depth <#>
 *	[no] source-bridge explorer-maxrate <#>
 *	[no] source-bridge explorer-fastswitch
 *	[no] source-bridge transparent-fastswitch
 *
 *
 *   Interface:
 *
 * 	no   source-bridge
 * 	     source-bridge <local-ring> <bridge-num> <target-ring>
 * 	[no] source-bridge max-rd <#>
 * 	[no] source-bridge spanning [auto]
 * 	[no] source-bridge old-sna
 * 	[no] source-bridge route-cache cbus
 *	[no] source-bridge route-cache sse
 *
 *
 * Global commands:
 *
 *     [no] source-bridge ring-group <ring-group>
 *
 * Establishes or removes a ring-group.  A ring-group establishes a virtual
 * ring that can be used to build multi-port token ring source bridges or
 * multi-port remote bridges.  A given interface is in a ring-group if its
 * target ring number is the ring-group #.  In the remote source bridge case
 * the virtual ring actually encompasses all of the intervening links between
 * the source ring interface and the ultimate destination.  It is similar but
 * no where near the same as bridge-group for the spanning tree transparent
 * bridges.
 *
 * This command's functionality could be consumed by that of remote-peer.
 * It's presence makes removing a ring group much easier but primarily its
 * here for playing with SBDP (Source Bridge Discovery Protocol).  Cost is
 * slight.
 *
 *
 *   [no] source-bridge remote-peer <ring-group> <addr type> <addr>
 *
 * ie. source-bridge remote-peer 5 tcp 131.108.2.68
 *
 * This command establishes a remote peer for the specified ring group.  If
 * the group doesn't exist it is created.  Ring discovery is used to determine
 * which rings are configured on the peer.  Any changes made to source bridge
 * ring configuration will be communicated dynamically.
 *
 * There are two types of remote-peers, tcp and interface.  TCP peers use
 * TCP/IP to communicate.  Interface peers communicate directly over the
 * media.   TCP peers are established as shown above.  Interface peers use
 * the following form:
 *
 * 	source-bridge remote-peer 5 interface Serial0
 * 
 *
 *      source-bridge remote-peer-keepalive <secs>
 *      source-bridge remote-peer-keepalive
 *   no source-bridge remote-peer-keepalive
 *
 * ie. source-bridge remote-peer-keepalive <secs>
 *
 * This command enables remote-peer keepalives.  These keepalives are used to
 * verify that the remote peer is reachable.  If a keepalive fails, the peer
 * connection is torn down and any local acknowledged sessions are reset.  One
 * keepalive is sent every <secs> secs.  If RSRB_KEEPALIVE_MISSES (currently
 * 2) are missed then the peer is declared down.  Note that any incoming
 * traffic flags the far side as being alive.  The keepalive message
 * (VersionReq) is simply used when things are otherwise idle.
 *
 * The command "no source-bridge remote-peer-keepalive" disables the keepalive
 * mechanism.  The form "source-bridge remote-peer-keepalive" sets the
 * keepalive interval back to the default of 30 secs.  10 secs is the minimum
 * and 300 secs is the maximum that can be specified.
 *
 *
 * 	  source-bridge connection-timeout <secs>
 *     no source-bridge connection-timeout
 *
 * ie. source-bridge connection-timeout 60
 *
 * This establishes the amount of time between first open until a timeout
 * is declared.  It is useful when the topology between bridged rings is
 * large.  Defaults to 10 secs.  The "no" form reestablishes the default.
 *
 *
 * There is no ring number translation done.  That is the target ring number
 * seen at the local bridge must match one of the ring numbers present at the
 * destination tr source bridge server.
 *
 * Neither is there a bridge number translation.  The virtual ring mechanism as
 * implemented by the ring-group is used to transfere bridged packets remotely.
 * All intervening networks between here and the other bridge are considered
 * part of the virtual ring as specified by the ring-group number.
 *
 *   [no] source-bridge sdllc-local-ack
 *
 * This command will locally terminate the llc2 session of the SDLLC session at
 * the router.  The "no" option will terminate the llc2 session at the router
 * containing the serial line.
 *
 *   [no] source-bridge qllc-local-ack
 *
 * This command will locally terminate the llc2 session of the QLLC session at
 * the router.  The "no" option will terminate the llc2 session at the router
 * containing the serial line interface to the X.25 network.
 *
 *
 * Interface commands:
 *
 *        source-bridge <local-ring> <bridge-num> <target-ring>
 *     no source-bridge
 *
 * This command establishes an interface's bridge configuration.  It must be
 * consistent with the rest of the network configuration.  Ie.  Other bridges
 * on the same ring must have the same local-ring number.  We currently do not
 * have provision for getting this information from a ring parameter server
 * (RPS).  No checks are made to see if indeed all of these conditions are
 * met.  Eventually we plan on implementing the source bridge server as a RPS.
 *
 * <local-ring> establishes the ring number of the ring this interface is
 * connected to.  Under no circumstances should it be a virtual ring number.
 *
 * <bridge-num> establishes the bridge number for this interface on this ring.
 *
 * <target-ring> defines the ring number corresponding to the other side of
 * this bridge.  Ordinarily I would allow multiple targets but due to
 * limitations in the hardware implementations of the Token Ring chipsets
 * there is only a single target.  If the target ring is a ring-group this
 * restriction is removed.  Multiple targets are the rings off of the virtual
 * ring.  Specifing a ring-group (virtual ring number) for the target-ring
 * makes this interface and the local ring part of that ring group.  When this
 * command is parsed and executed successfully the interface is restarted with
 * the new parameters.  It is reset.
 *
 * No source-bridge turns off bridging by this interface.  The interface is
 * restarted with the new parameters.
 *
 *
 *        source-bridge max-rd <#>
 *     no source-bridge max-rd
 *
 * Establishes the maximum number of routing descriptors allowed in the
 * routing information field by this interface.  This must be between 2 and
 * TRSRB_MAX_MAXRD (defined to be 8 for initial release [IBM default] and 14
 * for IEEE conformance) inclusive.  This establishes the maximum number of
 * bridges a given packet may traverse before it is dumped.  The number of
 * bridges is max_rd-1.  "no source-bridge max-rd" puts the max-rd paramter
 * back to its default which is 8 (TRSRB_DEF_MAXRD).
 *
 *     [no] source-bridge spanning	[auto]	(default off)
 *
 * Enables or disables a port to forward spanning route explorer packets.  It
 * defaults to off even though this is different from what the IBM bridges do.
 * The reason for this is it is safer.  An administrator has to explictily
 * do something to turn this on.  If we defaulted to on it would be real
 * easy to get routing loops.  The software would catch this but it is ugly
 * to do this kind of stuff by default.
 *
 * The auto key word denotes that this port should participate in the IEEE
 * 802.5D spanning tree algorithm.  This is currently not implemented.
 *
 * 	[no] source-bridge old-sna
 *
 * Enables or disable old-sna hackery.  When enabled certain XID and TEST
 * packets that come in with an 02xx rif will be written as c2xx and
 * passed on to the srb background process for propagation.
 *
 *	[no] input-address-list <list>
 *	[no] input-lsap-list <list>
 *	[no] input-type-list <list>
 *	[no] output-address-list <list>
 *	[no] output-lsap-list <list>
 *	[no] output-type-list <list>
 *
 * Enable or disable filtering of source route bridged packets on this interface.
 */


void srb_command (parseinfo *csb)
{
    int vrn;
    rsrb_cookie cookie;
    int type, largest, backup_group;
    boolean local_ack = FALSE;
    boolean rnr_disable = FALSE;
    int force_version = 0;
    boolean priority = FALSE;
    tr_vgrouptype *vrg;
    hwidbtype *tmpidb;
    hwidbtype *vhwidb;
    idbtype   *vswidb;
    uchar macaddr[IEEEBYTES];
    ulong tudp_name;
    ulong transit_oui = (ulong) DEFAULT_OUI;
    tr_vpeertype *vrp;
    netbios_acclisttype *netbios_bytes_list = NULL;
    netbios_acclisttype *netbios_host_list = NULL;
    ulong tcprcvwnd = RSRB_TCP_WINDOW_SIZE;
    queuetype overflow;
    paktype *pak;
    ulong srb_queue_size;

    /*
     * There is no reason to enable source route bridging
     * if I am booting. CSCdi10998
     */
    if (system_loading)
        return;

    if (!bridge_enable) {
	printf("\n%%Source Bridging not supported in this release");
	return;
    }

    if (csb->nvgen) {
	/* Ring group info needs to be generated in order for each
	 * ring group.  To do this, the parser must call here
	 * without generating any of the subcommand keywords.
	 */
	tr_vgrouptype *vrg;
	int i;

	nv_write(rsrb_fst_name, "%s fst-peername %i", csb->nv_command,  rsrb_fst_name);
	nv_write(enable_80d5, "%s enable-80d5", csb->nv_command);
	for (i = 0; i <= 0xff; i++) {
	    if (sap_8209_table[i] != default_sap_8209_table[i]) {
		if (sap_8209_table[i]) {
		    nv_write(TRUE, "%s sap-80d5 %02x", csb->nv_command, i);
		} else {
		    nv_write(TRUE, "no %s sap-80d5 %02x", csb->nv_command, i);
		}
	    }
	}
        if (rsrb_keepalive_interval == 0 )
	    nv_write(TRUE, "no %s keepalive ", csb->nv_command);
        else if (rsrb_keepalive_interval != RSRB_KEEPALIVE_DEFAULT_INTERVAL)
	    nv_write(TRUE, "%s keepalive %d", csb->nv_command, rsrb_keepalive_interval/ONESEC);
	for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
	    nv_write(TRUE, "%s ring-group %d", csb->nv_command, vrg->vrn);
	    nv_write(vrg->largest_frame != RC_LFINITIAL,
		     "%s largest-frame %d %d", csb->nv_command,
		     vrg->vrn, rc_code_to_bytes(vrg->largest_frame));
	    for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
		nv_write(TRUE, "%s remote-peer %d ", csb->nv_command,
			 vrg->vrn);
		switch (vrp->type) {
		  case RSRB_PT_TCPD:
		    if (vrp->backup_group != RSRB_BACKUP_GROUP_NONE) {
			nv_add(TRUE, "tcp %i%s backup-group %d",
			       vrp->ipaddr,
			       (vrp->local_ack ? " local-ack" : ""),
			       vrp->backup_group);
		    } else {
			nv_add(TRUE, "tcp %i%s%s%s",
			       vrp->ipaddr,
			       (vrp->local_ack ? " local-ack" : ""),
			       (vrp->priority ? " priority" : ""),
			       (vrp->rnr_disable ? " rnr-disable" : ""));
		    }
		    if (vrp->tcp_rcv_window != RSRB_TCP_WINDOW_SIZE)
			nv_add(TRUE, " tcp-receive-window %d",
			       vrp->tcp_rcv_window);
		    break;

		  case RSRB_PT_FST:
		    if (vrp->backup_group != RSRB_BACKUP_GROUP_NONE) {
			nv_add(TRUE, "fst %i backup-group %d", vrp->ipaddr,
			       vrp->backup_group);
		    } else {
			nv_add(TRUE, "fst %i", vrp->ipaddr);
		    }
		    break;
		  case RSRB_PT_IF:
		    if (vrp->backup_group != RSRB_BACKUP_GROUP_NONE) {
			nv_add(TRUE, "interface %s backup-group %d",
			       vrp->outidb->hw_namestring, vrp->backup_group);
		    } else {
			nv_add(TRUE, "interface %s", vrp->outidb->hw_namestring);
		    }
		    break;
		  case RSRB_PT_FR:
		    if (vrp->backup_group != RSRB_BACKUP_GROUP_NONE) {
			nv_add(TRUE, "frame-relay interface %s %d backup-group %d",
			       vrp->outswidb->namestring, vrp->dlci, vrp->backup_group);
		    } else {
			nv_add(TRUE, "frame-relay interface %s %d", 
				vrp->outswidb->namestring, vrp->dlci);
		    }
		    break;
		  case RSRB_PT_LAN:
		    if (vrp->backup_group != RSRB_BACKUP_GROUP_NONE) {
			nv_add(TRUE, "interface %s %e backup-group %d",
			       vrp->outidb->hw_namestring, vrp->macaddr,
			       vrp->backup_group);
		    } else {
			nv_add(TRUE, "interface %s %e",
			       vrp->outidb->hw_namestring, vrp->macaddr);
		    }
		    break;
		  case RSRB_PT_NONE:
		    break;
		}
		if (vrp->largest_frame != vrp->largest_frame_def)
		    nv_add(TRUE, " lf %d",
			   rc_code_to_bytes(vrp->largest_frame));
		if (vrp->netbios_h_oacf) {
		    nv_add(TRUE, " host-netbios-out %s",
			   vrp->netbios_h_oacf->name);
		}
		if (vrp->netbios_b_oacf) {
		    nv_add(TRUE, " bytes-netbios-out %s",
			   vrp->netbios_b_oacf->name);
		}
	    }
	    if (vrg->virtual_bridge) {
                ulong rsrb_transit_oui;
		vswidb = vrg->virtual_bridge;
		vhwidb = vswidb->hwptr;
                rsrb_transit_oui = srb_sb_get_rsrb_transit_oui(vswidb);
		nv_write((boolean)vrg->virtual_bridge,
			 "%s transparent %d %d %d %d", csb->nv_command,
			 vrg->vrn, 
                         srb_sb_get_thisring(vswidb),
                         srb_sb_get_bridge_num(vswidb),
			 vrg->virtual_bridge->span_index);
		nv_add((rsrb_transit_oui != DEFAULT_OUI),
		       " %s",
		       ((rsrb_transit_oui == ETHEROUI) ? 
                           "90-compatible" :
			   ((rsrb_transit_oui == CISCOOUI) ?
			       "cisco" : "unknown")));
		
		nv_add(netbios_sb_name_caching_is_enabled(vhwidb->firstsw),
		       " enable-name-cache");

		nv_write(vhwidb->span_bridge_process_force,
		        "no %s transparent %d fastswitch", 
			csb->nv_command, vrg->vrn);
	    }
	}
        nv_write(fs_explorer.data_maxrate != 
                 fs_explorer.data_maxrate_dflt / MAXRATE_PERIOD,
                 "%s explorer-maxrate %d",
                 csb->nv_command,fs_explorer.data_maxrate*MAXRATE_PERIOD);

        nv_write(process_queue_max_size(srb_explorerQ) !=
		 DEFAULT_EXPLORERQ_DEPTH,
                 "%s explorerQ-depth %d",
                 csb->nv_command, process_queue_max_size(srb_explorerQ));

        nv_write(fs_explorer.enabled == FALSE, 
                 "no %s explorer-fastswitch", csb->nv_command);

        nv_write(fs_explorer.ARE_dup_filter == TRUE,
                 "%s explorer-dup-ARE-filter", csb->nv_command);

	nv_write(sdllc_lak_enable, "%s sdllc-local-ack", csb->nv_command);
	nv_write(lnx_lak_enable, "%s qllc-local-ack", csb->nv_command);
	nv_write(rsrb_cos_enable, "%s cos-enable", csb->nv_command);
	nv_write(srb_proxy_netbios_only, "%s proxy-netbios-only",
		 csb->nv_command);
	nv_write(vrconn_timeout != RSRB_REMOPEN_DEFTMO,
		 "%s connection-timeout %d", csb->nv_command, vrconn_timeout/ONESEC);
	nv_write(vring_tcpqmax != RSRB_TCP_MAX_QUEUE_DEFAULT,
		 "%s tcp-queue-max %d", csb->nv_command, vring_tcpqmax);
	nv_write(lack_connect_timeout != RSRB_LACK_CONNECTION_TIMEOUT_DEFAULT,
		 "%s lack-connection-timeout %d", csb->nv_command, lack_connect_timeout);
	for (i = 0; i < LACK_MAX_PASSTHRU_RINGS; i++) {
	      if (lack_passthru_rings[i])
		  nv_write(TRUE, "%s passthrough %d", csb->nv_command,
			   lack_passthru_rings[i]);
	}
	return;
    }

    switch (csb->which) {
      case SRB_EXPLORER_FASTSWITCH:
        fs_explorer.enabled = csb->sense;
        fs_explorer_setup();
        break;
      case SRB_EXPLORER_DUP_ARE_FILTER:
        fs_explorer.ARE_dup_filter = csb->sense;
        break;
      case SRB_EXPLORER_MAXRATE:
        if(csb->sense)
            fs_explorer.data_maxrate = GETOBJ(int, 1) / MAXRATE_PERIOD;
        else 
            fs_explorer.data_maxrate = 
                        fs_explorer.data_maxrate_dflt / MAXRATE_PERIOD;
        break;
      case SRB_EXPLORERQ_DEPTH:
	queue_init(&overflow, 0);
	srb_queue_size = csb->sense ? GETOBJ(int,1) : DEFAULT_EXPLORERQ_DEPTH;
	process_resize_queue(srb_explorerQ, srb_queue_size, &overflow);
	while ((pak = dequeue(&overflow)) != NULL)
	    retbuffer(pak);
        break;
      case SRB_LACK_CON_TIMEOUT:
	if (csb->sense) {
	    set_rsrb_lack_connect_timeout(GETOBJ(int,1));
	} else {
	    set_rsrb_lack_connect_timeout(RSRB_LACK_CONNECTION_TIMEOUT_DEFAULT);
	}
	break;
      case SRB_LACK_PASSTHRU:
	if (csb->sense) {
	    lack_enter_passthru_ring(GETOBJ(int,1));
	} else {
	    lack_delete_passthru_ring(GETOBJ(int,1));
	}
	break;
      case SRB_TCPQMAX:
	if (csb->sense) {
	    set_rsrb_tcpqlen(GETOBJ(int,1));
	} else {
	    set_rsrb_tcpqlen(RSRB_TCP_MAX_QUEUE_DEFAULT);
	}
	break;
      case SRB_FRAMESIZE:
	if (csb->sense) {
	    set_rsrb_largest(GETOBJ(int,1), GETOBJ(int,2));
	} else {
	    set_rsrb_largest(GETOBJ(int,1), 0);
	}
	break;
      case SRB_RINGGR:
	if (csb->sense) {
	    if (find_ringidb(GETOBJ(int,1))) {
		printf("\n%%Local ring number not allowed");
                return;
	    }
	    if (!create_vring(GETOBJ(int,1), GETOBJ(hwaddr,1)->addr)) {
		printf(nomemory);
                return;
	    }
	} else {
	    destroy_vring(GETOBJ(int,1), GETOBJ(hwaddr,1)->addr);
	}
	reg_invoke_set_ibmnm_interfaces(NULL, FALSE);
        reg_invoke_dlsw_parse_srb_if(csb->sense);
	break;

      case SRB_REMPR:
	vrn = GETOBJ(int,1);
	cookie.idb = NULL;
	type = RSRB_PT_NONE;
	ieee_copy(zeromac, macaddr);
	switch (GETOBJ(int,2)) {
	  case SRB_REMPR_FTCP:
	    printf("\n%%FTCP no longer supported.  Converting to TCP peertype.");
	  case SRB_REMPR_TCP:
	    cookie.ip = GETOBJ(paddr,1)->ip_addr;
	    type = RSRB_PT_TCPD;
	    break;
	  case SRB_REMPR_FR:
	    cookie.fr_cookie.swidb = GETOBJ(idb,1); 
	    cookie.fr_cookie.dlci = (ushort) GETOBJ(int, 14);
	    type = RSRB_PT_FR;
	    break;
	  case SRB_REMPR_INT:
	    cookie.idb = GETOBJ(idb,1)->hwptr;
	    if (is_atm(cookie.idb)) {
		printf("\n%% RSRB over Direct Encapsulation not supported for ATM");
		return;
	    }
	    if (GETOBJ(idb,1)->hwptr->status & IDB_SERIAL) {
		type = RSRB_PT_IF;
	    } else {
		if (GETOBJ(idb,1)->hwptr->status & IDB_IEEEMASK) {
		    ieee_copy(GETOBJ(hwaddr,1)->addr, macaddr);
		    type = RSRB_PT_LAN;
		}
	    }
	    break;
	  case SRB_REMPR_FST:
	    cookie.ip = GETOBJ(paddr,1)->ip_addr;
	    type = RSRB_PT_FST;
	    break;
	  default:
	    printf("\n%% Invalid type, interface, or address");
	    return;
	}

	/* lf <size> */
	if (GETOBJ(int,3) != -1) {
	    largest = GETOBJ(int,3);
	} else {
	    largest = RC_LFINITIAL;
	}

	/* local-ack */
	if (GETOBJ(int,4)) {
	    if (type != RSRB_PT_TCPD) {
		printf("\n%%local-ack only allowed on TCP remote peers.");
		return;
	    }
	    local_ack = TRUE;
            if (csb->sense) {
            /*
             * only start things up if we are turning things on
             */
	       llc2_start();
	       lack_start();
	    }
	}

	/* version <num> */
	if (GETOBJ(int,5)) {
        /*
         * version forcing is going away with newer versions of rsrb starting
         * with rsrb verison 3.  it is silently ignored.
         */
	    force_version = 0;
	    printf ("\nWARNING: Version forcing is no longer supported. The version keyword is ignored");
	}

	/* backup-group <group> */
	if (GETOBJ(int,7)) {
	    backup_group = GETOBJ(int,8);
	}else{
	    backup_group = RSRB_BACKUP_GROUP_NONE;
	}

	/* host-netbios-out <netbios-list-name> */
	if (GETOBJ(int,10)) {
	    if (csb->sense) {
		netbios_host_list =
		    find_namedthing(&netbios_host_accQ, GETOBJ(string,1));
		if (netbios_host_list == NULL) {
		    printf("\n%%Netbios host list %s not found", 
			   GETOBJ(string,1));
		    return;
		}
	    } else {
		netbios_host_list = NULL;
	    }
	}

	/* bytes-netbios-out <netbios-bytes-list-name> */
	if (GETOBJ(int,11)) {
	    if (csb->sense) {
		netbios_bytes_list =
		    find_namedthing(&netbios_bytes_accQ, GETOBJ(string,2));
		if (netbios_bytes_list == NULL) {
		    printf("\n%%Netbios bytes list %s not found", 
			  GETOBJ(string,2));
		    return;
		}
	    } else {
		netbios_bytes_list = NULL;
	    }
	}

	/* priority */
	if (GETOBJ(int,12)) {
	    if (!local_ack) {
		printf("\nprioritization only allowed on local-ack remote peers");
		return;
	    }
	    priority = TRUE;
	}

	/* rnr-disable */
	if (GETOBJ(int,13)) {
	    if (!local_ack) {
                printf("\nrnr-disable only allowed on local-ack remote peers");
		return;
	    }
	    rnr_disable = TRUE;
	}

	/* tcp-receive-window */
	if (GETOBJ(int,15))
	    tcprcvwnd = GETOBJ(int,15);

	if ((priority || local_ack) && backup_group) {
	    printf("\n%%Backup groups cannot be used with local-ack or prioritization");
	    return;
	}

	if (!csb->sense) {
	    destroy_vrpeer(vrn, type, cookie, macaddr);
	}else{
	    reg_invoke_ip_tcpdriver_start();
	    create_vrpeer(vrn, type, cookie, macaddr, largest, local_ack, rnr_disable,
			  backup_group, priority, force_version,
			  netbios_host_list, netbios_bytes_list,
			  tcprcvwnd);
	}
	break;
      case SRB_REMPR_KEEP:
	if (csb->set_to_default)
            rsrb_keepalive_interval = RSRB_KEEPALIVE_DEFAULT_INTERVAL;
	else if (!csb->sense) 
            rsrb_keepalive_interval = 0;
        else {
            rsrb_keepalive_interval = GETOBJ(int,1) * ONESEC;
            for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next)
                for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next)
                    GET_TIMESTAMP(vrp->last_keepalive);
        }
        break;
      case SRB_CONTMO:
	if (csb->sense) {
	    vrconn_timeout = GETOBJ(int,1) * ONESEC;
	} else {
	    vrconn_timeout = RSRB_REMOPEN_DEFTMO;
	}
	break;
      case SRB_TRANSPARENT:

        /* GETOBJ(int,1) = target ring number
         * GETOBJ(int,2) = pseudo-ring number (if valid ring number)
         * GETOBJ(int,2) = fastswitch         (if equal to -1)
         * GETOBJ(int,3) = bridge number
         * GETOBJ(int,4) = bridge group
         * GETOBJ(int,5) = oui_type
         * GETOBJ(int,6) = netbios_cache
         */

	if (!(vrg = vrfind_ringgroup(GETOBJ(int,1)))) {
	    printf("\n%%ring-group %d not found", GETOBJ(int,1));
	    break;
	}

	/* Enable/disable fastswitched srtlb & explorers. */

        if(GETOBJ(int,2) == -1) {
	    if(!vrg->virtual_bridge) {
		printf("\n%%The source-route transparent %d not configured",
				                             GETOBJ(int,1));
		break;
		}

            tmpidb = vrg->virtual_bridge->hwptr;
            if(csb->sense) {
		tmpidb->span_bridge_process_force = FALSE;
		tmpidb->fs_expl_func = fs_srtlb_explorers;
	    }
            else{
		tmpidb->span_bridge_process_force = SPAN_BR_PROC_VIRTUAL_IDB;
		tmpidb->fs_expl_func = NULL;
	}
            break;
            }
 
        /* If this is the [no] form remove the virtual IDB
         * from the ring group and from the bridge group.  */
 
	if (!csb->sense) {
            rsrb_delete_virtual_bridge(vrg);
	    break;
	}

	FOR_ALL_HWIDBS(tmpidb)
	    if (srb_sb_get_targetring(tmpidb->firstsw) == GETOBJ(int,1)
            && !tmpidb->tr_transit_oui) {
                printf("\n%%The source-route transparent command cannot be ");
                printf("configured until all\ntoken rings in the specified ");
                printf("ring-group have an ethernet-transit-oui\nspecified.");
		return;
	        }

	/* If we are already bridged to another group, say so */

	if(vrg->virtual_bridge) {
	    printf("\n%%ring-group %d already bridged with bridge-group %d",
		   GETOBJ(int,1), vrg->virtual_bridge->span_index);
	    break;
	}

        if (rsrb_find_virtual_bridge(GETOBJ(int,4))) {
	    printf("\n%%Bridge group %d already bridged to a virtual ring",
                                                            GETOBJ(int,4));
	    break;
	}

	switch (GETOBJ(int,5)) {
	  case SRB_TRANSPARENT_90COMPAT:
	    transit_oui = ETHEROUI;
	    break;
	  case SRB_TRANSPARENT_STANDARD:
	    transit_oui = OLD_ETHEROUI;
	    break;
	  case SRB_TRANSPARENT_CISCO:
	    transit_oui = CISCOOUI;
	    break;
	  default:
                transit_oui = DEFAULT_OUI;
	    break;
	}

        if (!rsrb_create_virtual_bridge(vrg, GETOBJ(int,3), GETOBJ(int,2),
                          GETOBJ(int,4), GETOBJ(int,6), transit_oui)) {
	    printf(nomemory);
            break;
	}

        /* Make sure all the data structures get updated */
  
	set_srb_interfaces();
	set_vring_interfaces();
	fs_srb_setup();
	start_srb_background();
	break;
      case SRB_ENABLE_80D5:
	enable_80d5 = csb->sense;
	break;
      case SRB_SAP_80D5:
	if (csb->set_to_default)
	    sap_8209_table[GETOBJ(int,1)] = default_sap_8209_table[GETOBJ(int,1)];
	else
	    sap_8209_table[GETOBJ(int,1)] = csb->sense;
	break;
      case SRB_FST_PEERNAME:
	if (csb->sense) {
	    tudp_name = GETOBJ(paddr,1)->ip_addr;
	} else {
	    tudp_name = (ipaddrtype) 0;
	}
	if (tudp_name == (ipaddrtype) 0) {
	    rsrb_shutdownallfst();
	}
	rsrb_fst_name = (ipaddrtype) tudp_name;
	break;
      case SRB_QLLC_LAK:
      	srb_media_conv_local_ack(csb->sense, &lnx_lak_enable, RSRB_ET_QLLC);
	break;
      case SRB_SDLLC_LAK:
      	srb_media_conv_local_ack(csb->sense, &sdllc_lak_enable, RSRB_ET_SDLLC);
	break;
      case SRB_COS:
	rsrb_cos_enable = csb->sense;
	break;
      case SRB_PROXY_NETBIOS:
	srb_proxy_netbios_only = csb->sense;
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * srb_media_conv_local_ack
 *
 * Turn local ack on or off for media conversion products SDLLC, QLLC
 */
 
void srb_media_conv_local_ack(boolean sense, boolean *enable_flag, int vre_type)
{
    tr_vgrouptype	*vrg;
    tr_ventrytype	*vre;
    tr_vpeertype 	*vrp;
    boolean		match = FALSE;
    boolean 		do_update = FALSE;

    if (sense) {
	for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
	    for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
		if (vrp->local_ack) {
                     match = TRUE;
                     break;
                  }
               }
           }
           if (!match) {
                printf("\nConfigure source-bridge remote-peer command with local-ack");
                printf("\n  option before issuing this command.  Command ignored");
                return;
           }
    }
    
    if (*enable_flag != sense) {
	/*
	 * If any local rings are defined as sdllc/qllc rings, the "peers" need
	 * to know them as remote rings instead of sdllc/qllc rings for local
	 * ack to function properly.
	 *
	 * We also need to shut down the local LLC sessions and tell the
	 * remote end before we throw the switch.
	 */
	for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
	    for (vre = (tr_ventrytype *)vrg->rings.qhead; vre; vre = vre->next) {
		if (vre->type == vre_type) {
                    do_update = TRUE;
                    vrg->flags |= VRGF_GENUPDATE;
                    break;
                }
            }
        }
        if (do_update)
            update_peers();
	}
   *enable_flag = sense;
}

/*
 * srb_flush_explorer - Prevent explorer storms from overwhelming the system.
 *
 * Default max fast explorer rate in bytes per tenth of a second.
 * The valus is roughly based on the max bandwidth of interface.
 *
 * If the byte count reaches 'data_maxrate' bytes within 1/10th second,
 * drop the incoming explorer packets from the interface for the duration
 * of the interval.
 */
boolean srb_flush_explorer (
hwidbtype *idb,
ushort dgsize)
{
    if(!srb_running)
	return(TRUE);

    if (idb->fs_expl_input) {
        if(AWAKE(idb->fs_expl_input->data_timer)) {
            idb->fs_expl_input->data_rate = 0;
	    TIMER_START(idb->fs_expl_input->data_timer, ONESEC/10);
	    }
	else if (idb->fs_expl_input->data_rate >= fs_explorer.data_maxrate) {
	    fs_explorer.flush++;
	    return(TRUE);
	    }
	}

    return(FALSE);
}


void start_srb_background (void)
{
    if (srb_pid == NO_PROCESS) {
	srb_pid = process_create(srb_background, "SRB Background", LARGE_STACK,
				 PRIO_HIGH);
    }
}
static void srb_proxy_if_disable (hwidbtype *idb) 
{
    srb_sb_disable_proxy_explorer(idb->firstsw);
    netbios_name_if_disable(idb);
    FOR_ALL_HWIDBS(idb) {
	if (!is_srb_hw(idb)) {
	    continue;
	}
	if (srb_sb_get_proxy_explorer(idb->firstsw)) {
	    return;
	}
    }
    global_proxy_explorer_enable = FALSE;
}
void srbif_command (parseinfo *csb)
{
    ushort local_ring, target_ring, bridge_num;
    idbtype *swidb;
    hwidbtype *tmpidb, *idb;
    spantype *span;		/* SRB AST support */
    srb_sb_t *srb_sb;

    swidb = csb->interface;
    idb = swidb->hwptr;
    if (!is_srb_hw(idb)) {
	if (!csb->nvgen) {
	    printf("\n%%source-bridge only allowed on Token Rings and FDDI");
	}
	return;
    }

    srb_sb = srb_get_or_create_sb_inline(swidb);
    if (!srb_sb) {
        printf(nomemory);
        return;
    }

    if (csb->nvgen) {
	if (srb_sb->srb_bridge_num > SRB_MAX_BRIDGE_NUM) {
	    return;
	}
	switch (csb->which) {
	  case SRB_INADDRESS:
	    nv_write(idb->srb_address_in, "%s %d", csb->nv_command,
		     idb->srb_address_in);
	    break;
	  case SRB_INLSAP:
	    nv_write(idb->srb_lsap_in, "%s %d", csb->nv_command,
		     idb->srb_lsap_in);
	    break;
	  case SRB_INTYPE:
	    nv_write(idb->srb_type_in, "%s %d", csb->nv_command,
		     idb->srb_type_in);
	    break;
          case SRB_MAXHOPS:
            nv_write(srb_sb->srb_max_hops != SRB_DEF_MAXHOPS, "%s %d",
                     csb->nv_command, srb_sb->srb_max_hops);
            break;
	  case SRB_MAXOHOPS:
	    nv_write(srb_sb->srb_maxout_hops != SRB_DEF_MAXHOPS, "%s %d",
		     csb->nv_command, srb_sb->srb_maxout_hops);
	    break;
          case SRB_MAXIHOPS:
            nv_write(srb_sb->srb_maxin_hops != SRB_DEF_MAXHOPS, "%s %d",
                     csb->nv_command, srb_sb->srb_maxin_hops);
            break;
	  case SRB_MAXRD:
	    /* No NV generation */
	    break;
	  case SRB_OUTADDRESS:
	    nv_write(idb->srb_address_out, "%s %d", csb->nv_command,
		     idb->srb_address_out);
	    break;
	  case SRB_OUTLSAP:
	    nv_write(idb->srb_lsap_out, "%s %d", csb->nv_command,
		     idb->srb_lsap_out);
	    break;
	  case SRB_OUTTYPE:
	    nv_write(idb->srb_type_out, "%s %d", csb->nv_command,
		     idb->srb_type_out);
	    break;
	  case SRB_SPAN:
	    if (!((((idb->status & IDB_TR) && !swidb->span_ptr) ||
		   ((idb->status & IDB_FDDI) && !swidb->span_ptr)) ||
		  (((spantype *)swidb->span_ptr)->protocol == 
		   SPAN_PROTO_IBM))) {
		/* 
		 * This TR interface is not configured with either static ST or
		 * AST.
		 */
		/* Add check for the valid but unusual case where
		 * both source-bridge and bridge-group are configured
		 * on the same interface and ensure that spanning tree
		 * explorer forwarding status is set appropriately.
		 */
		if (idb->firstsw->span_index) {
		    nv_write(srb_sb->srb_spanning_explorer, csb->nv_command);
		}
		break;
	    } 
	    if (!idb->firstsw->span_index) {
		/* static ST running */
		nv_write(srb_sb->srb_spanning_explorer, csb->nv_command);
	    } else {
		nv_write(TRUE, "%s %d", csb->nv_command,
			 idb->firstsw->span_index);
		if (idb->firstsw->path_cost_explicit)
		    nv_write(TRUE, "%s %d path-cost %d", 
			     csb->nv_command,
			     idb->firstsw->span_index,
			     idb->firstsw->path_cost);
		nv_write(idb->firstsw->ibm_port_priority !=
			 SPAN_IBM_PORT_PRIORITY, "%s %d priority %d",
			 csb->nv_command,
			 idb->firstsw->span_index,
			 idb->firstsw->ibm_port_priority);
		/*
                 * Added to support hidden message-age-increment option in
                 * "source-bridge spanning X" interface command and permit
                 * message-age count manipulation in mixed AST environments
                 * where bridge vendors do not support MAX_AGE config.
		 */
		nv_write(idb->firstsw->ibm_message_age_increment !=
			 MESSAGE_AGE_INCREMENT,
			 "%s %d message-age-increment %d",
			 csb->nv_command,
			 idb->firstsw->span_index,
			 idb->firstsw->ibm_message_age_increment);
	    }
	    break;
	  case SRB_PROXY:
	    nv_write(srb_sb->srb_proxy_explorer, csb->nv_command);
	    break;
	  case SRB_OLDSNA:
	    nv_write(srb_sb->srb_oldsna_hack, csb->nv_command);
	    break;
	  case SRB_ROUTEC:
	    if (idb->srb_routecache & SRB_CBUS_ROUTECACHE) {
		nv_write(TRUE, "%s cbus", csb->nv_command);
	    } else if (idb->srb_routecache & SRB_SSE_ROUTECACHE) {
		nv_write(TRUE, "%s sse", csb->nv_command);
	    } else {
		if (!idb->srb_routecache) {
		    nv_write(TRUE, "no %s", csb->nv_command);
		}
	    }
	    break;
	  case SRB_ACTIVE:
            /* Do not continue if it is part of a PCbus virtual bridge */
	    if (reg_invoke_local_lnm_vr_bridge (idb))
		break;

	    nv_write(!(idb->status & IDB_CHANNEL),
		     "source-bridge %d %d %d",
                     srb_sb->srb_thisring,
                     srb_sb->srb_bridge_num,
                     srb_sb->srb_targetring);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case SRB_INADDRESS:
         if (idb->access_expression_in) {
               printf("\nCannot configure source-bridge input list with access-expression");
               return;
         }
	if (csb->sense) {
            if ((GETOBJ(int,1) == 0) || (GETOBJ(int,1) >= MINMACADDRACC))
	        idb->srb_address_in = GETOBJ(int,1);
            else {
                printf("\nAccess number invalid, restricted to %d .. %d",
                       MINMACADDRACC, MAXMACADDRACC);
                return;
	    }
	} else {
	    idb->srb_address_in = 0;
	}
	tr_set_access_flags(idb);
	break;
      case SRB_INLSAP:
         if (idb->access_expression_in) {
               printf("\nCannot configure source-bridge input list with access-expression");
               return;
         }
	srb_list(GETOBJ(int,1), idb, &idb->srb_lsap_in);
	break;
      case SRB_INTYPE:
         if (idb->access_expression_in) {
               printf("\nCannot configure source-bridge input list with access-expression");
               return;
         }
	srb_list(GETOBJ(int,1), idb,  &idb->srb_type_in);
	break;
      case SRB_MAXRD:
	printf("\n%%The 'max-rd' command has been superceded by the"
	       " 'max-hops' command."); 
        printf("\nIn the future, please use the new 'max-hops' command.");
	if (csb->sense) {
	    srb_sb->srb_max_hops = GETOBJ(int,1) - 1;
	} else {
	    srb_sb->srb_max_hops = SRB_DEF_MAXHOPS;
	}
	break;
      case SRB_MAXHOPS:
	if (csb->sense) {
	    srb_sb->srb_max_hops = GETOBJ(int,1);
	} else {
	    srb_sb->srb_max_hops = SRB_DEF_MAXHOPS;
	}
	break;
    case SRB_MAXOHOPS:
        if (csb->sense) {
            srb_sb->srb_maxout_hops = GETOBJ(int,1);
        } else {
            srb_sb->srb_maxout_hops = SRB_DEF_MAXHOPS;
        }
        break;
    case SRB_MAXIHOPS:
        if (csb->sense) {
            srb_sb->srb_maxin_hops = GETOBJ(int,1);
        } else {
            srb_sb->srb_maxin_hops = SRB_DEF_MAXHOPS;
        }
        break;
      case SRB_OUTADDRESS:
         if (idb->access_expression_out) {
               printf("\nCannot configure source-bridge input list with access-expression");
               return;
         }
	if (csb->sense) {
            if ((GETOBJ(int,1) == 0) || (GETOBJ(int,1) >= MINMACADDRACC))
	        idb->srb_address_out = GETOBJ(int,1);
            else {
                printf("\nAccess number invalid, restricted to %d .. %d",
                       MINMACADDRACC, MAXMACADDRACC);
                return;
	    }
	} else {
	    idb->srb_address_out = 0;
	}
	tr_set_access_flags(idb);
	break;
      case SRB_OUTLSAP:
         if (idb->access_expression_out) {
               printf("\nCannot configure source-bridge input list with access-expression");
               return;
         }
	srb_list(GETOBJ(int,1), idb, &idb->srb_lsap_out);
	break;
      case SRB_OUTTYPE:
         if (idb->access_expression_out) {
               printf("\nCannot configure source-bridge input list with access-expression");
               return;
         }
	srb_list(GETOBJ(int,1), idb, &idb->srb_type_out);
	break;
      case SRB_SPAN:
	/* SRB AST Support */
	srb_span_command(csb, idb);
	break;
      case SRB_PROXY:
	if (csb->sense) {
	    srb_sb->srb_proxy_explorer = TRUE;
	    global_proxy_explorer_enable = TRUE;
	    break;
	} else {
	    netbios_name_if_disable(idb);
	    srb_proxy_if_disable(idb);
	}
	break;
      case SRB_OLDSNA:
	srb_sb->srb_oldsna_hack = csb->sense;
	break;
      case SRB_ROUTEC:
	if ((!csb->sense) && !srb_on_idb(idb->firstsw)) {
	    printf("\n%%must enable SRB first");
	    break;
	}	    
	if (GETOBJ(int,1) == SRB_ROUTEC_CBUS) {
	    if (idb->status & IDB_CBUS) {
		if (csb->sense) {
		    idb->srb_routecache = (SRB_FAST_ROUTECACHE |
				           SRB_CBUS_ROUTECACHE);
		    delete_fair_queue(idb);
		} else {
		    idb->srb_routecache &= ~SRB_CBUS_ROUTECACHE;
		}
	    } else {
		printf("%% %s not cBus interface", swidb->namestring);
		break;
	    }
	} else if (GETOBJ(int,1) == SRB_ROUTEC_SSE) {
	    if (idb->status & IDB_CBUS) {
		if (!reg_invoke_sse_present()) {
		    printf("%% %s not an SSE interface", swidb->namestring);
		    break;
		}
		if (csb->sense) {
		    if (idb->status & IDB_TR) {
			idb->srb_routecache = (SRB_FAST_ROUTECACHE |
					       SRB_SSE_ROUTECACHE);
			delete_fair_queue(idb);
		    }
		    else {
			printf("%% %s does not support SSE-switching "
			       "for source-route bridging",
			       swidb->namestring);
			break;
		    }
		} else {
		    idb->srb_routecache &= ~SRB_SSE_ROUTECACHE;
		}
	    } else {
		printf("%% %s not cBus interface", swidb->namestring);
		break;
	    }
	} else {
	    idb->srb_routecache = csb->set_to_default || csb->sense;
	}
	reg_invoke_srt_setup_auto_sw_srb(idb);
        set_vring_fast_entry(idb);
        fs_srb_setup();
        break;
      case SRB_ACTIVE:
	if (!csb->sense) {
	    /*
	     * no source-bridge
	     */
	    netbios_name_if_disable(idb);
	    srb_proxy_if_disable(idb);
	    target_ring = srb_sb->srb_targetring; /* remember vring */
            srb_sb->srb_thisring   = SRB_INVALID_RING_NUM;
            srb_sb->srb_bridge_num = SRB_INVALID_BRIDGE_NUM;
            srb_sb->srb_targetring = SRB_INVALID_RING_NUM;
	    srb_sb->srb_max_hops    = SRB_DEF_MAXHOPS;
            srb_sb->srb_maxout_hops = SRB_DEF_MAXHOPS;
            srb_sb->srb_maxin_hops  = SRB_DEF_MAXHOPS;
	    srb_sb->srb_spanning_explorer = FALSE;
            idb->mac_use_exp_buf = FALSE;
	    set_srb_interfaces();
	    remove_vring_interface(idb, target_ring);
	    set_vring_interfaces();
	    fs_srb_setup();
            fs_explorer_setup();
	    reg_invoke_srt_setup_auto_sw_srb(idb);
	    reg_invoke_set_ibmnm_interfaces(idb, FALSE);
	    /* SRB AST support */
	    span = (spantype *)idb->firstsw->span_ptr;
	    if (span) {
	        /* 
		 * disable spanning tree on this interface 
		 * In this case, it doesn't matter whether we are running
		 * IEEE or IBM spanning tree, because bridging is terminated.
		 */
	        if (span->running)
		    span_port_disable(span, idb->firstsw);
		span_delete_port(idb->firstsw);
		swidb->span_index = 0;
	    }
	    else {
                srb_sb->srb_enabled_for_fddi = FALSE;
	    }
	    if (idb->state != IDBS_ADMINDOWN) {
                reg_invoke_local_lnm_reset_vr_bridge(idb);
                srbcore_setup_srb(idb);
            }
	    reg_invoke_srt_set_srb_params(idb, csb->sense);
	    break;
	}
	local_ring = GETOBJ(int,1);
	bridge_num = GETOBJ(int,2);
	target_ring = GETOBJ(int,3);
 	/*
 	 * See if this bridge (rn-br-rn) exists already. 
         * Do not permit the duplicate bridge command.
 	 */
 	FOR_ALL_HWIDBS(tmpidb) {
            srb_triplet_t srb_triplet;
            srb_sb_get_triplet(tmpidb->firstsw, &srb_triplet);
 	    if (is_srb_hw(tmpidb) && (idb != tmpidb)) {
 	        if (srb_triplet.localRing  == local_ring &&
 		    srb_triplet.bridgeNum  == bridge_num &&
 		    srb_triplet.remoteRing == target_ring) {
                       printf("\nDuplicate bridge command is not permitted(%s)."
,
                              tmpidb->hw_short_namestring);
                       return;  /* exit loop.  Duplicate bridge commands */
                                /* are not allowed on a router.          */
                }
	    }
 	}
	if (target_ring == local_ring) {
	    printf("\n%%local ring and target ring numbers must be"
		   " different!"); 
            return;
	}
	if (vrfind_ringgroup(local_ring)) {
	    printf("\n%%local ring can not be a ring group");
	    return;
	}
	if ((rsrb_exists_translational(srb_sb->srb_targetring) != NULL) &&
	    !(idb->tr_transit_oui)) {
	    printf("\n%%Ring-group %d is enabled for SR/TLB. You must"
		   " specify an OUI with the"); 
	    printf("\n\"ethernet-transit-oui\" command before enabling"
		   " interfaces for SRB"); 
	    printf("\n\"when translational bridging is used (This interface"
		   " is %s)", 
		   swidb->namestring);
	    return;
	}
	
	remove_vring_interface(idb, srb_sb->srb_targetring);
	srb_sb->srb_thisring   = local_ring;
	srb_sb->srb_bridge_num = bridge_num;
	srb_sb->srb_targetring = target_ring;
	set_srb_interfaces();
	set_vring_interfaces();
	fs_srb_setup();
        fs_explorer_setup();
	reg_invoke_srt_setup_auto_sw_srb(idb);
	reg_invoke_set_ibmnm_interfaces(idb, TRUE);
	start_srb_background();
	/* SRB AST support */
	span = (spantype *)idb->firstsw->span_ptr;
	if (span) {
	    if (span->protocol == SPAN_PROTO_IBM) {
 		/* stupid ibm variation */
	        idb->firstsw->port_id = 
		    ((local_ring << 4) & 0xfff0) | bridge_num; 
	    }
	    if (span->running)
	        span_port_enable(span, idb->firstsw); 
   	}
	if (idb->status & IDB_FDDI)
	    srb_sb->srb_enabled_for_fddi = TRUE;
	if (idb->state != IDBS_ADMINDOWN) {
            srbcore_setup_srb(idb);
        }
	reg_invoke_srt_set_srb_params(idb, csb->sense);
	idb->mac_use_exp_buf = FALSE;
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
    reg_invoke_srb_setup();
}


/*
 * srb_list
 * Common code to set up a type/LSAP list
 */

void srb_list (int value, hwidbtype *idb, int *list)
{
    if ((value == 0) || (value >= MINTYPEACC)) {
	*list = value;
        tr_set_access_flags(idb);
    }
    else 
	printf("\n%%Access list number invalid; range is %d .. %d",
	       MINTYPEACC, MAXTYPEACC);
}


/*
 * set_srb_interfaces: set any local bridge interface idbs
 *
 * This routine scans through all the srb interfaces setting the idb
 * variable tr_bridge_idb for any locally 2 port bridged interfaces.
 * For a given srb interface tr_bridge_idb points to the idb for
 * the output of this bridged ring.
 */

void set_srb_interfaces (void)
{
    hwidbtype *idb, **srb_idb;
    int i, j, ttl_srb_ports, max_srb_ports;
    
    max_srb_ports = get_max_srb_ports();

    srb_idb = malloc(max_srb_ports * sizeof(*srb_idb));

    if (!srb_idb) {
	printf(nomemory);
	return;
    }

    ttl_srb_ports = 0;

    FOR_ALL_HWIDBS(idb) {
	if (!is_srb_hw(idb))
	   continue;
	idb->tr_bridge_idb = NULL;
	if (ttl_srb_ports >= max_srb_ports)
	   break;                                    /* a bug */
	if (srb_sb_get_bridging_enabled(idb->firstsw)) {
	    srb_idb[ttl_srb_ports++] = idb;
	}
    }

    for (j = 0; j < ttl_srb_ports; j++) {
        srb_triplet_t srb_triplet_j;

	idb = srb_idb[j];
        srb_sb_get_triplet(idb->firstsw, &srb_triplet_j);
	for (i = 0; i < ttl_srb_ports; i++) {
            srb_triplet_t srb_triplet_i;
            srb_sb_get_triplet(srb_idb[i]->firstsw, &srb_triplet_i);
	    if ((srb_triplet_j.remoteRing == srb_triplet_i.localRing) &&
	        (srb_triplet_j.bridgeNum  == srb_triplet_i.bridgeNum)) {
	        idb->tr_bridge_idb = srb_idb[i];
	        idb->tr_vring_blk = NULL;
	        break;
	    }
        }
    }

    free(srb_idb);
    return;
}

/*
 * fs_explorer_setup
 *
 * Allocate input header for all idb's.
 * Clear all counters in the control structure.
 *
 * If "fs_explorer.enabled" is true:
 *   1. Check all idb's and setup vectors for those configured for srb.
 *   2. Allocate explorer fast-switch control block and working buffer.
 *
 * If "fs_explorer.enabled" is false;
 *
 *   1. Clear all explorer fast-switch vectors.
 *   2. Free fast-switch control block and working buffer.
 */

void fs_explorer_setup ()
{
    hwidbtype *idb;
    char *tptr;

    FOR_ALL_HWIDBS(idb) {

        /* Allocate explorer input header for all interface idb's. */

        if(!idb->fs_expl_input) {
            tptr = malloc_named(sizeof(fs_expl_que), "IDB: Explorers");
            if (tptr) {
	        ((fs_expl_que *)tptr)->idb = idb;
	        idb->fs_expl_input = (fs_expl_que *)tptr;
	        }
	    }

	if (!is_srb_hw(idb))
	    continue;

        /* If the default max rate is zero set it to the interface
         * specific default.  Just use the first idb since the low 
         * end and high end interfaces are never in the same box. */

        if(!fs_explorer.data_maxrate_dflt) {
            if(idb->type == IDBTYPE_TMS380) 
                fs_explorer.data_maxrate_dflt = DEFAULT_FS_MAXRATE_LES;
            else
                fs_explorer.data_maxrate_dflt = DEFAULT_FS_MAXRATE_HES;

            fs_explorer.data_maxrate = 
                        fs_explorer.data_maxrate_dflt / MAXRATE_PERIOD;
            }

        if (fs_explorer.enabled) {

	    /* If this interface is configured for srb then
	     * set interface specific explorer fastswitch vector. */

	    if (srb_sb_get_bridging_enabled(idb->firstsw)) {

                switch (idb->type) {
                    case IDBTYPE_CTR:
                    case IDBTYPE_FDDI:
                    case IDBTYPE_FDDIT:
		    case IDBTYPE_CIP_LAN_ADAPTER:
                        reg_invoke_mci_expl_vectors(idb);
                        break;

                    case IDBTYPE_TMS380:
		    case IDBTYPE_IBM2692: 
                        reg_invoke_les_expl_vectors(idb);
                        break;

                    default:
                        idb->fs_expl_func = (fs_explorer_t)return_false;
                        break;
                    }
		}
	    else{ /* Interface not configured for srb. */ 

                idb->fs_expl_func = (fs_explorer_t)return_false;
		continue;
		}

            /* Allocate global fast-switch control block and buffer. */

            if(!fs_explorer.hcb) {
    	        fs_explorer.hcb = malloc(sizeof(fs_hdrtype));

		if(!fs_explorer.hcb) {
		    fs_explorer.enabled = FALSE;
		    return;
		    }
		}

            /* Allocate working input packet.  Use the first idb to 
	     * get it from the pool then decrement idb's buffer count. */

            if(!fs_explorer.newpak) {
                fs_explorer.newpak = getbuffer(EXPLORER_BUF_SIZE);

		if(!fs_explorer.newpak) {
		    fs_explorer.enabled = FALSE;
		    return;
		    }

	        fs_explorer.newpak->datagramstart = 
		          fs_explorer.newpak->data_area + ENCAPBYTES;
	        }
	    }
        else{ /* Enable is FALSE.  Stub all explorer fast-switch vectors. */

            idb->fs_expl_func = (fs_explorer_t)return_false;

	    /* Free global packet fast-switch control block and buffer. */

	    if(fs_explorer.hcb) {
                tptr = (char *)fs_explorer.hcb;
	        fs_explorer.hcb = NULL;
	        free(tptr);
	        }

            /* Return working input packet to the pool. */

            if(fs_explorer.newpak) {
	        tptr = (char *)fs_explorer.newpak;
                fs_explorer.newpak = NULL;
	        retbuffer((paktype *)tptr);
	        }

	    }

	/* Always clear the counters. */

        fs_clear_counts(idb);
        }
}

/*
 * find_ringidb
 * return the idb for a given local ring if found.  NULL otherwise.
 */

hwidbtype *find_ringidb (ushort rn)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb)
	if (is_srb_hw(idb) &&  
	    (srb_sb_get_thisring(idb->firstsw) == rn))
	    return(idb);

    return(NULL);
}


static const char show_srb_idb_header[] = "\n"
"                                          v p s n r               "
"     Packets\n"
"Interface  St  MAC-Address    srn bn  trn r x p b c IP-Address    "
"     In   Out\n";

/*
 * show_srb_idb
 * Display link level statistics for one network interface
 */

static void show_srb_idb (idbtype *idb)
{
    hwidbtype *hwidb = idb->hwptr;
    srb_sb_t  *srb_sb = srb_get_sb_inline(idb);
    idb_si_counters_t	*sic;

    if (!srb_sb) {
        return;
    }

    printf( "\n%11s%2s",
            idb->short_namestring,
            interface_up(idb) ? "up" : "dn");
    if (hwidb->status & IDB_IEEEMASK) {
        printf(" %14e", hwidb->hardware);
    } else
	printf(" %14s", "");
    if (srb_sb->srb_thisring) {
	printf("%-5d%-3d%-5d %c %c %c %c",
	       srb_sb->srb_thisring, srb_sb->srb_bridge_num,
	       srb_sb->srb_targetring, (hwidb->tr_vring_blk ? '*' : ' '),
               (srb_sb->srb_proxy_explorer ? '*' : ' '),
               (srb_sb->srb_spanning_explorer ? 'f' : 'b'),
               (netbios_sb_name_caching_is_enabled(hwidb->firstsw) ? '*' : ' '));
	if (hwidb->srb_routecache & SRB_CBUS_ROUTECACHE) {
	    printf(" C");
	} else if (hwidb->srb_routecache & SRB_SSE_ROUTECACHE) {
	    printf(" S");
	} else if (hwidb->srb_routecache & SRB_FAST_ROUTECACHE) {
	    printf(" F");
	} else printf("  ");
    } else {
        printf("%23s", "");
    }
    if (idb->ip_address)
        printf(" %15i", idb->ip_address);
    else
	if (idb->ip_unnumbered)
	    printf(" unnumbered     ");
	else printf("%16s", "");
    
    if (!is_subinterface(idb)) {
        sic = idb_get_counter_info(hwidb, IDB_COUNTER_SI_CNT_PTR);
        printf(" %-5lu", (idb_inputs(hwidb) - sic->inputs));
        printf(" %-5lu", (idb_outputs(hwidb) - sic->outputs));
    }
}


/*
 * show_srb
 * display info about source route bridging.
 * (OBJ(int,1) == TRUE)   The 'interfaces' keyword was parsed.
 */

void show_srb (parseinfo *csb)
{
    boolean first;
    hwidbtype *idb;

    if (GETOBJ(int, 1)) {
        automore_enable(show_srb_idb_header);
        show_swidb_chain(NULL, show_srb_idb, 1);
	automore_disable();
	return;
    } 
    first = TRUE;
    automore_enable(NULL);
    FOR_ALL_HWIDBS(idb) {
        srb_sb_t *srb_sb = srb_get_sb_inline(idb->firstsw);
	if (!is_srb_hw(idb) || !srb_sb) {
            continue;
        }
	if (srb_sb->srb_bridge_num > SRB_MAX_BRIDGE_NUM) {
            continue;
        }
	if (first) {
	    first = FALSE;
	    printf("\n\nLocal Interfaces:                           receive     transmit");
	    printf("\n             srn bn  trn r p s n  max hops     cnt         cnt        drops");
	}
        printf("\n%12s%-4d %-2d %-4d %c %c %c %c %-2d %-2d %-2d %-8u    %-8u      %-5u",
	       idb->hw_short_namestring,
               srb_sb->srb_thisring, srb_sb->srb_bridge_num,
	       srb_sb->srb_targetring, (idb->tr_vring_blk ? '*' : ' '),
	       (srb_sb->srb_proxy_explorer ? ' ' : '*'),
	       (srb_sb->srb_spanning_explorer ? 'f' : 'b'),
	       (netbios_sb_name_caching_is_enabled(idb->firstsw) ? '*' : ' '),
	       srb_sb->srb_max_hops,
               srb_sb->srb_maxout_hops,
               srb_sb->srb_maxin_hops,
	       srb_sb->srb_rx_count + 
                 srb_inputs_fast(idb, srb_sb) + srb_inputs_auto(idb, srb_sb),
	       srb_sb->srb_tx_nb_count + srb_sb->srb_tx_b_count +
                 srb_outputs_fast(idb, srb_sb) + srb_outputs_auto(idb, srb_sb),
	       srb_sb->srb_tx_drops + srb_sb->srb_rx_drops);
    }

    show_rsrb();
    
    first = TRUE;
    FOR_ALL_HWIDBS(idb) {
        srb_sb_t *srb_sb = srb_get_sb_inline(idb->firstsw);
	if (!is_srb_hw(idb) || !srb_sb ||
	    (srb_sb->srb_bridge_num > SRB_MAX_BRIDGE_NUM))
	    continue;
	if (first) {
	    first = FALSE;
	    printf("\n\nExplorers: ------- input -------             ------- output -------");
	    printf("\n         spanning  all-rings     total      spanning  all-rings     total");
	}
	printf("\n%7s  %-8lu   %-8lu  %-8lu      %-8lu   %-8lu  %-8lu",
               idb->hw_short_namestring,
	       srb_sb->srb_span_in, srb_sb->srb_allrte_in,
	       srb_sb->srb_span_in + srb_sb->srb_allrte_in,
	       srb_sb->srb_span_out, srb_sb->srb_allrte_out,
	       srb_sb->srb_span_out + srb_sb->srb_allrte_out);
    }


    first = TRUE;
    FOR_ALL_HWIDBS(idb) {
        srb_sb_t *srb_sb = srb_get_sb_inline(idb->firstsw);
        if(!is_srb_hw(idb) || !srb_sb || 
              (srb_sb->srb_bridge_num > SRB_MAX_BRIDGE_NUM)) {
            continue;
        }

        if(first) {
            first = FALSE;
            printf("\n\n  Local: %s %8d  flushed %8d  max Bps %8d", 
                    fs_explorer.enabled ?
                                 "fastswitched" : "process switched",
	            fs_explorer.total, fs_explorer.flush, 
                    fs_explorer.data_maxrate * MAXRATE_PERIOD);

            printf("\n\n         rings      inputs         bursts         output drops");
            }

        printf("\n       %-7s    %-8d       %-8d          %-8d",
                idb->hw_short_namestring, 
                (idb->fs_expl_input ? idb->fs_expl_input->count : 0),
                (idb->fs_expl_input ? idb->fs_expl_input->storms : 0),
		(idb->fs_expl_input ? idb->fs_expl_input->out_drops : 0));
        }

    printf("\n");
    automore_disable();
}

/*
 * srb_proxy
 * when we get a bridged packet that is an XID or TEST we can do a proxy on
 * it.  This is only done on explorers or if the RIF is 02.  Only XID and
 * TEST packets are done because these are the only ones that we know
 * how to reverse easily.  Others would require protocol decode.
 *
 * This code assumes that it is only called by bridging code.  Proxies are
 * *not* done for the following conditions:
 *
 *       1) no RIF.  (rejected by bridge code or its for us (multi or direct)
 *       2) directed at us.  (for us, handled by local code)
 *       3) broadcast or multicast. (interface also rejects xid/test)
 *
 * In the bridge section from which this code is called the above conditions
 * are always valid.
 *
 * NOTE: IF YOU CHANGE THE LOGIC OF THIS CODE, MAKE SURE THAT IT DOESN'T BREAK
 *       netbios_name_cache_proxy IN netbios.c! 
 */
static boolean srb_proxy (
    paktype *pak,
    char *input_llc_type,
    boolean *did_reply,
    boolean non_null_dsap_ok)
{
    trrif_t *trh;
    int orl;
    snap_hdr *llc1;
    char *llc_type;
    riftype *cacheptr;
    paktype *newpak;
    leveltype level;
    hwidbtype *hwidb = hwidb_or_null(pak->if_input);
    uchar temp_saddr[IEEEBYTES];

    if (did_reply != NULL)
      *did_reply = FALSE;

    trh = (trrif_t *) pak->datagramstart;
    if (trh->saddr[0] & TR_RII)
	orl = trh->rc_blth & RC_LTHMASK;
    else {
	retbuffer(pak);			/* no rif, you're dead */
	return(TRUE);
    }
    if (trh->daddr[0] & TR_GROUP) {
        /* Can not proxy it */
        return(FALSE);
    }
    if ((trh->rc_blth != RC_NORD_LTH) && /* no kludge and not explorer! */
	((trh->rc_blth & RC_BROADBIT) == 0))
	return(FALSE);

    /*
     * CSCdi32284 - We should not bridge frames sent out
     * by us back in! 
     * Check if the src is idb addr
     * and discard the frame.
     */
    if (hwidb) {
      ieee_copy(trh->saddr, temp_saddr);
      temp_saddr[0] &= ~TR_RII;
      if (!ieee_compare(temp_saddr,  hwidb->hardware)) {
	retbuffer(pak);
	return TRUE;
      }
    }

    llc1 = (snap_hdr *)((uchar *) trh + TRING_ENCAPBYTES + orl);
    if (input_llc_type) {
	llc_type = input_llc_type;
    } else {
	switch (llc1->control & ~LLC1_P) {
	  case LLC1_XID:
	    llc_type = "XID";
	    /*
	     * its an XID, make sure its something we recognize.  This has to
	     * be an IEEE 802.2 standard format otherwise we don't know how
	     * to turn it around.  org[0] should be this byte.
	     */
            if (pak->datagramsize - TRING_ENCAPBYTES - orl > SAP_HDRBYTES &&
                  llc1->oui[0] != XID_FMT_STD) {
		if (trh->rc_blth == RC_NORD_LTH) {
		    retbuffer(pak);		/* can't go anywhere */
		    return(TRUE);
		}
		return(FALSE);		/* let it continue on */
	    }
	    break;
	  case LLC1_TEST:
	    llc_type = "TEST";
	    if ((llc1->ssap & ~SAP_RESPONSE) == SAP_IBMNM) {
		if (trh->rc_blth == RC_NORD_LTH) {
		    retbuffer(pak);		/* can't go anywhere */
		    return(TRUE);
		}
		return(FALSE);		/* let it continue on */
	    }
	    break;
	  default:
	    llc_type = "OTHER";
	    break;
	}
    }
    if (llc1->ssap & SAP_RESPONSE) {
	if (trh->rc_blth == RC_NORD_LTH) {
	    retbuffer(pak);		/* can't go anywhere */
	    return(TRUE);
	}
	return(FALSE);			/* let it continue on */
    }
    if ((llc1->dsap != SAP_NULL) && !(non_null_dsap_ok)) {
	if (trh->rc_blth == RC_NORD_LTH) {
	    retbuffer(pak);		/* can't go anywhere */
	    return(TRUE);
	}
	return(FALSE);			/* let it continue on */
    }

    /*
     * Special hack if the beasty is an 02xx rif and this
     * interface has srb_oldsna_hack turned on.  Convert to
     * an explorer and continue on.
     */
    if (trh->rc_blth == RC_NORD_LTH && srb_sb_get_oldsna_hack(hwidb->firstsw)) {
	trh->rc_blth |= RC_B_SPAN;
	srb_enq(srb_explorerQ, pak);
	return(TRUE);
    }

    /*
     * Do we want to proxy?
     */
    if (!srb_sb_get_proxy_explorer(hwidb->firstsw)) {
	if (trh->rc_blth == RC_NORD_LTH) {
	    retbuffer(pak);		/* can't go anywhere */
	    return(TRUE);
	}
	return(FALSE);
    }
	
    /*
     * Find the rif for the address XID'd.  Look for the cases where we
     * want to send a response.  These are:
     *     1) Input in a V-ring
     *         a) Destination ring is remote in same v-ring
     *         b) Destination ring is local in same v-ring
     * 
     *     2) Input in direct bridge
     *         a) Destination ring is opposite side
     */
    level = raise_interrupt_level(NETS_DISABLE);
    cacheptr = rif_lookup(trh->daddr, NULL, NULL, 0);
    if (!cacheptr || (cacheptr->length == 0)) {
	reset_interrupt_level(level);
	if (trh->rc_blth == RC_NORD_LTH) {
	    retbuffer(pak);		/* can't go anywhere */
	    return(TRUE);
	}
	if (cacheptr && (cacheptr->idb == hwidb)) {
	    retbuffer(pak);
	    return(TRUE);
	}
	return(FALSE);
    }
    reset_interrupt_level(level);

    /*
     * Only respond for those not on this ring!  We've already
     * checked to see if we should be doing proxies.  If so then
     * we eat proxies from this ring for nodes on this ring.
     */
    if (cacheptr->idb && hwidb == cacheptr->idb) {
	retbuffer(pak);
	return(TRUE);
    }

    if (rif_loop_check(hwidb, trh, cacheptr)) {
         retbuffer(pak);
         return(TRUE);
    }

    if (TIMER_RUNNING_AND_AWAKE(cacheptr->validate_time) &&
          !(cacheptr->flags &
                 (RIF_NOAGE | RIF_INTERFACE | RIF_HANDCRAFT | RIF_HANDCRAFT_REM))) {
        unset_rif(cacheptr->idb, cacheptr->vrn, cacheptr->hardware,
                                                cacheptr->src_addr, FALSE);
        return(FALSE);
    }

    if ((newpak = pak_duplicate(pak)) == NULL) {
        return(FALSE);
    }

    if (!rif_merge_rifs(pak, newpak, cacheptr)) {
        retbuffer(newpak);
        return(FALSE);
    }
    /* You must do different things if two port SRB versus all others... */
    if (hwidb->tr_bridge_idb) {
        newpak->if_output = hwidb->tr_bridge_idb->firstsw;
        newpak->linktype = LINK_RSRB;
        /*
         * spanning and all routes explorer processed here
         * newpack is changed into SRF by rif_merge_rifs, so check
         * explorer status in (presumably ;) unmodified version.
         * Within if, use 'newpak' as it's this copy which is sent
         * out the interface...
         */
        if (isRifExplorerPak(newpak))
        {
            trh = (trrif_t *) newpak->datagramstart;
            /*
             *
             * Processing of simple two-port case was put here, instead of
             * within 'datagram_out'.  The choice placement was to minimize
             * the amount of code within datagram_out, and allow faster
             * operation for other clients, about 150 as of 11/19/92...
             * The large number of calling locations was also the motivation _not_
             * to include an extra parameter to differentaite the call made here
             * from (for example) srb_background().
             */
            /*
             * Set new rif to minimum of that contained in frame, and
             * the Lf supported by the input and output ports...
             */
            setRifLfTrh(trh, min(getRifLfTrh(trh),
                                  min(getHwLf(newpak->if_input->hwptr),
                                      getHwLf(newpak->if_input->hwptr->tr_bridge_idb)
                                      )
                                  )
                        );
        }
        tr_to_media_out(newpak);
    } else {
          /*
           * Unlike the case above, Lf (largest frame) processing of
           * explorer frames is handled from within vrforward_pak().
           * The decision of placement was due to the low number of
           * calling clients (about 10) and the complexity of retrieving
           * supporting virtual data structures which are already retrieved once.
           */

        vrforward_pak(newpak, FALSE, RSRB_OP_FORWARDCRC, RSRB_RIFLF_KNOCKDOWN);
    }

    if (!TIMER_RUNNING(cacheptr->validate_time) &&
	!((cacheptr->flags & RIF_AGED) && rif_validate_enable) &&
        !(cacheptr->flags &
                 (RIF_NOAGE | RIF_INTERFACE | RIF_HANDCRAFT | RIF_HANDCRAFT_REM)))
        TIMER_START(cacheptr->validate_time, rif_validation_age);

    if (did_reply != NULL)
      *did_reply = TRUE;
    /*
     * When I return TRUE, I better consume the original pak
     * CSCdi14950
     */
    retbuffer(pak);
    return(TRUE);
}

static char fs_rif_string[SRB_MAXRIF*2+SRB_MAXRIF/2];

/*
 *  If the "source-bridge explorer-dup-ARE-filter" is configured, 
 *  then drop copies of explorers we believe we have seen.
 */
static boolean srb_ARE_is_duplicate (paktype        *pak, 
                                     hwidbtype      *idb,
                                     trrif_t        *trh)
{
    srbrd_t             *trrd;
    ushort              rd_cnt, ring_num;
    tr_vgrouptype       *vrg;
    srb_local_ring_type *srb_if = NULL;

    /*
     *  For All-Routes Explorers only, check the RIF to see 
     *  if a copy of this explorer has already been forwarded.  
     *  This is determined by looking for matches between the 
     *  ring numbers in the RIF (excluding the last ring) and the 
     *  ring numbers of the real rings attached to the virtual ring.
     *  This filtering mechanism is described in 802.1D - pp 162-163.
     */
    
    if((trh->rc_blth & RC_RTMASK) == RC_B_SPAN)
        return FALSE;

    vrg = vrfind_ringgroup(srb_sb_get_targetring(idb->firstsw));
    if (vrg) {
        trrd = (srbrd_t *) &trh->rd[0];
        for (rd_cnt = (trh->rc_blth & RC_LTHMASK) - 2; rd_cnt > 2; rd_cnt -= 2) {
             ring_num = GETSHORT(trrd) >> RD_RNSHIFT;
             for (srb_if = (srb_local_ring_type *) vrg->local_rings.qhead; 
                  srb_if; srb_if = srb_if->next) {
                  if ((ring_num == srb_sb_get_thisring(srb_if->idb->firstsw)) &&
                      (srb_if->idb->state == IDBS_UP) &&
                      !idb_is_throttled(srb_if->idb) ) {
                       if (srb_debug) {
                           buginf("\nSRB %d/%d: Duplicate explorer dropped - s: %e d: %e rif: %s",
                             idb->slot, idb->unit, trh->saddr, trh->daddr, 
                             get_rif_str((uchar *)&trh->rc_blth, fs_rif_string));
                       }
                       return TRUE;
                  }
             }
             trrd++;
        }
    }    
    return FALSE;
}

/* Check if the idb target ring is already in the rif.
 *
 * This function is primarily used to detect duplicate virtual rings, 
 * since the target ring of the input idb in a multiring configuration
 * is, in fact, the virtual ring.
 */

static boolean srb_target_ring_check (hwidbtype *idb, paktype *pak)
{
    trrif_t *trh  = (trrif_t *) pak->datagramstart;
    srbrd_t *trrd = (srbrd_t *) &trh->rd[0];
    int riflen    = trh->rc_blth & RC_LTHMASK;

    for (riflen -= 2; riflen; riflen -= 2, trrd++)
        if ((GETSHORT(trrd) >> 4) == srb_sb_get_targetring(idb->firstsw)) {
            if (srbevent_debug)
                buginf("\nFAST%d/%d: duplicate ring violation,"
                            " s: %e d: %e rif: %s", idb->slot,
                            idb->unit, trh->saddr, trh->daddr,
                            get_rif_str(&trh->rc_blth, fs_rif_string));
            return(FALSE);
	    }

    return(TRUE);
}


/*
 * Bridge flag is set.  There are four ways to handle this packet.
 * They are each handled separately:
 *       1) XID and TEST - do proxy if set set up for it
 *       2) Explorer - modify, send, and forward
 *       3) Local - send out to real interfaces
 *       4) Remote - forward to virtual rings
 */
void srb_common (paktype *pak, hwidbtype *idb, int riflen)
{
    trrif_t *trh = (trrif_t *) pak->datagramstart;
    snap_hdr *snap = (snap_hdr *)((uchar *)&trh->rc_blth + riflen);
    hwidbtype *outidb;
    uchar *dgstart = pak->datagramstart;
    int dgsize = pak->datagramsize;
    uchar address[IEEEBYTES];
    uchar vaddr[IEEEBYTES];
    boolean is_for_rsrb;
    tr_ventrytype *vre;
    tr_vgrouptype *vrg;

    srb_sb_incr_rx_counters(idb->firstsw, dgsize);
    if (idb->srb_access_in && !srb_in_check(dgstart, dgsize, idb, riflen)) {
	retbuffer(pak);
	return;
        }

    /* 
     * Check for protocols we should be routing. The microcode handles
     * this for us when autonomous switching. We let ARP's through
     * when IP routing is off. This code will need to change when
     * routing is moved from global status to per interface (routing
     * and bridging the same protocol).
     */

    if (ROUTING_RXTYPE(idb->firstsw, pak->rxtype)) {
        if (srberr_debug)
            buginf("\nSRB%d drop: Routed protocol %x", idb->unit, pak->rxtype);
        retbuffer(pak);
        return;
        }
 
    if (pak->rxtype == RXTYPE_UNKNOWN) {
        if(snap->dsap == SAP_SNAP && snap->ssap == SAP_SNAP 
        && snap->type == TYPE_RFC826_ARP 
        && ROUTING_RXTYPE(idb->firstsw,RXTYPE_DODIP)) {
            if (srberr_debug)
                buginf("\nSRB%d drop: TYPE_RFC826_ARP", idb->unit);
            retbuffer(pak);
            return;
            }
        }

    /* assumes rif in packet! */
    if (global_proxy_explorer_enable)
	rif_update(idb, 0, trh->saddr, NULL, (srbroute_t *)&trh->rc_blth,RIF_AGED);

    if(global_netbios_name_cache_enable)
	netbios_name_cache_update_pak((uchar *) trh, NETBIOS_NAME_AGED, idb, 0);

    if(((snap->control & ~LLC1_P) == LLC1_XID
    ||  (snap->control & ~LLC1_P) == LLC1_TEST)
    &&  !srb_proxy_netbios_only)
        if(srb_proxy(pak, NULL, NULL, FALSE))
	    return;

    if(snap->dsap == SAP_NETBIOS && (snap->control & ~LLC1_P) == LLC1_UI)
        if(netbios_name_cache_proxy(pak))
	    return;

    if(trh->rc_blth & RC_BROADBIT) {

	/* If explorer is from local ring try to fastswitch localy. */

	if (is_srb_hw(idb)) {

            /* Drop packet if srb is not configured on this interface. */
 
            if (!srb_sb_get_bridging_enabled(idb->firstsw)) {
                retbuffer(pak);
                return;
            }
 
            /* Drop fddi packet with duplicate virtual ring numbers. */
 
            if ((idb->status & IDB_FDDI)
            && !srb_target_ring_check(idb, pak)) {
                retbuffer(pak);
                srb_sb_incr_rx_drops(idb->firstsw);
                return;
                }

            /* Drop duplicate All-Routes-Explorers. */

            if (fs_explorer.ARE_dup_filter && (riflen >= 6) 
	    && srb_ARE_is_duplicate(pak, idb, trh)) {
                retbuffer(pak);
                srb_sb_incr_rx_drops(idb->firstsw);
                return;
                }
 
            /* Try to fastswitch the explorer. */
 
            fs_explorer.total++;
            idb->fs_expl_input->count++;

	    if(fs_explorer.enabled 
	    && !srb_fast_explorers(pak, idb, riflen)) {
	        retbuffer(pak);
                srb_sb_incr_rx_drops(idb->firstsw);
		return;
	        }
	    /* Packet passed all filtering.  Adjust the data rate value
	     * that is used to decide when packets need to be flushed. */

	    idb->fs_expl_input->data_rate += (ulong) dgsize;
	    }

	/* Enqueue explorer for schedualed processing. */

        if(!explorer_enq(pak)) {
	    retbuffer(pak);
            srb_sb_incr_rx_drops(idb->firstsw);
	    }

	return;
        }

    /* DLSw hooks */
    switch (dlsw_demux(pak, idb)) {
      case D_DLSW:           /* Final ring terminating at the router */
          reg_invoke_dlsw_work(pak, idb, riflen);
          return;
      case D_RSRB:
          break;
      case D_LNM:
	  if (reg_invoke_dlsw_running(0,0,0)) {
	      ieee_copy(trh->daddr, address);
	      address[0] &= 0x3F;
	      if (ieee_equal(address, idb->hardware)) {
	          reg_invoke_raw_enqueue(pak->linktype,pak);
                  return;
              } else {
                  /*
                   * Determine if DA is Virtual address of RSRB Peer
                   * If so, let RSRB handle it, not DLSw+ (break)
                   */
                  is_for_rsrb = FALSE;
                  for (vrg = (tr_vgrouptype *) vringQ.qhead; 
                        vrg && (!is_for_rsrb); vrg = vrg->next) {
                      for (vre = (tr_ventrytype *) vrg->rings.qhead; vre;
                                                      vre = vre->next) {
                          if (vre->type & RSRB_ET_REMOTE) {
                              ieee_copy(vre->macaddr, vaddr);
                              vaddr[0] &= 0x3F;
                              if (ieee_equal(address, vaddr)) {
                                  is_for_rsrb = TRUE;
                                  break;  /* Inner for loop */
			      }
			  }
		      }
		  }
                  if (!is_for_rsrb) {
                      reg_invoke_dlsw_work(pak, idb, riflen);
                      return;
                  }
	      }
	  }
	  break;
      default:
          retbuffer(pak);
          return;
    }

    if(idb->tr_bridge_idb) {
	outidb = idb->tr_bridge_idb;
	
	if (!srb_out_check(dgstart, dgsize, outidb, riflen)) {
	    retbuffer(pak);
	    return;
	}
	
	pak->if_output = outidb->firstsw;
	pak->flags |= PAK_SRB_OUT;
	pak->acct_proto = ACCT_PROTO_SRB;
        srb_sb_incr_tx_nb_counters(pak->if_output, dgsize);
	tr_to_media_out(pak);
	if (srb_debug) {
            srb_triplet_t srb_triplet;
            srb_sb_get_triplet(idb->firstsw, &srb_triplet);
	    buginf("\nSRB%d: direct forward (srn %d bn %d trn %d)",
		   idb->unit,
                   srb_triplet.localRing,
                   srb_triplet.bridgeNum,
                   srb_triplet.remoteRing);
        }
	return;
    }

    if(trh->rc_blth & RC_TRACE_FRAME)
        reg_invoke_send_trace_report(pak, idb, NULL);

    vrforward_pak(pak, FALSE, 0, RSRB_RIFLF_LEAVEALONE);
}


/*
 * Fast explorer rif update.
 * Copy rif from packet into rif buffer where it can be modified without
 * modifying the original packet.  If no route descriptors then include 
 * first descriptor info, if there's a virtual ring in the path include
 * it, adjust the rif size of the buffered copy just the first time.  
 * 
 * In all cases update the target ring info in the buffer.  The size has
 * already been calculated so don't change the length field again.
 *
 * Return FALSE if bad length.
 */

static inline boolean fs_expl_rif_update (
    hwidbtype *idb,
    paktype   *pak,
    fs_hdrtype *hcb,
    hwidbtype *oidb)
{
    srbrd_t *trrd;
    ushort maxmtu;
    char mtu_lf;
    srb_sb_t *srb_sb, *osrb_sb;
   

    /* Sanity check ... have the working buffers been alocated? */

    srb_sb  = srb_get_sb_inline(idb->firstsw);
    osrb_sb = srb_get_sb_inline(oidb->firstsw);
    if(!hcb || !fs_explorer.newpak || !srb_sb || !osrb_sb)
	return(FALSE);

    /* If valid flag is FALSE then this is first time through here for
     * this packet.  Fill in the control structure and update rif buf. */

    if(!hcb->valid) {

	/* Set up some packet specific header control block fields. */

	hcb->paktrh = (trrif_t *)pak->datagramstart;
	hcb->pakriflen = hcb->paktrh->rc_blth & RC_LTHMASK;
	hcb->pakdataptr = pak->datagramstart+TRING_ENCAPBYTES+hcb->pakriflen;
	hcb->pakdatasize = pak->datagramsize-TRING_ENCAPBYTES-hcb->pakriflen;

	hcb->newtrh = (trrif_t *)fs_explorer.newpak->datagramstart;

        /* Copy header info into header buffer. */

        bcopy(hcb->paktrh, hcb->newtrh, TRING_ENCAPBYTES + hcb->pakriflen);

	/* Update the copy of the rif in the header buffer. */

        if(hcb->pakriflen == 2) {
            trrd = (srbrd_t *) &hcb->newtrh->rd[0];
            trrd->ring_num   = srb_sb->srb_thisring;
            trrd->bridge_num = srb_sb->srb_bridge_num;
            hcb->newtrh->rc_blth += 2;
            }
	else{
            trrd = (srbrd_t *)((uchar *) &hcb->newtrh->rd[0]+hcb->pakriflen-4);
            trrd->bridge_num = srb_sb->srb_bridge_num;
            }

        hcb->newtrh->rc_blth += 2;

	if (idb->tr_vring_blk) {
	    trrd++;
	    trrd->ring_num   = srb_sb->srb_targetring;
            hcb->vrg_rd = trrd;
	    hcb->newtrh->rc_blth += 2;
            }
        else
            hcb->vrg_rd = NULL;

	/* Finish updating the packet specific control block fields. */ 

	hcb->newriflen = hcb->newtrh->rc_blth & RC_LTHMASK;
	hcb->newdataptr = (char *)hcb->newtrh+TRING_ENCAPBYTES
					     +hcb->newriflen;
	hcb->dgsize = TRING_ENCAPBYTES+hcb->newriflen+hcb->pakdatasize;
	hcb->valid = EXPL_RIF_UPDATED;

        hcb->iidb = idb;
        }

    /* If explorer part of virtual ring group, 
     * then add in output interface bridge number */

    if (hcb->vrg_rd)
        hcb->vrg_rd->bridge_num = osrb_sb->srb_bridge_num;

    /* Add the current hop for the output idb. */

    trrd = (srbrd_t *) ((uchar *) &hcb->newtrh->rd[0]+hcb->newriflen-4);
 
    trrd->ring_num = osrb_sb->srb_thisring;
    trrd->bridge_num = 0;

    /* Check now for duplicate ring numbers in the rif. */

    while(--trrd >= (srbrd_t *)&hcb->newtrh->rd[0])
	if(trrd->ring_num == osrb_sb->srb_thisring)
	    return(FALSE);

    /* Adjust the longest-frame code.  Pick the smallest mtu between
     * the original packet and the output idb.  Update the rif copy. */

    if(oidb->status & IDB_802ONLY)
        maxmtu = oidb->maxmtu + LLC_DATA_OFFSET;
    else
        maxmtu = oidb->maxmtu;

    mtu_lf = (bytes_to_rc_code((int)maxmtu) & RC_LFMASK);

    if((hcb->paktrh->rc_dlf & RC_LFMASK) < mtu_lf)
	hcb->newtrh->rc_dlf = (hcb->paktrh->rc_dlf & RC_LFMASK);
    else
	hcb->newtrh->rc_dlf = mtu_lf;

    return(TRUE);
}

/*
 * srb_fast_explorers - Fast switch explorer packets at interrupt level.
 *
 * Do access list checking and copy localy bound explorer directly
 * to the token ring interfaces.  The fowarding queues contain the
 * idb pointers.  The queue is maintained by the process level explorer
 * handler.  When the packet has been forwarded to all local interfaces
 * it will be put on the explorer queue.  The schedualed process checks
 * the forwarding queue to determine whether the packet has been sent.
 *
 */

boolean srb_fast_explorers (
    paktype *pak,
    hwidbtype *idb, 
    int riflen)
{
    trrif_t   *trh   = (trrif_t *) pak->datagramstart;
    boolean   spanning = FALSE;
    hwidbtype *oidb;
    srb_sb_t  *srb_sb;
    fs_expl_que *forwardQ, *entry;
    int outriflen = riflen + (idb->tr_vring_blk ? 4 : 2) + (riflen == 2 ? 2 : 0);

    /* Mark explorer as having been seen by this fastswitch function. */

    pak->flags |= PAK_SRB_EXP_FAST;

    /* Spanning or All Routes?  Pick the right forwarding queue. 
     * If the queue is empty let the system handle this explorer
     * at process level where the forwarding queue will be updated. */

    srb_sb = srb_get_or_create_sb_inline(idb->firstsw);
    if (!srb_sb) {
        printf(nomemory);
        return(FALSE);
    }
    if ((trh->rc_blth & RC_RTMASK) == RC_B_SPAN) {
        if ((!srb_sb->srb_spanning_explorer) || 
            (riflen > ((srb_sb->srb_maxin_hops + 1) * 2 + 2))) {
            return(FALSE);
        }

        if (!idb->fs_expl_spanQ)
	    return(TRUE);

        forwardQ = idb->fs_expl_spanQ;
	srb_sb->srb_span_in++;
	spanning = TRUE;
    } else {
        if (outriflen > (srb_sb->srb_max_hops + 1) * 2 + 2)
            return(FALSE);

        if (!idb->fs_expl_allQ)
	    return(TRUE);

        forwardQ = idb->fs_expl_allQ;
	srb_sb->srb_allrte_in++;
    }

    /* The forwarding queue is valid so mark the explorer as having been
     * sent by this fastswitch function and flood the packet to interfaces
     * on the forwarding queue. */

    pak->flags |= PAK_SRB_EXP_SENT;
    pak->linktype = LINK_RSRB;
    fs_explorer.hcb->valid = FALSE;

    /* Check output access and forward to all interfaces on the queue. */

    for (entry=forwardQ; entry; entry=entry->next) {
    
	oidb = entry->idb;

	if (oidb->state != IDBS_UP || !oidb->fs_expl_func) {
	    entry->prune++;
	    fs_explorer.prune++;
            continue;
	    }

	if (spanning) {
            ushort maxout_hops = srb_sb_get_maxout_hops(oidb->firstsw);
            if (!srb_sb_get_spanning_explorer(oidb->firstsw) || 
                (outriflen > (maxout_hops + 1) * 2 + 2 )) {
                srb_sb_incr_tx_drops(oidb->firstsw);
                continue;
	        }
            }

        if (oidb->srb_access_out 
	&& !srb_out_check((uchar *) trh, pak->datagramsize, oidb, riflen)) {
            srb_sb_incr_tx_drops(oidb->firstsw);
            continue;
	    }

        /* Update temp rif buffer with next hop route descriptor(s) */

        if (!fs_expl_rif_update(idb, pak, fs_explorer.hcb, oidb)) {
            srb_sb_incr_tx_drops(oidb->firstsw);
	    continue;
	    }

        /* Copy an explorer packet out to interface in two steps:
         * 1) The header buffer which includes the updated rif.
         * 2) The original explorer packet after the rif.  
         *
         * Note: The handler is responsible for incrementing the 
         *       output drops for fast explorers.
         */

        if ((*oidb->fs_expl_func)(pak, fs_explorer.hcb, oidb)) {
	    if (spanning)
                srb_sb_incr_span_out_counter(oidb->firstsw);
	    else
                srb_sb_incr_allrte_out_counter(oidb->firstsw);
	    
            if (srb_debug)
                buginf("\nFAST%d/%d: sent explorer at interrupt, [%s]", 
		       oidb->slot, oidb->unit, 
		       get_rif_str(&fs_explorer.hcb->newtrh->rc_blth,
		       fs_rif_string));
	    }
        }

    return(TRUE);
}

/* explorer_enq - Queue the explorer for scheduled processing.
 * 
 * Implement process bound explorer packet flow control by discarding
 * explorers when the queue is full and or the packet rate is too great.
 *
 * Note that this only applies to packets on the slow path.  Flow control 
 * for fast switched explorers is implemented in flush_explorer().
 */
boolean explorer_enq (paktype *pak)
{
    fs_expl_que *input = pak->if_input->hwptr->fs_expl_input;

    if (!srb_running)
	return(FALSE);

    if (input) {

	/* First check if the explorer queue is full. */

	if (process_is_queue_full(srb_explorerQ)) {
            input->congested = TRUE;
        } else {
	    /*
	     * Increment storm count if explorers are coming in too fast.
	     */
	    if (++input->queue_depth >= process_queue_max_size(srb_explorerQ)) {
		if (!input->congested) {
		    input->congested = TRUE;
		    input->storms++;
		}
	    } else {
		/*
		 * The explorer queue is fine.  Ensure congested flag is false.
		 */
		input->congested = FALSE;
	    }
	}

	/* If timer popped then reset the input queue count and timer. */

        if (AWAKE(input->queue_timer)) {
            input->queue_depth = 0;
	    TIMER_START(input->queue_timer, ONESEC/10);
	}

        if (input->congested)
            return(FALSE);
    }

    process_enqueue_pak(srb_explorerQ, pak);

    return(TRUE);
}



boolean srb_in_check (
    uchar *dgstart,
    ulong dgsize,
    hwidbtype *idb,
    int riflen)
{
    ushort lsap, typecode;
    uchar address[IEEEBYTES];
    snap_hdr *llc1;		/* pointer to llc-1 header */
    boolean value;
    int res;
    netbios_sb_t *netb_sb;
    
    if (idb->access_expression_in) {
      res = expr_evaluate_tree(dgstart, dgsize, riflen, idb,
			       idb->access_expression_in, &value);
      if (res != EXPR_OK)
	return(TRUE);
      if (value == FALSE) {
          srb_sb_incr_rx_drops(idb->firstsw);
	  if (srbevent_debug || srberr_debug)
	      buginf("\nSRB: failed input access expression on %s", idb->hw_namestring);
      }
      return(value);
    }
    /*
     * Do input LSAP/type access checking.
     *  - If has an LSAP, check it.
     *  - If LSAP okay and is SNAP, then check the SNAP type.
     * Do input address access checking.
     */
    if (idb->srb_lsap_in) {
	lsap = GETSHORT((dgstart+TRING_ENCAPBYTES+riflen));
	if (!access_check_type(idb->srb_lsap_in, lsap)) {
            srb_sb_incr_rx_drops(idb->firstsw);
	    if (srbevent_debug || srberr_debug)
		buginf("\nSRB: failed type check, dropping packet on %s", idb->hw_namestring);
	    return(FALSE);
	}
	if ((lsap == SNAPSNAP) && idb->srb_type_in) {
	    typecode = GETSHORT((dgstart
				 +TRING_ENCAPBYTES+riflen+LLC_LSAP_OFFSET));
	    if (!access_check_type(idb->srb_type_in, typecode)) {
                srb_sb_incr_rx_drops(idb->firstsw);
		if (srbevent_debug)
		    buginf("\nSRB: failed type check, dropping packet on %s", idb->hw_namestring);
		return(FALSE);
	    }
	}
    }
    if (idb->srb_address_in) {
	ieee_copy(dgstart + TR_SA_OFFSET, address);
	if (!access_check_mac(idb->srb_address_in, address)) {
            srb_sb_incr_rx_drops(idb->firstsw);
	    if (srbevent_debug || srberr_debug)
		buginf("\nSRB: failed address check, dropping packet on %s", idb->hw_namestring);
	    return(FALSE);
	}
    }
    /* access filtering based on type codes - only done for SNAP frames */
    if (idb->srb_type_in) {
        lsap = GETSHORT((dgstart+TRING_ENCAPBYTES+riflen));
        if (lsap == SNAPSNAP) {
            typecode = GETSHORT((dgstart
                                 +TRING_ENCAPBYTES+riflen+LLC_LSAP_OFFSET));
            if (!access_check_type(idb->srb_type_in, typecode)) {
                srb_sb_incr_rx_drops(idb->firstsw);
                return(FALSE);
            }
        }
    }
    /*
     * Support for NETBIOS access filters
     */

    /* Get the DSAP -- must be LLC, UI, and NETBIOS frame for this check */
    netb_sb = netbios_get_sb(idb->firstsw);
    if (netb_sb && 
        (netb_sb->ibm_netbios_h_iacf || netb_sb->ibm_netbios_b_iacf)) {
	llc1 = (snap_hdr *)((uint) dgstart + TRING_ENCAPBYTES + riflen);
	if (((llc1->control & ~LLC1_P) == LLC1_UI) &&
	    (llc1->dsap == SAP_NETBIOS)) {
	    if (!netbios_acfcheck(((uchar *) ((uint) dgstart
					      + (TRING_ENCAPBYTES + ET_SAP_LEN +
						 riflen))),
				  (dgsize - TRING_ENCAPBYTES - ET_SAP_LEN -
				   riflen),
				  idb->firstsw, TRUE, NULL, FALSE,
				  NETBIOS_TOKEN_LLC2_CHECK)) {
                srb_sb_incr_rx_drops(idb->firstsw);
		if (srbevent_debug || srberr_debug)
		    buginf("\nSRB: failed netbios filter, dropping packet on %s", idb->hw_namestring);
		return(FALSE);
	    }
	}
    }
    /*
     * Netbios session level byte filter
     */
    if (netb_sb && netb_sb->ibm_netbios_bs_iacf) {
        llc1 = (snap_hdr *)((uint) dgstart + TRING_ENCAPBYTES + riflen);
        if (((llc1->control & LLC_IMASK) == LLC_IFRAME) &&
            (llc1->dsap == SAP_NETBIOS)) {
            if (!netbios_session_acfcheck(((uchar *) ((uint) dgstart
                + (TRING_ENCAPBYTES + ET_SAP_LEN + riflen + 1))),
                (dgsize - TRING_ENCAPBYTES - ET_SAP_LEN - riflen - 1),
                netb_sb->ibm_netbios_bs_iacf)) {
                srb_sb_incr_rx_drops(idb->firstsw);
                if (srbevent_debug || srberr_debug)
                    buginf("\nSRB: input netbios access list failed on %s",
                        idb->hw_namestring);
                return(FALSE);
            }
        }
    }
    return(TRUE);
}


/* 
 * Output checking for token rings outbound packets
 */

boolean srb_out_check (
    uchar *dgstart,
    ulong dgsize,
    hwidbtype *outidb,
    int riflen)
{
    ushort lsap, typecode;
    uchar address[IEEEBYTES];
    snap_hdr *llc1;		/* pointer to llc-1 header */
    boolean value;
    int res;
    netbios_sb_t *netb_sb;
    
    if (outidb->access_expression_out) {
      res = expr_evaluate_tree(dgstart, dgsize, riflen, outidb,
			       (expr_tree_entry_t *)
			       outidb->access_expression_out, &value);
      if (res != EXPR_OK)
	return(TRUE);
      if (value == FALSE) {
          srb_sb_incr_tx_drops(outidb->firstsw);
	  if (srbevent_debug || srberr_debug)
	      buginf("\nSRB: output tree access list failed on %s", outidb->hw_namestring);
      }
      return(value);
    }
    /*
     * Do output LSAP/type access checking.
     *  - If has an LSAP, check it.
     *  - If LSAP okay and is SNAP, then check the SNAP type.
     * Do output address access checking.
     */
    if (outidb->srb_lsap_out) {
	lsap = GETSHORT((dgstart+TRING_ENCAPBYTES+riflen));
	if (!access_check_type(outidb->srb_lsap_out, lsap)) {
            srb_sb_incr_tx_drops(outidb->firstsw);
	    if (srbevent_debug || srberr_debug)
		buginf("\nSRB: output type access list failed on %s", outidb->hw_namestring);
	    return(FALSE);
	}
	if ((lsap == SNAPSNAP) && outidb->srb_type_out) {
	    typecode = GETSHORT((dgstart+
				 TRING_ENCAPBYTES+riflen+
				 LLC_LSAP_OFFSET));
	    if (!access_check_type(outidb->srb_type_out, typecode)) {
                srb_sb_incr_tx_drops(outidb->firstsw);
		if (srbevent_debug || srberr_debug)
		    buginf("\nSRB: output type access list failed on %s", outidb->hw_namestring);
		return(FALSE);
	    }
	}
    }
    if (outidb->srb_address_out) {
	ieee_copy(dgstart + TR_DA_OFFSET, address);
	if (!access_check_mac(outidb->srb_address_out, address)) {
            srb_sb_incr_tx_drops(outidb->firstsw);
	    if (srbevent_debug || srberr_debug)
		buginf("\nSRB: output address access list failed on %s", outidb->hw_namestring);
	    return(FALSE);
	}
    }
    /* Access filtering based on type codes for SNAP encapsulated packets */
    if (outidb->srb_type_out) {
        lsap = GETSHORT((dgstart+TRING_ENCAPBYTES+riflen));
        if (lsap == SNAPSNAP) {
            typecode = GETSHORT((dgstart+
                                 TRING_ENCAPBYTES+riflen+
                                 LLC_LSAP_OFFSET));
            if (!access_check_type(outidb->srb_type_out, typecode)) {
                srb_sb_incr_tx_drops(outidb->firstsw);
                return(FALSE);
            }
        }
    }
    /*
     * Support for NETBIOS access filters
     */
    /* Get the DSAP -- must be LLC, UI, and NETBIOS frame for this check */
    netb_sb = netbios_get_sb(outidb->firstsw);
    if (netb_sb && 
        (netb_sb->ibm_netbios_h_oacf || netb_sb->ibm_netbios_b_oacf)) {
	llc1 = (snap_hdr *)((uint) dgstart + TRING_ENCAPBYTES + riflen);
	if (((llc1->control & ~LLC1_P) == LLC1_UI) &&
	    (llc1->dsap == SAP_NETBIOS)) {
	    if (!netbios_acfcheck(((uchar *) ((uint) dgstart
					      + (TRING_ENCAPBYTES + ET_SAP_LEN +
						 riflen))),
				  (dgsize - TRING_ENCAPBYTES - ET_SAP_LEN -
				   riflen),
				  outidb->firstsw, FALSE, NULL, FALSE,
				  NETBIOS_TOKEN_LLC2_CHECK)) {
                srb_sb_incr_tx_drops(outidb->firstsw);
		if (srbevent_debug || srberr_debug)
		    buginf("\nSRB: output netbios access list failed on %s", outidb->hw_namestring);
		return(FALSE);
	    }
	}
    }
    /*
     * Netbios session level byte filter
     */
    if (netb_sb && netb_sb->ibm_netbios_bs_oacf) {
        llc1 = (snap_hdr *)((uint) dgstart + TRING_ENCAPBYTES + riflen);
        if (((llc1->control & LLC_IMASK) == LLC_IFRAME) &&
            (llc1->dsap == SAP_NETBIOS)) {
            if (!netbios_session_acfcheck(((uchar *) ((uint) dgstart
                + (TRING_ENCAPBYTES + ET_SAP_LEN + riflen + 1))),
                (dgsize - TRING_ENCAPBYTES - ET_SAP_LEN - riflen - 1),
                netb_sb->ibm_netbios_bs_oacf)) {
                srb_sb_incr_rx_drops(outidb->firstsw);
                if (srbevent_debug || srberr_debug)
                    buginf("\nSRB: output netbios access list failed on %s",
                        outidb->hw_namestring);
                return(FALSE);
            }
        }
    }
    return(TRUE);
}

void do_bridge_number_setup (hwidbtype *idb1, hwidbtype *idb2)
{
        remove_vring_interface(idb1, srb_sb_get_targetring(idb1->firstsw));
        remove_vring_interface(idb2, srb_sb_get_targetring(idb2->firstsw));
	set_srb_interfaces();
        set_vring_interfaces();
	fs_srb_setup();
	reg_invoke_srt_setup_auto_sw_srb(idb1);
	reg_invoke_srt_setup_auto_sw_srb(idb2);
	reg_invoke_set_ibmnm_interfaces(idb1, TRUE);
	reg_invoke_set_ibmnm_interfaces(idb2, TRUE);
}


/* process rsrb filtering command */
void rsrb_filter_command(parseinfo *csb)

{
    int vrn, type;
    rsrb_cookie cookie;
    uchar macaddr[IEEEBYTES];
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    int value;
    netbios_acclisttype *netblist = NULL;



    if (system_loading)
        return;

    if (!bridge_enable) {
        printf("\n%%Source Bridging not supported in this release");
        return;
    }

    if (csb->nvgen) {
        for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
            for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
                switch(vrp->type) {
                    case RSRB_PT_TCPD:
                        nv_write(vrp->rsrb_lsap_out,
                            "rsrb remote-peer %d tcp %i lsap-output-list %d",
                            vrg->vrn, vrp->ipaddr, vrp->rsrb_lsap_out);
                        nv_write((int)vrp->rsrb_netb_out,
                            "rsrb remote-peer %d tcp %i netbios-output-list host %s",
                            vrg->vrn, vrp->ipaddr, vrp->rsrb_netb_out->name);
                        break;
                    case RSRB_PT_FST:
                        nv_write(vrp->rsrb_lsap_out,
                            "rsrb remote-peer %d fst %i lsap-output-list %d",
                            vrg->vrn, vrp->ipaddr, vrp->rsrb_lsap_out);
                        nv_write((int)vrp->rsrb_netb_out,
                            "rsrb remote-peer %d fst %i netbios-output-list host %s",
                            vrg->vrn, vrp->ipaddr, vrp->rsrb_netb_out->name);
                        break;
                    case RSRB_PT_IF:
                        nv_write(vrp->rsrb_lsap_out,
                            "rsrb remote-peer %d interface %s lsap-output-list %d",
                            vrg->vrn, vrp->outidb->hw_namestring, 
                            vrp->rsrb_lsap_out);
                        nv_write((int)vrp->rsrb_netb_out,
                            "rsrb remote-peer %d interface %s netbios-output-list host %s",
                            vrg->vrn, vrp->outidb->hw_namestring,
                            vrp->rsrb_netb_out->name);
                        break;
                    case RSRB_PT_FR:
                        nv_write(vrp->rsrb_lsap_out,
                            "rsrb remote-peer %d interface %s lsap-output-list %d",
                            vrg->vrn, vrp->outswidb->namestring, 
                            vrp->rsrb_lsap_out);
                        nv_write((int)vrp->rsrb_netb_out,
                            "rsrb remote-peer %d interface %s netbios-output-list host %s",
                            vrg->vrn, vrp->outswidb->namestring,
                            vrp->rsrb_netb_out->name);
                        break;
                    case RSRB_PT_LAN:
                        nv_write(vrp->rsrb_lsap_out,
                            "rsrb remote-peer %d interface %s %e lsap-output-list %d",
                            vrg->vrn, vrp->outidb->hw_namestring, vrp->macaddr,
                            vrp->rsrb_lsap_out);
                        nv_write((int)vrp->rsrb_netb_out,
                            "rsrb remote-peer %d interface %s %e netbios-output-list host %s",
                            vrg->vrn, vrp->outidb->hw_namestring, vrp->macaddr,
                            vrp->rsrb_netb_out->name);
                        break;
                    case RSRB_PT_NONE:
                        break;
                }
            }
        }
        return;
    }

    vrn = GETOBJ(int, 1);
    cookie.idb = NULL;
    type = RSRB_PT_NONE;
    ieee_copy(zeromac, macaddr);
    switch (GETOBJ(int, 2)) {
        case SRB_REMPR_TCP:
            cookie.ip = GETOBJ(paddr, 1)->ip_addr;
            type = RSRB_PT_TCPD;
        break;
        case SRB_REMPR_INT:
            cookie.idb = GETOBJ(idb, 1)->hwptr;
            if (GETOBJ(idb,1)->hwptr->status & IDB_SERIAL) {
                type = RSRB_PT_IF;
            } else {
                if (GETOBJ(idb,1)->hwptr->status & IDB_IEEEMASK) {
                    ieee_copy(GETOBJ(hwaddr,1)->addr, macaddr);
                    type = RSRB_PT_LAN;
                }
            }
            break;
        case SRB_REMPR_FST:
            cookie.ip = GETOBJ(paddr,1)->ip_addr;
            type = RSRB_PT_FST;
            break;
        default:
            printf("\n%% Invalid type, interface, or address");
            return;
    }
    vrg = vrfind_ringgroup(vrn);
    if (!vrg) {
        printf("\nring group %d does not exist", vrn);
        return;
    }
    vrp = vrfind_peer(vrg, type, cookie, 0, 0, macaddr);
    if (!vrp) {
        printf("\nRemote peer does not exist");
        return;
    }
    if (GETOBJ(int, 4) == TRUE) { /* this is lsap filtering */
        value = GETOBJ(int, 5);
        if (!csb->sense)
            vrp->rsrb_lsap_out = 0;
        else
            vrp->rsrb_lsap_out = value;
    }
    if (GETOBJ(int, 3) == TRUE) { /* this is netbios host filtering */
        if (!csb->sense) {
            vrp->rsrb_netb_out = NULL;
        } else {
            if ((netblist =
                find_namedthing(&netbios_host_accQ, GETOBJ(string, 1)))== NULL) {
                printf("\nUnknown netbios host filter - %s",GETOBJ(string, 1));
                return;
            } else {
                vrp->rsrb_netb_out = netblist;
            }
        }
    }
}

/* SRB AST support */
/* 
 * source bridge spanning tree commands.
 * Commands supported :
 * 	source-bridge spanning <group number>
 *	source-bridge spanning <group number> pathcost <number>
 *      source-bridge priority <group number> priority <number>
 */

void srb_span_command (parseinfo *csb, hwidbtype *idb)
{
    spantype *span;
    int index = 0;
    int old_index;
    idbtype *swidb;
    srb_sb_t *srb_sb;
    ushort new_port_priority = SPAN_IBM_PORT_PRIORITY;
    ushort new_message_age_increment = MESSAGE_AGE_INCREMENT;
    boolean ast_on = FALSE;

    /*
     * Get index and make sure that it is in range.
     * Do not allow index to be changed to other than zero.
     * If there is no index number, some one wants to run static spanning
     * tree instead of AST. Grant the wish.
     */

    swidb = idb->firstsw;
    if (!csb->sense) {
	if ((swidb->span_ptr) &&
	    (swidb->span_ptr->protocol == SPAN_PROTO_IBM)) {
	    ast_on = TRUE;
	    index = get_srb_span_index(0);
        } else {
            index = GETOBJ(int, 1);
            if (index) {
                printf("\n%%Bridge group %d was not running on this interface.\n
",
                       index);
                return;
            }
        }
    }

    if (!GETOBJ(int, 2) && (csb->sense || !ast_on)) {
        srb_sb = srb_get_or_create_sb_inline(idb->firstsw);
        if (!srb_sb) {
            printf(nomemory);
            return;
        }
	if (!csb->sense) {
	    srb_sb_disable_spanning_explorer(idb->firstsw);
	} else {
	    if (ast_on) {
		printf("\n%%This interface is running AST.");
		return;
	    } else
	        srb_sb_enable_spanning_explorer(idb->firstsw);
	}	
	srbcore_modify_srb(idb, TR_MODIFY_SPAN, csb->sense);
	reg_invoke_lbs_srte_bcast(idb);
        return;
    }

    if (csb->sense) {
	index = GETOBJ(int, 1);
	if ((index < 1) || (index >= SPAN_MAXINDEX)) {
	    printf(badspan);
	    return;
	}
	if (swidb->span_index && index && (index != swidb->span_index)) {
	    printf(badspan);
	    return;
	}
	/* 
	 * Do I have another AST running? 
	 * If I do, use that index rather than accepting this new index.
	 */
	old_index = get_srb_span_index(0);
	if ((old_index < SPAN_MAXINDEX) && (old_index != index)) {
	    index = old_index;
	    printf("\n%%IBM protocol already configured. Bridge group %d will be used.", 
		   old_index); 
	}
    } 
   
    /* 'source-bridge spanning' command is only used for SRB AST */
    span = span_lookup(index, FALSE, SPAN_PROTO_IBM);
    if (!span) {
        printf(nomemory);
        return;
    }

    if (span->protocol != SPAN_PROTO_IBM) {
	printf("\n%%Bridge %d is not running AST.", index);
	return;
    }

    /* '[no] source-bridge spanning 1 path-cost <cost>' only affects
     *  port path cost.
     *  '[no] source-bridge spanning 1 priority <priority>' only affects
     *  port priority.
     * '[no] source-bridge spanning 1' adds port to or deletes port from
     *  spanning tree.
     */

    if ((GETOBJ(int, 4)) != -1) {
	if (!csb->sense)
	    swidb->path_cost = SCALED_BANDWIDTH(swidb->visible_bandwidth) /
		span->path_divisor;
	else 
	    swidb->path_cost = GETOBJ(int, 4);
	if (span->running)
	    span_path_cost(span);
	swidb->path_cost_explicit = csb->sense;
    }
    else if ((GETOBJ(int, 6)) != -1) {
        if (csb->sense) {
	    new_port_priority = GETOBJ(int,6);
	}
	if (span->running) {
	    ibm_span_port_priority(span, swidb, new_port_priority);
	}
    }
    /*
     * Added to support hidden message-age-increment option in
     * "source-bridge spanning X" interface command and permit
     * message-age count manipulation in mixed AST environments
     * where bridge vendors do not support MAX_AGE configuration.
     */
    else if ((GETOBJ(int, 8)) != -1) {
	if (csb->sense) {
	      new_message_age_increment = GETOBJ(int,8);
	}
	swidb->ibm_message_age_increment = new_message_age_increment;
    }	
    else if (GETOBJ(int, 2) || ast_on) {
        if (!csb->sense)
	    srb_span_disable(swidb, span);
        else 
	    srb_span_enable(swidb, span, index);
	/* Let LNM know about our spanning tree state */
	reg_invoke_lbs_srte_bcast(idb);
    }
}

/* 
 * srb_span_enable()
 * This routine enables the STP on token ring interface and any virtual
 * ring group that token ring interface is associated with.
 */
static void srb_span_enable (idbtype *swidb, spantype *span, int index)
{
    tr_vgrouptype *vrg;
    hwidbtype *hwidb, *span_hwidb;
    srb_sb_t *srb_sb;

    hwidb = hwidb_or_null(swidb);
    srb_sb = srb_get_sb_inline(swidb);
    if (!span || !swidb || !swidb || !srb_sb) {
        if (srb_debug || srbevent_debug)
            buginf("\nSRB%d: No span/idb structure.", hwidb->unit);
	return;
    }

    /* Add this interface to the bridge group */
    if (swidb->span_index != index) {
        swidb->span_index = index;
        /* Mess that IBM created */
        swidb->port_id =
            ((srb_sb->srb_thisring << 4) & 0xfff0) | srb_sb->srb_bridge_num;
	swidb->ibm_port_priority = SPAN_IBM_PORT_PRIORITY;
	swidb->ibm_message_age_increment = MESSAGE_AGE_INCREMENT;
        swidb->path_cost = SCALED_BANDWIDTH(swidb->visible_bandwidth) /
	    span->path_divisor;
        if (swidb->path_cost == 0)
            swidb->path_cost = SPAN_IBM_DEFAULT_PATHCOST;

        /*
         * See if there is any virtual interface
         * that needs to run spanning tree.
         */
	vrg = vrfind_ringgroup(srb_sb->srb_targetring);
        if (vrg) {
	     /* initialize span info for this virtual interface  */
            if (vrg->span_idb->span_index != index) {
                vrg->span_idb->span_index = index;
                vrg->span_idb->port_id =
                    ((vrg->vrn << 4) & 0xfff0) | srb_sb->srb_bridge_num;
		vrg->span_idb->ibm_port_priority = SPAN_IBM_PORT_PRIORITY;
		vrg->span_idb->ibm_message_age_increment =
		    MESSAGE_AGE_INCREMENT;
                vrg->span_idb->path_cost = 
		    SCALED_BANDWIDTH(vrg->span_idb->visible_bandwidth) /
			span->path_divisor;

                /* add this virtual interface to span structure */
		span_hwidb = hwidb_or_null(vrg->span_idb);
		if (!span_hwidb) {
		    buginf("\nNULL span_hwidb");
		    return;
		}
                span_fixup(index, span_hwidb, swidb);
                rsrb_enable_virtual_span(vrg->span_idb);
            }
        }
	span_fixup(index, hwidb, swidb);
	if (span->running)
	    span_port_enable(span, swidb);
    }
}

/*
 * srb_span_disable()
 * This routine disables the STP on token ring interface.
 */
static void srb_span_disable (idbtype *idb, spantype *span)
{
    hwidbtype *hwidb;
    tr_vgrouptype *vrg;

    if (!idb) {
       return;
    }

    if (!span) {
        if (srb_debug || srbevent_debug)
            buginf("\nSRB: AST is not running in this bridge.");
        return;
    }
    if (!idb->span_ptr) {
	hwidb = hwidb_or_null(idb);
	if (!hwidb) {
	    buginf("\nNULL hwidb");
	    return;
	}
	if (srb_debug || srbevent_debug)
	    buginf("\nSRB%d: AST is not running in this port.", hwidb->unit);
	return;
    }

    /*
     * Remove this interface from the bridge group.
     */
    if (span->running)
        span_port_disable(span, idb);
    span_delete_port(idb);

    /* see if target ring is a virtual ring */

    vrg = vrfind_ringgroup(srb_sb_get_targetring(idb));
    if (!vrg)
	return;

    /* is vrg in use? */

    FOR_ALL_HWIDBS(hwidb) {
	if (is_srb_hw(hwidb) && (srb_sb_get_targetring(hwidb->firstsw) ==
					 vrg->vrn) && hwidb->firstsw->span_ptr)
	    return;
    }	

    if (span->running)
	span_port_disable(span, vrg->span_idb);
    span_delete_port(vrg->span_idb);
}

/*
 * lnm_srb_span_command()
 * This routine handles setting of Spanning Tree Enable mode through
 * LNM. If the passed idb is configured with local SRB without virtual ring
 * i.e. idb->tr_bridge_idb is not NULL, then enable/disable ST on that interface
 * as well. This is necessary when AST is configured thru LNM because setting
 * Automatic Spanning Tree is bridge wide operation with LNM, not like enabling
 * disabling forwarding of STEs, which is done per port base. 
 * Remember that  idb->tr_vring_blk are mutually exclusive.
 * This routine assumes that user has already entered 'bridge n protocol ast'
 * command through a console.
 */
boolean lnm_srb_span_command (
    hwidbtype *bidb1,
    hwidbtype *bidb2,
    boolean span_on)
{
    int index;
    hwidbtype *idb;
    idbtype *swidb;
    spantype *span;

    index = get_srb_span_index(0);
    if (index >= SPAN_MAXINDEX) {
	if (srb_debug || srbevent_debug)
	    buginf("\nSRB(span): bridge protocol not configured");
	return(FALSE);
    }
    span = spanarray[index];
    if (!span) {
	if (srb_debug || srbevent_debug)
	    buginf("\nSRB(span): No span structure for bridge group %d",
		index);
	return(FALSE);
    }
    /* Use the physical interface */
    idb = (bidb1->status & IDB_VIRTUAL) ? bidb2 : bidb1;
    swidb = idb->firstsw;
    if (span_on) {
	srb_span_enable(swidb, span, index);	
	if (idb->tr_bridge_idb) 
	    srb_span_enable(idb->tr_bridge_idb->firstsw, span, index);
    } else {
        srb_span_disable(swidb, span);
	if (idb->tr_bridge_idb)
	    srb_span_disable(idb->tr_bridge_idb->firstsw, span);
    }
    return(TRUE);
}

/*
 * get_srb_span_index(int)
 * This routine finds an index to a SRB span structure.
 * returns SPAN_MAXINDEX if not found.
 */
int get_srb_span_index(int index)
{
    int i;

    for (i = 0; i < SPAN_MAXINDEX; i++) {
	if ((spanarray[i]) && (spanarray[i]->protocol == SPAN_PROTO_IBM) &&
	    (i != index))
	    break;
    } 
    return(i);
}

/*
 * Hack glue code to stick vidb's and dlsw together....
 *
 * DLSw likes to number things as 'ports', but because of the
 * dlsw_glob_ring_access[] it is a "Bad Thing(tm)" to have large
 * numbers.  The current implementation is limited to NUM_OF_RINGS (64
 * as of 12/94).  The code thinks of 'ports' as hwidb->unit numbers.
 * For physical ports, these numbers are indeed small.
 *
 * RSRB and vidb handling predates DLSw, and it wanted to use
 * potentially large 'port' numbers.  It inserts a ring number into
 * the hwidb->unit number.  Changing this to be something other than a
 * ring number would require a very large effort.
 *
 * The compromise is this function which fools the DLSw code and keeps
 * current legacy code happy as well.  The legacy code used to keep
 * track of vidb 'unit' numbers too, but only for a few short
 * microseconds, before it overwrote the value with a ring number.
 * Now, save off vidb_unit number into another little used vidb field
 * (hwidb->hw_if_number), which will be relatively small, as it only
 * counts vidbs.
 */

inline static int dlsw_get_unit_num(hwidbtype *idb)
{
    if (idb->status == (IDB_VIRTUAL | IDB_TR))
	return(idb->hw_if_index);
    else
	return(idb->unit);
}    

/*
 * demux()
 * This routine will forward the incoming packet to the appropriate
 * module base on predefined criteria.
 *
 * D_DLSW  - Final ring terminating at the router.
 *
 * D_RSRB  - else return RSRB.
 *
 * DLSW_ERORR - in case of error.
 */
 
int dlsw_demux (paktype *pak, hwidbtype *idb)
{
    trrif_t *trh;

    trh = (trrif_t *) pak->datagramstart;

    if (idb != NULL) {
        if (is_fddi(idb)) {  /* already in token ring format */
            pak->enc_flags |= ENC_DLSW;
            tring_parse_packet(idb, pak, NULL);
        }
        else
            (*idb->parse_packet)(idb, pak, NULL);
    }
    else
	return(DLSW_ERROR);

    /* After packet parsing info_start usually points at the SAP.
     *
     * prepare for (*idb->getlink)(pak).
     *
     * We need some care here to handle ET_LLC2.  idb->getlink uses
     * the llc2_sap, rather than dsap - who knows why ???
     *
     * Make sure that the llc2_sap is set up correctly
     */

    if (pak->enctype == ET_LLC2) {
        if (pak->info_start != NULL) {
            pak->llc2_sap = *(pak->info_start);
        }
    }
    pak->linktype = (*idb->getlink)(pak);
    if (pak->linktype == LINK_IBMNM)
        return(D_LNM);

    /*
     *  Terminated at the router.
     */
    if((RIF_DEST_RING(trh) == srb_sb_get_targetring(idb->firstsw)) &&
       !idb->tr_bridge_idb) {

        /*
	 * If DLSW is running then return D_DLSW. If not,
	 * then this is a frame terminated at the router which has
	 * not been classified as LINK_IBMNM (see above.)
	 * Classify it as DLSW_ERROR so that it will be dropped and
	 * not cause an input queue wedge (see CSCdi43561)
	 */
	
	if (reg_invoke_dlsw_running(0,0,0)) 
            return(D_DLSW);
	return(DLSW_ERROR);
    }
    
    return(D_RSRB);
}


/*
 * Activate ring.
 * This routine will activate the ring. This will behave similar to the
 * case statement that can be found in the legacy code.
 */

void activate_ring (hwidbtype *idb, ushort target_ring, 
		   ushort local_ring, ushort bridge_num) 
{
    srb_sb_t *srb_sb;

    if (target_ring == local_ring) {
	buginf("\n%%local ring and target ring numbers must be different!");
	return;
    }
 
    if (vrfind_ringgroup(local_ring)) {
	buginf("\n%%local ring can not be a ring group");
	return;
    }

    srb_sb = srb_get_or_create_sb_inline(idb->firstsw);
    if (!srb_sb) {
        printf(nomemory);
        return;
    }

    remove_vring_interface(idb, srb_sb->srb_targetring);
    srb_sb->srb_bridge_num = bridge_num;
    srb_sb->srb_thisring = local_ring;
    srb_sb->srb_targetring = target_ring;
    set_srb_interfaces();
    set_vring_interfaces();
    fs_srb_setup();
    fs_explorer_setup();
    reg_invoke_srt_setup_auto_sw_srb(idb);
    reg_invoke_set_ibmnm_interfaces(idb, TRUE);
    start_srb_background();
    srbcore_setup_srb(idb);
}


/*
 * DeActivate ring.
 * This routine will activate the ring. This will behave similar to the
 * case statement that can be found in the legacy code.
 */

void deactivate_ring(hwidbtype *idb, ushort target_ring, 
		     ushort local_ring, ushort bridge_num)
{
    srb_sb_t *srb_sb;

    srb_sb = srb_get_or_create_sb_inline(idb->firstsw);
    if (!srb_sb) {
        printf(nomemory);
        return;
    }
    target_ring = srb_sb->srb_targetring; /* remember if part of vring */
    srb_sb->srb_thisring    = SRB_INVALID_RING_NUM;
    srb_sb->srb_bridge_num  = SRB_INVALID_BRIDGE_NUM;
    srb_sb->srb_targetring  = SRB_INVALID_RING_NUM;
    srb_sb->srb_max_hops    = SRB_DEF_MAXHOPS;
    srb_sb->srb_maxout_hops = SRB_DEF_MAXHOPS;
    srb_sb->srb_maxin_hops  = SRB_DEF_MAXHOPS;
    srb_sb->srb_spanning_explorer = FALSE;
    set_srb_interfaces();
    remove_vring_interface(idb, target_ring);
    set_vring_interfaces();
    fs_srb_setup();
    fs_explorer_setup();
    reg_invoke_srt_setup_auto_sw_srb(idb);
    reg_invoke_set_ibmnm_interfaces(idb, FALSE);
    srbcore_setup_srb(idb);
}

/*
 * srb_input()
 *
 * srb_input() is a front-in routine to srb_common()
 *   invoked via reg_invoke_raw_enqueue() when (pak->linktype == LINK_SRB)
 *
 *   (only CSNA LAN uses the LINK_SRB linktype at this time)
 *
 */
static void srb_input(paktype *pak)
{
  srb_common(pak, pak->if_input->hwptr, pak->riflen);
}
