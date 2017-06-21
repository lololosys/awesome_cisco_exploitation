/* $Id: cks.c,v 3.2 1995/11/17 17:43:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/cks/cks.c,v $
 *------------------------------------------------------------------
 * cks.c - Functions to clear Kerberos checksum
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cks.c,v $
 * Revision 3.2  1995/11/17  17:43:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original f_cksum.c 3.3 6/3/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>


void csfC5clear_cksBlock (
	csfC5_cksBlock *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}


void csfC5free_cksBlock (
	csfC5_cksBlock *val
){
	if (val) {
		csfC5free_data( &val->cks_data );
		csfC5clear_cksBlock( val );
	}
}
