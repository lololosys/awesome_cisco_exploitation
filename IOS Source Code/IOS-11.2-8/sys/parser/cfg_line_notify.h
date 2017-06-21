/* $Id: cfg_line_notify.h,v 3.2 1995/11/17 18:44:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_line_notify.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ N O T I F Y . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_notify.h,v $
 * Revision 3.2  1995/11/17  18:44:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:10:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/********************************************************
 * notify
 * no notify
 */
EOLS	(ci_notify_eol, line_command, TERM_NOTIFY);
KEYWORD_MM(ci_notify, ci_notify_eol, ALTERNATE,
	   "notify",
	   "Inform users of output from concurrent sessions",
	   PRIV_USER, 3);

#undef	ALTERNATE
#define	ALTERNATE	ci_notify

