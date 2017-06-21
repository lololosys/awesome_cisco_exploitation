/* $Id: if_ether_quicc.h,v 3.3.62.2 1996/09/05 21:33:21 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-36/if_ether_quicc.h,v $
 *------------------------------------------------------------------
 * if_ether_quicc.h -- definitions for MOT 68360 (QUICC) ethernet SCC
 * 
 * November 1994, Ashwin Baindur
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_ether_quicc.h,v $
 * Revision 3.3.62.2  1996/09/05  21:33:21  snyder
 * CSCdi68132:  declare consts in lapb x25 atm_arp tcl
 *              forgot I needed to clean up this extern
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/06/12  19:56:32  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.3  1995/11/17  18:39:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/25  21:48:57  abaindur
 * CSCdi37628:  LOSTCARR msgs too frequent when c1000 ethernet cable
 * unplugged
 * Rate limit message. Also fix handling of line protocol state during
 * carrier loss, and counting of deferred transmits. Remove a bogus
 * check for idb->status in ethernet_enable.
 *
 * Revision 2.1  1995/06/07  22:51:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_ETHER_QUICC_H__
#define __IF_ETHER_QUICC_H__

#include "../src-36/quicc_driver_lib.h"

/******************** Driver instance ********************

struct driver_instance {
     *
     * Stuff that is common across interface types and
     * interface controller chips.
     * 

     *
     * Any stuff that is common across interface types
     * but specific to a controller chip.
     * 

     *
     * Any stuff that is specific to an interface type 
     * and a controller chip.
     * 
}
**********************************************************/

typedef struct ether_instance {

    /*
     * Driver defined data structure.
     * Allocate in CPU RAM and keep long words long word aligned for speed.
     */

    COMMON_DRIVER_INSTANCE

    /*
     * Fields common across interface types (but controller
     * specific).
     */

    QUICC_DRIVER_INSTANCE

    /* 
     * Ethernet controller specific fields.
     */

    /* TX errors */
    ulong tx_hbeat_loss;        /* Heartbeat check failure */
    ulong tx_def_count;         /* Total deferred count */
    
} ether_instance_t;


#define QUICC_ETHER_MAX_FRAME 1518
#define QUICC_ETHER_CRC_SIZE 4
#define QUICC_ETHER_INSTANCE_SIZE (sizeof(ether_instance_t) + 8)

 
#define QUICC_ETHERNET_MODE_HBC	0x8000  /* Heartbeat checking */
#define QUICC_ETHERNET_MODE_FC	0x4000  /* Force collision on every Tx */
#define QUICC_ETHERNET_MODE_RSH	0x2000  /* Accept short frames */
#define QUICC_ETHERNET_MODE_IAM	0x1000  /* Use individual hash table */
#define QUICC_ETHERNET_MODE_CRC	0x0800  /* CRC selection */
#define QUICC_ETHERNET_MODE_PRO	0x0200  /* Promiscuous mode */
#define QUICC_ETHERNET_MODE_BRO	0x0100  /* Reject broadcast address */
#define QUICC_ETHERNET_MODE_SBT	0x0080  /* Stop backoff timer on collision */
#define QUICC_ETHERNET_MODE_LPB	0x0040  /* Loopback based on DIAG bit in GSMR */
#define QUICC_ETHERNET_MODE_SIP	0x0020  /* Sample PB8-PB15 to create tag */  
#define QUICC_ETHERNET_MODE_LCW	0x0010  /* Decrease late collision window */ 
#define QUICC_ETHERNET_MODE_NIB(x)	((x & 0x7) << 1)
                                        /* Bits to ignore before SFD */

#define QUICC_ETHERNET_DEFAULT_MODE    \
                               (QUICC_ETHERNET_MODE_HBC |  \
                                 QUICC_ETHERNET_MODE_RSH | \
                                 QUICC_ETHERNET_MODE_IAM | \
                                 QUICC_ETHERNET_MODE_CRC | \
                                 QUICC_ETHERNET_MODE_NIB(5))

#define QUICC_ETHERNET_RX_ERR   (QUICC_BDSTAT_RX_FLV |    \
                                 QUICC_BDSTAT_RX_NOFA |   \
                                 QUICC_BDSTAT_RX_SHORT |   \
                                 QUICC_BDSTAT_RX_CRCERR | \
                                 QUICC_BDSTAT_RX_OVRRUN | \
                                 QUICC_BDSTAT_RX_COLLIS)

#define QUICC_ETHERNET_TX_ERR   (QUICC_BDSTAT_TX_RLIM |   \
                                 QUICC_BDSTAT_TX_UNRRUN | \
                                 QUICC_BDSTAT_TX_CSLOS |  \
                                 QUICC_BDSTAT_TX_HBEAT |  \
                                 QUICC_BDSTAT_TX_LCOL)

#define QUICC_ETHERNET_RX_MASK  (QUICC_ETHERNET_RX_ERR |  \
                                 QUICC_BDSTAT_RX_PKT)
#define QUICC_ETHERNET_TX_MASK  (QUICC_ETHERNET_TX_ERR |  \
                                 QUICC_BDSTAT_TX_PKT)

/*
 * External declarations:
 */
extern void c1000_ethernet_line_down(hwidbtype *);

extern boolean ether_debug;	/* debug flag for ethernet */

#endif /* __IF_ETHER_QUICC_H__ */

