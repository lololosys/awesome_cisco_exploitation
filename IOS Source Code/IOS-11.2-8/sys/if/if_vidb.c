/* $Id: if_vidb.c,v 3.3.58.4 1996/08/28 12:48:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/if_vidb.c,v $
 *------------------------------------------------------------------
 * if_vidb.c -- Virtual IDBs
 *
 * 15-March-1991, Joel Bion
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vidb.c,v $
 * Revision 3.3.58.4  1996/08/28  12:48:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.58.3  1996/08/13  02:18:49  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.3.58.2  1996/06/27  09:33:44  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.3.58.1  1996/05/09  14:29:57  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.88.1  1996/04/27  07:03:47  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.78.1  1996/04/08  01:54:37  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.64.1  1996/03/28  17:25:24  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.3  1995/11/17  09:28:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:59  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/16  01:07:19  fred
 * CSCdi40357:  Make WFQ configuration work correctly with Virtual and
 * Null IDBs.
 *
 * Revision 2.2  1995/09/09  00:52:33  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.1  1995/06/07  20:52:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "logger.h"
#include "config.h"
#include "if_vidb.h"
#include "ether.h"
#include "network.h"
#include "msg_datalink.c"		/* Not a typo, see logger.h */
#include "../parser/parser_defs_parser.h"

static queuetype vidb_inuseQ;
static queuetype vidb_freeQ;
static ulong vidb_unit;		/* no need to pollute global namespace */



/*
 * vidb_unused_entry - A routine to call for uninitialized entry points
 */

static boolean vidb_unused_entry (idbtype *idb)
{
    errmsg(&msgsym(BADVCALL, LINK), idb->namestring);
    return(FALSE);
}

static void vidb_unused_setencap_entry (parseinfo *csb)
{
    errmsg(&msgsym(BADVCALL, LINK), csb->interface->namestring);
}


/*
 * vidb_init
 * 
 * Initialize vidb data structures. These queues are created for two possible
 * future uses:
 *    vidb_inuseQ: As vIDBs are not put on the normal IDB queue, scanning
 *                 through this queue allows you to process them. Also used
 *                 by "vidb_free" function to ensure that an IDB freed is
 *                 really, truly, a vIDB.
 *    vidb_freeQ:  Users of VIDBs may want to free them, but other sections
 *                 of code may still be referencing them. The "vidb_freeQ" is
 *                 created for use by future garbage collection routines.
 */
void vidb_init (void)
{
    queue_init(&vidb_inuseQ, 0);
    queue_init(&vidb_freeQ, 0);
    vidb_unit = 1;		/* First, vidb will be numbered as
				   one.  catch unititialized fields...  */
}

/*
 * vidb_malloc
 *
 * Returns pointer to idb. Entity is *not* placed on normal IDB queue, but
 * instead on our special "vidb_inuseQ"
 */

idbtype *vidb_malloc (void)
{
    idbtype *idb;
    hwidbtype *hwidb;
    vidb_list *vidbl;

    hwidb = idb_create();
    if (!hwidb)
	return(NULL);

    /* malloc returns zero'd memory */
    vidbl = malloc(sizeof(vidb_list));
    if (!vidbl) {
	idb_free(hwidb);
        return(NULL);
    }

    idb = hwidb->firstsw;
    hwidb->status = IDB_VIRTUAL;
    hwidb->span_bridge_process_force = SPAN_BR_PROC_VIRTUAL_IDB;
    hwidb->unit = vidb_unit++;
    hwidb->name = IFNAME_VIRTUAL;
    hwidb->hw_namestring = idb->namestring = NULL;
    hwidb->hw_short_namestring = idb->short_namestring = NULL;
    hwidb->state = IDBS_DOWN;
  
    /*
     * Set the required vectors to a routine that will print out a nasty
     * message.
     */

    hwidb->deencap = NULL;
    hwidb->soutput = (soutput_t)vidb_unused_entry;
    hwidb->oqueue = (oqueue_t)vidb_unused_entry;
    hwidb->oqueue_dequeue = (oqueue_dequeue_t)vidb_unused_entry;
    hwidb->iqueue = (iqueue_t)vidb_unused_entry;
    hwidb->reset = (reset_t)vidb_unused_entry;
    hwidb->getlink = (linktype_t)vidb_unused_entry;
    hwidb->extract_hwaddr = invalidate_hwaddr;
    hwidb->extract_snpa = invalidate_snpa;
    hwidb->broadcast = (broadcast_t)return_fuz_false;
    hwidb->setencap = vidb_unused_setencap_entry;
    hwidb->linestate = (linestate_t) return_true;

    hwidb->input_defqcount = INPUTQ_DEFAULT;
    hwidb->keep_period = KEEPALIVE_DEFAULT;
    hwidb->maxmtu = hwidb->maxmtu_def = MAXETHERSIZE; /* maximum MTU */
    hwidb->firstsw->sub_mtu = hwidb->maxmtu_def;

    hwidb->max_pak_size_def = MAXETHERSIZE;
    if_maxdgram_adjust(hwidb, MAXETHERSIZE);
    hwidb->buffersize = MAXETHERSIZE;

    set_default_interface_bandwidth(idb, VISIBLE_VIDB_BANDWIDTH);
    set_default_interface_delay(idb, VIDB_DELAY);      /* 5ms */
    hwidb->reliability = 255;			/* good reliability */
    hwidb->load = 1;				/* poor loading */

    /*
     * Since this interface is created after hardware sizing and the
     * initial startup code, we call the protocol initialization registry
     * specifying that hardware fix up is required (TRUE).
     */
    reg_invoke_sw_idb_init(idb, TRUE);

    vidbl->idb = idb;
    enqueue(&vidb_inuseQ, vidbl);
    return(idb);
}

