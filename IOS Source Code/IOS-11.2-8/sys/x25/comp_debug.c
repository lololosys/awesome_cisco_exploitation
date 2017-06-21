/* $Id: comp_debug.c,v 3.2.60.1 1996/07/25 09:58:03 snyder Exp $
 * $Source: /release/112/cvs/Xsys/x25/comp_debug.c,v $
 *------------------------------------------------------------------
 * COMP_DEBUG.C
 *
 * 10/27/93 Bill Thomas
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: comp_debug.c,v $
 * Revision 3.2.60.1  1996/07/25  09:58:03  snyder
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
 * Revision 3.2  1995/11/17  18:05:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../x25/comp_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "comp_debug_flags.h"

/*
 * comp_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the COMPRESS debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void comp_debug_all (boolean flag)
{
    generic_debug_all(compress_debug_arr, flag);
}

/*
 * comp_debug_show is called to display the values of all the COMPRESS 
 * debugging variables.
 */

static void comp_debug_show (void)
{
    debug_show_flags(&(compress_debug_arr[0]), "COMPRESS");
}

/*
 * comp_debug_init is called at COMPRESS subsystem startup. It registers
 * the routines to maintain and display the COMPRESS debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when COMPRESS is started.
 */

void compress_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    (void) reg_add_debug_all(comp_debug_all, "comp_debug_all");
    (void) reg_add_debug_show(comp_debug_show, "comp_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when COMPRESS is initialized, we want to turn on
     * all the COMPRESS debugging right now.
     */
    comp_debug_all(debug_all_p());
}
