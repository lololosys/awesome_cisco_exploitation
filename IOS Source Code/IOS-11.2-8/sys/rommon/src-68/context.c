/* $Id: context.c,v 3.2.62.1 1996/06/16 21:18:39 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68/context.c,v $
 *------------------------------------------------------------------
 * context_68.c
 *
 * December 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Display the 68k context to the user.
 *------------------------------------------------------------------
 * $Log: context.c,v $
 * Revision 3.2.62.1  1996/06/16  21:18:39  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2  1995/11/17  18:48:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "exception.h"
#include "context.h"

int
show_context(int argc, char *argv[])
{
    cpu_context_t *ccp = entry_context;

    /* if PC is non-zero context must be valid */
    if(ccp->esf.pc) {
	printf("CPU Context:\n"
	       "d0 - %#.8x      a0 - %#.8x\n"
	       "d1 - %#.8x      a1 - %#.8x\n"
	       "d2 - %#.8x      a2 - %#.8x\n"
	       "d3 - %#.8x      a3 - %#.8x\n"
	       "d4 - %#.8x      a4 - %#.8x\n"
	       "d5 - %#.8x      a5 - %#.8x\n"
	       "d6 - %#.8x      a6 - %#.8x\n"
	       "d7 - %#.8x      a7 - %#.8x\n"
	       "pc - %#.8x     vbr - %#.8x\n"
	       "sr - %#.4x\n",
	       ccp->d0, ccp->a0, ccp->d1, ccp->a1,
	       ccp->d2, ccp->a2, ccp->d3, ccp->a3,
	       ccp->d4, ccp->a4, ccp->d5, ccp->a5,
	       ccp->d6, ccp->a6, ccp->d7, ccp->a7,
	       ccp->esf.pc, ccp->vbr, ccp->esf.sr);
    } else {
	printf("No context available\n");
    }
}

/* end of module */
