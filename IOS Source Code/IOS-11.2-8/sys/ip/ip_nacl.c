/* $Id: ip_nacl.c,v 3.1.2.8 1996/08/30 01:49:52 gchristy Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_nacl.c,v $
 *------------------------------------------------------------------
 * ip_nacl.c -- Named Access Lists
 *
 * March 1996, Greg Christy
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_nacl.c,v $
 * Revision 3.1.2.8  1996/08/30  01:49:52  gchristy
 * CSCdi67644:  IP NACL: inform user when access-list type is invalid
 * Branch: California_branch
 * - Complain if an existing list is an invalid type.
 *
 * Revision 3.1.2.7  1996/08/14  23:48:51  gchristy
 * CSCdi66011:  Give a warning if same name is used on ip standard and
 * extended list
 * Branch: California_branch
 * - Warn the user if an NACL exists with the same name, but a different
 *   type.
 *
 * Revision 3.1.2.6  1996/07/24  00:16:10  widmer
 * CSCdi63069:  2 ip choices presented by parser in config mode
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/07/20  01:13:10  bew
 * CSCdi63411:  ACL encrypted/decrypted flags are not useful
 * Branch: California_branch
 * Remove encrypted/decrypted flags introduced early in 11.2
 * from ACL code.
 *
 * Revision 3.1.2.4  1996/05/30  23:44:01  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/05/21  09:51:10  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.2.2  1996/05/20  07:52:15  gchristy
 * CSCdi57884:  IP NACL: functionality should be identical to numeric lists
 * Branch: California_branch
 * - Support crypto_flags and associated keywords in parser
 *
 * Revision 3.1.2.1  1996/05/04  07:14:30  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.1  1996/05/04  06:59:06  speakman
 * Missed a placeholder for IP Named Access Lists.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "globs.h"
#include "ctype.h"
#include "stdlib.h"
#include "config.h"
#include "parser.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../parser/macros.h"
#include "../parser/parser_actions.h"
#include "access.h"
#include "ip.h"
#include "ip_macros.h"
#include "ip_actions.h"
#include "tunnel.h"
#include "ip_registry.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/parser_defs_igmp.h"
#include "ipaccess.h"
#include "ipaccess_private.h"
#include "ipaccess2.h"
#include "parser_defs_ip.h"
#include "../parser/parser_defs_access.h"
#include <ciscolib.h>
#include "../tcp/http.h"

#undef IP_NACL_DEBUGGING
#ifdef IP_NACL_DEBUGGING
#define IP_NACL_DEBUG(s) buginf s
#else
#define IP_NACL_DEBUG(s)
#endif
/*
 * Pointers used in parseinfo structure.
 */

#define csb_aclptr	userDataBlock[0]

static parser_mode *ipnacl_simple_config_mode;
static parser_mode *ipnacl_extended_config_mode;

/*
 * Parse chains for top-level configuration commands
 */
#define ALTERNATE	NONE
#include "cfg_ip_nacl.h"
LINK_POINT(ip_nacl_cfg, ALTERNATE);
#undef ALTERNATE

static parser_extension_request ip_nacl_chain_table[] = {
    { PARSE_ADD_CFG_RTG_IP_CMD, &pname(ip_nacl_cfg) },
    { PARSE_LIST_END, NULL }
};
/*
 * ip_nacl_extended_nvgen
 * NVGEN an extended ACL
 */

