/* $Id: stacks_68.c,v 3.2.4.1 1996/03/18 21:58:42 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-68/stacks_68.c,v $
 *------------------------------------------------------------------
 * stacks_68.c -- M68K specific stack routines
 *  
 * June 1985, Kirk Lougheed
 * October 1993, Michael Beesley
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stacks_68.c,v $
 * Revision 3.2.4.1  1996/03/18  21:58:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.8.2  1996/03/16  07:33:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1.8.1  1996/03/09  05:12:02  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2  1996/03/09  01:01:01  hampton
 * Migrate processor specific files to src-xxx directories.  [CSCdi51128]
 *   Step 1: Create copies of the files in their new location.
 *
 * Revision 3.1  1996/02/21  22:16:16  hampton
 * Bump version number to 3.1 on new placeholder files.
 *
 * Revision 3.2  1995/11/17  18:51:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:48:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/14  00:18:09  dkatz
 * CSCdi40528:  errmsg traceback isnt robust
 * Validate addresses before dereferencing.
 *
 * Revision 2.2  1995/07/14  09:35:45  schaefer
 * CSCdi36176: misalign/spurious access traceback broken for leaf functions
 *
 * Revision 2.1  1995/06/07  21:58:55  hampton
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
#include "../os/region.h"

/*
 * externs
 */
extern void level1(void), level2(void), level3(void), level4(void),
            level5(void), level6(void);

void *lev3ssubr;		/* m68k specific */
ushort mci_cbus_intflag;	/* high-end specific */

/*
 * stack_hardware_init
 * Called by level_init() to set up hardware specific interrupt vectors
 */
void 
stack_hardware_init (void)
{
    lev3ssubr = NULL;
}

/*
 * stack_setup_interrupt
 * Initialize an interrupt level stack frame
 */
void 
stack_setup_interrupt (int index, void *routine)
{
    uchar *address;
    int size, level;
    void (*oldHandler)(void);

#ifdef DEBUG
    ttyprintf(CONTTY, "stack_setup_interrupt(index=%d, routine=%#x)\n",
	      index, routine);
#endif

    /*
     * Compute start of stack frame (it grows from high address to low address)
     * Remember handler subroutine address for interrupt level handler.
     */
    address = *intstacks[index].base;
    size = intstacks[index].size;
    level = index + 1;
    *intstacks[index].stack = address + (size - 4);
    *intstacks[index].routine = routine;

    if (routine != NULL) {
	switch (level) {
	case LEVEL1:
	    exceptionHandler(IRQ1Vector, level1);
	    break;
	case LEVEL2:
	    exceptionHandler(IRQ2Vector, level2);
	    break;
	case LEVEL3:
	    /*
	     * The C3000 installs a special level 3 exception handler at
	     * start up.  Make sure it can't be overridden.
	     */
	    oldHandler = exceptionHandler(IRQ3Vector, level3);
	    if (oldHandler != exception_handler)
		exceptionHandler(IRQ3Vector, oldHandler);
	    break;
	case LEVEL4:
	    exceptionHandler(IRQ4Vector, level4);
	    break;
	case LEVEL5:
	    exceptionHandler(IRQ5Vector, level5);
	    break;
	case LEVEL6:
	    exceptionHandler(IRQ6Vector, level6);
	    break;
	}
    }
}

/*
 * stack_setup_process
 * Hardware specific process stack initialization for M68K
 */

#define M68K_INITPS  0x2000
void 
stack_setup_process (sprocess *sp, long stackstart, void *address, 
		     long arg)
{
    long *spp;

    stackstart -= 12;		/* 3 longs for initial stack contents */
    sp->savedsp = (ulong *)stackstart;	/* save process stack pointer */
    spp = (long *) stackstart;	/* spp points to a stack element */
    *spp++ = 0;			/* saved A6 for unlink */
    *spp++ = (long) address;	/* entry address will go here */
    *spp++ = (long) process_hari_kari;	/* should this ever return, hari-kari */
    *spp = arg;			/* passed parameter */
    sp->ps = M68K_INITPS;	/* set initial PS value */
}

/*
 * trace_caller:
 * Return a pointer to the next frame. given the address of the
 * current frame and set pc to the owner of the next frame
 */
ulong *
trace_caller(ulong *frame, ulong **pc)
{
    if (!region_find_by_addr(frame+1) || !region_find_by_addr(frame))
	return(0);
    *pc = (ulong *) *(frame+1);
    return((ulong *)(*frame));
}
