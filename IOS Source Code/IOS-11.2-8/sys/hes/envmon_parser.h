/* $Id: envmon_parser.h,v 3.2 1995/11/17 09:15:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/envmon_parser.h,v $
 *------------------------------------------------------------------
 * Environmental monitor parser header file
 *
 * April 1995, Robert Widmer
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: envmon_parser.h,v $
 * Revision 3.2  1995/11/17  09:15:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:39:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

enum SHOW_ENV {
    SHOW_ENV_ALL,
    SHOW_ENV_DEFAULT,
    SHOW_ENV_LAST,
    SHOW_ENV_TABLE
};

extern void envmon_parser_init(void);
