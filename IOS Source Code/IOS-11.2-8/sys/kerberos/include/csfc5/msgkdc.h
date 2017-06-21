/* $Id: msgkdc.h,v 3.2 1995/11/17 17:48:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/msgkdc.h,v $
 *------------------------------------------------------------------
 * msgkdc.h - KDC request message definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgkdc.h,v $
 * Revision 3.2  1995/11/17  17:48:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_MSGKDC_H_
#define _CSFC5_MSGKDC_H_  "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

typedef struct _csfC5_msgKdcReqBody {
	csfC5_tktFlags kdc_options;	/* Options */
	csfC5_prnId client;			/* includes realm; optional */
	csfC5_prnId server;			/* includes realm (only if no client) */
	csfC5_tStamp tstart;			/* Start time */
	csfC5_tStamp tend;				/* End time */
	csfC5_tStamp trenew;			/* Renew time. */
	csfC5_nonce nonce;				/* Nonce to match request/response */
	csf_size_t netypes;			/* # of etypes */
	csfC5_crsEType *etypes;		/* Encryption type(s) */
	csfC5_netHAddr **haddrs;		/* Host addresses. */
	csfC5_authz **authz;			/* Authorization data. */
	csfC5_crsEData ec_authz;		/* Encoded/encrypted auth data. */
	csfC5_tkt **tkt2;				/* Second ticket array. */
	csfC5_data e_tkt2;				/* Encoded second ticket array. */
} csfC5_msgKdcReqBody;

void csfC5clear_msgKdcReqBody( csfC5_msgKdcReqBody * );
void csfC5free_msgKdcReqBody( csfC5_msgKdcReqBody * );

typedef struct _csfC5_msgKdcReq {
	csfC5_pVNo pvno;				/* Protocol version number. */
	csfC5_msgType msg_type;		/* AS_REQ or TGS_REQ? */
	csfC5_pauth **padata;			/* e.g. encoded AP_REQ */
	csfC5_msgKdcReqBody body;		/* Request body. */
	csfC5_data e_body;				/* Encoded body. */
} csfC5_msgKdcReq;

void csfC5clear_msgKdcReq( csfC5_msgKdcReq * );
void csfC5free_msgKdcReq( csfC5_msgKdcReq * );


typedef struct _csfC5_msgKdcRepEPart {	/* encrypted part: */
	csfC5_crsKBlock seskey;			/* session key */
	csfC5_lReq **lreq;					/* array of ptrs to entries */
	csfC5_nonce nonce;					/* nonce from request */
	csfC5_tStamp key_exp;				/* expiration date */
	csfC5_tktFlags flags;				/* ticket flags */
	csfC5_tktTimes times;				/* lifetime info */
	csfC5_prnId server;				/* server's principal identifier */
	csfC5_netHAddr **caddrs;			/* client addresses, optional */
} csfC5_msgKdcRepEPart;

void csfC5clear_msgKdcRepEPart( csfC5_msgKdcRepEPart * );
void csfC5free_msgKdcRepEPart( csfC5_msgKdcRepEPart * );

csf_sts csfC5rdEc_msgKdcRepEPart (
	const csfC5_crsEData *in,
	const csfC5_crsKBlock *kblock,
	csfC5_msgKdcRepEPart *out
);

typedef struct _csfC5_msgKdcRep {
	csfC5_pVNo pvno;				/* Protocol version number. */
	csfC5_msgType msg_type;		/* AS_REP or TGS_REP? */
	csfC5_pauth **padata;			/* preauthentication data from KDC */
	csfC5_prnId client;			/* client */
	csfC5_tkt tkt;					/* Ticket */
	csfC5_data e_tkt;				/* Encoded ticket. */
	csfC5_msgKdcRepEPart epart;	/* Encrypted part.  */
	csfC5_data e_epart;			/* Encoded epart. */
	csfC5_crsEData ec_epart;		/* Encoded/encrypted epart. */
} csfC5_msgKdcRep;

void csfC5clear_msgKdcRep( csfC5_msgKdcRep * );
void csfC5free_msgKdcRep( csfC5_msgKdcRep * );

csf_sts csfC5rdEc_msgTgsRep (
	csfC5_data *in,
	const csfC5_crsKBlock *key,
	csfC5_msgKdcRep *out
);

csf_sts csfC5rdEc_msgAsRep (
	csfC5_data *in,
	const csfC5_crsKBlock *key,
	csfC5_msgKdcRep *out
);

#endif
