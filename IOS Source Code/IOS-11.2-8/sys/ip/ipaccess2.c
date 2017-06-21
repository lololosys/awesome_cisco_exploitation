/* $Id: ipaccess2.c,v 3.6.20.9 1996/09/09 22:17:06 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaccess2.c,v $
 *------------------------------------------------------------------
 * ipaccess2.c -- Extended ("Slow") IP Access Lists
 *
 * 7-May-1987, Kirk Lougheed
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Note: Named access lists rely on some of these functions.  Be sure
 * to update ip_nacl.c to reflect any changes made here.
 *
 *------------------------------------------------------------------
 * $Log: ipaccess2.c,v $
 * Revision 3.6.20.9  1996/09/09  22:17:06  ahh
 * CSCdi68478:  clear access-list counters not working
 * Remove parse-chain ambiguity and allow NULL to be passed into the
 * access_clear registry to clear the counters on all access-lists.
 * Branch: California_branch
 *
 * Revision 3.6.20.8  1996/07/20  01:13:14  bew
 * CSCdi63411:  ACL encrypted/decrypted flags are not useful
 * Branch: California_branch
 * Remove encrypted/decrypted flags introduced early in 11.2
 * from ACL code.
 *
 * Revision 3.6.20.7  1996/06/28  23:17:32  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.20.6  1996/06/18  20:43:41  sbelair
 * CSCdi42319:  Lock&Key idle timer does not work correctly
 * Branch: California_branch
 *
 * Add a timestamp to the slowtype structure for use with dynamic access
 * lists. Record timestamp whenever there is a hit on the access list and
 * base the idle time expiration off  of  this time of last reference.
 *
 * Revision 3.6.20.5  1996/05/21  09:51:13  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.20.4  1996/05/08  01:09:43  gchristy
 * CSCdi56944:  Compilation errors from IP named access list commit
 * Branch: California_branch
 * - Modularize new ACL code for inclusion in VIP images.
 *
 * Revision 3.6.20.3  1996/05/04  01:02:52  wilber
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
 * Revision 3.6.20.2  1996/04/19  15:41:20  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.12  1996/04/17  23:01:38  carrel
 * Branch: IosSec_branch
 * Final commit from IP code review comments
 *
 * Revision 2.1.6.11  1996/04/11  23:14:46  carrel
 * Branch: IosSec_branch
 * IP code review comments #1
 *
 * Revision 2.1.6.10  1996/04/09  20:36:30  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.9  1996/03/30  05:19:01  carrel
 * cvs fixes
 *
 * Revision 2.1.6.8  1996/02/21  03:29:29  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.7  1996/02/13  19:17:54  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.1.6.6  1996/01/23  05:42:56  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.5  1996/01/04  19:02:13  xliu
 * Branch: IosSec_branch
 * Replaced the direct crypto system call as the crypto subsys registry
 * call.
 *
 * Revision 2.1.6.4  1995/12/19  03:56:14  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.3  1995/11/17  17:00:26  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.2  1995/10/10  18:19:59  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.6.1  1995/09/26  04:12:57  carrel
 * Branch: IosSec_branch
 * Initial commit of encryption ACL code.
 *
 * Revision 3.6.20.1  1996/03/18  20:16:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/07  09:46:01  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  00:59:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/05  19:17:33  rlfs
 * CSCdi46747:  Lock&key router hangs up with a show access-list
 *
 * Revision 3.5  1996/02/01  06:04:22  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/20  16:35:54  rlfs
 * CSCdi47234:  Lock&Key may remove all ACLs unnecessarily
 *
 * Revision 3.3  1995/12/17  18:29:34  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:34:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:04:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/23  11:50:47  tli
 * CSCdi39192:  IP: bug in UDP extended access lists
 *
 * Revision 2.3  1995/08/15  23:25:19  tli
 * CSCdi36981:  Need to be able to generate downward compatible
 * configurations
 *
 * Revision 2.2  1995/07/12  07:18:42  tli
 * CSCdi36426:  Policy routing will not disable on an interface
 * - Policy routed packets were creating fastswitching cache entries.
 *   That's not till _next_ release.
 * - Add counters to record packet and byte count on a per route map entry
 *   basis.
 * - Marketing identified a catastrophic deficiency.  Implemented "set ip
 *   tos" and "set ip precedence" to cover this deficiency.
 *
 * Revision 2.1  1995/06/07  20:57:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "ip.h"
#include "../tcp/tcp.h"
#include "access.h"
#include "ipaccess.h"
#include "ipaccess_private.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_defs_access.h"
#include "../ipmulticast/parser_defs_igmp.h"
#include "parser_defs_ip.h"
#include "sys_registry.h"
#include "ip_registry.h"
#include "../ipmulticast/igmp.h"
#include "logger.h"
#include "../crypto/crypto_public.h"
#include "ipaccess_inline.h"
#include "mgd_timers.h"
#include "ttysrv.h"
#include "connect.h"
#include "../parser/parser_actions.h"
#include "ip_actions.h"
#include "ipaccess2.h"

/*
 * In 10.3, we introduced a major overhaul to use keywords wherever
 * possible.  We also added many capabilities.
 */

#define KEYWORD_MAJOR_VER 10
#define KEYWORD_MINOR_VER  3

/*
 * Local Storage
 */


