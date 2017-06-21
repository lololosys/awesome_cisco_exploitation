/* $Id: cfg_acclist_xmac.h,v 3.3.56.1 1996/05/30 23:45:36 hampton Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_acclist_xmac.h,v $
 *------------------------------------------------------------------
 * Extended MAC access list commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_acclist_xmac.h,v $
 * Revision 3.3.56.1  1996/05/30  23:45:36  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/27  21:25:05  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  18:52:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:54  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:01:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************
 * Extended MAC access list
 *
 * access-list <1100-1199> permit|deny <src> <srcmask> <dst> <dstmask>
 *		<offset> <size> <operator> <operand>
 *
 * OBJ(int,1) = list number
 * OBJ(int,2) = TRUE for "permit", FALSE for "deny"
 * OBJ(hwaddr,1) = <src>
 * OBJ(hwaddr,2) = <srcmask>
 * OBJ(hwaddr,3) = <dst>
 * OBJ(hwaddr,4) = <dstmask>
 * OBJ(int,3) = <offset>
 * OBJ(int,4) = <size>
 * OBJ(int,5) = <operator>
 *		OPC_NONE, OPC_EQ, OPC_NEQ, OPC_LT, OPC_GT, OPC_AND, OPC_XOR
 * OBJ(int,6) = <operand>
 */

EOLS	(alxmaceol, xmacaccess_command, LIST_XMAC);

/* <operand> */
NUMBER (alxmacoperand, alxmaceol, no_alt,
	OBJ(int,6), 0, -1, "Pattern to compare, in decimal, hex, or octal");

/* <operator> */
KEYWORD_ID (alxmackeyxor, alxmacoperand, no_alt,
	    OBJ(int,5), OPC_XOR,
	    OPC_XOR_STR, "Exclusive 'or'", PRIV_CONF);

KEYWORD_ID (alxmackeyand, alxmacoperand, alxmackeyxor,
	    OBJ(int,5), OPC_AND,
	    OPC_AND_STR, "Logical 'and'", PRIV_CONF);

KEYWORD_ID (alxmackeygt, alxmacoperand, alxmackeyand,
	    OBJ(int,5), OPC_GT,
	    OPC_GT_STR, "Greater than", PRIV_CONF);

KEYWORD_ID (alxmackeylt, alxmacoperand, alxmackeygt,
	    OBJ(int,5), OPC_LT,
	    OPC_LT_STR, "Less than", PRIV_CONF);

KEYWORD_ID (alxmackeyneq, alxmacoperand, alxmackeylt,
	    OBJ(int,5), OPC_NEQ,
	    OPC_NEQ_STR, "Not equal", PRIV_CONF);

KEYWORD_ID (alxmackeyeq, alxmacoperand, alxmackeyneq,
	    OBJ(int,5), OPC_EQ,
	    OPC_EQ_STR, "Equal", PRIV_CONF);

/* <offset> <size> */
NUMBER (alxmacsize, alxmackeyeq, no_alt,
	OBJ(int,4), 1, 4, "Number of bytes to compare");
NUMBER (alxmacoffset, alxmacsize, alxmaceol,
	OBJ(int,3), 0, MAXETHERSIZE, "Offset into the packet");

/* <dst> <dstmask> */
ENADDR	(alxmacdstmask, alxmacoffset, no_alt,
	 OBJ(hwaddr,4), "48-bit hardware dest address mask");
ENADDR	(alxmacdstaddr, alxmacdstmask, no_alt,
	 OBJ(hwaddr,3), "48-bit hardware dest address");

/* <src> <srcmask> */
ENADDR	(alxmacsrcmask, alxmacdstaddr, no_alt,
	 OBJ(hwaddr,2), "48-bit hardware source address mask");
ENADDR	(alxmacsrcaddr, alxmacsrcmask, no_alt,
	 OBJ(hwaddr,1), "48-bit hardware source address");

PERMIT_DENY(alxmac_permit_deny, alxmacsrcaddr, no_alt, OBJ(int,2), PRIV_CONF);

NOPREFIX(alxmacnoprefix, alxmac_permit_deny, alxmaceol);

EVAL	(alxmac_modify, alxmacnoprefix,
	 GETOBJ(int,1) = GETOBJ(int,1) - MACADDR_MIN_EXT_ACL + MINXMACACC);
NUMBER	(alxmacnum, alxmac_modify, NONE,
	 OBJ(int,1), MACADDR_MIN_EXT_ACL, MACADDR_MAX_EXT_ACL,
	 "Extended 48-bit MAC address access list");
NVGENS	(alxmac_nvgen, alxmacnum, xmacaccess_command, LIST_XMAC);
NOP	(access_list_xmac, alxmac_nvgen, ALTERNATE);

#undef	ALTERNATE
#define	ALTERNATE	access_list_xmac


/*
 * Old style "[no] access-list ..."
 *
 * These remaining lines should be deleted in a future release.
 */
NUMBER	(oalxmacnum, alxmacnoprefix, NONE,
	 OBJ(int,1), MINXMACACC, MAXXMACACC,
	 "Extended 48-bit MAC address access list");
NVGENS	(oalxmac_nvgen, oalxmacnum, xmacaccess_command, LIST_XMAC);
NOP	(oaccess_list_xmac, oalxmac_nvgen, OALTERNATE);

#undef	OALTERNATE
#define	OALTERNATE	oaccess_list_xmac
