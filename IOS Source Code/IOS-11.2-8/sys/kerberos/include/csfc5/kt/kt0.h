/* $Id: kt0.h,v 3.2 1995/11/17 17:49:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/kt/kt0.h,v $
 *------------------------------------------------------------------
 * kt0.h - Keytab private definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: kt0.h,v $
 * Revision 3.2  1995/11/17  17:49:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_KT0_H_
#define _CSFC5_KT0_H_ "%W% %G%"
/*.original keytab.h 3.1 12/29/93*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Key table (KT) private definitions
 */

#ifndef CSFC5_CFG_KT_NULL
#define CSFC5_CFG_KT_NULL 0
#endif

#ifndef CSFC5_CFG_KT_MEM
#define CSFC5_CFG_KT_MEM 0
#endif

#ifndef CSFC5_CFG_KT_FILE
#define CSFC5_CFG_KT_FILE 0
#endif

#ifndef CSFC5_CFG_KT_WFILE
#define CSFC5_CFG_KT_WFILE 0
#endif

typedef struct _csfC5_ktBase {
	csfC5_kt kt;				/* Visible portion. */
	csfC5_ktType tidx;			/* Index into internal type table. */
	struct _csfC5_ktOpsTab		/* Ops table this came from. */
		 *ktops;
	void *data;				/* Type-specific data. */
	csf_ui32 xxx0;				/* Pad. */
} csfC5_ktBase;

typedef struct _csfC5_ktOpsTab {
	csfC5_ktType kttype;
	char *prefix;
	csf_sts (*resolve)
		( csfC5_ktBase **, const struct _csfC5_ktOpsTab *, const char * );
	csf_sts (*close) ( csfC5_ktBase * );
	csf_sts (*startGetEntry) ( csfC5_ktBase *, csfC5_ktCursor * );
	csf_sts (*nextGetEntry) ( csfC5_ktBase *, csfC5_ktCursor *, csfC5_ktEntry * );
	csf_sts (*endGetEntry) ( csfC5_ktBase *, csfC5_ktCursor * );
	csf_sts (*create) ( csfC5_ktBase * );
	csf_sts (*addEntry) ( csfC5_ktBase *, const csfC5_ktEntry * );
	csf_sts (*removeEntry) ( csfC5_ktBase *, const csfC5_ktEntry * );
} csfC5_ktOpsTab;

csf_sts csfC5closeNOP_kt ( csfC5_ktBase * );
csf_sts csfC5startGetEntryNOP_kt ( csfC5_ktBase *, csfC5_ktCursor * );
csf_sts csfC5nextGetEntryNOP_kt ( csfC5_ktBase *, csfC5_ktCursor *, csfC5_ktEntry * );
csf_sts csfC5endGetEntryNOP_kt ( csfC5_ktBase *, csfC5_ktCursor * );
csf_sts csfC5createNOP_kt ( csfC5_ktBase * );
csf_sts csfC5addEntryNOP_kt ( csfC5_ktBase *, const csfC5_ktEntry * );
csf_sts csfC5removeEntryNOP_kt ( csfC5_ktBase *, const csfC5_ktEntry * );

csf_sts csfC5closeNOT_kt ( csfC5_ktBase * );
csf_sts csfC5startGetEntryNOT_kt ( csfC5_ktBase *, csfC5_ktCursor * );
csf_sts csfC5nextGetEntryNOT_kt ( csfC5_ktBase *, csfC5_ktCursor *, csfC5_ktEntry * );
csf_sts csfC5endGetEntryNOT_kt ( csfC5_ktBase *, csfC5_ktCursor * );
csf_sts csfC5createNOT_kt ( csfC5_ktBase * );
csf_sts csfC5addEntryNOT_kt ( csfC5_ktBase *, const csfC5_ktEntry * );
csf_sts csfC5removeEntryNOT_kt ( csfC5_ktBase *, const csfC5_ktEntry * );

#endif /* _CSFC5_KT0_H_ */
