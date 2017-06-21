/* $Id: iprouting_actions.c,v 3.2.60.2 1996/08/23 22:02:29 jserpa Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/iprouting_actions.c,v $
 *------------------------------------------------------------------
 * IP routing specific parsing functions.
 *
 * August 1994, Bruce Cole
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iprouting_actions.c,v $
 * Revision 3.2.60.2  1996/08/23  22:02:29  jserpa
 * CSCdi67021:  router ospf <as> gets nv-ramed in reverse order of
 * configuration
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/15  21:18:11  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:34:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:07:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "iprouting_actions.h"
#include "packet.h"
#include "../ip/ip.h"
#include "route.h"

/*
 * ip_get_previous_pdb
 *
 * Returns pdb that appears before prev_pdb. If NULL is returned,
 * we reached the head of the queue.
 *	
 */
static pdbtype *ip_get_previous_pdb (pdbtype *pdb)
{
    pdbtype *prev_pdb;

    for (prev_pdb = protoQ.qhead; prev_pdb; prev_pdb = prev_pdb->next) {
	if (prev_pdb->next == pdb) {
	    return (prev_pdb);
	}
    }
    return (NULL);
}

/*
 * parser_ip_router
 *
 * This function needs to process the pdb queue in the reverse order
 * from the protoQ. However since there is no previous pointer in the
 * pdbtype, we need to artificially follow the protoQ backwards.
 */

void parser_ip_router (parseinfo *csb, const char *name,
		       char *cp, transition *accept)
{
    pdbtype *pdb;
    char buffer[26];

    /*
     * Go through the protoQ in reverse order so push_node() can NVgen
     * in forward order of protoQ.
     */
    for (pdb = protoQ.qtail; pdb; pdb = ip_get_previous_pdb(pdb)) {
	if (!pdb->running)
	    continue;
	if (strcmp(name, pdb->name))
	    continue;

	switch (pdb->proctype) {
	case PROC_BGP:
	case PROC_EGP:
	case PROC_IGRP:
	case PROC_IGRP2:
	case PROC_OSPF:
	    sprintf(buffer, " %s %d", pdb->name, pdb->asystem);
	    strcpy(cp, buffer);
	    break;
	case PROC_RIP:
	default:
	    *cp = ' ';
	    strcpy((cp + 1), pdb->name);
	    break;
	case PROC_ISIS:
	    continue;
	}

	SETOBJ(int,1) = PDB_IP;
	SETOBJ(pdb,1) = pdb;
	push_node(csb, accept);
    }
}
