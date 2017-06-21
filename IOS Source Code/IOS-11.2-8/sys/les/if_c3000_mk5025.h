/* $Id: if_c3000_mk5025.h,v 3.3 1995/11/17 17:38:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/if_c3000_mk5025.h,v $
 *------------------------------------------------------------------
 * if_c3000_mk5025.h -- Definitions for Pancake High Speed Serial Port
 * 
 * March 1991, Marciano Pitargue
 *             Steve Southam
 * March 1990, Chris Shaker
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_mk5025.h,v $
 * Revision 3.3  1995/11/17  17:38:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:43  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/18  06:47:16  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 21:33:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Defines for magic flap hack
 */
#define MODEM_CONTROL_INT	0x80	/* Enable the modem control int. */

/*
 * This structure is meant to be overlayed onto memory to provide
 * direct access to the mk5025 registers. The MK5025 has two
 * on board registers that can be directly read or written: The RAP
 * (register address pointer) and CSR (control and status register). 
 * The RAP selects which of the six control and status registers is 
 * currently accessable through a write or read of the CSR.
 * Following these registers is a block of bytes meant to compensate
 * for the unused memory between the csr/rap registers and the
 * serial control register (modem control register). The number of
 * bytes is constant between any csr/rap, control pair. The serial
 * control register is used to read/write the serial line signals:
 * DTR, DSR, DCD and CTS.
 */
typedef struct mk5025_regs mk5025_hw_reg_t;
struct mk5025_regs {
    volatile ushort csr; /* The 6 control/status registers */
    volatile ushort rap; /* ptr to which csr and byte/short mode */
    volatile char pad[0x3c]; /* pad for unused mem between registers */
    volatile uchar signal_control; /* serial signal control register */
    volatile uchar interrupt_control; /* serial interrupt control register */

};

typedef volatile uchar serial_register_t;

typedef struct mk5025_instance mk5025_instance_t;
struct mk5025_instance {

    /*
     * Driver defined structure (allocate in CPU RAM):
     * Allocate in CPU RAM and keep long words long word aligned for speed.
     */

    mk5025_hw_reg_t *regaddr;	/* Pointer to mk5025 registers */

    mk5025_initblock_t *ib;	/* Pointer to Hardware initialization block */

    short rx_size, tx_size;	/* RX and TX ring sizes */

    uchar *rx_base;		/* Address of malloced rx ring */
    mk5025_rx_ring_t *rx_ra;	/* Aligned address of rx ring */
    paktype *rx_p[MAX_RING];	/* Paktype * for buffers in rx ring */
    uchar rx_head;		/* Next packet we expect to see */

    uchar tx_head;		/* Next packet we expect transmitted */
    uchar tx_tail;		/* Last packet in tx ring */
    uchar tx_count;		/* Count of packets currently in TX queue */
    uchar *tx_base;		/* Address of malloced tx ring */
    mk5025_tx_ring_t *tx_ra;	/* Aligned address of tx ring */
    paktype *tx_p[MAX_RING];	/* Paktype * for buffers in tx ring */

    uchar *sb_base;		/* Address of malloced status block */
    mk5025_status_buffer_t *sb_ra; /* Aligned address of status block */

    ushort reset_mask;		/* Reset bit in control register */

    /* Flap Protection */
    boolean flap_enabled;       /* Whether we've throttled interrupts */
    uint    flap_backoff;       /* Number of backoffs after throttling */
    sys_timestamp flap_time;    /* Time of last flap */
    uint    flap_count;         /* Number of interface flaps */
    uint    flap_rate;          /* Maximum rate of flaps per second */

    /* Count errors: */
    ulong user_prim_err;	/* User primitive error */
    ulong prov_prim_lost;	/* Provider primitive lost */
    ulong spurious_prov_prim;	/* Received unexpected provider primitive */
    ulong spurious_prim_int;	/* Received spurious primitive interrupt */
};

/*
 * Prototypes
 */
void mk5025_enable_modem_interrupt(mk5025_instance_t *ds);
void mk5025_disable_modem_interrupt(mk5025_instance_t *ds);
void mk5025_get_regaddr(hwidbtype *idb);
void mk5025_set_vectors(hwidbtype *idb);
void mk5025_enter_transparent_mode(hwidbtype *idb);
void mk5025_reset(hwidbtype *idb);
void mk5025_stop(hwidbtype *idb, mk5025_hw_reg_t *reg_addr);
void mk5025_show_structure_info(hwidbtype *idb);
void mk5025_show_cable_state(hwidbtype *idb);

extern volatile ushort *status_ptr; /* Address of status register */
