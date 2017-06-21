/* $Id: ilmi_util.c,v 3.5.36.8 1996/08/28 16:55:27 kartik Exp $
 * $Source: /release/112/cvs/Xsys/atm/ilmi_util.c,v $
 *------------------------------------------------------------------
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_util.c,v $
 * Revision 3.5.36.8  1996/08/28  16:55:27  kartik
 * CSCdi50802:  ILMI manages to register un-registrable address with switch
 * Branch: California_branch
 *
 * Revision 3.5.36.7  1996/08/22  20:12:39  kartik
 * CSCdi63616:  Lock ilmi address structure while accessing it in the
 * 'show' cmd.
 * Branch: California_branch
 *
 * Revision 3.5.36.6  1996/07/10  22:20:47  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.5.36.5  1996/06/19  03:49:18  kartik
 * CSCdi60753:  CSCdi50421 breaks images which do not include clns.
 * Branch: California_branch
 *
 * Revision 3.5.36.4  1996/06/18  06:31:04  kartik
 * Branch: California_branch
 * CSCdi50421:  show atm ilmi command returns error when no interface
 * provided
 *              Added more details to show.
 *
 * Revision 3.5.36.3  1996/06/04  00:20:21  snyder
 * CSCdi59389:  spellink errors
 * Branch: California_branch
 *
 * Requestor -> Requester
 *
 * Revision 3.5.36.2  1996/05/09  14:11:34  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.36.1.18.1  1996/04/27  06:33:57  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.36.1.8.1  1996/04/08  01:44:37  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.36.1  1996/03/18  19:00:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.2  1996/03/07  08:30:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.12.1  1996/02/20  13:35:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5.40.1  1996/03/25  02:18:32  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.22.1  1996/03/21  21:50:55  cakyol
 * Kartik's fix for "ILMI hung" problem picked up manually.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1995/12/19  20:25:11  kartik
 * CSCdi45739:  ILMI deregisters address when it should still be registerd
 * Set flag to not update peer when not in local table.
 *
 * Revision 3.4  1995/12/11  19:02:36  kartik
 * CSCdi45345:  Memory corruption crashes from ILMI, probably due to
 * CSCdi42922
 * Fixed a Bad Malloc.
 *
 * Revision 3.3  1995/12/05  22:30:07  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI address registration will now ignore the selector byte.
 *
 * Revision 3.2  1995/11/17  08:47:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/03  01:48:25  kartik
 * CSCdi41363:  ILMI should return NoError for re-registration of same
 * prefix.
 *
 * Revision 2.4  1995/08/08  05:33:33  schrupp
 * CSCdi38341:  Dont cancel ATM NSAP if never registered
 *
 * Revision 2.3  1995/06/19  21:28:10  kartik
 * CSCdi36096:  Fixed mallocs in ILMI which were not being checked for
 * return
 * code.
 *
 * Revision 2.2  1995/06/18  20:31:49  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:12:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "address.h"
#include "../snmp/snmp_api.h"
#include "interface_private.h"
#include "../if/atm.h"
#include "sr_atmaddregmib.h"
#include "sr_atmaddregmibpart.h"
#include "../util/nsap.h"
#include "../clns/clns.h"
#include "ilmi_public.h"
#include "ilmi_private.h"
#include "ilmi_api.h"

/*
 * The hash table for the outstanding requests as they await response
 * from the remote entity.
 */
ilmi_msgblock  *ilmi_requestHashTable[ILMI_REQUEST_HASHSIZE];
boolean ilmi_localtable_update;
static void ilmi_display_perif(ilmi_intfblock *);
static void ilmi_print_prefix(OctetString *);
static ilmi_atmaddressRow *ilmi_getrow_foraddress(queuetype, OctetString *, int);



/* --------------------------------------------------------------------- */
/*
 * Given a client_id this function retreives the corresponding client
 * block from the client queue.
 */

ilmi_clientblock
*ilmi_getclientblk (ilmiClient client)
{
    ilmi_clientblock *node ;

    for(node=ilmi_clientQ.qhead; node ; node=node->nxt) {
        if(node->client_id == client)
            return node;
    }
    return NULL ;
}


/* --------------------------------------------------------------------- */
/*
 * Given a interface id this function retreives the corresponding interface
 * block from the interface queue.
 */

ilmi_intfblock
*ilmi_getintfblk (ilmiIntf intf)
{
    ilmi_intfblock *node ;

    for(node=ilmi_intfQ.qhead; node; node=node->nxt) {
        if(node->intf_id == intf)
            return node ;
    }
    return NULL ;
}


/* ---------------------------------------------------------------------
 * Destroy the interface block.
 */
void
ilmi_destroy_intfblk (ilmi_intfblock *intfblk)
{
    ilmi_intfblock *node;

    for(node=ilmi_intfQ.qhead; (node && (node != intfblk)); node=node->nxt)
        ;

    if(node) {
        ilmi_destroyevent_forintf(node);
        unqueue(&ilmi_intfQ,node);
        free(node);
        return;
    }
}


/* -----------------------------------------------------------------------
 * Free the message block 
 * Note that the library free routine checks for Nil pointers so we are
 * not going to, unless we dereference the pointer.
 */