static dynlisttype    *ip_dynlist = NULL;
static mgd_timer      dynacl_master_timer;
static mgd_timer      dynacl_idle_timer;
static mgd_timer      dynacl_max_timer;
static boolean	      dynacl_inited = FALSE;



/*
 * get_ipacl
 * Given an access-list number, return a pointer to the list. Returns
 * NULL if list doesn't exist, or the number is out of range.
 */
accessptr *
ip_getacl(int num)
{
    return(ip_getacl_inline(num));
}


/*
 * ip_old_accesscheck
 * This is a wrapper function for functions that still use numbered
 * ACLs.
 */

boolean ip_old_accesscheck (paktype *pak, int list)
{
    acl_headertype *acl;

    if ((list < MINFASTACC) || (list > MAXSLOWACC))
	return(TRUE);

    acl = acl_array[list];

    return(ip_accesscheck(pak, acl));
}

/*
 * ip_count_violations
 * Determine which interface the violation occurred on and logs the 
 * violation if ip_violations_acctg is TRUE for that interface.
 */

extern inline void ip_count_violations (paktype *pak,
					acl_headertype *list) 
{
    idbtype *idb;
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    idb = pak->if_input;
    if ((idb != NULL) && (idb->ip_input_accesslist == list)) {
	if (idb->ip_violation_acctg)
	    ip_accumulate_acctg(ip, list);
	return;
    }

    idb = pak->if_output;
    if ((idb != NULL) && (idb->ip_output_accesslist == list)) {
	if (idb->ip_violation_acctg)
	    ip_accumulate_acctg(ip, list);
	return;
    }
}

/*
 * ip_acc_ck_count_violations
 * Calls ip_accesscheck_snpa to collect source MAC info and perform an
 * access check on a routed IP datagram, then updates IP violation 
 * accounting database if access check fails.
 * Returns TRUE to send datagram, FALSE otherwise.
 */

boolean ip_acc_ck_count_violations (paktype *pak, acl_headertype *acl)
{
    if (ip_accesscheck_snpa(pak, acl, onintstack()))
	return(TRUE);
    else {
	ip_count_violations(pak, acl);
	return(FALSE);
    }
}

/*
 * A whole battery of functions to deal with the DYNAMIC access-lists
 */

/*
 * Search for a DYNAMIC access-list by it's name
 *
 */
dynlisttype *ipaccess_get_listbyname (char *name)
{
    dynlisttype *dynacl;

    dynacl = ip_dynlist;
    if (!dynacl) {
	return (NULL);
    }
    while (dynacl) {
    	if (!bcmp(name, dynacl->aclname, ACLNAME_LEN)) {
	    return(dynacl);
	}
	dynacl = dynacl->next;
    }
    return(NULL);
}
/*
 * ipaccess_create_dynacl:
 *	Create a new Nested Access-List.
 */
static dynlisttype *ipaccess_create_dynacl (char *name, uint timeout)
{
    dynlisttype *nacl;

    nacl = NULL;
    nacl = ipaccess_get_listbyname(name);
    if (nacl) {
	nacl->refcnt++;
	return(nacl);		/* Already have a list by that name */
    }
    nacl = malloc(sizeof(dynlisttype));
    nacl->refcnt++;
    nacl->timeout = timeout;
    bcopy(name, nacl->aclname, ACLNAME_LEN);
    nacl->item = NULL;			/* Nothing in list yet. */
    /*
     * Link it into our list of 'known' temp ACLs
     */
    nacl->next = ip_dynlist;
    ip_dynlist = nacl;

    return(nacl);
}

/*
 * Remove specific item entry from a linked singly linked list.
 */
static void ip_dynacl_remove (slowtype *item)
{
    slowtype *nitem;

    /*
     * Stop all timers for this item.
     */
    mgd_timer_stop(&item->dynmax_timer);
    mgd_timer_stop(&item->dynacl_timer);

    if (item == item->dynacl_p->item) {
	    item->dynacl_p->item = item->next;	
    } else {
        nitem = item->dynacl_p->item;
        while (nitem) {
  	    if (nitem->next == item) {
	        nitem->next = item->next;
	        item->next = NULL;
	        break;
	    }
	    nitem = nitem->next;
	}
    }
}

/*
 * ipaccess_remove_dynacl:
 *	Remove a dynamic access-list
 */
void ipaccess_remove_dynacl (dynlisttype *dynacl)
{
    slowtype *item, *next_item;
    dynlisttype *lacl;

    if(!dynacl) {
	return;
    }

    /*
     * Free up all entries that belong to this dynamic stub
     */
    item = dynacl->item;
    while (item) {
	next_item = item->next;
	if (item->dynacl_p == dynacl) {
	    ip_dynacl_remove(item);
	    free(item);
	} 
	item = next_item;
    }

    /*
     * Dec. the ref. count on this dynamic list.  If ZERO, free up the
     * list.
     */
    --dynacl->refcnt;
    if (dynacl->refcnt) {
	return;
    }

    /*
     * Unlink from well 'known' list
     */
    if (ip_dynlist == dynacl) {
	ip_dynlist = dynacl->next;
    } else {
	lacl = ip_dynlist;
	while (lacl) {
	    if (lacl->next == dynacl) {
		lacl->next = dynacl->next;
		break;
	    }
	    lacl = lacl->next;
	}
    }
    free(dynacl);		/* Free up the memory now. */
}

