/* $Id: if_hes_common.c,v 3.12.8.14 1996/08/26 15:07:19 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_hes_common.c,v $
 *------------------------------------------------------------------
 * common_if.c -- Common RSP, cBus, MCI interface code
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_hes_common.c,v $
 * Revision 3.12.8.14  1996/08/26  15:07:19  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.12.8.13  1996/08/15  18:06:16  ptran
 * Add ESA's idb to VIP and RSP hwidb/swidb chain.
 * CSCdi64021:  show diag doesnt display EPA info when the next PA is empty
 * Branch: California_branch
 *
 * Revision 3.12.8.12  1996/08/13  02:17:41  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.12.8.11  1996/08/08  14:56:29  bstewart
 * CSCdi64752:  Fix a port adapter name and add another
 * Branch: California_branch
 *
 * Revision 3.12.8.10  1996/07/26  01:09:55  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.12.8.9  1996/07/09  05:51:21  ppearce
 * CSCdi59527:  inlines wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.12.8.8  1996/07/08  22:25:25  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.12.8.7  1996/06/30  18:10:52  ptran
 * CSCdi61257:  snmp and show diag support for encryption PA
 * Branch: California_branch
 *
 * Revision 3.12.8.6  1996/06/09  07:58:32  cheilek
 * CSCdi52427:  VIP1/VIP2 cardType enumeration labels should be vip/vip2
 * Branch: California_branch
 * Change MIBS: rvip to vip; svip to vip2.
 *
 * Revision 3.12.8.5  1996/05/17  11:18:16  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.2.5  1996/05/05  23:07:14  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.11.2.4  1996/05/02  22:03:32  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.3  1996/04/26  15:18:50  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.2  1996/04/03  14:29:54  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.2.1  1996/03/17  17:49:22  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.12.8.4  1996/05/14  02:56:33  feisal
 * CSCdi56797:  Card type for the 5EFL PA not added properly to the chasis
 * MIB
 * Branch: California_branch
 *
 * Revision 3.12.8.3  1996/04/27  05:48:12  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.7.2.3  1996/04/26  00:56:41  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.7.2.2  1996/04/17  01:50:17  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.7.2.1  1996/02/23  20:21:51  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.12.8.2  1996/03/21  22:40:44  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.12.8.1  1996/03/18  19:41:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.5  1996/03/18  23:19:20  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.4  1996/03/14  01:02:06  fsunaval
 * CSCdi50801:  SW-VIP1: Show diag has wrong board type for FDDI PA
 * Branch: ELC_branch
 *
 * Revision 3.3.2.3  1996/02/08  07:21:54  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.2  1996/01/15  10:19:32  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/11  18:47:32  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.12  1996/02/28  20:21:52  kmitchel
 * CSCdi40850:  CSCdi40542 fix causes %CBUS-3-FCICMDFAIL1 error on
 *              mic reload
 *              - Select the interface before sending a command to it.
 *
 * Revision 3.11  1996/02/15  00:38:21  dbath
 * CSCdi38455:  HSA needs SNMP support
 * Add RSP controller types to card_type_and_descr().  (Moved the
 * code from rsp_chasint.c to here).
 *
 * Revision 3.10  1996/01/25  11:17:52  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.9  1996/01/23  08:47:53  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.8  1996/01/18  15:45:38  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.7  1996/01/11  03:50:19  kao
 * CSCdi43878:  IS_VIP() macro should be used
 *
 * Revision 3.6  1995/12/19  00:40:13  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.5  1995/12/10  23:27:32  rpratt
 * CSCdi45343:  Chassis MIB enhancements for VIP & port adapters
 *
 * Revision 3.4  1995/11/30  06:53:36  tboyle
 * CSCdi44059:  FE interface and protocol goes down and never comes up.
 *
 * Revision 3.3  1995/11/17  09:18:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:00:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/08/12  02:28:50  schaefer
 * CSCdi38497: don't get cbus errors during cbus reset
 *
 * Revision 2.6  1995/07/31  21:28:40  schaefer
 * CSCdi37941: OIR processes should block
 * remove edisms
 *
 * Revision 2.5  1995/07/17  08:21:14  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.4  1995/07/14  08:05:16  schaefer
 * CSCdi36459: hideous subsystem dependancy
 * use subsys_init_class service to do post-driver init
 *
 * Revision 2.3  1995/06/23  18:47:41  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/21  03:27:08  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  20:41:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "subsys.h"
#include "sys_registry.h"
#include "cbus_registry.h"
#include "interface_private.h"
#include "if_mci.h"
#include "if_fci.h"
#include "../if/network.h"
#include "if_hes_common.h"
#include "if_cmd.h"
#include "../wan/dialer_registry.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "ieee.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../if/linkdown_event.h"
#include "../h/logger.h"


/*
 * Global restart counters
 * These counters replaced the restarts_lineup, restarts_output, and
 * restarts_error counters that were in the cbus structure:
 */
