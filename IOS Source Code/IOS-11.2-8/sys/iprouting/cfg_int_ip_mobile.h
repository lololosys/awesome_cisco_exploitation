/* $Id: cfg_int_ip_mobile.h,v 3.4.26.3 1996/08/12 16:03:26 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_int_ip_mobile.h,v $
 *------------------------------------------------------------------
 * cfg_int_ip_mobile.h - Mobile IP configuration commands
 *
 * Oct. 1993 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_mobile.h,v $
 * Revision 3.4.26.3  1996/08/12  16:03:26  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.26.2  1996/06/30  09:11:29  agt
 * CSCdi61730:  IP Mobility too aggressive
 * Branch: California_branch
 * Prevent delete/add of existing mobile routes.
 * Limit arp refresh to once per keepalive, rather than each minute.
 *
 * Revision 3.4.26.1  1996/05/04  01:21:10  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.4  1996/01/18  02:13:23  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.3  1995/12/07  17:45:11  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  17:31:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:35  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:05:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * ip mobile ...
 *
 * OBJ(int,1) = MOBILE_CMD_ARP, MOBILE_CMD_PROXY,
 *		MOBILE_CMD_BEACONING, MOBILE_CMD_FOREIGN, MOBILE_CMD_HOME,
 *		MOBILE_CMD_NODE 
 * OBJ(string,1) = access list name
 * OBJ(int,2) = access list number
 * OBJ(int,3) = keepalive
 * OBJ(int,4) = holdtime
 * OBJ(int,5) = boolean (registration required, solicit)
 * OBJ(idb,1) = Interface for COA address
 */

EOLNS	(ci_ip_mobile_eol, ip_mobile_command);

/************************************************************************
 * [no] ip mobile proxy 
 */
 
KEYWORD_ID (ci_ip_mobile_proxy_real, ci_ip_mobile_eol, NONE,
	    OBJ(int,1), MOBILE_CMD_PROXY,
	    "proxy", "Act as a proxy host", PRIV_CONF|PRIV_HIDDEN);

TEST_IDBSTATUS(ci_ip_mobile_proxy, ci_ip_mobile_proxy_real, NONE, no_alt,
	       IDB_ASYNC);

/************************************************************************
 * [no] ip mobile node [solicit]
 */

KEYWORD_ID (ci_ip_mobile_node_solicit, ci_ip_mobile_eol, ci_ip_mobile_eol,
	    OBJ(int,5), TRUE,
	    "solicit", "Send solicit messages to find foreign agent",
	    PRIV_CONF);
KEYWORD_ID (ci_ip_mobile_node, ci_ip_mobile_node_solicit, ci_ip_mobile_proxy,
	    OBJ(int,1), MOBILE_CMD_NODE,
	    "node", "Mobile IP mobile node",
	    PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);

/************************************************************************
 * [no] ip mobile home-agent
 */

KEYWORD_ID (ci_ip_mobile_home, ci_ip_mobile_eol, ci_ip_mobile_node,
	    OBJ(int,1), MOBILE_CMD_HOME,
	    "home-agent", "Mobile IP home agent",
	    PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);

/************************************************************************
 * [no] ip mobile foreign-agent [authenticated] [access-group <value>]
 */

NUMBER	(ci_ip_mobile_foreign_list, ci_ip_mobile_eol, ci_ip_mobile_eol,
	 OBJ(int,2), MINFASTACC, MAXFASTACC, "IP standard access list");
	 
KEYWORD (ci_ip_mobile_foreign_acc, ci_ip_mobile_foreign_list, ci_ip_mobile_eol,
	 "access-group", "Access list to record registered hosts", PRIV_CONF);

KEYWORD_ID (ci_ip_mobile_foreign_auth, ci_ip_mobile_foreign_acc,
	    ci_ip_mobile_foreign_acc,
	    OBJ(int,5), TRUE,
	    "authenticated", "Authenticate registrations",
	    PRIV_CONF);	 

