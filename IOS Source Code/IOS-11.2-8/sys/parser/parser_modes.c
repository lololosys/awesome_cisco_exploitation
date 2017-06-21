/* $Id: parser_modes.c,v 3.3.4.3 1996/08/28 13:05:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_modes.c,v $
 *------------------------------------------------------------------
 * P A R S E R _ M O D E S . C  (formerly M O D E . C)
 *
 * Parser Mode functions
 *
 * October 1994, Robert Widmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_modes.c,v $
 * Revision 3.3.4.3  1996/08/28  13:05:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.4.2  1996/05/21  09:59:23  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.4.1  1996/03/18  21:33:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  07:22:48  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  10:40:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  16:49:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/03/09  22:57:09  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/12/17  18:35:20  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/12/15  03:46:19  thille
 * Parser organizational cleanup.  Rename core parser files to be
 * parser_<foo>.  Start by adding placeholders for the new filenames.
 *
 * OLD LOG as mode.c:
 * ------------------
 * Revision 3.4  1995/12/07  22:32:12  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/12/01  15:57:03  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  18:48:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/15  15:19:29  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  22:28:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "parser_actions.h"
#include "parser_modes.h"
#include "msg_parser.c"			/* Not a typo, see logger.h */

/*
 * Local structure definitions
 */
struct parser_mode_ {
    struct parser_mode_ *next;
    const char *name;
    const char *prompt;
    const char *help;
    const char *alt_mode;
    mode_save_var_func save_vars;
    mode_reset_var_func reset_vars;
    mode_http_init_func http_init;
    uint flags;
    transition *top;
    queuetype *aliases;
    queuetype *def_aliases;
    boolean priv_changed;
};

boolean parser_mode_debug = TRUE;
static queuetype parser_modes_queue;
static queuetype *parser_modes = &parser_modes_queue;
static boolean parser_modes_inited = FALSE;


/*
 * parser_modes_init
 * Initialize well known parser modes and add default aliases
 */
void parser_modes_init (void)
{
    if (!parser_modes_inited) {
	parser_modes_inited = TRUE;
	parser_modes = &parser_modes_queue;
	queue_init(parser_modes, 0);

	parser_add_main_modes();
    }
}


/*
 * create_new_mode
 * Allocate and fill in new mode structure
 */
static parser_mode *create_new_mode (const char *name, const char *prompt,
				     const char *help, 
				     boolean do_aliases, boolean do_privileges,
				     const char *alt_mode,
				     mode_save_var_func save_vars,
				     mode_reset_var_func reset_vars,
				     transition *top,
				     mode_http_init_func http_init)
{
    parser_mode *new_mode;

    if (!parser_modes_inited) {
	parser_modes_init();
    }

    new_mode = malloc_named(sizeof(parser_mode), "Parser Mode");
    if (new_mode == NULL) {
	return(NULL);
    }

    new_mode->name = name;
    new_mode->prompt = prompt;
    new_mode->help = help;
    new_mode->alt_mode = alt_mode;
    new_mode->save_vars = save_vars;
    new_mode->reset_vars = reset_vars;
    new_mode->flags = PARSER_MODE_NONE;
    if (do_aliases) {
	new_mode->flags |= PARSER_MODE_ALIASES;
    }
    if (do_privileges) {
	new_mode->flags |= PARSER_MODE_PRIVILEGE;
    }
    new_mode->top = top;
    new_mode->aliases = malloc_named(sizeof(queuetype), "Parser Mode Q1");
    if (new_mode->aliases) {
	queue_init(new_mode->aliases, 0);
    }
    new_mode->def_aliases = malloc_named(sizeof(queuetype), "Parser Mode Q2");
    if (new_mode->def_aliases) {
	queue_init(new_mode->def_aliases, 0);
    }
    new_mode->priv_changed = FALSE;
    new_mode->http_init = http_init;

    if (parser_mode_debug) {
	buginf("\nAdding parser mode: '%s'  0x%x", name, new_mode);
    }

    return(new_mode);
}


