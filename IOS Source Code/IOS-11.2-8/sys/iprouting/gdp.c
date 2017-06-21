/* $Id: gdp.c,v 3.3.20.3 1996/07/06 05:54:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/gdp.c,v $
 *------------------------------------------------------------------
 * gdp.c -- Gateway Discovery Protocol
 *
 * 29-July-1989, Kirk Lougheed
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gdp.c,v $
 * Revision 3.3.20.3  1996/07/06  05:54:50  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.3.20.2  1996/06/28  23:18:50  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/04/15  21:17:47  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/01  06:05:17  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  17:33:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/29  21:07:43  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * Send ICMP port unreachable is GDP is not enabled.
 *
 * Revision 2.1  1995/06/07  21:06:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "route.h"
#include "config.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../iprouting/gdp.h"
#include "parser_defs_gdp.h"
#include "../if/network.h"
#include "../ip/ip_registry.h"
#include "subsys.h"


/*
 * ip_gdp_command
 * Parse the GDP kewyord of the ip subcommand of the interface command
 *    [no] ip gdp
 *	   ip gdp priority <number>
 *	   ip gdp reporttime <seconds>
 *	   ip gdp holdtime <seconds>
 *	   ip gdp default-route
 *    [no] ip gdp address <priority> <holdtime>
 */

void ip_gdp_command (parseinfo *csb)
{
    ushort priority, holdtime;
    idbtype *idb;
    gproxy *ptr;
    ipaddrtype address;

    /* GDP server functions are mutually exclusive with GDP client */
    if (reg_invoke_ip_gdpclient_enable()) {
	if (!csb->nvgen)
	    printf("%% Cannot enable GDP server; client is running\n");
	return;

    }

    idb = csb->interface;
    if (csb->nvgen) {
	if (!idb->ip_enabled || !idb->ip_gdp_enabled)
	    return;
	nv_write(TRUE, csb->nv_command);
	if (idb->hwptr->status & IDB_IEEEMASK) {
	    nv_write(idb->ip_gdp_countreset != GDP_REPORTTIME,
		     "%s reporttime %d", csb->nv_command,
		     idb->ip_gdp_countreset);
	} else {
	    nv_write(idb->ip_gdp_countreset != GDP_XREPORTTIME,
		     "%s reporttime %d", csb->nv_command,
		     idb->ip_gdp_countreset);
	}
	nv_write(idb->ip_gdp_holdtime != GDP_HOLDTIME, "%s holdtime %d",
		 csb->nv_command, idb->ip_gdp_holdtime);
	nv_write(idb->ip_gdp_priority != GDP_PRIORITY, "%s priority %d",
		 csb->nv_command, idb->ip_gdp_priority);
	nv_write(idb->ip_gdp_default, "%s default-route", csb->nv_command);
	for (ptr = (gproxy *) idb->ip_gdp_proxyQ.qhead; ptr; ptr = ptr->next)
	    nv_write(TRUE, "%s address %i %d %d", csb->nv_command,
		     ptr->address, ptr->priority, ptr->holdtime);
	return;
    }

    switch (GETOBJ(int,1)) {

      case GDP_CMD_ENABLE:
	if (!idb->ip_gdp_enabled && csb->sense) {
	    if (idb->hwptr->status & IDB_IEEEMASK)
		idb->ip_gdp_countreset = GDP_REPORTTIME;
	    else
		idb->ip_gdp_countreset = GDP_XREPORTTIME;
	    idb->ip_gdp_count = idb->ip_gdp_countreset;
	    idb->ip_gdp_holdtime = GDP_HOLDTIME;
	    idb->ip_gdp_priority = GDP_PRIORITY;
	}
	idb->ip_gdp_enabled = csb->sense;
	break;

      case GDP_CMD_ADDRESS:
        address = GETOBJ(paddr,1)->ip_addr;
	priority = GETOBJ(int,2);
	holdtime = GETOBJ(int,3);
	for (ptr = (gproxy *) idb->ip_gdp_proxyQ.qhead; ptr; ptr = ptr->next) {
	    if (ptr->address == address) {
	        if (!csb->sense) {
		    unqueue(&idb->ip_gdp_proxyQ, ptr);
		    free(ptr);
		} else {
		    ptr->priority = priority;
		    ptr->holdtime = holdtime;
		}
		return;
	     }
	}
	if (csb->sense) {
	    ptr = malloc(sizeof(gproxy));
	    if (ptr) {
		ptr->address = address;
		ptr->priority = priority ? priority : GDP_PRIORITY;
		ptr->holdtime = holdtime ? holdtime : GDP_HOLDTIME;
		enqueue(&idb->ip_gdp_proxyQ, ptr);
	    }
	}
	break;

      case GDP_CMD_HOLDTIME:
	idb->ip_gdp_holdtime = csb->sense ? GETOBJ(int,2) : GDP_HOLDTIME;
	break;

      case GDP_CMD_PRIORITY:
	idb->ip_gdp_priority = csb->sense ? GETOBJ(int,2) : GDP_PRIORITY;
	break;

      case GDP_CMD_REPORTTIME:
	if (idb->hwptr->status & IDB_IEEEMASK)
	    idb->ip_gdp_countreset = csb->sense ? GETOBJ(int,2) :
		GDP_REPORTTIME;
	else
	    idb->ip_gdp_countreset = csb->sense ? GETOBJ(int,2) :
		GDP_XREPORTTIME;
	idb->ip_gdp_count = idb->ip_gdp_countreset;
	break;

      case GDP_CMD_DEFAULTROUTE:
	idb->ip_gdp_default = csb->sense;
	break;
    }
}

