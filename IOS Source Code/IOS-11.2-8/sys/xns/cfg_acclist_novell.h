/* $Id: cfg_acclist_novell.h,v 3.5.6.5 1996/08/02 07:22:35 akr Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_acclist_novell.h,v $
 *------------------------------------------------------------------
 * Novell access list commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_acclist_novell.h,v $
 * Revision 3.5.6.5  1996/08/02  07:22:35  akr
 * CSCdi64780:  access-list 900 broken for plain-english-filters
 * Branch: California_branch
 *
 * Revision 3.5.6.4  1996/07/26  00:08:25  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.5.6.3  1996/06/11  20:28:36  hampton
 * New access-list commands should set the NONVGEN flag. [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.5.6.2  1996/05/30  23:47:38  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.5.6.1  1996/05/03  02:15:41  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.5  1996/03/05  23:11:54  sluong
 * CSCdi50715:  NLSP route aggregation conformance (phase 2)
 *
 * Revision 3.4  1996/03/01  07:34:15  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.3  1995/11/27  21:25:10  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  19:20:51  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:37:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:27:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************
 * Novell summary access list:
 *
 * access-list <1200-1299> { permit | deny } <net> <mask>
 *                         [interface] [ticks] <number> [area-count] <number>
 *
 * OBJ(int,1) = access list number
 * OBJ(int,2) = TRUE for "permit", FALSE for "deny".
 * OBJ(int,3) = IPX network number
 * OBJ(int,4) = IPX network mask
 * OBJ(int,5) = ticks
 * OBJ(int,6) = area count
 * OBJ(int,7) = TRUE => interface specified in OBJ(idb,1)
 */

EOLS	(alssum_eol, novsumaccess_command, LIST_NOVELL4);

NUMBER  (alssum_area_count_value, alssum_eol, no_alt, OBJ(int,6),
	 NOV_MIN_AREA_COUNT, NOV_MAX_AREA_COUNT, "IPX area count");
KEYWORD (alssum_area_count, alssum_area_count_value, alssum_eol,
	 "area-count", "IPX area count", PRIV_CONF);
NUMBER  (alssum_ticks_value, alssum_area_count, no_alt, OBJ(int,5),
	 NOV_MIN_TICKS, NOV_MAX_TICKS, "IPX ticks count");
KEYWORD (alssum_ticks, alssum_ticks_value, alssum_area_count, "ticks",
	 "IPX ticks count", PRIV_CONF);

SET       (alssum_itf_set_yes, alssum_ticks, OBJ(int,7), TRUE);
INTERFACE (alssum_itf_get, alssum_itf_set_yes, alssum_ticks,
	   OBJ(idb,1), IFTYPE_ANYSUB);
SET       (alssum_itf_set_no, alssum_itf_get, OBJ(int,7), FALSE);

HEXDIGIT  (alssum_mask_get, alssum_itf_set_no, alssum_itf_set_no,
	   OBJ(int,4), 0x0, 0xffffffff, "IPX network mask");
SET     (alssum_mask_set, alssum_mask_get, OBJ(int,4), 0xffffffff);

NEG1	(alssum_neg, alssum_ticks, no_alt, OBJ(int,3), "Any IPX net");
HEXDIGIT  (alssum_net, alssum_mask_set, alssum_neg, OBJ(int,3), 0,
	   NOV_MAX_NET, "IPX network address");

/* Permit & Deny keywords */
KEYWORD_ID(alssum_permit, alssum_net, no_alt, OBJ(int,2), TRUE,
	   "permit", "Specify addresses to permit", PRIV_CONF);

KEYWORD_ID(alssum_deny, alssum_net, alssum_permit, OBJ(int,2), FALSE,
	   "deny", "Specify addresses to reject", PRIV_CONF);

NOPREFIX(alssum_noprefix, alssum_deny, alssum_eol);

EVAL	(alssum_modify, alssum_noprefix, /* cvt to old number */
	 GETOBJ(int,1) = GETOBJ(int,1) - IPX_MIN_SUM_ACL + MINNOVSUM);
NUMBER	(alssum_num, alssum_modify, NONE,
	 OBJ(int,1), IPX_MIN_SUM_ACL, IPX_MAX_SUM_ACL,
	 "IPX summary address access list");
