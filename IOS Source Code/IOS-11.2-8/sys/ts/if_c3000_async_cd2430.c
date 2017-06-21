/* $Id: if_c3000_async_cd2430.c,v 3.1.16.4 1996/08/28 13:18:01 thille Exp $
 * $Source: /release/112/cvs/Xsys/ts/if_c3000_async_cd2430.c,v $
 *------------------------------------------------------------------
 * if_c3000_async_cd2430.c - Terminal port driver for Cirrus CL-CD2430 quadart
 *
 * May 1996, Samuel Li
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_async_cd2430.c,v $
 * Revision 3.1.16.4  1996/08/28  13:18:01  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.16.3  1996/06/26  19:45:43  etrehus
 * CSCdi58973:  2430 Subsystem initialized on inappropriate hardware
 * Branch: California_branch
 *
 * Revision 3.1.16.2  1996/06/18  07:27:41  irfan
 * CSCdi59224:  Split monolithic traffic[] array into per-protocol arrays
 * Branch: California_branch
 * Make the -c- igs and cobra images compile again.
 *
 * Revision 3.1.16.1  1996/06/17  08:36:00  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/05/13  18:54:11  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#define APPP_INTERNAL
#include "master.h"
#include "../../boot/cpu.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "../if/network.h"
#include "../wan/serial.h"
#include "ttysrv.h"
#include "logger.h"
#include "../les/msg_cd2430.c"		/* Not a typo, see logger.h */
#include "stacks.h"
#include "subsys.h"
#include "../parser/parser_defs_parser.h"
#include "../os/async.h"
#include "../les/if_les.h"
#include "../les/if_les_common_cd2430.h"
#include "../ts/if_les_async_cd2430.h"
#include "../h/cs_registry.regh"
#include "../wan/ppp.h"
#include "if_async.h"
#include "async_ppp.h"
#include "fastswitch_registry.h"
#include "../os/pool.h"
#include "../os/buffers.h"

static void cd2430_async_subsys_init(subsystype *);

/*
 * CD2430 async subsystem header
 * Note, we share modem interrupt level with the sync interfaces; we must
 * set up our own stack if and only if the sync interfaces are not present
 * (i.e., their subsystem is left out).
 */
#define CD2430ASYNC_MAJVERSION   1
#define CD2430ASYNC_MINVERSION   0
#define CD2430ASYNC_EDITVERSION  1
SUBSYS_HEADER(cd2430async, CD2430ASYNC_MAJVERSION, CD2430ASYNC_MINVERSION, 
              CD2430ASYNC_EDITVERSION,
	      cd2430_async_subsys_init, SUBSYS_CLASS_DRIVER,
              "seq: cd2430, cd2430sync",
              "req: cd2430");

static void cd2430_async_init(void);

/*
 * Local storage
 */
paktype *break_pak;      /* Data encoding to ask for async BREAK */
pooltype *cd2430_buffpool; /* cd2430 permanent buffers pool */
int cd2430_buffpool_group; /* group # of above buff pool    */
char cd2430_buffpool_name[] = "CD2430 I/O";      /* Name of pool */
int cd2430_perm_buffs;     /* Total permanent buffs we will need */

/*
 * cd2430_async_subsys_init()
 *	Initialize CD2430 async subsystem
 *
 * CD2430 initialization is done in two distinct phases.  The first, here,
 * provides the count of asynch TTY lines (excluding aux); we also register
 * a line init callback, which will be invoked once MODEMS[] and the like
 * is set up.
 */
