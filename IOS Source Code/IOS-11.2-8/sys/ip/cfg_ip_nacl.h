/* $Id: cfg_ip_nacl.h,v 3.1.2.6 1996/08/12 16:02:50 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_ip_nacl.h,v $
 *------------------------------------------------------------------
 * cfg_ip_nacl.h -- IP Named Access List Parser Commands
 *
 * March 1996, Greg Christy
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_nacl.h,v $
 * Revision 3.1.2.6  1996/08/12  16:02:50  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.5  1996/07/24  00:16:09  widmer
 * CSCdi63069:  2 ip choices presented by parser in config mode
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/07/20  01:13:09  bew
 * CSCdi63411:  ACL encrypted/decrypted flags are not useful
 * Branch: California_branch
 * Remove encrypted/decrypted flags introduced early in 11.2
 * from ACL code.
 *
 * Revision 3.1.2.3  1996/06/17  23:31:53  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/20  07:52:13  gchristy
 * CSCdi57884:  IP NACL: functionality should be identical to numeric lists
 * Branch: California_branch
 * - Support crypto_flags and associated keywords in parser
 *
 * Revision 3.1.2.1  1996/05/04  08:00:29  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.1  1996/05/04  07:48:41  speakman
 * Two more (the last two?) missing placeholders for
 * IP Named Access Lists.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************
 * IP ext access list
 *
 * [dynamic <name>] { permit | deny } <protocol>
 *	<address-mask> [<modifier>] <address-mask> [<modifier>] [established]
 *      [<tos>] [<icmp-typecode>] [<igmp-type>]
 *
 * no [dynamic <name>] [{ permit | deny } <protocol>
 *	<address-mask> [<modifier>] <address-mask> [<modifier>] [established]
 *	[<tos>] [<icmp-typecode>] [<igmp-type>]
 *
 * <protocol>:	   { ip | tcp | udp | icmp | igrp | igmp | gre | nos | ospf
 *		        | eigrp | ipinip | dvmrp}
 *
 * <address-mask>: { <ipaddress> <ipmask> |
 *		   "any" |
 *		   "host" <ipaddress> }
 *
 * <modifier>:     {"lt"  <port>   (TCP or UDP only)
 *		   |"gt"  <port>   (TCP or UDP only)
 *		   |"eq"  <port>   (TCP or UDP only)
 *		   |"neq" <port>   (TCP or UDP only)
 *		   |"range" <port> <port> (TCP or UDP only)
 *
 * <port>:	   see ../ip/ip_actions.c
 *
 * established only applies to TCP.
 * ICMP typecodes only apply to ICMP.
 * IGMP types only apply to IGMP.
 *
 * <precedence>:   {"precedence" <value>"}
 *
 * OBJ(int,1) = access list number
 * OBJ(int,2) = TRUE for permit; FALSE for deny
 * OBJ(int,3) = protocol code
 * OBJ(paddr,1)= source addr
 * OBJ(paddr,2)= source mask
 * OBJ(paddr,3)= dest addr
 * OBJ(paddr,4)= dest mask
 * OBJ(int,4) = destination operation code
 * OBJ(int,5) = destination port number, ICMP type
 * OBJ(int,6) = second destination port number, ICMP code
 * OBJ(int,7) = TRUE if established
 * OBJ(int,8) = source operation code
 * OBJ(int,9) = source port number
 * OBJ(int,10) = second source port number
 * OBJ(int,11) = TRUE if precedence given
 * OBJ(int,12) = precedence value
 * OBJ(int,13) = TRUE if tos given
 * OBJ(int,14) = tos value
 * OBJ(int,15) = TRUE if logging
 * OBJ(int,16) = TRUE==DYNAMIC, FALSE==PERMANENT
 * OBJ(int,17) = timeout for dynamic acl
 * OBJ(string,1) = Name of Dynamic list
 */

EOLNS	(conf_ip_nacl_exit_eol, exit_config_submode);
KEYWORD	(conf_ip_nacl_exit_kwd, conf_ip_nacl_exit_eol, NONE,
	 "exit", "Exit from access-list configuration mode", PRIV_MIN);
TESTVAR	(conf_ip_nacl_exit, NONE, conf_ip_nacl_exit_kwd, NONE,
	 NONE, ALTERNATE, sense, FALSE);

EOLNS	(conf_ip_nacl_ext_eol, ip_nacl_extended_command);

/*
 * End of options.  This allows all options to occur in any order.
 */

pdecl(conf_ip_nacl_ext_opts);

/*
 * log-input
 */
KEYWORD_ID_MM (conf_ip_nacl_ext_logif, conf_ip_nacl_ext_opts,
	       conf_ip_nacl_ext_eol,
	       OBJ(int,15), IP_ACCESS_LOG_INPUT,
	       "log-input",
	       "Log matches against this entry, including input interface",
	       PRIV_CONF, 4);  
/*
 * log
 */
KEYWORD_ID (conf_ip_nacl_ext_iplog, conf_ip_nacl_ext_opts,
	    conf_ip_nacl_ext_logif, 
	    OBJ(int,15), IP_ACCESS_LOG_NORMAL,
	    "log", "Log matches against this entry", PRIV_CONF);

/* tos <value>
 *     1000   --   minimize delay
 *     0100   --   maximize throughput
 *     0010   --   maximize reliability
 *     0001   --   minimize monetary cost
 *     0000   --   normal service
 */

