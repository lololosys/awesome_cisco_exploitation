/* $Id: tty_as5200_cd2430.c,v 3.1.52.8 1996/08/28 13:18:21 thille Exp $
 * $Source: /release/112/cvs/Xsys/ts/tty_as5200_cd2430.c,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * January 1996, Tony Wu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Special note on Hardware flow control:
 *
 * The cd2430 is capable of supporting HW flow control without CPU 
 * intervention.  It will stop outputting when the CTS pin goes false,
 * and will de-assert DTR if internal buffers fill up for some reason.
 * Note that most external devices expect *RTS* to be deasserted for
 * flow control purposes.  For this reason, cisco HW uses the 2430 pin
 * labeled DTR as the RTS signal in external connectors, and moves the
 * external DTR signal is connected to the RTS chip lead.  Thus, within
 * the source code and for all practical purposes, 
 *     **** REFERENCES TO RTS AND DTR are SWAPPED ******
 *
 *------------------------------------------------------------------
 * $Log: tty_as5200_cd2430.c,v $
 * Revision 3.1.52.8  1996/08/28  13:18:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.52.7  1996/08/10  01:08:45  billw
 * CSCdi54603:  make cirrus hardware gotchas more obvious in source code
 * Branch: California_branch
 *
 * Revision 3.1.52.6  1996/07/04  00:29:11  irfan
 * CSCdi61001:  Autoselect throws away characters on startup
 * Branch: California_branch
 * Add "flush-at-activation" line subcommand to the ts system.
 *
 * Revision 3.1.52.5  1996/06/28  23:31:08  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.52.4  1996/06/28  08:55:41  elem
 * CSCdi59885:  As5200 drops RTS signal permanently during async stress
 * testing
 * Branch: California_branch
 * The fix uses the async idb's throttling mechanism to unthrottle
 * the port.  The network.c's net_restart_throttle will call back
 * to the CD2430 driver to re-arm the Cirrus Rx DMA.
 * This fixed for hardware PPP mode only because only time I know
 * the IDB is active.
 *
 * Revision 3.1.52.3  1996/06/18  16:52:59  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  Commit missed file.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.1.52.2  1996/06/18  01:49:49  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.1.52.1  1996/06/16  21:20:16  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/01/10  22:33:09  towu
 * created.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define APPP_INTERNAL
#include "master.h"
#include "sys_registry.h"
#include "packet.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "logger.h"
#include "../les/msg_cd2430.c"		/* Not a typo, see logger.h */
#include "../src-68-as5200/as5200.h"
#include "stacks.h"
#include "subsys.h"
#include "../os/async.h"
#include "../les/if_les.h"
#include "../h/cs_registry.regh"
#include "../wan/ppp.h"
#include "if_async.h"
#include "async_ppp.h"
#include "fastswitch_registry.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../dev/flash_defines.h"
#include "../les/if_as5200_tdm.h"
#include "../les/if_as5200_modem.h"
#include "as5200_cd2430.h"
#include "../asm-m68k/asm_as5200_cd2430.h"

static int cd2430_buffpool_group; /* group # of above buff pool    */
static char cd2430_buffpool_name[] = "CD2430 I/O";      /* Name of pool */
static int cd2430_perm_buffs;     /* Total permanent buffs we will need */
tt_soc **cd2430_ttyptr[CD2430_NUM_CONTROLLERS * AS5200_MAX_SLOTS];

/* Starting offset (in bytes) in the cd2430_ttyptr array for each slot */
int cd2430_controller_offset[AS5200_MAX_SLOTS];

/* Bit masks for selecting slot interrupts */
uint cd2430_tx_mask;
uint cd2430_rx_mask;
uint cd2430_mod_mask;

/* total number of oob lines */
static int cd2430_total_oob_lines;

/*
 * tty_as5200_pre_init
 *  This routine is called at analyze interface phase and only will be
 *  executed once.
 */
static boolean 
tty_as5200_pre_init(void)
{
    int i;

    /* set clock speed */
    cd2430clock = 30*1000000;

    for (i = 0; i < AS5200_MAX_SLOTS; i++) {
	cd2430_controller_offset[i] = 0;
    }
    
    cd2430_total_oob_lines = 0;

    cd2430_tx_mask = 0;
    cd2430_rx_mask = 0;
    cd2430_mod_mask = 0;

    /*
     * Initialize async fastswitching
     */
    reg_add_init_async_fs(init_async_fs, "init_async_fs");
    /* our base is first free line */
    modem_pool->ttybase = cd2430Base = freeLineBase;
    cd2430_vector_init();

    /* Disable Modem Board level 1,2,3 interrupts until all boards ready */
    *((ushort *)ADRSPC_BRASIL_MB_CTRL1_REG) &= ~BRASIL_ASYNC_INTR_ENABLE;

    /*
     * Create interrupt stacks.  Modem stack is usually created by the
     * sync driver, whose level we share.
     */
    createlevel(CD2430T_DISABLE, NULL, "CL-CD2430 transmit interrupts");
    createlevel(CD2430R_DISABLE, NULL, "CL-CD2430 receive interrupts");
    if (!level_exists(CD2430M_DISABLE)) {
        createlevel(CD2430M_DISABLE, NULL, "CL-CD2430 modem interrupts");
    }
    /*
     * Create Buffer pool.
     */
    cd2430_buffpool_group = pool_create_group();
    cd2430_buffpool = pak_pool_create(cd2430_buffpool_name,
                                          cd2430_buffpool_group,
                                          CD2430_PAKSIZE, POOL_SANITY,
                                          NULL);
    if (!cd2430_buffpool) {
        errmsg(&msgsym(NOBUF, TTYDRIVER), cd2430_perm_buffs);
        crashdump(0);
        /* Be paranoid */
        return FALSE;
    }
    cd2430_buffpool->fallback = pak_pool_find_by_size(CD2430_PAKSIZE);
    cd2430_perm_buffs = 0;
     
    /*
     * Fill in the interrupt service vectors
     */
    for (i = 0; i <= MODEM_IRQ_VECTOR_OFFSET; i++) {
	exceptionHandler(CD2430Vector + i, as5200_cd2430_vectors[i]);
    }

    /* Initial parser commands */
    tty_cd2430_parser_init();

    /* Set up the Motherboard IRQ Vector */
    *((ushort *)ADRSPC_BRASIL_MB_IRQ_VECTOR) = CD2430Vector;

    return TRUE;
}

/*
 * tty_cd2430_subsystem_init
 *	The routine is called at subsystem initialize phase.
 */
void
tty_cd2430_subsystem_init (void)
{
    nasync = 0;
}

/*
 * tty_as5200_cd2430_init
 *
 * Inialize the Cirrus CD2430 on Brasil platform at analyze interface phase.
 * Try to do most of the initializations here, anything that requires
 * knowledge of the total number of lines is done during the platform line
 * initializations later.
 */
void
tty_as5200_cd2430_init (modem_slot_t *slotinfo, int slot)
{
    tt_soc *tty;
    int total_number_ports, total_number_controllers;
    int total_physical_ports, total_modules, ports_per_module;
    int actual_modems_in_module;
    int ctrl, line, module_index, ctrls_per_module;
    cd2430_quadart *csr;
    modem_cookie_t  *cookie;
    modem_module_cookie_t *module;
    int oob_lines;
    boolean downloadable;

    if (!( modem_pool->modem_control_flags & MODEM_CTRL_PRE_INIT_DONE )) {
        modem_pool->modem_control_flags |= MODEM_CTRL_PRE_INIT_DONE;
	if ( tty_as5200_pre_init() == FALSE) {
	   return;
	}
    }

    cookie = slotinfo->modem_cookie;
    total_number_ports = cookie->carrier.number_of_ports;
    total_modules = cookie->carrier.max_modules;
    total_number_controllers = total_number_ports / CD2430_CHIPCHAN;
    ctrls_per_module = total_number_controllers / total_modules;
    /* round the number of ports down, if necessary, so that */
    /* only fully utilized controllers are present */
    total_number_ports = total_number_controllers * CD2430_CHIPCHAN;
    slotinfo->total_tty_ports = total_number_ports;
    ports_per_module = total_number_ports / total_modules;
    oob_lines = cookie->carrier.max_oob_ports;

    /* Get the actual physical ports present */
    total_physical_ports = 0;
    for (module_index = 0; module_index < total_modules; module_index++) {
    	module = &(slotinfo->modem_cookie->module[module_index]);
	if ( module->number_of_modems != 0xFF) {
	    /* Modem Module is present */
            if (module->number_of_modems > ports_per_module)
                actual_modems_in_module = ports_per_module;
            else
                actual_modems_in_module = module->number_of_modems;
	    total_physical_ports += actual_modems_in_module;
	} 
    }

    nasync += total_physical_ports;
    nttylines += total_physical_ports;
    if (oob_lines > CD2430_CHIPCHAN) {
	oob_lines = CD2430_CHIPCHAN;
    }
    if (oob_lines > OOB_PORTS_PER_SLOT) {
	oob_lines = OOB_PORTS_PER_SLOT;
    }
    if (slotinfo->modem_reg->mdb_rev & BRASIL_SRAM_BASED_CD24XX) {
	downloadable = TRUE;
    }
    else {
	downloadable = FALSE;
    }
 
    if (oob_lines) {
	total_number_controllers++;
    }

    /*
     * Adjust buffer pool
     */
    cd2430_perm_buffs += (total_number_ports + oob_lines) *
					(CD2430_DEF_RXBUFF+CD2430_DEF_TXBUFF);
    pool_adjust(cd2430_buffpool, 0, cd2430_perm_buffs, cd2430_perm_buffs, TRUE);

    /*
     * Set starting offset (in bytes) for this slot.
     * This is the (cd2430_controllers)'th controller.
     * Used as pointer into cd2430_ttyptr table.
     */
    cd2430_controller_offset[slot] = cd2430_controllers * 4;

    cd2430_tx_mask |= (1 << (BRASIL_TXINT_SLOT0_BIT + slot));
    cd2430_rx_mask |= (1 << (BRASIL_RXEXCEP_SLOT0_BIT + slot));
    cd2430_mod_mask |= (1 << (BRASIL_MODINT_SLOT0_BIT + slot));
    
    /*
     * Initialize the controllers' global context
     */
    for (ctrl = 0; ctrl < total_number_controllers; ctrl++) {

	/* Get the CSR pointer */
	csr = SLOT_CD2430ADDRESS(slot, ctrl);

	/*
	 * Download f/w if needed.
	 */
	if (downloadable) {
	    cd2430_download_controller_firmware(csr);
	}

	/*
	 * reset chip, and wait for reset
	 * to complete by checking for a non-zero firmware revision
	 */
	while (csr->gfrcr == 0) {
	    ;
	}
	sendccr(csr, CCR_RESET);
	while (csr->gfrcr == 0) {
	    ;
	}

	/*
	 * Read the version ID, and allow PPP mode if it's 2430-vintage
	 * firmware.  Note, this code assumes that all chips on a box
	 * are the same.
	 */
	can_ppp = (csr->gfrcr >= 0x1f);

	/*
	 * Set priority levels
	 */
	csr->mpilr = CD2430PILR(CD2430M_DISABLE);
	csr->tpilr = CD2430PILR(CD2430T_DISABLE);
	csr->rpilr = CD2430PILR(CD2430R_DISABLE);
        

	/* Modem Board OOB tty lines are on the last controller */
	if (oob_lines && (ctrl == (total_number_controllers - 1))) {  
	    /* The tty lines for modem control will not
		occupy the MODEMS table entry */
    	    for ( line = 0; line < oob_lines ; line ++ ) {
		tty = malloc_named(sizeof(tt_soc), "TTY data");
		if (tty) {
 		    slotinfo->mgmt_oob_tty[line] = tty;
		    /* assume freeLineBase+line, and reassign later. */
		    tty_init_data(tty, freeLineBase + line, READY, TTY_LINE,
		   				 &cd2430_vector);
		    /* temporarily store csr and ctrl number */
		    tty->uartdata = csr;
		    tty->modem_type = ctrl;
		}
	    }

	    /* setup ttyptr table to point to first
	     * entry in modem table for this controller 
	     */
	    cd2430_ttyptr[cd2430_controllers] = &slotinfo->mgmt_oob_tty[0];
	    cd2430_total_oob_lines = OOB_PORTS_PER_SLOT;
	}
        else {
            module_index = ctrl / ctrls_per_module;
            module = &(slotinfo->modem_cookie->module[module_index]);
            actual_modems_in_module = module->number_of_modems;
            if ((actual_modems_in_module != 0xFF) && 
                (actual_modems_in_module > ports_per_module))
                actual_modems_in_module = ports_per_module;
            
            /* Set up tty lines only if the module is present */
            if ((actual_modems_in_module != 0xFF) && 
                 (actual_modems_in_module > 
                  ((ctrl % ctrls_per_module) * CD2430_CHIPCHAN))) {
	        /* setup ttyptr table to point to first
	           entry in modem table for this controller */
	        cd2430_ttyptr[cd2430_controllers] = &MODEMS[freeLineBase];
	        for (line = 0; line < CD2430_CHIPCHAN; line++) {
                    if ((((ctrl % ctrls_per_module) * CD2430_CHIPCHAN) + line) <
                          actual_modems_in_module) {
	                tty = (tt_soc *) tty_init( freeLineBase, READY, 
                                             TTY_LINE, &cd2430_vector);
                        if (tty) {
		            /* temporarily store csr and ctrl number */
		            tty->uartdata = csr;
		            tty->modem_type = ctrl;
		        }
    
		        /* bump freeLineBase */
		        freeLineBase++;
                    }
	        }
            } 
        }

        /* increment current controller number */
        cd2430_controllers++;
    }
}

