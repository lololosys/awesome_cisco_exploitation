/* $Id: unix_exception.c,v 3.2.62.2 1996/08/28 13:18:51 thille Exp $
 * $Source: /release/112/cvs/Xsys/unix/unix_exception.c,v $
 *------------------------------------------------------------------
 * unix_exception.c - Unix exception handler stubs
 *
 * December 1991, Kirk Lougheed
 * May 1995, Scott Mackie
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_exception.c,v $
 * Revision 3.2.62.2  1996/08/28  13:18:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/03/18  22:29:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/09  05:14:31  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.2  1996/03/07  10:59:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:27:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:55:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:42:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  02:32:52  ahh
 * CSCdi35948:  Fix some undefined references in UNIX images
 *
 * Revision 2.1  1995/06/07  23:12:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../unix/msg_unix.c"		/* Not a typo, see logger.h */
#include <stdarg.h>
#include "ttysrv.h"
#include "config.h"
#include "../ui/debug.h"
#include "unix.h"
#include "../os/os_debug.h"
#include "../os/gdb.h"
#include "sys_registry.h"

watched_boolean *system_crashed;
char *traceback_prefix = traceback_prefix_string;
char *dumpfile;			/* Default dumpfile name */
char *sl_dumpfile;              /* Default sl_dumpfile name */


/*
 * Remote debugging support
 */

void crashdump (int seconds)
{
    int *bad;

    printf("\nCrashdump called");
    unix_setcooked();
    bad = 0;
    *bad = 0;
}

void exception_mempool_init (mempool *mem)
{
}

void exception_config (parseinfo *csb)
{
    if (!csb->nvgen)
	(*kernel_buginf)("\nexception configuration not implemented");
}

void sl_exception_config (parseinfo *csb)
{
    if (!csb->nvgen)
	(*kernel_buginf)("\nexception-slave configuration not implemented");
}

void manual_write_core (parseinfo *csb)
{
    printf("\nwrite core not implemented");
}

/*
 * abort:
 * Lets abort elegantly
 */
void
abort(void)
{
    (*kernel_errmsg)(&msgsym(SYSABORT, UNIX));
    reg_invoke_flush_logging();
    unix_exit(1);
}

void exception_jump (void fn(void))
{
}

void *exception_fault_addr (void)
{
    return(0);                  /* Address unknown */
}

boolean exception_interrupt (void)
{
    return(FALSE);              /* No interrupts in Unix */
}

void return_from_exception (void)
{
}

void exception_save_context (cpu_context_t *ctx)
{
}

void exception_restore_context (cpu_context_t *ctx)
{
}


boolean kernel_debugging = FALSE;
boolean gdb_kill_debug (sprocess *fork) {return(TRUE);}
void restore_vectors (void) {}
ulong *caller_frame (void) { return(NULL); }


/*
 * touch_device
 * Determines if an address exists by reading. Returns boolean.
 */

boolean touch_device (void *addr)
{
    return(TRUE);
}
