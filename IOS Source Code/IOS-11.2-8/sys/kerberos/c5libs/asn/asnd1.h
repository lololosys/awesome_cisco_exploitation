/* $Id: asnd1.h,v 3.2 1995/11/17 17:43:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asnd1.h,v $
 *------------------------------------------------------------------
 * asnd1.h - ASN decode definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnd1.h,v $
 * Revision 3.2  1995/11/17  17:43:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_ASND1_H_
#define _CSFC5_ASND1_H_ "%W% %G%"
/*.original src/lib/krb5/asn.1/asn1_decode_k.h*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>
#include <csfc5/sts/asn_sts.hs>
#include <csf/asnx.h>

#ifndef _CSFC5_ASND_CHECKTYPES
#define _CSFC5_ASND_CHECKTYPES 0
#endif

#include "asnd1x.h"

#if _CSFC5_ASND_CHECKTYPES

#define _CSFC5_ASND1F_PROTO( fn, dt ) \
	void fn( csfC5_asnD1Ctx *, dt )

#define _CSFC5_ASND1F_ENTRY( fn, dt ) \
	void fn( csfC5_asnD1Ctx *dctx, dt val ) \
	{

#else

#define _CSFC5_ASND1F_PROTO( fn, dt ) \
	void fn( csfC5_asnD1Ctx *, void * )

#define _CSFC5_ASND1F_ENTRY( fn, dt ) \
	void fn( csfC5_asnD1Ctx *dctx, void *val0 ) \
	{ \
	dt val = (dt) val0;

#endif

_CSFC5_ASND1F_PROTO( csfC5asnD1_autht, csfC5_autht * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_authz, csfC5_authz * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_cksBlock, csfC5_cksBlock * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_crsEData, csfC5_crsEData * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_crsKBlock, csfC5_crsKBlock * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_lReq, csfC5_lReq * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgApRep, csfC5_msgApRep * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgApRepEPart, csfC5_msgApRepEPart * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgApReq, csfC5_msgApReq * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgAsRep, csfC5_msgKdcRep * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgAsReq, csfC5_msgKdcReq * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgCred, csfC5_msgCred * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgCredEPart, csfC5_msgCredEPart * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgCredInfo, csfC5_msgCredInfo * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgErr, csfC5_msgErr * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgKdcRep, csfC5_msgKdcRep * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgKdcRepEPart, csfC5_msgKdcRepEPart * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgKdcReq, csfC5_msgKdcReq * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgKdcReqBody, csfC5_msgKdcReqBody * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgPriv, csfC5_msgPriv * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgPrivEPart, csfC5_msgPrivEPart * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgSafe, csfC5_msgSafe * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgSafeBody, csfC5_msgSafeBody * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgSPBody, csfC5_msgSPBody * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgTgsRep, csfC5_msgKdcRep * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_msgTgsReq, csfC5_msgKdcReq * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_netHAddr, csfC5_netHAddr * );

_CSFC5_ASND1F_PROTO( csfC5asnD1_oStr, csfC5_data * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_octet, csf_o * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_pauth, csfC5_pauth * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_prnName, csfC5_prnName * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_prnNC, csfC5_prnNC * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_tStamp, csfC5_tStamp * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_tkt, csfC5_tkt * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_tktEPart, csfC5_tktEPart * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_tktFlags, csfC5_tktFlags * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_trans, csfC5_trans * );

_CSFC5_ASND1F_PROTO( csfC5asnD1_gStr, csfC5_data * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_cStr, csfC5_data * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_asnData, csfC5_data * );

_CSFC5_ASND1F_PROTO( csfC5asnD1_i8, csf_i8 * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_i16, csf_i16 * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_i32, csf_i32 * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_ui8, csf_ui8 * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_ui16, csf_ui16 * );
_CSFC5_ASND1F_PROTO( csfC5asnD1_ui32, csf_ui32 * );

#define csfC5asnD1_authzType csfC5asnD1_ui16
#define csfC5asnD1_crsEType csfC5asnD1_ui16
#define csfC5asnD1_cksType csfC5asnD1_ui16
#define csfC5asnD1_crsKType csfC5asnD1_ui16
#define csfC5asnD1_crsKVNo csfC5asnD1_ui8
#define csfC5asnD1_lReqType csfC5asnD1_i32
#define csfC5asnD1_netHAddrType csfC5asnD1_ui16
#define csfC5asnD1_prnNType csfC5asnD1_ui32
#define csfC5asnD1_msgType csfC5asnD1_ui32
#define csfC5asnD1_pauthType csfC5asnD1_ui32
#define csfC5asnD1_transType csfC5asnD1_ui8
#define csfC5asnD1_pVNo csfC5asnD1_ui32
#define csfC5asnD1_authtVNo csfC5asnD1_ui32
#define csfC5asnD1_tktVNo csfC5asnD1_ui32
#define csfC5asnD1_seqNo csfC5asnD1_ui32
#define csfC5asnD1_nonce csfC5asnD1_i32
#define csfC5asnD1_usTStamp csfC5asnD1_ui32
#define csfC5asnD1_msgErrCode csfC5asnD1_ui32

void csfC5asnd1copy_Realm (
	csfC5_asnD1Ctx *dctx,
	csfC5_prnNC *target,
	csfC5_prnNC *source
);

#endif
