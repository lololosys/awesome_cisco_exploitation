/* $Id: asnd1x.c,v 3.2 1995/11/17 17:43:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asnd1x.c,v $
 *------------------------------------------------------------------
 * asnd1x.c - ASN misc. decode functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnd1x.c,v $
 * Revision 3.2  1995/11/17  17:43:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original src/lib/krb5/asn.1/asn1_misc.c*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1994 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include "asnd1.h"

void csfC5asnd1copy_Realm (
	csfC5_asnD1Ctx *dctx,
	csfC5_prnNC *target,
	csfC5_prnNC *source
){
	if (dctx->dsts) {
		return;
	}
	dctx->dsts = csfC5copy_prnNC( source, target );
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_tStamp, csfC5_tStamp * )
	csf_time_t val1;

	if (dctx->dsts) {
		return;
	}

	dctx->dsts = csfASNXdGTime( &dctx->sctx->buf, &val1 );
	if (!dctx->dsts) {
		*val = val1;
	}
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_oStr, csfC5_data * )
	if (dctx->dsts) {
		return;
	}
	dctx->dsts = csfASNXdOStr( &dctx->sctx->buf, (int *)(&val->dl), (csf_o **)(&val->dp) );
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_cStr, csfC5_data * )
	if (dctx->dsts) {
		return;
	}
	dctx->dsts = csfASNXdCStr( &dctx->sctx->buf, (int *)(&val->dl), (char **)(&val->dp) );
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_gStr, csfC5_data * )
	if (dctx->dsts) {
		return;
	}
	dctx->dsts = csfASNXdGStr( &dctx->sctx->buf, (int *)(&val->dl), (char **)(&val->dp) );
}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_asnData, csfC5_data * )
	csf_size_t len;

	if (dctx->dsts) {
		return;
	}

	len = dctx->len;
	dctx->dsts = csfASNXgOStr( &dctx->sctx->buf, len, (csf_o **)(&val->dp) );
	if (dctx->dsts) {
		return;
	}
	val->dl = len;

}

_CSFC5_ASND1F_ENTRY( csfC5asnD1_tktFlags, csfC5_tktFlags * )

	if (dctx->dsts) {
		return;
	}

	dctx->dsts = csfASNXdTag( &dctx->sctx->buf, &dctx->cl, &dctx->cs, &dctx->tn, &dctx->len );
	if (dctx->dsts) {
		return;
	}
	if (
		dctx->cl != csfASNX_clUNIV ||
		dctx->cs != csfASNX_csPRIM ||
		dctx->tn != csfASNX_tagBITSTR
	) {
		dctx->dsts = CSFASNX_STS_BAD_ID;
		return;
	}
	if (dctx->len != 5) {
		dctx->dsts = CSFASNX_STS_BAD_LENGTH;
		return;
	}

	{
		csf_o o;
		int i;
		csfC5_tktFlags f = 0;

		dctx->dsts = csfASNXgOctet( &dctx->sctx->buf, &o ); /* # of padding bits */
		if (dctx->dsts) {
			return;	/* should be 0 */
		}
		if (o != 0) {
			dctx->dsts = CSFASNX_STS_BAD_FORMAT;
			return;
		}

		for (i = 0; i < 4; i++) {
			dctx->dsts = csfASNXgOctet( &dctx->sctx->buf, &o );
			if (dctx->dsts) {
				return;
			}
			f = (f<<8) | ((csfC5_tktFlags)o & 0xFF);
		}

		*val = f;
	}

}

#define integer_convert(fname,ktype)\
_CSFC5_ASND1F_ENTRY( fname, ktype * ) \
	long n; \
 \
	if (dctx->dsts) { \
		return; \
	} \
	dctx->dsts = csfASNXdInt( &dctx->sctx->buf, &n ); \
	if (dctx->dsts) { \
	return; \
	} \
	*val = (ktype) n;\
}

#define unsigned_integer_convert( fname, ktype ) \
_CSFC5_ASND1F_ENTRY( fname, ktype * ) \
	unsigned long n; \
 \
	if (dctx->dsts) { \
		return; \
	} \
	dctx->dsts = csfASNXdUInt( &dctx->sctx->buf ,&n ); \
	if (dctx->dsts) { \
		return; \
	} \
	*val = (ktype) n; \
}

integer_convert(csfC5asnD1_i8,csf_i8)
integer_convert(csfC5asnD1_i16,csf_i16)
integer_convert(csfC5asnD1_i32,csf_i32)
unsigned_integer_convert(csfC5asnD1_ui8,csf_ui8)
unsigned_integer_convert(csfC5asnD1_ui16,csf_ui16)
unsigned_integer_convert(csfC5asnD1_ui32,csf_ui32)


void csfC5asnD1Tag0 (
	csfC5_asnD1Ctx *dctx,
	int tn
){

	if (dctx->dsts) {
		return;
	}

	if (tn != dctx->tn) {
		dctx->dsts = CSFASNX_STS_BAD_ID;
	}
}

csf_boolean csfC5asnD1TagOpt0 (
	csfC5_asnD1Ctx *dctx,
	int tn
){

	if (dctx->dsts) {
		return FALSE;
	}

	if (tn != dctx->tn) {
		return FALSE;
	}

	return TRUE;
}

void csfC5asnD1NextTag0 (
	csfC5_asnD1Ctx *dctx
){

	if (dctx->dsts) {
		return;
	}

	dctx->dsts = csfASNXdTag( &dctx->sctx->buf, &dctx->cl, &dctx->cs, &dctx->tn, &dctx->len );
	if (dctx->dsts) {
		return;
	}
	if (dctx->cl != csfASNX_clCTX || dctx->cs != csfASNX_csCONS) {
		dctx->dsts = CSFASNX_STS_BAD_ID;
	}
	return;
}

