/* $Id: ipaccess_sse_public.h,v 3.2 1995/11/17 09:34:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/ipaccess_sse_public.h,v $
 *------------------------------------------------------------------
 * ipaccess_sse_public.h
 *
 * June 1993 Tony Li
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipaccess_sse_public.h,v $
 * Revision 3.2  1995/11/17  09:34:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/15  18:38:32  gchristy
 * CSCdi26854:  SSE access-lists arent enabled with SSE switching
 *  - add a new registry function which causes SSE access lists to be
 *    rebuilt when SSE switching is enabled or disabled on an interface
 *    that has an access-list defined
 *
 * Revision 2.1  1995/06/07  20:58:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ipaccess_sse.c
 */ 
extern void sse_access_group_callback(idbtype *idb, int onoff, int listnumber, boolean new);
extern void sse_access_list_callback(int listnumber, boolean new);
extern void sse_access_check(idbtype *idb);
extern void ipaccess_sse_deny(ssetype *sse, idbtype *idb);
extern void ipaccess_sse_backpatch(ssetype *sse, idbtype *idb, spintype *newtree);
extern void ipaccess_sse_deletetree(ssetype *sse, idbtype *idb);
extern spintype *ipaccess_sse_term(ssetype *sse, boolean grant, ipaddrtype address, ipaddrtype mask);
extern int ipaccess_sse_code_gen(ssetype *sse, spintype *spin, ssestattype *stat, queuetype *queue);
extern spintype *ipaccess_sse_newtree(ssetype *sse, idbtype *idb);
extern void ipaccess_sse_update(ssetype *sse);
extern void ipaccess_sse_show_command(parseinfo *csb);
extern sse_ipaccessinfo *ipaccess_sse_init(void);
extern void ipaccess_sse_print_stats(ssetype *sse);
