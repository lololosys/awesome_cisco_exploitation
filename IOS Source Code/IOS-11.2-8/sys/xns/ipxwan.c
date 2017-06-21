/* $Id: ipxwan.c,v 3.7.4.7 1996/08/14 19:59:55 john Exp $
 * $Source: /release/112/cvs/Xsys/xns/ipxwan.c,v $
 *------------------------------------------------------------------
 * novipxwan.c - IPXWAN protocol for Novell/IPX
 *
 * October 1992, Joel P. Bion
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module implements IPXWAN for Novell/IPX as defined in RFC1362,
 * September, 1992 version. Currently, this support is written generically,
 * and will work for any serial link 'mechanism' where each remote device
 * exists on its own IDB.
 *
 * A BIG ASSUMPTION OF THIS MODULE: 'interface_up' will return true for an
 * IDB only when a remote connection is possible, and IPXWAN packets can be
 * sent across the link.
 *
 * !!!WARNING!!!WARNING!!!WARNING!!!WARNING!!!WARNING!!!WARNING!!!WARNING!!!
 * IPXWAN's headers are CHOCK-FULL of odd alignment! Don't expect anything
 * but 'uchar' arrays to work in defining them! {PUT,GET}{SHORT,LONG} are
 * needed throughout.
 * !!!WARNING!!!WARNING!!!WARNING!!!WARNING!!!WARNING!!!WARNING!!!WARNING!!!
 *------------------------------------------------------------------
 * $Log: ipxwan.c,v $
 * Revision 3.7.4.7  1996/08/14  19:59:55  john
 * CSCdi42325:  IPXWAN Client Mode negotiation failure with NetWare
 *              Connect Client. Also update inline Documentation.
 * Branch: California_branch
 *
 * Revision 3.7.4.6  1996/07/26  00:08:33  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.7.4.5  1996/06/05  00:45:35  kleung
 * CSCdi57957:  Unsolicited message should use errmsg facility
 * Branch: California_branch
 *
 * Revision 3.7.4.4  1996/06/03  20:23:46  john
 * CSCdi58748:  ipxwan and nlsp command nvgen order wrong and subsys issues
 * Branch: California_branch
 *
 * Revision 3.7.4.3  1996/05/03  02:16:56  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.7.4.2  1996/04/02  05:38:01  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.7.4.1  1996/03/18  22:52:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  07:59:16  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/07  11:21:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  21:59:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/03/08  20:35:34  sluong
 * CSCdi48717:  nlsp didnt update route when there is a better route
 *
 * Revision 3.6  1996/02/05  17:27:16  john
 * CSCdi47276:  Parallel equal bandwidth links get different ipxwan nlsp
 *              metrics
 *
 * Revision 3.5  1996/01/22  07:52:43  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/04  11:15:52  john
 * CSCdi44920:  IPXWAN not doing IPX Header compression negotiations
 *
 * Revision 3.3  1995/11/17  19:22:10  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:53:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:58:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:38:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/24  23:32:18  hampton
 * Remove the old inline s_kill_self_with_no_analysis by pushing it into
 * the routines that call it.  [CSCdi42780]
 *
 * Revision 2.6  1995/08/25  14:11:23  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi39328]
 *
 * Revision 2.5  1995/06/30 05:21:53  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.4  1995/06/20 21:53:18  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/18  06:19:51  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.2  1995/06/09 13:25:45  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:27:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Theory of operation:
 *
 *   It is assumed you have read and understood RFC1634 before
 *    reading this comment.
 *
 *   The way our implementation of IPXWAN fits into the existing Novell
 *   code is in the following ways:
 *
 *      1) Permanent Novell network numbers are not assigned to IPXWAN links.
 *         That is, 'novell network' commands are not issued on these links.
 *      2) When a user enables IPXWAN on a link, the user can specify three
 *         optional values:
 *              a) The 'Node ID' to assign to the router for this link. This
 *                 'Node ID' may be re-used on multiple IPXWAN links within
 *                 the same router, but may not be assigned as the 'real'
 *                 network number on any Novell-enabled interface, or be the
 *                 'extern network number' assigned to any IPXWAN link (see
 *                 below for the meaning of 'external network number'.)
 *                 This is stored in the NIDB as the 'internal network number.'
 *                 If no NodeID is given the system will use the IPX 
 *                 INTERNAL-NETWORK configured. If an ipx internal-net is
 *                 not configured then the user must configure a Node ID,
 *                 or must configure an ipx internal-net (preferred).
 *              b) The 'network number' to assign the link if this router wins
 *                 the Timer Request 'battle' (I.e., becomes 'Master' of the
 *                 link.) We call this the 'external network number' in the
 *                 idb. This number must not be assigned as the 'real' or
 *                 'external network number' for any other link, nor may it
 *                 be the 'internal network number' for any link.
 *                 This may be 0 or 'unnumbered' in which case the 
 *                 link or system's ipx internal-network is used to determine
 *                 'master' vs. 'slave' relationship at startup.
 * 		c) The 'node name' to assign to the link. Like the 'internal
 *                 network number' this may be re-used on multiple IPXWAN links
 *                 on the same router.
 *                 This now defaults to the configured hostname and no longer
 *                 must be uppercase.
 *
 * The state machine:
 *         The state machine implements RFC 1634/NLSP 1.0 chapter 3
 *         The states are as follows:
 *
 *         State		       Description
 *         -----------------------------------------------------------------
 *         DISCONNECT                  IPXWAN is not enabled on this link, or
 *				       the link is 'dead' for any other reason.
 *				       IPXWAN packets are NOT accepted on
 *				       this interface during this time.
 *
 *         TIMER_REQ_SENT              This link is currently sending 'timer
 *				       request' packets, but has not received
 *				       a response from the other end of the
 *				       link. Every so often in this state, an
 *				       alarm will wake up and cause another
 *				       'timer request' packet to be sent. In
 *				       fact, the method used to send the first
 *				       one of these is to enter the state, and
 *				       cause the alarm clock to wake up NOW.
 *
 *         NLSP_THROUGHPUT_SENT        We are master and slave has accepted
 *                                     NLSP routing. Master now send throughput
 *                                     requests and waits for reply from slave.
 *
 *         SLAVE_WAIT_THROUGHPUT       We are slave and have answer the timer
 *                                     request for NLSP routing. We now wait
 *                                     for the master to send throughput
 *                                     requests.
 *
 *         NLSP_DELAY_SENT             We are master and slave has sent
 *                                     throughput responses. We now send delay
 *                                     requests and wait for response from
 *                                     slave.
 *
 *         SLAVE_WAIT_DELAY            We are slave we have responded to
 *                                     thoughput requests and now await
 *                                     delay requests from the master.
 *
 *	   SLAVE_WAIT_ROUTER_INFO_REQ  We have determed we are the slave, and
 *				       have sent a 'timer response' packet.
 *                                     If NLSP was negotiated we have sent
 *                                     throughput and delay responses as well.
 *				       Now we are waiting for a 'ripsap info'
 *				       packet. The alarm clock is still
 *				       running. If it goes off, then it means
 *				       we assume the Master will NEVER send the
 *				       packet, and something bad has happened.
 *				       Therefore, we restart sending 'timer
 *				       request' packets by reentering the
 *				       'TIMER_REQ_SENT' state.
 *
 *	   MASTER_WAIT_ROUTER_INFO_RSP We have determed we are the master, and
 *				       have sent a 'ripsap info' packet.
 *				       Now we are waiting for a 'ripsap info'
 *				       response. The alarm clock is still
 *				       running. If it goes off, then it means
 *				       we assume the Slave will NEVER send the
 *				       response, and something bad has
 *				       happened. Therefore, we restart sending
 *				       'timer request' packets by reentering
 *				       the 'TIMER_REQ_SENT' state.
 *
 *	   MASTER_CONNECT and          We have received a 'ripsap info' 
 *          SLAVE_CONNECT              response (Master side) or have sent
 *                                     this packet (Slave side), and are now
 *                                     fully 'up' as far as IPXWAN is
 *                                     concerned. A 'real network number' (and
 *				       delay) has been assigned to the link,
 *                                     and the rest of the IPX code will now
 *                                     treat this link as fully operational.
 *                                     There is no functional difference, as
 *                                     far as either the core IPX or IPXWAN
 *                                     code is concerned, between these two
 *                                     states. The two states are kept
 *                                     discrete only for user-information
 *                                     purposes. Whenever entering either of
 *                                     these states, 'novell_add_interface' is
 *                                     called to place the new interface in
 *                                     the Novell routing tables, and whenever
 *                                     leaving either of these states, the
 *                                     'novell_delete_interface' routine is
 *                                     called to remove the interface from the
 *                                     Novell routing tables. This is exactly
 *                                     the same as what is done when a user
 *                                     manually configures or removes a Novell
 *                                     network number from non-IPXWAN links.
 *
 *   The normal flow of moving between the states has been described above.
 *   Some extra events that can change the state 'out from under the rug' are
 *   described here:
 *
 *           1) The IPXWAN state goes from 'anything' to DISCONNECT if the
 *              interface is de-configured for IPXWAN or misconfigured for
 *              it in any way -- even if the interface is fully up at the time.
 *
 *           2) The IPXWAN state goes from 'anything' to TIMER_REQ_SENT if
 *              any of the following happen:
 *                   a) The interface goes from up to down.
 *                   b) The interface is reconfigured to new IPXWAN values,
 *                      and the new values are 'acceptable.'
 *		     c) A 'timer request' packet is received on the interface.
 *                      This usually indicates the remote router has resetted.
 *		     d) IPXWAN has timed out waiting for a 'ripsap info'
 *                      request or response.
 *
 *   Trivia:
 *          1) As the RFC suggests, we set the Novell 'MAC address' for the
 *             link to the four bytes of the node (internal network) number,
 *             followed by two bytes of zero.
 *          2) We fill the pad area with a random sequence of 00 thru FF
 *          3) Connected networks on IPXWAN links are shown as type 'W' routes
 *             in the 'show novell route' display. (Normal links are shown as
 *             type 'C'.)
 *
 *   Where we deviate from RFC 1362/RFC 1634. Note that all have 'work 
 *   arounds' or do not really matter...
 *
 *	    1) We calculate 'delay' for the link by looking at idb->nov_delay.
 *             If this is set, we WILL use it. If it is NOT set, we will use
 *             Novell's suggested algorithm. This allows users to have some
 *	       choice in how the 'ticks' count is set.
 *          2) We allow a separate Node ID 'number' ('internal network
 *             number') and Node Name to be assigned to each link. They may
 *             be shared between the links, of course.
 *             However, We expect people to use the IPX INTERNAL-NETWORK
 *             number.
 *          3) We do not advertise our Node ID 'numbers' as reachable networks
 *             in Novell routing updates. We do not recognize packets sent to
 *             our NodeID network number as being 'for us.' We should NEVER
 *             receive packets to this network number ANYWAY. If a user wants
 *             this to happen, assign the Node ID numbers to loopback
 *             interfaces.
 *             If they are using the IPX INETERNAL-NETWORK we do advertise
 *             this network number and you can ping the internal address.
 *	    4) The time between successive 'timer request' packets being sent
 *             defaults to 20 seconds, as the RFC wants, but the user is
 *             allowed to reconfigure this value.
 *          5) There is no concept of a 'pool' of WAN network numbers. Each
 *             IPXWAN link is assigned 'one' (the 'external network number'),
 *             which is fully sufficient for IPXWAN's purposes.
 *          6) We do not implement Client mode completely at this time
 *             Client mode is a hidden command.
 *
 */
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "xns.h"
#include "novell.h"
#include "../iprouting/route.h"
#include "novroute.h"
#include "novnlsp.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/isis_ipx.h"
#include "ipx_registry.h"
#include "../util/random.h"
#include "config.h"
#include "parser.h"
#include "subsys.h"


#define IPXWAN_TIME_DELTA(end, start) \
	  ((CLOCK_DIFF_UNSIGNED(end, start) < 1) ? 1 : CLOCK_DIFF_UNSIGNED(end, start))

sys_timestamp nov_ipxwan_alarm;		/* Next time we need to do something */

/*
 * nov_ipxwan_state_to_string
 *
 * Return a string of the Novell IPXWAN state
 */
static char *nov_ipxwan_state_to_string (ulong state)
{
    switch (state) {
      case NOV_IPXWAN_STATE_DISCONNECT:
	return("Disconnect");
      case NOV_IPXWAN_STATE_TIMER_REQ_SENT:
	return("Sending Timer Requests");
      case NOV_IPXWAN_STATE_NLSP_THROUGHPUT_SENT:
	return("Master: Sent Throughput");
      case NOV_IPXWAN_STATE_SLAVE_WAIT_THROUGHPUT:
	return("Slave: Wait Throughput");
      case NOV_IPXWAN_STATE_NLSP_DELAY_SENT:
	return("Master: Sent Delay");
      case NOV_IPXWAN_STATE_SLAVE_WAIT_DELAY:
	return("Slave: Wait Delay");
      case NOV_IPXWAN_STATE_SLAVE_WAIT_ROUTER_INFO_REQ:
	return("Slave: Wait Router Info");
      case NOV_IPXWAN_STATE_MASTER_WAIT_ROUTER_INFO_RSP:
	return("Master: Sent Router Info");
      case NOV_IPXWAN_STATE_MASTER_CONNECT:
	return("Master: Connect");
      case NOV_IPXWAN_STATE_SLAVE_CONNECT:
	return("Slave: Connect");
      default:
	return("UNKNOWN");
    }
}
				 
/*
 * nov_ipxwan_router_type_to_string
 *
 * Return a string of the Novell IPXWAN router type
 */
static char *nov_ipxwan_router_type_to_string (ulong rtr_type)
{
    switch (rtr_type) {
      case NOV_IPXWAN_OPTION_ROUTING_TYPE_RIPSAP:
	return("RIP numbered");
      case NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP:
	return("NLSP");
      case NOV_IPXWAN_OPTION_ROUTING_TYPE_UNN_RIP:
	return("RIP Unnumbered");
      case NOV_IPXWAN_OPTION_ROUTING_TYPE_STATIC:
	return("Static");
      case NOV_IPXWAN_OPTION_ROUTING_TYPE_CLIENT:
	return("Remote Client");
      default:
	return("UNKNOWN");
    }
}
/*
 * nov_ipxwan_error_mode_to_string
 *
 * Return a string of the Novell IPXWAN error mode type
 */
static char *nov_ipxwan_error_mode_to_string (enum IPX_IPXWAN_ERROR_CFG err_type)
{
    switch (err_type) {
      case IPX_IPXWAN_ERROR_CONFIG_RESET:
        return("reset");
      case IPX_IPXWAN_ERROR_CONFIG_DOWN:
	return("down");
      case IPX_IPXWAN_ERROR_CONFIG_NOOP:
	return("resume");
      default:
	return("unknown");
      }
}

				 
/*
 * nov_ipxwan_display
 *
 * Display the current IPXWAN state for the given IDB.
 */
void nov_ipxwan_display (nidbtype * nidb)
{
    if (!nidb->nov_ipxwan_enabled) {
	printf("\n  IPXWAN processing not enabled on this interface.");
    } else {
	printf("\n   Local IPXWAN Node ID:        %x/%s",
	       NOVELL_INTERNAL_ID(nidb), nidb->nov_ipxwan_routername);
        printf("\n   Network when IPXWAN master: %-8x IPXWAN delay (master owns): %d",
	       nidb->nov_ipxwan_external_net, nidb->nov_ipxwan_delay);
	printf("\n   IPXWAN Retry Interval:      %-8d IPXWAN Retry limit:         %d",
	       nidb->nov_ipxwan_treq_int, nidb->nov_ipxwan_treq_max);
	printf("\n   IPXWAN Routing negotiated: %s",
	       nov_ipxwan_router_type_to_string(nidb->nov_ipxwan_routing_type));
	printf("\n   IPXWAN State:  %-26s",
	       nov_ipxwan_state_to_string(nidb->nov_ipxwan_state));
	if (NOV_IPXWAN_IS_RETRY_STATE(nidb->nov_ipxwan_state)) {
	    printf("  Retry Count:  %d", nidb->nov_ipxwan_treq_count);
	}
	printf("\n   State change reason: %s", nidb->nov_ipxwan_state_reason);
	printf("\n   Last received remote node info: %x/%s",
	       nidb->nov_ipxwan_remote_internal_net,
	       (char *) &(nidb->nov_ipxwan_remote_routername[0]) ?
	       (char *) nidb->nov_ipxwan_remote_routername : "[none]");
	printf("\n   Client mode %s, Static mode %s, Error mode is %s",
	       nidb->nov_ipxwan_client ? "enabled" : "disabled",
	       nidb->nov_ipxwan_static ? "enabled" : "disabled",
	       nov_ipxwan_error_mode_to_string(nidb->nov_ipxwan_error));
    }
}

/*
 * nov_ipxwan_set_alarm
 *
 * Sets the alarm to the next expiration time found on any NIDB
 */
static void nov_ipxwan_set_alarm (void)
{
    nidbtype *nidb;

    TIMER_STOP(nov_ipxwan_alarm);
    FOR_ALL_NIDBS(nidb) {
	if (TIMER_RUNNING(nidb->nov_ipxwan_treq_next)) {
	    if (!TIMER_RUNNING(nov_ipxwan_alarm) ||
		TIMER_LATER(nov_ipxwan_alarm, nidb->nov_ipxwan_treq_next)) {
		nov_ipxwan_alarm = nidb->nov_ipxwan_treq_next;
	    }
	}
    }
}

