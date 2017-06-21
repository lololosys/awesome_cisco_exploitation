/* $Id: modem_debug.c,v 3.1.64.3 1996/07/25 09:50:54 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_debug.c,v $
 *------------------------------------------------------------------
 * modem_debug.c 
 *
 * November 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_debug.c,v $
 * Revision 3.1.64.3  1996/07/25  09:50:54  snyder
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
 * Revision 3.1.64.2  1996/07/11  01:11:35  styang
 * CSCdi62212:  code review clean up
 * Branch: California_branch
 * Code review clean up.
 *
 * Revision 3.1.64.1  1996/06/16  21:16:15  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:29  vnguyen
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "logger.h"
#include "../ui/debug.h"
#include "modem_debug.h"
#include "parser_defs_modem.h"
#include "../dev/flash_defines.h"
#include "if_as5200_tdm.h"
#include "if_as5200_modem.h"
#include "modem_mgmt.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "modem_debug_flags.h"

/*
 * modem_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the Modem Mgmt debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

void modem_debug_all (boolean flag)
{
    int slot, port;
    int modem_in_slot;
    modem_info_t *modem_info;

    generic_debug_all(modem_debug_arr, flag);

    for (slot = 0; slot < modem_pool->max_slots; slot++) {
       if (modem_pool && modem_pool->modem_slot[slot]) {
           modem_in_slot = modem_pool->modem_slot[slot]->total_modem_num;
           for (port = 0; port < modem_in_slot; port++) {
               modem_info = CSM_MODEM_INFO(slot,port);
               if (modem_info) {
                   if (flag) {
                       if (!(modem_info->modem_config & CFG_MODEM_DEBUG_CSM)) {
                           modem_mgmt_csm_debug++;
                           modem_info->modem_config |= CFG_MODEM_DEBUG_CSM;
                       }
                       if (modem_info->modem_mgmt_info)
                           modem_info->modem_config |= 
                               CFG_MODEM_DEBUG_OOB | CFG_MODEM_DEBUG_TRACE;
                   } else {
                       if (modem_info->modem_config & CFG_MODEM_DEBUG_CSM) {
                           modem_mgmt_csm_debug--;
                           modem_info->modem_config &= ~CFG_MODEM_DEBUG_CSM;
                       }
                       if (modem_info->modem_mgmt_info)
                           modem_info->modem_config &= 
                               ~(CFG_MODEM_DEBUG_OOB | CFG_MODEM_DEBUG_TRACE);
                   }
               }
           }
       }
   }
}

/*
 * modem_debug_show is called to display the values of all the Modem Mgmt
 * debugging variables.
 */

void modem_debug_show (void)
{
    debug_show_flags(&(modem_debug_arr[0]), "Modem Management");
}

/*
 * modem_debug_init is called at Modem Mgmt subsystem startup. It
 * registers the routines to maintain and display the Modem Mgmt debug flags,
 * and makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when Modem Mgmt is started.
 */

void modem_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(modem_debug_all, "modem_debug_all");
    reg_add_debug_show(modem_debug_show, "modem_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when Modem Mgmt is initialized, we want to turn on
     * all the Modem Mgmt debugging right now.
     */
    modem_debug_all(debug_all_p());
}
