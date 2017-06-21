/* $Id: cfg_acclist_xns.h,v 3.3.54.2 1996/06/11 20:28:39 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_acclist_xns.h,v $
 *------------------------------------------------------------------
 * XNS access list commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_acclist_xns.h,v $
 * Revision 3.3.54.2  1996/06/11  20:28:39  hampton
 * New access-list commands should set the NONVGEN flag. [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3.54.1  1996/05/30  23:47:40  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/27  21:25:11  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  19:20:53  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:42:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:01:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*****************************************************************
 * XNS extended access list
 *
 *
 * access-list <500-599> { permit | deny } <proto>
 *	[<net> [.<host> [<mask>]]
 *	[<source-socket> [<net> [.<host> [<mask>]] [<destination-socket>]]]]
 *
 * This translates into the following state diagram.
 *
 *
 *
 *                         |permit-\           |<addr>--<mask>--\
 * access-list--<500-599>--|        --<proto>--|      \------------
 *                         |deny---/           |                /
 *                                             |<net>----------/
 *                                             |
 *                                             |EOLS
 *
 *
 *                    |
 *                    |<addr>--<mask>--\
 *   |                |      \          \ |
 * --|<src-socket>----|       \-----------|<dest-socket>--EOLS
 *   |                |               /   |
 *   |EOLS            |<net>---------/    |EOLS
 *                    |
 *                    |EOLS
 *
 * The vertical bars denote sets of alternate states.  I.E. after a "permit"
 * keyword, we expect to find an XNS address, an XNS network number, or
 * and End-of-Line (EOLS).
 * Following the XNS address, we can have a <mask>, a destination address, a
 * destination network, or an EOLS.
 *
 * The variables used in this access list are kept the same as those
 * used in the standard access list as much as possible.
 *
 * OBJ(int,1) = access list number
 * OBJ(int,2) = TRUE for "permit", FALSE for "deny".
 * OBJ(int,3) = Flags which tell what we've parsed.
 *	XNS_SRCNET
 * 	XNS_SRCHOST
 * 	XNS_SRCHOSTMASK
 * 	XNS_DSTNET
 * 	XNS_DSTHOST
 * 	XNS_DSTHOSTMASK
 * 	XNS_PROTOCOL
 * 	XNS_SRCSOCKET
 * 	XNS_DSTSOCKET
 * 	XNS_PEPCLIENT
 *      XNS_SRCNETMASK
 *      XNS_DSTNETMASK
 * OBJ(int,4) = Source net address.
 * OBJ(int,5) = Destination net address.
 * OBJ(int,6) = Protocol type number.
 * OBJ(int,7) = Source socket number.
 * OBJ(int,8) = Destination socket number.
 * OBJ(int,9) = PEP client type.
 * (*OBJ(paddr,1)) = Source XNS address.
 * (*OBJ(paddr,2)) = Source XNS mask (includes network if XNS_SRCNETMASK).
 * (*OBJ(paddr,3)) = Destination XNS address.
 * (*OBJ(paddr,4)) = Destination XNS mask (includes network if XNS_SRCNETMASK).
 *
 */

/* End of line */
EOLS	(alexns_eol, xnseaccess_command, LIST_XNS2);

/* Check for PEP client type */
EVAL	(alexns_pep_eval, alexns_eol, GETOBJ(int,3) |= XNS_PEPCLIENT);
NUMBER	(alexns_pep, alexns_pep_eval, alexns_eol,
	 OBJ(int,9), 0, 255, "PEP client type");

/* Check for destination socket number */
EVAL	(alexns_socket2set, alexns_pep, GETOBJ(int,3) |= XNS_DSTSOCKET);
NUMBER	(alexns_socket2, alexns_socket2set, alexns_eol,
	 OBJ(int,8), 0, 65535, "Socket number (0 for all sockets)");

/* Check for destination net address. First, check for an
 * unsigned decimal number, then check for "-1".
*/
EVAL	(alexns_net2set, alexns_socket2, GETOBJ(int,3) |= XNS_DSTNET);
NEG1	(alexns_net2neg, alexns_net2set, alexns_eol, OBJ(int,5),
	 "Any XNS net");
NUMBER	(alexns_net2, alexns_net2set, alexns_net2neg,
	 OBJ(int,5), 0, 0xFFFFFFFF, "Destination net");

/* Choice was NET.HOST mask */
EVAL    (alexns_mask2set, alexns_socket2,
         GETOBJ(int,3) |= XNS_DSTNETMASK + XNS_DSTHOSTMASK);
XNSADDR (alexns_mask2, alexns_mask2set, alexns_socket2,
         OBJ(paddr,4), "Destination net.host mask");

/* Check for a destination XNS address and mask. */
EVAL	(alexns_hstmsk2set, alexns_socket2, GETOBJ(int,3) |= XNS_DSTHOSTMASK);
XNSHOST	(alexns_hstmsk2, alexns_hstmsk2set, alexns_mask2,
	 OBJ(paddr,4), "Destination host mask");
