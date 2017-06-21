/* $Id: tring_setup.c,v 3.4.54.2 1996/05/17 11:24:03 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/if/tring_setup.c,v $
 *------------------------------------------------------------------
 * Token Ring setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tring_setup.c,v $
 * Revision 3.4.54.2  1996/05/17  11:24:03  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.42.2  1996/05/02  22:04:50  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.42.1  1996/04/03  14:39:28  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.54.1  1996/03/18  20:15:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.2  1996/03/07  09:44:52  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.22.1  1996/02/20  14:24:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/11/28  03:58:32  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.3  1995/11/17  09:31:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:39:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:53:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:55:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "ttysrv.h"
#include "../ui/setup.h"
#include "packet.h"
#include "../if/tring.h"
#include "../if/rif.h"

void tring_setup_int (setup_struct *sp)
{
    hwidbtype *hwidb = sp->idb->hwptr;
    char *c, buffer[MAXBUF];
    int speed, default_speed;
    boolean parsed;

    /*
     * Ask for the ring speed if this is an interface that
     * supports the 'ring-speed' command:
     */
    if (!(hwidb->status & IDB_TR) || (stdio->statbits & CONTROLC)) {
	return;
    }

    default_speed = hwidb->tr_ring_speed;
    do {
	printf("\n  Tokenring ring speed (4 or 16) ? [%u]: ", default_speed);
	if (!rdtty(buffer, MAXBUF))
	    return;
	c = deblank(buffer);
	/*
	 * If the user just typed RETURN, and we had a
	 * ring-speed configured previously, use it:
	 */
	if (null(c)) {
	    speed = default_speed;
       } else {
           speed = parse_unsigned(buffer, &parsed);
       }
    } while ((speed != 4) && (speed != 16) && !(stdio->statbits & CONTROLC));

    setup_printf(sp, "\nring-speed %d", speed);
}

