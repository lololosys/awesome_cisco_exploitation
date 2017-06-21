/* $Id: decodebe.c,v 3.1 1995/11/09 09:08:28 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-ags/decodebe.c,v $
 *------------------------------------------------------------------
 * decodebe.c - decode a MC68020 bus error
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: decodebe.c,v $
 * Revision 3.1  1995/11/09  09:08:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:14:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:33:00  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.2  1993/03/26  08:39:48  mdb
 * cleanup gcc 2.x warning messages.
 *
 * Revision 1.1  1992/03/07  23:56:19  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../statreg.h"
#include "../reason.h"
#include "../extern.h"

print_control(control) 
long control;
{
    printf(" (control reg=0x%x)", control);
}

decodebe(r_sr)			/* ARGSUSED */
    register long r_sr;		/* status register */
{
    register u_short control = *((volatile u_short *)ADRSPC_CONTROL);

    if (control & CONTROL_LOCTO) {
	puts("Local Timeout");
	print_control(control);
	return;
    }
    
    if (control & CONTROL_MBTO) {
	puts("Multibus Timeout");
	print_control(control);
	return;
    }
    
    if (control & CONTROL_PARITY) {

	/*
	 * Print out the control register because it contains the parity
	 * bits for each byte (if the user is interested, the correct
	 * parity can be computed for each byte and compared):
	 */
	puts("Parity");
	print_control(control);
	return;
    }

    puts("Unknown");
    print_control(control);
}
