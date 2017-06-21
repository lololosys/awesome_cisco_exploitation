/* $Id: breakp.c,v 3.2 1995/11/17 07:40:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/breakp.c,v $
 *------------------------------------------------------------------
 * breakp.c -- functions that handle the break point traps
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: breakp.c,v $
 * Revision 3.2  1995/11/17  07:40:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:03:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:11:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"
#include "ctype.h"
#include "reason.h"
#include "extern.h"
#include "stdio.h"

/*
 * Sets up a break point
 */
dobreak(s)
	char *s;
{
	register short *bp = breakaddr;
	register short *bpa;

	if (bp && !breakf && *bp != 0x4E41)  /* Trap 1 */
		bp = 0;			/* Got trashed? */
	if (!isxdigit(*s)) {
		printf("Break: %#x?", bp);
		gets(bufu.buf);
		bpa = (short *)gethexnum(bufu.buf);
	} else
		bpa = (short *)gethexnum(s);
	if ((long)bpa > 0) {
		breakf = FALSE;
		if (bp)
			*bp = breakvalue;
		breakvalue = *bpa;
		if (bpa)
			*bpa = 0x4E41;	/* Trap 1 */
		breakaddr = bpa;
	}
}

/*
 * Handle break point trap
 */
takebreak(pcp)
	long *pcp;
{
	*pcp -= 2;			/* back up to broken word */
	printf("\nBreak at %#x\n", *pcp);
	breakf = TRUE;
	*breakaddr = breakvalue;
}
