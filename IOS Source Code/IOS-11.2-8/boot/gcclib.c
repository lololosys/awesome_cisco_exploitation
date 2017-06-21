/* $Id: gcclib.c,v 3.2 1995/11/17 07:41:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/gcclib.c,v $
 *------------------------------------------------------------------
 * gcclib.c - GNU GCC library stub support
 *
 * August 1988, Greg Satz
 *
 * Copyright (c) 1988-1992 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Replace GNU library with these stubs so we don't have to provide
 * any GNU written code in the cisco product thus avoiding any GNU
 * copylefts.
 *------------------------------------------------------------------
 * $Log: gcclib.c,v $
 * Revision 3.2  1995/11/17  07:41:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * __udivsi3
 * Unsigned division
 */

unsigned int __udivsi3 (one, two)
    unsigned int one, two;
{
    return one / two;
}

/*
 * __umodsi3
 * Unsigned modulus
 */

unsigned int __umodsi3 (one, two)
    unsigned int one, two;
{
    return one % two;
}

/*
 * __divsi3
 * Signed dibision
 */

int __divsi3 (one, two)
    int one, two;
{
    return one / two;
}

/*
 * __mulsi3
 * Signed multiplication
 */

int __mulsi3 (one, two)
    int one, two;
{
    return one * two;
}

/*
 * __modsi3
 * Signed modulus
 */

int __modsi3 (one, two)
    int one, two;
{
    return one % two;
}