/*
 * tty_as5200_cd2430_line_init
 *
 *  Finial initializations and enable the tty lines for use.
 */
boolean
tty_as5200_cd2430_line_init (void)
{
    int dbsize, slot, ttynum, i;
    modem_slot_t *slotinfo;
    tt_soc *tty;

    if (nasync == 0)
		return FALSE;
   /*
    * Initialize private info fields for all 2430 lines
    */
    dbsize = (nasync + cd2430_total_oob_lines) * sizeof(cd2430_priv);
    cd2430info_base = malloc_named(dbsize,"cd2430 status block");
    if (!cd2430info_base) {
        errmsg(&msgsym(NOMEM, TTYDRIVER), dbsize);
        crashdump(0);
        /* Not sure if this aborts */
        return FALSE;
    }
    bzero(cd2430info_base, dbsize);

    for (ttynum = cd2430Base, i = 0; i < nasync; i++, ttynum++) {
	tty = MODEMS[ttynum];
	cd2430_channel_setup(tty, (cd2430_quadart *)tty->uartdata,
						tty->modem_type << 2);
    }

    /*
     * Go back and assign the oob ttys' ttynum
     * starting from the last regular ttynum.
     */
    for (slot = 0; slot < AS5200_MAX_SLOTS; slot++) {
	slotinfo = modem_pool->modem_slot[slot];
	if (slotinfo) {
	    for (i = 0; i < OOB_PORTS_PER_SLOT; i++) {
		if (slotinfo->mgmt_oob_tty[i]) {
		    tty = slotinfo->mgmt_oob_tty[i];
		    tty->ttynum = ttynum + i;
		    cd2430_channel_setup(tty, (cd2430_quadart *)tty->uartdata,
						tty->modem_type << 2);
		}
		else
		    break;
	    }
	}
    }

    if (cd2430_setup_break_packet() == FALSE)
        return FALSE;

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
    reg_add_tty_show(cd2430_print_modemcontrol, "print_modemcontrol");
    reg_add_tty_xon(tty_cd2430_xon, "tty_xon");
    reg_add_async_show_controllers(cd2430_show_contr, "cd2430");
    reg_add_setup_global2(async_setup_global, "async_setup_global");

    /* Enable Modem Board level 1,2,3 interrupts */
    *((ushort *)ADRSPC_BRASIL_MB_CTRL1_REG) |= BRASIL_ASYNC_INTR_ENABLE;
    return TRUE;
}

/*
 * channel_to_tty -
 * convert a phyical cd2430 channel number to a pointer of tty 
 */
tt_soc *
channel_to_tty (int cntrl, int chan)
{
    tt_soc **ttyptr;

    /* Note: Pointer will be NULL for un-allocated oob tty's */
    /* get the controller base tty */
    ttyptr = cd2430_ttyptr[cntrl];
    /* index to the particular channel */
    ttyptr += chan;
    return *ttyptr;
}


/****************************************************************************
 *
 *  The common code for the CD2430, "tty_cd2430.c", is being included
 *  here as a temporarily measure to isolate the Brasil async driver
 *  from the incoming CD2430 sync/async code to the 11.1 mainline.  The
 *  Brasil async driver will be revamped latter when the code separation
 *  effort which is currently underway for the CD2430 sync/async driver
 *  for mulitiple platform, is committed.  At that time it will conform
 *  separation architecture defined.
 *  
 ****************************************************************************/

/*
 * Local storage
 */
vectortype cd2430_vector;


int cd2430_istate;		/* Non-zero if in interrupt state */
uint cd2430clock;		/* Clock applied to chips */
int cd2430Base;                  /* Starting line # for CD2430's */
cd2430_priv *cd2430info_base;   /* Base of our priv info array */
int cd2430_controllers;         /* # 2430 chips in system */
boolean can_ppp;                /* Supports HW async HDLC */
int nasync;                     /* # 2430 ports on system */
pooltype *cd2430_buffpool; 	 /* cd2430 permanent buffers pool */

static paktype *break_pak;	/* Data encoding to ask for async BREAK */


/*
 * This is used to identify the packet memory which we have allocated
 * for dedicated CD2430 use.
 */
#define CD2430_POOL (cd2430_buffpool)


/*
 * Divisor of clock/2048 which drives timer register: rtpr/gt1/gt2/ttr
 */
#define CD2430_PRESCALE (10)

/*
 * "Basic" interrupt sources we always want to monitor.  This is receive
 * interrupts as well as modem line changes.
 */
#define IER_BASIC (IER_RET | IER_RXD | IER_MDM)

/*
 * setchan()
 *      Set 2430's CAR register based on TTY offset
 *
 * This function assumes CD2430_CHIPCHAN is a power of two--which should
 * always be true.
 */
static void
setchan (tt_soc *tty, cd2430_quadart *csr)
{
    uchar chan; 
 
    /*
     * Set channel. The car register can not be set in the interrupt context.
     */
    if (cd2430_istate == 0) {
       chan = (((tty)->ttynum - cd2430Base) & (CD2430_CHIPCHAN-1));
       csr->car = chan;
    }
}

/*
 * sendccr()
 *	Send a command to the CD24xx ccr register
 */
void
sendccr (cd2430_quadart *csr, uchar cmd)
{
    while (csr->ccr)
	;
    csr->ccr = cmd;
    while (csr->ccr)
	;
}

/*
 * cd2430_count_nested_txint()
 * This function will count nested transmit interrupts, if any.
 */
static inline int
cd2430_count_nested_txint (cd2430_quadart *csr)
{
    int count = 0;

    if((csr->stk & STK_MLVL_MASK) == STK_TX_MLVL) {
	count++;
	if((csr->stk & STK_TLVL_MASK) == STK_TX_TLVL)
	    count++;
    }
    return(count);
}

/*
 * cd2430_count_nested_rxint()
 * This function will count nested receive interrupts, if any.
 */
static inline int
cd2430_count_nested_rxint (cd2430_quadart *csr)
{
    int count = 0;

    if((csr->stk & STK_MLVL_MASK) == STK_RX_MLVL) {
	count++;
	if((csr->stk & STK_TLVL_MASK) == STK_RX_TLVL)
	    count++;
    }
    return(count);
}

/*
 * cd2430_count_nested_mdmint()
 * This function will count nested modem interrupts, if any.
 */
static inline int
cd2430_count_nested_mdmint (cd2430_quadart *csr)
{
    int count = 0;

    if((csr->stk & STK_MLVL_MASK) == STK_MODEM_MLVL) {
	count++;
	if((csr->stk & STK_TLVL_MASK) == STK_MODEM_TLVL)
	    count++;
    }
    return(count);
}

/*
 * cd2430_clear_nested_txint()
 *      This function will attempt to clear a potentially nested transmit 
 * interrupt. Writing to the TEOIR should result in the stack register
 * indicating that the current interrupt, if any, is something other
 * than a transmit interrupt. If we find that this is not the case, we
 * will write as many times as necessary to make that happen. This is
 * to guard against a known Cirrus bug where the chip indicates two
 * nested interrupts of the same kind - a state that is illegal for
 * the controller to be in.
 */
static inline void
cd2430_clear_nested_txint (cd2430_quadart *csr, ulong val, int count)
{
    while (count > 0) {
        csr->teoir = val;
	count--;
    }
}
 
/*
 * cd2430_clear_nested_rxint()
 *      This function will attempt to clear a potentially nested receive
 * interrupt. Writing to the REOIR should result in the stack register
 * indicating that the current interrupt, if any, is something other 
 * than a receive interrupt. If we find that this is not the case, we 
 * will write as many times as necessary to make that happen. This is 
 * to guard against a known Cirrus bug where the chip indicates two
 * nested interrupts of the same kind - a state that is illegal for
 * the controller to be in.
 */
static inline void
cd2430_clear_nested_rxint (cd2430_quadart *csr, ulong val, int count)
{
    while (count > 0) {
        csr->reoir = val;
	count--;
    }
}
 
/*
 * cd2430_clear_nested_modint()
 *      This function will attempt to clear a potentially nested modem
 * interrupt. Writing to the MEOIR should result in the stack register
 * indicating that the current interrupt, if any, is something other 
 * than a modem interrupt. If we find that this is not the case, we 
 * will write as many times as necessary to make that happen. This is 
 * to guard against a known Cirrus bug where the chip indicates two
 * nested interrupts of the same kind - a state that is illegal for
 * the controller to be in.
 */
static inline void
cd2430_clear_nested_modint (cd2430_quadart *csr, ulong val, int count)
{
    while (count > 0) {
        csr->meoir = val;
	count--;
    }
}
 

/*
 * cd2430_rxgood()
 *	Receive good data interrupt service
 *
 * Should never be entered; this is a DMA driver.  Packet completions and
 * buffer completions both arrive through the RX exception vector.  For
 * purposes of tracking stray interrupts, we tally them under "linenoise".
 */
void
cd2430_rxgood (tt_soc **ttybase)
{
    tt_soc *tty;
    cd2430_quadart *csr;
    cd2430_priv *cd2430chan_info;         /* Local status flags */
    int nested_rx_int = 0;

    GETCSR(ttybase, csr, tty);
    cd2430chan_info = &cd2430info_base[tty->ttynum - cd2430Base];
    tty->linenoise += 1;
    /*
     * See if there are nested receive interrupts.
     */
    nested_rx_int = cd2430_count_nested_rxint(csr);
    cd2430chan_info->nested_rx_interrupts += nested_rx_int;

    /*
     * Since we didn't transfer data, we must specify this bit in the
     * interrupt completion.
     */
    csr->reoir = REOIR_NOTRANSF;
    /*
     * Clear potentially nested receive interrupt.
     */
    if(nested_rx_int > 0) {
	cd2430_clear_nested_rxint(csr, REOIR_NOTRANSF, nested_rx_int);
    }
}

/*
 * inpak_done()
 *	A packet is finished, free to appropriate pool
 */
inline static void
inpak_done (tt_soc *tty, paktype *pak)
{
    if (pak->pool == CD2430_POOL) {
	enqueue(&tty->rxpakq, pak);
    } else {

	int *buffs_allocated = 
	    &(cd2430info_base[tty->ttynum-cd2430Base].rxbuffs_outstanding);

	datagram_done(pak);
	/*
	 * Do un-accounting of this buffer against this tty
	 *  We will decrement if this isn't HW PPP mode
	 */
	if ((!(tty->statbits & PPPMODE)) && (*buffs_allocated)) {
	    *buffs_allocated -= 1;
	}
    }
}

/*
 * tty_rxptrs()
 *	Fill in tty input pointers to use the given packet's data area
 *
 * This routine assumes that (1) we use the whole packet data area, and
 * (2) that we have placed the received size in datagramsize.
 */
inline static void
tty_rxptrs (tt_soc *tty, paktype *pak)
{
    tty->inheadpak = pak;
    tty->inhead = tty->intail = tty->instart = pak->datagramstart;
    tty->inend = tty->instart + pak->datagramsize;
    tty->incount = pak->datagramsize;
}

/*
 * flow_control()
 *	Apply flow control to named line
 *
 * HW flow control is done by the chip, so only SW is honored here
 */
