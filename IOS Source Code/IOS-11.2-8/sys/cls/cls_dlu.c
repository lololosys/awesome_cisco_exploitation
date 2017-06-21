/* $Id: cls_dlu.c,v 3.5.10.4 1996/08/08 03:48:59 mregan Exp $
 * $Source: /release/112/cvs/Xsys/cls/cls_dlu.c,v $
 *------------------------------------------------------------------
 * TCLSDlu class represents a DLU inside of the CLS subsystem.
 *
 * March 1994, Steve Berl
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * TCLSDlu objects represent the individual DLUs inside of CLS. The 
 * object is responsible for hooking together all the resources
 * associated with the DLU which include the TCLSIQueue objects for 
 * sending and receiving TCLSIMessages, and all of the TCLSSap and 
 * TCLSCep objects created on behalf of the DLU.
 *------------------------------------------------------------------
 * $Log: cls_dlu.c,v $
 * Revision 3.5.10.4  1996/08/08  03:48:59  mregan
 * CSCdi49060:  Only first CLS/RSRB virtual port gets closed
 * Branch: California_branch
 *
 * Revision 3.5.10.3  1996/05/17  10:41:47  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.4  1996/05/05  22:51:14  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.2.3  1996/04/26  01:52:25  doner
 * CSCdi55831:  IBU Modularity - CLSI and VDLC converted to new IOS
 * scheduler
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.2  1996/04/03  13:54:08  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.2.1  1996/04/01  23:16:21  fbordona
 * CSCdi52382:  DLSW:CPUHOG in CLS background, PC=0x60549f3c
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.10.2  1996/04/29  10:02:11  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.5.10.1  1996/03/18  19:09:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.3  1996/03/16  06:31:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.20.2  1996/03/07  08:41:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.20.1  1996/02/20  13:41:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/26  22:17:25  kpatel
 * CSCdi46944:  Getting CLS-3-CLSFAIL and DLSWC-3-BADCLSI error messages
 *
 * Revision 3.4  1995/11/30  08:10:15  pmorton
 * CSCdi41999:  Duplicate debug out for CLSI messages
 * Remove duplicate call
 *
 * Revision 3.3  1995/11/21  22:56:35  kpatel
 * CSCdi42053:  DLSWC-3-BADCLSIIDTYPE dlsw_is_bad_clsi
 *
 * Revision 3.2  1995/11/17  08:54:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:10:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  20:52:52  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/10/16  22:54:36  snyder
 * CSCdi42258:  spellink errors
 *              identifer -> identifier
 *              unkown -> unknown
 *
 * Revision 2.9  1995/08/17  22:20:05  rnaderi
 * CSCdi38906:  Data for DLU on the interrupt thread breaks DLSw
 * Backed out CSCdi38906
 *
 * Revision 2.8  1995/08/10  18:26:15  sberl
 * CSCdi38562:  CLSASSERT errors when link going down
 * Fix error handling in CLSDluSendError() so 1) only prints when debug is
 * enabled, and 2) it tells more about the erroneous message, and 3) it
 * cleans up correctly from the error by disposing of the message.
 *
 * Revision 2.7  1995/08/01  01:37:50  lpereira
 * CSCdi37988:  CLS should queue Data for DLU on the interrupt thread
 *
 * Revision 2.6  1995/07/21  17:07:49  sberl
 * CSCdi37437:  Add new ModifyStn primitive to CLSI
 * Just set the right bits in fModifyFlags and put the new values in the
 * parameter structure.
 *
 * Revision 2.5  1995/07/02  05:15:10  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.4  1995/06/21  23:22:45  sberl
 * CSCdi33675:  APPN crashes if DSPU configured for same sap
 * Make sure that error activate_sap.cnf and req_opnstn.cnf have the
 * correct fIdentifier and fIdentifierType values.
 *
 * Revision 2.3  1995/06/15  22:59:33  ppearce
 * CSCdi35921:  CLS/RSRB virtual ring not removed after "appn stop"
 *
 * Revision 2.2  1995/06/15  22:19:19  sberl
 * CSCdi35842:  debug cls error needs more!
 * Add debug messages. Fix some formatting.
 *
 * Revision 2.1  1995/06/07  20:19:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "cls_private.h"

/*
 * Local static constant data
 */
static const byte gDluClassId[] = {'D','L','U',' '};

/*
 * And now for the code itself!
 */

TCLSDlu* CLSDluCreate (void)
{
    TCLSDlu* this = malloc(sizeof(TCLSDlu));
    if (this != NULL)
    {
	this->fNext = NULL;
	bcopy(gDluClassId, this->fClassId, sizeof(this->fClassId));
	this->fWriteQ = NULL;
	this->fReadQ = NULL;
	queue_init(&this->fSapList, 0);
	queue_init(&this->fCepList, 0);
	queue_init(&this->fPendingMsg, 0);
	this->fWorkToDo = FALSE;
	process_set_boolean(gCLSProcess, TRUE); /* Tell process */
    }
    return (this);
}

