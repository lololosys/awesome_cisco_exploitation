/* $Id: at_igrp2_debug.c,v 3.3.18.4 1996/07/25 09:40:58 snyder Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_igrp2_debug.c,v $
 *------------------------------------------------------------------
 * Debugging support for AppleTalk EIGRP
 *
 * September 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_igrp2_debug.c,v $
 * Revision 3.3.18.4  1996/07/25  09:40:58  snyder
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
 * Revision 3.3.18.3  1996/07/10  22:18:16  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.18.2  1996/03/23  01:26:45  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3.18.1  1996/03/18  18:52:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:28:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:17:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/07  16:09:48  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.2  1995/11/17  08:42:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:50:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/28  00:25:33  slin
 * Placeholders for AppleTalk Code Modularization
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include <interface_private.h>
#include "packet.h"
#include "../ui/debug.h"
#include "parser.h"
#include "atalk_private.h"
#include "at_igrp2.h"
#include "at_igrp2_debug.h"
#include "at_debug.h"

/* Global storage */
atalkdebugtype *atalkigrp_events;
atalkdebugtype *atalkigrp_update_debug;
atalkdebugtype *atalkigrp_query_debug;
atalkdebugtype *atalkigrp_request_debug;
static atalkdebugtype *atalkigrp_hello_debug;
atalkdebugtype *atalkigrp_external_debug;
atalkdebugtype *atalkigrp_redist_debug;
atalkdebugtype *atalkigrp_packet_debug;
static atalkdebugtype *atalkigrp_target_debug;
static atalkdebugtype *atalkigrp_peer_debug;

static void atigrp2_debug_all(boolean);

/*
 * The following pointer is used to point at one piece of malloc'ed
 * memory which is carved up into atalkdebugtype blocks; ie, don't
 * assume that this points to one debug flag.
 */
static atalkdebugtype *atigrp2_debug_flags;
static char *const atigrp2_debug_text[DEBUG_ATALKIGRP_MAX] =
{
    "AT/EIGRP Event Logging",
    "AT/EIGRP External metric",
    "AT/EIGRP Request packet",
    "AT/EIGRP Hello packet",
    "AT/EIGRP Query packet",
    "AT/EIGRP Update packet",
    "AT/EIGRP Route redistribution",
    "AT/EIGRP general packet",
    "AT/EIGRP DUAL target",
    "AT/EIGRP all debugging"
};


static atalkdebugtype **const atigrp2_flagstoshow[] =
{
    &atalkigrp_events,
    &atalkigrp_external_debug,
    &atalkigrp_request_debug,
    &atalkigrp_hello_debug,
    &atalkigrp_query_debug,
    &atalkigrp_update_debug,
    &atalkigrp_redist_debug,
    &atalkigrp_packet_debug,
    &atalkigrp_target_debug,
    &atalkigrp_peer_debug,
    &atalkigrp_debug
};

static ulong const atigrp2_nflagstoshow =
    sizeof(atigrp2_flagstoshow) / sizeof(atalkdebugtype **);


/*
 * atigrp2_debug_command
 *
 * Called by the parser chain.
 */
