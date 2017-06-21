/* $Id: dlu.c,v 3.4.20.2 1996/05/17 10:42:58 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlu.c,v $
 *------------------------------------------------------------------
 * Data Link User Utility Routines
 *
 * September 1994, Meng Lyu
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: dlu.c,v $
 * Revision 3.4.20.2  1996/05/17  10:42:58  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.6.2  1996/04/26  07:33:16  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4.6.1  1996/04/03  13:55:00  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.20.1  1996/03/18  19:10:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/07  08:41:52  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  00:22:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/01  06:00:37  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/08  17:22:22  sberl
 * CSCdi44998:  Removing FRAS/BAN from subinterface crashes router
 * 1) Added checking for fSapState before calling functions to activate
 * and deactivate the sap. Don't want to deactivate a sap that is not
 * active, nor activate one that is already active. Defined an enum to
 * represent the sap state instead of useing 0 and 1.
 *
 * 2) Fix loop in DLURemoveBanCep so that when list is modified from
 * within the loop, we start again back at the beginning of the list.
 *
 * 3) Validate cep in close_stn.cnf to handle case where DLU did not wait
 * for confirm before deleting the cep control block. If we get a confirm
 * for a cep that doesn't exist any more, just drop the confirm message.
 *
 * 4) Generic cleanup and commenting.
 *
 * Revision 3.2  1995/11/17  08:56:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  20:53:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/10/24  23:48:11  vrawat
 * CSCdi42311:  FRAS to SDLC does not recover properly
 *
 * Revision 2.7  1995/10/11  04:20:22  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.6  1995/09/13  22:29:37  sberl
 * CSCdi40289:  FRAS backup doesnt clean up vring
 * Copy portID from enable.cnf into port structure. Use this in the
 * disable.req. Do some code cleanup while in the area.
 *
 * Revision 2.5  1995/07/20  21:13:27  kchiu
 * CSCdi37258:  FRAS Dial-Backup for multiple stations failed
 *
 * Revision 2.4  1995/07/02  05:15:41  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.3  1995/06/18  19:40:04  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  19:27:09  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.1  1995/06/07  20:20:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




#include "master.h"
#include <ciscolib.h>
#include "address.h"
#include "../cls/clsi_msg.h"
#include "../cls/clsi_queue.h"
#include "config.h"
#include "ieee.h"
#include "interface_generic.h"
#include "parser_defs_dlcsw.h"
#include "../if/rif.h"
#include "dlu.h"
#include "dlcsw.h"


static void DLUCepReset (TDLUCep *theCep)
{
    theCep->fPCepId = 0;
    theCep->fCepState = LS_RESET;
    theCep->fRole = 0;
    theCep->fRifLength = 0;
    theCep->fXidPtr = NULL;
    theCep->fXidState = 0;
    theCep->fMsg = NULL;
    theCep->fSendWindow = 0;
    theCep->fConnectionInit = 0;
}

void DLUInit (TDLUEntity *this)
{

    this->fQinit = NULL;
    this->fReadQ = NULL;
    this->fWriteQ = NULL;
    this->fNext = NULL;

    queue_init(&this->fPortList, 0);
    queue_init(&this->fSapList, 0);
    queue_init(&this->fCepList, 0);
    queue_init(&this->fCvtList, 0);
    queue_init(&this->fBanCmdList, 0);

}

void DLUShutdown (TDLUEntity *this)
{
    TDLUPort* thePort;
    TDLUSap* theSap;
    TDLUCep* theCep;
    TDLUCvt* theCvt;
    TDLUBan* theBanCmd;

    CLSClose(this->fWriteQ);
    CLSIQueueDestroy(this->fReadQ);
    CLSIQInitDestroy(this->fQinit);

    while ((thePort = dequeue(&this->fPortList))) 
	DLUPortDestroy(thePort);

    while ((theSap = dequeue(&this->fSapList)))
	DLUSapDestroy(theSap);

    while ((theCep = dequeue(&this->fCepList)))
	DLUCepDestroy(theCep);

    while ((theCvt = dequeue(&this->fCvtList)))
	DLUCvtDestroy(theCvt);

    while ((theBanCmd = dequeue(&this->fBanCmdList)))
	DLUBanCmdDestroy(theBanCmd);
}

void DLURemoveBanCmd(TDLUEntity* this, TDLUBan* aBanCmd)
{
    if (checkqueue(&this->fBanCmdList, aBanCmd)) {
	p_unqueue(&this->fBanCmdList, aBanCmd);
    } else {
	dlcSwErrDebug("\nDLURemoveBanCmd: BAN 0x%x is not on list", aBanCmd);
    }
}

TDLUBan* DLUBanCmdCreate(TDLUBan* theBanCmd)
{
    TDLUBan* this;
    uchar i;

    this = malloc(sizeof(TDLUBan));
    if (this != NULL) {
	this->fNext = NULL;
        this->fIdb = theBanCmd->fIdb;
        this->fLan_idb = theBanCmd->fLan_idb;
	ieee_copy(theBanCmd->fDlciMacAddr, this->fDlciMacAddr);
        ieee_copy(theBanCmd->fBniMacAddr, this->fBniMacAddr);
        this->fLocalRing = theBanCmd->fLocalRing;
        this->fTargetRing = theBanCmd->fTargetRing;
        this->fBridgeNumber = theBanCmd->fBridgeNumber;
        this->fTotalDlci = theBanCmd->fTotalDlci;
        this->fType = theBanCmd->fType;
	for (i = 0; i < this->fTotalDlci; i++) {
            this->fDlci[i].number = theBanCmd->fDlci[i].number;
            this->fDlci[i].session = 0;
            this->fDlci[i].status = DLU_DLCI_ACTIVE;
        };
        this->fAvailDlci = 0;
    }
    return(this);
}

TDLUCvt*  DLURemoveCvt(TDLUEntity* this, TDLUCvt* aConn)
{
    if (checkqueue(&this->fCvtList, aConn)) {
	p_unqueue(&this->fCvtList, aConn);
    } else {
	dlcSwErrDebug("\nDLURemoveConn: CONN 0x%x is not on list", aConn);
    }
    return aConn;    
}

void DLUAddCep(TDLUEntity* this, TDLUCep* aCep)
{
    p_enqueue(&this->fCepList, aCep);
    aCep->fSapPtr->fRefCount ++;
}

TDLUCep*  DLURemoveCep(TDLUEntity* this, TDLUCep* aCep)
{
    
    if (checkqueue(&this->fCepList, aCep)) {
	p_unqueue(&this->fCepList, aCep);
        aCep->fSapPtr->fRefCount --;
    } else {
	dlcSwErrDebug("\nDluRemoveCep: CEP 0x%x is not on list", aCep);
    }
    return aCep;    
}


void DLUAddSap(TDLUEntity* this, TDLUSap* aSap)
{
    p_enqueue(&this->fSapList, aSap);
    aSap->fPortPtr->fRefCount ++;
}

TDLUSap*  DLURemoveSap(TDLUEntity* this, TDLUSap* aSap)
{
    
    if (checkqueue(&this->fSapList, aSap)) {
	p_unqueue(&this->fSapList, aSap);
	aSap->fPortPtr->fRefCount --;
    } else {
	dlcSwErrDebug("\nDluRemoveSap: sap 0x%x not on list", aSap);
    }
    
    return (aSap);
}


void DLUAddPort(TDLUEntity* this, TDLUPort* thePort)
{
    p_enqueue(&this->fPortList, thePort);
}

TDLUPort*  DLURemovePort(TDLUEntity* this, TDLUPort* thePort)
{
    if (checkqueue(&this->fPortList, thePort)) {
	p_unqueue(&this->fPortList, thePort);
    } else {
	dlcSwErrDebug("\nDLURemovePort: port 0x%x not on list",
	       thePort);
    }
    return thePort;
}

static boolean DLUPortIdEqual (CLSPortID* portA, CLSPortID* portB)
{
    switch (portB->type)
    {
	case CLS_P_TYPE_SWIDB:
	    if (portA->fTypeInfo.swidb != portB->fTypeInfo.swidb)
		return(FALSE);
            break;
	default:
	    return(FALSE);
    }
    return(TRUE);
}

TDLUPort* DLUFindPort(TDLUEntity* this, CLSPortID* portID)
{
    TDLUPort* thePort = (TDLUPort*) this->fPortList.qhead;
    while (thePort != NULL)
    {
	if (DLUPortIdEqual(&thePort->fPortId, portID)) {
	    break;
	}
	thePort = thePort->fNext;
    }
    return(thePort);
}

TDLUPort* DLUPortCreate(void)
{
    TDLUPort* this = malloc(sizeof(TDLUPort));
    if (this != NULL) {
	this->fNext = NULL;
	this->fPortState = DLU_PORT_STATE_RESET;
        this->fPortId.type = CLS_P_TYPE_ILLEGAL;
        this->fDLCType = 0;
    }
    return (this);
}

void DLUPortDestroy(TDLUPort* this)
{
    free(this);
}

TDLUSap* DLUSapCreate(TDLUPort* thePort)
{
    TDLUSap* this;

    this = malloc(sizeof(TDLUSap));
    if (this != NULL) {
	this->fNext = NULL;
	this->fPortPtr = thePort;
	this->fDlu = thePort->fDlu;
        CLSIPortIdCopy(&thePort->fPortId, &this->fPortId);
        this->fDLCType = thePort->fDLCType;
	this->fUSapId = (word)this;
	this->fPSapId = 0;
	this->fSapState = DLU_SAP_STATE_RESET;
	this->fSapNumber = 0;
    }
    return (this);
}

void DLUSapDestroy(TDLUSap* this)
{
    free(this);
}

TDLUCep* DLUCepCreate(TDLUSap* theSap)
{
    TDLUCep* this;
    this = malloc(sizeof(TDLUCep));
    if (this != NULL) {
	this->fNext = NULL;
	this->fDlu = theSap->fDlu;
	this->fCvtPtr = NULL;
	this->fRole = 0;
	this->fSapPtr = theSap;
	this->fPCepId = 0;
	this->fUCepId = (word) this; 
	this->fCepState = LS_RESET;
	this->fFlags = 0;
	this->fSendWindow = 0;
        this->fCepOrigPtr = NULL;
        this->fConnectionInit = 0 ; /*don't know */ 
    }
    return(this);
}

