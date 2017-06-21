/* $Id: pauth.c,v 3.2 1995/11/17 17:44:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/pauth.c,v $
 *------------------------------------------------------------------
 * pauth.c - Functions to clear pauth structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pauth.c,v $
 * Revision 3.2  1995/11/17  17:44:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original f_padata.c 3.3 6/3/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

void csfC5clear_pauth (
	csfC5_pauth *val
){

	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_pauth (
	csfC5_pauth *val
){

	if (val) {
		csfC5free_data( &val->pa_data );
		csfC5clear_pauth( val );
	}
}

void csfC5freeC_pauthS (
	csfC5_pauth ***val
){

	if (val && *val) {
		csfC5_pauth **temp;
		for (temp = *val; *temp; temp++) {
			csfC5free_pauth( *temp );
			csfCfreez( *temp, sizeof(**temp) );
			*temp = NULL;
		}
		csfCfree( *val );
		*val = NULL;
	}
}
