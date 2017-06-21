/* $Id: loopback.h,v 3.3 1995/11/17 09:30:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/loopback.h,v $
 *------------------------------------------------------------------
 * Prototypes for loopback.h
 *
 * January 1993, Dave Katz
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: loopback.h,v $
 * Revision 3.3  1995/11/17  09:30:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:53:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

boolean loopback_parser_createidb(uint64 *, parseinfo *);
