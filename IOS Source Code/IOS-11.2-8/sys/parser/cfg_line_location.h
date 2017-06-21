/* $Id: cfg_line_location.h,v 3.2 1995/11/17 18:44:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_location.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ L O C A T I O N . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_location.h,v $
 * Revision 3.2  1995/11/17  18:44:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:09:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************
 *	location <text>
 *	no location [<text>]
 */
EOLS	(location_eol, line_command, LINE_LOC );
TEXT	(location_text, location_eol, no_alt,
	 OBJ(string,1), "One text line describing the terminal's location");
NOPREFIX (cl_location_noprefix, location_text, location_eol);
NVGENS	(cl_location_nvgen, cl_location_noprefix, line_command, LINE_LOC );
KEYWORD	(cl_location, cl_location_nvgen, ALTERNATE,
	 "location", "Enter terminal location description", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cl_location
