/* $Id: atmsig_api.c,v 3.6.12.16 1996/09/06 23:13:30 schrupp Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_api.c,v $
 *------------------------------------------------------------------
 * A T M S I G _ A P I . C 
 *
 * January 1995,  Percy P. Khabardar 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmsig_api.c,v $
 * Revision 3.6.12.16  1996/09/06  23:13:30  schrupp
 * CSCdi66036:  ATMSIG: Add Called/Calling Party Addr to debug
 * Branch: California_branch
 *
 * Revision 3.6.12.15  1996/09/04  05:11:02  schrupp
 * CSCdi67982:  ATMSIG: Dont suspend prematurely
 * Branch: California_branch
 *
 * Revision 3.6.12.14  1996/08/28  12:37:17  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.12.13  1996/07/10  22:20:11  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.6.12.12  1996/06/18  20:45:02  schrupp
 * CSCdi60667:  Exception: Illegal Instruction at 0x0, caused by no atm
 * esi-address
 * Branch: California_branch
 *
 * Revision 3.6.12.11  1996/06/08  20:13:11  cakyol
 * CSCdi59964:  a sig routine needs to return a vcd_t instead of an int
 * Branch: California_branch
 *
 * Revision 3.6.12.10  1996/06/03  23:09:13  schrupp
 * CSCdi59328:  ATMSMAP: Request prefix from ILMI upon prefix deletion
 * Branch: California_branch
 *
 * Revision 3.6.12.9  1996/05/24  21:08:37  schrupp
 * CSCdi58635:  ATM: ESI cleanup
 * Branch: California_branch
 *
 *  - Deregister addresses when a new prefix is added by ILMI
 *  - Deregister addresses when ESI/NSAP is changed from command line
 *  - Reregister addresses even if the new prefix is the same as the old
 *  - Deregister addresses when subinterface is shut
 *  - Reregister addresses when subinterface is no shutD
 *  - Don't allow qsaal and ilmi pvc's on subinterfaces
 *  - Init atm_db->peeruni_ifname
 *
 * Revision 3.6.12.8  1996/05/09  14:11:01  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.12.7  1996/05/08  02:43:49  grobelch
 * - Create system list registry functions invoked by static-map software
 *   when:
 *     a static-map is deleted, a map-group is deleted, and when a
 *     static-map is updated. A provider of static-map service (such
 *     as ATM signalling) can register for this, check for one of it's
 *     map's being the subject of the call, and act appropriately.
 *   - Provide ATM signalling routines for the static-map and group
 *     registries. Remove ARP table entries for IP and free up
 *     ATM signalling-specific data structures.
 *   - Improve vc "glomming" for point-to-point VCs to take in
 *     AAL5MUX case.
 *   - Fix mistaken use of an enumeration as a bit-value.
 * CSCdi53027:  ATMSIG: SVC static maps not updated properly in all cases
 * Branch: California_branch
 *
 * Revision 3.6.12.6  1996/04/29  21:08:37  rzagst
 * CSCdi55300:  Invalid vcd of 65535 causes FAIL_CREATEVC error
 * Branch: California_branch
 *
 * Revision 3.6.12.5.2.3  1996/05/03  21:40:40  kjadams
 * CSCdi56624:  Incorrect BLLI for Token Ring LANE Data Direct VCC
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * back out last change - confine fix to /lane.
 *
 * Revision 3.6.12.5.2.2  1996/05/03  18:27:29  kjadams
 * CSCdi56624:  Incorrect BLLI for Token Ring LANE Data Direct VCC
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * 2 additional blli's for tr lane.  Bump max_blli by 2 and initialize
 * the extra 2.
 *
 * Revision 3.6.12.5.2.1  1996/04/27  06:33:25  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.12.5  1996/04/25  23:02:08  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.6.12.4  1996/04/03  04:51:38  schrupp
 * CSCdi53125:  ATM signalling leaks memory
 * Branch: California_branch
 * Use malloc_named for ATMSIG to readily identify possible memory culprits.
 *
 * Revision 3.6.12.3.4.3  1996/04/24  21:40:13  rbadri
 * CSCdi55597:  SDU sizes in signalling do not follow sub-interface MTU
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.6.12.3.4.2  1996/04/10  23:35:28  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.6.12.3.4.1  1996/04/08  01:44:22  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.12.3  1996/03/29  23:42:14  schrupp
 * CSCdi53085:  ATMSIG  11.0, 11.1 bugfixes for 11.2
 * Branch: California_branch
 * Bugfixes from 11.0, 11.1 into 11.2
 *
 *  CSCdi35689 shut down an ATM ARP
 *  CSCdi46600 Concurrent 1577 and LANE crashes in ATMSIG upon reboot
 *  CSCdi47523 Crash in ATM Signalling code
 *  CSCdi48594 %SUBSYS-2-NOTFOUND: atmsig needs atmilmi on boothelper
 *  images
 *  CSCdi49728 bus error on 7000 running ATM,ATALK,OSPF,SNMP,IPX with 11.0
 *  CSCdi50591 ATMSIG:  release complete uses invalid cause value 80h
 *  CSCdi51037 IBM 25 Mbits ATM-ARP connectivity pb / rfc1755 support
 *  CSCdi52025 router crash at atmSig_callPresentState
 *
 * Revision 3.6.12.2  1996/03/29  19:27:43  grobelch
 *   Provide traffic shaping for SVCs when possible.
 *   Also:
 *   - Specify map-class rate parameters in kbps.
 *   - Provide a per-interface command to indicate
 *     whether SVC should be established if signalled
 *     parameters cannot be installed.
 *   - Don't attach ATM submaps to map-class block
 *   - Check for map-class params prior to using
 *     them (they can be null).
 *   - Fix help-strings for "clp1" parameters to
 *     indicate for clp0&1 flows.
 *   - Signal best effort if only pcr for clp0&1 are
 *     specified.
 *   - map-class parameters override map-list where
 *     they overlap.
 *   - Fix a couple of erroneous variable setting/usages
 *     in static-map handling.
 * CSCdi51550:  ATMSIG: SVCs should attempt to use traffic shaping
 * Branch: California_branch
 *
 * Revision 3.6.12.1  1996/03/18  18:59:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.16.2  1996/03/25  02:18:16  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6.16.1  1996/03/22  09:35:01  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.4.2  1996/03/11  23:56:45  cakyol
 * Put the svc validation check into atmSig_api_sameCallingPartyAddress.
 * Maybe we are not quite yet ready to trust the passed in vc.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5.4.1  1996/02/27  20:40:31  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/02/01  05:59:43  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/23  16:22:18  schrupp
 * CSCdi47031:  DROP_PARTY request to atmsig does not do DROP_PARTY
 *
 * Revision 3.3.12.5  1996/01/31  22:20:33  cakyol
 * - use bcopy to assign a structure rather than direct
 *   assignment.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.4  1996/01/30  23:55:59  cakyol
 * - last touches on the 3 additional atm sig api
 *   routines as recommended by Schrupp.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.3  1996/01/29  22:05:41  cakyol
 * - as per the signalling group's request, eliminate the
 *   direct invasion of their private data structures by
 *   using:
 *     atmSig_callId2CallingPartyAddress   and
 *     atmSig_sameCallingPartyAddress
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.2  1996/01/26  02:01:51  cakyol
 * more code review mods (still more to come):
 *
 * - "show lane config" now shows all the details of the
 *   connected LESs, such as elan name, relative priority
 *   and whether active or not.
 * - in the lecs finder, protect against getnext going past
 *   the end of the lecs addresses table, by checking the
 *   incoming oid every time.
 * - in the lecs finder, add a parameter to the user interface
 *   limiting the number of lecs addresses the caller is
 *   interested in.
 * - use a static global buffer for debugging messages; saves
 *   stack space.
 * - make "les-timeout" and "lane global-lecs-address" commands
 *   hidden.
 * - prepend "atmSig_" to the 2 new atm signalling api function
 *   names.
 * - prepend "lsv_" to some LES functions for consistency.
 * - more white space, if...else formatting cleanup in some
 *   of the files.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.1  1996/01/11  01:44:34  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.8.1  1995/12/29  01:05:25  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/22  20:44:27  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.2  1995/11/17  08:46:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.15  1995/10/03  23:40:39  schrupp
 * CSCdi40453:  ATM SDU size should be MTU + LLC/SNAP in Setup Msg
 *
 * Revision 2.14  1995/09/28  21:13:03  ronnie
 * CSCdi40913:  More alignment errors with RFC 1577
 *
 * Revision 2.13  1995/09/28  20:49:10  ronnie
 * CSCdi40510:  Too many ATM SVCs set up to same destination
 *              Also added some debugging to the ATM SigAPI
 *
 * Revision 2.12  1995/09/05  15:52:51  schrupp
 * CSCdi39792:  Check ATM nsap_ptr before cancelling NSAP address
 *
 * Revision 2.11  1995/09/01  17:48:21  percyk
 * CSCdi39722:  set sdu sizes per connection - part 2
 *
 * Revision 2.10  1995/08/23  17:15:39  schrupp
 * CSCdi38583:  memory leak after almost 100 retries of Create VC Failure
 *
 * Revision 2.9  1995/08/18  14:23:08  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.8  1995/08/11  20:58:24  ronnie
 * CSCdi38022:  ATM ARP client needs to check for valid NSAP before
 * creating map
 *
 * Revision 2.7  1995/08/11  19:46:47  schrupp
 * CSCdi38620:  Prohibit SVC opens until ATMSIG processes active
 *
 * Revision 2.6  1995/08/08  05:33:26  schrupp
 * CSCdi38341:  Dont cancel ATM NSAP if never registered
 *
 * Revision 2.5  1995/08/01  00:23:29  schrupp
 * CSCdi37269:  memory leak after Create VC failed, vpi/vci already in use
 * Fix the memory leak for setup and add party failure. Notify client for
 * add party failure.
 *
 * Revision 2.4  1995/07/26  15:13:12  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.3  1995/06/26  01:52:53  percyk
 * CSCdi35137:  BLLI in incoming CONNECT is garbage
 *
 * Revision 2.2  1995/06/18  20:31:40  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:11:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../atm/msg_atm.c"		/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "interface_private.h"
#include "address.h"
#include "../os/free.h"
#include "../if/atm.h"
#include "atmsig_api.h"
#include "packet.h"
#include "atmsig_private.h"


/*
 * a t m S i g _ h a n d l e U s e r R e q u e s t  ( )
 * 
 * Take a copy of user request and stuff it into the queue.  Used
 * by Lan Emulation. 
 */
