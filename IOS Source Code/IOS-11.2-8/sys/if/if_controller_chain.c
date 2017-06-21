/* $Id: if_controller_chain.c,v 3.6.4.4 1996/07/08 22:31:02 mwu Exp $
 * $Source: /release/112/cvs/Xsys/if/if_controller_chain.c,v $
 *------------------------------------------------------------------
 * if_controller_chain.c - Controller parser chain functions.
 *                 (based on hes/controller_chain.c by Rob Widmer)
 *
 * Jan 30 1995, Manoj Leelanivas
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_controller_chain.c,v $
 * Revision 3.6.4.4  1996/07/08  22:31:02  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.6.4.3  1996/06/17  23:31:10  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.6.4.2  1996/05/21  09:49:50  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.4.1  1996/03/18  20:13:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.3  1996/03/16  06:55:26  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.2  1996/03/07  09:43:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  14:23:23  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/03/09  22:55:08  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.5  1996/02/22  14:34:06  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1995/12/17  18:28:48  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/01  15:56:19  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  09:27:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:50:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/02  10:53:28  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/15  15:18:26  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  20:52:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include "interface_private.h"
#include "if_controller.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "sys_registry.h"
#include "../parser/parser_defs_parser.h"

struct cftype {
    uint type;
    const char *name;
    const char *help;
    uint min_slot;
    uint max_slot;
    uint min_unit;
    uint max_unit;
} static cftypes[] = {
    { IFTYPE_ETHER,	IFNAME_ETHER,	  "Ethernet controller",  0, 0, 0, 0 },
    { IFTYPE_FDDI,	IFNAME_FDDI,	  "Fddi controller",	  0, 0, 0, 0 },
    { IFTYPE_SERIAL,	IFNAME_SERIAL,	  "Serial controller",	  0, 0, 0, 0 },
    { IFTYPE_HSSI,	IFNAME_HSSI,	  "HSSI controller",	  0, 0, 0, 0 },
    { IFTYPE_TOKENRING, IFNAME_TOKENRING, "Token Ring controller",0, 0, 0, 0 },
    { IFTYPE_ATM,	IFNAME_ATM,	  "ATM controller",	  0, 0, 0, 0 },
    { IFTYPE_T1,	IFNAME_T1,	  "T1 controller",	  0, 0, 0, 0 },
    { IFTYPE_E1,	IFNAME_E1,	  "E1 controller",	  0, 0, 0, 0 },
    { IFTYPE_FASTETHER,	IFNAME_FASTETHER, "FastEthernet controller", 0, 0, 0, 0 },
    { IFTYPE_POSIP,	IFNAME_POSIP,     "POSI controller", 0, 0, 0, 0 },
    { 0, NULL, NULL, 0, 0, 0, 0 }
};



/* there are no arguments to the controller_action function */
void controller_action (parseinfo *csb, transition *mine, void *null)
{
    char	*cp;
    queuetype	*t1;	   /* save the csb->tokenQ here during NV generation */
    queuetype	t2;
    cdbtype	*cdb;
    parsenode	*pp;
    char	buffer[32];
    uint match_nows_flag = (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT |
			    OCT_ZERO_OK | NUMBER_DEC |
			    NUMBER_NO_WS_OK | NUMBER_HELP_CHECK);
    struct cftype *cft;
    uint types_present = 0;
    uint i;
    uint oldi;
    uint unit;
#ifdef SLOT_BASED_CDB_PLATFORM
    uint slot;
#endif  /* slot based platforms */

    /* set up for the alternate transition */
    push_node(csb, mine->alternate);

    /*
     * Find out which controllers we have
     */
    for (cft = cftypes; cft->type; cft++) {
	cft->min_slot = -1;
	cft->max_slot = 0;
	cft->min_unit = -1;
	cft->max_unit = 0;

	for (cdb = (cdbtype *)cdbQ->qhead ; cdb != NULL ; cdb = cdb->next) {
	    if (!strcmp(cft->name, cdb->cdb_name)) {
		types_present |= cft->type;

#define	MIN(a,b)	(((a) < (b)) ? (a) : (b))
#define	MAX(a,b)	(((a) > (b)) ? (a) : (b))

#ifdef SLOT_BASED_CDB_PLATFORM
		cft->min_slot = MIN(cft->min_slot, cdb->slot);
		cft->max_slot = MAX(cft->max_slot, cdb->slot);
#endif	/* slot based platforms */
		cft->min_unit = MIN(cft->min_unit, cdb->appl_no);
		cft->max_unit = MAX(cft->max_unit, cdb->appl_no);
	    }
	}
    }

    if (csb->nvgen) {
	/* (comment largely stolen from interface_action() )
	 *
	 * Find the end of the existing NV generated command, append a space,
	 * and the full controller name.  Set csb->controller to be the current
	 * controller then save the accepting state.
	 * 
	 * Because the stack is push-down, the controllers will be stored in
	 * reverse order. Use a temporary stack to store the controller nodes,
	 * then copy to the real stack.
	 * 
	 * PLAN: 
	 * 1. Save the existing csb->tokenQ. 
	 * 2. Create a temporary stack. 
	 * 3. Save the controller nodes onto this stack (push_node() calls). 
	 * 4. Copy the nodes to the saved tokenQ by popping from the temporary
	 *    stack and pushing onto the real stack (thus reversing the order 
	 *    of the nodes.) 
	 * 5. Free the temporary queue header. 
	 * 6. Restore csb->tokenQ.
	 */
	for (cp = csb->nv_command ; *cp ; cp++)
	    ;
	*cp++ = ' ';
	*cp = '\0';
	t1 = csb->tokenQ;	/* Save the existing stack */
	queue_init(&t2, 0);
	csb->tokenQ = &t2;

	/* now generate for every controller that needs it */
	for (cdb = (cdbtype *)cdbQ->qhead ; cdb != NULL ; cdb = cdb->next) {
	    if (!(cdb->cdb_status & CDB_CONFIGURE))
		continue;
	    strcpy(cp, cdb->cdb_namestring);
	    SETOBJ(cdb,1) = cdb;
	    push_node(csb, mine->accept);
	}

	/* copy the nodes onto the real stack */
	while (! QUEUEEMPTY(csb->tokenQ)) {
	    pp = dequeue(csb->tokenQ);
	    requeue(t1, pp);
	}
	csb->tokenQ = t1;
	SETOBJ(cdb,1) = NULL;
	*--cp = '\0';		/* Stomp on added space */
	return;
    }

    /*
     *	We're parsing a controller.  Try to match the controller name and
     *	unit designation.
     */
    if (!cdbQ || !cdbQ->qhead) {
	return;
    }

    oldi = 0;
    for (cft = cftypes; cft->type; cft++) {
	if (!(cft->type & types_present)) {
	    continue;
	}

	i = oldi;
	if (match_partial_keyword(csb, cft->name, &i, cft->help,
				  0, KEYWORD_WS_OK|KEYWORD_HELP_CHECK)) {
#ifdef SLOT_BASED_CDB_PLATFORM

	    if (match_number(csb, cft->min_slot, cft->max_slot, &i, &slot,
			     match_nows_flag, "Controller slot number")) {
		if (match_char(&csb->line[csb->line_index], &i, '/')) {
#endif	/* slot based platforms */
		    if (match_number(csb, cft->min_unit, cft->max_unit,
				     &i, &unit, match_nows_flag,
				     "Controller unit number")) {
			if (match_whitespace2(&csb->line[csb->line_index], &i,
					      csb->in_help)) {
			    boolean found_match = FALSE;
			    for (cdb = (cdbtype *)cdbQ->qhead;
				 cdb; cdb = cdb->next) {
				if (!strcmp(cft->name, cdb->cdb_name) &&
#ifdef SLOT_BASED_CDB_PLATFORM

				    (slot == cdb->slot) &&
#endif	/* slot based platforms */
				    (unit == cdb->appl_no)) {

				    /* we have a match */
				    found_match = TRUE;
				    csb->line_index += i;
				    SETOBJ(cdb,1) = cdb;
				    push_node(csb, mine->accept);
				    csb->line_index -= i;
				}
			    }
			    if (!found_match) {
#ifdef SLOT_BASED_CDB_PLATFORM

				printf("\nCan't find controller %s %d/%d",
				       cft->name, slot, unit);
#else
				printf("\nCan't find controller %s %d",
				       cft->name, unit);
#endif	/* slot based platforms */
			    }
			    break;
			} else {
			    general_number_short_help(&match_nows_flag,
						      cft->min_unit,
						      cft->max_unit,
						      buffer);
			    help_or_error(csb, i, buffer,
					  NO_COMMAND_COMPLETION);
			    
			}
		    }
#ifdef SLOT_BASED_CDB_PLATFORM

		} else {
		    help_or_error(csb, i, "/", NO_COMMAND_COMPLETION);
		}
	    }
#endif	/* slot based platforms */
	}
    }
}



