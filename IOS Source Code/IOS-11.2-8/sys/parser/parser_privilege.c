/* $Id: parser_privilege.c,v 3.2.64.1 1996/05/21 09:59:23 thille Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_privilege.c,v $
 *------------------------------------------------------------------
 * P A R S E R _ P R I V I L E G E . C  (formerly P R I V I L E G E . C)
 *
 * Change privilege level of parser commands
 *
 * November 1994, Robert Widmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_privilege.c,v $
 * Revision 3.2.64.1  1996/05/21  09:59:23  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/12/17  18:35:21  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/12/16  07:15:01  thille
 * Parser organizational cleanup.  Rename core parser files to be
 * parser_<foo>.  Start by adding placeholders for the new filenames.
 *
 * OLD LOG as privilege.c:
 * -----------------------
 * Revision 3.2  1995/11/17  18:53:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  07:16:02  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:32:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/parser_privilege.h"

static void set_privilege_level(parser_mode *, uint, uint,
				char *, boolean, boolean, char *);

/*
 * priv_enqueue
 *
 * Add a privilege structure onto queue, allocating storage
 * for queue if necessary
 */
static inline void priv_enqueue (queuetype **qp, void *pv)
{
    if (!*qp) {
	*qp = malloc(sizeof(queuetype));
	if (!*qp) {
	    /* No memory */
	    return;
	} else {
	    queue_init(*qp, 0);
	}
    }
    requeue(*qp, pv);
}

/*
 * priv_command
 *
 * Handle parsing of privilege global config command. Call
 * set_privilege_level since we need a new csb, so call with
 * needed arguments extracted from csb
 */
void priv_command (parseinfo *csb)
{
    if (csb->nvgen && !priv_changed_in_mode(csb->pmp)) {
	return;
    }
    set_privilege_level(csb->pmp, GETOBJ(int,1), GETOBJ(int,2),
			GETOBJ(string,1), csb->sense, csb->nvgen,
			csb->nv_command);
}

/*
 * set_privilege_level
 *
 * Handle parsing and NV generation of privilege global config command.
 * If doing NV generation
 *	traverse parse tree of current node
 *	NV generate commands added to priv_nvgenQ
 * else
 *	parser command with parse tree of current node
 *	change priv level of transitions added to privQ
 */
