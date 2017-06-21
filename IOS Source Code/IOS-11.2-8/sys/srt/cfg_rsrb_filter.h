/* $Id: cfg_rsrb_filter.h,v 3.3.28.3 1996/08/12 16:07:45 widmer Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_rsrb_filter.h,v $
 *------------------------------------------------------------------
 * RSRB filtering commands
 *
 * Nov 1994, Jacob Teplitsky
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_rsrb_filter.h,v $
 * Revision 3.3.28.3  1996/08/12  16:07:45  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.28.2  1996/05/30  23:45:46  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/05/21  10:05:20  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/01/18  02:15:20  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  18:53:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:00  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:13:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * [no] rsrb remote-peer <ring-group>
 *  { interface <name> [<mac-addr>] | tcp <ipaddr> | fst <ipaddr> }
 *  { lsap-output-list <list> | netbios-output-list host <name> }
 *
 */

EOLNS(rsrb_eol, rsrb_filter_command);
ACCESS_LIST(rsrb_outlsap_list, rsrb_eol, no_alt,
	OBJ(int, 5), MINTYPEACC, MAXTYPEACC, TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
        "Type code access list");
KEYWORD_ID(rsrb_outlsap, rsrb_outlsap_list, no_alt,
	OBJ(int, 4), TRUE,
	"lsap-output-list", "Filter output IEEE 802.5 encapsulated packets",
	PRIV_CONF);
STRING  (rsrb_netbios_host_name, rsrb_eol, no_alt,
	OBJ(string,1), "Name of previously defined access list filter");
KEYWORD(rsrb_netbios_host, rsrb_netbios_host_name, no_alt,
	"host", "Filter packets with declared host access list filter",
	PRIV_CONF);
KEYWORD_ID(rsrb_netbios, rsrb_netbios_host, rsrb_outlsap,
	OBJ(int,3), TRUE,
	"netbios-output-list","Access list filter on outgoing messages",
	PRIV_CONF);
/* interface <name> [<mac-addr>] */
ENADDR  (rsrb_rpeer_int_addr, rsrb_netbios, no_alt,
	OBJ(hwaddr,1), "Ethernet Address");
TESTVAR (rsrb_rpeer_int_test, rsrb_netbios, rsrb_rpeer_int_addr,
	NONE, NONE, NONE, OBJ(int,9), FALSE);
EVAL(rsrb_rpeer_int_eval, rsrb_rpeer_int_test,
	(GETOBJ(int,9) = GETOBJ(idb,1)->hwptr->status & IDB_IEEEMASK));
INTERFACE(rsrb_rpeer_int, rsrb_rpeer_int_eval, no_alt, 
	OBJ(idb,1), IFTYPE_HWIDB     );
KEYWORD_ID(rsrb_rpeer_interface, rsrb_rpeer_int, no_alt,
	OBJ(int,2), SRB_REMPR_INT, common_str_interface,
	"Serial interface over which to run TR traffic", PRIV_CONF);
/* fst <ipaddr> */
IPADDR  (rsrb_rpeer_fst_ipaddr, rsrb_netbios, no_alt,
	OBJ(paddr,1), "Remote FST peer's IP address");
KEYWORD_ID(rsrb_rpeer_fst, rsrb_rpeer_fst_ipaddr, rsrb_rpeer_interface,
	OBJ(int,2), SRB_REMPR_FST,
	"fst", "Fast Sequence Transport", PRIV_CONF);
/* tcp <ipaddr> */
IPADDR  (rsrb_rpeer_ipaddr, rsrb_netbios, no_alt,
	OBJ(paddr,1), "Remote cisco peer router IP address");
KEYWORD_ID(rsrb_rpeer_tcp, rsrb_rpeer_ipaddr, rsrb_rpeer_fst,
	OBJ(int,2), SRB_REMPR_TCP,
	"tcp", "Use TCP encapsulation", PRIV_CONF);
NUMBER (rsrb_rpeer_ring, rsrb_rpeer_tcp, no_alt,
	OBJ(int, 1), 1, SRB_MAX_RING_NUM, "Ring Group");
KEYWORD (rsrb_rpeer, rsrb_rpeer_ring, no_alt,
	"remote-peer",
	"Specify a remote peer for remote SRB", PRIV_CONF);
NVGENNS  (crsrb_nvgen, rsrb_rpeer, rsrb_filter_command);
KEYWORD (crsrb, crsrb_nvgen, NONE, "rsrb",
	"RSRB LSAP/DSAP filtering", PRIV_CONF);
TEST_BOOLEAN(crsrb_test, crsrb, NONE, ALTERNATE, bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	crsrb_test
