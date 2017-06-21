/* $Id: sysmgt_ciptcpip.h,v 3.2 1995/11/17 09:21:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/sysmgt_ciptcpip.h,v $
 *------------------------------------------------------------------
 * sysmgt_ciptcpip.h - Defintions for system management for CIP TCP/IP.
 *
 * April 1995, Mike Otto
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sysmgt_ciptcpip.h,v $
 * Revision 3.2  1995/11/17  09:21:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:45:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SYSMGT_CIPTCPIP_H__
#define __SYSMGT_CIPTCPIP_H__

/*
 * Special Note:  Keep this file in sync between the the CIP microcode
 *                and RP/RSP system code. The file will have the same
 *                name in both repositories. You must edit the changes
 *                so the cvs header does not get screwed up.   /motto
 */
 
 
/*
 * Sysmgt RPC CIP TCP/IP table identifiers
 */
typedef enum sysmgt_ciptcpip_table_ {
    SYSMGT_CIP_TCP,
    SYSMGT_CIP_TCP_CONN,
    SYSMGT_CIP_ICMP,
    SYSMGT_CIP_UDP,
    SYSMGT_CIP_UDP_LIST,
    SYSMGT_CIP_IP,
} sysmgt_ciptcpip_table_t;

/*
 * The cipIpEntry family. 
 */
typedef struct cip_ip_index_ {
    unsigned long   port;
    unsigned long   cipIpAddress;
} cip_ip_index_t;

typedef struct cip_ip_ {
    cip_ip_index_t        index;  
    long                  cipIpForwarding;
    long                  cipIpDefaultTTL;
    unsigned long         cipIpInReceives;
    unsigned long         cipIpInHdrErrors;
    unsigned long         cipIpInAddrErrors;
    unsigned long         cipIpForwDatagrams;
    unsigned long         cipIpInUnknownProtos;
    unsigned long         cipIpInDiscards;
    unsigned long         cipIpInDelivers;
    unsigned long         cipIpOutRequests;
    unsigned long         cipIpOutDiscards;
    unsigned long         cipIpOutNoRoutes;
    long                  cipIpReasmTimeout;
    unsigned long         cipIpReasmReqds;
    unsigned long         cipIpReasmOKs;
    unsigned long         cipIpReasmFails;
    unsigned long         cipIpFragOKs;
    unsigned long         cipIpFragFails;
    unsigned long         cipIpFragCreates;
    unsigned long         cipIpRoutingDiscards;
} cip_ip_t;

/*
 * The cipTcpStackEntry family. 
 */
typedef struct cip_tcp_ {
    cip_ip_index_t        index;
    long                  cipTcpRtoAlgorithm;
    long                  cipTcpRtoMin;
    long                  cipTcpRtoMax;
    long                  cipTcpMaxConn;
    unsigned long         cipTcpActiveOpens;
    unsigned long         cipTcpPassiveOpens;
    unsigned long         cipTcpAttemptFails;
    unsigned long         cipTcpEstabResets;
    unsigned long         cipTcpCurrEstab;
    unsigned long         cipTcpInSegs;
    unsigned long         cipTcpOutSegs;
    unsigned long         cipTcpRetransSegs;
    unsigned long         cipTcpInErrs;
    unsigned long         cipTcpOutRsts;
} cip_tcp_t;


/*
 * The cipTcpConnEntry family. 
 */
typedef struct cip_tcp_conn_index_ {
    unsigned long       port;
    unsigned long       cipIpAddress;
    long                cipTcpConnLocalPort;
    unsigned long       cipTcpConnRemAddress;
    long                cipTcpConnRemPort;
} cip_tcp_conn_index_t;

typedef struct cip_tcp_conn_ {
    cip_tcp_conn_index_t        index;
    long                        cipTcpConnState;
    unsigned long               cipTcpConnOutHCBytes_msw;
    unsigned long               cipTcpConnOutHCBytes_lsw;
    unsigned long               cipTcpConnInHCBytes_msw;
    unsigned long               cipTcpConnInHCBytes_lsw;
} cip_tcp_conn_t;


/*
 * The cipIcmpEntry family. 
 */
typedef struct cip_icmp_ {
    cip_ip_index_t        index;
    unsigned long         cipIcmpInMsgs;
    unsigned long         cipIcmpInErrors;
    unsigned long         cipIcmpInDestUnreachs;
    unsigned long         cipIcmpInTimeExcds;
    unsigned long         cipIcmpInParmProbs;
    unsigned long         cipIcmpInSrcQuenchs;
    unsigned long         cipIcmpInRedirects;
    unsigned long         cipIcmpInEchos;
    unsigned long         cipIcmpInAddrMaskReps;
    unsigned long         cipIcmpOutMsgs;
    unsigned long         cipIcmpOutErrors;
    unsigned long         cipIcmpOutDestUnreachs;
    unsigned long         cipIcmpOutEchos;
    unsigned long         cipIcmpOutEchoReps;
    unsigned long         cipIcmpOutTimestamps;
    unsigned long         cipIcmpOutTimestampReps;
    unsigned long         cipIcmpOutAddrMasks;
    unsigned long         cipIcmpOutAddrMaskReps;
} cip_icmp_t;


/*
 * The cipUdpEntry family. 
 */
typedef struct cip_udp_ {
    cip_ip_index_t        index;
    unsigned long         cipUdpInDatagrams;
    unsigned long         cipUdpNoPorts;
    unsigned long         cipUdpInErrors;
    unsigned long         cipUdpOutDatagrams;
} cip_udp_t;


/*
 * The cipUdpListenersEntry family. 
 */
typedef struct cip_udp_listeners_index_ {
    unsigned long       port;
    unsigned long       cipIpAddress;
    long                cipUdpLocalPort;
} cip_udp_listeners_index_t;

typedef struct cip_udp_listeners_ {
    cip_udp_listeners_index_t        index;
} cip_udp_listeners_t;

/*
 * Structure size definitions
 */
#define CIP_IP_SIZE                  sizeof(cip_ip_t)
#define CIP_TCP_SIZE                 sizeof(cip_tcp_t)
#define CIP_TCP_CONN_SIZE            sizeof(cip_tcp_conn_t)
#define CIP_ICMP_SIZE                sizeof(cip_icmp_t)
#define CIP_UDP_SIZE                 sizeof(cip_udp_t)
#define CIP_UDP_LISTENERS_SIZE       sizeof(cip_udp_listeners_t)
#define CIP_IP_INDEX_SIZE            sizeof(cip_ip_index_t)
#define CIP_TCP_CONN_INDEX_SIZE      sizeof(cip_tcp_conn_index_t)
#define CIP_UDP_LISTENERS_INDEX_SIZE sizeof(cip_udp_listeners_index_t)

#endif
