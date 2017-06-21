/* $Id: at_domain_debug.c,v 3.3.18.3 1996/07/25 09:40:58 snyder Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_domain_debug.c,v $
 *------------------------------------------------------------------
 * Support for debugging of AppleTalk Remapping
 *
 * September 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_domain_debug.c,v $
 * Revision 3.3.18.3  1996/07/25  09:40:58  snyder
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
 * Revision 3.3.18.2  1996/07/10  22:17:56  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.18.1  1996/03/18  18:51:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:28:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:16:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/07  16:09:40  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.2  1995/11/17  08:42:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/28  00:25:29  slin
 * Placeholders for AppleTalk Code Modularization
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
#include "at_domain_debug.h"



/* Global storage */
atalkdebugtype *atalkdomain_debug;
atalkdebugtype *atalkremap_debug;

static atalkdebugtype *atdomain_debug_flags;

static char *const atdomain_debug_text[DEBUG_ATALK_DOMAIN_MAX] =
{
    "AppleTalk Domain",
    "AppleTalk network number Remapping"
};

static atalkdebugtype **const atdomain_flagstoshow[] =
{
    &atalkdomain_debug,
    &atalkremap_debug
};

static ulong const atdomain_nflagstoshow =
    sizeof(atdomain_flagstoshow) / sizeof(atalkdebugtype **);


void
atdomain_debug_command (parseinfo *csb)
{
    if (csb == NULL)
	return;

    switch (csb->which) {
    case DEBUG_ATALK_DOMAIN :
	atalk_debug_setflag(csb, atalkdomain_debug);
	atalk_debug_show_flag(atalkdomain_debug, TRUE, FALSE);
	  break;

    case DEBUG_ATALK_REMAP :
	atalk_debug_setflag(csb, atalkremap_debug);
	atalk_debug_show_flag(atalkremap_debug, TRUE, FALSE);
	  break;

    default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }
}

/*
 * atdomain_debug_all
 * Registered to be called whenever "debug all" or "undebug all" is issued,
 * or whenever it's desirable to set all the AT/Domain debug flags at once.
 *
 */
static void
atdomain_debug_all (boolean flag)
{
    int num;
    if (flag)
	num = -1;
    else
	num = 0;

    atalkdomain_debug->flag = num;
    atalkremap_debug->flag = num;
}

/*
 * atdomain_debug_show
 * Implements the "show debug" command for AT/EIGRP flags.
 */
static void
atdomain_debug_show (void)
{
    int i;
    boolean hdisplayed = FALSE;

    for (i = 0; i < atdomain_nflagstoshow; i++) {
	if (!hdisplayed &&
	    ((debug_all_p() && ((*atdomain_flagstoshow[i])->flag != -1)) ||
	     (!debug_all_p() && ((*atdomain_flagstoshow[i])->flag != 0)))) {
	    printf("\nAT/Domain:");
	    hdisplayed = TRUE;
	}
	atalk_debug_show_flag(*atdomain_flagstoshow[i], FALSE, TRUE);
    }
}

/*
 * atigrp2_debug_init
 * Malloc and initialize all AT/EIGRP debugging flags, do general
 * AT/EIGRP debugging setup.
 */
void
atdomain_debug_init (void)
{
    /* Make multiple calls harmless */
    if (atdomain_debug_flags != NULL)
	return;

    /* Allocate space for all the debugging flags and attach descriptions */
    atdomain_debug_flags =
	malloc(sizeof(atalkdebugtype) * DEBUG_ATALK_DOMAIN_MAX);
    if (atdomain_debug_flags == NULL)
	return;

    atalkdomain_debug = &atdomain_debug_flags[DEBUG_ATALK_DOMAIN];
    atalkremap_debug  = &atdomain_debug_flags[DEBUG_ATALK_REMAP];

    atalkdomain_debug->text = atdomain_debug_text[DEBUG_ATALK_DOMAIN];
    atalkremap_debug->text  = atdomain_debug_text[DEBUG_ATALK_REMAP];
    /* Register for "debug/undebug all" and "show debug" commands */
    reg_add_debug_all(atdomain_debug_all, "atdomain_debug_all");
    reg_add_debug_show(atdomain_debug_show, "atdomain_debug_show");

    /* Initialize the flags to reflect the currents setting of "debug all" */
    atdomain_debug_all(debug_all_p());
}

