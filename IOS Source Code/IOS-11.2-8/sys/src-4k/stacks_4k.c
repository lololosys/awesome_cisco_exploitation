/* $Id: stacks_4k.c,v 3.2.4.1 1996/03/18 21:57:57 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-4k/stacks_4k.c,v $
 *------------------------------------------------------------------
 * stacks_4k.c -- R4K specific stack routines
 *  
 * June 1985, Kirk Lougheed
 * October 1993, Michael Beesley
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stacks_4k.c,v $
 * Revision 3.2.4.1  1996/03/18  21:57:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.10.2  1996/03/16  07:32:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1.10.1  1996/03/09  05:11:43  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2  1996/03/09  01:00:55  hampton
 * Migrate processor specific files to src-xxx directories.  [CSCdi51128]
 *   Step 1: Create copies of the files in their new location.
 *
 * Revision 3.1  1996/02/21  21:52:20  hampton
 * Placeholders for Port Ready.
 *
 * Revision 3.3  1996/01/24  17:52:15  wmay
 * CSCdi47324:  ISDN hoses bugtrace call - allow REGION_CLASS_IMAGEBS to
 * be displayed in bugtrace
 *
 * Revision 3.2  1995/11/17  18:51:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:48:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:19:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/07/14  09:35:41  schaefer
 * CSCdi36176: misalign/spurious access traceback broken for leaf functions
 *
 * Revision 2.3  1995/06/12  18:59:49  schaefer
 * CSCdi35699: spurious access in trace_caller
 *
 * Revision 2.2  1995/06/09  13:12:17  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:58:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/sched_private.h"
#include "stacks.h"
#include "ttysrv.h"
#include "../../boot/reason.h"
#include "../../boot/save_stack.h"
#include "../dev/monitor1.h"
#include "exception.h"
#include "config.h"
#include "parser.h"
#include "../src-4k/signal_4k.h"
#include "../src-4k/alignment.h"
#include "../os/region.h"

/*
 * stack_setup_interrupt
 * Initialize an interrupt level stack frame
 */
void 
stack_setup_interrupt(int index, void *routine)
{
    uchar *address;
    int size;

#ifdef DEBUG
    ttyprintf(CONTTY, "stack_setup_interrupt(index=%d, routine=%#x)\n",
	      index, routine);
#endif

    /*
     * Compute start of stack frame (it grows from high low address).
     * We need to ensure that the stack starts on an 8byte boundary.
     * Remember handler subroutine address for interrupt level handler.
     */
    address = *intstacks[index].base;
    size = intstacks[index].size;
    *intstacks[index].stack = 
	(uchar *)(((ulong)address + (size - 4)) & ~7);
    *intstacks[index].routine = routine;
}

/*
 * r4k_process_dispatch:
 * We go through this routine to dispatch a process for the first time
 * in order to detect an unnatural return from the process's main
 * routine, thus giving us a chance to call "process_hari_kari"
 */
static void
r4k_process_dispatch(void *addr, long arg)
{
    forktype (*padd)(void *) = (forktype (*)(void *))addr;

    /*
     * Dispatch the process
     */
    (*padd)((void *)arg);

    /*
     * It should have never returned, so call "process_hari_kari".
     * "process_hari_kari" calls process_kill to commit suicide, which
     * will never return.
     */
    process_hari_kari();
}

/*
 * stack_setup_process
 * Hardware specific process stack initialization for MIPS on SGI w/s.
 * This is very dependant on the suspend/resume routines.
 */

#define R4K_MINFRAMEBYTES 32

