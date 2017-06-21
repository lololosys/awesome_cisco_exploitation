/* $Id: cisco_svip.h,v 3.1.62.5 1996/08/07 17:41:48 drowell Exp $
 * $Source: /release/112/cvs/Xsys/machine/cisco_svip.h,v $
 *------------------------------------------------------------------
 * cisco_svip.h - Definitions for VIPER
 *
 * September 1995, David Getchell
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco_svip.h,v $
 * Revision 3.1.62.5  1996/08/07  17:41:48  drowell
 * CSCdi65226:  IPFast vip send stats during init require preallocated
 * buffer
 * Branch: California_branch
 * Changed the size of the pre-allocated buffer from very-big to huge.
 *
 * Revision 3.1.62.4  1996/06/04  22:55:17  mbeesley
 * CSCdi59380:  Optimize buffer code for traditional platforms
 * Branch: California_branch
 *
 * Revision 3.1.62.3  1996/06/03  18:04:33  getchell
 * CSCdi52344:  scheduler allocate needed for debug under load
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/05/23  03:58:33  kao
 * CSCdi58288:  inconsistent ip cache in VIP after mic rel with traffic
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/03/21  22:57:19  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.1  1996/02/10  00:06:19  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/28  04:55:14  getchell
 * Placeholder for VIPER development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
/*
 * The SVIP uses ELF images
 */
#include "image_elf.h"

/*
 * Interrupt level assignments for system software
 *
 *	1 - PA packet, DMA 			(direct)
 *	2 - Port Adaptor Management Interrupt	(direct)
 *	3 - Errors, GDB		                (nevada,direct)
 *	4 - Timer (msclock)			(direct)
 *	5 - Dbus Interrupt			(nevada)
 *	6 - Cbus MALU Attention Interrupt	(nevada)
 *      7 - Cbus Stall                          (nevada)
 */

/*
 * Interrupt levels
 */
#define LEVEL_DBUS		7 /* Dbus level */
#define LEVEL_CBUS_STALL        6
#define LEVEL_CBUS	 	5 /* Malu Attention level */
#define NMI_LEVEL		4 /* Timer level (msclock) */
#define LEVEL_ERROR		3 /* Errors, GDB */
#define LEVEL_DMA		1 /* DMA Engine */
#define LEVEL_CONSOLE		2 /* Console - no hw */
#define LEVEL_PA_MANAGEMENT	2 /* Port Adaptor Hi IRQ */
#define LEVEL_PA_NETWORK	1 /* Network IO level */

#define WASTETIME(usecs) 	wastetime(usecs)
#define DELAY(ms)        	msecdelay(ms)

/*
 * Define hw board
 */
#define VIPER		        TRUE

/* 
 * Max possible lines for SVIP
 */
#define PLATFORM_MAXLINES	6

/*
 * Highest numbered PA slot (zero-based)
 */
#define MAX_IF_SLOT		1

/*
 * Base of Memory
 * 
 * Important:  These values are dependent on virtual and physical address
 * defines in src-svip/svip_pcmap.h.  They must be kept in sync.
 * FIXME:  Consider taking these defines from that file.
 */
#define RAMBASE       		0x60000000
#define PHY_RAMBASE		0x00000000

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


#define REFRESHTIME          	4	/* 4ms per tick */                 
#define NMI_FREQ_NUMERATOR 	250	/* Numerator of NMI frequency */
#define NMI_FREQ_DENOMINATOR 	1   	/* Denominator of NMI frequency */

/*
 * R4K CPU Config and Status register initialization
 * Status Register : 	Coprocessor 0,1 usable
 *			32 FPU registers
 *			Bootstrap exceptions
 *			KX,SX,UX all 32 bit
 *			ERL,EXL reset to 0
 *
 * Config Register :	K0 set to cached non-coherent
 */
#define R4K_SREG_SETUP          0x34400000
#define R4K_CONFIG_SETUP        0x00000033

/*
 * Write buffer flush routines
 */
#ifndef ASMINCLUDE

extern int flush_io_wbuffer(void);
extern int flush_shmem_wbuffer(void);
extern void flush_all_wbuffers(void);

/*
 * Support protected writes to the dbus registers.
 * These are stubs as the SVIP can never take a bus error
 * from a read to the dbus (bus interface is on the same card)
 */
typedef uchar dbus_protect_t;

#define dbus_protect(statep)	(*(statep) = 0)
#define dbus_restore(statep)

/*
 * SVIP does not cache packets in shared memory
 */
#define pak_set_dirty(x)
 
/*
 * SVIP never ignores the input holdqueue count
 */
#define pak_ignore_inputq(pak)  FALSE

/*
 * SVIP deals with scattered packets
 */
#define pak_has_particles(pak)	platform_pak_has_particles_inline(pak)
 
/*
 * Scheduling loop callback
 */
extern void svip_sched_loop_end(boolean);

#endif /* ASMINCLUDE */

/*
 * Common interrupt levels
 *
 * Note : ALL_DISABLE will mask the DMA engine interrupt. We may
 * want to reconsider this as it will stall the handling of these 
 * interrupts.
 */
#define PLATFORM_ALL_DISABLE   2  /* disable all interrupts */
#define PLATFORM_TTY_DISABLE   2  /* priority to shut off all types of TTY */
#define PLATFORM_NETS_DISABLE  2  /* highest priority of a network device */
#define PLATFORM_ALL_ENABLE    0  /* enable all interrupts */

#define	PLATFORM_FAMTYPE FAMILY_VIP

/*
 * Nonstandard buffer pool sizes
 */
#define VIP2_HUGE_INIT1    1

/* end of file */
