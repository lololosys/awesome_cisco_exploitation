/* $Id: ipnhrp.h,v 3.2 1995/11/17 17:34:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/ipnhrp.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * June 1995, Bruce Cole
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipnhrp.h,v $
 * Revision 3.2  1995/11/17  17:34:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/24  22:34:28  bcole
 * CSCdi39279:  NHRP: Aggregation needed to provide purge notification
 *
 * Provide aggregated NHRP responses.
 * Cache now consists of radix tree(s) instead of hash table.
 * Allow NHRP resolution during encapsulation of NHRP purge messages.
 * For IP, NHRP now a routing process so as to receive callbacks.
 * Allow for radix tree walk where varargs have already been pre-collected.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void nhrp_create_pdb(void);
extern void nhrp_delete_pdb(void);
extern idbtype *nhrp_ip_rtlookup(nhrpaddr *, nhrp_mask *, void **, boolean *);
extern void nhrp_ip_rtrevise(void *);