boolean atmSig_handleUserRequest (sig_api_msg *userRequest)
{

   sig_api_msg *msgp;

 
   /* allocate a temp msg structure and copy users request to it */
   msgp = malloc( sizeof (sig_api_msg) );

   if (msgp == NULL) {
     return (FALSE);
   }

   *msgp = *userRequest;

   /* MUST be set to NULL BEFORE the enqueue */
   msgp->link = NULL;
 
   /*
    * put it on to signalling request queue.  When it is dequeued
    * and processed, it will be freed
    */
   process_enqueue(atmSig_outputQ, msgp);
 
   /* malloc went ok, does NOT mean the request succeeded */
   return (TRUE);

}



/*
 * a t m S i g _ a p i _ a l l o c a t e _ c a l l _ i d ( )
 * 
 * When a Signalling client wishes to initiate a connection via a Setup 
 * call, it must first call this function to allocate a "call_id". Then
 * the Setup request header _must_ contain this call_id.
 *
 * Subsequent callback messages (from Signalling driver to it's clients) 
 * will have the same call_id, thus allowing the signalling client to 
 * match indications to a particular call.
 *
 * A return value of 0 means that a call_id could not be allocated (no
 * memory).
 *
 * In this implementation, the call_id is actually a svc struct pointer.
 * The svc struct is allocated in this function by the Signalling driver.  
 * However for the Signalling client the call_id is simply a (void *)32
 * bit quantity, a magic cookie.
 * Every call requires at least 1 party block. Allocate that here.
 */
void *atmSig_api_allocate_call_id (void)
{

   atmSvcType *svc;
   atmSvcPartyType *party; 


   /* malloc a new svc block */
   svc = atmSig_allocateSvcBlock();

   if (!svc) {
      return (NULL);
   }

   /* need to allocate party block also */
   party = atmSig_allocatePartyBlock();

   if (!party) {
     free(svc);
     return (NULL);
   }

   svc->rootParty = party;
   party->svc     = svc;

   return (svc);

}

static char *
sigapi_msg_decode (sig_api_opcode op)
{
    switch(op) {
    case ATM_LISTEN_NSAP:	return "LISTEN_NSAP";
    case ATM_CANCEL_NSAP:	return "CANCEL_NSAP";
    case ATM_SETUP:		return "SETUP";
    case ATM_CONNECT:		return "CONNECT";
    case ATM_CONNECT_ACK:	return "CONNECT_ACK";
    case ATM_RELEASE:		return "RELEASE";
    case ATM_RELEASE_COMPLETE:	return "RELEASE_COMP";
    case ATM_STATUS_ENQUIRY:	return "STATUS_ENQ";
    case ATM_STATUS:		return "STATUS";
    case ATM_ADD_PARTY:		return "ADD_PARTY";
    case ATM_ADD_PARTY_ACK:	return "ADD_PARTY_ACK";
    case ATM_ADD_PARTY_REJ:	return "ADD_PARTY_REJ";
    case ATM_DROP_PARTY:	return "DROP_PARTY";
    case ATM_DROP_PARTY_ACK:	return "DROP_PARTY_ACK";
    }
    return "<Unknown Op Code>";
}


/*
 * a t m S i g _ o u t p u t ( )
 *
 * This function decodes messages from client's on Signalling driver's 
 * output queue and hands it off to the right functions for processing.
 */
void atmSig_output (sig_api_msg *msg)
{

   atmSvcType *svc;
   atmSvcPartyType *party = FALSE;
   boolean partyMsg = FALSE;

   /* sanity check */
   if (!msg) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: output - null msg");
      return;
   }

   if (atmSig_debugAll || atmSig_debugEvent) {
       blocktype *block;

       buginf("\nATMAPI: %s ", sigapi_msg_decode(msg->opcode));
       
       block = data_to_block((char *)msg);
       if (block->blockmagic == BLOCKMAGIC) {
	   buginf("from %s (PC 0x%x)",
		  block->allocator_name == NULL ? "<unspecified process>"
	                                        : block->allocator_name,
		  block->allocator_pc);
       } else {
	   /*
	    *  This is probably not an error.  This is probably just a
	    *  sig_api_msg which was allocated on the stack instead of
	    *  being malloc()'ed
	    */
	   ;
       }
   }

   svc = NULL;

   /* dispatch request to appropriate function based on msg opcode */
   switch (msg->opcode) {

      case ATM_LISTEN_NSAP:
         atmSig_api_listen_nsap(msg);
         break;

      case ATM_CANCEL_NSAP:
         atmSig_api_cancel_nsap(msg);
         break;

      case ATM_SETUP:
         svc = atmSig_api_setup_req(msg);
         break;

      case ATM_CONNECT:
         svc = atmSig_api_connect_req(msg);
         break;

      case ATM_CONNECT_ACK:
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMAPI: client should not send Connect Ack req");
         break;

      case ATM_RELEASE:
         svc = atmSig_api_release_req(msg);
         break;

      case ATM_RELEASE_COMPLETE:
         svc = atmSig_api_release_complete_req(msg);
         break;

      case ATM_ADD_PARTY:
         svc = atmSig_api_add_party_req(msg);
	 party = (atmSvcPartyType *)msg->u.add_party.endpoint_id;
	 partyMsg = TRUE;
         break;

      case ATM_ADD_PARTY_ACK:
         svc = atmSig_api_add_party_ack_req(msg);
	 party = (atmSvcPartyType *)msg->u.add_party_ack.endpoint_id;
	 partyMsg = TRUE;
         break;

      case ATM_ADD_PARTY_REJ:
         svc = atmSig_api_add_party_rej_req(msg);
	 party = (atmSvcPartyType *)msg->u.add_party_rej.endpoint_id;
	 partyMsg = TRUE;
         break;

      case ATM_DROP_PARTY:
         svc = atmSig_api_drop_party_req(msg);
	 party = (atmSvcPartyType *)msg->u.drop_party.endpoint_id;
	 partyMsg = TRUE;
         break;

      case ATM_DROP_PARTY_ACK:
         svc = atmSig_api_drop_party_ack_req(msg);
	 party = (atmSvcPartyType *)msg->u.drop_party_ack.endpoint_id;
	 partyMsg = TRUE;
         break;

      default:
         if (atmSig_debugAll || atmSig_debugError) 
            buginf("\nATMAPI: invalid request opcode = %x!", msg->opcode);
         break;

   }

   /* test for PTP or MTP, feed event to the Q.2931 state machine */
   if (svc) {
     if (!partyMsg) {
       atmSig_stateTable(svc);
     }
     else {
       atmSig_multiPointStateTable(svc, party);
     }
   }

   /* free memory block (sig client's request) */
   free(msg);

   /* 
    * localize freeing of svc's and party's (don't free them from 
    * everywhere) 
    */
   if (party && party->freeParty)
      atmSig_freePartyBlock(svc, party);

   if (svc && svc->freeSvc)
      atmSig_freeSvcBlock(svc);

}



/*
 *  a t m S i g _ a p i _ l i s t e n _ n s a p ( )
 *
 * The Signalling client registers an ATM address that it wishes to listen
 * on using this function.
 *
 * If the NSAP has already been logged, LISTEN_NSAP request will be ignored.
 *
 * For certain applications (such as switch CAC) the Signalling client may
 * want to listen to _all_ NSAPS.  In that case, it must register the 
 * promiscuous NSAP (all ones).  If promiscuous NSAP is registered on an
 * interface, then no further LISTEN_NSAP's (for individual NSAPs) will be 
 * allowed.
 *
 */
