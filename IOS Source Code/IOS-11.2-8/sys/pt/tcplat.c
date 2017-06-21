/* $Id: tcplat.c,v 3.4.6.1 1996/06/06 19:46:03 swenson Exp $
 * $Source: /release/112/cvs/Xsys/pt/tcplat.c,v $
 *------------------------------------------------------------------
 * tcplat.c  TCP --> LAT protocol translation.
 *
 * Jan 1991,  Bill Westfield  -- Split from tcppad.c
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcplat.c,v $
 * Revision 3.4.6.1  1996/06/06  19:46:03  swenson
 * CSCdi59591:  translate user count decremented incorrectly ...
 * Branch: California_branch
 *
 * Revision 3.4  1996/03/06  09:31:29  billw
 * CSCdi44863:  AAA Accounting not done for one-step protocol translation
 * Also ensure tty->ustruct gets set for one-step PT sessions.
 *
 * Revision 3.3  1996/01/29  11:38:43  billw
 * CSCdi46038:  vty access-class takes precedence over translate
 * access-class
 * Always use SYNC_LISTEN for incoming PT tcp sessions, and let the PT
 * process itself do the access check.
 *
 * Revision 3.2  1995/11/17  19:00:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:22:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/17  06:26:04  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.2  1995/06/09  13:18:28  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:35:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../lat/mtcsys.h"
#include "interface_private.h"
#include "address.h"
#include "name.h"
#include "packet.h"
#include "ttysrv.h"
#include "../ip/ip.h"
#include "subsys.h"
#include "registry.h"
#include "../pt/pt_registry.h"
#include "../tcp/tcp.h"
#include "connect.h"
#include "config.h"
#include "../tcp/telnet.h"
#include "../x25/x25.h"
#include "translate.h"
#include "../lat/latfnc.h"
#include "../lat/lat.h"
#include "../os/async.h"
#include "login.h"
#include "../ip/ipaccess.h"
#include "tcplat.h"
#include "pt_tcp_common.h"
#include "../h/auth_registry.regh"
#include "stacks.h"


/*
 * TCP-LAT Protocol Translator subsystem header
 */

#define PT_TCPLAT_MAJVERSION 1
#define PT_TCPLAT_MINVERSION 0
#define PT_TCPLAT_EDITVERSION  1

SUBSYS_HEADER(pt_tcplat,
              PT_TCPLAT_MAJVERSION, PT_TCPLAT_MINVERSION, PT_TCPLAT_EDITVERSION,
              pt_tcplat_init, SUBSYS_CLASS_PROTOCOL,
              "seq: pt_lat, pt_tcp",
              "req: pt_lat, pt_tcp");

/*
 * pt_tcplat_init
 *
 * Subsystem initialization code for tcp<->lat translation.
 */

void pt_tcplat_init (subsystype *subsys)
{
    reg_add_pt_tcpinput(TRANS_LAT, tcp_transto_lat, "tcp_transto_lat");
    reg_add_pt_latinput(TRANS_TCP, lat_transto_tcp, "lat_transto_tcp");
}


/*
 * tcptolat_dameon
 * translate incoming TCP to outgoing LAT.
 * Note: Some of the pt_..... helper subroutines are allowed to kill
 * the thread "death()" and might not return to the daemon process.
 */
static forktype tcptolat_daemon (translatetype *trans)
{
    tt_soc *tty = stdio;
    conntype *tcpconn = tty->conntop;
    conntype *latconn;

    if (translate_debug)
	buginf("\ntcplat%t: fork started", tty->ttynum);

    if ((trans->in.tcp.tcpflags & TRANSF_TCP_PRINTER)) {
	pt_tcp_check_access(trans, tty);
    } else {
	pt_setup_tcp_session(trans, tty);
    }

    latconn = lat_open_connection(tty,
				  trans->out.lat.latservice,
				  trans->out.lat.latnode,
				  trans->out.lat.latport,
				  trans->flags & TRANSF_GLOB_QUIET ?
				  LATOPEN_QUIET : 0,
				  NULL);

    startoutput(tty);
    if (latconn == NULL) {
	if (translate_debug)
	    buginf("\ntcplat%t: can't get lat connection", tty->ttynum);
	trans->failures++;
	if (!(trans->in.tcp.tcpflags & TRANSF_TCP_PRINTER)) 
	    trans->active--;
	death(tty);
    }

    if (trans->in.tcp.tcpflags & TRANSF_TCP_PRINTER) {
	pt_setup_tcp_session(trans, tty);
    }

    /*
     * Set up the TCP incoming queuemax. A non-zero value is used
     * to decide tcp SEGQUEUE buffer coalescing.
     */
    ((tcbtype *)tcpconn->tcb)->qinput_max = TRANS_QUEUE_MAX;
    if (translate_debug)
       buginf("\ntcplat%t: queuemax = %d", tty->ttynum,
               ((tcbtype *)tcpconn->tcb)->qinput_max);

    /*
     * We now have a connection going in each direction, one TCP, and one
     * LAT.  Shovel bytes both ways, and translate state changes.
     */
    psioff(tty);
    latconn->acct_info = reg_invoke_aaa_acct_conn(tty->ustruct, NULL,
						  latconn);
    tty->statbits |= CONNECTED;
    lat_run(latconn, FALSE);
    tty->statbits &= ~CONNECTED;
    conn_close(latconn);
    trans->active--;
    death(tty);
    process_kill(THIS_PROCESS);
}

/*
 * tcp_transto_lat
 * Translate TCP input to LAT output
 * Returns PID of translation process or -1 if failure
 * Called by SERVICE_TRANS_TCPINPUT, index is TRANS_LAT
 */

int tcp_transto_lat (translatetype *trans, tt_soc *tty)
{
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    if (reg_invoke_kerberos_is_running())
	stacksize = (ulong) EXEC_STACK_SIZE;
    return(cfork((forkproc *)tcptolat_daemon, (long)trans, stacksize,
		 "TCP to LAT", tty->ttynum));
}