static inline void
flow_control (tt_soc *tty, cd2430_quadart *csr)
{
    /*
     * Software flow control (special char 2 is XOFF)
     */
    if (tty->capabilities & SFLOWOUT) {
	if ((csr->csr & CSR_RXFLOFF) == 0) {
	    csr->stcr = STCR_SNDSPEC|STCR_SPC2;
	    tty->statbits |= SENTXOFF;
	}
    }
}

/*
 * tty_queue_receive_pak
 *      Queue receive pak either to the active input data (inhead/tail) 
 *      or, if that's still pointing to unconsumed data, to the rxdataq
 */
inline static void
tty_queue_receive_pak (tt_soc *tty, paktype *pak)
{
    if (tty->inheadpak == NULL) {
	tty_rxptrs(tty, pak);
    } else {
	enqueue(&tty->rxdataq, pak);
    }
}

/*
 * arm_rx_dma()
 *	Arm receive DMA to CD2430
 *
 * Reads the DMA status register to find out which buffer slot (A or B) is
 * to be used next, and then programs the buffer into that slot and enables
 * DMA (i.e., sets ownership to the 24XX) for it.
 */
inline static void
arm_rx_dma (cd2430_quadart *csr, int chan, ulong addr, uint count)
{
    if (chan) {
	csr->brbadrl = CD2430LO(addr);
	csr->brbadru = CD2430HI(addr);
	csr->brbcnt = count;
	csr->brbsts = RBSTS_24OWN;
    } else {
	csr->arbadrl = CD2430LO(addr);
	csr->arbadru = CD2430HI(addr);
	csr->arbcnt = count;
	csr->arbsts = RBSTS_24OWN;
    }
}

/*
 * alloc_rx_buffers()
 *	Allocate A/B if idle & buffers are available
 *
 * This routine is complex as it has to handle the case of buffers not
 * being available, as well as the case of buffers having been unavailable
 * in the past.  For the latter, it may find a need to arm both A and B
 * before returning.
 */
static void
alloc_rx_buffers (tt_soc *tty, cd2430_quadart *csr, int idx)
{
    paktype *newpak;
    pppContext *ctx = tty->appp_context;
    uint size;

    /*
     * Start with whichever buffer the 2400 wants next, and walk forward
     * (that is, to the other one) to fill both if needed.
     */
    for ( ; ; idx ^= 1) {
	/*
	 * Consider status of next buffer.  If it's still owned by the
	 * 2400, or has not been processed, return (we've filled as far
	 * as possible).
	 */
	if (tty->inpak[idx]) {
	    break;
	}

	/*
	 * Try to allocate a packet
	 */
	if (tty->statbits & PPPMODE) {
	    /*
	     * Ask async PPP to grab us our next receive packet.  Take
	     * it from PPP and consume it for DMA instead.
             * The size (to be passed on to firmware) is prepared:
             *  - to include the FCS size since firmware takes 'size'                       *    to also include the FCS size.
             *  - to include 2 bytes (even-aligned) to offset the
             *    off-by-one known bug in firmware. Obviously this will
             *    be taken off when the firmware is (ever) corrected.
             *  Note that the size of the pak itself, although it does
             *  not have these additional bytes tacked on, should not
             *  result in DMA access violation since the pak itself has
             *  'slush' always built-in to the size to be allocated
             *  (TRAILBYTES).
	     */
	    (void)appp_new_inpak(ctx);
	    newpak = ctx->inpak;
	    if (newpak) {
		ctx->inpak = NULL;
		newpak->length = ctx->imax;
                size = ctx->imax + PPP_FCS_SIZE + 2;
	     } else {
		 size = 0;
	     }
	} else {
	    /*
	     * Try our private pool, then the public one
	     */
	    newpak = dequeue(&tty->rxpakq);
	    if (NULL == newpak) {

		cd2430_priv *cd2430chan_info =
		    &(cd2430info_base[tty->ttynum - cd2430Base]);

		if (cd2430chan_info->rxbuffs_outstanding <
		    CD2430_MAX_SYS_BUFFS) {
		    newpak = getbuffer(CD2430_PAKSIZE);
		    if (newpak) {
			newpak->length = CD2430_PAKSIZE;
			cd2430chan_info->rxbuffs_outstanding += 1;
		    }
		} else {
		    /* record buffer limitation */
		    cd2430chan_info->rxbuffs_disallowed++;
		}
	    }
	    size = CD2430_PAKSIZE;
	}

	/*
	 * Found one.  Set it up for DMA, and record its identity so we
	 * can get the pak pointer back later.
	 */
	if (newpak != NULL) {
	    tty->inpak[idx] = newpak;
	    arm_rx_dma(csr, idx, VTOP(newpak->datagramstart), size);
	} else {
	    /*
	     * Apply flow control when our ability to arm DMAs is
	     * hindered.
	     */
	    if (tty->rxpakq.qhead == NULL) {
		flow_control(tty, csr);
	    }

	    /*
	     * Log buffer shortage
	     */
	    tty->overflow += 1;

	    /*
	     * Log buffer miss if associated with IDB
	     */
	    if (ctx && ctx->idb) {
		ctx->idb->counters.output_nobuffers += 1;

		if (tty->statbits & PPPMODE) {
		    idb_mark_throttled(ctx->idb);
		}
	    }

	    /*
	     * Get out of here
	     */
	    break;
	}
    }
}

/*
 * cd2430_pppmode_unthrottle
 *	enable() routine to re-arm the CD24xx during PPP mode when
 *	I/O buffers are available.
 */
void
cd2430_pppmode_unthrottle (hwidbtype *idb)
{
    tt_soc *tty;
    cd2430_quadart *csr;

    idb_mark_unthrottled(idb);
    tty = idb->idb_tty;
    if (tty->inpak[0] && tty->inpak[1]) {
	/*
	 * Must have picked up the buffers already
	 * (maybe via the interrupt on the other DMA channel).
	 */
	return;
    }
    csr = (cd2430_quadart *)tty->uartdata;
    setchan(tty, csr);
    alloc_rx_buffers(tty, csr, (csr->dmabsts & DMABSTS_NRBUF) ? 1 : 0);
}

/*
 * cd2430_rxexcep()
 *	Handle receive data exceptions
 *
 * This routine handles exceptional receive data conditions.  For this driver,
 * this is the main code path for DMA read completions.  Completions will
 * be caused by either the inter-character timer, or by an entire buffer
 * having filled.
 */
void
cd2430_rxexcep (tt_soc **ttybase)
{
    cd2430_quadart *csr;
    tt_soc *tty;
    ushort curbuf, status, bufstatus;
    ushort count = 0;
    uchar reoir;
    ulong statbits;
    pppContext *ctx;
    boolean received_break = FALSE;
    cd2430_priv *cd2430chan_info;         /* Local status flags */
    int nested_rx_int = 0;

    GETCSR(ttybase, csr, tty);
    ctx = tty->appp_context;
    cd2430chan_info = &cd2430info_base[tty->ttynum - cd2430Base];

    /*
     * Handle data first, then exceptions.  RISR tells us which buffer
     * is completing now.
     */
    statbits = tty->statbits;
    status = csr->risr;
    curbuf = (status & RISR_BUFB) ? 1 : 0;
    bufstatus = curbuf ? csr->brbsts : csr->arbsts;
    reoir = 0;

    /*
     * Restore real rtpr value if we overrode it to control DMA
     * latency
     */
    if (tty->short_rtpr) {
	csr->rtpr = tty->cache_rtpr;
	tty->short_rtpr = FALSE;
    }

    /*
     * check for TO first. All other status is invalid if TO set
     */

    /*
     * Force the buffer to be treated as a completion on receive timeout.
     * We only do this for timeouts where there is not already incoming
     * data in the input queue.  Or for rxservice--there is no queue for
     * us to examine.
     */


    if (status & RISR_TO) {
	if (cd2430chan_info->status & SEEINGBREAK) {
	    /*
	     * End-of-Break Condition is a TO right after ISR_BRK
	     * On a 2430 there is considerable delay b/w the
	     * Break-seen and End-of-break-seen. During this time,
	     * some CCR commands cause loss of data.
	     */
	    received_break = TRUE;
	    cd2430chan_info->status &= ~SEEINGBREAK;
	    /*
	     * Do not waste another buffer, if previous break
	     *  yet unnoticed. Also check to see if we do actually
	     *  have a buffer.
	     */
	    if ((tty->inpak[curbuf]) &&
		!(statbits & (LASTWASBREAK | ESCAPED))) {
		reoir = REOIR_TERM;
		bufstatus |= RBSTS_EOB;
	    }
	} else if (!(bufstatus & RBSTS_EOB)) {
	    if (tty->rxservice ||
		((tty->incount == 0) && (tty->rxdataq.qhead == NULL))) {
		reoir = REOIR_TERM;
		bufstatus |= RBSTS_EOB;
	    } else {
		/*
		 * Otherwise start the inter-character timer running for an
		 * interval twice the normal.  This more or less simulates
		 * retrying the inter-character idle timeout in the future.
		 */
		csr->gt1 = csr->rtpr * 2;
	    }
	}
    } else if (status & RISR_BRK) {
	cd2430chan_info->status |= SEEINGBREAK;
    }

    /*
     * If the buffer's done, feed in any bytes it has
     */
    if (bufstatus & RBSTS_EOB) {
	paktype *pak;

	/*
	 * Take completed packet off tt_soc.  Mark slot open.
	 */
	pak = tty->inpak[curbuf];
	tty->inpak[curbuf] = NULL;

	/*
	 * If bad packet, discard
	 */
	if ((statbits & PPPMODE) && (status & (RISR_CRC | RISR_RXABT))) {
	    /*
	     * Count bad frames, log as needed
	     */
	    if (async_framing_debug) {
		buginf("\nAPPP%t: CRC error", tty->ttynum);
	    }
	    if (ctx->idb) {
		ctx->idb->counters.input_crc++;
		slip_traffic.crcerr++;
	    }

	} else {
	    /*
	     * Figure out number of bytes in packet.  If the buffer's not
	     * done we have to use the DMA address, otherwise use the
	     * finishing count.
	     */
	    if (status & RISR_EOB) {
		count = (curbuf ? csr->brbcnt : csr->arbcnt);
		if (statbits & PPPMODE) {
		    /*
		     * If we got the whole buffer, but not end-of-frame,
		     * tally it as a giant.
		     */
		    if ((status & RISR_EOF) == 0) {
			if (!ctx->ingiant) {
			    if (async_framing_debug) {
				buginf("\nAPPP%t: Input packet too large",
				    tty->ttynum);
			    }
			    if (ctx->idb) {
				ctx->idb->counters.giants += 1;
			    }
			    appp_rcv_error(tty);
			    ctx->ingiant = TRUE;
			}
			count = 0;

		    /*
		     * Otherwise, we're in the middle of a giant, and have
		     * now seen the end of the frame.  Discard the last
		     * part and prepare for the next frame.
		     */
		    } else if (ctx->ingiant) {
			count = 0;
			ctx->ingiant = FALSE;

		    /*
		     * Otherwise, it's good data; trim the CRC off the
		     * end and get ready to use it.
		     */
		    } else {
			count -= 2;
		    }
		}
	    } else {
		count = (pak) ? (((csr->rcbadru << 16) | csr->rcbadrl) -
				 VTOP(pak->datagramstart)) : 0;
	    }
	}

	/*
	 * Allocate new buffer(s) now that we've used the old
	 * [ab]rbcnt field.
	 */
	alloc_rx_buffers(tty, csr, curbuf);

	/*
	 * See if there are nested receive interrupts.
	 */
	nested_rx_int = cd2430_count_nested_rxint(csr);
	cd2430chan_info->nested_rx_interrupts += nested_rx_int;

	/*
	 * Tell the 2402 that it can go on.  From here, we don't talk to it,
	 * and in receive exceptions DMA can be on hold until we complete the
	 * interrupt.
	 */
	csr->reoir = reoir;
	/*
	 * Clear potentially nested receive interrupt.
	 */
	if(nested_rx_int > 0) {
	    cd2430_clear_nested_rxint(csr, reoir, nested_rx_int);
	}
	/*
	 * If there's data, consume it.
	 */
	if ((!pak) || (count > pak->length)) {
	    /* bogus information back from the chip.  Ignore this packet */
	    count = 0;
	} else if (count > 0) {
	    pak->datagramsize = count;
	    if (statbits & PPPMODE) {
		/*
		 * PPP mode.  Fiddle packet pointers to reflect
		 * received encapsulation size.  Pass up.
                 * Since Xremote does not use PPP encaps, don't
                 * parse packet.
		 */
                if (!tty->xrcb)
		   (void)ppp_parse_packet(tty->tty_idb, pak, NULL);
		(*(ctx->iqueue))(pak, tty);
		pak = NULL;

	    } else if (tty->rxservice) {
		(*(tty->rxservice))(tty, pak->datagramstart, count);
	    } else {

		/*
		 * Non-rxservice might need to process escapes, hold
		 * characters, and so forth out of the byte stream.  We
		 * do this, overwriting the buffer in place.
		 */
		if (statbits & PSIENA) {
		    uchar *src, *dest;

		    src = dest = pak->datagramstart;
		    while (count-- > 0) {
			int c;

			/*
			 * Filter out non-data characters by walking
			 * buffer and rewriting data sequentially.
			 */
			c = *src++;
			if (special_chars(tty, c) >= 0) {
			    *dest++ = c;
			}
		    }
		    count = pak->datagramsize =
			dest - (uchar *)pak->datagramstart;
		}

		/*
		 * If there's still data, queue it to the tty 
		 */
		if (count > 0){ 
                   tty_queue_receive_pak(tty, pak);
                   pak = NULL;
		}
	    }
	} else {
            /*
             * Buffer is done but Count = 0
             * If completed receiving a break, stick a break character
	     * to the input stream if it is non-rxservice and non-PPPMODE
             */
            if (received_break && (!tty->rxservice) &&
		!(statbits & PPPMODE)) {
		special_chars(tty, BREAK_CHAR);
                *pak->datagramstart = 0x0;
                pak->datagramsize = 1;
                tty_queue_receive_pak(tty, pak);
                pak = NULL;
	    }
        }

	/*
	 * Release the packet
	 */
	if (pak) {
	    inpak_done(tty, pak);
	}
    } else {
	/*
	 * See if there are nested receive interrupts.
	 */
	nested_rx_int = cd2430_count_nested_rxint(csr);
	cd2430chan_info->nested_rx_interrupts += nested_rx_int;

	/*
	 * Complete the interrupt; no data motion
	 */
	csr->reoir = reoir;
	/*
	 * Clear potentially nested receive interrupt.
	 */
	if(nested_rx_int > 0) {
	    cd2430_clear_nested_rxint(csr, reoir, nested_rx_int);
	}
    }

    /*
     * Tally overruns
     */
    if (status & RISR_OE) {
	tty->overrun += 1;
    }
    if (status & (RISR_FE | RISR_PE)) {
	tty->linenoise += 1;
    }
}

