/* $Id: ara_debug.c,v 3.2.58.1 1996/07/25 09:40:23 snyder Exp $
 * $Source: /release/112/cvs/Xsys/arap/ara_debug.c,v $
 *------------------------------------------------------------------
 * ara_debug.c -- Debugging routines for ARA
 *
 * September 1993, Tim Kolar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ara_debug.c,v $
 * Revision 3.2.58.1  1996/07/25  09:40:23  snyder
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
 * Revision 3.2  1995/11/17  08:38:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:04:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "../ui/debug.h"
#include "ara_debug.h"
#include "arap_public.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "ara_debug_flags.h"

static const char debugmsg_fmt[]= " for line %d";

int arainttty_debug;
int aramemtty_debug;
int aramnp4tty_debug;
int arav42tty_debug;

void ara_debug_all (boolean flag)
{
    /* If debug all is enabled, interface specific debugging is disabled */
    if (flag) {
	arainttty_debug = NOTTY_DEBUG;
	aramemtty_debug = NOTTY_DEBUG;
	aramnp4tty_debug = NOTTY_DEBUG;
	arav42tty_debug = NOTTY_DEBUG;
    }

    generic_debug_all(ara_debug_arr, flag);
}

void ara_debug_show (void)
{
    uint i;
    boolean hdisplayed = FALSE;

    /* Show ARAP debugging enabled. Print line numbers if line specific
     * debugging is enabled
     */
    for (i = 0; ara_debug_arr[i].var != (boolean *) NULL; i++) {
	if (*ara_debug_arr[i].var) {
	    if (!hdisplayed) {
		printf("\nAppleTalk Remote Access:");
		hdisplayed = TRUE;
	    }

	    printf("\n  %s debugging is %s", ara_debug_arr[i].txt,
		    (*ara_debug_arr[i].var) ? "on" : "off");

	    if (debug_all_p())
		continue;

	    switch (i) {
	      case DEBUG_ARAP_INTERNAL:
		if (arainttty_debug != NOTTY_DEBUG) 
		    printf(debugmsg_fmt, arainttty_debug);
		break;
	      case DEBUG_ARAP_MEMORY:
		if (aramemtty_debug != NOTTY_DEBUG) 
		    printf(debugmsg_fmt, aramemtty_debug);
		break;
	      case DEBUG_ARAP_MNP4:
		if (aramnp4tty_debug != NOTTY_DEBUG) 
		    printf(debugmsg_fmt, aramnp4tty_debug);
		break;
	      case DEBUG_ARAP_V42BIS:
		if (arav42tty_debug != NOTTY_DEBUG) 
		    printf(debugmsg_fmt, arav42tty_debug);
		break;
	      default:
		break;
	    }
	}
    }
}

void ara_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    (void) reg_add_debug_all(ara_debug_all, "ara_debug_all");
    (void) reg_add_debug_show(ara_debug_show, "ara_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when ARA is initialized, we want to turn on
     * all the ARA debugging right now.
     */
    ara_debug_all(debug_all_p());
}

void ara_debug_command (parseinfo *csb )
{
    int num = GETOBJ(int,1);	/* num will be NOTTY_DEBUG if line specific
				 * debugging is not enabled
				 */

    switch (csb->which) {
      case DEBUG_ARAP_INTERNAL:
	arainttty_debug = num;
	break;
      case DEBUG_ARAP_MEMORY:
	aramemtty_debug = num;
	break;
      case DEBUG_ARAP_MNP4:
	aramnp4tty_debug = num; 
	break;
      case DEBUG_ARAP_V42BIS:
	arav42tty_debug = num; 
	break;
      default:
	break;
    }

    /* Enable debugging */
    debug_command(csb);

    if (num != NOTTY_DEBUG) 
	printf(debugmsg_fmt, num);
}
