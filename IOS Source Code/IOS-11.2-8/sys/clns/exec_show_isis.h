/* $Id: exec_show_isis.h,v 3.4.12.3 1996/08/12 16:00:43 widmer Exp $
 * $Source: /release/112/cvs/Xsys/clns/exec_show_isis.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ I S I S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_isis.h,v $
 * Revision 3.4.12.3  1996/08/12  16:00:43  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.12.2  1996/08/11  15:12:52  dkatz
 * CSCdi58739:  ISIS needs to be able to limit flooding
 * Branch: California_branch
 * Add phase I of mesh group support.  Allow an interface to be
 * configured to be part of a mesh group, or to be blocked for
 * flooding.  Allow CSNP transmission to be configured on point-to-
 * point interfaces as a minimal robustness mechanism.  Redefine
 * the SRM, etc., bits as an array of ulongs rather than bytes (to
 * speed up masking operations).  Change the interface number field
 * from a short to a long in order to shrink the code size by a few
 * hundred bytes.
 *
 * Revision 3.4.12.1  1996/08/04  23:34:57  dkatz
 * CSCdi63547:  ISIS database search is expensive
 * Branch: California_branch
 * Rewrite the dreaded isis_recursive_walk so that it no longer uses
 * recursion.  Fix "show isis tree" so that it works properly.  Clean up
 * references to isis_recursive_walk to make type coercion unnecessary.
 *
 * Revision 3.4  1996/02/19  21:32:25  dkatz
 * CSCdi48130:  ISIS timer debugging info needed
 * Add the ability to display managed timer chains.
 *
 * Revision 3.3  1996/02/01  23:33:02  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  08:53:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:08:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/21  19:02:57  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.2  1995/08/23  18:25:54  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.1  1995/07/02  01:47:04  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:24:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show isis mesh-groups 
 */
EOLS	(show_isis_meshgrps_eol, show_isis_mesh_groups, SHOW_ISIS_MESHGRPS);
KEYWORD (show_isis_meshgrps, show_isis_meshgrps_eol, no_alt,
	 "mesh-groups", "IS-IS mesh groups", PRIV_USER);

/******************************************************************
 * show isis timers
 */
EOLS	(show_isis_timers_eol, show_isis_timers, SHOW_ISIS_TIMERS);
KEYWORD (show_isis_timers, show_isis_timers_eol, show_isis_meshgrps,
	 "timers", "IS-IS timers", PRIV_USER|PRIV_HIDDEN);

/******************************************************************
 * show isis spf-log
 */
EOLS	(show_isis_spflog_eol, show_isis_spf_log, SHOW_ISIS_SPFLOG);
KEYWORD (show_isis_spflog, show_isis_spflog_eol, show_isis_timers,
	 "spf-log", "IS-IS SPF log", PRIV_USER);

/******************************************************************
 * show isis tree { level-1 | l1 | level-2 | l2 }
 *
 */
EOLNS	(show_isis_tree_eol, show_isis_tree);
KEYWORD_ID(show_isis_tree_l2, show_isis_tree_eol, show_isis_tree_eol,
	   OBJ(int,1), ISIS_CIRCUIT_L2,
	   "l2", "Level 2 AVL tree", PRIV_USER);
KEYWORD_ID(show_isis_tree_level2, show_isis_tree_eol, show_isis_tree_l2,
	   OBJ(int,1), ISIS_CIRCUIT_L2,
	   "level-2", "Level 2 AVL tree", PRIV_USER);
KEYWORD_ID(show_isis_tree_l1, show_isis_tree_eol, show_isis_tree_level2,
	   OBJ(int,1), ISIS_CIRCUIT_L1,
	   "l1", "Level 1 AVL tree", PRIV_USER);
KEYWORD_ID(show_isis_tree_level1, show_isis_tree_eol, show_isis_tree_l1,
	   OBJ(int,1), ISIS_CIRCUIT_L1,
	   "level-1", "Level 1 AVL tree", PRIV_USER);
KEYWORD_ID(show_isis_tree_kwd, show_isis_tree_level1, show_isis_spflog,
	   OBJ(int,1), ISIS_CIRCUIT_L1L2,
	   "tree", "IS-IS link state database AVL tree", PRIV_HIDDEN);

/******************************************************************
 * show isis route
 *
 */
EOLS	(show_isis_route_eol, show_isis_route, SHOW_ISIS_ROUTE);
KEYWORD (show_isis_route_kwd, show_isis_route_eol, show_isis_tree_kwd,
	"route", "IS-IS level-1 routing table", PRIV_USER);

/******************************************************************
 * show isis database [{ level-1 | level-2 }] [detail] [private]
 *
 * OBJ(int,1) =	ISIS_CIRCUIT_L1
 * 		ISIS_CIRCUIT_L2
 *		ISIS_CIRCUIT_L1L2
 * OBJ(int,2) =	TRUE if 'detail' keyword present
 * OBJ(int,3) = TRUE if 'private' keyword present
 * OBJ(string,1) = LSPID if present;
 */
pdecl	(show_isis_db_opts);

EOLS	(show_isis_db_eol, show_isis_database, SHOW_ISIS_DATABASE);

STRING (show_isis_db_get_lspid, show_isis_db_opts, NONE, OBJ(string,1),
	"LSPID in the form of xxxx.xxxx.xxxx.xx-xx or name.xx-xx");
IFELSE (show_isis_db_lspid, show_isis_db_get_lspid, NONE,
	!*GETOBJ(string,1));
TEST_MULTIPLE_FUNCS(show_isis_db_test, show_isis_db_lspid, show_isis_db_eol,
		    NONE);
KEYWORD_ID(show_isis_db_private, show_isis_db_opts, show_isis_db_test,
	   OBJ(int,3), TRUE, "private",
	   "Private database information", PRIV_USER|PRIV_HIDDEN);
KEYWORD_ID(show_isis_db_detail, show_isis_db_opts, show_isis_db_private,
	   OBJ(int,2), TRUE, "detail",
	   "Detailed link state database information", PRIV_USER);
KEYWORD_ID(show_isis_db_l2, show_isis_db_opts, show_isis_db_detail,
	   OBJ(int,1), ISIS_CIRCUIT_L2, "l2",
	   "IS-IS Level-2 routing link state database", PRIV_USER);
KEYWORD_ID(show_isis_db_level2, show_isis_db_opts, show_isis_db_l2,
	   OBJ(int,1), ISIS_CIRCUIT_L2, "level-2",
	   "IS-IS Level-2 routing link state database", PRIV_USER);
KEYWORD_ID(show_isis_db_l1, show_isis_db_opts, show_isis_db_level2,
	    OBJ(int,1), ISIS_CIRCUIT_L1, "l1",
	   "IS-IS Level-1 routing link state database", PRIV_USER);
KEYWORD_ID(show_isis_db_level1, show_isis_db_opts, show_isis_db_l1,
	   OBJ(int,1), ISIS_CIRCUIT_L1, "level-1",
	   "IS-IS Level-1 routing link state database", PRIV_USER);

NOP (show_isis_db_opts, show_isis_db_level1, show_isis_db_eol);

KEYWORD_ID(show_isis_db, show_isis_db_opts, show_isis_route_kwd,
	   OBJ(int,1), ISIS_CIRCUIT_L1L2,
	   "database", "IS-IS link state database", PRIV_USER);

KEYWORD (show_isis_kwd, show_isis_db, ALTERNATE,
	 "isis", "IS-IS routing information", PRIV_USER|PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_isis_kwd
