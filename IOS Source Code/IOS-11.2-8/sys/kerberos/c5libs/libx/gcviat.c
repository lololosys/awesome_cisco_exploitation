/* $Id: gcviat.c,v 3.2 1995/11/17 17:44:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libx/gcviat.c,v $
 *------------------------------------------------------------------
 * gcviat.c - Get creds via TGT cred
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gcviat.c,v $
 * Revision 3.2  1995/11/17  17:44:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original gc_via_tgt.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

csf_sts csfC5getViaTgt_creds (
	csfC5_credsReqCtx *creq,
	csfC5_creds *outcreds
){
	csf_sts rsts;
	csfC5_msgKdcRep tgsrep;
	csfC5_data req;
	csfC5_data rep;

	csfC5clear_creds( outcreds );
	csfC5clear_msgKdcRep( &tgsrep );
	csfC5clear_data( &rep );
	csfC5clear_data( &req );

	rsts = csfC5make_msgTgsReq( creq, &req );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5netSendtoKdc( &req, &creq->server->realm.namec, &rep );
	csfC5free_data( &req );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5rdEc_msgTgsRep( &rep, creq->key, &tgsrep );
	if (rsts) {
		/* Not a TgsRep; try an Err. */
		csf_sts rsts1;
		csfC5_msgErr errrep;

		rsts1 = csfC5asnD_msgErr( &rep, &errrep );
		if (!rsts1) {
			rsts = errrep.errcode + CSFC5c_STS__base;
			csfC5free_msgErr( &errrep );
		}
		csfC5free_data( &rep );
		return rsts;
	}
	csfC5free_data( &rep );

	rsts = csfC5check_msgKdcRep( creq, &tgsrep );
	if (rsts) {
		goto clean_exit;
	}

	/* If reply addresses are null [it means the reply (ticket) contains
	 * the original addresses] we copy them from the request (tgt).
	 * We do this simply to avoid a special-case in "moveKdcRepTo_creds"
	 * (below)--which would have to handle it otherwise, since the addresses
	 * must be visible in the creds structure.
	 */
	if (!tgsrep.epart.caddrs) {
		rsts = csfC5copyC_netHAddrS( creq->use->haddrs, &tgsrep.epart.caddrs );
		if (rsts) {
			goto clean_exit;
		}
	}

	rsts = csfC5asnD_tkt( &tgsrep.e_tkt, &tgsrep.tkt );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5moveKdcRepTo_creds( &tgsrep, outcreds );
	if (rsts) {
		goto clean_exit;
	}

	outcreds->is_skey =
		(creq->req->tktflags & CSFC5c_KDCOPT_ENC_TKT_IN_SKEY_m) ? TRUE : FALSE;


clean_exit:
	csfC5free_msgKdcRep( &tgsrep );
	return rsts;
}
