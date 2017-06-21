/* $Id: clsdriver.c,v 3.3.26.3 1996/08/28 12:46:32 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/clsdriver.c,v $
 *------------------------------------------------------------------
 * CLS Driver code for Local Acknowledgement. 
 *
 * April 1995, Vipin Rawat
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clsdriver.c,v $
 * Revision 3.3.26.3  1996/08/28  12:46:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.26.2  1996/06/28  23:06:32  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.26.1  1996/03/18  20:11:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  09:41:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/18  22:51:31  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  09:22:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:01:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/13  21:14:01  vrawat
 * CSCdi36577:  clsdriver code does CLSOpen(), when not configured
 *
 * Revision 2.6  1995/07/25  01:34:41  vrawat
 * CSCdi37602:  fix prioritization using new cls primitive
 *
 * Revision 2.5  1995/07/02  05:15:56  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.4  1995/06/28  09:25:21  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/09  19:28:09  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.2  1995/06/09  13:05:02  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:46:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../ibm/msg_clsdriver.c"	/* Not a typo, see logger.h */
#include "clsdriver.h" 
#include "subsys.h"
#include "packet.h"

#define IS_XID  1
#define IS_DEBUG 1

/* Global variables for FR-LACK DLU */
int iGnDluPid;

static TCLSIQInit* pQinit = NULL;
static TCLSIQueue* pWriteQ = NULL ; 
static TCLSIQueue* pReadQ = NULL ; 
queuetype GnDluIQueue; 


int GnDluReadPut(TCLSIQueue* dluReadQ, TCLSIMsg* theMsg)
{

    p_enqueue(&GnDluIQueue, theMsg);

    return (1); /* may process error conditions here*/ 
} /* of function GnDluReadPut */


GnDLUEntity* GnDLUCreate(int thisType)
{ 
  GnDLUEntity* this = malloc(sizeof(GnDLUEntity)) ; 

  if (this != NULL) {
     this->pNext=NULL; 
     this->iType = thisType;
     queue_init(&this->qPortList,0);
     p_enqueue(&qDLUList, this) ; 
  }

  return(this) ; 
} /* of function GnDLUCreate */ 



GnDLUPort* GnDLUPortCreate(void)
{
    GnDLUPort* this = malloc(sizeof(GnDLUPort));
    if (this != NULL) {
	this->pNext = NULL;
        queue_init(&this->qCepList,0);
        this->pDlu = NULL;
    }
    return (this);
} /* of function  GnDLUPortCreate */



void GnDLUAddPort(GnDLUEntity* this, GnDLUPort* thePort)
{
    p_enqueue(&this->qPortList, thePort);
} /* of function GnDLUAddPort */ 
     


GnDLUPort*  GnDLURemovePort(GnDLUEntity* this, GnDLUPort* thePort)
{
    if (checkqueue(&this->qPortList, thePort)) {
	p_unqueue(&this->qPortList, thePort);
    } else {
	errmsg(&msgsym(NOREMOVEPORT,CLSDR), thePort);
    }
    return thePort;
}  /* of function   GnDLURemovePort */
                                         
                                      
                                      
GnDLUPort* GnDLUFindPort(GnDLUEntity* this, CLSPortID* portID)
{
    GnDLUPort* thePort = (GnDLUPort*) this->qPortList.qhead;
    while (thePort != NULL)
    {
        /* ROUTER CRASH APR 13 */ 
	if (CLSIPortIdEqual(&thePort->PortId, portID)) {
	    break;
	}
	thePort = thePort->pNext;
    }
    return(thePort);
} /* of function GnDLUFindPort */

                                      
GnSapStruct* GnDLUFindSap(GnDLUPort *thisPort, byte thisSapValue) 
{
    GnDLUCep* theCep = (GnDLUCep*) thisPort->qCepList.qhead;
    while (theCep != NULL)
    {
	if (theCep->pSapStruct->SapValue == thisSapValue) { 
	    break;
	}
	theCep = theCep->pNext;
    }
    return(theCep->pSapStruct);
} /* of function GnDLUFindSap */


void GnDLUPortDestroy(GnDLUPort* this)
{
    GnDLUCep* thisCep;
   
    while ((thisCep = p_dequeue(&this->qCepList)))
	GnDLUCepDestroy(thisCep);

    free(this);
} /* of function  GnDLUPortDestroy */



GnSapStruct* GnDLUSapCreate(void)
{
   GnSapStruct* thisSap; 
   thisSap = malloc(sizeof(GnSapStruct)); 

   if(thisSap != NULL) { 
      thisSap->IsIncoming = TRUE;
      thisSap->SapValue = 0;
      thisSap->PSapId = 0 ; 
      thisSap->pDluPort = NULL;
      thisSap->pCep = NULL;
   } /* of if */ 

   return(thisSap);
} /* of function GnDLUSapCreate */
 

GnDLUCep* GnDLUCepCreate(void)
{
    GnDLUCep* this;
    this = malloc(sizeof(GnDLUCep));
    if (this != NULL) {
        this->pSapStruct = NULL;
	this->pNext = NULL;
	this->pDlu = NULL;
	this->PCepId = 0;
        this->IsFirstPacket = TRUE;
        this->Priority = 0;
    }
    return(this);
} /* of function  GnDLUCepCreate */ 


GnDLUCep*  GnDLURemoveCep(GnDLUPort* thisPort, GnDLUCep* thisCep)
{
    
    if (checkqueue(&thisPort->qCepList, thisCep)) {
	p_unqueue(&thisPort->qCepList, thisCep);
    } else {
	errmsg(&msgsym(NOREMOVECEP,CLSDR), thisCep);
    }
    return thisCep;    
} /* of function GnDLURemoveCep */



void GnDLUCepDestroy(GnDLUCep* thisCep)
{
    free(thisCep);
} /* of function GnDLUCepDestroy */



void GnShutdown()
{
   GnDLUEntity* thisDlu;

   while ((thisDlu = p_dequeue(&qDLUList)))
      GnDLUShutdown(thisDlu);
} /* of function GnShutdown */



