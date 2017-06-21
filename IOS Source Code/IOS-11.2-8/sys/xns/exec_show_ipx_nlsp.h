/* $Id: exec_show_ipx_nlsp.h,v 3.3.12.2 1996/08/16 00:14:21 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_show_ipx_nlsp.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ I P X _ N L S P . H
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_ipx_nlsp.h,v $
 * Revision 3.3.12.2  1996/08/16  00:14:21  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.3.12.1  1996/08/12  16:10:05  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/19  21:32:55  dkatz
 * CSCdi48130:  ISIS timer debugging info needed
 * Add the ability to display managed timer chains.
 *
 * Revision 3.2  1995/11/17  19:21:39  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:38:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/21  19:03:33  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.3  1995/08/23  18:27:30  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.2  1995/06/21  18:18:36  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  23:27:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

pdecl(show_novell_nlsp_opts);

/******************************************************************
 * show ipx nlsp [<tag-id>] database [detail] [private] [<lsp-id>]
 * 
 * OBJ(int,1) =	ISIS_CIRCUIT_L1
 * OBJ(int,2) =	TRUE if 'detail' keyword present
 * OBJ(int,3) = TRUE if 'private' keyword present
 * OBJ(string,1) = LSPID if present;
 * OBJ(string,2) = tag-id if present
 */
pdecl(show_nov_nlsp_db_opts);

EOLS	(show_nov_nlsp_db_eol, show_nlsp_database, SHOW_NLSP_DATABASE);

STRING (show_nov_nlsp_db_get_lspid, show_nov_nlsp_db_opts, NONE, OBJ(string,1),
	"LSPID in the form of xxxx.xxxx.xxxx.xx-xx or name.xx-xx");
IFELSE (show_nov_nlsp_db_lspid, show_nov_nlsp_db_get_lspid, NONE,
	!*GETOBJ(string,1));
TEST_MULTIPLE_FUNCS(show_nov_nlsp_db_test, show_nov_nlsp_db_lspid, 
		    show_nov_nlsp_db_eol,  NONE);
KEYWORD_ID(show_nov_nlsp_db_private, show_nov_nlsp_db_opts,
	   show_nov_nlsp_db_test, OBJ(int,3), TRUE, "private",
	   "Private link state database information", PRIV_USER);
KEYWORD_ID(show_nov_nlsp_db_detail, show_nov_nlsp_db_opts, 
	   show_nov_nlsp_db_private, OBJ(int,2), TRUE, "detail", 
	   "Detailed link state database information", PRIV_USER);
NOP (show_nov_nlsp_db_opts, show_nov_nlsp_db_detail, show_nov_nlsp_db_eol);

KEYWORD_ID(show_nov_nlsp_db, show_nov_nlsp_db_opts, NONE,
	   OBJ(int,1), ISIS_CIRCUIT_L1,
	   "database", "NLSP link state database", PRIV_USER);

/******************************************************************
 * show ipx nlsp [<tag-id>] timers
 *
 * OBJ(string,2) = tag-id if present
 */
EOLS	(show_nov_nlsp_timers_eol, show_nlsp_timers, SHOW_NLSP_TIMERS);
KEYWORD (show_nov_nlsp_timers, show_nov_nlsp_timers_eol, show_nov_nlsp_db,
	"timers", "NLSP timers", PRIV_USER|PRIV_HIDDEN);
 
/******************************************************************
 * show ipx nlsp [<tag-id>] spf-log
 *
 * OBJ(string,2) = tag-id if present
 */
EOLS	(show_nov_nlsp_spflog_eol, show_nlsp_spf_log, SHOW_NLSP_SPFLOG);
KEYWORD (show_nov_nlsp_spflog, show_nov_nlsp_spflog_eol, show_nov_nlsp_timers,
	"spf-log", "NLSP SPF log", PRIV_USER);

/******************************************************************
 * show ipx nlsp [<tag-id>] neighbors [<interface>] [detail]
 * 
 * OBJ(int,1) =  TRUE = "detail" keyword exists
 *		FALSE = no "detail" keyword
 * OBJ(idb,1) = <interface>
 * OBJ(string,2) = tag-id if present
 */

EOLNS	(show_novell_nlsp_neighbors_eol, show_nlsp_neigh);
KEYWORD_ID (show_novell_nlsp_neigh_detail, show_novell_nlsp_neighbors_eol, 
	    show_novell_nlsp_neighbors_eol,
	    OBJ(int,1), TRUE,
	    "detail", "Show detailed information", PRIV_USER);
INTERFACE (show_novell_nlsp_neigh_int, show_novell_nlsp_neigh_detail, 
	   show_novell_nlsp_neigh_detail,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_novell_nlsp_neighbors, show_novell_nlsp_neigh_int, 
	 show_nov_nlsp_spflog, "neighbors", "NLSP neighbor adjacencies", 
	 PRIV_USER);

NOP (show_novell_nlsp_opts, show_novell_nlsp_neighbors, NONE);

STRING (show_nov_nlsp_tag_string, show_novell_nlsp_opts, NONE,
	OBJ(string,2), "Routing process tag");
TEST_MULTIPLE_FUNCS(show_nov_nlsp_test, show_nov_nlsp_tag_string, NONE, no_alt);


NOP	(show_novell_nlsp_tag, show_novell_nlsp_opts, show_nov_nlsp_test);

KEYWORD (show_novell_nlsp, show_novell_nlsp_tag, no_alt,
	 "nlsp", "Show NLSP information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_novell_nlsp
