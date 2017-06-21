/* $Id: ieee_float.h,v 1.1.2.2 1996/08/20 20:18:03 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/ieee_float.h,v $
 *------------------------------------------------------------------
 * IEEE Floating Point basic definitions
 *
 * July 1996, Fred Baker
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ieee_float.h,v $
 * Revision 1.1.2.2  1996/08/20  20:18:03  fred
 * CSCdi66538:  implement integrated services objects
 *         integrate code review comments
 * Branch: California_branch
 *
 * Revision 1.1.2.1  1996/08/20  10:24:21  fred
 * CSCdi66538:  implement integrated services objects (RSVP)
 * Branch: California_branch
 *
 * Revision 1.1  1996/07/24  21:48:51  fred
 * Placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _IEEE_FLOAT_H_
#define _IEEE_FLOAT_H_

#define IEEE_NUMBER_WIDTH	32	/* bits in number */
#define IEEE_EXP_WIDTH		8	/* bits in exponent */
#define IEEE_MANTISSA_WIDTH	23	/* IEEE_NUMBER_WIDTH - 1 - IEEE_EXP_WIDTH */

#define IEEE_SIGN_MASK		0x80000000
#define IEEE_EXPONENT_MASK	0x7F800000
#define IEEE_MANTISSA_MASK	0x007FFFFF
#define IEEE_INFINITY		IEEE_EXPONENT_MASK

#define IEEE_IMPLIED_BIT (1 << IEEE_MANTISSA_WIDTH)
#define IEEE_INFINITE ((1 << IEEE_EXP_WIDTH) - 1)
#define IEEE_BIAS ((1 << (IEEE_EXP_WIDTH - 1)) - 1)

#define MINUS_INFINITY 0x80000000
#define PLUS_INFINITY  0x7FFFFFFF

typedef long ieee_float;	/* yes, this is truly a hack */

long ieee_to_long(ieee_float number);
ieee_float long_to_ieee(long number);
#endif /* _IEEE_FLOAT_H_ */
