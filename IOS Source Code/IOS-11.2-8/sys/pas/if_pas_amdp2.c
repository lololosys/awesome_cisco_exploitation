/* $Id: if_pas_amdp2.c,v 3.10.6.20 1996/08/28 13:06:12 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_amdp2.c,v $
 *------------------------------------------------------------------
 * if_pas_amdp2.c - 8e Presidio2 based PCI port adaptor support
 *		    (Platform independent components)
 *
 * April 1995, Dave Turner
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_amdp2.c,v $
 * Revision 3.10.6.20  1996/08/28  13:06:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.6.19  1996/08/26  18:23:39  feisal
 * CSCdi45144:  4E/1FE error handling cleanup
 * Branch: California_branch
 *
 * Revision 3.10.6.18  1996/06/11  19:00:34  feisal
 * CSCdi56807:  8E and 4E need clean up--Babbles and Unit numbers
 * Branch: California_branch
 *
 * Revision 3.10.6.17  1996/05/13  20:26:22  dcarroll
 * CSCdi56363:  OIR-insertion of 4E/8E/4T PAs should  bring-up the ports
 * in shut
 * Branch: California_branch
 * OIR'd adapters must stay shutdown through initializatio
 *
 * Revision 3.10.6.16  1996/05/08  06:09:25  feisal
 * CSCdi45137:  No line down event when 4E/FE removed the first time.
 * Branch: California_branch
 *
 * Revision 3.10.6.15  1996/05/07  01:22:34  mbeesley
 * CSCdi56710:  Correct AMDp2 fifo setting, update error messages
 * Branch: California_branch
 *
 * Revision 3.10.6.14  1996/05/07  00:23:55  mbeesley
 * CSCdi56157:  Enable IP Flow/Optimum on Ethernets
 * Branch: California_branch
 *
 * Revision 3.10.6.13  1996/05/01  13:28:05  dcarroll
 * CSCdi54895:  oir fix for ethernet port adapter
 * Branch: California_branch
 * OIR support for pas ethernets.
 *
 * Revision 3.10.6.12  1996/04/29  17:57:14  ssangiah
 * CSCdi54472: Inadequate information in the output of "sh controllers"
 *             command
 *   o Add a "show diag" command for the c7200 platform.
 *   o Make the output from "show controllers" consistent.
 * Branch: California_branch
 *
 * Revision 3.10.6.11  1996/04/23  17:13:40  gshen
 * CSCdi55375:  Need to have different address separation for IO and
 * memory based IO
 * Branch: California_branch
 *
 * Two new I/O address separation definitions:
 *
 * #define PCI_DEV_IO_MAPPED_IO_SEPARATION  0x0400 / * 1k * /
 * #define PCI_DEV_MEM_MAPPED_IO_SEPARATION 0x4000 / * 16k * /
 *
 * These changes have been code reviewed.
 *
 * Revision 3.10.6.10  1996/04/19  15:26:07  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.8  1996/04/09  21:34:14  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.7  1996/03/30  05:50:05  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 2.1.6.6  1996/03/19  08:29:49  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.5  1996/01/31  04:48:16  xliu
 * Branch: IosSec_branch
 * Updated epa driver.
 *
 * Revision 2.1.6.4  1996/01/23  06:27:33  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.3  1995/12/19  04:12:00  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.2  1995/12/17  02:14:16  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.10.6.9  1996/04/08  19:00:49  dcarroll
 * CSCdi53796:  Remove extra idb type assignments
 * Branch: California_branch
 * Remove the unneeded early setting of idb type.
 *
 * Revision 3.10.6.8  1996/04/05  19:56:07  dcarroll
 * CSCdi53769:  Return bool from pas init interface routines
 * Branch: California_branch
 * Pass boolean up the analyze chain, not idbs.
 *
 * Revision 3.10.6.7  1996/04/05  05:32:10  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.10.6.6  1996/04/04  20:04:10  getchell
 * CSCdi50943:  SVIP PA reset broken
 * Branch: California_branch
 *
 * Revision 3.10.6.5  1996/04/01  23:07:34  mbeesley
 * CSCdi53228:  Ether init routine loops infinitly on hw failure
 * Branch: California_branch
 *
 * Revision 3.10.6.4  1996/03/30  03:13:11  sjacobso
 * CSCdi52412:  4e/8e: Resolve code review comment
 * Branch: California_branch
 * Convert code in #if defined(PREDATOR) construct to
 * platform dependent macro.
 *
 * Revision 3.10.6.3  1996/03/29  04:05:15  sjacobso
 * CSCdi52266:  4E/8E receive promiscuously, hurting performance
 * Branch: California_branch
 * Change write_address_filter code to examine address filter
 * entries for important status, and to store that status in the
 * software address filter.  The correct common routines for
 * adding MAC addresses are called.  Also, fix Predator logical
 * address filter code to work correctly with HSRP addresses.
 *
 * Revision 3.10.6.2  1996/03/21  23:23:17  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.10.6.1  1996/03/18  21:34:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.17  1996/03/18  23:28:45  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.4.2.16  1996/03/07  17:28:59  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.4.2.15  1996/03/04  13:25:54  dcarroll
 * CSCdi49299:  Fix 4e frame miss count handling
 * Branch: ELC_branch
 * Correct input_error count for missed frames.
 *
 * Revision 3.4.2.14  1996/02/19  05:25:31  dcarroll
 * CSCdi49299:  Fix 4e frame miss count handling
 * Branch: ELC_branch
 * Disable miss frame counter overflow interrupt.
 *
 * Revision 3.4.2.13  1996/02/10  00:30:39  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.4.2.12  1996/01/31  12:54:45  dcarroll
 * Branch: ELC_branch
 * Fix use of amdp2 Logical Address Filter to allow multicast reception.
 *
 * Revision 3.4.2.11  1996/01/18  05:41:50  mbeesley
 * Branch: ELC_branch
 * Optimize 1fe and 4e driver. Fix 4e fastsend bug. Patch in Tx enable
 * fix for 1fe. Debug IP fastswitching.
 *
 * Revision 3.4.2.10  1996/01/17  23:37:42  dcarroll
 * Branch: ELC_branch
 * Make 4e fully tx polled.
 *
 * Revision 3.4.2.9  1996/01/16  01:42:43  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.4.2.8  1996/01/03  06:10:02  wfried
 * Branch: ELC_branch
 * Add temporary HW bug workaround to allow correct PCI arbitration and
 * selection of MP EEPROM device.
 *
 * Revision 3.4.2.7  1995/12/22  05:28:05  wfried
 * Branch: ELC_branch
 * Ensure that Presidio chip is working in 32-bits mode, fix pci devices
 * discovery mechanism and remove write to read-only register (PCI latency).
 *
 * Revision 3.4.2.6  1995/12/12  04:53:57  mbeesley
 * Branch: ELC_branch
 * Allow "show pci bridge" to display all bridges. Change the 1fe GPC
 * register settings for Predator IO card 1fe. Fix some amdp2 error
 * messages. And turn power off on unrecognized port adaptors.
 *
 * Revision 3.4.2.5  1995/12/11  19:13:45  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.4.2.4  1995/12/05  06:35:55  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.4.2.3  1995/12/04  22:18:18  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.4.2.2  1995/11/30  02:04:54  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.4.2.1  1995/11/22  21:41:52  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.10  1996/03/06  08:42:04  feisal
 * CSCdi50820:  Support for 5E port adapters on VIP1
 *
 * Revision 3.9  1995/12/19  20:24:22  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.8  1995/12/17  18:35:53  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.7  1995/12/06  18:31:38  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.6  1995/12/01  02:00:00  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.5  1995/11/28  02:26:05  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.4  1995/11/20  04:23:58  mbeesley
 * CSCdi43892:  AMDP2 driver can produce runt frames
 *
 * Revision 3.3  1995/11/17  18:54:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:21:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:57:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:32:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
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

#include "../smf/smf_public.h"

#include "if_pas.h"
#include "if_pas_amdp2.h"
#include "msg_amdp2.c"			/* Not a typo, see logger.h */
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "pas_id.h"

