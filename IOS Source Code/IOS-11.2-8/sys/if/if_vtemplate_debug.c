/* $Id: if_vtemplate_debug.c,v 3.1.34.2 1996/07/25 09:46:16 snyder Exp $
 * $Source: /release/112/cvs/Xsys/if/if_vtemplate_debug.c,v $
 *------------------------------------------------------------------
 * if_vtemplate_debug.c - Debug support for Virtual Template 
 * 
 * February 1996, Perry Leung 
 *
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vtemplate_debug.c,v $
 * Revision 3.1.34.2  1996/07/25  09:46:16  snyder
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
 * Revision 3.1.34.1  1996/04/27  05:49:46  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.2.1  1996/02/10  02:19:41  perryl
 * Branch: Dial1_branch
 * Add debug support for Virtual Template
 *
 * Revision 3.1  1996/02/10  00:27:08  perryl
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../if/if_vtemplate_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "if_vtemplate_debug_flags.h"

/*
 * vtemplate_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the VTEMPLATE debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void vtemplate_debug_all (boolean flag)
{
    generic_debug_all(vtemplate_debug_arr, flag);
}

/*
 * vtemplate_debug_show is called to display the values of all the VTEMPLATE
 * debugging variables.
 */

static void vtemplate_debug_show (void)
{
    debug_show_flags(&(vtemplate_debug_arr[0]), "VTEMPLATE");
}

/*
 * vtemplate_debug_init is called at VTEMPLATE subsystem startup. It registers
 * the routines to maintain and display the VTEMPLATE debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when VTEMPLATE is started.
 */

void vtemplate_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(vtemplate_debug_all, "vtemplate_debug_all");
    reg_add_debug_show(vtemplate_debug_show, "vtemplate_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when VTEMPLATE is initialized, we want to turn on
     * all the VTEMPLATE debugging right now.
     */
    vtemplate_debug_all(debug_all_p());
}
