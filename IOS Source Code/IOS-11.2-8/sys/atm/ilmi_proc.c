/* $Id: ilmi_proc.c,v 3.9.6.5 1996/08/09 02:13:31 yanke Exp $
 * $Source: /release/112/cvs/Xsys/atm/ilmi_proc.c,v $
 *------------------------------------------------------------------
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_proc.c,v $
 * Revision 3.9.6.5  1996/08/09  02:13:31  yanke
 * send queries for uni version after receives traps.
 *
 * CSCdi54317:  ilmi uni-version negotiation doesnt work properly sometimes
 * Branch: California_branch
 *
 * Revision 3.9.6.4  1996/07/10  22:20:44  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.9.6.3  1996/06/19  02:20:24  kartik
 * CSCdi60486:  ILMI failed to report delinquency in its Remote Table
 * Branch: California_branch
 *
 * Revision 3.9.6.2  1996/05/09  14:11:30  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.9.6.1.18.1  1996/04/27  06:33:52  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.9.6.1.8.2  1996/04/10  23:35:50  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.9.6.1.8.1  1996/04/08  01:44:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.9.6.1  1996/03/18  18:59:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.10.1  1996/03/25  02:18:26  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.9  1996/03/04  21:47:04  rbadri
 * CSCdi50537:  Router sends malformed OID in LECS address getnext request.
 * In getnext request to get the LECS address from the switch, specify
 * OID with and without length field(separate request) to work with
 * LS100/LS1010.
 *
 * Revision 3.8.2.3  1996/03/21  21:50:50  cakyol
 * Kartik's fix for "ILMI hung" problem picked up manually.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.9  1996/03/04  21:47:04  rbadri
 * CSCdi50537:  Router sends malformed OID in LECS address getnext request.
 * In getnext request to get the LECS address from the switch, specify
 * OID with and without length field(separate request) to work with
 * LS100/LS1010.
 *
 * Revision 3.8  1996/02/09  18:36:24  kartik
 * CSCdi48139:   Fixed ILMI request ID counter wrap-around which occurs
 * after the ILMI is up for an extended time.
 *
 * Revision 3.7  1996/01/31  11:44:39  kartik
 * CSCdi47885:  Preemptive: Initialize pointer to NULL after ILMI pkt
 * buffer freed.
 * Possible exposure due to fix for CSCdi47576.
 *
 * Revision 3.6  1996/01/29  19:16:16  kartik
 * CSCdi47576:  Memory leak in ILMI
 * Encoding and allocation erroneously repeated for request retries.
 *
 * Revision 3.5  1996/01/02  18:27:37  bbenson
 * CSCdi46281:  ILMI should consider trap as indication of empty
 * atm-address table.
 * ILMI on the router should reset to initial state whenever it receives a
 * trap from the switch
 *
 * Revision 3.4  1995/12/06  10:42:36  kartik
 * CSCdi44909:  memory leak in ilmi code
 * ILMI was holding on to the memory it allocated for trap encoding.
 *
 * Revision 3.3  1995/12/05  22:29:59  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI address registration will now ignore the selector byte.
 *
 * Revision 3.2  1995/11/17  08:46:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/10/23  19:46:14  kartik
 * CSCdi42628:  Confusing Debug Message when Router asks node for prefix.
 *
 * Revision 2.6  1995/10/17  06:18:03  kartik
 * CSCdi42254:  ILMI memory leak in conducting get-nexts.
 *
 * Revision 2.5  1995/08/08  16:45:30  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.4  1995/08/08 05:33:31  schrupp
 * CSCdi38341:  Dont cancel ATM NSAP if never registered
 *
 * Revision 2.3  1995/06/19  21:28:06  kartik
 * CSCdi36096:  Fixed mallocs in ILMI which were not being checked for
 * return
 * code.
 *
 * Revision 2.2  1995/06/18  20:31:46  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:12:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "address.h"
#include "../h/interface_private.h"
#include "../h/packet.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../snmp/snmp_api.h"
#include "mgd_timers.h"
#include "ilmi_public.h"
#include "ilmi_private.h"
#include "ilmi_api.h"

/*
 * The request id that is used by the ILMI for the outgoing requests.
 * This is defined as a static variable which is incremented before
 * being assigned. It will wrap around after it reaches the maximum
 * 32 bit unsigned value.
 */
