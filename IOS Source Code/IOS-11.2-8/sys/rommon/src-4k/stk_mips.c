/* $Id: stk_mips.c,v 3.2.58.1 1996/03/21 23:30:13 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k/stk_mips.c,v $
 *------------------------------------------------------------------
 * stk_mips.c
 *
 * May 1993, Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Stack tracing routines for R4000/Orion.
 *------------------------------------------------------------------
 * $Log: stk_mips.c,v $
 * Revision 3.2.58.1  1996/03/21  23:30:13  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:47  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:45:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "cpu_mips.h"
#include "mon_defs.h"
#include "queryflags.h"
#include "signal_4k.h"
#include "dis_mips.h"

extern int optind;
extern r4k_context_t *kernel_context_ptr, *process_context_ptr;

/*
 * stack trace algorithm
 * 
 *     - Search for jr ra instruction IE end of function
 *       from the present PC up. Note if the lw ra,***(sp) 
 *       instruction is present. Also note if the next instruction
 *       in the jr branch delay slot is addiu sp,sp,***.
 *       If so, there is a stack frame.
 *
 *     - If lw ra,***(sp) was encountered, this function is a 
 *       non-leaf function and it allocated a stack. Else if
 *       the addiu sp,sp,*** was encountered it also has a stack 
 *       frame, but it is a leaf function. Else it is a leaf function
 *	 without a stack frame.
 *
 *     - Calculate the PC of the caller and the address of its
 *       stack frame.
 *     
 *     - Printout the information to the user.
 *
 *     - Repeat until the no. of frames has been printed out
 *       or until there is an error or no frames left
 */

/*
 * MIPS register numbers
 */
#define A3_REG      7
#define RA_REG     31
#define SP_REG     29

/*
 * Status codes for stack analysis
 */
#define FRAME_LEAF  1
#define FRAME_LAST  2
#define FRAME_NEXT  3

/*
 * illegal_addr : Check an address. It is illegal if the TLB map fails
 * or a 32bit access fails or if it is not long word aligned.
 */
int
illegal_addr(addr)
     unsigned long *addr;
{
    unsigned long eaddr, data;
    SIG_PF savfcn;
    int tmp;

    savfcn = signal(SIGBUS,SIG_IGN);
    tmp = ((unsigned long)addr & 0x03) ||	/* not long aligned ?	*/
	(tlb_rd(addr, &eaddr, &data)) ||	/* not in the TLB ?	*/
	(chkberr(addr, BW_32BITS, 1));		/* causes a bus error ?	*/
    signal(SIGBUS, savfcn);
    return(tmp);
}

/*
 * analyze_strace : Analyze a stack frame
 */
int
analyze_strace(pc, sp, pc_s, caller, size)
     unsigned long *pc, *sp, **pc_s, **caller;
     int *size;
{
    union instr_r4k *op = (union instr_r4k *)pc;
    int ssize = 0, ra_off = 0;
    
    /*
     * Scan from PC up to find end of function. Note if we find
     * any instructions that create or destory a stack frame. Also
     * note any instructions that save or restore ra.
     */
    while ( op->op_code != OPCODE_JR_RA ) {

	/*
	 * Looking for reading/writing the RA register
	 * to/from the stack frame
	 */
	if ((op->i_t.op == OPCODE_LW || op->i_t.op == OPCODE_SW) &&
	    (op->i_t.rt == RA_REG) && 
	    (op->i_t.rs == SP_REG)) {
	    ra_off = op->i_t.s_imd;
	}

	/*
	 * Look for destruction of the stack frame
	 */
	if ((op->i_t.op == OPCODE_ADDIU) &&		/* addiu sp,sp,x */
	    (op->i_t.rs == SP_REG) &&
	    (op->i_t.rt == SP_REG)) {
	    ssize = op->i_t.s_imd;
	} 
	op = (union instr_r4k *)((long)op + 4);
	/*
	** In rare cases this has been known to run off the end of memory.
	** This is time consuming but necessary for robustness.
	** Stop looking after 256k iterations.
	*/
	if(chkberr(op, BW_32BITS, 1) || (((long)op - (long)pc) > 0x40000)) {
	    *pc_s = pc;
	    *size = 0;
	    return(FRAME_LAST);
	}
    }

    /*
     * Check for frame destruction in bd slot
     */
    op = (union instr_r4k *)((long)op + 4);
    if ((op->i_t.op == OPCODE_ADDIU) &&  	/* addiu sp,sp,x */
	(op->i_t.rs == SP_REG) &&		/* in bd slot    */
	(op->i_t.rt == SP_REG)) {
	ssize = op->i_t.s_imd;
    }

    /*
     * Make sure we got a good size
     */
    if (ssize < 0)
	ssize = -ssize;

    /*
     * If we have no frame we're a leaf and our caller 
     * should have the next PC.
     */
    if ( ssize == 0 ) {
	*pc_s = pc;
	*size = 0;
	return(FRAME_LEAF);
    }

    /*
     * We definitely have a frame. We may have picked up ra_off above
     * but if not, search backwards until we hit the frame creation
     * instruction looking for a sw ra,x(sp) or lw ra,x(sp). Frame
     * creation is done by allways adding a *negative* number to sp reg.
     */
    if (ra_off == 0) {
	while ( !((op->i_t.op == OPCODE_ADDIU) && 
		  (op->i_t.rs == SP_REG) &&
		  (op->i_t.rt == SP_REG) &&
		  (op->i_t.s_imd < 0)) ) {

	    /*
	     * Look for saving or restoring of ra
	     */
	    if ((op->i_t.op == OPCODE_LW || op->i_t.op == OPCODE_SW) &&
		(op->i_t.rt == RA_REG) && 
		(op->i_t.rs == SP_REG)) {
		ra_off = op->i_t.s_imd;
		break;
	    }
	    op = (union instr_r4k *)((long)op - 4);
	}
    }

    /*
     * Set our return values
     */
    *pc_s = pc;
    *size = ssize;

    /*
     * If we still do not have ra_off, then we were a leaf
     * function with a stack frame. If ra_off is not a multiple
     * of four, we have hit our "heuristical fence post" (see gdb)
     */
    if (ra_off == 0) {
	return(FRAME_LEAF);
    } else if (ra_off & 0x03) {
	return(FRAME_LAST);
    }
	
    /*
     * We are a non-leaf with a frame.
     * caller = sp[ra_off >> 2]
     */
    *caller = (unsigned long *)(sp[ra_off >> 2]);
    if ( illegal_addr(*caller) ) {
	return(FRAME_LAST);
    } else {
	return(FRAME_NEXT);
    }
}

