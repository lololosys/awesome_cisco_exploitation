/* $Id: if_pas_dec21140.c,v 3.11.10.28 1996/08/28 13:06:16 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_dec21140.c,v $
 *------------------------------------------------------------------
 * if_pas_dec21140.c - DEC21140 100 Mbit Ethernet PCI Port Adaptor
 *		       (Platform independent support)
 *
 * February 1995, Steve Jacobson
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_dec21140.c,v $
 * Revision 3.11.10.28  1996/08/28  13:06:16  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.10.27  1996/08/26  18:23:44  feisal
 * CSCdi45144:  4E/1FE error handling cleanup
 * Branch: California_branch
 *
 * Revision 3.11.10.26  1996/08/20  18:20:46  tboyle
 * CSCdi64994:  vip1 FE output stuck, lost packets, bad dual full-duplex
 * performance
 * Branch: California_branch
 *
 * Revision 3.11.10.25  1996/07/11  19:10:10  dcarroll
 * CSCdi61747:  PA insertion time not reset after OIR
 * Branch: California_branch
 * Reset insertion time on OIR removal.
 *
 * Revision 3.11.10.24  1996/07/10  02:12:50  feisal
 * CSCdi55145:  Fast-Ethernet reporting NO-CARRIER during initialization
 * Branch: California_branch
 *
 * Revision 3.11.10.23  1996/06/21  21:24:43  mbeesley
 * CSCdi59509:  FastEther init code can loop forever
 * Branch: California_branch
 *
 * Revision 3.11.10.22  1996/06/02  01:35:40  feisal
 * CSCdi55143:  Fast-Ethernet cable removal is not detected
 * Branch: California_branch
 *
 * Revision 3.11.10.21  1996/05/31  22:33:40  feisal
 * CSCdi56695:  ISL Broken on certain card
 * Branch: California_branch
 *
 * Revision 3.11.10.20  1996/05/24  20:32:49  ssangiah
 * CSCdi58375:  Need to add ISL support for new platform.
 * Add ISL support for the new c7200 platform.
 * Branch: California_branch
 *
 * Revision 3.11.10.19  1996/05/13  22:53:01  mbeesley
 * CSCdi57397:  DEC21140 interrupt setting incorrect
 * Branch: California_branch
 *
 * Revision 3.11.10.18  1996/05/13  20:26:31  dcarroll
 * CSCdi56363:  OIR-insertion of 4E/8E/4T PAs should  bring-up the ports
 * in shut
 * Branch: California_branch
 * OIR'd adapters must stay shutdown through initializatio
 *
 * Revision 3.11.10.17  1996/05/08  06:09:29  feisal
 * CSCdi45137:  No line down event when 4E/FE removed the first time.
 * Branch: California_branch
 *
 * Revision 3.11.10.16  1996/05/07  23:48:12  mbeesley
 * CSCdi56954:  DEC21140 show controller not correct for IO card interface
 * Branch: California_branch
 *
 * Revision 3.11.10.15  1996/05/07  19:24:04  mbeesley
 * CSCdi56937:  Update DEC21140 errmsgs to include slot/subunit
 * Branch: California_branch
 *
 * Revision 3.11.10.14  1996/05/07  00:23:58  mbeesley
 * CSCdi56157:  Enable IP Flow/Optimum on Ethernets
 * Branch: California_branch
 *
 * Revision 3.11.10.13  1996/05/01  13:28:10  dcarroll
 * CSCdi54895:  oir fix for ethernet port adapter
 * Branch: California_branch
 * OIR support for pas ethernets.
 *
 * Revision 3.11.10.12  1996/04/29  23:44:05  mbeesley
 * CSCdi55449:  1fe PA driver needs to clean-up after OIR
 * Branch: California_branch
 *
 * Revision 3.11.10.11  1996/04/29  17:57:18  ssangiah
 * CSCdi54472: Inadequate information in the output of "sh controllers"
 *             command
 *   o Add a "show diag" command for the c7200 platform.
 *   o Make the output from "show controllers" consistent.
 * Branch: California_branch
 *
 * Revision 3.11.10.10  1996/04/21  00:51:20  getchell
 * CSCdi51219:  Input ignores on FE PAs not correct under high input load
 * Branch: California_branch
 *
 * Revision 3.11.10.9  1996/04/08  19:00:52  dcarroll
 * CSCdi53796:  Remove extra idb type assignments
 * Branch: California_branch
 * Remove the unneeded early setting of idb type.
 *
 * Revision 3.11.10.8  1996/04/05  19:56:13  dcarroll
 * CSCdi53769:  Return bool from pas init interface routines
 * Branch: California_branch
 * Pass boolean up the analyze chain, not idbs.
 *
 * Revision 3.11.10.7  1996/04/05  05:32:13  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.11.10.6  1996/04/05  04:17:35  sjacobso
 * CSCdi52859:  New AF and new ISL code not fully integrated
 * Branch: California_branch
 * Cause dec21140 address filter write code to set the correct
 * ISL enable flags on receipt of an ISL entry.
 *
 * Revision 3.11.10.5  1996/04/04  20:04:13  getchell
 * CSCdi50943:  SVIP PA reset broken
 * Branch: California_branch
 *
 * Revision 3.11.10.4  1996/03/29  04:05:37  sjacobso
 * CSCdi52266:  4E/8E receive promiscuously, hurting performance
 * Branch: California_branch
 * Change write_address_filter code to examine address filter
 * entries for important status, and to store that status in the
 * software address filter.  The correct common routines for
 * adding MAC addresses are called.  Also, fix Predator logical
 * address filter code to work correctly with HSRP addresses.
 *
 * Revision 3.11.10.3  1996/03/29  00:45:15  sjacobso
 * CSCdi45152:  PERFORMANCE: 1FE runs in promiscuous mode instead of
 * filtering
 * Branch: California_branch
 * Add code to allow dec21140 chip to run in perfect
 * address filtering mode whenever possible.  Modify
 * address filter write code to record information
 * about address filter entries that may affect perfect
 * filter operation.  Interface VIP-specific software
 * address filter to RVIP/SVIP/Predator common routines.
 *
 * Revision 3.11.10.2  1996/03/21  23:23:22  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.11.10.1  1996/03/18  21:34:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.16  1996/03/18  23:28:48  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.15  1996/03/07  00:19:14  mbeesley
 * CSCdi50649:  DEC21140 in Store-and-Forward mode
 *         o Put device in tx cut-thru, 512 byte tx fifo threshold
 *         o Increase rx pool, eliminate use of fallback pool
 *         o Handle rx fifo overrun and error interrupt correctly
 * Branch: ELC_branch
 *
 * Revision 3.3.2.14  1996/03/03  08:40:17  wfried
 * CSCdi50555:  Fast Ether multicast filtering broken and unthrottling not
 * working
 * Branch: ELC_branch
 * Avoid multiple setting of single multicast address in hw addr filter,
 * register pool cache throttling callback function.
 *
 * Revision 3.3.2.13  1996/02/27  02:24:03  wfried
 * CSCdi49502:  1fe device driver has very long init delay
 * Branch: ELC_branch
 * Remove unnecessary initialization delay.
 *
 * Revision 3.3.2.12  1996/02/15  19:48:43  dcarroll
 * CSCdi49080:  Add slots support to pas drivers
 * Branch: ELC_branch
 * Add missing slots initialization to pas drives,
 * for oir and snmp support.
 *
 * Revision 3.3.2.11  1996/02/10  00:30:44  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3.2.10  1996/02/08  08:47:53  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.9  1996/01/29  19:04:29  wfried
 * Branch: ELC_branch
 * Use HW address filtering, disable rx side for better input throttling,
 * add loopback code, correct ignored frames counting, allow only MII media
 * on the Predator I/O card FE interface.
 *
 * Revision 3.3.2.8  1996/01/18  05:41:57  mbeesley
 * Branch: ELC_branch
 * Optimize 1fe and 4e driver. Fix 4e fastsend bug. Patch in Tx enable
 * fix for 1fe. Debug IP fastswitching.
 *
 * Revision 3.3.2.7  1996/01/16  01:42:52  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.6  1995/12/12  04:53:59  mbeesley
 * Branch: ELC_branch
 * Allow "show pci bridge" to display all bridges. Change the 1fe GPC
 * register settings for Predator IO card 1fe. Fix some amdp2 error
 * messages. And turn power off on unrecognized port adaptors.
 *
 * Revision 3.3.2.5  1995/12/11  19:13:48  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.3.2.4  1995/12/05  06:36:01  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.3.2.3  1995/12/04  22:18:26  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.3.2.2  1995/11/30  02:05:21  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.3.2.1  1995/11/22  21:41:55  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.11  1996/02/23  20:02:15  feisal
 * CSCdi45142:  Support VLAN ISL for new 1FE port adapter
 *
 * Revision 3.10  1996/01/21  22:31:41  feisal
 * CSCdi45141:  Need to support 1FE replacement port adapter
 *
 * Revision 3.9  1996/01/13  00:34:33  feisal
 * CSCdi45725:  1FE fails to transmit, found in DVT
 *
 * Revision 3.8  1995/12/19  20:24:26  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.7  1995/12/17  18:35:55  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.6  1995/12/06  18:31:40  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.5  1995/12/01  02:00:02  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.4  1995/11/28  02:26:10  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.3  1995/11/17  18:54:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:21:11  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:57:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "../../boot/cpu.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "subsys.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/network.h"
#include "../if/ether.h"
#include "../if/ether_private.h"
#include "../if/ether_inline.h"
#include "../if/pci.h"
#include "../if/linkdown_event.h"
#include "../ip/ip.h"

#include "../hes/ccb_cmd.h"
#include "../hes/if_fci.h"

#include "../smf/smf_public.h"

#include "../src-vip/vip_isl.h"

#include "if_pas.h"
#include "if_pas_dec21140.h"
#include "msg_dec21140.c"		/* Not a typo, see logger.h */
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "pas_id.h"

/*
 * dec21140_sanitize_rxpak:
 * Sanitize the rx paktype used by the driver
 */
static void dec21140_sanitize_rxpak (dec21140_instance_t *ds)
{
    paktype *rx_pak = ds->rx_pak;

    if (rx_pak) {
	datagram_done(rx_pak);
    }

    /*
     * Clean up all relevant entries in rx_pak
     */
    rx_pak->datagramsize = 0;
    rx_pak->datagramstart = NULL;
}

