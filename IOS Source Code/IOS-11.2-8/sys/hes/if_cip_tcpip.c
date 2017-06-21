/* $Id: if_cip_tcpip.c,v 3.3.60.1 1996/03/18 19:41:30 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/if_cip_tcpip.c,v $
 *------------------------------------------------------------------
 * if_cip_tcpip.c - CIP TCP/IP management interface
 *
 * April 1995, Mike Otto
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_cip_tcpip.c,v $
 * Revision 3.3.60.1  1996/03/18  19:41:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  21:46:26  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  09:17:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:36  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:41:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "if_mci.h"
#include "if_fci.h"
#include "if_cip.h"
#include "sysmgt_ciptcpip.h"
#include "if_cip_sysmgt_rpc.h"
#include "../if/channel_debug.h"


/*
 * get_cip_ip - Retrieve IP info from specified CIP card.
 * Input: search = SEARCH_NEXT or SEARCH_MATCH
 *        hwidb  = pointer to interface 
 *        cip_ip_address = IP address used in search
 *        cip_ip_p = pointer to data structure to 
 *        return IP info in.
 * Output: cip_ip_p pointer if found or NULL if not found
 */
cip_ip_t *get_cip_ip (int search, hwidbtype *hwidb, 
                      unsigned long cip_ip_address, 
                      cip_ip_t *cip_ip_p) {
    netmgt_header_t nm_hdr;
    cip_ip_t     *results = NULL;
    unsigned long port;
    
    if (cip_ip_p) {
        nm_hdr.class           = SYSMGT_TCP_IP;
        nm_hdr.type            = (ushort)SYSMGT_CIP_IP;
        nm_hdr.slot            = hwidb->slot;
        nm_hdr.index_length    = CIP_IP_INDEX_SIZE;
        nm_hdr.data_length     = CIP_IP_SIZE;
        nm_hdr.flags.operation = (search == SEARCH_MATCH) ? 
                                  NETMGT_GET_MATCH : NETMGT_GET_NEXT;
        port = hwidb->slotunit;
        cip_ip_p->index.port = port;
        cip_ip_p->index.cipIpAddress = cip_ip_address;

        if (channel_sysmgt_debug) {
            buginf("\n%s CIP IP (%d/%d,%15i)", 
                   netmgt_operations[nm_hdr.flags.operation],
                   nm_hdr.slot, cip_ip_p->index.port, cip_ip_address);
        }

        results = (cip_ip_t *)sysmgt_rpc(hwidb, &nm_hdr, cip_ip_p);
        if (results && (cip_ip_p->index.port != port))
            results = NULL;
    }
    return results;
}

/*
 * get_cip_tcp - Retrieve TCP info from specified CIP card.
 * Input: search = SEARCH_NEXT or SEARCH_MATCH
 *        hwidb  = pointer to interface 
 *        cip_ip_address = IP address used in search
 *        cip_tcp_p = pointer to data structure to 
 *        return TCP info in.
 * Output: cip_tcp_p pointer if found or NULL if not found
 */
cip_tcp_t *get_cip_tcp (int search, hwidbtype *hwidb, 
                        unsigned long cip_ip_address, 
                        cip_tcp_t *cip_tcp_p) {
    netmgt_header_t nm_hdr;
    cip_tcp_t    *results = NULL;
    unsigned long port;

    if (cip_tcp_p) {
        nm_hdr.class           = SYSMGT_TCP_IP;
        nm_hdr.type            = (ushort)SYSMGT_CIP_TCP;
        nm_hdr.index_length    = CIP_IP_INDEX_SIZE;
        nm_hdr.data_length     = CIP_TCP_SIZE;
        nm_hdr.slot            = hwidb->slot;
        nm_hdr.flags.operation = (search == SEARCH_MATCH) ? 
                                  NETMGT_GET_MATCH : NETMGT_GET_NEXT;
        port = hwidb->slotunit;
        cip_tcp_p->index.port = port;
        cip_tcp_p->index.cipIpAddress = cip_ip_address;

        if (channel_sysmgt_debug) {
            buginf("\n%s CIP TCP (%d/%d,%15i)", 
                   netmgt_operations[nm_hdr.flags.operation],
                   nm_hdr.slot, cip_tcp_p->index.port, cip_ip_address);
        }

        results = (cip_tcp_t *)sysmgt_rpc(hwidb, &nm_hdr, cip_tcp_p);
        if (results && (cip_tcp_p->index.port != port))
            results = NULL;
    }
    return results;
}

