/* $Id: des.h,v 3.2 1995/11/17 17:47:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/des.h,v $
 *------------------------------------------------------------------
 * des.h - DES definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: des.h,v $
 * Revision 3.2  1995/11/17  17:47:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_DES_H_
#define _CSF_DES_H_ "%W% %G%"
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

/* Control flag passed to routines (yuck).
 */
#define CSF_CRS_DES_ENCRYPT		TRUE		/* Encrypt the data. */
#define CSF_CRS_DES_DECRYPT		FALSE		/* Decrypt the data. */

typedef csf_o csfDES_kBlk[8];				/* Key/crypto-block size */

/* Key schedule--used internally by DES routines to gain some speed */
#if 0
typedef struct csfDES_kSched {
	csfDES_kBlk _;
} csfDES_kSched[16];
#else
typedef struct _csfDES_kSched {
	csfDES_kBlk kb[16];
} csfDES_kSched;
#endif

csf_sts csfDESEncryptCBC (
	const void *in,				/* >= length bytes of input text */
	void *out,						/* >= length bytes of output text */
	csf_size_t len,				/*  in bytes */
	const csfDES_kSched *keysch,	/* precomputed key schedule */
	const void *iv					/* 8 bytes of ivec */
);

csf_sts csfDESDecryptCBC (
	const void *in,				/* >= length bytes of input text */
	void *out,						/* >= length bytes of output text */
	csf_size_t len,				/*  in bytes */
	const csfDES_kSched *keysch,	/* precomputed key schedule */
	const void *iv					/* 8 bytes of ivec */
);

void csfDESCryptECB (
	const csf_ui32 *in,			/* 8 bytes of input */
	csf_ui32 *out,					/* 8 bytes of output */
	const csfDES_kSched *keysch,	/* precomputed key schedule */
	csf_boolean crypt				/* TRUE encrypt; else decrypt */
);

void csfDESchecksumCBC (
	const void *in,				/* >= length bytes of inputtext */
	void *out,						/* >= length bytes of outputtext */
	csf_size_t length,				/* in bytes */
	const csfDES_kSched *key,		/* precomputed key schedule */
	const void *iv					/* 8 bytes of ivec */
);

int csfDESmake_kSched (
	const csfDES_kBlk cblock,
	csfDES_kSched *keysch
);

void csfDESfixParity_kBlk (
	csfDES_kBlk cblock
);

csf_boolean csfDESisGoodParity_kBlk (
	const csfDES_kBlk cblock
);

csf_boolean csfDESisWeak_kBlk (
	const csfDES_kBlk cblock
);

csf_sts csfDESstringTo_kBlk (
	const void *data,
	csf_size_t datalen,
	const void *salt,
	csf_size_t saltlen,
	csfDES_kBlk *okey
);

#endif /* _CSF_DES_H_ */
