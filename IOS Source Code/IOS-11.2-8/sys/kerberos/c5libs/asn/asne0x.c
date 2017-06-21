/* $Id: asne0x.c,v 3.2 1995/11/17 17:43:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asne0x.c,v $
 *------------------------------------------------------------------
 * asne0x.c - ASN encode driver
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asne0x.c,v $
 * Revision 3.2  1995/11/17  17:43:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:22  shaker
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

#include "asne0.h"

/* EDriver -- Encoding driver. This function wraps the entry to all of the lower
 * level encoding functions: it create the encoding context prior to calling the
 * low-level encoding function, and handles disposition of the data on return
 * from the low-level encoding function.
 */
csf_sts csfC5asnEDriver (
	csfC5_asnE1F *encoder,
	const void *val,
	csfC5_data *data
){
	csf_sts rsts;
	csfC5_asnE1Ctx ectx;

	csfCmemzero( data, sizeof(*data) );

	/* Must have input data. */
	if (val == NULL) {
		return CSFASNX_STS_MISSING_FIELD;
	}

	/* Set up encoding context. */
	csfCmemzero( &ectx, sizeof(ectx) );
	ectx.rsts = CSF_STS_OK;

	/* Create the encoding buffer. */
	rsts = csfASNXcreate_buf( &(ectx.buf) );
	if (rsts) {
		return rsts;
	}

	/* Call the encoding routine. */
	(*encoder) ( &ectx, val );
	rsts = ectx.rsts;
	if (rsts) {
		goto clean_exit;
	}

	/* Take the data from the encoding buffer and put it in caller's data. */
	{
		csfC5_data td;

		rsts = csfASNXexportc_buf( ectx.buf, &td.dp, &td.dl );
		if (!rsts) {
			*data = td;
		}
	}

clean_exit:
	csfASNXdestroy_buf( &(ectx.buf) );
	return rsts;

}

