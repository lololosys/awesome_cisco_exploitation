/* $Id: cfg_snmp.h,v 3.7.28.3 1996/07/03 17:37:17 jjohnson Exp $
 * $Source: /release/112/cvs/Xsys/snmp/cfg_snmp.h,v $
 *------------------------------------------------------------------
 * C F G _ S N M P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_snmp.h,v $
 * Revision 3.7.28.3  1996/07/03  17:37:17  jjohnson
 * CSCdi61958:  Remove support for undocumented snmp configuration commands
 * remove the last vestiges of support for wildcard snmp communities
 * Branch: California_branch
 *
 * Revision 3.7.28.2  1996/04/15  03:05:37  jjohnson
 * CSCdi53343:  Remove support for the SNMPv2 Simplified Security
 * Conventions
 * Branch: California_branch
 *
 * Revision 3.7.28.1  1996/03/28  16:46:58  jjohnson
 * CSCdi50399:  SNMP can drive CPU utilization to over 90%
 * Make SNMP processes default to low priority, and add a hidden config
 * command to allow customers to change it
 * Branch: California_branch
 *
 * Revision 3.7  1996/01/18  02:15:05  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.6  1995/12/16  00:14:34  sdurham
 * CSCdi44645:  MAC illegal address trap needs snmp-server host and enable
 * cli support.
 *    moved all parser commands related to mib to appropriate mib
 *    subsystem.
 *
 * Revision 3.5  1995/12/14  13:07:58  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.4  1995/11/22  23:43:02  sdurham
 * CSCdi35282:  Need command to configure envmon trap enables
 *      add individual trap options in separate parser-mib file
 *      envmon_mib_chain.
 *
 * Revision 3.3  1995/11/18  00:21:35  jjohnson
 * CSCdi43548:  need mechanism to enable multiple traps for a subsystem
 *
 * Revision 3.2  1995/11/17  18:58:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:25:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/04  01:57:07  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/07/03  22:25:45  jjohnson
 * CSCdi35401:  Parser help for snmp-server packetsize is incorrect
 * Differentiate between the minimum, maximum, and default packetsize
 *
 * Revision 2.1  1995/06/07  22:13:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef  SR_SNMPv2
/*
 * We support V2 so make V2 commands visible
 */
#define V2FLAG		PRIV_CONF

#ifdef SR_UNSECURABLE
/*
 * no security, so hide all authentication and privacy options
 */
#define PRIVFLAG	PRIV_CONF | PRIV_HIDDEN
#define AUTHFLAG	PRIV_CONF | PRIV_HIDDEN
#else                           /* SR_UNSECURABLE */
/*
 * we support authentication, so make authentication options visible
 */
#define AUTHFLAG	PRIV_CONF

#ifdef SR_NO_PRIVACY
/*
 * no privacy, so hide the privacy options
 */
#define PRIVFLAG	PRIV_CONF | PRIV_HIDDEN

#else                           /* SR_NO_PRIVACY */
/*
 * We have privacy, so make privacy options visible
 */
#define PRIVFLAG	PRIV_CONF

#endif                          /* SR_NO_PRIVACY */
#endif                          /* SR_UNSECURABLE */
#else                           /* SR_SNMPv2 */
/*
 * no V2 support, so hide the V2 commands and security options
 * we will still parse them, but won't process them
 */
#define V2FLAG		PRIV_CONF | PRIV_HIDDEN
#define PRIVFLAG	PRIV_CONF | PRIV_HIDDEN
#define AUTHFLAG	PRIV_CONF | PRIV_HIDDEN

#endif                          /* SR_SNMPv2 */

/******************************************************************
 * no snmp-server
 */
EOLS	(conf_snmp_no_eol, snmp_service, SNMP_DISABLE);
TESTVAR	(conf_snmp_no, conf_snmp_no_eol, no_alt,
	 NONE, NONE, NONE, sense, FALSE);

LINK_TRANS(conf_snmp_extend_here, conf_snmp_no);

#define SNMP_ALTERNATE conf_snmp_extend_here

