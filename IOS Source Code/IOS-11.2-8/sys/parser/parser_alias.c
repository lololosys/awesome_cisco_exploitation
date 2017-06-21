/* $Id: parser_alias.c,v 3.2.64.4 1996/06/17 23:36:13 hampton Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_alias.c,v $
 *------------------------------------------------------------------
 * P A R S E R _ A L I A S . C  (formerly A L I A S . C)
 *
 * Command alias functions 
 *
 * October 1994, Robert Widmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_alias.c,v $
 * Revision 3.2.64.4  1996/06/17  23:36:13  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.2.64.3  1996/05/21  09:59:22  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.64.2  1996/04/30  21:11:52  widmer
 * CSCdi54842:  aliased commands should not be case sensitive
 * Branch: California_branch
 * Change strcmp to strcasecmp
 *
 * Revision 3.2.64.1  1996/03/18  21:33:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:40:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  16:48:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/12/17  18:35:20  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/12/16  07:15:02  thille
 * Parser organizational cleanup.  Rename core parser files to be
 * parser_<foo>.  Start by adding placeholders for the new filenames.
 *
 * OLD LOG as alias.c:
 * -------------------
 * Revision 3.4  1995/12/07  22:32:10  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/28  20:26:35  widmer
 * CSCdi44264:  Change save_line() to use variable-length arguments
 * Remove dead code also.
 *
 * Revision 3.2  1995/11/17  18:42:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:00:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#include <ciscolib.h>
#include <string.h>
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"


/*
 * Local Variables
 */
struct parser_alias_ {
    struct parser_alias_ *next;
    NAMEDTHINGMACRO
    char *command;
};
boolean parser_alias_debug = FALSE;


/*
 * Forward declarations
 */
static parser_alias *create_alias(const char *, const char *);
static void free_alias(parser_alias *);
static void add_parser_alias(queuetype *, const char *, const char *);
static boolean is_a_default_alias(parser_alias *, parser_mode *);
static parser_alias *find_default_alias(char *, parser_mode *);


/*
 * init_aliasQ
 * Initialize specific alias queue and add default aliases
 */
static void init_aliasQ (queuetype *qp, queuetype *default_qp)
{
    parser_alias *ap;

    queue_init(qp, 0);
    if (default_qp) {
	ap = default_qp->qhead;
	while (ap) {
	    add_parser_alias(qp, ap->name, ap->command);
	    ap = ap->next;
	}
    }
}


/*
 * alias_command
 * Handle alias configuration commands
 */
void parser_alias_command (parseinfo *csb)
{
    queuetype *qp;
    queuetype *default_qp;
    parser_alias *ap;
    parser_alias *dp;
    parser_alias *tmp;

    if (!csb->pmp) {
	/* Must have a mode */
	return;
    }

    qp = get_mode_aliases(csb->pmp);
    default_qp = get_mode_defaliases(csb->pmp);

    if (csb->nvgen) {

	/*
	 * Print out default aliases disabled by user
	 */
	if (default_qp) {
	    dp = default_qp->qhead;
	    while (dp) {
		ap = find_namedthing(qp, dp->name);
		if (!ap) {
		    nv_write(TRUE, "no %s %s",
			     csb->nv_command, dp->name);
		}
		dp = dp->next;
	    }
	}

	/*
	 * Print out user defined aliases
	 */
	ap = qp->qhead;
	while (ap) {
	    if (!is_a_default_alias(ap, csb->pmp)) {
		nv_write(TRUE, "%s %s %s",
			 csb->nv_command, ap->name, ap->command);
	    }
	    ap = ap->next;
	}
	return;
    }

    ap = find_namedthing(qp, GETOBJ(string,1));
    if (csb->sense) {
	/*
	 * Add alias
	 */
	if (ap) {
	    /*
	     * Delete old alias
	     */
	    unqueue(qp, ap);
	    free_alias(ap);
	}
	/*
	 * Adding new alias
	 */
	add_parser_alias(qp, GETOBJ(string,1), GETOBJ(string,2));
    } else {
	if (GETOBJ(string,1)[0]) {
	    /*
	     * Delete specific alias
	     */
	    if (ap) {
		unqueue(qp, ap);
		free_alias(ap);
	    }
	    if (csb->set_to_default) {
		ap = find_default_alias(GETOBJ(string,1), csb->pmp);
		if (ap != NULL)
		    add_parser_alias(qp, ap->name, ap->command);
	    }
	} else {
	    /*
	     * Delete all aliases and restore defaults
	     */
	    while ((ap = dequeue(qp))) {
		tmp = ap;
		ap = ap->next;
		free_alias(tmp);
	    }
	    init_aliasQ(qp, default_qp);
	}
    }
}


/*
 * add_parser_alias
 * Create and add alias to specified alias queue
 */
static void add_parser_alias (queuetype *qp, const char *name, const char *command)
{
    parser_alias *ap = create_alias(name, command);
    if (ap) {
	enqueue(qp, ap);
    }
}


/*
 * add_default_alias
 * Create and add default alias
 */
void add_default_alias (parser_mode *mode, const char *name, const char *command)
{
    queuetype *qp;
    queuetype *default_qp;

    qp = get_mode_aliases(mode);
    default_qp = get_mode_defaliases(mode);
    add_parser_alias(qp, name, command);
    add_parser_alias(default_qp, name, command);
}


/*
 * create_alias
 * Allocate and fill in alias structure
 */
