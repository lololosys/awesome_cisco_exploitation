/* $Id: if_c4000_mk5025.h,v 3.3 1995/11/17 17:39:49 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/if_c4000_mk5025.h,v $
 *------------------------------------------------------------------
 * if_c4000_mk5025.h -- Definitions for C4000 High Speed Serial Port
 *
 * March 1992, Marciano Pitargue
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_mk5025.h,v $
 * Revision 3.3  1995/11/17  17:39:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:52  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/18  06:47:31  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 21:35:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Definitions for the i8254 timer chip used on board for DCE clocking
 */
#define I8254_BCD        0x01
#define I8254_MODE_MASK  0x07
#define I8254_MODE_SHIFT 0x01
#define I8254_CH_0       0x00
#define I8254_CH_1       0x40
#define I8254_CH_2       0x80
#define I8254_READ_BACK  0xc0

#define I8254_MODE_0     0x00
#define I8254_MODE_1     0x02
#define I8254_MODE_2     0x04
#define I8254_MODE_3     0x06
#define I8254_MODE_4     0x08
#define I8254_MODE_5     0x0a

#define I8254_RL_LATCH   0x00
#define I8254_RL_LSB     0x10
#define I8254_RL_MSB     0x20
#define I8254_RL_LSB_MSB 0x30

typedef struct i8254 i8254_t;
struct i8254 {
    volatile ushort channel0;
    volatile ushort channel1;
    volatile ushort channel2;
    volatile ushort control;
};

/*
 * The MK5025 has two registers that be directly read or written: The RAP
 * (register address pointer) and CSR (control and status register). The
 * RAP selects which of the six control and status registers is currently
 * accessable through a write or read of the CSR.
 */

typedef struct mk5025_regs mk5025_hw_reg_t;
struct mk5025_regs {
    volatile ushort control;
    volatile ushort pad[0x3f];
    volatile ushort csr;	/* The 6 control/status registers */
    volatile ushort rap;	/* Pointer to which csr and byte/short mode */
};

typedef struct xx_dsim_regs xx_dsim_regs_t;
struct xx_dsim_regs {
    volatile ushort id_prom[2];
    volatile ushort pad_0[0xff];
    volatile ushort vector;          /* 0x202 Vector/Interrupt register */
    volatile ushort pad_1[0x7e];

    mk5025_hw_reg_t mk5025_port_A;   /* 0x300 Port A MK5025/Modem Control */

    volatile ushort pad_2[0x3e];

    mk5025_hw_reg_t mk5025_port_B;   /* 0x400 Port B MK5025/Modem Control */

    volatile ushort pad_3[0xbe];

    i8254_t         i8254;           /* 0x600 i8254 timer */
};

/*
 * Status Register bits:
 */

#define STATUS_FLAGSTAT 0x0001	/* Clear when a 705 uP interrupt is pending */
#define STATUS_MAIL020	0x0002	/* Clear when a 68020 interrupt is pending */
#define STATUS_PC2	0x0004	/* High means that 705 uP mailbox TX ready */

#define STATUS_CABLE	0x00C0	/* V.35 and RS-232. See table above */
#define STATUS_CABLE_SHIFT 6	/* Number of bits to shift right */
#define STATUS_CABLE_V35   0	/* V.35 cable attached */
#define STATUS_CABLE_232   1	/* RS-232C cable attached */
#define STATUS_CABLE_449   2    /* RS-449 or X.21 cable attached */
#define STATUS_CABLE_NONE  3	/* No cable attached */

/*
 * DSIM Control Register Bits (MCSR1)
 */
#define DSIM_PORT_A_ENABLE        0x0100
#define DSIM_PORT_A_RESET         (~DSIM_PORT_A_ENABLE)
#define DSIM_PORT_B_ENABLE        0x0200
#define DSIM_PORT_B_RESET         (~DSIM_PORT_B_ENABLE)

/*
 * DSIM Port A & Port B Control Register Bits (MCSR2 & MCSR3)
 */

/*
 * Modem control interrupt enable/disable
 */
#define DSIM_CONTROL_INTR_EN      0x0001

/*
 * DTE Specific Bits
 */
#define DSIM_CONTROL_DTE_DTR      0x0004
#define DSIM_CONTROL_DTE_CTS      0x0008
#define DSIM_CONTROL_DTE_DCD      0x0010
#define DSIM_CONTROL_DTE_DSR      0x0020

