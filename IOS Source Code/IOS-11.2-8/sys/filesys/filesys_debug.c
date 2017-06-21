/* $Id: filesys_debug.c,v 3.2.64.2 1996/07/25 09:43:56 snyder Exp $
 * $Source: /release/112/cvs/Xsys/filesys/filesys_debug.c,v $
 *------------------------------------------------------------------
 * Debugging routines for file system.
 *
 * Oct 1995, Yan Ke
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: filesys_debug.c,v $
 * Revision 3.2.64.2  1996/07/25  09:43:56  snyder
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
 * Revision 3.2.64.1  1996/03/18  19:35:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.28.2  1996/03/07  09:35:27  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.28.1  1996/02/20  13:49:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:07:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/17  17:46:17  yanke
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "../filesys/filesys_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "filesys_debug_flags.h"

static void filesys_debug_all (boolean debug_flag)
{
    generic_debug_all(filesys_debug_arr, debug_flag);
}

/*
 * Called when "[no] debug filesys all" is typed.
 */
void filesys_set_debug_all (parseinfo *csb)
{
    if (csb->sense)
	filesys_debug_all(TRUE);
    else
	filesys_debug_all(FALSE);
}

static void filesys_debug_show (void)
{
    debug_show_flags(&(filesys_debug_arr[0]), "File system");
}

void filesys_debug_init (void)
{

    /*
     * Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(filesys_debug_all, "filesys_debug_all");
    reg_add_debug_show(filesys_debug_show, "filesys_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when SRT is initialized, we want to turn on
     * all the SRT debugging right now.
     */
    filesys_debug_all(debug_all_p());
}
