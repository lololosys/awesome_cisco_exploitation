/* $Id: if_les_lance.h,v 3.5.12.2 1996/08/28 12:58:30 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_lance.h,v $
 *------------------------------------------------------------------
 * if_les_lance.h -- definitions for AMD AM7990 (Lance) chip
 * 
 * Based on "if_lance.h", June 1988, Bill Westfield
 * Modified for Pancake, December 1989, Chris Shaker
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_lance.h,v $
 * Revision 3.5.12.2  1996/08/28  12:58:30  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.12.1  1996/04/06  00:50:18  vdukki
 * CSCdi48472:  Sustained process switched throughput degraded 28%
 * Branch: California_branch
 * Modified the lance_stop() function to add 100 microseconds delay
 * only for 6E NIM.
 *
 * Revision 3.5  1996/02/13  21:26:19  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.4  1996/01/30  20:57:53  vandys
 * CSCdi46080:  Deep six CS500 in source base
 *
 * Revision 3.3  1995/11/17  17:40:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  06:47:44  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:36:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef struct lance_instance lance_instance_t; 
typedef struct lance_initblock lance_initblock_t;
typedef struct lance_rx_ring lance_rx_ring_t;
typedef struct lance_rx_descriptor lance_rx_descriptor_t;
typedef struct lance_tx_descriptor lance_tx_descriptor_t;
typedef struct lance_tx_ring lance_tx_ring_t;

/*
 * Prototypes for instance vectors.
 */
typedef void (* get_regaddr_t) (hwidbtype *);
typedef void (* dev_reset_t) (hwidbtype *);
typedef void (* set_vectors_t) (hwidbtype *);
typedef void (* show_structure_info_t) (hwidbtype *);
typedef void (* pre_init_controller_t) (hwidbtype *);
typedef void (* post_init_controller_t) (hwidbtype *);
typedef void (* delay_vector_t) (unsigned);

#if defined(XX) | defined(SIERRA)
#include "if_c4000_lance.h"
#else 	/* c3000 */
#include "if_c3000_lance.h"
#endif

/*
 * The Lance has 4 CSR registers that can be read or written.
 * The RAP register is a pointer to which CSR register is accessed
 * at lance_regs->csr:
 */

/*
 * LANCE_CSR0
 */
#define LANCE_CSR0 0
#define	LANCE_CSR0_ERR		0x8000	/* Some error has occured */
#define	LANCE_CSR0_BABL		0x4000	/* Tranmitter is babbling */
#define	LANCE_CSR0_HEART	0x2000	/* Heartbeat was not detected */
#define	LANCE_CSR0_MISS		0x1000	/* receiver lost a packet */
#define	LANCE_CSR0_MERR		0x0800	/* memory error */
#define LANCE_CSR0_RINT		0x0400	/* receiver interrupt */
#define	LANCE_CSR0_TINT		0x0200	/* Transmitter interrupt */
#define	LANCE_CSR0_IDON		0x0100	/* Initialization done */
#define	LANCE_CSR0_INTR		0x0080	/* Some interrupt */
#define	LANCE_CSR0_INEA		0x0040	/* Interrupt enable */
#define LANCE_CSR0_RXON		0x0020	/* Receiver on (read only) */
#define LANCE_CSR0_TXON		0x0010	/* Transmitter on (read only) */
#define LANCE_CSR0_TDMD		0x0008	/* Start transmitter */
#define	LANCE_CSR0_STOP		0x0004	/* Stop all external activity */
#define	LANCE_CSR0_STRT		0x0002	/* Start chip */
#define	LANCE_CSR0_INIT		0x0001	/* Initialize */
#define LANCE_CSR0_ERRBITS	(LANCE_CSR0_ERR|LANCE_CSR0_BABL|LANCE_CSR0_HEART|LANCE_CSR0_MISS|LANCE_CSR0_MERR)
#define LANCE_CSR0_NORMAL	(LANCE_CSR0_INEA|LANCE_CSR0_RXON|LANCE_CSR0_TXON|LANCE_CSR0_STRT)
                                        /* 
                                         * for compatibility between 
					 *  Lance and Presidio, do not 
					 *  also compare INIT bit
                                         */

