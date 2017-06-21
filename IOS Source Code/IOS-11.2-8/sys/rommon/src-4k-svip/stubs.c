/* $Id: stubs.c,v 1.1.6.1 1996/07/08 22:42:58 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/stubs.c,v $
 *------------------------------------------------------------------
 * stubs.c 
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stubs.c,v $
 * Revision 1.1.6.1  1996/07/08  22:42:58  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:22:54  pgreenfi
 * Placeholder files for src-4k-svip.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

unsigned long entrypt;
char sys_dirty;

/* these are used to satisfy functions that aren't required */
/* by the SVIP but are used in the code for other IPs       */

test_watchdog()
{}
testleds()
{}
sleep()
{}
alarm()
{}
disable_watchdog()
{}
timerint()
{}
wastetime()
{}
timer_calibrate()
{}
image_launchcode()
{}   /* from rsp_boot */
flush_all_wb()
{}	/* also in svip_int but ifed out */

