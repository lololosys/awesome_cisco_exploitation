/* $Id: exec_show_controllers_fddi.h,v 3.2.60.1 1996/08/12 16:02:32 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_show_controllers_fddi.h,v $
 *------------------------------------------------------------------
 * FDDI show controllers commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_fddi.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:32  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:27:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:24  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:23:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show controllers fddi [<unit number>]
 *
 */
EOLNS	(show_control_fddi_eol, show_controllers);
UNIT_NUMBER (show_control_fddi_num, show_control_fddi_eol,
	     show_control_fddi_eol,
	     OBJ(idb,1), IFTYPE_FDDI);
KEYWORD_ID (show_control_fddi, show_control_fddi_num, ALTERNATE,
	    OBJ(int,1), IDB_FDDI,
	    "fddi","FDDI interface internal state", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_control_fddi
