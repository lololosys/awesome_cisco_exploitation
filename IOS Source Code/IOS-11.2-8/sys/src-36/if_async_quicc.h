/* $Id: if_async_quicc.h,v 3.1.6.2 1996/08/28 13:10:17 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-36/if_async_quicc.h,v $
 *------------------------------------------------------------------
 * if_async_quicc.h  definitions for MOT 68360 (QUICC) async serial SCC
 *
 * March 1996, John Ng
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_async_quicc.h,v $
 * Revision 3.1.6.2  1996/08/28  13:10:17  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.6.1  1996/06/12  19:56:29  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.1  1996/03/20  01:27:08  jng
 * Initial Async Catwoman placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

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
*********************************************************/

#ifndef __IF_ASYNC_QUICC_H__
#define __IF_ASYNC_QUICC_H__

#include "../src-36/quicc_driver_lib.h"


typedef struct quicc_async_instance {

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
     * Serial controller specific fields.
     */

    uint output_ctslosts;	/* Number of CTS losts */

    volatile	ulong *rxbrgc;	/* Receive BRGC */
    volatile	ulong *txbrgc;	/* Transmit BRGC */

} quicc_async_instance_t;

#define QUICC_ODD_PARITY		0x0
#define QUICC_SPACE_PARITY		0x1
#define QUICC_EVEN_PARITY		0x2
#define QUICC_MARK_PARITY		0x3

#define MAX_ASYNC_SPEED			115200
#define MIN_ASYNC_SPEED			50

#define QUICC_UART_BDSTAT_RX_PKT	(QUICC_BDSTAT_RX_CNTRL | \
					 QUICC_BDSTAT_RX_ADDR)

#define QUICC_ASYNC_TX_ERR_MASK	  (QUICC_BDSTAT_TX_CTSLOS)

#define QUICC_ASYNC_RX_ERR_MASK   (QUICC_BDSTAT_RX_FR |  \
				   QUICC_BDSTAT_RX_PR | \
				   QUICC_BDSTAT_RX_OVRRUN | \
				   QUICC_BDSTAT_RX_CDLOST)

#define QUICC_ASYNC_RX_MASK  	  (QUICC_ASYNC_RX_ERR_MASK | \
				   QUICC_UART_BDSTAT_RX_PKT)

#define QUICC_ASYNC_TX_MASK  	  (QUICC_ASYNC_RX_ERR_MASK | \
				   QUICC_UART_BDSTAT_TX_PKT)

#define QUICC_SET_RX_BRGC(dpr)    (&((dpr)->regs.brg_brgc4));
#define QUICC_SET_TX_BRGC(dpr)	  (&((dpr)->regs.brg_brgc2));
#define QUICC_TX_LAST_BD	1

#endif /* __IF_ASYNC_QUICC_H__ */