boolean CLSDluCheck (TCLSDlu* this)
{
    int i;
    for (i = 0; i < sizeof(gDluClassId); i++)
    {
	if (this->fClassId[i] != gDluClassId[i])
	    return(FALSE);
    }
    return(TRUE);
}

void CLSDluDestroy (TCLSDlu* this)
{

    TCLSPort* rsrbPort = NULL;


    CLSASSERT(CLSDluCheck(this));
    while (this->fCepList.count > 0)
    {
	TCLSCep* aCep;

	aCep = p_dequeue(&this->fCepList);
	aCep->fNeedsDLCClose = TRUE;
	CLSCepDestroy(aCep);
    }
    while (this->fSapList.count > 0)
    {
	TCLSSap* aSap;

	aSap = p_dequeue(&this->fSapList);
        if (CLSPortGetType(aSap->fPortPtr) == CLS_RSRB)
            rsrbPort = aSap->fPortPtr;
        else
            rsrbPort = NULL;
        CLSSapCleanup(aSap);
        /*
         * If RSRB port was enabled for this DLU
         *   CLSDluDestroy must remove the vring interface
         *
         */
        if (rsrbPort) {

            idbtype *idb;

            idb = DlcToIdb(&rsrbPort->fPortId);
            if (idb) 
              reg_invoke_cls_remove_vring(idb);
        }
    }

    
    memset(this->fClassId, 0, sizeof(gDluClassId));
    free(this);
}

void CLSDluDoWork (TCLSDlu* this)
{
    TCLSIMsg* theMsg;
    TCLSCep* aCep;
    int count;
    
    CLSASSERT_FATAL(CLSDluCheck(this));

    this->fWorkToDo = FALSE;
    aCep = (TCLSCep*) this->fCepList.qhead;
    while (aCep != NULL)
    {
	if (aCep->fNeedsFlowInd == TRUE)
	    CLSCepSendFlowInd(aCep);
	if (aCep->fNeedsDisconnectInd == TRUE)
	    CLSCepSendDisconnectInd(aCep);
	aCep = aCep->fNext;
    }

    /*
     * Limit the time spent here to PAK_SWITCHCOUNT packets.
     */
    count = PAK_SWITCHCOUNT;
    while ((this->fPendingMsg.count > 0) && (count-- > 0))
    {
	theMsg = p_dequeue(&this->fPendingMsg);
	
	CLSDluSendToDLU(this, theMsg);
    }
    if (this->fPendingMsg.count > 0) 
        CLSDluSetWorkToDo(this);
}

void CLSDluAddCep (TCLSDlu* this, TCLSCep* aCep)
{
    CLSASSERT_FATAL(CLSDluCheck(this));
    CLSASSERT_FATAL(CLSCepCheck(aCep));
    p_enqueue(&this->fCepList, aCep);
}

void CLSDluAddSap (TCLSDlu* this, TCLSSap* aSap)
{
    CLSASSERT_FATAL(CLSDluCheck(this));
    CLSASSERT_FATAL(CLSSapCheck(aSap));
    p_enqueue(&this->fSapList, aSap);
}

TCLSCep* CLSDluRemoveCep (TCLSDlu* this, TCLSCep* aCep)
{
    CLSASSERT_FATAL(CLSDluCheck(this));
    CLSASSERT_FATAL(CLSCepCheck(aCep));
    
    if (checkqueue(&this->fCepList, aCep))
    {
	p_unqueue(&this->fCepList, aCep);
    }
    else
    {
	buginf("\nCLSDluRemoveCep: CEP 0x%x is not on list", aCep);
    }
    return aCep;    
}

TCLSSap* CLSDluRemoveSap (TCLSDlu* this, TCLSSap* aSap)
{
    CLSASSERT_FATAL(CLSDluCheck(this));
    CLSASSERT_FATAL(CLSSapCheck(aSap));
    
    if (checkqueue(&this->fSapList, aSap))
    {
	p_unqueue(&this->fSapList, aSap);
    }
    else
    {
	buginf("\nCLSDluRemoveSap: sap 0x%x not on list", aSap);
    }
    
    return (aSap);
}

