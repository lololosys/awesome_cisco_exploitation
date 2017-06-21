/* $Id: dhcp.c,v 3.8.8.8 1996/08/28 16:18:20 fox Exp $
 * $Source: /release/112/cvs/Xsys/ip/dhcp.c,v $
 *------------------------------------------------------------------
 * dhcp.c - DHCP protocol (proxy client implementation)
 *
 * Mar 10, 1994, Syed Irfan Ashraf
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dhcp.c,v $
 * Revision 3.8.8.8  1996/08/28  16:18:20  fox
 * CSCdi67334:  Proxy Client process already running message is unnecessary
 * Branch: California_branch
 *
 * Revision 3.8.8.7  1996/08/07  09:00:10  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.8.8.6  1996/07/19  22:55:10  irfan
 * CSCdi63267:  dhcp hickups
 * Branch: California_branch
 * misc. dhcp proxy client improvements:
 *  - appear to the server as a relay agent to get unicasts back correctly.
 *  - tap packets from bootp server to see if they are responses directed
 *    to the dhcp proxy client (relay-agent).
 *  - make it work for lines that don't have a logged-in user.
 *  - reduce the sleep interval in allocation attempts.
 *  - use client ID strings beginning with a byte of 0.
 *  - rename dhcp_sock.
 *  - make allocation work if prev. request is in selecting/requesting
 *    states.
 *  - make allocation work if prev. lease entry is being purged.
 *  - should use new process primitives. also block on crashes.
 *  - add more debug information.
 *
 * Revision 3.8.8.5  1996/07/05  01:41:52  irfan
 * CSCdi62121:  some dhcp debug messages not turned off by debug dhcp
 * Branch: California_branch
 *
 * Revision 3.8.8.4  1996/06/22  02:07:09  irfan
 * CSCdi55476:  DHCP does not work well with larrrge lease values
 * Branch: California_branch
 * set an upper limit for max lease time.
 *
 * Revision 3.8.8.3  1996/06/21  23:48:40  irfan
 * CSCdi60979:  dhcp proxy-client should release leases before reload
 *              release all assigned leases when gracefully reloading.
 * CSCdi60953:  dhcp should be more forgiving when validating packets.
 * CSCdi52819:  dhcp proxy client should support unicast messages.
 *
 * Branch: California_branch
 *
 * Revision 3.8.8.2  1996/06/17  23:31:56  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.8.8.1  1996/03/18  20:15:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.4  1996/03/16  06:57:08  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.3  1996/03/13  01:37:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.2.2  1996/03/07  09:45:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  00:58:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/28  09:32:59  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.7  1996/02/22  14:34:20  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/01/29  07:28:26  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/24  20:14:43  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4  1996/01/19  00:57:12  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/19  00:40:40  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.2  1995/11/17  09:32:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/10/25  16:25:23  vandys
 * CSCdi42511:  DHCP fumbles if IPCP does multiple rounds of CONFREQ
 * Debounce free/alloc to avoid spinning the DHCP
 * state machine on quick-turnaround transactions.
 *
 * Revision 2.11  1995/10/20  18:34:47  vandys
 * CSCdi42185:  DHCP crashes in lease queue code
 * Don't use stale ->next pointer
 *
 * Revision 2.10  1995/09/18  19:35:25  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.9  1995/09/13  04:07:11  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.8  1995/08/03 07:41:57  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.7  1995/06/30 10:10:41  irfan
 * CSCdi36611:  cleanup of sanity checks for remote SLIP/PPP parameters
 * Don't allow SLIP w/o required IP configuration. Same for PPP, but
 * don't block possible IPX or ATALK PPP clients.
 *
 * Revision 2.6  1995/06/30  05:21:03  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.5  1995/06/29 20:44:24  irfan
 * CSCdi36117:  DHCP debug message is misleading
 * spit the message only for interfaces actually using ip pooling.
 *
 * Revision 2.4  1995/06/28  09:26:40  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/19  01:06:19  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:05:19  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:56:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "packet.h"
#include "ieee.h"
#include "logger.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ip/dhcp.h"
#include "../ip/ip_registry.h"
#include "../ip/ipaddress.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ui/debug.h"
#include "../ip/dhcp_debug.h"
#include "sys_registry.h"
#include "../util/random.h"
#include "../ip/ip_addrpool.h"

/*
 * Function prototypes for functions in this module
 */


dhcp_server_type *find_dhcps_entry(ipaddrtype);
dhcp_server_type *add_dhcps_entry(ipaddrtype);
static void zero_dhcps_stats(dhcp_server_stats *);
static void init_dhcps_entry(dhcp_server_type *);

static iplease_type *find_dhcpc_lease_xid(ulong);
static iplease_type *find_dhcpc_lease_idb(idbtype *);
static iplease_type *find_dhcpc_lease_ipl(iplease_type *);
static iplease_type *find_dhcpc_lease_addr(ipaddrtype);
static iplease_type *add_lease_entry(ipaddrtype, idbtype *, uchar *);
static void remove_lease_entry(iplease_type *);

static void test_allocate(void);
static void test_release(ipaddrtype);
static void test_renewal(ipaddrtype);
void test_dhcp_command(parseinfo *);

static void dhcpc_death_cleanup(void);
static sys_timestamp dhcpc_scan_leaseQ(void);
static void dhcpc_process_inpak(paktype *, udptype *, dhcptype *);
void dump_dhcp_pkt(udptype *, dhcptype *, dhcp_values *);
static int dhcpc_scan_options(paktype *, udptype *, dhcptype *, dhcp_values *);
static int dhcpc_read_options (dhcp_values *, uchar *, uchar *);
static void dhcp_rcv_offer(dhcp_values *, iplease_type *);
static void dhcp_rcv_ack(dhcp_values *, iplease_type *);
static void dhcp_rcv_nak(dhcp_values *, iplease_type *);
static boolean dhcpc_check_server(ipaddrtype);
static boolean dhcp_verify_net_addr(ipaddrtype);
static void dhcpc_record_lease(dhcp_values *, iplease_type *);
static void dhcpc_addr_use(iplease_type *);
static void dhcpc_addr_purge(iplease_type *);
static boolean dhcpc_evaluate_offer(dhcp_values *, iplease_type *);
static void dhcp_select_offer(dhcp_values *, iplease_type *);
static void dhcp_new_state(iplease_type *, uint);

static void dhcp_client_init(subsystype *);
static forktype dhcp_px_client(void);
static boolean dhcpcBLOCK(ipsocktype *);

static void dhcp_pool_startup(void);
static void dhcp_pool_shutdown(void);
static void dhcp_pool_show(void);
static int dhcp_allocate_request(ipaddrtype *, idbtype *,
				 uchar *, uchar *, ulong);

static void dhcp_deallocate_request(ipaddrtype, idbtype *);
static void dhcp_chown_request(ipaddrtype, idbtype *);
static int dhcp_wait_allocation(iplease_type *, ulong);

/*
 * External functions that lack prototypes.
 */
extern boolean configuredBLOCK(void);  /* os/boot.c */

/*
 * static variables
 */
/*
 * DHCP client process variables
 */