void GnDLUShutdown (GnDLUEntity *thisDlu)
{
    GnDLUPort* thePort;

    while ((thePort = p_dequeue(&thisDlu->qPortList))) 
	GnDLUPortDestroy(thePort);

} /* of function GnDLUShutdown */ 



GnDLUEntity* GnFindDlu(int thisType) 
{
    GnDLUEntity *theDLU = qDLUList.qhead; 
    while (theDLU != NULL) { 
        if (theDLU->iType == thisType)
           return(theDLU);
        theDLU = theDLU -> pNext; 
    } /* of while */ 
    return(NULL);
} /* of function GnFindDlu */ 



GnDLUPort* GnEnablePort(int type, idbtype* thisIdb)
{

  GnDLUEntity*  thisDLU; 
  CLSPortID     thisPortId; 
  GnDLUPort*    thisPort;
  int i; 
  thisDLU = GnFindDlu(type); 

  if (!thisDLU) {  
     thisDLU = GnDLUCreate(type) ; 
  } /* of if */ 

  if (!thisDLU) { 
      errmsg(&msgsym(NODLUCREATE,CLSDR),type);
      return(NULL);
  }
  
  thisPortId.type = CLS_P_TYPE_SWIDB;
  thisPortId.fTypeInfo.swidb = thisIdb; 

  if (!(thisPort = GnDLUFindPort(thisDLU, &thisPortId))) { 
      
      thisPort = GnDLUPortCreate() ;
      
      if (!thisPort) { 

          errmsg(&msgsym(NOPORTCREATE,CLSDR));
          return(NULL);
      } /* of if */ 

      thisPort -> PortId.type = CLS_P_TYPE_SWIDB;
      thisPort -> PortId.fTypeInfo.swidb = thisIdb;
      thisPort -> pDlu = thisDLU; 
      thisPort -> pNext = NULL ; 
      thisPort -> NumListningSaps = NUM_LISTNING_SAPS; 
      p_enqueue(&thisDLU->qPortList, thisPort);  

      for (i=0;i< NUM_LISTNING_SAPS;i++) 
      {
          thisPort -> SapStruct[i].pDluPort = thisPort; 
          thisPort -> SapStruct[i].IsIncoming = TRUE;
          thisPort -> SapStruct[i].PSapId = 0 ; 
          thisPort -> SapStruct[i].pCep = NULL;
          thisPort -> SapStruct[i].SapValue = aListningSaps[i]; 

      } /* of for */ 


      GnDLUSendEnable(thisPort);   
    }

  return(thisPort); 

} /* of function GnEnablePort  */ 



void GnDLUSendEnable(GnDLUPort* thisPort) 
{
    uint parmSize;
    EnableReq_t*   enableReq;

    parmSize = sizeof(EnableFRParms_t);

    enableReq = (EnableReq_t*) CLSIMsgCreateHeader(
                                CLS_ENABLE | CLS_REQUEST,
                                offsetof(EnableReq_t,fParms) -
                                CLS_ENABLE_REQ_HDR_SZ + parmSize,
                                CLS_LLC_LAYER,
                                CLS_ENTITY_MGR,
                                0);
    if (enableReq == NULL)
    {
        errmsg(&msgsym(MSGERR,CLSDR)," SendEnable"); 
        return;
    }

    CLSIPortIdCopy(&thisPort->PortId, &enableReq->fPortId);
    enableReq->fCorrelator = (word)thisPort;   
    enableReq->fDLCType = CLS_FRAME_RELAY ; 
    CLSIQueuePut(pWriteQ, (TCLSIMsg*) enableReq);

}    /* of function  GnDLUSendEnable   */



/* This function, not used currently but will be used to deactivate the listening sap
   and disabling (which is done by the driver) the port when its no more used.
   Only one listening sap is assumed. 
*/ 

void GnDisablePort( GnDLUPort* thisPort)
{

  GnSapStruct* thisUSapId= &thisPort->SapStruct[0];

  GnDLUSendDeactSap(thisUSapId);

} /* of function GnDisablePort */ 


void GnDLUSendDisable(GnDLUPort* thisPort)
{
    DisableReq_t*   disableReq;
    
    disableReq = (DisableReq_t*) CLSIMsgCreateHeader(
				CLS_DISABLE | CLS_REQUEST,
				CLS_DISABLE_REQ_HDR_SZ,
				CLS_LLC_LAYER,
				CLS_ENTITY_MGR,
				0);
    if (disableReq == NULL)
    {
	errmsg(&msgsym(MSGERR,CLSDR), "SendDisable" );
	return;
    }
    
    CLSIPortIdCopy(&thisPort->PortId, &disableReq->fPortId);
    disableReq->fDLCType = CLS_FRAME_RELAY ; 
    disableReq->fCorrelator = (word)thisPort;  /* pointer mismatch */ 

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) disableReq);
    
} /* of function GnDLUSendDisable */ 



void GnDLUSendActListningSaps(TCLSIMsg* theMsg)
{
    EnableCnf_t *EnableCnfMsg = (EnableCnf_t *)theMsg;
    GnDLUPort *thisPort = (GnDLUPort *)EnableCnfMsg->fCorrelator ; 
    int i; 
       
    for (i=0; i < thisPort->NumListningSaps; i++)
        GnDLUSendActSap(thisPort, &thisPort->SapStruct[i] );

}   /* of function GnDLUSendActListningSaps */


     
void GnDLUSendActSap(GnDLUPort* thisPort, GnSapStruct *thisUSapId)
{
         
    ActSapReq_t* actSapReq;
    ActSapLLCParms_t* llcParms;
    int parmSize;

    parmSize = sizeof(ActSapLLCParms_t);

    actSapReq = (ActSapReq_t*) CLSIMsgCreateHeader(
                                CLS_ACTIVATE_SAP | CLS_REQUEST,
                                offsetof(ActSapReq_t, fParms) -
                                CLS_ACT_SAP_REQ_HDR_SZ +
                                parmSize,
                                CLS_LLC_LAYER,
                                CLS_ENTITY_MGR,
                                0);
    if (actSapReq == NULL)
    {
        errmsg(&msgsym(MSGERR,CLSDR), "SendActSap ");
        return;
    }

    actSapReq->fCLSIHdr.fUSapId = (word)thisUSapId; 
    actSapReq->fVersion = CLSI_VERSION;
    actSapReq->fCorrelator = (word)thisUSapId; 
    CLSIPortIdCopy(&thisPort->PortId, &actSapReq->fPortId);
    actSapReq->fDLCType = CLS_FRAME_RELAY;

    llcParms = (ActSapLLCParms_t*) &(actSapReq->fParms);
    llcParms->fUserClass = 0x02;
    llcParms->fSapValue = thisUSapId->SapValue;
    llcParms->fT1 = 0;
    llcParms->fT2 = 0;
    llcParms->fTi = 0;
    llcParms->fMaxOut = 0;
    llcParms->fMaxRetryCount = 0;           /* no limit */
    llcParms->fMaxIn = 0;
    llcParms->fMaxIField = 0;

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) actSapReq);

} /* of function GnDLUSendActSap  */



