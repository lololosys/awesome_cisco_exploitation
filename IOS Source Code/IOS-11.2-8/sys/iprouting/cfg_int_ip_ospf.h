/* $Id: cfg_int_ip_ospf.h,v 3.2.60.2 1996/09/09 08:17:40 myeung Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_int_ip_ospf.h,v $
 *------------------------------------------------------------------
 * OSPF interface commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_ospf.h,v $
 * Revision 3.2.60.2  1996/09/09  08:17:40  myeung
 * CSCdi62640:  OSPF dead-interval in NVRAM but not loaded into config
 * upon bootup.
 * Branch: California_branch
 * - Swap dead-interval and hello-interval in the config
 *
 * Revision 3.2.60.1  1996/04/16  19:01:20  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:31:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:35  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:05:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 * ip ospf { extended_auth_key <key-id> md5 <key> |
 *	    authentication-key <keyword> | cost <number> | 
 *	    dead-interval <number> | hello-interval <number> |
 *	    priority <number> | retransmit-interval <number> |
 *	    transmit-delay <number> | network <keyword> |
 *	    demand-circuit }
 *
 * no ip ospf { extended_auth_key <key-id> md5 <key> |
 *	    authentication-key [<keyword>] | cost [<number>] | 
 *	    dead-interval [<number>] | hello-interval [<number>] |
 *	    priority [<number>] | retransmit-interval [<number>] |
 *	    transmit-delay [<number>] | network <keyword> |
 *	    demand-circuit }
 *
 * OBJ(int,1) = <number>
 * OBJ(string,1) = <keyword>
 * csb->which = OSPF_RXMT_INTERVAL, OSPF_TRANS_DELAY, OSPF_PRIORITY,
 *	    OSPF_HELLO_INTERVAL, OSPF_DEAD_INTERVAL, OSPF_AUTH_TYPE1_KEY,
 *	    OSPF_IF_COST, OSPF_NETWORK_TYPE, OSPF_AUTH_TYPE2_KEY, 
 *	    OSPF_DEMAND_CIRCUIT
 */

EOLS	(ci_ip_ospf_dc_eol, ospf_intfc_command, OSPF_DEMAND_CIRCUIT);

KEYWORD (ci_ip_ospf_dc, ci_ip_ospf_dc_eol, no_alt, "demand-circuit", 
	 "OSPF demand circuit", PRIV_CONF|PRIV_SUBIF);

PARAMS_KEYONLY (ci_ip_ospf_trans, ci_ip_ospf_dc,
		"transmit-delay", OBJ(int,1), 1, 65535,
		ospf_intfc_command, OSPF_TRANS_DELAY,
		"Link state transmit delay", "Seconds", PRIV_CONF|PRIV_SUBIF);

PARAMS_KEYONLY (ci_ip_ospf_retrans, ci_ip_ospf_trans,
		"retransmit-interval", OBJ(int,1), 1, 65535,
		ospf_intfc_command, OSPF_RXMT_INTERVAL,
		"Time between retransmitting lost link state advertisements",
		"Seconds", PRIV_CONF|PRIV_SUBIF);

PARAMS_KEYONLY (ci_ip_ospf_priority, ci_ip_ospf_retrans,
		"priority", OBJ(int,1), 0, 255,
		ospf_intfc_command, OSPF_PRIORITY,
		"Router priority", "Priority", PRIV_CONF|PRIV_SUBIF);

PARAMS_KEYONLY (ci_ip_ospf_dead, ci_ip_ospf_priority,
		"dead-interval", OBJ(int,1), 1, 65535,
		ospf_intfc_command, OSPF_DEAD_INTERVAL,
		"Interval after which a neighbor is declared dead", "Seconds",
		PRIV_CONF|PRIV_SUBIF);

PARAMS_KEYONLY (ci_ip_ospf_hello, ci_ip_ospf_dead,
		"hello-interval", OBJ(int,1), 1, 65535,
		ospf_intfc_command, OSPF_HELLO_INTERVAL,
		"Time between HELLO packets", "Seconds", PRIV_CONF|PRIV_SUBIF);

PARAMS_KEYONLY (ci_ip_ospf_cost, ci_ip_ospf_hello,
		"cost", OBJ(int,1), 1, 65535,
		ospf_intfc_command, OSPF_IF_COST,
		"Interface cost", "Cost", PRIV_CONF|PRIV_SUBIF);

