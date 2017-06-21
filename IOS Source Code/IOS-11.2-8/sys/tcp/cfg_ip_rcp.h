/* $Id: cfg_ip_rcp.h,v 3.1.64.2 1996/04/30 20:55:41 jenny Exp $
 * $Source: /release/112/cvs/Xsys/tcp/cfg_ip_rcp.h,v $
 *------------------------------------------------------------------
 * C F G _ I P _ R C P . H
 *
 * Global configuration IP rcp commands.
 * 
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_rcp.h,v $
 * Revision 3.1.64.2  1996/04/30  20:55:41  jenny
 * CSCdi52475:  TFTP: cannot specify source address
 * Branch: California_branch
 * Create interface for ftp, rcp, and telnet ource interface commands
 * if they are not there when the commands are parsed.
 *
 * Revision 3.1.64.1  1996/04/15  21:23:12  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1995/11/09  13:35:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  08:35:20  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 *	[no] ip rcmd source-interface <interface>
 *
 * OBJ(idb,1) -- IDB to pull source IP address from
 */
EOLS	(cip_rcmd_source_eol, rcmd_command, RCMD_SOURCE_INTERFACE);
FORWARD_INTERFACE (cip_rcmd_source_name, cip_rcmd_source_eol, no_alt,
	   	   OBJ(idb,1), IFTYPE_ANYSUB);
NOPREFIX (cip_rcmd_source_noprefix, cip_rcmd_source_name,
	  cip_rcmd_source_eol);
NVGENS	(cip_rcmd_source_nvgen, cip_rcmd_source_noprefix,
	 rcmd_command, RCMD_SOURCE_INTERFACE);
KEYWORD	(cip_rcmd_source, cip_rcmd_source_nvgen, no_alt,
	 "source-interface",
	 "Specify interface for source address in rcmd connections",
	 PRIV_CONF);

/******************************************************************************
 *	[no] ip rcmd remote-username <name>
 *
 */
EOLS	(cip_rcmd_remote_user_eol, rcmd_command, RCMD_REMOTE_USER);

/* ... <name> */
STRING	(cip_rcmd_remote_user_name, cip_rcmd_remote_user_eol, no_alt,
	 OBJ(string,1), "This system's username for remote commands");

/* ... remote-username ... */
NOPREFIX (cip_rcmd_remote_user_noprefix, cip_rcmd_remote_user_name,
	  cip_rcmd_remote_user_eol);
NVGENS	(cip_rcmd_remote_user_nvgen, cip_rcmd_remote_user_noprefix,
	 rcmd_command, RCMD_REMOTE_USER);
KEYWORD	(cip_rcmd_remote_user, cip_rcmd_remote_user_nvgen, cip_rcmd_source,
	 "remote-username", "Set system's username for rcp", PRIV_CONF);

/******************************************************************************
 *	
 *    [no] ip rcmd remote-host <remote-user> { <host> | <ipaddr> }
 *	   <local-user> [enable [<level>]]
 * 
 *
 * OBJ(string,1) - local username (target user on the router)
 * OBJ(paddr,1)  - remote hostname
 * OBJ(string,2) - remote username (user the rcmd is coming from)
 * OBJ(int,1)    - enable level
 */
EOLS	(cip_rcmd_rhost_eol, rcmd_command, RCMD_REMOTE_HOST);

TEST_MULTIPLE_FUNCS(cip_rcmd_test, cip_rcmd_rhost_eol, no_alt, NONE);

/* ... [enable] */
NUMBER	   (cip_rcmd_rhost_enable_level, cip_rcmd_rhost_eol, cip_rcmd_test,
	    OBJ(int,1), PRIV_USER, PRIV_MAX, "Enable level");
KEYWORD_ID (cip_rcmd_rhost_enable, cip_rcmd_rhost_enable_level, cip_rcmd_test,
	    OBJ(int,1), PRIV_MAX,
	    "enable", "Allow enable-mode commands", PRIV_CONF);

