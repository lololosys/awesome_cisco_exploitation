/* $Id: tcpslip.c,v 3.3 1996/01/29 11:38:46 billw Exp $
 * $Source: /release/111/cvs/Xsys/pt/tcpslip.c,v $
 *------------------------------------------------------------------
 * tcpslip.c  TCP --> SLIP/PPP protocol
 *
 * May, 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcpslip.c,v $
 * Revision 3.3  1996/01/29  11:38:46  billw
 * CSCdi46038:  vty access-class takes precedence over translate
 * access-class
 * Always use SYNC_LISTEN for incoming PT tcp sessions, and let the PT
 * process itself do the access check.
 *
 * Revision 3.2  1995/11/17  19:00:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:22:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/27  16:50:56  eschaffe
 * CSCdi38359:  Incoming vty-async connections over TCP should use term
 * download
 *
 * Revision 2.2  1995/06/17  06:26:07  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.1  1995/06/07  22:35:46  hampton
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
#include "ttysrv.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "subsys.h"
#include "registry.h"
#include "../pt/pt_registry.h"
#include "connect.h"
#include "../tcp/telnet.h"
#include "access.h"
#include "translate.h"
#include "logger.h"
#include "login.h"
#include "../os/async.h"
#include "aaa.h"
#include "../ts/if_slip.h"
#include "../tcp/tcpcompress.h"
#include "vtyasync.h"
#include "pt_slip_ppp.h"
#include "pt_tcp_common.h"
#include "../h/auth_registry.regh"
#include "stacks.h"


/*
 * TCP-SLIP Protocol subsystem header
 */

#define PT_TCPSLIP_MAJVERSION 1
#define PT_TCPSLIP_MINVERSION 0
#define PT_TCPSLIP_EDITVERSION  1

SUBSYS_HEADER(pt_tcpslip_ppp,
              PT_TCPSLIP_MAJVERSION, PT_TCPSLIP_MINVERSION, 
	      PT_TCPSLIP_EDITVERSION, pt_tcpslip_ppp_init, 
	      SUBSYS_CLASS_PROTOCOL,
              "seq: pt_tcp, pt_slip_ppp",
              "req: pt_tcp, pt_slip_ppp");

/*
 * pt_tcpslip_ppp_init
 *
 * Subsystem initialization code for tcp -> slip/ppp 
 */

void pt_tcpslip_ppp_init (subsystype *subsys)
{
    reg_add_pt_tcpinput(TRANS_SLIP_PPP, tcp_transto_slip, "tcp_transto_slip");
}

/*
 * tcptoslip_daemon
 * process to manage one incoming tcp connection to outgoing slip/ppp
 */
static forktype tcptoslip_daemon (translatetype *trans)
{
    tt_soc *vty = stdio;
    int saved_escapechar = vty->tty_escapechar;
    ulong saved_capabilities = vty->capabilities;

    if (translate_debug)
	buginf("\ntcpasync%t: fork started", vty->ttynum);

    pt_setup_tcp_session(trans, vty);

    /*
     * TCP connections should be tunneled automatically.  This can be 
     * accomplished by:
     * Making the link transparent, stream, and escape character none.
     */
    vty->capabilities |= TRANSPARENT;
    vty->tty_escapechar = -1;
    /*
     * We now have a TCP connection going.  Setup and run the vty-async
     * connection.
     */
    pt_run_vtyasync(trans, vty);
    /*
     * Restore vty parameters.
     */
    vty->capabilities &= ~TRANSPARENT;
    vty->capabilities |= (saved_capabilities & TRANSPARENT);
    vty->tty_escapechar = saved_escapechar;
    pt_close_vtyasync(trans, vty);
}
/*
 * tcp_transto_slip
 * Pass TCP input to SLIP/PPP output
 * Returns PID of translation process or -1 if failure
 * Called by SERVICE_TRANS_TCPINPUT, index is TRANS_SLIP_PPP
 */
int tcp_transto_slip (translatetype *trans, tt_soc *vty)
{
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    if (reg_invoke_kerberos_is_running())
	stacksize = (ulong) EXEC_STACK_SIZE;
    return(cfork((forkproc *)tcptoslip_daemon, (long)trans, stacksize,
		 "TCP to ASYNC", vty->ttynum));
}

