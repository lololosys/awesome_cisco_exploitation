/* $Id: crs.h,v 3.2 1995/11/17 17:47:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/crs.h,v $
 *------------------------------------------------------------------
 * crs.h - Crypto subsystem public definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crs.h,v $
 * Revision 3.2  1995/11/17  17:47:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_CRS_H_
#define _CSFC5_CRS_H_ "%W% %G%"
/*.original encryption.h 3.4 5/16/94*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1989,1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Encryption (CRS)
 */

#ifndef _CSFC5_CRS_STS_HS_
#define _CSFC5_CRS_STS_HS_
#include <csfc5/sts/crs_sts.hs>
#endif


#ifndef csfC5_crsKType
typedef csf_ui16 csfC5_crsKType;
#endif


typedef struct _csfC5_crsKBlock {
	csfC5_crsKType key_type;
	csfC5_data key_data;
	csf_o key_data0[32];
} csfC5_crsKBlock;

void csfC5clear_crsKBlock( csfC5_crsKBlock * );
void csfC5free_crsKBlock( csfC5_crsKBlock * );
csf_sts csfC5copy_crsKBlock( const csfC5_crsKBlock *, csfC5_crsKBlock * );


csf_sts csfC5crsGenSeqNo (
	const csfC5_crsKBlock *,
	csfC5_seqNo *
);


#ifndef csfC5_crsEType
typedef csf_ui16 csfC5_crsEType;
#endif

#ifndef csfC5_crsKVNo
typedef csf_ui8 csfC5_crsKVNo;
#endif

typedef struct _csfC5_crsEData {
	csfC5_crsEType etype;
	csfC5_crsKVNo kvno;
	csfC5_data ciphertext;
} csfC5_crsEData;

void csfC5clear_crsEData( csfC5_crsEData * );
void csfC5free_crsEData( csfC5_crsEData * );
csf_sts csfC5copy_crsEData( const csfC5_crsEData *, csfC5_crsEData * );


csf_sts csfC5check_crsEType (
	csfC5_crsEType etype
);

csf_sts csfC5check_crsKType (
	csfC5_crsKType ktype
);


csf_sts csfC5getEType_crsKType (
	csfC5_crsKType in,
	csfC5_crsEType *out
);

csf_sts csfC5getKType_crsEType (
	csfC5_crsEType in,
	csfC5_crsKType *out
);

typedef struct _csfC5_crsEBlock {	/* Note -- NOT tied to any external defs. */
	csfC5_crsEType crstype;
	csfC5_crsKBlock *key;
	csfC5_data eb_data;
	csf_o eb_data0[256];
} csfC5_crsEBlock;

void csfC5clear_crsEBlock( csfC5_crsEBlock * );
void csfC5free_crsEBlock( csfC5_crsEBlock * );


csf_size_t csfC5blockSize_crsEBlock (
	csfC5_crsEBlock *eblock
);

csf_size_t csfC5crsEncryptSize (
	csf_size_t length,
	csfC5_crsEBlock *eblock
);

/* set up *eblockp to use etype */

csf_sts csfC5setEType_crsEBlock (
	csfC5_crsEBlock *eblockp,
	csfC5_crsEType etype
);

/* ...or keytype */

csf_sts csfC5setKType_crsEBlock (
	csfC5_crsEBlock *eblockp,
	csfC5_crsKType ktype
);

csf_sts csfC5crsEncrypt (
	void *inptr,
	void *outptr,
	csf_size_t length,
	csfC5_crsEBlock *eblock,
	void *ivec
);

csf_sts csfC5crsDecrypt (
	void *inptr,
	void *outptr,
	csf_size_t size,
	csfC5_crsEBlock *eblock,
	void *ivec
);

csf_sts csfC5crsProcessKey (
	csfC5_crsEBlock *eblock,
	const csfC5_crsKBlock *keyblock
);

csf_sts csfC5crsFinishKey (
	csfC5_crsEBlock *eblock
);

csf_sts csfC5crsStringToKey (
	csfC5_crsEBlock *eblock,
	csfC5_crsKType keytype,
	csfC5_crsKBlock *keyblock,
	const csfC5_data *data,
	const csfC5_data *salt
);

csf_sts csfC5crsInitRanKey (
	csfC5_crsEBlock *eblock,
	const csfC5_crsKBlock *keyblock,
	void **ptr
);

csf_sts csfC5crsFinishRanKey (
	const csfC5_crsEBlock *eblock,
	void **ptr
);

/*
 * Utilities.
 */

csf_sts csfC5crsGenRanKey (
	csfC5_crsEBlock *eblock,
	void *ptr,
	csfC5_crsKBlock *keyblock
);

csf_sts csfC5crsGenSubKey (
	const csfC5_crsKBlock *,
	csfC5_crsKBlock *
);

csf_sts csfC5crsDecryptData (
	const csfC5_data *in,
	const csfC5_crsKBlock *kblock,
	const csfC5_crsEType etype,
	void *ivec,
	csfC5_data *out
);

csf_sts csfC5crsEncryptData (
	const csfC5_data *in,
	const csfC5_crsKBlock *kblock,
	const csfC5_crsEType etype,
	void *ivec,
	csfC5_data *out
);

#endif /* _CSFC5_CRS_H_ */