NVGENS	(alssum_nvgen, alssum_num, novsumaccess_command, LIST_NOVELL4);
NOP	(access_list_summary_novell, alssum_nvgen, ALTERNATE);

/*
 * Old style "[no] access-list ..."
 *
 * This section should be deleted in a future release.
 */
NUMBER	(oalssum_num, alssum_noprefix, NONE, OBJ(int,1), MINNOVSUM, MAXNOVSUM,
	 "IPX summary address access list");
NVGENS	(oalssum_nvgen, oalssum_num, novsumaccess_command, LIST_NOVELL4);
NOP	(oaccess_list_summary_novell, oalssum_nvgen, OALTERNATE);

/*****************************************************************
 * Novell SAP access list
 *
 * access-list <1000-1099> { permit | deny }
 *	<net>[.<host>] [<service-type> [<server_name>]]
 *
 * Define an access list for filtering SAP requests.
 *
 * OBJ(int,1) = list number
 * OBJ(int,2) = TRUE for "permit", FALSE for "deny"
 * OBJ(int,3) = Flags denoting what was parsed.
 * OBJ(int,4) = Novell network number, if only a net number was specified.
 * OBJ(int,5) = Novell service type.
 * (*OBJ(paddr,1)) = Novell address.
 * (*OBJ(paddr,2)) = Novell address mask.
 * OBJ(string,1) = Novell server name.
 */


EOLS	(alsap_eol, novsaccess_command, LIST_NOVELL3);

/*
 * Check for optional Novell service name:
 */
EVAL    (alsap_nameset, alsap_eol, GETOBJ(int,3) |= XNS_SERVER_NAME);
STRING  (alsap_servname, alsap_nameset, alsap_eol, OBJ(string,1),
         "A SAP server name");

/* Check for the Novell service type.  0 means any service matches.
 */
EVAL	(alsap_servset, alsap_servname, GETOBJ(int,3) |= XNS_SERVER_TYPE);
HEXDIGIT(alsap_service, alsap_servset, alsap_eol,
	 OBJ(int,5), 0, 0xFFFF, "Service type-code (0 matches all services)");

/* Check for a full Novell source address mask. */
EVAL	(alsap_mask1set, alsap_service,
	 GETOBJ(int,3) |= XNS_SRCNETMASK + XNS_SRCHOSTMASK);
NOVADDR	(alsap_mask1, alsap_mask1set, alsap_service,
	 OBJ(paddr,2), "Source net.host mask");

/* Check for a source net address. First, check for an
 * unsigned hexadecimal number, then check for "-1".
 */
EVAL	(alsap_net1set, alsap_mask1, GETOBJ(int,3) |= XNS_SRCNET);
NEG1	(alsap_net1neg, alsap_net1set, no_alt, OBJ(int,4),
	 "Any IPX net");
HEXNUM (alsap_net1, alsap_net1set, alsap_net1neg,
	 OBJ(int,4), "Source net");

/* Check for a full Novell source address. */
EVAL	(alsap_addr1set, alsap_mask1,
	 GETOBJ(int,3) |= XNS_SRCNET + XNS_SRCHOST);
NOVADDR	(alsap_addr1, alsap_addr1set, alsap_net1,
	 OBJ(paddr,1), "Source net.host address");

/* Permit & Deny keywords */
PERMIT_DENY(alsap_permit_deny, alsap_addr1, no_alt, OBJ(int,2), PRIV_CONF);

NOPREFIX(alsap_noprefix, alsap_permit_deny, alsap_eol);

EVAL	(alsap_modify, alsap_noprefix, /* cvt to old number */
	 GETOBJ(int,1) = GETOBJ(int,1) - IPX_MIN_SAP_ACL + MINNOVSAPACC);
NUMBER	(alsap_num, alsap_modify, NONE,
	 OBJ(int,1), IPX_MIN_SAP_ACL, IPX_MAX_SAP_ACL,
	 "IPX SAP access list");
NVGENS	(alsap_nvgen, alsap_num, novsaccess_command, LIST_NOVELL3);
NOP	(access_list_sap, alsap_nvgen, access_list_summary_novell);

