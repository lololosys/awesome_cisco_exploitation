/* $Id: parser.c,v 3.19.4.13 1996/09/05 13:59:47 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser.c,v $
 *------------------------------------------------------------------
 * P A R S E R . C
 *
 * Parser specific routines.
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser.c,v $
 * Revision 3.19.4.13  1996/09/05  13:59:47  widmer
 * CSCdi67994:  Remove names from parser link points
 * Branch: California_branch
 *
 * Revision 3.19.4.12  1996/08/28  13:05:02  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.19.4.11  1996/07/12  02:01:31  widmer
 * CSCdi52582:  parser should remove the trailing spaces in the filename
 * Branch: California_branch
 *
 * Revision 3.19.4.10  1996/07/08  22:39:46  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.19.4.9  1996/06/28  23:25:13  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.19.4.8  1996/06/17  23:36:01  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.19.4.7  1996/06/06  04:25:34  widmer
 * CSCdi59599:  Add printf style formatting to parse_line
 * Branch: California_branch
 *
 * Revision 3.19.4.6  1996/06/05  15:05:53  widmer
 * CSCdi57346:  Add printf style formatting to readline prompt
 * Branch: California_branch
 *
 * Revision 3.19.4.5  1996/05/21  09:59:17  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.19.4.4  1996/05/19  00:24:46  rpratt
 * CSCdi57306:  HTML config has problems in new platform
 * Branch: California_branch
 *
 * Revision 3.19.4.3  1996/04/15  14:59:36  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.19.4.2  1996/04/10  19:20:08  widmer
 * CSCdi54243:  HTTP parser commands adds extraneous space
 * Branch: California_branch
 * Remove long-help space at end of automatically accepted http parser cmd
 *
 * Revision 3.19.4.1  1996/03/18  21:33:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.13.2.3  1996/03/16  07:22:32  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.13.2.2  1996/03/07  10:40:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.13.2.1  1996/02/20  16:47:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.19  1996/03/09  22:56:57  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.18  1996/02/22  14:37:01  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.17  1996/02/15  16:56:20  widmer
 * CSCdi48998:  Parser blows chunks
 * Decrease parser parsenode chunks to 21
 * Bail on push_node() if function is NONE_action
 *
 * Revision 3.16  1996/02/09  09:03:07  billw
 * CSCdi43680:  RADIUS doesnt support magic number ip address values
 * also add authstruct->auth_idb, and change auth->tty to auth->auth_tty
 *
 * Revision 3.15  1996/02/07  21:15:21  widmer
 * CSCdi48343:  Remove unused #ifdef STANDALONE from parser code
 *
 * Revision 3.14  1996/02/07  16:14:40  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.13  1996/01/22  06:57:58  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.12  1996/01/11  09:39:52  billw
 * CSCdi45253:  insufficient nesting of menus
 *
 * Revision 3.11  1995/12/17  18:35:17  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.10  1995/12/16  05:12:07  widmer
 * CSCdi45745:  Need to modularize ethernet address parsing more
 *
 * Revision 3.9  1995/12/10  21:13:17  widmer
 * CSCdi45105:  Running-configs line exec-timeout and history change at
 * login
 * Add argument to readline() to specify whether command should be added
 * to history, change existing readline() calls to use TRUE, change
 * askstring() to call readline() with FALSE.
 *
 * Revision 3.8  1995/12/07  22:32:15  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.7  1995/12/03  17:59:19  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.6  1995/12/03  16:42:30  widmer
 * CSCdi44243:  privilege exec level 0 help doesnt show up in config
 * Add sense to parsenode
 * Change help command to privilege level 0
 *
 * Revision 3.5  1995/12/01  15:57:05  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.4  1995/11/28  20:26:38  widmer
 * CSCdi44264:  Change save_line() to use variable-length arguments
 * Remove dead code also.
 *
 * Revision 3.3  1995/11/17  18:48:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:20:23  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:54:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/10/02  20:42:59  lol
 * CSCdi36244:  Console line failing authorization should be more forgiving
 *              Authenticated console users now bypass authorization.
 *
 * Revision 2.9  1995/09/19  18:40:07  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.8  1995/08/08  20:50:28  lol
 * CSCdi37361:  AAA should not require auth for originating PPP connections
 *
 * Revision 2.7  1995/08/03  21:52:08  hampton
 * Convert the parser to use the chunk manager.  This should allow the
 * parser to continue to work in low memory situations.  [CSCdi38208]
 *
 * Revision 2.6  1995/06/21 09:00:22  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.5  1995/06/20  07:15:58  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/15  15:19:32  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.3  1995/06/15  03:42:30  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 * Revision 2.2  1995/06/09  13:12:39  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:28:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ctype.h>
#undef   tolower			/* use library function call */
#include <ciscolib.h>
#include "interface_private.h"
#include "ttysrv.h"
#include "packet.h"
#include "../os/chunk.h"
#include "config.h"
#include "parser.h"
#include "aaa.h"
#include "../os/texti.h"
#include "parser_actions.h"
#include "../ui/command.h"
#include "logger.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "../parser/parser_defs_breakset.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../ui/exec_parser.h"
#include "stdarg.h"
#include "../h/auth_registry.regh"

/*
 * Local structures and variables
 */
static chain_list *cl = NULL;	/* Unresolved parse chains */
static chain_list *ll = NULL;	/* Dynamic link points */
static chunk_type *parsenode_chunks = NULL;
chunk_type *parseinfo_chunks = NULL;

/*
 * Forward declarations
 */
static void zero_csb_strings(parseinfo *csb);
static boolean word_completion(parseinfo *csb);
static boolean add_to_unresolved_chains(uint, dynamic_transition *
#ifdef	PARSER_DEBUG_LINKS
					, const char *
#endif	/* PARSER_DEBUG_LINKS */
					);
static void resolve_parse_chains(void);
static boolean parser_resolve_entry_link_point(chain_link *,
					       dynamic_transition *
#ifdef	PARSER_DEBUG_LINKS
					       , const char *
#endif	/* PARSER_DEBUG_LINKS */
					       );
static boolean parser_resolve_exit_link_point(chain_link *,
					      dynamic_transition *);
static boolean parser_add_link_to_list(uint,
#ifdef	PARSER_DEBUG_LINKS
				       const char *,
#endif	/* PARSER_DEBUG_LINKS */
				       dynamic_transition *, uint);
static parsenode *get_parsenode(parseinfo *);
static boolean parse_line_mode_args(void *, parser_mode *, uint,
				    const char *, va_list);


void push_node (parseinfo *csb, transition *node)
{
    parsenode *pp;

    if (node->func == (trans_func) NONE_action) {
	/*
	 * Nothing to do for NONE transitions, so return
	 */
	return;
    }

    /* If we fail to obtain memory, we will not push this node onto
     * the queue and this transition will never happen.  We cannot
     * propagate a return code all the way back up the stack and even
     * if we did, we'd probably have to just prune this portion of the
     * parse tree and try to keep going.  Doing it here seems the best
     * overall solution.
     */
    if ((pp = get_parsenode(csb)) == NULL) {
	return;
    }
    pp->node = node;
    pp->line_index = csb->line_index;
    if (csb->nvgen || (csb->flags & COMMAND_AUTHORIZE)) {
	strcpy(pp->nv_command, csb->nv_command);
    }
    pp->idb1 = GETOBJ(idb,1);
    pp->cdb1 = GETOBJ(cdb,1);
    pp->val1 = GETOBJ(int,1);
    pp->pdb1 = GETOBJ(pdb,1);
    pp->udb1 = GETOBJ(udb,1);
    pp->udb2 = GETOBJ(udb,2);
    pp->pmp = csb->pmp;
    pp->routemap1 = GETOBJ(routemap,1);
    pp->appn_object_verb = csb->appn_object_verb;
    pp->appn_object_type = csb->appn_object_type;
    strcpy(pp->visible_ambig, csb->visible_ambig.ambig);
    strcpy(pp->hidden_ambig, csb->hidden_ambig.ambig);
    pp->command_visible = csb->command_visible;
    pp->unsupported = csb->unsupported;
    pp->sense = csb->sense;
    pp->set_to_default = csb->set_to_default;
    requeue(csb->tokenQ, pp);
}

