/* $Id: autht.c,v 3.2 1995/11/17 17:44:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/autht.c,v $
 *------------------------------------------------------------------
 * autht.c - Functions to clear authenticator
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: autht.c,v $
 * Revision 3.2  1995/11/17  17:44:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:22  shaker
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

void csfC5clear_autht (
	csfC5_autht *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_autht (
	csfC5_autht *val
){
	if (val) {
		csfC5free_prnId( &val->client );
		csfC5free_cksBlock( &val->cks );
		csfC5free_crsKBlock( &val->subkey );
		csfC5freeC_authzS( &val->authz );
		csfC5clear_autht( val );
	}
}

