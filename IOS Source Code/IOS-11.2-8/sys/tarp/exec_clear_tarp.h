/* $Id: exec_clear_tarp.h,v 3.3.20.1 1996/08/12 16:07:57 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tarp/exec_clear_tarp.h,v $
 *------------------------------------------------------------------
 * TARP (Target ID address Resolution Protocol)
 *
 * October 1995, Arun Sastry
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_tarp.h,v $
 * Revision 3.3.20.1  1996/08/12  16:07:57  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:52  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:44:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear tarp ldb-table
 *
 */
EOLNS	(tarp_clear_ldb_table_eol, tarp_clear_ldb_table);
KEYWORD	(tarp_clear_ldb_table_kwd, tarp_clear_ldb_table_eol, no_alt,
	 "ldb-table", "Clear LDB (loop data base) table", PRIV_OPR);

/******************************************************************
 * clear tarp tid-table
 *
 * OBJ(int,1) =  TRUE means that the "all" keyword exists, in which case
 * all cache entries (including our own entry) will be deleted.
 */
EOLNS	(tarp_clear_tid_table_eol, tarp_handle_tid_table_delete);
KEYWORD_ID (tarp_clear_tid_table_all, tarp_clear_tid_table_eol, 
	    tarp_clear_tid_table_eol,
	    OBJ(int,1), TRUE,
	    "all", NULL /*"Clear all entries (including our own entry)"*/,
	    PRIV_HIDDEN);
KEYWORD	(tarp_clear_tid_table_kwd, tarp_clear_tid_table_all,
	 tarp_clear_ldb_table_kwd,
	 "tid-table", "Clear TID (target ID) table", PRIV_OPR);

/******************************************************************
 * clear tarp counters
 *
 */
EOLNS	(tarp_clear_eol, tarp_clear_counters);
KEYWORD (tarp_clear_counter_kwd, tarp_clear_eol, tarp_clear_tid_table_kwd, 
	 "counters", "Clear TARP counters", PRIV_OPR);
KEYWORD	(tarp_clear_kwd, tarp_clear_counter_kwd, ALTERNATE,
	 "tarp", "Reset tarp information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	tarp_clear_kwd
