/* $Id: nasi_chain.c,v 3.4.20.4 1996/07/26 00:08:36 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/nasi_chain.c,v $
 *------------------------------------------------------------------
 * NASI_chain.c Netware Asynchronous Services Interface
 *              (Parser/UI support code)
 *
 * Sept 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nasi_chain.c,v $
 * Revision 3.4.20.4  1996/07/26  00:08:36  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.4.20.3  1996/06/10  19:24:22  kleung
 * CSCdi57965:  Misc. cosmetic command bugs.
 * Branch: California_branch
 *
 * Revision 3.4.20.2  1996/06/07  01:17:43  irfan
 * CSCdi55494:  NASI server does not work after reload
 * Branch: California_branch
 * Don't reopen NASI's SPX socket if already opened.
 *
 * Revision 3.4.20.1  1996/05/21  10:11:22  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/01/29  07:31:45  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:42:24  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  19:22:21  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  12:13:04  irfan
 * Netware Asynchronous Services Interface: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Netware Asynchronous Services Interface  Parser support code
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "sys_registry.h"
#include "address.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "logger.h"
#include "../os/free.h"
#include "xns.h"
#include "novell.h"
#include "novell_public.h"
#include "config.h"
#include "../if/network.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../xns/spx.h"
#include "../xns/nasi.h"
#include "../ui/debug.h"
#include "../xns/nasi_debug.h"
#include "../h/auth_registry.regh"
#include "aaa.h"

/*
 * some constants
 */
#define NASI_CMD_START 1

static void nasi_show_all_con(void);
static void nasi_show_con(spxcontype *);


/*
 * nasi_show_all_con
 *
 * print the list of SPX connections on the NASI socket.
 * Not all are connected to tty's.
 */
static void nasi_show_all_con (void)
{

    spxcontype *spxcon;

    if (!nasi_my_spx_sock) {
	printf("\n%%NASI not running");
	return;
    }

    FOR_ALL_SPXCON_ON_SOCK(nasi_my_spx_sock, spxcon) {
	mem_lock(spxcon);
	nasi_show_con(spxcon);
	free(spxcon);
    }
}

/*
 * nasi_show_con
 *
 * print individual nasi type spx connection
 */
static void nasi_show_con (spxcontype *spxcon)
{
    printf ("\n\nNASI Remote: %x::%e:%x   Local: %x::%e:%x",
	    spxcon->remote_net, spxcon->remote_host, spxcon->remote_con,
	    spxcon->local_net, spxcon->local_host, spxcon->local_con);
    printf("\n  flags %x", spxcon->nasi_flags);
    if (spxcon->tty) {
	printf("\n   Connected to line %u  incount %u  outcount %u  OVF %u",
	       spxcon->tty->ttynum, spxcon->nasi_incount,
	       spxcon->nasi_outcount, spxcon->nasi_inoverflow);
    }
}

/*
 * show_nasi
 *
 * interface to the "show nasi" command
 */
void show_nasi (parseinfo *csb)
{
    switch (csb->which) {
	
    case SHOW_NASI_CON:
	nasi_show_all_con();
	break;

    default:
	break;
    }
}


/*
 * nasi_line_subcommand
 *
 * interface to the "nasi" line sub-command
 */
void nasi_line_subcommand (parseinfo *csb)
{

    tt_soc *tty;
    int ttycount;

    if (csb->nvgen) {
	tty = MODEMS[csb->line1];
	switch(csb->which) {

	case LINE_NASI_AUTH:
	    nv_write(tty->nasi_acc_list_name[0], "%s %s", csb->nv_command,
		     tty->nasi_acc_list_name);
	    break;

	default:
	    break;
	}

	return;
    }

    for (ttycount = csb->line1; ttycount < csb->line2; ttycount++) {

    	tty = MODEMS[ttycount];

	switch (csb->which) {

	case LINE_NASI_AUTH:

	    if (csb->sense) {
		char *list_name;
		switch (GETOBJ(int,2)) {

		case 1:
		    list_name = "default";
		    tty->nasi_acc_list_name[0] = '\0';
		    break;

		case 2:
		    list_name = GETOBJ(string,1);
		    sstrncpy(tty->nasi_acc_list_name, list_name,
			     sizeof(tty->nasi_acc_list_name));
		    break;

		default:
		    printf("\n%% AAA: NASI: bad value in authentication"
			   " list.");
		    list_name = NULL;
		    break;
		}

		if ((RES_MANUAL == csb->resolvemethod) &&
		    list_name &&
		    !aaa_is_list_defined(list_name, AAA_SVC_NASI)) {
		    printf("AAA: Warning authentication list \"%s\""
			   " is not defined for NASI.\n", list_name);
		}

	    } else {
		tty->nasi_acc_list_name[0] = '\0';
	    }
	    break;

	default:
	    break;

	}
    }
}

void nasi_enable_command (parseinfo *csb)
{
    if (csb->nvgen) {
	switch (csb->which) {

	case NASI_CMD_START:
	    nv_write(nasi_configured, "%s", csb->nv_command);
	    break;

	default:
	    break;
	}
	return;
    }

    if (csb->sense) {
	switch (csb->which) {

	case NASI_CMD_START:
	    if (!novell_started || !novell_running || novell_shutting_down) {
		printf("\n%%IPX Routing not available. Please configure:"
		       "\nipx routing,"
		       "\nipx internal-network xxxx,"
		       "\nipx network yyyy <on Ether/Token interface>");
		return;
	    }
	    nasi_configured = TRUE;
	    nasi_server_enable();
	    break;

	default:
	    break;
	}
    } else {

	switch (csb->which) {
	case NASI_CMD_START:
	    nasi_configured = FALSE;
	    nasi_server_disable();
	    break;

	default:
	    break;
	}
    }
}

/*
 * Parse chains for nasi "config" commands
 */
#define	ALTERNATE	NONE
#include "cfg_nasi.h"
LINK_POINT(cfg_nasi_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for nasi "show" commands
 */
#define	ALTERNATE	NONE
#include "exec_show_nasi.h"
LINK_POINT(nasi_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for "nasi" line sub-commands
 */
#define	ALTERNATE	NONE
#include "cfg_line_nasi.h"
LINK_POINT(nasi_line_subcommands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for Debug NASI commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_nasi.h"
LINK_POINT(debug_nasi_commands, ALTERNATE);
#undef	ALTERNATE



/*
 * Parse chain registration array for nasi commands
 */
static parser_extension_request nasi_chain_init_table[] = {

    { PARSE_ADD_CFG_TOP_IPX_CMD, &pname(cfg_nasi_commands) },
    { PARSE_ADD_SHOW_IPX_CMD, &pname(nasi_show_commands) },
    { PARSE_ADD_CFG_LINE_CMD, &pname(nasi_line_subcommands) },
    { PARSE_ADD_DEBUG_IPX_CMD, &pname(debug_nasi_commands) },
    { PARSE_LIST_END, NULL }
};


void nasi_setup_parser (void)
{
    parser_add_command_list(nasi_chain_init_table, "nasi");
}