/*
 * Old style "[no] access-list ..."
 *
 * This section should be deleted in a future release.
 */

NUMBER	(oalsap_num, alsap_noprefix, NONE,
	 OBJ(int,1), MINNOVSAPACC, MAXNOVSAPACC,
	 "IPX SAP access list");
NVGENS	(oalsap_nvgen, oalsap_num, novsaccess_command, LIST_NOVELL3);
NOP	(oaccess_list_sap, oalsap_nvgen, oaccess_list_summary_novell);

/*****************************************************************
 * Novell extended access list
 *
 *
 * access-list <900-999> { permit | deny } <proto>
 *      OPTIONAL END OF LINE
 *      /
 *      | [<srcnet>] OR
 *	| [<srcnet>[.<srchost>] [<srcnet-mask>.<srchost-mask>] OR
 *      |                                     [<srchost-mask>]]
 *      \
 *      OPTIONAL END OF LINE
 *      [<src-socket>]
 *      OPTIONAL END OF LINE
 *      /
 *      | [<dstnet>] OR
 *	| [<dstnet>[.<dsthost>] [<dstnet-mask>.<dsthost-mask>] OR
 *      |                                     [<dsthost-mask>]]
 *      \
 *      OPTIONAL END OF LINE
 *      [<dst-socket>]
 *      FORCED END OF LINE
 *
 *
 * The vertical bars denote sets of alternate states.  I.E. after a protocol
 * number, we expect to find an XNS address, an XNS network number,
 * a network.address combination or an End-of-Line (EOLS).
 * Following the XNS address, we can have a <mask>, a destination address, a
 * destination network, a destination network.address, or an EOLS.
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
 *      XNS_SRCNETMASK
 *      XNS_DSTNETMASK
 * OBJ(int,4) = Source net address.
 * OBJ(int,5) = Destination net address.
 * OBJ(int,6) = Protocol type number.
 * OBJ(int,7) = Source socket number.
 * OBJ(int,8) = Destination socket number.
 * OBJ(int,9) = TRUE for logging enabled.
 * (*OBJ(paddr,1)) = Source XNS address.
 * (*OBJ(paddr,2)) = Source XNS mask (includes network if XNS_SRCNETMASK).
 * (*OBJ(paddr,3)) = Destination XNS address.
 * (*OBJ(paddr,4)) = Destination XNS mask (includes network if XNS_DSTNETMASK).
 *
 */

/* End of line */
EOLS	(alenov_eol, noveaccess_command, LIST_NOVELL2);

/* Log */
KEYWORD_ID (alenov_log, alenov_eol, alenov_eol, OBJ(int,9), TRUE,
	"log", "Log matches against this entry", PRIV_CONF);

/* Check for destination socket number */
EVAL	(alenov_socket2set, alenov_log, GETOBJ(int,3) |= XNS_DSTSOCKET);
KEYWORD_ID_MM (alenov_soc2all, alenov_socket2set, alenov_log,
	OBJ(int, 8), NOVELL_SOCK_ALL, "all", "All sockets", PRIV_CONF, 2);
KEYWORD_ID (alenov_soc2cping, alenov_socket2set, alenov_soc2all,
	OBJ(int, 8), NOVELL_SOCK_ECHO, "cping",
	"Cisco ipx ping", PRIV_CONF);
KEYWORD_ID (alenov_soc2nping, alenov_socket2set, alenov_soc2cping,
	OBJ(int, 8), NOVELL_SOCK_ECHO_STND, "nping",
	"Standard IPX ping", PRIV_CONF);
KEYWORD_ID (alenov_soc2sap, alenov_socket2set, alenov_soc2nping,
        OBJ(int,8), NOVELL_SOCK_SAP, "sap",
        "Service Advertising Protocol", PRIV_CONF);
KEYWORD_ID (alenov_soc2rip, alenov_socket2set, alenov_soc2sap,
        OBJ(int,8), NOVELL_SOCK_RIP, "rip",
        "IPX Routing Information Protocol", PRIV_CONF);