void DLUCepDestroy(TDLUCep* this)
{
    free(this);
}

TDLUCvt* DLUCvtCreate(TDLUEntity *dlu)
{
    TDLUCvt* this;

    this = malloc(sizeof(TDLUCvt));

    if (this != NULL) {
	this->fNext = NULL;
	this->fDlu = dlu;
	this->fPriCep = NULL;
	this->fSecCep = NULL;
	this->fCvtState = C_IDLE;
    }
    return(this);
}

void DLUBanCmdDestroy(TDLUBan* this)
{
    free(this);
}

void DLUCvtDestroy(TDLUCvt* this)
{
    free(this);
}

void DLUAddBanCmd(TDLUEntity* this, TDLUBan* theBanCmd)
{
    p_enqueue(&this->fBanCmdList, theBanCmd);
}

void DLUAddCvt(TDLUEntity* this, TDLUCvt* theConn)
{
    p_enqueue(&this->fCvtList, theConn);
}

void DLUSendPortEnable(TDLUEntity* theDlu, TDLUPort* this, uchar* aVMacAddr,
		       hword aLocalRing, hword aBridgeNumber, hword aTargetRing)
{
    uint parmSize;
    EnableReq_t*   enableReq;

    switch (this->fDLCType)	{
    case CLS_LLC: 
	parmSize = sizeof(EnableTRParms_t); 
	break;

    case CLS_FRAME_RELAY:
	parmSize = sizeof(EnableFRParms_t); 
	break;

    case CLS_SDLC: 
    	parmSize = sizeof(EnableSDLCParms_t); 
    	break;

    case CLS_RSRB:
        parmSize = sizeof(EnableRSRBParms_t);

    default:
	parmSize = 0;
	break;
    }
    
    enableReq = (EnableReq_t*) CLSIMsgCreateHeader(
				CLS_ENABLE | CLS_REQUEST,
				offsetof(EnableReq_t,fParms) -
				CLS_ENABLE_REQ_HDR_SZ + parmSize,
				CLS_LLC_LAYER,
				CLS_ENTITY_MGR,
				0);
    if (enableReq == NULL)
    {
	dlcSwErrDebug("\nDLU: SendEnable() couldn't get a message");
	return;
    }
    
    CLSIPortIdCopy(&this->fPortId, &enableReq->fPortId);

    /* 
     * We will use this later to correlate the ENABLE.cnf 
     * with the proper TDLUPort object.
     */
    enableReq->fCorrelator = (word) this;

    switch (this->fDLCType)	{
    case CLS_LLC:
    {
	EnableTRParms_t* trParms;
	enableReq->fDLCType = this->fDLCType;

	trParms = (EnableTRParms_t*) &(enableReq->fParms);
	trParms->fVersion = CLSI_VERSION;

	/* Use defaults for everything, for now. */
	snpa_clear(&trParms->fLocalMac);
	trParms->fTrSpeed = 0;
	trParms->fMaxIFieldLength = 0;
	break;
    }
    case CLS_FRAME_RELAY:
    {
	enableReq->fDLCType = this->fDLCType;
	
	break;
    }
    case CLS_SDLC:
    {
	EnableSDLCParms_t *sdlcParms;
	enableReq->fDLCType = this->fDLCType;

	sdlcParms = (EnableSDLCParms_t *) &(enableReq->fParms);
	sdlcParms->fVersion = CLSI_VERSION;
	break;
    }
    case CLS_RSRB:
    {
        EnableRSRBParms_t *rsrbParmP = NULL;
        
        enableReq->fDLCType = CLS_RSRB;
        rsrbParmP = (EnableRSRBParms_t *) &(enableReq->fParms[0]);
        rsrbParmP->fVersion = CLSI_VERSION;
        snpa_init(&rsrbParmP->fVMacAddr, STATION_IEEE48, TARGET_UNICAST,
            aVMacAddr);
        rsrbParmP->fMaxIFieldLength = 0;
        rsrbParmP->fLocalRing = aLocalRing;
        rsrbParmP->fBridgeNumber = aBridgeNumber;
        rsrbParmP->fTargetRing = aTargetRing;
        rsrbParmP->fRsrbOptionFlags = 0;
        break;
    }
    default:
	parmSize = 0;
	break;
    }

    CLSIQueuePut(theDlu->fWriteQ, (TCLSIMsg*) enableReq);
    
}


