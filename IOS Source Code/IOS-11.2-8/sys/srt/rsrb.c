/* $Id: rsrb.c,v 3.24.4.26 1996/09/04 16:36:38 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/rsrb.c,v $
 *------------------------------------------------------------------
 * rsrb.c -- Token Ring Remote and Mulitport Source Route Bridge Support
 *
 * September 11 1989, Eric B. Decker
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsrb.c,v $
 * Revision 3.24.4.26  1996/09/04  16:36:38  ppearce
 * CSCdi67879:  75k Crashes with %ALIGN-1-FATAL after about 30 RSRB tests
 * Branch: California_branch
 *   malloc_vre() needs to check that malloc is successful before vre init
 *
 * Revision 3.24.4.25  1996/08/28  13:16:06  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.24.4.24  1996/08/26  15:12:26  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.24.4.23  1996/08/17  21:44:04  ppearce
 * CSCdi66186:  75k crashes w/ALIGN-1-FATAL after multiple RSRB tests
 * Branch: California_branch
 *   - Check malloc return before initializing SRB subblock
 *   - Free SRB subblock when vRing unconfigured to prevent memory leak
 *
 * Revision 3.24.4.22  1996/08/07  09:02:34  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.24.4.21  1996/08/03  23:37:40  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.24.4.20  1996/07/31  17:28:35  gcox
 * CSCdi64717:  Remove doubly-defined macros - SCALED_BANDWIDTH etc
 * Branch: California_branch
 *
 * Revision 3.24.4.19  1996/07/31  17:19:36  ppearce
 * CSCdi62053:  Router reloads with bus error after adding RSRB remote peer
 * Branch: California_branch
 *   Prevent crash in access_check_type() by catching out-of-range value
 *
 * Revision 3.24.4.18  1996/07/25  23:54:14  motto
 * CSCdi46834:  SRB explorer counters are incr. on CIP I/F for non CIP
 * destined expl
 * Branch: California_branch
 *
 * Revision 3.24.4.17  1996/07/23  18:49:03  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.24.4.16  1996/07/23  13:29:18  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.24.4.15  1996/07/18  20:29:32  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.24.4.14  1996/07/18  04:26:12  ppearce
 * CSCdi56398:  Router (sdlc-secondary) fails to respond to SNRM inputs
 * Branch: California_branch
 *   Ensure SDLLC process is re-started when SDLLC is configured
 *
 * Revision 3.24.4.13  1996/07/13  11:40:05  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.24.4.12  1996/07/09  06:05:32  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.24.4.11  1996/07/02  23:11:35  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.24.4.10  1996/06/28  23:26:20  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.24.4.9  1996/06/27  09:40:35  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.24.4.8  1996/06/18  01:48:51  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.24.4.7  1996/06/05  06:09:27  ioakley
 * CSCdi55919:  Remove vrconnect1 dequeue loop to eliminate possible
 *              bus error crash when deconfiguring rsrb peers.
 * Branch: California_branch
 *
 * Revision 3.24.4.6  1996/05/22  17:06:53  ppearce
 * CSCdi58099:  SRB fastswitching performance
 * Branch: California_branch
 *   Use direct accesses to SRB subblock fields rather than accessors
 *
 * Revision 3.24.4.5  1996/05/17  12:13:33  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.19.2.17  1996/05/06  00:01:23  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.19.2.16  1996/05/02  19:48:22  psandova
 * CSCdi54739:  small buffers wedge, memory leak
 * Branch: IbuMod_Calif_branch
 *
 * Return pak if system has run out of memory for tcp vrc header block.
 *
 * Revision 3.19.2.15  1996/04/26  15:23:16  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.14  1996/04/26  07:58:32  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.19.2.13  1996/04/25  19:31:26  ioakley
 * CSCdi53651:  Add support for hidden message-age-increment option in
 *              "source-bridge spanning X" interface command.  Permits
 *              AST interoperability in mixed vendor bridge environments
 *              where hard-coded MAX_AGE defaults are insufficient for
 *              the network diameter and MAX_AGE is not configurable.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.12  1996/04/24  06:30:41  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.19.2.11  1996/04/22  19:58:50  asethi
 * CSCdi50966:  rsrb caused local timeout
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.10  1996/04/09  16:50:00  schiang
 * CSCdi54116:  vr_purge_remote_rings() is not called
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.9  1996/04/07  05:57:25  schiang
 * CSCdi50513:  RSRB - Dead NCIA peers cause duplicate ring entries
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.8  1996/04/05  06:07:32  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.19.2.7  1996/04/05  01:06:59  ioakley
 * CSCdi53594:  Ensure vrp->connector_owns state is set
 *              correctly in vring_background function.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.6  1996/04/03  21:59:11  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.19.2.5  1996/03/20  17:55:03  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comment - name changes only
 *
 * Revision 3.19.2.4  1996/03/17  18:49:48  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.19.2.3  1996/03/16  12:10:00  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.19.2.2  1996/03/01  16:31:34  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.19.2.1  1996/02/28  21:24:47  ppearce
 * CSCdi50230:  IBU modularity - Remove unnecessary header file includes
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.24.4.4  1996/05/09  14:46:37  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.24.4.3.4.1  1996/04/27  07:26:18  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.24.4.3  1996/04/17  13:48:58  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.24.4.2.6.1  1996/04/08  14:49:20  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.24.4.2  1996/03/23  22:49:15  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.24.4.1  1996/03/18  22:10:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.14.2.5  1996/03/16  07:38:07  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.14.2.4  1996/03/13  02:02:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.14.2.3  1996/03/07  10:50:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.14.2.2  1996/02/20  18:48:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.24  1996/03/11  17:24:46  ppearce
 * CSCdi47301:  CLS/RSRB link activation may fail when LAK enabled
 *   Cannot depend on pak->peer_ptr to represent a valid peer entry
 *     use a different method to find valid peer entry
 *
 * Revision 3.23  1996/03/11  14:06:03  rbatz
 * CSCdi45116:  Duplicate All-Routes Explorers unnecessarily
 *              forwarded in redundant topologies.
 *
 * Revision 3.22  1996/03/04  20:15:16  ttsang
 * CSCdi50509:  RSRB explorer not forwarded to token-ring
 *
 * Revision 3.21  1996/03/04  07:14:30  kchiu
 * CSCdi49101:  make netbios name recognized filtering optional
 *
 * Revision 3.20  1996/02/29  00:19:52  kchiu
 * CSCdi46631:  lanmgr reporting erroneous Non-Broadcast frame counters
 *
 * Revision 3.19  1996/02/21  02:43:48  hampton
 * Clean up platform specific SRB defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.18  1996/02/20  17:50:44  ioakley
 * CSCdi40888: Prevent potential peer startq related buffer/memory leaks
 *             and establish a default limit for the peer startq size.
 *
 * Revision 3.17  1996/02/13  22:12:24  fred
 * CSCdi47956:  WFQ severely degrades RSRB performance
 *              move fair queue discriminator from srb.c to rsrb.c
 *              make sure pointers are st up for queueing routine
 *
 * Revision 3.16  1996/02/13  08:16:22  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.15  1996/02/08  18:02:06  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.14.2.1  1996/01/24  23:27:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.14  1996/01/22  07:16:30  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.13  1996/01/17  04:55:25  kchiu
 * CSCdi18993:  Crash in rsrb_netbios
 *
 * Revision 3.12  1996/01/16  03:37:44  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.11.4.1  1996/02/26  20:28:05  ddecapit
 * Branch: TRlane_branch
 *
 * Revision 3.11  1995/12/30  00:29:14  hou
 * CSCdi37413:  RTBR bridging fails for Ether->Token->Ether on vines and
 * ip.
 * - Set the default OUI for SNAP on token ring interface as 0x000000
 *   instead of 0x0000f8.
 * - Make low-end "ethernet-transit-oui" fully functional.
 *
 * Revision 3.10  1995/12/17  15:20:14  motto
 * CSCdi36169:  misaligned accesses
 *
 * Revision 3.9  1995/12/15  22:43:17  rbatz
 * CSCdi43378:  Explorers destined for SDLLC Virtual Ring forwarded to
 *              wrong ring.  Rework of vr_virtualp required.
 *
 * Revision 3.8  1995/12/13  04:24:06  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.7  1995/12/12  06:15:48  monical
 * CSCdi45017:  config command no source-br largest-fr gives a warning
 *
 * Revision 3.6  1995/12/11  10:20:31  ppearce
 * CSCdi44095:  SRB proxy-explorer causes FDDI packet drops
 *   rif_explore() should call tr_to_media_out() rather than datagram_out()
 *     so that packets can be correctly formatted for output to FDDI
 *
 *     - Rename srb_out() to tr_media_out()
 *     - Change srb_out() to invoke tr_fddi_out() for FDDI output
 *     - Move adjust_pak() code into tr_fddi_out()
 *
 * Revision 3.5  1995/11/30  02:59:32  syiu
 * CSCdi44150:  Remote Node NetBEUI on Token Ring platform not working
 *
 * Revision 3.4  1995/11/20  18:37:25  rbadri
 * CSCdi38662:  Configuring bridging on multiple ATM sub-interfaces causes
 *              hardware reset
 * Make span_fixup() recognize sub-interfaces, so that for ATM interfaces,
 * we can initiate a config change rather than a hardware reset.
 *
 * Revision 3.3  1995/11/17  18:55:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:42:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:31:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.23  1995/11/08  21:30:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.22  1995/10/13  01:00:36  ioakley
 * CSCdi35149:  Eliminate extraneous generation of AST config_bpdus
 *              within an RSRB disjoint peer context.
 *
 * Revision 2.21  1995/10/10  06:53:34  richl
 * CSCdi41537:  25k and 4k routers crash in fst/hdlc/fast rsrb test
 *
 * Revision 2.20  1995/09/25  08:35:42  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.19  1995/09/14  01:04:37  vrawat
 * CSCdi38633:  Rsrb cfg cmd broke for direct encap over frame-relay
 *
 * Revision 2.18  1995/09/07  03:12:57  motto
 * CSCdi39129:  Make the source-bridge config cmd on CIP ILANs work like
 * TRIP/FIP
 *
 * Revision 2.17  1995/09/02  22:28:18  alam
 * CSCdi39293:  SRB TR<->FDDI not forwarding Netbios UA frame
 * Fix explorer processing for SRB/FDDI.
 *
 * Revision 2.16  1995/08/25  11:45:17  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.15  1995/08/21 18:24:44  jacobt
 * CSCdi39059:  debug source bridge shows a lot of bogus peer messages
 *
 * Revision 2.14  1995/08/21  14:10:56  richl
 * CSCdi39040:  RSRB wrong state to keep debug level causes confusion
 *
 * Revision 2.13  1995/08/07  05:43:58  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.12  1995/07/20  23:09:10  jacobt
 * CSCdi34242:  FST Peers stuck in opening
 *
 * Revision 2.11  1995/07/19  21:06:41  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add services to reducce dependency on idb.
 * Install 'qllc dlsw' command.
 * Build QLLC Activate/Deactivate Ring CLSI message.
 * Consolidate qllc fields from swidb to a single data structure (subblock)
 *
 * Revision 2.10  1995/07/13  20:38:04  jacobt
 * CSCdi36703:  replace malloc() by malloc_named()
 *
 * Revision 2.9  1995/07/06  18:42:24  ccartee
 * CSCdi36735:  DLSw/EN 80d5 support completely broken
 * Isolate feature 'enable' check and packet transformations into seperate
 * functions.  Use these recycled functions for adding 80d5 functionality
 * to DLSw code.  Duplicate functionality in legacy code is removed and
 * use recycled code instead...
 *
 * Revision 2.8  1995/06/30  00:10:22  jacobt
 * CSCdi36326:  Router crashes with exception vector 2
 *
 * Revision 2.7  1995/06/22  22:28:45  rnaderi
 * CSCdi32752:  Dead RSRB Peers Cause Memory Leak
 *
 * Revision 2.6  1995/06/21  03:29:38  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.5  1995/06/20  07:19:07  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/17  06:26:18  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.3  1995/06/09  13:21:27  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.2  1995/06/08 22:52:28  ioakley
 * CVS:
 *
 * CSCdi34936:  Add a virtual interface to the spanning tree
 *              only if an associated port is enabled for AST.
 *
 * Revision 2.1  1995/06/07  23:01:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "fastswitch_registry.h"
#include "interface_private.h" /* -> _generic when subblock work done */
#include "sys_registry.h"
#include "srt_registry.h"
#include "packet.h"
#include "logger.h"
#include "../srt/msg_rsrb.c"		/* Not a typo, see logger.h */
#include "media_registry.h"
#include "../ip/ip_registry.h"
#include "srt_registry.h"
#include "../cls/dlc_registry.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "config.h"
#include "../if/ether.h"
#include "../wan/serial.h"
#include "../tbridge/tbridge_sde.h"
#include "../wan/frame_relay.h"
#include "../if/tring.h"
#include "../srt/trsrb.h"
#include "../if/tring_debug.h"
#include "../if/tring_common.h"
#include "../if/rif_inline.h"
#include "../if/tring_inline.h"
#include "rsrb.h"
#include "../if/rif.h"
#include "../ibm/netbios.h"
#include "../ibm/netbios_sb.h"
#include "../if/rif_util.h"
#include "../ibm/libibm_externs.h"
#include "../if/network.h"
#include "../if/arp.h"
#include "../srt/srb.h"
#include "../srt/srb_multiring.h"
#include "../srt/srb_sb_private.h"
#include "../tcp/tcpdriver.h"
#include "../if/if_vidb.h"
#include "../llc2/llc2.h"
#include "../llc2/llc2_sb.h"
#include "../ibm/lanmgr.h"
#include "../ibm/ibmnm.h"
#include "../ibm/lanmgr_registry.regh"
#include "../ibm/sna_util.h"
#include "../srt/lnx.h"
#include "span.h"
#include "access.h"
#include "srt_debug.h"
#include "../ibm/sna_la_pri.h"
#include "../if/fddi.h"
#include "../ip/ipfast.h"
#include "../srt/lack_externs.h"
#include "../srt/lack2fsm.h"
#include "../srt/lack2_externs.h"
#include "../if/priority_private.h"  /* ->priority.h when ->interface_generic */
#include "../hes/channel_sb.h"

/*
 * Externs
 */

extern boolean enable_80d5;
extern boolean sap_8209_table[], default_sap_8209_table[];
extern fs_explorer_cb fs_explorer;

/*
 * Global Storage
 */

static const char rsrb_cb[] = "RSRB CB";
queuetype vringQ;
queuetype vrconnQ;
watched_queue *vrconn_pakQ;
watched_queue *vrxchgQ;
watched_boolean *vr_wake_up;
int vrconn_pid;
ulong vrconn_timeout;			/* how long to wait for conn estab */
sys_timestamp vrconn_wait;		/* delay timer for connector */
ulong vring_tcpqmax;		        /* Maximum TCP queue length */
ulong rsrb_keepalive_interval;		/* how long to wait before timing out a peer */
sys_timestamp rsrb_next_keepalive;	/* when to wake up again for keep alive purposes */
ipaddrtype rsrb_fst_name;	        /* this peer's name */
char rif_string[SRB_MAXRIF*2+SRB_MAXRIF/2];
static uchar rsrb_broadcast_rif[] = { RC_DEF_SPAN, RC_LF1500 };

/*********************************************************************
 *
 * Low level routines.
 *
 *********************************************************************/

/*
 *
 */
tr_ventrytype *malloc_vre (const char *name)
{
    tr_ventrytype *vre;
    vre = malloc_named(sizeof(tr_ventrytype), name ? name : "VRE");
    if (vre) {
        vre->vre_les_fastout = (vre_les_fastout_type)vre_fastout_err;
        vre->vre_hes_fastout = (vre_hes_fastout_type)vre_fastout_err;
    }
    return(vre);
}

/*
 * rsrb_reset_tcp_encaps()
 *   Search through all vrp's looking for tcp_encaps ptr (t)
 *     and reset vrp->t to NULL
 *
 *   The tcp_encaps ptr has been freed and any vrp references
 *     need to be cleaned up
 *
 */

static inline void rsrb_tcpdriver_cleanup (tcp_encaps *t)
{
    tr_vgrouptype *vrg;
    tr_vpeertype  *vrp;
    for (vrg = (tr_vgrouptype *)vringQ.qhead; vrg; vrg = vrg->next) {
        for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
	    if (vrp->t == t) {
	        vrp->t = NULL;
	    }
        }
    }
}

/*
 * rsrb_tcpdriver_abort()
 *
 *   Front-end to invoke tcpdriver_abort() and cleanup tcp_encaps in vrp's
 *
 */

static void rsrb_tcpdriver_abort (tcp_encaps *t)
{
    if (!t) {
       return;
    }
    reg_invoke_ip_tcpdriver_abort(t);
    rsrb_tcpdriver_cleanup(t);
}

/*
 * rsrb_tcpdriver_close()
 *
 *   Front-end to invoke tcpdriver_close() and cleanup tcp_encaps in vrp's
 *
 *
 */

static void rsrb_tcpdriver_close (tcp_encaps *t)
{
    if (!t) {
       return;
    }
    reg_invoke_ip_tcpdriver_close(t);
    rsrb_tcpdriver_cleanup(t);
}

/*
 * vring_init
 * initilize structures for remote source route bridge
 */

void vring_init (void)
{
    queue_init(&vringQ, 0);
    queue_init(&vrconnQ, 0);
    vrconn_pakQ = create_watched_queue("vrconn_pak", 0, 0);
    vrxchgQ = create_watched_queue("vrxchg", 0, 0);
    vr_wake_up = create_watched_boolean("VR WakeUp", 0);
    vrconn_pid = 0;
    vrconn_timeout = RSRB_REMOPEN_DEFTMO;
    vring_tcpqmax = RSRB_TCP_MAX_QUEUE_DEFAULT;
    rsrb_keepalive_interval = RSRB_KEEPALIVE_DEFAULT_INTERVAL;
    TIMER_START(rsrb_next_keepalive, RSRB_KEEPALIVE_TX_PERIOD);
    lack_hash_freelist = NULL;
    lack_running = FALSE;
    lack_connect_timeout = RSRB_LACK_CONNECTION_TIMEOUT_DEFAULT;
    sdllc_pid = 0;
    lnx_running = FALSE;
}

/*
 * Given a peer pointer, delete its remote ring entries.
 * Called when a peer goes down or is deleted.
 */
static void vr_purge_remote_rings(tr_vpeertype *vrp) {
 
    tr_ventrytype *vre, *vre_next;
    leveltype status;
    if (!vrp)
        return;

    status = raise_interrupt_level(ALL_DISABLE);
    for (vre = (tr_ventrytype *) vrp->vrg->rings.qhead; vre; ) {
        vre_next = vre->next;
 
        /*
         * outhole is a vrp for remote rings, and an idb for
         * local rings.  No local rings removed here.
         * For the vre's that are removed here,
         * vring background frees these vre's at process lvl.
         */
        if (vre->outhole.vp == vrp) {
            if (vre->type & (RSRB_ET_REMOTE | RSRB_ET_REMSDLLC |
			     RSRB_ET_REMQLLC)) {
		vre->outhole.vp = NULL;
                unqueue(&vrp->vrg->rings, vre);
                enqueue(&vrp->vrg->dead_rings, vre);
                if (srb_debug)
                    buginf("\nRSRB: put ring %d to dead ring queue", vre->rn);
            }
            else
                errmsg(&msgsym(BADVRE, RSRB));
        }
        vre = vre_next;
 
    }
    reset_interrupt_level(status);
    
}

/*
 * rsrb_newstate
 * Set a peer to a new state.  Fascinating isn't it.
 */

void rsrb_newstate (
    tr_vpeertype *vrp,
    tr_vpeer_statetype newstate)
{
    char peerid[MAX_PEER_NAME];

    SAVE_CALLER();

    if (srbevent_debug)
	buginf("\nRSRB: peer %s %s -> %s (%x)", vrpeer_id(vrp, peerid),
	       vrpeer_state(vrp->state), vrpeer_state(newstate), caller());
    /*
     * If we are currently connected and go into any other state toss any lacks,
     * toss the rif cache, and toss the netbios cache.
     *
     * if we are not connected and go into closed or dead also toss connections.
     * This insures that any cross pointers are cleaned up.
     */
    if (vrp->state == RSRB_PS_CONNECTED) {
        if (newstate != RSRB_PS_CONNECTED) {
	    if (vrp->local_ack)
	        lack_toss_connections(vrp);
	    rif_ager(TRUE, TRUE);
	    netbios_name_cache_ager(TRUE, TRUE);
	    /* SRB AST support */
     	    /*
      	     * See if I need to diable spanning tree on the virtual interface
      	     * If there are less than two open peers for this ring group, 
 	     * AST will be disabled for this virtual interface.
      	     */
 	    vrp->state = newstate;
     	    rsrb_disable_virtual_span(vrp->vrg->span_idb);
            vr_purge_remote_rings(vrp);
	}
    } else {
        switch (newstate) {
          case RSRB_PS_DEAD:
            if (CLOCK_IN_STARTUP_INTERVAL(RSRB_INITIAL_START_WINDOW))
                newstate = RSRB_PS_CLOSED;

            /* fall through */

          case RSRB_PS_CLOSED:
            if (vrp->local_ack)
                lack_toss_connections(vrp);
            vrp->vrp_starting_up = TRUE;
            break;

	  case RSRB_PS_CONNECTED:
	    /* SRB AST support */
	    /* 
	     * See if I need to enable spanning tree on the virtual interface
	     * If there still are more than two open peers for this ring group,
	     * AST will be enabled for this virtual interface.
	     */	
	    vrp->state = newstate;    
	    rsrb_enable_virtual_span(vrp->vrg->span_idb);
	    break;

          default:
            break;
        }
    }
    /* Reset the ip packet counter on state changes */
    RSRB_FST_RESET_SEQUENCE_NUMBERS(vrp)
    vrp->state = newstate;
    GET_TIMESTAMP(vrp->last_keepalive);
    if (newstate == RSRB_PS_CLOSED) {
        vrp->vrp_version = RSRB_VERSION;
        vrp->inhdr.op = RSRB_OP_EMPTY;
    }
    /* send trap on remote peers */
    if ((((newstate == RSRB_PS_CLOSED) &&
          !CLOCK_IN_STARTUP_INTERVAL(RSRB_INITIAL_START_WINDOW)) ||
          newstate == RSRB_PS_DEAD ||
          newstate == RSRB_PS_CONNECTED) &&
        (vrp->ipaddr != vrtcp_peername(vrp->vrg)))
       reg_invoke_generateRSRBtrap(vrp);
}

/*
 * vrpak
 * get a packet for sending vring protocol information.
 */

paktype *vrpak (ushort version, ushort op, ushort trn,
		ushort vrn, ushort offset, ushort len)
{
    paktype *pak;
    tr_vhdrtype *vhdr;

    pak = getbuffer(sizeof(tr_vhdrtype) + len);
    if (!pak)
	return(NULL);
    vhdr = (tr_vhdrtype *)pak->network_start;
    vhdr->rhdr_version = version;
    vhdr->op = op;
    vhdr->trn = trn;
    vhdr->vrn = vrn;
    vhdr->offset = offset;
    vhdr->len = len;
    pak->datagramstart = (uchar *) vhdr;
    pak->datagramsize = sizeof(tr_vhdrtype) + len;
    return(pak);
}
    
/*
 * vrfind_ringgroup
 * given a virtual ring group, find its structure.
 */

tr_vgrouptype *vrfind_ringgroup (ushort vrn)
{
    tr_vgrouptype *vrg;
    
    for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next)
	if (vrg->vrn == vrn)
	    return(vrg);
    return(NULL);
}

/*
 * vrfind_rn2entry
 * given a ring number find its entry.  se is the start entry.  It
 * is a mempointer for use off the idb->tr_vring_blk as well as
 * internally.  It is assumed to be a pointer to a cell holding the
 * address of the first entry in the chain.  We don't point at the
 * first entry itself because this could change as rings are added and
 * deleted.
 */
tr_ventrytype *vrfind_rn2entry (ushort rn, ushort bn, tr_ventrytype *se,
				ushort kind)
{
    tr_ventrytype *vre;
    tr_vpeertype *vrp;

    if (se == NULL) {
	return(NULL);
    }
    for (vre = se->next; vre; vre = vre->next) {
	if (vre->rn != rn)
	    continue;
	if (bn != RSRB_BN_DONTCARE && vre->bn != bn)
	    continue;

	switch (kind) {
	  case RSRB_ET_EITHER:
	    if (vre->type &
		( RSRB_ET_LOCAL  |
		  RSRB_ET_LOCAL_VIRTUAL  |
		  RSRB_ET_SDLLC |
		  RSRB_ET_QLLC |
                  RSRB_ET_TR_LANE)) {
		    return(vre);
		}
	    /* else fall through to the next case: remote vrings */

	  case RSRB_ET_REMOTE:
	    if (vre->type &
		( RSRB_ET_REMOTE |
		  RSRB_ET_REMSTATIC |
		  RSRB_ET_REMSDLLC |
		  RSRB_ET_REMQLLC)) {

		vrp = vre->outhole.vp;
		/* vre->outhole.vp explicitly set to null in routine
                 * vr_purge_remote_rings()
                 * if the vrp (peer ) has been removed/deleted
                 */
                if(!vrp)
                    continue;

		/*
		 * Is it a "next hop" peer? If so, check its idb state. If
		 * not currently up, don't give this as a possibility. Also,
		 * it should be in a CONNECTED state. Let the explorers open
		 * the link. Don't give an entry unless it *is* connected.
		 */
		if (vrp->backup_group == RSRB_BACKUP_GROUP_NONE) {
		    return(vre);
		} else if ((vrp->type == RSRB_PT_LAN) ||
			   (vrp->type == RSRB_PT_IF)) {
 
		           if ((vrp->state == RSRB_PS_CONNECTED) &&
			       (hw_interface_up(vrp->outidb))) {
			       return(vre);
			   }
		       } else {
                             if (vrp->type == RSRB_PT_FR) { 
                                 if ((vrp->state == RSRB_PS_CONNECTED) &&
                                     (interface_up(vrp->outswidb))) {
                                    return(vre);
				 }
			     }
		             else { 
                              return(vre);
 			    }
			 }
	    }
            break;

	  case RSRB_ET_LOCAL:
	    if (vre->type &
		( RSRB_ET_LOCAL |
		  RSRB_ET_SDLLC |
		  RSRB_ET_QLLC |
                  RSRB_ET_TR_LANE |                  
		  RSRB_ET_LOCAL_VIRTUAL))
		return(vre);
	    break;

	  case RSRB_ET_REMSDLLC:
	  case RSRB_ET_REMQLLC:
	  case RSRB_ET_SDLLC:
          case RSRB_ET_QLLC:
          case RSRB_ET_TR_LANE:
          case RSRB_ET_LOCAL_VIRTUAL:
	  case RSRB_ET_REMSTATIC:
	    if (vre->type == kind)
	      return(vre);
	    break;

	}
    }
    return(NULL);
}

/*
 * vrvalid_peer
 * Given  peer entry find if it's a valid entry.
 */
static boolean vrvalid_peer(tr_vpeertype *vrp)
{
    tr_vgrouptype *vrg;

    for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
	if (rsrb_checkqueue(&vrg->peers, vrp)) {
	    return (TRUE);
	}
    }
    return (FALSE);
}
/*
 * vrfind_idb2entry
 * find the local entry if any that contains this idb
 */
tr_ventrytype *vrfind_idb2entry (hwidbtype *idb, tr_vgrouptype *vrg)
{
    tr_ventrytype *vre;

    if (!vrg)
	return(NULL);

    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next)
	if ((vre->type  & 
	  ( RSRB_ET_LOCAL | 
	    RSRB_ET_LOCAL_VIRTUAL | 
	    RSRB_ET_SDLLC | 
	    RSRB_ET_QLLC |
            RSRB_ET_TR_LANE)) &&
	    (vre->outhole.idb == idb)) 
	    return(vre);
    return(NULL);
}


/*
 * vrfind_peer
 * Given an foreign IP address and a vring find the corresponding
 * peer entry.
 */

tr_vpeertype *vrfind_peer (tr_vgrouptype *vrg, tr_vpeer_typetype type,
			   rsrb_cookie cookie, int lport, int fport,
			   uchar *macaddr)
{
    tr_vpeertype *vrp;

    if (!vrg)
	return(NULL);
    for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
	if (vrp->type != type)
	    continue;
	switch (type) {
	  case RSRB_PT_TCPD:
	    if (vrp->ipaddr == cookie.ip) {
		switch (lport) {
		  case 0:
		  case TR_RSRB_P0_PORT:
		    return(vrp);
		  case TR_RSRB_P1_PORT:
		    return(vrp->priority_peers[1]);
		  case TR_RSRB_P2_PORT:
		    return(vrp->priority_peers[2]);
		  case TR_RSRB_P3_PORT:
		    return(vrp->priority_peers[3]);
		  default:
		    switch (fport) {
		    case 0:
		    case TR_RSRB_P0_PORT:
			return(vrp);
		    case TR_RSRB_P1_PORT:
			return(vrp->priority_peers[1]);
		    case TR_RSRB_P2_PORT:
			return(vrp->priority_peers[2]);
		    case TR_RSRB_P3_PORT:
			return(vrp->priority_peers[3]);
		    default:
			return(NULL);
		    }
		    break;
		}
	    }
	    break;
	  case RSRB_PT_FST:
	    if (vrp->ipaddr == cookie.ip)
		return(vrp);
	    break;

	  case RSRB_PT_IF:
	    if (vrp->outidb == cookie.idb)
		return(vrp);
	    break;

	  case RSRB_PT_FR:
	    if ((vrp->outswidb == cookie.fr_cookie.swidb) && 
				(vrp->dlci == cookie.fr_cookie.dlci))
		return(vrp);
	    break;

	  case RSRB_PT_LAN:
	    if (vrp->outidb == cookie.idb)
		if (ieee_equal(vrp->macaddr, macaddr))
		    return(vrp);
	    break;
	default:
	    break;
	}
    }
    return(NULL);
}

char *vr_op2str (ushort op)
{
    switch(op) {
      case RSRB_OP_NONE:                return("NONE");
      case RSRB_OP_FORWARD:             return("FORWARD");
      case RSRB_OP_EXPLORER:            return("EXPLORER");
      case RSRB_OP_RINGXREQ:            return("RING_XCHG_REQ");
      case RSRB_OP_RINGXREPLY:          return("RING_XCHG_REP");
      case RSRB_OP_NOP:                 return("NOP");
      case RSRB_OP_VERSIONREQ:          return("VERSION_REQ");
      case RSRB_OP_VERSIONREPLY:        return("VERSION_REP");
      case RSRB_OP_VERSIONXREQ:         return("VERSION/RING_XREQ");
      case RSRB_OP_FORWARDCRC:          return("FORWARD/CRC");
      case RSRB_OP_LINKUP_REQ:          return("LINKUP_REQUEST");
      case RSRB_OP_LINKDOWN_REQ:        return("LINK_DOWN");
      case RSRB_OP_LLC2DATA:            return("LLC2_DATA");
      case RSRB_OP_LINKUP_PRSP:         return("LINKUP_POS_RESP");
      case RSRB_OP_LINKDOWN_PRSP:       return("LINKDOWN_POS_RESP");
      case RSRB_OP_SDLLC:               return("SDLLC");
      case RSRB_OP_QLLC:		return("QLLC");
      case RSRB_OP_TR_LANE:		return("TR LANE");
      case RSRB_OP_FORWARDFST:          return("FORWARD_FST");
      case RSRB_OP_FORWARDFSTCRC:       return("FORWARD_FST_CRC");
      case RSRB_OP_QUENCH_ON:           return("BC_QON");
      case RSRB_OP_QUENCH_OFF:          return("BC_QOFF");
      case RSRB_OP_ConnectRequest:      return("CONN_REQ");
      case RSRB_OP_ConnectPollReq:      return("CONN_POLL_REQ");
      case RSRB_OP_ConnectPollAck:      return("CONN_POLL_ACK");
      case RSRB_OP_ConnectPollAckAndReq:return("CONN_POLL_ACK_REQ");
      case RSRB_OP_ConnectWaitAck:      return("CONN_WAIT_ACK");
      case RSRB_OP_DisconnectRequest:   return("DISC_REQ");
      case RSRB_OP_DisconnectAck:       return("DISC_ACK");
      case RSRB_OP_CAPS_REQ:            return("CAPS_REQ");
      case RSRB_OP_CAPS_REPLY:          return("CAPS_REPLY");
      case RSRB_OP_IGNORE:              return("IGNORED");
      case RSRB_OP_EMPTY:               return("EMPTY");
      case RSRB_OP_ERR:                 return("ERROR");
      case RSRB_OP_UNKVRN:              return("UNK_RING_GROUP");
      case RSRB_OP_UNKPEER:             return("UNK_PEER");
      case RSRB_OP_UNKTRN:              return("UNK_TARGET_RING");
      case RSRB_OP_LA2_QON:             return("QON");
      case RSRB_OP_LA2_QON_ACK:         return("QON_ACK");
      case RSRB_OP_LA2_QOFF:            return("QOFF");
      case RSRB_OP_LA2_QOFF_ACK:        return("QOFF_ACK");
      case RSRB_OP_SEND_XID:            return("SEND_XID");
      case RSRB_OP_BPDU:		return("BPDU");
      case RSRB_OP_SPAN:		return("SPAN");
      case RSRB_OP_BADOP:               return("UNK_OP_CODE");
      case RSRB_OP_UNKVER:              return("INCOMPATIBLE_VERSION");
      default:                          return("UNKNOWN");
    }
}

char *vrpeer_id (tr_vpeertype *vrp, char *buff)
{
    char *bogus_peer = "bogus_peer";

    if (!vrp || !vrp->vrg || !buff) {
        return(bogus_peer);
    }
    switch (vrp->type) {
      case RSRB_PT_NONE:
      case RSRB_PT_TCPD:
      case RSRB_PT_FST:
	sprintf(buff, "%d/%i/%d", vrp->vrg->vrn, vrp->ipaddr, vrp->port);
	break;
      case RSRB_PT_FR:
	if (vrp->outswidb == NULL) {
	   return(bogus_peer); 
	}
	sprintf(buff, "%d/%s", vrp->vrg->vrn, vrp->outswidb->namestring);
	break;
      case RSRB_PT_IF:
      case RSRB_PT_LAN:
	if (vrp->outidb == NULL) {
	    return(bogus_peer); 
	}
	sprintf(buff, "%d/%s/%e", vrp->vrg->vrn, vrp->outidb->hw_namestring,
		vrp->macaddr);
	break;
    }
    return(buff);
}

char *vrpeer_state (tr_vpeer_statetype state)
{
    switch (state) {
      case RSRB_PS_DEAD:	return("dead");
      case RSRB_PS_CLOSED:	return("closed");
      case RSRB_PS_OPENING:	return("opening");
      case RSRB_PS_OPENWAIT:	return("openwait");
      case RSRB_PS_REMWAIT:	return("remwait");
      case RSRB_PS_REMGO:	return("remgo");
      case RSRB_PS_REMOPENED:	return("remopen");
      case RSRB_PS_DRAINING:	return("draining");
      case RSRB_PS_CONNECTED:	return("open");
      default:			return("unknown");
    }
}

/*
 * r i f _ m a t c h _ o f f s e t
 *
 * Scan a rif looking for LTRN/TBN/RTRN or RTRN/TBN/LTRN depending
 * on direction.  Return the next hop in RN and BN if match.  Return
 * the offset in bytes from the beginning of the rif.  0 will be
 * returned if we didn't match (you can't match at the RC field).
 */

int rif_match_offset(
    srbroute_t *rif,                    /* pntr to rif string */
    int ltrn,                           /* left  ring number searching for */
    int tbn,                            /* target bridge number */
    int rtrn,                           /* right ring number searching for */
    ushort *rn,                         /* next hop ring number */
    ushort *bn,                         /* next hop bridge number */
    char **why)                         /* if didn't work why */
{
    boolean reverse;
    int rd_cnt;
    srbrd_t *rd;
    ushort *rds;
    ushort leftval, rightval;

    if (!rif || !rn || !bn || !why)
        return(0);
    rd_cnt = (int) (rif->rc_blth & RC_LTHMASK) - 6;
    if (rd_cnt <= 0) {
        *why = "rif too short";
        return(0);
    }
    if (rif->rc_dlf & RC_D) {   /* preverse reverse? */
        rd = (srbrd_t *) &leftval;
        rd->ring_num = rtrn;
        rd->bridge_num = tbn;
        rightval = 0;
        rd = (srbrd_t *) &rightval;
        rd->ring_num = ltrn;
        rds = (ushort *) &rif->rd[1];
        reverse = TRUE;
    } else {
        rd = (srbrd_t *) &leftval;
        rd->ring_num = ltrn;
        rd->bridge_num = tbn;
        rightval = 0;
        rd = (srbrd_t *) &rightval;
        rd->ring_num = rtrn;
        rds = (ushort *) &rif->rd[0];
        reverse = FALSE;
    }
    *rn = *bn = (ushort) -1;
    for ( ; rd_cnt; rd_cnt -= 2) {
        if ((GETSHORT(&rds[0]) == leftval) && 
	   ((GETSHORT(&rds[1]) & RD_RNMASK) == rightval)) {            

	   if (reverse) {
                rd = (srbrd_t *) (--rds);
                *rn = ((GETSHORT(rd) & RD_RNMASK) >> RD_RNSHIFT);
                *bn = (GETSHORT(rd) & RD_BNMASK);
                rds++;
            } else {
                rd = (srbrd_t *) (++rds);
                *bn = (GETSHORT(rd++) & RD_BNMASK);
                *rn = ((GETSHORT(rd) & RD_RNMASK) >> RD_RNSHIFT);
            }
            break;
        }
        rds++;
    }
    if (*rn == (ushort ) -1) {
        *why = "no match";
        return(0);
    }
    return((uchar *) rds - (uchar *) rif);
}

/*
 * vrforce_xchg_delayed
 * Flag this vring so we will force ring exchanges to
 * take place.  This generally is only called when somekind
 * of processing has failed and we need to make sure this side
 * and the other are in sync.  A force is flaged for the process
 * level routines to do the proper thing.
 */

void vrforce_xchg_delayed (tr_vgrouptype *vrg)
{
    vrg->flags |= VRGF_FORCEXCHG;
    process_set_boolean(vr_wake_up, TRUE);
}

/*
 * ring_xchg_pak
 * get a packet properly constructed with ring exchange info
 * for the given ring group.
 */

paktype *ring_xchg_pak (tr_vgrouptype *vrg, ushort op, ushort version)
{
    paktype *pak;
    tr_vhdrtype *vhdr;
    tr_ventrytype *vre;
    tr_vrxtype_2 *vrx2 = NULL;
    int ringcount = 0;
    
    /* Actually determine how many things we have */
    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
	if (vre->type & 
	    ( RSRB_ET_LOCAL |
	      RSRB_ET_SDLLC |
	      RSRB_ET_QLLC  |
              RSRB_ET_TR_LANE |
	      RSRB_ET_LOCAL_VIRTUAL)) {
	    ringcount++;
	}
    }

    /*
     * We allocate slop here because there are other things than token rings
     * that are "local rings"
     */
    pak = vrpak(version, op, 0, vrg->vrn, 0, sizeof(tr_vrxtype_2) * ringcount);
    if (!pak) {
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: couldn't get pak for ringXchg");
	return(NULL);
    }
    vhdr = (tr_vhdrtype *)pak->network_start;
    vrx2 = (tr_vrxtype_2 *) ((uchar *) vhdr + sizeof(tr_vhdrtype));

    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
        if ( vre->type == RSRB_ET_QLLC && version < RSRB_VERSION) {
	    if (srberr_debug) {
		buginf("\nRSRB: QLLC not supported in version %d.\n\
      Please reconfigure.", version);
	    }
	    continue;
	}
	if (vre->type & 
	    ( RSRB_ET_LOCAL |
	      RSRB_ET_SDLLC |
	      RSRB_ET_QLLC  |
              RSRB_ET_TR_LANE |
 	      RSRB_ET_LOCAL_VIRTUAL)) {

	    vrx2->bn = vre->bn;
	    /* Only advertise SDLLC ring if sdllc local ack not set */
	    if ( (vre->type == RSRB_ET_SDLLC) && (!sdllc_lak_enable) )
		vrx2->bn |= RSRB_BN_SDLLC;

	    /* Only advertise QLLC ring if qllc local ack not set */
	    else if ( (vre->type == RSRB_ET_QLLC) && (!lnx_lak_enable) )
		vrx2->bn |= RSRB_BN_QLLC;

	    vrx2->rn = vre->rn;
	    ieee_copy(vre->macaddr, vrx2->macaddr);
	    vrx2++;
	}
    }
    PUTSHORT(&vhdr->len, ((uchar *) vrx2 - (uchar *) vhdr - 
			  sizeof(tr_vhdrtype)));
    pak->datagramsize = GETSHORT(&vhdr->len) + sizeof(tr_vhdrtype);
    return(pak);
}

/*
 * vrqueue_null_xchg
 * Handle special case where there isn't any data in the
 * ring exchange packet.  Since all ring exchange processing
 * must be done at process level we must have some kind of data
 * that gets passed.  We use an empty packet and queue it up
 * for the background processor.
 */

void vrqueue_null_xchg (tr_vpeertype *vrp)
{
    paktype *pak;

    pak = getbuffer(0);			/* smallest possible */
    if (pak) {
	pak->datagramstart = pak->network_start;
	pak->datagramsize = 0;		/* nothing to do */
	pak->peer_ptr = vrp;
	srb_enq(vrxchgQ, pak);
    } else
	vrforce_xchg_delayed(vrp->vrg);
}

/*
 * rsrb_checkqueue
 * Like checkqueue, but can cope with the "priority" structure
 */
boolean rsrb_checkqueue (
    queuetype *q,
    tr_vpeertype *e)
{
    tr_vpeertype *p;

    if (q == NULL)
        return(FALSE);
    p = q->qhead;	/* first member */
    while (p) {
	if (p == e) 			/* match? */
	    return(TRUE);		/* yes, return TRUE */
	else {
	    if (p->priority) {
		if ((e == p->priority_peers[1]) ||
		    (e == p->priority_peers[2]) ||
		    (e == p->priority_peers[3])) {
		    return(TRUE);
		}
	    }
	    p = p->next;		/* no, cdr the list */
	}
    }
    return(FALSE);			/* not in queue, return FALSE */
}



/*
 * vrprocess_ring_xchg
 * update the ring table with the results of a Ring Xchg Reply or
 * Request.  This handles both deletions and insertions.  Interrupts
 * aren't in general disabled here.  The only thing used at interrupt
 * level is idb->tr_vring_blk which points at the queue structure in
 * the group block.  This is so I didn't have to deal with this
 * routine pulling the rug out from under the interrupt routines.
 *
 * WARNING:  This routine should only be called from process level.
 * It uses malloc and free.
 */

void vrprocess_ring_xchg (tr_vpeertype *vrp, paktype *pak)
{
    tr_ventrytype *vre, *next;
    boolean foond = FALSE;
    int index;
    char peerid[MAX_PEER_NAME];
    int count;
    tr_vrxtype_2 *vrx2 = NULL;
    ushort bytes;
    tr_vgrouptype *vrg;

    /*
     * If a "sub priority," make us point to the top priority (geez, all
     * the ring information should be the SAME, after all, for each of the
     * priority peers...)
     */
    if (vrp->priority_top)
	vrp = vrp->priority_top;
    vrg = vrp->vrg;
    bytes = pak->datagramsize;
    vrx2 = (tr_vrxtype_2 *) pak->datagramstart;
    
    if (bytes % sizeof(tr_vrxtype_2)) {
	if (srb_debug || srberr_debug)
	    buginf("\nRSRB: XCHG: req/reply badly formed, length %d, peer %s",
		   bytes, vrpeer_id(vrp, peerid));
	return;
    }
    if (vrfind_ringgroup(vrg->vrn) != vrg || /* something changed */
	!rsrb_checkqueue(&vrg->peers,  vrp))
	return;
    count = bytes / sizeof(tr_vrxtype_2);
    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; ) {
	if (!((vre->type & (RSRB_ET_REMOTE |
			    RSRB_ET_REMSDLLC | RSRB_ET_REMQLLC))) ||
	    vrp != vre->outhole.vp) {
	  vre = vre->next;
	  continue;
	}
	foond = FALSE;
        if (vrx2 && count > 0) {
	    for (index = 0; index < count; index++) {
	        if ((vrx2[index].rn == vre->rn) &&
                      ((vrx2[index].bn & RSRB_BN_MASK) == vre->bn)) {
                    vrx2[index].rn = RSRB_RN_DONE;
		    foond = TRUE;
		    ieee_copy(vrx2[index].macaddr, vre->macaddr);
                    if (vrx2[index].bn & RSRB_BN_SDLLC)
		        vre->type = RSRB_ET_REMSDLLC;
		    else
		        vre->type = RSRB_ET_REMOTE;

		    break;
		}
	    }
	}
	if (!foond) {
	    if (srb_debug || srbevent_debug)
		buginf("\nRSRB: removing bridge %d ring %d from %s [%d]",
		       vre->bn, vre->rn, vrpeer_id(vrp, peerid),
		       vre->type);
	    next = vre->next;
	    p_unqueue(&vrg->rings, vre);
	    free(vre);
	    vre = next;
	    continue;
	}
	vre = vre->next;
    }
    if (vrx2 && count > 0) {
	for (index = 0; index < count; index++) {
            if (vrx2[index].rn != RSRB_RN_DONE) {
		vre = malloc_vre(rsrb_cb);
		if (vre) {
                    vre->bn = vrx2[index].bn;
                    vre->rn = vrx2[index].rn;
		    ieee_copy((char *)vrx2[index].macaddr, vre->macaddr);
                    vre->type = 
		    	(vre->bn & RSRB_BN_SDLLC) ? RSRB_ET_REMSDLLC :
		    	(vre->bn & RSRB_BN_QLLC)  ? RSRB_ET_REMQLLC  :
		    		RSRB_ET_REMOTE;
                    vre->bn &= RSRB_BN_MASK;
		    vre->outhole.vp = vrp;
		    p_enqueue(&vrg->rings, vre);
		    if (srb_debug || srbevent_debug)
		      buginf("\nRSRB: added bridge %d, ring %d for %s",
			     vre->bn, vre->rn, vrpeer_id(vrp, peerid));
		} else {
 		    (*vrp->abort)(vrp);
		}
	    }
	}
    }
}

/*
 * update_peers
 * Generate an update message to each of the peers for any ring groups
 * that have changed.  An update message tells the peers about any
 * configuration changes this node has undergone.
 */

void update_peers (void)
{
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    paktype *pak;
    char peerid[MAX_PEER_NAME];

    for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
	if (!(vrg->flags & VRGF_GENUPDATE))
	    continue;
	vrg->flags &= ~VRGF_GENUPDATE;
	if (vrg->peers.qhead == NULL)
	    continue;
	for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
	    switch (vrp->state) {
	      case RSRB_PS_DEAD:
	      case RSRB_PS_CLOSED:	/* will happen when the bugger */
	      case RSRB_PS_OPENING:	/* opens */
	      case RSRB_PS_OPENWAIT:
	      default:
		continue;

	      case RSRB_PS_REMWAIT:
	      case RSRB_PS_REMGO:
	      case RSRB_PS_REMOPENED:
	      case RSRB_PS_DRAINING:	/* do it */
	      case RSRB_PS_CONNECTED:
		break;
	    }
	    pak = ring_xchg_pak(vrg, RSRB_OP_RINGXREQ, vrp->vrp_version);
	    if (!pak) {
		errmsg(&msgsym(RNGXFAIL, RSRB), vrpeer_id(vrp, peerid));
	        break;
	    }
	    if ((*vrp->sender)(vrp, NULL, pak)) {
		if (srb_debug || srbevent_debug)
		buginf("\nRSRB: sent RingXreq to %s", vrpeer_id(vrp, peerid));
	}
	    else
		errmsg(&msgsym(RNGXFAIL, RSRB), vrpeer_id(vrp, peerid));
	}
    }
}

/*
 * vrsend_version
 * send a version request/reply to the peer indicated
 */

void vrsend_version (
    tr_vpeertype *vrp,
    ushort op,
    boolean abort_on_fail)
{
    paktype *pak;
    char peerid[MAX_PEER_NAME];
    
    if (!vrp) {
        buginf("\nRSRB: vrsend_version passed null peer pointer");
        return;
    }
    if (op == RSRB_OP_VERSIONXREQ)
	pak = ring_xchg_pak(vrp->vrg, op, vrp->vrp_version);
    else
	pak = vrpak(vrp->vrp_version, op, 0, vrp->vrg->vrn, 0, 0);
    if (!pak) {
	if (srb_debug || srberr_debug)
	    buginf("\nRSRB: couldn't get pak for version exchange");
	return;
    }

    if ((*vrp->direct)(vrp, pak)) {
        if (srb_debug || srbevent_debug)
            buginf("\nRSRB: %s sent -> %s", vr_op2str(op), vrpeer_id(vrp, peerid));
    } else {
        if (srb_debug || srberr_debug)
            buginf("\nRSRB: %s -> %s failed", vr_op2str(op), vrpeer_id(vrp, peerid));
        if (abort_on_fail)
	  (*vrp->abort)(vrp);
    }
}

boolean vrsend_caps (tr_vpeertype *vrp, ushort op, ulong caps, char *label)
{
    paktype *pak;
    char peerid[MAX_PEER_NAME];
    tr_vhdrtype *vrh;

    if (!vrp) {
        buginf("\nRSRB: %s: vrsend_caps passed null peer pointer", label);
        return(FALSE);
    }
    pak = vrpak(vrp->vrp_version, op, 0, vrp->vrg->vrn, 0, 0);
    if (!pak) {
        if (srb_debug || srberr_debug)
            buginf("\nRSRB: %s: couldn't get pak for caps packet", label);
        return(FALSE);
    }
    vrh = (tr_vhdrtype *) pak->datagramstart;
    vrh->param = caps;

    if ((*vrp->direct)(vrp, pak)) {
        if (srb_debug || srbevent_debug)
            buginf("\nRSRB: %s: %s sent -> %s", label, vr_op2str(op), vrpeer_id(vrp, peerid));
    } else {
        if (srb_debug || srberr_debug)
            buginf("\nRSRB: %s: %s -> %s failed", label, vr_op2str(op), vrpeer_id(vrp, peerid));
        return(FALSE);
    }
    return(TRUE);
}

/*
 * vrsend_xchg
 * force a request or a reply to be sent immediately and if it fails
 * abort the connection
 */

boolean vrsend_xchg (tr_vpeertype *vrp, ushort op, char *label,
		     boolean doabort)
{
    paktype *pak;
    char peerid[MAX_PEER_NAME];

    pak = ring_xchg_pak(vrp->vrg, op, vrp->vrp_version);
    if (pak && (*vrp->direct)(vrp, pak)) {
	if (srb_debug)
	  buginf("\nRSRB: %s: %s sent to peer %s", label,
		 vr_op2str(op), vrpeer_id(vrp, peerid));
	return(TRUE);
    }
    if (srb_debug || srbevent_debug || srberr_debug)
        buginf("\nRSRB: %s: %s for %s failed%s", label,
	       vr_op2str(op), vrpeer_id(vrp, peerid),
	       (pak ? "" : " (no pak)"));
    if (doabort) {
	(*vrp->abort)(vrp);
    }
    return(FALSE);
}

/*
 * v r _ v e r s i o n _ r e a s o n a b l e
 *
 * Check a headers version against what this RSRB implementation can accept.
 * If not the same, check to see if we can down rev and if reasonable do it.
 * If not return false telling our caller that the shit has hit the fan.
 */

boolean vr_version_reasonable (tr_vpeertype *vrp, tr_vhdrtype *vrh, char *label)
{
    char peerid[MAX_PEER_NAME];

    if (!vrp)                           /* no vrp can't possibly be reasonable */
        return(FALSE);
    if (vrh->rhdr_version != vrp->vrp_version) {
        /*
         * Do backward compatibility version testing.
         *
         * RSRB version 3 recognizes RSRB version 2.  Version 3 supports
         * RSRB_OP_CAPS_REQ and LOCACK (LAK2).  Version 2 supports old LACK but
         * doesn't support RSRB_OP_CAPS_REQ or CAPS_REPLY.
         */
        if (vrh->rhdr_version == RSRB_PREV_VERSION) {
            buginf("\n\nRSRB: %s: Connected to a back rev peer: %s, ver %d, ours %d",
                   label, vrpeer_id(vrp, peerid), vrh->rhdr_version, vrp->vrp_version);
            buginf("\nRSRB: Connectivity is being established but at a DEGRADED level.");
            buginf("\nRSRB: It is strongly suggested that the remote peer be upgraded.\n");
            vrp->vrp_version = RSRB_PREV_VERSION;
            return(TRUE);
        } else if (vrh->rhdr_version == RSRB_VERSION) {
            buginf("\n\nRSRB: %s: Connected to a current rev peer: %s, ver %d, ours %d\n",
                   label, vrpeer_id(vrp, peerid), vrh->rhdr_version, vrp->vrp_version);
            vrp->vrp_version = RSRB_VERSION;
            return(TRUE);
        } else
            return(FALSE);
    }
    return(TRUE);
}


/********************************************************************
 *
 * Connector
 *
 ********************************************************************/

boolean rsrb_tcp_correct_port (
    tr_vpeertype *vrp)
{
    if (!vrp->priority_top) {
	return(vrp->port == TR_RSRB_P0_PORT);
    } else {
	if (vrp->priority_top->priority_peers[1] == vrp)
	    return(vrp->port == TR_RSRB_P1_PORT);
	else if (vrp->priority_top->priority_peers[2] == vrp)
	    return(vrp->port == TR_RSRB_P2_PORT);
	else if (vrp->priority_top->priority_peers[3] == vrp)
	    return(vrp->port == TR_RSRB_P3_PORT);
    }
    return(FALSE);
}

boolean vrcheck_conn_state (tr_vgrouptype *vrg, tr_vpeertype *vrp)
{
    boolean bad;

    bad = FALSE;
    if (!vrg) {
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: CONN: bad connection block, vrn");
	return(FALSE);
    }
    if (vrg != vrp->vrg) {
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: CONN: peer modified");
	return(FALSE);
    }
    if (!rsrb_checkqueue(&vrg->peers, vrp)) {
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: CONN: corrupt connection block");
	return(FALSE);
    }
    switch (vrp->type) {
      case RSRB_PT_TCPD:
	if (!vrp->ipaddr || !(rsrb_tcp_correct_port(vrp)))
	    bad = TRUE;
	break;
      case RSRB_PT_FST:
	if (!vrp->ipaddr)
	    bad = TRUE;
	break;

      case RSRB_PT_IF:
	if (!vrp->outidb)
	    bad = TRUE;
	break;

      case RSRB_PT_FR:
	if ((!vrp->outswidb) || !(vrp->dlci))
	    bad = TRUE;
	break;
	
      case RSRB_PT_LAN:
	if ((!vrp->outidb) || is_ieee_zero(vrp->macaddr))
	    bad = TRUE;
	break;

      default:
	bad = TRUE;
	break;
    }
    if (bad || !vrp->sender || !vrp->connect || !vrp->connector_owns) {
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: CONN: bad connection info, peer: %i %-08x",
		   vrp->ipaddr, vrp);
	return(FALSE);
    }
    if (vrp->startq.qhead == NULL) {
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: CONN: warning, no initial packet, peer: %i %-08x",
		   vrp->ipaddr, vrp);
    }
    return(TRUE);
}

/*
 * vrconnectorBLOCK
 * block until we either become !REMWAIT or timeout
 */

boolean vrconnectorBLOCK (tr_vpeertype *vrp)
{
    if (vrp->state != RSRB_PS_REMWAIT)
	return(FALSE);
    if (AWAKE(vrconn_wait))
	return(FALSE);
    return(TRUE);
}

/*
 * vrconnector
 * Handle making connections to peers.  Connections needed are queued
 * up in the vrconnQ.  When the connection is established we throw
 * all queued packets at it.  If the connection fails the first of the
 * queued up packets is thrown away.
 *
 * Simultaneous opens are broken when the first packet comes into the
 * listener.
 */

void vrconnect1 (
    tr_conntype *vrc)
{
    tr_starttype *start;
    leveltype status;
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    tr_vhdrtype *vrh;
    ushort vrn, type;
    char peerid[MAX_PEER_NAME];
    int connres;

    status = raise_interrupt_level(NETS_DISABLE); /* flush the delay queue */
    /*
     * sanity checks
     */
    vrp = vrc->vrp;
    vrg = vrfind_ringgroup(vrc->vrn);
    if(!vrg) {
      vrp->connector_owns = FALSE;
      return;
    }
    if (!vrcheck_conn_state(vrg, vrp)) {
	vrp->connector_owns = FALSE;
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: check_conn_state failed, vrg = %x", (ulong) vrg);
	reset_interrupt_level(status);
	return;
    }

    /*
     * What state is this peer in? If not opening or remopened,
     * stop here...
     */
    switch (vrp->state) {
      case RSRB_PS_REMOPENED:
	if (vrp->type == RSRB_PT_TCPD && vrp->t == NULL) {
	    /*
	     * If there are any pak's in start up queue, drain them.
	     * CSCdi15658
	     */
	    while ((start = dequeue(&vrp->startq))) {
		vr_nukepak(start->pak);
		free(start);
	    }
	    errmsg(&msgsym(OPTNULL, RSRB));
	    rsrb_newstate(vrp, RSRB_PS_DEAD);
	}
	reset_interrupt_level(status);
	break;

      case RSRB_PS_OPENING:
	rsrb_newstate(vrp, RSRB_PS_OPENWAIT);
	reset_interrupt_level(status);
	vrn = vrc->vrn;
	type = vrp->type;
	connres = (*vrp->connect)(vrp, vrn);
	switch (connres) {
	  case RSRB_CONN_ABORTED:
	    vrp->connector_owns = FALSE;
	    return;
	  case RSRB_CONN_FAILED:
	    status = raise_interrupt_level(NETS_DISABLE); /* flush delay q */
	    while ((start = dequeue(&vrp->startq))) {
                vr_nukepak(start->pak);
		free(start);
	    }
	    rsrb_newstate(vrp, RSRB_PS_DEAD);
	    vrp->inhdr.op = RSRB_OP_EMPTY;
	    vrp->t = NULL;
	    vrp->connector_owns = FALSE;
	    reset_interrupt_level(status);
	    return;
	  case RSRB_CONN_CONNECTED:
	    break;
	}
	break;

      default:
        while ((start = dequeue(&vrp->startq))) {
              vr_nukepak(start->pak);
              free(start);
        }
	errmsg(&msgsym(PEERSTAT, RSRB), vrpeer_id(vrp, peerid), vrp->state);
	vrp->connector_owns = FALSE;
	reset_interrupt_level(status);
	return;
    }
    /*
     * Drain the startq under interrupt lockout so more don't sneak in
     * when we think we are empty.  We can't use sender because the
     * packets have already been munged.  Also be careful to detect
     * an abort and avoid going into a wedged state.
     */
    status = raise_interrupt_level(NETS_DISABLE);
    /* did we bail? */
    if (vrp->state == RSRB_PS_CLOSED || vrp->state == RSRB_PS_DEAD) {
	vrp->connector_owns = FALSE;
	reset_interrupt_level(status);
	return;
    }
    rsrb_newstate(vrp, RSRB_PS_DRAINING);
    reset_interrupt_level(status);
    while (TRUE) {
	process_suspend();	/* give queues a chance to drain */
	status = raise_interrupt_level(NETS_DISABLE);
	if ((srb_debug || srbevent_debug || srberr_debug) &&
	    (vrp->state != RSRB_PS_CLOSED &&
	     vrp->state != RSRB_PS_DRAINING))
	    buginf("\nRSRB: CONN: drain startq, peer %s bad state %d",
		   vrpeer_id(vrp, peerid), vrp->state);
	start = dequeue(&vrp->startq);
	if (!start) {
	    if (vrp->state == RSRB_PS_DRAINING) {
		rsrb_newstate(vrp, RSRB_PS_CONNECTED);
		vrp->connector_owns = FALSE;
		TIMER_START(vrp->connect_timer, RSRB_CONNECT_WINDOW);
	    }
	    vrp->connector_owns = FALSE;
	    reset_interrupt_level(status);
	    return;
	}
        /*
         * Fix the version number on frames that haven't gone out yet.
         */
 	vrh = (tr_vhdrtype *)(start->pak->datagramstart);
 	PUTSHORT(&vrh->rhdr_version, vrp->vrp_version);
        (void) (*vrp->direct)(vrp, start->pak);
	reset_interrupt_level(status);
	free(start);
    }
}


forktype vrconnector (void)
{
    tr_conntype *vrc;
    leveltype status;

    while (TRUE) {
	status = raise_interrupt_level(NETS_DISABLE);
	if (vrconnQ.qhead == NULL) {
	    vrconn_pid = 0;
	    if (srb_debug || srbevent_debug)
		buginf("\nRSRB: CONN: connector terminating");
	    reset_interrupt_level(status);
	    process_kill(THIS_PROCESS);
	}
	vrc = dequeue(&vrconnQ);
	reset_interrupt_level(status);
	vrconnect1(vrc);
	free(vrc);
    }
}

/*******************************************************************
 *
 * Virtual Ring IDB handling
 *
 * This handles packets/explorers sent and coming from our "virtual ring"
 * addresses (gads!).  The RSRB code puts the first byte of the actual
 * frame as the first byte after the overhead space.  We must shuffle the
 * packet down into the encapsulation are to align the first byte of
 * protocol data as the first byte after the overhead space.  This is
 * ghastly for performance, but hell, we're already in the middle of the
 * vring code.
 *
 *******************************************************************/

/*
 * Raw enqueue for virtual rings (take it in)
 */
void rsrb_vidb_iqueue (
    hwidbtype *idb,
    paktype *pak)
{
    tring_hdr *trh;		/* pointer to TR MAC header */
    int riflen;			/* length of RIF in TR header */
    int encsize;		/* length of encapsulation */
    int enctype;		/* encapsulation type */
    sap_hdr *llc1;		/* pointer to llc-1 header */
    uchar *newstart;
    int info_size;
    leveltype level;

    GET_TIMESTAMP(idb->lastinput);	/* remember time of last input */
    pak->inputtime = idb->lastinput;	/* save it in packet too */
    idb->counters.rx_cumbytes += pak->datagramsize;
    idb->counters.inputs++;

    trh = (tring_hdr *) pak->datagramstart;
    if (trh->saddr[0] & TR_RII) {	/* have a rif get its length */
	riflen = trh->data[0] & RC_LTHMASK;
	llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);
    } else {
	riflen = 0;
	llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES);
    }
    if (tokenring_debug) {
	if (tr_print_packet(idb," in:", trh, (ulong *) llc1, 0, pak->datagramsize))
	    buginf("\nVR%d:  in:      riflen %d, llc_offset %d",
		   idb->unit, riflen, (uchar *)llc1 - (uchar *)trh);
    }
    rif_update(idb, 0, trh->saddr, NULL, (srbroute_t *)
	       (riflen ? trh->data : NULL), RIF_AGED);

    /*
     * Terribly grody hack to get parse packet to work!  real way is
     * to call parse packet prior to calling rsrb_vidb_iqueue(), and
     * remove cruft at top of file.  Maybe calling inside is ok, but
     * need more time to look.
     */

    (*idb->parse_packet)(idb, pak, NULL);

    if ((trh->fc & FC_TYPE_MASK) == FC_TYPE_MAC) {
	enctype = ET_MAC;
	info_size = 0;
    } else {
	switch (llc1->control & ~LLC1_P) {
	  case LLC1_UI:
	    token_get_enctype_infosize(llc1, &enctype, &info_size, NULL);
	    break;

	  case LLC1_XID:
            tr_process_xid(idb, pak, FALSE, FALSE);
	    netinput_enqueue(pak);
	    return;

	  case LLC1_TEST:
            tr_process_test(idb, pak, FALSE);
	    netinput_enqueue(pak);
	    return;

	  default:
	    token_get_enctype_infosize(llc1, &enctype, &info_size, NULL);
	    encsize = TRING_ENCAPBYTES + riflen + info_size;
	    newstart = pak_center(pak) - encsize;
	    bcopy(pak->datagramstart, newstart, pak->datagramsize);
	    pak->datagramstart = newstart;
	    pak->enctype = enctype;
	    token_set_data_pointers(pak, riflen, info_size);
            level = raise_interrupt_level(NETS_DISABLE);
	    reg_invoke_llc2_input(pak);
            reset_interrupt_level(level);
	    return;
	}
    }

    if (trh->daddr[0] & TR_GROUP)
        idb->counters.input_broad++;
    encsize = TRING_ENCAPBYTES + riflen + info_size;
    newstart = pak_center(pak) - encsize;
    bcopy(pak->datagramstart, newstart, pak->datagramsize);
    pak->datagramstart = newstart;
    pak->enctype = enctype;
    token_set_data_pointers(pak, riflen, info_size);
    raw_enqueue(idb,pak);
}

/*
 * Raw enqueue for virtual rings (put it out). 
 */
boolean rsrb_vidb_oqueue (
    hwidbtype *idb,
    paktype *pak, 
    enum HEADTAIL which)
{
    tring_hdr *trh;		/* pointer to TR MAC header */
    int riflen;			/* length of RIF in TR header */
    ulong *llc1;		/* pointer to the LLC header */

    trh = (tring_hdr *) pak->datagramstart;
    if (tokenring_debug) {
	if (trh->saddr[0] & TR_RII)		/* have a rif get its length */
	    riflen = trh->data[0] & RC_LTHMASK;
	else riflen = 0;
	llc1 = (ulong *) (&trh->data[riflen]);
	tr_print_packet(idb, "out:", trh, llc1,
			((pak->flags & PAK_SRB_OUT) ? BF_BRIDGE :
			 ((pak->flags & PAK_SRB_EXPLORER) ?
			  (BF_BRIDGE|BF_EXPLORER) : 0)),
			pak->datagramsize);
    }
    pak->if_input = idb->firstsw;
    /* SRB AST support */
    if ((GETSHORT(&trh->daddr[0]) == TRFUNC_UPPER) &&
        (GETLONG(&trh->daddr[2]) == TRFUNC_BRIDGE)) {
	/* Spanning Tree BPDUs */
	vrforward_bpdu(pak);
    } else if ((GETSHORT(&trh->daddr[0]) == TRFUNC_UPPER) &&
        (GETLONG(&trh->daddr[2]) != TRFUNC_MON)) {
        /* make LNM happy */
        srb_sb_incr_tx_b_counters(idb->firstsw, pak->datagramsize);
        vrforward_explorer(pak, 0, NULL, idb->unit);
    } else {
	/* make LNM happy */
        srb_sb_incr_tx_nb_counters(idb->firstsw, pak->datagramsize);
	vrforward_pak(pak, FALSE, 0, RSRB_RIFLF_LEAVEALONE);
    }
    return(TRUE);
}

/*
 * vr_new_vidb
 * Create a virtual IDB with appropriately set parameters.
 */
hwidbtype *vr_new_vidb (ushort vrn, hwidbtype *tr_idb)
{
    idbtype   *swidb;
    hwidbtype *idb;
    srb_sb_t  *srb_sb, *tr_srb_sb;
    
    if ((swidb = vidb_malloc()) == NULL) {
	return(NULL);
    }
    idb = swidb->hwptr;

    idb->vencap 		= tr_vencap;
    idb->name 			= "VirtualRing";
    idb->unit 			= tr_idb->unit;
    idb_init_names(swidb, TRUE);
    idb->state 			= IDBS_UP;
    idb->status 		= IDB_TR | IDB_VIRTUAL;
    idb->maxmtu 		= TR_MTU;
    idb->enctype 		=
    idb->enctype_def 		=
    idb->enctype_cfg 		=
    swidb->ip_enctype 		= ET_SNAP;
    reg_invoke_media_enctype_setup(swidb, ET_SNAP);
    idb->nokeepalive 		= FALSE;
    idb->reliability 		= 255;
    idb->tr_3com_hack 		= FALSE;
    swidb->arp_arpa 		= 
    swidb->arp_arpa_def 	= FALSE;
    swidb->arp_probe 		=
    swidb->arp_probe_def 	= FALSE;
    swidb->arp_snap 		=
    swidb->arp_snap_def 	= TRUE;
    swidb->arp_timeout 		= ARP_MAXAGE;

    idb->deencap 		= NULL;
    idb->soutput 		= (soutput_t)return_nothing;
    idb->oqueue 		= rsrb_vidb_oqueue;
    idb->oqueue_dequeue 	= (oqueue_dequeue_t)return_null;
    idb->reset 			= (reset_t)return_nothing;
    idb->shutdown 		= (shutdown_t)return_nothing;
    idb->linestate 		= (linestate_t) return_true;
    idb->getlink 		= tring_getlink;
    idb->broadcast 		= tr_bcast;
    idb->periodic 		= (periodic_t)return_nothing;
    idb->iqueue 		= rsrb_vidb_iqueue;
    idb->counters.resets 	= 0;	/* clear reset counter */
    idb->counters.transitions 	= 0;

    tr_idb->vidb_link		= idb;
    idb->vidb_link 		= tr_idb;

    srb_sb = srb_create_sb(swidb);
    if (!srb_sb) {
        printf(nomemory);
        return(NULL);
    }

    tr_srb_sb = srb_get_sb_inline(tr_idb->firstsw);
    if (tr_srb_sb) {
        srb_sb->srb_thisring   = tr_srb_sb->srb_targetring;
        srb_sb->srb_bridge_num = tr_srb_sb->srb_bridge_num;
        srb_sb->srb_targetring = tr_srb_sb->srb_thisring; 
        srb_sb->srb_spanning_explorer  = tr_srb_sb->srb_spanning_explorer;
    } else {
        srb_sb->srb_thisring   = vrn;
        srb_sb->srb_targetring = vrn;
        srb_sb->srb_spanning_explorer = FALSE;
    }

    srb_multiring_enable_all(swidb);

    idb->tr_ring_mode 		= RINGMODE_NETMGR | RINGMODE_RPS | RINGMODE_REM;
    idb->extract_hwaddr = ieee_extract_hwaddr;
    idb->parse_packet   = tring_parse_packet;
    idb->extract_snpa   = tr_extract_addr;    

    llc_sb_copy(swidb, tr_idb->firstsw);

    (void)reg_invoke_lanmgr_create_ring(idb);
    return(idb);
}

/*
 * vr_virtualp
 * Tell whether this is a packet for one of our "virtual"
 * interfaces.  If so, return the pointer to the vre.
 */
tr_ventrytype * vr_virtualp ( tr_vgrouptype *   outvrg, 
                              ushort            vrn, 
                              uchar *           macaddr)
{
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;

    vrg = (outvrg) ? outvrg : vrfind_ringgroup(vrn);
  
    if (!vrg)
        return(NULL);
  
    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
         /*
          * The existence of vre->virtualp vector is not guaranteed!
          */
         if (vre->virtualp) {
             if (vre->virtualp(vre, macaddr))
                 return vre;
         }
         else {
             if (virtualp_default(vre, macaddr))
                 return vre;
         }
    }
    return NULL;
}

/* 
 * The default vre->virtualp vector 
 */
boolean virtualp_default(tr_ventrytype *vre, uchar *macaddr)
{
    if ( (vre->type & (RSRB_ET_LOCAL | RSRB_ET_LOCAL_VIRTUAL)) &&
         (!bcmp(vre->macaddr, macaddr, IEEEBYTES)) ) {
          return TRUE;
    }
  
    return FALSE;
}

/*
 */

uchar *vr_first_virtual (uint vrn)
{
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    
    vrg = vrfind_ringgroup(vrn);
    if (!vrg)
        return(NULL);

    /*
     * Find the requested MAC address
     */
    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
	if (vre->type == RSRB_ET_LOCAL)
	    break;
    }
    if (!vre)
	return(NULL);
    return(vre->macaddr);
}
/*
 */
uchar *vr_next_virtual (ushort vrn, uchar *macaddr)
{
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    
    vrg = vrfind_ringgroup(vrn);
    if (!vrg)
        return(NULL);

    /*
     * Find the requested MAC address
     */
    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
        if ((vre->type & RSRB_ET_USESMAC) && ieee_equal(vre->macaddr, macaddr))
	    break;
    }
    if (!vre)
	return(NULL);

    /*
     * Now find the next MAC address.
     */
    for (vre = vre->next ; vre; vre = vre->next) {
        if (vre->type & RSRB_ET_USESMAC)
	    break;
    }
    if (!vre) {
	/*
	 * Wrap around if necessary.
	 */
	for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
            if (vre->type & RSRB_ET_USESMAC)
		break;
	}
    }
    return(vre->macaddr);
}

/*
 * vr_vre_input
 * Pass a packet to the "virtual ring" input queue for processing
 * Boolean "from remote" says it is from a remote source (not
 * from a local token ring).
 */
void vr_vre_input (tr_ventrytype *vre, paktype *pak, boolean nuke_crc, int op)
{

typedef void (*sdllc_vidb_input_t)(hwidbtype *idb, paktype *pak,
				  uint opcode, tr_ventrytype *vre);


    hwidbtype *idb = vre->virtual_idb;
    if (nuke_crc)
	pak->datagramsize -= TR_FCS_SIZE;
    clear_if_input(pak);
    pak->if_input = idb->firstsw;
    pak->enctype = ET_NULL;
    if (vre->type & ( RSRB_ET_SDLLC | RSRB_ET_QLLC | RSRB_ET_TR_LANE) ) {
	/*
	 * Don't blame me.  I'm only the piano player.
	 * This is absolutely DISGUSTING.  Whoever did this
	 * overloading of iqueue should be shot.  It is asking
	 * for bugs.  Guess what?  They were there.  Coupled
	 * with the lack (not to be confused with local ack) of
	 * checking, this was a disaster waiting to happen.
	 *
	 * I'm leaving this in now because to fix this right
	 * requires a restructuring.  We WILL do this later.
	 *
	 * Note sdllc_vre_input has been nuked.   [cire]
	 *        lnx_vre_input is a copycat   [pmorton]
	 */

	/*
	 * Put this here for now. llc2_input() now requires that the
	 * packet be parsed properly before it can digest it. This
	 * should handle it for sdllc and qllc protocol conversion
	 * [sberl]
	 */
	(*pak->if_input->hwptr->parse_packet)(pak->if_input->hwptr,
					      pak, NULL);
	
	(*((sdllc_vidb_input_t)(idb->iqueue)))(idb, pak, op, vre);
    } else
	(*idb->iqueue)(idb, pak);
}


/********************************************************************
 *
 * Raw Interface: RSRB Implementation
 * Support
 *
 ********************************************************************/

void vr_nukepak (paktype *pak)
{
    leveltype status;

    if (pak->if_input) {
	status = raise_interrupt_level(ALL_DISABLE);
        if (pak->refcount > 1) {
            pak->refcount--;
        } else if (!(pak->flags & PAK_ABORT)) {
	    protocol_discard(pak, TRUE);
        }
	reset_interrupt_level(status);
    } else
    	datagram_done(pak);
}

void vrif_err (tr_vpeertype *vrp, tr_vhdrtype *vrh, char *label)
{
    hwidbtype *idb;

    idb = vrp->outidb;
    if (srb_debug || srbevent_debug || srberr_debug)
	errmsg(&msgsym(IFERR, RSRB), label, vr_op2str(vrh->op),
	       idb->hw_namestring, vrh->op, vrh->len, vrh->trn);
    switch (vrh->op) {
      case RSRB_OP_UNKVRN:		/* otherside is bitching */
      case RSRB_OP_UNKPEER:		/* about what we are sending */
      case RSRB_OP_UNKVER:
	rsrb_newstate(vrp, RSRB_PS_DEAD);
	break;
      default:
	break;
    }
}

void vrif_send_punt (tr_vpeertype *vrp, ushort op, tr_vhdrtype *vrh,
		     char *label)
{
    char peerid[MAX_PEER_NAME];
    paktype *pak;

    pak = vrpak(vrh->rhdr_version, op, vrh->trn, vrh->vrn, 0, 0);
    if (pak && vrif_direct(vrp, pak)) {
	if (srb_debug || srbevent_debug || srberr_debug)
	    errmsg(&msgsym(SENDPUNTIF, RSRB), label, vr_op2str(op),
		   vrpeer_id(vrp, peerid));
	return;
    }
    if (srb_debug || srbevent_debug || srberr_debug)
	buginf("\nRSRB: %s: send punt, couldn't send%s %s to %s",
	       label, (pak ? " (no pak)" : ""), vr_op2str(op),
	       vrpeer_id(vrp, peerid));
}

/*
 * vrif_input
 * process direct input from an interface
 */
static ushort find_dlci (paktype *pak)
{
    ushort dlci;
    idbtype *idb = pak->if_input;
    fr_idb_struct_type *fr_idb;
     
    fr_idb = idb->hwptr->frame_relay_stuff;
    dlci = DLCI_TO_NUM(*((ushort *)pak->datagramstart));
    
    return dlci;
}


void vrif_input (paktype *pak)
{
    idbtype *swidb;
    hwidbtype *idb, *outidb;
    hwidbtype *hwidb;
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    tr_vhdrtype *vrh;
    leveltype status;
    tr_ventrytype *vre;
    trrif_t *trh;
    char peerid[MAX_PEER_NAME];
    boolean gen_crc = FALSE;
    srbrd_t *rd;
    int riflen;
    ushort dlci;
    sap_hdr *llc1;
    uchar macaddr[IEEEBYTES];

    swidb = pak->if_input;
    idb = swidb->hwptr;
    vrp = idb->tr_ifpeer;

    pak->acct_proto = ACCT_PROTO_SRB;
    idb_increment_rx_proto_counts(pak, ACCT_PROTO_SRB);

    if (!vrp) {
	vr_nukepak(pak);
	return;
    }

    /*
     * If LAN transport is used, find the proper vrp first
     * before updating last_keepalive. CSCdi12749
     */
    if (vrp->type == RSRB_PT_LAN) {
	find_802addr(pak, macaddr, GET_SOURCE);
	for ( ; vrp; vrp = vrp->idbnext) {
	    if (ieee_equal(vrp->macaddr, macaddr))
		break;
	}
    } else if (vrp->type == RSRB_PT_FR) {
	dlci = find_dlci(pak);
	for ( ; vrp; vrp = vrp->idbnext) {
	    if (vrp->dlci == dlci)
		break;
	}
    }

    if (!vrp) {
	vr_nukepak(pak);
	return;
    }
    /*
     * Don't tear anything down if we are hearing anything.
     * Also mark the packet as having come from this peer.
     */
    GET_TIMESTAMP(vrp->last_keepalive);
    pak->peer_ptr = vrp;

    vrg = (vrp ? vrp->vrg : NULL);      /* just to be kosher */
    if (!vrp || !vrg) {
        if (srb_debug || srberr_debug || srbevent_debug)
            buginf("\nRSRB: IF: no peer or vrn found, dropping packet");
	vr_nukepak(pak);
	return;
    }
    vrh = (tr_vhdrtype *) pak->network_start;
    if (vrh->op & RSRB_OP_ERR) {
	vrif_err(vrp, vrh, "IFin");
	vr_nukepak(pak);
	return;
    }
    if (vrg->vrn != vrh->vrn) {
	vrif_send_punt(vrp, RSRB_OP_UNKVRN, vrh, "IFin");
	vr_nukepak(pak);
	return;
    }
    if (vrp->vrp_starting_up) {
        /*
         * first time through here?  Yep.  Need to do start up
         * kinds of things.  vrif doesn't check for proper
         * sequenceing.
         *
         * Display a connection start up message
         * Do version coercing.
         * Do Caps Exchange if supported.
         */
        if (srb_debug || srbevent_debug)
            buginf("\nRSRB: IFin: connecting, %s from %s (ver %d)", vr_op2str(vrh->op),
                   vrpeer_id(vrp, peerid), vrh->rhdr_version);
        if (!vr_version_reasonable(vrp, vrh, "IFin")) {
            vrif_send_punt(vrp, RSRB_OP_UNKVER, vrh, "IFin");
            (*vrp->abort)(vrp);
            vr_nukepak(pak);
            return;
        }
        if (vrp->vrp_version >= RSRB_CAP_VERSION)
            if (!vrsend_caps(vrp, RSRB_OP_CAPS_REQ, RSRB_OUR_CAPS, "IFin")) {
                (*vrp->abort)(vrp);
                vr_nukepak(pak);
                return;
            }
        vrp->vrp_starting_up = FALSE;
    }

    status = raise_interrupt_level(NETS_DISABLE);
    if (vrp->state != RSRB_PS_CONNECTED) {
	switch (vrp->state) {
	  case RSRB_PS_DEAD:
	  case RSRB_PS_CLOSED:
	  case RSRB_PS_OPENING:
	  case RSRB_PS_OPENWAIT:
	    if ((vrh->op != RSRB_OP_RINGXREQ) &&
                (vrh->op != RSRB_OP_RINGXREPLY) &&
		(vrh->op != RSRB_OP_VERSIONXREQ)) {
		if (!vrsend_xchg(vrp, RSRB_OP_RINGXREQ, "IFin", TRUE)) {
		    rsrb_newstate(vrp, RSRB_PS_DEAD);
		    vr_nukepak(pak);
		    reset_interrupt_level(status);
		    return;
		}
                break;
	    }
	    if (vrp->state == RSRB_PS_DEAD ||
		vrp->state == RSRB_PS_CLOSED ||
		vrp->state == RSRB_PS_OPENING)
		rsrb_newstate(vrp, RSRB_PS_CONNECTED);
	    else
		rsrb_newstate(vrp, RSRB_PS_REMOPENED);
	    break;

	  case RSRB_PS_REMWAIT:
	    if ((vrh->op != RSRB_OP_RINGXREQ) &&
		(vrh->op != RSRB_OP_RINGXREPLY) &&
		(vrh->op != RSRB_OP_VERSIONXREQ)) {
		if (!vrsend_xchg(vrp, RSRB_OP_RINGXREQ, "IFin", TRUE)) {
                    (*vrp->abort)(vrp);
		    vr_nukepak(pak);
		    reset_interrupt_level(status);
		    return;
		}
                break;
	    }
	    rsrb_newstate(vrp, RSRB_PS_REMOPENED);
	    break;
	  case RSRB_PS_REMOPENED:	/* connector running */
	  case RSRB_PS_DRAINING:	/* connector running */
	    break;
	  case RSRB_PS_REMGO:
	  case RSRB_PS_CONNECTED:	/* shouldn't be here */
	  default:
	    errmsg(&msgsym(CONNSTAT, RSRB),
		   vrpeer_id(vrp, peerid), vrp->state);
            (*vrp->abort)(vrp);
	    vr_nukepak(pak);
	    reset_interrupt_level(status);
	    return;
	}
    }
    reset_interrupt_level(status);		/* done with critical region */
    /*
     * process the incoming packet
     */
    if (srb_debug)
	buginf("\nRSRB: IFin: %s %s trn %d, vrn %d, off %d, len %d",
	       vrpeer_id(vrp, peerid), vr_op2str(vrh->op),
	       vrh->trn, vrh->vrn, vrh->offset, vrh->len);
    if (vrh->len == 0) {
        /*
         * Only check these ops, because they are the only ones that the IF
         * peer can have that require lenghts.
         */
        switch (vrh->op) {
          case RSRB_OP_FORWARD:
          case RSRB_OP_FORWARDCRC:
          case RSRB_OP_EXPLORER:
            errmsg(&msgsym(BADLEN, RSRB), vrpeer_id(vrp, peerid), "IFin",
                   vrh->len, vrh->trn);
            vr_nukepak(pak);
            return;
          default:
            break;
        }
    }
    pak->datagramstart = (uchar *) vrh + sizeof(tr_vhdrtype);
    pak->datagramsize = vrh->len;
    trh = (trrif_t *) pak->datagramstart;
    switch (vrh->op) {
      case RSRB_OP_FORWARDCRC:
	gen_crc = TRUE;
	pak->flags |= PAK_SRB_EXPLORER;
	/* Note the fall through */
      case RSRB_OP_FORWARD:
	if ((trh->saddr[0] & TR_RII) == 0) {
 	    vre = vr_virtualp(vrg, 0, trh->daddr);
 	    if (vre || (GETSHORT(trh->daddr) == TRFUNC_UPPER)) {
                srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, 
                                        pak->datagramsize);
                vr_vre_input(vre, pak, FALSE, vrh->op);
	    } else {
		vrp->drops++;
		if (srbevent_debug || srberr_debug)
		    buginf("\nRSRB: virtual forward not for us (NO RII)");
		vr_nukepak(pak);
	    }
	    return;
	}
	if (RIF_DEST_RING(trh) == vrg->vrn) {
 	    vre = vr_virtualp(vrg, 0, trh->daddr);
 	    if (vre) {
                srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, 
                                        pak->datagramsize);
                vr_vre_input(vre, pak, (gen_crc ? FALSE : TRUE), vrh->op);
	    } else {
		vrp->drops++;
		if (srbevent_debug || srberr_debug)
		    buginf("\nRSRB: virtual forward not for us (terminal ring not found)");
		vr_nukepak(pak);
	    }
	    return;
	}
	vre = vrfind_rn2entry(vrh->trn, RSRB_BN_DONTCARE,
			      (tr_ventrytype *)&vrg->rings.qhead, RSRB_ET_LOCAL);
	if (!vre) {
	    vrif_send_punt(vrp, RSRB_OP_UNKTRN, vrh, "IFin");
	    vrp->drops++;
	    vr_nukepak(pak);
	    return;
	}

	/*
	 * Going to receive it.  Count it for LNM.
	 */
        srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, pak->datagramsize);
	vre->forwards++;
	vrp->pkts_rx++;
        vrp->bytes_rx += pak->datagramsize;
        
	/*
	 * Is this a frame for the SDLLC/QLLC ring?
	 */
	if (vre->type & ( RSRB_ET_SDLLC | RSRB_ET_QLLC | RSRB_ET_TR_LANE) ) {
            vr_vre_input(vre, pak, (gen_crc ? FALSE : TRUE), vrh->op);
	    return;
	}
	if (!gen_crc) {
	    pak->datagramsize -= TR_FCS_SIZE;
	    pak->flags |= PAK_SRB_OUT;
	}
	pak->enctype = ET_NULL;
	outidb = vre->outhole.idb;

	if (!srb_out_check((uchar *) pak->datagramstart, pak->datagramsize,
			  outidb, (ushort) (trh->rc_blth & RC_LTHMASK))) {
	    vr_nukepak(pak);
	    return;
	}

	if (trh->rc_blth & RC_TRACE_FRAME)
	    reg_invoke_send_trace_report(pak, NULL, outidb);
	pak->if_output = outidb->firstsw;
	pak->linktype = LINK_RSRB;
        srb_sb_incr_tx_nb_counters(outidb->firstsw, pak->datagramsize);
	riflen = trh->rc_blth & RC_LTHMASK;
	llc1 = (sap_hdr *)((uchar *)&trh->rc_blth + riflen);
	if (((llc1->control & ~LLC1_P) == LLC1_XID) ||
	     ((llc1->control & ~LLC1_P) == LLC1_TEST)) {
	    rif_update(NULL, vrg->vrn, trh->saddr, NULL,
		       (srbroute_t *) &trh->rc_blth, RIF_AGED_REM);
	}
	if (global_netbios_name_cache_enable) {
	    netbios_name_cache_update_pak((uchar *) trh, NETBIOS_NAME_AGED_REM,
					  NULL, vrg->vrn);
	}
	tr_to_media_out(pak);
	return;

      case RSRB_OP_EXPLORER:
	/*
	 * Is it one of our "virtual" addresses?
	 */
	trh->rc_blth -= 2;
	if (RIF_DEST_RING(trh) == vrg->vrn) {
 	    vre = vr_virtualp(vrg, 0, trh->daddr);
 	    if (vre) {
		if ( !(vre->type &  ( RSRB_ET_SDLLC | RSRB_ET_QLLC | RSRB_ET_LOCAL_VIRTUAL) ) ) {
		    pak->datagramsize -= 2;
		    bcopy(((uchar *) &(trh->rd[((trh->rc_blth &
						RC_LTHMASK) - 2) / 2 + 1])),
			  ((uchar *) &(trh->rd[((trh->rc_blth &
						RC_LTHMASK) - 2) / 2])),
			  pak->datagramsize);
		} else {
		    trh->rc_blth += 2;
		    rd = (srbrd_t *) ((uchar *) trh + vrh->offset);
		    PUTSHORT(rd, ((GETSHORT(rd) & RD_RNMASK) | (vre->bn & RD_BNMASK)));
		    PUTSHORT(++rd, ((vre->rn << RD_RNSHIFT) & RD_RNMASK)); 
		    /* CSCdi29807 - clear the bridge field */ 
		}
		vr_vre_input(vre, pak, FALSE, vrh->op);
		return;
	    }
	}
	trh->rc_blth += 2;
	if (global_proxy_explorer_enable) {
	    trh->rc_blth -= 2;
	    rif_update(NULL, vrg->vrn, trh->saddr, NULL,
		       (srbroute_t *)&trh->rc_blth, RIF_AGED_REM);
	    trh->rc_blth += 2;
	    if (global_netbios_name_cache_enable) {
		netbios_name_cache_update_pak((uchar *) trh,
					      NETBIOS_NAME_AGED_REM, NULL,
					      vrg->vrn);
	    }
	}
	/* SRB AST support */
        /*
         * If this is a STE and this vidb is not in spanning tree forwarding
         * state, then drop it.
         */
        if ((trh->rc_blth & RC_RTMASK) == RC_B_SPAN) {
            if (!rsrb_vidb_span_forward(vrg, vrp)) {
                vr_nukepak(pak);
                return;
            }
        }
        /* Mark this packet and put it on the explorer queue. */
        pak->flags |= PAK_RSRB_EXPLORER;
        pak->rif_match = pak->datagramstart + vrh->offset;
        srb_enq(srb_explorerQ, pak);
	return;

      case RSRB_OP_VERSIONREQ:
      case RSRB_OP_VERSIONXREQ:
        if (srb_debug || srbevent_debug)
            buginf("\nRSRB: IFin: %s %s -> %s",
                   vr_op2str(vrh->op), vr_op2str(RSRB_OP_VERSIONREPLY),
                   vrpeer_id(vrp, peerid));
        vrsend_version(vrp, RSRB_OP_VERSIONREPLY, DONT_ABORT_ON_FAIL);
        if (vrh->op == RSRB_OP_VERSIONREQ)
            break;

        /* fall through */

      case RSRB_OP_RINGXREQ:
	if (!vrsend_xchg(vrp, RSRB_OP_RINGXREPLY, "IFin", TRUE)) {
            (*vrp->abort)(vrp);
	    return;
	}
	/*
	 * fall through and process data
	 */

      case RSRB_OP_RINGXREPLY:
	pak->peer_ptr = vrp;
	srb_enq(vrxchgQ, pak);
        return;

      case RSRB_OP_NOP:
        break;                          /* to dispose of packet */

      case RSRB_OP_VERSIONREPLY:
        if (srb_debug || srbevent_debug)
            buginf("\nRSRB: IFin: Received %s from %s (version %d)",
                   vr_op2str(vrh->op), vrpeer_id(vrp, peerid), vrh->rhdr_version);
        break;

      case RSRB_OP_BPDU:
      case RSRB_OP_SPAN:
        if (!vrg->span_idb) {
            /* This end is not running spanning tree */
            vr_nukepak(pak);
	    return;
	}
	if (vrh->op == RSRB_OP_SPAN) {
	    vrg->span_idb->disjoint_peers = TRUE;
	}
	clear_if_input(pak);
        pak->if_input = vrg->span_idb;
	pak->peer_ptr = vrp;
	hwidb = hwidb_or_null(vrg->span_idb);
	if (!hwidb) {
	    vr_nukepak(pak);
	    return;
	}
        (*hwidb->iqueue)(hwidb, pak);
        return;

      case RSRB_OP_CAPS_REQ:
        vrsend_caps(vrp, RSRB_OP_CAPS_REPLY, RSRB_OUR_CAPS, "IFin");

        /* fall through */

      case RSRB_OP_CAPS_REPLY:
        vrp->vrp_capabilities = vrh->param;
        break;

      default:
	if (srb_debug || srbevent_debug)
	    buginf("\nRSRB: IFin: bad op %d (%s) from %s",
		   vrh->op, vr_op2str(vrh->op), vrpeer_id(vrp, peerid));
	vrif_send_punt(vrp, RSRB_OP_BADOP, vrh, "IFin");
        break;
    }
    vr_nukepak(pak);
    return;
}


/********************************************************************
 *
 * Raw Interface: RSRB Implementation
 * Peer Vectors
 *
 ********************************************************************/

/*
 * vrif_sender
 * This is the sender for the raw interface implementation.
 */

boolean vrif_sender (tr_vpeertype *peer, tr_vhdrtype *hdr, paktype *pak)
{
    leveltype status;
    char peerid[MAX_PEER_NAME];
    boolean keep;
    tr_vhdrtype *vhdr;
    ushort vhdr_op;

    if (hdr) {
	if ((pak->datagramstart - pak->data_area) >= sizeof(tr_vhdrtype)) {
	    pak->datagramstart -= sizeof(tr_vhdrtype);
	    pak->datagramsize += sizeof(tr_vhdrtype);
	    bcopy((uchar *) hdr, pak->datagramstart, sizeof(tr_vhdrtype));
	} else {			/* no room */
	    if (srb_debug || srbevent_debug || srberr_debug)
		buginf("\nRSRB: vrif_sender, hdr won't fit");
	    vr_nukepak(pak);
	    return(FALSE);
	}
    }
    status = raise_interrupt_level(NETS_DISABLE);
   
    /* if interface not up, change peer state, discard pak and return */
    if (!hw_interface_up(peer->outidb)) {
       if ((peer->state != RSRB_PS_DEAD) && (peer->state != RSRB_PS_CLOSED))
          rsrb_newstate(peer, RSRB_PS_DEAD);
       vr_nukepak(pak);
       peer->drops++;
       reset_interrupt_level(status);
       if (srberr_debug)
	   buginf("\nRSRB IF: outgoing interface not up, dropping packet");
       return(FALSE);
    }
       
    if (peer->state == RSRB_PS_CONNECTED) {  /* most common case */
	reset_interrupt_level(status);
	return(vrif_direct(peer, pak));
    }
    switch (peer->state) {
      case RSRB_PS_CLOSED:
	rsrb_newstate(peer, RSRB_PS_OPENING);
	keep = TRUE;
	break;
      case RSRB_PS_OPENING:
	if (vrconn_pid == 0 && process_is_queue_empty(vrconn_pakQ) &&
	    QUEUEEMPTY(&vrconnQ)) {
	    if (srb_debug || srbevent_debug || srberr_debug)
		buginf("\nRSRB: Stuck in opening state");
	    rsrb_newstate(peer, RSRB_PS_CLOSED);
	}
      case RSRB_PS_OPENWAIT:
      case RSRB_PS_REMWAIT:
      case RSRB_PS_REMGO:
      case RSRB_PS_REMOPENED:
      case RSRB_PS_DRAINING:
	vhdr = (tr_vhdrtype *)pak->network_start;
	vhdr_op = GETSHORT(&vhdr->op);
	keep = (vhdr && (vhdr_op != RSRB_OP_FORWARD) &&
		(vhdr_op != RSRB_OP_EXPLORER) &&
	        (vhdr_op != RSRB_OP_FORWARDCRC));
	break;

      default:
	errmsg(&msgsym(ILLPEER, RSRB), vrpeer_id(peer, peerid), peer,
	       peer->state);
	/*
	 * Fall through
	 */
      case RSRB_PS_DEAD:
	keep = FALSE;
	break;
    }
    reset_interrupt_level(status);
    if (!keep) {
        if (srb_debug || srbevent_debug)
	    buginf("\nRSRB: IF wrong state to keep");
	vr_nukepak(pak);
	peer->drops++;
	return(FALSE);
    }
    pak->peer_ptr = peer;
    srb_enq(vrconn_pakQ, pak);
    return(TRUE);
}

/*
 * vrif_direct
 * Just send a packet directly.  No state checks.  datagramsize and
 * datagramstart are asssumed to be set up correctly..
 */

boolean vrif_direct (tr_vpeertype *peer, paktype *pak)
{
    hwidbtype *idb;
    ushort dlci = 0;

    /*
     * What's the point of sending anything to peer if srb is not running?
     * CSCdi12749
     */
    if (!srb_running) {
        vr_nukepak(pak);
        if (srbevent_debug)
            buginf("\nRSRB IF: SRB not running yet, dropping packet");
        return(FALSE);
    }

    idb = peer->outidb;
    pak->mac_start = NULL;		/* no MAC header */
    if (peer->type == RSRB_PT_FR) { 
        pak->if_output = peer->outswidb; 
        if (idb->maxmtu < (pak->datagramsize + FRAME_RELAY_SNAPENCAPBYTES )) { 
            if (srbevent_debug || srb_debug || srberr_debug) { 
                buginf("\n RSRB FR: Dropping packet, MTU size on FR interface too small");
                buginf("\n RSRB FR: MTU size %d , packet size %d \n",idb->maxmtu,
                                   (pak->datagramsize + FRAME_RELAY_SNAPENCAPBYTES ));
            } 
            vr_nukepak(pak);
            peer->drops++;
            return(FALSE);
	}
    }
    else
        pak->if_output = idb->firstsw;
    pak->linktype = LINK_RSRB;
    pak->acct_proto = ACCT_PROTO_SRB;

    if (peer->type == RSRB_PT_FR)
	dlci = peer->dlci;
    else
	dlci = 1;

    if (idb->vencap &&
	(*idb->vencap)(pak, ((idb->status & IDB_IEEEMASK) ?
			     (ulong) peer->macaddr : (ulong) dlci))) {
	peer->pkts_tx++;
        peer->bytes_tx += pak->datagramsize;
	datagram_out(pak);
	return(TRUE);
    } else {
       if (srbevent_debug)
	   buginf("\nRSRB vencap failed, dropping packet");
	vr_nukepak(pak);
	peer->drops++;
    }
    return(FALSE);
}

/*
 * vrif_abort
 * Abort a direct interface peer.  This doesn't actually do the
 * destruction of the data structure.  But it is responsible for tearing
 * down the communications link.
 */

void vrif_abort (tr_vpeertype *peer)
{
    rsrb_newstate(peer, RSRB_PS_DEAD);
}

/*
 * vrif_close
 * Close the transport connection.  Accept no further input but
 * allow any output to be physically transmitted.
 */

void vrif_close (tr_vpeertype *peer, ulong param)
{
    rsrb_newstate(peer, RSRB_PS_CLOSED);
}

/*
 * vrif_connect
 * establish a connection to a remote peer via a direct interface.
 */

vpeer_connrtn vrif_connect (tr_vpeertype *vrp, ushort vrn)
{
    char peerid[MAX_PEER_NAME];
    tr_vgrouptype *vrg;
    leveltype status;

    vrg = vrp->vrg;
    if (((vrp->type != RSRB_PT_IF) && (vrp->type != RSRB_PT_LAN) &&
	  (vrp->type != RSRB_PT_FR)) || (vrg->vrn != vrn)) {
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: CONN: connect pointer invalid (if)");
        return(RSRB_CONN_ABORTED);
    }
    vrp->retries = RSRB_MAX_OPEN_TRIES;
    while (vrp->retries--) {
	if (srb_debug || srbevent_debug)
	    buginf("\nRSRB: CONN: opening peer %s, %d",
		   vrpeer_id(vrp, peerid), vrp->retries);

	status = raise_interrupt_level(NETS_DISABLE);
	if (vrp->state == RSRB_PS_OPENWAIT) /* avoid mucking with */
	    rsrb_newstate(vrp, RSRB_PS_REMWAIT);   /* a remote open */
	reset_interrupt_level(status);
        vrsend_version(vrp, RSRB_OP_VERSIONXREQ, ABORT_ON_FAIL);
	TIMER_START(vrconn_wait, vrconn_timeout);
	edisms((blockproc *)vrconnectorBLOCK, (ulong)vrp);
	if ((vrg != vrfind_ringgroup(vrn)) || /* went bye bye? */
	      !rsrb_checkqueue(&vrg->peers, vrp) ||
	      vrp->vrg != vrg || 
	      ((vrp->type != RSRB_PT_IF) && (vrp->type != RSRB_PT_LAN) &&
	       (vrp->type != RSRB_PT_FR))) {
            if (srb_debug || srbevent_debug || srberr_debug)
                buginf("\nRSRB: CONN: consistency abort %s on open", vrpeer_id(vrp, peerid));
            return(RSRB_CONN_ABORTED);
	}
	switch (vrp->state) {
	  case RSRB_PS_OPENWAIT: /* Fallout from above... */
	    break;
	  case RSRB_PS_REMWAIT:	/* timeout */
            if (vrp->retries == 0)
		if (srb_debug || srbevent_debug || srberr_debug)
		    buginf("\nRSRB: CONN: %s timed out",
			   vrpeer_id(vrp, peerid));
	    rsrb_newstate(vrp, RSRB_PS_OPENWAIT);
	    break;
	  case RSRB_PS_REMOPENED:
	    return(RSRB_CONN_CONNECTED);
	  case RSRB_PS_CLOSED:
            if (srb_debug || srbevent_debug || srberr_debug)
                buginf("\nRSRB: CONN: Remote closed %s on open", vrpeer_id(vrp, peerid));
            if (vrp->retries == 0)
                return(RSRB_CONN_FAILED);
            rsrb_newstate(vrp, RSRB_PS_OPENWAIT);
	    break;
	  case RSRB_PS_DEAD:
            if (srb_debug || srbevent_debug || srberr_debug)
                buginf("\nRSRB: CONN: Local peer went dead state while connecting %s",
		       vrpeer_id(vrp, peerid));
	    return(RSRB_CONN_FAILED);
	  default:
	    errmsg(&msgsym(CONNILLSTATE, RSRB), vrpeer_id(vrp, peerid),
		   vrp->state);
	    return(RSRB_CONN_FAILED);
	}
    }
    if ((srb_debug || srbevent_debug || srberr_debug) && vrp->state != RSRB_PS_CLOSED)
	buginf("\nRSRB: CONN: peer %s open failed",
	       vrpeer_id(vrp, peerid));
    return(RSRB_CONN_FAILED);
}

/************************************************************************
 * RSRB Drivers: FST Implementation
 ************************************************************************/

void vrfst_err (tr_vpeertype *vrp, tr_vhdrtype *vrh, char *label)
{

    if (srb_debug || srbevent_debug || srberr_debug)
	errmsg(&msgsym(FSTERR, RSRB), label, vr_op2str(vrh->op),
	       vrp->ipaddr, vrh->op, vrh->len, vrh->trn);
    switch (vrh->op) {
      case RSRB_OP_UNKVRN:		/* otherside is bitching */
      case RSRB_OP_UNKPEER:		/* about what we are sending */
      case RSRB_OP_UNKVER:
	rsrb_newstate(vrp, RSRB_PS_DEAD);
	break;
      default:
	break;
    }
}

void vrfst_send_punt (tr_vpeertype *vrp, ushort op, tr_vhdrtype *vrh,
		      char *label)
{
    char peerid[MAX_PEER_NAME];
    paktype *pak;

    pak = vrpak(vrh->rhdr_version, op, vrh->trn, vrh->vrn, 0, 0);
    if (pak && vrfst_direct(vrp, pak)) {
	if (srb_debug || srbevent_debug || srberr_debug)
	    errmsg(&msgsym(SENDPUNTFST, RSRB), label, vr_op2str(op),
		   vrpeer_id(vrp, peerid));
	return;
    }
    if (srb_debug || srbevent_debug || srberr_debug)
	buginf("\nRSRB: %s: send punt, couldn't send%s %s to %s",
	       label, (pak ? " (no pak)" : ""), vr_op2str(op),
	       vrpeer_id(vrp, peerid));
}

/*
 * vrfst_rsrb_input
 * process input from a FST Peer. datagramstart points to start of IP header
 */

void vrfst_input (paktype *pak)
{
    iphdrtype *ip;
    hwidbtype *outidb, *hwidb;
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    tr_vhdrtype *vrh;
    leveltype status;
    tr_ventrytype *vre;
    trrif_t *trh;
    char peerid[MAX_PEER_NAME];
    boolean gen_crc = FALSE;
    srbrd_t *rd;
    int riflen, rx_size;
    sap_hdr *llc1;
    boolean ok_seq;

    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * If reassembly needed, do it here.
     */
    if ((ip->morefragments) || (getfo(ip) != 0)) {
	paktype *tmp;

	tmp = reassemble(pak);
	if (tmp == NULL)
	    return;			/* return now if it was a fragment */
	pak = tmp;
    }

    pak->acct_proto = ACCT_PROTO_SRB;

    vrh = (tr_vhdrtype *) ((uchar *) ipdatastart(pak));
    vrg = vrfind_ringgroup(vrh->vrn);
    vrp = vrfind_peer(vrg, RSRB_PT_FST, (rsrb_cookie) ip->srcadr, 0, 0, NULL);
    if (!vrp || !vrg) {
        if (srb_debug || srberr_debug || srbevent_debug)
            buginf("\nRSRB: FSTin: no peer or vrn found, dropping packet");
	vr_nukepak(pak);
	return;
    }
    /*
     * Don't tear anything down if we are hearing anything.
     * Also mark this packet as having come from this peer.
     */
    GET_TIMESTAMP(vrp->last_keepalive);
    pak->peer_ptr = vrp;

    if (vrh->op & RSRB_OP_ERR) {
	vrfst_err(vrp, vrh, "FSTin");
	vr_nukepak(pak);
	return;
    }
    if (vrg->vrn != vrh->vrn) {
	vrfst_send_punt(vrp, RSRB_OP_UNKVRN, vrh, "FSTin");
	vr_nukepak(pak);
	return;
    }

    if (vrp->vrp_starting_up) {
        /*
         * first time through here?  Yep.  Need to do start up
         * kinds of things.  vrif doesn't check for proper
         * sequenceing.
         *
         * Display a connection start up message
         * Do version coercing.
         * Do Caps Exchange if supported.
         */
        if (srb_debug || srbevent_debug)
            buginf("\nRSRB: FSTin: connecting, %s from %s (ver %d)", vr_op2str(vrh->op),
                   vrpeer_id(vrp, peerid), vrh->rhdr_version);
        if (!vr_version_reasonable(vrp, vrh, "FSTin")) {
            vrfst_send_punt(vrp, RSRB_OP_UNKVER, vrh, "FSTin");
            (*vrp->abort)(vrp);
            vr_nukepak(pak);
            return;
        }
        if (vrp->vrp_version >= RSRB_CAP_VERSION)
            if (!vrsend_caps(vrp, RSRB_OP_CAPS_REQ, RSRB_OUR_CAPS, "FSTin")) {
                (*vrp->abort)(vrp);
                vr_nukepak(pak);
                return;
            }
        vrp->vrp_starting_up = FALSE;
    }

    if (vrh->rhdr_version != vrp->vrp_version) {
        if (srb_debug || srbevent_debug || srberr_debug)
            errmsg(&msgsym(BADVERSIONFST, RSRB), vrpeer_id(vrp, peerid),
                   vrp->vrp_version, vrh->rhdr_version);
        (*vrp->abort)(vrp);
        vr_nukepak(pak);
        return;
    }

    /* Do sequence number work */
    switch (vrh->op) {
      case RSRB_OP_FORWARD:
      case RSRB_OP_FORWARDCRC:
      case RSRB_OP_FORWARDFST:
      case RSRB_OP_FORWARDFSTCRC:
      case RSRB_OP_SDLLC:
      case RSRB_OP_QLLC:
      case RSRB_OP_TR_LANE :
	if (ip->id != 0) {
	    status = raise_interrupt_level(NETS_DISABLE);

	    /* Hey, is this packet an okay one to accept? */
	    RSRB_FST_ACCEPTOR(vrp, ip->id, ok_seq)

	    if (!ok_seq) {
                if (srberr_debug)
		    buginf("RSRB FST: bad sequence number dropping.");
		vrp->drops++;
		vr_nukepak(pak);
	    }
	    reset_interrupt_level(status);
	    if (!ok_seq)
		return;
	}
	break;
      default:
	break;
    }

    status = raise_interrupt_level(NETS_DISABLE);
    if (vrp->state != RSRB_PS_CONNECTED) {
	switch (vrp->state) {
	  case RSRB_PS_DEAD:
	  case RSRB_PS_CLOSED:
	  case RSRB_PS_OPENING:
	  case RSRB_PS_OPENWAIT:
            if (vrh->op != RSRB_OP_RINGXREQ &&
                  vrh->op != RSRB_OP_RINGXREPLY &&
                  vrh->op != RSRB_OP_VERSIONXREQ) {
		/* remote thinks I am still open */
		if (!vrsend_xchg(vrp, RSRB_OP_RINGXREQ, "FSTin", FALSE)) {
		    rsrb_newstate(vrp, RSRB_PS_DEAD);
		    vr_nukepak(pak);
		    reset_interrupt_level(status);
		    return;
		}
		/*
		 * Don't change the state yet until ring exchange reply
		 * comes back from remote peer. CSCdi15946
		 */
		break;
	    }
	    if (vrp->state == RSRB_PS_DEAD ||
		vrp->state == RSRB_PS_CLOSED ||
		vrp->state == RSRB_PS_OPENING)
		rsrb_newstate(vrp, RSRB_PS_CONNECTED);
	    else
		rsrb_newstate(vrp, RSRB_PS_REMOPENED);
	    break;

	  case RSRB_PS_REMWAIT:
            if (vrh->op != RSRB_OP_RINGXREQ &&
                  vrh->op != RSRB_OP_RINGXREPLY &&
                  vrh->op != RSRB_OP_VERSIONXREQ) {
		if (!vrsend_xchg(vrp, RSRB_OP_RINGXREQ, "FSTin", FALSE)) {
                    (*vrp->abort)(vrp);
		    vr_nukepak(pak);
		    reset_interrupt_level(status);
		    return;
		}
		/*
		 * Don't change the state yet until ring exchange reply
		 * comes back from remote peer. CSCdi15946
		 */
		break;
	    }
	    rsrb_newstate(vrp, RSRB_PS_REMOPENED);
	    break;
	  case RSRB_PS_REMOPENED:	/* connector running */
	  case RSRB_PS_DRAINING:	/* connector running */
	    break;
	  case RSRB_PS_REMGO:
	  case RSRB_PS_CONNECTED:	/* shouldn't be here */
	  default:
	    errmsg(&msgsym(CONNSTAT, RSRB), vrpeer_id(vrp, peerid),
		   vrp->state);
            (*vrp->abort)(vrp);
	    vr_nukepak(pak);
	    reset_interrupt_level(status);
	    return;
	}
    }
    reset_interrupt_level(status);		/* done with critical region */
    /*
     * Process the incoming packet.  Save a copy of the original pak size.
     */

    rx_size = pak->datagramsize;

    if (srb_debug) {
	if ((vrh->op != RSRB_OP_FORWARDFST)
	    && (vrh->op != RSRB_OP_FORWARDFSTCRC)){
	    buginf("\nRSRB: FSTin: %s %s trn %d, vrn %d, off %d, len %d",
		   vrpeer_id(vrp, peerid), vr_op2str(vrh->op),
		   vrh->trn, vrh->vrn, vrh->offset, vrh->len);
	} else {
	    buginf("\nRSRB: FSTin: %s %s trn %d, vrn %d, off %d seq %d",
		   vrpeer_id(vrp, peerid), vr_op2str(vrh->op),
		   vrh->trn, vrh->vrn, vrh->offset,
		   ip->id);
	}
    }
    switch (vrh->op) {
      case RSRB_OP_FORWARDFST:
      case RSRB_OP_FORWARDFSTCRC:
        pak->datagramstart = (uchar *) vrh + (sizeof(tr_vhdrtype) - 6);
        pak->datagramsize = (ip->tl - (MINIPHEADERBYTES + 10));
        break;

      case RSRB_OP_FORWARD:
      case RSRB_OP_FORWARDCRC:
      case RSRB_OP_EXPLORER:
        if (vrh->len == 0) {
            /*
             * Only check these ops, because they are the only ones that the
	     * FST peer can have that require lenghts.
             */
            errmsg(&msgsym(BADLEN, RSRB), vrpeer_id(vrp, peerid), "FSTin",
                   vrh->len, vrh->trn);
            vr_nukepak(pak);
            return;
        }

        /* fall through */

      default:
        pak->datagramstart = (uchar *) vrh + sizeof(tr_vhdrtype);
        pak->datagramsize = vrh->len;
        break;
    }
    trh = (trrif_t *) pak->datagramstart;
    switch (vrh->op) {
      case RSRB_OP_FORWARDCRC:
      case RSRB_OP_FORWARDFSTCRC:
	gen_crc = TRUE;
	pak->flags |= PAK_SRB_EXPLORER;
	/* Note the fall through */
      case RSRB_OP_FORWARD:
      case RSRB_OP_FORWARDFST:
	if ((trh->saddr[0] & TR_RII) == 0) {
 	    vre = vr_virtualp(vrg, 0, trh->daddr);
 	    if (vre || (GETSHORT(trh->daddr) == TRFUNC_UPPER)) {
		vr_vre_input(vre, pak, FALSE, vrh->op);
	    } else {
                if (srberr_debug)
		    buginf("\nRSRB: couldn't find virtual interface");
		vrp->drops++;
		vr_nukepak(pak);
	    }
	    return;
	}
	if (RIF_DEST_RING(trh) == vrg->vrn) {
 	    vre = vr_virtualp(vrg, 0, trh->daddr);
 	    if (vre)
                vr_vre_input(vre, pak, (gen_crc ? FALSE : TRUE), vrh->op);
	    else {
                if (srberr_debug)
		    buginf("\nRSRB: couldn't find virtual interface");
		vrp->drops++;
		vr_nukepak(pak);
	    }
	    return;
	}
	vre = vrfind_rn2entry(vrh->trn, RSRB_BN_DONTCARE,
			      (tr_ventrytype *)&vrg->rings.qhead, RSRB_ET_LOCAL);
	if (!vre) {
	    vrfst_send_punt(vrp, RSRB_OP_UNKTRN, vrh, "IFin");
	    vrp->drops++;
	    vr_nukepak(pak);
	    return;
	}

  	/*
	 * Going to receive it.  Count it for LNM.
	 */
        srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, pak->datagramsize);
	vre->forwards++;
	vrp->pkts_rx++;
        vrp->bytes_rx += pak->datagramsize;

	/*
	 * Is this a frame for the SDLLC/QLLC ring?
	 */
	if (vre->type & ( RSRB_ET_SDLLC | RSRB_ET_QLLC | RSRB_ET_TR_LANE ) ) {
            vr_vre_input(vre, pak, (gen_crc ? FALSE : TRUE), vrh->op);
	    return;
	}

	/*
         * If packet is headed for a virtual interface try to fastswitch it.
	 */

        if (vre->type == RSRB_ET_LOCAL_VIRTUAL
	&& !vre->outhole.idb->span_bridge_process_force) {

	    outidb = vre->outhole.idb;
	    hwidb  = pak->if_input->hwptr;

	    /* This packet is about to be fastswitched but it has already
	     * been count as process switched.  Adjust the counts. */

	    hwidb->counters.inputs--;
            hwidb->counters.rx_cumbytes -= rx_size;
            
            pak->datagramsize -= TR_FCS_SIZE;
	    pak->if_output     = outidb->firstsw;
	    pak->linktype      = LINK_RSRB;
	    pak->rxtype        = RXTYPE_UNKNOWN;
	    pak->riflen        = trh->rc_blth & RC_LTHMASK;

            if ((*outidb->vbridge_forward)(pak, outidb)) {
		hwidb->counters.inputs_fast[ACCT_PROTO_SRB]++;
		hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB] += pak->datagramsize;
	        GET_TIMESTAMP(hwidb->lastinput);
	    }
            return;
	}

	if (!gen_crc) {
	    pak->datagramsize -= TR_FCS_SIZE;
	    pak->flags |= PAK_SRB_OUT;
	}
	pak->enctype = ET_NULL;
	outidb = vre->outhole.idb;

	if (!srb_out_check((uchar *) pak->datagramstart, pak->datagramsize,
			  outidb, (ushort) (trh->rc_blth & RC_LTHMASK))) {
	    vr_nukepak(pak);
	    return;
	}

	if (trh->rc_blth & RC_TRACE_FRAME)
	    reg_invoke_send_trace_report(pak, NULL, outidb);
	pak->if_output = outidb->firstsw;
	pak->linktype = LINK_RSRB;
        srb_sb_incr_tx_nb_counters(outidb->firstsw, pak->datagramsize);
	riflen = trh->rc_blth & RC_LTHMASK;
	llc1 = (sap_hdr *)((uchar *)&trh->rc_blth + riflen);
	if (((llc1->control & ~LLC1_P) == LLC1_XID) ||
	     ((llc1->control & ~LLC1_P) == LLC1_TEST)) {
	    rif_update(NULL, vrg->vrn, trh->saddr, NULL,
		       (srbroute_t *) &trh->rc_blth, RIF_AGED_REM);
	}
	if (global_netbios_name_cache_enable) {
	    netbios_name_cache_update_pak((uchar *) trh,
					  NETBIOS_NAME_AGED_REM, NULL,
					  vrg->vrn);
	}
	tr_to_media_out(pak);
        return;

      case RSRB_OP_EXPLORER:
	/*
	 * Is it one of our "virtual" addresses?
	 */
	trh->rc_blth -= 2;
	if (RIF_DEST_RING(trh) == vrg->vrn) {
 	    vre = vr_virtualp(vrg, 0, trh->daddr);
 	    if (vre) {
		if ( !(vre->type & ( RSRB_ET_SDLLC | RSRB_ET_QLLC | 
                                     RSRB_ET_TR_LANE | RSRB_ET_LOCAL_VIRTUAL) ) ) {
		    pak->datagramsize -= 2;
		    bcopy(((uchar *) &(trh->rd[((trh->rc_blth &
						RC_LTHMASK) - 2) / 2 + 1])),
			  ((uchar *) &(trh->rd[((trh->rc_blth &
						RC_LTHMASK) - 2) / 2])),
			  pak->datagramsize);
		} else {
		    trh->rc_blth += 2;
		    rd = (srbrd_t *) ((uchar *) trh + vrh->offset);
		    PUTSHORT(rd, ((GETSHORT(rd) & RD_RNMASK) | (vre->bn & RD_BNMASK)));
		    PUTSHORT(++rd, ((vre->rn << RD_RNSHIFT) & RD_RNMASK));
		    /* CSCdi29807 - clear the bridge field */ 
		}
                vr_vre_input(vre, pak, FALSE, vrh->op);
		return;
	    }
	}
	trh->rc_blth += 2;
	if (global_proxy_explorer_enable) {
	    trh->rc_blth -= 2;
	    rif_update(NULL, vrg->vrn, trh->saddr, NULL,
		       (srbroute_t *)&trh->rc_blth, RIF_AGED_REM);
	    trh->rc_blth += 2;
	    if (global_netbios_name_cache_enable) {
		netbios_name_cache_update_pak((uchar *) trh,
					      NETBIOS_NAME_AGED_REM, NULL,
					      vrg->vrn);
	    }
	}
	/* SRB AST support */
        /*
         * If this is a STE and this vidb is not in spanning tree forwarding
         * state, then drop it.
         */
        if ((trh->rc_blth & RC_RTMASK) == RC_B_SPAN) {
            if (!rsrb_vidb_span_forward(vrg, vrp)) {
                vr_nukepak(pak);
                return;
            }
        }

	vrforward_explorer(pak, vrh->offset, vrp, 99);
        return;

      case RSRB_OP_VERSIONREQ:
      case RSRB_OP_VERSIONXREQ:
        if (srb_debug || srbevent_debug)
            buginf("\nRSRB: FSTin: %s %s -> %s",
                   vr_op2str(vrh->op), vr_op2str(RSRB_OP_VERSIONREPLY),
                   vrpeer_id(vrp, peerid));
        vrsend_version(vrp, RSRB_OP_VERSIONREPLY, DONT_ABORT_ON_FAIL);
        if (vrh->op == RSRB_OP_VERSIONREQ)
            break;

        /* fall through */

      case RSRB_OP_RINGXREQ:
	if (!vrsend_xchg(vrp, RSRB_OP_RINGXREPLY, "FSTin", FALSE)) {
            (*vrp->abort)(vrp);
            break;
	}
	/*
	 * fall through and process data
	 */

      case RSRB_OP_RINGXREPLY:
	pak->peer_ptr = vrp;
	srb_enq(vrxchgQ, pak);
        return;

      case RSRB_OP_NOP:
        break;

      case RSRB_OP_BPDU:
      case RSRB_OP_SPAN:
        if (!vrg->span_idb) {
            /* This end is not running spanning tree */
           vr_nukepak(pak);
	   return; 
	}
	if (vrh->op == RSRB_OP_SPAN) {
	    vrg->span_idb->disjoint_peers = TRUE;
	}
	clear_if_input(pak);
        pak->if_input = vrg->span_idb;
	pak->peer_ptr = vrp;
	hwidb = hwidb_or_null(vrg->span_idb);
	if (!hwidb) {
	    vr_nukepak(pak);
	    return;
	}
        (*hwidb->iqueue)(hwidb, pak);
        return;

      case RSRB_OP_VERSIONREPLY:
        if (srb_debug || srbevent_debug)
            buginf("\nRSRB: FSTin: Received %s from %s (version %d)",
                   vr_op2str(vrh->op), vrpeer_id(vrp, peerid), vrh->rhdr_version);
        break;

      case RSRB_OP_CAPS_REQ:
        vrsend_caps(vrp, RSRB_OP_CAPS_REPLY, RSRB_OUR_CAPS, "FSTin");

        /* fall through */

      case RSRB_OP_CAPS_REPLY:
        vrp->vrp_capabilities = vrh->param;
        break;

      default:
        if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: FSTin: bad op %d (%s) from %s",
		   vrh->op, vr_op2str(vrh->op), vrpeer_id(vrp, peerid));
	vrfst_send_punt(vrp, RSRB_OP_BADOP, vrh, "FSTin");
        break;
    }
    vr_nukepak(pak);
    return;
}


/********************************************************************
 *
 * FST: RSRB Implementation
 * Peer Vectors
 *
 ********************************************************************/

/*
 * Duplicate FST frame correctly and efficiently for later encapsulation
 */
paktype *rsrb_fst_full_duplicate (
    paktype *oldpak,
    tr_vhdrtype *hdr,
    boolean long_hdr)
{
    paktype *newpak;

    newpak = input_getbuffer((long_hdr ? RSRB_FST_PACKET_SIZE :
        RSRB_FST_FORWARD_PACKET_SIZE) + oldpak->datagramsize, oldpak->if_input->hwptr);
    if (!newpak) {
	return(NULL);
    }
    if (srb_debug) {
	buginf("\nRSRB: Doing packet copy for FST. Old = %x, new = %x", oldpak,
	       newpak);
    }
    /*
     * Since it has to be copied now, align the packet at the start of
     * the new RSRB header so vrfst_direct can do some decent
     * work (not have to copy the thing twice). Handle both cases: Putting in
     * a header now, and putting it in later.
     */
    if (hdr) {
	newpak->datagramstart = newpak->network_start + IPHEADERBYTES(NOPT);
        if (long_hdr) {
            newpak->datagramsize = oldpak->datagramsize + sizeof(tr_vhdrtype);
            bcopy((uchar *) hdr, newpak->datagramstart, sizeof(tr_vhdrtype));
            bcopy(oldpak->datagramstart,
                &newpak->datagramstart[sizeof(tr_vhdrtype)],
                oldpak->datagramsize);
        } else {
            newpak->datagramsize = (sizeof(tr_vhdrtype) - 6) +
                oldpak->datagramsize;
            bcopy((uchar *) hdr,newpak->datagramstart,
                (sizeof(tr_vhdrtype) - 6));
            bcopy(oldpak->datagramstart,
                &newpak->datagramstart[(sizeof(tr_vhdrtype) - 6)],
                oldpak->datagramsize);
        }
    } else {
	newpak->datagramstart = newpak->network_start + 
	  IPHEADERBYTES(NOPT) + sizeof(tr_vhdrtype);
	newpak->datagramsize = oldpak->datagramsize;
	bcopy(oldpak->datagramstart,
	      &newpak->datagramstart[0],
	      oldpak->datagramsize);
    }
    return(newpak);
}

/*
 * vrfst_sender
 * This is the sender for the FST implementation.
 */

boolean vrfst_sender (tr_vpeertype *peer, tr_vhdrtype *hdr, paktype *pak)
{
    leveltype status;
    char peerid[MAX_PEER_NAME];
    boolean keep;
    tr_vhdrtype *vhdr;
    paktype *newpak;
    idbtype *input_idb;
    ushort vhdr_op;

    input_idb = pak->if_input;
    if (hdr) {
        /*
         * The reason for these change is regardless of the location
         * where pak was copied, there always be at least one copy, so I made
         * it simple and efficient.
         */
        if ((hdr->op != RSRB_OP_FORWARDFST)
            && (hdr->op != RSRB_OP_FORWARDFSTCRC)) {
            newpak = rsrb_fst_full_duplicate(pak, hdr, TRUE);
        } else {
            newpak = rsrb_fst_full_duplicate(pak, hdr, FALSE);
        }
        datagram_done(pak);
        if (!newpak) {
            if (srb_debug || srberr_debug)
                buginf("\nRSRB FST: sender, no memory for combination pak");
            peer->drops++;
            return(FALSE);
        }
        pak = newpak;
    }
    pak->if_input = input_idb;
    status = raise_interrupt_level(NETS_DISABLE);
    if (peer->state == RSRB_PS_CONNECTED) {  /* most common case */
	reset_interrupt_level(status);
	return(vrfst_direct(peer, pak));
    }
    switch (peer->state) {
      case RSRB_PS_CLOSED:
	rsrb_newstate(peer, RSRB_PS_OPENING);
	keep = TRUE;
	break;
      case RSRB_PS_OPENING:
	if (vrconn_pid == 0 && process_is_queue_empty(vrconn_pakQ) &&
	    QUEUEEMPTY(&vrconnQ)) {
	    if (srb_debug || srbevent_debug || srberr_debug)
		buginf("\nRSRB: Stuck in opening state");
	    rsrb_newstate(peer, RSRB_PS_CLOSED);
	}
      case RSRB_PS_OPENWAIT:
      case RSRB_PS_REMWAIT:
      case RSRB_PS_REMGO:
      case RSRB_PS_REMOPENED:
      case RSRB_PS_DRAINING:
         /* point to proper location. i.e., rsrb header, NOT ip header */
	vhdr = (tr_vhdrtype *)pak->datagramstart;
	vhdr_op = GETSHORT(&vhdr->op);
	keep = (vhdr && (vhdr_op != RSRB_OP_FORWARD) 
		&& (vhdr_op != RSRB_OP_EXPLORER)
		&& (vhdr_op != RSRB_OP_FORWARDCRC)
		&& (vhdr_op != RSRB_OP_FORWARDFST)
		&& (vhdr_op != RSRB_OP_FORWARDFSTCRC));
	break;

      default:
	errmsg(&msgsym(ILLPEER, RSRB), vrpeer_id(peer, peerid), peer,
	       peer->state);
	/*
	 * Fall through
	 */
      case RSRB_PS_DEAD:
	keep = FALSE;
	break;
    }
    reset_interrupt_level(status);
    if (!keep) {
        if (srb_debug || srbevent_debug)
	    buginf("\nRSRB FST: wrong state to keep.");
	vr_nukepak(pak);
	peer->drops++;
	return(FALSE);
    }
    pak->peer_ptr = peer;
    srb_enq(vrconn_pakQ, pak);
    return(TRUE);
}

/*
 * vrfst_direct
 * Just send a packet directly.  No state checks.  datagramsize and
 * datagramstart are assumed to be set up correctly..
 */

boolean vrfst_direct (tr_vpeertype *peer, paktype *pak)
{
    ushort ident;
    tr_vhdrtype *vhdr;
    iphdrtype *iphead;
    idbtype *idb;
    paktype *newpak;
    int atint;
    ipcache_rntype *cptr = NULL;

    /*
     * What's the point of sending anything to peer if srb is not running?
     * CSCdi12749
     */
    if (!srb_running) {
        vr_nukepak(pak);
        if (srbevent_debug || srberr_debug)
            buginf("\nRSRB IF: SRB not running yet, dropping packet");
        return(FALSE);
    }

    /*
     * WARNING!!!  The idb WILL BE NULL when the packet is locally sourced.
     * Ie. RSRB header ops.
     */
    idb = pak->if_input;
    vhdr = (tr_vhdrtype *) pak->datagramstart;
    clear_if_input(pak);
    pak->datagramstart -= IPHEADERBYTES(NOPT);
    pak->datagramsize +=  IPHEADERBYTES(NOPT);
    pak->linktype = LINK_RSRB;
    iphead = (iphdrtype *) pak->datagramstart;
    switch (GETSHORT(&vhdr->op)) {
      case RSRB_OP_FORWARD:
      case RSRB_OP_FORWARDCRC:
      case RSRB_OP_FORWARDFSTCRC:
      case RSRB_OP_FORWARDFST:
      case RSRB_OP_SDLLC:
      case RSRB_OP_QLLC:
      case RSRB_OP_TR_LANE:
	atint = get_interrupt_level();
	RSRB_FST_INCREMENT_SEND(peer, atint, ident)
	break;
      default:
	ident = 0;
	break;
    }
    bcopy((uchar *) peer->ip_header, (uchar *) iphead, IPHEADERBYTES(NOPT));
    iphead->id = ident;
    iphead->tl = pak->datagramsize;
    iphead->checksum = 0;			/* clear checksum field */
    pak->desthost = iphead->dstadr;
    iphead->checksum = ipcrc((ushort *) iphead, ltob(iphead->ihl));
    pak->linktype = LINK_IP;
    if (idb) {
      pak->if_input = idb->hwptr->vidb_link->firstsw;
    } 
    /*
     * Guarantee a reasonable likelihood of sending out the same interface
     */
    cptr = ipcache_lookup_fast(peer->ipaddr);
    if (cptr) {
	hwidbtype *hwidb = cptr->idb->hwptr;
	if ((hwidb->status & (IDB_ETHER | IDB_TR | IDB_FDDI)) ||
	    ((hwidb->status & IDB_SERIAL) && (hwidb->enctype == ET_HDLC))) {
	    if (hwidb->status & IDB_FDDI) {
		pak->datagramstart -= cptr->length + 1;
		pak->datagramsize += cptr->length + 1;
		pak->datagramstart[0] = (FDDI_ASYNCHRONOUS_FRAME |
					 FDDI_48BIT_ADDRESS |
					 FDDI_LLC_FRAME |
					 FDDI_PRIORITY_ZERO);
		bcopy(cptr->m.macstring, pak->datagramstart + 1, cptr->length);
	    } else if (hwidb->status & IDB_TR) {
		pak->datagramstart -= cptr->length + 2;
		pak->datagramsize += cptr->length + 2;
		pak->datagramstart[0] = AC_PRI4;
		pak->datagramstart[1] = FC_TYPE_LLC;
		bcopy(cptr->m.macstring, pak->datagramstart + 2, cptr->length);
	    } else {
		pak->datagramstart -= cptr->length;
		pak->datagramsize += cptr->length;
		bcopy(cptr->m.macstring, pak->datagramstart, cptr->length);
		/* CSCdi14377 */
		if (pak->datagramsize < MINETHERSIZE)
		    pak->datagramsize = MINETHERSIZE;
	    }
	    ip_traffic.outputs++;
	    pak->acct_proto = ACCT_PROTO_IP;
	    pak->if_output = cptr->idb;
	    datagram_out(pak);
	    peer->pkts_tx++;
            peer->bytes_tx += pak->datagramsize;
	    return(TRUE);
	}
    } else {
 	if (idb)
	    idb->hwptr->counters.cache_miss[ACCT_PROTO_IP]++;
    }

    /* Sigh. We must slow switch here and allow for normal ip_encap... */

    /* Are we aligned ok with the IP header at the start of data? */
    if (pak->datagramstart == pak_center(pak))
	newpak = pak;
    else {
	newpak = getbuffer(pak->datagramsize);
	if (srb_debug) {
	    buginf("\nRSRB: Doing alignment for FST. Old = %x, new = %x",
		   pak, newpak);
	}
	if (!newpak) {
	    vr_nukepak(pak);
	    peer->drops++;
	    return(FALSE);
	}
	newpak->datagramstart = pak_center(newpak);
	newpak->datagramsize = pak->datagramsize;
	bcopy((uchar *) pak->datagramstart, newpak->datagramstart,
	      pak->datagramsize);
	newpak->linktype = pak->linktype;
	newpak->if_input = pak->if_input;
	newpak->desthost = pak->desthost;
	datagram_done(pak);
    }
    ip_simple_enqueue(newpak);
    peer->pkts_tx++;
    peer->bytes_tx += pak->datagramsize;
    return(TRUE);
}

/*
 * vrfst_abort
 * Abort a FST peer.  This doesn't actually do the
 * destruction of the data structure.  But it is responsible for tearing
 * down the communications link.
 */

void vrfst_abort (tr_vpeertype *peer)
{
    rsrb_newstate(peer, RSRB_PS_DEAD);
}

/*
 * vrfst_close
 * Close the transport connection.  Accept no further input but
 * allow any output to be physically transmitted.
 */

void vrfst_close (tr_vpeertype *peer, ulong param)
{
    rsrb_newstate(peer, RSRB_PS_CLOSED);
}

/*
 * vrfst_connect
 * establish a connection to a remote peer via a FST datagram service.
 */

vpeer_connrtn vrfst_connect (tr_vpeertype *vrp, ushort vrn)
{
    char peerid[MAX_PEER_NAME];
    tr_vgrouptype *vrg;
    leveltype status;

    vrg = vrp->vrg;
    if ((vrp->type != RSRB_PT_FST) || (vrg->vrn != vrn)) {
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: CONN: connect pointer invalid (fst)");
        return(RSRB_CONN_ABORTED);
    }
    vrp->retries = RSRB_MAX_OPEN_TRIES;
    while (vrp->retries--) {
	if (srb_debug || srbevent_debug)
	    buginf("\nRSRB: CONN: opening peer %s, %d",
		   vrpeer_id(vrp, peerid), vrp->retries);

	status = raise_interrupt_level(NETS_DISABLE);
	if (vrp->state == RSRB_PS_OPENWAIT) /* avoid mucking with */
	    rsrb_newstate(vrp, RSRB_PS_REMWAIT);   /* a remote open */
	reset_interrupt_level(status);
        vrsend_version(vrp, RSRB_OP_VERSIONXREQ, ABORT_ON_FAIL);
	TIMER_START(vrconn_wait, vrconn_timeout);
	edisms((blockproc *)vrconnectorBLOCK, (ulong)vrp);
	if ((vrg != vrfind_ringgroup(vrn)) || /* went bye bye? */
	    (!rsrb_checkqueue(&vrg->peers, vrp)) ||
	    (vrp->vrg != vrg) ||
	    (vrp->type != RSRB_PT_FST)) {
             if (srb_debug || srbevent_debug || srberr_debug)
                buginf("\nRSRB: CONN: consistency abort %s on open",
                            vrpeer_id(vrp, peerid));
            return(RSRB_CONN_ABORTED);
	}
	switch (vrp->state) {
	  case RSRB_PS_OPENWAIT: /* Fallout from above... */
	    break;
	  case RSRB_PS_REMWAIT:	/* timeout */
	    /* Handle the ancient case... */
	    if (!vrp->retries) {
		if (srb_debug || srbevent_debug || srberr_debug)
		    buginf("\nRSRB: CONN: %s timed out",
			   vrpeer_id(vrp, peerid));
	    }
	    rsrb_newstate(vrp, RSRB_PS_OPENWAIT);
	    break;
	  case RSRB_PS_REMOPENED:
	    return(RSRB_CONN_CONNECTED);
	  case RSRB_PS_CLOSED:
            if (srb_debug || srbevent_debug || srberr_debug)
                buginf("\nRSRB: CONN: Remote closed %s on open",
                          vrpeer_id(vrp, peerid));
            if (vrp->retries == 0)
                return(RSRB_CONN_FAILED);
            rsrb_newstate(vrp, RSRB_PS_OPENWAIT);
	    break;
	  case RSRB_PS_DEAD:
            if (srb_debug || srbevent_debug || srberr_debug)
                buginf("\nRSRB: CONN: Local peer went dead state while connecting %s",
		       vrpeer_id(vrp, peerid));
	    return(RSRB_CONN_FAILED);
	  default:
	    errmsg(&msgsym(CONNILLSTATE, RSRB), vrpeer_id(vrp, peerid),
		   vrp->state);
	    return(RSRB_CONN_FAILED);
	}
    }
    if ((srb_debug || srbevent_debug || srberr_debug) &&
           vrp->state != RSRB_PS_CLOSED)
	buginf("\nRSRB: CONN: peer %s open failed",
	       vrpeer_id(vrp, peerid));
    return(RSRB_CONN_FAILED);
}

/*
 * Classify a packet for its priority.
 * If it is a FID4 TH do COS prioritization.
 * If it is a FID2 TH do SNA Local LU Address prioritization.
 * If it is an SNA FID2 frame, classify by the local address priority. 
 *
 * Since only SNA FID4 packets have priority bits set, check for LLC2
 * SNA FID4 packets.  If found extract and return the priority, else
 * return default priority.
 */
int rsrb_packet_classify (
    paktype *pak)
{
    tr_vhdrtype  *vrh;
    tring_hdr    *trh;		/* pointer to TR MAC header */
    int      	  riflen = 0;
    sap_hdr  	 *llc1;
    uchar        *frameptr;
    uchar         conch, *fid2th;
    sna_fid4_hdr *sna_hdr;
    ushort vrh_op_temp;


    vrh = (tr_vhdrtype *)(pak->datagramstart);
    vrh_op_temp = GETSHORT(&vrh->op);  /* to correct for alignment error seen i\n rsp & 7200 */

    if ((vrh_op_temp == RSRB_OP_FORWARD) || (vrh_op_temp == RSRB_OP_FORWARDCRC) 
                                      || (vrh_op_temp == RSRB_OP_LLC2DATA)) {

	trh = (tring_hdr *)(((uchar *) vrh) + sizeof(tr_vhdrtype));
         if (trh->saddr[0] & TR_RII)     /* have a rif? get its length */
            riflen = trh->data[0] & RC_LTHMASK;

        frameptr = ((uchar *)trh) + TRING_ENCAPBYTES + riflen;
        llc1     = (sap_hdr *)frameptr;

        /*
	 * First do FID4 COS priorization.
         * Only SNA FID 4 traffic enveloped in LLC2 is prioritized for COS
         * This may need to be modified (possibly by config) to include more
         * than just the base 04 SNA SAP.
         */
         if (((llc1->dsap & ~SAP_GROUP) == SAP_IBM_SNA) ||
	     ((llc1->ssap & ~SAP_GROUP)  == SAP_IBM_SNA)) {

            conch = *(frameptr + 2) & 0xEF;  /* snuff out p/f bit */
            if ((conch & LLC_IMASK) == LLC_IFRAME )
               frameptr += 4;

            sna_hdr = (sna_fid4_hdr *)frameptr;

            if ((rsrb_cos_enable) &&
		((sna_hdr->fid_tg & SNA_FID) == SNA_FID4)) {
		return(sna_hdr->priority & SNA_TPF);
            } else {

	       /* now do SNA FID2 Local LU address prioritization */
		   if(pak->if_input->hwptr->sna_la_prilist) {
               if ((conch & LLC_IMASK) == LLC_IFRAME) {
                    fid2th = frameptr;	
		    if ((*fid2th & SNA_TH_FID_MASK) == SNA_TH_FID2_VALUE) 
			 return(rsrb_fid2_pri_classify(pak->if_input->hwptr, fid2th, trh, llc1));
			}
			}
			if (pak->if_input->hwptr->sap_prilist)
				return(sap_pri_classify(pak->if_input->hwptr, trh, llc1));
		    else
                       return(PRIORITY_NORMAL);
	       } 
		   } else {
		   /* is SAP prioritization set on this interface ?*/
		   if(pak->if_input->hwptr->sap_prilist)
			return(sap_pri_classify(pak->if_input->hwptr, trh, llc1));
	      }
    }
    return(0);
}



/*
 * After getting the priority of a packet, return the peer it is to go out on
 */
tr_vpeertype *rsrb_packet_classify_peer (
    paktype *pak,
    tr_vpeertype *rootpeer)
{
    int priority, i;
    tr_vpeertype *defaultone = rootpeer;

    if (!rootpeer->priority) {
	return(rootpeer);
    } else {
	priority = rsrb_packet_classify(pak);
        if (priority == 0) {
            return(rootpeer);
        } else {
	    for (i = priority; i <= RSRB_PRIORITY_COUNT; i++) {
		if (rootpeer->priority_peers[i]) {
		    return(rootpeer->priority_peers[i]);
		}
	    }
	}
    }
    return(defaultone);
}

/********************************************************************
 *
 * TCPDRIVER: Remote SRB implementation
 *
 ********************************************************************/

/*
 * vrtcpd_sender
 * This is the sender for tcp using the tcpdriver.  If hdr is
 * non-NULL it points to a tr_vhdrtype structure.  If there is
 * room at the front of the packet it will be copied there.  If
 * not a new pak buffer is allocated for it.
 *
 * If the peer isn't open yet the appropriate state is entered
 * and the pak(s) are put on the startq of the peer.  Otherwise,
 * the pak(s) are sent to the tcp driver for output.
 *
 * This routine can be called from anywhere including interrupt level.
 * The routine returns TRUE if it has dealt with in someway pak.  hdr
 * is assumed to be owned by the caller.
 */

boolean vrtcpd_sender (tr_vpeertype *inpeer, tr_vhdrtype *hdr, paktype *pak)
{
    paktype *newpak;
    leveltype status;
    char peerid[MAX_PEER_NAME];
    boolean keep;
    int result;
    tr_vhdrtype *vhdr;
    tr_vpeertype *peer;
    ushort vhdr_op;
    tring_hdr *mac;
    srbroute_t *rifroute;
    char rif_string[SRB_MAXRIF*2+SRB_MAXRIF/2];
    static int ui_overflow_warning_count = 0;

    SAVE_CALLER();

    pak->acct_proto = ACCT_PROTO_SRB;

    if (hdr) {
	if ((pak->datagramstart - pak->data_area) >= sizeof(tr_vhdrtype)) {
	    pak->datagramstart -= sizeof(tr_vhdrtype);
	    pak->datagramsize += sizeof(tr_vhdrtype);
	    bcopy((uchar *) hdr, pak->datagramstart, sizeof(tr_vhdrtype));
	} else {		/* no room */
	    newpak = input_getbuffer(sizeof(tr_vhdrtype) + pak->datagramsize,
				     pak->if_input->hwptr);
	    if (!newpak) {
		vr_nukepak(pak);
		inpeer->drops++;
		return(FALSE);
	    }
	    newpak->datagramstart = newpak->network_start;
	    newpak->datagramsize = sizeof(tr_vhdrtype) + pak->datagramsize;
	    bcopy((uchar *) hdr, newpak->datagramstart, sizeof(tr_vhdrtype));
	    bcopy(pak->datagramstart,
		  &newpak->datagramstart[sizeof(tr_vhdrtype)],
		  pak->datagramsize);
	    vr_nukepak(pak);
	    pak = newpak;
	}
    }

    vhdr = (tr_vhdrtype *) pak->datagramstart;
    if (pak->datagramsize != GETSHORT(&vhdr->len) + sizeof(tr_vhdrtype)) {
	if (srbevent_debug) {
	    buginf("\nRSRB: tcpd_sender, length mismatch in header, lengths: %d %d, callers: %x",
		   pak->datagramsize - sizeof(tr_vhdrtype), 
		   GETSHORT(&vhdr->len), caller());
	    buginf("\nRSRB: tcpd_sender, header %04x %04x %04x %04x %04x %04x %08x",
                   GETSHORT(&vhdr->rhdr_version), GETSHORT(&vhdr->op),
		   GETSHORT(&vhdr->trn), GETSHORT(&vhdr->vrn),
		   GETSHORT(&vhdr->offset), GETSHORT(&vhdr->len), 
		   GETLONG(&vhdr->param));
	}
	vr_nukepak(pak);
	return(FALSE);
    }

    if (inpeer->priority)
	peer = rsrb_packet_classify_peer(pak, inpeer);
    else 
	peer = inpeer;

    if (!peer) {
	if (srbevent_debug || srberr_debug) {
	    buginf("\nRSRB: Cannot find peer for packet with priority %d",
		   rsrb_packet_classify(pak));
	}
	vr_nukepak(pak);
	return(FALSE);
    }
    if (GETSHORT(&vhdr->rhdr_version) != peer->vrp_version) {
	if (srbevent_debug || srberr_debug) {
	    buginf("\nRSRB: tcpd_sender, header smashed, version callers: %x",
		   caller());
	    buginf("\nRSRB: tcpd_sender, header %04x %04x %04x %04x %04x %04x %08x",
		   GETSHORT(&vhdr->rhdr_version), GETSHORT(&vhdr->op), 
		   GETSHORT(&vhdr->trn), GETSHORT(&vhdr->vrn),
		   GETSHORT(&vhdr->offset),
		   GETSHORT(&vhdr->len), GETLONG(&vhdr->param));
	}
	vr_nukepak(pak);
	return(FALSE);
    }
    status = raise_interrupt_level(NETS_DISABLE);
    if (peer->state == RSRB_PS_CONNECTED) { /* most common case */

	result = reg_invoke_ip_tcpdriver_send(peer->t, pak, FALSE);

        /*
         * Temporarily increase tcp queue size to hold I-frames
         * Restore to original queue size when done. This over-queueing is
         * necessary because we have locally acknowledged the packet.
	 * Also, over-queue all local ack packets. We still want to drop 
	 * excessive explorers or rsrb non-lack traffic so the following op
	 * codes are not over-queued.  Note that local-ack I-frames have 
	 * op == RSRB_OP_FORWARD with linktype == LLC2.  
	 */
	if (result == TCPDSEND_QFULL) {
	    if ((pak->linktype == LINK_LLC2) ||
		(hdr->op != RSRB_OP_EXPLORER &&
		 hdr->op != RSRB_OP_FORWARD  &&
		 hdr->op != RSRB_OP_FORWARDCRC)) {
		if( ((tcp_encaps  *)(peer->t))->outputq.count  > 
		   (RSRB_TCP_MAX_UI_FRAME_OVERQUEUE * vring_tcpqmax) )  {
			    
		    if(srberr_debug) {
			/* in order not to flood the system and be responsive on debug, 
		      	 * let the user know  which macs and rif so they can find the 
		       	 * culprits in the network that are flooding us with UI's .
		       	 * after 20 messages stop issuing messages until the 
		       	 * queue returns to below the defined limit of vring_tcpqmax .
		       	 */
		       	if( ui_overflow_warning_count < 20) {

		       	   mac = (tring_hdr *)pak->mac_start;
		       	   rifroute = (srbroute_t *)mac->data;
		       	   ui_overflow_warning_count++;
		       	   buginf("\nRSRB TCPD:tcp queue excessive ui frames for %s,address = 0x%x\n",
		       		  vrpeer_id(peer, peerid),&peer);
		       	   buginf("\nRSRB TCPD:ui frame, s: %e d: %e rif: %s\n",
		       		  mac->saddr, mac->daddr,
		       		  get_rif_str((uchar *)rifroute, rif_string));
		       }
		    }
			
		    vr_nukepak(pak);
		    reset_interrupt_level(status);
		    return(FALSE);
		}
		reg_invoke_ip_tcpdriver_set_oqmax(peer->t, 1 + reg_invoke_ip_tcpdriver_oqlen(peer->t));
		result = reg_invoke_ip_tcpdriver_send(peer->t, pak, FALSE);
		reg_invoke_ip_tcpdriver_set_oqmax(peer->t, vring_tcpqmax);
	    }
	}
	if(srberr_debug) {
	    if( ((tcp_encaps  *)(peer->t))->outputq.count < vring_tcpqmax) 
		ui_overflow_warning_count = 0;
	}
	if (TCPDSEND_ERROR(result)) {
	    reset_interrupt_level(status);
	    if (result == TCPDSEND_CLOSED)
		rsrb_newstate(peer, RSRB_PS_DEAD);
            if (srbevent_debug) {
                if (result == TCPDSEND_QFULL)
                    buginf("\nRSRB TCPD: tcp queue full for %s",
                           vrpeer_id(peer, peerid));
                else
                    buginf("\nRSRB TCPD: tcp send failed for %s %d",
                           vrpeer_id(peer, peerid), result);
            }
	    vr_nukepak(pak);
	    reset_interrupt_level(status);
	    return(FALSE);
	}
	peer->pkts_tx++;
        peer->bytes_tx += pak->datagramsize;
	clear_if_input(pak);
	reset_interrupt_level(status);
	return(TRUE);
    }
    switch (peer->state) {
    case RSRB_PS_CLOSED:
	rsrb_newstate(peer, RSRB_PS_OPENING);
	keep = TRUE;
	break;
    case RSRB_PS_DEAD:
	keep = FALSE;
	break;
    case RSRB_PS_OPENING:
	if (vrconn_pid == 0 && process_is_queue_empty(vrconn_pakQ) &&
	    QUEUEEMPTY(&vrconnQ)) {
	    if (srb_debug || srbevent_debug || srberr_debug)
		buginf("\nRSRB: Stuck in opening state");
	    rsrb_newstate(peer, RSRB_PS_CLOSED);
	}
    case RSRB_PS_OPENWAIT:
    case RSRB_PS_REMWAIT:
    case RSRB_PS_REMGO:
    case RSRB_PS_REMOPENED:
    case RSRB_PS_DRAINING:
        /*
         * fwd all packets - let them open tcp connection if it's not
         * already open.
         * NOPE. This is not a good idea. If you do this and if there is
         * a dead peer, all the explorers coming in from token ring interface
         * will be put into connection queue without clearing interface queue
         * count, and cause that count to reach max. This is NO NO NO, because
         * nothing will come in once input queue count maxes out. So I am
         * reverting this back to what it was before.
         * CSCdi13611
         */

	keep = (vhdr && ((vhdr_op = GETSHORT(&vhdr->op)) != RSRB_OP_FORWARD) &&
	       (vhdr_op != RSRB_OP_EXPLORER) &&
	       (vhdr_op != RSRB_OP_FORWARDCRC));
	break;
    default:
	errmsg(&msgsym(ILLPEER, RSRB), vrpeer_id(peer, peerid), peer,
	       peer->state);
	keep = FALSE;
	break;
    }
    reset_interrupt_level(status);
    if (!keep) {
        if (srb_debug || srbevent_debug)
            buginf("\nRSRB TCPD: wrong state to keep, %s %s %s",
                   vrpeer_id(inpeer, peerid), vrpeer_state(inpeer->state),
                   vr_op2str(vhdr ? GETSHORT(&vhdr->op) : 0));
	vr_nukepak(pak);
	peer->drops++;
	return(FALSE);
    }
    if (!srb_running) {
        if (srbevent_debug || srberr_debug)
            buginf("\nRSRB TCPD: SRB BACKGROUND not running, aborting");
        vr_nukepak(pak);
        peer->drops++;
        return(FALSE);
    }
    pak->peer_ptr = peer;
    srb_enq(vrconn_pakQ, pak);
    return(TRUE);
}

/*
 * vrtcpd_direct
 * Just send it.  Note that tcpdriver packet input assumes
 * that datagram_size and datagram_len are already set up.
 * len is superflous.
 */

boolean vrtcpd_direct (tr_vpeertype *vrp, paktype *pak)
{
    int result;

    /*
     * What's the point of sending anything to peer if srb is not running?
     * CSCdi12749
     */
    if (!srb_running) {
        vr_nukepak(pak);
        if (srbevent_debug)
            buginf("\nRSRB IF: SRB not running yet, dropping packet");
        return(FALSE);
    }

    if (!vrp->t) {
	vr_nukepak(pak);
	return(FALSE);
    }
    vrp->pkts_tx++;
    vrp->bytes_tx += pak->datagramsize;
    result = reg_invoke_ip_tcpdriver_send(vrp->t, pak, TRUE);
    if (TCPDSEND_ERROR(result)) {
        if (result == TCPDSEND_CLOSED) {
	    rsrb_newstate(vrp, RSRB_PS_DEAD);
	}
        if (srbevent_debug || srberr_debug)
	    buginf("\nRSRB TCPD: direct send failed %d", result);
	vr_nukepak(pak);
	vrp->drops++;
	return(FALSE);
    } else {
	clear_if_input(pak);
    }
    return(TRUE);
}

/*
 * vrtcpd_abort
 * nuke with extreme prejudice a tcpdriver peer.  This aborts the
 * connection and drains any input.
 */

void vrtcpd_abort (tr_vpeertype *vrp)
{
    if (srbevent_debug || srberr_debug) { 
      buginf("\nRSRB: aborting %d/%i (vrtcpd_abort)", 
             vrp->vrg->vrn, vrp->ipaddr); 
    } 
    rsrb_newstate(vrp, RSRB_PS_DEAD);
    if (vrp->t) {
	reg_invoke_ip_tcpdriver_update(vrp->t, NULL, 0, NULL);
	rsrb_tcpdriver_abort(vrp->t);
    }
}

void vrtcpd_close (tr_vpeertype *vrp, ulong param)
{
    if (srbevent_debug || srberr_debug) {
	if (vrp->t) {
	    buginf("\nRSRB: closing %d/%i, %i(%d)", vrp->vrg->vrn, vrp->ipaddr,
		   reg_invoke_ip_tcpdriver_fhost(vrp->t),
		   reg_invoke_ip_tcpdriver_fport(vrp->t));
	} else {
	    buginf("\nRSRB: closing %d/%i, no open tcp connection",
		   vrp->vrg->vrn, vrp->ipaddr);
	}
    }
    if (vrp->t) {
	reg_invoke_ip_tcpdriver_update(vrp->t, vrtcpd_drain, 1,
				       (void *) param);
	rsrb_tcpdriver_close(vrp->t);
    }
}

/*
 * vrtcpd_connect
 * perform an active open to establish a connection to our peer.  It
 * handles basic opening and potential dual opens that might be occuring.
 */

vpeer_connrtn vrtcpd_connect (tr_vpeertype *vrp, ushort vrn)
{
    int openstat;
    ipaddrtype lastip, peername;
    tcp_encaps *t;
    tr_vgrouptype *vrg;
    tcp_encaps tmp;

    vrg = vrp->vrg;
    if (vrp->type != RSRB_PT_TCPD) {
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: CONN: connect pointer invalid (tcpd)");
        return(RSRB_CONN_ABORTED);
    }
    peername = vrtcp_peername(vrg);
    if (!peername) {
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: CONN: no peername set, aborting (tcpd)");
	return(RSRB_CONN_FAILED);
    }
    vrp->retries = RSRB_MAX_OPEN_TRIES;
    while (vrp->retries--) {
	if (srb_debug || srbevent_debug)
	    buginf("\nRSRB: CONN: opening peer %d/%i, %d",
		   vrp->vrg->vrn, vrp->ipaddr, vrp->retries);
	openstat = -1;
	lastip = vrp->ipaddr;
	/*
	 * tcpdriver_active_open will block waiting for the connection
	 * to open.
	 */
	memset(&tmp, 0, sizeof(tcp_encaps));
	tmp.readf = (tcpdriver_readf) vrtcpd_ahdr;
	tmp.closef = (tcpdriver_closef) vrtcpd_fini;
	tmp.flushf = NULL;
	tmp.flow_ctrl = NULL;
	tmp.inthresh = sizeof(tr_vhdrtype);
	tmp.cookie = vrp;
	t = reg_invoke_ip_tcpdriver_active_open(vrp->ipaddr, peername,
						vrp->port,
						vrp->tcp_rcv_window,
						&tmp, &openstat);
	reg_invoke_ip_tcpdriver_set_oqmax(t, vring_tcpqmax);
	reg_invoke_ip_tcpdriver_change_timeout(t,TRUE); /* reenable timeouts */
	/*
	 * While we were asleep it's possible that someone pulled
	 * the rug.  Check out our pointers.
	 */
	if ((vrg != vrfind_ringgroup(vrn)) ||
	      !rsrb_checkqueue(&vrg->peers, vrp) ||
	      vrp->vrg != vrg || vrp->type != RSRB_PT_TCPD ||
	      lastip != vrp->ipaddr || vrp->state < RSRB_PS_OPENWAIT) {
            if (srb_debug || srbevent_debug || srberr_debug)
                buginf("\nRSRB: CONN: consistency abort %i on open",
                       lastip);
	    rsrb_tcpdriver_abort(t);
	    return(RSRB_CONN_ABORTED);
	}
	switch (vrp->state) {
	  case RSRB_PS_OPENWAIT:
	    if (t) {			/* successful open? */
		rsrb_newstate(vrp, RSRB_PS_REMWAIT); /* yep */
		rsrb_tcpdriver_abort(vrp->t);
		vrp->t = t;
                vrsend_version(vrp, RSRB_OP_VERSIONXREQ, ABORT_ON_FAIL);
		TIMER_START(vrconn_wait, vrconn_timeout);
		edisms((blockproc *)vrconnectorBLOCK, (ulong)vrp);
		if ((vrg != vrfind_ringgroup(vrn)) || /* went bye bye? */
		      !rsrb_checkqueue(&vrg->peers, vrp) ||
		      vrp->vrg != vrg || vrp->type != RSRB_PT_TCPD ||
		      lastip != vrp->ipaddr) {
                    if (srb_debug || srbevent_debug || srberr_debug)
                        buginf("\nRSRB: CONN: consistency abort %i on open",
                               lastip);
		    rsrb_tcpdriver_abort(t);
		    return(RSRB_CONN_ABORTED);
		}
		switch (vrp->state) {
		  case RSRB_PS_OPENWAIT: /* Fallout from above... */
		    break;
		  case RSRB_PS_REMWAIT:	/* timeout */
		    if (srb_debug || srbevent_debug || srberr_debug) {
			buginf("\nRSRB: CONN: attempt timed out");
		    }
		    reg_invoke_ip_tcpdriver_update(vrp->t, NULL, 0, NULL);
		    rsrb_tcpdriver_abort(vrp->t);
                    rsrb_newstate(vrp, RSRB_PS_OPENWAIT);
		    break;
		  case RSRB_PS_REMGO:	/* remote says its mine */
		    return(RSRB_CONN_CONNECTED);
		  case RSRB_PS_REMOPENED: /* remote got it, blow this one */
		    reg_invoke_ip_tcpdriver_update(vrp->t, NULL, 0, NULL);
		    if (srbevent_debug) {
			buginf("\nRSRB: Aborting at extra location #2");
		    }
		    rsrb_tcpdriver_abort(vrp->t);
		    return(RSRB_CONN_CONNECTED);
		  case RSRB_PS_CLOSED:
		    if (srb_debug || srbevent_debug || srberr_debug)
			buginf("\nRSRB: CONN: Remote closed %d/%i on open",
			       vrp->vrg->vrn, vrp->ipaddr);
                    if (vrp->retries == 0)
                        return(RSRB_CONN_FAILED);
                    rsrb_newstate(vrp, RSRB_PS_OPENWAIT);
		    break;
		  default:
		    if (srbevent_debug) {
			buginf("\nRSRB: peer state=0x%x, aborting active open",
			       vrp->state);
			if (!vrp->t)
			    buginf("\nRSRB: TCP connection was closed.");
		    }
		    /* use vrp->t instead of t.
		     * vrp->t could have been changed after edisms.
		     */
		    rsrb_tcpdriver_abort(vrp->t);
		    return(RSRB_CONN_FAILED);
		}
	    }
	    break;
	  case RSRB_PS_REMOPENED:	/* remote got it, close this one */
	    if (t) {
		if (srbevent_debug) {
		    buginf("\nAborting at extra location #3");
		}
		reg_invoke_ip_tcpdriver_update(t, NULL, 0, NULL);
		rsrb_tcpdriver_abort(t);
	    }
	    return(RSRB_CONN_CONNECTED);

	  default:
	    errmsg(&msgsym(CONIPST, RSRB), vrp->vrg->vrn, vrp->ipaddr,
		   vrp->state);
	    rsrb_tcpdriver_abort(t);
	    return(RSRB_CONN_FAILED);
	}
	if (openstat == NO_MEMORY || openstat == UNREACHABLE ||
	      openstat == TIMEDOUT || openstat == OPTSTOOBIG) {
	    vrp->retries = 0;
	}
    }
    if (vrp->state == RSRB_PS_OPENWAIT && openstat == -1)
	openstat = TIMEDOUT;
    if ((srb_debug || srbevent_debug || srberr_debug) &&
           vrp->state != RSRB_PS_CLOSED)
	buginf("\nRSRB: CONN: peer %d/%i open failed, %s [%d]",
	       vrp->vrg->vrn, vrp->ipaddr,
	       tcpuser_open_fail_str(openstat), openstat);
    return(RSRB_CONN_FAILED);
}


/********************************************************************
 *
 * TCPDRIVER: Support
 *
 ********************************************************************/

ipaddrtype vrtcp_peername (tr_vgrouptype *vrg)
{
    tr_vpeertype *vrp;
    idbtype *idb;

    if (!vrg)
	return((ipaddrtype) 0);
    for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
	if (vrp->type == RSRB_PT_TCPD) {
	    /*
	     * Always take the first one that matches.
	     */
	    FOR_ALL_SWIDBS(idb)
		if (idb->ip_enabled && ip_ifaddress(idb, vrp->ipaddr))
		    return(vrp->ipaddr);
	}
    }
    return((ipaddrtype) 0);
}

void vrtcpd_send_close (tcp_encaps *t, ushort op, tr_vhdrtype *vrh,
			ulong param, char *label, tr_vpeertype *close_cookie, boolean close)
{
    paktype *pak;

    if (srb_debug || srbevent_debug || srberr_debug){
	buginf("\nRSRB: %s: sent %s to %d/%i %s", label, vr_op2str(op),
	       vrh->vrn, reg_invoke_ip_tcpdriver_fhost(t),
               close ? "closing" : "");
    }
    pak = vrpak(vrh->rhdr_version, op, vrh->trn, vrh->vrn, 0, 0);
    if (!pak) {
	if (srb_debug || srbevent_debug || srberr_debug) {
	    buginf("\nRSRB: %s: send close, no pak", label);
	}
    } else {
	int result;

	((tr_vhdrtype *) (pak->datagramstart))->param = param;
	result = reg_invoke_ip_tcpdriver_send(t, pak, TRUE);
	if (TCPDSEND_ERROR(result)) {
	   if (srb_debug || srbevent_debug || srberr_debug) {
	       buginf("\nRSRB: %s: could not send close to %d/%i", label,
		      vrh->vrn, reg_invoke_ip_tcpdriver_fhost(t));
	   }
	   retbuffer(pak);
       }
    }
    if (close) {
	reg_invoke_ip_tcpdriver_update(t, vrtcpd_drain, 1, close_cookie);
        rsrb_tcpdriver_close(t);
    }
    return;
}

/*
 * vrtcpd_err
 * Simple error handler for dealing with errors coming back from a
 * peer.
 */

void vrtcpd_err (tcp_encaps *t, tr_vhdrtype *vrh, char *label)
{
    if (srb_debug || srbevent_debug || srberr_debug)
	buginf("\nRSRB: TCPD_ERR %s: %s: %d/%i, op %x, len %d, trn %d, param: %08x",
	        label,vr_op2str(vrh->op), vrh->vrn, reg_invoke_ip_tcpdriver_fhost(t),
	       vrh->op, vrh->len, vrh->trn, vrh->param);
    switch (vrh->op) {
      case RSRB_OP_UNKTRN:
      case RSRB_OP_UNKCAP:
	break;
      case RSRB_OP_UNKVRN:
      case RSRB_OP_UNKPEER:
      case RSRB_OP_BADOP:
      case RSRB_OP_UNKVER:
      default:
	rsrb_tcpdriver_abort(t);
	break;
    }
}

/*
 * vrtcpd_gethdr
 * obtain the bytes needed to build a header from a tcpdriver tcp stream.
 *
 * vrh          points to where to actually put the vring header bytes.
 * check_ver    is TRUE if we should verify the version in the header
 *              against the version of this peer.  (Only if vrp is non NULL).
 */

boolean vrtcpd_gethdr (tcp_encaps *t, tcbtype *tcb, int thresh, tr_vpeertype *vrp,
                   tr_vhdrtype *vrh, char *label, boolean check_ver)
{
    int bytes;
    int s;
    uchar *p, *dest;

    bytes = tcp_unread(tcb);
    if (thresh != sizeof(tr_vhdrtype) || bytes < thresh) {
        if (srb_debug || srbevent_debug || srberr_debug)
            buginf("\nRSRB: %s: %d/%i bad state, thresh %d, bytes %d",
                   label, (vrp ? vrp->vrg->vrn : -1),
		   reg_invoke_ip_tcpdriver_fhost(t),thresh, bytes);
        return(FALSE);
    }

    dest = (uchar *)vrh;
    do {
        s = tcp_getstring(tcb, &p, &bytes, thresh);
#ifdef RSRB_VERBOSE
        if (srb_debug)
            buginf("\nRSRB: %s: got %d bytes, thresh %d, ret code %d",
                   label, bytes, thresh, s);
#endif
        if (s != OK) {
            if (srb_debug || srbevent_debug || srberr_debug)
                buginf("\nRSRB: %s: getstring failed %d. %d to read - %i(%d) -> %d",
                       label, s, thresh, tcb->foreignhost.ip_addr,
		       tcb->foreignport, tcb->localport);
            return(FALSE);
        }
        if (bytes > 0) {
            bcopy(p, dest, bytes);
            dest += bytes;
            thresh -= bytes;
        } else {
            thresh = 0;  /* escape the loop */
        }
    } while (thresh > 0);
    if (vrp) {
        GET_TIMESTAMP(vrp->last_keepalive);
        if (check_ver && vrh->rhdr_version != vrp->vrp_version) {
            if (srb_debug || srbevent_debug || srberr_debug) {
                errmsg(&msgsym(BADVERSIONTCP, RSRB),
                       label, (vrp ? vrp->vrg->vrn : -1), reg_invoke_ip_tcpdriver_fhost(t),
                       vrp->vrp_version, vrh->rhdr_version);
                buginf("\nRSRB: hdr %4x %4x %4x %4x %4x %4x %8x",
                       vrh->rhdr_version, vrh->op, vrh->trn, vrh->vrn, vrh->offset,
                       vrh->len, vrh->param);
                dest = (uchar *) vrh + sizeof(tr_vhdrtype);
                buginf("\nRSRB: %08x %08x %08x %08x",
                       ((ulong *)dest)[0], ((ulong *)dest)[1],
                       ((ulong *)dest)[2], ((ulong *)dest)[3]);
            }
            return(FALSE);
        }
    }
    return(TRUE);
}

/*
 * vrtcpd_drain
 * Drain all input.  This makes sure we can't hang on input in a tcpdriver
 * close state while waiting for any output to finish up.
 */

void vrtcpd_drain (tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie)
{
    int bytes;
    uchar *p;
    int s;
    
    bytes = tcp_unread(tcb);
#ifdef RSRB_VERBOSE
    if (srb_debug) {
	buginf("\nRSRB: DRAIN: %i thresh %d unread %d, %d/%i",
	       reg_invoke_ip_tcpdriver_fhost(t), thresh, bytes,
	       (vrp ? vrp->vrg->vrn : 0), (vrp ? vrp->ipaddr : 0));
    }
#endif
    while (bytes) {
	s = tcp_getstring(tcb, &p, &bytes, bytes);
#ifdef RSRB_VERBOSE
	if (srb_debug)
	    buginf("\nRSRB: DRAIN: got %d bytes, thresh %d, ret code %d",
		   bytes, thresh, s);
#endif
	if (s != OK && (srb_debug || srbevent_debug || srberr_debug))
	    buginf("\nRSRB: DRAIN: getstring failed %d", s);
	bytes = tcp_unread(tcb);
    }
}


/*
 * v r t c p d _ o p _ e x p e c t e d
 *
 * Verify that the expected op was recieved.  If the op isn't the expected
 * one we check for err ops and do the correct thing.  Otherwise give an
 * appropriate error message.
 */

boolean vrtcpd_op_expected (tcp_encaps *t, tr_vhdrtype *vrh, tr_vpeertype *vrp,
                            ushort expected_op, char *label)
{
    if (vrh->op == expected_op)
        return(TRUE);
    if (vrh->op & RSRB_OP_ERR) {
        vrtcpd_err(t, vrh, label);
	reg_invoke_ip_tcpdriver_update(t, vrtcpd_drain, 1, (tcp_encaps *) vrp);
	rsrb_tcpdriver_close(t);
        return(FALSE);
    }
    if (srb_debug || srbevent_debug || srberr_debug)
        buginf("\nRSRB: %s: bad op %s (%d), required %s (%d) %d/%i",
               label, vr_op2str(vrh->op), vrh->op,
               vr_op2str(expected_op), expected_op,
               (vrp ? vrp->vrg->vrn : -1),
               reg_invoke_ip_tcpdriver_fhost(t));
    rsrb_tcpdriver_abort(t);
    return(FALSE);
}

/*
 * vrtcpd_phdr
 * Deal with the first packet for a new connection.  Check many things
 * including our local state.  Break any ties and either reject or
 * cause a state change on this side.
 */

void vrtcpd_phdr (tcp_encaps *t, tcbtype *tcb, int thresh, tr_vpeertype *vrp)
{
    tr_vhdrtype lhdr, *vrh;
    tr_vgrouptype *vrg;
    leveltype status;
    char peerid[MAX_PEER_NAME];

#ifdef RSRB_VERBOSE
    if (srb_debug) {
	int s;
	s = tcp_unread(tcb);
	buginf("\nRSRB: PHDR: %i thresh %d unread %d, %d/%i",
	       reg_invoke_ip_tcpdriver_fhost(t), thresh, s,
	       (vrp ? vrp->vrg->vrn : 0), (vrp ? vrp->ipaddr : 0));
    }
#endif
    if (vrp) {
	if (srb_debug || srbevent_debug || srberr_debug) {
	    buginf("\nRSRB: PHDR: vrp set, t %i, %d/%i",
		   reg_invoke_ip_tcpdriver_fhost(t), vrp->vrg->vrn,
		   vrp->ipaddr);
	}
	rsrb_tcpdriver_abort(t);
	return;
    }
    vrh = &lhdr;
    if (!vrtcpd_gethdr(t, tcb, thresh, vrp, vrh, "PHDR", RSRB_DONT_CHECK_VER)) {
	rsrb_tcpdriver_abort(t);
	return;
    }
    /*
     * verify startup sequence and process any Errors
     */
    if (!vrtcpd_op_expected(t, vrh, vrp, RSRB_OP_VERSIONXREQ, "PHDR"))
        return;

    vrg = vrfind_ringgroup(vrh->vrn);
    vrp = vrfind_peer(vrg, RSRB_PT_TCPD,
		      (rsrb_cookie) reg_invoke_ip_tcpdriver_fhost(t),
		      reg_invoke_ip_tcpdriver_lport(t),
		      reg_invoke_ip_tcpdriver_fport(t), NULL);
    if (!vrg || !vrp) {
	vrtcpd_send_close(t, (!vrg ? RSRB_OP_UNKVRN : RSRB_OP_UNKPEER), vrh,
			  vrh->vrn, "PHDR", NULL, TRUE);
	return;
    }
    if (srb_debug || srbevent_debug)
        buginf("\nRSRB: PHDR: connecting, %s from %s (ver %d)", vr_op2str(vrh->op),
               vrpeer_id(vrp, peerid), vrh->rhdr_version);

    if (!vr_version_reasonable(vrp, vrh, "PHDR")) {
        vrtcpd_send_close(t, RSRB_OP_UNKVER, vrh, vrh->rhdr_version, "PHDR", vrp, TRUE);
        return;
    }

    if (t->tcb)
	tcp_set_rcvwnd(t->tcb, vrp->tcp_rcv_window);

    status = raise_interrupt_level(NETS_DISABLE);
    switch (vrp->state) {
      case RSRB_PS_REMWAIT:
	if ((ulong) reg_invoke_ip_tcpdriver_lhost(t) >
	    (ulong) reg_invoke_ip_tcpdriver_fhost(vrp->t)) {
	    /*
	     * active wins.  passive abort.  vrp->t already set.  Don't
	     * answer.  Other side's passive will win and kill it.
	     */
	    rsrb_newstate(vrp, RSRB_PS_REMGO);
	    reset_interrupt_level(status);
	    /* other side closes */
	    reg_invoke_ip_tcpdriver_update(t, vrtcpd_drain, 1, NULL);
	    return;
	}
	/*
	 * passive wins.  fall through.  Will go to remopened which causes
	 * the active side to abort when it wakes up
	 */

      case RSRB_PS_DEAD:
      case RSRB_PS_CLOSED:
      case RSRB_PS_OPENING:
      case RSRB_PS_OPENWAIT:
	if (vrp->state == RSRB_PS_CLOSED || 
	    vrp->state == RSRB_PS_DEAD ||
	    vrp->state == RSRB_PS_OPENING) {
	    rsrb_newstate(vrp, RSRB_PS_CONNECTED);
	    TIMER_START(vrp->connect_timer, RSRB_CONNECT_WINDOW);
	} else
	    rsrb_newstate(vrp, RSRB_PS_REMOPENED);
reconnect:
	rsrb_tcpdriver_abort(vrp->t);
	vrp->t = t;
	if (vrh->len) {
	    bcopy((uchar *) vrh, (uchar *) &vrp->inhdr, sizeof(tr_vhdrtype));
	    reg_invoke_ip_tcpdriver_update(t, vrtcpd_rddata, vrh->len, vrp);
	} else {
	    vrqueue_null_xchg(vrp);
	    vrp->inhdr.op = RSRB_OP_EMPTY;
	    reg_invoke_ip_tcpdriver_update(t, vrtcpd_rdhdr,
					   sizeof(tr_vhdrtype), vrp);
	}
        vrsend_version(vrp, RSRB_OP_VERSIONREPLY, ABORT_ON_FAIL);
	if (!vrsend_xchg(vrp, RSRB_OP_RINGXREPLY, "PHDR", TRUE)) {
            (*vrp->abort)(vrp);
	}
	reset_interrupt_level(status);
        vrp->vrp_starting_up = FALSE;
	return;
		
      case RSRB_PS_CONNECTED:
	if (!SLEEPING(vrp->connect_timer)) {
	    /*
	     * Outside the connect window.  Must be other side crashed
	     * and is trying to reestablish communications.  Nuke the
	     * connection this peer is currently using.  Its pointing
	     * off in the tulies.  Then replace this peer's connection
	     * with the new one coming in.  Respond normally to the
	     * ring exchange packet which completes the far sides
	     * connection.
	     */
	    if (srb_debug || srbevent_debug)
		buginf("\nRSRB: peer %d/%i reestablishing connection",
		       vrp->vrg->vrn, vrp->ipaddr);
            if (vrp->local_ack)
               lack_toss_connections(vrp);
	    reg_invoke_ip_tcpdriver_update(vrp->t, NULL, 0, NULL);
	    rsrb_tcpdriver_abort(vrp->t);
	    goto reconnect;		/* and point at new connection */
	}
	/*
	 * if within the connection window, then active won
	 */
      case RSRB_PS_REMGO:		/* our active was told it won */
      case RSRB_PS_REMOPENED:
      case RSRB_PS_DRAINING:
	reset_interrupt_level(status);
	reg_invoke_ip_tcpdriver_update(t, vrtcpd_drain, 1, NULL);
	return;

      default:
	reset_interrupt_level(status);
	if (srb_debug || srbevent_debug || srberr_debug) {
	    buginf("\nRSRB: PHDR: bad connection state %d, %d/%i",
		   vrp->state, vrp->vrg->vrn, vrp->ipaddr);
	}
	(*vrp->abort)(vrp);
	return;
    }
}

/*
 * vrtcpd_ahdr
 * handle the first packet incoming for the active side.
 */

void vrtcpd_ahdr (tcp_encaps *t, tcbtype *tcb, int thresh, tr_vpeertype *vrp)
{
    tr_vhdrtype lhdr, *vrh;
    tr_vgrouptype *vrg;
    leveltype status;
    char peerid[MAX_PEER_NAME];

#ifdef RSRB_VERBOSE
    if (srb_debug) {
	int s;
	s = tcp_unread(tcb);
	buginf("\nRSRB: AHDR: %i thresh %d unread %d, %d/%i",
	       reg_invoke_ip_tcpdriver_fhost(t), thresh, s,
	       (vrp ? vrp->vrg->vrn : 0), (vrp ? vrp->ipaddr : 0));
    }
#endif
    if (!vrp) {
	if (srb_debug || srbevent_debug || srberr_debug) {
	    buginf("\nRSRB: AHDR: vrp not set, t %i",
		   reg_invoke_ip_tcpdriver_fhost(t));
	}
	rsrb_tcpdriver_abort(t);
	return;
    }
    vrh = &lhdr;
    if (!vrtcpd_gethdr(t, tcb, thresh, vrp, vrh, "AHDR", RSRB_DONT_CHECK_VER)) {
	rsrb_tcpdriver_abort(t);
	return;
    }

    /*
     * verify startup sequence and process any Errors
     */
    if (!vrtcpd_op_expected(t, vrh, vrp, RSRB_OP_VERSIONREPLY, "AHDR"))
        return;

    vrg = vrfind_ringgroup(vrh->vrn);	/* verify consistency on open */
    if (!vrg || vrg != vrp->vrg) {
	vrtcpd_send_close(t, RSRB_OP_UNKVRN, vrh, vrh->vrn, "AHDR", vrp, TRUE);
	return;
    }
    if (vrp != vrfind_peer(vrg, RSRB_PT_TCPD,
			   (rsrb_cookie) reg_invoke_ip_tcpdriver_fhost(t),
			   reg_invoke_ip_tcpdriver_lport(t),
			   reg_invoke_ip_tcpdriver_fport(t), NULL)) {
	vrtcpd_send_close(t, RSRB_OP_UNKPEER, vrh,
                          reg_invoke_ip_tcpdriver_fhost(t),
                          "AHDR", vrp, TRUE);
	return;
    }

    if (srb_debug || srbevent_debug)
        buginf("\nRSRB: AHDR: connecting, %s from %s (ver %d)", vr_op2str(vrh->op),
               vrpeer_id(vrp, peerid), vrh->rhdr_version);

    if (!vr_version_reasonable(vrp, vrh, "AHDR")) {
        vrtcpd_send_close(t, RSRB_OP_UNKVER, vrh, vrh->rhdr_version, "AHDR", vrp, TRUE);
        return;
    }

    if (vrp->vrp_version >= RSRB_CAP_VERSION)
        if (!vrsend_caps(vrp, RSRB_OP_CAPS_REQ, RSRB_OUR_CAPS, "AHDR")) {
            (*vrp->abort)(vrp);
            return;
        }

    status = raise_interrupt_level(NETS_DISABLE);
    switch (vrp->state) {
      case RSRB_PS_REMWAIT:
	rsrb_newstate(vrp, RSRB_PS_REMGO);
	/*
	 * fall tHrouGh.
	 */
      case RSRB_PS_REMGO:
	reset_interrupt_level(status);
	if (vrh->len) {
	    bcopy((uchar *) vrh, (uchar *) &vrp->inhdr, sizeof(tr_vhdrtype));
	    reg_invoke_ip_tcpdriver_update(t, vrtcpd_rddata, vrh->len, vrp);
	} else {
	    vrqueue_null_xchg(vrp);
	    vrp->inhdr.op = RSRB_OP_EMPTY;
	    reg_invoke_ip_tcpdriver_update(t, vrtcpd_rdhdr,
					   sizeof(tr_vhdrtype), vrp);
	}
        vrp->vrp_starting_up = FALSE;
	return;

      default:
	reset_interrupt_level(status);
	if (srb_debug || srbevent_debug || srberr_debug) {
	    buginf("\nRSRB: AHDR: bad connection state %d, %d/%i",
		   vrp->state, vrp->vrg->vrn, vrp->ipaddr);
	}
	rsrb_tcpdriver_abort(t);
	return;
    }
}


/*
 * vrtcpd_rdhdr
 * tcpdriver implementation of vring peer handler.  This
 * routine accepts the first part of an incoming packet from
 * a remote peer.  If there is data it resets the reader
 * function of the driver struct.
 */

void vrtcpd_rdhdr (tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie)
{
    tr_vhdrtype *vrh;
    tr_vgrouptype *vrg;
    char peerid[MAX_PEER_NAME];
    tr_vpeertype *vrp;

    vrp = (tr_vpeertype *)cookie;


#ifdef RSRB_VERBOSE
    if (srb_debug) {
	int bytes;
	bytes = tcp_unread(tcb);
	buginf("\nRSRB: HDR: %i thresh %d unread %d, %d/%i",
	       reg_invoke_ip_tcpdriver_fhost(t), thresh, bytes,
	       (vrp ? vrp->vrg->vrn : 0), (vrp ? vrp->ipaddr : 0));
    }
#endif
    if (!vrp) {
	errmsg(&msgsym(HDRNOVRP, RSRB), reg_invoke_ip_tcpdriver_fhost(t));
	rsrb_tcpdriver_abort(t);
	return;
    }
    if (vrp->state < RSRB_PS_REMWAIT) {
	errmsg(&msgsym(HDRVRP, RSRB), vrp->vrg->vrn, vrp->ipaddr, vrp->state);
	rsrb_tcpdriver_abort(t);
	return;
    }
    vrh = &vrp->inhdr;
    vrg = vrp->vrg;
    if (vrh->op != RSRB_OP_EMPTY) {
	errmsg(&msgsym(HDRRECV, RSRB), vrg->vrn, vrp->ipaddr, vrh->op);
    }
    if (!vrtcpd_gethdr(t, tcb, thresh, vrp, vrh, "HDR", RSRB_CHECK_VER)) {
	if (srbevent_debug || srberr_debug) {
            buginf("\nRSRB: HDR: header read for peer %d/%i failed.  Aborting.",
                   vrg->vrn, vrp->ipaddr);
	}
	rsrb_tcpdriver_abort(t);
	return;
    }
    if (vrh->op & RSRB_OP_ERR) {
	vrtcpd_err(t, vrh, "HDR");
	if (vrh->len) {
	    vrh->op = RSRB_OP_IGNORE;
	    reg_invoke_ip_tcpdriver_update(t, vrtcpd_rddata, vrh->len, vrp);
	} else {
	    vrh->op = RSRB_OP_EMPTY;
	}
	return;
    }
    if (vrh->vrn != vrg->vrn) {
	vrtcpd_send_close(t, RSRB_OP_UNKVRN, vrh, vrh->vrn, "HDR", vrp, TRUE);
	return;
    }

    switch (vrh->op) {
      case RSRB_OP_FORWARD:		/* trn checking delayed to data */
      case RSRB_OP_FORWARDCRC:
      case RSRB_OP_EXPLORER:

      case RSRB_OP_LINKUP_REQ:          /* lack ops */
      case RSRB_OP_LINKDOWN_REQ:
      case RSRB_OP_LINKUP_PRSP:
      case RSRB_OP_LINKDOWN_PRSP:

      case RSRB_OP_SDLLC:
      case RSRB_OP_QLLC:
      case RSRB_OP_TR_LANE:
      case RSRB_OP_LLC2DATA:
      case RSRB_OP_QUENCH_ON:
      case RSRB_OP_QUENCH_OFF:

      case RSRB_OP_ConnectRequest:      /* lak2 ops */
      case RSRB_OP_ConnectPollReq:
      case RSRB_OP_ConnectPollAck:
      case RSRB_OP_ConnectPollAckAndReq:
      case RSRB_OP_ConnectWaitAck:
      case RSRB_OP_DisconnectRequest:
      case RSRB_OP_DisconnectAck:
      case RSRB_OP_LA2_QON:
      case RSRB_OP_LA2_QON_ACK:
      case RSRB_OP_LA2_QOFF:
      case RSRB_OP_LA2_QOFF_ACK:
      case RSRB_OP_SEND_XID:
      case RSRB_OP_BPDU:		/* SRB AST support */
      case RSRB_OP_SPAN:		/* SRB AST support */
        if (vrh->len == 0) {                    /* weird case, len 0? */
            errmsg(&msgsym(BADLENIP, RSRB), vrg->vrn, vrp->ipaddr,
                   "HDR", vrh->len, vrh->trn);
            buginf("\nLOCACK: HDR: Bad length from: %s %d %d %03x %03x %d %d %08x",
                   vrpeer_id(vrp, peerid), vrh->rhdr_version, vrh->op, vrh->trn,
                   vrh->vrn, vrh->offset, vrh->len, vrh->param);
        }
        break;

      case RSRB_OP_VERSIONREQ:
      case RSRB_OP_VERSIONXREQ:
        if (srb_debug || srbevent_debug)
            buginf("\nRSRB: HDR: %s, sending %s -> %s",
                   vr_op2str(vrh->op), vr_op2str(RSRB_OP_VERSIONREPLY),
                   vrpeer_id(vrp, peerid));
        vrsend_version(vrp, RSRB_OP_VERSIONREPLY, DONT_ABORT_ON_FAIL);
        if (vrh->op == RSRB_OP_VERSIONREQ)
            break;

        /* fall through */
      case RSRB_OP_RINGXREQ:
	if (!vrsend_xchg(vrp, RSRB_OP_RINGXREPLY, "HDR", TRUE)) {
	    if (srbevent_debug || srberr_debug)
		buginf("\nRSRB: HDR: RingXRep send failed for %d/%i, aborting",
		       vrg->vrn, vrp->ipaddr);
            (*vrp->abort)(vrp);
	    return;
	}

	/*
	 * fall through
	 */
      case RSRB_OP_RINGXREPLY:
	if (vrh->len == 0)		/* no remote rings */
	    vrqueue_null_xchg(vrp);
	break;

      case RSRB_OP_NOP:
	break;

      case RSRB_OP_VERSIONREPLY:
        if (srb_debug || srbevent_debug)
            buginf("\nRSRB: HDR: Received %s from %s (version %d)",
                   vr_op2str(vrh->op), vrpeer_id(vrp, peerid), vrh->rhdr_version);
        break;

      case RSRB_OP_CAPS_REQ:
        vrsend_caps(vrp, RSRB_OP_CAPS_REPLY, RSRB_OUR_CAPS, "HDR");

        /* fall through */

      case RSRB_OP_CAPS_REPLY:
        vrp->vrp_capabilities = vrh->param;
        break;

      default:
	vrtcpd_send_close(t, RSRB_OP_BADOP, vrh, vrh->rhdr_version, "HDR", vrp, TRUE);
	return;
    }
    if (vrh->len) {
	reg_invoke_ip_tcpdriver_update(t, vrtcpd_rddata, vrh->len, vrp);
     } else {
	vrh->op = RSRB_OP_EMPTY;
    }
}

void vrtcpd_rddata (tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie)
{
    trrif_t *trh;
    tr_vhdrtype *vrh;
    tr_vgrouptype *vrg;
    paktype *pak;
    int s;
    int bytes, riflen;
    uchar *p, *dest;
    tr_ventrytype *vre;
    hwidbtype *outidb, *hwidb;
    boolean gen_crc = FALSE;
    srbrd_t *rd;
    sap_hdr *llc1;
    tr_vpeertype *vrp;

    vrp = (tr_vpeertype *)cookie;

#ifdef RSRB_VERBOSE
    if (srb_debug) {
	bytes = tcp_unread(tcb);
	buginf("\nRSRB: DATA: %i thresh %d unread %d, %d/%i",
	       reg_invoke_ip_tcpdriver_fhost(t), thresh, bytes,
	       (vrp ? vrp->vrg->vrn : 0), (vrp ? vrp->ipaddr : 0));
    }
#endif
    if (!vrp || vrp->inhdr.op == RSRB_OP_EMPTY || !vrp->inhdr.len) {
	if (srb_debug || srbevent_debug || srberr_debug)
	  buginf("\nRSRB: DATA: peer %d/%i is unusual, op %d len %d",
		 (vrp ? vrp->vrg->vrn : -1), reg_invoke_ip_tcpdriver_fhost(t),
		 (vrp ? vrp->inhdr.op : 0), (vrp ? vrp->inhdr.len : 0));
	if (vrp) {
	    rsrb_tcpdriver_abort(vrp->t);
        }
	return;
    }
    vrh = &vrp->inhdr;
    vrg = vrp->vrg;
    pak = getbuffer(vrh->len);
    if (!pak) {
	do {
	    s = tcp_getstring(tcb, &p, &bytes, thresh);
#ifdef RSRB_VERBOSE
	    if (srb_debug)
	      buginf("\nRSRB: DATA drain: got %d bytes, thresh %d, ret %d",
		     bytes, thresh, s);
#endif
	    if (s != OK) {
		if (srb_debug || srbevent_debug || srberr_debug)
		  buginf("\nRSRB: DATA: getstring failed %d during drain. %d yet to read", s, thresh);
		rsrb_tcpdriver_abort(t);
		return;
	    }
            if (bytes > 0) {
                thresh -= bytes;
            } else {
                thresh = 0;  /* escape the loop */
            }
	} while (thresh > 0);
	goto jail;
    }
    dest = pak->datagramstart = pak->network_start;
    pak->datagramsize = vrh->len;
    pak->acct_proto = ACCT_PROTO_SRB;
    do {
	s = tcp_getstring(tcb, &p, &bytes, thresh);
#ifdef RSRB_VERBOSE
	if (srb_debug) {
	    buginf("\nRSRB: DATA: got %d bytes, thresh %d, ret code %d",
		   bytes, thresh, s);
	}
#endif
	if (s != OK) {
	    if (srb_debug || srbevent_debug || srberr_debug) {
		buginf("\nRSRB: DATA: getstring failed %d. %d yet to read", s,
		       thresh);
	    }
	    rsrb_tcpdriver_abort(t);        /* assume out of sync */
            vr_nukepak(pak);
	    return;
	}
        if (bytes > 0) {
            bcopy(p, dest, bytes);
            dest += bytes;
            thresh -= bytes;
        } else {
            thresh = 0;  /* escape the loop */
        }
    } while (thresh > 0);
    if (srb_debug) {
	buginf("\nRSRB: DATA: %d/%i/%d %s, trn %d, vrn %d, off %d, len %d",
	       vrg->vrn, vrp->ipaddr, vrp->port, vr_op2str(vrh->op),
	       vrh->trn, vrh->vrn, vrh->offset, vrh->len);
    }

    pak->peer_ptr = vrp;
    trh = (trrif_t *) pak->datagramstart;
    switch (vrh->op) {
      case RSRB_OP_SEND_XID:            /* send_xids don't have crc's */
      case RSRB_OP_FORWARDCRC:
	gen_crc = TRUE;
	pak->flags |= PAK_SRB_EXPLORER;
	/* Note fall through... */

      case RSRB_OP_FORWARD:
      case RSRB_OP_LLC2DATA:

      case RSRB_OP_LINKUP_REQ:
      case RSRB_OP_LINKDOWN_REQ:
      case RSRB_OP_LINKUP_PRSP:
      case RSRB_OP_LINKDOWN_PRSP:
      case RSRB_OP_QUENCH_ON:
      case RSRB_OP_QUENCH_OFF:

      case RSRB_OP_ConnectRequest:      /* lak2 ops */
      case RSRB_OP_ConnectPollReq:
      case RSRB_OP_ConnectPollAck:
      case RSRB_OP_ConnectPollAckAndReq:
      case RSRB_OP_ConnectWaitAck:
      case RSRB_OP_DisconnectRequest:
      case RSRB_OP_DisconnectAck:
      case RSRB_OP_LA2_QON:
      case RSRB_OP_LA2_QON_ACK:
      case RSRB_OP_LA2_QOFF:
      case RSRB_OP_LA2_QOFF_ACK:

        /*************************************************************************
         *
         * This section of code handles, FORWARD, FORWARDCRC, and the Lack ops only
         *
         *************************************************************************/

        /*
         * Check for no rif.  If so must be going up the virtual interface.
         * Lan Manager?  Any packets that don't have a RIF or terminate on the vring
         * get eaten here.  Yum.  But I sure do hate getting packets stuck between
         * my teeth.
         */
	if ((trh->saddr[0] & TR_RII) == 0) {
 	    vre = vr_virtualp(vrg, 0, trh->daddr);
 	    if (vre || (GETSHORT(trh->daddr) == TRFUNC_UPPER)) {
                srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, 
                                        pak->datagramsize);
                vr_vre_input(vre, pak, FALSE, vrh->op);
		vrh->op = RSRB_OP_EMPTY;
		reg_invoke_ip_tcpdriver_update(t, vrtcpd_rdhdr,
					       sizeof(tr_vhdrtype), vrp);
		return;
	    } else {
		vrh->op = RSRB_OP_EMPTY;
		reg_invoke_ip_tcpdriver_update(t, vrtcpd_rdhdr,
					       sizeof(tr_vhdrtype), vrp);
                vr_nukepak(pak);
		return;
	    }
	}
        /*
         * Check for terminating on the virtual ring.  If so hand it
         * up to the virtual interface.  Lan Manager?
         */
	if (RIF_DEST_RING(trh) == vrg->vrn) {
 	    vre = vr_virtualp(vrg, 0, trh->daddr);
 	    if (vre) {
                srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, 
                                        pak->datagramsize);
                vr_vre_input(vre, pak, TRUE, vrh->op);
		vrh->op = RSRB_OP_EMPTY;
		reg_invoke_ip_tcpdriver_update(t, vrtcpd_rdhdr,
					       sizeof(tr_vhdrtype), vrp);
                return;
	    } else {
		vrh->op = RSRB_OP_EMPTY;
		reg_invoke_ip_tcpdriver_update(t, vrtcpd_rdhdr,
					       sizeof(tr_vhdrtype), vrp);
                vr_nukepak(pak);
		return;
	    }
	}
	vre = vrfind_rn2entry(vrh->trn, RSRB_BN_DONTCARE,
			      (tr_ventrytype *)&vrg->rings.qhead, RSRB_ET_LOCAL);
	if (!vre) {
	    vrtcpd_send_close(t, RSRB_OP_UNKTRN, vrh,
			      vrh->rhdr_version, "DATA", vrp, FALSE);
	    vrp->drops++;
            vr_nukepak(pak);
	    goto jail;
	}
        srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, pak->datagramsize);
	vre->forwards++;
	vrp->pkts_rx++;
        vrp->bytes_rx += pak->datagramsize;
 	/*
 	 * Is this a frame for an SDLLC OR QLLC ring?
 	 */
	if (vre->type & ( RSRB_ET_SDLLC | RSRB_ET_QLLC | RSRB_ET_TR_LANE) )  {
            vr_vre_input(vre, pak, (gen_crc ? FALSE : TRUE), vrh->op);
	    vrh->op = RSRB_OP_EMPTY;
	    reg_invoke_ip_tcpdriver_update(t, vrtcpd_rdhdr,
					   sizeof(tr_vhdrtype), vrp);
	    return;
 	}
	outidb = vre->outhole.idb;
	riflen = trh->rc_blth & RC_LTHMASK;
	if ((TRING_ENCAPBYTES + riflen + SAP_HDRBYTES) >
	    (pak->datagramsize - ((gen_crc) ? 0 : TR_FCS_SIZE))) {
	    if (srbevent_debug || srberr_debug) {
		buginf("\nTR%d: BADLEN: Illegal vring input packet length %d, min legal %d",
		       99, pak->datagramsize,
		       TRING_ENCAPBYTES + riflen + SAP_HDRBYTES);
		tr_print_packet(NULL, "BADLEN", (tring_hdr *) trh,
				(ulong *)((uchar *) (&trh->rc_blth) + riflen),
				0, pak->datagramsize - TR_FCS_SIZE);
	    }
	    vrp->drops++;
	    retbuffer(pak);
	    goto jail;
	}

	if (!srb_out_check((uchar *) pak->datagramstart, pak->datagramsize,
			  outidb, ((ushort) trh->rc_blth & RC_LTHMASK))) {
            vr_nukepak(pak);
	    goto jail;
	}
	pak->if_output = outidb->firstsw;
	if (!gen_crc) {
	    pak->datagramsize -= TR_FCS_SIZE;
	    pak->flags |= PAK_SRB_OUT;
	}
	pak->enctype = ET_NULL;
	pak->linktype = LINK_RSRB;
	if (trh->rc_blth & RC_TRACE_FRAME)
	  reg_invoke_send_trace_report(pak, NULL, outidb);
	pak->if_output = outidb->firstsw;

        llc1 = (sap_hdr *)((uchar *)&trh->rc_blth + riflen);
        if ((llc1->control & ~LLC1_P) == LLC1_XID) {
            if ( (vrh->op == RSRB_OP_FORWARD || vrh->op == RSRB_OP_FORWARDCRC ||
                vrh->op == RSRB_OP_SEND_XID) && !lak2_xid_ok(pak)) {
                if (lack_debug_state)
                    buginf("\n\t losing remote XID after connection.");
                vr_nukepak(pak);
                break;
            }
        }

        if (vrh->op == RSRB_OP_FORWARD || vrh->op == RSRB_OP_FORWARDCRC ||
              vrh->op == RSRB_OP_SEND_XID) {
            srb_sb_incr_tx_nb_counters(outidb->firstsw, pak->datagramsize);
	    llc1 = (sap_hdr *)((uchar *)&trh->rc_blth + riflen);
	    if (((llc1->control & ~LLC1_P) == LLC1_XID) ||
		((llc1->control & ~LLC1_P) == LLC1_TEST)) {
		rif_update(NULL, vrg->vrn, trh->saddr, NULL,
			   (srbroute_t *) &trh->rc_blth, RIF_AGED_REM);
	    }
	    if (global_netbios_name_cache_enable) {
		netbios_name_cache_update_pak((uchar *) trh,
					      NETBIOS_NAME_AGED_REM, NULL,
					      vrg->vrn);
	    }
	    tr_to_media_out(pak);
            break;
        } 
        /*
         * fall through
         */

        /*
         * L a c k / L a k 2     O p s    H a n d l i n g
         */

        /*
         * The assumption is that any of the LACK routines will dispose
         * of the packets passed into them.  We are also guaranteed to
         * have a RIF.  (Code earlier would have eaten the packet if no
         * RIF or if it terminates on the virtual ring.
         */

	 GET_TIMESTAMP(pak->inputtime); 
	 pak->flags |= PAK_LACK_OUT;
	 pak->flags &= ~PAK_LACK_IN;
	 pak->lack_opcode = vrh->op;
	 pak->if_input = outidb->firstsw;

	 switch (vrh->op) {
	  case RSRB_OP_LLC2DATA:
	    lack_data(pak, pak->datagramsize, vre);
	    break;
	    
	  case RSRB_OP_LINKUP_REQ:
	  case RSRB_OP_LINKDOWN_REQ:
	  case RSRB_OP_LINKUP_PRSP:
	  case RSRB_OP_LINKDOWN_PRSP:
	  case RSRB_OP_QUENCH_ON:
	  case RSRB_OP_QUENCH_OFF:
          case RSRB_OP_ConnectRequest:  /* lak2 ops */
          case RSRB_OP_ConnectPollReq:
          case RSRB_OP_ConnectPollAck:
          case RSRB_OP_ConnectPollAckAndReq:
          case RSRB_OP_ConnectWaitAck:
          case RSRB_OP_DisconnectRequest:
          case RSRB_OP_DisconnectAck:
          case RSRB_OP_LA2_QON:
          case RSRB_OP_LA2_QON_ACK:
          case RSRB_OP_LA2_QOFF:
          case RSRB_OP_LA2_QOFF_ACK:
 	    lak2_process_netmsg(pak, vrh->op, vre, DLC_LLC2);
	    break;

          default:
            if (lack_debug_err)
                buginf("\nLOCACK: bad LACK op code, %d", vrh->op);
            vr_nukepak(pak);
            break;
	}
	break;

      case RSRB_OP_EXPLORER:
	/* Is it one of our "virtual" addresses? */
	trh->rc_blth -= 2;
	if (RIF_DEST_RING(trh) == vrg->vrn) {
 	    vre = vr_virtualp(vrg, 0, trh->daddr);
 	    if (vre) {
                if ( (vre->type & ( RSRB_ET_SDLLC | RSRB_ET_QLLC
                                   | RSRB_ET_TR_LANE
                                   | RSRB_ET_LOCAL_VIRTUAL) )) {
		    trh->rc_blth += 2;
		    rd = (srbrd_t *) ((uchar *) trh + vrh->offset);
		    PUTSHORT(rd, ((GETSHORT(rd) & RD_RNMASK) | (vre->bn & RD_BNMASK)));
		    PUTSHORT(++rd, ((vre->rn << RD_RNSHIFT) & RD_RNMASK));
		    /* CSCdi29807 - clear the bridge field */ 
		} else {
		    pak->datagramsize -= 2;
		    bcopy(((uchar *) &(trh->rd[((trh->rc_blth &
						RC_LTHMASK) - 2) / 2 + 1])),
			  ((uchar *) &(trh->rd[((trh->rc_blth &
						RC_LTHMASK) - 2) / 2])),
			  pak->datagramsize);
		}
                vr_vre_input(vre, pak, FALSE, vrh->op);
		goto jail;
	    }
	}
	if (global_proxy_explorer_enable)
	  rif_update(NULL, vrg->vrn, trh->saddr, NULL,
		     (srbroute_t *) &trh->rc_blth, RIF_AGED_REM);
	trh->rc_blth += 2;
	if (global_netbios_name_cache_enable) {
	    netbios_name_cache_update_pak((uchar *) trh,
					  NETBIOS_NAME_AGED_REM, NULL,
					  vrg->vrn);
	}

	/* SRB AST support */
	/* 
	 * If this is a STE and this vidb is not in spanning tree forwarding 
	 * state, then drop it.
	 */
	if ((trh->rc_blth & RC_RTMASK) == RC_B_SPAN) {
	    if (!rsrb_vidb_span_forward(vrg, vrp)) {
		vr_nukepak(pak);
		break;
	    }
	}
	
	vrforward_explorer(pak, vrh->offset, vrp, 99);
	break;

      case RSRB_OP_RINGXREQ:
      case RSRB_OP_RINGXREPLY:
      case RSRB_OP_VERSIONXREQ:
	pak->peer_ptr = vrp;
	srb_enq(vrxchgQ, pak);
	break;

      case RSRB_OP_BPDU:
      case RSRB_OP_SPAN:
	if (!vrg->span_idb) 
	    /* This end is not running spanning tree */
	    goto jail;

        if (vrh->op == RSRB_OP_SPAN) {
	    vrg->span_idb->disjoint_peers = TRUE;
	}

	pak->if_input = vrg->span_idb;
	pak->peer_ptr = vrp;
	hwidb = hwidb_or_null(vrg->span_idb);
	if (!hwidb) {
	    vr_nukepak(pak);
	    return;
	}
        (*hwidb->iqueue)(hwidb, pak);
	break;
	
      default:
	if (srb_debug || srbevent_debug || srberr_debug)
	  buginf("\nRSRB: DATA: bad op %d (%s) from %d/%i",
		 vrh->op, vr_op2str(vrh->op), vrg->vrn,
		 reg_invoke_ip_tcpdriver_fhost(t));
        vr_nukepak(pak);
	break;
    }
  jail:
    vrh->op = RSRB_OP_EMPTY;
    reg_invoke_ip_tcpdriver_update(t, vrtcpd_rdhdr, sizeof(tr_vhdrtype), vrp);
}

/*
 * vrtcpd_fini
 * routine called when the tcpdriver detects a close on
 * the tcpdriver stream.
 */

void vrtcpd_fini (tcp_encaps *t, tr_vpeertype *vrp)
{
    leveltype status;
    tr_starttype *start;

    if (!(vrp && vrvalid_peer(vrp))) {	/* unknown peer abort */
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB: unknown or corrupt peer closed vrp = %x", vrp);
	return;
    }
    status = raise_interrupt_level(NETS_DISABLE);
    vrp->inhdr.op = RSRB_OP_EMPTY;
    while ((start = dequeue(&vrp->startq))) {
        vr_nukepak(start->pak);
	free(start);
    }
    vrp->t = NULL;
    if (srb_debug || srbevent_debug || srberr_debug)
        buginf("\nRSRB: peer %d/%i closed [last state %s (%d)]",
               vrp->vrg->vrn, vrp->ipaddr, vrpeer_state(vrp->state),
               vrp->state);
    rsrb_newstate(vrp, RSRB_PS_CLOSED);
    reset_interrupt_level(status);
}

/*
 * vrtcpd_listen
 * Given a tcb opened by tcp_listen upon an incoming connection, insert
 * it into the tcpdriver structures.  We don't have enough information
 * here to easily validate so we wait for an incoming packet.  If prior
 * to receiving that first packet this side tries to open it's open
 */

void vrtcpd_listen (tcbtype *tcb)
{
    tcp_encaps *t;
    tcp_encaps tmp;

    memset(&tmp, 0, sizeof(tcp_encaps));
    tmp.readf = (tcpdriver_readf) vrtcpd_phdr;
    tmp.closef = (tcpdriver_closef) vrtcpd_fini;
    tmp.flushf = NULL;
    tmp.flow_ctrl = NULL;
    tmp.inthresh = sizeof(tr_vhdrtype);
    tmp.cookie = NULL;
    t = reg_invoke_ip_tcpdriver_passive_open(tcb, RSRB_TCP_WINDOW_SIZE, &tmp);
    if (!t) {
	if (srb_debug || srbevent_debug) {
	    buginf("\nRSRB: passive open from %i(%d) -> %d failed",
		   tcb->foreignhost.ip_addr, tcb->foreignport, tcb->localport);
	}
	tcp_close(tcb);
    } else {
	reg_invoke_ip_tcpdriver_set_oqmax(t, vring_tcpqmax);
	reg_invoke_ip_tcpdriver_change_timeout(t, TRUE);
	if (srb_debug || srbevent_debug) {
	    buginf("\nRSRB: passive open %i(%d) -> %d",
		   tcb->foreignhost.ip_addr, tcb->foreignport, tcb->localport);
	}
    }
}



/*********************************************************************
 *
 * Peer and Ring-Group Destruction/Creation and Maintence functions
 *
 *********************************************************************/

/*
 * add_vring_interface - Add an interface to a virtual ring
 *
 * This routine adds an interface to a virtual ring. All checks for
 * applicability have been done.
 */

boolean add_vring_interface (
    tr_vgrouptype *vrg,
    hwidbtype *idb,
    ushort kind)
{
    tr_ventrytype *vre;
    srb_sb_t      *srb_sb;
    srb_local_ring_type * srb_local_if;

    if (!idb)
	return(FALSE);

    vre = malloc_vre(rsrb_cb);
    if (!vre)
	return(FALSE);

    vre->virtual_idb = vr_new_vidb(vrg->vrn, idb);
    if (!vre->virtual_idb) {
	printf("%%Interface %s, no virtual idb.\n", idb->hw_namestring);
	free(vre);
	return(FALSE);
    }

    srb_sb = srb_get_or_create_sb_inline(idb->firstsw);
    if (!srb_sb) {
        printf(nomemory);
        return(FALSE);
    }
    vre->bn = srb_sb->srb_bridge_num;
    vre->rn = srb_sb->srb_thisring; 
    vre->type = kind;
    vre->outhole.idb = idb;
    ieee_copy(idb->bia, &vre->macaddr[0]);
    vre->macaddr[0] |= TR_LOCAL;
    idb->tr_vring_blk = (tr_ventrytype *)&vrg->rings.qhead;
    vre->virtualp = virtualp_default;

    ieee_copy(&vre->macaddr[0], vre->virtual_idb->bia);
    ieee_copy(&vre->macaddr[0], vre->virtual_idb->hardware);

    vre->virtual_idb->tr_vring_blk = (tr_ventrytype *)&vrg->rings.qhead;

    enqueue(&vrg->rings, vre);
    vrg->flags |= VRGF_GENUPDATE;

    /*
     *  Add to list of local token ring interfaces for 
     *  dup-ARE-filter stuff.
     */
    if (kind == RSRB_ET_LOCAL) {
        srb_local_if = malloc(sizeof(srb_local_ring_type));
        if (srb_local_if) {
            srb_local_if->idb = idb;
            enqueue(&vrg->local_rings, srb_local_if);
        }
    }

    set_vring_fast_entry(idb);
    return(TRUE);
}

/*
 * set_vring_interfaces
 * This routine is responsible for updating vring and idb structures that deal
 * with local rings.  It ensures that all interfaces assigned to a given
 * virtual ring point to the beginning of the ring entries in the group
 * structure.  It is also responsible for adding new interface entries to the
 * same list.
 *
 * It is not responsible for deletions.  It is easier to deal with them in a
 * different fashion.  Nor are additions or deletions of peer ring entries
 * done here.
 *
 */

void set_vring_interfaces (void)
{
    leveltype status;
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    hwidbtype *idb;
    srb_triplet_t srb_triplet;

    status = raise_interrupt_level(ALL_DISABLE);
    FOR_ALL_HWIDBS(idb) {
	/*
	 * TOKEN RING or FDDI - it's a real ring,
	 * SERIAL and SDLLC or LNX - it's a virtual ring
	 * Channel, it's a virtual ring
	 * anything else isn't; continue
	 */

	if ( !(idb->status & IDB_TR) && !(idb->status & IDB_FDDI) &&
	    !( (idb->status & IDB_SERIAL) && 
	      reg_invoke_srt_other_media(idb) ) &&
	    !(idb->status & IDB_CHANNEL) &&
            !((idb->status & IDB_ATM) && (idb->tr_lane_active)) ) {
	    continue;
	}

        srb_sb_get_triplet(idb->firstsw, &srb_triplet);
	if (srb_triplet.bridgeNum > SRB_MAX_BRIDGE_NUM) {
	    idb->tr_vring_blk = NULL;
	    continue;
	}
	vrg = vrfind_ringgroup(srb_triplet.remoteRing);
	if (!vrg) {
	    idb->tr_vring_blk = NULL;
	    continue;
	}
	
	rsrb_add_virtual_span(vrg);
	vre = vrfind_idb2entry(idb, vrg);
	if (vre) {
	    /* idb->tr_vring_blk already set up */
	    continue;
	}
	if (idb->tr_bridge_idb) {
	    idb->tr_vring_blk = NULL;
	    errmsg(&msgsym(LOCAL, RSRB), idb->unit, srb_triplet.remoteRing);
	}

        if ( (idb->status & IDB_SERIAL) && 
	    reg_invoke_srt_other_media(idb)) {
            /*
             * If there is a virtual idb created for this real
             * interface, make sure that vre wasn't freed underneath me.
             * If there is vre, this interface has all the necessary
             * data structure already created. If not, remove this
             * dangling idb and create new one.
             */
            if (idb->vidb_link) {
                vre = vrfind_idb2entry(idb->vidb_link, vrg);
                /* data structure already exists. */
                if (vre)
                    continue;
                else
                    vidb_free (idb->vidb_link->firstsw);
            }
	    if (reg_invoke_srt_other_media(idb)) {
		if (!reg_invoke_rsrb_add_vring_interface(idb, 
                       srb_triplet.localRing,
		       srb_triplet.bridgeNum, 
                       srb_triplet.remoteRing))

		    errmsg(&msgsym(NOMEMORY, RSRB), idb->unit,
			   "sdllc/qllc vring interface entry");
	    }
	} else {
            /* Handle real token ring and fddi interfaces */
            if (!add_vring_interface(vrg, idb, RSRB_ET_LOCAL))
                errmsg(&msgsym(NOMEMORY, RSRB), idb->unit,
                       "vring interface entry");
                continue;
        }
    }

    reset_interrupt_level(status);
    update_peers();
}

/*
 * remove_vring_interface
 * remove a local interface from a ring group.  Update all local structures
 * and notify any open peers that something has changed.
 *
 * It is assumed that all pertinant bridging cells are set properly and
 * to their new state before this function is called.
 */

void remove_vring_interface (hwidbtype *idb, ushort vrn)
{
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    leveltype status;
    hwidbtype *hwidb;
    spantype *span;
    srb_local_ring_type *srb_if;

    vrg = vrfind_ringgroup(vrn);
    if (!vrg)
	return;				/* not part of a ring group */
    vre = vrfind_idb2entry(idb, vrg);
    if (!vre)
	return;
    hwidb = vre->virtual_idb;
    
    if (vrg->span_idb) {
	span = spanarray[vrg->span_idb->span_index];
	if (span) {
	    if (span->running)
		span_port_disable(span, vrg->span_idb);
	    span_delete_port(vrg->span_idb);
	}
    }

    if (hwidb) {
	reg_invoke_lanmgr_destroy_ring(hwidb);
	vidb_free(hwidb->firstsw);
    }

    /*
     * Is there a matching entry for this vre in the 
     * linked list of local_rings (for dup-ARE-filt)?
     */
    srb_if = NULL;
    if (vre->type == RSRB_ET_LOCAL) {
        for (srb_if = (srb_local_ring_type *) vrg->local_rings.qhead; 
             srb_if; srb_if = srb_if->next) {
             if (vre->outhole.idb == srb_if->idb)
                 break;
        }
    }

    status = raise_interrupt_level(NETS_DISABLE);
    unqueue(&vrg->rings, vre);
    free(vre);

    if (srb_if) {
        unqueue(&vrg->local_rings, srb_if);
        free(srb_if);
    }

    reset_interrupt_level(status);
    vrg->flags |= VRGF_GENUPDATE;
}

/* message errorneous calls to *vre->fastout */

boolean vre_fastout_err(idbtype *idb,     
			idbtype *oidb,
			uchar *dgstart,
			int dgsize)
{
    /* XXX - should use errmsg */
    buginf("vring: fast switching called but not initialized\n");
    return(FALSE);
}

/*
 * rsrb_shutdownallfst
 * Shutdown all FST peers...
 */
void rsrb_shutdownallfst(void)
{
    return;
}

/*
 * vrnail_peer
 * Actually remove a peer and associated structures
 */

void vrnail_peer (tr_vgrouptype *vrg, tr_vpeertype *vrp)
{
    tr_vpeertype *vrptop;
    tr_starttype *start;
    leveltype status;
    tr_vpeertype *temp;
    tr_ventrytype *vre, *vre_next;
    char peerid[MAX_PEER_NAME];
    int i;

    /* Something's already dead... */
    if (!vrg || !vrp)
      return;
    if (srb_debug || srbevent_debug || srberr_debug)
	buginf("\nRSRB: shutting down peer %s", vrpeer_id(vrp, peerid));
    status = raise_interrupt_level(ALL_DISABLE);
    /*
     * Why both to close the thing just before deleting it? Because closing
     * it does nice neat things like deleting entries used by proxy explorer
     * and NetBIOS name caching. While people rarely, if ever, hit this
     * problem in real life, you tend to have it happen from time to time in
     * tests and the result of not doing it is that it appears that no traffic
     * can go through to a remote side, as the proxy explorer will always
     * cause the frames to be converted to specific routes, which get fast
     * switched, and the fast switching code can *never* pass a frame to
     * process level!
     */
    rsrb_newstate(vrp, RSRB_PS_CLOSED);
    switch (vrp->type) {
    case RSRB_PT_IF:
	vrp->outidb->tr_ifpeer = NULL;
	break;
    case RSRB_PT_FST:
	if (vrp->ip_header) {
	    free(vrp->ip_header);
	    vrp->ip_header = NULL;
	}
	break;
    case RSRB_PT_LAN:
    case RSRB_PT_FR:
	/*
	 * Unchain this entry.  Case 1: head of the queue.  Case 2:
	 * somewhere else in the queue.
	 */
	if (vrp->outidb->tr_ifpeer == vrp) {
	    vrp->outidb->tr_ifpeer = vrp->idbnext;
	} else {
	    temp = vrp->outidb->tr_ifpeer;
	    for ( ; temp; temp = temp->idbnext) {
		if (temp->idbnext == vrp) {
		    temp->idbnext = temp->idbnext->idbnext;
		    break;
		}
	    }
	}
	break;
    default:
	break;
    }
    while ((start = dequeue(&vrp->startq))) {
        vr_nukepak(start->pak);
	free(start);
    }
    if (vrp->abort)
	(*vrp->abort)(vrp);
    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; ) {
	vre_next = vre->next;
	if ((vre->type & (RSRB_ET_REMOTE | RSRB_ET_REMSTATIC |
			  RSRB_ET_SDLLC  | RSRB_ET_REMSDLLC |
			  RSRB_ET_QLLC | RSRB_ET_REMQLLC |
                          RSRB_ET_TR_LANE
           )) &&
	      (vre->outhole.vp == vrp)) {
              if ( vre->type & (RSRB_ET_SDLLC | RSRB_ET_QLLC ) )
		  remove_vre(vrg, vre, vre->type);
              else  {
                  unqueue(&vrg->rings, vre);
                  free(vre);
              }
	}
	vre = vre_next;
    }
    if (!vrp->priority_top) {
	unqueue(&vrg->peers, vrp);
    } else {
	vrptop = vrp->priority_top;
	if (vrptop) {
	    for (i = 1; i <= RSRB_PRIORITY_COUNT; i++) {
		if (vrptop->priority_peers[i] == vrp) {
		    vrptop->priority_peers[i] = NULL;
		    break;
		}
	    }
	}
    }
    free(vrp);
    reset_interrupt_level(status);
}

/* add_all_spanidb adds all virtual ring ports to spanning tree array
 */

void add_all_spanidb(int index, spantype *span)
{
    int i;
    tr_vgrouptype *vrg;

    for (i = 0; i < span->port_count; i++) {
        ushort srb_targetring = srb_sb_get_targetring(span->port[i]);
	if (srb_targetring) {
	    vrg = vrfind_ringgroup(srb_targetring);
	    if (vrg) {
		vrg->span_idb->span_index = index;
		span_fixup(index, vrg->span_idb->hwptr, vrg->span_idb);
		rsrb_enable_virtual_span(vrg->span_idb);
	    }
	}
    }
}

/* rsrb_add_virtual_span adds a virtual ring port to spanning tree array
 */

void rsrb_add_virtual_span(tr_vgrouptype *vrg)
{
    int index;
    hwidbtype *hwidb;
    spantype *span;
    boolean found = FALSE;
    list_element *l_elt;

    if (!vrg)
	return;
    index = get_srb_span_index(0);
    if (index >= SPAN_MAXINDEX)
	return;

    span = span_lookup(index, FALSE, SPAN_PROTO_IBM);
    if (!span)
	return;

    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_TOKEN, l_elt, hwidb) {
	if (srb_sb_get_targetring(hwidb->firstsw) == vrg->vrn) {
	    found = TRUE;
	    break;
	}
    }
    if (!found)
	return;
    vrg->span_idb->port_id =
	((vrg->vrn << RD_RNSHIFT) & RD_RNMASK) | srb_sb_get_bridge_num(hwidb->firstsw);

    vrg->span_idb->ibm_port_priority = SPAN_IBM_PORT_PRIORITY;
    vrg->span_idb->ibm_message_age_increment = MESSAGE_AGE_INCREMENT;
    vrg->span_idb->path_cost = 
	SCALED_BANDWIDTH(vrg->span_idb->visible_bandwidth) /
	    span->path_divisor;
    vrg->span_idb->span_index = index;

    /* add this virtual interface to span structure */
    span_fixup(index, vrg->span_idb->hwptr, vrg->span_idb);
    rsrb_enable_virtual_span(vrg->span_idb);
}

/*
 * create_vring
 * create a new empty vring then populate it with any configured local
 * interfaces.  We don't update_peers because we don't have any.
 */

tr_vgrouptype *create_vring (ushort vrn, uchar *vmac)
{
    tr_vgrouptype *vrg;
    leveltype status;

    vrg = vrfind_ringgroup(vrn);
    if (vrg)
	return(vrg);			/* already exists, nop */
    vrg = malloc_named(sizeof(tr_vgrouptype), rsrb_cb);
    if (!vrg)
	return(NULL);			/* whoops */
    vrg->vrn = vrn;
    queue_init(&vrg->peers, 0);
    queue_init(&vrg->rings, 0);
    queue_init(&vrg->local_rings, 0);
    queue_init(&vrg->dead_rings, 0);

    /* SRB AST support */
    /* Create and link a virtual idb used for AST (automatic spanning tree) */
    vrg->span_idb = create_vidb(vrn);
    if (!vrg->span_idb) {
	printf("\nRing Group %d, no virtual idb for spanning tree",
	       vrn);
	return(NULL);
    }

    status = raise_interrupt_level(NETS_DISABLE);
    vrg->largest_frame = RC_LFINITIAL;
    enqueue(&vringQ, vrg);
    reg_invoke_vdlc_default_port(vrg, vmac, TRUE);
    set_vring_interfaces();
    fs_srb_setup();
    fs_explorer_setup();
    rsrb_add_virtual_span(vrg);
    vrg->next_vrp_snmp_index = 1;
    reset_interrupt_level(status);
    return(vrg);
}

/*
 * destroy_vring
 * This routine completely tears a ring group apart.  It throws all of the
 * ring and peer entries back into the free pool followed by the group
 * structure itself.  It then rebuilds all the interface pointers to reflect
 * any changes.
 */

void destroy_vring (int vrn, uchar *vmac)
{
    tr_conntype *vrc, *conn;
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    tr_ventrytype *vre;
    leveltype status;
    srb_local_ring_type *srb_if;
    int i;

    vrg = vrfind_ringgroup(vrn);
    if (!vrg)
	return;				/* nobodies home */

    status = raise_interrupt_level(ALL_DISABLE);

    rsrb_delete_virtual_bridge(vrg);
    /* SRB AST support */
    rsrb_delete_virtual_span(vrg);
    for (vrc = (tr_conntype *) vrconnQ.qhead; vrc; ) {
	if (vrc->vrn != vrn) {
	    vrc = vrc->next;
	    continue;
	}
	conn = vrc;
	vrc = vrc->next;
	unqueue(&vrconnQ, conn);
	free(conn);
    }

    while ( (vrp = (tr_vpeertype *) vrg->peers.qhead) != NULL) {
	if (vrp->priority) {
	    for (i = 1; i <= RSRB_PRIORITY_COUNT; i++)
	      vrnail_peer(vrg, vrp->priority_peers[i]);
	}
	vrnail_peer(vrg, vrp);
    }

    srb_if = NULL;
    while ((srb_if = dequeue(&vrg->local_rings)) != NULL) {
        free(srb_if);
    }

    vre = NULL;
    while (( vre = dequeue(&vrg->rings)) != NULL) {
	if ( vre->type & (RSRB_ET_SDLLC | RSRB_ET_QLLC ) )
	    remove_vre(vrg, vre, vre->type);
	else  {
	    if (vre->virtual_idb)
		vre->virtual_idb->tr_vring_blk = NULL;
	    free(vre);
	}
    }

    /* now that the vring is going away it's time to disable the port */
    reg_invoke_vdlc_default_port(vrg, vmac, FALSE);

    /* DON'T DO THIS BEFORE remove_vre, SINCE
     * remove_vre needs vrg to run vr_rn2entry
     * PHIL 1/24/94
     */
    unqueue(&vringQ, vrg);

/*
 * WHY TURN OFF GLOBALS FOR LOCAL ACK -  PHIL 1/12/94
 */
    sdllc_lak_enable = FALSE;
    lnx_lak_enable = FALSE;

    /*
     * Free SRB subblock associated with vrg->span_idb
     *   then free the VRG
     *
     */
    if (vrg->span_idb) {
        srb_destroy_sb(vrg->span_idb);
    }
    free(vrg);
    set_vring_interfaces();
    fs_srb_setup();
    fs_explorer_setup();
    reset_interrupt_level(status);
    return;
}


static tr_vpeertype *rsrb_newvrp (
    tr_vgrouptype *vrg,
    int vrn,
    int type,
    rsrb_cookie cookie,
    uchar *macaddr,
    int largest,
    boolean local_ack,
    boolean rnr_disable,
    ulong backup_group,
    boolean priority,
    ushort port,
    int force_version,
    netbios_acclisttype *netbios_host_list,
    netbios_acclisttype *netbios_bytes_list,
    ulong tcprcvwnd)
{
    iphdrtype *iphead;
    int largest_def;
    int bytes;
    paktype *pak;
    hwidbtype *idb;
    tr_vpeertype *vrp;
    leveltype status;

    vrp = malloc_named(sizeof(tr_vpeertype), rsrb_cb);
    if (!vrp) {
	printf(nomemory);
	return(NULL);
    }
    vrp->rsrb_lsap_out = 0;
    vrp->rsrb_netb_out = NULL;
    vrp->vrp_version = RSRB_VERSION;
    vrp->state = RSRB_PS_DEAD;
    vrp->type = type;
    vrp->local_ack = local_ack;
    vrp->rnr_disable = rnr_disable;
    GET_TIMESTAMP(vrp->last_keepalive);
    vrp->backup_group = backup_group;
    vrp->priority = priority;
    vrp->netbios_h_oacf = netbios_host_list;
    vrp->netbios_b_oacf = netbios_bytes_list;
    largest_def = RC_LFINITIAL;
    switch (type) {
      case RSRB_PT_TCPD:
	vrp->port = port;
	vrp->ipaddr = cookie.ip;
	vrp->t = NULL;
	vrp->tcp_rcv_window = tcprcvwnd;
	vrp->sender = vrtcpd_sender;
	vrp->direct = vrtcpd_direct;
	vrp->abort = vrtcpd_abort;
	vrp->close = vrtcpd_close;
	vrp->connect = vrtcpd_connect;
	break;

      case RSRB_PT_FST:
	/* First, build the IP/UDP packet with its header */
	if (vrp->local_ack) {
	    printf("\n%%Only TCP remote-peers can use local-acknowledgement");
	    free(vrp);
	    return(NULL);
	}
	if (!rsrb_fst_name) {
	    printf("\n%%An RSRB FST Peername must be set before creating FST remote peers");
	    free(vrp);
	    return(NULL);
	}
	bytes = IPHEADERBYTES(NOPT);
	pak = getbuffer(bytes);		/* get a pak temporarily */
	if (!pak) {
	    free(vrp);
	    return(NULL);
	}
	pak->datagramstart = pak->network_start;
	pak->datagramsize = bytes;
	ipbuild_head(pak, bytes, FST_RSRB_PROT, rsrb_fst_name,
		     cookie.ip,
		     NULL, &ipdefaultparams);
	iphead = (iphdrtype *) pak->datagramstart;
	iphead->dontfragment = TRUE;
	vrp->ip_header = malloc_named(sizeof(iphdrtype), rsrb_cb);
	if (vrp->ip_header == NULL) {
	    printf(nomemory);
	    retbuffer(pak);
	    free(vrp);
	    return(NULL);
	}
	*vrp->ip_header = *iphead;	/* copy the header */
	retbuffer(pak);			/* done with the pak, throw it away */
	vrp->port = port;
	vrp->ipaddr = cookie.ip;

	vrp->t = NULL;
	vrp->sender = vrfst_sender;
	vrp->direct = vrfst_direct;
	vrp->abort = vrfst_abort;
	vrp->close = vrfst_close;
	vrp->connect = vrfst_connect;
	vrp->outidb = NULL;
	largest_def = RC_LF516;
	break;

      case RSRB_PT_FR:
        idb = cookie.fr_cookie.swidb->hwptr;
        vrp->dlci = cookie.fr_cookie.dlci;
        vrp->outswidb =  cookie.fr_cookie.swidb;
       
        if (idb->maxmtu < MAXDGRAM_RSRB) { 
            printf("\n Warning! RSRB: MTU size of Frame Relay interface should be > %d", MAXDGRAM_RSRB);
	}

	vrp->outidb  = idb;
	vrp->sender  = vrif_sender;
	vrp->direct  = vrif_direct;
	vrp->abort   = vrif_abort;
	vrp->close   = vrif_close;
	vrp->connect = vrif_connect;
	largest_def = RC_LF2052;
	if (vrp->local_ack) {
	    printf("\n%%Only TCP remote-peers can use local-acknowledgement");
	    free(vrp);
	    return(NULL);
	}
	break;

      case RSRB_PT_IF:
	/*
	 * If this interface does not support an adjustable maxdgram size,
	 * or if we do not successfully change the maximum datagram size,
	 * clean up and return:
	 */
        idb = cookie.idb;
	if (!supports_maxdgram(idb)) {
	    free(vrp);
	    return(NULL);
	}
	if (idb->set_maxdgram) {
	    /*
	     * This is a hack.  These may or may not be the correct values
	     * to use.  It should do in a pinch
	     */
	    int buffersize;
	    buffersize = (idb->maxmtu > MAXDGRAM_RSRB) ?
	      idb->maxmtu : MAXDGRAM_RSRB;
	    if (!(*idb->set_maxdgram)(idb, buffersize,
				      (buffersize + HDLC_HEADER_SIZE))) {
		free(vrp);
		return(NULL);
	    }
	} else {
	    free(vrp);
	    return(NULL);
	}
	vrp->outidb  = idb;
	vrp->sender  = vrif_sender;
	vrp->direct  = vrif_direct;
	vrp->abort   = vrif_abort;
	vrp->close   = vrif_close;
	vrp->connect = vrif_connect;
	largest_def = RC_LF2052;
	if (vrp->local_ack) {
	    printf("\n%%Only TCP remote-peers can use local-acknowledgement");
	    free(vrp);
	    return(NULL);
	}
	break;

      case RSRB_PT_LAN:
	idb = cookie.idb;
	if (idb->status & IDB_ETHER) {
	    largest_def = RC_LF516;
	} else if (idb->status & IDB_FDDI) {
	    largest_def = RC_LF2052;
	} else if (idb->status & IDB_TR) {
	    /*
	     * Be conservative.  At this point, we have no way of knowing
	     * whether or not the traversed ring is 4M or 16M.
	     */
	    largest_def = RC_LF2052;
	} else {
	    printf("%%interface %s does not support direct encapsulation.\n",
	       idb->hw_namestring);
	    free(vrp);
	    return(NULL);
	}
	if (vrp->local_ack) {
	    printf("\n%%Only TCP remote-peers can use local-acknowledgement");
	    free(vrp);
	    return(NULL);
	}
	ieee_copy(macaddr, vrp->macaddr);
	vrp->outidb  = idb;
	vrp->sender  = vrif_sender;
	vrp->direct  = vrif_direct;
	vrp->abort   = vrif_abort;
	vrp->close   = vrif_close;
	vrp->connect = vrif_connect;
	break;

    }
    if (largest == RC_LFINITIAL) {
	largest = largest_def;
    } else if ((largest > largest_def) && !(type == RSRB_PT_FST)) {
        printf("Media only supports a largest frame of %d.  Lowering to %d.\n",
               rc_code_to_bytes(largest_def), largest_def);
	largest = largest_def;
    }
    vrp->largest_frame_def = largest_def;
    vrp->largest_frame = largest;
    vrp->vrg = vrg;
    rsrb_newstate(vrp, RSRB_PS_CLOSED);
    queue_init(&vrp->startq, RSRB_MAX_STARTQ);
    queue_init(&vrp->lackie, 0);
    vrp->inhdr.op = RSRB_OP_EMPTY;

    /*
     * now do those things that need interrupt lock out
     */
    status = raise_interrupt_level(NETS_DISABLE);
    switch (vrp->type) {
    case RSRB_PT_IF:
	vrp->outidb->tr_ifpeer = vrp;
	set_vring_fast_peer(vrp, cookie.idb);
	break;
    case RSRB_PT_LAN:
	vrp->idbnext = vrp->outidb->tr_ifpeer;
	vrp->outidb->tr_ifpeer = vrp;
	set_vring_fast_peer(vrp, cookie.idb);
	break;
    case RSRB_PT_FST:
	set_vring_fast_peer(vrp, cookie.idb);
	break;
    case RSRB_PT_FR:
	vrp->idbnext = vrp->outidb->tr_ifpeer;
	vrp->outidb->tr_ifpeer = vrp;
	idb = cookie.fr_cookie.swidb->hwptr;
	break;
    case RSRB_PT_NONE:
    case RSRB_PT_TCPD:
	break;
    }
    reset_interrupt_level(status);
    return(vrp);
}

/*
 * create_vrpeer
 * create a new peer entry for a given ring group.
 */

void create_vrpeer (int vrn, int type, rsrb_cookie cookie, uchar *macaddr,
                    int largest, boolean local_ack, boolean rnr_disable,
                    ulong backup_group, boolean priority, int force_version,
                    netbios_acclisttype *netbios_host_list, netbios_acclisttype *netbios_bytes_list,
		    ulong tcprcvwnd)
{
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp, *vrpsub;
    int i;

    vrg = vrfind_ringgroup(vrn);
    if (!vrg) {
	printf("\n%%ring group %d does not exist", vrn);
	return;
    }
    vrp = vrfind_peer(vrg, type, cookie, 0, 0, macaddr);
    /* 
     * Still set these few parameters even if it exists (this allows the user
     * to change them) 
     */
    if (vrp) {
	if (vrp->priority != priority) {
	    printf("\n%%sorry, you cannot change priority usage without "
		   "destroying the peer first");
	    return;
	}
	if (local_ack) {
	    if (type != RSRB_PT_TCPD) {
		printf("\n%%local-ack can only be used with TCP remote peers");
		return;
	    }
	}
	if (type == RSRB_PT_TCPD && vrp->tcp_rcv_window != tcprcvwnd) {
	    printf("\n%%sorry, you cannot change tcp-receive-window without "
		   "destroying the peer first");
	    return;
	}

	vrp->local_ack = local_ack;
	vrp->rnr_disable = rnr_disable;
	vrp->backup_group = backup_group;
	vrp->largest_frame = ((largest != RC_LFINITIAL) ?
			      largest : vrp->largest_frame_def);
        if (vrp->state <= RSRB_PS_CLOSED)
            vrp->vrp_version = RSRB_VERSION;
        vrp->netbios_h_oacf = netbios_host_list;
        vrp->netbios_b_oacf = netbios_bytes_list;
	if (vrp->priority) {
	    for (i = 1; i <= RSRB_PRIORITY_COUNT; i++) {
		vrp->priority_peers[i]->largest_frame =
		  ((largest != RC_LFINITIAL) ?
		   largest : vrp->largest_frame_def);
                vrp->priority_peers[i]->netbios_h_oacf = netbios_host_list;
                vrp->priority_peers[i]->netbios_b_oacf = netbios_bytes_list;
	    }
	}
	return;
    }

    vrp = rsrb_newvrp(vrg, vrn, type, cookie, macaddr, largest,
                      local_ack, rnr_disable,
                      backup_group, priority, TR_RSRB_P0_PORT, force_version,
                      netbios_host_list, netbios_bytes_list, tcprcvwnd);
    if (!vrp)
      return;
    /* Create the priority peers... */
    if (priority) {
	vrpsub = 
          rsrb_newvrp(vrg, vrn, type, cookie, macaddr, largest,
                      local_ack, rnr_disable,
                      backup_group, FALSE, TR_RSRB_P1_PORT, force_version,
                      netbios_host_list, netbios_bytes_list, tcprcvwnd);
	if (!vrpsub)
	  return;
	vrp->priority_peers[1] = vrpsub;
	vrp->priority_peers[1]->local_ack = local_ack;
	vrp->priority_peers[1]->backup_group = backup_group;
	vrp->priority_peers[1]->largest_frame = largest;
	vrpsub->priority_top = vrp;
	vrpsub = 
          rsrb_newvrp(vrg, vrn, type, cookie, macaddr, largest,
                      local_ack, rnr_disable,
                      backup_group, FALSE, TR_RSRB_P2_PORT, force_version,
                      netbios_host_list, netbios_bytes_list, tcprcvwnd);
	if (!vrpsub)
	  return;
	vrp->priority_peers[2] = vrpsub;
	vrp->priority_peers[2]->local_ack = local_ack;
	vrp->priority_peers[2]->backup_group = backup_group;
	vrp->priority_peers[2]->largest_frame = largest;
	vrpsub->priority_top = vrp;
	vrpsub = 
          rsrb_newvrp(vrg, vrn, type, cookie, macaddr, largest,
                      local_ack, rnr_disable,
                      backup_group, FALSE, TR_RSRB_P3_PORT, force_version,
                      netbios_host_list, netbios_bytes_list, tcprcvwnd);
	if (!vrpsub)
	  return;
	vrp->priority_peers[3] = vrpsub;
	vrp->priority_peers[3]->local_ack = local_ack;
	vrp->priority_peers[3]->backup_group = backup_group;
	vrp->priority_peers[3]->largest_frame = largest;
	vrpsub->priority_top = vrp;
    }
    vrp->priority_top = NULL;
    vrp->snmp_index = vrg->next_vrp_snmp_index++;
    enqueue(&vrg->peers, vrp);
}

/*
 * destroy_vrpeer
 * nuke a given peer for a ring group.  This also rifles through
 * the ring entries purging any remote entries that refer to this
 * peer.
 */

void destroy_vrpeer (int vrn, int type, rsrb_cookie cookie, uchar *macaddr)
{
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    tr_conntype *vrc, *conn;
    int i;

    vrg = vrfind_ringgroup(vrn);
    if (!vrg) {
	printf("\n%%ring group %d does not exist", vrn);
	return;
    }
    vrp = vrfind_peer(vrg, type, cookie, 0, 0, macaddr);
    if (!vrp) {
	printf("\n%%peer not found for ring group %d", vrn);
	return;
    }
    if (vrp->priority) {
	for (i = 1; i <= RSRB_PRIORITY_COUNT; i++)
	  vrnail_peer(vrg, vrp->priority_peers[i]);
    }

    for (vrc = (tr_conntype *) vrconnQ.qhead; vrc; ) {
        if (vrc->vrp != vrp) {
            vrc = vrc->next;
            continue;
        }
        if (srberr_debug)
            buginf("\ndestroy_vrpeer - destroying reference to vrp in vrconnQ");
        conn = vrc;
        vrc = vrc->next;
        unqueue(&vrconnQ, conn);
        free(conn);
    }

    vrnail_peer(vrg, vrp);
}

/*********************************************************************
 *
 * Generic Forwarding RSRB code
 *
 *********************************************************************/

/*
 * vrforward_send_all_peers
 */
void vrforward_send_all_peers (
    paktype *ppak,
    tr_vpeertype *input_vrp)
{
    trrif_t *trh;
    paktype *opak;
    idbtype *idb;
    tr_vhdrtype lhdr;
    tr_vpeertype *vrp;
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    char peerid[MAX_PEER_NAME];

    /*
     * Find the vring group
     */
    if (input_vrp) {
	idb = NULL;
	vrg = input_vrp->vrg;
    } else {
	idb = ppak->if_input;
	vrg = vrfind_ringgroup(srb_sb_get_thisring(idb));
    }

    /*
     * First duplicate and sent to all peers.
     */
    if (!input_vrp) {
	lhdr.op = RSRB_OP_EXPLORER;
	lhdr.vrn = vrg->vrn;
	lhdr.offset = 0;
	lhdr.len = ppak->datagramsize;
	for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
	    if (ip_ouraddress(vrp->ipaddr))
		continue;
	    if (srb_debug)
		buginf("\nRSRB: multicast frame to %s srn %d [%s]",
		       vrpeer_id(vrp, peerid), srb_sb_get_thisring(idb),
		       get_rif_str((uchar *)
				   &(((trrif_t *)
				      ppak->datagramstart)->rc_blth),
				   rif_string));
	    opak = pak_duplicate(ppak);
	    if (!opak)
		continue;
	    trh = (trrif_t *) opak->datagramstart;
	    lhdr.rhdr_version = vrp->vrp_version;
	    (void) (*vrp->sender)(vrp, &lhdr, opak);
	}
    }

    /*
     * Now duplicate and input on any local virtual rings.
     */
    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
	if ((vre->type != RSRB_ET_LOCAL) &&
	    (vre->type != RSRB_ET_LOCAL_VIRTUAL))
	    continue;
	opak = pak_duplicate(ppak);
	if (!opak)
	    continue;
        opak->peer_ptr = input_vrp;
        vr_vre_input(vre, opak, FALSE, RSRB_OP_EXPLORER);
    }
    if (srb_debug)
	buginf("\nRSRB: multicast frame to local interfaces");
    vr_nukepak(ppak);
}

/*
 * vrforward_explorer
 * flood an explorer packet to the appropriate nodes belonging to
 * this vring.  We first scan the ring list sending out any interfaces
 * that haven't seen this packet yet.  Spanning are only sent out
 * interfaces that are forwarding.  Next the peer list is scanned and
 * a message is sent to each one.  We don't do any throttling because
 * we don't have enough information.  Passing that level of information
 * around is death.
 *
 * This routine can be used by a local interface to forward to any
 * local interfaces and all remote peers (pak->if_input->tr_vring_blk
 * non-NULL) or by the input forwarder process for the remote input side
 * of a peer (input_vrp non-NULL).  These are intended to be mutually
 * exclusive.
 */

void vrforward_explorer (paktype *pak, int offset, tr_vpeertype *input_vrp,
			 int unit)
{
    paktype *opak = NULL;
    paktype *ppak = NULL;
    hwidbtype *idb, *oidb, *fddiidb;
    trrif_t *otrh;
    srbrd_t *rd;
    int rd_cnt, start_cnt, netbios_offset, mtu_lf;
    tr_vgrouptype *vrg;
    tr_ventrytype *vre, *first_vre;
    tr_vpeertype *vrp, *first_vrp;
    tr_vhdrtype lhdr;
    boolean spanning, flush;
    char peerid[MAX_PEER_NAME];
    uchar backup_group_forwards[((BACKUP_GROUP_COUNT / 8) + 1)];
    trrif_t *trh = (trrif_t *) pak->datagramstart;
    int riflen = trh->rc_blth & RC_LTHMASK;
    sap_hdr *llc1 = (sap_hdr *)((uchar *)&trh->rc_blth + riflen);
    uchar llc1_control = (llc1->control & ~LLC1_P);
    netbios_header *netbios_hdr;

    memset(backup_group_forwards, 0, ((BACKUP_GROUP_COUNT / 8) + 1));
    idb = NULL;
    ppak = NULL;
    fddiidb = NULL;

    if (!(trh->saddr[0] & TR_RII)) {
        if (srberr_debug) {
            buginf("vrforward_explorer - RII not set");
            bugtrace();
	    /* vrforward_send_all_peers(pak, input_vrp); */
        }
	return;
    }

    if (input_vrp)
	vrg = input_vrp->vrg;

    else{
        ushort srb_targetring;
	idb = hwidb_or_null(pak->if_input);
	if (idb->status & IDB_FDDI)
	    fddiidb = idb;
        srb_targetring = srb_sb_get_targetring(firstsw_or_null(idb));
	vrg = (idb ? vrfind_ringgroup(srb_targetring) : NULL);
	if (!vrg) {
	    if (idb) {
		if (srb_debug || srbevent_debug || srberr_debug)
		    buginf("\nRSRB%d: ring group %d not found",
			   idb->unit, srb_targetring);
                srb_sb_incr_rx_drops(idb->firstsw);
	    } else
	      errmsg(&msgsym(NOTREM, RSRB));
	    vr_nukepak(pak);
	    return;
	}
    }
    spanning = ((trh->rc_blth & RC_RTMASK) == RC_B_SPAN);
    start_cnt = (trh->rc_blth & RC_LTHMASK) - 4;

    netbios_offset = (TRING_ENCAPBYTES + riflen + ET_SAP_LEN);
    /*
     **  If this is not a NetBIOS packet with a name in it, then let it
     **  through.
     **
     **  N.B. This logic must match the same NetBIOS packet examination
     **      logic that exists in netbios_acfcheck().
     */
    netbios_hdr = (netbios_header *) (pak->datagramstart + netbios_offset);
    if (((llc1_control != LLC1_UI) || (llc1->dsap != SAP_NETBIOS))
    || (netbios_hdr->delimiter[0] != NETBIOS_DELIMITER_0)
    || (netbios_hdr->delimiter[1] != NETBIOS_DELIMITER_1)) {
        netbios_offset = 0;
    }
    if (TRING_ENCAPBYTES + riflen >= pak->datagramsize) {
	if (srbevent_debug || srberr_debug) {
	    buginf("\nTR%d: BADLEN: Illegal vring input explorer %d, min legal %d",
		   99, pak->datagramsize, TRING_ENCAPBYTES + riflen + SAP_HDRBYTES);
	    tr_print_packet(NULL, "BADLEN", (tring_hdr *) trh,
			    (ulong *)(&trh->rc_blth + riflen), 0,
			    pak->datagramsize);
	}
	vr_nukepak(pak);
	return;
    }

    if (start_cnt < 4) {
	if (input_vrp)
	    input_vrp->drops++;
	else
            srb_sb_incr_rx_drops(idb->firstsw);
	if (srb_debug || srbevent_debug || srberr_debug)
	    buginf("\nRSRB%d: explorer rif [%s] not long enough", unit,
		   get_rif_str((uchar *) &trh->rc_blth, rif_string));
	vr_nukepak(pak);
	return;
    }

    /*
     * Possibly cut down the maximum packet size.  Users can configure
     * this to help prevent delay timeouts on long lines.
     */
    if ((trh->rc_dlf & RC_LFMASK) > vrg->largest_frame)
	trh->rc_dlf = (trh->rc_dlf & ~RC_LFMASK) | vrg->largest_frame;

    first_vre = NULL;
    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
	if (vre->type & (RSRB_ET_LOCAL_VIRTUAL | RSRB_ET_LOCAL | RSRB_ET_TR_LANE)) {
	    if (vre->outhole.idb->state != IDBS_UP)
		continue;
	    if (vre->outhole.idb == idb)
		continue;
	    if (spanning) { 
                idbtype *swidb = firstsw_or_null(vre->outhole.idb);
                if (!srb_sb_get_spanning_explorer(swidb))
		  continue;
            }
	} else
	    continue;			/* only do locals in this section */

	flush = FALSE;
	/*
	 * scan packet's rif for already being on the candidate ring
	 */
	rd = (srbrd_t *) &trh->rd[0];
	for (rd_cnt = start_cnt; rd_cnt; rd_cnt -= 2) {
	    if (((GETSHORT(rd) & RD_RNMASK) >> RD_RNSHIFT) == vre->rn) {
		flush = TRUE;
		break;
	    }
	    rd++;
	}
	if (flush)
	    continue;

	oidb = vre->outhole.idb;

	if ((input_vrp || !(vre->type & RSRB_ET_LOCAL)) &&
	    (oidb->status & IDB_FDDI)) {
	    if (srb_debug)
		buginf("\nRSRB: explorer to %s dropped.",
		       oidb->hw_namestring);
	    continue;
	}

        if(spanning) {
            /* check for max output hop count */
            ushort maxout_hops = srb_sb_get_maxout_hops(oidb->firstsw);
            if ( riflen > ( (maxout_hops + 1) * 2 + 2 ) )
            {
                srb_sb_incr_tx_drops(oidb->firstsw);
                continue;
            }
        }

	if (!srb_out_check((uchar *) pak->datagramstart, pak->datagramsize,
			  oidb, ((ushort) trh->rc_blth & RC_LTHMASK)))
	    continue;
	
        /*  CIP internal LAN process-swtiched explorers filter */
        if (is_cip_lan(oidb)) {
            channel_sb_t *chan_sb = channel_get_sb(oidb);
            if ((chan_sb) && 
                (chan_sb->get_vc_from_macaddr(oidb, trh->daddr, 
                                    trh->saddr) == (ushort)NO_CHANNEL_VC)) {
                srb_sb_incr_tx_drops(oidb->firstsw);
		continue;
            }
        }

	vre->explorers++;
        srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, pak->datagramsize);

	if (first_vre) {

	    /* Check to see if this packet has already been fast forwarded
	     * from interrupt level.  If not add this oidb to the explorer 
	     * fast forward queue and send it out now.  Only packets from an
	     * interface that can fastswitch explorers which are destined for
	     * a local ring are considered. Note that checking PAK_SRB_EXP_SENT
             * after function call assures all enqueued packets are sent.*/

            if((pak->flags & PAK_SRB_EXP_FAST) 
	    && (vre->type == RSRB_ET_LOCAL 
	    ||  vre->type == RSRB_ET_LOCAL_VIRTUAL)
	    && fs_explorer_check(spanning, idb, oidb)
	    && (pak->flags & PAK_SRB_EXP_SENT))
		continue;

	    opak = pak_duplicate(pak);
	    if (!opak) {
		vr_nukepak(pak);
		return;
	    }
	    rd = (srbrd_t *) ((uchar *) opak->datagramstart + offset);
	    PUTSHORT(rd, ((GETSHORT(rd) & RD_RNMASK) | (vre->bn & RD_BNMASK)));
	    PUTSHORT(++rd, ((vre->rn << RD_RNSHIFT) & RD_RNMASK));
	    opak->if_output = oidb->firstsw;
	    opak->enctype = ET_NULL;
	    opak->linktype = LINK_RSRB;
	    opak->flags |= PAK_SRB_EXPLORER;
	    otrh = (trrif_t *) opak->datagramstart;
	    mtu_lf = bytes_to_rc_code(llc_mtu(oidb));
	    if ((otrh->rc_dlf & RC_LFMASK) > mtu_lf)
		otrh->rc_dlf = mtu_lf;
	    if (srb_debug)
		buginf("\nRSRB%d: sent explorer, bn %d trn %d, [%s]",
		     idb->unit,vre->bn,vre->rn,
	             get_rif_str((uchar *)&otrh->rc_blth, rif_string));
	    tr_to_media_out(opak);
            srb_sb_incr_tx_b_counters(oidb->firstsw, pak->datagramsize);
	    if (spanning)
                srb_sb_incr_span_out_counter(oidb->firstsw);
	    else
                srb_sb_incr_allrte_out_counter(oidb->firstsw);

	} else {
	    first_vre = vre;

            if(!idb)
                idb = vre->virtual_idb;

	    rd = (srbrd_t *) ((uchar *) pak->datagramstart + offset);
	    if ((((GETSHORT(rd) & RD_RNMASK) >> RD_RNSHIFT) != vrg->vrn) && 
                (srb_debug || srbevent_debug || srberr_debug))
		buginf("\nRSRB%d: explorer forward, bad rif, s: %e d: %e rif: %s",
		       idb->unit, trh->saddr, trh->daddr,
		       get_rif_str((uchar *) &trh->rc_blth, rif_string));
	    PUTSHORT(rd, ((GETSHORT(rd) & RD_RNMASK) | (vre->bn & RD_BNMASK))); 
	    PUTSHORT(++rd, ((vre->rn << RD_RNSHIFT) & RD_RNMASK));
	    pak->if_output = oidb->firstsw;
	    pak->enctype = ET_NULL;
	    pak->linktype = LINK_RSRB;
	    pak->flags |= PAK_SRB_EXPLORER;
	}
    }
    if (first_vre) {
	/* Check to see if this packet has already been fast forwarded
	 * from interrupt level.  If not add this oidb to the idb explorer 
	 * fast forward queue and send it out now.  Only packets from
	 * an interface that can fastswitch explorers which are destined
	 * for a local ring are considered.  Note that checking PAK_EXP_SENT
	 * after the function call assures that all enqueued packets are sent.*/

        if((pak->flags & PAK_SRB_EXP_FAST) 
	&& (first_vre->type == RSRB_ET_LOCAL 
	||  first_vre->type == RSRB_ET_LOCAL_VIRTUAL)
        && fs_explorer_check(spanning, idb, first_vre->outhole.idb)
	&& (pak->flags & PAK_SRB_EXP_SENT))
            ;

	else {
	    if (spanning) 
                srb_sb_incr_span_in_counter(idb->firstsw);
	    else
                srb_sb_incr_allrte_in_counter(idb->firstsw);

	    if (srb_debug)
		buginf("\nRSRB%d: sent explorer, bn %d trn %d, [%s]",
		       idb->unit, first_vre->bn, first_vre->rn,
		       get_rif_str((uchar *)&trh->rc_blth,rif_string));
	    if (input_vrp) {
	        otrh = (trrif_t *) pak->datagramstart;
	        oidb = pak->if_output->hwptr;
	        mtu_lf = bytes_to_rc_code(llc_mtu(oidb));
	        if ((otrh->rc_dlf & RC_LFMASK) > mtu_lf)
		    otrh->rc_dlf = mtu_lf;
	        tr_to_media_out(pak);
                srb_sb_incr_tx_b_counters(oidb->firstsw, pak->datagramsize);
	        if (spanning)
                    srb_sb_incr_span_out_counter(oidb->firstsw);
	        else
                    srb_sb_incr_allrte_out_counter(oidb->firstsw);
	        return;
	    } else {
	        opak = pak_duplicate(pak);
	        if (opak) {
		    otrh = (trrif_t *) opak->datagramstart;
		    oidb = opak->if_output->hwptr;
		    mtu_lf = bytes_to_rc_code(llc_mtu(oidb));
		    if ((otrh->rc_dlf & RC_LFMASK) > mtu_lf)
		        otrh->rc_dlf = mtu_lf;
		    tr_to_media_out(opak);
                    srb_sb_incr_tx_b_counters(oidb->firstsw, pak->datagramsize);
	            if (spanning)
                        srb_sb_incr_span_out_counter(oidb->firstsw);
	            else
                        srb_sb_incr_allrte_out_counter(oidb->firstsw);
	        }
	    }
	}
    } else if (input_vrp) {		/* from remote, doing only locals */
	vr_nukepak(pak);
	return;
    }

    if (fddiidb) {
	if (srb_debug)
	    buginf("\nRSRB: explorer from %s to remote peer dropped.",
		   fddiidb->hw_namestring);
	vr_nukepak(pak);
	return;
    }

    ppak = pak;
    lhdr.op = RSRB_OP_EXPLORER;
    lhdr.vrn = vrg->vrn;
    lhdr.offset = offset;
    lhdr.len = ppak->datagramsize;
    first_vrp = NULL;
    rd = (srbrd_t *) ((uchar *) ppak->datagramstart + offset);
    PUTSHORT(rd, GETSHORT(rd) & RD_RNMASK); 	/* rd[0].bridge_num = 0 */
    PUTSHORT(++rd, 0);
    for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
	if (ip_ouraddress(vrp->ipaddr))
	    continue;

        /* SRB AST support */
        if (spanning) {
	    if (!rsrb_vidb_span_forward(vrg, vrp))
		continue;
	}
	if (vrp->backup_group != RSRB_BACKUP_GROUP_NONE) {
	    if ((vrp->type == RSRB_PT_LAN) ||
		(vrp->type == RSRB_PT_IF) ||
		(vrp->type == RSRB_PT_FR)) {
		if (!hw_interface_up(vrp->outidb)) {
		    continue;
		}
		if (backup_group_forwards[vrp->backup_group >> 3] &
		    (0x80 >>  (vrp->backup_group & 0x07))) {
		    continue;
		}
		if (vrp->state == RSRB_PS_CONNECTED) {
		    backup_group_forwards[vrp->backup_group >> 3] |= 
			(0x80 >> (vrp->backup_group & 0x07));
		}
	    }
	}
        if ((vrp->netbios_h_oacf) && (netbios_offset)) {
            if (!netbios_expr_host_check(ppak->datagramstart + netbios_offset,
                                         ppak->datagramsize - netbios_offset,
					 vrp->netbios_h_oacf)) {
                if (srb_debug) {
                    buginf("\nRSRB: Dropping NetBIOS frame (host) for remote peer %s",
                           vrpeer_id(vrp, peerid));
                }
                continue;
            }
        }
        if ((vrp->netbios_b_oacf) && (netbios_offset)) {
            if (!netbios_expr_bytes_check(ppak->datagramstart + netbios_offset,
                                         ppak->datagramsize - netbios_offset,
                                          vrp->netbios_b_oacf)) {
                if (srb_debug) {
                    buginf("\nRSRB: Dropping NetBIOS frame (bytes) for remote peer %s",
                           vrpeer_id(vrp, peerid));
                }
                continue;
            }
        }
	vrp->explorers++;
	if (srb_debug)
	    buginf("\nRSRB: remote explorer to %s srn %d [%s]",
		   vrpeer_id(vrp, peerid), srb_sb_get_thisring(idb->firstsw),
		   get_rif_str((uchar *) &(((trrif_t *)ppak->datagramstart)->rc_blth),
			       rif_string));
	if (first_vrp) {
	    /* check to see whethter we need to do any rsrb filtering */
	    if (!rsrb_out_check((uchar *) ppak->datagramstart,
	        ppak->datagramsize, idb, ((ushort) trh->rc_blth & RC_LTHMASK),
	        vrp)) {
	        continue;
	    }
	    if (vrp->type == RSRB_PT_FST)
	        opak = rsrb_fst_full_duplicate(ppak, NULL, TRUE);
	    else
		opak = pak_duplicate(ppak);
	    if (!opak)
		continue;
	    trh = (trrif_t *) opak->datagramstart;
            lhdr.rhdr_version = vrp->vrp_version;
	    if ((trh->rc_dlf & RC_LFMASK) > vrp->largest_frame)
		trh->rc_dlf = (trh->rc_dlf & ~RC_LFMASK) |
		    vrp->largest_frame;
	    (void) (*vrp->sender)(vrp, &lhdr, opak);
	} else
	    first_vrp = vrp;
    }
    if (first_vrp) {
	trh = (trrif_t *) ppak->datagramstart;
        lhdr.rhdr_version = first_vrp->vrp_version;
	if ((trh->rc_dlf & RC_LFMASK) > first_vrp->largest_frame)
	    trh->rc_dlf = (trh->rc_dlf & ~RC_LFMASK) | first_vrp->largest_frame;
	/* check to see whethter we need to do any rsrb filtering */
	if (!rsrb_out_check((uchar *) ppak->datagramstart, ppak->datagramsize, 
			    idb, ((ushort) trh->rc_blth & RC_LTHMASK), first_vrp)) {
	    vr_nukepak(ppak);
	    return;
	}
	(void) (*first_vrp->sender)(first_vrp, &lhdr, ppak);
    } else
	vr_nukepak(ppak);
}

/*
 * fs_explorer_check()
 *
 * Has explorer already been fast forwarded at interrupt level?
 * If the output idb is in the fast queue then assume it has.
 * Otherwise update the fast queue so it'll be there next time
 * and return false so the packet will be sent now.
 *
 * If the global fs_explorer.prune flag has been set prune the queue.
 */

boolean fs_explorer_check (
    boolean   spanning,
    hwidbtype *idb,
    hwidbtype *oidb)
{
    fs_expl_que *fastQ = spanning ? idb->fs_expl_spanQ : idb->fs_expl_allQ;
    fs_expl_que *entry, *temp;

    if(!fs_explorer.enabled)
	return(FALSE);

    /* Does the queue need pruning ? */

    if(fs_explorer.prune) {

	for(entry=fastQ; entry; entry=entry->next)
	    if(entry->next && entry->next->prune) {
		temp = entry->next;
		entry->next = temp->next;
		free(temp);
                fs_explorer.prune--;
		}

	if(fastQ && fastQ->prune) {
	    temp = fastQ;
	    if(spanning)
		idb->fs_expl_spanQ = fastQ = fastQ->next;
            else
		idb->fs_expl_allQ = fastQ = fastQ->next;
	    free(temp);
            fs_explorer.prune--;
	    }
	}

    /* If interface went down or fast-explorers was de-configured the
     * idb was just pruned so don't put it back on the fast queue. */

    if (oidb->state != IDBS_UP || !oidb->fs_expl_func)
	return(FALSE);

    /* Now update the queue. */

    if(!fastQ) {
	temp = malloc(sizeof(fs_expl_que));

	if(!temp)
	    return(FALSE);

	temp->idb = oidb;
	if(spanning)
            idb->fs_expl_spanQ = temp;
        else
	    idb->fs_expl_allQ  = temp;
	}
    else{
        for(entry=fastQ; entry; entry=entry->next) {
	    if(entry->idb == oidb)
	        return(TRUE);

	    /* The out idb is not listed ... add a new entry. */

            if(!entry->next) {
	        temp = malloc(sizeof(fs_expl_que));
	        if(temp) {
	            temp->idb = oidb;
		    entry->next = temp;
		    }
		break;
	        }
	    }
	}

    return(FALSE);
}

/*
 * vrforward_pak
 * forward a regular (specific route) packet to the appropriate
 * peer or local ring.  This entails finding the next step in the
 * RIF which will yield the next ring/bridge to go over.  This
 * will allow us to find the next hop in the ring lists.
 *
 * This routine should only be called if pak->if_input->tr_vring_blk
 * is non NULL and the pak has a rif.  Some of this is checked by the
 * driver and others checked by the incoming interface.
 * 
 * If TYPE is specified, frame does not get local_ack checked, and is sent as
 * the frame type specified.
 *
 * processRifLf - RSRB_RIFLF_LEAVEALONE: Process input as always
 *                RSRB_RIFLF_KNOCKDOWN:  Check Rif's LF field and lower value 
 *                          based on the minimum of the sizes supported by:
 *                          a) Lf of incoming frame
 *                          b) Lf of ring group's Lf value
 *                             (source-bridge largest-frame [ringGoup] [size])
 *                          c) Lf of peer's Lf value
 *                             (source-bridge remote-peer .... [lf size])
 */

void vrforward_pak (paktype *pak, boolean sdllc_frame, int type, boolean processRifLf)
{
    ushort rd_offset = 0;
    hwidbtype *idb, *vidb;
    idbtype   *swidb;
    trrif_t *trh;
    ushort bn, rn, targetring = 0;
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    tr_ventrytype *vre;
    tr_vhdrtype lhdr;
    char peerid[MAX_PEER_NAME];
    char *why;
    boolean dontsetbridge = FALSE;
    uchar llc2_type = 0;
    lak_pass_t passthru = LAK2_PASSTHRU;

    swidb = pak->if_input;
    idb = swidb->hwptr;
    vidb = idb->vidb_link;
    trh = (trrif_t *) pak->datagramstart;
    /*
     * Packets that come from a virtual interface, or those that are actually
     * explorers that we are forwarding, don't have a CRC and should use
     * the source address from the frame itself instead of from the card.
     */
    if ((idb->status & IDB_VIRTUAL) 
	|| (type == RSRB_OP_FORWARDCRC)
        || (type == RSRB_OP_FORWARDFSTCRC)
        || (is_cip_lan(idb))) {
        dontsetbridge = TRUE;
	pak->flags |= PAK_SRB_EXPLORER;
    }
    /*
     * From a virtual interface, to another virtual interface.
     */
    if (!(trh->saddr[0] & TR_RII)) {
        if (srberr_debug) {
            buginf("vrforward_pak - RII not set");
            bugtrace();
        }
        goto endit;
	vrg = vrfind_ringgroup(srb_sb_get_thisring(swidb));
	if (!vrg) {
            srb_sb_incr_rx_drops(idb->firstsw);
	    if (srbevent_debug)
		buginf("\nRSRB: virtual to virtual, couldn't find virtual ring for %s",
		       idb->hw_namestring);
	    vr_nukepak(pak);
	    return;
	}
	for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
	    if (GETSHORT(trh->daddr) == TRFUNC_UPPER) {
		/* count vring transmit and receive for LNM */
                if (vidb && (vidb != RSRB_INVALID_ADDRESS)) {
                    srb_sb_incr_tx_nb_counters(vidb->firstsw, 
                                               pak->datagramsize);
                }
                srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, 
                                        pak->datagramsize);
		vr_vre_input(vre, pak, FALSE, type);
		return;
	    }
	    if (ieee_equal(vre->macaddr, trh->daddr)) {
		if ((vre->type != RSRB_ET_LOCAL) &&
		    (vre->type != RSRB_ET_LOCAL_VIRTUAL)) {
		    bn = 0;
		    rn = srb_sb_get_thisring(swidb);
		    goto forwardit;
		} else {
		    /* count vring transmit and receive for LNM */
                    if (vidb && (vidb != RSRB_INVALID_ADDRESS)) {
                        srb_sb_incr_tx_nb_counters(vidb->firstsw, 
                                                   pak->datagramsize);
                    }   
                    srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, 
                                            pak->datagramsize);
		    vr_vre_input(vre, pak, FALSE, type);
		    return;
		}
	    }
	}
endit:	vr_nukepak(pak);
	return;
    }

    /*
     * Do the special cases of the RD *ending* on the virtual ring.
     */
    targetring = srb_sb_get_targetring(swidb);
    if (!(idb->status & IDB_VIRTUAL) && (RIF_DEST_RING(trh) == targetring)) {
        vrg = vrfind_ringgroup(targetring);
        if (!vrg) {
           srb_sb_incr_rx_drops(idb->firstsw);
	   if (srbevent_debug)
	       buginf("\nRSRB: RD on virtual ring but couldn't find it for %s",
		      idb->hw_namestring);
           vr_nukepak(pak);
           return;
        }

  	for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next)  {
             if (vre->type & (RSRB_ET_SDLLC | RSRB_ET_QLLC | 
                              RSRB_ET_LOCAL | RSRB_ET_LOCAL_VIRTUAL) )  {
                 boolean addressmatch = FALSE;
                 if (vre->virtualp)
                     addressmatch = vre->virtualp(vre, trh->daddr);
                 else
                     addressmatch = virtualp_default(vre, trh->daddr);
                 if (addressmatch)  {
                     /* count vring transmit and receive for LNM */
                     if (vidb && (vidb != RSRB_INVALID_ADDRESS))  {
                         srb_sb_incr_tx_nb_counters(vidb->firstsw, 
                                                    pak->datagramsize);
                     }
                     srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, 
                                             pak->datagramsize);
                     vr_vre_input(vre, pak, FALSE, type);
                     return;
                 }
             } else {
                 if (ieee_equal(vre->macaddr, trh->daddr)) {
                     srb_triplet_t srb_triplet;
                     srb_sb_get_triplet(swidb, &srb_triplet);
                     bn = srb_triplet.bridgeNum;
                     rn = srb_triplet.remoteRing;
                     srb_sb_incr_tx_nb_counters(vidb->firstsw, 
                                                pak->datagramsize);
                     goto forwardit;
                 }
             }
        } 
	vr_nukepak(pak);
	return;
    }

    /*
     * Now do the special case of the RD *starting* on the virtual ring
     */
    if (sdllc_frame) {
        sap_entry_t *sap_entry;
        hwaddrtype interface_mac;
 
        sap_entry = sap_get_bridging_entry(idb->firstsw);
        (*idb->extract_hwaddr)(idb, &interface_mac);
        if (sap_entry && (!ieee_equal(trh->saddr, &interface_mac.addr[0]))) {
          bn = RIF_THIRD_BRIDGE(trh);
          rn = RIF_FOURTH_RING(trh);
        } else {
          bn = RIF_SECOND_BRIDGE(trh);
          rn = RIF_THIRD_RING(trh);
        }

	dontsetbridge = TRUE;
	pak->flags |= PAK_SRB_EXPLORER;
    } else if ((idb->status & IDB_VIRTUAL) &&
	       (!idb->firstsw->span_index) &&
	       (RIF_START_RING(trh) == srb_sb_get_thisring(swidb))) {
	bn = RIF_START_BRIDGE(trh);
	rn = RIF_SECOND_RING(trh);
	dontsetbridge = TRUE;
	pak->flags |= PAK_SRB_EXPLORER;
    } else {
        srb_triplet_t srb_triplet;
        srb_sb_get_triplet(swidb, &srb_triplet);
	rd_offset = rif_match_offset((srbroute_t *) &trh->rc_blth, 
                                     srb_triplet.localRing,
	        		     srb_triplet.bridgeNum, 
                                     srb_triplet.remoteRing,
				     &rn, &bn, &why);
	if (!rd_offset) {
	    if (srb_debug || srberr_debug)
		buginf("\nRSRB%d: rif match failed, %s, d: %e s: %e rif: %s",
		       idb->unit, why, trh->daddr, trh->saddr,
		       get_rif_str((uchar *) &trh->rc_blth, rif_string));
            srb_sb_incr_rx_drops(idb->firstsw);
	    vr_nukepak(pak);
	    return;
	}
    }

    /*
     * We've gotten this far.  Count it as transmitted onto the vring, as
     * far as LNM is concerned.
     */
    if (vidb && (vidb != RSRB_INVALID_ADDRESS)) {
        srb_sb_incr_tx_nb_counters(vidb->firstsw, pak->datagramsize);
    }

    /*
     * Now find the table entry for the destination cisco on the other
     * side of the virtual ring.
     */
    vre = vrfind_rn2entry(rn, bn, idb->tr_vring_blk, RSRB_ET_EITHER);
    if (!vre) {
	if (srb_debug || srbevent_debug) {
            srb_triplet_t srb_triplet;
            srb_sb_get_triplet(swidb, &srb_triplet);
	    buginf("\nRSRB%d: no forward target (srn %d bn %d vrn %d), src: %e dst: %e [%s]",
		   idb->unit,
                   srb_triplet.localRing,
                   srb_triplet.bridgeNum,
                   srb_triplet.remoteRing,
		   trh->saddr, trh->daddr,
		   get_rif_str((uchar *) &trh->rc_blth, rif_string));
	}
	vr_nukepak(pak);
        srb_sb_incr_rx_drops(idb->firstsw);
	/*
	 * With proxy-explorers it is possible that we have responded
	 * to a request which is now trying to get through us but we
	 * haven't opened up the remote connections yet.  To solve this
	 * problem we run through the peer list and for any connections
	 * that are closed send a nop.  We don't do this to ourselves.
	 *
	 * THIS IS GROSS!
	 */
        if (global_proxy_explorer_enable) {
	 vrg = vrfind_ringgroup(srb_sb_get_targetring(swidb));
	 if (!vrg)
	    return;
	 for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
	     if (vrp->state != RSRB_PS_CLOSED)
	 	continue;
	     if (ip_ouraddress(vrp->ipaddr))
	 	continue;
	     pak = vrpak(vrp->vrp_version, RSRB_OP_NOP, 0, vrp->vrg->vrn, 0, 0);
	     if (!pak || !(*vrp->sender)(vrp, NULL, pak))
		vrforce_xchg_delayed(vrp->vrg);
	 }
        }
	return;
    }

    /*
     * Apply ring _group_ based largest frame explorer reduction
     * Some code calls this function with explorer packets (bypassing original
     * vrforward_explorer) to "direct" an explorer towards a known peer.  Once
     * at the peer, the frame "explores" in the "peer" network.  The optimization
     * is the removal of the "exploring" throughout the "peer" network connecting
     * rsrb remote peers.
     */
    /*
     * Base qualifier soley on flag, because input routines may modify
     * packet's broadcast status...
     */
    if (processRifLf)
    {
        ushort srb_targetring = srb_sb_get_targetring(swidb);

 	/*
 	 * Possibly cut down the maximum packet size.  Users can configure
 	 * this to help prevent delay timeouts on long lines.
 	 *
 	 * Set new largest frame to be minimum of encoding in incoming frame
 	 * and largest frame size supported by ring group.
 	 */
 	/*
 	 * find ring group to which destination ring is attached,
 	 * (not normally part of vrforward_pak, as routine knows
 	 * where packet is going and doesn't need to rely on groups
 	 */
 	vrg = vrfind_ringgroup(srb_targetring);
 	if (!vrg)
 	{
            srb_sb_incr_rx_drops(idb->firstsw);
 	    if (srb_debug || srbevent_debug)
 		buginf("\nRSRB%d: ring group %d not found",
 		       idb->unit, srb_targetring);
	    else
 		errmsg(&msgsym(NOTREM, RSRB));
 	    vr_nukepak(pak);
 	    return;
 	}    /* endIf for vrg check */
 	setRifLfTrh(trh, min(getRifLfTrh(trh), vrg->largest_frame));
    }	/* endIf for Lf processing */

    if ((vre->type == RSRB_ET_LOCAL) || (vre->type == RSRB_ET_LOCAL_VIRTUAL)) {
	idb = vre->outhole.idb;
	if (!srb_out_check((uchar *) pak->datagramstart, pak->datagramsize, idb,
			  ((ushort) trh->rc_blth & RC_LTHMASK))) {
	    vr_nukepak(pak);
	    return;
	}
	if (idb == hwidb_or_null(pak->if_input)) {
	    if (srb_debug || srbevent_debug) {
                srb_triplet_t srb_triplet;
                srb_sb_get_triplet(swidb, &srb_triplet);
		buginf("\nRSRB%d: local out input interface (srn %d bn %d trn %d), src: %e dst: %e [%s]",
		       idb->unit,
                       srb_triplet.localRing,
                       srb_triplet.bridgeNum,
                       srb_triplet.remoteRing,
		       trh->saddr, trh->daddr,
		       get_rif_str((uchar *) &trh->rc_blth, rif_string));
            }
            srb_sb_incr_tx_drops(pak->if_input);
	    vr_nukepak(pak);
	    return;
	}
	pak->if_output = idb->firstsw;
	pak->linktype = LINK_RSRB;
	/* count vring transmit and receive for LNM */
        srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, pak->datagramsize);
        srb_sb_incr_tx_nb_counters(idb->firstsw, pak->datagramsize);
	vre->forwards++;
	if (!dontsetbridge)
	    pak->flags |= PAK_SRB_OUT;
	if (srb_debug) {
	    if (idb->status & IDB_VIRTUAL) {
		buginf("\nRSRB%d: forward (srn %d bn %d trn %d), src: %e dst: %e [%s]",
		       idb->unit, srb_sb_get_thisring(swidb), bn, rn, 
                       trh->saddr, trh->daddr, 
                       get_rif_str((uchar *) &trh->rc_blth, rif_string));
	    } else {
		int a, b;
                srb_triplet_t srb_triplet;
                srb_sb_get_triplet(swidb, &srb_triplet);
                if ((trh->rc_dlf & RC_D) == 0) {
		    a = srb_triplet.localRing;
		    b = srb_triplet.remoteRing;
		} else {
		    a = srb_triplet.remoteRing;
		    b = srb_triplet.localRing;
		}
		buginf("\nRSRB%d: forward (srn %d bn %d trn %d), src: %e dst: %e [%s]",
		       idb->unit, a, bn, b, trh->saddr, trh->daddr,
		       get_rif_str((uchar *) &trh->rc_blth, rif_string));
	    }
	}

	if (processRifLf)
	{
	    /*
	     * Possibly cut down the maximum packet size.  Users can configure
	     * this to help prevent delay timeouts on long lines.
	     *
	     * Set new largest frame to be minimum of encoding in incoming frame
	     * and largest frame size supported by remote ring peer.
	     */
	    setRifLfTrh(trh, min(getRifLfTrh(trh), getHwLf(pak->if_output->hwptr)));
	}
	if (trh->rc_blth & RC_TRACE_FRAME)
	    reg_invoke_send_trace_report(pak, idb, NULL);
	tr_to_media_out(pak);
	return;
    } else if (vre->type & ( RSRB_ET_SDLLC | RSRB_ET_QLLC | RSRB_ET_TR_LANE ) ) {
	/*
	 * An SDLLC or QLLC frame. Give it to the SDLLC/QLLC code.
	 * Count vring receive for LNM
	 */
        srb_sb_incr_rx_counters(vre->virtual_idb->firstsw, pak->datagramsize);
	vr_vre_input(vre, pak, FALSE, type);
	return;
    }
 forwardit:
    vre->forwards++;
    if (vre->type & (RSRB_ET_REMOTE | RSRB_ET_REMSTATIC | 
			RSRB_ET_SDLLC | RSRB_ET_QLLC | RSRB_ET_TR_LANE |
		    RSRB_ET_REMSDLLC | RSRB_ET_REMQLLC)) {
	vrp = vre->outhole.vp;
	if(!vrp) {
            vr_nukepak(pak);
            return;
        }


	/* check to see whether we need to do any rsrb filtering */
	if (!rsrb_out_check((uchar *) pak->datagramstart, pak->datagramsize, idb,
		((ushort) trh->rc_blth & RC_LTHMASK), vrp)) {
		vr_nukepak(pak);
		return;
	}

 	/*
 	 * Apply ring _peer_based largest frame explorer reduction
 	 * Some code calls this function with explorer packets (bypassing original
 	 * vrforward_explorer) to "direct" an explorer towards a known peer.  Once
 	 * at the peer, the frame "explores" in the "peer" network.  The optimization
 	 * is the removal of the "exploring" throughout the "peer" network connecting
 	 * rsrb remote peers.
 	 */
 	/*
 	 * Base qualifier soley on flag, because input routines may modify
 	 * packet's broadcast status...
 	 */
 	if (processRifLf)
 	{
 	    /*
 	     * Possibly cut down the maximum packet size.  Users can configure
 	     * this to help prevent delay timeouts on long lines.
 	     *
 	     * Set new largest frame to be minimum of encoding in incoming frame
 	     * and largest frame size supported by remote ring peer.
 	     */
 	    setRifLfTrh(trh, min(getRifLfTrh(trh), vrp->largest_frame));
 	}
 	

        /*
         * check for local ack of llc2 virtual circuits
         */
	if (!type) {
            /*
             * Set the local variable "passthru" here to be used later on in this function
             * to check if the tcp driver queue has reached the high water mark.
             */
	    passthru = lack_passthru(vre, vrp, pak);
	    if (passthru == LAK2_TERMINATE) {
		if (lak2_local_term(pak, (uchar *)&llc2_type, vrp, srb_sb_get_thisring(swidb), rn))
		    return;
	    } else if (passthru == LAK2_XID) {
		/*
		 * This is vrforward_pak and the type was 0 which means that this XID
		 * came from a physical media.  Flag it as such.  Note that SEND_XID
		 * assumes that there isn't a CRC on the end of the packet so flag
		 * it as such by setting the kludge variable dontsetbridge.
		 */
		dontsetbridge = TRUE;
		llc2_type = RSRB_OP_SEND_XID;
	    }
	}
	
	if (!dontsetbridge)
	  pak->datagramsize += TR_FCS_SIZE; /* unhide it */
	lhdr.rhdr_version = vrp->vrp_version;
	lhdr.op = (!dontsetbridge) ? RSRB_OP_FORWARD : RSRB_OP_FORWARDCRC;
	if ((type) && (type != RSRB_OP_SDLLC && type != RSRB_OP_QLLC
                                             && type != RSRB_OP_TR_LANE  ))
	    lhdr.op = type;
	else if (llc2_type)
	    lhdr.op = llc2_type;
	if (vrp->type == RSRB_PT_FST) {
	    switch (lhdr.op) {
  	      case RSRB_OP_FORWARD:
		lhdr.op = RSRB_OP_FORWARDFST;
		break;
  	      case RSRB_OP_FORWARDCRC:
		lhdr.op = RSRB_OP_FORWARDFSTCRC;
		break;
	      default:
		break;
	    }
	}

	lhdr.trn = rn;
  
	/*
	 * Modification of original 'if' statment to add LINK_CLS
	 * condition.  CLS virtual rings are pointed 'towards' the
	 * application pseudo-ring.  See function cls_add_vring() for
	 * more initialization information.  The lhdr.vrn field below
	 * is filled with the ring-group ring number and used as a
	 * sanity check 'on the other side' of the RSRB ring.  So
	 * headers 'from' a CLS vring must have the srb_targetring in
	 * the header for proper operation.
	 */
        if ((idb->status & IDB_VIRTUAL) &&
	    (!reg_invoke_srt_other_media(idb)) && 
	    (!idb->firstsw->span_index) &&
            (pak->linktype != LINK_CLS))
        {
            lhdr.vrn = srb_sb_get_thisring(swidb);
        } else
        {
            lhdr.vrn = srb_sb_get_targetring(swidb);
        }

	lhdr.len = pak->datagramsize;
	if (trh->rc_blth & RC_TRACE_FRAME)
	    reg_invoke_send_trace_report(pak, idb, NULL);

	if (!((*vrp->sender)(vrp, &lhdr, pak))) {
	    if (srb_debug || srbevent_debug)
		buginf("\nRSRB%d: send to peer %s failed",
		       idb->unit, vrpeer_id(vrp, peerid));
	    vrp->drops++;
	    return;
	} 

	else if (srb_debug) {
	    if (idb->status & IDB_VIRTUAL) {
		buginf("\nRSRB%d: fwd vrn %d bn %d trn %d to peer %s, o=%d v=%d t=%d len=%d",
		       idb->unit, srb_sb_get_thisring(swidb), bn, rn,
		       vrpeer_id(vrp, peerid), lhdr.op, lhdr.vrn, lhdr.trn,
		       lhdr.len);
	    } else {
		int a, b;
                srb_triplet_t srb_triplet;
                srb_sb_get_triplet(swidb, &srb_triplet);
		if ((trh->rc_dlf & RC_D) == 0) {
		    a = srb_triplet.localRing;
		    b = srb_triplet.remoteRing;
		} else {
		    a = srb_triplet.remoteRing;
		    b = srb_triplet.localRing;
		}
		buginf("\nRSRB%d: forward %s srn %d bn %d vrn %d to peer %s",
		       idb->unit, vr_op2str(lhdr.op),
		       a, bn, b, vrpeer_id(vrp, peerid));
	    }
	}
	return;
    }
    errmsg(&msgsym(BADVRE, RSRB));
    vr_nukepak(pak);
    return;
}

/*********************************************************************
 *
 * Misc routines.
 * 
 *********************************************************************/

/*
 * vring_background
 * Background processing for RSRB background functions.  This
 * module is called from srb_background to process vring type
 * things.  These include:
 * 
 * 	 0a) send periodic keepalives to check for link failure.
 * 	    This is very important to do especially in a local
 * 	    ack environment.
 * 	 0b) Check for keepalive failure and if so tear the peer
 * 	    down and any associated local ack sessions.
 * 	 1) Process delayed ring exchange requests.
 * 	 2) Draining and processing of the exchange queue.  This
 * 	    queue holds packets that will possibly change a remote
 * 	    peer's ring/bridge configuration.  These paks are ring
 * 	    exchange requests or replies from the other side.
 * 	 3) Draining and processing of the connection queue.  These
 * 	    are packets that need to be handed to the Connector after
 * 	    it is started up.  This start up can't happen at interrupt
 * 	    level.
 *
 * There is a subtlety concerning start up of the connector.  It is
 * possible to change the peer state towards connection (ie. REMOPENED)
 * before or after the connector is actually fired up.  We need to
 * fire the connector up if the packets on the vrconn_pakQ have not
 * been processed yet.  It is also possible to process multiple packets
 * destined for the remote side prior to the connector actually being
 * fired up.
 */

void vring_background (void)
{
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    tr_conntype *vrc;
    tr_vhdrtype *vrh;
    tr_starttype *start;
    paktype *pak;
    char peerid[MAX_PEER_NAME];
    int wakeup, i;
    leveltype level;

    if (process_get_boolean(vr_wake_up)) {
	/*
	 * handle special functions.
	 *
	 * 0a) send periodic VERSIONREQs to act as keep alives.
	 *     We will only send a new keep alive if we haven't
	 *     heard anything from our peer in the last
	 *     rsrb_keepalive_interval.
	 * 0b) check for keepalive failure.
	 * 1) scan the virtual rings for any pending delayed exchanges.
	 * 2) scan all virtual rings/all peers for RemOpened and no
	 *    connector running.  If found fire up a connector and
	 *    fake a connector message for the peer.
	 */
	process_set_boolean(vr_wake_up, FALSE);
	wakeup = FALSE;
	if (rsrb_keepalive_interval && AWAKE(rsrb_next_keepalive)) {
	    TIMER_START(rsrb_next_keepalive, ONESEC);
	    for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
		for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
		    if (vrp->state == RSRB_PS_CONNECTED) {
                        if (!CLOCK_IN_INTERVAL(vrp->last_keepalive,
                                               rsrb_keepalive_interval)) {
                             if (!CLOCK_IN_INTERVAL(vrp->last_keepalive, 
						    3*rsrb_keepalive_interval)) {
                                  /*
                                   * Local ACK currently is only allowed on TCP
				   * connections (because they are reliable).
				   * Abort will cause the connection
                                   * to eventually close causing a state change
				   * causing a call to lack_toss_connections.
				   * But is a fairly long causal thread
                                   * that is potentially easy to break.
				   * So we toss them here explicitly.
                                   */
				 /* Removed erroneous code added in a failed 
				    attempt to fix CSCdi92503.  Actual fix is
				    made below - changed %u format to %TA (Cisco-
				    specific format for timestamps). */

                                  if (srb_debug || srbevent_debug || srberr_debug)
                                       buginf("\nRSRB: %TA keepalive failed: tearing down peer %s",
                                              vrp->last_keepalive, vrpeer_id(vrp, peerid));
                                  /*
                                   * Close the peer and toss all associated 
				   * connections (SDLC/LLC)
                                   */
                                  TIMER_START(vrp->last_keepalive, rsrb_keepalive_interval);
                                  rsrb_newstate(vrp, RSRB_PS_CLOSED);
                                  TIMER_STOP(vrp->keep_timer);
                                  (*vrp->abort)(vrp);
                                  if (vrp->priority) {
                                     for (i = 1; i <= RSRB_PRIORITY_COUNT; i++) {
					 if (srb_debug || srbevent_debug || srberr_debug)
					     buginf("\nRSRB: keepalive failed: tearing down priority peers");
					 rsrb_newstate(vrp->priority_peers[i],
                                                      RSRB_PS_CLOSED);
					 (*vrp->priority_peers[i]->abort)(vrp->priority_peers[i]);
                                     }
                                  }
                             } else {
                                  if (!TIMER_RUNNING(vrp->keep_timer)) {
                                       vrsend_version(vrp, RSRB_OP_VERSIONREQ,
                                                      FALSE);
                                       TIMER_START(vrp->keep_timer,
                                                   rsrb_keepalive_interval);
                                  } else if (AWAKE(vrp->keep_timer)) {
                                       vrsend_version(vrp, RSRB_OP_VERSIONREQ,
                                                      FALSE);
                                       TIMER_START(vrp->keep_timer,
                                                   rsrb_keepalive_interval);
                                  }
                             }
                        } else {
                             if (TIMER_RUNNING(vrp->keep_timer))
                                  TIMER_STOP(vrp->keep_timer);
                        }
		    }
		}
	    }
	}
	if (!vrconn_pid) {
	    for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
		for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
		    if (vrp->state == RSRB_PS_REMOPENED) {
			wakeup = TRUE;
			vrp->connector_owns = TRUE;
			vrc = malloc_named(sizeof(tr_conntype), rsrb_cb);
			if (!vrc) {
			    continue;
			}
			vrc->vrn = vrp->vrg->vrn;
			vrc->vrp = vrp;
			p_enqueue(&vrconnQ, vrc);
			if (srb_debug || srbevent_debug)
			    buginf("\nRSRB: BGND: REMOPENED and no connector %s",
				   vrpeer_id(vrp, peerid));
		    }
		    if (vrp->priority) {
			for (i = 1; i <= RSRB_PRIORITY_COUNT; i++) {
			    if ((vrp->priority_peers[i]) &&
				( (vrp->priority_peers[i]->state ==
				    RSRB_PS_REMOPENED) ||
                                   (vrp->priority_peers[i]->state ==
                                    RSRB_PS_OPENING) 
                                   )
                                ) {
				wakeup = TRUE;
				vrp->priority_peers[i]->connector_owns =
				    TRUE;
				vrc = malloc_named(sizeof(tr_conntype), rsrb_cb);
				if (!vrc) {
				    continue;
				}
				vrc->vrn =
				    vrp->priority_peers[i]->vrg->vrn;
				vrc->vrp = vrp->priority_peers[i];
				p_enqueue(&vrconnQ, vrc);
				if (srb_debug || srbevent_debug || srberr_debug)
				    buginf("\nRSRB: BGND: REMOPENED and no connector %s",
					   vrpeer_id(vrp->priority_peers[i], peerid));
			    }
			}
		    }
		}
	    }
	    if (wakeup)
		vrconn_pid = cfork(vrconnector, 0L, 0, "RSRB Connector", 0);
	}
	for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
            tr_ventrytype *dead_ring;
 
            /* free the dead rings for this ring group */
	    level = raise_interrupt_level(ALL_DISABLE);
            while ((dead_ring = dequeue(&vrg->dead_rings)) != NULL) {
                   if (srb_debug)
                       buginf("\nRSRB: remove ring %d from list", dead_ring->rn);
                   free(dead_ring);
            }
	    reset_interrupt_level(level);

	    if (!(vrg->flags & VRGF_FORCEXCHG))
		continue;
	    vrg->flags &= ~VRGF_FORCEXCHG;
	    for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp;
		 vrp = vrp->next) {
		pak = ring_xchg_pak(vrp->vrg, RSRB_OP_RINGXREQ, vrp->vrp_version);
		if (pak && (*vrp->sender)(vrp, NULL, pak)) {
		    if (srb_debug)
			buginf("\nRSRB: BGND: %s sent to peer %s",
			       vr_op2str(RSRB_OP_RINGXREQ),
			       vrpeer_id(vrp, peerid));
		} else {
		    if (srb_debug || srbevent_debug)
			buginf("\nRSRB: BGND: %s for %s failed%s",
			       vr_op2str(RSRB_OP_RINGXREQ),
			       vrpeer_id(vrp, peerid),
			       (pak ? "" : " (no pak)"));
                    /*
                     * If I got here because vrp->sender() returned FALSE,
                     * this packet is already freed thru vr_nukepak().
                     * Therefore trying to free it again will trigger
                     * INLIST error message.
                     * CSCdi12179
                     */
                    if (!pak)
                        if (srb_debug || srbevent_debug)
                            buginf("\nRSRB: BGND: no memory for ring_xchg_pak");
		    continue;
		}
	    }
	}
    }

    /*
     * Now empty the xchg Q processing the packets.  The xchg Q holds
     * packets that have ring exchange messages.  We must process
     * at process level because it will effect the memory structures
     * used by the peer structures.
     */

    while ((pak = process_dequeue(vrxchgQ))) {
	vrp = pak->peer_ptr;
	vrprocess_ring_xchg(vrp, pak);
	retbuffer(pak);
    }

    /*
     * Now empty the connection packet queue.  This queue will have
     * packets that were queued up for any peer while it was not
     * in the connected state.  If closed we will build a connection
     * entry and fire up the connector.  We change state as appropriate.
     */
    while (!process_is_queue_empty(vrconn_pakQ)) {
	process_suspend();	/* give queues a chance to drain */
	pak = process_dequeue(vrconn_pakQ);
	/* This is paranoid, this proc should be the only drainer */
	if (!pak)
	    break;
	vrp = pak->peer_ptr;
	for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next)
	    if (rsrb_checkqueue(&vrg->peers, vrp))
		break;
	if (!vrg || vrp->vrg != vrg) {
	    retbuffer(pak);
	    continue;
	}
	vrh = (tr_vhdrtype *)(pak->datagramstart);
	PUTSHORT(&vrh->rhdr_version, vrp->vrp_version);
	switch (vrp->state) {
	case RSRB_PS_OPENING:
	case RSRB_PS_REMOPENED:
	    if (!vrp->connector_owns) {
		vrc = malloc_named(sizeof(tr_conntype), rsrb_cb);
		if (!vrc) {
		    retbuffer(pak);
		    continue;
		}
		vrp->connector_owns = TRUE;
		vrc->vrn = vrp->vrg->vrn;
		vrc->vrp = vrp;
		p_enqueue(&vrconnQ, vrc);
		if (vrconn_pid == 0)
		    vrconn_pid = cfork(vrconnector, 0L, 0, "RSRB Connector",
				       0);
	    }
	    /*
	     * fall through and deal with the packetoid
	     */
	case RSRB_PS_OPENWAIT:
	case RSRB_PS_REMWAIT:
	case RSRB_PS_REMGO:
	case RSRB_PS_DRAINING:
	    start = malloc_named(sizeof(tr_starttype), rsrb_cb);
	    if (!start) {
		retbuffer(pak);
		continue;
	    }
	    start->pak = pak;

	    if (vrp->startq.count >= vrp->startq.maximum) {
	        retbuffer(pak);
	        free(start);
	    }
	    else {
		p_enqueue(&vrp->startq, start);
            }
	    continue;

	case RSRB_PS_CONNECTED:
	    (void) (*vrp->sender)(vrp, NULL, pak);
	    continue;

	case RSRB_PS_CLOSED:
	case RSRB_PS_DEAD:
	    retbuffer(pak);
	    continue;
	    
	default:
	    errmsg(&msgsym(ILLPEER, RSRB), vrpeer_id(vrp,peerid), vrp,
		   vrp->state);
	    retbuffer(pak);
	    continue;
	}
    }
}

/*
 * rsrb_periodic
 * Do periodic things that need to be done.  Currently the only
 * two things we do are:
 *   1) scan the remote peers for dead ones and reactivate
 *      them.
 *   2) take all open direct interface peers, and send a version request.
 *      If no version reply arrives within this period, we make the interface
 *      closed.
 */

void rsrb_periodic_per_peer (
    tr_vpeertype *vrp)
{
    if (vrp->state == RSRB_PS_DEAD)
        rsrb_newstate(vrp, RSRB_PS_CLOSED);
}

void rsrb_periodic (void)
{
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    int i;

    for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
	for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
	    rsrb_periodic_per_peer(vrp);
	    if (vrp->priority) {
		for (i = 1; i <= RSRB_PRIORITY_COUNT; i++)
		    rsrb_periodic_per_peer(vrp->priority_peers[i]);
	    }
	}
    }
}

void set_rsrb_tcpqlen (ulong len)
{
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    int i;

    vring_tcpqmax = len;
    for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
	for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
	    if (vrp->type == RSRB_PT_TCPD) {
	        reg_invoke_ip_tcpdriver_set_oqmax(vrp->t, vring_tcpqmax);
	    }
	    if (vrp->priority) {
		for (i = 1; i <= RSRB_PRIORITY_COUNT; i++) {
		    if (vrp->priority_peers[i] &&
			(vrp->priority_peers[i]->type == RSRB_PT_TCPD)) {
			reg_invoke_ip_tcpdriver_set_oqmax(vrp->priority_peers
							  [i]->t,
							  vring_tcpqmax);
		    }
		}
	    }
	}
    }
}

void set_rsrb_lack_connect_timeout (ulong len)
{
    lack_connect_timeout = len;
}

/*
 * Clear any rsrb counters shown in "show source-bridge"
 */
void rsrb_clear_counters (void)
{
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp, *pvrp;
    tr_ventrytype *vre;
    int i;
    hwidbtype *idb;
    list_element *l_elt;

    for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
	for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
	    vrp->pkts_rx = vrp->pkts_tx = vrp->explorers = vrp->drops =
                vrp->bytes_rx = vrp->bytes_tx = 0;
	    if (vrp->priority)
		for (i = 1; i <= RSRB_PRIORITY_COUNT; i++) {
		    if (vrp->priority_peers[i]) {
			pvrp = vrp->priority_peers[i];
			pvrp->pkts_rx = pvrp->pkts_tx = pvrp->explorers =
			    pvrp->drops = pvrp->bytes_rx = pvrp->bytes_tx = 0;
		    }
		}
	}
	for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
	    vre->forwards = 0;
	}
    }    
    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_TOKEN, l_elt, idb) {
        srb_sb_t *srb_sb;
        srb_sb_t *vsrb_sb;
        
        srb_sb = srb_get_sb_inline(idb->firstsw);
        if (!srb_sb) {
            continue;
        }

        if (srb_sb->srb_bridge_num > SRB_MAX_BRIDGE_NUM) {
            continue;
        }

        vsrb_sb = srb_get_sb_inline(firstsw_or_null(idb->vidb_link));
        if (!vsrb_sb) {
            continue;
        }

        vsrb_sb->srb_rx_count    =
        vsrb_sb->srb_rx_bytes    =
        vsrb_sb->srb_tx_nb_count =
        vsrb_sb->srb_tx_b_count  =
        vsrb_sb->srb_tx_nb_bytes =
        vsrb_sb->srb_tx_b_bytes  =
        vsrb_sb->srb_tx_drops    =
        vsrb_sb->srb_rx_drops    =
        vsrb_sb->srb_span_in     =
        vsrb_sb->srb_allrte_in   =
        vsrb_sb->srb_span_out    =
        vsrb_sb->srb_allrte_out  = 0;

        vsrb_sb->si_inputs_fast_srb
	    = idb->vidb_link->counters.inputs_fast[ACCT_PROTO_SRB];
        vsrb_sb->si_rx_cumbytes_fast_srb
	    = idb->vidb_link->counters.rx_cumbytes_fast[ACCT_PROTO_SRB];
        vsrb_sb->si_outputs_fast_srb
	    = idb->vidb_link->counters.outputs_fast[ACCT_PROTO_SRB];
        vsrb_sb->si_tx_cumbytes_fast_srb
	    = idb->vidb_link->counters.tx_cumbytes_fast[ACCT_PROTO_SRB];
        vsrb_sb->si_inputs_auto_srb
	    = idb->vidb_link->counters.inputs_auto[ACCT_PROTO_SRB];
        vsrb_sb->si_rx_cumbytes_auto_srb
	    = idb->vidb_link->counters.rx_cumbytes_auto[ACCT_PROTO_SRB];
        vsrb_sb->si_outputs_auto_srb
	    = idb->vidb_link->counters.outputs_auto[ACCT_PROTO_SRB];
        vsrb_sb->si_tx_cumbytes_auto_srb
	    = idb->vidb_link->counters.tx_cumbytes_auto[ACCT_PROTO_SRB];
    }
}

void rsrb_print_peer_info (
    tr_vpeertype *vrp,
    ipaddrtype peername,
    int i)
{
    char sbuf1[10], sbuf2[90];

    switch (vrp->type) {
      case RSRB_PT_NONE:
      case RSRB_PT_TCPD:
	if (!(vrp->priority_top)) {
	    printf("\n   TCP %17i", vrp->ipaddr);
	    if (vrp->priority) {
		i = 0;
	    }
	}
	if ((vrp->priority_top) || (vrp->priority)) {
	    printf("\n   %s priority peer", priority_peername(i));
	}
	sprintf(sbuf2, "%-3u",
		((vrp->t) ? (reg_invoke_ip_tcpdriver_oqlen(vrp->t)) : 0));
	break;
      case RSRB_PT_FST:
	printf("\n   FST %17i", vrp->ipaddr);
	sprintf(sbuf2, "n/a\n\tPSI: %u\tPSO: %u\tISI: %u\tISO: %u",
		vrp->ip_seqnumin, vrp->ip_seqnumout,
		vrp->ip_int_seqnumin, vrp->ip_int_seqnumout);
	break;

    case RSRB_PT_IF:
	printf("\n   IF  %17s", vrp->outidb->hw_namestring);
	sprintf(sbuf2, "n/a");
	break;

    case RSRB_PT_FR:
	printf("\n   FR  %17s %d", vrp->outswidb->namestring, vrp->dlci);
	sprintf(sbuf2, "n/a");
	break;

    case RSRB_PT_LAN:
	printf("\n   IF  %c%d %e", vrp->outidb->name[0],
	       vrp->outidb->unit, vrp->macaddr);
	sprintf(sbuf2, "n/a");
	break;
    }
    if (vrp->backup_group == RSRB_BACKUP_GROUP_NONE) {
	sprintf(sbuf1, "   ");
    } else {
	sprintf(sbuf1, "%-2u%1c", vrp->backup_group,
		hw_interface_up(vrp->outidb) ? 'u' : 'd');
    }
    printf(" %8s %3s %1s%1d %-8u %-8u %-8u %-7u %s",
	   ((vrp->type != RSRB_PT_TCPD) || (vrp->ipaddr != peername))
	   ? vrpeer_state(vrp->state) : "-",
	   sbuf1, (vrp->local_ack ? "*" : " "), vrp->vrp_version,
	   vrp->pkts_rx, vrp->pkts_tx, vrp->explorers, vrp->drops,
	   sbuf2);
}


void show_rsrb (void)
{
    boolean first_peer;
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    tr_ventrytype *vre;
    ipaddrtype peername;
    int i;

    if (!vringQ.qhead) {
	printf("\n\nRemote Source Route Bridging not enabled.");
	return;
    }
    printf("\n\nGlobal RSRB Parameters:");
    printf("\n TCP Queue Length maximum: %u", vring_tcpqmax);
    if (rsrb_fst_name) {
	printf("\t\tRSRB FST Peername: %i", rsrb_fst_name);
    }
    for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
	mem_lock(vrg);
	printf("\n\nRing Group %d:", vrg->vrn);
	peername = vrtcp_peername(vrg);
	if (peername)
	    printf("\n  This TCP peer: %i", peername);
	else
	    printf("\n  No TCP peername set, TCP transport disabled");
	printf("\n   Maximum output TCP queue length, per peer: %d",
	       vring_tcpqmax);
	if (vrg->largest_frame != RC_LFINITIAL)
	    printf("\n   Largest packet size allowed is %d bytes",
		   rc_code_to_bytes(vrg->largest_frame));

	first_peer = TRUE;
	for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
	    mem_lock(vrp);
	    if (first_peer) {
		first_peer = FALSE;
		printf("\n  Peers:                 state     bg lv  pkts_rx  pkts_tx  expl_gn   drops TCP");
	    }
 	    rsrb_print_peer_info(vrp, peername, 0);
 	    if (vrp->priority) {
 		for (i = 1; i <= RSRB_PRIORITY_COUNT; i++) {
 		    if (vrp->priority_peers[i])
			rsrb_print_peer_info(vrp->priority_peers[i], peername,
					     i);
 		}
	    }
	    free(vrp);
	}
	printf("\n  Rings:");
	for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
	    mem_lock(vre);
	    printf("\n   bn: %2d rn: %4d %6s", vre->bn, vre->rn,
		   (vre->type & RSRB_ET_LOCAL) ? "local " :
		   (vre->type & RSRB_ET_REMOTE) ? "remote" :
		   (vre->type & RSRB_ET_REMSDLLC) ? "remvrt" :
		   (vre->type & RSRB_ET_REMQLLC) ? "r qllc" :
		   (vre->type & RSRB_ET_REMSTATIC) ? "static" :
		   (vre->type & RSRB_ET_SDLLC) ? "sdllc" :
		   (vre->type & RSRB_ET_LOCAL_VIRTUAL) ? "locvrt" :
		   (vre->type & RSRB_ET_QLLC) ? "qllc" :
		   (vre->type & RSRB_ET_TR_LANE) ? "tr lane" :
			 "other ");

	    if (!(vre->type & (RSRB_ET_SDLLC | RSRB_ET_QLLC
                                             | RSRB_ET_TR_LANE) )) {
	      printf(" ma: %e", vre->macaddr);
	    } else {
	      printf("                   ");
	    }
	    if (vre->type & (RSRB_ET_LOCAL |
			     RSRB_ET_SDLLC |
			     RSRB_ET_QLLC  |
			     RSRB_ET_TR_LANE |
			     RSRB_ET_LOCAL_VIRTUAL)) {

		if(!vre->outhole.idb) {
                    free(vre);
		    continue;
		}
	        if ((vre->outhole.idb->status & IDB_VIRTUAL) &&
		    vre->outhole.idb->firstsw && 
		    vre->outhole.idb->firstsw->span_index)
		  printf(" Bridge-group %d      ",
			 vre->outhole.idb->firstsw->span_index);
		else
		  printf(" %20s", vre->outhole.idb->hw_namestring);
		printf("  fwd: %d", vre->forwards);
	    } else if (vre->type & (RSRB_ET_REMOTE | RSRB_ET_REMSTATIC |
				    RSRB_ET_REMSDLLC | RSRB_ET_REMQLLC)) {
		vrp = vre->outhole.vp;
	
		if(!vrp) {
		    free(vre);
		    continue;
		}

		switch (vrp->type) {
		  case RSRB_PT_NONE:
		  case RSRB_PT_TCPD:
		    printf(" TCP %17i", vrp->ipaddr);
		    break;
		  case RSRB_PT_FST:
		    printf(" FST %17i", vrp->ipaddr);
		    break;
		  case RSRB_PT_IF:
		    printf(" IF  %17s", vrp->outidb->hw_namestring);
		    break;
		  case RSRB_PT_FR:
		    printf(" FR  %17s %d", vrp->outswidb->namestring, vrp->dlci);
		    break;
		  case RSRB_PT_LAN:
		    printf(" IF  %c%d %e", vrp->outidb->name[0],
		       vrp->outidb->unit, vrp->macaddr);
		    break;
		}
		printf(" fwd: %d", vre->forwards);
	    } else
		printf("other???    fwd: %d", vre->forwards);

	    /* 
	     * relase vre after mem_lock
	     */
	    free(vre);

	} /* for vre */
	
	/* 
	 * release vrg after mem_lock
	 */
	free(vrg);
    
    } /* for vrg */
}

/*
 * Get a rough sense of Remote Source Route Bridging's activity.
 */

boolean rsrb_active (ulong sport, ulong dport, ipaddrtype fhost)
{
    return(!QUEUEEMPTY(&vringQ));
}

/*
 * Do things for having a new burned in address on a token ring
 * card.
 */

void rsrb_newbia (hwidbtype *idb)
{
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp, *vrp_next;
    tr_ventrytype *vre;

   
    if (!idb)
        return;
 
    vrg = vrfind_ringgroup(srb_sb_get_targetring(idb->firstsw));
    if (!vrg)
	return;

    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next)
	if (((vre->type == RSRB_ET_LOCAL) ||
	     (vre->type == RSRB_ET_LOCAL_VIRTUAL)) &&
	    vre->outhole.idb == idb) {
	    ieee_copy(idb->bia, vre->macaddr);
	    vre->macaddr[0] |= TR_LOCAL;
	    if (vre->virtual_idb)
		ieee_copy(vre->macaddr, vre->virtual_idb->hardware); 
	}

    /* Use "next" to avoid any problems if the list changes beneath us */
    vrp = vrp_next = (tr_vpeertype *) vrg->peers.qhead;
    while (vrp) {
	vrp_next = vrp_next->next;
	if (vrp->state == RSRB_PS_CONNECTED)
	    (void) vrsend_xchg(vrp, RSRB_OP_RINGXREPLY, "BIA", FALSE);
	vrp = vrp_next;
    } 
}

void set_rsrb_largest (int vrn, int size)
{
    tr_vgrouptype *vrg;
    
    if ((vrg = vrfind_ringgroup(vrn)) == NULL) {
	printf("%%No such ring group - vrn.\n");
	return;
    }

    vrg->largest_frame = bytes_to_rc_code(size);

    if (vrg->largest_frame == -1) {
    /* This statement is executed if the source route configuration
       command is negated.                                          */
	vrg->largest_frame = RC_LFINITIAL;
    }
}

 
/*
 * Support for bridging between ring-groups and bridge groups
 *
 * As an alternate to running pure transparent bridging on token ring, we
 * also support a form of bridging similar to what is implemented by the
 * IBM 8209 bridge. This allows packets with a specific RIF to be forwarded
 * into a transparent bridge group, and allows packets from the bridge group
 * to be forwarded (with a RIF added) to the token ring bridge group.
 *
 * This is implemented via a "virtual" token ring interface which is set
 * up as both a member of the ring-group and the bridge-group. This
 * virtual interface checks the linktype field from packets sent to it
 * (*oqueue) hook, and moves the packets between the two bridging domains.
 *
 * rsrb_vbridge_oqueue - Queue an output packet to the virtual bridge
 *
 * This is the (*oqueue) hook for the virtual bridge IDB. If the
 * packet is being sent to LINK_RSRB, then this is a packet from the
 * virtual ring which needs to be sent to the transparent bridging
 * group. If this is a LINK_BRIDGE packet, then this is a packet from
 * transparent bridging that needs to be sent to the bridge group.
 * do_srtlb_translation 
 *
 * This routine also performs any 80d5 translation. It is easier to do here
 * than in bridge.c because it only applies to 8209 functionality and because
 * this routine will always get frames in a "token ring" format.
 *
 * This packet, if it was in "80d5" format on an Ethernet or other media, will
 * look like the following to this process when going to SRB:
 *
 * --------------------
 * | AC       FC      |       (pak->datagramstart points to AC)
 * --------------------
 * | DMAC 0   DMAC 1  |
 * --------------------
 * | DMAC 2   DMAC 3  |
 * --------------------
 * | DMAC 4   DMAC 5  |
 * --------------------
 * | SMAC 0   SMAC 1  |
 * --------------------
 * | SMAC 2   SMAC 3  |
 * --------------------
 * | SMAC 4   SMAC 5  |
 * --------------------
 * | SNAPSNAP (0xAAAA)|
 * --------------------
 * | ETHEROUIa 0x0300 |
 * --------------------
 * | ETHEROUIb 0x0000 | idb->tr_transit_oui contain what this is supposed to be
 * --------------------
 * | TYPE = 0x80D5    |
 * --------------------
 * | LENGTH of LLC2   |
 * --------------------
 * | PAD BYTE | DSAP  |
 * ------------       |
 * | SSAP    Control  |
 * --------------------
 * | LLC2 Data        |
 * --------------------
 *
 * To perform 80d5 translation, all we need do is the following:
 *
 * From Transparent Bridging to Source Route Bridging world:
 *    BEFORE a RIF is added:
 *       1) Make new pak->datagramsize from pak->datagramstart[22-23] +
 *          TRING_ENCAPBYTES
 *       2) Copy (in a backwards loop, sigh...)
 *          pak->datagramstart[13] ... pak->datagramstart[0] to
 *          pak->datagramstart[24] ... pak->datagramstart[11]
 *       3) pak->datagramstart += 11;
 *
 * The packet, on the Token Ring, will look like the following at this point,
 * before going to transparent bridging:
 *
 * --------------------
 * | AC       FC      |       (pak->datagramstart points to AC)
 * --------------------
 * | DMAC 0   DMAC 1  |
 * --------------------
 * | DMAC 2   DMAC 3  |
 * --------------------
 * | DMAC 4   DMAC 5  |
 * --------------------
 * | SMAC 0   SMAC 1  |
 * --------------------
 * | SMAC 2   SMAC 3  |
 * --------------------
 * | SMAC 4   SMAC 5  |
 * --------------------
 * | DSAP     SSAP    |
 * |      etc.        |
 * --------------------
 *
 * To Transparent Bridging world, make it appear as a Transit Frame:
 *    AFTER RIF is removed:
 *       1) Set pak->datagramstart -= 11;
 *       2) Copy pak->datagramstart[11]... pak->datagramstart[24] to
 *          pak->datagramstart[0]... pak->datagramstart[13] (bcopy
 *          can do this..)
 *       3) Set pak->datagramstart[14-15] to 0xAAAA
 *       4) Set pak->datagramstart[16-19] to 0x030000[f8 | 00 (if oldoui)]
 *       5) Set pak->datagramstart[20-21] = 0x80d5
 *       6) Set pak->datagramstart[22-23] = pak->datagramsize - header_size
 *       7) Set pak->datagramstart[24] (pad) = pak->datagramstart[26]
 *          (The stupid 8209 does this, and so must we)
 *       8) Set pak->datagramsize += 11;
 * That's it...
 */


/*
 * i s _ 8 0 d 5 _ e n a b l e d 
 *
 * Check to see if 80d5 translation needs to be performed on this
 * packet.
 */
boolean is_80d5_enabled(paktype *pak, hwidbtype *hwidb)
{
    uchar *cp;
    if (!enable_80d5)
	return(FALSE);
    
    cp = &(pak->datagramstart[TRING_ENCAPBYTES]);
    switch (pak->linktype)
    {
	/*
	 * pak came from RSRB -or- CLS, send to xparent bridge
	 */

    case LINK_RSRB:
    case LINK_CLS:
	if (!sap_8209_table[*cp])
	      return(FALSE);
	break;
	
	/*
	 * pak came from xparent bridge, send to RSRB -or- CLS
	 */
    case LINK_BRIDGE:
	if ((GETSHORT(cp) != SNAPSNAP) || 
	    (GETLONG((&(cp[2]))) != hwidb->tr_transit_oui) ||
	    (GETSHORT((&(cp[6]))) != ETHER_80D5_TYPE) ||
	    (!sap_8209_table[cp[11]])) {
	      return(FALSE);
	  }
	break;
    } /* endSwitch (pak->linktype) */
    return(TRUE);
}

/*
 * x f o r m _ 8 0 d 5 _ t o _ 8 0 2
 *
 * Old code taken from 'LINK_BRIDGE' processing in legacy
 * rsrb_vbridge_oqueue()
 */
void xform_80d5_to_802(paktype *pak)
{
    int i;
    uchar *cp = &(pak->datagramstart[TRING_ENCAPBYTES]);;

    /* 
     * Get the true size by adding the LLC2 PDU length of the packet
     * to the Token-Ring header.  See packet diagram above for
     * location of LLC length field.
     */
    pak->datagramsize = GETSHORT((&(cp[8]))) + TRING_ENCAPBYTES;
    for (i = TRING_ENCAPBYTES - 1; i >= 0; --i)
	pak->datagramstart[11 + i] = pak->datagramstart[i];
    pak->datagramstart += 11;
}


/*
 * x f o r m _ 8 0 2 _ t o _ 8 0 d 5
 *
 * Old code taken from 'LINK_RSRB' processing in legacy
 * rsrb_vbridge_oqueue() */
void xform_802_to_80d5(paktype *pak, ulong transit_oui)
{
    int i;

    pak->datagramstart -= 11;
    for (i = 0; i < TRING_ENCAPBYTES;i++)
	pak->datagramstart[i] =
	    pak->datagramstart[11 + i];
    PUTSHORT((&(pak->datagramstart[14])), SNAPSNAP);
    PUTLONG((&(pak->datagramstart[16])), transit_oui);
    PUTSHORT((&(pak->datagramstart[20])), ETHER_80D5_TYPE);
    PUTSHORT((&(pak->datagramstart[22])), (pak->datagramsize -
					   TRING_ENCAPBYTES));
    pak->datagramstart[24] = pak->datagramstart[26];
    pak->datagramsize += 11;
}


/* fs_srtlb_explorers 
 *
 * Fastswitch support for broadcasts between ring-groups and bridge groups.
 *
 * This is implemented via a "virtual" token ring interface which is set up
 * as both a member of the ring-group and the bridge-group. 
 *
 * This function translates a packet from explorers to broadcasts and 
 * call the common transparent bridge forward process.
 */

boolean fs_srtlb_explorers (paktype *pak, fs_hdrtype *hdr, hwidbtype *vidb)
{
    idbtype *swidb = vidb->firstsw;
    paktype *opak;
    trrif_t *tring; 
    int riflen;
    srbroute_t *route;

    /* Learn stations before bridging.  Use the rif length from the 
     * header control block since it has the current hop info. */

    route = (srbroute_t *)&hdr->newtrh->rc_blth;
    rif_update(vidb, 0, hdr->newtrh->saddr, NULL, route, RIF_AGED);

    /* Make copy of the original packet and remove the rif. */

    opak = getbuffer(pak->datagramsize);
    if (!opak)
        return(FALSE);

    pak_copy(pak, opak, pak->datagramsize);

    riflen = ((trrif_t *)pak->datagramstart)->rc_blth & RC_LTHMASK;
    opak->datagramstart += riflen;
    opak->datagramsize  -= riflen;
    bcopy(pak->datagramstart, opak->datagramstart, TRING_ENCAPBYTES);

    tring = (trrif_t *)opak->datagramstart;
    tring->saddr[0] &= ~TR_RII;

    /* All addresses are canonical in transparent domain. */

    ieee_swap(tring->daddr, tring->daddr);
    ieee_swap(tring->saddr, tring->saddr);

    opak->if_input  = swidb;
    opak->if_output = NULL;
    opak->mac_start  = opak->datagramstart;
    opak->addr_start = opak->datagramstart + TR_ACFC_SIZE;
    opak->info_start = opak->datagramstart + TRING_ENCAPBYTES;

    if(srb_debug)
	buginf("\n%%SRTLB%d explorer SRB to TLB, s:%e d:%e",
                             vidb->unit,tring->saddr,tring->daddr);

    reg_invoke_vbridge_forward(opak);
    return(TRUE);
}

/* fs_srtlb_packets 
 *
 * Fastswitch support for bridging between ring-groups and bridge groups.
 *
 * As an alternate to running pure transparent bridging on token ring, we
 * also support a form of bridging similar to what is implemented by the
 * IBM 8209 bridge. This allows packets with a specific RIF to be forwarded
 * into a transparent bridge group, and allows packets from the bridge 
 * group to be forwarded (with a RIF added) to the token ring bridge group.
 *
 * This is implemented via a "virtual" token ring interface which is set
 * up as both a member of the ring-group and the bridge-group. This
 * function checks the packet linktype and translates packets for bridging 
 * between the two domains.
 */


boolean fs_srtlb_packets (paktype *pak, hwidbtype *vidb)
{
    uchar explorer[] = {RC_DEF_SPAN, RC_LF1500};
    uchar tringmac[TRING_ENCAPBYTES+SRB_MAXRIF];
    trrif_t *tring;
    riftype *entry = NULL;
    uchar *rifptr;
    int riflen;
    srb_sb_t *srb_sb;

    switch (pak->linktype) {

      /*
       * Packet is transparent.  Convert it to source route.
       */

      case LINK_BRIDGE:

	/* Does pak need 80d5 processing? */

        if (is_80d5_enabled(pak, pak->if_input->hwptr)) {
	    xform_80d5_to_802(pak);
	    tring = (trrif_t *)pak->datagramstart;
	}
	  
	/* Pull a copy of the header into local scratch memory.  This
	 * is to optimize the byte accesses and riftable compares. */

	bcopy(pak->datagramstart, tringmac, TRING_ENCAPBYTES);
	tring = (trrif_t *)tringmac;
        tring->saddr[0] |= TR_RII;

	/* Check for rif cache entry for destination address. */

	rifptr = explorer;
	riflen = RC_NORD_LTH;

	if (!(tring->daddr[0] & TR_GROUP)) {
	    for(entry=riftable[rifhash(tring->daddr)];entry;entry=entry->next){
	        if (ieee_equal_sans_rii(tring->daddr, entry->hardware)
	        &&  entry->idb == vidb) {
	            rifptr = entry->rif;
	            riflen = entry->length;
	            break;
	            }
	    }
	}
		
	/* Adjust mac header and insert rif. */

        pak->datagramstart -= riflen;
        pak->datagramsize  += riflen;

        bcopy(tringmac, pak->datagramstart,  TRING_ENCAPBYTES);
	bcopy(rifptr,   pak->datagramstart + TRING_ENCAPBYTES, riflen);
    
	clear_if_input(pak);
        pak->if_input   = vidb->firstsw;
        pak->if_output  = NULL;
        pak->riflen     = riflen;
        pak->mac_start  = pak->datagramstart;
        pak->addr_start = pak->datagramstart + TR_ACFC_SIZE;
        pak->rif_start  = pak->datagramstart + TRING_ENCAPBYTES;
        pak->flags     |= PAK_SRB_IN;

	/* If rif was found in the cache try to fastswitch the packet.
	 * If this function returns false assume packet will be sent
	 * to process level. */

	if(entry) {

	    /* If netbios name caching learn stations before bridging. */

	    if (global_netbios_name_cache_enable &&
	        netbios_sb_name_caching_is_enabled(vidb->firstsw)) {
	        rif_update(vidb, 0, tring->saddr, NULL,
			         (srbroute_t *)&tring->rc_blth, RIF_AGED);
            }

	    /* Grab a pointer to the matching part of the rif. */

            srb_sb = srb_get_sb_inline(vidb->firstsw);
            pak->rif_match = fs_rif_match((srbroute_t *)pak->rif_start, riflen, 
                                          srb_sb->rng_br_rng_f,
                                          srb_sb->rng_br_rng_r);

	    if(!pak->rif_match) {
                if(srberr_debug)
	            buginf("\n%%SRTLB%d no rif match TLB to SRB, s:%e d:%e",
                                    vidb->unit,tring->saddr,tring->daddr);
	        datagram_done(pak);
		return(TRUE);
		}

	    if ((vidb->tr_srb_fastswitch)(vidb, pak, vidb)) {
                if(srb_debug)
	            buginf("\n%%SRTLB%d fast TLB to SRB, s:%e d:%e",
                                    vidb->unit,tring->saddr,tring->daddr);
	        return(TRUE);
		}

            if(srberr_debug)
	        buginf("\n%%SRTLB%d process switch TLB to SRB, s:%e d:%e",
                                    vidb->unit,tring->saddr,tring->daddr);
	    return(FALSE);
	    }

	/* Control explorer storms from transparent domain. */

	if (reg_invoke_srt_flush_explorer(vidb, pak->datagramsize)) {
	    datagram_done(pak);
	    return(TRUE);
	    }

        pak->flags |= PAK_SRB_EXPLORER;
        return(FALSE);

      /*
       * Packet is source route.  Convert it to transparent.
       */

      case LINK_RSRB:

	/* Convert packet from source route to transparent by taking out
	 * the rif indicator bit and writing over the rif.
	 *
	 * Pull a copy of the header into local scratch memory.  This
	 * is to optimize the byte accesses and riftable compares. */

	bcopy(pak->datagramstart, tringmac, TRING_ENCAPBYTES+pak->riflen);
	tring = (trrif_t *)tringmac;

	/* Learn stations before transparent bridging. */

	rif_update(vidb, 0, tring->saddr, NULL, 
			       (srbroute_t *)&tring->rc_blth, RIF_AGED);

	tring->saddr[0] &= ~TR_RII;

	/* All addresses are canonical in transparent domain. */

	ieee_swap(tring->daddr, tring->daddr);
	ieee_swap(tring->saddr, tring->saddr);

	pak->datagramstart += pak->riflen;
	pak->datagramsize  -= pak->riflen;
	bcopy(tringmac, pak->datagramstart, TRING_ENCAPBYTES);

        pak->mac_start  = pak->datagramstart;
        pak->addr_start = pak->datagramstart + TR_ACFC_SIZE;
	pak->info_start = pak->datagramstart + TRING_ENCAPBYTES;
	pak->riflen     = 0;

	/* If token ring get tr_transit_oui for 8209 compatibility. */

	if (pak->if_input->hwptr->status & IDB_TR)
	    vidb->tr_transit_oui = pak->if_input->hwptr->tr_transit_oui; 
	else
	    vidb->tr_transit_oui = srb_sb_get_rsrb_transit_oui(vidb->firstsw);

	/* Does pak need 80d5 processing? */

	if (is_80d5_enabled(pak, pak->if_input->hwptr)) 
	    xform_802_to_80d5(pak, vidb->tr_transit_oui);

	clear_if_input(pak);
	pak->if_input  = vidb->firstsw;
	pak->if_output = NULL;

	/* Fastswitch the packet in the transparent domain. */

	reg_invoke_vbridge_forward(pak);

        if (srb_debug)
	    buginf("\n%%SRTLB%d fast SRB to TLB s:%e d:%e",
                                   vidb->unit,tring->saddr,tring->daddr);
        return(TRUE);

      default:

	if (srberr_debug) {
	    buginf("\n%%SRTLB%d: unknown fs_srtlb_packets link type %x", 
	      			              vidb->unit, pak->linktype);
            bugtrace();
	    }

	break;
    }

    return(FALSE);
}

boolean rsrb_vbridge_oqueue (
    hwidbtype *idb,
    paktype *pak,
    enum HEADTAIL which)
{
    trrif_t *trh = (trrif_t *)pak->datagramstart;
    int riflen;
    uchar *rifptr;
    riftype *entry;
    int i;
    boolean broadcast;
    sap_hdr *llc1;
    idbtype *swidb = idb->firstsw;

    switch (pak->linktype) {

      case LINK_BRIDGE:

	/*
	 * Packet came from transparent bridge - send to RSRB.
	 */
	  if (is_80d5_enabled(pak, idb)) { /* does pak need 80d5 hack? */
	      xform_80d5_to_802(pak);
	      trh = (trrif_t *)pak->datagramstart;
	  }	      
	  
	/*
	 * Default to assuming that we will broadcast this packet
	 */

	riflen = 2;
	rifptr = rsrb_broadcast_rif;
	broadcast = TRUE;

	/*
	 * If this is not being sent to a group address, look for a
	 * RIF to this destination.
	 */

	if (!(trh->daddr[0] & TR_GROUP)) {
	    entry = riftable[rifhash(&trh->daddr[0])];
	    while (entry) {
		if ((ieee_equal_sans_rii(&trh->daddr[0], entry->hardware)) &&
		    (entry->idb == idb)) {
		    riflen = entry->length;
		    rifptr = entry->rif;
		    broadcast = FALSE;
		    break;
		}
		entry = entry->next;
	    }
	}
		
	/*
	 * Make room in the packet for the RIF, and move it in
	 */

	trh->saddr[0] |= TR_RII;
	bcopy(pak->datagramstart, pak->datagramstart - riflen, TRING_ENCAPBYTES);
	pak->datagramstart -= riflen;
	trh = (trrif_t *) pak->datagramstart;
	pak->datagramsize += riflen;
	bcopy(rifptr, pak->datagramstart + TRING_ENCAPBYTES, riflen);

	clear_if_input(pak);
	pak->if_input = swidb;
	pak->if_output = NULL;

	if (broadcast) {
	    pak->flags |= PAK_SRB_IN;
	    llc1 = (sap_hdr *)((uchar *)&trh->rc_blth + riflen);
	    if (global_netbios_name_cache_enable &&
		  netbios_sb_name_caching_is_enabled(idb->firstsw)) {
  	        netbios_name_cache_update_pak((uchar *) trh,
					      NETBIOS_NAME_AGED, idb, 0);
	    }
	    if ((llc1->dsap == SAP_NETBIOS) &&
		  ((llc1->control & ~LLC1_P) == LLC1_UI) &&
		netbios_name_cache_proxy(pak)) {
		;
	    } else {
		srb_enq(srb_explorerQ, pak);
	    }
	} else {
	    /*
	     * Learn stations as we bridge them FROM the ethernet (if doing
	     * netbios name caching)
	     */
	    if ((global_netbios_name_cache_enable) &&
		(netbios_sb_name_caching_is_enabled(idb->firstsw))) {
		rif_update(idb, 0, trh->saddr, NULL,
			   (srbroute_t *)&trh->rc_blth, RIF_AGED);
	    }
	    vrforward_pak(pak, FALSE, 0, RSRB_RIFLF_LEAVEALONE);
	}
	
	break;

      case LINK_RSRB:

	/*
	 * Packet came from RSRB - going to transparent land.
	 */

	if (!trh->saddr[0] & TR_RII) {
	    if (srberr_debug)
	      buginf("\nRSRB: no RIF on packet sent to virtual bridge");
	    datagram_done(pak);
	    break;
	}

	riflen = trh->rc_blth & RC_LTHMASK;
	if (!riflen) {
	    if (srberr_debug)
	      buginf("\nRSRB: RIF length of zero sent to virtual bridge");
	    datagram_done(pak);
	    break;
	}

	/* tr_transit_oui is needed in transparent bridge process for
	   8209 compatibility if frame came directly from TR interface */
	if (pak->if_input && pak->if_input->hwptr &&
	    (pak->if_input->hwptr->status & IDB_TR))
	    idb->tr_transit_oui =
		pak->if_input->hwptr->tr_transit_oui; 
	else
	    idb->tr_transit_oui = srb_sb_get_rsrb_transit_oui(idb->firstsw);
	clear_if_input(pak);
	pak->if_input = swidb;
	pak->if_output = NULL;

	/*
	 * Learn stations as we bridge them to the ethernet
	 */

	rif_update(idb, 0, trh->saddr, NULL, (srbroute_t *)&trh->rc_blth,
		   RIF_AGED);

	/*
	 * Now remove the RIF from the packet. We do this by moving
	 * the MAC header start upwards in memory - this is less data
	 * moving than moving the entire packet. Note that we do not use
	 * bcopy() - it does not handle overlaps properly
	 */

	trh->saddr[0] &= ~TR_RII;
	for (i = TRING_ENCAPBYTES - 1; i >= 0; --i)
	  pak->datagramstart[riflen + i] = pak->datagramstart[i];

	pak->datagramstart += riflen;
	pak->datagramsize -= riflen;

	if (is_80d5_enabled(pak, idb)) /* Does pak need 80d5 processing? */
	    xform_802_to_80d5(pak, idb->tr_transit_oui);


	/*
	 * Clear flag that would cause it to get handed to SRB.
	 * We want transparent handling.
	 */
	pak->flags &= ~PAK_SRB_IN;
	net_bridge_enqueue(pak);
	break;

      case LINK_IEEE_SPANNING:
        /*
         * Packet is a BPDU from transparent bridge
         *   just drop it - we don't send BPDUs out RSRB
         *
         */
        datagram_done(pak);
        break;

      default:

	if (srberr_debug) {
	    buginf("\nRSRB%d: unknown vbridge_oqueue link code %x",
		                      idb->unit, pak->linktype);
            bugtrace();
	    }

	datagram_done(pak);
	break;
    }
    return(FALSE);		/* Don't start output */
}


/*
 * rsrb_create_virtual_bridge - Create a virtual bridge
 *
 * This routine is used to create the virtual bridge between the token
 * ring virtual ring and a transparent bridge group.
 *
 */

boolean rsrb_create_virtual_bridge (tr_vgrouptype *vrg, int bridge_num,
				    int local_ring, int bridge_group,
				    boolean netbios_cache, ulong transit_oui)
{
    idbtype *swidb;
    hwidbtype *idb;
    srb_sb_t  *srb_sb;
    int i;

    swidb = vidb_malloc();		/* Get a virtual IDB */
    if (!swidb) {
	printf(nomemory);
	return(FALSE);
    }
    idb = swidb->hwptr;

    srb_sb = srb_create_sb(swidb);
    if (!srb_sb) {
	printf(nomemory);
	return(FALSE);
    }
	  
    /*
     * Set up the general IDB fields
     *
     * maxmtu is set to 1500 - 8 because this is the lowest common
     * value.  LF information does not propagate across the bridged
     * network (which can be multimedia).  The far end doesn't know
     * what our largest frame is and we don't know what they are doing.
     */

    idb->state = IDBS_UP;
    idb->status = IDB_VIRTUAL | IDB_TR;
    idb->maxmtu = RC_LF4472_VAL - LLC_DATA_OFFSET;

    if (netbios_cache) {
      netbios_sb_enable_name_caching(idb->firstsw);
    } else {
      netbios_sb_disable_name_caching(idb->firstsw);
    }

    idb->input_defqcount = INPUTQ_DEFAULT;

    for (i = 0; i < IEEEBYTES; i++)
      idb->bia[i] = idb->hardware[i] = BITSWAP(default_mac_addr[i]);

    idb->name = "RingGroup";
    idb->unit = vrg->vrn;
    idb_init_names(swidb, TRUE);

    /*
     * Set up the function dispatch vectors
     */
    idb->oqueue = rsrb_vbridge_oqueue;
    idb->oqueue_dequeue = (oqueue_dequeue_t) return_null;
    idb->reset = (reset_t) return_nothing;
    idb->broadcast = tr_bcast;
    idb->linestate = (linestate_t) return_true;

    /*
     * Vidb's act like TKR interfaces, complete the spoof by
     * setting address/encapsulation routines
     */
    idb->vencap         = tr_vencap;
    idb->extract_hwaddr = ieee_extract_hwaddr;
    idb->parse_packet   = tring_parse_packet;
    idb->extract_snpa   = tr_extract_addr;    
    idb->getlink 	= tring_getlink;

    /*
     * Set up the IDB fields for this interface to be in the ring-group
     */
    srb_sb->rsrb_transit_oui = transit_oui;
    srb_sb->srb_spanning_explorer = TRUE;

    srb_sb->srb_thisring   = local_ring;
    srb_sb->srb_bridge_num = bridge_num;
    srb_sb->srb_targetring = vrg->vrn;

    /*
     * Set up the IDB fields for this interface to be in the bridge-group
     */

    swidb->span_index = bridge_group;
    swidb->port_id = swidb->if_number;
    swidb->ibm_port_priority = SPAN_IBM_PORT_PRIORITY;
    swidb->ibm_message_age_increment = MESSAGE_AGE_INCREMENT;
    swidb->number_bit_mask =  1;           /* Real interfaces start at bit 1 */
    swidb->path_cost = 10;

    /* Configure fastswitching support. */

    swidb->tbridge_media = TBR_VBRIDGE;
    idb->span_bridge_process_force = FALSE;
    idb->srb_routecache = TRUE;

    idb->vbridge_forward   = fs_srtlb_packets;
    idb->fs_expl_func      = fs_srtlb_explorers;
    idb->fs_expl_input     = malloc(sizeof(fs_expl_que));

    srb_sb_init_rng_br_rng_mask(swidb);

    /*
     * Add this interface to the ring-group
     */

    if (!add_vring_interface(vrg, idb, RSRB_ET_LOCAL_VIRTUAL)) {
	vidb_free(swidb);
	return(FALSE);
    }
    vrg->virtual_bridge = swidb;	/* Save pointer to bridge IDB */
    update_peers();

    /*
     * Add this interface to the transparent bridging group
     */

    span_fixup(bridge_group, idb, swidb); /* This will add the interface */
    if (swidb->span_ptr && (swidb->span_ptr)->running)
      span_port_enable(swidb->span_ptr, swidb);

    return(TRUE);
}

/*
 * rsrb_find_virtual_bridge - Find a virtual bridge
 * 
 * This routine is used to find a virtual bridge pointer given the
 * transparent bridge group number. It is used to ensure that the
 * bridge is unique, and is used by the transparent bridging code to
 * build the flooding tables.
 */

idbtype *rsrb_find_virtual_bridge (int bridge_group)
{
    tr_vgrouptype *vrg;

    for (vrg = (tr_vgrouptype *)vringQ.qhead; vrg; vrg = vrg->next)
      if (vrg->virtual_bridge && 
	  vrg->virtual_bridge->span_index == bridge_group)
	return(vrg->virtual_bridge);

    return(NULL);
}

void rsrb_delete_virtual_bridge (tr_vgrouptype *vrg)
{
    idbtype *idb = vrg->virtual_bridge;
    spantype *span;

    if (!idb)
      return;

    vrg->virtual_bridge = NULL;

    span = spanarray[idb->span_index];

    if (span) {
	if (span->running)
	  span_port_disable(span, idb);
	span_delete_port(idb);
    }
	
    remove_vring_interface(idb->hwptr, vrg->vrn);
    set_vring_interfaces();

    free(idb->hwptr->fs_expl_input);
    idb->hwptr->fs_expl_input = NULL;

    free(idb->namestring);
    idb->namestring = NULL;

    free(idb->short_namestring);
    idb->short_namestring = NULL;
    
    vidb_free(idb);
}

/*
 * rsrb_exists_translational
 *
 * Returns TRUE if a translational bridging group exists for the ring group
 * used by the target ring of the given interface.
 */
tr_vgrouptype *rsrb_exists_translational (int rgn)
{
    tr_vgrouptype *vrg;

    vrg = vrfind_ringgroup(rgn);
    if (!vrg) {
        return(FALSE);
    }

    return(vrg->virtual_bridge ? vrg : NULL);
}

/*
 * rsrb_netbios_delete_list
 *
 * Go through all peers, removing a reference to an access list that is being
 * deleted.
 */

void rsrb_netbios_delete_list (netbios_acclisttype *netbios_list)
{
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) {
        if (vrg->peers.qhead == NULL)
            continue;
        for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
            if (vrp->netbios_h_oacf == netbios_list) {
                vrp->netbios_h_oacf = NULL;
            }
            if (vrp->netbios_b_oacf == netbios_list) {
                vrp->netbios_b_oacf = NULL;
            }
            if (vrp->rsrb_netb_out == netbios_list) {
                vrp->rsrb_netb_out = NULL;
            }
        }
    }
    reset_interrupt_level(level);
}


/*
 * r s r b _ o p c o d e _ v a l i d
 *
 * verify that a rsrb opcode is reasonable
 */

boolean rsrb_opcode_valid (uint op)
{
    switch (op) {
      case RSRB_OP_NONE:
      case RSRB_OP_FORWARD:
      case RSRB_OP_EXPLORER:
      case RSRB_OP_RINGXREQ:
      case RSRB_OP_RINGXREPLY:
      case RSRB_OP_NOP:
      case RSRB_OP_VERSIONREQ:
      case RSRB_OP_VERSIONREPLY:
      case RSRB_OP_VERSIONXREQ:
      case RSRB_OP_FORWARDCRC:
      case RSRB_OP_LINKUP_REQ:
      case RSRB_OP_LINKDOWN_REQ:
      case RSRB_OP_LLC2DATA:
      case RSRB_OP_LINKUP_PRSP:
      case RSRB_OP_LINKDOWN_PRSP:
      case RSRB_OP_SDLLC:
      case RSRB_OP_QLLC:
      case RSRB_ET_TR_LANE:
      case RSRB_OP_FORWARDFST:
      case RSRB_OP_FORWARDFSTCRC:
      case RSRB_OP_QUENCH_ON:
      case RSRB_OP_QUENCH_OFF:
      case RSRB_OP_ConnectRequest:
      case RSRB_OP_ConnectPollReq:
      case RSRB_OP_ConnectPollAck:
      case RSRB_OP_ConnectPollAckAndReq:
      case RSRB_OP_ConnectWaitAck:
      case RSRB_OP_DisconnectRequest:
      case RSRB_OP_DisconnectAck:
      case RSRB_OP_CAPS_REQ:
      case RSRB_OP_CAPS_REPLY:
      case RSRB_OP_LA2_QON:
      case RSRB_OP_LA2_QON_ACK:
      case RSRB_OP_LA2_QOFF:
      case RSRB_OP_LA2_QOFF_ACK:
      case RSRB_OP_SEND_XID:
      case RSRB_OP_UNKVRN:
      case RSRB_OP_UNKPEER:
      case RSRB_OP_UNKTRN:
      case RSRB_OP_BADOP:
        return(TRUE);

      default:
        return(FALSE);
    }
}

/* rsrb filtering check */
boolean rsrb_out_check (uchar *dgstart, ulong dgsize, hwidbtype *outidb,
			ushort riflen, tr_vpeertype *vrp)
{
	ushort lsap = 0;
	snap_hdr *llc1;       /* pointer to llc-1 header */

        /*
         * Prevent crash in access_check_type() by 
         *   catching out-of-range rsrb_lsap_out value here
         *
         */
        if (vrp->rsrb_lsap_out >= ACCESSSIZE) {
            /* 
             * Don't really know how rsrb_lsap_out can get out-of-range
             *
             * Collect debug info about peer to help diagnose problem 
             *   when/if it happens again
             *
             * --ppearce (07/29/96)
             *
             */
            if (srberr_debug) {
                char peerid[MAX_PEER_NAME];
                buginf("VRP: rsrb_lsap_out = %d, type = %d, state = %d\n",
                           vrp->rsrb_lsap_out, vrp->type, vrp->state);
                buginf("VRP peerID = %s", vrpeer_id(vrp, peerid));
            }
            return(FALSE);
        }

	if (vrp->rsrb_lsap_out) {
		lsap = GETSHORT((dgstart+TRING_ENCAPBYTES+riflen));
		if (!access_check_type(vrp->rsrb_lsap_out, lsap)) {
			/* if (rsrbfilter_debug) need debug command in the future
				buginf("\nRSRBFLTR: lsap: %x, on %s",lsap,outidb->namestring);
			 */
			return(FALSE);
		}
	}
	/*
	 * Support for NETBIOS access filters
	 */
	 /* Get the DSAP -- must be LLC, UI, and NETBIOS frame for this check */
	 if (vrp->rsrb_netb_out) {
		llc1 = (snap_hdr *)((uint) dgstart + TRING_ENCAPBYTES + riflen);
		if (((llc1->control & ~LLC1_P) == LLC1_UI) &&
			(llc1->dsap == SAP_NETBIOS)) {
			if (!rsrb_netbios_acfcheck(((uchar *) ((uint) dgstart
				+ (TRING_ENCAPBYTES + ET_SAP_LEN + riflen))),
				(dgsize - TRING_ENCAPBYTES - ET_SAP_LEN - riflen), vrp)) {
				/* if (rsrbfilter_debug) need debug command in the future
					buginf("\nRSRBFLTR: rsrb netbios access list filtered on %s", outidb->namestring);
				 */
				return(FALSE);
			}
		}
	}
	return(TRUE);
}

/*
 * rsrb_netbios_acfcheck
 *
 * rsrb netbios filtering check here
 */
boolean rsrb_netbios_acfcheck (uchar *dgstart, ulong dgsize,
			       tr_vpeertype *vrp)
{
    netbios_header *nbh;
    uchar *field;
    netbios_acctype *netb;
    ulong pattern[NETBIOS_MAX_NAME_ULONG];
    int i, length;
    boolean result;
    netbios_acclisttype *thelist = NULL;
    extern int netbios_name_length;
    uchar netbios_dest_mask;
    
    nbh = (netbios_header *) dgstart;

    /* If delimiter not NETBIOS, let it through */
    if ((nbh->delimiter[0] != NETBIOS_DELIMITER_0) ||
	(nbh->delimiter[1] != NETBIOS_DELIMITER_1))
	return(TRUE);

    netbios_dest_mask = (netb_acc_filter_name_recognized ?
                         NETBIOS_DEST_MASK_2 : NETBIOS_DEST_MASK_1);
    if ((nbh->command & ~(NETBIOS_SRC_MASK)) == NETBIOS_SRC_VALUE)
	field = nbh->srcname;
    else if ((nbh->command & ~(netbios_dest_mask)) == NETBIOS_DEST_VALUE)
	field = nbh->destname;
    else
	field = NULL;

	thelist = vrp->rsrb_netb_out;
    
    if (field && thelist) {
        netb = (netbios_acctype *) (thelist->q.qhead);
	result = FALSE;

	memset(pattern, 0, NETBIOS_MAX_NAME_LEN);
	/* Allows for faster compares without worrying about word allignment */
	bcopy(field, (uchar *) pattern, netbios_name_length);

	/* Ugh... space fill as some impl. use null-pad others, space pad */
	for (length = -1, field = (uchar *) pattern, i = 0;
	     i < NETBIOS_MAX_NAME_LEN;
	     i++, field++) {
	    if ((*field == '\0') || (*field == ' ')) {
		if (length < 0)
		  length = i;
		if (*field == '\0')
		  *field = ' ';
	    }
	}
	

     if (length < 0)
            length = netbios_name_length;
        else if (length > 0) {
        /* always use NETBIOS_MAX_NAME_LEN for access-list filter */
            field = (uchar *) pattern;
            if (netbios_name_length < NETBIOS_MAX_NAME_LEN)
                for (i = netbios_name_length;
                    i < NETBIOS_MAX_NAME_LEN; i++)
                    field[i] = ' ';
        }

	for (; netb; netb = netb->next) {
	    if ((length == netb->length) ||
		(netb->prefix && (length > netb->length))) {
		/* (Relatively) fast compare of NETBIOS access lists... */
		if ((netb->pattern[0] == (pattern[0] & ~netb->mask[0])) &&
		    (netb->pattern[1] == (pattern[1] & ~netb->mask[1])) &&
		    (netb->pattern[2] == (pattern[2] & ~netb->mask[2])) &&
		    (netb->pattern[3] == (pattern[3] & ~netb->mask[3]))) {
		    result = netb->result;
		    break;
		}
	    }
	}
	if (result == FALSE)
	    return(FALSE);
    }
	return (TRUE);
}    

/* SRB AST support */
/*
 * create_vidb
 * Create and initialize virtual interface data block.
 */

idbtype * create_vidb (int vrn)
{
    idbtype *swidb;
    hwidbtype *idb;
    srb_sb_t  *srb_sb;

    swidb = vidb_malloc();		/* Get a virtual IDB */
    if (!swidb) {
	printf(nomemory);
	return(FALSE);
    }
    idb = swidb->hwptr;
	  
    srb_sb = srb_create_sb(swidb);
    if (!srb_sb) {
	printf(nomemory);
	return(FALSE);
    }

    idb->vencap = tr_vencap;
    idb->state = IDBS_UP;
    idb->status = IDB_TR | IDB_VIRTUAL | IDB_BRIDGE;
    idb->maxmtu = TR_MTU;
    idb->enctype = idb->enctype_def = ET_SNAP;
    swidb->ip_enctype = ET_SNAP;
    idb->nokeepalive = FALSE;
    idb->reliability = 255;

    swidb->span_index = 0;

    srb_sb->srb_thisring   = vrn;
    srb_sb->srb_targetring = vrn;
    srb_sb->srb_proxy_explorer    = FALSE;
    srb_sb->srb_spanning_explorer = FALSE;

    srb_multiring_enable_all(swidb);

    idb->deencap = NULL;
    idb->soutput = (soutput_t)return_nothing;
    idb->reset = (reset_t)return_nothing;
    idb->shutdown = (shutdown_t)return_nothing;
    idb->getlink = tring_getlink;
    idb->broadcast = tr_bcast;
    idb->periodic = (periodic_t)return_nothing;
    idb->oqueue = rsrb_vidb_oqueue;
    idb->iqueue = rsrb_vidb_iqueue;
    idb->parse_packet = (parse_packet_t) return_nothing;
    idb->counters.resets      = 0;	/* clear reset counter */
    idb->counters.transitions = 0;
    swidb->namestring = "spanRSRB";
    ieee_copy(default_mac_addr, idb->hardware);
    idb->hardware[0] |= TR_LOCAL;
    /* SRB AST Support */
    set_default_interface_bandwidth(swidb, VRING_VISIBLE_BANDWIDTH);
    swidb->disjoint_peers = FALSE;
    swidb->disjoint_bpdu_generation = FALSE;

    return (swidb);
}

/* 
 * rsrb_add_spanidb()
 * This routine is used to add virtual interfaces associated with 
 * AST group to span structure.
 */

void rsrb_add_spanidb(spantype *span, int index, int count)
{
    tr_vgrouptype *vrg;
    hwidbtype *porthw;
    idbtype *port;

    for (vrg = (tr_vgrouptype *)vringQ.qhead; vrg; vrg = vrg->next) {
        if (vrg->span_idb && vrg->span_idb->span_index == index) {
	    if (count) {
		FOR_ALL_HWIDBS(porthw) {
		    port = porthw->firstsw;
		    if ((srb_sb_get_targetring(port) == vrg->vrn) &&
		        (port->span_ptr && (port->span_ptr)->running)) {   
	                span->port[count++] = vrg->span_idb;
		        span->port_count = count;
		        vrg->span_idb->span_ptr = span;
			break;
		    }
	        }
            } else {
	  	/* 
		 * If there is no other physical token ring interface running
		 * AST in this bridge, don't add this virtual interface into
		 * span structure. Disable AST on this virtual interface.
		 */
		if (span->running)
		    span_port_disable(span, vrg->span_idb);
		vrg->span_idb->span_index = 0;
		vrg->span_idb->span_ptr = NULL;
	    }
	}
    }        
}

/*
 * rsrb_delete_virtual_span()
 * This routine removes a virtual interface running spanning tree and
 * also clean up spanning tree structure. Only used when removing 
 * virtual ring group.
 */

void rsrb_delete_virtual_span (tr_vgrouptype *vrg)
{
    idbtype *idb = vrg->span_idb;
    spantype *span;

    if (!idb)
      return;

    vrg->span_idb = NULL;
    span = spanarray[idb->span_index];
    if (span) {
        if (span->running)
          span_port_disable(span, idb);
        span_delete_port(idb);
    }

    vidb_free(idb);
}

/*
 * vrforward_bpdu
 * This routine sends Spanning Tree BPDUs only to remote peers.
 */

void vrforward_bpdu(paktype *pak)
{
    paktype *ppak, *opak;
    idbtype *idb;
    hwidbtype *hwidb;
    tr_vgrouptype *vrg;
    tr_vhdrtype lhdr;
    tr_vpeertype *vrp, *first_vrp;
    char peerid[MAX_PEER_NAME];
    ushort srb_targetring;

    idb = pak->if_input;
    hwidb = hwidb_or_null(idb);
    srb_targetring = srb_sb_get_targetring(idb);
    vrg = (hwidb ? vrfind_ringgroup(srb_targetring) : NULL);
    if (!vrg) {
        if (hwidb) {
            if (srb_debug || srbevent_debug || srberr_debug)
                buginf("\nRSRB(span): ring group %d not found", 
                       srb_targetring);
            srb_sb_incr_rx_drops(idb);
        } else
            errmsg(&msgsym(NOTREM, RSRB));
        vr_nukepak(pak);
        return;
    }    
    ppak = pak;
    lhdr.vrn = vrg->vrn;
    lhdr.len = ppak->datagramsize;

    if (pak->peer_ptr) {
	if (!rsrb_checkqueue(&vrg->peers, pak->peer_ptr)) {
	    if (srberr_debug)
		buginf("\nRSRB(span): BPDU's peer not found");
	    vr_nukepak(pak);
	    return;
	}
	/* 
	 * This is the case where I am sending reply back. 
	 * Called from ieee_received_config_bpdu() in span1.c
	 */
        lhdr.op = RSRB_OP_SPAN;
	first_vrp = pak->peer_ptr;
    } else {
        lhdr.op = RSRB_OP_BPDU;
        first_vrp = NULL;
        for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {
	    if (ip_ouraddress(vrp->ipaddr))
		continue;

	    /* if peer is not open, why bother? */
	    if (vrp->state != RSRB_PS_CONNECTED)
	        continue;

            if (first_vrp) { 
	        /* 
	         * There are more than one peer, and now I am joining  
	         * pak_duplicate club.
	         */
                opak = pak_duplicate(ppak);
                if (!opak) {
                    continue;
	        }
                lhdr.rhdr_version = vrp->vrp_version;
	        if (!(*vrp->sender)(vrp, &lhdr, opak)) {
	            if (srb_debug || srbevent_debug || srberr_debug)
		        buginf("\nRSRB(span): vrp sender failed. BPDU not sent to %s",
		        vrpeer_id(vrp, peerid));	
 	        } else {
	            if (srb_debug)
		        buginf("\nRSRB(span): BPDU sent to %s", 
		            vrpeer_id(vrp, peerid));
	        }
	    } else
	        /* found the first peer */
	        first_vrp = vrp;
        }	
    }
    if (first_vrp) {
        lhdr.rhdr_version = first_vrp->vrp_version;
	if (!(*first_vrp->sender)(first_vrp, &lhdr, ppak)) {
	    if (srb_debug || srbevent_debug || srberr_debug)
		buginf("\nRSRB(span): vrp sender failed. BPDU not sent to %s",
		    vrpeer_id(first_vrp, peerid));       
	} else {
	    if (srb_debug)
		buginf("\nRSRB(span): BPDU sent to %s", 
		    vrpeer_id(first_vrp, peerid));
	}
    } else
	vr_nukepak(ppak);
}

/*
 * rsrb_permit_vidb_span
 * This routine checks to see if is ok to run spanning tree on this
 * virtual interface, and either enable or don't take any action depending
 * on the result.
 */

boolean rsrb_permit_vidb_span (idbtype *port)
{
    hwidbtype *hwidb;
    tr_vgrouptype *vrg;
    tr_vpeertype *vrp;
    int num_open_peers = 0;

    hwidb = hwidb_or_null(port);
    if (!hwidb)
	return(FALSE);
    vrg = vrfind_ringgroup(srb_sb_get_thisring(port));
    if (!vrg) 
	return(FALSE);

    /* 
     * if there are more than one peer in open state, then
     * enable spanning tree on this virtual interface. This way I don't have
     * to go thru entire idbQ to see if a vrp is mine or not.
     */
    for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) {     
        if (vrp->state == RSRB_PS_CONNECTED) {
	    /* Remember I am always in RSRB_PS_CLOSED state */
	    if(++num_open_peers >= 1)
		return(TRUE);
	}
    }		
    
    return(FALSE);
}

/*
 * rsrb_enable_virtual_span
 * This routine is used to enable spanning tree on a virtual interface.
 */
void rsrb_enable_virtual_span(idbtype *vidb)
{
    spantype *span;
    boolean span_running;

    if (!vidb)
	return;

    span = spanarray[vidb->span_index];
    span_running = span && span->running;
 
    if (span_running && rsrb_permit_vidb_span(vidb)) {
	if (vidb->port_state <= PORT_BLOCKING) {
	    span_port_enable(span, vidb);
	}
    }
}

/*
 * rsrb_disable_virtual_span
 * This routine is used to disable spanning tree from a virtual interface.
 */
void rsrb_disable_virtual_span (idbtype *vidb)
{
    spantype *span;
    boolean span_running;
    if (!vidb)
	return;
  
    span = spanarray[vidb->span_index];
    span_running = span && span->running;

    if (span_running && !rsrb_permit_vidb_span(vidb)) {
	vidb->disjoint_peers = FALSE;
	vidb->disjoint_bpdu_generation = FALSE;
	span_port_disable(span, vidb);
    }
}

/*
 * rsrb_vidb_span_forward()
 * This routine decides whether STEs are to be forwarded or not on this
 * vidb.
 * If static spanning tree is configured, span structure does not exist
 * and this routine returns TRUE, and STEs will be forwarded. 
 */

boolean rsrb_vidb_span_forward (tr_vgrouptype *vrg, tr_vpeertype *vrp)
{
    char peerid[MAX_PEER_NAME];
    spantype *span = NULL;

    if (vrp->state == RSRB_PS_CONNECTED) {
	span = spanarray[vrg->span_idb->span_index];
	if (span && span->running) {
	    if (vrg->span_idb->port_state != PORT_FORWARDING) {
		if (srb_debug || srberr_debug) 
		    buginf("\nRSRB: Dropping STE. %s not in forwarding state.",
			vrpeer_id(vrp, peerid));
	        return(FALSE);
	    }
	}
    }
    return(TRUE);
}
/*
 * fair conversation generator
 */
short rsrb_fair_queue_flow_id (paktype *pak)
{
    mac_hdr     *header;
    snap_hdr    *snap;
    int         sum;
 
    pak->fair_weight = FAIR_QUEUE_BASE_WEIGHT;
    header = (mac_hdr *) pak->mac_start;
    if (!header)
 	return linktype_is_fq_conversation(pak);
    snap = (snap_hdr *)(header->data + getRifLenPak(header->daddr, TR_RII));
 
    if (TRING_MAC_MULTICAST & header->daddr[0]) {
	/* multicast: destination only */
	sum = xnethash(header->daddr, IEEEBYTES);
    } else {
	/* unicast: source and destination */
	sum = xnethash(header->daddr, 2*IEEEBYTES);
	if (SAP_SNAP == snap->dsap) {
	    sum += xnethash(snap->oui, SNAP_HDRBYTES-SAP_HDRBYTES);
	} else {
	    /*
	     * Hash in DSAP and SSAP; don't include C/R flag 
	     */
	    sum += snap->dsap;
	    sum += ~SAP_RESPONSE & snap->ssap;
	}
    }
 
    return(fair_queue_sum(pak, sum));
}


/*
 * rsrb_find_vrp()
 *   Returns virtual peer (vrp) that pak was received from
 *   Returns NULL if pak not received from virtual peer
 *
 */
tr_vpeertype *rsrb_find_vrp(paktype *pak)
{
    hwidbtype     *hwidb;
    trrif_t       *trh;
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;


    /* 
     * Get hwidb from pak->if_input
     *   and ensure hwidb is virtual (i.e. an SRB vidb)
     *
     */
    hwidb = pak->if_input->hwptr;
    if (!hwidb)
      return(NULL);

    if (!(hwidb->status & IDB_VIRTUAL))
      return(NULL);
 
    /* 
     * srb_targetring identifies the SRB vRing 
     *
     */
    vrg = vrfind_ringgroup(srb_sb_get_targetring(hwidb->firstsw));
    if (!vrg)
      return(NULL);
 
    /* 
     * RIF_THIRD_RING_BACK() identifies the ring 
     *   that this pak was bridged from (i.e. vre should exist)
     *
     */
    trh = (trrif_t *) pak->datagramstart;
    vre = vrfind_rn2entry(RIF_THIRD_RING_BACK(trh), RSRB_BN_DONTCARE,
                          (tr_ventrytype *) &vrg->rings.qhead, RSRB_ET_REMOTE);
    if (!vre)
      return(NULL);

    /*
     * Return virtual peer corresponding to this vRing entry
     *
     */
    return(vre->outhole.vp);
}


/* 
 * rsrb_is_pak_from_lak_peer()
 *   Returns TRUE if pak received from virtual peer with local-ack enabled
 *
 */
boolean rsrb_is_pak_from_lak_peer(paktype *pak)
{
    tr_vpeertype *vrp = rsrb_find_vrp(pak);
    return((vrp && vrp->local_ack));
}


#undef LLCTRACE
#ifdef LLCTRACE
/*
 * LLC tracing
 */

#define NUMRECS	100

#define M_FRMR	17

#define LPDUBYTES 5

struct llc_trace_rec {
    int count;			/* record count */
    sys_timestamp time;
    int type;
    ulong nr;
    ulong ns;
    ulong func;
    ulong m;
    uchar lpdu[LPDUBYTES];
    ulong userdata[4];
};

static struct llc_trace_rec llc_trace_recs[NUMRECS];
static struct llc_trace_rec *llc_trace_rec = llc_trace_recs;
static struct llc_trace_rec *first_llc_trace_rec = llc_trace_recs;
static int wrapping = 0;
static int count = 0;
int llc_tracing = 0;

void init_llctrace (void)
{
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);

    first_llc_trace_rec = llc_trace_rec = llc_trace_recs;
    wrapping = 0;
    count = 0;

    reset_interrupt_level(status);
}

static struct llc_trace_rec *alloc_llc_trace_rec (void)
{
    struct llc_trace_rec *rec;
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);

    rec = llc_trace_rec;
    llc_trace_rec++;
    if (wrapping) {
	first_llc_trace_rec++;
	if (first_llc_trace_rec == &llc_trace_recs[NUMRECS]) {
	    /* wrapping around */
	    first_llc_trace_rec = llc_trace_recs;
	}
    }
    if (llc_trace_rec == &llc_trace_recs[NUMRECS]) {
	/* wrapping around */
	llc_trace_rec = llc_trace_recs;
	wrapping = 1;
    }

    reset_interrupt_level(status);

    return(rec);
}

static void dump_llc_trace_rec (struct llc_trace_rec *rec)
{
    char buffer[20];
    static const char *const funcs[] = {
	"RR", "RNR", "REJ", "xxx"
    };
    static const char *const *ms[] = {
	"0", "1", "2", "DM response", "4", "5", "6", "7",
	"DISC", "9", "10", "11", "UA response", "13", "14", "SABME",
	"16", "FRMR response", "18", "19", "20", "21", "22", "XID",
	"24", "25", "26", "27", "TEST", "29", "30", "31"
    };

    sprint_ticks_in_dhms(buffer, rec->time);

    switch (rec->type) {
    case LLC_TRC_UFRAME:
	printf("\n%d (%s) -- u-frame: %s", rec->count, buffer, ms[rec->m]);
	if (rec->m == M_FRMR) {
	    if (rec->lpdu[0] & 0x01) {
		/* supervisory or unnumbered frame */
		if (rec->lpdu[0] & 0x02) {
		    /* unnumbered frame */
		    printf(" (U %s)", ms[((rec->lpdu[0] >> 3) & 0x1c) |
					 ((rec->lpdu[0] >> 2) & 3)]);
		} else {
		    /* supervisory frame */
		    printf(" (S %s nr %d)", funcs[(rec->lpdu[0] >> 2) & 3],
			   rec->lpdu[1] >> 1);
		}
	    } else {
		/* information frame */
		printf(" (I ns %d nr %d)", rec->lpdu[0] >> 1,
		       rec->lpdu[1] >> 1);
	    }
	}
	break;
    case LLC_TRC_SFRAME:
	printf("\n%d (%s) -- s-frame: %s nr %d", rec->count, buffer,
	       funcs[rec->func], rec->nr);
	break;
    case LLC_TRC_IFRAME:
	printf("\n%d (%s) -- i-frame: ns %d nr %d", rec->count, buffer,
	       rec->ns, rec->nr);
	break;
    }
    printf(" user %d %d %d %d", rec->userdata[0], rec->userdata[1],
	   rec->userdata[2], rec->userdata[3]);
}

void show_llctrace(void)
{
    struct llc_trace_rec *recs_copy;
    struct llc_trace_rec *rec, *first_copy, *next_copy;
    leveltype status;

    printf("\nLLC tracing is %s.", llc_tracing ? "on" : "off");

    if (count == 0)
	return;

    recs_copy = malloc(sizeof(struct llc_trace_rec) * NUMRECS);
    if (recs_copy == NULL)
	return;

    status = raise_interrupt_level(NETS_DISABLE);
    bcopy(llc_trace_recs, recs_copy, sizeof(struct llc_trace_rec) * NUMRECS);
    first_copy = &recs_copy[first_llc_trace_rec - llc_trace_recs];
    next_copy = &recs_copy[llc_trace_rec - llc_trace_recs];
    reset_interrupt_level(status);

    if (first_copy < next_copy) {
	for (rec = first_copy; rec < next_copy; rec++) {
	    if (automore_quit())
		break;
	    dump_llc_trace_rec(rec);
	}
    } else {
	for (rec = first_copy; rec < &recs_copy[NUMRECS]; rec++) {
	    if (automore_quit())
		break;
	    dump_llc_trace_rec(rec);
	}
	for (rec = recs_copy; rec < next_copy; rec++) {
	    if (automore_quit())
		break;
	    dump_llc_trace_rec(rec);
	}
    }

    free(recs_copy);
}

void make_llctrace (sap_hdr *llc1, ulong udata0, ulong udata1,
		    ulong udata2, ulong udata3)
{
    struct llc_trace_rec *rec;

    rec = alloc_llc_trace_rec();
    if (rec == NULL)
	return;

    rec->count = count++;
    GET_TIMESTAMP(rec->time);
    if (llc1->control & 0x01) {
	/* supervisory or unnumbered frame */
	if (llc1->control & 0x02) {
	    /* unnumbered frame */
	    rec->type = LLC_TRC_UFRAME;
	    rec->m = ((llc1->control >> 3) & 0x1c) |
		((llc1->control >> 2) & 3);
	    if (rec->m == M_FRMR) {
		bcopy(llc1->data, rec->lpdu, LPDUBYTES);
	    }
	} else {
	    /* supervisory frame */
	    rec->type = LLC_TRC_SFRAME;
	    rec->nr = llc1->data[0] >> 1;
	    rec->func = (llc1->control >> 2) & 3;
	}
    } else {
	/* information frame */
	rec->type = LLC_TRC_IFRAME;
	rec->ns = llc1->control >> 1;
	rec->nr = llc1->data[0] >> 1;
    }
    rec->userdata[0] = udata0;
    rec->userdata[1] = udata1;
    rec->userdata[2] = udata2;
    rec->userdata[3] = udata3;
}

void frame_make_llctrace (uchar *frame, ulong udata0, ulong udata1,
			  ulong udata2, ulong udata3)
{
    tring_hdr *trh;
    sap_hdr *llc1;
    int riflen;

    trh = (tring_hdr *) frame;

    if (trh->saddr[0] & TR_RII) {	/* have a rif get its length */
	riflen = trh->data[0] & RC_LTHMASK;
	llc1 = (sap_hdr *) (frame + TRING_ENCAPBYTES + riflen);
    } else {
	riflen = 0;
	llc1 = (sap_hdr *) (frame + TRING_ENCAPBYTES);
    }

    make_llctrace(llc1, udata0, udata1, udata2, udata3);
}
#endif