KEYWORD_ID (alenov_soc2nlsp, alenov_socket2set, alenov_soc2rip,
        OBJ(int,8), NOVELL_SOCK_NLSP, "nlsp",
        "NetWare Link State Protocol", PRIV_CONF);
KEYWORD_ID (alenov_soc2ncp, alenov_socket2set, alenov_soc2nlsp,
        OBJ(int,8), NOVELL_SOCK_NCP, "ncp",
        "NetWare Core Protocol", PRIV_CONF);
KEYWORD_ID (alenov_soc2netbios, alenov_socket2set, alenov_soc2ncp,
        OBJ(int,8), NOVELL_SOCK_NETBIOS, "netbios",
        "IPX NetBIOS", PRIV_CONF);
KEYWORD_ID (alenov_soc2eigrp, alenov_socket2set, alenov_soc2netbios,
        OBJ(int,8), NOVELL_SOCK_IGRP, "eigrp",
        "IPX Enhanced Interior Gateway Routing Protocol", PRIV_CONF);
KEYWORD_ID (alenov_soc2diag, alenov_socket2set, alenov_soc2eigrp,
        OBJ(int,8), NOVELL_SOCK_DIAG, "diagnostic",
        "IPX Diagnostic packet", PRIV_CONF);
HEXNUM	(alenov_socket2, alenov_socket2set, alenov_soc2diag,
	 OBJ(int,8), "Destination Socket HEXIDECIMAL");

/* Check for destination net address. First, check for an
 * unsigned decimal number, then check for "-1".
 */
EVAL	(alenov_net2set, alenov_socket2, GETOBJ(int,3) |= XNS_DSTNET);
KEYWORD_ID_MM (alenov_net2any, alenov_net2set, alenov_log, OBJ(int,5),
	 NOVELL_TYPE_ANY_NET, "any", "Any IPX net", PRIV_CONF, 2);
KEYWORD_ID (alenov_net2neg, alenov_net2set, alenov_net2any, OBJ(int,5),
         -1L, "-1", "Any IPX net", PRIV_CONF | PRIV_HIDDEN);
HEXNUM (alenov_net2, alenov_net2set, alenov_net2neg,
	 OBJ(int,5), "Destination net");

/* Check for a destination XNS address and mask.
 * The mask is retrieved by the NOVHOST macro while the <net.host> must
 * be parsed by NOVADDR, due to the difference in the representation
 * of XNS and Novell network addresses.
 */
EVAL	(alenov_mask2set, alenov_socket2,
	 GETOBJ(int,3) |= XNS_DSTNETMASK + XNS_DSTHOSTMASK);
NOVADDR	(alenov_mask2, alenov_mask2set, alenov_socket2,
	 OBJ(paddr,4), "Destination net.host mask");

/* Choice was HOST mask */
EVAL    (alenov_hstmsk2set, alenov_socket2,
	 GETOBJ(int,3) |= XNS_DSTHOSTMASK);
ENADDR  (alenov_hstmsk2, alenov_hstmsk2set, alenov_mask2,
	 OBJ(hwaddr,4), "Destination host mask");

EVAL	(alenov_addr2set, alenov_hstmsk2,
	 GETOBJ(int,3) |= XNS_DSTNET + XNS_DSTHOST);
NOVADDR	(alenov_addr2, alenov_addr2set, alenov_net2,
	 OBJ(paddr,3), "Destination net.host address");

/* Check for a socket number */
EVAL	(alenov_socket1set, alenov_addr2, GETOBJ(int,3) |= XNS_SRCSOCKET);
KEYWORD_ID_MM (alenov_soc1all, alenov_socket1set, alenov_log,
	OBJ(int, 7), NOVELL_SOCK_ALL, "all", "All sockets", PRIV_CONF, 2);
KEYWORD_ID (alenov_soc1cping, alenov_socket1set, alenov_soc1all,
	OBJ(int, 7), NOVELL_SOCK_ECHO, "cping",
	"Cisco ipx ping", PRIV_CONF);
KEYWORD_ID (alenov_soc1nping, alenov_socket1set, alenov_soc1cping,
	OBJ(int, 7), NOVELL_SOCK_ECHO_STND, "nping",
	"Standard IPX ping", PRIV_CONF);