void DLUSendPortDisable(TDLUEntity* theDlu, TDLUPort* this)
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
	dlcSwErrDebug("\nDLU: SendDisable() couldn't get a message");
	return;
    }
    
    CLSIPortIdCopy(&this->fPortId, &disableReq->fPortId);
    disableReq->fDLCType = this->fDLCType;

    /* 
     * We will use this later to correlate the DISABLE.cnf 
     * with the proper TDLUPort object.
     */
    disableReq->fCorrelator = (word) this;

    CLSIQueuePut(theDlu->fWriteQ, (TCLSIMsg*) disableReq);
    
}

boolean DLUSendActSap(TDLUEntity* theDlu, TDLUSap *theSap)
{
    ActSapReq_t* actSapReq;
    ActSapLLCParms_t* llcParms;
    ActSapSDLCParms_t* sdlcParms;
    int parmSize;
    
    if (theSap->fDLCType == CLS_SDLC)
	parmSize = sizeof(ActSapSDLCParms_t);
    else 
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
	dlcSwErrDebug("\nDLU: SendActSap() couldn't get a message");
	return FALSE;
    }

    actSapReq->fCLSIHdr.fUSapId = theSap->fUSapId;
    actSapReq->fVersion = CLSI_VERSION;
    actSapReq->fCorrelator = (word) theSap;
    CLSIPortIdCopy(&theSap->fPortId, &actSapReq->fPortId);
    actSapReq->fDLCType = theSap->fDLCType;

    if (theSap->fDLCType == CLS_SDLC) {
	sdlcParms = (ActSapSDLCParms_t*) &(actSapReq->fParms);
	sdlcParms->fSapValue = theSap->fSapNumber;
	sdlcParms->fAddrType = STATION_SDLC;
    } else {
	llcParms = (ActSapLLCParms_t*) &(actSapReq->fParms);
        llcParms->fUserClass = 0x02;
    	llcParms->fSapValue = theSap->fSapNumber;
	llcParms->fT1 = 0;
	llcParms->fT2 = 0;
	llcParms->fTi = 0;
	llcParms->fMaxOut = 0;
	llcParms->fMaxRetryCount = 0;		/* no limit */
	llcParms->fMaxIn = 0;
	llcParms->fMaxIField = 0;
    }

    CLSIQueuePut(theDlu->fWriteQ, (TCLSIMsg*) actSapReq);
    return(TRUE);
}

