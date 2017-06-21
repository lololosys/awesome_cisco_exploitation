/* $Id: ieee_float.c,v 1.1.2.2 1996/08/20 20:18:02 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/ieee_float.c,v $
 *------------------------------------------------------------------
 * IEEE Floating Point basic definitions
 *
 * July 1996, Fred Baker
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ieee_float.c,v $
 * Revision 1.1.2.2  1996/08/20  20:18:02  fred
 * CSCdi66538:  implement integrated services objects
 *         integrate code review comments
 * Branch: California_branch
 *
 * Revision 1.1.2.1  1996/08/20  10:24:20  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 1.1  1996/07/24  21:48:51  fred
 * Placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"

/*#include "../os/signal.h"*/
#include "ieee_float.h"

static inline boolean ieee_float_is_zero (ieee_float number)
{
    return(!(number & ~IEEE_SIGN_MASK));
}

/*
 * simple conversion: ieee floating point to long
 */
long ieee_to_long (ieee_float number)
{
    long sign;
    long exponent;
    long mantissa;

    sign = number & IEEE_SIGN_MASK;
    exponent = number & IEEE_EXPONENT_MASK;
    mantissa = number & IEEE_MANTISSA_MASK;

    if (ieee_float_is_zero(number)) {
	/* number is zero, unnormalized, or not-a-number */
	return 0;
    }
    if (IEEE_INFINITY == exponent) {
	/* number is positive or negative infinity, or a special value */
	return (sign? MINUS_INFINITY: PLUS_INFINITY);
    }

    exponent = (exponent >> IEEE_MANTISSA_WIDTH) - IEEE_BIAS;
    if (exponent < 0) {
	/* number is between zero and one */
	return 0;
    }

    mantissa |= IEEE_IMPLIED_BIT;
    if (exponent <= IEEE_MANTISSA_WIDTH)
	mantissa >>= IEEE_MANTISSA_WIDTH - exponent;
    else
	mantissa <<= exponent - IEEE_MANTISSA_WIDTH;

    if (sign)
	return -mantissa;
    else
	return mantissa;
}

/*
 * simple conversion: long to ieee floating point
 */
ieee_float long_to_ieee (long number)
{
    long exponent = IEEE_BIAS + IEEE_MANTISSA_WIDTH;
    long sign;

    /*
     * special case values
     */
    if (number == 0) {
	return 0;
    }
    if (number == PLUS_INFINITY) {
	return IEEE_INFINITY;
    }
    if (number == MINUS_INFINITY) {
	return IEEE_INFINITY | IEEE_SIGN_MASK;
    }

    /*
     * it's a normal number, return a normalized number
     */
    if (number > 0) {
	sign = 0;
    } else {
	sign = IEEE_SIGN_MASK;
	number = -number;
    }

    while (number >= 2*IEEE_IMPLIED_BIT) {
	number >>= 1;		/* normalize to the right */
	exponent++;
    }
    while (number < IEEE_IMPLIED_BIT) {
	number <<= 1;		/* normalize to the left */
	exponent--;
    }

    /*
     * at this point, we get tricky. IEEE floating point has an implied
     * 1 in bit 8 of the number; the REMAINDER of the mantissa is stored,
     * but this bit gets overwritten by the exponent. Do so...
     */
    number &= IEEE_MANTISSA_MASK;
    exponent <<= IEEE_MANTISSA_WIDTH;
    return (sign | exponent | number);
}