static volatile int dhcpc_pid = 0;      /* DHCP client process id */
static volatile uint dhcpc_status = 0;  /* DHCP client process status word */
static sys_timestamp dhcp_next_invoke;  /* DHCP client wake-up time */
ipsocktype *dhcp_client_soc;            /* DHCP client's IP socket */


/*
 * IP address pooling table (address of which is passed to address
 *			     pooling logic thru a registry call)
 */
static ip_addresspool_type ippool_dhcp = \
       {PEER_ADDR_DHCP, "DHCP Proxy Client", dhcp_pool_startup,
	dhcp_pool_shutdown, dhcp_pool_show,
	dhcp_allocate_request, dhcp_deallocate_request,
	dhcp_chown_request};

/*
 * Table of all known DHCP servers
 */
dhcp_server_type dhcps_table[MAX_DHCP_SERVERS];

/*
 * Current number of DHCP servers known
 */
uint total_dhcpservers;

/*
 * Current DHCP lease list.
 *     Approved leases, as well as in request leases
 */
queuetype ip_leaseQ;	  /* Queue of current active lease entries */

/*
 * constant strings
 */
const char *const DHCPM_strings[] = {"Unknown", "Discover", "Offer", "Request",
				   "Decline", "Ack", "Nak", "Release"};

const char *const DHCPC_strings[] = {"Initial", "Selecting",
				   "Requesting", "Bound", "Rebinding",
				   "Renewing", "Releasing", "Holddown",
				   "Purging" };

/*
 * some test variables
 */
static ipaddrtype test_lastallocaddr;   /* used as default by test release
				         * driver */
/*
 * pool vector functions
 */

static void dhcp_pool_startup (void)
{

    if (dhcpc_pid > 0) {
	return;
    }

    /*
     * Spawn the DHCP client process
     */
    dhcpc_pid = process_create(dhcp_px_client, "DHCP Pxy Client",
			       NORMAL_STACK, PRIO_NORMAL);

    if (dhcpc_pid == NO_PROCESS)       /* any problems ? */
	return;

    process_set_crashblock(dhcpc_pid, TRUE);
    dhcpc_status = DHCPC_STATUS_RUN;
}

static void dhcp_pool_shutdown (void)
{
    if (dhcpc_pid <=0) {
	return;
    }
    dhcpc_status &= ~DHCPC_STATUS_RUN;
    dhcpc_status |= DHCPC_STATUS_INVOKE;
    if (dhcp_debug) {
	buginf("\n%%Shutting down DHCP");
    }
}

static void dhcp_pool_show (void)
{
    printf("\nDHCP Proxy Client Status:");
    show_dhcp_servers();
}


/*
 * find the lease entry in the queue
 *    given the xid
 */
static iplease_type *find_dhcpc_lease_xid (ulong search_xid)
{
    iplease_type *ipl = (iplease_type *) ip_leaseQ.qhead;

    while (ipl)
	if (ipl->my_xid == search_xid)
	    break;
	else
	    ipl = ipl->next;
    return ipl;
}

/*
 * find the lease entry in the queue
 *    given the user string
 */
static iplease_type *find_dhcpc_lease_ustring (uchar *ustring)
{
    iplease_type *ipl = (iplease_type *) ip_leaseQ.qhead;

    while (ipl)
	if (!(strcmp(ipl->client_id, ustring)))
	    break;
	else
	    ipl = ipl->next;
    return ipl;
}

/*
 * find the lease entry in the queue
 *    given the idb
 */
static iplease_type *find_dhcpc_lease_idb (idbtype *idb)
{
    iplease_type *ipl = (iplease_type *) ip_leaseQ.qhead;

    while (ipl)
	if (ipl->idb == idb)
	    break;
	else
	    ipl = ipl->next;
    return ipl;
}

/*
 * find the lease entry in the queue
 *    given the ipl
 */
static iplease_type *find_dhcpc_lease_ipl (iplease_type *ipl2find)
{
    iplease_type *ipl = (iplease_type *) ip_leaseQ.qhead;

    while (ipl)
	if (ipl == ipl2find)
	    break;
	else
	    ipl = ipl->next;
    return ipl;
}

/*
 * find the lease entry in the queue
 *    given the address
 */
static iplease_type *find_dhcpc_lease_addr (ipaddrtype search_addr)
{
    iplease_type *ipl = (iplease_type *) ip_leaseQ.qhead;

    while (ipl)
	if (ipl->client_addr == search_addr)
	    break;
	else
	    ipl = ipl->next;
    return ipl;
}

static void remove_lease_entry (iplease_type *ipl)
{
    SAVE_CALLER();

    if (!ipl) {
	if (dhcp_debug) {
	    buginf("\nDHCP: Null entry deletion requested from %X", caller());
	}
	return;
    }
    if (dhcp_debug) {
	buginf("\nDHCP: deleting entry %x %i from list",
	       ipl, ipl->client_addr);
    }
    if (dhcp_debug) {
	print_lease_entry(TRUE, ipl);
    }
    if (ipl == find_dhcpc_lease_ipl(ipl)) {
	unqueue(&ip_leaseQ, ipl);
	free(ipl);
    } else {
	if (dhcp_debug) {
	    buginf("\nDHCP: entry for deletion not in list");
	}
    }
    dhcpc_status |= DHCPC_STATUS_INVOKE;
}

static iplease_type *add_lease_entry (ipaddrtype request_addr,
				      idbtype *req_idb, uchar *client_id)
{

    iplease_type *ipl;

    ipl = malloc_named(sizeof(iplease_type), "add_lease_entry");
    if (!ipl) {
	return NULL;
    }
    dhcp_new_state(ipl, DHC_INITIAL);
    ipl->request_addr = request_addr;
    ipl->client_addr = IPADDR_ZERO;
    ipl->idb = req_idb;
    ipl->next = NULL;
    ipl->my_xid = random_gen();    /* generate a random xid */
    sstrncpy(ipl->client_id, client_id, DHCP_MAX_STR);

    /* now add this to list */
    enqueue(&ip_leaseQ, ipl);
    dhcpc_status |= DHCPC_STATUS_INVOKE;
    return ipl;
}


/*
 * dhcp_allocate_request
 *
 * attempt to provide a lease
 * Note that the test logic will pass an IDB address of zero!
 */
