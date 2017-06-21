/*
 *	P A R S E R _ D E F S _ B R E A K S E T . H
 *
 * $Id: parser_defs_breakset.h,v 3.2 1995/11/17 18:49:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/parser_defs_breakset.h,v $
 * 
 * Copyright (c) 1985-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_breakset.h,v $
 * Revision 3.2  1995/11/17  18:49:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:29:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Break character set for the exec and config command levels.
 */
#define EXEC_BREAKSET           "\r\n?\t"
#define CONFIG_BREAKSET         "\r\n\032?\t"
#define NVCONFIG_BREAKSET       "\r\n"

