/* $Id: vines_debug.c,v 3.2.62.1 1996/07/25 09:56:31 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_debug.c,v $
 *------------------------------------------------------------------
 * vines_debug.c -- Debugging routines for Banyan VINES
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_debug.c,v $
 * Revision 3.2.62.1  1996/07/25  09:56:31  snyder
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
 * Revision 3.2  1995/11/17  17:57:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:14:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../vines/vines_debug.h"
#include "../vines/vines_private.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "vines_debug_flags.h"

int vinespkt_debug_list;
int vinesrtp_debug_list;
int vinestable_debug_list;

/*
 * vines_debug_command
 * This is the routine the parser calls for VINES debug commands. It
 * handles the special case of an access list number on "debug vines
 * routing", and punts everything else to the generic handler.
 */
void vines_debug_command (parseinfo *csb)
{
    boolean verbose;
    int list = 0;

    if (csb->which == 0)
	csb->which = GETOBJ(int,10);
    if (csb->which == DEBUG_VINESPKT) {
	vinespkt_debug_list = list = GETOBJ(int,1);
    } else if (csb->which == DEBUG_VINESRTP) {
	vinesrtp_debug_list = list = GETOBJ(int,1);
    } else if (csb->which == DEBUG_VINESTABLE) {
	vinestable_debug_list = list = GETOBJ(int,1);
    }
    
    /*
     * Convert a command in the form "NO DEBUG xxx VERBOSE" into its
     * equivalent positive form "DEBUG xxx".
     */
    verbose = GETOBJ(int,9);
    if (!csb->sense && verbose) {
	csb->sense = TRUE;
	verbose = FALSE;
    }

    /*
     * Set up the flag and confirm the change to the user.
     * Filched from debug.c
     */
    *(vines_debug_arr[csb->which].var) = csb->sense;
    if (verbose)
	*(vines_debug_arr[csb->which].var) |= VERBOSE_DEBUG;
    debug_show_flag(*(vines_debug_arr[csb->which].var), TRUE,
		    vines_debug_arr[csb->which].txt);

    /*
     * Now print any tags, just to confirm additional selections.
     */
    if (verbose)
	printf(" (verbose)");
    if (list != 0)
	printf(" (filter list %d)", list);
}

/*
 * vines_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the VINES debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void vines_debug_all (boolean flag)
{
    generic_debug_all(vines_debug_arr, flag);
}

/*
 * vines_debug_show is called to display the values of all the VINES
 * debugging variables.
 */

static void vines_debug_show (void)
{
    boolean hdisplayed = FALSE;
    int list, i;

    for (i = 0; vines_debug_arr[i].var != (boolean *) NULL; i++) {
	if (!(*(vines_debug_arr[i].var)) != !debug_all_p()) {
	    if (!hdisplayed) {
		printf("\n%s:", "Banyan VINES");
		hdisplayed = TRUE;
	    }
	    printf ("\n  %s debugging is %s", vines_debug_arr[i].txt,
		    (*vines_debug_arr[i].var) ? "on" : "off");
	    if ((*vines_debug_arr[i].var) & VERBOSE_DEBUG)
		printf(" (verbose)");
	    if (vines_debug_arr[i].var == &vinespkt_debug)
		list = vinespkt_debug_list;
	    else if (vines_debug_arr[i].var == &vinesrtp_debug)
		list = vinesrtp_debug_list;
	    else if (vines_debug_arr[i].var == &vinestable_debug)
		list = vinestable_debug_list;
	    else {
		list = 0;
	    }
	    if (list) {
		printf(" (filter list %d)", list);
	    }
	}
    }
}

/*
 * vines_debug_init is called at VINES subsystem startup. It registers
 * the routines to maintain and display the VINES debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when VINES is started.
 */

void vines_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(vines_debug_all, "vines_debug_all");
    reg_add_debug_show(vines_debug_show, "vines_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when VINES is initialized, we want to turn on
     * all the VINES debugging right now.
     */
    vines_debug_all(debug_all_p());
}
