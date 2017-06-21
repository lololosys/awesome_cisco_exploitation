/* $Id: unix_monitor.c,v 3.2.62.1 1996/03/18 22:30:08 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/unix/unix_monitor.c,v $
 *------------------------------------------------------------------
 * unix_monitor.c - Unix Rom Monitor EMT call emulation
 *
 * December 1991, Kirk Lougheed
 * May 1995, Scott Mackie
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_monitor.c,v $
 * Revision 3.2.62.1  1996/03/18  22:30:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/09  05:14:35  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.2  1996/03/07  10:59:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:27:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:55:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:42:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:12:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "logger.h"
#include "ttysrv.h"
#include "../../boot/cpu.h"
#include "../../boot/emt.h"
#include "../dev/monitor1.h"
#include "../../boot/caps.h"
#include "unix.h"

/*
 * Local Storage
 */
ulong hclock = 0;


/*
 * emt_call
 * ROM monitor emulation trap support
 * Some calls have been removed and may need to be replaced later.
 */

ulong emt_call (int traptype, ...)
{
    va_list ap;
    int temp;

    va_start(ap, traptype);
    switch (traptype) {
    case EMT_PUTCHAR:
	return(0L);
	
    case EMT_TICKS:
	return(0L);		/* not in shared memory */
	
    case EMT_VERSION:
	return(SYS_MAJORVERSION*256+SYS_MINORVERSION);
	
    case EMT_GETCHAR:
	return(serial_getc(console));
	
    case EMT_GETMEMSIZE:
	temp = 0x01000000; 		/* 16 megabytes of memory */
	if (unix_brk(temp))
	    unix_exit(0);
	return(temp);
	
    case EMT_GETCONFIG:
	return(-1L);	/* for now */
	
    case EMT_PROCTYPE:
	return(CPU_UNIX);
	
    case EMT_RELOAD:
	unix_exit(0);
	
    case EMT_BOOTIPADDR:
	return(0L);
	
    case EMT_VSTRING:
	return((long) "\r\nBootstrap program is "OS_STRING"\n");
	
    case EMT_CLOCKADDR:
	return((long)&hclock);	/* upper levels don't use this on UNIX */
	
    case EMT_TOUCHR:
	return(FALSE);		/* can't do this on UNIX */
	
    case EMT_LASTERROR:
	return(0L);
	
    case EMT_ROMADDR:
	return(0L);
	
    case EMT_HCLOCKADDR:
	return((long)&hclock);
	
    case EMT_TOUCHW:
	return(FALSE);		/* can't do this on UNIX */
	
    case EMT_GETCAP: {
	static struct capabilities cap;
	
	/*
	 * Return address of capability list:
	 */
	return((ulong) &cap);
    }
	
    case EMT_SETCAP:
	return(-1L);
	
    case EMT_GET_NVRAM_SIZE:
	return(0x8000);
	
    case EMT_PROCNAME:
	return((long)"UNIX/SPARC");
	
    case EMT_GET_BOOTINFO:
    case EMT_BOOT_CONFREG:
    case EMT_VSTRING_SYS:
    case EMT_VSTRING_SYS_SET:
    case EMT_VSTRING_SYS_ROM:
    case EMT_VSTRING_SYS_ROM_SET:
	return(-1L);		/* the "not implemented" return */
	
    default:
	(*kernel_buginf)("\nBootstrap Emulator called with code %d", traptype);
	return(-1L);
    }
    va_end(ap);
    return(0L);
}

