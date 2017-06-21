/* $Id: sscop_debug.c,v 3.2.60.1 1996/07/25 09:41:27 snyder Exp $
 * $Source: /release/112/cvs/Xsys/atm/sscop_debug.c,v $
 *------------------------------------------------------------------
 * atm/sscop_debug.c: DEBUG command code
 *
 * November, 1993,  Percy P. Khabardar 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: sscop_debug.c,v $
 * Revision 3.2.60.1  1996/07/25  09:41:27  snyder
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
 * Revision 3.2  1995/11/17  08:47:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:12:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "sscop_debug.h"


/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "sscop_debug_flags.h"

/*
 * s s c o p _ d e b u g _ a l l ( )
 *
 * sscop_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the sscop debug flags at once.
 * The argument is TRUE for "debug all", FALSE for "undebug all".
 */
static void sscop_debug_all (boolean flag)
{
    generic_debug_all(sscop_debug_arr, flag);
}


/*
 * s s c o p _ d e b u g _ s h o w ( )
 *
 * sscop_debug_show is called to display the values of all the SSCOP 
 * debugging variables.
 */
static void sscop_debug_show (void)
{
    debug_show_flags( &(sscop_debug_arr[0]), "Generic SSCOP" );
}


/*
 * s s c o p _ d e b u g _ i n i t ( )
 *
 * sscop_debug_init is called at SSCOP startup.  It registers the
 * routines to maintain and display the SSCOP debug flags, and makes 
 * sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when SSCOP is started.
 */
void sscop_debug_init (void)
{

    /* register for "debug all" and "show debug" events.  A real programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list.  Oh, well...
     */
    (void)reg_add_debug_all(sscop_debug_all, "sscop_debug_all");
    (void)reg_add_debug_show(sscop_debug_show, "sscop_debug_show");

    /* make sure the debug flags are right at startup.  If "debug all"
     * is in effect when SSCOP is initialized, we want to turn on all
     * the SSCOP debugging right now.
     */
    sscop_debug_all(debug_all_p());
}