/*
 * amdp2_sanitize_rxpak:
 * Sanitize the rx paktype used by the driver
 */
static void amdp2_sanitize_rxpak (amdp2_instance_t *ds)
{
    paktype *rx_pak = ds->rx_pak;

    if (rx_pak) {
	datagram_done(rx_pak);
    }
}

/*
 * amdp2_set_rx_discard :
 * Control ds->rx_discard and flush rx_pak appropriately.
 */
void amdp2_set_rx_discard (amdp2_instance_t *ds, boolean state)
{
    if (!state) {
	/*
	 * Clearing rx_discard. Sanitize rx_pak.
	 */
	amdp2_sanitize_rxpak(ds);
    }
    ds->rx_discard = state;
}

/*
 * amdp2_throttle:
 * Throttle the Presidio2 interface
 */
void amdp2_throttle (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);

    idb_mark_throttled(idb);
    ds->throttled += 1;

    amdp2_init(idb, FALSE);
    if (idb->counters.resets > 0) {
	idb->counters.resets--;
    }
}

/*
 * amdp2_stop:
 * Stop the Presidio ethernet controller
 */
static void amdp2_stop (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    leveltype level;
    ulong phy_addr;

    level = raise_interrupt_level(NETS_DISABLE);
    ds->amdp2_regaddr->rap = AMDP2_CSR0;
    ds->amdp2_regaddr->rdp = AMDP2_CSR0_STOP;
    DELAY(AMDP2_STOP_DELAY);

    /*
     * Give the chip the initblock address
     */
    phy_addr = AMDP2_CNTL_ADDR(ds->ib);
    ds->amdp2_regaddr->rap = AMDP2_CSR1;
    ds->amdp2_regaddr->rdp = (ushort)(phy_addr & 0xffff);
    ds->amdp2_regaddr->rap = AMDP2_CSR2;
    ds->amdp2_regaddr->rdp = (ushort)(phy_addr >> 16);

    /* 
     * Setup CSR4
     */
    ds->amdp2_regaddr->rap = AMDP2_CSR4;
    ds->amdp2_regaddr->rdp = 
	(AMDP2_CSR4_DMAPLUS|AMDP2_CSR4_TXSTRTM|AMDP2_CSR4_AUTO_PAD_TX|
	 AMDP2_CSR4_MFCOM|AMDP2_CSR4_RCVCCOM);

    /*
     * Setup CSR80
     */
    ds->amdp2_regaddr->rap = AMDP2_CSR80;
    ds->amdp2_regaddr->rdp = 
	(AMDP2_RXFIFO_16 | AMDP2_TXFIFO_START_128 | AMDP2_TXFIFO_WMARK_16);
    
    /*
     * Leave rap pointing at CSR0 and re-enable interrupts
     */
    ds->amdp2_regaddr->rap = AMDP2_CSR0;
    reset_interrupt_level(level);
}

/*
 * amdp2_shutdown:
 * Shutdown a Presidio2 interface
 */
static void amdp2_shutdown (hwidbtype *idb, int newstate)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    leveltype level;

    net_cstate(idb, newstate);
    level = raise_interrupt_level(NETS_DISABLE);
    if (ds) {
	amdp2_stop(idb);
	(*idb->reset)(idb);
    }
    reset_interrupt_level(level);
}

/*
 * amdp2_linestate:
 * Return what the driver thinks is the carrier state of this ethernet
 */
static boolean amdp2_linestate (hwidbtype *idb)
{
    return(idb->ethernet_lineup);
}

/*
 * amdp2_reset:
 * Reset the Presidio2 chip
 */
static void amdp2_reset (hwidbtype *idb)
{
    leveltype level;
    amdp2_instance_t *ds = INSTANCE(idb);
    volatile ulong rd_reg;

    level = raise_interrupt_level(NETS_DISABLE);
    rd_reg = ds->amdp2_regaddr->reset;
    usecdelay(AMDP2_RESET_DELAY);

    /*
     * Force AMDP2 to work as a 32 bits interface
     * by writing ZERO into the data reg (rdp).
     */
    ds->amdp2_regaddr->rdp = 0;

    /*
     * Setup the DMA endianness
     */
    ds->amdp2_regaddr->rap = AMDP2_CSR3;
    rd_reg = (AMDP2_CSR3_DXSUFLO | AMDP2_CSR3_MISSM);
    if (AMDP2_DMA_BIGENDIAN) {
	ds->amdp2_regaddr->rdp = (rd_reg | AMDP2_CSR3_BSWP);
    } else {
	ds->amdp2_regaddr->rdp = rd_reg;
    }

    /* 
     * Setup CSR4
     */
    ds->amdp2_regaddr->rap = AMDP2_CSR4;
    ds->amdp2_regaddr->rdp = 
	(AMDP2_CSR4_DMAPLUS|AMDP2_CSR4_TXSTRTM|AMDP2_CSR4_AUTO_PAD_TX|
	 AMDP2_CSR4_MFCOM|AMDP2_CSR4_RCVCCOM);

    /*
     * Set the rap pointer back to CSR0 and re-enable interrupts
     */
    ds->amdp2_regaddr->rap = AMDP2_CSR0;
    reset_interrupt_level(level);
}
    
/*
 * amdp2_restart:
 * Wrapper routine for idb->reset vector
 */