/*
 * arm_tx_dma()
 *	Arm transmit DMA to CD2430
 *
 * Reads the DMA status register to find out which buffer slot (A or B) is
 * to be used next, and then programs the buffer into that slot and enables
 * DMA (i.e., sets ownership to the 24XX) for it.
 */
inline static void
arm_tx_dma (cd2430_quadart *csr, ulong addr, uint count, uint options)
{
    if (csr->dmabsts & DMABSTS_NTBUF) {
	csr->btbadrl = CD2430LO(addr);
	csr->btbadru = CD2430HI(addr);
	csr->btbcnt = count;
	csr->btbsts = TBSTS_INTR | TBSTS_24OWN | TBSTS_EOF | options;
    } else {
	csr->atbadrl = CD2430LO(addr);
	csr->atbadru = CD2430HI(addr);
	csr->atbcnt = count;
	csr->atbsts = TBSTS_INTR | TBSTS_24OWN | TBSTS_EOF | options;
    }
}

/*
 * tty_txptrs()
 *	Fill in tty output pointers to use the given packet's data area
 *
 * Data is assumed to be placed starting at datagramstart, and pak->length
 * is assumed to hold the amount of space available in the buffer for
 * transmit data to be enqueued.
 */
inline static void
tty_txptrs (tt_soc *tty, paktype *pak)
{
    tty->outhead = tty->outtail = tty->outstart = pak->datagramstart-1;
    tty->outend = tty->outstart + pak->length;
    tty->outsize = pak->length;
    tty->outcount = 0;
}

/*
 * outpak_done()
 *	Output packet finished, free to right pool
 */
inline static void
outpak_done (tt_soc *tty, paktype *pak)
{
    if (pak->pool == CD2430_POOL) {
	enqueue(&tty->txpakq, pak);
    } else {
	datagram_done(pak);
	tty->outtail = tty->outstart + tty->outcount; /* reset outtail */
    }
}

/*
 * cd2430_txint()
 *	Handle a transmit completion interrupt
 *
 * TBD: what if it's not done, but it's something exceptional?  Looks like
 * it would have to be something pretty dire, like bus error.
 */
void
cd2430_txint (tt_soc **ttybase)
{
    cd2430_quadart *csr;
    tt_soc *tty;
    int count;
    paktype *pak = NULL, *donepak;
    uint opts = 0;
    int nested_tx_int = 0;
    cd2430_priv *cd2430chan_info;         /* Local status flags */

    /*
     * Get TTY base pointer
     */
    GETCSR(ttybase, csr, tty);
    cd2430chan_info = &cd2430info_base[tty->ttynum - cd2430Base];

    /*
     * If buffer done, set up for next
     */
    if (csr->tisr & (TISR_EOF | TISR_EOB)) {
	/*
	 * Record which buffer completed, flag as not the output
	 * packet any more.
	 */
	donepak = tty->outpak;
	tty->outpak = NULL;
    } else {
	/*
	 * If the buffer's not done, and there IS a buffer being sent,
	 * bail out without trying to arm a new one.
	 */
	if (tty->outpak) {
	    csr->ier &= ~(IER_TXMPTY);
	    /*
	     * See if there are nested transmit interrupts.
	     */
	    nested_tx_int = cd2430_count_nested_txint(csr);
	    cd2430chan_info->nested_tx_interrupts += nested_tx_int;

	    csr->teoir = 0;
	    /*
	     * Clear potentially nested transmit interrupt.
	     */
	    if(nested_tx_int > 0) { 
		cd2430_clear_nested_txint(csr, 0, nested_tx_int);
	    }
	    return;
	}
	donepak = NULL;
    }

    /*
     * Always clear TXMPTY; we are mostly driven off packet completions,
     * except for starting transmission from an idle state.
     */
    if (donepak == NULL) {
	csr->ier &= ~(IER_TXMPTY);
    }

    /*
     * If PPP mode, pull packets from PPP queue directly
     */
    if (tty->statbits & PPPMODE) {
	pppContext *ctx = tty->appp_context;

	/*
	 * Get next queued packet
	 */
	pak = appp_get_oqueue(ctx);

	if (pak) {
	    /*
	     * LCP packets always transmit with the all-1's, most conservative
	     * escape map.
	     */
	    if (pak->flags & PAK_CONTROL) {
		opts |= TBSTS_MAP32;
	    }

	    /*
	     * Hold state as APPP_FRAME_END to keep appp_startoutput from
	     * thinking we need to be restarted.
	     */
	    ctx->outstate = APPP_FRAME_END;

	} else {
	    /*
	     * Indicate that we would need to be restarted next time
	     */
	    ctx->outstate = APPP_IDLE;
	}

    /*
     * If the BREAK handling machinery is active, do this in preference
     * to other work.  Note that breaks can not happen during PPP mode.
     */
    } else if (tty->statbits & (SENDBREAK|SENDINGBREAK)) {
	/*
	 * Break generation works as a state machine.  The complexity is
	 * because the ETC mode and the processing of the embedded commands
	 * operate asynchronous of each other.  Bug in the chip, IMHO, but
	 * we have to live with it.
	 *
	 * States:
	 * SENDBREAK !SENDINGBREAK !break_pak:
	 *	Set ETC, queue the break packet
	 * SENDBREAK  SENDINGBREAK  break_pak:
	 *	Break packet has been sent, need to wait for transmit empty
	 * SENDBREAK  SENDINGBREAK !break_pak:
	 *	Break all sent, clear ETC, life continues
	 */
	if (!(tty->statbits & SENDINGBREAK)) {
	    /*
	     * If we are not yet sending break, put the line into embedded
	     * command mode, and arrange for the next packet to contain
	     * the embedded commands necessary to send a break.
	     */
	    csr->cor2 |= COR2_ETC;
	    tty->statbits |= SENDINGBREAK;
	    pak = break_pak;
	} else {
	    if (donepak == break_pak) {
		/*
		 * Packet data queued, now wait for FIFO to empty
		 * entirely.  We bail out at this point, as we don't
		 * want to let the driver try to queue new data.
		 */
		csr->ier |= IER_TXMPTY;
		/*
		 * See if there are nested transmit interrupts.
		 */
		nested_tx_int = cd2430_count_nested_txint(csr);
		cd2430chan_info->nested_tx_interrupts += nested_tx_int;
		csr->teoir = 0;
		/*
		 * Clear potentially nested transmit interrupt.
		 */
		if(nested_tx_int > 0) {
		    cd2430_clear_nested_txint(csr, 0, nested_tx_int);
		}
		return;
	    } else {
		/*
		 * Done sending the break, so leave embedded command
		 * mode.
		 */
		csr->cor2 &= ~COR2_ETC;
		tty->statbits &= ~(SENDBREAK|SENDINGBREAK);
	    }

	    /*
	     * Don't let the break packet be cleaned up; it's a static
	     * resource.
	     */
	    donepak = NULL;
	}
    }

    /*
     * If we have a break or PPP packets, they take priority.
     * Otherwise see about getting the next packet of data to send.
     */
    if (pak == NULL) {
	if (tty->txservice) {
	    /*
	     * Txservice is handled here by pulling bytes from it and
	     * queueing to the normal TTY output buffer.  With all the
	     * bytes lined up in that buffer, we can start a simple DMA.
	     *
	     * Note that outtail is advanced before storing a byte.
	     */
	    count = (*(tty->txservice))(tty, tty->outtail + 1,
		tty->outend - tty->outtail);
	    tty->outcount += count;
	    tty->outtail += count;
	}

	/*
	 * At this point, outhead+1..outtail describe a range of
	 * memory which should be transmitted.  This memory has
	 * either been filled in with putc()'s elsewhere, or filled
	 * from our txservice right above; we don't care, it's just
	 * data from here.
	 */
	if (tty->outcount > 0) {
	    pak = dequeue(&tty->txpakq);
	    pak->datagramsize = tty->outcount;
	    tty->outstart = NULL;	/* Flag that we've taken it */
	}
    }

    /*
     * If we have data to send, fire up DMA
     */
    if (pak) {
	if (tty->ispy) {
	    (*tty->ispy)(tty, pak->datagramstart, pak->datagramsize);
	}
	arm_tx_dma(csr, VTOP(pak->datagramstart), pak->datagramsize, opts);
	tty->outpak = pak;
    }

    /*
     * Take care of previous packet.  If it's one of our "private reserve",
     * place it back on the queue, otherwise datagram_done() it.
     *
     * We waited until here to free the packet so that it may conveniently
     * become the target of outhead/tail/etc. in the following code.
     */
    if (donepak) {
	outpak_done(tty, donepak);
    }

    /*
     * If we removed the previous packet serving as outhead/tail/etc, set
     * up a new one.
     */
    if (tty->outstart == NULL) {
	pak = (paktype *)tty->txpakq.qhead;
	tty_txptrs(tty, pak);
    }
    /*
     * See if there are nested transmit interrupts.
     */
    nested_tx_int = cd2430_count_nested_txint(csr);
    cd2430chan_info->nested_tx_interrupts += nested_tx_int;
    csr->teoir = 0;
    /*
     * Clear potentially nested transmit interrupt.
     */
    if(nested_tx_int > 0) {
	cd2430_clear_nested_txint(csr, 0, nested_tx_int);
    }
}

/*
 * cd2430_modint()
 *	Handle modem state change interrupt
 *
 * Clear the modem interrupt, but take no action.  We handle modem line
 * changes using busy-polling.
 */
void
cd2430_modint (tt_soc **ttybase)
{
    cd2430_quadart *csr;
    tt_soc *tty;
    uchar misr;
    int nested_mdm_int = 0;
    cd2430_priv *cd2430chan_info;         /* Local status flags */

    /*
     * Get right register set, record which lines changed
     */
    GETCSR(ttybase, csr, tty);
    cd2430chan_info = &cd2430info_base[tty->ttynum - cd2430Base];
    misr = csr->misr;
    if (misr & MISR_DSR) {
	tty->modem_changes.ring = TRUE;
	if (tty->capabilities & NO_FLUSH_ACTIVATE) {
	    tty_portenable(tty);
	    inputpending(tty);
	}
    }
    if (misr & MISR_CTS) {
	tty->modem_changes.cts = TRUE;
    }
    /*
     * See if there are nested modem interrupts.
     */
    nested_mdm_int = cd2430_count_nested_mdmint(csr);
    cd2430chan_info->nested_mdm_interrupts += nested_mdm_int;

    /*
     * All done
     */
    csr->meoir = 0;
    /*
     * Clear potentially nested modem interrupt.
     */
    if(nested_mdm_int > 0) {
	cd2430_clear_nested_modint(csr, 0, nested_mdm_int);
    }
}

