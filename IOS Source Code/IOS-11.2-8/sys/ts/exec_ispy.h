/* $Id: exec_ispy.h,v 3.1.10.1 1996/08/12 16:08:30 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/exec_ispy.h,v $
 *------------------------------------------------------------------
 * exec_ispy.h - Command to initiate nasty sneaking spying
 *
 * Feb 1996, Andy Valencia
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_ispy.h,v $
 * Revision 3.1.10.1  1996/08/12  16:08:30  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/03/01  16:39:54  vandys
 * CSCdi50393:  No way to monitor TTY lines remotely
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ispy <line>
 */
EOLNS    (exec_ispy_eol, ispy_command);

TTYLINE	(exec_ispy_line, exec_ispy_eol, no_alt, OBJ(int,1),
    "TTY line on which to spy");

KEYWORD	(exec_ispy, exec_ispy_line, ALTERNATE, "ispy",
    "Spy on a TTY line", PRIV_ROOT);

#undef ALTERNATE
#define ALTERNATE exec_ispy
