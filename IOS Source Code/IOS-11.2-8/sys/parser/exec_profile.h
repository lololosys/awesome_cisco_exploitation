/* $Id: exec_profile.h,v 3.2.62.2 1996/08/12 16:06:12 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_profile.h,v $
 *------------------------------------------------------------------
 * E X E C _ P R O F I L E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_profile.h,v $
 * Revision 3.2.62.2  1996/08/12  16:06:12  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/08/06  01:41:43  dkatz
 * CSCdi65170:  Task profiling is broken
 * Branch: California_branch
 * Untangle parser chains.
 *
 * Revision 3.2  1995/11/17  18:46:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/29  04:02:26  pst
 * CSCdi36559:  clean up hidden command help in parser
 *
 * Revision 2.1  1995/06/07  22:21:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * profile task [interrupt] [PID1] [PID2] ... [PID10]
 * unprofile task
 *
 * OBJ(int,1) = TRUE if "interrupt" specified
 * OBJ(int,2) = number of PIDs specified (max 10)
 * OBJ(int,3) = TRUE if all PIDs requested
 * csb->uiarray[] = list of PIDs
 * OBJ(int,5) = TRUE if "profile task", FALSE if "unprofile task"
 *
 * OBJ(int,4) is used as a scratch variable.
 */
pdecl   (profile_task_parms);
EOLNS    (profile_task_eol, profile_task_command);
EVAL    (profile_task_addpid, profile_task_parms,
	 csb->uiarray[GETOBJ(int,2) ++] = GETOBJ(int,4));
NUMBER  (profile_task_pid, profile_task_addpid, profile_task_eol,
	 OBJ(int,4), 0, MAXUINT, NULL);
SPLIT   (profile_task_test, profile_task_eol, profile_task_pid,
	 OBJ(int,2), UIARRAY_SIZE);
KEYWORD_ID (profile_task_all, profile_task_parms, profile_task_test,
	    OBJ(int,3), TRUE, "all", NULL, PRIV_OPR);
KEYWORD_ID (profile_task_int, profile_task_parms, profile_task_all,
	    OBJ(int,1), TRUE, "interrupt", NULL, PRIV_OPR);
NOP     (profile_task_parms, profile_task_int, NONE);

/******************************************************************
 * profile <start_addr> <end_addr> <increment>
 * unprofile all | <start_addr> <end_addr> <increment>
 * profile start | stop | hogs
 *
 * OBJ(int,1) = TRUE if "all", FALSE if addresses present
 * OBJ(int,2) = start address
 * OBJ(int,3) = end address
 * OBJ(int,4) = increment
 * OBJ(int,5) = TRUE if "profile", FALSE if "unprofile"
 * csb->which = PROFILE_EXEC or PROFILE_START or PROFILE_STOP or PROFILE_HOG
 */
EOLS    (profile_eol, profile_command, PROFILE_EXEC);
HEXNUM  (profile_increment, profile_eol, no_alt, OBJ(int,4), NULL);
HEXNUM  (profile_end, profile_increment, no_alt, OBJ(int,3), NULL);
HEXNUM  (profile_startpc, profile_end, no_alt, OBJ(int,2), NULL);

EOLS    (profile_hog_eol, profile_command, PROFILE_HOG);
KEYWORD (profile_hog, profile_hog_eol, profile_startpc, "hogs", NULL,
	 PRIV_OPR);

EOLS    (profile_stop_eol, profile_command, PROFILE_STOP);
KEYWORD (profile_stop, profile_stop_eol, profile_hog, "stop", NULL,
	 PRIV_OPR);

EOLS    (profile_start_eol, profile_command, PROFILE_START);
KEYWORD (profile_start, profile_start_eol, profile_stop, "start", NULL,
	 PRIV_OPR);
KEYWORD (profile_task, profile_task_parms, profile_start, "task", NULL,
	 PRIV_OPR);

KEYWORD_ID (unprofile_all, profile_eol, profile_start, OBJ(int,1), TRUE, "all",
	    NULL, PRIV_OPR|PRIV_NOHELP);
KEYWORD (unprofile_task, profile_task_eol, unprofile_all, "task", NULL,
	 PRIV_OPR|PRIV_NOHELP);
KEYWORD (unprofile, unprofile_task, ALTERNATE, "unprofile", NULL, 
	 PRIV_OPR|PRIV_NOHELP);
KEYWORD_ID (profile, profile_task, unprofile, OBJ(int,5), TRUE, "profile", 
	    NULL, PRIV_OPR|PRIV_NOHELP);

#undef	ALTERNATE
#define	ALTERNATE	profile