void atmSig_api_listen_nsap (sig_api_msg *msg)
{

   int i;
   idbtype *idb;
   atmSig_nsap_info *nsap_ptr;
   boolean promiscuous_nsap;
   sig_api_error_code result;
   sig_api_msg listen;
   char nsapstr[MAX_ATM_NSAP_ASCII_LEN];
   

   /* get idb for which this ATM address is acceptable */
   idb = msg->idb;

   /* Check sig_user_callback. No client callback is a sure crash.
    * Catch it now
    */
   if (!msg->u.listen.sig_user_callback) {
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMAPI: no sig_user_callback, listen failed");
     return;
   }

   /* ATM port? */
   if ( !idb || !(idb->hwptr->status & IDB_ATM) ) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMAPI: listen_nsap: invalid ATM port, listen failed");
      result = ATM_SIGAPI_FAIL;
      goto listen_result;
   }

   /* If promiscuous NSAP already logged, then reject LISTEN request */
   if (idb->hwptr->atm_db->promiscuous_nsap == TRUE) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMAPI: promiscuous NSAP already registered, listen failed");
      result = ATM_SIGAPI_FAIL;
      goto listen_result; 
   }

   /*
    * promiscuous NSAP (all ones)?.  If so, then no other NSAP should have
    * been logged.  All incoming Setup messages are handed to the same
    * client.
    */
   promiscuous_nsap = TRUE;

   for (i = 0; i < ATM_NSAP_ADDR_LNGTH; ++i) {
      if (msg->u.listen.nsap.addr[i] != 0x01) {
         promiscuous_nsap = FALSE;
         break;
      }
   }

   if (promiscuous_nsap) {
      /* make sure no other NSAPs logged */
      if (idb->hwptr->atm_db->nsap_list_head != NULL) {
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMAPI: listen_nsap: cannot LISTEN on promiscuous NSAP");
         result = ATM_SIGAPI_FAIL;
         goto listen_result; 
      }

      idb->hwptr->atm_db->promiscuous_nsap = TRUE;
      idb->hwptr->atm_db->promiscuous_sig_user_callback = 
                                            msg->u.listen.sig_user_callback;

      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMAPI: listen_nsap: call succeeded");
      
      result = ATM_SIGAPI_OK;
      goto listen_result; 
   }

   /* get pointer to start of nsap_info linked list */
   nsap_ptr = idb->hwptr->atm_db->nsap_list_head;

   /* make sure that this NSAP is not already logged. if the nsap is from
    * the NSAP command, there shouldn't be an nsap_ptr yet. if the nsap
    * is from ESI command, then there will be an nsap_ptr already. if ESI, 
    * we had to wait until ILMI ok's the full nsap before we attempt to 
    * register. 
    * use !nsap_ptr->useIlmiPrefix to detect already registered from NSAP CMD.
    * use nsap_ptr->ilmiAddrRegistered to detect registered from ESI CMD.
    */
   while (nsap_ptr) {
     if ( bcmp(nsap_ptr->nsap.addr, msg->u.listen.nsap.addr, 
                                         ATM_NSAP_ADDR_LNGTH) == 0 ) {
       if ( (!nsap_ptr->useIlmiPrefix) || (nsap_ptr->ilmiAddrRegistered) ) {
	 /* This NSAP has already been registered, return error */
	 if (atmSig_debugAll || atmSig_debugError)
	   buginf( "\nATMAPI: NSAP already registered, listen failed ");
	 result = ATM_SIGAPI_FAIL;
	 goto listen_result;
       }
       else {
	 /* address match in atm_db, but not yet registered, register nsap */
	 goto register_nsap;
	}
     }
      else 
         nsap_ptr = nsap_ptr->next;
   }

   /* alloc an nsap struct and fill in info */
   nsap_ptr = malloc_named( sizeof(atmSig_nsap_info), "ATMSIG-NSAP");

   if (nsap_ptr == NULL) {
      /* no memory, can't register this nsap */
     result = ATM_SIGAPI_FAIL;
     goto listen_result; 
   }

   bcopy(&msg->u.listen.nsap.addr, &nsap_ptr->nsap.addr, ATM_NSAP_ADDR_LNGTH); 

   nsap_ptr->idb = idb;
   nsap_ptr->next = idb->hwptr->atm_db->nsap_list_head;
   idb->hwptr->atm_db->nsap_list_head = nsap_ptr;

register_nsap:
   nsap_ptr->sig_user_callback = msg->u.listen.sig_user_callback;
   if (nsap_ptr->useIlmiPrefix)
     nsap_ptr->ilmiAddrRegistered = TRUE;

   if (atmSig_debugAll || atmSig_debugEvent) {
     buginf("\nATMAPI: listen_nsap: address registered -\n");
     atm_printnsap(nsap_ptr->nsap.addr, nsapstr);
     buginf("%s", nsapstr);
   }

   result = ATM_SIGAPI_OK;

listen_result:
   listen.opcode               = ATM_LISTEN_NSAP;
   listen.idb                  = idb;
   listen.u.listen.result      = result;
   listen.u.listen.user_handle = msg->u.listen.user_handle;
   bcopy(&msg->u.listen.nsap, &listen.u.listen.nsap, sizeof(hwaddrtype));

   /* inform result to client */
   (*msg->u.listen.sig_user_callback)(&listen);
     

}



/*
 * a t m S i g _ a p i _ c a n c e l _ n s a p ( )
 *
 * Signalling clients can cancel a previously registered NSAP using
 * this function.
 */
void atmSig_api_cancel_nsap (sig_api_msg *msg)
{

   atmSig_nsap_info *nsap_ptr, **prev;
   int i;
   boolean promiscuous_nsap;
   idbtype *idb;
   sig_api_error_code result;
   sig_api_msg cancel;


   nsap_ptr = NULL;

   /* get idb for which this addr is to be cancelled */
   idb = msg->idb;

   /* ATM idb set? */
   if ( !idb || !(idb->hwptr->status & IDB_ATM) ) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMAPI: cancel_nsap: invalid ATM port");
      result = ATM_SIGAPI_FAIL;
      goto cancel_result;
   }
 
   /* promiscuous NSAP? (all ones) */
   promiscuous_nsap = TRUE;
   for (i = 0; i < ATM_NSAP_ADDR_LNGTH; ++i) {
      if (msg->u.cancel.nsap.addr[i] != 0x01) {
         promiscuous_nsap = FALSE;    
         break; 
      } 
   }
 
   if (promiscuous_nsap) {
 
      /* make sure no other NSAPs logged */ 
      if (idb->hwptr->atm_db->promiscuous_nsap == FALSE) { 
         if (atmSig_debugAll || atmSig_debugError)
            buginf("\nATMAPI: cancel_nsap failed: no promiscuous nsap");
         result = ATM_SIGAPI_FAIL;
         goto cancel_result; 
      }
  
      idb->hwptr->atm_db->promiscuous_nsap = FALSE; 
      idb->hwptr->atm_db->promiscuous_sig_user_callback = 
                                            ( void(*)(void *) )NULL;

      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMAPI: cancel_nsap(): call succeeded");

      result = ATM_SIGAPI_OK;
      goto cancel_result; 

   }

   /* get pointer to start of nsap_info linked list */
   prev = &(idb->hwptr->atm_db->nsap_list_head);
   nsap_ptr = *prev;
 
   /* find the NSAP to be CANCELled, and delete from NSAP list */ 
   while (nsap_ptr) {

     if ( (bcmp(nsap_ptr->nsap.addr, msg->u.cancel.nsap.addr, 
		ATM_NSAP_ADDR_LNGTH) == 0) && 
	 (nsap_ptr->idb == idb) ) {
	
         /* nsap found */
         *prev = nsap_ptr->next;
         if (atmSig_debugAll || atmSig_debugEvent)
            buginf("\nATMAPI: cancel_nsap: call succeeded");

         result = ATM_SIGAPI_OK;
         goto cancel_result;
          
      }

      prev = &(nsap_ptr->next);
      nsap_ptr = nsap_ptr->next;

   }

   /* addr not found */
   if (atmSig_debugAll || atmSig_debugEvent) 
      buginf("\nATMAPI: cancel_nsap: nsap not registered, call failed");

   result = ATM_SIGAPI_FAIL;


cancel_result:

   /* If no nsap_ptr, return here */
   if (!nsap_ptr)
     return;

   /* If no callback yet, return here */
   if (!nsap_ptr->sig_user_callback) {
     free(nsap_ptr); 
     return;
   }

      cancel.opcode               = ATM_CANCEL_NSAP;
      cancel.idb                  = idb;
      cancel.u.cancel.result      = result;
      cancel.u.cancel.user_handle = msg->u.cancel.user_handle;
      bcopy(&msg->u.cancel.nsap, &cancel.u.cancel.nsap, sizeof(hwaddrtype));

      /* inform result to client */
      (*nsap_ptr->sig_user_callback)(&cancel);
      free(nsap_ptr); 
 
}



/*
 *         R  E  Q  U  E  S  T       F  U  N  C  T  I  O  N  S
 *
 * Request functions from clients to the Signalling Driver.  The requests 
 * are queued on ATM Signalling driver's output queue.
 * 
 */ 