void GnDLUSendReqOpnStn(GnDLUCep* theCep)
{
    ReqOpnStnReq_t* reqOpnStnReq;
    ReqOpnStnLLCParms_t* llcParms;
    int parmSize;
/*  uchar DestAddr[IEEEBYTES];  */
    parmSize = sizeof(ReqOpnStnLLCParms_t);
    
    reqOpnStnReq = 
	(ReqOpnStnReq_t*) CLSIMsgCreateHeader(CLS_REQ_OPNSTN | CLS_REQUEST,
					      offsetof(ReqOpnStnReq_t,fParms)
					      -CLS_REQ_OPNSTN_REQ_HDR_SZ
					      +parmSize,
					      CLS_LLC_LAYER,
					      CLS_P_SAP_ID,
					      theCep->pSapStruct->PSapId);

    if (reqOpnStnReq == NULL)
    {
	errmsg(&msgsym(MSGERR,CLSDR), "SendReqOpnStn " );
	return;
    }
    
    reqOpnStnReq->fCLSIHdr.fUCepId = (word)theCep; 
    reqOpnStnReq->fVersion = CLSI_VERSION;
    reqOpnStnReq->fCorrelator = 0xdead;
    reqOpnStnReq->fQueuingType = CLS_QUEUING_DEFAULT;
    if (theCep->Priority) { 
        reqOpnStnReq->fQueuingType = CLS_QUEUING_CUSTOM;
        reqOpnStnReq->fQueuingParameter = theCep->Priority;
    }
    llcParms = (ReqOpnStnLLCParms_t*) &(reqOpnStnReq->fParms);

    snpa_copy(&theCep->LocalHWAddress,&llcParms->fDestAddr);
    snpa_clear(&llcParms->fSrcAddr);
    llcParms->fDlci = 0 ; 
    llcParms->fDestSap = theCep->RPeerSap;
    llcParms->fSrcSap = 0;
    llcParms->fExplorerOption = 0;
    llcParms->fT1 = 0;
    llcParms->fT2 = 0;
    llcParms->fTi = 0;
    llcParms->fMaxOut = 0;
    llcParms->fMaxRetryCount = 0;
    llcParms->fMaxIn = 0;
    llcParms->fDynamicWindow = 0;
    llcParms->fMaxOutIncr = 0;
    llcParms->fMaxIField = 0;
    llcParms->fL3Pid = CLS_L3PID_APPN; /* appn initially */     
    llcParms->fRifLength = 0 ; 
    llcParms->fExplorerTimer = 2000 ;
    llcParms->fExplorerRetries = 5;
    llcParms->fXIDTimer = 5000;
    llcParms->fXIDRetries = 10;
    CLSIQueuePut(pWriteQ, (TCLSIMsg*) reqOpnStnReq);

} /* of function GnDLUSendReqOpnStn */ 



void GnDLUSendConnectReq(GnDLUCep* theCep)
{
    ConnectReq_t* connectReq;
    ReqOpnStnLLCParms_t* llcParms;
    size_t parmSize = sizeof(ReqOpnStnLLCParms_t);
    
    connectReq = (ConnectReq_t*) CLSIMsgCreateHeader(
				CLS_CONNECT | CLS_REQUEST,
			     	offsetof(ConnectReq_t, fParms) -
			     	CLS_CONNECT_REQ_HDR_SZ + parmSize,
				CLS_LLC_LAYER,
				CLS_P_CEP_ID,
				theCep->PCepId);
    if (connectReq == NULL)
    {
	errmsg(&msgsym(MSGERR,CLSDR), " SendConnectReq ");
	return;
    }

    connectReq->fCLSIHdr.fModifyFlags = 0;
    llcParms = (ReqOpnStnLLCParms_t*) &connectReq->fParms[0];
    
    connectReq->fVersion = CLSI_VERSION;
    connectReq->fCorrelator = 0xdead;

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) connectReq);
}  /* of function GnDLUSendConnectReq */ 



void GnDLUSendModifyStnReq(GnDLUCep* theCep)
{
    ModifyStnReq_t* modifyStnReq;
    ReqOpnStnLLCParms_t* llcParms;
    size_t parmSize = sizeof(ReqOpnStnLLCParms_t);
    
    modifyStnReq  = (ModifyStnReq_t*) CLSIMsgCreateHeader(
				CLS_MODIFY_STN | CLS_REQUEST,
			     	offsetof(ModifyStnReq_t, fParms) -
			     	CLS_MODIFY_STN_REQ_HDR_SZ + parmSize,
				CLS_LLC_LAYER,
				CLS_P_CEP_ID,
				theCep->PCepId);
    if (modifyStnReq == NULL)
    {
	errmsg(&msgsym(MSGERR,CLSDR), " SendModifyStnReq ");
	return;
    }

    modifyStnReq->fCLSIHdr.fModifyFlags = 0;
    llcParms = (ReqOpnStnLLCParms_t*) &modifyStnReq->fParms[0];
    if (theCep->Priority) { 
        modifyStnReq->fCLSIHdr.fModifyFlags = CLS_MOD_QUEUING;
        modifyStnReq->fQueuingType = CLS_QUEUING_CUSTOM;
        modifyStnReq->fQueuingParameter = theCep->Priority;
    }
    modifyStnReq->fVersion = CLSI_VERSION;
    modifyStnReq->fCorrelator = 0xdead;

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) modifyStnReq);
}  /* of function GnDLUSendModifyStnReq */ 



