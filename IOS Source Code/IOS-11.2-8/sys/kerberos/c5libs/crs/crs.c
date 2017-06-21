/* $Id: crs.c,v 3.2 1995/11/17 17:43:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/crs/crs.c,v $
 *------------------------------------------------------------------
 * crs.c - Kerberos crypto subsystem
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crs.c,v $
 * Revision 3.2  1995/11/17  17:43:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>
#include <csfc5/crs/crs0.h>


/*
 * EData
 */

void csfC5clear_crsEData (
	csfC5_crsEData *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_crsEData (
	csfC5_crsEData *val
){
	if (val) {
		csfC5free_data( &val->ciphertext );
		csfC5clear_crsEData( val );
	}
}

csf_sts csfC5copy_crsEData (
	const csfC5_crsEData *in,
	csfC5_crsEData *out
){
	csfC5_crsEData tmp;
	csf_sts rsts;

	tmp = *in;
	rsts = csfC5copy_data( &in->ciphertext, &tmp.ciphertext );
	if (rsts) {
		return rsts;
	}
	*out = tmp;
	return rsts;
}


/*
 * KBlock
 */

void csfC5clear_crsKBlock (
	csfC5_crsKBlock *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_crsKBlock (
	csfC5_crsKBlock *val
){
	if (val) {
		csfC5free_data( &val->key_data );
		csfC5clear_crsKBlock( val );
	}
}

csf_sts csfC5copy_crsKBlock (
	const csfC5_crsKBlock *from,
	csfC5_crsKBlock *to
){
	csf_sts rsts;

	*to = *from;

	rsts = csfC5copy_data( &from->key_data, &to->key_data );
	if (rsts) {
		csfC5clear_crsKBlock( to );
	}

	return rsts;
}
