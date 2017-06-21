/* $Id: stacks.c,v 3.6.28.4 1996/08/28 13:04:25 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/stacks.c,v $
 *------------------------------------------------------------------
 * stacks.c -- stack creation, manipulation, and monitoring routines
 *  
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stacks.c,v $
 * Revision 3.6.28.4  1996/08/28  13:04:25  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.28.3  1996/06/28  23:25:06  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.28.2  1996/03/21  23:21:29  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.6.28.1  1996/03/18  21:31:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.5  1996/02/10  00:18:12  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3.2.4  1996/02/08  08:44:00  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.3  1996/01/26  22:27:49  mbeesley
 * Branch: ELC_branch
 * Tidy up show stack.
 *
 * Revision 3.3.2.2  1996/01/16  01:04:07  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/11  19:11:50  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.6  1996/01/22  06:56:22  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/11  04:05:32  dbath
 * CSCdi45341:  Cant get crash stack on HSA systems.
 * Include the slave's crash stack (if it has crashed recently)
 * at the bottom of the 'show stack' output.  Also, add what
 * appears to be a missing break from a switch statement,
 * while I have the bonnet up.
 *
 * Revision 3.4  1995/12/07  22:31:19  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  18:51:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:57  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/23  18:48:28  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/19  06:48:59  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:58:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <reent.h>
#include <ciscolib.h>
#include "../../boot/buff.h"
#include "sched_private.h"
#include "stacks.h"
#include "logger.h"
#include "ttysrv.h"
#include "../../boot/reason.h"
#include "../../boot/save_stack.h"
#include "../dev/monitor1.h"
#include "../os/free.h"
#include "exception.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */


/*
 * Local Storage
 */
queuetype stackQ;		/* list of analyzed stacks */
long *boot_data;		/* pointer to data from bootstrap monitor */
char *boot_reason;		/* reason why system rebooted */

/*
 * Hardware specific storage
 *
 * Note, on some processors having locality between the variables for
 * a particular interrupt level stack speeds up the interrupt context switch.
 * To this end, these variables are moved in the data section, which 
 * lets the linker group them together, rather then in the bss section.
 *
 * A modification to the compiler has been requested to allow alignment
 * of these variables to also be specified.
 */

/*
 * Level 1 interrupt stack
 */
ulong l1cnt = 0;
uchar *l1stk = 0, *lev1subr = 0, *l1sp = 0;

/*
 * Level 2 interrupt stack
 */
ulong l2cnt = 0;
uchar *l2stk = 0, *lev2subr = 0, *l2sp = 0;

/*
 * Level 3 interrupt stack
 */
ulong l3cnt = 0;
uchar *l3stk = 0, *lev3subr = 0, *l3sp = 0;

/*
 * Level 4 interrupt stack
 */
ulong l4cnt = 0;
uchar *l4stk = 0, *lev4subr = 0, *l4sp = 0;

/*
 * Level 5 interrupt stack
 */
ulong l5cnt = 0;
uchar *l5stk = 0, *lev5subr = 0, *l5sp = 0;

/*
 * Level 6 interrupt stack
 */
ulong l6cnt = 0;
uchar *l6stk = 0, *lev6subr = 0, *l6sp = 0;

/*
 * Level 7 interrupt stack
 */
ulong l7cnt = 0;
uchar *l7stk = 0, *lev7subr = 0, *l7sp = 0;

/*
 * Spurious interrupt count and stack base addresses
 */
ulong spurcnt;
uchar *l1base, *l2base, *l3base, *l4base, *l5base, *l6base, *l7base;

/*
 * Data copied into intstacks[]:
 */
intstacktype intstacks[STACK_INT_NUMBER] = {
    { &l1base, &l1stk, &lev1subr, &l1cnt, 0L, 0L, NULL, NULL },
    { &l2base, &l2stk, &lev2subr, &l2cnt, 0L, 0L, NULL, NULL },
    { &l3base, &l3stk, &lev3subr, &l3cnt, 0L, 0L, NULL, NULL },
    { &l4base, &l4stk, &lev4subr, &l4cnt, 0L, 0L, NULL, NULL },
    { &l5base, &l5stk, &lev5subr, &l5cnt, 0L, 0L, NULL, NULL },
    { &l6base, &l6stk, &lev6subr, &l6cnt, 0L, 0L, NULL, NULL },
    { &l7base, &l7stk, &lev7subr, &l7cnt, 0L, 0L, NULL, NULL }
};

