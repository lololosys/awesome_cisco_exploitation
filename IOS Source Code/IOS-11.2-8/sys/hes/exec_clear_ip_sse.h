/* $Id: exec_clear_ip_sse.h,v 3.2 1995/11/17 09:15:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/exec_clear_ip_sse.h,v $
 *------------------------------------------------------------------
 * exec_clear_ip_sse.h
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_ip_sse.h,v $
 * Revision 3.2  1995/11/17  09:15:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:52  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:16:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ip sse
 *
 */
EOLS	(clear_ip_sse_eol, clear_sse, SSE_PROTOCOL_IP);
KEYWORD_ID (clear_ip_sse, clear_ip_sse_eol, ALTERNATE,
	    OBJ(int,1), 0,
	    "sse", "Delete SSE cache table entries", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE 	clear_ip_sse
