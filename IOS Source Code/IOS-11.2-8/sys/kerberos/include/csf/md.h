/* $Id: md.h,v 3.2 1995/11/17 17:47:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/md.h,v $
 *------------------------------------------------------------------
 * md.h - Message Digest definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: md.h,v $
 * Revision 3.2  1995/11/17  17:47:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_MD_H_
#define _CSF_MD_H_  "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

/*
 * MD -- Message digest functions.
 */

typedef csf_ui8 csfMD4_cBlk[16];

csf_sts csfMD4sum (
	const void *in,
	csf_size_t len,
	csfMD4_cBlk *cks
);

typedef csf_ui8 csfMD5_cBlk[16];

csf_sts csfMD5sum (
	const void *in,
	csf_size_t len,
	csfMD5_cBlk *cks
);

#endif
