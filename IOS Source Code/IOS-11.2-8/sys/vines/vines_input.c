/* $Id: vines_input.c,v 3.7.8.17 1996/08/28 13:19:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_input.c,v $
 *------------------------------------------------------------------
 * vines_input.c -- handle Banyan Vines input
 *
 * February 1990, David R. Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_input.c,v $
 * Revision 3.7.8.17  1996/08/28  13:19:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.8.16  1996/08/13  14:34:50  rbadri
 * CSCdi64089:  Multiprotocol support fixes required
 * Branch: California_branch
 * Vines support for LANE
 *
 * Revision 3.7.8.15  1996/08/07  09:03:35  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.7.8.14  1996/08/03  23:39:48  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.7.8.13  1996/07/13  11:40:55  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.7.8.12  1996/07/02  23:11:54  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.7.8.11  1996/06/24  09:24:35  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.7.8.10  1996/06/04  01:46:56  ehk
 * CSCdi57934:  VINES: LAN Extender will not work with VINES.
 * Branch: California_branch
 * set default vines encapsulation to ARPA on LEX port and
 * made it user configurable.
 *
 * Revision 3.7.8.9  1996/05/17  12:15:43  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.4.4  1996/05/06  00:06:09  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.6.4.3  1996/04/26  07:59:17  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.4.2  1996/04/03  22:09:17  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.4.1  1996/03/17  18:53:35  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.7.8.8  1996/05/10  01:43:34  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.7.8.7  1996/05/09  14:48:06  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.8.6  1996/05/05  07:10:50  ehk
 * CSCdi55508:  VINES: Problem with padding and byte count for Intel NIC
 * for server.
 * Branch: California_branch
 * loosen up packet length validation to allow paddings.
 *
 * Revision 3.7.8.5  1996/05/04  02:15:13  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface smf support.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Revision 3.7.8.4  1996/04/29  09:43:27  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.7.8.3  1996/04/29  02:42:22  bcole
 * CSCdi55590:  configuring ipx routing  breaks IP ping
 * Branch: California_branch
 *
 * Add 'ipdiag' to the list of subsystem dependencies for subsystems which
 * support the ping command.
 *
 * Revision 3.7.8.2.4.1  1996/04/27  07:38:07  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.8.2  1996/04/17  13:50:00  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.7.8.1.8.1  1996/04/08  02:11:01  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.7.8.1  1996/03/18  22:31:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.12.1  1996/03/22  09:44:42  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7  1996/03/01  01:07:00  ehk
 * CSCdi50438:  Vines client cant attach Netpro CD_ROM drive on 10.3
 * allow odd length packets padded to even on Ethernet.
 *
 * Revision 3.6.8.1  1996/03/05  05:37:12  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.6.2.1  1996/02/27  21:20:07  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6  1996/02/08  03:07:31  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.5.6.1  1996/02/15  19:25:54  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * In vines_input(), make allowance for ATM-LANE packets so as not
 * to have bad_len set to FALSE.
 *
 * Revision 3.5  1995/12/07  22:33:26  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/12/01  21:38:56  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.3  1995/11/17  17:57:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:47:43  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:44:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/10/17  19:35:00  slin
 * CSCdi42292:  VINES:  too restrictive on MAC/VIP frame size comparison
 * - Allow frames with padding.
 *
 * Revision 2.7  1995/09/01  23:06:21  asastry
 * CSCdi39698:  WFQ: Display info (in "show queue") for non-IP protocols
 * as well
 *
 * Revision 2.6  1995/08/17  02:23:54  fred
 * CSCdi38896:  incorrect length of conversation discriminator hash in the
 * fair queue conversatiion discrimination routine.
 *
 * Revision 2.5  1995/07/09  21:10:18  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Derive the CRB route/bridge behaviour from explicit, per-interface
 * route/bridge indicators rather than inferring it from protocol-
 * specific configuration.
 *
 * Revision 2.4  1995/06/23  18:48:50  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.3  1995/06/20  20:11:25  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/19  04:09:17  speakman
 * CSCdi35697:  General TB and SRB regression failures
 * Make protocol-specific route/bridge test explicit rather than relying
 * on per-interface rxtypes indicators to provide backward compatible
 * SRB and TB behaviour.
 *
 * Revision 2.1  1995/06/07  23:14:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "linktype.h"
#include "logger.h"
#include "../os/signal.h"
#include "mgd_timers.h"
#include "../if/ether.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "../if/priority_private.h"
#include "../h/address.h"
#include "config.h"
#include "../iprouting/route.h"
#include "vines_private.h"
#include "vines_ipc.h"
#include "vinesfast.h"
#include "vines_debug.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../vines/msg_vines.c"		/* Not a typo, see logger.h */
#include "../wan/ppp.h"
#include "../wan/ppp_registry.h"
#include "../if/atm.h"
#include "../lane/lane.h"


extern void crashdump(int);

static watched_queue *vinesQ;		/* VINES input queue */
static watched_queue *vinesbgQ;		/* VINES input queue */
watched_queue *vinesrtpQ;		/* VINES RIP socket queue */
queuetype vines_proxyQ;
queuetype vines_idbQ_space, *vines_idbQ;

uint vines_network;			/* our VINES network number */
ushort vines_host;			/* our VINES host number (== 1) */
ushort vines_client;			/* next VINES host number to assign */
boolean vines_running;			/* if running VINES */
vinesidbtype *vinesnullidb;
vinesidbtype *vines_deleted_idbs;
pid_t vines_pid;
pid_t vines_bg_pid;
pid_t vines_rtr_pid;
mgd_timer vines_timer, vines_bkgnd_timer;


/*
 * VINES subsystem header
 */

#define VINES_MAJVERSION 1
#define VINES_MINVERSION 0
#define VINES_EDITVERSION  1

SUBSYS_HEADER(vines, VINES_MAJVERSION, VINES_MINVERSION, VINES_EDITVERSION,
	      vines_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: ipdiag, ppp",
	      "req: tring");

/*
 * fair conversation generator
 *
 *	level 2 protocol	1
 *	destination net		4
 *	destination host	6
 *	source net		4
 *	source host		6
 */

static
short vines_fair_queue_flow_id (paktype *pak)
{
    vinesiptype *vip;
    int 	 sum;

    pak->fair_weight = FAIR_QUEUE_BASE_WEIGHT;
    vip = (vinesiptype *)pak->network_start;
    sum = xnethash(&vip->protocol,
		   sizeof(uchar) + 2*V_NETBYTES + 2*sizeof(ushort));
    return(fair_queue_sum(pak, sum));
}

/*
 * vines_print_network_header
 *
 * Print Vines header.
 */
static inline ulong vines_address (uchar *vaddr) {
    ulong address;
    address = vaddr[0];
    address = address*256 + vaddr[1];
    address = address*256 + vaddr[2];
    address = address*256 + vaddr[3];
    return(address);
}
static void vines_print_network_header (paktype *pak, ushort **data)
{
    vinesiptype *vip;
    
    vip = (vinesiptype *)pak->network_start;
    printf("\n  source: %z, destination: %z, protocol: %d",
	   vines_address(vip->ssrcnet), vip->srchost, 
	   vines_address(vip->ddstnet), vip->dsthost, 
	   vip->protocol);
}

/*
 * vines_clear_counters
 *
 * Clear all VINES specific counters.  This is done by adding the value
 * in the counter to its the saved information counter, and then setting
 * the counter to zero.
 */

