/* $Id: cfg_int_bridge-group.h,v 3.5.44.2 1996/05/30 23:45:39 hampton Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_int_bridge-group.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ B R I D G E _ G R O U P . H
 *
 * Interface specific transparent bridging support
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_bridge-group.h,v $
 * Revision 3.5.44.2  1996/05/30  23:45:39  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.5.44.1  1996/03/18  22:10:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.2  1996/03/13  02:01:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.12.1  1996/02/20  18:45:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1995/12/27  18:59:29  hou
 * CSCdi45767:  Attempting to configure bridge-gr X sse causes
 *              %SYS-3-REGNONE:Registry 8 service 12 doesn't exist
 *              error message
 * - bgroup_sse parser node is removed from bgroup tree and dynamically
 *   added to the 7000 parser chain.
 *
 * Revision 3.4  1995/12/14  13:08:00  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.3  1995/12/07  17:45:32  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  18:53:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:30:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/04  01:57:56  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:03:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


LINK_TRANS(bgroup_extend_here, no_alt);

/**********************************************************************
 *	bridge-group <group> output-pattern-list <list>
 *	no bridge-group <group> output-pattern-list [<list>]
 *
 * OBJ(int,2) = <list>
 */
PARAMS_ALIST   (bgroup_outpattern, bgroup_extend_here,
		"output-pattern-list", OBJ(int,2), MINXMACACC, MAXXMACACC,
	        MACADDR_MIN_EXT_ACL, MACADDR_MAX_EXT_ACL,
		bridgegroup_command, BGROUP_OUTPATTERN,
		"Filter output with a pattern list",
		"Pattern access list number", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	bridge-group <group> input-pattern-list <list>
 *	no bridge-group <group> input-pattern-list [<list>]
 *
 * OBJ(int,2) = <list>
 */
PARAMS_ALIST   (bgroup_inpattern, bgroup_outpattern, 
		"input-pattern-list", OBJ(int,2), MINXMACACC, MAXXMACACC,
		MACADDR_MIN_EXT_ACL, MACADDR_MAX_EXT_ACL,
		bridgegroup_command, BGROUP_INPATTERN,
		"Filter input with a pattern list",
		"Pattern access list number", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 * bridge-group <group> spanning-disabled
 * no bridge-group <group> spanning-disabled
 */
EOLS	(bgroup_spandisable_eol, bridgegroup_command, BGROUP_SPANDISABLE);
KEYWORD	(bgroup_spandisable, bgroup_spandisable_eol, bgroup_inpattern,
	 "spanning-disabled", "Disable spanning tree on a bridge group",
	 PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	bridge-group <group> priority <priority>
 *	no bridge-group <group> priority [<priority>]
 *
 * OBJ(int,2) = <priority>
 */

PARAMS_KEYONLY (bgroup_priority, bgroup_spandisable,
		"priority", OBJ(int,2), 0, 255,
		bridgegroup_command, BGROUP_PRIORITY,
		"Set interface priority", "Priority", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	bridge-group <group> output-type-list <list>
 *	no bridge-group <group> output-type-list [<list>]
 * 
 * OBJ(int,2) = <list>
 */

PARAMS_ALIST   (bgroup_outtype, bgroup_priority,
		"output-type-list", OBJ(int,2), MINTYPEACC, MAXTYPEACC,
		TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
		bridgegroup_command, BGROUP_OUTTYPE,
		"Filter outgoing Ethernet packets by type code",
		"Ethernet type access list", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	bridge-group <group> output-lsap-list <list>
 *	no bridge-group <group> output-lsap-list [<list>]
 *
 * OBJ(int,2) = <list>
 */

PARAMS_ALIST   (bgroup_outlsap, bgroup_outtype, 
		"output-lsap-list", OBJ(int,2), MINTYPEACC, MAXTYPEACC,
		TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
		bridgegroup_command, BGROUP_OUTLSAP,
		"Filter outgoing IEEE 802.3 encapsulated packets",
		"Ethernet type access list", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	bridge-group <group> output-address-list <list>
 *	no bridge-group <group> output-address-list [<list>]
 *
 * OBJ(int,2) = <list>
 */

PARAMS_ALIST   (bgroup_outaddr, bgroup_outlsap, "output-address-list",
		OBJ(int,2), MINMACADDRACC, MAXMACADDRACC,
		MACADDR_MIN_STD_ACL, MACADDR_MAX_STD_ACL,
		bridgegroup_command, BGROUP_OUTADDRESS,
		"Filter packets by destination address",
		"Ethernet address access list", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	bridge-group <group> {
 *			input-lat-service-deny |
 *			input-lat-service-permit |
 *			output-lat-service-deny |
 *			output-lat-service-deny } <grouplist>
 *
 *	no bridge-group <group> *-lat-service-deny [<grouplist>]
 *
 * OBJ(int,2) = BGROUP_INLAT_DENY, BGROUP_INLAT_PERMIT,
 *		BGROUP_OUTLAT_DENY, BGROUP_OUTLAT_PERMIT
 *
 * OBJ(string,1) = <grouplist>
 * OBJ(int,3) = LAT_GROUP_LIST if a LAT group list was specified
 * OBJ(int,4) = LAT_GROUP_DISABLED, LAT_GROUP_ENABLED
 * 
 */

EOLS	(bgroup_latservice_eol, bridgegroup_command, BGROUP_LATSERVICE);

SET	(bgroup_latservice_set, bgroup_latservice_eol,
	 OBJ(int,4), LAT_GROUP_SET);
KEYWORD_ID (bgroup_latservice_disabled, bgroup_latservice_eol,
	   bgroup_latservice_set, OBJ(int,4), LAT_GROUP_DISABLED,
	   "disabled", "Remove from existing group code list",
	    PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID (bgroup_latservice_enabled, bgroup_latservice_eol,
	   bgroup_latservice_disabled, OBJ(int,4), LAT_GROUP_ENABLED,
	   "enabled", "Add to existing group code list", PRIV_CONF|PRIV_SUBIF);

SET	(bgroup_latservice_group_set, bgroup_latservice_enabled,
	 OBJ(int,3), LAT_GROUP_LIST);
GROUPLIST (bgroup_latservice_group, bgroup_latservice_group_set, NONE, 
	   OBJ(string,2), "A list of LAT service advertisements");

KEYWORD_ID (bgroup_latservice_all, bgroup_latservice_enabled, 
	    bgroup_latservice_group,
	    OBJ(int,3), LAT_GROUP_ALL,
	    "all", "All LAT groups", PRIV_CONF|PRIV_SUBIF);
TESTVAR	(bgroup_nolatservice, NONE, bgroup_latservice_eol,
	 NONE, NONE, bgroup_latservice_all, sense, TRUE);
NVGENS	(bgroup_latservice_nvgen, bgroup_nolatservice, bridgegroup_command,
	 BGROUP_LATSERVICE);

KEYWORD_ID(bgroup_outlatpermit, bgroup_latservice_nvgen, bgroup_outaddr,
	   OBJ(int,2), BGROUP_OUTLAT_PERMIT,
	   "output-lat-service-permit", 
	   "Permit output LAT service advertisements matching a group list",
	   PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(bgroup_outlatdeny, bgroup_latservice_nvgen, bgroup_outlatpermit,
	   OBJ(int,2), BGROUP_OUTLAT_DENY,
	   "output-lat-service-deny", 
	   "Deny output LAT service advertisements matching a group list",
	   PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(bgroup_inlatpermit, bgroup_latservice_nvgen, bgroup_outlatdeny,
	   OBJ(int,2), BGROUP_INLAT_PERMIT,
	   "input-lat-service-permit", 
	   "Permit input LAT service advertisements matching a group list", 
	   PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(bgroup_inlatdeny, bgroup_latservice_nvgen, bgroup_inlatpermit,
	   OBJ(int,2), BGROUP_INLAT_DENY,
	   "input-lat-service-deny", 
	   "Deny input LAT service advertisements matching a group list",
	   PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	bridge-group <group> lat-compression
 *	no bridge-group <group> lat-compression
 */
EOLS	(bgroup_latcomp_eol, bridgegroup_command, BGROUP_LAT);
KEYWORD (bgroup_latcomp, bgroup_latcomp_eol, bgroup_inlatdeny,
	 "lat-compression", "Enable LAT compression over serial interfaces", 
	 PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	bridge-group <group> input-type-list <list>
 *	no bridge-group <group> input-type-list [<list>]
 *
 * OBJ(int,2) = <list>
 */

PARAMS_ALIST (bgroup_intype, bgroup_latcomp,
		"input-type-list", OBJ(int,2), MINTYPEACC, MAXTYPEACC,
		TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
		bridgegroup_command, BGROUP_INTYPE,
		"Filter incoming Ethernet packets by type code",
		"Ethernet type access list", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	bridge-group <group> input-lsap-list <list>
 *	no bridge-group <group> input-lsap-list [<list>]
 *
 * OBJ(int,2) = <list>
 */

PARAMS_ALIST   (bgroup_inlsap, bgroup_intype,
		"input-lsap-list", OBJ(int,2), MINTYPEACC, MAXTYPEACC,
		TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
		bridgegroup_command, BGROUP_INLSAP,
		"Filter incoming IEEE 802.3 encapsulated packets",
		"Ethernet type access list", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	bridge-group <group> path-cost <cost>
 *	no bridge-group <group> path-cost [<cost>]
 *
 * OBJ(int,2) = <cost>
 */

PARAMS_KEYONLY (bgroup_pathcost, bgroup_inlsap,
		"path-cost", OBJ(int,2), 0, 65535,
		bridgegroup_command, BGROUP_PATHCOST,
		"Set interface path cost",
		"Path cost (higher values are higher costs)",
		PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	bridge-group <group> input-address-list <list>
 *	no bridge-group <group> input-address-list [<list>]
 *
 * OBJ(int,2) = <list>
 */

PARAMS_ALIST   (bgroup_inaddr, bgroup_pathcost, 
		"input-address-list", OBJ(int,2), MINMACADDRACC, MAXMACADDRACC,
		MACADDR_MIN_STD_ACL, MACADDR_MAX_STD_ACL,
		bridgegroup_command, BGROUP_INADDRESS,
		"Filter packets by source address",
		"Ethernet address access list", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	bridge-group <group> circuit-group <number>
 *	no bridge-group <group> circuit-group [<number>]
 *
 * OBJ(int,2) = <number>
 */

PARAMS_KEYONLY (bgroup_circuit, bgroup_inaddr,
                "circuit-group", OBJ(int,2), 1, 9,
                bridgegroup_command, BGROUP_CIRCUIT,
                "Associate serial interface with a circuit group",
                "Circuit group number", PRIV_CONF);


/**********************************************************************
 * bridge-group <group>
 * no bridge-group <group>
 */


EOLS    (bgroup_group, bridgegroup_command, BGROUP_ASSIGN);

NOP     (bgroup_group_nop, bgroup_group, bgroup_circuit);

ASSERT	(bgroup_grp_test, bgroup_group_nop, NONE,
	 !csb->nvgen ||
	 (csb->interface->span_index &&
	  csb->interface->span_index == GETOBJ(int,1)));

NUMBER_NV(bgroup_grp, bgroup_grp_test, no_alt,
	  OBJ(int,1), 1, SPAN_MAXINDEX - 1,
	  "Assign an interface to a Bridge Group.");

KEYWORD_MM(bgroup, bgroup_grp, NONE,
           "bridge-group", "Transparent bridging interface parameters",
           PRIV_CONF|PRIV_SUBIF, 7);

PRIV_TEST(bgroup_unsupp, bgroup, NONE, NONE, PRIV_UNSUPPORTED);

TEST_IDBSTATUS(bgroup_unsupp_test, bgroup_unsupp, bgroup, NONE,
	       IDB_TUNNEL);

ASSERT  (bgroup_subif, bgroup_unsupp_test, ALTERNATE,
         (bridge_enable && !is_cip(csb->interface->hwptr)
           && (is_sde_capable(csb->interface)
            || main_if_or_vc(csb->interface))));

#undef  ALTERNATE
#define ALTERNATE       bgroup_subif
