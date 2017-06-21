/* $Id: hub_debug.c,v 3.2.62.1 1996/07/25 09:50:52 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/hub_debug.c,v $
 *------------------------------------------------------------------
 * hub_debug.c -- Debugging routines for Hub repeater
 *
 * March 1994, Viet Nguyen
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Viet Nguyen
 *------------------------------------------------------------------
 * $Log: hub_debug.c,v $
 * Revision 3.2.62.1  1996/07/25  09:50:52  snyder
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
 * Revision 3.2  1995/11/17  17:37:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:31:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../les/hub_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "hub_debug_flags.h"

/*
 * =====================================================================
 * hub_debug_all()
 *
 * Description:
 * hub_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the pcbus debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 *
 * Parameters:
 * flag : boolean
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_debug_all (boolean flag)
{
    generic_debug_all(hub_debug_arr, flag);
}

/*
 * =====================================================================
 * hub_debug_show()
 * 
 * Description:
 * Display the values of all the hub debugging variables.
 * 
 * Parameters:
 * None
 * 
 * Returns:
 * None
 * =====================================================================
 */
void hub_debug_show (void)
{
    debug_show_flags(&(hub_debug_arr[0]), "Hub");
}

/*
 * =====================================================================
 * hub_debug_init()
 * 
 * Description:
 * hub_debug_init is called at hub subsystem startup. It registers
 * the routines to maintain and display the hub debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when hub is started.
 *
 * Parameters:
 * None
 * 
 * Returns:
 * None
 * =====================================================================
 */
void hub_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(hub_debug_all, "hub_debug_all");
    reg_add_debug_show(hub_debug_show, "hub_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when hub is initialized, we want to turn on
     * all the hub debugging right now.
     */
    hub_debug_all(debug_all_p());
}