/******************************************************************
 * snmp-server host <address>
 *	       <community-string> [{ snmp | framerelay | tty | x25 } ... ]
 * no snmp-server host <address>
 *	       [<community-string> [{ snmp | framerelay | x25 | thresh } ... ]]
 *
 *
 * OBJ(int,1) = TRAP_SNMP, TRAP_TTY, TRAP_X25, TRAP_FRAMERELAY, TRAP_ENVMON,
 * TRAP_ISDN
 * OBJ(string,1) = <community-string>
 * (*OBJ(paddr,1)) = <address>
 */

pdecl(conf_snmp_host_opts);

EOLS	(conf_snmp_host_eol, snmp_service, SNMP_HOST);

/* [{ snmp | tty | x25 | thresh } ... ]
 * NOTE: Several SNMP trap types can be listed.  This chain does looping.
 */
LINK_TRANS(conf_snmp_host_return_here, conf_snmp_host_opts);
LINK_TRANS(conf_snmp_host_extend_here, NONE);

KEYWORD_OR(conf_snmp_host_snmp, conf_snmp_host_opts,
	   conf_snmp_host_extend_here,
	   OBJ(int,1), (1<<TRAP_SNMP),
	   "snmp", "Allow SNMP-type traps", PRIV_CONF);

NOP	(conf_snmp_host_opts, conf_snmp_host_snmp, conf_snmp_host_eol);

/* <community-string> */
STRING	(conf_snmp_host_comm, conf_snmp_host_opts, no_alt,
	 OBJ(string,1), "SNMP community string");
/* <address> */
NOPREFIX (conf_snmp_host_noprefix, conf_snmp_host_comm, conf_snmp_host_eol);
IPADDR_NAME(conf_snmp_host_addr, conf_snmp_host_noprefix, no_alt,
	    OBJ(paddr,1), "IP address of SNMP TRAP host");
NVGENS	(conf_snmp_host_nvgen, conf_snmp_host_addr,
	 snmp_service, SNMP_HOST);
KEYWORD	(conf_snmp_host, conf_snmp_host_nvgen, SNMP_ALTERNATE,
	 "host", "Specify hosts to receive SNMP TRAPs", PRIV_CONF);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_host


/******************************************************************
 * [no] snmp-server enable {trap | inform} [snmp | framerelay | ...]
 *	[trap-number | trap-name_string | ...]
 *
 ******************************************************************/
EOLS	(conf_snmp_enable_eol, snmp_service, SNMP_ENABLE);

/* 
 * [trap-number | trap-name_string | ...] - additional parameters
 * associated with the trap/inform type.
 * Macros for this parameter can be added per implementation.
 */

pdecl	(conf_snmp_enable_trap_opts);	/* optional param. for traps */
/*
 * another _opts as below can be used for the parameter set of informs
pdecl	(conf_snmp_enable_inform_opts);
 */

/* 
 * [snmp | framerelay |...] -- multiple types allowed
 */
LINK_TRANS(conf_snmp_enable_return_here, conf_snmp_enable_eol);
LINK_TRANS(conf_snmp_enable_extend_here, NONE);

KEYWORD_OR(conf_snmp_enable_trap_snmp_auth,
	   conf_snmp_enable_eol,
	   conf_snmp_enable_eol,
	   OBJ(int,1), (1<<TRAP_SNMP_AUTHENTICATION),
	   "authentication", "Enable authentication trap", PRIV_CONF);

KEYWORD_OR(conf_snmp_enable_trap_snmp,
	   conf_snmp_enable_trap_snmp_auth,
	   conf_snmp_enable_extend_here,
	   OBJ(int,1), (1<<TRAP_SNMP),
	   "snmp", "Enable SNMP traps", PRIV_CONF);

NOP	(conf_snmp_enable_trap_opts, conf_snmp_enable_trap_snmp,
	 conf_snmp_enable_return_here);

/*
 * {traps | informs} -- either one
 */
KEYWORD_ID (conf_snmp_enable_trap, conf_snmp_enable_trap_opts,
	no_alt, OBJ(int,3), SNMP_ENABLE_TRAPS,
	"traps", "Enable SNMP Traps", PRIV_CONF);
KEYWORD_ID (conf_snmp_enable_inform, conf_snmp_enable_eol,
	conf_snmp_enable_trap, OBJ(int,3), SNMP_ENABLE_INFORMS,
	"informs", "Enable SNMP Informs", PRIV_CONF | PRIV_HIDDEN);
	/* currently no implementation for informs */

