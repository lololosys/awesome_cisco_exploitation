/* $Id: stun_clslack.c,v 3.4.18.2 1996/05/17 11:21:51 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/stun_clslack.c,v $
 *------------------------------------------------------------------
 * STUN specific code for supporting CLS-Local Acknowledgement
 *
 * April 1995, Vipin Rawat
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stun_clslack.c,v $
 * Revision 3.4.18.2  1996/05/17  11:21:51  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.4.2  1996/04/26  07:54:48  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4.4.1  1996/04/03  14:35:05  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.18.1  1996/03/18  20:12:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  01:23:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  09:42:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  00:55:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/08  03:20:49  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.3  1995/11/17  09:24:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:36:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:46:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/25  01:34:43  vrawat
 * CSCdi37602:  fix prioritization using new cls primitive
 *
 * Revision 2.3  1995/07/06  01:32:52  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.2  1995/07/02 05:15:59  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.1  1995/06/07  20:49:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"  /* -> _generic when frame relay sorted out */
#include "packet.h"
#include "logger.h"
#include "../ibm/msg_stun.c"    /* Not a typo, see logger.h */
#include "config.h"
#include "clsdriver.h"
#include "../ui/debug.h"
#include "ibm.h"
#include "stun.h"
#include "ibm_externs.h"
#include "../if/tring.h"
#include "sna_la_pri.h"
#include "sdlc.h"
#include "sdlc_debug.h"
#include "stun_sdlc_public.h"
#include "slack.h"
#include "ibm_public.h"
#include "stun_clslack.h"


/*
 * stcls_connect
 * perform an active open to establish a connection to our peer. 
 */

stunconnrtn stcls_connect (stunpeertype *stp)
{
    GnDLUCep* thisCep; 
    GnSapStruct* thisSap; 

    if (stp->type != STUN_PT_CLS) {
	if (stun_debug)
	    buginf("\nSTUN: CONN: connect pointer invalid (cls)");
        stun_newstate(stp, STUN_PS_DEAD);
        return(STUN_CONN_FAILED);
    }
      
    thisCep = GnDLUCepCreate(); 
    thisSap = GnDLUSapCreate(); 

    if ((!thisCep) || (!thisSap))  {
	if (stun_debug)
           buginf(" STUN:Can not create cep/sap structure, no connection initiated\n");
       stun_newstate(stp,STUN_PS_DEAD);
       return(STUN_CONN_FAILED);
    } /*  of if */       
        
    thisSap->IsIncoming = FALSE; 
    thisSap->SapValue = stp->lsap; 
    thisSap->pDluPort = stp->thisPort; 
    thisSap->pCep = thisCep ;   

    thisCep->pSapStruct = thisSap; 
    thisCep->pDlu = stp->thisDlu; 
    thisCep->RPeerSap = stp->rsap; 
    thisCep->pArgPointer = stp ; 
    thisCep->Priority = stp->cls_priority;

    stp->thisCep = thisCep;

    dlci_to_hwaddr(&stp->dlci, &thisCep->LocalHWAddress); 

    p_enqueue(& ((GnDLUPort *)stp->thisPort)->qCepList, thisCep);
    clsdriver_open(stp->thisPort, stp->lsap, thisSap);
    stp->state = STUN_PS_OPENWAIT; 
    return(STUN_CONN_CONNECTING);    /* currently not used */

} /* of function stcls_connect */ 



void stcls_connectRet(void *pArgPointer, boolean IsError)
{

    stunpeertype* stp = (stunpeertype *)pArgPointer; 

    if (IsError) { 
        stp->state = STUN_PS_DEAD; 
        stp->thisCep = NULL;
    }
    else 
        stp->state = STUN_PS_CONNECTED; 

} /* of function stcls_connected */ 



void stcls_err(void *pArgPointer) 
{

    stunpeertype *stp = (stunpeertype *)pArgPointer; 
    if (stp->thisCep)
        clsdriver_abort(stp->thisCep); 

} /* of function stcls_err */ 



