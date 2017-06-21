/* $Id: ilmi_clientcb.c,v 3.4.6.2 1996/05/09 14:11:27 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/atm/ilmi_clientcb.c,v $
 *------------------------------------------------------------------
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_clientcb.c,v $
 * Revision 3.4.6.2  1996/05/09  14:11:27  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.6.1.18.1  1996/04/27  06:33:49  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.6.1.8.2  1996/04/10  23:35:47  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.4.6.1.8.1  1996/04/08  01:44:30  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.6.1  1996/03/18  18:59:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.10.1  1996/03/25  02:18:21  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4  1996/03/04  21:47:01  rbadri
 * CSCdi50537:  Router sends malformed OID in LECS address getnext request.
 * In getnext request to get the LECS address from the switch, specify
 * OID with and without length field(separate request) to work with
 * LS100/LS1010.
 *
 * Revision 3.3.28.2  1996/03/21  21:50:46  cakyol
 * Kartik's fix for "ILMI hung" problem picked up manually.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4  1996/03/04  21:47:01  rbadri
 * CSCdi50537:  Router sends malformed OID in LECS address getnext request.
 * In getnext request to get the LECS address from the switch, specify
 * OID with and without length field(separate request) to work with
 * LS100/LS1010.
 *
 * Revision 3.3  1995/12/05  22:29:51  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI address registration will now ignore the selector byte.
 *
 * Revision 3.2  1995/11/17  08:46:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/03  01:48:24  kartik
 * CSCdi41363:  ILMI should return NoError for re-registration of same
 * prefix.
 *
 * Revision 2.1  1995/06/07  20:12:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../snmp/snmp_api.h"
#include "sr_atmaddregmibdefs.h"
#include "ilmi_public.h"
#include "ilmi_private.h"


/*
 * Notify the clients about an address. If the second parameter clientblk
 * is NULL then all the clients are informed else only the specified
 * (clientblk) is informed.
 */

void
ilmi_notify_Address (ilmiClientNotifyType type, ilmi_clientblock *cblk, ilmiIntf intf_id,OctetString *address)
{
    ilmi_clientblock *cb;
    ilmiClientReq notify;
    ilmi_intfblock *intfblk;

    if((type != ilmiAddressAdd) && (type != ilmiAddressDel)) {
        ilmi_log_error(ILMI_ID,ilmiBadNotifyType, "<ilmi_process_input>");
        return;
    }

    intfblk = ilmi_getintfblk(intf_id);
    notify.notify_type = type;
    notify.transid = 0;         /* No request associated */
    notify.error   = ILMI_OK;   /* No request associated */
    notify.address = address;

    /*
     * The clientblk true indicates cases such as when a client has just 
     * registered would like to know about all the existing addresses. 
     * Since these addresses are already in the table only the 
     * 'ilmiAddressAdd' notify makes sense here.
     */
    if(cblk != NULL) {
        notify.client_id = cblk->client_id;
        notify.intf_id   = intf_id;
        if( (type == ilmiAddressAdd) && (cblk->address_add)) {
	    if (atm_ilmi_debug) {
		buginf("\nILMI: Notifying Address Addition ");
		ilmi_print_address(address);
		buginf(" (%s)", ilmi_namestring_forintf(intfblk));
	    }
            cblk->address_add(&notify);
	}
    }
    else {
        for(cb=ilmi_clientQ.qhead; cb; cb=cb->nxt) {
            notify.client_id = cb->client_id;
            notify.intf_id   = intf_id;
            if((type == ilmiAddressAdd) && (cb->address_add)) {
		if (atm_ilmi_debug) {
		    buginf("\nILMI: Notifying Address Addition ");
		    ilmi_print_address(address);
		    buginf(" (%s)", ilmi_namestring_forintf(intfblk));
		}
		cb->address_add(&notify);
		
	    } else if ((type == ilmiAddressDel) && (cb->address_del)) {
		if (atm_ilmi_debug) {
	            buginf("\nILMI: Notifying Address Deletion");
		    ilmi_print_address(address);
	            buginf(" (%s)", ilmi_namestring_forintf(intfblk));
		}
                cb->address_del(&notify);
	    }
        }
    }
    return;
}



