/* $Id: spx_chain.c,v 3.5.20.1 1996/05/21 10:11:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/xns/spx_chain.c,v $
 *------------------------------------------------------------------
 * spx_chain.c   Sequenced Packet Exchange Protocol  (Parser support
 *                 C code
 *
 * September 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: spx_chain.c,v $
 * Revision 3.5.20.1  1996/05/21  10:11:50  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/01  06:13:29  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:32:03  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:42:41  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  19:24:46  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:40:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  11:57:14  irfan
 * Sequenced Packet Exchange Protocol: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "config.h"
#include "parser.h"
#include "../iprouting/route.h"
#include "../xns/novell.h"
#include "../xns/novroute.h"
#include "../ui/debug.h"
#include "xns.h"
#include "novell.h"
#include "novell_debug.h"

#include "../parser/parser_actions.h"
#include "../xns/novell_actions.h"
#include "../parser/macros.h"
#include "../xns/parser_defs_xns_nov.h"
#include "../parser/parser_defs_sse.h"
#include "../iprouting/parser_defs_igrp2.h"
#include "../clns/parser_defs_isis.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../xns/spx.h"
#include "../xns/spx_debug.h"

/*
 * Forward declarations
 */
static void print_spx_sock(spxsocktype *);
static void print_spx_con(spxcontype *);



static void spx_show (void)
{

    spxsocktype *spxsock;
    spxcontype *spxcon;

    if (!spx_running) {
	printf("\n%%SPX not running");
	return;
    }

    printf("\nNext wake time: ", spx_wake_time);

    FOR_ALL_SPX_SOCKETS(spxsock) {
	mem_lock(spxsock);
	print_spx_sock(spxsock);
	FOR_ALL_SPXCON_ON_SOCK(spxsock, spxcon) {
	    mem_lock(spxcon);
	    print_spx_con(spxcon);
	    free(spxcon);
	}
	free(spxsock);
	printf("\n");
    }
    printf("\n");
}

static void print_spx_sock (spxsocktype *spxsock)
{

    printf("\n\nSPX socket: %x", spxsock->sock_num);
    printf("\n  state: %x  Connections: %u", spxsock->state,
	   QUEUESIZE(&spxsock->spxconQ));
}

static void print_spx_con (spxcontype *spxcon)
{
    printf ("\n\n   SPX Remote: %x::%e:%x   Local: %x::%e:%x",
	    spxcon->remote_net, spxcon->remote_host, spxcon->remote_con,
	    spxcon->local_net, spxcon->local_host, spxcon->local_con);
    printf("\n   state %x  flags %x", spxcon->state, spxcon->flags);
    printf("\n   Queue counts:  inq %d,  outQ %d,  unackedQ %d",
	   QUEUESIZE(&spxcon->inQ), QUEUESIZE(&spxcon->outQ),
	   QUEUESIZE(&spxcon->unackedQ));
    printf("\n   Sequence: %u,  Ack: %u,  local-alloc: %u,"
	   "  remote-alloc: %u", spxcon->local_next_seq,
	   spxcon->remote_next_seq,
	   spxcon->local_alloc_num+spxcon->remote_next_seq,
	   spxcon->remote_alloc_num);
    if (TIMER_RUNNING(spxcon->purge_timer)) {
	printf("\n   Purge Timer fires in %u secs",
	       TIME_LEFT_SLEEPING(spxcon->purge_timer) / ONESEC);
    }
    if (TIMER_RUNNING(spxcon->abort_timer)) {
	printf("\n   Abort Timer fires in %u secs",
	       TIME_LEFT_SLEEPING(spxcon->abort_timer) / ONESEC);
    }
    if (TIMER_RUNNING(spxcon->send_verify_watchdog)) {
	printf("\n   Verify Watchdog Timer fires in %u secs",
	       TIME_LEFT_SLEEPING(spxcon->send_verify_watchdog) / ONESEC);
    }
}

/*
 * show_spx
 *
 * interface to the "show ipx spx" command
 */
void show_spx (parseinfo *csb)
{
    switch (csb->which) {
	
    case SHOW_SPX_ALL:
	spx_show();
	break;

    default:
	break;
    }
}


/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_ipx_spx.h"
LINK_POINT(ipx_spx_debug_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for spx "show" commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ipx_spx.h"
LINK_POINT(ipx_spx_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for IPX/SPX
 */
static parser_extension_request spx_chain_init_table[] = {

    { PARSE_ADD_SHOW_IPX_CMD,&pname(ipx_spx_show_commands) },
    { PARSE_ADD_DEBUG_IPX_CMD,&pname(ipx_spx_debug_commands) },
    { PARSE_LIST_END, NULL }
};

void spx_setup_parser (void)
{
    parser_add_command_list(spx_chain_init_table, "spx");
}
