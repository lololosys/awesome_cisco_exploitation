/* $Id: if_c3000_sync_cd2430.h,v 3.1.2.4 1996/08/22 00:19:55 etrehus Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_sync_cd2430.h,v $
 *------------------------------------------------------------------
 * if_c3000_sync_cd2430.h - Definitions for synchronous serial driver
 *                          for Cirrus CL-CD2430 controller (for c3000
 *                          platform only)
 *
 * May 1996, Jose Hernandez
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_sync_cd2430.h,v $
 * Revision 3.1.2.4  1996/08/22  00:19:55  etrehus
 * CSCdi64284:  Cobra - frame error on async/sync ports with x25
 * Branch: California_branch
 * CSCdi65997:  DTR dropping on Cobra boxes using DDR on low speed
 * interfaces
 *
 * Revision 3.1.2.3  1996/08/10  01:08:34  billw
 * CSCdi54603:  make cirrus hardware gotchas more obvious in source code
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/07/04  02:07:09  jhernand
 * CSCdi61640:  Cobra code does not conform to Cisco guidelines
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/06/17  08:33:42  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/05/13  18:57:30  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __IF_C3000_SYNC_CD2430_H__
#define __IF_C3000_SYNC_CD2430_H__


#define COBRA_PRE_DIVIDER_MASK	0xFF9F       /* Pre-divider mask */

/*
 * Current context of a CD2430 port in the Sync mode.
 */
typedef struct cd2430_context {
    paktype *inpak[2];          /* The two packets armed for Receive DMA */
    paktype *outpak[2];         /* The two packets armed for Transmit DMA */
    cd2430_quadart *reg_table;  /* The register table for the controller */
    encapstype enctype;         /* Recorded encapsulation type */
    int base_mode;              /* HW chip mode */
    int cur_mode;               /* Mode currently down on the chip */
    boolean ingiant;            /* Have seen giant packet */
    int ctrl;                   /* Controller number for this interface */
    int chan;                   /* Channel number for this interface */

    boolean cable_present;      /* Note if serial cable is inserted */
    boolean reset_interface;    /* Signal that interface be reset by the *
                                 * periodic function for this interface */
    boolean ignore_local_loopback; /* Flag to indicate that DCE interface is to
                                    * ignore the local loopback signal */

    /* Half-duplex transmit FSM */
    void (*hdx_tx_fsm)(hwidbtype *, enum HDX_EVENT); 

    /* Half-duplex receive FSM  */
    void (*hdx_rx_fsm)(hwidbtype *, ushort, enum HDX_EVENT);  

    /* Half-duplex states */
    enum HDX_TX_STATE hdx_tx_state;   /* Half-duplex transmit FSM state */
    enum HDX_RX_STATE hdx_rx_state;   /* Half-duplex receive FSM state  */

    /*
     * Half-duplex operation related timers
     */
    cd2430_timer_t *rts_timeout;   /* Time to wait after RTS is asserted by
                                     the DTE for CTS to get asserted */
    cd2430_timer_t *cts_delay;     /* Time for which the DCE waits after RTS
                                     is asserted to assert CTS */
    cd2430_timer_t *rts_drop_delay; /* Time for which the DTE waits after final
                                      frame transmission to deassert RTS */
    cd2430_timer_t *cts_drop_timeout; /* Time to wait after RTS is deasserted
                                        for CTS to get deasserted */
    cd2430_timer_t *dcd_txstart_delay; /* Time to wait after DCD is
                                      asserted in controlled carrier mode
                                      before transmission should start */
    cd2430_timer_t *dcd_drop_delay; /* Time to wait after
                                      transmission is done in controlled
                                      carrier mode before DCD can be 
                                      deasserted by the DCE */
    cd2430_timer_t *transmit_delay; /* Time delay prior to start of 
                                      transmission. The default value for
                                      this delay will be zero ms */

    /*
     * Fast-switching and error information counters
     */
    ulong fs_count;             /* Number of packets truly fast-switched */
    ulong sfs_count;            /* Number of packets semi fast-switched */
    ulong false_event_counters[HDX_MAX_EVENTS];
                                /* Counters for false half-duplex events */
    ulong cts_deassertion_fail; /* Count of CTS deassertion failures */

    /* Variables related to protection against flapping interfaces */
    boolean flap_enabled;       /* Whether we've throttled interrupts */
    uint    flap_backoff;       /* Number of backoffs after throttling */
    sys_timestamp flap_time;    /* Time of last flap */
    uint    flap_count;         /* Number of interface flaps */
    uint    flap_rate;          /* Maximum rate of flaps per second */

    void (*appl_periodic)(hwidbtype *);  /* no appliques yet, not used */

    /*
     * Half-duplex debug logging buffer.
     */
    hdx_log_element_t *hdx_log;
    ulong hdx_log_index;

    /*
     * Interrupt rate data
     */
    ulong txint_cnt;	/* Count of transmit interrupts for this interface */
    ulong rxint_cnt;	/* Count of receive interrupts for this interface */
    ulong modint_cnt;	/* Count of modem interrupts for this interface */
    ulong txint_rate;   /* Rate of incoming transmit interrupts */
    ulong rxint_rate;   /* Rate of incoming receive interrupts */
    ulong modint_rate;  /* Rate of incoming modem interrupts */
    sys_timestamp duration; /* Timestamp to calculate elapsed time */

    ulong test_tx_pak_cnt;        /* Count of test packets to be sent */
    ulong nested_tx_ints;         /* Count of nested transmit interrupts */
    ulong nested_rx_ints;         /* Count of nested receive interrupts */
    ulong nested_modem_ints;      /* Count of nested modem interrupts */
    ulong resind_cnt;             /* Count of residual indications */
    ulong buserr_cnt;             /* Count of CD2430 bus errors */
    ulong aborted_short_cnt;      /* Count of aborted short frames */
    ulong txhang_cnt;             /* Count of transmitter hangs */
/*
 * Platform-dependent elements of context of CD2430 port
 */

    ushort *serial_dev_reg_lo;  /* Pointer to the low LSS device register */
    ushort *serial_dev_reg_hi;  /* Pointer to the high LSS device register */
    ulong  helper_mask;         /* Helper mask for this interface */
    ushort led_mask;            /* LED register mask for this interface */

} cd2430_context_t;


