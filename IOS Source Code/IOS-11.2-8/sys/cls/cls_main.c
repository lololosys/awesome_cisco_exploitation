/* $Id: cls_main.c,v 3.6.20.1 1996/05/17 10:41:56 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/cls/cls_main.c,v $
 *------------------------------------------------------------------
 * Top level entrypoints for Cisco Link Services Subsystem
 *
 * March 1994, Steve Berl
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Implement the highest layer of interface between CLS and DLU.
 * This file contains code to implement the highest layers of CLS. The 
 * first group of routines is used by the client DLU to establish and 
 * destroy its relationship with CLS. This relationship is 
 * represented by a TClsDlu object, which is created by the call to 
 * CLSOpen(). The TClsDlu object is destroyed by the DLU call to 
 * CLSClose(). The TClsDlu object is hidden from the DLU, but it is 
 * used to tie together all of the resources associated with the DLU. 
 * These resources are the read and write queues used to exchange 
 * messages between the DLU and CLS, the CEP list and the SAP list. 
 *------------------------------------------------------------------
 * $Log: cls_main.c,v $
 * Revision 3.6.20.1  1996/05/17  10:41:56  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.6.6  1996/05/03  22:06:01  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.6.5  1996/04/26  01:52:42  doner
 * CSCdi55831:  IBU Modularity - CLSI and VDLC converted to new IOS
 * scheduler
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.6.4  1996/04/24  06:28:07  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.6.6.3  1996/04/11  14:35:42  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.6.2  1996/04/05  06:05:13  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.6.6.1  1996/03/28  08:19:18  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.6  1996/02/02  22:01:55  pmorton
 * CSCdi47867:  DLSw : Over QLLC broken
 * Back out change for CSCdi44928 that was overwriting QLLC Sap structure.
 *
 * Revision 3.5  1996/01/18  20:37:31  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/15  06:07:45  richl
 * CSCdi46890:  show cls is too verbose, add brief option
 *
 * Revision 3.3.2.1  1996/01/16  00:22:09  ioakley
 * Branch: Cal_Ibm_branch
 * Correct "show cls" command "type" and "user class" output errors.
 *
 * Revision 3.3  1995/11/21  22:56:47  kpatel
 * CSCdi42053:  DLSWC-3-BADCLSIIDTYPE dlsw_is_bad_clsi
 *
 * Revision 3.2  1995/11/17  08:55:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:10:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  20:52:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/09/13  20:37:12  sberl
 * CSCdi40288:  CLS debug doesnt decode all error codes
 * Add new error codes to debug xlat table.
 *
 * Revision 2.8  1995/07/25  01:34:57  vrawat
 * CSCdi37602:  fix prioritization using new cls primitive
 *
 * Revision 2.7  1995/07/21  17:07:58  sberl
 * CSCdi37437:  Add new ModifyStn primitive to CLSI
 * Just set the right bits in fModifyFlags and put the new values in the
 * parameter structure.
 *
 * Revision 2.6  1995/07/19  20:58:16  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add support for Activate/Deactivate Ring
 *
 * Revision 2.5  1995/07/02  05:15:16  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.4  1995/06/15  22:59:40  ppearce
 * CSCdi35921:  CLS/RSRB virtual ring not removed after "appn stop"
 *
 * Revision 2.3  1995/06/15  22:19:29  sberl
 * CSCdi35842:  debug cls error needs more!
 * Add debug messages. Fix some formatting.
 *
 * Revision 2.2  1995/06/09  12:59:25  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:19:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




#include "cls_private.h"
#include "../llc2/llc2.h"
#include "config.h"
#include "parser.h"

static const int CLS_PROC_STACK_SIZE = 0; /* use default */

static TCLSIQInit* gCLSQInit;
static TCLSDlu* gCLSDluListHead;
static pid_t gCLSProcPid;

static TCLSEntityManager* gCLSEntityManager;
TCLSCep* gCLSFreeCepList;
int gCLSFreeCepCount;
watched_boolean *gCLSProcess;  /*This flag is used to awaken the process
				 whenever an event has occurred which
				 warrants this.  Currently 4/17/96 this
				 is when 1. something is queued onto an
				 fPendingMsg queue for a cep 2. when
				 a cep has work to do
				*/

/* Forward declaration */
static int CLSWritePut(TCLSIQueue* theQueue, TCLSIMsg* theMsg);
static process CLSProcess(void);
boolean CLSBlock(void);

