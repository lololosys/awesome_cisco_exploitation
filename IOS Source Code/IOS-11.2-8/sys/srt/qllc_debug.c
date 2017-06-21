/* $Id: qllc_debug.c,v 3.2.62.3 1996/07/25 09:54:56 snyder Exp $
 * $Source: /release/112/cvs/Xsys/srt/qllc_debug.c,v $
 *------------------------------------------------------------------
 * Modularized QLLC debugging flags
 *
 * April, 1994          Phil Morton
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * As described in Notes.txt
 *------------------------------------------------------------------
 * $Log: qllc_debug.c,v $
 * Revision 3.2.62.3  1996/07/25  09:54:56  snyder
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
 * Revision 3.2.62.2  1996/05/17  12:13:18  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  07:58:23  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.1  1996/04/03  21:59:01  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.62.1  1996/03/18  22:10:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:50:49  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  18:47:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:55:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:01:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "interface_generic.h"
#include "logger.h"
#include "../ui/debug.h"
#include "qllc_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "qllc_debug_flags.h"

/* qllc_debug_command */
void qllc_debug_command (parseinfo *csb)
{
    switch  (csb->which) {

       /* we may add specific processing later */

    default:
       /* if there's no special case use the generic handler */
       debug_command(csb);
       break;
    } 
} 

/* qllc_debug_all is registered to be called whenever anybody issues     */
/* a "debug all" or "undebug all" command... or whenver you want to      */
/* set the state of all of the qllc debug flags at once. The argument is */
/*  true for "debug all", false for "undebug all".                       */

static void qllc_debug_all (boolean flag)
{
     generic_debug_all(qllc_debug_arr, flag);
}

/* qllc_debug_show is called to display the values of of all of the qllc */
/* debugging variables                                                   */

static void qllc_debug_show(void)
{
 /* show all of the flags that follow the standard scheme                */
 debug_show_flags(&(qllc_debug_arr[0]), "qllc debug");
}

/* qllc_debug_init should be called at qllc system startup               */
/* it registers the routines to maintain and display the qllc debug      */
/* flags, and makes sure the flags are set to the appropriate values     */
/* depending on whether "debug all" is in effect when qllc is started.   */

void qllc_debug_init(void)
{

     /* register for "debug all" and "show debug" events */

     reg_add_debug_all(qllc_debug_all, "qllc_debug_all");
     reg_add_debug_show(qllc_debug_show, "qllc_debug_show");

     /* make sure the debug flags are right at startup. If "debug all"   */
     /* is in effect when qllc is initialized, we want to turn on all    */
     /* the qllc debugging right now.                                    */ 

     qllc_debug_all(debug_all_p());
}

