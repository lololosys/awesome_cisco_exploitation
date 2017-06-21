/* $Id: msgap.c,v 3.2 1995/11/17 17:44:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/msgap.c,v $
 *------------------------------------------------------------------
 * msgap.c - Functions to clear msgap structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgap.c,v $
 * Revision 3.2  1995/11/17  17:44:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:27  shaker
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

void csfC5clear_msgApRep (
	csfC5_msgApRep *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_msgApRep (
	csfC5_msgApRep *val
){
	if (val) {
		csfC5free_msgApRepEPart( &val->epart );
		csfC5free_crsEData( &val->ec_epart );
		csfC5clear_msgApRep( val );
	}
}

void csfC5clear_msgApRepEPart (
	csfC5_msgApRepEPart *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_msgApRepEPart (
	csfC5_msgApRepEPart *val
){
	if (val) {
		csfC5free_crsKBlock( &val->subkey );
		csfC5clear_msgApRepEPart( val );
	}
}

void csfC5clear_msgApReq (
	csfC5_msgApReq *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_msgApReq (
	csfC5_msgApReq *val
){
	if (val) {
		csfC5free_tkt( &val->tkt );
		csfC5free_data( &val->e_tkt );
		csfC5free_autht( &val->autht );
		csfC5free_crsEData( &val->ec_autht );
		csfC5clear_msgApReq( val );
	}
}

