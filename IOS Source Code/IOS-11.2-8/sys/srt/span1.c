/* $Id: span1.c,v 3.9.28.15 1996/08/28 13:16:20 thille Exp $
 * $Source: /release/112/cvs/Xsys/srt/span1.c,v $
 *------------------------------------------------------------------
 * span1.c -- Execute the Spanning Tree Protocol 
 *
 * 7-August-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: span1.c,v $
 * Revision 3.9.28.15  1996/08/28  13:16:20  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.28.14  1996/08/28  00:27:03  amcrae
 * CSCdi66507:  no bridge-group command causes crash at span_port_disable
 * Branch: California_branch
 * Added sanity check when disabling port
 *
 * Revision 3.9.28.13  1996/08/26  15:12:44  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.9.28.12  1996/08/12  02:16:09  ppearce
 * CSCdi64450:  CSCdi59527 broke c4500-boot-m image
 * Branch: California_branch
 *   Add srb_core_registry that will definitely be included w/SRB subblock
 *
 * Revision 3.9.28.11  1996/07/23  13:29:28  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.9.28.10  1996/07/09  06:05:46  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.9.28.9  1996/07/02  23:11:41  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.9.28.8  1996/06/27  19:28:18  ioakley
 * CSCdi41851:  Prevent AST BPDU storms from occurring when root bridge
 *              priority is modified in an AST over RSRB context.
 * Branch: California_branch
 *
 * Revision 3.9.28.7  1996/05/23  03:11:35  wilber
 * CSCdi58046:  Ill formatted 802.10(SDE) vLAN BPDUs
 * Remove the redundant code that lays down the SDE header
 * Branch: California_branch
 *
 * Revision 3.9.28.6  1996/05/22  02:07:33  hou
 * CSCdi56274:  7507 send out ill formated ISL encap BPDU
 * Branch: California_branch
 * - use short name string for vLAN subinterface in spanning tree
 *   debugging msg.
 *
 * Revision 3.9.28.5  1996/05/17  12:13:54  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.14.4  1996/05/06  00:01:34  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.9.14.3  1996/04/26  15:23:27  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.14.2  1996/04/25  19:31:35  ioakley
 * CSCdi53651:  Add support for hidden message-age-increment option in
 *              "source-bridge spanning X" interface command.  Permits
 *              AST interoperability in mixed vendor bridge environments
 *              where hard-coded MAX_AGE defaults are insufficient for
 *              the network diameter and MAX_AGE is not configurable.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.14.1  1996/04/03  21:59:26  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.28.4  1996/05/10  01:42:59  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.9.28.3  1996/05/09  14:46:46  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.9.28.2  1996/05/04  01:49:10  wilber
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
 * Revision 3.9.28.1.18.1  1996/04/30  22:23:13  rbadri
 * CSCdi53442:  DEC STP and LANE is broken
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * For DEC STP, port path cost for ATM interface should be set to
 * non-zero value, if the calculation results in a zero value.
 *
 * Revision 3.9.28.1  1996/03/18  22:11:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.2.2  1996/03/13  02:02:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.9.2.1  1996/03/07  10:51:13  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.9  1996/01/22  07:16:38  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1995/12/28  22:43:34  pitargue
 * CSCdi45879:  atm lane int configured as rfc 1483
 * always check how atm interfaces are configured.  don't assume past
 * states
 * of tbridge configurations.  also add some fields to show span and show
 * bridge
 * group to help debug this in the future.
 *
 * Revision 3.7  1995/12/17  06:10:34  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.6  1995/12/16  00:27:39  pitargue
 * CSCdi45515:  ST MAC and all ones MAC are off by 2 bytes in show bridge
 * make sure that aal5snap subinterfaces can correctly set the
 * tbridge_media
 * flag after it has been configured as lane
 *
 * Revision 3.5  1995/11/25  02:06:23  ioakley
 * CSCdi43577:  Correct problem of TR and FDDI not forwarding
 *              explorers when AST is configured.
 *
 * Revision 3.4  1995/11/20  15:24:54  pitargue
 * CSCdi42745:  Bad SAP error received when ping over atm remote bridging.
 * make sure that aal5snap vc's are added to the atm tbridge flood list
 *
 * Revision 3.3  1995/11/17  18:56:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:42:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:32:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:30:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/10/13  01:00:41  ioakley
 * CSCdi35149:  Eliminate extraneous generation of AST config_bpdus
 *              within an RSRB disjoint peer context.
 *
 * Revision 2.7  1995/08/29  22:50:20  wmay
 * CSCdi39519:  Frame relay outputs not fast bridged - remove setting
 * of force process flag.
 *
 * Revision 2.6  1995/08/08  16:49:44  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.5  1995/08/04 05:08:26  ioakley
 * CSCdi38105:  In IBM AST mode "source-bridge spanning" configuration
 *              for an interface should not be vaporized on the basis
 *              of disabling AST operation for a bridge port.
 *
 * Revision 2.4  1995/07/06  00:25:14  wilber
 * CSCdi36382:  Age field is not updated correctly for filtered on-net
 * traffic
 * - Create counters to account for filtered on-net traffic
 * - Show those counters only in verbose option of show bridge output
 * - Get timestamp when creating a bridge table entry
 * - Update the bte age based on rx counter plus filtered rx counter
 *
 * Revision 2.3  1995/06/22  01:12:33  wilber
 * CSCdi35783:  ieee/dec bridging crash, jump to zero when running test
 * script
 * Explicitly check if root_port is NULL before sending out TCN on
 * root_port. Under fast configuration scenario(running test script),
 * non-root bridge could become a root bridge(hence root_port becomes
 * NULL) in the middle of transmitting TCN
 *
 * Revision 2.2  1995/06/09  13:21:32  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:01:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "span.h"
#include "srb_core.h"
#include "srb_core_registry.h"
#include "media_registry.h"
#include "srt_registry.h"
#include "../ui/debug.h"
#include "srt_debug.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "../lane/lane_registry.h"
#include "../wan/frame_relay.h"
#include "../x25/x25.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../ibm/lanmgr.h"
#include "../ibm/ibmnm.h"
#include "../tbridge/tbridge_sde.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../vlan/vlan.h"
#include <ciscolib.h>

/* SRB AST support */
extern uchar *bitswapped_ieee_bridge_func;      /* 0xc000.0000.0100 */


/*
 * Small INLINE functions. These function exist to make possible
 * additional changes to the code easier; when the next revision 
 * of 802.1 comes out.
 */

inline static void stop_hello_timer (
    spantype *span)
{
    span->hello_timer = 0;
}

inline static void start_hello_timer (
    spantype *span)
{
    span->hello_timer = span->hello_time;
}

inline static void stop_tcn_timer (
    spantype *span)
{
    span->topology_change_notification_timer = 0;
}

inline static void start_tcn_timer (
    spantype *span)
{
    span->topology_change_notification_timer =
	span->topology_change_notification_time;
}

inline static void set_port_state (
    idbtype *port,
    int status)
{
    hwidbtype *porthw;

    port->port_state = status;

    porthw = port->hwptr;
    reg_invoke_srt_report_port_state(port, status);
    reg_invoke_srt_set_port_state(porthw, status);
    if (port->span_ptr->protocol == SPAN_PROTO_IBM) {
        srb_vector_table_t *srbV_p;
        srbV_p = reg_invoke_srb_get_vector_table();
        if (!srbV_p) {
            return;
        }
	if (status <= PORT_BLOCKING) {
            if (srbV_p->srb_sb_get_spanning_explorer(port)) {
                srbV_p->srb_sb_disable_spanning_explorer(port);
	        srbV_p->srbcore_modify_srb(porthw, TR_MODIFY_SPAN, FALSE);
            }
	} else if (status == PORT_FORWARDING) {
            srbV_p->srb_sb_enable_spanning_explorer(port);
	    srbV_p->srbcore_modify_srb(porthw, TR_MODIFY_SPAN, TRUE);
	}
    } else {/* business as usual */
	span_circstate(port);
    }
}

static void start_forward_delay_timer (
    spantype *span,
    idbtype *port)
{
    int delay;

    delay = span->forward_delay;
    if (span->protocol == SPAN_PROTO_DEC)
	delay = (delay >> 1);
    port->forward_delay_timer = delay;
}

inline static void stop_forward_delay_timer (
    idbtype *port)
{
    port->forward_delay_timer = 0;
}

inline static void stop_message_age_timer (
    idbtype *port)
{
    port->message_age_timer_active = FALSE;
    port->message_age_timer = 0;
}

inline static void start_message_age_timer (
    idbtype *port,
    int interval)
{
    port->message_age_timer = interval;
    port->message_age_timer_active = TRUE;
}

inline static void stop_hold_timer (
    idbtype *port)
{
    port->hold_timer = 0;
}

inline static void start_hold_timer (
    spantype *span,
    idbtype *port)
{    
    port->hold_timer = span->hold_time;
}    

inline static void stop_topology_change_timer (
    spantype *span)
{
    /* initializing spanning tree change timer */
    span->topology_change_start_time = system_uptime_seconds();
    span->topology_change_timer = 0;
}

inline static void start_topology_change_timer (
    spantype *span)
{
    span->topology_change_start_time = system_uptime_seconds(); 
    span->topology_change_timer = span->topology_change_time;
}


