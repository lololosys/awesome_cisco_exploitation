/* $Id: rfc1510e.h,v 3.2 1995/11/17 17:48:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/rfc1510e.h,v $
 *------------------------------------------------------------------
 * rfc1510e.h - RFC1510 error code definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rfc1510e.h,v $
 * Revision 3.2  1995/11/17  17:48:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_RFC1510E_H_
#define _CSFC5_RFC1510E_H_ "%W% %G%"
/*.original errors.h 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */


/*
 * RFC defined error codes
 * These are used in KRB_ERROR protocol messages.
 *
 * These are NOT the return values of library routines, which use internally
 * defined status codes (specific to the local implementation).
 */

/*/TBD/JNK -- These are defined in the (generated) "csfC5err.h" file */
#if 0

/*
 * KDCERR -- KDC errors
 */

#define CSFC5c_STS_KDCERR_NONE					0	/* No error */
#define CSFC5c_STS_KDCERR_NAME_EXP				1	/* Client's entry in DB expired */
#define CSFC5c_STS_KDCERR_SERVICE_EXP			2	/* Server's entry in DB expired */
#define CSFC5c_STS_KDCERR_BAD_PVNO				3	/* Requested pvno not supported */
#define CSFC5c_STS_KDCERR_C_OLD_MAST_KVNO		4	/* C's key encrypted in old master */
#define CSFC5c_STS_KDCERR_S_OLD_MAST_KVNO		5	/* S's key encrypted in old master */
#define CSFC5c_STS_KDCERR_C_PRINCIPAL_UNKNOWN	6	/* Client not found in Kerberos DB */
#define CSFC5c_STS_KDCERR_S_PRINCIPAL_UNKNOWN	7	/* Server not found in Kerberos DB */
#define CSFC5c_STS_KDCERR_PRINCIPAL_NOT_UNIQUE	8	/* Multiple entries in Kerberos DB */
#define CSFC5c_STS_KDCERR_NULL_KEY				9	/* The C or S has a null key */
#define CSFC5c_STS_KDCERR_CANNOT_POSTDATE		10	/* Tkt ineligible for postdating */
#define CSFC5c_STS_KDCERR_NEVER_VALID			11	/* Requested starttime > endtime */
#define CSFC5c_STS_KDCERR_POLICY					12	/* KDC policy rejects request */
#define CSFC5c_STS_KDCERR_BADOPTION				13	/* KDC can't do requested opt. */
#define CSFC5c_STS_KDCERR_ETYPE_NOSUPP			14	/* No support for encryption type */
#define CSFC5c_STS_KDCERR_SUMTYPE_NOSUPP			15	/* No support for checksum type */
#define CSFC5c_STS_KDCERR_PADATA_TYPE_NOSUPP		16	/* No support for padata type */
#define CSFC5c_STS_KDCERR_TRTYPE_NOSUPP			17	/* No support for transited type */
#define CSFC5c_STS_KDCERR_CLIENT_REVOKED			18	/* C's creds have been revoked */
#define CSFC5c_STS_KDCERR_SERVICE_REVOKED		19	/* S's creds have been revoked */
#define CSFC5c_STS_KDCERR_TGT_REVOKED			20	/* TGT has been revoked */
#define CSFC5c_STS_KDCERR_CLIENT_NOTYET			21	/* C not yet valid */
#define CSFC5c_STS_KDCERR_SERVICE_NOTYET			22	/* S not yet valid */
#define CSFC5c_STS_KDCERR_KEY_EXP				23	/* Password has expired */
#define CSFC5c_STS_KDCERR_PREAUTH_FAILED			24	/* Preauthentication failed */
#define CSFC5c_STS_KDCERR_SERVER_NOMATCH			25	/* Requested server and */
													/*  ticket don't match*/

/*
 * 26-30 Reserved/unassigned
 */

/*
 * APERR -- Application errors
 */

#define CSFC5c_STS_APERR_BAD_INTEGRITY	31	/* Decrypt integrity check failed */
#define CSFC5c_STS_APERR_TKT_EXPIRED		32	/* Ticket expired */
#define CSFC5c_STS_APERR_TKT_NYV			33	/* Ticket not yet valid */
#define CSFC5c_STS_APERR_REPEAT			34	/* Request is a replay */
#define CSFC5c_STS_APERR_NOT_US			35	/* The ticket isn't for us */
#define CSFC5c_STS_APERR_BADMATCH		36	/* Ticket/authenticator don't match */
#define CSFC5c_STS_APERR_SKEW			37	/* Clock skew too great */
#define CSFC5c_STS_APERR_BADADDR			38	/* Incorrect net haddr */
#define CSFC5c_STS_APERR_BADVERSION		39	/* Protocol version mismatch */
#define CSFC5c_STS_APERR_MSG_TYPE		40	/* Invalid message type */
#define CSFC5c_STS_APERR_MODIFIED		41	/* Message stream modified */
#define CSFC5c_STS_APERR_BADORDER		42	/* Message out of order */
#define CSFC5c_STS_APERR_BADKEYVER		44	/* Key version is not available */
#define CSFC5c_STS_APERR_NOKEY			45	/* Service key not available */
#define CSFC5c_STS_APERR_MUT_FAIL		46	/* Mutual authentication failed */
#define CSFC5c_STS_APERR_BADDIRECTION	47	/* Incorrect message direction */
#define CSFC5c_STS_APERR_METHOD			48	/* Alternative authentication
												method required */
#define CSFC5c_STS_APERR_BADSEQ			49	/* Incorrect sequence numnber
												in message */
#define CSFC5c_STS_APERR_INAPP_CKSUM		50	/* Inappropriate type of
												checksum in message */


/*
 * 52-59 Reserved/unassigned
 */

/*
 * ERR -- Other errors
 */

#define KRBc_STS_ERR_GENERIC			60  /* Generic error (description
												in e-text) */
#define KRBc_STS_ERR_FIELD_TOOLONG		61  /* Field is too long for
												implementation. */

/*
 * 62-127 Reserved/unassigned
 */

#endif

#endif /* _CSFC5_RFC1510E_H_ */
