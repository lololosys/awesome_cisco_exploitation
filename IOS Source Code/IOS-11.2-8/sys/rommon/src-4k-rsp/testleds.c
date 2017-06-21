/* $Id: testleds.c,v 3.2.58.1 1996/03/21 23:32:38 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-rsp/testleds.c,v $
 *------------------------------------------------------------------
 * testleds.c
 *
 * Mar 1995, Steve J. Zhang
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Test of the Run Lamp for RSP
 *------------------------------------------------------------------
 * $Log: testleds.c,v $
 * Revision 3.2.58.1  1996/03/21  23:32:38  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/01/16  01:59:47  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:30  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:47:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:41:32  hampton
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
    return(0);
}

/* End of Module */
