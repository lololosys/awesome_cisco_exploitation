/* $Id: diag_68.c,v 3.3.60.3 1996/06/16 21:11:14 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/dev/diag_68.c,v $
 *------------------------------------------------------------------
 * diag_68.c -- M68K System Diagnostic Routines
 *
 * 5-February-1989, Kirk Lougheed
 * November 1993, Michael Beesley
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * These routines are primarily intended for the manufacturing process.
 * They need to return very simple, yes/no results for the operator.
 * Keep this in mind when adding additional functionality.
 *------------------------------------------------------------------
 * $Log: diag_68.c,v $
 * Revision 3.3.60.3  1996/06/16  21:11:14  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3.60.2  1996/05/17  10:44:19  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.2  1996/05/02  22:01:32  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.48.1  1996/04/03  13:59:15  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.60.1  1996/03/18  19:12:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/09  05:04:18  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.26.1  1996/02/20  13:45:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  09:00:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:30  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:18:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  12:59:51  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:24:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "logger.h"
#include "../os/free.h"
#include "../if/network.h"
#include "../os/nv.h"
#include "../dev/monitor1.h"
#include "registry.h"
#include "../ip/ip_registry.h"
#include "../os/region.h"

#if defined(XX)
#include "../../boot/src-68-c4000/pcmap.h"
#endif

#if defined(PAN) || defined(BRASIL)
#include "../../boot/src-68-c3000/pcmap.h"
#endif

#if defined(SAPPHIRE)
#include "../src-36-c1000/pcmap.h"
#endif

#if defined(CSC2)
#include "../../boot/src-68-ags/pcmap.h"
#endif

#if defined(RP1)
#include "../../boot/src-68-c7000/pcmap.h"
#endif

/*
 * test_parity_main:
 * Test main memory parity
 */
void
test_parity_main(volatile uint *addr)
{
    leveltype level;
    uint data;

    printf("Creating a RAM parity error in Main memory at address %#x...\n",
	   addr);
    flush();
    process_sleep_for(100);	/* let output out */
    
    level = raise_interrupt_level(ALL_DISABLE);
    
    /*
     * This EMT call will write the given value to the specified
     * main memory address with INCORRECT parity, so that we can
     * cause a parity error by reading this memory location.
     *
     * Note that the IGS/TR, and later IGS variants, don't allow
     * the selection of even or odd parity like the original IGS
     * did. The parity bit simply turns odd parity generation on
     * or off. So, for this reason, we select a value that has an
     * even number of ON bits IN EACH BYTE, so that the parity
     * bit for each byte should have had a one for odd parity
     * while it is written as a zero (with parity disabled):
     */
    if (mon_write_bad_main_memory_parity(0xca5ff5ac,
					 addr) != -1) {

	uint old_cacr = 0;
	
	if ((cpu_type == CPU_CSC4) || (cpu_type == CPU_RP1)) {
	    
	    /*
	     * This is a 68040 processor
	     * Set CACR to disable instruction and data caches
	     * while we are in this subroutine, then flush any
	     * dirty lines from either cache:
	     */
	    old_cacr = mon_setcacr(0);
	    asm(".word 0xf4f8"); /* 68040 CPUSH instruction to */
	    /* flush and invalidate caches */
	    
	} else {
#if !(defined(SAPPHIRE) || defined(RP1))
	    /*
	     * This is a 68020 or 68030 processor which has a
	     * Cache Control Register with "Clear Cache" bits.
	     * The 68360 (QUICC) does not have any caches, and the
	     * 68040 does not have CLEAR bits defined in its CACR reg.
	     * Hence neither of these platforms has a CLEAR_CACHES
	     * definition.
	     */
	    old_cacr = mon_setcacr(CLEAR_CACHES);
#endif
	}
	
	/*
	 * Read from the memory location that has invalid parity
	 * data to cause a parity error:
	 */
	data = *addr;
	
	(void)mon_setcacr(old_cacr);
	
    } else {
	printf("Can not generate a parity error in main memory because\nEMT_WRITE_BAD_MAIN_MEMORY_PARITY not supported by this ROM monitor version\n");
    }
    
    reset_interrupt_level(level);
}

/*
 * test_parity_io:
 * Test IO memory parity errors
 */
void
test_parity_io(void)
{
#ifdef 	XX

    volatile uint *address;
    regiontype    *region;
    uint           data;
    leveltype      level;

    region = region_find_by_class(REGION_CLASS_IOMEM);
    if (!region) {
	printf("Test not supported by this hardware\n");
	return;
    }

    address = (volatile uint *)(region->start);
    printf("Creating a RAM parity error in IO memory at address %#x...\n",
	   address);
    flush();
    process_sleep_for(100);	/* let output out */
    
    /*
     * Set IO memory to generate even parity before write:
     */
    level = set_interrupt_level(LEVEL5); /* Allow level 6 IO parity err int */
    *address = 0x0;	/* Write with odd parity */
    *((volatile uint *)ADRSPC_SHMEM_CONTROL) &= ~SHMEM_PARITY_ODD;
    *address = 0xca5f;;	/* Write with even parity */
    
    /*
     * Set IO memory to generate and check odd parity before read:
     */
    *((volatile uint *)ADRSPC_SHMEM_CONTROL) |= SHMEM_PARITY_ODD;
    data = *address;	/* Read with odd parity */
    reset_interrupt_level(level);

#else 	/* XX */

    printf("Test not supported by this hardware\n");

#endif 	/* XX */
}

/*
 * test_addr_err:
 * Test address error
 */
void
test_addr_err(void)
{
    leveltype level;

    printf("Forcing an address error by jumping to 0x1201\n");
    flush();
    process_sleep_for(100);	/* let output out */
    
    level = raise_interrupt_level(ALL_DISABLE);
    asm("	jmp 0x1201");
    reset_interrupt_level(level);
}

/*
 * test_jump_zero:
 * Test jumping to zero
 */
void
test_jump_zero(void)
{
    leveltype level;

    printf("Jumping to address 0x0...\n");
    flush();
    process_sleep_for(100);	/* let output out */
    
    level = raise_interrupt_level(ALL_DISABLE);
    asm("	jmp 0");
    reset_interrupt_level(level);
}

/* End of file */
