/* $Id: modemcap.h,v 3.2 1995/11/17 18:55:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/modemcap.h,v $
 *------------------------------------------------------------------
 * modemcap.h --  private info for modemcap.c
 *
 * June 1995, Tim Kolar
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modemcap.h,v $
 * Revision 3.2  1995/11/17  18:55:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:33:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/26  01:08:00  tkolar
 * *** empty log message ***
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*  
 *  Maximum number of times we will follow a template entry while doing
 *   a lookup.
 */
#define MODEMCAP_MAX_RECURSE 7

/*  States for parsing a modemcap attribute */
#define MCAP_ATTR	0
#define MCAP_VALUE	1


boolean parse_modemcap_entry(char *);
