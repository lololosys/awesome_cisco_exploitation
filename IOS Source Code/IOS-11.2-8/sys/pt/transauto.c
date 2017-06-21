/* $Id: transauto.c,v 3.2.62.3 1996/05/10 18:13:59 ravindra Exp $
 * $Source: /release/112/cvs/Xsys/pt/transauto.c,v $
 *------------------------------------------------------------------
 * Translate incoming LAT/PAD/TCP connection to any outgoing protocol
 * defined in the translation Autocommand argument.
 *
 * Dec. 1994, Eman Schaffer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: transauto.c,v $
 * Revision 3.2.62.3  1996/05/10  18:13:59  ravindra
 * CSCdi56911:  spurious access in pt_terminate_helper()
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/05/01  01:59:53  ravindra
 * CSCdi49877:  deleting translate with x25 pvc results in corruption crash
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  21:45:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:41:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:01:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  19:00:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/15  18:54:43  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.2  1995/06/09  13:18:31  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:35:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "registry.h"
#include "../pt/pt_registry.h"
#include "ttysrv.h"
#include "connect.h"
#include "../pt/translate.h"
#include "../os/async.h"
#include "../pt/pt_autocommand.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"


/*
 * Local Storage and Definitions
 */

/* A list of trans pointers indexed by vty->ttynum */ 
translatetype **trans_map;


/*
 * pt_transauto_init
 *
 * Subsystem initialization code for tcp/lat/pad -> autocommand
 */

void pt_transauto_init (void)
{
    trans_map = malloc(MAXLINES * sizeof(translatetype *));
    reg_add_death(pt_terminate_helper, "pt_terminate_helper");

    /* Reg_add vty-arap functions */
    pt_vtyarap_init();
}

/*
 * pt_setup_auto_session
 * Startup an exec process and automatically execute the EXEC command
 * defined by the translate autocommand argument.
 */
void pt_setup_auto_session (translatetype *trans, tt_soc *vty)
{
    trans_map[vty->ttynum] = trans;
    vty->tty_autocommand = trans->out.autocommand.autocommand_text;
    if (!start_exec_proc(vty)) {
	/*
	 * Failed to create an auto exec process
	 * Display the approperiate buginf
	 * Clean and close the session.
	 */
	if (translate_debug) {
	    if (vty->tiptop == -1) {
		buginf("\ntrans-auto: Unable to create EXEC - no memory or "
		       "too many processes");
	    }
	    else {
		buginf("\ntrans-auto: Unable to create EXEC - line is "
		       "configured for \"no exec\"");
	    }
	}
	process_sleep_for(ONESEC);
	trans->failures++;
	death(vty);
    }
    /* It worked. */
    if (translate_debug)
        buginf("\ntrans-auto: Started Exec:%d on vty:%t", vty->tiptop,
	       vty->ttynum);
    process_kill(THIS_PROCESS);   /* Now you can stop the translation process */
}

/*
 * pt_terminate_helper
 * Used for Autocommand PT sessions and when running two processes 
 * (ARAP and VTY-Helper) on one VTY line.   
 * Reg invoked by death because the PT process itself won't be running.
 * If necessary, terminate the helper process and update translation statics.
 */
void pt_terminate_helper (tt_soc *vty)
{
    boolean pvc = FALSE;
    /*
     * Connection has died. Close PT connection and terminate the 
     * helper process. Return if the helper isn't really a helper
     * process - vty-async connections set vty->helper to their own
     * pid to make the rx_tx_handler stuff work. Just return for those.
     */
    if (vty->helper == vty->tiptop) {
	return;
    } 
    if (vty->helper) {
	if (vty->conntop) {
	    conn_close(vty->conntop);
	    vty->conntop = NULL;
	}
	process_kill(vty->helper);
	vty->helper = 0;
    }
    /* Don't forget to decrement the number of current active users
     * on this Vty line.
     */
    if (trans_map[vty->ttynum]) {
    	pvc = trans_map[vty->ttynum]->in.x25.x25flags & TRANSF_X25_PVC;
	if (trans_map[vty->ttynum]->active != 0) {
	    trans_map[vty->ttynum]->active--;
	}
    	if(pvc)
    	    free(trans_map[vty->ttynum]);
	trans_map[vty->ttynum] = 0;
	vty->statbits &= ~CONNECTED;
    }
}
