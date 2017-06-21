/* $Id: ipname.c,v 3.2.60.6 1996/07/06 05:53:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipname.c,v $
 *------------------------------------------------------------------
 * ipname.c -- ip host and address lookup
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipname.c,v $
 * Revision 3.2.60.6  1996/07/06  05:53:50  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.2.60.5  1996/06/29  00:11:24  snyder
 * CSCdi61694:  change some char * to char [] in ip
 * Branch: California_branch
 *     save 116 bytes
 *
 * Revision 3.2.60.4  1996/06/17  23:32:28  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/05/21  09:51:29  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.60.2  1996/04/15  21:16:32  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  20:17:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:46:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:26:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:35:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/10/23  21:49:28  vandys
 * CSCdi41005:  ip host command writes port number above 32767 as negative
 * to nvram
 * Fix short/ushort handling
 *
 * Revision 2.5  1995/09/11  23:49:27  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.4  1995/08/30 07:41:01  pitargue
 * CSCdi30543:  dns invalid  with domain-name and domain-list specified
 * create a name lookup path for FQDN's only.  domain-list will not affect
 * the lookup, i.e., domains will not be appended to FQDN's.
 *
 * Revision 2.3  1995/08/20  07:47:56  agt
 * CSCdi35100:  Not too obvious error message returned
 * Give appropriate message according to whether addition or deletion is
 * failing.
 *
 * Revision 2.2  1995/06/21  08:55:02  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  20:59:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "address.h"
#include "packet.h"
#include "name.h"
#include "config.h"
#include "ip.h"
#include "ttysrv.h"
#include "../servers/domain.h"
#include "parser.h"
#include "../parser/parser_defs_tcpip.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "ip_registry.h"
#include "subsys.h"

/*
 * Local Storage
 */
extern char *default_domain;		/* default domain name */
queuetype domain_listQ;		/* names to use when resolving */
ipaddrtype domain_server[MAXDOMAINSERVERS]; /* address of the domain server */
boolean domain_nsap;    	/* do domain requests for NSAP addresses */

/*
 * iphost_nvgen
 * Write "ip host ..." entries. For the sake of CPU, process MAX_ENTRY_PER_NVGEN
 * entries at a time
 */
static void iphost_nvgen (parseinfo *csb)
{
    nametype *ptr;
    int i, entry, block;

    entry = block = 0;
    ptr = (nametype *) nameQ.qhead;
    while (ptr) {
	if (entry < MAX_ENTRY_PER_NVGEN) {
	    if ((ptr->flags & (NAM_PERMANENT | NAM_IP)) ==
		(NAM_PERMANENT | NAM_IP)) {
		nv_write(TRUE, "%s %s", csb->nv_command, ptr->name);
		nv_add(ptr->v.ip.port, " %u", (uint) ptr->v.ip.port);
		for (i = 0; i < MAXIPADDRESSES; i++)
		    nv_add(ptr->v.ip.adr[i].length,
			   " %i", ptr->v.ip.adr[i].addr.ip_address);
	    }
	    entry++;
	    ptr = ptr->next;
	}
	else {
	    block++;
	    process_may_suspend();
	    ptr = (nametype *) queuepick(&nameQ.qhead, 
					 MAX_ENTRY_PER_NVGEN*block);
	    entry = 0;
	}
    }
    return;
}

/*
 * iphost_command
 * Parse a string containing a host name/address sequence of the form
 * "host sierra {port} 10.4.0.11 36.40.0.213" where from one to MAXADDRESSES
 * addresses may be specified on the same line.  "port" is an optional 16 bit
 * port number that will be use instead of the default telnet port.
 * For use with the configuration mechanism.
 */

static const char badhost[] = "\nBad ip host command format";

