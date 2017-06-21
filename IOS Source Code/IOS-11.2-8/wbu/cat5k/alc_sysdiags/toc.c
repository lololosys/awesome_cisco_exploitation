/* $Id: toc.c,v 1.1.4.1 1996/05/09 15:01:18 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/toc.c,v $
 *------------------------------------------------------------------
 * toc.c -- Table of Contents Definitions for diagnostics
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: toc.c,v $
 * Revision 1.1.4.1  1996/05/09  15:01:18  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:16  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:41  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:26  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:12  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:45  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2  1995/07/21  20:55:06  rbau
 * added loopback test suite. some code clean-up
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include	"types.h"
#include	"constant.h"
#include	"struct.h"


/*--- Test Suite Description ---*/
extern	char		memdiag_description[];
extern	char		bigadiag_description[];
extern	char		loopback_description[];

/*--- Test Suite Parameter ---*/
extern	struct PARAMETER_TABLE	memdiag_parameter[];
extern	struct PARAMETER_TABLE	bigadiag_parameter[];
extern	struct PARAMETER_TABLE	loopback_parameter[];


/*--- Test Suite Test Token ---*/
extern	struct TEST_TOC		memdiag_toc[];
extern	struct TEST_TOC		bigadiag_toc[];
extern	struct TEST_TOC		loopback_toc[];


/*--------------------------------------------------------------------*
 * SECTION_TOC{}
 *--------------------------------------------------------------------*/
struct SECTION_TOC	diag_toc[] =
{
	{ memdiag_description, "MEMDIAG", "M", memdiag_parameter,
	    memdiag_toc, 0 },
	{ bigadiag_description, "BIGADIAG", "B", bigadiag_parameter,
	    bigadiag_toc, 0 },
	{ loopback_description, "LOOPBACK", "L", loopback_parameter,
	    loopback_toc, 0 },
	{ NULL, NULL, NULL }
};

