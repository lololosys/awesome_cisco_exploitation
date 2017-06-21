/* $Id: sse_debug.c,v 3.2.60.2 1996/07/25 09:44:40 snyder Exp $
 * $Source: /release/112/cvs/Xsys/hes/sse_debug.c,v $
 *------------------------------------------------------------------
 * sse_debug.c -- Debugging routines for the sse 
 *
 * June 1993 Tony Li
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sse_debug.c,v $
 * Revision 3.2.60.2  1996/07/25  09:44:40  snyder
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
 * Revision 3.2.60.1  1996/03/18  19:43:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:48:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:20:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:45:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "registry.h"
#include "../h/sys_registry.regh"
#include "../ui/debug.h"
#include "../hes/sse_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "sse_debug_flags.h"

/*
 * sse_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the sse debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void sse_debug_all (boolean flag)
{
    generic_debug_all(sse_debug_arr, flag);
}

/*
 * sse_debug_show is called to display the values of all the Sse
 * debugging variables.
 */

static void sse_debug_show (void)
{
    debug_show_flags(&(sse_debug_arr[0]), "Silicon Switching Engine");
}

/*
 * sse_debug_init is called at sse subsystem startup. It registers
 * the routines to maintain and display the sse debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when sse is started.
 */

void sse_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    (void) registry_add_list(REG_SYS, SERVICE_DEBUG_ALL,
			     sse_debug_all, "sse_debug_all");
    (void) registry_add_list(REG_SYS, SERVICE_DEBUG_SHOW,
			     sse_debug_show, "sse_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when Sse is initialized, we want to turn on
     * all the Sse debugging right now.
     */
    sse_debug_all(debug_all_p());
}
