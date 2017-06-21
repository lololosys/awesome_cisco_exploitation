/* $Id: iprouting_debug.c,v 3.2.58.3 1996/07/25 09:48:31 snyder Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/iprouting_debug.c,v $
 *------------------------------------------------------------------
 * iprouting_debug.c -- Debugging routines for IP routing
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iprouting_debug.c,v $
 * Revision 3.2.58.3  1996/07/25  09:48:31  snyder
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
 * Revision 3.2.58.2  1996/07/23  18:32:15  snyder
 * CSCdi63791:  debugging arrays should be const
 *              4112 out of data space, 556 out of image
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/04/05  03:46:33  rchandra
 * CSCdi53755:  BGP: flap dampening settings not flexible enough
 * Branch: California_branch
 * - add route-map support to set flap dampening parameters
 *
 * Revision 3.2  1995/11/17  17:34:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/06  22:53:27  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Resolve problem for boot image
 *
 * Revision 2.2  1995/09/05  22:08:52  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Add access-list debugging for SPF calculation and flooding
 * - Add separate debugging for intra-area, inter-area and external SPF
 *
 * Revision 2.1  1995/06/07  21:07:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "../ui/debug.h"
#include "../iprouting/iprouting_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "iprouting_debug_flags.h"

/* Variables that restrict output associated with various debugging flags */
ipaddrtype egp_debug_addr = 0;
ipaddrtype igrp_debug_addr = 0;
ipaddrtype igrp_debug_target;
int bgpupdate_acl = 0;
int bgpdamp_acl = 0;
int iprouting_acl = 0;
int ipcache_debug_acl = 0;
int ospf_lsa_gen_acl = 0;
int ospf_spf_intra_acl = 0;
int ospf_spf_inter_acl = 0;
int ospf_spf_ext_acl = 0;
int ospf_flood_acl;


/*
 * iprouting_debug_list
 * Just like generic debug handler but process an optional ACL
 */
static boolean iprouting_debug_list_command (parseinfo *csb, int *retacl)
{
    int acl;
    acl = GETOBJ(int, 1);

    debug_show_flag(csb->sense, TRUE, iprouting_debug_arr[csb->which].txt);
    if (acl != 0)
	printf(" for access list %d", acl);

    *retacl = acl;

    return csb->sense;
}

/*
 * iprouting_debug_command
 * Handles debug commands related to IP routing.
 *
 * csb->sense = TRUE for "debug", FALSE for "undebug".
 * csb->which = index into flag definition array.
 * GETOBJ(pdb,1) = mempointer to beginning of array... they really should
 *                 have called it "ptr1"...
 * Other objects specific to command codes...
 */

