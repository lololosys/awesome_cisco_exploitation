/* $Id: rc0.h,v 3.2 1995/11/17 17:49:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/rc/rc0.h,v $
 *------------------------------------------------------------------
 * rc0.h - Replay cache private definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rc0.h,v $
 * Revision 3.2  1995/11/17  17:49:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_RC_RC0_H_
#define _CSFC5_RC_RC0_H_ "%W% %G%"
/*.original rcache.h 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Replay cache (RC) private definitions.
 */

#ifndef CSFC5_CFG_RC_NULL
#define CSFC5_CFG_RC_NULL		0
#endif

#ifndef CSFC5_CFG_RC_MEM
#define CSFC5_CFG_RC_MEM		0
#endif

#ifndef CSFC5_CFG_RC_FILE
#define CSFC5_CFG_RC_FILE		0
#endif


typedef struct _csfC5_rcEntry {
	csfC5_tStamp rtime;
	csfMD5_cBlk rchk;
} csfC5_rcEntry;

void csfC5free_rcEntry ( csfC5_rcEntry * );
void csfC5clear_rcEntry ( csfC5_rcEntry * );
csf_sts csfC5copy_rcEntry ( const csfC5_rcEntry *, csfC5_rcEntry * );
csf_boolean csfC5isEq_rcEntry ( const csfC5_rcEntry *, const csfC5_rcEntry * );


csf_sts csfC5make_rcEntry (
	csfC5_tStamp rtime,
	const csfC5_data *rdata,
	csfC5_rcEntry *rce
);

csf_boolean csfC5isAlive_rcEntry (
	csfC5_rcEntry *rce,
	csfC5_tDelta t
);

unsigned int csfC5hash_rcEntry (
	csfC5_rcEntry *rce,
	int hsize
);


/* Hash size determines size of hash table allocated for
 * maintaining replay entries.
 */
#ifndef CSFC5_CFG_RC_HASHSIZE
#define CSFC5_CFG_RC_HASHSIZE 997	/* A convenient prime. */
#endif


/* rc_base -- Base data associated with all replay cache types. */
typedef struct csfC5_rcBase {
	csfC5_rc rc;				/* Visible part. */
	csfC5_rcType tidx;			/* Index into internal type table. */
	struct _csfC5_rcOpsTab		/* Ops table this came from. */
		*rcops;
	void *data;				/* Type-specific data. */
	csf_sPad xxx0;				/* Pad. */
} csfC5_rcBase;


typedef struct _csfC5_rcOpsTab {
	csfC5_rcType rctype;
	char *typename;
	csf_sts (*resolve) ( csfC5_rcBase **, const struct _csfC5_rcOpsTab *, const char * );
	csf_sts (*initialize) ( csfC5_rcBase *, csfC5_tDelta );
	csf_sts (*recover) ( csfC5_rcBase * );
	csf_sts (*destroy) ( csfC5_rcBase * );
	csf_sts (*close) ( csfC5_rcBase * );
	csf_sts (*putEntry) ( csfC5_rcBase *, csfC5_tStamp, const csfC5_data * );
	csf_sts (*purge) ( csfC5_rcBase * );
	csf_sts (*flush) ( csfC5_rcBase * );
	/* Internal use only. */
	csf_sts (*storeEntry) ( csfC5_rcBase *, csfC5_rcEntry * );
} csfC5_rcOpsTab;

csf_sts csfC5putEntryDEF_rc ( csfC5_rcBase *, csfC5_tStamp, const csfC5_data * );

csf_sts csfC5initializeNOP_rc ( csfC5_rcBase *, csfC5_tDelta );
csf_sts csfC5recoverNOP_rc ( csfC5_rcBase * );
csf_sts csfC5destroyNOP_rc ( csfC5_rcBase * );
csf_sts csfC5closeNOP_rc ( csfC5_rcBase * );
csf_sts csfC5storeEntryNOP_rc ( csfC5_rcBase *, csfC5_rcEntry * );
csf_sts csfC5putEntryNOP_rc ( csfC5_rcBase *, csfC5_tStamp, const csfC5_data * );
csf_sts csfC5purgeNOP_rc ( csfC5_rcBase * );
csf_sts csfC5flushNOP_rc ( csfC5_rcBase * );

csf_sts csfC5initializeNOT_rc ( csfC5_rcBase *, csfC5_tDelta );
csf_sts csfC5recoverNOT_rc ( csfC5_rcBase * );
csf_sts csfC5destroyNOT_rc ( csfC5_rcBase * );
csf_sts csfC5closeNOT_rc ( csfC5_rcBase * );
csf_sts csfC5storeEntryNOT_rc ( csfC5_rcBase *, csfC5_rcEntry * );
csf_sts csfC5putEntryNOT_rc ( csfC5_rcBase *, csfC5_tStamp, const csfC5_data * );
csf_sts csfC5purgeNOT_rc ( csfC5_rcBase * );
csf_sts csfC5flushNOT_rc ( csfC5_rcBase * );

#endif /* _CSFC5_RC_RC0_H_ */
