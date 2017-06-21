/* $Id: bri_debug.c,v 3.1.60.1 1996/07/25 09:46:10 snyder Exp $
 * $Source: /release/112/cvs/Xsys/if/bri_debug.c,v $
 *------------------------------------------------------------------
 * bri_debug.c -- Debugging routines for BRI.
 *
 * March 1994, Manoj Leelanivas.
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../if/bri_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "bri_debug_flags.h"

/*
 * bri_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the BRI debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

void bri_debug_all (boolean flag)
{
    generic_debug_all(bri_debug_arr, flag);
}

/*
 * bri_debug_show is called to display the values of all the BRI
 * debugging variables.
 */

void bri_debug_show (void)
{
    debug_show_flags(&(bri_debug_arr[0]), "General BRI");
}

/*
 * bri_debug_init is called at BRI subsystem startup. It registers
 * the routines to maintain and display the BRI debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when BRI is started.
*/

void bri_debug_init (void)
{
    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(bri_debug_all, "bri_debug_all");
    reg_add_debug_show(bri_debug_show, "bri_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when BRI is initialized, we want to turn on
     * all the BRI debugging right now.
     */
    bri_debug_all(debug_all_p());
}



