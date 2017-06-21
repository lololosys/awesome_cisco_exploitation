/* $Id: c7100.c,v 3.2.58.14 1996/08/28 13:11:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100.c,v $
 *------------------------------------------------------------------
 * c7100.c - Predator support
 *
 * July 1995, Michael Beesley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100.c,v $
 * Revision 3.2.58.14  1996/08/28  13:11:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.58.13  1996/08/20  00:10:24  mbeesley
 * CSCdi66438:  show version should display amount of packet sram
 * Branch: California_branch
 *
 * Revision 3.2.58.12  1996/06/26  18:40:09  mbeesley
 * CSCdi61260:  Make output of show c7200 more usefull to manufacturing
 * Branch: California_branch
 *
 * Revision 3.2.58.11  1996/06/22  03:33:22  mbeesley
 * CSCdi61026:  PCI PERR/SERR is now supported, use it
 * Branch: California_branch
 *
 * Revision 3.2.58.10  1996/06/17  23:36:54  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.2.58.9  1996/05/24  01:37:56  mbeesley
 * CSCdi57393:  GT64010 DMA engines can hog MB0 bandwidth
 * Branch: California_branch
 *
 * Revision 3.2.58.8  1996/05/24  00:17:12  mbeesley
 * CSCdi57936:  GT64010 Errors should reload the box
 * Branch: California_branch
 *
 * Revision 3.2.58.7  1996/05/21  20:43:58  dcarroll
 * CSCdi55856:  %ENVM-3-NOFORK: Could not start environment polling process
 * Branch: California_branch
 * Turn the envmon process back on.
 *
 * Revision 3.2.58.6  1996/05/21  06:39:36  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.2.58.5  1996/05/01  13:28:15  dcarroll
 * CSCdi54895:  oir fix for ethernet port adapter
 * Branch: California_branch
 * OIR support for pas ethernets.
 *
 * Revision 3.2.58.4  1996/04/23  19:21:49  mbeesley
 * CSCdi55360:  Enable cached, write thru view of packet memory
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/04/06  02:05:38  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Make changes to deal with the change in the product name for Predator
 * from c7100 to c7200.
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/04/04  09:37:57  smackie
 * Un-obfuscate buffer copy code for prolonged simplicity. Remove
 * misguided internal particle structure from paktype. Add a discrete
 * pak_coalesce() call for particle re-assembly. (CSCdi51223)
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:47:34  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.13  1996/03/13  03:59:40  mbeesley
 * CSCdi50648:  Flag for IP ttl/checksum check is lost going to process
 * level. Do not let old flags settings ghost in the recieve paktype.
 * Branch: ELC_branch
 *
 * Revision 3.2.2.12  1996/03/09  01:06:50  mbeesley
 * CSCdi51152:  DMA ring in SRAM should swap entries, not CPU
 * Branch: ELC_branch
 *
 * Revision 3.2.2.11  1996/03/08  21:36:23  mbeesley
 * CSCdi51090:  Need to support rev 2 CPU card
 * Branch: ELC_branch
 *
 * Revision 3.2.2.10  1996/02/19  23:24:18  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.2.2.9  1996/02/16  02:23:57  mbeesley
 * CSCdi48910:  Add storage of fault history for crashes
 * Branch: ELC_branch
 *
 * Revision 3.2.2.8  1996/02/15  00:06:46  mbeesley
 * CSCdi49037:  IO dram mapping incorrect with more than 24 meg
 *         o Fix mapping of IO dram with more than 24 meg of dram
 *         o Disable cached view of IO dram
 *         o Issue MB0 reset at init time (helps PCMCIA master abort)
 * Branch: ELC_branch
 *
 * Revision 3.2.2.7  1996/02/13  01:43:22  mbeesley
 * CSCdi48778:  IO DRAM gets exhausted
 *         o Reduce AMDp2 and FDDI rx particle pool
 *         o Allocate more DRAM for IO on small images
 *         o Add cached view of packet DRAM
 *         o Use packet sram as a fast heap on rev 1 boxes
 * Branch: ELC_branch
 *
 * Revision 3.2.2.6  1996/01/26  22:10:33  mbeesley
 * Branch: ELC_branch
 *         o Debug GT64010 timers and add support for counters
 *         o Increase GT64010 cache line size
 *         o Finish support for fine granularity throttling
 *
 * Revision 3.2.2.5  1996/01/23  02:18:09  mbeesley
 * Branch: ELC_branch
 * Add support for the CPU ID EEPROM. Postpone PCI SRAM initialization
 * until the rev of the CPU card has been read and verified.
 *
 * Revision 3.2.2.4  1996/01/09  21:27:43  mbeesley
 * Branch: ELC_branch
 * Enhance/fix Midplane EEPROM utility. Protect against left over
 * interrupts from boot helper image. Fix some gt64010 register naming
 * mistakes.
 *
 * Revision 3.2.2.3  1995/12/13  02:23:47  mbeesley
 * Branch: ELC_branch
 * Fix misdocumented register bit defintions. Add error interrupt
 * masks. Clear OIR interrupts at boot time. Ensure that PCI Signals
 * Invalid is not set at port adaptor power on.
 *
 * Revision 3.2.2.2  1995/12/05  20:00:05  mbeesley
 * Branch: ELC_branch
 * Correct some register initialization. Add support for dynamic
 * parallel bridge config. Do not use a small piece of SRAM as
 * fast memory on Rev 2 cards.
 *
 * Revision 3.2.2.1  1995/11/22  21:43:01  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Includes
 */
