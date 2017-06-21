/* $Id: cfg_acclist_ip.h,v 3.4.46.2 1996/07/20 01:13:08 bew Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_acclist_ip.h,v $
 *------------------------------------------------------------------
 * Simple access list commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_acclist_ip.h,v $
 * Revision 3.4.46.2  1996/07/20  01:13:08  bew
 * CSCdi63411:  ACL encrypted/decrypted flags are not useful
 * Branch: California_branch
 * Remove encrypted/decrypted flags introduced early in 11.2
 * from ACL code.
 *
 * Revision 3.4.46.1  1996/04/19  15:41:01  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.5  1996/04/17  23:01:28  carrel
 * Branch: IosSec_branch
 * Final commit from IP code review comments
 *
 * Revision 3.1.2.4  1996/02/13  19:17:48  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 3.1.2.3  1995/12/20  19:39:13  bew
 * Branch: IosSec_branch
 * Re-enabled the encrypt keyword.
 *
 * Revision 3.1.2.2  1995/12/17  01:39:28  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.4  1995/12/07  17:45:05  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.3  1995/11/27  21:24:41  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  09:32:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:53:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:03:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/16  03:40:44  tli
 * CSCdi38358:  IP: standard access lists should parse host keyword
 *
 * Revision 2.2  1995/07/13  21:38:57  ahh
 * CSCdi36962:  UDP parsing of tacacs-ds port name broken
 * Restore "tacacs-ds" as a keyword, but hide it and map it to TACACS_PORT
 * for backwards compatibility.
 *
 * Revision 2.1  1995/07/01  12:55:32  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:01:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*****************************************************************
 * IP extended access list
 *
 * access-list <100-199> [dynamic <name>] { permit | deny } <protocol>
 *	<address-mask> [<modifier>] <address-mask> [<modifier>] [established]
 *      [<tos>] [<icmp-typecode>] [<igmp-type>]
 *
 * no access-list <100-199> [dynamic <name>] [{ permit | deny } <protocol>
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

EOLS	(alist_eol, ip2access_command, LIST_IP2);

/*
 * End of options.  This allows all options to occur in any order.
 */

pdecl(alist_opts);

/*
 * log-input
 */
KEYWORD_ID_MM (alist_iplogif, alist_opts, alist_eol,
	       OBJ(int,15), IP_ACCESS_LOG_INPUT,
	       "log-input",
	       "Log matches against this entry, including input interface",
	       PRIV_CONF, 4);  
/*
 * log
 */
KEYWORD_ID (alist_iplog, alist_opts, alist_iplogif,
	 OBJ(int,15), IP_ACCESS_LOG_NORMAL,
	 "log", "Log matches against this entry", PRIV_CONF);

/* tos <value>
 *     1000   --   minimize delay
 *     0100   --   maximize throughput
 *     0010   --   maximize reliability
 *     0001   --   minimize monetary cost
 *     0000   --   normal service
 */

KEYWORD_ID (alist_iptosnormal, alist_opts, no_alt,
	    OBJ(int, 14), IP_TOS_NORMAL, TOS_NORMAL_NAME,
	    "Match packets with normal TOS (0)", PRIV_CONF);

KEYWORD_ID (alist_iptoscost, alist_opts, alist_iptosnormal,
	    OBJ(int, 14), IP_TOS_LOW_COST, TOS_COST_NAME,
	    "Match packets with min monetary cost TOS (1)", PRIV_CONF);

KEYWORD_ID (alist_iptosreliab, alist_opts, alist_iptoscost,
	    OBJ(int, 14), IP_TOS_HIGH_RELY, TOS_RELIABLE_NAME,
	    "Match packets with max reliable TOS (2)", PRIV_CONF);

KEYWORD_ID (alist_iptosthrough, alist_opts, alist_iptosreliab,
	    OBJ(int, 14), IP_TOS_HIGH_TPUT, TOS_THROUGH_NAME,
	    "Match packets with max throughput TOS (4)", PRIV_CONF);

KEYWORD_ID (alist_iptosdelay, alist_opts, alist_iptosthrough,
	    OBJ(int, 14), IP_TOS_LOW_DELAY, TOS_DELAY_NAME,
	    "Match packets with min delay TOS (8)", PRIV_CONF);

NUMBER	(alist_tos, alist_opts, alist_iptosdelay, OBJ(int, 14), 0, 15,
	 "Type of service value");

