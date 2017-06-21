/* $Id: cfg_int_srb.h,v 3.2.62.2 1996/05/30 23:45:41 hampton Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_int_srb.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ S R B . H
 *
 * Source-route bridging support
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_srb.h,v $
 * Revision 3.2.62.2  1996/05/30  23:45:41  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/05/17  12:12:35  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/04/25  19:31:21  ioakley
 * CSCdi53651:  Add support for hidden message-age-increment option in
 *              "source-bridge spanning X" interface command.  Permits
 *              AST interoperability in mixed vendor bridge environments
 *              where hard-coded MAX_AGE defaults are insufficient for
 *              the network diameter and MAX_AGE is not configurable.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2  1995/11/17  18:53:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:30:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/12  09:12:43  ogrady
 * CSCdi38477:  Configuring source-bridge spanning crashes router.
 * Don't allow configuring fddi/srb on router that doesn't yet support
 * it.
 *
 * Revision 2.1  1995/07/04  01:57:58  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:07:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 *	source-bridge max-rd <count>
 *	no source-bridge max-rd [<count>]
 */
PARAMS_KEYONLY (srb_maxrd, no_alt, 
		"max-rd", OBJ(int,1), SRB_MIN_MAXRD, SRB_MAX_MAXRD,
		srbif_command, SRB_MAXRD,
		"Set maximum number of Routing descriptors",
		"Number of routing descriptors in RIF", PRIV_CONF);

/************************************************************************ 
 * source-bridge output-address-list <list>
 * no source-bridge output-address-list [<list>]
 */
PARAMS_ALIST  (srb_outaddr, srb_maxrd,
	       "output-address-list", OBJ(int,1), MINMACADDRACC, MAXMACADDRACC,
	       MACADDR_MIN_STD_ACL, MACADDR_MAX_STD_ACL,
	       srbif_command, SRB_OUTADDRESS,
	       "Filter output packets by MAC address",
	       "MAC layer address", PRIV_CONF);

/************************************************************************ 
 * source-bridge output-type-list <list>
 * no source-bridge output-type-list [<list>]
 */
PARAMS_ALIST  (srb_outtype, srb_outaddr,
	       "output-type-list", OBJ(int,1), MINTYPEACC, MAXTYPEACC,
	       TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
	       srbif_command, SRB_OUTTYPE,
	       "Filter output packets by type code",
	       "Type code access list", PRIV_CONF);

/************************************************************************ 
 * source-bridge output-lsap-list <list>
 * no source-bridge output-lsap-list [<list>]
 */
PARAMS_ALIST  (srb_outlsap, srb_outtype,
	       "output-lsap-list", OBJ(int,1), MINTYPEACC, MAXTYPEACC,
	       TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
	       srbif_command, SRB_OUTLSAP,
	       "Filter output IEEE 802.5 encapsulated packets",
	       "Type code access list", PRIV_CONF);

/************************************************************************ 
 * source-bridge input-address-list <list>
 * no source-bridge input-address-list [<list>]
 */
PARAMS_ALIST  (srb_inaddr, srb_outlsap,
	       "input-address-list", OBJ(int,1), MINMACADDRACC, MAXMACADDRACC,
	       MACADDR_MIN_STD_ACL, MACADDR_MAX_STD_ACL,
	       srbif_command, SRB_INADDRESS,
	       "Filter input packets by MAC address",
	       "MAC layer address access list", PRIV_CONF);

/************************************************************************ 
 * source-bridge input-type-list <list>
 * no source-bridge input-type-list [<list>]
 */
PARAMS_ALIST  (srb_intype, srb_inaddr,
	       "input-type-list", OBJ(int,1), MINTYPEACC, MAXTYPEACC,
	       TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
	       srbif_command, SRB_INTYPE,
	       "Filter input packets by type code",
	       "Type code access list", PRIV_CONF);

/************************************************************************ 
 * source-bridge input-lsap-list <list>
 * no source-bridge input-lsap-list [<list>]
 */
PARAMS_ALIST  (srb_inlsap, srb_intype,
	       "input-lsap-list", OBJ(int,1), MINTYPEACC, MAXTYPEACC,
	       TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
	       srbif_command, SRB_INLSAP,
	       "Filter input IEEE 802.5 encapsulated packets",
	       "Type code access list", PRIV_CONF);

/************************************************************************
 *	source-bridge route-cache cbus 
 *	source-bridge route-cache sse
 *	no source-bridge route-cache cbus
 *	no source-bridge route-cache sse
 */

EOLS	(srb_routec_eol, srbif_command, SRB_ROUTEC);
KEYWORD_ID(srb_routec_sse, srb_routec_eol, srb_routec_eol,
	   OBJ(int,1), SRB_ROUTEC_SSE,
	   "sse", "Silicon Switching Engine", PRIV_CONF);
