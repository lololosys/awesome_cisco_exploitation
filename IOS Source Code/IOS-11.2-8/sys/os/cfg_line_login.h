/* $Id: cfg_line_login.h,v 3.3.48.1 1996/06/17 23:35:10 hampton Exp $
 * $Source: /release/112/cvs/Xsys/os/cfg_line_login.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ L O G I N . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_login.h,v $
 * Revision 3.3.48.1  1996/06/17  23:35:10  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.3  1995/12/07  17:45:22  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  18:45:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:50:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 * When old_access_control is FALSE:
 *
 *	authentication { default | <list-name> }
 *	no authentication
 *
 * OBJ(int,1) = AC_PASSWORD, AC_TACACS, AC_LOCAL
 */
EOLS	(cl_login_authen_eol, login_command, LINE_AUTHEN);
SET	(cl_login_authen_list_set, cl_login_authen_eol, OBJ(int,2), 2);
STRING	(cl_login_authen_list_name, cl_login_authen_list_set, no_alt,
	 OBJ(string,1), "Use an authentication list with this name.");
TESTVAR	(cl_login_authen_list_term, NONE, cl_login_authen_list_name,
	 NONE, NONE, NONE, OBJ(int,2), 1);
KEYWORD_ID (cl_login_authen_list, cl_login_authen_eol,
	    cl_login_authen_list_term, OBJ(int,2), 1,
	    "default", "Use the default authentication list.", PRIV_CONF);
NOPREFIX(cl_login_authen_nopre, cl_login_authen_list, cl_login_authen_eol);
NVGENS	(cl_login_authen_nvgen, cl_login_authen_nopre, login_command,
	 LINE_AUTHEN);
KEYWORD	(cl_login_authen, cl_login_authen_nvgen, no_alt,
	 "authentication", "Authentication parameters.", PRIV_CONF);


/************************************************************
 * When old_access_control is TRUE:
 *
 *	login [{ tacacs | local }]
 *	no login [{ tacacs | local }]
 *
 * OBJ(int,1) = AC_PASSWORD, AC_TACACS, AC_LOCAL
 */
EOLS	(cl_login_eol, login_command, LINE_LOGIN);
SET	(cl_login_default, cl_login_eol, OBJ(int,1), AC_PASSWORD);
KEYWORD_ID (cl_login_tacacs, cl_login_eol, cl_login_default,
	 OBJ(int,1), AC_TACACS,
	 "tacacs", "Use tacacs server for password checking", PRIV_CONF);
KEYWORD_ID (cl_login_local, cl_login_eol, cl_login_tacacs,
	OBJ(int,1), AC_LOCAL,
	"local", "Local password checking", PRIV_CONF);

TEST_BOOLEAN(cl_login_assert, cl_login_local, cl_login_authen, NONE,
	     old_access_control);
KEYWORD (cl_login, cl_login_assert, ALTERNATE,
	 "login", "Enable password checking", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cl_login