/* --------------------------------------------------------------------
 * Validate the address based on the table definition which means an 
 * address can be associated with only one interface but the the 
 * interface itself can have multiple addresses  associated with it. 
 * If this is the user-side then the Network prefix table is kept else 
 * the atm address  table is kept. In addition the clients are also
 * consulted if they are registered for validation.
 */

uint
ilmi_validate_address (ilmiIntf intf, OctetString *address, int state)
{
    ilmi_atmaddressRow *address_row ;
    ilmi_clientblock *clientblk;
    ilmiClientReq clq;


    address_row = ilmi_getrow_addressTab (intf,address);
    switch(state) {
    case D_atmfAddressStatus_valid:
        if(address_row)
            return (ILMI_OK);
        break;
    case D_atmfAddressStatus_invalid:
        if(!address_row)
            return (ILMI_ERROR);
        break;
    default:
        return (ILMI_ERROR);
        break;
    }

    clq.notify_type = ilmiAddressValidate;
    clq.address     = address;
    clq.intf_id     = intf;

    /* 
     * Send for validation to all the clients who have registered a 
     * validation  callback.
     */
    for (clientblk=ilmi_clientQ.qhead; clientblk; clientblk=clientblk->nxt) {
        if (clientblk->address_valid) {
            clientblk->address_valid(&clq);
            if( clq.error == ILMI_ERROR)
                return (ILMI_ERROR);
        }
    }
    return (ILMI_OK);
}




/*--------------------------------------------------------------------------
 * Notify the clients about Signalling version
 */

void
ilmi_notify_universion (ilmi_intfblock *intfblk, int  version)
{
    ilmiClientReq notify;
    ilmi_clientblock *clientblk;


    if (!intfblk)
        return;

    notify.notify_type = ilmiUniVersionNotify;
    notify.transid     = 0;         /* No request associated */
    notify.error       = ILMI_OK;   /* No request associated */
    /*
    notify.client_id   = intfblk->intf_id;
    */
    notify.intf_id   = intfblk->intf_id;
    notify.notifyCB    = NULL;
    notify.vblist      = NULL;
    notify.address     = NULL;

    /* Need to check this. */
    notify.myUniVersion = version;

    for (clientblk=ilmi_clientQ.qhead; clientblk; clientblk=clientblk->nxt) {
        if (clientblk->universion_rcv) {
            clientblk->universion_rcv(&notify);
        }
    }
}


/*
 * LECS Address Fetch
 *
 * This is the handler for responses received for the getnext conducted
 * on the Service Registry table for LECS addresses. 
 */

/* Table Object */
static ulong ilmiSrvcRegTabObjectName[] = { 1, 3, 6, 1, 4, 1, 353, 2, 8, 1, 1, 3};
static OID ilmiSrvcRegTabObjectID  =
{
     length:     sizeof(ilmiSrvcRegTabObjectName)/sizeof(ulong),
     oid_ptr:    ilmiSrvcRegTabObjectName
};


/* The Correct LECS OID */
static ulong ilmiLecsSrvcRegObjectName[] = {1, 3, 6, 1, 4, 1, 353, 2, 8, 1, 1, 3,
				  0,
				  10, 1, 3, 6, 1, 4, 1, 353, 1, 5, 1};

static OID ilmiLecsSrvcRegObjectID  =
{
     length:     sizeof(ilmiLecsSrvcRegObjectName)/sizeof(ulong),
     oid_ptr:    ilmiLecsSrvcRegObjectName
};

