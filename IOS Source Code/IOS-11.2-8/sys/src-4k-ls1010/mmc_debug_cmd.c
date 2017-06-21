/* $Id: mmc_debug_cmd.c,v 3.1 1995/11/09 13:23:43 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-ls1010/mmc_debug_cmd.c,v $
 *------------------------------------------------------------------
 * parser routines
 *
 * September 1995, Ram Bhide.
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * Revision 2.2  1995/08/03  00:28:55  jjohnson
 * CSCdi38141:  template files contain bogus log information
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "mmc_debug_cmd.h"


/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "mmc_debug_flags.h"

/*
 * This flag should really be static. It gets set when serial_debug_init
 * is called, and is used to make repeated calls into no-ops.
 */
boolean mmc_debug_inited = FALSE;

/*
 * mmc_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the atm MMC debug flags at once. The argument
 * is TRUE for "debug all", FALSE for "undebug all".
 */

void mmc_debug_all (boolean flag)
{
    int i;

    for (i = 0; mmc_debug_arr[i].var != (boolean *) NULL; i++) {
        *(mmc_debug_arr[i].var) = flag;
    }
}

/*
 * mmc_debug_show is called to display the values of all the mmc var 
 * 
 */

void mmc_debug_show (void)
{
    debug_show_flags(&(mmc_debug_arr[0]), "Generic ATM MMC");
}


/*
 * mmc_debug_init is called at ATM MMC startup. It registers
 * the routines to maintain and display the ATM debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when ATM MMC is started.
 */

void mmc_debug_init (void)
{
    /* Repeated calls to this routine do nothing */
    if (mmc_debug_inited) return;

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    (void) reg_add_debug_all(mmc_debug_all, "mmc_debug_all");
    (void) reg_add_debug_show(mmc_debug_show, "mmc_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when ATM MMC is initialized, we want to turn on
     * all the ATM MMC debugging right now.
     */
    mmc_debug_all(debug_all_p());
}
