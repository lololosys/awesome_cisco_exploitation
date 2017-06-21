/* $Id: isdn_debug.c,v 3.3.4.3 1996/07/25 09:57:21 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/isdn_debug.c,v $
 *------------------------------------------------------------------
 * isdn_debug.c -- Debugging routines for ISDN
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isdn_debug.c,v $
 * Revision 3.3.4.3  1996/07/25  09:57:21  snyder
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
 * Revision 3.3.4.2  1996/05/21  10:09:39  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.4.1  1996/03/18  22:47:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  07:55:48  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/13  02:12:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  21:51:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/03/09  22:58:28  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  18:03:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:18:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "../wan/isdn_debug.h"
#include "../wan/isdn.h"
#include "../les/isdn_bri.h"
#include "../isdn/tune.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "isdn_debug_flags.h"

/* DSL number for selective debugging */

int isdn_debug_dsl;

/*
 * isdn_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the ISDN debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void isdn_debug_all (boolean flag)
{
    generic_debug_all(isdn_debug_arr, flag);
    isdn_debug_dsl = ISDN_DEBUG_NO_DSL;
}

/*
 * isdn_debug_show is called to display the values of all the ISDN
 * debugging variables.
 */

static void isdn_debug_show (void)
{
    debug_show_flags(&(isdn_debug_arr[0]), "ISDN");
}

/*
 * isdn_debug_standard is called to turn on/off standard debugging
 * variables
 */
static void isdn_debug_standard (boolean sense)
{
    isdn_debug = sense;
}

/*
 * isdn_debug_init is called at ISDN subsystem startup. It registers
 * the routines to maintain and display the ISDN debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when ISDN is started.
 */

void isdn_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(isdn_debug_all, "isdn_debug_all");
    reg_add_debug_show(isdn_debug_show, "isdn_debug_show");
    reg_add_debug_standard(isdn_debug_standard, "isdn_debug_standard");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when ISDN is initialized, we want to turn on
     * all the ISDN debugging right now.
     */
    isdn_debug_all(debug_all_p());
}

/*
 * Command handler for "debug isdn <foo>"
 */
void isdn_debug_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    int dsl;

    /*
     * If there's an common_str_interface parameter present for Q921/Q931,
     * save the DSL number.
     */
    switch (csb->which) {
    case DEBUG_ISDN_Q921:
    case DEBUG_ISDN_Q931:
	isdn_debug_dsl = ISDN_DEBUG_NO_DSL;
	hwidb = hwidb_or_null(GETOBJ(idb,1));
	if (hwidb) {

	    /* Sigh.  Search for the hwidb to figure out the DSL. */

	    for (dsl = 0; dsl < MAX_DSL; dsl++) {
		if (isdn_idblist[dsl] == hwidb) {
		    isdn_debug_dsl = dsl;
		    break;
		}
	    }
	}
    }
    
    /* All else is generic. */

    debug_command(csb);
}
