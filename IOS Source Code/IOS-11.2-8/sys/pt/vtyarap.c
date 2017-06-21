/* $Id: vtyarap.c,v 3.3 1996/01/19 19:03:57 eschaffe Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/pt/vtyarap.c,v $
 *------------------------------------------------------------------
 * Translate PAD/LAT/TCP --> Virtual-ARAP
 *
 * March 1995, Eman Schaffer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtyarap.c,v $
 * Revision 3.3  1996/01/19  19:03:57  eschaffe
 * CSCdi45538:  %SYS-3-HARIKARI: Process VTY-ARAP Helper top-level routine
 * exited
 *
 * Revision 3.2  1995/11/17  19:00:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:35:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "registry.h"
#include "ttysrv.h"
#include "../h/cs_registry.regh"
#include "../pt/pt_autocommand.h"
#include "../pt/vtyasync.h"
#include "../pt/translate.h"


/*
 * pt_vtyarap_init
 */
void pt_vtyarap_init(void)
{
    reg_add_arap_start_pt_helper_process(VTY_LINE, pt_vtyarap_connection,
					 "pt_vtyarap_connection");
}

/*
 * pt_start_vtyarap
 * Start the helper process and reset PID when done.
 */
static void pt_start_vtyarap (tt_soc *vty)
{
    pt_manage_vtyasync(vty);
    vty->helper = 0;
    process_kill(THIS_PROCESS);
}

/* pt_vtyarap_connection
 * Handle translation autocommand Arap connection initiated from "Exec Arap"
 * process.
 */

void pt_vtyarap_connection (tt_soc *vty)
{
    /*
     * Set our connection to the current process id (forkx) so TCP/LAT/PAD
     * won't do any I/O on the connection and let the helper process handle
     * it.
     */
    vty->helper = cfork ((forkproc *) pt_start_vtyarap, (long) vty, 2000, 
			 "VTY-ARAP Helper", vty->ttynum);

    if (vty->helper > 0) {    /* It worked */
	if (translate_debug)
	    buginf("\ntrans:Started vty-arap helper:%d on vty:%t", vty->helper,
		   vty->ttynum); 
    }
    else {
	if (translate_debug)
	    buginf("\n Could not start vty-arap process on tty:%t", 
		   vty->ttynum);

	/* Clean up and update statics */
	if (trans_map[vty->ttynum]) {
	    trans_map[vty->ttynum]->active--;
	    trans_map[vty->ttynum]->failures++;
	    trans_map[vty->ttynum] = 0;
	    vty->statbits &= ~CONNECTED;
	}
    }
}
