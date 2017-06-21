/* $Id: testleds.c,v 3.2.58.1 1996/03/21 23:31:13 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/testleds.c,v $
 *------------------------------------------------------------------
 * testleds.c
 *
 * May 1993, Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Test of the Run Lamp for Sierra.
 *------------------------------------------------------------------
 * $Log: testleds.c,v $
 * Revision 3.2.58.1  1996/03/21  23:31:13  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:23:41  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:46:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"

testleds(int argc, char *argv[])
{
    long value;
    jmp_buf local_buf, *savjmp;

    if(argc != 2) {
	printf("usage: %s value\n",argv[0]);
	return(1);
    }
    if(getnum(argv[1],10,&value) == 0) return(1);
    printf("Calling errleds() with value %d\n"
	   "enter <break> to exit\n",value);
    savjmp = monjmpptr;
    if(!setjmp(local_buf)) {
	monjmpptr = &local_buf;
	errleds(value);
    } else {
	monjmpptr = savjmp;

	/*
	 * make sure runlamp is off
	 */
    }
    KSEG1_IO_ASIC->sys_ctl |= CONTROL_RUNLAMP;
    return(0);
}

/* End of Module */
