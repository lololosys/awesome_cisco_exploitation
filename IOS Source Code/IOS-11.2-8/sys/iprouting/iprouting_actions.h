/* $Id: iprouting_actions.h,v 3.2 1995/11/17 17:34:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/iprouting_actions.h,v $
 *------------------------------------------------------------------
 * IP routing specific parsing functions.
 *
 * August 1994, Bruce Cole
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iprouting_actions.h,v $
 * Revision 3.2  1995/11/17  17:34:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:07:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

void parser_ip_router(parseinfo *, const char *, char *, transition *);