/*
 * ip_dynacl_periodic()
 *	One min. periodic to clean up the dynamic list.  Flush out
 *	expired and idle entries.
 *	We handle 2 types of timers here.  Absolute time and idle-time.
 *	The Absolute timer is responsible for terminating an ACL within
 *	a certain time period, regardless of activity.
 *	The Idle-Timer is responsible for terminating an ACL that has
 *	not taken a hit within a specified time period(inactive).
 *
 * 	Managed timers are used to reduce the one-minute system
 *	overhead.
 */
static void ip_dynacl_periodic (void)
{
    mgd_timer *expired_timer;
    slowtype *item;

    /*
     * First we check the idle timers.  Remove the expired entries.
     */
    while (mgd_timer_expired(&dynacl_idle_timer)) {
	expired_timer = mgd_timer_first_expired(&dynacl_idle_timer);
	item = mgd_timer_context(expired_timer);
	mgd_timer_stop(expired_timer);
	if (!item) {
	    continue;
	}
	/*
	 * Do expiry chores
	 * If we've had hits, then restart the timer.  Otherwise, put this
	 * entry to rest.  It's been idle.
	 */
        if (item->hits != item->sav_hits) {

            int delta;

            delta = (item->timeout * ONEMIN) - ELAPSED_TIME(item->lastref);
            if (delta < 0)
                delta = item->timeout * ONEMIN;
            item->sav_hits = item->hits;
            /*
             * Start counting the idle timeout interval from
             * the time of the last reference, not from this
             * time of evaluation, unless the elapsed time
	     * has exceeded the idle timeout.
             */
            mgd_timer_start(&item->dynacl_timer, delta);
	    continue;
	}
	/*
	 * This entry has idled out.... Remove it.
	 */
	ip_dynacl_remove(item);
	reg_invoke_ip_access_list(item->acl);
	free(item);
    }
    /*
     * Now scan for the absolute timers.  Remove those that have 
     * expired their time limit
     */
    while (mgd_timer_expired(&dynacl_max_timer)) {
	expired_timer = mgd_timer_first_expired(&dynacl_max_timer);
	item = mgd_timer_context(expired_timer);
	mgd_timer_stop(expired_timer);
	if (!item) {
	    continue;
	}
	/*
	 * This entry has idled out.... Remove it.
	 */
	ip_dynacl_remove(item);
	reg_invoke_ip_access_list(item->acl);
	free(item);
    }
}

/*
 * ip_dynacl_init():
 *	Called to start the timers.  May get called many times, but will
 *	only start timers on first call
 */
static void ip_dynacl_init (void)
{
    if(!dynacl_inited) {
	mgd_timer_init_parent(&dynacl_master_timer, NULL);
    	mgd_timer_init_parent(&dynacl_idle_timer, &dynacl_master_timer);
    	mgd_timer_init_parent(&dynacl_max_timer, &dynacl_master_timer);
	reg_add_onemin(ip_dynacl_periodic, "Dynamic Access-List periodic");
	dynacl_inited = TRUE;
    }
}

/*
 * ip_tempacl_additem()
 */
static boolean ip_tempacl_additem(
    dynlisttype *dynacl,
    slowtype *item,
    ipaddrtype source, 
    ipaddrtype srcmask,
    ipaddrtype destination,
    ipaddrtype destmask,
    uint timeout, 
    uint max_timeout,
    acl_headertype *acl)
{
    slowtype *ditem;

    ditem = dynacl->item;
    while (ditem) {
	if ((ditem->grant == item->grant) &&
	    (ditem->source == source) &&
	    (ditem->srcmask == srcmask) &&
	    (ditem->destination == destination) &&
	    (ditem->dstmask == destmask) &&
	    (ditem->protocol == item->protocol) &&
	    (ditem->established == item->established) &&
	    (ditem->dstopr == item->dstopr) &&
	    (ditem->dport == item->dport) &&
	    (ditem->dport2 == item->dport2) &&
	    (ditem->srcopr == item->srcopr) &&
	    (ditem->sport == item->sport) &&
	    (ditem->sport2 == item->sport2) &&
	    (ditem->precedence == item->precedence) &&
	    (ditem->precedence_value == item->precedence_value) &&
	    (ditem->tos == item->tos) &&
	    (ditem->tos_value == item->tos_value) &&
	    (ditem->log == item->log)) {
	    printf("\nList#%s-%s already contains this IP address pair",
			acl->name, dynacl->aclname);
	    return(FALSE);
	}
	ditem = ditem->next;
    }
    /*
     * Construct the item descriptor
     */
    ditem = malloc(sizeof(slowtype));
    if (!ditem) {
	printf(nomemory);
	return(FALSE);
    }
    bcopy(item, ditem, sizeof(slowtype));
    /*
     * set item specifics
     */
    ditem->dynamic = FALSE;		/* Limit to 1 level only */
    ditem->source = source;
    ditem->srcmask = srcmask;
    ditem->destination = destination;
    ditem->dstmask = destmask;
    ditem->timeout = timeout;
    ditem->acl = acl;
    ditem->dynacl_p = dynacl;	/* Point back to parent */
    /*
     * Set the Idle timer as necessary
     */
    mgd_timer_init_leaf(&ditem->dynacl_timer, &dynacl_idle_timer,
		0, ditem, TRUE);
    /*
     * Set Absolute timer as necessary
     */
    mgd_timer_init_leaf(&ditem->dynmax_timer, &dynacl_max_timer,
		0, ditem, TRUE);
    if (timeout) {
    	mgd_timer_start(&ditem->dynacl_timer, timeout*ONEMIN);
    }
    if (item->timeout) {
    	mgd_timer_start(&ditem->dynmax_timer, max_timeout*ONEMIN);
    }
    /*
     * Link it into the temp list.
     */
    ditem->next = dynacl->item;
    dynacl->item = ditem;
    /*
     * Update the Access-list (Rebuild SSE tree)
     */
    reg_invoke_ip_access_list(item->acl);
    return(TRUE);
}