void stcls_close (stunpeertype *stp, ulong param)
{
  /* function not used, use abort instead */ 
    clsdriver_close(stp->thisCep); 
    /* handle param, if needed */ 

}  /* of function stcls_close */ 



void stcls_abort(stunpeertype *stp)
{
    if (stun_debug)
      buginf (" stun peer aborting \n ");
    
    if (stp->thisCep) {   /* non NULL */ 
        clsdriver_abort(stp->thisCep); 
        stp->thisCep = NULL;
    } /* of if (stp->thisCep) */         

} /* of function stcls_abort */ 



boolean stcls_senderErr(void *arg) 
{ 
    stunpeertype *stp= (stunpeertype *)arg; 

    if (slack_debug & SLACK_DBG_FLOW_CONTROL) {
       buginf("\nSLACK (Serial%d): Dropped SDLC frame ",
		       stp->stun_idb->unit);
    }
    stp->drops++;
/*    stif_nukepak(pak);  check it! clsdriver is already doing destroy of
                          the message */ 

    stun_newstate(stp, STUN_PS_DEAD);
    return(FALSE);

} /* of function stcls_senderErr */ 

/*
 * stcls_sender 
 * this is the sender for cls using the clsdriver. 
 *
 * if the peer isn't open yet the appropriate state is entered.
 * if this is the first packet in the not open state it is queued
 * for the background process which will open.  if this isn't the
 * first packet it is squished into the ground.
 *
 * this routine can be called from anywhere including interrupt level.
 */

boolean stcls_sender (stunpeertype *inpeer, paktype *pak, hwidbtype *outidb,
		       boolean force, stunpeertype **outpeer,
		       boolean usethispeer, boolean toallpp)
{
    leveltype status;
    int res;
    stunpeertype *peer;
    sdlc_data_block *sdb;
    stunhdrtype *sth;

    peer = inpeer;          
    status = raise_interrupt_level(NETS_DISABLE);  
    switch (peer->state) {
      case STUN_PS_CONNECTED:
	reset_interrupt_level(status); 
        if (!peer->thisCep)          /* no cep structure! */
           res = CLSSEND_CLOSED;
        else { 
           res = clsdriver_send(peer->thisCep, pak, force);

           peer->pkts_tx++;
	} /* of else */ 
	return(TRUE); /* Check it!  may cause problems, if data sending
                         wasn't successful */

      case STUN_PS_CLOSED:
      case STUN_PS_DEAD:    
        /*
         * Only do active open
         * if it is a secondary stun sdlc station,
         * or if stun local-ack is not active.
         * or if sdlc is in a disconnect state (allow XID to flow)
         * or if the op is a LINKUP_RSP (opens non root peers)
         *
         */
        sdb = SDB_FROM_PEER(peer);
        sth = (stunhdrtype *) pak->datagramstart;
        if ((!(peer->local_ack_active)) ||
	      ISSSDLC(peer->stun_idb) ||
              (sdb->sdlc_state == SDLC_DISCONNECT) ||
              (sth->op == STUN_OP_LINKUP_RSP)) {
	    stun_newstate(peer, STUN_PS_OPENING);
	    pak->peer_ptr = peer;
	    enqueue(&stconn_pakQ, pak);
	    reset_interrupt_level(status); 
	    peer->pkts_tx++;
	    return(TRUE);
        }
      case STUN_PS_OPENWAIT:
      case STUN_PS_OPENING:
      default:
	peer->drops++;
        if (stun_debug)
            buginf("\nSTUN: cls sender in wrong state, dropping packet");
	reset_interrupt_level(status); 
	stif_nukepak(pak);
	return(FALSE);
    }
} /* of function stcls_sender */ 