/*
 * nov_ipxwan_update_timer
 *
 * Change the timer value for the given IDB, and also set the next timeoff
 * time for the ipxwan background process. (When it has to wake up again).
 */
static void nov_ipxwan_update_timer (nidbtype *innidb, ulong delay)
{
    TIMER_START(innidb->nov_ipxwan_treq_next, delay);
    nov_ipxwan_set_alarm();
}

/*
 * nov_ipxwan_stop_timer
 *
 * Stop the timer for the given IDB, and also set the next alarm time.
 */
static void nov_ipxwan_stop_timer (nidbtype *innidb)
{
    TIMER_STOP(innidb->nov_ipxwan_treq_next);
    nov_ipxwan_set_alarm();
}

/*
 * nov_ipxwan_new_state
 *
 * Change the IPXWAN state of the IDB given to the value given.
 */
static void nov_ipxwan_new_state (nidbtype *nidb, ulong newstate, char *reason)
{
    /* don't do a no-op */
    if (nidb->nov_ipxwan_state == newstate)
        return;
    if (novellipxwan_debug) {
	buginf("\nIPXWAN: state (%s -> %s) [%s/%x:%x (%s)]",
	       nov_ipxwan_state_to_string(nidb->nov_ipxwan_state),
	       nov_ipxwan_state_to_string(newstate),
	       nidb->idb->namestring,
	       NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net,
	       reason);
    }
    /* Set the new state, and the reason for it */
    nidb->nov_ipxwan_state = newstate;
    bcopy(reason, nidb->nov_ipxwan_state_reason,
	  NOV_IPXWAN_STATE_REASON_MAX_LEN);
    nidb->nov_ipxwan_state_reason[NOV_IPXWAN_STATE_REASON_MAX_LEN - 1] = '\0';

    /*
     * Reset sequence number
     */
    nidb->nov_ipxwan_treq_count = 0;
    nidb->nov_ipxwan_treq_expect = 0;

    /*
     * Tell IPX/NCP compression that the world has changed...
     */
    if (NOV_IPXWAN_IS_REASON_TO_RESET_COMPRESSION(nidb->nov_ipxwan_state)) {
	reg_invoke_ipx_compression_reset(nidb);
    }

    /*
     * If the new state is the connect state, bring the network fully
     * UP for IPX's purposes. If going down, remove it from that awareness.
     */
    if (NOV_IPXWAN_IS_CONNECT_STATE(newstate)) {
	novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
	nov_ipxwan_stop_timer(nidb);
    }  else {
	nov_network_finish(&novrip_pdb, nidb->novellnet, FALSE);
	novell_nidb_set_network_number(nidb, 0);
	if (NOVELL_NIDB_IPXWAN_START_ALLOWED(nidb)) {
	    novell_nidb_newstate(nidb, NOVELL_NIDB_IPXWAN_STARTING);
	}
	if (newstate != NOV_IPXWAN_STATE_DISCONNECT) {
	    nov_ipxwan_update_timer(nidb, 0); /* Run right away */
	} else {
	    nov_ipxwan_stop_timer(nidb);
	}
    }
}

static forktype nov_ipxwan_idb_link_reset (nidbtype *nidb)
{
    if (nidb != NULL) {
	shutdown_interface(nidb->idb, TRUE);

	/* if error mode is reset bring the link up after waiting for
         * it to get fully set to down.
         * if error more is shutdown, just leave the interface down
         */

	if (nidb->nov_ipxwan_error == IPX_IPXWAN_ERROR_CONFIG_RESET) {
	  process_sleep_for(2 * ONESEC);
	  shutdown_interface(nidb->idb, FALSE);
	}
    }
    process_set_analyze(FALSE);
    process_kill(THIS_PROCESS);
}

/*
 * nov_ipxwan_reset_nidb
 *
 * This is a kludge to force a hard down/up on an interface
 * necessary for IPXWAN certification tests.
 *
 * Based upon nidb->nov_ipxwan_error we might return without
 * doing anything.
 */
static void nov_ipxwan_reset_nidb (nidbtype *nidb)
{
  if ((nidb != NULL) &&
      (nidb->nov_ipxwan_error != IPX_IPXWAN_ERROR_CONFIG_NOOP))
    cfork((forkproc *) nov_ipxwan_idb_link_reset, (long) nidb, 0,
	  "IPXWAN LReset", 0);
}

/*
 * nov_ipxwan_set_routing_type
 * Set the routing type for the specified interface; handle the RIPSAP
 * flags...
 */
static void nov_ipxwan_set_routing_type (nidbtype *nidb, int new_type)
{
    nidb->nov_ipxwan_routing_type = new_type;

    switch (nidb->nov_ipxwan_routing_type) {
      case NOV_IPXWAN_OPTION_ROUTING_TYPE_RIPSAP:
      case NOV_IPXWAN_OPTION_ROUTING_TYPE_UNN_RIP:
	nidb->nov_ipxwan_need_ripsap = TRUE;
	break;
      case NOV_IPXWAN_OPTION_ROUTING_TYPE_STATIC:
      case NOV_IPXWAN_OPTION_ROUTING_TYPE_CLIENT:
      case NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP:
	nidb->nov_ipxwan_need_ripsap = FALSE;
	break;
    }
}

/*
 * nov_ipxwan_process_timer_req
 *
 * Process NOV_IPXWAN_TIMER_REQ_PKT_TYPE IPXWAN packets.
 */
static void nov_ipxwan_process_timer_req (paktype *pak)
{
    ipxwanhdrtype *ipxwan;
    idbtype *idb;
    nidbtype *nidb;
    nov_ipxwan_option *optptr;
    nov_ipxwan_option_routing_type *routeoptptr;
    nov_ipxwan_option_ext_node_info *extnodeoptptr;
    nov_ipxwan_option_compression_info *compressionoptptr;
    boolean accepted_routing_type, wants_client_mode, wants_routing_mode;
    ulong node_id;
    boolean compression_enabled;
    uchar compression_type, compression_options, compression_slots;
    int curopt, tot_options_length;
    
    ipxwan = (ipxwanhdrtype *)ipxheadstart(pak);
    nidb = pak->nif_input;
    idb = nidb->idb;
    node_id = 0L;
    accepted_routing_type = FALSE;
    /* the following can not both be true */
    wants_client_mode = FALSE;
    wants_routing_mode = FALSE;

    /* We will subtract option length from packet len if we go negative */
    /*    then we will abort the Timer Request handling, we abort even if */
    /*    the option is unkown or after the routing options */

    tot_options_length = ipxwan->len - NOV_IPXWAN_HDRLEN;

    if (novellipxwan_debug) {
	buginf("\nIPXWAN: Rcv TIMER_REQ on %s/%x:%x, NodeID %x, Seq %x",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net, GETLONG(ipxwan->node_id),
	       ipxwan->sequence_number);
    }
    /*
     * Okay. We got a request. Are we the slave, to send a response? If not,
     * chuck the thing. But put us back TIMER_REQ state all the same, 'cause
     * this might be a new device coming up...
     */
    nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_TIMER_REQ_SENT,
			 "Timer Req. Received");
    if (nidb->nov_ipxwan_external_net == 0) {
	/* We asked for unnumbered we are Slave unless his node-Id == 0 */
	if (GETLONG(ipxwan->node_id) == 0) {
	    /* Ok, Here we both want unnumbered */
	    for (curopt = 0, optptr = (nov_ipxwan_option *)
		     ((uchar *) (&(ipxwan->num_options)) + 1);
		 curopt < ipxwan->num_options;
		 curopt++) {
		switch (optptr->option_number) {
		case NOV_IPXWAN_OPTION_EXT_NODE_INFO:
		    extnodeoptptr = (nov_ipxwan_option_ext_node_info *) optptr;
		    node_id = GETLONG(extnodeoptptr->node_id);
		    break;
		default:
		    break;
		}
		optptr = (nov_ipxwan_option *)
		    (((uchar *) optptr) + NOV_IPXWAN_OPTION_HEADER_SIZE +
		     GETSHORT(optptr->option_length));
	    }
	    if (novellipxwan_debug)
		buginf("\nIPXWAN: Rcv TIMER_REQ NodeID %x as %s asking for unnumbered on %s",
		       node_id, ((node_id < NOVELL_INTERNAL_ID(nidb)) ?
				 "SLAVE" : "MASTER"), idb->namestring);
	    /* We are MASTER ignore Timer Req, Slave will send Timer response */
	    if (node_id < NOVELL_INTERNAL_ID(nidb)) {
		retbuffer(pak);
		return;
	    } else if (node_id == NOVELL_INTERNAL_ID(nidb)) {
		buginf("\nIPXWAN: Hearing our NodeID on %s/%x:%x. Looped interface?",
		       idb->namestring, NOVELL_INTERNAL_ID(nidb),
		       nidb->nov_ipxwan_external_net);
		retbuffer(pak);
		/* Spec says we should "disconnect on the underlying protocol" */
		nov_ipxwan_reset_nidb(nidb);
		return;
	    }
	}
    } else {
	if (GETLONG(ipxwan->node_id) < NOVELL_INTERNAL_ID(nidb)) {
        /* We are MASTER ignore Timer Req, Slave will send Timer response */
	    retbuffer(pak);
	    return;
	} else if (GETLONG(ipxwan->node_id) == NOVELL_INTERNAL_ID(nidb)) {
	    buginf("\nIPXWAN: Hearing our NodeID on %s/%x:%x. Looped interface?",
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	    retbuffer(pak);
	    /* Spec says we should "disconnect on the underlying protocol" */
	    nov_ipxwan_reset_nidb(nidb);
	    return;
	}
    }
 
    /*
     * Well, we appear to be a SLAVE, today... Process the packet, and
     * send a reply, and do some work.
     */
    
    /*
     * Modify the packet. Handle any options
     */
    for (curopt = 0,
	     optptr = (nov_ipxwan_option *) (&(ipxwan->num_options) + 1);
	 curopt < ipxwan->num_options;
	 curopt++) {
	switch (optptr->option_number) {
	case NOV_IPXWAN_OPTION_ROUTING_TYPE:
	    routeoptptr = (nov_ipxwan_option_routing_type *) optptr;
	    tot_options_length = tot_options_length -
		NOV_IPXWAN_OPTION_ROUTING_SIZE;
	    if (accepted_routing_type == FALSE) {
		switch (routeoptptr->routing_type) {
		case NOV_IPXWAN_OPTION_ROUTING_TYPE_RIPSAP:
		    /* Don't accept RIP if RIP is explicitly off */
		    /* Don't accept numbered RIP if we both can't assign net number */
		    wants_routing_mode = TRUE;
		    if ((nidb->lsp_rip_cfg != IPX_RIPSAP_CONFIG_OFF) &&
			!((GETLONG(ipxwan->node_id) == 0L) &&
			  (nidb->nov_ipxwan_external_net == 0L)) &&
			!nidb->nov_ipxwan_static && !wants_client_mode) {
			accepted_routing_type = TRUE;
			optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
			nov_ipxwan_set_routing_type(
			    nidb, NOV_IPXWAN_OPTION_ROUTING_TYPE_RIPSAP);
		    } else {
			optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
		    }
		    break;
		case NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP:
		    wants_routing_mode = TRUE;
		    if ((nidb->nov_lsp_enabled && !nidb->nov_lsp_isis) &&
			!nidb->nov_ipxwan_static && !wants_client_mode) {
			accepted_routing_type = TRUE;
			optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
			nov_ipxwan_set_routing_type(
			    nidb, NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP);
		    } else {
			optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
		    }
		    break;
		case NOV_IPXWAN_OPTION_ROUTING_TYPE_UNN_RIP:
		    wants_routing_mode = TRUE;
		    /* Don't accept unnumbered RIP if RIP is turned off */
		    if ((nidb->nov_ipxwan_external_net == 0) &&
			(nidb->lsp_rip_cfg != IPX_RIPSAP_CONFIG_OFF) &&
			!nidb->nov_ipxwan_static && !wants_client_mode) {
			accepted_routing_type = TRUE;
			optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
			nov_ipxwan_set_routing_type(
			    nidb, NOV_IPXWAN_OPTION_ROUTING_TYPE_UNN_RIP);
		    } else {
			optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
		    }
		    break;
		case NOV_IPXWAN_OPTION_ROUTING_TYPE_STATIC:
		    wants_routing_mode = TRUE;
		    /* Don't accept static unless NLSP and RIP aren't configured */
		    if (nidb->nov_ipxwan_static && 
			!(nidb->nov_lsp_enabled && !nidb->nov_lsp_isis && 
			  !wants_client_mode)) {
			accepted_routing_type = TRUE;
			optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
			nov_ipxwan_set_routing_type(
			    nidb, NOV_IPXWAN_OPTION_ROUTING_TYPE_STATIC);
		    } else {
			optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
		    }
		    break;
		case NOV_IPXWAN_OPTION_ROUTING_TYPE_CLIENT:
		    wants_client_mode = TRUE;
		    /* Don't accept client unless NLSP and RIP aren't configured */
		    if (nidb->nov_ipxwan_client && !wants_routing_mode &&
			!(nidb->nov_lsp_enabled && !nidb->nov_lsp_isis) &&
			!nidb->nov_ipxwan_static) {
			accepted_routing_type = TRUE;
			optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
			nov_ipxwan_set_routing_type(
			    nidb, NOV_IPXWAN_OPTION_ROUTING_TYPE_CLIENT);
		    } else {
			optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
		    }
		    break;
		default:
                    optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
                    break;
                }
                if (novellipxwan_debug)
                    buginf("\nIPXWAN: Rcv TIMER_REQ %s as SLAVE %s on %s",
                           (accepted_routing_type ? "accept" : "reject"),
                           nov_ipxwan_router_type_to_string(
                               routeoptptr->routing_type), idb->namestring);
                if ((nidb->nov_ipxwan_routing_type ==
                     NOV_IPXWAN_OPTION_ROUTING_TYPE_CLIENT) &&
                    wants_routing_mode) {
                    if (novellipxwan_debug)
                        buginf("\nIPXWAN: Rcv TIMER_REQ reject Router asking fo
r Client mode %s",
                               idb->namestring);
                    retbuffer(pak);
                    /* Spec says we should "disconnect on the underlying protocol" */
                    nov_ipxwan_reset_nidb(nidb);
                    return;
                }
            } else {
                optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
                if (novellipxwan_debug)
                    buginf("\nIPXWAN: Rcv TIMER_REQ reject as SLAVE %s on %s",
                           nov_ipxwan_router_type_to_string(
                               routeoptptr->routing_type), idb->namestring);
                if ((nidb->nov_ipxwan_routing_type ==
                     NOV_IPXWAN_OPTION_ROUTING_TYPE_CLIENT) &&
                    wants_routing_mode) {
                    if (novellipxwan_debug)
                        buginf("\nIPXWAN: Rcv TIMER_REQ reject Router asking for Client mode %s",
                               idb->namestring);
                    retbuffer(pak);
                    /* Spec says we should "disconnect on the underlying protocol" */
                    nov_ipxwan_reset_nidb(nidb);
                    return;
                }
            }
            break;
	case NOV_IPXWAN_OPTION_EXT_NODE_INFO:
	    tot_options_length = tot_options_length - 
		NOV_IPXWAN_OPTION_HEADER_SIZE -
		GETSHORT(optptr->option_length);
	    if (novellipxwan_debug) {
		buginf("\nIPXWAN: Rcv TIMER_REQ receive as SLAVE Ext-NodeID option %s",
		       idb->namestring);
	    }
	    break;
	case NOV_IPXWAN_OPTION_COMPRESSION:
	    tot_options_length = tot_options_length - 
		NOV_IPXWAN_OPTION_HEADER_SIZE -
		GETSHORT(optptr->option_length);
	    compressionoptptr = (nov_ipxwan_option_compression_info *) optptr;
	    if (optptr->option_accept == NOV_IPXWAN_OPTION_ACCEPT_YES) {
		if (reg_invoke_ipx_compression_exists() == FALSE) {
		    compression_enabled = FALSE;
		} else {
		    reg_invoke_ipx_compression_options_provide
			(nidb, &compression_enabled, &compression_type,
			 &compression_options, &compression_slots);
		}
		if (compression_enabled) {
		    if (compressionoptptr->compression_type !=
			compression_type) compression_enabled = FALSE;
		}
		if (!compression_enabled) {
		    optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
		    compressionoptptr->compression_type =
			compressionoptptr->compression_options =
			compressionoptptr->compression_slots = 0;
		    reg_invoke_ipx_compression_options_receive(
			nidb, FALSE, 0, 0, 0);
		} else {
		    optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
		    compression_slots =
			min(compressionoptptr->compression_slots,
			    compression_slots);
		    compression_options =
			compressionoptptr->compression_options &
			compression_options;
		    compression_type = compressionoptptr->compression_type;
		    compressionoptptr->compression_type = compression_type;
		    compressionoptptr->compression_options =
			compression_options;
		    compressionoptptr->compression_slots =  compression_slots;
		    reg_invoke_ipx_compression_options_receive(
			nidb, compression_enabled, compression_type,
			compression_options, compression_slots);
		}
	    } else {
		optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
		compressionoptptr->compression_type =
		    compressionoptptr->compression_options =
		    compressionoptptr->compression_slots = 0;
		reg_invoke_ipx_compression_options_receive(
		    nidb, FALSE, 0, 0, 0);
	    }
	    if (novellipxwan_debug) {
		buginf("\nIPXWAN: Rcv TIMER_REQ %s as SLAVE compression %s",
		       (optptr->option_accept ? "accept" : "reject"),
		       idb->namestring);
	    }
	    break;
	case NOV_IPXWAN_OPTION_PAD:
	    tot_options_length = tot_options_length - 
		NOV_IPXWAN_OPTION_HEADER_SIZE -
		GETSHORT(optptr->option_length);
	    /* Pad needs to do nothing */
	    break;
	default:
	    tot_options_length = tot_options_length - 
		NOV_IPXWAN_OPTION_HEADER_SIZE -
		GETSHORT(optptr->option_length);
	    if (optptr->option_accept != NOV_IPXWAN_OPTION_ACCEPT_NA) {
		optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
		if (novellipxwan_debug) {
		    buginf("\nIPXWAN: Rcv TIMER_REQ reject as SLAVE unknown option %s",
			   idb->namestring);
		}
	    }
	    break;
	}
	optptr = (nov_ipxwan_option *)
	    (((uchar *) optptr) + NOV_IPXWAN_OPTION_HEADER_SIZE +
	     GETSHORT(optptr->option_length));
    }
    /* First did the options exceed the packet length? If so, abort */
    if (tot_options_length < 0) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Options extend beyond end of packet %s/%x:%x",
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
	/* Spec says we should "disconnect on the underlying protocol" */
	retbuffer(pak);
	nov_ipxwan_reset_nidb(nidb);
	return;
    }
    if (!accepted_routing_type) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Unable to reach routing type agreement on %s/%x:%x",
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
	/* Spec says we should "disconnect on the underlying protocol" */
	retbuffer(pak);
	nov_ipxwan_reset_nidb(nidb);
	return;
    }
    nidb->nov_ipxwan_remote_internal_net = 
	node_id ? node_id : GETLONG(ipxwan->node_id);
    ipxwan->packet_type = NOV_IPXWAN_TIMER_RSP_PKT_TYPE;
    PUTLONG(ipxwan->node_id, NOVELL_INTERNAL_ID(nidb));
    pak->if_output = idb;
    clear_if_input(pak);
    pak->nif_output = nidb;
    if (!nov_send1(pak, ipxwan->dhost, ipxwan->len)) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Cannot send TIMER_RSP packet out %s/%x:%x",
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
    } else {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Sending TIMER_RSP packet out %s/%x:%x",
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
	if (nidb->nov_ipxwan_routing_type !=
	    NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP)
	    /* Sent the packet. Change our state */
	    nov_ipxwan_new_state(nidb,
				 NOV_IPXWAN_STATE_SLAVE_WAIT_ROUTER_INFO_REQ,
				 "Sent Timer Response as Slave");
	else {
	    /* NLSP need to wait for THROUGHPUT */
	    nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_SLAVE_WAIT_THROUGHPUT,
				 "Sent Timer Response as Slave");
	    /* Clear the NLSP throughput and delay values */
	    nidb->nov_ipxwan_link_throughput = 0;
	    TIMER_STOP(nidb->nov_ipxwan_link_throughput_timer);
	    nidb->nov_ipxwan_link_delay = 0;
	    TIMER_STOP(nidb->nov_ipxwan_link_delay_timer);
	}
    }
}

