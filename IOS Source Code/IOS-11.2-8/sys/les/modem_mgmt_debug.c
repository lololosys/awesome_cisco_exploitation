/* $Id: modem_mgmt_debug.c,v 3.1.68.3 1996/07/25 09:50:55 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_mgmt_debug.c,v $
 *------------------------------------------------------------------
 * A dummy file for brasil branch.
 * modem_mgmt_debug.c -- Debugging routines for Modem Management
 *
 * November 1995, Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_mgmt_debug.c,v $
 * Revision 3.1.68.3  1996/07/25  09:50:55  snyder
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
 * Revision 3.1.68.2  1996/07/11  01:11:41  styang
 * CSCdi62212:  code review clean up
 * Branch: California_branch
 * Code review clean up.
 *
 * Revision 3.1.68.1  1996/06/16  21:16:27  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/06  19:36:34  jchlin
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "modem_mgmt_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "modem_mgmt_debug_flags.h"

/*
 * modem_mgmt_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the Modem Mgmt debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

void modem_mgmt_debug_all (boolean flag)
{
    generic_debug_all(modem_mgmt_debug_arr, flag);
}

/*
 * modem_mgmt_debug_show is called to display the values of all the Modem Mgmt
 * debugging variables.
 */

void modem_mgmt_debug_show (void)
{
    debug_show_flags(&(modem_mgmt_debug_arr[0]), "Modem Management");
}

/*
 * modem_mgmt_debug_init is called at Modem Mgmt subsystem startup. It
 * registers the routines to maintain and display the Modem Mgmt debug flags,
 * and makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when Modem Mgmt is started.
 */

void modem_mgmt_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(modem_mgmt_debug_all, "modem_mgmt_debug_all");
    reg_add_debug_show(modem_mgmt_debug_show, "modem_mgmt_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when Modem Mgmt is initialized, we want to turn on
     * all the Modem Mgmt debugging right now.
     */
    modem_mgmt_debug_all(debug_all_p());
}
