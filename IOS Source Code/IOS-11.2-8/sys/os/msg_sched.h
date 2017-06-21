/* $Id: msg_sched.h,v 3.4.20.4 1996/08/28 13:03:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/msg_sched.h,v $
 *------------------------------------------------------------------
 * msg_sched.h -- header for scheduler message definitions.
 *
 * October, 1995, David S.A. Stine
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_sched.h,v $
 * Revision 3.4.20.4  1996/08/28  13:03:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.20.3  1996/08/03  23:34:46  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.4.20.2  1996/05/08  00:06:38  hampton
 * Add better semaphore errors and better code to wait for a semaphore.
 * The routine process_wait_for_event_timed() shouldn't post a TIMER_EVENT
 * when the temporary timer expires.  [CSCdi56999]
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/03/27  21:51:57  hampton
 * Try to catch and report thrashing processes.  A few other minor
 * scheduler cleanups and enhancements.  [CSCdi50130]
 * Branch: California_branch
 *
 * Revision 3.4  1996/01/25  23:52:26  hampton
 * Add support to the scheduler for nesting wakeup event lists.  This
 * should make it easier to convert portion of the code.  Also allow for
 * the creation of a detached list of watched events.  [CSCdi46566]
 *
 * Revision 3.3  1995/11/18  06:06:13  hampton
 * Don't let the entire router be dragged down by a single process with a
 * busted timer.  [CSCdi44073]
 *
 * Revision 3.2  1995/11/17  00:18:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:47:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/18  08:18:02  dstine
 * Placeholders for message headers.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
