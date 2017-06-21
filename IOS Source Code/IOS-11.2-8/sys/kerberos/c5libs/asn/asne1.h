/* $Id: asne1.h,v 3.2 1995/11/17 17:43:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asne1.h,v $
 *------------------------------------------------------------------
 * asne1.h - ASN encode definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asne1.h,v $
 * Revision 3.2  1995/11/17  17:43:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_ASNE1_H_
#define _CSFC5_ASNE1_H_ "%W% %G%"
/*.original src/lib/krb5/asn.1/asn1_encode_k.h*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>
#include <csfc5/sts/asn_sts.hs>
#include <csf/asnx.h>


/* CHECKTYPES causes us to compile with encoders invoked and
 * defined with real arguments (instead of "void *"), so we
 * can check to see if arguments really match.  Errors such
 * as "&(unsigned char)" being passed to a function expecting
 * "* long" can be insidious--and undetectable since everything
 * is passed as a "void *".
 *
 * You better do this if you make any mods.
 *
 * If this is on, the resulting code is NOT executable.
 */
#ifndef _CSFC5_ASNE_CHECKTYPES
#define _CSFC5_ASNE_CHECKTYPES 0
#endif

/* E1Ctx -- Encoding context which carries all critical information around.
 */
typedef struct _csfC5_asnE1Ctx {
	csfASNX_buf *buf;
	csf_sts rsts;
	int sum;
	int seqdepth;	/*DEBUG*/
} csfC5_asnE1Ctx;

/* E1F -- Encoder function.
 */
typedef void csfC5_asnE1F ( csfC5_asnE1Ctx *, const void * );

/* E1LF -- Encoder with length function.
 */
typedef void csfC5_asnE1LF ( csfC5_asnE1Ctx *, int, const void * );


/* This is pretty ugly; much room for improvement here. */

#if _CSFC5_ASNE_CHECKTYPES

#define _CSFC5_ASNE1F_PROTO( fn, dt ) \
	void fn( csfC5_asnE1Ctx *ectx, const dt val )

#define _CSFC5_ASNE1F_ENTRY( fn, dt ) \
	void fn( csfC5_asnE1Ctx *ectx, const dt v ) \
	{ \
		const dt val = v;

#define _CSFC5_ASNE1LF_PROTO( fn, dt )\
	void fn( csfC5_asnE1Ctx *ectx, int len, const dt val )

#define _CSFC5_ASNE1LF_ENTRY( fn, dt ) \
	void fn( csfC5_asnE1Ctx *ectx, int len, const dt v ) \
	{ \
		const dt val = v;

#endif

#ifndef _CSFC5_ASNE1F_PROTO
#define _CSFC5_ASNE1F_PROTO( fn, dt ) \
	void fn ( csfC5_asnE1Ctx *, const void * );
#endif

#ifndef _CSFC5_ASNE1F_ENTRY
#define _CSFC5_ASNE1F_ENTRY( fn, dt ) \
	void fn ( csfC5_asnE1Ctx *ectx, const void *v ) \
	{ \
		const dt val = (const dt)v;
#endif

#ifndef _CSFC5_ASNE1LF_PROTO
#define _CSFC5_ASNE1LF_PROTO( fn, dt )\
	void fn ( csfC5_asnE1Ctx *, int, const void * )
#endif

#ifndef _CSFC5_ASNE1LF_ENTRY
#define _CSFC5_ASNE1LF_ENTRY( fn, dt ) \
	void fn ( csfC5_asnE1Ctx *ectx, int len, const void *v ) \
	{ \
		const dt val = (const dt)v;
#endif

_CSFC5_ASNE1F_PROTO( csfC5asnE1_autht, csfC5_autht * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_tkt, csfC5_tkt * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_crsKBlock, csfC5_crsKBlock * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_tktEPart, csfC5_tktEPart * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgKdcRepEPart, csfC5_msgKdcRepEPart * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgAsRep, csfC5_msgKdcRep * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgTgsRep, csfC5_msgKdcRep * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgApReq, csfC5_msgApReq * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgApRep, csfC5_msgApRep * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgApRepEPart, csfC5_msgApRepEPart * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgAsReq, csfC5_msgKdcReq * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgTgsReq, csfC5_msgKdcReq * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgKdcReqBody, csfC5_msgKdcReqBody * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgSafe, csfC5_msgSafe * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgSafeBody, csfC5_msgSafeBody * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgPriv, csfC5_msgPriv * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgPrivEPart, csfC5_msgPrivEPart * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgSPBody, csfC5_msgSPBody * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgCred, csfC5_msgCred * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgCredEPart, csfC5_msgCredEPart * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgErr, csfC5_msgErr * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_prnNC, csfC5_prnNC * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_prnName, csfC5_prnName * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_crsEData, csfC5_crsEData * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_tktFlags, csfC5_tktFlags * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_authz, csfC5_authz * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_authzS, csfC5_authz ** );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgKdcRep, csfC5_msgKdcRep * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1x_msgKdcRepEPart, csfC5_msgKdcRepEPart * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1x_crsKBlock, csfC5_crsKBlock * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_tStamp, csfC5_tStamp * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_cksBlock, csfC5_cksBlock * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_netHAddr, csfC5_netHAddr * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_netHAddrS, csfC5_netHAddr ** );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_trans, csfC5_trans * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_lReqS, csfC5_lReq ** );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_pauthS, csfC5_pauth ** );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_tktS, csfC5_tkt ** );
_CSFC5_ASNE1LF_PROTO( csfC5asnE1_crsETypeS, csfC5_crsEType * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgKdcReq, csfC5_msgKdcReq * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgKdcReqBody, csfC5_msgKdcReqBody * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1x_msgSafe, csfC5_msgSafe * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgCredInfoS, csfC5_msgCredInfo ** );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_msgCredInfo, csfC5_msgCredInfo * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_lReq, csfC5_lReq * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_pauth, csfC5_pauth * );