/*
 * stack_trace : Printout a stack trace
 * if bufptr is specified, put trace in it
 * else display the information
 * return the number of frames processed
 */
void
stack_trace(pc, sp, ra, frames, bufptr)
    unsigned long *pc, *sp, ra;
    int frames;
    unsigned long *bufptr;
{
    int i, size, tmp, leaf_cnt;
    unsigned long *caller, *pc_start;

    if ( illegal_addr(sp) ) {
	if(!bufptr)
	    printf("Cannot produce stack trace, invalid SP\n");
	return;
    }

    for (leaf_cnt=0, i=0; i<frames; i++) {
	
	/*
	 * Sanity check the PC & SP
	 */
	if ( illegal_addr(pc) || illegal_addr(sp) )
	    break;
	
	/*
	 * Analyze the stack trace and adjust the stack pointer
	 */
	tmp =  analyze_strace(pc, sp, &pc_start, &caller, &size);
	if(!bufptr)
	    printf("Frame %d : FP= 0x%.8x, PC= 0x%.8x, %3d bytes\n", 
		   i, sp, pc_start, size);
	else {
	    *bufptr++ = (unsigned long)sp;
	    *bufptr++ = (unsigned long)pc_start;
	}
	sp = (unsigned long *)((long)sp + size);

	/*
	 * There can only be one leaf function in the trace
	 * If the function is a leaf, ra has the callers PC
	 */
	if (tmp == FRAME_LAST || (tmp == FRAME_LEAF && leaf_cnt))
	    break;
	if ( tmp == FRAME_LEAF ) {
	    leaf_cnt += 1;
	    caller = (unsigned long *)ra;
	}
	pc = caller;
    }
}

do_stack_trace(context_str, init_sp, init_pc, ra, frames)
    char *context_str;
    unsigned long *init_pc, *init_sp, ra;
    int frames;
{
    printf("%s Stack Trace:\nInitial SP = 0x%x, Initial PC = 0x%x, RA = 0x%x\n", 
	   context_str, init_sp, init_pc, ra);
    stack_trace(init_pc, init_sp, ra, frames, (unsigned long *)0);
    putchar('\n');
}

/*
 * show_stack : Command line interface for stack_trace, default is 5 frames
 * from the system context.
 *
 * stack [-dm] [frames]
 */
