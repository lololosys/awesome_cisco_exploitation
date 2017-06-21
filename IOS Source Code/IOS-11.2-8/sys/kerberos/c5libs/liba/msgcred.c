/* $Id: msgcred.c,v 3.2 1995/11/17 17:44:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/msgcred.c,v $
 *------------------------------------------------------------------
 * msgcred.c - Functions to manipulate message credentials
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgcred.c,v $
 * Revision 3.2  1995/11/17  17:44:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:29  shaker
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

void csfC5clear_msgCredEPart (
	csfC5_msgCredEPart *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_msgCredEPart (
	csfC5_msgCredEPart *val
){
	if (val) {
		csfC5free_netHAddr( &val->r_haddr );
		csfC5free_netHAddr( &val->s_haddr );
		if (val->tktinfo) {
			csfC5_msgCredInfo **temp;
			for (temp = val->tktinfo; *temp; temp++) {
				csfC5free_crsKBlock( &(*temp)->seskey );
				csfC5free_prnId( &(*temp)->client );
				csfC5free_prnId( &(*temp)->server );
				csfC5freeC_netHAddrS( &(*temp)->caddrs );
				csfCfree( *temp );
			}
			csfCfree( val->tktinfo );
			val->tktinfo = NULL;
		}
		csfC5clear_msgCredEPart( val );
	}
}

