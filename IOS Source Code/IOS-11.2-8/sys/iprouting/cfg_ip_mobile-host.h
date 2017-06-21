/* $Id: cfg_ip_mobile-host.h,v 3.3.20.1 1996/08/12 16:03:31 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_ip_mobile-host.h,v $
 *------------------------------------------------------------------
 * cfg_ip_mobile-host.h -- Configuration of the mobile host database
 *
 * Nov. 1993 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_mobile-host.h,v $
 * Revision 3.3.20.1  1996/08/12  16:03:31  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:33:53  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:31:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:40  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:08:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * ip mobile-host <address> [<address2>] [privacy] [password (clear | md5)
 * <string> ] 
 *
 * OBJ(paddr,1) = <address>
 * OBJ(paddr,2) = <address2> - top of range
 * OBJ(int,1) = TRUE iff privacy
 * OBJ(int,2) = MOBILE_PASS_CLEAR, MOBILE_PASS_MD5
 * OBJ(int,3) = encryption type
 * OBJ(int,4) = TRUE if password keyword is mentioned
 * OBJ(string,1) = <password>
 */

EOLNS	(cip_mobile_eol, ip_mobile_host_command);

TEXT	(cip_mobile_passwd, cip_mobile_eol, no_alt,
	 OBJ(string,1), "Host password");

TEST_MULTIPLE_FUNCS(cip_mobile_passwd_test, cip_mobile_passwd,
		    no_alt, NONE);
DECIMAL (cip_mobile_pass_enctype, cip_mobile_passwd,
	 cip_mobile_passwd_test,
	 OBJ(int, 3), ENCRYPT_NONE, ENCRYPT_TEMP,
	 "Encryption type (0 for not yet encrypted, 7 for proprietary)");

KEYWORD_ID (cip_mobile_pass_md5, cip_mobile_pass_enctype, no_alt,
	    OBJ(int, 2), MOBILE_PASS_MD5, "md5",
	    "MD5 registration", PRIV_CONF);

KEYWORD_ID (cip_mobile_pass_clear, cip_mobile_pass_enctype,
	    cip_mobile_pass_md5, OBJ(int,2), MOBILE_PASS_CLEAR, "clear",
	    "Cleartext password", PRIV_CONF);

NOPREFIX (cip_mobile_pass_noprefix, cip_mobile_pass_clear,
	  cip_mobile_eol);

KEYWORD_ID(cip_mobile_password, cip_mobile_pass_noprefix, cip_mobile_eol,
	   OBJ(int,4), TRUE,
	   "password", "Host password information", PRIV_CONF);

KEYWORD_ID (cip_mobile_privacy, cip_mobile_password, cip_mobile_password,
	    OBJ(int,1), TRUE,
	    "privacy", "Host location privacy", PRIV_CONF);

IPADDR	(cip_mobile_top, cip_mobile_privacy, cip_mobile_privacy,
	 OBJ(paddr,2), "Top of range");

IPADDR	(cip_mobile_address, cip_mobile_top, no_alt,
	 OBJ(paddr,1), "IP address of mobile host or bottom of range");

NVGENNS	(cip_mobile_nvgen, cip_mobile_address, ip_mobile_host_command); 

KEYWORD	(cip_mobile_host, cip_mobile_nvgen, ALTERNATE,
	 "mobile-host", "Mobile host database", PRIV_CONF); 

#undef	ALTERNATE
#define	ALTERNATE	cip_mobile_host
