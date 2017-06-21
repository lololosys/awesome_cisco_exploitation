/* $Id: cks.h,v 3.2 1995/11/17 17:47:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/cks.h,v $
 *------------------------------------------------------------------
 * cks.h - Checksum public definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cks.h,v $
 * Revision 3.2  1995/11/17  17:47:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_CKS_H_
#define _CSFC5_CKS_H_ "%W% %G%"
/*.original encryption.h 3.4 5/16/94*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1989,1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Checksum (CKS)
 */

#ifndef _CSFC5_CKS_STS_HS_
#define _CSFC5_CKS_STS_HS_
#include <csfc5/sts/cks_sts.hs>
#endif

#ifndef csfC5_cksType
typedef csf_ui16 csfC5_cksType;
#endif

csf_size_t csfC5size_cksType (
	csfC5_cksType ckstype
);

csf_sts csfC5check_cksType (
	csfC5_cksType ckstype
);

csf_boolean csfC5isCollProof_cksType (
	csfC5_cksType ckstype
);

csf_boolean csfC5isKeyed_cksType (
	csfC5_cksType ckstype
);

typedef struct _csfC5_cksBlock {
	csfC5_cksType cks_type;
	csfC5_data cks_data;
	csf_o cks_data0[64];
} csfC5_cksBlock;

void csfC5clear_cksBlock( csfC5_cksBlock * );
void csfC5free_cksBlock( csfC5_cksBlock * );


csf_sts csfC5setSumType_cksBlock (
	csfC5_cksType ckstype,
	csfC5_cksBlock *cksblock
);

csf_sts csfC5cksSumData (
	const csfC5_data *indata,
	csfC5_cksType ckstype,
	const csfC5_data *seed,
	csfC5_cksBlock *cksblock
);

csf_sts csfC5cksCalculate (
	csfC5_cksType cktype,
	void *in,
	csf_size_t in_len,
	void *seed,
	csf_size_t seed_len,
	csfC5_cksBlock *out
);

/* This is the "standard" crc-32 (autodin II) used by various other
 * subsystems.  Note that it does not take seed and seed length
 * arguments.
 */
csf_sts csfC5cksCrc32 (
	void *in,
	csf_size_t len,
	csfC5_cksBlock *cks
);

#endif /* _CSFC5_CKS_H_ */
