/* $Id: exec_debug_appn.h,v 3.1 1996/02/01 04:56:51 hampton Exp $
 * $Source: /release/111/cvs/Xsys/appn/exec_debug_appn.h,v $
 *------------------------------------------------------------------
 * exec_debug_appn.h -- Contains the APPN debug commands
 *
 * April 15, 1994 - Dave McCowan
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_appn.h,v $
 * Revision 3.1  1996/02/01  04:56:51  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:46:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:47:08  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/******************************************************************
 * [no] debug appn ALL
 *                 RM                  
 *     		   TRS                 
 *     		   PS                  
 *     		   SS                  
 *     		   DS                  
 *     		   PC                  
 *     		   SCM                 
 *     		   NOF                 
 *     		   CS                  
 *     		   SM                  
 *     		   HS                  
 *     		   DLC                 
 *     		   MS                  
 *     		   EGPE_TIMER          
 *     		   EGPE_NONTIMER       
 *     		   SSA                 
 *     		   LSR                 
 *                 API LU TRACE ACS PSNAV DLUR
 */

EOLS	(debug_appn_eol, debug_appn_commands, CMD_DEBUGAPPN);

KEYWORD_ID(debug_appn_all, debug_appn_eol, no_alt,
	   OBJ(int,1), DEBUG_ALL,
	   "all", "Enable all APPN debugging", PRIV_USER);
KEYWORD_ID(debug_appn_rm, debug_appn_eol, debug_appn_all,
	   OBJ(int,1), DEBUG_RM,
	   "rm", "Resource Manager", PRIV_USER);
KEYWORD_ID(debug_appn_trs, debug_appn_eol, debug_appn_rm,
	   OBJ(int,1), DEBUG_TRS,
	   "trs", "Topology & Routing Services", PRIV_USER);
KEYWORD_ID(debug_appn_ps, debug_appn_eol, debug_appn_trs,
	   OBJ(int,1), DEBUG_PS,
	   "ps", "Presentation Services", PRIV_USER);
KEYWORD_ID(debug_appn_ss, debug_appn_eol, debug_appn_ps,
	   OBJ(int,1), DEBUG_SS,
	   "ss", "Session Services", PRIV_USER);
KEYWORD_ID(debug_appn_ds, debug_appn_eol, debug_appn_ss,
	   OBJ(int,1), DEBUG_DS,
	   "ds", "Directory Services", PRIV_USER);
KEYWORD_ID(debug_appn_pc, debug_appn_eol, debug_appn_ds,
	   OBJ(int,1), DEBUG_PC,
	   "pc", "Path Control", PRIV_USER);
KEYWORD_ID(debug_appn_scm, debug_appn_eol, debug_appn_pc,
	   OBJ(int,1), DEBUG_SCM,
	   "scm", "Session Connector Manager ", PRIV_USER);
KEYWORD_ID(debug_appn_nof, debug_appn_eol, debug_appn_scm,
	   OBJ(int,1), DEBUG_NOF,
	   "nof", "Node Operator Facility", PRIV_USER);
KEYWORD_ID(debug_appn_cs, debug_appn_eol, debug_appn_nof,
	   OBJ(int,1), DEBUG_CS,
	   "cs", "Configuration Services", PRIV_USER);
KEYWORD_ID(debug_appn_sm, debug_appn_eol, debug_appn_cs,
	   OBJ(int,1), DEBUG_SM,
	   "sm", "Session Manager", PRIV_USER);
KEYWORD_ID(debug_appn_hs, debug_appn_eol, debug_appn_sm,
	   OBJ(int,1), DEBUG_HS,
	   "hs", "Half Session", PRIV_USER);
KEYWORD_ID(debug_appn_dlc, debug_appn_eol, debug_appn_hs,
	   OBJ(int,1), DEBUG_DLC,
	   "dlc", "Data Link Control", PRIV_USER);
KEYWORD_ID(debug_appn_ms, debug_appn_eol, debug_appn_dlc,
	   OBJ(int,1), DEBUG_MS,
	   "ms", "Management Services", PRIV_USER);
KEYWORD_ID(debug_appn_egpe_timer, debug_appn_eol, debug_appn_ms,
	   OBJ(int,1), DEBUG_EGPE_TIMER,
	   "egpe_timer", "", PRIV_USER);
KEYWORD_ID(debug_appn_egpe_nontimer, debug_appn_eol, debug_appn_egpe_timer,
	   OBJ(int,1), DEBUG_EGPE_NONTIMER,
	   "egpe_nontimer", "", PRIV_USER);
KEYWORD_ID_MM(debug_appn_ssa, debug_appn_eol, debug_appn_egpe_nontimer,
	   OBJ(int,1), DEBUG_SSA,
	   "ssa", "", PRIV_USER, 3);
KEYWORD_ID(debug_appn_lsr, debug_appn_eol, debug_appn_ssa,
	   OBJ(int,1), DEBUG_LSR,
	   "lsr", "", PRIV_USER);
KEYWORD_ID(debug_appn_api, debug_appn_eol, debug_appn_lsr,
	   OBJ(int,1), DEBUG_API,
	   "api", "Application Programming Interface", PRIV_USER);
KEYWORD_ID(debug_appn_lu, debug_appn_eol, debug_appn_api,
	   OBJ(int,1), DEBUG_LU,
	   "lu", "", PRIV_USER);
KEYWORD_ID(debug_appn_trace, debug_appn_eol, debug_appn_lu,
	   OBJ(int,1), DEBUG_MODULE_TRACE,
	   "trace", "", PRIV_USER);
KEYWORD_ID(debug_appn_acs, debug_appn_eol, debug_appn_trace,
	   OBJ(int,1), DEBUG_ACS,
	   "acs", "", PRIV_USER);
KEYWORD_ID_MM(debug_appn_psnav, debug_appn_eol, debug_appn_acs,
	   OBJ(int,1), DEBUG_PSNAV,
	   "psnav", "", PRIV_USER,3);
KEYWORD_ID(debug_appn_dlur, debug_appn_eol, debug_appn_psnav,
	   OBJ(int,1), DEBUG_DLUR,
	   "dlur", "Dependant LU Requester", PRIV_USER);
KEYWORD (debug_appn, debug_appn_dlur, ALTERNATE,
	 "appn", "APPN debug commands", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	debug_appn
