/* $Id: lreq.c,v 3.2 1995/11/17 17:44:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/lreq.c,v $
 *------------------------------------------------------------------
 * lreq.c - Function to clear lreq entries
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lreq.c,v $
 * Revision 3.2  1995/11/17  17:44:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:26  shaker
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

void csfC5clear_lReq (
	csfC5_lReq *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_lReq (
	csfC5_lReq *val
){
	if (val) {
		csfC5clear_lReq( val );
	}
}

void csfC5freeC_lReqS (
	csfC5_lReq ***val
){
	if (val && *val) {
		csfC5_lReq **temp;
		for (temp = *val; *temp; temp++) {
			csfC5free_lReq( *temp );
			csfCfreez( *temp, sizeof(**temp) );
			*temp = NULL;
		}
		csfCfree( *val );
		*val = NULL;
	}
}