/*
 *			Utility routines
 */

/*
 * cd2430_setcor()
 *	Set channel option registers
 *
 * This routine sets the channel option registers to match the values
 * in the tty structure. The parameters managed by this are:
 *
 *	Parity
 *	Databits
 *	Stopbits
 *	Hardware flow control
 *	Software flow control
 */
static void
cd2430_setcor (tt_soc *tty)
{
    uchar options1 = 0,
	options2 = 0,
	options3 = 0,
	options4 = 0,
	options5 = 0,
	options6 = 0,
	options7 = 0;
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;

    /*
     * Select right UART channel
     */
    setchan(tty, csr);

    /*
     * COR1 options
     */
    switch (tty->tty_parity) {			/* Parity */
    case PARITY_NO:
	options1 |= COR1_PARITY_NONE;
	break;

    case PARITY_ODD:
	options1 |= COR1_PARITY_ODD|COR1_PARITY_NORM;
	break;

    case PARITY_EVEN:
	options1 |= COR1_PARITY_NORM;
	break;

    case PARITY_SPACE:
	options1 |= COR1_PARITY_FORCE;
	break;

    case PARITY_MARK:
	options1 |= COR1_PARITY_FORCE|COR1_PARITY_ODD;
	break;
    }
    options1 |= COR1_PARITY_IGN;

    switch (tty->tty_databits) {		/* Data bits */
    case 5:
	options1 |= COR1_5BIT;
	break;

    case 6:
	options1 |= COR1_6BIT;
	break;

    case 7:
	options1 |= COR1_7BIT;
	break;

    case 8:
	options1 |= COR1_8BIT;
	break;
    }

    /*
     * COR2 options (and COR3, where appropriate)
     */
    if (tty->capabilities & SFLOWIN) {		/* Software flow control */
	options2 |= COR2_TXINBAND;
	if (!(tty->statbits & PPPMODE)) {
	    options3 |= COR3_FCT | COR3_SCDE;
	}
    }
    if (tty->capabilities & HFLOWIN) {		/* Hardware flow control */
	options2 |= COR2_CTS;
    }

    /*
     * COR3 options
     */
    if (!(tty->statbits & PPPMODE)) {
	switch (tty->tty_stopbits) {		/* Stop bits */
	case STOPBITS1:
	    options3 |= COR3_1STOP;
	    break;

	case STOPBITS15:
	    options3 |= COR3_15STOP;
	    break;

	case STOPBITS2:
	    options3 |= COR3_2STOP;
	    break;
	}
    } else {
	options3 |= COR3_TXGEN | COR3_RXCK | COR3_CRCPASS;
    }

    /*
     * COR4 options
     */
    options4 = COR4_RXTHRESH(tty->tty_rxspeed); /* Set the input threshold */
    if (options4 < 2) {
	options4 = 2;
    }
    if (options4 > 12) {
	options4 = 12;
    }

    options4 |= COR4_DSRZD;
    if (!(tty->capabilities & HFLOWIN)) {
	options4 |= COR4_CTSZD;
    }

    /*
     * COR5 options
     */
    if (tty->capabilities & HFLOWOUT) {
	/*
	 * Previously RTS threhold was set to 8, leaving a 4 byte
	 * padding before overrunning.  But for some modems (i.e.
	 * Microcom internal modems), the response to RTS was just
	 * a tad slow, because the RTS signal ended up being polled,
	 * and so the fifo's RTS threshold need to be lowered.
	 */
	options5 |= (CD2430_BUFSIZE - RTS_OVERRUN_BUFSIZE);
	/* make sure the dma threshold is less than RTS threshold */
	if ((options4 & COR4_FIFOMASK) > (options5 - 2))
	    options4 = (options4 & ~COR4_FIFOMASK) | (options5 - 2);
    }
    options5 |= COR5_DSROD;
    if (!(tty->capabilities & HFLOWIN)) {
	options5 |= COR5_CTSOD;
    }

    /*
     * Select the channel
     * disable tx and rx for a while when changing parameters
     * note: don't reset/realloc buffers here
     */
    setchan(tty, csr);
    sendccr(csr, CCR_DISTX | CCR_DISRX);

    /*
     * Set the special characters
     */
    if (!(tty->statbits & PPPMODE)) {
	csr->schr1 = tty->tty_startchar;
	csr->schr2 = tty->tty_stopchar;
    }

    /*
     * Set the channel option registers
     */
    csr->cor1 = options1;	/* Set the option register 1 */
    csr->cor2 = options2;	/* Set the option register 2 */
    csr->cor3 = options3;	/* Set the option register 3 */
    csr->cor4 = options4;	/* Set the option register 4 */
    csr->cor5 = options5;	/* Set the option register 5 */
    csr->cor6 = options6;	/* Set the option register 6 */
    csr->cor7 = options7;	/* Set the option register 7 */

    /*
     * Set prescaler period
     */
    csr->tpr = CD2430_PRESCALE;

    /*
     * Tell 24XX to recalculate its internal tables and
     *      to re-enable the xmitter and receiver
     */
    sendccr(csr, CCR_INITCH | CCR_ENTX | CCR_ENRX);

    /*
     * Always start with "clear to send"
     */
    csr->msvr_dtr = MSVR_DTR;
}

/*
 * cd2430_putpkt()
 *	Output data stored in a packet buffer
 *
 * This routine is used to output data stored in a packet buffer.
 */
static int
cd2430_putpkt (tt_soc *tty, paktype *pak, uchar *datastart, int length)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    leveltype olev;
    
    /*
     * If padding needs to be added, do it as a byte stream, and stuff
     * into outhead/tail/etc.
     */
    if (tty->current.padding_ptr) {
	return(generic_putpkt(tty, pak, datastart, length));
    }

    /*
     * If a packet's going out, tell them to wait.  this is the "quick"
     * check; we'll check again with interrupts masked.
     */
    if (tty->outpak) {
	return(0);
    }

    /*
     * Access this TTY.  If we're not currently allowed to transmit,
     * tell them to wait.
     */
    setchan(tty, csr);
    if (tty->statbits & HOLDING) {
	return(0);			/* Don't output if holding */
    }

    /*
     * If we notice a byte stream of output is pending, wait until
     * it has cleared.  Note tty_dallytimer() will not wait forever.
     */
    if (tty->outcount != 0) {
	tty_dallytimer(tty);
	return(0);
    }

    /*
     * If we're wrapping up, just lie and say it's all taken care of.
     */
    if (tty->statbits & (CARDROP|IDLE)) {
	return(length);
    }

    /*
     * Place a lock on the packet, and mark it as our current output
     * packet.  Start DMA by handing the data to the 24XX.
     */
    olev = raise_interrupt_level(TTY_DISABLE);
    if (tty->outpak == NULL) {
	pak_lock(pak);
	tty->outpak = pak;
	tty->outtail = NULL;	/* Indicate a putpkt in progress */
	if (tty->ispy) {
	    (*tty->ispy)(tty, datastart, length);
	}
	arm_tx_dma(csr, VTOP(datastart), length, 0);

	/*
	 * Enable transmit interrupts so we'll hear about the
	 * completion.  tty_dallytimer() will watch for hung transmits.
	 */
	csr->ier |= (IER_TXD);
    } else {
	/*
	 * Raced with a transmit interrupt, probably a TXMPTY on the
	 * FIFO emptying.
	 */
	length = 0;
    }
    reset_interrupt_level(olev);

    /*
     * If we in fact started the transmit, start a timer.  Return
     * result.
     */
    if (length) {
	tty_dallytimer(tty);
    }
    return(length);
}

/*
 * cd2430_startinput()
 *	Resume pending input
 */
static void
cd2430_startinput (tt_soc *tty)
{
    cd2430_quadart *csr;
    leveltype level;

    csr = (cd2430_quadart *)tty->uartdata;
    setchan(tty, csr);
    level = raise_interrupt_level(TTY_DISABLE);

    /*
     * If we've held off the other side, request him to start now
     * STCR_SC1 is the XON character
     */
    if (csr->csr & CSR_RXFLOFF) {
	csr->stcr = STCR_SNDSPEC|STCR_SPC1;
    }
    csr->ier |= IER_BASIC;		/* Enable basic interrupts */
    tty->statbits &= ~STOPPEDINPUT;	/* Input isn't stopped */
    reset_interrupt_level(level);
}

/*
 * cd2430_startoutput()
 *	Start up pending output
 *
 * This routine can be called either in interrupt state or process state.
 * If we are called from interrupt state, we must not touch the channel
 * access register. We can only be called in interrupt state for an
 * interrupting line.
 */
static boolean
cd2430_startoutput (tt_soc *tty)
{
    cd2430_quadart *csr;
    leveltype olevel;

    /*
     * Don't do anything if our user has stopped us from sending
     */
    if ((tty->statbits & HOLDING) == 0) {

	/*
	 * Point to our particular device's registers.  If in interrupt
	 * state our channel is selected for us, otherwise select it
	 * explicitly.
	 */
	csr = (cd2430_quadart *)tty->uartdata;
	setchan(tty, csr);
	if (!cd2430_istate) {
	    olevel = raise_interrupt_level(TTY_DISABLE);
	} else {
	    olevel = (leveltype)0;		/* keep compiler quiet */
	}

	/*
	 * Allow our channel to interrupt on data completions
	 */
	if (tty->outpak == NULL) {
	    csr->ier |= (IER_TXD | IER_TXMPTY);
	} else {
	    csr->ier |= IER_TXD;
	}

	/*
	 * Restore interrupts if not running in interrupt handler
	 */
	if (!cd2430_istate) {
	    reset_interrupt_level(olevel);
	}
    }
    return(TRUE);		/* Say it worked */
}

/*
 * cd2430_outputxoffed()
 *	Is CD2430 Output Xoffed?
 *
 * Checks for both HW assisted and regular SW flow control
 */
static boolean
cd2430_outputxoffed (tt_soc *tty)
{
    cd2430_quadart *csr;

    csr = (cd2430_quadart *)(tty->uartdata);
    setchan(tty, csr);
    if((tty->statbits & WANTXON) || (csr->csr & CSR_TXFLOFF)) {
	return TRUE;
    }
    return FALSE;
}

/*
 * get_clock()
 *	Given baud rate, calculate divisor and period
 *
 * There can be multiple solutions; we return the closest.  Copan will be
 * showing up at various clock rates, thus we do the calculation instead
 * of using a table.
 *
 * Returns FALSE if no solution found, TRUE otherwise.
 */
static boolean
get_clock (uint baud, uchar *clkp, uchar *pdp)
{
    uint maxdx = 999999, clk;
    uint x, matches = 0;

    /*
     * For each possible divisor
     */
    for (x = 0, clk = cd2430clock/8; x < 5; ++x, clk >>= 2) {
	uint div, mod, tmpbaud, dx;

	/*
	 * Get nearest solution.  If the remainder is more than half way
	 * up the baud, we round up.  Floating point is for wimps.
	 */
	div = clk / baud;
	if ((div > 0xFF) || (div <= 0x01)) {	/* No solution in this range */
	    continue;
	}
	mod = clk % baud;
	if (mod >= (baud >> 1)) {
	    if (div < 0xFF) {
		div += 1;
	    } else {
		continue;
	    }
	}

	/*
	 * Figure out true baud rate, and record our parameters if this
	 * is the closest match yet.
	 */
	tmpbaud = clk / div;
	if (tmpbaud > baud) {
	    dx = tmpbaud - baud;
	} else {
	    dx = baud - tmpbaud;
	}
	if (dx < maxdx) {
	    matches += 1;
	    *clkp = x;
	    *pdp = div - 1;
	    maxdx = dx;
	}
    }
    return(matches != 0);
}

/*
 * calc_rtpr()
 *	Given baud rate, return right value for rtpr
 */
static uint
calc_rtpr (int baud)
{
    uint x;
    const uint target_char_delay = 6;

    x = (target_char_delay *
    	((cd2430clock/2048)/CD2430_PRESCALE)) / (baud/10);
    if (x > 254) {
	x = 254;
    } else if (x < 2) {
	x = 2;
    }
    return(x);
}

