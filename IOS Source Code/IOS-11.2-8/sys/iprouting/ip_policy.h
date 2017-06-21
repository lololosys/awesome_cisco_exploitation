/* $Id: ip_policy.h,v 3.2 1995/11/17 17:34:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/ip_policy.h,v $
 *------------------------------------------------------------------
 * ip_policy.h -- IP policy routing definitions
 *
 * Feb 1995 Tony Li
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_policy.h,v $
 * Revision 3.2  1995/11/17  17:34:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/12  07:19:11  tli
 * CSCdi36426:  Policy routing will not disable on an interface
 * - Policy routed packets were creating fastswitching cache entries.
 *   That's not till _next_ release.
 * - Add counters to record packet and byte count on a per route map entry
 *   basis.
 * - Marketing identified a catastrophic deficiency.  Implemented "set ip
 *   tos" and "set ip precedence" to cover this deficiency.
 *
 * Revision 2.1  1995/06/07  21:06:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * iprouting/ip_policy.c
 */
extern void ip_policy_command(parseinfo *csb);
extern void route_map_set_default_int_command(parseinfo *);
extern void route_map_set_default_nh_command(parseinfo *csb);
extern void route_map_set_ip_tos_command(parseinfo *csb);
extern void route_map_set_ip_precedence_command(parseinfo *csb);
extern void route_map_match_length_command(parseinfo *csb);

/*
 * iprouting/ip_policy_chain.c
 */
extern void ip_policy_parser_init(void);
