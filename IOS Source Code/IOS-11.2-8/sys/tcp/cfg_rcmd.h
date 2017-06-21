/* $Id: cfg_rcmd.h,v 3.3.28.1 1996/04/15 21:23:15 bcole Exp $
 * $Source: /release/112/cvs/Xsys/tcp/cfg_rcmd.h,v $
 *------------------------------------------------------------------
 * C F G _ R C M D . H
 *
 * Rcmd-related configuration commands -- obsolete
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_rcmd.h,v $
 * Revision 3.3.28.1  1996/04/15  21:23:15  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/22  07:29:09  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:47:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:31  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:11:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This parse chain is obsolete -- make changes/additions in cfg_ip.h
 */

/*
 * [no] rcmd domain-lookup
 */
EOLS (rcmd_domainlookup_eol, rcmd_command, RCMD_DOMAINLOOKUP);
KEYWORD (rcmd_domainlookup, rcmd_domainlookup_eol, no_alt,
         "domain-lookup", "Enable IP DNS queries for Rcmd",
         (PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN));

/*
 * [no] rcmd remote-username <name>
 */
EOLS	(rcmd_remote_user_eol, rcmd_command, RCMD_REMOTE_USER);

/* ... <name> */
STRING	(rcmd_remote_user_name, rcmd_remote_user_eol, no_alt, OBJ(string,1),
	 "This system's username for remote commands");

/* ... remote-username ... */
NOPREFIX (rcmd_remote_user_noprefix, rcmd_remote_user_name,
	  rcmd_remote_user_eol);
NVGENS	(rcmd_remote_user_nvgen, rcmd_remote_user_noprefix,
	 rcmd_command, RCMD_REMOTE_USER);
KEYWORD	(rcmd_remote_user, rcmd_remote_user_nvgen, rcmd_domainlookup,
	 "remote-username", "Set system's username for rcp",
	 (PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN));

/*
 * [no] rcmd remote-host <remote-user> { <host> | <ipaddr } <local-user> [enable]
 *
 * OBJ(string,1) - local username (target user on the router)
 * OBJ(paddr,1)  - remote hostname
 * OBJ(string,2) - remote username (user the rcmd is coming from)
 * OBJ(int,1)    - privilege-level (PRIV_ROOT if allowed for enable commands)
 */
EOLS	(rcmd_rhost_eol, rcmd_command, RCMD_REMOTE_HOST);

TEST_MULTIPLE_FUNCS(rcmd_test, rcmd_rhost_eol, no_alt, NONE);

/* ... [enabled] */
KEYWORD_ID (rcmd_rhost_enable, rcmd_rhost_eol, rcmd_test,
	    OBJ(int,1), PRIV_ROOT,
	    "enable", "Allow enable-mode commands",
	    (PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN));

/* ... <remote-user> */
STRING	(rcmd_rhost_remote_user, rcmd_rhost_enable, no_alt,
	 OBJ(string,2), "Remote username");

/* ... { <host> | <ipaddr> } ... */
IPADDR_NAME (rcmd_rhost_host, rcmd_rhost_remote_user, no_alt,
	 OBJ(paddr,1), "Remote hostname or IP address");

/* ... <local-user> */
STRING	(rcmd_rhost_local_user, rcmd_rhost_host, no_alt,
	 OBJ(string,1), "Local username");

/* ... remote-host ... */
NVGENS	(rcmd_rhost_nvgen, rcmd_rhost_local_user, rcmd_command,
	 RCMD_REMOTE_HOST);
KEYWORD	(rcmd_rhost, rcmd_rhost_nvgen, rcmd_remote_user,
	 "remote-host", "Set host+user equivalence",
	 (PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN)); 

/*
 * [no] rsh-disable-command <command>
 */
EOLS    (rsh_disable_command_eol, rcmd_command, RCMD_RSH_DISABLED_CMD);

/* ... <command> */
STRING  (rsh_disable_command_str, rsh_disable_command_eol, no_alt,
	 OBJ(string,1), "");

NVGENS  (rsh_disable_cmd_nvgen, rsh_disable_command_str,
	 rcmd_command, RCMD_RSH_DISABLED_CMD);
KEYWORD (rsh_disable_cmd, rsh_disable_cmd_nvgen, ALTERNATE,
	 "rsh-disable-command", "", PRIV_CONF|PRIV_NONVGEN|PRIV_HIDDEN);
/*
 * [no] rsh-enable
 */
EOLS	(rsh_enable_eol, rcmd_command, RCMD_RSH_ENABLE);

NVGENS	(rsh_enable_nvgen, rsh_enable_eol, rcmd_command, RCMD_RSH_ENABLE);
KEYWORD (rsh_enable, rsh_enable_nvgen, rsh_disable_cmd,
	 "rsh-enable", "Enable server side of RSH",
	 (PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN)); 

/*
 * [no] rcp-enable
 */
EOLS	(rcp_enable_eol, rcmd_command, RCMD_RCP_ENABLE);

NVGENS	(rcp_enable_nvgen, rcp_enable_eol, rcmd_command, RCMD_RCP_ENABLE);
KEYWORD (rcp_enable, rcp_enable_nvgen, rsh_enable,
	 "rcp-enable", "Enable server side of RCP", 
	 (PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN)); 

/* [no] rcmd ... */
KEYWORD	(rcmd, rcmd_rhost, rcp_enable,
	 "rcmd", "Remote command configuration commands",
	 (PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN));

#undef	ALTERNATE
#define	ALTERNATE	rcmd
