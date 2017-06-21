/* $Id: ilmi_state.c,v 3.4.42.1 1996/03/18 19:00:03 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/atm/ilmi_state.c,v $
 *------------------------------------------------------------------
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_state.c,v $
 * Revision 3.4.42.1  1996/03/18  19:00:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.16.2  1996/03/07  08:30:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.16.1  1996/02/20  13:35:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/06  10:42:37  kartik
 * CSCdi44909:  memory leak in ilmi code
 * ILMI was holding on to the memory it allocated for trap encoding.
 *
 * Revision 3.3  1995/12/05  22:30:05  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI address registration will now ignore the selector byte.
 *
 * Revision 3.2  1995/11/17  08:47:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  20:31:48  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:12:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../snmp/snmp_api.h"
#include "ilmi_public.h"
#include "ilmi_private.h"


/*
 * The following static definitions pertain to specific objects
 * which are used for the ILMI address registration. Each array
 * name is descriptive of its purpose.
 */

static long ilmiAddressTableObjectID[]  = {1,3,6,1,4,1,353,2,6,1};
static long ilmiPrefixTableObjectID[] =   {1,3,6,1,4,1,353,2,7,1};
static int ilmiaddressobjecIDlen = 10;
static long ilmiatmfAtmLayerUniVersion[] = {1,3,6,1,4,1,353,2,2,1,1,9,0};
static long ilmiatmfMyIpNmAddress[]      = {1,3,6,1,4,1,353,2,1,2,0};
static long ilmiatmfPortMyIfName[]       = {1,3,6,1,4,1,353,2,1,1,1,7,0};


/*--------------------------------------------------------------------------
 * This function is called whenever a particular interface is restarted and
 * whenever it receives a cold-start trap on the given interface.
 * It performs the following functions.
 * 1.  Initialize the address table. The table will be the prefix table
 *     if the ILMI_TYPE is USERSIDE or the address table if it is the 
 *     NETWORKSIDE. This will zap all the rows for this intf.
 * 2.  Send a cold start trap over this UNI.
 * 3.  Send a GetNext of the address table on the other side.
 */

void
ilmi_initiate_addreg (ilmi_intfblock *intfblk)
{
    ilmi_msgblock *msg;
    VarBind *vb;
    OID tmpoid;

    if(!intfblk)
        return;

    ilmi_state_transit(intfblk->intf_id,intfblk->state,ilmiIntfAwaitRestartAck,"<ilmi_initiate_addreg>");
    intfblk->state = ilmiIntfAwaitRestartAck;

    /*
     * Send a cold start trap over this UNI.
     */

    if( (ilmi_send_trap(intfblk->intf_id,ilmiColdStart,0,0,ilmiOther)) == FALSE) {
        ilmi_state_transit(intfblk->intf_id,intfblk->state,ilmiIntfRestarting,"<ilmi_initiate_addreg>");
        intfblk->state = ilmiIntfRestarting;
        return;
    }


    /*
     * Send a GetNext to ensure that the peer-side address
     * table is initialized.
     */
    if ( (msg = malloc(sizeof(ilmi_msgblock))) == NULL) {
        return;
    }
    if( (vb = malloc(sizeof(VarBind))) == NULL) {
        ilmi_free_msgblk(msg);
        return;
    }

    msg->vblist      = vb;
    tmpoid.oid_ptr = ((ilmi_type == ILMI_USERSIDE) ? ilmiAddressTableObjectID : ilmiPrefixTableObjectID);
    tmpoid.length  = ilmiaddressobjecIDlen;

    vb->name = CloneOID(&tmpoid);
    if(!vb->name) {
        ilmi_free_msgblk(msg);
        return;
    }

    msg->client_id   = ILMI_ID;               /* ILMI Itself */
    msg->transid     = intfblk->intf_id;
    msg->intf_id     = intfblk->intf_id;
    msg->retry_count = ILMI_MAXREQRETRY;
    msg->errcode     = ilmiNoError;
    msg->response    = ilmiRespNoErr;
    msg->oper        = ilmiGetNext;
    msg->ilmi_func   = ilmiAddressTableCheck;
    msg->address_reg_flag = FALSE;
    msg->encodedReq  = NULL;

    enqueue(&ilmi_inputQ,msg); /* Now add it to the input queue */
    return;
}