boolean DLUSendDeactSap(TDLUEntity* theDlu, TDLUSap *theSap)
{
    DeactSapReq_t* deactSapReq;
    
    deactSapReq = (DeactSapReq_t*) CLSIMsgCreateHeader(
				CLS_DEACTIVATE_SAP | CLS_REQUEST,
				CLS_DEACT_SAP_REQ_HDR_SZ,
				CLS_LLC_LAYER,
				CLS_P_SAP_ID,
				theSap->fPSapId);
    if (deactSapReq == NULL)
    {
	dlcSwErrDebug("\nDLU: SendDeactSap() couldn't get a message");
	return FALSE;
    }

    deactSapReq->fCLSIHdr.fUSapId = theSap->fUSapId;
    deactSapReq->fVersion = CLSI_VERSION;
    deactSapReq->fCorrelator = 0xdead;

    CLSIQueuePut(theDlu->fWriteQ, (TCLSIMsg*) deactSapReq);
    return TRUE;
}

/*
 * This function is currently for BAN sdlc only.
 */

boolean DLUSendActRing(TDLUEntity* theDlu, TDLUSap *theSap)
{
    ActRngReq_t* actringReq;
    int parmSize;
    ActRngTRParms_t*    trParms;

    if (theSap->fDLCType == CLS_SDLC)
        parmSize = sizeof(ActRngLLCParms_t);
    else {
        dlcSwErrDebug("\nDLU: SendActRing() support BAN sdlc only!");
        return FALSE;
    }

    actringReq = (ActRngReq_t*) CLSIMsgCreateHeader(
                                CLS_ACTIVATE_RING | CLS_REQUEST,
                                offsetof(ActRngReq_t, fParms) -
                                CLS_ACT_RING_REQ_HDR_SZ +
                                parmSize,
                                CLS_LLC_LAYER,
                                CLS_ENTITY_MGR,
                                0);
    if (actringReq == NULL) {
        dlcSwErrDebug("\nDLU: SendActRing() couldn't get a message");
        return FALSE;
    }
    trParms = (ActRngTRParms_t*) &(actringReq->fParms);
    trParms->fUserClass = 0x02;
    trParms->fSapValue = theSap->fSapNumber;
    trParms->fT1 = 0;
    trParms->fT2 = 0;
    trParms->fTi = 0;
    trParms->fMaxOut = 0;
    trParms->fMaxRetryCount = 0;
    trParms->fMaxIn = 0;
    trParms->fMaxIField = 0;
    actringReq->fCLSIHdr.fUSapId = theSap->fUSapId;
    actringReq->fVersion = CLSI_VERSION;
    actringReq->fCorrelator = (word) theSap;
    actringReq->fDLCType = theSap->fDLCType;
    CLSIPortIdCopy(&theSap->fPortId, &actringReq->fPortId);
    CLSIQueuePut(theDlu->fWriteQ, (TCLSIMsg*) actringReq);
    return TRUE;
}


