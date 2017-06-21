/* $Id: ilmi_main.c,v 3.4.26.3 1996/07/10 22:20:41 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atm/ilmi_main.c,v $
 *------------------------------------------------------------------
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_main.c,v $
 * Revision 3.4.26.3  1996/07/10  22:20:41  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.4.26.2  1996/04/25  23:02:45  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.4.26.1.2.1  1996/03/30  03:34:59  rzagst
 * ATM Modularity: Clean up and modularize by platform the ATM parser
 * commands.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.4.26.1  1996/03/18  18:59:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.1  1996/03/07  08:30:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/18  19:52:50  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/05  22:29:55  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI address registration will now ignore the selector byte.
 *
 * Revision 3.2  1995/11/17  08:46:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/26  15:13:20  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.3  1995/06/18  20:31:45  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/16  22:53:02  kartik
 * CSCdi35409:  ILMI doesnt work in some images, works in others
 * Use if_statechange_complete registry to detect hardware reset.
 *
 * Revision 2.1  1995/06/07  20:12:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "address.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "../if/atm.h"
#include "../snmp/snmp_api.h"
#include "../snmp/sr_secure.h"
#include "../snmp/snmp_registry.h"
#include "../ui/debug.h"
#include "../if/atm_debug_flags.h"
#include "ilmi_public.h"
#include "ilmi_private.h"
#include "ilmi_api.h"
#include "../atm/atm_registry.h"

/* ------------------------------------------------------------------------ */


/*
 * G l o b a l  S t o r a g e
 */

queuetype ilmi_requestQ;
queuetype ilmi_responseQ;
queuetype ilmi_inputQ;
queuetype ilmi_addressTabQ;
queuetype ilmi_clientQ;
queuetype ilmi_intfQ;
queuetype ilmi_eventQ;


unsigned short ilmi_type;
ilmiClient client_count;




/* ------------------------------------------------------------------------ 
 * i l m i _ i n i t ()
 *
 * Initializes input, output queues and spawns Input, Output and Timer
 * processes.  This routine is called only once at system startup time.
 */


void 
ilmi_init (void)
{
    static int first_time = TRUE;

    if(!first_time)
        return;
    else {
	    first_time = FALSE;
    }

    
    /* 
     * The request, response and the re-transmit queues. 
     */

    queue_init(&ilmi_requestQ, 0);
    queue_init(&ilmi_responseQ, 0);
    queue_init(&ilmi_inputQ, 0);
    queue_init(&ilmi_addressTabQ, 0);
    queue_init(&ilmi_clientQ, 0);
    queue_init(&ilmi_intfQ, 0);
    queue_init(&ilmi_eventQ, 0);

    /*
     *  What am i ? A switch or a router i.e network-side or 
     *  user-side. Currently we run on the router only so 
     *  initialize it to user-side.
     */

    ilmi_type = ILMI_USERSIDE;		

    /*
     * Initialize Client related. client_id=1 is given to the ILMI itself. 
     */
    client_count = ILMI_ID;

    /*
     * Register to hear about Interfaces coming up.
     */

	 reg_add_if_statechange_complete(ILMI_ChangeState, "ILMI_ChangeState");

	
    /*
     * Register callbacks with the SNMP Engine to receive responses and
     * incoming traps.
     */
    ilmi_snmp_init();
    ilmi_init_polldata();

    /* Initialize Parser */
    ilmi_parser_init();

    init_ilmi_timers();       /* The managed timers. */

    reg_add_atm_ilmi_send_trap(ilmi_send_trap, "ILMI Send Trap");
    reg_add_atm_ilmi_create_event(ilmi_create_event, "ILMI Create Event");
    reg_add_atm_ilmi_destroy_event(ilmi_destroy_event, "ILMI Destroy Event");
    reg_add_atm_ilmi_register_client(ilmi_register_client, "ILMI Register Client");
    reg_add_atm_ilmi_register_service(ilmi_register_service, "ILMI Register Service");
    reg_add_atm_ilmi_client_request(ilmi_client_request, "ILMI Client Request");
    reg_add_atm_ilmi_addrtraverse(ilmi_addrtraverse, "ILMI Address Traverse");
    reg_add_atm_ilmi_unregister_client(ilmi_unregister_client, "ILMI Unregister Client");
    reg_add_atm_ilmi_show_command(ilmi_show_command, "ILMI Show Command");
    
    /* Spawn the ILMI Process */
    (void)cfork(ilmi_Process, 0L, 0, "ILMI Process", 0);
}



/*-------------------------------------------------------------------------
 * i l m i _ p r o c e s s B L O C K () 
 * Scheduler test for the ILMI Process.  If request present on any of the
 * the ilmi_inputQ unblock process, else it remains blocked. It also checks 
 * for the poll.
 */

