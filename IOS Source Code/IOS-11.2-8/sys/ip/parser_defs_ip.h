/* $Id: parser_defs_ip.h,v 3.3 1996/02/13 08:13:40 dstine Exp $
 * $Source: /release/111/cvs/Xsys/ip/parser_defs_ip.h,v $
 *------------------------------------------------------------------
 * parser_defs_ip.h -- Parser definitions for IP
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-96 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_ip.h,v $
 * Revision 3.3  1996/02/13  08:13:40  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.2  1995/11/17  09:36:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:58:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:00:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__PARSER_DEFS_IP__
#define	__PARSER_DEFS_IP__

/*
 * TCP/UDP port names
 */

#define BGP_PORT_NAME		"bgp"
#define BIFF_PORT_NAME		"biff"
#define BOOTPC_PORT_NAME	"bootpc"
#define BOOTPS_PORT_NAME	"bootps"
#define CHARGEN_PORT_NAME	"chargen"
#define CMD_PORT_NAME		"cmd"
#define DAYTIME_PORT_NAME	"daytime"
#define DISCARD_PORT_NAME	"discard"
#define DNS_PORT_NAME		"domain"
#define DNSIX_PORT_NAME		"dnsix"
#define ECHO_PORT_NAME		"echo"
#define EXEC_PORT_NAME		"exec"
#define FINGER_PORT_NAME	"finger"
#define FTP_PORT_NAME		"ftp"
#define FTPD_PORT_NAME		"ftp-data"
#define GOPHER_PORT_NAME	"gopher"
#define HOSTNAME_PORT_NAME	"hostname"
#define IEN116_PORT_NAME	"nameserver"
#define IDENT_PORT_NAME	        "ident"
#define IRC_PORT_NAME		"irc"
#define KLOGIN_PORT_NAME	"klogin"
#define KSHELL_PORT_NAME	"kshell"
#define LOGIN_PORT_NAME		"login"
#define MOBILEIP_PORT_NAME	"mobile-ip"
#define NBNS_PORT_NAME		"netbios-ns"
#define NBDS_PORT_NAME		"netbios-dgm"
#define NICNAME_PORT_NAME	"whois"
#define NNTP_PORT_NAME		"nntp"
#define NTP_PORT_NAME		"ntp"
#define POP2_PORT_NAME		"pop2"
#define POP3_PORT_NAME		"pop3"
#define PRINTER_PORT_NAME	"lpd"
#define RIP_PORT_NAME		"rip"
#define SMTP_PORT_NAME		"smtp"
#define SNMP_PORT_NAME		"snmp"
#define SNMP_TRAP_PORT_NAME	"snmptrap"
#define SUNRPC_PORT_NAME	"sunrpc"
#define SYSLOG_PORT_NAME	"syslog"
#define TACACS_PORT_NAME	"tacacs"
#define TALK_PORT_NAME		"talk"
#define TELNET_PORT_NAME	"telnet"
#define TIME_PORT_NAME		"time"
#define TFTP_PORT_NAME		"tftp"
#define UUCP_PORT_NAME		"uucp"
#define WHO_PORT_NAME		"who"
#define WWW_PORT_NAME		"www"
#define XDMCP_PORT_NAME		"xdmcp"

#define ICMP_ECHOREPLY_NAME		"echo-reply"
#define ICMP_UNREACHABLE_NAME		"unreachable"
#define ICMP_UNRNET_NAME		"net-unreachable"
#define ICMP_UNRHOST_NAME		"host-unreachable"
#define ICMP_UNRPROTOCOL_NAME		"protocol-unreachable"
#define ICMP_UNRPORT_NAME		"port-unreachable"
#define ICMP_UNRFRAG_NAME		"packet-too-big"
#define	ICMP_UNRSRFAIL_NAME		"source-route-failed"
#define ICMP_UNRNONET_NAME		"network-unknown"
#define ICMP_UNRNOHOST_NAME		"host-unknown"
#define ICMP_UNRNETTOS_NAME		"net-tos-unreachable"
#define ICMP_UNRHOSTTOS_NAME		"host-tos-unreachable"
#define ICMP_UNRHOSTISOLATED_NAME	"host-isolated"
#define ICMP_UNRNETPROHIB_NAME		"dod-net-prohibited"
#define ICMP_UNRPROHIB_NAME		"dod-host-prohibited"
#define ICMP_UNRADMIN_NAME		"administratively-prohibited" 
#define ICMP_UNRHOSTPREC_NAME		"host-precedence-unreachable"
#define ICMP_UNRPRECEDENCE_NAME		"precedence-unreachable"
#define ICMP_QUENCH_NAME		"source-quench"
#define ICMP_REDIRECT_NAME		"redirect"
#define ICMP_REDNET_NAME		"net-redirect"
#define ICMP_REDHOST_NAME		"host-redirect"
#define ICMP_REDSERVNET_NAME		"net-tos-redirect"
#define ICMP_REDSERVHOST_NAME		"host-tos-redirect"
#define ICMP_ALTADDRESS_NAME		"alternate-address"
#define ICMP_ECHO_NAME			"echo"
#define	ICMP_RDPADVERT_NAME		"router-advertisement"
#define	ICMP_RDPSOLICIT_NAME		"router-solicitation"
#define ICMP_TIMEEXCEEDED_NAME		"time-exceeded"
#define ICMP_TIMTTL_NAME		"ttl-exceeded"
#define ICMP_TIMFRAG_NAME		"reassembly-timeout"
#define ICMP_PARAMETER_NAME		"parameter-problem"
#define ICMP_PARAMPROB_NAME		"general-parameter-problem"
#define ICMP_PARAMREQUIRED_NAME		"option-missing"
#define ICMP_PARAMREQNOROOM_NAME	"no-room-for-option"
#define ICMP_TIMESTAMP_NAME		"timestamp-request"
#define ICMP_TIMESTAMPREPLY_NAME	"timestamp-reply"
#define ICMP_INFOREQUEST_NAME		"information-request"
#define ICMP_INFOREPLY_NAME		"information-reply"
#define ICMP_MASKREQUEST_NAME		"mask-request"
#define ICMP_MASKREPLY_NAME		"mask-reply"
#define ICMP_TRACEROUTE_NAME		"traceroute"
#define ICMP_CONVERSION_NAME		"conversion-error"
#define ICMP_MOBILEREDIRECT_NAME	"mobile-redirect"


#endif	__PARSER_DEFS_IP__
