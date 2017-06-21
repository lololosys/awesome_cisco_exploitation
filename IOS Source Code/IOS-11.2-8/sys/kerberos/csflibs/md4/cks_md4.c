/* $Id: cks_md4.c,v 3.2 1995/11/17 17:46:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/md4/cks_md4.c,v $
 *------------------------------------------------------------------
 * cks_md4.c - MD4 Engine
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cks_md4.c,v $
 * Revision 3.2  1995/11/17  17:46:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original md4glue.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * MD4 checksum function.
 */

#include <csf/header.h>
#include "rsa_md4.h"

csf_sts csfMD4sum (
	const void *in,
	csf_size_t len,
	csfMD4_cBlk *cks
){
	csf_RSAMD4_CTX working;

	csf_RSAMD4Init( &working );
	csf_RSAMD4Update( &working, in, len );
	csf_RSAMD4Final( &working );

	csfCmemcpy( cks, &working.digest[0], sizeof(*cks) );
	csfCmemzero( &working, sizeof(working) );

	return CSF_STS_OK;
}
