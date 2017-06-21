/* $Id: msgerr.c,v 3.2 1995/11/17 17:44:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/msgerr.c,v $
 *------------------------------------------------------------------
 * msgerr.c - Functions to clear msgerr structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgerr.c,v $
 * Revision 3.2  1995/11/17  17:44:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:31  shaker
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

void csfC5clear_msgErr (
	csfC5_msgErr *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_msgErr (
	csfC5_msgErr *val
){
	if (val) {
		csfC5free_prnId( &val->client );
		csfC5free_prnId( &val->server );
		csfC5free_data( &val->text );
		csfC5free_data( &val->e_data );
		csfC5clear_msgErr( val );
	}
}