/* Initialize the CLS subsystem */
/* If we had constant objects and static constructors this wouldn't be needed */
int CLSInit (void)
{
    cls_debug_init();
    
    gCLSFreeCepList = NULL;
    gCLSFreeCepCount = 0;

    cep_count_low = create_watched_boolean(CEP_COUNT_LOW,
					   CEP_COUNT_LOW_EVENT);
    process_set_boolean(cep_count_low,TRUE); /*Below minimum*/

    gCLSProcess = create_watched_boolean(CLSPROCESS,
					 CLSPROCESS_EVENT);

    gCLSQInit = CLSIQInitCreate("CLS", CLSWritePut);
    gCLSEntityManager = CLSEntityManagerCreate();
    gCLSProcPid = 0;
    
    /* Check that everything went well and clean up if it didn't */
    if ((gCLSQInit != NULL) &&
	(gCLSEntityManager != NULL)) {
	return (0);
    } else {
	CLSShutdown();
	return (-1);
    }
}

/*
 * This is the CLS Background process
 */
static process CLSProcess (void)
{
    TCLSDlu* dluPtr;
    sys_timestamp starttime;
    
    process_wait_on_system_init(); /*Wait for system to be up*/

    process_watch_boolean(gCLSProcess, ENABLE, RECURRING);
    process_watch_boolean(cep_count_low, ENABLE, RECURRING);

    for (;;)
    {
	process_wait_for_event();                        /* Any event */
	/* Reset event flags */
	process_set_boolean(gCLSProcess, FALSE);         

	/*
	 * Some event has occurred, run through our process
	 */  
	
	GET_TIMESTAMP(starttime);
		
	CLSCepListManager();		/* refill free cep list */

	/*CLSASSERT(ELAPSED_TIME(starttime) < ONESEC);*/
	for (dluPtr = gCLSDluListHead; dluPtr != NULL; dluPtr = dluPtr->fNext)
	{
	    CLSDluDoWork(dluPtr);
	}
	/*CLSASSERT(ELAPSED_TIME(starttime) < ONESEC);*/
    }	
}


void CLSShutdown (void)
{
    if (gCLSProcPid != 0)
    {
	process_kill(gCLSProcPid);	/* kill off the background */
	gCLSProcPid = 0;		/* process */
    }
    if (gCLSQInit != NULL)
    {
	CLSIQInitDestroy(gCLSQInit);
	gCLSQInit = NULL;
    }
    if (gCLSEntityManager != NULL)
    {
	CLSEntityManagerDestroy(gCLSEntityManager);
	gCLSEntityManager = NULL;
    }
}

/* Calls to exchange queue pointers with CLS. Called by DLU to establish 
   connection */

TCLSIQueue* CLSOpen (TCLSIQueue* dluReadQPtr)
{
    TCLSDlu* dlu;
    TCLSIQueue* clsWriteQPtr = NULL;
    
    /*
     * If the CLS background process is not running, start it up. If
     * it fails to come up, we need to bail.
     */
    if (gCLSProcPid == 0) {
	gCLSProcPid = process_create(CLSProcess,"CLS Background",
		       NORMAL_STACK,PRIO_NORMAL);
	if (gCLSProcPid == 0) {
	    return(NULL);
	}
    }

    dlu = CLSDluCreate();		/* create dlu control block */
    if (dlu != NULL)
    {
	clsWriteQPtr = CLSIQueueCreate(gCLSQInit, dlu); /* create cls write 
							   queue */
	if (clsWriteQPtr != NULL)
	{
	    dlu->fWriteQ = clsWriteQPtr; /* hook everything together */
	    dlu->fReadQ = dluReadQPtr;
	    
	    /* Other initialization of dlu? */

	    dlu->fNext = gCLSDluListHead;
	    gCLSDluListHead = dlu;
	}
	else {
	    CLSDluDestroy(dlu);
	}
    }
    return (clsWriteQPtr);
}

