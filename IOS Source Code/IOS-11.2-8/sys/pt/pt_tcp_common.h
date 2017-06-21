/* $Id: pt_tcp_common.h,v 3.3 1996/01/29 11:38:42 billw Exp $
 * $Source: /release/111/cvs/Xsys/pt/pt_tcp_common.h,v $
 *------------------------------------------------------------------
 * pt_tcp_common.h  Data definitions for pt_tcp_.. to lat/pad helper
 * subroutines.
 *
 * Aug. 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pt_tcp_common.h,v $
 * Revision 3.3  1996/01/29  11:38:42  billw
 * CSCdi46038:  vty access-class takes precedence over translate
 * access-class
 * Always use SYNC_LISTEN for incoming PT tcp sessions, and let the PT
 * process itself do the access check.
 *
 * Revision 3.2  1995/11/17  19:00:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:35:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

void pt_tcp_check_options(translatetype *trans, tt_soc *tty);
void pt_tcp_check_access(translatetype *trans, tt_soc *tty);
void pt_setup_tcp_session(translatetype *trans, tt_soc *vty);
