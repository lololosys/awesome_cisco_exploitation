/* $Id: decodebe.c,v 3.1 1995/11/09 09:09:32 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c4000/decodebe.c,v $
 *------------------------------------------------------------------
 * decodebe.c - decode a MC68020 bus error
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1992-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: decodebe.c,v $
 * Revision 3.1  1995/11/09  09:09:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:33:49  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.7  1992/09/25  00:58:25  clev
 * Fix the byte order on a shared memory parity error report for XX.
 *
 * Revision 1.6  1992/09/05  00:28:29  clev
 * Fix the reporting of shared memory parity errors for XX.
 *
 * Revision 1.5  1992/05/14  00:17:58  clev
 * Fix the decoding of shared memory parity error information.
 *
 * Revision 1.4  1992/05/06  22:32:52  clev
 * XX cleanup stuff:
 *
 * ../asm.S - fix clear caches in the restart routine
 *
 * ../prom2.c - handle netbooting a system image with a load point different from
 *              firmware system image
 *
 * decodebe.c & printf.c - fix reporting of bus errors and parity errors
 *
 * defs_xx.h - defined BUSERROR for the XX for conditional compiles
 *
 * fldvr.c & rom_tests.c - provide abbreviated flash test for manufacturing
 *
 * init.c - fix the clobbering of bootinfo stuff
 *
 * Revision 1.3  1992/04/19  03:50:50  clev
 * Get the error messages right for shared and local memory parity errors.
 *
 * Revision 1.2  1992/04/18  00:00:33  clev
 * Added better support for shared memory parity errors and support for
 * EMT_ENABLE_DCACHE (and disable).  Also, added support for break enable
 * during boot and better decoding of bus errors.  Init was enhanced to
 * flash an error number in the run LED should the power-on confidence
 * test fail.  Init was also improved to initialize interrupt vectors up
 * to 0x200.
 *
 * Revision 1.1  1992/04/15  02:11:34  clev
 * Initial checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:08  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * February 1992, Rob Clevenger - add code to handle main memory
 * and shared memory parity errors on XX
 */

#include "../defs.h"
#include "../statreg.h"
#include "../reason.h"
#include "../extern.h"
#include "bitdef.h"

/*
** Display the bus error portion of the control register.
*/
struct bitdef ctlregbits[] = {
    1,"ram byte 0 parity error\n",
    1,"ram byte 1 parity error\n",
    1,"ram byte 2 parity error\n",
    1,"ram byte 3 parity error\n",
    0,"",  /* actually flash memory present bit */
    0,"non-local memory transaction timeout\n",
    0,"system overtemp\n",
    0,"local memory transaction timeout\n",
};

print_control(control)
ulong control;
{
    printf(" control register: %#.8x\n%#.8b", control,
	   ctlregbits, (control >> 24) & 0xff);
}

decodebe(r_sr)
    register long r_sr;		/* status register */
{
    register u_long data;
    register u_long control = *((volatile u_long *)ADRSPC_CONTROL);
#ifdef	lint
    data = r_sr;
#endif

    puts("Bus Error");
    print_control(control);
    puts("Bus");
}

/*
** Display the shared memory parity error information
** (the upper 8 bits of the shared memory control register)
*/
struct bitdef shmempereg[] = {  /* bit defs for sh mem parity error */
    1,"slot 3",
    1,"slot 2",
    1,"slot 1",
    1,"MPU",
    1,"0 ",
    1,"1 ",
    1,"2 ",
    1,"3 ",
};

decodeshmempe()
{
    volatile ulong *ctlreg = (volatile ulong *)ADRSPC_SHMEM_CONTROL;
    ulong shmemctl = *ctlreg;
    uchar parbits = shmemctl >> 24;

    if(parbits) {  /* parity error reported */
	printf("\nShared Memory Parity Error\n\
shared memory control register = %#x\n\
error caused by %#.8b access in byte(s) %#.8b\n",
	       shmemctl,
	       shmempereg,parbits & 0xf0,
	       shmempereg,parbits & 0x0f);
	shmemctl &= ~SHMEM_PARITY_MASK;  /* wipe out these bits */
	*ctlreg = shmemctl & ~SHMEM_PERR_CLEAR;
	*ctlreg = shmemctl | SHMEM_PERR_CLEAR;
    } else {
	puts(
"\nLevel 6 interrupt occured with no shared memory parity error reported\n"
	 );
    }
}
