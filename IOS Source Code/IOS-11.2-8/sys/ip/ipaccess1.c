/* $Id: ipaccess1.c,v 3.3.26.6 1996/06/28 23:17:28 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaccess1.c,v $
 *------------------------------------------------------------------
 * ipaccess1.c  -- Standard ("Fast") IP Access Lists
 *
 * 6-February-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Note: Named access lists rely on some of these functions.  Be sure
 * to update ip_nacl.c to reflect any changes made here.
 *
 *------------------------------------------------------------------
 * $Log: ipaccess1.c,v $
 * Revision 3.3.26.6  1996/06/28  23:17:28  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.26.5  1996/05/30  23:44:04  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3.26.4  1996/05/18  01:26:40  gchristy
 * CSCdi57694:  IP ACL: fix simple ACL bounds check in fast_check()
 * Branch: California_branch
 *
 * Revision 3.3.26.3  1996/05/13  05:38:18  gchristy
 * CSCdi57481:  IP SACL: fix nvgen typo
 * Branch: California_branch
 *
 * Revision 3.3.26.2  1996/05/08  01:09:41  gchristy
 * CSCdi56944:  Compilation errors from IP named access list commit
 * Branch: California_branch
 * - Modularize new ACL code for inclusion in VIP images.
 *
 * Revision 3.3.26.1  1996/05/04  01:02:46  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.3  1996/01/19  00:57:19  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  09:33:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:04:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:57:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "config.h"
#include "access.h"
#include "ctype.h"
#include "stdlib.h"
#include "../parser/parser_defs_access.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "../crypto/crypto_public.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccess_private.h"
#include "../ip/ipaccess_inline.h"
#include "sys_registry.h"
#include "ip_registry.h"
#include "../os/list.h"


void ipaccess_init (void)
{
    reg_add_access_named_show(show_ip1access, "show_ip1access");
    reg_add_access_named_show(show_ip2access, "show_ip2access");
    reg_add_access_clear(clear_ip_access_stats, "clear_ip_access_stats");
    reg_add_ip_access_list(accesslog_cache_flush, "accesslog_cache_flush");
    reg_add_onemin(accesslog_cache_print, "accesslog_cache_print");
}    

/*
 * check_access
 * Check if a tty has access to a particular host.
 * Returns TRUE if access granted, FALSE otherwise.
 *
 * Note that we do *not* check for an enabled EXEC.  The undergrads
 * merely learn the password and use it to defeat the access checking.
 * Force people to log onto an unrestricted host.
 */

boolean check_access (tt_soc *tty, ipaddrtype target, ulong sport, ulong
		      dport, boolean outflag)
{
    int group;
    ipaddrtype mask;
    fasttype *grp;
    fasthash *acc;
    fastitem *item;
    acl_headertype *acl;

    group = (outflag ? tty->accessout : tty->accessin);
    if (group <= 0)
      return(TRUE);

    if (group > MAXFASTACC) {
	acl = acl_array[group];
	return(slow_check(acl, FALSE, NULL, TCP_PROT,
			  outflag ? 0L : target,
			  outflag ? target : 0L,
			  sport, dport, FALSE, 0));
    }
    grp = (fasttype *) ip_getacl(group);
    if (grp == NULL)
	return(TRUE);

    /*
     * Check the hash table of non-wildcarded entries.
     * Fall through to linear scan of wild entries if no matches.
     */
    acc = grp->hashlist[nethash(target)];
    while (acc) {
	if (target == acc->address)
	    return(acc->grant);
	acc = acc->next;
    }

    /*
     * Scan the permission list and make a decision.
     */
    item = grp->items;
    while (item) {
	mask = ~item->wildmask;
	if ((target & mask) == item->permitmask)
	    return(item->grant);
	item = item->next;
    }
    return(FALSE);
}

/*
 * network_check
 * Used by network_update().  This routine is here so gs/route1.c doesn't
 * have to include h/access.h.  It also seems general enough for other 
 * areas of the system.
 */
boolean network_check (acl_headertype *acl,
		       ipaddrtype gateway, ipaddrtype target) 
{

    if (!acl)
	return(TRUE);

    switch (acl->type) {
    case ACL_IP_EXTENDED:
    case ACL_IP_NAMED_EXTENDED:

	return(slow_check(acl, FALSE, NULL, IP_PROT, gateway, target,
			  0, 0, FALSE, 0));
    case ACL_IP_SIMPLE:
    case ACL_IP_NAMED_SIMPLE:
	return(nfast_check(acl, target));
	
    default:
	break;
    }

    return(TRUE);
}

/*
 * route_check
 * This routine is here so gs/route1.c doesn't have to include h/access.h.
 * It also seems general enough for other areas of the system.
 */
boolean route_check (acl_headertype *acl, ipaddrtype network, ipaddrtype mask)
{

    if (!acl)
	return(TRUE);

    switch (acl->type) {
	
    case ACL_IP_EXTENDED:
    case ACL_IP_NAMED_EXTENDED:
	return(slow_check(acl, FALSE, NULL, IP_PROT, network, mask,
			  0, 0, FALSE, 0));
    case ACL_IP_SIMPLE:
    case ACL_IP_NAMED_SIMPLE:
	return(nfast_check(acl, network));

    default:
	break;
    }
    
    return(TRUE);
}