NVGENS	(conf_snmp_enable_nvgen, conf_snmp_enable_inform,
	 snmp_service, SNMP_ENABLE);
KEYWORD	(conf_snmp_enable, conf_snmp_enable_nvgen, SNMP_ALTERNATE,
	 "enable", "Enable SNMP Traps or Informs", PRIV_CONF);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_enable

/******************************************************************
 * [no] snmp-server trap-frequency decay  [repeater | ...]
 *
 *	[trap-number | trap-name_string | ...]
 * WARNING!!! This command is ONLY for the trap decay feature for
 * the MAC Security illegal address trap! This  should not 
 * be duplicated ANYWHERE FOR ANY REASON without prior approval
 * from the mib police...in triplicate!  This has only be added here
 * as a hidden command to get the associated object saved over a reboot!
 ******************************************************************/
EOLS	(conf_snmp_trap_freq_eol, snmp_service, SNMP_TRAP_FREQUENCY);

/* 
 * [trap-number | trap-name_string | ...] - additional parameters
 * associated with the trap/inform type.
 * Macros for this parameter can be added per implementation -ONLY
 * with mib police approval!
 */

NOPREFIX (conf_snmp_trap_freq_noprefix, conf_snmp_trap_freq_eol,
	 conf_snmp_trap_freq_eol);

pdecl	(conf_snmp_trap_freq_decay_opts);	/* optional param. for traps */

/* 
 * [repeater |...] -- multiple types allowed
 */
LINK_TRANS(conf_snmp_trap_freq_return_here, conf_snmp_trap_freq_eol);
LINK_TRANS(conf_snmp_trap_freq_extend_here, NONE);
NOP	(conf_snmp_trap_freq_decay_opts, conf_snmp_trap_freq_extend_here,
	 conf_snmp_trap_freq_return_here);

/*
 * {decay | ...}
 */
KEYWORD_ID (conf_snmp_trap_freq_decay, conf_snmp_trap_freq_decay_opts,
	no_alt, OBJ(int,3), SNMP_TRAP_FREQ_DECAY,
	"decay", "set SNMP Trap for decaying repeat", PRIV_CONF|PRIV_HIDDEN);
NVGENS	(conf_snmp_trap_freq_nvgen, conf_snmp_trap_freq_decay,
	 snmp_service, SNMP_TRAP_FREQUENCY);
KEYWORD	(conf_snmp_trap_freq, conf_snmp_trap_freq_nvgen, SNMP_ALTERNATE,
	 "trap-frequency", "set SNMP Traps frequency of repeat",
	 PRIV_CONF|PRIV_HIDDEN);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_trap_freq


/******************************************************************
 * snmp-server system-shutdown
 * no snmp-server system-shutdown
 */
EOLS	(conf_snmp_shutdown_eol, snmp_service, SNMP_SHUTDOWN);
KEYWORD	(conf_snmp_shutdown, conf_snmp_shutdown_eol, SNMP_ALTERNATE,
	 "system-shutdown", "Enable use of the SNMP reload command",
	 PRIV_CONF);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_shutdown
/******************************************************************
 * snmp-server chassis <string>
 * no snmp-server chassis [<string>]
 *
 */
EOLS	(conf_snmp_chassis_eol, snmp_service, SNMP_CHASSIS);
TEXT	(conf_snmp_chassis_string, conf_snmp_chassis_eol, no_alt,
	 OBJ(string,1),
	 "Unique ID string");
NOPREFIX(conf_snmp_chassis_noprefix, conf_snmp_chassis_string,
	 conf_snmp_chassis_eol);
NVGENS	(conf_snmp_chassis_nvgen, conf_snmp_chassis_noprefix,
	 snmp_service, SNMP_CHASSIS);
KEYWORD	(conf_snmp_chassis, conf_snmp_chassis_nvgen, SNMP_ALTERNATE,
	 "chassis-id", "String to uniquely identify this chassis", PRIV_CONF);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_chassis
/******************************************************************
 * snmp-server contact <string>
 * no snmp-server contact [<string>]
 *
 */
EOLS	(conf_snmp_contact_eol, snmp_service, SNMP_CONTACT);
TEXT	(conf_snmp_contact_string, conf_snmp_contact_eol, no_alt,
	 OBJ(string,1),
	 "identification of the contact person for this managed node");
