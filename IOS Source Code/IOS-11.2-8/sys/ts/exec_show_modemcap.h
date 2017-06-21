/* $Id: exec_show_modemcap.h,v 3.1.40.3 1996/08/12 16:08:34 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/exec_show_modemcap.h,v $
 *------------------------------------------------------------------
 * exec_show_modemcap.h --- parser stuff for "show modemcap"
 *
 * June 1995, Tim Kolar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_modemcap.h,v $
 * Revision 3.1.40.3  1996/08/12  16:08:34  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.40.2  1996/06/16  21:20:02  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1.40.1  1996/05/07  18:07:39  snyder
 * CSCdi56844:  spellink errors
 * Branch: California_branch
 *
 * miscllaneous -> miscellaneous
 * Capabilties -> Capabilities
 *
 * Revision 3.1  1996/02/01  05:14:03  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:09:25  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:47:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/26  01:07:52  tkolar
 * *** empty log message ***
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(show_modemcap_eol, show_modemcap);
STRING  (show_modemcap_string, show_modemcap_eol, show_modemcap_eol,
	 OBJ(string,1), "Modem entry to use");
KEYWORD_MM (show_modemcap, show_modemcap_string, ALTERNATE,
	    "modemcap", "Show Modem Capabilities database", PRIV_USER, 6);

#undef	ALTERNATE
#define	ALTERNATE	show_modemcap
