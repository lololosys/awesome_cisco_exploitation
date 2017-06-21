/* $Id: if_c3000_pcbus.h,v 3.3 1995/11/17 17:38:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/if_c3000_pcbus.h,v $
 *------------------------------------------------------------------
 * if_c3000_pcbus.h -- Definitions for Fiji PC bus Port
 *
 * April 1994, Johnny Chan
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c3000_pcbus.h,v $
 * Revision 3.3  1995/11/17  17:38:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:48  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  06:47:06  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:33:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */


#define FIJI_SHRAM_INITIALIZED   (*(ushort *)ADRSPC_FIJI_CONTROL \
                                 & FIJI_SHRAM_STATUS_FLAG)
#define FIJI_SHRAM_STATUS        (*(ds->regaddr) & FIJI_SHRAM_STATUS_FLAG)
#define PCBUS_SHRAM_MAGIC  0x41435A56
#define PCBUS_DS_SIZE     sizeof(pcbus_instance_t)
#define PCBUS_SHUTDOWN    0x2BEEDEAD
#define NEXT_READ_PACKET   goto next_RX_packet; 
#define PCBUS_MIN_PKT_SIZE sizeof(pcbus_frame_t)
#define PCBUS_FRAME_INC   (sizeof(ulong) + 3)
#define MAX_FIJI_RX_PACKET 16

/*
 * PC Bus packet structure
 */
typedef struct pcbus_frame {
    ulong frm_len;    /* Size excluding this field */
    ushort pcbus_station;
    ushort pcbus_hdlctype;
    uchar  data_strt[4];           /* start of data */
} pcbus_frame_t;

/*
 * This structure is meant to be overlayed onto memory to provide
 * direct access to the PC ISA bus Shared RAM interface. 
 */
typedef struct pc_if_ptrs pc_if_ptrs_t;
struct pc_if_ptrs {
    volatile ulong offset;      /* start of buffer */
    volatile ulong oflow;       /* buffer overflows */
    volatile ushort size;       /* buffer size */
    volatile ushort get_ptr;    /* buffer get pointer */
    volatile ushort put_ptr;    /* buffer put pointer */
    volatile ushort wrap_ptr;   /* buffer wrap pointer */
};

typedef struct shram_block shram_block_t;
struct shram_block {
    volatile ulong magic;       /* Magic no */
    volatile ushort ver_major;  /* version major no */
    volatile ushort ver_minor;  /* version minor no */
    volatile ulong shram_size;  /* Shared RAM total size */
    volatile ulong state;       /* Shutdown or UP state */
    pc_if_ptrs_t tx_buff;      /* Tx buffer pointers */
    pc_if_ptrs_t rx_buff;      /* Rx buffer pointers */
};

typedef struct pcbus_instance pcbus_instance_t;
struct pcbus_instance {

    /*
     * Driver defined structure (allocate in CPU RAM):
     * Allocate in CPU RAM and keep long words long word aligned for speed.
     */

    volatile ushort  *regaddr;  /* control and status register */
    shram_block_t *ifaddr;	/* Pointer to PC interface control block */
    shram_block_t c_blk;        /* Copy of Shared RAM control block */

    ulong tx_count;		/* Count of packets currently in TX queue */
    volatile ushort status;     /* Shared RAM initialization status */ 

    uchar *buff;                /* Offset to start of a buffer */
    pc_if_ptrs_t *rx_ptr;      /* ptr to Rx buffer in c_blk */
    pc_if_ptrs_t *tx_ptr;      /* ptr to Tx buffer in c_blk */

    int     intr_cnt;           /* Interrupt count */
};

/*
 * External declarations:
 */
extern boolean pcbus_debug;


/*
 * Prototypes
 */
extern void pcbus_debug_init(void);
extern void pcbus_parser_init(void);
