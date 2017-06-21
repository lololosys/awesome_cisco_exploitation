/* $Id: ip.h,v 3.2 1995/11/17 17:49:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/net/ip.h,v $
 *------------------------------------------------------------------
 * ip.h - IP definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip.h,v $
 * Revision 3.2  1995/11/17  17:49:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_NET_IP_H_
#define _CSFC5_NET_IP_H_ "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All rights reserved.
 */

csf_sts csfC5netinetLocateKdc
	( const csfC5_data *, struct sockaddr **, int * );

#endif /* _CSFC5_NET_IP_H_ */
