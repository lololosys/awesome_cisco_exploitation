/* $Id: xmem.c,v 3.1 1995/11/09 09:07:35 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/mci/xmem.c,v $
 *------------------------------------------------------------------
 * xmem.c -- MCI Xbus Diagnostic Routines
 *
 * Copyright (c) 1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xmem.c,v $
 * Revision 3.1  1995/11/09  09:07:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:57:00  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../defs.h"
#include "../../types.h"
#include "../../stdio.h"
/* #include <interface.h> */
#include <ether.h>
#include <if_mci.h>
#include <mci_err.h>
#include "../../test.h"
#include "../../reason.h"
#include "../../extern.h"

#define BUFLEN		30
#define WAITDELAY	100000

/*
 *  Function declarations
 */

extern boolean	store_error(), quit_display();
boolean		mci_xnvt();
void		mci_xmemt(), mci_xaddress(), type_memx(), mci_xmeml();

/*
 *  miscellaneous external variables
 */

extern MCI_TEST	*diag_results[];
extern boolean	looping;
extern int	errors;

/*
 *  mci_xmemt
 *  Xbus diagnostic menu
 */

void mci_xmemt(mci)
    
    register mcitype *mci;
{
    ushort	sresult;
    long	mlow, mhigh;
    int		i, rcount, waiting, linect, size;
    ulong	readl, writel;
    boolean	parsed, reporting;
    char	buff[BUFLEN], xcmdchar;
    
    
    /*
     * Ask about which kind of test
     *
     */
    
    waiting = WAITDELAY;
    mlow = mhigh = rcount = linect = 0;
    if ((i = mci_xnvs(mci)) == 0) {
	printf("\nNo NVRAM found!");
	return;
    }
    size = 0x1FFF + ((i - 1) * 0x2000);
    while (TRUE) {
	xcmdchar = '?';
	abort = FALSE;
	printf("\n\nXBUS Diagnostic (?AHLRTVWQ) [%c]", xcmdchar);
	parsed = (int) gets(buff);
	if (!null(buff) && parsed)
	    xcmdchar = cvu(buff[0]);
	switch (xcmdchar) {
	  case 'H':
	  case '?':
	    printf("\n\nA - List MEMX contents");
	    printf("\nL - Looped Access");
	    printf("\nT - Do basic tests");
	    printf("\nR - Read Current Location and Increment");
	    printf("\nV - Verify NovRAM Contents");
	    printf("\nW - Write Current Location and Increment");
	    printf("\nH or ? - Display this menu");
	    printf("\nQ - Quit XBUS Diagnostic\n");
	    break;
	  case 'A':
	    mci_xaddress(&mlow, &mhigh);
	    type_memx(mci, mlow, mhigh);
	    break;
	  case 'T':
	    reporting = TRUE;
	    rcount = 1;                    /* Set Repeat Count */
	    printf("\nRepeat Count [%d]: ", rcount);
	    if (parsed = (int) gets(buff)) {
		if (!null(buff))
		    rcount = parse_unsigned(buff);
		parsed = (rcount >= 0);
	    }
	    (void)mci_xnvt(mci, size, rcount, reporting, TRUE, &linect);
	    break;
	  case 'R':
	    readl = mci->regptr->memxlong;
	    printf ("\nMX: Read a 0x%-08x",readl);
	    break;
	  case 'V':
	    (void)mci_xver(mci, size, reporting, TRUE, &linect);
	    break;
	  case 'W':
	    writel = 0;
	    printf("\nMX: Write Word [0x%x]: ",writel);
	    if (parsed = (int) gets(buff)) {
		if (!null(buff))
		    writel = parse_unsigned(buff);
		parsed = (writel >= 0);
	    }
	    mci->regptr->memxlong = writel;
	    break;
	  case 'L':
	    printf("\nLooping Test");
	    mci_xmeml(mci, size);
	    break;
	  case 'Q':
	    return;
	    break;
	  default:
	    printf("\nUnknown command \"%c\"", xcmdchar);
	    xcmdchar = '?';
	    break;
	    
	}  /*  end switch(xcmdchar)  */
	
    }  /*  end while(TRUE)  */
    
}  /*  end mci_xmemt()  */

/*
 *  mci_xnvt
 *  basic Xbus memory test
 */

boolean mci_xnvt(mci, size, rcount, reporting, verbose, linect)
    register mcitype *mci;
    int size, rcount, *linect;
    boolean reporting, verbose;
    
