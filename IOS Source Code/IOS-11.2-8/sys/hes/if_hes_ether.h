/* $Id: if_hes_ether.h,v 3.4.52.1 1996/06/07 19:03:35 kramling Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_hes_ether.h,v $
 *------------------------------------------------------------------
 * if_hes_ether.h -- Prototypes for the ether specific code that was in 
 *                             if_fci.c
 *
 * September, 1989,  Robert Fletcher
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_hes_ether.h,v $
 * Revision 3.4.52.1  1996/06/07  19:03:35  kramling
 * CSCdi55236:  Problems with FEIP OIR
 * Branch: California_branch
 *
 * Revision 3.4  1995/12/02  06:55:31  tgrennan
 * CSCdi42449:  4/8e address filter improperly set by RP
 *
 * 4/8e address filter works like FE and FEIP instead of EIP/MCI.
 * Also, split large switch in cbus_mci_idb_init() into sub-functions.
 *
 * Revision 3.3  1995/11/17  09:18:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:28:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:00:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/09  00:19:52  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.1  1995/06/07  20:41:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __HES_ETHER_H__
#define __HES_ETHER_H__

void cbus_mci_ether_reset(hwidbtype *hwidb);
void cbus_mci_ether_periodic(hwidbtype *hwidb);
void cbus_mci_record_ether_address(idbtype *idb, uchar *address, uchar fsm);
void cbus_mci_record_ether_range(idbtype *idb, uchar *address, uchar *mask);
void cbus_feip_reset(hwidbtype *hwidb);
void cbus_pas_ethernet_reset(hwidbtype *hwidb);
void cbus_mci_record_ether_address(idbtype *idb, uchar *address, uchar fsm); 
void cbus_record_pas_ethernet_address(idbtype *idb, uchar *address,
				       uchar fsm); 
void cbus_mci_record_ether_range(idbtype *idb, uchar *address, uchar *mask);
void cbus_record_pas_ethernet_range(idbtype *idb, uchar *address,
				     uchar *mask);

extern void eip_platform_init(subsystype*);


#endif  /* __HES_ETHER_H__ */
