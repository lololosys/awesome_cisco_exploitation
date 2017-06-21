/* $Id: appss.h,v 3.2 1995/11/17 17:47:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/appss.h,v $
 *------------------------------------------------------------------
 * appss.h - Application subsystem definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appss.h,v $
 * Revision 3.2  1995/11/17  17:47:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_APPSS_H_
#define _CSF_APPSS_H_ "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

/* appctx -- Application/user context.  Points to all subsystem-
 * specific nodes.
 */
typedef struct _csfAPP_ctx {
	char *appname;
	const struct _csfAPP_ssEntry **sstab;	/* Static subsystem list (table). */
} csfAPP_ctx;

/* appss_entry -- Subsystem entry; these are normally const and
 * defined at compile/link time.
 */
typedef struct _csfAPP_ssEntry {
	char *ssname;								/* Subsystem name. */
	csf_sts (* appss_begin)					/* Startup function. */
		( struct _csfAPP_ctx * );
	csf_sts (* appss_end)						/* Shutdown function. */
		( struct _csfAPP_ctx * );
	struct _csfSTS_table *appss_ststab;		/* Status table. */
} csfAPP_ssEntry;

#endif /* _CSF_APPSS_H_ */
