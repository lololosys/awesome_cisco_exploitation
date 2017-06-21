/* $Id: tcpauto.c,v 3.2 1995/11/17 19:00:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/tcpauto.c,v $
 *------------------------------------------------------------------
 * Translate TCP -> Any protocol defined in the Autocommand argument
 *
 * March 1995, Eman Schaffer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcpauto.c,v $
 * Revision 3.2  1995/11/17  19:00:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:22:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:35:38  hampton
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
#include "../pt/pt_tcp_common.h"
#include "../pt/pt_autocommand.h"
#include "../h/auth_registry.regh"
#include "stacks.h"

#define PT_TCPAUTO_MAJVERSION 1
#define PT_TCPAUTO_MINVERSION 0
#define PT_TCPAUTO_EDITVERSION 1

SUBSYS_HEADER(pt_tcpauto,
              PT_TCPAUTO_MAJVERSION, PT_TCPAUTO_MINVERSION, 
	      PT_TCPAUTO_EDITVERSION, pt_tcpauto_init, 
	      SUBSYS_CLASS_PROTOCOL,
              "seq: pt_tcp, pt_autocommand, pt_lat, pt_slip_ppp",
              "req: pt_tcp, pt_autocommand, pt_lat, pt_slip_ppp");

/*
 * pt_tcpauto_init
 * 
 * Subsystem initialization code for tcp -> any protocol defined in the 
 * autocommand argument
 */
void pt_tcpauto_init (subsystype *subsys)
{
    reg_add_pt_tcpinput(TRANS_AUTOCOMMAND, tcp_transto_auto, 
			"tcp_transto_auto");
}

/*
 * tcptoauto_daemon
 * process to manage one tcp incoming connection to any outgoing protocol
 * connection efined in the autocommand argument.
 */
static forktype tcptoauto_daemon (translatetype *trans)
{
    tt_soc *vty = stdio;
    if (translate_debug)
        buginf("\ntrans: Started TCP-Auto:%d on vty:%t", vty->tiptop,
	       vty->ttynum);

    pt_setup_tcp_session(trans, vty);
    /*
     * We now have a TCP connection going. Hook it with the outgoing exec
     * command defined in the autocommand argument.
     */
    pt_setup_auto_session(trans, vty);
}

/*
 * tcp_transto_auto
 * Translate incoming TCP connection to any outgoing protocol defined in 
 * the autocommand argument.
 * Returns PID of translation process or -1 if failure
 * Called by SERVICE_TRANS_TCPINPUT, index is TRANS_AUTO
 */
int tcp_transto_auto (translatetype *trans, tt_soc *vty)
{
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    if (reg_invoke_kerberos_is_running())
	stacksize = (ulong) EXEC_STACK_SIZE;
    return(cfork((forkproc *)tcptoauto_daemon, (long)trans, stacksize,
		 "TCP to AUTO", vty->ttynum));
}
