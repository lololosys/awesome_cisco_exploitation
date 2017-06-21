/* $Id: cfg_int_lex.h,v 3.2.60.2 1996/08/12 16:04:41 widmer Exp $
 * $Source: /release/112/cvs/Xsys/lex/cfg_int_lex.h,v $
 *------------------------------------------------------------------
 * CFG_INT_LEX.H
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_lex.h,v $
 * Revision 3.2.60.2  1996/08/12  16:04:41  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.1  1996/05/30  23:44:45  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:42:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:54:28  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:16:50  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:06:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CFG_INT_LEX_H__
#define __CFG_INT_LEX_H__

/*
 * Because of the nature of the parser a few simple rules MUST be followed
 * when adding new commands to this file.  Always add the new command to
 * the top of this file.  Always check that the alternate field of the
 * last command points to the new command.  Always terminate the chain
 * by setting the alternate field on the new command to no_alt. Always
 * provide a comment describing the new syntax and any other relevant
 * details.
 */

/**********************************************************************
 *      lex burned-in-address <ieee-address>
 *   no lex burned-in-address [<ieee-address>]
 */
EOLNS    (lex_macaddr_eol, lex_macaddr_command);
ENADDR(lex_macaddr_addr, lex_macaddr_eol, no_alt, OBJ(hwaddr,1), "MAC address");
NOPREFIX(lex_macaddr_noprefix, lex_macaddr_addr, lex_macaddr_eol );
NVGENNS  (lex_macaddr_nvgen, lex_macaddr_noprefix, lex_macaddr_command);
KEYWORD (lex_macaddr, lex_macaddr_nvgen, NONE, "burned-in-address",
         "Manually set interface burned-in MAC address", PRIV_CONF);

/**********************************************************************
 *      lex timeout <milliseconds>
 *   no lex timeout [<milliseconds>]
 *
 * OBJ(int,1) = <milliseconds>
 */

PARAMS_KEYONLY (lex_timeout, lex_macaddr,
		"timeout", OBJ(int,1), LEX_MIN_TIMEOUT,
		LEX_MAX_TIMEOUT, lex_config_command, LEX_TIMEOUT,
		"Timeout length for response from LEX device",
		"Milliseconds", PRIV_CONF);


/**********************************************************************
 *      lex retry-count <count>
 *   no lex retry-count [<count>]
 *
 * OBJ(int,1) = <count>
 */

PARAMS_KEYONLY (lex_retry, lex_timeout,
		"retry-count", OBJ(int,1), LEX_MIN_RETRY_COUNT,
		LEX_MAX_RETRY_COUNT, lex_config_command, LEX_RETRY,
		"Number of times to re-send command to the LEX device",
		"Retry count", PRIV_CONF);


/**********************************************************************
 *      lex priority-group <1-10>
 */
PARAMS_KEYONLY (lex_priority, lex_retry,
		"priority-group", OBJ(int,1), 1, PRIORITY_MAXLIST-1,
		lex_config_command, LEX_PRIORITY,
		"Assign a priority group to a remote LEX interface",
		"Priority group", PRIV_CONF);


/**********************************************************************
 *      lex input-type-list <list>
 *
 * OBJ(int,1) = <list>
 */

PARAMS_ALIST   (lex_protofilter, lex_priority,
		"input-type-list", OBJ(int,1), MINTYPEACC, MAXTYPEACC,
		TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
		lex_config_command, LEX_INTYPE,
		"Filter Ethernet packets by type code at the remote LEX device",
		"Ethernet type access list", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *      lex input-address-list <list>
 *
 * OBJ(int,1) = <list>
 */

PARAMS_ALIST   (lex_addrfilter, lex_protofilter,
		"input-address-list", OBJ(int,1), MINMACADDRACC, MAXMACADDRACC,
		MACADDR_MIN_STD_ACL, MACADDR_MAX_STD_ACL,
		lex_config_command, LEX_INADDRESS,
		"Filter packets by source address at the LEX device",
		"Ethernet address access list", PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_lex, lex_addrfilter, NONE,
	"lex", "Configure lex device", PRIV_CONF);

TEST_IDBSTATUS(lex_int_test, ci_lex, NONE, ALTERNATE, IDB_LEX);

#undef  ALTERNATE
#define ALTERNATE lex_int_test

#endif