void csfC5asnD1AppTag0 (
	csfC5_asnD1Ctx *dctx,
	csfASNX_tag tagexpect
){

	if (dctx->dsts) {
		return;
	}

	dctx->dsts = csfASNXdTag( &dctx->sctx->buf, &dctx->cl, &dctx->cs, &dctx->tn, &dctx->len );
	if (dctx->dsts) {
		return;
	}

	if (
		dctx->cl != csfASNX_clAPP || dctx->cs != csfASNX_csCONS || dctx->tn != tagexpect
	) {
		dctx->dsts = CSFASNX_STS_BAD_ID;
	}

}

int csfC5asnD1HasData0 (
	csfC5_asnD1Ctx *dctx
) {
	if (dctx->dsts) {
		return 0;
	}
	return (csfASNXremains_buf( &dctx->sctx->buf ) > 0);
}

void csfC5asnD1AllocF0 (
	csfC5_asnD1Ctx *dctx,
	csf_size_t n,
	void **p
) {

	if (dctx->dsts) {
		return;
	}

	*p = csfCcmalloc( n );
	if (*p == NULL) {
		dctx->dsts = CSF_STS_ENOMEM;
	}

}

void csfC5asnD1SeqStart0 (
	csfC5_asnD1Ctx *dctx,
	csfC5_asnD1SeqCtx *sctx0
) {

	if (dctx->dsts) {
		return;
	}

	dctx->dsts = csfASNXdSeq( &dctx->sctx->buf, &dctx->len );
	if (dctx->dsts) {
		return;
	}

	
	dctx->dsts = csfASNXembed_buf( &sctx0->buf, &dctx->sctx->buf, dctx->len );
	if (dctx->dsts) {
		return;
	}

	sctx0->sctx1 = dctx->sctx;
	dctx->sctx = sctx0;

}

void csfC5asnD1SeqEnd0 (
	csfC5_asnD1Ctx *dctx,
	csfC5_asnD1SeqCtx *sctx0
) {

	if (dctx->dsts) {
		return;
	}

	dctx->sctx = sctx0->sctx1;
	csfASNXsync_buf( &dctx->sctx->buf, &sctx0->buf );

}

void csfC5asnD1F0 (
	csfC5_asnD1Ctx *dctx,
	void *val,
	csfC5_asnD1F *decoder
) {

	if (dctx->dsts) {
		return;
	}

	decoder( dctx, val );

}

void csfC5asnD1FT0 (
	csfC5_asnD1Ctx *dctx,
	void *val,
	int tag,
	csfC5_asnD1F *decoder
) {

	if (dctx->dsts) {
		return;
	}

	if (tag != dctx->tn) {
		dctx->dsts =  CSFASNX_STS_BAD_ID;
		return;
	}

	decoder( dctx, val );
	if (dctx->dsts) {
		return;
	}

	csfC5asnD1NextTag0( dctx );

}

void csfC5asnD1FTOpt0 (
	csfC5_asnD1Ctx *dctx,
	void *val,
	int tag,
	csfC5_asnD1F *decoder
) {

	if (dctx->dsts) {
		return;
	}

	if (tag != dctx->tn) {
		return;
	}

	decoder( dctx, val );
	if (dctx->dsts) {
		return;
	}

	csfC5asnD1NextTag0( dctx );

}

void csfC5asnD1A0 (
	csfC5_asnD1Ctx *dctx,
	csf_size_t *len,
	void *val,
	csf_size_t dts,
	csfC5_asnD1F *decoder
) {
	void *a = NULL;
	int nelem = 0;

	if (dctx->dsts) {
		return;
	}

	_csfC5asnD1SeqStart();
	{
		while (_csfC5asnD1HasData()) {
			void *elem;

			a = (void *) /*TBD/JNK--don't realloc in place*/
				csfCrealloc( a, (nelem + 1) * dts );
			if (a == NULL) {
				dctx->dsts = CSF_STS_ENOMEM;
				break;
			}

			elem = (void *) ((char *)a + (dts * nelem));
			csfCmemzero( elem, dts );

			decoder( dctx, elem );
			if (dctx->dsts) {
				break;
			}
			nelem++;
		}
	}
	_csfC5asnD1SeqEnd();

	*((void **)val) = a;
	*len = nelem;

}

void csfC5asnD1S0 (
	csfC5_asnD1Ctx *dctx,
	void *val,
	csf_size_t dts,
	csfC5_asnD1F *decoder
) {
	void **a = NULL;
	int nelem = 0;

	if (dctx->dsts) {
		return;
	}

	_csfC5asnD1SeqStart();
	{
		while (_csfC5asnD1HasData()) {

			nelem++;

			a = (void **) /*TBD/JNK--don't realloc in place*/
				csfCrealloc( a, (nelem + 1) * sizeof(void *) );
			if (a == NULL) {
				dctx->dsts = CSF_STS_ENOMEM;
				break;
			}

			a[nelem-1] = (void *) csfCcmalloc( dts );
			if (!a[nelem-1]) {
				dctx->dsts = CSF_STS_ENOMEM;
				break;
			}

			decoder( dctx, a[nelem-1] );
			if (dctx->dsts) {
				break;
			}
		}

	}
	_csfC5asnD1SeqEnd();

	if (a) {
		a[nelem] = NULL;
	}
	*((void **)val) = (void *)a;

}