/*
 * fast_check
 * Compatibility wrapper for functions still using numbered ACLs.
 */

boolean fast_check (int list, ipaddrtype target)
{
    acl_headertype *acl;

    /*
     * Find the access list.  Return TRUE (pass the packet)
     * if the group doesn't exist or is illegal.
     */
    if ((list <= 0) || (list > MAXFASTACC)) 
	return(TRUE);

    acl = acl_array[list];

    return(nfast_check(acl, target));
}

/*
 * ip1access_nvgen
 * NVGEN a simple ACL
 */

void ip1access_nvgen (parseinfo *csb, acl_headertype *acl)
{
    int j;
    fasttype *grp;
    fasthash *acc;
    fastitem *item;

    if (!acl)
	return;

    grp = (fasttype *) acl->list;

    if (!grp)
	return;
    /*
     * Scan the hash table of single addresses
     */
    for (j = 0; j < NETHASHLEN; j++) {
	for (acc = grp->hashlist[j]; acc; acc = acc->next)
	    nv_write(TRUE, "access-list %s %s %i", acl->name,
		     acc->grant ? "permit" : "deny  ",
		     acc->address);
    }
    /*
     * Scan the table of wildcarded entries
     */
    item = grp->items;
    while (item)  {
	nv_write(TRUE, "access-list %s %s ", acl->name,
		 item->grant ? "permit" : "deny  ");
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
 * ipaccess1_deletelist
 * Nuke the given simple list.
 */
void ip1access_deletelist (acl_headertype *acl)
{
    fasttype *grp;
    fasthash *acc, *next;
    fastitem *item, *wrk_item;
    int i;

    if (!acl)
	return;

    grp = (fasttype *) acl->list;
    acl->list = NULL;
    if (!grp)
	return;
    for (i = 0; i < NETHASHLEN; i++) {
	acc = grp->hashlist[i];
	while (acc) {
	    reg_invoke_ip_send_acl_change_ipc(FALSE, acl, acc,
					      NULL, NULL);
	    next = acc->next;
	    acc->next = NULL;
	    free(acc);
	    acc = next;
	}
    }
    item = grp->items;
    while (item) {
	reg_invoke_ip_send_acl_change_ipc(FALSE, acl, NULL, item,
					  NULL);
	wrk_item = item->next;
	item->next = NULL;
	free(item);
	item = wrk_item;
    }
    free(grp);
    reg_invoke_ip_access_list(acl);

    return;
}

/*
 * ip1access_parselist
 * Parse and generate a simple ACL entry.
 */
void ip1access_parselist (parseinfo *csb, acl_headertype *acl)
{
    boolean grant;
    fasttype *grp;
    fasthash *acc, *wrk_acc;
    fastitem *item, *wrk_item;
    ipaddrtype address, mask;
    uint i;

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
    if (grp == NULL) {
	grp = malloc(sizeof(fasttype));
	if (grp == NULL) 
	    return;
	grp->items = 0;
	acl->list = (accessptr *)grp;
    }

    /*
     * If no wildcards (mask is zero), enter the address
     * into the hash table and return.  Make sure entry is
     * not already in the list.
     */
    if (mask == IPADDR_ZERO) {
	wrk_acc = NULL;
	i = nethash(address);
	acc = grp->hashlist[i];
	while (acc) {
	    if ((acc->address == address) && (acc->grant == grant))
		return;
	    wrk_acc = acc;
	    acc = acc->next;
	}
	acc = malloc(sizeof(fasthash));
	if (acc) {
	    acc->address = address;
	    acc->grant = grant;
	    if (wrk_acc)
		wrk_acc->next = acc;
	    else
		grp->hashlist[i] = acc;
	    reg_invoke_ip_access_list(acl);
            reg_invoke_ip_send_acl_change_ipc(TRUE, acl, acc,
					      NULL, NULL);

	} 
	return;
    }
    address &= ~mask;

    /*
     * First make sure entry is not already in the table.
     */
    item = grp->items;
    wrk_item = grp->items;
    while (item) {
	if ((item->permitmask == address) && 
	    (item->wildmask == mask) &&(item->grant == grant))
	    return;
	wrk_item = item;
	item = item->next;
    }
    
    /*
     * Allocate some memory and add the item at the end of the chain.
     */
    item = malloc(sizeof(fastitem));
    if (item) {
	item->grant = grant;
	item->wildmask = mask;
	item->permitmask = address;
	if (!wrk_item)
	    grp->items = item;
	else
	    wrk_item->next =item;
	reg_invoke_ip_access_list(acl);
	reg_invoke_ip_send_acl_change_ipc(TRUE, acl, NULL,
					  item, NULL);

    }

}

/*
 * ip1access_command
 * Parse an access list specification of the form.
 * Format of list varies according to century.
 * To delete an access list in its entirety:
 *	"no access-list 4"
 */

void ip1access_command (parseinfo *csb)
{
    int i;
    int number;
    acl_headertype *acl;

    if (csb->nvgen) {
	for (i = MINFASTACC; i <= MAXFASTACC; i++) {
	    acl = acl_array[i];
	    ip1access_nvgen(csb, acl);
	}
	return;
    }

    /* Get access list number and permit/deny flag */
    number = GETOBJ(int,1);

    /*
     * If parsing "no access-list <list>", stop now to delete
     * the entire list. 
     */
    if (csb->sense == FALSE) {
	acl = acl_array[number];
	ip1access_deletelist(acl);
	return;
    }


    acl = access_find_or_create_acl(number);
    if (!acl) 
	return;

    ip1access_parselist(csb, acl);
    return;
}

static void ip1access_printlist (acl_headertype *acl)
{
    int i;
    fasttype *grp;
    fasthash *acc, *next;
    fastitem *item, *nextitem;

    if (!acl)
	return;

    if ((acl->type != ACL_IP_SIMPLE) &&
	(acl->type != ACL_IP_NAMED_SIMPLE))
	return;

    if (acl->type == ACL_IP_SIMPLE && !acl->list)
        return;

    printf("\nStandard IP access list %s", acl->name);

    grp = (fasttype *) acl->list;
    if (!grp)
	return;

    /*
     * First display the hash table of individual entries
     */
    for (i = 0; i < NETHASHLEN; i++) {
	if (automore_quit())
	    return;
	if (grp != (fasttype *) acl->list)
	    break;
	acc = grp->hashlist[i];
	while (acc) {
	    mem_lock(acc);
	    printf("\n    %s %i",
		   acc->grant ? "permit" : "deny  ", acc->address);
	    next = acc->next;
	    free(acc);
	    acc = next;
	}
    }

    /*
     * Now display the wildcarded entries
     */
    if (grp != (fasttype *) acl->list)
	return;
    
    item = grp->items;
    while (item) {
	if (automore_quit())
	    return;
	mem_lock(item);
	printf("\n    %s ",
	       item->grant ? "permit" : "deny  ");
	if ((item->permitmask == ANY_HOST) &&
	    (item->wildmask == WILD_ANY))
	    printf("any");
	else 
	    printf("%i, wildcard bits %i", item->permitmask,
		   item->wildmask); 
	nextitem = item->next;
	free(item);
	item = nextitem;
    }
}

/*
 * show_ip1access
 * Show contents of access lists.  If argument is non-NULL
 * show that particular access list, else show all access lists.
 */

void show_ip1access (acl_headertype *acl)
{
    acl_headertype *tacl;
    list_element *el;
    int j;

    if (acl) {
	ip1access_printlist(acl);
	return;
    }

    for (j = MINFASTACC; j <= MAXFASTACC; j++)
	ip1access_printlist(acl_array[j]);

    FOR_ALL_DATA_IN_LIST(&nacl_list, el, tacl) {
	ip1access_printlist(tacl);
    }

}

/*
 * show_ip_access
 * Show IP access lists
 */

void show_ip_access (parseinfo *csb)
{
    int list;
    char *name;
    acl_headertype *acl;

    automore_enable(NULL);

    list = GETOBJ(int, 1);
    name = GETOBJ(string, 1);

    if (list)			/* Number specified */
	acl = acl_array[list];
    else if (name && strlen(name)) 		/* Name specified */
	acl = access_find_nacl(name);
    else {			/* Show Everything */
	show_ip1access(NULL);
	show_ip2access(NULL);
	automore_disable();
	return;
    }

    if (acl) {
	switch (acl->type) {
	case ACL_IP_SIMPLE:
	case ACL_IP_NAMED_SIMPLE:
	    show_ip1access(acl);
	    break;

	case ACL_IP_EXTENDED:
	case ACL_IP_NAMED_EXTENDED:
	    show_ip2access(acl);
	    break;

	default:
	    break;
	}
    }
    automore_disable();
}

/*
 * clear_ip_access_stats
 *
 * Clear all ACL counters for a given list (0 = all lists)
 */

void clear_ip_access_stats (acl_headertype *acl)
{
    accesslog_cache_flush(acl);
    clear_ip2_access_stats(acl);
}

/*
 * clear_ip_access
 * Clear IP access lists
 */

void clear_ip_access (parseinfo *csb)
{
    int list, number;
    char *name;
    acl_headertype *acl;

    list = GETOBJ(int, 1);
    name = GETOBJ(string, 1);
    if (name && isdigit(name[0])) {
	number = atoi(name);
	if ((number >= MINFASTACC) && (number <= MAXSLOWACC)) {
	    list = number;
	    name = NULL;
	}
    }

    if (list) {			/* Number specified */
	acl = acl_array[list];
    } else if (name && strlen(name)) { 		/* Name specified */
	acl = access_find_nacl(name);
    } else {
	acl = NULL;
    }

    if (acl) {
	clear_ip_access_stats(acl);
    }
}