/*
 * ip_gdp_send_report
 * Send a report message out the given interface for the given destination
 */

static void
ip_gdp_send_report (idbtype *idb, ipaddrtype destination)
{
    ipaddtype *ap;
    gproxy *pp;
    int bytes, count;
    paktype *pak;
    udptype *udp;
    gdptype *gdp;
    gdpinfo *info;

    /*
     * Count addresses.  One for the primary, then as many secondary and
     * proxy addresses as have been previously specified.
     */
    if (!router_enable)
	return;
    count = 1;
    for (ap = (ipaddtype *) idb->ip_addressQ.qhead; ap; ap = ap->next)
	count++;
    for (pp = (gproxy *) idb->ip_gdp_proxyQ.qhead; pp; pp = pp->next)
	count++;

    /*
     * Build a UDP packet from scratch.  Do not use IP sockets.
     */
    bytes = IPHEADERBYTES(NOPT) + UDPHEADERBYTES + GDPHEADERBYTES +
		GDPDATABYTES(count);
    pak = getbuffer(bytes);
    if (!pak)
	return;
    udp = (udptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    udp->sport = udp->dport = GDP_PORT;
    udp->length = UDPHEADERBYTES + GDPHEADERBYTES + GDPDATABYTES(count);
    udp->checksum = 0;
    gdp = (gdptype *) ((uchar *) udp + UDPHEADERBYTES);
    gdp->version = GDP_VERSION;
    gdp->opcode = GDP_OPC_REPORT;
    gdp->count = count;
    gdp->reserved = 0;
    info = (gdpinfo *) (gdp + 1);
    info->address = idb->ip_address;
    if (idb->ip_gdp_default) {
	reg_invoke_iprouting_gdp_default_ndb_prio(&info->priority);
    } else
	info->priority = idb->ip_gdp_priority;
    info->holdtime = idb->ip_gdp_holdtime;
    for (ap = (ipaddtype *) idb->ip_addressQ.qhead; ap; ap = ap->next) {
	info++;
	info->address = ap->address;
	info->priority = idb->ip_gdp_priority;
	info->holdtime = idb->ip_gdp_holdtime;
    }  
    for (pp = (gproxy *) idb->ip_gdp_proxyQ.qhead; pp; pp = pp->next) {
	info++;
	info->address = pp->address;
	info->priority = pp->priority;
	info->holdtime = pp->holdtime;
    }
    if (udp_debug) 
	buginf("\nUDP: sent src=%i(%d), dst=%i(%d), length=%d",
	    n2hl(idb->ip_address), GDP_PORT, destination, GDP_PORT,
	    udp->length);
    ipwrite(pak, bytes, UDP_PROT, idb->ip_address, destination, idb, NULL,
		&ipdefaultparams);
}

/*
 * ip_gdp_periodic
 * Called by REGISTRY_IF_ONESEC
 * Do GDP only if IP and GDP are both enabled.  If there is a zero
 * report count, then we are only replying to queries, never
 * gratuitously volunteering information (consider the case of X.25).
 */

static void
ip_gdp_periodic (hwidbtype *hwidb)
{
    idbtype *swidb;

    if (!hw_interface_up(hwidb))
	return;
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	if (swidb->ip_enabled && swidb->ip_gdp_enabled && 
	    swidb->ip_gdp_countreset) {
	    if (swidb->ip_gdp_count-- == 0) {
		ip_gdp_send_report(swidb, -1L);
		swidb->ip_gdp_count = swidb->ip_gdp_countreset;
	    }
	}
	
    }
}

/*
 * ip_gdp_rcv_query
 * Handle a query by sending a report.
 */

static boolean
ip_gdp_rcv_query (paktype *pak, udptype *udp)
{
    idbtype *idb;
    iphdrtype *ip;
    gdptype *gdp;

    ip = (iphdrtype *)ipheadstart(pak);

    if (udp->dport != GDP_PORT)
	return(FALSE);
    idb = pak->if_input;
    if (idb->ip_enabled && idb->ip_gdp_enabled) {
	if (samecable(ip->srcadr) == idb) {
	    gdp = (gdptype *) ((uchar *) udp + UDPHEADERBYTES);
	    if (gdp->opcode == GDP_OPC_QUERY)
		ip_gdp_send_report(idb, ip->srcadr);
	}
    }
    if (gdp_gdp_enable) {
	process_enqueue_pak(gdp_clientQ, pak);
    } else {
	send_unreachable(pak, ICMP_UNRPORT);
	retbuffer(pak);
    }
    return(TRUE);
}

/*
 * gdp_init
 * Initialize gdp support
 */

static void gdp_init (subsystype *dummy)
{
    /*
     * Initialize Router Side
     */
    gdp_parser_init();
    reg_add_if_onesec(ip_gdp_periodic, "ip_gdp_periodic");
    reg_add_ip_udp_input(ip_gdp_rcv_query, "ip_gdp_rcv_query");
    gdpclient_init();
}

/*
 * GDP subsystem header
 */

#define GDP_MAJVERSION 1
#define GDP_MINVERSION 0
#define GDP_EDITVERSION  1

SUBSYS_HEADER(gdp, GDP_MAJVERSION, GDP_MINVERSION, GDP_EDITVERSION,
	      gdp_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
