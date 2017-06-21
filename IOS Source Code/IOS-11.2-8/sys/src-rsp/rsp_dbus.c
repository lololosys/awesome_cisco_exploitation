/* $Id: rsp_dbus.c,v 3.3.20.3 1996/08/28 13:13:39 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_dbus.c,v $
 *------------------------------------------------------------------
 * dbus for RSP
 *
 * March 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_dbus.c,v $
 * Revision 3.3.20.3  1996/08/28  13:13:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.20.2  1996/08/07  18:02:01  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/05/02  17:14:21  rbabu
 * CSCdi44138:  rsp becomes unresponsive when microcode is reloaded from
 * rom
 * Branch: California_branch
 *              Don't allow IPs in RSP to load ucode from ROM.
 *
 * Revision 3.3  1996/02/01  06:09:49  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  18:48:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/11  07:10:30  mcieslak
 * Reload the microcode for a slot that has failed and allow
 * for IPs that are disabled to be removed cleanly.
 * CSCdi41907:  complex restart / EOIR of disabled cards broken
 *
 * Revision 2.2  1995/07/17  08:21:55  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.1  1995/06/07  22:57:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/dbus.h"
#include "../hes/ucode.h"
#include "../hes/if_hes_common.h"
#include "rsp_if.h"

/*
 * platform-specific code to handle error in dbus status register
 *
 * A DBUSINTERR has been detected.
 *
 * Issue a hard dbus reset to the card.
 * Remove idbs so that they cannot be seen by any process
 * running the idb queues before this card is fully enabled
 * and initialized.
 * Mark the slot as disabled.
 *
 * If this card hasn't crashed recently, reload it and
 * request a complex restart.  Set the crash timer to
 * minimize number of consecutive restarts of the same card.
 * 
 * If the timer is already running, the card has crashed
 * recently.  An attempt will be made to bring it up after 
 * a sufficient time interval has elapsed.
 *
 */
void
dbus_slot_error (uint slot)
{
    slottype* sp;
    
    sp = &slots[slot];
    
    /*
     * Reset the card, remove its idbs and mark it disabled.
     */
    reset_ip(sp);
    if (!TIMER_RUNNING(sp->crash_time)) {
	GET_TIMESTAMP(sp->crash_time);
	/*
	 * Download new ucode to the offending board and set
	 * flag to reset cbus complex.
	 */
	dbus_load_ucode(slot);
	platform_req_complex_restart(NULL);
    } else {
	/* Do nothing more */
    }
    return;
}

/*
 * handle the situation when downloading bundled microcode fails.
 * no action required, since the ROM ucode isn't appropriate for RSP.
 */
boolean
rom_ucode_fallback (uint pslot)
{
    errmsg(&msgsym(NOSTART, RSP), slots[pslot].card_name, pslot);
    return FALSE;
}

/*
 * dbus_no_rom_ucode
 *
 * Return TRUE for all the ctrlr_type as they doesn't have operational 
 * code in ROM.
 */

inline boolean dbus_no_rom_ucode (uint ctrlr_type)
{
	return TRUE;
}
