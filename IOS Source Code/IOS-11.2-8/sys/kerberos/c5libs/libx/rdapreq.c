/* $Id: rdapreq.c,v 3.2 1995/11/17 17:45:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libx/rdapreq.c,v $
 *------------------------------------------------------------------
 * rdapreq.c - read AP req
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rdapreq.c,v $
 * Revision 3.2  1995/11/17  17:45:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (c) 1993-1994 CyberSAFE Coporation.
 * Copyright (c) 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>


csf_sts csfC5readSvcKey_msgApReq (
	void *ktname,
	const csfC5_prnId *prn,
	csfC5_crsKVNo kvno,
	csfC5_crsKBlock *key
){
	return csfC5getSvcKey( (char *)ktname, prn, kvno, key );
}


csf_sts csfC5read_msgApReq (
	const csfC5_data *inbuf,
	const csfC5_prnId *server,
	const csfC5_netHAddr *s_haddr,
	csfC5readKeyProc_msgApReq keyproc,
	void *keyprocarg,
	csfC5_rc *rc,
	csfC5_msgApReq *apreq
){
	csf_sts rsts;

	rsts = csfC5rd_msgApReq( inbuf, apreq );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5readD_msgApReq(
		apreq,
		server,
		s_haddr,
		keyproc,
		keyprocarg,
		rc
	);

	/* TBD/JNK--we do NOT free the apreq on error before exiting.
	 * Caller may want to look at it for, e.g., an expired ticket.
	 * Reasonable?
	 */
	return rsts;

}


csf_sts csfC5readD_msgApReq (
	csfC5_msgApReq *apreq,
	const csfC5_prnId *server,
	const csfC5_netHAddr *s_haddr,
	csfC5readKeyProc_msgApReq keyproc,
	void *keyprocarg,
	csfC5_rc *rc
){
	csf_sts rsts = CSF_STS_OK;
	csfC5_crsKBlock tktkey;
	csfC5_tStamp tnow, tstart;

	/*TBD/JNK--special processing for AP_OPTS_USE_SESSION_KEY?
	 * if (apreq->ap_options & AP_OPTS_USE_SESSION_KEY) {
	 *		...
	 * }
	 */

	if (keyproc == NULL) {
		keyproc = csfC5readSvcKey_msgApReq;
	}

	/* Fetch the key to decrypt the ticket. */
	rsts = (*keyproc)( keyprocarg, &apreq->tkt.server,
			apreq->tkt.ec_epart.kvno, &tktkey );
	if (rsts) {
		return rsts;
	}

	/* Decrypt the rest of the request (including the authenticator). */
	rsts = csfC5rdEc_msgApReq( &tktkey, apreq );
	if (rsts) {
		goto cleanup;
	}

	/* Check the authenticator.
	 */
	if (
		!csfC5isEq_prnId( &apreq->autht.client, &apreq->tkt.epart.client )
	) {
		rsts = CSFC5c_STS_APERR_BADMATCH;
		goto cleanup;
	}
	if (
		s_haddr &&
		!csfC5isIn_netHAddr( s_haddr, apreq->tkt.epart.caddrs )
	) {
		rsts = CSFC5c_STS_APERR_BADADDR;
		goto cleanup;
	}

	rsts = csfC5getNow_tStamp( &tnow );
	if (rsts) {
		goto cleanup;
	}
	if (csfC5cmpSkewed_tStamp( apreq->autht.ctime.secs, tnow ) != 0) {
		rsts = CSFC5c_STS_APERR_SKEW;
		goto cleanup;
	}

	if (rc) {
		rsts = csfC5putEntry_rc( rc, apreq->autht.ctime.secs,
			&apreq->ec_autht.ciphertext );
		if (rsts) {
			goto cleanup;
		}
	}

	/* if tstart is not in ticket, then treat it as tauth */
	tstart = apreq->tkt.epart.times.tstart;
	if (tstart == 0) {
		tstart = apreq->tkt.epart.times.tauth;
	}
	if (csfC5cmpSkewed_tStamp( tstart, tnow ) > 0) {
		rsts = CSFC5c_STS_APERR_TKT_NYV;
		goto cleanup;
	}

	if (apreq->tkt.epart.flags & CSFC5c_TKTFLG_INVALID_m) {
		rsts = CSFC5_STS_TKT_INVALID;
		goto cleanup;
	}

 cleanup:

	csfC5free_crsKBlock( &tktkey );

	if (rsts) {
		csfC5free_tktEPart( &apreq->tkt.epart );
		return rsts;
	}

	/* Now check to see if it is expired */
	if (csfC5cmpSkewed_tStamp( tnow, apreq->tkt.epart.times.tend ) > 0) {
		rsts = CSFC5c_STS_APERR_TKT_EXPIRED;	/* ticket expired */
	}

	return rsts;
}
