/* $Id: xx_nim.h,v 3.3 1995/11/17 18:43:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-68-c4000/xx_nim.h,v $
 *------------------------------------------------------------------
 * Dos XX nim header file
 *
 * 4/92, Steve Southam, Marciano Pitargue
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: xx_nim.h,v $
 * Revision 3.3  1995/11/17  18:43:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:19  hampton
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
#define MAX_TXRING_BIG		8
#define MAX_RXRING_BIG		32
#define MAX_TXRING_LARGE	4
#define MAX_RXRING_LARGE        8
#define MAX_TXRING_HUGE		2
#define MAX_RXRING_HUGE		4

#define RXBOOT_MAX_TXRING	1
#define RXBOOT_MAX_RXRING	4

/* define multipliers for private buffer pool allocations */

#define PRIV_NUM_BUF(rx_size)   (rx_size * 2)
#define PRIV_NUM_CACHE(rx_size) (rx_size / 2)

#define HIRQ_INTLEVEL 4
#define CMT_DISABLE 6

#define XX_START_VECTOR		64  	/* User Defined Vector Space Start */
#define XX_VECTORS_PER_SLOT	16  	/* Max user vectors avail per slot */

#define XX_SLOT_BASE		0x08000000 /* Base Address of first slot */
#define XX_SLOT_SEPARATION	0x0100000 /* Distance between successive slot base */

#define NIM_ID_OFFSET           0x0 /* ugly yes eli */
#define NIM_VERSION_OFFSET      0x2 /* */
#define IDPROM_OFFSET		0x0    /* The board ID (lsbyte of word) */
#define VERSION_PROM_OFFSET	0x0202 /* The board Version (lsbyte of word) */

#define CSR0_OFFSET             0x200L /* offset to CSR0 */
#define CSR1_OFFSET             0x202L /* CSR1 within the board's space */
#define CSR2_OFFSET             0x204L /* offset to CSR2 */

#define XX_SUBUNIT_A            0
#define XX_SUBUNIT_B            1
#define XX_SUBUNIT_C            2
#define XX_SUBUNIT_D            3

/* This macro stuffs the address of a handler into the "offset'th" vector for 
 *  the slot. The handler will normally be a wrapper.
 */
#define SET_SLOT_VECTOR(slot, offset, handler) \
        *(int *) (XX_START_VECTOR_ADDR    +    \
          ((slot) * XX_VECTOR_SEPARATION) +    \
          ((offset) * 4))                      \
        =  (handler) 

#define GET_VECTOR_LOC(slot) \
	XX_START_VECTOR + (slot * XX_VECTORS_PER_SLOT)

/*
 * Use this macro to calculate the base address of a board
 */
#define GET_SLOT_BASE( slot ) \
         ((unsigned short *)(XX_SLOT_BASE + ((slot) * XX_SLOT_SEPARATION)))

#define GET_SLOT_VECTOR(slot, offset) \
        (XX_START_VECTOR + ((slot) * XX_VECTORS_PER_SLOT) + (offset))


/*
 * Structures for interrupt chaining
 */
typedef struct ichain_instance ichain_instance_t;
struct ichain_instance {

    ichain_instance_t *next;     /* Next idb in ichain queue */
    hwidbtype           *idb;      /* Pointer to relevant idb block */
    uint               function; /* Entry point for chain */
    uint               slot;     /* Slot number of chain */
    uint               subunit;  /* Subunit (offset) of chain */

};

/*
 * External declarations
 */
extern hwidbtype **nim_idb_array; /* idb table in the SRAM scratch area */
extern cookie_t *cookie;   /* Address of cookie PROM */

extern queuetype *ichainQ;
extern queuetype *ichaincacheQ;

/*
 * Prototypes
 */
void nim_health_xx_light(hwidbtype *idb);
void set_slot_vector(uint slot, uint offset, uint handler);
void set_slot_vector_range(uint slot, uint start, uint end, uint function);
ushort get_slot_vector(uint slot, uint offset);
void set_slot_idb(hwidbtype *idb);
void init_ichain_cache(void);
void add_idb_to_ichain(uint slot, uint subunit, hwidbtype *idb, uint function);
void remove_idb_from_ichain(hwidbtype *idb);
void rebuild_ichain_jump_table(void);
void nim_ichain_jump_exit(void);
void hi_irq_dispatcher (void);

