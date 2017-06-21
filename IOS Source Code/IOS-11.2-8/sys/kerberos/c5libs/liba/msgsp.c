/* $Id: msgsp.c,v 3.2 1995/11/17 17:44:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/msgsp.c,v $
 *------------------------------------------------------------------
 * msgsp.c - Functions to clear msgsp structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgsp.c,v $
 * Revision 3.2  1995/11/17  17:44:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:39  shaker
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

void csfC5clear_msgSPBody (
	csfC5_msgSPBody *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_msgSPBody (
	csfC5_msgSPBody *val
){
	if (val) {
		csfC5free_data( &val->user_data );
		csfC5free_netHAddr( &val->r_haddr );
		csfC5free_netHAddr( &val->s_haddr );
		csfC5clear_msgSPBody( val );
	}
}

