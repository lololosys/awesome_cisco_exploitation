/* $Id: rsp_tmpstubs.c,v 3.3.20.1 1996/03/18 22:02:46 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-rsp/rsp_tmpstubs.c,v $
 *------------------------------------------------------------------
 * rsp_tmpstubs.c
 *
 * March 1994, Chris Shaker
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_tmpstubs.c,v $
 * Revision 3.3.20.1  1996/03/18  22:02:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:45:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  18:01:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/01  06:10:04  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  18:49:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:58:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "config.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../os/nv.h"


#define STUB_COMPLAIN() \
    SAVE_CALLER();					\
    do {						\
	static uchar complained = 0;			\
							\
	if (!system_loading && complained == 0) {	\
	    complained = 1;				\
	    buginf("\n%%RSP_STUB: " __FUNCTION__	\
		   " called from %x", caller());	\
	}						\
    } while (0)



/*
 * For the purpose of Tuesday, we've not yet fully eliminated
 * references to the global "cbus".  We therefore declare it here.
 * Not only do we declare it, but we make sure it's pointing to
 * something safe, in case we've eliminated the code that initialises
 * it before we've eliminated all the code that de-references it.
 */
mcitype rsp_dummy_cbus = {
    0,					/* next ptr */
    (mciregtype*) 0xcdcdcdcd,		/* use invalid regptr */
};
const mcitype* cbus = &rsp_dummy_cbus;


/* 
   XXX - this guy has been moved to ags_rp1_if.c, although it's not
   clear why, looks pretty portable to me.  Something for our bridging
   gurus to sort out.... come on down Bob and Marty.  I've added a
   stub here because trip.c makes direct calls to this routine - dB.
*/

void cbus_mci_rxtypes_update (hwidbtype *idb)
{
    STUB_COMPLAIN();
    return;
}

queuetype cdbpreQ;

#include "../if/if_controller.h"
#include "../hes/hes_controller.h"

void default_love_note_handler (cdbtype *cdb, ushort status)
{
    static uchar complained = 0;

    if (!system_loading && complained == 0) {
	complained = 1;
	buginf("\n%%RSP_STUB: default_love_note_handler, controller %s",
	       cdb->cdb_namestring);
    }
    return;
}
