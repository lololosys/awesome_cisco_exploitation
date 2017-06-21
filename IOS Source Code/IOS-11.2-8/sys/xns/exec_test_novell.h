/* $Id: exec_test_novell.h,v 3.4.20.2 1996/08/16 00:14:32 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_test_novell.h,v $
 *------------------------------------------------------------------
 * E X E C _ T E S T _ N O V E L L. H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_test_novell.h,v $
 * Revision 3.4.20.2  1996/08/16  00:14:32  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/05/03  02:16:38  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.4  1996/02/01  23:35:14  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1995/12/01  06:24:57  mschaef
 * CSCdi37234:  Allow fastswitching of certain directed broadcast packets
 *
 * Revision 3.2  1995/11/17  19:21:45  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:38:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/25  14:11:21  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi39328]
 *
 * Revision 2.1  1995/06/07 23:27:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(test_novell_extend_here, no_alt);

/******************************************************************
 * test ipx capacity <routes> <startnet> <next hop net> <servers>
 *
 * Create N static routes, and one server for each. Put them as
 * existing next hop off of the network number given. If routes > servers,
 * only the fist 'servers' routes get a server. If servers > routes,
 * the last route gets all the remaining services.
 *
 * OBJ(int,1) total number of routes
 * OBJ(int,2) network number to start with. 
 * OBJ(int,3) network number to attach routes to.
 * OBJ(int,4) total number of servers
 */
EOLS (priv_test_nov_cap_eol, test_novell, TEST_NOVELL_CAPACITY);
NUMBER (priv_test_nov_cap_servers, priv_test_nov_cap_eol, no_alt,
	OBJ(int,4), 0, 100000, "servers");
HEXDIGIT (priv_test_nov_cap_nexthop_net, priv_test_nov_cap_servers, no_alt,
	  OBJ(int,3), NOV_MIN_NET, NOV_MAX_NET,
	  "IPX network as route next hop");
HEXDIGIT (priv_test_nov_cap_routes_start, priv_test_nov_cap_nexthop_net,
	  no_alt, OBJ(int,2), NOV_MIN_NET, NOV_MAX_NET,
	  "IPX network to start with");
NUMBER (priv_test_nov_cap_routes, priv_test_nov_cap_routes_start, no_alt,
	OBJ(int,1), 1, 100000, "routes");
KEYWORD (priv_test_nov_cap, priv_test_nov_cap_routes, test_novell_extend_here,
	 "capacity", "IPX Capacity test", PRIV_OPR);

/******************************************************************
 * test ipx debug
 * [<ulong1> [<ulong2> [<ulong3> [<ulong4> [<ulong5> [<net.host>]]]]]]
 */

EOLS	(priv_test_nov_dbg_eol, test_novell, TEST_NOVELL_DEBUG);

NOVADDR	(priv_test_nov_dbg_get_addr2, priv_test_nov_dbg_eol,
	 priv_test_nov_dbg_eol, OBJ(paddr,2), "net2.host2");
NOVADDR	(priv_test_nov_dbg_get_addr1, priv_test_nov_dbg_get_addr2,
	 priv_test_nov_dbg_eol, OBJ(paddr,1), "net1.host1");

HEXNUM  (priv_test_nov_dbg_get8, priv_test_nov_dbg_get_addr1,
	 priv_test_nov_dbg_eol, OBJ(int,8), "8");
HEXNUM  (priv_test_nov_dbg_get7, priv_test_nov_dbg_get8,
	 priv_test_nov_dbg_eol, OBJ(int,7), "7");
HEXNUM  (priv_test_nov_dbg_get6, priv_test_nov_dbg_get7,
	 priv_test_nov_dbg_eol, OBJ(int,6), "6");
HEXNUM  (priv_test_nov_dbg_get5, priv_test_nov_dbg_get6,
	 priv_test_nov_dbg_eol, OBJ(int,5), "5");
HEXNUM  (priv_test_nov_dbg_get4, priv_test_nov_dbg_get5,
	 priv_test_nov_dbg_eol, OBJ(int,4), "4");
HEXNUM  (priv_test_nov_dbg_get3, priv_test_nov_dbg_get4,
	 priv_test_nov_dbg_eol, OBJ(int,3), "3");
HEXNUM  (priv_test_nov_dbg_get2, priv_test_nov_dbg_get3,
	 priv_test_nov_dbg_eol, OBJ(int,2), "2");
HEXNUM  (priv_test_nov_dbg_get1, priv_test_nov_dbg_get2,
	 priv_test_nov_dbg_eol, OBJ(int,1), "1");