void iphost_command (parseinfo *csb)
{
    register int size;
    register nametype *ptr;

    if (csb->nvgen) {
	iphost_nvgen(csb);
	return;
    }
    /*
     * Check for valid hostname
     */
    if (name_verify(GETOBJ(string,1)) != OK) {
	printf("\n%%IP: Bad hostname format");
	return;
    }
    ptr = (nametype *) name_cache_lookup(GETOBJ(string,1),NULL,NAM_IP);
    if (ptr || (csb->sense == FALSE)) {
	name_delete(GETOBJ(string,1),NAM_IP);
	if (csb->sense == FALSE)
	    return;
    }
    size = sizeof(nametype) + strlen(GETOBJ(string,1));
    ptr = malloc_named(size, "Host");
    if (!ptr)
	return;
    strcpy(ptr->name, GETOBJ(string,1));
    GET_TIMESTAMP(ptr->lasttime);
    ptr->flags = NAM_PERMANENT | NAM_IP | NAM_ADDRVALID | NAM_RLOGIN;

    /*
     * Now go for a port number that may not be there.
     */
    if (GETOBJ(int,2))
	ptr->v.ip.port = (ushort) GETOBJ(int,1);
    /* Now get the addresses, up to MAXIPADDRESSES of them (currently 8).
     * The parser depends on this number being 8 and needs to be changed
     * if this is ever changed.  We copy all addresses since any previous
     * entry was cleared above.
     */
    bcopy(GETOBJ(paddr,1), &ptr->v.ip.adr[0], sizeof(addrtype));
    bcopy(GETOBJ(paddr,2), &ptr->v.ip.adr[1], sizeof(addrtype));
    bcopy(GETOBJ(paddr,3), &ptr->v.ip.adr[2], sizeof(addrtype));
    bcopy(GETOBJ(paddr,4), &ptr->v.ip.adr[3], sizeof(addrtype));
    bcopy(GETOBJ(paddr,5), &ptr->v.ip.adr[4], sizeof(addrtype));
    bcopy(GETOBJ(paddr,6), &ptr->v.ip.adr[5], sizeof(addrtype));
    bcopy(GETOBJ(paddr,7), &ptr->v.ip.adr[6], sizeof(addrtype));
    bcopy(GETOBJ(paddr,8), &ptr->v.ip.adr[7], sizeof(addrtype));
    if (ptr->v.ip.adr[0].length == 0) {	/* flush if no addresses ever seen */
	free(ptr);
	printf(badhost);
    }
    else
	enqueue(&nameQ, ptr);
}

/*
 * iphphost_command
 * Parse a string containing a host name/address sequence of the form
 * "hp-host serv.sierra 36.40.0.213".
 * For use with HP probe protocol.
 */

void iphphost_command (parseinfo *csb)
{
    register int size;
    register nametype *ptr;

    if (csb->nvgen) {
	int i;
	for (ptr = (nametype *) nameQ.qhead; ptr; ptr = ptr->next) {
	    if ((ptr->flags & (NAM_PERMANENT | NAM_HP)) ==
		(NAM_PERMANENT | NAM_HP)) {
		nv_write(TRUE, "%s %s", csb->nv_command, ptr->name);
		nv_add(ptr->v.ip.port, " %u", (uint) ptr->v.ip.port);
		for (i = 0; i < MAXIPADDRESSES; i++)
		    nv_add(ptr->v.ip.adr[i].length,
			   " %i", ptr->v.ip.adr[i].addr.ip_address);
	    }
	}
	return;
    }

    ptr = (nametype *) name_cache_lookup(GETOBJ(string,1),NULL,NAM_HP);
    if (ptr || (csb->sense == FALSE)) {
	name_delete(GETOBJ(string,1),NAM_HP);
	if (csb->sense == FALSE)
	    return;
    }
    size = sizeof(nametype) + strlen(GETOBJ(string,1));
    ptr = malloc_named(size, "HP Host");
    if (!ptr)
	return;
    strcpy(ptr->name, GETOBJ(string,1));
    GET_TIMESTAMP(ptr->lasttime);
    ptr->flags = NAM_PERMANENT | NAM_ADDRVALID | NAM_HP;

    /*
     * Now go for an address.
     */
    bcopy(GETOBJ(paddr,1), &ptr->v.ip.adr[0], sizeof(addrtype));
    enqueue(&nameQ, ptr);
}

/*
 * ip_address_lookup
 * Given a host address, return pointer to a cache entry.
 * No revalidation done here.
 * Return NULL if name string for address not found.
 */

