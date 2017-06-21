/* $Id: if_c3000_sync_cd2430.c,v 3.1.2.4 1996/08/22 00:19:53 etrehus Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_sync_cd2430.c,v $
 *------------------------------------------------------------------
 * if_c3000_sync_cd2430.c - Interface driver for Cirrus CL-CD2430
 *                          controller for synchronous serial
 *                          operation (for c3000 platform only)
 *
 * April 1996, Jose Hernandez
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_sync_cd2430.c,v $
 * Revision 3.1.2.4  1996/08/22  00:19:53  etrehus
 * CSCdi64284:  Cobra - frame error on async/sync ports with x25
 * Branch: California_branch
 * CSCdi65997:  DTR dropping on Cobra boxes using DDR on low speed
 * interfaces
 *
 * Revision 3.1.2.3  1996/07/04  02:07:04  jhernand
 * CSCdi61640:  Cobra code does not conform to Cisco guidelines
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/26  19:46:00  etrehus
 * CSCdi58973:  2430 Subsystem initialized on inappropriate hardware
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/06/17  08:33:40  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/05/13  18:57:47  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../../boot/cpu.h"
#include "sys_registry.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "stacks.h"
#include "sched.h"
#include "subsys.h"
#include "logger.h"
#include "../os/buffers.h"
#include "../os/free.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_half_duplex.h"
#include "../if/network.h"
#include "../les/if_les.h"
#include "../les/if_les_common_cd2430.h"
#include "../les/if_les_sync_cd2430.h"
#include "../os/pool.h"
#include "../ip/ip.h"
#include "../les/les_driver_inline.h"
#include "../tcp/tcpcompress.h"
#include "logger.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "../wan/serial_private.h"
#include "ieee.h"
#include "../ibm/sdlc.h"
#include "../wan/ppp.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"
#include "../wan/serial_parser.h"
#include "config.h"
#include "../ip/ip.h"
#include "../wan/atm_dxi.h"
#include "if_les_serial.h"
#include "../if/serial_inline.h"
#include "../if/linkdown_event.h"

#include "../les/cd2430_sync_debug.h"

#include "../h/ttysrv.h"
#include "../util/itemlist.h"

/*
 * Basic interrupt sources for the Cirrus serial interface that we always
 * wish to monitor.
 */
#define IER_SYNC_BASIC (IER_MDM | IER_RXD | IER_TXD)


volatile ulong helper_mask;      /* Flags lines to start TX upon */

/*
 * Report if the cable is plugged in
 */
inline boolean 
cable_present (cd2430_context_t *ctx)
{
    return (((*(ctx->serial_dev_reg_hi)) & COBRA_CABLE_MASK) != COBRA_CABLE_MASK);
}


inline void cd2430_driver_context_init_chan(hwidbtype *idb, cd2430_context_t *ctx)
{
    ctx->ctrl = (idb->unit - cd2430Base) / CD2430_CHIPCHAN;
    ctx->chan = (idb->unit - cd2430Base) % CD2430_CHIPCHAN;

    ctx->reg_table = cd2430_reg_table[(idb->unit - cd2430Base)/CD2430_CHIPCHAN];
}


inline void cd2430_driver_context_init_var(hwidbtype *idb, cd2430_context_t *ctx)
{
    ctx->serial_dev_reg_lo = (ushort *)CD2430_LSSDEVREGLO(idb->unit);
    ctx->serial_dev_reg_hi = (ushort *)CD2430_LSSDEVREGHI(idb->unit);

    /*
     * Initialize the helper mask for this interface.
     */

    ctx->helper_mask = (1 << (idb->unit - cd2430Base));
    ctx->led_mask = ctx->helper_mask;
}


/*
 * cd2430_sync_helper()
 *      Endless loop process to support fastswitch startoutput
 *      Block waiting for TX channels which need starting
 * When a channel on a CD2430 needs to be woken because there is
 * something to transmit, the helper_mask will be appropriately
 * set to indicate the channel. The helper mask is set in the
 * fastsend routine to signal the cd2430_sync_helper routine that
 * there is a fast-switched packet to be transmitted in the
 * output queue, and that if the transmitter is idle it needs to
 * be started.
 */
process cd2430_sync_helper (void)
{
    ulong mask;
    cd2430_context_t *ctx;
    leveltype level;
    uint port;

    while (TRUE) {
        /*
         * Block waiting for non-zero mask
         */
        while (helper_mask == 0)
            process_safe_wait_for_event();

        /*
         * The save and clear of the helper_mask is a critical
         * section. Protect it.
         */
        level = raise_interrupt_level(ALL_DISABLE);
        mask = helper_mask;
        helper_mask = 0L;
        reset_interrupt_level(level);

        for (port = 0; port < cd2430ports; port++) {
            if (mask & (1L << port)) {
                hwidbtype *idb = cd2430_sync_idbs[port];
                ctx = INSTANCE;
                if (!(ctx->outpak[0] || ctx->outpak[1])) {
                    cd2430_sync_txstart(idb);
                }
            }
        }
    }
}


