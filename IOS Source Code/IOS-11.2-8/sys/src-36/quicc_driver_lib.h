/* $Id: quicc_driver_lib.h,v 3.3.62.3 1996/08/28 13:10:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-36/quicc_driver_lib.h,v $
 *------------------------------------------------------------------
 * quicc_driver_lib.h -- Common definitions for the quicc drivers.
 *
 * November 1994, Andrew G. Harvey
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: quicc_driver_lib.h,v $
 * Revision 3.3.62.3  1996/08/28  13:10:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.2  1996/06/12  19:56:46  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.3.62.1  1996/03/18  21:56:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:49:15  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:39:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/09  19:32:49  sgannu
 * CSCdi38309:  catwoman msg causes modular build failure
 *
 * Revision 2.2  1995/06/20  07:18:12  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:52:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __QUICC_DRIVER_LIB_H__
#define __QUICC_DRIVER_LIB_H__

#include "../les/les_driver.h"

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

#define DPRAM quicc_dpr_t
#define QUICC_REGS quicc_internal_regs_t

typedef void (*scc_handler_t)(hwidbtype *, paktype *);
typedef void (*bd_status_handler_t)(hwidbtype *, ushort); 
typedef void (*line_handler_t)(hwidbtype *);

#define QUICC_DRIVER_INSTANCE                                                \
    ulong port_no;            /* Serial number across all components -       \
				 SCCs, SMCs, SPI, etc */                     \
    DPRAM *dprbase;           /* DP RAM base address */                      \
    void *parmbase;           /* Ptr to start of SCC gen params */           \
    QUICC_REGS *regbase;      /* Ptr to start of internal regs */            \
    ushort mode;              /* Protocol specific mode */                   \
    ushort padding_QDS2;      /* Long word alignment  */                     \
                                                                             \
    /*                                                                       \
     * Pointers to any frequently accessed registers or                      \
     * parameter RAM locations.                                              \
     */                                                                      \
    volatile ulong *gsmr_hi;           /* High portion of GSMR register */   \
    volatile ulong *gsmr_lo;           /* Low portion of GSMR register */    \
    volatile ushort *psmr;             /* Protocol specific mode register */ \
    volatile ulong *cipr;              /* Pointer to CP Int pending reg */   \
    volatile ulong *cimr;              /* Pointer to CP Int mask reg*/       \
    volatile ulong *cisr;              /* Pointer to CP Int in-service reg */\
    volatile ulong *cicr;              /* Pointer to CP Int control reg */   \
    volatile ushort *cr;               /* Pointer to CP command reg */       \
    volatile ushort *scce;             /* Pointer to SCC event register */   \
    volatile ushort *sccm;             /* Pointer to SCC mask register */    \
    volatile uchar  *sccs;             /* Pointer to SCC status register */  \
    ushort *tx_todr;                   /* Pointer to Tx-on-demand reg */     \
                                                                             \
    /*                                                                       \
     * Interrupt handler callback functions                                  \
     */                                                                      \
    scc_handler_t rx_handler;          /* RX handler routine */              \
    scc_handler_t tx_handler;          /* TX handler routine */              \
    bd_status_handler_t misc_handler;  /* Misc handler routine */            \
    bd_status_handler_t RX_errors;     /* RX error decoding */               \
    bd_status_handler_t TX_errors;     /* TX error decoding */               \
    line_handler_t line_up_fn;         /* Mark line UP if DOWN */            \
    line_handler_t line_down_fn;       /* Mark line DOWN if UP */            \
                                                                             \
    /* RX errors */                                                          \
    ulong overflow_err;		/* # overflow errors */                      \
    ulong no_enp;		/* # no-end-of-packets */                    \
    ulong buffer_err;           /* # buffer errors (busy condition) */	     \
    volatile ushort rx_bd_status;  /* cached current rx buffer status */     \
    quicc_info_t *qinfo;						     \
    void (*quicc_async_tx_intr_ptr)(hwidbtype *);			    


#ifdef RXBD_T
#undef RXBD_T
#endif

#ifdef TXBD_T
#undef TXBD_T
#endif

#define RXBD_T quicc_bd_t
#define TXBD_T quicc_bd_t

typedef struct quicc_instance {

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

} quicc_instance_t;


#define QUICC_INSTANCE_SIZE   sizeof(quicc_instance_t)
#define QUICC_BDSTAT_TX_RCNT(x)  ((x & 0x003C) >> 2)

#define QUICC_PORT_NO(x)  QUICC_PORT_SCC ## x

#define QUICC_BDSTAT_RX_PKT     (QUICC_BDSTAT_RX_FIRST | QUICC_BDSTAT_RX_LAST)
#define QUICC_BDSTAT_TX_PKT     (QUICC_BDSTAT_TX_LAST)

#define QUICC_BDSTAT_RX_DEFAULT (QUICC_BDSTAT_RX_EMPTY | QUICC_BDSTAT_RX_INT)
#define QUICC_BDSTAT_TX_DEFAULT (QUICC_BDSTAT_TX_RDY |    \
                                 QUICC_BDSTAT_TX_INT |    \
                                 QUICC_BDSTAT_TX_PAD |    \
                                 QUICC_BDSTAT_TX_LAST |   \
                                 QUICC_BDSTAT_TX_TRCRC)

#define QUICC_TX_KICK           0x8000   /* Set TOD bit */


/*
 * Prototypes.
 */
boolean quicc_scc_bd_ring_init(hwidbtype *, boolean);
boolean quicc_scc_bd_rx_ring_init (hwidbtype *, boolean);
void quicc_scc_bd_tx_ring_init (hwidbtype *);
inline uint quicc_tx_slots_available (quicc_instance_t *);
void quicc_scc_transmit(quicc_instance_t *, paktype *, ushort);
void quicc_scc_transmit_buffer (quicc_instance_t *, paktype *,
				uchar *, int, ushort);
void quicc_scc_handler_install(hwidbtype *, ushort);
void quicc_scc_reg_init(quicc_dpr_t *, ushort);
void quicc_scc_show_regs(hwidbtype *, boolean);
void quicc_scc_show_ether_param(quicc_ether_param_t *);
void quicc_scc_show_hdlc_param(quicc_hdlc_param_t *);
void quicc_scc_show_uart_param(quicc_uart_param_t *);
void quicc_scc_tx_intr(hwidbtype *);
void quicc_tx_start_output(hwidbtype *);
void quicc_scc_driver_setup(hwidbtype *);

/*
 * QUICC Interrupt block.
 */
typedef struct quicc_interrupt {
    volatile ushort *event;
    hwidbtype *idb;
    ulong mask;
    volatile ulong *cisr;
} quicc_interrupt_t;

boolean quicc_debug;

#define RX_ENABLED 	TRUE
#define RX_DISABLED	TRUE
#define TX_ENABLED	TRUE
#define TX_DISABLED	TRUE

#endif  /* __QUICC_DRIVER_LIB_H__ */


