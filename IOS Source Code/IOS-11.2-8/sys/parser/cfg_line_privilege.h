/* $Id: cfg_line_privilege.h,v 3.2 1995/11/17 18:44:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_privilege.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ P R I V I L E G E . H
 *
 * Line privilege command
 *
 * November 1994, Robert Widmer
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_line_privilege.h,v $
 * Revision 3.2  1995/11/17  18:44:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:10:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

PARAMS_KEYONLY(cl_privilege_level, NONE, "level",
	       OBJ(int,1), 0, 15,
	       line_command, PARSER_LINE_PRIVILEGE,
	       "Assign default privilege level for line",
	       "Default privilege level for line", PRIV_CONF);

KEYWORD	(cl_privilege, cl_privilege_level, ALTERNATE,
	 "privilege", "Change privilege level for line", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cl_privilege
