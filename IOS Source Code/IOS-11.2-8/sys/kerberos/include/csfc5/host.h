/* $Id: host.h,v 3.2 1995/11/17 17:47:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/host.h,v $
 *------------------------------------------------------------------
 * host.h - Required function definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: host.h,v $
 * Revision 3.2  1995/11/17  17:47:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_HOST_H_
#define _CSFC5_HOST_H_  "%W% %G%"
/*.original los-proto.h 3.4 6/29/94*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#ifndef csfC5_getcfgnum
#define csfC5_getcfgnum( n ) n
#endif

#ifndef csfC5getcfgnum
#define csfC5getcfgnum csfCgetcfgnum
#endif

#ifndef csfC5getcfgstr
#define csfC5getcfgstr csfCgetcfgstr
#endif

#ifndef csfC5_ctx
typedef struct _csfC5_ctx {
	csf_ui32 xxx0;
} csfC5_ctx;
#endif

/*
 * Host-specific functions.
 */

csf_sts csfC5default_HostRealm ( char ** );
csf_sts csfC5get_HostRealmS ( const char *, char *** );
void csfC5freeC_HostRealmS ( char *** );

csf_sts csfC5get_KdcNameS ( const csfC5_data *, char *** );
void csfC5freeC_KdcNameS ( char *** );

/*
 * Subsystem-specific functions that live in the host area
 */

csf_sts csfC5defName_ccF ( char * dst, csf_size_t dstlen );

csf_sts csfC5defName_ktF ( char * dst, csf_size_t dstlen );

csf_sts csfC5genName_rc ( const csfC5_netHAddr *, const char *, char ** );

#endif /* _CSFC5_HOST_H_ */