/*
 * reset_rx_buffers()
 *	Reset the receive side of TTY buffering
 */
static void
reset_rx_buffers (tt_soc *tty)
{
    /*
     * Free up any armed receive packets.  Note that both PPP and async
     * stream mode organize packets the same way, so this cleanup works
     * for transitions into and out of PPP mode.
     */
    if (tty->inpak[0]) {
	inpak_done(tty, tty->inpak[0]);
	tty->inpak[0] = NULL;
    }
    if (tty->inpak[1]) {
	inpak_done(tty, tty->inpak[1]);
	tty->inpak[1] = NULL;
    }
    if (tty->inheadpak) {
	inpak_done(tty, tty->inheadpak);
	tty->inheadpak = NULL;
    }
    tty->incount = 0;
    tty->isavedchar = -1;

    while (tty->rxdataq.qhead) {
	inpak_done(tty, dequeue(&tty->rxdataq));
    }
}

/*
 * reset_buffers()
 *	Move all TTY buffers to their free/inactive location
 */
static void
reset_buffers (tt_soc *tty)
{
    /*
     * Free up inbound packets
     */
    reset_rx_buffers(tty);

    /*
     * Free up outbound packet
     */
    if (tty->outpak) {
	outpak_done(tty, tty->outpak);
	tty->outpak = NULL;
    }
}

/*
 * cd2430_setspeed()
 *	Set a baud rate
 */
static boolean
cd2430_setspeed (tt_soc *tty, int txbaud, int rxbaud)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    uchar txpd, rxpd, txclk, rxclk;
    leveltype olevel;

    /*
     * Default baud rates if so desired
     */
    if (txbaud == -1) {
	txbaud = tty->tty_txspeed;
    }
    if (rxbaud == -1) {
	rxbaud = tty->tty_rxspeed;
    }
    if (txbaud > 115200 || rxbaud > 115200 || txbaud < 50  || rxbaud < 50) {
	return(FALSE);		/* Speed not supported */
    }

    /*
     * Calculate closest clock divisor and and period for each
     * baud rate.
     */
    if (!get_clock(txbaud, &txclk, &txpd) ||
	    !get_clock(rxbaud, &rxclk, &rxpd)) {
	return(FALSE);
    }

    /*
     * Disable transmit and receive while changing baud rate.  Clear
     * the channels and move all buffers back to their idle state.
     */
    olevel = raise_interrupt_level(TTY_DISABLE);
    setchan(tty, csr);
    sendccr(csr, CCR_CLRCH | CCR_DISTX | CCR_DISRX);
    reset_buffers(tty);

    /*
     * Fix up transmit and receive speeds
     */
    if (tty->tty_txspeed != txbaud) {
	csr->tcor = (txclk << 5);
	csr->tbpr = txpd;
	tty->tty_txspeed = txbaud;
    }
    if (tty->tty_rxspeed != rxbaud) {
	csr->rcor = rxclk;
	csr->rbpr = rxpd;
	tty->tty_rxspeed = rxbaud;
	tty->cache_rtpr = calc_rtpr(tty->tty_rxspeed);
	csr->rtpr = tty->cache_rtpr;
    }

    /*
     * Replace the buffers under the receive slots.  Turn transmit and
     * receive back on at the new baud rate.  Kick of transmit again.
     */
    alloc_rx_buffers(tty, csr, 0);
    sendccr(csr, CCR_ENTX | CCR_ENRX);
    reset_interrupt_level(olevel);
    startoutput(tty);

    return(TRUE);
}

/*
 * cd2430_autosetspeed()
 *      Set speed for autobaud on given line
 */
static void
cd2430_autosetspeed (tt_soc *tty)
{
    (void)setspeed(tty, AUTOSPEEDHIGH, AUTOSPEEDHIGH);
}

/*
 * cd2430_autobaud
 * Automatically determine line speed from 300 to 19,200 baud.
 * Detect character is RETURN.
 */
static forktype
cd2430_autobaud (tt_soc *tty)
{
    static const uchar
	abmask[] =	{ 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F },
	abchar[] =	{ 0x7F, 0x7E, 0x7E, 0x0D, 0x3b, 0x66, 0x80, 0x0  };
    static const int
	abspeed[] =	{ 14400, 9600, 4800, 2400, 1800, 1200, 600,  300, -1 };
    int speed = -1;
    int ch, retries = autobaud_retry;
    uint i;

    MDMDBG("\nTTY%t: Autobaud started", tty->ttynum);

    /*
     * Try 19.2K first.
     */
    if (try_19200(tty)) {
        tty_startexec(tty);
        return;
    }
    /*
     * The wrong baud rate may well have left further cruft in
     * our buffer.  Clear it out so the regular autobaud code
     * can start cleanly.
     */
    tty_clearbuffers(tty);

    /*
     * Kick off regular autobaud
     */
    setspeed (tty, AUTOSPEED, AUTOSPEED);
    tty_portenable(tty);

    for (;;) {
	ch = getc(tty);			/* get character from the terminal */
	if (ch >= 0) {
	    MDMDBG("\n\nTTY%t: AUTOBAUD saw %#x at %u",tty->ttynum,
		    ch, AUTOSPEED);
	    for (i = 0; i < sizeof(abmask); i++)
		if ((ch & abmask[i]) == abchar[i])
		    break;
	    speed = abspeed[i];
	    if (speed != -1) {
try19:
		if (!setspeed (tty, speed, speed)) {
		    MDMDBG("\nTTY%t: autobaud failed setting speed %d",
			     tty->ttynum, speed);
		    setspeed(tty, AUTOSPEED, AUTOSPEED);
		    tty_clearbuffers(tty);
		    speed = -1;
		    continue;
		}
		MDMDBG("\nTTY%t: autobaud line speed is %d baud?", 
			tty->ttynum, speed);
		ch = auto_verify(tty);
		if (ch < 0) {
		    MDMDBG("\nTTY%t: Autobaud speed verify timeout",
			     tty->ttynum);
		    (void)setspeed(tty, AUTOSPEED, AUTOSPEED);
		    tty_clearbuffers(tty);
		    speed = -1;
		    continue;
		}

		/* Check for 4800 and 600 speeds */

                if ((speed == 300) && ((ch & 0x7f) == 0x72)) {
		    speed = 600;
		    goto try19;
		}

                if ((speed == 9600) && ((ch & 0x7f) == 0x66)) {
                    speed = 4800;
                    goto try19;
                }

		if ((ch == 015) || (ch == 0215)) {
		    tty_startexec(tty);
		} else {
		    MDMDBG("\nTTY%t: Autobaud verify failed (saw %#x), "
			"retrying", tty->ttynum, ch);
		    setspeed (tty, AUTOSPEED, AUTOSPEED);
		    tty_clearbuffers(tty);	
		    speed = -1;
		    continue;
		}
	    }
	}
	process_sleep_for (ONESEC >> 2);

	/*
	 * Either time out or give up if we were hung up (we are idle)
	 */
	if ((--retries <= 0) || (tty->statbits & CARDROP)) {
	    if (modem_debug) {
		if (retries <= 0)
		  ttybug(tty, "Autobaud timed out");
		if (tty->statbits & CARDROP)
		  ttybug(tty, "Autobaud line hung up");
	    }
	    death(tty);

/*
 * Should never get here.  For some reason the call to death() has returned.
 * Note it and kill it good.
 */

	    ttybug(tty, "Call to death() in autobaud() returned.");
	    process_kill(THIS_PROCESS);
	}
    }
}

/*
 * cd2430_paritybits()
 *	Set the requested parity
 *
 * This routine is used to set the parity of the line. If this is a
 * change, we set the appropriate value in the tty block and call
 * cd2430_setcor() to update the option register.
 */
static void
cd2430_paritybits (tt_soc *tty, int type)
{
    if (type != tty->tty_parity) {
	tty->tty_parity = type;
	cd2430_setcor(tty);
    }
}

/*
 * cd2430_setflow()
 *	Set the requested flow control
 *
 * This routine is used to set the flow control of the line. If
 * this is a change, we will call cd2430_setcor()
 * Note: reconfiguring SFLOW on already SFLOW line running output
 *       has been noticed to cause data garbling.
 */
static void
cd2430_setflow (tt_soc *tty)
{

    ulong *status = &(cd2430info_base[tty->ttynum - cd2430Base].status);
    ulong new_status = 0;


    if (tty->capabilities & SFLOWIN) {   /* what flow type is reqd. ? */
	new_status |= SFLOW_IN;
    }
    if (tty->capabilities & SFLOWOUT) {
	new_status |= SFLOW_OUT;
    }
    if (tty->capabilities & HFLOWIN) {
	new_status |= HFLOW_IN;
    }
    if (tty->capabilities & HFLOWOUT) {
	new_status |= HFLOW_OUT;
    }
    /*
     * are we already doing the required flow type ?
     */
    if (new_status != ((*status) & (SFLOW_IN | SFLOW_OUT | HFLOW_IN |
				    HFLOW_OUT))) {
	*status &= ~(SFLOW_IN | SFLOW_OUT | HFLOW_IN | HFLOW_OUT);
	*status |= new_status;    /* remember the new flow type */
	/*
	 * should we wait until xmitter is empty ?
	 * we know that it garbles data, otherwise
	 */
	tty_wait_output(tty, MAX_TTY_OUTPUT_WAIT);
	cd2430_setcor(tty);       /* go configure the channel */
    }
}


/*
 * cd2430_databits()
 *	Set the requested databits
 *
 * This routine is used to set the requested databits of the line. If this
 * is a change, we set the appropriate value in the tty block and update
 * using cd2430_setcor().
 */
static void
cd2430_databits (tt_soc *tty, int bits)
{
    if (bits < 5 || bits > 8) {
	bits = 8;
    }

    if (bits != tty->tty_databits) {
	tty->tty_databits = bits;
	cd2430_setcor(tty);
    }
}

/*
 * cd2430_stopbits()
 *	Set the requested stopbits
 *
 * This routine is used to set the requested stopbits of the line. If this
 * is a change, we set the appropriate value in the tty block and update
 * using cd2430_setcor().
 */
static void
cd2430_stopbits (tt_soc *tty, int bits)
{
    if (bits != tty->tty_stopbits) {
	tty->tty_stopbits = bits;
	cd2430_setcor(tty);
    }
}

/*
 * cd2430_sendbreak()
 *	Send a break
 */
static void
cd2430_sendbreak (tt_soc *tty)
{
    /*
     * We don't send the break here.  Instead, we flag that we would
     * like a break to be sent, and let the TX interrupt handler see
     * the flag, set the special chip mode, and queue up a packet containing
     * the needed data.
     */
    if (!(tty->statbits & (SENDBREAK | SENDINGBREAK))) {
	tty->statbits |= SENDBREAK;
	cd2430_startoutput(tty);
    }
}

/*
 * cd2430_portenable()
 *	Enable the specified port for I/O
 */
static void
cd2430_portenable (tt_soc *tty)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;

    /*
     * Wait for CCR to be free
     */
    setchan(tty, csr);
    if ((csr->csr & (CSR_RXEN|CSR_TXEN)) != (CSR_RXEN|CSR_TXEN))
	sendccr(csr, CCR_ENTX | CCR_ENRX);
}

/*
 * cd2430_portdisable()
 *	Disable the specified port for I/O
 */
static void
cd2430_portdisable (tt_soc *tty)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;

    /*
     * Wait for CCR to be free
     */
    setchan(tty, csr);
    sendccr(csr, CCR_DISTX | CCR_DISRX);
    csr->msvr_dtr = MSVR_DTR;         /* But let modems yack */
}

/*
 *		Modem Control Routines
 */

/*
 * cd2430_dtrstate()
 *	Set the state of DTR
 */
static void
cd2430_dtrstate (tt_soc *tty, boolean sense)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;

    setchan(tty, csr);
    if (sense) {
	csr->msvr_rts |= MSVR_RTS;
    } else {
	csr->msvr_rts &= ~MSVR_RTS;
    }
}

/*
 * cd2430_modemsignal()
 *	Return state of DSR and CTS
 */
static modem_status
cd2430_modemsignal (tt_soc *tty)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    uchar msvr;
    modem_status summary;

    setchan(tty, csr);
    msvr = csr->msvr_rts;		/* Read the modem status */
    summary.ring = ((msvr & MSVR_DSR) != 0);
    summary.cts = ((msvr & MSVR_CTS) != 0);
    summary.dtr = ((msvr & MSVR_RTS) != 0);
    summary.rts = ((msvr & MSVR_DTR) != 0);

    return(summary);
}

