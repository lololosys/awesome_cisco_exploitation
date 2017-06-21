/* $Id: trans.c,v 3.2 1995/11/17 17:44:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/trans.c,v $
 *------------------------------------------------------------------
 * trans.c - Functions to clear trans structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: trans.c,v $
 * Revision 3.2  1995/11/17  17:44:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * All rights reserved.
 */

#include <csfc5/header.h>


void csfC5clear_trans (
	csfC5_trans *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_trans (
	csfC5_trans *val
){
	if (val) {
		csfC5free_data( &val->tr_data );
		csfC5clear_trans( val );
	}
}

csf_sts csfC5copy_trans (
	const csfC5_trans *in,
	csfC5_trans *out
){
	csfC5_trans tmp;
	csf_sts rsts;

	tmp = *in;
	rsts = csfC5copy_data( &in->tr_data, &tmp.tr_data );
	if (rsts) {
		return rsts;
	}
	*out = tmp;
	return CSF_STS_OK;

}