void  stcls_rddata(void* TempCep, paktype *pak)
{
   GnDLUCep* thisCep= (GnDLUCep *)TempCep; 
   hwaddrtype address = thisCep->LocalHWAddress; 
   idbtype* swidb = thisCep->pSapStruct->pDluPort->PortId.fTypeInfo.swidb;    
   stunpeertype* stp= (stunpeertype *)thisCep->pArgPointer;
   stunhdrtype *sth;
   hwidbtype* idb;
   ushort opcode;
   paktype *newpak;
   ushort dlci; 
   sdlc_data_block *sdb; 

   hwaddr_to_dlci(&address,&dlci);
   
   sth = (stunhdrtype *)pak->datagramstart; 
   if (stun_debug) { 
      buginf("\n STUN: DATA "); /* update debug message */ 
   } /* of if */ 
   
   if ((sth->op == STUN_OP_VREQ) || (sth->op == STUN_OP_VREP)) {              

       if (sth->op == STUN_OP_VREQ) {
           if (stun_debug) {
               buginf("\nSTUN: received VREQ packet");
           }
           newpak = stpak(STUN_OP_VREP, 0, NULL, 0);
           if (newpak) {
               clsdriver_send(thisCep, newpak, TRUE);
           } else {
               errmsg(&msgsym(NOBUF, STUN), "STUN", "send keepalive reply");
               if (stun_debug) {
                   buginf("\nSTUN: send keepalive reply, nopak/fail");
               }
           }
       } else {
           if (stun_debug) {
               buginf("\nSTUN: received VREP packet");
           }
       }
       if (stp) {
           GET_TIMESTAMP(stp->last_keepalive);
       }
       datagram_done(pak); 
       return;  
    } 

    if ((sth->magic != STUN_MAGIC) || (stun_pinfo[sth->group] == NULL)) { 
       
       if (stun_debug) 
           buginf("\n STUN: Incorrect header/group "); 
       stif_nukepak(pak); 
       return; 
    } /* of if */ 

    idb = swidb->hwptr; 
    if (!stp) {   /* may be first packet */ 
        stp = stcls_findpeer(sth->group, &(sth->addr[0]),swidb, dlci);
       
        if (stp) { 
            switch(stp->state) { 

    	        case STUN_PS_DEAD:
	        case STUN_PS_CLOSED:
	        case STUN_PS_OPENING:
	        case STUN_PS_OPENWAIT:
                {
                    stun_newstate(stp, STUN_PS_CONNECTED);
		    stp->thisCep = thisCep; 
		    thisCep->pArgPointer= stp; 
		    thisCep->IsFirstPacket = FALSE;
                    thisCep->Priority = stp->cls_priority;
                    break;                
		}

                case STUN_PS_CONNECTED:
                {
                   if (stun_debug)   
                       buginf("STUN: stp is already CONNECTED on listening side, packet is history \n");
                   stif_nukepak(pak);
                   return;  
		}

	        default:
                {
                   if (stun_debug)   
                       buginf("STUN: bad connection state \n");
                   stp->drops++;
                   stif_nukepak(pak);
                   return; 
                }
	     }  /* of switch */ 
          } /* of if (stp) */ 
    } /* of if (!stp) */ 

    if (!stp) { 
	if (stun_debug)
            buginf("\nSTUN: no stp found \n");
        stif_nukepak(pak);   
        return;     
    }


    GET_TIMESTAMP(stp->last_keepalive);
    
    pak->datagramstart = pak->datagramstart + sizeof(stunhdrtype);
    pak->datagramsize = sth->len; 
    opcode = sth->op;

    if (opcode & STUN_OP_ERR) {
        stif_nukepak(pak); 
        stcls_err(thisCep);
        return;
    }
    if (opcode == STUN_OP_IGNORE) {
        stif_nukepak(pak); 
        return;
    }                                                                                      
    switch (opcode) {
      case STUN_OP_LINKDOWN_REQ:
      case STUN_OP_PASS:
      case STUN_OP_LINKUP_REQ:
        /* if we are running stun tg, save SNRM address to send on sdlc SNRM */
        if (stp->tg_active) {
           sdb = SDB_FROM_PEER(stp);
           if (sdb)
              sdb->tg_snrm_addr = pak->datagramstart[0];
        }
      case STUN_OP_LINKUP_RSP:
      case STUN_OP_LINKDOWN_RSP:
      case STUN_OP_ABORT_REQ:
      case STUN_OP_ABORT_RSP:
      case STUN_OP_RD_REQ:
      case STUN_OP_DATA:
      case STUN_OP_SIM: 
      case STUN_OP_QUENCH_ON:
      case STUN_OP_QUENCH_OFF:
      case STUN_OP_LINK_ESTAB:
         pak -> enctype = ET_NULL; 
         pak -> if_output = firstsw_or_null(stp->stun_idb);
         stp->pkts_rx++;
         stun_fromnet_forward(pak,stp,opcode);
         break; 
      default:
         if (stun_debug)
            buginf(" STUN: incorrect opcode, dropped \n");
         stif_nukepak(pak);
         stp->drops++;
         break;
    } /* switch */

    sth->op = STUN_OP_EMPTY; 

} /* of function stcls_rddata */ 



