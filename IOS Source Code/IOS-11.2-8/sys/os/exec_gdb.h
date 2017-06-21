/* $Id: exec_gdb.h,v 3.2 1995/11/17 18:46:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/exec_gdb.h,v $
 *------------------------------------------------------------------
 * E X E C _ G D B . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_gdb.h,v $
 * Revision 3.2  1995/11/17  18:46:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:07  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:20:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************
 * gdb kernel
 */
EOLS	(exec_gdb_kernel_eol, gdb_command, GDB_KERNEL);
KEYWORD	(exec_gdb_kernel, exec_gdb_kernel_eol, no_alt,
	"kernel", "", PRIV_OPR | PRIV_HIDDEN);

/****************************************************************
 * gdb examine [<pid>]
 *
 * OBJ(int,1) = <pid>
 * <pid> = -1 if no pid was specified.
 */

EOLS	(exec_gdb_examine_eol, gdb_command, GDB_EXAMINE);
NUMBER (exec_gdb_examine_pid, exec_gdb_examine_eol, exec_gdb_examine_eol,
	OBJ(int,1), 0, -2, "Process ID to examine");
KEYWORD_ID (exec_gdb_examine, exec_gdb_examine_pid, exec_gdb_kernel,
	OBJ(int,1), -1,
	"examine", "", PRIV_OPR | PRIV_HIDDEN);

/****************************************************************
 * gdb debug <pid>
 *
 * OBJ(int,1) = <pid>
 */

EOLS	(exec_gdb_debug_eol, gdb_command, GDB_DEBUG);
NUMBER (exec_gdb_debug_pid, exec_gdb_debug_eol, no_alt,
	OBJ(int,1), 0, -2, "Process ID to debug");
KEYWORD	(exec_gdb_debug, exec_gdb_debug_pid, exec_gdb_examine,
	"debug", "", PRIV_OPR | PRIV_HIDDEN);

KEYWORD	(exec_gdb, exec_gdb_debug, ALTERNATE,
	 "gdb", "", PRIV_OPR | PRIV_HIDDEN);

#undef  ALTERNATE
#define ALTERNATE	exec_gdb
