/* $Id: stub_ntbt.c,v 3.2 1995/11/17 07:41:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/stub_ntbt.c,v $
 *------------------------------------------------------------------
 * stub_ntbt.c -- stub routines for a netbooteable ROM monitor image
 *
 * 17-August-1989, Chris Shaker
 *
 * Copyright (c) 1989-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stub_ntbt.c,v $
 * Revision 3.2  1995/11/17  07:41:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

restart(reason, pc, buserror_address, stack_trace_ptr)
    long reason;
    long pc;
    long buserror_address;
    long stack_trace_ptr;
{
    printf("restart: reason = 0x%x, pc = 0x%x\n", reason, pc);
    printf("         buserror_address = 0x%x, stack_trace_ptr = 0x%x\n",
	   buserror_address, stack_trace_ptr);
}

cacheoff() {}
cacheon() {}
stale() {}