void GnDLUSendDataReq(GnDLUCep* theCep, paktype *pak)
{
    DataReq_t* dataReq;
    hword reserve_length;

      dataReq = (DataReq_t*) CLSIMsgCreateFromPak(pak,
                                               CLS_DATA | CLS_REQUEST,
					       CLS_LLC_LAYER,
					       CLS_P_CEP_ID,
					       theCep->PCepId,
                                               0,
					       &reserve_length);
    if (dataReq == NULL)
    {
	errmsg(&msgsym(MSGERR,CLSDR), "SendDataReq ");
	return;
    }
    
    dataReq->fVersion = CLSI_VERSION;
    dataReq->fCorrelator = 0xdead;
    dataReq->fReserveLength = reserve_length;

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) dataReq);
}  /* of function GnDLUSendDataReq */ 



void GnDLUSendSignalStn(GnDLUCep* theCep)
{
    SignalStnReq_t* sigStnReq;
    
    sigStnReq = (SignalStnReq_t*)
	CLSIMsgCreateHeader(CLS_SIGNAL_STN | CLS_REQUEST,
			    sizeof(SignalStnReq_t) -
			    CLS_SIGNAL_STN_REQ_HDR_SZ,
			    CLS_LLC_LAYER,
			    CLS_P_CEP_ID,
			    theCep->PCepId);
    if (sigStnReq == NULL)
    {
	errmsg(&msgsym(MSGERR,CLSDR), " SendSignalStn" ); 
	return;
    }
    
    sigStnReq->fCLSIHdr.fActionCode = 0; /* start data (exit local */
					 /* busy state) */
    sigStnReq->fVersion = CLSI_VERSION;
    sigStnReq->fCorrelator = 0xdead;

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) sigStnReq);
} /* of function GnDLUSendSignalStn */ 


void GnDLUSendDiscReq(GnDLUCep* theCep)
{
    DisconnectReq_t* disconnectReq;
    size_t parmSize = 0;		/* For now. Do parameters later */
    
    disconnectReq = (DisconnectReq_t*) CLSIMsgCreateHeader(
				CLS_DISCONNECT | CLS_REQUEST,
				offsetof(DisconnectReq_t, fCorrelator) -
				CLS_DISCONNECT_REQ_HDR_SZ +
				parmSize,
				CLS_LLC_LAYER,
				CLS_P_CEP_ID,
				theCep->PCepId);
    if (disconnectReq == NULL)
    {
	errmsg(&msgsym(MSGERR,CLSDR), "SendDiscReq ");
	return;
    }
    
    disconnectReq->fVersion = CLSI_VERSION;
    disconnectReq->fCorrelator = 0xdead;

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) disconnectReq);
} /* of function GnDLUSendDiscReq */


void GnDLUSendDiscRsp(GnDLUCep* theCep)
{
    DisconnectRsp_t* disconnectRsp;
    size_t parmSize = 0;		/* For now. Do parameters later */
    
    disconnectRsp = (DisconnectRsp_t*) CLSIMsgCreateHeader(
				CLS_DISCONNECT | CLS_RESPONSE,
				offsetof(DisconnectRsp_t, fCorrelator) -
				CLS_DISCONNECT_RSP_HDR_SZ +
				parmSize,
				CLS_LLC_LAYER,
				CLS_P_CEP_ID,
				theCep->PCepId);
    if (disconnectRsp == NULL)
    {
	errmsg(&msgsym(MSGERR,CLSDR), " SendDiscRsp ");
	return;
    }
    
    disconnectRsp->fVersion = CLSI_VERSION;
    disconnectRsp->fCorrelator = 0xdead;

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) disconnectRsp);
} /* of function GnDLUSendDiscRsp */


void GnDLUSendCloseStnReq(GnDLUCep* theCep)
{
    CloseStnReq_t* closeStnReq;
    
    closeStnReq = (CloseStnReq_t*) CLSIMsgCreateHeader(CLS_CLOSE_STN | CLS_REQUEST,
					     offsetof(CloseStnReq_t, fCorrelator) -
					     CLS_CLOSE_STN_REQ_HDR_SZ,
					     CLS_LLC_LAYER,
					     CLS_P_CEP_ID,
					     theCep->PCepId);
    if (closeStnReq == NULL)
    {
	errmsg(&msgsym(MSGERR,CLSDR), " SendCloseStnReq ");
	return;
    }
    
    closeStnReq->fCLSIHdr.fUCepId = (word)theCep; 
    closeStnReq->fCLSIHdr.fActionCode = CLS_ACTION_FORCED;
    
    closeStnReq->fVersion = CLSI_VERSION;
    closeStnReq->fCorrelator = 0xdead;

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) closeStnReq);
    theCep->PCepId = 0;
}  /* of function GnDLUSendCloseStnReq */


void GnDLUSendDeactSap(GnSapStruct *thisUSapId)
{
    DeactSapReq_t* deactSapReq;
    
    deactSapReq = (DeactSapReq_t*) CLSIMsgCreateHeader(
				CLS_DEACTIVATE_SAP | CLS_REQUEST,
				CLS_DEACT_SAP_REQ_HDR_SZ,
				CLS_LLC_LAYER,
				CLS_P_SAP_ID,
				thisUSapId->PSapId);
    if (deactSapReq == NULL)
    {
	errmsg(&msgsym(MSGERR,CLSDR), " SendDeactSap ");
	return;
    }

    deactSapReq->fCLSIHdr.fUSapId = (word)thisUSapId;  
    deactSapReq->fVersion = CLSI_VERSION;
    deactSapReq->fCorrelator = 0xdead; 

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) deactSapReq);
} /* of function GnDLUSendDeactSap */ 


