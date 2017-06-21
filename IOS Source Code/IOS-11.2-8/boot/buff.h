/* $Id: buff.h,v 3.2 1995/11/17 07:40:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/buff.h,v $
 *------------------------------------------------------------------
 * buff.h -- Definitions for buffer structures and unions
 *
 * January 1990, Chris Shaker
 *
 * Copyright (c) 1990-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: buff.h,v $
 * Revision 3.2  1995/11/17  07:40:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:03:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:11:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	BUFSIZ	512

struct buffer_halves {
    char buf1[BUFSIZ/2];		/* input buffer */
    char buf2[BUFSIZ/2];		/* input buffer */
};

union buffer_union {
    char buf[BUFSIZ];
    struct buffer_halves bufh;
};

#define VSTRING_BUFF_SIZE  257
