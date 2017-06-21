/* $Id: rvip.c,v 3.8.6.5 1996/08/28 13:14:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rvip/rvip.c,v $
 *------------------------------------------------------------------
 * rvip.c - RVIP specific routines
 *
 * December 94, David Getchell
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: rvip.c,v $
 * Revision 3.8.6.5  1996/08/28  13:14:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.6.4  1996/06/03  18:04:37  getchell
 * CSCdi52344:  scheduler allocate needed for debug under load
 * Branch: California_branch
 *
 * Revision 3.8.6.3  1996/04/08  13:44:56  getchell
 * CSCdi52361:  DFS broken using unlike media
 * Branch: California_branch
 *
 * Revision 3.8.6.2  1996/03/21  23:50:37  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.8.6.1  1996/03/18  22:03:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.7  1996/03/18  23:35:49  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.6  1996/02/16  02:24:37  mbeesley
 * CSCdi48910:  Add storage of fault history for crashes
 * Branch: ELC_branch
 *
 * Revision 3.2.2.5  1996/02/10  00:18:56  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.4  1996/02/08  09:30:53  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.3  1996/01/25  12:10:39  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.2.2.2  1995/12/23  04:42:04  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.2.2.1  1995/11/30  02:10:05  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.8  1996/03/06  21:24:44  dlobete
 * CSCdi49840:  Support for 32 MB DRAM required for DFS on RVIP
 * Map the DRAM according to the size we dynamically sense on board.
 * - For 8 MB RVIPs we'll have 6 MB DRAM and 3 MB of PCI space.
 * - For 32 MB RVIPs we'll have 28 MB DRAM and 4 MB of PCI space.
 *
 * Revision 3.7  1996/03/06  10:20:41  feisal
 * CSCdi47327:  Need means of getting RVIP stack trace
 *
 * Revision 3.6  1996/03/06  01:42:06  jwelder
 * function read_asic_control_reg() returns Nevada asic 0
 *         control reg B
 *
 * CSCdi50800:  Show Diagbus changed to show VIP1 Dram Size
 *
 * Revision 3.5  1996/03/04  07:14:24  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.4  1996/02/18  19:42:30  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.3  1996/01/12  21:53:06  jkuzma
 * CSCdi46358:  VIP1 PA mngmt and DMA interrupt stacks swapped
 *              General debug of PA management interrupts
 *
 * Revision 3.2  1995/11/17  18:50:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */

#include "master.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "packet.h"
#include "stacks.h"
#include "logger.h"

#include "../hes/dgbus.h"
#include "../ts/nonvol_x2444.h"
#include "../os/gdb.h"

#include "../pas/if_pas.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../src-vip/vip_dbus.h"
#include "../src-vip/vip_memd.h"

#include "../src-4k/vspace_4k.h"

extern int pick_tlb_page(long, long, long *, long *, long *);

#define EEPROM_DATAIN_REG_MASK0		CSC_FLX_PCI0_D0
#define EEPROM_DATAOUT_REG_MASK0	CSA_FLX_PCI0_DI

#define EEPROM_DATAIN_REG_MASK1		CSC_FLX_PCI1_D0
#define EEPROM_DATAOUT_REG_MASK1	CSA_FLX_PCI1_DI

#define EEPROM_DATAIN_REG		(IO_ASIC->csc)
#define EEPROM_DATAOUT_REG		(IO_ASIC->csa)

/*
 * Port Adaptor ID eeprom management data structures
 */
pas_management_t pa_bay0_management = {
    0,						/* Bay 0 */

    sizeof(IO_ASIC->cfga),			/* 16 bit register */
    CFGA_EEPROM_CLOCK_PA0,			/* PA 0 clock mask */
    &IO_ASIC->cfga,				/* Clock control register */

    sizeof(IO_ASIC->cfga),			/* 16 bit register */
    CFGA_EEPROM_SELECT_PA0,			/* PA 0 select mask */
    &IO_ASIC->cfga,				/* Select control register */

    sizeof(EEPROM_DATAIN_REG),			/* 8 bit register */
    EEPROM_DATAIN_REG_MASK0,			/* DATAIN mask */
    &EEPROM_DATAIN_REG,				/* DATAIN register */

    sizeof(EEPROM_DATAOUT_REG),			/* 8 bit register */
    EEPROM_DATAOUT_REG_MASK0,			/* DATAOUT mask */
    &EEPROM_DATAOUT_REG				/* DATAOUT register */
};