/*
 *
 * nov_ipxwan_process_timer_rsp
 *
 * Process NOV_IPXWAN_TIMER_RSP_PKT_TYPE IPXWAN packets.
 *
 */
static void nov_ipxwan_process_timer_rsp (paktype *pak)
{
    ipxwanhdrtype *ipxwan;
    idbtype *idb;
    nidbtype *nidb;
    nov_ipxwan_option *optptr;
    nov_ipxwan_option_routing_type *routeoptptr;
    boolean accepted_routing_type = FALSE;
    boolean accepted_dual = FALSE;
    nov_ipxwan_option_compression_info *compressionoptptr;
    boolean compression_enabled;
    uchar compression_type, compression_options, compression_slots;
    int curopt;
    
    ipxwan = (ipxwanhdrtype *)ipxheadstart(pak);
    nidb = pak->nif_input;
    idb = nidb->idb;

    if (idb->novell_delay != 0) {
	nidb->nov_ipxwan_delay = idb->novell_delay;
    } else {
	nidb->nov_ipxwan_delay =
	  IPXWAN_TIME_DELTA(nidb->nov_ipxwan_pak_sent_time, pak->inputtime);
	/* At least 55 msecs per RFC1362. */
	if (nidb->nov_ipxwan_delay < 55) {
	    nidb->nov_ipxwan_delay = 55;
	}
	/* Biasing... per RFC1362 */
	nidb->nov_ipxwan_delay *= 6;
	/* Now... Adjust our msclock to a 1/18second clock */
	nidb->nov_ipxwan_delay /= 55;
    }
    if (novellipxwan_debug) {
	buginf("\nIPXWAN: Rcv TIMER_RSP on %s/%x:%x, NodeID %x, Seq %x, Del %u",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net, GETLONG(ipxwan->node_id),
	       ipxwan->sequence_number, nidb->nov_ipxwan_delay);
    }
    /*
     * Okay. We got a response. We must be the master. Verify that it is the
     * one we sent.
     */
    if (ipxwan->sequence_number != nidb->nov_ipxwan_treq_expect) {
	retbuffer(pak);
	return;
    }
    if (nidb->nov_ipxwan_state > NOV_IPXWAN_STATE_TIMER_REQ_SENT) {
      if (novellipxwan_debug) {
	buginf("\nIPXWAN: Tossing TIMER_RSP request on %s/%x:%x in [%s]",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net,
	       nov_ipxwan_state_to_string(nidb->nov_ipxwan_state));
      }
      retbuffer(pak);
      /* Spec says we should "disconnect on the underlying protocol" */
      nov_ipxwan_reset_nidb(nidb);
      return;
    }

    /*
     * Well, we appear to be a master, today... Process the packet, and
     * set ourselves up to send a routing information request packet.
     */
    
    /*
     * Handle any options
     */
    for (curopt = 0,
	 optptr = (nov_ipxwan_option *) (&(ipxwan->num_options) + 1);
	 curopt < ipxwan->num_options;
	 curopt++) {
      switch (optptr->option_number) {
      case NOV_IPXWAN_OPTION_ROUTING_TYPE:
	  routeoptptr = (nov_ipxwan_option_routing_type *) optptr;
	  switch(routeoptptr->routing_type) {
	  case NOV_IPXWAN_OPTION_ROUTING_TYPE_RIPSAP:
	      if (optptr->option_accept == NOV_IPXWAN_OPTION_ACCEPT_YES) {
		  if (accepted_routing_type) accepted_dual = TRUE;
		  accepted_routing_type = TRUE;
		  nov_ipxwan_set_routing_type(nidb,
					      NOV_IPXWAN_OPTION_ROUTING_TYPE_RIPSAP);
		  if (novellipxwan_debug)
		      buginf("\nIPXWAN: Rcv TIMER_RSP accept as MASTER %s on %s",
			     nov_ipxwan_router_type_to_string(routeoptptr->routing_type),
			     idb->namestring);
	      }
	      break;
	  case NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP:
	      if (optptr->option_accept == NOV_IPXWAN_OPTION_ACCEPT_YES) {
		  if (accepted_routing_type) accepted_dual = TRUE;
		  accepted_routing_type = TRUE;
		  nov_ipxwan_set_routing_type(nidb,
					      NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP);
		  if (novellipxwan_debug)
		      buginf("\nIPXWAN: Rcv TIMER_RSP accept as MASTER %s on %s",
			     nov_ipxwan_router_type_to_string(routeoptptr->routing_type),
			     idb->namestring);
	      }
	      break;
	  case NOV_IPXWAN_OPTION_ROUTING_TYPE_UNN_RIP:
	      if (optptr->option_accept == NOV_IPXWAN_OPTION_ACCEPT_YES) {
		  if (accepted_routing_type) accepted_dual = TRUE;
		  accepted_routing_type = TRUE;
		  nov_ipxwan_set_routing_type(nidb,
					      NOV_IPXWAN_OPTION_ROUTING_TYPE_UNN_RIP);
		  if (novellipxwan_debug)
		      buginf("\nIPXWAN: Rcv TIMER_RSP accept as MASTER %s on %s",
			     nov_ipxwan_router_type_to_string(routeoptptr->routing_type),
			     idb->namestring);
	      }
	      break;
	  case NOV_IPXWAN_OPTION_ROUTING_TYPE_STATIC:
	      if (optptr->option_accept == NOV_IPXWAN_OPTION_ACCEPT_YES) {
		  if (accepted_routing_type) accepted_dual = TRUE;
		  accepted_routing_type = TRUE;
		  nov_ipxwan_set_routing_type(nidb,
					      NOV_IPXWAN_OPTION_ROUTING_TYPE_STATIC);
		  if (novellipxwan_debug)
		      buginf("\nIPXWAN: Rcv TIMER_RSP accept as MASTER %s on %s",
			     nov_ipxwan_router_type_to_string(routeoptptr->routing_type),
			     idb->namestring);
	      }
	      break;
	  case NOV_IPXWAN_OPTION_ROUTING_TYPE_CLIENT:
	      if (optptr->option_accept == NOV_IPXWAN_OPTION_ACCEPT_YES) {
		  if (accepted_routing_type) accepted_dual = TRUE;
		  accepted_routing_type = TRUE;
		  nov_ipxwan_set_routing_type(nidb,
					      NOV_IPXWAN_OPTION_ROUTING_TYPE_CLIENT);
		  if (novellipxwan_debug)
		      buginf("\nIPXWAN: Rcv TIMER_RSP accept as MASTER %s on %s",
			     nov_ipxwan_router_type_to_string(routeoptptr->routing_type),
			     idb->namestring);
	      }
	      break;
	  default:
	      /* Unknown routing type */
	      break;
	  }
	  if (novellipxwan_debug && !accepted_routing_type)
	      buginf("\nIPXWAN: Rcv TIMER_RSP reject as MASTER %s on %s",
		     nov_ipxwan_router_type_to_string(routeoptptr->routing_type),
		     idb->namestring);
	  break;
      case NOV_IPXWAN_OPTION_COMPRESSION:
	  compressionoptptr = (nov_ipxwan_option_compression_info *) optptr;
	  compression_enabled = FALSE;
	  if (optptr->option_accept == NOV_IPXWAN_OPTION_ACCEPT_YES) {
	      if (reg_invoke_ipx_compression_exists() == TRUE) {
		  reg_invoke_ipx_compression_options_provide
		      (nidb, &compression_enabled, &compression_type,
		       &compression_options, &compression_slots);
		  compression_slots =
		      min(compressionoptptr->compression_slots,
			  compression_slots);
		  compression_options =
		      compressionoptptr->compression_options &
			  compression_options;
		  if (compressionoptptr->compression_type  != compression_type)
		      compression_enabled = FALSE;
		  if (compression_enabled) {
		      reg_invoke_ipx_compression_options_receive
			  (nidb, TRUE, compression_type, compression_options,
			   compression_slots);
		  } else {
		      reg_invoke_ipx_compression_options_receive(nidb, FALSE,
								 0, 0, 0);
		  }
	      }
	  } else {
	      compressionoptptr->compression_type =
		  compressionoptptr->compression_options =
		      compressionoptptr->compression_slots = 0;
	      reg_invoke_ipx_compression_options_receive(nidb, FALSE, 0, 0, 0);
	  }
	  if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Rcv TIMER_RSP %s as MASTER compression %s",
		   (compression_enabled ? "accept" : "reject"),
		   idb->namestring);
	  }
	  break;
      case NOV_IPXWAN_OPTION_PAD:
	  break;
      default:
	  break;
      }
      optptr = (nov_ipxwan_option *)
	(((uchar *) optptr) + NOV_IPXWAN_OPTION_HEADER_SIZE +
	 GETSHORT(optptr->option_length));
    }
    nidb->nov_ipxwan_remote_internal_net = GETLONG(ipxwan->node_id);
    /* Did they accept our routing type? */
    if (accepted_routing_type && (!accepted_dual)) {
	/* Good! Now set ourselves up to send an information request */
	/* First, set up link delay */

	if (nidb->nov_ipxwan_routing_type ==
	    NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP) {
	  nov_ipxwan_new_state(nidb,
			       NOV_IPXWAN_STATE_NLSP_THROUGHPUT_SENT,
			       "Received Timer Response as master");
	  /* Clear the NLSP troughput and delay values */
	  nidb->nov_ipxwan_link_throughput = 0;
	  TIMER_STOP(nidb->nov_ipxwan_link_throughput_timer);
	  nidb->nov_ipxwan_link_delay = 0;
	  TIMER_STOP(nidb->nov_ipxwan_link_delay_timer);
	} else {
	  /* Here we have a RIP/SAP type routing */
	  nov_ipxwan_new_state(nidb,
			       NOV_IPXWAN_STATE_MASTER_WAIT_ROUTER_INFO_RSP,
			       "Received Timer Response as Master");
	}	  
    } else {
      /* Here we were unable to agree on a routing type */
      if (novellipxwan_debug) {
	buginf("\nIPXWAN: Unable to reach routing type agreement on %s/%x:%x",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net);
      }
      /* Spec says we should "disconnect on the underlying protocol" */
      /* The below doens't cause us to PASS, so don't use it */
      /*      (*(idb->hwptr->reset))(idb->hwptr); */
      nov_ipxwan_reset_nidb(nidb);
    }
    retbuffer(pak);
}

/*
 *
 * nov_ipxwan_send_nak
 *
 * Received a bad IPXWAN packet type. Return it with FF as packet type.
 * and our WNode ID as WNode ID, instead of theirs.
 *
 */
static void nov_ipxwan_send_nak (paktype *pak)
{
    paktype *newpak;
    ipxwanhdrtype *ipxwan;
    nidbtype *nidb;
    
    nidb = pak->nif_input;

    newpak = pak_duplicate(pak);
    if (newpak == NULL) {
      return;
    }
    ipxwan = (ipxwanhdrtype *)ipxheadstart(newpak);

    newpak->if_output = pak->if_input;
    newpak->nif_output = pak->nif_input;
    ipxwan->tc = 0;  /* should be 0 but make sure */
    ipxwan->packet_type = NOV_IPXWAN_NAK_PKT_TYPE;
    PUTLONG(ipxwan->node_id, NOVELL_INTERNAL_ID(nidb));
    if (novellipxwan_debug)
      buginf("\nIPXWAN: NAK sent %s/%x:%x",
	     pak->if_input->namestring,
	     NOVELL_INTERNAL_ID(pak->nif_input),
	     pak->nif_input->nov_ipxwan_external_net);

    nov_send1(newpak, ipxwan->dhost, ipxwan->len);
}

/*
 *
 * nov_ipxwan_process_ripsap_info_req
 *
 * Process NOV_IPXWAN_RIPSAP_INFO_REQ_PKT_TYPE IPXWAN packets.
 *
 */
