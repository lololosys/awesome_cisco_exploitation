/* $Id: creds.h,v 3.2 1995/11/17 17:47:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/creds.h,v $
 *------------------------------------------------------------------
 * creds.h - Credentials definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: creds.h,v $
 * Revision 3.2  1995/11/17  17:47:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_CREDS_H_
#define _CSFC5_CREDS_H_  "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */


typedef struct _csfC5_creds {
	csfC5_prnId client;		/* Client principal id. */
	csfC5_prnId server;		/* Server principal id. */
	csfC5_crsKBlock seskey;	/* Session key. */
	csfC5_tktTimes times;		/* Ticket lifetime info. */
	csf_boolean is_skey;		/* Ticket encrypted in anothers's skey? */
	csfC5_tktFlags tktflags;	/* Ticket flags (from KDC reply). */
	csfC5_netHAddr **haddrs;	/* Addrs in ticket. */
	csfC5_data e_tkt;			/* Encoded ticket. */
	csfC5_data e_tkt2;			/* Encoded second ticket. */
	csfC5_authz **authz;		/* Authorization data. */
} csfC5_creds;

void csfC5clear_creds( csfC5_creds * );
void csfC5free_creds( csfC5_creds * );
void csfC5freeC_credsS( csfC5_creds *** );
csf_sts csfC5copy_creds( const csfC5_creds *, csfC5_creds * );
csf_sts csfC5copyC_creds( const csfC5_creds *, csfC5_creds ** );

typedef struct _csfC5_credsReqCtx {
	const csfC5_creds *req;		/* Requested credentials. */
	const csfC5_creds *use;		/* Credentials to use. */
	const csfC5_prnId *client;
	const csfC5_prnId *server;
	const csfC5_crsKBlock *key;
	csfC5_crsEType etype;
	csfC5_crsKType ktype;			/* Unused */
	csfC5_cksType ckstype;
	csfC5_nonce nonce;
	csfC5_crsKBlock key0;
} csfC5_credsReqCtx;

void csfC5clear_credsReqCtx( csfC5_credsReqCtx * );

csf_sts csfC5setup_credsReqCtx (
	csfC5_credsReqCtx *rcreds
);

#endif
