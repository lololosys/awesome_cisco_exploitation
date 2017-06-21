/* $Id: exec_show_c7100.h,v 3.1.68.4 1996/08/12 16:07:22 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/exec_show_c7100.h,v $
 *------------------------------------------------------------------
 * exec_show_c7100.h : C7100 specific show commands
 *
 * November 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_c7100.h,v $
 * Revision 3.1.68.4  1996/08/12  16:07:22  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.68.3  1996/06/26  18:40:15  mbeesley
 * CSCdi61260:  Make output of show c7200 more usefull to manufacturing
 * Branch: California_branch
 *
 * Revision 3.1.68.2  1996/04/06  02:05:45  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Make changes to deal with the change in the product name for Predator
 * from c7100 to c7200.
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/03/21  23:48:21  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.1  1995/11/22  21:43:31  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/21  01:37:52  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show c7200
 */

EOLNS	(show_c7100_eol, show_c7100_command);

KEYWORD_MM(show_c7100, show_c7100_eol, ALTERNATE,
	   "c7200", "Show c7200 information", PRIV_USER, 3);

#undef	ALTERNATE
#define	ALTERNATE	show_c7100

/* end of file */