/*************************************************************************
 *         Inline Functions for the CD2430 Sync Driver
 *
 * The following functions have all been inlined because they are in
 * interrupt processing path for this driver where speed is of the
 * essence.
 *************************************************************************/

/*
 * General Notes on the Modem Signals for the Cirrus Serial Interfaces on
 *                      the Cobra Platform 
 *
 * The CD2430 has 2 general purpose output signal pins, and 3 general
 * purpose input signal pins that can be used for modem signals. The data
 * book has given representative names to these input and output pins but
 * in practice these pins need not necessarily be used as the signals
 * that the data book uses to name them. Further, the use of these pins 
 * will depend on whether the interface is in DTE mode ore DCE mode.
 *
 * Note that in DCE mode, we need three output pins, viz. DCD, DSR and
 * CTS while the CD2430 provides us with just two output signals. To
 * satisfy the need for a third output signal, we use the least
 * significant bit of the low byte of the LSS device register to drive
 * the DCD signal while in the DCE mode.
 * 
 * We list below the mapping between the signals on the CD2430 controller
 * as it is named in the data book and the signal to maps to on the 5-in-1
 * connector. Note that some of the signals are driven by bits in the
 * low byte of the serial device register for the Cirrus serial interfaces.
 * ******  REFERENCES TO RTS AND DTR are SWAPPED for HW flow ctrl ******
 * See comments in ts/tty_cd2430.c
 * 
 * I. DTE Mode
 *       CD2430 Pin              Cable Signal
 *       ==========              ============
 *       RTS                     DTR
 *       Bit 3 of LSS Reg Lo     RTS
 *       DCD                     DSR
 *       DSR                     DCD
 *       CTS                     CTS
 *
 * II. DCE Mode
 *       CD2430 Pin              Cable Signal
 *       ==========              ============
 *       RTS                     DSR
 *       Bit 0 of LSS Reg Lo     DCD
 *       Bit 3 of LSS Reg Lo     CTS
 *       DCD                     DTR
 *       CTS                     RTS
 * 
 * The following inlines also make use of the implicit assumption that
 * only certain signals may be asserted or deasserted depending on the
 * mode (DTE or DCE) of the interface. That is, in DTE mode the DTR and
 * RTS signals may be asserted/deasserted while in the DCE mode only
 * the signals DCD, DSR and CTS may be asserted/deasserted. Using this fact
 * along with the signal mapping described above, the following inline
 * functions to assert, deassert and test the state of the modem signals
 * will become obvious. 
 */

