/* $Id: dhcp_parse.c,v 3.6.10.2 1996/06/17 23:32:03 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ip/dhcp_parse.c,v $
 *------------------------------------------------------------------
 * dhcp_parse.c - DHCP proxy client user interface and utilities
 *
 * Mar 10, 1994, Syed Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module handles the user interface and other parser utilities
 *------------------------------------------------------------------
 * $Log: dhcp_parse.c,v $
 * Revision 3.6.10.2  1996/06/17  23:32:03  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.6.10.1  1996/05/21  09:50:59  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:34:23  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:04:12  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:28:29  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:29:23  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:32:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/18  19:35:29  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.2  1995/08/29  06:05:03  fox
 * CSCdi38536:  peer default ip address pool command does not match
 * documentation
 * Commit changes to mainline in order to match 11.0 Release
 * documentation.
 *
 * Revision 2.1  1995/06/07  20:56:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "logger.h"
#include "../ip/ip.h"
#include "../ip/dhcp.h"
#include "../parser/parser_defs_tcpip.h"
#include "../parser/parser_defs_parser.h"
#include "../ui/debug.h"
#include "../ip/dhcp_debug.h"
#include "../ip/ip_addrpool.h"



/*
 * dhcpsrv_command
 *
 * configure dhcp servers, upto a maximum of MAX_DCHP_SERVERS
 */
void dhcpsrv_command (parseinfo *csb)
{

    dhcp_server_type *dserv;
    ipaddrtype srv_addr = GETOBJ(paddr,1)->ip_addr;
    uint count = 0;

    if (csb->nvgen) {
	if (NULL == find_dhcps_entry(DHCP_default_server)) {
	    nv_write(TRUE, "no %s %i", csb->nv_command,
		     DHCP_default_server);
	}
	dserv = &dhcps_table[0];
	for (count = 0; count < MAX_DHCP_SERVERS; dserv++, count ++) {
	    nv_write(((dserv->flags & DHCPS_VALID) &&
		      (dserv->addr != DHCP_default_server)),
		     "%s %i", csb->nv_command, dserv->addr);
	}

    } else if (csb->set_to_default) {
	init_dhcps_table();
	add_dhcps_entry(DHCP_default_server);

    } else {

	dserv = find_dhcps_entry(srv_addr);
	if (csb->sense) {

	    if (dserv) {
		return;
	    }
	    if (total_dhcpservers >= MAX_DHCP_SERVERS) {
		printf ("\nToo many DHCP servers defined. Entry not accepted");
	    } else {
		add_dhcps_entry(srv_addr);
	    }
	} else {       /* csb->sense    delete this dhcp server */
	    if (NULL == dserv) {
		printf("\nNo such DHCP server defined");
	    } else {
		dserv->flags &= ~DHCPS_VALID;
		total_dhcpservers--;
	    }   /* else NULL== dserv */
	}       /* else csb->sense */
    }           /* else csb->nvgen */
}

/*
 * show_dhcp_servers
 *
 * will print the list of configured dhcp servers
 */
void show_dhcp_servers (void)
{

    dhcp_server_type *dserv = &dhcps_table[0];
    uint count;

    for (count = 0; count < MAX_DHCP_SERVERS; dserv++, count ++) {

	if (dserv->flags & DHCPS_VALID) {

	    printf("\n   DHCP server: %s%i%s",
		   (dserv->addr == DHCP_default_server) ? "ANY (":"",
		   dserv->addr,
		   (dserv->addr == DHCP_default_server) ? ")":"");
	    printf("\n    Leases:   %u", dserv->stats.leases);
	    printf("\n    Offers:   %u      Requests: %u     "
                   "Acks: %u     Naks: %u",
		   dserv->stats.offers, dserv->stats.requests,
		   dserv->stats.acks, dserv->stats.naks);
	    printf("\n    Declines: %u      Releases: %u"
		   "     Bad:  %u\n",
		   dserv->stats.declines, dserv->stats.releases,
		   dserv->stats.bad_types + dserv->stats.out_of_seq);
	}
    }
}

