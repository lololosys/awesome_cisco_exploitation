/* $Id: msgkdc1.c,v 3.2 1995/11/17 17:44:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/msgkdc1.c,v $
 *------------------------------------------------------------------
 * msgkdc1.c - Functions to manipulate msgkdc structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgkdc1.c,v $
 * Revision 3.2  1995/11/17  17:44:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:34  shaker
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


csf_sts csfC5rdEc_msgTgsRep (
	csfC5_data *in,
	const csfC5_crsKBlock *key,
	csfC5_msgKdcRep *out
){
	csf_sts rsts;
	csfC5_msgKdcRep kdcrep;

	csfC5clear_msgKdcRep( &kdcrep );
	rsts = csfC5asnD_msgTgsRep( in, &kdcrep );
	if (rsts) {
		goto clean_exit;
	}
	rsts = csfC5rdEc_msgKdcRepEPart( &kdcrep.ec_epart, key, &kdcrep.epart );

 clean_exit:
	if (rsts) {
		csfC5free_msgKdcRep( &kdcrep );
	}
	*out = kdcrep;
	return rsts;
}


csf_sts csfC5rdEc_msgAsRep (
	csfC5_data *in,
	const csfC5_crsKBlock *key,
	csfC5_msgKdcRep *out
){
	csf_sts rsts;
	csfC5_msgKdcRep kdcrep;

	csfC5clear_msgKdcRep( &kdcrep );
	rsts = csfC5asnD_msgAsRep( in, &kdcrep );
	if (rsts) {
		goto clean_exit;
	}
	rsts = csfC5rdEc_msgKdcRepEPart( &kdcrep.ec_epart, key, &kdcrep.epart );

 clean_exit:
	if (rsts) {
		csfC5free_msgKdcRep( &kdcrep );
	}
	*out = kdcrep;
	return rsts;
}


csf_sts csfC5rdEc_msgKdcRepEPart (
	const csfC5_crsEData *in,
	const csfC5_crsKBlock *kblock,
	csfC5_msgKdcRepEPart *out
){
	csf_sts rsts;
	csfC5_msgKdcRepEPart kdcrepep;
	csfC5_data scratch;

	csfC5clear_msgKdcRepEPart( &kdcrepep );
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

	rsts = csfC5asnD_msgKdcRepEPart( &scratch, &kdcrepep );


 clean_exit:
	csfC5free_data( &scratch );
	if (rsts) {
		csfC5free_msgKdcRepEPart( &kdcrepep );
	}
	*out = kdcrepep;

	return rsts;
}