void
ilmi_free_msgblk (ilmi_msgblock *msg)
{
    /*
     * Free VarBind First.
     */
    if(!msg)
        return;

    if(msg->vblist) {
        FreeVarBind(msg->vblist);
    }

    if(msg->encodedReq) {
        free(msg->encodedReq);
    }

    free(msg);
}


int
snmp_set_atmaddReg (ilmiIntf intf,OctetString *addr, ilmiAddrState action)
{
    ilmi_atmaddressRow *addrow=NULL;
    boolean sendtoclient=TRUE;

    if(action == ilmiAddressInValid) {
        addrow = ilmi_getrow_addressTab(intf,addr);
        if(!addrow)
            return (NO_CREATION_ERROR);
        else {
	    if (atm_ilmi_debug) {
		buginf("\nILMI: Prefix Deleted : ");
		ilmi_print_address(addr);
            }
            unqueue (&ilmi_addressTabQ, addrow);
            FreeOctetString(addrow->addr);
	    addrow->addr = NULL;
            free(addrow);

	    /*
	     * Do not delete the local and remote address tables
	     * at this time. We rely on our clients to deregister
	     * and these tables will be cleaned up for this prefix
	     * at that time.
	     */
	    
            ilmi_notify_Address(ilmiAddressDel,NULL,intf,addr);
	    /*
             * Check if the prefix was the last one for this
             * interface.
             */
            if (!ilmi_addrtraverse(intf, NULL)) {
                ilmi_create_event(intf, ilmiPeriodicAddressReg,
                                        ILMI_PREFIX_WAITTIME,0);
            }
        }
    }
    else {
        if((ilmi_add_addressTab(intf, addr, &sendtoclient)) == ILMI_ERROR)
            return (RESOURCE_UNAVAILABLE_ERROR);
        else {
	    if (sendtoclient == TRUE) {
		ilmi_notify_Address (ilmiAddressAdd,NULL,intf,addr);
		
	    } else if (atm_ilmi_debug) {
		    buginf("\nILMI: Duplicate Prefix. Not sent to client:");
		    ilmi_print_address(addr);
		    
            }
	}
    }

    return (NO_ERROR);
}




/* ------------------------------------------------------------------------ 
 *
 * This routine inserts a given address into the address table.  
 * Lexicographic ordering is done on first the port and then the 
 * address basis. This table does not care whether the address
 * is the prefix or the whole atm address. This is because at any
 * one time only one is supported and this check is done in the
 * method routine.
 */

int
ilmi_add_addressTab (ilmiIntf intf, OctetString *addr, boolean *sendtoclient)
{
   ilmi_atmaddressRow *newrow, *prev, *node=NULL ;
   

   /* Init */
    *sendtoclient = FALSE;
   
   /*
    * If the address (prefix or ATM address) is already registered
    * then return NO_ERROR i.e. pretend the set was carried through.
    */
    if (ilmi_getrow_addressTab(intf,addr)) {
	if (atm_ilmi_debug) {
	    buginf("\nILMI: Prefix Exists : ");
	}

	return (ILMI_OK);
    }

    if ( (newrow = malloc(sizeof(ilmi_atmaddressRow))) == NULL) {
        return (ILMI_ERROR);
    }

    newrow->nxt=NULL ;
    newrow->intf_id=intf ;
    newrow->type = ((ilmi_type == ILMI_USERSIDE) ? ilmiPrefixAddress:ilmiHostddress);
    newrow->status = ilmiAddressValid;
    newrow->addr = CloneOctetString(addr);


    for(prev=NULL,node=ilmi_addressTabQ.qhead; node;
        prev=node,node=node->nxt) {

        if(node->intf_id < intf)
            continue;
        if( (node->intf_id == intf) && (ilmi_CmpAddress(node->addr,addr)<0))
            continue;
        else 
            break ;
    }

   /*
    *prev maybe NULL in which case it is added at the head.
    */
   insqueue(&ilmi_addressTabQ, newrow, prev);
   *sendtoclient = TRUE;
   return (ILMI_OK);
}



/* -----------------------------------------------------------------------
 * This function returns a row from the address table (Prefix if user-side
 * or else the atm address table) which corrsponds to the interface number
 * and address provided.
 */

ilmi_atmaddressRow
*ilmi_getrow_addressTab (ilmiIntf intf, OctetString *addr)
{
    ilmi_atmaddressRow *node=NULL;

    for (node=ilmi_addressTabQ.qhead; node; node=node->nxt) {
        if ((node->intf_id == intf) && 
            (ilmi_CmpAddress(node->addr,addr) == 0))
            return node ;

    }
    return (NULL);
}



/* -----------------------------------------------------------------------
 * Search the address table for the exact specified index or return the
 * the next one, depending on the search criterion.
 */

ilmi_atmaddressRow
*ilmi_search_addrTab (ilmiIntf intf, OctetString *addr, int searchtype)
{

    if(!intf)     /* Will never happen */
        return NULL;

    if(searchtype == EXACT)
        return (ilmi_getrow_addressTab (intf,addr));
    else 
        return (ilmi_getnext_addressTab(intf,addr));

}