/*
 * cd2430_modemchange()
 *	Read and clear modem changed flags
 *
 * The 2400 hardware supports modem line changes, but only within a
 * modem change interrupt handler.  So the interrupt handler drops
 * them into tty->modem_changes, and we pull them out here.
 */
static modem_status
cd2430_modemchange (tt_soc *tty)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    modem_status summary;
    leveltype olev;

    /*
     * Select channel
     */
    setchan(tty, csr);

    /*
     * Note lines which have changed since last check, clear them out.
     * Protect against races.
     */
    olev = raise_interrupt_level(ALL_DISABLE);
    summary = tty->modem_changes;
    tty->modem_changes = (modem_status){0};
    reset_interrupt_level(olev);

    /*
     * Return results
     */
    return(summary);
}

/*
 * print_modemcontrol()
 *	Show the current state of modnem control
 */
void
cd2430_print_modemcontrol (tt_soc *tty)
{
    cd2430_quadart *csr;
    uchar msvr;

    /*
     * This is a global registry, don't try to do things to non-24XX
     * chips.
     */
    if ((tty->ttynum < cd2430Base) ||
	    (tty->ttynum >= (cd2430Base + nasync))) {
	return;
    }

    /*
     * Access chip, show state
     */
    csr = (cd2430_quadart *)tty->uartdata;
    setchan(tty, csr);
    msvr = csr->msvr_rts;
    printf("\nModem hardware state: %sCTS%s%sDSR%s %sDTR %sRTS",
	   msvr & MSVR_CTS ? "":"no", tty->modem_changes.cts ? "* ":" ",
	   msvr & MSVR_DSR ? "":"no", tty->modem_changes.ring ? "* ":" ",
	   msvr & MSVR_RTS ? "":"no",
	   msvr & MSVR_DTR ? "":"no");
}

/*
 * tty_cd2430_xon()
 *	Do out-of-band character for flow control
 */