NOPREFIX(conf_snmp_contact_noprefix, conf_snmp_contact_string,
	 conf_snmp_contact_eol);
NVGENS	(conf_snmp_contact_nvgen, conf_snmp_contact_noprefix,
	 snmp_service, SNMP_CONTACT);
KEYWORD	(conf_snmp_contact, conf_snmp_contact_nvgen, SNMP_ALTERNATE,
	 "contact", "Text for mib object sysContact", PRIV_CONF);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_contact
/******************************************************************
 * snmp-server location <string>
 * no snmp-server location
 *
 */
EOLS	(conf_snmp_loc_eol, snmp_service, SNMP_LOCATION);
TEXT	(conf_snmp_loc_text, conf_snmp_loc_eol, no_alt,
	 OBJ(string,1), "The physical location of this node");
NOPREFIX(conf_snmp_loc_noprefix, conf_snmp_loc_text, conf_snmp_loc_eol);
NVGENS	(conf_snmp_loc_nvgen, conf_snmp_loc_noprefix,
	 snmp_service, SNMP_LOCATION);
KEYWORD	(conf_snmp_loc, conf_snmp_loc_nvgen, SNMP_ALTERNATE,
	 "location", "Text for mib object sysLocation", PRIV_CONF);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_loc
/******************************************************************
 * snmp-server queue-length <length>
 * no snmp-server queue-length <length>
 */
PARAMS_KEYONLY (conf_snmp_qlen, SNMP_ALTERNATE,
		"queue-length", OBJ(int,1), 1, 1000,
		snmp_service, SNMP_QLEN,
		"Message queue length for each TRAP host",
		"Queue length (default 10)", PRIV_CONF);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_qlen


/******************************************************************
 * snmp-server sparse-tables         (default)
 * no snmp-server sparse-tables
 */
EOLS	(conf_snmp_sparse_eol, snmp_service, SNMP_NOSPARSE);
KEYWORD	(conf_snmp_sparse, conf_snmp_sparse_eol, SNMP_ALTERNATE,
	 "sparse-tables", "Turn on snmp sparse table implementation",
	 PRIV_HIDDEN);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_sparse


/******************************************************************
 * snmp-server trap-timeout <seconds>
 * no snmp-server trap-timeout [<seconds>]
 */
PARAMS_KEYONLY (conf_snmp_timeout, SNMP_ALTERNATE,
		"trap-timeout", OBJ(int,1), 1, 1000,
		snmp_service, SNMP_TIMEOUT,
		"Set timeout for TRAP message retransmissions",
		"Timeout (default 30 seconds)", PRIV_CONF);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_timeout
/******************************************************************
 * snmp-server trap-authentication [<snmpv1 | snmpv2>]
 * no snmp-server trap-authentication [<snmpv1 | snmpv2>]
 *
 * OBJ(idb,1) = SNMP_PARTY_SNMPV1, SNMP_PARTY_SNMPV2 
 */
EOLS	(conf_snmp_auth_eol, snmp_service, SNMP_AUTH);

KEYWORD_ID (conf_snmp_auth_v2, conf_snmp_auth_eol, conf_snmp_auth_eol,
	    OBJ(int,1), SNMP_AUTH_SNMPV2,
	    "snmpv2", "Send SNMP authentication traps to V2 hosts", 
	    PRIV_CONF);
KEYWORD_ID (conf_snmp_auth_v1, conf_snmp_auth_eol, conf_snmp_auth_v2,
	    OBJ(int,1), SNMP_AUTH_SNMPV1,
	    "snmpv1", "Send SNMP authentication traps to V1 hosts", 
	    PRIV_CONF);

NVGENS	(conf_snmp_auth_nvgen, conf_snmp_auth_v1, snmp_service, SNMP_AUTH);
KEYWORD	(conf_snmp_auth, conf_snmp_auth_nvgen, SNMP_ALTERNATE,
	 "trap-authentication",
	 "Send TRAP messages on receipt of incorrect community string",
	 PRIV_CONF);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_auth
/******************************************************************
 * snmp-server packetsize <bytes>
 * no snmp-server packetsize [<bytes>]
 */
