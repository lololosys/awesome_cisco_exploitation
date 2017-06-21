/* $Id: latpad.c,v 3.3.6.1 1996/04/04 06:24:50 swenson Exp $
 * $Source: /release/112/cvs/Xsys/pt/latpad.c,v $
 *------------------------------------------------------------------
 * latpad.c -- LAT -> PAD protocol conversion.
 *
 * Jan 1991,  Bill Westfield
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: latpad.c,v $
 * Revision 3.3.6.1  1996/04/04  06:24:50  swenson
 * CSCdi52237:  pad doesnt respond to set or set and read with no
 * parameters
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/06  09:31:21  billw
 * CSCdi44863:  AAA Accounting not done for one-step protocol translation
 * Also ensure tty->ustruct gets set for one-step PT sessions.
 *
 * Revision 3.2  1995/11/17  18:59:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:22:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/10  00:04:39  eschaffe
 * CSCdi35298:  Make sure all buginf messages are enabled by
 * "translate_debug" or "pad_debug" boolean.
 *
 * Revision 2.2  1995/06/09  13:18:15  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:34:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include <ciscolib.h>
#include "../lat/mtcsys.h"
#include "../lat/latsys.h"
#include "../lat/latob.h"
#include "../lat/latsb.h"
#include "../lat/lathi.h"
#include "../lat/latslt.h"
#include "address.h"
#include "name.h"
#include "packet.h"
#include "config.h"
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
#include "translate.h"
#include "pt_lat.h"
#include "logger.h"
#include "../os/async.h"
#include "login.h"
#include "../x25/x25_route.h"
#include "../h/auth_registry.regh"
#include "stacks.h"

#include "latpad.h"

/*
 * External symbols
 */

extern vectortype LATvector;
extern void termqueue_remove(void *);
extern void str_return(byte *, byte *, int);
extern void hi_reject(hi_ptr, int);
extern void *hi_open(hi_ptr, void *, int);
extern boolean lat_openBLOCK(conntype *);

/*
 * LAT-PAD Protocol Translator subsystem header
 */

#define PT_LATPAD_MAJVERSION 1
#define PT_LATPAD_MINVERSION 0
#define PT_LATPAD_EDITVERSION  1

SUBSYS_HEADER(pt_latpad,
              PT_LATPAD_MAJVERSION, PT_LATPAD_MINVERSION, PT_LATPAD_EDITVERSION,
              pt_latpad_init, SUBSYS_CLASS_PROTOCOL,
              "seq: pt_lat, pt_pad",
              "req: pt_lat, pt_pad");


/*
 * pt_latpad_init
 *
 * Subsystem initialization code for pad<->lat translation.
 */

void pt_latpad_init (subsystype *subsys)
{
    reg_add_pt_x25input(TRANS_LAT, x25_transto_lat, "x25_transto_lat");
    reg_add_pt_latinput(TRANS_X25, lat_transto_x25, "lat_transto_x25");
}

/*
 * latpad
 * translate from incoming LAT to outgoing PAD.
 *
 * For now, start up exec with appropriate arguments
 */

static forktype latpad (translatetype *trans)
{
    idbtype *idb = NULL;
    tt_soc *tty = stdio;
    conntype *padconn;
    conntype *latconn = tty->conntop;
    int n;
    pcbtype *pcb; /* so we can save our original paramters */	

    if (latconn->proto.lat.flags & LAT_CONN_HIC) {
	latpadhic(trans);
	goto latpad_die;
    }
    if (translate_debug)
	buginf("\nlatpad%t: fork started", tty->ttynum);


    tty->ovector = &LATvector;
    tty->ivector = &LATvector;

    trans->active++;
    if (trans->active > trans->maxusers)
	trans->maxusers = trans->active;

    idb = (idbtype *)pick_x25_idb(trans->out.x25.x121addr, 
				  trans->out.x25.cud,
				  strlen((char *)trans->out.x25.cud));
    
    if (!idb) {
	if (!(trans->flags & TRANSF_GLOB_QUIET)) {
	    printf("X.25 is not available.");
	    startoutput(tty);
	    }
	if (translate_debug)
	    buginf("\nlatpad%t: No X.25 interface present", tty->ttynum);
	trans->failures++;
	goto latpad_die;
    }

    /*
     * Perhaps check for a password.  Diddle the IO vectors and
     * use the EXEC code...
     */

    pt_do_login(trans, tty);


    padconn = pad_open_connection(idb, trans->out.x25.x121addr,
				  trans->flags & TRANSF_GLOB_QUIET ? &n : NULL,
				  FALSE, trans->out.x25.cud, 
				  strlen((char *) trans->out.x25.cud),
				  trans->out.x25.x25flags,
				  NULL);
    if (padconn == NULL) {
	startoutput(tty);
	if (translate_debug)
	    buginf("\nlatpad%t: can't get pad connection", tty->ttynum);
	trans->failures++;
	trans->active--;
	death(tty);
    }

    padconn->acct_info = reg_invoke_aaa_acct_conn(tty->ustruct, NULL,
						  padconn);

    /*
     * If we have a profile we are supposed to use, fake it by
     * pretending we received a SET for those parameters.
     */
    if (trans->out.x25.profile) {
	if (translate_debug)
	    buginf("\ntcppad%t: Loading profile %s", tty->ttynum,
		   trans->out.x25.profile->name);
	do_x3list(padconn, trans->out.x25.profile->params,
		  trans->out.x25.profile->length, X3_SETQUIETLY);
	/* save changes to outgoing parameters as our original */
	pcb = (pcbtype *)padconn->tcb;
	bcopy(pcb->paramsout, pcb->paramsorig, sizeof(pcb->paramsout));
    }

    psioff(tty);
    tty->statbits |= CONNECTED;
    pad_telnet(padconn, FALSE);
    tty->statbits &= ~CONNECTED;
    conn_close(padconn);
latpad_die:
    trans->active--;
    death(tty);
    process_kill(THIS_PROCESS);
}