static nametype *ip_address_lookup (ipaddrtype address)
{
    register nametype *ptr;
    int      owner_pid;
    addrtype addr;

    /*
     * Fail for null adddress and what is obviously a broadcast address
     */
    if (!address || address == -1)
	return(NULL);

    /* 
     * first, look in router's own host table 
     */
    addr.type = ADDR_IP;
    addr.length = ADDRLEN_IP;
    addr.addr.ip_address = address;
    ptr = name_cache_lookup(NULL, &addr, NAM_IP);

    if (ptr) {			/* return pointer if we found it */
	if (!systeminit_flag)
	    printf("Configuration mapped ip address %i to %s", 
		   address, ptr->name);
	return(ptr);
    }

    /*
     * Check to see if domain lookups are disabled on the current tty.
     * For everything but the console line, the tt_soc structure
     * contains all the necessary information.  On the console, the
     * current pid must be checked against the pid owning the data
     * structure.  All background processes latch onto the console,
     * but there will only be one exec process that owns it.
     */
    if (!stdio->tty_dns) {
	if (stdio != console) return(NULL);
	
	/*
	 * If the stdio is the console, we may not be in an exec
	 * command, make sure explicitly. 
	 */
	if (process_get_pid(&owner_pid)) {
	    if (owner_pid == stdio->tiptop) {
		return(NULL);
	    }
	}
    }

    /*
     * Ask domain system.  Force any pending output to screen so
     * user has an idea of why output has stopped.
     */
    if (domain_enable) {
	flush();
	ptr = domain_address_lookup(address);
	if (ptr) {
	    if (!systeminit_flag)
		printf("Domain server mapped ip address %i to %s", 
		       address, ptr->name);
	    return(ptr);
	}
    }
    return(NULL);
}

/*
 * ipname_lookup_number
 * Jacket for ip_address_lookup() that converts addrtype to ipaddrtype.
 * Called by SERVICE_NAME_LOOKUP_NUMBER, index is ADDR_IP
 */

static nametype *ipname_lookup_number (addrtype *address)
{
    return(ip_address_lookup(address->ip_addr));
}

/*
 * ipname_lookup_self
 * Try to figure out a system name for this machine
 * (in this case look for IP names via DNS ptr's or in the host cache)
 * Return FALSE if name string for ourselves.
 * Called by SERVICE_NAME_LOOKUP_SELF.
 */

static boolean ipname_lookup_self (char *buffer, int size)
{
    idbtype *idb;
    nametype *ptr;

    FOR_ALL_SWIDBS(idb)
	if (RUNNING_IF(LINK_IP, idb)) {
	    ptr = ip_address_lookup(idb->ip_address);
	    if (ptr) {
		sstrncpy(buffer, ptr->name, size);
		return(TRUE);
	    }
	}

    return(FALSE);
}

/*
 * ipdomain_server_add_delete
 *
 * Common add/delete function for set_ipdomainservers function.
 */
static void ipdomain_server_add_delete (addrtype addr, boolean sense)
{
    register int i;

    if (addr.length == 0)
	return;
    /* Add/Delete an address to/from the domain_server table  */
    for (i = 0; i < MAXDOMAINSERVERS; i++) {
	/* If we find the address, either delete it or return so
	 * that we don't add a duplicate entry.
	 */
	if (domain_server[i] == addr.ip_addr) { 
	    if (!sense) {
		/* Delete the current entry and backfill */
		domain_server[i] = 0L;
		for (i++; i < MAXDOMAINSERVERS && domain_server[i]; i++) {
		    domain_server[i-1] = domain_server[i];
		    domain_server[i] = 0L;
                }
	    }
	    return;             /* Don't allow duplicates */
	}
	if (sense && domain_server[i] == 0L) {
	    domain_server[i] = addr.ip_addr;
	    return;
	}
    }
    if (sense) {
	printf("%% Name-server table is full; %i not added\n", addr.ip_addr);
    } else {
	printf("%% Nameserver entry %i does not exist\n", addr.ip_addr);
    }
}

/*
 * ip_domainname_command
 * Handle the ip domain-name command.
 */

void ip_domainname_command (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write((default_domain != NULL), "%s %s",
		 csb->nv_command, default_domain);
    } else {
	setstring(&default_domain, csb->sense ? GETOBJ(string,1) : NULL);
    }
}

/*
 * ip_domainlookup_command
 * Handle the ip domain-lookup command.
 */

