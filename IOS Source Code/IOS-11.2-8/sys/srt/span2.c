/* $Id: span2.c,v 3.25.4.29 1996/08/27 17:38:11 syiu Exp $
 * $Source: /release/112/cvs/Xsys/srt/span2.c,v $
 *------------------------------------------------------------------
 * span2.c -- Configure and Monitor the Spanning Tree Protocol 
 *
 * 7-August-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: span2.c,v $
 * Revision 3.25.4.29  1996/08/27  17:38:11  syiu
 * CSCdi67056:  Multilink with compression and bridging not working
 * Branch: California_branch
 *
 * Revision 3.25.4.28  1996/08/26  15:12:51  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.25.4.27  1996/08/14  02:36:40  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.25.4.26  1996/08/12  02:16:14  ppearce
 * CSCdi64450:  CSCdi59527 broke c4500-boot-m image
 * Branch: California_branch
 *   Add srb_core_registry that will definitely be included w/SRB subblock
 *
 * Revision 3.25.4.25  1996/08/07  09:02:43  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.25.4.24  1996/08/03  23:38:10  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.25.4.23  1996/07/26  00:54:10  hampton
 * Fix the automore code in show access-list.  [CSCdi61084]
 * Branch: California_branch
 *
 * Revision 3.25.4.22  1996/07/17  19:32:52  snyder
 * CSCdi63233:  declare some char arrays const to save data
 *              remove a . to let the optimizer save more
 * Branch: California_branch
 *
 * Revision 3.25.4.21  1996/07/17  00:00:50  rbadri
 * CSCdi62168:  LEC continues to be a proxy client, after no bridge-group 1
 * Branch: California_branch
 * Send a config change so that LEC will no longer be a proxy after
 * it is removed from the bridge-group.
 *
 * Revision 3.25.4.20  1996/07/13  11:40:19  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.25.4.19  1996/07/09  06:05:51  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.25.4.18  1996/07/03  06:47:32  wilber
 * CSCdi55164:  Show span should show that spanning tree has been disabled
 * Branch: California_branch
 *
 * Revision 3.25.4.17  1996/06/29  01:07:19  thille
 * CSCdi51480:  Save some code space
 * Branch: California_branch
 *               experimenting with similiar string constants in the same
 *               .o file, worth about 200 bytes.
 *
 * Revision 3.25.4.16  1996/06/28  23:26:28  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.25.4.15  1996/06/25  02:02:01  hou
 * CSCdi59321:  vLAN ISL bridging fails on translation from native to ISL
 * encaps.
 * Branch: California_branch
 * - don't allow vLAN sub-interfaces and main interface configured in the
 *   same bridge group, because loop would occur if we do so.
 *
 * Revision 3.25.4.14  1996/06/24  09:23:17  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.25.4.13  1996/06/17  23:37:44  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.25.4.12  1996/05/31  00:50:18  weismann
 * CSCdi58072:  Frame Relay Subinteface configured for bridging corrupts
 * MAC address. Have bridge parser handle a hwidb flag bridge_transit
 * correctly.
 * Branch: California_branch
 *
 * Revision 3.25.4.11  1996/05/30  23:45:58  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.25.4.10  1996/05/21  22:29:02  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 3.25.4.9  1996/05/19  21:10:09  hou
 * CSCdi57265:  Bridging broke when ISL enabled on FEIP between Catalyst
 * and 7500
 * Branch: California_branch
 * - use short_namestring to show vLAN subinterface in bridge table.
 *
 * Revision 3.25.4.8  1996/05/17  12:14:00  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.22.2.6  1996/05/06  00:01:39  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.22.2.5  1996/04/26  15:23:31  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.22.2.4  1996/04/26  07:58:43  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.22.2.3  1996/04/03  21:59:31  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.22.2.2  1996/03/17  18:50:02  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.22.2.1  1996/02/28  21:24:53  ppearce
 * CSCdi50230:  IBU modularity - Remove unnecessary header file includes
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.25.4.7  1996/05/10  01:43:06  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.25.4.6  1996/05/09  14:46:50  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.25.4.5  1996/05/08  00:22:28  speakman
 * CSCdi56968:  CMF accounting for flooded packets is incorrect
 * Branch: California_branch
 * Show and clear rx and tx flood counts.
 *
 * Revision 3.25.4.4  1996/05/04  01:49:16  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface configuration & bridging
 *   (process-level) support.
 * - BVI smf functions and registry.
 * - BVI settable mtu support & dynamically select mtu for the BVI.
 * - 'bridge irb' & 'bridge <x> route/bridge <protcocol>' support.
 *
 * Constrained Multicast Flooding
 * - Add the CMF parser link points.
 * - Add the CMF data structures to spantype.
 * - Add the creation and deletion of the tbridge monitor.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Manage the CMF data structures when the spanning tree changes.
 *
 * Revision 3.25.4.3  1996/04/29  09:41:01  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.25.4.2  1996/04/17  13:49:07  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.25.4.1  1996/03/18  22:11:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.14.2.5  1996/03/16  07:38:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.14.2.4  1996/03/13  02:02:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.14.2.3  1996/03/09  05:13:38  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.14.2.2  1996/03/07  10:51:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.14.2.1  1996/02/20  18:48:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.25  1996/03/07  23:32:33  speakman
 * CSCdi48222:  SMF creates chunk storage even when unconfigured
 * Replace SMF chunk memory with managed memory and fix the original
 * problem with a simple check for onintstack.
 *
 * Revision 3.24  1996/03/01  00:03:28  speakman
 * CSCdi50360:  %SYS-3-TIMERNEG in spanning tree process
 * lvalues in assignment statements are always nice.
 *
 * Revision 3.23  1996/02/29  21:01:09  speakman
 * CSCdi50360:  %SYS-3-TIMERNEG in spanning tree process
 * Restore the bte short aging trick that was lost in the conversion
 * to the new timer macros.
 *
 * Revision 3.22  1996/02/13  08:16:28  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.21  1996/02/07  16:15:46  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.20  1996/02/05  20:15:30  fred
 * CSCdi46792:  Router Crashes when configuring no service align detect
 *         correct misaligned acces
 *
 * Revision 3.19  1996/02/01  06:10:30  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.18  1996/01/26  22:54:59  rbadri
 * CSCdi47659:  Software MAC filters not set up for 1483 bridging
 * Added the MAC addresses to SMF, regardless of 1483 or LANE bridging.
 *
 * Revision 3.17  1996/01/25  11:48:23  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 * Revision 3.16  1996/01/25  01:41:51  rbadri
 * CSCdi47461:  Spanning tree broken in bridged ELANs
 * For LANE, add interesting MAC addresses.
 *
 * Revision 3.15  1996/01/23  08:54:13  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.14  1996/01/22  07:16:42  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.13  1996/01/16  07:42:29  billw
 * CSCdi46868:  latgroup.o should not be in core subsystem
 *
 * Revision 3.12  1995/12/28  22:43:37  pitargue
 * CSCdi45879:  atm lane int configured as rfc 1483
 * always check how atm interfaces are configured.  don't assume past
 * states
 * of tbridge configurations.  also add some fields to show span and show
 * bridge
 * group to help debug this in the future.
 *
 * Revision 3.11  1995/12/27  18:59:34  hou
 * CSCdi45767:  Attempting to configure bridge-gr X sse causes
 *              %SYS-3-REGNONE:Registry 8 service 12 doesn't exist
 *              error message
 * - bgroup_sse parser node is removed from bgroup tree and dynamically
 *   added to the 7000 parser chain.
 *
 * Revision 3.10  1995/12/17  06:10:39  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.9  1995/12/12  03:31:27  wilber
 * CSCdi44131:  Bridges IP host function failed over serial interface
 * Bridge should listen to(self-receive) the box-wide default MAC address
 * on serial - Populate Serial's SMF with this MAC address
 *
 * Revision 3.8  1995/11/26  08:19:47  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *              When subinterface shutdown, do not remove it from the vLAN.
 *
 * Revision 3.7  1995/11/22  19:20:37  wilber
 * CSCdi42690:  No connectivity over trans-bridging for SOME stations
 * Calculate permission bit pattern for static bridge table entry if
 * interface is specified and MAC-address filter is configured
 *
 * Revision 3.6  1995/11/20  19:29:07  speakman
 * CSCdi42919:  CSCdi41837 broke lego images
 * Move SMF initialization into platform-independent code.
 *
 * Revision 3.5  1995/11/20  18:37:33  rbadri
 * CSCdi38662:  Configuring bridging on multiple ATM sub-interfaces causes
 *              hardware reset
 * Make span_fixup() recognize sub-interfaces, so that for ATM interfaces,
 * we can initiate a config change rather than a hardware reset.
 *
 * Revision 3.4  1995/11/20  15:24:58  pitargue
 * CSCdi42745:  Bad SAP error received when ping over atm remote bridging.
 * make sure that aal5snap vc's are added to the atm tbridge flood list
 *
 * Revision 3.3  1995/11/17  18:56:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:42:30  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:32:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.21  1995/11/08  21:30:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.20  1995/10/26  00:29:46  speakman
 * CSCdi41837:  SMF calls malloc and free at interrupt (SYS-3-INVMEMINT)
 * Convert tbridge and srb to use chunk-based SMF.
 *
 * Revision 2.19  1995/10/21  19:11:22  fred
 * CSCdi39516:  WFQ breaks TB flooding when congested - %SYS-2-BADSHARE
 *         Correct hashing of OUI and PID on SAN Encapsulated bridged
 *         traffi
 *
 * Revision 2.18  1995/10/04  22:47:01  pitargue
 * CSCdi40658:  Forward mac addr. filter failed on dec & ieee bridge
 * (eip-aip-eip)
 * atm needs a map between a mac addr and a vc number.
 *
 * Revision 2.17  1995/09/01  23:05:57  asastry
 * CSCdi39698:  WFQ: Display info (in "show queue") for non-IP protocols
 * as well
 *
 * Revision 2.16  1995/08/25  11:45:27  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.15  1995/08/23 05:31:20  gchristy
 * CSCdi28311:  UDP spanning-tree flooding with DLD creates duplicate
 * packets
 *  - Nuke bridge_flood_pak. Create and initialize udp_flood_pak during FS
 *    init.
 *
 *  - Instead of iterating over all interfaces, use the spanning tree
 *    flood table associated with the input interface to get the list of
 *    interfaces participating in the bridge-group.
 *
 *  - If a potential output interface is part of a circuit-group, use a
 *    hash of the source ip address and destination udp port number to
 *    choose one of the members of the group.
 *
 * Revision 2.14  1995/08/18  23:57:13  speakman
 * CSCdi39005:  CRB commands collide
 * Convert 'bridge <group> route <protocol>' to use PROTONAME macro.
 *
 * Revision 2.13  1995/08/16  22:45:21  wilber
 * CSCdi37565:  Adding bridging access-list causes BRI interface to go down
 * Should check if bridging is configured before reinitialzing the
 * interface
 *
 * Revision 2.12  1995/08/12  07:40:36  fred
 * CSCdi38525:  rewrite fair queue conversation discriminator using IEEE
 * definitions
 *
 * Revision 2.11  1995/08/11  18:34:08  wmay
 * CSCdi38645:  Invalid media message received while bridging on
 * subinterfaces - propogate tbridge_media type to subinterfaces - also
 * add point to point subints to bridge flood list
 *
 * Revision 2.10  1995/08/08  16:49:51  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.9  1995/07/26 03:26:46  ccpoe
 * CSCdi37429:  INverse arp should be enabled by default. Frame-relay PVC
 * dlci number was wrong on nv gen.
 *
 * Revision 2.8  1995/07/09  21:07:24  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Introduce explicit, bridge-group specific route/bridge indicators
 * for CRB, and the commands to configure them.  Provide some smarts
 * to generate the appropriate 'bridge route' commands when CRB is
 * enabled in the presence of existing bridge groups.
 *
 * Revision 2.7  1995/07/06  00:25:18  wilber
 * CSCdi36382:  Age field is not updated correctly for filtered on-net
 * traffic
 * - Create counters to account for filtered on-net traffic
 * - Show those counters only in verbose option of show bridge output
 * - Get timestamp when creating a bridge table entry
 * - Update the bte age based on rx counter plus filtered rx counter
 *
 * Revision 2.6  1995/06/28  09:31:29  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.5  1995/06/21  18:56:31  alam
 * CSCdi36191:  IBM spanning tree doesnt form with SRB/FDDI
 * Add sbridge_smf_update registry in srb_init.
 *
 * Revision 2.4  1995/06/21  09:01:07  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/21  02:06:45  alam
 * CSCdi36191:  IBM spanning tree doesnt form with SRB/FDDI
 * Insert IEEE spanning tree multicast address into SMF when AST over
 * SRB/FDDI is enabled.
 *
 * Revision 2.2  1995/06/09  13:21:38  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:01:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "../../boot/cpu.h"
#include "logger.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "ieee.h"
#include "../if/ether.h"
#include "config.h"
#include "../ui/debug.h"
#include "../wan/serial.h"
#include "span.h"
#include "fastswitch_registry.h"
#include "srt_registry.h"
#include "srb_core_registry.h"
#include "access.h"
#include "../if/fddi.h"
#include "../if/network.h"
#include "../if/priority_private.h"
#include "../wan/frame_relay.h"
#include "../if/tring.h"
#include "../srt/bridgeaccess.h"
#include "../srt/srb_core.h"
#include "../srt/srt_debug.h"
#include "../tbridge/tbridge_debug.h"
#include "parser.h"
#include "parser_defs_bridge.h"
#include "../parser/parser_defs_parser.h"
#include "../tbridge/tbridge_public.h"
#include "../tbridge/tbridge_cmf.h"
#include "../tbridge/tbridge_sde.h"
#include "../tbridge/tbridge_sde_inline.h"
#include "../tbridge/tbridge_bvi.h"
#include "../tbridge/tbridge_registry.h"
#include "../hes/sse_registry.h"
#include "../lat/lat_public.h"
#include "../iprouting/igrp.h"

#include "../vlan/vlan.h"

#include "../h/media_registry.h"

#include "../smf/smf_public.h"
#include "../smf/smf_debug.h"

#include "../if/rif.h"
#include "../srt/lack_externs.h"

#include "../if/atm.h"
#include "../lane/lane.h"
#include "../lane/lane_registry.h"
#include "../if/if_vidb.h"

#include "../wan/ppp.h"
#include "../wan/ppp_registry.h"

const char badspan[] = "\nInvalid spanning tree index";
const char nobridging[] = "\n%s does not support bridging";
const char notsupp[] = " not supported in this software version";
static const char exceedmaxcircidb[] = "\nExceed max number of interfaces in one circuit group";
static const char crb_gen_msg[] = "\nCRB: generating 'bridge %d route %s' configuration command";
static const char irb_gen_msg[] = "\nIRB: generating 'bridge %d route %s' configuration command";
paktype *bridge_sniff_pak;

/*
 * static forward declaration 
 */
static void span_random_mac_address_gen(spantype *span);
static void bvi_mac_address_change(hwidbtype *hwidb);


/*
 * Forward and External Declarations
 */
void bridge_address(parseinfo *csb, spantype *span);
void bgroup_list(parseinfo *csb, idbtype *idb, int *list);
void bgroup_circuit(parseinfo *csb, spantype *span, idbtype *idb);
extern int nfddinets, nTRs, nhssinets;
extern boolean enable_80d5;
extern boolean sap_8209_table[];
extern boolean default_sap_8209_table[];
extern boolean spanevents_debug;
extern void nv_lat_groupmask(uchar *, uchar *, boolean, boolean);
extern boolean lat_parse_grouplist(parseinfo *, uchar *, uchar *);
extern void lat_apply_groupmask(uchar *, uchar *, uchar *);
extern void print_lat_groups(uchar *, int, int, boolean);
extern boolean lat_parse_grouplist(parseinfo *csb,
				   uchar *setmask,
				   uchar *clearmask);
/* For bridged ELANs */
extern void lec_bridge_init(void);
/*
 * The generic bridge filtering code needs to know about the LAT master
 * multicast address even if LAT isn't present in the system.  Everyone
 * else should use the information provided by ../lat/lat_media.c
 */
static uchar bridge_lat_multicast[IEEEBYTES] = {0x09,0x00,0x2B,0x00,0x00,0x0F};


/*
 * Transparent Bridging subsystem header
 */

#define TBRIDGE_MAJVERSION 1
#define TBRIDGE_MINVERSION 0
#define TBRIDGE_EDITVERSION  1

SUBSYS_HEADER(tbridge, TBRIDGE_MAJVERSION, TBRIDGE_MINVERSION,
	      TBRIDGE_EDITVERSION,
	      bridge_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: ppp",
	      NULL);

/*
 * fair conversation generator
 */
