/* $Id: vip_debug.c,v 3.2.62.1 1996/07/25 09:54:39 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_debug.c,v $
 *------------------------------------------------------------------
 * vip_debug.c - debugging code to manage VIP debugging flags.
 *
 * March 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_debug.c,v $
 * Revision 3.2.62.1  1996/07/25  09:54:39  snyder
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
 * Revision 3.2  1995/11/17  18:51:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "../ui/debug.h"

#include "vip_debug_flags.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "vip_debug_flags.h"

/*
 * vip_debug_all:
 * Is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the VIP debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void vip_debug_all (boolean debug_flag)
{
    generic_debug_all(vip_debug_arr, debug_flag);
}

/*
 * vip_set_debug_all:
 * Ccalled when "[no] debug vip all" is typed at the exec.
 */

void vip_set_debug_all (parseinfo *csb)
{
    vip_debug_all(csb->sense);
}

/*
 * vip_debug_show:
 * Called by the debug_all registry to display the values of
 * all the VIP debugging variables.
 */
void vip_show_debug (void)
{
    debug_show_flags(&(vip_debug_arr[0]), "VIP System");
}

/*
 * vip_init_debug:
 * Called at VIP subsystem startup. It registers
 * the routines to maintain and display the VIP debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when the system is started.
 */

void vip_init_debug (void)
{
    /*
     * Register for "debug all" and "show debug" events.
     */
    reg_add_debug_all(vip_debug_all, "vip_debug_all");
    reg_add_debug_show(vip_show_debug, "vip_debug_show");

    /*
     * Make sure the debug flags are right at startup.
     */
    vip_debug_all(debug_all_p());
}

/* end of file */
