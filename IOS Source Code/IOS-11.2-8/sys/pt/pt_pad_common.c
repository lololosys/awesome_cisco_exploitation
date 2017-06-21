/* $Id: pt_pad_common.c,v 3.2.62.1 1996/05/01 01:59:46 ravindra Exp $
 * $Source: /release/112/cvs/Xsys/pt/pt_pad_common.c,v $
 *------------------------------------------------------------------
 * pt_pad_common.c  Protocol translation common subfunctions shared by 
 * pad to lat/tcp/slip-ppp files.
 *
 * Aug. 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pt_pad_common.c,v $
 * Revision 3.2.62.1  1996/05/01  01:59:46  ravindra
 * CSCdi49877:  deleting translate with x25 pvc results in corruption crash
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:59:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/16  19:28:42  eschaffe
 * CSCdi39675:  crash in pt_pad_check_lci
 *
 * Revision 2.2  1995/06/09  13:18:23  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:35:11  hampton
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
#include "padtcp.h"
#include "pt_pad_common.h"

extern vectortype PADvector;


/*
 * pt_pad_setup
 *
 * When the printer option is selected in the translation command,
 * we have to be QUIET till we finish the TCP connection process.
 * This is done by not setting the TCP vectors untill the TCP connection
 * is completed.
 */
void pt_pad_setup (translatetype *trans, tt_soc *tty)
{
    if (!(trans->in.x25.x25flags & TRANSF_X25_PRINTER)) {
	/* This will be done later if printer option is enabled. */

	tty->ovector = &PADvector;
	tty->ivector = &PADvector;
    }

    trans->active++;
    if (trans->active > trans->maxusers)
	trans->maxusers = trans->active;

    if (!(trans->in.x25.x25flags & TRANSF_X25_PRINTER)) {
        /*
	 * This will be done later if printer option is enabled.
	 */
	pt_do_login(trans, tty);
    }
}

/*
 * pt_pad_check_lci
 *
 */
boolean pt_pad_check_lci (translatetype *trans, tt_soc *tty)
{
    pcbtype *pcb;
    lcitype *lci = NULL;
    boolean pvc = FALSE;
    boolean printer = FALSE;

    if (trans) {
    	pvc = trans->in.x25.x25flags & TRANSF_X25_PVC;
    	printer = trans->in.x25.x25flags & TRANSF_X25_PRINTER;
    }

    if (tty->conntop) {
	pcb = (pcbtype *) tty->conntop->tcb;
	if (pcb)
	    lci = pcb->pcb_lci;
	if(pvc && (lci == NULL)) {
    	    if (translate_debug)
    	    	buginf("\n%s%s%t: PVC LCI has been removed",
		   	reg_invoke_pt_translate_string(INPROT(trans->type),
    	    	    	    	    	    	       INPROT(trans->type)),
		   	reg_invoke_pt_translate_string(OUTPROT(trans->type),
						       OUTPROT(trans->type)),
		   	tty->ttynum);
           return (FALSE);
	}
    } else {
	if (translate_debug)
	    buginf("\n%s%s%t: PAD connection is gone",
		   reg_invoke_pt_translate_string(INPROT(trans->type),
						  INPROT(trans->type)),
		   reg_invoke_pt_translate_string(OUTPROT(trans->type),
						  OUTPROT(trans->type)),
		   tty->ttynum);
	return (FALSE);
    }

    /*
     * If there is an X25 PVC driving us, keep trying the connection again
     */
    if (pvc && lci) {
	x25_sendcause(lci->lci_idb->hwptr, lci, X25_RESET,
		      X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, NULL);
	process_sleep_for(5*ONESEC);
	trans->totaluses++;
	if (translate_debug)
	    buginf("\n%s%s%t: restarting connection for PVC",
		   reg_invoke_pt_translate_string(INPROT(trans->type),
						  INPROT(trans->type)),
		   reg_invoke_pt_translate_string(OUTPROT(trans->type),
						  OUTPROT(trans->type)),
		   tty->ttynum);
    }
    else {
	/* Its an SVC.  Check if printer option is selected and lock the LCI, 
	 * just in case the X.25's CALL CONFIRM timer may expire before 
	 * the TCP connection succeeds or fails.
	 */
	if (lci && printer) {
	    /* 
	     * Printer option does not make sense for SLIP/PPP translation.
	     */
	    if (OUTPROT(trans->type) != TRANS_SLIP_PPP) {
		mem_lock(lci);
	    }
	}
    }
    return (TRUE);
}