/*
 * spanBLOCK
 * Check if it is time to activate the span tree process.
 */
static boolean spanBLOCK (
    spantype *span)
{
    if (!QUEUEEMPTY(&span->spanQ))
	return(FALSE);
    if (span->timer_running) {
	if (!SLEEPING(span->timer))
	    return(FALSE);
    }
    return(TRUE);
}
    

/*
 * span_process
 * Process that executes the spanning tree protocol
 */

forktype span_process (
    spantype *span)
{
    idbtype *port;
    btetype *mac;
    boolean working;
    leveltype level;
    ushort i, circgroup;
    paktype *pak;

    /*
     * For the system to finish intialization before going starting.
     */
    edisms(systeminitBLOCK, 0);    
    TIMER_START(span->timer, ONESEC);
    span->timer_running = TRUE;
    while (span->running) {
	edisms((blockproc *)spanBLOCK, (ulong)span);

	/*
	 * See if we have received any spanning tree datagrams.
	 */
	while (!QUEUEEMPTY(&span->spanQ)) {
	    pak = p_dequeue(&span->spanQ);
	    if (!pak)
		break;
	    port = pak->if_input;

	    idb_increment_rx_proto_counts(pak, ACCT_PROTO_SPAN);

	    if ((!span) || (!span->running)) {
		retbuffer(pak);
		continue;
	    }
	    switch (span->protocol) {
	    case SPAN_PROTO_IBM:	/* SRB AST support */
                if ((port->hwptr->status & IDB_FDDI) ||
		    (!cmpid(bitswapped_ieee_bridge_func, 
			   ((tring_hdr *)(pak->datagramstart))->daddr,
			   IEEEBYTES))) {
                    bpdu_enqueue(span, port, pak);
		    break;
                } else {
                    retbuffer(pak);
                    continue;           /* process next BPDU */
                }
	    case SPAN_PROTO_IEEE:
		bpdu_enqueue(span, port, pak);
		break;
	    case SPAN_PROTO_DEC:
		dec_enqueue(span, port, pak);
		break;
	    default:
		retbuffer(pak);
		break;
	    }
	}

	/*
	 * Process-level recovery of AB data structures lost due to
	 * cBus initialization or SP download.
	 */
	reg_invoke_srt_shifa_recovery();

	/*
	 * Check if it's time to check our timers.
	 */
	if (!span->timer_running)
	    continue;
	if (SLEEPING(span->timer))
	    continue;
	TIMER_UPDATE(span->timer, ONESEC);


	/*
	 * Check bridge global timers
	 * First, the HELLO timer (4.7.3)
	 */
	if (span->hello_timer) {
	    if (--span->hello_timer == 0) {
		config_bpdu_generation(span);
	
		start_hello_timer(span);
	    }
	}
	
	/*
	 * Topology Change Notification (4.7.6)
	 */
	if (span->topology_change_notification_timer) {
	    if (--span->topology_change_notification_timer == 0) {
		tcn_timer_expiry(span);
	    }
	}
	
	/*
	 * Topology Change timer (4.7.7)
	 */
	if (span->topology_change_timer) {
	    if (--span->topology_change_timer == 0) {
		topology_change_timer_expiry(span);
	    }
	}
	
	/*
	 * DEC's "use short ageout" timer.
	 */
	if (span->shorttimer)
	    --span->shorttimer;
	
	/*
	 * Circuit Group Pause timer
	 */
        for (circgroup = 1; circgroup < CCG_MAXGROUPS; circgroup++) {
	    if (TIMER_RUNNING(span->cct_groups[circgroup].ccg_pause_timer) &&
		AWAKE(span->cct_groups[circgroup].ccg_pause_timer))
		ccg_pause_timer_expiry(span, circgroup);
	}

	/*
	 * Check port specific timers
	 */
	for (i = 0; i < span->port_count; i++) {
	    port = span->port[i];
	    
  	    /*
	     * Make sure only usable interfaces are in the circuit group.
	     */
	    if (port->circ_group) {
		working = interface_up(port);
		if ((working && !port->circ_inserted) ||
		    (!working && port->circ_inserted)) {
		    /* This interface has changed up/down state */

		    /*
		     * Fast STP Circuit Recovery
		     * If this port has forward_delay timer running,
		     * immediately expire it.
		     */
		    if (port->forward_delay_timer)
			forward_delay_timer_expiry(span, port);

		    span_circfix(port, port->circ_group);
		}
	    }
	    
	    /*
	     * Forward delay timer (4.7.5)
	     */
	    if (port->forward_delay_timer) {
		if (--port->forward_delay_timer == 0) {
		    forward_delay_timer_expiry(span, port);
		}
	    }
	    
	    /*
	     * Message age timer (4.7.4)
	     */
	    if (port->message_age_timer_active) {
		/* SRB AST support */
                /*
                 * If disjoint_peers is set for this virtual interface,
                 * there is a Root bridge that I am not peered with, and
                 * whoever detected this situation, notified me.
		 * In this case, I will not be receiving Hello BPDUs from
		 * the Root, so I need to make sure that I generate Hello
		 * BPDUs to my designated ports.
		 * This is sickening, but I need to cover the case where
		 * customer is running STP in a non fully meshed RSRB network.
                 */
		if ((span->protocol == SPAN_PROTO_IBM) &&
		    (port->hwptr->status & IDB_VIRTUAL) &&
		    port->disjoint_peers) {
		    if (++port->message_age_timer >= span->hello_time) {
                        port->message_age_timer = 0;
			port->disjoint_bpdu_generation = TRUE;
                        config_bpdu_generation(span);
                    }
		} else { /* Normal case */
	            if (++port->message_age_timer >= span->max_age) {
	                port->message_age_timer_active = FALSE;
	                port->message_age_timer = 0;
	                message_age_timer_expiry(span, port);
	            }
		    port->disjoint_bpdu_generation = FALSE;
	        }
	    }
	    
	    /*
	     * Update filter timer (4.7.8)
	     */
	    if (port->hold_timer) {
		if (--port->hold_timer == 0) {
		    hold_timer_expiry(span, port);
		}
	    }
	}
	
	/*
	 * Update the timer for the last input from station address.
	 */
	for (i = 0; i < NETHASHLEN; i++) {
	    level = raise_interrupt_level(NETS_DISABLE);
	    mac = (btetype *) span->bridge_tbl[i];
	    while (mac) {
		if ((mac->bte_prev_rxcount != 
		     (mac->bte_rxcount + mac->bte_filtered_rxcount)) &&
		    (mac->bte_interface) && mac->bte_interface->circ_canlearn) {
		    GET_TIMESTAMP(mac->bte_timer);
		    mac->bte_prev_rxcount = (mac->bte_rxcount +
					     mac->bte_filtered_rxcount);
		}
		mac = mac->bte_next;
	    }
	    reset_interrupt_level(level);
	} 
	
	/*
	 * See if we need more bridge table entries.
	 */
	/* SRB AST support */
	if (span->protocol != SPAN_PROTO_IBM)
	    if (bte_count < BTE_MINIMUM)
	        grow_stations(BTE_ALIQUOT);
    }
    span->timer_running = FALSE;
    process_kill(THIS_PROCESS);
}

/*
 * transmit_config
 * Send a HELLO on the specified port (4.6.1)
 */

void transmit_config (spantype *span, idbtype *port)
{
    paktype *pak;
    
    if (port->hold_timer) {
	port->config_pending = TRUE;
	return;
    }
    pak = getbuffer(MINETHERSIZE);
    if (!pak)
	return;
    pak->peer_ptr = NULL;

    switch (span->protocol) {
    case SPAN_PROTO_IBM:	/* SRB AST support */
    case SPAN_PROTO_IEEE:
	ieee_transmit_config(span, port, pak);
	break;
    case SPAN_PROTO_DEC:
	dec_transmit_config(span, port, pak);
	break;
    default:
	retbuffer(pak);
	return;
    }
    port->config_pending = FALSE;
    start_hold_timer(span, port);

    /* SRB AST support */
    if ((span->protocol == SPAN_PROTO_IBM) && 
	(port->hwptr->status & IDB_VIRTUAL)) {
        port->disjoint_peers = FALSE;
    }

    /*
     * If LANE is configured on the interface, send a message to the
     * client, indicating no change in topology. 
     */
    if (is_atm_lane(port)) {
	reg_invoke_lec_send_topo_change(port->lane_info->lec_info, FALSE);
    }
}

/*
 * root_bridge
 * Returns TRUE if we're the root of the spanning tree
 */

boolean root_bridge (
    spantype *span)
{
    return(cmpid(span->designated_root, span->bridge_id,
		 sizeof(span->bridge_id)) == 0);
}

/*
 * ieee_supersedes_port_info
 * Returns TRUE if this IEEE HELLO changes our port information (4.6.2.2).
 */
