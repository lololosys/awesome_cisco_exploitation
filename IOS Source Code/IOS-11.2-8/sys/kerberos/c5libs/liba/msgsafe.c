/* $Id: msgsafe.c,v 3.2 1995/11/17 17:44:32 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/msgsafe.c,v $
 *------------------------------------------------------------------
 * msgsafe.c - Funtions to clear msgsafe structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgsafe.c,v $
 * Revision 3.2  1995/11/17  17:44:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:37  shaker
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

void csfC5clear_msgSafe (
	csfC5_msgSafe *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_msgSafe (
	csfC5_msgSafe *val
){
	if (val) {
		csfC5free_msgSPBody( &val->body );
		csfC5free_cksBlock( &val->cks );
		csfC5free_data( &val->e_body );
		csfC5clear_msgSafe( val );
	}
}