{
    ushort	sresult;
    int		i, waiting;
    ulong	readl, writel;
    boolean	read, parsed;
    char	buff[BUFLEN];
    
    waiting = WAITDELAY;
    errors = 0;
    if(verbose) {
	printf("\nXT: %d bytes of NVRAM", (size * 2));    /*  show size in bytes  */
    }
    
    while (rcount > 0 ) {
	if (verbose)
	    printf("\nXT: Testing XBUS Nov RAM");
	mci->regptr->argreg = 0;                           /* Zero Pointers */
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	mci->regptr->argreg = 0;
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
	
	writel = 0; 
	if (verbose)
	    printf("\nXT: Writing Zeros...."); 
	for (i = 0; i <= size; i++)  {                          /* Write Zeros */
	    mci->regptr->memxlong = writel;
	    i++;
	}
	mci->regptr->argreg = 0;                           /* Zero Pointers */
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	mci->regptr->argreg = 0;
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
	for (i = 0; i <= size; i++) {                 /* Check Zeros */
	    readl = mci->regptr->memxlong;
	    if (readl != 0 && !looping) {
		++errors;
		if (!store_error(&diag_results[mci->unit]->xmem_test, 0, readl,
			         i, 0, 0, 0, 0))
		    return(FALSE);
		if (reporting) {
		    if (quit_display((*linect)++, NULL))
			return(FALSE);
		    printf("\nXT: ERROR At Location 0x%x,  Read 0x%-08x", i, readl);
		}
	    }
	    i++;
	}
	
	mci->regptr->argreg = 0;                           /* Zero Pointers */
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	mci->regptr->argreg = 0;
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
	writel = 0xffffffff;
	if (verbose) printf("\nXT: Writing Ones.....");
	for (i = 0; i <= size; i++) {                 /* Write Ones */
	    mci->regptr->memxlong = writel;
	    i++;
	}
	mci->regptr->argreg = 0;                         /* Zero Pointers */
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	mci->regptr->argreg = 0;
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
	for (i = 0; i <= size; i++) {
	    readl = mci->regptr->memxlong;
	    if (readl != 0xFFFFFFFF && !looping) {
		++errors;
		if (!store_error(&diag_results[mci->unit]->xmem_test, 0xFFFFFFFF,
			         readl, i, 0, 0, 0, 0))
		    return(FALSE);
		if (reporting) {
		    if (quit_display((*linect)++, NULL))
			return(FALSE);
		    printf("\nXT: ERROR At Location 0x%x, Read 0x%-08x", i, readl);
		}
	    }
	    i++;
	}
	
	mci->regptr->argreg = 0;                         /* Zero Pointers */
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	mci->regptr->argreg = 0;
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
	if (verbose)
	    printf ("\nXT: Address Test...");
	writel = 2;
	for (i = 0; i <= size; i++) {
	    mci->regptr->memxlong = writel;          /* Write Current Pointer */
	    writel++;
	    writel++;
	    i++;
	}
	
	mci->regptr->argreg = 0;                         /* Zero Pointers */
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	mci->regptr->argreg = 0;
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
	writel = 2;
	for (i = 0; i <= size; i++) {
	    readl = mci->regptr->memxlong;
	    if (readl != writel && !looping) {
		if (!store_error(&diag_results[mci->unit]->xmem_test, writel,
			         readl, i, 0, 0, 0, 0))
		    return(FALSE);
		if (reporting) {
		    if (quit_display((*linect)++, NULL))
			return(FALSE);
		    printf("\nXT: ERROR - At Location 0x%x, Read 0x%-08x",i,readl);
		    printf("\nXT: ERROR - Should be a 0x%-08x",writel);
		}
	    }
	    writel++;
	    writel++;
	    i++;
	}
	rcount--;
	
    }  /*  end while(rcount > 0)  */
    
    if (verbose && (errors > 0))
	printf("\nXT: %d errors detected", errors);
    if (errors == 0 && !looping)
	diag_results[mci->unit]->xmem_test.stat = 'P';
    if(errors)
	return(FALSE);
    else
	return(TRUE);
    
}  /*  end mci_xnvt()  */

/*
 *  mci_xver
 *  Verify NonVolatile memory
 */

boolean mci_xver(mci, size, reporting, verbose, linect)
    register mcitype *mci;
    int size, *linect;
    boolean reporting, verbose;
    
{
    ushort	sresult;
    int		i, waiting;
    ulong	readl, writel;
    boolean	read, parsed;
    char	buff[BUFLEN];
    
    waiting = WAITDELAY;
    errors = 0;
    if(verbose) {
	printf("\nXT: %d bytes of NVRAM", (size * 2));    /*  show size in bytes  */
    }
    
    if (verbose)
	printf("\nXT: Verifying XBUS Nov RAM");
    mci->regptr->argreg = 0;                         /* Zero Pointers */
    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
    writel = 2;
    for (i = 0; i <= size; i++) {
	readl = mci->regptr->memxlong;
	if (readl != writel && !looping) {
	    if ((i & 0x1fff) != 0x1ffe){
		if (!store_error(&diag_results[mci->unit]->xmem_test, writel,
			         readl, i, 0, 0, 0, 0))
		    return(FALSE);
		if (reporting) {
		    if (quit_display((*linect)++, NULL))
			return(FALSE);
		    printf("\nXT: ERROR - At Location 0x%x, Read 0x%-08x",i,readl);
		    printf("\nXT: ERROR - Should be a 0x%-08x",writel);
		}
		
	    }
	}
	writel++;
	writel++;
	i++;
    }
    
    if (verbose && (errors > 0))
	printf("\nXT: %d errors detected", errors);
    if (errors == 0 && !looping){
	printf("\n\nVerify Passed\n\n");
	diag_results[mci->unit]->xmem_test.stat = 'P';
    }
    if(errors)
	return(FALSE);
    else
	return(TRUE);
    
}  /*  end mci_xver()  */	 

