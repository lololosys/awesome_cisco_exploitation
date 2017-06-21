/* $Id: atm_ilmi.c,v 3.10.12.5 1996/08/03 23:17:28 fox Exp $
 * $Source: /release/112/cvs/Xsys/atm/atm_ilmi.c,v $
 *------------------------------------------------------------------
 * A T M _ I L M I . C
 *
 * Contains the code necessary for handling the ILMI datagrams to/from
 * the ATM switch.
 *
 * January 1995, Ricky Li Fo Sjoe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_ilmi.c,v $
 * Revision 3.10.12.5  1996/08/03  23:17:28  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.10.12.4  1996/07/10  22:20:00  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.10.12.3  1996/06/28  23:05:08  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.10.12.2  1996/05/21  09:43:00  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.12.1  1996/03/18  18:59:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.12.3  1996/03/13  01:09:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.12.2  1996/02/20  00:18:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/02/13  19:50:10  hampton
 * Remove unnecessary includes.  [CSCdi48844]
 *
 * Revision 3.9  1996/02/01  05:59:34  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.8  1996/01/29  07:26:20  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.7.12.1  1996/01/24  21:17:37  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.7  1995/12/17  18:24:41  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.6  1995/12/06  10:42:35  kartik
 * CSCdi44909:  memory leak in ilmi code
 * ILMI was holding on to the memory it allocated for trap encoding.
 *
 * Revision 3.5  1995/12/05  22:29:45  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI address registration will now ignore the selector byte.
 *
 * Revision 3.4  1995/11/28  22:33:21  rzagst
 * CSCdi39434:  atm->vc_hash[] should be (vc_info_t *) not unsigned
 *
 * Revision 3.3  1995/11/17  08:45:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:07:00  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:59:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/08/18  14:23:03  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.5  1995/07/21  18:04:08  kartik
 * CSCdi37022:  ilmimib causes router crash on a mibwalk.
 *
 * Revision 2.4  1995/06/28  09:22:10  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/20  01:42:47  kartik
 * CSCdi36135:  After modularization ILMI had initialization sequence
 * problem.
 * Changed ILMI to run as KERNEL class.
 *
 * Revision 2.2  1995/06/18  06:17:59  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.1  1995/06/07 20:11:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "../if/network.h"

#include "../if/atm_debug_flags.h"
#include "../if/atm.h"

#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"

#include "../atm/ilmi_public.h"
#include "../atm/ilmi_private.h"
#include "../atm/atm_ilmi.h"
#include "../atm/atm_registry.h"

static watched_queue *atm_lmi_packetQ;       /* LMI packets */


/*
 * atm_ilmi_pak_enqueue: Called to enqueue a packet to the ILMI input Q
 */
static void
atm_ilmi_pak_enqueue(paktype *pak)
{
    process_enqueue_pak(atm_lmi_packetQ, pak);
}

/*
 * atm_ilmi_encap:      Called by ILMI layer to send an ILMI frame out
 * OUTPUT/
 */
boolean
atm_ilmi_encap(paktype *pak)
{
    idbtype     *idb;
    hwidbtype   *hwout;
    atm_db_t    *atm;
    ushort *enclen=NULL, encsize;
    vc_info_t 	*vc;

    idb = pak->if_output;
    hwout = hwidb_or_null(idb);
    if (!atm_interface_up(idb)) {
       if (atm_errors_debug)
          buginf("\nATM(%s): ATM not active yet", hwout->hw_namestring);
       return(FALSE);
    }

    atm = hwout->atm_db;
    vc = atm_getvc(atm, atm->ilmi_vc);
    if (!vc) {
       if (atm_errors_debug)
          buginf("\nATM(%s): ilmi_encap - no ILMI VCD", hwout->hw_namestring);  
	return(FALSE);
    }

    if ((vc->flags & VC_FLAG_AAL_MASK) != ATM_VC_AAL5ILMI) {
       if (atm_errors_debug)
          buginf("\nATM(%s): ilmi_encap - vc %d not ILMI", 
		hwout->hw_namestring, vc->vc);  
       return(FALSE);
    }

    encsize = vc->encapsize - ATM_ENCAP_HDRBYTES;

    encsize += (*atm->funcs.encapsulate)(atm, pak, vc, 0);
    enclen = (ushort *)(pak->datagramstart - sizeof(ushort));
	PUTSHORT(enclen, encsize);

    atm_show_packet(hwout, pak, TRUE);

    atm_out_inc(atm, vc);

    return(TRUE);
}