/*
 * show_dhcp
 *
 * interface to the "show dhcp servers" command
 * maybe it should be changed to "show ip pool-addresses"
 *  since it demultiplexes to the current mechanism.
 */
void show_dhcp (parseinfo *csb)
{
    printf("\nDHCP Proxy Client Status:");
    show_dhcp_servers();
}

/*
 * print_lease_entry
 *
 * given the lease entry, print it on either console | stdio
 *
 */
void print_lease_entry (boolean console, iplease_type *ipl)
{
    uchar time_string[DHCP_MAX_STR];
    void (*vprint_lentry)(const char *fmt, ...);   /* print function */

    vprint_lentry = (console) ? buginf : (void *) printf;

    if (!ipl)
	return;
	
    mem_lock(ipl);
    vprint_lentry("\nTemp IP addr: %i  for peer on Interface: %s",
		  ipl->client_addr,
		  (ipl->idb) ? ipl->idb->namestring:"unknown");
    vprint_lentry("\n   DHCP Lease server: %i, state: %u",
		  ipl->lease_server_addr, ipl->state);
    vprint_lentry(" %s", (DHC_PURGING >= ipl->state) ?
		  DHCPC_strings[ipl->state] : "");
    vprint_lentry("\n   DHCP transaction id: %X", ipl->my_xid);
    vprint_lentry("\n   Lease length: %u secs,  Renewal pt.:"
		  " %u secs,  Rebind pt.: %u secs", ipl->lease_secs,
		  ipl->t1_secs, ipl->t2_secs);

    if (TIMER_RUNNING(ipl->state_timer) &&
	SLEEPING(ipl->state_timer)) {
	sprint_ticks_in_dhms(time_string,
			     TIME_LEFT_SLEEPING(ipl->state_timer));
	vprint_lentry("\n   Next timer fires after: %s", time_string);
    } else {
	vprint_lentry("\n   No timer running");
    }

    vprint_lentry("\n   Retry count: %u   Client-ID: %s",
		  ipl->retry_count, ipl->client_id);
    free(ipl);
}

/*
 * show_dhcpc_leases
 *
 * walk the list and print the lease entries
 */
void show_dhcpc_leases (parseinfo *csb)
{

    iplease_type *ipl, *ipl_previous = NULL;
    int count = MAX_IPLEASE_ADDR;

    for (ipl = (iplease_type *) ip_leaseQ.qhead; (NULL != ipl);
	 ipl = ipl->next) {

	if (!--count) {
	    buginf("\n%%DHCP: Too many entries in leaseQ: "
		   "show_dhcpc_leases()");
	    return;
	}
	print_lease_entry(FALSE, ipl);
	if (ipl_previous && (ipl_previous->next != ipl)) /* are we off ? */
	    return;
	ipl_previous = ipl;
    }
}

/*
 * dump_dhcp_pkt
 *
 * given a packet scanned into the structures
 * print the values.
 */
