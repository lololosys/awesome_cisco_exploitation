/* $Id: les_driver.h,v 3.3 1995/11/17 17:41:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/les_driver.h,v $
 *------------------------------------------------------------------
 * les_driver.h -- Common driver stuff for les drivers
 * 
 * November 1994, Ashwin Baindur
 *
 * Copyright (c) 1988-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: les_driver.h,v $
 * Revision 3.3  1995/11/17  17:41:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:23  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:38:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LES_DRIVER_H__
#define __LES_DRIVER_H__

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
};
**********************************************************/

#define RXBD_T void
#define TXBD_T void

#define COMMON_DRIVER_INSTANCE                                          \
                                                                        \
    short rx_size, tx_size;     /* RX and TX ring sizes */              \
    short max_rx_ring_size;     /* Max RX ring size */                  \
    short max_tx_ring_size;     /* Max TX ring sizes */                 \
                                                                        \
    void *rx_base;              /* Malloced base of RX BD ring */       \
    RXBD_T *rx_ra;              /* Adjusted base of RX BD ring */       \
    RXBD_T *rx_head;            /* Pointer to next RX BD */             \
    RXBD_T *rx_max;             /* Pointer to last RX BD */             \
    paktype **head_rxp;         /* Pointer to RX packet for rx_head */  \
    paktype *rx_p[MAX_RING];    /* Copy of RX ring packet pointers */   \
                                                                        \
    void *tx_base;              /* Base address of TX BD ring */        \
    TXBD_T *tx_ra;              /* Adjusted base of TX BD ring */       \
    TXBD_T *tx_head;            /* Next ring entry we expect to TX */   \
    TXBD_T *tx_tail;            /* Last ring entry we expect to TX */   \
    TXBD_T *tx_max;             /* Pointer to last TX BD */             \
    paktype **head_txp;         /* Pointer to TX packet for tx_head */  \
    paktype **tail_txp;         /* Pointer to TX packet for tx_tail */  \
    paktype *tx_p[MAX_RING];    /* Copy of TX ring packet pointers */   \
    ulong tx_count;             /* Count of packets now in TX Q */      \
                                                                        \
    void (*hw_stop)(hwidbtype *);  /* Stop controller */                \
    void (*hw_start)(hwidbtype *); /* Start controller */               \
    void (*hw_init)(hwidbtype *, boolean);  /* Init controller */       \
    void (*hw_health)(hwidbtype *); /* Check controller health */       \
    void (*hw_mode)(hwidbtype *, int); /* Set controller mode */        \
    void (*hw_throttle)(hwidbtype *); /* Throttle controller*/          \
    void (*hw_enable)(hwidbtype *); /* Enable controller*/              \
    void (*show_rxbd)(paktype *, void *, int); /* Show a BD */          \
    void (*show_txbd)(paktype *, void *, int); /* Show a BD */          \
                                                                        \
    /* Throttle statistics */                                           \
    ulong enabled;              /* # times we have been enabled */      \
    ulong tx_restarts;          /* Count of Tx restarts */              \
                                                                        \
    /*                                                                  \
     * Max and min frame lengths supported by driver.                   \
     */                                                                 \
    ulong min_frame_len;        /* Min frame size supported */          \
    ulong max_frame_len;        /* max frame size supported */          \
                                                                        \
    ushort padding_S1;          /* Long word alignment pad */           \
    ushort rx_bd_mask;          /* Mask for checking errors */          \
    ushort rx_bd_good_mask;     /* Mask for good packets    */          \
    ushort tx_bd_mask;          /* Mask for checking errors */          \
    ushort tx_bd_good_mask;     /* Mask for good packets    */          \
    boolean fatal_tx_err;	/* A fatal tx error occured. */         \
    ulong  crc_length;          /* CRC length */                        \
                                                                        \
    /*                                                                  \
     * Error counts                                                     \
     */                                                                 \
    ulong E_ints;               /* Count of error interrupts */

typedef struct les_driver_instance {

    COMMON_DRIVER_INSTANCE

} les_driver_instance_t;

#define MAX_SPIN 16
#define UNKNOWN_DRIVER	0
#define LANCE_DRIVER 	1
#define QUICC_DRIVER 	2

#define RX_INTERRUPT    0
#define TX_INTERRUPT    1
#define E_INTERRUPT     2

#define MODE_LOOPBACK_ENABLE    1  /* Internal or External loopback */
#define MODE_LOCAL_LOOPBACK    	2  /* Local loopback mode enable */
#define MODE_AUTO_ECHO_ENABLE   3  /* Auto echo enable */
#define MODE_LOOPBACK_ECHO      4  /* Local loopback and Echo enable */

#define MODE_DIS_RETRY          5  /* Disable retries */
#define MODE_FORCE_COLLISION    6  /* Force a collision */
#define MODE_HEARTBEAT_CHECK    7  /* Enable heartbeat check */

#define MODE_RX_SHORT_FRAMES    8  /* Receive short frames */

#define MODE_PROM       	9  /* Promiscuous */ 
#define MODE_INDIV_FILTER       10 /* Use individual address filter */
#define MODE_GROUP_FILTER       11 /* Use group address filter */
#define MODE_REJECT_BROADCAST   12 /* Reject broadcast packets */

extern paktype *les_driver_getbuf(hwidbtype *);
extern int les_driver_txqlength(hwidbtype *);
extern boolean show_bdrings(hwidbtype *, ushort);

#endif  /* __LES_DRIVER_H__ */