/*
 * atm_ilmi_periodic(hwidb)
 */
void 
atm_ilmi_periodic(hwidbtype *hwidb)
{
}


/*
 * atm_ilmi_parse:	Parse the ILMI packet and process
 */
static void
atm_ilmi_parse(hwidbtype *hwidb, paktype *pak)
{
    paktype *pakout;
    uint outbuflen,indata_len;
    char *outbuf, *offset;
    addrtype src_addr;;

    src_addr.type = ADDR_ILMI;
    src_addr.length = ADDRLEN_ILMI;
    src_addr.ilmi_addr = hwidb->hw_if_index;

    pakout = getbuffer(snmp_size);
    if(!pakout)
	goto atm_ilmi_parse_cleanup;
	
    outbuf = pakout->datagramstart;
    outbuflen = snmp_size;

    offset = pak->datagramstart + ATM_ENCAP_HDRBYTES;
    indata_len = pak->datagramsize - ATM_ENCAP_HDRBYTES;

    reg_invoke_snmp_engine(&src_addr, offset, indata_len, outbuf, &outbuflen);
    
    /*
     * If outbuflen is > 0 then SNMP is asking us to send a reply back
     * to sender.  Find the ILMI VCD, encaps the SNMP frame & shove it
     * out the door.
     */
    if (outbuflen) {
	pakout->if_output = pak->if_input;
	pakout->datagramsize =  outbuflen;
	if (!atm_ilmi_encap(pakout)) {
	    retbuffer(pakout);
	    goto atm_ilmi_parse_cleanup;
	}
	datagram_out(pakout);
    } else {
	retbuffer(pakout);
    }
    
atm_ilmi_parse_cleanup :
    retbuffer(pak);
    
}


/*
 * atm_ilmi_process
 * handle incoming LMI packets
 */
static process 
atm_ilmi_process (void)
{
    int pak_count;
    hwidbtype *idb;
    paktype *pak;
    
    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    atm_lmi_packetQ = create_watched_queue("ATM ILMI Input", 40, 0);
    process_watch_queue(atm_lmi_packetQ, ENABLE, RECURRING);
    reg_add_atm_ilmi_pak_enqueue(atm_ilmi_pak_enqueue, "ATM ILMI Packet EnQ.");
    atm_ilmi_enabled = TRUE;

    while (TRUE) {
	process_wait_for_event();

	pak_count = PAK_SWITCHCOUNT;

	while (TRUE) {
	    /*
 	     * Process only a limited number of packets per pass
	     */
	    if (pak_count-- == 0) {
		break;
	    }

	    pak = process_dequeue(atm_lmi_packetQ);
	    if (pak == NULL) {
		break;
	    }
	    idb = hwidb_or_null(pak->if_input);
	    if (!idb) {
		buginf("\nATM_LMI_PROCESS(): source idb not set");
		datagram_done(pak);
		continue;
	    }
	    GET_TIMESTAMP(idb->lastinput);    /* remember time of last input */
	    GET_TIMESTAMP(pak->inputtime);    /* save it in packet too */
	    if (idb->state != IDBS_UP) {      /* if hardware not up */
		pak->flags |= PAK_ABORT;
		netinput_enqueue(pak);
		continue;		       	/* and return */
	    }
	    atm_ilmi_parse(idb,pak);
	}
    }
}

/*
 * atm_ilmi_init
 * Initialize the LMI queue
 */
void
atm_ilmi_init (subsystype *subsys)
{
    if(!atm_ilmi_enabled) {
	(void) process_create(atm_ilmi_process, "ATM ILMI Input",
			      NORMAL_STACK, PRIO_HIGH);
	ilmi_init();
    }
}


