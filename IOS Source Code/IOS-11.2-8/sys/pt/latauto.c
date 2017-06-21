/* $Id: latauto.c,v 3.2 1995/11/17 18:59:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/latauto.c,v $
 *------------------------------------------------------------------
 * Translate LAT -> Any protocol defined in the Autocommand argument
 *
 * March 1995, Eman Schaffer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: latauto.c,v $
 * Revision 3.2  1995/11/17  18:59:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:22:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:34:30  hampton
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
#include "../pt/pt_lat_common.h"
#include "../pt/pt_autocommand.h"
#include "../h/auth_registry.regh"
#include "stacks.h"

/*
 * LAT-AUTCOMMAND Protocol Translator subsystem header
 */

#define PT_LATAUTO_MAJVERSION 1
#define PT_LATAUTO_MINVERSION 0
#define PT_LATAUTO_EDITVERSION 1

SUBSYS_HEADER(pt_latauto,
              PT_LATAUTO_MAJVERSION, PT_LATAUTO_MINVERSION,
              PT_LATAUTO_EDITVERSION, pt_latauto_init,
              SUBSYS_CLASS_PROTOCOL,
              "seq: pt_lat, pt_autocommand, pt_tcp, pt_slip_ppp",
              "req: pt_lat, pt_autocommand, pt_tcp, pt_slip_ppp");

/*
 * pt_latauto_init
 *
 * Subsystem initialization code for lat -> any protocol defined in the 
 * autocommand argument
 */

void pt_latauto_init (subsystype *subsys)
{
    reg_add_pt_latinput(TRANS_AUTOCOMMAND, lat_transto_auto, 
			"lat_transto_auto");
}

/*
 * lattoauto_daemon
 * process to manage one incoming LAT connection to any outgoing protocol
 * connection defined in the autocommand argument.
 */
static forktype lattoauto_daemon (translatetype *trans)
{
    tt_soc *vty = stdio;
    if (translate_debug)
	buginf("\ntrans:Started LAT-Auto:%d on vty:%t", vty->tiptop, 
	       vty->ttynum);
    pt_setup_lat_session(trans, vty);
    /*
     * We now have a LAT connection going. Hook it with the outgoing exec
     * command defined in the autocommand argument.
     */
    pt_setup_auto_session(trans, vty);
}    

/*
 * lat_transto_auto
 * Translate incoming LAT connection to any outgoing protocol defined in 
 * the autocommand argument.
 * Returns PID of translation process or -1 if failure
 * Called by SERVICE_TRANS_LATINPUT, index is TRANS_AUTO
 */
int lat_transto_auto (translatetype *trans, tt_soc *vty)
{
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    if (reg_invoke_kerberos_is_running())
	stacksize = (ulong) EXEC_STACK_SIZE;
    return(cfork((forkproc *)lattoauto_daemon, (long)trans, stacksize,
		 "LAT to AUTO", vty->ttynum));
}