/*
 * Assert the DTR signal
 */
static inline void
assert_DTR (hwidbtype *idb, cd2430_quadart *reg_table)
{
    reg_table->msvr_rts |= MSVR_RTS;
    if (serial_debug)
        buginf("\nCIRRUS(%d): Asserting %s ", idb->unit, "DTR");
}

/*
 * Deassert the DTR signal
 */
static inline void
deassert_DTR (hwidbtype *idb, cd2430_quadart *reg_table)
{
    reg_table->msvr_rts &= ~MSVR_RTS;
    if (serial_debug)
        buginf("\nCIRRUS(%d): Deasserting %s ", idb->unit, "DTR");
}

/*
 * Assert the RTS signal
 */
static inline void
assert_RTS (hwidbtype *idb, cd2430_context_t *ctx)
{
    *(ctx->serial_dev_reg_lo) |= DEVICE_LSS_RTS_CTS;
    if (serial_debug)
        buginf("\nCIRRUS(%d): Asserting %s ", idb->unit, "RTS");
}

/*
 * De-assert the RTS signal
 */
static inline void
deassert_RTS (hwidbtype *idb, cd2430_context_t *ctx)
{
    *(ctx->serial_dev_reg_lo) &= ~DEVICE_LSS_RTS_CTS;
    if (serial_debug)
        buginf("\nCIRRUS(%d): Deasserting %s ", idb->unit, "RTS");
}

/*
 * Assert the DCD signal
 */
static inline void 
assert_DCD (hwidbtype *idb, cd2430_context_t *ctx)
{
    *(ctx->serial_dev_reg_lo) |= DEVICE_LSS_DCD;
    if (serial_debug)
        buginf("\nCIRRUS(%d): Asserting %s ", idb->unit, "DCD");
}

/*
 * Deassert the DCD signal
 */
static inline void
deassert_DCD (hwidbtype *idb, cd2430_context_t *ctx)
{
    *(ctx->serial_dev_reg_lo) &= ~DEVICE_LSS_DCD;
    if (serial_debug)
        buginf("\nCIRRUS(%d): Deasserting %s ", idb->unit, "DCD");
}

/*
 * Assert the DSR signal
 */
static inline void
assert_DSR (hwidbtype *idb, cd2430_quadart *reg_table)
{
    reg_table->msvr_rts |= MSVR_RTS;
    if (serial_debug)
        buginf("\nCIRRUS(%d): Asserting %s ", idb->unit, "DSR");
}

/*
 * Deassert the DSR signal
 */
static inline void
deassert_DSR (hwidbtype *idb, cd2430_quadart *reg_table)
{
    reg_table->msvr_rts &= ~MSVR_RTS;
    if (serial_debug)
        buginf("\nCIRRUS(%d): Deasserting %s ", idb->unit, "DSR");
}

/*
 * Assert the CTS signal
 */
static inline void
assert_CTS (hwidbtype *idb, cd2430_context_t *ctx)
{
    *(ctx->serial_dev_reg_lo) |= DEVICE_LSS_RTS_CTS;
    if (serial_debug)
        buginf("\nCIRRUS(%d): Asserting %s ", idb->unit, "CTS");
}

/*
 * De-assert the CTS signal
 */
static inline void
deassert_CTS (hwidbtype *idb, cd2430_context_t *ctx)
{
    *(ctx->serial_dev_reg_lo) &= ~DEVICE_LSS_RTS_CTS;
    if (serial_debug)
        buginf("\nCIRRUS(%d): Deasserting %s ", idb->unit, "CTS");
}

/*
 * Assert the loopback bit
 */
static inline void
assert_loopback (hwidbtype *idb, cd2430_context_t *ctx)
{
    *(ctx->serial_dev_reg_lo) |= DEVICE_LSS_LTST;
    if (serial_debug)
        buginf("\nCIRRUS(%d): Asserting %s ", idb->unit, "LTST");
}

/*
 * Deassert the loopback bit
 */
static inline void
deassert_loopback (hwidbtype *idb, cd2430_context_t *ctx)
{
    *(ctx->serial_dev_reg_lo) &= ~DEVICE_LSS_LTST;
    if (serial_debug)
        buginf("\nCIRRUS(%d): Deasserting %s ", idb->unit, "LTST");
}

