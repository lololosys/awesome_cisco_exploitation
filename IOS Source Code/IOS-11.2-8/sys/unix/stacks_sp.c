/* $Id: stacks_sp.c,v 3.2.4.1 1996/03/18 22:29:50 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/unix/stacks_sp.c,v $
 *------------------------------------------------------------------
 * stacks_sp.c -- SPARC specific stack routines
 *  
 * June 1985, Kirk Lougheed
 * October 1993, Michael Beesley
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stacks_sp.c,v $
 * Revision 3.2.4.1  1996/03/18  22:29:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.10.2  1996/03/16  07:46:22  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1.10.1  1996/03/09  05:14:26  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2  1996/03/09  01:01:03  hampton
 * Migrate processor specific files to src-xxx directories.  [CSCdi51128]
 *   Step 1: Create copies of the files in their new location.
 *
 * Revision 3.1  1996/02/21  21:52:37  hampton
 * Placeholders for Port Ready.
 *
 * Revision 3.2  1995/11/17  18:51:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:48:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:58:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/sched_private.h"
#include "ttysrv.h"
#include "../../boot/reason.h"
#include "../../boot/save_stack.h"
#include "../dev/monitor1.h"
#include "exception.h"
#include "config.h"
#include "parser.h"

/*
 * stack_hardware_init
 * Called by level_init() to set up hardware specific interrupt vectors
 */
void 
stack_hardware_init (void)
{
    return;
}

/*
 * stack_setup_interrupt
 * Initialize an interrupt level stack frame
 */
void 
stack_setup_interrupt (int index, void *routine)
{
    return;
}

/*
 * stack_setup_process
 * Hardware specific process stack initialization for SPARC
 */

#define SPARC_MINFRAMEBYTES (24*4)

void 
stack_setup_process (sprocess *sp, long stackstart, void *address,
		     long arg)
{
    /*
     * We put pointer at 96 bytes (24 longs) from base to create an
     * initial, minimum size stack frame.  We crash if we don't do this.
     */
    stackstart -= SPARC_MINFRAMEBYTES;	/* compute stack base */
    sp->regs[8] = (long) arg;	/* argument is passed in %o0 */
    sp->regs[14] = stackstart;	/* stack pointer goes in in %o6 (%sp) */
    sp->regs[15] = ((long)address) - 8; /* starting/return address in %o7 */
}

/*
 * trace_caller:
 * Return a pointer to the next frame. given the address of the
 * current frame and set pc to the owner of the next frame.
 * Note : Does not work. Need details of stack frames on Sparc
 */
ulong *
trace_caller(ulong *frame, ulong **pc)
{
    return((ulong *)0);
}

