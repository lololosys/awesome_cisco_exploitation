/* $Id: ipfast_sse.h,v 3.2 1995/11/17 09:35:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/ipfast_sse.h,v $
 *------------------------------------------------------------------
 * ipfast_sse.h - Definitions for IP fast switching with the SSE
 *
 * September 1992, Tony Li
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_sse.h,v $
 * Revision 3.2  1995/11/17  09:35:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:59:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

struct sse_ipinfo_ {
    boolean ip_rebuild;			/* TRUE if we need an overhaul */
    boolean ip_repopulate;		/* TRUE if we need to repopulate */
};

enum IPSSE_PROGRESS {
    IPSSE_SUCCESS,
    IPSSE_FAILURE,
    IPSSE_CONTNUE,
};

/*
 * ipfast_sse.c
 */ 
extern boolean ipfast_sse_dispatch(sse_protoinfo *protoinfo);
extern void ipfast_sse_update(ssetype *sse, sse_protoinfo *protoinfo);
extern void ipfast_clear_sse(sse_protoinfo *protoinfo);
extern void ipfast_sse_init(sse_protoinfo *protoinfo);
extern void ipfast_sse_register_thyself(void);

extern void ip_sse_chain_init(void);
