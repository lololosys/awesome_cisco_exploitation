/* $Id: exec_show_novell.h,v 3.3.20.8 1996/08/16 00:14:25 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_show_novell.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ N O V E L L . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_novell.h,v $
 * Revision 3.3.20.8  1996/08/16  00:14:25  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.3.20.7  1996/08/12  16:10:06  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.6  1996/08/07  01:21:09  akr
 * CSCdi65329:  show ipx access-list doesnt work for all lists
 * Branch: California_branch
 *
 * Revision 3.3.20.5  1996/06/24  22:31:32  perryl
 * CSCdi57969:  show ip interface not showing Vaccess
 * Branch: California_branch
 *
 * Revision 3.3.20.4  1996/05/30  23:48:09  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3.20.3  1996/05/21  10:11:18  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.20.2  1996/05/03  02:16:31  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.3.20.1  1996/04/02  05:37:59  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.3  1996/02/01  23:35:11  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  19:21:41  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:38:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/25  02:58:42  john
 * CSCdi39233:  Show IPX Server unsorted is sorted
 *
 * Revision 2.1  1995/06/07  23:27:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(show_novell_extend_here, no_alt);

/******************************************************************
 * show ipx spx-spoof
 */
EOLS	(show_novell_spx_spoof_eol, show_novell, SHOW_NOVELL_SPX_SPOOF);
KEYWORD (show_novell_spx_spoof, show_novell_spx_spoof_eol,
	 show_novell_extend_here, "spx-spoof", "SPX Spoofing table",
	 PRIV_USER);

/******************************************************************
 * show ipx private cache-history <network>
 *
 */
EOLNS	(show_nov_priv_cache_eol, show_nov_cache_history);

HEXNUM  (show_nov_priv_cache_netw,
	 show_nov_priv_cache_eol, show_nov_priv_cache_eol,
	 OBJ(int,1), NULL /*"Trigger network"*/);

KEYWORD	(show_nov_priv_cache, show_nov_priv_cache_netw, no_alt,
	 "cache-history", NULL /*"Cache invalidation history"*/, PRIV_USER);

KEYWORD (show_nov_private, show_nov_priv_cache, show_novell_spx_spoof,
	 "private", NULL, PRIV_USER|PRIV_HIDDEN);

#ifdef TESTCMD
/******************************************************************
 * show ipx riptest {-1|<num>} [<num>]
 */
EOLS	(sn_riptest_eol, show_novell, SHOW_NOVELL_RIPTEST);
SET	(sn_riptest_num2_set, sn_riptest_eol, OBJ(int,4), TRUE);
NUMBER	(sn_riptest_num2, sn_riptest_num2_set, sn_riptest_eol,
	 OBJ(int,3), 0, MAXUINT, "repeat count");
HEXNUM	(sn_riptest_num1, sn_riptest_num2, no_alt,
	 OBJ(int,2), "to specified net");
KEYWORD_ID(sn_riptest_neg1, sn_riptest_num2, sn_riptest_num1,
	   OBJ(int,1), TRUE,
	   "-1", "to all nets", PRIV_USER);
KEYWORD (show_novell_riptest, sn_riptest_neg1, show_nov_private,
	"riptest", "Riptest", PRIV_USER);

/******************************************************************
 * show ipx neartest [<num>]
 */
EOLS	(sn_neartest_eol, show_novell, SHOW_NOVELL_NEARTEST);
HEXDIGIT(sn_neartest_num, sn_neartest_eol, sn_neartest_eol,
	 OBJ(int,1), 0, 0xFFFF, "service type");
KEYWORD (show_novell_neartest, sn_neartest_num, show_novell_riptest,
	"neartest", "Neartest", PRIV_USER);

/******************************************************************
 * show ipx querytest [-1|<num>]
 */
EOLS	(sn_querytest_eol, show_novell, SHOW_NOVELL_QUERYTEST);
HEXDIGIT(sn_querytest_number, sn_querytest_eol, sn_querytest_eol,
	 OBJ(int,2), 0, 0xFFFF, "service type");
