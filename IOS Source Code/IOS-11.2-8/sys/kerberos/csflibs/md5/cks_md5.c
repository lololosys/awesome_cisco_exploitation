/* $Id: cks_md5.c,v 3.2 1995/11/17 17:46:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/md5/cks_md5.c,v $
 *------------------------------------------------------------------
 * cks_md5.c - MD5 Engine
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cks_md5.c,v $
 * Revision 3.2  1995/11/17  17:46:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original md5glue.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * All rights reserved.
 */

/*
 * MD5 checksum
 */

#include <csf/header.h>
#include "rsa_md5.h"

csf_sts csfMD5sum (
	const void *in,
	csf_size_t len,
	csfMD5_cBlk *cks
){
	csf_RSAMD5_CTX working;

	csf_RSAMD5Init( &working );
	csf_RSAMD5Update( &working, in, len );
	csf_RSAMD5Final( &working );

	csfCmemcpy( cks, &working.digest[0], sizeof(*cks) );
	csfCmemzero( &working, sizeof(working) );

	return CSF_STS_OK;
}
