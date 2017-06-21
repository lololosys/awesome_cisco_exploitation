/* $Id: berrtest.c,v 3.2.58.1 1996/03/21 23:33:28 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-68/berrtest.c,v $
 *------------------------------------------------------------------
 * berrtest.c
 *
 * November 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Bus error test and related routines.
 *------------------------------------------------------------------
 * $Log: berrtest.c,v $
 * Revision 3.2.58.1  1996/03/21  23:33:28  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:46  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:48:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "mon_defs.h"
#include "signal_4k.h"
#include "error.h"

extern volatile unsigned berr_addr;

/*
** Test the bus error mechanism.
*/
int
berrtest()
{
    int temp;

    testname("bus error");

    temp = chkberr(ADRSPC_BAD_ADDR, BW_32BITS, 0);  /* a known invalid addr */
    switch(temp) {  
    case 0:
	cterr('f',20,"expected bus error did not occur");
	return(1);
    case 1:
	cterr('f',21,"expected bus error on write did not occur");
	return(1);
    case 2:
	cterr('f',22,"expected bus error on read did not occur");
	return(1);
    case 3:
	if(berr_addr != ADRSPC_BAD_ADDR)
	    cterr('f',21,"bus errror address incorrect");
	break;
    default:
	cterr('f',23,"unexpected result %d from chkberr()", temp);
	break;
    }

    prcomplete(testpass,errcount,(char *)0);
    return(0);
}

/*
** Check to see if a read and write to the given address
** will create a bus error.
** The size argument is the operation size (1=byte, 2=word, 4=lword,
** and 8=dlword [if supported on hardware]) - defaults to byte.
** Set the readonly arg to zero if read/write test is to be done.
** Note, we will do the write before the read, due to the fact that
** the read results in berr_addr getting set to the virtual address
** where as the write bus error results in berr_addr being set to
** the physical address on MIPS platforms.
**
** Returns:
**    0   No bus error on read or write.
**    1   Bus error only on read.
**    2   Bus error only on write.
**    3   Bus error on read and write.
*/
int
chkberr(volatile unsigned long *address, int size, unsigned readonly)
{
    int retval = 0;  /* init */
    unsigned long temp;
    SIG_PF savfcn;

    /*
     * Capture bus error signals
     */
    savfcn = signal(SIGBUS,SIG_IGN);

    if(!readonly) {
	hkeepflags &= ~H_BUSERR;  /* initialize flag */

	switch(size) {
	case BW_32BITS:	/* lword access */
	    *address = retval;
	    BUSERR_NOP_X4();
	    break;
	case BW_16BITS:	/* word access */
	    *(volatile unsigned short *)address = retval;
	    BUSERR_NOP_X4();
	    break;
	case BW_8BITS:	/* byte access */
	    *(volatile unsigned char *)address = retval;
	    BUSERR_NOP_X4();
	    break;
	default:
bad_size:
	    printf("chkberr: unsupported access size: %d bytes\n", size);
	    return(-1);
	}
	if(hkeepflags & H_BUSERR) retval += 2;
    }

    hkeepflags &= ~H_BUSERR;  /* initialize flag */
    switch(size) {
    case BW_32BITS:   	/* lword access */
	temp = *address;
	BUSERR_NOP_X4();
	break;
    case BW_16BITS:   	/* word access */
	temp = *(volatile unsigned short *)address;
	BUSERR_NOP_X4();
	break;
    case BW_8BITS:	/* byte access */
	temp = *(volatile unsigned char *)address;
	BUSERR_NOP_X4();
	break;
    default:
	goto bad_size;
    }
    if(hkeepflags & H_BUSERR) retval += 1;
    signal(SIGBUS,savfcn);
    return(retval);
}

/* End of Module */
