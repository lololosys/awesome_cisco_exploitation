/* $Id: stack.c,v 3.2 1995/11/17 07:41:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/stack.c,v $
 *------------------------------------------------------------------
 * stackdump -- show trace of current stack
 *
 * March 1987, Greg Satz
 *
 * Copyright (c) 1987-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stack.c,v $
 * Revision 3.2  1995/11/17  07:41:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"
#include "stdio.h"

extern char abort;

#define	FP	14
#define	SS	15
#define	PC	19

#define	STACKSIZE	sizeof(u_long)


#ifdef	SHOW_FUNC_ARGS
#define HIWORD	0xffff0000
#define LOWORD	0x0000ffff
#define ADDLSP	0xdffc0000	/* addl #x,sp    */
#define ADDWSP	0xdefc0000	/* addw #x,sp    */
#define LEASP	0x4fef0000	/* lea	#x(sp),sp*/
#define MOVLD0	0x20000000	/* movl d0,dx	 */
#define MOVLA0	0x20400000	/* movl d0,ax	 */
#define MVLMSK	0xf1ff0000
#define	MOVQ	0x70000000	/* moveq #x,rx	 */
#define	MVQMSK	0xf0000000
#define ADQMSK	0xf1ff0000
#define ADDQSP	0x508f0000	/* addql #x,sp   */
#define ADDQWSP	0x504f0000	/* addqw #x,sp   */
#endif	/* SHOW_FUNC_ARGS */



struct frame {
    u_long fp;				/* frame pointer */
    u_long pc;				/* program counter */
#ifdef	SHOW_FUNC_ARGS
    short nargs;			/* arguments sizes */
#endif	/* SHOW_FUNC_ARGS */
};



struct frame *nextframe();

#ifdef	SHOW_FUNC_ARGS
u_long argn();
#endif	/* SHOW_FUNC_ARGS */

stackdump(regs)
    u_long *regs;
{
    struct frame *fp, frame;

    fp = &frame;
    if (regs[FP] == 0) {
	printf("No stack to dump (NULL frame pointer)\n");
    } else {
	getcurframe(regs, fp);
	while ((fp != (struct frame *)NULL) && !abort) {
	    printframe(fp);
	    fp = nextframe(fp);
	}
    }
}



getcurframe(regs, fp)
    u_long *regs;
    struct frame *fp;
{
    printf("Current PC: %#x\n", regs[PC]);
    fp->fp = regs[FP];
    fp->pc = *(u_long *)(fp->fp + STACKSIZE);

#ifdef	SHOW_FUNC_ARGS
    if (fp->fp == NULL)
	fp->nargs = 0;
    else
	findnargs(fp);
#endif	/* SHOW_FUNC_ARGS */
}

#ifdef	SHOW_FUNC_ARGS
findnargs(fp)
    struct frame *fp;
{
    u_long addr, inst, val;

    /*
     * This routine attempts to deduce the number of arguments input to a
     * function by examining instruction(s) in the calling program for stack
     * pointer adjustments.
     *
     * An example of gcc compiled code showing the calling convention:
     *		movel d2,sp@-		| push first argument
     *		movel a6@(8),sp@-	| push second argument
     *		jbsr _getcurframe	| call subroutine
     *		addqw #8,sp		| pop arguments
     * Note that we can decode the addqw instruction to deduce the number of
     * arguments that were pushde on the stack for us.
     *
     * Note that the GNU C compiler, gcc, requires the following option to
     * generate code using this calling convention:
     *     `-fno-defer-pop'
     *          Always pop the arguments to each function call as soon as that
     *          function returns.  Normally the compiler (when optimizing) lets
     *          arguments accumulate on the stack for several function calls
     *		and pops them all at once.
     */
    addr = fp->pc;
    inst = *(u_long *)addr;

    /*
     * Skip any instruction saving the return argument:
     */
    if ((inst & MVLMSK) == MOVLA0 || (inst & MVLMSK) == MOVLD0 ||
	(inst & MVQMSK) == MOVQ) {
	addr += 2;
	inst = *(u_long *)addr;
    }

    /*
     * Now look for an instruction cleaning up arguments from stack
     * and decode it to deduce the number of arguments:
     */
    if ((inst & ADQMSK) == ADDQSP || (inst & ADQMSK) == ADDQWSP) {
	val = (inst >> (16+9)) & 07;
	if (val == 0)
	    val = 8;
    } else if ((inst & HIWORD) == ADDLSP)
	val = *(u_long *)(addr + 2);
    else if ((inst & HIWORD) == ADDWSP || (inst & HIWORD) == LEASP)
	val = inst & LOWORD;
    else
	val = 0;
    fp->nargs = val / STACKSIZE;
}
#endif	/* SHOW_FUNC_ARGS */

printframe(fp)
    struct frame *fp;
{
#ifdef	SHOW_FUNC_ARGS
    u_long s, send;
#endif	/* SHOW_FUNC_ARGS */

    printf("FP: %#x RA: %#x ", fp->fp, fp->pc);

#ifdef	SHOW_FUNC_ARGS
    s = fp->nargs;
    if (s > 0) {
	printf(" ARGS(%d):", fp->nargs);
	send = s;
	for (;;) {
	    send--;
	    printf(" %#x", argn(s - send, fp));
	    if (send <= 0)
		break;
	    putchar(',');
	}
    }
#endif	/* SHOW_FUNC_ARGS */

    putchar('\n');
}



#ifdef	SHOW_FUNC_ARGS
u_long argn(n, fp)
    short n;
    struct frame *fp;
{
    u_long argaddr, w;

    argaddr = fp->fp + STACKSIZE + (n * STACKSIZE);
    w = *(u_long *)argaddr;
    return w;
}
#endif	/* SHOW_FUNC_ARGS */



struct frame *nextframe(fp)
    struct frame *fp;
{

#ifdef DEBUG
    printf("nextframe: fp %#x pc %#x new fp %#x new pc %#x\n",
	   fp->fp, fp->pc, *(u_long *)fp->fp, *(u_long *)(fp->fp + STACKSIZE));
#endif

    if (*(u_long *)fp->fp <= fp->fp)
	return((struct frame *)NULL);
    fp->fp = *(u_long *)fp->fp;
    fp->pc = *(u_long *)(fp->fp + STACKSIZE);
#ifdef	SHOW_FUNC_ARGS
    findnargs(fp);
#endif	/* SHOW_FUNC_ARGS */
    return (fp);
}
