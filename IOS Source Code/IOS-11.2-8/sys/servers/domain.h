/* $Id: domain.h,v 3.4 1996/03/09 22:57:24 widmer Exp $
 * $Source: /release/111/cvs/Xsys/servers/domain.h,v $
 *------------------------------------------------------------------
 * domain.h -- domain service definitions.  RFC 882, 883, etc.
 *
 * 27-October-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: domain.h,v $
 * Revision 3.4  1996/03/09  22:57:24  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.3  1995/11/29  22:09:56  pst
 * CSCdi44659:  clean up internals of domain code (make an interface API)
 *
 * Revision 3.2  1995/11/17  18:51:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/30  07:41:07  pitargue
 * CSCdi30543:  dns invalid  with domain-name and domain-list specified
 * create a name lookup path for FQDN's only.  domain-list will not affect
 * the lookup, i.e., domains will not be appended to FQDN's.
 *
 * Revision 2.1  1995/06/07  22:44:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DOMAIN_H__
#define __DOMAIN_H__

#define MAX_DNS_PTR_LEN 	30	/* length for reversed ascii string
					   xxx.xxx.xxx.xxx.in-addr.arpa */
#define MAX_DNS_LEN 		512	/* max datagram size include DNS 
					   header */ 

extern boolean   domain_match(char *, char *);
extern nametype *domain_name_lookup(char *, int);
extern nametype *domain_address_lookup(ipaddrtype);
extern void	 domainlist_command(parseinfo *);
extern nametype *domain_FQDN_lookup(char *, int);
extern boolean   domain_validate_addr(addrtype *);
extern int	 comp_copy(char *, char *, int, char *);
extern int	 numdotcat(uint, char *);

#endif __DOMAIN_H__
