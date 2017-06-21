/* $Id: stack.c,v 3.2 1995/11/17 18:48:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-68/stack.c,v $
 *------------------------------------------------------------------
 * stack.c
 *
 * December 1994, Rob Clevenger
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Stack manipulating routines for the 68k.
 *------------------------------------------------------------------
 * $Log: stack.c,v $
 * Revision 3.2  1995/11/17  18:48:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "mon_defs.h"
#include "stack.h"
#include "exception.h"
#include "context.h"

void
save_stack_trace(unsigned long *fp, stack_trace_info_t *stip,
		int numframes)
{
    int i;

    /* process requested number of frames */
    for(i=0; i < numframes; stip++, i++) {
	if(fp && !chkberr(fp, BW_32BITS, 1) &&
	   !chkberr(fp+1, BW_32BITS, 1)) {
	    stip->ra = *(fp + 1);
	    stip->fp = fp;
	    fp = (unsigned long *)*fp;
	} else {
	    fp = (unsigned long *)0;
	    stip->ra = 0;
	    stip->fp = fp;
	}
    }
}

/*
 * show_stack : Command line interface for stack_trace, default is 5 frames
 *              also doubles as the frame command
 *
 * stack [numframes]
 * frame num
 */

int
show_stack(int argc, char *argv[])
{
    int i;
    unsigned long *fp, *next_fp, ra;
    long framenum = NUM_STACK_FRAMES;
    stack_trace_info_t *stip = entry_stack_sav_p;
    char frame_cmd = 0;
    char *str;

    /* was this command invoked as "frame" */
    if(strcmp(argv[0], "frame") == 0) {
	if(stip) {  /* this is a saved stack */
	    printf("Sorry, you can use this command on a live stack only\n");
	    return(1);
	}
	frame_cmd = 1;
	if(argc != 2) {  /* must have a frame number */
badframecmd:
	    str = "<framenum>";
usage:
	    printf("usage: %s %s\n", argv[0], str);
	    return(1);
	}
    }

    if(argc > 1) {
	if(getnum(argv[1], 10, &framenum) <= 0) {
	    if(frame_cmd) goto badframecmd;
	    str = "[<numframes>]";
	    goto usage;
	}
    }

    fp = (unsigned long *)entry_context->a6;  /* pick up first frame pointer */

    if(fp) {       /* it must be non-zero */
	if(frame_cmd) framenum++;  /* for the for loop below */
	else printf("Stack trace:\nPC = %#.8x\n", entry_context->esf.pc);
	for(i=0; i < framenum; i++) {
	    if(stip) {  /* this is a saved stack */
		ra = stip->ra;
	    } else {    /* this is a live stack */
		if(!chkberr(fp, BW_32BITS, 1) &&
		   !chkberr(fp+1, BW_32BITS, 1)) {
		    ra = *(fp + 1);
		} else {
		    ra = 0;
		}
	    }
	    if(fp && ra) {
		if(!frame_cmd || i == (framenum - 1)) {
		    printf("Frame %.2d: FP = %#.8x    RA = %#.8x\n", i,
			   fp, ra);
		}
		if(frame_cmd && i == (framenum - 1)) {
		    next_fp = (unsigned long *)(*fp);
		    fp += 2;  /* skip over RA */
		    for(i = 8; fp < next_fp; i += 4, fp++) {
			printf("at %#.8x (fp + %#.2x) = %#.8x\n", 
			       fp, i, *fp);
			if(i > 40) break;
		    }
		    return(0);
		}
		if(stip) {    /* this is a saved stack */
		    stip++;   /* bump to next entry */
		    fp = stip->fp;
		} else {      /* this is a live stack */
		    fp = (unsigned long *)*fp;
		}
	    } else {
		if(!stip) {   /* this is a live stack */
		    printf("Suspect bogus FP = %#.8x, aborting\n",
			   fp);
		}
		break;
	    }
	}
    } else {
	printf("No stack trace available\n");
	return(1);
    }
    return(0);
}

/* end of module */