int cbus_restarts_lineup;
int cbus_restarts_output;
int cbus_restarts_error;


/*
 * Hotswap stuff.  Need to be declared in a global module because
 * drivers write to them, even on platforms that don't support
 * hotswap, and so don't link with hotswap.c (e.g. AGS).
 */
boolean hotstall_smt_flag;

/*
 * Controller counts
 */
int ncBus;			/* number of cisco cBus controllers */

/*
 * Maximum interfaces per cbus controller.  Use this when attempting
 * to decode hwidb->mci_index, sometimes known as 'unit'.  These
 * fields are calculated as slot*cbus_max_ctrlr_interfaces + port.
 * Traditionally, code has checked cbus->flags to determine what sort
 * of cbus is present, and then do it's modulo arithimetic on values
 * appropriate for that cbus type.  Some platforms (e.g. RSP) don't have
 * cbus->flags.  So, rather than have lots of code with intimate knowledge
 * of how units relate back to bus type, each platform will load up the
 * the following variable with an appropriate value.  Then, portable code
 * can use cbus_max_ctrlr_interfaces rather than CBUSn_MAX_CTRLR_INTERFACES.
 */
int cbus_max_ctrlr_interfaces;

/*
 * Autonomous switching hit counter.
 */
ulong cbus_ipcache_hits;

ushort cbus_switching_supported;

/*
 * Externs:
 */
extern void  platform_idb_init(hwidbtype *hwidb);


/*
 * initialize cbus stuff after all other drivers have initialized
 */
static void cbus_init_class (ushort class)
{
    switch (class) {
      case SUBSYS_CLASS_DRIVER:
	ncBus = cbus_analyze(TRUE /* first time */, FALSE /* not EOIR */);
	break;
    }
    return;
}

/*****************************************************************************
 * Description: subsystem initialization routine for IF
 *
 * NOTE: this must happen before using any cbus registries
 *
 * Input: 
 *   subsys -- pointer to the subsystem structure
 *
 * Return Value: None
 *
 *****************************************************************************/

static void cbus_mci_init_sub (subsystype *subsys)
{
    cbus_platform_registry_init();

    /*
     * arrange to call cbus_analyze after all drivers have initialized
     */
    reg_add_subsys_init_class(cbus_init_class, "cbus_init_class");

    /*
     * Link in code to show auxiliary switching counts for
     * 'show int stat'
     */
    reg_add_show_auxiliary_switch_counts(platform_show_switch_counts,
					 "platform_show_switch_counts");
}

/*
 * cBus/MCI subsystem header
 */
#define CBUS_MCI_SUBSYS_MAJVERSION   1
#define CBUS_MCI_SUBSYS_MINVERSION   0
#define CBUS_MCI_SUBSYS_EDITVERSION  1

SUBSYS_HEADER(cbus_mci,
	      CBUS_MCI_SUBSYS_MAJVERSION, CBUS_MCI_SUBSYS_MINVERSION, 
              CBUS_MCI_SUBSYS_EDITVERSION,
	      cbus_mci_init_sub, SUBSYS_CLASS_DRIVER,
	      "seq: dbus, rsp, chassis",
	      NULL);

/*****************************************************************************
 * Name: cbus_mci_shutdown
 *
 * Description: shutdown an interface (set state to "newstate)
 *   This function is used in the "hwidb->shutdown" vector in the IDB.
 *   Disable a cBus or MCI interface by turning off the VLSI 
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   newstate -- the new state of the IDB
 *
 * Return Value: None
 *
 *****************************************************************************/
static void cbus_mci_shutdown (hwidbtype *hwidb, int newstate)
{
    hwidb->state = newstate;
    hwidb->reset(hwidb);
}