/*
 * ip_accena_command:
 * 	access-enable
 *
 *	Called to enable access from a unpriviledged mode on a particular
 *	VTY.  
 */
void ip_accena_command (parseinfo *csb)
{
    uint timeout;
    slowtype *item;
    dynlisttype *dynacl;
    ipaddrtype source, destination, srcmask, destmask, host;
    ipaddrtype orig_source;
    idbtype *inidb;
    boolean hostonly;
    acl_headertype *acl;

    if (stdio->type != VTY_LINE) {
	printf("\nCommand allowed from VTY connections only");
	return;
    }

    ip_dynacl_init();			/* Init as necessary */

    host = orig_source = source = stdio->conntop->address.addr.ip_address;
    timeout = GETOBJ(int,2);
    hostonly = GETOBJ(int,1);
    inidb = reg_invoke_ip_first_hop(&source, source, IP_PATH_DEFFLAGS);
    if (!inidb) {
	printf("\nNo interface present for this address %i", source);
	return;
    }
    /*
     * Process input ACL entries first
     */
    dynacl = NULL;
    acl = inidb->ip_input_accesslist;

    if (!acl) {
	printf("\nNo input access group defined for %s.",
	       inidb->namestring);
	return;
    }

    switch (acl->type) {
    case ACL_IP_NAMED_EXTENDED:
    case ACL_IP_EXTENDED:
	break;

    default:
	printf("\n Interface %s does not have an extended list"
	       " defined.", inidb->namestring);
	return;
    }
    
    item = (slowtype *) inidb->ip_input_accesslist->list;

    while (item) {
	if (item->dynamic) {
	    dynacl = item->dynacl_p;
	    break;
	}
	item = item->next;
    }
    if (!dynacl) {
	printf("\nNo DYNAMIC list for %s", inidb->namestring);
	return;
    }
    if (hostonly && ((orig_source & ~item->srcmask) != item->source)) {
	printf("\nSource %i is not in mask(%i, %i) in the ACL",
			orig_source, item->source, item->srcmask);
	return;
    }
    srcmask = WILD_HOST;		/* host <ip.addr> format */
    /*
     * If !hostonly, then we want to enable a network.  Mask out the
     * appropriate bits and continue;
     */
    if(hostonly == FALSE) {
	srcmask = item->srcmask;
	host = orig_source & ~item->srcmask;
    }
    destination = item->destination;
    destmask = item->dstmask;
    ip_tempacl_additem(dynacl, item, host, srcmask, 
		destination, destmask, timeout, item->timeout, acl);
    /*
     * Process output side of ACLs
     */
    dynacl = NULL;
    if (!inidb->ip_output_accesslist)
	return;

    item = (slowtype *)inidb->ip_output_accesslist->list;
    while (item) {
	if (item->dynamic) {
	    dynacl = item->dynacl_p;
	    break;
	}
	item = item->next;
    }
    if (!dynacl) {
	return;
    }
    if (hostonly && ((orig_source & ~item->dstmask) != item->destination)) {
	printf("\nDestination %i is not in mask(%i, %i) in the ACL",
			orig_source, item->destination, item->dstmask);
	return;
    }
    destmask = WILD_HOST;
    /*
     * If !hostonly, then we want to enable a network.  Mask out the
     * appropriate bits and continue;
     */
    if(hostonly == FALSE) {
	destmask = item->dstmask;
	host = orig_source & ~item->dstmask;
    }
    source = item->source;
    srcmask = item->srcmask;
    destination = host;
    ip_tempacl_additem(dynacl, item, source, srcmask, 
		destination, destmask, timeout, item->timeout, acl);
}

/*
 * ip_tempacl_command:
 *	access-dynamic <acl #> <name> ip <src> <dest> [timeout <mins.>]
 *
 * Called to enable temporary access to a src/dest pair.  The entry
 * is added into the dynamic access-list and will remain.
 */
