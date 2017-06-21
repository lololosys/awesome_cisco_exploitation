/* $Id: access.c,v 3.3.62.9 1996/09/09 22:17:20 ahh Exp $
 * $Source: /release/112/cvs/Xsys/os/access.c,v $
 *------------------------------------------------------------------
 * access.c -- Common Access List Support
 *
 * 6-February-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: access.c,v $
 * Revision 3.3.62.9  1996/09/09  22:17:20  ahh
 * CSCdi68478:  clear access-list counters not working
 * Remove parse-chain ambiguity and allow NULL to be passed into the
 * access_clear registry to clear the counters on all access-lists.
 * Branch: California_branch
 *
 * Revision 3.3.62.8  1996/08/30  01:49:54  gchristy
 * CSCdi67644:  IP NACL: inform user when access-list type is invalid
 * Branch: California_branch
 * - Complain if an existing list is an invalid type.
 *
 * Revision 3.3.62.7  1996/08/29  23:09:29  gchristy
 * CSCdi67444:  IP NACL: resolve no ip access-group ambiguity
 * Branch: California_branch
 * - Make access-list names "in" and "out" illegal.
 * - Fix parse chain to do the right thing.
 *
 * Revision 3.3.62.6  1996/08/14  02:36:26  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.3.62.5  1996/07/27  02:32:26  akr
 * CSCdi63880:  show access-list can display unnecessary information
 * Branch: California_branch
 *
 * Revision 3.3.62.4  1996/07/10  22:21:24  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.62.3  1996/05/08  01:10:01  gchristy
 * CSCdi56944:  Compilation errors from IP named access list commit
 * Branch: California_branch
 * - Modularize new ACL code for inclusion in VIP images.
 *
 * Revision 3.3.62.2  1996/05/07  23:53:49  carrel
 * CSCdi56978:  Crypto Maps wont compile with named access lists
 * Branch: California_branch
 * Made checking of acl_headertype->type value stronger when finding named
 * acls
 *
 * Revision 3.3.62.1  1996/05/04  01:34:28  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR bridging support.
 *
 * Constrained Multicast Flooding
 * - Add the default function return_one.
 * - Initialize the default function for tbridge_cmf to return_one.
 *
 * Name Access List
 *
 * Revision 3.3  1995/11/17  18:44:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:16:40  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:43:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/18  06:47:58  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 21:53:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdlib.h>
#include "sys_registry.h"
#include "config.h"
#include "access.h"

/*
 * Local Storage
 */
accessptr *accessarray[ACCESSSIZE];


const char nacl_name_error[] = "\n%% Invalid access list name.";

sys_timestamp access_changed_times[ACCESSSIZE];
static access_callback *access_callbacks;
sys_timestamp access_entry_changed_time;

static void access_perform_change_callbacks(void);


/*
 * access_init
 * Initialize access checking facility.
 */

void access_init (void)
{
    ulong i;

    for (i = 0; i < ACCESSSIZE; i++) {
	accessarray[i] = NULL;
	TIMER_STOP(access_changed_times[i]); 
    }
    access_callbacks = NULL;
    TIMER_STOP(access_entry_changed_time);
    regexpaccess_init();

    acl_init();

    /*
     * Initialize access list parser support
     */
    access_parser_init();
    
    /*
     * Register the one-minute access list change checker
     */
    reg_add_onemin(access_perform_change_callbacks,
		   "access_perform_change_callbacks");
}


/*
 * access_type2string
 * Return a string describing the access list type.
 */
const char *access_type2string (acltype type)
{

    switch (type) {
    case ACL_IP_SIMPLE:
	return("simple");
    case ACL_IP_NAMED_SIMPLE:
	return("named standard");
    case ACL_IP_EXTENDED:
	return("extended");
    case ACL_IP_NAMED_EXTENDED:
	return("named extended");

    default:
	return("unknown");

    }
}

/*
 * access_print_name
 * Prints ACL name if set, else it prints "not set". 
 */
void access_print_name (acl_headertype *acl)
{
    if (!acl) {
	printf("not set");
	return;
    } else
	printf(acl->name);
}

/*
 * show_access
 * Show contents of access lists.
 */

void show_access (parseinfo *csb)
{
    acl_headertype *acl;
    uint listnum;
    char *name;

    automore_enable(NULL);
    listnum = GETOBJ(int, 1);
    name = GETOBJ(string, 1);

    if (listnum) {		/* Number specified*/
	if (listnum > MAXSLOWACC) 
	    reg_invoke_access_show(listnum);
	else
	    if (acl_array[listnum])
		reg_invoke_access_named_show(acl_array[listnum]);
    } else if (name && strlen(name)) {	/* Name specified */
	acl = access_find_nacl(name);
	if (acl)
	    reg_invoke_access_named_show(acl);
    } else {			/* Show everything */
	reg_invoke_access_show(0);
	reg_invoke_access_named_show(NULL);
    }

    automore_disable();
}