/*****************************************************************************
 * Name: cbus_mci_set_maxdgram
 *
 * Description: Change the maximum datagram size for this interface.
 *   This function is used in the "hwidb->set_maxdgram" vector in the IDB.
 *   This effects how a controller's memory is carved up.
 *   
 *   Be very careful not to reinitialize the controller if we are in
 *   a critical section.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   bufsize -- the size (in bytes) of the buffer minus the mci/cbus
 *     overhead 
 *   maxdgram -- maximum possible size (in bytes) of the datagram that
 *     we can receive 
 *
 * Return Value: 
 *   boolean -- TRUE = datagram size was changed, 
 *              FALSE =  datagram size was *NOT* changed
 *
 *****************************************************************************/
boolean cbus_mci_set_maxdgram (hwidbtype *hwidb, int bufsize, int maxdgram)
{
    if (!(hwidb->status & (IDB_MCI | IDB_CBUS))) {
	printf(set_maxdgram_failed, maxdgram, hwidb->hw_namestring);
	return(FALSE);
    } else {
	/*
	 * Make sure something really changed before resetting the cbus.
	 */
	if ((bufsize == hwidb->buffersize) &&
	    (maxdgram == hwidb->max_buffer_size)) {
	    return(TRUE);
	} else {
	    if_maxdgram_adjust(hwidb, maxdgram);
	    hwidb->buffersize = bufsize;

	    platform_demand_complex_restart(hwidb);

	    return(TRUE);
	}
    }
}


/*****************************************************************************
 * Name: cbus_mci_carrier
 *
 * Description: Handle carrier transitions occurring at interrupt level
 *   May be called from the normal interrupt handler or from the
 *   high speed switching routines.
 *
 *   NOTE: this function must be called with NETS_DISABLED, to work correctly.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void cbus_mci_carrier (hwidbtype *hwidb)
{
    int newstate;
    ushort carrier_state;
    
    select_if(hwidb);
    send_carrier_cmd(hwidb, &carrier_state);

    /*
     * If this is a VIP ethernet device, use carrier transition to turn
     * the protocol off or on so that keepalives can bring the line back
     */
    if (is_vip(hwidb) && (hwidb->status & IDB_ETHER)) {
        if (carrier_state == 0)
            hwidb->mci_lineup = 0;
        else
            hwidb->mci_lineup = MCI_KEEP_COUNT;
        return;
    }

    if (carrier_state == 0){
	newstate = IDBS_DOWN;
        if (hwidb->state != IDBS_DOWN)
             hwidb->failCause = Serial_SignalLost;
    } else {
	newstate = IDBS_UP;
    }
    if ((hwidb->state != newstate) && (hwidb->state != IDBS_ADMINDOWN) &&
	!reg_invoke_dialer_spoofing_int(hwidb)) {
	net_cstate(hwidb, newstate);
	hwidb->counters.transitions++;
	
	platform_light_off(hwidb);
    }

    if (hwidb->type == IDBTYPE_CBUSSERIAL)
	mci_applique(hwidb);
    
    /*
     * If we are spoofing, that means that we are pretending that the
     * idb state is IDBS_UP even though it isn't.  So, if carrier has
     * come up (newstate == IDBS_UP) and we are spoofing, act the same
     * as if idb->state != newstate.  Spoofing will be cleared in the
     * timer routine which deals with bouncing signals.
     */
    if ((reg_invoke_dialer_spoofing_int(hwidb) && newstate == IDBS_UP) &&
            (hwidb->state != IDBS_ADMINDOWN)) {
        net_cstate(hwidb, newstate);
	hwidb->counters.transitions++;
	platform_light_off(hwidb);
    }
    
    /*
     * Ensure lights are set up appropriately -- for MCI interfaces.
     */
    if (hwidb->status & IDB_MCI) {
	if (newstate == IDBS_UP)
	    platform_light_on(hwidb);
	platform_update_lights(hwidb);
    }
    
    /*
     * Protect against a thrashing DCD line by calling the
     * input error counter.  If we get too many transitions
     * within the counting period, the interface will be disabled.
     * Fix up idb->counters.input_error since these aren't real errors.
     */
    cbus_mci_input_error(hwidb);
    hwidb->counters.input_error--;
}

/*****************************************************************************
 * Name: cbus_mci_linestate
 *
 * Description: Return current line protocol status
 *   This function is used in the "hwidb->linestate" vector in the IDB.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: 
 *   boolean -- TRUE = line up,  FALSE = line down
 *
 *****************************************************************************/
static boolean cbus_mci_linestate (hwidbtype *hwidb)
{
    return(hwidb->mci_lineup);
}

