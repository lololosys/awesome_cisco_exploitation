/* $Id: if_cip_tcpip.h,v 3.2 1995/11/17 09:17:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/if_cip_tcpip.h,v $
 *------------------------------------------------------------------
 * if_cip_tcpip.c - CIP TCP/IP management interface
 *
 * April 1995, Mike Otto
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_cip_tcpip.h,v $
 * Revision 3.2  1995/11/17  09:17:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:40:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:41:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_CIP_TCPIP_H__
#define __IF_CIP_TCPIP_H__

#include "sysmgt_ciptcpip.h"
 
 
cip_ip_t *get_cip_ip(int search, hwidbtype *hwidb, 
                     unsigned long cip_ip_address, 
                     cip_ip_t *cip_ip_p);
cip_tcp_t *get_cip_tcp(int search, hwidbtype *hwidb, 
                       unsigned long cip_ip_address, 
                       cip_tcp_t *cip_tcp_p);
cip_tcp_conn_t *get_cip_tcp_conn( int search, hwidbtype *hwidb, 
                                  unsigned long cip_ip_address, 
                                  long cip_tcp_conn_local_port,
                                  unsigned long cip_tcp_conn_rem_address,
                                  long cip_tcp_conn_rem_port,
                                  cip_tcp_conn_t *cip_tcp_conn_p);
cip_icmp_t *get_cip_icmp( int search, hwidbtype *hwidb, 
                          unsigned long cip_ip_address,
                          cip_icmp_t *cip_icmp_p);
cip_udp_t *get_cip_udp( int search, hwidbtype *hwidb, 
                        unsigned long cip_ip_address,
                        cip_udp_t *cip_udp_p);
cip_udp_listeners_t *get_cip_udp_listeners( int search, hwidbtype *hwidb, 
                                     unsigned long cip_ip_address,
                                     long cip_udp_local_port,
                                     cip_udp_listeners_t *cip_udp_listeners_p);


#endif
