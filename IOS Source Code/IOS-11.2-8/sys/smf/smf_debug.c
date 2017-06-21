/* $Id: smf_debug.c,v 3.2.62.2 1996/07/25 09:53:59 snyder Exp $
 * $Source: /release/112/cvs/Xsys/smf/smf_debug.c,v $
 *------------------------------------------------------------------
 * Software MAC Address Filter (SMF) debug functions.
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The SMF is intended for use when an interface is listening
 * promiscuously to match and dispose of received packets based
 * upon their destination MAC addresses in an efficient way.
 * In particular, it provides a facility that may be used on a
 * per-interface basis in support of Concurrent Routing and
 * Bridging (CRB) and eventually Integrated Routing and Bridging (IRB).
 *
 *------------------------------------------------------------------
 * $Log: smf_debug.c,v $
 * Revision 3.2.62.2  1996/07/25  09:53:59  snyder
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
 * Revision 3.2.62.1  1996/03/18  21:47:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:42:25  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:10:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:52:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:44:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"

#include "../ui/debug.h"
#include "smf_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "smf_debug_flags.h"


/*
 * smf_debug_command
 * Handles debug commands related to SMF
 */

void smf_debug_command (parseinfo *csb)
{
    /* If there's no special case, use the generic handler */
    debug_command(csb);
}

/*
 * smf_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the SMF debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void smf_debug_all (boolean flag)
{
    generic_debug_all(smf_debug_arr, flag);
}

/*
 * smf_debug_show is called to display the values of all the SMF
 * debugging variables.
 */

static void smf_debug_show (void)
{
    debug_show_flags(&(smf_debug_arr[0]), "Software MAC filter");
}
	

/*
 * smf_debug_init is called at SMF subsystem startup. It registers
 * the routines to maintain and display the SMF debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when SMF is started.
 */

void smf_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(smf_debug_all, "smf_debug_all");
    reg_add_debug_show(smf_debug_show, "smf_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when SMF is initialized, we want to turn on
     * all the SMF debugging right now.
     */
    smf_debug_all(debug_all_p());
}