#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "stacks.h"
#include "ttysrv.h"
#include "sys_registry.h"
#include "config.h"
#include "parser.h"
#include "packet.h"

#include "../dev/monitor1.h"

#include "../os/gdb.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../hes/dgbus.h"

#include "../snmp/chassismib_registry.h"

#include "../pas/if_pas.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */

#include "../pas/pas_id.h"

#include "../src-4k/vspace_4k.h"
#include "../src-4k/gt64010.h"
#include "../src-4k/gt64010_dma.h"

#include "c7100_fpga.h"

/*
 * Global variables
 */
boolean c7100_debug;
tinybool c7100_netint_throttle_configured;	/* is it configured */

/*
 * Local storage
 */
static tinybool 	netint_throttle_active;	/* is net intr masked */
static ulong 		netint_throttle_count;	/* throttle activation count */
static ulong 		netint_timer_count;	/* timer intr. count */
static ulong 		netint_ticks;		/* max ticks in intr */
static ulong		netint_mask_ticks;	/* ticks to mask intr */
static ulong 		netint_mask_usec;	/* usecs to mask intr */
static ulong		netint_mask_gtusec;	/* gt64010 format usec */
static usec_timestamp	netint_exit;		/* when to mask intr */
static usec_timestamp	netint_return;		/* last time net intr retr. */
static dgbus_eeprom_t 	cpu_eeprom_copy;	/* copy of CPU ID eeprom */
static dgbus_eeprom_t 	*cpu_eeprom_ptr;	/* pointer to copy of CPU 
						   ID eeprom contents */

/*
 * CPU Card ID EEPROM
 */
