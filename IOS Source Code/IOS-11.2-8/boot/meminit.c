/* $Id: meminit.c,v 3.2.58.1 1996/05/09 13:59:57 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/meminit.c,v $
 *------------------------------------------------------------------
 * meminit.c -- memory initialization
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: meminit.c,v $
 * Revision 3.2.58.1  1996/05/09  13:59:57  rbadri
 * Branch: California_branch
 * Synergy features
 *
 * Revision 3.2.88.1  1996/04/27  06:12:57  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.78.1  1996/04/08  01:34:18  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  09:06:51  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.50.1  1996/03/03  21:18:56  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.48.1  1996/03/02  01:06:35  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.32.1  1996/02/29  07:41:53  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  07:41:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:35  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/10/03  07:04:34  rlowe
 * Re-port IOS Boot for Synergy ATM line card from 10.3 to 11.0.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/06/07  19:12:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../stdio.h"
#include "../reason.h"
#include "../extern.h"
#include "../test.h"
#include "../m68vectors.h"

extern long evec[];

int memory_test_cell;		/* Cell used for memory testing */

#define	ALLOCHUNK	16	/* minimum memory size quantum */

#define PATTERN1 ((unsigned)0x33333333)
#define PATTERN2 ((unsigned)0xCCCCCCCC)

notice_wraparound(loc)
    u_int *loc;
{
    printf("Detected wraparound:\n memory at address 0x%x changed from 0 to 0x%x after write to address 0x%x\n",
	   &memory_test_cell, ltouchr(&memory_test_cell), (u_int)loc);
}

explain_error(location, expected, got)
    u_int *location;
    u_int expected, got;
{
    printf("at 0x%x, expected 0x%x, got 0x%x ",
	   location, expected, got);
}

meminit(diag)
long diag;
{
    register int numpages, allpages;
    register u_int *ptr;
    
    /* Method used is to run down memory writing the page
     * number into selected spots. If wraparound ocurrs,
     * memory_test_cell will be trashed. If wraparound is detected,
     * complain and truncate memory. If no wraparound is detected,
     * read through these same words to verify that each page
     * contains its page number. Then, we know how many pages there are.
     * In addition, check adjacent locations can hold
     * alternate bit patterns. The simpler algorithim
     * has the failing that not all ram need be present
     * for it to claim the page is good.
     */
    
    if (diag)
	puts("writing...");
    buserrhandler = buserrnote;
    buserrcnt = 0;		/* Reset bus error count */
    *EVEC_BUSERR = evec[2];	/* Set bus error handler */

#ifdef SYNALC
   /* Split main DRAM into both processor ram and io ram. */
   /* Just size the processor ram right here.             */
    for (numpages = 0; numpages < RAMPAGES ; numpages += ALLOCHUNK) {
#else
    for (numpages = 0; ; numpages += ALLOCHUNK) {
#endif
	ptr = (u_int *)((numpages << PAGESHIFT) + 
			((uchar *)&memory_test_cell));
	
	/*
	 * Write the current page number at the start of each page.
	 * Check for wraparound (see if first page got stomped).
	 */
	ltouchw(ptr++, numpages);

        /* NOTE: On a CSC/4 with 4MB DRAM, when a write occurs to an
         * address between 4MB & 16MB, forcing buserror, the write
         * will still happen aliasing down into the appropriate
         * region.  This "feature" is accounted for below by rewriting
         * memory_test_cell with "0" before the read-loop attempts to 
         * check it.
         */
        if (buserrcnt) {   /* We've reached the end of DRAM */
	   ltouchw(&memory_test_cell, 0); /* Reset memory_test_cell */
                                          /* in case it was trashed */
           break;                         /* by aliasing */
        }
    
	if (ltouchr(&memory_test_cell) != 0) {
	    notice_wraparound(ptr - 1);
	    break;
	}
	
	/*
	 * Write PATTERN1 into the next location and check for wraparound:
	 */
	ltouchw(ptr++, PATTERN1);
	if (ltouchr(&memory_test_cell) != 0) {
	    notice_wraparound(ptr - 1);
#ifdef  SYNALC
                /* SYNALC normally does NOT bus error at end of DRAM; */
                /* a wraparound WILL occur.  Fix up memory_test_cell. */
            ltouchw (& memory_test_cell, 0) ;

       /* On a wrap, all of physical DRAM has been sized. */
       /* Do NOT count io ram or reserved ram portions.   */
       /* Keep adjustment in terms of whole multiples of  */
       /* ALLOCHUNK pages.                                */
       allpages = (((ADRSPC_SYNALC_TRU_RAM_TAIL / PAGESIZE) +
            (ALLOCHUNK - 1)) / ALLOCHUNK) ;
       numpages -= (allpages * ALLOCHUNK) ;
#endif

	    break;
	}
	
	/*
	 * Write PATTERN2 into the next location and check for wraparound:
	 */
	ltouchw(ptr, PATTERN2);
	if (ltouchr(&memory_test_cell) != 0) {
	    notice_wraparound(ptr - 1);
	    break;
	}
    }
    
    *EVEC_BUSERR = (long)catchbe; /* Ignore bus errors from here on */
    allpages = numpages;

    if (diag)
	puts("reading...");
    for (numpages = 0; numpages < allpages; numpages += ALLOCHUNK) {
	ptr = (u_int *)((numpages << PAGESHIFT) + 
			((uchar *)&memory_test_cell));
	if (ltouchr(ptr++) != numpages) {
	    if (diag)
		explain_error(ptr - 1, numpages, ltouchr(ptr - 1));
	    break;
	}
	if (ltouchr(ptr++) != PATTERN1) {
	    if (diag)
		explain_error(ptr - 1, PATTERN1, ltouchr(ptr - 1));
	    break;
	}
	if (ltouchr(ptr) != PATTERN2) {
	    if (diag)
		explain_error(ptr - 1, PATTERN2, ltouchr(ptr - 1));
	    break;
	}
    }
    
    if (diag) {
	putnum(numpages);
	puts(" pages...");
    }
    return(numpages);
}
