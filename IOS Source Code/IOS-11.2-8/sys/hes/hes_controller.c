/* $Id: hes_controller.c,v 3.4.46.1 1996/03/18 19:40:38 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/hes_controller.c,v $
 *------------------------------------------------------------------
 * hes_controller.c - Platform specific controller support for hes.
 *                 (based on hes/controller.c by Ronnie Kon)
 *
 * Jan 30 1995, Manoj Leelanivas
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hes_controller.c,v $
 * Revision 3.4.46.1  1996/03/18  19:40:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.16.2  1996/03/07  09:38:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.16.1  1996/02/20  14:19:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/07  22:28:29  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  09:16:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:23  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:59:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/23  18:47:32  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/18  23:16:47  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:39:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "if_hes_common.h"
#include "../if/if_controller.h"
#include "hes_controller.h"


cdbtype *controller_find_by_unit (int unit)
{
    int slot, slotunit;

    cbus_unit2slotunit(unit, &slot, &slotunit);

    /*
     * VIP - calling controller_find() is stupid, consider using
     * straight unit 
     */
    return(controller_find(slot, slotunit));
}

/*
 * controller_find_removed
 *
 * Walk the removed controller queue, looking for an exact match for the
 * passed information
 */
static cdbtype *controller_find_removed (int unit,
					 int ctrlr_type,
					 enum pif_types_ pif)
{
    cdbtype *cdb;

    for (cdb = (cdbtype *)removed_cdbQ->qhead; cdb != NULL ; cdb = cdb->next) {
	if (cdb->unit == unit &&
	    cdb->ctrlr_type == ctrlr_type &&
	    cdb->pif_type == pif)
	    return(cdb);
    }
    return(NULL);
}

/*
 * controller_reinsert
 *
 * A controller which had previously been removed has been reinsert into
 * the same place.
 */
static void controller_reinsert (cdbtype *cdb)
{
    cdb->cdb_status &= ~(CDB_NOHARDWARE | CDB_CRASHED);
    cdb->input_throttled = FALSE;
    unqueue(removed_cdbQ, cdb);
    enqueue(cdbQ, cdb);
    cdb_list[controller_index_inline(cdb->slot, cdb->appl_no,
				     max_ctrlr_interfaces)] = cdb;
}



/*
 *	Create the controller context
 */
cdbtype *controller_create (int				 ctrlr_type,
			    enum pif_types_		 pif,
			    int 			 unit,
			    platform_hware_handle_type   *hware_handle,
			    boolean		         ok_to_malloc)
{
    cdbtype *cdb;
    
    /*
     *	First question: is there already a cdb out there that matches us?
     *  This typically means that some other card was on-line installed.
     */
    if ((cdb = controller_find_by_unit(unit)) != NULL) {
	if (cdb->ctrlr_type == ctrlr_type && cdb->pif_type == pif) {
	    /* this changes with each cbus_analyze() */
	    cdb->platform_hware_handle = hware_handle;
	    return(cdb);
	} else {
	    /*
	     *	This should never happen.  Infer that a hot-remove/hot-install
	     *  occured within a single interrupt, so remove the old cdb.
	     */
	    controller_remove(cdb);
	}
    }

    /*
     *	Second question: is a cdb already sitting out there for us, with
     *  no hardware?  If it matches us, what we have is a card that was
     *  hot swapped in.
     */
    if ((cdb = controller_find_removed(unit, ctrlr_type, pif)) != NULL) {
	controller_reinsert(cdb);
	/* changes with each cbus_analyze() */
	cdb->platform_hware_handle = hware_handle;
	return(cdb);
    }

    if (ok_to_malloc) {
	cdb = malloc_named(sizeof(cdbtype), "Controller Info");
    } else {
	cdb = dequeue(&cdbpreQ);
	if (cdb != NULL)
	    memset(cdb, 0, sizeof(cdbtype)); /* clear out any stale stuff */
    }

    if (cdb == NULL) {
	return(NULL);
    }

    cdb->platform_hware_handle = hware_handle;
    cdb->ctrlr_type = ctrlr_type;
    cdb->pif_type = pif;
    cdb->unit = unit;

    cbus_unit2slotunit(unit, &cdb->slot, &cdb->appl_no);
        
    cdb->cdb_name = pif_name(pif);
    cdb->cdb_state = IDBS_DOWN;
    cdb->cdb_status = 0;	/* MIP depends on the CDB_CONFIGURE bit
				   being off as the default */

    init_cdb_name(cdb); 
    cdb->love_note = default_love_note_handler;
    controller_init_common(cdb);
    cdb_list[controller_index_inline(cdb->slot, cdb->appl_no,
				     max_ctrlr_interfaces)] = cdb;

    return(cdb);
}













