/* $Id: if_serial_quicc.h,v 3.3.62.3 1996/08/28 13:10:25 thille Exp $ 
 * $Source: /release/112/cvs/Xsys/src-36/if_serial_quicc.h,v $
 *------------------------------------------------------------------
 * if_serial_quicc.h -- definitions for MOT 68360 (QUICC) serial SCC
 *
 * March 1995, Dave Coli and John Ng
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_serial_quicc.h,v $
 * Revision 3.3.62.3  1996/08/28  13:10:25  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.2  1996/06/12  19:56:37  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.3.62.1  1996/03/18  21:56:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:49:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:39:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:51:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_SERIAL_QUICC_H__
#define __IF_SERIAL_QUICC_H__

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
*********************************************************/

typedef struct serial_instance {

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

    /* Flap Protection */
    boolean  flap_enabled;	/* Whether we've throttled interrupts */
    uint     flap_backoff;	/* Number of backoffs after throttling */
    sys_timestamp flap_time;	/* Time of last flap */
    uint     flap_count;	/* Number of interface flaps */
    uint     max_flap_rate;	/* Maximum rate of flaps per second */
    
    uint output_ctslosts;	/* Number of CTS losts */
    
} serial_instance_t;

#define QUICC_SERIAL_RX_ERR   (QUICC_BDSTAT_RX_DERR |   \
                               QUICC_BDSTAT_RX_FLV |    \
                               QUICC_BDSTAT_RX_NOFA |   \
                               QUICC_BDSTAT_RX_ABORT |  \
                               QUICC_BDSTAT_RX_CRCERR | \
                               QUICC_BDSTAT_RX_OVRRUN | \
                               QUICC_BDSTAT_RX_CDLOST)

#define QUICC_SERIAL_TX_ERR   (QUICC_BDSTAT_TX_UNRRUN | \
                               QUICC_BDSTAT_TX_CTSLOS)

#define QUICC_SERIAL_RX_MASK  (QUICC_SERIAL_RX_ERR |  \
                               QUICC_BDSTAT_RX_PKT)

#define QUICC_SERIAL_TX_MASK  (QUICC_SERIAL_TX_ERR |  \
                               QUICC_BDSTAT_TX_PKT)


#define QUICC_HDLC_CRC_SIZE   2

#define QUICC_SCC_NRZI_RX		0x00000100
#define QUICC_SCC_NRZI_TX		0x00000800


/*
 * External declarations:
 */
extern void quicc_serial_dcd_interrupt(void);

void quicc_cpic_handler_install(hwidbtype *,quicc_dpr_t *, ushort , ushort, 
                                ulong, void (*)(hwidbtype *), void (*)(void));
#endif /* __IF_SERIAL_QUICC_H__ */







