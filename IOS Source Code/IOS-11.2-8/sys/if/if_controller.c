/* $Id: if_controller.c,v 3.5.12.2 1996/05/06 23:23:31 sdurham Exp $
 * $Source: /release/112/cvs/Xsys/if/if_controller.c,v $
 *------------------------------------------------------------------
 * if_controller.c - Controller functions.
 *             (based on hes/controller.c by Ronnie Kon)
 *
 * Jan 30 1995, Manoj Leelanivas
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_controller.c,v $
 * Revision 3.5.12.2  1996/05/06  23:23:31  sdurham
 * CSCdi54843:  RFC1406 MIB implementation has table indices starting at 0
 * Branch: California_branch
 *      initialize seed value for 4000 as is done for mip.
 *
 * Revision 3.5.12.1  1996/03/18  20:13:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/02/20  00:56:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  08:12:55  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.4.14.1  1996/01/24  22:18:19  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/07  22:41:50  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  09:27:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:51  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:03:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:52:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "media_registry.h"
#include "interface_private.h"
#include "if_controller.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_defs_parser.h"

static queuetype 	cdbQ_;		/* queue of cdb's */
queuetype * const 	cdbQ = &cdbQ_;	/* Pointer to cdb queue */
static queuetype 	removed_cdbQ_;	/* queue of cdb's no longer in system*/
queuetype * const 	removed_cdbQ = &removed_cdbQ_;	/* ptr to above */
cdbtype                 **cdb_list;     /* list of cdbs possible */
uint                    ctrlr_units;    /* # of ctrlr units */
uint             	max_ctrlr_interfaces;

 unsigned int snmp_dsx1LineIndex_seed; /* dump when we support ifIndex */

/*
 * cdb_clear_counters  -
 *
 * Description:
 * clears counters on all cdb data sruct
 *
 * Returns: nothing
 *
 */
static void cdb_clear_counters (hwidbtype *idb)
{
    cdbtype *cdb;

    FOR_ALL_CDBS(cdb) {
	if ((cdb) && (cdb->clear_counts))
	    (*cdb->clear_counts)(cdb);
    }
}

/*
 * set_cdb_txcount -
 *
 * Description:
 * set txcount on those interfaces which have a cdb
 *
 * Returns: nothing
 *
 */
static void set_cdb_txcount (hwidbtype *idb)
{
    cdbtype *cdb;

    cdb = controller_find_from_idb(idb);
    /*
     *  Some interfaces might not be blessed w/ a cdb.
     *  If so, bail.
     */
    if (cdb == NULL) {
	return;
    }
    /*
     *  If there is a cdb, these assigns will make sure the
     *  TX queue threshold is properly displayed on 'sh cont cb'.
     */
    cdb->mci_txcount = idb->mci_txcount; 
    cdb->mci_txcount_enabled = idb->mci_txcount_enabled; 
}


/*
 * controller_init -
 *
 * Description:
 * Initialize controller data structure.
 *
 * Returns: nothing
 *
 */
void controller_init (boolean firsttime,        /* flag for first time */
		      uint max_ctrlr_num,       /* max slots */
		      uint max_ctrlr_ints       /* max applique/slot */)
{
    int index;
    
    if (!firsttime) {
	return;
    }

    /*
     * Init the parser and the cdbQ.
     */
    controller_parser_init();
    queue_init(cdbQ, 0);

    cdb_list = malloc_named(sizeof(cdbtype *) * (max_ctrlr_num + 1)
		      * (max_ctrlr_ints + 1), "Controller List");
    
    for (index = 0 ; index < (max_ctrlr_num + 1) * (max_ctrlr_ints + 1);
	 index++)
	cdb_list[index] = NULL;
    /*
     * Set the max parameters for the platform.
     */
    max_ctrlr_interfaces = max_ctrlr_ints;
    ctrlr_units = 0;

    reg_add_set_cdb_txcount(set_cdb_txcount, "set_cdb_txcount");
    reg_add_cdb_clear_counters(cdb_clear_counters, "cdb_clear_counters");

    /*
     * From RFC 1406
     *
     * This object is the identifier of a DS1  Inter-
     * face on a managed device.  If there is an ifEn-
     * try that is directly associated with  this  and
     * only  this  DS1  interface,  it should have the
     * same value as ifIndex.   Otherwise,  the  value
     * exceeds  ifNumber,  and  is a unique identifier
     * following this rule: inside  interfaces  (e.g.,
     * equipment  side)  with even numbers and outside
     * interfaces  (e.g.,  network   side)   with   odd
     * numbers.
     *
     * until we support an ifIndex mapping we will start
     * numbering for an inside interface at 0x7fff0000
     * this arbitarily limits dsx1 cards to 32000
     * until we implement ifIndex mapping
     *
     * note! - moved from if_mip.c to support rfc1406 in 4000 series
     */
    snmp_dsx1LineIndex_seed = 0x7fff0000; 

}

