/* $Id: iprouting_setup.c,v 3.4.4.1 1996/06/28 01:21:03 snyder Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/iprouting_setup.c,v $
 *------------------------------------------------------------------
 * IP Routing setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: iprouting_setup.c,v $
 * Revision 3.4.4.1  1996/06/28  01:21:03  snyder
 * CSCdi61601:  fix some char * that should have been char [] in iprouting
 * Branch: California_branch
 *
 *              You get space back for the pointer
 *              You get space back for each reference
 *              You get bytes out of data space
 *
 * Revision 3.4  1996/03/09  22:56:04  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.3  1995/11/28  03:58:38  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  17:34:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:07:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "../ui/setup.h"
#include "../iprouting/route.h"

typedef struct iprouting_setup_struct_ {
    boolean run_ip_igrp;
    boolean run_ip_rip;
    int igrp_as;
} iprouting_setup_struct;

static const char ip_proto_name[] = "IP";

void iprouting_setup_init (setup_struct *sp)
{
    iprouting_setup_struct *iss;
    pdbtype *pdb;

    sp->args[LINK_IP] = FALSE;
    if (system_loading || !router_enable) {
	return;
    }

    if ((iss = malloc(sizeof(iprouting_setup_struct))) == NULL) {
	return;
    }
    iss->igrp_as = 1;		/* default to be overwritten by existing */

    sp->args[LINK_IP] = iss;
    for (pdb = (pdbtype *)protoQ.qhead; pdb; pdb = pdb->next) {
	if (!pdb->running) {
	    continue;
	}
	if (pdb->proctype & PROC_IGRP) {
	    iss->igrp_as = pdb->asystem;
	    iss->run_ip_igrp = TRUE;
	    continue;
	}
	if (pdb->proctype & PROC_RIP) {
	    iss->run_ip_rip = TRUE;
	    continue;
	}
    }
    if (!iss->run_ip_rip) {
       iss->run_ip_igrp = TRUE;
    }
}

void iprouting_setup_global (setup_struct *sp)
{
    iprouting_setup_struct *iss = sp->args[LINK_IP];

    if (system_loading || !router_enable || !sp->args[LINK_IP]) {
	sp->run_ip = TRUE;
	if (sp->args[LINK_IP]) {
	    free(sp->args[LINK_IP]);
	    sp->args[LINK_IP] = FALSE;
	}
	return;
    }

    sp->run_ip = system_protocol(sp, ip_proto_name, RUNNING(LINK_IP));
    if (sp->run_ip) {
	iss->run_ip_igrp = yes_or_no("    Configure IGRP routing",
				     iss->run_ip_igrp, TRUE);
	if (iss->run_ip_igrp) {
	    iss->igrp_as = get_num_in_range("      Your IGRP autonomous "
					    "system number", 10, 1, 0xFFFF,
					    iss->igrp_as);
	}
	if (!iss->run_ip_igrp) {
	    iss->run_ip_rip = yes_or_no("    Configure RIP routing",
					iss->run_ip_rip, TRUE);
	}
    }
    /*
     * If we have turned off IP routing, but are running SNMP, ask for
     * IP addresses for the interfaces.
     */
    if (!sp->run_ip && sp->run_ip_snmp) {
        iss->run_ip_igrp = FALSE;
	iss->run_ip_rip = FALSE;
        sp->run_ip = TRUE;
    }
}

void iprouting_setup_end (setup_struct *sp)
{
    iprouting_setup_struct *iss = sp->args[LINK_IP];
    int index;

    if (!sp->args[LINK_IP]) {
	return;
    }

    if (sp->run_ip) {
	if (iss->run_ip_igrp) {
	    setup_printf(sp, "\n!\nrouter igrp %d", iss->igrp_as);
	} else {
	    if (iss->run_ip_rip) {
		setup_printf(sp, "\n!\nrouter rip");
	    }
	}
	if (sp->addrlist && (iss->run_ip_igrp || iss->run_ip_rip)) {
	    for (index = 0; sp->addrlist[index]; index++) {
		setup_printf(sp, "\nnetwork %i", sp->addrlist[index]);
	    }
	}
    }
    free(sp->args[LINK_IP]);
    sp->args[LINK_IP] = NULL;
}
