/* $Id: imagetest.c,v 3.2.58.1 1996/03/21 23:27:19 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/imagetest.c,v $
 *------------------------------------------------------------------
 * imagetest.c
 *
 * February 1992, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Checksum this (monitor) image.
 *------------------------------------------------------------------
 * $Log: imagetest.c,v $
 * Revision 3.2.58.1  1996/03/21  23:27:19  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:25  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:42:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Perform monitor image checksum test.
*/

#include "monitor.h"
#include "error.h"

#ifndef KSEG1_ADRSPC_PROM
#define KSEG1_ADRSPC_PROM ADRSPC_PROM
#endif

/*
 * Externals
 */
extern unsigned char etext[], evece[];
extern int netflashbooted;

/*
 * Locals - note checksum is initialized so it goes into text segment
 * on 68k boxes
 */ 
static unsigned int checksum=0;

/*
 * imagesumtest : Calculate and verify image checksum
 */
int
imagesumtest(void)
{
    unsigned char *addrptr;
    unsigned int temp;
    unsigned short sum;
    int i;

    testname("monitor image checksum");
    sum = 0;  /* initialize */
    temp = checksum;

    if ( netflashbooted )
	addrptr = (unsigned char *)evece;
    else
	addrptr = (unsigned char *)KSEG1_ADRSPC_PROM;

    for ( ; addrptr<(unsigned char *)etext; addrptr++ ) {
	sum += *addrptr;	
#ifdef DEBUG
	printf("%#x  ",sum);
#endif
    }

    /* 
     * On Sierra, checksum is in the data segment, which is not
     * included in the text segment, hence there is no need to
     * adjust for the value of checksum. On 68k platforms we need
     * to take out checksum value from sum
     */
#if !defined(SIERRA) && !defined(RSP) && !defined(RSP2)
    for( i=0 ; i<sizeof(int) ; i++ ) {
	sum -= (unsigned short)(temp&0xff);
	temp = temp>>8;  /* get next byte */
    }
#endif

    if(sum != (unsigned short)checksum){
	cterr('f',0,"eprom chksum failed - expected %#x, got %#x",
	      (unsigned short)checksum, sum);
	if ( netflashbooted ) {
	    printf("Note, this could be due to GDB breakpoints. Try removing all\n");
	    printf("breakpoints and repeating the test\n");
	}
	return(1);
    }
    prcomplete(testpass,errcount,0);
    return(0);
}

/* End of Module */