void CLSClose (TCLSIQueue* clsWriteQueuePtr)
{
    TCLSDlu* dlu;
    TCLSDlu* aDluPtr;
    
    dlu = (TCLSDlu*) CLSIQueueGetRefCon(clsWriteQueuePtr);
    
    /* scan the dlu list to see if it's there */
    if (dlu == gCLSDluListHead)
    {
	gCLSDluListHead = dlu->fNext;
	dlu->fNext = NULL;
    }
    else
    {
	aDluPtr = gCLSDluListHead;	
	while (aDluPtr != NULL)
	{
	    if (aDluPtr->fNext == dlu)
	    {
		aDluPtr->fNext = dlu->fNext; /* Remove it from the list */
		dlu->fNext = NULL;
		break;
	    }
	    aDluPtr = aDluPtr->fNext;
	}
    }

    if (dlu != NULL)
	CLSDluDestroy(dlu);	/* Do whatever cleanup we do to the dlu */

    CLSIQueueDestroy(clsWriteQueuePtr);

    /*
     * If there are no more DLUs, then we should kill of the CLS
     * background processes.
     */
    if (gCLSDluListHead == NULL) {
	if (gCLSProcPid != 0) {
	    process_kill(gCLSProcPid);
	    gCLSProcPid = 0;
	}
    }
    return;
}

static int CLSWritePut (TCLSIQueue* theQueue, TCLSIMsg* theMsg)
{
    TCLSDlu* dlu;
    
    /* First find the dlu object */
    dlu = (TCLSDlu*) CLSIQueueGetRefCon(theQueue);
    if ((!CLSDluCheck(dlu)) || (theQueue != dlu->fWriteQ))
    {
      return(CLS_INVALID_Q_PTR);
    }

    if (theMsg == NULL) {
	CLSASSERT(theMsg != NULL);
	return(CLS_NULL_MSG);
    }
    /* Route to proper object based on the */
    /* identifier type and identifer */

    switch (theMsg->fIdentifierType)
    {
    case CLS_U_CEP_ID:
    case CLS_P_CEP_ID:			/* Find the CEP */
	CLSDluSendToCep(dlu, (TCLSSap *)(theMsg->fIdentifier), theMsg);
	break;
    case CLS_P_SAP_ID:			/* Find the SAP */
	CLSDluSendToSap(dlu, theMsg);
	break;
    case CLS_ENTITY_MGR:		/* Must be enable or activate_sap, find port */
	CLSEntityManagerHandleMsg(gCLSEntityManager, theMsg, dlu);
	break;
    default:				/* Unknown destination */
	if (cls_debug_err)
	    buginf("\nCLSWritePut: unknown identifier type %d",
		   theMsg->fIdentifierType);

 	CLSDluSendError(dlu, theMsg, CLS_UNKNOWN_IDENTIFIER_TYPE,
                         theMsg->fIdentifierType, theMsg->fIdentifier);
	break;
    }
    return(CLS_OK);
}

void cls_rput (ClsUCepIdT uCepId, ClsOpcodeT opCode, ClsDlcParamBlockT* pb)
{
    if (uCepId == (ClsUCepIdT) CLS_NO_CEP) {
	CLSEntityManagerHandleDLCMsg(gCLSEntityManager, opCode, pb);
    } else {
	if (!CLSCepCheck((TCLSCep *)uCepId)) {
	    /*
	     * Got a message from DLC for a CEP that doesn't exist.
	     * Either it is garbage, or is is a CEP that has since been
	     * destroyed. In either case we want to clean up and discard
	     * any pak contained in the message. Probably want to print
	     * something on the console also.
	     */
	    if (cls_debug_err)
		buginf("\ncls_rput: CLSCepCheck failed uCepId 0x%x",
		       uCepId);

	    if (pb->fPakPtr != NULL) {
		datagram_done(pb->fPakPtr);
		pb->fPakPtr = NULL;
	    }
	    return;
	}

	switch (opCode) {
	case CLS_DATA_IND:
	    CLSCepDataInd((TCLSCep*)uCepId, pb);
	    break;
	case CLS_REQ_OPNSTN_CNF:
	    ClsCepReqOpnStnCnf((TCLSCep*)uCepId, pb);
	    break;
	case CLS_ID_IND:
	    CLSCepIdInd((TCLSCep*)uCepId, pb);
	    break;
	case CLS_CONNECT_IND:
	    CLSCepConnectInd((TCLSCep*)uCepId, pb);
	    break;
	case CLS_CONNECT_CNF:
	    CLSCepConnectCnf((TCLSCep*)uCepId, pb);
	    break;
	case CLS_CONNECTED_IND:
	    CLSCepConnectedInd((TCLSCep*)uCepId, pb);
	    break;
	case CLS_DISC_IND:
	    CLSCepDisconnectInd((TCLSCep*)uCepId, pb);
	    break;
	case CLS_DISC_CNF:
	    CLSCepDisconnectCnf((TCLSCep*)uCepId, pb);
	    break;
	case CLS_DATA_CNF:
	    CLSCepDataCnf((TCLSCep*)uCepId, pb);
	    break;
	case CLS_FLOW_IND:
	    CLSCepFlowInd((TCLSCep*)uCepId, pb);
	    break;
	case  CLS_CLOSE_STN_CNF:
	    CLSCepCloseStnCnf((TCLSCep*)uCepId, pb);
	    break;
	case CLS_UDATA_IND:
	    CLSCepUDataInd((TCLSCep*)uCepId, pb);
	    break;
	case CLS_TEST_IND:
	    CLSCepTestInd((TCLSCep*)uCepId, pb);
	    break;
	case CLS_PONG_CNF:
	    CLSCepPongCnf((TCLSCep*)uCepId, pb);
	    break;
	case CLS_MODIFY_STN_CNF:
	    CLSCepModifyStnCnf((TCLSCep*)uCepId, pb);
	    break;
	default:
	    if (cls_debug_err)
		buginf("\ncls_rput: unknown primitive type 0x%x", opCode);
	    CLSASSERT(FALSE);
	    break;
	}
	if (pb->fPakPtr != NULL) {
	    /* 
	     * If there is a pak, and the CEP didn't eat it, we need
	     * to dispose of it here.
	     */
	    datagram_done(pb->fPakPtr);
	    pb->fPakPtr = NULL;
	}
    }
}