/************* START:   TCPDRIVER LIKE FUNCTIONS ************************/

/*   Following functions are similar in functionality to TCP function
     with similar names. Many one-liners should be made inline. 
 */

void clsdriver_start(void)
{

    IsClsdriverRunning = FALSE;
    if (UserConfiguredLisnSap)
         aListningSaps[0] = UserConfiguredLisnSap;
    else
         aListningSaps[0] = LISTNING_SAP_0;

    queue_init(&GnDluIQueue,0);
    queue_init(&qDLUList,0);

    if (!(pQinit = CLSIQInitCreate("CLS-LACK Daemon", GnDluReadPut))) {
        errmsg(&msgsym(NOINIT, CLSDR));
        return;
      }

    if (!(pReadQ = CLSIQueueCreate(pQinit, NULL))) {
        errmsg(&msgsym(NOINIT, CLSDR));
        CLSIQInitDestroy(pQinit);
        return;
      }

    if (!(pWriteQ = CLSOpen(pReadQ))) {
        errmsg(&msgsym(NOINIT, CLSDR));
        CLSIQueueDestroy(pReadQ);
        CLSIQInitDestroy(pQinit);
        return;
      }

    if ((iGnDluPid = cfork (GnDluMain, 0L, 0, "CLS-LACK Server", 0)) < 0) {
        errmsg(&msgsym(NOCLSSERVER,CLSDR));
        CLSClose(pWriteQ);
        CLSIQueueDestroy(pReadQ);
        CLSIQInitDestroy(pQinit);
        return; 
   }

   IsClsdriverRunning = TRUE; /* this is also done in clsdriver routine, don't bother, be safe */

} /* of function clsdriver_start */ 



void clsdriver_open(GnDLUPort* thisPort, byte thisSapValue, GnSapStruct* thisSap) 
{

    GnDLUSendActSap(thisPort, thisSap) ; 

} /* of function clsdriver_open */

 
int clsdriver_send(GnDLUCep *thisCep, paktype *pak, boolean force)
{

  GnDLUSendDataReq(thisCep, pak); 
  return(1); /* no reason, think over it and change the code */ 

} /* of function clsdriver_send */ 

         
void clsdriver_close(GnDLUCep* thisCep)
{
/* No one uses this function currently, use abort instead */ 
  GnDLUSendDiscReq(thisCep); 

} /* of function clsdriver_close */



void clsdriver_abort(GnDLUCep* thisCep)
{

    if (thisCep) { 
        GnDLUSendDiscReq(thisCep);
    }

} /* of function clsdriver_abort */




/************* END:   TCPDRIVER LIKE FUNCTIONS ************************/


boolean GnDluBlock(void)
{
    if (GnDluIQueue.count > 0)
        return(FALSE);
    else
        return(IsClsdriverRunning);
} /* of function GnDluBlock */ 