boolean ieee_supersedes_port_info (
    spantype *span,
    idbtype *port,
    paktype *pak)
{
    bpduhdrtype *bpdu;
    int root, cost, designated_bridge, bridge_id, port_id, disjoint_check;

    disjoint_check = 1;
    
    bpdu = (bpduhdrtype *)bpduheadstart(pak);

    if (port->span_ptr->protocol == SPAN_PROTO_IBM) {
        disjoint_check = cmpid(&bpdu->root_id[2], &port->designated_root[2],
			      IEEEBYTES);
    }
  
    root = cmpid(bpdu->root_id, port->designated_root,
		 sizeof(port->designated_root));
    if (root != 0) {
	if (disjoint_check == 0) {
	    port->disjoint_bpdu_generation = TRUE;
	}
	else {
	    if (spanevents_debug) {
	        buginf("\nST: Heard root %-5d-%e on %s",
		       PRIORITY(bpdu->root_id), &bpdu->root_id[2],
		       port->namestring);
		if (root < 0)
		    buginf("\n    Supersedes %-5d-%e",
			   PRIORITY(port->designated_root),
			   &port->designated_root[2]);
	    }
	}
	return(root < 0);
    }
    
    /*
     * Roots are equal, further checks needed.
     */
    cost = cmpid(bpdu->root_path_cost, (uchar *)&port->designated_cost, 
		 sizeof(port->designated_cost));
    if (cost != 0)
	return(cost < 0);
    
    /*
     * Costs are equal, further checks are needed.
     */
    designated_bridge = cmpid(bpdu->bridge_id, port->designated_bridge,
			      sizeof(port->designated_bridge));
    if (designated_bridge != 0)
	return (designated_bridge < 0);
    
    /*
     * Designated bridges are equal, further checks are needed.
     */
    bridge_id = cmpid(bpdu->bridge_id, span->bridge_id, 
		      sizeof(span->bridge_id));
    if (bridge_id != 0)
	return(TRUE);
    
    port_id = cmpid(bpdu->port_id, (uchar *)&port->designated_port,
		    sizeof(port->designated_port));
    return(port_id <= 0);
}

/*
 * dec_supersedes_port_info
 * Returns TRUE if this DEC HELLO changes our port information (4.6.2.2)
 */

boolean dec_supersedes_port_info (
    spantype *span,
    idbtype *port,
    paktype *pak)
{
    dechdrtype *dec;
    boolean root_comp, bridge_comp;
    
    dec = (dechdrtype *)decheadstart(pak);

    /*
     * Supersede if the root ID is greater than our current designated root
     * (4.6.2.2.1).
     */
    root_comp = cmpid(dec->root_id, port->designated_root, 
		      sizeof(port->designated_root));
    if (root_comp < 0) {
	if (spanevents_debug) {
	    buginf("\nST: Heard root %-5d-%e on %s", PRIORITY(dec->root_id),
		   &dec->root_id[2], port->namestring);
	    buginf("\n    Supersedes %-5d-%e",
		   PRIORITY(port->designated_root),
		   &port->designated_root[2]);
	}

	return(TRUE);
    }
    /*
     * Supersede if ID's are the same, but the root path cost is less
     * than the designated cost of this port (4.6.2.2.2).
     */
    if (root_comp != 0)
	return(FALSE);
    if (dec->root_path_cost < (ushort) port->designated_cost) {
	if (spanevents_debug) {
	    buginf("\nST: Heard root %-5d-%e on %s", PRIORITY(dec->root_id),
		   &dec->root_id[2], port->namestring);
	    buginf("\n    Supersedes cost %d %d",
		   port->designated_cost,
		   dec->root_path_cost);
	}

	return(TRUE);
    }
    /*
     * If root ID's and path costs are the same, then supersede if the 
     * bridge ID in the HELLO has a greater priority than the recorded
     * designated bridge for this port (4.6.2.2.3).
     */
    if (dec->root_path_cost != (ushort) port->designated_cost)
	return(FALSE);
    bridge_comp = cmpid(dec->bridge_id, port->designated_bridge,
			sizeof(port->designated_bridge));
    if (bridge_comp < 0)
	return(TRUE);
    
    /*
     * If all previous conditions resulted in equalities, supersede if
     * we are not the designated bridge for the port or if the port ID
     * of the HELLO is greater than the Designated port ID (4.6.2.2.4).
     */
    if (bridge_comp != 0)
	return(FALSE);
    if (cmpid(dec->bridge_id, span->bridge_id, sizeof(span->bridge_id)) != 0)
	return(TRUE);
    return(dec->port_id <= port->designated_port);
}

/*
 * ieee_record_config_information
 * Move hello information into the port table (4.6.2).
 */
void ieee_record_config_information (idbtype *port, paktype *pak)
{
    bpduhdrtype *bpdu;

    bpdu = (bpduhdrtype *)bpduheadstart(pak);

    bcopy(bpdu->root_id, port->designated_root, sizeof(port->designated_root));
    bcopy(bpdu->root_path_cost, &port->designated_cost, 
	  sizeof(port->designated_cost));
    bcopy(bpdu->bridge_id, port->designated_bridge, 
	  sizeof(port->designated_bridge));
    port->designated_port = GETSHORT(bpdu->port_id);
    start_message_age_timer(port, (GETSHORT(bpdu->message_age) / 256));
}

/*
 * ieee_record_config_timeout_values
 * Record new timeout values for spanning tree (4.6.3).
 */
void ieee_record_config_timeout_values (spantype *span, paktype *pak)
{
    bpduhdrtype *bpdu;

    bpdu = (bpduhdrtype *)bpduheadstart(pak);

    span->max_age = (GETSHORT(bpdu->max_age) / 256);
    span->hello_time = (GETSHORT(bpdu->hello_time) / 256);
    span->forward_delay = (GETSHORT(bpdu->forward_delay) / 256);
    span->topology_change = bpdu->tc;
}

/*
 * dec_record_config_information
 * Move hello information into the port table (4.6.2).
 */
void dec_record_config_information (idbtype *port, paktype *pak)
{
    dechdrtype *dec;

    dec = (dechdrtype *)decheadstart(pak);
    
    bcopy(dec->root_id, port->designated_root, sizeof(port->designated_root));
    port->designated_cost = GETSHORT(&dec->root_path_cost);
    bcopy(dec->bridge_id, port->designated_bridge, 
	  sizeof(port->designated_bridge));
    port->designated_port = dec->port_id;
    start_message_age_timer(port, dec->message_age);
}

/*
 * dec_record_config_timeout_values
 * Record new timeout values for spanning tree (4.6.3).
 */
void dec_record_config_timeout_values (spantype *span, paktype *pak)
{
    dechdrtype *dec;

    dec = (dechdrtype *)decheadstart(pak);

    span->max_age = dec->max_age;
    span->hello_time = dec->hello_time;
    span->forward_delay = dec->forward_delay;
    span->topology_change = dec->tc;
}

/*
 * config_bpdu_generation
 * Generate HELLOs on all ports that are the designated port (4.6.4).
 */
void config_bpdu_generation (
    spantype *span)
{
    int i;
    idbtype *port;
    
    for (i = 0; i < span->port_count; i++) {
	port = span->port[i];
	if ((designated_port(span, port)) && 
	    interface_up(port) && (port->port_state != PORT_DISABLED)) {
	    if (port->port_state == PORT_BLOCKING) {
		/*
		 * Shouldn't send BPDUs from a BLOCKING port.
		 * Something is wrong and we should adjust the port state.
		 * See one example in CSCdi32843.
		 */
		tbridge_adjust(port);
	    } else {
		transmit_config(span, port);
	    }
	}
    }
}

/*
 * designated_port
 * Check if the port is a designated port
 */
boolean designated_port (spantype *span, idbtype *port)
{
    if ((cmpid(port->designated_bridge, span->bridge_id, IDBYTES) == 0) &&
	(port->designated_port == port->port_id)) {
	  if (span->protocol == SPAN_PROTO_IBM) {
	    if (!(ibm_port_priority_check(span,port->ibm_port_priority)))
	      return(FALSE);
          }
          return(TRUE);
    }
    return(FALSE);
}

/*
 * transmit_tcn
 * Transmit a topology change notification
 */
void transmit_tcn (
    spantype *span)
{
    paktype *pak;
    
    pak = getbuffer(MINETHERSIZE);
    if (!pak || !span->root_port)
	return;
/*
 * Possible BOGOSITY ALERT!!!  It looks like the above can cause a
 * packet leak if span->root_port is NULL.  I believe that it should 
 * just return if pak is NULL, and datagram_done(pak); then return if
 * span->root_port is NULL.
 */
    switch (span->protocol) {
    case SPAN_PROTO_IEEE:
	ieee_transmit_tcn(span->root_port, pak, span->domain);
	break;
    case SPAN_PROTO_DEC:
	dec_transmit_tcn(span->root_port, pak);
	break;
    case SPAN_PROTO_IBM:	
	if (spanevents_debug)
	    buginf("\nST: ibm spanning tree TCN dropped");
	/* fall through */
    default:
	retbuffer(pak);
	break;
    }
    if (spanevents_debug)
	buginf("\nST: sent Topology Change Notice on %s",
	       span->root_port->namestring);
    /*
     * If LANE is configured on the interface, send a message to the
     * client, indicating change in topology. 
     */
    if (is_atm_lane(span->root_port)) {
	reg_invoke_lec_send_topo_change(span->root_port->lane_info->lec_info, TRUE);
    }

}
	
/*
 * configuration_update
 * Update our knowledge of the spanning tree after recieving a HELLO (4.6.7)
 */
