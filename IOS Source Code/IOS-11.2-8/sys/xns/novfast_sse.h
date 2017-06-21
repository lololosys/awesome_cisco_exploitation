/* $Id: novfast_sse.h,v 3.2 1995/11/17 19:23:53 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/novfast_sse.h,v $
 *------------------------------------------------------------------
 * novfast_sse.h - Definitions for Novell fast switching with the SP+
 *
 * January 1993 William H. Palmer
 * Sept., 1992 Tony Li
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novfast_sse.h,v $
 * Revision 3.2  1995/11/17  19:23:53  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:29:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

struct sse_ipxinfo_ {
    uint nov_cache_version;
    uint nov_cache_entries;
    uint nov_cache_valid;
};

/*
 * novfast_sse.c
 */ 
extern void novell_read_cachefile(uchar*, uchar*);

/*
 * novell_sse_chain.c
 */ 
void ipx_sse_chain_init(void);
