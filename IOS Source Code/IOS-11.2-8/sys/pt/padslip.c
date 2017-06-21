/* $Id: padslip.c,v 3.2.62.1 1996/05/01 01:59:42 ravindra Exp $
 * $Source: /release/112/cvs/Xsys/pt/padslip.c,v $
 *------------------------------------------------------------------
 * padslip.c  PAD --> SLIP/PPP protocol
 *
 * May 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: padslip.c,v $
 * Revision 3.2.62.1  1996/05/01  01:59:42  ravindra
 * CSCdi49877:  deleting translate with x25 pvc results in corruption crash
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:59:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:22:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/16  19:28:41  eschaffe
 * CSCdi39675:  crash in pt_pad_check_lci
 *
 * Revision 2.1  1995/06/07  22:34:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../lat/mtcsys.h"
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "name.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "subsys.h"
#include "registry.h"
#include "../pt/pt_registry.h"
#include "ttysrv.h"
#include "connect.h"
#include "translate.h"
#include "vtyasync.h"
#include "pt_slip_ppp.h"
#include "../x25/x25.h"
#include "../x25/x25_address.h"
#include "../x25/x25_switch.h"
#include "../x25/pad.h"
#include "../tcp/telnet.h"
#include "access.h"
#include "translate.h"
#include "logger.h"
#include "login.h"
#include "../os/async.h"
#include "pt_pad_common.h"
#include "../h/auth_registry.regh"
#include "stacks.h"


/*
 * PAD-SLIP Protocol Translator subsystem header
 */

#define PT_PADSLIP_MAJVERSION 1
#define PT_PADSLIP_MINVERSION 0
#define PT_PADSLIP_EDITVERSION 1

SUBSYS_HEADER(pt_padslip_ppp,
              PT_PADSLIP_MAJVERSION, PT_PADSLIP_MINVERSION, 
	      PT_PADSLIP_EDITVERSION, pt_padslip_ppp_init, 
	      SUBSYS_CLASS_PROTOCOL,
              "seq: pt_pad, pt_tcp, pt_slip_ppp",
              "req: pt_pad, pt_tcp, pt_slip_ppp");

/*
 * pt_padslip_ppp_init
 *
 * Subsystem initialization code for pad -> slip/ppp
 */

void pt_padslip_ppp_init (subsystype *subsys)
{
    reg_add_pt_x25input(TRANS_SLIP_PPP, x25_transto_slip, "x25_transto_slip");
}

/*
 * padtoslip_daemon
 * process to manage one incoming pad connection to outgoing slip/ppp
 */
static forktype padtoslip_daemon (translatetype *trans)
{
    tt_soc *vty = stdio;
    conntype *padconn = vty->conntop;
    boolean pvc = trans->in.x25.x25flags & TRANSF_X25_PVC;

    if (translate_debug)
        buginf("\npad-async%t: fork started", vty->ttynum);

    pt_pad_setup(trans, vty);
    while(TRUE) {
        /*
         * Set up an SVC or PVC connection
         * Set the X.3 options on the remote side
         * Start managing the vtyasync connection.
         */

    	if (!pt_pad_check_lci(trans, vty)) {
    	    if (pvc) {
    	    	if (translate_debug)
    	    	    buginf("\npadlat%t:conntop:%x",vty->ttynum,vty->conntop);
    	    pt_pad_clear_connection(trans,vty);
    	    }
    	}
        startoutput(vty);
        vty->statbits |= CONNECTED;
        pt_pad_setup_x3_options(trans, vty);
        /*
         * We now have a PAD connection going. Setup and run the vty-async
         * connection.
         */
        pt_run_vtyasync(trans, vty);

        /*
         * connection has died. Set up a PVC connection again.  Otherwise,
         * clear up SVC connection.
         */
        if (pvc) {
            if ((vty->statbits & CARDROP) || !pad_alive(padconn)) {
                if (translate_debug)
                    buginf("\npad-async%t: PAD PVC %d went away!", vty->ttynum,                           trans->in.x25.cudprefix);
                break;
            }
        } else
            break;             /* Its an SVC, close the session */
    }                          /* try to set up the PVC connection again */
    pt_close_vtyasync(trans, vty);
}

/*
 * x25_transto_slip
 * Pass X25 input to slip over IP output
 * Returns PID of translation process or -1 if failure
 * Called by SERVICE_TRANS_X25INPUT, index is TRANS_SLIP_PPP
 */
int x25_transto_slip (translatetype *trans, tt_soc *tty)
{
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    if (reg_invoke_kerberos_is_running())
	stacksize = (ulong) EXEC_STACK_SIZE;
    return(cfork((forkproc *)padtoslip_daemon, (long)trans, stacksize,
		 "PAD to ASYNC", tty->ttynum));
}