/* ... <remote-user> */
STRING	(cip_rcmd_rhost_remote_user, cip_rcmd_rhost_enable, no_alt,
	 OBJ(string,2), "Remote username");

/* ... { <host> | <ipaddr> } ... */
IPADDR_NAME (cip_rcmd_rhost_host, cip_rcmd_rhost_remote_user, no_alt,
	 OBJ(paddr,1), "Remote hostname or IP address");

/* ... <local-user> */
STRING	(cip_rcmd_rhost_local_user, cip_rcmd_rhost_host, no_alt,
	 OBJ(string,1), "Local username");

/* ... remote-host ... */
SET	(cip_rcmd_rhost_set, cip_rcmd_rhost_local_user, OBJ(int,1), PRIV_USER);
NVGENS	(cip_rcmd_rhost_nvgen, cip_rcmd_rhost_set,
	 rcmd_command, RCMD_REMOTE_HOST);
KEYWORD	(cip_rcmd_rhost, cip_rcmd_rhost_nvgen, cip_rcmd_remote_user,
	 "remote-host", "Set host+user equivalence", PRIV_CONF);

/******************************************************************************
 *      [no] ip rcmd rsh-disable-command
 *
 *
 */
EOLS    (cip_rcmd_rsh_disable_cmd_eol, rcmd_command, RCMD_RSH_DISABLED_CMD);

/* ... <command> */
STRING  (cip_rcmd_rsh_disable_cmd_str, cip_rcmd_rsh_disable_cmd_eol,
	 no_alt, OBJ(string,1), "");

NVGENS  (cip_rcmd_rsh_disable_cmd_nvgen, cip_rcmd_rsh_disable_cmd_str,
	 rcmd_command, RCMD_RSH_DISABLED_CMD);
KEYWORD (cip_rcmd_rsh_disable_cmd, cip_rcmd_rsh_disable_cmd_nvgen,
	 cip_rcmd_rhost, "rsh-disable-command", "", PRIV_CONF|PRIV_HIDDEN);

/******************************************************************************
 *      [no] ip rcmd rsh-enable
 *
 *
 */
EOLS    (cip_rcmd_rsh_enable_eol, rcmd_command, RCMD_RSH_ENABLE);
NVGENS	(cip_rcmd_rsh_enable_nvgen, cip_rcmd_rsh_enable_eol, rcmd_command,
	 RCMD_RSH_ENABLE);
KEYWORD (cip_rcmd_rsh_enable, cip_rcmd_rsh_enable_nvgen,
	 cip_rcmd_rsh_disable_cmd, "rsh-enable",
	 "Enable server side of RSH", PRIV_CONF);

/******************************************************************************
 *      [no] ip rcmd rcp-enable
 *
 *
 */
EOLS    (cip_rcp_enable_eol, rcmd_command, RCMD_RCP_ENABLE);
NVGENS	(cip_rcmd_rcp_enable_nvgen, cip_rcp_enable_eol, rcmd_command,
	 RCMD_RCP_ENABLE);
KEYWORD (cip_rcmd_rcp_enable, cip_rcmd_rcp_enable_nvgen, cip_rcmd_rsh_enable,
	 "rcp-enable", "Enable server side of RCP", PRIV_CONF);

/******************************************************************************
 *      [no] ip rcmd domain-lookup
 *
 */
EOLS	(cip_rcmd_domainlookup_eol, rcmd_command, RCMD_DOMAINLOOKUP);
KEYWORD (cip_rcmd_domainlookup, cip_rcmd_domainlookup_eol, cip_rcmd_rcp_enable,
	 "domain-lookup", "Enable IP DNS queries for Rcmd", PRIV_CONF);

KEYWORD	(cip_rcmd, cip_rcmd_domainlookup, ALTERNATE,
	 "rcmd", "Rcmd commands", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_rcmd