void iprouting_debug_command (parseinfo *csb)
{
    boolean sense = csb->sense;
    int count;

    (const) SETOBJ(pdb,1) = iprouting_debug_arr;

    switch (csb->which) {

    case DEBUG_EGP:
	egp_debug = sense;
	if (!sense) {
	    if (!egp_debug && !egpevent_debug)
		egp_debug_addr = 0;
	} else {
	     if (GETOBJ(int,1) == TRUE) {
		egp_debug_addr = GETOBJ(paddr,1)->ip_addr;
	     }
	}
	debug_show_flag(egp_debug, TRUE, iprouting_debug_arr[DEBUG_EGP].txt);
	if (egp_debug_addr != 0)
	    printf(" for address %i", egp_debug_addr);
	break;

    case DEBUG_EGPEVENT:
	egpevent_debug = sense;
	if (!sense) {
	    if (!egp_debug && !egpevent_debug)
		egp_debug_addr = 0;
	} else {
	     if (GETOBJ(int,1) == TRUE) {
		egp_debug_addr = GETOBJ(paddr,1)->ip_addr;
	     }
	}
	debug_show_flag(egpevent_debug, TRUE,
			iprouting_debug_arr[DEBUG_EGPEVENT].txt);
	if (egp_debug_addr != 0)
	    printf(" for address %i", egp_debug_addr);
	break;

    case DEBUG_IGRP:
	igrp_debug = sense;
	if (!sense) {
	    if (!igrp_debug && !igrp_debug_events) {
		igrp_debug_addr = 0;
		igrp_debug_target = 0;
	    }
	} else {
	    count = GETOBJ(int,1);
	    if (count >= 1) 
		igrp_debug_addr = GETOBJ(paddr,1)->ip_addr;
	    if (count >= 2)
		igrp_debug_target = GETOBJ(paddr,2)->ip_addr;
	}
	debug_show_flag(igrp_debug, TRUE,
			iprouting_debug_arr[DEBUG_IGRP].txt);
	if (igrp_debug_addr != 0)
	    printf(" for address %i", igrp_debug_addr);
	if (igrp_debug_target != 0) {
	    if (igrp_debug_addr != 0)
		printf("\n    ");
	    printf(" for target route %i", igrp_debug_target);
	}
	break;

    case DEBUG_IGRP_EVENTS:
	igrp_debug_events = sense;
	if (!sense) {
	    if (!igrp_debug && !igrp_debug_events) 
		igrp_debug_addr = 0;
	} else {
	    count = GETOBJ(int,1);
	    if (count >= 1) 
		igrp_debug_addr = GETOBJ(paddr,1)->ip_addr;
	}
	debug_show_flag(igrp_debug_events, TRUE,
			iprouting_debug_arr[DEBUG_IGRP_EVENTS].txt);
	if (igrp_debug_addr != 0)
	    printf(" for address %i", igrp_debug_addr);
	break;

    case DEBUG_BGPUPDATE:
	bgpupdate_debug = iprouting_debug_list_command(csb, &bgpupdate_acl);
	break;
	
    case DEBUG_BGPDAMP:
	bgpdamp_debug = iprouting_debug_list_command(csb, &bgpdamp_acl);
	break;
	
    case DEBUG_ROUTING:
	route_debug = iprouting_debug_list_command(csb, &iprouting_acl);
	break;

    case DEBUG_IPCACHE:
	ipcache_debug = iprouting_debug_list_command(csb, &ipcache_debug_acl);
	break;

    case DEBUG_OSPF_LSA_GEN:
	ospf_debug_lsa_gen = iprouting_debug_list_command(csb,
							  &ospf_lsa_gen_acl);
	break;
    case DEBUG_OSPF_SPF:
	csb->which = DEBUG_OSPF_SPF_INTRA;
	ospf_debug_spf_intra = 
	    iprouting_debug_list_command(csb, &ospf_spf_intra_acl);
	csb->which = DEBUG_OSPF_SPF_INTER;
	ospf_debug_spf_inter = 
	    iprouting_debug_list_command(csb, &ospf_spf_inter_acl);
	csb->which = DEBUG_OSPF_SPF_EXT;
	ospf_debug_spf_ext = 
	    iprouting_debug_list_command(csb, &ospf_spf_ext_acl);
 	break;
    case DEBUG_OSPF_SPF_INTRA:
	ospf_debug_spf_intra = 
	    iprouting_debug_list_command(csb, &ospf_spf_intra_acl);
	break;
    case DEBUG_OSPF_SPF_INTER:
	ospf_debug_spf_inter = 
	    iprouting_debug_list_command(csb, &ospf_spf_inter_acl);
	break;
    case DEBUG_OSPF_SPF_EXT:
	ospf_debug_spf_ext = 
	    iprouting_debug_list_command(csb, &ospf_spf_ext_acl);
	break;
    case DEBUG_OSPF_FLOOD:
	ospf_debug_flood = 
	    iprouting_debug_list_command(csb, &ospf_flood_acl);
	break;
	
    default:
	/* If there's no special case, use the generic handler */
	debug_command(csb);
	break;
    }
}

/*
 * iprouting_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the IP routing debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void iprouting_debug_all (boolean flag)
{
    if (!flag) {
	egp_debug_addr = 0;
	igrp_debug_addr = 0;
	igrp_debug_target = 0;
	bgpupdate_acl = 0;
	bgpdamp_acl = 0;
	iprouting_acl = 0;
	ipcache_debug_acl = 0;
	ospf_lsa_gen_acl = 0;
    }
    generic_debug_all(iprouting_debug_arr, flag);
}

/*
 * iprouting_debug_show is called to display the values of all the IP routing
 * debugging variables.
 */