/*
 * ILMI GetNext.
 */

ilmi_atmaddressRow
*ilmi_getnext_addressTab (ilmiIntf intf, OctetString *addr)
{
    ilmi_atmaddressRow *nd=NULL;

    if (addr->length == 0) {
        for (nd=ilmi_addressTabQ.qhead; nd; nd=nd->nxt) {
            if (nd->intf_id == intf)
                return (nd);
        }
    }
    else {
        for (nd=ilmi_addressTabQ.qhead; nd; nd=nd->nxt) {
            if(nd->intf_id < intf)
                continue;

            if((nd->intf_id == intf)&&(ilmi_CmpAddress(nd->addr,addr)<0))
                continue;
            else
                break;
        }

        /* We ran out */
        if(!nd)
            return NULL;

        /* Exact found, but can we return the next one ?? */
        if((nd->intf_id == intf) && (ilmi_CmpAddress(nd->addr,addr) == 0)) {
            if( (!nd->nxt) || (nd->nxt->intf_id != intf))
                return NULL;
            else
                return (nd->nxt);
        }

        /* We went past */
        if(nd->intf_id != intf)
            return NULL;
    }
    return (NULL);
}



/* ------------------------------------------------------------------------
 * Initialize the poll data block before starting a new epoch of polling.
 * This needs to be called definitely after any of the poll parameters
 * are modified including through the CLI
 */

void
ilmi_init_polldata ()
{
    ilmi_intfblock *intfblk ;
    int ii ;

    for(intfblk=ilmi_intfQ.qhead; intfblk; intfblk=intfblk->nxt) {
        intfblk->curindex = 0 ;
        for(ii=0;ii<ILMI_MAXPOLLHISTORY;ii++)
            intfblk->history[ii] = FALSE ;
    }
}




/* ------------------------------------------------------------------------
 * Update the poll block. Assign the current status to the slot pointed
 * to by the current index and then increment the current index.
 */

void
ilmi_update_polldata (uint intfnum, ilmiErrorType status)
{
    int index ;
    ilmi_intfblock *intfblk ;
    ilmi_clientblock *cb;
    ilmiClientReq notify;
    boolean val;
    int ii;

    if((intfblk = ilmi_getintfblk (intfnum)) == NULL)
        return;

    index = intfblk->curindex % ILMI_DEFPOLLHISTORY ;
    if(index < ILMI_MAXPOLLHISTORY) {
        val = (status == ilmiResponseReceived ? TRUE : FALSE);
        intfblk->history[index] = val ;
        intfblk->curindex += 1 ;
    }

    /*
     * Check if Link Down notify has to be generated.
     */

    if(status == TRUE)
        return;
    for(ii=0; ii<ILMI_DEFPOLLHISTORY; ii++) {
        if(intfblk->history[ii] == TRUE)
            return;
    }

    /*
     * Sure Enough !
     */

    notify.notify_type = ilmiLinkDownNotify;
    notify.transid     = 0;         /* No request associated */
    notify.error       = ILMI_OK;
    notify.intf_id   = intfnum;

    for(cb=ilmi_clientQ.qhead;cb;cb=cb->nxt) {
        notify.client_id = cb->client_id;
        if(cb->linkdown)
            cb->linkdown(&notify);
    }
    return ;
}


/* -----------------------------------------------------------------------
 * Compare two ATM addresses (octet strings) and return :
 * They can either be prefixes or full atm address.
 *
 * 0  if O1 == O2
 * -1 if O1 < O2
 * 1  if O1 > O2
 */

int 
ilmi_CmpAddress (OctetString *A1,OctetString *A2)
{
    uchar *a1ptr,*a2ptr;
    int a1len, a2len, len1,len2;

    a1ptr = A1->octet_ptr;
    a2ptr = A2->octet_ptr;
    a1len = A1->length;
    a2len = A2->length;

    for(len1=0,len2=0; (len1 < a1len) && (len2 < a2len); len1++,len2++ ) {
        if(*(a1ptr+len1) == *(a2ptr+len2))
            continue;
        else    
            return ((*(a1ptr+len1) > *(a2ptr+len2)) ? 1 : -1);
    }

    /* Both are equal */
    if(a1len == a2len)
        return 0;

    /* Not Likely in our case as the lengths will be the same. */
    return ((a1len > a2len) ? 1 : -1);
}


/*
 * Check if the ILMI should conduct address registration on a interface.
 */
boolean
ilmi_validate_intf (ilmi_intfblock *intfblk)
{

    if (intfblk->idb->atm_db->ilmi_vc <=  0)
        return (FALSE);

    if (intfblk->state != ilmiIntfRestarting)
        return (FALSE);

    if (!atm_interface_up(intfblk->idb->firstsw))
        return (FALSE);

    /* Port is up so go ahead negotiate addresses. */
    return (TRUE);
}

char
*ilmiMakeStrFromOstr (OctetString *ostr)
{
    char *str;

    if(!ostr->length)
        return NULL;
    str = malloc(ostr->length+1);
	if(!str)
	   return (NULL);
    bcopy(ostr->octet_ptr,str,ostr->length);
    str[ostr->length] = '\0';
    return(str);
}