/*
 * controller_find -
 *
 * Description:
 * Find the appropriate cdb.
 *
 * Returns: ptr to the cdb.
 *
 */
cdbtype *controller_find (int slot, int applique)
{
    return(cdb_list[controller_index_inline(slot, applique,
					   max_ctrlr_interfaces)]);
}


/*
 * controller_command -
 *
 * Description:
 * Check the sanity of the controller config command.
 *
 * Returns: ptr to the cdb.
 *
 */
void controller_command (parseinfo *csb)
{
    cdbtype *cdb;

    if ((cdb = GETOBJ(cdb,1)) == NULL) {
	/*
	 *  They gave us a bogus controller.  Bitch and whine so they
	 *  get it right next time.
	 */
	printf("\n%% No controller specified for controller_command");
	return;
    }

    if (!(cdb->cdb_status & CDB_CONFIGURE)) {
	printf("\n%% Controller does not require configuration");
	return;
    }

    /* NV Generation */
    if (csb->nvgen) {
	nv_write(TRUE, "!");
	nv_write(TRUE, "%s", csb->nv_command);
	if (set_mode_byname(&csb->mode, "controller", MODE_SILENT)) {
	    csb->controller = cdb;
	    *csb->nv_command = '\0';  /* clear out before doing subcommands */
	    push_node(csb, get_mode_nv(csb));
	}
	return;
    }
 
    /* set the top of the parse tree to the controller command set */
    if (csb->sense) {
	if (set_mode_byname(&csb->mode, "controller", MODE_VERBOSE)) {
	    csb->controller = cdb;
	}
    } else {
	printf("\n%% Cannot remove controllers this way");
    }
}

/*
 * controller_remove -
 *
 * Description:
 * A controller has been hot-swapped out.  Light the NOHARDWARE bit and
 * shuffle it onto the removed queue.
 *
 * returns:
 * nothing.
 *
 */
void controller_remove (cdbtype *cdb)
{
    cdb->cdb_status |= CDB_NOHARDWARE;
    unqueue(cdbQ, cdb);
    enqueue(removed_cdbQ, cdb);
    cdb_list[controller_index_inline(cdb->slot, cdb->appl_no,
				     max_ctrlr_interfaces)] = NULL;
}

/*
 * controller_destroy -
 *
 * Description:
 * A controller has been hot-swapped out.  Light the NOHARDWARE bit and
 * shuffle it onto the removed queue.
 *
 * returns:
 * nothing.
 *
 */
void controller_destroy (cdbtype *cdb)
{
    unqueue(cdbQ, cdb);
    cdb_list[controller_index_inline(cdb->slot, cdb->appl_no,
				     max_ctrlr_interfaces)] = NULL;
    free(cdb);
}

/*
 * pif_name -
 *
 * Description:
 * return the name of the controller type.
 *
 * returns:
 * ptr to namestring.
 *
 */
const char * pif_name (enum pif_types_ pif_type)
{
    switch (pif_type) {
    case PIFT_UNKNOWN:	break;
    case PIFT_ETHERNET:	return(IFNAME_ETHER);
    case PIFT_TOKENRING:return(IFNAME_TOKENRING);
    case PIFT_SERIAL:	return(IFNAME_SERIAL);
    case PIFT_HSSI:	return(IFNAME_HSSI);
    case PIFT_FDDI:	return(IFNAME_FDDI);
    case PIFT_T1:	return(IFNAME_T1);
    case PIFT_E1:	return(IFNAME_E1);
    case PIFT_ATM:	return(IFNAME_ATM);
    case PIFT_CHANNEL:  return(IFNAME_CHANNEL);
    case PIFT_FASTETHER:return(IFNAME_FASTETHER);
    }
    return("Unknown");
}


/*
 * print_cdbstate -
 *
 * Description:
 * Print the state of CDB(analogous to the IDB).
 *
 * returns:
 * ptr to namestring.
 *
 */
