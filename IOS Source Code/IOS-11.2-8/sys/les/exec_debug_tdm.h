/* $Id: exec_debug_tdm.h,v 3.1.40.1 1996/06/16 21:14:17 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_debug_tdm.h,v $
 *------------------------------------------------------------------
 * exec_debug_tdm.h - TDM debug routines.
 *
 * February 1996, Joe Turner
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_tdm.h,v $
 * Revision 3.1.40.1  1996/06/16  21:14:17  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/02/02  22:50:13  jturner
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug tdm
 *
 */
EOLS (debug_tdm_eol, debug_command, DEBUG_TDM);
KEYWORD_DEBUG(debug_tdm, debug_tdm_eol, ALTERNATE,
	      OBJ(pdb,1), tdm_debug_arr,
	      "tdm", "TDM connection information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	debug_tdm