/*****************************************************************************
 * Name: cbus_set_ether_tx_delay
 *
 * Description: set transmitter_delay/tx_burst-deferral value
 * for EIP cbus interfaces
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
static void cbus_set_ether_tx_delay (hwidbtype *idb, int *value)
{
    if (*value & ETH_TXBD_MASK) { /* ie came from tx-burst-deferral command */
 
        idb->tx_burst_deferral = *value >> ETH_TXBD_SHIFT_GET;
        /*
         * If a user had previously entered a tx-burst-deferral value
         * with the old scheme (ie via transmitter-delay) wipe
         * the value from xmitdelay bit-field to avoid confusion.
         * Let the user know his old value has been superceded.
         */
        if (idb->xmitdelay & ETH_OLD_TXBD_MASK)
            printf("Now using tx-burst-deferral value of %d (previous %d).\n",
                        idb->tx_burst_deferral,
                    (idb->xmitdelay & ETH_OLD_TXBD_MASK) >> 8);
        idb->xmitdelay &= ETH_TXD_MASK;
        *value = (*value >> ETH_TXBD_SHIFT_PUT) | (*value & ETH_TXD_MASK);
 
    } else { /* i.e. came from a transmitter-delay command */
 
        /* Put in some AI - override with present tx-b-delay value */
        if (idb->tx_burst_deferral) {
            if (*value & (~ETH_TXD_MASK))
                 printf("Using tx-burst-deferral of %d;  transmitter-delay of %d
\n",
                        idb->tx_burst_deferral, *value & ETH_TXD_MASK);
            *value = (idb->tx_burst_deferral << 8) | (*value & ETH_TXD_MASK);
            idb->xmitdelay = (*value & ETH_TXD_MASK);
        } else
            idb->xmitdelay = *value;
 
    }
}

/*****************************************************************************
 * Name: cbus_set_transmit_delay
 *
 * Description: send transmit delay value to cbus interfaces
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void cbus_set_transmit_delay (hwidbtype *idb, int value)
{
    leveltype level;
    
    if (idb->status & IDB_ETHER)
        cbus_set_ether_tx_delay(idb, &value);
    else
        idb->xmitdelay = value;

    level = raise_interrupt_level(NETS_DISABLE);
    select_if(idb);
    send_delay_cmd(idb, value >> 1);
    reset_interrupt_level(level);
}

/*****************************************************************************
 * Name: get_cbus_errors
 *
 * Description: Accumulates cBus interface errors.
 *   Called by the interface periodic function.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void get_cbus_errors (hwidbtype *idb)
{
    leveltype level;
    
    /*
     * If a cbus reset or a hotswap is in progress, skip this error
     * check so that we don't accidentally access a card that has
     * just been removed
     */
    if (platform_cbus_reset_in_progress(idb) || hotstall_flag)
	return;

    /*
     * If the interface is a CBUS interface and if it is NOT
     * administratively down, we need to gather the statistics.
     */
    if (idb->state != IDBS_ADMINDOWN) {
	if (idb->status & IDB_CBUS) {
	    /*
	     * OK, here we go.
             *   1. disable interrupts,
	     *   2. select the interface,
	     *   3. gather stats from the sp (C7000 and AGS+ only)
	     *   4. gather stats based on interface type.
	     *   5. restore interrupt enable/disable level
	     */
            level = raise_interrupt_level(NETS_DISABLE);
	    select_if(idb);
	    get_cbus_ctrlr_errors(idb);

	    /*
	     * Gather statistics based on interface type
	     */
	    reg_invoke_cbus_get_ip_errors(idb->fci_type, idb);

            reset_interrupt_level(level);
	}
    }
}


/* 
 * walk idb queue and shut down any newly removed interfaces
 */
void shutdown_pulled_interfaces (void) 
{
    hwidbtype* hwidb;
    ulong state;

    if (oir_debug)
	buginf("\n  Starting shutdown_pulled_interfaces()");
    FOR_ALL_HWIDBS(hwidb) {
	if (hwidb->status & IDB_NOHARDWARE) {
	    state = hwidb->state;
	    if (oir_debug) {
		buginf("\nSaving pre_hotstall_state=%d, was %d, shutting down %s",
		       state, hwidb->pre_hotstall_state, hwidb->hw_namestring);
	    }
	    if (hwidb->state != IDBS_ADMINDOWN)
		shutdown_going_down(hwidb);
	    hwidb->pre_hotstall_state = state;
	}
    }
    if (oir_debug)
	buginf("\n  Ending shutdown_pulled_interfaces()");
}

