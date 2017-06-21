/* $Id: desran.h,v 3.2 1995/11/17 17:47:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/desran.h,v $
 *------------------------------------------------------------------
 * desran.h - DES definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: desran.h,v $
 * Revision 3.2  1995/11/17  17:47:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_DESRAN_H_
#define _CSF_DESRAN_H_ "%W% %G%"
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

typedef struct _csfDESRAN_kSeed {
	csf_o sequence_number[8];
	csfDES_kSched random_sequence_key;
} csfDESRAN_kSeed;

void csfDESRANnewKey (
	csfDES_kBlk,
	csfDESRAN_kSeed *
);

void csfDESRANinit (
	const csfDES_kBlk key,
	csfDESRAN_kSeed *p_seed,
	const csfDES_kBlk hseed,
	const csf_ui32 tseed[2]
);

void csfDESRANset (
	const csfDES_kBlk,
	csfDESRAN_kSeed *
);

void csfDESRANsetSeq (
	const csfDES_kBlk,
	csfDESRAN_kSeed *
);

void csfDESRANgenBlk (
	const csfDES_kBlk,
	csfDESRAN_kSeed *
);

#endif /* _CSF_DESRAN_H_ */