static int dhcp_allocate_request (ipaddrtype *request_addr,
				  idbtype *idb, uchar *ustring,
				  uchar *poolname, ulong wait_ticks)
{
    iplease_type *ipl = NULL;
    int ret_code;
    uchar client_id_string[DHCP_MAX_STR+1];  /* Our Client Id string */

    if (dhcp_debug) {
	buginf("\nDHCP: allocate request");
    }

    /* check to see if we already have this request ? */
    if (idb)
        ipl = find_dhcpc_lease_idb(idb);
    if (!ipl && ustring && *ustring) {
	ipl = find_dhcpc_lease_ustring(ustring);
    }
    if (ipl) {             /* we do have this request */

	switch (ipl->state) {

	case DHC_SELECTING:
	case DHC_REQUESTING:
	case DHC_INITIAL:
	    return (dhcp_wait_allocation(ipl, wait_ticks));

	case DHC_PURGING:
	    /*
	     * remove reference to idb/ustring
	     */
	    if (dhcp_debug) {
		buginf("\nDHCP: zapping entry in DHC_PURGING state for %s",
		       (idb)?idb->short_namestring:"");
	    }
	    ipl->idb = NULL;
	    ipl->client_id[0] = '\0';
	    ipl = NULL;    /* let the code fall through for new entries */
	    break;

	case DHC_HOLDDOWN:
	    if (dhcp_debug) {
		buginf("\nDHCP: reclaimed %i for %s",
		    ipl->client_addr, idb->namestring);
	    }
	    dhcp_new_state(ipl, DHC_BOUND);
	    /* VVV fall into VVV */

	case DHC_REBINDING:
	case DHC_RENEWING:
	case DHC_BOUND:
	    *request_addr = ipl->client_addr;   /* fill in the allocated */
	    if (idb && idb->ip_peer) {
		ippeerinfotype *ip_peer = idb->ip_peer;
		ip_peer->ippeer_addr = ipl->client_addr;      /*   addr */
		ip_peer->ippeer_addr_source = PEER_ADDR_FROM_DHCP;
	    }
	    return POOL_RET_OK;

	default:
	    if (dhcp_debug) {
		buginf("\nDHCP: unknown DHCP ipl state: %u", ipl->state);
	    }
	    return POOL_RET_FAIL;
	}
    }

    /*
     * we don't have this request in list.
     *    add it to the list and send a discover
     */
    if (dhcp_debug) {
	buginf("\nDHCP: new entry. add to queue");
    }
    if (ustring && (*ustring)) {
	ipl = add_lease_entry(*request_addr, idb, ustring);
    } else {
	sprintf(client_id_string, "cisco-%i-%s",
		ip_ouraddr(hwidb_or_null((idb) ? idb : SWIDBQ_HEAD)),
		(idb)?idb->namestring:"");
	ipl = add_lease_entry(*request_addr, idb, client_id_string);
    }
    if (!ipl) {
	if (dhcp_debug) {
	    buginf("\nDHCP: new entry. failed to add to queue");
	}
	return POOL_RET_FAIL;           /* something went wrong */
    }

    dhcp_new_state(ipl, DHC_SELECTING);
    dhcp_send_discover(ipl);         /* send discover */
    ret_code = dhcp_wait_allocation(ipl, wait_ticks) ;
    switch (ret_code) {

      case POOL_RET_OK:
	*request_addr = ipl->client_addr; /* fill in the allocated */
	break;

      case POOL_RET_PENDING:
	break;

      case POOL_RET_FAIL:
      default:
	ret_code = POOL_RET_FAIL;
	break;
    }
    return (ret_code);
}

static int dhcp_wait_allocation (iplease_type *ipl, ulong wait_ticks)
{

    ulong my_ticks;

    if (wait_ticks) {
	for (my_ticks = 0; my_ticks < wait_ticks;
	     my_ticks += DHCP_SLEEP_SMALL) {

	    process_sleep_for(DHCP_SLEEP_SMALL);
	    ipl = find_dhcpc_lease_ipl(ipl);
	    if (!ipl) {
		return POOL_RET_FAIL;
	    }
	    if (ipl->state == DHC_BOUND) {
		return POOL_RET_OK;
	    }
	}
	dhcp_new_state(ipl, DHC_PURGING);
	return POOL_RET_FAIL;
    }
    return POOL_RET_PENDING;            /* return ret_code */
}

/*
 * dhcp_deallocate_request
 *
 * See if we got this address leased from somebody.  If so, start
 * the process of releasing it.  Because some clients will do multiple
 * bounces of request-free-request-free-..., we debounce a free request
 * on a valid address.  Otherwise, we immediately kick off the cleanup.
 *
 */
static void dhcp_deallocate_request (ipaddrtype idb_ip_addr, idbtype *idb)
{
    iplease_type *ipl;

    ipl = find_dhcpc_lease_addr(idb_ip_addr);
    if (ipl && (DHC_RELEASING > ipl->state)) {
	if (dhcp_debug) {
	    buginf("\nDHCP: Interface %s going down. Releasing: %i",
		   (idb) ? idb->namestring : "", idb_ip_addr);
	}
	if (ipl->state == DHC_BOUND) {
	    if (dhcp_debug) {
		buginf("\nDHCP: start holddown for %i", idb_ip_addr);
	    }
	    dhcp_new_state(ipl, DHC_HOLDDOWN);
	} else {
	    dhcp_new_state(ipl, DHC_RELEASING);
	    dhcp_send_release(ipl);
	}
	dhcpc_status |= DHCPC_STATUS_INVOKE;
    }
}


/*
 * Change ownership of a DHCP provided address
 */

static void dhcp_chown_request (ipaddrtype ipaddr, idbtype *idb)
{
    iplease_type *ipl = find_dhcpc_lease_addr(ipaddr);

    if (ipl)
	ipl->idb = idb;
}


/*
 * some test routines to drive dhcp protocol
 */
static void test_renewal (ipaddrtype addr)
{

    iplease_type *ipl;

    ipl = find_dhcpc_lease_addr(addr);
    if (!ipl) {
	return;
    }

    if (dhcp_debug) {
	buginf("\nDHCP: Attempting to speed up renewal timer");
    }

    if (DHC_BOUND == ipl->state) {
	GET_TIMESTAMP(ipl->state_timer);
	TIMER_UPDATE(ipl->state_timer, 4 * ONESEC);
    } else {
	printf("\nNot in Bound state. Can't test Renewal");
    }

    return;
}

/*
 * No one should touch the state word directly
 *    except this routine. Let's keep it separate
 */
static void dhcp_new_state (iplease_type *ipl, uint new_state)
{
    SAVE_CALLER();

    switch (new_state) {

    case DHC_INITIAL:
	break;

    case DHC_BOUND:
	TIMER_START(ipl->state_timer, ipl->lease_secs * ONESEC);
	break;

    case DHC_RENEWING:
	TIMER_START(ipl->state_timer, ipl->t1_secs * ONESEC);
	break;

    case DHC_REBINDING:
	TIMER_START(ipl->state_timer, ipl->t2_secs * ONESEC);
	break;

    case DHC_SELECTING:
	TIMER_START(ipl->state_timer, DHCP_OFFER_WAIT_SECS * ONESEC);
	break;

    case DHC_REQUESTING:
	TIMER_START(ipl->state_timer, DHCP_ACK_WAIT_SECS * ONESEC);
	break;

    case DHC_HOLDDOWN:
	TIMER_START(ipl->state_timer, DHCP_HOLD_WAIT_SECS * ONESEC);
	break;

    case DHC_RELEASING:
	TIMER_START(ipl->state_timer, DHCP_RELEASE_WAIT_SECS * ONESEC);
	break;

    case DHC_PURGING:
	TIMER_START(ipl->state_timer, DHCP_PURGE_WAIT_SECS * ONESEC);
	break;

    default:
	if (dhcp_debug) {
	    buginf("\nDHCP: Change state to %X invalid!!  from %X",
		   new_state, caller());
	}
	return;
    }

    if (ipl->state != new_state) {
	ipl->retry_count = 0;
    }
    ipl->state = new_state;
}

