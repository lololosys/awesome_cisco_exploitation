/* $Id: parser_defs_scheduler.h,v 3.1 1996/02/01 04:59:04 hampton Exp $
 * $Source: /release/111/cvs/Xsys/os/parser_defs_scheduler.h,v $
 *------------------------------------------------------------------
 * Defaults for scheduler commands
 *
 * April 1994, Dave Katz
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: parser_defs_scheduler.h,v $
 * Revision 3.1  1996/02/01  04:59:04  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/25  18:38:19  hampton
 * Fix per-process heap checking code.  Also convert it from conditional
 * compilation to an "internal" command.  [CSCdi47517]
 *
 * Revision 3.2  1995/11/17  18:52:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Default for scheduler-max-task-time command
 */
#define SCHED_MAX_TASK_TIME_DEFAULT (2*ONESEC)

enum sched_watchdog_options {
    SCHED_WATCHDOG_NORMAL,
    SCHED_WATCHDOG_RELOAD,
    SCHED_WATCHDOG_HANG,
    SCHED_WATCHDOG_TERMINATE,
};

enum sched_heaps_command {
    SCHED_HEAP_POLL,
    SCHED_HEAP_PROC,
};

