/* $Id: if_c1000_bri.h,v 3.3.62.1 1996/06/12 19:57:03 jng Exp $ 
 * $Source: /release/112/cvs/Xsys/src-36-c1000/if_c1000_bri.h,v $
 *------------------------------------------------------------------
 * if_c1000_bri.h  Definitions for ISDN BRI on C1000
 *
 * November 1994, Greg Stovall
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c1000_bri.h,v $
 * Revision 3.3.62.1  1996/06/12  19:57:03  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.3  1995/11/17  18:40:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:41  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:52:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_C1000_BRI_H__
#define __IF_C1000_BRI_H__

/* Map SCC port numbers to BRI channels */
#define D_CHAN_PORT_NUM		QUICC_PORT_SCC3
#define B1_CHAN_PORT_NUM	QUICC_PORT_SCC2
#define B2_CHAN_PORT_NUM	QUICC_PORT_SCC4
#define	B1CHAN_INSTANCE		idb->isdn_info->d_chan_idb->isdn_info->b1_chan_idb->devctl
#define	B2CHAN_INSTANCE		idb->isdn_info->d_chan_idb->isdn_info->b2_chan_idb->devctl

#endif __IF_C1000_BRI_H__