/*
 * test the dhcp client mechanisms
 */
static void test_allocate (void)
{
    int count = 60;
    ipaddrtype new_addr = 0;
    int status;
    uchar my_test_string[DHCP_MAX_STR+1];

    sprintf(my_test_string, "cisco-test-%-2u", random_gen());
    status = dhcp_allocate_request(&new_addr, NULL,
				   my_test_string, NULL, 2*ONESEC);
    if (POOL_RET_FAIL == status) {
	printf("\n%%Unknown DHCP problem. No allocation possible");
	return;
    }
    process_sleep_for(ONESEC);
    psion(stdio);
    for (;count;count--) {

	status = dhcp_allocate_request(&new_addr, NULL,
				       my_test_string, NULL, 2*ONESEC);
	switch (status) {

	case POOL_RET_FAIL:
	    psioff(stdio);
	    printf("\n%%Unknown DHCP problem.. No allocation possible");
	    return;

	case POOL_RET_OK:
	    psioff(stdio);
	    printf("\nAllocated IP address = %i", new_addr);
	    return;

	case POOL_RET_PENDING:
	    process_sleep_for(ONESEC);
	    if (psipending()) {
		psioff(stdio);
		printf("\n%%....Aborted");
		return;
	    }
	    break;

	default:
	    psioff(stdio);
	    printf("\nDHCP problem...Aborting");
	    return;
	}
    }
    psioff(stdio);
}

static void test_release (ipaddrtype addr)
{
    if (addr)
	dhcp_deallocate_request(addr, 0);
    else
	dhcp_deallocate_request(test_lastallocaddr, 0);
}

void test_dhcp_command (parseinfo *csb)
{

    int test_what = GETOBJ(int, 1);

    if (dhcpc_pid <= 0) {
	printf("\nDHCP not running");
	return;
    }
    switch(test_what) {

    case 1:
	test_allocate();
	break;

    case 2:
	test_release(GETOBJ(paddr,1)->ip_addr);
	break;

    case 3:
	test_renewal(GETOBJ(paddr,1)->ip_addr);
	break;

    default:
	break;
    }
}

/*
 * Some utility routines
 */

static void zero_dhcps_stats (dhcp_server_stats *dserv)
{
    dserv->leases = dserv->offers = dserv->requests = dserv->acks = 0;
    dserv->naks = dserv->declines = dserv->releases = 0;
    dserv->bad_types = dserv->out_of_seq = 0;
}

dhcp_server_type *find_dhcps_entry (ipaddrtype find_addr)
{
    dhcp_server_type *dserv = &dhcps_table[0];
    uint count;

    for (count = 0; count < MAX_DHCP_SERVERS; dserv++, count++) {
	if (dserv->flags & DHCPS_VALID)
	    if (dserv->addr == find_addr)
		return dserv;
    }
    return NULL;
}

dhcp_server_type *add_dhcps_entry (ipaddrtype add_addr)
{

    dhcp_server_type *dserv = &dhcps_table[0];
    uint count;

    if (total_dhcpservers >= MAX_DHCP_SERVERS)
	return NULL;
    if (NULL != find_dhcps_entry(add_addr))
	return NULL;

    for (count = 0; count < MAX_DHCP_SERVERS; dserv++, count++) {
	if (!(dserv->flags & DHCPS_VALID)) {
	    dserv->addr = add_addr;
	    dserv->flags = DHCPS_VALID;
	    zero_dhcps_stats(&dserv->stats);
	    if (DHCP_default_server == add_addr) {
		dserv->flags |= DHCPS_DEFAULT;
	    }
	    total_dhcpservers++;
	    return dserv;
	}
    }
    return NULL;
}

static void init_dhcps_entry (dhcp_server_type *dserv)
{
    dserv->flags = 0;
    dserv->addr = (ipaddrtype) 0;
    zero_dhcps_stats(&dserv->stats);
}

void init_dhcps_table (void)
{
    dhcp_server_type *dserv = &dhcps_table[0];
    uint count;

    for (count = 0; count < MAX_DHCP_SERVERS; dserv++, count++)
	init_dhcps_entry(dserv);
    total_dhcpservers = 0;
}

static boolean dhcpcBLOCK (ipsocktype *soc)
{
    /*
     * return false if someone wants us to run
     *              if there is some input on the socket
     *              if our scan timer has fired
     */
    if ((dhcpc_status & DHCPC_STATUS_INVOKE) ||
	!process_is_queue_empty(soc->inq) ||
	((TIMER_RUNNING(dhcp_next_invoke)) && (AWAKE(dhcp_next_invoke)))) {

	dhcpc_status &= ~DHCPC_STATUS_INVOKE;
	return FALSE;
    }
    return TRUE;
}

static forktype dhcp_px_client (void)
{

    int dhcp_pak_count;
    paktype *pak;
    udptype *udp;
    dhcptype *dhcp;


    process_wait_on_system_config();

    /*
     * Open a client socket, die quietly if we fail.
     */
    dhcp_client_soc = open_ipsocket(UDP_PROT, NULL, NULL,
				    DHCPC_PORT, 0, SOCK_ANYSRC);
    if (!dhcp_client_soc) {
	if (dhcp_debug) {
	    buginf("\nDHCP: unable to open ipsocket...dying");
	}
	dhcp_pool_shutdown();
	dhcpc_pid = 0;
	process_kill(THIS_PROCESS);
    }

    process_sleep_for(2*ONESEC);   /* here I followed BOOTP example. */

    while (TRUE) {

	/*
	 * watch out for shutdown events
	 */
	if (!(dhcpc_status & DHCPC_STATUS_RUN))
	    break;

	/*
	 * process upto MAX_DHCP_PAKS incoming queued packets
	 */
	dhcp_pak_count = MAX_DHCP_PAKS;
	while (TRUE) {
	    /*
	     * process the DHCP client socket and the
	     * DHCP server packet queue
	     */
	    pak = read_ipsocket(dhcp_client_soc, FALSE, 0, &udp, &dhcp);

	    if (!pak) {
		break;
	    }
	    dhcpc_process_inpak(pak, udp, dhcp);
	    retbuffer(pak);
	    if (!--dhcp_pak_count)
		break;
	}

	/*
	 * scan the lease Q for various events
	 *      also figure out when should I wake up earliest
	 */
	dhcp_next_invoke = dhcpc_scan_leaseQ();
	edisms ((blockproc *)dhcpcBLOCK, (ulong) dhcp_client_soc);
    }

    reg_invoke_ip_close_socket(dhcp_client_soc); /* Done. Close socket */
    dhcpc_death_cleanup();              /* flush pkt and other q's */
    dhcpc_pid = 0;
    process_kill(THIS_PROCESS);		/* process termination */
}