/*
 * Recurse through instead of using a stack
 */
void recurse_node (parseinfo *csb, transition *node)
{
    queuetype *t1 = csb->tokenQ;
    queuetype t2;

    queue_init(&t2, 0);
    csb->tokenQ = &t2;
    push_node(csb, node);
    parse_token(csb);
    csb->tokenQ = t1;
}

/*
 * Parse tokens from the top of the token stack.  We check our
 * runtime against the saved runtime and allow other processes
 * access to the CPU every 250ms.  This prevents the starvation
 * of other processes during NVGEN and other CPU intensive operations.
 *
 * While there are tokens on the stack, we pop one off and call the
 * token specific processing routine.  The token specific processing
 * routine will make sure the top of the stack contains the next
 * node that should be processed (either by adding one or more nodes to the
 * stack, or not adding anything to the stack.
 */
void parse_token (parseinfo *csb)
{
    parsenode *tos;		/* Top of the stack */

    /*
     *  Check for disallowed keywords/commands/protocol IDs, etc.
     */
    if (reg_invoke_parser_preparse_filtered(csb) ) {
        tos = dequeue(csb->tokenQ);
        tos->next = NULL;
	chunk_free(parsenode_chunks, tos);
    }

    while (! QUEUEEMPTY(csb->tokenQ)) {
	/* Make sure we don't monopolize the CPU */
	process_may_suspend();

	/*
	 * If we got a hotstall interrupt, wait until the process level
	 * hotswap code gets to run and corrects our view of reality
	 * before continuing
	 */
	while (hotstall_flag)
	    process_suspend();

	tos = dequeue(csb->tokenQ);
	if (tos) {
	    if (tos->node) {
		csb->line_index = tos->line_index;
		SETOBJ(idb,1) = tos->idb1;
		SETOBJ(cdb,1) = tos->cdb1;
		SETOBJ(int,1) = tos->val1;
		SETOBJ(pdb,1) = tos->pdb1;
		SETOBJ(udb,1) = tos->udb1;
		SETOBJ(udb,2) = tos->udb2;
		SETOBJ(routemap,1) = tos->routemap1;
		csb->pmp = tos->pmp;
                csb->appn_object_verb = tos->appn_object_verb;
                csb->appn_object_type = tos->appn_object_type;
		strcpy(csb->visible_ambig.ambig, tos->visible_ambig);
		strcpy(csb->hidden_ambig.ambig, tos->hidden_ambig);
		csb->command_visible = tos->command_visible;
		csb->unsupported = tos->unsupported;
		csb->sense = tos->sense;
		csb->set_to_default = tos->set_to_default;
		if (csb->nvgen || (csb->flags & COMMAND_AUTHORIZE)) {
		    strcpy(csb->nv_command, tos->nv_command);
		}
		if (tos->node->func) {
		    tos->node->func(csb, tos->node, tos->node->args);
		}
	    }
	    tos->next = NULL;
	    chunk_free(parsenode_chunks, tos);
	}
    }
}

static const char *generic_csb_name = "Parser CSB";

parseinfo *get_csb (const char *name)
{
    parseinfo *csb;

    if (parseinfo_chunks == NULL)
	parseinfo_chunks = chunk_create(sizeof(parseinfo), 4,
					CHUNK_FLAGS_DYNAMIC,
					NULL, 0,
					"Parseinfo Blocks");
    if (!name) {
	name = generic_csb_name;
    }
    csb = chunk_malloc(parseinfo_chunks);
    if (!csb) {
	return(NULL);
    }

    /*
     * Assignments to make purify happy
     */
    csb->tokenQ = NULL;
    csb->help_save.buf = NULL;
    csb->comment_save.buf = NULL;

    /* Initialize the parsing stuff */
    csb->line = csb->line_buffer;
    csb->break_chars = EXEC_BREAKSET;
    csb->resolvemethod = RES_NULL;
    csb->flags = 0;
    csb->mode = exec_mode;
    csb->priv = CUR_PRIV(stdio->privilege);
    csb->priv_set = PRIV_NULL;

    /* Initialize tokenQ */	
    if ((csb->tokenQ = malloc(sizeof(queuetype))) == NULL) {
	free_csb(&csb);
	return(NULL);
    }
    queue_init(csb->tokenQ, 0);

    return(csb);
}

static boolean do_cmd_author (parseinfo *csb)
{
    int avail = 0, priv;
    authorstruct author;
    char *line, *ptr, *ptr2;
    boolean ret, cr = FALSE;
    
    if (old_access_control)
	return(TRUE);

    priv = csb->last_priv;		/* priv of last keyword */
    if (!aaa_author_needed(AAA_AUTHOR_TYPE_CMD, priv, NULL, stdio)) {
	if (priv == csb->highest_priv)
	    return(TRUE);
	priv = csb->highest_priv;	/* highest priv of all keywords */
	if (!aaa_author_needed(AAA_AUTHOR_TYPE_CMD, priv, NULL, stdio)) {
	    return(TRUE);
	}
    }

    /* Got here so Authorization must be required. */

    memset(&author, 0, sizeof(authorstruct));
    author.auth_idb = stdio->tty_idb;
    author.auth_tty = stdio;
    ret = TRUE;
    line = strdup(csb->nv_command);
    ptr2 = line;
    for (ptr = ptr2; (*ptr && (*ptr != ' ') && (*ptr != '\t')); ptr++);
    if (*ptr)
	while ((*ptr == ' ') || (*ptr == '\t'))
	    *ptr++ = '\0';
    if (!ptr2 || !aaa_add_AV(&author.out_args, &avail, AAA_ATTR_service,
			     AAA_VAL_SVC_shell, AAA_AV_MAND) ||
	!aaa_add_AV(&author.out_args, &avail, AAA_ATTR_cmd, ptr2, AAA_AV_MAND))
            ret = FALSE;
    while (ret && *ptr) {
	for (ptr2 = ptr; (*ptr && (*ptr != ' ') && (*ptr != '\t')); ptr++);
	if (*ptr)
	    while ((*ptr == ' ') || (*ptr == '\t'))
		*ptr++ = '\0';
	else
	    if (!strcmp(ptr2, parser_eol_string))
		cr = TRUE;
	if (!cr)
	    ret = aaa_add_AV(&author.out_args, &avail, AAA_ATTR_cmd_arg, ptr2,
			     AAA_AV_MAND);
    }
    free(line);

    if (ret) {
	ret = aaa_do_author(stdio->ustruct, &author, AAA_AUTHOR_TYPE_CMD,
			    priv);
	switch(author.status) {
	case AAA_AUTHOR_STATUS_ERROR:
	case AAA_AUTHOR_STATUS_FAIL:
	    ret = FALSE;
	    break;
	case AAA_AUTHOR_STATUS_PASS_ADD:
	    if (!author.in_args)
		break;
	    aaa_merge_attrs(author.out_args, author.in_args, &author.in_args);
	    /* FALLTHRU */
	case AAA_AUTHOR_STATUS_PASS_REPL:
	    if (aaa_author_debug)
		buginf("\nAAA/AUTHOR/CMD Cannot replace commands");
	    setstring(&author.server_msg, NULL);
	    ret = FALSE;
	    break;
	}
    }
    if (author.server_msg)
	printf("%s\n", author.server_msg);
    if (!ret && !author.server_msg) {
	printf("Command authorization failed.\n");
    }
    aaa_cleanup_author(&author);
    return(ret);
}

/*
 * Initialize parser variables
 */
