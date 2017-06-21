/* $Id: bstun_public.h,v 3.2 1995/11/17 08:49:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/bstun_public.h,v $
 *------------------------------------------------------------------
 * bstun_public.h - BSTUN external prototypes for public routines
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun_public.h,v $
 * Revision 3.2  1995/11/17  08:49:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/11  08:22:00  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.2  1995/06/16  06:24:51  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _BSTUN_PUBLIC_H_
#define _BSTUN_PUBLIC_H_

extern void     bstun_input(hwidbtype *, paktype *);
extern void     bstun_periodic(void);
extern void     bstun_sprintf_addr(uchar *, boolean, uchar, char *);
extern void     bstif_input(paktype *);
extern void     bstun_shutinterface(hwidbtype *, boolean);
extern bstunpeertype * bstun_findpeer(hwidbtype* idb, paktype* pak);

extern boolean  bstun_prioritization(plisttype *, paktype *, hwidbtype *);
extern boolean  prilist_protocol_bstun(parseinfo *, plisttype *);
extern void     priority_protocol_bstun_nv_add(plisttype *p);
extern boolean  priority_protocol_bstun_compare( plisttype *, plisttype *);

extern bstunaddrroutestate_e bstun_query_route(uchar * , hwidbtype *);
extern bstunpeertype* bstun_query_route_cfg(uchar * , hwidbtype *);

extern void     bstun_add_protocol(bstun_protocol_info *spi, char *name);

#endif
