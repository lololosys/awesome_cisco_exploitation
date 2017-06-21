/* $Id: lreq.h,v 3.2 1995/11/17 17:48:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/lreq.h,v $
 *------------------------------------------------------------------
 * lreq.h - Lreq definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lreq.h,v $
 * Revision 3.2  1995/11/17  17:48:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_LREQ_H_
#define _CSFC5_LREQ_H_  "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#ifndef csfC5_lReqType
typedef csf_i32 csfC5_lReqType;
#endif

typedef struct _csfC5_lReq {
	csfC5_lReqType lr_type;
	csfC5_tStamp value;
} csfC5_lReq;

void csfC5clear_lReq( csfC5_lReq * );
void csfC5free_lReq( csfC5_lReq * );
void csfC5freeC_lReqS( csfC5_lReq *** );

#endif
