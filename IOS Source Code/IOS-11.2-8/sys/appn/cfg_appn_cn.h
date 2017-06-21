/* 
 * $Id: cfg_appn_cn.h,v 3.1.40.1 1996/08/12 16:00:13 widmer Exp $
 * $Source: /release/112/cvs/Xsys/appn/cfg_appn_cn.h,v $
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
 * $Log: cfg_appn_cn.h,v $
 * Revision 3.1.40.1  1996/08/12  16:00:13  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:56:44  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/12/01  15:56:46  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  18:42:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:01:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
*/



/******************************************************************
 * appn connection-network <cnname>
 *    ...subcommands...
 */


/*
 *   exit
 */
EOLNS	(appn_cn_exit_eol, exit_appn_command);
KEYWORD	(appn_cn_exit_kwd, appn_cn_exit_eol, NONE,
	 "exit", "Exit from appn-conn-net configuration mode", PRIV_CONF);
TESTVAR	(appn_cn_exit, NONE, appn_cn_exit_kwd, NONE, NONE, ALTERNATE,
	 sense, FALSE);
#undef  ALTERNATE
#define ALTERNATE appn_cn_exit

/*
 *   [no] complete
 */
EOLS      (cfg_appn_cn_comp_eol, appn_command, APPN_CN_COMP);
KEYWORD_ID(cfg_appn_cn_comp, cfg_appn_cn_comp_eol, ALTERNATE,
           appn_subcommand_type, APPN_CN_COMP,
           "complete", "Complete Connection Network definition.", PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cn_comp

/*
 *    [no] port <name>
 */
EOLS	  (cfg_appn_cn_port_eol, appn_command, APPN_CN_PORT);
APPN_TYPE_A_NAME (cfg_appn_cn_port_name, cfg_appn_cn_port_eol, 
           no_alt,
           OBJ(string,1), APPN_CN_PORT_VAL_HELP);
NVGENS    (cfg_appn_cn_port_nvgen, cfg_appn_cn_port_name,
           appn_command, APPN_CN_PORT);
KEYWORD_ID(cfg_appn_cn_port, cfg_appn_cn_port_nvgen,ALTERNATE,
           appn_subcommand_type, APPN_CN_PORT,
           APPN_CN_PORT_KEY, APPN_CN_PORT_KEY_HELP, PRIV_CONF);
#undef  ALTERNATE
#define ALTERNATE cfg_appn_cn_port