static void vines_clear_counters (hwidbtype *hwidb)
{
    idbtype *swidb;
    vinesidbtype *idb;
    int i;

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	idb = vines_getidb(swidb);
	if (!idb)
	    continue;
	idb->si_rx_not_enabled = idb->rx_not_enabled;
	idb->si_rx_too_small = idb->rx_too_small;
	idb->si_rx_too_large = idb->rx_too_large;
	idb->si_rx_bad_size = idb->rx_bad_size;
	idb->si_rx_local_dest = idb->rx_local_dest;
	idb->si_rx_bcast_in = idb->rx_bcast_in;
	idb->si_rx_forwarded = idb->rx_forwarded;
	idb->si_rx_no_route = idb->rx_no_route;
	idb->si_rx_zero_hop_count = idb->rx_zero_hop_count;
	idb->si_rx_checksum_error = idb->rx_checksum_error;
	idb->si_rx_icp_error = idb->rx_icp_error;
	idb->si_rx_icp_metric = idb->rx_icp_metric;
	idb->si_rx_icp_illegal = idb->rx_icp_illegal;
	idb->si_rx_ipc = idb->rx_ipc;
	idb->si_rx_rtpv0_illegal = idb->rx_rtpv0_illegal;
	idb->si_rx_rtpv1_illegal = idb->rx_rtpv1_illegal;
	idb->si_rx_rtpv0_bcast = idb->rx_rtpv0_bcast;
	idb->si_rx_rtpv1_bcast = idb->rx_rtpv1_bcast;
	idb->si_rx_spp = idb->rx_spp;
	idb->si_rx_ip_unknown = idb->rx_ip_unknown;
	idb->si_rx_ipc_unknown = idb->rx_ipc_unknown;
	idb->si_rx_bcast_helpered = idb->rx_bcast_helpered;
	idb->si_rx_bcast_forwarded = idb->rx_bcast_forwarded;
	idb->si_rx_bcast_duplicate = idb->rx_bcast_duplicate;
	idb->si_rx_echo = idb->rx_echo;
	idb->si_rx_mac_echo = idb->rx_mac_echo;
	idb->si_rx_arpv0_discarded = idb->rx_arpv0_discarded;
	idb->si_rx_arpv0_illegal = idb->rx_arpv0_illegal;
	idb->si_rx_arpv1_discarded = idb->rx_arpv1_discarded;
	idb->si_rx_arpv1_illegal = idb->rx_arpv1_illegal;
	idb->si_rx_proxy_reply = idb->rx_proxy_reply;
    
	idb->si_tx_unicast = idb->tx_unicast;
	idb->si_tx_bcast = idb->tx_bcast;
	idb->si_tx_forwarded = idb->tx_forwarded;
	idb->si_tx_failed_encaps = idb->tx_failed_encaps;
	idb->si_tx_failed_access = idb->tx_failed_access;
	idb->si_tx_failed_down = idb->tx_failed_down;
	idb->si_tx_notbcast_toward_source = idb->tx_notbcast_toward_source;
	idb->si_tx_notbcast_noservers = idb->tx_notbcast_noservers;
	idb->si_tx_notbcast_notlan = idb->tx_notbcast_notlan;
	idb->si_tx_notbcast_notgt4800 = idb->tx_notbcast_notgt4800;
	idb->si_tx_notbcast_ppcharge = idb->tx_notbcast_ppcharge;
	idb->si_tx_bcast_forwarded = idb->tx_bcast_forwarded;
	idb->si_tx_bcast_helpered = idb->tx_bcast_helpered;
	idb->si_tx_icp_error = idb->tx_icp_error;
	idb->si_tx_icp_metric = idb->tx_icp_metric;
	idb->si_tx_ipc = idb->tx_ipc;
	idb->si_tx_spp = idb->tx_spp;
	idb->si_tx_echo = idb->tx_echo;
	idb->si_tx_mac_echo = idb->tx_mac_echo;
	idb->si_tx_proxy = idb->tx_proxy;
	
	for (i = 0; i < VARP_LAST; i++) {
	    idb->si_rx_arpv0_type[i] = idb->rx_arpv0_type[i];
	    idb->si_tx_arpv0_type[i] = idb->tx_arpv0_type[i];
	    idb->si_rx_arpv1_type[i] = idb->rx_arpv1_type[i];
	    idb->si_tx_arpv1_type[i] = idb->tx_arpv1_type[i];
	}
	for (i = 0; i < VRTP_LAST; i++) {
	    idb->si_rx_rtpv0_type[i] = idb->rx_rtpv0_type[i];
	    idb->si_tx_rtpv0_type[i] = idb->tx_rtpv0_type[i];
	}
	for (i = 0; i < VRTP55_LAST; i++) {
	    idb->si_rx_rtpv1_type[i] = idb->rx_rtpv1_type[i];
	    idb->si_tx_rtpv1_type[i] = idb->tx_rtpv1_type[i];
	}
    }
}
    
void vines_traffic_checkpoint (void)
{
    hwidbtype *idb;
    idb_si_counters_t	*sic;

    vipc_traffic_checkpoint();

    FOR_ALL_HWIDBS(idb) {
	sic = idb_get_counter_info(idb, IDB_COUNTER_SI_CNT_ACTIVE);
	if (sic) {
	    sic->rx_frames[ACCT_PROTO_VINES] = vines_frames_input(idb);
	    sic->tx_frames[ACCT_PROTO_VINES] = vines_frames_output(idb);
	    sic->rx_bytes[ACCT_PROTO_VINES] = vines_bytes_input(idb);
	    sic->tx_bytes[ACCT_PROTO_VINES] = vines_bytes_output(idb);
	}
	vines_clear_counters(idb);
    }
}

/*
 * vines_init_idb_counters 
 *
 * Initialize all counters on the VINES private interface structure.
 */

static void vines_init_idb_counters (vinesidbtype *idb)
{
    int i;

    idb->rx_not_enabled = idb->si_rx_not_enabled = 0;
    idb->rx_too_small = idb->si_rx_too_small = 0;
    idb->rx_too_large = idb->si_rx_too_large = 0;
    idb->rx_bad_size = idb->si_rx_bad_size = 0;
    idb->rx_local_dest = idb->si_rx_local_dest = 0;
    idb->rx_bcast_in = idb->si_rx_bcast_in = 0;
    idb->rx_forwarded = idb->si_rx_forwarded = 0;
    idb->rx_no_route = idb->si_rx_no_route = 0;
    idb->rx_zero_hop_count = idb->si_rx_zero_hop_count = 0;
    idb->rx_checksum_error = idb->si_rx_checksum_error = 0;
    idb->rx_icp_error = idb->si_rx_icp_error = 0;
    idb->rx_icp_metric = idb->si_rx_icp_metric = 0;
    idb->rx_icp_illegal = idb->si_rx_icp_illegal = 0;
    idb->rx_ipc = idb->si_rx_ipc = 0;
    idb->rx_rtpv0_illegal = idb->si_rx_rtpv0_illegal = 0;
    idb->rx_rtpv1_illegal = idb->si_rx_rtpv1_illegal = 0;
    idb->rx_rtpv0_bcast = idb->si_rx_rtpv0_bcast = 0;
    idb->rx_rtpv1_bcast = idb->si_rx_rtpv1_bcast = 0;
    idb->rx_spp = idb->si_rx_spp = 0;
    idb->rx_ip_unknown = idb->si_rx_ip_unknown = 0;
    idb->rx_ipc_unknown = idb->si_rx_ipc_unknown = 0;
    idb->rx_bcast_helpered = idb->si_rx_bcast_helpered = 0;
    idb->rx_bcast_forwarded = idb->si_rx_bcast_forwarded = 0;
    idb->rx_bcast_duplicate = idb->si_rx_bcast_duplicate = 0;
    idb->rx_echo = idb->si_rx_echo = 0;
    idb->rx_mac_echo = idb->si_rx_mac_echo = 0;
    idb->rx_arpv0_discarded = idb->si_rx_arpv0_discarded = 0;
    idb->rx_arpv0_illegal = idb->si_rx_arpv0_illegal = 0;
    idb->rx_arpv1_discarded = idb->si_rx_arpv1_discarded = 0;
    idb->rx_arpv1_illegal = idb->si_rx_arpv1_illegal = 0;
    idb->rx_proxy_reply = idb->si_rx_proxy_reply = 0;

    idb->tx_unicast = idb->si_tx_unicast = 0;
    idb->tx_bcast = idb->si_tx_bcast = 0;
    idb->tx_failed_encaps = idb->si_tx_failed_encaps = 0;
    idb->tx_failed_access = idb->si_tx_failed_access = 0;
    idb->tx_failed_down = idb->si_tx_failed_down = 0;
    idb->tx_notbcast_toward_source = idb->si_tx_notbcast_toward_source = 0;
    idb->tx_notbcast_noservers = idb->si_tx_notbcast_noservers = 0;
    idb->tx_notbcast_notlan = idb->si_tx_notbcast_notlan = 0;
    idb->tx_notbcast_notgt4800 = idb->si_tx_notbcast_notgt4800 = 0;
    idb->tx_notbcast_ppcharge = idb->si_tx_notbcast_ppcharge = 0;
    idb->tx_bcast_forwarded = idb->si_tx_bcast_forwarded = 0;
    idb->tx_icp_error = idb->si_tx_icp_error = 0;
    idb->tx_icp_metric = idb->si_tx_icp_metric = 0;
    idb->tx_ipc = idb->si_tx_ipc = 0;
    idb->tx_spp = idb->si_tx_spp = 0;
    idb->tx_echo = idb->si_tx_echo = 0;
    idb->tx_mac_echo = idb->si_tx_mac_echo = 0;
    idb->tx_proxy = idb->si_tx_proxy = 0;

    for (i = 0; i < VARP_LAST; i++) {
        idb->rx_arpv0_type[i] = idb->si_rx_arpv0_type[i] = 0;
        idb->tx_arpv0_type[i] = idb->si_tx_arpv0_type[i] = 0;
        idb->rx_arpv1_type[i] = idb->si_rx_arpv1_type[i] = 0;
        idb->tx_arpv1_type[i] = idb->si_tx_arpv1_type[i] = 0;
    }
    for (i = 0; i < VRTP_LAST; i++) {
        idb->rx_rtpv0_type[i] = idb->si_rx_rtpv0_type[i] = 0;
        idb->tx_rtpv0_type[i] = idb->si_tx_rtpv0_type[i] = 0;
    }
    for (i = 0; i < VRTP55_LAST; i++) {
        idb->rx_rtpv1_type[i] = idb->si_rx_rtpv1_type[i] = 0;
        idb->tx_rtpv1_type[i] = idb->si_tx_rtpv1_type[i] = 0;
    }
} 

