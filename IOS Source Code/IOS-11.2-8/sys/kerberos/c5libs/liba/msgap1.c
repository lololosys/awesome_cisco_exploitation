/* $Id: msgap1.c,v 3.2 1995/11/17 17:44:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/msgap1.c,v $
 *------------------------------------------------------------------
 * msgap1.c - Functions to manipulate msgap structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgap1.c,v $
 * Revision 3.2  1995/11/17  17:44:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:28  shaker
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


/*
 * ApRep
 */

csf_sts csfC5rd_msgApRep (
	const csfC5_data *in,
	csfC5_msgApRep *out
){
	csf_sts rsts;
	csfC5_msgApRep aprep;

	csfC5clear_msgApRep( &aprep );

	rsts = csfC5asnD_msgApRep( in, &aprep );

	return rsts;
}

csf_sts csfC5rdEc_msgApRep (
	const csfC5_data *in,
	const csfC5_crsKBlock *key,
	csfC5_msgApRep *out
){
	csf_sts rsts;
	csfC5_msgApRep aprep;

	csfC5clear_msgApRep( &aprep );

	rsts = csfC5asnD_msgApRep( in, &aprep );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5rdEc_msgApRepEPart( &aprep.ec_epart, key, &aprep.epart );

 clean_exit:
	if (rsts) {
		csfC5free_msgApRep( &aprep );
	}
	*out = aprep;

	return rsts;
}

csf_sts csfC5rdEc_msgApRepEPart (
	const csfC5_crsEData *in,
	const csfC5_crsKBlock *kblock,
	csfC5_msgApRepEPart *out
){
	csf_sts rsts;
	csfC5_msgApRepEPart aprepep;
	csfC5_data scratch;

	csfC5clear_msgApRepEPart( &aprepep );
	csfC5clear_data( &scratch );

	rsts = csfC5crsDecryptData(
		&in->ciphertext,
		kblock,
		in->etype,
		NULL, /*ivec*/
		&scratch
	);
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5asnD_msgApRepEPart( &scratch, &aprepep );

 clean_exit:
	csfC5free_data( &scratch );
	if (rsts) {
		csfC5free_msgApRepEPart( &aprepep );
	}
	*out = aprepep;

	return rsts;
}


csf_sts csfC5mk_msgApRep (
	csfC5_msgApRep *in,
	csfC5_data *out
){
	csf_sts rsts;

	csfC5clear_data( out );

	rsts = csfC5asnE_msgApRep( in, out );

	return rsts;
}

csf_sts csfC5mkEc_msgApRep (
	csfC5_msgApRep *in,
	const csfC5_crsKBlock *key,
	csfC5_data *out
){
	csf_sts rsts;

	csfC5clear_data( out );

	rsts = csfC5mkEc_msgApRepEPart( &in->epart, key, &in->ec_epart );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5asnE_msgApRep( in, out );

	return rsts;
}

csf_sts csfC5mkEc_msgApRepEPart (
	const csfC5_msgApRepEPart *in,
	const csfC5_crsKBlock *kblock,
	csfC5_crsEData *edata
){
	csf_sts rsts;
	csfC5_data scratch;

	rsts = csfC5asnE_msgApRepEPart( in, &scratch );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5crsEncryptData(
		&scratch,
		kblock,
		edata->etype,
		NULL, /*ivec*/
		&edata->ciphertext
	);
	csfC5free_data( &scratch );

	return rsts;
}


/*
 * ApReq
 */

csf_sts csfC5rd_msgApReq (
	const csfC5_data *in,
	csfC5_msgApReq *out
){
	csf_sts rsts;

	csfC5clear_msgApReq( out );

	rsts = csfC5asnD_msgApReq( in, out );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5asnD_tkt( &out->e_tkt, &out->tkt );
	if (rsts) {
		goto clean_exit;
	}

clean_exit:
	return rsts;
}

csf_sts csfC5rdEc_msgApReq (
	const csfC5_crsKBlock *key,
	csfC5_msgApReq *apreq
){
	csf_sts rsts;

	rsts = csfC5rdEc_tktEPart(
		&apreq->tkt.ec_epart,
		key,
		&apreq->tkt.epart
	);
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5rdEc_autht(
		&apreq->ec_autht,
		&apreq->tkt.epart.seskey,
		NULL,
		&apreq->autht
	);
	if (rsts) {
		goto clean_exit;
	}

 clean_exit:
	return rsts;

}

csf_sts csfC5mk_msgApReq (
	const csfC5_msgApReq *in,
	csfC5_data *out
){
	csf_sts rsts;

	csfC5clear_data( out );

	rsts = csfC5asnE_msgApReq( in, out );

	return rsts;
}