KEYWORD_ID (alenov_socsap, alenov_socket1set, alenov_soc1nping,
	OBJ(int,7), NOVELL_SOCK_SAP, "sap", 
	"Service Advertising Protocol", PRIV_CONF);
KEYWORD_ID (alenov_socrip, alenov_socket1set, alenov_socsap,
	OBJ(int,7), NOVELL_SOCK_RIP, "rip", 
	"IPX Routing Information Protocol", PRIV_CONF);
KEYWORD_ID (alenov_socnlsp, alenov_socket1set, alenov_socrip,
	OBJ(int,7), NOVELL_SOCK_NLSP, "nlsp", 
	"NetWare Link State Protocol", PRIV_CONF);
KEYWORD_ID (alenov_socncp, alenov_socket1set, alenov_socnlsp,
	OBJ(int,7), NOVELL_SOCK_NCP, "ncp", 
	"NetWare Core Protocol", PRIV_CONF);
KEYWORD_ID (alenov_socnetbios, alenov_socket1set, alenov_socncp,
	OBJ(int,7), NOVELL_SOCK_NETBIOS, "netbios", 
	"IPX NetBIOS", PRIV_CONF);
KEYWORD_ID (alenov_soceigrp, alenov_socket1set, alenov_socnetbios,
	OBJ(int,7), NOVELL_SOCK_IGRP, "eigrp", 
	"IPX Enhanced Interior Gateway Routing Protocol", PRIV_CONF);
KEYWORD_ID (alenov_socdiag, alenov_socket1set, alenov_soceigrp,
	OBJ(int,7), NOVELL_SOCK_DIAG, "diagnostic", 
	"Diagnostic packet", PRIV_CONF);
HEXNUM	(alenov_socket1, alenov_socket1set, alenov_socdiag,
	 OBJ(int,7), "Source Socket HEXIDECIMAL");

/* Check for a source net address. First, check for an
 * unsigned decimal number, then check for "-1".  If we don't
 * find any address, check for EOLS.
 */
EVAL	(alenov_net1set, alenov_socket1, GETOBJ(int,3) |= XNS_SRCNET);
KEYWORD_ID_MM (alenov_net1any, alenov_net1set, alenov_log, OBJ(int,4),
	 NOVELL_TYPE_ANY_NET, "any", "Any IPX net", PRIV_CONF, 2);
KEYWORD_ID (alenov_net1neg, alenov_net1set, alenov_net1any, OBJ(int,4),
         -1L, "-1", "Any IPX net", PRIV_CONF | PRIV_HIDDEN);
HEXNUM (alenov_net1, alenov_net1set, alenov_net1neg,
	 OBJ(int,4), "Source net");

/* Check for a full Novell source address and mask.
 * The <net.host> must
 * be parsed by NOVADDR, due to the difference in the representation
 * of XNS and Novell network addresses.
 */
/* Choice was NET.HOST mask */
EVAL	(alenov_mask1set, alenov_socket1,
	 GETOBJ(int,3) |=  XNS_SRCNETMASK + XNS_SRCHOSTMASK);
NOVADDR	(alenov_mask1, alenov_mask1set, alenov_socket1,
	 OBJ(paddr,2), "Source net.host mask");

/* Choice was HOST mask */
EVAL    (alenov_hstmsk1set, alenov_socket1,
	 GETOBJ(int,3) |= XNS_SRCHOSTMASK);
ENADDR  (alenov_hstmsk1, alenov_hstmsk1set, alenov_mask1,
	 OBJ(hwaddr,2), "Source host mask");

EVAL	(alenov_addr1set, alenov_hstmsk1,
	 GETOBJ(int,3) |= XNS_SRCNET + XNS_SRCHOST);
NOVADDR	(alenov_addr1, alenov_addr1set, alenov_net1,
	 OBJ(paddr,1), "Source net.host address");

/* Get protocol number */
EVAL    (alenov_proto_set, alenov_addr1, GETOBJ(int,3) |= XNS_PROTOCOL);
KEYWORD_ID (alenov_spx, alenov_proto_set, no_alt,
	OBJ(int,6), NOVELL_TYPE_SPX, "spx", 
	"Sequenced Packet Exchange", PRIV_CONF);
