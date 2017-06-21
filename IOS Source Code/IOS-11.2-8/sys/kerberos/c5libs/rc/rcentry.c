/* $Id: rcentry.c,v 3.2 1995/11/17 17:45:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/rc/rcentry.c,v $
 *------------------------------------------------------------------
 * rcentry.c - Functions to clear replay cache entries
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rcentry.c,v $
 * Revision 3.2  1995/11/17  17:45:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original rc_conv.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * All rights reserved.
 */

#include <csfc5/header.h>
#include <csfc5/rc/rc0.h>


void csfC5clear_rcEntry (
	csfC5_rcEntry *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}


void csfC5free_rcEntry (
	csfC5_rcEntry *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}


csf_sts csfC5copy_rcEntry (
	const csfC5_rcEntry *src,
	csfC5_rcEntry *dst
){
	*dst = *src;
	return CSF_STS_OK;
}


csf_boolean csfC5isEq_rcEntry (
	const csfC5_rcEntry *val1,
	const csfC5_rcEntry *val2
){
	if (csfCmemcmp( &val1->rchk, &val2->rchk, sizeof(val1->rchk)) == 0){
		return TRUE;
	}
	return FALSE;
}


csf_sts csfC5make_rcEntry (
	csfC5_tStamp rtime,
	const csfC5_data *rdata,
	csfC5_rcEntry *rce
){
	csf_sts rsts;

	rsts = csfMD5sum( rdata->dp, rdata->dl, &rce->rchk );
	if (rsts) {
		csfC5clear_rcEntry( rce );
		return rsts;
	}
	rce->rtime = rtime;
	return rsts;
}


unsigned int csfC5hash_rcEntry (
	csfC5_rcEntry *rep,
	int hsize
){
	unsigned int hashVal = 0;
	unsigned char *cp;
	int i;

	for (i = 0, cp = rep->rchk; i < sizeof(rep->rchk); i++, cp++) {
		hashVal += *cp;
	}

	hashVal = (((int)hashVal % hsize ) + hsize ) % hsize;
	return hashVal;
}


csf_boolean csfC5isAlive_rcEntry (
	csfC5_rcEntry *rce,
	csfC5_tDelta t
){
	csfC5_tStamp time;

	if (csfCsecTOD( &time )) {
		return TRUE; /* who cares? */
	}

	if (rce->rtime + t < time) {
		return FALSE;
	}

	return TRUE;
}
