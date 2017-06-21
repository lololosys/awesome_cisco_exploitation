/* $Id: cc.h,v 3.2 1995/11/17 17:47:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/cc.h,v $
 *------------------------------------------------------------------
 * cc.h - Credentials cache public definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cc.h,v $
 * Revision 3.2  1995/11/17  17:47:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_CC_H_
#define _CSFC5_CC_H_ "%W% %G%"
/*.original ccache.h 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Credentials cache (CC) public definitions.
 */

#ifndef _CSFC5_CC_STS_HS_
#define _CSFC5_CC_STS_HS_
#include <csfc5/sts/cc_sts.hs>
#endif

#ifndef CSFC5_CC_TYPE_NULL
#define CSFC5_CC_TYPE_NULL		1
#endif

#ifndef CSFC5_CC_TYPE_MEM
#define CSFC5_CC_TYPE_MEM		2
#endif

#ifndef CSFC5_CC_TYPE_FILE
#define CSFC5_CC_TYPE_FILE		3
#endif

#ifndef csfC5_ccType
typedef csf_i16 csfC5_ccType;
#endif

/* cc -- Visible portion of credentials cache. */
typedef struct _csfC5_cc {
	const char *name;			/* Name */
	const char *tname;			/* Type name */
	csfC5_ccType type;			/* Type. */
	csf_flags oflags;			/* Open flags. */
	csf_flags iflags;			/* Information flags. */
	csfC5_prnId prnid;			/* Default principal id. */
} csfC5_cc;

/* cursor for sequential lookup */
typedef void *csfC5_ccCursor;

csf_sts csfC5scan_cc (
	csfC5_cc *cc,					/* Cred cache to scan. */
	csf_sts (*ccscanf) (			/* Scan function. */
		const csfC5_creds *creds,	/* Current creds from cache. */
		void *arg					/* "scanarg". */
	),
	void *scanarg					/* Argument to pass to "ccscanf". */
);

/*TBD/JNK--open_cc -- replace resolve, initialize*/
#define CSFC5_CC_OPTS_CREATE		0x0001
#define CSFC5_CC_OPTS_CREATEIF		0x0002
#define CSFC5_CC_OPTS_WRITE			0x0004
#define CSFC5_CC_OPTS_TEMP			0x0008
#define CSFC5_CC_OPTS_LOCK			0x0010
#define CSFC5_CC_OPTS_MATCHPR		0x0020

typedef struct _csfC5_ccOpts {
	csfC5_prnId *prnid;
} csfC5_ccOpts;

csf_sts csfC5open_cc (
	csfC5_cc **cc,
	const char *name,
	csf_flags opts,
	const csfC5_ccOpts *ccopts
);

csf_sts csfC5resolve_cc (
	csfC5_cc **cc,
	const char *name
);

csf_sts csfC5close_cc (
	csfC5_cc **cc
);

csf_sts csfC5destroy_cc (
	csfC5_cc **cc
);

csf_sts csfC5initialize_cc (
	csfC5_cc *cc,
	const csfC5_prnId *prnid
);

csf_sts csfC5deleteCreds_cc (
	csfC5_cc *cc,
	csfC5_ccCursor *curs
);

/* for set_flags */
#define CSFC5_CC_OPTS_OPENCLOSE_m			0x00000001

csf_sts csfC5setOpts_cc (
	csfC5_cc *cc,
	csf_flags ccopts
);

csf_sts csfC5refInfo_cc (
	csfC5_cc *cc
);

csf_sts csfC5addCreds_cc (
	csfC5_cc *cc,
	const csfC5_creds *creds
);

csf_sts csfC5startGetCreds_cc (
	csfC5_cc *cc,
	csfC5_ccCursor *curs
);

csf_sts csfC5endGetCreds_cc (
	csfC5_cc *cc,
	csfC5_ccCursor *curs
);

csf_sts csfC5nextGetCreds_cc (
	csfC5_cc *cc,
	csfC5_ccCursor *curs,
	csfC5_creds *creds
);

#endif /* _CSFC5_CC_H_ */