void ip_domainlookup_command (parseinfo *csb)
{
    switch (csb->which) {
    case IP_DOMAINLOOKUP:
	if (csb->nvgen) {
	    nv_write(!domain_enable, "no %s", csb->nv_command);
	} else {
	    domain_enable = csb->set_to_default || csb->sense;
	}
	break;
    case IP_DOMAINLOOKUP_NSAP: /* domain-lookup nsap*/
	if (csb->nvgen) {
	    nv_write(domain_enable && !domain_nsap, "no %s", csb->nv_command);
	} else {
	    domain_nsap = csb->set_to_default || csb->sense;
	}
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * set_ipdomainservers
 * Given a string of blank-separated IP addresses, build domain_server
 * table.  Called by configuration file routines.
 * Note: there is a default 255.255.255.255 which is there
 * by default when you clear things, and will be replaced if a
 * new value is added.
 */
void set_ipdomainservers (parseinfo *csb)
{
    register int i;

    if (csb->nvgen) {
	/*
	 * No need to output name server address if it is the default
	 */
	if (domain_server[0] == -1)
	    return;
	for (i = 0; i < MAXDOMAINSERVERS; i++)
	    nv_write(domain_server[i] != 0L, "%s %i",
		     csb->nv_command, domain_server[i]);
	return;
    }

    if (GETOBJ(paddr,1)->length == 0)  {      /* clear the entire table */
	if (!csb->sense) {
	    for (i = 1; i < MAXDOMAINSERVERS; i++)
	         domain_server[i] = 0L;
	    domain_server[0] = (unsigned) -1L;
	}
	return;
    }

    /* Remove a possible 255.255.255.255 default  */
    if (csb->sense && domain_server[0] == -1L)
	domain_server[0] = 0L;

    ipdomain_server_add_delete(*GETOBJ(paddr,1), csb->sense);
    ipdomain_server_add_delete(*GETOBJ(paddr,2), csb->sense);
    ipdomain_server_add_delete(*GETOBJ(paddr,3), csb->sense);
    ipdomain_server_add_delete(*GETOBJ(paddr,4), csb->sense);
    ipdomain_server_add_delete(*GETOBJ(paddr,5), csb->sense);
    ipdomain_server_add_delete(*GETOBJ(paddr,6), csb->sense);

    for (i = 0; i < MAXDOMAINSERVERS; i++) {
	if (domain_server[i] != 0L)
	    return;
    }
    domain_server[0] = (unsigned) -1L;
}

/*
 * ip_parse_address
 * Given a string, parse as an IP address
 * Returns TRUE if parse succeeded, addrtype argument setup.
 * Called by SERVICE_PARSE_ADDRESS
 */

static boolean ip_parse_address (char *str, addrtype *address)
{
    if (parse_ip_address(str, &address->ip_addr)) {
	address->type = ADDR_IP;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * ipname_show_servers
 * Display configuration of ip name servers, specifically the DNS servers
 * Called by SERVICE_NAME_SHOW_SERVERS
 */

static void ipname_show_servers (void)
{
    domainlisttype *dom;
    int i;

    printf("\nDefault domain is %s",
	   default_domain ? default_domain : "not set");
    if (!QUEUEEMPTY(&domain_listQ)) {
	dom = (domainlisttype *) domain_listQ.qhead;
	printf("\nDomain list: %s", dom->name);
	for (dom=dom->next, i=1; dom && validmem(dom); dom=dom->next, i++) {
	    printf((i%3 ? ", %s" : ",\n             %s"), dom->name);
	}
    }
    printf("\nName/address lookup uses ");
    if (!domain_enable)
	printf("static mappings");
    else {
	printf("domain service");
	printf("\nName servers are %i", domain_server[0]);
	for (i = 1; i < MAXDOMAINSERVERS; i++) {
	    if (domain_server[i])
		printf(", %i",domain_server[i]);
	    else
		break;
	}
    }
    printc('\n');
}

/*
 * ipname_show_hosts
 * Display IP host information
 * Called by SERVICE_NAME_SHOW_HOSTS with NAM_IP or NAM_HP index
 */

static void ipname_show_hosts (nametype *ptr, char *continue_text)
{
    int i;

    if (ptr->flags & NAM_HP) 
	printf("  HP-IP");
    else
	printf("   IP  ");

    /*
     * Print out the IP style address(es).
     */
    for (i = 0; i < MAXIPADDRESSES; i++) {
	if (ptr->v.ip.adr[i].length == 0)
	    break;
	if (ptr->v.ip.adr[i].type != ADDR_IP)
	    continue;
	if ((i != 0) && !(i & 1)) {
	    if (automore_quit()) {
		ptr->sharecount--;
		return;
	    }
	    printf("%s     ", continue_text);
	}
	printf("  %i", ptr->v.ip.adr[i].addr.ip_address);
    }
}

/*
 * ipname_match_number
 * Check if address matches one of addresses in this name cache entry
 * Returns TRUE if match succeeds
 * Called by SERVICE_NAME_MATCH_NUMBER, index is ADDR_IP
 */

boolean ipname_match_number (nametype *ptr, addrtype *address)
{
    int i;

    for (i = 0; i < MAXIPADDRESSES; i++) {
	if (ptr->v.ip.adr[i].length == 0)
	    break;
	if (address->addr.ip_address == ptr->v.ip.adr[i].addr.ip_address) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * ipname_domain_lookup
 * Lookup a hostname string using the DNS
 * Called by SERVICE_NAME_LOOKUP_STRING.
 */

static boolean ipname_domain_lookup (char *name, int name_kind, nametype *old,
			      nametype **new, int *errcode)
{
    nametype *ptr;

    *new = NULL;

    if (!domain_nsap)
        name_kind &= ~NAM_CLNS;

    if (!(name_kind & NAM_USES_DOMAIN))
	return(FALSE);

    printf("Translating \"%s\"", name);
    if (domain_enable && !psipending()) {
	printf("...domain server");
	flush();
	ptr = (nametype *) domain_name_lookup(name, name_kind);
	if (ptr) {
	    name_update_entry(old, ptr);
	    printf(" [OK]\n");
	    if (ptr->flags & name_kind) {
		*new = ptr;
		return(TRUE);
	    }
	}
    }
    *errcode = UNKNOWN_HOSTNAME;	/* else couldn't find host name */

    if (psipending()) {
	printf(" %% Name lookup aborted\n");
	return(FALSE);
    }

    if (old) {                    /* if we revalidated and failed */
	requeue(&nameQ, old);	/* keep old entry */
	printf("...reusing cache entry\n");
	*new = old;
	return(TRUE);		/* and return pointer */
    }
    printc('\n');
    return(FALSE);
}

/*
 * ipname_domain_FQDN_lookup
 * Lookup a FQDN using the DNS
 * Called by SERVICE_NAME_LOOKUP_STRING.
 */

static boolean ipname_domain_FQDN_lookup (char *name, int name_kind,
				   nametype **new)
{
    nametype *ptr;

    *new = NULL;

    if (domain_enable) {
	ptr = (nametype *) domain_FQDN_lookup(name, name_kind);
	if (ptr) {
	    if (ptr->flags & name_kind) {
		*new = ptr;
		return(TRUE);
	    }
	}
    }

    return(FALSE);
}

/*
 * Parse chain registration array for ipname
 */

#define ALTERNATE       NONE
#include "cfg_ip_host.h"
LINK_POINT(ipname_ip_commands, ALTERNATE);
#undef	ALTERNATE

static parser_extension_request ipname_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(ipname_ip_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * ipname_init
 * Initialize variables for IP hostname handling
 */

static void ipname_init (subsystype *dummy)
{
    int i;

    default_domain = NULL;		/* no default domain name yet */
    queue_init(&domain_listQ, 0);	/* no domain names yet */
    for (i = 0; i < MAXDOMAINSERVERS; i++)
	domain_server[i] = 0L;		/* no other domain servers yet */
    domain_server[0] = -1L;		/* broadcast address for server */
    domain_nsap = TRUE;         	/* do DNS for NSAP addresses */

    /*
     * Register some functions
     */
    reg_add_parse_address(ADDR_IP, ip_parse_address, "ip_parse_address");
    reg_add_name_lookup_number(ADDR_IP,
		      ipname_lookup_number, "ipname_lookup_number");
    reg_add_name_lookup_self(ipname_lookup_self, "ipname_lookup_self");
    reg_add_name_lookup_string(ipname_domain_lookup,
			       "ipname_domain_lookup");
    reg_add_FQDN_lookup_string(ipname_domain_FQDN_lookup,
			       "ipname_domain_FQDN_lookup");
    reg_add_name_show_servers(ipname_show_servers, "ipname_show_servers");
    reg_add_name_show_hosts(NAM_IP, ipname_show_hosts, "ipname_show_hosts");
    reg_add_name_show_hosts(NAM_HP, ipname_show_hosts, "ipname_show_hosts");
    reg_add_name_match_number(ADDR_IP,
		      ipname_match_number, "ipname_match_number");
    reg_add_name_match_string(domain_match, "domain_match");
    reg_add_name_validate_addr(domain_validate_addr, "domain_validate_addr");
    reg_add_name_parse_hostname(ADDR_IP, ip_parse_hostname,
				"ip_parse_hostname");
    reg_add_name_martian(ADDR_IP, ip_martian, "ip_martian");
    reg_add_ip_address_lookup(ip_address_lookup, "ip_address_lookup");
    parser_add_command_list(ipname_chain_init_table, "ipname");
}

/*
 * IPNAME subsystem header
 */

#define IPNAME_MAJVERSION 1
#define IPNAME_MINVERSION 0
#define IPNAME_EDITVERSION  1

SUBSYS_HEADER(ipname, IPNAME_MAJVERSION, IPNAME_MINVERSION, IPNAME_EDITVERSION,
	      ipname_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      "req: iphost, domain");
