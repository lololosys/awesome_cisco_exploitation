/* $Id: rom_tests.c,v 3.1 1995/11/09 09:09:22 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c3000/rom_tests.c,v $
 *------------------------------------------------------------------
 * rom_tests.c -- Special ROM monitor Test menu
 *
 * November 1989, Chris Shaker
 *
 * Copyright (c) 1992-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Hacked for Moosehead by Rob Clevenger, April 7, 1992
 */

#include "../defs.h"
#include "../stdio.h"
#include "../m68vectors.h"
#include "../reason.h"
#include "../extern.h"
#include "../test.h"

extern flash_test();
extern cflash_test();

void print_menu() {
    printf("ROM Monitor Test Menu (for firmware hacks only):\n\
  H - This here help menu\n\
  F - Flash EPROM test\n\
  X - Xtended Flash EPROM test\n\
  C - Continuous Flash EPROM test\n\
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
	    
	case 'f': case 'F':
	    flash_test(0);
	    break;

	case 'x': case 'X':
	    flash_test(1);
	    break;

	case 'c': case 'C':
	    cflash_test();
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