/*
 * mci_xaddress
 * Get a range of memory addresses
 * This can now be up 32 bits 
 */

void mci_xaddress(low, high)
    long *low, *high;
{
    long	value;
    char	buff[BUFLEN];
    boolean	parsed;
    
    while (TRUE) {
	if (abort)
	    return;
	value = *low;
	printf("\nLow address [%#x]: ", value);
	if (parsed = (int) gets(buff)) {
	    if (!null(buff))
	        value = parse_unsigned(buff);
	    parsed = ((value >= 0) && (value <= 0xFFFFFFFF));
	}
	if (!parsed)
	    continue;
	*low = value;
	value = *high;
	printf("\nHigh address [%#x]: ", value);
	if (parsed = (int) gets(buff)) {
	    if (!null(buff))
	        value = parse_unsigned(buff);
	    parsed = ((value > 0) && (value <= 0xFFFFFFFF));
	}
	if (!parsed)
	    continue;
	*high = value;
	return;
	
    }  /*  end while(TRUE)  */
    
}  /*  end mci_xaddress()  */

/*
 *  type_memx
 *  Type out a range of locations in memx
 */

void type_memx(mci, low, high)
    register mcitype *mci;
    long low, high;
    
{
    mciregtype		*regptr;
    register int	i;
    ushort		sresult;
    ulong		lresult;
    int			linect;
    
    linect = 0;
    regptr = mci->regptr;
    regptr->argreg = (low >> 16 );           /*SWAP THIS BACK W NEW UCODE*/
    regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
    regptr->argreg = low;
    regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
    for (i = low; i < high + 1; i++) {
	lresult = regptr->memxlong;
	if (quit_display(linect++, NULL))
	    return;
	printf("\nmemx 0x%-08x: 0x%-08x", i, lresult);
	i++;
    }
    
}  /*  end type_memx()  */

/*
 *  mci_meml
 *  Continually does Reads or Writes to the Xbus
 */

void mci_xmeml(mci, size)
    register mcitype *mci;
    int size;
    
{
    int		i,j, stopc;
    ulong	writel;
    boolean	read, parsed;
    char	buff[BUFLEN], direc;
    extern char	abort;
    
    mci->regptr->argreg = 0;                         /* Zero Pointers */
    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
    mci->regptr->argreg = 0;
    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
    writel = 0xaaaaaaaa;
    
    /*
     * Check for read or write
     *
     */
    
    direc = 'R';
    printf("\nWrite Data [y]: ");
    parsed = (gets(buff) && cvu(*buff) == 'N') ? FALSE : TRUE;
    
    if(parsed) {
	printf("\nMX: Write Word [0x%x]: ",writel);
	if (parsed = (int) gets(buff)) {
	    if (!null(buff))
		writel = parse_unsigned(buff);
	    parsed = (writel >= 0);
	}
	printf("\nMX:  Starting Write Test, type BREAK to quit");
	stopc = 0;
	while (!abort) {
	    for (j = 0; j <= size; j++) {
		mci->regptr->memxlong = writel;
		j++;
	    }
	    mci->regptr->argreg = 0;                         /* Zero Pointers */
	    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	    mci->regptr->argreg = 0;
	    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
	}
    }
    
    if(!parsed) {
    	stopc = 0;
    	printf("\nMX:  Starting Read Test, type BREAK to quit");
    	while (!abort) {
	    for (j = 0; j <= size; j++) {
	    	writel = mci->regptr->memxlong;
	    	j++;
	    }
	    mci->regptr->argreg = 0;                         /* Zero Pointers */
	    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	    mci->regptr->argreg = 0;
	    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
    	}
    }
    
}  /*  end mci_xmeml()  */

/*
 *  mci_xnvs
 *  determines the size of the NVRAM
 */

mci_xnvs(mci)
    register mcitype *mci;
{
    ushort	sresult;
    int 	i, ecount, size, waiting;
    ulong	readl, writel;
    boolean	read, parsed;
    char	buff[BUFLEN];
    
    size = 0x1fff;
    waiting = WAITDELAY;
    for (i = 0; i <= 4; i++) {
	mci->regptr->argreg = 0;          
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	mci->regptr->argreg = size;
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;     
	mci->regptr->memxlong = 0xaaaa0000;
	mci->regptr->argreg = size;
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
	readl = mci->regptr->memxlong;
	if ((readl & 0xFFFF0000) != 0xaaaa0000)
	    break;
	size += 0x2000;
    }
    
    return(i);
    
}  /*  end mci_xnvs()  */

/*  end xmem.c  */
