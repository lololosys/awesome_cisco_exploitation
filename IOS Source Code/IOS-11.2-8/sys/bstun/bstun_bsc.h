/* $Id: bstun_bsc.h,v 3.2 1995/11/17 08:48:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/bstun_bsc.h,v $
 *------------------------------------------------------------------
 * bstun_bsc.c - Binding for BSTUN<->BSC configuration
 *
 * August 1994, Andrew Rothwell
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun_bsc.h,v $
 * Revision 3.2  1995/11/17  08:48:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/11  08:21:58  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.2  1995/06/16  06:24:49  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __BSTUN_BSC_H
#define __BSTUN_BSC_H

extern void
bstun_bsc_init(void);

extern boolean
bsc_validate_address(hwidbtype* idb,
                     uchar* address,
                     ushort length);

extern void
bstun_bsc_shutint(hwidbtype *idb);

extern void
bstun_bsc_startint(hwidbtype* idb);

extern void
bsc_print_frame(paktype *pak,
                int group,
                char *prefix,
                hwidbtype *idb);

extern bstun_protocol_info*
bstun_bsc_predefined_init(char **namep);

extern bstun_protocol_info*
bstun_bsc_local_ack_predefined_init(char **namep);

#endif  /* __BSTUN_BSC_H. */
/*---- End include file: bstun_bsc.h. */