KEYWORD_ID(sn_querytest_neg1, sn_querytest_eol, sn_querytest_number,
	   OBJ(int,1), TRUE,
	   "-1", "all services", PRIV_USER);
KEYWORD (show_novell_querytest, sn_querytest_neg1, show_novell_neartest,
	"querytest", "Querytest", PRIV_USER);

/******************************************************************
 * show ipx saptest <hex> <hex> <num> <string>
 */
EOLS	(sn_saptest_eol, show_novell, SHOW_NOVELL_SAPTEST);
STRING	(sn_saptest_server, sn_saptest_eol, no_alt,
	 OBJ(string,1), "server" );
NUMBER	(sn_saptest_hops, sn_saptest_server, no_alt,
	 OBJ(int,3), 0, MAXUINT, "hops");
HEXNUM	(sn_saptest_net, sn_saptest_hops, no_alt,
	 OBJ(int,2), "network");
HEXDIGIT(sn_saptest_type, sn_saptest_net, no_alt,
	 OBJ(int,1), 0, 0xFFFF, "type");
KEYWORD (show_novell_saptest, show_novell_test_eol, show_novell_querytest,
	"saptest", "Saptest", PRIV_USER);

#else	TESTCMD
NOP	(show_novell_saptest, show_nov_private, NONE);
#endif	TESTCMD


/******************************************************************
 * show ipx accounting [checkpoint]
 * 
 * OBJ(int,1) = TRUE if [checkpoint]
 */

EOLS	(show_novell_accounting_eol, show_ipxaccounting,
	 SHOW_NOVELL_ACCOUNTING);
KEYWORD_ID (show_novell_acc_check, show_novell_accounting_eol,
	    show_novell_accounting_eol, OBJ(int,1), TRUE,
	    "checkpoint", "The checkpointed IPX accounting database",
	    PRIV_USER);
KEYWORD (show_novell_acc, show_novell_acc_check, show_novell_saptest,
	 "accounting", "The active IPX accounting database", PRIV_USER);

/******************************************************************
 * show ipx traffic
 */
EOLS	(show_novell_traffic_eol, show_novell, SHOW_NOVELL_TRAFFIC);
KEYWORD (show_novell_traffic, show_novell_traffic_eol, show_novell_acc,
	"traffic", "IPX protocol statistics", PRIV_USER);

/******************************************************************
 * show ipx servers [detailed] [sorted [name | net | type] | unsorted]
 *
 * OBJ(int,1) = primary sort key:
 *
 *     0 = unsorted
 *     1 = name
 *     2 = net
 *     3 = type
 *
 * OBJ(int,2) = detail flag
 * OBJ(int,3) = regular expression 
 *
 */
EOLS	(show_novell_servers_eol, show_novell, SHOW_NOVELL_SERVERS);

STRING (show_novell_servers_regexp1, show_novell_servers_eol, no_alt,
	    OBJ(string,3), "Regular expression");

KEYWORD_ID (show_novell_servers_regexp,
	    show_novell_servers_regexp1, show_novell_servers_eol, 
	    OBJ(int,4), TRUE, "regexp",
	    "Display server list elements whose name matches a regular expression",
	    PRIV_USER);

KEYWORD_ID (show_novell_servers_type,
	    show_novell_servers_regexp, show_novell_servers_regexp,
	    OBJ(int,1), IPXSAP_SORT_BY_TYPE,
	    "type", "Sort by type", PRIV_USER);

KEYWORD_ID (show_novell_servers_net,
	    show_novell_servers_regexp, show_novell_servers_type,
	    OBJ(int,1), IPXSAP_SORT_BY_NET,
	    "net", "Sort by net", PRIV_USER);

KEYWORD_ID (show_novell_servers_name,
	    show_novell_servers_regexp, show_novell_servers_net,
	    OBJ(int,1), IPXSAP_SORT_BY_NAME,
	    "name", "Sort by name", PRIV_USER);

