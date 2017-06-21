/* $Id: ipalias.c,v 3.4.12.5 1996/07/06 05:53:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipalias.c,v $
 *------------------------------------------------------------------
 * ipalias.c -- Code for pretending that we are several IP addresses
 *
 * August 1987, William Westfield
 * January 1992, modified to support indefinite number of aliases.
 * Russ Tremain
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipalias.c,v $
 * Revision 3.4.12.5  1996/07/06  05:53:26  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.4.12.4  1996/07/02  17:00:58  slin
 * CSCdi61526:  MacIP ipgpAssign Error - Overlapping IP address with
 * client range
 * Branch: California_branch
 * - During IP address assignment, skip addresses that fail IP alias chec
 *
 * Revision 3.4.12.3  1996/06/29  00:11:18  snyder
 * CSCdi61694:  change some char * to char [] in ip
 * Branch: California_branch
 *     save 116 bytes
 *
 * Revision 3.4.12.2  1996/04/15  21:16:15  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/03/18  20:16:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.2  1996/03/13  01:37:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.20.1  1996/02/20  00:59:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/13  19:04:00  rja
 * CSCdi43596:  ARP throttling not working
 * Replumbed IPv4 ARP Request software so that the ARP throttling
 * sanity checks always occur and aren't accidentally bypassed by
 * some callers.  Removed one now dead function in ip_arp.c
 *
 * Revision 3.3  1995/11/30  22:58:20  bcole
 * CSCdi43875: HSRP ARP table entries contain bia instead of virtual MAC
 * address
 *
 * During ARP table refresh, don't revise dynamic IP aliases using the
 * interface's BIA.  Be a little more intelligent about which interfaces
 * to send gratuitous ARPs out.
 *
 * Revision 3.2  1995/11/17  09:34:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/15  00:20:05  bcole
 * CSCdi38170:  Ping to HSRP IP address causes ICMP echo request storm
 *
 * Add notion of dynamic IP aliases, so that HSRP virtual IP address is
 * now pingable.  Eliminate packet forwarding problem for this virtual
 * IP address as a side effect.
 *
 * Revision 2.2  1995/06/09  13:05:35  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:58:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "ip.h"
#include "ip_media.h"
#include "config.h"
#include "../ip/ip_registry.h"
#include "../ip/ipaddress.h"
#include "subsys.h"
#include "sys_registry.h"

/*
 * ipalias_port
 * Translate a localhost/localport pair to a "logical port number" that
 * is used by tcp_getline, and by access checks.  If dhost is NOT in the
 * hash table, just return dport.
 * For IP aliases aimed at telnet use, the original destination port must
 * be the telnet port.  Otherwise,  we would risk defeating firewalls.
 */

ulong ipalias_port (ipaddrtype dhost, ushort dport)
{
    ipaddr_tabletype *addr;

    FOR_IPADDR_HASH_CHAIN(dhost, addr) {
	if (addr->type == IPADDR_ALIAS && addr->address == dhost) {
	    /*
	     * if this is a translation or MacIP alias, ignore the local
	     * port that was originally requested.
	     */
	    if (addr->port & ALIAS_FLAGMASK)
		return(addr->port);
	    /*
	     * if this is a normal ip alias for accessing tty ports,
	     * insist that the original port be the telnet port!
	     * (otherwise, return zero, which should stop connections)
	     */
	    if (dport != TELNET_PORT)
		return(0);
	    return((ulong) addr->port);
	}
    }
    return((ulong) dport);
}

/*
 * ipalias_data
 * Return the extra data stored in an IP alias.
 */

void *ipalias_data (ipaddrtype dhost)
{
    ipaddr_tabletype *addr;

    FOR_IPADDR_HASH_CHAIN(dhost, addr) {
	if (addr->type == IPADDR_ALIAS && addr->address == dhost)
	    return(addr->data);
    }
    return(NULL);
}

/*
 * ipalias_check
 * Checks whether the given alias is legal.  It cannot be a martian,
 * must be on the same subnet as an existing interface, and there
 * cannot be another host that responds to ARPs for that address.
 */