/*
 * carry out protocol stack initialization functions for newly created idb
 */
void init_new_hwidb (hwidbtype* idb) 
{
    idbtype *swidb;

    swidb = idb->firstsw;

    idb_final_hw_init(idb);
    idb_final_sw_init(swidb);

    /*
     * Initialize protocol support in the swidb including hardware
     * data structures (TRUE).  Then initialize protocol support in
     * the hwidb (this needs to be reworked!). Then setup fast switching.
     */
    reg_invoke_sw_idb_init(swidb, TRUE);
    reg_invoke_hw_idb_init(idb);
    reg_invoke_fast_setup(idb);
}

/*****************************************************************************
 * Name: hes_idb_init_before
 *
 * Description: Sets up the basic IDB fields for the high end systems
 *   prior to any interface specific initialization that may overwrite
 *   some of the same fields.
 *
 * Input: 
 *   hwidb -- valid pointer to the hardware idb (must not be NULL)
 *     The following fields in the IDB must be filled in beforehand
 *	   -- NONE --> Except those required by platform_idb_init
 *
 * Return Value: None
 *
 *****************************************************************************/
void hes_idb_init_before (hwidbtype *hwidb)
{
    hwidb->next_idb_on_physint = NULL;
    set_default_queueing(hwidb);
    hwidb->iqueue = raw_enqueue;
    hwidb->linestate = cbus_mci_linestate;
    hwidb->shutdown = cbus_mci_shutdown;
    hwidb->device_periodic = get_cbus_errors;
    hwidb->reliability = 255;
    hwidb->load = 1;
    hwidb->rxload = 1;
    GET_TIMESTAMP(hwidb->state_time);
    ieee_zero(hwidb->hardware);
    hwidb->show_controller = NULL;
    hwidb->error_threshold = 5000;
    hwidb->error_count = 0;
    hwidb->mci_lineup = MCI_KEEP_COUNT;
    GET_TIMESTAMP(hwidb->mci_lineuptimer);
    hwidb->firstsw->span_disabled = FALSE;
    TIMER_STOP(hwidb->lastinput);
    TIMER_STOP(hwidb->lastoutput);
    TIMER_STOP(hwidb->lastreset);
    hwidb->vc = NO_VC;

    /*
     * Platform specific IDB initialization -- each platform has a
     * function named this
     */
    platform_idb_init(hwidb);
}

/*****************************************************************************
 * Name: hes_idb_init_after
 *
 * Description: Sets up additional IDB fields for the high end systems
 *   after any interface specific initialization.
 *   some of the same fields.
 *
 * Input: 
 *   hwidb -- valid pointer to the hardware idb (must not be NULL)
 *     The following fields in the IDB must be filled in beforehand
 *       slot -- the slot number in which the interface
 *       slotunit -- the bit-significant status of the IDB
 *       name -- the interface's name string
 *
 * Return Value: None
 *
 *****************************************************************************/
void hes_idb_init_after (hwidbtype *hwidb)
{
    hwidb->typestring = cbus_mci_idb_typestring(hwidb);
}

/*****************************************************************************
 * Name: cbus_af_or
 *
 * Description: OR a bit mask into an address filter location
 *
 * Input: 
 *   hwidb -- valid pointer to the hardware idb for the interface
 *   index -- the index of the address filter element to operate on 
 *   value -- the value to OR into the address filter element
 *
 * Return Value: None
 *
 *****************************************************************************/
void cbus_af_or (hwidbtype *hwidb, ushort index, ushort value)
{
    ushort data;

    /*
     * select and read the address filter location
     */
    (void)send_af_select_cmd(hwidb, index);
    (void)send_af_read_cmd(hwidb, &data);

    /*
     * OR in the value
     */
    data |= value;
	
    /*
     * select the address filter location again since the 
     * af_read command caused the address filter index on the 
     * ip to increment (af_read and af_write are auto-increment)
     * and write the modified value back to the address filter
     */
    (void)send_af_select_cmd(hwidb, index);
    (void)send_af_write_cmd(hwidb, data);
}

/*****************************************************************************
 * Name: cbus_af_and
 *
 * Description: AND a bit mask into an address filter location
 *
 * Input: 
 *   hwidb -- valid pointer to the hardware idb for the interface
 *   index -- the index of the address filter element to operate on 
 *   value -- the value to AND into the address filter element
 *
 * Return Value: None
 *
 *****************************************************************************/
