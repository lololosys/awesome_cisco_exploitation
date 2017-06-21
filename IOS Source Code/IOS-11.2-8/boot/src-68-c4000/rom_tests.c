/* $Id: rom_tests.c,v 3.1 1995/11/09 09:09:50 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c4000/rom_tests.c,v $
 *------------------------------------------------------------------
 * rom_tests.c -- Special ROM monitor Test menu
 *
 * November 1989, Chris Shaker
 *
 * Copyright (c) 1992-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: rom_tests.c,v $
 * Revision 3.1  1995/11/09  09:09:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:34:08  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.2  1992/05/06  22:33:02  clev
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
 * Revision 1.1  1992/04/15  02:11:57  clev
 * Initial checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:08  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Hacked for XX by Rob Clevenger, December 6, 1991
 */

#include "../defs.h"
#include "../stdio.h"
#include "../m68vectors.h"
#include "../reason.h"
#include "../extern.h"
#include "../test.h"

extern int mainmem_plogic_test();
extern int shmem_plogic_test();
extern int parity_test(), setcacr();
extern boolean flash_test();
extern void cflash_test();
extern void usdelay_test(), delay_test();
void print_menu() {
    printf("ROM Monitor Test Menu (for firmware hacks only):\n\
  H - This here help menu\n\
  L - Test the local (main) memory parity circuits\n\
  S - Test the shared memory parity circuits\n\
  F - Flash EPROM test\n\
  X - Xtended Flash EPROM test\n\
  C - Continuous Flash EPROM test\n\
  D - Test delay routine\n\
  U - Test usdelay routine\n\
  Q - Quit, return to main menu\n");
}

void rom_monitor_tests()
{
    print_menu();
    while(1) {
	printf("> ");
	flush();
	gets(bufu.buf);
	
	switch(bufu.buf[0]) {
	    
	case 'l': case 'L':
	    parity_test(0,0);  /* ensure longword boundary */
	    break;
	    
	case 's': case 'S':
	    parity_test(ADRSPC_SHAREDMEM,0);
	    break;
	    
	case 'f': case 'F':
	    flash_test(0);
	    break;

	case 'x': case 'X':
	    flash_test(1);
	    break;

	case 'c': case 'C':
	    cflash_test();
	    break;

	case 'u': case 'U':
	    usdelay_test();
	    break;

	case 'd': case 'D':
	    delay_test();
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

void
delay_test()
{
    ulong time1 = 2;
    ulong time2 = 50;

    printf("scope triggers every %d and %d ms.\n",time1,time2);

    while(1) {
	*(volatile ulong *)ADRSPC_CONTROL &= ~CONTROL_LED_DS10;
	delay(time1);
	*(volatile ulong *)ADRSPC_CONTROL |= CONTROL_LED_DS10;
	delay(time2);
    }
}