PARAMS_KEYONLY (conf_snmp_packet, SNMP_ALTERNATE,
		"packetsize", OBJ(int,1),
		SNMP_MAX_MSG_SIZE_MIN, SNMP_MAX_MSG_SIZE_MAX_CONST,
		snmp_service, SNMP_PACKET,
		"Largest SNMP packet size",
		"Packet size",
		PRIV_CONF);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_packet
/******************************************************************
 * snmp-server trap-source <interface>
 * [no] snmp-server trap-source [<interface>]
 *
 * OBJ(idb,1) = <interface>
 */
EOLS	(conf_snmp_trap_eol, snmp_service, SNMP_TRAP_INTERFACE);
INTERFACE (conf_snmp_trap_name, conf_snmp_trap_eol, no_alt,
	   OBJ(idb,1), IFTYPE_ANYSUB);
NOPREFIX (conf_snmp_trap_noprefix, conf_snmp_trap_name, conf_snmp_trap_eol);
NVGENS	(conf_snmp_trap_nvgen, conf_snmp_trap_noprefix, 
	 snmp_service, SNMP_TRAP_INTERFACE);
KEYWORD	(conf_snmp_trap, conf_snmp_trap_nvgen, SNMP_ALTERNATE,
	 "trap-source", 
	 "Assign an interface for the source address of all traps", PRIV_CONF);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_trap
/******************************************************************
 * snmp priority {low | normal | high}
 * <hidden command>
 */
EOLS    (conf_snmp_pri_eol, snmp_service, SNMP_PRIORITY);

KEYWORD_ID (conf_snmp_pri_high, conf_snmp_pri_eol, NONE,
            OBJ(int,1), PRIO_HIGH,
            "high", "", PRIV_CONF);
KEYWORD_ID (conf_snmp_pri_normal, conf_snmp_pri_eol, conf_snmp_pri_high,
            OBJ(int,1), PRIO_NORMAL,
            "normal", "", PRIV_CONF);
KEYWORD_ID (conf_snmp_pri_low, conf_snmp_pri_eol, conf_snmp_pri_normal,
            OBJ(int,1), PRIO_LOW,
            "low", "", PRIV_CONF);
NOPREFIX(conf_snmp_pri_noprefix, conf_snmp_pri_low, conf_snmp_pri_eol);
NVGENS  (conf_snmp_pri_nvgen, conf_snmp_pri_noprefix, snmp_service,
	 SNMP_PRIORITY);
KEYWORD (conf_snmp_pri, conf_snmp_pri_nvgen, SNMP_ALTERNATE,
         "priority", "", PRIV_CONF | PRIV_HIDDEN);

#undef SNMP_ALTERNATE
#define SNMP_ALTERNATE conf_snmp_pri
/******************************************************************
 * snmp-server community <string> [view {viewname>}] [{ RO | RW }] [<list>]
 * no snmp-server community <string> [view {viewname>}] [{ RO | RW }] [<list>]
 *
 * OBJ(string,1) = <string>
 * OBJ(string,2) = <viewname>
 * OBJ(int,1) = SR_READ_ONLY, SR_READ_WRITE
 * OBJ(int,2) = <list>
 */
EOLS	(conf_snmp_com_eol, snmp_service, SNMP_COMMUNITY);

/* <list> */
NUMBER	(conf_snmp_com_list, conf_snmp_com_eol, conf_snmp_com_eol,
	 OBJ(int,2), MINFASTACC, MAXFASTACC,
	 "Std IP accesslist allowing access with this community string");

/* { ro | rw } */
KEYWORD_ID (conf_snmp_com_rw, conf_snmp_com_list, conf_snmp_com_list,
	    OBJ(int,1), SR_READ_WRITE,
	    "rw", "Read-write access with this community string", PRIV_CONF);
KEYWORD_ID (conf_snmp_com_ro, conf_snmp_com_list, conf_snmp_com_rw,
	    OBJ(int,1), SR_READ_ONLY,
	    "ro", "Read-only access with this community string", PRIV_CONF);

/* [view {<viewname>}] */

STRING	   (conf_snmp_com_viewname, conf_snmp_com_ro, no_alt,
	    OBJ(string,2), "MIB view to which this community has access");
KEYWORD    (conf_snmp_com_view, conf_snmp_com_viewname, conf_snmp_com_ro,
	    "view", "Restrict this community to a named MIB view", PRIV_CONF);
	    
NOPREFIX(conf_snmp_com_test, conf_snmp_com_view, conf_snmp_com_eol);

