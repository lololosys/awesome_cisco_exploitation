/* $Id: cfg_acclist_ether.h,v 3.3.56.1 1996/05/30 23:45:32 hampton Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_acclist_ether.h,v $
 *------------------------------------------------------------------
 * Ethernet access lists commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_acclist_ether.h,v $
 * Revision 3.3.56.1  1996/05/30  23:45:32  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/27  21:25:03  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  18:52:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:53  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:00:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*****************************************************************
 * Ethernet access list
 *
 * [no] access-list <700-799> { permit | deny } <address> [<mask>]
 *
 * OBJ(int,1) = access list number
 * OBJ(int,2) = TRUE for permit; FALSE for deny
 * OBJ(hwaddr,1) = address
 * OBJ(hwaddr,2) = mask
 */

EOLS	(aleth_eol, addressaccess_command, LIST_MAC);

ENADDR	(aleth_mask, aleth_eol, aleth_eol,
	 OBJ(hwaddr,2), "48-bit hardware address mask");
ENADDR	(aleth_addr, aleth_mask, no_alt,
	 OBJ(hwaddr,1), "48-bit hardware address");

PERMIT_DENY(aleth_permit_deny, aleth_addr, no_alt, OBJ(int,2), PRIV_CONF);

NOPREFIX(aleth_noprefix, aleth_permit_deny, aleth_eol);

EVAL	(aleth_modify, aleth_noprefix,
	 GETOBJ(int,1) = GETOBJ(int,1) - MACADDR_MIN_STD_ACL + MINMACADDRACC);
NUMBER	(aleth_num, aleth_modify, NONE,
	 OBJ(int,1), MACADDR_MIN_STD_ACL, MACADDR_MAX_STD_ACL,
	 "48-bit MAC address access list");
NVGENS	(aleth_nvgen, aleth_num, addressaccess_command, LIST_MAC);
NOP	(access_list_ethernet, aleth_nvgen, ALTERNATE);

#undef	ALTERNATE
#define	ALTERNATE	access_list_ethernet

/*
 * Old style "[no] access-list ..."
 *
 * These remaining lines should be deleted in a future release.
 */
NUMBER	(oaleth_num, aleth_noprefix, NONE,
	 OBJ(int,1), MINMACADDRACC, MAXMACADDRACC,
	 "48-bit MAC address access list");
NVGENS	(oaleth_nvgen, oaleth_num, addressaccess_command, LIST_MAC);
NOP	(oaccess_list_ethernet, oaleth_nvgen, OALTERNATE);

#undef	OALTERNATE
#define	OALTERNATE	oaccess_list_ethernet