void cbus_af_and (hwidbtype *hwidb, ushort index, ushort value)
{
    ushort data;

    /*
     * select and read the address filter location
     */
    (void)send_af_select_cmd(hwidb, index);
    (void)send_af_read_cmd(hwidb, &data);

    /*
     * AND in the value
     */
    data &= value;

    /*
     * select the address filter location again since the 
     * af_write command caused the address filter index on the 
     * ip to increment (af_read and af_write are auto-increment)
     * and write the modified value back to the address filter
     */
    (void)send_af_select_cmd(hwidb, index);
    (void)send_af_write_cmd(hwidb, data);
}


/*****************************************************************************
 * Name: cbus_af_write
 *
 * Description: write a value to an address filter location
 *
 * Input: 
 *   hwidb -- valid pointer to the hardware idb for the interface
 *   index -- the index of the address filter element to operate on 
 *   value -- the value to write to the address filter location
 *
 * Return Value: None
 *
 *****************************************************************************/
void cbus_af_write (hwidbtype *hwidb, ushort index, ushort value)
{
    (void)send_af_select_cmd(hwidb, index);
    (void)send_af_write_cmd(hwidb, value);
}

/*****************************************************************************
 * Name: cbus_af_read
 *
 * Description: read a value from an address filter location
 *
 * Input: 
 *   hwidb -- valid pointer to the hardware idb for the interface
 *   index -- the index of the address filter element to operate on 
 *
 * Return Value: 
 *   ushort -- the value read from the address filter location
 *
 *****************************************************************************/
ushort cbus_af_read (hwidbtype *hwidb, ushort index)
{
    ushort return_value;

    (void)send_af_select_cmd(hwidb, index);
    (void)send_af_read_cmd(hwidb, &return_value);
    return(return_value);
}

/*
 * interface_panic
 * When an interface processor panics, print out debugging information.
 * See also fci_daughter_panic(), which is used when we do not know the IDB.
 */

void interface_panic (hwidbtype *hwidb, const char *namestring)
{
    ushort type, vals[16], i;
    char *str;

    select_if(hwidb);
    type = send_errors_cmd(hwidb, 0x8000);
    while (type) {
	for (i = 0; i < 16; i++)
	    vals[i] = 0xffff;
	if (type & 1) {			/* bad rxeof */
	    type &= ~1;
	    str = "bad rxeof vec";
	    vals[0] = send_errors_cmd(hwidb, 0x8001);
	    vals[1] = send_errors_cmd(hwidb, 0x8002);
	    vals[2] = send_errors_cmd(hwidb, 0x8003);
	    vals[3] = send_errors_cmd(hwidb, 0x8004);
	} else if (type & 2) {		/* bad tx state */
	    type &= ~2;
	    str = "bad tx state";
	    vals[0] = send_errors_cmd(hwidb, 0x8005);
	} else if (type & 4) {		/* bad txEof */
	    type &= ~4;
	    str = "bad txEof vec";
	    vals[0] = send_errors_cmd(hwidb, 0x8006);
	    vals[1] = send_errors_cmd(hwidb, 0x8007);
	} else if (type & 8) {		/* tx_start/pend active */
	    type &= ~8;
	    str = "tx_start/pend active";
	    vals[0] = send_errors_cmd(hwidb, 0x8008);
	    vals[1] = send_errors_cmd(hwidb, 0x8009);
	    vals[2] = send_errors_cmd(hwidb, 0x800a);
	} else {
	    type = 0;
	    str = "unknown";
	}
	buginf("\n%s ustatus: %s (%04x), %04x %04x %04x %04x", namestring,
	       str, type, vals[0], vals[1], vals[2], vals[3]);
    }
}

#if defined(C7000) || defined(RSP)
#define AGS_C7K(ags,c7k)	c7k
#else
#define AGS_C7K(ags,c7k)	ags
#endif

/*
 * translate an fci_type (FCI_*_CONTROLLER) into chassis MIB info
 */
