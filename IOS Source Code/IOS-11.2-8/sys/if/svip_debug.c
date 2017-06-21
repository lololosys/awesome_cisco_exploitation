/* $Id: svip_debug.c,v 3.1.58.3 1996/07/25 09:46:19 snyder Exp $
 * $Source: /release/112/cvs/Xsys/if/svip_debug.c,v $
 *------------------------------------------------------------------
 * svip_debug.c -- Debugging routines for SVIP
 *
 * November 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_debug.c,v $
 * Revision 3.1.58.3  1996/07/25  09:46:19  snyder
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
 * Revision 3.1.58.2  1996/04/03  22:49:14  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.1.58.1  1996/03/21  22:49:14  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.6.1  1996/02/10  00:05:03  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1996/01/02  06:27:09  getchell
 * Placeholder for VIPER development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../if/svip_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "svip_debug_flags.h"

/*
 * svip_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the svip debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

void svip_debug_all (boolean flag)
{
    generic_debug_all(svip_debug_arr, flag);
}

/*
 * svip_debug_show is called to display the values of all the svipnet
 * debugging variables.
 */

void svip_debug_show (void)
{
    debug_show_flags(&(svip_debug_arr[0]), "VIP2");
}

/*
 * svip_debug_init is called at SVIP subsystem startup. It registers
 * the routines to maintain and display the SVIP debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when SVIP is started.
 */

void svip_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(svip_debug_all, "svip_debug_all");
    reg_add_debug_show(svip_debug_show, "svip_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when SVIP is initialized, we want to turn on
     * all the SVIP debugging right now.
     */
    svip_debug_all(debug_all_p());
}

/* end of file */
