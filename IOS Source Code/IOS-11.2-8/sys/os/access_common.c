/* $Id: access_common.c,v 3.1.2.6 1996/08/30 01:49:55 gchristy Exp $
 * $Source: /release/112/cvs/Xsys/os/access_common.c,v $
 *------------------------------------------------------------------
 * access_common.c -- Common ACL functions
 *
 * May 1996, Greg Christy
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: access_common.c,v $
 * Revision 3.1.2.6  1996/08/30  01:49:55  gchristy
 * CSCdi67644:  IP NACL: inform user when access-list type is invalid
 * Branch: California_branch
 * - Complain if an existing list is an invalid type.
 *
 * Revision 3.1.2.5  1996/08/29  23:09:33  gchristy
 * CSCdi67444:  IP NACL: resolve no ip access-group ambiguity
 * Branch: California_branch
 * - Make access-list names "in" and "out" illegal.
 * - Fix parse chain to do the right thing.
 *
 * Revision 3.1.2.4  1996/08/16  08:02:30  dlobete
 * CSCdi59971:  DACL needs modularity work
 * Branch: California_branch
 * For the sake of future maintenance of IP access-lists, share as much
 * source code as possible between VIP and RSP.
 *
 * Revision 3.1.2.3  1996/06/03  23:24:20  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/31  06:28:24  gchristy
 * CSCdi59167:  IP NACL: access-group command disappears
 * Branch: California_branch
 * - Treat ACL_UNDEFINED as a wildcard when matching types.
 *
 * Revision 3.1.2.1  1996/05/28  16:17:50  xliu
 * CSCdi58788:  code changes for vip named access list and encryption perf
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/08  01:04:47  gchristy
 * Initial commit.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include <stdlib.h>
#include "ctype.h"
#include "access.h"
#include "string.h"
#include "sys_registry.h"
/*
 * Used to maintain compatibility with old style numbered ACLs with
 * named ACLs As IP is the only current user, it is sized accordingly.
 */
acl_headertype *acl_array[MAXSLOWACC+1];

/*
 * Holds list of Named ACLs.
 */
list_header nacl_list;


/*
 * acl_init
 * Initialize ACL data structures.
 */

void acl_init (void)
{
    int i;

    /*
     * Initial dongle array for numbered ACLs.
     */
    for (i = 0; i < MAXSLOWACC; i++) {
	acl_array[i] = NULL;
    }

    /*
     * Create NACL list.
     */
    list_create(&nacl_list, MAXNACLS, "Named Access Lists",
		LIST_FLAGS_AUTOMATIC); 
}
    
/*
 * create_dongle
 * Create and initialize an ACL header structure.
 */
static acl_headertype *create_dongle (const char *name, acltype type)
{
    acl_headertype *acl;

    acl = malloc_named(sizeof(acl_headertype), "ACL Header");

    if (acl) {
	setstring(&acl->name, name);
	acl->type = type;
	acl->tp_header_required = FALSE;
	acl->list = NULL;
    }
    return(acl);
}

/*
 * acl_sort
 * Used to order named ACL list during insertion.
 */
static int acl_sort (void *acl_arg1, void *acl_arg2)
{
    acl_headertype *acl1 = acl_arg1;
    acl_headertype *acl2 = acl_arg2;

    return(strcmp(acl1->name, acl2->name));
}

/*
 * access_delete_acl
 * Remove a numbered ACL from the list and nuke it.
 */
void access_delete_acl (ulong list)
{
    acl_headertype *acl;

    if ((list < MINFASTACC) || (list > MAXSLOWACC))
	return;

    acl = acl_array[list];
    acl_array[list] = NULL;

    free(acl);

    return;
}

/*
 * access_find_or_create_acl
 * Returns a pointer to a dongle for a numbered list and, if
 * necessary, creates and initializes it.
 */
acl_headertype *access_find_or_create_acl (ulong num)
{
    acl_headertype *acl;
    acltype type = ACL_UNDEFINED;
    char tstr[10];

    if ((num < MINFASTACC) || (num > MAXSLOWACC)) /* We only support IP */
	return(NULL);

    acl = acl_array[num];

    if (acl)			/* See if list already exists */
	return(acl);

    sprintf(tstr, "%d", num);

    if ((num >= MINFASTACC) && (num <= MAXFASTACC))
	type = ACL_IP_SIMPLE;
    else
	type = ACL_IP_EXTENDED;

    acl_array[num] = create_dongle(tstr, type);

    return(acl_array[num]);
}

