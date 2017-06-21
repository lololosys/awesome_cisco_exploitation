/* $Id: diag_4k.c,v 3.3.60.2 1996/05/17 10:44:16 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dev/diag_4k.c,v $
 *------------------------------------------------------------------
 * diag_4k.c -- R4K System Diagnostic Routines
 *
 * November 1993, Michael Beesley
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: diag_4k.c,v $
 * Revision 3.3.60.2  1996/05/17  10:44:16  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.2  1996/05/02  22:01:29  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.48.1  1996/04/03  13:59:13  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.60.1  1996/03/29  20:04:46  mbeesley
 * CSCdi52977: Some r4k implementations handle parity exceptions
 *             incorrectly. Fix them.
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/17  09:00:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:18:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:54:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/09  12:59:50  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:24:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "logger.h"
#include "../os/free.h"
#include "../if/network.h"
#include "../os/nv.h"
#include "../os/gdb.h"
#include "../dev/monitor1.h"
#include "registry.h"
#include "../ip/ip_registry.h"

/*
 * Define some address literals
 */
#define ADDR_ERR_ADDRESS        (RAMBASE + 0x1001)

/*
 * test_parity_main:
 * Test main memory parity
 */
void
test_parity_main(volatile uint *addr)
{
    leveltype level;

    printf("Creating a fake parity error\n");
    flush();
    process_sleep_for(100);	/* let output out */
    
    level = raise_interrupt_level(ALL_DISABLE);
    r4k_fake_parity_error();
    reset_interrupt_level(level);
}

/*
 * test_addr_err:
 * Test address error
 * Causes exception by fetching opcode from an unaligned address
 */
void
test_addr_err(void)
{
    leveltype level;
    ulong addr = ADDR_ERR_ADDRESS;

    printf("Forcing an address error by jumping to %#x\n", addr);
    flush();
    process_sleep_for(100);	/* let output out */
    
    level = raise_interrupt_level(ALL_DISABLE);
    asm(".set noreorder");
    asm("move $3, %0; jal $3; nop; .set reorder"
	: 		/* no outputs */
	: "r" (addr));	/* input variable */

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
    asm(".set noreorder");
    asm("jal $0");
    asm("nop");
    asm(".set reorder");
    reset_interrupt_level(level);
}

/* End of file */
