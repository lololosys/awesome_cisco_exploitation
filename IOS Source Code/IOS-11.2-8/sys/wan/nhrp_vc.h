/* $Id: nhrp_vc.h,v 3.2 1995/11/17 18:03:49 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/nhrp_vc.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nhrp_vc.h,v $
 * Revision 3.2  1995/11/17  18:03:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:36:35  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:04  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__NHRP_VC_H__
#define	__NHRP_VC_H__

extern void *nhrp_add_map(idbtype *, addrtype *, uchar const *, nhrpfuns *);
extern uchar *nhrp_proto_resolve(idbtype *, ushort, nhrpaddr *, uchar *,
				 nhrpfuns *);
extern void nhrp_macaddr_unmap(idbtype *, nhrpaddr *, uchar *, nhrpfuns *);
extern boolean nhrp_vc_exists(idbtype *swidb, nhrpaddr *addr, nhrpfuns *fun);

#endif	__NHRP_VC_H__