/*
 * access_find_acl
 * Returns a pointer to a dongle for a numbered list or NULL if the
 * list does not exist
 */
acl_headertype *access_find_acl (ulong num)
{
    acl_headertype *acl;

    if ((num < MINFASTACC) || (num > MAXSLOWACC)) /* We only support IP */
	return(NULL);

    acl = acl_array[num];
    if ( ! (acl && acl->list) ) {
	return(NULL);
    }

    return(acl_array[num]);
}

/*
 * access_find_nacl
 * Returns a pointer to a dongle for a named list. Returns NULL if
 * list doesn't exist.
 */
acl_headertype *access_find_nacl (const char *name)
{
    acl_headertype *acl;
    list_element *el;

    FOR_ALL_DATA_IN_LIST(&nacl_list, el, acl) {
	if (!strcmp(acl->name, name))
	    return(acl);
    }

    return(NULL);
}

/*
 * access_create_nacl
 * Returns a pointer to a new dongle for a named list.
 */
acl_headertype *access_create_nacl (const char *name, acltype type)
{
    acl_headertype *acl;

    acl = create_dongle(name, type);

    if (acl)
	list_insert(&nacl_list, NULL, acl, acl_sort);

    return(acl);
}

/*
 * access_delete_nacl
 * Remove a named ACL from the list and nuke it.
 */
void access_delete_nacl (acl_headertype *acl)
{

    acl = list_remove(&nacl_list, NULL, acl);

    free(acl);
    return;
}

/*
 * is_valid_nacl
 * Given an NACL name, return TRUE if it's copasetic.
 */
boolean is_valid_nacl (const char *name)
{
    if (name && isalpha(name[0])) {
	/* Access lists named "in" or "out" will create ambiguities
	   when trying to remove them from an interface, so don't allow
	   their use. */
	if (!strcasecmp(name, "in") || !(strcasecmp(name, "out")))
	    return(FALSE);
	else
	    return(TRUE);
    } 
    
    return(FALSE);
}

/*
 * access_find_or_create_nacl
 * Returns a pointer to a dongle for a named list and, if it doesn't exist,
 * creates and initializes it.
 */
acl_headertype *access_find_or_create_nacl (const char *name, acltype type) 
{
    acl_headertype *acl;

    switch (type) {
    case ACL_UNDEFINED:
    case ACL_IP_NAMED_EXTENDED:
    case ACL_IP_NAMED_SIMPLE:
      /* These are OK */
      break;
    case ACL_IP_EXTENDED:
    case ACL_IP_SIMPLE:
    default:
      /* Can't create a named ACL of these types */
      return(NULL);
      break;
    }

    if (!is_valid_nacl(name))
	return(NULL);

    acl = access_find_nacl(name);

    if (!acl) {
	acl = access_create_nacl(name, type);
    } else if (acl->type == ACL_UNDEFINED) {
	/* Define type if undefined */  
	acl->type = type;
    } else if ((type != ACL_UNDEFINED) && (acl->type != type)) {
	/* types have to match unless type is ACL_UNDEFINED */
	return(NULL);
    }
	
    return(acl);
}

/*
 * access_parse_acl
 *
 * Given a list number or list name, return pointer to ACL header and
 * ensure it's the type requested unless the type is ACL_UNDEFINED.
 * If the ACL doesn't exist, create it.
 */

acl_headertype *access_parse_acl (int list, char *name, boolean is_alpha,
				  acltype type)
{
    acl_headertype *acl = NULL;

    if (is_alpha) {
	if (!is_valid_nacl(name)) {
	    printf(nacl_name_error);
	    return(NULL);
	}
	acl = access_find_or_create_nacl(name, type);
	if (!acl) {
	    acl = access_find_nacl(name);
	    if (acl && type != ACL_UNDEFINED && acl->type != type) {
		printf("\n%% This command only accepts %s access-lists.",
		       access_type2string(type));
	    }
	    return(NULL);
	}
    } else {
	acl = access_find_or_create_acl(list);
    }

    return(acl);
}