forktype GnDluMain (void)
{
    word retCode;
    hword primitive;
    TCLSIMsg* theMsg;
    boolean dataFlag;

    IsClsdriverRunning = TRUE;

    while (TRUE) {

	edisms((blockproc*)GnDluBlock, 0L);
	if (!IsClsdriverRunning)
	    break;

	if (!(theMsg = p_dequeue(&GnDluIQueue)))
	    continue;

	primitive = CLSIMsgGetPrimitiveCode(theMsg);

 	dataFlag = FALSE;
	    
	switch(primitive) {
	case CLS_ENABLE | CLS_CONFIRM:
        {   /* activate listening saps */ 
	    retCode = CLSIMsgGetReturnCode(theMsg);
	    if (retCode != CLS_OK) {
	    	errmsg(&msgsym(ERRRTN,CLSDR), "Enable ",  retCode);
	    	CLSIMsgDestroy(theMsg);
		break;
            /* DO CLEANING ! port structure should be destroyed here. */ 
	    }
	    GnDLUSendActListningSaps(theMsg);
	    CLSIMsgDestroy(theMsg);
	    break;
        }		
        
        
	case CLS_ACTIVATE_SAP | CLS_CONFIRM:
	{
            GnSapStruct* thisUSapId;
            
            ActSapCnf_t *ActSapCnfMsg = (ActSapCnf_t *)theMsg;
	    retCode = CLSIMsgGetReturnCode(theMsg);
        
	    if (retCode == CLS_OK) { 
                thisUSapId = (GnSapStruct *)ActSapCnfMsg->fCLSIHdr.fIdentifier; 
                thisUSapId->PSapId = ActSapCnfMsg->fCLSIHdr.fPSapId; 
                thisUSapId->fType2Length =  ActSapCnfMsg->fType2Length; 
                if (!thisUSapId ->IsIncoming) { 
                    GnDLUSendReqOpnStn(thisUSapId->pCep); 
                } /* of if */            
                CLSIMsgDestroy(theMsg);
                break; 
	    } /* of if (retCode == CLS_OK) */ 
         
            errmsg(&msgsym(ERRRTN,CLSDR), " ActSap ",  retCode);
            thisUSapId = (GnSapStruct *)ActSapCnfMsg->fCorrelator; /* unfortunately
                       fIdentifier is not giving correct value, problem with cls! */

            if (thisUSapId ->IsIncoming) { 
                buginf(" catastrophic problem with enabling of listening sap \n");
                CLSIMsgDestroy(theMsg);
                break; 
            }                
            
            (*thisUSapId->pCep->pDlu->connectRet)(thisUSapId->pCep->pArgPointer, TRUE);
            GnDLURemoveCep(thisUSapId->pDluPort, thisUSapId->pCep);
            free(thisUSapId->pCep);
            free(thisUSapId); 
            CLSIMsgDestroy(theMsg);
            break; 
	} /* of case */ 
         
        
	case CLS_REQ_OPNSTN | CLS_CONFIRM:
	{
            ReqOpnStnCnf_t  *reqopnstnCnf = (ReqOpnStnCnf_t *)theMsg; 
            GnDLUCep*  theCep; 

	    retCode = CLSIMsgGetReturnCode(theMsg);
            if (retCode != CLS_OK) {
                errmsg(&msgsym(ERRRTN ,CLSDR), " ReqOpnStn ", retCode);
                theCep = (GnDLUCep *)reqopnstnCnf->fCLSIHdr.fIdentifier; 
                GnDLUSendDeactSap(theCep->pSapStruct); /* currently only sending deact sap, 
                                                          may require other action */ 
                CLSIMsgDestroy(theMsg);
                break;
              /* DO CLEANING, WHAT'S THE REASON OF FAILURE, TAKE SUITABLE CORRECTIVE
                 ACTION */ 
	    };
            theCep = (GnDLUCep *)reqopnstnCnf->fCLSIHdr.fIdentifier; 
            theCep->PCepId = reqopnstnCnf->fCLSIHdr.fPCepId;
            if (theCep->pSapStruct->IsIncoming)  {
#if IS_XID
                 GnDLUSendIDRsp(theCep,NULL,0);
#else  
                 GnDLUSendConnectRsp(theCep); 
#endif
	    } /* of if */ 
            else { 
#if IS_XID
                 GnDLUSendIDReq(theCep, NULL,0);
#else
                 GnDLUSendConnectReq(theCep);
#endif
	    }  /* of else */ 
	    CLSIMsgDestroy(theMsg);
            break; 
        } /* of case */ 

        
                
	case CLS_CONNECT | CLS_CONFIRM: 
	{
        
            ConnectCnf_t *connectCnf = (ConnectCnf_t *)theMsg; 
            GnDLUCep* theCep = (GnDLUCep *)connectCnf->fCLSIHdr.fIdentifier; 

	    retCode = CLSIMsgGetReturnCode(theMsg);
            if (retCode != CLS_OK) {
                errmsg(&msgsym(ERRRTN, CLSDR), "ConnectConfirm ", retCode);
                GnDLUSendCloseStnReq(theCep);                
	    }
            else { 
 
                if (! theCep->pSapStruct->IsIncoming) {  /* connecting side */ 
               
                    (*theCep->pDlu->connectRet)(theCep->pArgPointer, FALSE); /* for stun it will be 
                                                                           pointer to stp */ 
                    GnDLUSendSignalStn(theCep); 
		} /* of if */ 
	    } /* of else  */ 
	    CLSIMsgDestroy(theMsg);
            break; 
        }  
                  
         
                
	case CLS_CONNECT_STN | CLS_INDICATION:
	{
            ConnectStnInd_t *connectStnInd = (ConnectStnInd_t *)theMsg; 
            GnSapStruct* thisUSapId = (GnSapStruct *)connectStnInd->fCLSIHdr.fIdentifier; 
            GnDLUCep* thisCep;
            GnDLUPort* thisPort = thisUSapId->pDluPort; 
            hwaddrtype address; 

            if (!thisPort) { 
                errmsg(&msgsym(INCORRECTPORT,CLSDR));
                CLSIMsgDestroy(theMsg);
                break; 
	    }

            thisCep = GnDLUCepCreate();
            /* thisCep = p_dequeue(&qFreeCepPool); */ 

            if (!thisCep) { 
                errmsg(&msgsym(NOCEP,CLSDR));
	        CLSIMsgDestroy(theMsg);
                break; 
	    }

            CLSIConnectStnGetLocalMac(connectStnInd, &address);
            snpa_copy(&address,&thisCep->LocalHWAddress);

            thisCep->RPeerSap = connectStnInd->fRSap;
            thisCep->pSapStruct = thisUSapId;
            thisCep->pDlu = thisUSapId->pDluPort->pDlu;
            p_enqueue(&thisUSapId->pDluPort->qCepList, thisCep);

            thisCep->pSapStruct->IsIncoming = TRUE; 
            thisCep->Priority = 0 ; 
            GnDLUSendReqOpnStn(thisCep); 
	    CLSIMsgDestroy(theMsg);
            break; 
        }



#if IS_XID                
	case CLS_ID_STN | CLS_INDICATION:
	{
            IdStnInd_t *idStnInd = (IdStnInd_t *)theMsg; 
            GnSapStruct* thisUSapId = (GnSapStruct *)idStnInd->fCLSIHdr.fIdentifier; 
            GnDLUCep* thisCep;
            GnDLUPort* thisPort = thisUSapId->pDluPort; 
            hwaddrtype address; 

            if (!thisPort) { 
                errmsg(&msgsym(INCORRECTPORT,CLSDR));
                CLSIMsgDestroy(theMsg);
                break; 
	    }

            thisCep = GnDLUCepCreate();
            /* thisCep = p_dequeue(&qFreeCepPool); */ 

            if (!thisCep) { 
                errmsg(&msgsym(NOCEP,CLSDR));
	        CLSIMsgDestroy(theMsg);
                break; 
	    }

            CLSIIdStnGetLocalMac(idStnInd, &address);
            snpa_copy(&address,&thisCep->LocalHWAddress);

            thisCep->RPeerSap = idStnInd->fRSap;
            thisCep->pSapStruct = thisUSapId;
            thisCep->pDlu = thisUSapId->pDluPort->pDlu;
            p_enqueue(&thisUSapId->pDluPort->qCepList, thisCep);

            thisCep->pSapStruct->IsIncoming = TRUE; 
            thisCep->Priority = 0 ; 
            GnDLUSendReqOpnStn(thisCep); 
	    CLSIMsgDestroy(theMsg);
            break; 
        }

       
        case CLS_ID | CLS_CONFIRM:
        {
            IdCnf_t *idCnf = (IdCnf_t *)theMsg;
            GnDLUCep* theCep; 

            theCep = (GnDLUCep *)idCnf->fCLSIHdr.fIdentifier;

            retCode = CLSIMsgGetReturnCode(theMsg);
            if (retCode != CLS_OK) {
                errmsg(&msgsym(ERRRTN,CLSDR), "IdConfirm ", retCode);
                GnDLUSendCloseStnReq(theCep);   
                CLSIMsgDestroy(theMsg);
                break;
            };

            GnDLUSendConnectReq(theCep);
            CLSIMsgDestroy(theMsg);
            break;
        
	}


        case CLS_CONNECT | CLS_INDICATION:
        {
            ConnectInd_t *connectInd = (ConnectInd_t *)theMsg;  
            GnDLUCep* theCep; 

            theCep = (GnDLUCep *)connectInd->fCLSIHdr.fIdentifier;
            GnDLUSendConnectRsp(theCep);
            CLSIMsgDestroy(theMsg);
            break;
                        
	}
#endif

        case CLS_DISCONNECT | CLS_CONFIRM:
        {
            DisconnectCnf_t *disconnectCnf = (DisconnectCnf_t *)theMsg;
            GnDLUCep* theCep; 
	    retCode = CLSIMsgGetReturnCode(theMsg);
            if (retCode != CLS_OK) {
                errmsg(&msgsym(ERRRTN ,CLSDR), "DisconnectConfirm ",retCode);
                CLSIMsgDestroy(theMsg);
                break;
	    };

            theCep = (GnDLUCep *)disconnectCnf->fCLSIHdr.fIdentifier;
            if (theCep)
                GnDLUSendCloseStnReq(theCep); 
	    CLSIMsgDestroy(theMsg);
            break; 
	}
    
    
        case CLS_CLOSE_STN | CLS_CONFIRM:
        {
            CloseStnCnf_t* closestnCnf = (CloseStnCnf_t *)theMsg; 
            GnDLUCep* thisCep = (GnDLUCep *)closestnCnf->fCLSIHdr.fIdentifier;
            
            if (! thisCep->pSapStruct->IsIncoming) { /* initially connecting side */ 
                GnDLUSendDeactSap(thisCep->pSapStruct) ; 
	    } /* of if */ 
            else { 
               GnDLURemoveCep(thisCep->pSapStruct->pDluPort, thisCep);         
               GnDLUCepDestroy(thisCep); 
	    } /* of else */
            CLSIMsgDestroy(theMsg);
            break;
        }             


	case CLS_DISABLE | CLS_CONFIRM:
	{
	    DisableCnf_t *disableCnf = (DisableCnf_t *)theMsg;
	    GnDLUPort *thisPort; 
            GnDLUEntity *thisDlu; 
            thisPort = (GnDLUPort *) disableCnf->fCorrelator; 
            thisDlu = thisPort->pDlu; 
	    if ((thisPort = GnDLUFindPort(thisDlu, &thisPort->PortId))){
		GnDLURemovePort(thisPort->pDlu, thisPort);
		GnDLUPortDestroy(thisPort);
	    }
	    CLSIMsgDestroy(theMsg);
	    break;
	}


	case CLS_DEACTIVATE_SAP | CLS_CONFIRM:
	{
            DeactSapCnf_t* deactSapCnf= (DeactSapCnf_t *)theMsg; 
            GnSapStruct* thisSap =  (GnSapStruct *) deactSapCnf->fCLSIHdr.fIdentifier; 
            GnDLUPort* thisPort = thisSap->pDluPort; 
         
            if(thisSap->IsIncoming) { /* this side using LISTENING sap */ 
                if (QUEUEEMPTY (&thisPort->qCepList)) { 
	             GnDLUSendDisable(thisPort); 
                }
            } /* of if */     
            else {  /* NOT using listening sap */ 
                
                (*thisSap->pCep->pDlu->connectRet)(thisSap->pCep->pArgPointer, TRUE); 
                       /* in case of error in connecting */ 

                GnDLURemoveCep(thisPort, thisSap->pCep); 
                free(thisSap->pCep);
                free(thisSap);
	    }
           
            CLSIMsgDestroy(theMsg);
	    break;
	}
        
        
        case CLS_DISCONNECT | CLS_INDICATION:
        {
            DisconnectInd_t *disconnectInd= (DisconnectInd_t *)theMsg;
            GnDLUCep* thisCep=(GnDLUCep *)disconnectInd->fCLSIHdr.fIdentifier;
            GnDLUSendDiscRsp(thisCep);
            (*thisCep->pDlu->fini)(thisCep->pArgPointer); /* clear stp structures etc.. ) */ 
            GnDLUSendCloseStnReq(thisCep); 
	    CLSIMsgDestroy(theMsg);
            break; 
        }


	case CLS_DATA | CLS_INDICATION:
        {
            DataInd_t* dataInd = (DataInd_t *)theMsg; 
            GnDLUCep *thisCep = (GnDLUCep *)dataInd->fCLSIHdr.fIdentifier; 
            paktype* pak = CLSIMsgGetPak(theMsg); 
            size_t DataLength;
            boolean MayNeedPriorityChange = FALSE;
            /* there's no need of seperate functions for first packet and rest of the data. 
             * In case of first packet, no stp is known (CEP structure will have NULL for stp
             * pointer). In this case stcls_findpeer will be called to know the corresponding
             * stp and this value should be updated in CEP structure so that subsequent data
             * knows the stp. No TCP like complications are needed.
             * Remeber : If thisCep->pArgPointer is NULL than process like first packet else
             *               current stp pointer is assumed. Add error checking later on.
             *
             */

            if (thisCep->IsFirstPacket && thisCep->pSapStruct->IsIncoming) {
                thisCep->pArgPointer = NULL;
                MayNeedPriorityChange = TRUE;
            }		

            pak->datagramstart = CLSIDataIndGetData(dataInd, &DataLength); 
            pak->datagramsize  = DataLength;

            (*thisCep->pDlu->rddata)(thisCep, pak);

            if (MayNeedPriorityChange && thisCep->Priority) { 
                GnDLUSendModifyStnReq(thisCep);
            }
/*	    CLSIMsgDestroy(theMsg); Do not destroy, rddata will do this, if required */ 
            break; 
	}

	case CLS_DATA | CLS_CONFIRM:
        {
            DataCnf_t* dataCnf = (DataCnf_t *)theMsg; 
            GnDLUCep* thisCep=(GnDLUCep *)dataCnf->fCLSIHdr.fIdentifier;	  

            retCode = CLSIMsgGetReturnCode(theMsg);
            if (retCode != CLS_OK) {
                errmsg(&msgsym(ERRRTN ,CLSDR), "DataConfirm ",  retCode);
                (*thisCep->pDlu->senderErr)(thisCep->pArgPointer); 
	    } ; 
            /* no else, there's no data_confirm if the data is sent correctly */ 

	    CLSIMsgDestroy(theMsg);
            break; 
	}

        case CLS_CONNECTED | CLS_INDICATION: 
	{
                         
	    CLSIMsgDestroy(theMsg);
            break; 
	}

        case CLS_FLOW | CLS_INDICATION:
        {
            CLSIMsgDestroy(theMsg);
            break;
        }

        case CLS_MODIFY_STN | CLS_CONFIRM:
        {
           CLSIMsgDestroy(theMsg);
           break;
        }

	default:
	{
            errmsg(&msgsym(WRONGMSG,CLSDR),primitive);             
	    CLSIMsgDestroy(theMsg);
            break; 
	}
    }
    } /* of while (TRUE) */

/*    GnShutdown(); */

    CLSClose(pWriteQ);
    CLSIQueueDestroy(pReadQ);
    CLSIQInitDestroy(pQinit);
    process_kill(THIS_PROCESS);
  
} /* of function GnDluMain */ 


