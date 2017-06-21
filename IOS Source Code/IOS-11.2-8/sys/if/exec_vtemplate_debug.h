/* $Id: exec_vtemplate_debug.h,v 3.1.34.1 1996/04/27 05:49:39 syiu Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_vtemplate_debug.h,v $
 *------------------------------------------------------------------
 * exec_vtemplate_debug.h - Debug support for Virtual Template
 * 
 * February 1996, Perry Leung 
 *
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_vtemplate_debug.h,v $
 * Revision 3.1.34.1  1996/04/27  05:49:39  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.2.1  1996/02/10  02:19:43  perryl
 * Branch: Dial1_branch
 * Add debug support for Virtual Template
 *
 * Revision 3.1  1996/02/10  00:27:13  perryl
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug vtemplate
 *
 */
EOLS	(debug_vtemplate_eol, debug_command, DEBUG_VTEMPLATE);
KEYWORD_DEBUG(debug_vtemplate, debug_vtemplate_eol, ALTERNATE,
	      OBJ(pdb,1), vtemplate_debug_arr,
	      "vtemplate", "Virtual Template information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_vtemplate