void card_type_and_descr (int fci_type, int if_count,
			  int* card_type, char** card_descr)
{
    switch (fci_type) {
      case FCI_ETHER_CONTROLLER:
	switch (if_count) {
	  case 2:
	    *card_type = AGS_C7K(D_cardType_csc_mec2, D_cardType_eip);
	    break;

	  case 4:
	    *card_type = AGS_C7K(D_cardType_csc_mec4, D_cardType_eip);
	    break;

	  case 6:
	    *card_type = AGS_C7K(D_cardType_csc_mec6, D_cardType_eip);
	    break;

	  default:
	    *card_type = D_cardType_unknown;
	    break;
	}
	*card_descr = "Ethernet";
	break;

      case FCI_FDDI_CONTROLLER:
	*card_type = D_cardType_csc_fci;
	*card_descr = "FDDI";
	break;

      case FCI_FDDIT_CONTROLLER:
	*card_type = AGS_C7K(D_cardType_csc_fcit, D_cardType_fip);
	*card_descr = "FDDI";
	break;

      case FCI_LINK_CONTROLLER:
	*card_type = AGS_C7K(D_cardType_csc_hsci, D_cardType_hip);
	*card_descr = "high-speed serial";
	break;

      case FCI_CTR_CONTROLLER:
	*card_type = AGS_C7K(D_cardType_csc_ctr, D_cardType_trip);
	*card_descr = "token ring";
	break;

      case FCI_FSIP_CONTROLLER:
	*card_type = D_cardType_fsip;
	*card_descr = "serial";
	break;

      case FCI_MIP_CONTROLLER:
	*card_type = D_cardType_mip;
	*card_descr = "Multi-Channel";
	break;

      case FCI_AIP_CONTROLLER:
	*card_type = D_cardType_aip;
	*card_descr = "ATM";
	break;

      case FCI_CIP_CONTROLLER:
	*card_type = D_cardType_cip;
	*card_descr = "IBM channel";
	break;

      case FCI_FEIP_CONTROLLER:
	*card_type = D_cardType_feip;
	*card_descr = "FastEthernet";
	break;

      case FCI_RVIP_CONTROLLER:
	*card_type = D_cardType_vip;
	*card_descr = "Versatile Interface Processor";
	break;

      case FCI_SVIP_CONTROLLER:
	*card_type = D_cardType_vip2;
	*card_descr = "Versatile Interface Processor";
	break;

      case FCI_RSP1_CONTROLLER:
	*card_type = D_cardType_rsp1;
	*card_descr = "R4600";
	break;

      case FCI_RSP2_CONTROLLER:
	*card_type = D_cardType_rsp2;
	*card_descr = "dual-cbus R4600";
	break;

      case FCI_RSP7000_CONTROLLER:
	*card_type = D_cardType_rsp7000;
	*card_descr = "R4600";
	break;

      case FCI_POSIP_CONTROLLER:
	*card_type = D_cardType_posip;
	*card_descr = "Packet Over Sonet";
	break;

      default:
	*card_type = D_cardType_unknown;
	*card_descr = "Unknown " AGS_C7K("Cisco", "Cx") "Bus card";
	break;
    }
    return;
}

/*
 * translate an fci_sub_type (FCI_TYPE_VIP_*) into chassis MIB info
 */
void
sub_card_type_and_descr (int fci_sub_type, int if_count,
                     int* card_type, char** card_descr) 
{ 
    switch (fci_sub_type) {
      case FCI_TYPE_VIP_FETHER:
	*card_type = D_cardType_pa_1fe;
	*card_descr = "FastEthernet Port Adapter";
        break;
      case FCI_TYPE_VIP_FETHER_TX_NISL:
	*card_type = D_cardType_pa_1fe_tx_nisl;
	*card_descr = "FastEthernet Port Adapter (TX-nISL)";
        break;
      case FCI_TYPE_VIP_FETHER_TX_ISL:
	*card_type = D_cardType_pa_1fe_tx_isl;
	*card_descr = "FastEthernet Port Adapter (TX-ISL)";
        break;
      case FCI_TYPE_VIP_FETHER_FX_ISL:
	*card_type = D_cardType_pa_1fe_fx_isl;
	*card_descr = "FastEthernet Port Adapter (FX-ISL)";
        break;
      case FCI_TYPE_VIP_ETHER8:
	*card_descr = "Ethernet Port Adapter";
	switch (if_count) {
	  case 4 : 
	    *card_type = D_cardType_pa_4e;
	    break;
	  case 5 : 
	    *card_type = D_cardType_pa_5e;
	    break;
	  case 8 :
	    *card_type = D_cardType_pa_8e;
	    break;
	  default:
            *card_type = D_cardType_unknown;
	    break;
	}
        break;
      case FCI_TYPE_VIP_SERIAL4:
	*card_type = D_cardType_pa_4t;
	*card_descr = "Serial Port Adapter";
        break;
      case FCI_TYPE_VIP_TRING4:
	*card_type = D_cardType_pa_4r;
	*card_descr = "Token Ring Port Adapter";
        break;
      case FCI_TYPE_VIP_FDDI:
        *card_type = D_cardType_pa_fddi;
        *card_descr = "FDDI Port Adapter";
        break;
      case FCI_TYPE_VIP_ESA:
        *card_type = D_cardType_sa_encryption;
        *card_descr = "Encryption/Decryption Service Adapter";
        break;
      default:
        *card_type = D_cardType_unknown;
        *card_descr = "Unknown Port Adapter";
        break;
    }
}