KEYWORD_ID (show_novell_servers_sorted,
	    show_novell_servers_name, show_novell_servers_regexp,
	    OBJ(int,1), IPXSAP_SORT_BY_TYPE, "sorted",
	    "Display server list in sorted order", PRIV_USER);

KEYWORD_ID (show_novell_servers_unsorted,
	    show_novell_servers_regexp, show_novell_servers_sorted,
	    OBJ(int,1), IPXSAP_SORT_BY_NONE, "unsorted",
	    "Display server list in internal order", PRIV_USER);

KEYWORD_ID (show_novell_servers_detail,
	    show_novell_servers_unsorted, show_novell_servers_unsorted,
	    OBJ(int,2), TRUE, "detailed",
	    "Comprehensive display including path detail", PRIV_USER);

SET	(show_novell_servers_set_dflt2, show_novell_servers_detail,
	 OBJ(int,1), IPXSAP_SORT_BY_TYPE);

SET	(show_novell_servers_set_dflt1, show_novell_servers_set_dflt2,
	 OBJ(int,2), FALSE);

KEYWORD	(show_novell_servers, show_novell_servers_set_dflt1,
	 show_novell_traffic, "servers", "SAP servers", PRIV_USER);

/******************************************************************
 * show novell urd [<net>]
 */
EOLS       (show_novell_urd_eol, show_novell, SHOW_NOVELL_URD);
HEXDIGIT   (show_novell_urd_net, show_novell_urd_eol, show_novell_urd_eol,
	    OBJ(int,1), NOV_MIN_NET, NOV_MAX_NET, "IPX network number");
KEYWORD_ID (show_novell_urd, show_novell_urd_net, show_novell_servers,
	    OBJ(int,1), 0, "urd", "IPX unreachable route/svc table",
	    PRIV_USER|PRIV_HIDDEN);

/******************************************************************
 * show ipx backup [<net>] [<mask>]
 */
EOLS	 (show_novell_bkup_eol, show_novell, SHOW_NOVELL_BACKUP);
HEXDIGIT (show_novell_bkup_mask, show_novell_bkup_eol, show_novell_bkup_eol,
	  OBJ(int,2), 0x80000000, 0xffffffff, "IPX network mask");
HEXDIGIT (show_novell_bkup_net, show_novell_bkup_mask, show_novell_bkup_eol,
	  OBJ(int,1), NOV_MIN_NET, NOV_MAX_NET, "IPX network number");
KEYWORD_ID (show_novell_backup, show_novell_bkup_net, show_novell_urd,
	    OBJ(int,1), 0, "backup", "IPX NLSP backup route table",
	    PRIV_USER|PRIV_HIDDEN);

/******************************************************************
 * show ipx route [detailed] [default | <net>]
 *
 * OBJ(int,1) = 0           => all nets
 * OBJ(int,1) = 1..FFFFFFFD => specific net
 * OBJ(int,1) = FFFFFFFF    => default routes only
 *
 * OBJ(int,2) = detail flag
 */
EOLS	(show_novell_rt_eol, show_novell, SHOW_NOVELL_ROUTE);
HEXDIGIT (show_novell_rt_net, show_novell_rt_eol, show_novell_rt_eol,
	  OBJ(int,1), NOV_MIN_NET, NOV_MAX_NET, "IPX network number");

KEYWORD_ID_MM(show_novell_rt_dflt, show_novell_rt_eol, show_novell_rt_net,
	      OBJ(int,1), -1,
	      "default", "Default route", PRIV_USER, 5);

KEYWORD_ID_MM(show_novell_rt_det, show_novell_rt_dflt, show_novell_rt_dflt,
	      OBJ(int,2), TRUE,
	      "detailed", "Comprehensive display", PRIV_USER, 3);

