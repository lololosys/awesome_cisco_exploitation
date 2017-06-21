/* $Id: msgpriv1.c,v 3.2 1995/11/17 17:44:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/msgpriv1.c,v $
 *------------------------------------------------------------------
 * msgpriv1.c - functions to manipulate msgpriv structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgpriv1.c,v $
 * Revision 3.2  1995/11/17  17:44:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:36  shaker
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

csf_sts csfC5rdEc_msgPriv (
	const csfC5_data *inbuf,
	const csfC5_crsKBlock *key,
	void *ivec,
	csfC5_msgPriv *out
){
	csf_sts rsts;
	csfC5_msgPriv priv;

	csfC5clear_msgPriv( &priv );

	rsts = csfC5asnD_msgPriv( inbuf, &priv );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5rdEc_msgPrivEPart( &priv.ec_epart, key, ivec, &priv.epart );

 clean_exit:
	if (rsts) {
		csfC5free_msgPriv( &priv );
	}
	*out = priv;

	return rsts;
}


csf_sts csfC5mkEc_msgPriv (
	csfC5_msgPriv *privmsg,
	const csfC5_crsKBlock *key,
	void *ivec,
	csfC5_data *outbuf
){
	csf_sts rsts;

	csfC5clear_data( outbuf );

	rsts = csfC5mkEc_msgPrivEPart( &privmsg->epart, key, ivec, &privmsg->ec_epart );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5asnE_msgPriv( privmsg, outbuf );

	return rsts;
}


csf_sts csfC5rdEc_msgPrivEPart (
	const csfC5_crsEData *edata,
	const csfC5_crsKBlock *key,
	void *ivec,
	csfC5_msgSPBody *out
){
	csf_sts rsts;
	csfC5_msgSPBody privep;
	csfC5_data scratch;

	csfC5clear_msgSPBody( &privep );
	csfC5clear_data( &scratch );

	rsts = csfC5crsDecryptData(
		&edata->ciphertext,
		key,
		edata->etype,
		ivec,
		&scratch
	);
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5asnD_msgPrivEPart( &scratch, &privep );
	if (rsts) {
		goto clean_exit;
	}

 clean_exit:
	csfC5free_data( &scratch );
	if (rsts) {
		csfC5free_msgSPBody( &privep );
	}
	*out = privep;

	return rsts;
}


csf_sts csfC5mkEc_msgPrivEPart (
	const csfC5_msgSPBody *privep,
	const csfC5_crsKBlock *key,
	void *ivec,
	csfC5_crsEData *edata
){
	csf_sts rsts;
	csfC5_data scratch;

	rsts = csfC5asnE_msgPrivEPart( privep, &scratch );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5crsEncryptData(
		&scratch,
		key,
		edata->etype,
		ivec,
		&edata->ciphertext
	);
	csfC5free_data( &scratch );

	return rsts;

}
