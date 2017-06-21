/* $Id: exec_clear_logging.h,v 3.1.2.2 1996/08/12 16:05:02 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_clear_logging.h,v $
 *------------------------------------------------------------------
 * exec_clear_logging.h - Clear logging buffer
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_logging.h,v $
 * Revision 3.1.2.2  1996/08/12  16:05:02  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/06/19  04:43:57  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.1  1996/06/07  21:34:04  smackie
 * Add placeholders for logger cleanup
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear logging
 *
 */
EOLNS	(clear_logging_eol, logger_clear_command);
KEYWORD (clear_logging, clear_logging_eol, ALTERNATE,
	 "logging", "Clear logging buffer", PRIV_OPR);


#undef	ALTERNATE
#define	ALTERNATE	clear_logging
