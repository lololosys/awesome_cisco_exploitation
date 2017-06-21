/* $Id: msgpriv.c,v 3.2 1995/11/17 17:44:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/msgpriv.c,v $
 *------------------------------------------------------------------
 * msgpriv.c - Functions to clear msgpriv structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgpriv.c,v $
 * Revision 3.2  1995/11/17  17:44:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:35  shaker
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


void csfC5clear_msgPriv (
	csfC5_msgPriv *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}


void csfC5free_msgPriv (
	csfC5_msgPriv *val
){
	if (val) {
		csfC5free_msgSPBody( &val->epart );
		csfC5free_crsEData( &val->ec_epart );
		csfC5clear_msgPriv( val );
	}
}
