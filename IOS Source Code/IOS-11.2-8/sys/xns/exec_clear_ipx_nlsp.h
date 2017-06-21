/* $Id: exec_clear_ipx_nlsp.h,v 3.2.62.1 1996/08/12 16:10:01 widmer Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_clear_ipx_nlsp.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R  _ I P X _ N L S P . H
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_ipx_nlsp.h,v $
 * Revision 3.2.62.1  1996/08/12  16:10:01  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  19:21:22  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:38:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/21  19:03:33  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.2  1995/06/21  18:18:32  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  23:27:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ipx nlsp [tag-id] *
 *
 * OBJ(string,1) = tag-id
 */
pdecl   (clear_ipx_nlsp_opts);

STRING  (clear_ipx_nlsp_tag, clear_ipx_nlsp_opts, NONE,
	 OBJ(string,1), "Routing process id");
IFELSE (clear_ipx_nlsp_ifelse, clear_ipx_nlsp_tag, NONE, !*GETOBJ(string,1));
TEST_MULTIPLE_FUNCS(clear_ipx_nlsp_test, clear_ipx_nlsp_ifelse, no_alt, NONE);

EOLNS	(clear_ipx_nlsp_star_eol, novnlsp_clear_all);
KEYWORD (clear_ipx_nlsp_star, clear_ipx_nlsp_star_eol, 
	 clear_ipx_nlsp_test, "*", "clear NLSP database and adjacencies",
	 PRIV_OPR);

/******************************************************************
 * clear ipx nlsp [tagid] neighbors 
 *
 * OBJ(string,1) = tag-id
 */
EOLNS	(clear_ipx_nlsp_neighbors_eol, clear_nlsp_neighbors);
KEYWORD (clear_ipx_nlsp_neighbors, clear_ipx_nlsp_neighbors_eol, 
	 clear_ipx_nlsp_star, "neighbors", 
	 "clear NLSP neighbor adjacencies", PRIV_OPR);

NOP (clear_ipx_nlsp_opts, clear_ipx_nlsp_neighbors, NONE);

KEYWORD (clear_ipx_nlsp, clear_ipx_nlsp_opts, ALTERNATE,
	 "nlsp", "NLSP", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_ipx_nlsp