typedef struct
{
    hword fKey;
    const char* fString;
} TCLSNameTbl;
typedef struct
{
    CepState_e fKey;
    const char* fString;
} TCLSStateTbl;

static const char *CLSNameTblScan(const TCLSNameTbl* tbl, hword key)
{
    const TCLSNameTbl* ptr;
    
    for (ptr = tbl; ptr->fKey != 0xFFFF; ptr++)
    {
	if (ptr->fKey == key)
	    return ptr->fString;
    }
    return NULL;
}

static const char *CLSStateTblScan(const TCLSStateTbl* tbl, CepState_e key)
{
    const TCLSStateTbl* ptr;

    for (ptr = tbl; ptr->fKey != 0xFF; ptr++)
    {
        if (ptr->fKey == key)
            return ptr->fString;
    }
    if (cls_debug_err)
	buginf("\nCLSERROR: Unknown state for string %x", key);
    return NULL;
}

static const TCLSStateTbl gStateTypeNameTbl[] =
{
    {kCepStateInit, "Init"},
    {kCepStateReqOpnPending,"ReqOpnPending"},
    {kCepStateOpen,"Open"},
    {kCepStateIdReqPending,"IdReqPending"},
    {kCepStateTestReqPending,"TestReqPending"},
    {kCepStateConnectReqPending,"ConReqPending"},
    {kCepStateDisconnectReqPending,"DiscReqPending"},
    {kCepStateCloseStnPending,"CloseStnPending"},
    {kCepStatePongPending,"PongPending"},
    {kCepStateModifyStnReqPending,"ModifyStnReqPending"},
    {0xFFFF, NULL }
};
static const TCLSNameTbl gPrimitiveTypeNameTbl[] =
{
    {CLS_REQUEST, "Req" },
    {CLS_CONFIRM, "Cfm" },
    {CLS_INDICATION, "Ind" },
    {CLS_RESPONSE, "Rsp" },

    {0xFFFF, NULL }
};
const char* CLSStateToString(CepState_e fCepState) 
{
    CepState_e key = fCepState;

    return CLSStateTblScan(gStateTypeNameTbl, key);
}
    
const char* CLSPrimitiveTypeToString(hword primitiveCode)
{
    hword key = primitiveCode & CLS_TYPE_MASK;

    return CLSNameTblScan(gPrimitiveTypeNameTbl, key);
}

