/* $Id: autht.h,v 3.2 1995/11/17 17:47:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/autht.h,v $
 *------------------------------------------------------------------
 * autht.h - Authenticator definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: autht.h,v $
 * Revision 3.2  1995/11/17  17:47:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_AUTHT_H_
#define _CSFC5_AUTHT_H_  "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#ifndef csfC5_authtVNo
typedef csf_ui32 csfC5_authtVNo;
#endif

/* the unencrypted version */
typedef struct _csfC5_autht {
	csfC5_authtVNo authtvno;	/* Authenticator version number. */
	csfC5_prnId client;		/* Client name/realm */
	csfC5_cksBlock cks;		/* Checksum */
	csfC5_usTStamp ctime;		/* Client time. */
	csfC5_crsKBlock subkey;	/* Session sub-key */
	csfC5_seqNo seqno;			/* Sequence number */
	csfC5_authz **authz;		/* Authorization data.  */
} csfC5_autht;

void csfC5clear_autht( csfC5_autht * );
void csfC5free_autht( csfC5_autht * );

csf_sts csfC5rdEc_autht (
	const csfC5_crsEData *edata,
	const csfC5_crsKBlock *key,
	void *ivec,
	csfC5_autht *out
);


csf_sts csfC5mkEc_autht (
	const csfC5_autht *autht,
	const csfC5_crsKBlock *key,
	void *ivec,
	csfC5_crsEData *edata
);

#endif
