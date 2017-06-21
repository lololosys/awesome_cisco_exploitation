/* $Id: crypto_debug.c,v 3.1.60.2 1996/07/25 09:42:42 snyder Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_debug.c,v $
 *------------------------------------------------------------------
 * crypto_debug.c - Debugging routines for the Crypto subsystem
 *
 * June 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_debug.c,v $
 * Revision 3.1.60.2  1996/07/25  09:42:42  snyder
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
 * Revision 3.1.60.1  1996/04/19  14:55:34  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.2.3  1995/11/28  01:46:23  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.2  1995/11/17  16:33:15  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.2.1  1995/07/01  05:31:19  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 1.1.2.1  1995/06/07  00:44:35  bew
 * Branch: IosSec_branch
 * Addition of crypto debug routines.
 *
 * Revision 3.1  1995/11/09  11:13:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/29  22:59:51  carrel
 * Placeholders for IosSecurity.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../crypto/crypto_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "crypto_debug_flags.h"

/*
 * crypto_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the OS debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

void crypto_debug_all (boolean flag)
{
    generic_debug_all(crypto_debug_arr, flag);
}

/*
 * crypto_debug_show is called to display the values of all the OS
 * debugging variables.
 */

void crypto_debug_show (void)
{
    debug_show_flags(&(crypto_debug_arr[0]), "Cryptographic Subsystem");
}

/*
 * crypto_debug_init is called at crypto subsystem startup. It registers
 * the routines to maintain and display the crypto debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when crypto is started.
 */

void crypto_debug_init (void)
{
    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned 
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(crypto_debug_all, "crypto_debug_all");
    reg_add_debug_show(crypto_debug_show, "crypto_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when OS is initialized, we want to turn on
     * all the OS debugging right now.
     */
    crypto_debug_all(debug_all_p());
}
