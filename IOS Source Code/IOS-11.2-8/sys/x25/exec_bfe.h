/* $Id: exec_bfe.h,v 3.3.28.1 1996/08/12 16:09:44 widmer Exp $
 * $Source: /release/112/cvs/Xsys/x25/exec_bfe.h,v $
 *------------------------------------------------------------------
 * E X E C _ B F E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_bfe.h,v $
 * Revision 3.3.28.1  1996/08/12  16:09:44  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/18  02:16:09  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  18:06:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:05  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:15:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * bfe { enter | leave } <interface>
 */

EOLNS	(exec_bfe_eol, bfe_command);
INTERFACE(exec_bfe_interface, exec_bfe_eol, no_alt,
	  OBJ(idb,1), IFTYPE_HWIDB);

KEYWORD_ID(exec_bfe_leave, exec_bfe_interface, no_alt,
	   OBJ(int,1), PARSER_BFE_LEAVE,
	   "leave", "Leave Emergency mode", PRIV_OPR);
KEYWORD_ID(exec_bfe_enter, exec_bfe_interface, exec_bfe_leave,
	   OBJ(int,1), PARSER_BFE_ENTER,
	   "enter", "Enter Emergency mode", PRIV_OPR);

KEYWORD	(exec_bfe, exec_bfe_enter, NONE,
	 "bfe", "For manual emergency modes setting", PRIV_OPR);

TEST_BOOLEAN(exec_bfe_test, exec_bfe, NONE, ALTERNATE, x25_enable);

#undef	ALTERNATE
#define	ALTERNATE	exec_bfe_test