static void parse_cmd_init (parseinfo *csb)
{

    csb->nv_command[0] = '\0';

    csb->error_index = 0;
    csb->nvgen = 0;
    csb->sense = TRUE;
    csb->set_to_default = FALSE;
    csb->saved_help_used = 0;
    save_line_init(&csb->help_save);
    csb->visible_ambig.ambig[0] = '\0';
    csb->visible_ambig.ambig_save[0] = '\0';
    csb->hidden_ambig.ambig[0] = '\0';
    csb->hidden_ambig.ambig_save[0] = '\0';
    set_multiple_funcs(csb, 0);
    csb->command_visible = TRUE;
    csb->unsupported = FALSE;
    csb->action_func = NULL;
    csb->which = 0;
    csb->func = 0;

    /* Clear any lingering flags */
    csb->flags &= (CONFIG_TERM | CONFIG_HTTP | COMMAND_AUTHORIZE |
		   COMMAND_FUNCTION | HIDE_PASSWORDS | PARSE_EMPTY_STRING);

    /* Zap the data area. */

    bzero((void *) &(csb->csb_start_data), 
	  (int) ((uchar *) &csb->csb_end_data - 
		 (uchar *) &csb->csb_start_data));

    csb->last_priv = 0;
    csb->highest_priv = 0;

    /* Set lines for "terminal" commands */
    if (!csb->permanent && !csb->nvgen) {
	csb->line1 = stdio->ttynum;
	csb->line2 = csb->line1 + 1;
    }
}

/*
 * parse_checklevels
 * check previous parser levels to see if there is a command match
 * "up there" somewhere.  This used to be part of parse_cmd(), but
 * I moved it out to save stack space when it's not used.
 */

static void parse_checklevels (parseinfo *csb, int save_line_index)
{
#define	MAX_MODE_STACK	5
    typedef struct parser_mode_stack_ {
	parser_mode *mode;
	void *var;
	boolean leave_subcommand;
	uint error_index;
    } parser_mode_stack;

    /*
     * If the parse at the secondary level fails, we save the secondary state
     * and reset the system for parsing the at the primary level.  If the
     * primary parse also fails, we report the error for the best match.
     * The state is set for the best match (i.e. longest error_index).
     * This prevents an error in an interface command from allowing
     * the broken command's subcommands to mistakenly operate on
     * a previous interface.  Similarly for other commands which have
     * subcommands.
     *
     * We save (and clear) the IDB pointer so that we can get out of
     * subinterface state.  Ditto the CDB pointer.
     */

    parser_mode_stack mstack[MAX_MODE_STACK];
    parser_mode *alt_mode;
    int i = 0;
    
    alt_mode = get_alt_mode(csb->mode);
    while (alt_mode) {
	
	if (i < MAX_MODE_STACK) {
	    mstack[i].mode = csb->mode;
	    mstack[i].var = mode_save_vars(csb);
	    mstack[i].leave_subcommand = csb->leave_subcommand;
	    mstack[i].error_index = csb->error_index;
	    i++;
	}
	
	csb->mode = alt_mode;
	set_multiple_funcs(csb, 0);
	csb->error_index = 0;
	csb->line_index = save_line_index;
	csb->sense = TRUE;
	csb->set_to_default = FALSE;
	zero_csb_strings(csb);
	csb->leave_subcommand = FALSE;
	
	push_node(csb, get_mode_top(csb->mode));
	parse_token(csb);
	
	if (test_multiple_funcs(csb, '=', 0) || (csb->next == NULL)) {
	    alt_mode = get_alt_mode(csb->mode);
	} else {
	    alt_mode = NULL;
	}
    }
    
    if (test_multiple_funcs(csb, '=', 0) || (csb->next == NULL)) {
	while ((i > 0) && (!csb->leave_subcommand) ) {
	    i--;
	    csb->error_index = mstack[i].error_index;
	    csb->leave_subcommand = mstack[i].leave_subcommand;
	    csb->mode = mstack[i].mode;
	    mode_reset_vars(csb, mstack[i].var);
	}
    }
}

/*
 * Parse a command line.
 */