static const TCLSNameTbl gPrimitiveCommandNameTbl[] =
{
    {CLS_ACTIVATE_SAP, "ACTIVATE_SAP" },
    {CLS_CLOSE_STN, "CLOSE_STN" },
    {CLS_CONNECT, "CONNECT" },
    {CLS_CONNECTED, "CONNECTED" },
    {CLS_DATA, "DATA" },
    {CLS_DEACTIVATE_SAP, "DEACTIVATE_SAP" },
    {CLS_DISCONNECT, "DISCONNECT" },
    {CLS_ENABLE, "ENABLE" },
    {CLS_FLOW, "FLOW" },
    {CLS_ID, "ID" },
    {CLS_ID_STN, "ID_STN" },
    {CLS_OPEN_STN, "OPEN_STN" },
    {CLS_REQ_OPNSTN, "REQ_OPNSTN" },
    {CLS_SIGNAL_STN, "SIGNAL_STN" },
    {CLS_DISABLE, "DISABLE" },

    {CLS_UDATA, "UDATA"},
    {CLS_MODIFY_SAP, "MODIFY_SAP"},
    {CLS_MODIFY_STN, "MODIFY_STN"},
    {CLS_TEST, "TEST"},
    {CLS_ACTIVATE_RING, "ACTIVATE_RING"},
    {CLS_DEACTIVATE_RING, "DEACTIVATE_RING"},
    {CLS_TEST_STN, "TEST_STN"},
    {CLS_UDATA_STN, "UDATA_STN"},
    {CLS_CONNECT_STN, "CONECT_STN"},
    {CLS_PONG, "PONG"},

    {0xFFFF, NULL }
};
    
const char* CLSPrimitiveCommandToString(hword primitiveCode)
{
    hword key = primitiveCode & CLS_COMMAND_MASK;

    return CLSNameTblScan(gPrimitiveCommandNameTbl, key);
}

static const TCLSNameTbl gIdentifierTypeNameTbl[] =
{
    {CLS_ENTITY_MGR, "entMgr" },
    {CLS_U_CEP_ID, "uCEP" },
    {CLS_P_CEP_ID, "pCEP" },
    {CLS_U_SAP_ID, "uSAP" },
    {CLS_P_SAP_ID, "pSAP" },

    {0xFFFF, NULL }
};
    
const char* CLSIdentifierTypeToString(CLSIdentifierType_e identifierType)
{
    hword key = (hword) identifierType;

    return CLSNameTblScan(gIdentifierTypeNameTbl, key);
}

static const TCLSNameTbl gSelectorCodeNameTbl[] =
{
    {CLS_LLC_LAYER, "LLC" },

    {0xFFFF, NULL }
};
static const TCLSNameTbl gPortNameTbl[] =
{
    {CLS_SDLC, "sdlc"},
    {CLS_SOCKET, "socket"},
    {CLS_LLC,"llc"},
    {CLS_LOCAL, "local"},
    {CLS_RSRB, "rsrb"},             /* Covers LACK and pass through case */
    {CLS_TBRIDGE, "transparentBridging" },
    {CLS_FRAME_RELAY, "frameRelay"},
    {CLS_ATM_1483, "atm-rfc1483"},
    {CLS_QLLC, "qllc"},
    {CLS_VDLC, "vdlc"},
    {0xFFFF, NULL }
};

    
const char* CLSSelectorCodeToString(CLSSelectorCode_e selectorCode)
{
    return CLSNameTblScan(gSelectorCodeNameTbl, (hword) selectorCode);
}

static const char* CLSPortToString (TCLSPort *this)
{
    idbtype *idb;

    idb = DlcToIdb(&this->fPortId);
    if (idb != NULL) {
	return (idb->namestring);
    }
    return ("(NULL)");
}
static const TCLSNameTbl gReturnCodeNameTbl[] =
{
    {CLS_OK, "CLS_OK" },
    {CLS_OUT_OF_MEMORY, "OUT_OF_MEMORY" },
    {CLS_UNKNOWN_PORT, "UNKNOWN_PORT" },
    {CLS_UNKNOWN_SAP, "UNKNOWN_SAP" },
    {CLS_UNKNOWN_RING, "UNKNOWN_RING" },
    {CLS_UNKNOWN_CEP, "UNKNOWN_CEP" },
    {CLS_UNKNOWN_IDENTIFIER_TYPE, "UNKNOWN_IDENTIFIER_TYPE" },
    {CLS_SAP_BUSY, "SAP_BUSY" },
    {CLS_INVALID_Q_PTR, "INVALID_Q_PTR"},
    {CLS_UNKNOWN_PRIMITIVE, "UNKNOWN_PRIMITIVE" },
    {CLS_UNKNOWN_DLC_TYPE, "UNKNOWN_DLC_TYPE" },
    {CLS_NO_REM_STN, "NO_REMOTE_STN" },
    {CLS_NO_XID_RSP, "NO_XID_RSP" },
    {CLS_DISC_ERROR, "DISCONNECT_ERROR" },
    {CLS_CONFIG_CONFLICT,"CONFIG_CONFLICT"},
    {CLS_TIMEOUT, "TIMEOUT"},
    {CLS_CONNECTION_BUSY, "CONNECTION_BUSY"},
    {CLS_ALREADY_ENABLED, "ALREADY_ENABLED"},
    {CLS_NO_SAP_REGISTRY, "NO_SAP_REGISTRY"},
    {CLS_INVALID_LOCAL_RING, "INVALID_LOCAL_RING"},
    {CLS_INVALID_TARGET_RING, "INVALID_TARGET_RING"},
    {CLS_BAD_ADDRESS, "BAD_ADDRESS"},
    {CLS_INTERFACE_DOWN, "INTERFACE_DOWN"},
    {CLS_VMAC_OVERLAP, "VMAC_OVERLAP"},
    {CLS_LOCAL_ERROR, "LOCAL_ERROR"},
    {CLS_VDLC_STATE_ERROR, "VDLC State Error"},
    {CLS_REQUEST_OVERRIDE, "Request Override"},
    {CLS_INACTIVITY_TIMEOUT, "Inactivity Timeout"},
    {CLS_NULL_MSG, "NULL_MSG"},
    {CLS_INACTIVITY_TIMEOUT, "INACTIVITY_TIMEOUT"},
    {0xFFFF, NULL }
};
    
