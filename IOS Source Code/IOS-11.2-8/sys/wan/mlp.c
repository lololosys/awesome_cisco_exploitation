/* $Id: mlp.c,v 3.10.4.46 1996/09/10 07:32:43 dblair Exp $ 
 * $Source: /release/112/cvs/Xsys/wan/mlp.c,v $ 
 *------------------------------------------------------------------
 * mlp.c - Multilink PPP (sort of) control protocol.
 *
 * 28-June-1995, Dana Blair
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlp.c,v $
 * Revision 3.10.4.46  1996/09/10  07:32:43  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * Fix botched comment during previous commit of CSCdi62263
 *
 * Revision 3.10.4.45  1996/09/10  07:17:45  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * Fix multilink and dialer profiles
 *
 * Revision 3.10.4.44  1996/09/06  16:46:46  dblair
 * CSCdi68304:  Pings fail when multilink ppp is negotiated with 1 link in
 * the bundl
 * Branch: California_branch
 * Reorder mlp_raw_enqueue to handle the case where LINK_PPP is an IP
 * packet but IPCP is not up yet
 *
 * Revision 3.10.4.43  1996/08/28  23:09:22  syiu
 * CSCdi67460:  Multichassis multilink not dropping L2F mid
 * Branch: California_branch
 *
 * Revision 3.10.4.42  1996/08/28  13:21:59  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.4.41  1996/08/22  07:20:13  perryl
 * CSCdi66647:  Can not tell what a virtual access interface is used for
 * Branch: California_branch
 * Add an addition parameter to reg_invoke_vtemplate_create_vaccess, to
 * describe the type of the new virtual access interface.
 *
 * Revision 3.10.4.40  1996/08/22  06:00:06  syiu
 * CSCdi66793:  Bundle appointment timeout premature
 * Branch: California_branch
 *
 * Revision 3.10.4.39  1996/08/21  23:12:47  dblair
 * CSCdi66129:  bus error at PC _mlp_add_link, address 0x4AFC4B04
 * Branch: California_branch
 * Don't allow reset while changing the multilink bundle interface
 * bandwidth
 *
 * Revision 3.10.4.38  1996/08/19  19:44:56  syiu
 * CSCdi66459:  VPDN Multilink users see unformated show ppp multilink
 * Branch: California_branch
 *
 * Revision 3.10.4.37  1996/08/19  19:11:22  syiu
 * CSCdi63332:  multilink ppp will not open dec, vines, or clns
 * Branch: California_branch
 *
 * Revision 3.10.4.36  1996/08/14  14:19:07  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * The multilink half-fastswitching performance improvement
 *
 * Revision 3.10.4.35  1996/08/07  21:07:33  syiu
 * CSCdi65420:  Multichassis multilink IPCP not coming up sometimes
 * Branch: California_branch
 *
 * Revision 3.10.4.34  1996/08/03  23:41:49  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.10.4.33  1996/07/30  21:01:44  dblair
 * CSCdi57980:  debug ppp multilink is too busy
 * Branch: California_branch
 * Clean up multilink debugs
 *
 * Revision 3.10.4.32  1996/07/23  13:31:37  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.10.4.31  1996/07/23  02:18:03  syiu
 * CSCdi63744:  MLP debug linktypes should be in ascii
 * Branch: California_branch
 *
 * Revision 3.10.4.30  1996/07/22  22:14:56  syiu
 * CSCdi63719:  MLP output message would be helpful
 * Branch: California_branch
 *
 * Revision 3.10.4.29  1996/07/19  19:50:44  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly - do not allocate
 * holdq until needed, and pass the holdq correctly as calls and callbacks
 * are made.  Redo a little callback code to deal with the holdq better and
 * rename some of the structures.  Add a debug dialer holdq (hidden).
 * Branch: California_branch
 *
 * Revision 3.10.4.28  1996/07/13  00:40:06  syiu
 * CSCdi62866:  Bundle interface still up when physical interface down
 * Branch: California_branch
 *
 * Revision 3.10.4.27  1996/07/10  00:46:51  syiu
 * CSCdi62375:  VPDN message should be more helpful when no vtemplate
 * defined
 * Branch: California_branch
 *
 * Revision 3.10.4.26  1996/07/09  22:34:11  syiu
 * CSCdi62356:  Bundle interfaces on reused Vaccess int sometimes dont
 * route
 * Branch: California_branch
 *
 * Revision 3.10.4.25  1996/07/08  15:57:56  syiu
 * CSCdi62107:  Multichassis config conflict with Dialer
 * Branch: California_branch
 *
 * Revision 3.10.4.24  1996/07/01  12:55:37  dblair
 * CSCdi61321:  crash in mlp_remove_link when chaning bri encap
 * Prevent encaps changes on a dialer/ISDN interface from changing any
 * VACCESS interfaces that may be a part of the rotary-group.
 * Branch: California_branch
 *
 * Revision 3.10.4.23  1996/06/27  09:45:15  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.10.4.22  1996/06/26  04:43:38  syiu
 * CSCdi61310:  Offload MMP and Vtemplate conflicts on PRI
 * Branch: California_branch
 *
 * Revision 3.10.4.21  1996/06/22  07:38:24  syiu
 * CSCdi59165:  PPP Virtual-Access Int will not negotiate NCPs on
 * projected links
 * Branch: California_branch
 *
 * Revision 3.10.4.20  1996/06/22  02:32:52  dblair
 * CSCdi61104:  Multilink max-fragment 1 does not round robin
 * Branch: California_branch
 *
 * Revision 3.10.4.19  1996/06/19  20:38:17  fox
 * CSCdi59847:  Dynamic dialer maps are not cleared on async dialer ints
 * Branch: California_branch
 * Call reg_invoke_ipcp_cleanup_ip_peer() to remove the neighbor route,
 * the dynamic dialer map, and any other mechanisms tied to the current
 * peer address before clearing the peer address.
 *
 * Revision 3.10.4.18  1996/05/27  04:03:59  fox
 * CSCdi58112:  LCP PROTREJ packets not transmitted from a multilink
 * virtual interfa
 * Branch: California_branch
 * Send LCP Protocol Rejects out on the bundle interface.
 *
 * Revision 3.10.4.17  1996/05/24  05:04:07  syiu
 * CSCdi58634:  PPP Multilink not working with AAA
 * Branch: California_branch
 *
 * Revision 3.10.4.16  1996/05/22  22:04:18  syiu
 * CSCdi58463:  show ppp multi to show dialer interface (if any)
 * Branch: California_branch
 *
 * Revision 3.10.4.15  1996/05/22  07:50:38  perryl
 * CSCdi58348:  Divide by Zero crash cause by zero bandwidth MLP bundle
 * master
 * Branch: California_branch
 * In mlp_remove_link(), do not subtract bandwidth of the last link on a
 * bundle interface
 *
 * Revision 3.10.4.14  1996/05/21  18:18:05  perryl
 * CSCdi56888:  Bundle partitioning should reset and display warning
 * Branch: California_branch
 *
 * Revision 3.10.4.13  1996/05/21  17:26:26  syiu
 * CSCdi57970:  Installed Route remains in Dialer interface after
 * disconnected
 * Branch: California_branch
 *
 * Revision 3.10.4.12  1996/05/21  10:09:41  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.4.11  1996/05/19  23:31:34  perryl
 * CSCdi56888:  Bundle partitioning should reset and display warning
 * Branch: California_branch
 *
 * Revision 3.10.4.10  1996/05/19  06:21:03  dblair
 * CSCdi57958:  Multiple Dialer Maps and MLP dont mix well
 * Branch: California_branch
 *
 * Revision 3.10.4.9  1996/05/19  05:48:56  dblair
 * CSCdi57979:  Multilink clean up from Dial1_branch
 * Branch: California_branch
 * Optimize fragmentation to remove multiply and divides from the
 * data path.
 * Xmit fragments from the mlp_holdq_enqueue.
 * update BW when links are added and removed from bundle
 * Fix byte counts and packet counts on the virtual bundle interface
 * Clean up
 *
 * Revision 3.10.4.8  1996/05/07  19:13:55  dblair
 * CSCdi56726:  c1000: %SYS-3-SUPNONE: Registry 26 doesnt exist
 * mlpvt is not in all images that contain multilink.  remove
 * mlpvt_event_occured registry and use mlp_appoint_bundle instead.
 * Branch: California_branch
 *
 * Revision 3.10.4.7  1996/05/07  18:08:03  snyder
 * CSCdi56844:  spellink errors
 * Branch: California_branch
 *
 * mulitilink -> multilink
 *
 * Revision 3.10.4.6  1996/05/06  18:52:40  perryl
 * CSCdi56515:  Virtual Access IP unnumbered is intrusive
 * Branch: California_branch
 * Virtual Template and Access interfaces are default to "no ip address"
 * when
 * created. Creating virtual access intf with "ip unnumbered" is now an
 * option.
 * For MLP, if there isn't a dialer interface associated with an incoming
 * call,
 * it will create a virtual access interface as the bundle interface with
 * the
 * "ip unnumbered" options. In most cases, the default "no ip address" is
 * used.
 * In selecting "ip unnumbered" source, it will first try to pick up
 * ethernet,
 * token ring, FDDI, ATM, serial and it will not pick dialer, virtual
 * template
 * and virtual access interfaces.
 *
 * Revision 3.10.4.5  1996/05/06  18:09:16  syiu
 * CSCdi56520:  Show ppp multilink not showing member links
 * Branch: California_branch
 *
 * Revision 3.10.4.4  1996/04/29  09:45:45  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.10.4.3  1996/04/27  06:36:55  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.10.4.2  1996/04/20  04:03:18  dblair
 * CSCdi51057:  Crash in m32_tx_start using multilink ppp connected to
 * 3com Impact.  Kill fragments and packet if reassembled size is larger
 * than mrru.
 * Branch: California_branch
 *
 * Revision 3.7.8.19  1996/04/25  23:59:23  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.7.8.18  1996/04/17  00:01:27  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.7.8.17  1996/04/08  18:07:35  syiu
 * Branch: Dial1_branch
 * Synchronize Vaccess interface state changes. Otherwise route adjusts etc
 * won't work correctly.
 *
 * Revision 3.7.8.16  1996/04/06  21:26:52  dblair
 * Branch: Dial1_branch
 * Fix CSCdi53768 and added restart lcp when ppp multilink command
 * processed, and fix multilink not negotiated at reload problem.
 *
 * Revision 3.7.8.15  1996/04/03  22:59:11  abaindur
 * Branch: Dial1_branch
 *  Enhance bid generation scheme for MLP bundle mastership. Provide
 * a means of controlling mastership based on either locality of a call
 * or load-sharing.
 *
 * Revision 3.7.8.14  1996/03/29  15:54:58  dblair
 * Branch: Dial1_branch
 * Fix crash when clearing virtual interface and add some multilink knobs
 *
 * Revision 3.7.8.13  1996/03/19  17:34:19  dblair
 * Branch: Dial1_branch
 * Multilink should not update idb->status
 *
 * Revision 3.7.8.12  1996/03/19  12:42:31  dblair
 * Branch: Dial1_branch
 * use NULL string for bundle name if no authetication and no caller id.
 * free bundle after removing it in mlp_timer.
 *
 * Revision 3.7.8.11  1996/03/13  22:12:43  syiu
 * Branch: Dial1_branch
 * Remove unecessary double Vtemplate clone attempts
 *
 * Revision 3.7.8.10  1996/03/09  07:18:49  syiu
 * Branch: Dial1_branch
 * Move virtual-template from mlpvt to mlp
 *
 * Revision 3.7.8.9  1996/03/08  17:30:32  dblair
 * Branch: Dial1_branch
 * Change for multilink virtual bundle interfaces to support ISDN BRI,
 * PRI, and Dialer interfaces
 *
 * Revision 3.7.8.8  1996/03/01  22:28:36  perryl
 * Branch: Dial1_branch
 * Check bundle pointer before passing into set_if_input() in
 * mlp_process_rcv_packet().
 *
 * Revision 3.7.8.7  1996/02/23  21:31:44  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.7.8.6  1996/02/08  06:50:38  syiu
 * Branch: Dial1_branch
 * Tie together MLPVT/MLP/VPN registries, add vtemplate
 * support in MLPV
 *
 * Revision 3.7.8.5  1996/02/07  20:23:11  syiu
 * Branch: Dial1_branch
 * Add MLP Stackability calls support
 *
 * Revision 3.7.8.4  1996/02/06  05:03:08  syiu
 * Branch: Dial1_branch
 * Add MLP Bundle Interface reset support
 *
 * Revision 3.7.8.3  1996/02/05  16:16:54  syiu
 * Branch: Dial1_branch
 * Fix break in MLP modularity
 *
 * Revision 3.7.8.2  1996/02/04  06:29:39  syiu
 * Branch: Dial1_branch
 * Support for MLP Bundle interfaces
 *
 * Revision 3.7.8.1  1996/01/16  15:45:19  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.10.4.1  1996/03/18  22:48:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.3  1996/03/16  07:55:52  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.2.2  1996/03/07  11:07:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.2.1  1996/02/20  21:52:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/08  18:02:43  dblair
 * CSCdi48424:  Multilink PPP may discard fragments while processing input
 * fragments.  Do not drop fragments when unassigned threshold is reached
 * and allow lost fragments to be detected earlier.
 *
 * Revision 3.9  1996/02/07  19:46:08  dblair
 * CSCdi47994:  Router crashes when low on memory
 * Handle getbuffer fail correctly and clean up multilink data structures
 * when bundle goes down
 *
 * Revision 3.8  1996/01/20  04:44:04  dblair
 * CSCdi45363:  MLP information no longer displayed in show dialer output
 * Move multilink bundle info from show dialer to new show ppp multilink
 * command
 *
 * Revision 3.7  1995/12/23  03:46:25  dblair
 * CSCdi45604:  Crashed later on after changing the BRI encaps to PPP
 * on other side.
 *
 * Revision 3.6  1995/12/11  14:20:20  dblair
 * CSCdi45019:  Previous commit broke modularity by calling ncp_lowerup
 * from mlp.c.  Added registry to fix modularity.
 *
 * Revision 3.5  1995/12/08  20:13:23  dblair
 * CSCdi45292:  Mulitlink lost frag timer started when Received lost
 * fragment.  Prevent the start of the lost frag timer in this case.
 *
 * Revision 3.4  1995/12/08  18:52:49  dblair
 * CSCdi45019:  Disconnect of initial multilink call requires NCPs
 * to renegotiate.  Added code to for the NCP's to renegotiate when the
 * initial link is disconnected.
 *
 * Revision 3.3  1995/11/29  18:28:55  dblair
 * CSCdi40974:  PPP Multi Link breaks modularity
 * include ppp_debug.h to resolve references to multilink_debug
 *
 * Revision 3.2  1995/11/29  01:59:07  dblair
 * CSCdi41333:  DDR Callback and Multilink PPP clean up
 *
 * Revision 3.1  1995/11/09  13:49:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/25  23:10:29  dblair
 * CSCdi42567:  PPP Multilink dereferences NULL
 * Used change_if_input after reassembling fragments into a
 * newly created packet.  Changed to use set_if_input
 *
 * Revision 2.3  1995/10/20  17:14:08  dblair
 * CSCdi40641:  Bridging over MLP ISDN PRI broken
 *         Bridge code assumed 6 byte header.  Multilink now completes
 *         header when receiving and encapsulated bridge packet
 *
 * Revision 2.2  1995/10/16  22:53:41  snyder
 * CSCdi42258:  spellink errors
 *              fialed -> failed
 *
 * Revision 2.1  1995/09/20  22:39:01  dblair
 * CSCdi40745:  PPP Multilink feature commit
 * 
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "dialer_registry.h"
#include "media_registry.h"
#include "../ip/ip_registry.h"
#include "../wan/ppp_registry.h"
#include "../mlpvt/mlpvt_registry.h"
#include "packet.h"
#include "../os/pool.h"
#include "address.h"
#include "login.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../wan/msg_ppp.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "parser.h"
#include "../util/md5.h"
#include "aaa.h"
#include "../wan/parser_defs_dialer.h"
#include "../wan/parser_defs_ppp.h"
#include "../parser/parser_actions.h"
#include "dialer.h"
#include "dialer_private.h"
#include "ppp_public.h"
#include "ppp.h"
#include "ppp_auth.h"
#include "ncp.h"
#include "mlp.h"
#include "../mlpvt/mlpvt.h"
#include "../mlpvt/mlpvt_mq_protocol.h"
#include "../ip/ip_addrpool.h"
#include "../if/network.h"
#include "../if/if_vtemplate.h"
#include "../vpn/l2f.h"
#include "auth_registry.regh"

/*
 * Global variables
 */

mlpbundletype *mlp_bundle_list_head = NULL;
int mlp_virtual_template_num;
queuetype mlp_pending_ncpQ;

/*
 * Local variables
 */

static pid_t mlp_pid;
static pid_t mlp_outbound_pid;
static watched_queue *MLPpacketQ;
static watched_queue *MLPpacketOutQ;
static sys_timestamp MLPtimer;

/*
 *  Local prototypes
 */  

static void mlp_disconnect_link(mlpbundletype *);
static void mlp_kill_link(hwidbtype *);
static void mlp_timer(void);
static void mlp_enqueue(paktype *);



/*
 *  The code
 */

/*
 *  mlp_fastsend
 *  fastsend code for outbound half-fastswitching
 */

static void mlp_fastsend (hwidbtype *idb, paktype *pak)
{
    paktype *newpak;

    /*
     *  Set pak output idb to the one chosen fastswitching code
     *    If ddr is involved, idb != pak->if_output BUT idb ==
     *    d-channel or dialer interface.
     */
    pak->if_output = idb->firstsw;

    if (pak->buffer_flags & PAK_BUF_DO_NOT_Q) {
	/*
	 *  The packet is in special buffer pool, most likely MEMD.
	 *    copy the packet to a global pool buffer before queueing
	 *    to process level.
	 */

	/*
	 *  pak_duplicate is necessary because the eventual
	 *   call to datagram_out may reference any parameters
	 *   in the packet header.
	 */

	newpak = pak_duplicate(pak);
	if (!newpak) {
	    datagram_done(pak);
	    return;
	}
	set_if_input(newpak, pak->if_input);
	datagram_done(pak);
	pak = newpak;

    }

    process_enqueue_pak(MLPpacketOutQ, pak);

}


/*
 *  Do something with the half-fastswitched packets that
 *    have been queued from the fastswitching code
 */