/*
 * get_cip_tcp_conn - Retrieve TCP Connection info from specified CIP card.
 * Input: search = SEARCH_NEXT or SEARCH_MATCH
 *        hwidb  = pointer to interface 
 *        cip_ip_address = IP address used in search
 *        local and remote port/addresses for search
 *        cip_tcp_conn_p = pointer to data structure to 
 *        return TCP Connection info in.
 * Output: cip_tcp_conn_p pointer if found or NULL if not found
 */
cip_tcp_conn_t *get_cip_tcp_conn (int search, hwidbtype *hwidb, 
                                  unsigned long cip_ip_address, 
                                  long cip_tcp_conn_local_port,
                                  unsigned long cip_tcp_conn_rem_address,
                                  long cip_tcp_conn_rem_port,
                                  cip_tcp_conn_t *cip_tcp_conn_p) {
    netmgt_header_t nm_hdr;
    cip_tcp_conn_t *results = NULL;
    unsigned long port;
    
    if (cip_tcp_conn_p) {
        nm_hdr.class           = SYSMGT_TCP_IP;
        nm_hdr.type            = (ushort)SYSMGT_CIP_TCP_CONN;
        nm_hdr.data_length     = CIP_TCP_CONN_SIZE;
        nm_hdr.index_length    = CIP_TCP_CONN_INDEX_SIZE;
        nm_hdr.slot            = hwidb->slot;
        nm_hdr.flags.operation = (search == SEARCH_MATCH) ? 
                                  NETMGT_GET_MATCH : NETMGT_GET_NEXT;
        port = hwidb->slotunit;
        cip_tcp_conn_p->index.port = port;
        cip_tcp_conn_p->index.cipIpAddress           = cip_ip_address;
        cip_tcp_conn_p->index.cipTcpConnRemAddress   = cip_tcp_conn_rem_address;
        cip_tcp_conn_p->index.cipTcpConnLocalPort    = cip_tcp_conn_local_port;
        cip_tcp_conn_p->index.cipTcpConnRemPort      = cip_tcp_conn_rem_port;

        if (channel_sysmgt_debug) {
            buginf("\n%s CIP TCP CONN (%d/%d,%15i,%d,%15i,%d)", 
                   netmgt_operations[nm_hdr.flags.operation],
                   nm_hdr.slot, cip_tcp_conn_p->index.port, 
                   cip_ip_address, cip_tcp_conn_local_port,
                   cip_tcp_conn_rem_address, cip_tcp_conn_rem_port);
        }

        results = (cip_tcp_conn_t *)sysmgt_rpc(hwidb, &nm_hdr, cip_tcp_conn_p);
        if (results && (cip_tcp_conn_p->index.port != port))
            results = NULL;
    }
    return results;
}

/*
 * get_cip_icmp - Retrieve ICMP info from specified CIP card.
 * Input: search = SEARCH_NEXT or SEARCH_MATCH
 *        hwidb  = pointer to interface 
 *        cip_ip_address = IP address used in search
 *        cip_icmp_p = pointer to data structure to 
 *        return ICMP info in.
 * Output: cip_icmp_p pointer if found or NULL if not found
 */
cip_icmp_t *get_cip_icmp (int search, hwidbtype *hwidb, 
                          unsigned long cip_ip_address,
                          cip_icmp_t *cip_icmp_p) {
   netmgt_header_t nm_hdr;
   cip_icmp_t *results = NULL;
   unsigned long port;
   
    if (cip_icmp_p) {
        nm_hdr.class           = SYSMGT_TCP_IP;
        nm_hdr.type            = (ushort)SYSMGT_CIP_ICMP;
        nm_hdr.index_length    = CIP_IP_INDEX_SIZE;
        nm_hdr.data_length     = CIP_ICMP_SIZE;
        nm_hdr.slot            = hwidb->slot;
        nm_hdr.flags.operation = (search == SEARCH_MATCH) ? 
                                  NETMGT_GET_MATCH : NETMGT_GET_NEXT;
        port = hwidb->slotunit;
        cip_icmp_p->index.port = port;
        cip_icmp_p->index.cipIpAddress = cip_ip_address;

        if (channel_sysmgt_debug) {
            buginf("\n%s CIP ICMP (%d/%d,%15i)", 
                   netmgt_operations[nm_hdr.flags.operation],
                   nm_hdr.slot, cip_icmp_p->index.port, cip_ip_address);
        }

        results = (cip_icmp_t *)sysmgt_rpc(hwidb, &nm_hdr, cip_icmp_p);
        if (results && (cip_icmp_p->index.port != port))
            results = NULL;
    }
    return results;
}

