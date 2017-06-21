/*-------------------------------------------------------------------------
 *
 * $Id: vlan_debug.c,v 3.2.22.1 1996/07/25 09:56:43 snyder Exp $
 *
 * vlan/vlan_debug.c
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1997 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Virtual LAN debugging routines.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: vlan_debug.c,v $
 * Revision 3.2.22.1  1996/07/25  09:56:43  snyder
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
 * Revision 3.2  1996/01/23  08:55:30  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


#include "master.h"

#include "sys_registry.h"

#include "../ui/debug.h"
#include "vlan_debug.h"


/* Declare the initialized text for the debugging array */

#define __DECLARE_DEBUG_ARR__
#include "vlan_debug_flags.h"

/*
 * vlan_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the vLAN debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void vlan_debug_all (boolean flag)
{
    generic_debug_all(vlan_debug_arr, flag);
}


/*
 * vlan_debug_show is called to display the values of all the VLAN debugging
 * variables.
 */

static void vlan_debug_show (void)
{
    debug_show_flags(&(vlan_debug_arr[0]), "vLAN");
}


/*
 * vlan_debug_init is called at vLAN subsystem startup. It registers the
 * routines to maintain and display the vLAN debug flags, and makes sure the
 * flags are set to the appropriate values depending on whether "debug all"
 * is in effect when the vLAN subsystem is started.
 */

void vlan_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned from
     * registry_add_list. Oh, well...
     */

    reg_add_debug_all(vlan_debug_all, "vlan_debug_all");

    reg_add_debug_show(vlan_debug_show, "vlan_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all" is in
     * effect when VLAN is initialized, we want to turn on all the vLAN debugging
     * right now.
     */

    vlan_debug_all(debug_all_p());
}

