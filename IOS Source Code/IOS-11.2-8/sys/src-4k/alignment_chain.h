/* $Id: alignment_chain.h,v 3.2.62.1 1996/08/12 16:07:18 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/alignment_chain.h,v $
 *------------------------------------------------------------------
 * alignment_chain.h - parse chain definitions for alignment subsys
 *
 * April 1994, Jim Hayes
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: alignment_chain.h,v $
 * Revision 3.2.62.1  1996/08/12  16:07:18  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:40:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:20:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:52:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show fixade
 * 
 */
EOLNS    (show_alignment_eol, show_alignment);
KEYWORD (show_alignment, show_alignment_eol, NONE,
         "alignment", NULL, PRIV_ROOT | PRIV_HIDDEN);

LINK_POINT(show_alignment_command, show_alignment);

/******************************************************************
 * show context
 * 
 */
EOLNS    (show_context_eol, show_context_command);
KEYWORD_MM (show_context, show_context_eol, NONE,
         "context", "Show context information", PRIV_USER, 5);

LINK_POINT(context_show_commands, show_context);

/******************************************************************
 * test alignment
 */

EOLS	(test_alignment_eol, alignment_command, ALIGN_TEST);

KEYWORD	(test_alignment, test_alignment_eol, NONE,
	 "alignment", NULL, PRIV_ROOT | PRIV_HIDDEN);

LINK_POINT(test_alignment_command, test_alignment);

/***************************************************************
 * [no] service alignment logging
 */

EOLS	(alignment_logging_eol, alignment_command, ALIGN_LOGGING);

NOPREFIX(alignment_logging_no_prefix, alignment_logging_eol,
	 alignment_logging_eol);

NVGENS	(alignment_logging_nvgen, alignment_logging_no_prefix,
	 alignment_command, ALIGN_LOGGING);

KEYWORD (alignment_logging, alignment_logging_nvgen, no_alt,
	 "logging", "Enable logging of alignment issues",
	 PRIV_CONF);

/***************************************************************
 * [no] service alignment detection
 */

EOLS	(alignment_enable_eol, alignment_command, ALIGN_ENABLE);

NOPREFIX(alignment_enable_no_prefix, alignment_enable_eol, 
	 alignment_enable_eol);

NVGENS	(alignment_enable_nvgen, alignment_enable_no_prefix,
	 alignment_command, ALIGN_ENABLE);

KEYWORD	(alignment_enable, alignment_enable_nvgen, alignment_logging,
	 "detection", "Enable detection of alignment issues",
	PRIV_CONF);

KEYWORD (service_alignment, alignment_enable, NONE,
	 "alignment", "Control alignment correction and logging",
	 PRIV_CONF);

LINK_POINT(service_alignment_commands, service_alignment);
