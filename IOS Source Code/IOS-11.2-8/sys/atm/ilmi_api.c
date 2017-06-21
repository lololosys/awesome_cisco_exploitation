/* $Id: ilmi_api.c,v 3.3.42.2 1996/05/09 14:11:25 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/atm/ilmi_api.c,v $
 *------------------------------------------------------------------
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_api.c,v $
 * Revision 3.3.42.2  1996/05/09  14:11:25  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.42.1.18.1  1996/04/27  06:33:48  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.42.1.8.2  1996/04/10  23:35:45  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.42.1.8.1  1996/04/08  01:44:28  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.42.1  1996/03/18  18:59:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.2  1996/03/07  08:30:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.16.1  1996/02/20  13:35:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3.46.1  1996/03/25  02:18:19  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.28.1  1996/03/21  21:50:44  cakyol
 * Kartik's fix for "ILMI hung" problem picked up manually.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3  1995/12/05  22:29:49  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI address registration will now ignore the selector byte.
 *
 * Revision 3.2  1995/11/17  08:46:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  20:31:44  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:12:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../snmp/snmp_api.h"
#include "ilmi_public.h"
#include "ilmi_private.h"
#include "ilmi_api.h"


/* ------------------------------------------------------------------------ */
/*
 *                            T H E  I L M I  A P I
 */
/* ------------------------------------------------------------------------ */


/*
 * This routine registers a client with the ILMI. It assigns and returns a
 * client id to the caller. It also adds the client to its record.
 */


ilmiClient 
ilmi_register_client (char *name)
{
    ilmi_clientblock *newclient;

    if ( (newclient = malloc(sizeof(ilmi_clientblock))) == NULL) {
        return ILMI_ERROR;
    }
    client_count++;
    newclient->client_id = client_count;
    newclient->name      = strdup(name);
    enqueue(&ilmi_clientQ,newclient);
    return client_count;
}





/* ------------------------------------------------------------------------
 * This function is called by the Clients to register callbacks for different
 * ILMI services.
 */

int
ilmi_register_service (ilmiClientReq *client_req)
{
    ilmi_clientblock *clientblk;
    ilmi_atmaddressRow *add;
    int err = 0;

    if((!client_req) || 
        (!client_req->notifyCB) || 
        (client_req->client_id <= ILMI_ID))
        err = 1;

    clientblk = ilmi_getclientblk(client_req->client_id);
    if (!clientblk)
        err = 1;

    if(err) {
        ilmi_log_error(ILMI_ID,ilmiIllegalReq, "<ilmi_register_service>");
        return ILMI_ERROR;
    }

    switch(client_req->notify_type) {
    case ilmiAddressAdd :
        clientblk->address_add = client_req->notifyCB;
        /* 
             * Now inform the client immediately of any 
             * address already registered 
             */
        for(add=ilmi_addressTabQ.qhead; add; add=add->nxt) {
            if(add->intf_id == client_req->intf_id) {
                ilmi_notify_Address(ilmiAddressAdd,clientblk,add->intf_id,add->addr);
            }
        }
        break;
    case ilmiAddressDel :
        clientblk->address_del = client_req->notifyCB;
        break;
    case ilmiAddressValidate :
        clientblk->address_valid = client_req->notifyCB;
        break;
    case ilmiTrapNotify :
        clientblk->trap_notify  = client_req->notifyCB;
        break;
    case ilmiResponseNotify :
        clientblk->response_notify = client_req->notifyCB;
        break;
    case ilmiUniVersionNotify:
        clientblk->universion_rcv = client_req->notifyCB;
        break;
   default :
        if (atm_ilmi_debug)
            buginf("\nILMI: Illegal request type <ilmi_register_service>");
        return ILMI_ERROR;
        break;
    }
    return ILMI_OK;
}


/* -------------------------------------------------------------------------
 * This function is called by the Client to send a request to the adjacent
 * UME.
 */

int
ilmi_client_request (ilmiClientReq *req)
{
    ilmi_msgblock *msg;

    if( (req == NULL) ||
        (req->vblist == NULL) ||
        (req->client_id <= ILMI_ID) ) {
        ilmi_log_error(ILMI_ID,ilmiBadClientReq, "<ilmi_client_request>");
        return ILMI_ERROR;
    }

    /*
     * Is the specified interface up.
     */
    if (!(ilmi_getintfblk (req->intf_id))) {
        ilmi_log_error(req->client_id,ilmiBadIntfBlock,"<ilmi_client_request>");
        return ILMI_ERROR;
    }


    if ( (msg = malloc(sizeof(ilmi_msgblock))) == NULL) {
        return ILMI_ERROR;
    }
    msg->client_id   = req->client_id;
    msg->retry_count = ILMI_MAXREQRETRY;
    msg->intf_id     = req->intf_id;
    msg->transid     = req->transid;
    msg->ilmi_func   = ilmiReqOther;
    msg->errcode     = ilmiNoError;
    msg->response    = ilmiRespNoErr;
    msg->oper        = req->oper_type;
    msg->address_reg_flag = FALSE;
    msg->vblist      = CloneVarBindList(req->vblist);
    msg->encodedReq  = NULL;

    /* Note that we only allow one varbind per-request. */
    /* Now add it to the input queue */
    if (atm_ilmi_debug) {
	buginf("\nILMI: REQRCVD ");
	ilmi_dump_msgblock(msg);
    }
    enqueue(&ilmi_inputQ,msg);
    return (ILMI_OK);
}


/*  -------------------------------------------------------------------------
  * This is a direct call made by the Client to find out the status of a link 
  * across a UNI. It returns a TRUE if the link is up or else it returns a 
  * zero. The status is based on the ILMI polling and is declared down only
  * if the previous 'ILMI_DEFPOLLHISTORY' consecutive responses were 'down 
  * responses. This function may have been rendered redundant with the 
  * introduction of the linkdown notification. 
  */

boolean
ilmi_get_linkstatus (ilmiIntf intf)
{
    ilmi_intfblock *intfblk=NULL;
    int ii;
    boolean status;

    status = FALSE;
    if(!(intfblk = ilmi_getintfblk(intf))) {
        ilmi_log_error(ILMI_ID,ilmiBadIntfBlock,"<ilmi_get_linkstatus>");
        return (FALSE);
    }

    for(ii=0; ii<ILMI_DEFPOLLHISTORY; ii++) {
        if(intfblk->history[ii] == TRUE) {
            status = TRUE;
            break;
        }
    }
    return status;
}


/* -------------------------------------------------------------------------
  * This function allows a Client to traverse through the address table.
  * The Clients should not modify the returned memory address or the 
  * contents of the memory address. If the specified address is NULL
  * the first address (prefix/host-address) is returned.
  */

OctetString
*ilmi_addrtraverse(ilmiIntf intf, OctetString *prev_add)
{
    ilmi_atmaddressRow *add;
    int match=0;

    if(!intf)
        return (NULL);

    for(add=ilmi_addressTabQ.qhead,match=0; add; add=add->nxt) {
        if(add->intf_id != intf)
            continue;

        if( (!prev_add) || (match == 1) )
            return (add->addr);

        if(add->addr == prev_add)
            match = 1;
    }
    return (NULL);
}



void
ilmi_unregister_client (ilmiClient client_id)
{
    ilmi_clientblock *clientblk;

    clientblk =  ilmi_getclientblk(client_id);
    if(!clientblk)
        return;
    unqueue(&ilmi_clientQ,clientblk);
    if(clientblk->name)
        free(clientblk->name);
    free(clientblk);
    return;
}








      
