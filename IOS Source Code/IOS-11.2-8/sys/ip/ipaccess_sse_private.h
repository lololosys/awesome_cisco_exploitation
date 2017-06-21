/* $Id: ipaccess_sse_private.h,v 3.2 1995/11/17 09:34:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/ipaccess_sse_private.h,v $
 *------------------------------------------------------------------
 * ipaccess_sse_private.h - Private information for SSE IP access lists
 *
 * Oct 1993 Tony Li
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipaccess_sse_private.h,v $
 * Revision 3.2  1995/11/17  09:34:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  08:31:59  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.1  1995/06/07  20:58:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef struct cacheentrytype_ {
    treeaddrtype patchpoint;
    spintype *ip_access_tree;
} cacheentrytype;

struct sse_ipaccessinfo_ {
    ssestattype ipaccess_stat;		/* IP access list stats */
    ulong version;			/* Current ACL config version # */
    ulong generated;			/* ACL Vers. we have code generated */
    cacheentrytype *ipaccess_cache[MAXSLOWACC+1]; /* for ACL tree maintenance */
    spintype *iacl_tree;	/* Pointer to input ACL tree */
};