void ip_tempacl_command (parseinfo *csb)
{
    uint aclnum, timeout;
    slowtype *item;
    dynlisttype *dynacl;
    ipaddrtype source, destination, srcmask, destmask;
    char *aclname;
    acl_headertype *acl;

    ip_dynacl_init();			/* Init as necessary */
    aclnum = GETOBJ(int,1);
    aclname = GETOBJ(string, 1);
    timeout = GETOBJ(int,2);
    source = GETOBJ(paddr,1)->ip_addr;
    srcmask = GETOBJ(paddr,2)->ip_addr;
    destination = GETOBJ(paddr,3)->ip_addr;
    destmask = GETOBJ(paddr,4)->ip_addr;

    if (aclnum) {
	acl = acl_array[aclnum];
	if (!acl) {
	    printf("\nAccess list %d does not exist", aclnum);
	    return;
	}
    } else {
	acl = access_find_nacl(aclname);
	if (!acl) {
	    printf("\nAccess list %s does not exist", aclname);
	    return;
	}
    }

    switch (acl->type) {
    case ACL_IP_NAMED_EXTENDED:
    case ACL_IP_EXTENDED:
	break;

    default:
	printf("\n Specified access-list does not support dynamic"
	       " lists."); 
	return;
    }

    item = (slowtype *) acl->list;
    dynacl = ipaccess_get_listbyname(GETOBJ(string,2));
    if (!dynacl) {
	printf("\nDynamic list %s does not exist", GETOBJ(string,2));
	return;
    }
    while (item) {
	if (item->dynacl_p == dynacl) {
	    break;
	}
	item = item->next;
    }
    if (!item) {
	printf("\nDynamic list %s is not in access-list %d",
		GETOBJ(string,2), aclnum);
	return;
    }
    if ((source & ~item->srcmask) != item->source) {
	printf("\nSource %i is not in mask(%i, %i) in the ACL",
			source, item->source, item->srcmask);
	return;
    }
    if ((destination & ~item->dstmask) != item->destination) {
	printf("\nDestination %i is not in mask(%i, %i) in the ACL",
			destination, item->destination, item->dstmask);
	return;
    }

    ip_tempacl_additem(dynacl, item, source, srcmask, destination, destmask,
				timeout, item->timeout, acl);
}

/*
 * Clear_tempacl_command:
 *	Called to manually remove access
 */
void clear_tempacl_command (parseinfo *csb)
{
    uint aclnum;
    char *aclname;
    int  srchost, desthost;
    slowtype *item, *ditem;
    dynlisttype *dynacl;
    ipaddrtype source, destination, srcmask, destmask;
    acl_headertype *acl;

    ip_dynacl_init();			/* Init as necessary */
    aclnum = GETOBJ(int,1);
    aclname = GETOBJ(string,1);
    srchost = GETOBJ(int,2);
    desthost = GETOBJ(int,3);
    source = GETOBJ(paddr,1)->ip_addr;
    srcmask = GETOBJ(paddr,2)->ip_addr;
    destination = GETOBJ(paddr,3)->ip_addr;
    destmask = GETOBJ(paddr,4)->ip_addr;

    if (srchost)
	srcmask = WILD_HOST;
    else if (source == WILD_ANY)
	srcmask = WILD_ANY;
    if (desthost)
	destmask = WILD_HOST;
    else if (destination == WILD_ANY)
	destmask = WILD_ANY;
    
    if (aclnum) {
	acl = acl_array[aclnum];
	if (!acl) {
	    printf("\nAccess list %d does not exist", aclnum);
	    return;
	}
    } else {
	acl = access_find_nacl(aclname);
	if (!acl) {
	    printf("\nAccess list %s does not exist", aclname);
	    return;
	}
    }

    switch (acl->type) {
    case ACL_IP_NAMED_EXTENDED:
    case ACL_IP_EXTENDED:
	break;

    default:
	printf("\n Specified access-list does not support dynamic "
	       "lists."); 
	return;
    }

    item = (slowtype *) acl->list;
    dynacl = ipaccess_get_listbyname(aclname);
    if (!dynacl) {
	printf("\nDynamic list %s does not exist", aclname);
	return;
    }
    while (item) {
	if (item->dynacl_p == dynacl) {
	    break;
	}
	item = item->next;
    }
    if (!item) {
	printf("\nDynamic list %s is not in access-list %s",
		aclname, acl->name);
	return;
    }
    ditem = dynacl->item;
    while (ditem) {
	if ((ditem->grant == item->grant) &&
	    ((ditem->source) ? (ditem->source == source) : 
	     (source == WILD_ANY)) &&
	    ((ditem->srcmask == WILD_HOST) || (ditem->srcmask == srcmask)) &&
	    ((ditem->destination) ? (ditem->destination == destination) :
	     (destination == WILD_ANY)) &&
	    ((ditem->dstmask ==  WILD_HOST) || (ditem->dstmask == destmask)) &&
	    (ditem->protocol == item->protocol) &&
	    (ditem->established == item->established) &&
	    (ditem->dstopr == item->dstopr) &&
	    (ditem->dport == item->dport) &&
	    (ditem->dport2 == item->dport2) &&
	    (ditem->srcopr == item->srcopr) &&
	    (ditem->sport == item->sport) &&
	    (ditem->sport2 == item->sport2) &&
	    (ditem->precedence == item->precedence) &&
	    (ditem->precedence_value == item->precedence_value) &&
	    (ditem->tos == item->tos) &&
	    (ditem->tos_value == item->tos_value) &&
	    (ditem->log == item->log)) {
	    break;
	}
	ditem = ditem->next;
    }
    if (!ditem) {
	printf("\nList %s-%s doesn't contain this IP address pair",
			acl->name, dynacl->aclname);
	return;
    }
    /*
     * Now unlink this entry from this dynamic list
     */
    ip_dynacl_remove(ditem);
    free(ditem);
}

/*
 * ip2access_command
 * Parse an extended IP access-list command line.
 *
 *  "access-list <list> [dynamic <name>] permit|deny <protocol> 
 *				<src> <srcmask> <dst> <dstmask>
 *				[<operator> <operand>] [ log ]
 */


/*
 * ip2access_nvgen
 * NVGEN an extended ACL
 */

