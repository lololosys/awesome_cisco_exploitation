/* $Id: exec_clear_controller_lex.h,v 3.2 1995/11/17 17:42:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lex/exec_clear_controller_lex.h,v $
 *------------------------------------------------------------------
 * exec_clear_controller_lex.h
 *
 * June 1994, David Coli
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_clear_controller_lex.h,v $
 * Revision 3.2  1995/11/17  17:42:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:54:29  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:17:32  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:15:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS    (clear_controller_lex_prom_eol, lex_reload_command, LEX_RELOAD_PROM);
EOLS    (clear_controller_lex_eol, lex_reload_command, LEX_RELOAD_DEFAULT);

KEYWORD (clear_controller_lex_prom, clear_controller_lex_prom_eol,
	 clear_controller_lex_eol, "prom", "reload PROM image ", PRIV_ROOT);

UNIT_NUMBER(clear_controller_lex_unit, clear_controller_lex_prom,
            no_alt, OBJ(idb,1), IFTYPE_LEX);

KEYWORD_ID(clear_controller_lex_kwd, clear_controller_lex_unit, NONE,
           OBJ(int,1), IDB_LEX, "Lex", "Lex controller", PRIV_USER);

TEST_FUNC(clear_controller_lex_test, clear_controller_lex_kwd, NONE, NONE,
         is_lex_configured);

KEYWORD (lex_clear_controller, clear_controller_lex_test, ALTERNATE,
         "controller", " ", PRIV_DUPLICATE | PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE       lex_clear_controller