void CLSDluSendToCep (TCLSDlu* this, TCLSSap *theSap, TCLSIMsg* theMsg)
{
    TCLSCep* theCep;
    hword primitive;
    
    CLSDebugMessage("DLU", "CEP", CLSDluGetDLUName(this), theMsg);
    
    if (theMsg->fIdentifierType == CLS_P_CEP_ID) {
	theCep = (TCLSCep*) theMsg->fIdentifier;
	if (CLSCepCheck(theCep) == FALSE) {
	    if (cls_debug_err)
		buginf("\nCLSDluSendToCep: CLSCepCheck failed pCepId 0x%x", theCep);
	    theCep = NULL;
	}
    } else {
	theCep = CLSDluFindCepByUCepId(this, theMsg->fIdentifier);
    }

    if (theCep != NULL) {
	primitive = CLSIMsgGetPrimitiveCode(theMsg);
	switch (primitive) {
	case CLS_DATA | CLS_REQUEST:
	    CLSCepDataReq(theCep, (DataReq_t*) theMsg);
	    break;
	case CLS_UDATA | CLS_REQUEST:
	    CLSCepUDataReq(theCep, (UDataReq_t*) theMsg);
	    break;
	case CLS_REQ_OPNSTN | CLS_REQUEST:
	    CLSCepReqOpnStnReq(theCep, theSap, (ReqOpnStnReq_t*) theMsg);
	    break;
	case CLS_ID | CLS_REQUEST:
	    CLSCepIdReq(theCep, (IdReq_t*) theMsg);
	    break;
	case CLS_ID | CLS_RESPONSE:
	    CLSCepIdRsp(theCep, (IdRsp_t*) theMsg);
	    break;
	case CLS_CONNECT | CLS_REQUEST:
	    CLSCepConnectReq(theCep, (ConnectReq_t*) theMsg);
	    break;
	case CLS_CONNECT | CLS_RESPONSE:
	    CLSCepConnectRsp(theCep, (ConnectRsp_t*) theMsg);
	    break;
	case CLS_SIGNAL_STN | CLS_REQUEST:
	    CLSCepSignalStnReq(theCep, (SignalStnReq_t*) theMsg);
	    break;
	case CLS_FLOW | CLS_REQUEST:
	    CLSCepFlowReq(theCep, (FlowReq_t*) theMsg);
	    break;
	case CLS_DISCONNECT | CLS_REQUEST:
	    CLSCepDisconnectReq(theCep, (DisconnectReq_t*) theMsg);
	    break;
	case CLS_DISCONNECT | CLS_RESPONSE:
	    CLSCepDisconnectRsp(theCep, (DisconnectRsp_t*) theMsg);
	    break;
	case CLS_CLOSE_STN | CLS_REQUEST:
	    CLSCepCloseStnReq(theCep, (CloseStnReq_t*) theMsg);
	    break;
	case CLS_OPEN_STN | CLS_REQUEST:
	    CLSCepOpenStnReq(theCep, (OpenStnReq_t*) theMsg);
	    break;
	    
	case CLS_TEST | CLS_REQUEST:
	    CLSCepTestReq(theCep, (TestReq_t*) theMsg);
	    break;
	case CLS_TEST | CLS_RESPONSE:
	    CLSCepTestRsp(theCep, (TestRsp_t*) theMsg);
	    break;

	case CLS_PONG | CLS_REQUEST:
	    CLSCepPongReq(theCep, (PongReq_t*) theMsg);
	    break;
	    
	case CLS_MODIFY_STN | CLS_REQUEST:
	    CLSCepModifyStnReq(theCep, (ModifyStnReq_t*) theMsg);
	    break;

	default:
	    if (cls_debug_err)
		buginf("\nCLSDluSendToCep: unknown primitive type 0x%x", primitive);
	    CLSASSERT(FALSE);
	    break;				/* illegal primitive */
	}
    } else {
	/*
	 * If we can't find the CEP, and this is a closestn.req, then
	 * we are probably handling a stateless incoming connection.
	 * Go ahead and say that all is well.
	 */
	if ((CLSIMsgGetPrimitiveCode(theMsg) == (CLS_CLOSE_STN|CLS_REQUEST)) &&
	    (theMsg->fIdentifierType == CLS_P_CEP_ID) &&
	    (theMsg->fIdentifier == CLS_NO_CEP)) {
 	    CLSDluSendError(this, theMsg, CLS_OK, CLS_U_CEP_ID, 
                                                       theMsg->fIdentifier);
	} else {
	    if (cls_debug_err)
		buginf("\nCLSDluSendToCep: unknown cep fIdentifierType 0x%x fIdentifier 0x%x", 
		       theMsg->fIdentifierType, theMsg->fIdentifier);
 	    CLSDluSendError(this, theMsg, CLS_UNKNOWN_CEP,
                                           CLS_U_CEP_ID, theMsg->fIdentifier);
	}
    }
}