static void amdp2_restart (hwidbtype *idb)
{
    leveltype level;

    /*
     * Raise the interrupt level, flush the i/o holq queues
     * and re-initialize the device.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    holdq_reset(idb);
    if (idb->state != IDBS_ADMINDOWN) {
	amdp2_init(idb, FALSE);
    }
    reset_interrupt_level(level);
}

/*
 * amdp2_suspend:
 * Suspend the chip without stopping it. Must be called with interrupts
 * masked already.
 */
static boolean amdp2_suspend (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    ulong status, i;

    /*
     * Suspend the chip
     */
    ds->amdp2_regaddr->rap = AMDP2_CSR5;
    ds->amdp2_regaddr->rdp = AMDP2_CSR5_SUSPEND;
    ds->amdp2_regaddr->rap = AMDP2_CSR0;

    /*
     * Wait for it to suspend
     */
    for (i=0; i<AMDP2_MAX_SUSPEND_LOOP; i++) {

	ds->amdp2_regaddr->rap = AMDP2_CSR5;
	status = ds->amdp2_regaddr->rdp & AMDP2_CSR5_SUSPEND;
	ds->amdp2_regaddr->rap = AMDP2_CSR0;

	/*
	 * Did it suspend ?
	 */
	if (status) {
	    break;
	}
	DELAY(1);
    }

    if (i >= AMDP2_MAX_SUSPEND_LOOP) {
	if (ether_debug) {
	    buginf("\nAMDP2(%d): amdp2_suspend failed", idb->unit);
	}
	return(FALSE);
    }
    return(TRUE);
}

/*
 * amdp2_resume:
 * Resume a suspended chip. Must be called with interrupts masked.
 */
static void amdp2_resume (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    
    /*
     * Reset the suspend bit get ready to start again.
     */
    ds->amdp2_regaddr->rap = AMDP2_CSR5;
    ds->amdp2_regaddr->rdp &= ~(AMDP2_CSR5_SUSPEND);
    ds->amdp2_regaddr->rap = AMDP2_CSR0;
}

/*
 * amdp2_set_csr15_promiscuous_mode:
 * Set promiscuous mode by writing directly to CSR15.
 */
static void amdp2_set_csr15_promiscuous_mode (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    amdp2_regs_t *reg;
    ulong csr0, csr15;
    leveltype level;
 
    /*
     * Raise the interrupt level.
     */
    reg = ds->amdp2_regaddr;
    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * If the chip is stopped, set promiscuous mode.  If it isn't,
     * suspend the chip; if successfully suspended, set promiscuous mode.
     */
    reg->rap = AMDP2_CSR0;
    csr0 = reg->rdp;

    if (csr0 & AMDP2_CSR0_STOP) {
        reg->rap = AMDP2_CSR15;
        csr15 = reg->rdp & 0xFFFF;
        reg->rdp = csr15 | AMDP2_MODE_PROM;
    } else {
        if (amdp2_suspend(idb)) {
            reg->rap = AMDP2_CSR15;
            csr15 = reg->rdp & 0xFFFF;
            reg->rdp = csr15 | AMDP2_MODE_PROM;

            amdp2_resume(idb);
        }
    }

    /*
     * Restore RAP to CSR0 and re-enable interrupts.
     */
    reg->rap = AMDP2_CSR0;
    reset_interrupt_level(level);
}

/*
 * amdp2_show_regs:
 * Print out the AMDP2 registers.
 */
static void amdp2_show_regs (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    amdp2_regs_t *reg;
    ulong csr0, crda, cxda, csr3, csr4;
    ulong csr8, csr9, csr10, csr11, csr12;
    ulong csr13, csr14, csr15;
    leveltype level;

    /*
     * Raise the interrupt level and suspend the chip
     */
    reg = ds->amdp2_regaddr;
    level = raise_interrupt_level(NETS_DISABLE);
    amdp2_suspend(idb);

    /*
     * Take a snapshot of the registers
     */
    reg = ds->amdp2_regaddr;
    reg->rap = AMDP2_CSR0;
    csr0 = reg->rdp & 0xFFFF;

    /*
     * CSR3, CSR4
     */
    reg->rap = AMDP2_CSR3;
    csr3 = reg->rdp & 0xFFFF;
    reg->rap = AMDP2_CSR4;
    csr4 = reg->rdp & 0xFFFF;

    /*
     * CRDA
     */
    reg->rap = AMDP2_CSR28;
    crda = reg->rdp & 0xffff;
    reg->rap = AMDP2_CSR29;
    crda |= (reg->rdp << 16);

    /*
     * CXDA
     */
    reg->rap = AMDP2_CSR34;
    cxda = reg->rdp & 0xFFFF;
    reg->rap = AMDP2_CSR35;
    cxda |= (reg->rdp << 16);

    /*
     * CSR15
     */
    reg->rap = AMDP2_CSR15;
    csr15 = reg->rdp & 0xFFFF;

    /*
     * Logical address filter
     */
    reg->rap = AMDP2_CSR11;
    csr11 = reg->rdp & 0xFFFF;
    reg->rap = AMDP2_CSR10;
    csr10 = reg->rdp & 0xFFFF;
    reg->rap = AMDP2_CSR9;
    csr9 = reg->rdp & 0xFFFF;
    reg->rap = AMDP2_CSR8;
    csr8 = reg->rdp & 0xFFFF;

    /*
     * The physical address, flip bytes for clear output
     */
    reg->rap = AMDP2_CSR12;
    csr12 = ((reg->rdp & 0xFF)<<8) + ((reg->rdp & 0xFF00)>>8);
    reg->rap = AMDP2_CSR13;
    csr13 = ((reg->rdp & 0xFF)<<8) + ((reg->rdp & 0xFF00)>>8);
    reg->rap = AMDP2_CSR14;
    csr14 = ((reg->rdp & 0xFF)<<8) + ((reg->rdp & 0xFF00)>>8);

    /*
     * Resume the chip
     */
    amdp2_resume(idb);

    /*
     * Restore RAP to CSR0 and re-enable interrupts
     */
    reg->rap = AMDP2_CSR0;
    reset_interrupt_level(level);

    printf(" CSR0=%#x, CSR3=%#x, CSR4=%#x, CRDA=%#x, CXDA=%#x\n", 
	   csr0, csr3, csr4, crda, cxda);
    printf(" CSR15=%#x\n", csr15);
    printf(" HW filtering information:\n");
    printf("  Promiscuous Mode %s, PHY Addr %s, Broadcast Addr %s\n",
	   (csr15 & AMDP2_MODE_PROM ? "Enabled" : "Disabled"),
	   (csr15 & AMDP2_MODE_DRCVPA ? "Disabled" : "Enabled"),
	   (csr15 & AMDP2_MODE_DRCVBC ? "Disabled" : "Enabled"));
    printf("  PHY Addr=%-04x.%-04x.%-04x, Multicast Filter=0x%-04x 0x%-04x 0x%-04x 0x%-04x\n", 
	   csr12, csr13, csr14, csr11, csr10, csr9, csr8);
}

/*
 * amdp2_show_controller:
 * Print out info about this Presidio interface. Return TRUE if the output
 * terminated abnormally.
 */
static boolean amdp2_show_controller (hwidbtype *idb, parseinfo *csb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    amdp2_instance_t *ds_snap;
    leveltype level;

    printf("\nInterface %s\n", idb->hw_namestring);
    printf("Hardware is AMD Presidio2\n");

    printf("ADDR: %x, FASTSEND: %x, MCI_INDEX: %x\n", idb, idb->fastsend, idb->mci_index);

    printf("DIST ROUTE ENABLED: %d\n", idb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE? 1:0);

    printf("Route Cache Flag: %x\n", idb->ip_routecache);


    if (!ds || !ds->ib_malloc) {
	printf("Not initialized\n");
	return(FALSE);
    }

    /*
     * Take a snapshot of the rx ring, rx shadow ring, tx ring, tx shadow
     */
    ds_snap = malloc(sizeof(amdp2_instance_t));
    if (!ds_snap) {
	return(TRUE);
    }

    /*
     * Lock out interrupts and copy the data structures. Then re-enable
     * network interrupts.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    bcopy(ds, ds_snap, sizeof(amdp2_instance_t));
    reset_interrupt_level(level);

    /*
     * Print the chips registers
     */
    amdp2_show_regs(idb);

    /*
     * Print out the addresses of some stuff
     */
    printf(" amdp2_ds=%#x, registers=%#x, ib=%#x\n",
	   ds, ds->amdp2_regaddr, ds->ib);
    printf(" rx ring entries=%d, tx ring entries=%d\n",
	   AMDP2_RX_RING_ENTRIES, AMDP2_TX_RING_ENTRIES);
    printf(" rxring=%#x, rxr shadow=%#x, rx_head=%d, rx_tail=%d\n",
	   ds_snap->rxr, ds_snap->rxr_shadow, ds_snap->rx_head, 
	   ds_snap->rx_tail);
    printf(" txring=%#x, txr shadow=%#x, tx_head=%d, tx_tail=%d, "
	   "tx_count=%d\n",
	   ds_snap->txr, ds_snap->txr_shadow, ds_snap->tx_head, 
	   ds_snap->tx_tail, ds_snap->tx_count);

    /*
     * Display the instance structure
     */
    amdp2_show_instance(ds_snap);

    /*
     * Release all memory
     */
    free(ds_snap);
    return(FALSE);
}    

/*
 * amdp2_ladrf_clear:
 * Clear the HW logical address filter variables in the init block.
 */
void amdp2_ladrf_clear (hwidbtype *idb)
{
    amdp2_initblock_t *ib;
    amdp2_instance_t *ds;
    int i;

    ds = INSTANCE(idb);
    ib = ds->ib;

    for (i = 0; i < 4; i++) {
	ib->mc_filter_bits[i] = 0x0;
    }

    /*
     * Clear HSRP configured boolean.  If true, this variable
     * causes us to run in promiscuous mode.
     */
    ds->hsrp_configured = FALSE;
	
    ds->need_af_check = FALSE;	
}

/*
 * amdp2_add_multicast_address:
 * Add another ethernet multicast address to the 
 * AMD presidio2 chip's Logical Address Filter.
 *
 * The amdp2 chip uses 64 bits of address filtering.
 * A crc is done on the incomming packet's destination
 * mac address. The high order 6 bits of the crc are used
 * to index into the address filter. If the bit is set
 * then the frame is accepted and processed. Else it is
 * discarded.
 *
 * This routine was stolen (like so much else) from the lance
 * driver in the les directory. The mac address and the
 * polynomial are both flipped in this routine before the
 * crc is calculated. This has no effect other than causing
 * the least significant 6 bits of the crc to be used rather
 * than the most significant bits.
 *
 * Note that the HSRP unicast address is also passed down to the
 * driver through this vector.  The code differentiates by examining
 * the multicast bit of the MAC address.
 */
void amdp2_add_multicast_address (idbtype *swidb, uchar *mac, 
					 uchar fsm)
{
    /*
     * Format of multicast_address on the wire:
     *
     *   47                                                   2   1   0
     *  +---+-----------------------------------------------+---+---+---+
     *  |   |                                               |   | L | M |
     *  +---+-----------------------------------------------+---+---+---+
     *		L = locally administered bit
     *		M = Multicast bit
     *
     * Format of multicast_address in memory:
     *
     *                                   L    M
     *	+----+----+----+----+----+----+----+----+
     *	| 07 | 06 | 05 | 04 | 03 | 02 | 01 | 00 |  byte 0
     *	+----+----+----+----+----+----+----+----+
     * 
     *	+----+----+----+----+----+----+----+----+
     *	| 15 | 14 | 13 | 12 | 11 | 10 | 09 | 08 |  byte 1
     *	+----+----+----+----+----+----+----+----+
     *			...
     *	+----+----+----+----+----+----+----+----+
     *	| 47 | 46 | 45 | 44 | 43 | 42 | 41 | 40 |  byte 5
     *	+----+----+----+----+----+----+----+----+
     */

    ushort *wptr, addrw, bitmask, addrbit = 0, crcbit;
    uchar wire_address[IEEEBYTES], *cptr;
    int i, j, t, bitshift;
    uint crc;
    hwidbtype *idb;
    amdp2_initblock_t *ib;
    amdp2_instance_t *ds;
    amdp2_regs_t *reg;
    leveltype status;
    ulong filter_index, filter_bit, filter_word;
    ulong csr0;

    status = raise_interrupt_level(NETS_DISABLE);
    idb = swidb->hwptr;
    ds = INSTANCE(idb);
    ib = ds->ib;
    reg = ds->amdp2_regaddr;

 
    /*
     * If this is a multicast address, program it into the amdp2 LADRF.
     * Else configure the chip for HSRP.
     */
    if (mac[0] & 0x01) {
        /*
         * Reverse the bits in each byte of the ethernet address so
         * that the bit order is the same as it will be on the wire:
         */
        cptr = mac;
        ieee_swap(cptr, wire_address);
 
        /*
         * Implement the algorithm as published in BASIC in the manual:
         */
        crc = -1;
        wptr = (ushort *)wire_address;
        for (i = 0; i < 3; i++) {
            addrw = *wptr++;
            bitmask = 0x8000;
            bitshift = 15;
            for (j = 0; j < 16; j++) {
                crcbit = (crc & 0x80000000) >> 31; /* crcbit = c(32) */
                crc = crc << 1;             /* left shift crc by 1 */
                addrbit = (addrw & bitmask) >> bitshift; /* get a(n) bit */
                t = crcbit ^ addrbit;    /* t = c(32) xor a(n) : control bit */
                if (t) {
                    crc = crc ^ 0x04C11DB6;
                    crc = crc | 1;          /* c(0) = 1 */
                }
                bitmask = bitmask >> 1;
                bitshift = bitshift -1;
            }
        }

        /*
         * Compute the hash number. Hashing function is the bottom
         * six bits of the crc. Use these to index into the 64
         * bits of filtering (4 ushorts).
         */
        filter_index = 0;
        filter_bit = 32;

        for (i = 0; i < 6; i++) {
            if (crc & 1)
                filter_index = filter_index + filter_bit;
            filter_bit = filter_bit >> 1;
            crc = crc >> 1;
        }

        /*
         * Compute the word and bit offset. Notice that the
         * halfword offset must be swapped within the word
         * address to get to the right place.
         */
        filter_word = filter_index >> 4;
        filter_word = filter_word ^= 1;
        filter_bit = filter_index & 0xf;
        ib->mc_filter_bits[filter_word] |= (1 << filter_bit);

        /*
         * Update the AMD's multicast registers (csr8-csr11) to match
         * to the values in the init table (ib->mc_filter_bits).
         * Assume that rap is already set to CSR0, since this is the
         * convention for this driver.
         */
        csr0 = reg->rdp;

        if (csr0 & AMDP2_CSR0_STOP) {
            reg->rap = AMDP2_CSR8 + (filter_word ^ 1);
            reg->rdp = ib->mc_filter_bits[filter_word];
        } else {
            /*
             * Chip is not stopped, so suspend it before
             * modifying logical (multicast) address registers.
             */
            if (amdp2_suspend(idb)) {
                reg->rap = AMDP2_CSR8 + (filter_word ^ 1);
                reg->rdp = ib->mc_filter_bits[filter_word];
                amdp2_resume(idb);
            }
        }
        reg->rap = AMDP2_CSR0;
        
    } else {
        /*
         * Not multicast; this is the HSRP address.  Since the amdp2
         * can only filter on a single unicast address, cause it to
         * be placed in promiscuous mode.
         */
        ds->hsrp_configured = TRUE;
	ds->need_af_check = TRUE;
        ib->mode |= AMDP2_MODE_PROM;
        amdp2_set_csr15_promiscuous_mode(idb);
    }

    reset_interrupt_level(status);
}

/*
 * amdp2_add_multicast_range:
 * Add multicast address range to the logical address filter. 
 */
void amdp2_add_multicast_range (idbtype *swidb, uchar *address, uchar *mask)
{
    amdp2_initblock_t *ib;
    amdp2_instance_t *ds;
    hwidbtype *idb;
    int i;

    idb = swidb->hwptr;
    ds = INSTANCE(idb);
    ib = ds->ib;

    /*
     * Since even a small multicast address range will fill the
     * hardware address filter, cause the chip to accept all multicast
     * addresses.
     */
    for (i = 0; i < 4; i++) {
	ib->mc_filter_bits[i] = 0xffff;
    }
}

/*
 * amdp2_setup_bridging:
 * Set up for bridging on this interface. Actually the init code does it.
 */
static void amdp2_setup_bridging (hwidbtype *idb)
{
    amdp2_init(idb, FALSE);
    if (idb->counters.resets > 0) {
	idb->counters.resets--;
    }
}

/*
 * amdp2_check:
 * If it should be running, read CSR0 and ensure that it is
 * running and has a normal value. Otherwise, reset it.
 */
static void amdp2_check (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    leveltype level;
    ulong csr0;

    if (idb_is_throttled(idb))
	return;

    if (!ds)
	return;
    
    /*
     * Raise interrupt level and read status register
     */
    level = raise_interrupt_level(NETS_DISABLE);
    ds->amdp2_regaddr->rap = AMDP2_CSR0;
    csr0 = ds->amdp2_regaddr->rdp;
    reset_interrupt_level(level);
    
    /*
     * Check the status register
     */
    if ((csr0 & AMDP2_CSR0_STOP) ||
	(csr0 & AMDP2_CSR0_NORMAL) != AMDP2_CSR0_NORMAL) {
	if (ether_debug) {
	    buginf("\nAMDP2(%d): Bad state in CSR0=(0x%x)\n", idb->unit, csr0);
	}
	amdp2_init(idb, FALSE);
    }
}

/*
 * amdp2_txqlength:
 * Return the number of packetson the tx ring
 */
static int amdp2_txqlength (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);

    /*
     * Note: we are not able to support this exactly, as the tx_count
     * field is the number of particle buffers queued on the tx ring rather
     * than the number of packets. But this should be close enough.
     */
    return(ds->tx_count);
}

