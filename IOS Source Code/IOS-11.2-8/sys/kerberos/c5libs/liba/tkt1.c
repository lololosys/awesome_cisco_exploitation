/* $Id: tkt1.c,v 3.2 1995/11/17 17:44:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/tkt1.c,v $
 *------------------------------------------------------------------
 * tkt1.c - Functions to manipulate Kerberos tickets
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tkt1.c,v $
 * Revision 3.2  1995/11/17  17:44:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:46  shaker
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


csf_sts csfC5rdEc_tkt (
	const csfC5_data *in,
	const csfC5_crsKBlock *key,
	csfC5_tkt *out
){
	csf_sts rsts;
	csfC5_tkt tkt;

	csfC5clear_tkt( &tkt );

	rsts = csfC5asnD_tkt( in, &tkt );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5rdEc_tktEPart( &tkt.ec_epart, key, &tkt.epart );

 clean_exit:
	if (rsts) {
		csfC5free_tkt( &tkt );
	}
	*out = tkt;

	return rsts;
}


csf_sts csfC5rdEc_tktEPart (
	const csfC5_crsEData *in,
	const csfC5_crsKBlock *kblock,
	csfC5_tktEPart *out
){
	csf_sts rsts;
	csfC5_tktEPart tktep;
	csfC5_data scratch;

	csfC5clear_tktEPart( &tktep );
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

	rsts = csfC5asnD_tktEPart( &scratch, &tktep );

 clean_exit:
	csfC5free_data( &scratch );
	if (rsts) {
		csfC5free_tktEPart( &tktep );
	}
	*out = tktep;

	return rsts;
}

#if 0
csf_sts csfC5rdEc_tkt (
	const csfC5_crsEData *in,
	const csfC5_crsKBlock *key,
	csfC5_tkt *out
){
	csfC5_data scratch;
	csf_sts rsts;

	csfC5clear_tktEPart( &ticket->epart );

	rsts = csfC5crsDecryptData(
		&ticket->ec_epart.ciphertext,
		key,
		ticket->ec_epart.etype,
		NULL /*ivec*/,
		&scratch
	);
	if (rsts) {
		return rsts;
	}

	rsts = csfC5asnD_tktEPart( &scratch, &ticket->epart );
	csfC5free_data( &scratch );

	return rsts;
}
#endif