/*
 * dec21140_set_rx_discard :
 * Control ds->rx_discard and flush rx_pak appropriately.
 */
void dec21140_set_rx_discard (dec21140_instance_t *ds, boolean state)
{
    if (!state) {
	/*
	 * Clearing rx_discard. Sanitize rx_pak.
	 */
	dec21140_sanitize_rxpak(ds);
    }
    ds->rx_discard = state;
}

/*
 * dec21140_throttle:
 * Throttle the DEC21140 interface
 *
 * The interface is throttled by stopping the Receive side.
 * Whenever the reason for throttling is the lack of receive
 * minibuffers, the pool threshold callback function is responsible
 * for re-enabling the interface, so that it doesn't have to
 * wait for the background process to wake it up after 10 seconds.
 */
void dec21140_throttle (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    leveltype level;

    /*
     * Mark the interface as throttled
     */
    idb_mark_throttled(idb);
    ds->throttled += 1;

    /*
     * Stop the Rx process
     */
    level = raise_interrupt_level(NETS_DISABLE);
    ds->dec21140_regaddr->csr6 &= ~DEC21140_CSR6_RECEIVE_START;
    reset_interrupt_level(level);
}

/*
 * dec21140_stop:
 * Stop the DEC21140 ethernet controller
 */
static void dec21140_stop (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    /* 
     * Stop the Rx and Tx sides of the chip and wait a while
     */
    ds->dec21140_regaddr->csr6 = DEC21140_CSR6_RECEIVE_STOP |
	DEC21140_CSR6_TRANSMIT_STOP;
    DELAY(DEC21140_STOP_DELAY);

    /*
     * Clear all pending interrupts
     */
    ds->dec21140_regaddr->csr5 = DEC21140_CSR5_CLEAR_ALL_INTERRUPTS;

    /*
     * Disable all interrupts
     */
    ds->dec21140_regaddr->csr7 = 0;

    /*
     * Wait a little longer and return
     */
    DELAY(DEC21140_STOP_DELAY);
    reset_interrupt_level(level);
}

/*
 * dec21140_shutdown:
 * Shutdown a DEC21140 interface
 */
static void dec21140_shutdown (hwidbtype *idb, int newstate)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    leveltype level;

    net_cstate(idb, newstate);
    level = raise_interrupt_level(NETS_DISABLE);
    if (ds) {
	dec21140_stop(idb);
	(*idb->reset)(idb);
    }
    reset_interrupt_level(level);
}

/*
 * dec21140_linestate:
 * Return what the driver thinks is the carrier state of this ethernet
 */
static boolean dec21140_linestate (hwidbtype *idb)
{
    return (idb->ethernet_lineup);
}

/*
 * dec21140_reset:
 * Reset the DEC21140 chip
 */
static void dec21140_reset (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    dec21140_regs_t *reg;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    reg = ds->dec21140_regaddr;

    /*
     * Put the chip into the MII/SYM mode before issuing a software reset.
     *
     * Warning: whenever the mode of the chip is changed, a software reset
     * has to be issued because of a hardware quirk/bug. Do not change this
     * unless you are sure this equivalent sequence will be performed.
     */
    reg->csr6 |= DEC21140_CSR6_PORT_SELECT_MII_SYM;

    /*
     * Put the chip into reset
     */
    reg->csr0 |= DEC21140_CSR0_SOFTWARE_RESET_MASK;
    usecdelay(DEC21140_RESET_DELAY);

    /*
     * And take it out of reset
     */
    reg->csr0 &= ~DEC21140_CSR0_SOFTWARE_RESET_MASK;
    usecdelay(DEC21140_RESET_DELAY);

    reset_interrupt_level(level);
}
    
/*
 * dec21140_restart:
 * Wrapper routine for idb->reset vector
 */