/*
 * vines_enable_test
 * Return TRUE if we're running Vines
 * Called by SERVICE_PROTO_RUNNING.
 */

static boolean vines_enable_test (void)
{
    return(vines_running);
}
/*
 * vines_routing_idb
 * Return TRUE iff Vines packets inbound on this interface should be
 * routed.
 */

static boolean vines_routing_idb (idbtype *swidb)
{
    if ((global_crb_enable || global_irb_enable) && swidb->tbridge_on_swidb) {
	/*
	 * For CRB, just return the vines route/bridge indicator
	 * if this interface is in a bridge group.  This is
	 * set by the "bridge <n> route vines" command.
	 */
	return(swidb->tbridge_route_linktypes & LINK_VINES_RBI);
    } else {
	/*
	 * Otherwise, just verify that vines routing is enabled,
	 * and this interface is enabled for vines.  This latter
	 * is set by the "vines metric" command.
	 */
	vinesidbtype *vines_idb = vines_getidb(swidb);
	
	return(vines_running &&
	       vines_idb &&
	       vines_idb != vinesnullidb &&
	       vines_idb->vines_metric != 0);
    }
}

/*
 * vines_bridging_idb
 * Return TRUE iff Vines packets inbound on this interface should be
 * bridged.
 */

static boolean vines_bridging_idb (idbtype *swidb)
{
    if (global_crb_enable) {
	/*
	 * For CRB, just return the complement of the vines
	 * route/bridge indicator if this interface is in
	 * a bridge group.  This is cleared by the
	 * "no bridge <n> route vines" command (the default).
	 */
	return(swidb->tbridge_on_swidb &&
	       (!(swidb->tbridge_route_linktypes & LINK_VINES_RBI)));
    } else if (global_irb_enable) {
	return(swidb->tbridge_on_swidb &&
	       !(swidb->tbridge_bridge_linktypes & LINK_VINES_RBI));
    } else {
	/*
	 * To support SRB and the archaic transparent bridging
	 * behaviour just check for the absence of vines routing.
	 */
	return(!vines_running);
    }
}

/*
 * vines_rxtypes_update
 *
 * Maintain the route/bridge state of a protocol's rxtypes.
 * Called whenever protocol-specific configuration changes
 * or transparent-bridging-specific configuration changes
 * are made.  Updates a swidb's rxtype_flags to reflect the
 * new configuration.
 *
 * Determines whether the protocol should be routed and sets/
 * clears rxtypes_flags appropriately.  If Concurrent Routing
 * and Bridging is enabled, the decision is based on the
 * protocol configuration for the given swidb.  Otherwise,
 * the decision is based on the protocol configuration for
 * the router as a whole.
 */

void vines_rxtypes_update (idbtype *swidb)
{
    boolean rte_vn, br_vn;

    if (global_crb_enable || global_irb_enable) {
	rte_vn = vines_routing_idb(swidb);
	br_vn = vines_bridging_idb(swidb);
    } else {
	rte_vn = vines_running;
	br_vn = !rte_vn;
    }

    if (rte_vn) {
	SET_ROUTING_RXTYPE(swidb, RXTYPE_VINES);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_VINES);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_SAP_VINES);
    } else {
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_VINES);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_VINES);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_SAP_VINES);
    }

    if (br_vn) {
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_VINES);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_VINES);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_SAP_VINES);
    } else {
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_VINES);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_VINES);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_SAP_VINES);
    }

    if (swidb->hwptr->setup_bridging)
	(*(swidb->hwptr->setup_bridging))(swidb->hwptr);

    reg_invoke_bridge_setup();	/* sse_bridging_setup_enqueue() */
}

/*
 * vines_showproto
 * Interface specific portion of "show protocols" commands
 * Called by SERVICE_SHOWPROTO
 */

static void vines_showproto (idbtype *idb)
{
    uint value;

    if (idb == NULL) {
	if (RUNNING(LINK_VINES))
	    printf("\n  Vines routing is enabled");
	return;
    }
    if (RUNNING_IF(LINK_VINES, idb)) {
	value = vines_get_interface_metric(idb);
	printf("\n  Vines metric is %04x [%s] (%s seconds)", value,
	       vmetric_to_config(value), vmetric_to_seconds(value));
    }
}

/*
 * vines_init_idb
 *
 * Initialize an interface for VINES.
 */
void vines_init_idb (idbtype *phys_idb)
{
    vinesidbtype *idb;
    leveltype level;
    hwidb_sb_t sbtype;

    if (vines_deleted_idbs) {
	level = raise_interrupt_level(ALL_DISABLE);
	idb = vines_deleted_idbs;
	vines_deleted_idbs = idb->next_deleted;
	reset_interrupt_level(level);
	idb->next_deleted = NULL;
    } else {
	idb = malloc_named(sizeof(vinesidbtype), "IDB: Vines Info");
	if (!idb) {
	    crashdump(0);
	}
    }

    /*
     * Set up pointers back and forth.
     */ 
    sbtype = SWIDB_SB_VINES;
    idb_add_swsb(phys_idb, &sbtype, idb);
    idb->phys_idb = phys_idb;
    if (!vines_idbQ) {
	vines_idbQ = &vines_idbQ_space;
	queue_init(vines_idbQ, 0);
    }
    if (phys_idb != nullidb)
	enqueue(vines_idbQ, idb);

    /*
     * Initialize the configuration infirmation.
     */
    idb->vines_metric = 0L;
    idb->metric_is_default = FALSE;
    idb->vines_accesslist = 0;
    TIMER_STOP(idb->vines_redirect_time);
    idb->vines_redirect_interval = ONESEC;
    if (idb_is_ether_like(phys_idb))
      idb->vines_enctype = ET_ARPA;
    else if (phys_idb->hwptr->status & IDB_TR)
      idb->vines_enctype = ET_VINES_TR;
    else
      idb->vines_enctype = ET_SNAP;
    idb->vines_split_disabled = FALSE;
    idb->vines_lineup = FALSE;
    phys_idb->hwptr->vines_routecache = 
	    reg_invoke_fast_supported(LINK_VINES, phys_idb->hwptr);
    idb->vines_wakeup_interval = VRTP_DEF_WAKEUP_INTERVAL;
    idb->vines_update_count = 0;
    idb->vines_update_delta_only = FALSE;
    idb->vines_query_count = 0;
    mgd_timer_init_leaf(&idb->wakeup_timer, &vines_idb_timer,
			VT_WAKEUP, idb, FALSE);
    mgd_timer_init_leaf(&idb->query_timer, &vines_idb_timer,
			VT_QUERY, idb, FALSE);
    mgd_timer_init_leaf(&idb->sync_timer, &vsrtp_sync_timer,
			VT_SYNC, idb, FALSE);

    /*
     * Initialize Network state and topology
     */
    varp_idb_init(idb);
    idb->propagate = VPROP_DYNAMIC;
    idb->serverless = VSVLESS_DYNAMIC;
    o_init(&idb->path_list, NULL);
    idb->v0_servers = idb->v0_routers = idb->v0_clients = 0;
    idb->v1_servers = idb->v1_routers = idb->v1_clients = 0;

    /*
     * Initialize the counters.
     */
    vines_init_idb_counters(idb);
}

