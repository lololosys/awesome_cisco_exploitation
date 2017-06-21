/* $Id: exec_show_processes.h,v 3.2.62.1 1996/08/12 16:05:14 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_processes.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ P R O C E S S E S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_processes.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:14  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:46:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:11  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:25:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show processes [memory | buffers | cpu | events | all-events]
 *
 */

EOLNS	(show_processes_eol, show_processes);
NOP	(show_processes_buf, NONE, show_processes_eol);

EOLNS    (show_processes_cpu_eol, show_proc_cpu);
KEYWORD (show_processes_cpu, show_processes_cpu_eol, show_processes_buf,
	 "cpu", "Show CPU use per process", PRIV_USER);

EOLNS	(show_processes_memory_eol, show_proc_memory);
KEYWORD	(show_processes_memory, show_processes_memory_eol, show_processes_cpu,
	 "memory", "Show memory use per process", PRIV_USER);

EOLNS	(show_processes_events_eol, show_proc_events);
NUMBER  (show_processes_events_num, show_processes_events_eol,
	 show_processes_events_eol,
	 OBJ(int,1), 0, MAXUINT, "Process Number");
KEYWORD	(show_processes_events, show_processes_events_num, show_processes_memory,
	 "events", "Show events for which processes want notification",
	 PRIV_OPR | PRIV_INTERNAL);

EOLNS	(show_processes_allevents_eol, show_proc_allevents);
KEYWORD	(show_processes_allevents, show_processes_allevents_eol,
	 show_processes_events,
	 "all-events", "Show all notifications", PRIV_OPR | PRIV_INTERNAL);

KEYWORD (show_processes_kwd, show_processes_allevents, ALTERNATE,
	 "processes", "Active process statistics", PRIV_USER|PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_processes_kwd
