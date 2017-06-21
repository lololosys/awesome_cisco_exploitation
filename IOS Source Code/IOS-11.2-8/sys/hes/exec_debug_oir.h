/* $Id: exec_debug_oir.h,v 3.2 1995/11/17 09:15:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/exec_debug_oir.h,v $
 *------------------------------------------------------------------
 * exec_debug_oir.h - Debugging for Online Insertion & Removal
 *
 * November 1994, William H. Palmer
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_oir.h,v $
 * Revision 3.2  1995/11/17  09:15:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:53  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/19  14:48:32  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug oir
 *
 */
EOLS	(debug_oir_eol, debug_command, DEBUG_OIR);
KEYWORD_DEBUG(debug_oir, debug_oir_eol, ALTERNATE,
	      OBJ(pdb,1), oir_debug_arr,
	      "oir", "Online Insertion and Removal information",
	      PRIV_OPR|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	debug_oir