void ip2access_nvgen (parseinfo *csb, acl_headertype *acl)
{
    slowtype *item;

    if (!acl)
	return;

    item = (slowtype  *) acl->list;

    while (item) {
	nv_write(TRUE,"%s %s ", 
	       csb->nv_command, acl->name);
	nv_add(item->dynamic, "dynamic %s ",
	       item->dynacl_p->aclname);
	nv_add(item->timeout, "timeout %d ", item->timeout);
	nv_add(TRUE, "%s ", item->grant ? "permit" : "deny  ");
	if (config_before(KEYWORD_MAJOR_VER, KEYWORD_MINOR_VER))
	    nv_add_id_string(old_proto_items, item->protocol);
	else
	    nv_add_id_string(proto_items, item->protocol);
	
	if (config_before(KEYWORD_MAJOR_VER, KEYWORD_MINOR_VER)) {
	    nv_add(TRUE, " %i %i", item->source, item->srcmask);
	} else {
	    if ((item->source == ANY_HOST) &&
		(item->srcmask == WILD_ANY))
		nv_add(TRUE, " any");
	    else if (item->srcmask == WILD_HOST)
		nv_add(TRUE, " host %i", item->source);
	    else 
		nv_add(TRUE, " %i %i", item->source, item->srcmask);
	}	    
	if (item->srcopr != OPC_NONE) {
	    config_atleast(csb, KEYWORD_MAJOR_VER, KEYWORD_MINOR_VER);
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

	if (config_before(KEYWORD_MAJOR_VER, KEYWORD_MINOR_VER)) {
	    nv_add(TRUE, " %i %i", item->destination,
		   item->dstmask);
	} else {
	    if ((item->destination == ANY_HOST) &&
		(item->dstmask == WILD_ANY))
		nv_add(TRUE, " any");
	    else if (item->dstmask == WILD_HOST)
		nv_add(TRUE, " host %i", item->destination);
	    else 
		nv_add(TRUE, " %i %i", item->destination,
		       item->dstmask);
	}

	if (item->dstopr != OPC_NONE) {
	    nv_add(TRUE, " ");
	    switch (item->protocol) {
	    case TCP_PROT:
		nv_add_id_string(ip2access_cmds, item->dstopr);
		nv_add(TRUE, " ");
		if (config_before(KEYWORD_MAJOR_VER,
				  KEYWORD_MINOR_VER))
		    nv_add(TRUE, "%d", item->dport);
		else
		    nv_add_id_string(tcp_ports, item->dport);
		break;
	    case UDP_PROT:
		nv_add_id_string(ip2access_cmds, item->dstopr);
		nv_add(TRUE, " ");
		if (config_before(KEYWORD_MAJOR_VER,
				  KEYWORD_MINOR_VER))
		    nv_add(TRUE, "%d", item->dport);
		else
		    nv_add_id_string(udp_ports, item->dport);
		break;
	    case ICMP_PROT:
		if (item->dstopr == OPC_ONEBYTE) {
		    nv_add_id_string(icmptypes, item->dport);
		    config_atleast(csb, KEYWORD_MAJOR_VER,
				   KEYWORD_MINOR_VER); 
		} else if (item->dstopr == OPC_TWOBYTES) {
		    nv_add_id_subid_string(icmptypecodes,
					   item->dport,
					   item->dport2);
		    config_atleast(csb, KEYWORD_MAJOR_VER,
				   KEYWORD_MINOR_VER); 
		}
		break;
	    case IGMP_PROT:
		if (item->dstopr == OPC_ONEBYTE) {
		    nv_add_id_string(igmptypes, item->dport & 0xf);
		    config_atleast(csb, KEYWORD_MAJOR_VER,
				   KEYWORD_MINOR_VER); 
		}
		break;
	    default:
		break;
	    }
	    if (item->dstopr == OPC_RANGE) {
		config_atleast(csb, KEYWORD_MAJOR_VER,
			       KEYWORD_MINOR_VER); 
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
	    config_atleast(csb, KEYWORD_MAJOR_VER,
			   KEYWORD_MINOR_VER); 
	    nv_add(TRUE, " tos ");
	    nv_add_id_string(tos_values, item->tos_value >>
			     IP_TOS_SHIFT); 
	}
	
	switch (item->log) {
	case IP_ACCESS_LOG_NORMAL:
	    nv_add(TRUE, " log");
	    config_atleast(csb, KEYWORD_MAJOR_VER,
			   KEYWORD_MINOR_VER);
	    break;

	case IP_ACCESS_LOG_INPUT:
	    nv_add(TRUE, " log-input");
	    config_atleast(csb, KEYWORD_MAJOR_VER,
			   KEYWORD_MINOR_VER);
	    break;

	default:
	    break;

	}
	item = item->next;
    }
}

void ip2access_deletelist (acl_headertype *acl)
{
    slowtype *item, *wrk_item;

    if (!acl)
	return;

    item = (slowtype *) acl->list;
    acl->list = NULL;
    acl->tp_header_required = FALSE;
    
    while (item) {
	reg_invoke_ip_send_acl_change_ipc(FALSE, acl, NULL, NULL, item);
	wrk_item = item->next;
	/*
	 * If this is a DYNAMIC list type, we must also clean up
	 * the list.  Only if refcnt is 1
	 */
	if (item->dynamic && item->dynacl_p) {
	    ipaccess_remove_dynacl(item->dynacl_p);
	}
	item->next = NULL;
	free(item);
	item = wrk_item;
    }
    reg_invoke_ip_access_list(acl);

}

/*
 * ip2access_parselist
 * Parse and generate an extended ACL.
 */
void ip2access_parselist (parseinfo *csb, acl_headertype *acl)
{
    uint protocol, opr, srcopr, precedence_value, tos_value;
    boolean grant, new, established, precedence, tos, log;
    ipaddrtype source, destination, srcmask, dstmask;
    ulong dport, dport2, sport, sport2, i;
    slowtype *item, *wrk_item;
    dynlisttype *dynacl;
    boolean dynamic;
    uint timeout;

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
     * Create the list, make it ready for use.
     */
    dynamic = GETOBJ(int,16);
    timeout = GETOBJ(int,17);
    dynacl = NULL;
    if (dynamic) {
	dynacl = ipaccess_create_dynacl(GETOBJ(string,1), timeout);
	if (!dynacl) {
	    /*
	     * Unable to create this DYNAMIC access list 
	     */
	    return;
	}
    }

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
    wrk_item = (slowtype *) acl->list;
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
	    (item->log == log))
	    return;
	wrk_item = item;
	item = item->next;
    }
    
    /*
     * Construct the item descriptor
     */
    item = malloc(sizeof(slowtype));
    if (!item) {
	printf(nomemory);
	if (dynacl) {
	    ipaccess_remove_dynacl(dynacl);
	}
	return;
    }
    item->grant = grant;
    item->source = source;
    item->srcmask = srcmask;
    item->destination = destination;
    item->dstmask = dstmask;
    item->protocol = protocol;
    item->established = established;
    item->dstopr = opr;
    item->dport = dport;
    item->dport2 = dport2;
    item->srcopr = srcopr;
    item->sport = sport;
    item->sport2 = sport2;
    item->precedence = precedence;
    item->precedence_value = precedence_value;
    item->tos = tos;
    item->tos_value = tos_value;
    item->log = log;
    item->dynamic = dynamic;
    item->timeout = timeout;
    item->dynacl_p = dynacl;

    /*
     * Pre-evaluate some things about this item for quicker testing at
     * switching time.
     */
    item->fancy = (srcopr != OPC_NONE) | precedence | tos;
    if ((srcopr != OPC_NONE) || (opr != OPC_NONE) || established) {
	/*
	 * This access list needs to check transport layer header.
	 * Note that this also correctly snags non-transport protocols such
	 * as IGMP or ICMP.
	 */
	acl->tp_header_required = TRUE;
    }
    new = FALSE;
    if (!wrk_item) {
	acl->list = (accessptr *) item;
	new = TRUE;
    } else 
	wrk_item->next = item;

    reg_invoke_ip_send_acl_change_ipc(TRUE, acl, NULL, NULL, item);
    reg_invoke_ip_access_list(acl);
}


