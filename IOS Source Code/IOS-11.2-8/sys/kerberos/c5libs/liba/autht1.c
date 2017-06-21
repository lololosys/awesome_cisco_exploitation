/* $Id: autht1.c,v 3.2 1995/11/17 17:44:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/autht1.c,v $
 *------------------------------------------------------------------
 * autht1.c - Functions to manipulate authenticator
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: autht1.c,v $
 * Revision 3.2  1995/11/17  17:44:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:23  shaker
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


csf_sts csfC5rdEc_autht (
	const csfC5_crsEData *edata,
	const csfC5_crsKBlock *key,
	void *ivec,
	csfC5_autht *out
){
	csf_sts rsts;
	csfC5_autht autht;
	csfC5_data scratch;

	csfC5clear_autht( &autht );
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

	rsts = csfC5asnD_autht( &scratch, &autht );
	if (rsts) {
		goto clean_exit;
	}

 clean_exit:
	csfC5free_data( &scratch );
	if (rsts) {
		csfC5free_autht( &autht );
	}
	*out = autht;

	return rsts;
}


csf_sts csfC5mkEc_autht (
	const csfC5_autht *autht,
	const csfC5_crsKBlock *key,
	void *ivec,
	csfC5_crsEData *edata
){
	csf_sts rsts;
	csfC5_data scratch;

	rsts = csfC5asnE_autht( autht, &scratch );
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