static inline
int parse_cmd_internal (parseinfo *csb)
{
    int error = PARSE_ERROR_NOERR;	/* 0=ERROR_NOERR; ERROR_AMBIG;
					 * ERROR_NOMATCH; ERROR_UNKNOWN;
					 * ERROR_ALIAS;
					 */
    int save_line_index;
    boolean first_func;
    static boolean first_time_through = TRUE;
    boolean alias_was_expanded = FALSE;

    if (first_time_through) {
	resolve_parse_chains();
	parser_modes_init();
	first_time_through = FALSE;
	parsenode_chunks = chunk_create(sizeof(parsenode), 21,
					CHUNK_FLAGS_DYNAMIC |
					CHUNK_FLAGS_UNZEROED,
					NULL, 0,
					"Parse Nodes");
    }

    parse_cmd_init(csb);

    /*
     * Check for empty lines.
     * Strip leading whitespace.
     * Save comments if in config mode.
     */
    for (csb->line_index = 0; csb->line[csb->line_index] == ' ' ||
               csb->line[csb->line_index] == '\t'; csb->line_index++)
        ; /* Null body */

    /* Ignore empty lines which aren't help requests */
    if ((csb->line[csb->line_index] == '\0') &&
	!csb->in_help &&
	!(csb->flags & PARSE_EMPTY_STRING)) {
	*csb->line = '\0';
	return(PARSE_ERROR_NOERR);
    }
    if ((csb->line[csb->line_index] == '!' ||
         csb->line[csb->line_index] == '#' ||
         csb->line[csb->line_index] == ';')) {
	/* XXX Need a better algorithm for saving comments. */
	save_line(csb, &csb->comment_save, "%s\n", csb->line);
	*csb->line = '\0';
	return(PARSE_ERROR_NOERR);	/* comments are always successful */
    }

    if (expand_alias(csb)) {
	alias_was_expanded = TRUE;
    }
    save_line_index = csb->line_index;

    push_node(csb, get_mode_top(csb->mode));
    parse_token(csb);

    /*
     * If "CR" is the only option, then turn off help processing
     * and re-parse the command.
     */
    if ((csb->flags & CONFIG_HTTP) &&
	(csb->lastchar == '?') &&
	csb->help_save.used &&
	strstr(csb->help_save.buf, ">CR<")) {
	char *href = strstr(csb->help_save.buf, "HREF");

	if (href && !strstr(csb->help_save.buf, "FORM")) {
	    href += 4;		/* Skip past "HREF" */
	    if (!strstr(href, "HREF")) {
		/*
		 * CR is the only help option, so strip long help
		 * space from end and automagically accept command
		 */
		char *cp;

		cp = csb->line;
		while (*cp) {
		    cp++;
		}
		if (cp > csb->line) {
		    cp--;
		    if (*cp == ' ') {
			*cp = '\0';
			csb->lastchar = *(cp-1);
		    } else {
			csb->lastchar = '\0';
		    }
		}

		parse_cmd_init(csb);
		csb->in_help = PARSER_NO_HELP;
		csb->line_index = save_line_index;
		push_node(csb, get_mode_top(csb->mode));
		parse_token(csb);
	    }
	}
    }

    /*
     * If the parse at the secondary level fails, we save the secondary state
     * and reset the system for parsing the at the primary level.
     */
    if ((test_multiple_funcs(csb, '=', 0) || (csb->next == NULL))
	&& (!csb->in_help)
	&& (test_mode_config_subcommand(csb))) {
	parse_checklevels(csb, save_line_index);
    }

    /* If setting privilege levels on commands, csb->priv_set will
     * be non-zero, and csb->multiple_funcs will be > 1.  This is
     * not an error.
     */
    if (csb->priv_set) {
        return(PARSE_ERROR_NOERR);
    }

    /* Doing help processing.
     * If there was an ambiguous command match,
     * print the ambiguous message, otherwise print the help message.
     * If no help was provided, then tell the user that the command
     * was unrecognized.
     */
    if (csb->in_help) {
	/*
	 *  If the number of visible matches is > 1, or
	 *  the number of visible matches == 0 and the
	 *  number of invisible matches > 1, then the
	 *  command is ambiguous
	 */
	if (test_visible_multiple_funcs(csb, '>', 1) ||
	    (test_visible_multiple_funcs(csb, '=', 0) &&
	     test_invisible_multiple_funcs(csb, '>', 1))) {
	    error = PARSE_ERROR_AMBIG;
	} else {

	    /* There are two choices here:
	    * 1. The user asked for help.
	    * 2. The user requested keyword completion.
	    */
	    if (csb->lastchar == '?') {
		if (csb->help_save.used > 0) {
		    print_buf(csb->help_save, FALSE,
			      !(csb->flags & CONFIG_HTTP));
		} else {
		    error = PARSE_ERROR_UNKNOWN;
		}
	    } else {	/* csb->lastchar == '\t' */
		if ((word_completion(csb) == FALSE) && (!system_loading)) {
		    printf("\007");
		}
	    }
            csb->help_save.used = 0;
	    csb->visible_ambig.ambig[0] = '\0';
	    csb->visible_ambig.ambig_save[0] = '\0';
	    csb->hidden_ambig.ambig[0] = '\0';
	    csb->hidden_ambig.ambig_save[0] = '\0';
	}
    } else {
	/* If not doing help processing, then check whether the
	 * parse uniquely identified a command.  If not, report
	 * the error (if no command was matched) or report that
	 * the command was ambiguous (if more than one command matched).
	 */
	
        /* No commands matched.  Report the error */
        if (test_multiple_funcs(csb, '=', 0) || (csb->next == NULL)) {
	    if (alias_was_expanded) {
		error = PARSE_ERROR_NOMATCH | PARSE_ERROR_ALIAS;
	    } else {
		error = PARSE_ERROR_NOMATCH;
	    }
        }

        /*
	 *  If the number of visible matches is > 1, or
	 *  the number of visible matches == 0 and the
	 *  number of invisible matches > 1, then the
	 *  command is ambiguous.
	 *
	 * Multiple functions were identified.  This was an
         * ambiguous command.  Report the error.
         */
	if (test_visible_multiple_funcs(csb, '>', 1)) {
	    error = PARSE_ERROR_AMBIG;
	} else {
	    if ((test_visible_multiple_funcs(csb, '=', 0) &&
		 test_invisible_multiple_funcs(csb, '>', 1))) {
	        if ((csb->line[csb->error_index] == '\0') &&
		    (csb->error_index > 0) && !csb->next) {
		    error = PARSE_ERROR_NOMATCH;
	        } else {
	            error = PARSE_ERROR_AMBIG;
		}
	    }
	}
        /* If not doing help processing and one function selected, 
         * then we've successfully parsed a command.  Restore its
         * state and call the command function.
         */
	if (test_visible_multiple_funcs(csb, '=', 1) ||
	    (test_visible_multiple_funcs(csb, '=', 0) &&
	     test_invisible_multiple_funcs(csb, '=', 1))) {
	    boolean do_hidden_funcs = TRUE;

	    /*
	     * If test_visible_multiple_funcs == 1, then
	     * don't execute hidden functions
	     */

	    if (test_visible_multiple_funcs(csb, '=', 1)) {
		do_hidden_funcs = FALSE;
	    }

            /* A single function was selected, 
             * restore its state and call it.
             */
	    if (csb->createidb) {
		first_func = TRUE;
	    } else {
		first_func = FALSE;
	    }

            while (csb->next != NULL) {
		parseinfo *thiscsb;

		/* Don't free the csb itself.  Must copy its contents
		 * since csb was passed to us and the caller will need to
		 * continue to use this particular structure.
		 */
		thiscsb = csb->next;
		*csb = *csb->next;	/* Struct copy */
		chunk_free(parseinfo_chunks, thiscsb);
		thiscsb = NULL;

		if (csb->command_visible ||
		    (!csb->command_visible && do_hidden_funcs)) {

		    if (csb->flags & COMMAND_FUNCTION) {
			if (csb->command_function) {
			    csb->command_function(csb);
			}
		    } else {
			/*
			 * Command isn't supported yet
			 */
			if (csb->unsupported && !csb->nvgen &&
			    !system_loading && !bootstrap_enable &&
			    (csb->flags & CONFIG_TERM)) {
			    printf("\n%% This command is an unreleased "
				   "and unsupported feature");
			}

			if ((!(csb->flags & COMMAND_AUTHORIZE)) ||
			    do_cmd_author(csb)) {
			    if (first_func) {
				first_func = FALSE;
				*CSBVAR(csb, csb->createidb_var, idbtype *) =
				    csb->createidb(csb->createidb_unit);
			    }

			    reg_invoke_aaa_acct_cmd(stdio->ustruct, csb);

			    /*
			     * Call the command action function
			     */
			    if (csb->action_func) {
				csb->action_func(csb);
				reg_invoke_config_changed(csb);

				if (get_interrupt_level()) {
				    errmsg(&msgsym(INTDISABLE, PARSER),
					   get_mode_name(csb->mode),
					   csb->line);
				    crashdump(10);
				}
			    }
			} else {
			    /* Authorization failed */
			}
		    }
		}
            }
	    if (csb->flags & CONFIG_HTTP) {
		const char *save_name = get_mode_name(csb->mode);
		if ((csb->mode == config_mode) ||
				test_mode_config_subcommand(csb)) {
		    exit_config_command(csb);
		}
		csb->http_exited_name = save_name;
	    }
        }
    }

    /* Free any lingering saved parseinfo structures, if needed. */
    {
      parseinfo *thiscsb = csb->next;
      while (thiscsb != NULL) {
	parseinfo *nextpd = thiscsb->next;
	chunk_free(parseinfo_chunks, thiscsb);
	thiscsb = nextpd;
      }
      csb->next = NULL;
    }

    /* Reset prompt_lead if needed.  This has to be done here because
     * a primary config command may cause the parser to switch from
     * a subcommand set.  Only here do we know that this switch has
     * occurred and only by examining the top of the active parse tree.
     * This also has to be done for handling configuration from
     * memory or net so that we aren't left with the prompt
     * indicating the wrong command level.
     */
    if (csb->mode == exec_mode) {
	csb->permanent = FALSE;
	/* Clear all flags except CONFIG_TERM and KEYWORD_MATCHED */
	csb->flags &= (CONFIG_TERM | KEYWORD_MATCHED);
    }
    /* Reset variables which are not active. */
    if (csb->mode == config_mode) {
	csb->protocol = NULL;
	csb->line1 = csb->line2 = 0;
	csb->interface = NULL;
	csb->prototype = 0;
	csb->maplist = NULL;
	csb->mapclass = NULL;
	csb->controller = NULL;
    }

    /* The only errors that we can report are parse errors.  The commands 
     * are expected to report their internal errors.
     */
    return(error);
}

/*
 * Wrapper to lock the config while the command is 
 * being parsed - but only for config commands.
 * and only for actual configuration.
 * nv_gen asserts a read lock in nv_current_common.
 */
int parse_cmd (parseinfo *csb)
{
    int result;
    result = parse_cmd_internal(csb);
    return(result);
}

/*
 * get_parsenode
 * Get parsenode structure.
 */
static parsenode *get_parsenode (parseinfo *csb)
{
    parsenode *pp = NULL;

    pp = chunk_malloc(parsenode_chunks);
    if (!pp)
	return(NULL);
    pp->next = NULL;
    pp->visible_ambig[0] = '\0';
    pp->hidden_ambig[0] = '\0';
    pp->nv_command[0] = '\0';
    return(pp);
}

static inline void empty_queue (queuetype *qp)
{
    void *tmp;

    while (! QUEUEEMPTY(qp)) {
	tmp = dequeue(qp);
	free(tmp);
    }
}

static void free_queue (queuetype **qp)
{
    if (*qp) {
	empty_queue(*qp);
	free(*qp);
	*qp = NULL;
    }
}

/*
 * free_csb_mem
 *
 * Free the memory allocated for the various structures within
 * the csb. This includes dirty, token and line Qs and help_save,
 * comment_save fields.
 */
void free_csb_mem (parseinfo *csb)
{
    if (csb) {
	free_queue(&csb->tokenQ);
	free_queue(&csb->privQ);
	free_queue(&csb->priv_nvgenQ);
	save_line_init(&csb->help_save);  /* Free help text elements */
	save_line_init(&csb->comment_save); /* Free comment text elements */
    }
}

/* Mind our Free's & Q's */	
void free_csb (parseinfo **csbp)
{
    parseinfo *csb = *csbp;

    if (csb) {
	free_csb_mem(csb);   /* Free allocated memory hidden in csb */
	chunk_free(parseinfo_chunks, csb);
    }
    *csbp = NULL;
}

static void zero_csb_strings (parseinfo *csb)
{
    csb->nv_command[0] = 0;
    SETOBJ(string,1)[0] = 0;
    SETOBJ(string,2)[0] = 0;
    SETOBJ(string,3)[0] = 0;
}