/*
 * get_cip_udp - Retrieve UDP info from specified CIP card.
 * Input: search = SEARCH_NEXT or SEARCH_MATCH
 *        hwidb  = pointer to interface 
 *        cip_ip_address = IP address used in search
 *        cip_udp_p = pointer to data structure to 
 *        return UDP info in.
 * Output: cip_udp_p pointer if found or NULL if not found
 */
cip_udp_t *get_cip_udp (int search, hwidbtype *hwidb, 
                        unsigned long cip_ip_address,
                        cip_udp_t *cip_udp_p) {
    netmgt_header_t nm_hdr;
    cip_udp_t *results = NULL;
    unsigned long port;
   
    if (cip_udp_p) {
        nm_hdr.class           = SYSMGT_TCP_IP;
        nm_hdr.type            = (ushort)SYSMGT_CIP_UDP;
        nm_hdr.index_length    = CIP_IP_INDEX_SIZE;
        nm_hdr.data_length     = CIP_UDP_SIZE;
        nm_hdr.slot            = hwidb->slot;
        nm_hdr.flags.operation = (search == SEARCH_MATCH) ? 
                                  NETMGT_GET_MATCH : NETMGT_GET_NEXT;
        port = hwidb->slotunit;
        cip_udp_p->index.port = port;
        cip_udp_p->index.cipIpAddress = cip_ip_address;

        if (channel_sysmgt_debug) {
            buginf("\n%s CIP UDP (%d/%d,%15i)", 
                   netmgt_operations[nm_hdr.flags.operation],
                   nm_hdr.slot, cip_udp_p->index.port, cip_ip_address);
        }

        results = (cip_udp_t *)sysmgt_rpc(hwidb, &nm_hdr, cip_udp_p);
        if (results && (cip_udp_p->index.port != port))
            results = NULL;
    }
    return results;
}

/*
 * get_cip_udp_listeners - Retrieve UDP Listeners info from specified CIP card.
 * Input: search = SEARCH_NEXT or SEARCH_MATCH
 *        hwidb  = pointer to interface 
 *        cip_ip_address = IP address used in search
 *        cip_udp_listeners_p = pointer to data structure to 
 *        return UDP Listeners info in.
 * Output: cip_udp_listeners_p pointer if found or NULL if not found
 */
cip_udp_listeners_t *get_cip_udp_listeners (int search, hwidbtype *hwidb, 
                                 unsigned long cip_ip_address,
                                 long cip_udp_local_port,
                                 cip_udp_listeners_t *cip_udp_listeners_p) {
    netmgt_header_t nm_hdr;
    cip_udp_listeners_t *results = NULL;
    unsigned long port;

    if (cip_udp_listeners_p) {
        nm_hdr.class           = SYSMGT_TCP_IP;
        nm_hdr.type            = (ushort)SYSMGT_CIP_UDP_LIST;
        nm_hdr.index_length    = CIP_UDP_LISTENERS_INDEX_SIZE;
        nm_hdr.data_length     = CIP_UDP_LISTENERS_SIZE;
        nm_hdr.flags.operation = (search == SEARCH_MATCH) ? 
                                  NETMGT_GET_MATCH : NETMGT_GET_NEXT;
        nm_hdr.slot            = hwidb->slot;
        port = hwidb->slotunit;
        cip_udp_listeners_p->index.port = port;
        cip_udp_listeners_p->index.cipIpAddress       = cip_ip_address;
        cip_udp_listeners_p->index.cipUdpLocalPort    = cip_udp_local_port;
   
        if (channel_sysmgt_debug) {
            buginf("\n%s CIP UPD LISTENERS (%d/%d,%15i,%d)", 
                   netmgt_operations[nm_hdr.flags.operation],
                   nm_hdr.slot, cip_udp_listeners_p->index.port, 
                   cip_ip_address, cip_udp_local_port);
        }

        results = (cip_udp_listeners_t *)sysmgt_rpc(hwidb, &nm_hdr, 
                                                    cip_udp_listeners_p);
        if (results && (cip_udp_listeners_p->index.port != port))
            results = NULL;
    }
    return results;
}

