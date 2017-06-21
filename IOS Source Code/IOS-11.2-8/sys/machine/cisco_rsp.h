/* $Id: cisco_rsp.h,v 3.4.20.1 1996/06/04 22:55:14 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/machine/cisco_rsp.h,v $
 *------------------------------------------------------------------
 * cisco_rsp.h - Definitions for R4600 based C7000 RSP
 *
 * March 1994, Carl Schaefer
 * December 1994, Scott Mackie (moved and altered from sys/h)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco_rsp.h,v $
 * Revision 3.4.20.1  1996/06/04  22:55:14  mbeesley
 * CSCdi59380:  Optimize buffer code for traditional platforms
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/02  04:29:20  mbeesley
 * CSCdi48003:  Buffer manager not optimal on all boxes
 * Make pak_set_dirty() and pak_ignore_inputq() platform specific.
 *
 * Revision 3.3  1995/12/21  16:29:13  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.2  1995/11/17  17:50:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:15:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/15  18:54:02  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.1  1995/06/07  21:43:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * The R4600 RSP uses ELF files
 */

#include "image_elf.h"

/*
 * Interrupt level assignments for system software
 */

#define NMI_LEVEL	7	/* NMI Timer level */
#define LEVEL_ERROR	6	/* bus errors, etc. */
#define LEVEL_CONSOLE	5	/* console UART */
#define LEVEL_PCMCIA	4
#define LEVEL_STALL	3
#define LEVEL_NETWORK_HI 2	/* high priority QA interrupts */
#define LEVEL_NETWORK	1	/* normal priority QA interrupts */

#define PCMCIA_DISABLE	4
#define STALL_DISABLE	3	/* cbus stall signal from OIR */

#define UARTDEVBASEADDR	0x3e840400

#define RSP_DCACHE_LINE_SIZE	32

/*
 * This really should not be here !!!
 */
#define RAMBASE                 0x60000000
#define PHY_RAMBASE		RAMBASE

/*
 * Define the position of the pointer to the crashinfo structure in the
 * core file.  This pointer (and data structure) are used by GDB as a
 * starting point for analyzing a crash dump.  This location does not
 * need to survive a reload, it just needs to appear in the crash dump.
 * If you change this value, you must coordinate with Cisco's GDB
 * maintainers.
 *
 * Note: The offset into DRAM of the pointer to the crashinfo structure
 * is dependent on the exception handling code in asm-mips/asm_4k.S.  Do
 * not change it without changing the asm code.
 */
#define CRASH_INFO_OFFSET	0x250

#define CRASH_INFO	 	(*((crashinfo **)(R4K_K0BASE + 		\
						  CRASH_INFO_OFFSET)))


/*
 * R4K CPU Config and Status register initialization
 * Status Register :	31:28	Coprocessor 0,1 usable
 *			26	32 FPU registers
 *			22	use bootstrap exception vector table
 *			KX,SX,UX all 32 bit
 *			ERL,EXL reset to 0
 *
 * Config Register :	30:28	divide-by-two sysclock
 *			27:24	DDDD writeback
 *			14	parity enabled (always on R4600)
 *			13	subblock enabled (always on R4600)
 *			5	icache line 32bytes (always on R4600)
 *			4	dcache line 32bytes (always on R4600)
 *			2:0	KSEG0 caching - non-coherent write-back
 */

#define R4K_SREG_SETUP          (SREG_CU1 | SREG_CU0 | SREG_FR | SREG_BEV)

/*
 * the other bits in this register are set at power-on reset by the
 * serial config bitstream
 */
#define R4K_CONFIG_SETUP	(TLB_CACHE_NONCOHERENT << \
				 CONFIG_KSEG0_COHERENCY_SHIFT)

/*
 * Main NVRAM define and prototype
 */
#ifndef ASMINCLUDE
extern void bcopy_32(void const *, void *, int);
#endif

#define NVBCOPY(s, d, l)	bcopy_32(s, d, l)

#define NVBASE			ADRSPC_NVRAM_START

#define RSP_DELAY_VALUE		SIERRA_DELAY_VALUE

/*
 * Common interrupt levels
 */
 
#define PLATFORM_ALL_DISABLE   6  /* disable all interrupts */
#define PLATFORM_TTY_DISABLE   5  /* priority to shut off all types of TTY */
#define PLATFORM_NETS_DISABLE  2  /* highest priority of a network device */
#define PLATFORM_ALL_ENABLE    0  /* enable all interrupts */

#define PLATFORM_FAMTYPE       FAMILY_RSP


/*
 * platform-specific stuff
 */

#define REFRESHTIME          4          /* 4ms per tick */                 
#define NMI_FREQ_NUMERATOR   250	/* Numerator of NMI frequency */
#define NMI_FREQ_DENOMINATOR 1		/* Denominator of NMI frequency */

#define WASTETIME(usecs) wastetime(usecs)
#define DELAY(ms)        msecdelay(ms)

#define INVALID_ADDR	0x3fefefef

#define MAX_SLOTS		13			/* tokamak */
#define MAX_CONTROLLERS		MAX_SLOTS
#define MAX_CTRLR_INTERFACES	32
#define MAX_CBUS_INTERFACES	(MAX_CONTROLLERS * MAX_CTRLR_INTERFACES)

/* 
 * Max possible lines for 7000 RSP
 */
#define PLATFORM_MAXLINES 1002

/*
 * highest numbered IP slot (zero-based)
 */
#define MAX_IF_SLOT		12


/*
 * The number of transmit packet slots to reserve for process
 * level output against fast switching output.
 */
#define RSP_RES_PROC_TX		2

/*
 * No new network buffers if coreleft is below this threshold.
 */
#define BUFFERTHRESHOLD		(btow(256 * 1024))

/*
 * network interrupt throttling
 */
#ifndef ASMINCLUDE
extern void rsp_sched_loop_end (boolean);

/*
 * support protected writes to dbus registers
 */
typedef struct {
    ulong sreg;
    uchar cbus_grant;
} dbus_protect_t;

extern void rsp_dbus_protect(dbus_protect_t*);
extern void rsp_dbus_restore(dbus_protect_t*);

/*
 * RSP caches packets in shared memory
 */
#define pak_set_dirty(pak)	platform_pak_set_dirty_inline(pak)

/*
 * RSP can ignore the input holdqueue count, so call the inline
 */
#define pak_ignore_inputq(pak)	platform_pak_ignore_inputq_inline(pak)

/*
 * RSP never deals with scattered packets
 */
#define pak_has_particles(pak)	FALSE

#endif

#define dbus_protect(state)	rsp_dbus_protect(state)
#define dbus_restore(state)	rsp_dbus_restore(state)