static int count_help_matches (parseinfo *csb)
{
    char *cp;
    int n = 0;

    for (cp = csb->help_save.buf; cp && *cp; cp++) {
	if (*cp == '\t' || *cp == '\n'){
	    n++;
	}
    }
    return(n);
}

#define BEL "\007"

/*
 * word_completion
 *
 * Complete a keyword by comparing the current input line and the
 * contents of the help buffer.  If there is only one match in the help
 * buffer, then verify that the keyword and the match are the same.
 */
static boolean word_completion (parseinfo *csb)
{
    char *cp, *sp;
    char *hp, *jp, *ojp, *kp;
    int help_matches;

    if ((help_matches = count_help_matches(csb)) == 0) {
	/* No matches, seeya */
	return(FALSE);
    }

    /* Find the start of the last word in the input */
    for (cp = csb->line; *cp; cp++)
	; /* Null body */ 
    while (cp != csb->line && *(cp-1) != ' ') {
	cp--;
    }

    /* Find first help match */
    for (hp = csb->help_save.buf;
	 hp && *hp && *hp != '\t' && *hp != '\n';
	 hp++) {
	;	/* Null body */
    }
    /* See if we can find anything shorter */
    jp = hp;
    while (jp && *jp) {
	/* move past white space to next help */
	for (; jp && (*jp == '\t' || *jp == '\n'); jp++) {
	    ;	/* Null body */
	}
	if (!jp || !*jp) {
	    /* end of help */
	    continue;
	}
	/* save pointer to beginning of current help */
	ojp = jp;
	/* find length of match */
	for (kp = csb->help_save.buf;
	     jp && *jp && *jp != '\t' && *jp != '\n' &&
		 (tolower(*kp) == tolower(*jp));
	     jp++, kp++) {
	    ;	/* Null body */
	}
	/* See if matched length is shorter */
	if ((hp - csb->help_save.buf) > (jp - ojp)) {
	    hp = csb->help_save.buf + (jp - ojp);
	}
	/* Move to next white space */
	for (; jp && *jp && *jp != '\t' && *jp != '\n'; jp++) {
	    ;	/* Null body */
	}
    }

    /* move to end of last word in the input */
    for (sp = csb->help_save.buf;
	 *cp && *sp && (sp != hp) && (*sp != '\t') && (*sp != '\n');
	 cp++,sp++) {
	/* Check that the input and the matched word are the same. */
	if (tolower(*cp) != tolower(*sp)) {
	    return(FALSE);
	}
    }
    /* We may have been attempting a match at the end of a line.
     * In this case, we could copy an invalid help string onto the
     * end of the current line (e.g. adding "<cr>" to the line.)
     *
     * This proccedure of using the help mechanism is totally wrong.
     * Instead, a separate flag should be used for command completion
     * and only the keyword action routine should add text to the help
     *  buffer.  The other action routines should not add anything to
     * the help buffer in this case.
     *
     * Until the above gets done, do not add help text which begins with
     * a blank.
     */
    if (*sp == ' ') {
	return(FALSE);
    }

    while (*sp && (sp != hp) && (*sp != '\t') && (*sp != '\n')) {
	/* copy the rest of the matched word into the input buffer */
	*cp++ = *sp++;
    }
    if (help_matches != 1) {
	*cp = '\0';
	return(FALSE);
    }
    *cp++ = ' ';    /* Add trailing space */
    *cp = '\0';
    return(TRUE);
}

/*
 * Report where we found an error in the command line.
 * "error" indicates what type of error occured.
 * If 'reprint' is TRUE, then we have to reprint the line.
 */
void report_error (parseinfo * csb, int error, boolean need_reprint)
{
    int promptlen;
    int incomplete = 0;

    if (system_loading) {
	return;
    }

    /*
     * If we're in a bootstrap image, be silent about what we do and don't
     * understand, unless specifically configured from the console.
     */

    if (bootstrap_enable && !(csb->flags & CONFIG_TERM)) {
	return;
    }

    if (parser_ambig_debug) {
	need_reprint = TRUE;
    }

    if (error == PARSE_ERROR_UNKNOWN) {
	printf("\n%% Unrecognized command");
	return;
    }

    if (error == PARSE_ERROR_AMBIG) {
	printf("\n%% Ambiguous command:  \"%s\"", csb->line);
	return;
    }

    if (error == PARSE_ERROR_LOCKED) {
	/* 	
	 * Configuration is still locked by anther process.
	 * lock_config spits out an error message.
	 */
	return;
    }

    /* error == PARSE_ERROR_NOMATCH */
    /*
     * Prepare to designate the error.  We have to know the length of the
     * command line prompt and if configuration is from other than a
     * terminal, print the command before indicating the error location.
     */
    if (csb->flags & CONFIG_TERM) {
	promptlen = strlen(csb->prompt);
	/* Reprint if the command is too long. */
	if (promptlen + strlen(csb->line) >= SCREEN_WIDTH ||
	    need_reprint ||
	    (error & PARSE_ERROR_ALIAS)) {
	    promptlen = 0;
	    need_reprint = TRUE;
	}
    } else {
	promptlen = 0;
	need_reprint = TRUE;
    }

    if (csb->line[csb->error_index] == '\0') {
	if (csb->error_index > 0) {
	    /* NON-blank input line. Handle INCOMPLETE error similarly
	     to other errors, below. */
	    incomplete = 1;
	}
    }
    if (need_reprint) {
	int length = csb->error_index;
	int i;
	int base;

	printc('\n');

	if (SCREEN_WIDTH <= 0) {
	    base = length;
	} else {
	    base = length / SCREEN_WIDTH;
	}
	for (i = 0; csb->line[i]; i++) {
	    printc(csb->line[i]);
	    if ((SCREEN_WIDTH > 0) && ((i+1) % SCREEN_WIDTH == 0)) {
		/* Next char starts new line. */
		if (i / SCREEN_WIDTH == base) {
		    /* We just printed the error line. */
		    print_caret(length % SCREEN_WIDTH);
		} else {
		    /* Not the error line, print a blank line. */
		    printf("\n");
		}
	    }
	}

	if (! incomplete) {
	    printc('\n');

	    if (SCREEN_WIDTH <= 0) {
		print_caret(length);
	    } else {
		if (i / SCREEN_WIDTH == base) {
		    /* We just printed the error line. */
		    print_caret(length % SCREEN_WIDTH);
		}
	    }
	}
    } else {
	if (! incomplete) {
	    /* No reprint needed, just show caret. */
	    print_caret(promptlen + csb->error_index);
	}
    }

    if (incomplete) {
	printf("\n%% Incomplete command.\n");
    } else {
	printf("\n%% Invalid input detected at '^' marker.\n");
    }
}

/*
 * exit_exec_command
 *
 * Cleanup for an exit from the exec.  Call exit_command() to
 * drop any outstanding connections and set the exit flag.
 */
void exit_exec_command (parseinfo *csb)
{
    if (exit_command(TRUE)) {
	csb->exit_all = TRUE;
    }
}

void exit_config_submode (parseinfo *csb)
{
    csb->mode = config_mode;
    csb->interface = NULL;
    csb->protocol = NULL;
    csb->line1 = csb->line2 = 0;
    csb->prototype = 0;
    csb->maplist = NULL;
    csb->mapclass = NULL;
    csb->controller = NULL;
}

/*
 * exit any configuration mode session block might be in.
 */
void exit_config_modes (parseinfo *csb)
{
    if ((csb->mode == config_mode) || test_mode_config_subcommand(csb)) {
        exit_config_command(csb);
    }
}

/*
 * 
 *
 */
boolean parse_line (void *pointer, transition *top, uint len,
		    const char *prompt_fmt, ...)
{
    va_list prompt_args;
    boolean ret = FALSE;
    parser_mode *mode;

    mode = get_mode_tmp(NULL, NULL, NULL, FALSE, FALSE, top);
    if (mode) {
	va_start(prompt_args, prompt_fmt);
	ret = parse_line_mode_args(pointer, mode, len,
				   prompt_fmt, prompt_args);
	va_end(prompt_args);
    }
    free_mode(mode);
    return(ret);
}

