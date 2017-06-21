/* $Id: decnet_parser.h,v 3.2 1995/11/17 08:58:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/decnet/decnet_parser.h,v $
 *------------------------------------------------------------------
 * decnet_parser.h -- parser definitions for DECnet protocol family
 *
 * Copyright (c) 1985-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: decnet_parser.h,v $
 * Revision 3.2  1995/11/17  08:58:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:16:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:22:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __DECNET_PARSER_H__
#define __DECNET_PARSER_H__

/*
 * Prototypes for parser specific functions.  These are separate
 * from decnet.h so as to reduce the needless dependence of lots 
 * of source files on config.h
 */

extern void decnetmap_command(parseinfo *, dnnet *);
extern void decnet_clear_counters(parseinfo *);

#endif __DECNET_PARSER_H__