static void set_privilege_level (parser_mode *mode, uint level, uint reset,
				 char *com, boolean sense, boolean nvgen,
				 char *nv_command)
{
    parseinfo *csb;
    privq *pq;
    priv_nvgenq *pnvq;
    priv_nvgenq *ptmp;
    priv_nvgenq *prev;

    csb = get_csb("Privilege CSB");
    if (!csb) {
	printf(nomemory);
	return;
    }

    csb->mode = mode;
    csb->nvgen = nvgen;
    csb->priv = PRIV_ROOT;
    csb->sense = TRUE;
    csb->priv_set = TRUE;
    csb->permanent = TRUE;
    push_node(csb, get_mode_top(csb->mode));

    if (csb->nvgen) {
	parse_token(csb);		    /* traverse parse tree */
	if (csb->priv_nvgenQ) {
	    /*
	     * Weed out duplicate entries
	     */
	    pnvq = csb->priv_nvgenQ->qhead;
	    prev = NULL;
	    while (pnvq) {

		ptmp = pnvq->next;
		while (ptmp) {
		    if ((pnvq->level == ptmp->level) &&
			(strcmp(pnvq->command, ptmp->command) == 0)) {
			/* Duplicate entry */
			if (parser_priv_debug) {
			    printf("\nDuplicate priv entry 0x%x %d '%s'",
				   mode, pnvq->level, pnvq->command);
			}
			if (prev) {
			    prev->next = pnvq->next;
			} else {
			    csb->priv_nvgenQ->qhead = pnvq->next;
			}
			free(pnvq);
			pnvq = NULL;
			break;
		    }
		    ptmp = ptmp->next;
		}
		if (pnvq) {
		    prev = pnvq;
		    pnvq = pnvq->next;
		} else {
		    pnvq = csb->priv_nvgenQ->qhead;
		}
	    }

	    /*
	     * NV generate remaining entries
	     */
	    while (! QUEUEEMPTY(csb->priv_nvgenQ)) {
		/*
		 * For each priv structure, remove from nvgenQ,
		 * NV generate, and free priv structure
		 */
		pnvq = dequeue(csb->priv_nvgenQ);
		nv_write(TRUE, "%s level %d %s", nv_command,
			 pnvq->level, pnvq->command);
		free(pnvq);
	    }
	}
	free_csb(&csb);
	return;
    }

    strcpy(csb->line, com);
    if (parser_priv_debug) {
	printf("\nSetting privilege level to %d for command '%s'",
	       level, csb->line);
    }

    /*
     * Parse the input buffer and find the
     * transitions that need to be changed.
     */
    parse_token(csb);
    if (!csb->privQ) {
	/* No transitions found, so report error */
	printf("\n%% Unknown command '%s'", csb->line);
    } else {
	/*
	 * We have some transitions, so change the
	 * privilege levels for them.
	 */
	set_priv_changed_in_mode(mode, TRUE);
	while (! QUEUEEMPTY(csb->privQ)) {
	    pq = dequeue(csb->privQ);
	    if (sense && !reset) {
		if (parser_priv_debug) {
		    printf("\nChanging privilege for 0x%x from %x-%x-%d-0x%x",
			   pq->priv, pq->priv->cur_priv,
			   pq->priv->def_priv, pq->priv->priv_changed,
			   TRANS_PRIV_FLAGS_GET(pq->priv->flags));
		}
		pq->priv->priv_changed = TRUE;
		pq->priv->cur_priv = level;
		if (parser_priv_debug) {
		    printf(" to %x-%x-%d-0x%x", pq->priv->cur_priv,
			   pq->priv->def_priv, pq->priv->priv_changed,
			   TRANS_PRIV_FLAGS_GET(pq->priv->flags));
		}
	    } else {
		if (parser_priv_debug) {
		    printf("\nChanging privilege for 0x%x from %x-%x-%d-0x%x",
			   pq->priv, pq->priv->cur_priv,
			   pq->priv->def_priv, pq->priv->priv_changed,
			   TRANS_PRIV_FLAGS_GET(pq->priv->flags));
		}
		pq->priv->cur_priv = pq->priv->def_priv;
		if (reset) {
		    pq->priv->priv_changed = FALSE;
		}
		if (parser_priv_debug) {
		    printf(" to default %x-%x-%d-0x%x", pq->priv->cur_priv,
			   pq->priv->def_priv, pq->priv->priv_changed,
			   TRANS_PRIV_FLAGS_GET(pq->priv->flags));
		}
	    }
	    free(pq);
	}
    }
    free_csb(&csb);
}

/*
 * show_priv_command
 *
 * Display users current privilege level
 */
void show_priv_command (parseinfo *csb)
{
    printf("\nCurrent privilege level is %d", csb->priv);
}

/*
 * priv_push
 *
 * Allocate storage for privilege structure and add to privQ
 */
void priv_push (parseinfo *csb, trans_priv *priv)
{
    privq *pq;

    /*
     * Modify the current privilege
     */
    if (parser_priv_debug) {
	printf("\nPushing transition on stack  0x%x", priv);
    }
    pq = malloc(sizeof(privq));
    if (!pq) {
	printf(nomemory);
	return;
    }
    pq->priv = priv;
    pq->next = NULL;
    /* Add priv structure to privQ */
    priv_enqueue(&csb->privQ, pq);
}

/*
 * nvgen_privilege
 *
 * Allocate storage for privilege structure and add to priv_nvgenQ
 */
boolean nvgen_privilege (parseinfo *csb, trans_priv *priv, const char *str)
{
    priv_nvgenq *pnvq;

    if ((! priv->priv_changed) || (csb->flags & CONFIG_ABORT)) {
	/*
	 * Privilege level hasn't changed, or a previous command
	 * failed and we shouldn't bother
	 */
	return(FALSE);
    }

    pnvq = malloc(sizeof(priv_nvgenq));
    if (!pnvq) {
	/*
	 * Out of memory, flag CONFIG_ABORT so furthur commands aren't
	 * NV generated and return
	 */
	csb->flags |= CONFIG_ABORT;
	printf(nomemory);
	return(FALSE);
    }
    /* Add text of command */
    if (csb->nv_command[0] != '\0') {
	sprintf(pnvq->command, "%s %s", csb->nv_command, str);
    } else {
	strcpy(pnvq->command, str);
    }
    pnvq->level = priv->cur_priv;
    pnvq->next = NULL;
    /* Add priv structure to priv_nvgenQ */
    priv_enqueue(&csb->priv_nvgenQ, pnvq);
    return(TRUE);
}


