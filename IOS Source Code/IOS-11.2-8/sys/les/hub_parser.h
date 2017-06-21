/* $Id: hub_parser.h,v 3.2 1995/11/17 17:37:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/hub_parser.h,v $
 *------------------------------------------------------------------
 * hub_parser.h - header for ether hub parser functions.
 *
 * June 1994, Steve Garcia
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: hub_parser.h,v $
 * Revision 3.2  1995/11/17  17:37:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:32:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Function prototypes
 */
#ifndef __HUB_PARSER_H__
#define __HUB_PARSER_H__

extern void hub_show_int (hwidbtype *);
extern void show_hub(parseinfo *, hub_t *);

#endif