static void nov_ipxwan_process_ripsap_info_req (paktype *pak)
{
    ipxwanhdrtype *ipxwan;
    nidbtype *nidb;
    idbtype *idb;
    nov_ipxwan_option *optptr;
    nov_ipxwan_option_ripsap_info *ripsapoptptr;
    nov_ipxwan_option_nlsp_info *nlspoptptr;
    nov_ipxwan_option_client_info *ipxclnt; 
    boolean accepted_routing_type = FALSE;
    boolean ripsap_info_present = FALSE;
    boolean nlsp_info_present = FALSE;
    ulong net_number = 0;
    ushort delay_number = 0;
    ulong nlsp_delay = 0;
    ulong nlsp_throughput = 0;
    leveltype level;

    int curopt, len;
    
    ipxwan = (ipxwanhdrtype *)ipxheadstart(pak);
    nidb = pak->nif_input;
    idb = nidb->idb;
    if (novellipxwan_debug) {
	buginf("\nIPXWAN: Rcv ROUTING_INFO_REQ from %s/%x:%x, NodeID %x, Seq %x",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net, GETLONG(ipxwan->node_id),
	       ipxwan->sequence_number);
    }
    /*
     * Okay. We got a request. Are we in the right state to receive one?
     * we might have to resend so allow state after this one to send too.
     * If not, chuck the thing. 
     */
    if (nidb->nov_ipxwan_state < NOV_IPXWAN_STATE_SLAVE_WAIT_ROUTER_INFO_REQ) {
      if (novellipxwan_debug) {
	buginf("\nIPXWAN: Tossing ROUTING_INFO_REQ request on %s/%x:%x in [%s]",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net,
	       nov_ipxwan_state_to_string(nidb->nov_ipxwan_state));
      }
      retbuffer(pak);
      /* Spec says we should "disconnect on the underlying protocol" */
      nov_ipxwan_reset_nidb(nidb);
      return;
    }
    /*
     * Well, we appear to still be a slave, today... Process the packet, and
     * send a reply, and do some work.
     */
    
    /*
     * Modify the packet. Handle any options
     */
    for (curopt = 0,
	 optptr = (nov_ipxwan_option *) (&(ipxwan->num_options) + 1);
	 curopt < ipxwan->num_options;
	 curopt++) {
      ripsapoptptr = (nov_ipxwan_option_ripsap_info *) optptr;
      len = GETSHORT(optptr->option_length);
	switch (optptr->option_number) {
	  case NOV_IPXWAN_OPTION_RIPSAP_INFO:
	    ripsapoptptr = (nov_ipxwan_option_ripsap_info *) optptr;
	    accepted_routing_type = TRUE;
	    ripsap_info_present = TRUE;
	    optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
	    net_number = GETLONG(ripsapoptptr->network_number);
	    delay_number = GETSHORT(ripsapoptptr->link_delay);
	    bcopy(ripsapoptptr->router_name,
		  nidb->nov_ipxwan_remote_routername, NOVELL_SERVER_NAME_LEN);
	    bcopy(nidb->nov_ipxwan_routername, ripsapoptptr->router_name,
		  NOVELL_SERVER_NAME_LEN);

	    /* If the RIP/SAP Length isn't right len don't accept it */
	    if (len != NOV_IPXWAN_OPTION_RIPSAP_INFO_LENGTH)
	      accepted_routing_type = FALSE;
	    /* If we negotiated unnumbered but he says link is numbered */
	    if ((net_number != 0) && (nidb->nov_ipxwan_routing_type ==
		NOV_IPXWAN_OPTION_ROUTING_TYPE_UNN_RIP))
	      accepted_routing_type = FALSE;
	    if ((net_number == 0) && (nidb->nov_ipxwan_routing_type ==
		NOV_IPXWAN_OPTION_ROUTING_TYPE_RIPSAP)) {
	      accepted_routing_type = FALSE;
	      retbuffer(pak);
	      /* Spec says we should "disconnect on the underlying protocol" */
	      nov_ipxwan_reset_nidb(nidb);
	      return;
	    }
	    break;

	  case NOV_IPXWAN_OPTION_NLSP_INFO:
	    nlspoptptr = (nov_ipxwan_option_nlsp_info *) optptr;
	    if (len == NOV_IPXWAN_OPTION_NLSP_INFO_LENGTH) {
	      nlsp_info_present = TRUE;
	      nlsp_delay = GETLONG(nlspoptptr->delay);
	      nlsp_throughput = GETLONG(nlspoptptr->throughput);
	      if (nlsp_delay == 0) nlsp_info_present = FALSE;
	      if (nlsp_throughput == 0) nlsp_info_present = FALSE;

	      /* If we want RIP then we don't want NLSP */
	      if (nidb->nov_ipxwan_routing_type != 
		  NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP)
		optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
	      if (novellipxwan_debug) {
		buginf("\nIPXWAN: NLSP delay %d, throughput %d", nlsp_delay,
		       nlsp_throughput);
	      }
	    } else {
	      nlsp_info_present = FALSE;
	    }
	    break;

	  case NOV_IPXWAN_OPTION_CLIENT_INFO:
	    if (nidb->nov_ipxwan_routing_type ==
		NOV_IPXWAN_OPTION_ROUTING_TYPE_CLIENT) {
	      optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;

	      ipxclnt = (nov_ipxwan_option_client_info *) 
		(((uchar *) (&ripsapoptptr->link_delay)) +
		 NOV_IPXWAN_OPTION_RIPSAP_INFO_LENGTH);

	      /* Get the CLient node number for the interface */
	      ieee_copy(ipxclnt->mac_address, nidb->nov_ipxwan_clnt_address);
	      if (novellipxwan_debug) {
		buginf("\nIPXWAN: Client address %e assigned",ipxclnt->mac_address);
	      }
	    } else {
	      optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
	    }
	    break;

	  default:
	    if (optptr->option_accept != NOV_IPXWAN_OPTION_ACCEPT_NA) {
		optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
	    }
	    break;
	}
	optptr = (nov_ipxwan_option *)
	  (((uchar *) optptr) + NOV_IPXWAN_OPTION_HEADER_SIZE +
	   GETSHORT(optptr->option_length));
    }
    if (nidb->nov_ipxwan_routing_type == NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP) {
      accepted_routing_type = nlsp_info_present;
    }
    /* If we did accept a routing type and had ripsap info we reply */
    if (ripsap_info_present && accepted_routing_type) {
      nidb->nov_ipxwan_remote_internal_net = GETLONG(ipxwan->node_id);
      ipxwan->packet_type = NOV_IPXWAN_RIPSAP_INFO_RSP_PKT_TYPE;
      PUTLONG(ipxwan->node_id, NOVELL_INTERNAL_ID(nidb));
      pak->if_output = idb;
      clear_if_input(pak);
      pak->nif_output = idb->primary_nidb;
      if (!nov_send1(pak, ipxwan->dhost, ipxwan->len)) {
	if (novellipxwan_debug) {
	  buginf("\nIPXWAN: Cannot send ROUTING_INFO_RSP out %s/%x:%x",
		 idb->namestring, NOVELL_INTERNAL_ID(nidb),
		 nidb->nov_ipxwan_external_net);
	}
      } else {
	/* Sent the packet. Change our state. */
	/* Interrupts must be blocked out so this is an atomic act */
	if (accepted_routing_type == TRUE) {
	  level = raise_interrupt_level(NETS_DISABLE);
	  novell_nidb_set_network_number(nidb, net_number);
	  nidb->nov_ipxwan_delay = delay_number;
	  nidb->nov_ipxwan_link_delay = nlsp_delay;
	  nidb->nov_ipxwan_link_throughput = nlsp_throughput;
	  /* Now... Adjust our msclock to a 1/18second clock */
	  nidb->nov_ipxwan_delay /= 55;
	  nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_SLAVE_CONNECT,
			       "Received Router Info Req as Slave");
	  nov_network_finish(&novrip_pdb, nidb->novellnet, TRUE);
	  reset_interrupt_level(level);
	}
      }
    } else {
      /* No RIP/SAP options stop now, reset state */
      if (novellipxwan_debug) {
	buginf("\nIPXWAN: Invalid ROUTING_INFO_REQ %s/%x:%x",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net);
      }
      /* only send NAKs if we aren't doing numbered RIP,  */
      /* compatible with IPXWAN V1 */
      if (nidb->nov_ipxwan_routing_type >
	  NOV_IPXWAN_OPTION_ROUTING_TYPE_RIPSAP) {
	nov_ipxwan_send_nak(pak);
      } else {
	/* Spec says we should "disconnect on the underlying protocol" */
	nov_ipxwan_reset_nidb(nidb);
      }
      retbuffer(pak);
    }
}

/*
 *
 * nov_ipxwan_process_nlsp_thru_req
 *
 * Process NOV_IPXWAN_NOV_IPXWAN_NLSP_THRU_REQ_PKT_TYPE IPXWAN packets.
 *
 */
static void nov_ipxwan_process_nlsp_thru_req (paktype *pak)
{
    ipxwanthruputinfo_hdrtype *ipxwan;
    nidbtype *nidb;
    idbtype *idb;
    int len;
    
    ipxwan = (ipxwanthruputinfo_hdrtype *)ipxheadstart(pak);
    nidb = pak->nif_input;
    idb = nidb->idb;
    if (novellipxwan_debug) {
	buginf("\nIPXWAN: Rcv THROUGHPUT from %s/%x:%x, NodeID %x, Seq %x",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net, GETLONG(ipxwan->node_id),
	       ipxwan->sequence_number);
    }
    /*
     * Okay. We got a request. Are we in the right state to receive one?
     * If not, chuck the thing. 
     */
    if ((nidb->nov_ipxwan_state >= NOV_IPXWAN_STATE_SLAVE_WAIT_THROUGHPUT)
	&& (nidb->nov_ipxwan_state <= 
	    NOV_IPXWAN_STATE_SLAVE_WAIT_ROUTER_INFO_REQ)
	&& (nidb->nov_ipxwan_routing_type == 
	    NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP)) {
      /*
       * Okay. We have one throughput packet. Set a time stamp if first
       * If second we need to reply. Turn packet around quickly.
       */
      /* If we are expecting a different sequence number toss the old info
       * this is now a new first throughput
       */
      if (TIMER_RUNNING(nidb->nov_ipxwan_link_throughput_timer) &&
	  (nidb->nov_ipxwan_treq_expect != ipxwan->sequence_number))
	TIMER_STOP(nidb->nov_ipxwan_link_throughput_timer);

      if (!TIMER_RUNNING(nidb->nov_ipxwan_link_throughput_timer)) {
	COPY_TIMESTAMP(pak->inputtime, nidb->nov_ipxwan_link_throughput_timer);
	nidb->nov_ipxwan_treq_expect = ipxwan->sequence_number;
	retbuffer(pak);
	return;
      } else {
	/* Second Throughput packet, build response */
	/* first trim off padding, by cutting packet size. */
	pak->datagramsize = NOV_IPXWAN_NLSP_THRU_RSP_PKT_LEN;
	len = ipxwan->len;       
	ipxwan->len = NOV_IPXWAN_NLSP_THRU_RSP_PKT_LEN;

	PUTLONG(ipxwan->node_id, NOVELL_INTERNAL_ID(nidb));
	ipxwan->packet_type = NOV_IPXWAN_NLSP_THRU_RSP_PKT_TYPE;
	ipxwan->num_options = 1;

	ipxwan->option_number = NOV_IPXWAN_OPTION_NLSP_RAW_INFO;
	ipxwan->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
	PUTSHORT(ipxwan->option_length,
		 NOV_IPXWAN_OPTION_NLSP_RAW_INFO_LENGTH);
	PUTLONG(ipxwan->size, len);
	PUTLONG(ipxwan->delta,
		IPXWAN_TIME_DELTA(nidb->nov_ipxwan_link_throughput_timer,
				  pak->inputtime) * 1000);
	pak->if_output = pak->if_input;
	pak->nif_output = pak->nif_input;

	nov_send1(pak,ipxwan->dhost, ipxwan->len);
	if (novellipxwan_debug) {
	  buginf("\nIPXWAN: Send THROUGHPUT_RSP %s/%x:%x, Seq %x",
		 idb->namestring, NOVELL_INTERNAL_ID(nidb),
		 nidb->nov_ipxwan_external_net, ipxwan->sequence_number);
	}
	if (novellipxwan_debug)
	    buginf("\nIPXWAN: Throughput %d %d %d", GETLONG(ipxwan->size),
		   GETLONG(ipxwan->delta),
		   ELAPSED_TIME(nidb->nov_ipxwan_link_throughput_timer));
	/* NLSP need to wait for SECOND THROUGHPUT PAIR */
	if (nidb->nov_ipxwan_link_throughput == 0) {
	  /* First throughput response, stay same state */
	  nidb->nov_ipxwan_link_throughput = 
	      IPXWAN_TIME_DELTA(nidb->nov_ipxwan_link_throughput_timer,
				pak->inputtime);
	  nidb->nov_ipxwan_treq_count++;
	  TIMER_STOP(nidb->nov_ipxwan_link_throughput_timer);
	  nov_ipxwan_update_timer(nidb, nidb->nov_ipxwan_treq_int * ONESEC);
	} else {
	  /* second throughput response move to delay waiting */
	  nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_SLAVE_WAIT_DELAY,
			       "Sent Second Throughput Response");
	  /* Clear the NLSP troughput and delay values */
	  nidb->nov_ipxwan_link_throughput = 0;
	  TIMER_STOP(nidb->nov_ipxwan_link_throughput_timer);
	  nidb->nov_ipxwan_link_delay = 0;
	  TIMER_STOP(nidb->nov_ipxwan_link_delay_timer);
	}
      }
    } else {
      /* Wrong state through away the throughput quietly */
      if (novellipxwan_debug) {
	buginf("\nIPXWAN: Tossing THROUGHPUT request on %s/%x:%x in [%s]",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net,
	       nov_ipxwan_state_to_string(nidb->nov_ipxwan_state));
      }
      retbuffer(pak);
      /* Spec says we should "disconnect on the underlying protocol" */
      /* reset only if we aren't already in Connect State  otherwise */
      /* we just ignore any spurious throughput requests (IPXWAN VX) */
      if (nidb->nov_ipxwan_state <
	  NOV_IPXWAN_STATE_SLAVE_CONNECT) {
	nov_ipxwan_reset_nidb(nidb);
      }
      return;
    }
}

/*
 * nov_ipxwan_master_wait_throughput_rsp_timer
 *
 * Process IPXWAN MASTER_WAIT_THROUGHPUT_RSP timeouts
 */
static void nov_ipxwan_master_wait_throughput_rsp_timer (nidbtype *nidb)
{
    paktype *pak, *pak2;
    ipxwanthruputinfo_hdrtype *ipxwan;
    nov_ipxwan_option_pad *ipxpad;
    int i;

    /* Time to give up? */
    if (nidb->nov_ipxwan_treq_count >= nidb->nov_ipxwan_treq_max) {
	nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_NLSP_THROUGHPUT_SENT,
			     "Master Wait Throughput timer went off");
	return;
    }
    if (interface_up(nidb->idb)) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Send NLSP THROUGHPUT [seq %d] out %s/%x:%x",
		   nidb->nov_ipxwan_treq_count, nidb->idb->namestring,
		   NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
	pak = getbuffer(NOV_IPXWAN_NLSP_THRU_REQ_PKT_LEN);
	if (!pak) {
	  return;
	}
	ipxwan = (ipxwanthruputinfo_hdrtype *)ipxheadstart(pak);

	pak->linktype = LINK_NOVELL;
	pak->nif_output = nidb; 
	pak->if_output = nidb->idb;
	pak->datagramsize = NOV_IPXWAN_NLSP_THRU_REQ_PKT_LEN;
	ipxwan->cksum = NOVELL_NO_CHECKSUM;
	ipxwan->len = NOV_IPXWAN_NLSP_THRU_REQ_PKT_LEN;
	ipxwan->tc = 0;
	ipxwan->pt = NOVELL_TYPE_PEP;
	memset(ipxwan->ddnet, 0, XNS_NETBYTES);
	ieee_copy(baddr, ipxwan->dhost);
	ipxwan->dsock = NOVELL_SOCK_IPXWAN;
	memset(ipxwan->ssnet, 0, XNS_NETBYTES);
	memset(ipxwan->shost, 0, IEEEBYTES);
	ipxwan->ssock = NOVELL_SOCK_IPXWAN;
	PUTLONG(ipxwan->confidence_identifier, NOV_IPXWAN_CONFIDENCE_ID);
	ipxwan->packet_type = NOV_IPXWAN_NLSP_THRU_REQ_PKT_TYPE;
	PUTLONG(ipxwan->node_id, NOVELL_INTERNAL_ID(nidb));
	ipxwan->sequence_number = nidb->nov_ipxwan_treq_count;
	nidb->nov_ipxwan_treq_expect = nidb->nov_ipxwan_treq_count;
	nidb->nov_ipxwan_treq_count++;
	ipxwan->num_options = 1;

	/* Point to pad area */
        ipxpad = (nov_ipxwan_option_pad *)
	  (((uchar *) &(ipxwan->num_options)) + 1);

        /* Padding option to the end of the packet */
        ipxpad->option_number = NOV_IPXWAN_OPTION_PAD;
        ipxpad->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
	PUTSHORT(ipxpad->option_length,
                   (NOV_IPXWAN_NLSP_THRU_REQ_PKT_LEN -
                    (NOV_IPXWAN_HDRLEN +
                     NOV_IPXWAN_OPTION_HEADER_SIZE)));
        for (i = 0; i < GETSHORT(ipxpad->option_length); i++) {
            ipxpad->padding_area[i] = random_gen() & 0xFF;
	  }

	/* Throughput test calls for two identical packets sent back2back */
	pak2 = pak_duplicate(pak);
	if (!pak2) {
	  return;
	}
	nov_send1(pak, baddr, NOV_IPXWAN_NLSP_THRU_REQ_PKT_LEN);
	nov_send1(pak2, baddr, NOV_IPXWAN_NLSP_THRU_REQ_PKT_LEN);
    }
    nov_ipxwan_update_timer(nidb, nidb->nov_ipxwan_treq_int * ONESEC);
}


/*
 *
 * nov_ipxwan_process_nlsp_thru_rsp
 *
 * Process NOV_IPXWAN_NLSP_THRU_RSP_PKT_TYPE IPXWAN packets.
 *
 */
