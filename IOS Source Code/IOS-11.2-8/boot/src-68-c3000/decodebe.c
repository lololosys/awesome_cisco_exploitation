/* $Id: decodebe.c,v 3.1 1995/11/09 09:09:08 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c3000/decodebe.c,v $
 *------------------------------------------------------------------
 * decodebe.c - decode a MC68020 bus error
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: decodebe.c,v $
 * Revision 3.1  1995/11/09  09:09:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:15:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1993/12/21 06:25:43  enf
 * Cancun boot support
 *
 * Revision 1.1  1993/09/24  16:33:22  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.2  1993/03/26  08:39:50  mdb
 * cleanup gcc 2.x warning messages.
 *
 * Revision 1.1  1992/03/07  23:57:34  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../statreg.h"
#include "../reason.h"
#include "../extern.h"
#include "../cpu.h"

print_control(control) 
    long control;
{
    printf(" (control reg=0x%x)", control);
}

decodebe(r_sr)
    register long r_sr;		/* status register */
{
    register u_long data;
    register u_short control = *((volatile u_short *)ADRSPC_CONTROL);
#ifdef	lint
    data = r_sr;
#endif
    
    /*
     * There is a BUG in the ASIC's used for cancun.  Reading the control
     * register can cause the sytem to go into boot mode -- if device access
     * occuring at the same time.  Cancun code then doesnt read the control 
     * register.  Unfortuneatly during power up, Some bus errors occur while
     * sizing flash. and Thus the LOCTO bit is set and never reset.  If during
     * normal operation we get a Parity error then both the parity bit and
     * the local bus timeout bit are set.  So we know that we got a parity
     * error.  If we get a bus error, then the parity bit won't be set and 
     * we'll report a local bus timeout.
     */

    if (cpu_type == CPU_CANCUN) { /* cancun puts parity error before bus err */
	if (control & CONTROL_PARITY_ERROR) {
	    /*
	     * Print out the control register because it contains the parity
	     * bits for each byte (if the user is interested, the correct
	     * parity can be computed for each byte and compared):
	     */
	    puts("Parity ");
	    print_control(control);
	    return;
	}
    }

    if (control & CONTROL_LOCTO) {
	puts("Local Timeout");
	print_control(control);
	return;
    }

    if (control & CONTROL_PARITY_ERROR) {
	/*
	 * Print out the control register because it contains the parity
	 * bits for each byte (if the user is interested, the correct
	 * parity can be computed for each byte and compared):
	 */
	puts("Parity ");
	print_control(control);
	return;
    }

    puts("Unknown");
    print_control(control);
}
