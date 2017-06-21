/* $Id: cfg_stun.h,v 3.2 1995/11/17 09:22:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/cfg_stun.h,v $
 *------------------------------------------------------------------
 * C F G _ S T U N . H
 * 
 * STUN global configuration commands.
 * 
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_stun.h,v $
 * Revision 3.2  1995/11/17  09:22:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/27  03:11:02  vrawat
 * CSCdi33572:  AS/400 sdlc multidrop enviromnment
 *
 * Revision 2.1  1995/07/02  10:53:52  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:14:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS    (conf_stun_quickresponse_eol,stun_command,STUN_QUICK_RESPONSE);
KEYWORD (conf_stun_quickresponse,conf_stun_quickresponse_eol,no_alt,
         "quick-response",
         "Immediate response to XID and SNRM ", PRIV_CONF);

/************************************************************************
 * stun slack-fsm-timeout <seconds>
 * no stun slack-fsm-timeout
 */
EOLS	(conf_stun_fsm_timeout_eol, stun_command, STUN_SLACK_FSM_TIMEOUT);

NUMBER	(conf_stun_fsm_timeout_seconds, conf_stun_fsm_timeout_eol, conf_stun_fsm_timeout_eol,
	 OBJ(int,1), 10, 100, "slack FSM timeout in seconds (default 20 seconds)");

NVGENS	(conf_stun_fsm_timeout_nvgen, conf_stun_fsm_timeout_seconds, stun_command,
              STUN_SLACK_FSM_TIMEOUT);

/* Set OBJ(int,1) to 20 for the default value */
KEYWORD_ID (conf_stun_fsm_timeout, conf_stun_fsm_timeout_nvgen, conf_stun_quickresponse,
            OBJ(int,1), 20, "slack-fsm-timeout", "Set STUN local-ack FSM timeout", PRIV_CONF);


/************************************************************************
 * stun keepalive-count <count>
 * no stun keepalive-count
 */
PARAMS_KEYONLY (conf_stun_count, conf_stun_quickresponse, "keepalive-count",
		OBJ(int,1), 2, 10,
		stun_command, STUN_KEEPALIVE_COUNT,
		"Specify STUN peer keepalive retry count",
                "Number of retries must be between 2 and 10",
		PRIV_CONF);

/************************************************************************
 * stun remote-peer-keepalive <seconds>
 * no stun remote-peer-keepalive
 */
EOLS	(conf_stun_keepalive_eol, stun_command, STUN_KEEPALIVE);

NUMBER	(conf_stun_keepalive_seconds, conf_stun_keepalive_eol, conf_stun_keepalive_eol,
	 OBJ(int,1), 1, 300, "Keepalive interval (default 30 seconds)");

NVGENS	(conf_stun_keepalive_nvgen, conf_stun_keepalive_seconds, stun_command,
              STUN_KEEPALIVE);

/* Set OBJ(int,1) to 30 for the default period. */
KEYWORD_ID (conf_stun_keepalive, conf_stun_keepalive_nvgen, conf_stun_count, OBJ(int,1), 30,
	 "remote-peer-keepalive", "Enable STUN peer keepalive", PRIV_CONF);


/************************************************************************
 * stun protocol-group <group-number> <protocol>
 * no stun protocol-group
 *
 * OBJ(int,1) = protocol group number
 * OBJ(string,1) = protocol name
 */
EOLS	(conf_stun_protoeol, stun_command, STUN_PROTOCOL);
STRING	(conf_stun_protocol, conf_stun_protoeol, no_alt, OBJ(string,1),
	 "Protocol name (max 20 characters)");
NUMBER	(conf_stun_protogrpnum, conf_stun_protocol, no_alt,
	 OBJ(int,1), 1, 255, "A STUN protocol group number");
NVGENS	(conf_stun_protogrpnvgen, conf_stun_protogrpnum, stun_command, 
	 STUN_PROTOCOL);
KEYWORD (conf_stun_protogrp, conf_stun_protogrpnvgen, conf_stun_keepalive,
	 "protocol-group", "Assign a protocol to a protocol group", PRIV_CONF);
                                     
                                     
                                 
/************************************************************************
 * [no] stun schema <name> offset <constant-offset> length <addr-length>
 *	format { decimal | hexadecimal | octal }
 * OBJ(string,1) = name of schema
 * OBJ(int,1) = offset
 * OBJ(int,2) = length
 * OBJ(int,3) = 'd' for "decimal", 'o' for "octal", or 'x' for "hexadecimal"
 */
EOLS	(conf_stun_scheol, stun_command, STUN_SCHEMA);