/*
 * pt_pad_accept_trans
 * Confirm the CALL request after the TCP session is establised.
 */
void pt_pad_accept_trans (translatetype *trans, tt_soc *tty)
{

    conntype *padconn = tty->conntop;
    pcbtype *pcb = (pcbtype *) padconn->tcb;
    lcitype *lci = pcb->pcb_lci;
    boolean printer = trans->in.x25.x25flags & TRANSF_X25_PRINTER;

    /* If the translate x25 option "printer" is selected, remote connection
     * is established, send a CALL CONNECT confirmation to X.25 only
     * if the LCI is still in DCE or DTE waiting state.
     */
    if (printer) {
        if (lci->lci_state == X25_P2 || lci->lci_state == X25_P3){
	    x25_process_pad_accept_call(lci);
	    free (lci);
	    /*
	     * Now that we actually have a connection, set up the vector,
	     * and do everything else that was deferred.
	     */
	    tty->ovector = &PADvector;
	    tty->ivector = &PADvector;
	    pt_do_login(trans, tty);
        }
        /* T11/T12 timer expired and the LCI is cleared before the result of
         * of the TCP connection is obtained.
	 */
        else {
            free (lci);
	    trans->active--;
	    death(tty);
	}
    }
}

/*
 * pt_pad_setup_x3_options
 *
 */
void pt_pad_setup_x3_options (translatetype *trans, tt_soc *tty)
{
    conntype *padconn = tty->conntop;
    pcbtype *pcb = (pcbtype *) padconn->tcb;
    int n;

    /*
     * Try to set the X3 options on the remote host to be similar to
     * those of a default state telnet session.
     */
    if (pcb && (trans->in.x25.profile)) {
	pad_defaults(pcb);
	pcb->flags &= ~PCB_GETPARAM;
	if (translate_debug)
	    buginf("\n%s%s%t: Sending profile %s", 
		   reg_invoke_pt_translate_string(INPROT(trans->type),
						  INPROT(trans->type)),
		   reg_invoke_pt_translate_string(OUTPROT(trans->type),
						  OUTPROT(trans->type)),
		   tty->ttynum, trans->in.x25.profile->name);
	for (n=0; n < trans->in.x25.profile->length; ) {
	    pad_option_emit((pcbtype *)padconn->tcb, X3_SETANDREAD,
			    trans->in.x25.profile->params[n],
			    trans->in.x25.profile->params[n+1]);
	    n+=2;
	}
	pad_putcontrol(pcb);
	do_x3list(padconn, trans->in.x25.profile->params,
		  trans->in.x25.profile->length, X3_SET);
    }
}

/*
 * pt_pad_clear_connection
 * Send a Clear request to the X.25 side and terminate the translation
 * session.
 */
void pt_pad_clear_connection (translatetype *trans, tt_soc *tty)
{
    conntype *padconn = tty->conntop;
    pcbtype *pcb = (pcbtype *) padconn->tcb;
    lcitype *lci = pcb->pcb_lci;
    boolean pvc = FALSE;
    boolean printer = FALSE;

    if (trans) {
    	pvc = trans->in.x25.x25flags & TRANSF_X25_PVC;
    	printer = trans->in.x25.x25flags & TRANSF_X25_PRINTER;
    }

    /* Its an SVC, check if the "printer" option is selected.
     * LAT/TCP connections couldn't be established.  Check the current
     * state of the LCI before sending a CLEAR REQUEST to X.25.
     */
    if (printer) {
	if (lci->lci_state == X25_P2 || lci->lci_state == X25_P3)
	    x25_sendcause(lci->lci_idb->hwptr, lci,
			  X25_CLEAR, X25_CLEAR_NOT_OBTAINABLE,
			  X25_DIAG_NO_INFO, NULL);
	free (lci);
    }

    trans->active--;
    /*
     * PVC translation entry has not been freed at the exec command
     * level in order to avoid dangling pointers 
     */
    if (pvc)
    	free(trans); 
    death(tty);
}		