static pas_management_t c7100_cpu_eeprom = {
    -1,						/* Not a bay */

    sizeof(C7100_IO_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    SK1_CLOCK_CPU,				/* Clock mask */
    &C7100_IO_FPGA->serial_eprom_ctrl,		/* Clock control register */

    sizeof(C7100_IO_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    CS1_CHIP_SEL_CPU,				/* Select mask */
    &C7100_IO_FPGA->serial_eprom_ctrl,		/* Select control register */

    sizeof(C7100_IO_FPGA->serial_eprom_ctrl),   /* 8 bit register */
    DI1_DATA_IN_CPU,				/* DATAIN mask */
    &C7100_IO_FPGA->serial_eprom_ctrl,		/* DATAIN register */

    sizeof(C7100_IO_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    DO2_DATA_OUT_CPU,				/* DATAOUT mask */
    &C7100_IO_FPGA->serial_eprom_ctrl		/* DATAOUT register */
};

/*
 * throttle_netio:
 * Either enable or disable netio interrupts. When TRUE, disable
 * the interrupts. Else enable them. Note, this is called from process
 * level, netio level, timer interrupt level and OIR interrupt level.
 */
static inline void throttle_netio (boolean throttle)
{
    if (throttle) {
	/*
	 * Turn all network IO interrupts off
	 */
	C7100_MP_FPGA->pkt_int_enbl_lsw = 0;
	C7100_MP_FPGA->pkt_int_enbl_nsw = 0;
    } else {
	/*
	 * Turn network IO interrupts on
	 */
	C7100_MP_FPGA->pkt_int_enbl_lsw = 0xffffffff;
	C7100_MP_FPGA->pkt_int_enbl_nsw = 0xffffffff;
    }
}

/*
 * throttle_off_inline:
 * Re-enable netio interrupts.
 */
static inline void throttle_off_inline (void)
{
    netint_throttle_active = FALSE;
    USEC_GET_TIMESTAMP(netint_exit);
    netint_exit += netint_ticks;
    throttle_netio(FALSE);
}

/*
 * netint_throttle_off:
 * Re-enable netio interrupts. Called from timer tick.
 */
static void netint_throttle_off (void)
{
    netint_timer_count += 1;
    throttle_off_inline();
}

/*
 * process_throttle_off:
 * Re-enable netio interrupts. Called from scheduler and shutdown code.
 */
static void process_throttle_off (void)
{
    throttle_off_inline();
}

/*
 * c7100_netint_throttle_init:
 * Initialize net IO interrupt throttling
 */
static void c7100_netint_throttle_init (void)
{
    /*
     * Install a GT64010 timer
     */
    gt64010_install_timer(GT64010_TIMER_0, netint_throttle_off);

    /*
     * Init timestamps
     */
    USEC_GET_TIMESTAMP(netint_return);
    netint_exit = netint_return;

    /*
     * Init variables
     */
    netint_throttle_active = FALSE;
    netint_throttle_count = 0;
    netint_timer_count = 0;
    netint_ticks = R4K_USEC2COUNT(NETINT_DEFAULT);
    netint_mask_usec = NETINT_MASK_DEFAULT;
    netint_mask_ticks = R4K_USEC2COUNT(netint_mask_usec);
    netint_mask_gtusec = GT64010_USEC(netint_mask_usec);
    c7100_netint_throttle_configured = NETINT_THROTTLE_DEFAULT;

    reg_add_hardware_shutdown(process_throttle_off, "process_throttle_off");
    reg_add_hardware_shutdown(fault_history_purge, "fault_history_purge");
}

/*
 * c7100_netint_throttle_setup:
 * Called at the beginning of the net IO intr loop. Setup the exit timer
 * for the interrupt handler
 */
void c7100_netint_throttle_setup (void)
{
    ulong diff;
    usec_timestamp now;

    USEC_GET_TIMESTAMP(now);
    diff = now - netint_return;
    if (diff >= netint_mask_ticks) {
	netint_exit = now + netint_ticks;
    } else {
	netint_exit += diff;
    }
}

/*
 * c7100_netint_throttle_check:
 * Called from the net IO interrupt handler every time it loops.
 * Check to see if we need to disable the interrupts. If we disabled the
 * interrupts, return TRUE.
 */
boolean c7100_netint_throttle_check (void)
{
    if (USEC_AWAKE(netint_exit)) {

	/*
	 * Account for it and disable the interrupts
	 */
	netint_throttle_active = TRUE;
	netint_throttle_count += 1;
	throttle_netio(TRUE);

	/*
	 * Startup the GT64010 timer to allow re-enabling of
	 * the net IO interrupt
	 */
	gt64010_start_timer(GT64010_TIMER_0, netint_mask_gtusec);

	/*
	 * Assuming that the interrupt handler will finish quickly
	 * we can fire ahead and set the netint_return timer
	 */
	USEC_GET_TIMESTAMP(netint_return);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * c7100_oir_throttle:
 * Control net IO interrupts for OIR processing. If onoff is TRUE, 
 * we are being called from OIR process level and we need to re-enable
 * net IO interrupts (with throttling if it is configured). If onoff
 * is FALSE, we are being called from the OIR interrupt and we need
 * to shutdown net IO interrupts.
 */
void c7100_oir_throttle (boolean onoff)
{
    leveltype level;

    if (onoff) {

	/*
	 * Re-enable stuff, called from process level
	 */
	level = raise_interrupt_level(ALL_DISABLE);
	netint_throttle_active = FALSE;
	USEC_GET_TIMESTAMP(netint_return);
	netint_exit = netint_return;
	throttle_netio(FALSE);
	reset_interrupt_level(level);

    } else {
	
	/*
	 * Turn net IO off, called from OIR interrupt
	 */
	netint_throttle_active = TRUE;
	gt64010_stop_timer(GT64010_TIMER_0);
	throttle_netio(TRUE);
    }
}

/*
 * c7100_sched_loop_end:
 * Called at the bottom of the scheduler loop. Turns off throttling
 * IE turns net IO interrupts back on if active and nothing is
 * happening at process level
 */
void c7100_sched_loop_end (boolean ran_norm)
{
    if (c7100_netint_throttle_configured) {
	if (!ran_norm && netint_throttle_active) {
	    process_throttle_off();
	}
    }
}

/*
 * sched_alloc_command:
 * Handle "scheduler allocate" config command
 */
void sched_alloc_command (parseinfo *csb)
{
    if (csb->nvgen) {
	/*
	 * emit a config line if any value differs from its default
	 */
	if ((c7100_netint_throttle_configured != NETINT_THROTTLE_DEFAULT) ||
	    (netint_ticks != R4K_USEC2COUNT(NETINT_DEFAULT)) ||
	    (netint_mask_usec != NETINT_MASK_DEFAULT)) {

	    nv_write(TRUE, "%s%s",
		     (c7100_netint_throttle_configured) ? "" : "no ",
		     csb->nv_command);
	    /*
	     * emit parameters only if it's ON
	     */
	    if (c7100_netint_throttle_configured)
		nv_add(TRUE, " %u %u",
		       R4K_COUNT2USEC(netint_ticks), netint_mask_usec);
	}
	return;
    }

    if (csb->set_to_default) {
	csb->sense = NETINT_THROTTLE_DEFAULT;
	GETOBJ(int,1) = 0;
    }
    if (csb->sense && !c7100_netint_throttle_configured) {
	/*
	 * extra init if it's being switched on
	 */
	USEC_GET_TIMESTAMP(netint_return);
	netint_exit = netint_return;
    }
    if (GETOBJ(int,1) == 0) {
	netint_ticks = R4K_USEC2COUNT(NETINT_DEFAULT);
	netint_mask_usec = NETINT_MASK_DEFAULT;
    } else {
	netint_ticks = R4K_USEC2COUNT(GETOBJ(int,1));
	netint_mask_usec = GETOBJ(int,2);
    }
    netint_mask_gtusec = GT64010_USEC(netint_mask_usec);
    netint_mask_ticks = R4K_USEC2COUNT(netint_mask_usec);
    c7100_netint_throttle_configured = csb->sense;
}

/*
 * c7100_netint_throttle_show:
 * Display info about network IO interrupt throttling
 */
void c7100_netint_throttle_show (void)
{
    printf("C7200 Network IO Interrupt Throttling:\n");
    printf(" throttle count=%u, timer count=%u\n",
	   netint_throttle_count, netint_timer_count);
    printf(" active=%u, configured=%u\n", 
	   netint_throttle_active, c7100_netint_throttle_configured);
    printf(" netint usec=%u, netint mask usec=%u\n",
	   R4K_COUNT2USEC(netint_ticks), netint_mask_usec);
}

/*
 * flash_device_slot:
 * No slot really controls the flash, although it is on the IO card
 */
int flash_device_slot (char *devnm)
{
    return(NO_SLOTNUMBER);
}

/*
 * c7100_pcmcia_reset:
 * Take the PCMCIA card out of reset
 */
static void c7100_pcmcia_reset (void)
{
    C7100_IO_FPGA->io_ctrl_reg &= ~PCMCIA_RESET;
    flush_io_wbuffer();
    usecdelay(5);		/* give it a break */
}

/*
 * c7100_setup_hardware_registers:
 * Do any hardware register setup needed
 */
void c7100_setup_hardware_registers (void)
{
    /*
     * Setup the PCMCIA management interrupt and PCI errors
     */
    C7100_MP_FPGA->mgmt_int_enbl_lsw |= 
	(PCMCIA_INTR_ENABLE | MGMT_INTR_ENABLE_PCI_ERRORS);

    /*
     * Setup the diagnostic leds. Turn them off (active low)
     */
    C7100_IO_FPGA->led_reg |= LEDS_OFF;

    /*
     * Take the IO card out of reset
     */
    C7100_MP_FPGA->pa_ctrl_lsw |= IOSLOT_RESET;

    /*
     * Setup hardware shutdown callbacks
     */
    reg_add_hardware_shutdown(c7100_pcmcia_reset, "c7100_pcmcia_reset");

    /*
     * Init the network IO throttling mechanism
     */
    c7100_netint_throttle_init();
}

/*
 * c7100_pkt_sram_error:
 * Handle an interrupt from the packet SRAM subsystem
 * indicating that an erronous cycle was cleaned up. Return TRUE
 * if the box should be taken down.
 */
static boolean c7100_pkt_sram_error (r4k_context_t *ptr, ulong error, 
				     ulong oir_event)
{
    /*
     * Acknowledge the interrupt
     */
    C7100_MP_FPGA->err_int_stat_lsw = C7100_PKT_SRAM_ERR_MASK;

    /*
     * Crash the box
     */
    return(TRUE);
}

/*
 * c7100_pci_error_interrupt:
 * Process a PCI Serr/Perr error
 */
static boolean c7100_pci_error_interrupt (ulong error)
{
    /*
     * PCMCIA card insertion/removals occaisonally cause a PCI system/parity 
     * error. The root cause in hardware is not clear yet. So, if the PCI 
     * error happens when PCMCIA is being read, ignore the error.
     */
    if (pcmcia_read_flag) {
        return(FALSE);
    }

    /*
     * Check for PCI Perr/Serr
     */
    if (error & MGMT_INTR_STATUS_PCI_PERR) {
 	ttyprintf(CONTTY, "\n%%ERR-1-PERR: PCI bus parity error");
    }
    if (error & MGMT_INTR_STATUS_PCI_SYSERR) {
 	ttyprintf(CONTTY, "\n%%ERR-1-SERR: PCI bus system/parity error");
    }
    
    /*
     * Crash the box
     */
    return(TRUE);
}
 

/*
 * c7100_error_interrupt:
 * Handle error interrupts. Note, this handler runs at an interrupt
 * level above ALL_DISABLE.
 */
static void c7100_error_interrupt (r4k_context_t *ptr)
{
    ulong error, oir_event, envmon_event, pci_event, mgmt_event;
    int crash = 0;

    /*
     * We can come here for several reasons :
     * 1) GT64010 error interrupt
     * 2) Packet SRAM error interrupt
     * 3) OIR error interrupt
     * 4) Power supply failure error interrupt
     * 5) Fatal PCI error interrupt
     */
    error = C7100_MP_FPGA->err_int_stat_lsw;
    oir_event = (error & C7100_OIR_EVENT_MASK);
    mgmt_event = C7100_MP_FPGA->mgmt_int_stat_lsw;
    envmon_event = mgmt_event & MGMT_INTR_STATUS_PS_FAIL;
    pci_event = mgmt_event & MGMT_INTR_STATUS_PCI_ERRORS;

    /*
     * Check for GT64010 errors
     */
    if (error & C7100_GT64010_ERR_MASK) {
	crash |= gt64010_error_interrupt(ptr);
    }

    /*
     * Check for packet memory addr errors
     */
    if (error & C7100_PKT_SRAM_ERR_MASK) {
	crash |= c7100_pkt_sram_error(ptr, error, oir_event);
    }

    /*
     * Check for OIR events
     */
    if (oir_event) {
	if (system_running) {
	    crash |= c7100_oir_interrupt(oir_event);
	} else {
	    C7100_MP_FPGA->err_int_stat_lsw = C7100_OIR_EVENT_MASK;
	}
    }

    /*
     * Check for ENVMON events
     */
    if (envmon_event) {
	C7100_MP_FPGA->mgmt_int_stat_lsw = envmon_event;
	crash |= c7100_envmon_interrupt(envmon_event);
    }

    /*
     * Check for PCI fatal errors
     */
    if (pci_event) {
  	C7100_MP_FPGA->mgmt_int_stat_lsw = pci_event;
  	crash |= c7100_pci_error_interrupt(pci_event);
    }

    /*
     * If crash is asserted, bring down the box 
     */
    if (crash) {
	ttyprintf(CONTTY,
		  "\n%%ERR-1-FATAL: Fatal error interrupt, reloading\n");
	ttyprintf(CONTTY, " err_stat=%#x, err_enable=%#x, mgmt_event=%#x\n",
		  error, C7100_MP_FPGA->err_int_enbl_lsw, mgmt_event);

	/*
	 * Mask all error interrupts
	 */
	C7100_MP_FPGA->err_int_enbl_lsw = 0;
	C7100_MP_FPGA->mgmt_int_enbl_lsw = 0;
	flush_io_wbuffer();

	/*
	 * Return to ROM monitor
	 */
	r4k_return_to_monitor(SIGERROR, 0, ptr);
    }
}

/*
 * c7100_exception_init:
 * Called from platform_exception_init to setup hw specific interrupt
 * vectors.
 */
void c7100_exception_init (void)
{
    volatile ulong *ptr;

    /*
     * Reset MB0
     */
    ptr = (volatile ulong *)(ADRSPC_MB0_RESET);
    *ptr = 0;					/* activate reset */
    usecdelay(50);				/* wait */
    *ptr = 0;					/* deactivate reset */

    /*
     * Init errors
     */
    createlevel(LEVEL_ERROR,
		(void(*)(void)) c7100_error_interrupt, "OIR/Error Interrupt");

    /*
     * Reset pcmcia
     */
    c7100_pcmcia_reset();

    /*
     * Reset any pending OIR events and disable all interrupts
     * from network devices (IO and management).
     */
    C7100_MP_FPGA->err_int_stat_lsw = C7100_OIR_EVENT_MASK;
    C7100_MP_FPGA->pkt_int_enbl_lsw = 0;
    C7100_MP_FPGA->mgmt_int_enbl_lsw = 0;

    /*
     * Initialize Predators PCI subsystem
     */
    c7100_pci_init();
}

/*
 * c7100_map_iomemory:
 * Map DRAM packet memory into virtual memory map.
 */
boolean c7100_map_iomemory (int memsize, int iomemsize) 
{
    struct r4k_tlb_t tlb_eu, tlb_ec;
    long pmask, bytes, bytes1, pfn0, pfn1;
    ulong addr, caddr;

    /*
     * Map the packet DRAM. Physically and virtually it should
     * start at the end of physical memory used for text, data, bss
     * and the pmem heap. It is iomemsize bytes big. Map it uncached.
     *
     * Also create a mapping for it cached, but without L2 cache
     */
    addr = memsize;
    caddr = ADRSPC_CACHED_IODRAM + memsize;

    tlb_eu.lo0.c = tlb_eu.lo1.c = TLB_UNCACHED;
    tlb_eu.lo0.d = tlb_eu.lo1.d = TLB_RW;
    tlb_eu.lo0.v = tlb_eu.lo1.v = TLB_VALID;
    tlb_eu.lo0.g = tlb_eu.lo1.g = TLB_GLOBAL;

    tlb_ec.lo0.c = tlb_ec.lo1.c = TLB_CACHE_WT;
    tlb_ec.lo0.d = tlb_ec.lo1.d = TLB_RW;
    tlb_ec.lo0.v = tlb_ec.lo1.v = TLB_VALID;
    tlb_ec.lo0.g = tlb_ec.lo1.g = TLB_GLOBAL;

    while (iomemsize > 0) {
	if (tlb_entries_used >= MAX_TLB_ENTRIES) {
	    return(FALSE);
	}
	pmask = pick_tlb_page(addr, iomemsize, &pfn0, &pfn1, &bytes);
	if (pmask == -1) {
	    return(FALSE);
	}

	/*
	 * Setup the uncached view
	 */
	tlb_eu.lo0.pfn = pfn0;
	tlb_eu.lo1.pfn = pfn1;
	tlb_eu.pmask.mask = pmask;
	tlb_eu.hi.vpn2 = (addr >> TLB_HI_VPN2SHIFT);
	r4k_settlb(&tlb_eu, tlb_entries_used, FALSE);
	tlb_entries_used += 1;

	/*
	 * Setup the cached view
	 */
	if (tlb_entries_used >= MAX_TLB_ENTRIES) {
	    return(FALSE);
	}
	pmask = pick_tlb_page(addr, iomemsize, &pfn0, &pfn1, &bytes1);
	if (pmask == -1) {
	    return(FALSE);
	}
	if (bytes1 != bytes) {
	    return(FALSE);
	}
	tlb_ec.lo0.pfn = pfn0;
	tlb_ec.lo1.pfn = pfn1;
	tlb_ec.pmask.mask = pmask;
	tlb_ec.hi.vpn2 = (caddr >> TLB_HI_VPN2SHIFT);
	
	tlb_ec.lo0.pfn |= (ADDR_BIT33_ONE << ADDR_BIT35_32_SHIFT);
	tlb_ec.lo1.pfn |= (ADDR_BIT33_ONE << ADDR_BIT35_32_SHIFT);
	r4k_settlb(&tlb_ec, tlb_entries_used, FALSE);
	tlb_entries_used += 1;

	/*
	 * Increment the pointers
	 */
	addr += bytes;
	caddr += bytes;
	iomemsize -= bytes;
    }
    return(TRUE);
}

/*
 * c7100_read_cpueeprom:
 * Read the CPU card's ID eeprom
 */
void c7100_read_cpueeprom (void *eeprom)
{
    pas_access_sys_eeprom(&c7100_cpu_eeprom, eeprom, PAS_EEPROM_NMC93C46,
			  PAS_EEPROM_CMD_READ, 
			  (sizeof(dgbus_eeprom_t) >> 1));
}

/*
 * c7100_write_cpueeprom:
 * Write the CPU card's ID eeprom
 */
void c7100_write_cpueeprom (void *eeprom)
{
    pas_access_sys_eeprom(&c7100_cpu_eeprom, eeprom, PAS_EEPROM_NMC93C46,
			  PAS_EEPROM_CMD_WRITE, 0);
}

/*
 * c7100_dump_ideeprom:
 * Hex dump an ID eeprom (32 bytes)
 */
void c7100_dump_ideeprom (uchar *ep)
{
    printf("\n\tEEPROM contents (hex):");
    printf("\n\t  0x20: %02x %02x %02x %02x %02x %02x %02x %02x "
	   "%02x %02x %02x %02x %02x %02x %02x %02x",
	   ep[0], ep[1], ep[2], ep[3], ep[4], ep[5], ep[6], ep[7],
	   ep[8], ep[9], ep[10], ep[11], ep[12], ep[13], ep[14], ep[15]);
    printf("\n\t  0x30: %02x %02x %02x %02x %02x %02x %02x %02x "
	   "%02x %02x %02x %02x %02x %02x %02x %02x\n",
	   ep[16], ep[17], ep[18], ep[19], ep[20], ep[21], ep[22], ep[23],
	   ep[24], ep[25], ep[26], ep[27], ep[28], ep[29], ep[30], ep[31]);
}

/*
 * c7100_show_cpueeprom:
 * Print out the contents of the CPU ID EEPROM
 */
void c7100_show_cpueeprom (void)
{
    dgbus_eeprom_t ep;
    ulong serial_number;
    uchar revision;

    /*
     * Read the EEPROM
     */
    pas_access_sys_eeprom(&c7100_cpu_eeprom, &ep, 
			  PAS_EEPROM_NMC93C46, PAS_EEPROM_CMD_READ, 
			  (sizeof(dgbus_eeprom_t) >> 1));

    /*
     * Print the gathered information.
     */
    printf("\nC7200 CPU EEPROM:");
    revision = ep.board_revision;
    serial_number = ep.serial_number_31_24 << 24 |
	 	    ep.serial_number_23_16 << 16 |
		    ep.serial_number_15_8 << 8   |
                    ep.serial_number_7_0;

    printf("\n\tHardware revision %d.%10d\t", 
	   ep.hardware_version_high, ep.hardware_version_low);
    if (revision < 0x50 || revision == 0xff) {
        printf("Board revision UNKNOWN");
    } else {
        printf("Board revision %c%d", 'A' + ((revision - 0x50) / 8),
	       revision & 0x7);
    }
    printf("\n\tSerial number     %10lu\tPart number    %02u-%04u-%02u",
	   serial_number, ep.part_number_31_24, 
	   ((ep.part_number_23_16 << 8) | ep.part_number_15_8),
	   ep.part_number_7_0);
    printf("\n\tTest history      0x%10x\tRMA number     %02d-%02d-%02d",
	   ep.test_history, ep.rma_number_23_16, ep.rma_number_15_8, 
	   ep.rma_number_7_0);
    printf("\n\tEEPROM format version %d", ep.eeprom_version);
    c7100_dump_ideeprom((uchar *)&ep);
}

/*
 * c7100_cpucard_version:
 * Read the CPU EEPROM and return the version
 */
int c7100_cpucard_version (void)
{
    if (!cpu_eeprom_ptr) {
	cpu_eeprom_ptr = &cpu_eeprom_copy;
	pas_access_sys_eeprom(&c7100_cpu_eeprom, cpu_eeprom_ptr, 
			      PAS_EEPROM_NMC93C46,
			      PAS_EEPROM_CMD_READ, 
			      (sizeof(dgbus_eeprom_t) >> 1));
    }

    /*
     * The CPU card must be an NPE100 or 150
     */
    if ((cpu_eeprom_ptr->controller_type == PA_HARDWARE_C7100_CPU) ||
	(cpu_eeprom_ptr->controller_type == PA_HARDWARE_C7100_JUNIOR)) {
	return(cpu_eeprom_ptr->hardware_version_high);
    }
    return(C7100_VERSION_UNKNOWN);
}

/*
 * c7100_cpucard_type:
 * Read the CPU EEPROM and return the type of CPU card
 */
int c7100_cpucard_type (void)
{
    if (!cpu_eeprom_ptr) {
	cpu_eeprom_ptr = &cpu_eeprom_copy;
	pas_access_sys_eeprom(&c7100_cpu_eeprom, cpu_eeprom_ptr, 
			      PAS_EEPROM_NMC93C46,
			      PAS_EEPROM_CMD_READ, 
			      (sizeof(dgbus_eeprom_t) >> 1));
    }

    /*
     * Return the correct CPU type
     */
    if (cpu_eeprom_ptr->controller_type == PA_HARDWARE_C7100_CPU) {
	return(C7100_CPU_NPE150);
    }
    if (cpu_eeprom_ptr->controller_type == PA_HARDWARE_C7100_JUNIOR) {
	return(C7100_CPU_NPE100);
    }
    return(C7100_CPU_UNKNOWN);
}

/*
 * test_parity_io:
 * Test IO memory parity errors
 */
void test_parity_io (void)
{
    printf("\n%%not supported");
}

/*
 * gt64010_dma_allocate_ring:
 * Allocate the ring data structure in shared memory
 */
gt64010_dma_ring_t *gt64010_dma_allocate_ring (gt64010_dma0_cntl_t *dma0)
{
    gt64010_dma_ring_t *ring;

    ring = malloc_pcimem(sizeof(gt64010_dma_ring_t) * GT64010_DMA0_ENTRIES);
    if (!ring) {
	crashdump(0);			/* no memory */
    }

    /*
     * If this is a rev 2 predator CPU card and the memory allocated above
     * is in packet SRAM, then the DMA engines can read the control
     * data structure as byte swapped.
     */
    if (c7100_has_packet_sram &&
	((ulong)ring & PCISRAM_ADDRESS_MASK)) {
	
	/*
	 * Byteswappable memory
	 */
	dma0->byte_swap_memory = TRUE;
	dma0->byte_swap_addr_mask = PCISRAM_SWAP_MASK;

    } else {

	/*
	 * No byteswappable memory.
	 */
	dma0->byte_swap_memory = FALSE;
    }
    return(ring);
}

/*
 * gt64010_errors_are_fatal:
 * Return TRUE if a GT64010 error should bring the box down
 */
boolean gt64010_errors_are_fatal (void)
{
    /*
     * If the CPU card revision is version 1 or less, then
     * we should not bring down the box (pre-production hardware
     * that has some idiosyncracies). Otherwise, down we go.
     */
    if (c7100_cpucard_version() <= C7100_VERSION_REV1) {
	return(FALSE);
    }
    return(TRUE);
}

/*
 * c7100_pak_coalesce:
 * Coalesce a scattered packet. It is assumed to be called from
 * interrupt level, so no protection is needed on the input qcount.
 */
boolean c7100_pak_coalesce (hwidbtype *idb, paktype *pak, 
			    c7100_coalesce_v func)
{
    boolean inputq_ok;
    paktype *newpak;

    /*
     * Get an input buffer
     */
    newpak = input_getbuffer(pak->datagramsize, idb);
    if (newpak && pak_coalesce(pak, newpak)) {
	newpak->flags |= PAK_INPUTQ;
	newpak->if_input = pak->if_input;
	(*func)(idb, newpak);
	inputq_ok = TRUE;
    } else {
	inputq_ok = FALSE;
    }	

    /*
     * Cleanup the pak and return
     */
    pak->flags = 0;
    pak->if_input = NULL;
    particle_retbuffer(pak);
    pak->datagramsize = 0;
    pak->datagramstart = NULL;
    return(inputq_ok);
}

/*
 * c7100_coalesce_with_dma:
 * Return TRUE if the packet is in DRAM. Else return FALSE, hence
 * using the CPU to coalesce a frame in SRAM (preserves MB0 bandwidth)
 */
static inline boolean c7100_coalesce_with_dma (uchar *ptr)
{
    return( !((ulong)ptr & PCISRAM_ADDRESS_MASK));
}

/*
 * c7100_dma_pak_coalesce :
 * Fast DMA driven coalesce of a frame
 */
boolean c7100_dma_pak_coalesce (hwidbtype *idb, paktype *pak, 
				void *func, uchar *parse_ptr)
{
    boolean use_dma, inputq_ok;
    gt64010_dma0_entry_t *dma = NULL;
    paktype *newpak;
    int count;

    /*
     * If the packet is not SRAM based, use the DMA engine. Otherwise
     * use the CPU to copy the frame (preserve MB0 bandwidth)
     */
    use_dma = c7100_coalesce_with_dma(pak->datagramstart);
    count = pak_particle_count(pak);
    if (use_dma) {
	/*
	 * Get DMA entries
	 */
	dma = gt64010_dma0_get_free_blocks(count);
	if (!dma) {
	    idb->counters.input_drops += 1;
	    pak->if_input = NULL;
	    pak->flags = 0;
	    particle_retbuffer(pak);
	    pak->datagramsize = 0;
	    pak->datagramstart = NULL;
	    return(TRUE);
	}
    }
	
    /*
     * Get an input buffer
     */
    newpak = input_getbuffer(pak->datagramsize, idb);
    if (newpak) {
	/*
	 * Setup the newpak
	 */
	inputq_ok = TRUE;
	newpak->flags = (pak->flags | PAK_INPUTQ);
	
	/*
	 * If IRB(Integrated Routing and Bridging) is enabled,
	 * the input interface *may* have been overwritten to the BVI
	 * (Bridge-group Virtual Interface) in tbridging fast-switching
	 * code where it makes route/bridge decision. Take car of
	 * input_qcount instead of just overwriting it.
	 */
	if (is_bvi(pak->if_input->hwptr)) {
	    change_if_input(newpak, pak->if_input);
	    newpak->irb_rcvd_input = pak->irb_rcvd_input;
	} else {
	    newpak->if_input = pak->if_input;
	}

	newpak->rxtype = pak->rxtype;
	newpak->datagramsize = pak->datagramsize;

	if (!use_dma) {
	    particletype *mp;
	    pas_coalesce_v newfunc = (pas_coalesce_v)func;
	    iqueue_t iqueue;
	    uchar *ptr;

	    /*
	     * Parse the packet
	     */
	    iqueue = (*idb->parse_packet)(idb, newpak, parse_ptr);

	    /*
	     * Do the copy
	     */
	    ptr = newpak->datagramstart;
	    while ((mp = particle_dequeue_inline(pak))) {
		bcopy(mp->data_start, ptr, mp->data_bytes);
		ptr += mp->data_bytes;
		retparticle_inline(mp);
	    }
	    (*newfunc)(idb, newpak, iqueue);
	} else {

	    /*
	     * Use the DMA engines. Parse the packet
	     */
	    dma->iqueue = (*idb->parse_packet)(idb, newpak, parse_ptr);
	    dma->newpak = newpak;
	    dma->idb = idb;
	    dma->func = func;
	    
	    /*
	     * Initiate the DMA
	     */
	    gt64010_pak_coalesce(pak, newpak->datagramstart, count, dma);
	}
    } else {
	particle_retbuffer(pak);
	inputq_ok = FALSE;
    }

    /*
     * Cleanup the pak and return
     */
    pak->flags = 0;
    pak->if_input = NULL;
    pak->datagramsize = 0;
    pak->datagramstart = NULL;
    return(inputq_ok);
}

/* end of file */