/*
 * level_init
 * Called from the scheduler at startup to clear interrupt level assignments.
 */

void level_init (void)
{
    queue_init(&stackQ, 0);
}

/*
 * createlevel
 * Set up interrupt level data structures, do sanity checking
 */

void createlevel (
    int level,
    void (*routine) (void),
    char *string)
{
    int index = level - 1;
    int size = STACK_SIZE_INTERRUPT;
    uchar *addr;

#ifdef DEBUG
    ttyprintf(CONTTY,
	      "\ncreatelevel(level=%d, routine=%#x, string=%s, size=%d)\n",
	      level, routine, string, size);
#endif

    if (level > STACK_INT_NUMBER) {
	ttyprintf(CONTTY,
		  "\nSTACKS: createlevel called with bad level number");
	return;
    }
    if (intstacks[index].size != 0L) {
	ttyprintf(CONTTY,
		  "\nSTACKS: multiple devices assigned to level %d: %s, and %s.",
		  level,intstacks[index].desc,string);
        return;
    }
    addr = mempool_malloc(MEMPOOL_CLASS_ISTACK, size);
    if (!addr) {
	system_running = FALSE;
	crashdump(0);
	return;
    }
    name_memory(addr, "Interrupt Stack");
    memset(addr, 0xff, size);			/* set to all ones */
    intstacks[index].desc = string;
    intstacks[index].size = size;
    intstacks[index].low = size;
    *intstacks[index].count = 0L;		/* start counting */
    *intstacks[index].base = addr;		/* set base */
    intstacks[index].reent_block = _reent_init();
    stack_setup_interrupt(index, routine);

#ifdef DEBUG
    ttyprintf(CONTTY, "intstacks[%d]: *base=%#x, *stack=%#x, *routine=%#x, *count=%d, size=%d, low=%d, desc=%s\n",
	      index, *intstacks[index].base, *intstacks[index].stack,
	      *intstacks[index].routine, *intstacks[index].count,
	      intstacks[index].size, intstacks[index].low,
	      intstacks[index].desc);
    ttyprintf(CONTTY, "exiting createlevel()\n\n");
#endif
}

/*
 * level_exists()
 * Tell if a given level has already been set up
 */

boolean level_exists (int level)
{
    if ((level > STACK_INT_NUMBER) || (level < 1))
	return(FALSE);
    return (intstacks[level - 1].size != 0L);
}

/*
 * changelevel
 * Change the routine and string for a level that is already in use.
 */

void changelevel (
    int level,
    void (*routine) (void),
    char *string)
{
    int index = level - 1;

    if (level > STACK_INT_NUMBER) {
	printf("\nSTACKS: changelevel called with bad level number");
	return;
    }			
    if (intstacks[index].size == 0L) {
	printf("\nSTACKS: attempt to change a non-existent level %d: %s",
		level,string);
        return;
    }
    intstacks[index].desc = string;
    *intstacks[index].count = 0L;		/* start counting */
    *intstacks[index].routine = (uchar *)routine;
}

/*
 * stackleft
 * Find the number of bytes free in a process or interrupt stack.
 * Stacks are inited to all ones. So, scan for not all ones.
 * That will be close to the stack bottom.  This is an approximation, but
 * should be quite close.
 */

int stackleft (ulong *baseadd, ulong size)
{
    ushort *ptr;
    int i;

    ptr = (ushort *)baseadd;
    for (i = 0; i < size; i+=2)
	if (*ptr++ != 0xFFFF)
	    break;
    return(i);
}

/*
 * stack_analyze
 * Called from scheduler on process death to analyze stack
 */

