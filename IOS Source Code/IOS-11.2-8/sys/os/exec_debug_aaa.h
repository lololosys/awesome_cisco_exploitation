/* $Id: exec_debug_aaa.h,v 3.1 1996/02/16 00:43:11 hampton Exp $
 * $Source: /release/111/cvs/Xsys/os/exec_debug_aaa.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ A A A . H
 *
 * June 1995, Lol Grant
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_aaa.h,v $
 * Revision 3.1  1996/02/16  00:43:11  hampton
 * Migrate more files out of the parser directory.  [CSCdi49139]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.1  1995/11/09  12:52:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:47:02  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/15  03:42:27  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * debug aaa accounting
 */

EOLS    (debug_aaa_acct_eol, debug_command, DEBUG_AAA_ACCT);
KEYWORD (debug_aaa_acct, debug_aaa_acct_eol, no_alt,
	 "accounting", "Accounting", PRIV_OPR);

/*
 * debug aaa authorization
 */

EOLS    (debug_aaa_author_eol, debug_command, DEBUG_AAA_AUTHOR);
KEYWORD (debug_aaa_author, debug_aaa_author_eol, debug_aaa_acct,
	 "authorization", "Authorization", PRIV_OPR);

/*
 * debug aaa authentication
 */

EOLS    (debug_aaa_authen_eol, debug_command, DEBUG_AAA_AUTHEN);
KEYWORD (debug_aaa_authen, debug_aaa_authen_eol, debug_aaa_author,
	 "authentication", "Authentication", PRIV_OPR);

/******************************************************************
 * debug aaa
 */

KEYWORD_DEBUG(debug_aaa, debug_aaa_authen, ALTERNATE,
	      OBJ(pdb,1), os_debug_arr,
	      "aaa", "AAA Authentication, Authorization and Accounting",
	      PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_aaa
