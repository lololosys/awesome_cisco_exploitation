/* $Id: msgsp.h,v 3.2 1995/11/17 17:48:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/msgsp.h,v $
 *------------------------------------------------------------------
 * msgsp.h - SP message definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgsp.h,v $
 * Revision 3.2  1995/11/17  17:48:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_MSGSP_H_
#define _CSFC5_MSGSP_H_  "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

typedef struct _csfC5_msgSPBody {
	csfC5_data user_data;		/* user data */
	csfC5_usTStamp ctime;		/* Client time. */
	csfC5_seqNo seqno;			/* sequence #, optional */
	csfC5_netHAddr s_haddr;	/* sender haddr */
	csfC5_netHAddr r_haddr;	/* recipient haddr, optional */
} csfC5_msgSPBody;

void csfC5clear_msgSPBody( csfC5_msgSPBody * );
void csfC5free_msgSPBody( csfC5_msgSPBody * );


typedef struct _csfC5_msgSafe {
	csfC5_pVNo pvno;				/* Protocol version number. */
	csfC5_msgType msg_type;		/* Message type == SAFE. */
	csfC5_msgSPBody body;			/* Protected part. */
	csfC5_cksBlock cks;			/* Body checksum. */
	csfC5_data e_body;				/* Encoded body. */
} csfC5_msgSafe;
typedef csfC5_msgSPBody csfC5_msgSafeBody;

void csfC5clear_msgSafe( csfC5_msgSafe * );
void csfC5free_msgSafe( csfC5_msgSafe * );

csf_sts csfC5mkEc_msgSafe (
	csfC5_msgSafe *safemsg,
	const csfC5_data *key,
	csfC5_data *out
);

csf_sts csfC5rdEc_msgSafe (
	const csfC5_data *in,
	const csfC5_data *key,
	csfC5_msgSafe *safemsg
);


typedef struct _csfC5_msgPriv {
	csfC5_pVNo pvno;				/* Protocol version number. */
	csfC5_msgType msg_type;		/* Message type == PRIV. */
	csfC5_msgSPBody epart;			/* Encrypted part. */
	csfC5_crsEData ec_epart;		/* Encoded/encrypted epart */
} csfC5_msgPriv;
typedef csfC5_msgSPBody csfC5_msgPrivEPart;

void csfC5clear_msgPriv( csfC5_msgPriv * );
void csfC5free_msgPriv( csfC5_msgPriv * );

csf_sts csfC5rdEc_msgPriv (
	const csfC5_data *inbuf,
	const csfC5_crsKBlock *key,
	void *ivec,
	csfC5_msgPriv *out
);

csf_sts csfC5mkEc_msgPriv (
	csfC5_msgPriv *in,
	const csfC5_crsKBlock *key,
	void *ivec,
	csfC5_data *outbuf
);

csf_sts csfC5rdEc_msgPrivEPart (
	const csfC5_crsEData *edata,
	const csfC5_crsKBlock *key,
	void *ivec,
	csfC5_msgSPBody *out
);

csf_sts csfC5mkEc_msgPrivEPart (
	const csfC5_msgSPBody *in,
	const csfC5_crsKBlock *key,
	void *ivec,
	csfC5_crsEData *edata
);

#endif