KEYWORD_ID(srb_routec_cbus, srb_routec_eol, srb_routec_sse,
	   OBJ(int,1), SRB_ROUTEC_CBUS,
	   "cbus", "Cbus route-cache", PRIV_CONF);

TEST_IDBSTATUS(srb_routec_cbus_test, srb_routec_cbus, NONE, srb_inlsap, IDB_TR|IDB_FDDI);

NVGENS	(srb_routec_nvgen, srb_routec_cbus_test, srbif_command, SRB_ROUTEC);
KEYWORD	(srb_routec, srb_routec_nvgen, srb_inlsap,
	 "route-cache", "Enable route-caching", PRIV_CONF);

/************************************************************************
 *	source-bridge old-sna
 *	no source-bridge old-sna
 */

EOLS	(srb_oldsna1, srbif_command, SRB_OLDSNA);
KEYWORD	(srb_oldsna, srb_oldsna1, srb_routec,
	 "old-sna", "Enable IBM PC/3270 compatibility mode", PRIV_CONF);

/************************************************************************
 *	source-bridge proxy-explorer
 *	no source-bridge proxy-explorer
 */

EOLS	(srb_proxyexp1, srbif_command, SRB_PROXY);
KEYWORD_ID(srb_proxy_dsap, srb_proxyexp1, srb_proxyexp1,
	   OBJ(int,1), SRB_PROXY_DSAP,
	   "non-null-dsap", "", PRIV_HIDDEN);
NVGENS	(srb_proxyexp_nvgen, srb_proxy_dsap, srbif_command, SRB_PROXY);
KEYWORD	(srb_proxyexp, srb_proxyexp_nvgen, srb_oldsna,
	 "proxy-explorer", "Enable proxy explorer packets", PRIV_CONF);

/**********************************************************************
 * source-bridge spanning [<group> [path-cost <cost>] [priority <cost>]]
 * no source-bridge spanning [<group>]
 *
 * OBJ(int,1) = <group>
 * OBJ(int,2) = TRUE if OBJ(int,1) is set
 *		FALSE if OBJ(int,1) is not set
 * OBJ(int,3) = <cost>
 * OBJ(int,4) = TRUE if OBJ(int,3) is set
 *		FALSE if OBJ(int,3) is not set
 * OBJ(int,5) = <priority>
 * OBJ(int,6) = TRUE if OBJ(int,6) is set
 *              FALSE if OBJ(int,6) is not set
 * OBJ(int,7) = <message-age-increment>
 * OBJ(int,8) = TRUE if OBJ(int,8) is set
 *              FALSE if OBJ(int,8) is not set
 */

EOLS    (srb_span_eol, srbif_command, SRB_SPAN);

NUMBER  (srb_span_message_age_increment, srb_span_eol, no_alt,
	OBJ(int,8), 0, 1,
        "Message Age Increment (0 = passthrough / 1 = normal(default))");
KEYWORD_ID(srb_span_message_age, srb_span_message_age_increment, srb_span_eol,
           OBJ(int,7), TRUE,
           "message-age-increment", "Set message age increment",
           PRIV_HIDDEN|PRIV_SUBIF);

NUMBER  (srb_span_priority_number, srb_span_eol, no_alt,
	OBJ(int,6), 0, 255, "Priority (lower values are higher priorities)");
KEYWORD_ID(srb_span_priority, srb_span_priority_number, srb_span_message_age,
           OBJ(int,5), TRUE,
	   "priority", "Set interface priority", PRIV_CONF|PRIV_SUBIF);

NUMBER	(srb_span_pathcost_number, srb_span_eol, no_alt,
	OBJ(int,4), 0, 65535, "Path cost (higher values are higher costs)");
KEYWORD_ID(srb_span_pathcost, srb_span_pathcost_number, srb_span_priority,
	   OBJ(int,3), TRUE,
	   "path-cost", "Set interface path cost", PRIV_CONF|PRIV_SUBIF);

SET     (srb_span_message_age_set, srb_span_pathcost, OBJ(int,8), -1);
SET     (srb_span_priority_set, srb_span_message_age_set, OBJ(int,6), -1);
SET     (srb_span_pathcost_set, srb_span_priority_set, OBJ(int,4), -1);
SET	(srb_span_group_set, srb_span_pathcost_set, OBJ(int,2), TRUE);
NUMBER	(srb_span_group, srb_span_group_set, srb_span_eol,
	 OBJ(int,1), 1, SPAN_MAXINDEX - 1, "SRB spanning group number");
NVGENS  (srb_spanning_nvgen, srb_span_group, srbif_command, SRB_SPAN);
KEYWORD (srb_spanning, srb_spanning_nvgen, srb_proxyexp,
         "spanning", "Enables use of spanning explorers", PRIV_CONF);