static void dec21140_restart (hwidbtype *idb)
{
    leveltype level;

    /*
     * Raise the interrupt level, flush the i/o holq queues
     * and re-initialize the device.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    holdq_reset(idb);
    if (idb->state != IDBS_ADMINDOWN) {
	dec21140_init(idb, FALSE);
    }
    reset_interrupt_level(level);
}

/*
 * dec21140_show_regs:
 * Print out the DEC21140 registers.
 */
static void dec21140_show_regs (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    uint csr0, csr3, csr4, csr5, csr6, csr7;
    uint csr8, csr9, csr11, csr12, csr15;
    dec21140_regs_t *reg;
    leveltype level;

    if (!ds || !ds->dec21140_regaddr) {
	return;
    }

    /*
     * Raise the interrupt level and capture the chips registers
     */
    reg = ds->dec21140_regaddr;
    level = raise_interrupt_level(NETS_DISABLE);

    csr0 = reg->csr0;
    csr3 = reg->csr3;
    csr4 = reg->csr4;
    csr5 = reg->csr5;
    csr6 = reg->csr6;
    csr7 = reg->csr7;
    csr8 = reg->csr8;
    csr9 = reg->csr9;
    csr11 = reg->csr11;
    csr12 = reg->csr12;
    csr15 = reg->csr15;

    /*
     * Update software counters based on hardware counters
     * that clear on read
     */
    idb->counters.input_resource += (csr8 & DEC21140_CSR8_MISSED_FRAME_MASK);

    /*
     * Restore interrupt level
     */
    reset_interrupt_level(level);

    /*
     * Print out the registers
     */
    printf(" CSR0=%#x, CSR3=%#x, CSR4=%#x, CSR5=%#x\n",
	   csr0, csr3, csr4, csr5);
    printf(" CSR6=%#x, CSR7=%#x, CSR8=%#x, CSR9=%#x\n",
	   csr6, csr7, csr8, csr9);
    printf(" CSR11=%#x, CSR12=%#x, CSR15=%#x\n",
	   csr11, csr12, csr15);
}

/*
 * dec21140_print_pci_regs:
 * Print out the PCI registers for a DEC21140 interface
 */
static void dec21140_print_pci_regs (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    uint cfid, cfcs, cfrv, cflt, cbio, cbma, cfit, cfda, pa_bay;
    uchar bus_no;

    /*
     * Get bus number and device number
     */
    pa_bay = ds->pacb.pa_bay;
    bus_no = get_pci_bus_no(pa_bay);

    printf(" DEC21140 PCI registers:\n");
    printf("  bus_no=%d, device_no=%d\n", bus_no, ds->pci_device_number);
    cfid = pci_config_read(bus_no, ds->pci_device_number, 0, 
			   DEC21140_PCI_CFID_REG_OFFSET );
    cfcs = pci_config_read(bus_no, ds->pci_device_number, 0, 
			   DEC21140_PCI_CFCS_REG_OFFSET);
    cfrv = pci_config_read(bus_no, ds->pci_device_number, 0, 
			   DEC21140_PCI_CFRV_REG_OFFSET);
    cflt = pci_config_read(bus_no, ds->pci_device_number, 0, 
			   DEC21140_PCI_CFLT_REG_OFFSET);
    cbio = pci_config_read(bus_no, ds->pci_device_number, 0, 
			   DEC21140_PCI_CBIO_REG_OFFSET);
    cbma = pci_config_read(bus_no, ds->pci_device_number, 0, 
			   DEC21140_PCI_CBMA_REG_OFFSET);
    cfit = pci_config_read(bus_no, ds->pci_device_number, 0, 
			   DEC21140_PCI_CFIT_REG_OFFSET);
    cfda = pci_config_read(bus_no, ds->pci_device_number, 0, 
			   DEC21140_PCI_CFDA_REG_OFFSET);

    /*
     * And finally print it all out
     */
    printf("  CFID=0x%08x, CFCS=0x%08x, CFRV=0x%08x, CFLT=0x%08x\n",
	   cfid, cfcs, cfrv, cflt);
    printf("  CBIO=0x%08x, CBMA=0x%08x, CFIT=0x%08x, CFDA=0x%08x\n",
	   cbio, cbma, cfit, cfda);
}

/*
 * dec21140_read_mii_bits:  
 * Read from 0 to 32 bits (inclusive) from an MII PHY through 
 * the Management Data I/O line.
 *
 * During the data phase, The PHY places valid data on MDIO within 300 ns
 * of the rising edge of MDC.  Data remains valid until the next rising
 * edge of MDC.
 */
static uint dec21140_read_mii_bits (volatile ulong *csr9_ptr, int num_bits)
{
    int i;
    ulong rx_bits = 0;

    /* 
     * Send bits to the MII, leftmost bit first 
     */
    for (i=0; i < num_bits; i++) {

        /* Read command, clock 0 */
        *csr9_ptr = (DEC21140_CSR9_MII_READ|DEC21140_CSR9_READ);
	usecdelay(10);
 
        /* Read command, clock 1 */
        *csr9_ptr = (DEC21140_CSR9_MII_READ|DEC21140_CSR9_READ|
		     DEC21140_CSR9_MDC_CLOCK);
	usecdelay(10);

	/*
	 * Read back the bits
	 */
        rx_bits = rx_bits << 1;
        rx_bits |= (((*csr9_ptr) >> DEC21140_CSR9_RX_BIT_SHIFT) & 0x1);
    }
    return(rx_bits);
}

/*
 * dec21140_write_mii_bits:  
 * Write from 0 to 32 bits (inclusive) to an MII PHY through the 
 * Management Data I/O line
 */
static void dec21140_write_mii_bits (volatile ulong *csr9_ptr, 
				     uint bit_pattern, uint num_bits)
{
    uint i;
    uint tx_bit;
 
    /* 
     * Send bits to the MII, leftmost bit first 
     */
    for (i=0; i < num_bits; i++) {

        tx_bit = (bit_pattern >> (num_bits - i - 1)) & 0x1;
 
        /* Write tx_bit, clock 0 */
        *csr9_ptr = (tx_bit << DEC21140_CSR9_TX_BIT_SHIFT) 
	    | DEC21140_CSR9_WRITE;
	usecdelay(10);

        /* Write tx_bit, clock 1 */
        *csr9_ptr = (tx_bit << DEC21140_CSR9_TX_BIT_SHIFT)
	    | (DEC21140_CSR9_WRITE|DEC21140_CSR9_MDC_CLOCK);
	usecdelay(10);
    }
}

/*
 * dec21140_read_mii_reg:  
 * Read a specific MII register from the specified PHY through a DEC21140 chip.
 * 
 * This code provides only one turnaround cycle; the MII spec requires two.
 * The National DP83840 MII PHY drives the first bit of data in the second
 * cycle.  This algorithm may not work correctly with other vendor's PHYs.
 */
 
static ushort dec21140_read_mii_reg (dec21140_regs_t *reg, 
				     uint phy_addr, uint reg_addr)
{
    ushort mii_data;

    if (!reg) {
        return(0);
    }
 
    /*
     * Write preamble, 32 bits of 1
     * Some MII devices may want a longer preamble.  The DP83840 works
     * with 32 bits, which is the standard.
     */
    dec21140_write_mii_bits(&reg->csr9, 0xFFFFFFFF, 32);
 
    /*
     * Write start of frame, two bits, 01
     */
    dec21140_write_mii_bits(&reg->csr9, 0x1, 2);
 
    /*
     * Write op code, two bits, 10 (read)
     */
    dec21140_write_mii_bits(&reg->csr9, 0x2, 2);
 
    /*
     * Write PHY address, 5 bits.  Which PHY we want to access on the MII bus.
     * The first PHY to come up will be PHY 0.
     */
    dec21140_write_mii_bits(&reg->csr9, phy_addr, 5);
 
    /*
     * Write register address, 5 bits.  Which register we want to read
     * on the PHY.
     */
    dec21140_write_mii_bits(&reg->csr9, reg_addr, 5);
 
    /*
     * Turnaround, two bit times of waiting.  In the first cycle, MDIO
     * is high Z.  In the second, the PHY should drive 0.
     *
     * The National DP83840 MII PHY appears to only want one turnaround
     * cycle.  In the second cycle, it drives the first bit of data.
     * This appears to violate the standard, and may not be compatible
     * with other PHY manufacturers.
     */
    dec21140_read_mii_bits(&reg->csr9, 1);
 
    /*
     * Read 16 bits of data.  The PHY places valid data on MDIO within
     * 300 ns of the rising edge of MDC.  Data remains valid until the
     * next rising edge of MDC.
     */
    mii_data = dec21140_read_mii_bits(&reg->csr9, 16);
 
    /*
     * Clean up the bus.  The PHY will hold the last data bit on MDIO
     * until the next rising edge of MDC.
     *
     * This call will generate an unnecessary read of csr9; it will
     * flush out the other writes (possibly good) and waste a microsecond
     * or two doing it (probably bad).  Perhaps a routine that clocks
     * csr9 without reading it would be better.
     */
    dec21140_read_mii_bits(&reg->csr9, 1);
 
    return(mii_data);
}
 
/*
 * dec21140_write_mii_reg:  
 * Write a specific MII register on the specified PHY through a DEC21140 chip
 */
static void dec21140_write_mii_reg (dec21140_regs_t *reg, 
				    uint phy_addr, uint reg_addr, 
				    ushort mii_data)
{
    if (!reg) {
        return;
    }
 
    /*
     * Write preamble, 32 bits of 1
     *
     * Some MII devices may want a longer preamble.  The DP83840 works
     * well with 32 bits, which is the standard.
     */
    dec21140_write_mii_bits(&reg->csr9, 0xFFFFFFFF, 32);
 
    /*
     * Write start of frame, two bits, 01
     */
    dec21140_write_mii_bits(&reg->csr9, 0x1, 2);
 
    /*
     * Write op code, two bits, 01 (write)
     */
    dec21140_write_mii_bits(&reg->csr9, 0x1, 2);
 
    /*
     * Write PHY address, 5 bits.  Which PHY we want to access on the MII bus.
     * The first PHY to come up will be PHY 0.
     */
    dec21140_write_mii_bits(&reg->csr9, phy_addr, 5);
 
    /*
     * Write register address, 5 bits.  Which register we want to read
     * on the PHY.
     */
    dec21140_write_mii_bits(&reg->csr9, reg_addr, 5);
 
    /*
     * Turnaround, two bit times of waiting.  In the first cycle, MDIO
     * is driven to 1.  In the second, it is driven to 0.
     */
    dec21140_write_mii_bits(&reg->csr9, 0x2, 2);
 
    /*
     * Write 16 bits of data.  The STA must place valid data on MDIO within
     * 10 ns of the rising edge of MDC, and hold it for 10 ns after the
     * rising edge.  This code provides much longer setup and hold times
     * which is good, as the DP83840 can't meet the spec.
     */
    dec21140_write_mii_bits(&reg->csr9, mii_data, 16);
 
    /*
     * Clean up the bus.  Tri-state the MDIO line.  This code generates
     * an extra MDC clock, which may not be a good idea.
     *
     * This call will generate an unnecessary read of csr9; it will
     * flush out the other writes (possibly good) and waste a microsecond
     * or two doing it (probably bad).  Perhaps a routine that changes
     * csr9 without changing the MDIO or reading csr9 would be better.
     */
    dec21140_read_mii_bits(&reg->csr9, 1);
}

/*
 * dec21140_get_chip_version:
 * find out which dec21140 is it.
 */
static void dec21140_get_chip_version (hwidbtype *idb)
{
	dec21140_instance_t *ds = INSTANCE(idb);
	uint cfrv, pa_bay;
	uchar bus_no;

	/*
	 * Get bus number and device number
	 */
	pa_bay = ds->pacb.pa_bay;
	bus_no = get_pci_bus_no(pa_bay);

	cfrv = pci_config_read(bus_no, ds->pci_device_number, 0, 
						DEC21140_PCI_CFRV_REG_OFFSET);
	ds->is_DEC21140A = ((cfrv & DEC21140_REV_MASK) == DEC21140_IS_A_21140A);
}

/*
 * dec21140_print_mii_regs:  
 * Print the readable registers from a DP83840 MII PHY chip, 
 * read through a DEC21140 chip
 */
static void dec21140_print_mii_regs (dec21140_instance_t *ds)
{
    dec21140_regs_t *reg;
    uint reg_addr;

    if (!ds || !ds->dec21140_regaddr) {
	return;
    }
    reg = ds->dec21140_regaddr;
 
    printf(" MII registers:");
 
    /* 
     * There are 32 MII registers.  Print all of them, except registers
     * 0x14 and 0x1D - 0x1F; National claims these registers shouldn't
     * be read on the DP83840. 
     */
    for (reg_addr=0; reg_addr < 0x1D; reg_addr++) {
 
        if ((reg_addr % 8) == 0) {
            printf("\n  Register 0x%-02x: ", reg_addr);
        }
        if (reg_addr != 0x14) {
            printf("  %-04x", dec21140_read_mii_reg(reg, 0, reg_addr));
        } else {
            printf("      ");
        }
    }
    printf("\n");
}

/*
 * dec21140_hwaf_print:
 * Print the content of the hardware address filter
 */
static void dec21140_hwaf_print (dec21140_instance_t *ds)
{
    dec21140_hwaf_t *hwaf_ptr;
    dec21140_regs_t *reg;
    leveltype level;
    uint indx;
 
    /*
     * Check parameters
     */
    if (!ds || !ds->hwaf_buf_ptr)
	return;
  
    /*
     * Malloc some memory to create a copy of hardware AF structure.
     */
    hwaf_ptr = malloc(sizeof(dec21140_perf_hwaf_t));
    if (!hwaf_ptr)
        return;

    /*
     * Print banner
     */
    printf(" HW addr filter: %#x, ISL %s", ds->hwaf_buf_ptr,
	   (ds->pacb.isl_enabled ? "Enabled" : "Disabled"));
    reg = ds->dec21140_regaddr;
    if (reg->csr6 & DEC21140_CSR6_PROMISCUOUS_MODE)
	printf("\n  Promiscuous mode enabled");

    /*
     * Lock out interrupts and copy hardware address filter
     */
    level = raise_interrupt_level(NETS_DISABLE);
    bcopy(ds->hwaf_buf_ptr, hwaf_ptr, sizeof(dec21140_perf_hwaf_t));
    reset_interrupt_level(level);

    /*
     * Print out hardware address filter
     */
    printf("\n");
    for (indx=0; indx < DEC21140_PERF_HWAF_NUM_ENTRIES; indx++) {
	printf("  Entry=%-2d:  Addr=%-04x.%-04x.%-04x\n", indx,
	    hwaf_ptr->addr[indx].w[0].word, hwaf_ptr->addr[indx].w[1].word,
	    hwaf_ptr->addr[indx].w[2].word);
    }
    free(hwaf_ptr);
}

/*
 * dec21140_show_controller:
 * Print out info about this DEC21140 interface. Return TRUE if the output
 * terminated abnormally.
 */
static boolean dec21140_show_controller (hwidbtype *idb, parseinfo *csb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    dec21140_instance_t *ds_snap;
    leveltype level;

    printf("\nInterface %s\n", idb->hw_namestring);
    printf("Hardware is %s\n", idb->typestring);
    if (!ds || !ds->ib_malloc) {
	printf("Not initialized\n");
	return(FALSE);
    }

    /*
     * Take a snapshot of the rx ring, rx shadow ring, tx ring, tx shadow
     */
    ds_snap = malloc(sizeof(dec21140_instance_t));
    if (!ds_snap) {
	return(TRUE);
    }

    /*
     * Lock out interrupts and copy the data structures. Then re-enable
     * network interrupts.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    bcopy(ds, ds_snap, sizeof(dec21140_instance_t));
    reset_interrupt_level(level);

    /*
     * Print out the addresses of some stuff
     */
    printf(" dec21140_ds=%#x, registers=%#x, ib=%#x\n",
	   ds, ds->dec21140_regaddr, ds->ib);
    printf(" rx ring entries=%d, tx ring entries=%d\n",
	   DEC21140_RX_RING_ENTRIES, DEC21140_TX_RING_ENTRIES);
    printf(" rxring=%#x, rxr shadow=%#x, rx_head=%d, rx_tail=%d\n",
	   ds_snap->rxr, ds_snap->rxr_shadow, ds_snap->rx_head, 
	   ds_snap->rx_tail);
    printf(" txring=%#x, txr shadow=%#x, tx_head=%d, tx_tail=%d, "
	   "tx_count=%d\n",
	   ds_snap->txr, ds_snap->txr_shadow, ds_snap->tx_head, 
	   ds_snap->tx_tail, ds_snap->tx_count);
    printf(" PHY link %s\n", (ds->phy_link_active ? "up" : "down"));

    /*
     * Print the chips registers
     */
    dec21140_show_regs(idb);

    /*
     * Print out pci registers
     */
    dec21140_print_pci_regs(idb);

    /*
     * Print out MII registers
     */
    dec21140_print_mii_regs(ds);

    /*
     * Display the instance structure
     */
    dec21140_show_instance(ds_snap);

    /*
     * Print hardware address filter entries
     */
    dec21140_hwaf_print(ds_snap);

    /*
     * Release all memory
     */
    free(ds_snap);
    return(FALSE);
}    

/*
 * dec21140_hwaf_clear
 *
 * Clear the HW Address Filter frame structure to its reset condition.
 * Since all entries are read by the hardware, a default address has
 * to be set (broadcast).
 */
void dec21140_hwaf_clear (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    dec21140_hwaf_t *hwaf_ptr;
    uint indx, jndx;

    /*
     * Clear all filter entries (preset them to the broadcast addr)
     */
    hwaf_ptr = ds->hwaf_buf_ptr;
    for (indx=0; indx < DEC21140_PERF_HWAF_NUM_ENTRIES; indx++) {
	for (jndx=0; jndx<3; jndx++) {
	    hwaf_ptr->addr[indx].w[jndx].word = 0xFFFF;
	}
    }

    /*
     * Reset flag that indicates that all hw addr entries have been used
     */
    ds->addr_table_overflow = FALSE;

    /*
     * Reset flag that indicates that we're in pass all multicast packet mode
     */
    ds->rx_all_multicast = FALSE;
}

/*
 * dec21140_hwaf_refresh
 * Send the updated address filter (previously updated) to the
 * DEC chip. The receive side has to be stopped the first time
 * this function is called (to set the local MAC address)
 *
 * MUST BE CALLED WITH NETS INTERRUPTS DISABLED
 */
static void dec21140_hwaf_refresh (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    particletype *mr1, *mr2;
    dec21140_tx_desc_t *tmd;
    dec21140_tx_shadow_t *tmds;

    /*
     * Make sure there is room to send the address setup frame
     */
    if (ds->tx_count >= DEC21140_TX_RING_ENTRIES-1) {
        /*
         * We need two descriptors. Since there is no space, lets
         * reset the interface which will allocate space and send
         * the updated setup frame.
         */
        dec21140_init(idb, FALSE);
        return;
    }

    /*
     * Make sure we can get two particles to copy the
     * address setup frame and that it is the right size.
     */
    if (!(mr1 = dec21140_get_particle(idb))) {
        dec21140_init(idb, FALSE);
        return;
    }
    mr1->data_bytes = 0;
    if (!(mr2 = dec21140_get_particle(idb))) {
        retparticle(mr1);
        dec21140_init(idb, FALSE);
        return;
    }
    mr2->data_bytes = 192;
    bcopy(ds->hwaf_buf_ptr, mr2->data_start, 192);

    /*
     * Set up first empty descriptor.
     * This descriptor will not have any resources
     * (particle or pak) associated to it.
     */
    tmd = &ds->txr[ds->tx_tail];
    tmds = &ds->txr_shadow[ds->tx_tail];
    tmds->mp = mr1;
    tmds->pak = NULL; /* No pak associated to this particle */
    if (tmd->tx_status & DEC21140_TMD_OWN) {
	dec21140_init(idb, FALSE);
	return;
    }
    tmd->control_length = 0;

    /* 
     * workaround bug where SETUP frames ignore RING CHAINS
     */
    if (((ds->tx_tail+1) & DEC21140_TX_RING_INDEX_MASK) == 0) {
	tmd->control_length = tmd->control_length |
	    DEC21140_TMD_TX_END_OF_RING;
    }
    tmd->tx_status = DEC21140_TMD_OWN;
    DEC21140_ADVANCE_TX_RING_INDEX(ds->tx_tail);
    ds->tx_count += 1;

    /*
     * Prepare setup frame descriptor
     */
    tmd = &ds->txr[ds->tx_tail];
    tmds = &ds->txr_shadow[ds->tx_tail];
    tmds->mp = mr2;
    tmds->pak = NULL; /* No pak associated to this particle */
    if (tmd->tx_status & DEC21140_TMD_OWN) {
	dec21140_init(idb, FALSE);
	return;
    }
    tmd->buf_addr = DEC21140_BUF_ADDR(mr2->data_start);
    tmd->control_length = (DEC21140_TMD_COMPLETION_INT |
                           DEC21140_TMD_SETUP_PACKET |
                           DEC21140_TMD_FT_PERFECT) | 192;

    /*
     * workaround bug where SETUP frames ignore RING CHAINS
     */
    if (((ds->tx_tail+1) & DEC21140_TX_RING_INDEX_MASK) == 0) {
	tmd->control_length = tmd->control_length |
	    DEC21140_TMD_TX_END_OF_RING;
    }
    tmd->tx_status = DEC21140_TMD_OWN;
    DEC21140_ADVANCE_TX_RING_INDEX(ds->tx_tail);
    ds->tx_count += 1;
}

/*
 * dec21140_set_hw_address:
 * Program local unicast MAC address.
 *
 * The chip allows no more than 16 addresses to be programmed while in
 * perfect filtering mode.  The following table illustrates how those
 * 16 entries will be allocated:
 *
 * Entry: 0-13	any addresses
 *        14	broadcast address
 *        15	local unicast address
 */
void dec21140_set_hw_address (hwidbtype *idb, uchar *mac)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Update HW address filter structure with given unicast address.
     */
    ds->hwaf_buf_ptr->station_uni_addr.w[0].word = *((ushort *)mac);
    ds->hwaf_buf_ptr->station_uni_addr.w[1].word = *((ushort *)mac + 1);
    ds->hwaf_buf_ptr->station_uni_addr.w[2].word = *((ushort *)mac + 2);
    dec21140_hwaf_refresh(idb);

    reset_interrupt_level(level);
}

