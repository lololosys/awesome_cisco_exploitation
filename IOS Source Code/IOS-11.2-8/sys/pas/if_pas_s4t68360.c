/* $Id: if_pas_s4t68360.c,v 3.21.6.25 1996/08/28 13:06:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_s4t68360.c,v $
 *------------------------------------------------------------------
 * if_pas_s4t68360.c  PAS specific 4T driver routines
 *
 * 3/02/95   H.M. Robison 
 *
 * Copyright (c) 1995-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_pas_s4t68360.c,v $
 * Revision 3.21.6.25  1996/08/28  13:06:38  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.21.6.24  1996/08/26  23:05:31  rharagan
 * CSCdi66480:  WFQ refuses to be disabled on serial port adapters
 * Branch: California_branch
 * Disable WFQ on VIP, enable WFQ on c7200 for S4T.
 *
 * Revision 3.21.6.23  1996/08/23  04:47:26  xtang
 * CSCdi59553:  clockrate 250000 is not configurable on FSIP in 75xx
 * Branch: California_branch
 *              added 250000 Hz clockrate to s4t68360 4T
 *              port adaptor. The corresponding microcode change are in
 *              cvs -d /micro s4t68360
 *
 * Revision 3.21.6.22  1996/08/22  18:45:44  dujihara
 * CSCdi65825:  Error messages do not specify slot/subunit
 * Branch: California_branch
 * Added slot (bay) and unit to error messages.
 *
 * Revision 3.21.6.21  1996/08/20  02:34:12  rmontino
 * CSCdi66480:  WFQ refuses to be disabled on serial port adapters
 * Branch: California_branch
 *
 * Revision 3.21.6.20  1996/08/13  02:21:34  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.21.6.19  1996/08/09  23:32:30  amcrae
 * CSCdi65046:  4T is reset during configuration, fails to restart,
 * disappears
 * Branch: California_branch
 * OIR driven restart has wrong retry count. Provide some delays when
 * checking AMCC registers. Will no one rid me of this troublesome board?
 *
 * Revision 3.21.6.18  1996/07/23  00:04:50  gstovall
 * CSCdi62888:  OIR serial PA causes %ALIGN-1-FATAL pc=s4t_init
 * Branch: California_branch
 * When we nuke the instance, also reset the idb vectors to point at
 * benign functions.
 *
 * Revision 3.21.6.17  1996/06/18  19:25:49  gstovall
 * CSCdi58572:  Serial line comes back from the dead
 * Branch: California_branch
 * Check physical line state before deciding if the line is up or not.
 *
 * Revision 3.21.6.16  1996/06/07  20:43:15  fred
 * CSCdi58655:  turning RED off on an interface turns WFQ off on that
 * interface
 *         save default setting and restore it when folks remove RED,
 *         Custom, or Priority Queuing
 * Branch: California_branch
 *
 * Revision 3.21.6.15  1996/06/07  04:08:16  amcrae
 * CSCdi56758:  4T does not recover after power surge - should be restarted
 * Branch: California_branch
 * Added structure to carry 4T PA data. Added periodic watchdog check
 * and restart handling. Nuked some unused counters.
 *
 * Revision 3.21.6.14  1996/05/13  20:26:45  dcarroll
 * CSCdi56363:  OIR-insertion of 4E/8E/4T PAs should  bring-up the ports
 * in shut
 * Branch: California_branch
 * OIR'd adapters must stay shutdown through initializatio
 *
 * Revision 3.21.6.13  1996/05/10  19:05:10  gstovall
 * CSCdi56862:  PPP: LCP not open, when testing ddr
 * Branch: California_branch
 * Make the pas serial code support the dialer.
 *
 * Revision 3.21.6.12  1996/05/06  16:43:05  gstovall
 * CSCdi54863:  Priority queuing not working on new interface
 * Branch: California_branch
 * Allow the various queueing options to be enabled on the 4T in a
 * predator.
 *
 * Revision 3.21.6.11  1996/04/30  22:40:09  gstovall
 * CSCdi55850:  Serial PAs reports Elapse time for download and report
 * version
 * Branch: California_branch
 * Clean up the 4T error messages.
 *
 * Revision 3.21.6.10  1996/04/30  16:49:58  gstovall
 * CSCdi55513:  Fix OIR problems
 * Branch: California_branch
 * Fix oir memory handling on 4t.
 *
 * Revision 3.21.6.9  1996/04/29  17:57:29  ssangiah
 * CSCdi54472: Inadequate information in the output of "sh controllers"
 *             command
 *   o Add a "show diag" command for the c7200 platform.
 *   o Make the output from "show controllers" consistent.
 * Branch: California_branch
 *
 * Revision 3.21.6.8  1996/04/22  18:15:26  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.21.6.7  1996/04/10  02:43:24  rramacha
 * CSCdi52294:  Add 4T support for VIP2
 * Branch: California_branch
 * VIP2 support for 4T
 *
 * Revision 3.21.6.6  1996/04/08  19:01:00  dcarroll
 * CSCdi53796:  Remove extra idb type assignments
 * Branch: California_branch
 * Remove the unneeded early setting of idb type.
 *
 * Revision 3.21.6.5  1996/04/05  19:56:31  dcarroll
 * CSCdi53769:  Return bool from pas init interface routines
 * Branch: California_branch
 * Pass boolean up the analyze chain, not idbs.
 *
 * Revision 3.21.6.4  1996/04/05  05:32:25  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.21.6.3  1996/04/04  20:04:26  getchell
 * CSCdi50943:  SVIP PA reset broken
 * Branch: California_branch
 *
 * Revision 3.21.6.2  1996/03/21  23:23:42  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.21.6.1  1996/03/18  21:34:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.19  1996/03/18  23:29:02  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.18  1996/03/08  22:09:58  gstovall
 * CSCdi51111:  Need new 4T firmware
 * Branch: ELC_branch
 * Suck in some of Andrews magic firmware, plus update the driver to deal
 * with it.
 *
 * Revision 3.3.2.17  1996/03/01  23:33:14  gstovall
 * CSCdi49899:  %Software-forced reload occurred while verifying smds over
 * serial.
 * Branch: ELC_branch
 * Memory corruption, Be Gone!
 *
 * Revision 3.3.2.16  1996/02/22  01:11:31  gstovall
 * CSCdi48723:  %Software-forced reload, while testing serial hdlc to
 * serial x25.
 * Branch: ELC_branch
 * Sigh... Forgot to use PARTICLE_ENCAPBYTES instead of ENCAPBYTES.
 *
 * Revision 3.3.2.15  1996/02/21  18:29:38  gstovall
 * CSCdi49117:  serial signals are not displayed on show int command
 * Branch: ELC_branch
 * Set up the idb->show_serial_state vector.
 *
 * Revision 3.3.2.14  1996/02/21  18:01:58  gstovall
 * CSCdi48723:  %Software-forced reload, while testing serial hdlc to
 * serial x25.
 * Branch: ELC_branch
 * Set the pointers correctly when we grab a new particle.  While I am
 * rummaging around in there, clean up some less than optimal code.
 *
 * Revision 3.3.2.13  1996/02/16  21:34:53  dcarroll
 * CSCdi49080:  Add slots support to pas drivers
 * Branch: ELC_branch
 * Remove platform specific code.
 *
 * Revision 3.3.2.12  1996/02/15  19:48:47  dcarroll
 * CSCdi49080:  Add slots support to pas drivers
 * Branch: ELC_branch
 * Add missing slots initialization to pas drives,
 * for oir and snmp support.
 *
 * Revision 3.3.2.11  1996/02/14  01:58:08  gstovall
 * CSCdi48946:  Pull in 4T bugfixes from mainline
 * Branch: ELC_branch
 * Bring the ELC 4T driver up to date.
 *
 * Revision 3.3.2.10  1996/02/08  08:48:06  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.9  1996/02/05  22:17:14  gstovall
 * Branch: ELC_branch
 * Handle changing of maxdgram, strip of the CRC length correctly,
 * implement loopback.
 *
 * Revision 3.3.2.8  1996/01/26  02:50:12  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.3.2.7  1996/01/24  04:16:43  gstovall
 * Branch: ELC_branch
 * Pull in some bug fixes from the 11.1 mainline, make fastswitching work
 * AND count correctly, notice if a cable gets pulled.
 *
 * Revision 3.3.2.6  1996/01/18  22:11:15  gstovall
 * Branch: ELC_branch
 * Preadator now knows the joy and splendor of serial interfaces.
 *
 * Revision 3.3.2.5  1996/01/16  01:43:14  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.4  1995/12/11  19:13:54  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.3.2.3  1995/12/04  22:18:33  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.3.2.2  1995/11/30  02:06:21  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.3.2.1  1995/11/22  21:42:04  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.21  1996/03/05  07:28:34  amcrae
 * CSCdi50610:  4T interfaces die at low temperatures
 * Moved transmit start to after ring update.
 * Rearranged firmware counters.
 *
 * Revision 3.20  1996/02/11  20:24:11  rharagan
 * CSCdi48311:  4T PA will hang rvip if download fails
 * Read back cksm after '360 down load, purge magic numbers from init block.
 *
 * Revision 3.19  1996/02/09  01:53:54  rharagan
 * CSCdi48311:  4T PA will hang rvip if download fails
 * Use variable delay to bound timeout for '360 download and init.
 *
 * Revision 3.18  1996/02/04  09:32:49  rharagan
 * CSCdi45869:  4T does not receive data when cabled as rs-232 DCE
 * Set no_txce control based on sensed cable type, handle rx errors.
 *
 * Revision 3.17.2.1  1996/01/24  22:47:55  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.17  1996/01/22  06:58:33  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.16  1996/01/20  04:59:26  amcrae
 * CSCdi47215:  4T exception processing doesnt signal host
 * CSCdi44907:  Fixed macros overlooked in previous commit
 *
 * Revision 3.15  1996/01/18  23:38:34  bburns
 * CSCdi43000:  4T driver should not call ../if/pci_discover_devices()
 * Deleted call to registry by removing call to pci_device_discover().
 *
 * Revision 3.14  1996/01/13  03:24:09  amcrae
 * CSCdi45724:  4T f/w is not answering mbx msgs during an interface reset
 * CSCdi43260:  VIP 4T has no implementation for transmit-delay
 * New interrupt scheme added. Firmware counter update added.
 *
 * Revision 3.13  1996/01/09  22:32:43  rharagan
 * CSCdi46625:  Holdq dequeueing broken for 4T
 * Pick up 4T version CSCdi41406. Sweep up a bit.
 *
 * Revision 3.12  1995/12/23  01:14:31  rharagan
 * CSCdi45811:  4T interface linestate does not refletct modem control
 * lines
 * Straighten out changes in linestate, continue to bring in code review
 * comments
 *
 * Revision 3.11  1995/12/19  20:24:31  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.10  1995/12/19  16:59:56  amcrae
 * New firmware download for 4T.
 *
 * Revision 3.9  1995/12/17  18:35:59  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.8  1995/12/14  07:41:05  rharagan
 * CSCdi42045:  tx0 output hung (800E - queue full) when pinging on serial
 * 4T
 * Strip out now unneeded tx pkt fragmentation, lose unwieldy ddelays on
 * mbx reads.
 *
 * Revision 3.7  1995/12/06  18:31:45  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.6  1995/12/02  05:50:25  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.5  1995/12/01  02:00:07  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.4  1995/11/28  02:26:16  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.3  1995/11/17  18:54:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:21:18  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:57:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../pas/msg_s4t68360.c"	/* Not a typo, see logger.h */
