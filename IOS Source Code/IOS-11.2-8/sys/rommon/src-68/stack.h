/* $Id: stack.h,v 3.2 1995/11/17 18:48:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-68/stack.h,v $
 *------------------------------------------------------------------
 * stack.h
 *
 * December 1994, Rob Clevenger
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for stack manipulating routines for the 68k.
 *------------------------------------------------------------------
 * $Log: stack.h,v $
 * Revision 3.2  1995/11/17  18:48:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __STACK_H__
#define __STACK_H__

/*
** Do not change the order of the items in this structure.
** To do so will transpose the data fields in the sys_ret_info
** array (old ROM Mon BOOTDATA mechanism).
*/
typedef struct stack_trace_info {
    unsigned long *fp;  /* frame pointer */
    unsigned long ra;   /* return address */
} stack_trace_info_t;

/* default number of stack frames to process */
#define NUM_STACK_FRAMES 5

extern stack_trace_info_t *entry_stack_sav_p;

#endif /* __STACK_H__ */