/*
 * dec21140_add_hw_address:
 * Add a multicast/unicast ethernet hw address to the logical filter.
 * 
 * Note that the HSRP unicast address is also passed down to the driver
 * through this vector.  The code differentiates by examining the multicast
 * bit of the MAC address.
 *
 * There is a bug in the DEC21140 chip which prevents imperfect filter
 * mode from being used.  The perfect address filter is used instead;
 * that filter can be programmed with no more than 16 MAC addresses.
 * Entries are reserved for the station unicast address, broadcast address
 * that leaves room for up to 13 multicast addresses.
 */
void dec21140_add_hw_address (idbtype *swidb, uchar *mac, uchar fsm)
{
    hwidbtype *idb;
    dec21140_instance_t *ds;
    dec21140_hwaf_t *hwaf_ptr;
    dec21140_regs_t *reg;
    boolean added_to_table = FALSE;
    boolean already_in_table = FALSE;
    uint indx;
    leveltype status;

    /*
     * Initialize local variables
     */
    status = raise_interrupt_level(NETS_DISABLE);
    idb = swidb->hwptr;
    ds = INSTANCE(swidb->hwptr);
    hwaf_ptr = ds->hwaf_buf_ptr;
    reg = ds->dec21140_regaddr;

    if (ether_debug) {
        buginf("\nDEC21140(%d): adding MAC addr %2x:%2x:%2x\n",
        idb->unit, *((ushort *)mac), *((ushort *)mac+1), *((ushort *)mac+2));
    }

    /*
     * Look for free entry and also make sure addr
     * wasn't already added
     */
    for (indx=0; indx < DEC21140_PERF_HWAF_BROAD_ENTRY; indx++) {
	if (DEC21140_PERF_HWAF_ENTRY_IS_EQUAL(mac, hwaf_ptr->addr[indx])) {
            already_in_table = TRUE;
	    break;
	}
        if (DEC21140_PERF_HWAF_ENTRY_IS_FREE(hwaf_ptr->addr[indx])) {
	    /*
	     * Update address filtering structure with multicast addr
	     */
	     hwaf_ptr->addr[indx].w[0].word = *((ushort *)mac);
	     hwaf_ptr->addr[indx].w[1].word = *((ushort *)mac + 1);
	     hwaf_ptr->addr[indx].w[2].word = *((ushort *)mac + 2);
	     added_to_table = TRUE;
	     break;
	}
    }

    if (added_to_table) {
	/*
	 * Refresh hwaf
	 */
	dec21140_hwaf_refresh(idb);
    } else if (!already_in_table) {
	/*
	 * Table full - enable promiscuous mode
	 */
	reg->csr6 |= DEC21140_CSR6_PROMISCUOUS_MODE;
	ds->addr_table_overflow = TRUE;
    }
    reset_interrupt_level(status);

    /*
     * Setup the instance with regard to isl_vlan_on_hwidb
     */
    if (idb->isl_vlan_on_hwidb) {
	if (ds->pacb.isl_supported) {
	    ds->pacb.isl_enabled = TRUE;
	} else {
	    errmsg(&msgsym(NOISL, DEC21140), idb->hw_namestring);
	    ds->pacb.isl_enabled = FALSE;
	}
    } else {
	ds->pacb.isl_enabled = FALSE;
    }
}

/*
 * dec21140_add_multicast_range:
 * Add multicast address range to the hardware address filter.
 * The only way this can be done is by enabling the reception of
 * all multicast addresses.
 */
void dec21140_add_multicast_range (idbtype *swidb, uchar *address, uchar *mask)
{
    dec21140_instance_t *ds = INSTANCE(swidb->hwptr);

    /*
     * Since even a small multicast address range will fill the
     * hardware address filter, enable reception of all multicast
     * addresses.
     */
    ds->dec21140_regaddr->csr6 |= DEC21140_CSR6_PASS_ALL_MULTICAST;
    ds->rx_all_multicast = TRUE;
}

/*
 * dec21140_setup_bridging:
 * Set up for bridging on this interface. Actually the init code does it.
 */
static void dec21140_setup_bridging (hwidbtype *idb)
{
    dec21140_init(idb, FALSE);
    if (idb->counters.resets > 0) {
	idb->counters.resets--;
    }
}

/*
 * dec21140_check:
 * If it should be running, read CSR0 and ensure that it is
 * running and has a normal value. Otherwise, reset it.
 */
