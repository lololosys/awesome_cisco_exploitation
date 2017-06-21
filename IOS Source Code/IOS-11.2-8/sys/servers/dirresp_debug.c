/* $Id: dirresp_debug.c,v 3.1.18.2 1996/07/25 09:53:51 snyder Exp $
 * $Source: /release/112/cvs/Xsys/servers/dirresp_debug.c,v $
 *------------------------------------------------------------------
 * dirresp_debug.c -- Debugging routines
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dirresp_debug.c,v $
 * Revision 3.1.18.2  1996/07/25  09:53:51  snyder
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
 * Revision 3.1.18.1  1996/07/23  18:32:27  snyder
 * CSCdi63791:  debugging arrays should be const
 *              4112 out of data space, 556 out of image
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/20  22:46:00  pst
 * Initial incorporation of distributed directory agent support
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "../ui/debug.h"

#include "dirresp_debug.h"
/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "dirresp_debug_flags.h"

boolean dirresp_debug;


/*
 * dirresp_debug_command
 *
 * csb->sense = TRUE for "debug", FALSE for "undebug".
 * csb->which = index into flag definition array.
 * GETOBJ(pdb,1) = mempointer to beginning of array... they really should
 *                 have called it "ptr1"...
 * Other objects specific to command codes...
 */

void
dirresp_debug_command (parseinfo *csb)
{
    (const) SETOBJ(pdb,1) = dirresp_debug_arr;
    debug_command(csb);
}

/*
 * dirresp_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void
dirresp_debug_all (boolean flag)
{
    int i;

    for (i = 0; dirresp_debug_arr[i].var != (boolean *) NULL; i++)
	*(dirresp_debug_arr[i].var) = flag;
}

/*
 * dirresp_debug_show
 * Called to display the values of all the DR debugging variables.
 */

static void
dirresp_debug_show (void)
{
    int i;
    boolean willprint;		/* Will we print anything on this line? */

    for (i = 0; dirresp_debug_arr[i].var != (boolean *) NULL; i++) {

	/*
	 * Normally, we only print stuff that doesn't match the current
	 * status of "debug all". However, if something restricts the
	 * output associated with a flag that's turned on, we need to
	 * talk about that flag even if "debug all" is turned on.
	 */
	willprint = (!*(dirresp_debug_arr[i].var) != !debug_all_p());
	if (willprint) {
	    printf("\n  %s debugging is %s", dirresp_debug_arr[i].txt,
		   *(dirresp_debug_arr[i].var) ? "on" : "off");
	}
    }
}


/*
 * dirresp_debug_init is called at director-responder subsystem startup.
 * It registers the routines to maintain and display the debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when we're initialized.
 */

void
dirresp_debug_init (void)
{
    static boolean dirresp_debug_inited = FALSE;
    if (dirresp_debug_inited)
	return;

    reg_add_debug_all(dirresp_debug_all,   "dirresp_debug_all");
    reg_add_debug_show(dirresp_debug_show, "dirresp_debug_show");
    dirresp_debug_all(debug_all_p());
}
