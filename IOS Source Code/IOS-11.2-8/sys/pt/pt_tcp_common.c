/* $Id: pt_tcp_common.c,v 3.3.20.1 1996/05/04 01:37:32 wilber Exp $
 * $Source: /release/112/cvs/Xsys/pt/pt_tcp_common.c,v $
 *------------------------------------------------------------------
 * pt_tcp_common.c  Protocol translation common subfunctions shared by 
 * tcp to lat/pad/slip_ppp files.
 *
 * Aug. 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pt_tcp_common.c,v $
 * Revision 3.3.20.1  1996/05/04  01:37:32  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.3  1996/01/29  11:38:40  billw
 * CSCdi46038:  vty access-class takes precedence over translate
 * access-class
 * Always use SYNC_LISTEN for incoming PT tcp sessions, and let the PT
 * process itself do the access check.
 *
 * Revision 3.2  1995/11/17  19:00:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/17  06:26:02  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.2  1995/06/09  13:18:27  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:35:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "address.h"
#include "name.h"
#include "packet.h"
#include "ttysrv.h"
#include "connect.h"
#include "config.h"
#include "../ip/ip.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../tcp/tcp.h"
#include "../tcp/telnet.h"
#include "access.h"
#include "registry.h"
#include "../pt/pt_registry.h"
#include "translate.h"
#include "../os/async.h"
#include "login.h"
#include "subsys.h"
#include "../pt/pt_tcp_common.h"


/*
 * pt_setup_tcp_session
 * This sets up tcp for both normal and printer connections, but it is
 * done AFTER the outgoing connection is established for printers, and
 * much earlier for normal connections.
 */
void pt_setup_tcp_session (translatetype *trans, tt_soc *vty)
{
    conntype *tcpconn = vty->conntop;
    tcbtype *tcb = (tcbtype *) tcpconn->tcb; 

    pt_tcp_check_access(trans, vty);

    /* Finish connection processing */
    if (tcp_establish(tcb) == -1) {
	if (translate_debug)
	    buginf("\n%s%s%t: can't complete tcp connection",
		   reg_invoke_pt_translate_string(INPROT(trans->type),
						  INPROT(trans->type)),
		   reg_invoke_pt_translate_string(OUTPROT(trans->type), 
						  OUTPROT(trans->type)),
		   vty->ttynum);
	trans->failures++;
	death(vty);
	/*NOTREACHED*/
    }
    vty->ovector = &TCPvector;
    vty->ivector = &TCPvector;

    trans->active++;
    if (trans->active > trans->maxusers)
	trans->maxusers = trans->active;

    /*
     * Setup the telnet default options, check login, and access class
     * if selected. 
     */
    pt_tcp_check_options(trans, vty);
    startoutput(vty);
    psioff(vty);
    vty->statbits |= CONNECTED;
}

/*
 * pt_tcp_check_options
 * Setup the telnet default options and check login if selected.
 */
void pt_tcp_check_options (translatetype *trans, tt_soc *tty)
{
    conntype *tcpconn = tty->conntop;

    /*
     * Try to set the telnet options on the remote host to be similar to
     * those of a pad (eg local echo, 8 bit binary data, no crlf hacking).
     */
    if (trans->in.tcp.tcpflags & TRANSF_TCP_STREAM)
	tcpconn->status |= CONN_STREAM;	
    else telnet_defaults(tcpconn, trans->in.tcp.tcpflags &
			 TRANSF_TCP_BINARY);
    pt_do_login(trans, tty);

}

/* 
 * pt_tcp_check_access
 * Check if this foreign host should be allowed to pass through
 * the translation.  Note that this code is now called before the
 * tcp connection is fully established, so it won't generate a
 * message to the user.
 */
void pt_tcp_check_access (translatetype *trans, tt_soc *tty)
{
    conntype *tcpconn = tty->conntop;
    tcbtype *tcb = (tcbtype *) tcpconn->tcb; 
    int accessclass;

    if (trans) {
	if (trans->accessclass)
	    accessclass = trans->accessclass;
	else
	    accessclass = tty->accessin;

	if (fast_check(accessclass, tcb->foreignhost.ip_addr))
	    return;		/* access is OK */
	if (translate_debug)
	    buginf("\n%s%s%t: Translation access denied, list %d",
		   reg_invoke_pt_translate_string(INPROT(trans->type),
						  INPROT(trans->type)),
		   reg_invoke_pt_translate_string(OUTPROT(trans->type), 
						  OUTPROT(trans->type)),
		   tty->ttynum, accessclass);
    } else {
	if (translate_debug)
	    buginf("\n%s%s%t: Translate command missing",
		   reg_invoke_pt_translate_string(INPROT(trans->type),
						  INPROT(trans->type)),
		   reg_invoke_pt_translate_string(OUTPROT(trans->type), 
						  OUTPROT(trans->type)),
		   tty->ttynum);
    }
    trans->failures++;
    death(tty);
}
