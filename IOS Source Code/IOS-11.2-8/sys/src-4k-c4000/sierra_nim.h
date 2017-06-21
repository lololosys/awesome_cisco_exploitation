/* $Id: sierra_nim.h,v 3.3 1995/11/17 18:41:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-c4000/sierra_nim.h,v $
 *------------------------------------------------------------------
 * Sierra nim header file
 *
 * 10/93, Lele Nardin
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: sierra_nim.h,v $
 * Revision 3.3  1995/11/17  18:41:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:22:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:40  hampton
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
#define MAX_RING		64

/*
 * Define the Tx and Rx ring sizes for different mtus
 */
#define MAX_TXRING_BIG		32
#define MAX_RXRING_BIG		32
#define MAX_TXRING_LARGE	4
#define MAX_RXRING_LARGE        8
#define MAX_TXRING_HUGE		2
#define MAX_RXRING_HUGE		4

#define RXBOOT_MAX_TXRING	1
#define RXBOOT_MAX_RXRING	4

/* define multipliers for private buffer pool allocations */

#define PRIV_NUM_BUF(rx_size)   (rx_size * 3)
#define PRIV_NUM_CACHE(rx_size) (rx_size)

#define HIRQ_INTLEVEL 		3
#define CMT_DISABLE 		5

#define MAX_IDB_PER_NIM		8	/* no of IDB pointers per NIM */

#define SIERRA_SUBUNIT_A	0
#define SIERRA_SUBUNIT_B	1
#define SIERRA_SUBUNIT_C	2
#define SIERRA_SUBUNIT_D	3
#define SIERRA_SUBUNIT_E	4
#define SIERRA_SUBUNIT_F	5
#define SIERRA_SUBUNIT_G	6
#define SIERRA_SUBUNIT_H	7

#define CSR0_OFFSET		0x200L	/* offset of CSR0 */
#define CSR1_OFFSET		0x202L	/* CSR1 within board's space */
#define CSR2_OFFSET		0x204L	/* offset of CSR2 */

/*
 * Define addressing macros
 */
#define GET_SLOT_BASE(slot) \
    ((unsigned short *)(ADRSPC_SLOT_1 + (slot * SIERRA_SLOT_SEPARATION)))

#define GET_IACK_ADDR(slot) \
    ((uchar *)(ADRSPC_IACK_1 + (slot * SIERRA_IACK_SEPARATION)))

/*
 * lev2_jumptable_t data structure
 */
typedef struct lev2_jumptable_t_ {
    void (*wrapper)(void);		/* Wrapper pointer  */
    void *slot_base_address;		/* Slot base address */
    uchar *slot_iack_reg;		/* Slot IACK register */
    hwidbtype *idb[MAX_IDB_PER_NIM];	/* Array of pointers to idbs */
} lev2_jumptable_t;

/*
 * Define minimum NIM version levels
 */
#define SIERRA_2R_1_MIN_VERSION 0x10
#define SIERRA_2R_2_MIN_VERSION 0x04
#define SIERRA_FDDI_MIN_VERSION 0x4C
#define SIERRA_4T_MIN_VERSION   0x03

/*
 * Prototypes and externals
 */
extern lev2_jumptable_t lev2_jumptable[];

void nim_health_sierra_light(hwidbtype *idb);
void set_slot_vector(uint slot, void (*handler)(void));
void set_slot_idb(hwidbtype *idb);
void hi_irq_dispatcher(void);