static void nov_ipxwan_process_nlsp_thru_rsp (paktype *pak)
{
    ipxwanthruputinfo_hdrtype *ipxwan;
    nidbtype *nidb;
    idbtype *idb;

    ipxwan = (ipxwanthruputinfo_hdrtype *)ipxheadstart(pak);
    nidb = pak->nif_input;
    idb = nidb->idb;

    if (novellipxwan_debug) {
	buginf("\nIPXWAN: Rcv THROUGHPUT response from %s/%x:%x, NodeID %x, Seq %x",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net, GETLONG(ipxwan->node_id),
	       ipxwan->sequence_number);
    }
    /*
     * Okay. We got a response. Are we in the right state to receive one?
     * If not, chuck the thing. 
     */
    if (nidb->nov_ipxwan_state <
	NOV_IPXWAN_STATE_NLSP_THROUGHPUT_SENT) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Tossing THROUGHPUT response on %s/%x:%x in [%s]",
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net,
		   nov_ipxwan_state_to_string(nidb->nov_ipxwan_state));
	}
	retbuffer(pak);
	/* Spec says we should "disconnect on the underlying protocol" */
	nov_ipxwan_reset_nidb(nidb);
	return;
    }
    /*
     * Okay. We got a response. We must be the master. Verify that it is the
     * one we sent.
     */
    if (ipxwan->sequence_number != nidb->nov_ipxwan_treq_expect) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Rcv THROUGHPUT_RSP seq %d, want %d, %s/%x:%x",
		   ipxwan->sequence_number, nidb->nov_ipxwan_treq_expect,
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
	retbuffer(pak);
	return;
    }
    
    if ((ipxwan->packet_type == NOV_IPXWAN_NLSP_THRU_RSP_PKT_TYPE) &&
	(ipxwan->option_number == NOV_IPXWAN_OPTION_NLSP_RAW_INFO)) {
	/*
	 * Ok, We have a throughput packet. Determine if this is the first
	 * thoughput response or the second. If first store values slave
	 * gave us. If second, calculate throughput and store.
	 */
	/* Check the size returned. We sent it so it must be the size we send */
	if (GETLONG(ipxwan->size) <= 0) {
	    if (novellipxwan_debug)
		buginf("\nIPXWAN: THROUGHPUT_RSP invalid size %d, adjusting to %d on %s",
		       GETLONG(ipxwan->size), NOV_IPXWAN_NLSP_THRU_REQ_PKT_LEN,
		       idb->namestring);
	    PUTLONG(ipxwan->size, NOV_IPXWAN_NLSP_THRU_REQ_PKT_LEN);
	}
	/* Make sure delta is at least 1 to avoid math problems */
	if (GETLONG(ipxwan->delta) <= 0) {
	    if (novellipxwan_debug)
		buginf("\nIPXWAN: THROUGHPUT_RSP invalid delta %d, adjusting to 1 on %s",
		       GETLONG(ipxwan->delta), idb->namestring);
	    PUTLONG(ipxwan->delta, 1);
	}
	if (nidb->nov_ipxwan_link_throughput == 0) {
	    nidb->nov_ipxwan_link_throughput = 
		(((GETLONG(ipxwan->size) * 8000)
		  / GETLONG(ipxwan->delta)) * 1000);
	    nov_ipxwan_master_wait_throughput_rsp_timer(nidb);
	} else {
	    /* Second Throughput packet, go on to delay */
	    nidb->nov_ipxwan_link_throughput = 
		((nidb->nov_ipxwan_link_throughput +
		  (((GETLONG(ipxwan->size) * 8000) 
		    / GETLONG(ipxwan->delta)) * 1000 )) / 2);
	    nov_ipxwan_new_state(nidb,
				 NOV_IPXWAN_STATE_NLSP_DELAY_SENT,
				 "Received Second Throughput Response ");
	}
	if (novellipxwan_debug)
	    buginf("\nIPXWAN: Throughput size %d delta %d",
		   GETLONG(ipxwan->size), GETLONG(ipxwan->delta));
    } else {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Invalid THROUGHPUT_RSP %s/%x:%x",
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
	/* something is wrong, NAK the packet */
	nov_ipxwan_send_nak(pak);
    }
    retbuffer(pak);
}

/*
 *
 * nov_ipxwan_process_nlsp_delay_req
 *
 * Process NOV_IPXWAN_NOV_IPXWAN_NLSP_DELAY_REQ_PKT_TYPE IPXWAN packets.
 * This is just an echo. Get a back and send it back as fast as possible.
 *
 */
static void nov_ipxwan_process_nlsp_delay_req (paktype *pak)
{
    ipxwanhdrtype *ipxwan;
    nidbtype *nidb;
    idbtype *idb;
    
    ipxwan = (ipxwanhdrtype *)ipxheadstart(pak);
    nidb = pak->nif_input;
    idb = nidb->idb;
    if (novellipxwan_debug) {
	buginf("\nIPXWAN: Rcv DELAY_REQ from %s/%x:%x, NodeID %x, Seq %x",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net, GETLONG(ipxwan->node_id),
	       ipxwan->sequence_number);
    }
    /*
     * Okay. We got a request. Are we in the right state to receive one?
     * If not, chuck the thing. 
     */
    if ((nidb->nov_ipxwan_state <
	 NOV_IPXWAN_STATE_SLAVE_WAIT_DELAY) ||
	(nidb->nov_ipxwan_state >
	 NOV_IPXWAN_STATE_SLAVE_WAIT_ROUTER_INFO_REQ)
	|| (nidb->nov_ipxwan_routing_type != 
	    NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP)) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Tossing DELAY_REQ on %s/%x:%x in [%s]",
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net,
		   nov_ipxwan_state_to_string(nidb->nov_ipxwan_state));
	}
	retbuffer(pak);
	/* Spec says we should "disconnect on the underlying protocol" */
	/* only ignore the packet if we are in connect state           */
	if (nidb->nov_ipxwan_state < NOV_IPXWAN_STATE_SLAVE_CONNECT) {
	  nov_ipxwan_reset_nidb(nidb);
	}
	return;
    }
    /*
     * Okay. We got a request, right sequence number?
     * one we sent.
     */
/*    if (ipxwan->sequence_number != nidb->nov_ipxwan_treq_expect) {
 *	if (novellipxwan_debug) {
 *	    buginf("\nIPXWAN: Rcv DELAY_REQ seq %d, want %d, %s/%x:%x",
 *		   ipxwan->sequence_number, nidb->nov_ipxwan_treq_expect,
 *		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
 *		   nidb->nov_ipxwan_external_net);
 *	}
 *	retbuffer(pak);
 *	return;
 *    }
 */
    PUTLONG(ipxwan->node_id, NOVELL_INTERNAL_ID(nidb));
    ipxwan->packet_type = NOV_IPXWAN_NLSP_DELAY_RSP_PKT_TYPE;
    pak->if_output = pak->if_input;
    pak->nif_output = pak->nif_input;
    nov_send1(pak,ipxwan->dhost, ipxwan->len);
    if (novellipxwan_debug) {
      buginf("\nIPXWAN: Send DELAY_RSP %s/%x:%x, Seq %x",
	     idb->namestring, NOVELL_INTERNAL_ID(nidb),
	     nidb->nov_ipxwan_external_net, ipxwan->sequence_number);
    }
    if (nidb->nov_ipxwan_link_delay == 0) {
      /* NLSP need to wait for SECOND DELAY packet */
      nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_SLAVE_WAIT_DELAY,
			   "Received First Delay Request");
      nidb->nov_ipxwan_link_delay = 1;	/* Take other branch next time */
      nidb->nov_ipxwan_treq_count = 0;
      COPY_TIMESTAMP(pak->inputtime, nidb->nov_ipxwan_link_delay_timer);
      nov_ipxwan_update_timer(nidb, nidb->nov_ipxwan_treq_int * ONESEC);
    } else {
      nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_SLAVE_WAIT_ROUTER_INFO_REQ,
			   "Received Second Delay Request");
	nidb->nov_ipxwan_link_delay = 0; 
    }
}


/*
 * nov_ipxwan_master_wait_delay_rsp_timer
 *
 * Process IPXWAN MASTER_WAIT_DELAY_RSP timeouts
 */
static void nov_ipxwan_master_wait_delay_rsp_timer (nidbtype *nidb)
{
    paktype *pak;
    ipxwanripsapinfo_hdrtype *ipxwan;
    nov_ipxwan_option_pad *ipxpad;
    int i;

    /* Time to give up? */
    if (nidb->nov_ipxwan_treq_count >= nidb->nov_ipxwan_treq_max) {
	nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_NLSP_DELAY_SENT,
			     "Master Wait Delay timer went off");
	return;
    }
    if (interface_up(nidb->idb)) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Send NLSP DELAY [seq %d] out %s/%x:%x",
		   nidb->nov_ipxwan_treq_count, nidb->idb->namestring,
		   NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
	pak = getbuffer(NOV_IPXWAN_NLSP_DELAY_RSP_PKT_LEN);
	if (!pak) {
	  return;
	}
	ipxwan = (ipxwanripsapinfo_hdrtype *)ipxheadstart(pak);

	pak->linktype = LINK_NOVELL;
	pak->nif_output = nidb; 
	pak->if_output = nidb->idb;
	pak->datagramsize = NOV_IPXWAN_NLSP_DELAY_RSP_PKT_LEN;
	ipxwan->cksum = NOVELL_NO_CHECKSUM;
	ipxwan->len = NOV_IPXWAN_NLSP_DELAY_RSP_PKT_LEN;
	ipxwan->tc = 0;
	ipxwan->pt = NOVELL_TYPE_PEP;
	memset(ipxwan->ddnet, 0, XNS_NETBYTES);
	ieee_copy(baddr, ipxwan->dhost);
	ipxwan->dsock = NOVELL_SOCK_IPXWAN;
	memset(ipxwan->ssnet, 0, XNS_NETBYTES);
	memset(ipxwan->shost, 0, IEEEBYTES);
	ipxwan->ssock = NOVELL_SOCK_IPXWAN;
	PUTLONG(ipxwan->confidence_identifier, NOV_IPXWAN_CONFIDENCE_ID);
	ipxwan->packet_type = NOV_IPXWAN_NLSP_DELAY_REQ_PKT_TYPE;
	PUTLONG(ipxwan->node_id, NOVELL_INTERNAL_ID(nidb));
	ipxwan->sequence_number = nidb->nov_ipxwan_treq_count;
	nidb->nov_ipxwan_treq_expect = nidb->nov_ipxwan_treq_count;
	nidb->nov_ipxwan_treq_count++;
	ipxwan->num_options = 1;
        ipxwan->option_number = NOV_IPXWAN_OPTION_PAD;
	ipxwan->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;

        /* Point to pad area */
        ipxpad = (nov_ipxwan_option_pad *)
          (((uchar *) &(ipxwan->num_options)) + 1);

        /* Padding option to the end of the packet */
        PUTSHORT(ipxpad->option_length,
                   (NOV_IPXWAN_NLSP_DELAY_RSP_PKT_LEN -
                    (NOV_IPXWAN_HDRLEN +
                    NOV_IPXWAN_OPTION_HEADER_SIZE)));
        for (i = 0; i < GETSHORT(ipxpad->option_length); i++) {
	  ipxpad->padding_area[i] = random_gen() & 0xFF;
	}

	nov_send1(pak, ipxwan->dhost, ipxwan->len);
	GET_TIMESTAMP(nidb->nov_ipxwan_link_delay_timer);
    }
    nov_ipxwan_update_timer(nidb, nidb->nov_ipxwan_treq_int * ONESEC);
}

/*
 *
 * nov_ipxwan_process_nlsp_delay_rsp
 *
 * Process NOV_IPXWAN_NLSP_DELAY_RSP_PKT_TYPE IPXWAN packets.
 */
static void nov_ipxwan_process_nlsp_delay_rsp (paktype *pak)
{
    ipxwanhdrtype *ipxwan;
    nidbtype *nidb;
    idbtype *idb;

    ipxwan = (ipxwanhdrtype *)ipxheadstart(pak);
    nidb = pak->nif_input;
    idb = nidb->idb;
    if (novellipxwan_debug) {
	buginf("\nIPXWAN: Rcv DELAY_RSP from %s/%x:%x, NodeID %x, Seq %x",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net, GETLONG(ipxwan->node_id),
	       ipxwan->sequence_number);
    }
    /*
     * Okay. We got a respone. Are we in the right state to receive one?
     * If not, chuck the thing. 
     */
    if (nidb->nov_ipxwan_state <
	NOV_IPXWAN_STATE_NLSP_DELAY_SENT) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Tossing DELAY_RSP on %s/%x:%x in [%s]",
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net,
		   nov_ipxwan_state_to_string(nidb->nov_ipxwan_state));
	}
	retbuffer(pak);
	/* Spec says we should "disconnect on the underlying protocol" */
	nov_ipxwan_reset_nidb(nidb);
	return;
    }
    /*
     * Okay. We got a response. We must be the master. Verify that it is the
     * one we sent.
     */
    if (ipxwan->sequence_number != nidb->nov_ipxwan_treq_expect) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Rcv DELAY_RSP seq %d, want %d, %s/%x:%x",
		   ipxwan->sequence_number, nidb->nov_ipxwan_treq_expect,
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
	retbuffer(pak);
	return;
    }
    if (nidb->nov_ipxwan_link_delay == 0) {
      nidb->nov_ipxwan_link_delay =
      IPXWAN_TIME_DELTA(nidb->nov_ipxwan_link_delay_timer, pak->inputtime) / 2;
      nov_ipxwan_new_state(nidb,
			   NOV_IPXWAN_STATE_NLSP_DELAY_SENT,
			   "Received First Delay Response");
      nidb->nov_ipxwan_treq_expect = ipxwan->sequence_number = 
	nidb->nov_ipxwan_treq_count = 0;
      nov_ipxwan_master_wait_delay_rsp_timer(nidb);
    } else {
      if (novellipxwan_debug)
	buginf("\nIPXWAN: Rcv DELAY_RSP Delay1 %d, Delay2 %d, thru %d",
	       nidb->nov_ipxwan_link_delay * 2,
	       (IPXWAN_TIME_DELTA(nidb->nov_ipxwan_link_delay_timer,
				  pak->inputtime)),
	       nidb->nov_ipxwan_link_throughput);
      nidb->nov_ipxwan_link_delay =
	(((((IPXWAN_TIME_DELTA(nidb->nov_ipxwan_link_delay_timer,
			       pak->inputtime)) /2 )
	   + nidb->nov_ipxwan_link_delay) * 100) 
	 - ((8000 * 576) / nidb->nov_ipxwan_link_throughput));
      nidb->nov_ipxwan_treq_expect = ipxwan->sequence_number = 
	nidb->nov_ipxwan_treq_count = 0;
      nov_ipxwan_new_state(nidb,
			   NOV_IPXWAN_STATE_MASTER_WAIT_ROUTER_INFO_RSP,
			   "Received Second Delay Response");
    }
    retbuffer(pak);
}

/*
 *
 * nov_ipxwan_process_nak
 *
 * Other side didn't like something we just sent and has NAK'd it.!!!!
 * We should probably RESET our current state, but for now just timeout
 * and print debugging info if wanted.
 */
static void nov_ipxwan_process_nak (paktype *pak)
{
  if (novellipxwan_debug)
    buginf("\nIPXWAN: NAK received %s/%x:%x",
	   pak->if_input->namestring,
	   NOVELL_INTERNAL_ID(pak->nif_input),
	   pak->nif_input->nov_ipxwan_external_net);
  /* Spec says we should "disconnect on the underlying protocol" */
}

/*
 *
 * nov_ipxwan_process_ripsap_info_rsp
 *
 * Process NOV_IPXWAN_RIPSAP_INFO_RSP_PKT_TYPE IPXWAN packets.
 *
 */