void stack_analyze (sprocess *sp)
{
    stacktype *stack;
    int count;

    /*
     * First, count the number of unused stack bytes.
     */
    count = stackleft(sp->stack, sp->stacksize);

    /*
     * Search existing stack list.  If a match is found,
     * and we have fewer unused bytes, update unused count.
     */
    stack = (stacktype *) stackQ.qhead;
    while (stack) {
	if (!strcasecmp(stack->name, sp->name)) {
	    if (count < stack->free)
		stack->free = count;
	    return;
	}
	stack = stack->next;
    }

    /*
     * New process. Create and enqueue entry.
     */
    stack = malloc(sizeof(stacktype));
    if (!stack)
	return;
    stack->size = sp->stacksize;
    stack->free = count;
    stack->name = sp->name;
    enqueue(&stackQ, stack);
}

/*
 * stack_check
 * Check the interrupt stacks for overflow
 * called from network background
 */

void stack_check (void)
{
    int i, left;

    for (i = 0; i < STACK_INT_NUMBER; i++) {
	if (intstacks[i].size != 0L) {
	    left = stackleft((ulong *)*intstacks[i].base, intstacks[i].size);
	    if (left <= STACK_INT_THRESHOLD && left <= intstacks[i].low) {
		intstacks[i].low = left;
		printf("\nINTSTACK: Stack for level %s running low, %d/%d",
		    intstacks[i].desc, left, intstacks[i].size);
		(*kernel_errmsg)(&msgsym(STACKLOW, SYS), "level",
		    intstacks[i].desc, left, intstacks[i].size);
	    }
	}
    }
}

/*
 * onintstack
 * Return TRUE if running on an interrupt stack
 */

uint onintstack (void)
{
    int i;
    uchar *frame;

    frame = (uchar *)&frame;
    for (i = 0; i < STACK_INT_NUMBER; i++)
	if (intstacks[i].size != 0L)
	    if (frame >= *intstacks[i].base && frame <= *intstacks[i].stack)
		return(i+1);
    return(0);
}

/*
 * stack_reent_ptr
 * a parameter of zero returns the forkx->reent_block while a positive
 * paramter will return the reent_block associated with the interrupt
 * level.
 */
static void *reent_block = NULL;
void *
stack_reent_ptr (uint i)
{
    if (i > STACK_INT_NUMBER) {
	/*
	 * XXX Fix me: it should not be possible to get here,
	 * adding some kind of error message is probably a good idea.
	 */
	if (reent_block == NULL) {
	    reent_block = _reent_init();
	}
	return(reent_block);
    }
    else if (i == 0) {
	if (forkx == NULL) {
	    /* we have probably been called in the scheduler context */
	    if (reent_block == NULL) {
		reent_block = _reent_init();
	    }
	    return(reent_block);
	}
	return(forkx->reent_block);
    }
    return(intstacks[i-1].reent_block);
}

/*
 * make_reason
 * Create reason string which contains why we rebooted or NULL
 */

#define	REASON_STR_LEN	256

