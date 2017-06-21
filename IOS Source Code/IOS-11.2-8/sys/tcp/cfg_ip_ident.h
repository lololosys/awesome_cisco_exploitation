/* $Id: cfg_ip_ident.h,v 3.4 1996/02/10 03:04:49 tkolar Exp $
 * $Source: /release/111/cvs/Xsys/tcp/cfg_ip_ident.h,v $
 *------------------------------------------------------------------
 * C F G _ I P _ I D E N T . H
 *
 * Ident configuration commands
 *
 * September 1995, Tim Kolar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_ident.h,v $
 * Revision 3.4  1996/02/10  03:04:49  tkolar
 * CSCdi48619:  ip ident causes general_string_action warning
 * Add an NVGEN.
 *
 * Revision 3.3  1996/01/31  23:34:40  tkolar
 * CSCdi47936:  Customers want additional Ident functionality
 * Add an option to prevent username polling attacks.
 *
 * Revision 3.2  1995/11/17  18:47:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:32:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/10  20:36:55  tkolar
 * initial commit
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*************************************************************
 *
 *  [no] ip identd
 *
 */


EOLS (cfg_ip_ident_alwaysbe_eol, ip_ident_command, IP_IDENT_ALWAYSBE);

STRING (cfg_ip_ident_alwaysbe_string, cfg_ip_ident_alwaysbe_eol, no_alt,
	 OBJ(string,1), "String to return");

EOLS (cfg_ip_ident_eol, ip_ident_command, IP_IDENT);

KEYWORD (cfg_ip_ident_alwaysbe, cfg_ip_ident_alwaysbe_string, cfg_ip_ident_eol,
	 "always-be", "Always return a fixed string.",
	 PRIV_HIDDEN);

NVGENS  (cfg_ip_ident_nvgen, cfg_ip_ident_alwaysbe,
	 ip_ident_command, 0);

KEYWORD (cfg_ip_ident, cfg_ip_ident_nvgen, ALTERNATE,
	 "identd", "Ident server",
	 PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	cfg_ip_ident
