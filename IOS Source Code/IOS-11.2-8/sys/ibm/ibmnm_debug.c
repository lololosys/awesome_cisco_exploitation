/* $Id: ibmnm_debug.c,v 3.2.60.1 1996/07/25 09:45:20 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ibm/ibmnm_debug.c,v $
 *------------------------------------------------------------------
 * ibmnm_debug.c -- Debugging routines for LAN Net Manager
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibmnm_debug.c,v $
 * Revision 3.2.60.1  1996/07/25  09:45:20  snyder
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
 * Revision 3.2  1995/11/17  09:22:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:46:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../ibm/ibmnm_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "ibmnm_debug_flags.h"

/*
 * ibmnm_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the LNM debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void ibmnm_debug_all (boolean flag)
{
    generic_debug_all(ibmnm_debug_arr, flag);
}

/*
 * ibmnm_debug_show is called to display the values of all the LNM
 * debugging variables.
 */

static void ibmnm_debug_show (void)
{
    debug_show_flags(&(ibmnm_debug_arr[0]), "LAN Net Manager");
}

/*
 * ibmnm_debug_init is called at LNM subsystem startup. It registers
 * the routines to maintain and display the LNM debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when LNM is started.
 */

void ibmnm_debug_init (void)
{
    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(ibmnm_debug_all, "ibmnm_debug_all");
    reg_add_debug_show(ibmnm_debug_show, "ibmnm_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when LNM is initialized, we want to turn on
     * all the LNM debugging right now.
     */
    ibmnm_debug_all(debug_all_p());
}
