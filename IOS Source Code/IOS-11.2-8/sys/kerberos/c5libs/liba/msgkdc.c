/* $Id: msgkdc.c,v 3.2 1995/11/17 17:44:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/msgkdc.c,v $
 *------------------------------------------------------------------
 * msgkdc.c - Functions to clear msgkdc structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgkdc.c,v $
 * Revision 3.2  1995/11/17  17:44:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:33  shaker
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

void csfC5clear_msgKdcRep (
	csfC5_msgKdcRep *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_msgKdcRep (
	csfC5_msgKdcRep *val
){
	if (val) {
		csfC5freeC_pauthS( &val->padata );
		csfC5free_prnId( &val->client );
		csfC5free_tkt( &val->tkt );
		csfC5free_data( &val->e_tkt );
		csfC5free_msgKdcRepEPart( &val->epart );
		csfC5free_data( &val->e_epart );
		csfC5free_crsEData( &val->ec_epart );
		csfC5clear_msgKdcRep( val );
	}
}

void csfC5clear_msgKdcRepEPart (
	csfC5_msgKdcRepEPart *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}


void csfC5free_msgKdcRepEPart (
	csfC5_msgKdcRepEPart *val
){
	if (val) {
		csfC5free_crsKBlock( &val->seskey );
		csfC5freeC_lReqS( &val->lreq );
		csfC5free_prnId( &val->server );
		csfC5freeC_netHAddrS( &val->caddrs );
		csfC5clear_msgKdcRepEPart( val );
	}
}

void csfC5clear_msgKdcReqBody (
	csfC5_msgKdcReqBody *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_msgKdcReqBody (
	csfC5_msgKdcReqBody *val
){
	if (val) {
		csfC5free_prnId( &val->client );
		csfC5free_prnId( &val->server );
		csfCfree( val->etypes );
		csfC5freeC_netHAddrS( &val->haddrs );
		csfC5free_crsEData( &val->ec_authz );
		csfC5freeC_authzS( &val->authz );
		csfC5freeC_tktS( &val->tkt2 );
		csfC5free_data( &val->e_tkt2 );
		csfC5clear_msgKdcReqBody( val );
	}
}

void csfC5clear_msgKdcReq (
	csfC5_msgKdcReq *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_msgKdcReq (
	csfC5_msgKdcReq *val
){
	if (val) {
		csfC5freeC_pauthS( &val->padata );
		csfC5free_data( &val->e_body );
		csfC5free_msgKdcReqBody( &val->body );
		csfC5clear_msgKdcReq( val );
	}
}

