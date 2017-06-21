/* $Id: net.h,v 3.2.60.1 1996/04/19 15:12:38 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/include/csfc5/net.h,v $
 *------------------------------------------------------------------
 * net.h - Network-specific definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: net.h,v $
 * Revision 3.2.60.1  1996/04/19  15:12:38  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.2  1996/04/02  08:37:55  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:47:29  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:48:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_NET_H_
#define _CSFC5_NET_H_ "%W% %G%"
/*.original hostaddr.h 3.2 2/2/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#ifndef csfC5_net_handle
typedef struct TCB csfC5_net_handle;
#endif

#ifndef csfC5_netHAddrType
typedef csf_ui16 csfC5_netHAddrType;
#endif


typedef struct _csfC5_netHAddr {
	csfC5_netHAddrType haddr_type;
	csfC5_data haddr_data;
	csf_o haddr_data0[16];
} csfC5_netHAddr;

void csfC5clear_netHAddr( csfC5_netHAddr * );
void csfC5free_netHAddr( csfC5_netHAddr * );
void csfC5freeC_netHAddr( csfC5_netHAddr ** );
void csfC5freeC_netHAddrS( csfC5_netHAddr *** );
csf_sts csfC5copy_netHAddr( const csfC5_netHAddr *, csfC5_netHAddr * );
csf_sts csfC5copyC_netHAddr( const csfC5_netHAddr *, csfC5_netHAddr ** );
csf_sts csfC5copyC_netHAddrS( csfC5_netHAddr * const *, csfC5_netHAddr *** );

csf_sts csfC5appendL_netHAddr(
	csfC5_netHAddr * const *,
	csfC5_netHAddr ***
);

csf_boolean csfC5isIn_netHAddr (
	const csfC5_netHAddr *,
	csfC5_netHAddr * const *
);

csf_boolean csfC5isEq_netHAddr (
	const csfC5_netHAddr *,
	const csfC5_netHAddr *
);


csf_sts csfC5netReadMsg (
	csfNET_handle *,
	csfC5_data *
);

csf_sts csfC5netWriteMsg (
	csfNET_handle *,
	csfC5_data *
);

csf_sts csfC5netLocaladdr (
	csfC5_netHAddr ***
);

csf_sts csfC5netSendtoKdc (
	const csfC5_data *,
	const csfC5_data *,
	csfC5_data *
);


typedef struct _csfC5_netFAddr {
	csfC5_netHAddr *haddr;
	csf_ui32 port;			/* port, for some haddr types.
								large enough for most protos? */
} csfC5_netFAddr;

int csfC5netFaddrOrder (
	const csfC5_netFAddr *,
	const csfC5_netFAddr *
);

#ifndef csfC5_netINetAddr
typedef csf_ui32 csfC5_netINetAddr;
#endif

#ifndef csfC5_netINetport
typedef csf_i16 csfC5_netINetport;
#endif

#endif /* _CSFC5_NET_H_ */
