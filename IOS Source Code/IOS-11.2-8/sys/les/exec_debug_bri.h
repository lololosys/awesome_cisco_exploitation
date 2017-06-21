/* $Id: exec_debug_bri.h,v 3.1 1995/11/09 12:29:49 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/exec_debug_bri.h,v $
 *------------------------------------------------------------------
 * exec_debug_bri.h -- DEBUG command parsing code for bri.
 *
 * March 1994, Manoj Leelanivas.
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_debug_bri.h,v $
 * Revision 3.1  1995/11/09  12:29:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:32  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/19  14:47:21  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 * 
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug bri_interface
 *
 */
EOLS	(debug_bri_interface_eol, debug_command, DEBUG_BRI);
KEYWORD_DEBUG(debug_bri_interface, debug_bri_interface_eol, NONE,
	      OBJ(pdb,1), bri_debug_arr,
	      "bri-interface", "bri network interface events", PRIV_OPR);

TEST_INT(debug_bri, debug_bri_interface, NONE, ALTERNATE, nbrinets);

#undef	ALTERNATE
#define	ALTERNATE	debug_bri_interface


