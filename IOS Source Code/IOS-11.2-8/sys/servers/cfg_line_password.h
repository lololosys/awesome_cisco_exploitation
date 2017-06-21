/* $Id: cfg_line_password.h,v 3.2.62.2 1996/06/05 23:12:28 carrel Exp $
 * $Source: /release/112/cvs/Xsys/servers/cfg_line_password.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ P A S S W O R D . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_password.h,v $
 * Revision 3.2.62.2  1996/06/05  23:12:28  carrel
 * CSCdi44545:  enable secret ? help is confusing to customers
 * Branch: California_branch
 * ^'enable'^line
 *
 * Revision 3.2.62.1  1996/06/05  21:13:07  carrel
 * CSCdi44545:  enable secret ? help is confusing to customers
 * Branch: California_branch
 * cleanup password usage (lines).
 *
 * Revision 3.2  1995/11/17  18:50:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/07  02:08:47  ahh
 * CSCdi36796:  typo in help for passwords
 * Add missing parenthesis.
 *
 * Revision 2.1  1995/07/04  01:56:42  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:10:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * password 0 <cleartext password>
 * password 7 <hidden password>
 * no password [ ... ]
 *
 * OBJ(int,1) = (encryption type), or -1 if no type given
 * OBJ(string,1) = <password>
 */
EOLS	(cl_pass_eol, password_line_command, LINE_PASSWORD);
TEXT    (cl_pass_passwd_enc, cl_pass_eol, no_alt,
         OBJ(string,1), "The HIDDEN line password string");
TEXT    (cl_pass_passwd, cl_pass_eol, no_alt,
         OBJ(string,1), "The UNENCRYPTED (cleartext) line password");
TEST_MULTIPLE_FUNCS(cl_pass_test2, cl_pass_passwd, no_alt, no_alt);
DECIMAL (cl_pass_enctype_other, cl_pass_passwd, cl_pass_test2,
         OBJ(int,1), 0, 9, "Encryption types not explicitly specified");
PRIV_TEST(cl_pass_enctype_hide, cl_pass_enctype_other,
          NONE, cl_pass_test2, PRIV_HIDDEN);     
TEST_MULTIPLE_FUNCS(cl_pass_test1, cl_pass_enctype_hide, no_alt, NONE);
KEYWORD_ID(cl_pass_enctype7, cl_pass_passwd_enc, cl_pass_test1,
           OBJ(int,1), ENCRYPT_TEMP,
           "7", "Specifies a HIDDEN password will follow", PRIV_CONF);
KEYWORD_ID(cl_pass_enctype0, cl_pass_passwd, cl_pass_enctype7,
           OBJ(int,1), ENCRYPT_NONE,
           "0", "Specifies an UNENCRYPTED password will follow", PRIV_CONF);
NOPREFIX (cl_pass_noprefix, cl_pass_enctype0, cl_pass_eol);
NVGENS	(cl_pass_nvgen, cl_pass_noprefix,
	 password_line_command, LINE_PASSWORD);
KEYWORD	(cl_pass, cl_pass_nvgen, ALTERNATE,
	 "password", "Set a password", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cl_pass