SET     (priv_test_nov_dbg_set8, priv_test_nov_dbg_get1, OBJ(int,8), 0);
SET     (priv_test_nov_dbg_set7, priv_test_nov_dbg_set8, OBJ(int,7), 0);
SET     (priv_test_nov_dbg_set6, priv_test_nov_dbg_set7, OBJ(int,6), 0);
SET     (priv_test_nov_dbg_set5, priv_test_nov_dbg_set6, OBJ(int,5), 0);
SET     (priv_test_nov_dbg_set4, priv_test_nov_dbg_set5, OBJ(int,4), 0);
SET     (priv_test_nov_dbg_set3, priv_test_nov_dbg_set4, OBJ(int,3), 0);
SET     (priv_test_nov_dbg_set2, priv_test_nov_dbg_set3, OBJ(int,2), 0);
SET     (priv_test_nov_dbg_set1, priv_test_nov_dbg_set2, OBJ(int,1), 0);

KEYWORD (priv_test_nov_dbg, priv_test_nov_dbg_set1, priv_test_nov_cap,
	 "debug", "1..5", PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * test ipx echo net.host
 *      [<count> [<interval> [<lower> [<upper> [<csum> [<method> [<seed>]]]]]]]
 */
EOLS	(priv_test_nov_echo_eol, test_novell, TEST_NOVELL_ECHO);

HEXNUM  (priv_test_nov_echo_get_tc,
	 priv_test_nov_echo_eol, priv_test_nov_echo_eol,
	 OBJ(int,8), "initial tc value");
HEXNUM  (priv_test_nov_echo_get_seed,
	 priv_test_nov_echo_get_tc, priv_test_nov_echo_eol,
	 OBJ(int,7), "initial seed value");
NUMBER  (priv_test_nov_echo_get_how,
	 priv_test_nov_echo_get_seed, priv_test_nov_echo_eol,
	 OBJ(int,6), 1, 3,
	 "fill method (1=sequential, 2=pseudo-random, 3=fixed)");
NUMBER  (priv_test_nov_echo_get_csum,
	 priv_test_nov_echo_get_how, priv_test_nov_echo_eol,
	 OBJ(int,5), 0, 0xFFFF, "checksum");
NUMBER  (priv_test_nov_echo_get_hi,
	 priv_test_nov_echo_get_csum, priv_test_nov_echo_eol,
	 OBJ(int,4), XNS_MINECHOSIZE, 0x10000, "datagram size upper bound");
NUMBER  (priv_test_nov_echo_get_lo,
	 priv_test_nov_echo_get_hi, priv_test_nov_echo_eol,
	 OBJ(int,3), XNS_MINECHOSIZE, 0x10000, "datagram size lower bound)");
NUMBER  (priv_test_nov_echo_get_ival,
	 priv_test_nov_echo_get_lo, priv_test_nov_echo_eol,
	 OBJ(int,2), 0, 60000, "interval (mSecs)");
NUMBER  (priv_test_nov_echo_get_cnt,
	 priv_test_nov_echo_get_ival, priv_test_nov_echo_eol,
	 OBJ(int,1), 1, -1, "count");
NOVADDR	(priv_test_nov_echo_get_addr, priv_test_nov_echo_get_cnt, no_alt,
	 OBJ(paddr,1), "Address (N.HHHH.HHHH.HHHH)");

SET     (priv_test_nov_echo_set_tc, priv_test_nov_echo_get_addr,
	 OBJ(int,8), 0);
SET     (priv_test_nov_echo_set_seed, priv_test_nov_echo_set_tc,
	 OBJ(int,7), 42424242);
SET     (priv_test_nov_echo_set_how, priv_test_nov_echo_set_seed,
	 OBJ(int,6), 2);
SET     (priv_test_nov_echo_set_csum, priv_test_nov_echo_set_how,
	 OBJ(int,5), 0xFFFF);
SET     (priv_test_nov_echo_set_hi, priv_test_nov_echo_set_csum,
	 OBJ(int,4), 1492);
SET     (priv_test_nov_echo_set_lo, priv_test_nov_echo_set_hi,
	 OBJ(int,3), 46);
SET     (priv_test_nov_echo_set_ival, priv_test_nov_echo_set_lo,
	 OBJ(int,2), 1000);
SET     (priv_test_nov_echo_set_cnt, priv_test_nov_echo_set_ival,
	 OBJ(int,1), 1000);

KEYWORD (priv_test_nov_echo, priv_test_nov_echo_set_cnt, priv_test_nov_dbg,
	 "echo", "automated echo test", PRIV_OPR);

/******************************************************************
 * test ipx netbios find
 *        [<name> [<count> [<interval> [<net> [<net0> ... <net7>]]]]]
 */
EOLS	(priv_test_nov_nb_fn_eol, test_novell, TEST_NOVELL_NETBIOS_FINDNAME);
HEXNUM  (priv_test_nov_nb_fn_get_net7,
	 priv_test_nov_nb_fn_eol, priv_test_nov_nb_fn_eol,
	 OBJ(int,11), "net7");
HEXNUM  (priv_test_nov_nb_fn_get_net6,
	 priv_test_nov_nb_fn_get_net7, priv_test_nov_nb_fn_eol,
	 OBJ(int,10), "net6");
HEXNUM  (priv_test_nov_nb_fn_get_net5,
	 priv_test_nov_nb_fn_get_net6, priv_test_nov_nb_fn_eol,
	 OBJ(int,9), "net5");
HEXNUM  (priv_test_nov_nb_fn_get_net4,
	 priv_test_nov_nb_fn_get_net5, priv_test_nov_nb_fn_eol,
	 OBJ(int,8), "net4");
HEXNUM  (priv_test_nov_nb_fn_get_net3,
	 priv_test_nov_nb_fn_get_net4, priv_test_nov_nb_fn_eol,
	 OBJ(int,7), "net3");
HEXNUM  (priv_test_nov_nb_fn_get_net2,
	 priv_test_nov_nb_fn_get_net3, priv_test_nov_nb_fn_eol,
	 OBJ(int,6), "net2");
HEXNUM  (priv_test_nov_nb_fn_get_net1,
	 priv_test_nov_nb_fn_get_net2, priv_test_nov_nb_fn_eol,
	 OBJ(int,5), "net1");
HEXNUM  (priv_test_nov_nb_fn_get_net0,
	 priv_test_nov_nb_fn_get_net1, priv_test_nov_nb_fn_eol,
	 OBJ(int,4), "net0");
HEXNUM  (priv_test_nov_nb_fn_get_net,
	 priv_test_nov_nb_fn_get_net0, priv_test_nov_nb_fn_eol,
	 OBJ(int,3), "net");
NUMBER  (priv_test_nov_nb_fn_int,
	 priv_test_nov_nb_fn_get_net, priv_test_nov_nb_fn_eol,
	 OBJ(int,2), 0, 60000, "interval (mSecs)");
NUMBER  (priv_test_nov_nb_fn_cnt,
	 priv_test_nov_nb_fn_int, priv_test_nov_nb_fn_eol,
	 OBJ(int,1), 1, -1, "count");
STRING  (priv_test_nov_nb_fn_str,
	 priv_test_nov_nb_fn_cnt, priv_test_nov_nb_fn_eol,
	 OBJ(string,1), "name string");
SET     (priv_test_nov_nb_fn_set_int, priv_test_nov_nb_fn_str,
	 OBJ(int,2), 2000);
SET     (priv_test_nov_nb_fn_set_cnt, priv_test_nov_nb_fn_set_int,
	 OBJ(int,1), 1);
KEYWORD (priv_test_nov_nb_fn, priv_test_nov_nb_fn_set_cnt, no_alt,
	 "find", "find name", PRIV_OPR);

KEYWORD (priv_test_nov_nb, priv_test_nov_nb_fn, priv_test_nov_echo,
	 "netbios", "netbios pkt gen", PRIV_OPR);

/******************************************************************
 * test ipx watchdog net.host
 */
EOLS	(priv_test_nov_wd_fn_eol, test_novell, TEST_NOVELL_WATCHDOG);
NOVADDR	(priv_test_nov_wd_get_addr, priv_test_nov_wd_fn_eol, no_alt,
	 OBJ(paddr,1), "Address (N.HHHH.HHHH.HHHH)");
KEYWORD (priv_test_nov_wd, priv_test_nov_wd_get_addr, priv_test_nov_nb,
	 "watchdog", "automated watchdog query generation", PRIV_OPR);

/******************************************************************
 * test ipx gns [<type> [<count> [<interval> [<net>]]]]
 */
EOLS	(priv_test_nov_gns_eol, test_novell, TEST_NOVELL_GNS);
HEXNUM  (priv_test_nov_gns_get_net,
	 priv_test_nov_gns_eol, priv_test_nov_gns_eol,
	 OBJ(int,4), "net");
NUMBER  (priv_test_nov_gns_get_int,
	 priv_test_nov_gns_get_net, priv_test_nov_gns_eol,
	 OBJ(int,3), 0, 60000, "interval (mSecs)");
NUMBER  (priv_test_nov_gns_get_cnt,
	 priv_test_nov_gns_get_int, priv_test_nov_gns_eol,
	 OBJ(int,2), 1, -1, "count");
HEXDIGIT(priv_test_nov_gns_get_typ,
	 priv_test_nov_gns_get_cnt, priv_test_nov_gns_eol,
	 OBJ(int,1), 0, 0xFFFF, "service type");
SET     (priv_test_nov_gns_set_net, priv_test_nov_gns_get_typ, OBJ(int,4), -1);
SET     (priv_test_nov_gns_set_int, priv_test_nov_gns_set_net, OBJ(int,3), 2000);
SET     (priv_test_nov_gns_set_cnt, priv_test_nov_gns_set_int, OBJ(int,2),  1);
SET     (priv_test_nov_gns_set_typ, priv_test_nov_gns_set_cnt, OBJ(int,1),  4);
KEYWORD	(priv_test_nov_gns, priv_test_nov_gns_set_typ, priv_test_nov_wd,
	 "gns", "get nearest server", PRIV_OPR);

/******************************************************************
 * test ipx query [<query type> [<type> [<name> [<net> [<mask>[<count> [<interval> [<net>]]]]]]]]
 */
EOLS	(priv_test_nov_name_eol, test_novell, TEST_NOVELL_NAME);
HEXNUM  (priv_test_nov_name_get_net,
	 priv_test_nov_name_eol, priv_test_nov_name_eol,
	 OBJ(int,7), "net to send to");
NUMBER  (priv_test_nov_name_get_int,
	 priv_test_nov_name_get_net, priv_test_nov_name_eol,
	 OBJ(int,6), 0, 60000, "interval (mSecs)");
NUMBER  (priv_test_nov_name_get_cnt,
	 priv_test_nov_name_get_int, priv_test_nov_name_eol,
	 OBJ(int,5), 1, -1, "count");
HEXNUM  (priv_test_nov_name_get_dmask,
	 priv_test_nov_name_get_cnt, priv_test_nov_name_eol,
	 OBJ(int,4), "network mask");
HEXNUM  (priv_test_nov_name_get_dnet,
	 priv_test_nov_name_get_dmask, priv_test_nov_name_eol,
	 OBJ(int,3), "service network");
STRING  (priv_test_nov_name_str,
         priv_test_nov_name_get_dnet, priv_test_nov_name_eol,
         OBJ(string,1), "Server name");
HEXDIGIT(priv_test_nov_name_get_typ,
	 priv_test_nov_name_str, priv_test_nov_name_eol,
	 OBJ(int,2), 0, 0xFFFF, "service type");
HEXNUM  (priv_test_nov_name_get_qry,
	 priv_test_nov_name_get_typ, priv_test_nov_name_eol,
	 OBJ(int,1), "query type");
SET     (priv_test_nov_name_set_net, priv_test_nov_name_get_qry, OBJ(int,7), -1);
SET     (priv_test_nov_name_set_int, priv_test_nov_name_set_net, OBJ(int,6), 2000);
SET     (priv_test_nov_name_set_cnt, priv_test_nov_name_set_int, OBJ(int,5),  1);
SET     (priv_test_nov_name_set_dmask, priv_test_nov_name_set_cnt, OBJ(int,4),
	 NOVELL_EVERYONE);
SET     (priv_test_nov_name_set_dnet, priv_test_nov_name_set_dmask, OBJ(int,3),
	 NOVELL_EVERYONE);
SET     (priv_test_nov_name_set_typ, priv_test_nov_name_set_dnet, OBJ(int,2),
	 NOVELL_SAP_FILESERVER);
SET     (priv_test_nov_name_set_qry, priv_test_nov_name_set_typ, OBJ(int,1), 
	 NOVELL_SAP_NEARNAMEQUERY);
KEYWORD	(priv_test_nov_name, priv_test_nov_name_set_qry, priv_test_nov_gns,
	 "query", "send a sap query", PRIV_OPR);

/******************************************************************
 * test ipx ripreq [net]
 */
EOLS	(priv_test_nov_ripreq_eol, test_novell, TEST_NOVELL_RIPREQ);
HEXNUM  (priv_test_nov_ripreq_net, priv_test_nov_ripreq_eol, no_alt, 
	OBJ(int,1), "net");
KEYWORD (priv_test_nov_ripreq, priv_test_nov_ripreq_net, priv_test_nov_name,
	"ripreq", "rip request", PRIV_OPR);

NOVELL_IPX_KEYWORD (priv_test_nov, priv_test_nov_ripreq, ALTERNATE,
		    "Novell/IPX test commands", PRIV_OPR|PRIV_HIDDEN);
#undef	ALTERNATE
#define	ALTERNATE	priv_test_nov
