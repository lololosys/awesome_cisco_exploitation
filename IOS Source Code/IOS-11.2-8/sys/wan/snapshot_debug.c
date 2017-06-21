/* $Id: snapshot_debug.c,v 3.2.62.1 1996/07/25 09:57:25 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/snapshot_debug.c,v $
 *------------------------------------------------------------------
 * snapshot_debug.c -- Debugging routines for Snapshot support
 *
 * April 1994, Joel P. Bion
 *
 * Copyright (c) 1994-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snapshot_debug.c,v $
 * Revision 3.2.62.1  1996/07/25  09:57:25  snyder
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
 * Revision 3.2  1995/11/17  18:04:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../wan/snapshot_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "snapshot_debug_flags.h"

/*
 * snapshot_debug_all
 *
 * This is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the Snapshot debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */
static void snapshot_debug_all (boolean flag)
{
    generic_debug_all(snapshot_debug_arr, flag);
}

/*
 * snapshot_debug_show
 *
 * This is called to display the values of all the
 * Snapshot debugging variables.
 */
static void snapshot_debug_show (void)
{
    debug_show_flags(&(snapshot_debug_arr[0]), "Snapshot");
}

/*
 * snapshot_debug_init
 *
 * This is called at Snapshot subsystem startup. It
 * registers the routines to maintain and display the Snapshot debug
 * flags, and makes sure the flags are set to the appropriate values
 * depending on whether "debug all" is in effect when Snapshot is
 * started.
 */
void snapshot_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. */
    reg_add_debug_all(snapshot_debug_all, "snapshot_debug_all");
    reg_add_debug_show(snapshot_debug_show, "snapshot_debug_show");

    /*
     * Make sure the debug flags are right at startup. If "debug all"
     * is in effect when Snapshot is initialized, we want to turn on
     * all the Snapshot debugging right now.
     */
    snapshot_debug_all(debug_all_p());
}