/*
 *  a t m S i g _ a p i _ s e t u p _ r e q ( )
 * 
 * The client's Setup message to the Signalling driver results in this
 * function being called.
 *
 * This function merely translates the client's message to the svc 
 * and calls the Signalling driver to actually send the Setup msg.
 */
atmSvcType *atmSig_api_setup_req (sig_api_msg *msg)
{

   atm_db_t *atm;
   atmSvcType *svc;
   atm_params_type *params;
   vc_info_t *vc;
   atmSvcPartyType *party;
   sig_api_msg msg1;
   char nsapstr[MAX_ATM_NSAP_ASCII_LEN];

   /*
    * the call-id field in the msg is actually a pointer to svc struct 
    * that sig allocated when atmSig_api_allocate_call_id() was called by 
    * client 
    */ 
   svc = (atmSvcType *)msg->u.setup.call_id;
 
   /* valid svc block? */
   if (!svc) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMAPI: invalid svc block, setup svc req failed");
      return (NULL);
   }

   /* Check sig_user_callback. No client callback is a sure crash.
    * Catch it now
    */
   svc->sig_user_callback = msg->u.setup.sig_user_callback;
   if (!svc->sig_user_callback) {
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMAPI: no sig_user_callback, setup svc req failed");
     return (NULL);
   }

   /* connection request already pending? */
   if (svc->state != ATMSIG_ST_NULL) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: setup_req: svc %x already in use", svc);
      return (NULL);
   }

   party = svc->rootParty;

   if (!party) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG_API: no party block, setup svc req failed");
      return (NULL);
   }

   svc->idb = msg->idb;

   if (!svc->idb) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: no svc->idb, open req failed");
      return (NULL);
   }

   /* KEEP THIS HERE
    * Nsap info must be filled into party block in case the setup_req
    * fails. The static map (or other) client needs it in order to clean
    * up when receiving release complete.
    */
   /* save destination atm length & address in svc block */
   party->destNsap.type = STATION_ATMNSAP;
   party->destNsap.length = msg->u.setup.called_party.length;
   bcopy(msg->u.setup.called_party.addr, party->destNsap.addr,
	 party->destNsap.length); 

   /* save source atm length & address in svc block */
   svc->srcNsap.length = msg->u.setup.calling_party.length;
   bcopy(msg->u.setup.calling_party.addr, svc->srcNsap.addr, 
	 svc->srcNsap.length); 

   atm = svc->idb->hwptr->atm_db;

   /* limit # of local connections req to max locally open req config'd */
   if (atm->conn_pending >= atm->max_conn_pending) {
     if (atmSig_debugAll || atmSig_debugEvent) 
       buginf("\nATMSIG: %d connections pending, open req failed, %x",
	      atm->conn_pending, svc);
     goto setup_req_failed;
    }

   if (!atm_interface_up(svc->idb)) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: output port invalid/not-active, open req failed");
      goto setup_req_failed;
   }

   /* signalling vc configured? */
   if (!svc->idb->hwptr->atm_db->sig_vcnum) { 
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: sig vc not configured, open req failed");
      goto setup_req_failed;
   }

   /* save signalling idb in svc block */
   vc = atm_getvc(atm, atm->sig_vcnum);
   if (!vc) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: sig vc info blk not found, open req failed");
      goto setup_req_failed;
   }

   svc->sig_idb = vc->swidb;

   /* save user vc type */
   svc->user_vc_type = msg->u.setup.user_vc_type;

   /* get (TOS) params block */
   params = &msg->u.setup.params;

   /* copy Type of Service parameters into svc */
   atmSig_fillTosInSvc(svc, params);

   /* fill in BLL-IE info - max of 3 currently supported */

   svc->blli[0].length = msg->u.setup.blli[0].length;
   bcopy(msg->u.setup.blli[0].string, svc->blli[0].string,
	 svc->blli[0].length); 

   svc->blli[1].length = msg->u.setup.blli[1].length;
   bcopy(msg->u.setup.blli[1].string, svc->blli[1].string,
	 svc->blli[1].length); 

   svc->blli[2].length = msg->u.setup.blli[2].length;
   bcopy(msg->u.setup.blli[2].string, svc->blli[2].string,
	 svc->blli[2].length); 

   svc->flags = msg->u.setup.flags;

   /*
    * fwd sdu size for this connection 
    * if client specifies invalid mtu size, 
    *  if MUX, default to sub_interface mtu size
    *  else default to sub_interface mtu size + ATM_SNAP_HDRBYTES
    */
   if ( (msg->u.setup.fwd_sdu_size > 0) &&
                     (msg->u.setup.fwd_sdu_size <= ATMSIG_MAX_MTU_SIZE) )
      svc->tos.fwdSduSize = msg->u.setup.fwd_sdu_size;
   else {

     if ((svc->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL5MUX) 
       svc->tos.fwdSduSize = svc->idb->sub_mtu;
     else
       svc->tos.fwdSduSize = svc->idb->sub_mtu + ATM_SNAP_HDRBYTES;
   }

   /*
    * bwd sdu size for this connection 
    * if client specifies invalid mtu size,
    *  if MUX, default to sub_interface mtu size
    *  else default to sub_interface mtu size + ATM_SNAP_HDRBYTES
    */
   if ( (msg->u.setup.bwd_sdu_size > 0) &&
                     (msg->u.setup.bwd_sdu_size <= ATMSIG_MAX_MTU_SIZE) )
      svc->tos.bwdSduSize = msg->u.setup.bwd_sdu_size;
   else {

     if ((svc->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL5MUX)
       svc->tos.bwdSduSize = svc->idb->sub_mtu;
     else
       svc->tos.bwdSduSize = svc->idb->sub_mtu + ATM_SNAP_HDRBYTES;
   }
   
   /* if aal5mux, need to parse blli and get svc->linktype */
   if ((svc->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL5MUX) {
     atmSig_parseBroadbandLowLayerInfo(svc);
   }

   svc->tos.aalType = ATM_AALIE_TYPE5;
   
   svc->layer2id = ATMSIG_LLAYER2_ID;

   /*
    * put in best effort for now, so we don't have to fill in
    * all the rates.
    */
   svc->tos.userCellRate.bestEffortIndicator = 1;

   /* for now don't tag */
   svc->tos.userCellRate.FwdTag =
   svc->tos.userCellRate.BwdTag = -1;

   /* Broadband Bearer Capability Parameters */
   svc->tos.bearerClass = ATM_BCLASS_BCOBC;
   svc->tos.trafficType = ATM_TRAFFICTYPE_VBR;
   svc->tos.timingRequirements = ATM_TIMINGREQ_NULL;
   svc->tos.clipping = ATM_CLIPPING_NULL;
   svc->tos.usrPlaneAccessConfig = ATM_UPAC_NULL;

   /* Quality of Service Parameters */
   svc->tos.txQosClassFwd = ATM_QOSIE_UNSPECD;
   svc->tos.txQosClassBackwd = ATM_QOSIE_UNSPECD;
 
   /* Numbering Plan and Type */
   svc->tos.numberType = ATM_CALLEDPARTY_NUM_INTERNTL;
   svc->tos.numberingPlan = ATM_CALLEDPARTY_NUMPLAN_NSAP;

   /* Point-to-point, Multipoint */
   svc->multiPoint = msg->u.setup.multipoint;

   /* allocate a new call reference value to this svc */
   if ( !atmSig_allocateCallRef(svc) ) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf( "\nATMSIG: unable to alloc callref, open req failed");
      goto setup_req_failed;
   }

   /* enter new svc in the svc hash table */
   atmSig_enterHashEntry(svc);

   /* don't copy timer values - get them from idb when needed */
   party->n2 = svc->idb->hwptr->atm_db->atmsig_retry_cnt;

   /* initialize svc state */ 
   svc->state = ATMSIG_ST_NULL;

   /* initialize party state */ 
   party->partyState = ATMSIG_PST_NULL;

   /* check for multipoint call 
    * if so, set endpointRef = 0 for first party
    */
   if (svc->multiPoint) {
     party->endpointRef = ATMSIG_ENDPOINT_REF_ZERO;
     atmSig_enterPartyBlock(svc, party);
     atmSig_enterPartyHashEntry(svc, party);   
     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMMTP: multiPoint setup");
   }

   /* set conn pending */
   svc->conn_pending = TRUE;
   ++atm->conn_pending;

   svc->event = ATMSIG_REQ_SETUP;

   svc->clientPresent = TRUE;

   if (atmSig_debugAll || atmSig_debugEvent) {
     atm_printnsap(party->destNsap.addr, nsapstr);
     buginf("\nATMSIG: Called  Party Addr: %s", nsapstr);
     atm_printnsap(svc->srcNsap.addr, nsapstr);
     buginf("\nATMSIG: Calling Party Addr: %s", nsapstr);
   }

   return (svc);


/* setup requests from client failed */
setup_req_failed:

   msg1.opcode                 = ATM_RELEASE_COMPLETE;
   msg1.idb                    = msg->idb;
   msg1.u.release_comp.call_id = svc;
   msg1.u.release_comp.cause   = ATM_SIGAPI_FAIL;
 
   if (atmSig_debugAll || atmSig_debugEvent) 
      buginf("\nATMAPI: notifying setup req failed event to client, %x", svc);
   
   /* inform client that setup req has failed */
   (*msg->u.setup.sig_user_callback)(&msg1);

   /* setup failure, free the svc and root party here after the callback */
   free(svc->rootParty);
   free(svc);

   return (NULL);

}



/*
 *  a t m S i g _ a p i _ r e l e a s e _ r e q ( )
 *
 * Called when client request the Signalling driver to send a Release 
 * message. 
 *
 */
atmSvcType *atmSig_api_release_req (sig_api_msg *msg)
{

   atmSvcType *svc;


   svc = (atmSvcType *)msg->u.release.call_id;

   /* valid svc block? */
   if ( !svc || !atmSig_validateSvc(svc) ) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMAPI: invalid svc block, release_req failed");
      return (NULL);
   }

   /* set cause code */
   svc->cause_req = msg->u.release.cause;

   /* this is a new cause for UNI 3.1. it should be applied when user
      initiates normal call clearing */
   if ( atmSig_isV31(svc) && (svc->cause_req == ATM_NORMAL_UNSPECIFIED) )
     svc->cause_req = ATM_CAUSE_CALL_CLEARING;

   svc->event = ATMSIG_REQ_RELEASE;
   
   return (svc);

}



