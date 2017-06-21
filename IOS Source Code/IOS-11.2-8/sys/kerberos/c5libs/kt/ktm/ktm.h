/* $Id: ktm.h,v 3.2 1995/11/17 17:44:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/kt/ktm/ktm.h,v $
 *------------------------------------------------------------------
 * ktm.h - In-memory keytab definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ktm.h,v $
 * Revision 3.2  1995/11/17  17:44:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_LIB_KT_KTM_H_
#define _CSFC5_LIB_KT_KTM_H_
/*.original ktfile.h 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>
#include <csfc5/kt/kt0.h>

#if CSFC5_CFG_KT_MEM

/* ktm_ktlist -- Each keytab entry is pointed to by one of these;
 * these are kept in a linked list.
 */
typedef struct _csfC5_ktMKList {
	struct _csfC5_ktMKList *next;	/* Next in list; NULL if none. */
	csfC5_ktEntry kte;				/* Keytab entry. */
	csfC5_ktBase *csfC5_kt;		/* Where we came from. Debug. */
} csfC5_ktMKList;

/* ktm-specific data */
typedef struct _csfC5_ktMData {
	csfC5_ktBase *kt0;
	csfC5_ktMKList *ktlist;		/* List head of entries. */
} csfC5_ktMData;

/* cursor for positioning operations */
typedef struct _csfC5_ktMCursor {
	csfC5_ktMKList *ktentry;		/* Next keytab entry. */
} csfC5_ktMCursor;

#endif /* CSFC5_CFG_KT_MEM */

#endif /* _CSFC5_LIB_KT_KTM_H_ */
