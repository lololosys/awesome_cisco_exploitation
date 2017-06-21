/* $Id: cfg_enable.h,v 3.4.20.2 1996/08/12 16:07:06 widmer Exp $
 * $Source: /release/112/cvs/Xsys/servers/cfg_enable.h,v $
 *------------------------------------------------------------------
 * C F G _ E N A B L E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_enable.h,v $
 * Revision 3.4.20.2  1996/08/12  16:07:06  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.20.1  1996/06/05  18:19:30  carrel
 * CSCdi44545:  enable secret ? help is confusing to customers
 * Branch: California_branch
 * Cleaned up usage of passwords (enable pass, enable secret and username)
 * throughout.
 *
 * Revision 3.4  1996/02/01  23:34:25  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1995/12/07  17:45:30  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  18:50:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/03  01:23:34  carrel
 * CSCdi41368:  no enable secret not functioning
 * Changed NOPREFIX to a TESTVAR.
 *
 * Revision 2.1  1995/07/04  01:56:42  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:02:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * [no] enable password [level <level>] <cleartext password>
 * [no] enable password [level <level>] 0 <cleartext password>
 * [no] enable password [level <level>] 7 <hidden password>
 *
 * The 'no' version of this command clears any existing password.
 *
 * OBJ(int,1) = [0-7] (encryption type), or -1 if no type given
 * OBJ(string,1) = <password>
 *
 * The parse chain here is a bit strange.  Initialize OBJ(int,1) to -1
 * then only use TESTVAR to determine if we've parsed an encryption
 * type.  This prevents TEXT (and EOLS) from being encountered twice, 
 * once if DECIMAL accepts and once as the alternate from DECIMAL.
 */
EOLS	(conf_enable_pass_eol, configure_enable, ENABLE_PASSWORD);
TEXT	(conf_enable_pass_passwd_enc, conf_enable_pass_eol, no_alt,
	 OBJ(string,1), "The HIDDEN 'enable' password string");
TEXT	(conf_enable_pass_passwd, conf_enable_pass_eol, no_alt,
	 OBJ(string,1), "The UNENCRYPTED (cleartext) 'enable' password");
TEST_MULTIPLE_FUNCS(conf_enable_pass_test2, conf_enable_pass_passwd,
		    no_alt, NONE);
DECIMAL	(conf_enable_pass_enctype_other, conf_enable_pass_passwd,
	 conf_enable_pass_test2,
	 OBJ(int,1), 0, 9, "Encryption types not explicitly specified");
PRIV_TEST(conf_enable_pass_enctype_hide, conf_enable_pass_enctype_other,
	  NONE, conf_enable_pass_test2, PRIV_HIDDEN);	  
TEST_MULTIPLE_FUNCS(conf_enable_pass_test1,
		    conf_enable_pass_enctype_hide, no_alt, NONE);
KEYWORD_ID(conf_enable_pass_enctype7, conf_enable_pass_passwd_enc,
	   conf_enable_pass_test1,
	   OBJ(int,1), ENCRYPT_TEMP,
	   "7", "Specifies a HIDDEN password will follow", PRIV_CONF);
KEYWORD_ID(conf_enable_pass_enctype0, conf_enable_pass_passwd,
	   conf_enable_pass_enctype7,
	   OBJ(int,1), ENCRYPT_NONE,
	   "0", "Specifies an UNENCRYPTED password will follow", PRIV_CONF);
TESTVAR	(conf_enable_pass_noprefix, conf_enable_pass_eol, NONE,
	 NONE, NONE, conf_enable_pass_enctype0, sense, FALSE);

SET	(conf_enable_pass_defaultlevel, conf_enable_pass_noprefix,
	 OBJ(int,2), PRIV_MAX);
NUMBER	(conf_enable_pass_level_num, conf_enable_pass_noprefix, no_alt,
	 OBJ(int,2), PRIV_MIN+1, PRIV_MAX, "Level number");
KEYWORD	(conf_enable_pass_level, conf_enable_pass_level_num,
	 conf_enable_pass_defaultlevel,
	 "level", "Set exec level password", PRIV_ROOT);

NVGENS	(conf_enable_pass_nvgen, conf_enable_pass_level, 
         configure_enable, ENABLE_PASSWORD);
KEYWORD	(conf_enable_pass, conf_enable_pass_nvgen, no_alt,
	 "password", "Assign the privileged level password", PRIV_CONF);

/******************************************************************
 * [no] enable secret [level <level>] <password>
 * [no] enable secret [level <level>] 5 <encrypted password>
 *
 * The 'no' version of this command clears any existing secret.
 *
 * OBJ(int,1) = [0-7] (encryption type), or ENCRYPT_NONE if no type given
 * OBJ(string,1) = <secret>
 *
 * The parse chain here is a bit strange.  Initialize OBJ(int,1) to
 * ENCRYPT_NONE then only use TESTVAR to determine if we've parsed an
 * encryption type.  This prevents TEXT (and EOLS) from being encountered
 * twice, once if DECIMAL accepts and once as the alternate from DECIMAL.
 */