/*
 * DCE Specific Bits
 */
#define DSIM_CONTROL_DCE_DCD      0x0002
#define DSIM_CONTROL_DCE_DSR      0x0004
#define DSIM_CONTROL_DCE_RTS      0x0008
#define DSIM_CONTROL_DCE_LTST     0x0010
#define DSIM_CONTROL_DCE_DTR      0x0020

/*
 * Cable Type Bits
 */
#define DSIM_CONTROL_RS232        0x0040
#define DSIM_CONTROL_RS449        0x0080

#define DSIM_CONTROL_CABLE_MASK   0x00C0

/*
 * Other Signal Control Bits
 */
#define DSIM_CONTROL_DTE_DCE_SEL  0x0100
#define DSIM_CONTROL_DTE_TI_TXC   0x0200
#define DSIM_CONTROL_DCE_TXC_SCTE 0x0400
#define DSIM_CONTROL_DTE_LTST     0x0800


#define DSIM_CABLE_TYPE(x) (((x) & STATUS_CABLE) >> STATUS_CABLE_SHIFT)

/*
 * Structure to hold the clockrate information when functioning as a DCE
 */
typedef struct xxclocktype_ {
    int rate;
    ushort counter;
} xxclocktype;

#define NXXCLOCKS 19

typedef struct mk5025_instance mk5025_instance_t;
struct mk5025_instance {

    /*
     * Driver defined structure (allocate in CPU RAM):
     * Allocate in CPU RAM and keep long words long word aligned for speed.
     */

    mk5025_hw_reg_t *regaddr;	   /* Pointer to mk5025 registers */
    xx_dsim_regs_t *dsimaddr;      /* Pointer to NIM registers */

    mk5025_initblock_t *ib;	   /* Pointer to initialization block */

    short rx_size;                 /* RX ring size */
    short tx_size;	           /* TX ring size */

    uchar *rx_base;		   /* Address of malloced rx ring */
    mk5025_rx_ring_t *rx_ra;	   /* Aligned address of rx ring */
    paktype *rx_p[MAX_RING];	   /* Paktype * for buffers in rx ring */
    uchar rx_head;		   /* Next packet we expect to see */

    uchar tx_head;		   /* Next packet we expect transmitted */
    uchar tx_tail;		   /* Last packet in tx ring */
    uchar tx_count;		   /* Count of packets currently in TX queue */
    uchar *tx_base;		   /* Address of malloced tx ring */
    mk5025_tx_ring_t *tx_ra;	   /* Aligned address of tx ring */
    paktype *tx_p[MAX_RING];	   /* Paktype * for buffers in tx ring */

    uchar *sb_base;		   /* Address of malloced status block */
    mk5025_status_buffer_t *sb_ra; /* Aligned address of status block */

    ushort reset_mask;		   /* Reset bit in control reg (unused in XX)*/

    /* Flap Protection */
    boolean flap_enabled;          /* Whether we've throttled interrupts */
    uint flap_backoff;             /* Number of backoffs after throttling */
    sys_timestamp flap_time;       /* Time of last flap */
    uint flap_count;               /* Number of interface flaps */
    uint flap_rate;                /* Maximum rate of flaps per second */

    /* Count errors: */
    ulong user_prim_err;	   /* User primitive error */
    ulong prov_prim_lost;	   /* Provider primitive lost */
    ulong spurious_prov_prim;	   /* Received unexpected provider primitive */
    ulong spurious_prim_int;	   /* Received spurious primitive interrupt */
};

/*
 * Prototypes
 */
void mk5025_enable_modem_interrupt(mk5025_instance_t *ds);
void mk5025_disable_modem_interrupt(mk5025_instance_t *ds);
void mk5025_enter_transparent_mode(hwidbtype *idb);
void mk5025_stop(hwidbtype *idb, mk5025_hw_reg_t *reg_addr);
ushort mk5025_xx_delta_interrupt(hwidbtype *idb);
void mk5025_show_structure_info(hwidbtype *idb);
void mk5025_show_cable_state(hwidbtype *idb);

/*
 * Product specific prototypes
 */
void mk5025_reset(hwidbtype *idb);
void mk5025_get_regaddr(hwidbtype *idb);
void mk5025_set_vectors(hwidbtype *idb);