boolean
ilmi_processBLOCK (void) 
{

    /* 
     * Check Input queue.
     */
    if (!QUEUEEMPTY(&ilmi_inputQ))
	    return(FALSE);

    /* 
     * Check Response queue.
     */
    if (!QUEUEEMPTY(&ilmi_responseQ))
	    return(FALSE);

    return(TRUE);
}


/*-------------------------------------------------------------------------
 * I L M I _  P r o c e s s ()
 */
forktype 
ilmi_Process (void)
{
    ilmi_msgblock *msg;

    /* wait for system to initialize */
    edisms(systeminitBLOCK, 0);

    for (;;) {

	    edisms((blockproc *)ilmi_processBLOCK, 0);

	    /* Process the input queue */
	    while ((msg = dequeue(&ilmi_inputQ)) != NULL) {
	        ilmi_process_input(msg);
	    }

	    /* The Response queue */
	    while ((msg = dequeue(&ilmi_responseQ)) != NULL) {
	        ilmi_process_response(msg);
	    }

   }
}


/*-------------------------------------------------------------------------
 * Creates and initializes a interface block and issues a local 
 * restart sequence.
 */

void
ILMI_ChangeState (idbtype *swidb, hwidbtype *hwidb)
{
    ilmi_intfblock *intfblk;
    int ii;

	if (!hwidb || !(hwidb->status & IDB_ATM))
        return;

    if(!(hwidb->hw_if_index))
        return;

    intfblk = ilmi_getintfblk (hwidb->hw_if_index);
    if (hwidb->state == IDBS_ADMINDOWN || hwidb->state == IDBS_DOWN) {
        if(intfblk) {
            ilmi_clear_addrTab(intfblk);
	    ilmi_delete_localdb(intfblk);
            ilmi_destroy_intfblk(intfblk);
        }
        return;
    }
    if(!intfblk) {
        if ((intfblk = malloc(sizeof(ilmi_intfblock))) == NULL) {
            return;
        } else {
            intfblk->idb = hwidb;
            intfblk->curindex = 0 ;
            intfblk->intf_id = hwidb->hw_if_index ;
            intfblk->state = ilmiIntfRestarting;
	    intfblk->initial_set_count = ILMI_MAXREQRETRY;
	    queue_init(&intfblk->localAddressTabQ,0);
	    queue_init(&intfblk->peerAddressTabQ,0);
            for(ii=0; ii<ILMI_MAXPOLLHISTORY; ii++)
                intfblk->history[ii] = FALSE ;

            enqueue(&ilmi_intfQ,intfblk);
        }
    }
    ilmi_clear_addrTab(intfblk);
    ilmi_delete_localdb(intfblk);
    ilmi_process_intfRestart(intfblk);
    return;
}


/*-------------------------------------------------------------------------
 * Periodically issues a restart sequence based on a timer and the state 
 * of the  interface.
 */

void
ilmi_process_intfRestart (ilmi_intfblock *intfblk)
{
    /*
     * Start up the Timer process.
     */
    ilmi_create_event(intfblk->intf_id, ilmiPeriodicAddressReg, 
        ILMI_ADDREG_BACKOFFTIME,0);


    if((!intfblk->idb) || 
        (!intfblk->idb->atm_db))
	return;

    if (!intfblk->idb->atm_db->address_reg) {
	intfblk->state = ilmiIntfUpAndNormal;
	if (atm_ilmi_debug)
	    buginf("\nILMI(%s): Address Registration turned off. Getting general info only from peer");
	
	ilmi_initial_sets(intfblk);
        return;
    }

    if ((ilmi_validate_intf(intfblk)) && (intfblk->state==ilmiIntfRestarting))
        ilmi_initiate_addreg(intfblk) ;
}



/*-------------------------------------------------------------------------
 * SNMP Initialization procedures.
 */

void
ilmi_snmp_initcb (void)
{
    reg_invoke_add_snmp_view("*ilmi","1.3.6.1.2.1.1",INCLUDED,PERMANENT);
    reg_invoke_add_snmp_view("*ilmi","1.3.6.1.4.1.353.2",INCLUDED,PERMANENT);
    reg_invoke_add_snmp_community("ILMI","*ilmi",ILMI_ACCESS_LIST,0xff);
}

void
ilmi_snmp_init (void)
{
    reg_add_snmp_init(ilmi_snmp_initcb,"ilmi_snmp_initcb");
    reg_add_snmp_access_check(ADDR_ILMI,ilmi_access_check,"*ilmi");
    reg_add_process_snmp_management_pdu(ilmi_snmp_callback,"*ilmi");

    return;
}



boolean
ilmi_access_check (int access_list, addrtype *address)
{
    return ((address->type == ADDR_ILMI) && (access_list == ILMI_ACCESS_LIST));
}


      