#include "sys_registry.h"
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "subsys.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/network.h"
#include "../if/pci.h"

#include "ieee.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../wan/ppp.h"
#include "../wan/smds.h"
#include "../wan/atm_dxi.h"
#include "../ip/ip.h"
#include "../if/serial_inline.h"

#include "../parser/parser_defs_priority.h"

#include "if_pas.h" 
#include "if_pas_s4t68360.h" 
#include "pas_id.h"
#include "../util/itemlist.h"
#include "../wan/dialer_registry.h"

/* 4T port adapter downloadable firmware */
#include "../firmware/s4t68360_fw.h"     

/*#define	S4T_FW_DEBUG */

/*
 * 4T-specific data.
 */
s4t_pa *s4t_info[MAX_PA_BAYS];

const s4t_clocktype_t s4t_clock[S4T_CLOCKS] = {
    {0, BPS_NONE},
    {1200, BPS_1200}, {2400, BPS_2400}, {4800, BPS_4800}, {9600, BPS_9600},
    {19200, BPS_19200}, {38400, BPS_38400}, {56000, BPS_56K}, {64000, BPS_64K},
    {72000, BPS_72K}, {125000, BPS_125K}, {148000, BPS_148K},
    {250000, BPS_250K},
    {500000, BPS_500K}, {800000, BPS_800K}, {1000000, BPS_1000K},
    {1300000, BPS_1300K}, {2000000, BPS_2M}, {4000000, BPS_4M},
    {8000000, BPS_8M}
};

/* Static Function Prototypes */
static boolean s4t_init_interface (s4t_pa *, uint, uint);

/*
 * pci_s4t_pa_analyze_interface
 * Initialize the pci-to-pci bridge above the port adaptor, 
 * then discover pci interfaces under the port adaptor.
 * Note: the pa_bay is the connector number or context.
 *
 */
static void pci_s4t_pa_analyze_interface (int pa_bay)
{
    s4t_pa *pa;
    uint port_no;

    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(INITFAIL, S4T68360), pa_bay,
			"pci_s4t_pa_analyze_interface(pa_bay)");
	return;
    }
    /*
     * The s4t_info entry should be null unless this re-enters
     * due to OIR. If so, reuse the old entry.
     */
    pa = s4t_info[pa_bay];
    if (pa == NULL) {
	pa = malloc_fast(sizeof(*pa));
	if (pa == NULL) {
	    errmsg(&msgsym(INITFAIL, S4T68360), pa_bay,
		"pci_s4t_pa_analyze_interface(malloc fail)");
	    return;
	}
	s4t_info[pa_bay] = pa;
    }
    pa->bus = get_pci_bus_no(pa_bay);
    pa->slot = pa_bay;
    pa->no_response = 0;

    if (!s4t_hardware_setup(pa)) {
	pas_deactivate_bay(pa_bay);
 	return;
    }
    
    /* Initialize all serial ports attached to this device */
    for (port_no = 0; port_no < S4T_MAX_PORTS; port_no++ ) {
	
	/* allocate & initialize port idb */
	if (!s4t_init_interface(pa, nserialnets, port_no)) {
	    pas_deactivate_bay(pa_bay);
	    return;
	} else {
	    nserialnets += 1;
	}
    }
    
    /*
     * Update slots state
     */
    pas_activate_bay(pa_bay);

    /* Install PA interrupt handler wrapper */
    pas_install_wrapper(pa_bay, s4t_interrupt_wrapper);
}

/*
 * s4t_pci_init
 * Description: Perform PCI configuration specific to the AMCC S5933.
 */
static boolean s4t_pci_init (s4t_pa *pa)
{
    s4t_regs_t *device_base;
    uint  value;
    
    /*
     * Read VID, DID Vendor/Device ID register, to verify that this
     * is a AMCC S5933 PCI Controller.
     */
    value = pci_config_read(pa->bus, S4T_PCI_DEVICE_NO,
		0, PCI_VENDOR_ID_OFFSET);
    if (value != S4T_AMCC_S5933_VENDOR_DEVICE_ID) {
	errmsg(&msgsym(NOTS4T68360, S4T68360), pa->slot, value);
	return(FALSE);
    }
    device_base = get_pci_device_base(pa->slot, S4T_PCI_DEVICE_NO);
    pa->devbase = device_base;

    /* 
     * Now do device specific configuration :
     * 	  - Set the IO offset for the device
     *    - Enable IO and Bus Master Mode
     *    Use the I/O base b/c that is where the mailboxes have to be.
     */
    value = get_pci_device_base_offset(pa->slot, S4T_PCI_DEVICE_NO);
    pci_config_write(pa->bus, S4T_PCI_DEVICE_NO, 0, PCI_IO_BASE_OFFSET, value);

    /*
     * Configure PCICMD, the command configuration register.
     * The write clears PCISTS, the status register.
     * 
     * Set bus master enable, I/O, and memory access enable.
     */
    value = PCI_COMMAND_IOEN  | PCI_COMMAND_MEMEN | PCI_COMMAND_BMEN;
    pci_config_write(pa->bus, S4T_PCI_DEVICE_NO, 0,
		PCI_COMMAND_REG_OFFSET, value);
    
    /*
     * Configure LAT, the configuration latency timer register.
     * Note this actual offset for LAT is 0x0D; PCI_LATENCY_OFFFSET
     * is 0x0C.
     */
    value = S5933_LATENCY_TIMER_VALUE;
    pci_config_write(pa->bus, S4T_PCI_DEVICE_NO, 0, PCI_LATENCY_OFFSET, value);
    
    /* Set read & write enable in bus master operation register */
    device_base->bus_mstr_ctrl_reg = 
	S5933_BUS_MSTR_RD_XFER_ENABLE | S5933_BUS_MSTR_WR_XFER_ENABLE;
    /*
     *	Enable incoming mailbox interrupts on Mailbox word 4
     */
    device_base->int_ctrl_reg =
	S5933_INT_CTRL_IMBOX_ENABLE | S5933_INT_CTRL_IMBOX_4;
    return(TRUE);
}

/*
 * s4t_write_s5933_mailbox
 * Not to be used for 4T download.
 */
static boolean s4t_write_s5933_mailbox (s4t_pa *pa, mbx_message_t *cmd)
{
    boolean status = FALSE;
    uint retry;
    leveltype level;
    s4t_regs_t *device_base = pa->devbase;

    /*
     *	Lock out interrupts so that we don't have a conflict with
     *	the interrupt handlers.
     */
    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *	Make sure mailbox is empty
     *	If it isn't, wait for awhile.
     */
    for (retry = 0; retry < S5933_MBX_EMPTY_RETRIES*2; retry++) {
	if (!(device_base->mbx_status_reg & S5933_OUT_MBX_STATUS_MASK)) {
	    status = TRUE;
	    device_base->out_mbx3 = cmd->mbx_regs.mbx3;
	    device_base->out_mbx2 = cmd->mbx_regs.mbx2;
	    device_base->out_mbx1 = cmd->mbx_regs.mbx1;
	    break;
        }
	usecdelay(5);
    }
    /*
     *	If the mailbox didn't respond, then we have a problem.
     *	If this happens, hit the big red button.
     */
    if (status == FALSE) {
	pa->no_response++;
    }

    reset_interrupt_level(level);

    return(status);
}

/*
 * s4t_dwnld_write_mailbox
 *
 * Input parameter 'reverse' to tells  whether mailboxes should be written
 * 1-2-3 or reversed 3-2-1, or if contiguous download, write mailbox 3 only.
 */
static boolean s4t_dwnld_write_mailbox (s4t_pa *pa,
					mbx_message_t *cmd, int reverse)
{
    boolean status = TRUE;
    uchar retry;
    uint temp0, temp1, temp2;
    s4t_regs_t *device_base = pa->devbase;
    

    
    /* check for stale read data */
    if (device_base->mbx_status_reg & S5933_IN_MBX_STATUS_MASK) {
	/* read message in  first 3 mailboxes */
	temp2 = device_base->in_mbx3;
	temp1 = device_base->in_mbx2;
	temp0 = device_base->in_mbx1;
    }
    
    
    /* write message to first 3 mailboxes */
    if (reverse == S5933_REVERSE_MBX_WRITE) {
	device_base->out_mbx3 = cmd->mbx_regs.mbx3;
	device_base->out_mbx2 = cmd->mbx_regs.mbx2;
	device_base->out_mbx1 = cmd->mbx_regs.mbx1;
    }
    else if (reverse == S5933_CON_DWNLD_MBX_WRITE) {
	device_base->out_mbx3 = cmd->mbx_regs.mbx3;
    }
    else /* (reverse == S5933_NORMAL_MBX_WRITE ) */ {
	device_base->out_mbx1 = cmd->mbx_regs.mbx1;
	device_base->out_mbx2 = cmd->mbx_regs.mbx2;
	device_base->out_mbx3 = cmd->mbx_regs.mbx3;
    }
    
    
    /* Wait for PA to read mailboxes */
    /* make sure mailboxes are now empty */
    status = FALSE;
    
    /* read mailbox empty/full status */
    for (retry = 0; retry < S5933_MBX_EMPTY_RETRIES_DWNLD; retry++) {
	usecdelay(2);
	/* if any bits set, then mailbox not entirely read by PA */
	if (!(device_base->mbx_status_reg & S5933_OUT_MBX_STATUS_MASK)) {
	    status = TRUE;
	    break;
        }
    }
    if (status == FALSE) {
	pa->no_response++;
    }
    return(status);
}