void ip2access_command (parseinfo *csb)
{
    int number, i;
    acl_headertype *acl;
    
    if (csb->nvgen) {
	for (i = MINSLOWACC; i <= MAXSLOWACC; i++) {
	    acl = acl_array[i];
	    ip2access_nvgen(csb,acl);
	}

	return;
    }

    /* Get list number and permit/deny flag */
    number = GETOBJ(int, 1);

    /*
     * If parsing "no access-list <list>", stop now to delete list.
     */
    if (csb->sense == FALSE) {
	acl = acl_array[number];
	ip2access_deletelist(acl);
	return;
    }

    acl = access_find_or_create_acl(number);
    if (!acl) 
	return;

    ip2access_parselist(csb, acl);

}

static void ip2access_printlist (acl_headertype *acl)
{
    slowtype *item, *next, *inclitem;
    ulong hits;

    if (!acl)
	return;

    if ((acl->type != ACL_IP_EXTENDED) &&
	(acl->type != ACL_IP_NAMED_EXTENDED))
	return;

    if (acl->type == ACL_IP_EXTENDED && !acl->list)
        return;

    printf("\nExtended IP access list %s", acl->name);

    item = (slowtype *) acl->list;
    if (!item) 
        return;

    inclitem = NULL;
    while (item) {
	if (automore_quit()) {
	    if (inclitem) 
		free(inclitem);	/* It was memlock'ed before */
	    return;
	}
	mem_lock(item);
	if (!item->dynamic) {
	    if (!inclitem) {
		printf("\n    %s ", item->grant ? "permit" : "deny  ");
	    } else {
		printf("\n       %s ", 
		       item->grant ? "permit" : "deny  ");
	    }
	} else {
	    printf("\n    Dynamic %s", item->dynacl_p->aclname);
	    if (item->timeout) {
		printf(" Max. %d mins.", item->timeout);
	    }
	    printf(" %s ", item->grant ? "permit" : "deny  ");
	}
	printf_id_string(proto_items, item->protocol);
	
	if ((item->source == ANY_HOST) &&
	    (item->srcmask == WILD_ANY))
	    printf(" any");
	else if (item->srcmask == WILD_HOST)
	    printf(" host %i", item->source);
	else 
	    printf(" %i %i", item->source, item->srcmask);
	
	if (item->srcopr != OPC_NONE) {
	    printf(" ");
	    printf_id_string(ip2access_cmds, item->srcopr);
	    printf(" ");
	    switch (item->protocol) {
	    case TCP_PROT:
		printf_id_string(tcp_ports, item->sport);
		break;
	    case UDP_PROT:
		printf_id_string(udp_ports, item->sport);
		break;
	    default:
		break;
	    }
	    if (item->srcopr == OPC_RANGE) {
		printf(" ");
		switch (item->protocol) {
		case TCP_PROT:
		    printf_id_string(tcp_ports, item->sport2);
		    break;
		case UDP_PROT:
		    printf_id_string(udp_ports, item->sport2);
		    break;
		default:
		    break;
		}
	    }
	}
	
	if ((item->destination == ANY_HOST) &&
	    (item->dstmask == WILD_ANY))
	    printf(" any");
	else if (item->dstmask == WILD_HOST)
	    printf(" host %i", item->destination);
	else 
	    printf(" %i %i", item->destination, item->dstmask);
	
	if (item->dstopr != OPC_NONE) {
	    printf(" ");
	    switch (item->protocol) {
	    case TCP_PROT:
		printf_id_string(ip2access_cmds, item->dstopr);
		printf(" ");
		printf_id_string(tcp_ports, item->dport);
		break;
	    case UDP_PROT:
		printf_id_string(ip2access_cmds, item->dstopr);
		printf(" ");
		printf_id_string(udp_ports, item->dport);
		break;
	    case ICMP_PROT:
		if (item->dstopr == OPC_ONEBYTE) 
		    printf_id_string(icmptypes, item->dport);
		else if (item->dstopr == OPC_TWOBYTES) {
		    printf_id_subid_string(icmptypecodes,
					   item->dport,
					   item->dport2);
		}
		break;
	    case IGMP_PROT:
		if (item->dstopr == OPC_ONEBYTE) 
		    printf_id_string(igmptypes, item->dport & 0xf);
		break;
	    default:
		break;
	    }
	    if (item->dstopr == OPC_RANGE) {
		printf(" ");
		switch (item->protocol) {
		case TCP_PROT:
		    printf_id_string(tcp_ports, item->dport2);
		    break;
		case UDP_PROT:
		    printf_id_string(udp_ports, item->dport2);
		    break;
		default:
		    break;
		}
	    }
	}
	if (item->established)
	    printf(" established");
	if (item->precedence) {
	    printf(" precedence ");
	    printf_id_string(precedence_values, item->precedence_value
			     >> IP_PRECEDENCE_SHIFT);
	}
	if (item->tos) {
	    printf(" tos ");
	    printf_id_string(tos_values, item->tos_value >>
			     IP_TOS_SHIFT);
	}

	switch (item->log) {
	case IP_ACCESS_LOG_NORMAL:
	    printf(" log");
	    break;

	case IP_ACCESS_LOG_INPUT:
	    printf(" log-input");
	    break;

	default:
	    break;

	}
	
	if (item->timeout) {
	    if (item->dynamic) {
		printf(" timeout %d min.", item->timeout);
	    } else {
		printf(" idle-time %d min.", item->timeout);
	    }
	}
	hits = item->hits - item->si_hits;
	if (hits)
	    printf(" (%d match%s)", hits, hits == 1 ? "" : "es");
	
	if (item->dynamic && item->dynacl_p->item) {
	    inclitem = item;
	    item = item->dynacl_p->item;	/* Go to include */
	    continue;
	}
	next = item->next;
	free(item);
	item = next;
	if (!item && inclitem) {
	    next = inclitem->next;
	    free(inclitem);
	    inclitem = NULL;
	    item = next;
	}
    }
    
}