/*
 * clear_access
 * Clear contents of access lists.
 */

void clear_access (parseinfo *csb)
{
    acl_headertype *acl;

    automore_enable(NULL);
    if (GETOBJ(int,1)) 
	acl = acl_array[GETOBJ(int,1)];
    else
	acl = access_find_nacl(GETOBJ(string,1));

    reg_invoke_access_clear(acl);
    automore_disable();
}

/*
 * access_check_type
 * Given an access list and a typecode, determine permit/deny result.
 */

boolean access_check_type (ulong list, ushort typecode)
{
    typetype *grp;
    int i;
    typeitem *item;
    ushort mask;
    
    grp = type_get_acl(list);
    if (!grp)
	return(TRUE);
    for (i = 0, item = &grp->item[0]; i < grp->items; item++, i++) {
	mask = ~item->mask;
	if ((typecode & mask) == item->value)
	    return(item->grant);
    }
    return(FALSE);
}

/*
 * access_check_mac (was bridge_address_ok)
 * Check if the mac layer address is allowed to be bridged based on the 
 * passed access_list number.
 */

boolean access_check_mac (int access_list, uchar *etheraddress)
{
    macaddracctype *item;
    ulong ul_mask, ul_addr;
    ushort us_mask, us_addr;
    
    item = mac_get_std_acl(access_list);
    if (item == NULL)
	return(TRUE);

    /*
     * Looks crude, but really VERY efficient.
     */
    ul_addr = etheraddress[0];
    ul_addr <<= 8;
    ul_addr |= etheraddress[1];
    ul_addr <<= 8;
    ul_addr |= etheraddress[2];
    ul_addr <<= 8;
    ul_addr |= etheraddress[3];
    us_addr = etheraddress[4];
    us_addr <<= 8;
    us_addr |= etheraddress[5];
    while (item) {
	us_mask = ~item->mask.u.sword[2];
	if (item->addr.u.sword[2] == (us_addr & us_mask)) {
	    ul_mask = ~item->mask.u.lword[0];
	    if (item->addr.u.lword[0] == (ul_addr & ul_mask))
		return(item->grant);
	}
	item = item->next;
    }
    return(FALSE);	    
}

/*
 * Support for access list callbacks. Will callback functions defined at
 * runtime if an access list is changed. Will be checked once a minute by
 * net_background.
 */

/*
 * access_create_callback
 *
 * Create an acccess callback function
 */
boolean access_create_callback (access_callbackfunction callback_function,
				ulong low, ulong high)
{
    access_callback *new;

    if ((new = malloc(sizeof(access_callback))) == NULL)
        return(FALSE);
    new->callback_function = callback_function;
    new->low = low;
    new->high = high;
    new->next = access_callbacks;
    access_callbacks = new;
    return(TRUE);
}

/*
 * access_destroy_callback
 *
 * Destroy an access callback function
 */
boolean access_destroy_callback (ulong low, ulong high)
{
    access_callback *prev, *current;

    for (prev = (access_callback *) &access_callbacks, current = prev->next;
	 current != NULL; prev = current, current = current->next) {
	if ((current->low == low) && (current->high == high)) {
	    prev->next = current->next;
	    free(current);
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * access_perform_change_callbacks
 *
 * Periodically see if access lists have changed, and call callbacks
 */
static void access_perform_change_callbacks (void)
{
    access_callback *current;
    ulong i;

    /* If nothing changed, just return */
    if (!TIMER_RUNNING(access_entry_changed_time))
        return;

    TIMER_STOP(access_entry_changed_time);

    for (current = access_callbacks; current != NULL;
	 current = current->next) {
	for (i = current->low; i <= current->high; i++) {
	    if (TIMER_RUNNING(access_changed_times[i])) {
		(*current->callback_function)(i);
	    }
	}
    }
    for (i = 0; i < ACCESSSIZE; i++) {
	TIMER_STOP(access_changed_times[i]);
    }
}

/*
 * access_mark_list_changed
 *
 * Mark an access list as changed. Too bad this cannot be made automatic.
 */
void access_mark_list_changed (ulong list)
{
    if ((list > 0) && (list < ACCESSSIZE)) {
	GET_TIMESTAMP(access_changed_times[list]);
    }
    GET_TIMESTAMP(access_entry_changed_time);
}
