/* $Id: des0.h,v 3.2 1995/11/17 17:44:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/crs/des/des0.h,v $
 *------------------------------------------------------------------
 * des0.h - DES definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: des0.h,v $
 * Revision 3.2  1995/11/17  17:44:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_LIB_DES0_H_
#define _CSFC5_LIB_DES0_H_ "%W% %G%"
/*.original mit-des.h 3.2 5/11/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * Export of this software from the United States of America is assumed
 * to require a specific license from the United States Government.
 * It is the responsibility of any person or organization contemplating
 * export to obtain such a license before exporting.
 */

/*
 * MIT Data Encryption Standard software implementation declarations.
 */

#include <csfc5/crs/crs0.h>

csf_sts csfC5crsDesProcessKey (
	csfC5_crsEBlock *eblock,
	const csfC5_crsKBlock *keyblock
);

csf_sts csfC5crsDesFinishKey (
	csfC5_crsEBlock *eblock
);

csf_sts csfC5crsDesFinishRandomKey (
	void **
);

csf_sts csfC5crsDesInitRandomKey (
	const csfC5_crsKBlock *,
	void **
);

csf_sts csfC5crsDesEncryptFunc (
	void const *,
	void *,
	const csf_size_t,
	csfC5_crsEBlock *,
	void *
);

csf_sts csfC5crsDesDecryptFunc (
	void const *,
	void *,
	const csf_size_t,
	csfC5_crsEBlock *,
	void *
);

csf_sts csfC5crsDesrawEncryptFunc (
	void const *,
	void *,
	const csf_size_t,
	csfC5_crsEBlock *,
	void *
);

csf_sts csfC5crsDesrawDecryptFunc (
	void const *,
	void *,
	const csf_size_t,
	csfC5_crsEBlock *,
	void *
);

csf_sts csfC5crsDesEncryptF (
	void const *in,
	void *out,
	const csf_size_t size,
	csfC5_crsEBlock * key,
	void *ivec
);

csf_sts csfC5crsDesDecryptF (
	void const *in,
	void *out,
	const csf_size_t size,
	csfC5_crsEBlock * key,
	void *ivec
);

void csfC5crsDesCbcCksum (
	void const *in,				/* >= len bytes of inputtext */
	void *out,						/* >= len bytes of outputtext */
	csf_size_t len,				/* in bytes */
	csfDES_kSched key,		/* precomputed key schedule */
	void *iv						/* 8 bytes of ivec */
);

csf_sts csfC5crsDesRandomKey (
	void *,
	csfC5_crsKBlock *
);

csf_sts csfC5crsDesStringToKey (
	const csfC5_crsKType,
	csfC5_crsKBlock *,
	const csfC5_data *,
	const csfC5_data *
);

#endif /* _CSFC5_LIB_DES0_H_ */
