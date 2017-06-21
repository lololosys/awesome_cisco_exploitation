/* $Id: cfg_username.h,v 3.2.62.4 1996/08/12 16:07:08 widmer Exp $
 * $Source: /release/112/cvs/Xsys/servers/cfg_username.h,v $
 *------------------------------------------------------------------
 * C F G _ U S E R N A M E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_username.h,v $
 * Revision 3.2.62.4  1996/08/12  16:07:08  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.3  1996/08/09  21:18:20  billw
 * CSCdi65014:  conflict in username parsing
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/06/05  18:19:33  carrel
 * CSCdi44545:  enable secret ? help is confusing to customers
 * Branch: California_branch
 * Cleaned up usage of passwords (enable pass, enable secret and username)
 * throughout.
 *
 * Revision 3.2.62.1  1996/05/29  00:18:02  swenson
 * CSCdi58793:  conflict in parsing of username command
 * Branch: California_branch
 * use different variables for privilege and callback options
 *
 * Revision 3.2  1995/11/17  18:50:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:44  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:14:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 * username name			string1
 *   access-class <1-199>		int1=TRUE, int2=list
 *   autocommand <line>			int3=TRUE, string2=command
 *   callback-dialstring <string>	int10=TRUE, string4=string
 *   callback-line <n>			int11=TRUE, int13=line
 *   callback-rotary <n>		int12=TRUE, int13=rotary
 *   nocallback-verify			int9=TRUE
 *   noescape				int5=TRUE
 *   nohangup				int8=TRUE
 *   nopassword				int4=TRUE
 *   password <line>			int6=TRUE, int7=encryp, string3=word
 *   privilege <0-15>			int14=TRUE, int15=level
 */
EOLNS	(cl_username_eol, username_command);

pdecl(cl_username_options);

TTYLINE(cl_username_callback_line_num, cl_username_options, no_alt,
	OBJ(int,13), "TTY line number");
KEYWORD_ID(cl_username_callback_line, cl_username_callback_line_num, 
           cl_username_eol,
	   OBJ(int,12), TRUE,
	   "callback-line", "Associate a specific line with this callback",
 	   PRIV_CONF);

NUMBER	(cl_username_callback_rotary_num, cl_username_options, no_alt,
	 OBJ(int,13), 0, 100, "Rotary group number" );
KEYWORD_ID(cl_username_callback_rotary, cl_username_callback_rotary_num, 
           cl_username_callback_line,
	   OBJ(int,11), TRUE,
	   "callback-rotary", "Associate a rotary group with this callback",
	   PRIV_CONF);

STRING	(cl_username_callback_dialstring_string, cl_username_options, no_alt,
	 OBJ(string,4), "Dial-string");
KEYWORD_ID(cl_username_callback_dialstring, 
           cl_username_callback_dialstring_string, 
           cl_username_callback_rotary,
	   OBJ(int,10), TRUE,
	   "callback-dialstring", "Callback dialstring",
	   PRIV_CONF);

KEYWORD_ID(cl_username_nocallback_verify, cl_username_options,  
           cl_username_callback_dialstring, 
	   OBJ(int,9), TRUE,
	   "nocallback-verify", "Do not require authentication after callback",
	   PRIV_CONF);

KEYWORD_ID(cl_username_nohangup, cl_username_options, 
           cl_username_nocallback_verify,
	   OBJ(int,8), TRUE,
	   "nohangup", "Do not disconnect after an automatic command",
	   PRIV_CONF);

TEXT    (cl_username_password_passwd_enc, cl_username_eol, no_alt,
         OBJ(string,3), "The HIDDEN user password string");
TEXT    (cl_username_password_passwd, cl_username_eol, no_alt,
         OBJ(string,3), "The UNENCRYPTED (cleartext) user password");
TEST_MULTIPLE_FUNCS(cl_username_password_test2, cl_username_password_passwd,
		    no_alt, NONE);
DECIMAL	(cl_username_password_enctype_other, cl_username_password_passwd,
	 cl_username_password_test2,
	 OBJ(int,7), 0, 9, "Encryption types not explicitly specified");
PRIV_TEST(cl_username_password_enctype_hide,
	  cl_username_password_enctype_other,
	  NONE, cl_username_password_test2, PRIV_HIDDEN);	  
TEST_MULTIPLE_FUNCS(cl_username_password_test1,
		    cl_username_password_enctype_hide, no_alt, NONE);
KEYWORD_ID(cl_username_password_enctype7, cl_username_password_passwd_enc,
	   cl_username_password_test1,
	   OBJ(int,7), ENCRYPT_TEMP,
	   "7", "Specifies a HIDDEN password will follow", PRIV_CONF);
KEYWORD_ID(cl_username_password_enctype0, cl_username_password_passwd,
           cl_username_password_enctype7,
           OBJ(int,7), ENCRYPT_NONE,
           "0", "Specifies an UNENCRYPTED password will follow", PRIV_CONF);
KEYWORD_ID(cl_username_password, cl_username_password_enctype0,
	   cl_username_nohangup,
	   OBJ(int,6), TRUE,
	   "password", "Specify the password for the user", PRIV_CONF);

KEYWORD_ID(cl_username_noescape, cl_username_options, cl_username_password,
	   OBJ(int,5), TRUE,
	   "noescape", "Prevent the user from using an escape character",
	   PRIV_CONF);

KEYWORD_ID(cl_username_nopassword, cl_username_options, cl_username_noescape,
	   OBJ(int,4), TRUE,
	   "nopassword", "No password is required for the user to log in",
	   PRIV_CONF);

TEXT	(cl_username_autocommand_string, cl_username_eol, no_alt,
	 OBJ(string,2),
	 "Command to be automatically issued after the user logs in");
KEYWORD_ID(cl_username_autocommand, cl_username_autocommand_string,
	   cl_username_nopassword,
	   OBJ(int,3), TRUE,
	   "autocommand",
	   "Automatically issue a command after the user logs in",
	   PRIV_CONF);

NUMBER	(cl_username_priv_level, cl_username_options, no_alt,
	 OBJ(int,15), PRIV_MIN, PRIV_MAX, "User privilege level");
KEYWORD_ID(cl_username_priv, cl_username_priv_level, cl_username_autocommand,
	   OBJ(int,14), TRUE,
	   "privilege", "Set user privilege level", PRIV_CONF);

NUMBER	(cl_username_access_num, cl_username_options, no_alt,
	 OBJ(int,2), MINFASTACC, MAXSLOWACC, "Access-class number");
KEYWORD_ID(cl_username_access, cl_username_access_num, cl_username_priv,
	   OBJ(int,1), TRUE,
	   "access-class", "Restrict access by access-class", PRIV_CONF);

NOP	(cl_username_options, cl_username_access, NONE);

NOPREFIX(cl_username_noprefix, cl_username_options, cl_username_eol);

STRING	(cl_username_name, cl_username_noprefix, no_alt,
	 OBJ(string,1), "User name");
NVGENNS	(cl_username_nvgens, cl_username_name, username_command);
KEYWORD	(cl_username, cl_username_nvgens, ALTERNATE,
	 "username", "Establish User Name Authentication", PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	cl_username