/*
 *  a t m S i g _ a p i _ c o n n e c t _ r e q ( )
 *
 * Translates Sig client's Connect request to svc struct. 
 * 
 */
atmSvcType *atmSig_api_connect_req (sig_api_msg *msg)
{

   atmSvcType *svc;
   int result;
   atmSvcPartyType *party;
 
   svc = (atmSvcType *)msg->u.connect.call_id;

   /* valid svc block? */
   if ( !svc || !atmSig_validateSvc(svc) ) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMAPI: invalid svc block, connect_req failed");
      return (NULL);
   }

   party = svc->rootParty;

   if (!party) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG_API: no party block, connect req failed");
      return (NULL);
   }

   svc->flags = msg->u.connect.flags;

   svc->user_vc_type = msg->u.connect.user_vc_type;

   /* ask driver to create a vc */

   /*
    * vcnum was already obtained from the driver on the incoming Setup
    * message.  But do a sanity check here for it's presence anyway.
    */

   /* vcnum present? */
   if (svc->vcnum <= 0) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: connect_req - vcnum = %d", svc->vcnum);
      return (NULL);
   }

   result = atmSig_reqVcFromDriver(svc,svc->vcnum);

   /* vc created? if not, then release call */
   if (!result) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: atm_create_vc call failed\n" );
      svc->event = ATMSIG_REQ_RELEASE;
      return (svc);
   }

   svc->event = ATMSIG_REQ_CONNECT;

   return (svc);

}



/*
 * a t m S i g _ a p i _ r e l e a s e _ c o m p l e t e _ r e q ( )
 *
 * Called when client sends Release Complete request.
 * 
 */
atmSvcType *atmSig_api_release_complete_req (sig_api_msg *msg)
{

   atmSvcType *svc;

 
   svc = (atmSvcType *)msg->u.release_comp.call_id;

   /* valid svc block? */
   if ( !svc || !atmSig_validateSvc(svc) ) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMAPI: invalid svc block, release_complete_req failed");
      return (NULL);
   }

   svc->event = ATMSIG_REQ_RELEASE_COMPLETE;

   /* set cause code */
   svc->cause_req = msg->u.release_comp.cause;
 
   svc->clientPresent = FALSE;

   return (svc);

}



/*
 * a t m S i g _ g e t S v c D e t a i l s ( )
 *
 */
void atmsig_getSvcDetails (sig_api_msg *msg)
{



}



/*
 *         C  A  L  L  B  A  C  K     F  U  N  C  T  I  O  N  S
 *
 *
 * Called from ATM signalling driver to notify ATM signalling clients of
 * incoming events.  The interface from signalling driver to signalling
 * clients is a function call interface.  (The interface from signalling 
 * clients to the Signalling Driver is message based).
 */


/*
 *  a t m S i g _ n o t i f y S e t u p I n d i c a t i o n ( )
 *
 * Callback function from Signalling state machine.  Called when an
 * incoming Setup message arrives.
 *
 * Checks registered NSAPS and if called number matches, sends back a 
 * Call Proceeding message and then sends a Setup event to the client.
 *
 * If called number does not match any registered NSAPs, sends back a
 * Release Complete message out on the wire.
 */
void atmSig_notifySetupIndication (atmSvcType *svc)
{

   boolean found_nsap;
   atmSig_nsap_info *nsap_ptr;
   sig_api_msg msg;
   atmSvcPartyType *party;
   idbtype *sig_idb;
   hwidbtype *hwptr;
   char nsapstr[MAX_ATM_NSAP_ASCII_LEN];
   

   party = svc->rootParty;

   /* verify sig_idb and hwptr, possible race conditions with console 
    * and hotswap 
    */
   sig_idb = svc->sig_idb;
   if (!sig_idb) {
     if (atmSig_debugAll || atmSig_debugEvent) 
       buginf("\nATMSIGERR: notifySetupIndication, no sig idb");
     return;
   }

   hwptr = sig_idb->hwptr;
   if (!hwptr) {
     if (atmSig_debugAll || atmSig_debugEvent) 
       buginf("\nATMSIGERR: notifySetupIndication, no sig hwptr");
     return;
   }

   /*
    * called party number matches any of the addresses registered on
    * this interface.  Return Release Complete message if not.
    */
   if (hwptr->atm_db->promiscuous_nsap)
      svc->sig_user_callback = 
           hwptr->atm_db->promiscuous_sig_user_callback;
   else {

      /* called party registered? (via listen) */
      found_nsap = FALSE;

      nsap_ptr = hwptr->atm_db->nsap_list_head;

      while (nsap_ptr) {
	if ( bcmp(nsap_ptr->nsap.addr, svc->srcNsap.addr, 
		                      ATM_NSAP_ADDR_LNGTH) == 0 ) {

	  svc->sig_user_callback = nsap_ptr->sig_user_callback;

	  if (!svc->sig_user_callback) {
	    atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_UNALLOC);
	    svc->state = ATMSIG_ST_NULL;
	    svc->freeSvc = TRUE;

	    if (atmSig_debugAll || atmSig_debugError) {
	      buginf("\nATMAPI: no callback for Setup msg\n");
	      atm_printnsap(svc->srcNsap.addr, nsapstr);
	      buginf("%s", nsapstr);
	    } 

	    return;
	  }
	  
	  found_nsap = TRUE;
	  svc->idb = nsap_ptr->idb;
	  break;


         }
         else
            nsap_ptr = nsap_ptr->next;
      }

      /* if NSAP not registered, then reject Setup message and return */
      if (found_nsap == FALSE) {

         if (atmSig_debugAll || atmSig_debugEvent) {
            buginf("\nATMAPI: called addr in Setup msg not registered\n");
	    atm_printnsap(svc->srcNsap.addr, nsapstr);
	    buginf("%s", nsapstr);
         } 

         atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_UNALLOC);
         svc->state = ATMSIG_ST_NULL;
         svc->freeSvc = TRUE;
         if (atmSig_debugAll || atmSig_debugEvent)
            buginf("\nATMSIG: Call rejected in Call Present State (U6)");

         return;
      }

   }

   /*
    * Sig clients could start receiving data before a Connect Ack (which
    * provides a vcd to the client).  To remedy this situation we create
    * a vcd on an incoming Setup message.  If the client decides for any
    * reason to not accept the incoming connection we'll give back the 
    * vcd created.
    */

   /* get a free vcnum from ATM driver */
   svc->vcnum = atm_getvcnum(hwptr);

   /* found free vcnum? */
   if (svc->vcnum == ATM_OUT_OF_VCDS) {
       errmsg(&msgsym(OUT_OF_VCDS,ATM),hwptr->hw_namestring);

      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: atm_getvcnum() returns %d", svc->vcnum);

      atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_RSRC_UNAVAIL);
      svc->state = ATMSIG_ST_NULL;
      svc->freeSvc = TRUE;

      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: Call rejected in Call Present State (U6)");

      return; 
   }

   if (atmSig_debugAll || atmSig_debugEvent) {
     atm_printnsap(svc->srcNsap.addr, nsapstr);
     buginf("\nATMSIG: Called  Party Addr: %s", nsapstr);
     atm_printnsap(party->destNsap.addr, nsapstr);
     buginf("\nATMSIG: Calling Party Addr: %s", nsapstr);
   }

 
   /* got a vcnum, send a Call Proceeding message on the link */ 
   atmSig_txCallProceedingMsg(svc);
   party->retryCount = 0;
   party->n2 = hwptr->atm_db->atmsig_retry_cnt; 
   svc->state = ATMSIG_ST_INCALL_PROCEEDING;

   /* notify client of incoming Setup message */

   msg.opcode          = ATM_SETUP;
   msg.u.setup.call_id = svc;
   msg.u.setup.endpoint_id = svc->rootParty;
   msg.idb             = svc->idb;
   msg.u.setup.vcd     = svc->vcnum;

   /*
    * Called party identifies the destination in the Setup message which
    * represents the source address for this particular svc block.
    */
   msg.u.setup.called_party.type = STATION_ATMNSAP;
   msg.u.setup.called_party.length = svc->srcNsap.length;
   bcopy(svc->srcNsap.addr, msg.u.setup.called_party.addr, 
	 msg.u.setup.called_party.length);
   /*
    * Calling party identifies the source in the Setup message which
    * represents the destination address for this particular svc block.
    * Calling address IE is optional. 
    */
   if (party->destNsap.length > 0) {
      msg.u.setup.calling_party.type   = STATION_ATMNSAP; 
      msg.u.setup.calling_party.length = party->destNsap.length;
      bcopy(party->destNsap.addr, msg.u.setup.calling_party.addr, 
	    msg.u.setup.calling_party.length);
   } else {
      msg.u.setup.calling_party.type   = STATION_ILLEGAL;
      msg.u.setup.calling_party.length = 0;
   } 

   /* fill in B-LLI field */
   msg.u.setup.blli[0].length = svc->blli[0].length;
   bcopy(svc->blli[0].string, msg.u.setup.blli[0].string, 
	 msg.u.setup.blli[0].length);

   msg.u.setup.blli[1].length = msg.u.setup.blli[2].length = 0;

   /* fill in fwd & bwd sdu sizes for this connection */
   msg.u.setup.fwd_sdu_size = svc->tos.fwdSduSize;
   msg.u.setup.bwd_sdu_size = svc->tos.bwdSduSize;

   /* if aal5mux, parse blli to get svc->linktype */
   if (msg.u.setup.blli[0].length >= ATMSIG_LLAYER3_LENGTH)
      atmSig_parseBroadbandLowLayerInfo(svc);

   msg.u.setup.multipoint = svc->multiPoint;

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG: notifying Setup event to client");

   (*svc->sig_user_callback)(&msg);

   svc->clientPresent = TRUE;

}



