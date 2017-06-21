/* $Id: stacks_68_chain.c,v 3.1.18.1 1996/03/18 21:58:43 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-68/stacks_68_chain.c,v $
 *------------------------------------------------------------------
 * stacks_68_chain.c -- M68K specific stack parser routines
 *  
 * June 1985, Kirk Lougheed
 * October 1993, Michael Beesley
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stacks_68_chain.c,v $
 * Revision 3.1.18.1  1996/03/18  21:58:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.8.1  1996/03/09  05:12:03  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  22:16:18  hampton
 * Bump version number to 3.1 on new placeholder files.
 *
 * Revision 1.1  1996/02/21  21:52:26  hampton
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
 * Hardware specific printing of a stack trace called by stack_backtrace()
 */
void 
stack_printtrace (sprocess *p)
{
    long *stack;

    automore_enable(NULL);
    for (stack = (long *)p->savedsp; stack; stack = (long *)stack[0]) {
	printf("\n  FP: %#x, RA: %#x", stack[0], stack[1]);
    }
    automore_disable();
}
