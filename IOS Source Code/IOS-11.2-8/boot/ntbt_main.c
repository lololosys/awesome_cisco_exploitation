/* $Id: ntbt_main.c,v 3.2 1995/11/17 07:41:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/ntbt_main.c,v $
 *------------------------------------------------------------------
 * ntbt_main.c -- Main for building a netbootable partial ROM monitor image:
 *
 * 16-August-1989, Chris Shaker
 *
 * Copyright (c) 1989-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntbt_main.c,v $
 * Revision 3.2  1995/11/17  07:41:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../sys/h/master.h"
#include "../reason.h"

/*
 * _start
 * The world begins here.
 * This must be the first code in the text segment
 */

_start (code, arg)
    int code, arg;
{
    long r_d0, r_d1, r_d2, r_d3, r_d4, r_d5, r_d6, r_d7,
    r_a0, r_a1, r_a2, r_a3, r_a4, r_a5, r_a6, r_ss,
    r_us, cause, r_sr, r_pc;

    asm("	.globl enter");
    asm("enter:");

    /*
     * Use GNU gcc magic to move most of our current register values
     * into arguments for monitor():
     */
    asm("	movl	d0,%0" : "=g" (r_d0));
    asm("	movl	d1,%0" : "=g" (r_d1));
    asm("	movl	d2,%0" : "=g" (r_d2));
    asm("	movl	d3,%0" : "=g" (r_d3));
    asm("	movl	d4,%0" : "=g" (r_d4));
    asm("	movl	d5,%0" : "=g" (r_d5));
    asm("	movl	d6,%0" : "=g" (r_d6));
    asm("	movl	d7,%0" : "=g" (r_d7));

    asm("	movl	a0,%0" : "=g" (r_a0));
    asm("	movl	a1,%0" : "=g" (r_a1));
    asm("	movl	a2,%0" : "=g" (r_a2));
    asm("	movl	a3,%0" : "=g" (r_a3));
    asm("	movl	a4,%0" : "=g" (r_a4));
    asm("	movl	a5,%0" : "=g" (r_a5));
    asm("	movl	a6,%0" : "=g" (r_a6));

    asm("	movl	sp,%0" : "=g" (r_ss));
    r_us = 0;
    cause = EXC_ABORT;
    r_sr = 0;
    r_pc = 0;

    monitor(r_d0, r_d1, r_d2, r_d3, r_d4, r_d5, r_d6, r_d7,
	    r_a0, r_a1, r_a2, r_a3, r_a4, r_a5, r_a6, r_ss,
	    r_us, cause, r_sr, r_pc);

    return(TRUE);
}