static boolean ipalias_check (ipaddrtype target, boolean check_dynamic,
			      boolean send_arp)
{
    idbtype *idb;
    arptype *ptr;

    /* A known alias? */
    if (ip_staticaliasaddr(target) ||
	(check_dynamic && ip_aliasaddress(target)))
	return(TRUE);		/*  must be OK then. */
    if (ip_ouraddress(target))          /* used by an interface? */
	return(FALSE);
    if (martian(target, TRUE))
	return(FALSE);
    idb = ipaddr_findidb(target);
    if (idb == NULL)
	return(FALSE);
    if (idb->hwptr->status & IDB_ASYNC) 	/* ignore async interfaces */
	return(FALSE);

    /*
     * If an IEEE medium, send an ARP to see if someone else
     * is using this address.  Could be generalized.
     */
    if (send_arp && idb_is_ieeelike(idb)) {
	ptr = ip_arp_table_lookup(idb, target);
	if ((ptr == NULL) && ip_router_running) {
	    process_sleep_for(ONESEC / 2);             	/* wait half a second*/
	    ptr = ip_arp_table_lookup(idb, target); /* and try again */
	}
	if (ptr != NULL)
	    return(FALSE);	/* something else has this address */
    }
    return(TRUE);		/* address is OK */
}

/*
 * ipalias_insert_common
 * Do all the work necessary to insert an ipaddress/port pair into the
 * set of aliases we will respond to.  Check legality, insert in hash
 * table, and arp cache, all at once.
 */

static boolean ipalias_insert_common (ipaddrtype addr, ulong port,
				      enum ipaddr_type type, void *data,
				      boolean send_arp)
{
    idbtype *idb;

    if (!ipalias_check(addr, type == IPADDR_DYN_ALIAS, send_arp))
	return(FALSE);
    if (!ipaddr_table_insert(addr, NULL, type, 0, port, data))
	return(FALSE);
    idb = ipaddr_findidb(addr);
    if (idb == NULL)
	return(FALSE);
    if (idb_is_ieeelike(idb))
	ip_arp_table_add(idb, addr, idb->hwptr->hardware,
			 idb->hwptr->enctype, ARP_INTERFACE);
    return(TRUE);
}

/*
 * ipalias_insert
 *
 * Insert an alias into the address table.  Primes the ARP cache.
 */

boolean ipalias_insert (ipaddrtype addr, ulong port, void *data)
{
    return(ipalias_insert_common(addr, port, IPADDR_ALIAS, data, TRUE));
}

/*
 * ipalias_insert_dynamic
 *
 * Insert a dynamic alias (address aliasing only, does not show
 * in configuration) into the address table.  Primes the ARP cache.
 */

boolean ipalias_insert_dynamic (ipaddrtype addr, void *data, boolean send_arp)
{
    return(ipalias_insert_common(addr, 0, IPADDR_DYN_ALIAS, data, send_arp));
}

/*
 * ipalias_delete_common
 * Remove alias of an ipaddress/port pair from tables and flush ARP cache.
 */

static void ipalias_delete_common (ipaddrtype address, boolean dynamic)
{
    if (dynamic) {
	if (!ip_aliasaddress(address))	      /* make sure a bona fide alias */
	    return;
    } else {
	if (!ip_staticaliasaddr(address))     /* make sure a bona fide alias */
	    return;
    }
    if (!ipaddr_table_delete(address, NULL))  /* remove hash table entry */
	return;
    arp_table_remove(address, LINK_IP);	      /* remove ARP entry, if any */
}

/*
 * ipalias_delete
 * Remove static alias of an ipaddress/port pair from tables
 * and flush ARP cache.
 */

void ipalias_delete (ipaddrtype address)
{
    ipalias_delete_common(address, FALSE);
}

/*
 * ipalias_delete_dynamic
 * Remove dynamic alias of an ipaddress/port pair from tables
 * and flush ARP cache.
 */

void ipalias_delete_dynamic (ipaddrtype address)
{
    ipalias_delete_common(address, TRUE);
}

/*
 * ipalias_reload
 * Add an entry to the arp cache for every entry in the alias table.
 * We need to do this after a CLEAR ARP command is done...
 */

static void ipalias_reload (idbtype *reload_if)
{
    ipaddr_tabletype *addr;
    idbtype *swidb;
    int bucket;

    for (bucket = 0; bucket < NETHASHLEN; bucket++) {
	for (addr = ipaddr_hash_table[bucket]; addr; addr = addr->next) {
	    /*
	     * Suppress the reload of this address if the maintaining
	     * protocol will be handling it.
	     */
	    if (addr->addrflags & IPADDRFLAG_NO_ARP_RELOAD)
		continue;
	    swidb = ipaddr_findidb(addr->address);
	    if (swidb == NULL ||
                (addr->type == IPADDR_INTERFACE &&
                 interface_admin_down(swidb))) {
		continue;
	    }
	    if (idb_is_ieeelike(swidb)) {
		ip_arp_table_add(swidb, addr->address, swidb->hwptr->hardware,
				 swidb->hwptr->enctype, ARP_INTERFACE);
		ip_arp_unsolicited(swidb, addr->address, NULL, NULL);
	    }
	}
    }
}

