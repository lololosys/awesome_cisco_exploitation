/* $Id: isis_linkage.h,v 3.1 1995/11/09 11:09:40 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/isis_linkage.h,v $
 *------------------------------------------------------------------
 * Structure for linking routes into ISIS
 *
 * August 1995, Dave Katz
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isis_linkage.h,v $
 * Revision 3.1  1995/11/09  11:09:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/23  21:37:47  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _ISIS_LINKAGE_H_
#define _ISIS_LINKAGE_H_

struct isis_lsp_linkage_ {
    struct isis_lsp_linkage_ *lsp_next;	  /* Next in chain */
    struct isis_lsp_linkage_ **lsp_prev;  /* Previous guy's next pointer */
    ushort lsp_index;			/* LSP index we're linked to */
    ushort lsp_version;			/* Version number of said LSP */
};

#endif					/* _ISIS_LINKAGE_H_ */
