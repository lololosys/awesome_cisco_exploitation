/* $Id: prompt.c,v 3.4.10.2 1996/05/21 10:07:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/ui/prompt.c,v $
 *------------------------------------------------------------------
 * prompt.c  -- Prompt customization
 *  
 * 23-Aug-1994, Andy Valencia
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: prompt.c,v $
 * Revision 3.4.10.2  1996/05/21  10:07:50  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.10.1  1996/03/18  22:29:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.4  1996/03/16  07:45:59  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.3  1996/03/14  01:36:18  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.14.2  1996/03/07  10:58:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  21:26:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/22  14:39:37  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:40:21  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:49:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  07:19:46  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:11:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "ttysrv.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "sys_registry.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

static int prompt_rewrite(parseinfo *, char *, int *);
static void prompt_command(parseinfo *);

/*
 * The current override prompt
 */
static char *cur_prompt;

/*
 * Our parse chain to config the prompt override
 */
#define ALTERNATE NONE
#include "../parser/cfg_prompt.h"
LINK_POINT(prompt_commands, ALTERNATE);
#undef ALTERNATE
static parser_extension_request prompt_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(prompt_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * prompt_init()
 *	Initialize custom prompting
 *
 * This includes hooking in the prompt parse chains and registering with
 * the parser hook to rewrite the "final" prompt.
 */
void
prompt_init (subsystype *subsys)
{
    parser_add_command_list(prompt_chain_init_table, "prompt");
    reg_add_parse_cli_prompt(PARSER_CLI_OVERRIDE, prompt_rewrite, "prompt");
    cur_prompt = NULL;
}

/*
 * prompt_rewrite()
 *	Given full prompt, rewrite it based on currently configured prompt
 */
static int
prompt_rewrite (parseinfo *csb, char *prompt, int *prompt_length)
{
    char c, *src, *dest;
    int olen;

    /*
     * If no special prompt configured, don't touch anything
     */
    src = cur_prompt;
    if (src == NULL) {
	return(0);
    }

    /*
     * If configuring, let normal prompt through--too confusing if we don't
     */
    if (csb->mode != exec_mode) {
	return(0);
    }

    /*
     * Record old length so we can return the size adjustment
     */
    dest = csb->prompt;
    olen = prompt - dest;
    *prompt_length = MAXPROMPTLEN;

    /*
     * Walk prompt string, assembling the new prompt
     */
    while ((c = *src++)) {
	/*
	 * Protect against overflowing the prompt buffer.  We leave a
	 * couple bytes to be safe.
	 */
	if ((*prompt_length)-- < 2) {
	    continue;
	}

	/*
	 * Non-special, just copy into string
	 */
	if (c != '%') {
	    *dest++ = c;
	    continue;
	}

	/*
	 * Escapes
	 */
	c = *src++;
	switch (c) {

	case 'h':	/* Hostname */
	    /*
	     * No hostname, put in nothing.  System init should have at
	     * least put in "Router", but we'll protect ourselves just
	     * in case.
	     */
	    if (hostname == NULL) {
		break;
	    }

	    /*
	     * If the hostname fits, copy it in.  If not, copy in
	     * what will fit.
	     */
	    if (strlen(hostname) > *prompt_length) {
		bcopy(hostname, dest, *prompt_length);
		dest += *prompt_length;
		*prompt_length = 0;
	    } else {
		bcopy(hostname, dest, strlen(hostname));
		dest += strlen(hostname);
	    }
	    break;

	case 'n':	/* TTY number */
	    if (*prompt_length < 3) {
		break;
	    }
	    dest += sprintf(dest, "%d", stdio->ttynum);
	    break;

	case 'p':	/* Prompt char--#/> for enabled/disabled */
	    *dest++ = ((CUR_PRIV(stdio->privilege) > PRIV_USER) ? '#' : '>');
	    break;

	case 's':	/* Space/tab */
	    *dest++ = ' ';
	    break;
	case 't':
	    *dest++ = '\t';
	    break;

	case '\0':	/* Last char was %--naughty, naughty */
	    src -= 1;
	    break;

	default:	/* Just put literal */
	    *dest++ = c;
	    break;
	}
    }

    /*
     * Null-terminate
     */
    *dest++ = '\0';

    /*
     * Return an adjustment of length before and after
     */
    return(strlen(csb->prompt) - olen);
}

/*
 * prompt_command()
 *	Set our system prompt string
 */
static void
prompt_command (parseinfo *csb)
{
    /*
     * NV generation, write out current prompt if any
     */
    if (csb->nvgen) {
	nv_write(cur_prompt != NULL, "%s %s", csb->nv_command, cur_prompt);
	return;
    }

    /*
     * Clear custom prompt
     */
    if (cur_prompt) {
	free(cur_prompt);
    }
    cur_prompt = NULL;

    /* 
     * If "no prompt string" then that's it
     */
    if (!csb->sense) {
	return;
    }

    /*
     * Set prompt to string
     */
    cur_prompt = strdup(GETOBJ(string,1));
}