/* ----------------------------------------------------------------- 
 * The following two routines relate to the hash implementation for the
 * outstanding requests queue. The first one adds the  message to the
 * request queue based on its reqid. The second routine extracts a 
 * message based on its request id.
 */

void
ilmi_reqmsg_hashenter (ilmi_msgblock *msg)
{
    int index = 0;

    if(!msg)
        return;
    index =  (msg->req_id % ILMI_REQUEST_HASHSIZE);
    msg->hash_next = ilmi_requestHashTable[index];
    ilmi_requestHashTable[index] = msg;
}

ilmi_msgblock
*ilmi_reqmsg_hashretreive (int reqid)
{
    int index=0;
    ilmi_msgblock *msg=NULL, **prev;

    if (reqid <= 0)
	return (NULL);
    
    index =  (reqid % ILMI_REQUEST_HASHSIZE);
    prev = &ilmi_requestHashTable[index];

    if (!prev)
	return (NULL);

    msg=*prev;
    while(msg) {
        if(msg->req_id == reqid) {
            *prev = msg->hash_next;
            msg->hash_next = NULL;
            return (msg);
        }
        prev = &msg->hash_next;
        msg = msg->hash_next;
    }
    return (NULL);
}
/* ----------------------------------------------------------------- */





/* -----------------------------------------------------------------------
 * Error Routine.
 */

void
ilmi_log_error (ilmiClient client_id,ilmiErrorType errtype, char *locstr)
{
    char name[CLIENTNAMESIZE];
    ilmi_clientblock *clientblk;

    if(client_id == ILMI_ID)
        strcpy(name,"ILMI");
    else {
        clientblk = ilmi_getclientblk(client_id);
        if( (!clientblk) || (!clientblk->name) )
            strcpy(name,"ILMI");
        else
            strcpy(name,clientblk->name);
    }

    if(!atm_ilmi_debug)
        return;

    switch(errtype) {
    case ilmiReqEncodeError:
        buginf("\n%s: Unable to encode ILMI Pdu %s",locstr);
        break;
    case ilmiSendPduError:
        buginf("\n%s: Unable to Send Pdu out %s",name,locstr);
        break;
    case ilmiBadClientReq:
        buginf("\n%s: Illegal Request received from Client %s",name,locstr);
        break;
    case ilmiBadIntfBlock:
        buginf("\n%s: Illegal or Inactive Interface Block %s",name,locstr);
        break;
    case ilmiBadNotifyType:
        buginf("\n%s: Illegal Client notification type %s",name,locstr);
        break;
    case ilmiIllegalTrapType:
        buginf("\n%s: Traptype is illegal or not supported %s",name,locstr);
        break;
    case ilmiAddrDupError:
        buginf("\n%s: Address is already registered %s",name,locstr);
        break;
    case ilmiEncapError:
        buginf("\n%s: Encapsulation error on o/g ILMI Pdu %s",name,locstr);
        break;
    case ilmiNoResponse:
        buginf("\n%s: No Response received %s",name,locstr);
        break;
    case ilmiBadObject:
        buginf("\n%s: Bad SNMP Object %s",name,locstr);
        break;
    case ilmiLocalAddFailure:
        buginf("\n%s: Unable to add to local address database %s",name,locstr);
        break;
    case ilmiNoError:
        break;
    default:
        break;
    }
}

char
*ilmi_namestring_forintf(ilmi_intfblock *intfblk)
{
    if (!intfblk)
	return ("Unknown Interface");
    else
	return (intfblk->idb->firstsw->namestring);
}

char
*ilmi_string_for_reqtype(ilmiOperType type)
{
    switch (type) {
    case ilmiGet :
	return ("GET");
	break;
    case ilmiGetNext :
	return ("GETNEXT");
	break;
    case ilmiSet:
	return ("SET");
	break;
    default:
	return ("Unknown");
	break;
    }

    return ("UnKnown");
}

char
*ilmi_string_forclient(ilmiClient client_id)
{
    ilmi_clientblock *client_blk = NULL;

    if (client_id == ILMI_ID)
	return ("ILMI");
    
    client_blk = ilmi_getclientblk(client_id);
    if(!client_blk)
	return ("Unknown");
    else
	return (client_blk->name);
}

void
ilmi_dump_msgblock(ilmi_msgblock *msg)
{
    if (!msg)
	return;
    
    buginf("Reqtype = %s Reqid = %d Requester = %s, Transid = %d (%s)",
	   ilmi_string_for_reqtype(msg->oper),
	   msg->req_id,
	   ilmi_string_forclient(msg->client_id),
	   msg->transid,
	   ilmi_namestring_forintf(ilmi_getintfblk(msg->intf_id)));
    
}


void
ilmi_state_transit (ilmiIntf intf, int from_state, int to_state, char *fn)
{

    if(!atm_ilmi_debug)
        return;

    buginf("\nILMI Transition :\n Intf := %d From  %s To  %s  %s",intf,ilmi_print_state(from_state),ilmi_print_state(to_state),
        fn);
    return;

}

