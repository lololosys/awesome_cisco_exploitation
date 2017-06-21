/* $Id: unix_nv.c,v 3.2.62.2 1996/06/28 23:31:26 hampton Exp $
 * $Source: /release/112/cvs/Xsys/unix/unix_nv.c,v $
 *------------------------------------------------------------------
 * unix_nv.c - Routines for Unix NVRAM emulation
 *
 * March 1995, Scott Mackie (moved from unix_init.c)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: unix_nv.c,v $
 * Revision 3.2.62.2  1996/06/28  23:31:26  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  22:30:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:59:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:27:57  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:55:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:42:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:12:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <ciscolib.h>
#include "logger.h"
#include "ttysrv.h"
#include "../../boot/emt.h"
#include "../dev/monitor1.h"
#include "../../boot/caps.h"
#include "config.h"
#include "../ui/debug.h"
#include "unix.h"
#include "../os/nv.h"
#include "../os/os_debug.h"
#include "../os/gdb.h"
#include "../os/nv.h"

boolean nv_writeflag;

void nv_setconfig (ushort number) { return;}
void nv_write_summary (boolean eraseflag) { return; }

/*
 * nv_init
 * Claim we have NVRAM.  In this case, the UNIX filesystem.
 */

void nv_init (void)
{
    extern long configbufsize;

    nvsize = mon_get_nvram_size();
    configbufsize = nvsize;		/* XXX Kludge */
}

/*
 * nv_badptr
 * Sanity check the nvtype pointer.  Always good for us.
 */

boolean nv_badptr (nvtype *ptr)
{
    return(FALSE);
}

/*
 * nv_writeenable
 */
void nv_writeenable (nvtype *ptr)
{ 
    nv_writeflag = TRUE;
}

/*
 * nv_writedisable
 */
void nv_writedisable (nvtype *ptr) 
{ 
    nv_writeflag = FALSE;
}


/*
 * nv_getptr
 * Read a UNIX file into memory and construct a pseudo-NVRAM structure,
 * complete with magic numbers and checksum.
 */

nvtype *nv_getptr (void)
{
    nvtype *nvptr;

    nvptr = malloc(nvsize);
    if (!nvptr)
	return(NULL);
    nvptr->textbase = ((uchar *) nvptr) + sizeof(nvtype);
    nvptr->textptr = nvptr->textbase;
    nvptr->textsize = 0;
    if (unix_read_file("test-confg", nvptr->textbase, &nvptr->textsize,
			nvsize-sizeof(nvtype)))
	nvptr->magic = NVMAGIC;	/* set magic number if we found something */
    nvptr->version = NVVERSION;
    nvptr->checksum = 0;
    nvptr->checksum = ipcrc((ushort *)nvptr, nvsize);
    nv_check_getptr(nvptr);
    return(nvptr);
}

/*
 * nv_done
 * Write NVRAM buffer to "memory"
 */

void nv_done (nvtype *ptr)
{
    char *outfile = "test-confg";

    nv_check_done(ptr);
    if (nv_writeflag) {
	if (ptr->textsize) {
	    if (!unix_write_file(outfile, ptr->textbase, ptr->textsize)) 
		printf("%% failed to write %s", outfile);
	}
	else {  /* "write erase" */
	    unix_remove_file(outfile);
	} 
    }
    free(ptr);
}

/*
 * nv_bcopy
 * NVRAM text copy
 */
void
nv_bcopy(void const *src, void *dst, int length)
{
    bcopy(src, dst, length);
}

