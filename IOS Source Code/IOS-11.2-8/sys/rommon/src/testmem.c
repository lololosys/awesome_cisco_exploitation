/* $Id: testmem.c,v 3.2.58.1 1996/03/21 23:28:03 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/testmem.c,v $
 *------------------------------------------------------------------
 * testmem.c
 *
 * December 1992, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Memory test routines.
 *------------------------------------------------------------------
 * $Log: testmem.c,v $
 * Revision 3.2.58.1  1996/03/21  23:28:03  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:22  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:43:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Test the specified range of memory as longwords.
**
** A tmemp->flag of INDIAGS means that the test was called from
** the diagnostic menu system.
**
** The following characteristics apply:
**   if tmemp->passcount == 0     Do test forever
**   else                         Do specified number of passes
**
**   if pass == 0                 Pass number will not be printed
*/
#include "monitor.h"
#include "testmem.h"
#include "error.h"
#include "signal_4k.h"

static char memerrmsg[] = 
"memory test error at location 0x%06x on pass %d\n\
  expected 0x%08x  is 0x%08x\n";
static int testmemphase();
int phase;

int
testmem(struct testmem *tmemp)
{
    register unsigned pat, newpat;
    
    while(1) {
	phase = 1;
	pat = 0x5555aaaa;
	newpat = 0xaaaa5555;
	fillword(tmemp->start, tmemp->length, pat);
	if(testmemphase(tmemp, pat, newpat) < 0) return(-1);  /* chkrbd test */
	pat = newpat;
	if(tmemp->flag & QUICKTEST) newpat = 0;  /* lv memory zeroed */
	else newpat = 0xfffffffe;
	if(testmemphase(tmemp, pat, newpat) < 0) return(-1);
	if(tmemp->flag & QUICKTEST) break;
	do {  /* walking zero test */
	    pat = newpat;
	    newpat = (newpat << 1) + 1;
	    if(newpat == 0xffffffff) newpat = 0x01;
	    if(testmemphase(tmemp, pat, newpat) < 0) return(-1);
	} while(newpat != 0x01);
	do {  /* walking zero test */
	    pat = newpat;
	    newpat <<= 1;
	    if(testmemphase(tmemp, pat, newpat) < 0) return (-1);
	} while(newpat);
	if(tmemp->passcount == 1) {  /* one pass is finished */
	    break;
	}
	if(tmemp->passcount && testpass >= tmemp->passcount)
	    break;  /* we have reached our passcount */
	testpass++;
    }
    prcomplete(testpass,errcount, 0);
    return(0);
}

static
testmemphase(tmemp, pat, newpat)
    register struct testmem *tmemp;
    register unsigned pat, newpat;
{
    register unsigned value;
    register unsigned *wrptr, *rdptr, *end;
    int retval = 0;  /* init */
    
    if(!(tmemp->flag & (QUICKTEST | MUTE))) {
	prpass(testpass, "phase %d,  ", phase++);
    }
    end = (unsigned *)((unsigned)tmemp->start + tmemp->length);
    wrptr = tmemp->start;
    if(tmemp->flag & RDADDR) rdptr = tmemp->rdaddr;
    else rdptr = wrptr;
    while(wrptr < end) {
	if((value = *rdptr) != pat) {  /* check location for pattern */
	    cterr('w',0,memerrmsg,rdptr,testpass,pat,value);
	    retval = -1;
	    if(tmemp->flag & QUITONERR) {
		if(!(tmemp->flag & INDIAGS)) {
		    printf("test stopped on error\n");
		}
		return(-1);
	    }
	}
	*wrptr++ = newpat;  /* write new pattern into location */
	rdptr++;
    }
    return(retval);
}


static int cmpvalue();
/*
** Perform memory refresh test.
*/

int
memrefresh(struct testmem *tmemp)
{
    extern long memsize;
    unsigned pattern, *addr;
    short i;
    int tmp=0;
    long delaycnt, delay;

    delaycnt = timer_calibrate(40000); /* 40 ms */
    fillword(tmemp->start, tmemp->length, (unsigned)0x5a5aa5a5);
    for(i=0;i<375;i++){
	delay = delaycnt;
	while(delay > 0)   delay--; /* wait for about 15 seconds */
    }
    if(cmpvalue(tmemp->start, tmemp->length, (unsigned)0x5a5aa5a5)) {
	cterr('f',0,"system failed to refresh the memory");
	return(-1);
    } 
    else {
	prcomplete(testpass, errcount, 0);
	return(0);
    }
}

static
int cmpvalue(unsigned *addr, unsigned length, unsigned val)
{
    unsigned tmpval;
    register unsigned *end = (unsigned *)((unsigned)addr + length);

    while(addr < end) {
	tmpval = *addr++;
	if (tmpval!=val) return(1);
    }
    return(0);
}



