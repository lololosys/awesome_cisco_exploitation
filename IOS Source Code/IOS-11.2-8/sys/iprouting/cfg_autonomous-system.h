/* $Id: cfg_autonomous-system.h,v 3.2 1995/11/17 17:31:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/cfg_autonomous-system.h,v $
 *------------------------------------------------------------------
 * C F G _ A U T O N O M O U S _ S Y S T E M . H
 *
 * Autonomous system command
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_autonomous-system.h,v $
 * Revision 3.2  1995/11/17  17:31:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/19  10:50:27  tli
 * CSCdi36065:  Rationalize EGP parser file placement
 * Move parser support to iprouting subdirectory.
 *
 * Revision 2.1  1995/06/07  22:01:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *	[no] autonomous-system <local-as>
 */
PARAMS_KEYONLY(config_autosys, ALTERNATE, "autonomous-system",
	       OBJ(int,1), 1, 65535,
	       autonomous_command, 0,
	       "Specify local AS number to which we belong",
	       "Local AS number", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	config_autosys
