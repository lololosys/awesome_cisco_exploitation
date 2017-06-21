/* $Id: cc0.h,v 3.2 1995/11/17 17:49:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/cc/cc0.h,v $
 *------------------------------------------------------------------
 * cc0.h - Credentials cache private definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cc0.h,v $
 * Revision 3.2  1995/11/17  17:49:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_CC_CC0_H_
#define _CSFC5_CC_CC0_H_ "%W% %G%"
/*.original ccache.h 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Credentials cache (CC) private definitions.
 */

#ifndef CSFC5_CFG_CC_NULL
#define CSFC5_CFG_CC_NULL 0
#endif

#ifndef CSFC5_CFG_CC_MEM
#define CSFC5_CFG_CC_MEM 0
#endif

#ifndef CSFC5_CFG_CC_FILE
#define CSFC5_CFG_CC_FILE 0
#endif

/* cc_base -- Base data associated with all creds cache types. */
typedef struct _csfC5_ccBase {
	csfC5_cc cc;				/* Visible part. */
	csfC5_ccType 	tidx;		/* Index into internal type table. */
	struct _csfC5_ccOpsTab		/* Ops table this came from. */
		 *ccops;
	csf_flags iflags;			/* Internal flags. */
	void *data;				/* Type-specific data. */
	csf_sPad xxx0;				/* Pad. */
} csfC5_ccBase;

typedef struct _csfC5_ccOpsTab {
	csfC5_ccType cctype;
	char *prefix;
	csf_sts (*resolve) (csfC5_ccBase **, const struct _csfC5_ccOpsTab *,
		const char * );
	csf_sts (*open) (csfC5_ccBase **, const struct _csfC5_ccOpsTab *,
		const char *, csf_flags, const csfC5_ccOpts * );
	csf_sts (*close) (csfC5_ccBase *);
	csf_sts (*initialize) (csfC5_ccBase *, const csfC5_prnId *);
	csf_sts (*destroy) (csfC5_ccBase *);
	csf_sts (*addCreds) (csfC5_ccBase *, const csfC5_creds *);
	csf_sts (*startGetCreds) (csfC5_ccBase *, csfC5_ccCursor *);
	csf_sts (*nextGetCreds) (csfC5_ccBase *, csfC5_ccCursor *, csfC5_creds *);
	csf_sts (*endGetCreds) (csfC5_ccBase *, csfC5_ccCursor *);
	csf_sts (*deleteCreds) (csfC5_ccBase *, csfC5_ccCursor *);
	csf_sts (*refInfo) (csfC5_ccBase *);
	csf_sts (*setOpts) (csfC5_ccBase *, csf_flags);
} csfC5_ccOpsTab;

csf_sts csfC5closeNOP_cc ( csfC5_ccBase * );
csf_sts csfC5initializeNOP_cc ( csfC5_ccBase *, const csfC5_prnId * );
csf_sts csfC5destroyNOP_cc ( csfC5_ccBase * );
csf_sts csfC5refInfoNOP_cc ( csfC5_ccBase *);
csf_sts csfC5addCredsNOP_cc ( csfC5_ccBase *, const csfC5_creds * );
csf_sts csfC5deleteCredsNOP_cc ( csfC5_ccBase *, csfC5_ccCursor * );
csf_sts csfC5startGetCredsNOP_cc ( csfC5_ccBase *, csfC5_ccCursor * );
csf_sts csfC5nextGetCredsNOP_cc ( csfC5_ccBase *, csfC5_ccCursor *, csfC5_creds * );
csf_sts csfC5endGetCredsNOP_cc ( csfC5_ccBase *, csfC5_ccCursor * );
csf_sts csfC5setOptsNOP_cc ( csfC5_ccBase *, csf_flags );

csf_sts csfC5closeNOT_cc ( csfC5_ccBase * );
csf_sts csfC5initializeNOT_cc ( csfC5_ccBase *, const csfC5_prnId * );
csf_sts csfC5destroyNOT_cc ( csfC5_ccBase * );
csf_sts csfC5refInfoNOT_cc ( csfC5_ccBase *);
csf_sts csfC5addCredsNOT_cc ( csfC5_ccBase *, const csfC5_creds * );
csf_sts csfC5deleteCredsNOT_cc ( csfC5_ccBase *, csfC5_ccCursor * );
csf_sts csfC5startGetCredsNOT_cc ( csfC5_ccBase *, csfC5_ccCursor * );
csf_sts csfC5nextGetCredsNOT_cc ( csfC5_ccBase *, csfC5_ccCursor *, csfC5_creds * );
csf_sts csfC5endGetCredsNOT_cc ( csfC5_ccBase *, csfC5_ccCursor * );
csf_sts csfC5setOptsNOT_cc ( csfC5_ccBase *, csf_flags );

#endif /* _CSFC5_CC_CC0_H_ */