/*
 * This function is currently for BAN sdlc only.
 */
boolean DLUSendDeactRing(TDLUEntity* theDlu, TDLUSap *theSap)
{
    DeActRngReq_t* deactringReq;

    deactringReq = (DeActRngReq_t*) CLSIMsgCreateHeader(CLS_DEACTIVATE_RING |
                                                        CLS_REQUEST,
                                                        0,
                                                        CLS_LLC_LAYER,
                                                        CLS_P_SAP_ID,
                                                        theSap->fPSapId);

    if (deactringReq == NULL) {
        dlcSwErrDebug("\nDLU: SendDeactRing() couldn't get a message");
        return FALSE;
    }
    deactringReq->fVersion = CLSI_VERSION;
    deactringReq->fCorrelator = 0xdead;
    deactringReq->fCLSIHdr.fUSapId = theSap->fUSapId;
    CLSIQueuePut(theDlu->fWriteQ, (TCLSIMsg*) deactringReq);
    return TRUE;
}

void DLUSendReqOpnStn(TDLUCep* theCep)
{
    ReqOpnStnReq_t* reqOpnStnReq;
    ReqOpnStnLLCParms_t* llcParms;
    ReqOpnStnSDLCParms_t* sdlcParms;
    int parmSize;

    if (theCep->fSapPtr->fDLCType == CLS_SDLC) 
	parmSize = sizeof(ReqOpnStnSDLCParms_t);
    else
	parmSize = sizeof(ReqOpnStnLLCParms_t);
    
    reqOpnStnReq = 
	(ReqOpnStnReq_t*) CLSIMsgCreateHeader(CLS_REQ_OPNSTN | CLS_REQUEST,
					      offsetof(ReqOpnStnReq_t,fParms)
					      -CLS_REQ_OPNSTN_REQ_HDR_SZ
					      +parmSize
					      +theCep->fRifLength,
					      CLS_LLC_LAYER,
					      CLS_P_SAP_ID,
					      theCep->fSapPtr->fPSapId);

    if (reqOpnStnReq == NULL)
    {
	dlcSwErrDebug("\nDLU: SendReqOpnStn() couldn't get a message");
	return;
    }
    
    reqOpnStnReq->fCLSIHdr.fUCepId = theCep->fUCepId;
    reqOpnStnReq->fVersion = CLSI_VERSION;
    reqOpnStnReq->fCorrelator = 0xdead;
    reqOpnStnReq->fQueuingType = CLS_QUEUING_DEFAULT;


/*    if (theCep->fSapPtr->fDLCType == CLS_SDLC) {
	sdlcParms = (ReqOpnStnSDLCParms_t*) &(reqOpnStnReq->fParms); */

    if (theCep->fSapPtr->fDLCType == CLS_SDLC &&
            theCep->fFrasType != FRAS_BAN) {
        sdlcParms = (ReqOpnStnSDLCParms_t*) &(reqOpnStnReq->fParms);


	/*
	 * If this is an incoming call, these came from the idStn.ind
	 */

        snpa_copy(&theCep->fDstAddr, &sdlcParms->fDestAddr);
	snpa_clear(&sdlcParms->fSrcAddr);
    
	sdlcParms->fDestSap = theCep->fSapPtr->fSapNumber;
	sdlcParms->fSrcSap = 0;
	sdlcParms->fExplorerOption = theCep->fExplorerOption;
    
	sdlcParms->fExplorerRetries = 5;
	sdlcParms->fExplorerTimer = 2000;
    
	/*
	 * These are other configuration parameters. Mostly use defaults.
	 */
	sdlcParms->fXIDRetries = 10;
	sdlcParms->fXIDTimer = 5000;
    } else {
    	llcParms = (ReqOpnStnLLCParms_t*) &(reqOpnStnReq->fParms);

    /*
     * If this is an incoming call, these came from the idStn.ind
     */
        snpa_copy(&theCep->fDstAddr, &llcParms->fDestAddr);
    	snpa_clear(&llcParms->fSrcAddr);
        /*
         * This is specific to Frame Relay/RFC1490 connections
	 */


        if ((theCep->fSapPtr->fDLCType == CLS_FRAME_RELAY) &&
            (theCep->fFrasType == FRAS_BAN)) {
            snpa_copy(&theCep->fSrcAddr, &llcParms->fSrcAddr);
            llcParms->fDlci = theCep->fDlci;
            llcParms->fL3Pid = CLS_BAN_FLAG;
        } else { 
	    llcParms->fL3Pid = theCep->fL3Pid;        
	    llcParms->fDlci = 0;
	    if (theCep->fSapPtr->fDLCType == CLS_SDLC)
                snpa_copy(&theCep->fSrcAddr, &llcParms->fSrcAddr);
        }

        llcParms->fDestSap = theCep->fRSap;
    	llcParms->fSrcSap = theCep->fLSap;
    	llcParms->fExplorerOption = theCep->fExplorerOption;
    	llcParms->fRifLength = theCep->fRifLength;
    	if (theCep->fRifLength > 0)
	    bcopy(theCep->fRif, llcParms->fRif, theCep->fRifLength);
	if (theCep->fExplorerOption == EO_INBOUND_DEFAULT)
            llcParms->fRif[1] ^= 0x80;
    
    /*
     * These are other configuration parameters. Mostly use defaults.
     */
    	llcParms->fT1 = 0;
    	llcParms->fT2 = 0;
    	llcParms->fTi = 0;
    	llcParms->fMaxIField = 0;
    	llcParms->fMaxOut = 0;
    	llcParms->fMaxRetryCount = 10;
    	llcParms->fMaxIn = 0;
    	llcParms->fDynamicWindow = 0;
    	llcParms->fMaxOutIncr = 0;
    	llcParms->fXIDRetries = 10;
    	llcParms->fXIDTimer = 5000;
    	llcParms->fExplorerRetries = 5;
    	llcParms->fExplorerTimer = 2000;

    }
    
    CLSIQueuePut(theCep->fDlu->fWriteQ, (TCLSIMsg*) reqOpnStnReq);
}