static void
cd2430_async_subsys_init (subsystype *subsys)
{
    cd2430_quadart *csr;

    switch (cd2430_platform_type) {
    /*
     * Depending on the 2500 flavor that this driver is running on, the
     * initialization done will be different. If the platform is a Copan,
     * detection of the daughter card, ascertaining our clock rate,
     * figuring out the number of ttylines etc. is done here. For a 
     * Cobra, however, the bulk of this kind of initialization is done in
     * Sync driver for the CD24XX.
     */
    case PLATFORM_COPAN:
        /*
         * Probe for lss_misc register, and also probe for lower 24XX
         * controller just to be sure.
         */
        if (!touch_device((void *)ADRSPC_LSS_MISC)) {
            return;
        }
        csr = CD2430ADDRESS(0);
        if (!touch_device((void *)&csr->gfrcr)) {
            return;
        }
 
        /*
         * Ok, we're a Copan flavor.  Read the async misc. register, which will
         * tell us if we have a daughter card, and also what clock speed we're
         * driving the chips at.
         */
        lss_misc = *(ushort *)ADRSPC_LSS_MISC;
        if (!(lss_misc & LSS_DAUGHTER)) {
            cd2430ports = 16;
        } else {
            cd2430ports = 8;
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
        cd2430Base = freeLineBase;            /* our base is first free line */
        nttylines += cd2430ports;             /* add our tty lines */
        freeLineBase += cd2430ports;          /* update first free line */
        cd2430_controllers = cd2430ports / CD2430_CHIPCHAN;
        break;
    case PLATFORM_COBRA:
        /*
         * Since we are a Cobra, the bulk of the initialization has been
         * been done in the sync driver which got initialized prior to
         * this driver. Initialize the async related variables here.
         */
        ndummyttylines = 1;
        nttylines += (cd2430ports + ndummyttylines); /* add our tty lines */
        freeLineBase += (cd2430ports + ndummyttylines); /* update first free 
                                                           line */
        break;
    default:
        return;
    }

    /*
     * Arrange callback for later
     */
    reg_add_line_init(cd2430_async_init, "cd2430_async_init");

    /*
     * Initialize async fastswitching
     */
    reg_add_init_async_fs(init_async_fs, "init_async_fs");
    tty_cd2430_parser_init();
}

/*
 * cd2430_async_setup_isr_vectors ()
 *      This function will set up the vectors in the context for this
 * channel to the async interrupt service routines.
 */
static void cd2430_async_setup_isr_vectors (tt_soc *tty, int ctrl, int line)
{
    cd2430_channel_inst_t **chan_inst;

    chan_inst = &cd2430_channel_info[(ctrl * CD2430_CHIPCHAN) + line];

    /*
     * If this is the first time that we are setting up the channel context
     * malloc the channel context structure.
     */
    if (*chan_inst == NULL) {
        *chan_inst = malloc(sizeof(cd2430_channel_inst_t));
        if (*chan_inst == NULL)
            crashdump(0);
    }

    /*
     * Now set up the channel context for synchronous operation.
     */
    (*chan_inst)->aid = tty;
    (*chan_inst)->rxint_ptr  = cd2430_async_rxint;
    (*chan_inst)->txint_ptr  = cd2430_async_txint;
    (*chan_inst)->modint_ptr = cd2430_async_modint;
    (*chan_inst)->rxgoodint_ptr = cd2430_async_rxgoodint;
}

/*
 * cd2430_async_line_init()
 *	Per-line initializations for the CD2430 controller.
 */
static void
cd2430_async_line_init (int unit)
{
    tt_soc *tty;
    cd2430_quadart *csr = NULL;
    hwidbtype *idb;
    idbtype *swidb;
    char buffer[20];
    int ctrl, line;
    leveltype level;
    
    ctrl = (unit - cd2430Base)/CD2430_CHIPCHAN;
    line = (unit - cd2430Base)%CD2430_CHIPCHAN;

    /*
     * Get the pointer to the register table for this controller.
     */
    switch (cd2430_platform_type) {
    case PLATFORM_COPAN:
        csr = CD2430ADDRESS(ctrl); 
        break;
    case PLATFORM_COBRA:
        csr = CD2430ADDRESS_COBRA(ctrl);
        break;
    default:
        crashdump(0);
    }

    tty = MODEMS[unit];
    if (!tty) { 
        tty = cd2430_async_tty_init (unit, csr);
    }
    else
        tty_reset(tty);

    /*
     * Take care of the tasks needed to be done that depend on the actual
     * platform that this driver is running on.
     */

    switch (cd2430_platform_type) {
    case PLATFORM_COPAN:
        /*
         * Initialize the cd2430 registers to put the channel in the async
         * mode. 
         */
        cd2430_registers_init(tty, csr);
        /*
         * Arm receive buffers
         */
	level = raise_interrupt_level(TTY_DISABLE);
	setchan_async(csr, tty);
        async_alloc_rx_buffers(tty, csr, 0);
	reset_interrupt_level(level);
        /*
         * Set up the ISR vectors fot async mode operation.
         */
        cd2430_async_setup_isr_vectors(tty, ctrl, line);

        break;
    case PLATFORM_COBRA:
        if (!tty->tty_idb) { 
            /*
             * This is the code path taken during system initialization.
             * Create and initialize the corresponding async IDB, but do 
             * not enqueue it in the hwidbQ. It will get enqueued only
             * when there is a mode change to the async mode. Do not
             * initialize the controller registers to put the channel in
             * the async mode.
             */
            idb = init_async_idb(tty, FALSE);
            /*
             * Reset some fields in the serial 'async' IDB so that
             * they come out right.
             */
            idb->typestring = "CD2430 in async mode";
            swidb = idb->firstsw;
            idb->name = IFNAME_SERIAL;
            sprintf(buffer, "%s%d", idb->name, idb->unit);
            setstring(&swidb->namestring, buffer);
            idb->hw_namestring = swidb->namestring;
            sprintf(buffer, "%c%c%d", idb->name[0], idb->name[1], idb->unit);
            setstring(&swidb->short_namestring, buffer);
            idb->serial_flags |= SERIAL_FLAGS_SYNCASYNC;
            idb->show_controller = cd2430_async_show_controller;
            idb->status |= IDB_DELETED; /* Mark the inactive IDB as deleted
                                           to hide it from SNMP */
        }
        else { 
            /* 
             * Once the serial async IDBs have been created, this is the 
             * code path taken for initialization of a channel in the 
             * async mode. In particular, the appropriate registers will
             * now be written to initialize the channel.
             */
            cd2430_registers_init(tty, csr);
            /*
             * Arm receive buffers
             */
	    level = raise_interrupt_level(TTY_DISABLE);
	    setchan_async(csr, tty);
            async_alloc_rx_buffers(tty, csr, 0);
	    reset_interrupt_level(level);
        }
        break;
    default:
        break;
    }
    return;
}

/*
 * cd2430_async_global_init()
 *	Initializations that are global to all controllers.
 */

static void
cd2430_async_global_init (void)
{
    /*
     * Initialize private info fields for all 2430 lines
     */
    cd2430info_base = malloc_named(sizeof(cd2430_priv) * cd2430ports,
				   "cd2430 status block");
    if (!cd2430info_base) {
	errmsg(&msgsym(NOMEM, TTYDRIVER), (sizeof(cd2430_priv) * cd2430ports));
	crashdump(0);
	return;                        /* Not sure if this aborts */
    }
    bzero(cd2430info_base, sizeof(cd2430info_base) * cd2430ports);

    if (cd2430_setup_break_packet() == FALSE)
        return;

    /*
     * Define and create a private permanent buffers pool
     */

    cd2430_perm_buffs = cd2430_controllers * CD2430_CHIPCHAN * \
	                (CD2430_DEF_RXBUFF+CD2430_DEF_TXBUFF);
    cd2430_buffpool_group = pool_create_group();
    cd2430_buffpool = pak_pool_create(cd2430_buffpool_name,
					  cd2430_buffpool_group,
					  CD2430_PAKSIZE, POOL_SANITY,
					  NULL);
    if (!cd2430_buffpool) {
	errmsg(&msgsym(NOBUF, TTYDRIVER), cd2430_perm_buffs);
	crashdump(0);
	return;       /* Be paranoid */
    }
    cd2430_buffpool->fallback = pak_pool_find_by_size(CD2430_PAKSIZE);
    pool_adjust(cd2430_buffpool, 0, cd2430_perm_buffs,
		cd2430_perm_buffs, TRUE);

    cd2430_async_vector_init();

}

/*
 * Terminal line restart.
 * This function is called via a function registry and reinitializes the
 * appropriate channel of the CD2430 controller involved in the Async
 * mode.
 */
boolean cd2430_transition_to_async (hwidbtype *idb, parseinfo *csb, int type)
{
    int ctrl, line, unit;
    idbtype *old_swidb;

    if (type != PHY_LAYER_ASYNC)		/* Want async? */
	return(FALSE);
    if (idb->type == IDBTYPE_ASYNC)		/* already async? */
	return(TRUE);
    if (idb->type != IDBTYPE_CD2430)		/* possibe to transition? */
	return(FALSE);

    unit = idb->unit;
    ctrl = (unit - cd2430Base)/CD2430_CHIPCHAN;
    line = (unit - cd2430Base)%CD2430_CHIPCHAN;
    /*
     * Shutdown and unlink the currently linked IDB. Note that this must
     * be the first thing to be done in this routine. In particular,
     * this must be done before calling the async line initialization
     * routine.
     */
    old_swidb = idb->firstsw;
    shutdown_interface(idb->firstsw, TRUE);
    idb_unlink(idb);
    idb->status |= IDB_DELETED;  /* Mark the inactive IDB as deleted */

    /* Get the async IDB and enqueue it in the hwidbQ */
    idb = ((tt_soc *)(MODEMS[unit]))->tty_idb;
    idb_enqueue(idb);
    idb->status &= ~IDB_DELETED; /* Undelete the active IDB */

    /*
     * Fix up the parseinfo structure so that all subcommands get applied
     * to the correct IDB. Notify those who wish to know about the IDB
     * change.
     */
    csb->interface = idb->firstsw;
    reg_invoke_media_change_idb(old_swidb, idb->firstsw);

    /*
     * If this is a Cobra, set the sync/async bit to indicate that this
     * interface is now an async interface.
     */
    if (cd2430_platform_type == PLATFORM_COBRA) 
        (*(ushort *)CD2430_LSSDEVREGLO(idb->unit)) &= ~DEVICE_LSS_AS; 

    /*
     * Initialize the channel to be in the async mode of operation, and
     * set up the ISR vectors for async mode operation.
     */
    cd2430_async_line_init(unit);
    cd2430_async_setup_isr_vectors(idb->idb_tty, ctrl, line);
    return(TRUE);
}  

static boolean cd2430_invalid_line_specified (int line, boolean verbose)
{
    if (cd2430_platform_type != PLATFORM_COBRA)
        return (FALSE);

    if ((line < 1) || (line > (cd2430Base + cd2430ports - 1)))
        return (FALSE);

    if (line == 1) {
        if (verbose)
            printf("%%Error: TTY lines start from line 2");
        return (TRUE);
    }

    if (cd2430_channel_info[line - cd2430Base]->aid != MODEMS[line]) {
        if (verbose)
            printf("%%Error: Line tty %d is not in async mode", line);
        return (TRUE);
    }

    return (FALSE);
}

static int cd2430_async_int_base_unit (void)
{
    if (cd2430_platform_type == PLATFORM_COBRA)
        return ((cd2430Base + nttylines - ndummyttylines));
    else
        return (1);
}

/*
 * cd2430_async_init()
 *	Initialize controller structures
 *
 * This routine is called as part of system startup to enable the CD-2430
 * controller and create the associated units.
 *
 * Interrupts are disabled during this routine--it is part of early system
 * startup.
 */
static void
cd2430_async_init (void)
{
    int ctrl;
    int line;

    /*
     * First, do the global async initialization.
     */
    cd2430_async_global_init();

    /*
     * If a Copan, do the controller initialization. For the Cobra
     * this initialization is done in the Sync driver.
     */
    if (cd2430_platform_type == PLATFORM_COPAN)
        for (ctrl = 0; ctrl < cd2430_controllers; ctrl++) 
            cd2430_controller_init(ctrl);

    /*
     * Initialize all the lines in the system.
     */
    for (line = 0; line < cd2430ports; line++)
        cd2430_async_line_init(line + cd2430Base);

    /*
     * If executing on a Cobra, initialize the dummy tty line
     */
    if (cd2430_platform_type == PLATFORM_COBRA) {
        vector_init(&cd2430_dummy_vector);
        tty_init(1, READY, TTY_LINE, &cd2430_dummy_vector);
    }

    /*
     * Determine if PPP mode is allowable for this firmware.
     */
    can_ppp = cd2430_async_allow_pppmode();

    /*
     * Hook up only if we support HW async HDLC
     */
    if (can_ppp) {
            cd2430_vector.vpppmode = cd2430_pppmode;
            cd2430_vector.vpppmap = cd2430_pppmap;
    }

    /*
     * Register some functions
     */
    reg_add_tty_show(print_modemcontrol, "print_modemcontrol");
    reg_add_tty_xon(tty_xon, "tty_xon");
    reg_add_async_show_controllers(show_contr, "cd2430");
    reg_add_setup_global2(async_setup_global, "async_setup_global");
    reg_add_serial_physical_layer(cd2430_transition_to_async,
                                  "cd2430_transition_to_async");
    reg_add_invalid_line_specified(cd2430_invalid_line_specified,
                         "cd2430_invalid_line_specified");
    reg_add_async_int_base_unit(cd2430_async_int_base_unit,
                                "cd2430_async_int_base_unit");
}

cd2430_quadart *cd2430_async_show_controller_init (hwidbtype *idb, parseinfo *csb)
{
    int chip, chan;
    cd2430_quadart *reg_table;
 
    /*
     * Make sure it is us.
     */
    if ((idb->unit < cd2430Base) || (idb->unit >= (cd2430Base+cd2430ports)))
        return(FALSE);
 
    chip = (idb->unit - cd2430Base)/CD2430_CHIPCHAN;
    chan = (idb->unit - cd2430Base)%CD2430_CHIPCHAN;
 
    /*
     * Point to chip for this bank of channels. Also, set the channel access
     * register to point to the concerned channel.
     */
    reg_table = cd2430_reg_table[chip];
 
    setchan_async(reg_table, idb->idb_tty);
 
    /*
     * Print out the preamble info for this channel
     */
    printf("\nCD2430 unit %d, Channel %d, Chip Revision %02x, Microcode %x",
	   chip, chan, initial_gfrcr[chip], reg_table->gfrcr);

    return reg_table;
}
