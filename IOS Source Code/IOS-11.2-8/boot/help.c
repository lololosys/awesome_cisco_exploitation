/* $Id: help.c,v 3.2.58.1 1996/05/10 23:17:02 gstovall Exp $
 * $Source: /release/112/cvs/Xboot/help.c,v $
 *------------------------------------------------------------------
 * help.c -- give help to the recipient
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: help.c,v $
 * Revision 3.2.58.1  1996/05/10  23:17:02  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * The beast still exists, finish killing off the CS500, and while I am in
 * there, nuke the even crustier CSC1.
 *
 * Revision 3.2  1995/11/17  07:41:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"

help() {

    register char processor_version;

#if defined(MC68040)
        puts("$ state      Toggle cache state (? for help)\n");
#endif

#if defined(MC68020)
# ifdef CSC2
    processor_version = sys_status_reg->version;
    if (processor_version == VER_CSC4)
        puts("$ state      Toggle cache state (? for help)\n");
    else
        puts("$            Toggle cache state\n");
# else
        puts("$            Toggle cache state\n");
# endif
#endif

    puts("B [filename] [TFTP Server IP address | TFTP Server Name]\n");
    puts("             Load and execute system image from ROM or from TFTP server\n");
    puts("C [address]  Continue execution [optional address]\n");
    puts("D /S M L V   Deposit value V of size S into location L with modifier M\n");
    puts("E /S M L     Examine location L with size S with modifier M\n");
    puts("G [address]  Begin execution\n");
    puts("H            Help for commands\n");
    puts("I            Initialize\n");
    puts("K            Stack trace\n");
    puts("L [filename] [TFTP Server IP address | TFTP Server Name]\n");
    puts("             Load system image from ROM or from TFTP server, but do not\n");
    puts("             begin execution\n");
    puts("O            Show configuration register option settings\n");
    puts("P            Set the break point\n");
    puts("S            Single step next instruction\n");
    puts("T function   Test device (? for help)\n\n");
    puts("Deposit and Examine sizes may be B (byte), L (long) or S (short).\n");
    puts("Modifiers may be R (register) or S (byte swap).\n");
    puts("Register names are: D0-D7, A0-A6, SS, US, SR, and PC\n");
}
