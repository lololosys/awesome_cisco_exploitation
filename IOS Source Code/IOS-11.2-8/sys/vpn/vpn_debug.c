/* $Id: vpn_debug.c,v 1.2.40.1 1996/07/25 09:56:45 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vpn/vpn_debug.c,v $
 *------------------------------------------------------------------
 * vpn_debug.c   --  debugging support for VPN
 *
 * January 1996, Shoou Yiu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vpn_debug.c,v $
 * Revision 1.2.40.1  1996/07/25  09:56:45  snyder
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
 * Revision 1.2  1996/01/31  18:38:42  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:13  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../vpn/vpn_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "vpn_debug_flags.h"

/*
 * vpn_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the VPN debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void vpn_debug_all (boolean flag)
{
    generic_debug_all(vpn_debug_arr, flag);
}

/*
 * vpn_debug_show is called to display the values of all the VPN
 * debugging variables.
 */

static void vpn_debug_show (void)
{
    debug_show_flags(&(vpn_debug_arr[0]), "VPN");
}

/*
 * vpn_debug_init is called at VPN subsystem startup. It registers
 * the routines to maintain and display the VPN debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when VPN is started.
 */

void vpn_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(vpn_debug_all, "vpn_debug_all");
    reg_add_debug_show(vpn_debug_show, "vpn_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when VPN is initialized, we want to turn on
     * all the VPN debugging right now.
     */
    vpn_debug_all(debug_all_p());
}