boolean
tty_cd2430_xon (tt_soc *tty)
{
    /*
     * This is a global registry, don't try to do things to non-24XX
     * chips.
     */
    if ((tty->ttynum < cd2430Base) ||
	    (tty->ttynum >= (cd2430Base + nasync))) {
	return(FALSE);
    }

    /*
     * Send XON if currently in XOFF
     */
    if (tty->capabilities & SFLOWOUT) {
	cd2430_quadart *csr;

	csr = (cd2430_quadart *)(tty->uartdata);
	setchan(tty, csr);
	if (!csr->ccr) {
	    csr->stcr = STCR_SNDSPEC|STCR_SPC1;
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * cd2430_show_contr()
 *	Show async controller info
 */
void
cd2430_show_contr (void)
{
    int chip;

    automore_enable(NULL);
    for (chip = 0; chip < cd2430_controllers; ++chip) {
	cd2430_quadart *csr;
	int chan;

        /*
         * If Modems are not present (ttyptr is NULL), ignore the chip.
         */
        if (cd2430_ttyptr[chip] == NULL)
            continue;

	/*
	 * Point to chip for this bank of channels
	 */
	csr = CD2430ADDRESS(chip);

	/*
	 * Global registers
	 */
	printf("\nCirrus CD2430 Quadart %d, Revision %x\nGlobal registers:",
	    chip, csr->gfrcr);
	printf(
"\n  RPILR %x, RIR %x, RISR %x, STK %x, BERCNT %x",
	    csr->rpilr, csr->rir, csr->risr, csr->stk, csr->bercnt);
	printf(
"\n  TPILR %x, TIR %x, TISR %x, TFTC %x, TPR %x",
	    csr->tpilr, csr->tir, csr->tisr, csr->tftc, csr->tpr);
	printf(
"\n  MPILR %x, MIR %x, MISR %x\n",
	    csr->mpilr, csr->mir, csr->misr);
	printf("\nPer Channel registers:");
	for (chan = 0; chan < CD2430_CHIPCHAN; chan++) {

	    tt_soc *tty;
	    cd2430_priv *cd2430info_chan;

	    if ((tty = CD2430_TO_TTY(chip, chan)) == NULL)
		continue;
	    cd2430info_chan = &cd2430info_base[tty->ttynum - cd2430Base];
	    setchan(tty, csr);

	    printf(
"\n%2d COR: %02x %02x %02x %02x %02x %02x %02x SCHR: %02x %02x %02x %02x %02x",
	    chan, csr->cor1, csr->cor2, csr->cor3,
	    csr->cor4, csr->cor5, csr->cor6, csr->cor7,
	    csr->schr1, csr->schr2, csr->schr3, csr->schr4, csr->cmr);
	    printf(
"\n   SCRlh: %02x %02x LNXT: %02x RX/TX: %x %x %x %x DMABSTS: %x",
	    csr->scrl, csr->scrh, csr->lnxt,
	    csr->rbpr, csr->rcor, csr->tbpr, csr->tcor, csr->dmabsts);
	    printf(
"\n   CSR %x MSVR %x IER %x RFOC %x RTPR %x TTR %x",
	    csr->csr, csr->msvr_rts, csr->ier, csr->rfoc,
	    csr->rtpr, csr->ttr);
	    printf(
"\n   Rx DMA %x/%x %x/%x ST: %x/%x POS: %x TTYCNT: %x SYSBUF: %d %d",
	    csr->arbadrl | (csr->arbadru << 16),
	    csr->brbadrl | (csr->brbadru << 16),
	    csr->arbcnt, csr->brbcnt, csr->arbsts, csr->brbsts,
	    csr->rcbadrl | (csr->rcbadru << 16), tty->incount,
	    cd2430info_chan->rxbuffs_outstanding,
	    cd2430info_chan->rxbuffs_disallowed);
	    printf(
"\n   Tx DMA %x/%x %x/%x ST: %x/%x POS: %x TTYCNT: %x",
	    csr->atbadrl | (csr->atbadru << 16),
	    csr->btbadrl | (csr->btbadru << 16),
	    csr->atbcnt, csr->btbcnt, csr->atbsts, csr->btbsts,
	    csr->tcbadrl | (csr->tcbadru << 16), tty->outcount);
	    printf(
"\n   Rx Buffs: inpk: %x/%x inheadpk: %x dataq: %x %x %d"
"\n             pakq: %x %x %d",
            tty->inpak[0], tty->inpak[1], tty->inheadpak,
            tty->rxdataq.qhead, tty->rxdataq.qtail, tty->rxdataq.count,
            tty->rxpakq.qhead, tty->rxpakq.qtail, tty->rxpakq.count);
             printf(
"\n   Tx Buffs: outpk: %x  txpkq: %x %x %d",
            tty->outpak, tty->txpakq.qhead, tty->txpakq.qtail,
            tty->txpakq.count);
	    printf(
"\n   Error counters: Nested interrupts transmit %d, receive %d, modem %d",
	    cd2430info_chan->nested_tx_interrupts, 
	    cd2430info_chan->nested_rx_interrupts,
	    cd2430info_chan->nested_mdm_interrupts);
	    printf(
"\n   Priv Flags: %8X", cd2430info_chan->status);
	}
    }
    automore_disable();
}

/*
 * fillq()
 *	Initialize a 2430 packet queue
 *
 * These queues are just used to hold our local transmit or receive
 * resources.  We prime them with the indicated number of the indicated
 * size of packet.
 */
static void
fillq (queuetype *q, int max)
{
	int x;
	paktype *pak;

	queue_init(q, max);
	for (x = 0; x < max; ++x) {
	    pak = pool_getbuffer(cd2430_buffpool);
	    if (pak == NULL) {
		break;
	    }
	    pak->length = pak->pool->size;
	    enqueue(q, pak);
	}
}

/*
 * cd2430_inputpending()
 *	Tell if we have input available for our user
 */
static int
cd2430_inputpending (tt_soc *tty)
{
    uint count;

    /*
     * A pushed-back character counts as one more
     */
    if (tty->isavedchar != -1) {
	count = 1;
    } else {
	count = 0;
    }

    /*
     * Check for an empty input queue.  If it appears empty, interlock
     * interrupts, then free the old packet and set up a new one as
     * needed.
     */
    count += tty->incount;
    if (count == 0) {
	leveltype olevel;
	cd2430_quadart *csr = NULL;

	olevel = raise_interrupt_level(TTY_DISABLE);
	if (tty->inheadpak) {
	    inpak_done(tty, tty->inheadpak);
	    tty->inheadpak = NULL;
	}
	if (tty->rxdataq.qhead) {
	    tty->inheadpak = dequeue(&tty->rxdataq);
	    tty_rxptrs(tty, tty->inheadpak);
	}

	/*
	 * If the freed data is needed to continue input, arm
	 * new receive DMA now.
	 */
	if ((tty->inpak[0] == NULL) || (tty->inpak[1] == NULL)) {

	    csr = (cd2430_quadart *)(tty->uartdata);
	    setchan(tty, csr);
	    alloc_rx_buffers(tty, csr, (tty->inpak[0] == NULL) ? 0 : 1);
	}


	/*
	 * If we still have no data, request interrupt after next byte
	 */
	count = tty->incount;
	if ((count == 0) && !tty->short_rtpr) {
	    if (csr == NULL) {
		csr = (cd2430_quadart *)(tty->uartdata);
		setchan(tty, csr);
	    }
	    csr->rtpr = 1;
	    tty->short_rtpr = TRUE;
	}

	reset_interrupt_level(olevel);
    }

    /*
     * If input has been stopped due to SW flow control, start it again
     */
    if ((count == 0) && (tty->statbits & STOPPEDINPUT)) {
	startinput(tty);
    }

    /*
     * Return count
     */
    return(count);
}

/*
 * cd2430_pppmode()
 *	Switching to PPP mode, tell chip
 */
void
cd2430_pppmode (tt_soc *tty, int on)
{
    cd2430_quadart *csr;
    pppContext *ctx;

    /*
     * If not changing, or can't change, no-op
     */
    if (!can_ppp ||
	    (on && (tty->statbits & PPPMODE)) ||
	    (!on && !(tty->statbits & PPPMODE))) {
	return;
    }

    /*
     * Access channel's registers
     */
    csr = (cd2430_quadart *)(tty->uartdata);
    setchan(tty, csr);

    /*
     * Disable interrupts
     */
    csr->ier = 0;

    /*
     * Flush I/O
     */
    sendccr(csr, CCR_CLRCH | CCR_DISTX | CCR_DISRX);

    /*
     * Update 2430 mode
     */
    ctx = tty->appp_context;
    if (on) {
	csr->cmr = CMR_RXDMA | CMR_TXDMA | CMR_MODE_PPP;
	tty->statbits |= PPPMODE;
	tty->txservice = NULL;		/* Avoid this interface */
	tty->rxservice = NULL;

	/*
	 *  Set the Rx throttle re-enable.
	 */
	if (ctx && ctx->idb) {
		ctx->idb->enable = cd2430_pppmode_unthrottle;
		idb_mark_unthrottled(ctx->idb);
	}

    } else {
	csr->cmr = CMR_RXDMA | CMR_TXDMA | CMR_MODE_ASYNC;
	tty->statbits &= ~PPPMODE;

	/*
	 *  Clear the Rx throttle re-enable.
	 */
	if (ctx && ctx->idb) {
		ctx->idb->enable = (enable_t)return_nothing;
		idb_mark_unthrottled(ctx->idb);
	}

    }

    /*
     * Clear all connected buffers
     */
    reset_buffers(tty);

    /*
     * Re-arm RX DMA under new mode.
     */
    alloc_rx_buffers(tty, csr, 0);

    /*
     * Set channel options. This also initializes the channel;
     * the chip will see its new mode.
     */
    cd2430_setcor(tty);

    /*
     * Restore rtpr to correct value
     */
    csr->rtpr = tty->cache_rtpr;

    /*
     * Re-enable transmit/receive
     */
    sendccr(csr, CCR_ENTX | CCR_ENRX);

    /*
     * Re-enable interrupts.  Transmit interrupts will come on when output
     * is started.
     */
    csr->ier = IER_BASIC;
}

/*
 * cd2430_pppmap()
 *	Update hardware knowledge of PPP control character map
 */
void
cd2430_pppmap (tt_soc *tty, ulong txmap, ulong rxmap)
{
    cd2430_quadart *csr;

    /*
     * Take no action if PPP is unavailable or off.  Otherwise access the
     * appropriate channel.
     */
    if (!can_ppp || !(tty->statbits & PPPMODE)) {
	return;
    }
    csr = (cd2430_quadart *)(tty->uartdata);
    setchan(tty, csr);

    /*
     * Fill in TXMAP and RXMAP
     */
    csr->rxmap1 = rxmap;		csr->txmap1 = txmap;
    csr->rxmap2 = (rxmap >> 8);		csr->txmap2 = (txmap >> 8);
    csr->rxmap3 = (rxmap >> 16);	csr->txmap3 = (txmap >> 16);
    csr->rxmap4 = (rxmap >> 24);	csr->txmap4 = (txmap >> 24);
}

/*
 * cd2430_clearinput()
 *	Remove all input
 */
static void
cd2430_clearinput (tt_soc *tty)
{
    leveltype olev;
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    boolean rx_disabled = FALSE;

    /*
     * Hold off input interrupts
     */
    olev = raise_interrupt_level(TTY_DISABLE);

    /*
     * Disable receiver (if enabled), clear RX DMA, free all queued data and 
     * arm fresh RX buffers
     */
    setchan(tty, csr);
    if (csr->csr & CSR_RXEN) {
	sendccr(csr, CCR_DISRX); 
	rx_disabled = TRUE;
    }
    sendccr(csr, CCR_SET1 | CCR_CLRRCV);
    reset_rx_buffers(tty);
    alloc_rx_buffers (tty, csr, 0);

    /*
     * Release flow control
     */
    if (tty->statbits & SENTXOFF) {

	csr->stcr = STCR_SNDSPEC|STCR_SPC1;
	tty->statbits &= ~(SENTXOFF);
    }

    /*
     * Reenable receiver if it was disabled 
     */
    if (rx_disabled) {
	sendccr(csr, CCR_ENRX);
    }

    /*
     * Allow interrupts
     */
    reset_interrupt_level(olev);

    /*
     * Continue input if held
     */
    if (tty->statbits & STOPPEDINPUT) {
	startinput(tty);
    }
}

/*
 * cd2430_clearoutput()
 *	Remove output queued but not yet in DMA
 */
static void
cd2430_clearoutput (tt_soc *tty)
{
    leveltype status;
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    boolean tx_disabled = FALSE;

    status = raise_interrupt_level(TTY_DISABLE);
    setchan(tty, csr);

    /*
     * Disable transmitter (if enabled), clear current transmit buffer 
     * if necessary & possible
     */
    if (tty->outpak && !cd2430_istate) {

  	if (csr->csr & CSR_TXEN) {
  	    sendccr(csr, CCR_DISTX); 
  	    tx_disabled = TRUE;
  	}
	sendccr(csr, CCR_SET1 | CCR_CLRTX);
	outpak_done(tty, tty->outpak);
	tty->outpak = NULL;
    }

    /*
     * Dump any queued outbound data, resetting pointers and
     * ensure an output packet is set up.
     */
    tty_txptrs(tty, (paktype *)(tty->txpakq.qhead));

    /*
     * Reenable transmitter if it was disabled
     */
    if (tx_disabled) {
 	sendccr(csr, CCR_ENTX);
    }
    reset_interrupt_level(status);
}

/*
 * store_2402()
 *	Store next 18-bit value to 2402
 */
static inline void
store_2402 (cd2430_quadart *csr, ulong val)
{
    /*
     * Feed value, strobe dmr
     */
    csr->airl = val;
    csr->airm = val >> 8;
    csr->airh = val >> 16;
    csr->dmr = DMR_SSTEP|DMR_CLKDIS;
}

/*
 * cd2402_download()
 *	Download firmware to the programmable flavor of 24XX
 */
void
cd2402_download (cd2430_quadart *csr)
{
    uchar *p;
    uint index;
    ulong val;
    leveltype level;
    extern uchar cd2402_firmware[];
    extern uint cd2402_firmware_size;

    level = raise_interrupt_level(ALL_DISABLE);

    /*
     * Set jmp 0x1fff instruction
     */
    csr->airl = 0xff;
    csr->airm = 0x3f;
    csr->airh = 0x3;

    /*
     * Set data write mode w. clock stopped
     */
    csr->mtcr = 1;
    csr->dmr = DMR_CLKDIS;
    csr->mtcr = 2;

    /*
     * Encapsulate repetetive loop condition
     */
#define STORE(v) store_2402(csr, v); \
    if (++index >= cd2402_firmware_size) break;

    /*
     * Extract 18-bit words from the byte array, write them to the chip
     */
    index = 0;
    p = cd2402_firmware;
    for (;;) {
	/*
	 * Word 1: lies on aligned byte
	 */
	val =   (*p++);
	val |=  (*p++ << 8);
	val |= ((*p & 0x3) << 16);
	STORE(val);

	/*
	 * Word 2: lies two bits up on byte
	 */
	val =  ((*p++ >> 2) & 0x3F);
	val |=  (*p++ << 6);
	val |= ((*p & 0xF) << 14);
	STORE(val);

	/*
	 * Word 3: lies four bits up on byte
	 */
	val =  ((*p++ >> 4) & 0xF);
	val |=  (*p++ << 4);
	val |= ((*p & 0x3F) << 12);
	STORE(val);

	/*
	 * Word 4: lies six bits up on byte, last byte is aligned,
	 * and thus we start over at word 1 next time.
	 */
	val =  ((*p++ >> 6) & 0x3);
	val |=  (*p++ << 2);
	val |=  (*p++ << 10);
	STORE(val);
    }
#undef STORE

    /*
     * Fill in trailing storage with no-op
     */
    for ( ; index <= 0x1fff; ++index) {
	store_2402(csr, 0x1e000);
    }

    /*
     * Now tell him to run the code
     */
    csr->airl = 0xff;			/* jump 0x1fff */
    csr->airm = 0x3f;
    csr->airh = 0x3;
    csr->mtcr = 0x1;			/* single step mode */
    csr->dmr = DMR_SSTEP|DMR_CLKDIS;	/* step */
    csr->dmr = DMR_SSTEP|DMR_CLKDIS;	/* step */
    csr->mtcr = 0x3;			/* end single step */
    csr->dmr = 0;			/* run */

    reset_interrupt_level(level);
}

/*
 * cd2430_vector_init
 *	setup tty vector table.
 */
void
cd2430_vector_init()
{
    /*
     * Wire ourselves in to our interrupt vectors
     */
    vector_init(&cd2430_vector);

    /*
     * Input processing
     */
    cd2430_vector.vgetc = serial_getc;
    cd2430_vector.vstartinput = cd2430_startinput;
    cd2430_vector.vclearinput = cd2430_clearinput;
    cd2430_vector.vinputpending = cd2430_inputpending;

    /*
     * Output processing
     */
    cd2430_vector.vputc = serial_putc;
    cd2430_vector.vstartoutput = cd2430_startoutput;
    cd2430_vector.vclearoutput = cd2430_clearoutput;
    cd2430_vector.voutputpending = serial_outputpending;
    cd2430_vector.voutputblock = serial_outputblock;
    cd2430_vector.voutputxoffed = cd2430_outputxoffed;
    cd2430_vector.vputpkt = cd2430_putpkt;

    /*
     * Special functions
     */
    cd2430_vector.vsetspeed = cd2430_setspeed;
    cd2430_vector.vautosetspeed = cd2430_autosetspeed;
    cd2430_vector.vauto_baud = cd2430_autobaud;
    cd2430_vector.vparitybits = cd2430_paritybits;
    cd2430_vector.vdatabits = cd2430_databits;
    cd2430_vector.vstopbits = cd2430_stopbits;
    cd2430_vector.vsendbreak = cd2430_sendbreak;

    cd2430_vector.vservicehook = generic_servicehook;
    cd2430_vector.vnoservicehook = generic_noservicehook;

    cd2430_vector.vdtrstate = cd2430_dtrstate;
    cd2430_vector.vportenable = cd2430_portenable;
    cd2430_vector.vportdisable = cd2430_portdisable;
    cd2430_vector.vmodemsignal = cd2430_modemsignal;
    cd2430_vector.vmodemchange = cd2430_modemchange;
    cd2430_vector.vsetflow = cd2430_setflow;
    cd2430_vector.vsetsflow = cd2430_setflow;
}

/*
 * cd2430_channel_setup -
 *    setup tty parameters and initialize CD2430 channel and identification
 */

void
cd2430_channel_setup(tty, csr, channel_id)
tt_soc *tty;
cd2430_quadart *csr;
int  channel_id;
{
    tty->uartdata = csr;
    tty->tty_databits = 8;
    tty->tty_parity = PARITY_NO;
    tty->tty_stopbits = STOPBITS2;
    tty->tty_stopchar = CTRLS; /* XOFF */
    tty->tty_startchar = CTRLQ;	/* XON */
    tty->modem_type = MODEMTYPE_LOCAL; /* Default to local line */
    tty->modem_changes = (modem_status){0};

    /*
     * Initialize the packet memory which is dedicated statically
     * as DMA data buffers.  Flag that the TTY, initially, has no
     * input data.  insize is not really used (we use the pak size
     * instead) but tell the world the size our pool uses.
     */
    fillq(&tty->rxpakq, CD2430_DEF_RXBUFF);
    tty->insize = CD2430_PAKSIZE;
    tty->incount = 0;
    fillq(&tty->txpakq, CD2430_DEF_TXBUFF);
    tty_txptrs(tty, (paktype *)tty->txpakq.qhead);

    /*
     * Initialize TTY queues
     */
    queue_init(&tty->rxdataq, 0);

    /*
     * Select proper line
     */
     setchan(tty, csr);

    /*
     * Initialize mode, set options and baud rate
     */
     csr->cmr = CMR_RXDMA | CMR_TXDMA | CMR_MODE_ASYNC;
     csr->cpsr = CPSR_V41;
     tty->statbits &= ~PPPMODE;
     sendccr(csr, CCR_INITCH);
     cd2430_setcor(tty);
     cd2430_setspeed(tty, ASYNC_DEFSPEED, ASYNC_DEFSPEED);
     cd2430_setflow(tty);


     csr->msvr_dtr = MSVR_DTR; /* Set DTR and RTS */
     csr->msvr_rts |= MSVR_RTS;
     csr->livr = channel_id;

    /*
     * Command channel to enable receive/transmit
     */
     sendccr(csr, CCR_ENTX | CCR_ENRX);
     csr->ier = IER_BASIC;	/* Enable only basic interrupts */

    /*
     * Arm receive buffers
     */
     alloc_rx_buffers(tty, csr, 0);
}


/*
 * cd2430_setup_break_packet()
 * create a BREAK packet.
 */
boolean cd2430_setup_break_packet(void)
{
  uchar *p;
 
    /*
     * Create a "generate BREAK" packet. Malloc from IO mem instead
     * of getting a buffer to leave initial system buffer levels at
     * desired/configured values.   For compatibility, we send break
     * for 1/2 sec.  The trailing escapes are to cover a race condition
     * where the data is still streaming out, but our interrupt handler
     * wants to turn off ETC mode.  The escapes will be harmless trailer
     * data with or without ETC mode.
     */
    break_pak = malloc_iomem(CD2430BREAKDATA + sizeof(paktype));
    if (!break_pak) {
        errmsg(&msgsym(NOBRKPAK, TTYDRIVER));
        crashdump(0);
        return FALSE;       /* Be paranoid */
    }
    break_pak->datagramstart = ((char *)break_pak) + sizeof(paktype);
 
    p = break_pak->datagramstart;
    *p++ = ETC_ESC; *p++ = ETC_BREAK;
    *p++ = ETC_ESC; *p++ = ETC_DELAY; *p++ =
        (cd2430clock / 2048 / CD2430_PRESCALE) / 2;
    *p++ = ETC_ESC; *p++ = ETC_ENDBREAK;
    break_pak->datagramsize = break_pak->length =
        (char *)p - (char *)break_pak->datagramstart;
    return TRUE;
}

void cd2430_download_controller_firmware (cd2430_quadart *csr)
{
    int line;
    uint x;

    /*
     * Disable the bit engines
     */
    for (line = 0; line < CD2430_CHIPCHAN; line++) {
	csr->car = line;
	csr->tbpr = 0;
	csr->rbpr = 0;
    }

    /*
     * Dump new code to it
     */
    cd2402_download(csr);

    /*
     * Wait a bit for gfrcr to clear (reset started on the chip).
     * Max delay 1/8 sec, in case we missed the entire reset.
     */
    for (x = 0; x < 25; ++x) {
	if (csr->gfrcr == 0) {
	    break;
	}
	DELAY(5);
    }
}
