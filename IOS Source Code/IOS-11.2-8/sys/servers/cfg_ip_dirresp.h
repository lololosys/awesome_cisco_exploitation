/* $Id: cfg_ip_dirresp.h,v 3.1.18.2 1996/08/15 06:49:43 raj Exp $
 * $Source: /release/112/cvs/Xsys/servers/cfg_ip_dirresp.h,v $
 *------------------------------------------------------------------
 * Configuration commands for director-responder
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_dirresp.h,v $
 * Revision 3.1.18.2  1996/08/15  06:49:43  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Update Director Responder Agent to the latest version.
 *
 * Revision 3.1.18.1  1996/07/16  08:01:25  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 3.1  1996/02/20  23:50:12  pst
 * Add rest of client files...
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * [no] ip drp server
 *
 */
EOLS	(cip_dirresp_enable_eol, dirresp_command, DRP_CMD_ENABLE);
NVGENS	(cip_dirresp_enable_nvgen, cip_dirresp_enable_eol,
	 dirresp_command, DRP_CMD_ENABLE);
KEYWORD	(cip_dirresp_enable, cip_dirresp_enable_nvgen, no_alt,
	 "server", "Enable DRP Server Agent", PRIV_CONF);


/******************************************************************************
 * [no] ip drp authentication key-chain <name>
 *
 * OBJ(string,1) -- key chain name
 */
EOLS	(cip_dirresp_authkey_eol, dirresp_command, DRP_CMD_AUTH_KEYCHAIN);
STRING	(cip_dirresp_authkey_key, cip_dirresp_authkey_eol, no_alt,
	 OBJ(string, 1), "Name of key-chain");
NOPREFIX(cip_dirresp_authkey_noprefix, cip_dirresp_authkey_key,
	 cip_dirresp_authkey_eol);
NVGENS	(cip_dirresp_authkey_nvgen, cip_dirresp_authkey_noprefix,
	 dirresp_command, DRP_CMD_AUTH_KEYCHAIN);
KEYWORD	(cip_dirresp_authkey, cip_dirresp_authkey_nvgen, no_alt,
	 "key-chain", "Authentication key-chain", PRIV_CONF);

KEYWORD	(cip_dirresp_auth, cip_dirresp_authkey, cip_dirresp_enable,
	 "authentication", "Authentication commands", PRIV_CONF);

/******************************************************************************
 * [no] ip drp access-group <list>
 *
 * OBJ(int,1) -- access list number
 */
EOLS	(cip_dirresp_access_eol, dirresp_command, DRP_CMD_ACCESS);
NUMBER  (cip_dirresp_access_val, cip_dirresp_access_eol, no_alt,
	 OBJ(int, 1), 1, MAXFASTACC, "Access-list number");
NVGENS	(cip_dirresp_access_nvgen, cip_dirresp_access_val,
	 dirresp_command, DRP_CMD_ACCESS);
KEYWORD (cip_dirresp_access, cip_dirresp_access_nvgen, cip_dirresp_auth,
	"access-group", "List of valid director queriers", PRIV_CONF);

/******************************************************************************
 * ip drp delay
 *
 */
EOLS	(cip_dirresp_delay_eol, dirresp_delay, 0);
KEYWORD (cip_dirresp_delay, cip_dirresp_delay_eol, cip_dirresp_access,
	"delay", "Delay DRP responses for testing",
	 PRIV_CONF|PRIV_HIDDEN);


KEYWORD	(cip_dirresp, cip_dirresp_delay, ALTERNATE,
	 "drp", "Director response protocol configuration commands",
	 PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	cip_dirresp
