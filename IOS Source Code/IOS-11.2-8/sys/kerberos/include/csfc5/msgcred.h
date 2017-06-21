/* $Id: msgcred.h,v 3.2 1995/11/17 17:48:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/msgcred.h,v $
 *------------------------------------------------------------------
 * msgcred.h - Credentials message definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgcred.h,v $
 * Revision 3.2  1995/11/17  17:48:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_MSGCRED_H_
#define _CSFC5_MSGCRED_H_  "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */


typedef struct _csfC5_msgCred {
	csfC5_pVNo pvno;				/* Protocol version number. */
	csfC5_msgType msg_type;		/* Message type == CRED. */
	csfC5_tkt **tickets;			/* tickets */
	csfC5_crsEData ec_epart;		/* Encoded/encrypted part */
} csfC5_msgCred;


typedef struct _csfC5_msgCredInfo {
	csfC5_crsKBlock seskey;		/* session key used to encrypt ticket */
	csfC5_prnId client;				/* client name/realm, optional */
	csfC5_prnId server;				/* server name/realm, optional */
	csfC5_tktFlags flags;			/* ticket flags, optional */
	csfC5_tktTimes times;			/* auth, start, end, renew_till, optional */
	csfC5_netHAddr **caddrs;		/* client addresses */
} csfC5_msgCredInfo;


typedef struct _csfC5_msgCredEPart {
	csfC5_nonce nonce;					/* nonce, optional */
	csfC5_usTStamp ctime;				/* Client time. */
	csfC5_netHAddr s_haddr;			/* sender addresses, optional */
	csfC5_netHAddr r_haddr;			/* recipient addresses, optional */
	csfC5_msgCredInfo **tktinfo;
} csfC5_msgCredEPart;

void csfC5clear_msgCredEPart( csfC5_msgCredEPart * );
void csfC5free_msgCredEPart( csfC5_msgCredEPart * );

#endif