static ilmiReqID ilmi_reqid = 1;
static long ilmiPollObject[] = {1,3,6,1,4,1,353,2,1,1,1,5,0};
static int ilmiPollObjectlen = 13;

/*
 * The managed timers.
 */
static mgd_timer ilmi_master_timer;
static mgd_timer ilmi_master_evtimer;


/*-------------------------------------------------------------------------
 * This function is called whenever there is a message in the input queue.
 * It goes through all the messages in the input queue and processes them
 * one at a time. The input queue itself is filled up by the ILMI API which
 * handles the Client requests. The ILMI process itself may add timed out 
 * requests which have a non-zero retry value.
 */

void
ilmi_process_input (ilmi_msgblock *msg)
{
    boolean action;
    OctetString *address;


    
    if (msg->encodedReq == NULL) {   /* New Reqid not required for Retry */
	ilmi_reqid++;
	if (ilmi_reqid >= MAXSHORT) {
	    ilmi_reqid = 1;
	}
	msg->req_id = ilmi_reqid;
    }

    if (atm_ilmi_debug) {
	buginf("\nILMI: REQ_PROCESSING ");
	ilmi_dump_msgblock(msg);
    }
	    
    /*
     * Check if related to address registration/deregistration.
     */

    if (msg->client_id  != ILMI_ID) {
	address = ilmi_addressreg_vb(msg);
	if (address) {
	    
	/* It is related to address registration so we perform local
	 * bookkeeping. This is because for ILMI (De)Registration we
	 * only look at the first 19 bytes and ignore the selector
	 * byte
	 */

	    if (ilmi_validate_localtable(msg, address, &action) == FALSE) {
		ilmi_log_error(ILMI_ID, ilmiLocalAddFailure, "<ilmi_process_input>");
		msg->errcode = ilmiBadObject;
		enqueue(&ilmi_responseQ,msg);
		FreeOctetString(address);
		return;
	    }

	    if (action == FALSE) {
		/*
		 * Based on the local table there is no need to register this
		 * address.
		 */
		msg->errcode = ilmiResponseReceived;
		msg->response = ilmiRespNoErr;
		enqueue(&ilmi_responseQ,msg);
		FreeOctetString(address);
		return;

	    }


	    /*
	     * This is set only for messages which are sent out.
	     */

	    msg->address_reg_flag = TRUE;
	    FreeOctetString(address);
	    /* Fall through and send this message out. */
	}
    }
	
    /* 
     * Get the ASN.1 Encoding for the Message.
     */

    if (msg->encodedReq == NULL ) { /* Ensure not a retry */
	if(!(ilmi_req_encode(msg))) {
	    ilmi_log_error(ILMI_ID, ilmiReqEncodeError, "<ilmi_process_input>");
	    msg->errcode = ilmiBadObject;
	    enqueue(&ilmi_responseQ,msg);
	    return;
	}
    }
    
    

    
    if( !(ilmi_send_pkt(msg))) {
        ilmi_log_error(ILMI_ID,ilmiSendPduError,"<ilmi_process_input>");
        msg->errcode = ilmiBadIntfBlock;
        enqueue(&ilmi_responseQ,msg);
        return;
    }



    /* 
     * We were able to send the message out so now we
     * put the message in the hash table and set the 
     * timer ticking .. TICK-TOCK TICK-TOCK .
     */

    if ( (ilmi_create_event(msg->intf_id, ilmiPeriodicReqRetry,
        ILMI_REQTIMEOUT, msg->req_id)) == FALSE) {
	if (atm_ilmi_debug) {
	    buginf("\nILMI: TIMERADDFAILURE ");
	    ilmi_dump_msgblock(msg);
	}
	msg->errcode = ilmiBadObject;
	enqueue(&ilmi_responseQ,msg);
    }
    
    ilmi_reqmsg_hashenter(msg);	
	
}



/*-------------------------------------------------------------------------
 * This function is called whenever there is a message in the response
 * queue. It goes through all the messages in the response queue and 
 * processes them all. The queue is filled up  whenever a response to 
 * ILMI requests is received.
 */

