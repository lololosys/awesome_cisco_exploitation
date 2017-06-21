/* $Id: xnstypes.h,v 3.2 1995/11/17 19:26:07 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/xnstypes.h,v $
 *------------------------------------------------------------------
 * ../xns/xnstypes.h -- definitions for XNS protocol family
 *
 * March 1990, Vicki Ralls
 *
 * Copyright (c) 1990-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xnstypes.h,v $
 * Revision 3.2  1995/11/17  19:26:07  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/12  08:30:19  dstine
 * CSCdi36963:  XNS network numbers print in hex
 *
 * Revision 2.1  1995/06/07  23:31:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef	__XNSTYPES_H__
#define __XNSTYPES_H__

/*
 * These are codes that idenity the various types of XNS's paths or routes.
 * They are used primarily by xns/novell/apollo and the printf code.
 */

typedef enum {
    XNS_TYPE,
    NOVELL_TYPE,
    APOLLO_TYPE,
    UBXNS_TYPE
} xnstype;

#endif
