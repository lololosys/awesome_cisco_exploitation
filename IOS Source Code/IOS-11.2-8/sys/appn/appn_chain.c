/* $Id: appn_chain.c,v 3.11.4.6 1996/08/28 12:35:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/appn/appn_chain.c,v $
 *------------------------------------------------------------------
 * APPN Parse chain
 *
 * May 1995, Steve Berl
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appn_chain.c,v $
 * Revision 3.11.4.6  1996/08/28  12:35:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.4.5  1996/06/28  23:04:23  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.11.4.4  1996/06/17  23:28:29  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.11.4.3  1996/05/21  09:41:32  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.4.2  1996/05/17  10:38:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.10.2.3  1996/04/03  13:40:04  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.10.2.2  1996/04/01  04:41:28  bchan
 * CSCdi53188:  appn parser multiple definition
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.10.2.1  1996/03/17  17:34:22  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.11.4.1  1996/03/18  18:49:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/16  06:24:11  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.2  1996/03/07  08:27:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  00:15:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/09  22:54:18  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.10  1996/02/22  14:46:07  mregan
 * CSCdi48499:  sho appn int filter does not work for unqualified LU names
 * corrected logic for filtering on names
 *
 * Revision 3.9  1996/02/22  14:31:46  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.8  1996/02/20  18:02:01  widmer
 * CSCdi49363:  Drop function name from INVALID_NVGEN_CHECK
 *
 * Revision 3.7  1996/02/01  05:58:31  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/18  19:43:50  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/17  18:23:51  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/05  21:57:22  mregan
 * CSCdi42175:  appn show output gets too large to be useful
 * added ability to filter output
 *
 * Revision 3.3  1995/12/01  15:55:55  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  08:37:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:52:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/06/22  22:31:57  dmccowan
 * CSCdi36064:  APPN doesnt check malloc return
 *
 * Revision 2.6  1995/06/22  15:51:45  dmccowan
 * CSCdi36064:  APPN doesnt check malloc return
 *
 * Revision 2.5  1995/06/19  17:14:45  dmccowan
 * CSCdi35705:  should allow lowercase characters when specifying names
 *
 * Revision 2.4  1995/06/16  21:25:13  widmer
 * CSCdi35996:  appn code doesnt compile due to parser changes
 *
 * Revision 2.3  1995/06/15  15:18:02  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.2  1995/06/09  19:25:53  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * Allow appn to specify the priority parameter for each link station.
 *
 * Revision 2.1  1995/06/07  20:03:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include"logger.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include <ctype.h>
#include "config.h"
#include "parser.h"
#include "interface_private.h"
#include "../cls/clsi_msg.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "sys_registry.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_exec.h"
#include "appn_subsys.h"
#include "appn_parser.h"
#define NO_APPN_GETOBJ
#include "appn_function.h"

struct appntype {
    const uint type;
    const char *name;
    const char *help;
} appntypes[] = {
    { DEFCP_POSITION, APPN_NAME_CP, "Define the APPN Control Point"},
    { DEFLINK_POSITION, APPN_NAME_LINK, "Define an APPN Logical Link"},
    { DEFPORT_POSITION, APPN_NAME_PORT, "Define an APPN Port"},
    { DEFCN_POSITION, APPN_NAME_CN, "Define an APPN Connection Network"},
    { DEFRLU_LOC_POSITION, APPN_NAME_PLOC, "Define an APPN Partner LU Location"},
    { DEFCOS_POSITION, APPN_NAME_COS, "Define an APPN Class of Service"},
    { DEFMODE_POSITION, APPN_NAME_MODE, "Define an APPN Mode"},
    { DEFSTART_POSITION, APPN_NAME_START, "Start the APPN subsystem"},
    { 0, NULL, NULL}
};


void appn_action (parseinfo *csb, transition *mine, void *null)
{
    char	*cp;
    queuetype	*saveQ;	   /* save the csb->tokenQ here during NV generation */
    parsenode	*pp;
    void * verb;
    struct appntype *appnt;
    uint i;   /* index into verb array */
    uint li;  /* index into command line */
    char *keyword = NULL; /* keyword for nv_command */

    /* set up for the alternate transition */
    push_node(csb, mine->alternate);


    if (csb->nvgen) {
	/* (comment largely stolen from controller_action() )
	 *
	 * Find the end of the existing NV generated command, append a space,
	 * and the full appn name.  Set csb->appn to be the current
	 * appn command then save the accepting state.
	 * 
	 * Because the stack is push-down, the appn commands will be stored in
	 * reverse order. Use a temporary stack to store the appn command
	 * nodes, then copy to the real stack.
	 * 
	 * PLAN: 
	 * 1. Save the existing csb->tokenQ. 
	 * 2. Create a temporary stack. 
	 * 3. Save the appn command nodes onto this stack (push_node() calls). 
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
	saveQ = csb->tokenQ;	/* Save the existing stack */
	csb->tokenQ = malloc(sizeof(queuetype));
	if (!csb->tokenQ) {
	    csb->tokenQ = saveQ;
	    return;
	}
	queue_init(csb->tokenQ, 0);

	/* now generate a node for every appn command that needs it */
        i = 0;
        verb = NULL;
        while (get_appn_verb(&i, &verb, &keyword))
	  {
              strcpy(cp, keyword);
              csb->appn_object_verb = verb;
              csb->appn_object_type = i;
	      push_node(csb, mine->accept);              
          }

	/* copy the nodes onto the real stack */
	while (! QUEUEEMPTY(csb->tokenQ)) {
	    pp = dequeue(csb->tokenQ);
	    requeue(saveQ, pp);
	}
	free(csb->tokenQ);
	csb->tokenQ = saveQ;
	csb->appn_object_verb = NULL;
        csb->appn_object_type = 0;
	*--cp = '\0';		/* Stomp on added space */
	return;
    }

    /*
     *	We're parsing an appn command.  Try to match the appn object.
     */
    for (appnt = appntypes; appnt->name; appnt++) 
        {
        li = 0;
	if (match_partial_keyword(csb, appnt->name, &li, appnt->help,
				  0, KEYWORD_WS_OK|KEYWORD_HELP_CHECK))
	   {
           csb->appn_object_type = appnt->type;
           csb->line_index += li;
	   push_node(csb, mine->accept);
	   csb->line_index -= li;
	   }
        else
	   {
	   }
        }
}