/*
 * latpadhic
 * special handling for Host initiated connections.  Make sure we can actually
 * create the outgoing connection before we fully accept the HIC...
 */

void latpadhic (translatetype *trans)
{
    idbtype *idb = NULL;
    tt_soc *tty = stdio;
    conntype *latconn = tty->conntop;
    conntype *padconn = NULL;
    int n;
    hi_ptr hi = latconn->proto.lat.hiptr;
    void *sb;

    if (translate_debug)
	buginf("\nlatpad%t: HIC fork started", tty->ttynum);


    trans->active++;
    if (trans->active > trans->maxusers)
	trans->maxusers = trans->active;

    idb = pick_x25_idb(trans->out.x25.x121addr, 
		       trans->out.x25.cud,
		       strlen((char *)trans->out.x25.cud));

    if (!idb) goto hicqueue;
    /*
     * Perhaps check for a password.  Diddle the IO vectors and
     * use the EXEC code...
     */
    if ((trans->flags & TRANSF_GLOB_LOGIN)) {
	if (translate_debug)
	    buginf("\nCan't query for password on HIC connection");
	goto hicqueue;
    }

    tty->ovector = &LATvector;
    tty->ivector = &LATvector;
    padconn = pad_open_connection(idb,trans->out.x25.x121addr,
				  &n, FALSE, trans->out.x25.cud,
				  strlen((char *) trans->out.x25.cud),
				  trans->out.x25.x25flags,
				  NULL);
    if (padconn == NULL) {
hicqueue:
	/*
	 * If Data Buffer is allocated, deallocate it
	 */
	if (latconn->proto.lat.databuf) {
	    free(latconn->proto.lat.databuf);
	    latconn->proto.lat.databuf = NULL;
	}
	freeconnection(latconn);
	if (translate_debug)
	    buginf("\nlatpad%t: connection remains queued", tty->ttynum);
	tty_reset(tty);			/* reset terminal parameters */
        trans->active--;
	tty_deallocate(tty);		/* line is no longer assigned */
	kfork(tty);
    }

    /*
     * The outgoing conection worked.  Open up the incoming connection,
     * and remove it from the terminal queue.
     */
    termqueue_remove(hi);
    str_return((byte *)latconn->hostname, hi->hi_node, CONNLEN);
    /* Set up connection name */
    latconn->proto.lat.max_credits = LAT_INBOUND_CREDITS;
    sb = hi_open(hi, latconn, latconn->proto.lat.max_credits);
    if (sb) {
	latconn->status |= CONN_INBOUND; /* Mark as inbound */
	latconn->proto.lat.t_capabilities = tty->capabilities;
	latconn->proto.lat.t_parity = tty->tty_parity;
	latconn->proto.lat.t_databits = tty->tty_databits;
	latconn->tcb =  sb;
	latconn->proto.lat.hiptr = NULL; /* No longer queued */
	tty->tty_autocommand = NULL;
    } else {
	hi_reject(hi, SLOT_STOP_NO_RESOURCE);	/* No resources */
	goto hicqueue;
    }
    edisms((blockproc *)lat_openBLOCK, (ulong) latconn);

    /*
     * If we have a profile we are supposed to use, fake it by
     * pretending we received a SET for those parameters.
     */
    if (trans->out.x25.profile) {
	if (translate_debug)
	    buginf("\ntcppad%t: Loading profile %s", tty->ttynum,
		   trans->out.x25.profile->name);
	do_x3list(padconn, trans->out.x25.profile->params,
		  trans->out.x25.profile->length, X3_SET);
    }

    psioff(tty);
    tty->statbits |= CONNECTED;
    pad_telnet(padconn, FALSE);
    tty->statbits &= ~CONNECTED;
    conn_close(padconn);
}

/*
 * lat_transto_x25
 * Translate LAT input to TCP output
 * Returns PID of translation process or -1 if failure
 * Called by SERVICE_TRANS_LATINPUT, index is TRANS_X25
 */

int lat_transto_x25 (translatetype *trans, tt_soc *tty)
{
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    if (reg_invoke_kerberos_is_running())
	stacksize = (ulong) EXEC_STACK_SIZE;
    return(cfork((forkproc *)latpad, (long)trans, stacksize, "LAT to PAD",
		 tty->ttynum));
}