static short bridge_fair_queue_flow_id (paktype *pak)
{
    ether_hdr   *header;
    snap_hdr    *snap;
    int 	sum;

    pak->fair_weight = FAIR_QUEUE_BASE_WEIGHT;
    header = (ether_hdr*) pak->mac_start;
    snap = (snap_hdr *) header->data;
    if (ISMULTICAST(header->daddr)) {
	/* multicast: destination only */
	sum = xnethash(header->daddr, IEEEBYTES);
    } else {
	/* unicast: source and destination */
	sum = xnethash(header->daddr, 2*IEEEBYTES);
	if (TYPE_XNS <= GETSHORT(&header->type_or_len)) {
	    /* pick up packet type as two octets */
	    sum += header->data[-1];
	    sum += header->data[-2];
	} else if (SAP_SNAP == header->data[0]) {
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
 * bridge_print_network_header
 *
 * Print Ethernet header.
 */
static void bridge_print_network_header (paktype *pak, ushort **data)
{
    ether_hdr   *header;
    snap_hdr    *snap;

    header = (ether_hdr*) pak->mac_start;
    snap = (snap_hdr *) header->data;

    /* unicast: source and destination */
    printf("\n  source: %e", header->saddr);
    printf(" destination: %e", header->daddr);
    if (TYPE_XNS <= header->type_or_len) {
	/* pick up packet type as two octets */
	printf(" type 0x%04x", header->type_or_len);
    } else if (SAP_SNAP == header->data[0]) {
	printf(" SNAP OUI: %02x-%02x-%02x PID %04x",
	       snap->oui[0], snap->oui[1], snap->oui[2], snap->type);
    } else {
	printf(" 802.2 DSAP: %02x SSAP: %02x CNTL %02x",
	       snap->dsap, snap->ssap, snap->control);
    }
}

/*
 * bridge_init_idb
 * Initialize per-idb data structures
 */

void bridge_init_idb (idbtype* idb, boolean setup_hwidb)
{
    idb->circ_group = 0;
}

/*
 * bridge_hwidb_init
 * Initialize our data structures.  Called by SERVICE_HW_IDB_INIT
 */

void bridge_hwidb_init (hwidbtype *hwidb)
{
    if (hwidb->setup_bridging)
	(*(hwidb->setup_bridging))(hwidb);
}


/*
 * bridge_use_defs
 * Determine whether the default value is used for a spanning protocol
   parameter.
 */

static boolean bridge_use_defs (spantype *span, int proto1, int proto2, 
		     int param, int value1, int value2)
{
    if (((span->protocol == proto1) && 
	 (param == value1)) ||
	((span->protocol == proto2) &&
	 (param == value2)))
	return (TRUE);
    return (FALSE);
}

/*
 * set_def_port_priority
 * set default port priority when a bridge group changes protocol.
 */

static void set_def_port_priority (spantype *span, int new_protocol_type)
{
    int i;
    ushort new_port_id = 0;
    idbtype *idb;
    for (i = 0; i < span->port_count; i++) {
	idb = span->port[i];
	if (!(idb->hwptr->status & IDB_VIRTUAL)) {
	    if (new_protocol_type != SPAN_PROTO_IBM)
		/* if protocol was ibm, now ieee or dec */
		new_port_id = (SPAN_IEEE_PORT_PRIORITY << 8) |
		    (idb->port_id & 0xFF);
	    else {
		/* if protocol wasn't ibm, now ibm */
                srb_vector_table_t *srbV_p;
                srbV_p = reg_invoke_srb_get_vector_table();
                if (srbV_p) {
                    srb_triplet_t srb_triplet;
                    srbV_p->srb_sb_get_triplet(idb, &srb_triplet);
		    new_port_id = ((srb_triplet.localRing << 4) & 0xfff0) | 
                                      (srb_triplet.bridgeNum);
                }
	    }
	    if (span->running) {
                span->protocol = new_protocol_type;
		span_port_priority(span, idb, new_port_id);
	    }
            else {
		idb->port_id = new_port_id;
	    }
	}
    }
}

/* 
 * remove_vidb
 * When protocol changes from IBM to IEEE or DEC, remove ring
 * group spanning virtual idb from span structure.
 */

static void remove_vidb(spantype *span)
{
    int i;
    int count = span->port_count;

    for (i = 0; i < count; i++) {
	if ((span->protocol == SPAN_PROTO_IBM) && 
	    (span->port[i]->hwptr->status & IDB_VIRTUAL)) {
	    if (span->running)
		span_port_disable(span, span->port[i]);
	    span_delete_port(span->port[i]);
	}
    }
}

static void bvi_mac_address_change (hwidbtype *hwidb)
{
    if(!is_bvi(hwidb)) {
	return;
    }

    tbridge_bvi_initialize(hwidb->firstsw);
}

/*
 * bridge_command
 * Define parameters for a spanning tree
 */

void bridge_command (parseinfo *csb)
{
    spantype *span;
    idbtype *idb;
    int index, ibm_index, value;
    boolean span_running;
    int	i;
    btetype *mac;
    int protocol_type = 0;		/* SRB AST support */


    index = GETOBJ(int,1);

    if (!bridge_enable) {
	if (!csb->nvgen) {
	    printf("\nBridging%s", notsupp);
	}
	return;
    }

    if (csb->nvgen) {

	if (csb->which == BRIDGE_CRB) {
	    nv_write(global_crb_enable, "%s", csb->nv_command);
	    return;
	}

	if (csb->which == BRIDGE_IRB) {
	    nv_write(global_irb_enable, "%s", csb->nv_command);
	    return;
	}

	span = spanarray[index];
	if (!span  ||  !span->running) {
	    return;
	}

	switch (csb->which) {

	  case BRIDGE_PROTOCOL:
	    nv_write(GETOBJ(int,2) == span->protocol, "%s", csb->nv_command);
	    break;

	  case BRIDGE_ROUTE:
	    if (span->route_linktypes & LINK_APOLLO_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " %s %s", csb->nv_command,
			 print_linktype(LINK_APOLLO));
	    if (span->route_linktypes & LINK_APPLETALK_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " %s %s", csb->nv_command,
			 print_linktype(LINK_APPLETALK));
	    if (span->route_linktypes & LINK_CLNS_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " %s %s", csb->nv_command,
			 print_linktype(LINK_CLNS));
	    if (span->route_linktypes & LINK_DECNET_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " %s %s", csb->nv_command,
			 print_linktype(LINK_DECNET));
	    if (span->route_linktypes & LINK_IP_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " %s %s", csb->nv_command,
			 print_linktype(LINK_IP));
	    if (span->route_linktypes & LINK_NOVELL_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " %s %s", csb->nv_command,
			 print_linktype(LINK_NOVELL));
	    if (span->route_linktypes & LINK_VINES_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " %s %s", csb->nv_command,
			 print_linktype(LINK_VINES));
	    if (span->route_linktypes & LINK_XNS_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " %s %s", csb->nv_command,
			 print_linktype(LINK_XNS));
	    break;

	  case BRIDGE_BRIDGE:
	    /*
	     * Note that for bridge_linktypes, if the bit is 
	     *  set(1)	: it means this protocol is NOT bridged
	     *  clear(0): it means this protocol IS bridged
	     */
	    if (span->bridge_linktypes & LINK_APOLLO_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " no %s %s", csb->nv_command,
			 print_linktype(LINK_APOLLO));
	    if (span->bridge_linktypes & LINK_APPLETALK_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " no %s %s", csb->nv_command,
			 print_linktype(LINK_APPLETALK));
	    if (span->bridge_linktypes & LINK_CLNS_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " no %s %s", csb->nv_command,
			 print_linktype(LINK_CLNS));
	    if (span->bridge_linktypes & LINK_DECNET_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " no %s %s", csb->nv_command,
			 print_linktype(LINK_DECNET));
	    if (span->bridge_linktypes & LINK_IP_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " no %s %s", csb->nv_command,
			 print_linktype(LINK_IP));
	    if (span->bridge_linktypes & LINK_NOVELL_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " no %s %s", csb->nv_command,
			 print_linktype(LINK_NOVELL));
	    if (span->bridge_linktypes & LINK_VINES_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " no %s %s", csb->nv_command,
			 print_linktype(LINK_VINES));
	    if (span->bridge_linktypes & LINK_XNS_RBI)
		nv_write(global_crb_enable || global_irb_enable, 
			 " no %s %s", csb->nv_command,
			 print_linktype(LINK_XNS));
	    break;

	  case BRIDGE_ACQUIRE:
            nv_write(!span->acquire, "no %s", csb->nv_command);
	    break;

	  case BRIDGE_ADDRESS:
            /* Generate any address entries in the Bridge Table */
            for (i=0; i < NETHASHLEN; i++) {

                for (mac = span->bridge_tbl[i]; mac; mac = mac->bte_next) {

                    if (mac->bte_attributes & BTE_STATIC) {
    
                        if (mac->bte_attributes & BTE_IFSPECIFIED) {
        
                            char dlci_str[16];

                            if (is_frame_relay(mac->bte_interface->hwptr)) {
                                sprintf(dlci_str,"%d",
                                        mac->bte_proxy_addr.fr_dlci);
                            } else if (is_atm(mac->bte_interface->hwptr)) {
                                sprintf(dlci_str,"%d",
                                        mac->bte_proxy_addr.vc_num);
                            } else {
                                dlci_str[0] = '\0';
			    }

                            nv_write(TRUE, "%s %e %s %s %s",
				     csb->nv_command, 
				     &mac->bte_mac_addr.sword[0],
                                     baction2string(mac->bte_action),
                                     mac->bte_interface->namestring, dlci_str);
                        } else {
                            nv_write(TRUE, "%s %e %s",
				     csb->nv_command, 
				     &mac->bte_mac_addr.sword[0],
                                     baction2string(mac->bte_action));
                        }
                    }
                }
            }
	    break;

	  case BRIDGE_DOMAIN:
            nv_write((span->protocol == SPAN_PROTO_IEEE) && span->domain,
                     "%s %d", csb->nv_command, (span->domain >> 4));
	    break;

	  case BRIDGE_AGINGTIME:
            nv_write((span->bridge_aging_time != BTE_MAXAGE / ONESEC),
                     "%s %d", csb->nv_command, span->bridge_aging_time);
	    break;

	  case BRIDGE_FORWARDTIME:
            nv_write((((span->bridge_forward_delay != SPAN_IEEE_FORWARDDELAY) &&
		       (span->protocol == SPAN_PROTO_IEEE)) ||
		      ((span->bridge_forward_delay != SPAN_DEC_FORWARDDELAY) &&
		       (span->protocol == SPAN_PROTO_DEC)) ||
		      ((span->bridge_forward_delay != SPAN_IBM_FORWARDDELAY) &&
		       (span->protocol == SPAN_PROTO_IBM))),
                     "%s %d", csb->nv_command, span->bridge_forward_delay);
	    break;

	  case BRIDGE_HELLOTIME:
            nv_write((((span->bridge_hello_time != SPAN_IEEE_HELLOTIME) &&
		       (span->protocol == SPAN_PROTO_IEEE)) ||
		      ((span->bridge_hello_time != SPAN_DEC_HELLOTIME) &&
		       (span->protocol == SPAN_PROTO_DEC)) ||
		      ((span->bridge_hello_time != SPAN_IBM_HELLOTIME) &&
		       (span->protocol == SPAN_PROTO_IBM))),
                     "%s %d", csb->nv_command, span->bridge_hello_time);
	    break;

	  case BRIDGE_LATFILTER:
            nv_write(span->lat_svc_filter, "%s", csb->nv_command);
	    break;

	  case BRIDGE_MAXAGE:
            nv_write(((span->bridge_max_age != SPAN_IEEE_MAXAGE) &&
		      (span->protocol == SPAN_PROTO_IEEE)) ||
                     ((span->bridge_max_age != SPAN_DEC_MAXAGE) &&
		      (span->protocol == SPAN_PROTO_DEC)) ||
                     ((span->bridge_max_age != SPAN_IBM_MAXAGE) &&
		      (span->protocol == SPAN_PROTO_IBM)),
                     "%s %d", csb->nv_command, span->bridge_max_age);
	    break;

	  case BRIDGE_MCASTSRC:
            nv_write(span->mcast_source, "%s", csb->nv_command);
	    break;

	  case BRIDGE_PRIORITY:
            nv_write((((span->priority != SPAN_IEEE_BRIDGE_PRIORITY) &&
		       ((span->protocol == SPAN_PROTO_IEEE) ||
			(span->protocol == SPAN_PROTO_IBM))) ||
		      ((span->priority != SPAN_DEC_BRIDGE_PRIORITY) &&
		       (span->protocol == SPAN_PROTO_DEC))),
		     "%s %d", csb->nv_command, span->priority);
	    break;

	  case BRIDGE_CCG:
            for (value = 1; value < CCG_MAXGROUPS; value++) {

                nv_write(span->cct_groups[value].ccg_pause_interval !=
                         CCG_DEFAULT_PAUSE_INTERVAL,
			 "%s %d pause %d", csb->nv_command, value,
			 span->cct_groups[value].ccg_pause_interval);

                nv_write(span->cct_groups[value].srcmac_based,
			 "%s %d source-based", csb->nv_command, value);
            }
	    break;

	case BRIDGE_VLANDOMAIN:
	    nv_write(span->dname_length != 0,"%s %s vlan-id %d",
		     csb->nv_command, span->domain_name, 
		     span->vlan_id); 
	    break;
	  default:
	    break;
        }

        return;    /* From doing NV Generation. */
    }

    if (csb->which == BRIDGE_CRB) {

	ushort *rxtype_flags;

	if (csb->sense) {	/* Enable CRB if not already enabled. */
	    if (FALSE == global_crb_enable) {

		/*
		 * To avoid any confusion, for now enforce user 
		 * to choose either CRB or IRB, but not both.
		 */
		if (TRUE == global_irb_enable) {
		    printf("\nIRB(Integrated Routing and Bridging) is" 
			   " already configured!");
		    printf("\nPlease deconfigure IRB first!");
		    return;
		}

		/*
		 * For each currently active, non-IBM bridge group,
		 * determine whether to route a given protocol in the
		 * bridge group based on whether ANY interface in the
		 * bridge group is currently configured to route that
		 * protocol.
		 *
		 * This is just a precaution that keeps unsuspecting
		 * users who configure CRB in the presence of existing
		 * bridge groups from getting burned by the default
		 * CRB behaviour which is to bridge all protocols unless
		 * explicitly configured to route them.
		 *
		 * This precaution will generate 'bridge route' config
		 * commands for those apparently routed protocols.
		 *
		 * If CRB is already enabled, this precaution does NOT
		 * kick in.
		 *
		 * NOTA BENE: do this check BEFORE setting global_crb_enable
		 * since it calls ROUTING_IF.
		 */

		for (index = 1; index < SPAN_MAXINDEX; index++) {
		    if ((span = spanarray[index]) &&
			(span->protocol != SPAN_PROTO_IBM) &&
			(span->running)) {

			/* Start by assuming that all routeable are bridged. */
			span->route_linktypes = 0x0;

			for (i = 0; i < span->port_count; i++) {
			    idb = span->port[i];
			    if (ROUTING_IF(idb, LINK_APOLLO) &&
				(!(span->route_linktypes & LINK_APOLLO_RBI))) {
				span->route_linktypes |= LINK_APOLLO_RBI;
				printf(crb_gen_msg, index, "apollo");
			    }
			    if (ROUTING_IF(idb, LINK_APPLETALK) &&
				(!(span->route_linktypes &
				   LINK_APPLETALK_RBI))) {
				span->route_linktypes |= LINK_APPLETALK_RBI;
				printf(crb_gen_msg, index, "appletalk");
			    }
			    if (ROUTING_IF(idb, LINK_CLNS) &&
				(!(span->route_linktypes & LINK_CLNS_RBI))) {
				span->route_linktypes |= LINK_CLNS_RBI;
				printf(crb_gen_msg, index, "clns");
			    }
			    if (ROUTING_IF(idb, LINK_DECNET) &&
				(!(span->route_linktypes & LINK_DECNET_RBI))) {
				span->route_linktypes |= LINK_DECNET_RBI;
				printf(crb_gen_msg, index, "decnet");
			    }
			    if (ROUTING_IF(idb, LINK_IP) &&
				(!(span->route_linktypes & LINK_IP_RBI))) {
				span->route_linktypes |= LINK_IP_RBI;
				printf(crb_gen_msg, index, "ip");
			    }
			    if (ROUTING_IF(idb, LINK_NOVELL) &&
				(!(span->route_linktypes & LINK_NOVELL_RBI))) {
				span->route_linktypes |= LINK_NOVELL_RBI;
				printf(crb_gen_msg, index, "novell");
			    }
			    if (ROUTING_IF(idb, LINK_VINES) &&
				(!(span->route_linktypes & LINK_VINES_RBI))) {
				span->route_linktypes |= LINK_VINES_RBI;
				printf(crb_gen_msg, index, "vines");
			    }
			    if (ROUTING_IF(idb, LINK_XNS) &&
				(!(span->route_linktypes & LINK_XNS_RBI))) {
				span->route_linktypes |= LINK_XNS_RBI;
				printf(crb_gen_msg, index, "xns");
			    }
			}
		    }
		}

		global_crb_enable = TRUE;

		/*
		 * Allocate per-interface instances of the rxtype
		 * route/bridge indicators and initialize them.
		 */
		FOR_ALL_SWIDBS(idb) {

		    /*
		     * Initialize the route/bridge indicators from
		     * the value recorded for the bridge group if
		     * there is one for this interface.
		     */
		    idb->tbridge_route_linktypes = (idb->span_ptr) ?
			(idb->span_ptr->route_linktypes) : (0x0);

		    rxtype_flags = idb->rxtype_flags;
		    if (!rxtype_flags || (rxtype_flags == global_rxtype_flags))
			rxtype_flags = malloc_named(SIZEOF_RXTYPE_FLAGS,
						    "CRB rxtypes");
		    if (rxtype_flags) {
			idb->rxtype_flags = rxtype_flags;
			reg_invoke_rxtypes_update(idb);
		    }
		}
	    }
	} else {		/* Disable CRB if not already disabled. */
	    if (TRUE == global_crb_enable) {
		global_crb_enable = FALSE;

		for (index = 1; index < SPAN_MAXINDEX; index++)
		    if ((span = spanarray[index]))
			span->route_linktypes = 0x0;

		/*
		 * Deallocate per-interface instances of the rxtype
		 * route/bridge indicators and point all interfaces
		 * at the global instance.
		 */
		FOR_ALL_SWIDBS(idb) {
		    idb->tbridge_route_linktypes = 0x0;
		    rxtype_flags = idb->rxtype_flags;
		    if (rxtype_flags && (rxtype_flags != global_rxtype_flags))
			free(rxtype_flags);
		    idb->rxtype_flags = global_rxtype_flags;
		    reg_invoke_rxtypes_update(idb);
		}
	    }
	}
	return;
    }

    if (csb->which == BRIDGE_IRB) {

        ushort *rxtype_flags;

        if (csb->sense) {       /* Enable IRB if not already enabled. */
            if (FALSE == global_irb_enable) {

		/*
		 * To avoid any confusion, for now enforce user 
		 * to choose either CRB or IRB, but not both.
		 */
		if (TRUE == global_crb_enable) {
		    printf("\nCRB(Concurrent Routing and Bridging) is" 
			   " already configured!");
		    printf("\nPlease deconfigure CRB first!");
		    return;
		}

		/*
		 * IRB is supported only on packet-memory platforms.
		 */
		if (IS_FAMILY(FAMILY_RP1) || IS_FAMILY(FAMILY_CSC2)) {
		    printf("\nIRB(Integrated Routing and Bridging) is"
			   " not supported on this platform!");
		    return;
		}

                /*
                 * For each currently active, non-IBM bridge group,
                 * determine whether to route a given protocol in the
                 * bridge group based on whether ANY interface in the
                 * bridge group is currently configured to route that
                 * protocol.
                 *
                 * This is just a precaution that keeps unsuspecting
                 * users who configure IRB in the presence of existing
                 * bridge groups from getting burned by the default
                 * IRB behaviour which is to bridge all protocols unless
                 * explicitly configured to route them.
                 *
                 * This precaution will generate 'bridge route' config
                 * commands for those apparently routed protocols.
                 *
                 * If IRB is already enabled, this precaution does NOT
                 * kick in.
                 *
                 * NOTA BENE: do this check BEFORE setting global_irb_enable
                 * since it calls ROUTING_IF.
                 */

                for (index = 1; index < SPAN_MAXINDEX; index++) {
                    if ((span = spanarray[index]) &&
                        (span->protocol != SPAN_PROTO_IBM) &&
                        (span->running)) {
			
			/* Start by assuming that all routeable are bridged. */
                        span->route_linktypes = 0x0;
                        span->bridge_linktypes = 0x0;
                        for (i = 0; i < span->port_count; i++) {
                            idb = span->port[i];
                            if (ROUTING_IF(idb, LINK_APOLLO) &&
                                (!(span->route_linktypes & LINK_APOLLO_RBI))) {
                                span->route_linktypes |= LINK_APOLLO_RBI;
                                printf(irb_gen_msg, index, "apollo");
			    }
                            if (ROUTING_IF(idb, LINK_APPLETALK) &&
                                (!(span->route_linktypes &
                                   LINK_APPLETALK_RBI))) {
                                span->route_linktypes |= LINK_APPLETALK_RBI;
                                printf(irb_gen_msg, index, "appletalk");
                            }
                            if (ROUTING_IF(idb, LINK_CLNS) &&
                                (!(span->route_linktypes & LINK_CLNS_RBI))) {
                                span->route_linktypes |= LINK_CLNS_RBI;
                                printf(irb_gen_msg, index, "clns");
                            }
                            if (ROUTING_IF(idb, LINK_DECNET) &&
                                (!(span->route_linktypes & LINK_DECNET_RBI))) {
                                span->route_linktypes |= LINK_DECNET_RBI;
                                printf(irb_gen_msg, index, "decnet");
			    }
                            if (ROUTING_IF(idb, LINK_IP) &&
                                (!(span->route_linktypes & LINK_IP_RBI))) {
                                span->route_linktypes |= LINK_IP_RBI;
                                printf(irb_gen_msg, index, "ip");
                            }
                            if (ROUTING_IF(idb, LINK_NOVELL) &&
                                (!(span->route_linktypes & LINK_NOVELL_RBI))) {
                                span->route_linktypes |= LINK_NOVELL_RBI;
                                printf(irb_gen_msg, index, "novell");
			    }
                            if (ROUTING_IF(idb, LINK_VINES) &&
                                (!(span->route_linktypes & LINK_VINES_RBI))) {
                                span->route_linktypes |= LINK_VINES_RBI;
                                printf(irb_gen_msg, index, "vines");
			    }
                            if (ROUTING_IF(idb, LINK_XNS) &&
                                (!(span->route_linktypes & LINK_XNS_RBI))) {
                                span->route_linktypes |= LINK_XNS_RBI;
                                printf(irb_gen_msg, index, "xns");
                            }
			}
		    }
		}

                global_irb_enable = TRUE;

                /*
                 * Allocate per-interface instances of the rxtype
                 * route/bridge indicators and initialize them.
                 */
                FOR_ALL_SWIDBS(idb) {

                    /*
                     * Initialize the route/bridge indicators from
                     * the value recorded for the bridge group if
                     * there is one for this interface.
                     */
                    idb->tbridge_route_linktypes = (idb->span_ptr) ?
                        (idb->span_ptr->route_linktypes) : (0x0);
		    
                    idb->tbridge_bridge_linktypes = (idb->span_ptr) ?
                        (idb->span_ptr->bridge_linktypes) : (0x0);
		    
                    rxtype_flags = idb->rxtype_flags;
                    if (!rxtype_flags || (rxtype_flags == global_rxtype_flags))
                        rxtype_flags = malloc_named(SIZEOF_RXTYPE_FLAGS,
                                                    "IRB route rxtypes");
                    if (rxtype_flags)
                        idb->rxtype_flags = rxtype_flags;
                    if (!rxtype_flags) {
                        printf(nomemory);
                    } else {
                        reg_invoke_rxtypes_update(idb);
                    }
		}
	    }
	} else {                /* Disable IRB if not already disabled. */
            if (TRUE == global_irb_enable) {

                /*
                 * Ask user to take out the Bridge-group Virtual interface
                 * before disabling IRB.
                 */
                for (index = 1; index < SPAN_MAXINDEX; index++) {
                    if ((span = spanarray[index])) {
                        if (span->irb_bg_vidb) {
                            printf("\nPlease remove the BVI %d"
                                   " interface first!", index);
                            return;
                        }
                    }
                }
		
                global_irb_enable = FALSE;

                /*
                 * Get back to default: Bridge everything
                 * In fact, both are not referenced if C/IRB is disabled
                 */
                for (index = 1; index < SPAN_MAXINDEX; index++)
                    if ((span = spanarray[index])) {
                        span->route_linktypes = 0x0;
                        span->bridge_linktypes = 0x0;
                    }
                /*
                 * Deallocate per-interface instances of the rxtype
                 * route/bridge indicators and point all interfaces
                 * at the global instance.
                 *
		 */
                FOR_ALL_SWIDBS(idb) {
                    idb->tbridge_route_linktypes = 0x0;
                    idb->tbridge_bridge_linktypes = 0x0;
		    
                    rxtype_flags = idb->rxtype_flags;
                    if (rxtype_flags && (rxtype_flags != global_rxtype_flags))
                        free(rxtype_flags);
                    idb->rxtype_flags = global_rxtype_flags;
                    reg_invoke_rxtypes_update(idb);
                }
            }
        }
        return;
    }

    if (((csb->which == NO_BRIDGE) || (csb->which == BRIDGE_PROTOCOL))
	&& (!csb->sense)) {

	boolean		goodnight_pumpkin;
	int		ix;

	span = spanarray[index];
	if (NULL == span)
	    return;

	/*
	 * Turn this off BEFORE either deleting tbridge_monitor or,
	 * more importantly, freeing the data structures allocated
	 * by tbridge_monitor.
	 */
	span->running = FALSE;

	/*
	 * If this is the last bridge group, delete tbridge_monitor.
	 */
	goodnight_pumpkin = TRUE;
	for (ix = 1; ix < SPAN_MAXINDEX; ix++) {
	    if ((ix != index) && (spanarray[ix] != NULL)) {
		goodnight_pumpkin = FALSE;
	    }
	}
	if (goodnight_pumpkin)
	    tbridge_monitor_lights_out();

	/*
	 * Free all multicast group data structures.
	 */
	reg_invoke_tbridge_cmf_clear(span);

        span_delete(index);
	return;
    }

    /*
     * Remember if the spanning tree is already running.
     */
    span_running = FALSE;
    span = spanarray[index];

    /*
     * Ensure that a bridging/Spanning Tree protocol has already 
     * been configured, before allowing other dependent commands.
     */

    if ((span == NULL) && (csb->which != BRIDGE_PROTOCOL)) {

	printf("\n\nSpanning Tree protocol for Bridge Group %d has not been"
	       " configured.\n", index);
	return;

    }

    if ((span) && (span->running)) {
	span_running = TRUE;
    }

    if (csb->which == BRIDGE_PROTOCOL) {
	protocol_type = GETOBJ(int, 2);
    } else {
	/* span data structure is already created */
	protocol_type = span->protocol;
    }

    /* SRB AST support */
    span = span_lookup(index, TRUE, protocol_type);
    if (!span) {
	printf(nomemory);
	return;
    }

    switch (csb->which) {
      case BRIDGE_ACQUIRE:
	span->acquire = csb->set_to_default || csb->sense;
	gc_stations(span, FALSE);	/* flush any existing temp entries */
	break;
      case BRIDGE_ADDRESS:
	bridge_address(csb, span);
	break;
      case BRIDGE_DOMAIN:
	if (span->protocol != SPAN_PROTO_IEEE) {
	    printf("\nDomains only apply to ieee protocol bridge groups");
	    return;
	}
	if (csb->sense)
	    value = GETOBJ(int,2) << 4;
	else
	    value = 0;
	/* Don't change if we are already in the domain */
	if (span->domain != (uchar) value) {
	    span->domain = value;
	    if (span->running) {
		span_bridge_priority(span, span->priority);
	    }
	    span_initialize(span);
	}
	break;
      case BRIDGE_FORWARDTIME:
	if (csb->sense) {
	    span->bridge_forward_delay = GETOBJ(int,2);
	} else {
	    if (span->protocol == SPAN_PROTO_IEEE) {
		span->bridge_forward_delay = SPAN_IEEE_FORWARDDELAY;
	    } else if (span->protocol == SPAN_PROTO_DEC) {
		span->bridge_forward_delay = SPAN_DEC_FORWARDDELAY;
	    } else
		span->bridge_forward_delay = SPAN_IBM_FORWARDDELAY;
	}
	    
        /*
         * If root of the spanning tree operation then update 
	 * running forward delay timer.
	 */

        if (root_bridge(span))
	    span->forward_delay = span->bridge_forward_delay;
        break;

      case BRIDGE_AGINGTIME:
	if (csb->sense) {
	    span->bridge_aging_time = (GETOBJ(int,2));
	} else {
	    span->bridge_aging_time = BTE_MAXAGE / ONESEC;
	}
       	break;
      case BRIDGE_HELLOTIME:
	if (csb->sense) {
	    span->bridge_hello_time = GETOBJ(int,2);
	} else {
	    if (span->protocol == SPAN_PROTO_IEEE) {
		span->bridge_hello_time = SPAN_IEEE_HELLOTIME;
	    } else if (span->protocol == SPAN_PROTO_DEC) {
		span->bridge_hello_time = SPAN_DEC_HELLOTIME;
	    } else
		span->bridge_hello_time = SPAN_IBM_HELLOTIME;
	}

	/*
	 * If root of the spanning tree -- update running hello timer.
	 */
	if (root_bridge(span)) {
	    span->hello_time = GETOBJ(int,2);
	}
	break;
      case BRIDGE_LATFILTER:

	/*
	 * Set the filter to the requested sense. If this is an
	 * enable, add the LAT multicast to our station list.
	 * Fix up the spanning tree so that the LAT multicast is
	 * properly enabled or disabled.
	 */

	span->lat_svc_filter = csb->sense;
	if (csb->sense) {
	    FOR_ALL_SWIDBS(idb) {
		reg_invoke_tbridge_smf_update(bridge_lat_multicast,
					      idb,
					      SMF_MACTYP_LAT_MCAST |
					      SMF_ACTION_RECEIVE);
	    }
	} else {
	    FOR_ALL_SWIDBS(idb) {
		reg_invoke_tbridge_smf_delete(bridge_lat_multicast,
					      idb,
					      SMF_MACTYP_LAT_MCAST |
					      SMF_ACTION_RECEIVE);
	    }
	}
	span_fixup(index, (hwidbtype *) NULL, (idbtype *)NULL);
	break;

      case BRIDGE_MAXAGE:

	if (csb->sense) {
	    span->bridge_max_age = GETOBJ(int,2);
	} else {
	    if (span->protocol == SPAN_PROTO_IEEE) {
		span->bridge_max_age = SPAN_IEEE_MAXAGE;
	    } else if (span->protocol == SPAN_PROTO_DEC) {
		span->bridge_max_age = SPAN_DEC_MAXAGE;
	    } else 
		span->bridge_max_age = SPAN_IBM_MAXAGE;
	}
	
	/*
	 * If root of the spanning tree -- update running max age timer.
	 */

	if (root_bridge(span)) 
	    span->max_age = span->bridge_max_age;
	break;

      case BRIDGE_MCASTSRC:
	span->mcast_source = csb->sense;
	break;
      case BRIDGE_PRIORITY:
	if (csb->sense) {
	    span->priority = GETOBJ(int,2);
	} else {
	    if ((span->protocol == SPAN_PROTO_IEEE) ||
		(span->protocol == SPAN_PROTO_IBM)) {
		span->priority = SPAN_IEEE_BRIDGE_PRIORITY;
	    } else {
		span->priority = SPAN_DEC_BRIDGE_PRIORITY;
	    }
	}
	span_bridge_priority(span, span->priority);
	break;
	
      case BRIDGE_PROTOCOL:
	if (!csb->sense) {
	    span_delete(index);
	    return;
	}
	protocol_type = GETOBJ(int, 2);

	switch (protocol_type) {
	    /* SRB AST support */
	    case SPAN_PROTO_IBM:
 	        if (subsys_find_by_name("srb") == NULL) {
 		     printf("\nSource Route Bridging is not available in this image.");
 		     return;
 		}
	    	ibm_index = reg_invoke_srt_span_index(index);
	    	if ((ibm_index < SPAN_MAXINDEX) && 
		    (index != ibm_index)) {
		    printf("\nBridge Group %d is running the IBM protocol"
			   " already.", ibm_index);
		    span_delete(index);
		    return;
		}
		for (i = 0; i < span->port_count; i++) {
		    if (!is_srb_hw(span->port[i]->hwptr)) {
			printf("\n%%All interfaces in bridge group %d \
must support source-route bridging.", index);
			printf("\n%%Bridge command will not be accepted.");
			return;
		    }
		}
		if (bridge_use_defs(span, SPAN_PROTO_IEEE,
				    SPAN_PROTO_DEC,
				    span->bridge_max_age,
				    SPAN_IEEE_MAXAGE, SPAN_DEC_MAXAGE))
		    span->bridge_max_age = SPAN_IBM_MAXAGE;

		if (bridge_use_defs(span, SPAN_PROTO_IEEE,
				    SPAN_PROTO_DEC,
				    span->bridge_hello_time,
				    SPAN_IEEE_HELLOTIME, SPAN_DEC_HELLOTIME))
		    span->bridge_hello_time = SPAN_IBM_HELLOTIME;

		if (bridge_use_defs(span, SPAN_PROTO_IEEE,
				    SPAN_PROTO_DEC,
				    span->bridge_forward_delay,
				    SPAN_IEEE_FORWARDDELAY, 
				    SPAN_DEC_FORWARDDELAY))
		    span->bridge_forward_delay = SPAN_IBM_FORWARDDELAY;

		if (bridge_use_defs(span, SPAN_PROTO_IEEE,
				    SPAN_PROTO_DEC,
				    span->priority,
				    SPAN_IEEE_BRIDGE_PRIORITY,
				    SPAN_DEC_BRIDGE_PRIORITY))
		    span->priority = SPAN_IEEE_BRIDGE_PRIORITY;

		span->path_divisor = SPAN_IBM_PATHDIVISOR;

		if (span->protocol != SPAN_PROTO_IBM)
		    set_def_port_priority(span, protocol_type);

		span_bridge_priority(span, span->priority);
		reg_invoke_srt_add_all_spanidb(index, span);
		break;
	    case SPAN_PROTO_IEEE:
		if (bridge_use_defs(span, SPAN_PROTO_IBM,
				    SPAN_PROTO_DEC,
				    span->bridge_max_age,
				    SPAN_IBM_MAXAGE, SPAN_DEC_MAXAGE))
		    span->bridge_max_age = SPAN_IEEE_MAXAGE;

		if (bridge_use_defs(span, SPAN_PROTO_IBM,
				    SPAN_PROTO_DEC,
				    span->bridge_hello_time,
				    SPAN_IBM_HELLOTIME, SPAN_DEC_HELLOTIME))
		    span->bridge_hello_time = SPAN_IEEE_HELLOTIME;

		if (bridge_use_defs(span, SPAN_PROTO_IBM,
				    SPAN_PROTO_DEC,
				    span->bridge_forward_delay,
				    SPAN_IBM_FORWARDDELAY, 
				    SPAN_DEC_FORWARDDELAY))
		    span->bridge_forward_delay = SPAN_IEEE_FORWARDDELAY;

		if (bridge_use_defs(span, SPAN_PROTO_IBM,
				    SPAN_PROTO_DEC,
				    span->priority,
				    SPAN_IEEE_BRIDGE_PRIORITY,
				    SPAN_DEC_FORWARDDELAY))
		    span->priority = SPAN_IEEE_BRIDGE_PRIORITY;

		span->path_divisor = SPAN_IEEE_PATHDIVISOR;

		if (span->protocol == SPAN_PROTO_IBM) {
		    remove_vidb(span);
		    set_def_port_priority(span, protocol_type);
		}
			    
	        span_bridge_priority(span, span->priority);

		break;
	    case SPAN_PROTO_DEC:
		if (bridge_use_defs(span, SPAN_PROTO_IEEE,
				    SPAN_PROTO_IBM,
				    span->bridge_max_age,
				    SPAN_IEEE_MAXAGE, SPAN_IBM_MAXAGE))
		    span->bridge_max_age = SPAN_DEC_MAXAGE;

		if (bridge_use_defs(span, SPAN_PROTO_IEEE,
				    SPAN_PROTO_IBM,
				    span->bridge_hello_time,
				    SPAN_IEEE_HELLOTIME, SPAN_IBM_HELLOTIME))
		    span->bridge_hello_time = SPAN_DEC_HELLOTIME;

		if (bridge_use_defs(span, SPAN_PROTO_IEEE,
				    SPAN_PROTO_IBM,
				    span->bridge_forward_delay,
				    SPAN_IEEE_FORWARDDELAY, 
				    SPAN_IBM_FORWARDDELAY))
		    span->bridge_forward_delay = SPAN_DEC_FORWARDDELAY;

		if (bridge_use_defs(span, SPAN_PROTO_IEEE,
				    SPAN_PROTO_IBM,
				    span->priority,
				    SPAN_IEEE_BRIDGE_PRIORITY,
				    SPAN_IEEE_BRIDGE_PRIORITY))
		    span->priority = SPAN_DEC_BRIDGE_PRIORITY;

		span->path_divisor = SPAN_DEC_PATHDIVISOR;

		if (span->protocol == SPAN_PROTO_IBM) {
		    remove_vidb(span);
		    set_def_port_priority(span, protocol_type);
		}

                /*
                 * If previously this Bridge Group was running the IEEE
                 * flavour then may need to deconfigure any features that
                 * are compatible only with the IEEE Spanning Tree.
                 * Very much a corner case, but occurs when administrators
                 * suddenly discover a old DEC bridge in their network !
                 */

                else if (span->protocol == SPAN_PROTO_IEEE) {

                    check_802_10_subif_bridging(span, TRUE);
                }

	        span_bridge_priority(span, span->priority);
		
		break;
	    default:
		return;
	}
	for (i = 0; i < span->port_count; i++) {
	    idb = span->port[i];
	    if (!idb->path_cost_explicit) /* if not explicitly set */
		idb->path_cost = SCALED_BANDWIDTH(idb->visible_bandwidth) /
		    span->path_divisor;
	    if (idb->path_cost == 0) {
		/*
		 * Could happen for ATM and so protect against it.
		 */
		idb->path_cost = 1;
	    }
	}
	/*
	 * Only call span_initialize() if we are changing the protocol
	 * or starting it for the first time.
	 */
	if ((!span_running) || (span->protocol != protocol_type)) {
	    span->protocol = protocol_type;
	    span_initialize(span);
	}

	/*
	 * Create tbridge_monitor.
	 */
	tbridge_monitor_fiat_lux();

	break;

      case BRIDGE_ROUTE:
	if (global_crb_enable || global_irb_enable) {

	    boolean update_needed = FALSE;

	    switch (GETOBJ(int,2)) {
	    case LINK_APOLLO:
		if (csb->sense) {
		    if (!(span->route_linktypes & LINK_APOLLO_RBI)) {
			span->route_linktypes |= LINK_APOLLO_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (span->route_linktypes & LINK_APOLLO_RBI) {
			span->route_linktypes &= ~LINK_APOLLO_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_APPLETALK:
		if (csb->sense) {
		    if (!(span->route_linktypes & LINK_APPLETALK_RBI)) {
			span->route_linktypes |= LINK_APPLETALK_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (span->route_linktypes & LINK_APPLETALK_RBI) {
			span->route_linktypes &= ~LINK_APPLETALK_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_CLNS:
		if (csb->sense) {
		    if (!(span->route_linktypes & LINK_CLNS_RBI)) {
			span->route_linktypes |= LINK_CLNS_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (span->route_linktypes & LINK_CLNS_RBI) {
			span->route_linktypes &= ~LINK_CLNS_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_DECNET:
		if (csb->sense) {
		    if (!(span->route_linktypes & LINK_DECNET_RBI)) {
			span->route_linktypes |= LINK_DECNET_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (span->route_linktypes & LINK_DECNET_RBI) {
			span->route_linktypes &= ~LINK_DECNET_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_IP:
		if (csb->sense) {
		    if (!(span->route_linktypes & LINK_IP_RBI)) {
			span->route_linktypes |= LINK_IP_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (span->route_linktypes & LINK_IP_RBI) {
			span->route_linktypes &= ~LINK_IP_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_NOVELL:
		if (csb->sense) {
		    if (!(span->route_linktypes & LINK_NOVELL_RBI)) {
			span->route_linktypes |= LINK_NOVELL_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (span->route_linktypes & LINK_NOVELL_RBI) {
			span->route_linktypes &= ~LINK_NOVELL_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_VINES:
		if (csb->sense) {
		    if (!(span->route_linktypes & LINK_VINES_RBI)) {
			span->route_linktypes |= LINK_VINES_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (span->route_linktypes & LINK_VINES_RBI) {
			span->route_linktypes &= ~LINK_VINES_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_XNS:
		if (csb->sense) {
		    if (!(span->route_linktypes & LINK_XNS_RBI)) {
			span->route_linktypes |= LINK_XNS_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (span->route_linktypes & LINK_XNS_RBI) {
			span->route_linktypes &= ~LINK_XNS_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    }

	    if (update_needed)
		for (i = 0; i < span->port_count; i++) {
		    idb = span->port[i];
		    idb->tbridge_route_linktypes = span->route_linktypes;
		    reg_invoke_rxtypes_update(idb);
		}
	} else {
	    printf("\n Concurrent Routing and Bridging (CRB) or"
		   "\n Integrated Routing and Bridging (IRB) must be"
		   "\n enabled before specifying routed protocols in"
		   "\n a bridge group (see the 'bridge crb/irb' command.)");
	}
	break;

      case BRIDGE_BRIDGE:
	if (global_crb_enable || global_irb_enable) {

	    boolean update_needed = FALSE;

	    /*
	     * Note that for bridge_linktypes, if the bit is 
	     *  set(1)	: it means this protocol is NOT bridged
	     *  clear(0): it means this protocol IS bridged
	     */
	    switch (GETOBJ(int,2)) {
	    case LINK_APOLLO:
		if (csb->sense) {
		    if (span->bridge_linktypes & LINK_APOLLO_RBI) {
			span->bridge_linktypes &= ~LINK_APOLLO_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (!(span->bridge_linktypes & LINK_APOLLO_RBI)) {
			span->bridge_linktypes |=  LINK_APOLLO_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_APPLETALK:
		if (csb->sense) {
		    if (span->bridge_linktypes & LINK_APPLETALK_RBI) {
			span->bridge_linktypes &= ~LINK_APPLETALK_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (!(span->bridge_linktypes & LINK_APPLETALK_RBI)) {
			span->bridge_linktypes |= LINK_APPLETALK_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_CLNS:
		if (csb->sense) {
		    if (span->bridge_linktypes & LINK_CLNS_RBI) {
			span->bridge_linktypes &= ~LINK_CLNS_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (!(span->bridge_linktypes & LINK_CLNS_RBI)) {
			span->bridge_linktypes |= LINK_CLNS_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_DECNET:
		if (csb->sense) {
		    if (span->bridge_linktypes & LINK_DECNET_RBI) {
			span->bridge_linktypes &= ~LINK_DECNET_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (!(span->bridge_linktypes & LINK_DECNET_RBI)) {
			span->bridge_linktypes |= LINK_DECNET_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_IP:
		if (csb->sense) {
		    if (span->bridge_linktypes & LINK_IP_RBI) {
			span->bridge_linktypes &= ~LINK_IP_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (!(span->bridge_linktypes & LINK_IP_RBI)) {
			span->bridge_linktypes |= LINK_IP_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_NOVELL:
		if (csb->sense) {
		    if (span->bridge_linktypes & LINK_NOVELL_RBI) {
			span->bridge_linktypes &= ~LINK_NOVELL_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (!(span->bridge_linktypes & LINK_NOVELL_RBI)) {
			span->bridge_linktypes |= LINK_NOVELL_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_VINES:
		if (csb->sense) {
		    if (span->bridge_linktypes & LINK_VINES_RBI) {
			span->bridge_linktypes &= ~LINK_VINES_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (!(span->bridge_linktypes & LINK_VINES_RBI)) {
			span->bridge_linktypes |= LINK_VINES_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    case LINK_XNS:
		if (csb->sense) {
		    if (span->bridge_linktypes & LINK_XNS_RBI) {
			span->bridge_linktypes &= ~LINK_XNS_RBI;
			update_needed = TRUE;
		    }
		} else {
		    if (!(span->bridge_linktypes & LINK_XNS_RBI)) {
			span->bridge_linktypes |= LINK_XNS_RBI;
			update_needed = TRUE;
		    }
		}
		break;
	    }

	    if (update_needed)
		for (i = 0; i < span->port_count; i++) {
		    idb = span->port[i];
		    idb->tbridge_bridge_linktypes = span->bridge_linktypes;
		    reg_invoke_rxtypes_update(idb);
		}
	} else {
	    printf("\n Concurrent Routing and Bridging (CRB) or"
		   "\n Integrated Routing and Bridging (IRB) must be"
		   "\n enabled before specifying bridged protocols in"
		   "\n a bridge group (see the 'bridge crb/irb' command.)");
	}
	break;

      case BRIDGE_CCG_PAUSE:
	index = GETOBJ(int,2);
	if (csb->sense) {
	    span->cct_groups[index].ccg_pause_interval = GETOBJ(int,3);
	} else {
	    span->cct_groups[index].ccg_pause_interval = CCG_DEFAULT_PAUSE_INTERVAL;
	}
       	break;

      case BRIDGE_CCG_SRC:
	index = GETOBJ(int,2);
	if (csb->sense) {
	    span->cct_groups[index].srcmac_based = TRUE;
	} else {
	    span->cct_groups[index].srcmac_based = FALSE;
	}
        break;

    case BRIDGE_VLANDOMAIN:
	strcpy(span->domain_name, GETOBJ(string,1));
	span->dname_length = strlen(span->domain_name);
	span->vlan_id = GETOBJ(int,2);
	break;
	
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * bridge_address
 * Parse the "bridge <group> address <address> <action>" command.
 */

void bridge_address (parseinfo *csb, spantype *span)
{
    uchar *etheraddress;
    btetype *mac, *last;
    idbtype *interface;
    int code, hash;
    leveltype level;
    boolean success = TRUE;
    int dlci = 0;
    fr_idb_struct_type *fr_idb = 0;

    /*
     * Parse Ethernet address and action keyword.
     */
    
    etheraddress = GETOBJ(hwaddr,1)->addr;
    code = 0;
    interface = NULL;
    if (csb->sense) {
	code = GETOBJ(int,2);	/* Either BRIDGE_FORWARD or BRIDGE_DISCARD */
	interface = GETOBJ(idb,1);
	if (interface && ((interface->hwptr->status & IDB_BRIDGE) == 0)) {
	    printf(nobridging, interface->namestring);
	    return;
	}
	
	/*
	 * Call a list of interested parties to let them know about this
	 */
	reg_invoke_bridge_address(csb, span, &success);

	/*
	 * Oops. Someone didn't like this for some reason. Just return.
	 */
	if (!success)
	    return;
    }

    /*
     * Now get a DLCI if this interface is frame-relay en\cap'ed
     */
    if (interface && is_frame_relay(interface->hwptr)) {
	dlci = GETOBJ(int,3);
	fr_idb = interface->hwptr->frame_relay_stuff;
    }

    /*
     * get a vc number if this interface is an atm interface
     */
    if (interface && is_atm(interface->hwptr)) {
	dlci = GETOBJ(int,3);
    }

    /*
     * Search table for existing entry
     */
    hash = etheraddress[4] ^ etheraddress[5];
    level = raise_interrupt_level(NETS_DISABLE);
    for (last = NULL, mac = span->bridge_tbl[hash]; mac; mac = mac->bte_next) {
	if (ieee_equal((uchar *)&mac->bte_mac_addr.sword[0], etheraddress)) {
	    if (csb->sense) {
		mac->bte_action = code;
		mac->bte_attributes |= BTE_STATIC;
                mac->bte_interface = interface;
		if (interface) {
		    mac->bte_attributes |= BTE_IFSPECIFIED;
		    if (is_frame_relay(interface->hwptr) ||
			is_atm_dxi(interface->hwptr)) {
			mac->bte_proxy_addr.fr_dlci = dlci;
		    } else if (is_atm(interface->hwptr)) {
			mac->bte_proxy_addr.vc_num = dlci;
		    }
                    if (interface->span_address_in ||
                        interface->span_address_out)
                        bridge_address_check(mac);
		} else {
		    mac->bte_attributes &= ~BTE_IFSPECIFIED;
		}
                /*
		 * Update hardware copy of bridge-cache entry.
                 */
		reg_invoke_srt_bce_update(mac);
	    } else
                bridge_entry_remove(span, mac, last);

	    reset_interrupt_level(level);
	    return;
	}
	last = mac;
    }
    reset_interrupt_level(level);
    
    /*
     * If no existing entry, add one.
     */
    if (csb->sense) {
	if (!bte_count)
	    grow_stations(BTE_ALIQUOT);
	mac = p_dequeue(&bteQ);
	if (!mac)
	    return;
	bte_count--;
	ieee_copy(etheraddress, (uchar *)&mac->bte_mac_addr.sword[0]);
	mac->bte_interface = interface;
	mac->bte_rxcount = 0;
	mac->bte_txcount = 0;
	mac->bte_auto_rxcount = 0;
	mac->bte_auto_txcount = 0;
	mac->bte_action = code;
	mac->bte_attributes = BTE_STATIC;
	if (interface) {
	    mac->bte_attributes |= BTE_IFSPECIFIED;

	    if (is_frame_relay(interface->hwptr) ||
		is_atm_dxi(interface->hwptr)){
		mac->bte_proxy_addr.fr_dlci = dlci;
	    } else if (is_atm(interface->hwptr)) {
		mac->bte_proxy_addr.vc_num = dlci;
	    }

	    if ((interface->hwptr->status & IDB_FDDI) &&
		(interface->hwptr->fddi_options & FDDI_ENCAPSULATE)) {
		if (interface->hwptr->type == IDBTYPE_FDDIT)
		    ieee_copy(transit_bridge_multicast,
			      mac->bte_proxy_addr.proxy_mac);
		else
		    ieee_copy(bit_swapped_transit_multicast,
			      mac->bte_proxy_addr.proxy_mac);
	    }

            if (interface->span_address_in || interface->span_address_out)
                bridge_address_check(mac);

            /*
	     * Create the bridge-cache entry if appropriate
             */
	    reg_invoke_srt_bce_add(span, mac, interface->hwptr);
	}
	level = raise_interrupt_level(NETS_DISABLE);
	mac->bte_next = span->bridge_tbl[hash];
	span->bridge_tbl[hash] = mac;
	reset_interrupt_level(level);
    }
}

/*
 * typeaccess_command
 * Parse an Ethernet type access list.
 *	"[no] access-list <list> permit|deny <value> <wildmask>"
 */

void typeaccess_command (parseinfo *csb)
{
    int number;
    boolean grant;
    ushort value, mask;
    typetype *tmp;
    typetype *grp;
    int i, j;

    if (csb->nvgen) {
	for (i = MINTYPEACC; i <= MAXTYPEACC; i++) {
	    grp = type_get_acl(i);
	    if (!grp)
		continue;
	    for (j = 0; j < grp->items; j++) {
		nv_write(TRUE,"%s %d %s 0x%-04x 0x%-04x", 
			 csb->nv_command, i,
			 grp->item[j].grant ? "permit" : "deny  ",
			 grp->item[j].value, grp->item[j].mask);
	    }
	}
	return;
    }

    number = GETOBJ(int,1);
    grant = (boolean)GETOBJ(int,2);

    /*
     * Check if we're deleting this list
     */
    if (!csb->sense) {
	tmp = type_get_acl(number);
        type_set_acl(number, NULL);
	if (tmp)
	    free(tmp);
	access_mark_list_changed(number);
	return;
    }
    
    /*
     * Get the value and mask.
     * Mask off the value now to save recomputation at interrupt level.
     */
    value = (ushort)GETOBJ(int,3);
    mask = (ushort)GETOBJ(int,4);
    value &= ~mask;
    
    /*
     * Get pointer to access list or create one
     */
    grp = type_get_acl(number);
    if (!grp) {
	grp = malloc(sizeof(typetype));
	if (!grp)
	    return;
	grp->items = 0;
	type_set_acl(number, grp);
    }
    
    /*
     * Make sure that item is not already in list
     */
    for (i = 0; i < grp->items; i++) {
	if ((grp->item[i].value == value) && (grp->item[i].mask == mask) &&
	    (grp->item[i].grant == grant))
	    return;
    }
    
    /*
     * Insert item into list if there is room
     */
    if (grp->items >= MAXTYPEITEMS) {
	printf("list %d overflowed", number);
	return;
    }
    i = grp->items++;
    grp->item[i].grant = grant;
    grp->item[i].value = value;
    grp->item[i].mask = mask;

    access_mark_list_changed(number);
}

/*
 * addressaccess_command(csb, number, grant)
 * Parse the Ethernet address access list.
 * 	[no] access-list <list> permit|deny <ethernet address> <wildcard>
 */

void addressaccess_command (parseinfo *csb)
{
    int number;
    int i;
    boolean grant;
    mac_address_type etheraddress, ethermask;
    macaddracctype *item, *wrk_item;
    
    if (csb->nvgen) {
	for (i = MINMACADDRACC; i <= MAXMACADDRACC; i++) {
	    item = mac_get_std_acl(i);
	    while (item) {
		nv_write(TRUE,"%s %d %s %e   %e", 
			 csb->nv_command, i,
			 item->grant ? "permit" : "deny  ",
			 &item->addr.u.sword[0], 
			 &item->mask.u.sword[0]);
		item = item->next;
	    }
	}
	return;
    }

    /* Get list number and permit/deny flag */
    number = GETOBJ(int,1);
    grant = GETOBJ(int,2);

    if (!csb->sense) {
	item = mac_get_std_acl(number);
	mac_set_std_acl(number, NULL);
	while (item) {
	    wrk_item = item->next;
	    free(item);
	    item = wrk_item;
	}
	set_station_bits(number);
	access_mark_list_changed(number);
	return;
    }
    
    /*
     * Get the etheraddress.
     */
    ieee_copy(GETOBJ(hwaddr,1)->addr, &etheraddress.u.byte[0]);

    /*
     * Get the ethernet mask -- if not present use an 0000.0000.0000 mask.
     */
    if (GETOBJ(hwaddr,2)->type == STATION_IEEE48) {
	ieee_copy(GETOBJ(hwaddr,2)->addr, &ethermask.u.byte[0]);
    } else {
	ethermask.u.lword[0] = 0;
	ethermask.u.sword[2] = 0;
    }

    /*
     * Clear any bit in etheraddress that is set in the mask
     * so that the comparisons work as expected:
     */
    etheraddress.u.lword[0] &= ~ethermask.u.lword[0];
    etheraddress.u.sword[2] &= ~ethermask.u.sword[2];
    
    /*
     * Check to ensure that an entry does not exist that exactly matchs what
     * we are attempting to add. If it does, exit stage left.
     */
    item = mac_get_std_acl(number);
    wrk_item = mac_get_std_acl(number);
    while (item) {
	if ((item->addr.u.lword[0] == etheraddress.u.lword[0]) &&
	    (item->addr.u.sword[2] == etheraddress.u.sword[2]) &&
	    (item->mask.u.lword[0] == ethermask.u.lword[0]) &&
	    (item->mask.u.sword[2] == ethermask.u.sword[2]) &&
	    (item->grant == grant))
	    return;
	wrk_item = item;
	item = item->next;
	
    }	
    
    /*
     * Finally -- add the entry to the list.
     */
    
    item = malloc(sizeof(macaddracctype));
    if (!item)
	return;
    item->grant = grant;
    item->addr.u.lword[0] = etheraddress.u.lword[0];
    item->addr.u.sword[2] = etheraddress.u.sword[2];
    item->mask.u.lword[0] = ethermask.u.lword[0];
    item->mask.u.sword[2] = ethermask.u.sword[2];
    if (!wrk_item)
	mac_set_std_acl(number, item);
    else
	wrk_item->next = item;
    
    /*
     * If we are *denying* access to a ethernet address; we need to see
     * if the new address (and mask) matches any station entries in the
     * spanning tree.
     */
    set_station_bits(number);
    access_mark_list_changed(number);
}

/*
 * set_station_bits
 * This function is called when an access list that is used for the
 * spanning tree protocol is defined or cleared or when a input or output
 * address list is added to an interface via the bridge-group interface
 * sub-command. Loop thru all the interfaces looking for a match on the 
 * access list *number* and set or clear the appropriate bit in the 
 * bridging *btetype*.
 */

void set_station_bits (int number)
{
    idbtype *idb;
    hwidbtype *hwidb;
    int port_bit, i, index, port_word;
    spantype *span;
    btetype *mac;
    
    for (index = 1; index < SPAN_MAXINDEX; index++) {
	span = spanarray[index];
	if (span) {
	    for (i = 0; i < NETHASHLEN; i++) {
		mac = span->bridge_tbl[i];
		while (mac) {
		    idb = mac->bte_interface;
		    if (idb) {
			port_bit = idb->number_bit_mask;
			port_word = idb->bit_mask_word;
			if (access_check_mac(number,(uchar *)mac->bte_mac_addr.sword)){

			    /*
			     * Permit access by setting the appropriate bit.
			     */
			    if (idb->span_address_in == number)
				mac->bte_permit_in[port_word] |= port_bit;
			    if (idb->span_address_out == number)
				mac->bte_permit_out[port_word] |= port_bit;
			} else {
			    
			    /*
			     * Deny access by clearing the appropriate bit.
			     */
			    if (idb->span_address_in == number)
				mac->bte_permit_in[port_word] &= ~port_bit;
			    if (idb->span_address_out == number)
				mac->bte_permit_out[port_word] &= ~port_bit;
			}

			/*
			 * Update hardware copy of bridge-cache entry.
			 */
			reg_invoke_srt_bce_update(mac);
		    }
		    mac = mac->bte_next;
		}
	    }
	}
    }

    FOR_ALL_HWIDBS(hwidb) {
	/*
	 * If the interface has bridging configured and has a 
	 * setup_bridging routine, call it to make the interface 
	 * driver notice any changes:
	 */
	if (hwidb->tbridge_on_hwidb && hwidb->setup_bridging)
	    (*(hwidb->setup_bridging))(hwidb);
    }

}

/*
 * span_lookup
 * Create or delete a bridge data structure
 * Caller has to check the index is valid
 * Return NULL if no memory to create spantype struct
 */

spantype *span_lookup (int index, boolean startflag, int protocol_type)
{
    spantype *span;
    boolean ibm_span_type;	/* SRB AST support */

    /* SRB AST support */
    ibm_span_type = (protocol_type == SPAN_PROTO_IBM);
    span = spanarray[index];

    if (!span) {
	/*
	 * Ugly check for cbus platforms
	 */
	if ((IS_FAMILY(FAMILY_CSC2) || IS_FAMILY(FAMILY_RP1)) &&
	    (!ibm_span_type)) {
	    int bufsize;

	    /*
	     * Get an I/O buffer to use for flooding at interrupt level
	     */

	    bufsize = MAXETHERSIZE;
	    if (nTRs)
		bufsize = max(bufsize, MAXTRSIZE);
	    if (nfddinets)
		bufsize = max(bufsize, MAXFDDISIZE);
	    if (nhssinets)
		bufsize = max(bufsize, MAXDS3SIZE);

	    if (!bridge_sniff_pak) {
		/*
		 * SMALLDATA should be more than enough.
		 */
		bridge_sniff_pak = getbuffer(SMALLDATA);
		if (!bridge_sniff_pak)
		    return(NULL);
	    }
	} /* if protocol type is not IBM */

	/*
	 * Got the buffer, now allocate the spanning tree data structure
	 */

	span = malloc(sizeof(spantype));
	if (!span)
	    return(NULL);
	spanarray[index] = span;
	span->spanindex = index;
	span->protocol = SPAN_PROTO_IEEE;
	span->path_divisor = SPAN_IEEE_PATHDIVISOR;
	if (ibm_span_type) {
	    span->protocol = SPAN_PROTO_IBM;
	    span->bridge_max_age = SPAN_IBM_MAXAGE;
	    span->bridge_hello_time = SPAN_IBM_HELLOTIME;
	    span->bridge_forward_delay = SPAN_IBM_FORWARDDELAY;
	} else {
	    span->bridge_max_age = SPAN_IEEE_MAXAGE;
	    span->bridge_hello_time = SPAN_IEEE_HELLOTIME;
	    span->bridge_forward_delay = SPAN_IEEE_FORWARDDELAY;
	}
	span->bridge_aging_time = BTE_MAXAGE / ONESEC;
	span->priority = SPAN_IEEE_BRIDGE_PRIORITY;
	span->bridge_id[0] = (SPAN_IEEE_BRIDGE_PRIORITY >> 8) & 0xFF;
	span->bridge_id[1] = SPAN_IEEE_BRIDGE_PRIORITY & 0xFF;
	ieee_copy(default_mac_addr, &span->bridge_id[2]);
	span->acquire = TRUE;
	queue_init(&span->shifaQ, 0L);
	span_fixup(index, (hwidbtype *) NULL, (idbtype *)NULL);

	span->vlan_id = 0;

	/*for null termination */ 
	memset(span->domain_name, 0, VTP_DOMAIN_NAME_MAX_LENGTH); 
	span->dname_length = 0;

	if (!ibm_span_type) {
	    if (!bte_count)
		grow_stations(BTE_ALIQUOT);
	    if (!hybridge_started) {
		pid_t pid;
		/*
		 * CSCdi84012
		 * Need to create bridgeQ here to avoide race condition
		 * writing to bridgeQ while it is still uninitialized.
		 * Check for NULL to avoid problems if process creation failed.
		 */
		if (bridgeQ == NULL)
    		    bridgeQ = create_watched_queue("HyBridge Input", 0, 0);
		pid = process_create(hybridge_input, "HyBridge Input",
				     LARGE_STACK, PRIO_HIGH);
		hybridge_started = (pid != NO_PROCESS);
	    }
	}
    }
    if (startflag && !span->running) {
	span_initialize(span);
	(void) cfork((forkproc *)span_process, (long)span, 0, "Spanning Tree", 0);
	span->running = TRUE;
    }
    return(span);
}


/*
 * span_delete
 * Remove a spanning tree from the system.
 * Note that port must be set to PORT_DISABLED before we actually
 * release the data structure, otherwise bridge_receive() might crash
 * us by dereferencing a NULL spantype pointer. 
 */

void span_delete (int index)
{
    idbtype *swidb, *other_swidb;
    hwidbtype *hwidb;
    spantype *span;
    btetype *mac, *tmp;
    int i;
    leveltype level;
    
    span = spanarray[index];
    if (!span)
	return;

    if (span->irb_bg_vidb) {
	printf("\nPlease remove the BVI %d interface first!",
	       index);
	return;
    }

    FOR_ALL_SWIDBS(swidb) {
	if (swidb->span_index == index)
	    span_port_disable(span, swidb);
    }
    span_port_disable_onVidb(span, index);
    span->running = FALSE;
    process_sleep_for(ONESEC);

    /*
     * Change the address filters -- no longer bridging.
     */
    level = raise_interrupt_level(NETS_DISABLE);

    for (i = 0; i < span->port_count; i++) {

	swidb = span->port[i];
	hwidb = swidb->hwptr;

	swidb->span_index = 0;
	swidb->circ_group = 0;
	swidb->span_ptr = NULL;
	swidb->tbridge_on_swidb = FALSE;

	reg_invoke_tbridge_smf_update((uchar *)baddr,
				      swidb,
				      SMF_MACTYP_PHYS_BCAST |
				      SMF_ACTION_RECEIVE);

	/*
	 * If this was the last subinterface configured for
	 * transparent bridging, explicitly de-configure
	 * transparent bridging on the interface itself.
	 */
	FOR_ALL_SWIDBS_ON_HW(hwidb, other_swidb)
	    if (other_swidb->tbridge_on_swidb)
		break;
	if (!other_swidb) {
	    hwidb->tbridge_on_hwidb = FALSE;
	    reg_invoke_rxtypes_update(hwidb->firstsw);
	}

	if (hwidb->status & IDB_BRIDGERESET) {
	    (*hwidb->reset)(hwidb);
	    if (hwidb->counters.resets > 0)
		hwidb->counters.resets--;
	}
    }

    reset_interrupt_level(level);

    /*
     * Re-claim all station blocks.
     */
    spanarray[index] = NULL;
    for (i = 0; i < NETHASHLEN; i++) {
	mac = (btetype *) span->bridge_tbl[i];
	while (mac) {
	    tmp = mac;			/* save copy */
	    mac = mac->bte_next;		/* advance down chain */
            bridge_entry_remove(span, tmp, NULL);
	}
    }

    /*
     * Free the interface descriptors on the flood list.
     */
    while (span->floodQ.count > 0)
	free(p_dequeue(&(span->floodQ)));

    free(span);
}

/*
 * span_bandwidth_change
 * Called with the bandwidth parameter has been changed
 */

void span_bandwidth_change (idbtype *idb)
{
    spantype *span;

    if (idb->span_index) {
	span = spanarray[idb->span_index];
	if (span && !idb->path_cost_explicit) {
	    idb->path_cost = SCALED_BANDWIDTH(idb->visible_bandwidth) /
		span->path_divisor;
	    if (idb->path_cost == 0) {
		/*
		 * Could happen for ATM and so protect against it.
		 */
		idb->path_cost = 1;
	    }
	    span_fixup(idb->span_index, idb->hwptr, idb);
	}
    }
}

/*
 * span_bridge_id_addr_select
 *
 * Select a MAC address as the hardware address portion of BridgeID for 
 * this bridge group.
 * This is the MAC address of the port whose MAC address hasn't been 
 * used by other bridge groups, and has the lowest port number in the 
 * port list.
 * If there are no IEEE devices in the port list, we use msclock
 * to construct an address.
 */

void span_bridge_id_address_select (spantype *span)
{
    idbtype *port, *lowest_port = NULL;
    int count, index, offending_bridge_group = 0;
    spantype *span1;
    uchar old_mac[IEEEBYTES];

    if (!span || !span->port_count || !span->running)
        return;

    /*
     * Find the IEEE port with the lowest port number.
     */
    for (count = 0; count < span->port_count; count++) {
        port = span->port[count];
        if (port->hwptr->status & IDB_IEEEMASK) {
	    /*
	     * Find the lowest_port first. If a lowest port exist, replaced with
             * the current port when lower.
	     */
	    if ((!lowest_port) ||
		(((port->port_id) & 0xFF) < ((lowest_port->port_id) & 0xFF))) {
		/*
		 * Make sure no other running bridge group use this hardware 
		 * address as its bridgeID already.
		 * Make sure the offending_bridge_group starts with 0.
		 * Make sure that the address chosen is non-zero.
		 */
		for (index = 0, offending_bridge_group = 0; 
		     index < SPAN_MAXINDEX; index++) {
		    span1 = spanarray[index];
		    if (span == span1 || !span1 || !span1->running)
			continue;
		    if (!cmpid(port->hwptr->hardware,
			       &span1->bridge_id[2], IEEEBYTES)) {
			offending_bridge_group = span1->spanindex;
			break;
		    }
		}

		if (!offending_bridge_group &&
		    ((GETSHORT(&port->hwptr->hardware[4]) != 0) ||
		     (GETSHORT(&port->hwptr->hardware[2]) != 0) ||
		     (GETSHORT(&port->hwptr->hardware[0]) != 0)))
		    lowest_port = port;
	    }
	}
    }

    ieee_copy(&span->bridge_id[2], &old_mac[0]);    

    /*
     * Make up an address using msclock, if we haven't found one that has
     * IEEE MAC address.
     * The make-up MAC address will be 0x00000Cxxxxxx. The last three bytes
     * comes from msclock.
     */
    if (lowest_port == NULL) {

	span_random_mac_address_gen(span);

        buginf("\n\nNote: A random Spanning Tree Bridge Identifier address of %e"
	       "\n      has been chosen for Bridge Group %d since ", 
	       &span->bridge_id[2], span->spanindex);

	if (!offending_bridge_group)
	    buginf("there is no mac address"
		   "\n      associated with the selected interface.");
	else
	    buginf("the selected mac address"
		   "\n      is already being used by Bridge Group %d.", 
		   offending_bridge_group);
        buginf("\n\n      Ensure that this address is unique.\n");
    } else {
	/*
	 * We find a valid port
	 *
	 * If this port is a token-ring interface, bit-swap it since
	 * we want to use canonical format as the bridgeID. One of the
	 * reasons to do is for IRB(Integrated Routing and Bridging), 
	 * the BVI(Bridge-group Virtual Interface) need to borrow the
	 * MAC of BridgeID as its MAC address and the BVI is a 
	 * virtual-ether interface.
	 * Choosing canonical or non-canonical format as the BridgeID
	 * shouldn't have any impact of Spanning tree.
	 */
	if (is_tokenring(lowest_port->hwptr)) {
	    uchar swapped_addr[IEEEBYTES];
	    ieee_swap(lowest_port->hwptr->hardware, swapped_addr);
	    ieee_copy(swapped_addr, &span->bridge_id[2]);
	} else {
	    ieee_copy(lowest_port->hwptr->hardware, &span->bridge_id[2]);
	}
    }

    /*
     * If we change our own bridgeID, the original bridge with the original
     * suddenly disapplers, we should act like a new bridge in this bridged
     * network by believing ourselves is the root bridge and the original
     * bridge died.
     * Fail to do so will let, for example,
     * 		span->designated_root 
     *		port->designated_root
     *		port->designated_bridge
     * stay at the original bridge forever in some cases.
     * 
     * We need to do this only when we change bridgeID.
     * A new root may be elected as a result of this.
     */
    if (cmpid(&old_mac[0], &span->bridge_id[2], IEEEBYTES)) {
	/*
	 * resync bvi mac with spanning tree bridge id
	 */
	tbridge_bvi_initialize(span->irb_bg_vidb);

        bcopy(span->bridge_id, span->designated_root, 
	      sizeof(span->designated_root));
	span->root_path_cost = span->root_port_id = 0;
	span->root_port = NULL;

	for (count = 0; count < span->port_count; count++) {
	    port = span->port[count];
	    become_designated_port(span, port);
	}
    }

    return;
}

/*
 * span_random_mac_address_gen
 * This function will be called when caller trys to find 
 * a unique mac address within a bridge group as spanning
 * tree bridge id.
 * To guarantee 100% uniqueness within the router, a thorough 
 * check is applied whenever a new address is generated since 
 * we are just using part of the timestamp value we generate
 */
static void span_random_mac_address_gen (spantype *span)
{
    ulong random_value;
    uchar *prandom_value;
    uchar *addr;
    int i;
   
    if (!span) {
	return;
    }

    prandom_value = (uchar *)&random_value;

    addr = span->bridge_id;

    PUTSHORT(&addr[2], 0);

    /* 
     * Always within company ...
     */
    addr[4] = 0x0c;

    while (TRUE) {
	GET_TIMESTAMP32(random_value);

	/*
	 * use only 3 bytes
	 */
	memcpy(&addr[5], &prandom_value[1], 3);

	/*
	 * Now take a look at all the spanning tree bridge id 
	 * to see if dupliate address is there
	 */
	for (i = 0; i < SPAN_MAXINDEX; i++) {
	    /*
	     * No spanning configured for this group
	     */
	     if (!spanarray[i]) {
        	continue;
	     }

	    /*
	     * skip the same group 
	     */
	    if (span == spanarray[i]) {
		continue;
	    }

	    /*
	     * Found duplicate
	     */
	    if (ieee_equal(&addr[2], &spanarray[i]->bridge_id[2])) {
 		break;
	    }
	}
 	/*
	 * No duplicate has been found 
	 */
	if (i >= SPAN_MAXINDEX) {
	    break;
	}
    }
}


/*
 * span_fixup
 * Fixup links between interface and spanning tree data structures.
 * Called when creating a spanning tree or when changing the spanning index
 * of an interface descriptor or when changing interface encapsulation method.
 */
void span_fixup (int index, hwidbtype *hwidb, idbtype *idb)
{
    idbtype *port;
    hwidbtype *porthw;
    spantype *span;
    leveltype status;
    int count;
    boolean ibmtype;		/* SRB AST support */
    
    status = raise_interrupt_level(NETS_DISABLE);
    span = spanarray[index];
    if (!span) {
	reset_interrupt_level(status);
	return;
    }

    /* SRB AST support */

    ibmtype = (span->protocol == SPAN_PROTO_IBM);
    count = 0;
    span->port_count = 0;
    span->tr_port_count =0;
    span->fddi_port_count =0;
    FOR_ALL_SWIDBS(port) {
	porthw = port->hwptr;
	if ((porthw->status & IDB_BRIDGE) && (port->span_index == index)) {
	    if ((porthw->status & (IDB_ETHER | IDB_TUNNEL | IDB_FDDI | IDB_TR |
				  IDB_LEX)) ||
		((porthw->enctype == ET_HDLC) || is_x25(porthw) ||
		 is_smds(porthw) || is_frame_relay(porthw) || is_ppp(porthw) ||
		 is_atm(porthw) || is_atm_dxi(porthw) ||
		 is_mlapb_only(porthw) || is_cpp(porthw))) {
		span->port[count++] = port;
		span->port_count = count;
		port->span_ptr = span;

 		if(!ibmtype && hwidb && (hwidb == porthw)) {
		    reg_invoke_tbridge_smf_update((uchar *)baddr,
						  port,
						  SMF_MACTYP_PHYS_BCAST |
						  SMF_ACTION_RECEIVE);
                    /*
                     * If the interface is serial or atm type of interface,
                     * populate its SMF with the box-wide default_mac_addr to
                     * support bridge's IP host functionality since serial
                     * interface use this mac address as its src mac inside
                     * ARP packet, thus we need to listen to this mac on
                     * serial interface
                     */
                    if (porthw->status & (IDB_SERIAL | IDB_ATM)) {
                        reg_invoke_tbridge_smf_update((uchar*)default_mac_addr,
                                                      port,
                                                      SMF_MACTYP_OWN_UNICAST |
                                                      SMF_ACTION_RECEIVE);

			/*
			 * If Integrated Routing and Bridging is enabled AND
			 * the Bridge-group Virtual Interface is configured, 
			 * populate the bridged interface with the BVI's
			 * MAC address.
			 * Note that this is specifically for the serial since
			 * serial doesn't have 
			 * "reg_invoke_media_interesting_addresses()"
			 * mechanism to do this.
			 */
			if (global_irb_enable) {
			    idbtype *bvi;
			    bvi = span->irb_bg_vidb;
			    if (bvi)
				reg_invoke_bvi_smf_update(bvi->hwptr->hardware, 
							  bvi, 
							  SMF_MACTYP_BVI |
							  SMF_ACTION_RECEIVE,
							  IDB_BRIDGE);
			}
		    }

		    /*
                     * Only reset the interfaces where a reset is desired.
                     * Do not reset serial or virtual interfaces, but have
		     * to make sure that we populate SMF with the interesting
		     * MAC addresses.
                     */
                    if (porthw->status & IDB_BRIDGERESET) {
                        (*porthw->reset)(porthw);
                    } else {
                        if (is_atm(porthw) && lec_enabled(port) &&
                            (port == idb)) {
                            reg_invoke_lec_config_change(port->lane_info);
                        }
                        reg_invoke_media_interesting_addresses(port);
                    }
		    if (porthw->counters.resets > 0)
			porthw->counters.resets--;
		}
	    }
	    if (porthw->status & IDB_TR)
		span->tr_port_count++;
	    if (porthw->status & IDB_FDDI)
		span->fddi_port_count++;
	} else {
	    reg_invoke_tbridge_smf_update((uchar *)baddr,
					  port,
					  SMF_MACTYP_PHYS_BCAST |
					  SMF_ACTION_RECEIVE);
	}
    }

    /*
     * If there is a virtual bridge, add it to the port list
     */
    port = reg_invoke_srt_find_virtual_port(index);
    if (port) {
	span->port[count++] = port;
	span->port_count = count;
	span->tr_port_count++;
	port->span_ptr = span;
	/*
	 * Need to update tbridge_on_hwidb accordingly?
	 */
	port->tbridge_on_swidb = TRUE;
    }

    /*
     * If there are any CLS created TBRIDGE ports which need to be
     * linked in, add them to the port list too.  Continue until there
     * are no more CLS vidbs to add.
     */
    for (port = NULL; (port = reg_invoke_cls_find_virtual_port(index, port)); )
    {
	span->port[count++] = port;
	span->port_count = count;
	span->tr_port_count++;
	port->span_ptr = span;
    }


    /* SRB AST support */
    /*
     * If there are spanning tree virtual interfaces, add them to the
     * ports list.
     */
    reg_invoke_srt_add_spanidb(span, index, count);

    /*
     * Now organize the list of interfaces for flooding.
     */
    span_floodtable(span);
    /*
     * Reselect the MAC address of BridgeID.
     */
    span_bridge_id_address_select(span);

    /*
     * Reselect the BVI(Bridge-group Virtual Interface's MTU
     */
    if (span->irb_bg_vidb)
	tbridge_bvi_mtu_select(span->irb_bg_vidb->hwptr);

    reset_interrupt_level(status);
}

/*
 * span_fastsetup
 * Called by SERVICE_FAST_SETUP when an interface has changed
 * encapsulation or something equally major.
 */

void span_fastsetup (hwidbtype *hwidb)
{
    idbtype *swidb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	span_fixup(swidb->span_index, hwidb, swidb);
    }
}

/*
 * grow_stations
 * Grow the specified number of station data structures
 */

void grow_stations (int count)
{
    btetype *station;
    leveltype status;
    int i;
    
    station = malloc(count*sizeof(btetype));
    if (station) {
	status = raise_interrupt_level(ALL_DISABLE);
	for (i = 0; i < count; i++) {
	    enqueue(&bteQ, &station[i]);
	    bte_count++;
	    bte_total++;
	}
	reset_interrupt_level(status);
    }
}

/*
 * gc_stations
 * Scan spanning trees, reclaiming old station blocks
 * If target is NULL, scan all trees.
 */

void gc_stations (spantype *target, boolean clearflag)
{
    spantype *span;
    btetype *mac, *last, *tmp;
    leveltype status;
    int i, index;
    
    status = raise_interrupt_level(ALL_DISABLE);
    for (index = 0; index < SPAN_MAXINDEX; index++) {
	span = spanarray[index];
	if (!span || (target && (span != target)))
	    continue;
	for (i = 0; i < NETHASHLEN; i++) {
	    last = NULL;
	    mac = (btetype *) span->bridge_tbl[i];
	    while (mac) {
	      if(clearflag) {
		mac->bte_txcount = 0;
		mac->bte_rxcount = 0;
		mac->bte_auto_txcount = 0;
		mac->bte_auto_rxcount = 0;
	      }
	      if (!(mac->bte_attributes & BTE_STATIC)) {
		if (clearflag) {
		    mac->bte_txcount = 0;
		    mac->bte_rxcount = 0;
		    mac->bte_auto_txcount = 0;
		    mac->bte_auto_rxcount = 0;
		    /*
		     * CSCdi50360
		     * Dumb trick to make the bte look as though it
		     * expires within the forward delay interval.
		     * This is sort of a short aging timer.
		     * Subtract slightly less than the aging time
		     * from the current time to yield the desired
		     * time in the past.  Only do this if the aging
		     * time is larger than the forward delay.
		     * Otherwise, just let the bte age out naturally.
		     */

		    if (span->bridge_aging_time >=
			span->bridge_forward_delay) {
			GET_TIMESTAMP(mac->bte_timer);
			mac->bte_timer = TIMER_SUB_DELTA(mac->bte_timer,
				 (span->bridge_aging_time -
				  span->bridge_forward_delay) * ONESEC);
		    }
		}

		/*
		 * Collect the SP-maintained statistics on demand
		 * when a BTE appears to have expired.  Reevaluate
		 * the age of the entry afterwards.
		 */
		if (mac->bte_bce_addr &&
		    CLOCK_OUTSIDE_INTERVAL(mac->bte_timer,
					   span->bridge_aging_time * ONESEC)) {
		    reg_invoke_srt_bce_statistics(mac);
		    if (((mac->bte_rxcount + mac->bte_filtered_rxcount) !=
			 mac->bte_prev_rxcount) &&
			(mac->bte_interface) &&
			(mac->bte_interface->port_state > PORT_LISTENING)) { 
			GET_TIMESTAMP(mac->bte_timer);
			mac->bte_prev_rxcount = (mac->bte_rxcount +
						 mac->bte_filtered_rxcount);
		    }
		}

		if (target ||
		    CLOCK_OUTSIDE_INTERVAL(mac->bte_timer,
					   span->bridge_aging_time * ONESEC)) {
		    tmp = mac;
		    mac = mac->bte_next;
                    bridge_entry_remove(span, tmp, last);
		    continue;
		}
	    }
	      last = mac;
	      mac = mac->bte_next;
	  }
	}
    }
    reset_interrupt_level(status);
}

/*
 * span_clearif
 * Remove entries from the bridging data base that are assoicated with a
 * particular interface.
 */

void span_clearif (spantype *span, idbtype *port)
{
    btetype *mac, *last, *tmp;
    leveltype status;
    int i;

    status = raise_interrupt_level(NETS_DISABLE);
    for (i = 0; i < NETHASHLEN; i++) {
        mac = (btetype *) span->bridge_tbl[i];
	last = NULL;
        while (mac) {
            if (!(mac->bte_attributes) && port && port == mac->bte_interface) {
                /*
                 * Need to delink this entry and return to bteQ.
                 */
		tmp = mac;
		mac = mac->bte_next;
                bridge_entry_remove(span, tmp, last);
                continue;
            }
	    last = mac;
	    mac = mac->bte_next;
	}
    }
    reset_interrupt_level(status);
}

/*
 * span_clear_interface
 * Called to clear the spanning tree for an interface.  tbridge_adjust()
 * can't be used because the interface may still be up, but one of the map
 * entries has been deleted so the spanning tree needs to be changed.
 */

static void span_clear_interface (idbtype *idb)
{
    int index;
    spantype *span;
 
    index = idb->span_index;
    if (index) {
	span = spanarray[index];
	if (span)
	    span_clearif(span, idb);
    }
}

/*
 * tbridge_network_fixup
 * Wrapper over tbridge_adjust for reg_invoke_encap_fixup.
 */
void tbridge_network_fixup (idbtype *idb, int old_encap)
{
    return(tbridge_adjust(idb));
}

/*
 * tbridge_adjust
 * Adjust the bridging database when a bridge port goes down or up.
 * This routine MUST ONLY be called when a line state or interface
 * state transition occurs.  DO NOT call this repeatedly.
 */

void tbridge_adjust (idbtype *idb)
{
    if (!idb->span_ptr)
	return;
    if (is_subinterface(idb) ?				/* is it a subif? */
	 (idb->subif_state == SUBIF_STATE_DELETED) :	/* yes, test state */
	 (idb->hwptr->status & IDB_DELETED)) {		/* no, test status */

        if (idb->span_ptr->running)
            span_port_disable(idb->span_ptr, idb);
	span_delete_port(idb);
	return;
    }
    if (interface_up(idb)) {
	/*
	 * make_forwarding will return true if we actually change state.
	 * Don't call span_circfix unless we change state.
	 */

        /*
         * Fast STP Circuit Recovery
         * If the port isn't in blocking state, has circuit group configured
         * and forward delay timer is running, time out the forward delay
         * timer immediately to have fast circuit recovery.
         * This is for the port which was put in listening state by STP, and
         * STP checks the line status to see if it should enter forwarding
         * state upon the expiration of forward delay timer.
         */
	if (make_forwarding(idb->span_ptr, idb)) {
	    span_circfix(idb, idb->circ_group);
	} else if (idb->circ_group && idb->forward_delay_timer) {
            forward_delay_timer_expiry(idb->span_ptr, idb);
            span_circfix(idb, idb->circ_group);
        }
    } else {
	/*
	 * make_blocking will return true if we actually change state.
	 * Don't call span_circfix unless we change state.
	 */
	if (make_blocking(idb->span_ptr, idb)) {
	    span_circfix(idb, idb->circ_group);
            /*
             * Fast STP Circuit Recovery
             * If this port has circuit group configured, immediately time
             * out the configuration message in order to let other ports
             * recover fast.
             * This is for the case that a forwarding port of a non-root
             * bridge goes down and enters blocking state,
             * other ports want to recover fast instead of waiting for
             * the downed port's expiry of message age timer to recompute
             * the topology.
             */
            if (idb->circ_group && idb->message_age_timer_active) {
                idb->message_age_timer_active = FALSE;
                idb->message_age_timer = 0;
                message_age_timer_expiry(idb->span_ptr, idb);
	    }
	}
    }
}


/*
 * clear_bridgecommand
 *
 * Reset forwarding cache of bridge group, if no group
 * specified then all bridge groups are cleared !
 */

void clear_bridgecommand (parseinfo *csb)
{
int    bridge_group, index;

    spantype *span;
    tbifd_type	*flood_port;

    bridge_group = GETOBJ(int,1);    /* NULL means all Bridge Groups */

    if (bridge_group) {

        span = spanarray[bridge_group];

        if (!span) {
            printf("\n\nBridge Group %d not configured.\n", bridge_group);
            return;
        }
        else {

            gc_stations(span, TRUE);

	    for (flood_port = span->floodQ.qhead; flood_port;
		 flood_port = flood_port->tbifd_next) {
		flood_port->tbifd_rx_flood_packets = 0;
		flood_port->tbifd_tx_flood_packets = 0;
	    }
            return;
        }
    }

    for (index = 1; index < SPAN_MAXINDEX; index++) {

        span = spanarray[index];
        if (!span)
            continue;

        gc_stations(span, TRUE);

	for (flood_port = span->floodQ.qhead; flood_port;
	     flood_port = flood_port->tbifd_next) {
	    flood_port->tbifd_rx_flood_packets = 0;
	    flood_port->tbifd_tx_flood_packets = 0;
	}
    }
}


/*
 * span_delete_port
 * Function to remove an interface from a bridge group.
 */

void span_delete_port (idbtype *idb)
{
    leveltype level;
    int index;
    hwidbtype *hwidb = idb->hwptr;
    boolean ibmtype = FALSE;		/* SRB AST support */

    level = raise_interrupt_level(NETS_DISABLE);    

    if (idb->span_ptr && 
	(((spantype *)idb->span_ptr)->protocol == SPAN_PROTO_IBM)) {
	/* 
	 * If this port is running AST, don't reset the port because
	 * if you do, LNM is not going to be happy about that and close
	 * the connection.
	 */
	ibmtype = TRUE;
    }

    /* 
     * Stop all possible input by resetting the interface.
     */
    index = idb->span_index;
    /* SRB AST support */
    if (!ibmtype && (hwidb->status & IDB_BRIDGERESET)) {
	idb->span_index = 0;
	(*hwidb->reset) (hwidb);
	if (hwidb->counters.resets > 0)
	    hwidb->counters.resets--;
    }
    idb->span_index = 0;
    idb->span_ptr = NULL;
    reset_interrupt_level(level);
    span_fixup(index, hwidb, idb);
    idb->tbridge_on_swidb = FALSE;
}


/*
 * bgroup_lat - Control LAT filtering
 */

static void bgroup_lat (parseinfo *csb, idbtype *idb, uchar *mask)
{
    uchar setmask[32];
    uchar clearmask[32];

    /*
     * If there was no list at all, and no was specified, remove the
     * list.
     */

    if (!csb->sense && (GETOBJ(int,3) == 0)) {
	memset(mask, 0, 33);
	return;
    }

    if (lat_parse_grouplist(csb, setmask, clearmask))
	lat_apply_groupmask(mask, setmask, clearmask);
}

/*
 * bridgegroup_command
 * Set interface's spanning tree index.
 * Allow zero to mean no spanning tree.
 */

/*
 * These are a list of possible commands which are useful
 * for testing the parser.
 *
 * bridge-group 1 input-lat-service-deny foo
 * bridge-group 1 input-lat-service-permit bar
 * bridge-group 1 output-lat-service-deny ack
 * bridge-group 1 output-lat-service-permit not
 * [no] bridge-group 1
 * [no] bridge-group 1 circuit 1
 * [no] bridge-group 1 input-address-list 700
 * [no] bridge-group 1 input-lsap-list 200
 * [no] bridge-group 1 input-type-list 250
 * [no] bridge-group 1 input-pattern-list 1100
 * [no] bridge-group 1 lat-compression
 * [no] bridge-group 1 output-address-list 799
 * [no] bridge-group 1 output-lsap-list 202
 * [no] bridge-group 1 output-type-list 252
 * [no] bridge-group 1 output-pattern-list 1100
 * [no] bridge-group 1 path-cost 11
 * [no] bridge-group 1 priority 123
 * [no] bridge-group 1 spanning-disabled
 * [no] bridge-group 1 sse
 */


void bridgegroup_command (parseinfo *csb)
{
    spantype *span;
    idbtype *idb, *swidb;
    hwidbtype *hwidb;
    int index, circgrp;
    macaddracctype *item;
    ushort new_port_id = 0;

    idb = csb->interface;
    hwidb = idb->hwptr;

    if (csb->nvgen) {

	index = idb->span_index;
	if ((!index) || (index != GETOBJ(int,1))) {
	    return;
	}

	if (((hwidb->status & IDB_TR) && !idb->span_ptr) ||
	    (((spantype *)idb->span_ptr)->protocol == SPAN_PROTO_IBM)) {
	    /* 
	     * This TR interface is configured with either static ST or AST.
	     */
	    return;
	}

	switch (csb->which) {

	  case BGROUP_ASSIGN:
	    nv_write(idb->span_index, "%s", csb->nv_command);
	    break;

	  case BGROUP_CIRCUIT:
	    nv_write(idb->circ_group, "%s %d", csb->nv_command,
		     idb->circ_group);
	    break;

	  case BGROUP_INADDRESS:
	    nv_write(idb->span_address_in, "%s %d", 
		     csb->nv_command, idb->span_address_in);
	    break;

	  case BGROUP_PATHCOST:
	    nv_write(idb->path_cost_explicit, "%s %d", 
		     csb->nv_command, idb->path_cost);
	    break;

	  case BGROUP_INLSAP:
	    nv_write(idb->span_lsap_in, "%s %d",
		     csb->nv_command, idb->span_lsap_in);
	    break;

	  case BGROUP_INTYPE:
	    nv_write(idb->span_type_in, "%s %d",
		     csb->nv_command, idb->span_type_in);
	    break;

	  case BGROUP_LAT:
	    nv_write(idb->span_latcompress, "%s", csb->nv_command);
	    break;

	  case BGROUP_LATSERVICE:
	    switch (GETOBJ(int,2)) {
	      case BGROUP_INLAT_DENY:
		if (idb->span_lat_in_svc_deny[0]) {
		    nv_write(TRUE, "%s", csb->nv_command);
		    nv_lat_groupmask(NULL, idb->span_lat_in_svc_deny,
				     TRUE, FALSE);
		}
		break;

	      case BGROUP_INLAT_PERMIT:
		if (idb->span_lat_in_svc_permit[0]) {
		    nv_write(TRUE, "%s", csb->nv_command);
		    nv_lat_groupmask(NULL, idb->span_lat_in_svc_permit, TRUE, FALSE);
		}
		break;

	      case BGROUP_OUTLAT_DENY:
		if (idb->span_lat_out_svc_deny[0]) {
		    nv_write(TRUE, "%s", csb->nv_command);
		    nv_lat_groupmask(NULL, idb->span_lat_out_svc_deny, TRUE, FALSE);
		}
		break;

	      case BGROUP_OUTLAT_PERMIT:
		if (idb->span_lat_out_svc_permit[0]) {
		    nv_write(TRUE, "%s", csb->nv_command);
		    nv_lat_groupmask(NULL, idb->span_lat_out_svc_permit, TRUE, FALSE);
		}
		break;
	    }
	    break;

	  case BGROUP_OUTADDRESS:
	    nv_write(idb->span_address_out, "%s %d",
		     csb->nv_command, idb->span_address_out);
	    break;

	  case BGROUP_OUTLSAP:
	    nv_write(idb->span_lsap_out, "%s %d",
		     csb->nv_command, idb->span_lsap_out);
	    break;

	  case BGROUP_OUTTYPE:
	    nv_write(idb->span_type_out, "%s %d",
		     csb->nv_command, idb->span_type_out);
	    break;

	  case BGROUP_PRIORITY:
	    /*
	     * CSCdi12635
	     * Change the default port priority value from 0 to
	     * SPAN_IEEE_PORT_PRIORITY(128)
	     * SPAN_DEC_PORT_PRIORITY = SPAN_IEEE_PORT_PRIORITY
	     */
	    nv_write(idb->port_id >> 8 != SPAN_IEEE_PORT_PRIORITY, 
		     "%s %d", csb->nv_command, (idb->port_id >> 8));
	    break;

	  case BGROUP_SPANDISABLE:
	    nv_write(idb->span_disabled, "%s", csb->nv_command);
	    break;

	  case BGROUP_INPATTERN:
	    nv_write(idb->span_pattern_in, "%s %d",
		     csb->nv_command, idb->span_pattern_in);
	    break;

	  case BGROUP_OUTPATTERN:
	    nv_write(idb->span_pattern_out, "%s %d",
		     csb->nv_command, idb->span_pattern_out);
	    break;

	  case BGROUP_CBUSBRIDGE:
	    nv_write(idb->hwptr->cbus_bridge_config, "%s",
		     csb->nv_command);
	    break;

	  default:
	    break;
	}
	return;
    }
    
    if (!bridge_enable) {
	printf("\nBridging%s", notsupp);
	return;
    }
    
    if ((hwidb->status & IDB_BRIDGE) == 0) {
	printf(nobridging, idb->namestring);
	return;
    }

    /*
     * Get index and make sure that it is in range.
     * Do not allow index to be changed to other than zero.
     */
    index = GETOBJ(int,1);

    if ((index) && ((index < 1) || (index >= SPAN_MAXINDEX))) {
	printf(badspan);
	return;
    }

    if (idb->span_index && index && (index != idb->span_index)) {
	printf("\n\nInterface already configured within Bridge Group %d.\n",
	       idb->span_index);
	return;
    }

    span = span_lookup(index, FALSE, 0);
    if (!span) {
	printf(nomemory);
	return;
    }
    
    if (span->protocol == SPAN_PROTO_IBM) {
	printf("\nBridge %d is running AST.", index);
	printf("\nBridge group command will not be accepted.");
	return;
    }

    /*
     * If attempting to configure bridging on a LAN or HDLC serial
     * Subinterface then the interface must already be configured for
     * IEEE 802.10 SDE encapsulation or Inter Switch Link (ISL) vLAN
     * encapsulation.
     *
     * Perform the necessary checks, based upon the subinterface's
     * appropriate vLAN flag being set via the encapsulation command.
     *
     * Note a subinterface can only support one kind of vLAN interface.
     */

    if (is_sde_capable(idb) || is_isl_vlan_capable(idb)) {

        if (IS_DOT10_VLAN_SUBINTERFACE(idb)) {

	    if (dot10_capable_subinterface(idb, span, index) == FALSE)
	    return;

        } else if (IS_ISL_VLAN_SUBINTERFACE(idb)) {

            if (reg_invoke_isl_vlan_bridging_capable_subinterface(idb, index,
							        span) == FALSE)
            return;

        } else {

           printf("\n\nBridging on a LAN or HDLC subinterface requires that the"
                  " interface be already\nconfigured for 802.10 SDE"
                  " encapsulation or Inter Switch Link (ISL) encapsulation\n");
           return;
       }
    }

    switch (csb->which) {

    case BGROUP_ASSIGN:

	if (!csb->sense) {

	    /*
	     * Explicitly de-configure transparent bridging
	     * on the sub-interface.
	     */
	    idb->tbridge_on_swidb = FALSE;
	    idb->tbridge_route_linktypes = 0x0;

	    /*
	     * If this was the last sub-interface configured for
	     * transparent bridging, explicitly de-configure
	     * transparent bridging on the interface itself.
	     */
	    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb)
		if (swidb->tbridge_on_swidb)
		    break;
	    if (!swidb) {
		hwidb->tbridge_on_hwidb = FALSE;
	        idb->hwptr->bridge_transit = FALSE;	
                reg_invoke_rxtypes_update(hwidb->firstsw);
	    }
	    /*
	     * Remove this interface from the bridge group.
	     */
	    if (span->running)
		span_port_disable(span, idb);
	    span_delete_port(idb);
	    /*
	     * Make sure that cbus-bridging is explicitly
	     * deconfigured here otherwise the configuration
	     * check function will re-configure it.
	     */
	    idb->hwptr->cbus_bridge_config = FALSE;

	    if (idb->circ_group) {
		circgrp = idb->circ_group;
		idb->circ_group = 0;
		span_circfix(idb, circgrp);
	    }
	    reg_invoke_media_tbridge_swidb_vc_queue_clear(ET_ATM, idb);
	    if (is_atm_lane(idb)) {
		reg_invoke_lec_config_change(idb->lane_info);
	    }

	    reg_invoke_srt_report_port_removal(idb, index);

	} else {
	    /*
	     * Add new interface to the bridge group. First, if token ring,
	     * verify the transit OUI has been specified
	     */
	    if ((hwidb->status & IDB_TR) &&
		!(hwidb->tr_transit_oui)) {
	        printf("\nYou must specify a valid \"ethernet-transit-oui\" command");
		printf("\nbefore enabling Token Ring interfaces for Transparent Bridging.");
		printf("\n(This interface is %s)", idb->namestring);
		return;
	    }
            if (is_slapb_only(hwidb)) {
                printf("\nBridging is not supported on"
		       " LAPB-encapsulated interfaces: %s", idb->namestring); 
                return;
            }

	    /*
	     * Here is a vLAN specific check: we can't configure
	     * native interface and vLAN subinterface within the
	     * same bridge group, because if we do so, 
	     * broadcast/unknown-unicast packets received from 
	     * native interface would be flooded to vLAN
	     * subinterface, and bridged back to the router in
	     * the other end, this would corrupt its bridge table
	     * CSCdi59321.
	     */ 

	    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	        if (IS_VLAN_CONFIGURED_SUBINTERFACE(swidb) 
		    &&(((idb == swidb)
			&& (hwidb->firstsw)
			&& (hwidb->firstsw->tbridge_on_swidb)
			&& (index == hwidb->firstsw->span_index)) 
		       ||((idb == hwidb->firstsw)
			  && (swidb->tbridge_on_swidb)		      
			  && (index == swidb->span_index)))){

		    printf("\n\nConfiguration of vLAN subinterfaces and main"
			   "\ninterface within the same bridge group is not permitted");
		    return;
		}
	    }	      
	    
	    if (idb->span_index != index) {
		idb->span_index = index;

		/*
		 * Explicitly configure transparent bridging
		 * on this subinterface, and indicate that
		 * transparent bridging is configured on at
		 * least one subinterface on this interface.
		 */
		idb->tbridge_on_swidb = TRUE;
		idb->tbridge_route_linktypes = span->route_linktypes;
		hwidb->tbridge_on_hwidb = TRUE;
		reg_invoke_rxtypes_update(idb);
		reg_invoke_srt_report_new_port(idb, index);

		/*
		 * CSCdi12635
		 * Change the default port priority value from 0 to
		 * SPAN_IEEE_PORT_PRIORITY(128)
		 * SPAN_DEC_PORT_PRIORITY = SPAN_IEEE_PORT_PRIORITY
		 */
		idb->port_id = ((SPAN_IEEE_PORT_PRIORITY << 8) |
				(idb->if_number & 0xFF));
		idb->number_bit_mask = (1 << (idb->if_number % 32));
		idb->bit_mask_word = idb->if_number / 32;
		idb->path_cost = SCALED_BANDWIDTH(idb->visible_bandwidth) /
		    span->path_divisor;
		if (idb->path_cost == 0) {
		    idb->path_cost = span->protocol == SPAN_PROTO_DEC ?
		                     SPAN_DEC_DEFAULT_PATHCOST  :
				     SPAN_IEEE_DEFAULT_PATHCOST ;
		}
		span_fixup(index, hwidb, idb);
		if (span->running) {
		    span_port_enable(span, idb);
		}
		if ((hwidb->status & (IDB_SERIAL|IDB_TUNNEL|IDB_LEX)) ||
		    ((hwidb->status & IDB_FDDI) &&
		     (hwidb->fddi_options & FDDI_ENCAPSULATE))) {
		    hwidb->bridge_transit = TRUE;
		}
		if ((is_frame_relay(hwidb)) &&
		     (is_p2p(idb))) {
		    reg_invoke_media_tbridge_swidb_vc_queue_init(
			 ET_FRAME_RELAY, idb);
		} else if (is_atm(hwidb)) {
		    /*
		     * now put all the vc's associated w/ this sw
		     * interface in a linked list of vc's.  this is to
		     * make flooding easier.Applies to AAL5SNAP encap only.
		     */
		    reg_invoke_media_tbridge_swidb_vc_queue_init(ET_ATM,
								     idb);
		    hwidb->bridge_transit = TRUE;
                    /*
		     * Set up SMF filter here for ATM interfaces
		     */
		    reg_invoke_tbridge_smf_update(hwidb->hardware,
						  idb,
						  SMF_MACTYP_OWN_UNICAST |
						  SMF_ACTION_RECEIVE);
		}
	    }
	}
	break;

    case BGROUP_CIRCUIT:
	/*
	 * do not support circuit groups over Frame Relay and X25
	 */
	if (is_frame_relay(hwidb) || is_x25(hwidb) || is_atm_dxi(hwidb)) {
	    printf("\n%%Circuit groups not allowed over Frame-Relay and X25");
	    return;
	}
	if (!idb->tbridge_on_swidb) {
	    printf("\nPlease configure bridging on this interface first");
	    return;
	}
	bgroup_circuit(csb, span, idb);
	break;
    case BGROUP_INADDRESS:
	item = mac_get_std_acl(idb->span_address_in);
	if (item) {
	    
	    /*
	     * If a access list currently exists for the interface,
	     * clear all station bits before we use the NEW list.
	     */
	    mac_set_std_acl(idb->span_address_in, NULL);
	    set_station_bits(idb->span_address_in);
	    mac_set_std_acl(idb->span_address_in, item);
	}
	idb->span_address_in = GETOBJ(int,2);
	set_station_bits(idb->span_address_in);
	break;
    case BGROUP_LATSERVICE:
	switch (GETOBJ(int,2)) {
	case BGROUP_INLAT_DENY:
	    bgroup_lat(csb, idb, idb->span_lat_in_svc_deny);
	    break;
	case BGROUP_INLAT_PERMIT:
	    bgroup_lat(csb, idb, idb->span_lat_in_svc_permit);
	    break;
	case BGROUP_OUTLAT_DENY:
	    bgroup_lat(csb, idb, idb->span_lat_out_svc_deny);
	    break;
	case BGROUP_OUTLAT_PERMIT:
	    bgroup_lat(csb, idb, idb->span_lat_out_svc_permit);
	    break;
	}
	break;
    case BGROUP_INLSAP:
	bgroup_list(csb, idb, &idb->span_lsap_in);
	break;
    case BGROUP_INTYPE:
	bgroup_list(csb, idb, &idb->span_type_in);
	break;
    case BGROUP_LAT:
	if (hwidb->status & (IDB_SERIAL|IDB_TUNNEL)) {
	    idb->span_latcompress = csb->sense;
	} else {
	    printf("\nLAT compression only on serial interfaces");
	}
	break;
    case BGROUP_OUTADDRESS:
	item = mac_get_std_acl(idb->span_address_out);
	if (item) {
	    
	    /*
	     * If a access list currently exists for the interface,
	     * clear all station bits before we use the NEW list.
	     */
	    mac_set_std_acl(idb->span_address_out, NULL);
	    set_station_bits(idb->span_address_out);
	    mac_set_std_acl(idb->span_address_out, item);
	}
	idb->span_address_out = GETOBJ(int,2);
	set_station_bits(idb->span_address_out);
	break;
	break;
    case BGROUP_OUTLSAP:
	bgroup_list(csb, idb, &idb->span_lsap_out);
	break;
    case BGROUP_OUTTYPE:
	bgroup_list(csb, idb, &idb->span_type_out);
	break;
    case BGROUP_INPATTERN:
	if (csb->sense) {
	    idb->span_pattern_in = GETOBJ(int,2);
	} else {
	    idb->span_pattern_in = 0;
	}
        break;
    case BGROUP_OUTPATTERN:
	if (csb->sense) {
	    idb->span_pattern_out = GETOBJ(int,2);
	} else {
	    idb->span_pattern_out = 0;
	}
	break;
    case BGROUP_PATHCOST:
	{
	    int cost;
	    
	    if (!csb->sense) {
		cost = SCALED_BANDWIDTH(idb->visible_bandwidth) /
		    span->path_divisor;
	    } else {
		cost = GETOBJ(int,2);
	    }
	    idb->path_cost_explicit = csb->sense;
	    if (cost != 0) {
		idb->path_cost = cost;
		if (span->running) {
		    span_path_cost(span);
		}
	    } else {
                /* 
		 * add a default value.
		 */
		idb->path_cost = 1;
	    }
	}
	break;
    case BGROUP_PRIORITY:
	/*
         * CSCdi12635
         * Change the default port priority value from 0 to
         * SPAN_IEEE_PORT_PRIORITY(128)
         * SPAN_DEC_PORT_PRIORITY = SPAN_IEEE_PORT_PRIORITY
	 * Correct the handling of 'no bridge priority' to restore the
	 * default rather than zero.
         */
        if (csb->sense) {
	    new_port_id = (GETOBJ(int,2) << 8) | (idb->port_id & 0xFF);
	} else {
	    new_port_id = (SPAN_IEEE_PORT_PRIORITY << 8) | (idb->port_id & 0xFF);
	}
	
	if (span->running) {
	    span_port_priority(span, idb, new_port_id);
	} else {
	    idb->port_id = new_port_id;
	}
	break;
    case BGROUP_SPANDISABLE:
	idb->span_disabled = csb->sense;
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }

    reg_invoke_srt_config_check(idb, span);
}

/*
 * bgroup_list
 * Common code to set up a type/LSAP list
 */

void bgroup_list (parseinfo *csb, idbtype *idb, int *list)
{
    *list = GETOBJ(int,2);

    /*
     * Recalculate flags that indicate if any input/output checking is done.
     * These flags save an extra check at interrupt level
     */
    idb->span_flag_in = (idb->span_lsap_in | idb->span_type_in);
    idb->span_flag_out = (idb->span_lsap_out | idb->span_type_out);
}

/*
 * bgroup_circuit
 * Set up circuit group data structures
 */

void bgroup_circuit (parseinfo *csb, spantype *span, idbtype *idb)
{
    int oldindex, index;

    if (csb->sense == FALSE) {
	if (idb->circ_group) {
	    oldindex = idb->circ_group;
            idb->circ_group = 0;
            span_circfix(idb, oldindex);
	}
    } else {
	index = GETOBJ(int,2);
	if ((span->cct_groups[index].ccg_avail_idb_count +
	     span->cct_groups[index].ccg_unavail_idb_count) >= CCG_MAXCIRCUITS){
	    printf(exceedmaxcircidb);
	    return;
	}
	oldindex = idb->circ_group;
        idb->circ_group = index;
	if (oldindex && (oldindex != idb->circ_group))
	    span_circfix(idb, oldindex);
	span_circfix(idb, idb->circ_group);
    }
}

/*
 * span_circfix
 *
 * An interface has changed availability (up/down/add/delete) or
 * Spanning tree port state changes.
 * Start pause timer.
 * Recompute the associated circuit group information.
 */

void span_circfix (idbtype *port, int group)
{
    idbtype *idb;
    leveltype level;
    spantype *span;
    int index, avail_count = 0, unavail_count = 0;

    span = port->span_ptr;
    if (!span || !group)
	return;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Don't have to clear ccg_idb_count and start pause timer if 
     * ccg_pause_timer is zero since we don't want to pause tx.
     */
    if (span->cct_groups[group].ccg_pause_interval) {
	span->cct_groups[group].ccg_idb_count = 0;
	TIMER_START(span->cct_groups[group].ccg_pause_timer, 
		    span->cct_groups[group].ccg_pause_interval);
    }

    span->cct_groups[group].ccg_avail_idb_count = 0;
    span->cct_groups[group].ccg_unavail_idb_count = 0;

    for (index = 0; index < CCG_MAXCIRCUITS; index++)
	span->cct_groups[group].idb_array[index] = NULL;

    FOR_ALL_SWIDBS(idb) {
	if (idb->circ_group == group) {
	    if (!interface_up(idb)) {
                idb->circ_inserted = FALSE;
		unavail_count++;
		span->cct_groups[group].idb_array[CCG_MAXCIRCUITS - 
						  unavail_count] = idb;
                continue;
	    }
            idb->circ_inserted = TRUE;
	    span->cct_groups[group].idb_array[avail_count] = idb;
	    avail_count++;
	}
    }

    span->cct_groups[group].ccg_unavail_idb_count = unavail_count;
    span->cct_groups[group].ccg_avail_idb_count = avail_count;    

    /*
     * Update ccg_idb_count if ccg_pause_timer is zero since we didn't start
     * pause timer and hence won't call ccg_pause_timer_expiry() to
     * restore the correct ccg_idb_count.
     */
    if (!span->cct_groups[group].ccg_pause_interval)
	span->cct_groups[group].ccg_idb_count = avail_count;

    span_clearif(span, port);		/* clear forwarding database */
    span_circstate(port);		/* get correct spanning status */
    span_floodtable(span);		/* recompute flooding list */
    reset_interrupt_level(level);
}

/*
 * span_circuit_remove
 * Remove an interface from a circuit group
 * Used by protocols such as FR and X.25 that don't support circuit groups.
 */

static void span_circuit_remove (idbtype *swidb)
{
    int oldindex;

    oldindex = swidb->circ_group;
    if (!oldindex)
	return;
    swidb->circ_group = 0;
    span_circfix(swidb, oldindex);
}

/*
 * span_circstate
 * Called whenever a port changes spanning state to determine if any member
 * of the circuit group can learn new addresses or forward datagrams.  These
 * are the variables looked at by the interrupt level forwarding code.
 */

void span_circstate (idbtype *port)
{
    idbtype *idb;
    boolean forward, learn;
    spantype *span;
    int	count, i;

    span = port->span_ptr;
    if (!span)
	return;

    /*
     * Set circ_canforward and circ_canlearn accordingly.
     * They are used by any routine to check if this port should
     * forward and learn - not just for circuit group.
     */
    port->circ_canforward =
	(port->port_state == PORT_FORWARDING) ? TRUE : FALSE;
    port->circ_canlearn = (port->port_state >= PORT_LEARNING) ? TRUE : FALSE;

    /*
     * All done for interfaces not in circuit groups.
     */
    if (port->circ_group == 0)
	return;

    /*
     * See if anyone can forward or can learn new addresses.
     */
    forward = FALSE;
    learn = FALSE;
    count = span->cct_groups[port->circ_group].ccg_avail_idb_count;
    for(i = 0; i < count; i++) {
	idb = span->cct_groups[port->circ_group].idb_array[i];
	if (idb->port_state == PORT_FORWARDING)
	    forward = TRUE;
	if (idb->port_state >= PORT_LEARNING)
	    learn = TRUE;
    }

    /*
     * Set all members of the circuit group to the same state
     */
    for(i = 0; i < count; i++) {
	idb = span->cct_groups[port->circ_group].idb_array[i];
	idb->circ_canforward = forward;
	idb->circ_canlearn = learn;
    }
}

/*
 * tbridge_check_cbus_copy_capable()
 *
 * For cbus assistance copy for flooding, it requires,
 * (1) 7xxx box because of ucode support.
 * (2) All interfaces in this bridge group are cbus interfaces thus
 * 	we don't have to copy the packet to a network buffer.
 * (3) No autonomous bridging configured on any interface of this
 *	group because AB always flushes the packet afterwards and
 *	we have on control of that.
 *
 * Set the span->cbus_copy_capable to TRUE if cbus copy assistance
 * for flooding is enabled.
 */

void tbridge_check_cbus_copy_capable (spantype *span)
{
    idbtype *port;
    int index;

    span->cbus_copy_capable = FALSE;

    /* 
     * Check if this box is 7xxx.
     */
    if (!IS_FAMILY(FAMILY_RP1))
	return;

    for (index = 0; index < span->port_count; index++) {
        port = span->port[index];
        if(!(port->hwptr->status & IDB_CBUS) || 
	   port->hwptr->cbus_bridge_enable)
            return;
    }

    span->cbus_copy_capable = TRUE;
}

/*
 * span_floodtable
 * Construct the spanning tree's list of interfaces for flooding.
 * There is one entry (interface pointer) for each circuit group.
 * Skip over down (!port->circ_inserted) interfaces that belong to
 * a user defined circuit group.
 */

void span_floodtable (spantype *span)
{
    idbtype *port;
    tbifd_type *oif;
    boolean inlist;
    int group, i;

    /*
     * Free the interface descriptors on the existing floodlist.
     */
    while (span->floodQ.count > 0)
	free(p_dequeue(&(span->floodQ)));

    /*
     * Free all multicast group data structures.
     */
    reg_invoke_tbridge_cmf_clear(span);

    for (i = 0; i < span->port_count; i++) {
	port = span->port[i];
	inlist = FALSE;

	/*
	 * Check if the current interface is already in the flooding list.
	 * We go by circuit group number.  If the interface is a member of
	 * a user defined circuit group and is not inserted (available),
	 * do not enter it into the flooding table.
	 */
	group = port->circ_group;
	if (group && !port->circ_inserted)
	    continue;
	if (group) {
	    for (oif = span->floodQ.qhead; oif; oif = oif->tbifd_next) {
		if (oif->tbifd_swidb->circ_group == group) {
		    inlist = TRUE;
		    break;
		}
	    }
	}
	if (inlist)
	    continue;

	/*
	 * Circuit group not in the flooding list, add this interface
	 */
	oif = malloc_named(sizeof(tbifd_type), "TB IFD flood list");
	if (!oif)
	    return;
	oif->tbifd_swidb = span->port[i];
	oif->tbifd_attributes = TBIFD_FLOOD;
	enqueue(&(span->floodQ), oif);
    }
    /*
     * Membership of flooding list may have been changed; 
     * Reevaluate the boolean for cbus copy assistance for flooding.
     */
    tbridge_check_cbus_copy_capable(span);
}

/*
 * show_lat_service_filters - Display LAT service filters
 */

static void show_lat_service_filter (
     char *direction,
     char *type,
     uchar *filter)
{
    printf("\n      %sput LAT service %s group code list", direction, type);
    if (!filter[0]) {
	printf(" is not set");
	return;
    }
    printf(":\n     ");
    print_lat_groups(filter, 5, 16, TRUE);
}


/*
 * show_span
 *
 * Display Spanning Tree data structures.
 *
 */

void show_span (parseinfo *csb)
{

spantype *span;
idbtype  *idb;
int i,   index, group;
boolean  onceflag = FALSE, ibm_span_type;		/* SRB AST support */


    automore_enable(NULL);

    group = GETOBJ(int,1);

    /*
     * If no group number we'll display all groups.
     * Parser MACRO will only give us a valid index.
     */

    if (group && (spanarray[group] == NULL)) {

        printf("\n\nNo Spanning Tree configured for Bridge Group %d.\n",
	       group);
        return;
    }

    for (index = 1; index < SPAN_MAXINDEX; index++) {

        if ((group) && (group != index))
            continue;

	span = spanarray[index];
	if ((!span) || (!span->running))
	    continue;

	ibm_span_type = (span->protocol == SPAN_PROTO_IBM);

	if (onceflag == TRUE) {

            printf("\n");
            automore_conditional(0);
	}

	onceflag = TRUE;
	    
	printf("\n\nBridge Group %d is executing the %s compatible Spanning"
	       " Tree protocol", index, stype2string(span));

	if ((span->protocol == SPAN_PROTO_IEEE) && (span->domain)) 
            printf("\n  The IEEE bridge domain for this group is %d",
                   (span->domain >> 4));

	printf("\n  Bridge Identifier has priority %d, address %e",
	       PRIORITY(span->bridge_id), &span->bridge_id[2]);

        printf("\n  Configured hello time %d, max age %d, forward delay %d",
	       span->bridge_hello_time, span->bridge_max_age,
	       span->bridge_forward_delay);

	if (root_bridge(span))
	    printf("\n  We are the root of the spanning tree");

	else {
	    printf("\n  Current root has priority %d, address %e",
		   PRIORITY(span->designated_root),&span->designated_root[2]);

	    /* SRB AST support */

	    if (ibm_span_type) {
	        printf("\n  Root port is %-04x (%s), cost of root path is %d",
		       span->root_port_id, span->root_port->namestring,
		       span->root_path_cost);
	    } else {
	        printf("\n  Root port is %d (%s), cost of root path is %d",
		       (span->root_port_id & 0xFF), span->root_port->namestring,
		       span->root_path_cost);
	    }
	}

        printf("\n  Topology change flag %sset, detected flag %sset",
	       span->topology_change ? NULL : "not ",
               span->topology_change_detected ? NULL : "not ");

        printf("\n  Times:  hold %d, topology change %d, notification %d",
	       span->hold_time, span->topology_change_time,
               span->topology_change_notification_time);

        printf("\n          hello %d, max age %d, forward delay %d, aging %d",
               span->hello_time, span->max_age, span->forward_delay,
               span->bridge_aging_time);

        printf("\n  Timers: hello %d, topology change %d, notification %d\n",
               span->hello_timer, span->topology_change_timer, 
               span->topology_change_notification_timer);

	for (i = 0; i < span->port_count; i++) {

	    if (i)
	        printf("\n");
                automore_conditional(0);

	    idb = span->port[i];

	    /* SRB AST support */

	    if (ibm_span_type) {
	        printf("\nPort %-04x (%s) of bridge group %d is %s",
                       idb->port_id, idb->namestring, index,
		       (interface_up(idb) ?  bstate2string(idb->port_state) : 
		       "down"));

		if (idb->span_disabled) 
		    printf("\n   Transmission of BPDUs is disabled");  

                printf("\n   Port path cost %d, Port priority %u",
                       idb->path_cost, (idb->ibm_port_priority));
	    } else {
		char *str;

		str = tbridge_encap_string(idb);

		if (str) {
		    printf("\nPort %d (%s %s) of bridge group %d is %s",
			   (idb->port_id & 0xFF), idb->namestring, str, index,
			   (interface_up(idb) ?
			    bstate2string(idb->port_state) : "down"));
		} else {
		    printf("\nPort %d (%s) of bridge group %d is %s",
			   (idb->port_id & 0xFF), idb->namestring, index,
			   (interface_up(idb) ?
			    bstate2string(idb->port_state) : "down"));
		}

		if (idb->span_disabled) 
		    printf("\n   Transmission of BPDUs is disabled");  

                printf("\n   Port path cost %d, Port priority %u",
                       idb->path_cost, (idb->port_id >> 8));
	    }
	    printf("\n   Designated root has priority %d, address %e",
		   PRIORITY(idb->designated_root), &idb->designated_root[2]);

	    printf("\n   Designated bridge has priority %d, address %e",
		   PRIORITY(idb->designated_bridge), 
		   &idb->designated_bridge[2]);

	    /* SRB AST support */

	    if (ibm_span_type) {
	        printf("\n   Designated port is %-04x, path cost %d, peer %d",
		       idb->designated_port, idb->designated_cost, 
		       idb->disjoint_peers);
	    } else {
	        printf("\n   Designated port is %d, path cost %d",
		       (idb->designated_port & 0xFF), idb->designated_cost);
	    }

	    if (idb->config_pending)
		printf("   Hello is pending");

	    if (idb->topology_change_acknowledge)
		printf(", Topology change is set");

            printf("\n   Timers: message age %d, forward delay %d, hold %d",
                   idb->message_age_timer, idb->forward_delay_timer,
		   idb->hold_timer);
	}
    }

    printf("\n");
    automore_disable();
}


/*
 * show_bridge_table
 *
 * Display the transparent bridging forwarding/filtering table.
 */

static const char bhead_verbose[] = "BG Hash      Address      Action  "
	                     "Interface      VC    Age   RX count   TX count";

static const char bhead[] = "    Address       Action   Interface       Age   "
                     "RX count   TX count";

void show_bridge_table (parseinfo *csb)
{

int              i, index, minimum, depth;
boolean          onceflag;
spantype         *span;
btetype          *mac;
int              group = 0;
idbtype          *swidb = NULL;
boolean          verbose = FALSE, mac_option = FALSE;
mac_address_type address, address_mask;

    if (!bridge_enable) {
	printf("Bridging%s", notsupp);
	return;
    }

    group = GETOBJ(int, 1);                  /* no group means all groups */

    /*
     * Check for the "show bridge table <verbose>" extension.
     * This does what the old command "show bridge" used to do.
     */

    if (group && (spanarray[group] == NULL)) {
        return;
    }

    verbose = GETOBJ(int,2);

    swidb = GETOBJ(idb, 1);   /* NULL software IDB if no interface specified */

    if (swidb == NULL) {

    /*
     * Verify that the mac address and mask have non-zero length
     * before using their values.  I'm making the assumption that
     * a non-zero length means the values were parsed from the
     * command line.
     */

        if (GETOBJ(hwaddr,1)->length) {
            mac_option = TRUE;
            ieee_copy(GETOBJ(hwaddr,1)->addr, address.u.byte);
            if (GETOBJ(hwaddr,2)->length) {
                ieee_copy(GETOBJ(hwaddr,2)->addr,
	                  address_mask.u.byte);
             } else {
                address_mask.u.lword[0] = 0;
                address_mask.u.sword[2] = 0;
            }
        }
    }

    automore_enable(NULL);

    /* 
     * Bail out if no Bridge Groups configured.
     */

    for (index = 1; index < SPAN_MAXINDEX; index++) {

        if ((spanarray[index] != NULL) && (spanarray[index]->running))
	    break;

	else if (index == (SPAN_MAXINDEX - 1)) {
            return;
        }
    }

    /*
     * Summarize hardware capabilities of this system
     */

    reg_invoke_srt_status_general();

    printf("\n\nTotal of %d station blocks, %d free\nCodes: "
	   "P - permanent, S - self\n", bte_total, bte_count);

    for (index = 1, onceflag = FALSE; index < SPAN_MAXINDEX; index++) {

	if ((group) && (group != index))
	    continue;

	span = spanarray[index];

	if ((!span) || (!span->running))
	    continue;

	if (!verbose)
	    printf("\nBridge Group %d:\n", index);

	/*
	 * Summarize hardware specific details of this bridge group.
	 */
	reg_invoke_srt_status_bgroup(span);

	/*
	 * Now display each bridge cache entry
	 */

	for (i = 0; i < NETHASHLEN; i++) {

	    depth = 0;
	    for (mac = span->bridge_tbl[i]; mac; mac = mac->bte_next, depth++) {
		if ((swidb) && (swidb != mac->bte_interface))
		    continue;

		if (mac_option) {
		    if (((mac->bte_mac_addr.lword[0] 
			  & (~address_mask.u.lword[0])) != address.u.lword[0])
			  || ((mac->bte_mac_addr.sword[2]
			  & (~address_mask.u.sword[2])) != address.u.sword[2]))
			continue;
		}

                if (!onceflag) {
		    if (verbose)
                        automore_header(bhead_verbose);
		    else
			automore_header(bhead);
                    onceflag = TRUE;
		}

		if (verbose) {

		    printf("\n%-2d %-02x/%3d %e %7s  %15s", index, i, depth,
		           &mac->bte_mac_addr.sword[0],
			   baction2string(mac->bte_action),
		           (mac->bte_interface) ? 
			   ((IS_VLAN_CONFIGURED_SUBINTERFACE(mac->bte_interface)
			     || (mac->bte_interface->hwptr->type == 
				 IDBTYPE_VACCESS))?
			    mac->bte_interface->short_namestring:
			    mac->bte_interface->namestring) : "    -    ");

		    if ((is_frame_relay(hwidb_or_null(mac->bte_interface)) ||
			 (is_atm_dxi(hwidb_or_null(mac->bte_interface))))
                        && mac->bte_proxy_addr.fr_dlci) {
		        printf(" %4d ", mac->bte_proxy_addr.fr_dlci);
		    } else if(is_atm(hwidb_or_null(mac->bte_interface))
			      && mac->bte_proxy_addr.proxy_mac) {
		        printf(" %4d ", mac->bte_proxy_addr.vc_num);
		    } else {
		        printf(" -    ");
		    }

		    if (mac->bte_attributes & BTE_STATIC)
		        printf(" P ");
		    else {
		        minimum = ELAPSED_TIME(mac->bte_timer)/ONEMIN;
		        if (!minimum)
			    printf(" 0 ");
		        else if (minimum < 60)
			    printf("%-2d ", minimum);
		        else
			    printf("***");
		    }
	        }

		else {    /* The concise form */
		
                    printf("\n%e   %7s   %15s ", &mac->bte_mac_addr.sword[0], 
			   baction2string(mac->bte_action),
		           (mac->bte_interface) ? 
			   ((IS_VLAN_CONFIGURED_SUBINTERFACE(mac->bte_interface)
			     || (mac->bte_interface->hwptr->type == 
				 IDBTYPE_VACCESS))?
			    mac->bte_interface->short_namestring:
			    mac->bte_interface->namestring) : "    -     ");

		    if (mac->bte_attributes & BTE_STATIC)
		        printf(" P ");
		    else {
		        minimum = ELAPSED_TIME(mac->bte_timer)/ONEMIN;
		        if (!minimum)
			    printf(" 0 ");
		        else if (minimum < 60)
			    printf("%-2d ", minimum);
		        else
                            printf("***");
                    }

                }

		/*
		 * Collect the SP-maintained statistics on demand
		 * when the bridge is displayed.
		 */
		if (mac->bte_bce_addr)
		    reg_invoke_srt_bce_statistics(mac);
		printf("%-11d%-11d", mac->bte_rxcount, mac->bte_txcount);
		if (verbose && (mac->bte_filtered_rxcount ||
				mac->bte_filtered_txcount)) {
		    printf("\n%-54s%-15d%-11d", "Filtered on-net packets",
			   mac->bte_filtered_rxcount,
			   mac->bte_filtered_txcount);
		}

		/*
		 * Print hardware specific details
		 */
		reg_invoke_srt_status_bce(mac, verbose);
		if (verbose && (is_smds(hwidb_or_null(mac->bte_interface)))) {
		    int ix;
		    printf("\n                                  ");
		    for (ix = 0; ix < STATIONLEN_SMDS64; ix++) {
			printf("%02x", mac->bte_proxy_addr.proxy_mac[ix]);
		    }
		}
	    }
	}

	/*
	 * Display the flood list and counts.
	 */
	if (verbose) {

	    tbifd_type	*flood_port;

	    printf("\n\n%15s %-11s %-11s",
		   "Flood ports", "RX count", "TX count");
	    for (flood_port = span->floodQ.qhead; flood_port;
		 flood_port = flood_port->tbifd_next) {
		printf("\n%15s %-11d %-11d",
		       flood_port->tbifd_swidb->namestring,
		       flood_port->tbifd_rx_flood_packets,
		       flood_port->tbifd_tx_flood_packets);
	    }
	}

	printf("\n");
    }

    automore_disable();
}


/*
 * show_bridge_group [<1 - 63>] "group" <"verbose">
 *
 * Display configuration and interface information for the Bridge Groups.
 *
 */

void show_bridge_group (parseinfo *csb)
{
    spantype *span;
    idbtype *idb;
    int i, index, group, printcount;
    boolean verbose = FALSE, onceflag = FALSE;
    boolean ibm_span_type;              /* SRB AST support */

    automore_enable(NULL);

    /*
     * If `group' is zero we'll display all groups.
     * Parser MACRO will only give us a valid index.
     */

    group = GETOBJ(int,1);

    if (group && (spanarray[group] == NULL)) {
        return;
    }

    /*
     * Check for the "show bridge group <verbose>" extension.
     * This displays all the redundant information that the
     * old command "show span" used to !
     */

    verbose = GETOBJ(int,2);

    if (global_crb_enable)
	printf("\n\nConcurrent routing and bridging is enabled.");

    /*
     * Note when Transparent Bridging is separated from the
     * Spanning Tree the following check will be removed.
     */

    for (index = 1; index < SPAN_MAXINDEX; index++) {

        if ((group) && (group != index))
            continue;

        span = spanarray[index];
        if ((!span) || (!span->running))
            continue;

        if (onceflag) {
            automore_conditional(0);
        } else {
            onceflag = TRUE;
            printf("\n");
        }

        ibm_span_type = (span->protocol == SPAN_PROTO_IBM);

	printf("\nBridge Group %d is running the %s compatible Spanning "
	       "Tree protocol\n",
	       index, stype2string(span));

        if (!ibm_span_type) {

            if (!span->acquire)
	        printf("\n   Acquisition of new addresses is%s", " disabled");
	    else if (verbose)
                printf("\n   Acquisition of new addresses is%s", " enabled");

	    if (IS_FAMILY(FAMILY_RP1)) {
		if (!span->cbus_copy_capable)
		    printf("\n   Cbus copy assistance is%s", " disabled");
		else if (verbose)
		    printf("\n   Cbus copy assistance is%s", " enabled");
	    }

	    if (span->mcast_source)
                printf("\n   Forwarding of multicast source addresses is%s",
		       " enabled");
	    else if (verbose)
                printf("\n   Forwarding of multicast source addresses is%s",
                       " disabled");
	    
	    if (span->lat_svc_filter)
	        printf("\n   LAT service filtering is%s", " enabled");
	    else if (verbose)
                printf("\n   LAT service filtering is%s", " disabled");

	    if ((span->lat_svc_filter) || (span->mcast_source) 
		|| (IS_FAMILY(FAMILY_RP1) && !span->cbus_copy_capable)
		|| (!span->acquire) || (verbose))
                printf("\n");
        }

	for (i = 0; i < span->port_count; i++) {

	    if ((i) && (verbose)) {
		printf("\n");
                automore_conditional(0);
	    }

	    idb = span->port[i];

	    /*
	     * In the absense of a Spanning Tree the interface will
	     * be reported as being `forwarding', if it is up.
	     * (For when we split the Spanning Tree from bridging).
             */

	    if (idb->span_ptr == NULL) {
		char *str;

		str = tbridge_encap_string(idb);
		if (str) {
		    printf("\n   Port %d (%s %s) of bridge group %d is %s",
			   (idb->port_id & 0xFF), idb->namestring,
			   str, index,
			   (interface_up(idb) ? "forwarding" : "down"));
		} else {
		    printf("\n   Port %d (%s) of bridge group %d is %s",
			   (idb->port_id & 0xFF), idb->namestring, index,
			   (interface_up(idb) ? "forwarding" : "down"));
		}
	    } else {
		char *str;

		str = tbridge_encap_string(idb);

		if (str) {
		    printf("\n   Port %d (%s %s) of bridge group %d is %s",
			   (idb->port_id & 0xFF), idb->namestring, str, index,
			   (interface_up(idb) ?
			    bstate2string(idb->port_state) : "down"));
		} else {
		    printf("\n   Port %d (%s) of bridge group %d is %s",
			   (idb->port_id & 0xFF), idb->namestring, index,
			   (interface_up(idb) ?
			    bstate2string(idb->port_state) : "down"));
		}
	    }

            if (!ibm_span_type) {

		if (idb->span_latcompress)
		    printf("\n      LAT compression is set");
		else if (verbose)
                    printf("\n      LAT compression is not set");

		if (idb->span_lat_in_svc_deny[0])
                    show_lat_service_filter("In", "deny", 
					    idb->span_lat_in_svc_deny);
		else if (verbose)
                    show_lat_service_filter("In", "deny",
                                            idb->span_lat_in_svc_deny);

		if (idb->span_lat_in_svc_permit[0])
                    show_lat_service_filter("In", "permit", 
					idb->span_lat_in_svc_permit);
                else if (verbose)
                    show_lat_service_filter("In", "permit",
                                            idb->span_lat_in_svc_permit);

                if (idb->span_lat_out_svc_deny[0])
                    show_lat_service_filter("Out", "deny",
                                        idb->span_lat_out_svc_deny);
                else if (verbose)
                    show_lat_service_filter("Out", "deny",
                                            idb->span_lat_out_svc_deny);

                if (idb->span_lat_out_svc_permit[0])
                    show_lat_service_filter("Out", "permit",
                                        idb->span_lat_out_svc_permit);
                else if (verbose)
                    show_lat_service_filter("Out", "Permit",
                                            idb->span_lat_out_svc_permit);

	    if (idb->circ_group)
		printf("\n      Circuit group %d (%sinserted, %slearning, "
		       "%sforwarding, next %s)",
		       idb->circ_group,
		       idb->circ_inserted ? NULL : "not ",
		       idb->circ_canlearn ? NULL : "not ",
		       idb->circ_canforward ? NULL : "not ",
		       ccg_get_next_idb(idb) 
		       ? ((idbtype *)(ccg_get_next_idb(idb)))->namestring 
		       : "None");

		if (idb->span_type_in)
                    printf("\n      Access list for input filtering on type"
			   " is set");
		else if (verbose)
                    printf("\n      Access list for input filtering on type"
                           " is not set");

                if (idb->span_lsap_in)
                    printf("\n      Access list for input filtering for LSAP"
                           " is set");
                else if (verbose)
                    printf("\n      Access list for input filtering for LSAP"
                           " is not set");

                if (idb->span_address_in)
                    printf("\n      Access list for input address filter is"
			   " set");
                else if (verbose)
                    printf("\n      Access list for input address filter is"
                           " not set");

                if (idb->span_pattern_in)
                    printf("\n      Access list for input pattern is set");
                else if (verbose)
                    printf("\n      Access list for input pattern is not"
			   " set");

                if (idb->span_type_out)
                    printf("\n\n      Access list for output filtering on"
                           " type is set");
                else if (verbose)
                    printf("\n      Access list for output filtering on type"
                           " is not set");

                if (idb->span_lsap_out)
                    printf("\n\n      Access list for LSAP is set");
                else if (verbose)
                    printf("\n      Access list for LSAP is not set");

                if (idb->span_address_out)
                    printf("\n      Access list for output address filter is"
                           " set");
                else if (verbose)
                    printf("\n      Access list for output address filter is"
                           " not set");

                if (idb->span_pattern_out)
                    printf("\n      Access list for output pattern filter"
                           " is set");
                else if (verbose)
                    printf("\n      Access list for output pattern filter"
			   " is not set");

		if ((idb->hwptr->span_in_bigdrop)
		    || (idb->hwptr->span_out_bigdrop))
		    printf("\n      Packets too large for translational"
		           " bridging: %d input, %d output",
		           idb->hwptr->span_in_bigdrop,
		           idb->hwptr->span_out_bigdrop);
		else if (verbose)
                    printf("\n      Packets too large for translational"
		           " bridging: %d input, %d output",
                           idb->hwptr->span_in_bigdrop,
                           idb->hwptr->span_out_bigdrop);

		/*
		 * Indicate explicitly if an FDDI interface
		 * is running in encapsulation mode.
		 */

		if ((idb->hwptr->status & IDB_FDDI) && 
		    (verbose || (idb->hwptr->fddi_options & FDDI_ENCAPSULATE)))

		    printf("\n      Interface is %s.",
			   (idb->hwptr->fddi_options & FDDI_ENCAPSULATE) ?
			   "encapsulating" : "transparent/translational");

		if (verbose && is_atm(idb->hwptr))
		    reg_invoke_media_tbridge_display_VCs(ET_ATM, idb);

		if (idb->hwptr->cbus_bridge_config)
		    if (idb->hwptr->cbus_bridge_enable)
			printf("\n      Autonomous Bridging is enabled");
		    else
			printf("\n      Autonomous Bridging is suppressed");

                if (idb->hwptr->sse_bridge_forwarding)
                        printf("\n      SSE Bridging is enabled");
	    }
	}
        printf("\n");
    }

    if (enable_80d5) {
	if (onceflag) {
	    automore_conditional(0);
	}
	printf("\n  Special translation of Ethernet Version 2 80d5 frames"
	       " is enabled\n  for the following DSAPs when using SR/TLB:");
	printcount = 0;
	for (i = 0; i <= 0xff; i++) {
	    if (sap_8209_table[i]) {
		if (!(printcount++ % 7)) {
		    printf("\n");
		}
		printf("\t %02x", i);
	    }
	}
    }
    automore_disable();
}


/*
 * show_typeaccess
 * Show the protocol type code access lists
 */

void show_typeaccess (int listnum)
{
    int i, j;
    typetype *grp;
    
    /*
     * First check for type access lists.
     */
    for (i = MINTYPEACC; i <= MAXTYPEACC; i++) {
	if ((listnum > 0) && (i != listnum))
	    continue;
	grp = type_get_acl(i);
	if (!grp)
	    continue;
	if (automore_quit())
	    return;
	printf("\nType code access list %d", i);
	for (j = 0; j < grp->items; j++) {
	    printf("\n    %s 0x%-04x 0x%-04x",
		   grp->item[j].grant ? "permit" : "deny  ",
		   grp->item[j].value, grp->item[j].mask);
	}
    }
}

/*
 * show_typeaccess_shell
 *
 * Wrap an automore shell around show_typeaccess so that show_access()
 * doesn't get all ruffled by our use of automore().
 */
static void
show_typeaccess_shell (int list)
{
    automore_enable(NULL);
    show_typeaccess(list);
    automore_disable();
}    

/*
 * show_macaddraccess
 * Show the MAC address access lists
 */

void show_macaddraccess (int listnum)
{
    int i;
    macaddracctype *item;

    /*
     * Now check for address access lists.
     */
    for (i = MINMACADDRACC; i <= MAXMACADDRACC; i++) {
	if ((listnum > 0) && (i != listnum))
	    continue;
	item = mac_get_std_acl(i);
	if (!item)
	    continue;
	if (automore_quit())
	    return;
	printf("\nBridge address access list %d", i);
	while (item) {
	    printf("\n    %s %e   %e",
		   item->grant ? "permit" : "deny  ",
		   &item->addr.u.sword[0],
		   &item->mask.u.sword[0]);
	    item = item->next;
	}
    }
}

/*
 * show_macaddraccess_shell
 *
 * Wrap an automore shell around show_macaddraccess so that show_access()
 * doesn't get all ruffled by our use of automore().
 */
static void
show_macaddraccess_shell (int list)
{
    automore_enable(NULL);
    show_macaddraccess(list);
    automore_disable();
}    

void
show_media (parseinfo *csb)
{
    int acl = GETOBJ(int,1);

    if ((acl == 0) || ((acl >= MINTYPEACC) && (acl <= MAXTYPEACC)))
	show_typeaccess_shell(acl);
    if ((acl == 0) || ((acl >= MINMACADDRACC) && (acl <= MAXMACADDRACC)))
	show_macaddraccess_shell(acl);
    if ((acl == 0) || ((acl >= MINXMACACC) && (acl <= MAXXMACACC)))
	show_xmacaccess_shell(acl);
}


/*
 * baction2string
 * Convert action code to string
 */

char *baction2string (int code)
{
    switch (code) {
    case BRIDGE_FORWARD: return("forward");
    case BRIDGE_DISCARD: return("discard");
    case BRIDGE_RECEIVE: return("receive");
    default:             return("unknown");
    }
}

/*
 * bstate2string
 * Convert bridge interface state code to string
 */

char *bstate2string (int code)
{
    switch (code) {
    case PORT_DISABLED:   return("disabled");
    case PORT_LISTENING:  return("listening");
    case PORT_LEARNING:   return("learning");
    case PORT_FORWARDING: return("forwarding");
    case PORT_BLOCKING:   return("blocking");
    default:	 	  return("unknown");
    }
}

char *stype2string (spantype *span)
{
    switch (span->protocol) {
        case SPAN_PROTO_IEEE : return("IEEE");
	case SPAN_PROTO_DEC  : return("DEC");
  	case SPAN_PROTO_IBM  : return("IBM");
	default:	       return("unknown");
	}
}

/*
 * bridging_on_idb
 *
 * tell if bridging is enabled or not
 */

static boolean bridging_on_idb (idbtype *idb)
{
    if (idb->tbridge_on_swidb) {
	return(spanarray[idb->span_index] ? TRUE : FALSE);
    }
    return(FALSE);
}

/*
 * bridging_enabled
 * Returns TRUE if bridging enabled somewhere in system
 */

static boolean bridging_enabled (void)
{
    idbtype *swidb;

    FOR_ALL_SWIDBS(swidb) {
	if (bridging_on_idb(swidb))
	    return(TRUE);
    }
    return(FALSE);
}

static boolean ppp_bridge_protocheck (hwidbtype *hwidb,
				      hwidbtype *ignore,
				      int linktype)
{
    if (hwidb->ppp_bridge_ip ||
	hwidb->ppp_bridge_ipx ||
	hwidb->ppp_bridge_atalk)
	return(TRUE);
    
    return (reg_invoke_proto_on_swidb(linktype, hwidb->firstsw));
}

ulong vtp_find_vlan_by_bridge_group (int spanindex) {
  if ((spanindex < 0) || (spanindex >= SPAN_MAXINDEX))
    return 0;

  if (spanarray[spanindex])
    return spanarray[spanindex]->vlan_id;
  return 0;
}

boolean vtp_find_domain_by_bridge_group (int span_index, 
					 unsigned char* domain_name, 
					 unsigned char* dname_length) {
  spantype* span;
  
  if ((span_index < 0) || (span_index >= SPAN_MAXINDEX))
    return FALSE;
  
  span = spanarray[span_index];
  if (span && span->vlan_id) {
    memcpy(domain_name, span->domain_name, span->dname_length);
    *dname_length = span->dname_length;
    return TRUE;
  }
  
  return FALSE;
}


int vtp_find_bridge_group_by_vlan (uchar* domain_name, 
				   uchar dname_length, ulong vlan_id) {
  int ix;
  spantype* span;
  
  for (ix = 0; ix < SPAN_MAXINDEX; ix++) {
    span = spanarray[ix];
    if (!span)
      continue;
    if (dname_length != span->dname_length)
      continue;
    if (memcmp(domain_name, span->domain_name, dname_length))
      continue;
    if (spanarray[ix]->vlan_id == vlan_id)
      return ix;
  }
  return -1; /* failure */
}

int vtp_configure_bridge_group (boolean create_group, 
				uchar* domain_name, 
				uchar dname_length, 
				ulong vlan_id) { 
  spantype *span;
  idbtype *idb;
  int index, i;
  
  if (!bridge_enable) 
    return -1;
  
  if (!create_group) {
    index = vtp_find_bridge_group_by_vlan(domain_name,
					  dname_length, vlan_id);
    if (index == -1)
      return -1;
    span_delete(index);
    return index;
  }
  
  /* Find an empty spanarray entry */
  for (index = 1; index < SPAN_MAXINDEX; index++) {
    span = spanarray[index];
    if ((!span) || (!span->running))
      break;
  }
  
  if (index == SPAN_MAXINDEX)
    return -1;
  
  /* SRB AST support */
  span = span_lookup(index, TRUE, SPAN_PROTO_IEEE);
  if (!span) 
    return -1;
  
  span->bridge_max_age = SPAN_IEEE_MAXAGE;
  
  span->bridge_hello_time = SPAN_IEEE_HELLOTIME;
  
  span->bridge_forward_delay = SPAN_IEEE_FORWARDDELAY;
  
  span->priority = SPAN_IEEE_BRIDGE_PRIORITY;
  
  span->path_divisor = SPAN_IEEE_PATHDIVISOR;
  
  span_bridge_priority(span, span->priority);
  
  for (i = 0; i < span->port_count; i++) {
    idb = span->port[i];
    if (!idb->path_cost_explicit) /* if not explicitly set */
      idb->path_cost = SCALED_BANDWIDTH(idb->visible_bandwidth) /
	span->path_divisor;
  }
  
  span->protocol = SPAN_PROTO_IEEE;
  span->vlan_id = vlan_id;
  memset(span->domain_name, 0, VTP_DOMAIN_NAME_MAX_LENGTH);
  memcpy(span->domain_name, domain_name, dname_length);
  span->dname_length = dname_length;
  span_initialize(span);
  
  return index;
}

void vtp_config_subif_in_bridge_group (boolean is_adding, int bg,
				       idbtype* swidb) {
  spantype *span;
  idbtype *pidb;
  hwidbtype *hwidb;
  short circgrp;
  
  hwidb = swidb->hwptr;
  
  if (!bridge_enable) 
    return;
  
  if ((hwidb->status & IDB_BRIDGE) == 0) 
    return;
  
  /*
   * Make sure bg is in range.
   */
  
  if ((bg) && ((bg < 1) || (bg >= SPAN_MAXINDEX))) 
    return;
  
  span = span_lookup(bg, FALSE, 0);
  if (!span) 
    return;
  
  if (!is_adding) {
    
    /*
     * Explicitly de-configure transparent bridging
     * on the sub-interface.
     */
    swidb->tbridge_on_swidb = FALSE;
    swidb->tbridge_route_linktypes = 0x0;
    
    /*
     * If this was the last sub-interface configured for
     * transparent bridging, explicitly de-configure
     * transparent bridging on the interface itself.
     */
    FOR_ALL_SWIDBS_ON_HW(hwidb, pidb)
      if (pidb->tbridge_on_swidb)
	break;
    if (!pidb) {
      hwidb->tbridge_on_hwidb = FALSE;
      reg_invoke_rxtypes_update(hwidb->firstsw);
    }
    /*
     * Remove this interface from the bridge group.
     */
    if (span->running)
      span_port_disable(span, swidb);
    span_delete_port(swidb);
    /*
     * Make sure that cbus-bridging is explicitly
     * deconfigured here otherwise the configuration
     * check function will re-configure it.
     */
    swidb->hwptr->cbus_bridge_config = FALSE;
    swidb->hwptr->bridge_transit = FALSE;
    
    if (swidb->circ_group) {
      circgrp = swidb->circ_group;
      swidb->circ_group = 0;
      span_circfix(swidb, circgrp);
    }
    reg_invoke_media_tbridge_swidb_vc_queue_clear(ET_ATM, swidb);
    
  } else {
    /*
     * Add new interface to the bridge group. 
     */
    
    if (swidb->span_index != bg) {
      
      /*
       * malloc the hash table base for the SMF for this
       * interface.  Can't get started without it.  SMF
       * entries themselves are allocated from a chunk.
       * Once malloc'ed, the SMF base is never freed.
       * It will be re-used if bridging is re-configured.
       */
      if (!smf_create_filter(swidb))
	return;
      
      swidb->span_index = bg;
      
      /*
       * Explicitly configure transparent bridging
       * on this subinterface, and indicate that
       * transparent bridging is configured on at
       * least one subinterface on this interface.
       */
      swidb->tbridge_on_swidb = TRUE;
      swidb->tbridge_route_linktypes = span->route_linktypes;
      hwidb->tbridge_on_hwidb = TRUE;
      reg_invoke_rxtypes_update(swidb);
      
      /*
       * CSCdi12635
       * Change the default port priority value from 0 to
       * SPAN_IEEE_PORT_PRIORITY(128)
       * SPAN_DEC_PORT_PRIORITY = SPAN_IEEE_PORT_PRIORITY
       */
      swidb->port_id = ((SPAN_IEEE_PORT_PRIORITY << 8) |
			(swidb->if_number & 0xFF));
      swidb->number_bit_mask = (1 << (swidb->if_number % 32));
      swidb->bit_mask_word = swidb->if_number / 32;
      swidb->path_cost = SCALED_BANDWIDTH(swidb->visible_bandwidth) /
	span->path_divisor;
      if (swidb->path_cost == 0) {
	swidb->path_cost = span->protocol == SPAN_PROTO_DEC ?
	  SPAN_DEC_DEFAULT_PATHCOST  :
	  SPAN_IEEE_DEFAULT_PATHCOST ;
      }
      span_fixup(bg, hwidb, swidb);
      if (span->running) {
	span_port_enable(span, swidb);
      }
      if ((hwidb->status & IDB_FDDI) && (hwidb->fddi_options &
					 FDDI_ENCAPSULATE))  {
	hwidb->bridge_transit = TRUE;
      }
      if (is_atm(hwidb)) {
	/*
	 * now put all the vc's associated w/ this sw
	 * interface in a linked list of vc's.  this is to
	 * make flooding easier.Applies to AAL5SNAP encap only.
	 */
	reg_invoke_media_tbridge_swidb_vc_queue_init(ET_ATM,
						     swidb);
	hwidb->bridge_transit = TRUE;
	/*
	 * Set up SMF filter here for ATM interfaces
	 */
	reg_invoke_tbridge_smf_update(hwidb->hardware,
				      swidb,
				      SMF_MACTYP_OWN_UNICAST |
				      SMF_ACTION_RECEIVE);
      }
    }
  }
  
  reg_invoke_srt_config_check(swidb, span);
}

/*
 * bridge_init
 * Initialize our data structures for ST and SR bridging.
 */

void bridge_init (subsystype *subsys)
{
    int i;
    idbtype *idb;

    /*
     * Register some functions with the system registry
     */
    srt_media_registry();
    reg_add_bandwidth_changed(span_bandwidth_change, "span_bandwidth_change");
    reg_add_encap_fixup(tbridge_network_fixup, "tbridge_network_fixup");
    reg_add_raw_enqueue(LINK_IEEE_SPANNING, span_enqueue, "span_enqueue");
    reg_add_raw_enqueue(LINK_DEC_SPANNING, span_enqueue, "span_enqueue");
    reg_add_raw_enqueue(LINK_SDE, sde_enqueue, "sde_enqueue");
    reg_add_fast_setup(span_fastsetup, "span_fastsetup");
    reg_add_onemin(tbridge_ager, "tbridge_ager");
    reg_add_sw_idb_init(bridge_init_idb, "bridge_init_idb");
    reg_add_hw_idb_init(bridge_hwidb_init, "bridge_hwidb_init");
    reg_add_proto_running(LINK_BRIDGE, bridging_enabled, "bridging_enabled");
    reg_add_proto_on_swidb(LINK_BRIDGE, bridging_on_idb, "bridging_on_idb");
    reg_add_bridge_adjust(tbridge_adjust, "tbridge_adjust");
    reg_add_determine_fair_queue_flow_id(LINK_BRIDGE,
					 bridge_fair_queue_flow_id,
					 "bridge_fair_queue_flow_id");
    reg_add_print_network_header(LINK_BRIDGE, bridge_print_network_header,
				 "bridge_print_network_header");
    reg_add_access_show(show_typeaccess, "show_typeaccess");
    reg_add_access_show(show_macaddraccess, "show_macaddraccess");
    reg_add_access_show(show_xmacaccess, "show_xmacaccess");
    reg_add_bridge_enqueue(bridge_routefailed, "bridge_routefailed");
    reg_add_tbridge_smf_update(tbridge_smf_update, "tbridge_smf_update");
    reg_add_tbridge_smf_delete(tbridge_smf_delete, "tbridge_smf_delete");
    reg_add_tbridge_smf_delete_by_result(tbridge_smf_delete_by_result,
					 "tbridge_smf_delete_by_result");
    reg_add_tbridge_smf_show(tbridge_smf_show, "tbridge_smf_show");
    reg_add_bridge_entry_global_remove(bridge_entry_global_remove, 
				       "bridge_entry_global_remove");
    reg_add_tbridge_bte_entry_remove(tbridge_bte_entry_remove, 
				     "tbridge_bte_entry_remove");
    reg_add_tbridge_bte_entry_locate(tbridge_bte_entry_locate, 
				     "tbridge_bte_entry_locate");
    reg_add_bridge_circuit_remove(span_circuit_remove, "span_circuit_remove");
    reg_add_bridge_clear_interface(span_clear_interface,
				   "span_clear_interface");
    reg_add_srt_srb_multicast((service_srt_srb_multicast_type)datagram_done,
			    "datagram_done");
    reg_add_srt_srb_forward((service_srt_srb_forward_type)datagram_done,
			    "datagram_done");
    reg_add_srt_permit_vidb_span((service_srt_permit_vidb_span_type) return_false,
				 "return_false");
    reg_add_srt_add_spanidb((service_srt_add_spanidb_type) return_nothing,
			    "return_nothing");
    reg_add_srt_add_all_spanidb((service_srt_add_all_spanidb_type) return_nothing,
			    "return_nothing");
    reg_add_srt_span_index((service_srt_span_index_type) return_zero,
			   "return_zero");

    reg_add_encaps_nvram_write(ET_SDE, dot10_sde_vlan_encaps_nvram_write,
                               "dot10_sde_vlan_encaps_nvram_write");
    reg_add_set_802_10_sde_encap(set_802_10_sde_encap, "set_802_10_sde_encap");
    reg_add_deencap_802_10_sde(deencap_802_10_sde, "deencap_802_10_sde");
    reg_add_match_group_name(match_lat_group_name, "match_lat_group_name");

    reg_add_create_idb(tbridge_bvi_parser_createidb,
		       "tbridge_bvi_parser_createidb");
    reg_add_stay_shutdown(tbridge_bvi_stay_shutdown,
			  "tbridge_bvi_stay_shutdown");
    reg_add_swif_comingup(tbridge_bvi_swif_comingup,
			  "tbridge_bvi_swif_comingup");
    reg_add_swif_erase(tbridge_bvi_swif_erase,
		       "tbridge_bvi_swif_erase");
    reg_add_bvi_smf_update(bvi_smf_update, "bvi_smf_update");
    reg_add_bvi_smf_delete(bvi_smf_delete, "bvi_smf_delete");
    reg_add_bvi_smf_delete_by_result(bvi_smf_delete_by_result, 
				    "bvi_smf_delete_by_result");

    reg_add_mac_address_change(bvi_mac_address_change,
			       "bvi_mac_address_change");

    reg_add_lec_bridge_init(lec_bridge_init, "lec_bridge_init");
    reg_add_vtp_find_vlan_by_bridge_group(vtp_find_vlan_by_bridge_group,
					  "vtp_bridge_group_vlan"); 
    reg_add_vtp_find_domain_by_bridge_group(vtp_find_domain_by_bridge_group,
					    "vtp_bridge_group_domain"); 
    reg_add_vtp_find_bridge_group_by_vlan(vtp_find_bridge_group_by_vlan, 
					  "vtp_find_bridge_group_by_vlan");
    reg_add_vtp_configure_bridge_group(vtp_configure_bridge_group,
				       "vtp_configure_bridge_group");
    reg_add_vtp_config_subif_in_bridge_group(vtp_config_subif_in_bridge_group,
				     "vtp_config_subif_in_bridge_group");
    reg_add_span_port_disable(span_port_disable, "span_port_disable");
    reg_add_bridge_address_check(bridge_address_check, "bridge_address_check");

    reg_add_tbridge_clear_cache(tbridge_clear_cache,
                                "tbridge_clear_cache");

    /*
     * Set up some debugging flags
     */
    srt_debug_init();
    tbridge_debug_init();
    smf_debug_init();
    smf_parser_init();
    
    for (i = 0; i < SPAN_MAXINDEX; i++)
	spanarray[i] = NULL;
    bridgeQ = NULL;
    hybridge_started = FALSE;
    enable_80d5 = FALSE;
    for (i = 0; i <= 0xff; i++) {
	sap_8209_table[i] = FALSE;
    }
    /* Start with some defaults */
    default_sap_8209_table[0xf0] = default_sap_8209_table[0x04] =
      default_sap_8209_table[0x08] = default_sap_8209_table[0x0c] = 
	default_sap_8209_table[0x00] = TRUE;
    for (i = 0; i <= 0xff; i++) {
	sap_8209_table[i] = default_sap_8209_table[i];
    }
    bte_count = 0;
    queue_init(&bteQ, 0);
    bte_total = 0;
    FOR_ALL_SWIDBS(idb)
	bridge_init_idb(idb, FALSE);
    reg_invoke_srt_set_shifa(TRUE);

    /*
     * Register BRIDGECP with PPP
     */
    reg_invoke_ncp_register(LINK_BRIDGE, TYPE_PPP_BRIDGECP,
			    NULL, ppp_bridge_protocheck, "BRIDGECP");
    reg_invoke_ncp_register_link(LINK_IEEE_SPANNING, 0, LINK_BRIDGE);
    reg_invoke_ncp_register_link(LINK_DEC_SPANNING, 0, LINK_BRIDGE);
    reg_invoke_ncp_register_link(LINK_LAT, 0, LINK_BRIDGE);
    reg_invoke_ncp_register_link(LINK_MOP_CONSOLE, 0, LINK_BRIDGE);
    reg_invoke_ncp_register_link(LINK_MOP_BOOT, 0, LINK_BRIDGE);

}