void dump_dhcp_pkt (udptype *udp, dhcptype *dhcp, dhcp_values *dhc)
{

    int len;

    if (!(udp && dhcp && dhc)) {
	return;
    }
    buginf("\nDHCP: rcvd pkt source: %i,  destination:  %i",
	   dhc->c_source, dhc->c_destination);
    len = udp->length - UDPHEADERBYTES - DHCP_FIXED_SIZE;
    if (len <= 0) {
	buginf("\nDHCP: bad DHCP packet. length less than minimum");
	return;
    }
    buginf("\n   UDP  sport: %X,  dport: %X,  length: %u",
	   udp->sport, udp->dport, udp->length);
    buginf("\n   DHCP op: %X, htype: %X, hlen: %X, hops: %X",
	   dhcp->dhc_op, dhcp->dhc_htype, dhcp->dhc_hlen, dhcp->dhc_hops);
    buginf("\n   DHCP server identifier: %i", dhc->c_srvr_id);
    buginf("\n        xid: %X, secs: %X, flags: %X",
	   dhcp->dhc_xid, dhcp->dhc_secs, dhcp->dhc_flags);
    buginf("\n        client: %i, your: %i",
	   dhcp->dhc_ciaddr, dhcp->dhc_yiaddr);
    buginf("\n        srvr:   %i, gw: %i",
	   dhcp->dhc_siaddr, dhcp->dhc_giaddr);
    buginf("\n        options block length: %d\n", len);

    if (dhc->c_msg_type < DHCPM_RELEASE) {
	buginf("\nDHCP %s Message", DHCPM_strings[dhc->c_msg_type]);
    }
    if (DHCPM_OFFER == dhc->c_msg_type) {
	buginf("   Offered Address: %i", dhc->c_offer_ipaddr);
    }
    if (dhc->c_req_ipaddr) {
	buginf("\nDHCP: Requested IP-addr: %i", dhc->c_req_ipaddr);
    }
    if (dhc->c_lease) {
	buginf("\nDHCP: Lease Seconds: %u", dhc->c_lease);
    }
    if (dhc->c_renewal) {
	buginf("    Renewal secs:  %u", dhc->c_renewal);
    }
    if (dhc->c_rebind) {
	buginf("    Rebind secs:   %u", dhc->c_rebind);
    }
    if (dhc->c_srvr_id) {
	buginf("\nDHCP: Server ID Option: %i", dhc->c_srvr_id);
    }
    if (dhc->c_max_msg) {
	buginf("\nDHCP Max Message Option: %u", dhc->c_max_msg);
    }
    if (dhc->c_file_sname_ov & (DHCO_OVERFLOW_FILE |
				DHCO_OVERFLOW_SNAME)) {
	buginf("\nDHCP Overflow into: %s %s",
	       (dhc->c_file_sname_ov & DHCO_OVERFLOW_FILE) ?
	       "file":"", (dhc->c_file_sname_ov & DHCO_OVERFLOW_SNAME) ?
	       "sname":"");
    }
    if ('\0' != dhc->c_class_id[0]) {
	buginf("\nDHCP Class ID: %s", dhc->c_class_id);
    }
    if ('\0' != dhc->c_client_id[0]) {
	buginf("\nDHCP Client ID: %s", dhc->c_client_id);
    }
    if ('\0' != dhc->c_reason[0]) {
	buginf("\nDHCP Reason Message: %s", dhc->c_reason);
    }
}


/*
 * Parser chains for dhcp scripts.
 * Include these in menus.c to simplify makefile changes necessary to
 * add dhcp to an image.
 */

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_dhcp.h"

/*
 * Parser chains and definitions for DHCP client subsystem
 */

/*
 * Parse chains for Debug DHCP commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_dhcp.h"
LINK_POINT(debug_dhcp_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP DHCP subcommand
 */
#define	ALTERNATE	NONE
#include "cfg_dhcp.h"
LINK_POINT(ip_dhcp_client_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for SHOW DHCP commands
 */
#define	ALTERNATE	NONE
#include "exec_show_dhcp.h"
LINK_POINT(dhcp_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for DHCP test commands
 */
#define	ALTERNATE	NONE
#include "exec_test_dhcp.h"
LINK_POINT(dhcp_test_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for DHCP client UI
 */
static parser_extension_request dhcp_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(ip_dhcp_client_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(debug_dhcp_commands) },
    { PARSE_ADD_TEST_CMD, &pname(dhcp_test_commands) }, /* hack alert */
    { PARSE_ADD_SHOW_CMD, &pname(dhcp_show_commands) },
    { PARSE_LIST_END, NULL }
};

void dhcp_parser_init (void)
{
    parser_add_command_list(dhcp_chain_init_table, "dhcp");
}