/*
 * s4t_read_s5933_mailbox
 *
 */
static boolean s4t_read_s5933_mailbox (s4t_pa *pa,
				       mbx_message_t *cmd, int delay)
{
    boolean status;
    uint retry;
    uchar msg_cmd;
    ulong temp0, temp1, temp2;
    leveltype level;
    s4t_regs_t *device_base = pa->devbase;

    /*
     *	Raise interrupts so that we don't collide with the
     *	interrupt handler.
     */
    level = raise_interrupt_level(NETS_DISABLE);

    /* clear stale read data, the PA apparently requires this */
    if (device_base->mbx_status_reg & S5933_IN_MBX_STATUS_MASK) {
	/* read message in  first 3 mailboxes */
	temp2 = device_base->in_mbx3;
	temp1 = device_base->in_mbx2;
	temp0 = device_base->in_mbx1;	    
	errmsg(&msgsym(MBXREAD, S4T68360), pa->slot, "mbx read ",
               temp0, temp1,temp2);
    }
    
    /* first write message requesting read */
    if(!s4t_write_s5933_mailbox(pa, cmd)) {
	reset_interrupt_level(level);
	return(FALSE);
    }
    
    /* Wait for response from PA */
    status = FALSE;
    msg_cmd = cmd->msg.cmd_byte;
    
    for (retry = 0; retry < S5933_MBX_EMPTY_RETRIES; retry ++) {      
	if (delay) {
	    usecdelay(delay);
	}
	if ((device_base->mbx_status_reg & S5933_IN_MBX_STATUS_MASK)
	    == S5933_IN_MBX_STATUS_MASK) {
	    /* read message in  first 3 mailboxes */
	    cmd->mbx_regs.mbx3 = device_base->in_mbx3;
	    cmd->mbx_regs.mbx2 = device_base->in_mbx2;
	    cmd->mbx_regs.mbx1 = device_base->in_mbx1;
		status = TRUE;
		break;
	}
    }
    reset_interrupt_level(level);
    if (status == FALSE) {
	/* clear response */
	cmd->mbx_regs.mbx3 = 0;
	cmd->mbx_regs.mbx2 = 0;
	cmd->mbx_regs.mbx1 = 0;
	pa->no_response++;
	if (serial_debug) {
	    buginf("\nS4T mailbox cmd timeout: command %d", msg_cmd);
	}
    } else if (msg_cmd != cmd->msg.cmd_byte) {
	status = FALSE;
	if (serial_debug) {
	    buginf("\nS4T: mailbox cmd mismatch, sent %d, got %d",
			msg_cmd, cmd->msg.cmd_byte);
	}
	}
    
    return(status);
}

/*
 * s4t_dwnld_read_mailbox
 *
 */
static boolean s4t_dwnld_read_mailbox (s4t_pa *pa,
				       mbx_message_t *cmd, int reverse)
{
    boolean status;
    uchar retry;
    uint temp0, temp1, temp2; 
    s4t_regs_t *device_base = pa->devbase;
    
    
    /* first write message requesting read */
    if(!s4t_dwnld_write_mailbox(pa, cmd, reverse)) {
	/* check for read data */
	if (device_base->mbx_status_reg & S5933_IN_MBX_STATUS_MASK) {
	    /* msg not read, read message  that was written 
	       in  first 3 mailboxes */
	    temp2 = device_base->in_mbx3;
	    temp1 = device_base->in_mbx2;
	    temp0 = device_base->in_mbx1;
	    errmsg(&msgsym(MBXREAD, S4T68360), pa->slot, "dwnld read start ",
		   temp0, temp1, temp2);
	}
	return(FALSE);
    }
    
    /* Wait for response from PA */
    status = FALSE;
    for (retry = 0; retry < S5933_MBX_EMPTY_RETRIES_DWNLD; retry++) {      
	if ((device_base->mbx_status_reg & S5933_IN_MBX_STATUS_MASK)
	    == S5933_IN_MBX_STATUS_MASK) {
	    status = TRUE;
	    break;
	}
    }

    /* read message in  first 3 mailboxes */
    cmd->mbx_regs.mbx3 = device_base->in_mbx3;
    cmd->mbx_regs.mbx2 = device_base->in_mbx2;
    cmd->mbx_regs.mbx1 = device_base->in_mbx1;
	
    if (status == FALSE) {
	errmsg(&msgsym(MBXREAD, S4T68360), pa->slot, "dwnld read ack ",
	       cmd->mbx_regs.mbx1, cmd->mbx_regs.mbx2, cmd->mbx_regs.mbx3);
	/* clear response - it was invalid */
	cmd->mbx_regs.mbx3 = 0;
	cmd->mbx_regs.mbx2 = 0;
	cmd->mbx_regs.mbx1 = 0;
    }
    
    return(status);
    
}

/* 
 * s4t_download
 * 
 * Download code to 68360 on PA.  Return TRUE
 * if successful.
 */
static boolean s4t_download (s4t_pa *pa)
{
    mbx_message_t cmd;
    ulong size, *dp;
    ulong blksz, sum = 0;
    volatile uchar *addr;     /* track address for contiguous loading mode */
    sys_timestamp timer;

    size = (sizeof(s4t_download_code))/(sizeof(ulong));
    
    cmd.msg.cmd_byte = S4T_CMD_DOWNLOAD_READY;
    cmd.msg.not_use[0] = 0;
    cmd.msg.not_use[1] = 0;
    cmd.msg.not_use[2] = 0;
    cmd.msg.msg_body.download_ready.no_message_body = 0;
    
    GET_TIMESTAMP(timer);
    if(!s4t_read_s5933_mailbox(pa, &cmd, S4T_POWERUP)) {
	if (serial_debug) {
	    buginf("\nElapsed time for download init w/ delay is %d",
		   ELAPSED_TIME(timer));
	}
	errmsg(&msgsym(NOTREADY,S4T68360), pa->slot);
	return(FALSE);
    }
    pa->download_delay = ELAPSED_TIME(timer);

    cmd.msg.cmd_byte = S4T_CMD_DOWNLOAD;
    cmd.msg.not_use[0] = 0;
    cmd.msg.not_use[1] = 0;
    cmd.msg.not_use[2] = 0;
    
    dp = s4t_download_code;
    /*
     * We must have at least 3 words, (length, address and one data word).
     */
    while (size > 2) {
	blksz = *dp++;
	addr = (uchar *)*dp++;
	/*
	 *	Sanity check the block size. Must not be larger than the
	 *	size of the remaining download data array.
	 */
	if (blksz > size) {
	    errmsg(&msgsym(NOTCMPLT,S4T68360), pa->slot);
	    return(FALSE);
	}
	/*
	 *	Adjust sizes. 
	 */
	size -= blksz;
	blksz -= 3;
	/*
	 * Download the first data word as a `REVERSE_MBX_WRITE'. This
	 * should tell the 4T that the data is being stored in a
	 * non-contiguous location from the last write.
	 */
	cmd.msg.msg_body.download.load_address = addr;
	sum += *dp;
	cmd.msg.msg_body.download.data = *dp++;
	if(!s4t_dwnld_write_mailbox(pa, &cmd, S5933_REVERSE_MBX_WRITE)) {
	    return(FALSE);
	}
	/*
	 *  Download the next N data words as contiguous address mailbox
	 *  writes.
	 */
	while (blksz-- > 0) {
	    addr += 4;
	    cmd.msg.msg_body.download.load_address = addr;
	    sum += *dp;
	    cmd.msg.msg_body.download.data = *dp++;
	    if(!s4t_dwnld_write_mailbox(pa, &cmd, S5933_CON_DWNLD_MBX_WRITE)) {
		return(FALSE);
	    }
	}
    }
    /* 
     * Write to mailboxes in order 1-2-3 for DONE message to break
     * contiguous addressing mode.
     */
    cmd.msg.cmd_byte = S4T_CMD_DOWNLOAD_DONE;
    cmd.msg.msg_body.download_done.checksum = 0;
    
    if(!s4t_dwnld_read_mailbox(pa, &cmd, S5933_NORMAL_MBX_WRITE)) {
	errmsg(&msgsym(DWNLDFL,S4T68360), pa->slot);
	return(FALSE);
    }
    if(cmd.msg.msg_body.download_done.checksum != sum) {
	errmsg(&msgsym(DWNLDCKSM,S4T68360), pa->slot, sum,
	       cmd.msg.msg_body.download_done.checksum);
	return(FALSE);
    }
    
    cmd.msg.cmd_byte = S4T_CMD_DOWNLOAD_EXEC;
    cmd.msg.msg_body.download_exec.exec_address = (uchar *)s4t_dwnld_exec_addr;
    if(!s4t_dwnld_write_mailbox(pa, &cmd, S5933_REVERSE_MBX_WRITE)) {
	errmsg(&msgsym(NOTCMPLT,S4T68360), pa->slot);
	return(FALSE);
    }
    
    return(TRUE);
}
/*
 * s4t_mask_status_int
 * Hand down the interrupt mask for rx/tx interrupts
 */
static boolean s4t_mask_status_int (hwidbtype *idb)
{
    mbx_message_t cmd;
    s4t_instance_t *ds;

    ds = INSTANCE(idb);

    /* Set port status interrupts */
    cmd.msg.cmd_byte = S4T_CMD_MASK_PORT_STATUS;
    cmd.msg.msg_body.mask_port_status.port_id = idb->subunit;	
    cmd.msg.msg_body.mask_port_status.port_mask = ds->port_status_mask;

    return(s4t_write_s5933_mailbox(ds->pa_info, &cmd));
}
/*
 * s4t_report_verson
 * Read PA's software & hardware rev level
 */