static void dec21140_check (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    leveltype level;
    uint csr5;

    if (idb_is_throttled(idb))
	return;

    if (!ds)
	return;
    
    /*
     * Raise interrupt level and read status register
     */
    level = raise_interrupt_level(NETS_DISABLE);
    csr5 = ds->dec21140_regaddr->csr5;
    reset_interrupt_level(level);

    /*
     * Check that everything is cool with csr5
     */
    if (csr5 & (DEC21140_CSR5_ABNORM_INT_SUMMARY
		| DEC21140_CSR5_RX_PROCESS_STOPPED
		| DEC21140_CSR5_TX_PROCESS_STOPPED)) {
	if (ether_debug) {
	    buginf("\nDEC21140(%d): Bad state in CSR5=(%#x)\n",
		   idb->unit, csr5);
	}
	dec21140_init(idb, FALSE);
    }
}

/*
 * dec21140_txqlength:
 * Return the number of packets on the tx ring
 */
static int dec21140_txqlength (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);

    /*
     * Note: we are not able to support this exactly, as the tx_count
     * field is the number of particles queued on the tx ring rather
     * than the number of packets. But this should be close enough.
     */
    return(ds->tx_count);
}

/*
 * dec21140_enable:
 * Enable a DEC21140 interface. This function "unthrottles"
 * the interface.
 */
static void dec21140_enable (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    leveltype level;

    /*
     * Mark the interface as enabled (NOT throttled)
     */
    idb_mark_unthrottled(idb);
    ds->enabled++;

    /*
     * Reenable the Rx side
     */
    level = raise_interrupt_level(NETS_DISABLE);
    ds->dec21140_regaddr->csr6 |= DEC21140_CSR6_RECEIVE_START;
    reset_interrupt_level(level);
}

/*
 * dec21140_cache_threshold
 *
 * Callback routine from the buffer management code. We get a callback
 * when the interface private pool reaches our threshold value.
 * This function will reenable the receive side if it has been disabled.
 */
void dec21140_cache_threshold (pooltype *pool)
{
    hwidbtype *idb = (hwidbtype *)pool->hwidb;

    if (idb_is_throttled(idb))
        dec21140_enable(idb);
}

/*
 * dec21140_chip_startup:
 * Move the dec21140 from stopped to running state
 */
static boolean dec21140_chip_startup (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    uint csr6, csr12;
    dec21140_regs_t *reg;
    ethersb	*esb;

    reg = ds->dec21140_regaddr;
    esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);

    /*
     * Set up csr6 and csr12 for MII/RJ45, Full/Half duplex and promiscuous
     * mode or not.
     */
    if (ds->pa_type == PA_HARDWARE_1FE) { /* 1FE with motorolla PHY */

	csr6 = DEC21140_CSR6_INITIAL_VALUE;
	csr12 = DEC21140_CSR12_INITIAL_VALUE;

	if (esb->media_type == ETHER_MEDIA_TYPE_FEIP_RJ45) {
	    csr6 |= (DEC21140_CSR6_MEDIA_PORT_SCRAMBLE
		     | DEC21140_CSR6_MEDIA_PORT_MII_SYM_PCS);
	    csr12 &= ~(DEC21140_CSR12_MII);
	} else if (esb->media_type == ETHER_MEDIA_TYPE_FEIP_MII) {
	    csr6 &= ~(DEC21140_CSR6_MEDIA_PORT_SCRAMBLE
		      | DEC21140_CSR6_MEDIA_PORT_MII_SYM_PCS);
	    csr12 |= DEC21140_CSR12_MII;
	} else {
	    /*
	     * There can be at least two other types of media here,
	     * but they are not valid in this application. Return false.
	     */
	    return (FALSE);
	}

    } else { 				/* 1FE with national PHY */

	csr6  = DEC21140_CSR6_MICE_INITIAL_VALUE;
        /*
         * Set the initial value for csr12. The initial value is different
         * for the I/O card FE port adapter as compared to the other FE
         * port adapters because the MICE chip is wired differently on
         * I/O card. Also, since the FE port on the IO-card sports only 
         * the MII connector, the MII bit only applies to FE ports not 
         * on the IO-card.
         */
        if (ds->pa_type == PA_HARDWARE_C7100_IO1FE) {
            csr12 = DEC21140_CSR12_MICE_IOFE_INITIAL_VALUE;
        } else {
            csr12 = DEC21140_CSR12_MICE_INITIAL_VALUE;
            if (esb->media_type == ETHER_MEDIA_TYPE_FEIP_RJ45) {
	        csr12 &= ~(DEC21140_CSR12_MICE_MII);
            } else if (esb->media_type == ETHER_MEDIA_TYPE_FEIP_MII) {
	        csr12 |= DEC21140_CSR12_MICE_MII;
            } else {
	        /*
	         * There can be at least two other types of media here,
	         * but they are not valid in this application. Return false.
	         */
	        return (FALSE);
            }
        }
    }

    switch (idb->ext_loop_type) {
    case FEIP_LOOP_MOTOROLA:
          csr6 |= DEC21140_CSR6_OPER_MODE_EXT_LOOPBACK;
          csr12 &= ~DEC21140_CSR12_MOT_68836_NORMAL;
          break;
    case FEIP_LOOP_DEC:
          csr6 |= DEC21140_CSR6_OPER_MODE_INT_LOOPBACK;
          break;
    case FEIP_LOOP_MICRO_LINEAR:
          csr6 |= DEC21140_CSR6_OPER_MODE_EXT_LOOPBACK;
          csr12 &= ~DEC21140_CSR12_MICRO_6671_NORMAL;
          break;
    }

    if (esb->feip_fullduplex) {
	csr6 |= DEC21140_CSR6_FULL_DUPLEX;
    }
    if (DEC21140_DEFAULT_PROM_MODE || 
	(idb->tbridge_on_hwidb && bridge_enable &&
	 !(system_loading | bootstrap_enable))) {
	csr6 |= DEC21140_CSR6_PROMISCUOUS_MODE;
    }

    /* 
     * If the number of configured addresses is too large for
     * the hardware address filter, make the chip accept all packets.
     */
    if (ds->addr_table_overflow) {
	csr6 |= DEC21140_CSR6_PROMISCUOUS_MODE;
    }

    /*
     * If we're in PASS-ALL-MULTICAST-PACKET mode, make the chip do that
     */
    if (ds->rx_all_multicast) {
	csr6 |= DEC21140_CSR6_PASS_ALL_MULTICAST;
    }

    /*
     * Stop tx and rx engines of the chip
     */
    reg->csr6 = csr6 | (DEC21140_CSR6_RECEIVE_STOP
			| DEC21140_CSR6_TRANSMIT_STOP);
    usecdelay(DEC21140_RESET_DELAY);

    /*
     * Set up csr0
     */
    if (DEC21140_DMA_BIGENDIAN) {
	reg->csr0 = (DEC21140_CSR0_TX_AUTO_POLL_200_US
		     | DEC21140_CSR0_CACHE_ALIGN
		     | DEC21140_CSR0_PROG_BURST_LEN
		     | DEC21140_CSR0_BIG_ENDIAN
		     | DEC21140_CSR0_DESCRPT_SKIP_LENGTH
		     | DEC21140_CSR0_BAR);
    } else {
	reg->csr0 = (DEC21140_CSR0_TX_AUTO_POLL_200_US
		     | DEC21140_CSR0_CACHE_ALIGN
		     | DEC21140_CSR0_PROG_BURST_LEN
		     | DEC21140_CSR0_DESCRPT_SKIP_LENGTH
		     | DEC21140_CSR0_BAR);
    }

    /*
     * Set up interrupt control
     */
    reg->csr7 = (DEC21140_CSR7_ENB_NORM_INT_SUMMARY
		 | DEC21140_CSR7_ENB_ABNORM_INT_SUMMARY
		 | DEC21140_CSR7_ENB_FATAL_BUS_ERROR_INT
		 | DEC21140_CSR7_ENB_RX_WATCHDOG_TO_INT
		 | DEC21140_CSR7_ENB_RX_INT
		 | DEC21140_CSR7_ENB_TX_UNDERFLOW_INT
		 | DEC21140_CSR7_ENB_TX_INT);

    /*
     * Clear any pending interrupts
     */
    reg->csr5 = DEC21140_CSR5_CLEAR_ALL_INTERRUPTS;

    /*
     * Give the tx and rx ring address to the chip
     */
    reg->csr3 = DEC21140_CNTL_ADDR(ds->rxr);
    reg->csr4 = DEC21140_CNTL_ADDR(ds->txr);
     
    /* 
     * Configure pins on the general purpose port as outputs 
     */
    if (ds->pa_type == PA_HARDWARE_C7100_IO1FE) {
	/*
	 * Define all general purpose pins of CSR12 as outputs. After
         * setting the ISL_ENABLE bit high, delay a little and set the
         * ISL_RESET bit high. 
	 */
	reg->csr12 = DEC21140_CSR12_GPC | DEC21140_CSR12_MICE_OUTPUT_MASK;
	reg->csr12 = csr12;
        usecdelay(DEC21140_CSR12_MICE_IOFE_DELAY);
        reg->csr12 = csr12 | DEC21140_CSR12_MICE_IOFE_ISL_RESET;
    } else {
	if (ds->pa_type == PA_HARDWARE_1FE) { /* 1FE without MICE hardware */
	    reg->csr12 = DEC21140_CSR12_GPC | DEC21140_CSR12_OUTPUT_MASK;
	    reg->csr12 = csr12;
	} else { /* 1FE with MICE hardware */
	    reg->csr12 = DEC21140_CSR12_GPC | DEC21140_CSR12_MICE_OUTPUT_MASK;
	    reg->csr12 = csr12;
	}
    }

    /*
     * Re-enable the rx and tx engines. Don't re-enable the rx side if the
     * interface is being throttled
     */
    if (idb_is_throttled(idb)) {
	reg->csr6 = csr6 | DEC21140_CSR6_TRANSMIT_START;
    } else {
	reg->csr6 = csr6 | (DEC21140_CSR6_RECEIVE_START
			    | DEC21140_CSR6_TRANSMIT_START);
    }
    idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
    
    /* 
     * Return success
     */
    return(TRUE);
}

/*
 * dec21140_init:
 * Initialize the hardware interface
 */ 