KEYWORD_ID (conf_ip_nacl_ext_iptosnormal,
	    conf_ip_nacl_ext_opts, no_alt, 
	    OBJ(int, 14), IP_TOS_NORMAL, TOS_NORMAL_NAME,
	    "Match packets with normal TOS (0)", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_iptoscost,
	    conf_ip_nacl_ext_opts,
	    conf_ip_nacl_ext_iptosnormal, 
	    OBJ(int, 14), IP_TOS_LOW_COST, TOS_COST_NAME,
	    "Match packets with min monetary cost TOS (1)", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_iptosreliab,
	    conf_ip_nacl_ext_opts,
	    conf_ip_nacl_ext_iptoscost, 
	    OBJ(int, 14), IP_TOS_HIGH_RELY, TOS_RELIABLE_NAME,
	    "Match packets with max reliable TOS (2)", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_iptosthrough,
	    conf_ip_nacl_ext_opts,
	    conf_ip_nacl_ext_iptosreliab, 
	    OBJ(int, 14), IP_TOS_HIGH_TPUT, TOS_THROUGH_NAME,
	    "Match packets with max throughput TOS (4)", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_iptosdelay,
	    conf_ip_nacl_ext_opts,
	    conf_ip_nacl_ext_iptosthrough, 
	    OBJ(int, 14), IP_TOS_LOW_DELAY, TOS_DELAY_NAME,
	    "Match packets with min delay TOS (8)", PRIV_CONF);

NUMBER	(conf_ip_nacl_ext_tos, conf_ip_nacl_ext_opts,
	 conf_ip_nacl_ext_iptosdelay, OBJ(int, 14), 0, 15, 
	 "Type of service value");

KEYWORD_ID (conf_ip_nacl_ext_iptos, conf_ip_nacl_ext_tos,
	    conf_ip_nacl_ext_iplog, 
	    OBJ(int, 13), TRUE,
	    "tos", "Match packets with given TOS value", PRIV_CONF);

/*
 * precedence <value>
 *        111 - Network Control
 *        110 - Internetwork Control
 *        101 - CRITIC/ECP
 *        100 - Flash Override
 *        011 - Flash
 *        010 - Immediate
 *        001 - Priority
 *        000 - Routine
 * Values are taken from RFC 791
 */

KEYWORD_ID (conf_ip_nacl_ext_ippredroutine,
	    conf_ip_nacl_ext_opts, no_alt, OBJ(int, 12), 
	    PRECEDENCE_ROUTINE, PRECEDENCE_ROUTINE_STR,
	    "Match packets with routine precedence (0)", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_ippredpriority,
	    conf_ip_nacl_ext_opts,
	    conf_ip_nacl_ext_ippredroutine, 
	    OBJ(int, 12), PRECEDENCE_PRIORITY, PRECEDENCE_PRIORITY_STR,
	    "Match packets with priority precedence (1)", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_ippredimmediate,
	    conf_ip_nacl_ext_opts,
	    conf_ip_nacl_ext_ippredpriority, 
	    OBJ(int, 12), 
	    PRECEDENCE_IMMEDIATE, PRECEDENCE_IMMEDIATE_STR,
	    "Match packets with immediate precedence (2)", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_ippredflash,
	    conf_ip_nacl_ext_opts,
	    conf_ip_nacl_ext_ippredimmediate, 
	    OBJ(int, 12), PRECEDENCE_FLASH, PRECEDENCE_FLASH_STR,
	    "Match packets with flash precedence (3)", PRIV_CONF);

KEYWORD_ID_MM(conf_ip_nacl_ext_ippredflashover,
	      conf_ip_nacl_ext_opts,
	      conf_ip_nacl_ext_ippredflash, 
	      OBJ(int, 12), PRECEDENCE_FLASHOVER,
	      PRECEDENCE_FLASHOVER_STR,
	      "Match packets with flash override precedence (4)", PRIV_CONF, 6);

KEYWORD_ID (conf_ip_nacl_ext_ippredcrit,
	    conf_ip_nacl_ext_opts,
	    conf_ip_nacl_ext_ippredflashover, OBJ(int, 12), 
	    PRECEDENCE_CRITICAL, PRECEDENCE_CRITICAL_STR,
	    "Match packets with critical precedence (5)", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_ippredinternet,
	    conf_ip_nacl_ext_opts,
	    conf_ip_nacl_ext_ippredcrit, OBJ(int, 12), 
	    PRECEDENCE_INTERNET, PRECEDENCE_INTERNET_STR,
	    "Match packets with internetwork control precedence (6)",
	    PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_ipprednet,
	    conf_ip_nacl_ext_opts,
	    conf_ip_nacl_ext_ippredinternet, OBJ(int, 12), 
	    PRECEDENCE_NETWORK, PRECEDENCE_NETWORK_STR,
	    "Match packets with network control precedence (7)", PRIV_CONF);

NUMBER	(conf_ip_nacl_ext_precedence, conf_ip_nacl_ext_opts,
	 conf_ip_nacl_ext_ipprednet, OBJ(int, 12), 0, 7, 
	 "Precedence value");

KEYWORD_ID(conf_ip_nacl_ext_ipprecedence,
	   conf_ip_nacl_ext_precedence,
	   conf_ip_nacl_ext_iptos, 
	   OBJ(int,11), TRUE, "precedence",
	    "Match packets with given precedence value", PRIV_CONF);

/*
 * Parse IGMP message types.
 */

SET	(conf_ip_nacl_ext_igmptypepresent,
	 conf_ip_nacl_ext_opts, OBJ(int, 4), OPC_ONEBYTE); 

KEYWORD_ID (conf_ip_nacl_ext_igmptrace,
	    conf_ip_nacl_ext_igmptypepresent,
	    conf_ip_nacl_ext_ipprecedence, 
	    OBJ(int, 5), IGMP_TRACE_TYPE, IGMP_TRACE_NAME,
	    "Multicast trace", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_igmppim,
	    conf_ip_nacl_ext_igmptypepresent,
	    conf_ip_nacl_ext_igmptrace, 
	    OBJ(int, 5), IGMP_PIM_TYPE, IGMP_PIM_NAME,
	    "Protocol Independent Multicast", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_igmpdvmrp,
	    conf_ip_nacl_ext_igmptypepresent,
	    conf_ip_nacl_ext_igmppim, 
	    OBJ(int, 5), IGMP_DVMRP_TYPE, IGMP_DVMRP_NAME,
	    "Distance Vector Multicast Routing Protocol", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_igmphostreport,
	    conf_ip_nacl_ext_igmptypepresent,
	    conf_ip_nacl_ext_igmpdvmrp, 
	    OBJ(int, 5), IGMP_REPORT_TYPE, IGMP_REPORT_NAME,
	    "Host report", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_igmphostquery,
	    conf_ip_nacl_ext_igmptypepresent,
	    conf_ip_nacl_ext_igmphostreport, 
	    OBJ(int, 5), IGMP_QUERY_TYPE, IGMP_QUERY_NAME,
	    "Host query", PRIV_CONF);

