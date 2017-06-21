/*
 *	P A R S E R _ D E F S _ T C P I P . H
 *
 * $Id: parser_defs_tcpip.h,v 3.7.4.1 1996/04/15 21:20:43 bcole Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_defs_tcpip.h,v $
 * 
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_tcpip.h,v $
 * Revision 3.7.4.1  1996/04/15  21:20:43  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.7  1996/03/11  19:26:13  ahh
 * CSCdi43020:  netbooting from not-directly connected tftp-server fails
 * Allow user to force a boot interface when spurious redirects cause
 * our automagic booting to fail.
 *
 * Revision 3.6  1996/02/26  07:14:25  pst
 * CSCdi49774:  remove as-path command from regular expression library
 *                 (whoops: I missed a file when I was moving stuff into
 *                  the interim tree to commit... dunce cap)
 *
 * Revision 3.5  1996/01/29  07:01:06  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.4  1995/11/21  07:43:19  jenny
 * CSCdi43195:  TFTP: cannot specify source address
 *
 * Revision 3.3  1995/11/20  23:21:59  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  18:53:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/25  08:36:28  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.2  1995/08/29  21:21:21  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * Added new knob "service udp-small-servers".
 *
 * Revision 2.1  1995/06/07  22:31:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* IP defines for ip_command() */
enum {
    IP_DEFAULTGATEWAY,
    IP_ROUTING,
    IP_SRCROUTE,
    IP_SUBNETZERO,
    IP_SECURITY,
    IP_FLOW_CACHE,
    IP_GRATUITOUS_ARPS
};

/*
 * TCP commands
 */
enum {
    IP_TCP_CHUNKSZ,
    IP_TCP_WINDSZ,
    IP_TCP_FINWAITTM,
    IP_TCP_QUEUEMAX,
    IP_TCP_SYNWAIT,
    IP_TCP_PATHMTU,
    IP_TELNET_SOURCE,
    IP_TCP_TUNNEL
};

/*
 * IP TFTP commands
 */

enum {
    IP_TFTPMODE,
    IP_TFTP_SOURCE,
    IP_TFTP_BOOT_INTERFACE
};

/*
 * IP DNS commands
 */

enum {
    IP_DOMAINLOOKUP,
    IP_DOMAINLOOKUP_NSAP,
};


/* OSPF defines for ospf_routing_commands() */
enum {
    IP_OSPFNAMELOOKUP
};
  
/*
 * Misc trash
 * This aren't actually used for switch statements, but
 * they might be someday, so leave them for now.
 */
enum {
    IP_ACCTLIST,
    IP_ACCTTHRESHOLD,
    IP_ACCTTRANSITS,
    IP_ALIAS,
    IP_DEFAULTNETWORK,
    IP_ROUTE,
    IP_HELPER_ADDR,
    IP_MTU,
    IP_PROBE,
    IP_ROUTEC,
    IP_UNNUMBERED,
    IP_UNREACHABLE,
    IP_ROUTEC_SAME,
    IP_DHCPSERVER
};

/* IP security defines for ip_security_command() */
enum {
    P_IPSECSTYLE_FIRST,
    P_IPSECSTYLE_STRIP,
    P_IPSECSTYLE_ADD,
    P_IPSECSTYLE_EXTSECOK,
    P_IPSECSTYLE_IMPLICIT,
    P_IPSECSTYLE_IGNOREAUTH,
    P_IPSECSTYLE_IGNOREEXT,
    P_IPSECSTYLE_IGNORECIPSO,
    P_IPSECMODE_DEDICATED,
    P_IPSECMODE_MULTILEVEL,
    P_IPSEC_ALLOW_RESERVED,
    P_IPSEC_DEFAULT
};

/* Extended IPSO defines */
#define IP_SEC_ESO_MIN		 0
#define IP_SEC_ESO_MAX		 1
#define IP_SEC_AESO		 2
#define IP_SEC_MAX_COMPARTMENT_BYTES	16

/* TCP header compression defines for ip_header_compression_command() */
enum {
    TCP_COMPRESS_PASSIVE,
    TCP_COMPRESS_ON,
    TCP_COMPRESS_OFF
};

/* Protocol types for ip_forwardproto_command() */

enum {
    PROTOCOL_IP,
    PROTOCOL_TCP,
    PROTOCOL_UDP,
    PROTOCOL_ICMP,
    PROTOCOL_ND,
    PROTOCOL_SPANNING,
    PROTOCOL_FLOOD,
    PROTOCOL_SDNS
};

/*
 * Some "ip tcp packet" debug filtering.
 */

enum {
    TCPPKT_DBG_INOUT,
    TCPPKT_DBG_IN,
    TCPPKT_DBG_OUT
};

/* ip accounting types */

enum {
    IP_ACCOUNTING_OUTPAKS,
    IP_ACCOUNTING_VIOLATIONS,
    NUM_IP_ACCOUNTING_TYPES		/* Number of types of accounting */
};

enum {
    RCMD_SOURCE_INTERFACE,
    RCMD_REMOTE_USER,
    RCMD_REMOTE_HOST,
    RCMD_DOMAINLOOKUP,
    RCMD_RSH_ENABLE,
    RCMD_RCP_ENABLE,
    RCMD_RSH_DISABLED_CMD
};

/* FTP defines for ip_ftp_command() */
enum {
    FTP_PASSIVE,
    FTP_SOURCE_INTERFACE,
    FTP_USERNAME,
    FTP_PASSWORD
};

/* TCP Test commands */
enum {
    TEST_TCP_LINE,
    TEST_TCP_DROP,
    TEST_TCP_DELAY,
    TEST_TCP_RANDOM,
};

/* TCP Clear commands */
enum {
    CLEAR_TCP_BY_PORT,
    CLEAR_TCP_BY_TCB,
    CLEAR_TCP_BY_LINE
};
