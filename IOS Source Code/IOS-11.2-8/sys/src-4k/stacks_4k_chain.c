/* $Id: stacks_4k_chain.c,v 3.1.20.1 1996/03/18 21:57:59 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-4k/stacks_4k_chain.c,v $
 *------------------------------------------------------------------
 * stacks_4k_chain.c -- R4K specific stack parse routines
 *  
 * June 1985, Kirk Lougheed
 * October 1993, Michael Beesley
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stacks_4k_chain.c,v $
 * Revision 3.1.20.1  1996/03/18  21:57:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.10.1  1996/03/09  05:11:44  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:21  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/sched_private.h"
#include "stacks.h"
#include "ttysrv.h"

/*
 * stack_printtrace
 * Hardware specific printing of a stack trace called by stack_backtrace().
 * Notes: Processes allways start at r4k_process_dispatch. The stack starts
 *        at p->savedsp and grows down. The context structure has the present
 *        SP and RA fields filled in.
 */
void 
stack_printtrace (sprocess *p)
{
    ulong *fp;
    ulong *ra;
    int size, ra_off;
    union instr_r4k *op;

    automore_enable(NULL);

    /*
     * We start at the bottom of the stack and work up until
     * our return address is r4k_process_dispatch or our
     * frame pointer no longer lies within the stack
     */
    fp = p->savedsp;			/* get initial sp  */
    ra = (ulong *)p->regs[13];		/* get initial ra  */
    
    while ((ra > (ulong *)TEXT_START) &&
	   (ra < (ulong *)TEXT_END) &&
	   (fp >= p->stack) &&
	   (fp < ((ulong *)((ulong)p->stack + p->stacksize)))) {

	/* 
	 * Print out this frame
	 */
	printf("\n  FP: %#x, RA: %#x", fp, ra);

	/*
	 * Disassemble the code at RA to find the size of the present
	 * stack frame and to get the RA from that frame. Note, the 
	 * first function will *always* have a frame as it was the
	 * caller of the suspend function. And therefore they all
	 * will have frames and we do not have to deal with leaf routines.
	 */
	op = (union instr_r4k *)ra;
	size = 0;
	ra_off = -1;

	while (op->op_code != OPCODE_JR_RA &&
	       (ulong)op > (ulong)TEXT_START && 
	       (ulong)op < (ulong)TEXT_END) {

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

	    /*
	     * Increment the pc
	     */
	    op = (union instr_r4k *)((long)op + 4);
	}

	/*
	 * check for destruction of frame in bd slot
	 */
	op = (union instr_r4k *)((long)op + 4);
	if ((op->i_t.op == OPCODE_ADDIU) &&  	/* addiu sp,sp,x */
	    (op->i_t.rs == R4K_SP_REG) &&		/* in bd slot    */
	    (op->i_t.rt == R4K_SP_REG)) {
	    size = op->i_t.s_imd;
	}

	/*
	 * check for legal frame size and ra offset
	 */
	if (size < 0)
	    size = -size;
	if ((size==0) || (ra_off==-1)) {
	    break;
	}
	
	/*
	 * Make sure size and ra_off are multiples of 4 and
	 * adjust for the next frame
	 */
	size &= ~3;
	ra_off &= ~3;
	ra = (ulong *)( *(ulong *)((ulong)fp + ra_off) );
	fp = (ulong *)((ulong)fp + size);
    }

    automore_disable();
}