NUMBER	(conf_ip_nacl_ext_igmptype,
	 conf_ip_nacl_ext_igmptypepresent,
	 conf_ip_nacl_ext_igmphostquery, 
	 OBJ(int, 5), 0, 15, "IGMP message type");

/*
 * Parse ICMP typecodes.  Only applies to ICMP.
 * Deal with the case of filtering on specific type and code.
 */

SET	(conf_ip_nacl_ext_icmpcodepresent,
	 conf_ip_nacl_ext_opts, OBJ(int, 4), OPC_TWOBYTES); 

NUMBER (conf_ip_nacl_ext_icmpcode,
	conf_ip_nacl_ext_icmpcodepresent,
	conf_ip_nacl_ext_eol, 
	OBJ(int, 6), 0, 255, "ICMP message code");

SET	(conf_ip_nacl_ext_icmpparam,
	 conf_ip_nacl_ext_icmpcodepresent, OBJ(int, 5),
	 ICMP_PARAMETER); 

KEYWORD_ID (conf_ip_nacl_ext_icmpparamreqnoroom,
	    conf_ip_nacl_ext_icmpparam, 
	    conf_ip_nacl_ext_ipprecedence,
	    OBJ(int, 6), ICMP_PARAMREQNOROOM, ICMP_PARAMREQNOROOM_NAME,
	    "Parameter required but no room", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpparamrequired,
	    conf_ip_nacl_ext_icmpparam, 
	    conf_ip_nacl_ext_icmpparamreqnoroom,
	    OBJ(int, 6), ICMP_PARAMREQUIRED, ICMP_PARAMREQUIRED_NAME,
	    "Parameter required but not present", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpparamprob,
	    conf_ip_nacl_ext_icmpparam, 
	    conf_ip_nacl_ext_icmpparamrequired,
	    OBJ(int, 6), ICMP_PARAMPROB, ICMP_PARAMPROB_NAME,
	    "Parameter problem", PRIV_CONF);

SET	(conf_ip_nacl_ext_icmptim,
	 conf_ip_nacl_ext_icmpcodepresent, OBJ(int, 5), 
	 ICMP_TIMEEXCEEDED);

KEYWORD_ID (conf_ip_nacl_ext_icmptimfrag,
	    conf_ip_nacl_ext_icmptim, 
	    conf_ip_nacl_ext_icmpparamprob,
	    OBJ(int, 6), ICMP_TIMFRAG, ICMP_TIMFRAG_NAME,
	    "Reassembly timeout", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmptimttl,
	    conf_ip_nacl_ext_icmptim, 
	    conf_ip_nacl_ext_icmptimfrag,
	    OBJ(int, 6), ICMP_TIMTTL, ICMP_TIMTTL_NAME,
	    "TTL exceeded", PRIV_CONF);

SET	(conf_ip_nacl_ext_icmpred,
	 conf_ip_nacl_ext_icmpcodepresent, OBJ(int, 5),
	 ICMP_REDIRECT); 

KEYWORD_ID (conf_ip_nacl_ext_icmpredservhost,
	    conf_ip_nacl_ext_icmpred, 
	    conf_ip_nacl_ext_icmptimttl,
	    OBJ(int, 6), ICMP_REDSERVHOST, ICMP_REDSERVHOST_NAME,
	    "Host redirect for TOS", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpredservnet,
	    conf_ip_nacl_ext_icmpred, 
	    conf_ip_nacl_ext_icmpredservhost,
	    OBJ(int, 6), ICMP_REDSERVNET, ICMP_REDSERVNET_NAME,
	    "Net redirect for TOS", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpredhost,
	    conf_ip_nacl_ext_icmpred, 
	    conf_ip_nacl_ext_icmpredservnet,
	    OBJ(int, 6), ICMP_REDHOST, ICMP_REDHOST_NAME,
	    "Host redirect", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmprednet,
	    conf_ip_nacl_ext_icmpred, 
	    conf_ip_nacl_ext_icmpredhost,
	    OBJ(int, 6), ICMP_REDNET, ICMP_REDNET_NAME,
	    "Network redirect", PRIV_CONF);

SET	(conf_ip_nacl_ext_icmpunr,
	 conf_ip_nacl_ext_icmpcodepresent, OBJ(int, 5), 
	 ICMP_UNREACHABLE);

