/* $Id: asne1x.c,v 3.2 1995/11/17 17:43:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asne1x.c,v $
 *------------------------------------------------------------------
 * asne1x.c - ASN encode functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asne1x.c,v $
 * Revision 3.2  1995/11/17  17:43:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original src/lib/krb5/asn.1/asn1_encode_k.c*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include "asne1.h"

/*
 * Wrappers for ASN1 primitives.
 */

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_tktFlags, csfC5_tktFlags * )
	csfC5_tktFlags valcopy = *val;
	int rlen;
	int i;

	for (i = 0; i < 4; i++) {
		ectx->rsts = csfASNXpOctet( ectx->buf, valcopy & 0xFF );
		if (ectx->rsts) {
			return;
		}
		valcopy >>= 8;
	}
	ectx->rsts = csfASNXpOctet( ectx->buf, 0 ); /* 0 padding bits */
	if (ectx->rsts) {
		return;
	}
	rlen = 5;	/* Yuck--number of octets inserted above. */
	ectx->sum += rlen;

	ectx->rsts = csfASNXeTag( ectx->buf, csfASNX_clUNIV,
		csfASNX_csPRIM, csfASNX_tagBITSTR, rlen, &rlen );
	if (ectx->rsts) {
		return;
	}

	ectx->sum += rlen;
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_tStamp, csfC5_tStamp * )
	int rlen;

	if (ectx->rsts) {
		return;
	}
	ectx->rsts = csfASNXeGTime( ectx->buf, *val, &rlen );
	if (!ectx->rsts) {
		ectx->sum += rlen;
	}

}

/* uint -- Common function used by all of the csfC5asnE1_uiXX encoders. */
static void csfC5asnE1_uint (
	csfC5_asnE1Ctx *ectx,
	csf_i32 val
){
	int rlen;

	if (ectx->rsts) {
		return;
	}

	ectx->rsts = csfASNXeUInt( ectx->buf, val, &rlen );
	if (!ectx->rsts) {
		ectx->sum += rlen;
	}

}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_ui8, csf_ui8 * )
	csfC5asnE1_uint( ectx, (csf_ui32)(*val) );
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_ui16, csf_ui16 * )
	csfC5asnE1_uint( ectx, (csf_ui32)(*val) );
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_ui32, csf_ui32 * )
	csfC5asnE1_uint( ectx, *val );
}


/* int -- Common function used by all of the csfC5asnE1_iXX encoders. */
static void csfC5asnE1_int (
	csfC5_asnE1Ctx *ectx,
	csf_i32 val
){
	int rlen;

	if (ectx->rsts) {
		return;
	}

	ectx->rsts = csfASNXeInt( ectx->buf, val, &rlen );
	if (!ectx->rsts) {
		ectx->sum += rlen;
	}

}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_i8, csf_i8 * )
	csfC5asnE1_int( ectx, (csf_i32)(*val) );
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_i16, csf_i16 * )
	csfC5asnE1_int( ectx, (csf_i32)(*val) );
}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_i32, csf_i32 * )
	csfC5asnE1_int( ectx, *val );
}


_CSFC5_ASNE1F_ENTRY( csfC5asnE1_cstr, csfC5_data * )
	int rlen;

	if (ectx->rsts) {
		return;
	}
	ectx->rsts = csfASNXeCStr( ectx->buf, val->dl, (char *)(val->dp), &rlen );
	if (!ectx->rsts) {
		ectx->sum += rlen;
	}

}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_gstr,  csfC5_data * )
	int rlen;

	if (ectx->rsts) {
		return;
	}
	ectx->rsts = csfASNXeGStr( ectx->buf, val->dl, (char *)(val->dp), &rlen );
	if (!ectx->rsts) {
		ectx->sum += rlen;
	}

}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_ostr,  csfC5_data * )
	int rlen;

	if (ectx->rsts) {
		return;
	}
	ectx->rsts = csfASNXeOStr( ectx->buf, val->dl, (csf_o *)(val->dp), &rlen );
	if (!ectx->rsts) {
		ectx->sum += rlen;
	}

}

_CSFC5_ASNE1F_ENTRY( csfC5asnE1_asnData, csfC5_data * )

	if (ectx->rsts) {
		return;
	}
	ectx->rsts = csfASNXpOStr( ectx->buf, val->dl, (csf_o *)(val->dp) );
	if (!ectx->rsts) {
		ectx->sum += val->dl;
	}

}

/*
 * Internal support functions. These are all invoked via macros.
 */

void csfC5asnE1AddF (
	csfC5_asnE1Ctx *ectx,
	int tag,
	void *value,
	csfC5_asnE1F *encoder
){

	if (!ectx->rsts) {
		int savelen = ectx->sum;
		ectx->sum = 0;
		(void) (*encoder)( ectx, value );
		if (!ectx->rsts) {
			int rlen = ectx->sum;
			ectx->rsts = csfASNXeETag( ectx->buf, csfASNX_clCTX, tag,
				rlen, &rlen );
			if (!ectx->rsts) {
				ectx->sum += rlen;
			}
		}
		ectx->sum += savelen;
	}

}

void csfC5asnE1AddLF (
	csfC5_asnE1Ctx *ectx,
	int tag,
	void *value,
	int len,
	csfC5_asnE1LF *encoder
){

	if (!ectx->rsts) {
		int savelen = ectx->sum;
		ectx->sum = 0;
		(void) (*encoder)( ectx, len, value );
		if (!ectx->rsts) {
			int rlen = ectx->sum;
			ectx->rsts = csfASNXeETag( ectx->buf, csfASNX_clCTX, tag,
				rlen, &rlen );
			if (!ectx->rsts) {
				ectx->sum += rlen;
			}
		}
		ectx->sum += savelen;
	}

}

void csfC5asnE1AddCI (
	csfC5_asnE1Ctx *ectx,
	int tag,
	int val,
	csfC5_asnE1F *encoder
){
	csfC5asnE1AddF( ectx, tag, (void *)&val, encoder );
}

void csfC5asnE1StartSeq (
	csfC5_asnE1Ctx *ectx,
	int savesum
){
	int rlen;

	ectx->seqdepth--;	/*DEBUG*/

	if (ectx->rsts) {
		ectx->sum = savesum;
		return;
	}

	ectx->rsts = csfASNXeSeq( ectx->buf, ectx->sum, &rlen );
	if (ectx->rsts) {
		ectx->sum = savesum;
		return;
	}

	ectx->sum += savesum + rlen;

}

int csfC5asnE1EndSeq (
	csfC5_asnE1Ctx *ectx
){
	int rsum;

	ectx->seqdepth++;

	rsum = ectx->sum;
	ectx->sum = 0;

	return rsum;
}

void csfC5asnE1AppTag (
	csfC5_asnE1Ctx *ectx,
	int num
){
	int rlen;

	if (ectx->rsts) {
		return;
	}

	ectx->rsts = csfASNXeETag( ectx->buf, csfASNX_clAPP,
		num, ectx->sum, &rlen );
	if (!ectx->rsts) {
		ectx->sum += rlen;
	}

}


void csfC5asnE1ETag (
	csfC5_asnE1Ctx *ectx,
	int n,
	int t
){
	int rlen;

	if (ectx->rsts) {
		return;
	}

	ectx->rsts = csfASNXeETag( ectx->buf, t, n, ectx->sum, &rlen );
	if (!ectx->rsts) {
		ectx->sum += rlen;
	}
		
}