char
*ilmi_print_state (int state)
{
    switch(state) {
    case ilmiIntfDown:
        return ("Down");
        break;
    case ilmiIntfRestarting:
        return ("Restarting");
        break;
    case ilmiIntfAwaitRestartAck:
        return ("AwaitRestartAck");
        break;
    case ilmiIntfUpAndNormal:
        return ("UpAndNormal");
        break;
    }
    return ("--");
}

void
ilmi_print_address (OctetString *addr)
{
    char buf[52];

    if (!addr)
	return;
    
    if (addr->length == ILMI_NETPREFIX_LEN)
	atm_printprefix(addr->octet_ptr, buf);
    else if (addr->length == ILMI_ATMADDRESS_LEN)
	atm_printnsap(addr->octet_ptr, buf);
    else
	return;

   buginf("%s", buf);

}

/* ----------------------------------------------------------------------
 * The following routines have been added to fix the problem of the
 * selector byte being not checked.
 */
/*
 * This routine is called after the calling has verified that this
 * message is for performing a SET related to  address (De)registration.
 * Hence we do not repeat the above validation here.
 */
 

boolean
ilmi_validate_localtable (ilmi_msgblock *msg, OctetString *address, boolean *send)
{
    boolean result;

    if (!address)
	return (FALSE);

    switch (msg->vblist->value.sl_value) {
    case ilmiAddressValid :
	result = ilmi_checkreg_addlocal(address, msg->intf_id, send);
	return (result);
	break;
    case ilmiAddressInValid :
	result =  ilmi_checkdereg_removelocal(address, msg->intf_id, send);
	return (result);
	break;
    default :
	return (FALSE);
	break;
    }

    return (FALSE);
}
    





/* ----------------------------------------------------------------------
 * Given a message block. This routine checks if it is a address
 * registration/deregistration message. If it is, then it extracts
 * the address octetstring from the varbind in the message. If
 * not it returns NULL.
 */

OctetString
*ilmi_addressreg_vb (ilmi_msgblock *msg)
{
    OID address_oid, *oid;
    OctetString *address;
    int carry=0;

    
    if (!msg || !msg->vblist || (msg->oper !=  ilmiSet))
	return (NULL); 

   
    oid = msg->vblist->name;
    address_oid.oid_ptr = (ulong *)IDatmfAddressStatus;
    address_oid.length  = LNatmfAddressStatus;
    if (!CmpNOID(oid, &address_oid,LNatmfAddressStatus)) {
	if ((InstToVariableOctetString(oid, LNatmfAddressStatus+1,
				       &address, EXACT, &carry)) < 0) {
	    return (NULL);
	}
	return (address);
    }
    return (NULL);
}

boolean
ilmi_checkreg_addlocal (OctetString *incoming, int intf_id, boolean *send_to_peer)
{
    ilmiAddressTestType result=ilmiAddressNoMatch;
    ilmi_atmaddressRow *newrow=NULL;
    ilmi_intfblock *intfblk=NULL;


    
    if (!incoming)
	return (FALSE);

    intfblk = ilmi_getintfblk(intf_id);


    if (!intfblk ||
	(incoming->length != ILMI_ATMADDRESS_LEN))
	return (FALSE);

    if (atm_ilmi_debug) {
	buginf("\nILMI: (Local) Reg. validation attempt for ");
	ilmi_print_address(incoming);
    }

    /* Check if the address exists in the local table. */
    if ((ilmi_getrow_foraddress(intfblk->localAddressTabQ, incoming,
				ILMI_ATMADDRESS_LEN)) != NULL) {
	result = ilmiAddressMatchWithSEL;
    } else {
	result = ilmiAddressNoMatch;
    }

    /*
     * We now need to find if the address needs to be sent to the peer.
     * For this we look for a 19 byte match in the remote table.
     */

    if ((ilmi_getrow_foraddress(intfblk->peerAddressTabQ, incoming,
			   ILMI_ATMADDRESS_LEN-1)) != NULL) {
	/* Remote table has a 19 byte match so we do not send it out. */
	if (atm_ilmi_debug)
	    buginf("\nILMI: Found in remote table. Address not sent to peer.");
	*send_to_peer = FALSE;
    } else {
	*send_to_peer = TRUE;
	if (atm_ilmi_debug)
	     buginf("\nILMI: Not Found in remote table. Address sent to peer");
    }

    
    if (result == ilmiAddressMatchWithSEL) {
	if (atm_ilmi_debug)
	    buginf("\nILMI: Address already in local table. Not added");
	return (TRUE);
    } else if (atm_ilmi_debug) {
	buginf("\nILMI: Address not in local table. Adding");
	}
    
    if ( (newrow = malloc(sizeof(ilmi_atmaddressRow))) == NULL) {
	return (FALSE);
    }

    newrow->nxt=NULL;
    newrow->addr = CloneOctetString(incoming);
    if (!newrow->addr) {
	free(newrow);
	return (FALSE);
    }

    enqueue (&intfblk->localAddressTabQ, newrow);
    ilmi_localtable_update = TRUE;
    return (TRUE);
}

/* ------------------------------------------------------------------
 * Check if the request to de-register this address should be sent
 * to the peer.
 */

