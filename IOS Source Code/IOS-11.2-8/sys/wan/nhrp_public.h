/* $Id: nhrp_public.h,v 3.3 1996/01/11 07:06:34 bcole Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/wan/nhrp_public.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nhrp_public.h,v $
 * Revision 3.3  1996/01/11  07:06:34  bcole
 * CSCdi46724:  NHRP: Provide for migration from RFC1577
 *
 * Glean NHRP information from ATM ARP server.
 *
 * Revision 3.2  1995/11/17  18:03:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:36:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:04  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern boolean nhrp_tunnel_bcast(paktype *);
/*
 * nhrp_cache.c
 */
extern void nhrp_show_cache(ushort, idbtype *, int, int);
/*
 * wan/nhrp_media.c
 */
extern void nhrp_media_registry(void);
/*
 * nhrp_vc.c
 */
extern boolean nhrp_macaddr_map(paktype *, addrtype *, boolean,
				staticmap_type **);
extern void nhrp_vc_init(void);
/*
 * IP specific
 * iprouting/ipnhrp.c
 * iprouting/ipnhrp_chain.c
 */
extern void nhrp_ip_clear(parseinfo *);
extern void nhrp_ip_parser_init(void);
/*
 * IPX specific
 * xns/ipxnhrp.c
 * xns/ipxnhrp_chain.c
 */
extern void nhrp_ipx_clear(parseinfo *);
extern void nhrp_ipx_parser_init(void);
