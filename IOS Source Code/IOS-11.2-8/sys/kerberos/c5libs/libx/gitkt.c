/* $Id: gitkt.c,v 3.2 1995/11/17 17:44:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libx/gitkt.c,v $
 *------------------------------------------------------------------
 * gitkt.c - Get TGT from KDC
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gitkt.c,v $
 * Revision 3.2  1995/11/17  17:44:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original get_in_tkt.c 3.4 6/7/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>


csf_sts csfC5getITkt (
	const csfC5_creds *increds,
	const csfC5_pauthType patype,
	const csfC5_crsEType etype,
	csfC5pauthKeyProc key_proc,
	void const *keyseed,
	csfC5_creds *outcreds,
	csfC5_cc *cc
){
	csf_sts rsts = CSF_STS_OK;
	csfC5_pauth padata;
	csfC5_pauth *paitem = NULL;
	csfC5_msgKdcRep asrep;
	csfC5_msgErr errrep;
	csfC5_credsReqCtx reqc;
	csfC5_creds ocreds;
	csfC5_data req;
	csfC5_data rep;

	csfC5clear_pauth( &padata );
	csfC5clear_msgKdcRep( &asrep );
	csfC5clear_msgErr( &errrep );
	csfC5clear_credsReqCtx( &reqc );
	csfC5clear_creds( &ocreds );
	csfC5clear_data( &req );
	csfC5clear_data( &rep );

	reqc.key0.key_type = increds->seskey.key_type;
	rsts = (*key_proc)(
		reqc.key0.key_type,
		&reqc.key0,
		keyseed,
		NULL
	);
	if (rsts) {
		goto clean_exit;
	}

	reqc.key = &reqc.key0;
	reqc.req = increds;
	rsts = csfC5setup_credsReqCtx( &reqc );
	if (rsts) {
		goto clean_exit;
	}

	/* Fetch preauth data. */
	if (patype != CSFC5c_PADATA_T_NONE) {
		rsts = csfC5obtain_pauth( patype, reqc.key, &padata );
		if (rsts) {
			goto clean_exit;
		}
		paitem = &padata;
	}

	rsts = csfC5make_msgAsReq( &reqc, paitem, &req );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5netSendtoKdc( &req, &reqc.client->realm.namec, &rep );
	csfC5free_data( &req );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5read_msgAsRep( &rep, &reqc, key_proc, keyseed, &asrep, &errrep );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5moveKdcRepTo_creds( &asrep, &ocreds );
	if (rsts) {
		goto clean_exit;
	}

	/* Store it in the ccache (maybe). */
	if (cc) {
		rsts = csfC5addCreds_cc( cc, &ocreds );
	}

 clean_exit:

	csfC5free_pauth( &padata );
	csfC5free_msgKdcRep( &asrep );
	csfC5free_msgErr( &errrep );
	csfC5free_crsKBlock( &reqc.key0 );
	csfC5free_data( &req );
	csfC5free_data( &rep );
	if (rsts) {
		csfC5free_creds( &ocreds );
	}

	if (outcreds != NULL) {
		*outcreds = ocreds;
	}

	return rsts;
}

