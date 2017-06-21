/* $Id: parser_defs_reload.h,v 3.1 1995/11/09 13:41:28 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/ui/parser_defs_reload.h,v $
 *------------------------------------------------------------------
 *   P A R S E R _ D E F S _ R E L O A D . H
 *
 * Definitions for reload command
 *
 * September 1995, Dave Katz
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_reload.h,v $
 * Revision 3.1  1995/11/09  13:41:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/24  00:55:02  dkatz
 * CSCdi40898:  Synchronized reload needed
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Different reload command types */

enum {RELOAD_TYPE_IMMEDIATE = 0, RELOAD_TYPE_IN, RELOAD_TYPE_AT,
	 RELOAD_TYPE_CANCEL};