/*
 * amdp2_enable:
 * Enable a Presidio2 interface.
 */
static void amdp2_enable (hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);

    idb_mark_unthrottled(idb);
    ds->enabled++;
    amdp2_init(idb, FALSE);
    if (idb->counters.resets > 0) {
	idb->counters.resets--;
    }
}

/*
 * amdp2_periodic:
 * Periodic activities for Presidio device driver
 */
static void amdp2_periodic (hwidbtype *idb)
{
    ethersb	*esb;

    esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);
    /*
     * Force lineup if no keepalives are configured or we are shutdown.
     * Otherwise, send a keepalive. We cannot hear ourself like an MCI does,
     * but amdp2_tx_interrupt will clear ethernet_lineup if we get a lost
     * carrier indication back from the chip. If we do not get a lost
     * carrier indication, amdp2_tx_interrupt will set ethernet_lineup. If
     * amdp2_rx_interrupt receives a packet, ethernet_lineup will be set.
     */
    if (AMDP2_LOCAL_KEEPALIVE) {
	if (idb->nokeepalive || idb->state == IDBS_ADMINDOWN) {
	    if (!idb->ethernet_lineup) {
		idb->ethernet_lineup = TRUE;
		if (idb->state != IDBS_ADMINDOWN) {
		    idb->reason = "Checking disabled";
		    amdp2_snmp_link_trap(AMDP2_LINK_UP, idb);
		}
	    }
	} else {
	    send_loop(idb, idb->hardware);
	}
    }

    /*
     * Check the device, specifically CSR0
     */
    if (idb->state == IDBS_UP) {
	amdp2_check(idb);
    }

    idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
}

