/* $Id: stacks_chain.c,v 3.1.16.4 1996/08/28 13:04:27 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/stacks_chain.c,v $
 *------------------------------------------------------------------
 * stacks_chain.c -- parse routines for stack creation, manipulation,
 * and monitoring routines
 *  
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stacks_chain.c,v $
 * Revision 3.1.16.4  1996/08/28  13:04:27  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.16.3  1996/08/07  09:01:58  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.1.16.2  1996/03/21  23:21:31  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.16.1  1996/03/18  21:31:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/09  05:10:34  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:04  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/buff.h"
#include "sched_private.h"
#include "stacks.h"
#include "logger.h"
#include "ttysrv.h"
#include "../../boot/reason.h"
#include "../../boot/save_stack.h"
#include "../dev/monitor1.h"
#include "../os/free.h"
#include "sys_registry.h"
#include "exception.h"
#include "config.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */


/*
 * Externs
 */
extern queuetype stackQ;		/* list of analyzed stacks */
extern long *boot_data;			/* pointer to data from bootstrap monitor */
extern char *boot_reason;		/* reason why system rebooted */
extern ulong spurcnt;
extern intstacktype intstacks[];



/*
 * stack_backtrace
 */

static void stack_backtrace (int stacknum)
{
    sprocess *p;

    p = process_pid2ptr(stacknum);
    if (!p) {
	printf("\nNo such process #%d", stacknum);
	return;
    }
    if (p->caller_pc == 0) {
	printf("\nProcess %d is currently active", stacknum);
	return;
    }
    printf("\nProcess %d:  %s", stacknum, p->name);
    printf("\n  Stack segment %#x - %#x", p->stack,
	   (uchar *)p->stack + p->stacksize);
    stack_printtrace(p);
}

/*
 * stack_whyreload
 * Print out reason we restarted and possibly a stack trace
 * This is processor independant. The frame data structure
 * only contains the frame pointer and the PC for that frame.
 */
void 
stack_whyreload (void)
{
    struct frame *frames;
    int i;

    /*
     * Ensure that boot_data[] is a reasonable pointer:
     */
    if ((boot_data == (long *)NULL) || (boot_data == (long *)-1) ||
	(touch_device(boot_data) != TRUE)) {
	printf("\n\nInvalid boot_data=%#x\n", boot_data);
	return;
    }

    /*
     * If BOOT_COUNT is correct and a system failure caused the
     * rebooot, print the reason for the last reboot and then print
     * the stack trace:
     */
    if ((boot_data[BOOT_COUNT] == BOOTDATA) &&
	(boot_data[BOOT_REASON] != EXC_RESET) &&
	(boot_data[BOOT_REASON] != EXC_ABORT) &&
	(boot_data[BOOT_REASON] != EXC_RELOAD)) {
	if (boot_reason)
	    printf("\n\nSystem was restarted by %s", boot_reason);
	frames = (struct frame *)&boot_data[BOOT_STACK_TRACE];
	if ((frames != NULL) &&
	    ((frames[0].fp != 0) || (frames[0].pc != 0))) {
	    if (last_sysversion)
		printf("\n%s", last_sysversion);
	    else {
		printf("\n%s (current version)", oemversion);
                print_text_base_string();
            }
	    printf("\nStack trace from system failure:\n");
	    for (i = 0; i < BOOT_STACK_FRAMES; i++) {
		if ((frames[i].fp == 0) && (frames[i].pc == 0))
		    break;
		printf("FP: %#x, RA: %#x\n",
		       frames[i].fp, frames[i].pc);
	    }
	}
    }

    /*
     * Also, check out the slave's crash history.  On HSA systems,
     * we swap processors after a crash, so the interesting info about
     * the  last software crash is most likely hiding over on the slave.
     */
    reg_invoke_slave_stack_whyreload();
}

/*
 * show_stacks
 * Show process and interrupt stacks
 */

static const char mhead[] = "Minimum process stacks:\n Free/Size   Name";

void show_stacks (parseinfo *csb)
{
    stacktype *stack;
    int i;

    i = GETOBJ(int,1);
    if (i > 0) {
	/*
	 * Parser only lets us here if enabled
	 */
	stack_backtrace(i);
	return;
    }

    /*
     * Show minimum process stacks
     */
    automore_enable(NULL);
    stack = (stacktype *) stackQ.qhead;
    if (stack) {
	automore_header(mhead);
	while (stack) {
	    printf("\n%-5d/%5d  %s", stack->free, stack->size, stack->name);
	    stack = stack->next;
	}
    }

    /*
     * Show interrupt stacks
     */
   automore_header("\nInterrupt level stacks:"
		   "\nLevel    Called Unused/Size  Name");
    for (i = 0; i < STACK_INT_NUMBER; i++) {
	if (intstacks[i].size != 0L)
	    printf("\n  %d%-12d%-7d/%4d  %s",i+1, *intstacks[i].count,
		stackleft((ulong *)*intstacks[i].base,intstacks[i].size),
		intstacks[i].size,intstacks[i].desc);
    }
    automore_header(NULL);

    /*
     * If we had any spurious interrupts, show the count here
     */
    if (spurcnt)
	printf("\n\nSpurious interrupts: %d", spurcnt);
    /*
     * Print out why we reloaded, possibly with a stack trace
     */
    stack_whyreload();
    automore_disable();
}
