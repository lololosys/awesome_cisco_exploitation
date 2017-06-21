/* $Id: exec_debug_nmtest.h,v 3.1 1996/02/01 04:57:30 hampton Exp $
 * $Source: /release/111/cvs/Xsys/dspu/exec_debug_nmtest.h,v $
 *------------------------------------------------------------------
 * SNA Network Management Alert test
 *
 * April 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_nmtest.h,v $
 * Revision 3.1  1996/02/01  04:57:30  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:46:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:48:31  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS    (sna_nm_test_eol, nmtest_debug_command, 100);
NUMBER	(sna_nm_test_num, sna_nm_test_eol, no_alt,
	OBJ(int, 1), 0, 255, "test number");
KEYWORD (debug_sna_nm_test, sna_nm_test_num, ALTERNATE,
	 "nmtest", "Network management ", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_sna_nm_test
