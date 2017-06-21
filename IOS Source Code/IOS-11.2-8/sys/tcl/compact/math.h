/* $Id: math.h,v 3.2 1995/11/17 18:47:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tcl/compact/math.h,v $ 
 *------------------------------------------------------------------
 * math.h
 * April 1994, Brad Benson
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: math.h,v $
 * Revision 3.2  1995/11/17  18:47:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:34:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:05:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*	@(#)math.h 1.30 89/07/20 SMI	*/

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * Math library definitions for all the public functions implemented in libm.a.
 */

#ifndef	__math_h
#define	__math_h

/*
 * Posix (actually ansi C) section
 */
#define	HUGE_VAL	(__infinity())	/* Produces IEEE Infinity. */

/* 	Constants, variables, and functions from System V */

#define	_ABS(x) ((x) < 0 ? -(x) : (x))

#define	HUGE		(infinity())	/* For historical compatibility. */

#define	DOMAIN		1
#define	SING		2
#define	OVERFLOW	3
#define	UNDERFLOW	4
#define	TLOSS		5
#define	PLOSS		6

struct exception {
	int type;
	char *name;
	double arg1;
	double arg2;
	double retval;
};

/*
 * First three have to be defined exactly as in values.h including spacing!
 */
#define	M_LN2	0.69314718055994530942
#define	M_PI	3.14159265358979323846
#define	M_SQRT2	1.41421356237309504880

#define	M_E		2.7182818284590452354
#define	M_LOG2E		1.4426950408889634074
#define	M_LOG10E	0.43429448190325182765
#define	M_LN10		2.30258509299404568402
#define	M_PI_2		1.57079632679489661923
#define	M_PI_4		0.78539816339744830962
#define	M_1_PI		0.31830988618379067154
#define	M_2_PI		0.63661977236758134308
#define	M_2_SQRTPI	1.12837916709551257390
#define	M_SQRT1_2	0.70710678118654752440
#define	_REDUCE(TYPE, X, XN, C1, C2)	{ \
	double x1 = (double)(TYPE)X, x2 = X - x1; \
	X = x1 - (XN) * (C1); X += x2; X -= (XN) * (C2); }
#define	_POLY1(x, c)    ((c)[0] * (x) + (c)[1])
#define	_POLY2(x, c)    (_POLY1((x), (c)) * (x) + (c)[2])
#define	_POLY3(x, c)    (_POLY2((x), (c)) * (x) + (c)[3])
#define	_POLY4(x, c)    (_POLY3((x), (c)) * (x) + (c)[4])
#define	_POLY5(x, c)    (_POLY4((x), (c)) * (x) + (c)[5])
#define	_POLY6(x, c)    (_POLY5((x), (c)) * (x) + (c)[6])
#define	_POLY7(x, c)    (_POLY6((x), (c)) * (x) + (c)[7])
#define	_POLY8(x, c)    (_POLY7((x), (c)) * (x) + (c)[8])
#define	_POLY9(x, c)    (_POLY8((x), (c)) * (x) + (c)[9])

extern int	signgam;
#endif	/* !__math_h */
