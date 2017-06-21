/* $Id: sched_compatibility.c,v 3.3.62.3 1996/08/28 13:04:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/sched_compatibility.c,v $
 *------------------------------------------------------------------
 * sched_compatibility.c - Compatibility code for old scheduler calls
 *
 * April 1995, David Hampton
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sched_compatibility.c,v $
 * Revision 3.3.62.3  1996/08/28  13:04:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.2  1996/08/03  23:35:03  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.3.62.1  1996/03/18  21:31:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/09  05:10:26  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.26.1  1996/02/20  16:41:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:51:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:48  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  06:19:02  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.1  1995/06/07 21:58:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sched_private.h"
#include "stacks.h"
#include "logger.h"
#include "ttysrv.h"
#include "exception.h"
#include "signal.h"
#include "interface_private.h"
#include "../if/network.h"
#include "profile.h"
#include "async.h"
#include "config.h"
#include "mgd_timers.h"
#include "chunk.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */

/*
 * Local Storage
 */
list_header procq_runnorm;	/* Standard Priority process list */
list_header procq_runlow;	/* Low Priority process list */


/*
 * old_run_add
 *
 * Add a process to a compatibility run queue
 */
void old_run_add (sprocess *p)
{
    list_header *new_list;

    switch (p->priority) {
      case PRIO_HIGH:
      case PRIO_NORMAL:		new_list = &procq_runnorm;	break;
      case PRIO_LOW:		new_list = &procq_runlow;	break;
      default:/*need something*/new_list = &procq_runlow;	break;
    }
    list_move(new_list, &p->sched_list);
    p->on_old_queue = TRUE;
}

/*
 * cfork
 *
 * Create a normal priority process that prefers to use the old
 * scheduler model.
 */
pid_t cfork (
    forkproc (*padd),	/* process start address */
    long pp,		/* possible parameter */
    int stacksize,	/* stacksize [0-3], or explicit number */ 
    char *name,		/* a human readable name */
    int ttynum)		/* terminal number for standard I/O */
{
    pid_t pid;

    /*
     * If requested stack size is zero (0), use the default size.
     * Scale request according to processor type.
     */
    if (stacksize == 0)
	stacksize = STACK_SIZE_DEFAULT;
    stacksize = wtob(stacksize) * STACK_SIZE_SCALE;

    /*
     * Create it.
     */
    pid = process_create_common((process_t*)padd, pp, stacksize, name,
				PRIO_NORMAL, FALSE);
    if (pid != NO_PROCESS)
	process_set_ttynum(pid, ttynum);
    return(pid);
}

/*
 * edisms
 *
 * Dismiss running process until event occurs.  This is present only
 * for backwards compatibility with the existing code base.
 */
void edisms (
    blockproc (*proc),
    ulong pp)	/* for backward compatibility */
{
    SAVE_CALLER();

    if (!process_ok_to_reschedule("event dismiss"))
	return;

    /*
     * Backwards compatibility.  Put the process onto an old style queue
     * so that the wakeup routine will actually be executed.
     */
    if (forkx->prefers_new) {
	if (forkx->priority <= PRIO_HIGH) {
	    (*kernel_errmsg)(&msgsym(EDISMSCRIT, SCHED), forkx->name);
	}
    }
    if (!forkx->on_old_queue)
	old_run_add(forkx);
    forkx->caller_pc = caller();   	/* record PC of caller */
    forkx->callee_pc = current_pc();	/* record current address */
    forkx->state = SCHEDSTATE_EDISMS;	/* event dismiss */
    forkx->edisms = proc;		/* copy scheduler test */
    forkx->eparm = (void *) pp;		/* copy parameter, if any */
    suspend();				/* switch to scheduler context */
}

/*
 * runtime
 * Return the runtime that the current process has used.
 */
ulong runtime (void)
{
    ulong runtime;

    if (process_get_info(PI_RUNTIME, THIS_PROCESS, &runtime))
	return(runtime);
    return(0L);
}