/*
 *  a t m S i g _ n o t i f y C o n n e c t E v e n t ( )
 *
 * Callback function from the Signalling state machine.  Called when an 
 * incoming Connect message arrives (in response to a previous Setup
 * message that the client had sent).
 *
 * Calls the ATM driver to open a vc and if successful sends a Connect
 * event to the client and sends back a Connect Ack message on the line.
 *
 * If call to open vc to the driver is unsuccessful, then it sends a 
 * a RELEASE message to the aprtner. (A future Release Complete message
 * will notify the client that things went awry, no need to send 
 * anything to the client right now).
 *
 */
void atmSig_notifyConnectEvent (atmSvcType *svc)
{

   sig_api_msg msg;
   boolean result;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG_API: no party block, notify connect failed");
      return;
   }

   /* ask driver to create a vc */

   /* get a free vcnum from ATM driver */
   svc->vcnum = atm_getvcnum(svc->sig_idb->hwptr);

   /* found free vcnum? */
   if (svc->vcnum == ATM_OUT_OF_VCDS) {
       errmsg(&msgsym(OUT_OF_VCDS,ATM),svc->sig_idb->hwptr->hw_namestring);

       if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG: atm_getvcnum() returns %d", svc->vcnum);
      goto tx_release; 
   }

   result = atmSig_reqVcFromDriver(svc,svc->vcnum);

   /* vc created? */
   if (!result) {
      /* give back vc number. */
     atmSig_removeVcFromDriver(svc);

      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMAPI: atm_create_vc call failed\n" );
      goto tx_release;
   }

   /* notify client of incoming Connect message */
 
   msg.opcode                = ATM_CONNECT;
   msg.idb                   = svc->idb;
   msg.u.connect.call_id     = svc;
   msg.u.connect.endpoint_id = svc->rootParty;
   msg.u.connect.vcd         = svc->vcnum;

   /* fill in B-LLI field */
   msg.u.connect.blli.length = svc->blli[0].length;
   bcopy(svc->blli[0].string, msg.u.connect.blli.string,
                                   msg.u.connect.blli.length);

   /* send a Connect Ack message to our partner */
   atmSig_txConnectAckMsg(svc);
   atmSig_stopTimer(party);
   party->retryCount = 0;
   svc->state = ATMSIG_ST_ACTIVE;
   if (svc->multiPoint)
      party->partyState = ATMSIG_PST_ACTIVE;


   (*svc->sig_user_callback)(&msg);
   
   /* stop conn pending */
   if ( (svc->conn_pending) &&
                (svc->sig_idb->hwptr->atm_db->conn_pending > 0) ) {
     --svc->sig_idb->hwptr->atm_db->conn_pending;
     svc->conn_pending = FALSE;
   }

   return;

tx_release:
   atmSig_txReleaseMsg(svc, ATM_CAUSE_RSRC_UNAVAIL);
   atmSig_stopTimer(party);
   svc->state = ATMSIG_ST_RELEASE_REQ;
   atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t308);
   party->retryCount = 0;

}



/*
 *  a t m S i g _ n o t i f y C o n n e c t A c k E v e n t ( )
 *
 * Callback function from the Signalling state machine.  Called when
 * an incoming Connect Acknowledge arrives in reponse to the client's 
 * Connect message.
 *
 * On receipt of this event, the client can send data over this vc.
 */
void atmSig_notifyConnectAckEvent (atmSvcType *svc)
{

   sig_api_msg msg;


   /* notify client of incoming Connect Ack message */
 
   msg.opcode                = ATM_CONNECT_ACK;
   msg.idb                   = svc->idb;
   msg.u.connect_ack.call_id = svc;

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMAPI: notifying Connect Ack event to client");

   (*svc->sig_user_callback)(&msg);

}



/*
 *  a t m S i g _ n o t i f y R e l e a s e I n d i c a t i o n ( )
 *
 * Notifies client of an incoming Release message arrives.
 *
 * Does NOT send Release Complete message out on the wire immediately. 
 * However it starts a timer just in case the client does not send a
 * Release Complete message.  It sends one upon expiration of timer
 * and frees the svc.
 *
 * The client MUST send a Release Complete request after it has rcvd
 * this message & deallocated all resources pertaining to this svc.
 */
void atmSig_notifyReleaseIndication (atmSvcType *svc)
{

   sig_api_msg msg;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG_API: no party block, notify release failed");
      return;
   }


   /* notify client of incoming Release message */

   msg.opcode            = ATM_RELEASE;
   msg.idb               = svc->idb;
   msg.u.release.call_id = svc;
   msg.u.release.cause   = svc->causeValue;

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMAPI: notifying Release event to client");

   (*svc->sig_user_callback)(&msg);

   svc->state = ATMSIG_ST_RELEASE_IND;

   /* start timer in case Release Complete event is not rcvd from client */
   party->retryCount = 0;
   atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t308);

}



/*
 * a t m S i g _ n o t i f y R e l e a s e C o m p l e t e I n d i c a t i o n ( )
 *
 * Notifies client of an incoming Release Complete message.
 *
 */
void atmSig_notifyReleaseCompleteIndication (atmSvcType *svc)
{

    sig_api_msg msg;


   /* notify client of incoming Release Complete message */ 

   msg.opcode                 = ATM_RELEASE_COMPLETE;
   msg.idb                    = svc->idb;
   msg.u.release_comp.call_id = svc;
   msg.u.release_comp.cause   = svc->causeValue;

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMAPI: notifying Release Complete event to client");

   (*svc->sig_user_callback)(&msg);

   svc->clientPresent = FALSE;

}



/*
 * Multipoint Functions 
 */
/*
 * a t m S i g _ a p i _ a l l o c a t e _ e n d p o i n t _ r e f _ i d ( )
 * 
 * Signalling client has a connection and wishes to initiate adding another 
 * party to the call, it must first call this function to allocate an 
 * "endpoint_id". The AddParty request header _must_ contain this 
 * endpoint_id. This is the same behavior as atmSig_api_allocate_call_id.
 * 
 * Subsequent callback messages for this party will have the same endpoint_id, 
 * thus allowing the signalling client to match indications to a particular 
 * endpoint.
 *
 * A return value of 0 means that an endpoint_id could not be allocated (no
 * memory).
 *
 * In this implementation, the endpoint_id is actually a party struct pointer.
 * The party struct is allocated in this function by the Signalling driver.  
 * However for the Signalling client the endpoint_id is simply a (void *)32
 * bit quantity, a magic cookie.
 */
void *atmSig_api_allocate_endpoint_id (void *call_id)
{

   atmSvcType *svc;
   atmSvcPartyType *party;

   /* sanity here, validate svc */
   svc = (atmSvcType *)call_id;

   /* malloc party block */
   party = atmSig_allocatePartyBlock();

   if (!party) {
     return (NULL);
   }

   party->svc = svc;

   return (party);

}



/*
 * a t m S i g _ a p i _ a d d _ p a r t y _ r e q
 *
 * Client requests Signalling driver to send AddParty message
 */