/*
 * vidb_free
 *
 * "Free" a previously allocated virtual IDB. Does sanity checks to
 * ensure that what is being freed actually *is* a virtual IDB. Actually,
 * this does not free the structure itself, but rather puts it on a free
 * list for possible reuse at some other time.  NEVER FREE IDBS! There
 * are dangling pointers all over the system which will cause horrible
 * crashes if you actually free IDBs.
 */
void vidb_free (idbtype *idb)
{
    leveltype status;
    vidb_list *vidbl;

    status = raise_interrupt_level(ALL_DISABLE);

    /* look for the entry on the free queue */
    for (vidbl = (vidb_list *) (&(vidb_freeQ))->qhead; vidbl;
	 vidbl = vidbl->next)
	if (vidbl->idb == idb)
	    goto done;		/* already free */

    /* look for the entry on the inuse queue */
    for (vidbl = (vidb_list *) (&(vidb_inuseQ))->qhead; vidbl;
	 vidbl = vidbl->next)
	if (vidbl->idb == idb)
	    break;		/* found it */

    /* 
     * we are not responsible for removing the item from the idbQ but
     * we are responsible for proper maintenance of the vidbQs
     */
    if (vidbl) {
	unqueue(&vidb_inuseQ, vidbl);
	enqueue(&vidb_freeQ, vidbl);
    }

 done:
    reset_interrupt_level(status);
    return;
}

/*
 * Find a vidb() by hw_if_index, return NULL if not found
 */
idbtype *find_vidb (ushort portNumber)
{
    vidb_list *vidbl;

    for (vidbl = (vidb_list *) (&(vidb_inuseQ))->qhead; vidbl;
	 vidbl = vidbl->next)
    {
	if (vidbl->idb->hwptr->hw_if_index == portNumber)
	    return(vidbl->idb);
    }
    return(NULL);
}

/*
 * Return next 'free' unit number to be used.
 */
int get_current_vidb_unit (void)
{
    return(vidb_unit);
}

/*
 * Return 'next' vidb in list.  If input is NULL, return head
 * element...  If no next is available, return NULL...
 */
idbtype *get_next_vidb (idbtype *idb)
{
    vidb_list *vidbl;

    if (idb == NULL)
	return((vidb_inuseQ.qhead == NULL) ? NULL :
	       ((vidb_list *) (vidb_inuseQ.qhead))->idb);

    for (vidbl = (vidb_list *) (&(vidb_inuseQ))->qhead; vidbl;
	 vidbl = vidbl->next)
    {
	if (vidbl->idb == idb)
	    return((vidbl->next == NULL) ? NULL :
		   vidbl->next->idb);
    }
    return(NULL);
}

/*
 * Disable span_port on all the Vidbs.
 * Used when "no bridge" - like "no bridge 10 protocol ieee
 */
void span_port_disable_onVidb(spantype *span, int index)
{
    vidb_list *vidbl;

    for (vidbl = (vidb_list *) (&(vidb_inuseQ))->qhead; vidbl;
         vidbl = vidbl->next) {
         if (vidbl->idb->span_index == index)
             reg_invoke_span_port_disable(span, vidbl->idb);
    }
}
 