/*
 * Count an input error.  If we cross an error threshold, reset.
 * Treat MCI Ethernets specially
 */
void cbus_mci_input_error (hwidbtype *idb)
{
    idb->counters.input_error++;
    MAYBE_DROPPED(idb);
    if (++idb->error_count > idb->error_threshold) {
	if (idb->serial_pulseinterval && (idb->status & IDB_SERIAL)) {
	    idb->reset(idb);
	    return;
	}
	if (!idb->nokeepalive && !(idb->status & IDB_CBUS))
	    mci_input_error(idb);
    }
}

void
mark_hwidbs_nohardware (hwidbtype *first_hwidb)
{
    hwidbtype* hwidb;

    /*
     * Mark first_hwidb and ALL hwidbs hanging off
     * first_hwidb->next_idb_on_physint as NOHARDWARE:
     */
    if (oir_debug) {
	buginf("\nMarking %s nohardware, first_hwidb %#x",
	       first_hwidb->hw_namestring, first_hwidb);
	if (first_hwidb->fci_type & FCI_TYPE_BASE_FLAG)
	    buginf(" BASE HWIDB");
	bugtrace();
    }
    first_hwidb->status |= IDB_NOHARDWARE;

    hwidb = first_hwidb->next_idb_on_physint;
    while (hwidb) {
	if (oir_debug) {
	    buginf("\n Marking %s nohardware, hwidb %#x",
		   hwidb->hw_namestring, hwidb);
	    if (hwidb->fci_type & FCI_TYPE_BASE_FLAG)
		buginf(" BASE HWIDB");
	}
	hwidb->status |= IDB_NOHARDWARE;
	hwidb = hwidb->next_idb_on_physint;
    }
}




/*
 * mark_hwidbs_hardware_present
 * Return number of non base HWIDBs marked present
 */

int
mark_hwidbs_hardware_present (hwidbtype *first_hwidb)
{
    hwidbtype* hwidb;
    int count = 0;

    /*
     * Mark first_hwidb and ALL hwidbs hanging off
     * first_hwidb->next_idb_on_physint as present:
     */
    if (oir_debug) {
	buginf("\nMarking %s as hardware present, first_hwidb %#x",
	       first_hwidb->hw_namestring, first_hwidb);
	if (first_hwidb->fci_type & FCI_TYPE_BASE_FLAG)
	    buginf(" BASE HWIDB");
	bugtrace();
    }

    /*
     * Count non base IDBs only, and mark it present.
     * The RSP uses one base IDB per MIP port (plughole in hack lingo).
     */
    if ( (first_hwidb->fci_type & FCI_TYPE_BASE_FLAG) == 0)
	count++;
    first_hwidb->status &= ~IDB_NOHARDWARE;

    /*
     * Do the same for all non base HWIDBs chained off this IDB:
     */
    hwidb = first_hwidb->next_idb_on_physint;
    while (hwidb) {
	if (oir_debug) {
	    buginf("\n Marking %s as hardware present, hwidb %#x",
		   hwidb->hw_namestring, hwidb);
	    if (hwidb->fci_type & FCI_TYPE_BASE_FLAG)
		buginf(" BASE HWIDB");
	}

	if ( (hwidb->fci_type & FCI_TYPE_BASE_FLAG) == 0)
	    count++;
	hwidb->status &= ~IDB_NOHARDWARE;

	hwidb = hwidb->next_idb_on_physint;
    }
    return(count);
}

boolean ctrlr_uses_2bar_notation (int ctrlr_type)
{
    switch (ctrlr_type) {
    case FCI_RVIP_CONTROLLER:
    case FCI_SVIP_CONTROLLER:
	return TRUE;
    default:
	return FALSE;
    }
}