void DLUSendIDReq(TDLUCep* theCep, byte* xidData, size_t xidLength)
{
    IdReq_t* idReq;
    
    idReq = (IdReq_t*) CLSIMsgCreateHeader(CLS_ID|CLS_REQUEST,
					   offsetof(IdReq_t, fXID) -
					   CLS_ID_REQ_HDR_SZ +
					   xidLength,
					   CLS_LLC_LAYER,
					   CLS_P_CEP_ID,
					   theCep->fPCepId);
    if (idReq == NULL)
    {
	dlcSwErrDebug("\nDLU: SendIdReq() couldn't get a message");
	return;
    }
    
    idReq->fVersion = CLSI_VERSION;
    idReq->fCorrelator = 0xdead;
    idReq->fReserveLength = 0;
    bcopy(xidData, &idReq->fXID[0], xidLength);

    CLSIQueuePut(theCep->fDlu->fWriteQ, (TCLSIMsg*) idReq);
    
}


void DLUSendIDRsp(TDLUCep* theCep, byte* xidData, size_t xidLength)
{
    IdRsp_t* idRsp;

    idRsp = (IdRsp_t*) CLSIMsgCreateHeader(CLS_ID | CLS_RESPONSE,
					   offsetof(IdRsp_t, fXID) -
					   CLS_ID_RSP_HDR_SZ +
					   xidLength,
					   CLS_LLC_LAYER,
					   CLS_P_CEP_ID,
					   theCep->fPCepId);
    if (idRsp == NULL)
    {
	dlcSwErrDebug("\nDLUTest: SendIdRsp() couldn't get a message");
	return;
    }
    
    idRsp->fVersion = CLSI_VERSION;
    idRsp->fCorrelator = 0xdead;
    idRsp->fReserveLength = 0;
    bcopy(xidData, &idRsp->fXID[0], xidLength);

    CLSIQueuePut(theCep->fDlu->fWriteQ, (TCLSIMsg*) idRsp);
    
}

