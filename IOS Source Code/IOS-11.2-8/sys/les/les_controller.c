/* $Id: les_controller.c,v 3.5.12.3 1996/08/28 12:59:08 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/les_controller.c,v $
 *------------------------------------------------------------------
 * les_controller.c - controller routines for the low-end.
 *
 * Nov 27 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: les_controller.c,v $
 * Revision 3.5.12.3  1996/08/28  12:59:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.12.2  1996/05/21  09:56:17  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.12.1  1996/03/18  20:42:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/02/20  01:09:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  08:14:55  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.4  1996/01/29  07:28:56  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3.14.1  1996/01/24  22:24:42  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/17  18:32:50  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:41:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:33:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/06/23  18:48:20  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.4  1995/06/19  06:47:58  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/17  00:54:50  jliu
 * CSCdi35899:  Controller didnt come back up after shut/no shut
 *
 * Revision 2.2  1995/06/09  00:24:21  hampton
 * Remove include of deleted file.  [CSCdi35520]
 *
 * Revision 2.1  1995/06/07 21:38:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_public.h"
#include "../parser/parser_actions.h"
#include "if_les.h"
#include "../if/if_controller.h"
#include "les_controller.h"
#include "../if/dsx1.h"
#include "../if/if_msg_controller.c"	/* Not a typo, see logger.h */




/*
 * =====================================================================
 * controller_create  -
 *
 * Description:
 * Platform dependant initialisation of a cdb.
 *
 * returns:
 * ptr to cdb.
 *
 * =====================================================================
 */
cdbtype *controller_create (int			ctrlr_type,
			    enum pif_types_	pif,
			    int 		context /*could be the */
							/*slot #*/
			    )
{
    cdbtype *cdb;
    
    cdb = malloc(sizeof(cdbtype));

    if (cdb == NULL) {
	return(NULL);
    }
    
    /*
     * Appl no and unit are the same. This is to make sure
     * the controller parser chains work correctly. Note that
     * in midrange/low-end, we go only by unit# and not by
     * slot:slotunit convention as in high end. 
     */
    cdb->appl_no = cdb->unit = ctrlr_units++;
    cdb->slot = context;
    cdb->cdb_name = pif_name(pif);
    cdb->ctrlr_type = ctrlr_type;
    cdb->pif_type = pif;

    init_cdb_name(cdb);
    controller_init_common(cdb);

    /*
     * Midrange/low-end boxes go by controller unit#. Hence
     * we do not use slot# to get the controller index.
     */
    cdb_list[controller_index_inline(0, cdb->appl_no,
				     max_ctrlr_interfaces)] = cdb; 
    return(cdb);
}


 /*
  * =====================================================================
  * controller_state_change  -
  *
  * Description:
  * State change on controller and all idbs associate with it
  *
  * returns:
  *
  *
  * =====================================================================
  */
 void controller_state_change (cdbtype  *cdb, enum IDB_STATES newstate)
 {

        errmsg(&msgsym(UPDOWN, CONTROLLER),
                cdb->cdb_namestring, print_cdbstate(newstate));
        cdb->cdb_state = newstate;
        dsx1_change_all_idbs_state(cdb, newstate);
 }