EVAL	(alexns_addr2set, alexns_hstmsk2,
	 GETOBJ(int,3) |= XNS_DSTNET + XNS_DSTHOST);
XNSADDR	(alexns_addr2, alexns_addr2set, alexns_net2,
	 OBJ(paddr,3), "Destination net.host address");

/* Check for a socket number */
EVAL	(alexns_socket1set, alexns_addr2, GETOBJ(int,3) |= XNS_SRCSOCKET);
NUMBER	(alexns_socket1, alexns_socket1set, alexns_eol,
	 OBJ(int,7), 0, 65535, "Socket number (0 for all sockets)");

/* Check for a source net address. First, check for an
 * unsigned decimal number, then check for "-1".  If we don't
 * find any address, check for EOLS.
 */
EVAL	(alexns_net1set, alexns_socket1, GETOBJ(int,3) |= XNS_SRCNET);
NEG1	(alexns_net1neg, alexns_net1set, alexns_eol, OBJ(int,4),
	 "Any XNS net");
NUMBER	(alexns_net1, alexns_net1set, alexns_net1neg,
	 OBJ(int,4),  0, 0xFFFFFFFF, "Source net");

/* Choice was NET.HOST mask */
EVAL    (alexns_mask1set, alexns_socket1,
         GETOBJ(int,3) |=  XNS_SRCNETMASK + XNS_SRCHOSTMASK);
XNSADDR (alexns_mask1, alexns_mask1set, alexns_socket1,
         OBJ(paddr,2), "Source net.host mask");

/* Check for a full XNS source address and possibly host mask. */
EVAL	(alexns_hstmsk1set, alexns_socket1, GETOBJ(int,3) |= XNS_SRCHOSTMASK);
XNSHOST	(alexns_hstmsk1, alexns_hstmsk1set, alexns_mask1,
	 OBJ(paddr,2), "Source host mask");
EVAL	(alexns_addr1set, alexns_hstmsk1,
	 GETOBJ(int,3) |= XNS_SRCNET + XNS_SRCHOST);
XNSADDR	(alexns_addr1, alexns_addr1set, alexns_net1,
	 OBJ(paddr,1), "Source net.host address");

/* Get protocol number */
EVAL	(alexns_proto_set, alexns_addr1, GETOBJ(int,3) |= XNS_PROTOCOL);
NUMBER	(alexns_proto, alexns_proto_set, no_alt, OBJ(int,6), 0, 255,
	 "Protocol type number");

/* Permit & Deny keywords */
PERMIT_DENY(alexns_permit_deny, alexns_proto, no_alt, OBJ(int,2), PRIV_CONF);

NOPREFIX(alexns_noprefix, alexns_permit_deny, alexns_eol);

EVAL	(alexns_modify, alexns_noprefix,
	 GETOBJ(int,1) = GETOBJ(int,1) - XNS_MIN_EXT_ACL + MINEXNSACC);
NUMBER	(alexns_num, alexns_modify, NONE,
	 OBJ(int,1), XNS_MIN_EXT_ACL, XNS_MAX_EXT_ACL,
	 "XNS extended access list");
NVGENS	(alexns_nvgen, alexns_num, xnseaccess_command, LIST_XNS2);
NOP	(access_list_ext_xns, alexns_nvgen, NONE);

/*
 * Old style "[no] access-list ..."
 *
 * This section should be deleted in a future release.
 */
NUMBER	(oalexns_num, alexns_noprefix, NONE,
	 OBJ(int,1), MINEXNSACC, MAXEXNSACC,
	 "XNS extended access list");
NVGENS	(oalexns_nvgen, oalexns_num, xnseaccess_command, LIST_XNS2);
NOP	(oaccess_list_ext_xns, oalexns_nvgen, NONE);

/*****************************************************************
 * XNS standard access list
 *
 *
 * access-list <400-499> { permit | deny } <net> [.<host> [<mask>]]
 *	[<net> [.<host> [<mask>]]]
 *
 * This translates into the following state diagram.
 *
 *                                                              |<mask>--EOLS
 *                         |permit-\ |<addr>--<mask>-\ |<addr>--|
 * access-list--<400-499>--|        -|      \----------|        |EOLS
 *                         |deny---/ |                /|
 *                                   |<net>----------/ |
 *                                                     |<net>--EOLS
 *                                                     |
 *                                                     |EOLS
 *
 * The vertical bars denote sets of alternate states.  I.E. after a "permit"
 * keyword, we expect to find either an XNS address, or an XNS network number.
 * There is no End-of-Line (EOLS) listed, so this is a required parameter.
 * Following the XNS address, we can have a <mask>, a destination address, a
 * destination network, or an EOLS.
 *
 * OBJ(int,1) = access list number
 * OBJ(int,2) = TRUE for "permit", FALSE for "deny".
 * OBJ(int,3) = Flags which tell what we've parsed.
 *	XNS_SRCNET
 * 	XNS_SRCHOST
 * 	XNS_SRCHOSTMASK
 * 	XNS_DSTNET
 * 	XNS_DSTHOST
 * 	XNS_DSTHOSTMASK
 * OBJ(int,4) = Source net address.
 * OBJ(int,5) = Destination net address.
 * (*OBJ(paddr,1)) = Source XNS address.
 * (*OBJ(paddr,2)) = Source XNS mask.
 * (*OBJ(paddr,3)) = Destination XNS address.
 * (*OBJ(paddr,4)) = Destination XNS mask.
 *
 */

