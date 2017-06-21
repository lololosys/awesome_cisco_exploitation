/* $Id: nv_les.c,v 3.3.46.1 1996/03/18 20:43:11 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/les/nv_les.c,v $
 *------------------------------------------------------------------
 * nv_les.c - cpu dependent support for non-volatile configuration memory
 *
 * 12-September-1989, Chris Shaker
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nv_les.c,v $
 * Revision 3.3.46.1  1996/03/18  20:43:11  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/07  10:05:56  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/01/24  22:24:54  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/17  18:32:52  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:42:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:39:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "config.h"
#include "../os/nv.h"

boolean nv_writeflag;

/*
 * configreg_command
 * parse a number and store it as the "config register"
 */
void configreg_command (parseinfo *csb)
{
    if (csb->nvgen) {
	/* No NV generation of config register - use show hardware */
	return;
    }
    nv_setconfig(GETOBJ(int,1));
}

/*
 * nv_write_summary - Write the configuration summary
 *
 * This routine is used to write the configuration summary software
 * configuration register.
 */

void nv_write_summary (
     boolean eraseflag)
{
}

/*
 * nv_bcopy:
 * Used to write text directly to NVRAM.
 */
void nv_bcopy (void const *src, void *dst, int length)
{
    NVBCOPY(src, dst, length);
}