void DLUSendConnectReq(TDLUCep* theCep)
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
				theCep->fPCepId);
    if (connectReq == NULL)
    {
	dlcSwErrDebug("\nDLU: SendConnectReq() couldn't get a message");
	return;
    }
    
    connectReq->fCLSIHdr.fModifyFlags = 0;
    llcParms = (ReqOpnStnLLCParms_t*) &connectReq->fParms[0];
    if (theCep->fSendWindow != 0) {
	connectReq->fCLSIHdr.fModifyFlags = CLS_MOD_MAXOUT;
	llcParms->fMaxOut = theCep->fSendWindow;
    } 

    connectReq->fVersion = CLSI_VERSION;
    connectReq->fCorrelator = 0xdead;

    CLSIQueuePut(theCep->fDlu->fWriteQ, (TCLSIMsg*) connectReq);
}


void DLUSendConnectRsp(TDLUCep* theCep)
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
				theCep->fPCepId);
    if (connectRsp == NULL)
    {
	dlcSwErrDebug("\nDLU: SendConnectRsp() couldn't get a message");
	return;
    }
    
    connectRsp->fCLSIHdr.fModifyFlags = 0;
    llcParms = (ReqOpnStnLLCParms_t*) &connectRsp->fParms[0];
    if (theCep->fSendWindow != 0) {
	connectRsp->fCLSIHdr.fModifyFlags = CLS_MOD_MAXOUT;
	llcParms->fMaxOut = theCep->fSendWindow;
    } 

    connectRsp->fCLSIHdr.fActionCode = 0;
    connectRsp->fVersion = CLSI_VERSION;
    connectRsp->fCorrelator = 0xdead;

    CLSIQueuePut(theCep->fDlu->fWriteQ, (TCLSIMsg*) connectRsp);
}

