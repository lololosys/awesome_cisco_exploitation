/* $Id: callback_debug.c,v 3.2.62.1 1996/07/25 09:55:51 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ts/callback_debug.c,v $
 *------------------------------------------------------------------
 * Callback debug
 *
 * April 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: callback_debug.c,v $
 * Revision 3.2.62.1  1996/07/25  09:55:51  snyder
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
 * Revision 3.2  1995/11/17  18:52:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:08:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../ts/callback_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "callback_debug_flags.h"

/*
 * callback_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the callback debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void callback_debug_all (boolean flag)
{
    generic_debug_all(callback_debug_arr, flag);
}

/*
 * callback_debug_show is called to display the values of all the callback
 * debugging variables.
 */

static void callback_debug_show (void)
{
    debug_show_flags(&(callback_debug_arr[0]), "Callback");
}

/*
 * callback_debug_init is called at callback startup. It registers
 * the routines to maintain and display the callback debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when callback is started.
 */

void callback_debug_init (void)
{

    /* 
     * Register for "debug all" and "show debug" events. 
     */
    reg_add_debug_all(callback_debug_all, "callback_debug_all");
    reg_add_debug_show(callback_debug_show, "callback_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when callback is initialized, we want to turn on
     * all the callback debugging right now.
     */
    callback_debug_all(debug_all_p());
}