/*
 * cd2430_show_cable_state()
 *
 * Show cable state for the Cirrus slow-speed serial interfaces
 */
void cd2430_show_cable_state (hwidbtype *idb)
{
    cd2430_context_t *ctx;
    serialsb *ssb;
    ushort status;

    if (cpu_type != CPU_CANCUN)
        return;

    ctx = INSTANCE;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
    status = *(ctx->serial_dev_reg_hi);
    status = ((status & COBRA_CABLE_MASK) >> COBRA_CABLE_SHIFT);

    switch (status) {
        case CANCUN_CABLE_NONE:
            printf(" No");
            break;
        case CANCUN_CABLE_232:
            printf(" RS-232");
            break;
        case CANCUN_CABLE_449:
            printf(" RS-449");
            break;
        case CANCUN_CABLE_V35:
            printf(" V.35");
            break;
        case CANCUN_CABLE_X21:
            printf(" X.21");
            break;
        case CANCUN_CABLE_RS530:
            printf(" RS-530");
            break;
        default:
            printf("Unknown (%d) ",status);
            break;
    }

    if (idb->dte_interface)
        printf(" DTE");
    else
        printf(" DCE");

    printf(" cable");

    /*
     * If a DCE print the clock rate too
     */
    if (!idb->dte_interface) {
        if (ssb->serial_clockindex)
            printf(", clockrate %d", ssb->serial_clockindex);
    }
    printf("\n");
}

/*
 * cd2430_sync_show_driver_context()
 *	This function is called by the show_controller routine for this
 * interface to display driver context information.
 */
void cd2430_sync_show_driver_context (cd2430_context_t *ctx)
{
    printf("\nDriver context information");
    printf(
"\n  Context structure 0x%x, Register table 0x%x",
    ctx, ctx->reg_table);
    printf(
"\n  Serial device register low (0x%x) is 0x%x",
    ctx->serial_dev_reg_lo, *ctx->serial_dev_reg_lo);
    printf(
"\n  Serial device register high (0x%x) is 0x%x",
    ctx->serial_dev_reg_hi, *ctx->serial_dev_reg_hi);
    printf(
"\n  Receive packet buffers 0x%x, 0x%x",
    ctx->inpak[0], ctx->inpak[1]);
    printf(
"\n  Transmit packet buffers 0x%x, 0x%x",
    ctx->outpak[0], ctx->outpak[1]);
    printf(
"\n  Interrupt rates (per second) transmit %d, receive %d, modem %d",
    ctx->txint_rate, ctx->rxint_rate, ctx->modint_rate);
    printf(
"\n  True fast-switched packets\t%d", ctx->fs_count);
    printf(
"\n  Semi fast-switched packets\t%d", ctx->sfs_count);
    printf(
"\n  Transmitter hang count\t%d", ctx->txhang_cnt);
    printf(
"\n  Residual indication count\t%d", ctx->resind_cnt);
    printf(
"\n  Bus error count\t%d", ctx->buserr_cnt);
    printf(
"\n  Aborted short frames count\t%d", ctx->aborted_short_cnt);
}

void cd2430_show_channel_info (hwidbtype *idb)
{
    int chip, chan;
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;

    ctx = INSTANCE;
    chip = ctx->ctrl;
    chan = ctx->chan;

    reg_table = ctx->reg_table;

    /*
     * Print out the preamble info for this channel
     */
    printf("\nCD2430 unit %d, Channel %d, Chip Revision %02x, Microcode %x",
           chip, chan, initial_gfrcr[chip], reg_table->gfrcr);
}

/*
 * This function is called via a list registry to change the mode of a
 * sync/async interface to the sync mode. The currently active IDB for
 * this interface is passed in as a parameter.
 */