KEYWORD_ID (alenov_sap, alenov_proto_set, alenov_spx,
	OBJ(int,6), NOVELL_TYPE_SAPRESP, "sap", 
	"Service Advertising Protocol", PRIV_CONF);
KEYWORD_ID (alenov_rip, alenov_proto_set, alenov_sap,
	OBJ(int,6), NOVELL_TYPE_RIP, "rip", 
	"IPX Routing Information Protocol", PRIV_CONF);
KEYWORD_ID (alenov_ncp, alenov_proto_set, alenov_rip,
	OBJ(int,6), NOVELL_TYPE_NCP, "ncp", 
	"NetWare Core Protocol", PRIV_CONF);
KEYWORD_ID (alenov_netbios, alenov_proto_set, alenov_ncp,
	OBJ(int,6), NOVELL_TYPE_NETBIOS, "netbios", 
	"IPX NetBIOS", PRIV_CONF);
KEYWORD_ID (alenov_proto_any, alenov_proto_set, alenov_netbios, OBJ(int,6),
	 NOVELL_TYPE_ANY_PROT, "any", "Any IPX protocol type", PRIV_CONF);
KEYWORD_ID (alenov_proto_neg, alenov_proto_set, alenov_proto_any, OBJ(int,6),
         -1L, "-1", "Any IPX protocol type", PRIV_CONF | PRIV_HIDDEN);
NUMBER	(alenov_proto, alenov_proto_set, alenov_proto_neg, OBJ(int,6), 0, 255,
	 "Protocol type number (DECIMAL)");

/* Permit & Deny keywords */
KEYWORD_ID(alenov_permit, alenov_proto, no_alt,
	   OBJ(int,2), TRUE,
	   "permit", "Specify packets to permit", PRIV_CONF);

KEYWORD_ID(alenov_deny, alenov_proto, alenov_permit,
	   OBJ(int,2), FALSE,
	   "deny", "Specify packets to reject", PRIV_CONF);

NOPREFIX(alenov_noprefix, alenov_deny, alenov_eol);

EVAL	(alenov_modify, alenov_noprefix, /* cvt to old number */
	 GETOBJ(int,1) = GETOBJ(int,1) - IPX_MIN_EXT_ACL + MINENOVACC);
NUMBER	(alenov_num, alenov_modify, NONE,
	 OBJ(int,1), IPX_MIN_EXT_ACL, IPX_MAX_EXT_ACL,
	 "IPX extended access list");
NVGENS	(alenov_nvgen, alenov_num, noveaccess_command, LIST_NOVELL2);
NOP	(access_list_ext_novell, alenov_nvgen, access_list_sap);

/*
 * Old style "[no] access-list ..."
 *
 * This section should be deleted in a future release.
 */
NUMBER	(oalenov_num, alenov_noprefix, NONE,
	 OBJ(int,1), MINENOVACC, MAXENOVACC,
	 "IPX extended access list");
NVGENS	(oalenov_nvgen, oalenov_num, noveaccess_command, LIST_NOVELL2);
NOP	(oaccess_list_ext_novell, oalenov_nvgen, oaccess_list_sap);

/*****************************************************************
 * Novell standard access list:
 *
 * access-list <800-899> { permit | deny } <net> [[.<host> [<mask>]]
 *	[<net> [.<host> [<mask>]]]
 *
 * This translates into the following state diagram.
 *
 *                                                              |<mask>--EOLS
 *                         |permit-\ |<addr>--<mask>-\ |<addr>--|
 * access-list--<800-899>--|        -|      \----------|        |EOLS
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
EOLS	(alsnov_eol, novnaccess_command, LIST_NOVELL1);

/* Check for destination net address. First, check for an
 * unsigned hexadecimal number, then check for "-1".
 */
EVAL	(alsnov_net2set, alsnov_eol, GETOBJ(int,3) |= XNS_DSTNET);
NEG1	(alsnov_net2neg, alsnov_net2set, alsnov_eol, OBJ(int,5),
	 "Any IPX net");
HEXNUM	(alsnov_net2, alsnov_net2set, alsnov_net2neg,
	 OBJ(int,5), "Destination net");