static int dhcpc_read_options (dhcp_values *dhc, uchar *cp, uchar *ep)
{

    /*
     * scan a dhcp options field, beginning at cp and ending at ep
     * Note: we will overwrite multiple instances of an option by
     *       the last value that we find.
     */
    uchar tag, len, byte_count;

    while (cp < ep) {
	tag = *cp++;

	switch (tag) {

	case DHCO_PAD:
	    continue;

	case DHCO_END:
	    cp = ep + 1;   /* break out the loop */
	    continue;

	case DHCO_IPADDR:
	    len = *cp++;
	    if (len != DHCO_LEN_LONG) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad Client IP Addr Option len");
		}
		return DHCP_SCAN_BAD;
	    }
	    DHCP_GETLONG(dhc->c_req_ipaddr, cp);
	    if (dhcp_debug) {
		buginf("\nDHCP: Scan: Client requested IP-addr: %i",
		       dhc->c_req_ipaddr);
	    }
	    break;

	case DHCO_LEASE:
	    len = *cp++;
	    if (len != DHCO_LEN_LONG) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad addr Lease Option len");
		}
		return DHCP_SCAN_BAD;
	    }
	    DHCP_GETLONG(dhc->c_lease, cp);
	    if (dhcp_debug) {
		buginf("\nDHCP: Scan: Lease Time: %u", dhc->c_lease);
	    }
	    break;

	case DHCO_RENEWAL_TIME:
	    len = *cp++;
	    if (len != DHCO_LEN_LONG) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad Renewal-Time Option len");
		}
		return DHCP_SCAN_BAD;
	    }
	    DHCP_GETLONG(dhc->c_renewal, cp);
	    if (dhcp_debug) {
		buginf("\nDHCP: Scan: Renewal time: %u", dhc->c_renewal);
	    }
	    break;

	case DHCO_REBIND_TIME:
	    len = *cp++;
	    if (len != DHCO_LEN_LONG) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad Rebind-Time Option len");
		}
		return DHCP_SCAN_BAD;
	    }
	    DHCP_GETLONG(dhc->c_rebind, cp);
	    if (dhcp_debug) {
		buginf("\nDHCP: Scan: Rebind time: %u", dhc->c_rebind);
	    }
	    break;

	case DHCO_SRVR_ID:
	    len = *cp++;
	    if (len != DHCO_LEN_LONG) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad Server ID Option len");
		}
		return DHCP_SCAN_BAD;
	    }
	    DHCP_GETLONG(dhc->c_srvr_id, cp);
	    if (dhcp_debug) {
		buginf("\nDHCP: Scan: Server ID Option: %i = %X",
		       dhc->c_srvr_id, dhc->c_srvr_id);
	    }
	    break;

	case DHCO_MAX_MSG:
	    len = *cp++;
	    if (len != DHCO_LEN_SHORT) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad Max Message Option len");
		}
		return DHCP_SCAN_BAD;
	    }
	    DHCP_GETSHORT(dhc->c_max_msg, cp);
	    if (dhcp_debug) {
		buginf("\nDHCP: Scan: Max Message Option: %u",
		       dhc->c_max_msg);
	    }
	    if (dhc->c_max_msg < DHCP_MIN_MSG_SIZE_OPTION) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad Max Message length ..");
		}
		return DHCP_SCAN_BAD;
	    }
	    break;

	case DHCO_MSG_TYPE:
	    len = *cp++;
	    if (len != DHCO_LEN_OCTET) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad DHCP Message Type Option len");
		}
		return DHCP_SCAN_BAD;
	    }
	    dhc->c_msg_type = *cp++;
	    if ((dhc->c_msg_type < DHCPM_DISCOVER) ||
		(dhc->c_msg_type > DHCPM_RELEASE)) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad DHCP Message Type %X",
			   dhc->c_msg_type);
		}
		return DHCP_SCAN_BAD;
	    }
	    if (dhcp_debug) {
		buginf("\nDHCP: Scan: Message type: DHCP %s",
		       DHCPM_strings[dhc->c_msg_type]);
	    }
	    break;

	case DHCO_OVERLOAD:
	    /*
	     * we should not read this option other
	     *    than in "real" DHCP options field
	     */
	    len = *cp++;
	    if (len != DHCO_LEN_OCTET) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad Overload Option len");
		}
		return DHCP_SCAN_BAD;
	    }
	    dhc->c_file_sname_ov = *cp++;
	    if (dhc->c_file_sname_ov != (DHCO_OVERFLOW_ALLFLAGS &
					   dhc->c_file_sname_ov)) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad Overflow Option %X",
			   dhc->c_file_sname_ov);
		}
		return DHCP_SCAN_BAD;
	    }
	    if (dhcp_debug) {
		buginf("\nDHCP: Scan: Overflow into: %s %s",
		       (dhc->c_file_sname_ov & DHCO_OVERFLOW_FILE) ?
		       "file":"",
		       (dhc->c_file_sname_ov & DHCO_OVERFLOW_SNAME) ?
		       "sname":"");
	    }
	    break;

	case DHCO_CLASS_ID:
	    len = *cp++;
	    if (len < DHCO_LEN_SHORT) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad Class ID Option len");

		}
		return DHCP_SCAN_BAD;
	    }
	    for (byte_count = 0, len = *cp++; len; len--, cp++) {
		if (byte_count < (DHCP_MAX_STR)) {
		    dhc->c_class_id[byte_count++] = *cp;
		}
	    }
	    dhc->c_class_id[byte_count] = '\0';
	    if (dhcp_debug) {
		buginf("\nDHCP: Scan: Class ID: %s", &dhc->c_class_id[1]);
	    }
	    break;

	case DHCO_CLIENT_ID:
	    len = *cp++;
	    if (len < DHCO_LEN_SHORT) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad Client ID Option len");

		}
		return DHCP_SCAN_BAD;
	    }
	    for (byte_count = 0, len = *cp++; len; len--, cp++) {
		if (byte_count < (DHCP_MAX_STR)) {
		    dhc->c_client_id[byte_count++] = *cp;
		}
	    }
	    dhc->c_client_id[byte_count] = '\0';
	    if (dhcp_debug) {
		buginf("\nDHCP: Scan: Client ID: %s", &dhc->c_client_id[1]);
	    }
	    break;

	case DHCO_MESSAGE:
	    len = *cp++;
	    if (len < DHCO_LEN_OCTET) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad Message reason Option len");
		}
		return DHCP_SCAN_BAD;
	    }
	    for (byte_count = 0, len = *cp++; len; len--, cp++) {
		if (byte_count < (DHCP_MAX_STR)) {
		    dhc->c_reason[byte_count++] = *cp;
		}
	    }
	    dhc->c_reason[byte_count] = '\0';
	    if (dhcp_debug) {
		buginf("\nDHCP: Scan: Reason Message: %s", dhc->c_reason);
	    }
	    break;

	default:
	    len = *cp++;
	    if (len < DHCO_LEN_OCTET) {
		if (dhcp_debug) {
		    buginf("\nDHCP: Scan: bad len %u for option %u in pkt",
			   len, tag);
		}
	    }            /* if len < 1 */
	    cp += len;
	}                /* switch */
    }                    /* while */

    return DHCP_SCAN_OK;
}