static boolean
cd2430_transition_to_sync (hwidbtype *idb, parseinfo *csb, int type)
{
    int unit;
    idbtype *old_swidb;

    if (type != PHY_LAYER_SYNC ||		/* Want sync? */
	idb->type != IDBTYPE_ASYNC)		/* But not currntly async? */
	return(FALSE);
    /*
     * Make sure that this seems to be a 2430 device.  Shouldn't this be
     * a flag somewhere inside the idb so we don't need to make such guesses?
     */
    if ((idb->unit < cd2430Base) || (idb->unit >= (cd2430Base+cd2430ports)))
        return(FALSE);

    unit = idb->unit;
    old_swidb = idb->firstsw;

    /*
     * Shutdown and unlink the currently active IDB. Note that this must
     * the first thing you must do in this routine.
     */
    shutdown_interface(idb->firstsw, TRUE);
    idb_unlink(idb);
    idb->status |= IDB_DELETED; /* Mark the inactive IDB as deleted */

    /*
     * Get the sync IDB for this interface, and enqueue it in the
     * hwidbQ.
     */
    idb = cd2430_sync_idbs[unit - cd2430Base];
    idb_enqueue(idb);
    idb->status &= ~IDB_DELETED; /* Undelete the active IDB */

    /*
     * Fix up the parseinfo structure so that all subcommands that follow
     * get applied to the right IDB. Notify those who wish to know about
     * the IDB change.
     */
    csb->interface = idb->firstsw;
    reg_invoke_media_change_idb(old_swidb, idb->firstsw);

    /*
     * If this is a Cobra, set the sync/async bit to indicate that this
     * interface is now a sync interface.
     */
    if (cd2430_platform_type == PLATFORM_COBRA) 
        (*(ushort *)CD2430_LSSDEVREGLO(idb->unit)) |= DEVICE_LSS_AS;

    /*
     * Now, reset this IDB so that the controller is put in the sync mode.
     */
    cd2430_sync_mode_init(idb);
    return(TRUE);
}



/*************************************************************************
 *     Initialization Related Functions for the CD2430 Sync Driver
 * 
 * The routines used in the initialization for this driver. The
 * initialization consists of the subsystem initialization and the
 * initialization of an IDB when it is first created.
 *************************************************************************/


/*
 * Subsystem Initialization
 */
static void cd2430_sync_subsys_init (subsystype *subsys)
{
    hwidbtype *idb;
    int ctrl, portnum;

    if (cd2430_platform_type != PLATFORM_COBRA)
      return;

    /*
     * Okay, so we are a Cobra flavor. Read the miscellaneous register
     * and figure out if we have a daughter card. Also, figure out
     * from the same register what clock speed the controller is
     * driven at.
     */
    lss_misc = *(ushort *)ADRSPC_LSS_MISC;
    if (!(lss_misc & LSS_DAUGHTER)) {
        cd2430ports = 8;
    }
    else {
        cd2430ports = 2;
    }

    switch (lss_misc & LSS_CLK) {
    case LSS_CLK30:
        cd2430clock = 30*1000000;
        break;
    case LSS_CLK27:
        cd2430clock = 27*1000000;
        break;
    case LSS_CLK25:
        cd2430clock = 25*1000000;
        break;
    case LSS_CLK20:
        cd2430clock = 20*1000000;
        break;
    }

    cd2430Base = nserialnets;
    nserialnets += cd2430ports;
    nas_serialnets += cd2430ports;  /* Number of sync/async interfaces */
    cd2430_controllers = cd2430ports / CD2430_CHIPCHAN;
    /*
     * Take care of the possibility that not all channels on a controller
     * may be used.
     */
    if (cd2430ports % CD2430_CHIPCHAN )
        cd2430_controllers++;

    /*
     * Initialize the global context for all cd2430 controllers.
     */
    for (ctrl = 0; ctrl < cd2430_controllers; ctrl++)
        cd2430_controller_init(ctrl);

    /*
     * Create the synchronous serial IDBs for all the cd2430 ports.
     */
    for (portnum = 0; portnum < cd2430ports; portnum++) {
        idb = idb_create();
        idb->unit = cd2430Base + portnum;
        cd2430_init_sync_idb(idb);
    }

    /*
     * Launch the fast-switching helper.
     */
    helper_mask = 0L;
    helper_pid = process_create(cd2430_sync_helper, "SYNCCD2430 Helper", 
                                NORMAL_STACK, PRIO_NORMAL);

    /*
     * Add required function registries for this subsystem.
     */
    reg_add_media_set_rxoffset(IDBTYPE_CD2430, cd2430_set_rxoffset,
                               "cd2430_set_rxoffset");
    reg_add_serial_physical_layer(cd2430_transition_to_sync,
                                  "cd2430_transition_to_sync");
    reg_add_media_transmit_delay(IDBTYPE_CD2430, cd2430_set_transmit_delay,
                                 "cd2430_set_transmit_delay");
    reg_add_media_half_or_full_duplex_cmd(IDBTYPE_CD2430,
                                          serial_half_duplex_command,
                                          "serial_half_duplex_command");
}

/*
 * CD2430 sync subsystem header
 */
#define CD2430SYNC_MAJVERSION   1
#define CD2430SYNC_MINVERSION   0
#define CD2430SYNC_EDITVERSION  1
SUBSYS_HEADER(cd2430sync, CD2430SYNC_MAJVERSION, CD2430SYNC_MINVERSION,
        CD2430SYNC_EDITVERSION, cd2430_sync_subsys_init, SUBSYS_CLASS_DRIVER,
        "seq: cd2430",
        "req: cd2430");

