/* $Id: oir_debug.c,v 3.2.60.1 1996/07/25 09:44:39 snyder Exp $
 * $Source: /release/112/cvs/Xsys/hes/oir_debug.c,v $
 *------------------------------------------------------------------
 * oir_debug.c -- Debugging routines for OIR
 *
 * November 1994, William H. Palmer
 *
 * Copyright (c) 1994-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: oir_debug.c,v $
 * Revision 3.2.60.1  1996/07/25  09:44:39  snyder
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
 * Revision 3.2  1995/11/17  09:19:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:43:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "registry.h"
#include "../h/sys_registry.regh"
#include "../ui/debug.h"
#include "../hes/oir_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "oir_debug_flags.h"

/*
 * oir_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the oir debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void oir_debug_all (boolean flag)
{
    generic_debug_all(oir_debug_arr, flag);
}

/*
 * oir_debug_show is called to display the values of all the OIR
 * debugging variables.
 */

static void oir_debug_show (void)
{
    debug_show_flags(&(oir_debug_arr[0]), "Online Insertion and Removal");
}

/*
 * oir_debug_init is called at oir subsystem startup. It registers
 * the routines to maintain and display the oir debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when oir is started.
 */

void oir_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    (void) registry_add_list(REG_SYS, SERVICE_DEBUG_ALL,
			     oir_debug_all, "oir_debug_all");
    (void) registry_add_list(REG_SYS, SERVICE_DEBUG_SHOW,
			     oir_debug_show, "oir_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when OIR is initialized, we want to turn on
     * all the OIR debugging right now.
     */
    oir_debug_all(debug_all_p());
}