/* End of line */
EOLS	(alsxns_eol, xnsnaccess_command, LIST_XNS1);

/* Check for destination net address. First, check for an
 * unsigned decimal number, then check for "-1".
 */
EVAL	(alsxns_net2set, alsxns_eol, GETOBJ(int,3) |= XNS_DSTNET);
NEG1	(alsxns_net2neg, alsxns_net2set, alsxns_eol,
	 OBJ(int,5), "Any XNS net");
NUMBER	(alsxns_net2, alsxns_net2set, alsxns_net2neg,
	 OBJ(int,5), 1, -2, "Destination net");

/* Check for a destination XNS address and mask. */
EVAL	(alsxns_mask2set, alsxns_eol, GETOBJ(int,3) |= XNS_DSTHOSTMASK);
XNSHOST	(alsxns_mask2, alsxns_mask2set, alsxns_eol,
	 OBJ(paddr,4), "Destination host mask");
EVAL	(alsxns_addr2set, alsxns_mask2,
	 GETOBJ(int,3) |= XNS_DSTNET + XNS_DSTHOST);
XNSADDR	(alsxns_addr2, alsxns_addr2set, alsxns_net2,
	 OBJ(paddr,3), "Destination net.host address");

/* Check for a source net address. First, check for an
 * unsigned decimal number, then check for "-1".
 */
EVAL	(alsxns_net1set, alsxns_addr2, GETOBJ(int,3) |= XNS_SRCNET);
NEG1	(alsxns_net1neg, alsxns_net1set, no_alt,
	 OBJ(int,4), "Any XNS net");
NUMBER (alsxns_net1, alsxns_net1set, alsxns_net1neg,
	 OBJ(int,4),  1, -2, "Source net");

/* Check for a full XNS source address and mask. */
EVAL	(alsxns_mask1set, alsxns_addr2, GETOBJ(int,3) |= XNS_SRCHOSTMASK);
XNSHOST	(alsxns_mask1, alsxns_mask1set, alsxns_addr2,
	 OBJ(paddr,2), "Source host mask");
EVAL	(alsxns_addr1set, alsxns_mask1,
	 GETOBJ(int,3) |= XNS_SRCNET + XNS_SRCHOST);
XNSADDR	(alsxns_addr1, alsxns_addr1set, alsxns_net1,
	 OBJ(paddr,1), "Source net.host address");

/* Permit & Deny keywords */
KEYWORD_ID(alsxns_permit, alsxns_addr1, no_alt,
	   OBJ(int,2), TRUE,
	   "permit", "Specify packets to permit", PRIV_CONF);

KEYWORD_ID(alsxns_deny, alsxns_addr1, alsxns_permit,
	   OBJ(int,2), FALSE,
	   "deny", "Specify packets to reject", PRIV_CONF);

NOPREFIX(alsxns_noprefix, alsxns_deny, alsxns_eol);

EVAL	(alsxns_modify, alsxns_noprefix,
	 GETOBJ(int,1) = GETOBJ(int,1) - XNS_MIN_STD_ACL + MINXNSACC);
NUMBER	(alsxns_num, alsxns_modify, NONE,
	 OBJ(int,1), XNS_MIN_STD_ACL, XNS_MAX_STD_ACL,
	 "XNS standard access list");
NVGENS	(alsxns_nvgen, alsxns_num, xnsnaccess_command, LIST_XNS1);
NOP	(access_list_std_xns, alsxns_nvgen, access_list_ext_xns);
KEYWORD (access_list_xns, access_list_std_xns, ALTERNATE,
	 "access-list", "XNS Access lists",
	 PRIV_CONF | PRIV_HIDDEN | PRIV_NONVGEN);

#undef	ALTERNATE
#define	ALTERNATE	access_list_xns

/*
 * Old style "[no] access-list ..."
 *
 * These remaining lines should be deleted in a future release.
 */
NUMBER	(oalsxns_num, alsxns_noprefix, NONE,
	 OBJ(int,1), MINXNSACC, MAXXNSACC,
	 "XNS standard access list");
NVGENS	(oalsxns_nvgen, oalsxns_num, xnsnaccess_command, LIST_XNS1);
NOP	(oaccess_list_std_xns, oalsxns_nvgen, oaccess_list_ext_xns);

#undef	OALTERNATE
#define	OALTERNATE	oaccess_list_std_xns