pas_management_t pa_bay1_management = {
    1,						/* Bay 1 */

    sizeof(IO_ASIC->cfga),			/* 16 bit register */
    CFGA_EEPROM_CLOCK_PA1,			/* PA 1 clock mask */
    &IO_ASIC->cfga,				/* Clock control register */

    sizeof(IO_ASIC->cfga),			/* 16 bit register */
    CFGA_EEPROM_SELECT_PA1,			/* PA 1 select mask */
    &IO_ASIC->cfga,				/* Select control register */

    sizeof(EEPROM_DATAIN_REG),			/* 8 bit register */
    EEPROM_DATAIN_REG_MASK1,			/* DATAIN mask */
    &EEPROM_DATAIN_REG,				/* DATAIN register */

    sizeof(EEPROM_DATAOUT_REG),			/* 8 bit register */
    EEPROM_DATAOUT_REG_MASK1,			/* DATAOUT mask */
    &EEPROM_DATAOUT_REG				/* DATAOUT register */
};

pas_management_t *pas_management_list[MAX_PA_BAYS] = {
    &pa_bay0_management,
    &pa_bay1_management
};

/*
 * flush_io_wbuffer:
 * Flush the IO write buffer
 */
int flush_io_wbuffer (void)
{
    /* Read anything off Orion */
    return(IO_ASIC->asic_reg);
}

/*
 * turn_on_pa_led :
 * Turn on the PA's RUN lamp to indicate that we're up and running
 */
void turn_on_pa_led (int pa_bay)
{
    if (pa_bay == PA0) {
	IO_ASIC->cfgb &= (~CFGB_ENBL_LED_PA0);
    } else if (pa_bay == PA1) {
	IO_ASIC->cfgb &= (~CFGB_ENBL_LED_PA1);
    } else {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
    }
}

/*
 * turn_off_pa_led :
 * Turn off the PA's RUN lamp to indicate that we're down and dieing
 */
void turn_off_pa_led (int pa_bay)
{
    if (pa_bay == PA0) {
	IO_ASIC->cfgb |= (CFGB_ENBL_LED_PA0);
    } else if (pa_bay == PA1) {
	IO_ASIC->cfgb |= (CFGB_ENBL_LED_PA1);
    } else {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
    }
}

/*
 * pas_pa_present :
 * See if a Port Adaptor is present in the selected bay
 */
boolean pas_pa_present (uint pa_bay)
{
    if (pa_bay == PA0) {
	return(!(IO_ASIC->stata & STATA_PRSNT_PA0));
    } else if (pa_bay == PA1) {
	return(!(IO_ASIC->stata & STATA_PRSNT_PA1));
    } else {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	return FALSE;
    }
}

/*
 * pas_control_power:
 * Control power on a PA. Only applies to checkers hw rev 2.
 */
void pas_control_power (uint pa_bay, boolean onoff)
{
    if (onoff) {
	/*
	 * Turn on power
	 */
	if (pa_bay == PA0) {
	    IO_ASIC->cfgb |= CFGB_POWERON_PA0;
	} else if (pa_bay == PA1) {
	    IO_ASIC->cfgb |= CFGB_POWERON_PA1;
	} else {
	    errmsg(&msgsym(BADPABAY, PA), pa_bay);
	}
    } else {
	/*
	 * Turn off power
	 */
	if (pa_bay == PA0) {
	    IO_ASIC->cfgb &= ~CFGB_POWERON_PA0;
	} else if (pa_bay == PA1) {
	    IO_ASIC->cfgb &= ~CFGB_POWERON_PA1;
	} else {
	    errmsg(&msgsym(BADPABAY, PA), pa_bay);
	}
    }
}

/*
 * pas_control_reset:
 * Control reset signal to PA's
 */
