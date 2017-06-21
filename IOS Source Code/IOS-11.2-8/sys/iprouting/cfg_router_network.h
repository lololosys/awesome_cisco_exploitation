/* $Id: cfg_router_network.h,v 3.2.60.1 1996/08/12 16:03:46 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_network.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ N E T W O R K . H
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_network.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:46  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:32:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  08:30:52  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.1  1995/07/01  12:15:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * [no] network <address> [passive]
 * [no] network <address> <mask> area <area-id>			(OSPF only)
 *
 * The <area-id> can be in either IP address or decimal number format.  We
 * use the DECIMAL construct, which will also allow hex or octal numeric
 * formats.
 *
 * (*OBJ(paddr,1)) = <address>
 * (*OBJ(paddr,2)) = <mask>
 * (*OBJ(paddr,3)) = <area-id> if in IP address format
 *
 * OBJ(int,1) = <area-id>
 * OBJ(int,3) = TRUE if passive
 *
 * The parse chain is a bit strange due to the ASSERTs.  Each ASSERT pushes
 * its alternate before performing the test.  The parse macros following
 * each ASSERT must have alternates of NONE.  The ASSERT's alternate will
 * be taken after the parse macros it is protecting have executed.  A
 * better mechanism would be to have the csb->prototype variable contain
 * info about the type of protocol so that we can use a TESTVAR macro.
 */

EOLNS	(rtr_network_eol, network_command);

/* ALL OTHERS (not OSPF and not BGP) [passive] */
KEYWORD_ID (rtr_network_passive, rtr_network_eol, rtr_network_eol,
	    OBJ(int,3), TRUE,
	    "passive", "", PRIV_CONF | PRIV_HIDDEN);

/* OSPF ONLY <ospf-mask> area <area-id> */
NUMBER(rtr_network_areaid_num, rtr_network_eol, no_alt, 
	OBJ(int,1), 0, -1, "OSPF area ID as a decimal value");
IPADDR	(rtr_network_areaid, rtr_network_eol, rtr_network_areaid_num,
	 OBJ(paddr,8), "OSPF area ID in IP address format");
KEYWORD (rtr_network_area, rtr_network_areaid, no_alt,
	 "area", "Set the OSPF area ID", PRIV_CONF);

/* OSPF ONLY <mask> */
IPADDR	(rtr_network_ospf, rtr_network_area, no_alt,
	 OBJ(paddr,2), "OSPF wild card bits");

IFELSE (rtr_network_ospf_test, rtr_network_ospf, rtr_network_passive,
	((pdbtype *)(csb->protocol))->proctype & PROC_OSPF);

/* <address> */
IPADDR	(rtr_network_net, rtr_network_ospf_test, no_alt,
	 OBJ(paddr,1), "Network number");

NVGENNS	(rtr_network_nvgen, rtr_network_net, network_command);
KEYWORD	(rtr_networkkwd, rtr_network_nvgen, NONE,
	 "network", "Enable routing on an IP network", PRIV_CONF);

/* We only allow the 'network' keyword if an IP protocol was specified.
 * This is counter to the standard policy of not hiding keywords, but
 * in this case, the ambiguity between 'net' and 'network' is difficult
 * to handle.  Since both keywords perform a nearly identical function,
 * this is not seen as a major problem.
 */
ASSERT	(router_network, rtr_networkkwd, ALTERNATE,
	((((clns_pdbtype *)csb->protocol)->family == PDB_IP) &&
	 !(((pdbtype *)(csb->protocol))->proctype & PROC_BGP)));

#undef	ALTERNATE
#define ALTERNATE	router_network