void DLUSendSignalStn(TDLUCep* theCep)
{
    SignalStnReq_t* sigStnReq;
    
    sigStnReq = (SignalStnReq_t*)
	CLSIMsgCreateHeader(CLS_SIGNAL_STN | CLS_REQUEST,
			    sizeof(SignalStnReq_t) -
			    CLS_SIGNAL_STN_REQ_HDR_SZ,
			    CLS_LLC_LAYER,
			    CLS_P_CEP_ID,
			    theCep->fPCepId);
    if (sigStnReq == NULL)
    {
	dlcSwErrDebug("\nDLU: SendSignalStn() couldn't get a message");
	return;
    }
    
    sigStnReq->fCLSIHdr.fActionCode = 0; /* start data (exit local */
					 /* busy state) */
    sigStnReq->fVersion = CLSI_VERSION;
    sigStnReq->fCorrelator = 0xdead;


    CLSIQueuePut(theCep->fDlu->fWriteQ, (TCLSIMsg*) sigStnReq);
}

void DLUSendDiscReq(TDLUCep* theCep)
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
				theCep->fPCepId);
    if (disconnectReq == NULL)
    {
	dlcSwErrDebug("\nDLU: SendDiscReq() couldn't get a message");
	return;
    }
    
    disconnectReq->fVersion = CLSI_VERSION;
    disconnectReq->fCorrelator = 0xdead;

    CLSIQueuePut(theCep->fDlu->fWriteQ, (TCLSIMsg*) disconnectReq);
}

void DLUSendDiscRsp(TDLUCep* theCep)
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
				theCep->fPCepId);
    if (disconnectRsp == NULL)
    {
	dlcSwErrDebug("\nDLUTest: SendDiscRsp() couldn't get a message");
	return;
    }
    
    disconnectRsp->fVersion = CLSI_VERSION;
    disconnectRsp->fCorrelator = 0xdead;

    CLSIQueuePut(theCep->fDlu->fWriteQ, (TCLSIMsg*) disconnectRsp);
}

void DLUSendCloseStnReq(TDLUCep* theCep)
{
    CloseStnReq_t* closeStnReq;
    
    closeStnReq = (CloseStnReq_t*) CLSIMsgCreateHeader(CLS_CLOSE_STN | CLS_REQUEST,
					     offsetof(CloseStnReq_t, fCorrelator) -
					     CLS_CLOSE_STN_REQ_HDR_SZ,
					     CLS_LLC_LAYER,
					     CLS_P_CEP_ID,
						     theCep->fPCepId);
    if (closeStnReq == NULL)
    {
	dlcSwErrDebug("\nDLU: SendCloseStnReq() couldn't get a message");
	return;
    }
    
    closeStnReq->fCLSIHdr.fUCepId = theCep->fUCepId;
    closeStnReq->fCLSIHdr.fActionCode = CLS_ACTION_FORCED;
    
    closeStnReq->fVersion = CLSI_VERSION;
    closeStnReq->fCorrelator = 0xdead;

    CLSIQueuePut(theCep->fDlu->fWriteQ, (TCLSIMsg*) closeStnReq);
    DLUCepReset(theCep);
}

void DLUSendFlowRq(TDLUCep *theCep, int set)
{

}