/*
 * A controller name and unit number
 */
#define	CONTROLLER_COMMAND(name, acc, alt)\
    static transition pname(name) =\
         {&pname(acc), &pname(alt), (trans_func) controller_action, NULL}



/*
 * Parse chain for clear controller command
 */
#define	ALTERNATE	NONE
#include "exec_clear_controller.h"
LINK_POINT(controller_clear_command, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain for controller command
 */
#define		ALTERNATE	NONE
#include "cfg_controller.h"
LINK_POINT(controller_cmd, ALTERNATE);
#undef	ALTERNATE


/*******************************************************
 *
 *	Start of config controller commands
 */
LINK_EXIT(after_controller_exit, no_alt);

#undef ALTERNATE
#define ALTERNATE after_controller_exit

/*
 *  Note that no commands #include'd after cfg_ctrlr_exit.h will ever
 *  go through NV Generation.  You probably want to add only below
 *  it.
 */
#include	"cfg_ctrlr_exit.h"

#include	"cfg_ctrlr_description.h"

LINK_TRANS	(cfg_controller_extend_here, ALTERNATE);
NO_OR_DEFAULT(configctrlr_no, cfg_controller_extend_here,
	   PRIV_MIN | PRIV_SUBIF | PRIV_NONVGEN);
HELP	(top_config_controller, configctrlr_no,
	 "Controller configuration commands:\n");

static parser_extension_request controller_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(controller_clear_command) },
    { PARSE_ADD_CFG_CTRLLR_CMD, &pname(controller_cmd) },
    { PARSE_ADD_GLOBAL_EXIT,
	  (dynamic_transition *) &pname(after_controller_exit) },
    { PARSE_LIST_END, NULL }
};

static void *controller_save_var (parseinfo *csb)
{
    cdbtype *controller = csb->controller;

    csb->controller = NULL;
    return(controller);
}

static void controller_reset_var (parseinfo *csb, void *var)
{
    csb->controller = var;
}

/*
 * 
 */
void controller_parser_init (void)
{
    parser_add_command_list(controller_chain_init_table, "controller");
    parser_add_link_point(PARSE_ADD_CFG_CONTROLLER_CMD, "controller cmds",
		      &pname(cfg_controller_extend_here));
    parser_add_mode("controller", "config-controller",
		    "Controller configuration mode",
		    TRUE, TRUE, "configure",
		    controller_save_var, controller_reset_var,
		    &pname(top_config_controller),
		    NULL);
}