static boolean s4t_report_version (s4t_pa *pa)
{
    mbx_message_t cmd;
    boolean status;
    sys_timestamp timer;

    cmd.msg.cmd_byte = S4T_CMD_REPORT_VERSION;
    
    cmd.msg.msg_body.report_version.sw_version = 0;
    cmd.msg.msg_body.report_version.hw_version = 0;
    GET_TIMESTAMP(timer);
    status = s4t_read_s5933_mailbox(pa, &cmd, S4T_STARTUP);
    pa->report_delay = ELAPSED_TIME(timer);
    if (serial_debug) {
	buginf("\nElapsed time for report version w/ delay is %d",
	       pa->report_delay);
    }

    /*
     * Squirrel these versions away for later shipment upstairs.
     */
    pa->versions.sw_version = cmd.msg.msg_body.report_version.sw_version;
    pa->versions.hw_version = cmd.msg.msg_body.report_version.hw_version;
    pa->versions.cpu_speed = cmd.msg.msg_body.report_version.cpu_speed;

    return(status);
}
#ifdef	S4T_FW_DEBUG
/*
 * s4t_get_trace
 * Read trace code.
 */
static boolean s4t_get_trace (s4t_pa *pa, ulong *tracecode)
{
    mbx_message_t cmd;
    boolean status;

    cmd.msg.cmd_byte = S4T_CMD_GET_TRACE;
    cmd.msg.msg_body.get_trace.tracecode = 0;
    status = s4t_read_s5933_mailbox(pa, &cmd, 10);
    tracecode[0] = cmd.msg.msg_body.get_trace.tracecode;
    tracecode[1] = cmd.msg.msg_body.get_trace.tracedata;

    return(status);
}
/*
 * s4t_force_crash
 * Force 4T to crash
 */
static boolean s4t_force_crash (s4t_pa *pa)
{
    mbx_message_t cmd;

    cmd.msg.cmd_byte = S4T_CMD_LOCKUP;
    return(s4t_write_s5933_mailbox(pa, &cmd));
}
#endif

/*
 * s4t_init_port
 * Give port it's init block address
 *
 */
static boolean s4t_init_port (hwidbtype *idb)
{
    mbx_message_t cmd;
    s4t_instance_t *ds;
    serialsb *ssb;
    
    ds = INSTANCE(idb);

    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * Build an initialization block in PCI memory
     */  
    ds->init_block_size = sizeof(s4t_init_block_t);
    
    /*
     * Convert to pointer that has desired cache attributes
     */  
    
    ds->init_block->tx_bit_rate = ssb->serial_clockindex;   
    ds->init_block->idle_code = (idb->serial_flags &
				 SERIAL_FLAGS_DO_ALT_FLAG) ?  1 : 0;

    ds->init_block->txc_invert = ssb->invert_txc;  
    ds->init_block->rx_sync_disable = ssb->rx_sync_disable; 
    ds->init_block->full_half_dplx = idb->hdx_flags;
    
    ds->init_block->mtu = idb->max_buffer_size;            

    ds->init_block->mode = S4T_HDLC_MODE;         

    ds->init_block->particle_size = S4T_PARTICLE_SIZE;         

    ds->init_block->tx_bit_rate 
      = s4t_clock[ssb->serial_clockindex].bitrate_code;

    ds->init_block->coding_format = ssb->nrzi_enable;
    ds->init_block->crc_type = ssb->crc_32_enabled;
    ds->crc_size = (ssb->crc_32_enabled) ? 4 : 2;

    if (ssb->ignore_dcd) {
        ds->up_signal = S4T_STATUS_DTR_DSR;
    } else {
        ds->up_signal = S4T_STATUS_DCD;
    }

    ds->init_block->tx_delay = idb->xmitdelay;

    cmd.msg.cmd_byte = S4T_CMD_INIT_PORT;
    
    cmd.msg.msg_body.init_port.port_id = idb->subunit;
    cmd.msg.msg_body.init_port.length = sizeof(s4t_init_block_t);
    cmd.msg.msg_body.init_port.init_block = ds->init_block;

    return(s4t_write_s5933_mailbox(ds->pa_info, &cmd));
}
/*
 * s4t_start_transmit
 * Tell 4T to poll transmit ring now.
 */
boolean s4t_start_transmit (hwidbtype *idb)
{
    mbx_message_t cmd;
    s4t_instance_t *ds;

    ds = INSTANCE(idb);

    cmd.msg.cmd_byte = S4T_CMD_START_TRANSMIT;

    cmd.msg.msg_body.start_transmit.port_number = idb->subunit;
    return(s4t_write_s5933_mailbox(ds->pa_info, &cmd));
}

/*
 * s4t_update_stats
 * request update of stats counters
 *
 */
static boolean s4t_update_stats (hwidbtype *idb)
{
    mbx_message_t cmd;
    s4t_instance_t *ds;
    
    ds = INSTANCE(idb);

    ds->fw_stats->updated = 0;
    cmd.msg.cmd_byte = S4T_CMD_UPDATE_STATS;
    cmd.msg.not_use[0] = 0xAA;
    cmd.msg.not_use[0] = 0xBB;
    cmd.msg.not_use[0] = 0xCC;
    
    cmd.msg.msg_body.update_stats.port_id = idb->subunit;
    cmd.msg.msg_body.update_stats.length = sizeof(s4t_stats);
    cmd.msg.msg_body.update_stats.stats = ds->fw_stats;

    return(s4t_write_s5933_mailbox(ds->pa_info, &cmd));
}

/*
 * s4t_clear_stats
 * Clear stats counters
 *
 */
boolean s4t_clear_stats (hwidbtype *idb)
{
    mbx_message_t cmd;
    s4t_instance_t *ds;
    
    ds = INSTANCE(idb);

    cmd.msg.cmd_byte = S4T_CMD_CLEAR_STATS;
    
    cmd.msg.msg_body.clear_stats.port_id = idb->subunit;

    return(s4t_write_s5933_mailbox(ds->pa_info, &cmd));
}

/*
 * s4t_chg_init_param
 * Change a parameter in port's init block
 */
boolean s4t_chg_init_param (hwidbtype *idb, uchar offset, ushort size,
			    ulong new_value)
{
    mbx_message_t cmd;
    s4t_instance_t *ds;

    ds = INSTANCE(idb);

    cmd.msg.cmd_byte = S4T_CMD_CHG_INIT_PARAM;
    
    cmd.msg.msg_body.chg_init_param.port_id = idb->subunit;
    cmd.msg.msg_body.chg_init_param.offset = offset;
    cmd.msg.msg_body.chg_init_param.size = size;
    cmd.msg.msg_body.chg_init_param.new_value = new_value;
    
    return(s4t_write_s5933_mailbox(ds->pa_info, &cmd));
}				

/*
 * s4t_linestate
 * 
 * Return indicates whether line carrier state has changed
 * TRUE indicates no change, FALSE not the same.
 */ 
boolean s4t_linestate (hwidbtype *idb)
{
    s4t_instance_t *ds;
    boolean old_state;
    
    ds = INSTANCE(idb);
    
    old_state = ds->s4t_lineup;
    if ((ds->port_status & ds->up_signal) == ds->up_signal) {
	ds->s4t_lineup = TRUE;   
    } else {
	ds->s4t_lineup = FALSE;   
    }
    if (old_state == ds->s4t_lineup) {
	return(TRUE);
    } else {
	return(FALSE);
    }
}


/*
 * s4t_read_qsize
 * Read firmware message queue length.
 */
static int s4t_read_qsize (hwidbtype *idb)
{
    mbx_message_t cmd;
    s4t_instance_t *ds;
    
    ds = INSTANCE(idb);
    
    cmd.msg.cmd_byte = S4T_CMD_MSGQ;

    if(!s4t_read_s5933_mailbox(ds->pa_info, &cmd, 10)) {
	return(-1);
    } else {
        if (serial_debug)
	    buginf("\n%s: msg queue size is %d",
		   idb->hw_namestring, cmd.msg.msg_body.msgq.qsize);

	return(cmd.msg.msg_body.msgq.qsize);
    }
}

/*
 * s4t_read_port_status
 * Read port control line status & cable type
 * 
 */
boolean s4t_read_port_status (hwidbtype *idb)
{
    mbx_message_t cmd;
    s4t_instance_t *ds;
    
    ds = INSTANCE(idb);
    
    cmd.msg.cmd_byte = S4T_CMD_QUERY_STATUS;
    cmd.msg.msg_body.query_status.port_id = idb->subunit;
    cmd.msg.msg_body.query_status.port_status = 0;

    if(!s4t_read_s5933_mailbox(ds->pa_info, &cmd, 10)) {
	return(FALSE);
    } else {
	ds->port_status = cmd.msg.msg_body.query_status.port_status;
	ds->applique_id = (ds->port_status & S4T_CABLE_MODE) >> 4;
	/* Test for odd/even works w/o G.703 */
	if (ds->applique_id & 1) {
	    idb->dte_interface = FALSE;
	} else {
	    idb->dte_interface = TRUE;
	}
	return(TRUE);
    }
}

/*
 * s4t_txqlength
 * 
 * Returns the number of active packets in the transmit ring.
 */
static int s4t_txqlength (hwidbtype *idb)
{
    s4t_instance_t *ds;
    
    ds = INSTANCE(idb);

    return (ds->tx_count);
}

/*
 * s4t_restart 
 * Wrapper routine for idb->reset vector:
 */
void s4t_restart (hwidbtype *idb)
{
    leveltype level;
    s4t_instance_t *ds;

    ds = INSTANCE(idb);
       
    /*
     * Raise the interrupt level, flush the i/o holdq queues
     * and re-initialize the device.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    
    holdq_reset(idb);		/* Flush input and output queues */
    
    if (idb->state != IDBS_ADMINDOWN) {
	/* Reset interface */
	if(s4t_init(idb)) {
	    s4t_linestate(idb);
	    s4t_lineproto_change(idb, ds->s4t_lineup);
	} else {
	    errmsg(&msgsym(STARTFAIL, S4T68360), 
		   idb->hw_namestring, " s4t_restart()");
	}
    }
    reset_interrupt_level(level);
}