static int dhcpc_scan_options (paktype *pak, udptype *udp, dhcptype *dhcp,
			       dhcp_values *dhc)
{

    int options_length, retcode;
    iphdrtype *ip;

    if (!(udp && dhcp)) {
	return DHCP_SCAN_BAD;
    }
    options_length = udp->length - UDPHEADERBYTES - DHCP_FIXED_SIZE;
    if (options_length <= 0) {
	if (dhcp_debug) {
	    buginf("\nDHCP: bad DHCP packet. length less than minimum");
	}
	return DHCP_SCAN_BAD;
    }
    memset(dhc, 0, sizeof(dhcp_values));  /* Initialize to avoid garbage */
    retcode = dhcpc_read_options(dhc, dhcp->dhc_options,
				 &dhcp->dhc_options[options_length]);

    if (DHCP_SCAN_OK != retcode) {
	return retcode;
    }

    /*
     * scan spill over options. If any in the file or sname field
     *      note the order
     */
    if (dhc->c_file_sname_ov & DHCO_OVERFLOW_FILE) {
	retcode = dhcpc_read_options(dhc, &dhcp->dhc_file[0],
				     (uchar *) (&dhcp->dhc_vend_cookie));
	if (DHCP_SCAN_OK != retcode) {
	    return retcode;
	}
    }

    if (dhc->c_file_sname_ov & DHCO_OVERFLOW_SNAME) {
	retcode = dhcpc_read_options(dhc, &dhcp->dhc_sname[0],
				     &dhcp->dhc_file[0]);
	if (DHCP_SCAN_OK != retcode) {
	    return retcode;
	}
    }

    /*
     * some sanity checks at this point
     *       DHCP Message type field must exist
     *       both t1 and t2 timers (if available) should
     *           be less than the lease length
     */

    if (!dhc->c_msg_type) {

	if (dhcp_debug) {
	    buginf("\nDHCP: Scan: **** No DHCP Message Type");
	}
	return DHCP_SCAN_BAD;
    }

    if (dhc->c_lease &&
	((dhc->c_renewal && (dhc->c_renewal > dhc->c_lease)) ||
	 (dhc->c_rebind && (dhc->c_rebind > dhc->c_lease)))) {
	if (dhcp_debug) {
	    buginf("\nDHCP: Scan: Renewal or Rebind time"
		   " larger than lease time");
	}
	return DHCP_SCAN_BAD;
    }

    ip = (iphdrtype *)ipheadstart(pak);
    dhc->c_source = ip->srcadr;
    dhc->c_destination = ip->dstadr;
    dhc->c_offer_ipaddr = dhcp->dhc_yiaddr;

    return DHCP_SCAN_OK;

}

static boolean dhcpc_check_server (ipaddrtype srvr_addr)
{

    /*
     * approve if "any" server is OK.
     * else look for specific server to be in the list
     */
    if (find_dhcps_entry(DHCP_default_server)) {
	return TRUE;
    }
    if (NULL == find_dhcps_entry(srvr_addr)) {
	return FALSE;
    }
    return TRUE;
}


static void dhcp_select_offer (dhcp_values *dhc, iplease_type *ipl)
{

    ipl->client_addr = dhc->c_offer_ipaddr;
    ipl->lease_server_addr = dhc->c_srvr_id;
    GET_TIMESTAMP(ipl->offer_timestamp);

    ipl->lease_secs = dhc->c_lease;
}

static boolean dhcpc_evaluate_offer (dhcp_values *dhc, iplease_type *ipl)
{

    /*
     * if we have a requested IP address,
     *       we want it back
     */
    if (ipl->request_addr) {
	if (ipl->request_addr != dhc->c_offer_ipaddr) {
	    if (dhcp_debug) {
		buginf("\nDHCP: offer not our requested addr. punt");
	    }
	    return FALSE;
	}
    }

    /*
     * we will take any lease offer > MIN_LEASE_LENGTH
     */
    if (dhc->c_lease && (DHCPC_MIN_LEASE_LENGTH > dhc->c_lease)) {
	if (dhcp_debug) {
	    buginf("\nDHCP: offer has below min lease"
		   " length: %u. punt",dhc->c_lease);
	}
	return FALSE;
    }
    return TRUE;
}

static void dhcpc_record_lease (dhcp_values *dhc, iplease_type *ipl)
{

    if (DHC_SELECTING == ipl->state){
	ipl->lease_secs = dhc->c_lease -
	    (ELAPSED_TIME(ipl->offer_timestamp)/ONESEC);
    } else {
	ipl->lease_secs = dhc->c_lease;
    }
    if (ipl->lease_secs > MAX_IPLEASE_SECS) {

	ipl->lease_secs = MAX_IPLEASE_SECS;
	dhc->c_renewal = 0;
	dhc->c_rebind = 0;
    }

    ipl->t1_secs = (dhc->c_renewal) ? dhc->c_renewal :
	(ipl->lease_secs * DHCPC_DEF_RENEWAL_CONST);
    ipl->t2_secs = (dhc->c_rebind) ? dhc->c_rebind :
	(ipl->lease_secs * DHCPC_DEF_REBIND_CONST);
    test_lastallocaddr = ipl->client_addr;
}

static void dhcp_rcv_offer (dhcp_values *dhc, iplease_type *ipl)
{

    /*
     * server identifier option is a must
     */
    if (!dhc->c_srvr_id) {
	if (dhcp_debug) {
	    buginf("\nDHCP: offer received w/o Server ID");
	}
	return;
    }

    if (dhcp_debug) {
	buginf("\nDHCP: offer received from %i", dhc->c_srvr_id);
    }

    /*
     * an actual offered IP address is a must too
     */
    if (!dhc->c_offer_ipaddr) {
	if (dhcp_debug) {
	    buginf("\nDHCP: offer received w/o IP address offer");
	}
	return;
    }

    if (!dhcpc_check_server(dhc->c_srvr_id)) {
	if (dhcp_debug) {
	    buginf("\nDHCP: offer: server not in approved list",
		   dhc->c_srvr_id);
	}
	return;
    }

    /*
     * lease length is a must
     */
    if (!dhc->c_lease) {
	if (dhcp_debug) {
	    buginf("\nDHCP: offer: No lease length ?");
	}
	/*
	 * we will be forgiving, as we can always later attempt
	 *    a request with a suggested lease. But do bitch so
	 *    people fix their end.
	 */

    }

    if (DHC_SELECTING != ipl->state ) {
	if (dhcp_debug) {
	    buginf("\nDHCP: offer received in bad state: %s  punt",
		   DHCPC_strings[ipl->state]);
	}
	return;
    }

    /*
     * check if the IP address, lease times etc. are acceptable
     */
    if (!dhcpc_evaluate_offer(dhc, ipl)) {
	if (dhcp_debug) {
	    buginf("\nDHCP: offer not good enough. punt");
	}
	return;
    }
    dhcp_select_offer(dhc, ipl);
    dhcp_new_state(ipl, DHC_REQUESTING);
    dhcp_send_request(ipl);
}

/*
 * dhcp_rcv_ack
 *
 * a so far valid ack packet has been received.
 * what can we do it. Find more reasons to punt it.
 */
