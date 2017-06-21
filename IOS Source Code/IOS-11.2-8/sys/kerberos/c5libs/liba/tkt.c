/* $Id: tkt.c,v 3.2 1995/11/17 17:44:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/tkt.c,v $
 *------------------------------------------------------------------
 * tkt.c - Functions to clear Kerberos tickets
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tkt.c,v $
 * Revision 3.2  1995/11/17  17:44:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:45  shaker
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


void csfC5clear_tkt (
	csfC5_tkt *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}


void csfC5free_tkt (
	csfC5_tkt *val
){
	if (val) {
		csfC5free_prnId( &val->server );
		csfC5free_tktEPart( &val->epart );
		csfC5free_crsEData( &val->ec_epart );
		csfC5clear_tkt( val );
	}
}


void csfC5freeC_tktS (
	csfC5_tkt ***val
){

	if (val && *val) {
		csfC5_tkt **temp;
		for (temp = *val; *temp; temp++) {
			csfC5free_tkt( *temp );
			csfCfreez( *temp, sizeof(**temp) );
			*temp =  NULL;
		}
		csfCfree( *val );
		*val = NULL;
	}
}


csf_sts csfC5copy_tkt (
	const csfC5_tkt *from,
	csfC5_tkt *pto
){
	csf_sts rsts = CSF_STS_OK;
	csfC5_tkt tempto;

	csfC5clear_tkt( pto );
	csfC5clear_tkt( &tempto );

	tempto.tktvno = from->tktvno;

	rsts = csfC5copy_prnId( &from->server, &tempto.server );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5copy_crsEData( &from->ec_epart, &tempto.ec_epart );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5copy_tktEPart( &from->epart, &tempto.epart );
	if (rsts) {
		goto clean_exit;
	}

 clean_exit:
	if (rsts) {
		csfC5free_tkt( &tempto );
	}
	else {
		*pto = tempto;
	}

	return rsts;
}

void csfC5clear_tktEPart (
	csfC5_tktEPart *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_tktEPart (
	csfC5_tktEPart *val
){
	if (val) {
		csfC5free_crsKBlock( &val->seskey );
		csfC5free_prnId( &val->client );
		csfC5free_trans( &val->trans );
		csfC5freeC_netHAddrS( &val->caddrs );
		csfC5freeC_authzS( &val->authz );
		csfC5clear_tktEPart( val );
	}
}


csf_sts csfC5copy_tktEPart (
	const csfC5_tktEPart *partfrom,
	csfC5_tktEPart *partto
){
	csf_sts rsts = CSF_STS_OK;
	csfC5_tktEPart tempto;

	csfC5clear_tktEPart( &tempto );
	csfC5clear_tktEPart( partto );

	tempto.flags = partfrom->flags;

	rsts = csfC5copy_crsKBlock( &partfrom->seskey, &tempto.seskey );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5copy_prnId( &partfrom->client, &tempto.client );
	if (rsts) {
		goto clean_exit;
	}

	rsts = csfC5copy_trans( &partfrom->trans, &tempto.trans );
	if (rsts) {
		goto clean_exit;
	}

	tempto.times = partfrom->times;

	rsts = csfC5copyC_netHAddrS( partfrom->caddrs, &tempto.caddrs );
	if (rsts) {
		goto clean_exit;
	}

	if (partfrom->authz) {
		rsts = csfC5copyC_authzS( partfrom->authz, &tempto.authz );
		if (rsts) {
			goto clean_exit;
		}
	}

 clean_exit:

	if (rsts) {
		csfC5free_tktEPart( &tempto );
		csfC5clear_tktEPart( &tempto );
	}
	*partto = tempto;

	return rsts;
}
