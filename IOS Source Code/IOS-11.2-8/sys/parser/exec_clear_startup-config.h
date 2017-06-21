/* $Id: exec_clear_startup-config.h,v 3.2 1995/11/17 18:45:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/exec_clear_startup-config.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _  S T A R T U P - C O N F I G . H
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_startup-config.h,v $
 * Revision 3.2  1995/11/17  18:45:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:16:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear startup-config -- OBSOLETE
 *
 */

EOLS	(clear_startup_config_eol, write_erase, WRITE_ERASE);
KEYWORD	(clear_startup_config, clear_startup_config_eol, ALTERNATE,
	 "startup-config", "Clear contents of configuration memory",
	 PRIV_ROOT|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	clear_startup_config