boolean dec21140_init (hwidbtype *idb, boolean maxdgram_change)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    dec21140_rx_desc_t  *rmd;
    dec21140_tx_desc_t  *tmd;
    idbtype 		*swidb;
    leveltype 		level;
    ushort 		hw_station[3];
    particletype 	*mp;
    int 		i;
    ethersb		*esb;

    /*
     * Raise the interrupt level to protect from race conditions
     */
    level = raise_interrupt_level(NETS_DISABLE);
    
    /*
     * Stop the DEC21140 chip and count the reset
     */
    ds->fatal_tx_err = FALSE;
    dec21140_stop(idb);
    idb->counters.resets++;
    if (idb->state != IDBS_ADMINDOWN) {
	net_cstate(idb, IDBS_RESET);
    }

    /*
     * Flush the hold queue
     */
    holdq_reset(idb);

    /*
     * If interface is administratively down, don't re-enable it.
     */
    if (idb->state == IDBS_ADMINDOWN) {
	idb->ethernet_lineup = TRUE;
	dec21140_lineproto_change(idb, TRUE);
	reset_interrupt_level(level);
	return(TRUE);
    }

    dec21140_stop(idb);
    dec21140_reset(idb);

    ds->rx_head = ds->rx_tail = 0;	/* Rx head, tail indexes */
    ds->tx_head = ds->tx_tail = 0;	/* Tx head, tail indexes */
    ds->tx_count = 0;			/* no particles on Tx ring */
    
    ds->tx_limited = hwidb_use_short_tx_q_inline(idb);
    if (ds->tx_limited) {
	idb->mci_txcount = 1;
    } else {
	idb->mci_txcount = DEC21140_TX_RING_ENTRIES;
    }

    /*
     * Initialize idb namestrings
     */
    if (!idb->hw_namestring) {
	idb_init_names(idb->firstsw, FALSE);
    }

	dec21140_get_chip_version(idb);
	if (ds->is_DEC21140A)
		idb->typestring = "DEC21140A";
	else
		idb->typestring = "DEC21140";
    /*
     * Initialize private buffer pools
     */
    dec21140_init_buffer_pools(idb);

    /*
     * Init the rx ring with particles owned by the DEC21140
     */
    for (i=0; i<DEC21140_RX_RING_ENTRIES; i++) {

	rmd = &ds->rxr[i];
	rmd->control_length = DEC21140_RX_CONT_LENGTH(DEC21140_PARTICLE_SIZE);

	if (i < (DEC21140_RX_RING_ENTRIES-1)) {
	    rmd->next_descrpt_ptr = 
		(dec21140_rx_desc_t *)(DEC21140_CNTL_ADDR(&ds->rxr[i+1]));
	} else {
	    rmd->next_descrpt_ptr = 
		(dec21140_rx_desc_t *)(DEC21140_CNTL_ADDR(&ds->rxr[0]));
	}
	if (ds->rxr_shadow[i]) {
	    /*
	     * There is already a particle on the shadow rx ring.
	     * Give it to the DEC21140.
	     */
	    mp = ds->rxr_shadow[i];
	    rmd->buf_addr = DEC21140_BUF_ADDR(mp->data_start);
	    rmd->rx_status = DEC21140_RMD_OWN;
	} else {
	    /*
	     * There is no particle on the shadow rx ring, so 
	     * allocate it and give it to the DEC21140
	     */
	    mp = dec21140_get_particle(idb);
	    if (mp) {
		ds->rxr_shadow[i] = mp;
		rmd->buf_addr = DEC21140_BUF_ADDR(mp->data_start);
		rmd->rx_status = DEC21140_RMD_OWN;
	    } else {
		idb->ethernet_lineup = FALSE;
		net_cstate(idb, IDBS_DOWN);
		dec21140_lineproto_change(idb, FALSE);
		reset_interrupt_level(level);
		return(FALSE);
	    }
	}
    }
    dec21140_rxring_flush(ds);

    /*
     * Set up the tx ring and ensure that the CPU owns
     * all the buffers and/or particles
     */
    for (i=0; i<DEC21140_TX_RING_ENTRIES; i++) {

	tmd = &ds->txr[i];

	tmd->tx_status = 0;
	tmd->control_length = 0;
	tmd->buf_addr = 0;

	if (i < (DEC21140_TX_RING_ENTRIES-1)) {
	    tmd->next_descrpt_ptr = 
		(dec21140_tx_desc_t *)(DEC21140_CNTL_ADDR(&ds->txr[i+1]));
	} else {
	    tmd->next_descrpt_ptr = 
		(dec21140_tx_desc_t *)(DEC21140_CNTL_ADDR(&ds->txr[0]));
	}
	if (ds->txr_shadow[i].pak && 
	    (ds->txr_shadow[i].pak != DEC21140_MAGIC_PAK_PTR)) {
	    datagram_done(ds->txr_shadow[i].pak);
	}
	if (ds->txr_shadow[i].mp) {
	    retparticle(ds->txr_shadow[i].mp);
	}
	ds->txr_shadow[i].pak = NULL;
	ds->txr_shadow[i].mp = NULL;
    }

    /*
     * Initialize the hardware addresses.
     * Initialization of hardware addresees needs to come
     * after the Tx ring is setup since it uses transmit
     * descriptors to set the hardware MAC address(es).
     *
     * The hardware address filter cannot be set here for
     * RVIP or SVIP.
     */
#if defined(PREDATOR)
    dec21140_hwaf_clear(idb);
#endif
    dec21140_get_default_hw_address(idb, idb->bia);
    if (is_ieee_zero(idb->hardware)) {
	dec21140_get_default_hw_address(idb, idb->hardware);
    }
    bscopy(idb->hardware, (uchar *)hw_station, IEEEBYTES);

    /*
     * Give the chip its MAC address
     */
    dec21140_set_hw_address(idb, idb->hardware);

    /*
     * Refresh any mac filters
     */
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	reg_invoke_tbridge_smf_update(idb->hardware,
				      swidb,
				      SMF_MACTYP_OWN_UNICAST |
				      SMF_ACTION_RECEIVE);
	if (!(system_loading | bootstrap_enable)) {
	    reg_invoke_media_interesting_addresses(swidb);
	}
    }

    /*
     * Startup the device
     */
    if (dec21140_chip_startup(idb)) {

	esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);
	/*
	 * If we have 1FE port adapters with MICE hardware or if we have 
         * 1FE port adapter that has been configured for MII, bring the MII 
         * transciever out of isolation
	 */
	if ((ds->pa_type != PA_HARDWARE_1FE) ||
	    (esb->media_type == ETHER_MEDIA_TYPE_FEIP_MII)) {
	    ushort mii_data;

	    mii_data = DP83840_CSR00_NO_LOOPBACK |
	               DP83840_CSR00_SPEED_100MBIT |
	               DP83840_CSR00_NO_ISOLATE;

	    if (esb->feip_fullduplex)
		mii_data |= DP83840_CSR00_FULL_DUPLEX;

            dec21140_write_mii_reg(ds->dec21140_regaddr, 0, 0, mii_data); 
        }
	idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);

	/*
	 * Signify that we are now up
         * Force the line state to be up as desired by the caller (from RSP).
         * If the carrier was gone, we'll detect that in next round of
         * keepalive check. (note: ethernet_lineup state must in sync
         * with state we call dec21140_lineproto_change with)
         */
        idb->ethernet_lineup = TRUE;
        ds->phy_link_active = TRUE;
        net_cstate(idb, IDBS_UP);
        dec21140_lineproto_change(idb, TRUE);
        if (ether_debug) {
            buginf("%s: Interface is alive\n",idb->hw_namestring);
	}
        reset_interrupt_level(level);
	return(TRUE);

    } else {

        errmsg(&msgsym(INITFAIL, DEC21140), 
	       idb->slot, idb->subunit, ds->dec21140_regaddr->csr5);
        dec21140_shutdown(idb, IDBS_ADMINDOWN);
        idb->ethernet_lineup = FALSE;
	dec21140_lineproto_change(idb, FALSE);
	reset_interrupt_level(level);
	return(FALSE);
    }
}

/*
 * dec21140_error_interrupt:
 * Handle Error interrupts from the DEC21140
 */
void dec21140_error_interrupt (ulong csr5, hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    uint csr0;

    csr0 = ds->dec21140_regaddr->csr0;
    
    /*
     * Report the error and re-initialize the chip
     */
    errmsg(&msgsym(ERRINT, DEC21140), idb->slot, idb->subunit, csr0, csr5);
    dec21140_init(idb, FALSE);
}
 
/*
 * dec21140_update_ignore
 * Update the missed frame counter
*/
void dec21140_update_ignore (hwidbtype *idb, int csr8count)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    uint rx_overrun;

    idb->counters.input_resource +=
	(csr8count & DEC21140_CSR8_MISSED_FRAME_MASK);

    if (ds->is_DEC21140A) {
        /*
         * these are for the new DEC21140-AE, for the old one, nothing t here
         */
        rx_overrun = (csr8count & DEC21140_CSR8_OVERFLOW_COUNT_MASK) >> 17;
        ds->rx_fifo_overflow += rx_overrun;
        idb->counters.input_overrun += rx_overrun;
    }

}

/*
 * dec21140_update_link_status:
 * Check to see if the link is active 
 */
static void dec21140_update_link_status (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    short phy_status_reg;

    ds->phy_link_active = TRUE;

    /*  
     * In loopback or half-duplex mode, consider the link to be active.
     */
    if (((ds->dec21140_regaddr->csr6 & DEC21140_CSR6_OPER_MODE_MASK) !=
        DEC21140_CSR6_OPER_MODE_NORMAL) || 
	!(ds->dec21140_regaddr->csr6 & DEC21140_CSR6_FULL_DUPLEX))
        return;

    /*  
     * For the 1FE port adapter with the national part, read the status register
     * on the PHY and for the 1FE port adapter without the national part, check
     * the general purpose register's link detection line. In MII mode. read
     * the external transeiver's status register.
     */
    if ((ds->pa_type != PA_HARDWARE_1FE) ||
        (ds->dec21140_regaddr->csr12 & DEC21140_CSR12_MII)) {
	phy_status_reg = dec21140_read_mii_reg(ds->dec21140_regaddr, 0, 0x1);
	if (!(phy_status_reg & DP83840_CSR01_LINK_STATUS))
	    ds->phy_link_active = FALSE;
    } else {
	if (!(ds->dec21140_regaddr->csr12 & 
	      DEC21140_CSR12_LINK_SIGNAL_DETECT_IN))
	    ds->phy_link_active = FALSE;
    }
}

/*
 * dec21140_periodic:
 * Periodic activities for DEC21140 device driver
 */