/*
 * parser_add_mode
 * Create and add new parser mode
 *	name - mode name, used in MODE macro for matching, so
 *	       can't contain whitespace
 *	prompt - mode prompt extension, ie. (config-if), except
 *		 without the parentheses
 *	help - help strings describing mode, used as MODE macro help
 *	alt_mode - alternate mode to try if parse fails
 *	save_vars - function to save csb vars when trying alt mode
 *	reset_vars - function to reset csb vars when alt mode fails
 *	do_aliases - are aliases allowed in this mode?
 *	do_privileges - can privilege levels be changed in this mode?
 *	top - top of parse chain for parsing
 */
parser_mode *parser_add_mode (const char *name, const char *prompt,
			      const char *help,
			      boolean do_aliases, boolean do_privileges,
			      const char *alt_mode,
			      mode_save_var_func save_vars,
			      mode_reset_var_func reset_vars,
			      transition *top,
			      mode_http_init_func http_init)
{
    parser_mode *mode;

    mode = create_new_mode(name, prompt, help, do_aliases, do_privileges,
			   alt_mode, save_vars, reset_vars, top, http_init);
    if (mode) {
	enqueue(parser_modes, mode);
    }
    return(mode);
}	


/*
 * get_mode_tmp
 * Create temporary mode
 */
parser_mode *get_mode_tmp (const char *name, const char *prompt,
			   const char *help,
			   boolean do_aliases, boolean do_privileges,
			   transition *top)
{
    return(create_new_mode(name, prompt, help, do_aliases, do_privileges,
			   NULL, NULL, NULL, top, NULL));
}


/*
 * free_mode
 *
 */
void free_mode (parser_mode *mode)
{
    free_aliases(mode->aliases);
    free_aliases(mode->def_aliases);
    free(mode);
}


/*
 * show_parser_modes
 *
 */
void show_parser_modes (parseinfo *csb)
{
    parser_mode *mode;

    mode = parser_modes->qhead;
    if (mode) {
	printf("\nParser modes:");
	printf("\nName                Prompt              Top       "
	       "Alias   Privilege\n");
	while (mode) {
	    printf("%20s%20s0x%8x%8s%8s\n",
		   mode->name, mode->prompt, mode->top,
		   ((mode->flags & PARSER_MODE_ALIASES) ? "TRUE" : "FALSE"),
		   ((mode->flags & PARSER_MODE_PRIVILEGE) ? "TRUE" : "FALSE"));
	    mode = mode->next;
	}
    } else {
	printf("\nNo parser modes");
    }
}


/*
 * get_mode_byname
 * Find mode with name
 */
parser_mode *get_mode_byname (const char *name, uint volume)
{
    parser_mode *mode;

    if (!parser_modes_inited) {
	parser_modes_init();
    }

    mode = parser_modes->qhead;
    while (mode) {
	if (strcmp(name, mode->name) == 0) {
	    break;
	}
	mode = mode->next;
    }
    if (parser_mode_debug) {
	buginf("\nLook up of parser mode '%s' %s",
	       name, mode ? "succeeded" : "failed");
    }
    if (!mode && (volume == MODE_VERBOSE)) {
	printf("\n%% Unable to find %s mode", name);
    }

    return(mode);
}


/*
 * set_mode_byname
 * Set current mode to name
 */
boolean set_mode_byname (parser_mode **mode, const char *name, uint volume)
{
    parser_mode *new_mode;

    new_mode = get_mode_byname(name, volume);
    if (new_mode) {
	*mode = new_mode;
	return(TRUE);
    } else {
	return(FALSE);
    }
}


/*
 * test_mode_config_subcommand
 * Return TRUE if mode is submode of config mode
 */
boolean test_mode_config_subcommand (parseinfo *csb)
{
    if (csb && csb->mode && csb->mode->alt_mode) {
	return(TRUE);
    }
    return(FALSE);
}

parser_mode *get_alt_mode (parser_mode *mode)
{
    if (mode && mode->alt_mode) {
	return(get_mode_byname(mode->alt_mode, MODE_SILENT));
    } else {
	return(NULL);
    }
}



/*
 * get_mode_aliases
 * Return pointer to aliases in this mode
 */
