/* 
 * $Id: cfg_appn_mode.h,v 3.1.40.1 1996/08/12 16:00:17 widmer Exp $
 * $Source: /release/112/cvs/Xsys/appn/cfg_appn_mode.h,v $
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
 * $Log: cfg_appn_mode.h,v $
 * Revision 3.1.40.1  1996/08/12  16:00:17  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:56:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/12/01  15:56:52  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  18:42:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:01:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
*/



/******************************************************************
 * appn mode <modename>
 *    ...subcommands...
 */


/*
 *   exit
 */
EOLNS	(appn_mode_exit_eol, exit_appn_command);
KEYWORD	(appn_mode_exit_kwd, appn_mode_exit_eol, NONE,
	 "exit", "Exit from appn-mode configuration mode", PRIV_CONF);
TESTVAR	(appn_mode_exit, NONE, appn_mode_exit_kwd, NONE, NONE, ALTERNATE,
	 sense, FALSE);
#undef  ALTERNATE
#define ALTERNATE appn_mode_exit

/*
 *   [no] complete
 */
EOLS      (cfg_appn_mode_comp_eol, appn_command, APPN_MODE_COMP);
KEYWORD_ID(cfg_appn_mode_comp, cfg_appn_mode_comp_eol, ALTERNATE,
           appn_subcommand_type, APPN_MODE_COMP,
           "complete", "Complete mode definition.", PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_mode_comp

/*
 *    [no] cos_name <name>
 */
EOLS	  (cfg_appn_mode_cos_eol, appn_command, APPN_MODE_COS);
APPN_TYPE_A_NAME (cfg_appn_mode_cos_num, cfg_appn_mode_cos_eol, no_alt,
           OBJ(string,1), APPN_MODE_COS_VAL_HELP);
NOPREFIX  (cfg_appn_mode_cos_noprefix, cfg_appn_mode_cos_num, cfg_appn_mode_cos_eol);
NVGENS    (cfg_appn_mode_cos_nvgen, cfg_appn_mode_cos_noprefix,
           appn_command, APPN_MODE_COS);
KEYWORD_ID(cfg_appn_mode_cos, cfg_appn_mode_cos_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_MODE_COS,
           APPN_MODE_COS_KEY, 
           APPN_MODE_COS_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_mode_cos