/*
 * LANCE_CSR1 low 16 bits of address of Init Block.
 * LANCE_CSR2 high 8 bits of address of Init Block.
 */
#define LANCE_CSR1 1
#define LANCE_CSR2 2

/*
 * LANCE_CSR3 bus interface.  Must be rewritten every time chip is stopped.
 */

#define LANCE_CSR3 3
#define	LANCE_CSR3_BSWP		0x0004	/* Byte Swap */
#define	LANCE_CSR3_ACON		0x0002	/* ALE Control */
#define	LANCE_CSR3_BCON		0x0001	/* Byte Control */

#define LANCE_CSR3_NORMAL	(LANCE_CSR3_BSWP) /* normal mode */

/*
 * buffer descriptor description
 */

struct lance_rx_descriptor {
    volatile ushort buffer_low;		/* address of buffer */
    volatile uchar  status;		/* Status bits */
    volatile uchar  buffer_high;	/* High part of address */
    volatile ushort max_buffer_size;	/* Length of buffer (negative) */
    volatile ushort packet_size;	/* Length of data */
};

struct lance_rx_ring {
    lance_rx_descriptor_t ring_entry[MAX_RING];
};

#define	LANCE_RMD_OWN		0x80 /* Buffer is owned by lance */
#define LANCE_RMD_ERR		0x40 /* Some error */
#define LANCE_RMD_FRAM		0x20 /* Framing error */
#define LANCE_RMD_OFLO		0x10 /* ran out of memory */
#define LANCE_RMD_CRC		0x08 /* CRC error */
#define LANCE_RMD_BUFF		0x04 /* ran out of buffers */
#define LANCE_RMD_STP		0x02 /* buffer contains start of packet */
#define LANCE_RMD_ENP		0x01 /* Buffer constains end of packet */
#define LANCE_RMD_ERRBITS  (LANCE_RMD_FRAM |  LANCE_RMD_CRC | LANCE_RMD_BUFF)

struct lance_tx_descriptor {
    volatile ushort buffer_low;		/* Address of buffer */
    volatile uchar  status;		/* Status bits */
    volatile uchar  buffer_high;	/* High part of address */
    volatile ushort packet_size;	/* Length of data */
    volatile ushort status2;		/* more status */
};

struct lance_tx_ring {
    lance_tx_descriptor_t ring_entry[MAX_RING];
};

#define	LANCE_TMD_OWN	0x80	/* Buffer is owned by lance */
#define LANCE_TMD_ERR	0x40	/* Some error occured */
				/* Reserved bit */
#define LANCE_TMD_MORE	0x10	/* More than one retry was required */
#define LANCE_TMD_ONE	0x08	/* One retry */
#define LANCE_TMD_DEF	0x04	/* Had to defer */
#define LANCE_TMD_STP	0x02	/* Buffer contains start of packet */
#define LANCE_TMD_ENP	0x01	/* Buffer constains end of packet */

#define LANCE_TMD3_BUFF	0x8000	/* ENP never found */
#define LANCE_TMD3_UFLO	0x4000	/* Underflow */
				/* Reserved bit */
#define LANCE_TMD3_LCOL	0x1000	/* Late collision */
#define LANCE_TMD3_LCAR	0x0800	/* Loss of carrier */
#define LANCE_TMD3_RTRY	0x0400	/* Max retries exceeded */
#define LANCE_TMD3_TDR	0x03FF	/* TDR count */
#define LANCE_TMD3_ERRBITS \
 (LANCE_TMD3_BUFF | LANCE_TMD3_UFLO | LANCE_TMD3_LCOL | LANCE_TMD3_RTRY)

/*
 * Lance mode register bits:
 */
#define LANCE_MODE_PROM	0x8000		/* Promiscuous mode */
#define LANCE_MODE_INTL	0x0040		/* Internal loopback mode */
#define LANCE_MODE_DRTY	0x0020		/* Disable retry */
#define LANCE_MODE_COLL	0x0010		/* Force collision */
#define LANCE_MODE_DTCR	0x0008		/* Disable TX CRC */
#define LANCE_MODE_LOOP	0x0004		/* Full duplex loopback mode */
#define LANCE_MODE_DTX	0x0002		/* Disable the transmitter */
#define LANCE_MODE_DRX	0x0001		/* Disable the receiver */