/*
 * amdp2_8e_analyze_pa:
 * Init the pci to pci bridge above the port adaptor, then discover devices
 * behind the bridge on the port adaptor.
 */
static void amdp2_8e_analyze_pa (int pa_bay)
{
    ulong count, expected_count;
    uchar bus_no;

    bus_no = get_pci_bus_no(pa_bay);

    /*
     * Figure out the expected interface count
     */
    switch (pas_get_pa_type(pa_bay)) {
    case PA_HARDWARE_8E: 
	expected_count = 8; break;
    case PA_HARDWARE_4E: 
	expected_count = 4; break;
    case PA_HARDWARE_5E: 
	expected_count = 5; break;
    default: 
	expected_count = 0; break;
    }

    /*
     * Discover and initialize devices on the PA
     * For now, avoid the problem where config reads find devices for every
     * device number regardless of there being a real hardware interface or
     * not (6E seems to have the same problem) by passing the exact number
     * of expected devices to the discovery function.
     */
    count = pci_discover_devices(bus_no, expected_count, pa_bay); 
    if (count != expected_count) {
	errmsg(&msgsym(DISCOVER, AMDP2), count, pa_bay);
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
    pas_install_wrapper(pa_bay, amdp2_eim_wrapper);
}

/*
 * amdp2_init_idb:
 * IDB init routine for AMDP2 interfaces
 */
static boolean amdp2_init_idb (hwidbtype *idb, amdp2_regs_t *base_addr)
{
    idbtype *swidb = idb->firstsw;
    amdp2_instance_t *ds;

    /*
     * If this is the first time through this routine
     * for this idb, then initialize the following.
     * For idbs off the removed queue, this initialization
     * should not be repeated.
     */
    if (idb->oir_flags & IDB_FIRST_INIT) {
	idb->status = IDB_ETHER | IDB_BCAST | IDB_BRIDGE;
	idb->type = IDBTYPE_AMDP2;
	idb->typestring = "AmdP2";
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
	idb->soutput = amdp2_tx_start;
	idb->reset = amdp2_restart;
	idb->linestate = amdp2_linestate;
	idb->shutdown = amdp2_shutdown;
	idb->periodic = amdp2_periodic;
	idb->listen = amdp2_add_multicast_address;
	idb->listen_range = amdp2_add_multicast_range;
	idb->enable = amdp2_enable;
	idb->setup_bridging = amdp2_setup_bridging;
	swidb->span_disabled = FALSE;
	idb->txqlength = amdp2_txqlength;
	idb->show_controller = amdp2_show_controller;
	idb->fastsend = amdp2_fastsend;

	ether_idb_init(idb);
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
    ds = malloc_fast(sizeof(amdp2_instance_t));
    if (!ds) {
	return(FALSE);
    }
    INSTANCE(idb) = (void *)ds;
    ds->amdp2_regaddr = base_addr;
    pas_instance_init_common(&ds->pacb, idb);

    /*
     * Setup the removal vector here so that if the init process
     * fails from here on in, there will be a device specific vector
     * defined to clean up resources
     */
    ds->pacb.pas_deallocate = amdp2_deallocate;

    /*
     * Allocate init block
     */
    ds->ib_malloc = malloc_named_pcimem(sizeof(amdp2_initblock_t),
					idb->hw_namestring);
    if (!ds->ib_malloc) {
	return(FALSE);
    }
    ds->ib = ds->ib_malloc;

    /*
     * Allocate rx ring and rx ring shadow
     */
    ds->rxr_malloc = malloc_named_pcimem(sizeof(amdp2_rx_desc_t) * 
					 AMDP2_RX_RING_ENTRIES,
					 idb->hw_namestring);
    if (!ds->rxr_malloc) {
	return(FALSE);
    }
    ds->rxr = pci_memaddr_convert(ds->rxr_malloc, AMDP2_RX_RING_ATTRIBUTE);
    ds->rxr_shadow = malloc_fast(sizeof(particletype *) * 
				 AMDP2_RX_RING_ENTRIES);
    if (!ds->rxr_shadow) {
	return(FALSE);
    }
    amdp2_rxring_flush(ds);

    /*
     * Allocate tx ring and tx ring shadow
     */
    ds->txr_malloc = malloc_named_pcimem(sizeof(amdp2_tx_desc_t) * 
					 AMDP2_TX_RING_ENTRIES,
					 idb->hw_namestring);
    if (!ds->txr_malloc) {
	return(FALSE);
    }
    ds->txr = ds->txr_malloc;
    ds->txr_shadow = malloc_fast(sizeof(amdp2_tx_shadow_t) * 
				 AMDP2_TX_RING_ENTRIES);
    if (!ds->txr_shadow) {
	return(FALSE);
    }

    /*
     * Initialize the logical address filter values in the init block,
     * and clear HSRP configured boolean.
     */
    amdp2_ladrf_clear(idb);

    /*
     * Do platform dependent initialization
     */
    if (!amdp2_platform_init(idb)) {
	return(FALSE);
    }

    /*
     * Successfully got all resources
     */
    return(TRUE);
}

/*
 * amdp2_init:
 * Initialize the hardware interface
 */ 
boolean amdp2_init (hwidbtype *idb, boolean maxdgram_change)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    amdp2_rx_desc_t *rmd;
    amdp2_tx_desc_t *tmd;
    amdp2_initblock_t *ib;
    leveltype level;
    particletype *mp;
    ethersb	*esb;
    int i;
    ushort hw_station[3];

    /*
     * Raise the interrupt level to protect from race conditions
     */
    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Stop the Presidio chip and count the reset
     */
    ds->rx_miss_count = 0;
    ds->fatal_tx_err = FALSE;
    amdp2_stop(idb);
    idb->counters.resets++;
    if (idb->state != IDBS_ADMINDOWN) {
	net_cstate(idb, IDBS_RESET);
    }

    /*
     * Flush the hold queue
     */
    holdq_reset(idb);
    esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);
    /*
     * If interface is administratively down, don't re-enable it.
     */
    if (idb->state == IDBS_ADMINDOWN) {
	idb->ethernet_lineup = TRUE;
	amdp2_lineproto_change(idb, TRUE);
	idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
	reset_interrupt_level(level);
	return(TRUE);
    }

    amdp2_stop(idb);
    amdp2_reset(idb);

    ds->rx_head = ds->rx_tail = 0;	/* Rx head, tail indexes */
    ds->tx_head = ds->tx_tail = 0;	/* Tx head, tail indexes */
    ds->tx_count = 0;			/* no buffers on Tx ring */
    
    ds->tx_limited = hwidb_use_short_tx_q_inline(idb);
    if (ds->tx_limited) {
	idb->mci_txcount = 1;
    } else {
	idb->mci_txcount = AMDP2_TX_RING_ENTRIES;
    }
    
    /*
     * Initialize the hardware addresses. *FIXME* This needs work.
     */
    amdp2_get_default_hw_address(idb, idb->bia);
    if (is_ieee_zero(idb->hardware)) {
	amdp2_get_default_hw_address(idb, idb->hardware);
    }
    bscopy(idb->hardware, (uchar *)hw_station, IEEEBYTES);
    ib = ds->ib;
    ib->phy_addr_15_0 = hw_station[0];
    ib->phy_addr_31_16 = hw_station[1];
    ib->phy_addr_47_32 = hw_station[2];

    /*
     * Initialize idb namestrings
     */
    if (!idb->hw_namestring) {
	idb_init_names(idb->firstsw, FALSE);
    }

    /*
     * Initialize private buffer pools
     */
    amdp2_init_buffer_pools(idb);

    /*
     * Setup the rx ring, tx ring addresses and sizes in the initblock
     */
    ib->rx_ring_addr = AMDP2_CNTL_ADDR(ds->rxr);
    ib->tx_ring_addr = AMDP2_CNTL_ADDR(ds->txr);
    ib->tlen_rlen = (AMDP2_TX_RING_ENTRIES_LOG2 << 12) | 
	(AMDP2_RX_RING_ENTRIES_LOG2 << 4);
    
    /*
     * Init the rx ring with particles owned by the presidio
     */
    for (i=0; i<AMDP2_RX_RING_ENTRIES; i++) {
	rmd = &ds->rxr[i];
	rmd->pkt_size = 0;
	rmd->buf_size = -(AMDP2_PARTICLE_SIZE);

	if (ds->rxr_shadow[i]) {
	    /*
	     * There is already a particle on the shadow rx ring.
	     * Give it to the Presidio.
	     */
	    mp = ds->rxr_shadow[i];
	    rmd->buf_addr = AMDP2_BUF_ADDR(mp->data_start);
	    rmd->status = AMDP2_RMD_OWN;
	} else {
	    /*
	     * There is no particle on the shadow rx ring, so 
	     * allocate it and give it to the Presidio
	     */
	    mp = amdp2_get_particle(idb);
	    if (mp) {
		ds->rxr_shadow[i] = mp;
		rmd->buf_addr = AMDP2_BUF_ADDR(mp->data_start);
		rmd->status = AMDP2_RMD_OWN;
	    } else {
		idb->ethernet_lineup = FALSE;
		net_cstate(idb, IDBS_DOWN);
		amdp2_lineproto_change(idb, FALSE);
		idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
		reset_interrupt_level(level);
		return(FALSE);
	    }
	}
    }
    amdp2_rxring_flush(ds);

    /*
     * Setup the tx ring and ensure that the CPU owns
     * all the buffers and/or particles
     */
    for (i=0; i<AMDP2_TX_RING_ENTRIES; i++) {

	tmd = &ds->txr[i];
	tmd->status2 = 0;
	tmd->cntl.s.status = 0;
	tmd->cntl.s.buf_size = 0;
	tmd->buf_addr = 0;
	if (ds->txr_shadow[i].pak && 
	    (ds->txr_shadow[i].pak != AMDP2_MAGIC_PAK_PTR)) {
	    datagram_done(ds->txr_shadow[i].pak);
	}
	if (ds->txr_shadow[i].mp) {
	    retparticle(ds->txr_shadow[i].mp);
	}
	ds->txr_shadow[i].pak = NULL;
	ds->txr_shadow[i].mp = NULL;
    }

    /*
     * Enable reception for appropriate addresses and address
     * groups. *FIXME* What is this ?
     */
    
    /*
     * If the interface is throttled, disable the receiver
     */
    ib->mode = 0;
    if (idb_is_throttled(idb)) {
	ib->mode |= AMDP2_MODE_DRX;
    }
    
    /*
     * Reset the Logical Address Filter to clean up any old
     * unwanted multicast addresses. Any wanted
     * multicast addresses will be picked back up in the
     * reg media interesting addresses call below.
     * 
     * Note that the logical address filter cannot be reset
     * here on RVIP or SVIP.  Hence the platform-specific macro.
     */
     RESET_AF_AND_UPDATE_MAC_ADDRESSES;

    /*
     * Enable promiscuous listening if bridging is configured on
     * this interface and all those other caveats apply.  Or if the
     * driver is configured to be always in promiscuous mode.
     */
    if (AMDP2_DEFAULT_PROMISCUOUS_MODE || ds->hsrp_configured ||
	 (idb->tbridge_on_hwidb && bridge_enable &&
	 !(system_loading | bootstrap_enable))) {
	ib->mode |= AMDP2_MODE_PROM;
    }

    /*
     * Tell the Presidio to read the initblock and initialize
     */
    ds->amdp2_regaddr->rap = AMDP2_CSR0;
    ds->amdp2_regaddr->rdp = AMDP2_CSR0_INIT;

    /* 
     * Poll to check that init is complete
     */
    for (i = AMDP2_MAX_INIT_POLL; i > 0; i--) {
	if (ds->amdp2_regaddr->rdp & AMDP2_CSR0_IDON) {
	    break;
	}
	DELAY(1);
    }
    if (i == 0) {
	errmsg(&msgsym(INITFAIL, AMDP2), 
	       idb->slot, idb->subunit, 0, ds->amdp2_regaddr->rdp);
	amdp2_shutdown(idb, IDBS_ADMINDOWN);
	idb->ethernet_lineup = FALSE;
	amdp2_lineproto_change(idb, FALSE);
	idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
	reset_interrupt_level(level);
	return(FALSE);
    }

    /*
     * Initialization succeeded. 
     * Set tx polling interval.
     * Start tx, rx and enable interrupts
     */
    ds->amdp2_regaddr->rap = AMDP2_CSR0;
    ds->amdp2_regaddr->rdp = AMDP2_CSR0_STOP;
    DELAY(AMDP2_STOP_DELAY);
    ds->amdp2_regaddr->rap = AMDP2_CSR47;
    ds->amdp2_regaddr->rdp = AMDP2_CSR47_POLLING_INTERVAL;

    ds->amdp2_regaddr->rap = AMDP2_CSR0;
    ds->amdp2_regaddr->rdp = AMDP2_CSR0_IDON | AMDP2_CSR0_IENA|AMDP2_CSR0_STRT;
    /*
     * Force the line state to be up as desired by the caller (from RSP).
     * If the carrier was gone, we'll detect that in next round of keepalive
     * check. (note: ethernet_lineup state must in sync with state we
     * call amdp2_lineproto_change with)
     */
    idb->ethernet_lineup = TRUE;
    net_cstate(idb, IDBS_UP);
    amdp2_lineproto_change(idb, TRUE);
    if (ether_debug) {
	buginf("%s: Interface is alive\n", idb->hw_namestring);
    }
    reset_interrupt_level(level);
    return(TRUE);
}

