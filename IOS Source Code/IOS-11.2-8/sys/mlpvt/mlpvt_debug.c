/* $Id: mlpvt_debug.c,v 1.1.42.2 1996/07/25 09:51:33 snyder Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt_debug.c,v $
 *------------------------------------------------------------------
 * mlpvt_debug.c -- Debugging support for MLPVT
 *
 * January 1996, Ashwin Baindur
 *
 * Copyright (c) 1996-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt_debug.c,v $
 * Revision 1.1.42.2  1996/07/25  09:51:33  snyder
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
 * Revision 1.1.42.1  1996/04/27  06:34:26  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.2  1996/03/05  02:46:56  agh
 * Branch: Dial1_branch
 * Much cleanup of unwanted code, also added comments.
 *
 * Revision 1.1.2.1  1996/01/26  20:28:40  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:37  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../mlpvt/mlpvt_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "../mlpvt/mlpvt_debug_flags.h"

/*
 * mlpvt_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the mlpvt debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void mlpvt_debug_all (boolean flag)
{
    generic_debug_all(mlpvt_debug_arr, flag);
}

/*
 * mlpvt_debug_show is called to display the values of all the mlpvt
 * debugging variables.
 */

static void mlpvt_debug_show (void)
{
    debug_show_flags(&(mlpvt_debug_arr[0]), "MLPVT group");
}

/*
 * mlpvt_debug_init is called at mlpvt subsystem startup.
 * It registers the routines to maintain and display the mlpvt
 * debug flags, and makes sure the flags are set to the appropriate
 * values depending on whether "debug all" is in effect when mlpvt
 * is initialized.
 */

void mlpvt_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(mlpvt_debug_all, "mlpvt_debug_all");
    reg_add_debug_show(mlpvt_debug_show, "mlpvt_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when mlpvt is initialized, we want to turn on
     * all the mlpvt debugging right now.
     */
    mlpvt_debug_all(debug_all_p());
}