boolean parse_line_mode (void *pointer, parser_mode *mode, uint len,
			 const char *prompt_fmt, ...)
{
    va_list prompt_args;
    boolean ret;

    va_start(prompt_args, prompt_fmt);
    ret = parse_line_mode_args(pointer, mode, len,
			       prompt_fmt, prompt_args);
    va_end(prompt_args);
    return(ret);
}

static boolean parse_line_mode_args (void *pointer, parser_mode *mode,
				     uint len, const char *prompt_fmt,
				     va_list prompt_args)
{
    int error;
    parseinfo *csb;
    int running;
    int retval;
    tt_soc *tty = stdio;

    csb = get_csb("Parse_Line Parser CSB");
    if (!csb) {
	return(FALSE);
    }

    csb->priv = CUR_PRIV(stdio->privilege);
    csb->resolvemethod = RES_MANUAL;
    csb->mode = mode;
    csb->permanent = FALSE;
    csb->flags |= PARSE_EMPTY_STRING;

    /*
     *  ... and then some magic happens ... 
     *
     * Behind the curtain...
     * 'pointer' is a pointer to an opaque data structure which
     * the caller hands us.  The action routine called at the end
     * of the parse needs this structure, so we store it in
     * the csb where it can be found by the action routine.
     * This is pretty gross, but otherwise we'd have to add
     * yet another variable to the csb, just for this one case.
     */
    csb->parse_line_pointer = pointer;

    running = TRUE;
    retval = TRUE;
    while (running) {
	if (readline_args(csb->line, len, "\r\n?", &csb->lastchar,
			  TRUE, prompt_fmt, prompt_args)) {

	    if (csb->lastchar == '?') {
		csb->in_help = PARSER_HELP;
	    } else {
		if (csb->lastchar == '\t') {
		    csb->in_help = PARSER_COMMAND_COMPLETION;
		} else {
		    csb->in_help = PARSER_NO_HELP;
		}
	    }

	    if ((error = parse_cmd(csb)) != PARSE_ERROR_NOERR) {
		/* Report the error, but don't require line re-print */
		report_error(csb, error, FALSE);
	    }

	} else {
	    running = FALSE;
	    retval = FALSE;
	    continue;
	}
	if (csb->exit_all) {
	    running = FALSE;
	    continue;
	}
	if (tty->statbits & IDLETIMEOUT) {
	    /* The current session timed out.  If there are no more
	     * connections, exit.  Otherwise simulate an empty input line
	     * so that we will resume the open connection.
	     */
	    *csb->line = '\0';
	    csb->lastchar = '\0';
	    if (tty->currentcon == NULL) {
		exit_command(FALSE);
		csb->exit_all = TRUE;
		running = FALSE;
		continue;
	    }
	}
	/* Clear the input if we were not doing help or command completion */
	if (csb->in_help == PARSER_NO_HELP) {
	    csb->line[0] = '\0';
	}
	csb->lastchar = '\0';
    }   /* while (running) */


    free_csb(&csb);
    return(retval);
}

void set_multiple_funcs (parseinfo *csb, int value)
{
    csb->visible_ambig.multiple_funcs = value;
    csb->hidden_ambig.multiple_funcs = value;
}

void increment_multiple_funcs (parseinfo *csb, char *str)
{
    char *visible_str;
    int value;

    if (csb->command_visible) {
	csb->visible_ambig.multiple_funcs++;
	visible_str = "visible";
	value = csb->visible_ambig.multiple_funcs;
    } else {
	csb->hidden_ambig.multiple_funcs++;
	visible_str = "hidden";
	value = csb->hidden_ambig.multiple_funcs;
    }
    if (parser_help_debug || parser_ambig_debug) {
	buginf("\nIncreasing %s multiple_funcs to %d in %s",
	       visible_str, value, str);
    }
	
}

int test_multiple_funcs (parseinfo *csb, char test, int value)
{
    int mf = (csb->visible_ambig.multiple_funcs +
	      csb->hidden_ambig.multiple_funcs);

    switch (test) {
      case '<': return(mf < value);
      case '>': return(mf > value);
      case '=': return(mf == value);
      default:
	errmsg(&msgsym(MULFUNCS, PARSER), test);
	break;
    }
    return(0);
}

int test_visible_multiple_funcs (parseinfo *csb, char test, int value)
{
    int mf = csb->visible_ambig.multiple_funcs;

    switch (test) {
      case '<': return(mf < value);
      case '>': return(mf > value);
      case '=': return(mf == value);
      default:
	errmsg(&msgsym(MULFUNCS, PARSER), test);
	break;
    }
    return(0);
}

int test_invisible_multiple_funcs (parseinfo *csb, char test, int value)
{
    int mf = csb->hidden_ambig.multiple_funcs;

    switch (test) {
      case '<': return(mf < value);
      case '>': return(mf > value);
      case '=': return(mf == value);
      default:
	printf("\n%% Unknown test in test_multiple_funcs '%c'", test);
	break;
    }
    return(0);
}

void print_multiple_funcs (parseinfo *csb)
{
    buginf("\ncsb->visible_ambig.multiple_funcs = %d",
	   csb->visible_ambig.multiple_funcs);
    buginf("\ncsb->hidden_ambig.multiple_funcs = %d",
	   csb->hidden_ambig.multiple_funcs);
}

static chain_link *get_link_point (uint which_chain)
{
    int i = 0;
    chain_link *cp;
    chain_list *lp;

    /* Check static link points */
    cp = &chain_gang[i++];
    while (cp && (cp->id != PARSE_LIST_END)) {
	if (cp->id == which_chain) {
	    return(cp);
	}
	cp = &chain_gang[i++];
    }
    /* Check dynamic link points */
    lp = ll;
    while (lp != NULL) {
	if (lp->cp.id == which_chain) {
	    return(&lp->cp);
	}
	lp = lp->next;
    }
    /* Didn't find anything */
    return(NULL);
}

#ifdef	PARSER_DEBUG_LINKS
/*
 * Get link point by name
 */
static chain_link *get_link_point_by_name (const char *name)
{
    int i = 0;
    chain_link *cp;
    chain_list *lp;

    /* Check static link points */
    cp = &chain_gang[i++];
    while (cp && cp->name != NULL) {
	if (strcmp(cp->name, name) == 0) {
	    return(cp);
	}
	cp = &chain_gang[i++];
    }
    /* Check dynamic link points */
    lp = ll;
    while (lp != NULL) {
	if (strcmp(lp->cp.name, name) == 0) {
	    return(&lp->cp);
	}
	lp = lp->next;
    }
    /* Didn't find anything */
    return(NULL);
}
#endif	/* PARSER_DEBUG_LINKS */

static boolean parser_add_chains (uint which_chain,
				  dynamic_transition *commands,
#ifdef	PARSER_DEBUG_LINKS
				  const char *module,
#endif	/* PARSER_DEBUG_LINKS */
				  boolean already_on_list)
{
    chain_link *where;

    /*
     * Make sure we have a place to link this
     */
    where = get_link_point(which_chain);
    if (!where) {
	if (already_on_list) {
	    /*
	     * point is already on unresolved list, so return FALSE
	     */
	    return(FALSE);
	} else {
	    /*
	     * No place to add it yet, so place on unresolved list
	     */
	    return(add_to_unresolved_chains(which_chain, commands
#ifdef	PARSER_DEBUG_LINKS
					    , module
#endif	/* PARSER_DEBUG_LINKS */
					    ));
	}
    }

    switch (where->type) {
      case PARSER_ENTRY_LINK_POINT:
	return(parser_resolve_entry_link_point(where, commands
#ifdef	PARSER_DEBUG_LINKS
					       , module
#endif	/* PARSER_DEBUG_LINKS */
					       ));
      case PARSER_EXIT_LINK_POINT:
	return(parser_resolve_exit_link_point(where, commands));
      default:
	return(FALSE);
    }
}
	
boolean parser_add_commands_func (uint which_chain,
				  dynamic_transition *commands
#ifdef	PARSER_DEBUG_LINKS
				  , const char *module
#endif	/* PARSER_DEBUG_LINKS */
				  )
{
    return(parser_add_chains(which_chain, commands,
#ifdef	PARSER_DEBUG_LINKS
			     module,
#endif	/* PARSER_DEBUG_LINKS */
			     FALSE));
}