static void dhcp_rcv_ack (dhcp_values *dhc, iplease_type *ipl)
{

    /*
     * server identifier option is a must
     */
    if (!dhc->c_srvr_id) {
	if (dhcp_debug) {
	    buginf("\nDHCP: offer received w/o Server ID");
	}
	return;
    }

    /*
     * match ack with the requested server
     */
    if (dhc->c_srvr_id != ipl->lease_server_addr) {   /* punt if unkown */
	if (dhcp_debug) {
	    buginf("\nDHCP: rcv ack %i is not our server %i",
		   dhc->c_srvr_id, ipl->lease_server_addr);
	}
	return;
    }

    /*
     * is the state valid for acks ?
     */
    switch (ipl->state) {

    case DHC_REQUESTING:
    case DHC_RENEWING:
    case DHC_REBINDING:
	break;

    default:
	if (dhcp_debug) {
	    buginf("\nDHCP: rcv ack in %s state: punt",
		   DHCPC_strings[ipl->state]);
	}
	return;
    }

    /*
     * Check the acked allocated address again
     */

    if (ipl->client_addr != dhc->c_offer_ipaddr) {
	if (dhcp_debug) {
	    buginf("\nDHCP: Ack, not our requested addr. punt");
	}
	return;
    }

    /*
     * probe network to check collissions
     */
    if (!dhcp_verify_net_addr(ipl->client_addr)) {
	if (dhcp_debug) {
	    buginf("\nDHCP: rcv ack: Address in use..send DECLINE");
	}
	dhcp_send_decline(dhc, ipl);
	ipl->client_addr = IPADDR_ZERO;  /* forget offered addr */
	dhcp_new_state(ipl, DHC_SELECTING);
	dhcp_send_discover(ipl);
	return;
    }
    dhcpc_record_lease(dhc, ipl);
    dhcp_new_state(ipl, DHC_BOUND);
    dhcpc_addr_use(ipl);
}

/*
 * dhcp_rcv_nak
 *
 * a so far valid nak packet has been received.
 * what can we do it. Find more reasons to punt it.
 */
static void dhcp_rcv_nak (dhcp_values *dhc, iplease_type *ipl)
{

    /*
     * server identifier option is a must
     */
    if (!dhc->c_srvr_id) {
	if (dhcp_debug) {
	    buginf("\nDHCP: offer received w/o Server ID");
	}
	return;
    }

    /*
     * match Nak with the requested server
     */
    if (dhc->c_srvr_id != ipl->lease_server_addr) {   /* punt if unkown */
	if (dhcp_debug) {
	    buginf("\nDHCP: rcv nak %i is not our server %i",
		   dhc->c_srvr_id, ipl->lease_server_addr);
	}
	return;
    }

    /*
     * is the state valid for naks ?
     */
    switch (ipl->state) {

    case DHC_REQUESTING:
    case DHC_RENEWING:
    case DHC_REBINDING:
	break;

    default:
	if (dhcp_debug) {
	    buginf("\nDHCP: rcv nak in %s state: punt",
		   DHCPC_strings[ipl->state]);
	}
	return;
    }

    /*
     * check the Nak'd address again
     */
    if (ipl->client_addr != dhc->c_offer_ipaddr) {
	if (dhcp_debug) {
	    buginf("\nDHCP: Nak.. not our requested addr. punt");
	}
	return;
    }

    if (DHC_REQUESTING != ipl->state) {
	return;
    }

    /*
     * we don't plan to continue discovering.
     *    we will just forget about the query.
     *    someday we will add recovery.
     */
    /*
      ipl->client_addr = IPADDR_ZERO;  | * forget offered addr * |
      */
    dhcpc_addr_purge(ipl);                 /* Halt network */
}

static void dhcpc_addr_use (iplease_type *ipl)
{
    if (dhcp_debug) {
	buginf("\nDHCP: ***Allocated IP address: %i",
	       ipl->client_addr);
    }
    if (ipl->idb && ipl->idb->ip_peer) {
	ippeerinfotype *ip_peer = ipl->idb->ip_peer;
	ip_peer->ippeer_addr = ipl->client_addr;
	ip_peer->ippeer_addr_source = PEER_ADDR_FROM_DHCP;
    }
}

static void dhcpc_addr_purge (iplease_type *ipl)
{
    hwidbtype *hwidb;

    dhcp_new_state(ipl, DHC_PURGING);
    if (dhcp_debug) {
	buginf("\nDHCP: Address lease expired. Attempting Shutdown");
    }
    hwidb = hwidb_or_null(ipl->idb);
    if (hwidb) {
	hwidb->reset(hwidb);           /* Attempt a shutdown */
    }
}

static boolean dhcp_verify_net_addr (ipaddrtype addr)
{
    /*
     *  we may arp the nets and see if someone
     *     else is also using the same address.
     *     will add later!
     */
    return TRUE;
}

static void dhcpc_process_inpak (paktype *pak, udptype *udp,
				 dhcptype *dhcp)
{

    dhcp_values dhcp_val;
    iplease_type *ipl;

    /*
     * some minor checks:
     *     packet should be DHCP_BOOTREPLY,
     *     packet should have DHCP_MAGIC_COOKIE
     *     we should have the xid in one of our lists
     *     etc.
     */

    if ((dhcp->dhc_op != DHCP_BOOTREPLY) ||
	(dhcp->dhc_vend_cookie != DHCP_MAGIC_COOKIE)) {

	if (dhcp_debug) {
	    buginf("\nDHCP: non dhcp packet received");
	}
	return;
    }

    if (dhcp_debug) {
	buginf("\nDHCP: Received a boot reply packet");
    }
    if (NULL == (ipl = find_dhcpc_lease_xid(dhcp->dhc_xid))) {
	if (dhcp_debug) {
	    buginf("\nDHCP: Not for us...xid: %X", dhcp->dhc_xid);
	    dhcpc_scan_options(pak, udp, dhcp, &dhcp_val);
	    dump_dhcp_pkt(udp, dhcp, &dhcp_val);
	}
	return;    /* don't bother scanning it */
    }

    /*
     * this packet should have DHCP options
     *   in a valid format.
     *   Also fill in dhcp_values, while at it.
     */
    if (DHCP_SCAN_OK != dhcpc_scan_options(pak, udp, dhcp, &dhcp_val)) {
	if (dhcp_debug) {
	    buginf("\nDHCP: Error in pkt. punting");
	}
	return;
    }
    if (dhcp_debug) {
	dump_dhcp_pkt(udp, dhcp, &dhcp_val);
    }

    /*
     *  at this time, we have a valid DHCP packet,
     *     with a valid message type option
     *     let's see what we need to do with it.
     */
    switch(dhcp_val.c_msg_type) {

    case DHCPM_DISCOVER:
    case DHCPM_REQUEST:
    case DHCPM_RELEASE:
    case DHCPM_DECLINE:
	/*
	 * we might have wasted our time scanning options... but
	 * there was no telling it's a useless type otherwise.
	 */
	if (dhcp_debug) {
	    buginf("\nDHCP: Received DHCP %s",
		   DHCPM_strings[dhcp_val.c_msg_type]);
	}
	break;

    case DHCPM_OFFER:
	dhcp_rcv_offer(&dhcp_val, ipl);
	break;

    case DHCPM_ACK:
	dhcp_rcv_ack(&dhcp_val, ipl);
	break;

    case DHCPM_NAK:
	dhcp_rcv_nak(&dhcp_val, ipl);
	break;

    default:
	if (dhcp_debug) {
	    buginf("\nDHCP: bad DHCP message type in dhcpc_process()");
	}
	break;
    }
}

