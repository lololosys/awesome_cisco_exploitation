/* $Id: modem_config_debug.c,v 3.1.14.1 1996/07/25 09:55:52 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ts/modem_config_debug.c,v $
 *------------------------------------------------------------------
 * Modem Autoconfig debug
 *
 * Febuary 1996, Tim Kolar
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_config_debug.c,v $
 * Revision 3.1.14.1  1996/07/25  09:55:52  snyder
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
 * Revision 3.1  1996/02/23  04:12:03  tkolar
 * CSCdi49648:  Modem autoconfig break modularity rules
 * Move confmodem_debug out of the commserver subsystem.
 *
 * $Endlog$
 */
#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../ts/modem_config_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "modem_config_debug_flags.h"

/*
 * modem_config_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the modem_config debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void modem_config_debug_all (boolean flag)
{
    generic_debug_all(modem_config_debug_arr, flag);
}

/*
 * modem_config_debug_show is called to display the values of all the 
 * autoconfig debugging variables.
 */

static void modem_config_debug_show (void)
{
    debug_show_flags(&(modem_config_debug_arr[0]), "Modem Autoconfig");
}

/*
 * modem_config_debug_init is called at modem_config startup. It registers
 * the routines to maintain and display the modem_config debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when modem_config is started.
 */

void modem_config_debug_init (void)
{

    /* 
     * Register for "debug all" and "show debug" events. 
     */
    reg_add_debug_all(modem_config_debug_all, "modem_config_debug_all");
    reg_add_debug_show(modem_config_debug_show, "modem_config_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when modem_config is initialized, we want to turn on
     * all the modem_config debugging right now.
     */
    modem_config_debug_all(debug_all_p());
}