STRING	(conf_snmp_com_str, conf_snmp_com_test, no_alt,
	 OBJ(string,1), "SNMP community string");

NVGENS	(conf_snmp_com_nvgen, conf_snmp_com_str, snmp_service, 
	SNMP_COMMUNITY);
KEYWORD_ID(conf_snmp_community, conf_snmp_com_nvgen, SNMP_ALTERNATE,
        OBJ(int,1), SR_READ_ONLY,
	 "community", "Enable SNMP; set community string and access privs",
	 PRIV_CONF);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_community
/******************************************************************
 * snmp-server access-policy <dst party name> <src party name>
 *			     <context name> <privilege> 
 * no snmp-server access-policy <dst party> <src party> <context name>
 *
 * OBJ(string,1) = <dst party name>
 * OBJ(string,2) = <src party name>
 * OBJ(string,3) = <context name>
 * OBJ(int,1) = <privilege>
 *
 */
EOLS	(conf_snmp_accpolicy_eol, snmp_service, SNMP_ACCESS_POLICY);
NUMBER	(conf_snmp_accpolicy_priv, conf_snmp_accpolicy_eol, no_alt,
	 OBJ(int,1), 0, 255,
	 "Bitmap of privileges allowed\n"
	 "0x01 Get Requests\n"
	 "0x02 GetNext Requests\n"
	 "0x04 Get/Set Responses\n"
	 "0x08 Set Requests\n"
	 "0x10 <unused>\n"
	 "0x20 GetBulk Requests\n"
	 "0x40 Inform\n"
	 "0x80 Trap");
NOPREFIX(conf_snmp_accpolicy_noprefix, conf_snmp_accpolicy_priv,
	 conf_snmp_accpolicy_eol);
STRING	(conf_snmp_accpolicy_context, conf_snmp_accpolicy_noprefix, no_alt,
	 OBJ(string,3), "Name of the resource context");
STRING	(conf_snmp_accpolicy_src, conf_snmp_accpolicy_context, no_alt,
	 OBJ(string,2), "Name of the subject party");
STRING	(conf_snmp_accpolicy_dst, conf_snmp_accpolicy_src, no_alt,
	 OBJ(string,1), "Name of the target party");
NVGENS	(conf_snmp_accpolicy_nvgen, conf_snmp_accpolicy_dst,
	 snmp_service, SNMP_ACCESS_POLICY);
KEYWORD	(conf_snmp_accpolicy, conf_snmp_accpolicy_nvgen, SNMP_ALTERNATE,
	 "access-policy", "Define an SNMPv2 access policy (acl)", V2FLAG);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_accpolicy

/******************************************************************
 * snmp-server party <partyname> <partyoid>
 *				[udp <ipaddr> <port>]
 *				[packetsize <packetsize>]
 *				[local]
 *				[remote]
 *				[authentication {
 *                              	md5 <key> [clock <clock>]
 *						  [lifetype <lifetime>]
 *						  [privacy des <key>] |
 *					snmpv1 <community>}]
 * no snmp-server party <partyname>
 *
 * OBJ(string,1) = <partyname>
 * OBJ(string,2) = <partyoid>
 * OBJ(string,3) = <md5 key> or <community>
 * OBJ(string,4) = <des key>
 * OBJ(paddr,1) = <ipaddress>
 * OBJ(int,1) = <protocol>	SNMP_PARTY_PROTO_UDP == udp
 *				SNMP_PARTY_PROTO_IPX == ipx
 *				SNMP_PARTY_PROTO_ATALK == atalk
 * OBJ(int,2) = <port>
 * OBJ(int,3) = <packetsize>
 * OBJ(int,4) = SNMP_PARTY_LOCAL == local
 *		SNMP_PARTY_REMOTE == remote
 * OBJ(int,5) = <authentication>	SNMP_PARTY_AUTH_MD5 == md5
 *					SNMP_PARTY_AUTH_SNMPV1 == snmpv1
 * OBJ(int,6) = TRUE == clock is set
 * OBJ(int,7) = <clock>
 * OBJ(int,8) = TRUE == lifetime is set
 * OBJ(int,9) = <lifetime>
 * OBJ(int,10) = <privacy>		SNMP_PARTY_PRIV_DES == des
 *
 */