/* The Incorrect LECS OID which some vendors are using */
static ulong ilmiALTiLecsSrvcRegObjectName[] = {1, 3, 6, 1, 4, 1, 353, 2, 8, 1, 1, 3,
				     0,
				     1, 3, 6, 1, 4, 1, 353, 1, 5, 1};

static OID ilmiALTiLecsSrvcRegObjectID =
{
     length:    sizeof(ilmiALTiLecsSrvcRegObjectName)/sizeof(ulong),
     oid_ptr:   ilmiALTiLecsSrvcRegObjectName
};

int
ilmi_lecs_response (ilmi_clientblock *ilmi_client, ilmi_msgblock *msg)
{
    OID *incoming;
    boolean send_to_client = TRUE;
    int ret_code;
    ilmiClientReq client_req;
    ilmiResponseType  response_code;
    ilmiErrorType  error_code;
    ilmi_intfblock *intfblk=NULL;

    /* Init */
    send_to_client = TRUE;
    response_code  = msg->response;
    error_code     = msg->errcode;
    ret_code       = ILMI_OK; 
    incoming       = msg->vblist->name;
    intfblk        = ilmi_getintfblk(msg->intf_id);

    /* Is the response valid */
    if ((error_code != ilmiResponseReceived) || (response_code  != ilmiRespNoErr)) {

	    ret_code      = ILMI_ERROR;                   /* Terminate Getnext walk */

    } else if ((CmpNOID(incoming, &ilmiSrvcRegTabObjectID, ilmiSrvcRegTabObjectID.length)) > 0) {

	   /* Is it within bounds */

	   response_code = ilmiRespNoSuchNameErr;        /* Out of bounds */
	   ret_code      = ILMI_ERROR;                   /* Terminate Getnext walk */


    } else {

	  /* The getnext response appears to be valid */

	    ret_code   = ILMI_OK;

	    /* If we do not match both the 'wrong' and the right OID
	     * then continue searching and do not inform the client.
	     */
	    if (((CmpNOID(incoming, &ilmiLecsSrvcRegObjectID, ilmiLecsSrvcRegObjectID.length)) !=0) &&
	        ((CmpNOID(incoming, &ilmiALTiLecsSrvcRegObjectID,
              ilmiALTiLecsSrvcRegObjectID.length)) != 0)) {
	        send_to_client = FALSE;
		buginf("\nILMI: GetNext Response within bounds but no match. (%s)",ilmi_namestring_forintf(intfblk));

	    }

	    /* We continue with our getnext walk */
	    msg->retry_count = ILMI_MAXREQRETRY;
	    if (msg->encodedReq) {
	        free(msg->encodedReq);
	        msg->encodedReq = NULL;
	        msg->encodedReqLen = 0;
	    }

	    if (atm_ilmi_debug)
		buginf("\nILMI Continuing Getnext processing");
	    enqueue(&ilmi_inputQ,msg);
    }


    /* Build the client request */
    if ((send_to_client == TRUE) && (ilmi_client->response_notify)) {
	    client_req.client_id     = msg->client_id;
	    client_req.intf_id       = msg->intf_id;
	    client_req.vblist        = msg->vblist;
	    client_req.error         = error_code;
	    client_req.response      = response_code;
	    client_req.transid       = msg->transid;
	    client_req.notify_type   = ilmiResponseNotify;
	    client_req.address       = NULL;
	    client_req.notifyCB      = NULL;
    
        if (atm_ilmi_debug) {
            if (ret_code == ILMI_OK) {
                buginf("\nILMI: Delivering LECS Address to Client "); 
                ilmi_print_address(msg->vblist->value.u.osval);
            } else {
               buginf("\nILMI: End of LECS address search received from peer");
            }

        }
    	
	    (ilmi_client->response_notify)(&client_req);

    }

    if (atm_ilmi_debug) {
	buginf("\nILMI: GETNEXTCOMPLETE ");
	ilmi_dump_msgblock(msg);
    }
    return (ret_code);
    
}












      
