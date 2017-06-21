/* $Id: gcreds.c,v 3.2 1995/11/17 17:44:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libx/gcreds.c,v $
 *------------------------------------------------------------------
 * gcreds.c - Gets credentials from credentials cache
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gcreds.c,v $
 * Revision 3.2  1995/11/17  17:44:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original get_creds.c 3.3 5/24/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

/* Argument to pass through "scan_cc". */
struct _csfC5ccmatch {
	const csfC5_creds *mcreds;	/* Creds to match. */
	csfC5_creds *rcreds;		/* Where to return creds. */
};

/*TBD/JNK -- The three functions below are used for matching credentials.
 * They should be moved into separate modules and made available as
 * general-purpose functions in the library.
 * (These came straight out of "fcc_retrv.c", which no longer exists.).
 */

static csf_boolean authdata_match (
	csfC5_authz * const *mdata,
	csfC5_authz * const *data
){
	const csfC5_authz *mdatap;
	const csfC5_authz *datap;

	if (mdata == data) {
		return TRUE;
	}
	if (mdata == NULL) {
		return *data == NULL;
	}
	if (data == NULL) {
		return *mdata == NULL;
	}

	while ((mdatap = *mdata ) && ( datap = *data )) {
		if (
			(mdatap->authz_type != datap->authz_type) ||
			(csfC5cmp_data( &mdatap->authz_data, &datap->authz_data ) != 0)
		) {
			return FALSE;
		}
		mdata++;
		data++;
	}
	return (*mdata == NULL) && (*data == NULL);
}

static csf_boolean times_match (
	const csfC5_tktTimes *t1,
	const csfC5_tktTimes *t2
){
	if (t1->trenew) {
		if (t1->trenew > t2->trenew) {
			return FALSE;				/* this one expires too late */
		}
	}
	if (t1->tend) {
		if (t1->tend > t2->tend) {
			return FALSE;				/* this one expires too late */
		}
	}
	/* only care about expiration on a times_match */
	return TRUE;
}


/* cmatch -- This function is passed to scan_cc
 * to match the client & server name.  It is called by
 * scan_cc for each credsential until no more or until this
 * function returns anything other than CSF_STS_OK.
 */
static csf_sts cmatch_tkt (
	const csfC5_creds *ccreds,	/* Creds from cache. */
	void *ccmatch					/* Creds to match & return. */
){
	const csfC5_creds *mcreds;	/* Match these. */
	csfC5_creds *rcreds;		/* Return creds here. */

	mcreds = ((struct _csfC5ccmatch *)ccmatch)->mcreds;
	rcreds = ((struct _csfC5ccmatch *)ccmatch)->rcreds;

	/* If skey, then must have both skey and second ticket match. */
	if (mcreds->is_skey) {
		if (!(
			(mcreds->is_skey == ccreds->is_skey) &&
			(csfC5cmp_data( &mcreds->e_tkt2, &ccreds->e_tkt2 ) == 0)
		)) {
			return CSFC5_STS_NOTFOUND;	/* Try again. */
		}
	}
	/* skey false or match succeeded */

	/* Regardless of above, times and authdata must match. */
	if (!(
		csfC5isEq_prnId( &mcreds->client, &ccreds->client ) &&
		csfC5isEq_prnId( &mcreds->server, &ccreds->server ) &&
		times_match( &mcreds->times, &ccreds->times ) &&
		authdata_match( mcreds->authz, ccreds->authz )
	)) {
			return CSFC5_STS_NOTFOUND;	/* Try again. */
	}

	*rcreds = *ccreds;		/* Note scan_cc does NOT free if OK. */
	return CSF_STS_OK;		/* Don't come back. */

}

csf_sts csfC5get_creds (
	csf_flags gcopts,
	csfC5_credsReqCtx *reqc,
	csfC5_cc *ccache,
	csfC5_creds *outcreds
){
	csf_sts rsts;
	struct _csfC5ccmatch ccmatch;

	csfC5clear_creds( outcreds );

	ccmatch.mcreds = reqc->req;
	ccmatch.rcreds = outcreds;
	rsts = csfC5scan_cc( ccache, cmatch_tkt, (void *) &ccmatch );
	if (rsts == CSF_STS_OK) {
		return rsts;		/* Found. */
	}
	if (rsts != CSFC5_STS_NOTFOUND) {
		return rsts;		/* Error. */
	}
	if (gcopts & CSFC5_GC_CACHED) {
		return rsts;		/* Don't go to kdc. */
	}

	rsts = csfC5getFromKdc_creds( reqc, ccache, outcreds );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5addCreds_cc( ccache, outcreds );

clean_exit:
	if (rsts) {
		csfC5free_creds( outcreds );
	}
	return rsts;
}


csf_sts csfC5getStupid_creds (
	const csfC5_prnId *server,
	csfC5_cc *cc,
	csfC5_creds *creds
){
	csfC5_cc *cc0;
	csfC5_credsReqCtx reqc;
	csfC5_creds reqcreds;
	csfC5_creds outcreds;
	csf_sts rsts;

	csfC5clear_credsReqCtx( &reqc );
	csfC5clear_creds( &reqcreds );
	csfC5clear_creds( &outcreds );

	cc0 = cc;
	if (cc0 == NULL) {
		rsts = csfC5resolve_cc( &cc0, NULL );
		if (rsts) {
			goto clean_exit;
		}
	}

	/* clear took care of these:
	 *	creds.times.endtime = 0; -- as long as possible
	 *	creds.keyblock.keytype = 0; -- no session keytype preference
	 */
	rsts = csfC5copy_prnId( &cc0->prnid, &reqcreds.client );
	if (rsts) {
		goto clean_exit;
	}
	reqcreds.server = *server;
	reqcreds.tktflags =
		(csfC5_tktFlags) csfC5getcfgnum( CSFC5_CFG_KDCREQ_DEFOPT ),

	reqc.req = &reqcreds;
	rsts = csfC5get_creds( 0, &reqc, cc0, &outcreds );

 clean_exit:
	if (cc == NULL && cc0 != NULL) {
		csfC5close_cc( &cc0 );
	}
	csfC5clear_prnId( &reqcreds.server );
	csfC5free_creds( &reqcreds );
	if (rsts) {
		csfC5free_creds( &outcreds );
	}
	*creds = outcreds;
	return rsts;
}
