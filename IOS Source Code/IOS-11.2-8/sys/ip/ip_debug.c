/* $Id: ip_debug.c,v 3.2.58.3 1996/07/25 09:47:14 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_debug.c,v $
 *------------------------------------------------------------------
 * ip_debug.c -- Debugging routines for General IP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_debug.c,v $
 * Revision 3.2.58.3  1996/07/25  09:47:14  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/07/23  18:31:57  snyder
 * CSCdi63791:  debugging arrays should be const
 *              4112 out of data space, 556 out of image
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/05/22  20:54:57  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.2  1995/11/17  09:33:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:57:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "../ui/debug.h"
#include "../ip/ip_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "ip_debug_flags.h"

/*
 * Initialization and storage allocation for the access list that restricts
 * IP packet debugging output and the "detail" flag.
 */
int ipdebug_acl = 0;
boolean ipdebug_detail = FALSE;
boolean ipdebug_dump = FALSE;

int iperror_acl = 0;
boolean iperror_detail = FALSE;
boolean iperror_dump = FALSE;

/*
 * Initialization and storage allocation for the access list that restricts
 * IP mpacket debugging output and the "detail" flag.
 */
int ipmdebug_acl = 0;
boolean ipmdebug_detail = FALSE;

/*
 * Used for selective debugging of multicast packets.
 */
ipaddrtype ipmpkt_group_debug;


/*
 * ip_debug_command
 * This is the routine the parser calls for IP debug commands. It
 * handles the special case of an access list number on "debug ip packet",
 * a group address on "debug ip mpacket", and a "detail" keyword on both,
 * and punts everything else to the generic handler.
 */
void ip_debug_command (parseinfo *csb)
{
    if (csb->which == DEBUG_IPPKT) {
	ipdebug_acl = GETOBJ(int,1);
	ipdebug_detail = GETOBJ(int,2);
	ipdebug_dump = GETOBJ(int,3);
    } else if (csb->which == DEBUG_IPMPKT) {
	ipmdebug_acl = GETOBJ(int,1);
	ipmdebug_detail = GETOBJ(int,2);
    } else if (csb->which == DEBUG_IPERROR) {
	iperror_acl = GETOBJ(int,1);
	iperror_detail = GETOBJ(int,2);
	iperror_dump = GETOBJ(int,3);
    }
    (const) SETOBJ(pdb,1) = ip_debug_arr;

    /* Everything else is generic */
    debug_command(csb);

    if (csb->which == DEBUG_IPPKT) {
	if (ipdebug_detail)
	    printf(" (detailed)");
	if (ipdebug_dump)
	    printf(" (dump)");
	if (ipdebug_acl != 0)
	    printf(" for access list %d", ipdebug_acl);
    } else if (csb->which == DEBUG_IPMPKT) {
	if (ipmdebug_detail)
	    printf(" (detailed)");
	if (ipmdebug_acl != 0)
	    printf(" for access list %d", ipmdebug_acl);
    } else if (csb->which == DEBUG_IPERROR) {
	if (iperror_detail)
	    printf(" (detailed)");
	if (iperror_dump)
	    printf(" (dump)");
	if (iperror_acl != 0)
	    printf(" for access list %d", iperror_acl);
    }

    /*
     * "debug ip mpackets [<group-address>]" accepts optional group
     * address.
     */
    if (csb->which == DEBUG_IPMPKT) {
	ipmpkt_group_debug = 0;
	if (csb->sense) {
	    ipmpkt_group_debug = GETOBJ(paddr,3)->ip_addr;
	    if (ipmpkt_group_debug)
		printf(" for group %i", ipmpkt_group_debug);
	}
    }
}


/*
 * ip_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the IP debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void ip_debug_all (boolean flag)
{

    if (!flag) {
	ipdebug_acl = 0;
	ipdebug_detail = FALSE;
	ipdebug_dump = FALSE;
	iperror_acl = 0;
	iperror_detail = FALSE;
	iperror_dump = FALSE;
    }

    generic_debug_all(ip_debug_arr, flag);
}

/*
 * ip_debug_show is called to display the values of all the IP
 * debugging variables.
 */
static void ip_debug_show (void)
{
    int i;
    boolean hdisplayed = FALSE;	/* Printed the heading yet? */
    boolean willprint;		/* Will we print anything on this line? */

    for (i = 0; ip_debug_arr[i].var != (boolean *) NULL; i++) {

	/*
	 * Normally, we only print stuff that doesn't match the current
	 * status of "debug all". However, if something restricts the
	 * output associated with a flag that's turned on, we need to
	 * talk about that flag even if "debug all" is turned on.
	 */
	willprint = (!*(ip_debug_arr[i].var) != !debug_all_p());
	if ((i == DEBUG_IPPKT) && ippkt_debug && (ipdebug_acl != 0))
	    willprint = TRUE;

	if (willprint) {
	    if (!hdisplayed) {
		printf("\nGeneric IP:");
		hdisplayed = TRUE;
	    }

	    printf("\n  %s debugging is %s",
		   ip_debug_arr[i].txt,
		   *(ip_debug_arr[i].var) ? "on" : "off");

	    if (i == DEBUG_IPPKT) {
		if (ipdebug_detail)
		    printf(" (detailed)");
		if (ipdebug_dump)
		    printf(" (dump)");
		if (ipdebug_acl != 0)
		    printf(" for access list %d", ipdebug_acl);
	    }
	    if (i == DEBUG_IPMPKT) {
		if (ipmpkt_group_debug)
		    printf(" for group %i", ipmpkt_group_debug);
		if (ipmdebug_detail)
		    printf(" (detailed)");
		if (ipmdebug_acl != 0)
		    printf(" for access list %d", ipmdebug_acl);
	    }

	    if (i == DEBUG_IPERROR) {
		if (iperror_detail)
		    printf(" (detailed)");
		if (iperror_dump)
		    printf(" (dump)");
		if (iperror_acl != 0)
		    printf(" for access list %d", iperror_acl);
	    }
	}
    }
}


/*
 * ip_debug_init is called at IP subsystem startup. It registers
 * the routines to maintain and display the IP debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when IP is started.
 */

void ip_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(ip_debug_all, "ip_debug_all");
    reg_add_debug_show(ip_debug_show, "ip_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when IP is initialized, we want to turn on
     * all the IP debugging right now.
     */
    ip_debug_all(debug_all_p());
}
