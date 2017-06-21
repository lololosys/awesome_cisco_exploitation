/* $Id: cdp_ncp.c,v 3.2.62.3 1996/06/24 09:26:59 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/cdp_ncp.c,v $
 *------------------------------------------------------------------
 * CDP support over PPP
 *
 * March 1995, Arun Sastry
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cdp_ncp.c,v $
 * Revision 3.2.62.3  1996/06/24  09:26:59  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.2.62.2  1996/05/10  01:44:18  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/04/29  09:45:16  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.2  1995/11/17  18:00:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:17:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "ppp.h"
#include "ncp.h"
#include "cdp_ncp.h"
#include "ppp_public.h"
#include "ppp_registry.h"

static fsm_callbacks cdp_callbacks = { /* CDP callback routines */
    NULL,		        /* Reset our Configuration Information */
    NULL,			/* Length of our Configuration Information */
    NULL,			/* Add our Configuration Information */
    cdp_ackci,			/* ACK our Configuration Information */
    NULL,			/* NAK our Configuration Information */
    NULL,			/* Reject our Configuration Information */
    cdp_reqci,			/* Request peer's Configuration Information */
    NULL,			/* Called when fsm reaches OPEN state */
    NULL,			/* Called when fsm leaves OPEN state */
    NULL,			/* Called when fsm reaches CLOSED state */
    NULL,			/* Called when Protocol-Reject received */
    NULL,			/* Retransmission is necessary */
	NULL,			/* Reset Request */
	NULL,			/* Rest Acknowledge */
	NULL			/* Code Rejected */
};

/*
 * cdpcp_init - Initialize CDP.
 */
static void cdpcp_init (fsm *cdp_fsm)
{
    cdp_fsm->ouroptions = NULL;

    cdp_fsm->retransmits = 0;
    TIMER_STOP(cdp_fsm->timeouttime);
    cdp_fsm->failures = 0;
    cdp_fsm->restarttime = DEFTIMEOUT;
    cdp_fsm->maxconfigure = DEFMAXCONFIGURE;
    cdp_fsm->maxtermtransmits = DEFMAXTERMTRANSMITS;
    cdp_fsm->maxnakloops = DEFMAXNAKLOOPS;
    cdp_fsm->maxfailure = DEFMAXFAILURE;
    cdp_fsm->callbacks = &cdp_callbacks;
    cdp_fsm->flags |= AOPENDING;           /* Active Open */
}

/*
 * cdp_reqci - Handle CONFREQ.
 *
 */
uchar cdp_reqci (fsm *f, uchar *inp, short *len)
{
    if (ppp_negotiation_debug)
	buginf ( "\nppp: cdp_reqci: returning CONFACK");

    return (CONFACK);
}

/*
 * cdp_ackci - Ack CIs.
 *
 */

int cdp_ackci (fsm *f, uchar *p, int len)
{
    if (ppp_negotiation_debug)
        buginf ( "\nppp: cdp_reqci: received CONFACK");
  
    return (TRUE);
}

/*
 * cdpcp_init_subsys
 * Initialization of CDPCP subsystem
 */

static void cdpcp_init_subsys (subsystype *subsys)
{
    reg_invoke_ncp_register(LINK_CDP, TYPE_PPP_CDPCP,
			    cdpcp_init, NULL, "CDP");
}

/*
 * The CDPCP subsystem header
 */

#define CDPCP_MAJVERSION 1
#define CDPCP_MINVERSION 0
#define CDPCP_EDITVERSION  1

SUBSYS_HEADER(cdpcp, CDPCP_MAJVERSION, CDPCP_MINVERSION, CDPCP_EDITVERSION,
	      cdpcp_init_subsys, SUBSYS_CLASS_PROTOCOL,
	      "seq: ppp",
	      "req: cdp, ppp");
