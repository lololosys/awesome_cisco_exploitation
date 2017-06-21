/* $Id: asnd0x.c,v 3.2 1995/11/17 17:43:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asnd0x.c,v $
 *------------------------------------------------------------------
 * asnd0x.c - ASN decode driver
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnd0x.c,v $
 * Revision 3.2  1995/11/17  17:43:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#include "asnd0.h"

csf_sts csfC5asnDDriver (
	const csfC5_data *in,
	void *out,
	csf_size_t dts,
	csfC5_asnD1F *decoder
){
	csfC5_asnD1Ctx dctx0;
	csfC5_asnD1SeqCtx sctx0;

	csfCmemzero( out, dts );

	csfCmemzero( &dctx0, sizeof(dctx0) );
	csfCmemzero( &sctx0, sizeof(sctx0) );
	dctx0.sctx = &sctx0;

	dctx0.dsts = csfASNXimportp_buf( &sctx0.buf, in->dp, in->dl );
	if (dctx0.dsts) {
		return dctx0.dsts;
	}

	decoder( &dctx0, out );

	return dctx0.dsts;

}

