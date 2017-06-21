/* $Id: exec_debug_domain.h,v 3.2 1995/11/17 09:32:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/exec_debug_domain.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ D O M A I N . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *
 *------------------------------------------------------------------
 * $Log: exec_debug_domain.h,v $
 * Revision 3.2  1995/11/17  09:32:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:41  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/19  14:47:53  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:18:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug domain
 *
 */
EOLS	(debug_domain_eol, debug_command, DEBUG_DOMAIN);
KEYWORD_DEBUG(debug_domain, debug_domain_eol, ALTERNATE,
	      OBJ(pdb,1), ip_debug_arr,
	      "domain", "Domain Name System", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_domain
