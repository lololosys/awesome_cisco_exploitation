/* $Id: rtt_scheduler.c,v 1.1.4.2 1996/06/07 22:26:22 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_scheduler.c,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor Scheduler.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the RTT Monitor Scheduler.
 *------------------------------------------------------------------
 * $Log: rtt_scheduler.c,v $
 * Revision 1.1.4.2  1996/06/07  22:26:22  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:36  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:50  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:51  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:46  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:33  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "rtt_lib.h"
#include "rtt_scheduler.h"

static boolean rttMonSchedulerIsRunning = FALSE;
static watched_boolean *rttSchedulerInitWatchedBoolean;

static void rtt_scheduler (void)
{
    boolean die;

    rttMonSchedulerInit();

    process_set_boolean(rttSchedulerInitWatchedBoolean,
			TRUE);

    die = FALSE;
    while (!die) {
	process_wait_for_event();
	die = rttMonSchedulerUpdate();
    } 
    rttMonSchedulerCleanup();

    rttMonSchedulerIsRunning = FALSE;

    setRttMonSchedulerPid(NO_PROCESS);

    process_kill(THIS_PROCESS);

    return;
} 

void start_rtt_scheduler (void)
{
    pid_t pid;

    if (rttMonSchedulerIsRunning) {
	return;
    }
    rttSchedulerInitWatchedBoolean =
	create_watched_boolean("rttSchedulerInitWatchedBoolean",
			       0);
    process_set_boolean(rttSchedulerInitWatchedBoolean,
			FALSE);

    pid = process_create(&rtt_scheduler,
			 "RTR Scheduler",
			 SMALL_STACK,
			 PRIO_NORMAL);

    setRttMonSchedulerPid(pid);

    if (pid != NO_PROCESS) {
	rttMonSchedulerIsRunning = TRUE;
    }
    /* 
     * Don't return until the 
     *  scheduler is fully initialized.
     */ 
    process_watch_boolean(rttSchedulerInitWatchedBoolean,
			  ENABLE, ONE_SHOT);
    process_wait_for_event();
    delete_watched_boolean(&rttSchedulerInitWatchedBoolean);

    return;
}
