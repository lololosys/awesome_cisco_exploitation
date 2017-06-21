/* $Id: clns_adj2.c,v 3.2 1995/11/17 08:51:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/clns_adj2.c,v $
 *------------------------------------------------------------------
 * CLNS-only adjacency management routines
 *
 * January 1995, Dave Katz
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module consists of code cleaved off of clns_adj.c in order
 * to remove the CLNS-specific components of it (to allow NLSP
 * to exist without all of CLNS).
 *
 *------------------------------------------------------------------
 * $Log: clns_adj2.c,v $
 * Revision 3.2  1995/11/17  08:51:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/25  14:10:25  hampton
 * Eliminate unnecessary includes of novell.h.  [CSCdi39328]
 *
 * Revision 2.1  1995/06/07 20:16:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_adj.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_inlines.h"
#include "../xns/novell.h"
#include "../xns/ipx_registry.h"

/*
 * clns_delete_igrp_adj
 *
 * Called when deleting an adjacency created by ISO-IGRP.
 */
void clns_delete_igrp_adj (clns_pdbtype *pdb, clns_adjtype *adj)
{
    NSAP_T nsap;

    BUILD_NSAP(adj, nsap);
    pdb = clns_find_pdb(nsap);
    if (pdb) {
	
	/*
	 * Removes system id routes.
	 */
	if ((pdb->level == CLNS_LEVEL1) && !adj->L2only) {
	    clns_remove_neighbor_depend(pdb, adj->key);
	}
	adj->protocol_source &= ~pdb->typeflag;
	
	/*
	 * Removes area routes.
	 */
	if (pdb->level == CLNS_LEVEL1) {
	    pdb = clns_find_level2_pdb(nsap);
	}
	clns_remove_neighbor_depend(pdb, adj->key);
	adj->protocol_source &= ~pdb->typeflag;
    }
    
    /*
     * Removes prefix routes.
     */
    clns_remove_neighbor_depend(NULL, adj->key);
}
