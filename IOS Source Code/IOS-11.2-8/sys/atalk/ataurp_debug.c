/* $Id: ataurp_debug.c,v 3.1.8.3 1996/07/25 09:40:59 snyder Exp $
 * $Source: /release/112/cvs/Xsys/atalk/ataurp_debug.c,v $
 *------------------------------------------------------------------
 * Appletalk AURP-service debugging support
 *
 * March 1996, Dean Wong
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ataurp_debug.c,v $
 * Revision 3.1.8.3  1996/07/25  09:40:59  snyder
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
 * Revision 3.1.8.2  1996/07/10  22:19:16  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.1.8.1  1996/05/02  08:41:00  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.1  1996/03/14  00:34:10  dwong
 * Placeholder for California.
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
#include "ataurp_debug.h"

/* Global storage */
atalkdebugtype *atalkaurp_update_debug;
atalkdebugtype *atalkaurp_redist_debug;
atalkdebugtype *atalkaurp_packet_debug;
atalkdebugtype *atalkaurp_connection_debug;

/* Local "static" storage */
static atalkdebugtype *ataurp_debug_flags;


void
ataurp_debug_command (parseinfo *csb)
{
    switch (csb->which) {
    case DEBUG_AURP_UPDATE:
	atalk_debug_setflag(csb, atalkaurp_update_debug);
	atalk_debug_show_flag(atalkaurp_update_debug, TRUE, FALSE);
	break;
    case DEBUG_AURP_PACKET:
	atalk_debug_setflag(csb, atalkaurp_packet_debug);
	atalk_debug_show_flag(atalkaurp_packet_debug, TRUE, FALSE);
	break;
    case DEBUG_AURP_CONNECTION:
	atalk_debug_setflag(csb, atalkaurp_connection_debug);
	atalk_debug_show_flag(atalkaurp_connection_debug, TRUE, FALSE);
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	return;

    }
}

/*
 * ataurp_debug_all
 * Registered to be called whenever "debug all" or "undebug all" is issued,
 * or whenever it's desirable to set all the AppleTalk debug flags at once.
 */
static void
ataurp_debug_all (boolean flag)
{
    int num;
    if (flag)
	num = -1;
    else
	num = 0;

    atalkaurp_update_debug->flag = num;
    atalkaurp_packet_debug->flag = num;
    atalkaurp_connection_debug->flag = num;
}

static atalkdebugtype **const ataurp_flagstoshow[] = {
    &atalkaurp_update_debug,
    &atalkaurp_packet_debug,
    &atalkaurp_connection_debug
};
static int const ataurp_nflagstoshow =
    sizeof(ataurp_flagstoshow) / sizeof(atalkdebugtype **);


/*
 * ataurp_debug_show
 * Implements the "show debug" command for ATAURP flags.
 */
static void
ataurp_debug_show (void)
{
    int i;
    boolean hdisplayed = FALSE;

    for (i = 0; i < ataurp_nflagstoshow; i++) {
	if (!hdisplayed &&
	    ((debug_all_p() && ((*ataurp_flagstoshow[i])->flag != -1)) ||
	     (!debug_all_p() && ((*ataurp_flagstoshow[i])->flag != 0)))) {
	    printf("\nATAURP:");
	    hdisplayed = TRUE;
	}
	atalk_debug_show_flag(*ataurp_flagstoshow[i], FALSE, TRUE);
    }
}

/*
 * ataurp_debug_init
 * Malloc and initialize all ATAURP debugging flags, do general
 * ATAURP debugging setup.
 */
void
ataurp_debug_init (void)
{
    /* Make multiple calls harmless */
    if (ataurp_debug_flags != NULL)
	return;

    /* Allocate space for all the debugging flags and attach descriptions */
    ataurp_debug_flags = malloc(sizeof(atalkdebugtype) * DEBUG_ATAURP_MAX);

    atalkaurp_update_debug = &ataurp_debug_flags[DEBUG_AURP_UPDATE];
    atalkaurp_update_debug->text = "AURP routing updates";

    atalkaurp_redist_debug = &ataurp_debug_flags[DEBUG_AURP_REDIST];
    atalkaurp_redist_debug->text = "AURP route redistribution";

    atalkaurp_packet_debug = &ataurp_debug_flags[DEBUG_AURP_PACKET];
    atalkaurp_packet_debug->text = "AURP packets";

    atalkaurp_connection_debug = &ataurp_debug_flags[DEBUG_AURP_CONNECTION];
    atalkaurp_connection_debug->text = "AURP connection";

    /* Register for "debug/undebug all" and "show debug" commands */
    reg_add_debug_all(ataurp_debug_all, "ataurp_debug_all");
    reg_add_debug_show(ataurp_debug_show, "ataurp_debug_show");

    /* Initialize the flags to reflect the currents setting of "debug all" */
    ataurp_debug_all(debug_all_p());
}