static void dec21140_periodic (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);

    /*
     * update the link status
     */
    dec21140_update_link_status(idb);

    /*
     * Force lineup if no keepalives are configured or we are shutdown.
     * Otherwise, send a keepalive. We cannot hear ourself like an MCI does,
     * but dec21140_tx_interrupt will clear ethernet_lineup if we get a lost
     * carrier indication back from the chip. If we do not get a lost
     * carrier indication, dec21140_tx_interrupt will set lance_lineup. If
     * dec21140_rx_interrupt receives a packet, ethernet_lineup will be set.
     */
    if (DEC21140_LOCAL_KEEPALIVE) {
	if (idb->nokeepalive || idb->state == IDBS_ADMINDOWN) {
	    if (!idb->ethernet_lineup) {
		idb->ethernet_lineup = TRUE;
		if (idb->state != IDBS_ADMINDOWN) {
		    idb->reason = "Checking disabled";
		    dec21140_lineproto_change(idb, TRUE);
		}
	    }
	} else {
	    if (system_configured)
		send_loop(idb, idb->hardware);
	}
    }

    /*
     * Update the Ignores counter.
     * Include the overflow bit as an extra bit in the counter
     * (assume that a single overflow happened).
     *
     * PS: In the worst scenario it can overflow up to 3 times per
     * second! (when dropping small packets arriving at line rate).
     * However, this counter does NOT need to be exact.
     */
    dec21140_update_ignore(idb, ds->dec21140_regaddr->csr8);

    /*
     * Check the device
     */
    if (idb->state == IDBS_UP) {
	dec21140_check(idb);
    }
}

/*
 * dec21140_isl_supported:
 * Return TRUE if ISL is supported on this variant of the 1fe hardware.
 * Otherwise return false.
 */
static boolean dec21140_isl_supported (ushort pa_type)
{
    if (pa_type == PA_HARDWARE_1FE_TX_ISL ||
	pa_type == PA_HARDWARE_1FE_FX_ISL ||
        pa_type == PA_HARDWARE_C7100_IO1FE) {
	/*
	 * Can support ISL in hardware.
	 */
	return(TRUE);
    }
    return(FALSE);
}

/*
 * dec21140_init_idb:
 * IDB init routine for DEC21140 interfaces
 */
static boolean dec21140_init_idb (hwidbtype *idb, dec21140_regs_t *base_addr)
{
    idbtype *swidb = idb->firstsw;
    dec21140_instance_t *ds;
    ethersb		*esb;

    /*
     * If this is the first time through this routine
     * for this idb, then initialize the following.
     * For idbs off the removed queue, this initialization
     * should not be repeated.
     */
    if (idb->oir_flags & IDB_FIRST_INIT) {
	idb->status = IDB_ETHER | IDB_BCAST | IDB_BRIDGE;
	idb->type = IDBTYPE_FEIP;
	if (pas_oir_active()) {
	    idb->state = IDBS_ADMINDOWN; 
	} else {
	    idb->state = IDBS_INIT;
	}
	idb->enctype = ET_ARPA;
	idb->error_threshold = 5000;
	idb->error_count = 0;
	idb->fast_switch_flag = 0;
	idb->rxload = 1;
	idb->span_encapsize = 0;
	idb->pool = NULL;
	idb->pool_group = POOL_GROUP_INVALID;
	swidb->tbridge_media = TBR_ETHER;

	/*
	 * Set up function calls
	 */  
	idb->soutput = dec21140_tx_start;
	idb->reset = dec21140_restart;
	idb->linestate = dec21140_linestate;
	idb->shutdown = dec21140_shutdown;
	idb->periodic = dec21140_periodic;
	idb->listen = dec21140_add_hw_address;
	idb->listen_range = dec21140_add_multicast_range;
	idb->enable = dec21140_enable;
	idb->setup_bridging = dec21140_setup_bridging;
	swidb->span_disabled = FALSE;
	idb->txqlength = dec21140_txqlength;
	idb->show_controller = dec21140_show_controller;
	idb->fastsend = dec21140_fastsend;

	ether_idb_init(idb);

	/*
	 * Initialize default half/full duplex and RJ45/MII fields
	 */
	esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);
	esb->media_type = dec21140_default_media(idb);
	esb->feip_fullduplex = dec21140_default_duplex(idb);

	/*
	 * Don't use 'esb' beyond this call.
	 */
	idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);

	idb->oir_flags &= ~IDB_FIRST_INIT;
    }
    media_idb_enqueue(idb);

    /*
     * Setup encsize
     */
    idb->encsize = ETHER_ARPA_ENCAPBYTES;

    /*
     * Allocate an instance structure for this device
     */
    ds = malloc_fast(sizeof(dec21140_instance_t));
    if (!ds) {
	return(FALSE);
    }
    INSTANCE(idb) = (void *)ds;
    ds->dec21140_regaddr = base_addr;
    pas_instance_init_common(&ds->pacb, idb);
    ds->pa_type = get_pa_type(ds->pacb.pa_bay);
    ds->phy_link_active = TRUE;
    ds->pacb.isl_supported = dec21140_isl_supported(ds->pa_type);

    /*
     * Set up the removal vector here so that if the init process
     * fails from here on in, there will be a device specific vector
     * defined to clean up resources
     */
    ds->pacb.pas_deallocate = dec21140_deallocate;

    /*
     * Allocate init block
     */
    ds->ib_malloc = malloc_named_pcimem(sizeof(dec21140_initblock_t),
					idb->hw_namestring);
    if (!ds->ib_malloc) {
	return(FALSE);
    }
    ds->ib = ds->ib_malloc;

    /*
     * Allocate rx ring and rx ring shadow
     */
    ds->rxr_malloc = malloc_named_pcimem(sizeof(dec21140_rx_desc_t) * 
					 DEC21140_RX_RING_ENTRIES,
					 idb->hw_namestring);
    if (!ds->rxr_malloc) {
	return(FALSE);
    }
    ds->rxr = pci_memaddr_convert(ds->rxr_malloc, DEC21140_RX_RING_ATTRIBUTE);
    ds->rxr_shadow = malloc_fast(sizeof(particletype *) * 
				 DEC21140_RX_RING_ENTRIES);
    if (!ds->rxr_shadow) {
	return(FALSE);
    }
    dec21140_rxring_flush(ds);

    /*
     * Allocate tx ring and tx ring shadow
     */
    ds->txr_malloc = malloc_named_pcimem(sizeof(dec21140_tx_desc_t) * 
					 DEC21140_TX_RING_ENTRIES,
					 idb->hw_namestring);
    if (!ds->txr_malloc) {
	return(FALSE);
    }
    ds->txr = ds->txr_malloc;
    ds->txr_shadow = malloc_fast(sizeof(dec21140_tx_shadow_t) * 
				 DEC21140_TX_RING_ENTRIES);
    if (!ds->txr_shadow) {
	return(FALSE);
    }

    /*
     * Allocate address filtering frame buffer
     * This buffer holds an exact copy of the
     * Perfect Filtering Setup Frame as programmed in the
     * DEC21140.
     */
    ds->hwaf_buf_ptr = malloc(sizeof(dec21140_hwaf_t));
    if (!ds->hwaf_buf_ptr) {
      return(FALSE);
    }

    /*
     * Clear hardware address filter structure and address
     * overflow boolean.
     */
    dec21140_hwaf_clear(idb);
 
    /*
     * Do platform dependent initialization
     */
    if (!dec21140_platform_init(idb)) {
	return(FALSE);
    }

    /*
     * Successfully got all resources
     */
    return(TRUE);
}

/*
 * dec21140_init_interface:
 * Initialize a DEC21140 PA interface
 */
static boolean dec21140_init_interface (uint slot, uint unit, uint subunit,
					dec21140_regs_t *base_addr)
{
    dec21140_instance_t *ds;
    hwidbtype *idb;

    /*
     * Set up storage, add IDB to interface queue
     */
    idb = dec21140_idb_create(slot, subunit);
    if (!idb) {
	return(FALSE);
    }

    /*
     * Save our unit number and PA slot number. Note, since there is only
     * one interface on the fast etherenet PA, we hardcode subunit and
     * slotunit to 0 (which allows the Predator 1fe IO port to come up
     * as interface fast 0/0).
     */
    idb->slot = slot;       /* Corresponds to IP */
    idb->unit = unit;       /* nth ethernet interface */
    idb->subunit = 0; 	    /* Corresponds to viper concept of interface # */
    idb->slotunit = 0;      /* For slotted interface names */
    pas_set_fci_type(idb, slot); /* Need fci type for OIR support */
    idb->status &= ~IDB_NOHARDWARE;

    /*
     * Initialize the idb
     */
    if (!dec21140_init_idb(idb, base_addr)) {
	return(FALSE);
    }
    ds = INSTANCE(idb);
    ds->pci_device_number = subunit;

    /*
     * Initialize the hardware
     */
    if (!dec21140_init(idb, FALSE)) {
	return(FALSE);
    } else {
	idb->counters.resets = 0;
	return(TRUE);
    }
}

/*
 * dec21140_pci_init:
 * Perform PCI configuration specific to the DEC21140
 */
static dec21140_regs_t *dec21140_pci_init (uint pa_bay, uint device_no)
{
    uint value, id;
    dec21140_regs_t *device_base;
    uchar bus_no;

    /*
     * Get our bus no
     */
    bus_no = get_pci_bus_no(pa_bay);

    /*
     * Read the device/vendor registers to verify that this
     * is a DEC21140 Ethernet controller.
     */
    id = pci_config_read(bus_no, device_no, 0, DEC21140_PCI_CFID_REG_OFFSET);
    if (id != DEC21140_DEVICE_VENDOR_ID) {
	errmsg(&msgsym(NOTDEC21140, DEC21140), id);
	return(NULL);
    }

    /*
     * Get device base address
     */
    device_base = get_pci_device_base(pa_bay, device_no);

    /*
     * Configure CFCS, the command and status configuration register
     * and the device address space defintion.
     */
    if (DEC21140_MEMMAPPED_IO) {
	value = (DEC21140_CFCS_MEM_SPACE_ACCESS|
		 DEC21140_CFCS_MASTER_OPERATION);
	pci_config_write(bus_no, device_no, 0, DEC21140_PCI_CFCS_REG_OFFSET, 
			 value);
	value = get_pci_device_base_offset(pa_bay, device_no);
	pci_config_write(bus_no, device_no, 0, DEC21140_PCI_CBMA_REG_OFFSET,
			 value);
    } else {
	value = (DEC21140_CFCS_IO_SPACE_ACCESS|
		 DEC21140_CFCS_MASTER_OPERATION);
	pci_config_write(bus_no, device_no, 0, DEC21140_PCI_CFCS_REG_OFFSET, 
			 value);
	value = get_pci_device_base_offset(pa_bay, device_no);
	pci_config_write(bus_no, device_no, 0, DEC21140_PCI_CBIO_REG_OFFSET,
			 value & 0xffff);
    }

    /*
     * Configure CFLT, the configuration latency timer register.
     */
    pci_config_write(bus_no, device_no, 0, DEC21140_PCI_CFLT_REG_OFFSET,
		     DEC21140_CFLT_LATENCY_TIMER);

    /*
     * Return the device base address
     */
    return(device_base);
}