stunpeertype *stcls_findpeer (uchar group, uchar *addrp, idbtype *swidb, ushort dlci)
{
    stunpeertype *stp;
    hwidbtype *in_idb = swidb->hwptr;
    hwidbtype *idb = NULL;
    stun_protocol_info *gpi;    
    fr_idb_struct_type *fr_idb;

    fr_idb = in_idb->frame_relay_stuff;

    for (idb = stun_first_idb; idb; idb = IDB_NEXT(idb)) {
	if (!idb->stun_Q.qhead)
	    continue;
	if ((gpi = stun_pinfo[idb->stun_group]) == NULL)
	  continue;
	for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next) {
	    if ((stp->type != STUN_PT_CLS) || 
		(stp->outswidb != swidb)   ||
                (stp->dlci != dlci))
		continue;
	    if ((stp->group == group) && (stp->all_addr ||
					!bcmp(addrp, stp->addr,
					      gpi->addrlength))) {
		in_idb->stun_if_last = idb;
		return(stp);
	    }
	}
    }
     return NULL;
} /* of function stcls_findpeer */


void  stcls_fini(void* cookie)
{
    leveltype status;
    stunpeertype *stp;

    stp = (stunpeertype *)cookie;

    if (!stp) {                         /* unknown peer abort */
        if (stun_debug)
            buginf("\nSTUN: unknown peer closed");
        return;
    }
    status = raise_interrupt_level(NETS_DISABLE);
    stp->inhdr.op = STUN_OP_EMPTY;
    stp->thisCep = NULL;
    if (stun_debug)
        buginf(" STUN: CLS error in fini routine \n");
    stun_newstate(stp, STUN_PS_CLOSED);
    reset_interrupt_level(status);

} /* of function stcls_fini */ 


boolean stcls_initpeer(stunpeertype* stp, idbtype *outswidb,byte lsap)
{
   GnDLUPort*   thisPort; 

   thisPort=GnEnablePort(DLU_TYPE_STUN,outswidb);
   if (thisPort) {
         thisPort->pDlu->connectRet= stcls_connectRet;
         thisPort->pDlu->rddata    = stcls_rddata;
         thisPort->pDlu->err       = stcls_err;
         thisPort->pDlu->fini      = stcls_fini;
         thisPort->pDlu->senderErr = stcls_senderErr;

    } /* of if */
    else {
         printf(" STUN: Can not enable port \n");
         return(FALSE);
    } /* of else */

     stp->thisCep = NULL; 
     stp->thisDlu = thisPort->pDlu;
     stp->thisPort = thisPort;
     stp->state  = STUN_PS_CLOSED;
     stp->sender = stcls_sender;
     stp->abort  = stcls_abort;
     stp->close  = stcls_close;
     stp->connect= stcls_connect;
     stp->lsap = lsap;
     stp->rsap = aListningSaps[0];
     stp->inhdr.op = STUN_OP_EMPTY;

     return(TRUE);

} /* of function stcls_initpeer */ 


void stcls_start(void)
{
    clsdriver_start();
}