static boolean s4t_hw_set_config (hwidbtype *hwidb, itemlist *ilist,
				      serialsb *ssb)
{
    s4t_instance_t *ds;
    itemdesc *item;
    boolean any_unhandled = FALSE;   /* assume we've handled all items */
    tinybool this_item_unhandled;

    ds = INSTANCE(hwidb); 

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;

	switch (item->item) {

#if 0
	  case SERIAL_HW_DCE_TERMINAL_TIMING_ENABLE:
	    ssb->dce_terminal_timing_enable = item->u.l_value;
	    s4t_init(hwidb, FALSE);	/* Reset code will set it up */
	    break;

#endif
	  case SERIAL_HW_INVERT_TXC:
	    ssb->invert_txc = item->u.l_value;
	    s4t_init(hwidb);
	    break;

	  case SERIAL_HW_NRZI_ENABLE:
	    ssb->nrzi_enable = item->u.l_value;
	    s4t_init(hwidb);
	    break;

	  case SERIAL_HW_CRC_32_ENABLED:
	    ssb->crc_32_enabled = item->u.l_value;
	    s4t_init(hwidb);
	    break;
#if 0
	  case SERIAL_HW_RX_SYNC_DISABLE:
	    ssb->rx_sync_disable = item->u.l_value;
	    s4t_init(hwidb, FALSE);	/* Reset code will set it up */
	    break;

#endif
	  case SERIAL_HW_IGNORE_DCD:
	    ssb->ignore_dcd = item->u.l_value;
	    s4t_init(hwidb);
	    break;

	  case SERIAL_HW_SET_CLOCKRATE:
	    s4t_clockrate_command(hwidb, item->u.buffer);
	    break;

	  default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}

	if (!this_item_unhandled)
	    item->handled = TRUE;
    }

    return(!any_unhandled);
}

static boolean s4t_hwidb_state_config (hwidbtype *hwidb, ulong opcode,
					   itemlist *ilist)
{
    boolean ret;
    serialsb *ssb;

    if ((hwidb == NULL) && (ilist == NULL))
	return (FALSE);

    if (ilist->item_count == -1)
	ilist->item_count = itemlist_count(ilist);

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    if (ssb == NULL)
	return (FALSE);

    switch (opcode) {
      case IDB_CONTROL_SET_CONFIG:
	ret = s4t_hw_set_config(hwidb, ilist, ssb);
	break;

      default:
	ret = FALSE;
	break;
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (!ret) {
	itemlist_reset(ilist);
	ret = (*ssb->hw_super_state_config)(hwidb, opcode, ilist);
    }
    return (ret);
}

/*
 * s4t_show_serial_state 
 * Print out DTR, CTS, etc. on serial PAS
 *   Note: char * input is not used; It's there to conform
 *   with the function prototype declared for the idb pointer.
 */
static void s4t_show_serial_state (hwidbtype *idb, char *leader)
{
    s4t_instance_t *ds;
    ushort modem_control;
    const char *fmt_str2 = "     DCD=%s  DSR=%s  DTR=%s  RTS=%s  CTS=%s";
    
    ds = INSTANCE(idb);
    
    /*
     * Be courteous, print all hand shaking signals.
     */
    modem_control = ds->port_status & S4T_SIGNAL_MASK;
    
    /* 
     * make the display consistent with fsip and with what is 
     * in s4t_update_port_status() in if_vip_s4t68360.c
     */
    printf(fmt_str2, 
	   ((modem_control & S4T_STATUS_DCD) ? "up" : "down"),
	   ((modem_control & S4T_STATUS_DTR_DSR) ? "up" : "down"),
	   (((modem_control & ds->up_signal) == ds->up_signal) ? "up" : "down"),
	   (((modem_control & ds->up_signal) == ds->up_signal) ? "up" : "down"),
	   ((modem_control & S4T_STATUS_RTS_CTS) ? "up" : "down"));
}

/*
 * s4t_init_idb
 * Common IDB initialization routine:
 */
static boolean s4t_init_idb (hwidbtype *idb, s4t_pa *pa)
{
    idbtype *swidb = idb->firstsw;
    s4t_instance_t *ds;
    serialsb *ssb;
    
    /*
     * If this is the first time through this routine
     * for this idb, then initialize the following.
     * For idbs off the removed queue, this initialization
     * should not be repeated.
     */
    if (idb->oir_flags & IDB_FIRST_INIT) {
	idb->status = IDB_SERIAL | IDB_SYNC | IDB_BCAST | IDB_BRIDGE |
	    IDB_HEARSELF;
	idb->type = IDBTYPE_S4T68360;
	idb->typestring = "4T/MC68360";
    
        /* set the platform specific queuing policy */
        idb->queue_algorithm = SERIAL_QUEUE_POLICY;
	serial_idb_init(idb, MAXSERIALSIZE, SERIAL_MAXDGRAM,
			VISIBLE_SERIAL_BANDWIDTH, SERIAL_DELAY); 

	if (pas_oir_active()) {
	    idb->state = IDBS_ADMINDOWN; 
	} else {
	    idb->state = IDBS_INIT;
	}
	idb->enctype = ET_HDLC;
	GET_TIMESTAMP(idb->state_time);
	idb->error_threshold = 5000;
	idb->error_count = 0;
	idb->fast_switch_flag = 0;
	idb->serial_flags |= SERIAL_FLAGS_CAN_ALT_FLAG |
	    SERIAL_FLAGS_CAN_CLK_INV | SERIAL_FLAGS_CAN_NRZI |
		SERIAL_FLAGS_CAN_IGNORE_DCD | SERIAL_FLAGS_CAN_CRC_32;
	/* allow 'clock rate' command */
	idb->serial_flags &= ~SERIAL_FLAGS_NO_CLOCKRATE; 
	idb->pool = NULL;
	idb->pool_group = POOL_GROUP_INVALID;
    
	/*
	 * Set up function calls
	 */
	idb->listen = (listen_t) NULL;
	idb->listen_range = (listenrange_t) NULL;
	idb->setup_bridging = NULL;
	swidb->span_disabled = FALSE;
       
	idb->reliability = 255;
	idb->load = 1;
	idb->rxload = 1;

	idb->oir_flags &= ~IDB_FIRST_INIT;
    }

    /*
     * Set up function calls.  Do this everytime, because we clean up
     * these vectors when a card gets yanked.
     */
    idb->soutput = s4t_tx_start;       /* start transmitter */
    idb->reset = s4t_restart;
    idb->shutdown = s4t_shutdown;
    idb->device_periodic = (periodic_t) s4t_periodic; 
    idb->enable = s4t_enable;
    idb->txqlength = s4t_txqlength;
    idb->show_controller = s4t_show_controller;
    idb->fastsend = s4t_fastsend;
    idb->show_serial_state = s4t_show_serial_state;
    
    /*
     * idb->set_maxdgram must be initialized after calling
     * serial_idb_init() so that hdlc_setup does not attempt to call
     * s4t_set_maxdgram() while doing system init
     */
    idb->set_maxdgram = s4t_set_maxdgram;

    media_idb_enqueue(idb);
    /*
     * Set up state config vector
     */
    if (idb->state_config != s4t_hwidb_state_config) {
	ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);

	idb->hdx_flags = S4T_FULL_DUPLEX;
	idb->serial_flags &= ~SERIAL_FLAGS_DO_ALT_FLAG; /* set to 0 */
	ssb->serial_clockindex = BPS_NONE;
	ssb->nrzi_enable = S4T_NRZ;
	ssb->crc_32_enabled = 0;
	ssb->invert_txc = S4T_TXC_NORMAL;
	ssb->rx_sync_disable = S4T_RX_SYNC_NORMAL;

	ssb->hw_super_state_config = idb->state_config;
	idb->state_config = s4t_hwidb_state_config;
	idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
    }

    /*
     * Allocate an instance structure for this device
     */
    ds = malloc_fast(sizeof(s4t_instance_t));
    if (!ds) {
	return(FALSE);
    }
    INSTANCE(idb) = (void *)ds;
    
    /*ds->s5933_regbase = base_addr; */
    ds->pa_info = pa;
    pa->references++;
    pas_instance_init_common(&ds->pacb, idb);

    /*
     * Setup the removal vector here so that if the init process
     * fails from here on in, there will be a device specific vector
     * defined to clean up resources
     */
    ds->pacb.pas_deallocate = s4t_deallocate;

    /*
     * Allocate init block and stats counters block.
     */
    ds->init_block_malloc = malloc_named_pcimem(sizeof(s4t_init_block_t), 
						idb->hw_namestring);
    if (!ds->init_block_malloc) {
	return(FALSE);
    }
    ds->init_block = ds->init_block_malloc;
    ds->fw_stats = malloc_pcimem(sizeof(s4t_stats));
    /*
     * Allocate rx ring and rx ring shadow
     */
    ds->rxr_malloc = malloc_named_pcimem(sizeof(s4t_dscrptr_t) * 
					 S4T_RX_RING_ENTRIES,
					 idb->hw_namestring);
    if (!ds->rxr_malloc) {
	return(FALSE);
    }
    ds->rxr = pci_memaddr_convert(ds->rxr_malloc, S4T_RX_RING_ATTRIBUTE);
    ds->rxr_shadow = malloc_fast(sizeof(particletype *) * S4T_RX_RING_ENTRIES);
    if (!ds->rxr_shadow) {
	return(FALSE);
    }
    s4t_rxring_flush(ds);

    /*
     * Allocate tx ring and tx ring shadow
     */
    ds->txr_malloc = malloc_named_pcimem(sizeof(s4t_dscrptr_t) * 
					 S4T_TX_RING_ENTRIES,
					 idb->hw_namestring);
    if (!ds->txr_malloc) {
	return(FALSE);
    }
    ds->txr = ds->txr_malloc;
    ds->txr_shadow = malloc_fast(sizeof(s4t_tx_shadow_t) * 
				 S4T_TX_RING_ENTRIES);
    if (!ds->txr_shadow) {
	return(FALSE);
    }

    /*
     * Do platform dependent initialization
     */
    s4t_platform_init(idb);

    /*
     * Successfully obtained all resources
     */
    return(TRUE);

}
 
/*
 * s4t_init_interface
 * Set up an idb for a s4t interface
 *
 */

static boolean s4t_init_interface (s4t_pa *pa, uint unit, uint subunit)
{
    hwidbtype *idb = NULL;
    
    /*
     * Set up storage, add IDB to interface queue
     */
    
    idb = s4t68360_idb_create(pa, subunit);
    if (!idb) {
	return(FALSE);
    }
 
    /*
     * Save our unit number and PA bay number
     */
    idb->slot = pa->slot;     /* Corresponds to pa_bay */
    pa->idbs[subunit] = idb;
    idb->unit = unit;       /* nth interface of type media */
    idb->subunit = subunit; /* Corresponds to vip concept of interface */
    idb->slotunit = subunit; /* port/unit number for this slot */
    pas_set_fci_type(idb, pa->slot); /* Need fci type for OIR support */
    idb->status &= ~IDB_NOHARDWARE;
 
    if (!s4t_init_idb(idb, pa)) {
	return(FALSE);
    }
    /*
     * Initialize the hardware
     */
    if (!s4t_init(idb)) {
	return(FALSE);
    } else {
	return (TRUE);
    }
}