void CLSDluSendToSap (TCLSDlu* this, TCLSIMsg* theMsg)
{
    TCLSSap* theSap;

    CLSDebugMessage("DLU", "SAP", CLSDluGetDLUName(this), theMsg);
    theSap = CLSDluFindSap(this, theMsg->fIdentifier);
    if (theSap == NULL)
    {
	if (cls_debug_err)
	    buginf("\nCLSDluSendToSap: unknown sap fIdentifier 0x%x",
		   theMsg->fIdentifier);
	if (CLSIMsgGetPrimitiveCode(theMsg) == (CLS_REQ_OPNSTN|CLS_REQUEST)) {
	    CLSIMsgSetDestination(theMsg, CLS_U_CEP_ID, ((ReqOpnStnReq_t*)theMsg)->fCLSIHdr.fUCepId);
	}
 	CLSDluSendError(this, theMsg, CLS_UNKNOWN_SAP,
                          CLS_U_SAP_ID, theMsg->fIdentifier); /* No such Sap */
    }
    else
    {
	CLSSapHandleMsg(theSap, theMsg);
    }
}

TCLSSap* CLSDluFindSap (TCLSDlu* this, word theIdentifier)
{
    /* For starters, lets use a simple linked list. */
    TCLSSap* aSap;
    
     if (!CLSSapCheck((TCLSSap*)theIdentifier)) {
	 if (cls_debug_err)
	     buginf("\nCLSDluFindSap: CLSSapCheck failed identifier 0x%x", theIdentifier);
	 return(NULL);
     }

    aSap = (TCLSSap*) this->fSapList.qhead;
    while (aSap != NULL)
    {
	if (aSap->fPSapId == theIdentifier)
	    break;
	else
	    aSap = aSap->fNext;
    }
    return (aSap);
}

TCLSCep* CLSDluFindCepByUCepId (TCLSDlu* this, word theIdentifier)
{
    TCLSCep* aCep;
    
    aCep = (TCLSCep*) this->fCepList.qhead;
    while (aCep != NULL)
    {
	if (aCep->fUCepId == theIdentifier)
	    break;
	else
	    aCep = aCep->fNext;
    }
    return (aCep);
}

void CLSDluSendError (TCLSDlu* this, TCLSIMsg* theMsg, hword errorCode,
                                        CLSIdentifierType_e idType, word id)
{
    if (CLSIMsgIsRequest(theMsg) != TRUE) {
	if (cls_debug_err) {
	    char *str;
	    
	    str = string_getnext();
	    CLSIMsgSprintfShort(theMsg, str);
	    CLSASSERT(CLSIMsgIsRequest(theMsg) == TRUE);
	    buginf("\nCLSDluSendError: message is not a Request %s",
		   str);
	}
	CLSIMsgDestroy(theMsg);
    }
    else {
        CLSASSERT_FATAL(CLSDluCheck(this));

        CLSIMsgSetDestination(theMsg, idType, id);
        CLSIMsgSetPrimitiveType(theMsg, CLS_CONFIRM);
        CLSIMsgSetReturnCode(theMsg, errorCode);
        CLSDluAddPendingMsg(this, theMsg);
    }
}

const char* CLSDluGetDLUName (const TCLSDlu* this)
{
    TCLSIQInit* theQInit;

    theQInit = CLSIQueueGetQInit(this->fReadQ);
    return CLSIQInitGetModuleName(theQInit);
}

void CLSDluAddPendingMsg (TCLSDlu* this, TCLSIMsg* theMsg)
{
    CLSASSERT_FATAL(CLSDluCheck(this));

    if (cls_debug_err) {
	if ((theMsg->fIdentifierType == CLS_P_CEP_ID) ||
	    (theMsg->fIdentifierType == CLS_P_SAP_ID)) {
	    buginf("\nCLSDluAddPendingMsg: bad fIdentifierType 0x%x, identifier 0x%x", 
		   theMsg->fIdentifierType, theMsg->fIdentifier);
	    buginf("\nCLSDluAddPendingMsg: primitive 0x%x retcode 0x%x",
		   theMsg->fPrimitiveCode, CLSIMsgGetReturnCode(theMsg));
	}
    }
/* 
 * Process messages at interrupt level
 *
 * Try this:
 *
 * It is holding fine for DLSw. 
 * Let see how it holds for other DLUs.
 * In case your DLU can not handle it, uncomment the previous
 * line, comment the following 2 lines, and let me know -RN-.
 *
 * But doesn't work well for APPN at the moment
 * CSCdi37988
 */

#ifdef INT_LEVEL
    CLSIQueuePut(this->fReadQ, theMsg);
#else
    p_enqueue(&this->fPendingMsg, theMsg);
    process_set_boolean(gCLSProcess, TRUE); /*Awake process*/
#endif
}
