/* $Id: cfg_line_arap.h,v 3.3.46.1 1996/06/04 00:19:52 snyder Exp $
 * $Source: /release/112/cvs/Xsys/arap/cfg_line_arap.h,v $
 *------------------------------------------------------------------
 * ARAP Config commands
 *
 * October 1993, Steven Lin
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_line_arap.h,v $
 * Revision 3.3.46.1  1996/06/04  00:19:52  snyder
 * CSCdi59389:  spellink errors
 * Branch: California_branch
 *
 * maximium -> maximum
 *
 * Revision 3.3  1995/12/07  17:44:52  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  08:39:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:34:53  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.1  1995/06/07  22:08:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * arap 
 * arap authentication [ default | name ] [ one-time ]
 * 
 */
EOLS	(cl_arap_authen_eol, arap_command, PARSER_ARAP_AUTHEN);

KEYWORD_ID (cl_arap_authen_final_otp, cl_arap_authen_eol, cl_arap_authen_eol, 
	    OBJ(int,4), 1,
	    "one-time", "Allow use of username*OTP for one-time passwords", 
	    PRIV_CONF);
SET	(cl_arap_authen_list_set, cl_arap_authen_final_otp, OBJ(int,2), 2);
STRING	(cl_arap_authen_list_name, cl_arap_authen_list_set, cl_arap_authen_eol,
	 OBJ(string,1), "Use an authentication list with this name.");
TEST_MULTIPLE_FUNCS (cl_arap_authen_list_term, cl_arap_authen_list_name,
		     NONE, no_alt);
KEYWORD_ID (cl_arap_authen_name_otp, cl_arap_authen_eol, 
	    cl_arap_authen_list_term,
	    OBJ(int,4), 1,
	    "one-time", "Allow use of username*OTP for one-time passwords", 
	    PRIV_CONF);
KEYWORD_ID (cl_arap_authen_list, cl_arap_authen_final_otp, 
	    cl_arap_authen_name_otp,
	    OBJ(int,2), 1,
	    "default", "Use the default authentication list.", PRIV_CONF);
NOPREFIX(cl_arap_authen_nopre, cl_arap_authen_list, cl_arap_authen_eol);
NVGENS	(cl_arap_authen_nvgen, cl_arap_authen_nopre, arap_command,
	 PARSER_ARAP_AUTHEN);
KEYWORD	(cl_arap_authen, cl_arap_authen_nvgen, no_alt, "authentication",
	 "Authentication parameters.", PRIV_CONF);

EOLS	(cl_arap_tacacs_eol, arap_command, PARSER_ARAP_TACACS);

KEYWORD_ID(cl_arap_tacacs_sl, cl_arap_tacacs_eol, cl_arap_tacacs_eol,
	   OBJ(int,1), TRUE, "single-line",
	   "Accept username and password in the username field.", PRIV_CONF);
NVGENS  (cl_arap_tacacs_nvgen, cl_arap_tacacs_sl, arap_command, 
	 PARSER_ARAP_TACACS);
KEYWORD	(cl_arap_tacacs_kwd, cl_arap_tacacs_nvgen, no_alt,
	 "use-tacacs", "Use TACACS to verify ARAP authentications", PRIV_CONF);
TEST_BOOLEAN(cl_arap_tacacs, cl_arap_tacacs_kwd, cl_arap_authen, NONE,
	     old_access_control);

PARAMS_KEYONLY(cl_arap_zonelist, cl_arap_tacacs, "zonelist",
		OBJ(int,1), 1, -1,
		arap_command, PARSER_ARAP_ZONELIST,
		"Zone access list",
		"Access list number", PRIV_CONF );

PARAMS_KEYONLY(cl_arap_warningtime, cl_arap_zonelist, "warningtime",
		OBJ(int,1), 1, -1,
		arap_command, PARSER_ARAP_WARNINGTIME,
		"Set connection disconnect warning time",
		"Warning time in minutes", PRIV_CONF );

PARAMS_KEYONLY(cl_arap_timelimit, cl_arap_warningtime, "timelimit",
		OBJ(int,1), 1, -1,
		arap_command, PARSER_ARAP_TIMELIMIT,
		"Set maximum connection time",
		"Timeout in minutes", PRIV_CONF );

EOLS	(cl_arap_manpassword_eol, arap_command, PARSER_ARAP_MANUALPASSWORD);
KEYWORD	(cl_arap_manpassword, cl_arap_manpassword_eol, cl_arap_timelimit,
	 "require-manual-password", "Require manual entry of ARAP passwords" , PRIV_CONF);

EOLS	(cl_arap_smartbuffer_eol, arap_command, PARSER_ARAP_NOSMARTBUFFER);
KEYWORD	(cl_arap_smartbuffer, cl_arap_smartbuffer_eol, cl_arap_manpassword,
	 "nosmartbuffer", "Turn off Smartbuffering" , PRIV_CONF|PRIV_HIDDEN);

EOLS	(cl_arap_guest_eol, arap_command, PARSER_ARAP_NOGUEST);
KEYWORD_ID(cl_arap_guest_ifneeded, cl_arap_guest_eol, cl_arap_guest_eol,
	 OBJ(int,1), PARSER_ARAP_NOGUEST_IFNEEDED, "if-needed", 
	 "Allow guest logins if pre-authenticated", PRIV_CONF);
NVGENS 	(cl_arap_guest_nvgens, cl_arap_guest_ifneeded, arap_command,
				PARSER_ARAP_NOGUEST);
KEYWORD	(cl_arap_guest_kwd, cl_arap_guest_nvgens, cl_arap_smartbuffer,
	 "noguest", "Disallow guest logins", PRIV_CONF);
TEST_BOOLEAN(cl_arap_guest, cl_arap_guest_kwd, cl_arap_smartbuffer, NONE,
	     old_access_control);

PARAMS_KEYONLY(cl_arap_netaccess, cl_arap_guest, "net-access-list",
		OBJ(int,1), 1, -1,
		arap_command, PARSER_ARAP_NETACCESSLIST,
		"Net access list",
		"Access list number", PRIV_CONF );

EOLS	(cl_arap_enable_eol, arap_command, PARSER_ARAP_ENABLED);
KEYWORD	(cl_arap_enable, cl_arap_enable_eol, cl_arap_netaccess,
	 "enable", "Enable ARAP connections", PRIV_CONF);

EOLS	(cl_arap_dedicated_eol, arap_command, PARSER_ARAP_DEDICATED);
KEYWORD	(cl_arap_dedicated, cl_arap_dedicated_eol, cl_arap_enable,
	 "dedicated", "Set line to only accept ARAP connections", PRIV_CONF);

KEYWORD	(cl_line_arap, cl_arap_dedicated, ALTERNATE,
	 "arap", "Appletalk Remote Access Protocol", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cl_line_arap