/*
 * vines_set_encaps
 *
 * Used to set the default encapsulation. Used by LANE to set the default
 * to ARPA instead of the SNAP encapsulation for ATM interfaces.
 */
void vines_set_encaps (idbtype *swidb, int encap)
{
    vinesidbtype *idb;
    
    if (!vines_on_idb(swidb))
        return;
    idb = vines_getidb(swidb);
    /*
     * If there is an encapsulation change, then record the correct one.
     * This happens for ATM-LANE. This is the only way we could change
     * the default encaps to ARPA for LANE.
     */
    idb->vines_enctype = encap;
}
/*
 * vines_register_thyself
 */
static void vines_register_thyself (void)
{
    vines_parser_init();
    vines_media_registry();

    /*
     * Register some functions
     */
    reg_add_fast_setup(vines_fastsetup, "vines_fastsetup");
    reg_add_will_reload(vines_reload_imminent, "vines_reload_imminent");
    reg_add_clear_counters(vines_clear_counters, "vines_clear_counters");
    reg_add_proto_on_swidb(LINK_VINES, vines_on_idb, "vines_on_idb");
    reg_add_routed_proto_on_swidb(LINK_VINES, vines_on_idb, "vines_on_idb");
    reg_add_printf((long) 'Z', vines_printf_bigz, "vines_printf_bigz");
    reg_add_printf((long) 'z', vines_printf_littlez, "vines_printf_littlez");
    reg_add_ping_parse(vines_ping_parse, "vines_ping_parse");
    reg_add_ping_run(LINK_VINES, vines_ping, "vines_ping");
    reg_add_ping_run(LINK_VINES_ECHO, vines_ping, "vines_ping");
    reg_add_trace_run(LINK_VINES, vines_trace_banyan, "vines_trace_banyan");
    reg_add_trace_run(LINK_VINES_ECHO, vines_trace_cisco, "vines_trace_cisco");
    reg_add_proto_running(LINK_VINES, vines_enable_test, "vines_enable_test");
    reg_add_proto_bridging_on_swidb(LINK_VINES, vines_bridging_idb,
				    "vines_bridging_idb"); 
    reg_add_proto_routing_on_swidb(LINK_VINES, vines_routing_idb,
				   "vines_routing_idb");
    reg_add_rxtypes_update(vines_rxtypes_update,"vines_rxtypes_update");
    reg_add_showproto(vines_showproto, "vines_showproto");
    reg_add_setup_global(vines_setup_global, "vines_setup_global");
    reg_add_setup_int(vines_setup_int, "vines_setup_int");
    reg_add_setup_end(vines_setup_end, "vines_setup_end");
    register_snmp_vines_echo();
    reg_add_bandwidth_changed(vines_bandwidth_changed, "vines_bandwidth_changed");
    reg_add_snapshot_start_aging_routes(LINK_VINES, vines_start_aging_paths,
					"vines_start_aging_paths");
    reg_add_swif_erase(vines_swif_erase,"vines_swif_erase");
    reg_add_determine_fair_queue_flow_id(LINK_VINES, vines_fair_queue_flow_id,
					 "vines_fair_queue_flow_id");
    reg_add_print_network_header(LINK_VINES, vines_print_network_header,
				 "vines_print_network_header");
    reg_add_encap_fixup(vines_flush_static_paths,"vines_flush_static_paths");
    reg_add_set_protocol_encaps(LINK_VINES, vines_set_encaps,
				vines_set_encaps);
}

/*
 * vines_init
 *
 * Initialize VINES protocol.  Default state is to allow bridging of
 * VINES by MCI hardware.
 */
void vines_init (subsystype *subsys)
{
    idbtype *swidb;

    vines_register_thyself();
    vines_debug_init();
    
    mgd_timer_init_parent(&vines_timer, NULL);
    mgd_timer_init_parent(&vines_idb_timer, &vines_timer);
    mgd_timer_init_leaf(&vines_changed, &vines_timer, VT_CHANGED, NULL, FALSE);

    mgd_timer_init_parent(&vines_bkgnd_timer, NULL);

    varp_init();
    vines_init_ipc();
    vines_neighbor_init();
    vines_route_init();

    vines_deleted_idbs = NULL;
    vines_init_idb(nullidb);
    vinesnullidb = vines_getidb(nullidb);
    vinesnullidb->vines_metric = 1;

    vipc_traffic_init();
    vines_cache_init();
    queue_init(&vines_nameQ, 0);
    queue_init(&vines_proxyQ, 0);
    vines_running = FALSE;
    vines_network = VINES_UNKNOWNNET;
    vines_host = VINES_UNKNOWNHOST;
    vines_client = VINES_1ST_CLIENT;
    FOR_ALL_SWIDBS(swidb)
	vines_rxtypes_update(swidb);
    vines_decimal_addresses = FALSE;
    vines_slowest_update = VRTP_DEF_WAKEUP_INTERVAL;

    /*
     * Register VINESCP with PPP
     */
    reg_invoke_ncp_register(LINK_VINES, TYPE_PPP_VINESCP,
			    NULL, NULL, "VINESCP");
    /*
     * Cache invalidation routine, used by ATM interface to 
     * invalidate fast cache entries, when VCs go down.
     */
    reg_add_bump_cache_version(vines_bump_cache, "vines_bump_cache");
}

/*
 * vines_start
 *
 * This routine starts the VINES protocol processes if they are not already
 * running.
 */

void vines_start (void)
{
    if (vines_running)
	return;
    if (vines_pid || vines_bg_pid || vines_rtr_pid) {
	printf("\n%%The VINES processes that were previously running have not "
	       "stopped\nyet.  Please wait a few seconds and reissue this "
	       "command.");
	return;
    }

    vines_pid = process_create(vines_input, "VINES Input", NORMAL_STACK,
			       PRIO_HIGH);
    vines_bg_pid = process_create(vines_background, "VINES Protocols",
				  NORMAL_STACK, PRIO_NORMAL);
    vines_rtr_pid = process_create(vines_router, "VINES Router", NORMAL_STACK,
				   PRIO_NORMAL);
}

/*
 * vines_enqueue
 *
 * Enqueue a VINES packet for later processing by the VINES proocol
 * handler.  If VINES is not running, as determined by the vines_running
 * variable, then throw all packets away.
 */

void vines_enqueue (paktype *pak)
{
    if (vines_running) {
	process_enqueue_pak(vinesQ, pak);
    } else {
	protocol_discard(pak, ROUTING_IF(pak->if_input, LINK_VINES));
    }
}

/*
 * vines_bkgnd_enqueue
 */

static inline void vines_bkgnd_enqueue (paktype *pak)
{
    process_enqueue_pak(vinesbgQ, pak);
}

/*
 * vines_input_teardown 
 *
 * This process is being killed.  Remove any data structures that this
 * process set up when it started.
 */
static void vines_input_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    reg_delete_raw_enqueue(LINK_VINES);
    reg_delete_raw_enqueue(LINK_VINES_ECHO);
    process_watch_queue(vinesQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(vinesQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&vinesQ);
    vines_pid = 0;
}

/*
 * vines_input
 *
 * The VINES world begins here.  This routine is responsible for
 * dequeueing every VINES packet received by the sysem, and determining
 * what to do with it.  Forwarding occurs here.  Everything else is
 * poassed off to a background task.
 */