/*
 * ip_alias_command
 * Parse the rest of an alias command from a config file.
 * The command looks like " alias 192.31.7.29 4008 ".
 * Check for validity, insert the address into the alias
 * hash table, and insert the address into the arp table
 * just like our real local address is inserted.
 */

void ip_alias_command (parseinfo *csb)
{
    ipaddr_tabletype *addr;
    ipaddrtype host;
    int bucket;
    ulong port;

    if (csb->nvgen) {
	for (bucket = 0; bucket < NETHASHLEN; bucket++) {
	    for (addr = ipaddr_hash_table[bucket]; addr; addr = addr->next) {
		if (addr->type == IPADDR_ALIAS)
		    nv_write(((addr->port & ALIAS_FLAGMASK) == 0), "%s %i %d",
			     csb->nv_command, addr->address, addr->port);
	    }
	}
	return;
    }
    port = 0;
    host = GETOBJ(paddr,1)->ip_addr;
    if (csb->sense) {
	port = (int)((short)GETOBJ(int,1));
	if (!ipalias_insert(host, port, NULL))
	    printf("Invalid IP address");
    } else
	ipalias_delete(host);
}

/*
 * show_ip_alias
 * Show all the currently defined aliases.
 */

static const char ipasliasheader[] = "Address Type IP Address     Port";

void show_ip_alias (parseinfo *csb)
{
    ipaddr_tabletype *addr;
    int bucket, len;
    char *str;

    automore_enable(ipasliasheader);
    for (bucket = 0; bucket < NETHASHLEN; bucket++) {
	for (addr = ipaddr_hash_table[bucket]; addr; addr = addr->next) {
	    switch (addr->type) {
	    case IPADDR_INTERFACE:
		str = "Interface ";
		break;
	    case IPADDR_ALIAS:
		str = "Alias     ";
		break;
	    case IPADDR_DYN_ALIAS:
		str = "Dyn. Alias";
		break;
	    default:
		str = "Unknown   ";
		break;
	    }
	    printf("\n%s   ", str);
	    len = printf("%i", addr->address);
	    if (addr->port == 0)
		continue;
	    if (addr->port <= ALIAS_PORTMASK)
		printf("%-*c%d", 15-len, ' ', addr->port);
	    else {
		switch (addr->port & ALIAS_FLAGMASK) {
		  case TCPPAD_ALIASPORT:
		    printf("%-*cProtocol Translation", 15-len, ' ');
		    break;
		  default:
		    printf("%-*c0x%lx+%d", 15-len, ' ', 
			   addr->port & ALIAS_FLAGMASK,
			   addr->port & ALIAS_PORTMASK);
		    break;
		}
	    }
	}
    }
    automore_disable();
}

/*
 * ipalias_init
 * Initialize IP alias support
 */

static void ipalias_init (subsystype *dummy)
{
    reg_add_arp_reload(ipalias_reload, "ipalias_reload");
    reg_add_ipalias_port(ipalias_port, "ipalias_port");
    reg_add_ipalias_data(ipalias_data, "ipalias_data");
    reg_add_ipalias_check(ipalias_check, "ipalias_check");
    reg_add_ipalias_insert(ipalias_insert, "ipalias_insert");
    reg_add_ipalias_insert_dynamic(ipalias_insert_dynamic,
				   "ipalias_insert_dynamic");
    reg_add_ipalias_delete(ipalias_delete, "ipalias_delete");
    reg_add_ipalias_delete_dynamic(ipalias_delete_dynamic,
				   "ipalias_delete_dynamic");
    /*
     * Initialize parser for "ip alias" facility
     */
    ipalias_parser_init();
}

/*
 * IPALIAS subsystem header
 */

#define IPALIAS_MAJVERSION 1
#define IPALIAS_MINVERSION 0
#define IPALIAS_EDITVERSION  1

SUBSYS_HEADER(ipalias, IPALIAS_MAJVERSION, IPALIAS_MINVERSION,
	      IPALIAS_EDITVERSION,
	      ipalias_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