void
ilmi_process_response (ilmi_msgblock *msg)
{
    ilmi_clientblock *ilmi_client;
    ilmiClientReq client_req;
    ilmi_intfblock *intfblk;
    VarBind *vb_ptr=NULL;
    char *str;
    long ipaddr;

    if (msg->client_id == ILMI_ID) {
        switch(msg->oper) {
        case GET_REQUEST_TYPE:
            if ((intfblk =  ilmi_getintfblk (msg->intf_id)) == NULL)
                break;
            if( (msg->errcode == ilmiResponseReceived) &&  
                (msg->response == ilmiRespNoErr)) 
                vb_ptr = msg->vblist;
                switch(msg->ilmi_func) {
                case ilmiPoll:
		    if( (msg->errcode == ilmiResponseReceived) &&
		       (msg->response == ilmiRespNoErr)) {
			ilmi_update_polldata(msg->intf_id,msg->errcode);
		    }
                    break;
                case ilmiUniVersion:
		     if(msg->errcode == ilmiResponseReceived) {
			 if (msg->response == ilmiRespNoErr) {
			     intfblk->idb->atm_db->peeruni_version = vb_ptr->value.sl_value;
			 } else {
			     intfblk->idb->atm_db->peeruni_version = ATMUNI_VERSION_30;
			 }
			
		     } else {
			 intfblk->idb->atm_db->peeruni_version =
			     ATMUNI_VERSION_INVALID;
		     }

		     /* Any way now notify signalling */
		     ilmi_notify_universion(intfblk,
						intfblk->idb->atm_db->peeruni_version);

		    if(atm_ilmi_debug)
			buginf("\nILMI: Peer UNI Version on %d = %d",
			       msg->intf_id,
			       intfblk->idb->atm_db->peeruni_version);

			       
                    break;
                case ilmiNeighborsIp:
		    if( (msg->errcode == ilmiResponseReceived) &&
		       (msg->response == ilmiRespNoErr)) {
			ipaddr = OctetStringToIP(vb_ptr->value.os_value);
			intfblk->idb->atm_db->peeruni_ip = ipaddr;
		    }
		    break;
                case ilmiNeighborsIfName:
		     if( (msg->errcode == ilmiResponseReceived) &&
			(msg->response == ilmiRespNoErr)) {
			 str = ilmiMakeStrFromOstr(vb_ptr->value.os_value);
			 if(str) {
			     if (intfblk->idb->atm_db->peeruni_ifname)
				 free(intfblk->idb->atm_db->peeruni_ifname);
			     intfblk->idb->atm_db->peeruni_ifname = str ;
			     if(atm_ilmi_debug)
				 buginf("\nILMI: Peer IfName on %d = %s",msg->intf_id,intfblk->idb->atm_db->peeruni_ifname);
			 }
		     }
		    break;
                case ilmiAddressTableCheck:
                case ilmiReqOther:
                    break;
                }

            break;
        case SET_REQUEST_TYPE:
	    if( (msg->oper == SET_REQUEST_TYPE) &&
	       (msg->address_reg_flag == TRUE) &&
	       (msg->errcode == ilmiResponseReceived)) {
		/* If this is a response to an address request we
		 * update our table for the addresses registered
		 * with the peer. These usually are related to
		 * synching the tables and generated by ILMI_ID.
		 */
		ilmi_updatepeer_table(msg);
	    }
	    
	    break;
        case GET_NEXT_REQUEST_TYPE:
            if (msg->ilmi_func == ilmiAddressTableCheck) {
                /* Is the interface still there. */
                if ((intfblk =  ilmi_getintfblk (msg->intf_id)) == NULL)
                    break;

                if( (msg->errcode == ilmiResponseReceived) && 
                    ( (msg->response == ilmiRespNoSuchNameErr) || 
                    (ilmiAddTableEmpty(msg)) )
                    ) {
                    if(intfblk->state == ilmiIntfAwaitRestartAck) {
                        ilmi_state_transit(msg->intf_id,intfblk->state,
                            ilmiIntfUpAndNormal,"<ilmi_process_response>");
                        intfblk->state = ilmiIntfUpAndNormal;
			/*
			 * Start time for periodic address check.
			 */
			ilmi_create_event(intfblk->intf_id, 
					  ilmiPeriodicPeerAddressCheck, 
					  ILMI_PEERADDRCHECK_TIMEOUT,0);
                        ilmi_initial_sets(intfblk);
                    }
                }
                else if (intfblk->state != ilmiIntfUpAndNormal) {
                    ilmi_state_transit(intfblk->intf_id,intfblk->state,
                        ilmiIntfRestarting,"<ilmi_process_response>");
                    intfblk->state = ilmiIntfRestarting;
                }
            }
            break;
        default:
            break;
        }
    }
    else if ( (ilmi_client = ilmi_getclientblk (msg->client_id)) != NULL) {

	switch (msg->oper) {
	case GET_NEXT_REQUEST_TYPE:
	    /* At this point we assume that the only getnexts we are
	     * doing is to get the lecs addresses.
	     */
	    if (ilmi_lecs_response(ilmi_client, msg) == ILMI_OK) {
		return;
	    }
	    break;
	case SET_REQUEST_TYPE:
	    if ((msg->address_reg_flag == TRUE) &&
		(msg->errcode == ilmiResponseReceived) &&
		(msg->errcode == ilmiResponseReceived)) {
		/* If this is a response to an address request we
		 * update our table for the addresses registered
		 * with the peer.
		 */
		ilmi_updatepeer_table(msg);
	    }
	    /* Fall Through */
	    
	default:
	    if(ilmi_client->response_notify) {
		client_req.client_id     = msg->client_id;
		client_req.intf_id       = msg->intf_id;
		client_req.vblist        = msg->vblist;
		client_req.error         = msg->errcode;
		client_req.response      = msg->response;
		client_req.transid       = msg->transid;
		client_req.notify_type   = ilmiResponseNotify;
		client_req.address       = NULL;
		client_req.notifyCB      = NULL;
		(ilmi_client->response_notify)(&client_req);
	    }
	    
	    break;
	}
    } else {
	if (atm_ilmi_debug)
	    buginf("\nILMI: Orphan Message. Client not registered");
    }

    
    if (atm_ilmi_debug) {
	buginf("\nILMI: TERMINATE ");
	ilmi_dump_msgblock(msg);
    }

    ilmi_free_msgblk(msg);
    return;
}