int
show_stack(argc, argv)
     int argc;
     char **argv;
{
    static char *optstr = "dm";
    int proc, frames = 5, i, debug = 0, manual = 0;
    unsigned long *init_pc, *init_sp, ra;
    r4k_context_t *context;
    char *context_str;

    while ( (i=getopt(argc, argv, optstr)) >= 0) {
	switch (i) {
	    case 'd': debug = 1; break;
	    case 'm': manual = 1; break;
	    default : goto usage; break;
	}
    }
    if ((argc > optind && getnum(argv[optind++],10,&frames) == 0) ||
	(argc != optind) ) {
usage:
	printf("usage: %s [-%s] [frames]\n", argv[0], optstr);
	return(1);
    }
    
    if (debug) {
	init_pc = (unsigned long *)((long)show_stack + 0x10);
	init_sp = (unsigned long *)getcpu_sp();
	ra = 0;
	context_str = "Monitor";
    } else if (manual) {
	init_sp = (unsigned long *)gethex_answer("Enter Initial SP : ", 
						 0, 0, 0xffffffff);
	init_pc = (unsigned long *)gethex_answer("Enter Initial PC : ", 
						 0, 0, 0xffffffff);
	ra = gethex_answer("Enter Initial RA : ", 0, 0, 0xffffffff);
	context_str = "Manual";
    } else {
	for(proc = 0; proc < 2; proc++) {  /* do both kernel and process level */
	    if (proc) {
		context = process_context_ptr;
		context_str = "Process Level";
	    } else {
		context = kernel_context_ptr;
		context_str = "Kernel Level";
	    }
	    if ( !context ) {
		printf("%s: %s context state is invalid, can not proceed\n",
		       argv[0], proc ? "process" : "kernel");
		return(0);
	    }
	    init_pc = (unsigned long *)context->epc;
	    init_sp = (unsigned long *)context->regs[SP_REGNUM];
	    ra = context->regs[RA_REGNUM];
	    do_stack_trace(context_str, init_sp, init_pc, ra, frames);
	}
	return(0);
    }
    do_stack_trace(context_str, init_sp, init_pc, ra, frames);
    return(0);
}

/*
 * stack_frame : Display one stack frame
 */
void
stack_frame(pc, sp, ra, frame)
     unsigned long *pc, *sp, ra;
     int frame;
{
    int i=0, size, tmp, leaf_cnt=0;
    unsigned long *caller, *pc_start;

    if ( illegal_addr(sp) ) {
	printf("Cannot display stack frame, invalid SP\n");
	return;
    }

    while ( ! illegal_addr(pc) ) {

	/*
	 * Analyze the stack trace
	 */
	tmp =  analyze_strace(pc, sp, &pc_start, &caller, &size);
	if ( i == frame ) 
	    break;             /* this is the one we want */

	sp = (unsigned long *)((long)sp + size);

	/*
	 * There can only be one leaf function in the trace
	 * If the function is a leaf, ra has the callers PC
	 */
	if (tmp == FRAME_LAST || (tmp == FRAME_LEAF && leaf_cnt))
	    return;
	if ( tmp == FRAME_LEAF ) {
	    leaf_cnt += 1;
	    caller = (unsigned long *)ra;
	}
	pc = caller;
	i += 1;
    }
    
    /*
     * Printout the frame
     */
    if ( illegal_addr(pc) ) {
	printf("Failed to find frame, PC : 0x%.8x is illegal\n", pc);
	return;
    }
    printf("Stack Frame %d, SP = 0x%.8x, Size = %d bytes\n",
	   i, sp, size);
    for (i=0; i<size; i += sizeof(unsigned long)) {
	printf("[0x%.8x : sp + 0x%.3x] = 0x%.8x\n", sp, i, *sp);
	sp += 1;
    }
}
     
static char *foptstr = "dp";

/*
 * show_frame : Command line interface to stack_frame. Displays one
 * stack frame in its entirety. Default is the system kernel context.
 * frame [-dp] [frame_number]
 */
int
show_frame(argc, argv)
     int argc;
     char **argv;
{
    int frame = 0, i, debug = 0, proc = 0;
    unsigned long *init_pc, *init_sp, ra;
    r4k_context_t *context = kernel_context_ptr;

    while ( (i=getopt(argc, argv, foptstr)) >= 0) {
	switch (i) {
	    case 'd': debug = 1; break;
	    case 'p': proc = 1; break;
	    default : goto usage; break;
	}
    }
    if ((argc > optind && getnum(argv[optind++],10,&frame) == 0) ||
	(argc != optind) ) {
usage:
	printf("usage: %s [-%s] [frame]\n", argv[0], foptstr);
	return(1);
    }

    if ( debug ) {
	init_pc = (unsigned long *)((long)show_frame + 0x10);
	init_sp = (unsigned long *)getcpu_sp();
	ra = 0;
    } else {
	if (proc) {
	    context = process_context_ptr;
	} else {
	    context = kernel_context_ptr;
	}
	if ( !context ) {
	    printf("%s: %s context state is invalid, can not proceed\n",
		   argv[0], proc ? "process" : "kernel");
	    return(0);
	}
	init_pc = (unsigned long *)context->epc;
	init_sp = (unsigned long *)context->regs[SP_REGNUM];
	ra = context->regs[RA_REGNUM];
    }
    stack_frame(init_pc, init_sp, ra, frame);
    return(0);
}

/* End of Module */
