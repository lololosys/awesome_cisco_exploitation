/* $Id: xremote_debug.c,v 3.2.60.1 1996/07/25 09:59:16 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xremote/xremote_debug.c,v $
 *------------------------------------------------------------------
 * xremote_debug.c -- Debugging routines for XRemote
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xremote_debug.c,v $
 * Revision 3.2.60.1  1996/07/25  09:59:16  snyder
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
 * Revision 3.2  1995/11/17  19:26:55  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:35:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../xremote/xremote_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "xremote_debug_flags.h"

/*
 * xr_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the XRemote debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void xr_debug_all (boolean flag)
{
    generic_debug_all(xr_debug_arr, flag);
}

/*
 * xr_debug_show is called to display the values of all the XRemote
 * debugging variables.
 */

static void xr_debug_show (void)
{
    debug_show_flags(&(xr_debug_arr[0]), "XRemote");
}

/*
 * xr_debug_init is called at XRemote subsystem startup. It registers
 * the routines to maintain and display the XRemote debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when XRemote is started.
 */

void xr_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(xr_debug_all, "xr_debug_all");
    reg_add_debug_show(xr_debug_show, "xr_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when XRemote is initialized, we want to turn on
     * all the XRemote debugging right now.
     */
    xr_debug_all(debug_all_p());
}