static void nov_ipxwan_process_ripsap_info_rsp (paktype *pak)
{
    ipxwanhdrtype *ipxwan;
    nidbtype *nidb;
    idbtype *idb;
    nov_ipxwan_option *optptr;
    nov_ipxwan_option_ripsap_info *ripsapoptptr;
    nov_ipxwan_option_nlsp_info *nlspoptptr;
    boolean accepted_routing_type = FALSE;
    boolean ripsap_info_present = FALSE;
    boolean nlsp_info_present = FALSE;
    ulong net_number = 0;
    ushort delay_number = 0;
    ulong nlsp_delay = 0;
    ulong nlsp_throughput = 0;
    leveltype level;
    int curopt, len;
    
    ipxwan = (ipxwanhdrtype *)ipxheadstart(pak);
    nidb = pak->nif_input;
    idb = nidb->idb;
    if (novellipxwan_debug) {
	buginf("\nIPXWAN: Rcv ROUTING_INFO_RSP from %s/%x:%x, NodeID %x, Seq %x",
	       idb->namestring, NOVELL_INTERNAL_ID(nidb),
	       nidb->nov_ipxwan_external_net, GETLONG(ipxwan->node_id),
	       ipxwan->sequence_number);
    }

    /*
     * Okay. We got a request. Are we in the right state to receive one?
     * If not, chuck the thing. 
     */
    if (nidb->nov_ipxwan_state <
	NOV_IPXWAN_STATE_MASTER_WAIT_ROUTER_INFO_RSP) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Tossing ROUTING_INFO_RSP on %s/%x:%x in [%s]",
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net,
		   nov_ipxwan_state_to_string(nidb->nov_ipxwan_state));
	}
	retbuffer(pak);
	/* Spec says we should "disconnect on the underlying protocol" */
	nov_ipxwan_reset_nidb(nidb);
	return;
    }
    /*
     * Okay. We got a response. We must be the master. Verify that it is the
     * one we sent.
     */
    if (ipxwan->sequence_number != nidb->nov_ipxwan_treq_expect) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Rcv ROUTING_RSP seq %d, want %d, %s/%x:%x",
		   ipxwan->sequence_number, nidb->nov_ipxwan_treq_expect,
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
	retbuffer(pak);
	return;
    }
    /*
     * Well, we appear to still be a master, today... Process the packet, and
     * do some work.
     */
    
    /*
     * Handle any options
     */
    for (curopt = 0,
	 optptr = (nov_ipxwan_option *) (&(ipxwan->num_options) + 1);
	 curopt < ipxwan->num_options;
	 curopt++) {
      len = GETSHORT(optptr->option_length);
        switch (optptr->option_number) {
	  case NOV_IPXWAN_OPTION_RIPSAP_INFO:
	    if (len == NOV_IPXWAN_OPTION_RIPSAP_INFO_LENGTH)
	       ripsap_info_present = TRUE;
	    ripsapoptptr = (nov_ipxwan_option_ripsap_info *) optptr;
	    if ((accepted_routing_type == FALSE) &&
		(GETLONG(ripsapoptptr->network_number) ==
		 nidb->nov_ipxwan_external_net) &&
		(optptr->option_accept ==
		 NOV_IPXWAN_OPTION_ACCEPT_YES)) {
		accepted_routing_type = TRUE;
	    }
	    net_number = GETLONG(ripsapoptptr->network_number);
	    delay_number = GETSHORT(ripsapoptptr->link_delay);	    
	    bcopy(ripsapoptptr->router_name,
		  nidb->nov_ipxwan_remote_routername, NOVELL_SERVER_NAME_LEN);

	    /* Don't accept anything if they haven't changed IDs */
	    if (GETLONG(ipxwan->node_id) == NOVELL_INTERNAL_ID(nidb))
	      accepted_routing_type = FALSE;
	    if (strcmp(nidb->nov_ipxwan_remote_routername, 
		       nidb->nov_ipxwan_routername) == 0)
	      accepted_routing_type = FALSE;
	    break;
          case NOV_IPXWAN_OPTION_NLSP_INFO:
            nlspoptptr = (nov_ipxwan_option_nlsp_info *) optptr;
            if (len == NOV_IPXWAN_OPTION_NLSP_INFO_LENGTH) {
              nlsp_info_present = TRUE;
              nlsp_delay = GETLONG(nlspoptptr->delay);
              nlsp_throughput = GETLONG(nlspoptptr->throughput);
              if (nlsp_delay == 0) nlsp_info_present = FALSE;
              if (nlsp_throughput == 0) nlsp_info_present = FALSE;
	      
              /* If we want RIP then we don't want NLSP */
              if (nidb->nov_ipxwan_routing_type !=
                  NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP)
                optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
              if (novellipxwan_debug) {
                buginf("\nIPXWAN: NLSP delay %d, throughput %d", nlsp_delay,
                       nlsp_throughput);
              }
            } else {
              nlsp_info_present = FALSE;
            }
            break;

	  default:
	    if (optptr->option_accept != NOV_IPXWAN_OPTION_ACCEPT_NA) {
		optptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_NO;
	    }
	    break;
	}
	optptr = (nov_ipxwan_option *)
	  (((uchar *) optptr) + NOV_IPXWAN_OPTION_HEADER_SIZE +
	   GETSHORT(optptr->option_length));
    }
    nidb->nov_ipxwan_remote_internal_net = GETLONG(ipxwan->node_id);
    if ((nidb->nov_ipxwan_routing_type ==  NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP)
	&& (!nlsp_info_present)) accepted_routing_type = FALSE;
    if (accepted_routing_type && ripsap_info_present) {
        /* Interrupts must be blocked out so this is an atomic act */
	level = raise_interrupt_level(NETS_DISABLE);
	novell_nidb_set_network_number(nidb, nidb->nov_ipxwan_external_net);
	nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_MASTER_CONNECT,
			     "Received Router Info Rsp as Master");
	nov_network_finish(&novrip_pdb, nidb->novellnet, TRUE);
	reset_interrupt_level(level);
    } else {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Rcv ROUTING_RSP, w/o ack of RIP, %s/%x:%x",
		   idb->namestring, NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
	/* only send NAKs if we aren't doing RIP, compatible with IPXWAN V1 */
	if (nidb->nov_ipxwan_routing_type >
	    NOV_IPXWAN_OPTION_ROUTING_TYPE_RIPSAP) {
	  nov_ipxwan_send_nak(pak);
	} else {
	  nov_ipxwan_reset_nidb(nidb);
	}
      }
    retbuffer(pak);
}


/*
 * Receive IPXWAN packets, and call the appropriate routines to process them.
 */
forktype novell_ipxwan (void)
{
    int count;
    paktype *pak;
    ipxwanhdrtype *ipxwan;

    TIMER_STOP(nov_ipxwan_alarm);
    edisms(systeminitBLOCK, (long)&novell_ipxwanQ);
    process_set_priority(PRIO_LOW);
    while (TRUE) {
	edisms(queueBLOCK, (long)&novell_ipxwanQ);
	count = 10;
	while (TRUE) {
	    if (--count == 0) {
		break;
	    }
	    if (!(pak = dequeue(&novell_ipxwanQ))) {
		break;
	    }
	    if (!novell_running || novell_shutting_down) {
		retbuffer(pak);
		continue;
	    }
	    if (!interface_up(pak->if_input) ||
		(!pak->nif_input->nov_ipxwan_enabled) ||
		(NOV_IPXWAN_IS_DEAF_STATE(pak->nif_input->nov_ipxwan_state))) {
		if (novellipxwan_debug) {
		    buginf("\nIPXWAN: rcv packet/not ready to receive, %s/%x:%x",
			   pak->if_input->namestring,
			   NOVELL_INTERNAL_ID(pak->nif_input),
			   pak->nif_input->nov_ipxwan_external_net);
		}
		retbuffer(pak);
		continue;
	    }

	    ipxwan = (ipxwanhdrtype *)ipxheadstart(pak);

	    /* Tons o'sanity checks */
	    if ((ipxwan->dsock != NOVELL_SOCK_IPXWAN) ||
		(ipxwan->ssock != NOVELL_SOCK_IPXWAN) ||
		(ipxwan->pt != NOVELL_TYPE_PEP) ||
		(GET2WD(ipxwan->ddnet) != 0) ||
		(GET2WD(ipxwan->ssnet) != 0)) {
		if (novellipxwan_debug) {
		    buginf("\nIPXWAN: Bad rcv packet IPX hdr %s/%x:%x",
			   pak->if_input->namestring,
			   NOVELL_INTERNAL_ID(pak->nif_input),
			   pak->nif_input->nov_ipxwan_external_net);
		}
		retbuffer(pak);
		continue;
	    }
	    /* More bucket-loads of sanity checks */
	    if (GETLONG(ipxwan->confidence_identifier) !=
		NOV_IPXWAN_CONFIDENCE_ID) {
	      if (novellipxwan_debug) {
		buginf("\nIPXWAN: Bad rcv packet IPXWAN hdr %s/%x:%x",
		       pak->if_input->namestring,
		       NOVELL_INTERNAL_ID(pak->nif_input),
		       pak->nif_input->nov_ipxwan_external_net);
	      }
	      retbuffer(pak);
	      continue;
	    }
	    /*
	     * Okay. Figure out the packet type, and parcel the work to
	     * the correct routine. THE ROUTINES CALLED BELOW HAVE THE
	     * RESPONSIBILTY OF TOSSING THE FRAME.
	     */
	    switch (ipxwan->packet_type) {
	      case NOV_IPXWAN_TIMER_REQ_PKT_TYPE:
		nov_ipxwan_process_timer_req(pak);
		break;

	      case NOV_IPXWAN_TIMER_RSP_PKT_TYPE:
		nov_ipxwan_process_timer_rsp(pak);
		break;

	      case NOV_IPXWAN_RIPSAP_INFO_REQ_PKT_TYPE:
		nov_ipxwan_process_ripsap_info_req(pak);
		break;

	      case NOV_IPXWAN_RIPSAP_INFO_RSP_PKT_TYPE:
		nov_ipxwan_process_ripsap_info_rsp(pak);
		break;

	      case NOV_IPXWAN_NLSP_THRU_REQ_PKT_TYPE:
		nov_ipxwan_process_nlsp_thru_req(pak);
		break;

	      case NOV_IPXWAN_NLSP_THRU_RSP_PKT_TYPE:
		nov_ipxwan_process_nlsp_thru_rsp(pak);
		break;

	      case NOV_IPXWAN_NLSP_DELAY_REQ_PKT_TYPE:
		nov_ipxwan_process_nlsp_delay_req(pak);
		break;

	      case NOV_IPXWAN_NLSP_DELAY_RSP_PKT_TYPE:
		nov_ipxwan_process_nlsp_delay_rsp(pak);
		break;

	      case NOV_IPXWAN_NAK_PKT_TYPE:
                nov_ipxwan_process_nak(pak);
		retbuffer(pak);
		break;
	      default:
		if (novellipxwan_debug) {
		    buginf("\nIPXWAN: Bad rcv packet IPXWAN type %s/%x:%x",
			   pak->if_input->namestring,
			   NOVELL_INTERNAL_ID(pak->nif_input),
			   pak->nif_input->nov_ipxwan_external_net);
		}
		/* send_nak sends the packet back to the source */
		if (pak->nif_input->nov_ipxwan_routing_type != 0) {
		  nov_ipxwan_send_nak(pak);
		} else {
		  /*
		   * Spec says we should "disconnect on the
		   * underlying protocol"
		   */
		  nov_ipxwan_reset_nidb(pak->nif_input);
		}
		retbuffer(pak);
		break;
	    }
	    (void) process_may_suspend();
	} /* While more packets to process */
    }
}

/*
 * nov_ipxwan_timer_req_timer
 *
 * Process IPXWAN TIMER_REQ timeouts
 */
static void nov_ipxwan_timer_req_timer (nidbtype *nidb)
{
    idbtype *idb;
    paktype *pak;
    ulong i;
    ipxwanhdrtype *ipxwan;
    nov_ipxwan_option_pad *ipxpad;
    nov_ipxwan_option_routing_type *routeoptptr;
    nov_ipxwan_option_ext_node_info *extnodeoptptr;
    nov_ipxwan_option_compression_info *compressionoptptr;
    uchar *nextoptptr;
    boolean compression_enabled;
    uchar compression_type, compression_options, compression_slots;
    ushort pad_option_size, current_option_size;

    idb = nidb->idb;

    if (interface_up(idb)) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Send TIMER_REQ [seq %d] out %s/%x:%x",
		   nidb->nov_ipxwan_treq_count, idb->namestring,
		   NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
	/* If we reached our limit, Disconnect */
	if (nidb->nov_ipxwan_treq_count+1 > nidb->nov_ipxwan_treq_max) {
	  nov_ipxwan_update_timer(nidb, nidb->nov_ipxwan_treq_int * ONESEC);
	  nov_ipxwan_reset_nidb(nidb);
	  return;
	}
	pak = getbuffer(NOV_IPXWAN_TIMER_REQ_PKT_LEN);
	if (!pak) {
	    return;
	}
	ipxwan = (ipxwanhdrtype *)ipxheadstart(pak);

	pak->linktype = LINK_NOVELL;
	pak->if_output = idb;
	pak->nif_output = nidb;
	pak->datagramsize = NOV_IPXWAN_TIMER_REQ_PKT_LEN;
    
	ipxwan->cksum = NOVELL_NO_CHECKSUM;
	ipxwan->len = NOV_IPXWAN_TIMER_REQ_PKT_LEN;
	pad_option_size = NOV_IPXWAN_TIMER_REQ_HDRLEN;
	ipxwan->tc = 0;
	ipxwan->pt = NOVELL_TYPE_PEP;
	memset(ipxwan->ddnet, 0, XNS_NETBYTES);
	ieee_copy(baddr, ipxwan->dhost);
	ipxwan->dsock = NOVELL_SOCK_IPXWAN;
	memset(ipxwan->ssnet, 0, XNS_NETBYTES);
	ieee_zero(ipxwan->shost);
	ipxwan->ssock = NOVELL_SOCK_IPXWAN;
	PUTLONG(ipxwan->confidence_identifier, NOV_IPXWAN_CONFIDENCE_ID);
	ipxwan->packet_type = NOV_IPXWAN_TIMER_REQ_PKT_TYPE;
	PUTLONG(ipxwan->node_id, (nidb->nov_ipxwan_external_net ?
		NOVELL_INTERNAL_ID(nidb) : 0L));
	ipxwan->sequence_number = nidb->nov_ipxwan_treq_count;
	nidb->nov_ipxwan_treq_expect = nidb->nov_ipxwan_treq_count;
	nidb->nov_ipxwan_treq_count =
	  (nidb->nov_ipxwan_treq_count + 1) % nidb->nov_ipxwan_treq_max;
	nidb->nov_ipxwan_routing_type = 0;
	nextoptptr = (uchar *) (&(ipxwan->num_options) +1);
	routeoptptr = (nov_ipxwan_option_routing_type *) nextoptptr;

	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: offer TIMER_REQ routing options");
	}
	/* Offer routing options to use */
	ipxwan->num_options = 0;
	/* Do we want to negotiate NLSP? only if enabled on interface  */
	if ((nidb->nov_lsp_enabled && !nidb->nov_lsp_isis) 
	    && !nidb->nov_ipxwan_static) {
	  routeoptptr = (nov_ipxwan_option_routing_type *) nextoptptr;
	  routeoptptr->routing_type = NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP;
	  routeoptptr->option_number = NOV_IPXWAN_OPTION_ROUTING_TYPE;
	  routeoptptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
	  PUTSHORT(routeoptptr->option_length,
		   NOV_IPXWAN_OPTION_ROUTING_TYPE_LENGTH);
	  current_option_size = NOV_IPXWAN_OPTION_HEADER_SIZE +
	    NOV_IPXWAN_OPTION_ROUTING_TYPE_LENGTH;
	  pad_option_size -= current_option_size;
	  nextoptptr =  (uchar * ) (((uchar *) nextoptptr) +
				    current_option_size);
	  ipxwan->num_options++;
	  if (novellipxwan_debug) {
	    buginf(" NLSP");
	  }
	}
	/* Do we want to try for unnumbered routing */
	if ((nidb->nov_ipxwan_external_net == 0) &&
	    (nidb->lsp_rip_cfg != IPX_RIPSAP_CONFIG_OFF)
	    && !nidb->nov_ipxwan_static) {
	  routeoptptr = (nov_ipxwan_option_routing_type *) nextoptptr;
	  routeoptptr->option_number = NOV_IPXWAN_OPTION_ROUTING_TYPE;
	  routeoptptr->routing_type = NOV_IPXWAN_OPTION_ROUTING_TYPE_UNN_RIP;
	  routeoptptr->option_accept = (nidb->nov_ipxwan_client ?
					NOV_IPXWAN_OPTION_ACCEPT_NO :
					NOV_IPXWAN_OPTION_ACCEPT_YES);
	  PUTSHORT(routeoptptr->option_length,
		   NOV_IPXWAN_OPTION_ROUTING_TYPE_LENGTH);
	  current_option_size = NOV_IPXWAN_OPTION_HEADER_SIZE +
	    NOV_IPXWAN_OPTION_ROUTING_TYPE_LENGTH;
	  pad_option_size -= current_option_size;
	  nextoptptr =  (uchar * ) (((uchar *) nextoptptr) +
				    current_option_size);
	  ipxwan->num_options++;
	  if (novellipxwan_debug) {
	    buginf(" unnumbered-RIP");
	  }
	}
	/* Don't offer RIP if RIP is turned off */
	if ((nidb->lsp_rip_cfg != IPX_RIPSAP_CONFIG_OFF)
	    && !nidb->nov_ipxwan_static &&
	    !(nidb->nov_ipxwan_client &&
	      (nidb->nov_ipxwan_external_net == 0))) {
	  routeoptptr = (nov_ipxwan_option_routing_type *) nextoptptr;
	  routeoptptr->routing_type = NOV_IPXWAN_OPTION_ROUTING_TYPE_RIPSAP;
	  routeoptptr->option_number = NOV_IPXWAN_OPTION_ROUTING_TYPE;
	  routeoptptr->option_accept = (nidb->nov_ipxwan_client ?
					NOV_IPXWAN_OPTION_ACCEPT_NO :
					NOV_IPXWAN_OPTION_ACCEPT_YES);
	  PUTSHORT(routeoptptr->option_length,
		   NOV_IPXWAN_OPTION_ROUTING_TYPE_LENGTH);
	  current_option_size = NOV_IPXWAN_OPTION_HEADER_SIZE +
	    NOV_IPXWAN_OPTION_ROUTING_TYPE_LENGTH;
	  pad_option_size -= current_option_size;
	  nextoptptr =  (uchar * ) (((uchar *) nextoptptr) +
				    current_option_size);
	  ipxwan->num_options++;
	  if (novellipxwan_debug) {
	    buginf(" RIP");
	  }
	}
	/* Client: Only offer if RIP/SAP/NLSP are off for this interface */
	/* End Client may not be officially supported in the first release */
	if (nidb->nov_ipxwan_client && !(nidb->nov_lsp_enabled &&
					 !nidb->nov_lsp_isis) &&
	    !nidb->nov_ipxwan_static) {
	  routeoptptr->routing_type = NOV_IPXWAN_OPTION_ROUTING_TYPE_CLIENT;
	  routeoptptr->option_number =  NOV_IPXWAN_OPTION_ROUTING_TYPE;
	  routeoptptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
	  PUTSHORT(routeoptptr->option_length,
		   NOV_IPXWAN_OPTION_ROUTING_TYPE_LENGTH);
	  routeoptptr = (nov_ipxwan_option_routing_type *)
	    (((uchar *) routeoptptr) + NOV_IPXWAN_OPTION_HEADER_SIZE +
	     GETSHORT(routeoptptr->option_length));
	  ipxwan->num_options++;
	  if (novellipxwan_debug) {
	    buginf(" Client");
	  }
	}
	/* Static: Only offer if RIP/SAP/NLSP are off for this interface */
	if (nidb->nov_ipxwan_static && 
	    !(nidb->nov_lsp_enabled && !nidb->nov_lsp_isis)) {
	  routeoptptr = (nov_ipxwan_option_routing_type *) nextoptptr;
	  routeoptptr->routing_type = NOV_IPXWAN_OPTION_ROUTING_TYPE_STATIC;
	  routeoptptr->option_number =  NOV_IPXWAN_OPTION_ROUTING_TYPE;
	  routeoptptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
	  PUTSHORT(routeoptptr->option_length,
		   NOV_IPXWAN_OPTION_ROUTING_TYPE_LENGTH);
	  current_option_size = NOV_IPXWAN_OPTION_HEADER_SIZE +
	    NOV_IPXWAN_OPTION_ROUTING_TYPE_LENGTH;
	  pad_option_size -= current_option_size;
	  nextoptptr =  (uchar * ) (((uchar *) nextoptptr) +
				    current_option_size);
	  ipxwan->num_options++;
	  if (novellipxwan_debug) {
	    buginf(" Static");
	  }
	}
	/* Check if we need to offer extended node id option
	 * MUST do so when link is unnumbered
	 */
	if (nidb->nov_ipxwan_external_net == 0L) {
	  extnodeoptptr = (nov_ipxwan_option_ext_node_info *) nextoptptr;
	  extnodeoptptr->option_number = NOV_IPXWAN_OPTION_EXT_NODE_INFO;
	  extnodeoptptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
	  PUTLONG(extnodeoptptr->node_id, NOVELL_INTERNAL_ID(nidb));
	  PUTSHORT(extnodeoptptr->option_length,
		   NOV_IPXWAN_OPTION_EXT_NODE_INFO_LENGTH);
	  current_option_size = NOV_IPXWAN_OPTION_HEADER_SIZE +
	    NOV_IPXWAN_OPTION_EXT_NODE_INFO_LENGTH;
	  pad_option_size -= current_option_size;
	  nextoptptr =  (uchar * ) (((uchar *) nextoptptr) +
				    current_option_size);
	  ipxwan->num_options++;
	  if (novellipxwan_debug) {
	    buginf(" Ext-NodeID ");
	  }
	}
	/*
	 * Check if we need to offer compression option.
	 */
	compression_enabled = FALSE;
	if (reg_invoke_ipx_compression_exists()) {
	  reg_invoke_ipx_compression_options_provide(nidb,
						     &compression_enabled,
						     &compression_type,
						     &compression_options,
						     &compression_slots);
	}
	if (compression_enabled) {
	  compressionoptptr = (nov_ipxwan_option_compression_info *) 
	    nextoptptr;
	  compressionoptptr->option_number = NOV_IPXWAN_OPTION_COMPRESSION;
	  compressionoptptr->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
	  compressionoptptr->compression_type = compression_type;
	  compressionoptptr->compression_options = compression_options;
	  compressionoptptr->compression_slots = compression_slots;
	  PUTSHORT(compressionoptptr->option_length,
		   NOV_IPXWAN_OPTION_COMPRESSION_INFO_LENGTH);
	  current_option_size = NOV_IPXWAN_OPTION_HEADER_SIZE +
	    NOV_IPXWAN_OPTION_COMPRESSION_INFO_LENGTH;
	  pad_option_size -= current_option_size;
	  nextoptptr =  (uchar * ) (((uchar *) nextoptptr) +
				    current_option_size);
	  ipxwan->num_options++;
	  if (novellipxwan_debug) {
	    buginf(" Compression ");
	  }
	}
	ipxpad = (nov_ipxwan_option_pad *)  nextoptptr;

	/* Padding option to the end of the packet */
	ipxpad->option_number = NOV_IPXWAN_OPTION_PAD;
	ipxpad->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
	PUTSHORT(ipxpad->option_length,(pad_option_size -
					  NOV_IPXWAN_OPTION_HEADER_SIZE));
	for (i = 0; i < GETSHORT(ipxpad->option_length); i++) {
	    ipxpad->padding_area[i] = random_gen() & 0xFF;
	}
	ipxwan->num_options++; /* padding is counted as an option */
	nov_send1(pak, ipxwan->dhost, ipxwan->len);
	GET_TIMESTAMP(nidb->nov_ipxwan_pak_sent_time);
    }
    nov_ipxwan_update_timer(nidb, nidb->nov_ipxwan_treq_int * ONESEC);
}