/*
 * amdp2_init_interface:
 * Initialize an AMDP2 PA interface
 */
static boolean amdp2_init_interface (uint slot, uint unit, uint subunit,
					amdp2_regs_t *base_addr)
{
    hwidbtype *idb;

    /*
     * Set up storage, add IDB to interface queue
     */
    idb = amdp2_idb_create(slot, subunit);
    if (!idb) {
	return(FALSE);
    }

    /*
     * Save our unit number and PA slot number:
     */
    idb->slot = slot;       /* Corresponds to IP */
    idb->unit = unit;       /* nth ethernet interface */
    idb->subunit = subunit; /* Corresponds to viper concept of interface # */
    idb->slotunit = subunit;/* For slotted interface names */
    pas_set_fci_type(idb, slot); /* Need fci type for OIR support */
    idb->status &= ~IDB_NOHARDWARE;

    /*
     * Initialize the idb
     */
    if (!amdp2_init_idb(idb, base_addr)) {
	return(FALSE);
    }

    /*
     * Initialize the hardware
     */
    if (!amdp2_init(idb, FALSE)) {
	return(FALSE);
    } else {
	idb->counters.resets = 0;
	return (TRUE);
    }
}

/*
 * amdp2_pci_init:
 * Do PCI specific initialization for a Presidio2 interface
 */