EOLS	(conf_snmp_party_eol, snmp_service, SNMP_PARTY);

pdecl(conf_snmp_party_opts); 

pdecl(conf_snmp_party_auth_opts);

STRING  (conf_snmp_party_des_key, conf_snmp_party_auth_opts, no_alt,
	 OBJ(string,4), "The party's DES privacy key");
KEYWORD_ID(conf_snmp_party_des, conf_snmp_party_des_key, no_alt,
	   OBJ(int,10), SNMP_PARTY_PRIV_DES,
	   "des", "Indicates DES privacy", PRIV_CONF);
KEYWORD (conf_snmp_party_priv, conf_snmp_party_des, no_alt,
	 "privacy", "Specifies the party privacy protocol", PRIVFLAG);
NUMBER	(conf_snmp_party_lifetime_num, conf_snmp_party_auth_opts, no_alt,
	 OBJ(int,9), 0, 0x7fffffff, "Authenticated message lifetime");
KEYWORD_ID(conf_snmp_party_lifetime, conf_snmp_party_lifetime_num,
	   conf_snmp_party_priv,
	   OBJ(int,8), TRUE,
	   "lifetime", "Specifies the lifetime of an authenticated message",
	   PRIV_CONF);
NUMBER	(conf_snmp_party_clock_num, conf_snmp_party_auth_opts, no_alt,
	 OBJ(int,7), 0, 0xffffffff, "Value of the party authentication clock");
KEYWORD_ID(conf_snmp_party_clock, conf_snmp_party_clock_num,
	   conf_snmp_party_lifetime,
	   OBJ(int,6), TRUE,
	   "clock", "Specifies the value of the party's authentication clock",
	   PRIV_CONF);
NOP	(conf_snmp_party_auth_opts, conf_snmp_party_clock,
	 conf_snmp_party_opts);
STRING	(conf_snmp_party_md5_key, conf_snmp_party_auth_opts, no_alt,
	 OBJ(string,3), "The party's MD5 authentication key");
KEYWORD_ID(conf_snmp_party_md5, conf_snmp_party_md5_key, no_alt,
	   OBJ(int,5), SNMP_PARTY_AUTH_MD5,
	 "md5", "Indicates MD5 authentication", AUTHFLAG);
STRING	(conf_snmp_party_v1_comm, conf_snmp_party_opts, no_alt,
	 OBJ(string,3), "The party's SNMPv1 community string");
KEYWORD_ID(conf_snmp_party_v1, conf_snmp_party_v1_comm, conf_snmp_party_md5,
	   OBJ(int,5), SNMP_PARTY_AUTH_SNMPV1,
	   "snmpv1", "Indicates SNMPv1 authentication", PRIV_CONF);
KEYWORD	(conf_snmp_party_auth, conf_snmp_party_v1, NONE,
	 "authentication", "Specifies the party authentication protocol",
	 PRIV_CONF);

KEYWORD_ID(conf_snmp_party_remote, conf_snmp_party_opts, conf_snmp_party_auth,
	   OBJ(int,4), SNMP_PARTY_REMOTE,
	   "remote", "Indicates a remote party", PRIV_CONF);
KEYWORD_ID(conf_snmp_party_local, conf_snmp_party_opts, conf_snmp_party_remote,
	   OBJ(int,4), SNMP_PARTY_LOCAL,
	   "local", "Indicates a local party", PRIV_CONF);

NUMBER	(conf_snmp_party_packet_size, conf_snmp_party_opts, no_alt,
	 OBJ(int,3), 484, 65507, "The party's maximum message size");
KEYWORD	(conf_snmp_party_packet, conf_snmp_party_packet_size,
	 conf_snmp_party_local,
	 "packetsize", "Specifies the party's maximum message size",
	 PRIV_CONF);

NUMBER	(conf_snmp_party_udp_port, conf_snmp_party_opts, no_alt,
	 OBJ(int,2), 0, -1, "The party's UDP port number");
IPADDR	(conf_snmp_party_udp_addr, conf_snmp_party_udp_port, no_alt,
	 OBJ(paddr,1), "The party's IP address");
KEYWORD_ID(conf_snmp_party_udp, conf_snmp_party_udp_addr,
	   conf_snmp_party_packet,
	   OBJ(int,1), SNMP_PARTY_PROTO_UDP,
	   "udp", "Indicates the party utilizes SNMP over UDP", PRIV_CONF);

