/* $Id: techsupport.c,v 3.1.2.5 1996/07/23 23:22:44 avaz Exp $
 * $Source: /release/112/cvs/Xsys/os/techsupport.c,v $
 *------------------------------------------------------------------
 * Tech-support functions
 *
 * April 1996, Robert Widmer
 *
 * Copyright (c) 1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: techsupport.c,v $
 * Revision 3.1.2.5  1996/07/23  23:22:44  avaz
 * CSCdi62098:  add show diagbus and show cont cbus to show tech-support
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/07/19  01:02:15  avaz
 * CSCdi62098:  add show diagbus and show cont cbus to show tech-support
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/17  22:42:34  widmer
 * CSCdi59426:  <sh tech-support> adds <length 0> under line con 0
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/21  09:58:29  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.2.1  1996/04/15  14:59:16  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.1  1996/04/12  19:08:48  widmer
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "techsupport.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "subsys.h"
#include "sys_registry.h"

#define SHOW_TECHSUPPORT_PAGE	0x1
#define SHOW_TECHSUPPORT_PW	0x2

const char *techsupport_blank_string = "<removed>";

void show_techsupport_command (parseinfo *old_csb, const char *cmd)
{
    parseinfo *csb;

    csb = get_csb("Show tech-support");
    if (!csb) {
	return;
    }

    csb->priv = old_csb->priv;
    csb->mode = exec_mode;
    csb->lastchar = ' ';
    if (old_csb->flags & HIDE_PASSWORDS) {
	csb->flags |= HIDE_PASSWORDS;
    }
    strncpy(csb->line, cmd, PARSEBUF-1);
    printf("\n------------------ %s ------------------\n\n", cmd);
    (void) parse_cmd(csb);
    free_csb(&csb);
    automore_enable(NULL);
    automore_conditional(0);
    printf("\n");
    automore_disable();
}

static void show_techsupport (parseinfo *csb)
{
    uint old_length = stdio->tty_length;
    boolean stream = !(GETOBJ(int,1) & SHOW_TECHSUPPORT_PAGE);
    boolean password = GETOBJ(int,1) & SHOW_TECHSUPPORT_PW;

    if (stream) {
	tty_savedefaults(stdio);
	stdio->tty_length = 0;
    }
    if (!password) {
	csb->flags |= HIDE_PASSWORDS;
    }
    SETOBJ(int,1) = 0;

    printf("\n");
    show_techsupport_command(csb, "show version");
    show_techsupport_command(csb, "show running-config");
    if (GETOBJ(int,2)) {
	reg_invoke_show_techsupport_protocol(GETOBJ(int,2), csb);
    } else {
	reg_invoke_show_techsupport(csb);
    }
    printf("\n");

    if (stream) {
	stdio->tty_length = old_length;
    }
}

#define	ALTERNATE	NONE
#include	"exec_show_techsupport.h"
LINK_POINT(show_techsuppport_cmds, ALTERNATE);
#undef	ALTERNATE

static parser_extension_request techsupport_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(show_techsuppport_cmds) },
    { PARSE_LIST_END, NULL }
};

static void techsupport_init (subsystype *subsys)
{
    parser_add_link_point(PARSE_ADD_SHOW_TECHSUPPORT_CMD, "tech-support",
			  &pname(show_techsupport_extend_here));
    parser_add_link_exit(PARSE_ADD_SHOW_TECHSUPPORT_EXIT, "tech-support",
			 &pname(show_techsupport_return_here));
    parser_add_command_list(techsupport_chain_init_table,"tech-support");
}

#define	TECHSUPPORT_MAJVERSION	1
#define	TECHSUPPORT_MINVERSION	0
#define	TECHSUPPORT_EDITVERSION	1

SUBSYS_HEADER(techsupport, TECHSUPPORT_MAJVERSION,
	      TECHSUPPORT_MINVERSION, TECHSUPPORT_EDITVERSION,
	      techsupport_init, SUBSYS_CLASS_MANAGEMENT,
	      NULL, NULL);