static amdp2_regs_t *amdp2_pci_init (uint pa_bay, uint device_no)
{
    uint value, id;
    amdp2_regs_t *device_base;
    uchar bus_no;

    /*
     * Get our bus no
     */
    bus_no = get_pci_bus_no(pa_bay);

    /*
     * Read the device/vendor registers to verify that this
     * is a Presidio2 Ethernet controller.
     */
    id = pci_config_read(bus_no, device_no, 0, PCI_VENDOR_ID_OFFSET);
    if (id != AMDP2_VENDOR_DEVICE_ID) {
	errmsg(&msgsym(NOTAMDP2, AMDP2), id);
	return(NULL);
    }

    /*
     * Get device base address
     */
    device_base = get_pci_device_base(pa_bay, device_no);

    /* 
     * Now do device specific configuration :
     * 	  - Set the IO offset for the device
     *    - Enable Memory Mapped I/O and Bus Master Mode
     * Note: Support the memory mapped I/O only
     */
    value = get_pci_device_base_offset(pa_bay, device_no);
    pci_config_write(bus_no, device_no, 0, PCI_MMIO_BASE_OFFSET, value);
    pci_config_write(bus_no, device_no, 0, PCI_COMMAND_REG_OFFSET,
                     PCI_COMMAND_BMEN|PCI_COMMAND_MEMEN);
    pci_config_write(bus_no, device_no, 0, PCI_LATENCY_OFFSET,
		     AMDP2_LAT_TIMER);

    /*
     * Force AMDP2 to work as a 32 bits interface
     * by writing ZERO into the data reg (rdp).
     */
    ((amdp2_regs_t *)device_base)->rdp = 0;

    return(device_base);
}

/*
 * amdp2_analyze_device:
 * Analyze an AMDP2 interface
 */
static boolean amdp2_analyze_device (uint pa_bay, uint device_no)
{
    amdp2_regs_t *device_base;

    /*
     * Perform PCI configuration specific to the device
     */
    device_base = amdp2_pci_init(pa_bay, device_no);
    if (!device_base) {
	return(FALSE);
    }

    /* 
     * Initialize this device
     */
    if (!amdp2_init_interface(pa_bay, nethernets, device_no, device_base)) {
	return(FALSE);
    } else {
	nethernets += 1;
	return(TRUE);
    }
}

/*
 * amdp2_rx_error:
 * Process a receive error, *FIXME* This all needs work.
 */
void amdp2_rx_error (ushort rmd_status, hwidbtype *idb, particletype *mp)
{
    amdp2_instance_t *ds = INSTANCE(idb);

    if (rmd_status & AMDP2_RMD_ENP) {

	/* 
	 * Framing error
	 */
	if ((rmd_status & AMDP2_RMD_FRAM) && 
	    (!(rmd_status & AMDP2_RMD_OFLO))) {
	    idb->counters.input_frame++;
	    ds->rx_overflow_err++;
	}
	/*
	 * Input CRC error
	 */
	if ((rmd_status & AMDP2_RMD_CRC) && 
	    (!(rmd_status  & AMDP2_RMD_OFLO))) {
	    idb->counters.input_crc++;
	}
	/*
	 * Bus parity error. *FIXME* this is fatal.
	 */
	if (rmd_status & AMDP2_RMD_BPE) {
	    ds->rx_bpe_err++;
	}
		  
    } else {
	
	/*
	 * Out of rx particles
	 */
	if (rmd_status & AMDP2_RMD_BUFF) {
	    idb->counters.input_overrun++;
	    ds->rx_buffer_err++;
	}

	/*
	 * Rx FIFO overflowed
	 */
	if (rmd_status & AMDP2_RMD_OFLO) {
	    ds->rx_overflow_err++;
	    if (rmd_status & AMDP2_RMD_BPE)
		/* Bus parity error also valid when OFLO set */
		ds->rx_bpe_err++;
	}
    }

    /*
     * Account for the error
     */
    idb->counters.input_error++;

    /*
     * Clean up. If it was the last particle in the packet, clear 
     * rx_discard, otherwise set it.
     */
    retparticle(mp);
    if (rmd_status & AMDP2_RMD_ENP) {
	amdp2_set_rx_discard(ds, FALSE);
    } else {
	amdp2_set_rx_discard(ds, TRUE);
    }
}

/*
 * amdp2_tx_error:
 * Handle an error on a transmit. FIXME: fatal tx errors ?
 */
