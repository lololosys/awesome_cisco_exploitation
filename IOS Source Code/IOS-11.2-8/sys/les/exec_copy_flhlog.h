/* $Id: exec_copy_flhlog.h,v 3.3.28.1 1996/08/12 16:04:30 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_copy_flhlog.h,v $
 *------------------------------------------------------------------
 * exec_copy_flhlog.h
 *
 * April 1995, Suresh Sangiah
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_copy_flhlog.h,v $
 * Revision 3.3.28.1  1996/08/12  16:04:30  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/22  06:37:23  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  17:36:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:32  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/21  18:17:35  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:17:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __EXEC_COPY_FLHLOG_H__
#define __EXEC_COPY_FLHLOG_H__

/*************************************************************************
 * copy flh-log tftp
 *
 * Exec command to copy FLH log files to a TFTP server.
 * 
 *************************************************************************/

/* copy flh-log tftp command */
EOLNS    (exec_copy_flhlog_tftp_eol, copy_flhlog_tftp_wrapper);

KEYWORD (exec_copy_flhlog_tftp, exec_copy_flhlog_tftp_eol, no_alt,
         "tftp", "Copy FLH log to TFTP server", PRIV_OPR);

/* copy flh-log ... */
KEYWORD_ID (exec_copy_flhlog, exec_copy_flhlog_tftp, ALTERNATE,
            OBJ(int,1), COPY_FLHLOG,
            "flh-log", "Copy FLH log file to server", PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE       exec_copy_flhlog

#endif  /* __EXEC_COPY_FLHLOG_H__ */