/*
 * nov_ipxwan_slave_wait_router_info_req_timer
 *
 * Process IPXWAN SLAVE_WAIT_ROUTER_INFO_REQ timeouts
 */
static void nov_ipxwan_slave_wait_router_info_req_timer (nidbtype *nidb)
{
    /* Time to give up? */
    if (nidb->nov_ipxwan_treq_count > 0) {
	nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_TIMER_REQ_SENT,
			     "Slave Wait Router Info timer went off");
    } else {
	nidb->nov_ipxwan_treq_count++;
	nov_ipxwan_update_timer(nidb, nidb->nov_ipxwan_treq_int * ONESEC);
    }
}

/*
 * nov_ipxwan_master_wait_router_info_rsp_timer
 *
 * Process IPXWAN MASTER_WAIT_ROUTER_INFO_RSP timeouts
 */
static void nov_ipxwan_master_wait_router_info_rsp_timer (nidbtype *nidb)
{
    paktype *pak;
    ipxwanripsapinfo_hdrtype *ipxwan;
    nov_ipxwan_option_nlsp_info *ipxnlsp;
    nov_ipxwan_option_client_info *ipxclnt;

    /* Time to give up? */
    if (nidb->nov_ipxwan_treq_count >= nidb->nov_ipxwan_treq_max) {
	nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_TIMER_REQ_SENT,
			     "Master Wait Router Info timer went off");
	return;
    }
    if (interface_up(nidb->idb)) {
	if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Send ROUTING_INFO_REQ [seq %d] out %s/%x:%x",
		   nidb->nov_ipxwan_treq_count, nidb->idb->namestring,
		   NOVELL_INTERNAL_ID(nidb),
		   nidb->nov_ipxwan_external_net);
	}
	pak = getbuffer(NOV_IPXWAN_RIPSAP_INFO_REQ_PKT_LEN +
			NOV_IPXWAN_OPTION_NLSP_INFO_LENGTH +
			NOV_IPXWAN_OPTION_HEADER_SIZE);
	if (!pak) {
	    return;
	}
	ipxwan = (ipxwanripsapinfo_hdrtype *)ipxheadstart(pak);

	pak->linktype = LINK_NOVELL;
	pak->nif_output = nidb; 
	pak->if_output = nidb->idb;
	ipxwan->cksum = NOVELL_NO_CHECKSUM;
	ipxwan->len = NOV_IPXWAN_RIPSAP_INFO_REQ_PKT_LEN;
	ipxwan->tc = 0;
	ipxwan->pt = NOVELL_TYPE_PEP;
	memset(ipxwan->ddnet, 0, XNS_NETBYTES);
	ieee_copy(baddr, ipxwan->dhost);
	ipxwan->dsock = NOVELL_SOCK_IPXWAN;
	memset(ipxwan->ssnet, 0, XNS_NETBYTES);
	memset(ipxwan->shost, 0, IEEEBYTES);
	ipxwan->ssock = NOVELL_SOCK_IPXWAN;
	PUTLONG(ipxwan->confidence_identifier, NOV_IPXWAN_CONFIDENCE_ID);
	ipxwan->packet_type = NOV_IPXWAN_RIPSAP_INFO_REQ_PKT_TYPE;
	PUTLONG(ipxwan->node_id, NOVELL_INTERNAL_ID(nidb));
	ipxwan->sequence_number = nidb->nov_ipxwan_treq_count;
	nidb->nov_ipxwan_treq_expect = nidb->nov_ipxwan_treq_count;
	nidb->nov_ipxwan_treq_count++;
	ipxwan->num_options = 1;
	ipxwan->option_number = NOV_IPXWAN_OPTION_RIPSAP_INFO;
	ipxwan->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
	PUTSHORT(ipxwan->option_length, NOV_IPXWAN_OPTION_RIPSAP_INFO_LENGTH);
	PUTSHORT(ipxwan->link_delay, (nidb->nov_ipxwan_delay * 55));
	PUTLONG(ipxwan->network_number, nidb->nov_ipxwan_external_net);
	bcopy(nidb->nov_ipxwan_routername, ipxwan->router_name,
	      NOVELL_SERVER_NAME_LEN);

	/* If we have negotiated NLSP we must send more info */
	if (nidb->nov_ipxwan_routing_type ==
	    NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP) {
	  ipxwan->num_options++;
	  ipxwan->len = pak->datagramsize = 
	    NOV_IPXWAN_RIPSAP_INFO_REQ_PKT_LEN + 
	      NOV_IPXWAN_OPTION_NLSP_INFO_LENGTH +
		NOV_IPXWAN_OPTION_HEADER_SIZE;

	  /* point to the end of the packet */
	  ipxnlsp = (nov_ipxwan_option_nlsp_info *) 
	    (((uchar *) (&ipxwan->link_delay)) +
	     NOV_IPXWAN_OPTION_RIPSAP_INFO_LENGTH);

	  ipxnlsp->option_number = NOV_IPXWAN_OPTION_NLSP_INFO;
	    ipxnlsp->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
	  PUTSHORT(ipxnlsp->option_length, NOV_IPXWAN_OPTION_NLSP_INFO_LENGTH);
	  PUTLONG(ipxnlsp->delay, NOVELL_LINK_DELAY(nidb));
	  PUTLONG(ipxnlsp->throughput, NOVELL_LINK_THROUGHPUT(nidb));
	  if (novellipxwan_debug) {
	    buginf("\nIPXWAN: NLSP delay %d, throughput %d",
		   NOVELL_LINK_DELAY(nidb),
		   NOVELL_LINK_THROUGHPUT(nidb));
	  }
	} else if (nidb->nov_ipxwan_routing_type ==
	    NOV_IPXWAN_OPTION_ROUTING_TYPE_CLIENT) {
	  ipxwan->num_options++;
	  ipxwan->len = pak->datagramsize = 
	    NOV_IPXWAN_RIPSAP_INFO_REQ_PKT_LEN + 
	      NOV_IPXWAN_OPTION_CLIENT_INFO_LENGTH +
		NOV_IPXWAN_OPTION_HEADER_SIZE;

	  /* point to the end of the packet */
	  ipxclnt = (nov_ipxwan_option_client_info *) 
	    (((uchar *) (&ipxwan->link_delay)) +
	     NOV_IPXWAN_OPTION_RIPSAP_INFO_LENGTH);

	  ipxclnt->option_number = NOV_IPXWAN_OPTION_CLIENT_INFO;
	  ipxclnt->option_accept = NOV_IPXWAN_OPTION_ACCEPT_YES;
	  PUTSHORT(ipxclnt->option_length, NOV_IPXWAN_OPTION_CLIENT_INFO_LENGTH);
	  ieee_copy(nidb->nov_ipxwan_clnt_address, ipxclnt->mac_address);
	  if (novellipxwan_debug) {
	    buginf("\nIPXWAN: Client address %e assigned",ipxclnt->mac_address);
	  }
	} else {
	  pak->datagramsize = NOV_IPXWAN_RIPSAP_INFO_REQ_PKT_LEN;
	  ipxwan->len = NOV_IPXWAN_RIPSAP_INFO_REQ_PKT_LEN;
	}
	  
	nov_send1(pak, ipxwan->dhost, ipxwan->len);
    }
    nov_ipxwan_update_timer(nidb, nidb->nov_ipxwan_treq_int * ONESEC);
}

/*
 * nov_ipxwan_slave_wait_throughput_req_timer
 *
 * Process IPXWAN SLAVE_WAIT_THROUGHPUT_REQ timeouts
 */
static void nov_ipxwan_slave_wait_throughput_req_timer (nidbtype *nidb)
{
    /* Time to give up? */
    if (nidb->nov_ipxwan_treq_count > 0) {
	nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_TIMER_REQ_SENT,
			     "Slave Wait Throughput timer went off");
    } else {
	nidb->nov_ipxwan_treq_count++;
	nov_ipxwan_update_timer(nidb, nidb->nov_ipxwan_treq_int * ONESEC);
    }
}

/*
 * nov_ipxwan_slave_wait_delay_req_timer
 *
 * Process IPXWAN SLAVE_WAIT_DELAY_REQ timeouts
 */
static void nov_ipxwan_slave_wait_delay_req_timer (nidbtype *nidb)
{
    /* Time to give up? */
    if (nidb->nov_ipxwan_treq_count > 2) {
	nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_TIMER_REQ_SENT,
			     "Slave Wait Delay timer went off");
    } else {
	nidb->nov_ipxwan_treq_count++;
	nov_ipxwan_update_timer(nidb, nidb->nov_ipxwan_treq_int * ONESEC);
    }
}


/*
 * Handle background tasks for IPXWAN. Essentially, send out the various
 * requests that must be sent
 */
static boolean novell_ipxwan_bg_BLOCK (void)
{
    return((!novell_running || novell_shutting_down) ||
	   !TIMER_RUNNING(nov_ipxwan_alarm) ||
	   SLEEPING(nov_ipxwan_alarm));
}

forktype novell_ipxwan_bg (void)
{
    nidbtype *nidb;
    
    edisms(systeminitBLOCK, 0);
    process_set_priority(PRIO_LOW);
    while (TRUE) {
	edisms((blockproc *) novell_ipxwan_bg_BLOCK, (ulong) NULL);
	TIMER_STOP(nov_ipxwan_alarm);
	FOR_ALL_NIDBS(nidb) {
	    /* If interface not enabled for IPXWAN, do nothing */
	    if (!nov_ipxwan_on_nidb(nidb)) {
		continue;
	    }
	    /* This interface is not doing anything? */
	    if (!TIMER_RUNNING(nidb->nov_ipxwan_treq_next)) {
		continue;
	    }
	    /*
	     * This interface is still asleep. It may want to be the
	     * one that triggers the next alarm.
	     */
	    if (SLEEPING(nidb->nov_ipxwan_treq_next)) {
		if (!TIMER_RUNNING(nov_ipxwan_alarm) ||
		    TIMER_LATER(nov_ipxwan_alarm, 
				nidb->nov_ipxwan_treq_next)) {
		    nov_ipxwan_alarm = nidb->nov_ipxwan_treq_next;
		}
		continue;
	    }
	    /* Aha! Found an awake NIDB! Turn the alarm off... */
	    TIMER_STOP(nidb->nov_ipxwan_treq_next);

	    /* Now process the alarm clock according to our current state */
	    switch (nidb->nov_ipxwan_state) {
	      case NOV_IPXWAN_STATE_TIMER_REQ_SENT:
		nov_ipxwan_timer_req_timer(nidb);
		break;

	      case NOV_IPXWAN_STATE_SLAVE_WAIT_ROUTER_INFO_REQ:
		nov_ipxwan_slave_wait_router_info_req_timer(nidb);
		break;

	      case NOV_IPXWAN_STATE_MASTER_WAIT_ROUTER_INFO_RSP:
		nov_ipxwan_master_wait_router_info_rsp_timer(nidb);
		break;

	      case NOV_IPXWAN_STATE_NLSP_THROUGHPUT_SENT:
		nov_ipxwan_master_wait_throughput_rsp_timer(nidb);
		break;

	      case NOV_IPXWAN_STATE_SLAVE_WAIT_THROUGHPUT:
		nov_ipxwan_slave_wait_throughput_req_timer(nidb);
		break;

	      case NOV_IPXWAN_STATE_NLSP_DELAY_SENT:
		nov_ipxwan_master_wait_delay_rsp_timer(nidb);
		break;

	      case NOV_IPXWAN_STATE_SLAVE_WAIT_DELAY:
		nov_ipxwan_slave_wait_delay_req_timer(nidb);
		break;

	      case NOV_IPXWAN_STATE_DISCONNECT:
	      case NOV_IPXWAN_STATE_MASTER_CONNECT:
	      case NOV_IPXWAN_STATE_SLAVE_CONNECT:
	      default:
		break;
	    }
	}
    }
}

/*
 * nov_ipxwan_start_nidb
 * nov_ipxwan_stop_nidb
 *
 * Start and stop IPXWAN on the given interface
 */
void nov_ipxwan_start_nidb (nidbtype *nidb)
{
    nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_TIMER_REQ_SENT,
			 "IPX line state brought up");
}
void nov_ipxwan_stop_nidb (nidbtype *nidb)
{
    nov_ipxwan_new_state(nidb, NOV_IPXWAN_STATE_DISCONNECT,
			 "IPX line state brought down");
}