void amdp2_tx_error (ushort tmd_status, uint tx_index, hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    amdp2_tx_desc_t *tmd;
    ulong tmd_status2;
    ethersb	*esb;

    tmd = &ds->txr[tx_index];
    tmd_status2 = tmd->status2;
    esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);

    /*
     * One collision
     */
    if ((tmd_status & AMDP2_TMD_ONE) && (tmd_status & AMDP2_TMD_ENP) &&
	(!(tmd_status2 & AMDP2_TMD0_LCOL))) {
	esb->output_one_collision++;
	ds->tx_one_col_err++;
    }

    /*
     * More than one collision
     */
    if ((tmd_status & AMDP2_TMD_MORE) && (tmd_status & AMDP2_TMD_ENP)) {
	esb->output_more_collision++;
	ds->tx_more_col_err++;
    }

    /*
     * Output deferred
     */
    if (tmd_status & AMDP2_TMD_DEF) {
        esb->output_deferred++;
	ds->tx_deferred_err++;
    }

    /*
     * tmd_status2 errors
     */
    if (tmd_status2 & AMDP2_TMD0_ERR_SUMMARY) {

	idb->counters.output_error++;

	/*
	 * Tx buffer error
	 */
	if ((tmd_status2 & AMDP2_TMD0_BUFF) &&
	    (!(tmd_status2 & (AMDP2_TMD0_LCOL|AMDP2_TMD0_RTRY)))) {
	    ds->tx_buff_err++;
	    ds->fatal_tx_err = TRUE;
	    idb->failCause = Ether_FatalTxError;
	}

	/*
	 * Tx fifo underrun
	 */
	if (tmd_status2 & AMDP2_TMD0_UFLO) {
	    idb->counters.output_underrun++;
	    errmsg(&msgsym(UNDERFLO, AMDP2), idb->slot, idb->subunit);
	    ds->fatal_tx_err = TRUE;
	    ds->tx_underrun_err++;
	    idb->failCause = Ether_FatalTxError;
	}

	/*
	 * Late collision
	 */
	if (tmd_status2 & AMDP2_TMD0_LCOL) {
	    esb->output_late_collision++;
	    ds->tx_late_collision_err++;
	    errmsg(&msgsym(LATECOLL, AMDP2), idb->slot, idb->subunit);
	    ds->fatal_tx_err = TRUE;
            idb->failCause = Ether_LateCollision;
	}
	
	/*
	 * Lost carrier
	 */
	if (tmd_status2 & AMDP2_TMD0_LCAR) {
	    esb->output_lost_carrier++;
	    ds->tx_loss_carrier_err++;

	    if (AMDP2_LOCAL_KEEPALIVE) {
		if (!idb->nokeepalive) {
		    if (idb->ethernet_lineup == TRUE) {
			idb->ethernet_lineup = FALSE;
			idb->reason = "Keepalive failed";
			amdp2_snmp_link_trap(AMDP2_LINK_DOWN, idb);
		    }
		}
	    } else {
		if (idb->ethernet_lineup == TRUE) {
		    idb->ethernet_lineup = FALSE;
		    idb->reason = "Keepalive failed";
		    amdp2_lineproto_change(idb, FALSE);
		}
	    }

	    if (ether_debug) {
		errmsg(&msgsym(LOSTCARR, AMDP2), idb->slot, idb->subunit);
	    }
	    idb->failCause = Ether_LostCarrier;
	}

	/*
	 * Excessive collisions
	 */
	if (tmd_status2 & AMDP2_TMD0_RTRY) {
	    ushort tdr;
	    ushort trc;

	    tdr = (tmd_status2 & AMDP2_TMD0_TDR) >> AMDP2_TDR_SHIFT;
	    trc = (tmd_status2 & AMDP2_TMD0_TRC);
	    esb->output_excessive_collision++;
	    ds->tx_exc_collision_err++;
	    errmsg(&msgsym(COLL, AMDP2), idb->slot, idb->subunit, tdr, trc);
            idb->failCause = Ether_ExcessiveCongestion;
	}
    }
    
    /*
     * Clean up the descriptor
     */
    tmd->status2 = 0;
    idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
}

/*
 * amdp2_error_interrupt:
 * Handle error interrupts from the Presidio chip
 */
void amdp2_error_interrupt (ulong csr0_status, hwidbtype *idb)
{
    amdp2_instance_t *ds = INSTANCE(idb);
    ethersb	*esb;
    ulong temp_miss_count;

    esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);
    if (csr0_status & AMDP2_CSR0_ERR) {
	/*
	 * Output Babbles
	 */
	if (csr0_status & AMDP2_CSR0_BABL) {
	    /*
	     * Disable the babble error message for now (except when debugging)
	     * as the P2 generates a lot of false babbles because of a H/W bug.
	     * Also stop counting babble errors (idb->output_error++;
	     * esb->output_babbles++;).
	     */
	    if (ether_debug) {
		errmsg(&msgsym(BABBLE, AMDP2), 
		   idb->slot, idb->subunit, csr0_status);
	    }
	}
	
	/*
	 * Input miss
	 */
	if (csr0_status & AMDP2_CSR0_MISS) {
	    /*
	     * Get miss counter - csr 112. This event happens when
	     * the driver has to drop an incomming packet becuase 
	     * there are no usable descriptors on the ring.
	     * Make sure to leave rap pointing at csr 0.
	     */
	    ds->amdp2_regaddr->rap = AMDP2_CSR112;
	    temp_miss_count = ds->amdp2_regaddr->rdp & 0xffff;
	    ds->amdp2_regaddr->rap = AMDP2_CSR0;

	    /*
	     * Determine the amount to add to the input error counter
	     * and input resources counter. This is the sum total of 
	     * all misses since the router was powered on. Handle 
	     * the case where csr 112 wraps at 64k.
	     */
	    if (((int)(temp_miss_count - ds->rx_miss_count)) < 0) {
		idb->counters.input_error += (0xffff - ds->rx_miss_count);
		idb->counters.input_error += temp_miss_count;
		idb->counters.input_resource += (0xffff - ds->rx_miss_count);
		idb->counters.input_resource += temp_miss_count;
	    } else {
		idb->counters.input_error +=
		    (temp_miss_count - ds->rx_miss_count);
		idb->counters.input_resource +=
		    (temp_miss_count - ds->rx_miss_count);
	    }
	    ds->rx_miss_count = temp_miss_count;
	}

	/*
	 * Memory error
	 */
	if (csr0_status & AMDP2_CSR0_MERR) {
	    idb->dma_memory_error++;
	    errmsg(&msgsym(MEMERR, AMDP2), 
		   idb->slot, idb->subunit, 0, csr0_status);
	    amdp2_shutdown(idb, IDBS_DOWN);
	    ds->fatal_tx_err = TRUE;
	}
    }

    /*
     * Spurious init done interrupt
     */
    if (csr0_status & AMDP2_CSR0_IDON) {
	ds->spurious_idon++;
	errmsg(&msgsym(SPURIDON, AMDP2), 
	       idb->slot, idb->subunit, 0, csr0_status);
	amdp2_shutdown(idb, IDBS_DOWN);
	ds->fatal_tx_err = TRUE;
    }
    idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
}

/*
 * amdp2_subsys_init:
 * AMDP2 subsystem initialisation entry point
 */
static void amdp2_subsys_init (subsystype *subsys)
{
    /*
     * Register the PCI 8e/4e PA.
     */
    reg_add_analyze_interface(PA_HARDWARE_8E,
                              amdp2_8e_analyze_pa,
                              "amdp2_8e_analyze_pa");

    reg_add_analyze_interface(PA_HARDWARE_4E,
                              amdp2_8e_analyze_pa,
                              "amdp2_8e_analyze_pa");

    reg_add_analyze_interface(PA_HARDWARE_5E,
                              amdp2_8e_analyze_pa,
                              "amdp2_8e_analyze_pa");

    reg_add_pci_device_init(AMDP2_VENDOR_DEVICE_ID,
			    amdp2_analyze_device,
			    "amdp2_analyze_device");
}

/*
 * amdp2 subsystem header
 */
#define AMDP2_MAJVERSION   1
#define AMDP2_MINVERSION   0
#define AMDP2_EDITVERSION  1

SUBSYS_HEADER(amdp2, AMDP2_MAJVERSION, AMDP2_MINVERSION,
	      AMDP2_EDITVERSION,
	      amdp2_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);

/* end of file */
