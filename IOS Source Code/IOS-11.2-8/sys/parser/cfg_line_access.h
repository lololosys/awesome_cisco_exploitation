/* $Id: cfg_line_access.h,v 3.2 1995/11/17 18:44:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_access.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ A C C E S S . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_access.h,v $
 * Revision 3.2  1995/11/17  18:44:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:08:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * access-class <list> { in | out }
 * no access-class <list> { in | out }
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = IN, OUT
 */
EOLS	(cl_access_eol, line_command, LINE_ACCESS_CLASS);
KEYWORD_ID (cl_access_out, cl_access_eol, no_alt,
	    OBJ(int,2), OUT,
	    "out", "Filter outgoing connections", PRIV_CONF);

KEYWORD_ID (cl_access_in, cl_access_eol, cl_access_out,
	    OBJ(int,2), IN,
	    "in", "Filter incoming connections", PRIV_CONF);

TESTVAR	(cl_access_nolist, cl_access_in, no_alt,
	 NONE, NONE, NONE, sense, FALSE);
NUMBER	(cl_access_list, cl_access_in, cl_access_nolist,
	 OBJ(int,1), MINFASTACC, MAXSLOWACC,
	 "IP access list");

NVGENS	(cl_access_nvgen, cl_access_list, line_command, LINE_ACCESS_CLASS);
KEYWORD	(cl_access, cl_access_nvgen, ALTERNATE,
	 "access-class", "Filter connections based on an IP access list",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cl_access