/*
 * go through the leaseQ, looking for timeouts
 *    and other (?) events
 */
static sys_timestamp
dhcpc_scan_leaseQ (void)
{
    iplease_type *ipl, *iplnext;
    sys_timestamp earliest_time;

    TIMER_STOP(earliest_time);
    ipl = (iplease_type *) ip_leaseQ.qhead;
    while (ipl) {
	/*
	 * We may free this entry, so get the next pointer now before
	 * it's poisoned.
	 */
	iplnext = ipl->next;

	if (TIMER_RUNNING_AND_AWAKE(ipl->state_timer)) {

	    switch (ipl->state) {

	    case DHC_SELECTING:
		if (ipl->retry_count >= DHCP_MAX_DISCOVER_RETRY) {
		    if (dhcp_debug) {
			buginf("\nDHCP: QScan: Timed out Selecting state");
		    }
		    dhcp_new_state(ipl, DHC_PURGING);
		    break;
		}
		dhcp_send_discover(ipl);
		break;

	    case DHC_HOLDDOWN:
		dhcp_new_state(ipl, DHC_RELEASING);
		dhcp_send_release(ipl);
		break;

	    case DHC_RELEASING:
		if (ipl->retry_count >= DHCP_MAX_RELEASE_RETRY) {
		    dhcp_new_state(ipl, DHC_PURGING);
		    break;
		}
		dhcp_send_release(ipl);
		break;

	    case DHC_REQUESTING:
		if (ipl->retry_count >= DHCP_MAX_REQUEST_RETRY) {
		    if (dhcp_debug) {
			buginf("\nDHCP: QScan: Timed out Requesting state");
		    }
		    dhcp_new_state(ipl, DHC_PURGING);
		    break;
		}
		dhcp_new_state(ipl, DHC_REQUESTING);
		dhcp_send_request(ipl);
		break;

	    case DHC_BOUND:
		if (dhcp_debug) {
		    buginf("\nDHCP: QScan: Bound..T1 fired..Renewing");
		}
		dhcp_new_state(ipl, DHC_RENEWING);
		dhcp_send_request(ipl);
		break;

	    case DHC_RENEWING:
		if (dhcp_debug) {
		    buginf("\nDHCP: QScan: Renewal..T2 fired..Rebinding");
		}
		dhcp_new_state(ipl, DHC_REBINDING);
		dhcp_send_request(ipl);
		break;

	    case DHC_REBINDING:
		if (dhcp_debug) {
		    buginf("\nDHCP: QScan: Rebind..LT fired..Halting");
		}
		/*
		 * we want the interface to stop using the leased
		 * address. Followed by attempting to discover a new
		 * lease. Hopefully the interface will try to bring
		 * itself up.
		 */
		dhcpc_addr_purge(ipl);                /* Halt network */
		break;

	    case DHC_PURGING:
		if (dhcp_debug) {
		    buginf("\nDHCP: QScan: Purging entry");
		}
		remove_lease_entry(ipl);
		break;

	    default:
		if (dhcp_debug) {
		    buginf("\nDHCP: QScan: Entry in bad state: %X",
			   ipl->state);
		}
		dhcp_new_state(ipl, DHC_PURGING);
		break;
	    }                                           /* switch */
	}                                               /* if timer fired */
	/*
	 * adjust next wake up timer
	 */
	earliest_time = TIMER_SOONEST(earliest_time, ipl->state_timer);

	ipl = iplnext;
    }
    return (earliest_time);
}

/*
 * dhcp_router_reload
 *
 * Called when router is going down gracefully
 */
static void dhcp_router_reload (void)
{
    iplease_type *ipl, *iplnext;

    ipl = (iplease_type *) ip_leaseQ.qhead;
    while (ipl) {
	/*
	 * We will be freeing and releasing all lease entries
	 */
	iplnext = ipl->next;

	switch (ipl->state) {

	case DHC_SELECTING:
	case DHC_RELEASING:
	    dhcp_new_state(ipl, DHC_PURGING);
	    break;

	case DHC_HOLDDOWN:
	case DHC_BOUND:
	case DHC_REQUESTING:
	case DHC_RENEWING:
	case DHC_REBINDING:
	    dhcp_new_state(ipl, DHC_RELEASING);
	    dhcp_send_release(ipl);
	    break;

	default:
	    break;
	}
	ipl = iplnext;
    }
    dhcpc_status |= DHCPC_STATUS_INVOKE;
}


/*
 * dhcp_for_us
 *
 * this is a packet received by bootp server port.
 * see if this should be utilized by us.
 */
static inline boolean dhcp_for_us (paktype *pak)
{

    dhcptype *dhcp;

    dhcp = (dhcptype *) (ipdatastart(pak) + UDPHEADERBYTES);

    /*
     * some positive id checks:
     *     packet should be a unicast
     *     packet should be DHCP_BOOTREPLY,
     *     packet should have DHCP_MAGIC_COOKIE
     *     packet should have client hw addr same as ours
     */

    if ((FUZ_FALSE == ip_broadcast_check(pak, TRUE)) &&
	(DHCP_BOOTREPLY == dhcp->dhc_op) &&
	(DHCP_MAGIC_COOKIE == dhcp->dhc_vend_cookie) &&
	(ieee_equal(default_mac_addr, dhcp->dhc_chaddr))) {

	enqueue_ip_socket(dhcp_client_soc, pak);
	return (TRUE);
    }

    return (FALSE);
}



static void dhcpc_death_cleanup (void)
{
    /*
     * any stuff that we may want to do at shutdown
     */
    dhcp_router_reload();
}



/*
 * dhcp_client_init
 * Initialize the dhcp client system. Includes setting up data
 * structurs, registering parse chains and hooking into some
 * ip registeries.
 */
static void dhcp_client_init (subsystype *subsys)
{
    queue_init(&ip_leaseQ, 0);
    dhcp_debug_init();
    dhcp_parser_init();
    init_dhcps_table();
    add_dhcps_entry(DHCP_default_server);

    dhcp_debug = FALSE;
    reg_invoke_ip_add_pooling_mechanism(&ippool_dhcp);
    reg_add_bootp_check_local_delivery(dhcp_for_us, "dhcp_for_us");
    reg_add_will_reload(dhcp_router_reload, "dhcp_router_reload");
}

/*
 * DHCP PROXY CLIENT subsystem header
 */
#define DHCP_CLIENT_MAJVERSION 1
#define DHCP_CLIENT_MINVERSION 0
#define DHCP_CLIENT_EDITVERSION  1

SUBSYS_HEADER(dhcp, DHCP_CLIENT_MAJVERSION, DHCP_CLIENT_MINVERSION,
	      DHCP_CLIENT_EDITVERSION,
	      dhcp_client_init, SUBSYS_CLASS_LIBRARY,
	      "seq: ip_addrpool_sys",
	      "req: ip_addrpool_sys");