void configuration_update (
    spantype *span)
{
    root_selection(span);
    designated_port_selection(span);
}

/*
 * root_selection
 * Select our designated root and root port (4.6.8)
 */

void root_selection (
    spantype *span)
{
    int i, cmp, port_cost, root_cost;
    idbtype *port, *root, *oldroot;
    
    oldroot = span->root_port;
    root = NULL;
    for (i = 0; i < span->port_count; i++) {
	port = span->port[i];
	if (port) {
	    if ((designated_port(span, port) == FALSE) &&
		(port->port_state != PORT_DISABLED) &&
		(cmpid(port->designated_root, span->bridge_id, 
		       sizeof(span->bridge_id)) < 0)) {
		if (root == NULL) {
		    root = port;
		    continue;
		}
		cmp = cmpid(port->designated_root, root->designated_root,
			    sizeof(root->designated_root));
		if (cmp != 0) {
		    if (cmp < 0)
			root = port;
		    continue;
		}
		
		/*
		 * The designated roots are equal, check cost.
		 */
		port_cost = port->designated_cost + port->path_cost;
		root_cost = root->designated_cost + root->path_cost;
		if (port_cost != root_cost) {
		    if (port_cost < root_cost)
			root = port;
		    continue;
		}
		
		/*
		 * The path costs are equal, check designated bridge.
		 */
		cmp = cmpid(port->designated_bridge, root->designated_bridge,
			    sizeof(root->designated_bridge));
		if (cmp != 0) {
		    if (cmp < 0)
			root = port;
		    continue;
		}
		
		/*
		 * The designated bridges are equal, check designated ports.
		 */
		if (port->designated_port != root->designated_port) {
		    if (port->designated_port < root->designated_port)
			root = port;
		    continue;
		}
		
		if (port->port_id < root->port_id)
		    root = port;
	    }
	}
    }
    span->root_port = root;
    if (root == NULL) {
	if (spanevents_debug && (oldroot != NULL))
	    buginf("\nST: we are the spanning tree root");
	span->root_port_id = 0;
	bcopy(span->bridge_id, span->designated_root, 
	      sizeof(span->designated_root));
	span->root_path_cost = 0;
    } else { 
	span->root_port_id = root->port_id;
	bcopy(root->designated_root, span->designated_root,
	      sizeof(span->designated_root));	
	span->root_path_cost = root->designated_cost + root->path_cost;
	if (spanevents_debug && (oldroot != root))
	    buginf("\nST: new root is %d, %e on port %s, cost %d", 
		   PRIORITY(span->designated_root), &span->designated_root[2],
		   span->root_port->namestring, span->root_path_cost);
    }
}
    
/*
 * designated_port_selection
 * Figure out who is our designated port.
 * This function must be called after root selection (4.6.9).
 */

void designated_port_selection (
    spantype *span)
{
    idbtype *port;
    int i, cmp;
    
    for (i = 0; i < span->port_count; i++) {
	port = span->port[i];
	if (designated_port(span, port)) {
	    become_designated_port(span, port);
	    continue;
	}
	
	cmp = cmpid(port->designated_root, span->designated_root,
		    sizeof(span->designated_root));
	if (cmp != 0) {
	    become_designated_port(span, port);
	    continue;
	}
	if (span->root_path_cost < port->designated_cost) {
	    become_designated_port(span, port);
	    continue;
	}
	if (span->root_path_cost == port->designated_cost) {
	    cmp = cmpid(span->bridge_id, port->designated_bridge,
			sizeof(port->designated_bridge));
	    if (cmp < 0) {
		become_designated_port(span, port);
		continue;
	    }
	    if ((cmp == 0) && (port->port_id <= port->designated_port)) {
		become_designated_port(span, port);
		continue;
	    }
	}	    
    }
}

/*
 * become_designated_port
 * Make the current port the designated port (4.6.10)
 */

void become_designated_port (
    spantype *span,
    idbtype *port)
{
    bcopy(span->designated_root, port->designated_root, 
	  sizeof(port->designated_root));
    port->designated_cost = span->root_path_cost;
    bcopy(span->bridge_id, port->designated_bridge,
	  sizeof(port->designated_bridge));
    port->designated_port = port->port_id;
}

/*
 * port_state_selection
 * Adjust port states given current topology (4.6.11)
 */

void port_state_selection (
    spantype *span)
{
    idbtype *port;
    int i;
    
    for (i = 0; i < span->port_count; i++) {
	port = span->port[i];
	if (port == span->root_port) {
	    port->config_pending = FALSE;
	    port->topology_change_acknowledge = FALSE;
	    (void) make_forwarding(span, port);
	} else if (designated_port(span, port)) {
	    stop_message_age_timer(port);
	    (void) make_forwarding(span, port);
	} else {
	    port->config_pending = FALSE;
	    port->topology_change_acknowledge = FALSE;
	    (void) make_blocking(span, port);
	}
    }
}

/*
 * jump_to_forwarding
 *
 * Bypass the port state transition and jump into forwarding state.
 * Used by circuit-group to have Fast STP Circuit Recovery.
 *
 * Return TRUE if we enter the forwarding state.
 */

static boolean jump_to_forwarding (spantype *span, idbtype *port)
{
    if (!span || !interface_up(port))
        return(FALSE);

    if (spanevents_debug)
        buginf("\nST: %s ->jump to forwarding from %s", port->namestring,
               bstate2string(port->port_state));
    set_port_state(port, PORT_FORWARDING);
    port->forward_transition_count++;
    if (designated_for_some_port(span))
        topology_change_detection(span);
    return (TRUE);
}

/*
 * make_forwarding
 * Set port state to PORT_FORWARDING (4.6.12)
 * Stay in blocking state if the interface is not up.
 */

boolean make_forwarding (spantype *span, idbtype *port)
{
    if (!span)
	return(FALSE);
    if (port->port_state != PORT_BLOCKING)
	return(FALSE);

    /*
     * Fast STP Circuit Recovery
     * If this port should go to forwarding state decided by STP and has
     * circuit-group configured, try to bypass the port transition and
     * enter the forwarding state directly.
     */
    if (port->circ_group)
        if (jump_to_forwarding(span, port))
            return (TRUE);

    set_port_state(port, PORT_LISTENING);
    start_forward_delay_timer(span, port);
    if (spanevents_debug)
	buginf("\nST: %s -> listening", port->namestring);
    return(TRUE);
}

/*
 * make_blocking
 * Set port state to PORT_BLOCKING (4.6.13)
 *
 * returns true if we actually changed state.
 */

boolean make_blocking (spantype *span, idbtype *port)
{
    if (!span)
	return(FALSE);
    if ((port->port_state == PORT_DISABLED) ||
	(port->port_state == PORT_BLOCKING))
	return(FALSE);
    if ((port->port_state == PORT_FORWARDING) ||
	(port->port_state == PORT_LEARNING))
	topology_change_detection(span);
    set_port_state(port, PORT_BLOCKING);
    stop_forward_delay_timer(port);
    if (spanevents_debug)
	buginf("\nST: %s -> blocking", port->namestring);
    return(TRUE);
}

/*
 * topology_change_detection
 * Record that a topology change has been detected so we can notify
 * the root that things have changed (4.6.14).
 */

void topology_change_detection (
    spantype *span)
{
    if (span->protocol == SPAN_PROTO_IBM) {
	return;
    }

    start_topology_change_timer(span);

    if (root_bridge(span)) {
	span->topology_change = TRUE;
	start_topology_change_timer(span);
	if (span->protocol == SPAN_PROTO_DEC)
	    span->shorttimer = SPAN_DEC_SHORTTIME;
    } else if (span->topology_change_detected == FALSE) {
	transmit_tcn(span);
	start_tcn_timer(span);
    }
    span->topology_change_detected = TRUE;
    span->topology_change_count++;
}

/*
 * topology_change_acknowledged
 * Acknowledge that a topology change has been answered (4.6.15).
 */
void topology_change_acknowledged (
    spantype *span)
{
    span->topology_change_detected = FALSE;
    stop_tcn_timer(span);
}

/*
 * acknowledge_topology_change
 * Reply to a topology change (4.6.16).
 */
void acknowledge_topology_change (spantype *span, idbtype *port)
{
    port->topology_change_acknowledge = TRUE;
    gc_stations (span, TRUE);
    transmit_config(span, port);
}


/*
 * ieee_received_config_bpdu
 * Process an IEEE config message (4.7.1)
 */

