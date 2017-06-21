/* $Id: gcfkdc.c,v 3.2 1995/11/17 17:44:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libx/gcfkdc.c,v $
 *------------------------------------------------------------------
 * gcfkdc.c - Match creds in credential cache
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gcfkdc.c,v $
 * Revision 3.2  1995/11/17  17:44:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original gc_frm_kdc.c 3.10 6/13/94*/

/*
 * Copyright (c) 1993-1994 CyberSAFE Corporation
 * Copyright (c) 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

/* Argument to pass through "scan_cc". */
struct _csfC5ccmatch {
	const csfC5_creds *mcreds;	/* Creds to match. */
	csfC5_creds *rcreds;		/* Where to return creds. */
};

/* cmatch_names -- This function is passed to scan_cc
 * to match the client & server name.  It is called by
 * scan_cc for each credsential until no more or until this
 * function returns anything other than CSF_STS_OK.
 */
static csf_sts cmatch_names (
	const csfC5_creds *ccreds,	/* Creds from cache. */
	void *ccmatch				/* Creds to match & return. */
){
	const csfC5_creds *mcreds;	/* Match these. */
	csfC5_creds *rcreds;		/* Return creds here. */

	mcreds = ((struct _csfC5ccmatch *)ccmatch)->mcreds;
	rcreds = ((struct _csfC5ccmatch *)ccmatch)->rcreds;

	if (
		csfC5isEq_prnId( &ccreds->client, &mcreds->client ) &&
		csfC5isEq_prnId( &ccreds->server, &mcreds->server )
	) {
		*rcreds = *ccreds;		/* scan_cc does NOT free if FOUND */
		return CSF_STS_OK;		/* Don't come back. */
	}
	return CSFC5_STS_NOTFOUND;	/* Try again. */

}

csf_sts csfC5getFromKdc_creds (
	csfC5_credsReqCtx *reqc,
	csfC5_cc *cc,
	csfC5_creds *outcreds
){
	csf_sts rsts;
	csfC5_creds tgtq;
	csfC5_creds tgt;
	csfC5_prnId int_server;
	struct _csfC5ccmatch ccmatch;

	csfC5clear_creds( outcreds );

	csfC5clear_creds( &tgt );
	csfC5clear_creds( &tgtq );
	csfC5clear_prnId( &int_server );

	rsts = csfC5tgsName_prnId( &reqc->req->server.realm,
		&reqc->req->client.realm, NULL, &int_server );
	if (rsts) {
		goto clean_exit;
	}

	tgtq.client = reqc->req->client;
	tgtq.server = int_server;

	ccmatch.mcreds = &tgtq;
	ccmatch.rcreds = &tgt;
	rsts = csfC5scan_cc( cc, &cmatch_names, (void *) &ccmatch );
	if (rsts) {
		goto clean_exit;
	}

	reqc->use = &tgt;
	rsts = csfC5getViaTgt_creds( reqc, outcreds );

 clean_exit:
	csfC5free_prnId( &int_server );
	csfC5free_creds( &tgt );
	if (rsts) {
		csfC5free_creds( outcreds );
	}

	return rsts;
}
