/* $Id: exec_test_interfaces.h,v 3.4.20.1 1996/08/12 16:01:11 widmer Exp $
 * $Source: /release/112/cvs/Xsys/dev/exec_test_interfaces.h,v $
 *------------------------------------------------------------------
 * Test interface command
 *
 * August 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_test_interfaces.h,v $
 * Revision 3.4.20.1  1996/08/12  16:01:11  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/01/29  07:08:31  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/18  02:12:24  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  09:00:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:52:41  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:27:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS    (priv_test_interfaces_eol, test_interfaces);
INTERFACE (test_interfaces_get_interface, priv_test_interfaces_eol,
           priv_test_interfaces_eol, OBJ(idb,1), IFTYPE_HWIDB);
KEYWORD (priv_test_interfaces, test_interfaces_get_interface, ALTERNATE,
         "interfaces", "Network interfaces", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_interfaces