KEYWORD_ID (alist_iptos, alist_tos, alist_iplog,
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

KEYWORD_ID (alist_ippredroutine, alist_opts, no_alt, OBJ(int, 12),
	    PRECEDENCE_ROUTINE, PRECEDENCE_ROUTINE_STR,
	    "Match packets with routine precedence (0)", PRIV_CONF);

KEYWORD_ID (alist_ippredpriority, alist_opts, alist_ippredroutine,
	    OBJ(int, 12), PRECEDENCE_PRIORITY, PRECEDENCE_PRIORITY_STR,
	    "Match packets with priority precedence (1)", PRIV_CONF);

KEYWORD_ID (alist_ippredimmediate, alist_opts, alist_ippredpriority,
	    OBJ(int, 12),
	    PRECEDENCE_IMMEDIATE, PRECEDENCE_IMMEDIATE_STR,
	    "Match packets with immediate precedence (2)", PRIV_CONF);

KEYWORD_ID (alist_ippredflash, alist_opts, alist_ippredimmediate,
	    OBJ(int, 12), PRECEDENCE_FLASH, PRECEDENCE_FLASH_STR,
	    "Match packets with flash precedence (3)", PRIV_CONF);

KEYWORD_ID_MM(alist_ippredflashover, alist_opts, alist_ippredflash,
	      OBJ(int, 12), PRECEDENCE_FLASHOVER,
	      PRECEDENCE_FLASHOVER_STR,
	      "Match packets with flash override precedence (4)", PRIV_CONF, 6);

KEYWORD_ID (alist_ippredcrit, alist_opts, alist_ippredflashover, OBJ(int, 12),
	    PRECEDENCE_CRITICAL, PRECEDENCE_CRITICAL_STR,
	    "Match packets with critical precedence (5)", PRIV_CONF);

KEYWORD_ID (alist_ippredinternet, alist_opts, alist_ippredcrit, OBJ(int, 12),
	    PRECEDENCE_INTERNET, PRECEDENCE_INTERNET_STR,
	    "Match packets with internetwork control precedence (6)",
	    PRIV_CONF);

KEYWORD_ID (alist_ipprednet, alist_opts, alist_ippredinternet, OBJ(int, 12),
	    PRECEDENCE_NETWORK, PRECEDENCE_NETWORK_STR,
	    "Match packets with network control precedence (7)", PRIV_CONF);

NUMBER	(alist_precedence, alist_opts, alist_ipprednet, OBJ(int, 12), 0, 7,
	 "Precedence value");

KEYWORD_ID(alist_ipprecedence, alist_precedence, alist_iptos,
	   OBJ(int,11), TRUE, "precedence",
	    "Match packets with given precedence value", PRIV_CONF);

/*
 * Parse IGMP message types.
 */

SET	(alist_igmptypepresent, alist_opts, OBJ(int, 4), OPC_ONEBYTE);

KEYWORD_ID (alist_igmptrace, alist_igmptypepresent, alist_ipprecedence,
	    OBJ(int, 5), IGMP_TRACE_TYPE, IGMP_TRACE_NAME,
	    "Multicast trace", PRIV_CONF);

KEYWORD_ID (alist_igmppim, alist_igmptypepresent, alist_igmptrace,
	    OBJ(int, 5), IGMP_PIM_TYPE, IGMP_PIM_NAME,
	    "Protocol Independent Multicast", PRIV_CONF);

KEYWORD_ID (alist_igmpdvmrp, alist_igmptypepresent, alist_igmppim,
	    OBJ(int, 5), IGMP_DVMRP_TYPE, IGMP_DVMRP_NAME,
	    "Distance Vector Multicast Routing Protocol", PRIV_CONF);

KEYWORD_ID (alist_igmphostreport, alist_igmptypepresent, alist_igmpdvmrp,
	    OBJ(int, 5), IGMP_REPORT_TYPE, IGMP_REPORT_NAME,
	    "Host report", PRIV_CONF);

KEYWORD_ID (alist_igmphostquery, alist_igmptypepresent, alist_igmphostreport,
	    OBJ(int, 5), IGMP_QUERY_TYPE, IGMP_QUERY_NAME,
	    "Host query", PRIV_CONF);

NUMBER	(alist_igmptype, alist_igmptypepresent, alist_igmphostquery,
	 OBJ(int, 5), 0, 15, "IGMP message type");

/*
 * Parse ICMP typecodes.  Only applies to ICMP.
 * Deal with the case of filtering on specific type and code.
 */

SET	(alist_icmpcodepresent, alist_opts, OBJ(int, 4), OPC_TWOBYTES);

NUMBER (alist_icmpcode, alist_icmpcodepresent, alist_eol,
	OBJ(int, 6), 0, 255, "ICMP message code");

SET	(alist_icmpparam, alist_icmpcodepresent, OBJ(int, 5), ICMP_PARAMETER);

KEYWORD_ID (alist_icmpparamreqnoroom, alist_icmpparam,
	    alist_ipprecedence,
	    OBJ(int, 6), ICMP_PARAMREQNOROOM, ICMP_PARAMREQNOROOM_NAME,
	    "Parameter required but no room", PRIV_CONF);

KEYWORD_ID (alist_icmpparamrequired, alist_icmpparam,
	    alist_icmpparamreqnoroom,
	    OBJ(int, 6), ICMP_PARAMREQUIRED, ICMP_PARAMREQUIRED_NAME,
	    "Parameter required but not present", PRIV_CONF);

KEYWORD_ID (alist_icmpparamprob, alist_icmpparam,
	    alist_icmpparamrequired,
	    OBJ(int, 6), ICMP_PARAMPROB, ICMP_PARAMPROB_NAME,
	    "Parameter problem", PRIV_CONF);

SET	(alist_icmptim, alist_icmpcodepresent, OBJ(int, 5),
	 ICMP_TIMEEXCEEDED);

KEYWORD_ID (alist_icmptimfrag, alist_icmptim,
	    alist_icmpparamprob,
	    OBJ(int, 6), ICMP_TIMFRAG, ICMP_TIMFRAG_NAME,
	    "Reassembly timeout", PRIV_CONF);

KEYWORD_ID (alist_icmptimttl, alist_icmptim,
	    alist_icmptimfrag,
	    OBJ(int, 6), ICMP_TIMTTL, ICMP_TIMTTL_NAME,
	    "TTL exceeded", PRIV_CONF);

SET	(alist_icmpred, alist_icmpcodepresent, OBJ(int, 5), ICMP_REDIRECT);

KEYWORD_ID (alist_icmpredservhost, alist_icmpred,
	    alist_icmptimttl,
	    OBJ(int, 6), ICMP_REDSERVHOST, ICMP_REDSERVHOST_NAME,
	    "Host redirect for TOS", PRIV_CONF);

KEYWORD_ID (alist_icmpredservnet, alist_icmpred,
	    alist_icmpredservhost,
	    OBJ(int, 6), ICMP_REDSERVNET, ICMP_REDSERVNET_NAME,
	    "Net redirect for TOS", PRIV_CONF);

KEYWORD_ID (alist_icmpredhost, alist_icmpred,
	    alist_icmpredservnet,
	    OBJ(int, 6), ICMP_REDHOST, ICMP_REDHOST_NAME,
	    "Host redirect", PRIV_CONF);

KEYWORD_ID (alist_icmprednet, alist_icmpred,
	    alist_icmpredhost,
	    OBJ(int, 6), ICMP_REDNET, ICMP_REDNET_NAME,
	    "Network redirect", PRIV_CONF);

SET	(alist_icmpunr, alist_icmpcodepresent, OBJ(int, 5),
	 ICMP_UNREACHABLE);

KEYWORD_ID_MM(alist_icmpunrprecedence, alist_icmpunr, alist_icmprednet,
	      OBJ(int, 6), ICMP_UNRPRECEDENCE,
	      ICMP_UNRPRECEDENCE_NAME, "Precedence cutoff", PRIV_CONF, 11);

KEYWORD_ID (alist_icmpunrhostprec, alist_icmpunr,
	    alist_icmpunrprecedence,
	    OBJ(int, 6), ICMP_UNRHOSTPREC, ICMP_UNRHOSTPREC_NAME,
	    "Host unreachable for precedence", PRIV_CONF);

KEYWORD_ID (alist_icmpunradmin, alist_icmpunr,
	    alist_icmpunrhostprec,
	    OBJ(int, 6), ICMP_UNRADMIN, ICMP_UNRADMIN_NAME,
	    "Administratively prohibited", PRIV_CONF);

KEYWORD_ID (alist_icmpunrhosttos, alist_icmpunr,
	    alist_icmpunradmin,
	    OBJ(int, 6), ICMP_UNRHOSTTOS, ICMP_UNRHOSTTOS_NAME,
	    "Host unreachable for TOS", PRIV_CONF);

KEYWORD_ID (alist_icmpunrnettos, alist_icmpunr,
	    alist_icmpunrhosttos,
	    OBJ(int, 6), ICMP_UNRNETTOS, ICMP_UNRNETTOS_NAME,
	    "Network unreachable for TOS", PRIV_CONF);

KEYWORD_ID (alist_icmpunrprohib, alist_icmpunr,
	    alist_icmpunrnettos,
	    OBJ(int, 6), ICMP_UNRPROHIB, ICMP_UNRPROHIB_NAME,
	    "Host prohibited", PRIV_CONF);

KEYWORD_ID (alist_icmpunrnetprohib, alist_icmpunr,
	    alist_icmpunrprohib,
	    OBJ(int, 6), ICMP_UNRNETPROHIB, ICMP_UNRNETPROHIB_NAME,
	    "Net prohibited", PRIV_CONF);

KEYWORD_ID (alist_icmpunrhostisolated, alist_icmpunr,
	    alist_icmpunrnetprohib,
	    OBJ(int, 6), ICMP_UNRHOSTISOLATED, ICMP_UNRHOSTISOLATED_NAME,
	    "Host isolated", PRIV_CONF);

KEYWORD_ID (alist_icmpunrnohost, alist_icmpunr,
	    alist_icmpunrhostisolated,
	    OBJ(int, 6), ICMP_UNRNOHOST, ICMP_UNRNOHOST_NAME,
	    "Host unknown", PRIV_CONF);

KEYWORD_ID (alist_icmpunrnonet, alist_icmpunr,
	    alist_icmpunrnohost,
	    OBJ(int, 6), ICMP_UNRNONET, ICMP_UNRNONET_NAME,
	    "Network unknown", PRIV_CONF);

KEYWORD_ID (alist_icmpunrsrfail, alist_icmpunr,
	    alist_icmpunrnonet,
	    OBJ(int, 6), ICMP_UNRSRFAIL, ICMP_UNRSRFAIL_NAME,
	    "Source route failed", PRIV_CONF);

KEYWORD_ID (alist_icmpunrfrag, alist_icmpunr,
	    alist_icmpunrsrfail,
	    OBJ(int, 6), ICMP_UNRFRAG, ICMP_UNRFRAG_NAME,
	    "Fragmentation needed and DF set", PRIV_CONF);

KEYWORD_ID (alist_icmpunrport, alist_icmpunr,
	    alist_icmpunrfrag,
	    OBJ(int, 6), ICMP_UNRPORT, ICMP_UNRPORT_NAME,
	    "Port unreachable", PRIV_CONF);

KEYWORD_ID (alist_icmpunrprotocol, alist_icmpunr,
	    alist_icmpunrport,
	    OBJ(int, 6), ICMP_UNRPORT, ICMP_UNRPROTOCOL_NAME,
	    "Protocol unreachable", PRIV_CONF);

KEYWORD_ID (alist_icmpunrhost, alist_icmpunr,
	    alist_icmpunrprotocol,
	    OBJ(int, 6), ICMP_UNRHOST, ICMP_UNRHOST_NAME,
	    "Host unreachable", PRIV_CONF);

KEYWORD_ID (alist_icmpunrnet, alist_icmpunr,
	    alist_icmpunrhost,
	    OBJ(int, 6), ICMP_UNRNET, ICMP_UNRNET_NAME,
	    "Net unreachable", PRIV_CONF);

/*
 * ICMP types only
 */

SET	(alist_icmptypepresent, alist_opts, OBJ(int, 4), OPC_ONEBYTE);

KEYWORD_ID (alist_icmpmobileredirect, alist_icmptypepresent,
	    alist_icmpunrnet,
	    OBJ(int, 5), ICMP_MOBILEREDIRECT, ICMP_MOBILEREDIRECT_NAME,
	    "Mobile host redirect", PRIV_CONF);

KEYWORD_ID (alist_icmpconversion, alist_icmptypepresent,
	    alist_icmpmobileredirect,
	    OBJ(int, 5), ICMP_CONVERSION, ICMP_CONVERSION_NAME,
	    "Datagram conversion", PRIV_CONF);

KEYWORD_ID (alist_icmptraceroute, alist_icmptypepresent,
	    alist_icmpconversion,
	    OBJ(int, 5), ICMP_TRACEROUTE, ICMP_TRACEROUTE_NAME,
	    "Traceroute", PRIV_CONF);

KEYWORD_ID (alist_icmpmaskreply, alist_icmptypepresent,
	    alist_icmptraceroute,
	    OBJ(int, 5), ICMP_MASKREPLY, ICMP_MASKREPLY_NAME,
	    "Mask replies", PRIV_CONF);

KEYWORD_ID (alist_icmpmaskrequest, alist_icmptypepresent,
	    alist_icmpmaskreply,
	    OBJ(int, 5), ICMP_MASKREQUEST, ICMP_MASKREQUEST_NAME,
	    "Mask requests", PRIV_CONF);

KEYWORD_ID (alist_icmpinforeply, alist_icmptypepresent,
	    alist_icmpmaskrequest,
	    OBJ(int, 5), ICMP_INFOREPLY, ICMP_INFOREPLY_NAME,
	    "Information replies", PRIV_CONF);

KEYWORD_ID (alist_icmpinforequest, alist_icmptypepresent,
	    alist_icmpinforeply,
	    OBJ(int, 5), ICMP_INFOREQUEST, ICMP_INFOREQUEST_NAME,
	    "Information requests", PRIV_CONF);

KEYWORD_ID (alist_icmptimestampreply, alist_icmptypepresent,
	    alist_icmpinforequest,
	    OBJ(int, 5), ICMP_TIMESTAMPREPLY, ICMP_TIMESTAMPREPLY_NAME,
	    "Timestamp replies", PRIV_CONF);

KEYWORD_ID (alist_icmptimestamp, alist_icmptypepresent,
	    alist_icmptimestampreply,
	    OBJ(int, 5), ICMP_TIMESTAMP, ICMP_TIMESTAMP_NAME,
	    "Timestamp requests", PRIV_CONF);

KEYWORD_ID (alist_icmpparameter, alist_icmptypepresent,
	    alist_icmptimestamp,
	    OBJ(int, 5), ICMP_PARAMETER, ICMP_PARAMETER_NAME,
	    "All parameter problems", PRIV_CONF);

KEYWORD_ID (alist_icmptimeexceeded, alist_icmptypepresent,
	    alist_icmpparameter,
	    OBJ(int, 5), ICMP_TIMEEXCEEDED, ICMP_TIMEEXCEEDED_NAME,
	    "All time exceededs", PRIV_CONF);

KEYWORD_ID (alist_icmprdpsolicit, alist_icmptypepresent,
	    alist_icmptimeexceeded,
	    OBJ(int, 5), ICMP_RDPSOLICIT, ICMP_RDPSOLICIT_NAME,
	    "Router discovery solicitations", PRIV_CONF);

KEYWORD_ID (alist_icmprdpadvert, alist_icmptypepresent,
	    alist_icmprdpsolicit,
	    OBJ(int, 5), ICMP_RDPADVERT, ICMP_RDPADVERT_NAME,
	    "Router discovery advertisements", PRIV_CONF);

KEYWORD_ID (alist_icmpaltaddress, alist_icmptypepresent, alist_icmprdpadvert,
	    OBJ(int, 5), ICMP_ALTADDRESS, ICMP_ALTADDRESS_NAME,
	    "Alternate address", PRIV_CONF);

KEYWORD_ID (alist_icmpecho, alist_icmptypepresent, alist_icmpaltaddress,
	    OBJ(int, 5), ICMP_ECHO, ICMP_ECHO_NAME,
	    "Echo (ping)", PRIV_CONF);

KEYWORD_ID (alist_icmpredirect, alist_icmptypepresent, alist_icmpecho,
	    OBJ(int, 5), ICMP_REDIRECT, ICMP_REDIRECT_NAME,
	    "All redirects", PRIV_CONF);

KEYWORD_ID (alist_icmpquench, alist_icmptypepresent, alist_icmpredirect,
	    OBJ(int, 5), ICMP_QUENCH, ICMP_QUENCH_NAME,
	    "Source quenches", PRIV_CONF);

KEYWORD_ID (alist_icmpunreach, alist_icmptypepresent, alist_icmpquench,
	    OBJ(int, 5), ICMP_UNREACHABLE, ICMP_UNREACHABLE_NAME,
	    "All unreachables", PRIV_CONF);

KEYWORD_ID_MM(alist_icmpechoreply, alist_icmptypepresent, alist_icmpunreach,
	      OBJ(int, 5), ICMP_ECHOREPLY,
	      ICMP_ECHOREPLY_NAME, "Echo reply", PRIV_CONF, 5);

SET	(alist_icmptypeset, alist_icmpcode, OBJ(int, 4), OPC_ONEBYTE);

NUMBER (alist_icmptype, alist_icmptypeset, alist_icmpechoreply,
	OBJ(int, 5), 0, 255, "ICMP message type");

/*
 * Parse [established]
 * This only applies for TCP
 */
KEYWORD_ID (alist_tcpest, alist_opts, NONE,
	    OBJ(int,7), TRUE,
	    "established", "Match established connections", PRIV_CONF);

TESTVAR	(alist_tcpopts, alist_tcpest, NONE, NONE, NONE, alist_ipprecedence,
	 OBJ(int,3), TCP_PROT);

/*
 * TCP destination ports
 */

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM(alist_dsttacacsds, alist_opts, no_alt,
	      OBJ(int, 5), TACACS_PORT,
	      "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (alist_tcpdstport, alist_opts, alist_dsttacacsds, OBJ(int, 5));

/*
 * UDP destination ports
 */

UDP_PORT (alist_udpdstport, alist_opts, alist_dsttacacsds, OBJ(int, 5));

TESTVAR	(alist_dstport, alist_tcpdstport, alist_udpdstport,
	 NONE, NONE, NONE, OBJ(int,3), TCP_PROT);

/*
 * TCP destination port range
 */

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (alist_tacacsdsr2, alist_opts, no_alt,
	       OBJ(int, 6), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (alist_tcpdstp2, alist_opts, alist_tacacsdsr2, OBJ(int,6));

KEYWORD_ID_MM (alist_tacacsdsr1, alist_tcpdstp2, no_alt,
	       OBJ(int, 5), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (alist_tcpdstrange, alist_tcpdstp2, alist_tacacsdsr1, OBJ(int,5));

/*
 * UDP destination port range
 */

UDP_PORT (alist_udpdstp2, alist_opts, alist_tacacsdsr2, OBJ(int, 6));

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (alist_udptacacsdsr1, alist_udpdstp2, no_alt,
	       OBJ(int, 5), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
UDP_PORT (alist_udpdstrange, alist_udpdstp2, alist_udptacacsdsr1,
	  OBJ(int, 5));

TESTVAR	(alist_dstrange, alist_tcpdstrange, alist_udpdstrange,
	 NONE, NONE, NONE, OBJ(int,3), TCP_PROT);

/*
 * { lt | gt | eq | neq | range }
 */

KEYWORD_ID(alist_iprange, alist_dstrange, alist_tcpopts,
	   OBJ(int, 4), OPC_RANGE, OPC_RANGE_STR,
	   "Match only packets in the range of port numbers",
	   PRIV_CONF);
KEYWORD_ID(alist_ipneq, alist_dstport, alist_iprange,
	   OBJ(int,4), OPC_NEQ,
	   OPC_NEQ_STR,
	   "Match only packets not on a given port number",
	   PRIV_CONF);
KEYWORD_ID(alist_ipeq, alist_dstport, alist_ipneq,
	   OBJ(int,4), OPC_EQ,
	   OPC_EQ_STR,
	   "Match only packets on a given port number",
	   PRIV_CONF);
KEYWORD_ID(alist_ipgt, alist_dstport, alist_ipeq,
	   OBJ(int,4), OPC_GT,
	   OPC_GT_STR,
	   "Match only packets with a greater port number",
	   PRIV_CONF);
KEYWORD_ID(alist_iplt, alist_dstport, alist_ipgt,
	   OBJ(int,4), OPC_LT,
	   OPC_LT_STR,
	   "Match only packets with a lower port number",
	   PRIV_CONF);

TESTVAR	(alist_transport2, alist_icmptype, alist_iplt,
	 NONE, NONE, NONE, OBJ(int,3), ICMP_PROT);
TESTVAR	(alist_transport, alist_igmptype, alist_transport2,
	 NONE, NONE, NONE, OBJ(int,3), IGMP_PROT);

/*
 * Only provide the above options for TCP, UDP and ICMP.
 */

IFELSE	(alist_opts, alist_transport, alist_ipprecedence,
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

IPADDR_NAME(alist_dsthostaddr, alist_opts, no_alt,
	 OBJ(paddr, 3), "Destination address");
KEYWORD (alist_dsthost, alist_dsthostaddr, no_alt,
	 "host", "A single destination host", PRIV_CONF);

KEYWORD_ID(alist_dstany, alist_opts, alist_dsthost,
	   OBJ(paddr, 4).ip_addr, WILD_ANY,
	   "any", "Any destination host", PRIV_CONF);

IPADDR	(alist_ipaddr4, alist_opts, no_alt,
	 OBJ(paddr,4), "Destination wildcard bits");
IPADDR	(alist_ipaddr3, alist_ipaddr4, alist_dstany,
	 OBJ(paddr,3),"Destination address");

/*
 * TCP source ports
 */

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (alist_srctacacsds, alist_ipaddr3, no_alt,
	       OBJ(int, 9), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (alist_tcpsrcport, alist_ipaddr3, alist_srctacacsds, OBJ(int, 9));

/*
 * UDP source ports
 */

UDP_PORT (alist_udpsrcport, alist_ipaddr3, alist_srctacacsds, OBJ(int, 9));

TESTVAR	(alist_srcport, alist_tcpsrcport, alist_udpsrcport,
	 NONE, NONE, NONE, OBJ(int,3), TCP_PROT);

/*
 * TCP source port range
 */

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (alist_srctacacsdsr2, alist_ipaddr3, no_alt,
	       OBJ(int, 10), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (alist_tcpsrcp2, alist_ipaddr3, alist_srctacacsdsr2, OBJ(int, 10));

KEYWORD_ID_MM (alist_tcpsrctacacsdsr1, alist_tcpsrcp2, no_alt,
	       OBJ(int, 9), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
TCP_PORT (alist_tcpsrcrange, alist_tcpsrcp2, alist_tcpsrctacacsdsr1,
	  OBJ(int, 9));

/*
 * UDP source port range
 */

UDP_PORT (alist_udpsrcp2, alist_ipaddr3, alist_srctacacsdsr2, OBJ(int, 10));

/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (alist_udpsrctacacsdsr1, alist_udpsrcp2, no_alt,
	       OBJ(int, 9), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
UDP_PORT (alist_udpsrcrange, alist_udpsrcp2, alist_udpsrctacacsdsr1,
	  OBJ(int, 9));

TESTVAR	(alist_srcprange, alist_tcpsrcrange, alist_udpsrcrange,
	 NONE, NONE, NONE, OBJ(int,3), TCP_PROT);

/*
 * { lt | gt | eq | neq | range }
 */

KEYWORD_ID(alist_srcrange, alist_srcprange, alist_ipaddr3,
	   OBJ(int, 8), OPC_RANGE, OPC_RANGE_STR,
	   "Match only packets in the range of port numbers",
	   PRIV_CONF);
KEYWORD_ID(alist_srcneq, alist_srcport, alist_srcrange,
	   OBJ(int, 8), OPC_NEQ,
	   OPC_NEQ_STR,
	   "Match only packets not on a given port number",
	   PRIV_CONF);
KEYWORD_ID(alist_srceq, alist_srcport, alist_srcneq,
	   OBJ(int, 8), OPC_EQ,
	   OPC_EQ_STR,
	   "Match only packets on a given port number",
	   PRIV_CONF);
KEYWORD_ID(alist_srcgt, alist_srcport, alist_srceq,
	   OBJ(int, 8), OPC_GT,
	   OPC_GT_STR,
	   "Match only packets with a greater port number",
	   PRIV_CONF);
KEYWORD_ID(alist_srclt, alist_srcport, alist_srcgt,
	   OBJ(int, 8), OPC_LT,
	   OPC_LT_STR,
	   "Match only packets with a lower port number",
	   PRIV_CONF);

SET	(alist_sopts, alist_srclt, OBJ(int, 8), OPC_NONE);

/*
 * Only provide the above options for TCP and UDP.
 */

IFELSE	(alist_sportopts, alist_sopts, alist_ipaddr3,
	 ((GETOBJ(int,3) == TCP_PROT) ||
	  (GETOBJ(int,3) == UDP_PROT)));

/*
 * Parse a source
 * <srcaddr> <srcmask>
 * "any"
 * "host" <srcaddr>"
 */

IPADDR_NAME (alist_srchostaddr, alist_sportopts, no_alt,
	 OBJ(paddr,1), "Source address");
KEYWORD (alist_srchost, alist_srchostaddr, no_alt,
	 "host", "A single source host", PRIV_CONF);

KEYWORD_ID(alist_srcany, alist_sportopts, alist_srchost,
	   OBJ(paddr, 2).ip_addr, WILD_ANY,
	   "any", "Any source host", PRIV_CONF);

IPADDR	(alist_ipaddr2, alist_sportopts, no_alt,
	 OBJ(paddr,2), "Source wildcard bits");
IPADDR	(alist_ipaddr1, alist_ipaddr2, alist_srcany,
	 OBJ(paddr,1), "Source address");

/*
 * If none of the protocol names match, the check for a protocol number
 */
NUMBER (alist_protonum, alist_ipaddr1, no_alt,
	OBJ(int,3), 0, 255, "An IP protocol number");

/*
 * { ip | tcp | udp | icmp | igrp | igmp | gre | nos | eigrp | ospf |
 *   ipinip | dvmrp }
 */
KEYWORD_ID_MM(alist_ipinip, alist_ipaddr1, alist_protonum,
	      OBJ(int,3), IPINIP_PROT,
	      IPINIP_PROT_NAME, "IP in IP tunneling", PRIV_USER, 3);
KEYWORD_ID (alist_dvmrp, alist_ipaddr1, alist_ipinip,
	    OBJ(int,3), IPINIP_PROT, IGMP_DVMRP_NAME,
	    NULL, PRIV_USER|PRIV_HIDDEN);
KEYWORD_ID (alist_nos, alist_ipaddr1, alist_dvmrp,
	    OBJ(int,3), NOSIP_PROT, NOSIP_PROT_NAME,
	    "KA9Q NOS compatible IP over IP tunneling", PRIV_USER);
KEYWORD_ID (alist_ospf, alist_ipaddr1, alist_nos,
	    OBJ(int,3), OSPF_PROT, OSPF_PROT_NAME,
	    "OSPF routing protocol", PRIV_USER);
KEYWORD_ID (alist_gre, alist_ipaddr1, alist_ospf,
	    OBJ(int,3), GRE_PROT,
	    GRE_PROT_NAME, "Cisco's GRE tunneling", PRIV_USER);
KEYWORD_ID (alist_igmp, alist_ipaddr1, alist_gre,
	    OBJ(int,3), IGMP_PROT,
	    IGMP_PROT_NAME, "Internet Gateway Message Protocol", PRIV_USER);
KEYWORD_ID (alist_eigrp, alist_ipaddr1, alist_igmp,
	    OBJ(int,3), NEWIGRP_PROT,
	    EIGRP_PROT_NAME, "Cisco's EIGRP routing protocol", PRIV_USER);
KEYWORD_ID (alist_igrp, alist_ipaddr1, alist_eigrp,
	    OBJ(int,3), IGRP_PROT,
	    IGRP_PROT_NAME, "Cisco's IGRP routing protocol", PRIV_USER);
KEYWORD_ID (alist_icmp, alist_ipaddr1, alist_igrp,
	    OBJ(int,3), ICMP_PROT,
	    ICMP_PROT_NAME, "Internet Control Message Protocol", PRIV_USER);
KEYWORD_ID (alist_udp, alist_ipaddr1, alist_icmp,
	    OBJ(int,3), UDP_PROT,
	    UDP_PROT_NAME, "User Datagram Protocol", PRIV_USER);
KEYWORD_ID (alist_tcp, alist_ipaddr1, alist_udp,
	    OBJ(int,3), TCP_PROT,
	    TCP_PROT_NAME, "Transmission Control Protocol", PRIV_USER);

KEYWORD_ID (alist_ip, alist_ipaddr1, alist_tcp,
	    OBJ(int,3), IP_PROT,
	    IP_PROT_NAME, "Any Internet Protocol", PRIV_USER);

/*
 * { permit | deny }
 */
PERMIT_DENY(alist_permit_deny, alist_ip, no_alt, OBJ(int,2), PRIV_CONF);

NUMBER	(alist_dyn_timeout, alist_permit_deny, no_alt,
	 OBJ(int,17), MINIDLEACC, MAXIDLEACC, "Maximum time to live");

KEYWORD	(alist_dyn_time, alist_dyn_timeout, alist_permit_deny,
	 "timeout", "Maximum time for dynamic ACL to live", PRIV_CONF);

STRING	(alist_dyn_name, alist_dyn_time, no_alt,
	 OBJ(string, 1), "Name of a Dynamic list");

KEYWORD_ID (alist_dynamic, alist_dyn_name, alist_permit_deny,
	    OBJ(int,16), TRUE,
	    "dynamic", "Specify a DYNAMIC list of PERMITs or DENYs",
	    PRIV_CONF);

NOPREFIX(alist_noprefix, alist_dynamic, alist_eol);
NUMBER	(access_list_ipxtnd1, alist_noprefix, NONE,
	 OBJ(int,1), MINSLOWACC, MAXSLOWACC, "IP extended access list");
NVGENS	(alist_ipxtnd_nvgen, access_list_ipxtnd1, ip2access_command, LIST_IP2);
NOP	(access_list_ipxtnd, alist_ipxtnd_nvgen, ALTERNATE);

/*****************************************************************
 * IP standard access list
 *
 * access-list <1-99> { permit | deny }  <addr> [<mask>]
 * no access-list <1-99> [{ permit | deny }  <addr> <mask>]
 *
 * any == "0.0.0.0 255.255.255.255"
 *
 * OBJ(int,1) = access list number
 * OBJ(int,2) = TRUE for permit; FALSE for deny
 * OBJ(paddr,1)= address
 * OBJ(paddr,2)= mask
 */

EOLS	(alist_simple_eol, ip1access_command, LIST_IP1);

IPADDR	(alist_simple_2, alist_simple_eol, alist_simple_eol,
	 OBJ(paddr,2), "Wildcard bits");
IPADDR_NAME(alist_simple_1, alist_simple_2, no_alt,
	 OBJ(paddr,1), "Address to match");

TEST_MULTIPLE_FUNCS (alist_simple_test, alist_simple_1, no_alt, NONE);

IPADDR_NAME (alist_simple_hostaddr, alist_simple_eol, no_alt,
	 OBJ(paddr,1), "Host address");
KEYWORD (alist_simple_host, alist_simple_hostaddr, alist_simple_test,
	 "host", "A single host address", PRIV_CONF);

KEYWORD_ID(alist_simple_any, alist_simple_eol, alist_simple_host,
	   OBJ(paddr, 2).ip_addr, WILD_ANY,
	   "any", "Any source host", PRIV_CONF);

PERMIT_DENY(alist_simple_permitdeny, alist_simple_any, no_alt,
	    OBJ(int,2), PRIV_CONF);

NOPREFIX(alist_simple_noprefix, alist_simple_permitdeny, alist_simple_eol);
NUMBER	(access_list_ipsimple1, alist_simple_noprefix, NONE,
	 OBJ(int,1), MINFASTACC, MAXFASTACC, "IP standard access list");
NVGENS	(alist_simple_nvgen, access_list_ipsimple1,
	 ip1access_command, LIST_IP1);
NOP	(access_list_ipsimple, alist_simple_nvgen, access_list_ipxtnd);

#undef	ALTERNATE
#define	ALTERNATE	access_list_ipsimple
