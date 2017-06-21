/* $Id: pauth.h,v 3.2 1995/11/17 17:48:49 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/pauth.h,v $
 *------------------------------------------------------------------
 * pauth.h - pauth definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pauth.h,v $
 * Revision 3.2  1995/11/17  17:48:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_PA_H_
#define _CSFC5_PA_H_  "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */


#ifndef csfC5_pauthType
typedef csf_ui32 csfC5_pauthType;
#endif

typedef struct _csfC5_pauth {
	csfC5_pauthType pa_type;
	csfC5_data pa_data;
	csf_o pa_data0[32];
} csfC5_pauth;

void csfC5clear_pauth( csfC5_pauth * );
void csfC5free_pauth( csfC5_pauth * );
void csfC5freeC_pauthS( csfC5_pauth *** );


csf_sts csfC5obtain_pauth (
	const csfC5_pauthType type,
	const csfC5_crsKBlock *key,
	csfC5_pauth *pauth
);

csf_sts csfC5verify_pauth (
	csfC5_pauth *data,
	const csfC5_crsKBlock *key,
	int *req_id,
	int *flags
);

#endif
