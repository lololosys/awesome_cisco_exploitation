/* $Id: rom_tests.c,v 3.1 1995/11/09 09:08:41 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-ags/rom_tests.c,v $
 *------------------------------------------------------------------
 * rom_tests.c -- Special ROM monitor Test menu
 *
 * November 1989, Chris Shaker
 *
 * Copyright (c) 1989-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rom_tests.c,v $
 * Revision 3.1  1995/11/09  09:08:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:15:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:33:14  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1992/03/07  23:56:31  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../types.h"
#include "confreg.h"
#include "../stdio.h"
#include "../m68vectors.h"
#include "../reason.h"
#include "../extern.h"
#include "../test.h"

void print_menu() {
    printf("ROM Monitor Test Menu (for firmware hacks only):\n");
    printf("  H - Help menu\n");
    printf("  L - Cause a local bus timeout error\n");
#ifndef	PAN
    printf("  M - Cause a Multibus timeout error\n");
#endif
    printf("  P - Cause a RAM parity error\n");
    printf("  Q - Quit, return to main menu\n");
}

void rom_monitor_tests() {
    u_short *address;
    volatile u_short data;
    int ilevel;
    
    print_menu();
    while(1) {
	printf("> ");
	flush();
	gets(bufu.buf);
	
	switch(bufu.buf[0]) {
	    
	  case 'l': case 'L':
	    /*
	     * Create a Local bus timeout error by accessing an illegal
	     * local RAM address:
	     */
	    address = (u_short *)(ADRSPC_RAM + (RAMPAGES * PAGESIZE));
	    printf("Creating a Local bus timeout error by reading from address %#x...\n", address);
	    flush();
	    data = *address;
	    break;

#ifndef	PAN	    
	  case 'm': case 'M':
	    /*
	     * Create a Multibus timeout error by accessing unused an
	     * unused Multibus address:
	     */
	    address = (u_short *)(ADRSPC_MULTI + 0xEFFFF);
	    printf("Creating a Multibus timeout error by reading from address %#x...\n", address);
	    flush();
	    data = *address;
	    break;
#endif
	    
	  case 'p': case 'P':
	    /*
	     * Generate a RAM Parity error:
	     *  o Set CSC2 to generate even parity (odd by default).
	     *  o Write data with even parity.
	     *  o Set CSC2 to generate and check odd parity.
	     *  o Read the data with even parity while checking for odd
	     *    parity to cause a parity error.
	     */
	    printf("Creating a RAM parity error at address %#x...\n",
		   INITSTACKP);
	    flush();
	    ilevel = sil(7);	/* Mask interrupts */
	    *((volatile u_short *)ADRSPC_CONTROL) |= CONTROL_EVENPAR;
	    *((volatile u_short *)INITSTACKP) = 0; /* Write even parity */
	    *((volatile u_short *)ADRSPC_CONTROL) &= ~CONTROL_EVENPAR;
	    (void)sil(ilevel);	/* Restore interrupts */
	    data = *((volatile u_short *)INITSTACKP); /* Read, odd parity */
	    break;
	    
	  case 'h': case 'H': case '?':
	    print_menu();
	    break;
	    
	  case 'q': case 'Q':
	    return;
	    break;
	}
    }
}
