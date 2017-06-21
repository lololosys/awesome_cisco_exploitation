/* $Id: authz.c,v 3.2 1995/11/17 17:44:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/authz.c,v $
 *------------------------------------------------------------------
 * authz.c - Functions to manipulate authenticator
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: authz.c,v $
 * Revision 3.2  1995/11/17  17:44:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:24  shaker
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


void csfC5clear_authz (
	csfC5_authz *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_authz (
	csfC5_authz *val
){
	if (val) {
		csfC5free_data( &val->authz_data );
		csfC5clear_authz( val );
	}
}

void csfC5freeC_authzS (
	csfC5_authz ***val
){
	if (val && *val) {
		csfC5_authz **temp;
		for (temp = *val; *temp; temp++) {
			csfC5free_authz( *temp );
			csfCfreez( *temp, sizeof(**temp) );
			*temp = NULL;
		}
		csfCfree( *val );
		*val = NULL;
	}
}


csf_sts csfC5copy_authz (
	const csfC5_authz *in,
	csfC5_authz *out
){
	csf_sts rsts;

	*out = *in;

	rsts = csfC5copy_data( &in->authz_data, &out->authz_data );

	if (rsts) {
		csfC5clear_authz( out );
	}
	return rsts;
}

csf_sts csfC5copyC_authz (
	const csfC5_authz *in,
	csfC5_authz **out
){
	csf_sts rsts;

	*out = (csfC5_authz *) csfCcmalloc( sizeof(**out) );
	if (!*out) {
		return CSF_STS_ENOMEM;
	}

	rsts = csfC5copy_authz( in, *out );

	if (rsts) {
		csfC5free_authz( *out );
		*out = NULL;
	}
	return rsts;
}

csf_sts csfC5copyC_authzS (
	csfC5_authz * const *inauthdat,
	csfC5_authz ***outauthdat
){
	csf_sts rsts;
	csfC5_authz **tempauthdat;
	int nelems = 0;

	while (inauthdat[nelems]) {
		nelems++;	/* Count number of elements. */
	}

	/* one more for a null terminated list */
	tempauthdat = (csfC5_authz **)
		csfCcalloc( nelems+1, sizeof(*tempauthdat) );
	if (!tempauthdat) {
		return CSF_STS_ENOMEM;
	}

	for (nelems = 0; inauthdat[nelems]; nelems++) {
		rsts = csfC5copyC_authz
			( inauthdat[nelems], &tempauthdat[nelems] );
		if (rsts) {
			csfC5freeC_authzS( &tempauthdat );
			return rsts;
		}
	}

	*outauthdat = tempauthdat;
	return CSF_STS_OK;
}
