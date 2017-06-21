/* $Id: rc.h,v 3.2 1995/11/17 17:48:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/rc.h,v $
 *------------------------------------------------------------------
 * rc.h - Replay cache public definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rc.h,v $
 * Revision 3.2  1995/11/17  17:48:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_RC_H_
#define _CSFC5_RC_H_ "%W% %G%"
/*.original rcache.h 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Replay cache (RC)
 * Public definitions.
 */

#ifndef _CSFC5_RC_STS_H_
#define _CSFC5_RC_STS_H_
#include <csfc5/sts/rc_sts.hs>
#endif

#ifndef CSFC5_RC_TYPE_NULL
#define CSFC5_RC_TYPE_NULL		1
#endif

#ifndef CSFC5_RC_TYPE_MEM
#define CSFC5_RC_TYPE_MEM		2
#endif

#ifndef CSFC5_RC_TYPE_FILE
#define CSFC5_RC_TYPE_FILE		3
#endif


#ifndef csfC5_rcType
typedef csf_i16 csfC5_rcType;
#endif

/* rc -- Visible portion of replay cache. */
typedef struct _csfC5_rc {
	const char *name;			/* Name */
	const char *tname;			/* Type name */
	csfC5_ccType type;			/* Type. */
	csf_flags oflags;			/* Open flags. */
	csf_flags iflags;			/* Information flags. */
	csfC5_tDelta lifespan;		/* Lifespan. */
	/* Statistics. */
	csf_ui32 nreps;			/* Replays found. */
	csf_ui32 nentry;			/* Entries in cache. */
	csf_ui32 npurges;			/* Purges done. */
	csf_ui32 nflushes;			/* Flushes done. */
	csf_ui32 nadded;			/* Entries added since flush. */
} csfC5_rc;


/*TBD/JNK--open_rc -- replace resolve, recover, initialize*/
#define CSFC5_RC_OPTS_RECOVER		0x0001
#define CSFC5_RC_OPTS_INIT			0x0002
#define CSFC5_RC_OPTS_INITIF		0x0004
#define CSFC5_RC_OPTS_TEMP			0x0008
#define CSFC5_RC_OPTS_APURGE		0x0010
#define CSFC5_RC_OPTS_AFLUSH		0x0020

typedef struct _csfC5_rcOpts {
	csfC5_tDelta life;
	csf_ui32 apurge;
	csf_ui32 aflush;
} csfC5_rcOpts;

csf_sts csfC5open_rc (
	csfC5_rc **rc,
	const char *name,
	csf_flags opts,
	const csfC5_rcOpts *rcopts
);

csf_sts csfC5close_rc (
	csfC5_rc **rc
);

csf_sts csfC5destroy_rc (
	csfC5_rc **rc
);

csf_sts csfC5resolve_rc (
	csfC5_rc **rc,
	const char *name
);

csf_sts csfC5recover_rc (
	csfC5_rc *rc
);

csf_sts csfC5initialize_rc (
	csfC5_rc *rc,
	csfC5_tDelta life
);

csf_sts csfC5purge_rc (
	csfC5_rc *rc
);

csf_sts csfC5flush_rc (
	csfC5_rc *rc
);

csf_sts csfC5putEntry_rc (
	csfC5_rc *rc,
	csfC5_tStamp rtime,
	const csfC5_data *rdata
);

#endif /* _CSFC5_RC_H_ */
