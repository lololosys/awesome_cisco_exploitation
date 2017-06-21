/* $Id: http_debug.c,v 3.2.4.1 1996/07/25 09:55:26 snyder Exp $
 * $Source: /release/112/cvs/Xsys/tcp/http_debug.c,v $
 *------------------------------------------------------------------
 * http_debug.c -- Debugging for HTTP
 *
 * July 1995 Tony Li
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: http_debug.c,v $
 * Revision 3.2.4.1  1996/07/25  09:55:26  snyder
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
 * Revision 3.2  1996/03/09  22:57:39  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:25:54  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "http_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "http_debug_flags.h"

/*
 * http_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the HTTP debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void http_debug_all (boolean flag)
{
    generic_debug_all(http_debug_arr, flag);
}

/*
 * http_debug_show is called to display the values of all the HTTP
 * debugging variables.
 */

static void http_debug_show (void)
{
    debug_show_flags(&(http_debug_arr[0]), "HTTP");
}

/*
 * http_debug_init is called at HTTP subsystem startup. It registers
 * the routines to maintain and display the HTTP debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when HTTP is started.
 */

void http_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(http_debug_all, "http_debug_all");
    reg_add_debug_show(http_debug_show, "http_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when HTTP is initialized, we want to turn on
     * all the HTTP debugging right now.
     */
    http_debug_all(debug_all_p());
}