/*
 * dec21140_analyze_device:
 * Analyze an DEC21140 interface
 */
boolean dec21140_analyze_device (uint pa_bay, uint device_no)
{
    dec21140_regs_t *device_base;

    /*
     * Perform PCI configuration specific to the device
     */
    device_base = dec21140_pci_init(pa_bay, device_no);
    if (!device_base) {
	return(FALSE);
    }

    /* 
     * Initialize this device
     */
    if (!dec21140_init_interface(pa_bay, nfethernets, device_no, 
				 device_base)) {
	return(FALSE);
    } else {
	nfethernets += 1;
	nethernets += 1;
	return(TRUE);
    }
}

/*
 * dec21140_rx_error:
 * Process a receive error.
 */
void dec21140_rx_error (ulong rmd_status, hwidbtype *idb, particletype *mp)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    ethersb		*esb;

    esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);

    /*
     * Runt frame
     */
    if (rmd_status & DEC21140_RMD_RUNT_FRAME_ERROR) {
	idb->counters.runts++;
    }

    /*
     * Giants
     */
    if (rmd_status & DEC21140_RMD_FRAME_TOO_LONG_ERROR) {
        idb->counters.giants++;
    }

    /* 
     * Late collision 
     */
    if (rmd_status & DEC21140_RMD_LATE_COLLISION_SEEN_ERROR) {
        ds->rx_late_collision++;
        errmsg(&msgsym(LATECOLL, DEC21140), idb->slot, idb->subunit);
    }

    /*
     * Receive watchdog
     */
    if (rmd_status & DEC21140_RMD_RECEIVE_WATCHDOG_ERROR) {
	esb->feip_watchdog++;
        errmsg(&msgsym(WATCHDOG, DEC21140), idb->slot, idb->subunit);
    }

    /* 
     * Dribbling bits 
     */
    if (rmd_status & DEC21140_RMD_DRIBBLE_BITS_ERROR) {
        idb->counters.input_dribble++;
    }

    /*
     * Input CRC
     */
    if (rmd_status & DEC21140_RMD_CRC_ERROR) {
        idb->counters.input_crc++;
    }

    /* 
     * FIFO overflow caused data truncation 
     */
    if (!ds->is_DEC21140A && (rmd_status & DEC21140_RMD_FIFO_OVERFLOW_ERROR)) {
	ds->rx_fifo_overflow++;
        idb->counters.input_overrun++;
	if (ether_debug) {
	    errmsg(&msgsym(RXFIFO, DEC21140), idb->slot, idb->subunit);
	}
    }

    /*
     * Account for the error
     */
    idb->counters.input_error++;
    idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);

    /*
     * Clean up. If it was the last particle in the packet, clear 
     * rx_discard, otherwise set it.
     */
    mp->next = NULL;
    retparticle(mp);
    dec21140_set_rx_discard(ds, FALSE);
}

/*
 * dec21140_link_down:
 * Handle link down on a transmit.
 */
void dec21140_link_down (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);

    /* 
     * In loopback mode, we don't care about link status
     */
    if ((ds->dec21140_regaddr->csr6 & DEC21140_CSR6_OPER_MODE_MASK) ==
	DEC21140_CSR6_OPER_MODE_INT_LOOPBACK) {
	return;
    }

    if (DEC21140_LOCAL_KEEPALIVE) {
	if (!idb->nokeepalive) {
	    if (idb->ethernet_lineup == TRUE) {
		idb->ethernet_lineup = FALSE;
		idb->reason = "Keepalive failed";
		dec21140_lineproto_change(idb, FALSE);
	    }
	}
     } else {
	if (idb->ethernet_lineup == TRUE) {
	    idb->ethernet_lineup = FALSE;
	    idb->reason = "Keepalive failed";
	    dec21140_lineproto_change(idb, FALSE);
	}
     }
     idb->failCause = Ether_LostCarrier;
}

/*
 * dec21140_tx_error:
 * Handle an error on a transmit.
 */
void dec21140_tx_error (ulong tmd_status, hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    boolean loopback;
    ethersb	*esb;

    if ((ds->dec21140_regaddr->csr6 & DEC21140_CSR6_OPER_MODE_MASK) ==
	DEC21140_CSR6_OPER_MODE_INT_LOOPBACK) {
	loopback = TRUE;
    } else {
	loopback = FALSE;
    }

    /*
     * Count the error
     */
    idb->counters.output_error++;
    esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);

    /* 
     * TX process stopped 
     */
    if (tmd_status & DEC21140_TMD_JABBER_TO) {
	ds->tx_jabber_timeout++;
	esb->output_babbles++;
	ds->fatal_tx_err = TRUE;
	idb->failCause = Ether_FatalTxError;
	errmsg(&msgsym(JABBER, DEC21140), idb->slot, idb->subunit);
    }

    /* 
     * Carrier loss, only valid when not in loopback
     */
    if ((tmd_status & DEC21140_TMD_CARRIER_LOSS) && !loopback) {
	ds->tx_carrier_loss++;
	esb->output_lost_carrier++;
	
	if (DEC21140_LOCAL_KEEPALIVE) {
	    if (!idb->nokeepalive) {
		if (idb->ethernet_lineup == TRUE) {
		    idb->ethernet_lineup = FALSE;
		    idb->reason = "Keepalive failed";
		    dec21140_lineproto_change(idb, FALSE);
		}
	    }
	} else {
	    if (idb->ethernet_lineup == TRUE) {
		idb->ethernet_lineup = FALSE;
		idb->reason = "Keepalive failed";
		dec21140_lineproto_change(idb, FALSE);
	    }
	}
	if (ether_debug) {
	    errmsg(&msgsym(LOSTCARR, DEC21140), idb->slot, idb->subunit);
	}
	idb->failCause = Ether_LostCarrier;
    }

    /*
     * No carrier, only valid when not in loopback
     */
    if ((tmd_status & DEC21140_TMD_NO_CARRIER) && !loopback) {
	ds->tx_no_carrier++;
	if (ether_debug) {
	    errmsg(&msgsym(NOCARR, DEC21140), idb->slot, idb->subunit);
	}
    }
     
    /*
     * Late collison, not valid if underflow error set 
     */
    if ((tmd_status & DEC21140_TMD_LATE_COLL)
	&& (!(tmd_status & DEC21140_TMD_UFLO_ERR))) {
	ds->tx_late_collision++;
	esb->output_late_collision++;
	errmsg(&msgsym(LATECOLL, DEC21140), idb->slot, idb->subunit);
    }

    /*
     * Excessive collisions
     */
    if (tmd_status & DEC21140_TMD_EXCESS_COLL) {
	esb->output_excessive_collision++;
	ds->tx_excess_coll++;
	if (ether_debug) {
	    errmsg(&msgsym(COLL, DEC21140), idb->slot, idb->subunit);
	}
	idb->failCause = Ether_ExcessiveCongestion;
    }
    
    /*
     * Tx underflow
     */
    if (tmd_status & DEC21140_TMD_UFLO_ERR) {
	ds->tx_underrun_err++;
	idb->counters.output_underrun++;
	if (ether_debug) {
	    errmsg(&msgsym(UNDERFLO, DEC21140), idb->slot, idb->subunit);
	}
	ds->dec21140_regaddr->csr1 = DEC21140_CSR1_TX_POLL_DEMAND;
    }

    /*
     * Output deffered
     */
    if (tmd_status & DEC21140_TMD_DEFERRED) {
	ds->tx_deferred++;
        esb->output_deferred++;
    }
    idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
}

/*
 * dec21140_1fe_analyze_pa:
 * Init the pci to pci bridge above the port adaptor, then discover devices
 * behind the bridge on the port adaptor.
 */
static void dec21140_1fe_analyze_pa (int pa_bay)
{
    ulong count;
    uchar bus_no;

    bus_no = get_pci_bus_no(pa_bay);

    /*
     * Discover and initialize devices on the PA
     */
    count = pci_discover_devices(bus_no, MAX_PCI_DEVICES, pa_bay); 
    if (count != 1) {
	errmsg(&msgsym(DISCOVER, DEC21140), count, pa_bay);
	pas_deactivate_bay(pa_bay);
	return;
    }

    /*
     * Update slots state.
     */
    pas_activate_bay(pa_bay);

    /*
     * Install PA wrapper
     */
    pas_install_wrapper(pa_bay, dec21140_eim_wrapper);
}

/*
 * dec21140_subsys_init:
 * Initialize the DEC21140 subsystem
 */
static void dec21140_subsys_init (subsystype *subsys)
{
    /*
     * Register the PCI 100e PAs.
     */
    reg_add_analyze_interface(PA_HARDWARE_1FE,
                              dec21140_1fe_analyze_pa,
                              "dec21140_1fe_analyze_pa");
    reg_add_analyze_interface(PA_HARDWARE_1FE_TX_ISL,
                              dec21140_1fe_analyze_pa,
                              "dec21140_1fe_analyze_pa");
    reg_add_analyze_interface(PA_HARDWARE_1FE_FX_ISL,
                              dec21140_1fe_analyze_pa,
                              "dec21140_1fe_analyze_pa");
    reg_add_analyze_interface(PA_HARDWARE_1FE_TX_NISL,
                              dec21140_1fe_analyze_pa,
                              "dec21140_1fe_analyze_pa");
    reg_add_pci_device_init(DEC21140_DEVICE_VENDOR_ID,
			    dec21140_analyze_device,
			    "dec21140_analyze_device");
}
 
/*
 * DEC21140 subsystem header
 */
#define DEC21140_MAJVERSION   1
#define DEC21140_MINVERSION   0
#define DEC21140_EDITVERSION  1

SUBSYS_HEADER(dec21140, DEC21140_MAJVERSION, DEC21140_MINVERSION,
	      DEC21140_EDITVERSION,
	      dec21140_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);

/* end of file */