EOLS	(ci_ip_ospf_network_eol, ospf_intfc_command, OSPF_NETWORK_TYPE);

KEYWORD_ID (ci_ip_ospf_network_p2mp, ci_ip_ospf_network_eol,
	    no_alt, OBJ(int,1), IF_POINT_TO_M_POINT,
	    "point-to-multipoint", "Specify OSPF Type of Network",
	    PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID (ci_ip_ospf_network_nonbroad, ci_ip_ospf_network_eol,
	    ci_ip_ospf_network_p2mp, OBJ(int,1), IF_NONBROADCAST,
	    "non-broadcast", "Specify OSPF Type of Network",
	    PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID (ci_ip_ospf_network_broad, ci_ip_ospf_network_eol,
	    ci_ip_ospf_network_nonbroad, OBJ(int,1), IF_BROADCAST,
	    "broadcast", "Specify OSPF Type of Network", PRIV_CONF|PRIV_SUBIF);

NVGENS	(ci_ip_ospf_network_nvgen, ci_ip_ospf_network_broad,
	 ospf_intfc_command, OSPF_NETWORK_TYPE);
KEYWORD (ci_ip_ospf_network, ci_ip_ospf_network_nvgen, ci_ip_ospf_cost,
	 "network", "Network type", PRIV_CONF|PRIV_SUBIF);

EOLS	(ci_ip_ospf_md_eol, ospf_intfc_command, OSPF_AUTH_MD_KEY);
TEXT	(ci_ip_ospf_md_key, ci_ip_ospf_md_eol, no_alt, 
	 OBJ(string,1), "The OSPF password (key)");
TEST_MULTIPLE_FUNCS(ci_ip_ospf_md_test, ci_ip_ospf_md_key, no_alt, NONE);
DECIMAL (ci_ip_ospf_md_enctype, ci_ip_ospf_md_key, ci_ip_ospf_md_test,
	 OBJ(int,3), ENCRYPT_NONE, ENCRYPT_TEMP,
	 "Encryption type (0 for not yet encrypted, 7 for proprietary)");
KEYWORD (ci_ip_ospf_md_md5, ci_ip_ospf_md_enctype, no_alt, "md5", 
	 "Use MD5 algorithm", PRIV_CONF|PRIV_SUBIF);
NOPREFIX (ci_ip_ospf_md_noprefix, ci_ip_ospf_md_md5, ci_ip_ospf_md_eol);
NUMBER (ci_ip_ospf_md_keyid, ci_ip_ospf_md_noprefix,
        no_alt, OBJ(int,1), 1, 255, "Key ID");
NVGENS	(ci_ip_ospf_md_nvgen, ci_ip_ospf_md_keyid, 
	 ospf_intfc_command, OSPF_AUTH_MD_KEY);
KEYWORD (ci_ip_ospf_md, ci_ip_ospf_md_nvgen, 
	 ci_ip_ospf_network, "message-digest-key", 
	 "Message digest authentication password (key)", PRIV_CONF|PRIV_SUBIF);

EOLS	(ci_ip_ospf_auth_eol, ospf_intfc_command, OSPF_AUTH_CT_KEY);
TEXT	(ci_ip_ospf_auth_key, ci_ip_ospf_auth_eol, no_alt, 
	 OBJ(string,1), "The OSPF password (key)");
TEST_MULTIPLE_FUNCS(ci_ip_ospf_auth_test, ci_ip_ospf_auth_key,
		    no_alt, NONE);
DECIMAL (ci_ip_ospf_auth_enctype, ci_ip_ospf_auth_key,
	 ci_ip_ospf_auth_test,
	 OBJ(int,1), ENCRYPT_NONE, ENCRYPT_TEMP,
	 "Encryption type (0 for not yet encrypted, 7 for proprietary)");

NOPREFIX (ci_ip_ospf_auth_noprefix, ci_ip_ospf_auth_enctype, 
	  ci_ip_ospf_auth_eol);
NVGENS	(ci_ip_ospf_auth_nvgen, ci_ip_ospf_auth_noprefix, ospf_intfc_command,
	 OSPF_AUTH_CT_KEY);
KEYWORD (ci_ip_ospf_auth, ci_ip_ospf_auth_nvgen, ci_ip_ospf_md,
	 "authentication-key", "Authentication password (key)",
	 PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_ip_ospf, ci_ip_ospf_auth, ALTERNATE,
	 "ospf", "OSPF interface commands", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_ospf