/*-------------------------------------------------------------------------
* This function issues a periodic poll to update the status of each one of 
* its UNI links.
*/
void
ilmi_process_poll (ilmiIntf intf_id)
{
    ilmi_msgblock *msg;
    VarBind *vb;
    OID tmpoid;
    ilmi_intfblock *intfblk=NULL;

    intfblk = ilmi_getintfblk(intf_id);

    if(!intfblk)
        return;

    if ( (msg = malloc(sizeof(ilmi_msgblock))) == NULL) {
        return;
    }
    if( (vb = malloc(sizeof(VarBind))) == NULL) {
        ilmi_free_msgblk(msg);
        return;
    }

    msg->vblist = vb;
    tmpoid.oid_ptr = ilmiPollObject;
    tmpoid.length  = ilmiPollObjectlen;
    vb->name = CloneOID(&tmpoid);

    if(!vb->name) {
        ilmi_free_msgblk(msg);    /* This frees the vb also. */
        return;
    }


    msg->client_id = ILMI_ID;
    msg->ilmi_func = ilmiPoll;
    msg->transid = intfblk->intf_id;
    msg->intf_id = intfblk->intf_id;
    msg->retry_count = ILMI_POLL_RETRYS;
    msg->errcode = ilmiNoError;
    msg->response    = ilmiRespNoErr;
    msg->address_reg_flag = FALSE;
    msg->oper = ilmiGet;
    msg->encodedReq = NULL;

    /* Now add it to the input queue */
    enqueue(&ilmi_inputQ,msg);
}


/*-------------------------------------------------------------------------
 * This function is called whenever an interface goes down and a cold-start
 * trap has to sent down the associated UNI.
 */
boolean
ilmi_send_trap (uint intf, ushort type, int vpi, int vci, int status)
{
    uchar *buf=NULL;
    int len;
    static ilmi_msgblock msg;
    boolean ret;

    ret = TRUE;
    if( (ilmi_trap_encode(type,vpi,vci,status,&buf,&len)) == FALSE) {
        ilmi_log_error(ILMI_ID,ilmiReqEncodeError, "<ilmi_send_trap>");
        ret = FALSE;
	goto ilmi_send_trap_cleanup;
    }
    
    msg.intf_id = intf;
    msg.encodedReqLen = len;
    msg.encodedReq = buf;
    if( !(ilmi_send_pkt(&msg))) {
        ilmi_log_error(ILMI_ID,ilmiSendPduError,"<ilmi_send_trap>");
        ret = FALSE;
    }

ilmi_send_trap_cleanup:
    if(msg.encodedReq)
	free(msg.encodedReq);
    
    return (ret);

}


/* ------------------------------------------------------------------------- 
 * This function takes a pre-allocated sequence of bytes as data and forms
 * the pak struture before sending it out to the AAL5 encapsulation routine
 * for ILMI.
 */

