/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_tmq.c,v 3.9.8.1 1996/08/28 13:10:06 thille Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_tmq.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_tmq.c,v $
 * Revision 3.9.8.1  1996/08/28  13:10:06  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9  1996/02/28  09:33:40  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.8  1996/02/12  06:58:19  jjohnson
 * CSCdi47099:  not nearly enough processes call process_set_crashblock
 * There isn't much demand for network management during a meltdown
 *
 * Revision 3.7  1996/02/11  00:43:01  jjohnson
 * CSCdi48628:  SNMP Timer process should be created on-demand
 *
 * Revision 3.6  1996/01/18  16:38:00  jjohnson
 * CSCdi45754:  snmp subsystem should not contain non-snmp mibs
 * move the rs232 mib, image mib, and terminal services mib into separate
 * subsystems.  move the cisco interfaces mib into the interfaces mib
 * subsystem.  consolidate the snmp timer subsystem into the snmp
 * subsystem since it really is core snmp technology.
 *
 * Revision 3.5  1996/01/18  05:34:24  jjohnson
 * CSCdi47100:  snmp timer debug messages are not timestamped
 * The initial \n must be part of the format string
 *
 * Revision 3.4  1996/01/17  19:46:15  jjohnson
 * CSCdi47027:  Need mechanism for debugging snmp timers
 *
 * Revision 3.3  1995/11/17  19:01:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:28:33  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:18:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/03  17:32:40  jjohnson
 * Initial placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "sr_snmpd.h"
#include "sr_tmq.h"
#include "subsys.h"
#include "mgd_timers.h"
#include "../ui/debug.h"
#include "snmp_debug.h"

typedef struct _TimerEvent {
    unsigned long   period;
    mgd_timer	    timer;
    TimeOutDescriptor *info;
}               TimerEvent;


static mgd_timer snmp_timer;
static pid_t snmp_timer_pid;

/* forward reference */
static process snmp_tmq (void);

int
SetPeriodicTimeout (
    unsigned long   when,
    unsigned long   period,
    TimeOutDescriptor *info)
{
    TimerEvent *tep;
    int delta_t;

    /*
     * create the timer process if necessary
     */
    if (snmp_timer_pid == NO_PROCESS) {

	snmp_timer_pid = process_create(snmp_tmq, "SNMP Timers",
					NORMAL_STACK, PRIO_NORMAL);

	if (snmp_timer_debug)
	    if (snmp_timer_pid != NO_PROCESS)
		buginf("\nSNMP: Timer process created (pid %d)",
		       snmp_timer_pid);

	/*
	 * if the process creation failed, we're hosed
	 */
	if (snmp_timer_pid == NO_PROCESS)
	    return -1;

	process_set_crashblock(snmp_timer_pid, TRUE);

    }

    tep = malloc(sizeof(TimerEvent));
    if (tep == NULL) {
	if (snmp_timer_debug)
	    buginf("\nSNMP: Timer creation failed");
	return -1;
    }

    /*
     * initialize the managed timer
     */
    mgd_timer_init_leaf(&tep->timer, &snmp_timer, 0, tep, FALSE);

    /*
     * calculate the delta time between "now" and when the timer
     * should fire (in centiseconds)
     */
    delta_t = (int) when - snmp_sysUpTime();
    if (delta_t < 0)
	delta_t = 0;

    tep->period = period * 10;
    tep->info = info;
    mgd_timer_start(&tep->timer, delta_t * 10);

    if (snmp_timer_debug)
	buginf("\nSNMP: Created timer %0x, delay = %d, period = %d",
	       tep, delta_t * 10, tep->period);

    return (int)tep;
}


int
SetOneshotTimeout (unsigned long when, TimeOutDescriptor *info)
{
    return (SetPeriodicTimeout(when, 0, info));
}

int
CancelTimeout (int id)
{
    TimerEvent     *tep;

    tep = (TimerEvent *)id;
    mgd_timer_stop(&tep->timer);
    if (snmp_timer_debug)
	buginf("\nSNMP: Cancelled timer %0x", tep);
    free(tep);
    return TRUE;
}

int
TimerFreeInfo(
    long            id,
    void            (*tifree) (void *p))
{
    TimerEvent     *tep;

    tep = (TimerEvent *)id;
    (*tifree) (tep->info);
    return TRUE;
}


static process
snmp_tmq (void)
{
    mgd_timer		*expired_timer;
    ulong		major, minor;
    TimerEvent		*tep;
    TimeOutDescriptor	*tdp;

    process_watch_mgd_timer(&snmp_timer, ENABLE);
    for (;;) {

	if (snmp_timer_debug)
	    buginf("\nSNMP: Timer process sleeping");

        process_wait_for_event();

	if (snmp_timer_debug)
	    buginf("\nSNMP: Timer process awake");

        while (process_get_wakeup(&major, &minor)) {

	    if (snmp_timer_debug)
		buginf("\nSNMP: Timer processing event");

            switch (major) {
	    case TIMER_EVENT:
                while (mgd_timer_expired(&snmp_timer)) {
                    expired_timer = mgd_timer_first_expired(&snmp_timer);
                    tep = mgd_timer_context(expired_timer);

		    if (snmp_timer_debug)
			buginf("\nSNMP: Timer %0x fired, calling %0x",
			       tep, tep->info->CallBack);

		    /* make sure we have exclusive access to snmp */
		    process_lock_semaphore(snmp_semaphore, MAXULONG);
		    process_get_pid(&snmp_semaphore_pid);

		    /* invoke the callback routine */
		    tdp = tep->info;
		    (*tdp->CallBack) (tdp);

		    /* relinquish snmp lock */
		    process_unlock_semaphore(snmp_semaphore);
		    snmp_semaphore_pid = NO_PROCESS;

		    /* see if this is a periodic or one-shot timer */
		    if (tep->period == 0) {
			/* one-shot so free the TimerEvent block */
			mgd_timer_stop(&tep->timer);

			if (snmp_timer_debug)
			    buginf("\nSNMP: Timer %0x stopped",
				   tep);

			free(tep);
		    } else {
			/* periodic timer -- restart it */
			mgd_timer_start(&tep->timer, tep->period);

			if (snmp_timer_debug)
			    buginf("\nSNMP: Timer %0x rearmed, delay = %d",
				   tep, tep->period);
		    }
		}
		break;
	    default:
                errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
                break;
            }
            process_may_suspend();
	}
    }
}

void
init_snmp_timers (void)
{
    snmp_timer_pid = NO_PROCESS;
    mgd_timer_init_parent(&snmp_timer, NULL);
}
