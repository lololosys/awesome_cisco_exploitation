/* $Id: authz.h,v 3.2 1995/11/17 17:47:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/authz.h,v $
 *------------------------------------------------------------------
 * authz.h - Authenticator definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: authz.h,v $
 * Revision 3.2  1995/11/17  17:47:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_AUTHZ_H_
#define _CSFC5_AUTHZ_H_  "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#ifndef csfC5_authzType
typedef csf_ui16 csfC5_authzType;
#endif

typedef struct _csfC5_authz {
	csfC5_authzType authz_type;
	csfC5_data authz_data;
} csfC5_authz;

void csfC5clear_authz( csfC5_authz * );
void csfC5free_authz( csfC5_authz * );
void csfC5freeC_authzS( csfC5_authz *** );
csf_sts csfC5copyC_authzS( csfC5_authz * const *, csfC5_authz *** );

#endif