void novell_ipxwan_enable_ifcommand (boolean interactive, boolean enabling,
				     idbtype *idb, int int1, int int2,
				     char *string1, int int3, int int4,
				     int int5)
{
    nidbtype *nidb, *existing_ipxwan_nidb;
    hwidbtype *hwidb = hwidb_or_null(idb);
    boolean deleted_one;
    char *servernameptr;
    int servernamelen, i;

    if (!novell_running || novell_shutting_down)
	return;

    if (hwidb == NULL)
	return;

    switch (int1) {  /* Which IPXWAN option*/
    case IPX_IPXWAN_STATIC:
      FOR_ALL_NIDBS(nidb) {
	if (nidb->idb == idb) {
	  if (enabling && nidb->nov_ipxwan_client) {
	    if (interactive)
	      printf("%%IPXWAN: may not configure both Static and Client modes");
	    return;
	  }
	  if (enabling && (nidb->nov_lsp_enabled && !nidb->nov_lsp_isis)) {
	    if (interactive)
	      printf("%%IPXWAN: may not configure both NLSP and Client modes");
	    return;
	  }
	  if (enabling && !nidb->nov_ipxwan_enabled) {
	    if (interactive)
	      printf("%%IPXWAN: IPXWAN must be configured before setting Static mode");
	    return;
	  }
	  /* If we change our mode reset the ipxwan link */
	  if (nidb->nov_ipxwan_static != enabling) {
	    nidb->nov_ipxwan_static = enabling;
	    novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
	    novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
	  }
	}
      }
      return;
      break;

    case IPX_IPXWAN_CLIENT:
      FOR_ALL_NIDBS(nidb) {
	if (nidb->idb == idb) {
	  if (enabling && nidb->nov_ipxwan_static) {
	    if (interactive)
	      printf("%%IPXWAN: may not configure both Static and Client modes");
	    return;
	  }
	  if (enabling && (nidb->nov_lsp_enabled && !nidb->nov_lsp_isis)) {
	    if (interactive)
	      printf("%%IPXWAN: may not configure both NLSP and Static modes");
	    return;
	  }
	  if (enabling && !nidb->nov_ipxwan_enabled) {
	    if (interactive)
	      printf("%%IPXWAN: IPXWAN must be configured before setting Client mode");
	    return;
	  }
	  /* If we change our mode reset the ipxwan link */
	  if (nidb->nov_ipxwan_client != enabling) {
	    nidb->nov_ipxwan_client = enabling;
	    novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
	    novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
	  }
	}
      }
      return;
      break;

    case IPX_IPXWAN_ERROR:
      FOR_ALL_NIDBS(nidb) {
	if (nidb->idb == idb) {
	  if (enabling && !nidb->nov_ipxwan_enabled) {
	    if (interactive)
	      printf("%%IPXWAN: IPXWAN must be configured first");
	    return;
	  }
	  if(!enabling) {
	    nidb->nov_ipxwan_error = IPX_IPXWAN_ERROR_CONFIG_RESET;
	  } else {
	    nidb->nov_ipxwan_error = int2;
	  }
	}
      }
      return;
      break;

    case IPX_IPXWAN_DEFAULT:
      break;
    }    

    /* We do not yet think we are changing an existing IPXWAN link */
    existing_ipxwan_nidb = NULL;
    if (!enabling) {
	if (idb->novellnets) {
	    /*
	     * Delete a specified network, or all networks if
	     * none specified. Note that if the primary interface
	     * is specified, all secondary networks are also
	     * deleted.
	     */
	    deleted_one = FALSE;
	    FOR_ALL_NIDBS(nidb) {
		if ((nidb->idb == idb) && (nidb->nov_ipxwan_enabled) &&
		    (((int2 == nidb->nov_ipxwan_internal_net) &&
		    (int3 == nidb->nov_ipxwan_external_net) &&
		    (!strcmp(string1, 
			     (char *) nidb->nov_ipxwan_routername))) ||
		    ((int2 == 0) && (int3 == 0) && (string1[0] == 0)))) {
		    novell_nidb_newstate(nidb, NOVELL_NIDB_DELETE_PENDING);
		    deleted_one = TRUE;
		    nidb->nov_ipxwan_internal_net =
			nidb->nov_ipxwan_external_net = 0;
		    nidb->nov_ipxwan_delay = 0;
		    nidb->idb->nov_ipxwan_enabled = FALSE;
		    nidb->nov_ipxwan_enabled = FALSE;
		    nidb->nov_ipxwan_static = FALSE;
		    nidb->nov_ipxwan_client = FALSE;
		    nidb->nov_ipxwan_error = IPX_IPXWAN_ERROR_CONFIG_RESET;
		}
	    }
	    if (!deleted_one) {
		if (interactive)
		    printf("%%IPXWAN: not configured as such on interface");
	    }
	}
	return;
    }  else {
	if (!is_p2p(idb)) {
	    if (interactive)
		printf("\n%%Novell IPXWAN can only be enabled on Point-to-Point interfaces.");
	    return;
	}
	if (idb->novellnets && !(idb->nov_ipxwan_enabled)) {
	    if (interactive)
		printf("\n%%Remove the IPX network number(s) on interface before enabling IPXWAN");
	    return;
	}
	/*
	 * Is it the same thing as before? If so, just modify
	 * modified timer parameters and return...
	 */
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->idb == idb) {
		if ((int2 == nidb->nov_ipxwan_internal_net) &&
		    (int3 == nidb->nov_ipxwan_external_net) &&
		    (!strcmp(string1,
			     (char *) nidb->nov_ipxwan_routername))) {
		    nidb->nov_ipxwan_treq_int = ((int4 != 0) ?
			int4 : NOV_IPXWAN_TREQ_INT_DEFAULT);
		    nidb->nov_ipxwan_treq_max = ((int5 != 0) ?
			int5 : NOV_IPXWAN_TREQ_MAX_DEFAULT);
		    return;
		} else {
		    /* Okay... the user wants to change their definition */
		    existing_ipxwan_nidb = nidb;
		    break;
		}
	    }
	}
	if ((int2 == 0) && (ipx_internal_net == 0)) {
	    if (interactive)
	      printf("\n%%IPXWAN: The System Internal Network Number must be configured first");
	    return;
	}
	if ((int3 != 0) && (int3 == ipx_internal_net)) {
	    if (interactive)
		printf("\n%%IPXWAN: The System Internal Network and IPXWAN external network numbers cannot be the same");
	    return;
	}

	/* The internal and external network numbers must NOT match */
	if ((int2 != 0) && (int2 == int3)) {
	    if (interactive)
		printf("\n%%IPXWAN: The internal and external network numbers cannot be the same");
	    return;
	}
	/*
	 * The internal net chosen cannot be any other interface's
	 * permanent network number, or anyone else's external IPXWAN
	 * network number
	 */
	if (int2 !=0) {
	  FOR_ALL_NIDBS(nidb) {
	    if (!IPX_NIDB_IS_INTERNAL(nidb) &&
		(nidb->idb != idb) &&
		((nidb->novellnet == int2) ||
		 ((nidb->nov_ipxwan_enabled) &&
		  (nidb->nov_ipxwan_external_net == int2)))) {
	      if (interactive)
		printf("%%IPXWAN: internal network %x already exists on interface %s",			   int2, nidb->idb->namestring);
	      return;
	    }
	  }
	}
	/*
	 * The external net chosen cannot be any other interface's
	 * permanent network number, or anyone else's internal OR
	 * external IPXWAN network number, except for unnumbered... 
	 */
	if (int3 != 0) {
	    FOR_ALL_NIDBS(nidb) {
		if ((nidb->idb != idb) &&
		    ((nidb->novellnet == int3) ||
		     (nidb->nov_ipxwan_internal_net == int3) ||
		     ((nidb->nov_ipxwan_enabled) &&
		      (nidb->nov_ipxwan_external_net == int3)))) {
		    if (interactive)
			printf("%%IPXWAN: external network %x already exists on interface %s",
			       int3, nidb->idb->namestring);
		    return;
		}
	    }
	}
	if (strlen(string1) >= NOVELL_SERVER_NAME_LEN) {
	    if (interactive)
		printf("\n%%Local IPXWAN node name must be no greater than %d characters long",
		       NOVELL_SERVER_NAME_LEN-1);
	    return;
	}
	if (strlen((char *) string1) == 0) {
	  strcpy((char *) string1, ipx_internal_name);
	} else {
	  servernameptr = (char *) string1;
	  servernamelen = strlen(servernameptr);
	  /* Check for legal name */
	  for (i = 0; i < servernamelen; i++) {
	    if ((servernameptr[i] < 'A') ||
		(servernameptr[i] > 'Z')) {
	      if (((servernameptr[i] < '0') ||
		   (servernameptr[i] > '9')) &&
		  ((servernameptr[i] < 'a') ||
		   (servernameptr[i] > 'z')) &&
		  (servernameptr[i] != '@') &&
		  (servernameptr[i] != '_') &&
		  (servernameptr[i] != '-')) {
		if (interactive)
		  printf("\n%%Local IPXWAN node name must contain only alphanumerics or,@,-,_");
		return;
	      }
	    }
	  }
	}
	if (existing_ipxwan_nidb != NULL) {
	    /*
	     * If changing existing NIDB, bring it down, and change
	     * its parameters as if it were a 'new' nidb
	     */
	    nidb = existing_ipxwan_nidb;
	    /* Force world to know it's gone and bring IPXWAN down... */
	    novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
	} else {
	    /* Create one, and set it up */
	    nidb = novell_nidb_new(idb, 0, ET_HDLC, TRUE, FALSE, FALSE, FALSE,
				   FALSE);
	    /*
	     * NLSP not enabled BUT needs to know else link info
	     * field will not not be generated:
	     */
	    reg_invoke_ipx_nidb_network_changed(nidb, IPX_ROUTE_EVENT_ADD);
	}
	/*
	 * Remember if the primary network was given with or
	 * without explicit encapsulation. This information
	 * will be used when the configuration is written to
	 * NVM in order to keep things backwards compatible
	 * with 9.1 and earlier.
	 */
	nidb->old_style = FALSE;
	if (idb->novellnets == 1) {
	    idb->primary_nidb = nidb;
	}
	idb->nov_ipxwan_enabled = TRUE;
	strcpy((char *) nidb->nov_ipxwan_routername, string1);
	/* Null pad the server name */
	for (servernamelen = strlen((char *) nidb->nov_ipxwan_routername);
	     servernamelen < NOVELL_SERVER_NAME_LEN;
	     servernamelen++)
	    nidb->nov_ipxwan_routername[servernamelen] = '\0';
	/* Now, finally set things up */
	nidb->nov_ipxwan_internal_net = int2;
	nidb->nov_ipxwan_external_net = int3;
	nidb->nov_ipxwan_treq_int = (int4 != 0) ? int4 :
	    NOV_IPXWAN_TREQ_INT_DEFAULT;
	nidb->nov_ipxwan_treq_max = (int5 != 0) ? int5 :
	    NOV_IPXWAN_TREQ_MAX_DEFAULT;
	if (nidb->nov_ipxwan_internal_net != 0)
	  bcopy((uchar *) &(nidb->nov_ipxwan_internal_net),
		idb->novell_node, XNS_NETBYTES);
	else
	  bcopy((uchar *) &(ipx_internal_net),
		idb->novell_node, XNS_NETBYTES);
	idb->novell_node[4] = idb->novell_node[5] = 0;
	nidb->nov_ipxwan_clnt_address[0] = 0x02;
	bcopy((uchar *) &(idb->novell_node),
	     (uchar *) &(nidb->nov_ipxwan_clnt_address[1]), XNS_NETBYTES);
        nidb->nov_ipxwan_clnt_address[5] = 0x01;
	nidb->nov_ipxwan_delay = 0;
	nidb->nov_ipxwan_enabled =TRUE;
	if (existing_ipxwan_nidb == NULL)
	  novell_nidb_newstate(nidb, NOVELL_NIDB_NEW);
	novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
	/*
	 * Force a reset of the link if this is a new nidb
	 * and the link is up.
	 * This makes ppp, etc. renegotiate with ipx in mind 
	 * otherwise you get encapsulation failures as ipxcp
	 * hasn't successfully finished.
	 */
	if ((existing_ipxwan_nidb == NULL) && interface_up(idb)) {
	  if (interactive)
	    printf("\n%%IPXWAN Resetting Interface %s", idb->namestring);
	  shutdown_interface(idb, TRUE);
	  process_sleep_for(2 * ONESEC);
	  shutdown_interface(idb, FALSE);
	}
    }
}

/*
 * nov_ipxwan_commands
 * Process Novell ipxwan interface keywords.
 */
void nov_ipxwan_commands (parseinfo *csb)
{
    idbtype *idb;
    hwidbtype *hwidb;
    nidbtype *nidb = NULL;
    boolean all_nets_deleted;

    /*
     * Parse given args if any
     */
    idb = csb->interface;
    hwidb = hwidb_or_null(idb);
    if (!novell_running || novell_shutting_down) {
	if (!csb->nvgen) {
	    printf("\n%%Must give \"ipx routing\" command first");
	}
	return;
    }
    if (csb->nvgen) {
	/* Always bail if IPX not enabled */
	if (!novell_on_idb(idb)) {
	    return;
	}
	/*
	 * Also bail if the network(s) associated with this interface have
	 * been deleted. Note that networks must hang around until all
	 * poisons/reverse poisons have been sent. This could be a very
	 * long time if the internet is large and/or significant output
	 * rip/sap delays have been configured. We do not want to write
	 * deleted network state to nvram while this is happening. On the
	 * other hand, only a subset of networks on an interface may be
	 * deleted. In this case we want to update nvram with everything
	 * except the deleted network(s).
	 */
	all_nets_deleted = TRUE;
	FOR_ALL_NIDBS(nidb) {
	    if ((nidb->idb == idb) && (!NOVELL_NIDB_DELETED_MODE(nidb))) {
		all_nets_deleted = FALSE;
		break;
	    }
	}
	if (all_nets_deleted)
	    return;

	switch (csb->which) {
	  case NOVELL_IPXWAN_ENABLE:
	    FOR_ALL_NIDBS(nidb) {
		if (nidb->idb != idb)
		  continue;
		if (!nov_ipxwan_on_nidb(nidb))
		  continue;
		nv_write(TRUE, "%s %x ", csb->nv_command,
			 nidb->nov_ipxwan_internal_net);
		if (nidb->nov_ipxwan_external_net != 0) {
		    nv_add(TRUE, "%x ", nidb->nov_ipxwan_external_net);
		} else {
		    nv_add(TRUE, "unnumbered ");
		}
	        nv_add(TRUE, "%s", nidb->nov_ipxwan_routername);
		if ((nidb->nov_ipxwan_treq_int !=
		     NOV_IPXWAN_TREQ_INT_DEFAULT) ||
		    (nidb->nov_ipxwan_treq_max !=
		     NOV_IPXWAN_TREQ_MAX_DEFAULT)) {
		  nv_add(TRUE, " %u", nidb->nov_ipxwan_treq_int);
		  nv_add((nidb->nov_ipxwan_treq_max !=
			  NOV_IPXWAN_TREQ_MAX_DEFAULT), " %u",
			 nidb->nov_ipxwan_treq_max);
		}
		nv_write(nidb->nov_ipxwan_client,"%s client", csb->nv_command);
		nv_write(nidb->nov_ipxwan_static,"%s static", csb->nv_command);
		if (nidb->nov_ipxwan_error != IPX_IPXWAN_ERROR_CONFIG_RESET) {
		  nv_write(TRUE,"%s error",
			   csb->nv_command);
		  if (nidb->nov_ipxwan_error == IPX_IPXWAN_ERROR_CONFIG_NOOP) {
		    nv_add(TRUE," resume");
		  } else {
		    nv_add(TRUE," shutdown");
		  }
		}
	    }
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case NOVELL_IPXWAN_ENABLE:
	novell_ipxwan_enable_ifcommand(TRUE, csb->sense, idb, GETOBJ(int, 1),
				       GETOBJ(int, 2), (char *)
				       GETOBJ(string, 1), GETOBJ(int, 3),
				       GETOBJ(int, 4), GETOBJ(int, 5));
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * novell_ipxwan_bg_start
 */
void novell_ipxwan_bg_start (void)
{
    novell_ipxwanbg_pid = cfork(novell_ipxwan_bg, 0L, 0, "IPXWAN Timer", 0);
    if (novell_ipxwanbg_pid == NO_PROCESS)
	novell_ipxwanbg_pid = 0;
}


/*
 * novell_ipxwan_start
 */
void novell_ipxwan_start (void)
{
    novell_ipxwan_pid = cfork(novell_ipxwan, 0L, 0, "IPXWAN Input", 0);
    if (novell_ipxwan_pid == NO_PROCESS)
	novell_ipxwan_pid = 0;
}

extern void ipxwan_parser_init(void);

/*
 * ipxwan_init
 *
 * Initialize novell ipxwan support
 */
static void ipxwan_init (subsystype *subsys)
{
    ipxwan_enabled = TRUE;	/* Give IPX a quick way of finding this */
    
    reg_add_novell_ipxwan_bg_start(novell_ipxwan_bg_start,
				   "novell_ipxwan_bg_start");
    reg_add_novell_ipxwan_start(novell_ipxwan_start, "novell_ipxwan_start");
    reg_add_nov_ipxwan_start_nidb(nov_ipxwan_start_nidb, 
				  "nov_ipxwan_start_nidb");
    reg_add_nov_ipxwan_stop_nidb(nov_ipxwan_stop_nidb, "nov_ipxwan_stop_nidb");
    reg_add_nov_ipxwan_display(nov_ipxwan_display, "nov_ipxwan_display");
    ipxwan_parser_init();
}

/*
 * IPXWAN subsystem header
 */
#define IPXWAN_MAJVERSION	1
#define IPXWAN_MINVERSION	0
#define IPXWAN_EDITVERSION	1

SUBSYS_HEADER(ipxwan, IPXWAN_MAJVERSION, IPXWAN_MINVERSION, 
	      IPXWAN_EDITVERSION,
	      ipxwan_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: ipx", "req: ipx");
