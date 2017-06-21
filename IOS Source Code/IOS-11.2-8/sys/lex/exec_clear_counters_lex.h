/* $Id: exec_clear_counters_lex.h,v 3.3.20.1 1996/08/12 16:04:42 widmer Exp $
 * $Source: /release/112/cvs/Xsys/lex/exec_clear_counters_lex.h,v $
 *------------------------------------------------------------------
 * exec_clear_counters_lex.h
 *
 * June 1994, David Coli
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_clear_counters_lex.h,v $
 * Revision 3.3.20.1  1996/08/12  16:04:42  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:00  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  17:42:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:54:30  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:15:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/******************************************************************
 * clear counters lex n [ether | serial]
 *
 * OBJ(int,1) = TRUE if <interface-name> was given
 * csb->interface = idb of specified interface
 */
EOLNS    (clear_lex_ctrs_eol, clear_counters);
SET     (clear_lex_ctrs_set, clear_lex_ctrs_eol, OBJ(int,1), TRUE);

EOLNS    (clear_lex_ether_eol, lex_clear_ether_counters);
EOLNS    (clear_lex_serial_eol, lex_clear_serial_counters);
KEYWORD_ID(clear_counters_lex_serial, clear_lex_serial_eol,
           clear_lex_ctrs_set, OBJ(int,1), TRUE, "serial",
           "Clear counters for remote Lex serial interface", PRIV_USER);

KEYWORD_ID(clear_counters_lex_ether, clear_lex_ether_eol,
           clear_counters_lex_serial, OBJ(int,1), TRUE, "ethernet",
           "Clear counters for remote Lex ethernet interface", PRIV_USER);

INTERFACE (clear_counters_lex_test, clear_counters_lex_ether, no_alt,
           OBJ(idb,1), IFTYPE_LEX);

KEYWORD (exec_clear_counters_lex, clear_counters_lex_test, ALTERNATE,
         "counters", NULL, PRIV_DUPLICATE | PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE       exec_clear_counters_lex