queuetype *get_mode_aliases (parser_mode *mode)
{
    if (mode) {
	return(mode->aliases);
    } else {
	return(NULL);
    }
}


/*
 * get_mode_defaliases
 * Return pointer to default aliases in this mode
 */
queuetype *get_mode_defaliases (parser_mode *mode)
{
    if (mode) {
	return(mode->def_aliases);
    } else {
	return(NULL);
    }
}


/*
 * get_mode_prompt
 * Return prompt extension for this mode
 */
const char *get_mode_prompt (parseinfo *csb)
{
    if (csb && csb->mode) {
	return(csb->mode->prompt);
    } else {
	return("UNKNOWN-MODE");
    }
}


/*
 * get_mode_top
 * Return top of parse chain for this mode
 */
transition *get_mode_top (parser_mode *mode)
{
    if (mode) {
	return(mode->top);
    } else {
	return(NULL);
    }
}


/*
 * get_mode_nv
 * Return top of parse chain for NV generation for this mode
 */
transition *get_mode_nv (parseinfo *csb)
{
    if (csb && csb->mode) {
	return(csb->mode->top);
    } else {
	return(NULL);
    }
}


/*
 * get_mode_name
 * Return name of mode
 */
const char *get_mode_name (parser_mode *mode)
{
    return(mode->name);
}



/*
 * match_mode
 * Parser function for matching the MODE macro
 */
uint match_mode (parseinfo *csb, int *pi,
		 parser_mode **matched_mode, uint flags)
{
    parser_mode *mode;
    uint num_matches = 0;
    int i;
    int matched_pi = *pi;
    char *word;

    mode = parser_modes->qhead;
    while (mode) {
	if (mode->flags & flags) {
	    i = *pi;
	    if (match_partial_keyword(csb, mode->name, &i, mode->help,
				      0, KEYWORD_WS_OK | KEYWORD_HELP_CHECK)) {
		*matched_mode = mode;
		matched_pi = i;
		num_matches++;

		word = csb->line + csb->line_index;
		if (strncasecmp(mode->name, word, strlen(mode->name)) == 0) {
		    *pi = matched_pi;
		    return(1);
		}
	    }
	}
	mode = mode->next;
    }
    if (num_matches > 1) {
	*matched_mode = NULL;
	return(0);
    }
    *pi = matched_pi;
    return(num_matches);
}


/*
 * nv_mode
 * Do NV generation for MODE macro
 */
void nv_mode (parseinfo *csb, transition *mine, mode_struct * const arg)
{
    parser_mode *mode;

    mode = parser_modes->qhead;
    while (mode) {
	if (mode->flags & arg->flags) {
	    *CSBVAR(csb, arg->offset, parser_mode *) = mode;
	    nvgen_token(csb, mine, mode->name);
	}
	mode = mode->next;
    }
}


/*
 * display_mode_aliases
 * Show mode aliases
 *	if (mp != NULL)
 *	    show mp mode aliases
 *	else
 *	    show all mode aliases
 */
void display_mode_aliases (parseinfo *csb, parser_mode *mp)
{
    parser_mode *mode;

    mode = parser_modes->qhead;
    while (mode) {
	if (!mp || (mp == mode)) {
	    show_mode_aliases(csb, mode->aliases, mode->help);
	}
	mode = mode->next;
    }
}


void *mode_save_vars (parseinfo *csb)
{
    if (csb && csb->mode &&
	csb->mode->save_vars && csb->mode->reset_vars) {
	return(csb->mode->save_vars(csb));
    } else {
	return(NULL);
    }
}

void mode_reset_vars (parseinfo *csb, void *var)
{
    if (csb && csb->mode && csb->mode->reset_vars) {
	csb->mode->reset_vars(csb, var);
    }
}

boolean priv_changed_in_mode (parser_mode *mode)
{
    return(mode->priv_changed);
}

void set_priv_changed_in_mode (parser_mode *mode, boolean val)
{
    mode->priv_changed = val;
}

boolean mode_http_init (parseinfo *csb)
{
    if (csb && csb->mode && csb->mode->http_init) {
	return(csb->mode->http_init(csb));
    } else {
	return(TRUE);
    }
}
