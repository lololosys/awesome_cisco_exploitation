/* $Id: random.c,v 3.2 1995/11/17 17:56:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/util/random.c,v $
 *------------------------------------------------------------------
 * random.c - Random number generator
 *
 * March 1994 Dave Stine
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: random.c,v $
 * Revision 3.2  1995/11/17  17:56:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:13:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * MODMULT computes s*b mod m, provided that m=a*b+c and
 * 0 <= c < m.
 */

#define MODMULT(a,b,c,m,s) q=s/a; s=b*(s-a*q) - c*q; if (s<0) s+=m;
static long	s1 = 1;
static long	s2 = 1;

/*
 * random_init
 * Called to initialize random_gen.
 */
void
random_init (long s1_initial, long s2_initial)
{
    s1 = s1_initial;
    s2 = s2_initial;
}

/*
 * random_gen
 * Generate a random number in the range of 0 to 10,000.
 */

long
random_gen (void)
{
    long	q;
    long	z;

    MODMULT (53668, 40014, 12211, 2147483563L, s1);
    MODMULT (52774, 40692, 3791, 2147483399L, s2);
    z = s1 - s2;
    if (z < 1)
	z += 2147483562;
    return (z / 214749);
}

#ifdef TEST
#include <stdio.h>
main()
{
    char	response[10];
    int		resp;
    int		i;

    printf("\nGenerate how many numbers? ");
    if (gets(response))
	resp = atoi(response);
    else
	return;
    for (i = 0; i < resp; ++i)
	printf("\n Random #: %d", random_gen());
    putchar('\n');
    return (0);
}
#endif



