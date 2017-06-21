/* $Id: spx_debug.c,v 3.2.62.2 1996/07/25 09:58:48 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xns/spx_debug.c,v $
 *------------------------------------------------------------------
 * spx_debug.c   Sequenced Packet Exchange Protocol  (Debug flags
 *               support code)
 *
 * September 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: spx_debug.c,v $
 * Revision 3.2.62.2  1996/07/25  09:58:48  snyder
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
 * Revision 3.2.62.1  1996/07/23  18:32:55  snyder
 * CSCdi63791:  debugging arrays should be const
 *              4112 out of data space, 556 out of image
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  19:24:48  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:40:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  11:57:15  irfan
 * Sequenced Packet Exchange Protocol: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "config.h"
#include "spx_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "spx_debug_flags.h"

boolean spx_debug;

/*
 * spx_debug_command
 * This is the routine the parser calls for Spx debug commands. It
 * handles the special case of a line number on "debug spx line",
 * and punts everything else to the generic handler.
 */

void spx_debug_command (parseinfo *csb)
{

    (const) SETOBJ(pdb,1) = spx_debug_arr;

    /* Everything else is generic */
    debug_command(csb);

    if (csb->sense){
	spx_debug = TRUE;
    }
    else {
	spx_debug = FALSE;
    }
}

/*
 * spx_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the spx debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

void spx_debug_all (boolean flag)
{
    generic_debug_all(spx_debug_arr, flag);
}

/*
 * spx_debug_show is called to display the values of all the spx
 * debugging variables.
 */

void spx_debug_show (void)
{
    debug_show_flags(&(spx_debug_arr[0]), "SPX");
}

/*
 * spx_debug_init is called at spx subsystem startup. It registers
 * the routines to maintain and display the spx debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when spx is started.
 */

void spx_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(spx_debug_all, "spx_debug_all");
    reg_add_debug_show(spx_debug_show, "spx_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when spx is initialized, we want to turn on
     * all the spx debugging right now.
     */
    spx_debug_all(debug_all_p());
}
