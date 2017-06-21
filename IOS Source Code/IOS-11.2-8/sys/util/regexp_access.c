/* $Id: regexp_access.c,v 3.4.10.6 1996/08/28 13:19:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/util/regexp_access.c,v $
 *------------------------------------------------------------------
 * regexp_access.c -- X25, LAT, and BGP as-path access control
 *
 * May 1990, Bill Westfield
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * X25 access lists work just like X.25 routing - using regular expressions
 * since regular expressions are pretty general purpose things, the same
 * access lists can be used for other things (like Novell SAP Server names)
 *------------------------------------------------------------------
 * $Log: regexp_access.c,v $
 * Revision 3.4.10.6  1996/08/28  13:19:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.10.5  1996/08/19  19:21:03  widmer
 * CSCdi66453:  regexpaccess_command() does not notice failed regexp
 * compiles
 * Branch: California_branch
 * Add more checking
 *
 * Revision 3.4.10.4  1996/08/19  18:59:32  widmer
 * CSCdi66453:  regexpaccess_command() does not notice failed regexp
 * compiles
 * Branch: California_branch
 *
 * Revision 3.4.10.3  1996/07/26  00:54:15  hampton
 * Fix the automore code in show access-list.  [CSCdi61084]
 * Branch: California_branch
 *
 * Revision 3.4.10.2  1996/04/04  15:41:25  widmer
 * CSCdi53503:  long regexp in ip as-path access-list cmd crashes router
 * Branch: California_branch
 * Malloc string instead of using static size.
 *
 * Revision 3.4.10.1  1996/03/18  22:30:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  07:52:07  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/03/07  11:00:09  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  21:45:23  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/24  01:56:37  pst
 * CSCdi49774:  modularize bgp regexp acl's (part 2)
 *
 * Revision 3.3  1995/11/17  17:56:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:47:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:43:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:13:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "access.h"
#include "regexp.h"
#include "config.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include <string.h>

/*
 * Local storage
 */
regexpacctype *x29_accesslists[(MAXREGEXPACC-MINREGEXPACC)+1];
regexpacctype *lat_accesslists[(MAXREGEXPACC-MINREGEXPACC)+1];

/*
 * forward references
 */

/*
 * externals
 */


boolean
regexp_accesscheck (uchar *address, int list, regexpacctype **array)
{
    regexpacctype *acc;

    if (list == 0)
	return(TRUE);

    if (list < MINREGEXPACC || list > MAXREGEXPACC) {
	errmsg(&msgsym(BADLIST, SYS), list);
	return(FALSE);
    }
    for (acc = array[list]; acc; acc = acc->next) {
	if (!acc)
	    return(FALSE);
	if (regexec(acc->comppattern, (char *)address))
	    return(acc->grant);
    }
    return(FALSE);
}

/*
 * Parse a regular expression access list
 *   "access-list <list> permit|deny <pattern>"
 * Return the access-list number if there is any change/addition.
 * Return 0 if there is no change.
 * The returned value is used to trigger callbacks.
 */
int
regexpaccess_command (parseinfo *csb, regexpacctype *array[])
{
    int i;
    int number, grant;
    regexpacctype *item, *list;

    if (csb->nvgen) {
	for (i= MINREGEXPACC; i <=  MAXREGEXPACC; i++) {
	    list = array[i];
	    if (list) {
		while (list) {
		    nv_write(TRUE, "%s %d %s %s", csb->nv_command,
			     i, list->grant ? "permit" : "deny",
			     list->pattern);
		    list = list->next;
		}
	    }
	}
	return(0);
    }

    /*
     * Get the access list number
     */
    number = GETOBJ(int,1);

    /*
     * Get the access type, either "permit" or "deny".
     * Skip this if deleting an access list.
     */
    if (csb->sense == FALSE) {
	list = array[number];
	array[number] = NULL;
	while (list != NULL) {
	    item = list->next;
	    free(list->comppattern);
	    free(list->pattern);
	    free(list);
	    list = item;
	}
	return(number);
    }

    grant = GETOBJ(int,2);

    /*
     * Check for a duplicate entry
     */

    for (list = array[number]; list; list = list->next) {
	if (strcmp(list->pattern, GETOBJ(string,1)) == 0) {

	    /*
	     * At this point, we could check for the "no" case and
	     * remove an individual entry.
	     */

	    return(0);
	}
    }

    /*
     * Not a duplicate, create the access list entry
     */

    item = malloc(sizeof(regexpacctype));
    if (!item) {
	printf(nomemory);
	return(0);
    }
    item->grant = grant;	
    setstring(&item->pattern, GETOBJ(string,1));
    item->comppattern = regcomp(GETOBJ(string,1));

    /*
     * Compiling expression failed, so bail and don't accept
     */
    if (!item->pattern || !item->comppattern) {
	free(item->pattern);
	free(item);
	return(0);
    }

    /*
     * Add the entry to the end of the list
     */

    list = array[number];	/* Get pointer to start of list */
    if (!list) {
	array[number] = item;	/* Make this the head of the list */
    } else {
	while (list->next) {
	    list = list->next;	/* Find the last entry in the list */
	}
	list->next = item;	/* Make this the tail of the list */
    }
    return(number);
}

void
show_regexpacl (regexpacctype *acl_array[], int list, char *title)
{
    int i;
    regexpacctype *item;

    for (i = MINREGEXPACC; i <=  MAXREGEXPACC; i++) {
	if (acl_array[i] == NULL || (list > 0 && list != i))
	    continue;
	if (automore_quit())
	    return;
	printf("\n%s access list %d", title, i);
	item = acl_array[i];
	while (item) {
	    printf("\n    %s %s", item->grant ? "permit" : "deny",
		   item->pattern);
	    item = item->next;
	}
    }
}

/*
 * show_regexpaccess
 * Show the regular expression based access lists
 */
void
show_regexpaccess (int list)
{
    show_regexpacl(x29_accesslists, list, "X29");
    show_regexpacl(lat_accesslists, list, "LAT");
}

/*
 * regexpaccess_init
 * initialize regular expression based access lists
 */

void regexpaccess_init (void)
{
    /*
     * Register some functions
     */
    reg_add_access_show(show_regexpaccess, "show_regexpaccess");
}
