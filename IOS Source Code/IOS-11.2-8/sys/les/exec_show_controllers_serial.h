/* $Id: exec_show_controllers_serial.h,v 3.2.60.1 1996/08/12 16:04:35 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_show_controllers_serial.h,v $
 *------------------------------------------------------------------
 * Serial show controllers commands
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_serial.h,v $
 * Revision 3.2.60.1  1996/08/12  16:04:35  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:37:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:36  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:23:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show controllers serial [<unit>]
 *
 */
EOLNS	(show_control_serial_eol, show_controllers);
UNIT_NUMBER(show_control_serial_unit, show_control_serial_eol,
	    show_control_serial_eol,
	    OBJ(idb,1),  IFTYPE_SERIAL);
KEYWORD_ID(show_control_serial_kwd, show_control_serial_unit, NONE,
	   OBJ(int,1), IDB_SERIAL,
	   "serial", "Show serial controllers", PRIV_USER);
TEST_INT(show_control_serial, show_control_serial_kwd, NONE, ALTERNATE,
	 nserialnets);

#undef	ALTERNATE
#define	ALTERNATE	show_control_serial
