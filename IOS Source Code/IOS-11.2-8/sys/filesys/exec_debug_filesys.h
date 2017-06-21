/* $Id: exec_debug_filesys.h,v 3.2 1995/11/17 09:07:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/exec_debug_filesys.h,v $
 *------------------------------------------------------------------
 * Debug command parser tree
 *
 * Oct 1995, Yan Ke
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_filesys.h,v $
 * Revision 3.2  1995/11/17  09:07:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:52  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/17  17:46:25  yanke
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug filesys
 *
 */
EOLS (debug_filesys_eol, debug_command, DEBUG_FILESYS);

KEYWORD_DEBUG (debug_filesys, debug_filesys_eol, ALTERNATE,
	OBJ(pdb,1), filesys_debug_arr,
	"filesys", "File system information", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE       debug_filesys
