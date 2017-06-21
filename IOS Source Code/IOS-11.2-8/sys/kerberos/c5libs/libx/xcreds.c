/* $Id: xcreds.c,v 3.2 1995/11/17 17:45:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libx/xcreds.c,v $
 *------------------------------------------------------------------
 * xcreds.c - setup credential request
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xcreds.c,v $
 * Revision 3.2  1995/11/17  17:45:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:04  shaker
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

csf_sts csfC5setup_credsReqCtx (
	csfC5_credsReqCtx *creq
){
	csf_sts rsts = CSF_STS_OK;
	const csfC5_creds *req;
	const csfC5_creds *use;

	req = creq->req;
	use = creq->use;

	if (creq->client == NULL) {
		creq->client = &req->client;
	}
	if (csfC5isNull_prnId( creq->client ) && (use != NULL)) {
		creq->client = &use->client;
	}

	if (creq->server == NULL) {
		creq->server = &req->server;
	}
	if (csfC5isNull_prnId( creq->server ) && (use != NULL)) {
		creq->server = &use->client;
	}

	if (creq->key == NULL) {
		creq->key = &req->seskey;
	}
	if (csfC5isNull_data( &creq->key->key_data ) && (use != NULL)) {
		creq->key = &use->seskey;
	}

	if (creq->etype == 0 && (creq->key != NULL)) {
		rsts = csfC5getEType_crsKType( creq->key->key_type, &creq->etype );
		if (rsts) {
			goto clean_exit;
		}
	}

	/*TBD/JNK--checksum type should be dependent on key type*/
	if (creq->ckstype == 0) {
		creq->ckstype = (csfC5_cksType)
			csfC5getcfgnum( CSFC5_CFG_KDCREQ_DEFCKS_T );
	}

clean_exit:
	return rsts;
}