process vines_input (void)
{
    int count, len, pkt_len;
    paktype *pak;
    vinesiptype *vip;
    vinesidbtype *idb;
    boolean bad_len;

    process_wait_on_system_init();

    /*
     * Set up this processes world.
     */
    signal_permanent(SIGEXIT, vines_input_teardown);
    vinesQ = create_watched_queue("VINES packets", 0, 0);
    process_watch_queue(vinesQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_VINES, vines_enqueue, "vines_enqueue");
    reg_add_raw_enqueue(LINK_VINES_ECHO, vines_enqueue, "vines_enqueue");

    while (TRUE) {
	process_wait_for_event();
	count = PAK_SWITCHCOUNT;
	while (TRUE) {
	    /*
	     * Switch only a limited number of packets per pass
	     */
	    if (--count == 0)
		break;

	    /*
	     * Dequeue an VINES datagram
	     */
	    pak = process_dequeue(vinesQ);
	    if (!pak)
		break;

	    idb = vines_getidb(pak->if_input);
	    if ((idb == NULL) || (idb->vines_metric == 0L)) {
		if (idb)
		    idb->rx_not_enabled++;
		datagram_done(pak);
		continue;
	    }

	    idb_increment_rx_proto_counts(pak, ACCT_PROTO_VINES);
	    
	    /*
	     * VINES echo protocol processing is handled by the
	     * background process.  It is simple enough that it could
	     * really be handled at interrupt level, but putting in
	     * background provides priority to forwarded packets and
	     * prvents throttling the processor with echo packets.
	     */
	    if (pak->linktype == LINK_VINES_ECHO) {
		/* Mac echos counted later */
		vines_bkgnd_enqueue(pak);
		continue;
	    }

	    /*
	     * This is really a VINES IP packet now.  Start by setting up
	     * a pointer to the start of the transport layer.
	     */
	    vip = (vinesiptype *)pak->network_start;
	    pak->transport_start = vip->data;

	    /*
	     * Run a sanity check on the header fields.
	     */
	    pkt_len = pak->datagramsize - pak->encsize;
	    len = vip->len;
	    if (len != pkt_len) {
		if (is_smds(pak->if_input->hwptr)) {
		    bad_len = FALSE;  /* $#%@!& trailers */
		} else if (is_lapb(pak->if_input->hwptr)) {
		    /*
		     * LAPB subtracts its header length
		     */
		    bad_len = (len != pak->datagramsize);
		} else if (is_atm_lane(pak->if_input)) {
		    /* 
		     * ATM-LANE packets include 4-6 bytes of encaps info
		     * like VCD/enc_type/LEC_ID
		     */
                    bad_len = FALSE;
		} else if ((pak->if_input->hwptr->status & IDB_ETHER)
			   || (pak->if_input->hwptr->status & IDB_LEX)) {
		    /*
		     * On Ethernet, odd byte packets can be evenized
		     * depending on NICs. Byte padding can occur for any
		     * odd length packets regardless of the min. Ethernet
		     * packet size. So all we can check here is to ensure
		     * the VINES data packet does not cross the actual
		     * buffer boundary. For historical reasons, leave the 
		     * original check here as a reminder.
		     *
		     * bad_len = (pak->datagramsize > MINETHERSIZE);
		     */
		    if (len < pkt_len) {
			bad_len = FALSE;
		    } else {
			bad_len = TRUE;
		    }
		} else if (len < pkt_len) {
		    /*
		     * Assume that the extra bytes are padding.  This
		     * happens when a padded frame is bridged from Ethernet
		     * to FDDI; the pad is carried along.
		     */
		    bad_len = FALSE;
                } else {
		    bad_len = TRUE;
		}
		if (bad_len) {
		    if (vinespkt_debug) {
			char temp[30];
			sprintf(temp, "mac/vip size mismatch [ip %u]", len);
			vinesbug(pak, temp);
		    }
		    idb->rx_bad_size++;
		    datagram_done(pak);
		    continue;
		}
	    }
	    if (len < VINES_MINSIZE) {
		if (vinespkt_debug)
		    vinesbug(pak, "too small");
		idb->rx_too_small++;
		datagram_done(pak);
		continue;
	    } else if (len > VINES_MAXSIZE) {
		if (vinespkt_debug)
		    vinesbug(pak, "too large");
		idb->rx_too_large++;
		datagram_done(pak);
		continue;
	    }
	    
	    /*
	     * Decide what to do with this packet.  The vines_forward
	     * route will take care of forwarding the packet to another
	     * router, broadcasting it onto all interfaces, handing it up
	     * to the VIP level in this router, or some combination of
	     * the above.  If it returns FALSE, then ther is nothing that
	     * can be done with the packet.
	     */
	    if (vines_forward(pak))
		continue;
	    if (do_vines_pkt_debug(GETLONG(vip->ssrcnet), vip->srchost))
		vinesbug(pak, "no route to destination");
	    idb->rx_no_route++;
	    vicp_send_error(pak, VINES_ERR_HOST_UNR, FALSE);
	    datagram_done(pak);
	}
    }
}

/*
 * vines_forus
 *
 * Determine if a packet is meant for this host.  A packet is for us if
 * it has our VINES address, or one of the valid broadcast addresses.
 */
boolean vines_forus (paktype *pak)
{
    vinesiptype *vip;
    ulong dstnet;
    vinesidbtype *vidb_in = vines_getidb(pak->if_input);

    /*
     * Aimed directly at us as a network level unicast?
     */
    vip = (vinesiptype *)pak->network_start;
    dstnet = GET2WD(vip->ddstnet);
    if (dstnet == vines_network && vip->dsthost == vines_host) {
	vidb_in->rx_local_dest++;
	return(TRUE);
    }

    /*
     * Network level broadcast?  The legal combinations are
     * ffffffff:ffff, ffffffff:0001, localnet:ffff, and localnet:0001.
     */
    if (((dstnet == VINES_ALLNETS) || (dstnet == vines_network)) &&
	((vip->dsthost == VINES_ALLHOSTS) || (vip->dsthost == vines_host))) {
	vidb_in->rx_bcast_in++;
	pak->flags |= PAK_BROADCAST;
	return(TRUE);
    }
    /*
     * None of the above.
     */
    return(FALSE);
}

/*
 * vines_forward
 *
 * Forward an VINES packet.  If the hop count is zero, and the packet is
 * for one of our addresses, then feed it upstairs for further
 * consideration.
 */
boolean vines_forward (paktype *pak)
{
    vinesiptype *vip;
    vinesidbtype *idb;
    routingdecision thewaythere;
    ulong srcnet;
    boolean do_debug;

    /*
     * Did this router send the packet?  If so, then kill it before doing
     * any more processing.
     */
    vip = (vinesiptype *)pak->network_start;
    srcnet = GETLONG(vip->ssrcnet);
    do_debug = do_vines_pkt_debug(srcnet, vip->srchost);
    if ((srcnet == vines_network) && (vip->srchost == vines_host)) {
	hwidbtype *hwidb = pak->if_input->hwptr;
	if (do_debug)
	    vinesbug(pak, "ignored, my source addr");
	if ((vip->protocol == VINES_RTP) && !vines_is_my_saddr(pak) &&
	    !hwidb->loopback && !hwidb->inloopback)
	    errmsg(&msgsym(DUPADDR, VINES), pak->if_input->namestring);
	datagram_done(pak);
	return(TRUE);	  /* already taken care of */
    }

    /*
     * Handle all cases of zero hop packets here at the start.
     */
    idb = vines_getidb(pak->if_input);
    if ((vip->tc & VINES_HOPS) == 0) {
	if (vines_forus(pak)) {
	    if (do_debug)
		vinesbug(pak, "received, 0 hops");
	    vines_bkgnd_enqueue(pak);
	    return(TRUE);  /* already passed off */
	}

	/*
	 * Note: There is no need to check for a StreetTalk Server
	 * Request packet here.  This special StreetTalk packet, that we
	 * are looking for to hack, will ALWAYS be an all nets broadcast.
	 * These were just handed upstairs by the previous lines of code.
	 * Go upstairs to find it.
	 */
	if (do_debug)
	    vinesbug(pak, "hop count exceeded");
	idb->rx_zero_hop_count++;
	if (vip->protocol == VINES_IPC) {
	    vinesipctype *ipc = (vinesipctype *)vip->data;
	    if (ipc->dstport == VIPC_PORT_ECHO)
		vicp_send_error(pak, 123, FALSE);
	}
	datagram_done(pak);
	return(TRUE);
    }

    /*
     * Is this an all nets broadcast?
     */
    if (GET2WD(vip->ddstnet) == VINES_ALLNETS) {
	if (!vines_from_reverse_path(pak)) {
	    if (do_debug)
		vinesbug(pak, "drop dup all nets bcast");
	    idb->rx_bcast_duplicate++;
	    datagram_done(pak);
	    return(TRUE);
	}
	if (do_debug)
	    vinesbug(pak, "received, all nets bcast");
	idb->rx_bcast_in++;
	pak->flags |= PAK_BROADCAST;
	vines_flood_bcast(pak, TRUE, FALSE, NULL);
	vines_bkgnd_enqueue(pak);
	return(TRUE);
    }

    /*
     * Is this a subnet broadcast?  If so, do a reverse path test here to
     * see if this is a duplicate packet.  If it isn't, fall through and
     * figure out where to route it.
     */
    if (vip->dsthost == VINES_ALLHOSTS) {
	if (!vines_from_reverse_path(pak)) {
	    if (do_debug)
		vinesbug(pak, "drop dup my net bcast");
	    idb->rx_bcast_duplicate++;
	    datagram_done(pak);
	    return(TRUE);
	}
	if (GET2WD(vip->ddstnet) == vines_network) {
	    if (do_debug)
		vinesbug(pak, "received, my net bcast");
	    idb->rx_bcast_in++;
	    vines_flood_bcast(pak, TRUE, TRUE, NULL);
	    pak->flags |= PAK_BROADCAST;
	    vines_bkgnd_enqueue(pak);
	    return(TRUE);
	}
    }

    /*
     * See if the destination address (or network) exists in this routers
     * tables.  If not, then there's nothing that can be done with the
     * packet.
     */
    if (!vines_first_hop(GET2WD(vip->ddstnet), vip->dsthost, &thewaythere)) {
	vicp_send_error(pak, VINES_ERR_HOST_UNR, FALSE);
	return(FALSE);
    }

    /*
     * Is the packet destined for a station reachable via this router
     * itself?  The only cases that should trigger thisis a packet for
     * the router itself.  Because of the way the lookup function works,
     * we can also get here if the address is that of a non-existent
     * client of this router.
     */
    if (thewaythere.neighbor->newflags & V_N_SELF) {
	if (vip->dsthost != vines_host) {
	    if (do_debug)
		vinesbug(pak, "non-existent client");
	    idb->rx_no_route++;
	    datagram_done(pak);
	    vines_unlock_nodes(&thewaythere);
	    return(TRUE);
	}
	if (do_debug)
	    vinesbug(pak, "received, my address");
	idb->rx_local_dest++;
	vines_bkgnd_enqueue(pak);
	vines_unlock_nodes(&thewaythere);
	return(TRUE);
    }

    if ((vip->tc & VINES_REDIRECT) && (vip->dsthost != VINES_ALLHOSTS))
	if (vines_get_path_by_idb(thewaythere.neighbor,
				  vines_getidb(pak->if_input)))
	    vroute_send_redirects(pak, &thewaythere);
    vip->tc--;	/* since hops are the low order bits */
    pak->if_output = thewaythere.path->idb->phys_idb;
    if (vines_send1(pak, &thewaythere)) {
	idb->rx_forwarded++;
	thewaythere.path->idb->tx_forwarded++;
    }
    vines_unlock_nodes(&thewaythere);
    return(TRUE);
}

