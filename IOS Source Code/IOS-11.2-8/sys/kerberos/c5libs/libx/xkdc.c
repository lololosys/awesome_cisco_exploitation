/* $Id: xkdc.c,v 3.2 1995/11/17 17:45:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libx/xkdc.c,v $
 *------------------------------------------------------------------
 * xkdc.c - Deal with KDC reply
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xkdc.c,v $
 * Revision 3.2  1995/11/17  17:45:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:06  shaker
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

csf_sts csfC5moveKdcRepTo_creds (
	csfC5_msgKdcRep *kdcrep,
	csfC5_creds *creds
){

	csfC5clear_creds( creds );

	creds->client = kdcrep->client;
	csfC5clear_prnId( &kdcrep->client );

	creds->server = kdcrep->epart.server;
	csfC5clear_prnId( &kdcrep->epart.server );

	creds->times = kdcrep->epart.times;
	creds->is_skey = FALSE;
	creds->tktflags = kdcrep->epart.flags;

	creds->seskey = kdcrep->epart.seskey;
	csfC5clear_crsKBlock( &kdcrep->epart.seskey );

	creds->haddrs = kdcrep->epart.caddrs;
	kdcrep->epart.caddrs = NULL;

	creds->e_tkt = kdcrep->e_tkt;
	csfC5clear_data( &kdcrep->e_tkt );

	return CSF_STS_OK;
}


csf_sts csfC5make_msgKdcReqBody (
	csfC5_credsReqCtx *creq,
	csfC5_data *req
){
	csf_sts rsts;
	const csfC5_creds *reqc;
	csfC5_msgKdcReqBody reqbody;
	csfC5_crsEType elist[1];
	csfC5_data e_req;

	csfC5clear_msgKdcReqBody( &reqbody );
	csfC5clear_data( &e_req );

	/* This is simply to eliminate additional dereferences. */
	reqc = creq->req;

	reqbody.kdc_options = reqc->tktflags;
	reqbody.client = *(creq->client);
	reqbody.server = *(creq->server);

	reqbody.tstart = reqc->times.tstart;
	reqbody.tend = reqc->times.tend;
	reqbody.trenew = reqc->times.trenew;

	elist[0] = creq->etype;
	reqbody.etypes = elist;
	reqbody.netypes = 1;

	reqbody.haddrs = reqc->haddrs;

	if (reqc->authz) {
		csfC5_data e_authz;

		rsts = csfC5asnE_authzS(
			(const csfC5_authz **) reqc->authz, /*TBD/JNK--cast*/
			&e_authz
		);
		if (rsts) {
			goto clean_exit;
		}
		rsts = csfC5crsEncryptData(
			&e_authz,
			creq->key,
			creq->etype,
			NULL,
			&reqbody.ec_authz.ciphertext
		);
		if (rsts) {
			csfC5free_data( &e_authz );
			goto clean_exit;
		}
	}

	reqbody.e_tkt2 = reqc->e_tkt2;

	if (creq->nonce == 0) {
		csfC5_tStamp tnonce;

		rsts = csfC5getNow_tStamp( &tnonce );
		if (rsts) {
			goto clean_exit;
		}
		creq->nonce = (csfC5_nonce) tnonce;
	}
	reqbody.nonce = creq->nonce;

	rsts = csfC5asnE_msgKdcReqBody( &reqbody, &e_req );

clean_exit:

	reqbody.etypes = NULL;
	reqbody.haddrs = NULL;

	csfC5clear_prnId( &reqbody.client );
	csfC5clear_prnId( &reqbody.server );
	csfC5clear_data( &reqbody.e_tkt2 );

	csfC5free_msgKdcReqBody( &reqbody );
	if (rsts) {
		csfC5free_data( &e_req );
	}
	*req = e_req;

	return rsts;
}


csf_sts csfC5check_msgKdcRep (
	csfC5_credsReqCtx *creq,
	const csfC5_msgKdcRep *kdcrep
){
	csfC5_tStamp tstart;

	/*TBD/JNK--is this really necessary? */
	tstart = kdcrep->epart.times.tstart;
	if (tstart == 0) {
		tstart = kdcrep->epart.times.tauth;
	}

	if (!csfC5isEq_prnId( &kdcrep->client, creq->client )) {
		return CSFC5_STS_KDCREP_MODIFIED;
	}
	if (!csfC5isEq_prnId( &kdcrep->epart.server, creq->server )) {
		return CSFC5_STS_KDCREP_MODIFIED;
	}
	if (creq->nonce != kdcrep->epart.nonce) {
		return CSFC5_STS_KDCREP_MODIFIED;
	}
	if (
		creq->req->times.tstart &&
		(creq->req->times.tstart != tstart)
	) {
		return CSFC5_STS_KDCREP_MODIFIED;
	}
	if (
		creq->req->times.tend &&
		(kdcrep->epart.times.tend > creq->req->times.tend)
	) {
		return CSFC5_STS_KDCREP_MODIFIED;
	}
	if (
		(creq->req->tktflags & CSFC5c_KDCOPT_RENEWABLE_m) &&
		creq->req->times.trenew &&
			(kdcrep->epart.times.trenew > creq->req->times.trenew)
	) {
		return CSFC5_STS_KDCREP_MODIFIED;
	}
	if (
		(creq->req->tktflags & CSFC5c_KDCOPT_RENEWABLE_OK_m) &&
		(kdcrep->epart.flags & CSFC5c_KDCOPT_RENEWABLE_m) &&
		creq->req->times.tend &&
		(kdcrep->epart.times.trenew > creq->req->times.tend)
	) {
		return CSFC5_STS_KDCREP_MODIFIED;
	}

	return CSF_STS_OK;
}

