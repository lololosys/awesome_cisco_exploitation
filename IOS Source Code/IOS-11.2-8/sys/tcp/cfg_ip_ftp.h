/* $Id: cfg_ip_ftp.h,v 3.2.8.1 1996/04/30 20:55:39 jenny Exp $
 * $Source: /release/112/cvs/Xsys/tcp/cfg_ip_ftp.h,v $
 *------------------------------------------------------------------
 * C F G _ I P _ F T P . H
 *
 * Global configuration IP FTP commands.
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_ftp.h,v $
 * Revision 3.2.8.1  1996/04/30  20:55:39  jenny
 * CSCdi52475:  TFTP: cannot specify source address
 * Branch: California_branch
 * Create interface for ftp, rcp, and telnet ource interface commands
 * if they are not there when the commands are parsed.
 *
 * Revision 3.2  1996/02/29  23:39:30  ahh
 * CSCdi50429:  TCP: FTP config commands should be hidden
 * (or PRIV_NOHELP, like the exception config commands)
 *
 * Revision 3.1  1995/11/09  13:35:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  08:35:20  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 *	[no] ip ftp password <string>
 *
 * OBJ(int,1) = [0-7] (encryption type), or -1 if no type given
 * OBJ(string,1) -- password
 *
 * Parse chain duped from line config command.
 */
EOLS	(cip_ftp_password_eol, ip_ftp_command, FTP_PASSWORD);
TEXT	(cip_ftp_password_passwd, cip_ftp_password_eol, no_alt,
	 OBJ(string,1), "The password");
TEST_MULTIPLE_FUNCS(cip_ftp_password_test, cip_ftp_password_passwd,
	 no_alt, no_alt);
DECIMAL (cip_ftp_password_enctype, cip_ftp_password_passwd,
	 cip_ftp_password_test,
	 OBJ(int,1), ENCRYPT_NONE, ENCRYPT_TEMP,
	 "Encryption type (0 to disable encryption, 7 for proprietary");
NOPREFIX(cip_ftp_password_noprefix, cip_ftp_password_enctype,
	  cip_ftp_password_eol);
NVGENS	(cip_ftp_password_nvgen, cip_ftp_password_noprefix,
	 ip_ftp_command, FTP_PASSWORD);
KEYWORD_MM(cip_ftp_password, cip_ftp_password_nvgen, no_alt,
	 "password",
	 "Specify password for FTP connections",
	 PRIV_CONF, 5);

/******************************************************************************
 *	[no] ip ftp username <string>
 *
 * OBJ(string,1) -- username
 */
EOLS	(cip_ftp_username_eol, ip_ftp_command, FTP_USERNAME);
STRING	(cip_ftp_username_name, cip_ftp_username_eol, no_alt,
	 OBJ(string,1), "Username");
NOPREFIX (cip_ftp_username_noprefix, cip_ftp_username_name,
	  cip_ftp_username_eol);
NVGENS	(cip_ftp_username_nvgen, cip_ftp_username_noprefix,
	 ip_ftp_command, FTP_USERNAME);
KEYWORD	(cip_ftp_username, cip_ftp_username_nvgen, cip_ftp_password,
	 "username",
	 "Specify username for FTP connections",
	 PRIV_CONF);


/******************************************************************************
 *	[no] ip ftp source-interface <interface>
 *
 * OBJ(idb,1) -- IDB to pull source IP address from
 */
EOLS	(cip_ftp_source_eol, ip_ftp_command, FTP_SOURCE_INTERFACE);
FORWARD_INTERFACE (cip_ftp_source_name, cip_ftp_source_eol, no_alt,
		   OBJ(idb,1), IFTYPE_ANYSUB);
NOPREFIX (cip_ftp_source_noprefix, cip_ftp_source_name,
	  cip_ftp_source_eol);
NVGENS	(cip_ftp_source_nvgen, cip_ftp_source_noprefix,
	 ip_ftp_command, FTP_SOURCE_INTERFACE);
KEYWORD	(cip_ftp_source, cip_ftp_source_nvgen, cip_ftp_username,
	 "source-interface",
	 "Specify interface for source address in FTP connections",
	 PRIV_CONF);

/******************************************************************************
 *	[no] ip ftp passive
 */
EOLS	(cip_ftp_eol, ip_ftp_command, FTP_PASSIVE);
KEYWORD	(cip_ftp_passive, cip_ftp_eol, cip_ftp_source,
	 "passive", "Connect using passive mode",
	 PRIV_CONF);

KEYWORD	(cip_ftp, cip_ftp_passive, ALTERNATE,
	 "ftp", "FTP configuration commands",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_ftp