boolean
ilmi_send_pkt (ilmi_msgblock *msg)
{
    uint size;
    paktype *pak;
    ilmi_intfblock *intfblk;


    size = msg->encodedReqLen;
    intfblk = ilmi_getintfblk (msg->intf_id);
    if(!intfblk) {
        ilmi_log_error(ILMI_ID,ilmiBadIntfBlock,"<ilmi_send_pkt>");
        return (FALSE);
    }

    /* Get a Buffer.  */
    pak = getbuffer(size);
    if (!pak) {
        return (FALSE);
    }
    pak->datagramsize = size;
    bcopy(msg->encodedReq,pak->datagramstart,size);
    pak->if_output = intfblk->idb->firstsw;
    /*
     * encap the packet  - the routine will return FALSE if the ATM hw
     * is not completely up and active.  In such a case just free the packet.
     */
    if ( !atm_ilmi_encap(pak) ) {
        if (atm_ilmi_debug)
            ilmi_log_error(ILMI_ID,ilmiEncapError,"<ilmi_send_pkt>");
        datagram_done(pak);
        return (FALSE);
    }
    /* get it out!! */
    datagram_out(pak);
    return (TRUE);
}



/* ------------------------------------------------------------------------
 * Event Related.
 */


/*
 * This function is called to create a periodic event such as to 
 * start polling on a Interface. Note here we do not check for the
 * existance pf the interface because when called from startup CLI
 * config. the interface may not have come up. Note the context 
 * field in the event structure is used only by the Request Retry
 * to store the reqid.
 */

boolean
ilmi_create_event (int intfnum, int evtype, int freq, int context)
{
    ilmi_eventblk *ev=NULL;

    switch (evtype) {
    case ilmiPeriodicPoll :
        freq = ((freq < ATM_ILMIPOLL_MIN) ? ATM_ILMIPOLL_MIN : freq);
        break;
    default:
        break;
    }

    ev=ilmi_getevent_fromQ(intfnum,evtype,context);
    if(ev)  {
        ilmi_disable_eventtimer(ev);
        ev->freq = freq;
    } else {
        ev = malloc(sizeof(ilmi_eventblk));
        if(!ev)
            return (FALSE);
        else
            enqueue(&ilmi_eventQ,ev);
    }
    ev->freq = freq;
    ev->type = evtype;
    ev->intf_id = intfnum;
    ev->context = context;
    ilmi_enable_eventtimer(ev);

    return (TRUE);
}

void
ilmi_destroyevent_forintf (ilmi_intfblock *intfblk)
{
    ilmi_eventblk *event;

    for(event=ilmi_eventQ.qhead; event;event=event->nxt) {
        if(event->intf_id == intfblk->intf_id) {
            ilmi_disable_eventtimer(event);
            unqueue(&ilmi_eventQ, event);
            free(event);
            return;
        }
    }
}

void
ilmi_destroy_event (int intfnum, int evtype, int context)
{
    ilmi_eventblk *ev = NULL;

    ev=ilmi_getevent_fromQ(intfnum, evtype, context);
    if(ev) {
        ilmi_disable_eventtimer(ev);
        unqueue(&ilmi_eventQ, ev);
        free(ev);
        return;
    }
}

ilmi_eventblk
*ilmi_getevent_fromQ (ilmiIntf intfnum, ilmiEventType evtype, ilmiEvContext context)
{
    ilmi_eventblk *event;
    for(event=ilmi_eventQ.qhead; event;event=event->nxt) {
        if((event->type == evtype) && (event->intf_id == intfnum)) {
            if(evtype == ilmiPeriodicReqRetry) {
                if(event->context == context)
                    return event;
                else
                    continue;
            } else {
                return event;
            }
        }
    }
    return (NULL);
}



/*--------------------------------------------------------------------------
 *  Managed Timer Handling.
 */


/*
 * Enable ILMI event timer.
 */
void
ilmi_enable_eventtimer (ilmi_eventblk *event)
{

    if (!event)
        return;

    mgd_timer_init_leaf(&event->m_timer, &ilmi_master_evtimer,
        0, event, TRUE);
    mgd_timer_start(&event->m_timer, event->freq);
}

/*
 * Disable ILMI event timer.
 */

void
ilmi_disable_eventtimer (ilmi_eventblk *event)
{
    if (event) {
        mgd_timer_stop(&event->m_timer);
    }
}


