/* $Id: rvip_debug.c,v 3.2.60.2 1996/07/25 09:46:18 snyder Exp $
 * $Source: /release/112/cvs/Xsys/if/rvip_debug.c,v $
 *------------------------------------------------------------------
 * rvip_debug.c -- Debugging routines for RVIP
 *
 * July 1995, David Getchell
 *
 * Copyright (c) 1996-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_debug.c,v $
 * Revision 3.2.60.2  1996/07/25  09:46:18  snyder
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
 * Revision 3.2.60.1  1996/04/03  22:49:12  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:31:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:52:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:03:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/19  19:31:49  getchell
 * Placeholder files for RSP VIP support
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../if/rvip_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "rvip_debug_flags.h"

/*
 * rvip_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the rvip debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void rvip_debug_all (boolean flag)
{
    generic_debug_all(rvip_debug_arr, flag);
}

/*
 * rvip_debug_show is called to display the values of all the Rvipnet
 * debugging variables.
 */

static void rvip_debug_show (void)
{
    debug_show_flags(&(rvip_debug_arr[0]), "VIP");
}

/*
 * rvip_debug_init is called at RVIP subsystem startup. It registers
 * the routines to maintain and display the RVIP debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when RVIP is started.
 */

void rvip_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(rvip_debug_all, "rvip_debug_all");
    reg_add_debug_show(rvip_debug_show, "rvip_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when RVIP is initialized, we want to turn on
     * all the RVIP debugging right now.
     */
    rvip_debug_all(debug_all_p());
}