/*
 *  Initialise the 4T hardware from power up.
 */
boolean s4t_hardware_setup (s4t_pa *pa)
{
    /* Perform PCI configuration specific to the AMCC S5933. */
    /* The 4T is a bit different from the other PAS: it has
     * only 1 PCI interface device for all 4 serial ports.
     */
    if (s4t_pci_init(pa) == FALSE) {
	errmsg(&msgsym(INITFAIL, S4T68360), pa->slot, "s4t_pci_init()");
	return(FALSE);
    }
    
    /* download software to PA */
    if (!s4t_download(pa)) {
	errmsg(&msgsym(INITFAIL, S4T68360), pa->slot, "s4t_download()");
	return(FALSE);
    }
    

    /* get sw & hw revision info */
    DELAY(60); /* permit the '360 to load it's Xilinx */
    if (!s4t_report_version(pa)) {
	errmsg(&msgsym(INITFAIL, S4T68360), pa->slot, "s4t_report_version()");
	return(FALSE);
    }
    return(TRUE);
}

/*
 * s4t_reset_port
 * Reset port; Should follow with init port command
 */
static boolean s4t_reset_port (hwidbtype *idb)
{
    mbx_message_t cmd;
    s4t_instance_t *ds;
    
    ds = INSTANCE(idb);
    
    cmd.msg.cmd_byte = S4T_CMD_RESET_PORT;
    cmd.msg.msg_body.reset_port.port_id = idb->subunit;
    cmd.msg.msg_body.reset_port.port_reset = S4T_RESET_PORT;

    return(s4t_write_s5933_mailbox(ds->pa_info, &cmd));
}

/*
 * s4t_enable_port
 * Enable/Disable port Rx &/or Tx reception/transmission
 */
static boolean s4t_enable_port (hwidbtype *idb, uchar state )
{
    mbx_message_t cmd;
    s4t_instance_t *ds;
    
    ds = INSTANCE(idb);
    
    cmd.msg.cmd_byte = S4T_CMD_ENABLE_PORT;
    cmd.msg.msg_body.enable_port.port_id = idb->subunit;  
    cmd.msg.msg_body.enable_port.port_en = state;

    return(s4t_write_s5933_mailbox(ds->pa_info, &cmd));
}

/*
 * s4t_write_port_cmd
 * Set state of modem control lines and loopback.
 *
 */
static boolean s4t_write_port_cmd (hwidbtype *idb)
{
    mbx_message_t cmd;
    s4t_instance_t *ds;

    ds = INSTANCE(idb);

    cmd.msg.cmd_byte = S4T_CMD_CONTROL_PORT;
    
    cmd.msg.msg_body.control_port.port_id = idb->subunit;
    cmd.msg.msg_body.control_port.port_control = ds->port_cmd;

    return(s4t_write_s5933_mailbox(ds->pa_info, &cmd));
}

/*
 * s4t_start
 * Move s4t from stopped to running state.
 * Cause the s4t to start; wait for start to complete;
 * return false if not successful.
 *
 */
static boolean s4t_start (hwidbtype *idb) 
{
    s4t_instance_t *ds;
    
    ds = INSTANCE(idb);
    
    
    /* Enable status interrupt, rx/tx interrupts */
    ds->port_status_mask = 0;
    if (!s4t_mask_status_int(idb)) {
  	errmsg(&msgsym(STARTFAIL, S4T68360), idb->hw_namestring, 
               "clear of int mask failed");
    }
    
    /* raise control lines, loopback if configured  */
    ds->port_cmd = ds->loopback | S4T_CONTROL_RTS_CTS | S4T_CONTROL_DTR_DSR;
    if (!s4t_write_port_cmd(idb)) {
	errmsg(&msgsym(STARTFAIL, S4T68360), idb->hw_namestring, 
               "RTS/CTS not enabled");
    }
    
    /* Write init block to interface */
    if (!s4t_init_port(idb)) {
	errmsg(&msgsym(STARTFAIL, S4T68360), idb->hw_namestring,
               "init_block cmd");
	return(FALSE);
    }  
    
    ds->pa_state = RX_ENABLE | TX_ENABLE;
    
    if (!s4t_enable_port(idb, ds->pa_state)) {
	ds->pa_state = DISABLE_PORT;
	errmsg(&msgsym(STARTFAIL, S4T68360), idb->hw_namestring, 
               "enable port");
	return(FALSE);
    }
    
    if (!s4t_read_port_status(idb)) {
	errmsg(&msgsym(STARTFAIL, S4T68360), idb->hw_namestring, 
               "read port status ");
	return(FALSE);
    }
    /* port is alive */
    return(TRUE);
}
/*
 * s4t_sanitize_rxpak:
 * Sanitize the rx paktype used by the driver
 */
static void s4t_sanitize_rxpak (s4t_instance_t *ds)
{
    paktype *rx_pak = ds->rx_pak;
    particletype *mp;

    if (!rx_pak) {
	return;
    }

    /*
     * Clean up all the particles in rx_pak
     */
    while (mp = particle_dequeue_inline(rx_pak), mp) {
	retparticle(mp);
    }

    /*
     * Clean up all relevant entries in rx_pak
     */
    rx_pak->datagramsize = 0;
    rx_pak->datagramstart = NULL;
    rx_pak->particlequeue.qhead = rx_pak->particlequeue.qtail = NULL;
    rx_pak->particlequeue.count = 0;
}
/*
 * s4t_set_rx_discard :
 * Control ds->rx_discard and flush rx_pak appropriately.
 */
void s4t_set_rx_discard (s4t_instance_t *ds, boolean state)
{
    if (!state) {
	/*
	 * Clearing rx_discard. Sanitize rx_pak.
	 */
	s4t_sanitize_rxpak(ds);
    }
    ds->rx_discard = state;
}

/*
 * s4t_throttle 
 * Called by s4t_rx_interrupt() to halt 
 *    receive process & return mini-buffers, etc.
 */
void s4t_throttle (hwidbtype *idb)
{
    s4t_instance_t *ds;
 
    ds = INSTANCE(idb);

    idb_mark_throttled(idb);
    ds->throttled++;
    /*
     * Re-initialize the 4T PA and driver with the receiver disabled:
     */
     /* FIXME: if you throttle, consider just throwing away the packets
      * don't fool around w/ the PA or the rx enable.
      */
    s4t_init(idb);
}
 
/*
 * s4t_stop 
 * Stop the chip and reinitialize its registers.
 */
static void s4t_stop (hwidbtype *idb)
{
    s4t_instance_t *ds;
    leveltype level;
    int i;
    
    level = raise_interrupt_level(NETS_DISABLE);
    
    /* Write device registers so as to disable Rx & TX
       processes.
       */
    ds = INSTANCE(idb);

    /* change state to inactive */
    if (!s4t_enable_port(idb, DISABLE_PORT)) {
	errmsg(&msgsym(STOPFAIL, S4T68360), idb->hw_namestring, 
               "port did not respond to disable");
    }
    /*
     * Make sure 4T is *really* stopped
     */
    i = 0;
    while ((s4t_read_qsize(idb) > 0) && (i<100)) {
      DELAY(1); /* 
                 * Not try to poll too often, but not wait for
                 * too long either.
                 */
      i++;
    }
    if ((serial_debug) && (i == 100)) {
	errmsg(&msgsym(STOPFAIL, S4T68360), idb->hw_namestring, 
               "port did not confirm disable done after 100ms");
    }
    ds->pa_state = DISABLE_PORT;
    
    ds->port_status_mask = S4T_MASK_STATUS_INT;
    if (!s4t_mask_status_int(idb)) {
	errmsg(&msgsym(STOPFAIL, S4T68360), idb->hw_namestring, 
               "masking status INT failed");
    }
 
    /* Disable/Clear any pending interrupts */

    /* Pull modem control lines down */
    ds->port_cmd = S4T_DISABLE_MODEM_CTL;
    if (!s4t_write_port_cmd(idb)) {
	errmsg(&msgsym(STOPFAIL, S4T68360), idb->hw_namestring, 
               "disabling modem control failed");
    }

    /* Enable all interrupts */
    ds->port_status_mask = 0;
    if (!s4t_mask_status_int(idb)) {
  	errmsg(&msgsym(STOPFAIL, S4T68360), idb->hw_namestring, 
               "masking status INT failed");
    }
    
    if (!s4t_read_port_status(idb)) {
	errmsg(&msgsym(STOPFAIL, S4T68360), idb->hw_namestring, 
               "read port status ");
    }
    reset_interrupt_level(level);
    
}

/*
 * s4t_reset
 * Reset device
 *
 */
void s4t_reset (hwidbtype *idb)
{
    /* 
     * if this fails we need something stronger than 
     * a debug message,   
     * we need to signal all the way to the RP/RSP console.
     */  
    if (!s4t_reset_port(idb)) {
	if (serial_debug) {
	    buginf("\n4T(%d), PA%d: s4t_reset() failed "
		   "for interface %d.", idb->unit, idb->slot, idb->subunit);
	}
    }
}


/*
 * s4t_init
 * Initialize the interface:
 */