static inline void mlp_outbound_packet (paktype *pak) 
{
    
    hwidbtype *idb;

    idb = pak->if_output->hwptr;

    if (!pak->if_output->is_bundle_interface) {
	if (multilink_events_debug) {
	    buginf("\nMLP %s:Packet forwarded to wrong interface",
		   idb->hw_namestring);
	}
	datagram_done(pak);
	return;
    }

    if (mlp_fragment(pak, idb->mlp_link->bundle)) {
	/*
	 *  Overload pak->mlp_prior_frag with pak->if_output.
	 *  pak->if_output points to the actual output idb for the
	 *    first fragment.  We need to restore pak->if_output to
	 *    point to the virtual bundle interface so that datagram_out
	 *    will call mlp_holdq_enqueue.  We set pak->if_output =
	 *    (idbtype *)pak->mlp_prior_frag in mlp_holdq_enqueue so
	 *    the first fragment will go out over the designated interface.
	 */
	pak->mlp_prior_frag = (paktype *)pak->if_output;
	pak->if_output = idb->firstsw;
    }

    /*
     *  Finally send the packet
     */
    datagram_out(pak);

}

/*
 *  mlp_output
 *  process multilink packets that have been queued from fastswitching
 *  this is outbound half-fastswitching
 */

static process mlp_output (void)
{
    ulong major, minor;
    int pak_count;
    paktype *pak;

    process_wait_on_system_init();

    /*
     *  Setup the world for this process
     */

    MLPpacketOutQ = create_watched_queue("MLP output",0,0);
    process_watch_queue(MLPpacketOutQ, ENABLE, RECURRING);

    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		for (pak_count = PAK_SWITCHCOUNT; pak_count; pak_count--) {
		    pak = process_dequeue(MLPpacketOutQ);
		    if (pak == NULL) {
			break;
		    }
		    mlp_outbound_packet(pak);
		}
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/* 
 * Register thyself
 */
void mlp_subsys_init()
{
    mlp_virtual_template_num = 0;
    mlp_outbound_pid = NO_PROCESS;
    MLPpacketOutQ = NULL;
    mlp_pid = NO_PROCESS;
    MLPpacketQ = NULL;

    /*
     * Add Local registry
     */ 
    reg_add_mlp_search_bundle(mlp_search_bundle, "mlp_search_bundle");
    reg_add_mlp_get_vtemplate_num(mlp_get_vtemplate_num,"mlp_get_vtemplate_num");
    reg_add_mlp_appoint_bundle(mlp_appoint_bundle,"mlp_appoint_bundle");
    reg_add_mlp_get_bundle_list(mlp_get_bundle_list,"mlp_get_bundle_list");
    reg_add_mlp_reset_bundle(mlp_reset_bundle, "mlp_reset_bundle");
    reg_add_mlp_find_bundle_netidb(mlp_find_bundle_netidb,
				   "mlp_find_bundle_netidb");

    /*
     * Register Multilink PPP as an NCP
     *
     * This kludge is done to minimize checks in the data path lookup of
     * link type and PPP protocol.  In effect, we trade off exception case
     * checks in all of the routines that manipulate the fsm entries in
     * return for no 'if multilink' checks on the data path.
     */
    reg_invoke_ncp_register(LINK_MULTILINK, TYPE_PPP_MULTILINK, NULL, NULL,
			    "Multilink");

    queue_init(&mlp_pending_ncpQ, 0); 
}

static void mlp_init_bundle (mlpbundletype *bundle,
		      enum MLP_BUNDLE_STATE bundle_state)
{
    /*
     *  Multilink initialization
     */
    bundle->state = bundle_state;
    bundle->nlinks = 0;
    bundle->min_links = 0;
    bundle->max_links = MAXINT;
    bundle->max_frags = MLP_DEFAULT_MAX_FRAGS;
    bundle->is_frag = TRUE;
    bundle->is_lost_frag = FALSE;
    bundle->mrru = DEF_MULTILINK_MRRU;
    bundle->min_frag_size = MLP_DEFAULT_MIN_FRAG_SIZE;
    bundle->send_seq = MLP_INIT_SEQ;
    bundle->rcv_seq = MLP_INIT_SEQ;
    bundle->lost_fragments = 0;
    bundle->discarded_fragments = 0;
    bundle->received_lost = 0;
    bundle->out_of_order = 0;
    bundle->unassigned = 0;
    bundle->una_frag_list = NULL;
    bundle->una_frag_end = NULL;
    bundle->rcved_end_bit = TRUE;
    bundle->lost_frag_algo = MLP_LOST_FRAG_UNA_GT_LINKS;
    bundle->seq_window_size = MLP_SEQ_WINDOW_SIZE_24BITS;
    bundle->first_link = NULL;
    bundle->master_link = NULL;
    bundle->add_link_threshold = 0;
    bundle->delete_link_threshold = 0;
    bundle->thresh_direction = DIALER_THRESH_EITHER;
    bundle->are_links_equal = TRUE;
    bundle->callbacktype = CALLBACK_NONE;
    bundle->wait_for_add_ticks = MLP_DEFAULT_WAIT_FOR_ADD_TIME;
    bundle->wait_before_disconnect_ticks = MLP_DEFAULT_WAIT_BEFORE_DISCONNECT_TIME;
    bundle->load_analyze_ticks = MLP_DEFAULT_LOAD_ANALYZE_TIME;

    TIMER_START(bundle->load_analyze_timer, bundle->load_analyze_ticks);

    TIMER_STOP(bundle->lost_frag_timer);
    TIMER_STOP(bundle->wait_for_add_timer);
    TIMER_STOP(bundle->wait_before_disconnect_timer);
    TIMER_STOP(bundle->appointment_timer);

}

static void mlp_init_link (mlplinktype *link,
			   mlpbundletype *bundle, hwidbtype *idb)
{
    link->bundle = bundle;
    link->idb = idb;
    idb->firstsw->is_bundle_interface = FALSE;
    ncp_enable(idb, LINK_MULTILINK);
}

static void mlp_clear_link (mlplinktype *link, hwidbtype *idb)
{
    ncp_disable(idb, LINK_MULTILINK);
}

/*
 *  mlp_remove_encaps
 *  Remove the multilink header from the packet
 */

inline static void mlp_remove_encaps (paktype *pak)
{
    ushort size;
    
    /*
     * encsize = sizeof of LCP header and protocl considering header and
     *  protocol compression.  Multilink has another 4 or 2 bytes of
     *  header depending on sequence number size
     */
    
    size = pak->encsize + mlp_get_seq_size();
    
    pak->datagramstart += size;
    pak->datagramsize -= size;
    pak->encsize = 0;
    
}

/*
 * mlp_update_timer
 * Inline to update a timer.
 */
static inline void mlp_update_timer (sys_timestamp *timer, long ticks)
{
    sys_timestamp newtimer;

    TIMER_START(newtimer, ticks);
    if (TIMER_RUNNING(*timer)) {
        *timer = newtimer;
    }
}

static mlpbundletype * mlp_create_bundle (void)
{

    mlpbundletype *bundle;

    bundle = malloc_named(sizeof(mlpbundletype), "MLP bundle");
    if (!bundle) {
	return(NULL);
    }

    return(bundle);

}

/*
 *  mlp_add_bundle_to_dialer_rotary_group
 *  attempt to add the virtual bundle interface to the same
 *    rotary-group as the first link in the bundle.
 */

static boolean mlp_add_bundle_to_dialer_rotary_group (hwidbtype *link_hwidb,
                                                      hwidbtype *virtual_bundle_hwidb) 
{

    hwidbtype *netidb;

    if (!is_ddr(virtual_bundle_hwidb) || !is_ddr(link_hwidb) ||
        !HUNTGROUP_MEMBER(link_hwidb->dialerdb))
        return(FALSE);

    netidb = GET_NETIDB(link_hwidb->dialerdb);

    add_to_hunt_group(virtual_bundle_hwidb->dialerdb,
                      netidb->unit, TRUE, netidb); 

    return(TRUE);

}

/*
 *  mlp_add_bundle_to_dialer_pool
 *  attempt to add the virtual bundle interface to the same pool
 *    as the first link in the bundle
 */

static boolean mlp_add_bundle_to_dialer_pool (hwidbtype *link_hwidb,
                                              hwidbtype *virtual_bundle_hwidb)
{
    hwidbtype *netidb;

    /*
     * By this time we are already bound to a dialer profile
     */
    netidb = get_netidb(link_hwidb);

    if (!is_ddr(link_hwidb) || !is_ddr(virtual_bundle_hwidb) ||
        !link_hwidb->dialerdb->dialer_bound)
        return(FALSE);

    if (!dialer_add_pool(virtual_bundle_hwidb->dialerdb,
			 netidb->dialerdb->dialer_pool->pool_number,
                         0,
                         0,
                         0)) {
        return(FALSE); 
    }

    /*
     *  Try to bind the virtual access interface.
     */

    if (!dialer_match_bound_ddb(virtual_bundle_hwidb->dialerdb,
                                link_hwidb->dialerdb)) {
        return(FALSE);
    }

    return(TRUE);

}

/*
 *  mlp_remove_bundle_from_dialer_pool
 *  Attempt to unbind a bundle interface from a dialer pool and
 *    remove it from the pool
 */
 
static boolean mlp_remove_bundle_from_dialer_pool (hwidbtype *bundle_hwidb)
{
 
    if (!is_ddr(bundle_hwidb))
        return(FALSE);

    /*
     *  this bundle interface is in a dialer pool, and bound to
     *    a dialer profile.  clean it up.
     */

    /*
     *  Remove the bundle from the dialer connected group so that
     *   the dialer does not forward data to the bundle interface
     *   anymore
     */

    if (bundle_hwidb->dialerdb->dialer_bound)
	dialer_unbind_profile(bundle_hwidb->dialerdb);

    dialer_remove_pool(bundle_hwidb->dialerdb, 
		       bundle_hwidb->dialerdb->dialer_pool->pool_number);
    
    return(TRUE);

}

/*
 * Create a Bundle interface for MLP bundle
 */
hwidbtype *mlp_create_bundle_interface (hwidbtype *link_hwidb,
					mlpbundletype *bundle)
{
    hwidbtype *virtual_bundle_hwidb, *netidb;
    dialerdbtype *netddb;
    int template_num = 0;
    int global_template_num;
    int status;
    lcp_state *bundle_lcpstate;
    boolean blocking, aaa_present;

    aaa_present = FALSE;

    /*
     * Get the local MLP Virtual template number if any
     * If the link_hwidb is a Vaccess interface, it itself would already 
     * be cloned from some template. E.g. for Multichassis Multilink, that
     * template is the same as the global multilink template. For Virtual 
     * Profiles, it would originate from the Virtual Profiles configs. 
     * For VPDN connections, the Vtemplate is defined in the  'vpdn 
     * incoming' command.  
     */
   global_template_num = mlp_get_vtemplate_num();
   if (link_hwidb->type == IDBTYPE_VACCESS) {
     template_num = link_hwidb->firstsw->vaccess_info->vtemplate->unit;

     /* 
      * Give a warning if global multilink template was defined and does
      * not match this template
      */ 
     if ((global_template_num != 0) && 
         (template_num != global_template_num) 
         && multilink_events_debug)
        buginf("\nMLP Ignoring Global Multilink Vtemplate "
               " %d, Vtemplate %d from %s in effect",
               global_template_num, template_num, 
               link_hwidb->hw_namestring);     
      
   } else
     template_num = global_template_num; 

    if ((template_num != 0) || is_ddr(link_hwidb) || aaa_present)
      blocking = FALSE;
    else
      blocking = TRUE;

    /*
     * Create a Bundle Interface (it is a Virtual Access Interface)
     * If a Virtual template, DDR or AAA is defined, don't block since it will 
     * be blocked later (in the clone vaccess operation) 
     * If there is a dialer interface behind this interface, do
     * not assign ip address to the bundle interface.
     * If there isn't a dialer interface behind it, 
     * assign an ip unnumbered to the bundle interface
     */
    
    virtual_bundle_hwidb = reg_invoke_vtemplate_create_vaccess(
                                     blocking,
				     is_ddr(link_hwidb) ? FALSE : TRUE,
				     VTEMPLATE_TYPE_MLP_BUNDLE);
    if (!virtual_bundle_hwidb)
	return(NULL);

    /*
     * If there is no template defined and no DDR is defined,
     * try to use the ip address or ip unnumbered on the link 
     * interface. 
     * If there is no ip address from the link intf, unnumbered 
     * from an intf found in reg_invoke_vtemplate_create_vaccess().
     */ 
     if((template_num == 0) && !is_ddr(link_hwidb)) {
	 if (link_hwidb->firstsw->ip_address)
	     virtual_bundle_hwidb->firstsw->ip_address =
		 link_hwidb->firstsw->ip_address;
	 else if (link_hwidb->firstsw->ip_unnumbered)
	     virtual_bundle_hwidb->firstsw->ip_unnumbered =
		 link_hwidb->firstsw->ip_unnumbered;
     }

       
   
    /*
     * Setup the PPP state on the interface
     */
    reg_invoke_media_serial_setup(ET_PPP, NULL, virtual_bundle_hwidb);

    bundle_lcpstate = virtual_bundle_hwidb->lcpstate;
    
    if (is_ddr(link_hwidb) && SLAVE_DIALER(link_hwidb->dialerdb)) {
	/*
	 *  The first link in the bundle is_ddr AND the first
	 *    link is either a huntgroup member or dialer pool member
	 *  Try to add the link to the dialer pool then the
	 *    huntgroup.
	 */
	
	/*
	 *  put the dialerdb structure on the bundle interface
	 */
	if (!init_dialer_idb(virtual_bundle_hwidb)) {
	    /*
	     *  For some reason we can't make this a dialer in-band
	     *    interface, probably no memory.  Punt.
	     */
	    reg_invoke_vtemplate_free_vaccess(virtual_bundle_hwidb);
	    return(NULL);
        }
	dialer_command_inband(TRUE, virtual_bundle_hwidb->dialerdb,
                              DIALER_INBAND_NOPARITY);
	/*
	 *  Try to add the bundle interface to a dialer pool
	 */  
	if (mlp_add_bundle_to_dialer_pool(link_hwidb,
					  virtual_bundle_hwidb)) {
	    if (multilink_events_debug)
		buginf("\nMLP %s: added to dialer pool %s", 
		       virtual_bundle_hwidb->hw_namestring,
		       virtual_bundle_hwidb->dialerdb->dialer_pool->ddb->ownidb->hw_namestring);
	} else if (mlp_add_bundle_to_dialer_rotary_group(link_hwidb,
						     virtual_bundle_hwidb)) {
	    if (multilink_events_debug)
		buginf("\nMLP %s: added to huntgroup %s", 
		       virtual_bundle_hwidb->hw_namestring,
		       virtual_bundle_hwidb->dialerdb->hunt_group_master->ownidb->hw_namestring);
	} else {
	    
	    /*
	     *  This else should never execute because of the SLAVE_DIALER
	     *    test above.  But you should never say never.
	     */
	    
	    if (multilink_events_debug)
		buginf("\nMLP %s: invalid dialer %s", 
		       virtual_bundle_hwidb->hw_namestring,
		       link_hwidb->hw_namestring);
	    
	    reg_invoke_vtemplate_free_vaccess(virtual_bundle_hwidb);
            mlp_kill_link(link_hwidb);
	    return(NULL);
	}
	
	
	netddb = GET_NETDDB(virtual_bundle_hwidb->dialerdb);
	netidb = GET_NETIDB(virtual_bundle_hwidb->dialerdb);

	/*
	 *  logging_events assignment is necessary to 
	 *     suppress messages after virtual_bundle_hwidb has 
	 *     been removed from ddr and/or free'd
	 */

	virtual_bundle_hwidb->logging_events =
	    netidb->logging_events;

	bundle->add_link_threshold = netddb->dialer_load_threshold;
	bundle->delete_link_threshold = netddb->dialer_load_threshold;
	bundle->wait_for_add_ticks = netddb->dialer_carrier_ticks;
	bundle->wait_before_disconnect_ticks = netddb->dialer_idle_ticks;
	bundle->thresh_direction = netddb->thresh_direction;
	virtual_bundle_hwidb->dialerdb->dialer_idle_ticks =
	    netddb->dialer_idle_ticks;
	virtual_bundle_hwidb->dialerdb->dialer_list = netddb->dialer_list;
	
	/*
	 *  Make sure dialer never tries to dial over this
	 *    over bundle interface.
	 */
	if (reg_invoke_dialer_is_answer(link_hwidb->dialerdb->dialer, 
					link_hwidb->dialerdb)) {
	    /*
	     *  We answered the call for this bundle.
	     *   The bundle interface is ready to rock.
	     */
	    set_dialer_state(virtual_bundle_hwidb->dialerdb,
			     DIALER_OPERATIONAL);

	} else {
	    /*
	     *  We dialed the call
	     *  Set dialer state to PENDING to allow dialer_statechange
	     *     to execute the DIALER_CALLING section and prevent
	     *     lcp authorization when dialing out
	     */
	    set_dialer_state(virtual_bundle_hwidb->dialerdb, DIALER_PENDING);

	}
	virtual_bundle_hwidb->dialerdb->dialer_placed_call = 
	    link_hwidb->dialerdb->dialer_placed_call;
	strcpy(virtual_bundle_hwidb->dialerdb->remote_name, 
	       link_hwidb->dialerdb->remote_name);
	strcpy(virtual_bundle_hwidb->dialerdb->number_called, 
	       link_hwidb->dialerdb->number_called);
	TIMER_START(virtual_bundle_hwidb->dialerdb->dialer_idle_timer,
		    virtual_bundle_hwidb->dialerdb->dialer_idle_ticks);
	TIMER_STOP(virtual_bundle_hwidb->dialerdb->dialer_enable_timer);
	virtual_bundle_hwidb->dialerdb->dialer_holdQ =
	    link_hwidb->dialerdb->dialer_holdQ;
	link_hwidb->dialerdb->dialer_holdQ = NULL;

        if (multilink_events_debug)
          buginf("\nMLP %s: Clone from %s", 
                virtual_bundle_hwidb->hw_namestring,
                netidb->hw_namestring);

        /*
         * If DDR then clone from dialer interface ONLY.
         * Block and don't filter PPP (not applicable) 
         * No cloning from Vtemplate/AAA
         */
        status = reg_invoke_vtemplate_clone_vaccess(VTEMPLATE_CLONE_DIALER,
                                         0, 
                                         netidb,
                                         NULL,
				         virtual_bundle_hwidb, 
                                         TRUE, 
                                         FALSE);
        if (status != VTEMPLATE_SUCCESS) {
	   if (multilink_events_debug)
	      buginf("\nMLP %s: Invalid Dialer cloning %s", 
		   virtual_bundle_hwidb->hw_namestring,
                   netidb->hw_namestring);
	   reg_invoke_vtemplate_free_vaccess(virtual_bundle_hwidb);
	   mlp_kill_link(link_hwidb);
	   return(NULL);
        }
    } else {

      /*
       * Bundle is not associated with Dialer at all.
       * Clone from Vtemplate
       */
      if (template_num !=0) {

           if (multilink_events_debug)
              buginf("\nMLP %s: Clone from Vtemplate %d block=%d",
                     virtual_bundle_hwidb->hw_namestring, 
                     template_num, !aaa_present);

           status = reg_invoke_vtemplate_clone_vaccess(
                                         VTEMPLATE_CLONE_VTEMPLATE,
                                         template_num, 
                                         NULL,
                                         NULL,
				         virtual_bundle_hwidb, 
                                         (aaa_present ? FALSE : TRUE),
                                         FALSE); 
         if (status != VTEMPLATE_SUCCESS) {
	    if (multilink_events_debug)
		buginf("\nMLP %s: Invalid Vtemplate cloning", 
		       virtual_bundle_hwidb->hw_namestring);
	    reg_invoke_vtemplate_free_vaccess(virtual_bundle_hwidb);
	    mlp_kill_link(link_hwidb);
	    return(NULL);
	}
      } else {
         if (multilink_events_debug)
	      buginf("\nMLP %s: No Virtual template defined - check configs", 
		   virtual_bundle_hwidb->hw_namestring);
      }

      /*
       * Clone from AAA last.
       * Always BLOCK
       */
      if (aaa_present) {

        if (multilink_events_debug)
            buginf("\nMLP %s: Clone from AAA",
                     virtual_bundle_hwidb->hw_namestring);

        status = reg_invoke_vtemplate_clone_vaccess(VTEMPLATE_CLONE_AAA,
                                         0, 
                                         NULL,
                                         NULL,  /* should be buffer address */
				         virtual_bundle_hwidb, 
                                         TRUE, 
                                         FALSE);    
        if (status != VTEMPLATE_SUCCESS) {
	    if (multilink_events_debug)
		buginf("\nMLP %s: Invalid AAA cloning", 
		       virtual_bundle_hwidb->hw_namestring);
	    reg_invoke_vtemplate_free_vaccess(virtual_bundle_hwidb);
	    mlp_kill_link(link_hwidb);
	    return(NULL);
	}   
      }

    }
     
    /*
     *  Update bundle variables if cloned config changed them
     */
    if (bundle_lcpstate && bundle_lcpstate->mlp_threshold_confg) {
	    bundle->add_link_threshold = bundle_lcpstate->mlp_threshold;
	    bundle->delete_link_threshold = bundle_lcpstate->mlp_threshold;
	    bundle->thresh_direction = 
		bundle_lcpstate->mlp_threshold_direction;
    }

    bundle->min_links = bundle_lcpstate->mlp_min_links;
    bundle->max_links = bundle_lcpstate->mlp_max_links;
    bundle->max_frags = bundle_lcpstate->mlp_max_frags;
    if (bundle->max_frags == 1)
	    bundle->is_frag = FALSE;

    /*
     * Propagate the status and bandwidth from the member link
     * to the bundle interface
     */
    virtual_bundle_hwidb->firstsw->visible_bandwidth = 
	link_hwidb->firstsw->visible_bandwidth;
    
    /*
     * Create a ustruct for the bundle interface which now represents 
     * interface now represents all the member interfaces
     * If ustruct is present, AAA was enabled on the 1st link. therefore
     * duplicate that ustruct in the bundle interface.
     * Whether AAA was enabled or not (old_access_control=FALSE/TRUE), 
     * the remote_name on the 1st link is always the remote client's name.
     * Duplicate that also.
     */
    if (bundle_lcpstate && link_hwidb->lcpstate->ustruct) {
	if(!aaa_dup_user(&link_hwidb->lcpstate->ustruct, 
                       &bundle_lcpstate->ustruct,
                       AAA_SVC_ENABLE, PRIV_USER)) {
	    reg_invoke_vtemplate_free_vaccess(virtual_bundle_hwidb);
	    mlp_kill_link(link_hwidb);
	    return(NULL);
	} else {
	    bundle_lcpstate->ustruct->no_acct = TRUE;
	}
    }

    /*
     * If the underlying interface goes down for whatever reason
     * (e.g. Callback might have disconnected the physical interface
     *  while the clone operation was being carried out
     */
    if (!interface_up_simple(link_hwidb->firstsw)) {

	if (multilink_events_debug)
	    buginf("\nMLP First link %s DOWN, deleting Bundle interface %s",
		   link_hwidb->hw_namestring,
		   virtual_bundle_hwidb->hw_namestring);

	reg_invoke_vtemplate_free_vaccess(virtual_bundle_hwidb);
	return(NULL);
    }

    /*
     * Set up the holdq_enqueue vector to catch datagram_out to the 
     * the bundle interface.  
     * Set the MLP reset vector to catch resets to this interface.
     */
    virtual_bundle_hwidb->oqueue = mlp_holdq_enqueue;
    virtual_bundle_hwidb->reset = mlp_bundle_reset;
    virtual_bundle_hwidb->fastsend = mlp_fastsend;
    return(virtual_bundle_hwidb);
    
}

/*
 * Make the bundle interface come up on PPP  
 */ 
void mlp_bundle_interface_up (hwidbtype *bundle_hwidb)
{
    lcp_options *go;
    fsm *f;
    
    f = bundle_hwidb->lcpfsm;
    go = f->gotoptions;
    
    /*
     * The LCP is now in OPEN state
     */
    bundle_hwidb->lcpfsm->state = OPEN;
    go->neg_numbered_mode = 0;
    go->neg_asyncmap = 0;
    go->neg_auth = 0;

    /*
     * Make the LCP come up
     */
    if (bundle_hwidb->lcpfsm->callbacks->up)
       (*bundle_hwidb->lcpfsm->callbacks->up)(bundle_hwidb->lcpfsm);
}


/*
 * Bind the interface with mlp vector
 */
void mlp_bind_interface (hwidbtype *link_hwidb)
{
     link_hwidb->firstsw->vpn_save_iqueue = link_hwidb->iqueue;
     link_hwidb->iqueue = mlp_raw_enqueue;
}

/*
 * Unbind the interface 
 */
void mlp_unbind_interface (hwidbtype *link_hwidb)
{
  if (link_hwidb->firstsw->vpn_save_iqueue) {
     link_hwidb->iqueue = link_hwidb->firstsw->vpn_save_iqueue;
     link_hwidb->firstsw->vpn_save_iqueue = NULL;
  }    
}

/*
 *  Take the link down immediatley and without question.
 *  Send a terminate request
 */

static void mlp_kill_link (hwidbtype *idb)
{

    multilink_down(idb);
    lcp_close(idb->lcpfsm);

    /*
     * If this is a Virtual Access Interface, explicitly reset it.
     */
    if (idb->type == IDBTYPE_VACCESS)
       (*idb->reset)(idb);
}

/*
 * Bundle reset
 */
void mlp_bundle_reset (hwidbtype *bundle_hwidb)
{
    mlpbundletype *bundle;
    idbtype *dialer_swidb;
    mlplinktype *temp_link, *next_link, *master_link;
    boolean save_blocking_disabled;

    if (!bundle_hwidb->firstsw->is_bundle_interface)
       return;

    /*
     *  Don't reset if prevent_reset flag is set
     */
    if (is_vaccess_prevent_reset(bundle_hwidb->firstsw))
	return;

    /*
     *  !!!!!!!!!!!!!!!  Start of critical section
     */
    save_blocking_disabled = blocking_disabled;
    blocking_disabled = TRUE;

    bundle_hwidb->firstsw->is_bundle_interface = FALSE;

    /*
     * Mark Bundle as a down
     */
    if (bundle_hwidb->state != IDBS_ADMINDOWN)
      net_cstate(bundle_hwidb, IDBS_DOWN);

    /*
     * Find the bundle, given the bundle interface
     */ 
    if (!bundle_hwidb->mlp_link || !bundle_hwidb->mlp_link->bundle)
       return;
    bundle = bundle_hwidb->mlp_link->bundle;

    if (multilink_events_debug)
      buginf("\nMLP %s: Bundle reset", bundle->name);

    /*
     * Free the master link (this link is not connected to other 
     * links)
     */    
    master_link = bundle_hwidb->mlp_link;
    bundle_hwidb->mlp_link = NULL;
    bundle->master_link = NULL;
    
    /*
     * Go thru all member interfaces, remove them from the bundle
     *  and reset them.
     * Be careful here because mlp_kill_link may free bundle data.
     * If mlp_kill_link suspends this code may crash if a links
     *    is disonnceted during the suspensions.  At this time,
     *    I have not discovered a suspend in mlp_kill_link.
     */
    temp_link = bundle->first_link;
    while(temp_link) {
	next_link = temp_link->next;
	mlp_kill_link(temp_link->idb);
	temp_link = next_link;
    }

    /*
     *  Free the bundle master_link data only after the bundle
     *    is completely cleaned up.
     */
    free(master_link);    
    master_link = NULL;
 
    blocking_disabled = save_blocking_disabled;

    /*
     * !!!!!!!!!!!!!!!  End of critical section
     */
    
    /*
     *  If bundle interface was a member of a hunt group remove it
     *     from the hunt group and remove dialer in-band
     */
    if (is_ddr(bundle_hwidb)) {
        /*
         * Remove any route that was installed on the dialer interface
         * Why remove here and not wait for ipcp to catch it ?
         * Because IPCP hasn't gotten around to catching it and we'll 
         * be removing dialerdb pointer (below), causing IPCP to not 
         * see a dialer interface and remove the route accordingly
	 * We also need to make certain that we remove any dynamic
	 * dialer maps that are configured.
         */
        dialer_swidb = get_netcfg_idb(bundle_hwidb->firstsw);
 	if (dialer_swidb && bundle_hwidb->firstsw->ip_peer) {
	    reg_invoke_clear_ip_peer_addr(bundle_hwidb->firstsw);
	}

	
	if (DIALPOOL_MEMBER(bundle_hwidb->dialerdb)) {
	    
	    mlp_remove_bundle_from_dialer_pool(bundle_hwidb);

	} else if (HUNTGROUP_MEMBER(bundle_hwidb->dialerdb)) {
	    add_to_hunt_group(bundle_hwidb->dialerdb, 0, FALSE, NULL);
	}
	
	/*
	 *  If bundle interface was dialer in-band, remove dialer in-band
	 */
	dialer_command_inband(FALSE, bundle_hwidb->dialerdb,
			      DIALER_INBAND_NOPARITY);
	/*
	 *  bundle_hwidb->dialerdb has been freed by dialer_command_inband
	 *  Don't use bundle_hwidb->dialerdb after this point
	 */
    }

    /*
     * Free the interface 
     */
    reg_invoke_vtemplate_free_vaccess(bundle_hwidb);

}

 
static mlplinktype * mlp_create_link (void)
{

    mlplinktype *link;

    link = malloc_named(sizeof(mlplinktype), "MLP link");
    if (!link) {
	return(NULL);
    }

    return(link);

}

/*
 * mlp_prepend_bundle
 * Put the bundle at the beginning of the bundle list
 */

static void mlp_prepend_bundle (mlpbundletype *bundle)
{

    if (!bundle) {
	if (multilink_events_debug)
	    buginf("\nMLP: Can't add a null bundle");
	return;
    }

    bundle->next = mlp_bundle_list_head;
    bundle->prior = NULL;
    mlp_bundle_list_head = bundle;
    
    if (bundle->next)
	bundle->next->prior = bundle;

}

/*
 *  mlp_get_link_bandwidth
 *  get the bandwidth for the link
 */

static inline ulong mlp_get_link_bandwidth (mlplinktype *link)
{
    return(link->idb->firstsw->visible_bandwidth);
}

/*
 *  Add the link to the end of the bundle link list
 */

static void mlp_append_link (mlpbundletype *bundle, mlplinktype *link)
{
    if (!link) {
	if (multilink_events_debug)
	    buginf("\nMLP %s: Can't add a null link", bundle->name);
	return;
    }

    if (!bundle->first_link) {
	/*
	 * This is the first link in the bundle.
	 */
	bundle->first_link = link;
	bundle->last_link = link;
	link->prior = NULL;
	link->next = NULL;
    } else {
	/*
	 *  Put link at the end of the list.
	 */
	link->next = NULL;
	link->prior = bundle->last_link;
	bundle->last_link->next = link;
	bundle->last_link = link;
    }
    bundle->nlinks++;
}

/*
 *  kill all fragments on the unassigned fragment list
 */

static void mlp_kill_una_frag_list (mlpbundletype *bundle)
{
    paktype *una_frag, *next_frag;

    una_frag = bundle->una_frag_list;

    while(una_frag) {
	next_frag = una_frag->mlp_next_frag;
	datagram_done(una_frag);
	una_frag = next_frag;
    }

    bundle->una_frag_list = NULL;
}

/*
 * mlp_remove_bundle
 * remove a bundle from the bundle list
 */

static void mlp_remove_bundle (mlpbundletype *bundle)
{
    hwidbtype *tmpidb;
    mlpbundletype *next, *prior;

    if (!bundle) {
	if (multilink_events_debug)
	    buginf("\nMLP: Can't remove a null bundle");
	return;
    }

    if (multilink_events_debug)
	buginf("\nMLP %s: Removing bundle", bundle->name);

    next = bundle->next;
    prior = bundle->prior;

    if (prior)
	prior->next = next;
    else
	mlp_bundle_list_head = next;

    if (next)
	next->prior = prior;

    /*
     *  Clean up data structures that the bundle points to.
     */
	
    mlp_kill_una_frag_list(bundle);
    if (bundle->working_pak)
	datagram_done(bundle->working_pak);

    /*
     * If there is a Master Bundle Interface, free it and the link also
     * (the link is not chained with the other links in the bundle)
     * bundle->master_link test is needed because this can be called
     *   from the bundle_reset code
     */
    if (bundle->master_link && 
        bundle->master_link->idb->firstsw->is_bundle_interface) {
       tmpidb = bundle->master_link->idb;
       bundle->master_link = NULL;
       (*tmpidb->reset)(tmpidb);
    }

}

static void mlp_remove_link (mlplinktype *link)
{

    mlplinktype *next, *prior, *master_link;
    ulong new_bandwidth;

    if (!link) {
	if (multilink_events_debug)
	    buginf("\nMLP: Can't remove a null link");
	return;
    }

    if (multilink_events_debug)
	buginf("\nMLP %s: Removing link %s",
	       link->bundle->name ,link->idb->hw_namestring);

    next = link->next;
    prior = link->prior;

    if (prior)
	prior->next = next;
    else
	link->bundle->first_link = next;

    if (next)
	next->prior = prior;
    else
	link->bundle->last_link = prior;
	
    if (link->bundle->next_xmit_link == link)
	link->bundle->next_xmit_link = link->bundle->first_link;

    link->bundle->nlinks--;

    master_link = link->bundle->master_link;
    if (master_link) {
	/*
	 *  Check master link first because bundle_reset sets it
	 *   to NULL.
	 */  
	new_bandwidth = mlp_get_link_bandwidth(master_link) -
	    mlp_get_link_bandwidth(link);
	/*
	 * Can't allow master's visible bandwidth to goto
	 * zero. Since the master is still UP and we can't
	 * have a UP idb with zero visible bandwidth.
	 */
	if (new_bandwidth != 0) {
	    /*
	     *  changing the interface bandwidth may try to reset the bundle
	     *    interface when bridging.  This is bad.  Prevent
	     *    this call from resetting the interface for
	     *    all cases.
	     */
	    set_default_interface_bandwidth(master_link->idb->firstsw,
					    new_bandwidth);
	}
    }

    mlp_unbind_interface(link->idb); 
}

/*
 * mlp_find_bundle_name
 * search the bundle list looking for a bundle with a certain
 *  name and return that bundle pointer.
 * NULL strings are not searched.
 */
static mlpbundletype * mlp_find_bundle_name (char *name)
{

    mlpbundletype *bundle;

    bundle = mlp_bundle_list_head;

    if (!name)
	return(NULL);

    while (bundle && (strcmp(bundle->name, name) != 0)) {
	bundle = bundle->next;
    }
    return(bundle);
}

/*
 * mlp_find_bundle_netidb
 * return the bundle net_idb if it is in a dialer hunt group
 *   or bound to a dialer profile.
 * Also return the idb of the first link in the bundle if 
 *  it is ddr.
 * Otherwise return NULL
 */
hwidbtype *mlp_find_bundle_netidb (char *name, hwidbtype **ptr_link_hwidb)
{
   mlpbundletype *bundle;
   hwidbtype *master_idb;

   *ptr_link_hwidb = NULL;
    
   /*
    * Search for Bundle by name
    */
   bundle = mlp_find_bundle_name(name);

   /*
    * Find the dialer interface (if any)
    * A dialer interface is one which is not the same as the Bundle 
    * interface
    */
   if (bundle && bundle->master_link && 
      is_ddr(bundle->master_link->idb)) {
      master_idb = get_netidb(bundle->master_link->idb);      
      /*
       * make sure this is ddr interface which is in a hunt group
       */
      if (master_idb != bundle->master_link->idb) {
	  if (bundle->first_link && is_ddr(bundle->first_link->idb)) {
	      *ptr_link_hwidb = bundle->first_link->idb;
	  }
	  return(master_idb);
      }
   }
   return(NULL);
}

/*
 * mlp_get_bundle_list
 * return the total number of bundles and a list of the bundles.
 * The bundle list is  a list of ACSII NULL term. bundle names in one
 * malloced continuous buffer.
 * It is the caller's responsiblity to free the bundle list, after
 * its use. 
 */
void mlp_get_bundle_list(int  *bundle_cnt, char **bundle_list, int *list_len)
{
    char *listptr;
    mlpbundletype *bundle;
    int    buf_len;
    int name_len;
    
    buf_len = 0;
    *bundle_list = NULL;
    *bundle_cnt = 0;
    *list_len = 0;

    bundle = mlp_bundle_list_head;  
    /*
     * If there is no bundle, return now
     */
    if (!bundle)
	return;

    /*
     * Count the bundle list size
     */
    while (bundle) {
	name_len = strlen(bundle->name)+1;
	buf_len += name_len;
	bundle = bundle->next;
    }

    /*
     * Return an empty list, if we can't malloc buffer
     */
    *bundle_list = malloc_named(buf_len, "MLP Get bundle list");

    if (!(*bundle_list))
	return;

    listptr = *bundle_list;
    bundle = mlp_bundle_list_head;
    *list_len = buf_len;

    /*
     * Walk thru the bundle structure, copy the bundle names
     * into the bundle list.
     */
    while (bundle) {
	name_len = strlen(bundle->name)+1;
	/*
	 * Copy the individual bundle names to the bundle list
	 */
	(*bundle_cnt)++;
	strcpy(listptr, bundle->name);
	listptr[name_len - 1] = 0;
	listptr += name_len;
	bundle = bundle->next;
    }
}

/*
 * mlp_reset_bundle
 * Reset an existing bundle
 * Return TRUE if the bundle is found, else FALSE
 */
boolean  mlp_reset_bundle(char *bundle_name)
{
    mlpbundletype *bundle;
    hwidbtype *tmpidb;

    bundle = mlp_bundle_list_head;

    while (bundle) {
	if (bundle->name && bundle_name) {
	    if (!strcmp(bundle->name, bundle_name)) {
		/*
		 * Good we found the bundle, check does
		 * it has a master link
		 * if yes, call the idb->reset function
		 * if no, call mlp_remove_bundle() to
		 * free the bundle structure
		 */
		if(bundle->master_link && bundle->master_link->idb) {
		    tmpidb = bundle->master_link->idb;
		    (*tmpidb->reset)(tmpidb);
		}
		else {
		    mlp_remove_bundle(bundle);
		    free(bundle);
		}
		return TRUE;
	    }
	}
	bundle = bundle->next;
    }

    return FALSE;
}


/*
 * mlp_search_bundle
 * Query for whether a bundle exists on local host
 * Args:
 *   Input:
 *     bundle_name - ptr to null-terminated bundle_name
 *     max         - bid value for mastership claim (bundle exists)
 * Return 
 *    max                     : bundle exists.
 *    0..(max-1)	      : count of bundles held; if count
 *			        exceeds (max-1), return (max-1).
 */
int mlp_search_bundle (char *bundle_name, int max)
{
    mlpbundletype *bundle;
    int bcnt;
    
    if (mlp_find_bundle_name(bundle_name))
	return (max);  /* mastership claim */
    else {
	bundle    = mlp_bundle_list_head;
	bcnt      = 0;
	while (bundle) {
	    bcnt++;
	    bundle = bundle->next;
	}
	return (bcnt >= max ? (max - 1) : bcnt);
    }
}
/*
 *  mlp_is_callback_client
 *  return (TRUE) if the bundle is a callback client
 */

static inline boolean mlp_is_bundle_callback_client (mlpbundletype *bundle)
{
    return(bundle->callbacktype == CALLBACK_CLIENT);
}

/*
 *  mlp_set_bundle_callbacktype
 *  sets the callbacktype field in the bundle structure to the correct 
 *   value
 */
static inline void mlp_set_bundle_callbacktype (mlpbundletype *bundle)
{
    hwidbtype *master_idb;
    dialerdbtype *ddb;
    
    master_idb = bundle->master_link->idb;
    if (master_idb) {
	ddb = get_dialerdb(master_idb);
	if (ddb != NULL)
	    bundle->callbacktype = ddb->callback_type;
    }
}

/*
 * mlp_appoint_bundle
 * Appointment of a MLP bundle which does not yet exist.
 * MLP to honor subsequent calls to mlp_search_bundle with affirmative 
 * on the same bundle name. MLP is return with minimal latency. 
 */
void mlp_appoint_bundle (char *bundle_name)
{
    mlpbundletype *bundle;

    /*
     * Check if the bundle already exists
     */
    bundle = mlp_find_bundle_name(bundle_name);

    /*
     * If it already exists, the appointment is already fullfilled
     */
    if (bundle) {
	if (multilink_events_debug)
	    buginf("\nMLP %s: Bundle appointed (implicit)", bundle_name);
	return;
    }

    /*
     * Create a bundle. 
     * If this fails, currently not implemented to inform caller
     * of failure. Caller always assumes appointment is successful.
     * However, if this happens, the box is in trouble anyway.
     */ 
    bundle = mlp_create_bundle();
    if (!bundle) {
	errmsg(&msgsym(NOAPPOINT, PPP), bundle_name);
	return;
    }
    
    if (multilink_events_debug)
	buginf("\nMLP bundle %s appointed", bundle_name);
    
    /*
     * Initialize the bundle and start timer on it.
     * Bundle is removed on timeout. If for some reason
     * the appointment is still kept and the bundle is still 
     * formed, the bundle (a newly created one) will still be formed 
     * in this node. This is probably not good since another stack member
     * may have been appointed in the interim. However, the timeout is
     * unnusually large. We are just trying make sure that a
     * runaway condition does not end up appointing bundles that
     * are do not meet the appointment
     */ 
    mlp_init_bundle(bundle, MLP_BUNDLE_APPOINTED);
    strcpy(bundle->name, bundle_name);
    TIMER_START(bundle->appointment_timer, MLP_WAIT_BUNDLE_APPOINTMENT);
    mlp_prepend_bundle(bundle);
}

/*
 *  mlp_add_link
 *  idb = hwidb of interface being added to a bundle
 *  first = output inidicating if link is first in a bundle
 *  return(mlpbundltype *) != NULL 
 *    link added to bundle and bundle created if first link
 */

static mlpbundletype * mlp_add_link (hwidbtype *idb, boolean *first)
{
    mlpbundletype *bundle;
    mlplinktype *link, *bundle_link;
    char *name, *remote_name;
    dialerdbtype *ddb;
    hwidbtype *virtual_bundle_hwidb;
    char default_name[1] = {0x0};
    ulong new_bandwidth;
    sys_timestamp state_time;

    virtual_bundle_hwidb = NULL;
    bundle_link = NULL;
    *first = FALSE;
    bundle = NULL;
    new_bandwidth = 0;

    if (is_multilink(idb))
	return(idb->mlp_link->bundle);

    /*
     *  Find the name of the bundle
     *  make sure remote name or callerid is available with remote name
     *    having precedence.
     */
    ddb = get_dialerdb(idb);
    remote_name = ppp_get_remote_name(idb);
    if (remote_name && (remote_name[0] != '\0'))
	name = remote_name;
    else if ((ddb != NULL) && (ddb->number_called[0] != '\0'))
	name = ddb->number_called;
    else {
	if (multilink_events_debug)
	    buginf("\nMLP %s: No name found, using NULL string",
		   idb->hw_namestring);
	name = default_name;
    }

    /*
     *
     *  When a link is added there a few structures that
     *    may be created.  These structures need to be freed if
     *    the code executes return(NULL) after their creation
     *  If you add a return(NULL) make sure you free them.
     *
     *  mlplinktype for the added link
     *  mlpbundletype
     *  mlplinktype for the bundle master_link
     *  virtual_bundle_hwidb interface for the bundle
     *
     */
    
    link = mlp_create_link();
    if (!link) {
	return(NULL);
    }

    bundle = mlp_find_bundle_name(name);
    if (!bundle || (bundle->nlinks == 0)) {

	/*
	 *  This is the first link for this bundle;
	 *  Bundle does not exist OR bundle was appointed
	 *
	 */
	
	if (!bundle) {
	    /*
	     *  Create the bundle data structure
	     */
	    bundle = mlp_create_bundle();
	    if (!bundle) {
		free(link);
		return(NULL);
	    }
	    
	    mlp_init_bundle(bundle, MLP_BUNDLE_ACTIVE);
	    mlp_prepend_bundle(bundle);
	    strcpy(bundle->name, name);
	} else {
	    /*
             * Change the state of the appointed bundle to active
             */
	    TIMER_STOP(bundle->appointment_timer);
	    bundle->state = MLP_BUNDLE_ACTIVE;
        }

	bundle_link = mlp_create_link();
	if (!bundle_link) {
	    free(link);
	    mlp_remove_bundle(bundle);
	    free(bundle);
	    return(NULL);
	}

        /*
         * The state time does not change unless there is an explicit 
         * interface state change on the requesting interface -
         * save the state time of the member interface.
         */
        state_time = idb->lcpstate->link_transition;
          
        /*
         * Create a virtual bundle interface.
         * Mark the member interface to indicate Bundle Interface is 
         * being created. Unmark it when it is done. 
         * This is required since the bundle creation will block.
         * A separate thread can then come in on the member interface 
         * thru ppp_input and fail the is_multilink test. 
         * We do not want it to then start NCP on the member link. 
         * This flag indicates that. 
         */
	virtual_bundle_hwidb = mlp_create_bundle_interface(idb, bundle);

        /*
         * If the Bundle interface creation fails, clean up.
         * OR
         * If the requesting interface changed state, clean up.
         * Other than the bundle joining the global bundle list, there 
         * has been no attachment of the interim links to the requesting 
         * interface. Therefore, we are free to clean up by merely freeing
         * allocated structures.
         */
	if (!virtual_bundle_hwidb ||
            TIMERS_NOT_EQUAL(state_time, idb->lcpstate->link_transition)) {

            /* 
             * Immediately invalidate all queued entries in PPP Manager and PPP
             * Auth Background pertaining to this interface
             */
            GET_TIMESTAMP(idb->lcpstate->link_transition);
	    if (multilink_events_debug)
		buginf("\nMLP %s: Bundle failed in creation/cloning",
		       idb->hw_namestring);
	    free(link);
	    free(bundle_link);
	    mlp_remove_bundle(bundle); 
	    free(bundle);

            /*
             * Free the Bundle interface in the case of state change
             */
            if (virtual_bundle_hwidb)
               reg_invoke_vtemplate_free_vaccess(virtual_bundle_hwidb);
	    return(NULL);
	} 

	*first = TRUE;
    } else {
	/*
	 *  Bundle already exists.  We are adding another link
	 */

	/*
	 *  Callback on links following the first link poses an
	 *   interesting problem.
	 *  If I add the link to the bundle, the multilink code will
	 *   immediately begin sending fragments down the link.
	 *   However, the callback server on the remote end is about
	 *   to disconnecte the link and in all likely hood will not
	 *   process any of the incoming fragments.
	 */

	/*
	 *  Set the callback type for the bundle
	 */
	if (bundle->nlinks == 2)
	    mlp_set_bundle_callbacktype(bundle);

	/*
	 *  So if the proposed new link is not the first link and
	 *   and the bundle is a callback client and callback
	 *   was negotiated on the current link indicating it is
	 *   the return call, do not add the link to a bundle
	 */

	if (mlp_is_bundle_callback_client(bundle) && 
	    ppp_is_callback_negotiated(idb)) {
	    return(NULL);
	}

	/*
	 *  Check for max links threshold before allowing link to be akked
	 *   to the bundle.
	 *  Favor lease lines (non-dialed). 
	 *  If idb is not dialer, then allow it to be added but check
	 *    for max links and delete a dialer link later.
	 *  ????? DLB need to change for non-dialer answering interfaces
	 *    as well.
	 */

	if ((bundle->nlinks >= bundle->max_links) && is_ddr(idb)) {
	    if (multilink_events_debug)
		buginf("\nMLP %s: %s exceeds maximum links",
		       bundle->name, idb->hw_namestring);
	    mlp_kill_link(idb);
	    return(NULL);
	}
    }

    mlp_init_link(link, bundle, idb);
    mlp_append_link(bundle, link);
    if (*first == TRUE) {
	bundle->next_xmit_link = link;
	/*
	 * Need to create the Master link
	 * and make the bundle master link point here.
	 */

	bundle->master_link = bundle_link;
	mlp_init_link(bundle_link, bundle, virtual_bundle_hwidb);
	virtual_bundle_hwidb->mlp_link = bundle_link;
	virtual_bundle_hwidb->firstsw->is_bundle_interface = TRUE;
	
	/*
	 * If there was an installed route and assigned addresses, 
	 * remove it.  As well as any dynamic dialer maps.
	 */
	if (idb->firstsw->ip_peer) {
	    reg_invoke_clear_ip_peer_addr(idb->firstsw);
	}

        /*
         * Prevent the Bundle from being reset in its interim state
         * Note: This means that in this interim state, a reset is a noop.
         *       What we need is probably to remember the reset and actually
         *       reset after the block.
         */
        set_vaccess_prevent_reset(virtual_bundle_hwidb->firstsw);

	/*
	 *  Call to mlp_bundle_interface_up must occur after
	 *    initializing virtual_bundle_hwidb->mlp_link.  Because
	 *    multilink_up is eventually called and would result in
	 *    an recursive loop of mlp_bundle_interface_up calls multilink_up
	 *    which calles mlp_bundle_interface_up and so on.
	 */
	mlp_bundle_interface_up(virtual_bundle_hwidb);

        /*
         * Reenable Bundle resets
         */
        clear_vaccess_prevent_reset(virtual_bundle_hwidb->firstsw);

        /*
         * If the requesting interface changed state, back pedal and clean up 
         * this thread. Note that since we have the bundle already set up, 
         * we clean up the bundle by simply calling  mlp_remove_bundle. 
         * Since the requesting the bundle's master_link is cleared, 
         * that routine does not clean up the link and the requesting 
         * interface. That is what we *WANT*, since that interface may 
         * very well have transitioned back to UP, and already have PPP 
         * packets in the PPP queue waiting behind this thread (PPP manager) 
         * waiting to come up. 
         */
        if (TIMERS_NOT_EQUAL(state_time, idb->lcpstate->link_transition)) {

           /* 
            * Immediately invalidate all queued entries in PPP Manager and PPP
            * Auth Background pertaining to this interface
            */
           GET_TIMESTAMP(idb->lcpstate->link_transition);

           if (multilink_events_debug)
	      buginf("\nMLP %s: Bundle interface failed in bundle bringup",
		       idb->hw_namestring);

           bundle->master_link = NULL;
           free(bundle_link);
           mlp_remove_bundle(bundle);
           free(bundle);
           free(link);
           virtual_bundle_hwidb->mlp_link = NULL;
           reg_invoke_vtemplate_free_vaccess(virtual_bundle_hwidb);
	   return(NULL);
        }

	if (idb->authstate->remote_name) {
	    if (virtual_bundle_hwidb->authstate->remote_name)
		free(virtual_bundle_hwidb->authstate->remote_name);
	    virtual_bundle_hwidb->authstate->remote_name = 
		strdup_named(idb->authstate->remote_name, "MLP remname"); 
	}

    } else {
	/*
	 *  This is not the first link
	 */  

	/*
	 *  update the bandwidth for the bundle
	 */
	
	new_bandwidth = mlp_get_link_bandwidth(bundle->master_link) +
	    idb->firstsw->visible_bandwidth;
	/*
	 *  changing the interface bandwidth may try to reset the bundle
	 *    interface when bridging.  This is bad.  Prevent
	 *    this call from resetting the interface for
	 *    all cases.
	 */
	set_default_interface_bandwidth(bundle->master_link->idb->firstsw,
					new_bandwidth);

	/*
	 * For accounting sake, add Framed-IP-Address info for all ustructs
	 * that is associated with a physical link.  The first link gets
	 * its Framed-IP-Address in ipcp_add_p2p_neighbor().
	 */
	if (idb && idb->lcpstate) {
	    userstruct *user = idb->lcpstate->ustruct;
	    ipaddrtype neighbor = bundle->master_link->idb->firstsw->ip_peer->ippeer_addr;

	    reg_invoke_aaa_acct_netinfo(user, AAA_ATTR_PROTO,
					"ip", AAA_TYPE_STR, 2, FALSE);
	    reg_invoke_aaa_acct_netinfo(user, AAA_ATTR_ADDR, &neighbor,
					AAA_TYPE_IP, sizeof neighbor, TRUE);
	}

    }
            

    idb->mlp_link = link;

    /*
     * If the Master link is a Bundle Interface, bind the interface
     * so that its raw_enqueue is MLP intelligent - i.e. redirect 
     * the packet to the Bundle Interface when necessary
     */
    if (bundle->master_link->idb->firstsw->is_bundle_interface) {

	/*
	 * If there was an an assigned addresses, remove it
	 * This code also removes any neighbor routes and
	 * dynamic dialer maps that may have been set.
	 */
	if (idb->firstsw->ip_peer) {
	    reg_invoke_clear_ip_peer_addr(idb->firstsw);
	}
	
	mlp_bind_interface(idb);
    }

    return(bundle);
}

static void mlp_check_max_links (mlpbundletype *bundle)
{

    if (bundle->nlinks > bundle->max_links)
	mlp_disconnect_link(bundle);


}

/*
 * mlp_process_pending_ncps
 *
 * Process pending NCP packets received on a MLP (negotiated)
 * link interface during authenticating phase.
 * this function is called in 3 places
 * - multilink_up() right after the bundle is built and up
 * we redirect any pending NCP packet to the bundle interface
 * - lcp_down() and
 * - lcp_closed(), 
 * we check if there is any pending NCP packets that associate 
 * with the downing interface then datagram_done them.
 *
 * Parameter:
 * hwidb: bundle interface that packet redirect to, if redirect is TRUE.
 *        link interface that is going down, if redirect is FALSE.  
 *    
 */
void mlp_process_pending_ncps (hwidbtype *hwidb, boolean redirect)
{
    int protocol = 0;
    int count, queue_size;
    paktype *pak;

    queue_size = QUEUESIZE(&mlp_pending_ncpQ);
    if (multilink_detailed_debug)
	buginf("\nMLP pending ncpQ size is %d", queue_size);

    for (count = 0; count < queue_size; count++) {
	pak = pak_dequeue(&mlp_pending_ncpQ);

	if (!pak->if_input) {
	    datagram_done(pak);
            continue;
	}

        /*
         * If interface transitioned whilst the packet was in the queue,
         * the packet is basically invalid. Discard it silently.
         */
        if (TIMERS_NOT_EQUAL(pak->gp_timer, 
                         pak->if_input->hwptr->lcpstate->link_transition)) {
           if (ppp_error_debug || multilink_events_debug)
              buginf("\nMLP %s transition interfaced, discarding packet",
                         pak->if_input->namestring); 
           datagram_done(pak);
           continue;
	}

	protocol = ppp_get_protocol_type(pak);	

	if (redirect && is_multilink(pak->if_input->hwptr) &&
		 hwidb== mlp_get_master_idb(pak->if_input->hwptr)) {
	    /* 
             * if redirect, check if we should redirect the packets
	     * to the bundle interface.
	     */
	    if (multilink_detailed_debug)
		buginf("\nMLP Redirect protocol %x packet from %s to %s",
		       protocol, pak->if_input->namestring, 
		       hwidb->hw_namestring);
	    pak->if_input = hwidb->firstsw;

            /*
             * Setup the pak timer to be the same as the Bundle 
             * interface transition timer - we do this since we
             * are bypassing the ppp_enqueue call
             */
           if (pak->if_input->hwptr->lcpstate)
              pak->gp_timer = pak->if_input->hwptr->lcpstate->link_transition; 

	    reg_invoke_ppp_continue_vpn(protocol, pak, hwidb);

	} else if (!redirect && hwidb == pak->if_input->hwptr) {
	    /*
	     * if not redirect, it must be a downing interface, check to
	     * see if there are any unused pending NCP packet,
	     * datagram_done if there is
	     */
	    if (multilink_detailed_debug)
		buginf("\nMLP  %s interface is down, free pending prot %x packet",
		       pak->if_input->namestring, protocol);
	    datagram_done(pak);
	} else {
	    /*
	     * this packet must be related to some other interfaces
	     * so put it back into the queue.
	     */
	    pak_enqueue(&mlp_pending_ncpQ, pak);
	}
    } 
}

/*
 *  intialize multilink stuff
 *  return TRUE if bundles exists and NCP's do not need to negotiate
 *  return FALSE to indicate that NCP's need to proceed as if no multilink
 */


boolean multilink_up (hwidbtype *idb)
{

    boolean first;
    mlpbundletype *bundle;
    dialerdbtype *ddb;

    if (multilink_detailed_debug)
	buginf("\nMLP add %s to a bundle",idb->hw_namestring);

    if (is_multilink(idb))
	return(TRUE);

    if (idb->firstsw->is_bundle_interface)
	return(TRUE);

    bundle = mlp_add_link(idb, &first);

    ddb = get_dialerdb(idb);
    if (bundle && (bundle->state == MLP_BUNDLE_ACTIVE)) {
	if (first) {
	/*
	 *  Force wait for add timer to be wake on first link to allow
	 *    next the second add as soon as possible.
	 *  links 3,4,... will have to wait for the timer to exper
	 */
	    TIMER_START(bundle->wait_for_add_timer, 0);
	    if (multilink_events_debug)
		buginf("\nMLP %s: %s, multilink up, first link",
		       idb->hw_namestring, bundle->name);

	    /*
	     * See if we have any pending NCP packets for this bundle 
	     */
	    mlp_process_pending_ncps(bundle->master_link->idb, TRUE);
	     
            /*
             * If a Master Bundle Interface was created, return status
             * such that NCPs will not start on this link. Instead, 
             * NCPs will start on the Master Bundle Interface
             */
            if (bundle->master_link->idb->firstsw->is_bundle_interface)
               return(TRUE);

	    return(FALSE);
	} else {
	    if (ddb != NULL) {
		ddb->dialer_spoofing_prot = FALSE;
		ddb->dialer_spoofing_int = FALSE;
		dialer_holdq_discard_queue(&ddb->dialer_holdQ, "MLP bundle");
	    }
	    if (multilink_events_debug)
		buginf("\n%s: %s, multilink up",
		       idb->hw_namestring, bundle->name);
	    
	    mlp_check_max_links(bundle);

	    return(TRUE);
	}
    } else {

	if (multilink_events_debug) {
 	    buginf("\nMLP %s: %s multilink not up", idb->hw_namestring,
 		ddb ? ddb->number_called : common_str_empty_str);
	}
	return(FALSE);
    }

    return(FALSE);

}

static inline mlplinktype * mlp_get_master_link (mlplinktype *link)
{
    return(link->bundle->master_link);
}

static inline boolean mlp_is_disconnect_idle (hwidbtype *idb)
{
    dialerdbtype *ddb;

    if (idb != NULL) {
	ddb = get_dialerdb(idb);
	if (ddb != NULL)
	    return(ddb->disconnect_reason == DISCONNECT_IDLE);
    }
    return(FALSE);
}

void multilink_down (hwidbtype *idb)
{
    mlplinktype *link;
    mlpbundletype *bundle;

    /*
     *  Only execute if  multilink is up and the interface is not a
     *    bundle interface.  multilink_down should only be used
     *    for member links and not a bundle interface.
     *  Use idb->reset on the bundle interface to bring down a
     *    bundle and all of the member links.
     */


    /*
     *  no multilink_down on a bundle interface.
     */
    
    if (idb->firstsw->vaccess_info && 
	(idb->firstsw->vaccess_info->type == VTEMPLATE_TYPE_MLP_BUNDLE)) {
	return;
    }

    if (is_multilink(idb)) {

        link = idb->mlp_link;

	/*
	 *  Set NULL here that interrupt processes may not get a false
	 *  return of TRUE from is_multilink and try to access bundle info.
	 */
	idb->mlp_link = NULL;

	bundle = link->bundle;
	
	mlp_clear_link(link, idb);

	mlp_remove_link(link);

	if (!bundle->nlinks) {
	    /*
	     *  Removed the last link of a bundle
	     */
	    mlp_remove_bundle(bundle);
	    free(bundle);
	}
	free(link);
    }
}

/*
 * mlp_dataprint
 * Print out first bytes of a frame
 */

/*
 *  Do not increase MLP_MAX_DEBUG_BYTES value or modify the sprintf below
 *    without considering the size of print_buffer.
 *  string_getnext returns an 80 byte buffer.  the sprintf loop
 *    will generate at most 5 bytes or every data byte.  That is
 *    15 * 5 = 75 < 80
 */
#define MLP_MAX_DEBUG_BYTES 15

static void mlp_dataprint (paktype *pak)
{
    ushort length;
    uchar *data_ptr;
    uchar *print_buffer;
    uchar *buffer_ptr;
    int counter;     

    print_buffer = string_getnext();
    if (print_buffer) {
	buffer_ptr = print_buffer;
	data_ptr = pak->datagramstart;
	length = min(MLP_MAX_DEBUG_BYTES,pak->datagramsize);
	for (counter = 0; counter < length; counter++) {
	    buffer_ptr += sprintf(buffer_ptr, " 0x%x", data_ptr[counter]);
	}
	if (buffer_ptr != print_buffer)
	    buginf("\n%s", print_buffer);
    }
}


static void mlp_print (paktype *pak, boolean is_outbound)
{
    char *protocol;
    uchar *p;
    idbtype *idb;
    
    if (is_outbound) {
	idb = pak->if_output;
    } else {
	idb = pak->if_input;
    }

    p = pak->datagramstart + pak->encsize;
    protocol = string_getnext();

    sprintf(protocol, "MLP %s: %c seq %x : size %d", 
	    idb ? idb->namestring : "?",
	    is_outbound ? 'O' : 'I',
	    GETLONG(p),
	    pak->datagramsize);
    buginf("\n%s", protocol);

    if (multilink_data_debug)
	mlp_dataprint(pak);
    
}

/*
 *  mlp_remove_una_sequence 
 *  search the unassigned fragment list for a specific sequence number
 *  if found, remove that fragment from the list.
 *  The list is sorted with the fragment closest to the expected sequence
 *    number last.  So search from the end.
 */

static paktype *mlp_remove_una_sequence (ulong sequence,
					 mlpbundletype *bundle)
{
    paktype *una_frag, *next, *prior;
    
    una_frag = bundle->una_frag_end;
    if (!una_frag)
	return(NULL);
    
    if (mlp_get_pak_seq(una_frag) != sequence) {
	/*
	 *  Sequence number is not in the unassigned fragment list
	 *    Start the lost fragment timer
	 */
	if (!TIMER_RUNNING(bundle->lost_frag_timer))
	    TIMER_START(bundle->lost_frag_timer, MLP_LOST_FRAG_TIME);
	return(NULL);
    }

    next = una_frag->mlp_next_frag;
    prior = una_frag->mlp_prior_frag;
    
    if (next)
	next->mlp_prior_frag = prior;
    else {
	/* removing end of list */
	bundle->una_frag_end = prior;
    }
    if (prior)
	prior->mlp_next_frag = next;
    else {
	/* removing head of list */
	bundle->una_frag_list = next;
    }
    
    bundle->unassigned--;
    return(una_frag);
    
}

/*
 *  mlp_put_frag_unassigned_sorted
 *  puts incoming fragments on the unassigned list for the bundle
 *   associcated with the interface the fragment came in on.
 *  The una_frag_list is sorted with decreasing distance of sequence
 *    number from the expected sequence number first.  Last frament
 *    on the list is closest to the expected sequence number
 *  The una_frag_list head is the paktype * una_frag_list in the
 *    mlpbundletype structure.  The list is managed by mlp_frag pointer
 *    in the paktype data structure.
 *  This function assumes fragment is within the window of expected
 *    sequence numbers
 */


static void mlp_put_frag_unassigned_sorted (mlpbundletype *bundle,
					    paktype *una_frag, ulong seq)
{
    
    paktype *before, *after;
    
    before = bundle->una_frag_list;
    if (!before) {
	/*
	 *  List is empty. Put fragment on the list
	 */
	bundle->una_frag_list = una_frag;
	bundle->una_frag_end = una_frag;
	una_frag->mlp_next_frag = NULL;
	una_frag->mlp_prior_frag = NULL;
    } else {
	/*
	 *  The last fragment on the list is closest to the
	 *   next expected sequence number.  
	 *  Determine the distance between the new fragment and
	 *   fragment on the list.
	 *  If the distance is greater than the expected sequence number
	 *   number window, then put the new fragment before the current
	 *   fragment on the list.
	 *  Else move to the next fragment.
	 */
	while (before) {
	    if (mlp_frag_distance(seq, mlp_get_pak_seq(before)) < 
		MLP_SEQ_WINDOW_SIZE_24BITS) {
		before = before->mlp_next_frag;
	    } else
		break;
	}
	
	if (!before) {
	    /*
	     *  Fragment follows all those on the list.  
	     *  Insert fragment at end of list.
	     */
	    una_frag->mlp_prior_frag = bundle->una_frag_end;
	    una_frag->mlp_next_frag = NULL;
	    bundle->una_frag_end->mlp_next_frag = una_frag;
	    bundle->una_frag_end = una_frag;
	} else {
	    /*
	     *  Fragment is inserted at beginning or middle
	     *    before -> to fragment immediately following insertion point.
	     */
	    after = before->mlp_prior_frag;

	    una_frag->mlp_prior_frag = after;
	    una_frag->mlp_next_frag = before;

	    before->mlp_prior_frag = una_frag;
	    if (after)
		after->mlp_next_frag = una_frag;
	    else {
		/*
		 *  Put fragment at the beggining of the list
		 */
		bundle->una_frag_list = una_frag;
	    }
	}
    }
    bundle->unassigned++;
}

static void mlp_add_header (paktype *pak, ulong sequence, boolean beg_bit,
			    boolean end_bit)
{
    int size;
    uchar * pchar;
    hwidbtype *idb;

    pchar = pak->datagramstart;
    idb = pak->if_output->hwptr;
    pak->encsize = 0;

    /*
     *  Put the multilink long sequence header on the packet
     */
    pak->datagramstart -= sizeof(ulong);
    PUTLONG(pak->datagramstart, sequence);

    if (beg_bit)
	mlp_set_beg_bit(pak->datagramstart);
    if (end_bit)
	mlp_set_end_bit(pak->datagramstart);
    
    pak->datagramstart -= PPP_PROTO_HDR_ENCAPBYTES;
    PUTSHORT(pak->datagramstart, TYPE_PPP_MULTILINK);
    pak->encsize += PPP_PROTO_HDR_ENCAPBYTES;
    

    /*
     *  If running ppp reliable link don't put FF03 at front of frame
     */
    if (!idb->ppp_new_vencap) {
	pak->datagramstart -= PPP_ADDRCNTL_HDR_ENCAPBYTES;
	PUTSHORT(pak->datagramstart, PPP_STATION); /* Now fill it in */
	pak->encsize += PPP_ADDRCNTL_HDR_ENCAPBYTES;
    }

    size = pchar - pak->datagramstart;
    pak->datagramsize += size;
}

static inline int mlp_get_num_links (mlpbundletype *bundle) 
{
    return(bundle->nlinks);
}

static inline boolean mlp_is_frag_enabled (mlpbundletype *bundle)
{
    return(bundle->is_frag);
}

/*
 * mlp_get_next_link
 *  return the next link in the link list.
 */

static inline mlplinktype * mlp_get_next_link (mlplinktype *link)
{
    mlplinktype *next;
    next = link->next;
    if (!next)
	next = link->bundle->first_link;
    return(next);
}

/*
 * mlp_update_next_xmit_link
 *  Move next_xmit_link to point to the next link we desire to xmit over.
 *  return the new xmit link.
 */

static inline mlplinktype * mlp_update_next_xmit_link (mlpbundletype *bundle)
{
    mlplinktype *next_link;
    
    /*
     *  Round robin scheme
     */
    next_link = mlp_get_next_link(bundle->next_xmit_link);
    bundle->next_xmit_link = next_link;
    return(next_link);
}

/*
 * mlp_change_if_output_next_xmit_link
 *  change if_output for a packet to output over the xmit link
 */

static inline void mlp_change_if_output_next_xmit_link (paktype *pak,
							mlpbundletype *bundle)
{
    pak->if_output = bundle->next_xmit_link->idb->firstsw;
}

/*
 *  Fill in the fragment definitiaton structure associated with the bundle
 *  return(TRUE)  packet has more than one fragment
 *  return(FALSE) packet has only one fragment
 */

/*
 *  log_2_round is a table that takes the log base 2 of the index and
 *  rounds down.
 */  

static const uchar log_2_round[17] = {0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4};

static boolean mlp_fill_in_frag_struct (ushort pak_size, mlpbundletype *bundle)
{

    ushort nlinks, temp_exp;
    mlplinktype *next_link;
    ushort min_frag_size, frag_size;
    ushort extra_bytes;
    
    frag_size = 0;
    extra_bytes = 0;
    /*
     *  The maximum number links that we will send fragments over (nlinks) is
     *   the lesser of:
     *   a.  the number of links in the bundle
     *   b.  the configured maximum number of fragments per packet
     *  Optimize nlinks so that is is always a power of 2.  Then we can
     *   use shifts instead of divides to determine the fragment size.
     */
    temp_exp = log_2_round[min(bundle->nlinks, bundle->max_frags)];
    if (!temp_exp)
	return(FALSE);

    next_link = mlp_update_next_xmit_link(bundle);
    min_frag_size = bundle->min_frag_size;

    /* 
     * more than one link in bundle and fragmentation enabled
     *
     * Possible ways to fragment the packet and distribute it
     * 1. Divide packet by n links in bundle
     * 1a. Divide packet by n and consider Bandwidth of each link
     * 1b. Divide packet by n links in bundle but if fragment size
     *   is less than min_frag_size decreas number of links until
     *   frag_size is greater then min_frag_size or only one link
     *   with round robin use of links.
     * 1c. Same as 1b but use powers of 2 and shifts instead of divide
     * 2. Minimize the difference in queue depths in packets.
     * 2a. Minimize the difference in queue depths in bytes.
     * 3. Fragment according to protocol.
     * 4. Minimize the delay of the packet.
     */

    if (bundle->are_links_equal) {
	/*
	 *  Determine the fragment size by option 1c
	 */

	while ((frag_size = (pak_size >> temp_exp)) < min_frag_size) {
	    if (!(--temp_exp))
		return(FALSE);
	}
	nlinks = 0x1 << temp_exp;

	next_link->num_frags = nlinks;
	extra_bytes = pak_size & (nlinks - 1);
	if (frag_size & 0x1) {
	    /*
	     *  Frag size is odd.  make it even and add the odd bytes
	     *    back into extra_bytes
	     */
	    extra_bytes += nlinks;
	    frag_size -= 1;
	}
	while (nlinks--) {
	    if (!next_link) {
		if (multilink_events_debug)
		    buginf("\nMLP %s: Invalid link pointer", bundle->name);
		return(FALSE);
	    }
	    /*
	     *  Put the extra bytes on the last fragment
	     */
	    if (nlinks)
		next_link->frag_size = frag_size;
	    else
		next_link->frag_size = frag_size + extra_bytes;

	    next_link = mlp_get_next_link(next_link);
	}
    } else {
	
	/*
	 *  Unequal cost paths
	 */
	buginf("\nMLP: Unequal costs paths not done yet");

    }
    return(TRUE);
}

/*
 *  Kill all fragments in a packet
 */

static void mlp_kill_packet (paktype *pak)
{
    /*
     *  Datagram done the packet.  datagram_done takes care of
     *    killing any fragments chained to the packet.
     */

    datagram_done(pak);

}


/*
 * mlp_raw_enqueue
 * Called from interrupt level to enqueue an input packet
 * The physical idb that is an MLP negotiated will have this vector 
 * in the idb. The main task here is to raw_enqueue only LINK_PPP and/or
 * LINK_MULTILINK packets to this interface. All other packets 
 * get redirected to the master Bundle interface 
 */
void mlp_raw_enqueue (hwidbtype *link_hwidb, paktype *pak)
{
    hwidbtype *bundle_hwidb;
    idbtype *temp_idb;
    int protocol;

    if (!pak->if_input) {
	retbuffer(pak);
	return;
    }
    GET_TIMESTAMP(link_hwidb->lastinput);  /* remember time of last input */

    /*
     * If hardware is not up, discard packet and return
     */
    if (link_hwidb->state != IDBS_UP) {
	protocol_discard(pak, TRUE);
	return;
    }

    pak->linktype = (*link_hwidb->getlink)(pak);

    /*
     * Only PPP and Multilink packets go thru standard raw_enqueue.
     * PPP packets (all *NCPs etc), inside PPP, will also find its way
     * into the Bundle Interface's iqueue. LCP are not redirected. They 
     * up in the LCP engine for the (this) link's interface. 
     * Multilink packets are also raw_enqueued to the Multilink engine. 
     * The reassembled (if required) packet will then find its way to 
     * the Master Bundle's iqueue.  
     * All other packets go directly to the Bundle Interface. This is 
     * necessary since a Multilink negotiated link, on the first link, 
     * will send non-Multilink packets e.g. IP packets etc. If we 
     * do not redirect the packet to the Bundle interface (where 
     * IPCP resides), we'll have mass confusion
     *
     *  Multilink packets do not need to be aligned
     *    becuase they are always process switched and terminate
     *    in the PPP process
     *  The cost of having an unaligned protocol field is
     *    much less than recopying the whole fragment.
     */

    if (pak->linktype == LINK_MULTILINK) {
	mlp_enqueue(pak);
	return;
    }
    
    /*
     *  Packet is not multilink encapsulated.  Could be LINK_PPP
     *    or  a protocol packet.
     *  The following code may seem convoluted but we are trying
     *   to optimize the multilink path, then the non-multilink
     *   data path and sacrificing the LINK_PPP path.
     */
    
    if (!link_hwidb->mlp_link || !link_hwidb->mlp_link->bundle->master_link) {
	retbuffer(pak);
	return;
    }
    
    bundle_hwidb = mlp_get_master_idb(link_hwidb);
    
    /* 
     * Change the pak's input interface to the bundle interface, BUT
     *   save the original idb.  We might have to change it back.
     */
    
    temp_idb = pak->if_input;
    if (!change_if_input(pak, bundle_hwidb->firstsw)) {
	retbuffer(pak);
	return;
    }
    
    /*
     * Snap the frame to alignment if required
     */
    ALIGN_DATAGRAM_CMD(pak_realign(pak, NULL));
    
    pak->linktype = (*bundle_hwidb->getlink)(pak);
    
    if (pak->linktype == LINK_PPP) {
	
	/*
	 *  This could an NCP, lcp, or auth packet
	 *  NCP's will fall through this test and be raw enqueued
	 *    when bundle_hwidb->iqueue is called below.
	 *  For LCP and auth packets, the pak->if_input is changed back
	 *    to original and should be queued directly to ppp_enqueue.
	 */


	protocol = ppp_get_protocol_type(pak);

	if (protocol >= TYPE_PPP_LCP) {
	    /*
	     *  Change if_input to the original idb and queue the packet.
	     *    because this is an lcp or auth packet.
	     */
	    
	    if (!change_if_input(pak, temp_idb)) {
		retbuffer(pak);
		return;
	    }
	    
	    ppp_enqueue(pak);
	    return;
	}
    }
    
    if (multilink_detailed_debug && multilink_debug)
        buginf("\n%s MLP (I) mlp_raw_enqueue pak=%x linktype=%s redirect to %s", 
	       link_hwidb->hw_namestring, pak, print_linktype(pak->linktype),
	       bundle_hwidb->hw_namestring);
    
    /*
     * Send it to the Master Bundle Interface's iqueue
     */
    bundle_hwidb->counters.rx_cumbytes += pak->datagramsize;
    bundle_hwidb->counters.inputs++;
    (*bundle_hwidb->iqueue)(bundle_hwidb, pak);
}

/*
 * A datagram_out to a Bundle interface will end up here. 
 * All *CP packets except LCP get sent to the first link in the 
 * bundle. All other packets are MLP fragmented if necessary and sent 
 * out on individual MLP member links. LCP packets are silently discarded
 * with the exception of the LCP Protocol Reject which effectively is
 * an NCP level reject.
 */
boolean mlp_holdq_enqueue (hwidbtype *bundle_hwidb, paktype *pak, enum HEADTAIL which)
{
    hwidbtype *out_idb;
    
    /*
     * If it is any NCP packet (via ppp_send_packet), send it thru unfragmented
     * to the first link. Exception is LCP packet - discard it unless it is a
     * Protocol Reject.
     */
    if (pak->linktype == LINK_PPP) {
	ppp_dll_hdr *dll;
	ppp_hdr *ppp_hdrp;

	dll = (ppp_dll_hdr *)pak->datagramstart;
	ppp_hdrp = (ppp_hdr *)dll->data;

	/*
	 * LCP packets are discarded (except for the LCP Protocol Reject)
	 */
	if ((pak->flags & PAK_CONTROL) && (ppp_hdrp->code != PROTREJ)) {
	    datagram_done(pak);
	} else {
	    /*
	     * Select the first link's idb to output the NCP packet
	     */
	    if (!bundle_hwidb->mlp_link ||
		!bundle_hwidb->mlp_link->bundle->first_link) {
		if (multilink_events_debug)
		    buginf("\nMLP Bad link %s", bundle_hwidb->hw_namestring);
		datagram_done(pak);
		return(FALSE);
	    }
	    
	    pak->if_output = 
		bundle_hwidb->mlp_link->bundle->first_link->idb->firstsw;
	    
	    /*
	     * Update Bundle Interface's bytes since it has been transmitted
	     * from it
	     */
	    bundle_hwidb->counters.tx_cumbytes += pak->datagramsize;
	    
	    out_idb = pak->if_output->hwptr;
	    
	    if (multilink_detailed_debug && multilink_debug)
		buginf("\n%s MLP (O) *CP packets to %s", 
		       bundle_hwidb->hw_namestring, 
		       out_idb->hw_namestring);
	    
	    if (out_idb->board_encap) {
		if (!(*out_idb->board_encap)(pak, out_idb)) {
		  if (multilink_events_debug) {
		      buginf("\nMLP %s: Bad board encap, 1",
			     out_idb->hw_namestring);
		  }
		  mlp_kill_packet(pak);
		  return(FALSE);
	      }
	    }
	    datagram_out(pak);
	} 
	return(TRUE);
    }
    

    if (multilink_detailed_debug && multilink_debug)
       buginf("\n%s MLP (O) packets link=%s", 
 	      bundle_hwidb->hw_namestring, print_linktype(pak->linktype));
 
    /*
     * Update Bundle Interface's bytes since it has been transmitted
     * from it
     * ???? size here does not include multilink encapsulation bytes
     *  for fragment(s).
     */
    bundle_hwidb->counters.tx_cumbytes += pak->datagramsize;
    
    /*
     * Xmit the fragmented paks (if any)  
     */
    pak->if_output = (idbtype *)pak->mlp_prior_frag;
    if (mlp_xmit_fragments(bundle_hwidb, pak, 0))
 	return(TRUE);

    /*
     * Unexpected failure. Even if MLP decides not to fragment 
     * the packet, it still returns TRUE (above)
     */
    if (multilink_events_debug) 
	buginf("\nMLP Fragment unexpected failure %s", 
	       bundle_hwidb->hw_namestring);     
    
    return(FALSE);
}

/*
 * Transmit the fragmented packets 
 */
boolean mlp_xmit_fragments (hwidbtype *bundle_hwidb, paktype *pak, long address)
{
    mlpbundletype *bundle;
    paktype *fragment, *next_frag, *first_frag;
    hwidbtype *out_idb;
    ulong sequence;

	bundle = bundle_hwidb->mlp_link->bundle;
	/*
	 *  pak may be one of three types
	 *  1.  Single packet with no mutlilink encapsulation
	 *  2.  Single packet with multilink encapsulation
	 *  3.  Packet with multilink fragments with multilink encapsulation
	 */
	if (!(pak->mlp_next_frag || pak->mlp_frag_size)) {
	    /*
	     *  Type 1.
	     */

	    out_idb = pak->if_output->hwptr;
	    if (out_idb->ppp_new_vencap) {
		if (!(*out_idb->ppp_new_vencap)(pak, address)) {
		    if (multilink_events_debug) {
			buginf("\nMLP %s: Bad new vencap",
			       out_idb->hw_namestring);
		    }
		    mlp_kill_packet(pak);
		    return(FALSE);
		}
	    } else if (out_idb->board_encap) {
		if (!(*out_idb->board_encap)(pak, out_idb)) {
		    if (multilink_events_debug) {
			buginf("\nMLP %s: Bad board encap, 2",
			       out_idb->hw_namestring);
		    }
		    mlp_kill_packet(pak);
		    return(FALSE);
		}
	    } 
	    datagram_out(pak);

	} else if (!pak->mlp_next_frag) {
	    /*
	     *  Type 2.  Put multilink stuff on the packet
	     */
	    sequence = bundle->send_seq;
	    mlp_add_header(pak, sequence, TRUE, TRUE);
	    bundle->send_seq = mlp_inc_sequence(sequence);
	    if (multilink_debug) {
		mlp_print(pak, TRUE);
	    }
    
	    out_idb = pak->if_output->hwptr;
	    if (out_idb->ppp_new_vencap) {
		if (!(*out_idb->ppp_new_vencap)(pak, address)) {
		    if (multilink_events_debug) {
			buginf("\nMLP %s: Bad new vencap",
			       out_idb->hw_namestring);
		    }
		    mlp_kill_packet(pak);
		    return(FALSE);
		}
	    } else if (out_idb->board_encap) {
		if (!(*out_idb->board_encap)(pak, out_idb)) {
		    if (multilink_events_debug) {
			buginf("\nMLP %s: Bad board encap, 2",
			       out_idb->hw_namestring);
		    }
		    mlp_kill_packet(pak);
		    return(FALSE);
		}
	    } 
	    datagram_out(pak);

	} else {
	    /*
	     *  Type 3.  
	     *  Update size of first fragment 
	     *  Put sequence numbers in all fragments
	     *  Output all fragments
	     */

	    /*
	     *  Not compatible with fair queueing
	     */
	    pak->datagramsize = pak->mlp_frag_size;
	    sequence = bundle->send_seq;
	    /*
	     *  Process the first fragment here to put the beginning bit
	     *    in the header.
	     */
	    first_frag = pak;
	    mlp_add_header(first_frag, sequence, TRUE,
			   !(first_frag->mlp_next_frag));
	    sequence = mlp_inc_sequence(sequence);
	    if (multilink_debug) {
		mlp_print(first_frag, TRUE);
	    }
	    /*
	     *  Put the multilink header on the rest of the fragments
	     */
	    first_frag = pak->mlp_next_frag;
	    FOR_ALL_FRAGMENTS_IN_PACKET(first_frag, fragment) {
		mlp_add_header(fragment, sequence, FALSE,
			       !(fragment->mlp_next_frag));
		sequence = mlp_inc_sequence(sequence);
		if (multilink_debug) {
		    mlp_print(fragment, TRUE);
		}
	    }
	    bundle->send_seq = sequence;  /* update bundle seq. number */
	    /*
	     *  Put any other headers on the multilink fragments.
	     */
	    FOR_ALL_FRAGMENTS_IN_PACKET(pak, fragment) {
		out_idb = fragment->if_output->hwptr;
		/*
		 *  put other encapsulations on after multilink
		 *  reliable link puts board_encap on so don't do both
		 *    if ppp_new_vencap
		 */
		if (out_idb->ppp_new_vencap) {
		    if (!(*out_idb->ppp_new_vencap)(fragment, address)) {
			if (multilink_events_debug) {
			    buginf("\nMLP %s: Bad new vencap",
				   out_idb->hw_namestring);
			}
			mlp_kill_packet(fragment);
			return(FALSE);
		    }
		} else if (out_idb->board_encap) {
		    /*
		     *  Board encap is used for PRI interfaces.
		     */
		    if (!(*out_idb->board_encap)(fragment, out_idb)) {
			if (ppp_error_debug) {
			    buginf("\nMLP %s: Bad board encap, 2",
				   out_idb->hw_namestring);
			}
			mlp_kill_packet(pak);
			return(FALSE);
		    }
		} 
	    }
	    /*
	     *  Output all fragments.
	     *  datagram_out may datagram_done(fragment) so we need
	     *   to save a pointer to the next fragment in next_frag
	     */
	    fragment = pak;
	    while (fragment) {
		next_frag = fragment->mlp_next_frag;
		fragment->mlp_next_frag = NULL;
		datagram_out(fragment);
		fragment = next_frag;
	    }
	}
     return(TRUE);
}

/*
 *  Fragment a packet if necessary and output the fragments using
 *    datagram_out
 *  pak is modified to contain the first fragment. 
 *  if a fragmentation error (ie.  no memory) occurs, back up to
 *    a single fragment in pak.
 */

boolean mlp_fragment (paktype *pak, mlpbundletype *bundle)
{
    uchar *pdata;
    int nlinks, num_frags;
    mlplinktype *next_link;
    ulong mlp_total_hdr_size;
    paktype *prior_frag, *next_frag;

    next_link = NULL;
    mlp_total_hdr_size = 0;
    nlinks = mlp_get_num_links(bundle);

    if (nlinks == 0) {
	if (multilink_events_debug) 
	    buginf("\nMLP %s: No members", bundle->name);
	return(FALSE);
    }
    
    /* 
     *  Indicate packet is not fragmented by default
     *  pak->mlp_frag_size = 0 && pak->mlp_next_frag = NULL
     *    indicates packet does not need mlp encapsulation
     */
    pak->mlp_frag_size = 0;
    pak->mlp_next_frag = NULL;
    pak->mlp_prior_frag = NULL;

    if (nlinks == 1) {
	
	/* 
	 *  only one link in bundle.
	 *  no fragmentation, no Multilink encaps 
	 */
	
	mlp_change_if_output_next_xmit_link(pak, bundle);
	return(TRUE);
	
    }

    /*
     *  packet has the full PPP header on it.  Remove the address
     *    and control fields befor encapsulating in multilink.
     */
    pak->datagramsize -= PPP_ADDRCNTL_HDR_ENCAPBYTES;
    pak->datagramstart += PPP_ADDRCNTL_HDR_ENCAPBYTES;
    
    if ((mlp_is_frag_enabled(bundle) == FALSE) ||
	(!mlp_fill_in_frag_struct(pak->datagramsize, bundle))) {
	
	/* 
	 *  fragmentation disabled or we decided not to fragment
	 */

	/*
	 *  Update the next transmit link.
	 *  mlp_fill_in_frag_struct already updates it so only
	 *    update if fragmentation is disabled.
	 */

	if (mlp_is_frag_enabled(bundle) == FALSE)
	    next_link = mlp_update_next_xmit_link(bundle);
	
	/*
	 *  Bridge Alert !!! Bridge Alert
	 *    Multilink Encapsulating a NON-fragmented Bridge Packet
	 *      over a PRI and maybe a BRI depending on Bridge header
	 *      will traceback in if_ags-c7000 because the size is
	 *      larger than 1524.
	 *  Therefore Always fragment Bridged Packets
	 */
	
	/* How do we choose the interface to send it over 
	 * 1. Round robin.  Assumes all channels equivalent bit rate
	 * 2. Check queue depths and queue to shallowest queue
	 * 2a. Shallowest queue but Least Recently used
	 * 3. Round robin with bit rates considered (not simple)
	 * 4. use old load balancing algorithm
	 * 5. Send packets to the queue with the longest time.
	 */  
	
	mlp_change_if_output_next_xmit_link(pak, bundle);
	pak->mlp_frag_size = pak->datagramsize;
	pak->mlp_next_frag = NULL;
	return(TRUE);
	
    }
    
    /*
     * Form first fragment in existing packet. 
     *  Preventing copy on unfragmented packets and one copy on fragmented
     *  bundle->next_xmit_link is the first output link for the packet
     */

    /* 
     * size of next_frag = frag size + PPP header size + addition
     *   Multilink header info
     */
    pdata = pak->datagramstart;
    next_link = bundle->next_xmit_link;

    /*
     *  next_link->num_frags was updated by mlp_fill_in_frag_struct
     */
    num_frags = next_link->num_frags;         
    mlp_total_hdr_size = PPP_ENCAPBYTES + mlp_get_seq_size();
    pak->if_output = next_link->idb->firstsw;
    pak->mlp_frag_size = next_link->frag_size;
    num_frags--;
    pdata += next_link->frag_size;
    prior_frag = pak;
    
    while (num_frags--)
    {
	next_link = mlp_get_next_link(next_link);
	next_frag = getbuffer(mlp_total_hdr_size + next_link->frag_size);   
	if (!next_frag) {
	    prior_frag->mlp_next_frag = NULL;
	    mlp_kill_packet(pak->mlp_next_frag);
	    return(FALSE);
	}
	next_frag->if_output = next_link->idb->firstsw;
	next_frag->datagramsize = next_link->frag_size;
	bcopy(pdata, next_frag->datagramstart, next_link->frag_size);
	pdata += next_link->frag_size;
	prior_frag->mlp_next_frag = next_frag;
	prior_frag = next_frag;
    }
    prior_frag->mlp_next_frag = NULL;
    bundle->next_xmit_link = next_link;
    return(TRUE);
}

static boolean mlp_add_to_working_pak(mlpbundletype *bundle, paktype *pak)
{
    ushort size;
    ushort new_size;

    if (!pak)
	return(FALSE);

    if (!bundle->working_pak) {
	datagram_done(pak);
	bundle->is_lost_frag = TRUE;
	/*
	 *  Return False fragmented was not put in a packet
	 */
	return(FALSE);
    }
    
    size = pak->datagramsize;
    if (!size) {
	datagram_done(pak);
	/*
	 *  Return TRUE because packet of size zero does not need
	 *   to be added to working pak.
	 */
	return(TRUE);
    }

    new_size = bundle->working_pak->datagramsize + size;
    if (new_size > bundle->mrru) {
	/*
	 *  Incoming fragments make a packet larger than the negotitated mrru
	 */
	if (multilink_events_debug) {
	    buginf("\nMLP %s: Packet too large", bundle->name);
	}
	datagram_done(pak);
	bundle->is_lost_frag = TRUE;

	/*
	 *  Working pak is not valid, trash it.
	 */
	datagram_done(bundle->working_pak);
	bundle->working_pak = NULL;

	return(FALSE);
    }
    
    bcopy(pak->datagramstart, bundle->working_pak_ptr, size);
    bundle->working_pak_ptr += size;
    bundle->working_pak->datagramsize += size;
    datagram_done(pak);
    return(TRUE);
}

static boolean mlp_create_empty_working_packet (mlpbundletype *bundle)
{

    paktype *newpak;
    
    if (bundle->working_pak)
	return(FALSE);
    
    newpak = getbuffer(bundle->mrru);
    if (!newpak)
	return(FALSE);
   
    /*
     *  copy the data
     */
    newpak->datagramsize = 0;
    newpak->datagramstart = newpak->network_start;
    newpak->encsize = 0;
    newpak->mlp_next_frag = NULL;
    newpak->mlp_frag_size = 0;
    newpak->mlp_prior_frag = NULL;

    bundle->working_pak = newpak;
    bundle->working_pak_ptr = newpak->datagramstart;
    
    return(TRUE);
}

/*
 *  mlp_is_out_of_order_all_links
 *  return(TRUE) if an out of order fragment has been received over
 *   each link in a bundle
 *  return(FALSE) otherwise
 */

static inline boolean mlp_is_out_of_order_all_links (mlpbundletype *bundle)
{

    boolean tempbool;
    mlplinktype *link;

    tempbool = TRUE;

    FOR_ALL_LINKS_IN_BUNDLE(bundle, link) {
	tempbool = tempbool && link->rcv_out_of_order_frag;
	if (!tempbool)
	    return(FALSE);
    }

    return(tempbool);

}

/*
 *  Process incoming fragments.
 *  Reassemble them in working_pak.
 *  Verify that Beginning and Ending bits are received in the proper order
 */

static paktype *mlp_process_rcv_packet (mlpbundletype *bundle, paktype *pak)
{
    ulong seq;
    ulong expected_seq;
    boolean begin_bit;
    boolean end_bit;
    paktype *newpak;
    mlplinktype *link;
    
    /* get header data */
    seq = mlp_get_pak_seq(pak);
    begin_bit = mlp_get_beg_bit(pak);
    end_bit = mlp_get_end_bit(pak);
    expected_seq = bundle->rcv_seq;
    
    link = pak->if_input->hwptr->mlp_link;
    /*
     *  link may be NULL if the interface disconnected while
     *    the fragment was on the unassigned fragment queue
     *  Make sure you test link before using it.
     */
    
    /* check sequence number */
    if (seq  == expected_seq) {
	
	/* 
	 *  we have the next expected sequence number 
	 */
	
	TIMER_STOP(bundle->lost_frag_timer);
	bundle->rcv_seq = mlp_inc_sequence(bundle->rcv_seq);
	if (link) {
	    link->rcv_out_of_order_frag = FALSE;
	}
	
	/* 
	 *  If a fragment has been lost, kill all fragments until the next
	 *    beginning fragment.
	 */
	
	if (bundle->is_lost_frag && !begin_bit) {
	    bundle->discarded_fragments++;
	    datagram_done(pak);
	    return(NULL);
	}
	
	mlp_remove_encaps(pak);
	if (begin_bit && end_bit) {
	    
	    /* expected packet with no fragmentation  */
	    
	    bundle->rcved_end_bit = TRUE;
	    if (change_if_input(pak, bundle->master_link->idb->firstsw)) {
		return(pak);
	    } else {
		if (multilink_events_debug) {
		    buginf("\nMLP %s: Cannot requeue packet to %s",
			   bundle->name,
			   bundle->master_link->idb->firstsw->namestring);
		}
		datagram_done(pak);
		bundle->is_lost_frag = TRUE;
		return(NULL);
	    }
	} else if (end_bit) {
	    
	    /*  last fragment of a packet received  */
	    
	    if ((!bundle->rcved_end_bit) && bundle->working_pak) {
		
		/*  received end bit was in proper order */
		
		if (!mlp_add_to_working_pak(bundle, pak)) {
		    bundle->is_lost_frag = TRUE;
		    return(NULL);
		}
		
		newpak = bundle->working_pak;
		bundle->working_pak = NULL;
		
		bundle->rcved_end_bit = TRUE;
		
		if (bundle && bundle->master_link)
		    set_if_input(newpak, bundle->master_link->idb->firstsw);
		else
		    return(NULL);

		return(newpak);
		
	    } else {
		
		/* 
		 *  End bit followed by and End bit
		 *  indicates that a beggining bit fragment was lost  
		 */
	       
		if (multilink_events_debug)
		    buginf("\nMLP %s: End bit followed by end bit",
			   bundle->name);
		datagram_done(pak);
		
		bundle->is_lost_frag = TRUE;
		
	    }
	    
	} else if (begin_bit) {
	    
	    /* 
	     *  received the first fragment of a packet 
	     */
	    
	    /*
	     * If working pak exists kill it
	     */
	    
	    if (bundle->working_pak) {
		datagram_done(bundle->working_pak);
		bundle->working_pak = NULL;
	    }

	    bundle->is_lost_frag = FALSE;

	    if (pak->pool->size < bundle->mrru) {
		if (mlp_create_empty_working_packet(bundle)) {

		    if (!mlp_add_to_working_pak(bundle, pak))
			bundle->is_lost_frag = TRUE;

		} else {
		    if (multilink_events_debug)
			buginf("\nMLP %s: Unable to create working pack",
			       bundle->name);
			bundle->discarded_fragments++;
			bundle->is_lost_frag = TRUE;
			datagram_done(pak);
		}
	    } else {
		/*
		 *  packet containing the first fragment is large enough
		 *    to hold the largest reassembled packet
		 */
		bundle->working_pak = pak;
		bundle->working_pak_ptr = pak->datagramstart +
					  pak->datagramsize;
		clear_if_input(pak);
	    }
	    bundle->rcved_end_bit = FALSE;

	} else {
	    
	    /* 
	     * neither end or begin bit fragment  
	     */
	    
	    if ((!bundle->rcved_end_bit) && bundle->working_pak) {

		if (!mlp_add_to_working_pak(bundle, pak)) {
		    bundle->is_lost_frag = TRUE;
		}
	    
	    } else {
		
		if (multilink_events_debug)
		    buginf("\nMLP: End bit not followed by beg bit.");
		datagram_done(pak);
		
		bundle->is_lost_frag = TRUE;
		
	    }
	}
    } else {
	
	/* fragment is out of order */
	/*
	 *  This code can only be reached the first time through the
	 *     the loop in mlp_packet.
	 */

	bundle->out_of_order++;
	
	/* 
	 *  See if sequence number is out of expected range
	 */
	
	if (!mlp_is_frag_outside_expected_seq(bundle->rcv_seq,
					      bundle->seq_window_size,
					      seq)) {
	    mlp_put_frag_unassigned_sorted(bundle, pak, seq);
	} else {
	    
	    /* 
	     *  received a fragment that we believe we have already
	     *    marked as lost
	     *  This indicates that lost fragment detection has holes or
	     *    that received packets are so grossly out of order that
	     *    we do not have enough buffer space to queue up all
	     *    the out of order packets.
	     *  Any instances if this need to be closely investigated.
	     */
	    
	    if (multilink_events_debug)
		buginf("\nMLP %s: Received lost fragment %x",
		       bundle->name, seq);
	    bundle->received_lost++;
	    datagram_done(pak);
	    return(NULL);
	}
	
	/* 
	 *  Fragment loss detection algorithm
	 *  0.  Simple timer
	 *  1.  RFC1717 increasing sequence number
	 *  1a. If received out of order packets on all links in a bundle
	 *      then mark expected sequence number as lost.
	 *  2.  No. out of order > No. of links 
	 *  2a. No. out of order fragments is greater than
	 *        max( M minimum # of fragments to hold, 2^N * # of links)
	 *  3.  # of available buffers approaches zero.
	 */
	
	/*
	 * Algorithm 1  - Suggested by RFC1717
	 *  Save the latest received seq for each member link
	 *  m = minimum of latest receive sequence for all member links
	 *  if expected_seq < m, then expected_seq fragment has been lost.
	 *
	 * Pitfalls:
	 *  Rollover of sequence numbers makes this ugly.
	 *  If we stop receiving traffic on one link m never increases.
	 *  May require keepalive null Multilink fragments or Round Robin of
	 *  traffic over member links
	 */
	
	/*
	 * Algorithm 1a  
	 *  Assumes fragments over each link are strictly increasing as
	 *    as stated in rfc1717.
	 *  If fragment is received out of order, check all links in bundle
	 *   to see if each one has received an out of order packet.
	 *  If true, current exepected sequence number is lost.
	 *  If expected fragment is received, clear received out of order
	 *    boolean for all links in the bundle.
	 *
	 * Pitfalls:
	 *  If we stop receiving traffic on one link, this algorithm
	 *    will not detect lost sequence number..
	 */

	if (link) {
	    link->rcv_out_of_order_frag = TRUE;
	}
	if (link && mlp_is_out_of_order_all_links(bundle)) {

	    if (multilink_events_debug)
		buginf("\nMLP %s: Lost fragment 1a %x", 
		       bundle->name,
		       expected_seq);
	    
	    bundle->lost_fragments++;
	    bundle->is_lost_frag = TRUE;
	    
	    /*
	     *  Move expected sequence to the lost sequence number
	     *   in the unassigned list.
	     */

	    if (bundle->una_frag_end)
		bundle->rcv_seq = mlp_get_pak_seq(bundle->una_frag_end);
	    else {
		if (multilink_events_debug)
		    buginf("\nMLP %s: Can't remove end fragment", 
			   bundle->name);
		/*
		 *  Unassigned pak list is improperly linked
		 *    Kill it and wait for the next beginning bit
		 *    fragment to start receiving data.
		 */
		mlp_kill_una_frag_list(bundle);
	    }
	} else if (bundle->lost_frag_algo == MLP_LOST_FRAG_UNA_GT_LINKS) {

	/* 
	 * Algorithm 2a
	 *  M = MLP_DEF_MIN_UNA_SIZE, N = MLP_LOST_FRAG_EXP
	 *  M or N to large and may consume alot of memory
	 *  M or N to small and fragments will be marked as lost and
	 *    may be received later.
	 */
	
	    if (bundle->unassigned > (max((bundle->nlinks << MLP_LOST_FRAG_EXP),
					  MLP_DEF_MIN_UNA_SIZE))) {
		
		if (multilink_events_debug)
		    buginf("\nMLP %s: Lost fragment 2 %x, unassigned %d", 
			   bundle->name,
			   mlp_get_pak_seq(bundle->una_frag_end),
			   bundle->unassigned);
		
		bundle->lost_fragments++;
		bundle->is_lost_frag = TRUE;
		
		/*
		 *  Remove the fragment at the end of the unassigned
		 *    packet list
		 */
		
		if (bundle->una_frag_end) {
		    bundle->rcv_seq = mlp_get_pak_seq(bundle->una_frag_end);
		} else {
		    if (multilink_events_debug)
			buginf("\nMLP %s: Can't remove end fragment", 
			       bundle->name);
		    /*
		     *  Unassigned pak list is improperly linked
		     *    Kill it and wait for the next beginning bit
		     *    fragment to start receiving data.
		     */
		    mlp_kill_una_frag_list(bundle);
		}
	    }
	}
	
	/*
	 * Algorithm 0  
	 *  Start lost fragment timer 
	 */
	
	if (!TIMER_RUNNING(bundle->lost_frag_timer))
	    TIMER_START(bundle->lost_frag_timer, MLP_LOST_FRAG_TIME);
    } 
    return(NULL);
}

static void mlp_packet (paktype *pak)
{
    paktype *next_frag, *new_pak;
    void (*siqueue)(hwidbtype *, paktype *);
    mlpbundletype *bundle;
    hwidbtype *input_idb, *idb;
    leveltype status;

    idb = pak->if_input->hwptr;

    if (multilink_debug && pak)
	mlp_print(pak, FALSE);

    if (!idb->mlp_link) {
	/*
	 *  If the input interface for this packet was disconnected
	 *     while the packet was in the mlp packet queue, then
	 *     discard it because there is no way to get the bundle
	 *     information.
	 */
	if (multilink_events_debug)
	    buginf("\nMLP %s: Multilink not enabled",idb->hw_namestring);
	datagram_done(pak);
	return;
    }

    bundle = idb->mlp_link->bundle;
    if (!bundle) {
	/*
	 *   The bundle is down.  So discard the packet
	 */
	if (multilink_events_debug)
	    buginf("\nMLP %s: No bundle info",idb->hw_namestring);
	datagram_done(pak);
	return;
    }

    pak->mlp_next_frag = NULL;
    pak->mlp_prior_frag = NULL;
    pak->mlp_frag_size = 0;

    /*
     *  check fragments.
     *  If fragment create a new packet forward it.
     *  remove next fragment from the unassigned list.
     */

    next_frag = pak;

    do {
	new_pak = mlp_process_rcv_packet(bundle, next_frag);
	if (!new_pak)
	    continue;
	if (!new_pak->datagramsize) {
	    if (multilink_events_debug)
		buginf("\nMLP %s: No packet data",bundle->name);
	    datagram_done(new_pak);
	    continue;
	}

	/*
	 *  Bridging and ppp reliable link require the complete and full
	 *  PPP header.
	 *  So let's just put the complete header on for all packets.
	 */

	if (*(new_pak->datagramstart) & PPP_ENDPROTO) {

	    /*
	     * protocol header has been compressed
	     * Need to prepend the rest of the header
	     */

	    /*
	     *  Upper byte of protocol is 0x00 when protocol header can
	     *  be compressed
	     */

	    new_pak->datagramstart -= 1;
	    *new_pak->datagramstart = 0x0;
	    new_pak->datagramsize += 1;
	}

	new_pak->datagramstart -= PPP_ADDRCNTL_HDR_ENCAPBYTES;
	PUTSHORT(new_pak->datagramstart, PPP_STATION);
	new_pak->datagramsize += PPP_ADDRCNTL_HDR_ENCAPBYTES;

	input_idb = new_pak->if_input->hwptr;

	/*
	 * Parse the packet using the pak's input interface which
	 * is setup by mlp_process_rcv_packet
	 * This effectively enqueues the packet to the master bundle
	 * interface (bundle interface way) or master link interface
	 * (dialer-way)
	 */
	siqueue = ppp_parse_packet(input_idb, new_pak, NULL);
	
	/*
	 * If fast switching is enabled on this interface, try to fast switch
	 * when the link type is IP only
	 */
	new_pak->linktype = (*input_idb->getlink)(new_pak);
	switch (new_pak->linktype){
	case LINK_IP:
	    status = raise_interrupt_level(NETS_DISABLE);
	
	    if (reg_invoke_ppp_from_process_fs(RXTYPE_DODIP, new_pak)){
	        reset_interrupt_level(status);
	        continue;
	    }

	    reset_interrupt_level(status);

	    break;

	case LINK_NOVELL:
	    status = raise_interrupt_level(NETS_DISABLE);
	
	    if (reg_invoke_ppp_from_process_fs(RXTYPE_NOVELL1, new_pak)){
	        reset_interrupt_level(status);
	        continue;
	    }

	    reset_interrupt_level(status);

	    break;
	}
	
	if (siqueue) {
	    if (input_idb->firstsw->is_bundle_interface) {
	        if (multilink_debug && multilink_detailed_debug)
	            buginf("\n%s MLP mlp_input (I) packets",
	                   input_idb->hw_namestring);
	        input_idb->counters.rx_cumbytes += new_pak->datagramsize;
		input_idb->counters.inputs++;
	    }
	    (*siqueue)(input_idb, new_pak);
	} else {
	    if (multilink_events_debug)
		buginf("\nMLP %s: Failed to forward packet", bundle->name);
	    datagram_done(new_pak);
	}
    } while ((next_frag = mlp_remove_una_sequence(bundle->rcv_seq, bundle)));
}

/*
 * mlp_enqueue
 * Interrupt level packet enqueue
 */
static void mlp_enqueue (paktype *pak)
{
    process_enqueue_pak(MLPpacketQ, pak);
}

static process mlp_input (void)
{
    int pak_count;
    paktype *pak;
    ulong major, minor;

    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    MLPpacketQ = create_watched_queue("MLP Input", 0, 0);
    process_watch_queue(MLPpacketQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_MULTILINK, mlp_enqueue, "mlp_enqueue");
    TIMER_START(MLPtimer, ONESEC);
    process_watch_timer(&MLPtimer, ENABLE);

    while (TRUE) {
	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {

	    switch (major) {

	    case TIMER_EVENT:
		mlp_timer();
		TIMER_START(MLPtimer, ONESEC);
		break;

	    case QUEUE_EVENT:
		/*
		 * Process only a limited number of packets per pass
		 */
		for (pak_count = PAK_SWITCHCOUNT; pak_count; pak_count--) {
		    pak = process_dequeue(MLPpacketQ);
		    if (pak == NULL) {
			break;
		    }
		    mlp_packet(pak);
		}
		break;

	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * mlp_connect_new_link
 * Try to add a new link to the bundle
 */

static void mlp_connect_new_link (mlpbundletype *bundle, char *reason)
{
    /*
     * Do not allow the number of links to exceed the maximum configured
     */

    if (bundle->nlinks >= bundle->max_links) {
       if (multilink_detailed_debug)
	   buginf("\nMLP %s: Cannot exceed max links",
		  bundle->name);
	return;
    }
    
    if (!reg_invoke_dialer_connect_name(bundle->name, bundle->first_link->idb,
					reason))
       if (multilink_detailed_debug)
	   buginf("\nMLP %s: Could not dial new link",
		  bundle->name);

}

/*
 *  mlp_drop_link
 *  drop/reset/disconnect a link
 */

static inline void mlp_drop_link (mlplinktype *link)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(link->idb);
    if (ddb == NULL)
	return;

    if (multilink_events_debug)
	buginf("\nMLP %s: dropping link",
	       link->idb->hw_namestring);

    TIMER_STOP(ddb->dialer_idle_timer);
    (*(link->idb->reset))(link->idb);

    if (link->bundle->nlinks > 1) {
	/*
	 *  Remove link from the bundle now to prevent fragments
	 *   from being transmitted over the link on its way down
	 *  Don't remove the link here if it is the last in the bundle
	 *   because multilink_down will free the bundle structure.
	 */
	multilink_down(link->idb);
    }
    
}

/*
 * mlp_disconnect_link 
 * disconnect one link from the bundle
 */
static void mlp_disconnect_link (mlpbundletype *bundle)
{

    mlplinktype *drop_link;

    /*
     *  Do not allow the number of links in the bundle to go
     *  below the minimum number configured
     */

    if (bundle->nlinks <= bundle->min_links) {
	if (multilink_events_debug)
	    buginf("\nMLP %s: Cannot go below minimum links",
		   bundle->name);
	return;
    }
    /*
     *  Drop alogrithm one
     *  Drop the slowest link first if it is not the master link
     *  else drop the next slowest
     *  Drop only if more than one link in the bundle
     *  Drop only ddr links
     *  Final link in bundle disconnects via dialer idle timer.
     *  Links are sorted in the bundle with the slowest at the end.
     */

    if (bundle->nlinks > 1) {
	/*
	 *  Find slowest,non-master ddr link
	 */
	drop_link = bundle->last_link;
	while (!if_is_ddr(drop_link->idb) ||
	       (drop_link == bundle->master_link)) {
	    drop_link = drop_link->prior;
	    if (!drop_link)
		return;
	}
	
	
	if (multilink_events_debug)
	    buginf("\nMLP %s: Dropping link %s",
		   bundle->name, drop_link->idb->hw_namestring);
	mlp_drop_link(drop_link);
    }
}

/*
 *  mlp_sum_loads
 *  sum all of the loads of the links in the bundle
 */
static int mlp_sum_loads (mlpbundletype *bundle)
{
    ulong sumload;
    ulong sumrxload;
    enum MULTILINK_THRESHOLD_DIRECTION direction;
    mlplinktype *link;

    sumload = 0;
    sumrxload = 0;
    direction = bundle->thresh_direction;

    /*
     * Go through all connected members, add up load and determine least
     * loaded interface for possible disconnect.
     */
    if ((direction == MULTILINK_THRESH_EITHER) || 
	(direction == MULTILINK_THRESH_OUTBOUND)) {
	FOR_ALL_LINKS_IN_BUNDLE(bundle, link) {
	    sumload += link->idb->load;
	}
    }

    if ((direction == MULTILINK_THRESH_EITHER) || 
	(direction == MULTILINK_THRESH_INBOUND)) {
	FOR_ALL_LINKS_IN_BUNDLE(bundle, link) {
	    sumrxload += link->idb->rxload;
	}
    }
    
    switch(direction) {
      case MULTILINK_THRESH_EITHER:
	sumload = max(sumload, sumrxload);
	break;
      case MULTILINK_THRESH_INBOUND:
	sumload = sumrxload;
	break;
      case MULTILINK_THRESH_OUTBOUND:
      default:
	break;
    }
    return(sumload);
}

/*
 * mlp_timer
 *  Perform multilink timer operations.
 */

static void mlp_timer (void)
{

    paktype *next_frag, *prior;
    mlpbundletype *bundle, *tembundle;
    hwidbtype *idb;
    mlplinktype *link;
    int sum_load, avg_load = 0;
    int nlinks;
    dialerdbtype *ddb;

    bundle = mlp_bundle_list_head;
    while (bundle) {
	
	nlinks = bundle->nlinks;

	if (bundle->state == MLP_BUNDLE_REMOVE) {
	    FOR_ALL_LINKS_IN_BUNDLE(bundle, link) {
		mlp_drop_link(link);
	    }
	    bundle->state = MLP_BUNDLE_DOWN;
            bundle = bundle->next;
	    continue;
	}

        /*
         * If the bundle is by appointment and thus without any
         * links, check to see if it has timed out. If it has, 
         * remove it. Load the bundle variable to the next bundle
         * and continue. Otherwise, ignore this bundle.
	 *
	 *  This timer needs to be replaced with another timer specific
	 *   to appointed bundles timing out.
         */
	  if (TIMER_RUNNING_AND_AWAKE(bundle->appointment_timer)
	      && (bundle->state == MLP_BUNDLE_APPOINTED)) {
	      tembundle = bundle->next;
	      if (multilink_events_debug)
                 buginf("\nMLP bundle %s missed appointment, removed", 
                        bundle->name);
              mlp_remove_bundle(bundle);
	      free(bundle);
              bundle = tembundle;
              continue;
	   }

	/*
	 *  If bundle has no links stop processing it here
	 */

	if (bundle->nlinks == 0) {
	    bundle = bundle->next;
	    continue;
	}
	

	/* 
	 *  Process all bundles once per second
	 */
	
	if (TIMER_RUNNING_AND_AWAKE(bundle->lost_frag_timer)) {
	    TIMER_STOP(bundle->lost_frag_timer);
	    if (multilink_events_debug)
		buginf("\nMLP %s: lost fragment timeout %x ",
		       bundle->name, bundle->rcv_seq);
	    
	    bundle->is_lost_frag = TRUE;
	    bundle->lost_fragments++;
	    
	    if (bundle->una_frag_list) {
		/*
		 *  If timer expires indicating lost fragment, move to the
		 *    next sequence number in the unassiged list if there
		 *    is an unassigned list.  The list is sorted with
		 *    the lowest sequence number fragment at the end.
		 */
		next_frag = bundle->una_frag_end;
		bundle->rcv_seq = mlp_get_pak_seq(bundle->una_frag_end);
		prior = bundle->una_frag_end->mlp_prior_frag;
		if (prior)
		    prior->mlp_next_frag = NULL;
		else
		    bundle->una_frag_list = NULL;
		
		bundle->una_frag_end = prior;
		
		bundle->unassigned--;
		
		mlp_packet(next_frag);
		
	    } else {
		/*
		 *  Lost a fragment but there are no unassigned fragments
		 *  means lost fragment was last fragment sent to this
		 *  bundle
		 */
		bundle->rcv_seq = mlp_inc_sequence(bundle->rcv_seq);
	    }
	}

	/*
	 *  Analyze the loads to see if we need to bring up another link
	 *
	 *  Method for adding:
	 *  if the calculated add load exceds/equals the add_link_threshold
	 *    and the wait_for_add timer has expired add try to add a new
	 *    link.
	 *    calculated add load may have several algorithms.
	 *      1.  unweighted average
	 *      2.  weighted average,  bandwidth is weight
	 *      2a. weighted average w/ priority.
	 *    Restart the wait_for_add timer to prevent another add during
	 *      the next load analyze time before the first add is complete.
	 *    This effectively serializes the adding of links.  One link
	 *      cannot be added before the current link is added or the
	 *      wait_for_add timer expires
	 *    multilink_up expires the wait_for_add timer immediately
	 *      allowing another add on the next load interval.
	 *    Reliaze the once the link is added, nlinks += 1 so load
	 *      must step considerably before the next load interval
	 *      to add a new link.
	 *
	 *  Method for disconnecting links:
	 *  if the caculated add load !> add_link_threshold, then test
	 *     caculated disconnect load.
	 *     calculated disconnect load can be several algorithms.
	 *     1.  unweighted average
	 *     1a.  unweighted average w/ (nlinks - 1) hysterisis.
	 *          avg = sum / nlinks - 1 artificial inflates the actual
	 *          load to add hysterisis.
	 *     2.  weighted average
	 *  if the calculated disconnect load is < delete_link_threshold
	 *     start the wait_before_disconnect timer.
	 *  if the calculated disconnect load is still < delete_link_threshold
	 *     after the wait_before_disconnet timer expires, disconnect
	 *     the link.
	 *  if the caculated add load > add_link_thrshold stop the wait
	 *     before disconnect timer to prevent a disconnect if the
	 *     the load goes back up during the wait before disconnect time.
	 *  
	 */

	if (TIMER_RUNNING_AND_AWAKE(bundle->load_analyze_timer)) {
	    TIMER_START(bundle->load_analyze_timer,
			bundle->load_analyze_ticks);

	    /*
	     *  Algorithm 1 for calculated add load, unweighted average
	     */
	    sum_load = mlp_sum_loads(bundle);
            if (sum_load != 0)
	       avg_load = sum_load / nlinks;
	    bundle->load = avg_load;

	    /*
	     *  If a threshold is configured AND the average load is greater/eq
	     *  than the threshold AND the the minimum time between adds timer
	     *  is awake or stopped THEN try to connect a new link
	     *  No load on an idb is a load of 1.
	     */
	    
	    if (bundle->add_link_threshold &&
		avg_load >= bundle->add_link_threshold) {

		TIMER_STOP(bundle->wait_before_disconnect_timer);

		if (TIMER_RUNNING_AND_AWAKE(bundle->wait_for_add_timer)) {

		    if (multilink_detailed_debug)
			buginf("\nMLP %s: Bundle overloaded, try to add link, %d",
			       bundle->name, bundle->load);
		    mlp_connect_new_link(bundle,
					 "Multilink bundle overloaded");
		    TIMER_START(bundle->wait_for_add_timer,
				bundle->wait_for_add_ticks);
		    
		}
	    } else  {
		if (TIMER_RUNNING(bundle->wait_before_disconnect_timer)) {
		    if (AWAKE(bundle->wait_before_disconnect_timer) &&
			(nlinks > 1)) {
			/*
			 *  Cacluted disconnect load aglorithm 1a.
			 *  Divide the sum for n - 1 to create hysterisis
			 *  For two links, the avg_load must be less than
			 *  half of the delete_link_threshold before a link
			 *  will be disconnected.
			 *  Load should never disconnect the last link
			 */
			avg_load = sum_load / (nlinks - 1);
			if (avg_load < bundle->delete_link_threshold) {
			    if (multilink_detailed_debug)
				buginf("\nMLP %s: Bundle underloaded, try to delete link, %d",
				       bundle->name, bundle->load);
			    mlp_disconnect_link(bundle);
			    /*
			     *  prevent an add from taking place soon
			     *  after the disconnect.  This reduces
			     *  thrashing of connect/disconnect.
			     */
			    TIMER_START(bundle->wait_before_disconnect_timer,
					bundle->wait_before_disconnect_ticks);
			    TIMER_START(bundle->wait_for_add_timer,
					bundle->wait_for_add_ticks);
			} else {
			    /*
			     *  the load peaked up during wait before
			     *   disconnect time.  Stop the timer 
			     *    to prevent unwanted disconnect as the
			     *    load dances around the threshold.
			     */
			    TIMER_STOP(bundle->wait_before_disconnect_timer);
			}
		    }
		} else {
		    TIMER_START(bundle->wait_before_disconnect_timer,
				bundle->wait_before_disconnect_ticks);
		}
	    }
	}
	
	FOR_ALL_LINKS_IN_BUNDLE(bundle, link) {
	    idb = link->idb;
	    /*
	     * Prevent idle timeout on non-master links and the master
	     *   links if delete link theshold is 1
	     */
	    if ((link != bundle->master_link) ||
		(bundle->delete_link_threshold == 1)) {
		/*
		 *  If dialer, update idle timers for all non-master
		 *    links in each bundle so they won't ever timeout
		 */
		ddb = get_dialerdb(idb);
		if (ddb != NULL) {
		    if (TIMER_RUNNING(ddb->dialer_idle_timer) &&
			(TIME_LEFT_SLEEPING(ddb->dialer_idle_timer) <
			 MLP_UPDATE_DIALER_IDLE_INTERVAL))
			mlp_update_timer(&ddb->dialer_idle_timer,
					 ddb->dialer_idle_ticks);
		}
	    }
	}
      bundle = bundle->next;
    }
}

/*
 * show_ppp_mlp
 *  Display info about multilink bundles and links
 */

void show_ppp_mlp (parseinfo *csb)
{

    mlpbundletype *bundle;
    mlplinktype *next_link;
    char membername[MAXSTR+1];
    idbtype *dialer_swidb;
    int nlinks, nbundles;
    hwidbtype *idb;
    l2f_tunneltype *owner;
    mlplinktype **next_links;
    mlplinktype **next_links_ptr;
    mlpbundletype **bundles;
    mlpbundletype **bundles_ptr;

    nbundles = 0;
    FOR_ALL_BUNDLES(bundle) {
	nbundles++;
    }
    if (!nbundles) {
	printf("\nNo active bundles");
	return;
    }

    bundles = malloc_named(nbundles * sizeof(mlpbundletype *), "MLP bundles");
    if (!bundles)
	return;
    bundle = mlp_bundle_list_head;
    for (bundles_ptr = bundles; bundle && bundles_ptr < bundles + nbundles;
	bundles_ptr++) {
	mem_lock(bundle);			/* lock bundle */
	*bundles_ptr = bundle;
	bundle = bundle->next;
    }

    automore_enable(NULL);
    for (bundles_ptr = bundles;
	*bundles_ptr && bundles_ptr < bundles + nbundles; bundles_ptr++) {
	bundle = *bundles_ptr;
	printf("\n\nBundle %s, %d member%s",
	       bundle->name, bundle->nlinks,
	       bundle->nlinks == 1 ? common_str_empty_str : "s");
        if (bundle->nlinks == 0) {
	    free(bundle);			/* unlock bundle */
	    continue;
	}

	if (bundle->master_link) {
	    idb = bundle->master_link->idb;
	    if (idb) {
		printf(", Master link is %s", idb->hw_namestring);
		if (is_ddr(idb)) {
		    dialer_swidb = get_netcfg_idb(idb->firstsw);
		    if (dialer_swidb && (dialer_swidb != idb->firstsw))
			printf("\nDialer Interface is %s",
			       dialer_swidb->namestring);
		}
	    }
	    else printf(", no Master link idb");
	}
	else {
	    printf(", no Master link");
	}

	printf("\n  %u lost fragments, %u reordered, %u unassigned, "
	       "sequence %#x/%#x rcvd/sent",
	       bundle->lost_fragments, bundle->out_of_order,
	       bundle->unassigned,
	       bundle->rcv_seq, bundle->send_seq);
	printf("\n  %u discarded, %u lost received, "
	       "%d/255 load",
	       bundle->discarded_fragments, bundle->received_lost,
	       bundle->load);

        /*
         * Display all the member links on this bundle
         */
	nlinks = bundle->nlinks;
	next_links = NULL;
	if (nlinks) {
	    next_links = malloc_named(nlinks * sizeof(mlplinktype *),
				      "MLP link");
	    if (next_links) {
		next_link = bundle->first_link;
		for (next_links_ptr = next_links; next_link &&
		    next_links_ptr < next_links + nlinks; next_links_ptr++) {
		    mem_lock(next_link);	/* lock next_link */
		    *next_links_ptr = next_link;
		    next_link = next_link->next;
		}
	    }
	}
        printf("\n\nMember Link%s: %d",
	       nlinks == 1 ? common_str_empty_str : "s", nlinks);
	if (next_links) {
	    for (next_links_ptr = next_links; *next_links_ptr &&
		next_links_ptr < next_links + nlinks; next_links_ptr++) {
		idb = (*next_links_ptr)->idb;
		if (!idb) {
		    free(*next_links_ptr);	/* unlock next_link */
		    continue;
		}
		if ((idb->type == IDBTYPE_VACCESS) && idb->firstsw &&
		    idb->firstsw->l2f_mid && idb->firstsw->l2f_mid->owner) {
		    owner = idb->firstsw->l2f_mid->owner;
		    mem_lock(owner);		/* lock owner */

		    /*
		     * Get the MLPVT member name
		     * There may be cases that the user does a "no" to the MLPVT
		     * member name and hence is not found here (MLPVT does not
		     * disconnect any bundle links that are already active). 
		     * In that case, just show the IP address only
		     */
		    bzero(membername, (MAXSTR+1));
		    if (reg_invoke_mlpvt_find_membername(membername, MAXSTR,
			owner->ip_address))
			printf("\n%s:", membername);
		    else
			printf("\n%s:", owner->nas_name);
		    printf("%s  (%i)", idb->hw_namestring, owner->ip_address);

		    free(owner);		/* unlock owner */
		} else
		    printf("\n%s", idb->hw_namestring);

		free(*next_links_ptr);		/* unlock next_link */
	    }
	    free(next_links);
	}
	free(bundle);				/* unlock bundle */
    }
    free(bundles);
    automore_disable();
}

void ppp_mlp_command (parseinfo *csb)
{

    if (csb->nvgen) {

	switch (csb->which) {
	   case PPP_MULTILINK_VTEMPLATE:
                if (mlp_virtual_template_num != 0)
	          nv_write(1, "%s %d", csb->nv_command, 
                           mlp_virtual_template_num);
	        break;
	   case PPP_MULTILINK_DEFAULT:
                break;
	   default:
	        bad_parser_subcommand(csb, csb->which);
	        break;
	}
	return;
    }


    switch (csb->which) {
      case PPP_MULTILINK_VTEMPLATE:
           if (csb->sense)
              mlp_virtual_template_num = GETOBJ(int,1);
           else
              mlp_virtual_template_num = 0;
           break;
      case PPP_MULTILINK_DEFAULT:
           break;
      default:
           bad_parser_subcommand(csb, csb->which);
	   break;      
    }
}

/*
 * Get Virtual template interface for MLP 
 */
int mlp_get_vtemplate_num (void)
{
    return(mlp_virtual_template_num);
}

/*
 * multilink_command
 * process multilink configuration commands
 */

void multilink_command (parseinfo *csb)
{
    hwidbtype *idb;
    lcp_state *lcpstate;

    idb = csb->interface->hwptr;
    lcpstate = idb->lcpstate;

    if (csb->nvgen) {
	switch (csb->which) {
	case MULTILINK_THRESHOLD:
	    if (lcpstate->mlp_threshold_confg) {
		nv_write(TRUE, "%s %d",
			 csb->nv_command, lcpstate->mlp_threshold);
		nv_add((lcpstate->mlp_threshold_direction ==
			MULTILINK_THRESH_OUTBOUND), " outbound");
		nv_add((lcpstate->mlp_threshold_direction ==
			MULTILINK_THRESH_INBOUND), " inbound");
	    }
	    break;

	case MULTILINK_MIN_LINKS:
	    nv_write(lcpstate->mlp_min_links,
		     "%s %d", csb->nv_command, lcpstate->mlp_min_links);
	    break;

	case MULTILINK_MAX_LINKS:
	    nv_write(lcpstate->mlp_max_links != MAXINT,
		     "%s %d", csb->nv_command, lcpstate->mlp_max_links);
	    break;

	case MULTILINK_MAX_FRAGS:
	    nv_write(lcpstate->mlp_max_frags != MLP_DEFAULT_MAX_FRAGS,
		     "%s %d", csb->nv_command, lcpstate->mlp_max_frags);
	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case MULTILINK_THRESHOLD:
  	if (csb->sense) {
 	    lcpstate->mlp_threshold_confg = TRUE;
 	    lcpstate->mlp_threshold = GETOBJ(int,1);
 	    lcpstate->mlp_threshold_direction = GETOBJ(int,2);
 	} else {
 	    lcpstate->mlp_threshold_confg = FALSE;
 	    lcpstate->mlp_threshold = 0;
  	}
  	break;
 	
      case MULTILINK_MIN_LINKS:
 	if (csb->sense) {
 	    lcpstate->mlp_min_links = GETOBJ(int,1);
 	} else {
 	    lcpstate->mlp_min_links = 0;
 	}
 	break;
 	
      case MULTILINK_MAX_LINKS:
 	if (csb->sense) {
 	    lcpstate->mlp_max_links = GETOBJ(int,1);
 	} else {
 	    lcpstate->mlp_max_links = MAXINT;
 	}
 	break;
 	
      case MULTILINK_MAX_FRAGS:
 	if (csb->sense) {
 	    lcpstate->mlp_max_frags = GETOBJ(int,1);
 	} else {
 	    lcpstate->mlp_max_frags = MLP_DEFAULT_MAX_FRAGS;
 	}
 	break;
 	
      default:
  	bad_parser_subcommand(csb, csb->which);
  	break;
    }
}

void mlp_setup (hwidbtype *idb)
{
    if (mlp_pid == NO_PROCESS) {
	mlp_pid = process_create(mlp_input, "Multilink PPP", NORMAL_STACK,
				 PRIO_HIGH);
    }
    if (mlp_outbound_pid == NO_PROCESS) {
	mlp_outbound_pid = process_create(mlp_output, "Multilink PPP out",
					  NORMAL_STACK, PRIO_HIGH);
    }
}