void 
make_reason (void)
{
    uint len;
    int pc;
    uint address;

    len = 0;
    boot_reason = malloc(REASON_STR_LEN);
    if (boot_reason == NULL) {
	crashdump(0);
	return;
    }

    boot_data = mon_lasterror();
    if ((boot_data == (long *)-1) || (boot_data == (long *)0)) {
	len = sprintf(boot_reason,
		      "unknown reload cause - invalid boot_data ptr %#x",
		      boot_data);
	return;
    }

    /*
     * If a correct length is not present, we probably trashed ourselves
     * pretty badly last time.
     */
    if (boot_data[BOOT_COUNT] != BOOTDATA) {
	len = sprintf(boot_reason,
		      "unknown reload cause - suspect boot_data[BOOT_COUNT] %#x, BOOT_COUNT %d, BOOTDATA %d",
		      boot_data[BOOT_COUNT], BOOT_COUNT, BOOTDATA);
	return;
    }

    pc = (int)boot_data[BOOT_PC];
    address = (uint)boot_data[BOOT_ADDRESS];

    switch (boot_data[BOOT_REASON]) {
    case EXC_RESET:
	len = sprintf(boot_reason, "power-on");
	break;
    case EXC_ABORT:
	len = sprintf(boot_reason, "abort at PC %#lx", pc);
	break;
    case EXC_BUSERR:
	len = sprintf(boot_reason, "bus error at PC %#lx, address %#lx",
		      pc, address);
	break;
    case EXC_ADRERR:
	len = sprintf(boot_reason, "address error at PC %#lx, address %#lx",
		      pc, address);
	break;
    case EXC_WATCHDOG:
	len = sprintf(boot_reason, "watchdog timer expired");
	break;
    case EXC_RELOAD:
	len = sprintf(boot_reason, "reload");
	break;
    case EXC_PARITY:
	len = sprintf(boot_reason, "processor memory parity error at PC %#lx, address %#lx",
		      pc, address);
	break;
    case EXC_SHMEM_PE:
	len = sprintf(boot_reason, "shared memory parity error at PC %#lx",
		      pc);
	break;
    case EXC_UNEXP_INT:
	len = sprintf(boot_reason, "unexpected interrupt at PC %#lx, address %#lx",
		      pc, address);
	break;
    default:
	{
	    char *err_msg;

	    /*
	     * If the pointer to an error message is valid,
	     * print the error message. Otherwise, just print
	     * out the bad pointer.
	     * Remember that these sprintfs must fit in boot_reason,
	     * or we'll overwrite the memory linked list and be hosed!
	     */
	    err_msg = (char *)boot_data[BOOT_REASON];
	    if (touch_device(err_msg) == TRUE) {

		/*
		 * Is the error message really a string?
		 */
		if (is_string(err_msg)) {

		    char *first_msg = "error - ";
		    char *second_msg = ", PC %#lx";
		    int max_err_msg_len;

		    max_err_msg_len = REASON_STR_LEN -
			(strlen(first_msg) + strlen(second_msg) + 8 + 1);
		    strcpy(boot_reason, first_msg);
		    sstrncat(boot_reason, err_msg, max_err_msg_len);
		    sprintf((char *)((int)boot_reason + strlen(boot_reason)),
			    second_msg, pc);
		    len = strlen(boot_reason);
		} else {

		    /*
		     * Error, message starts with non ascii character:
		     */
		    len = sprintf(boot_reason,
			    "unknown reload cause - ptr to non-ascii bytes %#x",
			    err_msg);
		    return;
		}
	    } else {
		len = sprintf(boot_reason,
			      "unknown reload cause - reason ptr %#x, PC %#x, address %#x",
			      err_msg, pc, address);
	    }
	}
	break;
    }

    /*
     * Sanity check on length:
     */
    if (len == (uint)-1)
	len = 0;
    boot_reason[len] = '\000';
}

/*
 * stack_remaining
 * calculate (roughly) what percentage of forkx's stack is left for use,
 * relative to the CURRENT value of the stack pointer.  Warning - this
 * code assumes that the free part of the stack is between forkx->stack
 * and the current SP value...
 */
int stack_remaining (void)
{
    long stackfree, stacksize;

    stacksize = forkx->stacksize;
    stackfree = (long)(&stackfree) - (long)(forkx->stack);
    if (stackfree < 0)
	stackfree = - stackfree;
    return((stackfree*100) /stacksize);
}

/*
 * process_stacks
 * Check processes stack sizes for near overflow.
 * Called by network_background.
 */

#define STACK_THRESHOLD 50

void process_stacks (void)
{
    sprocess_list *array, *next;
    sprocess *pp;
    int i, left;

    for (array = sarrayQ.qhead; array; array = next) {
      mem_lock(array);
      for (i = 0; i < MAX_PROC_PER_ARRAY; ++i) {
	pp = array->sl_process[i];
	if (pp == NULL)
	    continue;		/* empty slot */
	left = stackleft(pp->stack, pp->stacksize);
	if (left <= STACK_THRESHOLD && left < pp->lowstack) {
	    pp->lowstack = left;
	    printf("\nSCHED: Stack for process %s running low, %d/%d",
		pp->name, left, pp->stacksize);
	    (*kernel_errmsg)(&msgsym(STACKLOW, SYS), "process",
		pp->name, left, pp->stacksize);
	}
      }
      next = array->sl_next;
      free(array);
    }
}