boolean
ilmi_checkdereg_removelocal (OctetString *incoming, int intf_id, boolean *send_to_peer)
{
    ilmi_atmaddressRow *row=NULL;
    ilmi_intfblock *intfblk;
    
    if (!incoming)
	return (FALSE);

    intfblk = ilmi_getintfblk(intf_id);

    if (!intfblk)
	return (FALSE);

    if (atm_ilmi_debug) {
	buginf("\nILMI: (Local) DeReg. validation attempt for ");
	ilmi_print_address(incoming);
    }
    
    /* First extract this address out */
    row = ilmi_getrow_foraddress(intfblk->localAddressTabQ, incoming,
				 ILMI_ATMADDRESS_LEN);
    if (row) {
	unqueue(&intfblk->localAddressTabQ, row);
	if (atm_ilmi_debug)
	    buginf("\nILMI: Removing Address from local table.");
	FreeOctetString(row->addr);
	row->addr = NULL;
	free(row);
	
    } else {
	if (atm_ilmi_debug)
	    buginf("\nILMI: Request not sent out. Address non-existent in local table.");
	*send_to_peer = FALSE;	
	return (FALSE);
    }

    /*
     * Now we check if there are any other addresses in the local
     * table with the same first 19 bytes.
     */

    if ((ilmi_getrow_foraddress(intfblk->localAddressTabQ, incoming,
				ILMI_ATMADDRESS_LEN-1)) != NULL) {
	if (atm_ilmi_debug)
	    buginf("\nILMI: Deregister request not sent to peer");
	*send_to_peer = FALSE;
	
    } else {
	*send_to_peer = TRUE;
	if (atm_ilmi_debug)
	    buginf("\nILMI: Deregister request sent to peer");
    }

    ilmi_localtable_update = TRUE;
    return (TRUE);    
}

/* ------------------------------------------------------------------------
 * Fetch the row based on the address match.
 */

static ilmi_atmaddressRow
*ilmi_getrow_foraddress (queuetype table, OctetString *incoming,
			 int length)
{
    ilmi_atmaddressRow *add=NULL;

    if (!incoming)
	return (NULL);

    for (add=table.qhead;add;add=add->nxt) {
	if (!ilmi_CmpNOctetString(add->addr, incoming, length))
	    return (add);
	else
	    continue;
    }

    return (NULL);
}
/* ----------------------------------------------------------------------
 * Delete the local address table.
 */
void
ilmi_delete_localdb(ilmi_intfblock *intfblk)
{
    ilmi_atmaddressRow *node;

    while ((node = dequeue(&intfblk->localAddressTabQ)) != NULL) {
	 if(node->addr) {
	     FreeOctetString(node->addr);
	     node->addr = NULL;
	 }
	  
	 free(node);
    }

    while ((node = dequeue(&intfblk->peerAddressTabQ)) != NULL) {
	if(node->addr) {
	    FreeOctetString(node->addr);
	    node->addr = NULL;
	}
	
	free(node);
    }
    
}

/* -----------------------------------------------------------------------
 * Similar to ilmi_CmpoctetString but takes a specified length.
 */

int
ilmi_CmpNOctetString (OctetString *o1, OctetString *o2, int length)
{
    long            i, runlength, diff;

    runlength = MIN(o1->length, o2->length);
    runlength = MIN(runlength, length);

    for (diff = i = 0; (i < runlength) && !diff; i++) {
	if (o1->octet_ptr[i] != o2->octet_ptr[i]) {
	    if (o1->octet_ptr[i] < o2->octet_ptr[i])
		diff = -1;
	    else
		diff = 1;
	}
    }

    if (diff == 0 && (o1->length < length || o2->length < length)) {
	diff = o1->length - o2->length;
    }

    return ((int) diff);
    
}

void
ilmi_print_nsap (OctetString *address)
{
    char nsap[52];

    if (!address)
	return;
    atm_printnsap (address->octet_ptr, nsap);
    printf("%s",nsap);
    return;
}

static void
ilmi_print_prefix (OctetString *prefix)
{
    char pref_out[52];

    if (!prefix)
    return;

    atm_printprefix(prefix->octet_ptr, pref_out);
    printf("%s", pref_out);
    return;
}

void
ilmi_print_addTable(ilmi_intfblock *intfblk)
{
    ilmi_atmaddressRow *nd=NULL, *tmpnode=NULL;

    if (!intfblk)
	return;

    printf("\nLocal Table :");
    nd = intfblk->localAddressTabQ.qhead;
    while(nd) {
	 mem_lock(nd);
	 printf("\n");
	 ilmi_print_nsap(nd->addr);
	 tmpnode = nd;
	 nd=nd->nxt;
	 free(tmpnode);
     }

    printf("\nRemote Table :");
    nd = intfblk->peerAddressTabQ.qhead;
    while(nd) {
	mem_lock(nd);
	printf("\n");
	ilmi_print_nsap(nd->addr);
	tmpnode = nd;
	nd=nd->nxt;
	free(tmpnode);
    }    
	
}