EOLS	(conf_enable_secret_eol, configure_enable, ENABLE_SECRET);
TEXT	(conf_enable_secret_passwd_enc, conf_enable_secret_eol, no_alt,
	 OBJ(string,1), "The ENCRYPTED 'enable' secret string");
TEXT	(conf_enable_secret_passwd, conf_enable_secret_eol, no_alt,
	 OBJ(string,1), "The UNENCRYPTED (cleartext) 'enable' secret");
TEST_MULTIPLE_FUNCS(conf_enable_secret_test2, conf_enable_secret_passwd,
		    no_alt, NONE);
DECIMAL	(conf_enable_secret_enctype_other, conf_enable_secret_passwd,
	 conf_enable_secret_test2,
	 OBJ(int,1), 0, 9, "Encryption types not explicitly specified");
PRIV_TEST(conf_enable_secret_enctype_hide, conf_enable_secret_enctype_other,
	  NONE, conf_enable_secret_test2, PRIV_HIDDEN);	  
TEST_MULTIPLE_FUNCS(conf_enable_secret_test1,
		    conf_enable_secret_enctype_hide, no_alt, NONE);
KEYWORD_ID(conf_enable_secret_enctype5, conf_enable_secret_passwd_enc,
	   conf_enable_secret_test1, OBJ(int,1), ENCRYPT_MD5,
	   "5", "Specifies an ENCRYPTED secret will follow", PRIV_CONF);
KEYWORD_ID(conf_enable_secret_enctype0, conf_enable_secret_passwd,
	   conf_enable_secret_enctype5,
	   OBJ(int,1), ENCRYPT_NONE,
	   "0", "Specifies an UNENCRYPTED password will follow", PRIV_CONF);
SET	(conf_enable_secret_set, conf_enable_secret_enctype0,
	 OBJ(int,1), ENCRYPT_NONE);
TESTVAR	(conf_enable_secret_noprefix, conf_enable_secret_eol, NONE,
	 NONE, NONE, conf_enable_secret_set, sense, FALSE);

SET	(conf_enable_secret_defaultlevel, conf_enable_secret_noprefix,
	 OBJ(int,2), PRIV_MAX);
NUMBER	(conf_enable_secret_level_num, conf_enable_secret_noprefix, no_alt,
	 OBJ(int,2), PRIV_MIN+1, PRIV_MAX, "Level number");
KEYWORD	(conf_enable_secret_level, conf_enable_secret_level_num,
	 conf_enable_secret_defaultlevel,
	 "level", "Set exec level password", PRIV_ROOT);

NVGENS	(conf_enable_secret_nvgen, conf_enable_secret_level, 
         configure_enable, ENABLE_SECRET);
KEYWORD	(conf_enable_secret, conf_enable_secret_nvgen, conf_enable_pass,
	 "secret", "Assign the privileged level secret", PRIV_CONF);

/*
 * The remaining enable options (from here down) are not shown if
 * the variable `old_access_control' is not FALSE
 */

/******************************************************************
 * enable last-resort { succeed | password }
 * no enable last-resort [{ succeed | password }]
 *
 * OBJ(int,1) = TACACS_LAST_FAIL, TACACS_LAST_SUCCEED, TACACS_LAST_PASSWORD
 * Default is for TACACS_LAST_FAIL.
 */
EOLS	(conf_enable_lr_eol, configure_enable, ENABLE_LASTRESORT);

KEYWORD_ID (conf_enable_lr_pass, conf_enable_lr_eol, no_alt,
	    OBJ(int,1), TACACS_LAST_PASSWORD,
	    "password", "Enable by giving the local enable password", PRIV_CONF);
KEYWORD_ID (conf_enable_lr_succ, conf_enable_lr_eol, conf_enable_lr_pass,
	    OBJ(int,1), TACACS_LAST_SUCCEED,
	    "succeed", "Enable without further question", PRIV_CONF);
NOPREFIX (conf_enable_lr_noprefix, conf_enable_lr_succ, conf_enable_lr_eol);
KEYWORD_ID(conf_enable_lr, conf_enable_lr_noprefix, conf_enable_secret,
	   OBJ(int,1), TACACS_LAST_FAIL,
	   "last-resort",
	   "Define enable action if no TACACS servers respond", PRIV_CONF);

/******************************************************************
 * enable use-tacacs
 * no enable use-tacacs
 */

EOLS	(conf_enable_tacacs_eol, configure_enable, ENABLE_USETACACS);
KEYWORD	(conf_enable_tacacs_kwd, conf_enable_tacacs_eol, conf_enable_lr,
	 "use-tacacs", "Use TACACS to check enable passwords", PRIV_CONF);
TEST_BOOLEAN(conf_enable_tacacs, conf_enable_tacacs_kwd, conf_enable_secret,
	     NONE, old_access_control);


KEYWORD_MM(conf_enablepassword, conf_enable_pass_defaultlevel, ALTERNATE,
	   "enable-password", NULL, PRIV_ROOT | PRIV_NOHELP | PRIV_NONVGEN, 7);

KEYWORD	(conf_enable, conf_enable_tacacs, conf_enablepassword,
	 "enable", "Modify enable password parameters", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_enable