static boolean parser_resolve_entry_link_point (chain_link *where,
						dynamic_transition *commands
#ifdef	PARSER_DEBUG_LINKS
						,const char *module
#endif	/* PARSER_DEBUG_LINKS */
						)
{
    dynamic_transition *tw;

    /*
     * Check to make sure the chain hasn't already been linked.
     */
    tw = where->link_point->accept;
    while (tw) {
	if (tw == commands) {
	    /*
	     * Parse chain already linked in, but it's not really
	     * a failure because the parse chain has been linked
	     * in, so we return TRUE.
	     */
	    return(TRUE);
	}
	if (tw->alternate == (dynamic_transition *)&pname(link_trans)) {
	    break;
	}
	tw = tw->alternate;
    }
    if (!tw || (tw->alternate != (dynamic_transition *)&pname(link_trans))) {
	errmsg(&msgsym(NOLINK, PARSER), 3,
#ifdef	PARSER_DEBUG_LINKS
	       module, where->name
#else	/* PARSER_DEBUG_LINKS */
	       NULL, NULL
#endif	/* PARSER_DEBUG_LINKS */
	       );
	return(FALSE);
    }

    if (commands) {
	/*
	 * At this point last_cmd's alternate points to link_point,
	 * so we have the right place
	 */
	commands->alternate = (dynamic_transition *)tw->alternate;
#ifdef	PARSER_DEBUG_LINKS
	commands->args = module;
#endif	/* PARSER_DEBUG_LINKS */
	tw->alternate = commands;
	return(TRUE);
    } else {
	errmsg(&msgsym(NOLINK, PARSER), 2,
#ifdef	PARSER_DEBUG_LINKS
	       module, where->name
#else	/* PARSER_DEBUG_LINKS */
	       NULL, NULL
#endif	/* PARSER_DEBUG_LINKS */
	       );
	return(FALSE);
    }
}

static boolean parser_resolve_exit_link_point (chain_link *where,
					       dynamic_transition *commands)
{
    if (!commands ||
	!where ||
	!commands->accept ||
	(commands->accept->alternate !=
	 (dynamic_transition *)&pname(link_trans))) {

	/*
	 * Transition is broken or already linked
	 */
	return(FALSE);
    }

    commands->accept->alternate = where->link_point;
    return(TRUE);
}

/*
 * parser_add_command_list - Add a series of parse chains
 */