_CSFC5_ASNE1F_PROTO( csfC5asnE1_cstr, char * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_gstr, char * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_ostr, csf_o * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_asnData, csfC5_data * );

_CSFC5_ASNE1F_PROTO( csfC5asnE1_ui8, csf_ui8 * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_ui16, csf_ui16 * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_ui32, csf_ui32 * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_i8, csf_i8 * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_i16, csf_i16 * );
_CSFC5_ASNE1F_PROTO( csfC5asnE1_i32, csf_i32 * );

#define csfC5asnE1_authzType csfC5asnE1_ui16
#define csfC5asnE1_crsEType csfC5asnE1_ui16
#define csfC5asnE1_cksType csfC5asnE1_ui16
#define csfC5asnE1_crsKType csfC5asnE1_ui16
#define csfC5asnE1_crsKVNo csfC5asnE1_ui8
#define csfC5asnE1_lReqType csfC5asnE1_i32
#define csfC5asnE1_netHAddrType csfC5asnE1_ui16
#define csfC5asnE1_prnNType csfC5asnE1_ui32
#define csfC5asnE1_msgType csfC5asnE1_ui32
#define csfC5asnE1_pauthType csfC5asnE1_ui32
#define csfC5asnE1_transType csfC5asnE1_ui8
#define csfC5asnE1_pVNo csfC5asnE1_ui32
#define csfC5asnE1_authtVNo csfC5asnE1_ui32
#define csfC5asnE1_tktVNo csfC5asnE1_ui32
#define csfC5asnE1_seqNo csfC5asnE1_ui32
#define csfC5asnE1_nonce csfC5asnE1_i32
#define csfC5asnE1_usTStamp csfC5asnE1_ui32
#define csfC5asnE1_msgErrCode csfC5asnE1_ui32


#if _CSFC5_ASNE_CHECKTYPES

#define _csfC5asnE1Add( value, encoder ) \
	encoder( ectx, value )

#define _csfC5asnE1AddL( len, value, encoder ) \
	encoder( ectx, len, value )

#define _csfC5asnE1AddF( value, tag, encoder ) \
	encoder( ectx, value )

#define _csfC5asnE1AddLF( len, value, tag, encoder ) \
	encoder( ectx, len, value )

/* We'll assume these are correct.. no easy way to do this. */
#define _csfC5asnE1AddCI( value, tag, encoder )

#endif

/* _csfC5asnE1Add -- add encoded component.
 */
#ifndef _csfC5asnE1Add
#define _csfC5asnE1Add( value, encoder ) \
	encoder( ectx, (void *)value )
#endif

/* _csfC5asnE1AddL -- add length-qualified encoded component.
 */
#ifndef _csfC5asnE1AddL
#define _csfC5asnE1AddL( len, value, encoder ) \
	encoder( ectx, len, (void *)value )
#endif

/* _csfC5asnE1AddF -- add a field to the encoding
 */
#ifndef _csfC5asnE1AddF
#define _csfC5asnE1AddF( value, tag, encoder ) \
	csfC5asnE1AddF( ectx, tag, (void *)(value), encoder )
#endif

void csfC5asnE1AddF (
	csfC5_asnE1Ctx *ectx,
	int tag,
	void *value,
	csfC5_asnE1F *encoder
);

/* _csfC5asnE1AddLF -- add a length-qualitifed field to the encoding
 */
#ifndef _csfC5asnE1AddLF
#define _csfC5asnE1AddLF( len, value, tag, encoder ) \
	csfC5asnE1AddLF( ectx, tag, (void *)(value), len, encoder )
#endif

void csfC5asnE1AddLF (
	csfC5_asnE1Ctx *ectx,
	int tag,
	void *value,
	int len,
	csfC5_asnE1LF *encoder
);

/* _csfC5asnE1AddCI -- add constant integer field.  Necessary because
 * functions require address-of, and can't do that for a constant (otherwise
 * this wouldn't be necessary.
 */
#ifndef _csfC5asnE1AddCI
#define _csfC5asnE1AddCI( value, tag, encoder ) \
	csfC5asnE1AddCI( ectx, tag, value, encoder )
#endif

void csfC5asnE1AddCI (
	csfC5_asnE1Ctx *ectx,
	int tag,
	int value,
	csfC5_asnE1F *encoder
);

/* Form a sequence (by adding a sequence header to the current encoding.
 * Ends scope and sums in saved length.
 */
#ifndef _csfC5asnE1StartSeq
#define _csfC5asnE1StartSeq() \
		csfC5asnE1StartSeq( ectx, savesum ); \
	}
#endif
void csfC5asnE1StartSeq( csfC5_asnE1Ctx *ectx, int savesum );

/* End a sequence: start a new scope for length.
 * Saves current encoded length and sets it to zero in
 * the context.
 */
#ifndef _csfC5asnE1EndSeq
#define _csfC5asnE1EndSeq() \
	{ \
		int savesum = csfC5asnE1EndSeq( ectx );
#endif
int csfC5asnE1EndSeq( csfC5_asnE1Ctx *ectx );

#ifndef _csfC5asnE1ETag
#define _csfC5asnE1ETag( n, t ) \
	csfC5asnE1ETag( ectx, n, t )
#endif
void csfC5asnE1ETag( csfC5_asnE1Ctx *ectx, int n, int t );

#ifndef _csfC5asnE1AppTag
#define _csfC5asnE1AppTag( n ) \
	csfC5asnE1AppTag( ectx, n )
#endif
void csfC5asnE1AppTag( csfC5_asnE1Ctx *ectx, int n );

#endif