NVGENNS	   (ci_ip_mobile_foreign_nvgen, ci_ip_mobile_foreign_auth,
	    ip_mobile_command);

KEYWORD_ID (ci_ip_mobile_foreign, ci_ip_mobile_foreign_nvgen,
	    ci_ip_mobile_home, 
	    OBJ(int,1), MOBILE_CMD_FOREIGN,
	    "foreign-agent", "Mobile IP foreign agent",
	    PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);

/************************************************************************
 * [no] ip mobile beaconing [registration-required] [<interface>]
 */

INTERFACE  (ci_ip_mobile_beaconing_int, ci_ip_mobile_eol, no_alt,
	    OBJ(idb,1), IFTYPE_HWIDB);

KEYWORD	   (ci_ip_mobile_beaconing_careof, ci_ip_mobile_beaconing_int,
	    ci_ip_mobile_eol,
	    "care-of", NULL /*"Interface for care of address"*/, PRIV_CONF);

KEYWORD_ID (ci_ip_mobile_beaconing_regreq, ci_ip_mobile_beaconing_careof,
	    ci_ip_mobile_beaconing_int, OBJ(int,5), TRUE,
	    "registration-required", NULL /*"Mobile nodes must register"*/,
	    PRIV_CONF);

NVGENNS	   (ci_ip_mobile_beaconing_nvgen, ci_ip_mobile_beaconing_regreq,
	    ip_mobile_command);

KEYWORD_ID (ci_ip_mobile_beaconing, ci_ip_mobile_beaconing_nvgen,
	    ci_ip_mobile_proxy,
	    OBJ(int,1), MOBILE_CMD_BEACONING,
	    "beaconing", NULL /*"Send Mobile IP beacon packets"*/,
	    PRIV_CONF|PRIV_HIDDEN);

/************************************************************************
 * [no] ip mobile arp [timers <keepalive> <holdtime>] [access-group <value>]
 */

GENERAL_STRING (ci_ip_mobile_arp_accname,
		ci_ip_mobile_eol, ci_ip_mobile_eol, OBJ(string,1),
		"Access-list name", STRING_HELP_CHECK); 
TEST_MULTIPLE_FUNCS(ci_ip_mobile_arp_stest,
		    ci_ip_mobile_arp_accname, NONE, no_alt);
NUMBER	(ci_ip_mobile_arp_list, ci_ip_mobile_eol, ci_ip_mobile_arp_stest,
	 OBJ(int,2), MINFASTACC, MAXFASTACC, "IP standard access list");
	 
KEYWORD (ci_ip_mobile_arp_acc, ci_ip_mobile_arp_list, ci_ip_mobile_eol,
	 "access-group", "Access list of acceptable mobile hosts", PRIV_CONF);

NUMBER  (ci_ip_mobile_holdtime, ci_ip_mobile_arp_acc, no_alt,
	 OBJ(int,4), 1, 14400, "Holdtime in minutes");

/* 1440 = 60 * 24 -- i.e. 1 day */
NUMBER	(ci_ip_mobile_keepalive, ci_ip_mobile_holdtime, no_alt,
	 OBJ(int,3), 1, 1440, "Keepalive interval in minutes");

KEYWORD	(ci_ip_mobile_timers, ci_ip_mobile_keepalive, ci_ip_mobile_arp_acc,
	 "timers", "Set keepalive and holdtime timers", PRIV_CONF);

NVGENNS	(ci_ip_mobile_arp_nvgen, ci_ip_mobile_timers, ip_mobile_command);

KEYWORD_ID (ci_ip_mobile_arp, ci_ip_mobile_arp_nvgen, ci_ip_mobile_beaconing,
	    OBJ(int,1), MOBILE_CMD_ARP,
	    "arp", "ARP discovery of mobile hosts", PRIV_CONF);

KEYWORD	(ci_ip_mobile, ci_ip_mobile_arp, ALTERNATE,
	 "mobile", "Mobile Host Protocol", PRIV_CONF|PRIV_SUBIF);


#undef	ALTERNATE
#define	ALTERNATE	ci_ip_mobile