static void ip_nacl_extended_nvgen (parseinfo *csb, acl_headertype *acl)
{
    slowtype *item;

    if (!acl)
	return;

    item = (slowtype  *) acl->list;

    nv_write(TRUE, "%s %s", csb->nv_command, acl->name);

    while (item) {
	nv_write(TRUE, " ");
	nv_add(item->dynamic, "dynamic %s ",
	       item->dynacl_p->aclname);
	nv_add(item->timeout, "timeout %d ", item->timeout);
	nv_add(TRUE, "%s ", item->grant ? "permit" : "deny  ");
	nv_add_id_string(proto_items, item->protocol);
	
	if ((item->source == ANY_HOST) &&
	    (item->srcmask == WILD_ANY))
	    nv_add(TRUE, " any");
	else if (item->srcmask == WILD_HOST)
	    nv_add(TRUE, " host %i", item->source);
	else 
	    nv_add(TRUE, " %i %i", item->source, item->srcmask);

	if (item->srcopr != OPC_NONE) {
	    nv_add(TRUE, " ");
	    nv_add_id_string(ip2access_cmds, item->srcopr);
	    nv_add(TRUE, " ");
	    switch (item->protocol) {
	    case TCP_PROT:
		reg_invoke_nv_add_tcp_port(item->sport);
		break;
	    case UDP_PROT:
		reg_invoke_nv_add_udp_port(item->sport);
		break;
	    default:
		break;
	    }
	    if (item->srcopr == OPC_RANGE) {
		nv_add(TRUE, " ");
		switch (item->protocol) {
		case TCP_PROT:
		    nv_add_id_string(tcp_ports, item->sport2);
		    break;
		case UDP_PROT:
		    nv_add_id_string(udp_ports, item->sport2);
		    break;
		default:
		    break;
		}
	    }
	}

	if ((item->destination == ANY_HOST) &&
	    (item->dstmask == WILD_ANY))
	    nv_add(TRUE, " any");
	else if (item->dstmask == WILD_HOST)
	    nv_add(TRUE, " host %i", item->destination);
	else 
	    nv_add(TRUE, " %i %i", item->destination,
		   item->dstmask);

	if (item->dstopr != OPC_NONE) {
	    nv_add(TRUE, " ");
	    switch (item->protocol) {
	    case TCP_PROT:
		nv_add_id_string(ip2access_cmds, item->dstopr);
		nv_add(TRUE, " ");
		nv_add_id_string(tcp_ports, item->dport);
		break;
	    case UDP_PROT:
		nv_add_id_string(ip2access_cmds, item->dstopr);
		nv_add(TRUE, " ");
		nv_add_id_string(udp_ports, item->dport);
		break;
	    case ICMP_PROT:
		if (item->dstopr == OPC_ONEBYTE) {
		    nv_add_id_string(icmptypes, item->dport);
		} else if (item->dstopr == OPC_TWOBYTES)
		    nv_add_id_subid_string(icmptypecodes,
					   item->dport,
					   item->dport2);
		break;
	    case IGMP_PROT:
		if (item->dstopr == OPC_ONEBYTE) 
		    nv_add_id_string(igmptypes, item->dport & 0xf);
		break;
	    default:
		break;
	    }
	    if (item->dstopr == OPC_RANGE) {
		nv_add(TRUE, " ");
		switch (item->protocol) {
		case TCP_PROT:
		    nv_add_id_string(tcp_ports, item->dport2);
		    break;
		case UDP_PROT:
		    nv_add_id_string(udp_ports, item->dport2);
		    break;
		default:
		    break;
		}
	    }
	}
	
	if (item->established)
	    nv_add(TRUE," established");
	
	if (item->precedence) {
	    nv_add(TRUE, " precedence ");
	    nv_add_id_string(precedence_values,
			     item->precedence_value >>
			     IP_PRECEDENCE_SHIFT);
	}
	
	if (item->tos) {
	    nv_add(TRUE, " tos ");
	    nv_add_id_string(tos_values, item->tos_value >>
			     IP_TOS_SHIFT); 
	}
	
	switch (item->log) {
	case IP_ACCESS_LOG_NORMAL:
	    nv_add(TRUE, " log");
	    break;

	case IP_ACCESS_LOG_INPUT:
	    nv_add(TRUE, " log-input");
	    break;

	default:
	    break;

	}
	item = item->next;
    }
}

/*
 * ip_nacl_simple_nvgen
 * NVGEN a simple ACL
 */

