/* $Id: netbios_as_debug.c,v 3.2.60.1 1996/07/25 09:45:22 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ibm/netbios_as_debug.c,v $
 *------------------------------------------------------------------
 * netbios_as_debug.c -- Debugging routines for NBF Access Server
 *
 * September 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_as_debug.c,v $
 * Revision 3.2.60.1  1996/07/25  09:45:22  snyder
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
 * Revision 3.2  1995/11/17  09:23:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  00:32:32  syiu
 * Initial placeholder for NBF.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../ibm/netbios_as_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "netbios_as_debug_flags.h"

/*
 * netbios_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the NETBIOS debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void netbios_as_debug_all (boolean flag)
{
    generic_debug_all(netbios_as_debug_arr, flag);
}

/*
 * netbios_as_debug_show is called to display the values of all the NETBIOS
 * debugging variables.
 */

static void netbios_as_debug_show (void)
{
    debug_show_flags(&(netbios_as_debug_arr[0]), "NETBIOS");
}

/*
 * netbios_as_debug_init is called at NETBIOS subsystem startup. It registers
 * the routines to maintain and display the NETBIOS debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when NETBIOS is started.
 */

void netbios_as_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(netbios_as_debug_all, "netbios_as_debug_all");
    reg_add_debug_show(netbios_as_debug_show, "netbios_as_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when NETBIOS is initialized, we want to turn on
     * all the NETBIOS debugging right now.
     */
    netbios_as_debug_all(debug_all_p());
}
