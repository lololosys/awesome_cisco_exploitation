/* $Id: dbg.h,v 3.2 1995/11/17 17:47:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/dbg.h,v $
 *------------------------------------------------------------------
 * dbg.h - Kerberos debug definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dbg.h,v $
 * Revision 3.2  1995/11/17  17:47:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_DBG_H_
#define _CSF_DBG_H_  "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#ifdef CSF_DEBUG

#define _csfDBGlog( largs ) \
	csfDBGlog largs

void csfDBGlog (
	int ltype,
	...
);

#define _csfDBGprint( largs ) \
	csfDBGprint largs

void csfDBGprint (
	int ltype,
	...
);

#define _csfDBGassert( cnd ) \
	csfDBGassert( (cnd) == 0 )

void csfDBGassert (
	int cnd
);

#else

#define _csfDBGassert( cnd )

#define _csfDBGlog( largs )

#define _csfDBGprint( largs )

#endif /* CSF_DEBUG */

#endif /* _CSF_DBG_H_ */