void
ilmi_show_command (hwidbtype *hwidb)
{
    ilmi_intfblock *intfblk=NULL, *tmp=NULL;

    if (!hwidb) {
	automore_enable(NULL);
	intfblk=ilmi_intfQ.qhead;
	while (intfblk) {
	    mem_lock(intfblk);
	    ilmi_display_perif(intfblk);
	    tmp = intfblk;
	    intfblk = intfblk->nxt;
	    free(tmp);
	}
	automore_disable();	
    } else {
	intfblk = ilmi_getintfblk(hwidb->hw_if_index);
	if (!intfblk)
	    return;

	automore_enable(NULL);
	mem_lock(intfblk);
	ilmi_display_perif(intfblk);
	free(intfblk);
	automore_disable();
    }

}

#define NEWLINE printf("\n")
static void
ilmi_display_perif( ilmi_intfblock *intfblk)
{
    atm_db_t  *atm_db;
    int intf;
    OctetString *addr=NULL, *tmpaddr=NULL;
    vc_info_t   *vc=NULL;

    if (!intfblk)
	return;

    atm_db = intfblk->idb->atm_db;
    
    NEWLINE;
    NEWLINE;
    printf("%22s %15s ", "Interface",intfblk->idb->firstsw->namestring);

    printf("%17s ", "ILMI VCC:");
    vc = atm_getvc(atm_db, atm_db->ilmi_vc);
    if (vc)
	printf("(%u, %u)", vc->vpi, vc->vci);
    else
	printf("Not Configured");

    
    NEWLINE;
    printf("%22s ","ILMI Keepalive: ");
    if (atm_db->ilmipoll_interval) {
	printf("Enabled (%d Seconds)", atm_db->ilmipoll_interval/1000);
    } else {
	printf("Disabled");
    }

    NEWLINE;
    printf("%22s ","Address Registration: ");
    if (atm_db->address_reg)
	printf("Enabled");
    else
	printf("Disabled");
    

    NEWLINE;

    if (intfblk->state != ilmiIntfUpAndNormal) {
	printf("%22s %s", "ILMI State:", "Waiting");
	NEWLINE;
	return;
    }

    if (intfblk->idb->atm_db->address_reg)
	printf("%22s %s", "ILMI State: ","UpAndNormal");
    else
	printf("%22s %s", "ILMI State: ", "Registration Disabled");
    
    printf("\n%22s %15i ", "Peer IP Addr:", atm_db->peeruni_ip);
    if (atm_db->peeruni_ifname )
	printf("%17s %s", "Peer IF Name:", atm_db->peeruni_ifname);

    NEWLINE;
    printf("Prefix(s):");
    intf = intfblk->intf_id;
    addr=ilmi_addrtraverse(intf,NULL);
    while(addr) {
	mem_lock(addr);
	printf("\n");
	ilmi_print_prefix(addr);
	tmpaddr = addr;
	addr=ilmi_addrtraverse(intf,addr);  /* Get Next */
	free(tmpaddr);
    }

    NEWLINE;
    printf("Addresses Registered:");
    ilmi_print_addTable(intfblk);
	
    
    NEWLINE;
}
/* ------------------------------------------------------------------------
 * Called when a response to address (de)registration request is received.
 * This confirms the receipt by the peer and we thus update our peer
 * table.
 */
void
ilmi_updatepeer_table(ilmi_msgblock *msg)
{
    OctetString *address;
    ilmi_intfblock *intfblk=NULL;
    ilmi_atmaddressRow *nd=NULL, *newrow=NULL;
    boolean found;

    /* Checks */
    if (!msg || !msg->vblist)
	return;
  
    /*
     * For DeRegistration a return error of NO_SUCH_NAME is equivalent
     * to a successful DeRegistration so mark it such and proceed.
     */
    if (msg->response != ilmiRespNoErr) {
 	if ((msg->vblist->value.sl_value != ilmiAddressInValid) ||
 	    (msg->response != ilmiRespNoSuchNameErr))
 	    /* Error Case. Discard */
 	    return;
 	else
 	    msg->response = ilmiRespNoErr;
    }
    

    /*
     * address is malloc'd we need to free it once done by callng
     * goto ilmi_updatepeer_table_cleanup.
     */
    address = ilmi_addressreg_vb(msg);
    if(!address)
	return;
    
    intfblk = ilmi_getintfblk(msg->intf_id);
    if (!intfblk ||  (address->length != ILMI_ATMADDRESS_LEN))
	goto ilmi_updatepeer_table_cleanup;
    

	if (atm_ilmi_debug)
        buginf("\nILMI: Set confirmed. Updating peer address table");

    found = FALSE;
    for (nd=intfblk->peerAddressTabQ.qhead;nd;nd=nd->nxt) {
	if (!ilmi_CmpNOctetString(nd->addr,address,ILMI_ATMADDRESS_LEN - 1)) {
	    found = TRUE;
	    break;
	}
    }

    switch (msg->vblist->value.sl_value) {
    case ilmiAddressValid :    /* Registration Confirmation */
	if (found == TRUE)
	    goto ilmi_updatepeer_table_cleanup;
	else {
	    if ( (newrow = malloc(sizeof(ilmi_atmaddressRow))) == NULL) {
		goto ilmi_updatepeer_table_cleanup;
	    }
	    newrow->nxt=NULL;
	    newrow->addr = CloneOctetString(address);
	    if (!newrow->addr) {
		free(newrow);
		goto ilmi_updatepeer_table_cleanup;
	    }
	    enqueue(&intfblk->peerAddressTabQ, newrow);
	    goto ilmi_updatepeer_table_cleanup;
	}
	break;
    case ilmiAddressInValid :  /* Deregistration Confirmation */
	if (found == FALSE)
	   goto ilmi_updatepeer_table_cleanup;
	else {
	    unqueue(&intfblk->peerAddressTabQ, nd);
	    FreeOctetString(nd->addr);
	    nd->addr = NULL;
	    free(nd);
	}
	break;
    default :
	break;
    }

ilmi_updatepeer_table_cleanup:
    FreeOctetString(address);
    return;
}