boolean parser_add_command_list_func (parser_extension_request *chain
#ifdef	PARSER_DEBUG_LINKS
				      , const char *module
#endif	/* PARSER_DEBUG_LINKS */
				      )
{
    int i;

    for (i = 0; chain[i].type != PARSE_LIST_END; i++) {
	if (!parser_add_commands(chain[i].type, chain[i].commands, module)) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

static void protoaddr_add_set(link_name *, enum LINK, const char *,
			      const char *, const char *,
			      const uint, const uint);

link_name *link_names = NULL;
static uint link_names_length = 0;

#define	ADDR_GROW	 5
#define	ROUTER_GROW	 5

void parser_add_protoaddr (const enum LINK link_type, const char *codes,
			   const char *name, const char *help,
			   const uint min_match, const uint addr_type)
{
    int i;
    link_name *tmp;

    if (link_names == NULL) {
	/* First time, malloc link_names */
	if ((link_names = malloc(ADDR_GROW*sizeof(link_name))) == NULL) {
	    printf(nomemory);
	    return;
	}
	link_names_length = ADDR_GROW;
	protoaddr_add_set(&link_names[0], LINK_ILLEGAL, "",
			  NULL, NULL, 0, ADDR_ILLEGAL);
    }

    i = 0;
    while ((link_names[i].link_type != LINK_ILLEGAL) &&
	   (i < link_names_length)) {
	i++;
    }

    if (link_names[i].link_type == LINK_ILLEGAL) {
	/* found a spot */
	if ((i+1) >= link_names_length) {
	    if ((tmp = malloc((ADDR_GROW + link_names_length)*
			      sizeof(link_name))) == NULL) {
		printf(nomemory);
		return;
	    }
	    bcopy(link_names, tmp, link_names_length * sizeof(link_name));
	    free(link_names);
	    link_names = tmp;
	    link_names_length += ADDR_GROW;
	}
	protoaddr_add_set(&link_names[i], link_type, codes,
			  name, help, min_match, addr_type);
	protoaddr_add_set(&link_names[i+1], LINK_ILLEGAL, "",
			  NULL, NULL, 0, ADDR_ILLEGAL);
    } else {
	/* something's wrong */
	printf("\n%% Invalid end to protoaddr list, can't add protocol '%s'",
	       name);
    }
}

static void protoaddr_add_set (link_name *link, const enum LINK link_type,
			       const char *codes, const char *name,
			       const char *help, const uint min_match,
			       const uint addr_type)
{
    link->link_type = link_type;
    link->codes = codes;
    link->name = name;
    link->min_match = min_match;
    link->help = help;
    link->addr_type = addr_type;
}

addr_func *addr_funcs = NULL;
static uint addr_funcs_length = 0;

static void parser_add_address_set(addr_func *addr, const uint addr_type,
				   short_help_func get_short_help,
				   match_gen_func func);

void parser_add_address (const uint addr_type, short_help_func get_short_help,
			 match_gen_func func)
{
    int i;
    addr_func *tmp;

    if (addr_funcs == NULL) {
	if ((addr_funcs = malloc(ADDR_GROW*sizeof(addr_func))) == NULL) {
	    printf(nomemory);
	    return;
	}
	addr_funcs_length = ADDR_GROW;
	parser_add_address_set(&addr_funcs[0], ADDR_ILLEGAL, NULL, NULL);
    }

    i = 0;
    while ((addr_funcs[i].addr_type != ADDR_ILLEGAL) &&
	   (i < addr_funcs_length)) {
	i++;
    }

    if (addr_funcs[i].addr_type == ADDR_ILLEGAL) {
	if ((i+1) >= addr_funcs_length) {
	    if ((tmp = malloc((ADDR_GROW + addr_funcs_length) *
			      sizeof(addr_func))) == NULL) {
		printf(nomemory);
		return;
	    }
	    bcopy(addr_funcs, tmp, addr_funcs_length * sizeof(addr_func));
	    free(addr_funcs);
	    addr_funcs = tmp;
	    addr_funcs_length += ADDR_GROW;
	}
	parser_add_address_set(&addr_funcs[i], addr_type,
			       get_short_help, func);
	parser_add_address_set(&addr_funcs[i+1], ADDR_ILLEGAL, NULL, NULL);
    } else {
	/* something's wrong */
	printf("\n%% Invalid end to address list, can't add protocol %d",
	       addr_type);
    }
}

static void parser_add_address_set (addr_func *addr, const uint addr_type,
				    short_help_func get_short_help,
				    match_gen_func func)
{
    addr->addr_type = addr_type;
    addr->get_short_help = get_short_help;
    addr->func = func;
}

static void router_add_set (router_name *router, uint type,
			    void (*func)(parseinfo *, const char *,
					 char *, transition *));

router_name *router_names = NULL;
static int router_names_length = 0;

void parser_add_router (uint type, void (*func)(parseinfo *, const char *,
						char *, transition *))
{
    int i;
    router_name *tmp;

    if (router_names == NULL) {
	/* First time, malloc router_names */
	if ((router_names = malloc(ROUTER_GROW*sizeof(router_name))) == NULL) {
	    printf(nomemory);
	    return;
	}
	router_names_length = ROUTER_GROW;
	router_add_set(&router_names[0], RMATCH_NONE, NULL);
    }

    i = 0;
    while ((router_names[i].type != RMATCH_NONE) &&
	   (i < router_names_length)) {
	i++;
    }

    if (router_names[i].type == RMATCH_NONE) {
	/* found a spot */
	if ((i+1) >= router_names_length) {
	    /* Need room for more */
	    if ((tmp = malloc((ROUTER_GROW+router_names_length)*
			      sizeof(router_name))) == NULL) {
		printf(nomemory);
		return;
	    }
	    bcopy(router_names, tmp, router_names_length*sizeof(router_name));
	    free(router_names);
	    router_names = tmp;
	    router_names_length += ROUTER_GROW;
	}
	router_add_set(&router_names[i], type, func);
	router_add_set(&router_names[i+1], RMATCH_NONE, NULL);
    } else {
	/* something's wrong */
	printf("\n%% Invalid end to router name list, can't add router %d",
	       type);
    }
}

static void router_add_set (router_name *router, uint type,
			    void (*func)(parseinfo *, const char *,
					 char *, transition *))
{
    router->type = type;
    router->func = func;
}

static void resolve_parse_chains (void)
{
    chain_list *cp = cl;
    chain_list *tmp;
    chain_list *old = NULL;

#ifdef	LINK_DEBUG
    printf("\nResolving parse chains");
#endif	/* LINK_DEBUG */

    while (cp != NULL) {
	if (!parser_add_chains(cp->cp.id, cp->cp.link_point,
#ifdef	PARSER_DEBUG_LINKS
			       cp->cp.name,
#endif	/* PARSER_DEBUG_LINKS */
			       TRUE)) {
	    /*
	     * Failed again, leave on list and try later
	     */
#ifdef	LINK_DEBUG
	    printf("\n  Failed to resolve module %d '%s'",
		   cp->cp.id, cp->cp.name);
#endif	/* LINK_DEBUG */
	    old = cp;
	    cp = cp->next;
	} else {
	    /*
	     * Success, remove and free link
	     */
#ifdef	LINK_DEBUG
	    printf("\n  Resolved module %d '%s'", cp->cp.id, cp->cp.name);
#endif	/* LINK_DEBUG */
	    if (cp == cl) {
		cl = cl->next;
	    }
	    tmp = cp;
	    cp = cp->next;
	    if (old) {
		old->next = cp;
	    }
	    free(tmp);
	}
    }
}

static boolean add_to_unresolved_chains (uint which_chain,
					 dynamic_transition *commands
#ifdef	PARSER_DEBUG_LINKS
					 , const char *module
#endif	/* PARSER_DEBUG_LINKS */
					 )
{
    chain_list *cp;

    /*
     * Make sure commands isn't alreay on the list
     */
    cp = cl;
    while (cp != NULL) {
	if ((cp->cp.link_point == commands) &&
	    (cp->cp.id == which_chain)) {
#ifdef	LINK_DEBUG
	    printf("\n  Duplicate module %d '%s'", which_chain, module);
#endif	/* LINK_DEBUG */
	    /*
	     * It's a duplicate, but we return true because
	     * the link point has been successfully added.
	     */
	    return(TRUE);
	}
	cp = cp->next;
    }

    if ((cp = malloc(sizeof(chain_list))) == NULL) {
	printf(nomemory);
	return(FALSE);
    }

    cp->cp.id = which_chain;
#ifdef	PARSER_DEBUG_LINKS
    cp->cp.name = module;
#endif	/* PARSER_DEBUG_LINKS */
    cp->cp.link_point = commands;
    cp->next = cl;
    cl = cp;
#ifdef	LINK_DEBUG
    printf("\n  Unresolved module %d '%s'", cl->cp.id, cl->cp.name);
#endif	/* LINK_DEBUG */
    return(TRUE);
}

boolean parser_add_link_point_func (uint which_chain,
#ifdef	PARSER_DEBUG_LINKS
				    const char *module,
#endif	/* PARSER_DEBUG_LINKS */
				    transition *lp)
{
    dynamic_transition *dp = (dynamic_transition *)lp;

    return(parser_add_link_to_list(which_chain,
#ifdef	PARSER_DEBUG_LINKS
				   module,
#endif	/* PARSER_DEBUG_LINKS */
				   dp, PARSER_ENTRY_LINK_POINT));
}

boolean parser_add_link_exit_func (uint which_chain,
#ifdef	PARSER_DEBUG_LINKS
				   const char *module,
#endif	/* PARSER_DEBUG_LINKS */
				   transition *lp)
{
    dynamic_transition *dp = (dynamic_transition *)lp;

    return(parser_add_link_to_list(which_chain,
#ifdef	PARSER_DEBUG_LINKS
				   module,
#endif	/* PARSER_DEBUG_LINKS */
				   dp, PARSER_EXIT_LINK_POINT));
}

static boolean parser_add_link_to_list (uint which_chain,
#ifdef	PARSER_DEBUG_LINKS
					const char *module,
#endif	/* PARSER_DEBUG_LINKS */
					dynamic_transition *lp, uint type)
{
    chain_list *new_lp;

    if ((new_lp = malloc_named(sizeof(chain_list), "Parser Linkage")) == NULL) {
	printf(nomemory);
	return(FALSE);
    }

    new_lp->cp.id = which_chain;
#ifdef	PARSER_DEBUG_LINKS
    new_lp->cp.name = module;
#endif	/* PARSER_DEBUG_LINKS */
    new_lp->cp.link_point = lp;
    new_lp->cp.type = type;
    new_lp->next = ll;
    ll = new_lp;
#ifdef	LINK_DEBUG
    printf("\nAdding link point %d '%s'", which_chain, module);
#endif	/* LINK_DEBUG */
    return(TRUE);
}

/*
 * parser_link_point_exists
 * Return TRUE if someone has added the named entry link point.
 */

boolean parser_link_point_exists (uint which_chain)
{
    chain_list *lp;

    for (lp = ll; lp; lp = lp->next) {
	if ((which_chain == lp->cp.id) &&
	    (PARSER_ENTRY_LINK_POINT == lp->cp.type))
	    return(TRUE);
    }
    return(FALSE);
}
    
#ifdef	PARSER_DEBUG_LINKS
void show_parser_links (parseinfo *csb)
{
    chain_link *cp;
    dynamic_transition *tp;
    int i = 0;
    chain_list *lp;
    static const char *show_links_header =
	"\n  Name                             ID   Addr      Type";

    if (!GETOBJ(string,1)[0]) {
	printf("\nCurrent parser link points:");
	printf(show_links_header);

	cp = &chain_gang[i++];
	while (cp && cp->name != NULL) {
	    printf("\n  %30s %-4d   0x%6x %-2d",
		   cp->name, cp->id, cp->link_point, cp->type);
	    cp = &chain_gang[i++];
	}
	printf("\nAdded at run time:");
	printf(show_links_header);
	lp = ll;
	while (lp != NULL) {
	    printf("\n  %30s %-4d   0x%6x %-2d",
		   lp->cp.name, lp->cp.id, lp->cp.link_point, lp->cp.type);
	    lp = lp->next;
	}
	printf("\n");
	return;
    }	    

    cp = get_link_point_by_name(GETOBJ(string,1));
    if (!cp) {
	printf("\n%% Unknown parser link point '%s'", GETOBJ(string,1));
	return;
    }

    if (!cp->link_point ||
	!cp->link_point->accept ||
	cp->link_point->accept == (dynamic_transition *)&pname(link_trans)) {
	printf("\nNo links for link point '%s'", GETOBJ(string,1));
	return;
    }

    automore_enable(NULL);
    printf("\nCurrent links for link point '%s':", GETOBJ(string,1));
    tp = cp->link_point->accept;
    while (tp && (tp != (dynamic_transition *)&pname(link_trans))) {
	if (tp->args) {
	    printf("\n  %s", tp->args);
	}
	tp = tp->alternate;
    }
    printf("\n");
    automore_disable();
}
#endif	/* PARSER_DEBUG_LINKS */

void show_parser_unresolved (parseinfo *csb)
{
    chain_list *cp = cl;

    printf("\nUnresolved parse chains:");
    while (cp != NULL) {
	printf("\n  %-3d", cp->cp.id);
#ifdef	PARSER_DEBUG_LINKS
	printf(" %s", cp->cp.name);
#endif	/* PARSER_DEBUG_LINKS */
	cp = cp->next;
    }
}

/*
 *  bad_parser_subcommand
 *  Common code fragment for handling a bad subcommands with the new parser
 */
void bad_parser_subcommand (parseinfo *csb, uint var)
{
    errmsg(&msgsym(BADSUBCMD, PARSER), var, get_mode_name(csb->mode),
	   csb->nvgen ? csb->nv_command : csb->line);
}
