/* $Id: exec_debug_standby.h,v 3.2 1995/11/17 17:33:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/exec_debug_standby.h,v $
 *------------------------------------------------------------------
 * exec_debug_standby.h -- debug commands for hot standby protocol
 *
 * Sep 1993 Tony Li
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_standby.h,v $
 * Revision 3.2  1995/11/17  17:33:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:16  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.2  1995/06/19  14:49:08  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug standby
 *
 */
EOLS	(debug_standby_eol, debug_command, DEBUG_STANDBY);
KEYWORD_DEBUG(debug_standby, debug_standby_eol, ALTERNATE,
	      OBJ(pdb,1), iprouting_debug_arr,
	      "standby", "Hot standby protocol", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_standby
