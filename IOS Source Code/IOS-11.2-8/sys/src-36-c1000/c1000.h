/* $Id: c1000.h,v 3.3.62.1 1996/03/18 21:57:10 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-36-c1000/c1000.h,v $
 *------------------------------------------------------------------
 * c1000.h -- Common definitions for all C1000 platforms
 *
 * November 1994, David Coli
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c1000.h,v $
 * Revision 3.3.62.1  1996/03/18  21:57:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:49:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:39:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:34  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:26:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/23  05:58:09  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.2  1995/09/05  21:39:59  enf
 * CSCdi39019:  <SYSTEM OK> LED off after booting
 * move LED initialization to after Exception setup
 *
 * Revision 2.1  1995/06/07  22:52:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __C1000_H__
#define __C1000_H__

#include "../src-36/quicc.h"
#include "../src-36/quicc_lib.h"
#include "logger.h"

#define CONFIG_MAGIC 0xf0a5

/*
 * Notes on buffer sizing:
 * The largest packet we must handle is an HDLC bridged, maximum sized
 * Ethernet datagram.  The header in this case is 4 bytes, and the data
 * may be up to 1517 bytes (European interpretation of IEEE).  If we
 * allocate a data buffer of 1520 bytes, this datagram will be received
 * just fine by the HDLC receiver.  The total size of this datagram is
 * 1521 bytes, so we should allow (MAXSERIALSIZE+ENCAPBYTES) == 1524
 * byte datagrams to be transmitted from process level.
 */
#define PAN_MAXDGRAM		1524	/* bytes, for buffer size checking */

/*
 * Maximum size of a RX or TX buffer ring:
 */
#define MAX_RING		16

/*
 * Define the Tx and Rx ring sizes for different mtus
 */
#define MAX_TXRING_BIG		4
#define MAX_RXRING_BIG		16
#define MAX_TXRING_LARGE	4
#define MAX_RXRING_LARGE        8
#define MAX_TXRING_HUGE		2
#define MAX_RXRING_HUGE		4

#define RXBOOT_MAX_TXRING	1
#define RXBOOT_MAX_RXRING	4

/* 
 * Define multipliers for private buffer pool allocations
 */
#define PRIV_NUM_BUF(rx_size)   (rx_size * 2)
#define PRIV_NUM_CACHE(rx_size) (rx_size / 2)

#define C1000_IOMEM_SIZE    0x80000   /* One-half meg for I/O memory */


/*
 * Shared externals:
 */

extern cookie_t *cookie;	/* Address of cookie PROM */

/*
 * LED Definitions
 */
#define C1000_RUN_LED_ON      QUICC_CLR_PB17 /* Active low */
#define C1000_RUN_LED_OFF     QUICC_SET_PB17

/* Sapphire and Frankenstorm (C1003, C1004) LED definitions */
#define C1000_ISDN_B2_LED_ON   QUICC_CLR_PB11
#define C1000_ISDN_B2_LED_OFF  QUICC_SET_PB11
#define C1000_ISDN_B1_LED_ON  QUICC_CLR_PB10
#define C1000_ISDN_B1_LED_OFF  QUICC_SET_PB10
 
/* Catwoman (C1005) LED definitions */
#define QUICC_WAN_RX_LED_ON   QUICC_CLR_PB11
#define QUICC_WAN_RX_LED_OFF  QUICC_SET_PB11
#define QUICC_WAN_TX_LED_ON   QUICC_CLR_PB10
#define QUICC_WAN_TX_LED_OFF  QUICC_SET_PB10
 

/*
 * Seconds to allow break during booting
 */

#define BREAK_BOOT_WINDOW (60 * ONESEC)

/*
 * Prototypes
 */

extern void c1000_init_pit_timer(void);
extern void c1000_pit_isr(void);
extern void c1000_post_exception_init(void);
extern void c1000_add_registries(void);
#endif
