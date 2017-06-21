/* $Id: padlat.c,v 3.3.6.1 1996/05/01 01:59:39 ravindra Exp $
 * $Source: /release/112/cvs/Xsys/pt/padlat.c,v $
 *------------------------------------------------------------------
 * padlat.c  PAD <--> LAT protocol translation.
 *
 * May 1990,  Bill Westfield
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: padlat.c,v $
 * Revision 3.3.6.1  1996/05/01  01:59:39  ravindra
 * CSCdi49877:  deleting translate with x25 pvc results in corruption crash
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/06  09:31:24  billw
 * CSCdi44863:  AAA Accounting not done for one-step protocol translation
 * Also ensure tty->ustruct gets set for one-step PT sessions.
 *
 * Revision 3.2  1995/11/17  18:59:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:22:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/09  13:18:19  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:34:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../lat/mtcsys.h"
#include "interface_private.h"
#include "address.h"
#include "packet.h"
#include "ttysrv.h"
#include "connect.h"
#include "login.h"
#include "../x25/x25.h"
#include "../x25/x25_address.h"
#include "../x25/x25_switch.h"
#include "../x25/pad.h"
#include "access.h"
#include "translate.h"
#include "../os/async.h"
#include "../lat/latfnc.h"
#include "../lat/lat.h"
#include "latpad.h"
#include "pt_pad_common.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "stacks.h"

extern vectortype PADvector;


/*
 * padtolat_daemon
 * Do protocol translation from X.25 to LAT.  Load profiles appropriately.
 * Note: Some of the pt_..... helper subroutines are allowed to kill
 * the thread "death()" and might not return to the daemon process.
 */
static forktype padtolat_daemon (translatetype *trans)
{
    tt_soc *tty = stdio;
    conntype *latconn;
    boolean pvc = trans->in.x25.x25flags & TRANSF_X25_PVC;

    if (translate_debug)
	buginf("\npadlat%t: fork started", tty->ttynum);

    pt_pad_setup(trans, tty);

lagain:

    if (!pt_pad_check_lci(trans, tty)) {
    	if (pvc) {
    	    if (translate_debug)
    	    	buginf("\npadlat%t:conntop:%x",tty->ttynum,tty->conntop);
    	    pt_pad_clear_connection(trans,tty);
    	}
    }

    latconn = lat_open_connection(tty,
				  trans->out.lat.latservice,
				  trans->out.lat.latnode,
				  trans->out.lat.latport,
				  (trans->flags & TRANSF_GLOB_QUIET) ?
				  LATOPEN_QUIET : 0,
				  NULL);
    if (latconn == NULL) {
	if (translate_debug)
	    buginf("\npadlat%t: can't get lat connection", tty->ttynum);
	trans->failures++;
	if (pvc)
	    goto lagain;

	pt_pad_clear_connection(trans, tty);
    }

    pt_pad_accept_trans(trans, tty);

    latconn->acct_info = reg_invoke_aaa_acct_conn(tty->ustruct, NULL,
						  latconn);

    pt_pad_setup_x3_options(trans, tty);

    /*
     * We now have a connection going in each direction, one LAT, and one
     * PAD.  Shovel bytes both ways, and translate state changes.
     */
    tty->statbits |= CONNECTED;
    psioff(tty);
    lat_run(latconn, FALSE);
    tty->statbits &= ~CONNECTED;
    conn_close(latconn);
    if (pvc)
	goto lagain;
    trans->active--;
    death(tty);
    process_kill(THIS_PROCESS);
}

/*
 * x25_transto_lat
 * Translate X25 input to LAT output
 * Returns PID of translation process or -1 if failure
 * Called by SERVICE_TRANS_X25INPUT, index is TRANS_LAT
 */

int x25_transto_lat (translatetype *trans, tt_soc *tty)
{
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    if (reg_invoke_kerberos_is_running())
	stacksize = (ulong) EXEC_STACK_SIZE;
    return(cfork((forkproc *)padtolat_daemon, (long)trans, stacksize, 
		 "PAD to LAT", tty->ttynum));
}
