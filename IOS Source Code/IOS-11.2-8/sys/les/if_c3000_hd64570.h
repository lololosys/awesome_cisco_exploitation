/* $Id: if_c3000_hd64570.h,v 3.3.62.1 1996/06/13 19:07:55 etrehus Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_hd64570.h,v $
 *------------------------------------------------------------------
 * if_c3000_hd64570.h -- Definitions for C3000 HD64570 serial controller
 *
 * August 1992, Greg Stovall
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: if_c3000_hd64570.h,v $
 * Revision 3.3.62.1  1996/06/13  19:07:55  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.3  1995/11/17  17:38:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:31  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:30:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/18  06:47:12  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 21:32:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Base address and register address definitions
 */
#define HD64570_BASE_ADDR       0x02132000      /* Base address */

#define MAX_HD_SERIAL_CHAN 2

/*
 * DMA channel priority setting.
 */
#define HD64570_DMA_PRIORITY    HD64570_PCR_RR

/*
 * Transmit and receive fifo thresholds
 */
#define HD64570_TX_HIGH_MARKER       0x1F00
#define HD64570_TX_LOW_MARKER        0x0017
#define HD64570_RX_MARKER            0x0F
#define HD64570_RX_BSC_MARKER        0x00

struct hd64570_instance {
    hd64570_reg_t *hd64570_reg;      /* Pointer to HD64570 registers */
    hd64570_msci_reg_t *msci_reg;    /* Pointer to MSCI for this channel */
    hd64570_dmac_reg_t *dmac_rx_reg; /* Pointer to DMAC RX register */
    hd64570_dmac_reg_t *dmac_tx_reg; /* Pointer to DMAC TX register */
    ushort *serial_dev_reg;          /* Pointer to serial device register */
    void *csu_dsu_info;              /* Integrated CSU/DSU driver data structure */
    ushort reset_mask;               /* Reset bit in control register */
    ushort rx_size, tx_size;	    /* RX and TX ring sizes */
    ushort tx_count;		   /* Count of packets currently in TX queue */

    uchar *rx_base;		     /* Address of malloced rx ring */
    hd64570_buf_ring_t *rx_ra;	     /* Aligned address of rx ring */
    hd64570_buf_descr_t *rx_last;    /* The last packet we processed */

    ulong shift_tx_cpb;           /* Shifted version of tx cpb */
    uchar *tx_base;		  /* Address of malloced tx ring */
    hd64570_buf_ring_t *tx_ra;	  /* Aligned address of tx ring */
    hd64570_buf_descr_t *tx_last; /* Last buffer we processed */

    ushort up_signal;             /* signal to use as link up/down indicator */

    ulong residual_bit;           /* Residual bit errors */

    ulong hdx_state;              /* State of the half duplex conection */

    /* Flap Protection */
    boolean flap_enabled;       /* Whether we've throttled interrupts */
    uint    flap_backoff;       /* Number of backoffs after throttling */
    sys_timestamp flap_time;    /* Time of last flap */
    uint    flap_count;         /* Number of interface flaps */
    uint    flap_rate;          /* Maximum rate of flaps per second */

    appl_periodic_t appl_periodic; /* no appliques yet, not used */
};

/*
 * Platform specific read port id. 
 */
#define HD64570_PLATFORM_READ_PORT_ID(idb) 

/*
 * Prototypes
 */
void hd64570_timer_interrupt(hwidbtype *idb);
void hd64570_dmer_pcr_write(hd64570_instance_t *ds, boolean even_flag,
			    u_char value);
int hd64570_get_cable_type(hwidbtype *idb);
void hd64570_show_serial_state(hwidbtype *, char *);