/* -----------------------------------------------------------------------
 * This routine is called periodically to make sure that for each
 * interface the local and peer table are in sync.
 */

void
ilmi_sync_localpeertable (ilmi_intfblock *intfblk)
{
    ilmi_atmaddressRow *peeradd, *localadd;
    boolean found;

    
    if (!intfblk)
	return;

    /* If no (de)registration attempts since last sync abandon sync. */       
    if (ilmi_localtable_update == FALSE) {
        if (atm_ilmi_debug)
	    buginf("\nILMI: Table is up to date. Sync not done");
	return;
    }


    if (atm_ilmi_debug)
	buginf("\nILMI: Table is not up to date. Syncing");

    /* Initialize */
    ilmi_localtable_update = FALSE;



    /*
     * Check for deletions to be made to the Peer table.
     */
    for (peeradd=intfblk->peerAddressTabQ.qhead; peeradd; peeradd=peeradd->nxt) {
	
	found = FALSE;
	for (localadd=intfblk->localAddressTabQ.qhead; localadd;
	     localadd=localadd->nxt) {
	    if (!ilmi_CmpNOctetString(localadd->addr,peeradd->addr,
				      ILMI_ATMADDRESS_LEN - 1)) {
		found = TRUE;
		break;
	    }
	}

	if (found == FALSE) {
	    /*
	     * We have entries in the peer table which do not have a
	     * corresponding entry in the local table hence we remove
	     * it.
	     */
	    ilmi_set_peeraddress (intfblk,peeradd->addr,ilmiAddressInValid);
	     /* Indicate update required as the tables are yet up to date */
	    ilmi_localtable_update = TRUE;
	}
    }
    
    /*
     * Check for additions to be made to the Peer table.
     */
    for (localadd=intfblk->localAddressTabQ.qhead; localadd; localadd=localadd->nxt) {
	
	found = FALSE;
	for (peeradd=intfblk->peerAddressTabQ.qhead; peeradd;
	     peeradd=peeradd->nxt) {
	    if (!ilmi_CmpNOctetString(localadd->addr,peeradd->addr,
				      ILMI_ATMADDRESS_LEN - 1)) {
		found = TRUE;
		break;
	    }
	}

	if (found == FALSE) {
	    /*
	     * We have entries in the local  table which do not have a
	     * corresponding entry in the peer table hence we need to
	     * add them.
 	     */
	    ilmi_set_peeraddress (intfblk,localadd->addr,ilmiAddressValid);
	    /* Indicate update required as the tables are yet up to date */
	    ilmi_localtable_update = TRUE;	    
	}
    }    


}

#define ILMI_ADDREG_OID_LEN 14
void
ilmi_set_peeraddress (ilmi_intfblock *intfblk,OctetString *addr,ilmiAddrState action)
{
    VarBind *vb;
    OID tmpoid;
    ilmi_msgblock *msg;
    int ii,value;
    
    static long oid_ptr[] = {1, 3, 6, 1, 4, 1, 353, 2, 6, 1, 1, 3, 0, 20,
				 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				 0, 0, 0, 0, 0, 0, 0};

    /* OID_HDR initialized in array, now fill in the nsap */
    for(ii=0; ii<20; ii++)
	oid_ptr[(ILMI_ADDREG_OID_LEN + ii)] = addr->octet_ptr[ii];

    tmpoid.oid_ptr =  oid_ptr;
    tmpoid.length  =  ILMI_ADDR_SET_LEN;
    
    value = action;
    vb = MakeVarBindWithValue(&tmpoid,NULL,INTEGER_TYPE,&value);
    if (!vb) {
	return;
    }

    msg = malloc(sizeof(ilmi_msgblock));
    if (!msg) {
	FreeVarBind(vb);
	return;
    }
    msg->vblist = vb;
    msg->intf_id = intfblk->intf_id;
    msg->retry_count = ILMI_MAXREQRETRY;
    msg->errcode     = ilmiNoError;
    msg->response    = ilmiRespNoErr;
    msg->encodedReq  = NULL;
    msg->oper        = ilmiSet;
    msg->client_id   = ILMI_ID;
    msg->ilmi_func   = ilmiReqOther;
    msg->address_reg_flag = TRUE;
    msg->encodedReq  = NULL;
	
    enqueue(&ilmi_inputQ,msg);

    
}






      