KEYWORD_ID_MM(conf_ip_nacl_ext_icmpunrprecedence,
	      conf_ip_nacl_ext_icmpunr,
	      conf_ip_nacl_ext_icmprednet, 
	      OBJ(int, 6), ICMP_UNRPRECEDENCE,
	      ICMP_UNRPRECEDENCE_NAME, "Precedence cutoff", PRIV_CONF, 11);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrhostprec,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrprecedence,
	    OBJ(int, 6), ICMP_UNRHOSTPREC, ICMP_UNRHOSTPREC_NAME,
	    "Host unreachable for precedence", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunradmin,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrhostprec,
	    OBJ(int, 6), ICMP_UNRADMIN, ICMP_UNRADMIN_NAME,
	    "Administratively prohibited", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrhosttos,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunradmin,
	    OBJ(int, 6), ICMP_UNRHOSTTOS, ICMP_UNRHOSTTOS_NAME,
	    "Host unreachable for TOS", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrnettos,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrhosttos,
	    OBJ(int, 6), ICMP_UNRNETTOS, ICMP_UNRNETTOS_NAME,
	    "Network unreachable for TOS", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrprohib,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrnettos,
	    OBJ(int, 6), ICMP_UNRPROHIB, ICMP_UNRPROHIB_NAME,
	    "Host prohibited", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrnetprohib,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrprohib,
	    OBJ(int, 6), ICMP_UNRNETPROHIB, ICMP_UNRNETPROHIB_NAME,
	    "Net prohibited", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrhostisolated,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrnetprohib,
	    OBJ(int, 6), ICMP_UNRHOSTISOLATED, ICMP_UNRHOSTISOLATED_NAME,
	    "Host isolated", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrnohost,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrhostisolated,
	    OBJ(int, 6), ICMP_UNRNOHOST, ICMP_UNRNOHOST_NAME,
	    "Host unknown", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrnonet,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrnohost,
	    OBJ(int, 6), ICMP_UNRNONET, ICMP_UNRNONET_NAME,
	    "Network unknown", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrsrfail,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrnonet,
	    OBJ(int, 6), ICMP_UNRSRFAIL, ICMP_UNRSRFAIL_NAME,
	    "Source route failed", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrfrag,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrsrfail,
	    OBJ(int, 6), ICMP_UNRFRAG, ICMP_UNRFRAG_NAME,
	    "Fragmentation needed and DF set", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrport,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrfrag,
	    OBJ(int, 6), ICMP_UNRPORT, ICMP_UNRPORT_NAME,
	    "Port unreachable", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrprotocol,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrport,
	    OBJ(int, 6), ICMP_UNRPORT, ICMP_UNRPROTOCOL_NAME,
	    "Protocol unreachable", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrhost,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrprotocol,
	    OBJ(int, 6), ICMP_UNRHOST, ICMP_UNRHOST_NAME,
	    "Host unreachable", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunrnet,
	    conf_ip_nacl_ext_icmpunr, 
	    conf_ip_nacl_ext_icmpunrhost,
	    OBJ(int, 6), ICMP_UNRNET, ICMP_UNRNET_NAME,
	    "Net unreachable", PRIV_CONF);

/*
 * ICMP types only
 */

SET	(conf_ip_nacl_ext_icmptypepresent,
	 conf_ip_nacl_ext_opts, OBJ(int, 4), OPC_ONEBYTE); 

KEYWORD_ID (conf_ip_nacl_ext_icmpmobileredirect,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmpunrnet,
	    OBJ(int, 5), ICMP_MOBILEREDIRECT, ICMP_MOBILEREDIRECT_NAME,
	    "Mobile host redirect", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpconversion,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmpmobileredirect,
	    OBJ(int, 5), ICMP_CONVERSION, ICMP_CONVERSION_NAME,
	    "Datagram conversion", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmptraceroute,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmpconversion,
	    OBJ(int, 5), ICMP_TRACEROUTE, ICMP_TRACEROUTE_NAME,
	    "Traceroute", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpmaskreply,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmptraceroute,
	    OBJ(int, 5), ICMP_MASKREPLY, ICMP_MASKREPLY_NAME,
	    "Mask replies", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpmaskrequest,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmpmaskreply,
	    OBJ(int, 5), ICMP_MASKREQUEST, ICMP_MASKREQUEST_NAME,
	    "Mask requests", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpinforeply,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmpmaskrequest,
	    OBJ(int, 5), ICMP_INFOREPLY, ICMP_INFOREPLY_NAME,
	    "Information replies", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpinforequest,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmpinforeply,
	    OBJ(int, 5), ICMP_INFOREQUEST, ICMP_INFOREQUEST_NAME,
	    "Information requests", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmptimestampreply,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmpinforequest,
	    OBJ(int, 5), ICMP_TIMESTAMPREPLY, ICMP_TIMESTAMPREPLY_NAME,
	    "Timestamp replies", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmptimestamp,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmptimestampreply,
	    OBJ(int, 5), ICMP_TIMESTAMP, ICMP_TIMESTAMP_NAME,
	    "Timestamp requests", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpparameter,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmptimestamp, 
	    OBJ(int, 5), ICMP_PARAMETER, ICMP_PARAMETER_NAME,
	    "All parameter problems", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmptimeexceeded,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmpparameter,
	    OBJ(int, 5), ICMP_TIMEEXCEEDED, ICMP_TIMEEXCEEDED_NAME,
	    "All time exceededs", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmprdpsolicit,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmptimeexceeded,
	    OBJ(int, 5), ICMP_RDPSOLICIT, ICMP_RDPSOLICIT_NAME,
	    "Router discovery solicitations", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmprdpadvert,
	    conf_ip_nacl_ext_icmptypepresent, 
	    conf_ip_nacl_ext_icmprdpsolicit,
	    OBJ(int, 5), ICMP_RDPADVERT, ICMP_RDPADVERT_NAME,
	    "Router discovery advertisements", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpaltaddress,
	    conf_ip_nacl_ext_icmptypepresent,
	    conf_ip_nacl_ext_icmprdpadvert, 
	    OBJ(int, 5), ICMP_ALTADDRESS, ICMP_ALTADDRESS_NAME,
	    "Alternate address", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpecho,
	    conf_ip_nacl_ext_icmptypepresent,
	    conf_ip_nacl_ext_icmpaltaddress, 
	    OBJ(int, 5), ICMP_ECHO, ICMP_ECHO_NAME,
	    "Echo (ping)", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpredirect,
	    conf_ip_nacl_ext_icmptypepresent,
	    conf_ip_nacl_ext_icmpecho, 
	    OBJ(int, 5), ICMP_REDIRECT, ICMP_REDIRECT_NAME,
	    "All redirects", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpquench,
	    conf_ip_nacl_ext_icmptypepresent,
	    conf_ip_nacl_ext_icmpredirect, 
	    OBJ(int, 5), ICMP_QUENCH, ICMP_QUENCH_NAME,
	    "Source quenches", PRIV_CONF);

KEYWORD_ID (conf_ip_nacl_ext_icmpunreach,
	    conf_ip_nacl_ext_icmptypepresent,
	    conf_ip_nacl_ext_icmpquench, 
	    OBJ(int, 5), ICMP_UNREACHABLE, ICMP_UNREACHABLE_NAME,
	    "All unreachables", PRIV_CONF);

