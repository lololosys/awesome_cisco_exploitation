/* $Id: exec_show_controllers_ether.h,v 3.2.62.1 1996/08/12 16:04:33 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_show_controllers_ether.h,v $
 *------------------------------------------------------------------
 * Ethernet show controllers commands
 *
 * Novemger 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_ether.h,v $
 * Revision 3.2.62.1  1996/08/12  16:04:33  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:37:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:34  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:23:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show controllers ethernet [<unit>]
 *
 */
EOLNS	(show_control_ether_eol, show_controllers);
UNIT_NUMBER(show_control_ether_unit, show_control_ether_eol,
	    show_control_ether_eol,
	    OBJ(idb,1), IFTYPE_ETHER);
KEYWORD_ID(show_control_ether, show_control_ether_unit, ALTERNATE,
	   OBJ(int,1), IDB_ETHER,
	   "ethernet", "Show ethernet controllers", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_control_ether
