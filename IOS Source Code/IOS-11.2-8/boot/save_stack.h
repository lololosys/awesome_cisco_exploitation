/* $Id: save_stack.h,v 3.2 1995/11/17 07:41:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/save_stack.h,v $
 *------------------------------------------------------------------
 * save_stack.h - include file for save_stack
 *
 * July 27, 1989, Chris Shaker
 *
 * Copyright (c) 1989-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: save_stack.h,v $
 * Revision 3.2  1995/11/17  07:41:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	STACK_WORD_SIZE	sizeof(long)

struct frame {
    long fp;			/* Frame pointer */
    long pc;			/* Program counter */
};

struct frame *save_stack(long reg_fp);
