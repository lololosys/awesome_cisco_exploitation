/* $Id: ioprobetest.c,v 3.1 1995/11/09 09:08:34 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-ags/ioprobetest.c,v $
 *------------------------------------------------------------------
 * ioprobe.c -- I/O space probe
 *
 * August 1986, Len Bosack
 *
 * Copyright (c) 1986-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ioprobetest.c,v $
 * Revision 3.1  1995/11/09  09:08:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:14:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:33:05  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1992/03/07  23:56:25  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"
#include "stdio.h"
#include "ctype.h"
#include "m68vectors.h"
#include "reason.h"
#include "extern.h"
#include "test.h"

ioprobetest()
{
	register short *base = (short *)MBMEMLOWADR;
	register short *top = (short *)MBIOHIADR;
	register short *tp;
	short *blockbase, *blocktop;
	short incr = 1;
	volatile long temp;

	printf("IO/memory space probe\n");
	printf("\nStarting address [%#x]? ", base);
	gets(bufu.buf);
	if ((temp = gethexnum(bufu.buf)) >= 0)
		base = (short *)(temp & ~1);  /* align on word */
	printf("Ending address [%#x]? ", top);
	gets(bufu.buf);
	if ((temp = gethexnum(bufu.buf)) >= 0)
		top = (short *)(temp & ~1);  /* align on word */
	printf("Probe increment (in shorts) [%#x]? ",incr);
	gets(bufu.buf);
	if ((temp = gethexnum(bufu.buf)) > 0)
		incr = temp;
	abortstate = ABORT_INTERNAL;
	buserrhandler = buserrnote;
	printf("\nProbing from %#x to %#x with interval %#x\n", base, top, incr);
	buserrcnt = 0;
	blockbase = NULL;
	blocktop = NULL;
	for (tp = base; (tp <= top) && !abort; tp += incr) {
	    if (!blockbase) {
		temp = (long) stouchr(tp);
		if (!buserrcnt) {
		    blocktop = tp;
		    blockbase = tp;
		}
	    }
	    else {
		temp = (long) stouchr(tp);
		if (!buserrcnt) {
		     blocktop = tp;
		}
		else {
		    printf("\nRegion %#x to %#x exists (%dK)",
			blockbase, blocktop,
			(blocktop - blockbase + sizeof(blocktop)) / 512);
		    blockbase = NULL;
		    blocktop = NULL;
		}
	    }
	    buserrcnt = 0;
	}
	printf("\n\nDone\n");
	abortstate = ABORT_IGNORE;
	abort = FALSE;
	buserrhandler = 0;
}