static void ip_nacl_simple_nvgen (parseinfo *csb, acl_headertype *acl)
{
    int j;
    fasttype *grp;
    fasthash *acc;
    fastitem *item;

    if (!acl)
	return;

    nv_write(TRUE, "%s %s", csb->nv_command, acl->name);

    grp = (fasttype *) acl->list;
    if (!grp)			/* nothing to do */
	return;

    /*
     * Scan the hash table of single addresses
     */
    for (j = 0; j < NETHASHLEN; j++) {
	for (acc = grp->hashlist[j]; acc; acc = acc->next)
	    nv_write(TRUE, " %s %i", acc->grant ? "permit" : "deny  ", 
		     acc->address);
    }
    /*
     * Scan the table of wildcarded entries
     */
    item = grp->items;
    while (item)  {
	nv_write(TRUE, " %s ", item->grant ? "permit" : "deny  ");
	if ((item->permitmask == ANY_HOST) &&
	    (item->wildmask == WILD_ANY))
	    nv_add(TRUE, "any");
	else 
	    nv_add(TRUE, "%i %i", item->permitmask,
		   item->wildmask); 
	item = item->next;
    }

}

/*
 * ip_nacl_command_nvgen
 * NVGEN named ACLs.
 */

void
ip_nacl_command_nvgen (parseinfo *csb)
{
    acl_headertype *acl;
    list_element *el;
    boolean first_list;

    if (LIST_EMPTY(&nacl_list))
	return;

    first_list = TRUE;

    switch (csb->which) {
    case ACL_IP_NAMED_SIMPLE:
	FOR_ALL_DATA_IN_LIST(&nacl_list, el, acl) {
	    if (acl->type == ACL_IP_NAMED_SIMPLE) {
		if (first_list) {
		    nv_write(TRUE,"!");
		    first_list = FALSE;
		}
		ip_nacl_simple_nvgen(csb, acl);
	    }
	}
	break;
    case ACL_IP_NAMED_EXTENDED:
	FOR_ALL_DATA_IN_LIST(&nacl_list, el, acl) {
	    if (acl->type == ACL_IP_NAMED_EXTENDED) {
		if (first_list) {
		    nv_write(TRUE,"!");
		    first_list = FALSE;
		}
		ip_nacl_extended_nvgen(csb, acl);
	    }
	}
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * ip_nacl_delete
 * Delete a list.
 */
static void
ip_nacl_delete (acl_headertype *acl)
{
    switch (acl->type) {

    case ACL_IP_SIMPLE:
    case ACL_IP_NAMED_SIMPLE:
	ip1access_deletelist(acl);
	break;

    case ACL_IP_EXTENDED:
    case ACL_IP_NAMED_EXTENDED:
	ip2access_deletelist(acl);
	break;

    default:
	break;
    }

    acl->type = ACL_UNDEFINED;
}

/*
 * ip_nacl_simple_cleanup
 * See if any hash entries or list exists. If not, delete fasttype
 * header.
 */
static void
ip_nacl_simple_cleanup (acl_headertype *acl)
{
    int i;
    fasttype *grp;

    if (!acl)
	return;
    grp = (fasttype *) acl->list;

    if (!grp)
	return;

    if (grp->items)
	return;

    for (i = 0; i < NETHASHLEN; i++)
	if (grp->hashlist[i])
	    return;

    acl->list = NULL;
    free(grp);
}

/*
 * ip_nacl_simple_deleteentry
 * Delete an entry from an existing simple ACL
 */
static void
ip_nacl_simple_deleteentry (parseinfo *csb, acl_headertype *acl)
{
    boolean grant;
    fasttype *grp;
    fasthash *acc, *prev_acc;
    fastitem *item, *prev_item;
    ipaddrtype address, mask;
    leveltype level;
    int i;

    grant = GETOBJ(int,2);

    /*
     * Get address and wildcard mask
     */

    address = GETOBJ(paddr,1)->ip_addr;
    mask = GETOBJ(paddr,2)->ip_addr;

    /*
     * Get pointer to the access list.
     * If no access list found, create a new one.
     */
    grp = (fasttype *) acl->list;
    if (!grp)
	return;

    /*
     * If no wildcards (mask is zero), find the address
     * in the hash table, delete it, and return.
     */
    if (mask == IPADDR_ZERO) {
	i = nethash(address);
	prev_acc = NULL;
	acc = grp->hashlist[i];
	while (acc) {
	    if ((acc->address == address) && (acc->grant == grant)) {
		level = raise_interrupt_level(ALL_DISABLE);

		if (prev_acc)
		    prev_acc->next = acc->next;
		else
		    grp->hashlist[i] = acc->next;

		reset_interrupt_level(level);

		reg_invoke_ip_send_acl_change_ipc(FALSE, acl, acc,
						  NULL, NULL);
		reg_invoke_ip_access_list(acl);
		free(acc);
		ip_nacl_simple_cleanup(acl);
		return;
	    }
	    prev_acc = acc;
	    acc = acc->next;
	}
	return;
    }
    address &= ~mask;

    /*
     * Find and delete item.
     */
    item = grp->items;
    prev_item = NULL;
    while (item) {
	if ((item->permitmask == address) && 
	    (item->wildmask == mask) &&(item->grant == grant)) {
	    level = raise_interrupt_level(ALL_DISABLE);

	    if (prev_item)
		prev_item->next = item->next;
	    else
		grp->items = item->next;

	    reset_interrupt_level(level);

	    reg_invoke_ip_access_list(acl);
	    reg_invoke_ip_send_acl_change_ipc(FALSE, acl, NULL,
					      item, NULL);
	    free(item);
	    ip_nacl_simple_cleanup(acl);
	    return;
	}
	prev_item = item;
	item = item->next;
    }

}

/*
 * ip_nacl_extended_deleteentry
 * Delete an entry from an existing extended ACL
 */
static void
ip_nacl_extended_deleteentry (parseinfo *csb, acl_headertype *acl)
{
    uint protocol, opr, srcopr, precedence_value, tos_value;
    boolean grant, established, precedence, tos, log;
    ipaddrtype source, destination, srcmask, dstmask;
    ulong dport, dport2, sport, sport2, i;
    slowtype *item, *prev_item;
    boolean dynamic;
    leveltype level;

    if (!acl)
	return;

    grant = GETOBJ(int, 2);
    established = GETOBJ(int, 7);
    /*
     * Set some default values 
     */
    opr = OPC_NONE;
    srcopr = OPC_NONE;
    dport = 0;
    dport2 = 0;
    sport = 0;
    sport2 = 0;
    precedence = FALSE;
    tos = FALSE;
    log = IP_ACCESS_LOG_NONE;

    /*
     * Collect info. for Dynamic lists.
     */
    dynamic = GETOBJ(int,16);

    /*
     * Get the protocol type
     * Protocol may either be one of the strings defined above, or a
     * number from 0 to 255.
     */
    protocol = GETOBJ(int,3);
    
    /*
     * Get source address and wildcard mask
     */
    source = GETOBJ(paddr,1)->ip_addr;
    srcmask =  GETOBJ(paddr,2)->ip_addr;

    /*
     * Get destination address and wildcard mask
     */
    destination = GETOBJ(paddr,3)->ip_addr;
    dstmask = GETOBJ(paddr,4)->ip_addr;
    
    /*
     * Get the operator code.  If end of string, we default
     * to matching only on protocol addresses.
     */
    if ((protocol == UDP_PROT) ||
	(protocol == TCP_PROT) ||
	(protocol == ICMP_PROT) ||
	(protocol == IGMP_PROT)) {
	opr = GETOBJ(int,4);
	if (opr != OPC_NONE) {
	    dport = GETOBJ(int,5);
	    dport2 = GETOBJ(int,6);
	    if (opr == OPC_RANGE) {
		if (dport == dport2)
		    opr = OPC_EQ;
		else if (dport > dport2) {
		    i = dport;
		    dport = dport2;
		    dport2 = i;
		}
	    }
	    /*
	     * This hack puts the version number in the byte along with the
	     * value that we're trying to filter so that we can do simple
	     * byte operations later.
	     */
	    if (protocol == IGMP_PROT)
		dport |= IGMP_VERSION1 << 4;
	}
	srcopr = GETOBJ(int, 8);
	if (srcopr != OPC_NONE) {
	    sport = GETOBJ(int, 9);
	    sport2 = GETOBJ(int, 10);
	    if (srcopr == OPC_RANGE) {
		if (sport == sport2)
		    srcopr = OPC_EQ;
		else if (sport > sport2) {
		    i = sport;
		    sport = sport2;
		    sport2 = i;
		}
	    }
	}
    }
	
    /*
     * Get the precedence and precedence value.
     */
    precedence = GETOBJ(int, 11);
    if (precedence)
	precedence_value = (GETOBJ(int, 12) << IP_PRECEDENCE_SHIFT);
    else
	precedence_value = 0;

    /*
     * Get the tos and tos value.
     */
    tos = GETOBJ(int, 13);
    if (tos)
	tos_value = (GETOBJ(int, 14) << IP_TOS_SHIFT);
    else
	tos_value = 0;

    /*
     * Get the log information
     */
    log = GETOBJ(int, 15);

    /*
     * Ensure item is not already in list
     */
    destination &= ~dstmask;
    source &= ~srcmask;
    item = (slowtype *) acl->list;
    prev_item = NULL;
    while (item) {
	if ((item->grant == grant) &&
	    (item->source == source) &&
	    (item->srcmask == srcmask) &&
	    (item->destination == destination) &&
	    (item->dstmask == dstmask) &&
	    (item->protocol == protocol) &&
	    (item->established == established) &&
	    (item->dstopr == opr) &&
	    (item->dport == dport) &&
	    (item->dport2 == dport2) &&
	    (item->srcopr == srcopr) &&
	    (item->sport == sport) &&
	    (item->sport2 == sport2) &&
	    (item->precedence == precedence) &&
	    (item->precedence_value == precedence_value) &&
	    (item->tos == tos) &&
	    (item->tos_value == tos_value) &&
	    (item->log == log) &&
	    (item->dynamic == dynamic)) {

	    level = raise_interrupt_level(ALL_DISABLE);

	    if (prev_item) {
		prev_item->next = item->next;
	    } else {
		acl->list = (accessptr *) item->next;
	    }

	    reset_interrupt_level(level);

	    if (item->dynamic)
		ipaccess_remove_dynacl(item->dynacl_p);
	    
	    free(item);

	    reg_invoke_ip_send_acl_change_ipc(TRUE, acl, NULL, NULL, item);
	    reg_invoke_ip_access_list(acl);
	    return;
	}
	prev_item = item;
	item = item->next;
    }
    

}

/*
 * find_ip_acl
 *
 * Find access-list for parser/http
 */

static acl_headertype *find_ip_acl (char *name, uint *type, boolean sense)
{
    acl_headertype *acl;
    int number = 0;

    if (isdigit(name[0])) {
	number = atoi(name);
	if ((*type == ACL_IP_NAMED_SIMPLE) &&
	    (number >= MINFASTACC) && (number <= MAXFASTACC)) {
	    *type = ACL_IP_SIMPLE;
	} else if ((*type == ACL_IP_NAMED_EXTENDED) &&
		   (number >= MINSLOWACC) && (number <= MAXSLOWACC)) {
	    *type = ACL_IP_EXTENDED;
	} else {
	    printf(nacl_name_error);
	    return(NULL);
	}
    }

    if (!sense) {
	if (number) {
	    acl = acl_array[number];
	} else {
	    acl = access_find_nacl(name);
	}
	if (acl && (acl->type == *type)) {
	    ip_nacl_delete(acl);
	}
	return(NULL);
    }


    if (number) {
	acl = access_find_or_create_acl(number);
    } else {
	if (!is_valid_nacl(name)) {
	    printf(nacl_name_error);
	    return(NULL);
	}	
	if (ipaccess_get_listbyname(name)) {
	    printf("\n%% A Dynamic access list with this name already exists");
	    return(NULL);
	}
	acl = access_find_or_create_nacl(name, *type);
	if (!acl) {
	    /*
	     * Warn the user if an NACL exists with the same name, but
	     * a different type. 
	     */ 
	    acl = access_find_nacl(name);
	    if (acl && (acl->type != *type)) {
		printf("\n%% A %s access list with this name already exists",
		       access_type2string(acl->type));
	    }
	    return(NULL);
	}
    }
    return(acl);
}

/*
 * ip_nacl_command
 *
 * Parse top-level "ip access-list" config commands.
 */

void
ip_nacl_command (parseinfo *csb)
{
    csb->csb_aclptr = find_ip_acl(GETOBJ(string,1), &csb->which, csb->sense);
    if (!csb->csb_aclptr) {
	return;
    }

    switch (csb->which) {
    case ACL_IP_SIMPLE:
    case ACL_IP_NAMED_SIMPLE:
	set_mode_byname(&csb->mode, "ipsnacl", MODE_VERBOSE);
	break;
    case ACL_IP_EXTENDED:
    case ACL_IP_NAMED_EXTENDED:
	set_mode_byname(&csb->mode, "ipenacl", MODE_VERBOSE);
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }

    if (csb->flags & CONFIG_HTTP) {
	char *new_uri = string_getnext();
	char *old_uri = csb->http_state->uri;

	/* Setup new URI with parser mode */
	sprintf(new_uri, "/%s/%s", get_mode_name(csb->mode), GETOBJ(string,1));

	/* Call parser and generate help for new URI */
	csb->http_state->uri = new_uri;
	reg_invoke_http_parser(csb->http_state, csb->mode, FALSE);

	/* Put back old URI */
	csb->http_state->uri = old_uri;
    }
}

/*
 * ip_nacl_simple_command
 *
 * Parse simple access lists
 */

void
ip_nacl_simple_command (parseinfo *csb)
{
    acl_headertype *acl;

    acl = csb->csb_aclptr;
    if (!acl)
	return;

    if (csb->sense)
	ip1access_parselist(csb, acl);
    else
	ip_nacl_simple_deleteentry(csb, acl);
}

/*
 * ip_nacl_extended_command
 *
 * Parse extended access lists
 */

void
ip_nacl_extended_command (parseinfo *csb)
{
    acl_headertype *acl;

    acl = csb->csb_aclptr;
    if (!acl) 
	return;

    if (csb->sense)
	ip2access_parselist(csb, acl);
    else
	ip_nacl_extended_deleteentry(csb, acl);
}

/*
 * ipsnacl_http_init
 *
 * http initialization for ipsnacl mode
 */

static boolean ipsnacl_http_init (parseinfo *csb)
{
    char *status = reg_invoke_http_get_uri_token(csb->http_state);

    if (status) {
	csb->which = isdigit(status[0]) ? ACL_IP_SIMPLE :
	    ACL_IP_NAMED_SIMPLE;
	csb->csb_aclptr = find_ip_acl(status, &csb->which, TRUE);
	if (csb->csb_aclptr) {
	    csb->mode = ipnacl_simple_config_mode;
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * ipenacl_http_init
 *
 * http initialization for ipenacl mode
 */

static boolean ipenacl_http_init (parseinfo *csb)
{
    char *status = reg_invoke_http_get_uri_token(csb->http_state);

    if (status) {
	csb->which = isdigit(status[0]) ? ACL_IP_EXTENDED :
	    ACL_IP_NAMED_EXTENDED;
	csb->csb_aclptr = find_ip_acl(status, &csb->which, TRUE);
	if (csb->csb_aclptr) {
	    csb->mode = ipnacl_extended_config_mode;
	    return(TRUE);
	}
    }
    return(FALSE);
}


/*
 * ip_nacl_init
 *
 * Initialize IP named access list subsystem
 */

static void
ip_nacl_init (subsystype *subsys)
{
    parser_add_command_list(ip_nacl_chain_table, "ip named access-lists");
    ipnacl_simple_config_mode = parser_add_mode(
			"ipsnacl", "config-std-nacl",
			"IP named simple access-list configuration mode", 
			TRUE, FALSE, "configure", NULL,
			NULL, &pname(top_conf_ip_snacl),
			ipsnacl_http_init);

    ipnacl_extended_config_mode = parser_add_mode(
			"ipenacl", "config-ext-nacl",
			"IP named extended access-list configuration mode", 
			TRUE, FALSE, "configure", NULL,
			NULL, &pname(top_conf_ip_enacl),
			ipenacl_http_init);

}

/*
 * IP NACL subsystem header
 */

#define IP_NACL_MAJVERSION	1
#define IP_NACL_MINVERSION	0
#define IP_NACL_EDITVERSION	1

SUBSYS_HEADER(ip_nacl, IP_NACL_MAJVERSION, IP_NACL_MINVERSION,
	      IP_NACL_EDITVERSION, ip_nacl_init, SUBSYS_CLASS_LIBRARY,
	      NULL, NULL);