/*
 * Control (and initialization) block.
 */

struct lance_initblock {

    /*
     * Lance Initialization block - allocated in IO RAM
     * Defined by hardware - do not change!
     */
    volatile ushort mode;		/* operational modes */
    volatile ushort hw_station_address[3]; /* Hardware Enet address */
    volatile ushort mc_filter_bits[4];	/* Multicast address filter bits */
    volatile ushort lance_rx_ring[2];	/* Receive Ring Address */
    volatile ushort lance_tx_ring[2];	/* Transmit Ring Address */
};

/*
 * The following structure block of vectors will get initialised
 * in case of the AM79970. A pointer to this block is passed to the
 * lance_init_idb_common() routine. If the pointer is NULL, the
 * routine will initialize the lance default vectors, otherwise
 * it will copy the vectors into the lance_instance. 
 */
typedef struct lance_vectors lance_vectors_t;
struct lance_vectors {
    /*
     * Function pointers so that the Presidio chip driver can also
     * use the lance functionality. 
     */
    get_regaddr_t 		get_regaddr;
    dev_reset_t  		reset;
    set_vectors_t 		set_vectors;
    show_structure_info_t   	show_structure_info;
    pre_init_controller_t 	ctrlr_pre_init;
    post_init_controller_t      ctrlr_post_init;
    delay_vector_t              delay_vector;
};


#define LANCE_BUFF_LO(addr) (ushort)	((ulong)(addr) & 0xFFFF)
#define LANCE_BUFF_HI(addr) (ushort)	(((ulong)(addr) >> 16) & 0xFF)

#define LANCE_GET_HI(first_word)	(first_word & 0xFF)
#define LANCE_GET_NEG_W(count)		((-count) & 0xFFFF)

#define LANCE_DS_SIZE (sizeof(lance_instance_t) + 8)
#define LANCE_IB_SIZE (sizeof(lance_initblock_t))
#define LANCE_RX_SIZE ((MAX_RING * sizeof(lance_rx_descriptor_t)) + 16)
#define LANCE_TX_SIZE ((MAX_RING * sizeof(lance_tx_descriptor_t)) + 16)

#define LANCE_MAKE_PTR(high,low)	(((high) << 16) | (low))

#define MAX_LANCE 1519
#define LANCE_CRC_SIZE 4

#define LANCE_MAX_SPIN 32

/*
 * Macro used to get actual 16Meg IO page
 */

#define LANCE_16M_PAGE(addr) ((ulong)(addr) & 0xFF000000)

/*
 * Macros to manipulate Rx and Tx descriptors
 */
#define LANCE_WRITE_RMD(rmd, start, mode)      		\
    lance_put_short(&rmd->buffer_low, LANCE_BUFF_LO(start), mode);     	\
    lance_put_byte(&rmd->buffer_high, LANCE_BUFF_HI(start), mode);     	\
    lance_put_byte(&rmd->status, LANCE_RMD_OWN, mode);	


#define LANCE_READ_RMD(rmd, rmd_status, count, mode)   	\
    (rmd_status = lance_get_byte(&rmd->status, mode), 	\
     count = lance_get_short(&rmd->packet_size, mode) - LANCE_CRC_SIZE,	\
     (rmd_status & LANCE_RMD_OWN) == 0)
 
/*
 * External declarations:
 */

extern boolean ether_debug;	/* debug flag for ethernet */

/*
 * Prototypes
 */
void lance_init_idb_common(hwidbtype *idb, lance_vectors_t *);
void lance_RX_interrupt(hwidbtype *idb);
void lance_TX_interrupt(hwidbtype *idb);
void lance_E_interrupt(ulong csr0_status, hwidbtype *idb);


/*
 * Externs
 */
extern void lance_subsys_init(subsystype *subsys);
extern void lance_update_vendor_states(hwidbtype *idb);
extern void lance_parser_init(void);
extern void lance_media_type_command(parseinfo *);
