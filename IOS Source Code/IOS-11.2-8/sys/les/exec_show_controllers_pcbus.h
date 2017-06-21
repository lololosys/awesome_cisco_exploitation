/* $Id: exec_show_controllers_pcbus.h,v 3.2.62.1 1996/08/12 16:04:35 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_show_controllers_pcbus.h,v $
 *------------------------------------------------------------------
 * PCbus show controllers commands
 *
 * April 1994, Johnny Chan   
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_pcbus.h,v $
 * Revision 3.2.62.1  1996/08/12  16:04:35  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:37:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:35  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:23:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/******************************************************************
 * show controllers pcbus [<unit>]
 *
 */
EOLNS	(show_control_pcbus_eol, show_controllers);
UNIT_NUMBER(show_control_pcbus_unit, show_control_pcbus_eol,
	    show_control_pcbus_eol,
	    OBJ(idb,1),  IFTYPE_PCBUS);
KEYWORD_ID(show_control_pcbus_kwd, show_control_pcbus_unit, NONE,
	   OBJ(int,1), IDB_PCBUS,
	   "pcbus", "Show pcbus controllers", PRIV_USER);
TEST_INT(show_control_pcbus, show_control_pcbus_kwd, NONE, ALTERNATE,
	 npcbusnets);

#undef	ALTERNATE
#define	ALTERNATE	show_control_pcbus