boolean s4t_init (hwidbtype *idb)
{
    s4t_instance_t *ds = INSTANCE(idb);
    s4t_dscrptr_t *rmd, *tmd;
    s4t_init_block_t *init_block = ds->init_block;
    leveltype level;
    particletype *mp;
    int i;
    idbtype *swidb;

    level = raise_interrupt_level(NETS_DISABLE);
  
    /*
     * stop the device.
     */
    ds->fatal_tx_err = FALSE;
    
    s4t_stop(idb);

    if (idb->state != IDBS_ADMINDOWN) {
	/*
	 * Mark idb as reset.
	 */
	net_cstate(idb, IDBS_RESET);
    }
    
    /*
     * Flush the hold queue:
     */
    holdq_reset(idb);

    if (!serial_pulse_DTR(idb)) {
	reset_interrupt_level(level);
	return(TRUE);
    }
    
    /*
     * If a DTR dialer is configured on this interface we may not want
     * to raise DTR, so return here if this is the case.
     */
    if (is_ddr(idb)) {
	if (reg_invoke_dialer_keep_dtr_low(idb)) {
	    reset_interrupt_level(level);
	    return(TRUE);
	}
    }
    
    /*
     * If interface is administratively down, don't re-enable it.
     * Note that we do this check after shutting down the device
     */
    if (idb->state == IDBS_ADMINDOWN) {
	reset_interrupt_level(level);
	return(TRUE);
    }

    /*
     * Handle bridging
     */
    switch (idb->enctype) {
      case ET_HDLC:
      case ET_PPP:
	/*
	 * Configure multicast addresses for interface on bridge table
	 */
	FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	    if (!(system_loading | bootstrap_enable))
		reg_invoke_media_interesting_addresses(swidb);
	}
	break;

      default:
	break;
    }
	
    /* After stop, so we don't mess up a DMA.
     */
    s4t_reset(idb);      
    
    ds->rx_head = ds->rx_tail = 0;	/* Rx head, tail indexes */
    ds->tx_head = ds->tx_tail = 0;	/* Tx head, tail indexes */
    ds->tx_count = 0;			/* no buffers on Tx ring */
    
    ds->tx_limited = hwidb_use_short_tx_q_inline(idb);
    if (ds->tx_limited) {
	idb->mci_txcount = 1;
    } else {
	idb->mci_txcount = S4T_TX_RING_ENTRIES;
    }
    
    /*
     * Initialize idb namestrings
     */
    if (!idb->hw_namestring) {
	idb_init_names(idb->firstsw, FALSE);
    }
    
    if (!s4t_init_buffer_pools(idb)) {
 	reset_interrupt_level(level);
	return(FALSE);
    }

    /*
     * Setup the rx ring, tx ring beginning and ending addresses
     */
    init_block->rx_beg = S4T_CNTL_ADDR(ds->rxr);
    init_block->rx_end = S4T_CNTL_ADDR(&ds->rxr[S4T_RX_RING_ENTRIES]);  
    init_block->tx_beg = S4T_CNTL_ADDR(ds->txr);
    init_block->tx_end = S4T_CNTL_ADDR(&ds->txr[S4T_TX_RING_ENTRIES]);  
    
    /*
     * Init the rx ring with particles owned by the s4t PA
     */
    for (i=0; i< S4T_RX_RING_ENTRIES; i++) {
	rmd = &ds->rxr[i];
	rmd->length = 0;

	if (ds->rxr_shadow[i]) {
	    /*
	     * There is already a particle on the shadow rx ring.
	     * Give it to the s4t.
	     */
	    mp = ds->rxr_shadow[i];
	    rmd->buf_addr = S4T_BUF_ADDR(mp->data_start);
	    rmd->status = S4T_RXD_OWN;
	} else {
	    /*
	     * There is no particle on the shadow rx ring, so 
	     * allocate it and give it to the s4t68360.
	     */
	    mp = s4t_get_particle(idb);
	    if (mp) {
		ds->rxr_shadow[i] = mp;
		rmd->buf_addr = S4T_BUF_ADDR(mp->data_start);
		rmd->status = S4T_RXD_OWN;
	    } else {
		net_cstate(idb, IDBS_DOWN);
		s4t_lineproto_change(idb, FALSE);
		reset_interrupt_level(level);
		return(FALSE);
	    }
	}
    }
    s4t_rxring_flush(ds);
    
    /*
     * Setup the tx ring and ensure that the CPU owns
     * all the buffers and/or particles
     */
    for (i=0; i< S4T_TX_RING_ENTRIES; i++) {
	
	tmd = &ds->txr[i];
	tmd->status = 0;
	tmd->buf_addr = 0;
	if (ds->txr_shadow[i].pak && 
	    (ds->txr_shadow[i].pak != S4T_MAGIC_PAK_PTR)) {
	    datagram_done(ds->txr_shadow[i].pak);
	}
	if (ds->txr_shadow[i].mp){
	    retparticle(ds->txr_shadow[i].mp);
	}
	ds->txr_shadow[i].pak = NULL;
	ds->txr_shadow[i].mp = NULL;
    }
    
    /*
     * Check for loopbacks
     */
    if (idb->loopback) {
	ds->loopback = S4T_CONTROL_LOOP;
    } else {
	ds->loopback = S4T_CONTROL_NOLOOP;
    }

    /*
     * Set up the clockrate
     */
    s4t_init_clockrate(idb);

    if (s4t_start(idb)) {
	if (s4t_line_is_up(idb)) {
	    net_cstate(idb, IDBS_UP);
	} else {
	    net_cstate(idb, IDBS_DOWN);
	}

	if (serial_debug) {
	    buginf("\n%s: Interface is alive",
		   idb->hw_namestring);
	    buginf("\n port_stat %#lx, cable_type %#lx",
		   ds->port_status,
		   ds->applique_id);
	}
    } else {
	s4t_shutdown(idb, IDBS_DOWN);
    }
   
    reset_interrupt_level(level);      
    return(TRUE);
}


/*
 * s4t_shutdown
 * Shut down an interface
 */
void s4t_shutdown (hwidbtype *idb, int newstate)
{
    s4t_instance_t *ds;
    leveltype level;
    
    ds = INSTANCE(idb);
    
    level = raise_interrupt_level(NETS_DISABLE);
    
    s4t_stop(idb);
    
    s4t_reset(idb);
    
    reset_interrupt_level(level);
    
    /* 
     *  poke the RP - we're down
     */ 
    net_cstate(idb, newstate);

    /* do this unconditionally b/c we are down */
    ds->s4t_lineup = FALSE;
    s4t_lineproto_change(idb, FALSE);
    
}

/*
 * s4t_rx_error  
 * Check the status word from a S4T receive
 *   descriptor for errors. Take the correct action for each error.
 *   Assumes that this is the last descriptor of a packet, since none
 *   of the receive descriptor errors are valid otherwise.
 *
 */
void s4t_rx_error (hwidbtype *idb, ushort rxd_status, particletype *mp)
{
    
    s4t_instance_t *ds = INSTANCE(idb);
    
    idb->counters.input_error++;
    
    if (rxd_status & S4T_RXD_EOP) { 
	/* Frame too long */
	if (rxd_status & S4T_RXD_LG) {
	    idb->counters.giants++;
	}
	
	/* Frame Alignment error */
	if (rxd_status & S4T_RXD_NO) {
	    idb->counters.input_frame++;
	}
	
	/* Ones Violation - aborts */
	if (rxd_status & S4T_RXD_AB) {
	    idb->counters.input_abort++;
	}
	/* CRC error */
	if (rxd_status & S4T_RXD_CR) {
	    idb->counters.input_crc++;
	}
	
	/* Receiver overrun */
	if (rxd_status & S4T_RXD_OV) {
	    idb->counters.input_overrun++;
	}
	
	/* Carrier Detect lost */
	if (rxd_status & S4T_RXD_CD) {
	    ds->rx_cd_lost_err++;
	}
    }
    /*
     * Clean up. If it was the last particle in the packet, clear 
     * rx_discard, otherwise set it.
     */
    
    retparticle(mp);
    if (rxd_status & S4T_RXD_EOP) {
	s4t_set_rx_discard(ds, FALSE);
    } else {
	s4t_set_rx_discard(ds, TRUE);
    }
}

/*
 * s4t_tx_error
 * Check the transmit descriptor for errors.
 * Take the correct action for each error.
 * Handle per buffer and per packet errors differently.
 *
 */
void s4t_tx_error (ushort tmd_status, uint tx_index , hwidbtype *idb)
{
    s4t_instance_t *ds = INSTANCE(idb);
    s4t_dscrptr_t *tmd;
    
    tmd = &ds->txr[tx_index];
    idb->counters.output_error++;
    
    /* TX process mini-buffer error */
    if (tmd_status & S4T_TXD_BUFF) {
	ds->tx_soft_buffer_err++;
    }
    
    if (tmd_status & S4T_TXD_FATAL_ERRS)
	ds->fatal_tx_err = TRUE;
    
    if (!ds->fatal_tx_err) {
	if (!ds->s4t_lineup) {
	    /* FIXME: everything in this block */
	    ds->s4t_lineup = TRUE; /* Transmitted ok, must be up */
	    idb->reason = "Keepalive OK";
	    net_cstate(idb, IDBS_UP);
	}
    }
    
    /*
     * Clean up the descriptor
     */
    tmd->status = 0;
}

/*
 * s4t_enable 
 * Unthrottle an interface.
 * Please note; this function must be called 
 * with NETS_DISABLED, to work correctly.
 */
void s4t_enable (hwidbtype *idb)
{
    s4t_instance_t *ds;
    
    ds = INSTANCE(idb);
    
    idb_mark_unthrottled(idb);
    ds->enabled++;		/* Count number of times enabled */
    
    
    /* Necessary to make the init code happy */
    idb->state = IDBS_UP;
    
    if (serial_debug)
	buginf("\n4T(%d) enabled: state = %d",
	       idb->subunit, idb->state);
    /*
     * Reset the s4t with the receiver enabled:
     */
    s4t_init(idb);
    s4t_linestate(idb);
}

/*
 * s4t_disable (hwidbtype *idb)
 * Respond to the disable MALU Attention
 *
 */
void s4t_disable (hwidbtype *idb)
{
    s4t_instance_t *ds;
    leveltype level;
    
    ds = INSTANCE(idb);      
    
    level = raise_interrupt_level(NETS_DISABLE);

    s4t_shutdown(idb, IDBS_ADMINDOWN);

    reset_interrupt_level(level);
}
/*
 * s4t_check_pa_health - Invoked every 10 seconds to watchdog the 4T.
 */
void s4t_check_pa_health (s4t_pa *pa)
{
    if (pa->no_response) {
	errmsg(&msgsym(RESTART, S4T68360), pa->slot);
	pa->no_response = 0;
	s4t_is_really_dead_jim(pa);
/*
 * Warning!! Do not touch anything in the pa structure after
 * calling s4t_is_really_dead_jim, since that may call the PA
 * deallocate routine, which will free that structure.
 */
    }
}


/*
 * s4t_show_structure 
 * Show 4T structure pointers
 *
 *
 */