SET	(show_novell_rt_set2, show_novell_rt_det, OBJ(int,2), FALSE);
SET	(show_novell_rt_set1, show_novell_rt_set2, OBJ(int,1), 0);
KEYWORD_ID (show_novell_route, show_novell_rt_set1, show_novell_backup,
	    OBJ(int,1), 0, "route", "IPX routing table", PRIV_USER);

/******************************************************************
 * show ipx interface [<interface>]
 *
 * OBJ(idb,1) = <interface>
 * OBJ(int,2) = TRUE if BRIEF form wanted.
 */
EOLS	(show_novell_interface_eol, show_novell, SHOW_NOVELL_INTERFACE);
INTERFACE (show_novell_ifname, show_novell_interface_eol,
	   show_novell_interface_eol, 
	   OBJ(idb,1), (IFTYPE_ANYSUB | IFTYPE_VACCESS));
KEYWORD_ID_MM(show_novell_ifbrief, show_novell_ifname, show_novell_ifname,
	      OBJ(int,2), TRUE,
	      "brief",
	      "Brief summary of IPX interface status and configuration",
	      PRIV_USER, 4);
KEYWORD (show_novell_if, show_novell_ifbrief, show_novell_route,
	common_str_interface, "IPX interface status and configuration", PRIV_USER);

/******************************************************************
 * show ipx cache [cbus | hash-table]
 *
 * OBJ(int,1) = 1 => cbus keyword present
 *
 * At the present time, *cbus* command is undocumented and hidden
 */
EOLS	(show_nov_cache_eol, show_novell, SHOW_NOVELL_CACHE);
KEYWORD_ID(show_nov_cache_hash, show_nov_cache_eol, show_nov_cache_eol,
	   OBJ(int,1), 2,
	   "hash-table", "IPX fastswitch hash table", PRIV_USER|PRIV_HIDDEN);
KEYWORD_ID(show_nov_cache_cbus, show_nov_cache_eol, show_nov_cache_hash,
	   OBJ(int,1), 1,
	   "cbus", "IPX autonomous-switching cache", PRIV_USER|PRIV_HIDDEN);
KEYWORD_ID(show_nov_cache, show_nov_cache_cbus, show_novell_if,
	   OBJ(int,1), 0,
	   "cache", "IPX fast-switching cache", PRIV_USER);

/******************************************************************
 * show ipx access-lists [<list-number>]
 *
 * OBJ(int,1) = <list-number>
 */
EOLS	(show_nov_access_eol, show_novell, SHOW_NOVELL_ACCESS);
ACCESS_LIST(show_nov_access_list4, show_nov_access_eol, show_nov_access_eol,
	   OBJ(int,1), MINNOVSUM, MAXNOVSUM,
	   IPX_MIN_SUM_ACL, IPX_MAX_SUM_ACL, "Access list number");
ACCESS_LIST(show_nov_access_list3, show_nov_access_eol, show_nov_access_list4, 
	   OBJ(int,1), MINNOVSAPACC, MAXNOVSAPACC,
	   IPX_MIN_SAP_ACL, IPX_MAX_SAP_ACL, "Access list number");
ACCESS_LIST(show_nov_access_list2, show_nov_access_eol, show_nov_access_list3,
	   OBJ(int,1), MINENOVACC, MAXENOVACC,
	   IPX_MIN_EXT_ACL, IPX_MAX_EXT_ACL, "Access list number");
ACCESS_LIST(show_nov_access_list, show_nov_access_eol, show_nov_access_list2,
	   OBJ(int,1), MINNOVACC, MAXNOVACC,
	   IPX_MIN_STD_ACL, IPX_MAX_STD_ACL, "Access list number");
KEYWORD (show_nov_access, show_nov_access_list, show_nov_cache,
	 "access-lists", "IPX access lists", PRIV_USER);

/******************************************************************/
NOVELL_IPX_KEYWORD (show_novell, show_nov_access, ALTERNATE,
		    "Novell IPX information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_novell