/*
 * Report if the loopback bit is asserted when the interface is in
 * DCE mode.
 */
static inline boolean
DCE_loopback_is_asserted (cd2430_context_t *ctx)
{
    return ((*(ctx->serial_dev_reg_lo)) & DEVICE_LSS_LTST);
}

/*
 * Report if the CTS input is asserted when the interface is in
 * DTE mode.
 */
static inline boolean
DTE_CTS_is_asserted (cd2430_quadart *reg_table)
{
    return (reg_table->msvr_dtr & MSVR_CTS);
}
 
/*
 * Report if the CTS output is asserted when the interface is in
 * DCE mode.
 */
static inline boolean
DCE_CTS_is_asserted (cd2430_context_t *ctx)
{
    return(*(ctx->serial_dev_reg_lo) & DEVICE_LSS_RTS_CTS);
}
 
/*
 * Report if the RTS output is asserted when the interface is in
 * DTE mode.
 */
static inline boolean
DTE_RTS_is_asserted (cd2430_context_t *ctx)
{
    return(*(ctx->serial_dev_reg_lo) & DEVICE_LSS_RTS_CTS);
}
 
/*
 * Report if the RTS input is asserted when the interface is in
 * DCE mode.
 */
static inline boolean
DCE_RTS_is_asserted (cd2430_quadart *reg_table)
{
    return(reg_table->msvr_rts & MSVR_CTS);
}
 
/*
 * Report if the DCD input is asserted when the interface is in
 * DTE mode.
 */
static inline boolean
DTE_DCD_is_asserted (cd2430_quadart *reg_table)
{
    return(reg_table->msvr_dtr & MSVR_DSR);
}
 
/*
 * Report if the DCD output is asserted when the interface is in
 * DCE mode.
 */
static inline boolean
DCE_DCD_is_asserted (cd2430_context_t *ctx)
{
    return(*(ctx->serial_dev_reg_lo) & DEVICE_LSS_DCD);
}
 
/*
 * Report if the DSR input is asserted when the interface is in
 * DTE mode.
 */
static inline boolean
DTE_DSR_is_asserted (cd2430_quadart *reg_table)
{
    return (reg_table->msvr_dtr & MSVR_CD);
}
 
/*
 * Report if the DSR output is asserted when the interface is in
 * DCE mode.
 */
static inline boolean
DCE_DSR_is_asserted (cd2430_quadart *reg_table)
{
    return (reg_table->msvr_rts & MSVR_RTS);
}
 
/*
 * Report if the DTR output is asserted when the interface is in
 * DTE mode.
 */
static inline boolean
DTE_DTR_is_asserted (cd2430_quadart *reg_table)
{
    return (reg_table->msvr_rts & MSVR_RTS);
}
 
/*
 * Report if the DTR input is asserted when the interface is in
 * DCE mode.
 */
static inline boolean
DCE_DTR_is_asserted (cd2430_quadart *reg_table)
{
    return (reg_table->msvr_rts & MSVR_CD);
}

/*
 * Report if this is a DTE interface
 */
static inline boolean
DTE_interface (cd2430_context_t *ctx)
{
    return (*(ctx->serial_dev_reg_hi) & DEVICE_LSS_DTE);
}

extern cookie_t *cookie;
extern int helper_pid;
extern ulong helper_mask;
extern char *dce_only_msg;
extern boolean cable_present(cd2430_context_t *);
extern process cd2430_sync_helper(void);

extern void setchan_sync(cd2430_quadart *, hwidbtype *);
extern void cd2430_sync_show_driver_context(cd2430_context_t *);
extern void cd2430_driver_context_init_chan(hwidbtype *, cd2430_context_t *);
extern void cd2430_driver_context_init_var(hwidbtype *, cd2430_context_t *);
extern void cd2430_set_clock_option_regs(hwidbtype *, int);
extern void cd2430_clockrate_command(parseinfo *);
extern void cd2430_sync_txstart(hwidbtype *idb);
extern void cd2430_init_sync_idb(hwidbtype *idb);
extern void cd2430_set_rxoffset(hwidbtype *idb);
extern void cd2430_set_transmit_delay(hwidbtype *idb, int value);
extern void cd2430_sync_mode_init(hwidbtype *);
extern void cd2430_show_cable_state(hwidbtype *);
extern void cd2430_show_channel_info(hwidbtype *);

#endif  /* of __IF_C3000_SYNC_CD2430_H__ */
