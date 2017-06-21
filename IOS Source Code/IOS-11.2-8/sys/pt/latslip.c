/* $Id: latslip.c,v 3.2 1995/11/17 18:59:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/latslip.c,v $
 *------------------------------------------------------------------
 * latslip.c  LAT --> SLIP/PPP protocol 
 *
 * May 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: latslip.c,v $
 * Revision 3.2  1995/11/17  18:59:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:22:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:34:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../lat/mtcsys.h"
#include "../lat/latsys.h"
#include "../lat/latob.h"
#include "../lat/latsb.h"
#include "../lat/lathi.h"
#include "../lat/latslt.h"
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
#include "../tcp/telnet.h"
#include "access.h"
#include "translate.h"
#include "login.h"
#include "../os/async.h"
#include "termqueue.h"
#include "../lat/sysfnc.h"
#include "../lat/latfnc.h"
#include "../lat/lat.h"
#include "tcplat.h"
#include "vtyasync.h"
#include "pt_slip_ppp.h"
#include "pt_lat_common.h"
#include "../h/auth_registry.regh"
#include "stacks.h"


/*
 * LAT-SLIP Protocol Translator subsystem header
 */

#define PT_LATSLIP_MAJVERSION 1
#define PT_LATSLIP_MINVERSION 0
#define PT_LATSLIP_EDITVERSION 1

SUBSYS_HEADER(pt_latslip_ppp,
              PT_LATSLIP_MAJVERSION, PT_LATSLIP_MINVERSION,
              PT_LATSLIP_EDITVERSION, pt_latslip_ppp_init,
              SUBSYS_CLASS_PROTOCOL,
              "seq: pt_lat, pt_tcp, pt_slip_ppp",
              "req: pt_lat, pt_tcp, pt_slip_ppp");
/*
 * pt_latslip_ppp_init
 *
 * Subsystem initialization code for lat -> slip/ppp 
 */

void pt_latslip_ppp_init (subsystype *subsys)
{
    reg_add_pt_latinput(TRANS_SLIP_PPP, lat_transto_slip, "lat_transto_slip");
}

/*
 * latslip_daemon
 * process to manage one incoming lat connection to outgoing slip/ppp
 *
 */
static forktype lattoslip_daemon (translatetype *trans)
{
    tt_soc *vty = stdio;

    if (translate_debug)
        buginf("\nlatslip%t: fork %d started", vty->ttynum, vty->tiptop);

    pt_setup_lat_session(trans, vty);
    /*
     * We now have a LAT connection going.  Setup and run the vty-async
     * connection.
     */
    pt_run_vtyasync(trans, vty);
    pt_close_vtyasync(trans, vty);
}
/*
 * lat_transto_slip
 * Pass LAT input to SLIP/PPP output
 * Returns PID of translation process or -1 if failure
 * Called by SERVICE_TRANS_LATINPUT, index is TRANS_SLIP_PPP
 */
int lat_transto_slip (translatetype *trans, tt_soc *vty)
{
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    if (reg_invoke_kerberos_is_running())
	stacksize = (ulong) EXEC_STACK_SIZE;
    return(cfork((forkproc *)lattoslip_daemon, (long)trans, stacksize,
		 "LAT to ASYNC", vty->ttynum));

}