void
atigrp2_debug_command (parseinfo *csb)
{
    if (csb == NULL)
	return;

    switch (csb->which) {
      case DEBUG_ATALKIGRP_EXTERNAL:
	atalk_debug_setflag(csb, atalkigrp_external_debug);
	atalk_debug_show_flag(atalkigrp_external_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKIGRP_REQUEST:
	atalk_debug_setflag(csb, atalkigrp_request_debug);
	atalk_debug_show_flag(atalkigrp_request_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKIGRP_HELLO:
	atalk_debug_setflag(csb, atalkigrp_hello_debug);
	atalk_debug_show_flag(atalkigrp_hello_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKIGRP_QUERY:
	atalk_debug_setflag(csb, atalkigrp_query_debug);
	atalk_debug_show_flag(atalkigrp_query_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKIGRP_UPDATE:
	atalk_debug_setflag(csb, atalkigrp_update_debug);
	atalk_debug_show_flag(atalkigrp_update_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKIGRP_REDIST:
	atalk_debug_setflag(csb, atalkigrp_redist_debug);
	atalk_debug_show_flag(atalkigrp_redist_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKIGRP_PACKET:
	atalk_debug_setflag(csb, atalkigrp_packet_debug);
	atalk_debug_show_flag(atalkigrp_packet_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKIGRP_TARGET:
	atalk_debug_setflag(csb, atalkigrp_target_debug);
	atigrp2_ddb_debug(csb);
	atalk_debug_show_flag(atalkigrp_target_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKIGRP_PEER:
	atalk_debug_setflag(csb, atalkigrp_peer_debug);
	atigrp2_ddb_debug(csb);
	atalk_debug_show_flag(atalkigrp_peer_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKIGRP_ALL:
	atigrp2_debug_all(csb->sense);
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }
}

/*
 * atigrp2_debug_all
 * Registered to be called whenever "debug all" or "undebug all" is issued,
 * or whenever it's desirable to set all the AT/EIGRP debug flags at once.
 *
 * atigrp2_target_debug isn't set, since that is really a special hack for
 * DUAL debugging.
 */
static void
atigrp2_debug_all (boolean flag)
{
    int num;
    if (flag)
	num = -1;
    else
	num = 0;

    atalkigrp_debug->flag = num;
    atalkigrp_update_debug->flag = num;
    atalkigrp_query_debug->flag = num;
    atalkigrp_request_debug->flag = num;
    atalkigrp_hello_debug->flag = num;
    atalkigrp_external_debug->flag = num;
    atalkigrp_redist_debug->flag = num;
    atalkigrp_packet_debug->flag = num;
    atalkigrp_target_debug->flag = num;
    atalkigrp_peer_debug->flag = num;
}

/*
 * atigrp2_debug_show
 * Implements the "show debug" command for AT/EIGRP flags.
 */
static void
atigrp2_debug_show (void)
{
    ulong i;
    boolean hdisplayed = FALSE;

    for (i = 0; i < atigrp2_nflagstoshow; i++) {
	if (!hdisplayed &&
	    ((debug_all_p() && ((*atigrp2_flagstoshow[i])->flag != -1)) ||
	     (!debug_all_p() && ((*atigrp2_flagstoshow[i])->flag != 0)))) {
	    printf("\nAT/EIGRP:");
	    hdisplayed = TRUE;
	}
	atalk_debug_show_flag(*atigrp2_flagstoshow[i], FALSE, TRUE);
    }
}

/*
 * atigrp2_debug_init
 * Malloc and initialize all AT/EIGRP debugging flags, do general
 * AT/EIGRP debugging setup.
 */
void
atigrp2_debug_init (void)
{
    /* Make multiple calls harmless */
    if (atigrp2_debug_flags != NULL)
	return;

    /* Allocate space for all the debugging flags and attach descriptions */
    atigrp2_debug_flags = malloc(sizeof(atalkdebugtype) * DEBUG_ATALKIGRP_MAX);
    if (atigrp2_debug_flags == NULL)
	return;

    atalkigrp_events         = &atigrp2_debug_flags[DEBUG_ATALKIGRP_EVENTS];
    atalkigrp_external_debug = &atigrp2_debug_flags[DEBUG_ATALKIGRP_EXTERNAL];
    atalkigrp_request_debug  = &atigrp2_debug_flags[DEBUG_ATALKIGRP_REQUEST];
    atalkigrp_hello_debug    = &atigrp2_debug_flags[DEBUG_ATALKIGRP_HELLO];
    atalkigrp_query_debug    = &atigrp2_debug_flags[DEBUG_ATALKIGRP_QUERY];
    atalkigrp_update_debug   = &atigrp2_debug_flags[DEBUG_ATALKIGRP_UPDATE];
    atalkigrp_redist_debug   = &atigrp2_debug_flags[DEBUG_ATALKIGRP_REDIST];
    atalkigrp_packet_debug   = &atigrp2_debug_flags[DEBUG_ATALKIGRP_PACKET];
    atalkigrp_target_debug   = &atigrp2_debug_flags[DEBUG_ATALKIGRP_TARGET];
    atalkigrp_peer_debug     = &atigrp2_debug_flags[DEBUG_ATALKIGRP_PEER];

    atalkigrp_events->text         = atigrp2_debug_text[DEBUG_ATALKIGRP_EVENTS];
    atalkigrp_external_debug->text = atigrp2_debug_text[DEBUG_ATALKIGRP_EXTERNAL];
    atalkigrp_request_debug->text  = atigrp2_debug_text[DEBUG_ATALKIGRP_REQUEST];
    atalkigrp_hello_debug->text    = atigrp2_debug_text[DEBUG_ATALKIGRP_HELLO];
    atalkigrp_query_debug->text    = atigrp2_debug_text[DEBUG_ATALKIGRP_QUERY];
    atalkigrp_update_debug->text   = atigrp2_debug_text[DEBUG_ATALKIGRP_UPDATE];
    atalkigrp_redist_debug->text   = atigrp2_debug_text[DEBUG_ATALKIGRP_REDIST];
    atalkigrp_packet_debug->text   = atigrp2_debug_text[DEBUG_ATALKIGRP_PACKET];
    atalkigrp_target_debug->text   = atigrp2_debug_text[DEBUG_ATALKIGRP_TARGET];
    atalkigrp_peer_debug->text     = atigrp2_debug_text[DEBUG_ATALKIGRP_TARGET];

    /* Register for "debug/undebug all" and "show debug" commands */
    reg_add_debug_all(atigrp2_debug_all, "atigrp2_debug_all");
    reg_add_debug_show(atigrp2_debug_show, "atigrp2_debug_show");

    /* Initialize the flags to reflect the currents setting of "debug all" */
    atigrp2_debug_all(debug_all_p());
}