static void s4t_show_structure (hwidbtype *idb)
{
    s4t_instance_t *ds;
    s4t_pa *pa;
    
    ds = INSTANCE(idb);

    pa = ds->pa_info;
    printf("\nInterface %s\n", idb->hw_namestring);
    printf("PAS f/w rev %d, PAS h/w rev %d, ", pa->versions.sw_version, 
	   pa->versions.hw_version);
    printf("CPU is %dMhz\n", pa->versions.cpu_speed);
    printf("idb = 0x%x, ds = 0x%x, PCI cfg_regaddr = 0x%x\n",
           idb, ds);
}

/*
 * File: s4t_show_cable_state 
 * Show the cable type.
 */
static void s4t_show_cable_state (hwidbtype *idb)
{
    s4t_instance_t *ds;
    serialsb	   *ssb;
    int            i = 0;

    ds = INSTANCE(idb);
    if (!ds) {
        return;
    }
    
    printf("\n4T(%d) s4t_linestate: %s ", idb->subunit, 
	   ((ds->s4t_lineup) ? "Up" : "Down"));
    printf(", cable type : ", idb->subunit);
    if (ds->applique_id == S4T_PA_NO_CABLE) {
        printf("None present.\n");
        return;
    }
    
    /*
     * 5/1 applique
     */
    if (ds->applique_id & 1)
	printf("DCE ");
    else {
	printf("DTE ");
    }
    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);
    /*
     * Print up the interface type
     */
    switch (ds->applique_id & ~1) {        /* convert to DTE for this test */
	
    case S4T_PA_NONE:
	printf("No");
	break;
	
    case S4T_PA_RS232_DTE:
	printf("V.24 (RS-232)");
	break;
	
    case S4T_PA_V35_DTE:
	printf("V.35");
	break;
	
    case S4T_PA_RS449_DTE:
	printf("V.11 (RS-449)");
	break;
	
    case S4T_PA_X21_DTE:
	printf("V.11 (X.21)");
	break;
	
    case S4T_PA_RS530_DTE:
	printf("RS-530");
	break;
	
    default:
	printf("Unknown cable id (%d)", ds->applique_id);
	break;
    } 
    
    printf(" cable");
    
    /*
     * If a DCE, print the clockrate too
     */
    if (!idb->dte_interface) {
        if (ssb->serial_clockindex) {
            while (i < S4T_CLOCKS) {
                if (s4t_clock[i].bitrate_code == ds->init_block->tx_bit_rate){
                    break;
		}

		i++;
	    }
	    printf(",\nclockrate %d (index=%d)",
		   s4t_clock[i].rate, ds->init_block->tx_bit_rate);
        }
    }      
    
    printf("\n");
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
}

/*
 * s4t_show_fw_counters 
 * print out the firmware counters.
 */
static void s4t_show_fw_counters (s4t_stats *stats)
{
	if (stats->updated == 0) {
	    printf("\n4T Firmware counters not updated");
	    return;
	}
	printf("\nMbox reads=%d, writes=%d",
		stats->mbox_read, stats->mbox_write);
	printf(", 4T->PCI %d, PCI->4T %d",
		stats->dma_write, stats->dma_read);
	printf("\nMRAR err=%d, MWAR err=%d, MRTC err=%d, MWTC err=%d,",
			stats->dma_mrar, stats->dma_mwar,
			stats->dma_mrtc, stats->dma_mwtc);
	printf("\nhost interrupts=%d, no mem=%d, DMA retries=%d",
	       stats->interrupts, stats->dma_nomem, stats->dma_spurint);
	printf("\nLine changes=%d, Free memory=%d bytes, MTU=%d, IB copy=0x%x",
	       stats->line_change, stats->freemem,
	       stats->init_copy.mtu, &stats->init_copy);
	printf("\nTX: Ring polls=%d, updates=%d, pending=%d, head=%d"
		", ring err=%d",
	       stats->tx_ring_polled, stats->tx_ring_updated,
	       stats->tx_pending, stats->tx_head, stats->tx_bad_buf);
	printf("\n  bufs sent=%d, errs=%d, too big=%d, scc inuse=%d"
	       ", spurints=%d",
	       stats->tx_pkt, stats->tx_err,
	       stats->tx_toobig, stats->tx_scc_used, stats->tx_spurint);
	if (stats->tx_sp3 || stats->tx_sp4 || stats->tx_sp5)
	    printf("\n  sp3=0x%x, sp4=0x%x, sp5=0x%x",
		   stats->tx_sp3, stats->tx_sp4, stats->tx_sp5);
	printf("\nRX: Ring polls=%d, updates=%d, ring err=%d, head=%d",
	       stats->rx_ring_polled, stats->rx_ring_updated,
	       stats->rx_bad_ring, stats->rx_head);
	printf(
	 "\n  rx bufs=%d, rx pkts=%d, errs=%d, frame=%d, nobufs=%d, scc hd=%d",
	       stats->rx_buf, stats->rx_pkt, stats->rx_err,
	       stats->rx_framing, stats->rx_dropped, stats->rx_scc_head);
	if (stats->rx_sp2 || stats->rx_sp3 || stats->rx_sp4 || stats->rx_sp5)
	    printf("\n  sp2=0x%x, sp3=0x%x, sp4=0x%x, sp5=0x%x, sp6=0x%x",
		   stats->rx_sp2, stats->rx_sp3, stats->rx_sp4, stats->rx_sp5);
}
#ifdef	S4T_FW_DEBUG
static hwidbtype *s4t_idb_debug;

void hook_s4t(void)
{
	if (s4t_idb_debug)
		(void)s4t_update_stats(s4t_idb_debug);
}
#endif

/*
 *
 * s4t_show_controller
 * Show 4T status. Return true if we quit the display.
 *
 */
boolean s4t_show_controller (hwidbtype *idb, parseinfo *csb)
{
    s4t_instance_t *ds = INSTANCE(idb);
    s4t_instance_t *ds_snap;
    s4t_pa *pa;
    leveltype level;
#ifdef S4T_FW_DEBUG
    union {
        uchar b[8];
        ulong i[2];
        } tracecode;

    s4t_idb_debug = idb;
#endif

    /*
     *	Send a message to the 4T requesting that it update its
     *	stats counters.
     */
    if (ds->fw_stats)
	(void)s4t_update_stats(idb);
    
    pa = ds->pa_info;
    s4t_show_structure(idb);
    
    printf("IDB type = 0x%x", idb->type);
    printf(", status = 0x%x", idb->status);
    printf(", maxdgram =  %d\n ", idb->max_buffer_size);

    /*
     * Read and show the cable related info.
     */
    s4t_read_port_status(idb);
    if (pa->no_response) {
	printf("No response from 4T - aborted\n");
	return(TRUE);
    }

    s4t_show_serial_state(idb, NULL);

    s4t_show_cable_state (idb);
#ifdef S4T_FW_DEBUG
    if (s4t_get_trace (ds->pa_info, &tracecode.i[0]))
    printf("f/w trace code = %d, %d, %d, %d (0x%x, 0x%x)\n", tracecode.b[0],
	    tracecode.b[1], tracecode.b[2], tracecode.b[3],
	    tracecode.i[0], tracecode.i[1]);
	else
	    printf("Trace code request failed!\n");
#endif
    /*
     * Take a snapshot of the rx ring, rx shadow ring, tx ring, tx shadow
     */
    ds_snap = malloc(sizeof(s4t_instance_t));
    if (!ds_snap) {
	return(TRUE);
    }
    
    /*
     * Lock out interrupts and copy the data structures. Then re-enable
     * network interrupts.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    bcopy(ds, ds_snap, sizeof(s4t_instance_t));
    reset_interrupt_level(level);

    /*
     * Print out the addresses of some stuff
     */
    printf("s4t_ds=%#x, pa_info=%#x, ib=%#x\n", ds, pa, ds->init_block);
    printf("PCI reg=%#x, bus=%d, D/L time=%d\n",
	   pa->devbase, pa->bus, pa->download_delay);
    printf("\nRX ring=%#x, shadow=%#x, entries=%d, head=%d, tail=%d",
	   ds_snap->rxr, ds_snap->rxr_shadow, S4T_RX_RING_ENTRIES,
	   ds_snap->rx_head, ds_snap->rx_tail);
    printf("\nTX ring=%#x, shadow=%#x, entries=%d, head=%d, tail=%d",
	   ds_snap->txr, ds_snap->txr_shadow, S4T_TX_RING_ENTRIES,
	   ds_snap->tx_head, ds_snap->tx_tail);

    s4t_show_hw_instance(ds_snap);

    printf("\nthrottled=%d, enabled=%d\n", ds_snap->throttled,
	   ds_snap->enabled);
    printf(" rx_cd_lost_err=%d, rx_no_enp=%d\n",
	   ds_snap->rx_cd_lost_err, ds_snap->rx_no_enp);

    printf(" tx_limited=%d, tx_started=%d, tx_fullring=%d, tx_count=%d\n",
	   ds_snap->tx_limited, ds_snap->tx_started,
	   ds_snap->tx_fullring, ds_snap->tx_count);

    if (ds->fw_stats)
	s4t_show_fw_counters(ds->fw_stats);
#ifdef S4T_FW_DEBUG
    if (idb->subunit == 3)
	s4t_force_crash(ds->pa_info);
#endif

    printf("\n");
    /*
     * Release all memory
     */
    free(ds_snap);
    return(FALSE);
}
/**********************************************************************
 * File: s4t_subsys_init -
 * Inputs:
 * Outputs:
 * Description:	s4t subsystem initialisation entry point
 *
 * Change History:
 * 03/28/95   H.M. Robison   Added invocation of 'analyze_interface'
 *                           as well as 'analyze_device'.
 *                           Note:  The 4T has 1 PCI device, but 4
 *                           serial I/O devices.
 **********************************************************************/
static void s4t_subsys_init (subsystype *subsys)
{
    /* register the PCI 4T PA */

    /* PA_HARDWARE_4T is defined in ../pas/if_pas.h.
       This defines the cisco PA type, that should
       be in the 4T EEPROM.
    */
    reg_add_analyze_interface(PA_HARDWARE_4T,
                              pci_s4t_pa_analyze_interface,
                              "pci_s4t_pa_analyze_interface");
}

/*
 * s4t subsystem header
 */
#define S4T_MAJVERSION   1
#define S4T_MINVERSION   0
#define S4T_EDITVERSION  1


SUBSYS_HEADER(s4t, S4T_MAJVERSION, S4T_MINVERSION,
              S4T_EDITVERSION,
              s4t_subsys_init, SUBSYS_CLASS_DRIVER,
              NULL,
              NULL);






