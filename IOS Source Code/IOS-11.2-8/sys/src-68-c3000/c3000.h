/* $Id: c3000.h,v 3.3.62.3 1996/06/16 21:19:46 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c3000/c3000.h,v $
 *------------------------------------------------------------------
 * c3000.h -- Common definitions for all C3000 (not XX) interfaces
 *
 * July 1990, Chris Shaker
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c3000.h,v $
 * Revision 3.3.62.3  1996/06/16  21:19:46  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3.62.2  1996/04/30  18:38:20  etrehus
 * CSCdi51546:  Need warning for non-isdn image on platforms with bri
 * Branch: California_branch Committed.
 *
 * Revision 3.3.62.1  1996/03/18  21:59:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  23:01:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:43:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:57  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:54:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

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

/* define multipliers for private buffer pool allocations */

#define PRIV_NUM_BUF(rx_size)   (rx_size * 2)
#define PRIV_NUM_CACHE(rx_size) (rx_size / 2)

/*
 * Interrupt array structure
 */
#define PAN_MAX_INTER 4

typedef struct interrupt_array interrupt_array_t;
struct interrupt_array {
    ulong      function;
    hwidbtype   *idb1;
    hwidbtype   *idb2;
};

interrupt_array_t pan_int_array[PAN_MAX_INTER];
interrupt_array_t t1_int_array[PAN_MAX_INTER];
interrupt_array_t pan_level2_int;


/*
 * Shared externals:
 */
extern hwidbtype *network_a_idb;	/* IDB pointers */
extern hwidbtype *network_b_idb;
extern hwidbtype *network_c_idb;
extern hwidbtype *network_c0_idb;
extern hwidbtype *network_c1_idb;
extern hwidbtype *network_d_idb;

extern cookie_t *cookie;	/* Address of cookie PROM */

/*
 * Prototypes
 */
extern int count_hd64570(int, cookie_t *);
extern int count_bri_hardware(void);