/*
 *  Given an interface index and a search type, return the atm descriptor
 *  block for it.  If doing a next search, set the index to the index
 *  for which you are returning data.
 */
atm_db_t *atm_find_SNMP (long *index, boolean next)
{
    leveltype sr;
    int atm_index, best_index, best_if;
    hwidbtype *idb; 

    /*
     *  Loop through all the descriptors (indexing with atm_index) to find the
     *  exact or best match.  The best match will have the index saved in
     *  best_index and interface number in best_if.
     */
    best_index = best_if = -1;
    for (atm_index = 0 ; atm_index < MAX_ATM_INTFC ; atm_index++) {
	int cur;
	
	/* hot-swap can change these fields */
	sr = raise_interrupt_level(ALL_DISABLE);
	if (!(atm_db[atm_index].flags & ATM_FLAG_DB_INUSE)) {
	    reset_interrupt_level(sr);
	    continue;
	}
	idb = atm_db[atm_index].hwidb;
	cur = idb->hw_if_index;
	reset_interrupt_level(sr);

	if (cur == *index) {
	    /* an exact match */
	    return(&atm_db[atm_index]);
	}
	/*
	 *  No exact match.  If this is an EXACT search, just continue
	 */
	if (!next)
	    continue;
	
	/*
	 *  If this interface is earlier than the requested one, skip it
	 */
	if (cur < *index)
	    continue;

	/*
	 *  If this is between the target and the best so far, it becomes
	 *  the new best
	 */
	if (best_index == -1 || cur < best_if) {
	    best_if = cur;
	    best_index = atm_index;
	}
    }

    /* if we get here, there is no exact match */
    if ( (next) && (best_if != -1) ) {
		/* settle for the best we could find */
		*index = best_if;
		return(&atm_db[best_index]);
    } else
		return(NULL);
}	



/*
 * This routine does a get or a getnext search for the VC table in
 * the ILMI MIB.
 */

vc_info_t 
*atm_vc_find_SNMP (int port,int vpi,int vci,boolean next)
{
   ilmi_intfblock *intfblk;
   atm_db_t *atm;
   int bucket;
   vc_info_t *vc, *best_vc=NULL;

   intfblk = ilmi_getintfblk(port);

   if(!intfblk || !intfblk->idb)
       return (NULL);

   atm = intfblk->idb->atm_db;
   if(!atm)
       return (NULL);


   best_vc  =  atm_vpivci_to_vc(atm, vpi, vci);
      
   /* Exact search */ 
   if(!next) {
       if (!(best_vc->flags & VC_FLAG_ACTIVE))
	   return (best_vc);
       else
	   return (NULL);
   }
   
   /* Get Next */
   best_vc = NULL;
   for (bucket = 0; bucket < ATM_HASH_DEFAULT ; bucket++) {
       for(vc = atm->vc_hash_tbl[bucket] ;
	   vc != NULL ; vc = vc->next_vc) {

	   if ((vc->vpi < vpi) || ( vc->vpi == vpi && vc->vci <= vci))
	       continue;

	   if (!(vc->flags & VC_FLAG_ACTIVE))
	       continue;

	   if(best_vc == NULL) {
	       best_vc = vc;
	       continue;
	   } else {
	       if ((vc->vpi < best_vc->vpi) ||
		   (vc->vpi == best_vc->vpi && vc->vci <= best_vc->vci) )
		   best_vc = vc;
	       else
		   continue;
	   }
       }
   }
   
   return (best_vc);
}




/*
 * ATM ILMI subsystem header
 */

#define ATMILMI_MAJVERSION 1
#define ATMILMI_MINVERSION 0
#define ATMILMI_EDITVERSION  1

/*
 * Changed this registration to KERNEL from PROTOCOL to ensure it gets
 * initialized for use by the AIP driver (if present).
 */
SUBSYS_HEADER(atmilmi, ATMILMI_MAJVERSION, ATMILMI_MINVERSION, 
		ATMILMI_EDITVERSION,
	    atm_ilmi_init, SUBSYS_CLASS_KERNEL,
	   	"seq: atm",
	   	NULL);
      