atmSvcType *atmSig_api_add_party_req (sig_api_msg *msg)
{

   atmSvcType *svc;
   atmSvcPartyType *party;
   boolean result;
   sig_api_msg msg1;
   atm_db_t *atm;
   char nsapstr[MAX_ATM_NSAP_ASCII_LEN];

   /* 
    * the "call-id" field in the msg header is actually a pointer to the 
    * svc struct that sig allocated when atmSig_api_allocate_call_id() 
    * was called by Sig User
    */ 
   svc = (atmSvcType *)msg->u.add_party.call_id;
  
   /* validate the SVC for this MTP call */
   result = atmSig_validateReqSvc(svc, msg);
   if (result == FALSE) {
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIG_API: add_party_req, invalid svc block");
     return (NULL);
   }

   party = (atmSvcPartyType *)msg->u.add_party.endpoint_id;

   /* valid party block? */
   if (!party) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG_API: add_party_req, invalid party block");
      return (NULL);
   }

   /* KEEP THIS HERE
    * Nsap info must be filled into party block in case the add_party_req
    * fails. The static map (or other) client needs it in order to clean
    * up when receiving add party reject.
    */
   /* save dest atm length & address in party block */
   party->destNsapLength = msg->u.add_party.called_party.length;
   bcopy(msg->u.add_party.called_party.addr, party->destNsap.addr, 
	 party->destNsapLength);

   atm = svc->idb->hwptr->atm_db;

   /* limit # of local connections req to max locally open req config'd */
   if (atm->conn_pending >= atm->max_conn_pending) {
      if (atmSig_debugAll || atmSig_debugEvent)
	buginf("\nATMSIG: %d connections pending, party open req failed",
	       atm->conn_pending);
      goto add_party_req_failed;
    }

   /* allocate a new endpoint reference value to this party */
   if ( !atmSig_allocateEndpointRef(svc, party) ) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG_API: add_party_req, can't allocate endpoint ref");
      goto add_party_req_failed;
    }

   /* enter party into svcPartyQ */
   if ( !(atmSig_enterPartyBlock(svc, party)) ) {
     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMSIG_API: add_party_req, can't enter party block");
     goto add_party_req_failed;
   }

   /* enter party into party Hash table */
   if ( !(atmSig_enterPartyHashEntry(svc, party)) ) {
     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMSIG_API: add_party_req, can't hash party block");
     goto add_party_req_failed;
   }

   /* don't copy timer values - get them from idb structure when needed */
   party->n2 = svc->idb->hwptr->atm_db->atmsig_retry_cnt;

   /* set party state and event, use multiStateTable upon return */

   /* initialize party state */ 
   party->partyState = ATMSIG_PST_NULL;

   /* set conn pending */
   party->conn_pending = TRUE;
   ++atm->conn_pending;

   svc->event = ATMSIG_REQ_ADD_PARTY;

   if (atmSig_debugAll || atmSig_debugEvent) {
     atm_printnsap(party->destNsap.addr, nsapstr);
     buginf("\nATMSIG: Called  Party Addr: %s", nsapstr);
     atm_printnsap(svc->srcNsap.addr, nsapstr);
     buginf("\nATMSIG: Calling Party Addr: %s", nsapstr);
   }


   return (svc);

/* add_party requests from client failed */
add_party_req_failed:

   msg1.opcode                      = ATM_ADD_PARTY_REJ;
   msg1.idb                         = svc->idb;
   msg1.u.add_party_rej.call_id     = svc;
   msg1.u.add_party_rej.endpoint_id = party;
   msg1.u.add_party_rej.cause       = ATM_SIGAPI_FAIL;
 
   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMAPI: notifying add_party_req failed event to client");

   /* inform client that add_party req has failed */
   (*svc->sig_user_callback)(&msg1);

   /* add_party failure, free the party here after the callback */
   free(party);

   return (NULL);

}



/*
 * a t m S i g _ a p i _ a d d _ p a r t y _ a c k _ r e q
 *
 * Client requests Signalling driver to send AddPartyAck message
 */
atmSvcType *atmSig_api_add_party_ack_req (sig_api_msg *msg)
{

   atmSvcType *svc;
   atmSvcPartyType *party;
   boolean result;


   svc = (atmSvcType *)msg->u.add_party_ack.call_id;

   /* validate the SVC for this MTP call */
   result = atmSig_validateReqSvc(svc, msg);
   if (result == FALSE)
     return (NULL);

   party = (atmSvcPartyType *)msg->u.add_party_ack.endpoint_id;

   /* valid party block? */
   if (!party || !atmSig_validateParty(svc, party) ) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG_API: add_party_ack_req, invalid party block");
      return (NULL);
   }

   svc->event = ATMSIG_REQ_ADD_PARTY_ACK;

   return(svc);


}


/*
 * a t m S i g _ a p i _ a d d _ p a r t y _ r e j _ r e q
 *
 * Client requests Signalling driver to send AddPartyRej message
 */
atmSvcType *atmSig_api_add_party_rej_req (sig_api_msg *msg)
{

   atmSvcType *svc;
   atmSvcPartyType *party;
   boolean result;


   svc = (atmSvcType *)msg->u.add_party_rej.call_id;

   /* validate the SVC for this MTP call */
   result = atmSig_validateReqSvc(svc, msg);
   if (result == FALSE)
     return (NULL);

   party = (atmSvcPartyType *)msg->u.add_party_rej.endpoint_id;

   /* valid party block? */ 
   if ( !party || !atmSig_validateParty(svc, party) ) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG_API: add_party_rej_req, invalid party block");
      return (NULL);
   }

   svc->event = ATMSIG_REQ_ADD_PARTY_REJ;

   return(svc);

}


/*
 * a t m S i g _ a p i _ d r o p _ p a r t y _ r e q
 *
 * Client requests Signalling driver to send DropParty message
 */
atmSvcType *atmSig_api_drop_party_req (sig_api_msg *msg)
{

   atmSvcType *svc;
   atmSvcPartyType *party;
   boolean result;


   svc = (atmSvcType *)msg->u.drop_party.call_id;

   /* validate the SVC for this MTP call */
   result = atmSig_validateReqSvc(svc, msg);
   if (result == FALSE)
     return (NULL);

   party = (atmSvcPartyType *)msg->u.drop_party.endpoint_id;

   /* valid party block? */ 
   if ( !party || !atmSig_validateParty(svc, party) ) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG_API: drop_party_req, invalid party block");
      return (NULL);
   }

   svc->event = ATMSIG_REQ_DROP_PARTY;

   return(svc);


}


/*
 * a t m S i g _ a p i _ d r o p _ p a r t y _ a c k _ r e q
 *
 * Client requests Signalling driver to send DropPartyAck message
 */
atmSvcType *atmSig_api_drop_party_ack_req (sig_api_msg *msg)
{

   atmSvcType *svc;
   atmSvcPartyType *party;
   boolean result;


   svc = (atmSvcType *)msg->u.drop_party_ack.call_id;

   /* validate the SVC for this MTP call */
   result = atmSig_validateReqSvc(svc, msg);
   if (result == FALSE)
     return (NULL);

   party = (atmSvcPartyType *)msg->u.drop_party_ack.endpoint_id;

   /* valid party block? */ 
   if ( !party || !atmSig_validateParty(svc, party) ) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG_API: drop_party_ack_req, invalid party block");
      return (NULL);
   }

   svc->event = ATMSIG_REQ_DROP_PARTY_ACK;

   return(svc);


}



/*
 * a t m S i g _ n o t i f y A d d P a r t y E v e n t 
 * 
 * Callback function from Signalling state machine.  Called when an
 * incoming AddPartyEvent message arrives.
 */
void atmSig_notifyAddPartyEvent (atmSvcType *svc, atmSvcPartyType *party)
{

   boolean found_nsap;
   atmSig_nsap_info *nsap_ptr;
   int i;
   sig_api_msg msg;
   idbtype *sig_idb;
   hwidbtype *hwptr;
   char nsapstr[MAX_ATM_NSAP_ASCII_LEN];   

   /* verify sig_idb and hwptr, possible race conditions with console 
    * and hotswap 
    */
   sig_idb = svc->sig_idb;
   if (!sig_idb) {
     if (atmSig_debugAll || atmSig_debugEvent) 
       buginf("\nATMSIGERR: notifyAddPartyIndication, no sig idb");
     return;
   }

   hwptr = sig_idb->hwptr;
   if (!hwptr) {
     if (atmSig_debugAll || atmSig_debugEvent) 
       buginf("\nATMSIGERR: notifyAddpartyIndication, no sig hwptr");
     return;
   }



   /*
    * Check to see if called party number matches any of the NSAP's
    * registered on this interface.  Return Add Party Rej message if not.
    */
   if (hwptr->atm_db->promiscuous_nsap)
      svc->sig_user_callback = 
           hwptr->atm_db->promiscuous_sig_user_callback;
   else {
      /* called party NSAP registered? (via LISTEN) */
      found_nsap = FALSE;
      nsap_ptr = hwptr->atm_db->nsap_list_head;
      while (nsap_ptr) {
	if (bcmp(nsap_ptr->nsap.addr, svc->srcNsap.addr, 
		                          ATM_NSAP_ADDR_LNGTH) == 0) { 
	  found_nsap = TRUE;
          svc->sig_user_callback = nsap_ptr->sig_user_callback;
	  break;
	}
	else
	  nsap_ptr = nsap_ptr->next;
      }

      /* if NSAP not registered, then reject Add Party message */
      if (found_nsap == FALSE) {

         if (atmSig_debugAll || atmSig_debugEvent) {
            buginf("\nATMSIG_API: Received AddParty msg");
            for (i = 0; i < ATM_NSAP_ADDR_LNGTH; ++i)
               buginf("%x ", party->destNsap.addr[i]);
         } 

	 atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_NORMAL_UNSPECIFIED);
	 party->partyState = ATMSIG_PST_NULL;
	 party->freeParty = TRUE;
	 if (atmSig_debugAll || atmSig_debugEvent)
	   buginf("\nATMSIG: AddParty rejected, no NSAP");

	 return;
      }
   }

   if (atmSig_debugAll || atmSig_debugEvent) {
     atm_printnsap(svc->srcNsap.addr, nsapstr);
     buginf("\nATMSIG: Called  Party Addr: %s", nsapstr);
     atm_printnsap(party->destNsap.addr, nsapstr);
     buginf("\nATMSIG: Calling Party Addr: %s", nsapstr);
   }

   /*
    * send AddParty event to the Signalling client.
    * PartyState = ADD_PARTY_RECEIVED from multi state machine
    */

   msg.opcode                  = ATM_ADD_PARTY;
   msg.idb                     = svc->idb;
   msg.u.add_party.call_id     = svc;
   msg.u.add_party.endpoint_id = party;

   /*
    * Called party identifies the source in the AddParty message which
    * represents the destination address for this particular party block.
    */
   msg.u.add_party.called_party.length = svc->srcNsap.length;
   bcopy(svc->srcNsap.addr, msg.u.add_party.called_party.addr, 
	 msg.u.add_party.called_party.length);

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG_API: Sending Add Party event to client");

   (*svc->sig_user_callback)(&msg);

}