/*
 * Process 'exit' from within an appn config command
 */
void exit_appn_command (parseinfo *csb)
{
    csb->mode = config_mode;
    csb->appn_object_verb = NULL;
    csb->appn_object_type = 0;
}


/*
 * Parse chains for APPN exec commands
 */
#define	ALTERNATE	NONE
#include "../../sys/appn/exec_startappn.h"
LINK_POINT(appn_exec_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for APPN show commands
 */
#define	ALTERNATE	NONE
#include "../../sys/appn/exec_show_appn.h"
LINK_POINT(appn_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for APPN debug commands
 */
#define	ALTERNATE	NONE
#include "../../sys/appn/exec_debug_appn.h"
LINK_POINT(appn_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for APPN config commands
 */

#define	APPN_COMMAND(name, acc, alt)\
    static transition pname(name) =\
         {&pname(acc), &pname(alt), (trans_func) appn_action, NULL}

#define	ALTERNATE	NONE
#include "../../sys/appn/cfg_appn.h"
LINK_POINT(appn_cfg_command, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for APPN control point config commands
 */
#define	ALTERNATE	no_alt
#include "../../sys/appn/cfg_appn_cp.h"
NO_OR_DEFAULT(cfg_appn_cp_no, ALTERNATE, PRIV_MIN|PRIV_SUBIF|PRIV_NONVGEN);
HELP	(cfg_appn_cp, cfg_appn_cp_no,
	 "APPN Control Point configuration commands:\n");
#undef	ALTERNATE

/*
 * Parse chains for APPN link config commands
 */
#define	ALTERNATE	no_alt
#include "../../sys/appn/cfg_appn_link.h"
NO_OR_DEFAULT(cfg_appn_link_no, ALTERNATE, PRIV_MIN|PRIV_SUBIF|PRIV_NONVGEN);
HELP	(cfg_appn_link, cfg_appn_link_no,
	 "APPN Link Station configuration commands:\n");
#undef	ALTERNATE

/*
 * Parse chains for APPN port config commands
 */
#define	ALTERNATE	no_alt
#include "../../sys/appn/cfg_appn_port.h"
NO_OR_DEFAULT(cfg_appn_port_no, ALTERNATE, PRIV_MIN|PRIV_SUBIF|PRIV_NONVGEN);
HELP	(cfg_appn_port, cfg_appn_port_no,
	 "APPN Port configuration commands:\n");
#undef	ALTERNATE

/*
 * Parse chains for APPN conn-net config commands
 */
#define	ALTERNATE	no_alt
#include "../../sys/appn/cfg_appn_cn.h"
NO_OR_DEFAULT(cfg_appn_cn_no, ALTERNATE, PRIV_MIN|PRIV_SUBIF|PRIV_NONVGEN);
HELP	(cfg_appn_cn, cfg_appn_cn_no,
	 "APPN Connection Network configuration commands:\n");
#undef	ALTERNATE

/*
 * Parse chains for APPN ploc config commands
 */
#define	ALTERNATE	no_alt
#include "../../sys/appn/cfg_appn_ploc.h"
NO_OR_DEFAULT(cfg_appn_ploc_no, ALTERNATE, PRIV_MIN|PRIV_SUBIF|PRIV_NONVGEN);
HELP	(cfg_appn_ploc, cfg_appn_ploc_no,
	 "APPN Partner LU Location configuration commands:\n");
#undef	ALTERNATE

/*
 * Parse chains for APPN cos config commands
 */
#define	ALTERNATE	no_alt
#include "../../sys/appn/cfg_appn_cos.h"
NO_OR_DEFAULT(cfg_appn_cos_no, ALTERNATE, PRIV_MIN|PRIV_SUBIF|PRIV_NONVGEN);
HELP	(cfg_appn_cos, cfg_appn_cos_no,
	 "APPN COS configuration commands:\n");
#undef	ALTERNATE

/*
 * Parse chains for APPN mode config commands
 */
#define	ALTERNATE	no_alt
#include "../../sys/appn/cfg_appn_mode.h"
NO_OR_DEFAULT(cfg_appn_mode_no, ALTERNATE, PRIV_MIN|PRIV_SUBIF|PRIV_NONVGEN);
HELP	(cfg_appn_mode, cfg_appn_mode_no,
	 "APPN Mode configuration commands:\n");
#undef	ALTERNATE

/*
 * Parse chain registration array for APPN
 */
static parser_extension_request appn_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(appn_exec_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(appn_show_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(appn_debug_commands) },
    { PARSE_ADD_CFG_BEFORE_RTR_CMD, &pname(appn_cfg_command) },
    { PARSE_LIST_END, NULL }
};

/*
 * struct to save appn mode specific information
 */
typedef struct appn_save_pinfo_ {
  uint appn_object_type;
  void *appn_object_verb;
} appn_save_pinfo;

/*
 * appn_save_var
 */
void *appn_save_var (parseinfo *csb)
{
  appn_save_pinfo *appn_save_p;

  appn_save_p = malloc(sizeof(appn_save_pinfo));
  if (!appn_save_p) {
     printf(nomemory);
     return(NULL);
  }

  appn_save_p->appn_object_type = csb->appn_object_type;
  appn_save_p->appn_object_verb = csb->appn_object_verb;

  csb->appn_object_type = 0;
  csb->appn_object_verb = NULL;

  return(appn_save_p);
}

/*
 * appn_reset_var
 */
void appn_reset_var (parseinfo *csb, void *var)
{
  
  csb->appn_object_type = ((appn_save_pinfo *)var)->appn_object_type;
  csb->appn_object_verb = ((appn_save_pinfo *)var)->appn_object_verb;

  free(var);
}


void appn_push_node (appn_csb_t *appn_csb_p)
{
   parseinfo *csb;

   csb = (parseinfo *)appn_csb_p->original_csb_p;

   *csb->nv_command = '\0';

   switch (csb->appn_object_type) {
      case DEFCP_POSITION:
         push_node(csb, &pname(cfg_appn_cp));
         break;
      case DEFLINK_POSITION:
         push_node(csb, &pname(cfg_appn_link));
         break;
      case DEFPORT_POSITION:
         push_node(csb, &pname(cfg_appn_port));
         break;
      case DEFRLU_LOC_POSITION:
         push_node(csb, &pname(cfg_appn_ploc));
         break;
      case DEFMODE_POSITION:
         push_node(csb, &pname(cfg_appn_mode));
         break;
      case DEFCOS_POSITION:
         push_node(csb, &pname(cfg_appn_cos));
         break;
      case DEFCN_POSITION:
         push_node(csb, &pname(cfg_appn_cn));
         break;
      default:
         buginf("%% Error: invalid appn_object_type in appn_push_node.");
   }

}

/*
 * appn_parser_init - Initialize APPN parser support
 */
void appn_parser_init (void)
{
    parser_add_command_list(appn_chain_init_table, "appn");

    parser_add_mode("appn-cp", PROMPT_APPN_CP, 
		    "APPN control-point configuration mode",
		    TRUE, TRUE, "configure",
		    appn_save_var, appn_reset_var,
		    &pname(cfg_appn_cp),
		    NULL);

    parser_add_mode("appn-link", PROMPT_APPN_LINK, 
		    "APPN link-station configuration mode",
		    TRUE, TRUE, "configure",
		    appn_save_var, appn_reset_var,
		    &pname(cfg_appn_link),
		    NULL);

    parser_add_mode("appn-port", PROMPT_APPN_PORT, 
		    "APPN port configuration mode",
		    TRUE, TRUE, "configure",
		    appn_save_var, appn_reset_var,
		    &pname(cfg_appn_port),
		    NULL);

    parser_add_mode("appn-cn", PROMPT_APPN_CN, 
		    "APPN connection-network configuration mode",
		    TRUE, TRUE, "configure",
		    appn_save_var, appn_reset_var,
		    &pname(cfg_appn_cn),
		    NULL);

    parser_add_mode("appn-ploc", PROMPT_APPN_PLOC, 
		    "APPN partner-lu-location configuration mode",
		    TRUE, TRUE, "configure",
		    appn_save_var, appn_reset_var,
		    &pname(cfg_appn_ploc),
		    NULL);

    parser_add_mode("appn-cos", PROMPT_APPN_COS, 
		    "APPN class-of-service configuration mode",
		    TRUE, TRUE, "configure",
		    appn_save_var, appn_reset_var,
		    &pname(cfg_appn_cos),
		    NULL);

    parser_add_mode("appn-mode", PROMPT_APPN_MODE, 
		    "APPN mode configuration mode",
		    TRUE, TRUE, "configure",
		    appn_save_var, appn_reset_var,
		    &pname(cfg_appn_mode),
		    NULL);
}
