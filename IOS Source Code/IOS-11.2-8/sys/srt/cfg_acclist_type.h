/* $Id: cfg_acclist_type.h,v 3.3.56.1 1996/05/30 23:45:34 hampton Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_acclist_type.h,v $
 *------------------------------------------------------------------
 * Type code access list commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_acclist_type.h,v $
 * Revision 3.3.56.1  1996/05/30  23:45:34  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/27  21:25:04  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  18:52:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:53  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:01:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*****************************************************************
 * Type code access list
 *
 * access-list <200-299> { permit | deny } <typecode> [<mask>]
 *
 * OBJ(int,1) = access list number
 * OBJ(int,2) = TRUE for permit; FALSE for deny
 * OBJ(int,3) = typecode
 * OBJ(int,4) = mask
 */

EOLS	(altc_eol, typeaccess_command, LIST_TYPE);

GENERAL_NUMBER(altc_mask, altc_eol, altc_eol,
	       OBJ(int,4), 0x0, 0xffff,
	       "Protocol type-code mask",
	       (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_WS_OK | NUMBER_HELP_CHECK));
GENERAL_NUMBER(altc_tc, altc_mask, no_alt,
	       OBJ(int,3), 0x0, 0xffff,
	       "Protocol type-code",
	       (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_WS_OK | NUMBER_HELP_CHECK));

PERMIT_DENY(altc_permit_deny, altc_tc, no_alt, OBJ(int,2), PRIV_CONF);

NOPREFIX(altc_noprefix, altc_permit_deny, altc_eol);

EVAL	(altc_modify, altc_noprefix,
	 GETOBJ(int,1) = GETOBJ(int,1) - TYPECODE_MIN_ACL + MINTYPEACC);
NUMBER	(altc_num, altc_modify, NONE,
	 OBJ(int,1), TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
	 "Protocol type-code access list");
NVGENS	(alist_typecode_nvgen, altc_num, typeaccess_command, LIST_TYPE);
NOP	(access_list_typecode, alist_typecode_nvgen, ALTERNATE);

#undef	ALTERNATE
#define	ALTERNATE	access_list_typecode

/*
 * Old style "[no] access-list ..."
 *
 * These remaining lines should be deleted in a future release.
 */
NUMBER	(oaltc_num, altc_noprefix, NONE,
	 OBJ(int,1), MINTYPEACC, MAXTYPEACC, "Protocol type-code access list");
NVGENS	(oalist_typecode_nvgen, oaltc_num, typeaccess_command, LIST_TYPE);
NOP	(oaccess_list_typecode, oalist_typecode_nvgen, OALTERNATE);

#undef	OALTERNATE
#define	OALTERNATE	oaccess_list_typecode
