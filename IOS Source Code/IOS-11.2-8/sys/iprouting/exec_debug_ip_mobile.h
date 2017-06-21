/* $Id: exec_debug_ip_mobile.h,v 3.2 1995/11/17 17:33:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/exec_debug_ip_mobile.h,v $
 *------------------------------------------------------------------
 * exec_debug_ip_mobile.h -- Debugging for IP mobility
 *
 * Nov 1993 Tony Li
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_mobile.h,v $
 * Revision 3.2  1995/11/17  17:33:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:13  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.2  1995/06/19  14:48:06  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:18:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ip mobile
 *
 */
EOLS	(debug_ip_mobile_eol, debug_command, DEBUG_IP_MOBILE);


KEYWORD_DEBUG(debug_ip_mobile, debug_ip_mobile_eol, ALTERNATE,
	      OBJ(pdb,1), iprouting_debug_arr,
	      "mobile", "Mobility protocols", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_mobile