char* CLSReturnCodeToString(hword retCode)
{
     char *ptr;

    ptr = (char*) CLSNameTblScan(gReturnCodeNameTbl, retCode);
    if (ptr == NULL) {
	ptr = string_getnext();
	sprintf(ptr, "unknown err: 0x%x", retCode);
    }
    return ptr;
}

char* CLSIMsgSprintf(const TCLSIMsg* this, char* buffer)
{
    if (CLSIMsgGetPrimitiveType(this) == CLS_CONFIRM)
    {
	sprintf(buffer, "	%s.%s(%s) to %s: 0x%x sel: %s hlen: %d, dlen: %d",
		CLSPrimitiveCommandToString(this->fPrimitiveCode),
		CLSPrimitiveTypeToString(this->fPrimitiveCode),
		CLSReturnCodeToString(((TCLSICnfMsg*)this)->fStatus.fRetCode),
		CLSIdentifierTypeToString(this->fIdentifierType),
		this->fIdentifier,
		CLSSelectorCodeToString(this->fSelector),
		this->fHeaderLength,
		this->fDataSize);
    }
    else
    {
	sprintf(buffer, "	%s.%s to %s: 0x%x sel: %s hlen: %d, dlen: %d",
		CLSPrimitiveCommandToString(this->fPrimitiveCode),
		CLSPrimitiveTypeToString(this->fPrimitiveCode),
		CLSIdentifierTypeToString(this->fIdentifierType),
		this->fIdentifier,
		CLSSelectorCodeToString(this->fSelector),
		this->fHeaderLength,
		this->fDataSize);
    }

    return buffer;
}
/*
 * Created so we could make use of string_getnext() heap based buffer.
 * Helps the stack starved CLSI when we get rid of print buffers in stack.
 */
char* CLSIMsgSprintfShort(const TCLSIMsg* this, char* buffer)
{
	sprintf(buffer, "CLSI Msg : %s.%s %s dlen: %d",
		CLSPrimitiveCommandToString(this->fPrimitiveCode),
		CLSPrimitiveTypeToString(this->fPrimitiveCode),
		(CLSIMsgGetPrimitiveType(this) == CLS_CONFIRM) ? 
		CLSReturnCodeToString(((TCLSICnfMsg*)this)->fStatus.fRetCode) :
		" ",
		this->fDataSize);
    return buffer;
}

void CLSDebugMessage(const char* src, const char* dest, const char* dluName, 
		     const TCLSIMsg* theMsg)
{
    char printBuffer[200];

    if (cls_debug_message) {
	if (cls_debug_state) {
             if(theMsg->fIdentifierType == CLS_P_CEP_ID)
		buginf("\n(%s:%s-->%s) state: %s ",
		       dluName, src, dest, 
			CLSStateToString(((TCLSCep*)(theMsg->fIdentifier))->fCepState));
	}
	else {	
	    buginf("\n(%s:%s-->%s): ", dluName, src, dest); 
	}

	buginf("\t%s", CLSIMsgSprintf(theMsg, printBuffer));

	}

}

