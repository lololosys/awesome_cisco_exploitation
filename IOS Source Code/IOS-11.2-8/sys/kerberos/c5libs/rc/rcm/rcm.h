/* $Id: rcm.h,v 3.2 1995/11/17 17:45:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/rc/rcm/rcm.h,v $
 *------------------------------------------------------------------
 * rcm.h - In-memory replay cache definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rcm.h,v $
 * Revision 3.2  1995/11/17  17:45:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_LIB_RCF_H_
#define _CSFC5_LIB_RCF_H_ "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * All rights reserved.
 */

#include <csfc5/header.h>
#include <csfc5/rc/rc0.h>

#if CSFC5_CFG_RC_MEM


#ifndef CSFC5_CFG_RCM_HASHSIZE
#define CSFC5_CFG_RCM_HASHSIZE CSFC5_CFG_RC_HASHSIZE
#endif


typedef struct _csfC5_rcMBucket {
	struct _csfC5_rcMBucket *next;
	csfC5_rcEntry rep;
} csfC5_rcMBucket;

typedef struct _csfC5_rcMData {
	csfC5_rcBase *rc0;
	int hsize;
	int numhits;
	int nummisses;
	csfC5_rcMBucket **buckets;
} csfC5_rcMData;

csf_sts csfC5resolve_rcM ( csfC5_rcBase **, const csfC5_rcOpsTab *, const char * );
csf_sts csfC5initialize_rcM ( csfC5_rcBase *, csfC5_tDelta );
csf_sts csfC5recover_rcM ( csfC5_rcBase * );
csf_sts csfC5destroy_rcM ( csfC5_rcBase * );
csf_sts csfC5close_rcM ( csfC5_rcBase * );
csf_sts csfC5storeEntry_rcM ( csfC5_rcBase *, csfC5_rcEntry * );
csf_sts csfC5expunge_rcM ( csfC5_rcBase * );

#endif /* CSFC5_CFG_RC_MEM */

#endif /* _CSFC5_LIB_RCF_H_ */
