/* $Id: exec_tn3270.h,v 3.2 1995/11/17 18:51:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tn3270/exec_tn3270.h,v $
 *------------------------------------------------------------------
 * TN3270 exec command
 *
 * January 1995, Robert Widmer
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_tn3270.h,v $
 * Revision 3.2  1995/11/17  18:51:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:37  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:27:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_EXIT(exec_tn3270_exit, no_alt);

/* tn3270 */
KEYWORD_ID(exec_tn3270, exec_tn3270_exit, ALTERNATE,
	   OBJ(int,9), CMD_CONNECT_TN3270,
	   "tn3270", "Open a tn3270 connection", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	exec_tn3270