static void iprouting_debug_show (void)
{
    int i;
    boolean hdisplayed = FALSE;	/* Printed the heading yet? */
    boolean willprint;		/* Will we print anything on this line? */

    for (i = 0; iprouting_debug_arr[i].var != (boolean *) NULL; i++) {

	/*
	 * Normally, we only print stuff that doesn't match the current
	 * status of "debug all". However, if something restricts the
	 * output associated with a flag that's turned on, we need to
	 * talk about that flag even if "debug all" is turned on.
	 */
	willprint = (!*(iprouting_debug_arr[i].var) != !debug_all_p());
	if ((i == DEBUG_EGP) && egp_debug && (egp_debug_addr != 0))
	    willprint = TRUE;
	if ((i == DEBUG_EGPEVENT) && egpevent_debug && (egp_debug_addr != 0))
	    willprint = TRUE;
	if ((i == DEBUG_IGRP) && igrp_debug && (igrp_debug_addr != 0))
	    willprint = TRUE;
	if ((i == DEBUG_IGRP_EVENTS) && igrp_debug_events &&
	    (igrp_debug_addr != 0))
	    willprint = TRUE;
	if ((i == DEBUG_BGPUPDATE) && bgpupdate_debug && (bgpupdate_acl != 0))
	    willprint = TRUE;
	if ((i == DEBUG_BGPDAMP) && bgpdamp_debug && (bgpdamp_acl != 0))
	    willprint = TRUE;
	if ((i == DEBUG_IPCACHE) && ipcache_debug && (ipcache_debug_acl != 0))
	    willprint = TRUE;

	if (willprint) {
	    if (!hdisplayed) {
		printf("\nIP routing:");
		hdisplayed = TRUE;
	    }

	    printf("\n  %s debugging is %s",
		   iprouting_debug_arr[i].txt,
		   *(iprouting_debug_arr[i].var) ? "on" : "off");

	    switch (i) {

	    case DEBUG_EGP:
	    case DEBUG_EGPEVENT:
		if (egp_debug_addr != 0)
		    printf(" for address %i", egp_debug_addr);
		break;

	    case DEBUG_IGRP:
	    case DEBUG_IGRP_EVENTS:
		if (igrp_debug_addr != 0)
		    printf(" for address %i", igrp_debug_addr);
		if (igrp_debug_target && (i == DEBUG_IGRP)) {
		    if (igrp_debug_addr)
			printf("\n    ");
		    printf(" for target network %i", igrp_debug_target);
		}
		break;

	    case DEBUG_BGPUPDATE:
		if (bgpupdate_acl != 0)
		    printf(" for access list %d", bgpupdate_acl);
		break;

	    case DEBUG_BGPDAMP:
		if (bgpdamp_acl != 0)
		    printf(" for access list %d", bgpdamp_acl);
		break;

	    case DEBUG_ROUTING:
		if (iprouting_acl != 0)
		    printf(" for access list %d", iprouting_acl);
		break;

	    case DEBUG_IPCACHE:
		if (ipcache_debug_acl != 0)
		    printf(" for access list %d", ipcache_debug_acl);
		break;

	    case DEBUG_OSPF_SPF_INTRA:
		if (ospf_spf_intra_acl != 0)
		    printf(" for access list %d", ospf_spf_intra_acl);
		break;

	    case DEBUG_OSPF_SPF_INTER:
		if (ospf_spf_inter_acl != 0)
		    printf(" for access list %d", ospf_spf_inter_acl);
		break;

	    case DEBUG_OSPF_SPF_EXT:
		if (ospf_spf_ext_acl != 0)
		    printf(" for access list %d", ospf_spf_ext_acl);
		break;

	    case DEBUG_OSPF_FLOOD:
		if (ospf_flood_acl != 0)
		    printf(" for access list %d", ospf_flood_acl);
		break;

	    }
	}
    }
}


/*
 * iprouting_debug_init is called at IP routing subsystem startup. It registers
 * the routines to maintain and display the IP routing debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when IP routing is started.
 */

void iprouting_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(iprouting_debug_all, "iprouting_debug_all");
    reg_add_debug_show(iprouting_debug_show, "iprouting_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when IP routing is initialized, we want to turn on
     * all the IP routing debugging right now.
     */
    iprouting_debug_all(debug_all_p());
}

































