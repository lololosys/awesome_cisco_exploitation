/* $Id: cfg_line_download.h,v 3.2 1995/11/17 18:45:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_download.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ D O W N L O A D . H
 *
 * Set a line in 'download' mode.
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_download.h,v $
 * Revision 3.2  1995/11/17  18:45:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:50:57  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:09:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * download
 *
 * This command is never NV generated.
 */
EOLS	(cfg_download_eol, download_command, PARSER_LINE_DOWNLOAD);
KEYWORD	(cfg_download, cfg_download_eol, ALTERNATE,
	 "download", "Put line into 'download' mode",
	 PRIV_USER | PRIV_NONVGEN);

#undef	ALTERNATE
#define	ALTERNATE	cfg_download
