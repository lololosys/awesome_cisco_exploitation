/* $Id: exec_show_gt64010.h,v 3.1.68.2 1996/08/12 16:07:19 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/exec_show_gt64010.h,v $
 *------------------------------------------------------------------
 * exec_show_gt64010.h - parse chain for GT64010
 *
 * Octobert 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_gt64010.h,v $
 * Revision 3.1.68.2  1996/08/12  16:07:19  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.68.1  1996/03/21  23:46:25  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.1  1995/11/22  21:42:47  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/21  21:29:52  mbeesley
 * Add placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show gt64010
 */

EOLNS	(show_gt64010_eol, show_gt64010);

KEYWORD	(show_gt64010, show_gt64010_eol, NONE,
     	"gt64010", "Show GT64010 registers", PRIV_ROOT);

LINK_POINT(show_gt64010_command, show_gt64010);

/* end of file */