void pas_control_reset (uint pa_bay, boolean onoff)
{
    if (onoff) {

	/*
	 * Assert reset
	 */
	if (pa_bay == PA0) {
	    IO_ASIC->cfgb &= ~CFGB_RESET_PA0;
	} else if (pa_bay == PA1) {
	    IO_ASIC->cfgb &= ~CFGB_RESET_PA1;
	} else {
	    errmsg(&msgsym(BADPABAY, PA), pa_bay);
	}

    } else {

	/*
	 * De-assert reset
	 */
	if (pa_bay == PA0) {
	    IO_ASIC->cfgb |= CFGB_RESET_PA0;
	} else if (pa_bay == PA1) {
	    IO_ASIC->cfgb |= CFGB_RESET_PA1;
	} else {
	    errmsg(&msgsym(BADPABAY, PA), pa_bay);
	}
    }
}

/*
 * pas_deactivate_bay :
 * Put the specified port adaptor in reset
 */
void pas_deactivate_bay (uint pa_bay)
{
    if (pa_bay == PA0) {
	IO_ASIC->cfgb &= ~CFGB_RESET_PA0;  /* Reset PA0 - active low */
    } else if (pa_bay == PA1) {
	IO_ASIC->cfgb &= ~CFGB_RESET_PA1;  /* Reset PA1 - active low */
    } else {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
    }
    pas_control_reset(pa_bay, TRUE);
    pas_control_power(pa_bay, FALSE);
}

/*
 * rvip_activate_flex :
 * take the flex parts out of reset
 */
static void rvip_activate_flex (void)
{
    IO_ASIC->cfgb |= CFGB_RESET_FLEX;  /* Clear Reset Flex - active low */
}

/*
 * pas_nmi_dispatcher :
 * Call all the interrupt handlers associated with all Ports requesting
 * a Nevada PA NMI interrupt (in a sequence based on their priorities).
 */
void pas_nmi_dispatcher (void)
{
    pa_nmi_handler_t *hdlr_ptr;
    ulong pmac_status;
    ushort mir = 0;

    pmac_status = *(volatile ulong *) (&PCI_ASIC->pmac_intrpt_status_reg);

    if (pmac_status & PMAC_PA0_NMI_MASK) {
        mir += 1;
    }
    if (pmac_status & PMAC_PA1_NMI_MASK) {
        mir += 2;
    }

    /* Dispatch all slot interrupt handlers ONCE */
    while (mir) {
        hdlr_ptr = pa_nmi_dispatch_table[mir];
        (*hdlr_ptr->func)(hdlr_ptr->arg);
        mir &= hdlr_ptr->mask;
    }
}

/*
 * rvip_map_iomemory:
 * Map DRAM packet memory into virtual memory map.
 */
boolean rvip_map_iomemory (int memsize, int iomemsize)
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
    addr = PHY_RAMBASE + memsize;
    caddr = PHY_RAMBASE + PCIMEM_CWT_OFS + memsize;
 
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
	 * Setup the cached view.
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
 * vip_stop_microcode :
 * Halt the microcode
 */
void vip_stop_microcode (void)
{
    dbus_hardware_regs *r;
    void (*fnc)(void);

    r = (dbus_hardware_regs *)(ADRSPC_DBUS);
    r->reset_reg = COLD_RESET;
    fnc = (void (*)(void))(PHY_TO_KSEG1(PHY_ADRSPC_PROM));
    for (;;) {
	(*fnc)();
    }
}

/*
 * test_parity_io:
 * Test IO memory parity errors
 */
void test_parity_io (void)
{
    printf("\n%%not supported");
    return;
}

/*
 * sched_alloc_command:
 * Stub for VIP sched_alloc_command
 */
void sched_alloc_command (parseinfo *csb)
{
}

/*
 * vip_netint_throttle_show:
 * Stub for display info about network IO interrupt throttling
 */
void vip_netint_throttle_show (void)
{
}

/*
 * install_cbus_handler :
 * Install the CBus interrupt handler and initialize command Queue
 */
void install_cbus_handler (void)
{
    /*
     * Create the interrupt stack
     */
    createlevel(LEVEL_CBUS, NULL, "CBUS Interrupt");

    /*
     * Enable the CBUS interface in the flex parts. This signal is 
     * active low.
     */
    rvip_activate_flex();
    
    /*
     * Initialize the vip generic memd support. Not a Viper, but Checkers.
     */
    vip_init_memd_support(FALSE);
}

/* end of file */