void 
stack_setup_process (sprocess *sp, long stackstart, void *address,
		     long arg)
{
    /*
     * We put pointer at 32 bytes (8 longs) from base to create an
     * initial, minimum size stack frame.  We crash if we don't do this.
     */
    stackstart -= R4K_MINFRAMEBYTES;	   /* compute stack base   	  */
    sp->savedsp = (ulong *)stackstart;	   /* save process stack pointer  */

    sp->regs[0] = (long) address;          /* pass function address in a0 */
    sp->regs[1] = (long) arg;	           /* argument is passed in a1 	  */
    sp->regs[11] = stackstart;	           /* stack pointer goes in sp 	  */

    /*
     * Initialize ps so that resume does not accidentally send us
     * into the SIGUSR1 exception handler and the r4k_process_dispatch 
     * routine as the entry point. He will then call the real entry 
     * point of the process
     */
    sp->ps = 0x00;
    sp->regs[13] = (long) r4k_process_dispatch;
}

/*
 * trace_caller:
 * Return a pointer to the next frame, given the address of the current frame
 * and the PC of its owner. Update the PC variable to reflect the owner of
 * the next frame in line, thus allowing multiple calls to work.
 */
ulong *
trace_caller (ulong *frame, ulong **pc)
{
    union instr_r4k *op;
    int size, ra_off;
    ulong *newframe;
    regiontype *region;
    region_class class;

    /*
     * Disassemble the code at RA to find the size of the present
     * stack frame and to get the RA from that frame. Note we will allways
     * be looking at a non-leaf function due to the fact that we were
     * called.
     */
    op = (union instr_r4k *)*pc;
    size = 0;
    ra_off = -1;

    for (;;) {
	if (!is_valid_pc(op))
	    return NULL;

	if (op->op_code == OPCODE_JR_RA)
	    break;

	/*
	 * Look for reading/writing the RA register 
	 * to/from the stack frame
	 */
	if ((op->i_t.op == OPCODE_LW || op->i_t.op == OPCODE_SW) &&
	    (op->i_t.rt == R4K_RA_REG) && 
	    (op->i_t.rs == R4K_SP_REG)) {
	    ra_off = op->i_t.s_imd;
	}

	/*
	 * Look for destruction of the stack frame
	 */
	if ((op->i_t.op == OPCODE_ADDIU) &&		/* addiu sp,sp,x */
	    (op->i_t.rs == R4K_SP_REG) &&
	    (op->i_t.rt == R4K_SP_REG)) {
	    size = op->i_t.s_imd;
	} 
	op = (union instr_r4k *)((long)op + 4);
    }

    /*
     * Check for frame destruction in bd slot
     */
    op = (union instr_r4k *)((long)op + 4);
    if (!is_valid_pc(op))
	return NULL;

    if ((op->i_t.op == OPCODE_ADDIU) &&  	/* addiu sp,sp,x */
	(op->i_t.rs == R4K_SP_REG) &&		/* in bd slot    */
	(op->i_t.rt == R4K_SP_REG)) {
	size = op->i_t.s_imd;
    }
    
    /*
     * Make sure we got a good size, ra_off
     */
    if (size < 0)
	size = -size;
    if ((size==0) || (ra_off==-1) || (size & 0x3) || (ra_off & 0x3)) {
	return(0);
    }

    /*
     * Update the pc and check for sanity. When we have traced
     * back as far as we can go, the pc will get assigned -1 due
     * to all ones initialization of the stacks.
     */
    newframe = (ulong *)((ulong)frame + size);

    frame += ra_off / sizeof(*frame);
    if ((ulong) frame & 0x03)
	return NULL;
    /*
     * frames should always be in the heap, which is class "local"
     * or in a KSEG0/1 view of main memory (also class "local") or
     * in fast memory, which is class "fast"
     * ISDN will also have stacks for monitor & timer tasks in the imagebss
     * region - it allocates a chunk in bss.
     */
    region = region_find_by_addr(frame);
    if (region == NULL)
	return NULL;
    class = region_get_class(region);
    if (class != REGION_CLASS_LOCAL && class != REGION_CLASS_FAST &&
	class != REGION_CLASS_IMAGEBSS)
	return NULL;

    if (!is_valid_pc((void*) *frame))
	return(NULL);
    *pc = (ulong *) *frame;

    /*
     * return the next frame pointer
     */
    return(newframe);
}