/* format { decimal | hexadecimal | octal } */
KEYWORD_ID(conf_stun_schfmtoctal, conf_stun_scheol, no_alt,
	   OBJ(int,3), 'o',
	   "octal", "Octal SDLC address format", PRIV_CONF);
KEYWORD_ID(conf_stun_schfmthex, conf_stun_scheol, conf_stun_schfmtoctal,
	   OBJ(int,3), 'x',
	   "hexadecimal", "Hexadecimal SDLC address format", PRIV_CONF);
KEYWORD_ID(conf_stun_schfmtdecimal, conf_stun_scheol, conf_stun_schfmthex,
	   OBJ(int,3), 'd',
	   "decimal", "Decimal SDLC address format", PRIV_CONF);
KEYWORD	(conf_stun_schformat, conf_stun_schfmtdecimal, no_alt,
	 "format", "Set the address format in configuration and show commands",
	 PRIV_CONF);

/* length <addr-length> */
NUMBER	(conf_stun_schlenval, conf_stun_schformat, no_alt,
	 OBJ(int,2), 1, 8,
	 "Address length, in bytes (max 8 for hex, 4 for decimal & octal)");
KEYWORD	(conf_stun_schlength, conf_stun_schlenval, no_alt,
	 "length", "Set the length of the address", PRIV_CONF);

/* offset <constand-offset> */
NUMBER	(conf_stun_schoffval, conf_stun_schlength, no_alt,
	 OBJ(int,1), 0, -1, "Address offset within SDLC frame");
KEYWORD (conf_stun_schoffset, conf_stun_schoffval, no_alt,
	 "offset", "Set the starting offset in SDLC frame", PRIV_CONF);

/* Handle 'no' prefixed command */
NOPREFIX (conf_stun_nosch, conf_stun_schoffset, conf_stun_scheol);

/* schema <name> */
STRING	(conf_stun_schname, conf_stun_nosch, no_alt,
	 OBJ(string,1), "Schema name");
NVGENS	(conf_stun_schnvgen, conf_stun_schname, stun_command, STUN_SCHEMA);
KEYWORD	(conf_stun_schema, conf_stun_schnvgen, conf_stun_protogrp,
	 "schema", "Define a protocol schema (format)", PRIV_CONF);


/************************************************************************
 * stun primary-pass-through <seconds>
 * no stun primary-pass-through
 *
 * The code shows that the value is optional on the 'no' version.
 * (Actually, the 9.0 code shows the value to be entirely optional and
 * the default value is set if no value is given.  I think this should be
 * an error, so that the user is aware of the value being set.  This
 * implementation does this.)
 */
PARAMS_KEYONLY (conf_stun_primary, conf_stun_schema, "primary-pass-through",
		OBJ(int,1), 1, -1, stun_command, STUN_PRIMARY,
		"Set the pass-through polling interval",
		"Pass-through polling interval in seconds", PRIV_CONF);


/************************************************************************
 * stun poll-interval <milliseconds>
 * no stun poll-interval
 */
PARAMS_KEYONLY (conf_stun_poll, conf_stun_primary, "poll-interval",
		OBJ(int,1), 20, -1,
		stun_command, STUN_POLL,
		"Proxy polling interval", "Polling interval in milliseconds",
		PRIV_CONF);


/************************************************************************
 * stun peer-name <ipaddr>
 * no stun peer-name <ipaddr>
 */
EOLS	(conf_stun_peereol, stun_command, STUN_PEER);
HEXDIGIT  (conf_stun_peersap, conf_stun_peereol, conf_stun_peereol,
           OBJ(int,2), 0, 0xFF, "Listening SAP");
KEYWORD_ID(conf_stun_cls, conf_stun_peersap, conf_stun_peereol,
           OBJ(int,1), TRUE, "cls", " cls-local ack", PRIV_CONF);
IPADDR	(conf_stun_peeraddr, conf_stun_cls, no_alt,
	 OBJ(paddr,1), "IP address which identifies this STUN peer");
NOPREFIX(conf_stun_peer_noprefix, conf_stun_peeraddr, conf_stun_peereol);
NVGENS	(conf_stun_peernvgen, conf_stun_peer_noprefix,
	 stun_command, STUN_PEER);
KEYWORD	(conf_stun_peer, conf_stun_peernvgen, conf_stun_poll,
	 "peer-name", "Enable STUN", PRIV_CONF);

KEYWORD	(configure_stun, conf_stun_peer, NONE, "stun",
	 "STUN global configuration commands", PRIV_CONF);

TEST_BOOLEAN(configure_stun_test, configure_stun, NONE,
	     ALTERNATE, bridge_enable);

#undef  ALTERNATE
#define ALTERNATE configure_stun_test





