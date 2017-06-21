/* $Id: xkdctgs.c,v 3.2 1995/11/17 17:45:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libx/xkdctgs.c,v $
 *------------------------------------------------------------------
 * xkdctgs.c - Generate TGS message request
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xkdctgs.c,v $
 * Revision 3.2  1995/11/17  17:45:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

csf_sts csfC5make_msgTgsReq (
	csfC5_credsReqCtx *creq,
	csfC5_data *req
){
	csf_sts rsts;
	csfC5_msgKdcReq tgsreq;
	csfC5_cksBlock tgsreq_cks;
	csfC5_pauth padata;
	csfC5_pauth *palist[2];
	csfC5_data e_req;

	csfC5clear_msgKdcReq( &tgsreq );
	csfC5clear_cksBlock( &tgsreq_cks );
	csfC5clear_pauth( &padata );
	csfC5clear_data( &e_req );

	rsts = csfC5setup_credsReqCtx( creq );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5make_msgKdcReqBody( creq, &tgsreq.e_body );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5cksSumData( &tgsreq.e_body, creq->ckstype,
		&creq->key->key_data, &tgsreq_cks );
	csfC5free_data( &e_req );
	if (rsts) {
		goto clean_exit;
	}

	/* Attach ap_req to the tgsreq.
	 */
	rsts = csfC5make_msgApReq(
				0, /* no ap options */
				&tgsreq_cks,
				creq->client,
				creq->key,
				&creq->use->e_tkt,
				0, /* no initial sequence */
				NULL, /* no new key */
				NULL, /* don't need authenticator */
				&padata.pa_data
			);
	if (rsts) {
		goto clean_exit;
	}
	padata.pa_type = CSFC5c_PADATA_T_TGS_REQ;
	palist[0] = &padata;
	palist[1] = NULL;
	tgsreq.padata = palist;

	/* the TGS_REQ is assembled in tgsreq, so encode it */
	rsts = csfC5asnE_msgTgsReq( &tgsreq, &e_req );
	if (rsts) {
		goto clean_exit;
	}

clean_exit:

	csfC5free_cksBlock( &tgsreq_cks );

	csfC5free_pauth( &padata );
	tgsreq.padata = NULL;

	csfC5free_msgKdcReq( &tgsreq );
	if (rsts) {
		csfC5free_data( &e_req );
	}
	*req = e_req;

	return rsts;
}
