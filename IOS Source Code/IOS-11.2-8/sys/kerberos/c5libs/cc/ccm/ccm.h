/* $Id: ccm.h,v 3.2 1995/11/17 17:43:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/cc/ccm/ccm.h,v $
 *------------------------------------------------------------------
 * cmm.h - In-memory credentials cache definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ccm.h,v $
 * Revision 3.2  1995/11/17  17:43:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_LIB_CCM_H_
#define _CSFC5_LIB_CCM_H_ "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#include <csfc5/header.h>
#include <csfc5/cc/cc0.h>

#if CSFC5_CFG_CC_MEM

/* ccm_creds -- For the moment, the same as a normal
 * credential; in the future we may want to pack these.
 */
typedef csfC5_creds csfC5_ccMCreds;


/* ccm_credslist -- Each credential is pointed to by one of these;
 * these are kept in a linked list.
 */
typedef struct _csfC5_ccMCredsList {
	struct _csfC5_ccMCredsList *next;		/* Next in list; NULL if none. */
	csfC5_ccBase *ccache;					/* Where we came from. For debug.. */
											/* ..and consistency checking. */
	csfC5_creds creds;						/* Credentials. */
} _csfC5_ccMCredsList;


/* ccm-specific data */
typedef struct _csfC5_ccMData {
	csfC5_ccBase *cc0;
	csfC5_prnId prnid;				/* Default principal id for these creds. */
	_csfC5_ccMCredsList *crlist;	/* List head of credentials. */
} csfC5_ccMData;

/* cursor for positioning operations */
typedef struct _csfC5_ccMCursor {
	_csfC5_ccMCredsList *crlentry;	/* Next credslist entry. */
} csfC5_ccMCursor;

csf_sts csfC5resolve_ccM
	( csfC5_ccBase **id, const csfC5_ccOpsTab *, const char *residual );

csf_sts csfC5initialize_ccM
	( csfC5_ccBase *id, const csfC5_prnId *prnid );

csf_sts csfC5close_ccM
	( csfC5_ccBase *id );

csf_sts csfC5destroy_ccM
	( csfC5_ccBase *id );

csf_sts csfC5startGetCreds_ccM
	( csfC5_ccBase *id, csfC5_ccCursor *cursor );

csf_sts csfC5nextGetCreds_ccM
	( csfC5_ccBase *id, csfC5_ccCursor *cursor, csfC5_creds *creds );

csf_sts csfC5endGetCreds_ccM
	( csfC5_ccBase *id, csfC5_ccCursor *cursor );

csf_sts csfC5addCreds_ccM
	( csfC5_ccBase *id, const csfC5_creds *creds );

#endif /* CSFC5_CFG_CC_MEM */

#endif /* _CSFC5_LIB_CCM_H_ */
