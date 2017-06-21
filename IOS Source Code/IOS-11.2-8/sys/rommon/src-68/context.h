/* $Id: context.h,v 3.2 1995/11/17 18:48:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-68/context.h,v $
 *------------------------------------------------------------------
 * context_68.h
 *
 * December 1994, Rob Clevenger
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Structures and defines for the 68k family CPU context.
 *------------------------------------------------------------------
 * $Log: context.h,v $
 * Revision 3.2  1995/11/17  18:48:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __CONTEXT_H__
#define __CONTEXT_H__

/*
** This defines the 68k context structure.
** This happens to be what is on the stack when an exception
** occurs in the Monitor.
*/

typedef struct cpu_context {
    unsigned long vbr;
    unsigned long d0, d1, d2, d3, d4, d5, d6, d7;
    unsigned long a0, a1, a2, a3, a4, a5, a6, a7;
    struct excpt_stack_frame esf;
} cpu_context_t;

extern cpu_context_t *entry_context;
extern void log_sys_ret(int reason, cpu_context_t *ccp);
extern void copy_context(cpu_context_t *ccp);
extern long mon_vbr_sav;
extern short mon_sr_sav;

#endif /* __CONTEXT_H__ */