char *print_cdbstate (enum IDB_STATES state)
{
    switch (state) {
    case IDBS_DOWN:		return("down");
    case IDBS_UP: 		return("up");
    case IDBS_ADMINDOWN:	return("administratively down");
    default:			return("unknown state");
    }
}


/*
 * controller_find_for_SNMP_hw_if -
 *
 * Description:
 * This finds a controller for SNMP.  It needs a pif_type, an index
 * to find, and a next.  If next is FALSE, it will find the exact
 * index, but if it is TRUE, it will find the next index with that
 * pif_type
 *
 * ** side effect ** 
 *
 *      index will contain the value of cdb->hw_if_index
 *      if a cdb was found.
 *
 * returns:
 * ptr to cdb.
 *
 */
cdbtype *controller_find_for_SNMP_hw_if (long *index,
					 enum pif_types_ pif,
					 boolean next)
{
    int maxindex = MAXINT;
    cdbtype *ret = NULL, *cdb;
    
    FOR_ALL_CDBS (cdb) {
	if (pif == cdb->pif_type) {
	    if (next) {
		if (( *index <= cdb->hw_if_index )
		&& ( cdb->hw_if_index < maxindex )) {
		    maxindex = cdb->hw_if_index;
                    *index = cdb->hw_if_index;
		    ret = cdb;
		}
	    } else {
		if ( *index == cdb->hw_if_index )
                    /* no need to modify index for == case */
		    return(cdb);
	    }
	}
    }
    return(ret);
}

/*
 * clear_controller_command  -
 *
 * Description:
 * Call the cdb->reset vector if any..
 *
 * returns:
 * void.
 *
 */
void clear_controller_command (parseinfo *csb)
{
    if (csb->cdb1 != NULL && csb->cdb1->reset != NULL)
	(csb->cdb1->reset)(csb->cdb1);
    else
	printf("\n%% Cannot clear this controller");

}

/*
 * clear_controller_command  -
 *
 * Description:
 * Cut and pasted from description_command()
 * Parse a descriptive name for a controller
 *
 * returns:
 * void.
 *
 */
void controller_description_command (parseinfo *csb)
{
    char *tmp, *tmp2;

    if (csb->controller == NULL)
	return;

    if (csb->nvgen) {
	nv_write((csb->controller->cdb_description != NULL),
	         "%s %s", csb->nv_command, csb->controller->cdb_description);
	return;
    }

    if (!csb->sense) {
	tmp = NULL;
    } else {
	tmp = strdup(GETOBJ(string,1));
	if (tmp == NULL) {
	    printf("\n%% No memory for description");
	    return;
	}
    }
    tmp2 = csb->controller->cdb_description;
    csb->controller->cdb_description = tmp;
    if (tmp2 != NULL) {
	free(tmp2);
    }
}


/*
 * controller_init_common  -
 *
 * Description:
 * Common platform independant initialisation of a cdb.
 *
 * returns:
 * void.
 *
 */
void controller_init_common (cdbtype *cdb)
{
    cdb->cdb_state = IDBS_DOWN;
    cdb->encsize = 0;
    cdb->reset = NULL;
    cdb->idb_init = NULL;
    cdb->clear_counts = NULL;
    enqueue(cdbQ, cdb);
}

/*
 * init_cdb_name
 * Initialize controller name string
 */
void init_cdb_name (cdbtype *cdb)
{
    char buffer[128];

    cdb->cdb_namestring = NULL;		/* setstring does a free() */
    /*
     *	setstring does a malloc.  cdbnamestring_fix() will catch this for
     *  us if we are here after a hot swap
     */
    if (!onintstack()) {
#ifdef SLOT_BASED_CDB_PLATFORM
	extern ctrlr_uses_2bar_notation(int); /* VIP TODO, hack */

	if (!ctrlr_uses_2bar_notation(cdb->ctrlr_type))
	    sprintf(buffer, "%s %d/%d", cdb->cdb_name, cdb->slot,
		    cdb->appl_no);
	else {
	    if (cdb->appl_no < PA_VIRTUAL_SLOT_BOUNDARY)
		sprintf(buffer, "%s %d/0/%d", cdb->cdb_name, cdb->slot,
			cdb->appl_no);
	    else
		sprintf(buffer, "%s %d/1/%d", cdb->cdb_name, cdb->slot, 
			cdb->appl_no - PA_VIRTUAL_SLOT_BOUNDARY);
	}
#else
	sprintf(buffer, "%s %d", cdb->cdb_name, cdb->unit);
#endif	/* slot based platforms */
	setstring(&cdb->cdb_namestring, buffer);
    }
}
