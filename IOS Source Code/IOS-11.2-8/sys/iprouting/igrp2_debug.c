/* $Id: igrp2_debug.c,v 3.2.60.2 1996/07/25 09:48:30 snyder Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/igrp2_debug.c,v $
 *------------------------------------------------------------------
 * IGRP2 debugging
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: igrp2_debug.c,v $
 * Revision 3.2.60.2  1996/07/25  09:48:30  snyder
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
 * Revision 3.2.60.1  1996/04/02  05:37:06  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2  1995/11/17  17:34:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:06:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "igrp2_debug.h"
#include "dual.h"
#include "parser_defs_igrp2.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "igrp2_debug_flags.h"

ulong igrp2_packet_debug_flags;
ulong igrp2_xmit_debug_flags;


/*
 * igrp2_display_packet_debugs_delimiter
 *
 * Handle the printing of the delimiters between flags.
 */
static boolean igrp2_display_packet_debugs_delimiter (boolean first_time)
{
    if (first_time) {
	printf("\n    (");
	first_time = FALSE;
    } else {
	printf(", ");
    }
    return(first_time);
}

/*
 * igrp2_display_packet_debugs
 *
 * List packet types subject to debugging
 */
static void igrp2_display_packet_debugs (void)
{
    ulong index;
    boolean first_time;

    first_time = TRUE;

    for (index = 0; index < IGRP_NUM_OPCODES; index++) {
	if (igrp2_packet_debug_flags & (1 << index)) {
	    first_time = igrp2_display_packet_debugs_delimiter(first_time);
	    printf(igrp2_otype(index));
	}
    }
    if (igrp2_packet_debug_flags & IGRP2_DEBUG_RETRY) {
	first_time = igrp2_display_packet_debugs_delimiter(first_time);
	printf("RETRY");
    }
    if (!first_time) {
	printf(")");
    }
}

/*
 * igrp2_display_xmit_debugs
 *
 * List transmit events subject to debugging
 */
static char *const xmit_debug_names[XDB_NUMFLAGS] =
    {"ACK", "PACKETIZE", "STARTUP", "PEERDOWN",
     "LINK", "BUILD", "STRANGE", "DETAIL"};
static void igrp2_display_xmit_debugs (void)
{
    ulong index;
    boolean first_time;

    first_time = TRUE;

    for (index = 0; index < XDB_NUMFLAGS; index++) {
	if (igrp2_xmit_debug_flags & (1 << index)) {
	    if (first_time) {
		printf("\n    (");
		first_time = FALSE;
	    } else {
		printf(", ");
	    }
	    printf(xmit_debug_names[index]);
	}
    }
    if (igrp2_xmit_debug_flags) {
	printf(")");
    }
}

/*
 * igrp2_debug_command
 * Handles debug commands related to IGRP2
 *
 */
void igrp2_debug_command (parseinfo *csb)
{
    switch (csb->which) {
      case DEBUG_IGRP2_PKT:
	igrp2_packet_debug = csb->sense;
	if (csb->sense) {

	    /*
	     * Pick up the flags.  If no keywords (other than possibly
	     * "retry") were specified, set the verbose flags.
	     */
	    igrp2_packet_debug_flags = GETOBJ(int,1);
	    if (!(igrp2_packet_debug_flags & (~IGRP2_DEBUG_RETRY))) {
		igrp2_packet_debug_flags |= IGRP2_DEBUG_VERBOSE;
	    }
	} else {
	    igrp2_packet_debug_flags = 0;
	}
	debug_show_flag(igrp2_packet_debug, TRUE,
			igrp2_debug_arr[DEBUG_IGRP2_PKT].txt);
	igrp2_display_packet_debugs();
	break;
      case DEBUG_IGRP2_FSM:
	igrp2_fsm_debug = csb->sense;
	debug_show_flag(igrp2_fsm_debug, TRUE,
			igrp2_debug_arr[DEBUG_IGRP2_FSM].txt);
	break;
      case DEBUG_IGRP2_NBRS:
	igrp2_nbrs_debug = csb->sense;
	debug_show_flag(igrp2_nbrs_debug, TRUE,
			igrp2_debug_arr[DEBUG_IGRP2_NBRS].txt);
	break;
      case DEBUG_IGRP2_XMIT:
	igrp2_xmit_debug = csb->sense;
	if (csb->sense) {
	    igrp2_xmit_debug_flags = GETOBJ(int,1);
	    if (!igrp2_xmit_debug_flags) /* No keywords? */
		igrp2_xmit_debug_flags = ~0; /* Turn on everything */
	} else {
	    igrp2_xmit_debug_flags = 0;
	}
	debug_show_flag(igrp2_xmit_debug, TRUE,
			igrp2_debug_arr[DEBUG_IGRP2_XMIT].txt);
	igrp2_display_xmit_debugs();
	break;
      default:
	/* If there's no special case, use the generic handler */
	debug_command(csb);
	break;
    }
}


/*
 * igrp2_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the IGRP2 debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void igrp2_debug_all (boolean sense)
{
    if (!sense) {
	igrp2_packet_debug_flags = 0;
    }
    generic_debug_all(igrp2_debug_arr, sense);
}


/*
 * igrp2_debug_show
 *
 * Display the values of all the IGRP2 debug variables
 */
static void igrp2_debug_show (void)
{
    int i;
    boolean hdisplayed = FALSE; /* Printed the heading yet? */
    boolean willprint;          /* Will we print anything on this line? */

    for (i = 0; igrp2_debug_arr[i].var != (boolean *) NULL; i++) {

	/*
	 * Normally, we only print stuff that doesn't match the current
	 * status of "debug all". However, if something restricts the
	 * output associated with a flag that's turned on, we need to
	 * talk about that flag even if "debug all" is turned on.
	 */

	willprint = (!*(igrp2_debug_arr[i].var) != !debug_all_p());
	if (willprint) {
	    if (!hdisplayed) {
		printf("\nEIGRP:");
		hdisplayed = TRUE;
	    }

	    printf("\n  %s debugging is %s",
		   igrp2_debug_arr[i].txt,
		   *(igrp2_debug_arr[i].var) ? "on" : "off");
	    if (i == DEBUG_IGRP2_PKT)
		igrp2_display_packet_debugs();
	    if (i == DEBUG_IGRP2_XMIT)
		igrp2_display_xmit_debugs();
	}
    }
}


/*
 * igrp2_debug_init
 *
 * Register the igrp2 debug routines
 */
void igrp2_debug_init (void)
{

    reg_add_debug_all(igrp2_debug_all, "igrp2_debug_all");
    reg_add_debug_show(igrp2_debug_show, "igrp2_debug_show");

    igrp2_debug_all(debug_all_p());
}
