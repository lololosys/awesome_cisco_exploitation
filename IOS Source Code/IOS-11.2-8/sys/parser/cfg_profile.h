/* $Id: cfg_profile.h,v 3.3.28.1 1996/08/12 16:05:51 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_profile.h,v $
 *------------------------------------------------------------------
 * C F G _ P R O F I L E . H
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_profile.h,v $
 * Revision 3.3.28.1  1996/08/12  16:05:51  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/22  06:57:48  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:44:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/29  04:02:25  pst
 * CSCdi36559:  clean up hidden command help in parser
 *
 * Revision 2.1  1995/06/07  22:11:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * [no] profile <start_addr> <end_addr> <increment>
 *
 * OBJ(int,1) = FALSE;
 * OBJ(int,2) = start address
 * OBJ(int,3) = end address
 * OBJ(int,4) = increment
 * csb->which = PROFILE_EXEC
 */

EOLS    (cfg_profile_eol, profile_command, PROFILE_CONFIG);
HEXNUM  (cfg_profile_increment, cfg_profile_eol, no_alt,
	 OBJ(int,4), "increment");
HEXNUM  (cfg_profile_end, cfg_profile_increment, no_alt,
	 OBJ(int,3), "end addr");
HEXNUM  (cfg_profile_start, cfg_profile_end, no_alt, OBJ(int,2), "start addr");
NVGENNS  (cfg_profile_nvgen, cfg_profile_start, profile_nvgens);
KEYWORD (cfg_profile, cfg_profile_nvgen, ALTERNATE,
	 "profile", NULL, PRIV_CONF | PRIV_NOHELP);

#undef	ALTERNATE
#define	ALTERNATE	cfg_profile
