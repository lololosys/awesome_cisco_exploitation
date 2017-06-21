/* $Id: x25_debug.c,v 3.2.62.1 1996/07/25 09:58:06 snyder Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_debug.c,v $
 *------------------------------------------------------------------
 * x25_debug.c -- Debugging routines for X.25
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_debug.c,v $
 * Revision 3.2.62.1  1996/07/25  09:58:06  snyder
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
 * Revision 3.2  1995/11/17  18:07:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:22:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "../ui/debug.h"
#include "../x25/x25_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "x25_debug_flags.h"

/* Other initialized storage */

int x25_vcnum = 0;		/* Restricts debugging output */



/*
 * x25_debug_command
 * This is the routine the parser calls for X.25 debug commands. It
 * handles the special case of the "debug x25 vc" command and punts
 * everything else to the generic handler.
 */
void x25_debug_command (parseinfo *csb)
{
    if (csb->which == DEBUG_X25VC) {
	if (csb->sense && (x25_vcnum = GETOBJ(int,1))) {
	    if (!x25_debug && !x25event_debug)
		printf("\nX.25 debugging output not specified");
	    else
		printf("\nX.25 debugging output restricted to VC %d",
			x25_vcnum);
	} else {
	    x25_vcnum = 0;
	    printf("\nX.25 debugging output not restricted");
	}
    } else {
	/*
	 *  Standard debuggging and event debugging are somewhat generic;
	 *  event debugging is a subset of standard, so clear both flags
	 *  before-hand so both won't be set
	 */
	if (csb->sense) {
	    x25_debug = FALSE;
	    x25event_debug = FALSE;
	}
	debug_command(csb);
	/*
	 *  try for least astonishment by turning off either debug mode if
	 *  either of the "no debug" forms is used
	 */
	if (!csb->sense) {
	    x25_debug = FALSE;
	    x25event_debug = FALSE;
	}
    }
}


/*
 * x25_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the X.25 debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void x25_debug_all (boolean flag)
{
    x25_debug = flag;
    x25event_debug = FALSE;
    x25_vcnum = 0;
}

/*
 * x25_debug_show is called to display the values of all the X.25
 * debugging variables.
 */

static void x25_debug_show (void)
{
    char *basehead = "X.25";
    char *heading = basehead;

    if (x25_vcnum != 0) {
	heading = malloc(64);
	if (heading != NULL)
	    sprintf(heading, "X.25 (debugging restricted to VC number %d)",
		    x25_vcnum);
    }
    debug_show_flags(&(x25_debug_arr[0]), heading);
    if ((heading != NULL) && (heading != basehead))
	free(heading);
}

/*
 * x25_debug_init is called at X.25 subsystem startup. It registers
 * the routines to maintain and display the X.25 debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when X.25 is started.
 */

void x25_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(x25_debug_all, "x25_debug_all");
    reg_add_debug_show(x25_debug_show, "x25_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when X.25 is initialized, we want to turn on
     * all the X.25 debugging right now.
     */
    x25_debug_all(debug_all_p());
}