/*
 * a t m S i g _ n o t i f y A d d P a r t y A c k E v e n t 
 * 
 * Callback function from Signalling state machine.  Called when an
 * incoming AddPartyAckEvent message arrives.
 */
void atmSig_notifyAddPartyAckEvent (atmSvcType *svc, atmSvcPartyType *party)
{

   sig_api_msg msg;


   /* send Add Party Ack event to signalling client */
 
   msg.opcode                      = ATM_ADD_PARTY_ACK;
   msg.idb                         = svc->idb;
   msg.u.add_party_ack.call_id     = svc;
   msg.u.add_party_ack.endpoint_id = party;

   /* Our Add Party msg accepted, change party state */
   party->partyState = ATMSIG_PST_ACTIVE;

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG_API: Sending Add Party Ack event to client");

   (*svc->sig_user_callback)(&msg); 

   /* stop conn pending */
   if ( (party->conn_pending) &&
                (svc->sig_idb->hwptr->atm_db->conn_pending > 0) ) {
     --svc->sig_idb->hwptr->atm_db->conn_pending;
     party->conn_pending = FALSE;
   }

   return;

}


/*
 * a t m S i g _ n o t i f y A d d R e j P a r t y E v e n t 
 * 
 * Callback function from Signalling state machine.  Called when an
 * incoming AddPartyEventRej message arrives.
 */
void atmSig_notifyAddPartyRejEvent (atmSvcType *svc, atmSvcPartyType *party)
{

   sig_api_msg msg;


   /* send Add Party Rej event to signalling client */
 
   msg.opcode                      = ATM_ADD_PARTY_REJ;
   msg.idb                         = svc->idb;
   msg.u.add_party_rej.call_id     = svc;
   msg.u.add_party_rej.endpoint_id = party;
   msg.u.add_party_rej.cause       = svc->causeValue;

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG_API: Sending Add Party Rej event to client");

   (*svc->sig_user_callback)(&msg);

   /* stop conn pending */
   if ( (party->conn_pending) &&
                (svc->sig_idb->hwptr->atm_db->conn_pending > 0) ) {
     --svc->sig_idb->hwptr->atm_db->conn_pending;
     party->conn_pending = FALSE;
   }

}


/*
 * a t m S i g _ n o t i f y D r o p P a r t y E v e n t 
 * 
 * Callback function from Signalling state machine.  Called when an
 * incoming DropPartyEvent message arrives.
 */
void atmSig_notifyDropPartyEvent (atmSvcType *svc, atmSvcPartyType *party)
{

   sig_api_msg msg;

     
   /* send Drop Party event to signalling client */
 
   msg.opcode                   = ATM_DROP_PARTY;
   msg.idb                      = svc->idb;
   msg.u.drop_party.call_id     = svc;
   msg.u.drop_party.endpoint_id = party;
   msg.u.drop_party.cause       = svc->causeValue;

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG_API: Sending Drop Party event to client");

   (*svc->sig_user_callback)(&msg);

}


/*
 * a t m S i g _ n o t i f y D r o p P a r t y A c k E v e n t 
 * 
 * Callback function from Signalling state machine.  Called when an
 * incoming DropPartyEventAck message arrives.
 */
void atmSig_notifyDropPartyAckEvent (atmSvcType *svc, atmSvcPartyType *party)
{

   sig_api_msg msg;

   /* send Drop Party Ack event to signalling client */
 
   msg.opcode                       = ATM_DROP_PARTY_ACK;
   msg.idb                          = svc->idb;
   msg.u.drop_party_ack.call_id     = svc;
     msg.u.drop_party_ack.endpoint_id = party;

   if (atmSig_debugAll || atmSig_debugEvent)
      buginf("\nATMSIG_API: Sending Drop Party Ack event to client");

   (*svc->sig_user_callback)(&msg);

}



/*
 *
 *
 * Validate SVC for this Multipoint Request
 *
 */
boolean atmSig_validateReqSvc (atmSvcType *svc, sig_api_msg *msg)
{

   char *msg_string;


   switch (msg->opcode) {

   case ATM_ADD_PARTY:
     svc = (atmSvcType *)msg->u.add_party.call_id;
     msg_string = "add_party_req";
     break;

   case ATM_ADD_PARTY_ACK:
     svc = (atmSvcType *)msg->u.add_party_ack.call_id;
     msg_string = "add_party_ack_req";
     break;

   case ATM_ADD_PARTY_REJ:
     svc = (atmSvcType *)msg->u.add_party_rej.call_id;
     msg_string = "add_party_rej_req";
     break;

   case ATM_DROP_PARTY:
     svc = (atmSvcType *)msg->u.drop_party.call_id;
     msg_string = "drop_party_req";
     break;

   case ATM_DROP_PARTY_ACK:
     svc = (atmSvcType *)msg->u.drop_party_ack.call_id;
     msg_string = "drop_party_ack_req";
     break;

   default:
     if (atmSig_debugAll || atmSig_debugError) 
       buginf("\nATMAPI: validateReqSvc, invalid multipoint opcode = %x!", 
	      msg->opcode);
     return (FALSE);

   }


   /* valid svc block? */
   if (!svc || !atmSig_validateSvc(svc) ) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG_API: %s, svc not valid", msg_string);
      return (FALSE);
   }

   /* multipoint svc? */
   if (!svc->multiPoint ) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG_API: %s, svc not multiPoint", msg_string);
      return (FALSE);
   }

   /* svc active? */
   if (svc->state != ATMSIG_ST_ACTIVE) {
      if (atmSig_debugAll || atmSig_debugError)
         buginf("\nATMSIG_API: %s, svc not active", msg_string);
      return (FALSE);
   }

   return (TRUE);
}


/*
** given a "callId", returns corresponding vcd
*/
vcd_t atmSig_api_callId2vcd (void *callId)
{
    atmSvcType *svc = (atmSvcType*) callId;

    if (! atmSig_validateSvc(svc)) return 0;
    return svc->vcnum; 
}

/*
** given a callid, copies the calling party address from
** it to the user supplied hwaddrtype.  If calling party
** address is not available (since it is an optional ie),
** sets the type to illegal address and length to zero.
*/
void atmSig_api_callId2CallingPartyAddress (void *callId, 
    hwaddrtype *callingPartyAddress)
{
    atmSvcType *svc = (atmSvcType*) callId;

    if (! atmSig_validateSvc(svc))
    {
	snpa_clear(callingPartyAddress);
	return;
    }
    /* *callingPartyAddress = svc->rootParty->destNsap; */
    /* use this on account of being more "readable" */
    bcopy(&svc->rootParty->destNsap, callingPartyAddress,
	sizeof(hwaddrtype));
}

/*
** compares a user specified calling party address with
** the calling party address of a given call id.  Returns
** TRUE if same, FALSE otherwise.
*/
boolean atmSig_api_sameCallingPartyAddress (void *callId,
    hwaddrtype *callingPartyAddress)
{
    atmSvcType *svc = (atmSvcType*) callId;
    hwaddrtype *cpa;

    if (! atmSig_validateSvc(svc)) return FALSE;
    cpa = &svc->rootParty->destNsap;
    if ((cpa->length != STATIONLEN_ATMNSAP) || 
	(cpa->type != STATION_ATMNSAP))
	    return FALSE;
    return 
	bcmp(cpa->addr, callingPartyAddress->addr, 
	    STATIONLEN_ATMNSAP) == 0;
}



