/* $Id: berrtest.c,v 3.2.58.1 1996/03/21 23:26:33 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/berrtest.c,v $
 *------------------------------------------------------------------
 * berrtest.c
 *
 * July 1993, mbeesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Bus error test routines.
 *------------------------------------------------------------------
 * $Log: berrtest.c,v $
 * Revision 3.2.58.1  1996/03/21  23:26:33  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:19:41  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:40:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:36:43  hampton
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
berrtest(void)
{
    int temp;

    testname("bus error");
    temp = chkberr(KSEG1_ADRSPC_BAD_ADDR, BW_32BITS, 0);  /* a known invalid addr */
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
	if(berr_addr != KSEG1_ADRSPC_BAD_ADDR)
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
** multibyte_rd: Check for a bus error on 3,5,6,7 byte accesses
** Return 0 if no bus error, else 1
*/
static int
multibyte_rd(addr, t, tu, size)
    volatile unsigned long *addr, *t, *tu;
    int size;
{
    unsigned long bits_3 = (unsigned long)addr & 0x7;
    unsigned long bits_2 = (unsigned long)addr & 0x3;

    hkeepflags &= ~H_BUSERR;	       /* clear the flag      */
    switch(size) {
    case BW_56BITS:
	if (bits_3 == 7)
	    LDR_ACCESS(addr, t, tu);   /* read 7 bytes	      */
	else if (bits_3 == 1)
	    LDL_ACCESS(addr, t, tu);   /* read 7 bytes 	      */
	break;
    case BW_48BITS:
	if (bits_3 == 6)
	    LDR_ACCESS(addr, t, tu);   /* read 6 bytes	      */
	else if (bits_3 == 2)
	    LDL_ACCESS(addr, t, tu);   /* read 6 bytes	      */
	break;
    case BW_40BITS:
	if (bits_3 == 5)
	    LDR_ACCESS(addr, t, tu);   /* read 5 bytes	      */
	else if (bits_3 == 3)
	    LDL_ACCESS(addr, t, tu);   /* read 5 bytes	      */
	break;
    case BW_24BITS:
	if (bits_2 == 2)
	    LWR_ACCESS(addr, t);       /* read 3 bytes	      */
	else if (bits_2 == 1)
	    LWL_ACCESS(addr, t);       /* read 3 bytes        */
	break;
    default:
	return(0);
    }

    if (hkeepflags & H_BUSERR)         /* check the flag      */
	return(1);
    else
	return(0);
}

/*
** multibyte_wr: Check for a bus error on 3,5,6,7 byte write
** Return 0 if no bus error, else 2
*/
static int
multibyte_wr(addr, t, tu, size)
    volatile unsigned long *addr, t, tu;
    int size;
{
    unsigned long bits_3 = (unsigned long)addr & 0x7;
    unsigned long bits_2 = (unsigned long)addr & 0x3;

    hkeepflags &= ~H_BUSERR;	       /* clear the flag      */
    switch(size) {
    case BW_56BITS:
	if (bits_3 == 7)
	    SDR_ACCESS(addr, t, tu);   /* write 7 bytes	      */
	else if (bits_3 == 1)
	    SDL_ACCESS(addr, t, tu);   /* write 7 bytes       */
	break;
    case BW_48BITS:
	if (bits_3 == 6)
	    SDR_ACCESS(addr, t, tu);   /* write 6 bytes	      */
	else if (bits_3 == 2)
	    SDL_ACCESS(addr, t, tu);   /* write 6 bytes	      */
	break;
    case BW_40BITS:
	if (bits_3 == 5)
	    SDR_ACCESS(addr, t, tu);   /* write 5 bytes	      */
	else if (bits_3 == 3)
	    SDL_ACCESS(addr, t, tu);   /* write 5 bytes	      */
	break;
    case BW_24BITS:
	if (bits_2 == 2)
	    SWR_ACCESS(addr, t);       /* write 3 bytes	      */
	else if (bits_2 == 1)
	    SWL_ACCESS(addr, t);       /* write 3 bytes        */
	break;
    default:
	return(0);
    }

    FLUSH_BUFFERS;
    if (hkeepflags & H_BUSERR)         /* check the flag      */
	return(2);
    else
	return(0);
}

/*
** dlword_rd : Check for a bus error on a longlong read
** Return 0 if no bus error, else 1
*/
static int
dlword_rd(addr, temp, temp_u)
    volatile unsigned long *addr, *temp, *temp_u;
{
    hkeepflags &= ~H_BUSERR;           /* initialize the flag */
    LD_ACCESS(addr, temp, temp_u);     /* read the address    */
    if (hkeepflags & H_BUSERR)         /* check the flag      */
	return(1);
    else
	return(0);
}

/*
** dlword_wr : Check for a bus error on a longlong write
** Return 0 if no bus error, else 2
*/
static int
dlword_wr(addr, temp, temp_u)
    volatile unsigned long *addr, temp, temp_u;
{
    hkeepflags &= ~H_BUSERR;           /* initialize the flag */
    SD_ACCESS(addr, temp, temp_u);     /* write the address   */
    FLUSH_BUFFERS;		       /* flush the access */
    if (hkeepflags & H_BUSERR)         /* check the flag      */
	return(2);
    else
	return(0);
}