/* copied from fr_copy_snpa function of the wan subsystem */ 
void hwaddr_to_dlci (hwaddrtype *address, ushort* dlci)
{

    bcopy((uchar *)&address->addr, (uchar *)dlci, STATIONLEN_FR10);

} /* of function hwaddr_to_dlci */ 


void dlci_to_hwaddr(ushort* dlci, hwaddrtype* address)
{

    uchar TempAddr[IEEEBYTES];
    memset(TempAddr, 0, IEEEBYTES);
    TempAddr[0] = *dlci >> 8;
    TempAddr[1] = *dlci & 0xFF;
    snpa_init(address, STATION_FR10, TARGET_UNICAST, TempAddr);

} /* of function dlci_to_hwaddr */ 
   


void GnDLUSendIDReq(GnDLUCep* theCep, byte* xidData, size_t xidLength)
{
    IdReq_t* idReq;
    
    idReq = (IdReq_t*) CLSIMsgCreateHeader(CLS_ID|CLS_REQUEST,
					   offsetof(IdReq_t, fXID) -
					   CLS_ID_REQ_HDR_SZ +
					   xidLength,
					   CLS_LLC_LAYER,
					   CLS_P_CEP_ID,
					   theCep->PCepId);
    if (idReq == NULL)
    {
	errmsg(&msgsym(MSGERR,CLSDR), " SendIDReq " );
	return;
    }
    
    idReq->fVersion = CLSI_VERSION;
    idReq->fCorrelator = 0xdead;
    idReq->fReserveLength = theCep->pSapStruct->fType2Length;
    bcopy(xidData, &idReq->fXID[0], xidLength);

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) idReq);
    
} /* of function GnDLUSendIDReq */ 



