/* $Id: xkdcas.c,v 3.3 1996/01/17 23:49:33 che Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/kerberos/c5libs/libx/xkdcas.c,v $
 *------------------------------------------------------------------
 * xkdcas.c - Generate and read AS req 
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xkdcas.c,v $
 * Revision 3.3  1996/01/17  23:49:33  che
 * CSCdi47067:  Need to display Kerberos error messages returned from KDC
 *
 * Revision 3.2  1995/11/17  17:45:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:07  shaker
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
#include "../../../ui/debug.h"
#include "../../../os/os_debug_flags.h"

csf_sts csfC5make_msgAsReq (
	csfC5_credsReqCtx *creq,
	csfC5_pauth *padata,
	csfC5_data *req
){
	csf_sts rsts = CSF_STS_OK;
	csfC5_pauth *palist[2];
	csfC5_msgKdcReq asreq;
	csfC5_data req0;

	csfC5clear_msgKdcReq( &asreq );
	csfC5clear_data( &req0 );

	rsts = csfC5make_msgKdcReqBody( creq, &asreq.e_body );
	if (rsts) {
		goto clean_exit;
	}

	if (padata != NULL) {
		palist[0] = padata;
		palist[1] = NULL;
		asreq.padata = palist;
	}

	rsts = csfC5asnE_msgAsReq( &asreq, &req0 );
	if (rsts) {
		goto clean_exit;
	}

 clean_exit:
	if (rsts) {
		csfC5free_data( &req0 );
	}
	*req = req0;

	return rsts;
}


csf_sts csfC5read_msgAsRep (
	const csfC5_data *rep,
	csfC5_credsReqCtx *creq,
	csfC5pauthKeyProc key_proc,
	void const *keyseed,
	csfC5_msgKdcRep *asrep,
	csfC5_msgErr *errrep
){
	csf_sts rsts = CSF_STS_OK;
	csfC5_crsKBlock decrypt_key;

	csfC5clear_crsKBlock( &decrypt_key );
	csfC5clear_msgKdcRep( asrep );
	csfC5clear_msgErr( errrep );

	rsts = csfC5asnD_msgAsRep( rep, asrep );
	if (rsts) {
		csf_sts rsts1;
		rsts1 = csfC5asnD_msgErr( rep, errrep );
		if (rsts1) {
			goto clean_exit;
		}
		/* Assume the error text is ok.. */
		rsts = errrep->errcode + CSFC5c_STS__base;
		/* ..unless we detect something fishy.. */
		/* A little sanity checking please. */
		if (errrep->stime.secs != creq->nonce) {
			rsts = CSFC5_STS_KDCREP_MODIFIED;
		}
		if (!csfC5isEq_prnId( &errrep->server, creq->server )) {
			rsts = CSFC5_STS_KDCREP_MODIFIED;
		}
		if (!csfC5isEq_prnId( &errrep->client, creq->client )) {
			rsts = CSFC5_STS_KDCREP_MODIFIED;
		}
		if (kerberos_debug) {
		    if (errrep->text.dp) {
			buginf("\nKerberos:\t%s",(char *) errrep->text.dp);
		    }
		    if (errrep->e_data.dp) {
			buginf("\nKerberos:\t%s",(char *) errrep->e_data.dp);
		    }
		}
		goto clean_exit;
	}

	/* Re-generate the key and decrypt the reply. */
	_csfDBGassert( decrypt_proc );
	csfC5free_crsKBlock( &decrypt_key );
	rsts = (*key_proc)( creq->key->key_type, &decrypt_key,
		keyseed, asrep->padata );
	if (rsts) {
		goto clean_exit;
	}
	rsts = csfC5rdEc_msgKdcRepEPart( &asrep->ec_epart, &decrypt_key,
		&asrep->epart );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5check_msgKdcRep( creq, asrep );

 clean_exit:
	csfC5free_crsKBlock( &decrypt_key );

	return rsts;
}