/*
 * show_ip2access
 * Show the extended IP access lists
 */

void show_ip2access (acl_headertype *acl)
{
    int i;
    acl_headertype *tacl;
    list_element *el;

    if (acl) {
	ip2access_printlist(acl);
	return;
    }

    for (i = MINSLOWACC; i <= MAXSLOWACC; i++)
	ip2access_printlist(acl_array[i]);
	
    FOR_ALL_DATA_IN_LIST(&nacl_list, el, tacl) {
	ip2access_printlist(tacl);
    }
}

/*
 * clear_ip2access_stats
 * Clear access list statistics on a given (or all) acl's.
 */

void clear_ip2_access_stats (acl_headertype *acl)
{
    int i;
    slowtype *item;
    acl_headertype *tacl;
    list_element *el;

    if (acl) {			/* Clear specific ACL */
	for (item = (slowtype *) acl->list; item; item = item->next)
	    item->si_hits = item->hits;
    } else {			/* Clear them all */
	for (i = MINSLOWACC; i <= MAXSLOWACC; i++) {
	    tacl = acl_array[i];
	    if (tacl) {
		for (item = (slowtype *) tacl->list; item; item = item->next)
		    item->si_hits = item->hits;
	    }
	}
	FOR_ALL_DATA_IN_LIST(&nacl_list, el, tacl) {
	    for (item = (slowtype *) tacl->list; item; item = item->next)
		item->si_hits = item->hits;
	}
    }    
}

/*
 * ip_access_list_configured
 *
 * Returns TRUE if access-list number is configured.
 */
boolean ip_access_list_configured (int list)
{
    if ((list <= MINFASTACC) || (list > MAXSLOWACC)) 
	return(FALSE);

    if (ip_getacl(list))
	return(TRUE);
    else
	return(FALSE);
}