KEYWORD_ID_MM(conf_ip_nacl_ext_icmpechoreply,
	      conf_ip_nacl_ext_icmptypepresent,
	      conf_ip_nacl_ext_icmpunreach, 
	      OBJ(int, 5), ICMP_ECHOREPLY,
	      ICMP_ECHOREPLY_NAME, "Echo reply", PRIV_CONF, 5);

SET	(conf_ip_nacl_ext_icmptypeset,
	 conf_ip_nacl_ext_icmpcode, OBJ(int, 4), OPC_ONEBYTE); 

NUMBER (conf_ip_nacl_ext_icmptype,
	conf_ip_nacl_ext_icmptypeset,
	conf_ip_nacl_ext_icmpechoreply, 
	OBJ(int, 5), 0, 255, "ICMP message type");

/*
 * Parse [established]
 * This only applies for TCP
 */
KEYWORD_ID (conf_ip_nacl_ext_tcpest, conf_ip_nacl_ext_opts,
	    NONE, 
	    OBJ(int,7), TRUE,
	    "established", "Match established connections", PRIV_CONF);

TESTVAR	(conf_ip_nacl_ext_tcpopts, conf_ip_nacl_ext_tcpest,
	 NONE, NONE, NONE, conf_ip_nacl_ext_ipprecedence, 
	 OBJ(int,3), TCP_PROT);

/*
 * TCP destination ports
 */

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM(conf_ip_nacl_ext_dtacacsds,
	      conf_ip_nacl_ext_opts, no_alt, 
	      OBJ(int, 5), TACACS_PORT,
	      "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (conf_ip_nacl_ext_tcpdstport,
	  conf_ip_nacl_ext_opts,
	  conf_ip_nacl_ext_dtacacsds, OBJ(int, 5)); 

/*
 * UDP destination ports
 */

UDP_PORT (conf_ip_nacl_ext_udpdstport,
	  conf_ip_nacl_ext_opts,
	  conf_ip_nacl_ext_dtacacsds, OBJ(int, 5)); 

TESTVAR	(conf_ip_nacl_ext_dstport,
	 conf_ip_nacl_ext_tcpdstport,
	 conf_ip_nacl_ext_udpdstport, 
	 NONE, NONE, NONE, OBJ(int,3), TCP_PROT);

/*
 * TCP destination port range
 */

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards
   compatibility */ 
