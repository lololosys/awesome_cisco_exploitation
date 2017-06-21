/* $Id: ip_setup.c,v 3.4 1995/11/28 03:58:35 widmer Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/ip_setup.c,v $
 *------------------------------------------------------------------
 * IP setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ip_setup.c,v $
 * Revision 3.4  1995/11/28  03:58:35  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.3  1995/11/17  09:33:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:55:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:57:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "packet.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ip/ipaddress.h"
#include "../ui/setup.h"

/*
 * Forward declarations
 */
static void ip_setup_add_int_history(setup_struct *, idbtype *,
        ipaddrtype, ipaddrtype);
static boolean ip_setup_check_int_history(setup_struct *, idbtype **,
        ipaddrtype *, ipaddrtype *, boolean);


void ip_setup_int (setup_struct *sp)
{
    idbtype *didb = NULL, *tidb;
    char *cp, buffer[MAXBUF];
    ipaddrtype addr, mask;

    /*
     * IP is different than other protocols since it is on by default.
     * Check to see if an interface has or can have an ip unnumbered
     * assignment. If so, find the first IP interface and use it as a
     * default (didb).  Prompt the user to supply an interface (tidb), and
     * verify that it is acceptable.
     */
    if (sp->run_ip && interface_protocol("IP", sp->idb->ip_enabled)) {
	if (sp->idb->ip_unnumbered && sp->idb->ip_unnumbered->ip_address) {
	    didb = sp->idb->ip_unnumbered;
	} else {
	    ip_setup_check_int_history(sp, &didb, &addr, &mask, TRUE);
	}	
	if ((didb != NULL) && (sp->idb->hwptr->status & IDB_SERIAL) &&
	    yes_or_no("  Configure IP unnumbered on this interface",
		      (boolean)sp->idb->ip_unnumbered, TRUE)) {
	    setup_printf(sp, "\nno ip address");
	    cp = setup_string(buffer, "    Assign to which interface",
			      didb->namestring);
	    parse_interface(cp, &tidb);
	    while ( (!tidb || sp->idb == tidb || 
		    !ip_setup_check_int_history(sp, &tidb, &addr, &mask, TRUE))
		    && !(stdio->statbits & CONTROLC)) {
		printf("\n  Not a valid IP interface -- %s", cp);
		cp = setup_string(buffer, "    Assign to which interface",
				  didb->namestring);
		parse_interface(cp, &tidb);
	    }
	    if (tidb) {
		setup_printf(sp, "\nip unnumbered %s", tidb->namestring);
	    }
	} else {
	    resolve_ipaddr(&addr, &mask, sp->idb->ip_address,
			   sp->addrlist, sp->masklist);
	    setup_printf(sp, "\nip address %i %i", addr, mask);
	    ip_setup_add_int_history(sp, sp->idb, addr, mask);
	}
    } else {
	setup_printf(sp, "\nno ip address");
    }
}


/*
 * insert current addr/mask plan
 */
static void ip_setup_add_int_history(setup_struct *sp, idbtype *idb,
        ipaddrtype addr, ipaddrtype mask)
{
    struct setup_ip_history_ *sih;
    sih = malloc(sizeof(struct setup_ip_history_));
    if (sih == NULL || sp == NULL || idb == NULL)
	return;
    sih->next = sp->ip_history_head;
    sp->ip_history_head = sih;
    sih->addr = addr;
    sih->mask = mask;
    sih->idb = idb;
}


/*
 * return current understanding of what the addr and mask of this
 * interface will be.  If idb==NULL, find and set a default.
 */
static boolean ip_setup_check_int_history(setup_struct *sp, idbtype **idb, 
	ipaddrtype *addr, ipaddrtype *mask, boolean check_beyond)
{
    struct setup_ip_history_ *sih, *tsih = NULL;
    idbtype *tidb;

    for (sih = sp->ip_history_head; sih; sih = sih->next) {
	if (sih->idb == *idb || (*idb == NULL && sih->addr != 0L))
	    tsih = sih;
    }

    *addr = 0L;
    *mask = 0L;

    if (tsih) {
	*addr = tsih->addr;
	*mask = tsih->mask;
	*idb  = tsih->idb;
    } else if (check_beyond) {
	for (tidb = sp->idb->next; tidb; tidb = IDB_NEXT(tidb)) {
	    if (tidb == *idb || (*idb == NULL && tidb->ip_enabled && 
		tidb->ip_address)) { 
		*addr = tidb->ip_address;
		*mask = tidb->ip_nets_mask;
		*idb  = tidb;
		break;
	    }
	}
    } else
	*idb  = NULL;

    return(*idb != NULL && *addr != 0L);
}