NOP	(conf_snmp_party_opts, conf_snmp_party_udp, conf_snmp_party_eol);

STRING	(conf_snmp_party_oid, conf_snmp_party_opts, no_alt,
	 OBJ(string,2), "Object Identifier of the party");
NOPREFIX(conf_snmp_party_noprefix, conf_snmp_party_oid, conf_snmp_party_eol);
STRING	(conf_snmp_party_name, conf_snmp_party_noprefix, no_alt,
	 OBJ(string,1), "Name of the party");
NVGENS	(conf_snmp_party_nvgen, conf_snmp_party_name,
	 snmp_service, SNMP_PARTY);
KEYWORD	(conf_snmp_party, conf_snmp_party_nvgen, SNMP_ALTERNATE,
	 "party", "Define an SNMPv2 party", V2FLAG);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_party
/******************************************************************
 * snmp-server context <contextname> <contextoid> <viewname> 
 * no snmp-server context <contextname>
 *
 * OBJ(string,1) = <contextname>
 * OBJ(string,2) = <contextoid>
 * OBJ(string,3) = <viewname>
 *
 */
EOLS	(conf_snmp_context_eol, snmp_service, SNMP_CONTEXT);
STRING	(conf_snmp_context_view, conf_snmp_context_eol, no_alt,
	 OBJ(string,3), "Name of the MIB view permitted to this context");
STRING	(conf_snmp_context_oid, conf_snmp_context_view, no_alt,
	 OBJ(string,2), "Object identifier of the context");
NOPREFIX(conf_snmp_context_noprefix, conf_snmp_context_oid,
	 conf_snmp_context_eol);
STRING	(conf_snmp_context_name, conf_snmp_context_noprefix, no_alt,
	 OBJ(string,1), "Name of the context");
NVGENS	(conf_snmp_context_nvgen, conf_snmp_context_name,
	 snmp_service, SNMP_CONTEXT);
KEYWORD	(conf_snmp_context, conf_snmp_context_nvgen, SNMP_ALTERNATE,
	 "context", "Define an SNMPv2 context", V2FLAG);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_context
/******************************************************************
 * snmp-server view <viewname> <oidtree>
 *		    {included | excluded} 
 * no snmp-server view <viewname> <oidtree>
 *
 * OBJ(string,1) = <viewname>
 * OBJ(string,2) = <oidtree>
 * OBJ(int,1) =	TRUE == included
 *		FALSE == excluded
 *
 */
EOLS	(conf_snmp_view_eol, snmp_service, SNMP_VIEW);
KEYWORD_ID(conf_snmp_view_excluded, conf_snmp_view_eol, no_alt,
	   OBJ(int,1), FALSE,
	   "excluded", "MIB family is excluded from the view", PRIV_CONF);
KEYWORD_ID(conf_snmp_view_included, conf_snmp_view_eol,
	   conf_snmp_view_excluded,
	   OBJ(int,1), TRUE,
	   "included", "MIB family is included in the view", PRIV_CONF);
NOPREFIX(conf_snmp_view_noprefix2, conf_snmp_view_included, conf_snmp_view_eol);
STRING	(conf_snmp_view_oidtree, conf_snmp_view_noprefix2, no_alt,
	 OBJ(string,2), "MIB view family name");
NOPREFIX(conf_snmp_view_noprefix1, conf_snmp_view_oidtree, conf_snmp_view_eol);
STRING	(conf_snmp_view_name, conf_snmp_view_noprefix1, no_alt,
	 OBJ(string,1), "Name of the view");
NVGENS	(conf_snmp_view_nvgen, conf_snmp_view_name, snmp_service, SNMP_VIEW);
KEYWORD	(conf_snmp_view, conf_snmp_view_nvgen, SNMP_ALTERNATE,
	 "view", "Define an SNMPv2 MIB view", V2FLAG);

#undef  SNMP_ALTERNATE
#define SNMP_ALTERNATE   conf_snmp_view
/******************************************************************/
KEYWORD	(conf_snmp, SNMP_ALTERNATE, ALTERNATE,
	 "snmp-server", "Modify SNMP parameters", PRIV_CONF);

#undef  ALTERNATE
#define	ALTERNATE	conf_snmp
