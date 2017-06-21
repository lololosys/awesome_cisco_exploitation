/* $Id: ipc_debug.c,v 3.2.58.1 1996/07/25 09:47:49 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_debug.c,v $
 *------------------------------------------------------------------
 * ipc_debug.c - debugging code to manage IPC debugging flags.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_debug.c,v $
 * Revision 3.2.58.1  1996/07/25  09:47:49  snyder
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
 * Revision 3.2  1995/11/17  17:28:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:02:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "../ui/debug.h"
#include "ipc_debug_flags.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "ipc_debug_flags.h"


/*
 * ipc_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the IPC debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void ipc_debug_all (boolean debug_flag)
{
    generic_debug_all(ipc_debug_arr, debug_flag);
}

/*
 * ipc_set_debug_all is called when "[no] debug ipc all" is typed at 
 * the exec.
 */

void ipc_set_debug_all (parseinfo *csb)
{
    ipc_debug_all(csb->sense);
}

/*
 * ipc_debug_show is called by the debug_all registry to display the values of
 * all the IPC debugging variables.
 */
static void ipc_show_debug (void)
{
    debug_show_flags(&(ipc_debug_arr[0]), "Interprocess Communications");
}

/*
 * ipc_init_debug is called at IPC subsystem startup. It registers
 * the routines to maintain and display the IPC debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when the IPC is started.
 */

void ipc_init_debug(void)
{

    /*
     * Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(ipc_debug_all, "ipc_debug_all");
    reg_add_debug_show(ipc_show_debug, "ipc_debug_show");

    /*
     * Make sure the debug flags are right at startup. If "debug all"
     * is in effect when the IPC is initialized, we want to turn on
     * all the IPC debugging right now.
     */
    ipc_debug_all(debug_all_p());
}
