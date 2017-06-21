/* $Id: exec_ncia.h,v 3.1.2.1 1996/06/23 05:52:57 schiang Exp $
 * $Source: /release/112/cvs/Xsys/cls/exec_ncia.h,v $
 *------------------------------------------------------------------
 * exec_ncia.h - Definitions for NCIA exec command
 *
 * June 1996, Steve Chiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_ncia.h,v $
 * Revision 3.1.2.1  1996/06/23  05:52:57  schiang
 * CSCdi61120:  split exec_show_ncia.h into exec_show_ncia.h and
 * exec_ncia.h
 * Branch: California_branch
 *
 * Revision 3.1  1996/06/23  05:35:04  schiang
 * CSCdi56188:  add placeholder for exec_ncia.h
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*******************************************************************
 *  ncia [start | stop] <server_num>
 */
EOLS    (exec_ncia_stop_eol, ncia_exec_command, EXEC_NCIA_STOP );

NUMBER  (set_stop_ncia_server_num, exec_ncia_stop_eol, exec_ncia_stop_eol,
        OBJ(int, 1), 1, 1, "NCIA Server Number");

KEYWORD (exec_ncia_stop, set_stop_ncia_server_num, no_alt,
        "stop", "NCIA Stop", PRIV_OPR );

EOLS    (exec_ncia_start_eol, ncia_exec_command, EXEC_NCIA_START );

NUMBER  (set_start_ncia_server_num, exec_ncia_start_eol, exec_ncia_start_eol,
        OBJ(int, 1), 1, 1, "NCIA Server Number");

KEYWORD (exec_ncia_start, set_start_ncia_server_num, exec_ncia_stop,
        "start", "NCIA Start", PRIV_OPR );

SET     (set_ncia_server_num, exec_ncia_start, OBJ(int, 1), 1);

/******************************************************************/
KEYWORD (exec_ncia, set_ncia_server_num, NONE,
	"ncia", "Start/Stop NCIA Server", PRIV_OPR );

#undef	ALTERNATE
#define	ALTERNATE	exec_ncia
