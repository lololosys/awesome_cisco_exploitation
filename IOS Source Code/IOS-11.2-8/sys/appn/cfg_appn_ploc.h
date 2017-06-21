/* 
 * $Id: cfg_appn_ploc.h,v 3.1.40.1 1996/08/12 16:00:18 widmer Exp $
 * $Source: /release/112/cvs/Xsys/appn/cfg_appn_ploc.h,v $
 *------------------------------------------------------------------
 *
 * Configuration Commands for APPN
 *
 * August 1994, Dave McCowan
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 *
 * $Log: cfg_appn_ploc.h,v $
 * Revision 3.1.40.1  1996/08/12  16:00:18  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:56:49  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/12/01  15:56:54  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  18:42:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:01:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
*/


/******************************************************************
 * appn partner-lu-location <luname>
 *    ...subcommands...
 */


/*
 *   exit
 */
EOLNS	(appn_ploc_exit_eol, exit_appn_command);
KEYWORD	(appn_ploc_exit_kwd, appn_ploc_exit_eol, NONE,
	 "exit", "Exit from appn-ploc configuration mode", PRIV_CONF);
TESTVAR	(appn_ploc_exit, NONE, appn_ploc_exit_kwd, NONE, NONE, ALTERNATE,
	 sense, FALSE);
#undef  ALTERNATE
#define ALTERNATE appn_ploc_exit

/*
 *   [no] complete
 */
EOLS      (cfg_appn_ploc_comp_eol, appn_command, APPN_PLOC_COMP);
KEYWORD_ID(cfg_appn_ploc_comp, cfg_appn_ploc_comp_eol, ALTERNATE,
           appn_subcommand_type, APPN_PLOC_COMP,
           "complete", "Complete PLOC definition.", PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_ploc_comp

/*
 *    [no] owning-nn-name <name>
 */
EOLS	  (cfg_appn_ploc_ownnn_eol, appn_command, APPN_PLOC_OWNNN);
APPN_FQNAME (cfg_appn_ploc_ownnn_num, cfg_appn_ploc_ownnn_eol, no_alt,
           OBJ(string,1), APPN_PLOC_OWNNN_VAL_HELP);
NOPREFIX  (cfg_appn_ploc_ownnn_noprefix, cfg_appn_ploc_ownnn_num, cfg_appn_ploc_ownnn_eol);
NVGENS    (cfg_appn_ploc_ownnn_nvgen, cfg_appn_ploc_ownnn_noprefix,
           appn_command, APPN_PLOC_OWNNN);
KEYWORD_ID(cfg_appn_ploc_ownnn, cfg_appn_ploc_ownnn_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PLOC_OWNNN,
           APPN_PLOC_OWNNN_KEY, 
           APPN_PLOC_OWNNN_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_ploc_ownnn

/*
 *    [no] owning-cp-name <name>
 */
EOLS	  (cfg_appn_ploc_owncp_eol, appn_command, APPN_PLOC_OWNCP);
APPN_FQNAME (cfg_appn_ploc_owncp_num, cfg_appn_ploc_owncp_eol, no_alt,
           OBJ(string,1), APPN_PLOC_OWNCP_VAL_HELP);
NOPREFIX  (cfg_appn_ploc_owncp_noprefix, cfg_appn_ploc_owncp_num, cfg_appn_ploc_owncp_eol);
NVGENS    (cfg_appn_ploc_owncp_nvgen, cfg_appn_ploc_owncp_noprefix,
           appn_command, APPN_PLOC_OWNCP);
KEYWORD_ID(cfg_appn_ploc_owncp, cfg_appn_ploc_owncp_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_PLOC_OWNCP,
           APPN_PLOC_OWNCP_KEY, 
           APPN_PLOC_OWNCP_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_ploc_owncp

/*
 *   [no] wildcard
 */
EOLS      (cfg_appn_ploc_wild_eol, appn_command, APPN_PLOC_WILD);
KEYWORD_ID(cfg_appn_ploc_wild, cfg_appn_ploc_wild_eol, ALTERNATE,
           appn_subcommand_type, APPN_PLOC_WILD,
           APPN_PLOC_WILD_KEY, APPN_PLOC_WILD_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_ploc_wild
