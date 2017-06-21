/* $Id: atip_debug.c,v 3.3.18.3 1996/07/10 22:19:21 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/atip_debug.c,v $
 *------------------------------------------------------------------
 * Appletalk IP-services debugging support
 *
 * March 1994, Steven Lin
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atip_debug.c,v $
 * Revision 3.3.18.3  1996/07/10  22:19:21  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.18.2  1996/05/02  08:41:07  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.3.18.1  1996/03/18  18:53:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:29:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:18:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/07  16:09:58  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.2  1995/11/17  08:44:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:50:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "parser.h"
#include "atalk_private.h"
#include "atip_debug.h"

/* Global storage */
atalkdebugtype *atalkiptalk_debug;
atalkdebugtype *atalkmacip_debug;

/* Local "static" storage */
static atalkdebugtype *atip_debug_flags;


void
atip_debug_command (parseinfo *csb)
{
    switch (csb->which) {
    case DEBUG_ATALK_IPTALK:
	atalk_debug_setflag(csb, atalkiptalk_debug);
	atalk_debug_show_flag(atalkiptalk_debug, TRUE, FALSE);
	break;
    case DEBUG_ATALK_MACIP:
	atalk_debug_setflag(csb, atalkmacip_debug);
	atalk_debug_show_flag(atalkmacip_debug, TRUE, FALSE);
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	return;

    }
}

/*
 * atip_debug_all
 * Registered to be called whenever "debug all" or "undebug all" is issued,
 * or whenever it's desirable to set all the AppleTalk debug flags at once.
 */
static void
atip_debug_all (boolean flag)
{
    int num;
    if (flag)
	num = -1;
    else
	num = 0;

    atalkiptalk_debug->flag = num;
    atalkmacip_debug->flag = num;
}

static atalkdebugtype **atip_flagstoshow[] = {
    &atalkiptalk_debug,
    &atalkmacip_debug
};
static int const atip_nflagstoshow =
    sizeof(atip_flagstoshow) / sizeof(atalkdebugtype **);


/*
 * atip_debug_show
 * Implements the "show debug" command for ATIP flags.
 */
static void
atip_debug_show (void)
{
    int i;
    boolean hdisplayed = FALSE;

    for (i = 0; i < atip_nflagstoshow; i++) {
	if (!hdisplayed &&
	    ((debug_all_p() && ((*atip_flagstoshow[i])->flag != -1)) ||
	     (!debug_all_p() && ((*atip_flagstoshow[i])->flag != 0)))) {
	    printf("\nATIP:");
	    hdisplayed = TRUE;
	}
	atalk_debug_show_flag(*atip_flagstoshow[i], FALSE, TRUE);
    }
}

/*
 * atip_debug_init
 * Malloc and initialize all ATIP debugging flags, do general
 * ATIP debugging setup.
 */
void
atip_debug_init (void)
{
    /* Make multiple calls harmless */
    if (atip_debug_flags != NULL)
	return;

    /* Allocate space for all the debugging flags and attach descriptions */
    atip_debug_flags = malloc(sizeof(atalkdebugtype) * DEBUG_ATIP_MAX);

    atalkiptalk_debug = &atip_debug_flags[DEBUG_ATALK_IPTALK];
    atalkiptalk_debug->text = "AppleTalk over IP (IPtalk)";

    atalkmacip_debug = &atip_debug_flags[DEBUG_ATALK_MACIP];
    atalkmacip_debug->text = "AppleTalk MacIP server-client info";

    /* Register for "debug/undebug all" and "show debug" commands */
    reg_add_debug_all(atip_debug_all, "atip_debug_all");
    reg_add_debug_show(atip_debug_show, "atip_debug_show");

    /* Initialize the flags to reflect the currents setting of "debug all" */
    atip_debug_all(debug_all_p());
}
