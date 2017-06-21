/* $Id: ip_actions.h,v 3.2.60.1 1996/07/29 22:45:39 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_actions.h,v $
 *------------------------------------------------------------------
 * Header file for IP specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ip_actions.h,v $
 * Revision 3.2.60.1  1996/07/29  22:45:39  snyder
 * CSCdi64484:  declare arrays const in sub_ipcore
 *              moves 1344 out of data, 20 out of image
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:33:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/12  07:18:38  tli
 * CSCdi36426:  Policy routing will not disable on an interface
 * - Policy routed packets were creating fastswitching cache entries.
 *   That's not till _next_ release.
 * - Add counters to record packet and byte count on a per route map entry
 *   basis.
 * - Marketing identified a catastrophic deficiency.  Implemented "set ip
 *   tos" and "set ip precedence" to cover this deficiency.
 *
 * Revision 2.1  1995/06/07  20:56:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

char *ip_addr_short_help(parseinfo *, uint, idbtype *);
boolean match_ip_addr(parseinfo *, char *, int *, void *,
		      int, uint, idbtype *);
void tcpport_action(parseinfo *, transition *, portparse_struct * const);
void udpport_action(parseinfo *, transition *, portparse_struct * const);
void nv_add_tcp_port(uint port);
void nv_add_udp_port(uint port);
void printf_tcp_port(uint port);
void printf_udp_port(uint port);

extern const id_string precedence_values[];
extern const id_string tos_values[];
extern const id_string tcp_ports[];
extern const id_string udp_ports[];