/*
 * vines_send1
 *
 * Send an VINES packet.  This routine is guaranteed to eat the packet
 * buffer, so that the caller never needs to worry about it again.
 */
vinesidbtype *vines_send1 (
    paktype *pak,
    routingdecision *explicitpath)
{
    vinesiptype *vip;
    idbtype *phys_outidb;
    vinesidbtype *outidb;
    vinesidbtype *vidb_in;
    boolean send, do_debug;
    ulong path;

    vip = (vinesiptype *)pak->network_start;
    do_debug = do_vines_pkt_debug(GETLONG(vip->ssrcnet), vip->srchost);
    phys_outidb = pak->if_output;
    vidb_in = vines_getidb(pak->if_input);
    if (!phys_outidb || (!validmem(phys_outidb))) {
	if ((GET2WD(vip->ddstnet) == vines_network) &&
	    (vip->dsthost == vines_host)) {
	    clear_if_input(pak);
	    pak->if_output = pak->if_input = nullidb;
	    vines_enqueue(pak);
	    return(vinesnullidb);
	}
	if (do_debug)
	    vinesbug(pak, "not sent, no route (1)");
	if (vidb_in)
	    vidb_in->rx_no_route++;
	datagram_done(pak);
	return(NULL);
    }

    outidb = vines_getidb(phys_outidb);
/*  buginf("\nVINES: send packet of length %d to %#z via %s",
	   vip->len, GET2WD(vip->ddstnet), vip->dsthost,
	   outidb->phys_idb->namestring);
*/
    /*
     * Is the interface up?
     */
    if (!interface_up_simple(phys_outidb)) {
	if (do_debug)
	    vinesbug(pak, "dest interface down");
	outidb->tx_failed_down++;
	datagram_done(pak);
	return(NULL);
    }

    /*
     * Before sending the packet, see if access lists are defined.
     * Only do access checks for routed traffic (if_input != NULL).
     */
    if (outidb->vines_accesslist && pak->if_input) {
	if (!vines_accesscheck(pak, outidb->vines_accesslist)) {
	    if (do_debug)
		vinesbug(pak, "blocked by access list");
	    outidb->tx_failed_access++;
	    datagram_done(pak);
	    return(NULL);
	}
    }

    /*
     * Was metric notification requested by the sender?  If so, and this
     * is the last hop to get there, then a response also needs to be
     * fired back to the originator.  Never do this for broadcasts (a
     * null second argument).
     */
    if ((vip->tc & VINES_METRIC) && explicitpath && 
	(GET2WD(vip->ddstnet) == explicitpath->neighbor->net) &&
	(vip->dsthost == explicitpath->neighbor->host))
	vicp_send_metric(pak, explicitpath->path->metric);

    /*
     * Encapsulate and ship.
     *
     * **** Make sure datagamstart is set to network_start.  This   ****
     * **** should not be necessary, but the encapsulation routines ****
     * **** currently key off datagramstart.                        ****
     */
    pak->linktype = LINK_VINES;
    pak->acct_proto = ACCT_PROTO_VINES;
    pak->datagramstart = pak->network_start;
    pak->datagramsize = vip->len;
    path =  (explicitpath ? (long)explicitpath->path : 0);
    if (phys_outidb->hwptr->vencap == NULL) {
	errmsg(&msgsym(NOVENCAP, VINES), phys_outidb->namestring,
	       GET2WD(vip->ddstnet), vip->dsthost);
	return(NULL);
    }
    send = (*phys_outidb->hwptr->vencap)(pak, path);
    if (send && explicitpath) {
	vines_cache_update(pak);
    }

    if (send) {
	if (do_debug)
	    vinesbuggw(pak, explicitpath ? explicitpath->neighbor : NULL, "sent");
	datagram_out(pak);		/* queue it for output */
	return(outidb);
    }

    /*
     * Encapsulation failed for some reason.
     */
    if (do_debug)
	vinesbug(pak, "encapsulation failed");
    outidb->tx_failed_encaps++;
    datagram_done(pak);
    return(NULL);
}

/*
 * vines_send
 *
 * Send an VINES packet.  This routine is guaranteed to eat the packet
 * buffer, so that the caller never needs to worry about it again.
 */
vinesidbtype *vines_send (paktype *pak)
{
    vinesiptype *vip;
    routingdecision thewaythere;
    vinesidbtype *idb;
    ulong dstnet;
    boolean do_debug;

    vip = (vinesiptype *)pak->network_start;
    do_debug = do_vines_pkt_debug(GETLONG(vip->ssrcnet), vip->srchost);
    dstnet = GET2WD(vip->ddstnet);
    if (dstnet == VINES_ALLNETS)
	return(vines_send1(pak, NULL));
    if (dstnet == vines_network) {
	if (vip->dsthost == VINES_ALLHOSTS) {
	    return(vines_send1(pak, NULL));
	} else if (vip->dsthost == vines_host) {
	    clear_if_input(pak);
	    if (do_debug)
		vinesbuggw(pak, NULL, "sent/received");
	    pak->if_output = pak->if_input = nullidb;
	    vines_bkgnd_enqueue(pak);
	    return(vinesnullidb);
	}
    }
    memset(&thewaythere, 0, sizeof(routingdecision));
    if (vines_first_hop(dstnet, vip->dsthost, &thewaythere)) {
	if (thewaythere.neighbor->newflags & V_NET_ME) {
	    /*
	     * Packets for this router were caught earlier.  The only way to get
	     * here is to try to send to a non-existent client.
	     */
	    vines_unlock_nodes(&thewaythere);
	    /* fall out */
	} else if (thewaythere.path && thewaythere.path->idb) {
	    pak->if_output = thewaythere.path->idb->phys_idb;
	    idb = vines_send1(pak, &thewaythere);
	    vines_unlock_nodes(&thewaythere);
	    return(idb);
	} else {
	    vines_unlock_nodes(&thewaythere);
	    /* fall out */
	}
    }

    if (do_debug)
	vinesbug(pak, "not sent, no route");
    idb = vines_getidb(pak->if_input);
    if (idb)
	idb->rx_no_route++;
    datagram_done(pak);
    return(NULL);
}

/*
 * vip_send_pak_common
 *
 * Fill in all local fields in a VINES packet and send it.
 */