/************************************************************************
 *	source-bridge max-hops <count>
 *	no source-bridge max-hops [<count>]
 */

PARAMS_KEYONLY(srb_maxhops, srb_spanning,
	       "max-hops", OBJ(int,1), SRB_MIN_MAXHOPS, SRB_MAX_MAXHOPS,
	       srbif_command, SRB_MAXHOPS,
	       "Set maximum number of hops a SRB packet may take",
	       "Hop count", PRIV_CONF);

/************************************************************************
 *      source-bridge max-out-hops <count>
 *      no source-bridge max-out-hops [<count>]
 */
 
PARAMS_KEYONLY(srb_maxouthops, srb_maxhops,
               "max-out-hops", OBJ(int,1), SRB_MIN_MAXHOPS, SRB_MAX_MAXHOPS,
               srbif_command, SRB_MAXOHOPS,
               "Set maximum number of Span Output hops a SRB packet may take",
               "Span output hop count", PRIV_CONF);
 
/************************************************************************
 *      source-bridge max-in-hops <count>
 *      no source-bridge max-in-hops [<count>]
 */
 
PARAMS_KEYONLY(srb_maxinhops, srb_maxouthops,
               "max-in-hops", OBJ(int,1), SRB_MIN_MAXHOPS, SRB_MAX_MAXHOPS,
               srbif_command, SRB_MAXIHOPS,
               "Set maximum number of Span Input hops a SRB packet may take",
               "Span input hop count", PRIV_CONF);
 

/************************************************************************ 
 * source-bridge active <local-ring> <bridge> <target-ring>
 * no source-bridge active [<local-ring> <bridge> <target-ring>]
 *
 * See below.
 */

/************************************************************************ 
 *	source-bridge <local-ring> <bridge> <target_ring>
 *	no source-bridge <local-ring> <bridge> <target_ring>
 *
 * OBJ(int,1) = <local-ring>
 * OBJ(int,2) = <bridge>
 * OBJ(int,3) = <target-ring>
 *
 * The 9.0 sources added the keyword 'active' to denote
 * this command and the 'no source-bridge' command.  This is difficult
 * to parse and still do NV generation.  
 * It requires several tests to do right and not have NVgen generate
 * the output for each instance of the command.
 * 
 * Because of the syntax of this command, the 'no' version
 * must have either all the parameters, or none.  This is contrary
 * to the other commands, but there is no way to glob the
 * input without affecting the other source-bridge commands.
 * See below for the shorter 'no' version of the command.
 *
 *	no source-bridge
 *
 * The source-bridge command must have either all its parameters, or
 * none.  This macro set is a special case to check for "no source-bridge".
 * This would have been much easier if there had been a keyword 
 * to uniquely identify the source-bridge parameter command.
 * Note that if this doesn't match, we also check for the full
 * command syntax.  This allows the user to prefix the standard command
 * with 'no' in a config file.
 */

EOLS	(srb_eol, srbif_command, SRB_ACTIVE);

NUMBER	(srb_target, srb_eol, no_alt,
	 OBJ(int,3), 1, SRB_MAX_RING_NUM, "Target ring number");
NUMBER	(srb_bridge, srb_target, no_alt,
	 OBJ(int,2), 1, SRB_MAX_BRIDGE_NUM, "Bridge number");
NUMBER	(srb_localnum, srb_bridge, NONE,
	 OBJ(int,1), 1, SRB_MAX_RING_NUM, "Ring number of local interface");
TESTVAR	(srb_active_noprefix, srb_eol,NONE,
	 NONE, NONE, srb_localnum, sense, FALSE);

/*
 * This will take care of "[no] source-bridge active"
 */
NVGENS	(srb_active_nvgen, srb_active_noprefix, srbif_command, SRB_ACTIVE);
KEYWORD	(srb_active_kwd, srb_active_nvgen, srb_maxinhops,
	 "active", "", PRIV_CONF | PRIV_HIDDEN);

/*
 * This will take care of "no source-bridge"
 * and "source-bridge <num> <num> <num>"
 */
TESTVAR	(srb_active, NONE, srb_active_noprefix,
	 NONE, NONE, srb_active_kwd, nvgen, TRUE);

/*********************************************************************/
KEYWORD	(ci_srb, srb_active, NONE,
	 "source-bridge", "Configure interface for source-route bridging",
	 PRIV_CONF);


TEST_BOOLEAN(ci_srb_enable, ci_srb, NONE, NONE, bridge_enable);
ASSERT	(ci_srb_test_fddi, ci_srb_enable, ALTERNATE,
         (csb->interface->hwptr->status & IDB_FDDI));

TEST_IDBSTATUS(ci_srb_test_tr, ci_srb_enable, NONE, ci_srb_test_fddi, IDB_TR);

#undef	ALTERNATE
#define	ALTERNATE	ci_srb_test_tr
