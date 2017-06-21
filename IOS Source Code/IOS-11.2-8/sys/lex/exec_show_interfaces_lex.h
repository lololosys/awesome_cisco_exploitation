/* $Id: exec_show_interfaces_lex.h,v 3.3.20.2 1996/08/12 16:04:44 widmer Exp $
 * $Source: /release/112/cvs/Xsys/lex/exec_show_interfaces_lex.h,v $
 *------------------------------------------------------------------
 * exec_show_interfaces_lex.h
 *
 * June 1994, David Coli
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_show_interfaces_lex.h,v $
 * Revision 3.3.20.2  1996/08/12  16:04:44  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.1  1996/03/18  20:43:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/13  01:50:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.24.1  1996/02/20  01:09:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/01/29  07:09:01  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  17:42:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:54:33  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:24:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*************************************************************************
 * show interfaces lex [<num>] [accounting] [stats] [ethernet|serial]
 *
 * OBJ(idb,1) = <interface>
 */

EOLNS    (normal_show_interfaces_eol, show_interface);
EOLNS    (lex_show_interfaces_eol, lex_show_interface);
SET     (lex_show_interface_set, normal_show_interfaces_eol, OBJ(int,1), TRUE);

KEYWORD_ID(show_interface_remote_serial, lex_show_interfaces_eol,
           lex_show_interface_set, OBJ(int,8), TRUE, "serial",
           "Show serial interface statistics for remote Lex device",
           PRIV_USER);

KEYWORD_ID(show_interface_remote_ether, lex_show_interfaces_eol,
           show_interface_remote_serial, OBJ(int,7), TRUE, "ethernet",
           "Show ethernet interface statistics for remote Lex device",
           PRIV_USER);

KEYWORD_ID(lex_show_interface_accounting, normal_show_interfaces_eol, no_alt,
           OBJ(int,10), TRUE,
           "accounting", "Show interface accounting", PRIV_USER );

ASSERT    (lex_show_interface_accounting_test, lex_show_interface_accounting,
           show_interface_remote_ether,
           (GETOBJ(idb,1) == NULL) || !is_subinterface(GETOBJ(idb,1)));

KEYWORD_ID(lex_show_interface_stats, normal_show_interfaces_eol,
           lex_show_interface_accounting_test, OBJ(int,9), TRUE,
           "stats", NULL /*"Show interface statistical data"*/,
	   PRIV_USER | PRIV_HIDDEN);

INTERFACE (lex_show_interface_get_interface, lex_show_interface_stats,
           no_alt, OBJ(idb,1), IFTYPE_LEX);

KEYWORD (lex_show_interfaces, lex_show_interface_get_interface, ALTERNATE,
         "interfaces", NULL, PRIV_DUPLICATE | PRIV_USER | PRIV_USER_HIDDEN);


#undef  ALTERNATE
#define ALTERNATE       lex_show_interfaces
