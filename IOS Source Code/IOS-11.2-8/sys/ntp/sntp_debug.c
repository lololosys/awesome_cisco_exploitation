/* $Id: sntp_debug.c,v 3.1.2.3 1996/07/25 09:51:41 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ntp/sntp_debug.c,v $
 *------------------------------------------------------------------
 * Debugging routines for SNTP
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sntp_debug.c,v $
 * Revision 3.1.2.3  1996/07/25  09:51:41  snyder
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
 * Revision 3.1.2.2  1996/06/23  18:26:07  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Code review comments.
 *
 * Revision 3.1.2.1  1996/06/23  04:40:25  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add an SNTP client.  This is not included in shipping IOS images.
 *
 * Revision 3.1  1996/06/22  21:14:53  dkatz
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../ntp/sntp_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "sntp_debug_flags.h"

/*
 * sntp_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the SNTP debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

void sntp_debug_all (boolean flag)
{
    generic_debug_all(sntp_debug_arr, flag);
}

/*
 * sntp_debug_show is called to display the values of all the SNTP
 * debugging variables.
 */

void sntp_debug_show (void)
{
    debug_show_flags(&(sntp_debug_arr[0]), "SNTP");
}

/*
 * sntp_debug_init is called at SNTP subsystem startup. It registers
 * the routines to maintain and display the SNTP debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when SNTP is started.
 */

void sntp_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(sntp_debug_all, "sntp_debug_all");
    reg_add_debug_show(sntp_debug_show, "sntp_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when SNTP is initialized, we want to turn on
     * all the SNTP debugging right now.
     */
    sntp_debug_all(debug_all_p());
}