void ieee_received_config_bpdu (
    spantype *span,
    idbtype *port,
    paktype *pak)
{
    boolean root;
    bpduhdrtype *bpdu;

    bpdu = (bpduhdrtype *)bpduheadstart(pak);
    
    /* We should check if the "message age" has been already
     * larger then MAX AGE. To prevent wrong info to be
     * passed back and forth between routers in extensively
     * redundant topology.
     */
    if (GETSHORT(bpdu->message_age) >= GETSHORT(bpdu->max_age))
      return;

    root = root_bridge(span);
    if (port->port_state != PORT_DISABLED) {
	if (ieee_supersedes_port_info(span, port, pak)) {
	    ieee_record_config_information(port, pak);
	    configuration_update(span);
	    port_state_selection(span);
	    if ((!root_bridge(span)) && root) {
		stop_hello_timer(span);
		if (span->topology_change_detected) {
		    stop_topology_change_timer(span);
		    transmit_tcn(span);
		    start_tcn_timer(span);
		}
	    }
	    
	    if (port == span->root_port) {
		ieee_record_config_timeout_values(span, pak);
		if (port->disjoint_bpdu_generation == FALSE) {    
		    config_bpdu_generation(span);
                }
		else 
		    port->disjoint_bpdu_generation = FALSE;

		if (bpdu->tc_acknowledgement)
		    topology_change_acknowledged(span);
	    }
	} else if (designated_port(span, port)) {
	    /* 
	     * This is the case where spanning tree information that I have
	     * is better than what I just heard. In this case, I should let
	     * the other guys know that I am the designated port and let them
	     * know the information that I have.
	     */
	    transmit_config(span, port);
	} else if ((span->protocol == SPAN_PROTO_IBM) &&   
		   (port->hwptr->status & IDB_VIRTUAL)) {

	/* SRB AST support */
	/* 
	 * This is an AST unique case. What this means is that the info that
	 * I heard is not better than what I know and this port is not the
	 * designated port. This can happen in a non fully meshed RSRB network.
	 * Basically, there are RSRB peers that do not communicate, not peered,
	 * and do not share spanning tree information. That's why I am hearing
	 * more than one BPDUs. Because of RSRB design, I can not use idb per
	 * peer and I am using one virtual idb to represent whole cloud. 
	 * This scheme has one big implication and that is:
	 * There could be a peer(A) whose vidb is my designated port and at the
	 * same time there could be another peer(B) who does not know about 
	 * peer A, and sends me BPDUs. In this case, since this port(vidb) is
	 * not a designated port, I will not reply back to peer B and cause
	 * all kinds of problem. So what I need to do is to let peer B know that
	 * I have a designated port and it should not send me BPDUs
	 * Since vrp input routines marked which vrp this pak is from, I should
	 * be able to send reply back to that particular peer.
	 */

	    ast_peer_transmit_config(span, port, pak->peer_ptr);
    	    if (spanevents_debug)
                buginf("\nST: Root bridge %e is not peered with bridge %e",
            	    &port->designated_root[2], &bpdu->bridge_id[2]);
	}
    }
}


/*
 * received_tcn_bpdu
 * Handle Topology Change Notification (4.7.2)
 */

void received_tcn_bpdu (spantype *span, idbtype *port)
{
    gc_stations(span, TRUE);
    if (port->port_state != PORT_DISABLED) {
	if (designated_port(span, port)) {
	    if (spanevents_debug)
		buginf("\nST: Topology Change rcvd on %s", port->namestring);
	    topology_change_detection(span);
	    acknowledge_topology_change(span, port);
	}
    }
} 

/*
 * hello_timer_expiry
 * The hello timer expired, generation another hello (4.7.3).
 */

void hello_timer_expiry (spantype *span, idbtype *port)
{
    /*
     * Don't waste resources if the interface is down.
     */
    if (interface_up(port)) {
	config_bpdu_generation(span);
        start_hello_timer(span);
    }
}

/*
 * message_age_timer_expiry
 * The message age timer expired (4.7.4)
 */
void message_age_timer_expiry (spantype *span, idbtype *port)
{
    boolean root;
    
    root = root_bridge(span);
    become_designated_port(span, port);
    configuration_update(span);
    port_state_selection(span);
    /* I was not the root, but now I am */
    if ((root_bridge(span)) && (!root)) {
	span->max_age = span->bridge_max_age;
	span->hello_time = span->bridge_hello_time;
	span->forward_delay = span->bridge_forward_delay;
	topology_change_detection(span);
	stop_tcn_timer(span);
	config_bpdu_generation(span);
	start_hello_timer(span);
    }
}

/*
 * forward_delay_timer_expiry
 * The forward delay timer expired (4.7.5).
 */

void forward_delay_timer_expiry (spantype *span, idbtype *port)
{
    if (!interface_up(port)) {
	start_forward_delay_timer(span, port);
    } else if (port->port_state == PORT_LISTENING) {
        if (port->circ_group) {
            /*
             * Fast STP Circuit Recovery
             * If this port has circuit-group configured, bypass the
             * learning state and enter the forwarding state directly.
             */
            jump_to_forwarding(span, port);
        } else {
	    set_port_state(port, PORT_LEARNING);
	    start_forward_delay_timer(span, port);
	    if (spanevents_debug)
		buginf("\nST: %s -> learning", port->namestring);
	}
    } else if (port->port_state == PORT_LEARNING) {
	set_port_state(port, PORT_FORWARDING);
	port->forward_transition_count++;
	if (designated_for_some_port(span))
	    topology_change_detection(span);
	if (spanevents_debug)
	    buginf("\nST: %s -> forwarding",port->namestring);
    }
}

/*
 * designated_for_some_port
 *
 */