vinesidbtype *vip_send_pak_common (
    paktype *pak,
    ulong dnet,
    ushort dhost,
    ulong snet,
    ushort shost,
    int protocol,
    int flags,
    int ttl)
{
    vinesiptype *vip;
    vinesidbtype *idb;

    pak->network_start = pak->transport_start - sizeof(vinesiptype);
    pak->datagramsize += sizeof(vinesiptype);
    vip = (vinesiptype *)pak->network_start;

    vip->cksum = 0;
    vip->len = pak->datagramsize;
    vip->tc = (flags | ttl) & 0xFF;
    vip->protocol = protocol;
    PUT2WD(vip->ddstnet, dnet);
    vip->dsthost = dhost;
    PUT2WD(vip->ssrcnet, snet);
    vip->srchost = shost;

    vines_crc(pak);
    pak_lock(pak);
    idb = vines_send(pak);
    if (idb) {
	if (vines_is_bcast_addr(dnet, dhost)) {
	    idb->tx_bcast++;
	} else {
	    idb->tx_unicast++;
	}
    }
    if (vinestest_debug)
	buginf_vinespak(pak);
    datagram_done(pak);
    return(idb);
}

/*
 * vip_send_specific
 *
 * Fill in all local fields in a VINES packet and send it to a specific
 * neighbor on a specific interface.
 */
vinesidbtype *vip_send_specific (
    paktype *pak,
    vinesneighbortype *neighbor,
    vinespathtype *path,
    int protocol,
    int flags)
{
    routingdecision thewaythere;
    vinesidbtype *idb;
    vinesiptype *vip;

    pak->network_start = pak->transport_start - sizeof(vinesiptype);
    pak->datagramsize += sizeof(vinesiptype);
    vip = (vinesiptype *)pak->network_start;

    vip->cksum = 0;
    vip->len = pak->datagramsize;
    vip->tc = (flags | VINES_HOPS) & 0xFF;
    vip->protocol = protocol;
    PUT2WD(vip->ddstnet, neighbor->net);
    vip->dsthost = neighbor->host;
    PUT2WD(vip->ssrcnet, vines_network);
    vip->srchost = vines_host;

    vines_crc(pak);

    thewaythere.server = NULL;
    thewaythere.route = NULL;
    thewaythere.neighbor = neighbor;
    thewaythere.path = path;
    if (path->idb)
	pak->if_output = path->idb->phys_idb;
    if ((idb = vines_send1(pak, &thewaythere)) != NULL)
	idb->tx_unicast++;
    return(idb);
}

/*
 * vip_send_broadcast
 *
 * Fill in all local fields in a VINES packet and send it.
 */
void vip_send_broadcast (
    paktype *pak,
    vinesidbtype *use,
    vinesidbtype *exclude,
    int protocol,
    int flags,
    int ttl)
{
    vinesiptype *vip;

    pak->network_start = pak->transport_start - sizeof(vinesiptype);
    pak->datagramsize += sizeof(vinesiptype);
    vip = (vinesiptype *)pak->network_start;

    vip->cksum = 0;
    vip->len = pak->datagramsize;
    vip->tc = (flags | ttl) & 0xFF;
    vip->protocol = protocol;
    PUT2WD(vip->ddstnet, VINES_ALLNETS);
    vip->dsthost = VINES_ALLHOSTS;
    PUT2WD(vip->ssrcnet, vines_network);
    vip->srchost = vines_host;
    vines_crc(pak);

    if (use) {
	pak->if_output = use->phys_idb;
	if (vines_send(pak))
	    use->tx_bcast++;
	return;
    }

    vines_flood_bcast(pak, FALSE, TRUE, exclude);
    datagram_done(pak);
}

/*
 * vip_rtp_send_specific
 *
 * Fill in all local fields in a VINES packet and send it to a specific
 * neighbor on a specific interface.  Use the VIP broadcast address, and
 * the destinations unique MAC address.
 */
vinesidbtype *vip_rtp_send_specific (
    paktype *pak,
    vinespathtype *path,
    int flags)
{
    routingdecision thewaythere;
    vinesidbtype *idb;
    vinesiptype *vip;

    pak->network_start = pak->transport_start - sizeof(vinesiptype);
    pak->datagramsize += sizeof(vinesiptype);
    vip = (vinesiptype *)pak->network_start;

    vip->cksum = 0;
    vip->len = pak->datagramsize;
    vip->tc = (flags | VINES_TTL0) & 0xFF;
    vip->protocol = VINES_RTP;
    PUT2WD(vip->ddstnet, VINES_ALLNETS);
    vip->dsthost = VINES_ALLHOSTS;
    PUT2WD(vip->ssrcnet, vines_network);
    vip->srchost = vines_host;

    vines_crc(pak);

    thewaythere.server = NULL;
    thewaythere.route = NULL;
    thewaythere.neighbor = path->neighbor;
    thewaythere.path = path;
    if (path->idb)
	pak->if_output = path->idb->phys_idb;
    if ((idb = vines_send1(pak, &thewaythere)) != NULL)
	idb->tx_unicast++;
    return(idb);
}

/*
 * vines_flood_bcast
 *
 * Broadcast processing.  This section performs the reverse path
 * forwarding test to prevent looping broadcast packets.  If then
 * iterates over the set of interfaces, testing to see if the broadcast
 * should be sent over tha interface.  It does this by checking the
 * broadcast control bits for the media type.  If an interface is to be
 * used, it makes a copy of the packet and calls vines_send.
 */
static const char bad_class[] = "\nVINES: Not sent on %s, %s.";
static const char propagate_anyway[] = "\nVINES: Sent on %s, 'Propagate' overrides '%s'.";

void vines_flood_bcast (
    paktype *pak,
    boolean decrement_hops,
    boolean all_interfaces,
    vinesidbtype *exclude)
{
    vinesiptype *vip, *newvip;
    vinesidbtype *idb, *idb_back;
    vinesidbtype *vidb_in;
    paktype *newpak;
    boolean propagate, do_debug;
    int hops;

    /*
     * Only do reverse path forwarding.  Essentially, figure out which
     * neighbor this packet should have come from if it took the most
     * direct route here from the originator.  If it did not come from
     * the router that we would expect, then throw the broadcast away so
     * that we not create a looping packet.
     */
    idb_back = NULL;
    vip = (vinesiptype *)pak->network_start;
    vidb_in = vines_getidb(pak->if_input);
    if (pak->if_input) {
	if (!vines_from_reverse_path(pak)) {
	    vidb_in->rx_bcast_duplicate++;
	    return;
	}
	idb_back = vines_get_reverse_idb(pak);
    }

    do_debug = do_vines_pkt_debug(GETLONG(vip->ssrcnet), vip->srchost);
    do_debug |= vines_bcast_debug;
    if (do_debug) {
	char foo[30];
	sprintf(foo, "flooding, hops %d class %d", (vip->tc & VINES_HOPS),
		(vip->tc & VINES_CLASS) >> 4);
	vinesbug(pak, foo);
    }
    
    FOR_ALL_VIDBS(idb) {
	/*
	 * Is the interface up and running vines?
	 */
	if (!vines_interface_up(idb))
	    continue;
	if (exclude && (exclude == idb))
	    continue;
	/*
	 * Is it in a direction AWAY from the source, and do we care?  If
	 * split horizon has been disabled, then this packet needs to be
	 * sent back out the source interface.  The prime example of this
	 * case is a hub and spoke frame relay network.  The checks above
	 * should prevent broadcasts from ping-ponging back and forth
	 * around a triangle of routers.
	 */
	if ((idb == idb_back) && !all_interfaces && pak->if_input) {
	    if (!idb->vines_split_disabled) {
		idb->tx_notbcast_toward_source++;
		continue;
	    }
	}
	/*
	 * Now check the media type for this broadcast.  The three
	 * (mutually exclusive) types are:
	 *
	 * 	  1) all lans
	 * 	  2) all interfaces faster than 48000 bps
	 * 	  3) all interfaces with no per packet charge
	 *
	 * There is also a special hack to allow propagation of packets
	 * over a serial line without decrementing the hop count.  This
	 * solves a problem where clients come up and ask for a file
	 * server wih a hop count of zero, and then a hop count of one.
	 * The minimum hop count necessary to do ether - serial - ether
	 * is two.
	 */
	hops = (vip->tc & VINES_HOPS);
	switch (idb->propagate) {
	  default:		propagate = FALSE;	break;
	  case VPROP_NEVER:	propagate = FALSE;	break;
	  case VPROP_ALWAYS:	propagate = TRUE;	break;
	  case VPROP_DYNAMIC:
	    propagate = !vines_any_servers_present();
	    break;
	}
	if (decrement_hops && pak->if_input && !propagate)
	    hops--;
	if (vip->tc & VINES_CLASS) {
	    if (vip->tc & VINES_B_SERVERS) {
		if (!vines_routers_present(idb)) { /* stub net ? */
		    if (!propagate) {
			if (do_debug)
			    buginf(bad_class, idb->phys_idb->namestring,
				   "no routers/servers");
			continue;
		    }
		    if (do_debug)
			buginf(propagate_anyway, idb->phys_idb->namestring,
			       "servers only");
		}
	    }
	    switch (vip->tc & VINES_B_LANS) {
	      case VINES_B_LANS:
		if (!(idb_is_ieeelike(idb->phys_idb))) {
		    if (propagate) {
			if (do_debug)
			    buginf(propagate_anyway, idb->phys_idb->namestring,
				   "LAN only");
			break;
		    }
		    if (do_debug)
			buginf(bad_class, idb->phys_idb->namestring,
			       "not a lan media");
		    idb->tx_notbcast_notlan++;
		    continue;
		}
		break;
		    
	      case VINES_B_GT4800:
		if (idb->phys_idb->visible_bandwidth < 4) { /* Roughly 4.8K */
		    if (propagate) {
			if (do_debug)
			    buginf(propagate_anyway, idb->phys_idb->namestring,
				   "4800bps or greater");
			break;
		    }
		    if (do_debug)
			buginf(bad_class, idb->phys_idb->namestring,
			       "speed below 4800 bps");
		    idb->tx_notbcast_notgt4800++;
		    continue;
		}
		break;
		
	      case VINES_B_NOCHARG:
		if (is_x25(idb->phys_idb->hwptr)) {
		    if (propagate) {
			if (do_debug)
			    buginf(propagate_anyway, idb->phys_idb->namestring,
				   "no per packet charges");
			break;
		    }
		    if (do_debug)
			buginf(bad_class, idb->phys_idb->namestring,
			       "has per packet charges");
		    idb->tx_notbcast_ppcharge++;
		    continue;
		}
		break;
		
	      default:
		break;
	    }
	}
	
	newpak = pak_duplicate(pak);
	if (!newpak)
	    break;
	newvip = (vinesiptype *)newpak->network_start;
	newvip->tc = (newvip->tc & ~VINES_HOPS) | hops;
	newpak->if_output = idb->phys_idb;	/* must set for bcast */
	if (vines_send(newpak)) {
	    if (pak->if_input)
		idb->tx_bcast_forwarded++;
	    else
		idb->tx_bcast++;
	}
    }
    if (vidb_in)
	vidb_in->rx_bcast_forwarded++;
}