/*
** lword_rd : Check for a bus error on a long word read
** Return 0 if no bus error, else 1
*/
static int
lword_rd(addr, temp)
    volatile unsigned long *addr, *temp;
{
    hkeepflags &= ~H_BUSERR;           /* initialize the flag */
    LW_ACCESS(addr, temp);
    if (hkeepflags & H_BUSERR)         /* check the flag      */
	return(1);
    else
	return(0);
}

/*
** lword_wr : Check for a bus error on a long word write
** Return 0 if no bus error, else 2
*/
static int
lword_wr(addr, temp)
    volatile unsigned long *addr, temp;
{
    hkeepflags &= ~H_BUSERR;           /* initialize the flag */
    *addr = temp;                      /* write the address   */
    FLUSH_BUFFERS;		       /* flush buffers       */
    if (hkeepflags & H_BUSERR)         /* check the flag      */
	return(2);
    else
	return(0);
}

/*
** word_rd : Check for a bus error on a short word read
** Return 0 if no bus error, else 1
*/
static int
word_rd(addr, temp)
    volatile unsigned short *addr, *temp;
{
    hkeepflags &= ~H_BUSERR;           /* initialize the flag */
    LH_ACCESS(addr, temp);
    if (hkeepflags & H_BUSERR)         /* check the flag      */
	return(1);
    else
	return(0);
}

/*
** word_wr : Check for a bus error on a short word write
** Return 0 if no bus error, else 2
*/
static int
word_wr(addr, temp)
    volatile unsigned short *addr, temp;
{
    hkeepflags &= ~H_BUSERR;           /* initialize the flag */
    *addr = temp;                      /* write the address   */
    FLUSH_BUFFERS;		       /* flush buffers       */
    if (hkeepflags & H_BUSERR)         /* check the flag      */
	return(2);
    else
	return(0);
}

/*
** byte_rd : Check for a bus error on a byte read
** Return 0 if no bus error, else 1
*/
static int
byte_rd(addr, temp)
    volatile unsigned char *addr, *temp;
{
    hkeepflags &= ~H_BUSERR;           /* initialize the flag */
    LB_ACCESS(addr, temp);
    if (hkeepflags & H_BUSERR)         /* check the flag      */
	return(1);
    else
	return(0);
}

/*
** byte_wr : Check for a bus error on a byte write
** Return 0 if no bus error, else 2
*/
static int
byte_wr(addr, temp)
    volatile unsigned char *addr, temp;
{
    hkeepflags &= ~H_BUSERR;           /* initialize the flag */
    *addr = temp;                      /* write the address   */
    FLUSH_BUFFERS;		       /* flush buffers       */
    if (hkeepflags & H_BUSERR)         /* check the flag      */
	return(2);
    else
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
    unsigned long temp, temp_upper;
    SIG_PF savfcn;

    /*
     * Capture bus error signals
     */
    savfcn = signal(SIGBUS,SIG_IGN);

    if(!readonly) {

	switch(size) {
	case BW_64BITS: /* dlword access */
	    retval += dlword_wr(address, temp, temp_upper);
	    break;
	case BW_56BITS:	/* 7 byte access */
	case BW_48BITS:	/* 6 byte access */
	case BW_40BITS:	/* 5 byte access */
	    retval += multibyte_wr(address, temp, temp_upper, size);
	    break;
	case BW_32BITS:	/* lword access */
	    retval += lword_wr((unsigned long *)address, temp);
	    break;
	case BW_24BITS:	/* 3 byte access */
	    retval += multibyte_wr(address, temp, temp_upper, size);
	    break;
	case BW_16BITS:	/* word access */
	    retval += word_wr((unsigned short *)address, (unsigned short)temp);
	    break;
	default:   	/* byte access */
	    retval += byte_wr((unsigned char *)address, (unsigned char)temp);
	    break;
	}
    }

    switch(size) {
    case BW_64BITS:   	/* dlword access */
	retval += dlword_rd(address, &temp, &temp_upper);
	break;
    case BW_56BITS:	/* 7 byte access */
    case BW_48BITS:	/* 6 byte access */
    case BW_40BITS:	/* 5 byte access */
	retval += multibyte_rd(address, &temp, &temp_upper, size);
	break;
    case BW_32BITS:   	/* lword access */
	retval += lword_rd((unsigned long *)address, &temp);
	break;
    case BW_24BITS:	/* 3 byte access */
	retval += multibyte_rd(address, &temp, &temp_upper, size);
	break;
    case BW_16BITS:   	/* word access */
	retval += word_rd((unsigned short *)address, (unsigned short *)&temp);
	break;
    default:  		/* byte access */
	retval += byte_rd((unsigned char *)address, (unsigned char *)&temp);
	break;
    }

    signal(SIGBUS,savfcn);
    return(retval);
}

/* End of Module */
