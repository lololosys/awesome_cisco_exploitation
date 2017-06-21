/* $Id: creds.c,v 3.2 1995/11/17 17:45:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libz/creds.c,v $
 *------------------------------------------------------------------
 * creds.c - Misc credentials functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: creds.c,v $
 * Revision 3.2  1995/11/17  17:45:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:17  shaker
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


void csfC5clear_creds (
	csfC5_creds *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_creds (
	csfC5_creds *val
){
	if (val) {
		csfC5free_prnId( &val->client );
		csfC5free_prnId( &val->server );
		csfC5free_crsKBlock( &val->seskey );
		csfC5free_data( &val->e_tkt );
		csfC5free_data( &val->e_tkt2 );
		csfC5freeC_netHAddrS( &val->haddrs );
		csfC5freeC_authzS( &val->authz );
		csfC5clear_creds( val );
	}
}

void csfC5freeC_credsS (
	csfC5_creds ***credl
){
	if (credl) {
		csfC5_creds **credp;
		for (credp = *credl; *credp; credp++) {
			csfC5free_creds( *credp );
			*credp = NULL;
		}
		csfCfree( *credl );
		*credl = NULL;
	}
}


csf_sts csfC5copy_creds (
	const csfC5_creds *increds,
	csfC5_creds *outcreds
){
	csf_sts rsts;

	*outcreds = *increds;

	rsts = csfC5copy_prnId( &increds->client, &outcreds->client );
	if (rsts) {
		goto cleanlast;
	}

	rsts = csfC5copy_prnId( &increds->server, &outcreds->server );
	if (rsts) {
		goto cleanclient;
	}

	rsts = csfC5copy_crsKBlock( &increds->seskey, &outcreds->seskey );
	if (rsts) {
		goto cleanserver;
	}

	rsts = csfC5copyC_netHAddrS( increds->haddrs, &outcreds->haddrs );
	if (rsts) {
		goto cleanblock;
	}

	rsts = csfC5copy_data( &increds->e_tkt, &outcreds->e_tkt );
	if (rsts) {
		goto cleanaddrs;
	}

	rsts = csfC5copy_data( &increds->e_tkt2, &outcreds->e_tkt2 );
	if (rsts) {
		goto cleanticket;
	}

	return CSF_STS_OK;

 cleanticket:
	csfC5free_data( &outcreds->e_tkt );
 cleanaddrs:
	csfC5freeC_netHAddrS( &outcreds->haddrs );
 cleanblock:
	csfC5free_crsKBlock( &outcreds->seskey );
 cleanserver:
	csfC5free_prnId( &outcreds->server );
 cleanclient:
	csfC5free_prnId( &outcreds->client );
 cleanlast:
	csfC5clear_creds( outcreds );

	return rsts;
}


csf_sts csfC5copyC_creds (
	const csfC5_creds *increds,
	csfC5_creds **outcreds
){
	csf_sts rsts;

	*outcreds = (csfC5_creds *) csfCmalloc( sizeof(*outcreds) );
	if (!*outcreds) {
		return CSF_STS_ENOMEM;
	}

	rsts = csfC5copy_creds( increds, *outcreds );

	if (rsts) {
		csfCfree( *outcreds );
		*outcreds = NULL;
	}
	return rsts;
}


void csfC5clear_credsReqCtx (
	csfC5_credsReqCtx *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

