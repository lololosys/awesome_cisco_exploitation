/* $Id: kt.c,v 3.2 1995/11/17 17:44:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/kt/kt.c,v $
 *------------------------------------------------------------------
 * kt.c - Functions to manipulate Kerberos Keytab
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: kt.c,v $
 * Revision 3.2  1995/11/17  17:44:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:11  shaker
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


void csfC5clear_ktEntry (
	csfC5_ktEntry *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_ktEntry (
	csfC5_ktEntry *val
){
	if (val) {
		csfC5free_prnId( &val->prnid );
		csfC5free_crsKBlock( &val->key );
	}
}

csf_sts csfC5copy_ktEntry (
	const csfC5_ktEntry *src,
	csfC5_ktEntry *dst
){
	csf_sts rsts;

	csfC5clear_ktEntry( dst );

	rsts = csfC5copy_prnId( &src->prnid, &dst->prnid );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5copy_crsKBlock( &src->key, &dst->key );
	if (rsts) {
		goto clean_exit;
	}

	dst->tstamp = src->tstamp;
	dst->kvno = src->kvno;

 clean_exit:
	if (rsts) {
		csfC5free_ktEntry( dst );
	}
	return rsts;
}
