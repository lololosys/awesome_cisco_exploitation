/* $Id: exec_show_techsupport.h,v 3.1.2.2 1996/08/12 16:05:19 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_techsupport.h,v $
 *------------------------------------------------------------------
 * Show systems commands
 *
 * April 1996, Robert Widmer
 *
 * Copyright (c) 1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_techsupport.h,v $
 * Revision 3.1.2.2  1996/08/12  16:05:19  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/04/15  14:58:57  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.1  1996/04/12  19:08:47  widmer
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(show_techsupport_eol, show_techsupport);

LINK_TRANS(show_techsupport_return_here, show_techsupport_eol);
LINK_TRANS(show_techsupport_extend_here, show_techsupport_eol);

static keyword_options show_techsupport_options[] = {
    { "page", "Page through output", SHOW_TECHSUPPORT_PAGE },
    { "password", "Include passwords", SHOW_TECHSUPPORT_PW },
    { NULL, NULL, 0 }
};

KEYWORD_OPTIONS(show_techsupport_opts, show_techsupport_opts,
		show_techsupport_extend_here,
		show_techsupport_options, OBJ(int,1),
		PRIV_USER, KEYWORD_OR_SET | KEYWORD_TEST);
KEYWORD	(show_techsupport, show_techsupport_opts, ALTERNATE,
	 "tech-support", "Show system information for Tech-Support",
	 PRIV_USER | PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_techsupport