KEYWORD_ID_MM (conf_ip_nacl_ext_tacacsdsr2,
	       conf_ip_nacl_ext_opts, no_alt, 
	       OBJ(int, 6), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (conf_ip_nacl_ext_tcpdstp2, conf_ip_nacl_ext_opts,
	  conf_ip_nacl_ext_tacacsdsr2, OBJ(int,6)); 

KEYWORD_ID_MM (conf_ip_nacl_ext_tacacsdsr1,
	       conf_ip_nacl_ext_tcpdstp2, no_alt, 
	       OBJ(int, 5), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (conf_ip_nacl_ext_tcpdstrange,
	  conf_ip_nacl_ext_tcpdstp2,
	  conf_ip_nacl_ext_tacacsdsr1, OBJ(int,5)); 

/*
 * UDP destination port range
 */

UDP_PORT (conf_ip_nacl_ext_udpdstp2, conf_ip_nacl_ext_opts,
	  conf_ip_nacl_ext_tacacsdsr2, OBJ(int, 6)); 

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (conf_ip_nacl_ext_udptacacsdsr1,
	       conf_ip_nacl_ext_udpdstp2, no_alt, 
	       OBJ(int, 5), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
UDP_PORT (conf_ip_nacl_ext_udpdstrange,
	  conf_ip_nacl_ext_udpdstp2,
	  conf_ip_nacl_ext_udptacacsdsr1, 
	  OBJ(int, 5));

TESTVAR	(conf_ip_nacl_ext_dstrange,
	 conf_ip_nacl_ext_tcpdstrange,
	 conf_ip_nacl_ext_udpdstrange, 
	 NONE, NONE, NONE, OBJ(int,3), TCP_PROT);

/*
 * { lt | gt | eq | neq | range }
 */

KEYWORD_ID(conf_ip_nacl_ext_iprange,
	   conf_ip_nacl_ext_dstrange,
	   conf_ip_nacl_ext_tcpopts, 
	   OBJ(int, 4), OPC_RANGE, OPC_RANGE_STR,
	   "Match only packets in the range of port numbers",
	   PRIV_CONF);
KEYWORD_ID(conf_ip_nacl_ext_ipneq, conf_ip_nacl_ext_dstport,
	   conf_ip_nacl_ext_iprange, 
	   OBJ(int,4), OPC_NEQ,
	   OPC_NEQ_STR,
	   "Match only packets not on a given port number",
	   PRIV_CONF);
KEYWORD_ID(conf_ip_nacl_ext_ipeq, conf_ip_nacl_ext_dstport,
	   conf_ip_nacl_ext_ipneq, 
	   OBJ(int,4), OPC_EQ,
	   OPC_EQ_STR,
	   "Match only packets on a given port number",
	   PRIV_CONF);
KEYWORD_ID(conf_ip_nacl_ext_ipgt, conf_ip_nacl_ext_dstport,
	   conf_ip_nacl_ext_ipeq, 
	   OBJ(int,4), OPC_GT,
	   OPC_GT_STR,
	   "Match only packets with a greater port number",
	   PRIV_CONF);
KEYWORD_ID(conf_ip_nacl_ext_iplt, conf_ip_nacl_ext_dstport,
	   conf_ip_nacl_ext_ipgt, 
	   OBJ(int,4), OPC_LT,
	   OPC_LT_STR,
	   "Match only packets with a lower port number",
	   PRIV_CONF);

TESTVAR	(conf_ip_nacl_ext_transport2,
	 conf_ip_nacl_ext_icmptype, conf_ip_nacl_ext_iplt, 
	 NONE, NONE, NONE, OBJ(int,3), ICMP_PROT);
TESTVAR	(conf_ip_nacl_ext_transport,
	 conf_ip_nacl_ext_igmptype,
	 conf_ip_nacl_ext_transport2, 
	 NONE, NONE, NONE, OBJ(int,3), IGMP_PROT);

/*
 * Only provide the above options for TCP, UDP and ICMP.
 */

IFELSE	(conf_ip_nacl_ext_opts, conf_ip_nacl_ext_transport,
	 conf_ip_nacl_ext_ipprecedence, 
	 ((GETOBJ(int,3) == TCP_PROT) ||
	  (GETOBJ(int,3) == UDP_PROT) ||
	  (GETOBJ(int,3) == ICMP_PROT)||
	  (GETOBJ(int,3) == IGMP_PROT)));

/*
 * Parse a destination
 * <dstaddr> <dstmask>
 * "any"
 * "host" <dstaddr>"
 */

IPADDR_NAME(conf_ip_nacl_ext_dsthostaddr,
	    conf_ip_nacl_ext_opts, no_alt, 
	 OBJ(paddr, 3), "Destination address");
KEYWORD (conf_ip_nacl_ext_dsthost,
	 conf_ip_nacl_ext_dsthostaddr, no_alt, 
	 "host", "A single destination host", PRIV_CONF);

KEYWORD_ID(conf_ip_nacl_ext_dstany, conf_ip_nacl_ext_opts,
	   conf_ip_nacl_ext_dsthost, 
	   OBJ(paddr, 4).ip_addr, WILD_ANY,
	   "any", "Any destination host", PRIV_CONF);

IPADDR	(conf_ip_nacl_ext_ipaddr4, conf_ip_nacl_ext_opts,
	 no_alt, 
	 OBJ(paddr,4), "Destination wildcard bits");
IPADDR	(conf_ip_nacl_ext_ipaddr3, conf_ip_nacl_ext_ipaddr4,
	 conf_ip_nacl_ext_dstany, 
	 OBJ(paddr,3),"Destination address");

/*
 * TCP source ports
 */

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (conf_ip_nacl_ext_srctacacsds,
	       conf_ip_nacl_ext_ipaddr3, no_alt, 
	       OBJ(int, 9), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (conf_ip_nacl_ext_tcpsrcport,
	  conf_ip_nacl_ext_ipaddr3,
	  conf_ip_nacl_ext_srctacacsds, OBJ(int, 9)); 

/*
 * UDP source ports
 */

UDP_PORT (conf_ip_nacl_ext_udpsrcport,
	  conf_ip_nacl_ext_ipaddr3,
	  conf_ip_nacl_ext_srctacacsds, OBJ(int, 9)); 

TESTVAR	(conf_ip_nacl_ext_srcport,
	 conf_ip_nacl_ext_tcpsrcport,
	 conf_ip_nacl_ext_udpsrcport, 
	 NONE, NONE, NONE, OBJ(int,3), TCP_PROT); 

/*
 * TCP source port range
 */

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (conf_ip_nacl_ext_srctacacsdsr2,
	       conf_ip_nacl_ext_ipaddr3, no_alt, 
	       OBJ(int, 10), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (conf_ip_nacl_ext_tcpsrcp2,
	  conf_ip_nacl_ext_ipaddr3,
	  conf_ip_nacl_ext_srctacacsdsr2, OBJ(int, 10)); 

KEYWORD_ID_MM (conf_ip_nacl_ext_tcpsrctacacsdsr1,
	       conf_ip_nacl_ext_tcpsrcp2, no_alt, 
	       OBJ(int, 9), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (conf_ip_nacl_ext_tcpsrcrange,
	  conf_ip_nacl_ext_tcpsrcp2,
	  conf_ip_nacl_ext_tcpsrctacacsdsr1, 
	  OBJ(int, 9));

/*
 * UDP source port range
 */

UDP_PORT (conf_ip_nacl_ext_udpsrcp2,
	  conf_ip_nacl_ext_ipaddr3,
	  conf_ip_nacl_ext_srctacacsdsr2, OBJ(int, 10)); 

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (conf_ip_nacl_ext_udpsrctacacsdsr1,
	       conf_ip_nacl_ext_udpsrcp2, no_alt, 
	       OBJ(int, 9), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
UDP_PORT (conf_ip_nacl_ext_udpsrcrange,
	  conf_ip_nacl_ext_udpsrcp2,
	  conf_ip_nacl_ext_udpsrctacacsdsr1, 
	  OBJ(int, 9));

TESTVAR	(conf_ip_nacl_ext_srcprange,
	 conf_ip_nacl_ext_tcpsrcrange,
	 conf_ip_nacl_ext_udpsrcrange, 
	 NONE, NONE, NONE, OBJ(int,3), TCP_PROT);

/*
 * { lt | gt | eq | neq | range }
 */

KEYWORD_ID(conf_ip_nacl_ext_srcrange,
	   conf_ip_nacl_ext_srcprange,
	   conf_ip_nacl_ext_ipaddr3, 
	   OBJ(int, 8), OPC_RANGE, OPC_RANGE_STR,
	   "Match only packets in the range of port numbers",
	   PRIV_CONF);
KEYWORD_ID(conf_ip_nacl_ext_srcneq,
	   conf_ip_nacl_ext_srcport,
	   conf_ip_nacl_ext_srcrange, 
	   OBJ(int, 8), OPC_NEQ,
	   OPC_NEQ_STR,
	   "Match only packets not on a given port number",
	   PRIV_CONF);
KEYWORD_ID(conf_ip_nacl_ext_srceq, conf_ip_nacl_ext_srcport,
	   conf_ip_nacl_ext_srcneq, 
	   OBJ(int, 8), OPC_EQ,
	   OPC_EQ_STR,
	   "Match only packets on a given port number",
	   PRIV_CONF);
KEYWORD_ID(conf_ip_nacl_ext_srcgt, conf_ip_nacl_ext_srcport,
	   conf_ip_nacl_ext_srceq, 
	   OBJ(int, 8), OPC_GT,
	   OPC_GT_STR,
	   "Match only packets with a greater port number",
	   PRIV_CONF);
KEYWORD_ID(conf_ip_nacl_ext_srclt, conf_ip_nacl_ext_srcport,
	   conf_ip_nacl_ext_srcgt, 
	   OBJ(int, 8), OPC_LT,
	   OPC_LT_STR,
	   "Match only packets with a lower port number",
	   PRIV_CONF);

SET	(conf_ip_nacl_ext_sopts, conf_ip_nacl_ext_srclt,
	 OBJ(int, 8), OPC_NONE); 

/*
 * Only provide the above options for TCP and UDP.
 */

IFELSE	(conf_ip_nacl_ext_sportopts, conf_ip_nacl_ext_sopts,
	 conf_ip_nacl_ext_ipaddr3, 
	 ((GETOBJ(int,3) == TCP_PROT) ||
	  (GETOBJ(int,3) == UDP_PROT)));

/*
 * Parse a source
 * <srcaddr> <srcmask>
 * "any"
 * "host" <srcaddr>"
 */

IPADDR_NAME (conf_ip_nacl_ext_srchostaddr,
	     conf_ip_nacl_ext_sportopts, no_alt, 
	 OBJ(paddr,1), "Source address");
KEYWORD (conf_ip_nacl_ext_srchost,
	 conf_ip_nacl_ext_srchostaddr, no_alt, 
	 "host", "A single source host", PRIV_CONF);

KEYWORD_ID(conf_ip_nacl_ext_srcany,
	   conf_ip_nacl_ext_sportopts,
	   conf_ip_nacl_ext_srchost, 
	   OBJ(paddr, 2).ip_addr, WILD_ANY,
	   "any", "Any source host", PRIV_CONF);

IPADDR	(conf_ip_nacl_ext_ipaddr2,
	 conf_ip_nacl_ext_sportopts, no_alt, 
	 OBJ(paddr,2), "Source wildcard bits");
IPADDR	(conf_ip_nacl_ext_ipaddr1, conf_ip_nacl_ext_ipaddr2,
	 conf_ip_nacl_ext_srcany, 
	 OBJ(paddr,1), "Source address");

/*
 * If none of the protocol names match, the check for a protocol number
 */
NUMBER (conf_ip_nacl_ext_protonum, conf_ip_nacl_ext_ipaddr1,
	no_alt, OBJ(int,3), 0, 255, "An IP protocol number");

/*
 * { ip | tcp | udp | icmp | igrp | igmp | gre | nos | eigrp | ospf |
 *   ipinip | dvmrp }
 */
KEYWORD_ID_MM(conf_ip_nacl_ext_ipinip,
	      conf_ip_nacl_ext_ipaddr1,
	      conf_ip_nacl_ext_protonum, 
	      OBJ(int,3), IPINIP_PROT,
	      IPINIP_PROT_NAME, "IP in IP tunneling", PRIV_USER, 3);
KEYWORD_ID (conf_ip_nacl_ext_dvmrp,
	    conf_ip_nacl_ext_ipaddr1,
	    conf_ip_nacl_ext_ipinip, 
	    OBJ(int,3), IPINIP_PROT, IGMP_DVMRP_NAME,
	    NULL, PRIV_USER|PRIV_HIDDEN);
KEYWORD_ID (conf_ip_nacl_ext_nos, conf_ip_nacl_ext_ipaddr1,
	    conf_ip_nacl_ext_dvmrp, 
	    OBJ(int,3), NOSIP_PROT, NOSIP_PROT_NAME,
	    "KA9Q NOS compatible IP over IP tunneling", PRIV_USER);
KEYWORD_ID (conf_ip_nacl_ext_ospf, conf_ip_nacl_ext_ipaddr1,
	    conf_ip_nacl_ext_nos, 
	    OBJ(int,3), OSPF_PROT, OSPF_PROT_NAME,
	    "OSPF routing protocol", PRIV_USER);
KEYWORD_ID (conf_ip_nacl_ext_gre, conf_ip_nacl_ext_ipaddr1,
	    conf_ip_nacl_ext_ospf, 
	    OBJ(int,3), GRE_PROT,
	    GRE_PROT_NAME, "Cisco's GRE tunneling", PRIV_USER);
KEYWORD_ID (conf_ip_nacl_ext_igmp, conf_ip_nacl_ext_ipaddr1,
	    conf_ip_nacl_ext_gre, 
	    OBJ(int,3), IGMP_PROT,
	    IGMP_PROT_NAME, "Internet Gateway Message Protocol", PRIV_USER);
KEYWORD_ID (conf_ip_nacl_ext_eigrp,
	    conf_ip_nacl_ext_ipaddr1, conf_ip_nacl_ext_igmp,
	    
	    OBJ(int,3), NEWIGRP_PROT,
	    EIGRP_PROT_NAME, "Cisco's EIGRP routing protocol", PRIV_USER);
KEYWORD_ID (conf_ip_nacl_ext_igrp, conf_ip_nacl_ext_ipaddr1,
	    conf_ip_nacl_ext_eigrp, 
	    OBJ(int,3), IGRP_PROT,
	    IGRP_PROT_NAME, "Cisco's IGRP routing protocol", PRIV_USER);
KEYWORD_ID (conf_ip_nacl_ext_icmp, conf_ip_nacl_ext_ipaddr1,
	    conf_ip_nacl_ext_igrp, 
	    OBJ(int,3), ICMP_PROT,
	    ICMP_PROT_NAME, "Internet Control Message Protocol", PRIV_USER);
KEYWORD_ID (conf_ip_nacl_ext_udp, conf_ip_nacl_ext_ipaddr1,
	    conf_ip_nacl_ext_icmp, 
	    OBJ(int,3), UDP_PROT,
	    UDP_PROT_NAME, "User Datagram Protocol", PRIV_USER);
KEYWORD_ID (conf_ip_nacl_ext_tcp, conf_ip_nacl_ext_ipaddr1,
	    conf_ip_nacl_ext_udp, 
	    OBJ(int,3), TCP_PROT,
	    TCP_PROT_NAME, "Transmission Control Protocol", PRIV_USER);

KEYWORD_ID (conf_ip_nacl_ext_ip, conf_ip_nacl_ext_ipaddr1,
	    conf_ip_nacl_ext_tcp, 
	    OBJ(int,3), IP_PROT,
	    IP_PROT_NAME, "Any Internet Protocol", PRIV_USER);

/*
 * { permit | deny }
 */
PERMIT_DENY(conf_ip_nacl_ext_permit_deny,
	    conf_ip_nacl_ext_ip, conf_ip_nacl_exit, OBJ(int,2), PRIV_CONF); 

NUMBER	(conf_ip_nacl_ext_dyn_timeout,
	 conf_ip_nacl_ext_permit_deny, no_alt, 
	 OBJ(int,17), MINIDLEACC, MAXIDLEACC, "Maximum time to live");

KEYWORD	(conf_ip_nacl_ext_dyn_time,
	 conf_ip_nacl_ext_dyn_timeout,
	 conf_ip_nacl_ext_permit_deny, 
	 "timeout", "Maximum time for dynamic ACL to live", PRIV_CONF);

STRING	(conf_ip_nacl_ext_dyn_name,
	 conf_ip_nacl_ext_dyn_time, no_alt, 
	 OBJ(string, 1), "Name of a Dynamic list");

KEYWORD_ID (conf_ip_nacl_ext_dynamic,
	    conf_ip_nacl_ext_dyn_name,
	    conf_ip_nacl_ext_permit_deny, 
	    OBJ(int,16), TRUE,
	    "dynamic", "Specify a DYNAMIC list of PERMITs or DENYs",
	    PRIV_CONF);

NO_OR_DEFAULT(conf_ip_nacl_ext_no, conf_ip_nacl_ext_dynamic,
	   PRIV_MIN | PRIV_NONVGEN);
HELP	(top_conf_ip_enacl, conf_ip_nacl_ext_no,
	 "Ext Access List configuration commands:\n");

/*****************************************************************
 * IP standard access list
 *
 * { permit | deny }  <addr> [<mask>]
 * no [{ permit | deny }  <addr> <mask>]
 *
 * any == "0.0.0.0 255.255.255.255"
 *
 * OBJ(int,1) = access list number
 * OBJ(int,2) = TRUE for permit; FALSE for deny
 * OBJ(paddr,1)= address
 * OBJ(paddr,2)= mask
 */

EOLNS	(conf_ip_nacl_simple_eol, ip_nacl_simple_command);

IPADDR	(conf_ip_nacl_simple_2, conf_ip_nacl_simple_eol,
	 conf_ip_nacl_simple_eol, 
	 OBJ(paddr,2), "Wildcard bits");
IPADDR_NAME(conf_ip_nacl_simple_1, conf_ip_nacl_simple_2, no_alt,
	 OBJ(paddr,1), "Address to match");

TEST_MULTIPLE_FUNCS (conf_ip_nacl_simple_test, conf_ip_nacl_simple_1,
		     no_alt, NONE); 

IPADDR_NAME (conf_ip_nacl_simple_hostaddr, conf_ip_nacl_simple_eol,
	     no_alt, OBJ(paddr,1), "Host address");
KEYWORD (conf_ip_nacl_simple_host, conf_ip_nacl_simple_hostaddr,
	 conf_ip_nacl_simple_test, 
	 "host", "A single host address", PRIV_CONF);

KEYWORD_ID(conf_ip_nacl_simple_any, conf_ip_nacl_simple_eol,
	   conf_ip_nacl_simple_host, 
	   OBJ(paddr, 2).ip_addr, WILD_ANY,
	   "any", "Any source host", PRIV_CONF);

PERMIT_DENY(conf_ip_nacl_simple_permitdeny, conf_ip_nacl_simple_any,
	    conf_ip_nacl_exit, OBJ(int,2), PRIV_CONF);

NO_OR_DEFAULT(conf_ip_nacl_simple_no, conf_ip_nacl_simple_permitdeny,
	   PRIV_MIN | PRIV_NONVGEN);
HELP	(top_conf_ip_snacl, conf_ip_nacl_simple_no,
	 "Standard Access List configuration commands:\n");

/******************************************************************************
 * [no] ip access-list { standard | extended } <name>
 *
 * OBJ(string,1) -- access-list name
 */

EOLS    (conf_ip_nacl_ext_mode_eol, ip_nacl_command,
	 ACL_IP_NAMED_EXTENDED); 
GENERAL_STRING  (conf_ip_nacl_ext_name, conf_ip_nacl_ext_mode_eol,
		 NONE,  OBJ(string,1), "Access-list name",
		 STRING_HELP_CHECK); 
NVGENS	(conf_ip_nacl_ext_nvgen, conf_ip_nacl_ext_name,
	 ip_nacl_command_nvgen, ACL_IP_NAMED_EXTENDED);
KEYWORD (conf_ip_nacl_ext_mode, conf_ip_nacl_ext_nvgen,
	 NONE, "extended", "Extended Access List",
	 PRIV_CONF); 
EOLS    (conf_ip_nacl_simple_mode_eol, ip_nacl_command,
	 ACL_IP_NAMED_SIMPLE); 
GENERAL_STRING  (conf_ip_nacl_simple_name,
		 conf_ip_nacl_simple_mode_eol, NONE, OBJ(string,1),
		 "Access-list name", STRING_HELP_CHECK); 
NVGENS	(conf_ip_nacl_simple_nvgen, conf_ip_nacl_simple_name,
	 ip_nacl_command_nvgen, ACL_IP_NAMED_SIMPLE);
KEYWORD (conf_ip_nacl_simple_mode, conf_ip_nacl_simple_nvgen,
	 conf_ip_nacl_ext_mode, "standard", "Standard Access List",
	 PRIV_CONF); 
KEYWORD (conf_ip_nacl_acl, conf_ip_nacl_simple_mode, ALTERNATE,
	 "access-list",
	 "Named access-list", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_ip_nacl_acl