static parser_alias *create_alias (const char *name, const char *command)
{
    parser_alias *ap;

    ap = malloc_named(sizeof(parser_alias), "Parser Alias");
    if (ap == NULL) {
	return(NULL);
    }
    sstrncpy(ap->name, name, NAMEDTHINGNAMELEN);
    ap->name[NAMEDTHINGNAMELEN] = '\0';

    setstring(&ap->command, command);
    if (ap->command == NULL) {
	free(ap);
	return(NULL);
    }

    return(ap);
}


/*
 * free_alias
 * Deallocate alias structure
 */
static void free_alias (parser_alias *ap)
{
    free(ap->command);
    free(ap);
}


/*
 *
 */
void free_aliases (queuetype *qp)
{
    parser_alias *ap;
    parser_alias *tmp;

    if (!qp) {
	return;
    }
    ap = qp->qhead;
    if (ap) {
	while (ap) {
	    tmp = ap;
	    ap = ap->next;
	    free_alias(tmp);
	}
    }
}


/*
 * expand_alias
 * Take input line and try to match and expand alias
 */
boolean expand_alias (parseinfo *csb)
{
    char buf[PARSEBUF];
    char *cp = csb->line + csb->line_index;
    char *dp;
    queuetype *qp;
    parser_alias *ap;
    boolean provide_help = FALSE;

    qp = get_mode_aliases(csb->mode);
    if (!qp || !qp->qhead) {
	return(FALSE);
    } else {
	ap = qp->qhead;
    }

    if (csb->line_index != 0) {
	/*
	 * If line begins with whitespace, don't try to do alias
	 * expansion so that someone nasty can't alias all of the
	 * commands to NOOP and deny access to the router.
	 */
	return(FALSE);
    }

    /*
     * Find first keyword
     */
    dp = buf;
    while (*cp && !isspace(*cp)) {
	*dp = *cp;
	dp++;
	cp++;
    }
    *dp = '\0';

    if (! isspace(*cp) && csb->in_help) {
	/*
	 * We've found a keyword, and there isn't any trailing whitespace,
	 * and we're in help, so we should provide short help
	 */
	provide_help = TRUE;
    } else {
	/*
	 * Move to beginning of next keyword or end of input line
	 */
	while (*cp && isspace(*cp)) {
	    cp++;
	}
    }

    /*
     * Try to find alias that matches keyword
     */
    while (ap) {
	if (provide_help) {
	    if (partialmatch(ap->name, buf)) {
		/*
		 * Doing short help
		 */
		if (strchr(ap->command, ' ') == NULL) {
		    /*
		     * Add alias to short help list with a
		     * preceding '*' to distinguish it as an alias
		     */
		    save_line(csb, &csb->help_save, "*%s=%s\t",
			      ap->name, ap->command);
		} else {
		    /*
		     * Alias contains whitespace, so put quotes
		     * around alias
		     */
		    save_line(csb, &csb->help_save, "*%s=\"%s\"\t",
			      ap->name, ap->command);
		}
	    }
	} else {
	    if (strcasecmp(ap->name, buf) == 0) {
		/*
		 * Found a match and we're not doing short help,
		 * so replace keyword with alias
		 */
		if ((strlen(ap->command) + strlen(cp) + 2) <= PARSEBUF) {
		    sprintf(buf, "%s %s", ap->command, cp);
		    strcpy(csb->line_buffer, buf);
		    csb->line_index = 0;
		    return(TRUE);
		} else {
		    printf("\n%% Command too long to expand alias '%s',"
			   "\n%% trying command without expansion",
			   ap->command);
		    return(FALSE);
		}
	    }
	}
	/*
	 * No match, so try next one
	 */
	ap = ap->next;
    }
    /*
     * No matches, so leave input line alone
     */
    return(FALSE);
}


/*
 * show_alias_command
 * Display all current aliases
 */
void show_alias_command (parseinfo *csb)
{
    display_mode_aliases(csb, csb->pmp);
}


/*
 * show_mode_aliases
 * Show specific mode aliases
 */
void show_mode_aliases (parseinfo *csb, queuetype *qp, const char *help)
{
    parser_alias *ap;

    if (!qp) {
	return;
    } else {
	ap = qp->qhead;
    }

    if (ap) {
	printf("\n%s aliases:\n", help);
	while (ap) {
	    printf("  %21s %s\n", ap->name, ap->command);
	    ap = ap->next;
	}
    }
}


/*
 * is_a_default_alias
 * Return true if alias is a default alias
 */
static boolean is_a_default_alias (parser_alias *ap, parser_mode *mode)
{
    queuetype *default_qp;
    parser_alias *dp;

    default_qp = get_mode_defaliases(mode);

    if (default_qp) {
	dp = default_qp->qhead;
	while (dp) {
	    if ((strcasecmp(ap->name, dp->name) == 0) &&
		(strcasecmp(ap->command, dp->command) == 0)) {
		return(TRUE);
	    }
	    dp = dp->next;
	}
    }

    return(FALSE);
}

/*
 * find_default_alias
 * Return a default alias (if any) for the keyword
 */
static parser_alias *find_default_alias (char *name, parser_mode *mode)
{
    queuetype *default_qp;
    parser_alias *dp;

    dp = NULL;
    default_qp = get_mode_defaliases(mode);
    if (default_qp) {
	for (dp = default_qp->qhead; dp != NULL; dp = dp->next) {
	    if (strcasecmp(dp->name, name) == 0)
		break;
	}
    }
    return(dp);
}
