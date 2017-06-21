/* $Id: msgsafe1.c,v 3.2 1995/11/17 17:44:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/msgsafe1.c,v $
 *------------------------------------------------------------------
 * msgsafe1.c - Functions to manipulate msgsafe fucntions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgsafe1.c,v $
 * Revision 3.2  1995/11/17  17:44:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:38  shaker
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


csf_sts csfC5mkEc_msgSafe (
	csfC5_msgSafe *safemsg,
	const csfC5_data *key,
	csfC5_data *out
){
	csf_sts rsts;

	rsts = csfC5asnE_msgSafeBody( &safemsg->body, &safemsg->e_body );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5cksSumData( &safemsg->e_body, safemsg->cks.cks_type,
		key, &safemsg->cks );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5asnE_msgSafe( safemsg, out );
	if (rsts) {
		return rsts;
	}

	return rsts;
}


csf_sts csfC5rdEc_msgSafe (
	const csfC5_data *in,
	const csfC5_data *key,
	csfC5_msgSafe *safemsg
){
	csf_sts rsts;
	csfC5_msgSafe tsafe;
	csfC5_cksBlock our_cksum;

	csfC5clear_msgSafe( &tsafe );
	csfC5clear_cksBlock( &our_cksum );

	rsts = csfC5asnD_msgSafe( in, &tsafe );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5cksSumData( &tsafe.e_body, tsafe.cks.cks_type,
		key, &our_cksum );
	if (rsts) {
		goto clean_exit;
	}
	if (csfC5cmp_data( &our_cksum.cks_data, &tsafe.cks.cks_data ) != 0) {
		rsts = CSFC5c_STS_APERR_MODIFIED;
		goto clean_exit;
	}
	rsts = csfC5asnD_msgSafeBody( &tsafe.e_body, &tsafe.body );

 clean_exit:
	if (rsts) {
		csfC5free_msgSafe( &tsafe );
	}
	csfC5free_cksBlock( &our_cksum );
	*safemsg = tsafe;

	return rsts;
}
