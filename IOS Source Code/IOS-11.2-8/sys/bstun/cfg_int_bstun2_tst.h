/* $Id: cfg_int_bstun2_tst.h,v 3.2.8.1 1996/05/21 09:43:23 thille Exp $
 * $Source: /release/112/cvs/Xsys/bstun/cfg_int_bstun2_tst.h,v $
 *------------------------------------------------------------------
 * Test version of cfg_int_bstun2.h BSTUN interface config commands
 *
 * Oct 95. Saritha Roach
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_bstun2_tst.h,v $
 * Revision 3.2.8.1  1996/05/21  09:43:23  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1996/02/29  20:34:51  jbalestr
 * CSCdi48520:  BSTUN route command parses DIRECT
 * stops invalid configuration
 *
 * Revision 3.1  1995/11/09  11:04:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/10  07:56:39  sroach
 * CSCdi41879:  Add BSC test command support
 *
 * Revision 2.1  1995/10/10  05:42:19  sroach
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] bstun route { all | address <address> }
 *	{ interface serial <number> |
 *	  tcp <ipaddr> [ priority | input-address <bstunaddr> ] |
 *        test }
 *
 * OBJ(int,1) = BSTUN_RTALL if forwarding all addresses
 *         = BSTUN_RTADDR if forwarding only specific address
 *
 * OBJ(int,2) = BSTUN_INTF if configuring a BSTUN serial interface transport
 *            = BSTUN_TCP if using TCP transport
 *            = BSTUN_TEST if using TEST interface
 *
 * if_name = interface name & number
 *
 * (*OBJ(paddr,1)) = BSTUN address (if not 'all')
 * (*OBJ(paddr,2)) = IP address (if tcp)
 *
 */

EOLS	(ci_bstun_rteol, bstunif_command, BSTUNIF_ROUTE);

/* test */
KEYWORD_ID(ci_bstun_rttest, ci_bstun_rteol, no_alt,
	   OBJ(int,2), BSTUN_TEST,
	   "test", "Interface is to a test script", PRIV_CONF);

/* tcp options */
pdecl( ci_bstun_tcpopts );
NUMBER  (ci_bstun_tcpopts_qmax_val, ci_bstun_tcpopts, no_alt,
         OBJ(int,6), 10, 500,
         "Maximum TCP driver queue length (length is from 10 to 500)");
KEYWORD (ci_bstun_tcpopts_qmax, ci_bstun_tcpopts_qmax_val, no_alt,
         "tcp-queue-max", "Set the max depth of the TCP driver queue", PRIV_CONF);
BSTUNADDR(ci_bstun_tcpopts_inaddr_addr, ci_bstun_tcpopts, no_alt,
	 OBJ(paddr,3), "BSTUN address" );
KEYWORD_ID(ci_bstun_tcpopts_inaddr, ci_bstun_tcpopts_inaddr_addr, 
           ci_bstun_tcpopts_qmax,
	   OBJ(int,5), TRUE,
	   "input-address", "", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_bstun_tcpopts_priority, ci_bstun_tcpopts, ci_bstun_tcpopts_inaddr,
           OBJ(int,4), TRUE,
           "priority", "Enable priority queueing for BSC frames", PRIV_CONF );
NOP	(ci_bstun_tcpopts, ci_bstun_tcpopts_priority, ci_bstun_rteol );

/* tcp <ipaddr> */
IPADDR	(ci_bstun_rttcpaddr, ci_bstun_tcpopts, no_alt,
	 OBJ(paddr,2), "Remote BSTUN peer's IP address");
KEYWORD_ID(ci_bstun_rttcp, ci_bstun_rttcpaddr, ci_bstun_rttest,
	   OBJ(int,2), BSTUN_TCP,
	   "tcp", "Use TCP transport to propagate frames", PRIV_CONF);

/* interface serial <number>
 * (The existing (9.0) parser allows "interface serial0"
 * because it uses the 'get_interface()' function.)
 */
/* [direct] */
KEYWORD_ID(ci_bstun_rtifdirect, ci_bstun_rteol, ci_bstun_rteol,
	   OBJ(int,3), TRUE,
	   "direct", "Interface is a direct BSTUN link", PRIV_CONF);
INTERFACE (ci_bstun_rtifname, ci_bstun_rtifdirect, no_alt, OBJ(idb,1), IFTYPE_SERIAL);
KEYWORD_ID(ci_bstun_rtif, ci_bstun_rtifname, ci_bstun_rttcp,
	   OBJ(int,2), BSTUN_INTF,
	   common_str_interface, "Use the Serial Transport to propagate frames", PRIV_CONF);

/* address <address> */
BSTUNADDR (ci_bstun_rtaddrval, ci_bstun_rtif, no_alt,
	  OBJ(paddr,1), "BSTUN address");
KEYWORD_ID(ci_bstun_rtaddr, ci_bstun_rtaddrval, no_alt,
	   OBJ(int,1), BSTUN_RTADDR,
	   "address", "Set BSTUN address", PRIV_CONF);

/* all */
KEYWORD_ID(ci_bstun_rtall, ci_bstun_rtif, ci_bstun_rtaddr,
	   OBJ(int,1), BSTUN_RTALL,
	   "all", "Forward all frames", PRIV_CONF);

NVGENS	(ci_bstun_route_nvgen, ci_bstun_rtall, bstunif_command, BSTUNIF_ROUTE);
KEYWORD	(ci_bstun_route, ci_bstun_route_nvgen, no_alt,
	 "route", "Set forwarding of BSTUN frames", PRIV_CONF);

KEYWORD	(ci_bstun1, ci_bstun_route, NONE, "bstun",
	 "BSTUN interface subcommands", PRIV_CONF|PRIV_DUPLICATE);

ASSERT	(ci_bstun1_test, ci_bstun1, ALTERNATE,
	 (csb->interface->hwptr->status & IDB_SERIAL));

#undef  ALTERNATE
#define ALTERNATE ci_bstun1_test
