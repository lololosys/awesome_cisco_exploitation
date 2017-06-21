/* $Id: exception.h,v 3.2.60.1 1996/03/18 19:36:58 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/h/exception.h,v $
 *------------------------------------------------------------------
 * exception.h	-- prototypes to deal with exceptions
 *
 * August 7 1991, Tony Li
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exception.h,v $
 * Revision 3.2.60.1  1996/03/18  19:36:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/09  05:04:58  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.1  1996/02/20  14:17:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:12:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/18  06:45:51  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 20:34:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * exception.c
 */ 

extern watched_boolean *system_crashed;
extern addrtype dumphost;
extern addrtype sl_dumphost;
extern int exception_pid;
extern char *dumpfile;
extern char *sl_dumpfile;
extern boolean dumpfile_def;
extern boolean sl_dumpfile_def;
extern sys_timestamp crash_msclock;
extern ulong spurious_threshold;
extern void manual_write_core(parseinfo *);
extern void exception_write_core(void);
extern void init_exception_handler(void);
extern void restore_vectors(void);

/*
 * processor dependant routines
 */
extern void gdb_init_exceptions(void);
extern void gdb_crash_restore(void);
extern void pass_bus_errors(void);
extern void trap_bus_errors(void);
extern void init_clocktick_handler(void);