/* Check for a destination Novell address and mask.
 * The mask is retrieved by the NOVHOST macro while the <net.host> must
 * be parsed by NOVADDR, due to the difference in the representation
 * of XNS and Novell network addresses.
 */
EVAL	(alsnov_mask2set, alsnov_eol, GETOBJ(int,3) |= XNS_DSTHOSTMASK);
NOVHOST	(alsnov_mask2, alsnov_mask2set, alsnov_eol,
	 OBJ(paddr,4), "Destination host mask");
EVAL	(alsnov_addr2set, alsnov_mask2,
	 GETOBJ(int,3) |= XNS_DSTNET + XNS_DSTHOST);
NOVADDR	(alsnov_addr2, alsnov_addr2set, alsnov_net2,
	 OBJ(paddr,3), "Destination net.host address");

/* Check for a source net address. First, check for an
 * unsigned hexadecimal number, then check for "-1".
 */
EVAL	(alsnov_net1set, alsnov_addr2, GETOBJ(int,3) |= XNS_SRCNET);

NEG1	(alsnov_net1neg, alsnov_net1set, no_alt, OBJ(int,4),
	 "Any IPX net");
HEXNUM (alsnov_net1, alsnov_net1set, alsnov_net1neg,
	 OBJ(int,4), "Source net");

/* Check for a full Novell source address and mask.
 * The mask is retrieved by the NOVHOST macro while the <net.host> must
 * be parsed by NOVADDR, due to the difference in the representation
 * of XNS and Novell network addresses.
 */
EVAL	(alsnov_mask1set, alsnov_addr2, GETOBJ(int,3) |= XNS_SRCHOSTMASK);
NOVHOST	(alsnov_mask1, alsnov_mask1set, alsnov_addr2,
	 OBJ(paddr,2), "Source host mask");
EVAL	(alsnov_addr1set, alsnov_mask1,
	 GETOBJ(int,3) |= XNS_SRCNET + XNS_SRCHOST);
NOVADDR	(alsnov_addr1, alsnov_addr1set, alsnov_net1,
	 OBJ(paddr,1), "Source net.host address");

/* Permit & Deny keywords */
KEYWORD_ID(alsnov_permit, alsnov_addr1, no_alt,
	   OBJ(int,2), TRUE,
	   "permit", "Specify packets to permit", PRIV_CONF);

KEYWORD_ID(alsnov_deny, alsnov_addr1, alsnov_permit,
	   OBJ(int,2), FALSE,
	   "deny", "Specify packets to reject", PRIV_CONF);

NOPREFIX(alsnov_noprefix, alsnov_deny, alsnov_eol);

EVAL	(alsnov_modify, alsnov_noprefix,
	 GETOBJ(int,1) = GETOBJ(int,1) - IPX_MIN_STD_ACL + MINNOVACC);
NUMBER	(alsnov_num, alsnov_modify, NONE,
	 OBJ(int,1), IPX_MIN_STD_ACL, IPX_MAX_STD_ACL,
	 "IPX standard access list");
NVGENS	(alsnov_nvgen, alsnov_num, novnaccess_command, LIST_NOVELL1);
NOP	(access_list_std_novell, alsnov_nvgen, access_list_ext_novell);
KEYWORD (access_list_novell, access_list_std_novell, ALTERNATE,
	 "access-list", "IPX Access lists",
	 PRIV_CONF | PRIV_HIDDEN | PRIV_NONVGEN);

#undef	ALTERNATE
#define	ALTERNATE	access_list_novell

/*
 * Old style "[no] access-list ..."
 *
 * These remaining lines should be deleted in a future release.
 */
NUMBER	(oalsnov_num, alsnov_noprefix, NONE,
	 OBJ(int,1), MINNOVACC, MAXNOVACC,
	 "IPX standard access list");
NVGENS	(oalsnov_nvgen, oalsnov_num, novnaccess_command, LIST_NOVELL1);
NOP	(oaccess_list_std_novell, oalsnov_nvgen, oaccess_list_ext_novell);

#undef	OALTERNATE
#define	OALTERNATE	oaccess_list_std_novell