/*
 * vines_bkgnd_do_timers
 *
 * Handle any expired timers for this process.
 */
static void vines_bkgnd_do_timers (void)
{
    mgd_timer *expired_timer;

    /*
     * Process all expired timers
     */
    while (mgd_timer_expired(&vines_bkgnd_timer)) {
	expired_timer = mgd_timer_first_expired(&vines_bkgnd_timer);
	switch (mgd_timer_type(expired_timer)) {
	  case VT_ARP:
	    varp_timer_expired(expired_timer);
	    break;
	    
	  case VT_IPC_ACK:
	    vipc_ack_timer_expired(expired_timer);
	    break;
	    
	  case VT_IPC_IDLE:
	    vipc_idle_timer_expired(expired_timer);
	    break;
	    
	  case VT_IPC_RETRANS:
	    vipc_retrans_timer_expired(expired_timer);
	    break;
	    
	  case VT_SVC_SVC:
	    vsvc_process_service(expired_timer);
	    break;
	    
	  case VT_SVC_TREE:
	    vsvc_process_tree();
	    break;
	    
	  default:
	    errmsg(&msgsym(UNEXPECTEDTIMER,SCHED), expired_timer,
		   mgd_timer_type(expired_timer));
	    mgd_timer_stop(expired_timer);
	    break;
	}
    }
}

/*
 * vines_bkgnd_do_packets
 *
 * Handle any received packets for this process.
 */
static void vines_bkgnd_do_packets (void)
{
    paktype *pak;
    vinesiptype *vip;
    vinesidbtype *idb;
    int count;

    /*
     * Process only a limit number of packets per pass.  When the limit
     * is hit, go on and do other things.  Execution will get back here
     * as soon as everything else is handled.
     */
    count = PAK_SWITCHCOUNT;
    while (count-- > 0) {
	pak = process_dequeue(vinesbgQ);
	if (pak == NULL)
	    break;

	if (pak->linktype == LINK_VINES_ECHO) {
	    vines_mac_echo(pak);
	    continue;
	}

	/*
	 * Check the VINES IP checksum on this packet.  Checksum's
	 * are only suppose to be calculated as the ultimate
	 * destination, and this is it!  Note that this means we are
	 * checking the checksum on broadcast packets before we
	 * forward them.  I don't consider this a bad thing.
	 */
	vip = (vinesiptype *)pak->network_start;
	idb = vines_getidb(pak->if_input);
	if (!vines_validcrc(pak)) {
	    idb->rx_checksum_error++;
	    datagram_done(pak);
	    continue;
	}

	pak->transport_start = vip->data;
	switch (vip->protocol) {
	  case VINES_RTP:		/* routing update protocol */
	    process_enqueue_pak(vinesrtpQ, pak);
	    break;

	  case VINES_ARP:		/* address resolution protocol */
	    vines_arp(pak);
	    datagram_done(pak);
	    break;

	  case VINES_ICP:		/* error protocol */
	    vines_icp_receive(pak, idb);
	    datagram_done(pak);
	    break;

	  case VINES_SPP:
	    idb->rx_spp++;
	    vicp_send_error(pak, VINES_ERR_NO_PROTOCOL, TRUE);
	    datagram_done(pak);
	    break;

	  case VINES_IPC:
	    idb->rx_ipc++;
	    vines_ipc_receive(pak, idb);
	    datagram_done(pak);
	    break;

	  default:
	    idb->rx_ip_unknown++;
	    vicp_send_error(pak, VINES_ERR_NO_PROTOCOL, TRUE);
	    datagram_done(pak);
	}
    }
}

/*
 * vines_background_teardown 
 *
 * This process is being killed.  Remove any data structures that this
 * process set up when it started.
 */
static void vines_background_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_mgd_timer(&vines_bkgnd_timer, DISABLE);
    process_watch_queue(vinesbgQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(vinesbgQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&vinesbgQ);
    vines_bg_pid = 0;
}

/*
 * vines_background
 *
 * Perform VINES background task handling.  This includes returning ECHO
 * packets and replying to ARP requests.  RTP is not handled here (mabye
 * later) but is processed by it's own task.
 */
process vines_background (void)
{
    ulong major, minor;

    /*
     * Startup
     */
    signal_permanent(SIGEXIT, vines_background_teardown);
    vinesbgQ = create_watched_queue("VINES packets (bg)", 0, 0);
    process_watch_queue(vinesbgQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&vines_bkgnd_timer, ENABLE);

    /*
     * Main loop
     */
    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
		vines_bkgnd_do_timers();
		break;
	      case QUEUE_EVENT:
		vines_bkgnd_do_packets();
		break;
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/* 
 * vines_crc
 * 
 * checksum a VINES packet
 */
void vines_crc (paktype *pak)
{
    vinesiptype *vip;

    /* 
     * checksum on FULL word boundary - use appropriate pointer 
     * since char stuff sometimes clears rest WORD, eg, on 11s 
     */
    vip = (vinesiptype *)pak->network_start;
    if (vip->cksum == VINES_NOCHECKSUM)
	return;
    /* check sum the packet */
    vip->cksum = vines_crc1(vip);
}


/*
 * vines_validcrc
 * 
 * Compute checksum - return TRUE if good, else FALSE
 */
boolean vines_validcrc (paktype *pak)
{		
    vinesiptype *vip;
    ushort computed;

    vip = (vinesiptype *)pak->network_start;
    if (vip->cksum == VINES_NOCHECKSUM)
	return (TRUE);			/* Not checksummed */
    computed = vines_crc1(vip);
    if (vip->cksum != computed) {
	if (do_vines_pkt_debug(GETLONG(vip->ssrcnet), vip->srchost))
	    buginf("\nVINES: bad checksum, packet %#x, computed %#x",
		   vip->cksum, computed);
	return(FALSE);
    }
    return(TRUE);
}
