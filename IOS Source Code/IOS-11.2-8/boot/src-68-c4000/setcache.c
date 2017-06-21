/* $Id: setcache.c,v 3.1 1995/11/09 09:09:51 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c4000/setcache.c,v $
 *------------------------------------------------------------------
 * setcache.c - cache enable routine
 *
 * April 1992, Rob Clevenger
 *
 * Copyright (c) 1992-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: setcache.c,v $
 * Revision 3.1  1995/11/09  09:09:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:34:10  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1992/04/15  02:11:58  clev
 * Initial checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:08  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * set caches on or off, works in 4 modes:
 * $    -- toggle instr & data cache state
 * $ d  -- toggle data cache state
 * $ i  -- toggle instr cache state
 * $ 0xwhatever -- set cacr to 0xwhatever 
 *
 */
#include "pcmap.h"
static char enamsg[] = "enabled";
static char dismsg[] = "disabled";

void setcache(cp)
char *cp;
{
    int orig, new;

    orig = getcacr();
    switch ( *cp ) {
    case '0':
	new = gethexnum(cp+2);
	break;
    case 'I':
	if(orig & CACHE_I) new = orig & ~CACHE_I;
	else new = orig | CACHE_I;
	break;
    case 'D':
	if(orig & CACHE_D) new = orig & ~CACHE_D;
	else new = orig | CACHE_D;
	break;
    case '\0':
	if(orig & CACHE_I || orig & CACHE_D) new = CLEAR_CACHES;
	else new = ENABLE_CACHES;
	break;
    default:
	printf("illegal cache command.\n");
	new = orig;
	break;
    }
    printf("set cacr to %x from %x\n",new, orig);
    printf("instruction cache %s\n", new & ENA_ICACHE ? enamsg : dismsg );
    printf("instruction burst %s\n", new & INST_BURST ? enamsg : dismsg );
    printf("data cache %s\n", new & ENA_DCACHE ? enamsg : dismsg );
    printf("data burst %s\n", new & DATA_BURST ? enamsg : dismsg );
    printf("write allocate %s\n", new & WRITE_ALLOCATE ? enamsg : dismsg );
    setcacr(new);
}
