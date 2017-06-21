/* $Id: padauto.c,v 3.2 1995/11/17 18:59:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/padauto.c,v $
 *------------------------------------------------------------------
 * Translate PAD -> Any protocol defined in the Autocommand argument
 *
 * March 1995, Eman Schaffer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: padauto.c,v $
 * Revision 3.2  1995/11/17  18:59:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:22:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:34:39  hampton
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
#include "../pt/pt_pad_common.h"
#include "../pt/pt_autocommand.h"
#include "../h/auth_registry.regh"
#include "stacks.h"

#define PT_PADAUTO_MAJVERSION 1
#define PT_PADAUTO_MINVERSION 0
#define PT_PADAUTO_EDITVERSION 1

SUBSYS_HEADER(pt_padauto,
              PT_PADAUTO_MAJVERSION, PT_PADAUTO_MINVERSION,
              PT_PADAUTO_EDITVERSION, pt_padauto_init,
              SUBSYS_CLASS_PROTOCOL,
              "seq: pt_pad, pt_autocommand, pt_tcp, pt_slip_ppp",
              "req: pt_pad, pt_autocommand, pt_slip_ppp");

/*
 * pt_padauto_init
 *
 * Subsystem initialization code for pad -> any protocotl defined in
 * autocommand argument
 */

void pt_padauto_init (subsystype *subsys)
{
    reg_add_pt_x25input(TRANS_AUTOCOMMAND, pad_transto_auto, 
			"pad_transto_auto");
}

/*
 * padtoauto_daemon
 * process to manage one incoming PAD connection to any outgoing
 * protocol connection defined in the autocommand argument.
 */
static forktype padtoauto_daemon (translatetype *trans)
{
    tt_soc *vty = stdio;

    if (translate_debug)
        buginf("\ntrans:Started PAD-Auto:%d on vty:%t", vty->tiptop,
	       vty->ttynum);

    pt_pad_setup(trans, vty);
    pt_pad_check_lci(trans, vty);
    startoutput(vty);
    vty->statbits |= CONNECTED;
    pt_pad_setup_x3_options(trans, vty);
    /*
     * We now have a PAD connection going. Hook it with the outgoing exec
     * command defined in the autocommand argument.
     */
    pt_setup_auto_session(trans, vty);
}

/*
 * pad_transto_auto
 * Translate incoming PAD connection to any outgoing protocol defined in 
 * the autocommand argument.
 * Returns PID of translation process or -1 if failure
 * Called by SERVICE_TRANS_PADINPUT, index is TRANS_AUTO
 */
int pad_transto_auto (translatetype *trans, tt_soc *vty)
{
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    if (reg_invoke_kerberos_is_running())
	stacksize = (ulong) EXEC_STACK_SIZE;
    return(cfork((forkproc *)padtoauto_daemon, (long)trans, stacksize,
		 "PAD to AUTO", vty->ttynum));
}