boolean designated_for_some_port (
    spantype *span)
{
    int i;
    idbtype *port;
    
    for (i = 0; i < span->port_count; i++) {
	port = span->port[i];
	if (cmpid(port->designated_bridge, span->bridge_id,
		  sizeof(port->designated_bridge)) == 0)
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * tcn_timer_expiry
 * Topology change notification timer expired (4.7.6)
 */
void tcn_timer_expiry (
    spantype *span)
{
    transmit_tcn(span);
    start_tcn_timer(span);
}

/*
 * topology_change_timer_expiry
 * Topology change timer expired (4.7.7).
 */
void topology_change_timer_expiry (
    spantype *span)
{
    span->topology_change_detected = FALSE;
    span->topology_change = FALSE;
}

/*
 * hold_timer_expiry
 * Hold timer expired (4.7.8).
 */
void hold_timer_expiry (spantype *span, idbtype *port)
{
    if (port->config_pending)
	transmit_config(span, port);
}

/*
 * span_initialize
 * Initialize a spanning tree process (4.8.1)
 */

void span_initialize (
    spantype *span)
{
    idbtype *port;
    hwidbtype *porthw;
    int i;


    /*
     * Initialize the MIB variables
     */
    span->topology_change_count = 0;
    span->learned_entry_discards = 0;
    span->cbus_bce_count = 0;

    /*
     * Initialize Circuit Group variables
     */
    for (i = 1; i < CCG_MAXGROUPS; i++) {
	span->cct_groups[i].ccg_pause_interval = CCG_DEFAULT_PAUSE_INTERVAL;
	span->cct_groups[i].srcmac_based = FALSE;
    }

    /*
     * Initialize Bridge ID
     */
    span_bridge_id_address_select(span);

    /*
     * Initialize the BVI(Bridge-group Virtual Interface's MTU
     */
    if (span->irb_bg_vidb)
	tbridge_bvi_mtu_select(span->irb_bg_vidb->hwptr);

    /*
     * Initialize spanning tree parameters
     */
    bcopy(span->bridge_id, span->designated_root, 
	  sizeof(span->designated_root));
    span->root_path_cost = 0;
    span->root_port_id = 0;
    span->root_port = NULL;
    span->max_age = span->bridge_max_age;
    span->hello_time = span->bridge_hello_time;
    span->forward_delay = span->bridge_forward_delay;
    span->hold_time = SPAN_DEFAULT_HOLDTIME;   
    span->topology_change_time = SPAN_DEFAULT_TCTIME;
    span->topology_change_notification_time = SPAN_DEFAULT_TCNTIME;
    span->topology_change_detected = FALSE;
    span->topology_change = FALSE;
    stop_tcn_timer(span);
    stop_topology_change_timer(span);
    
    /*
     * Initialize port parameters
     */
    for (i = 0; i < span->port_count; i++) {
	port = span->port[i];
	porthw = port->hwptr;
 	/* SRB AST support */
	if ((span->protocol == SPAN_PROTO_IBM) &&
	    (porthw->status & IDB_VIRTUAL)) {
	    if (!reg_invoke_srt_permit_vidb_span(port)) {
	        /* 
	         * if this vidb is not ready to run STP, then don't initialize it
	         * yet. This port will get initialized when peers open up.
	         */
	        continue;
	    }
	}

	become_designated_port(span, port);
	set_port_state(port, PORT_BLOCKING);
	port->topology_change_acknowledge = FALSE;
	port->config_pending = FALSE;
	stop_message_age_timer(port);
	stop_forward_delay_timer(port);
	stop_hold_timer(port);
	port->forward_transition_count = 0;
        if (is_x25(porthw)) {
	    porthw->x25_bridge = TRUE;
	    porthw->span_bridge_process_force |= SPAN_BR_PROC_X25_BRIDGE;
	}
	if (is_mlapb_only(porthw)) {
	    porthw->mlapb_bridge = TRUE;
	    porthw->span_bridge_process_force |= SPAN_BR_PROC_MLAPB_BRIDGE;
	}
 	if ( is_frame_relay(porthw) ) {
 	    porthw->frame_relay_stuff->fr_bridge = TRUE;
	}
	if ( is_smds(porthw) ) {
 	    porthw->smds_bridge = TRUE;
	}
	if (is_ppp(porthw)) {
	    porthw->ppp_bridge = TRUE;
	    porthw->span_bridge_process_force |= SPAN_BR_PROC_PPP_BRIDGE;
	}
	if (is_atm(porthw)) {
	    /*
	     * since subinterfaces can bridge make sure to set it here.
	     * For LANE, it is already initialized.
	     */
	    if (lec_enabled(port))
		port->tbridge_media = TBR_LANE_ETHER;
	    else
		port->tbridge_media = TBR_ATM_TRANSIT;

	}

	if (is_cpp(porthw)) {
	    porthw->span_bridge_process_force |= SPAN_BR_PROC_PPP_BRIDGE;
	}
	/*
	 * This call to span_circfix is required to build the
	 * circuit groups after the serial line states have had
	 * a chance to settle after reload.
	 */
	if (port->circ_group)
	    span_circfix(port, port->circ_group);
    }
    tbridge_check_cbus_copy_capable(span);
    port_state_selection(span);
    config_bpdu_generation(span);
    start_hello_timer(span);

}

/*
 * span_port_enable
 * Initialize a spanning tree port (4.8.2)
 */

void span_port_enable (spantype *span, idbtype *port)
{
    hwidbtype *porthw = port->hwptr;

    reg_invoke_srt_cbus_bridge_port_enable(span,port);
    become_designated_port(span, port);
    set_port_state(port, PORT_BLOCKING);
    port->topology_change_acknowledge = FALSE;
    port->config_pending = FALSE;
    stop_message_age_timer(port);
    stop_forward_delay_timer(port);
    stop_hold_timer(port);
    port_state_selection(span);
    if (is_x25(porthw)) {
	porthw->x25_bridge = TRUE;
	porthw->span_bridge_process_force |= SPAN_BR_PROC_X25_BRIDGE;
    }
    if (is_mlapb_only(porthw)) {
	porthw->mlapb_bridge = TRUE;
	porthw->span_bridge_process_force |= SPAN_BR_PROC_MLAPB_BRIDGE;
    }
    if ( is_frame_relay(porthw) ) {
	porthw->frame_relay_stuff->fr_bridge = TRUE;
    }
    if ( is_smds(porthw) ) {
        porthw->smds_bridge = TRUE;
    }
    if (is_ppp(porthw)) {
	porthw->ppp_bridge = TRUE;
	porthw->span_bridge_process_force |= SPAN_BR_PROC_PPP_BRIDGE;
    }
    if (is_cpp(porthw)) {
	porthw->span_bridge_process_force |= SPAN_BR_PROC_PPP_BRIDGE;
    }
    if ( is_atm(porthw) ) {
	/*
	 * since subinterfaces can bridge make sure to set it here.
	 * for lane, it is already set...
	 */
	if (lec_enabled(port))
	    port->tbridge_media = TBR_LANE_ETHER;
	else
	    port->tbridge_media = TBR_ATM_TRANSIT;

	reg_invoke_media_tbridge_swidb_vc_queue_init(ET_ATM, port);
    }

}

/*
 * span_port_disable
 * Disable a spanning tree port (4.8.3)
 */

void span_port_disable (spantype *span, idbtype *port)
{
    hwidbtype *porthw = port->hwptr;
    boolean root;

    /*
     * Sanity check to ensure port has spanning tree running.
     */
    if (port->span_ptr == NULL)
	return;
    root = root_bridge(span);
    become_designated_port(span, port);
    set_port_state(port, PORT_DISABLED);
    reg_invoke_srt_cbus_bridge_port_disable(span, port);
    span_clearif(span, port);
    port->topology_change_acknowledge = FALSE;
    port->config_pending = FALSE;
    stop_message_age_timer(port);
    stop_forward_delay_timer(port);
    configuration_update(span);
    port_state_selection(span);
    if ((root_bridge(span)) && (!root)) {
	span->max_age = span->bridge_max_age;
	span->hello_time = span->bridge_hello_time;
	span->forward_delay = span->bridge_forward_delay;
	topology_change_detection(span);
	stop_tcn_timer(span);
	config_bpdu_generation(span);
	start_hello_timer(span);
    }
    if (is_x25(porthw)) {
	porthw->x25_bridge = FALSE;
	porthw->span_bridge_process_force &= ~SPAN_BR_PROC_X25_BRIDGE;
    }
    if (is_mlapb_only(porthw)) {
	porthw->mlapb_bridge = FALSE;
	porthw->span_bridge_process_force &= ~SPAN_BR_PROC_MLAPB_BRIDGE;
    }
    if (is_frame_relay(porthw) ) {
	porthw->frame_relay_stuff->fr_bridge = FALSE;
	porthw->span_bridge_process_force &= ~SPAN_BR_PROC_FR_BRIDGE;
    }
    if ( is_smds(porthw) ) {
        porthw->smds_bridge = FALSE;
        porthw->span_bridge_process_force &= ~SPAN_BR_PROC_SMDS_BRIDGE;
    }
    if (is_ppp(porthw)) {
	porthw->ppp_bridge = FALSE;
	porthw->span_bridge_process_force &= ~ SPAN_BR_PROC_PPP_BRIDGE;
    }
    if ( is_atm(porthw) ) {
	reg_invoke_media_tbridge_swidb_vc_queue_clear(ET_ATM, port);
    }
}

/*
 * span_bridge_priority
 * Set priority of bridge (4.8.4) after changing span->bridge_id.
 * This routine does not work correctly.
 */

void span_bridge_priority (spantype *span, int new_priority)
{
    boolean root;
    int i;
    idbtype *port;
    
    
    root = root_bridge(span);
    for (i = 0; i < span->port_count; i++) {
	port = span->port[i];
	if (designated_port(span, port)) {
	    port->designated_bridge[0] = (new_priority >> 8) & 0xFF;
	    port->designated_bridge[1] = new_priority & 0xFF;
	}
    }
    
    span->bridge_id[0] = (new_priority >> 8) & 0xFF;
    span->bridge_id[1] = new_priority & 0xFF;
    configuration_update(span);
    port_state_selection(span);
    if ((root_bridge(span)) && (!root)) {
	span->max_age = span->bridge_max_age;
	span->hello_time = span->bridge_hello_time;
	span->forward_delay = span->bridge_forward_delay;
	topology_change_detection(span);
	stop_tcn_timer(span);
	config_bpdu_generation(span);
	start_hello_timer(span);
    }
}

/*
 * span_port_priority
 * Adjust topology after changing priority of a port (4.8.5)
 */

void span_port_priority (spantype *span, idbtype *port, ushort new_port_id)
{
    if (designated_port(span, port))
	port->designated_port = new_port_id;
    port->port_id = new_port_id;
    if ((cmpid(span->bridge_id, port->designated_bridge, 
	       sizeof(span->bridge_id)) == 0) &&
	(port->port_id < port->designated_port)) {
	become_designated_port(span, port);
	port_state_selection(span);
    }
}

/*
 * ibm_span_port_priority
 * Adjust ibm spanning tree topology after changing priority of a port
 *
 * Note: This routine required as a result of differences in port_id usage
 *       between the IEEE/DEC and IBM Spanning Tree implementations.  Since
 *       the IBM Spanning Tree uses the entire port_id ushort for port
 *       identification, it is not possible to take advantage of existing
 *       IEEE/DEC port prioritization logic which maintains the current
 *       port priority within the unused portion of the port_id ushort.
 */

void ibm_span_port_priority (
    spantype *span, idbtype *port, ushort new_port_priority)
{
    if (designated_port(span, port)) {
	port->designated_port = port->port_id;
    }
    port->ibm_port_priority = new_port_priority;
    if ((cmpid(span->bridge_id, port->designated_bridge, 
	       sizeof(span->bridge_id)) == 0) &&
	       (ibm_port_priority_check(span, port->ibm_port_priority))) {
	  become_designated_port(span, port);
	  port_state_selection(span);
    }
}

/*
 * ibm_port_priority_check
 * Check to ensure that the IBM Spanning Tree port priority
 * has the highest priority (lowest value).
 */

boolean ibm_port_priority_check(spantype *span, ushort priority)
{
    int i;
    idbtype *ibm_port;
    ushort highest_priority = 0;

    for (i = 0; i < span->port_count; i++) {
	 ibm_port = span->port[i];
	 if (ibm_port->port_state != PORT_DISABLED) {
	     if (ibm_port->ibm_port_priority < priority)
		 highest_priority = ibm_port->ibm_port_priority;
	     else
	         highest_priority = priority;
	 }
    }
    if (priority !=  highest_priority)
	return(FALSE);
    return(TRUE);
}

/*
 * span_path_cost
 * Adjust topology after setting cost of a path (4.8.6).
 */

void span_path_cost (
    spantype *span)
{
    configuration_update(span);
    port_state_selection(span);
}

/*
 * span_enqueue
 * Input packets for spanning tree algorithm. Called from interrupt level.
 * If valid, queue datagram for process level.
 */

static sys_timestamp bridge_conflict_timer = timer_no_time;

void span_enqueue (paktype *pak)
{
    spantype *span;
    idbtype *port;

    port = pak->if_input;
    span = spanarray[port->span_index];
    if ((!span) || (!span->running) || !interface_up_simple(port)) {
	retbuffer(pak);
	return;
    }

    if ((port->hwptr->status & IDB_FDDI) &&
	!(port->hwptr->fddi_options & FDDI_ENCAPSULATE) &&
	(span->protocol != SPAN_PROTO_IBM)) {
	/*
	 * input port is a FDDI running transparently.  Check to see if we are
	 * hearing from another cisco running in encapsulation mode.  If so
	 * throw out an error message.
	 */
	hwaddrtype address;

	if ((*port->hwptr->extract_snpa)(pak, &address, DESTINATION)) {
	    if (ieee_equal(transit_bridge_multicast, address.addr)) {
		if (!SLEEPING(bridge_conflict_timer)) {
		    /*
		     * We've got a cuckoo in the nest. Squawk who it is.
		     */
		    (*port->hwptr->extract_snpa)(pak, &address, SOURCE);
		    errmsg(&msgsym(BRIDGECONFLICT, LINK),
			   port->namestring,
			   address.addr);
		    TIMER_START(bridge_conflict_timer, 15 * ONESEC);
		}
		retbuffer(pak);
		return;
	    }
	}
    }
    switch (span->protocol) {
    case SPAN_PROTO_IBM:		/* SRB AST support */
    case SPAN_PROTO_IEEE:
	p_enqueue(&span->spanQ, pak);
	break;
    case SPAN_PROTO_DEC:
	p_enqueue(&span->spanQ, pak);
	break;
    default:
	retbuffer(pak);
	break;
    }
}

/*
 * dec_out
 * Transmit a DEC format datagram on the specified port.
 * Since the IEEE multicast address has not yet been defined, we are
 * using this routine to send BPDU's as well as DEC packets.
 */

void dec_out (idbtype *port, paktype *pak)
{
    pak->if_output = port;
    pak->linktype = LINK_DEC_SPANNING;
    pak->acct_proto = ACCT_PROTO_SPAN;

    if (interface_up(port) == FALSE) {
	datagram_done(pak);
	return;
    }

    /*
     * Fix for CSCdi13811
     * Make sure that the spanning tree stays alive in the presence
     * of a saturating load of high priority traffic.
     */
    pak->flags |= PAK_PRIORITY;

    if ((!port->span_disabled) && pak_encap(port->hwptr, pak, DEC_BPDU_LENGTH,
					    0))
      datagram_out(pak);
    else
      retbuffer(pak);
}

/*
 * The next few pages of routines are DEC specific
 */

/*
 * dec_enqueue * Enqueue packet for DEC spanning tree
 */

void dec_enqueue (spantype *span, idbtype *port, paktype *pak)
{
    dechdrtype *dec;
    int j;
    uchar *cp;
    
    dec = (dechdrtype *)decheadstart(pak);

    if (span_debug) {
	cp = &dec->code;
	buginf("\nST: %s ", port->namestring);
	for (j = 0; j < DEC_BPDU_LENGTH; j++)
	    buginf("%-02x", *cp++);
    }
    
    /*
     * Check for looped back serial line by seeing if this is a HELLO BPDU
     * that we generated (has our bridge_id and our current port id).
     * For lack of anything better to do, we put the port in a listening
     * state and set the forward delay timer.
     */
    if ((port->hwptr->status & IDB_SERIAL) && (dec->type == DEC_TYPE_HELLO)) {
	if ((dec->port_id == port->port_id) &&
	    (cmpid(dec->bridge_id, span->bridge_id,
		   sizeof(span->bridge_id)) == 0))  {
	    if (spanevents_debug)
		buginf("\nST: %s is looped back", port->namestring);
	    span_clearif(span, port);
	    set_port_state(port, PORT_LISTENING);
	    start_forward_delay_timer(span, port);
	    retbuffer(pak);
	    return;
	}
    }
    if (dec->code == DEC_CODE) {
	switch (dec->type) {

	case DEC_TYPE_HELLO:
            /*
             * CSCdi18812: filter BPDU runts.
             */
            if (pak->datagramsize -
                (pak->network_start - pak->datagramstart) < DEC_BPDU_LENGTH) {
                if (spanevents_debug) {
                    buginf("\nST: DEC BPDU runt received on %s discarded:\n",
                           pak->if_input->namestring);
                    for (cp = pak->datagramstart;
                         cp < (pak->datagramstart +
                               pak->datagramsize); cp++)
                        buginf("%-02x", (*cp)++);
                }
            } else {
                dec_received_config_bpdu(span, port, pak);
            }
            break;

	case DEC_TYPE_TCN:
	    received_tcn_bpdu(span, port);
	    break;

        default:
            break;
	}
    }
    retbuffer(pak);
}

/*
 * dec_received_config_bpdu
 * Process a DEC HELLO message (4.7.1)
 */

void dec_received_config_bpdu (spantype *span, idbtype *port, paktype *pak)
{
    dechdrtype *dec;
    boolean root;
    
    dec = (dechdrtype *)decheadstart(pak);

    /* We should check if the "message age" has been already
     * larger then MAX AGE. To prevent wrong info to be
     * passed back and forth between routers in extensively
     * redundant topology.
     */
    if (dec->message_age >= dec->max_age)
      return;

    root = root_bridge(span);
    if (port->port_state != PORT_DISABLED) {
	if (dec_supersedes_port_info(span, port, pak)) {
	    
	    /*
	     * If new information received, we need to update data structures
	     */
	    dec_record_config_information(port, pak);
	    configuration_update(span);
	    port_state_selection(span);
	    if (!root_bridge(span) && root) {
		stop_hello_timer(span);
		if (span->topology_change_detected) {
		    stop_topology_change_timer(span);
		    transmit_tcn(span);
		    start_tcn_timer(span);
		}
	    }
	    
	    if (port == span->root_port) {
		dec_record_config_timeout_values(span, pak);
		config_bpdu_generation(span);
	
		if (dec->tcnack)
		    topology_change_acknowledged(span);
	    }
	} else if (designated_port(span, port)) {
	    transmit_config(span, port);
	}
    }
    
}

/*
 * ieee_out
 * Transmit a IEEE format datagram on the specified port.
 */

void ieee_out (idbtype *port, paktype *pak, int len)
{
    pak->if_output = port;
    pak->linktype = LINK_IEEE_SPANNING;
    pak->acct_proto = ACCT_PROTO_SPAN;

    if (interface_up(port) == FALSE) {
	datagram_done(pak);
	return;
    }

    /*
     * Fix for CSCdi13811
     * Make sure that the spanning tree stays alive in the presence
     * of a saturating load of high priority traffic.
     */
    pak->flags |= PAK_PRIORITY;

    if ((!port->span_disabled) && pak_encap(port->hwptr, pak, len, 0)) {
        /*
         * No need to insert SDE header here if the output
         * swidb is a SDE interface since pak_encap()
         * already takes cares of it. However, for now, serial
         * doesn't have the generic SDE support to lay down the
         * header yet.
         */
	if (port->tbridge_media == TBR_HDLC_TRANSIT_SDE) 
	    tbridge_construct_hdlc_sde_bpdu_header(pak);

	datagram_out(pak);
    } else 
	retbuffer(pak);
}

/*
 * bpdu_enqueue
 * Enqueue packet for IEEE spanning tree
 */

void bpdu_enqueue (spantype *span, idbtype *port, paktype *pak)
{
    int j, len;
    uchar *cp;
    bpduhdrtype *bpdu;

    bpdu = (bpduhdrtype *)bpduheadstart(pak);
    
    if (span_debug) {
	cp = (uchar *)&bpdu->protocol;
	if (bpdu->type == BPDU_TYPE_CONFIG)
	    len = BPDU_CONFIG_BYTES;
	else
	    len = BPDU_TCN_BYTES;
	buginf("\nST: %s ", IS_VLAN_CONFIGURED_SUBINTERFACE(port)?
	       port->short_namestring:port->namestring);

	for (j = 0; j < len; j++)
	    buginf("%-02x", *cp++); 
    }
    
    /*
     * Check for looped back serial line by seeing if this is a HELLO BPDU
     * that we generated (has our bridge_id and our current port id).
     * For lack of anything better to do, we put the port in a listening
     * state and set the forward delay timer.
     */
    if ((port->hwptr->status & IDB_SERIAL) && (bpdu->type == BPDU_TYPE_CONFIG)) {
	if ((GETSHORT(bpdu->port_id) == port->port_id) &&
	    (cmpid(bpdu->bridge_id, span->bridge_id,
		   sizeof(span->bridge_id)) == 0))  {
	    if (spanevents_debug && (port->port_state != PORT_BLOCKING))
		buginf("\nST: %s is looped back", port->namestring);	
	    span_clearif(span, port);
	    set_port_state(port, PORT_LISTENING);
	    start_forward_delay_timer (span, port);
	    retbuffer(pak);
	    return;
	}
    }

    /* SRB AST support */
    if ((span->protocol == SPAN_PROTO_IBM) &&
	(port->hwptr->status & IDB_VIRTUAL) && port->disjoint_peers){
	if(ast_received_disjoint_peers(span, port, pak)) {
            retbuffer(pak);
            return;
        }
    }
 
    if (bpdu->version == (BPDU_VERSION | span->domain)) {
	switch (bpdu->type) {

	case BPDU_TYPE_CONFIG:
            /*
             * CSCdi18812: filter BPDU runts.
             */
            if (pak->datagramsize -
                (pak->network_start - pak->datagramstart) <
                BPDU_CONFIG_BYTES) {
                if (spanevents_debug) {
                    buginf("\nST: IEEE BPDU runt received on %s discarded:\n",
                           pak->if_input->namestring);
                    for (cp = pak->datagramstart;
                         cp < (pak->datagramstart +
                               pak->datagramsize); cp++)
                        buginf("%-02x", (*cp)++);
                }
            } else {
                ieee_received_config_bpdu(span, port, pak);
            }
            break;


	case BPDU_TYPE_TCN:
	    received_tcn_bpdu(span, port);
	    break;

	default:
	    break;	
	}
    }
    retbuffer(pak);	
}

/*
 * ieee_transmit_config
 * Send a IEEE HELLO on the specified port (4.6.1)
 */

void ieee_transmit_config (spantype *span, idbtype *port, paktype *pak)
{
    ushort temp;
    bpduhdrtype *bpdu;

    bpdu = (bpduhdrtype *)bpduheadstart(pak);
    
    bpdu->protocol = BPDU_PROTOCOL;
    bpdu->version = (BPDU_VERSION | span->domain);
    bpdu->type = BPDU_TYPE_CONFIG;
    bpdu->notusedflags = 0;
    bcopy(span->designated_root, bpdu->root_id, sizeof(bpdu->root_id));
    bcopy(&span->root_path_cost, bpdu->root_path_cost, 
	  sizeof(bpdu->root_path_cost));
    bcopy(span->bridge_id, bpdu->bridge_id, sizeof(bpdu->bridge_id));
    bcopy(&port->port_id, bpdu->port_id, sizeof(bpdu->port_id));
    if (root_bridge(span)) {
	bpdu->message_age[0] = 0;
	bpdu->message_age[1] = 0;
    } else {
        /* New check added to support hidden message-age-increment
         * option in "source-bridge spanning X" interface command and
         * permit message-age count manipulation in mixed AST environments
         * where bridge vendors do not support MAX_AGE configuration.
         * In the AST case if we pass the ibm_message_age_increment
	 * check, then we forward the bpdu without + MESSAGE_AGE_INCREMENT.
         * Otherwise, in all IEEE/DEC cases or AST cases which fail this
	 * check we will forward the BPDUs with the + MESSAGE_AGE_INCREMENT.
         */
	if ((port->span_ptr->protocol == SPAN_PROTO_IBM) &&
          (port->ibm_message_age_increment == SPAN_IBM_MESSAGE_AGE_PASSTHRU)) {
	    temp= span->root_port->message_age_timer;
	    PUTSHORT(bpdu->message_age, (temp * 256));
	}   
        else { 
	    temp = span->root_port->message_age_timer + MESSAGE_AGE_INCREMENT;
	    PUTSHORT(bpdu->message_age, (temp * 256));
	}
    }
    temp = span->max_age;
    PUTSHORT(bpdu->max_age, (temp * 256)); 
    temp = span->hello_time;
    PUTSHORT(bpdu->hello_time, (temp * 256)); 
    temp = span->forward_delay;
    PUTSHORT(bpdu->forward_delay, (temp * 256)); 
    bpdu->tc_acknowledgement = port->topology_change_acknowledge;
    port->topology_change_acknowledge = FALSE;
    bpdu->tc = span->topology_change;
    ieee_out(port, pak, BPDU_CONFIG_BYTES);
    port->config_pending = FALSE;
    start_hold_timer(span, port);
    
}

/*
 * dec_transmit_config
 * Send a DEC HELLO on the specified port (4.6.1)
 */
void dec_transmit_config (spantype *span, idbtype *port, paktype *pak)
{
    dechdrtype *dec;

    dec = (dechdrtype *)decheadstart(pak);

    dec->code = DEC_CODE;
    dec->type = DEC_TYPE_HELLO;
    dec->version = DEC_VERSION;
    dec->tc = FALSE;
    dec->tcnack = port->topology_change_acknowledge;
    port->topology_change_acknowledge = FALSE;
    dec->unused = 0;
    if (root_bridge(span)) {
	dec->shorttime = span->shorttimer ? TRUE : FALSE;
	dec->message_age = 0;
    } else {
	dec->shorttime = span->shortflag;
	dec->message_age = span->root_port->message_age_timer + 
	    MESSAGE_AGE_INCREMENT;
    }
    bcopy(span->designated_root, dec->root_id, sizeof(dec->root_id));
    dec->root_path_cost = span->root_path_cost;
    bcopy(span->bridge_id, dec->bridge_id, sizeof(dec->bridge_id));
    dec->port_id = port->port_id;
    dec->hello_time = span->hello_time;
    dec->forward_delay = span->forward_delay;
    dec->max_age = span->max_age;
    dec_out(port, pak);
}

/*
 * ieee_transmit_tcn
 * Send an IEEE Topology Change Notification BPDU
 */

void ieee_transmit_tcn (idbtype *port, paktype *pak, uchar domain)
{
    spantype *span;
    bpduhdrtype *bpdu;

    bpdu = (bpduhdrtype *)bpduheadstart(pak);

    span = spanarray[port->span_index];
    gc_stations(span, TRUE);
    bpdu->protocol = BPDU_PROTOCOL;
    bpdu->version = (BPDU_VERSION | domain);
    bpdu->type = BPDU_TYPE_TCN;
    ieee_out(port, pak, BPDU_TCN_BYTES);
}

/*
 * dec_send_tcn
 * Send a DEC Topology Change Notification datagram
 */

void dec_transmit_tcn (idbtype *port, paktype *pak)
{
    dechdrtype *dec;
    spantype *span;

    dec = (dechdrtype *)decheadstart(pak);

    span = spanarray[port->span_index];
    gc_stations(span, TRUE);
    dec->code = DEC_CODE;			/* E1 */
    dec->type = DEC_TYPE_TCN;			/* 02 */
    dec->version = DEC_VERSION;			/* 01 */
    dec->tc = TRUE;				/* 80 */
    dec->tcnack = FALSE;
    dec->unused = 0;
    dec->shorttime = FALSE;
    dec_out(port, pak);
}

/* SRB AST support */
/*
 * ast_peer_transmit_config()
 * Relay the information that I've heard from my designated bridge.
 * This is necessary if there are peers that are not talking to each other.
 */

void ast_peer_transmit_config (spantype *span, idbtype *port, void *peer)
{
    ushort temp;
    bpduhdrtype *bpdu;
    paktype *pak;

    pak = getbuffer(MINETHERSIZE);
    if (!pak) {
	return;
    }

    pak->peer_ptr = peer;
    bpdu = (bpduhdrtype *) bpduheadstart(pak);
    bpdu->protocol = BPDU_PROTOCOL;
    bpdu->version = BPDU_VERSION;
    bpdu->type = BPDU_TYPE_CONFIG;
    bpdu->notusedflags = 0;
    bcopy(span->designated_root, bpdu->root_id, sizeof(bpdu->root_id));
    bcopy(&span->root_path_cost, bpdu->root_path_cost,
          sizeof(bpdu->root_path_cost));
    bcopy(span->bridge_id, bpdu->bridge_id, sizeof(bpdu->bridge_id));
    bcopy(&port->port_id, bpdu->port_id, sizeof(bpdu->port_id));
    if (root_bridge(span)) {
        bpdu->message_age[0] = 0;
        bpdu->message_age[1] = 0;
    } else {
        temp = span->root_port->message_age_timer + MESSAGE_AGE_INCREMENT;
        PUTSHORT(bpdu->message_age, (temp * 256));
    }
    temp = span->max_age;
    PUTSHORT(bpdu->max_age, (temp * 256));
    temp = span->hello_time;
    PUTSHORT(bpdu->hello_time, (temp * 256));
    temp = span->forward_delay;
    PUTSHORT(bpdu->forward_delay, (temp * 256));

    /* Indicate the destination Peer */
    ieee_out(port, pak, BPDU_CONFIG_BYTES);
}

/*
 * ast_received_disjoint_peers()
 * Remedy for RSRB non fully meshed configuration defect.
 * There are two case where port->disjoint_peers are set.
 * IF (I am not the root) and (new root is not same as the old one)
 *	then 
 *	    /I heard a new information/ 
 *	    (need to restart)
 * ELSE
 *	(take normal route)	
 */

boolean ast_received_disjoint_peers(spantype *span, idbtype *port, 
				    paktype *pak)
{
    bpduhdrtype *bpdu;

    bpdu = (bpduhdrtype *)bpduheadstart(pak);
    if (span_debug)
        buginf("\nST: Received New Info. SDR %e, BPR %e, PDR %e",
	    &span->designated_root[2], &bpdu->root_id[2],
	    &port->designated_root[2]); 
    /* Remember cmpid() returns non zero when two values are different */
    if (cmpid(span->designated_root, span->bridge_id,
	sizeof(span->designated_root)) &&      /* I am not the root */
	cmpid(bpdu->root_id, port->designated_root,
	sizeof(port->designated_root))) {	/* new root != old root */
	port->disjoint_peers = FALSE;
	port->disjoint_bpdu_generation = FALSE;
 	span_initialize(span);
	return(TRUE);
    }
    return(FALSE);
}
