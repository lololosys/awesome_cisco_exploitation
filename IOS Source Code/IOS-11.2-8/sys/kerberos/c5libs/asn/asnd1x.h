/* $Id: asnd1x.h,v 3.2 1995/11/17 17:43:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asnd1x.h,v $
 *------------------------------------------------------------------
 * asnd1x.h - ASN decode definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnd1x.h,v $
 * Revision 3.2  1995/11/17  17:43:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_ASND1X_H_
#define _CSFC5_ASND1X_H_ "%W% %G%"
/*.original src/lib/krb5/asn.1/asn1_decode_k.h*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

typedef struct _csfC5_asnD1SeqCtx {
	csfASNX_buf buf;
	struct _csfC5_asnD1SeqCtx *sctx1;
} csfC5_asnD1SeqCtx;

typedef struct _csfC5_asnD1Ctx {
	csf_sts dsts;
	csfASNX_cl cl;
	csfASNX_cs cs;
	csfASNX_tag tn;
	int len;
	csfC5_asnD1SeqCtx *sctx;
} csfC5_asnD1Ctx;

/* D1F -- Decoder function.
 */

typedef void csfC5_asnD1F ( csfC5_asnD1Ctx *, void * );

#define _csfC5asnD1Setup()

#define _csfC5asnD1Cleanup()

int csfC5asnD1HasData0 ( csfC5_asnD1Ctx *dctx );
#define _csfC5asnD1HasData() \
	csfC5asnD1HasData0( dctx )

void csfC5asnD1AllocF0 ( csfC5_asnD1Ctx *dctx, csf_size_t n, void **p );
#define _csfC5asnD1AllocF( v, dt ) \
	csfC5asnD1AllocF0( dctx, sizeof(dt), (void **)(v) )

void csfC5asnD1Tag0 ( csfC5_asnD1Ctx *dctx, int t );
#define _csfC5asnD1Tag( t, f ) \
	csfC5asnD1Tag0( dctx, t ); \
	{ \
		f; \
		_csfC5asnD1NextTag(); \
	}

csf_boolean csfC5asnD1TagOpt0 ( csfC5_asnD1Ctx *dctx, int t );
#define _csfC5asnD1TagOpt( t, f ) \
	if (csfC5asnD1TagOpt0( dctx, t )) { \
		f; \
		_csfC5asnD1NextTag(); \
	}

void csfC5asnD1NextTag0 ( csfC5_asnD1Ctx *dctx );
#define _csfC5asnD1NextTag() \
	csfC5asnD1NextTag0( dctx )

void csfC5asnD1AppTag0 ( csfC5_asnD1Ctx *dctx, csfASNX_tag t );
#define _csfC5asnD1AppTag( t ) \
	csfC5asnD1AppTag0( dctx, t )

void csfC5asnD1SeqStart0 ( csfC5_asnD1Ctx *dctx, csfC5_asnD1SeqCtx *sctx0 );
#define _csfC5asnD1SeqStart() \
	{ \
		csfC5_asnD1SeqCtx sctx0; \
		csfC5asnD1SeqStart0( dctx, &sctx0 ); \

void csfC5asnD1SeqEnd0 ( csfC5_asnD1Ctx *dctx, csfC5_asnD1SeqCtx *sctx0 );
#define _csfC5asnD1SeqEnd() \
		csfC5asnD1SeqEnd0( dctx, &sctx0 ); \
	}

#define _csfC5asnD1BeginStruct() \
	_csfC5asnD1SeqStart(); \
	_csfC5asnD1NextTag();

#define _csfC5asnD1EndStruct() \
	_csfC5asnD1SeqEnd()

#if _CSFC5_ASND_CHECKTYPES

#define _csfC5asnD1F( v, d ) \
	d( dctx, (v) )

#define _csfC5asnD1FT( v, t, d ) \
	d( dctx, (v) )

#define _csfC5asnD1FTOpt( v, t, d ) \
	d( dctx, (v) )

#define _csfC5asnD1A( l, v, dt, d ) \
	d( dctx, (dt *)(v) )

#define _csfC5asnD1S( v, dt, d ) \
	d( dctx, (dt *)(v) )

#else

void csfC5asnD1F0( csfC5_asnD1Ctx *dctx, void *v, csfC5_asnD1F *d );
#define _csfC5asnD1F( v, d ) \
	csfC5asnD1F0( dctx, (void *)(v), d )

void csfC5asnD1FT0( csfC5_asnD1Ctx *dctx, void *v, int t, csfC5_asnD1F *d );
#define _csfC5asnD1FT( v, t, d ) \
	csfC5asnD1FT0( dctx, (void *)(v), t, d )

void csfC5asnD1FTOpt0( csfC5_asnD1Ctx *dctx, void *v, int t, csfC5_asnD1F *d );
#define _csfC5asnD1FTOpt( v, t, d ) \
	csfC5asnD1FTOpt0( dctx, (void *)(v), t, d )

void csfC5asnD1A0( csfC5_asnD1Ctx *dctx, csf_size_t *l, void *v, csf_size_t dts, csfC5_asnD1F *d );
#define _csfC5asnD1A( l, v, dt, d ) \
	csfC5asnD1A0( dctx, l, (void *)(v), sizeof(dt), d )

void csfC5asnD1S0( csfC5_asnD1Ctx *dctx, void *v, csf_size_t dts, csfC5_asnD1F *d );
#define _csfC5asnD1S( v, dt, d ) \
	csfC5asnD1S0( dctx, (void *)(v), sizeof(dt), d )

#endif

#endif