/* UNI Version. */
/* Neighbors IP for NM purposes. */
/* Neighbors ifName for NM purposes. */

void
ilmi_initial_sets(ilmi_intfblock *intfblk)
{
    OID tmpoid;

    tmpoid.oid_ptr = ilmiatmfAtmLayerUniVersion;
    tmpoid.length  = 13;
    ilmi_send_initialreq (intfblk, &tmpoid, ilmiUniVersion);

    tmpoid.oid_ptr = ilmiatmfMyIpNmAddress;
    tmpoid.length  = 11;
    ilmi_send_initialreq (intfblk, &tmpoid, ilmiNeighborsIp);

    tmpoid.oid_ptr = ilmiatmfPortMyIfName;
    tmpoid.length  = 13;
    ilmi_send_initialreq (intfblk, &tmpoid, ilmiNeighborsIfName);

    return;
}

void
ilmi_send_initialreq (ilmi_intfblock *intfblk,OID *tmpoid,ilmiFunctionType type)
{
    ilmi_msgblock *msg;
    VarBind *vb;

    if ( (msg = malloc(sizeof(ilmi_msgblock))) == NULL) {
        return;
    }

    if( (vb = malloc(sizeof(VarBind))) == NULL) {
	free(msg);
        return;
    }

    vb->name = CloneOID(tmpoid);
    if(!vb->name) {
	free(msg);
	free(vb);
	return;
    }
    
    msg->ilmi_func   = type;
    msg->client_id   = ILMI_ID;               /* ILMI Itself */
    msg->transid     = intfblk->intf_id;
    msg->intf_id     = intfblk->intf_id;
    msg->retry_count = ILMI_MAXREQRETRY;
    msg->errcode     = ilmiNoError;
    msg->response    = ilmiRespNoErr;
    msg->oper        = ilmiGet;
    msg->encodedReq  = NULL;
    msg->address_reg_flag = FALSE;
    msg->vblist      = vb;

    enqueue(&ilmi_inputQ,msg); /* Now add it to the input queue */
    return;
}


void
ilmi_clear_addrTab (ilmi_intfblock *intfblk)
{
    ilmi_atmaddressRow *newrow, *prev, *node=NULL;
    ilmiIntf intf;

    intf = intfblk->intf_id;
    prev=NULL;
    node=ilmi_addressTabQ.qhead;
    while(node != NULL) {
        if (node->intf_id == intf)  {
            if (prev==NULL)
                ilmi_addressTabQ.qhead = node->nxt;
            else
                prev->nxt = node->nxt;
            newrow=node->nxt;
            if(node->addr)
                FreeOctetString(node->addr);
            free(node);
            node=newrow;
        } else {
            prev=node;
            node=node->nxt;
        }
    }
}



/* 
 * Check if the GetNext resulted in our falling off the end of the Table.
 * If we are not in the table anyymore we return a TRUE or else a FALSE.
 * Note the remote ILMI has to ensure that the view is contained within
 * the local interface. We do not check for that here.
 */
boolean
ilmiAddTableEmpty (ilmi_msgblock *msg)
{
    OID *ret ;
    OID sent;

    ret = msg->vblist->name;

    if(ret->length < 10)
        return (TRUE);

    sent.length = 10;
    sent.oid_ptr = ((ilmi_type == ILMI_USERSIDE) ? ilmiAddressTableObjectID:ilmiPrefixTableObjectID);

    if (CmpNOID(ret,&sent, 10) != 0)
        return (TRUE);
    else
        return (FALSE);
}
      
