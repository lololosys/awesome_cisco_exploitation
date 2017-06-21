/* $Id: msgap.h,v 3.2 1995/11/17 17:48:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/msgap.h,v $
 *------------------------------------------------------------------
 * msgap.h - AP message definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgap.h,v $
 * Revision 3.2  1995/11/17  17:48:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_MSGAP_H_
#define _CSFC5_MSGAP_H_  "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */


typedef struct _csfC5_msgApReq {
	csfC5_pVNo pvno;				/* Protocol version number. */
	csfC5_msgType msg_type;		/* Message type == AP_REQ. */
	csfC5_tktFlags ap_opts;		/* requested options */
	csfC5_tkt tkt;					/* ticket */
	csfC5_data e_tkt;				/* Encoded (encrypted) ticket. */
	csfC5_autht autht;				/* Authenticator. */
	csfC5_crsEData ec_autht;		/* Encrypted/encoded authenticator */
} csfC5_msgApReq;

void csfC5clear_msgApReq( csfC5_msgApReq * );
void csfC5free_msgApReq( csfC5_msgApReq * );

csf_sts csfC5rd_msgApReq (
	const csfC5_data *,
	csfC5_msgApReq *
);

csf_sts csfC5rdEc_msgApReq (
	const csfC5_crsKBlock *,
	csfC5_msgApReq *
);

csf_sts csfC5mk_msgApReq (
	const csfC5_msgApReq *,
	csfC5_data *
);


typedef struct _csfC5_msgApRepEPart {
	csfC5_usTStamp ctime;			/* Client time. */
	csfC5_crsKBlock subkey;		/* Session sub-key, optional */
	csfC5_seqNo seqno;				/* sequence #, optional */
} csfC5_msgApRepEPart;

void csfC5clear_msgApRepEPart( csfC5_msgApRepEPart * );
void csfC5free_msgApRepEPart( csfC5_msgApRepEPart * );

csf_sts csfC5mkEc_msgApRepEPart (
	const csfC5_msgApRepEPart *,
	const csfC5_crsKBlock *,
	csfC5_crsEData *
);

csf_sts csfC5rdEc_msgApRepEPart (
	const csfC5_crsEData *,
	const csfC5_crsKBlock *,
	csfC5_msgApRepEPart *
);


typedef struct _csfC5_msgApRep {
	csfC5_pVNo pvno;				/* Protocol version number. */
	csfC5_msgType msg_type;		/* Message type == AP_REP. */
	csfC5_msgApRepEPart epart;		/* Encrypted part. */
	csfC5_crsEData ec_epart;		/* Encoded/encrypted epart. */
} csfC5_msgApRep;

void csfC5clear_msgApRep( csfC5_msgApRep * );
void csfC5free_msgApRep( csfC5_msgApRep * );

csf_sts csfC5mkEc_msgApRep (
	csfC5_msgApRep *,
	const csfC5_crsKBlock *,
	csfC5_data *
);

csf_sts csfC5rdEc_msgApRep (
	const csfC5_data *,
	const csfC5_crsKBlock *,
	csfC5_msgApRep *
);

csf_sts csfC5rd_msgApRep (
	const csfC5_data *,
	csfC5_msgApRep *
);

#endif
