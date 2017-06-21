/* $Id: v120_debug.c,v 3.2.62.1 1996/07/25 09:57:26 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/v120_debug.c,v $
 *------------------------------------------------------------------
 * v120_debug.c -- Debugging routines for v120
 *
 * July, 1995 Bill May
 *
 * Copyright (c) 1995-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: v120_debug.c,v $
 * Revision 3.2.62.1  1996/07/25  09:57:26  snyder
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
 * Revision 3.2  1995/11/17  18:05:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:37:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  23:30:28  wmay
 * Placeholder for CS_Arkansas_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../wan/v120_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "v120_debug_flags.h"

/*
 * v120_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the v120 debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void v120_debug_all (boolean flag)
{
    generic_debug_all(v120_debug_arr, flag);
}

/*
 * v120_debug_show is called to display the values of all the v120
 * debugging variables.
 */

static void v120_debug_show (void)
{
    debug_show_flags(&(v120_debug_arr[0]), "v120");
}

/*
 * v120_debug_init is called at v120 subsystem startup. It registers
 * the routines to maintain and display the v120 debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when v120 is started.
 */

void v120_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(v120_debug_all, "v120_debug_all");
    reg_add_debug_show(v120_debug_show, "v120_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when v120 is initialized, we want to turn on
     * all the v120 debugging right now.
     */
    v120_debug_all(debug_all_p());
}