void  show_cls_command (parseinfo *csb)
{
    uchar    dtestring[LLC_MAX_DTE_SIZE];
    TCLSDlu* dluPtr;
    TCLSSap* sapPtr;
    TCLSCep* cepPtr;
    int	     value;
    boolean  bridging;
    char*    serviceType;

    automore_enable(NULL);
    for (dluPtr = gCLSDluListHead; dluPtr != NULL; dluPtr = dluPtr->fNext) {
	printf("\nDLU user: %s", CLSDluGetDLUName(dluPtr));
	for (sapPtr =(TCLSSap*)dluPtr->fSapList.qhead; 
	     sapPtr != NULL; sapPtr=sapPtr->fNext) {
	    value = -1;
	    bridging = FALSE;
	    
	    switch (sapPtr->fPortPtr->fDLCType) {
	    case CLS_NCIA:
		serviceType = "NCIA";
		value = sapPtr->fParms.fNCIAParms.fSapValue;
		if (value == 0) {
		    bridging = TRUE;
		}
		break;
	    case CLS_VDLC:
		serviceType = "VDLC";
		value = sapPtr->fParms.fVDLCParms.fSapValue;
		if (value == 0) {
		    bridging = TRUE;
		}
		break;

	    case CLS_QLLC:
		serviceType = "QLLC";
		bridging = QTestDlsw(&(sapPtr->fParms.fQLLCParms.
				       fQActSap.fQSapFlags));
		break;

	    case CLS_SDLC:
		serviceType = "SDLC";
		value = sapPtr->fParms.fSDLCParms.fSapValue;
		break;

	    default:
		serviceType = "LLC ";
		value = sapPtr->fParms.fLLCParms.fSapValue;
		if(value == 0) {
		    bridging = TRUE;
		}
		break;
	    }
	    if (bridging) {
		printf("\n      Bridging ");
	    } else if (value != -1) {
		printf("\n      SSap:0x%02x", value);
	    } else {
		printf("\n               ");
	    }
	    printf("       %s %s",
		   serviceType,
		   CLSPortToString(sapPtr->fPortPtr));
	}
	for (cepPtr = (TCLSCep*)dluPtr->fCepList.qhead; 
	     cepPtr != NULL; cepPtr=cepPtr->fNext) {
	    llc2_build_dte_string(dtestring, 
				  &(cepPtr->fParms.fLLCParms.fDestAddr),
				  &(cepPtr->fParms.fLLCParms.fSrcAddr),
				  (uchar)(cepPtr->fParms.fLLCParms.fDestSap),
				  (uchar)(cepPtr->fParms.fLLCParms.fSrcSap), 0);
             if (snpa_get_type(&(cepPtr->fParms.fLLCParms.fSrcAddr)) != STATION_SMDS64) {
	 	   printf("\n	DTE:%e %e %02x %02x",
			  &dtestring[0],
			  &dtestring[8],
			  dtestring[RSAP_DTE],
			  dtestring[LSAP_DTE]);
	      } else {
	 	   printf("\n	DTE:%E %E %02x %02x",
			  &dtestring[SMDS_RMAC_DTE],
			  &dtestring[SMDS_LMAC_DTE],
			  dtestring[SMDS_RSAP_DTE],
			  dtestring[SMDS_LSAP_DTE]);
	      }
  	    
	    /*
	     * show cls brief stops here
	     */
   	    if (GETOBJ(int,1)) {
   		continue;
   	    }

	    printf("\n	T1 timer:%6d	T2 timer:%6d	Inact timer:%6d",
		   cepPtr->fParms.fLLCParms.fT1,
		   cepPtr->fParms.fLLCParms.fT2,
		   cepPtr->fParms.fLLCParms.fTi);
	    
	    printf("\n	max out:%6d	max in:%6d	retry count:%6d",
		   cepPtr->fParms.fLLCParms.fMaxOut,
		   cepPtr->fParms.fLLCParms.fMaxIn,
		   cepPtr->fParms.fLLCParms.fMaxRetryCount);
	    printf("\n	XID retry:%5d XID timer:%5d I-Frame:%5d\n",
		   cepPtr->fParms.fLLCParms.fXIDRetries,
		   cepPtr->fParms.fLLCParms.fXIDTimer,
		   cepPtr->fParms.fLLCParms.fMaxIField);
	    
	}
    }
    printf("\n");
    automore_disable();
}

