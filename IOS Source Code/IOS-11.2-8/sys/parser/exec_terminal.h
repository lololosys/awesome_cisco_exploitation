/* $Id: exec_terminal.h,v 3.4 1995/12/15 22:28:14 widmer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/parser/exec_terminal.h,v $
 *------------------------------------------------------------------
 * E X E C _ T E R M I N A L . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_terminal.h,v $
 * Revision 3.4  1995/12/15  22:28:14  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.3  1995/12/01  15:57:01  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  18:47:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:26:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * Transition to the line configuration command set for handling
 * the remainder of the input.  This avoids having two parse chains
 * which handle the same input, one for terminal option selection
 * by the user and one for system configuration.
 */
KEYWORD	(terminal, configline_no, ALTERNATE,
	 "terminal", "Set terminal line parameters",
	 PRIV_USER | PRIV_INTERACTIVE);

#undef  ALTERNATE
#define ALTERNATE	terminal
