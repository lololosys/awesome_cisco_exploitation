/* $Id: tkt.h,v 3.2 1995/11/17 17:48:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/tkt.h,v $
 *------------------------------------------------------------------
 * tkt.h - Kerberos ticket definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tkt.h,v $
 * Revision 3.2  1995/11/17  17:48:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_TKT_H_
#define _CSFC5_TKT_H_  "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#ifndef csfC5_tktVNo
typedef csf_ui32 csfC5_tktVNo;
#endif

#ifndef csfC5_tktFlags
typedef csf_ui32 csfC5_tktFlags;
#endif


typedef struct _csfC5_tktTimes {
	csfC5_tStamp tauth;
	csfC5_tStamp tstart;
	csfC5_tStamp tend;
	csfC5_tStamp trenew;
} csfC5_tktTimes;


typedef struct _csfC5_tktEPart {
	csfC5_tktFlags flags;			/* Flags */
	csfC5_crsKBlock seskey;		/* Session key. */
	csfC5_prnId client;			/* Client name/realm. */
	csfC5_trans trans;				/* Transited realms. */
	csfC5_tktTimes times;			/* Auth, start, end, renew times. */
	csfC5_netHAddr **caddrs;		/* Client addresses. */
	csfC5_authz **authz;			/* Authorization data. */
} csfC5_tktEPart;

void csfC5clear_tktEPart( csfC5_tktEPart * );
void csfC5free_tktEPart( csfC5_tktEPart * );
csf_sts csfC5copy_tktEPart( const csfC5_tktEPart *, csfC5_tktEPart * );

csf_sts csfC5rdEc_tktEPart (
	const csfC5_crsEData *in,
	const csfC5_crsKBlock *key,
	csfC5_tktEPart *out
);


typedef struct _csfC5_tkt {
	csfC5_tktVNo tktvno;			/* Ticket version number. */
	csfC5_prnId server;				/* Server principal id. */
	csfC5_tktEPart epart;			/* Encrypted part. */
	csfC5_crsEData ec_epart;		/* Encoded/encrypted epart. */
} csfC5_tkt;

void csfC5clear_tkt( csfC5_tkt * );
void csfC5free_tkt( csfC5_tkt * );
void csfC5freeC_tktS( csfC5_tkt *** );
csf_sts csfC5copy_tkt( const csfC5_tkt *, csfC5_tkt * );

csf_sts csfC5rdEc_tkt (
	const csfC5_data *in,
	const csfC5_crsKBlock *key,
	csfC5_tkt *out
);

#endif