void GnDLUSendIDRsp(GnDLUCep* theCep, byte* xidData, size_t xidLength)
{
    IdRsp_t* idRsp;

    idRsp = (IdRsp_t*) CLSIMsgCreateHeader(CLS_ID | CLS_RESPONSE,
					   offsetof(IdRsp_t, fXID) -
					   CLS_ID_RSP_HDR_SZ +
					   xidLength,
					   CLS_LLC_LAYER,
					   CLS_P_CEP_ID,
					   theCep->PCepId);
    if (idRsp == NULL)
    {
	errmsg(&msgsym(MSGERR,CLSDR), " SendIDRsp "); 
	return;
    }
    
    idRsp->fVersion = CLSI_VERSION;
    idRsp->fCorrelator = 0xdead;
    idRsp->fReserveLength = theCep->pSapStruct->fType2Length;
    bcopy(xidData, &idRsp->fXID[0], xidLength);

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) idRsp);
    
} /* of function GnDLUSendIDRsp */ 




void GnDLUSendConnectRsp(GnDLUCep* theCep)
{
    ConnectRsp_t* connectRsp;
    ReqOpnStnLLCParms_t* llcParms;
    size_t parmSize = sizeof(ReqOpnStnLLCParms_t);
    
    connectRsp = (ConnectRsp_t*) CLSIMsgCreateHeader(
				CLS_CONNECT | CLS_RESPONSE,
				offsetof(ConnectRsp_t, fParms) -
				CLS_CONNECT_RSP_HDR_SZ + parmSize,
				CLS_LLC_LAYER,
				CLS_P_CEP_ID,
				theCep->PCepId);
    if (connectRsp == NULL)
    {
	errmsg(&msgsym(MSGERR, CLSDR), " SendConnectRsp " );
	return;
    }
    
    connectRsp->fCLSIHdr.fModifyFlags = 0;

    llcParms = (ReqOpnStnLLCParms_t*) &connectRsp->fParms[0];

    connectRsp->fCLSIHdr.fActionCode = 0;
    connectRsp->fVersion = CLSI_VERSION;
    connectRsp->fCorrelator = 0xdead;

    CLSIQueuePut(pWriteQ, (TCLSIMsg*) connectRsp);
} /* of function GnDLUSendConnectRsp */ 



static void clsdriver_init (subsystype *subsys)
{

    /* initializing the default values */ 
    aListningSaps[0] = LISTNING_SAP_0; 
    aListningSaps[1] = LISTNING_SAP_1; 
    aListningSaps[2] = LISTNING_SAP_2;  
    aListningSaps[3] = LISTNING_SAP_3; 
    UserConfiguredLisnSap = 0;
    IsClsdriverRunning = FALSE;

} /* of function clsdriver_init */ 


/*
 * CLSDRIVER subsystem header
 */

#define CLSDRIVER_MAJVERSION 1
#define CLSDRIVER_MINVERSION 0
#define CLSDRIVER_EDITVERSION  1

SUBSYS_HEADER(clsdriver, CLSDRIVER_MAJVERSION, CLSDRIVER_MINVERSION, CLSDRIVER_EDITVERSION,
              clsdriver_init, SUBSYS_CLASS_PROTOCOL,
              "seq: cls",
              "req: cls");