void
ilmi_periodic (void)
{
    ilmi_eventblk *ev;
    mgd_timer *expired_timer;
    ilmi_intfblock *intfblk=NULL;
    int reqid;
    ilmi_msgblock *msg;
    static boolean pass = FALSE;

    /*
     * Increase coarseness of the periodic call.
     */

    if(pass != TRUE) {
        pass = TRUE;
        return;
    }
    else
        pass = FALSE;


    while (mgd_timer_expired(&ilmi_master_timer)) {
        expired_timer = mgd_timer_first_expired(&ilmi_master_timer);
        ev = mgd_timer_context(expired_timer);
        if(!ev) {
	    if (atm_ilmi_debug)
		buginf("\nILMI: Expired Timer has no context. Ignoring");
            continue;
	}
        mgd_timer_stop(&ev->m_timer);


        switch (ev->type) {

        /*
         * ILMI Poll
         */
        case ilmiPeriodicPoll:
            mgd_timer_start(&ev->m_timer,ev->freq);
            ilmi_process_poll(ev->intf_id);
            break;


        /*
         * Outstanding Requests Retries.
         */
        case ilmiPeriodicReqRetry:
            reqid = ev->context;
            msg = ilmi_reqmsg_hashretreive(reqid);

	    
            if(!msg) {
                ilmi_destroy_event(ev->intf_id, ilmiPeriodicReqRetry,reqid);
		if (atm_ilmi_debug)
		    buginf("\nILMI: No request associated with Expired Timer Reqid = %d",
			   reqid);
                continue;
            }
	    
	    if (atm_ilmi_debug) {
		buginf("\nILMI: REQ_TIMEOUT ");
		ilmi_dump_msgblock(msg);
		buginf("\n ILMI Retry count (before decrement) = %d", msg->retry_count);
	    }

            if (msg->retry_count <= 0) {
                ilmi_destroy_event(ev->intf_id, ilmiPeriodicReqRetry,reqid);
                msg->errcode = ilmiNoResponse;
                msg->response    = ilmiRespGenErr;
                enqueue(&ilmi_responseQ,msg);
                continue;
            } else {
                /* Send it out again */
                msg->retry_count--;
                enqueue(&ilmi_inputQ,msg);
            }
            break;


        /*
         * Address Registration
         */
        case ilmiPeriodicAddressReg :
            intfblk = ilmi_getintfblk(ev->intf_id);
	    if (!intfblk) {
		ilmi_destroy_event(ev->intf_id, ilmiPeriodicAddressReg,0);
		continue;
            }
	    
	    if (!intfblk->idb->atm_db->address_reg) {
		intfblk->state = ilmiIntfUpAndNormal;
		if (!intfblk->initial_set_count) {
		    ilmi_destroy_event(ev->intf_id, ilmiPeriodicAddressReg,0);
		} else {
		    intfblk->initial_set_count--;
		    ilmi_process_intfRestart(intfblk);
		}
		continue;			
	    }


	    
	    if (intfblk->state == ilmiIntfUpAndNormal) {
		if (!ilmi_addrtraverse(ev->intf_id, NULL)) {
		    if(atm_ilmi_debug)
			buginf("\nILMI: Trap sent. Waiting for Prefix %s",
			ilmi_namestring_forintf(intfblk));

		    ilmi_delete_localdb(intfblk);
		    ilmi_send_trap(ev->intf_id,ilmiColdStart,0,0,ilmiOther);
		    mgd_timer_start(&ev->m_timer,ILMI_PREFIX_WAITTIME);
		} else {
		    ilmi_destroy_event(ev->intf_id, ilmiPeriodicAddressReg,0);
		}
		continue;
            } else {
                ilmi_process_intfRestart(intfblk);
            }
            break;

	case ilmiPeriodicPeerAddressCheck :
	    intfblk = ilmi_getintfblk(ev->intf_id);
	    mgd_timer_start(&ev->m_timer,ILMI_PEERADDRCHECK_TIMEOUT);
	    ilmi_sync_localpeertable (intfblk);
	    break;

        default :
            break;
        }
    }
}

void
init_ilmi_timers (void)
{
    mgd_timer_init_parent(&ilmi_master_timer, NULL);
    mgd_timer_init_parent(&ilmi_master_evtimer, &ilmi_master_timer);
    reg_add_onesec(ilmi_periodic, "ATM ILMI Periodic");

}
      
