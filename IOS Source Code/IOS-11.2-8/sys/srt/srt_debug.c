/* $Id: srt_debug.c,v 3.2.62.2 1996/07/25 09:54:57 snyder Exp $
 * $Source: /release/112/cvs/Xsys/srt/srt_debug.c,v $
 *------------------------------------------------------------------
 * srt_debug.c -- Debugging routines for General bridging
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srt_debug.c,v $
 * Revision 3.2.62.2  1996/07/25  09:54:57  snyder
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
 * Revision 3.2.62.1  1996/03/18  22:11:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:51:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  18:50:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:57:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:02:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "interface_private.h"
#include "../ibm/netbios.h"
#include "access_expr.h"
#include "../ui/debug.h"
#include "srt_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "srt_debug_flags.h"


/*
 * srt_debug_command
 * Handles debug commands related to SRT
 */

void srt_debug_command (parseinfo *csb)
{
    if (csb->which == DEBUG_ACCESS_EXPR) {
	if (!csb->sense) {
	    access_expr_debug = 0;
	} else {
	    if (GETOBJ(int,1) == -1)
		access_expr_debug = 0;
	    else
		access_expr_debug = GETOBJ(int,1);
	    
	    printf("\n  Boolean access expression debugging is %s",
		   access_expr_debug ? "on" : "off");
	    if (access_expr_debug != 0) {
		printf(": ");
		if (access_expr_debug & EXPR_DBG_EXPRESSION_TREE)
		    printf("expression-tree ");
		if (access_expr_debug & EXPR_DBG_OPERATOR_STACK)
		    printf("operator-stack ");
		if (access_expr_debug & EXPR_DBG_RETURN_CODE)
		    printf("return-code ");
		if (access_expr_debug & EXPR_DBG_SYSTEM_CALLS)
		    printf("system-calls ");
	    }
	}
    } else {
	/* If there's no special case, use the generic handler */
	debug_command(csb);
    }
}

/*
 * srt_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the SRT debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void srt_debug_all (boolean flag)
{
    /* access_expr_debug contains bitmapped options */
    if (flag) {
	access_expr_debug = ~0;
    } else {
	access_expr_debug = 0;
    }
    generic_debug_all(srt_debug_arr, flag);
}

/*
 * srt_debug_show is called to display the values of all the SRT
 * debugging variables.
 */

static void srt_debug_show (void)
{
    debug_show_flags(&(srt_debug_arr[0]), "General bridging");
    if (!access_expr_debug != !debug_all_p()) {
	printf("\n  Boolean access expression debugging is %s",
	       access_expr_debug ? "on" : "off");
	if (access_expr_debug != 0) {
	    printf(": ");
	    if (access_expr_debug & EXPR_DBG_EXPRESSION_TREE)
		printf("expression-tree ");
	    if (access_expr_debug & EXPR_DBG_OPERATOR_STACK)
		printf("operator-stack ");
	    if (access_expr_debug & EXPR_DBG_RETURN_CODE)
		printf("return-code ");
	    if (access_expr_debug & EXPR_DBG_SYSTEM_CALLS)
		printf("system-calls ");
	}
    }
}
	

/*
 * srt_debug_init is called at SRT subsystem startup. It registers
 * the routines to maintain and display the SRT debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when SRT is started.
 */

void srt_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(srt_debug_all, "srt_debug_all");
    reg_add_debug_show(srt_debug_show, "srt_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when SRT is initialized, we want to turn on
     * all the SRT debugging right now.
     */
    srt_debug_all(debug_all_p());
}
