/* $Id: cls_private.h,v 3.5.10.2 1996/05/17 10:41:59 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/cls/cls_private.h,v $
 *------------------------------------------------------------------
 * Header file included by all CLS implementation files
 *
 * March 1994, Steve Berl
 *
 * Copyright (c) 1993-1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Contains declarations for all the internal class of the CLS
 * subsystem. This includes typedefs, constants, data structures and
 * function prototypes.
 *
 *------------------------------------------------------------------
 * $Log: cls_private.h,v $
 * Revision 3.5.10.2  1996/05/17  10:41:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.5  1996/04/26  07:33:04  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.2.4  1996/04/26  01:52:47  doner
 * CSCdi55831:  IBU Modularity - CLSI and VDLC converted to new IOS
 * scheduler
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.3  1996/04/11  14:35:47  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.2  1996/04/05  06:05:15  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.5.2.1  1996/04/03  13:54:18  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.10.1  1996/03/18  19:10:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.3  1996/03/16  06:32:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.22.2  1996/03/07  08:41:28  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.22.1  1996/02/20  13:41:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/26  06:09:08  dmccowan
 * CSCdi49560:  APPN Connection Network over FDDI fails to activate
 * Fill in EnableTRParms_t structure for FDDI.
 * Remove Obsolete EnableFDDIParms_t structure.
 *
 * Revision 3.4  1995/11/23  00:52:50  sberl
 * CSCdi44391:  CLS assertion failure cepstate != pongPending
 * Give pong flows their own field in the cep control block. This resolves
 * conflicts caused by other events changing the state while there is an
 * outstanding pong request.
 *
 * Revision 3.3  1995/11/21  22:56:49  kpatel
 * CSCdi42053:  DLSWC-3-BADCLSIIDTYPE dlsw_is_bad_clsi
 *
 * Revision 3.2  1995/11/17  08:55:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:52:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/21  17:07:59  sberl
 * CSCdi37437:  Add new ModifyStn primitive to CLSI
 * Just set the right bits in fModifyFlags and put the new values in the
 * parameter structure.
 *
 * Revision 2.1  1995/07/02  05:15:18  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.3  1995/06/30  23:18:26  sberl
 * CSCdi33897:  CLSI race between ReqOpnstn.cnf and disconnect.ind
 * If the interface goes down while we are in req_opnstn pending state,
 * send the dlu a req_opnstn.cnf instead of a disconnect.ind. Make sure
 * that the cls cep and the dlc cep get cleaned up.
 *
 * Revision 2.2  1995/06/15  22:19:17  sberl
 * CSCdi35842:  debug cls error needs more!
 * Add debug messages. Fix some formatting.
 *
 * Revision 2.1  1995/06/07  20:19:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




#ifndef __CLS_H__
#define __CLS_H__
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "address.h"
#include "clsi_msg.h"
#include "clsi_queue.h"
#include "../if/tring.h"
#include "../cls/dlc_registry.h"
#include "dlc_public.h"
#include "dlc.h"
#include "../ui/debug.h"
#include "cls_debug.h"
#include "cls_assert.h"

STRUCTURE_REF(TCLSDlu);
STRUCTURE_REF(TCLSSap);
STRUCTURE_REF(TCLSCep);
STRUCTURE_REF(TCLSEntityManager);
STRUCTURE_REF(TCLSPort);

/*
 * Events which awaken the CLS Process
 */

#define CLSPROCESS "CLS Background Process flag"
#define CEP_COUNT_LOW "CEP count low"

/* 
 *  minor codes for CLS events 
 */
typedef enum cls_events_ {
    CLSPROCESS_EVENT,
    CEP_COUNT_LOW_EVENT
} cls_events;  

/*
 * Watched items
 */
watched_boolean *cep_count_low; 
extern watched_boolean *gCLSProcess; 

/*
 * External symbols
 */

extern TCLSCep* gCLSFreeCepList;
extern int gCLSFreeCepCount;

#define CLS_FREE_CEP_MIN 5
#define CLS_FREE_CEP_MAX 20

void CLSCepListManager(void);
int CLSInit(void);
void CLSShutdown(void);
void cls_parser_init(void);
void CLSDebugMessage(const char* src, const char* dest, const char* dluName, 
		     const TCLSIMsg* theMsg);


struct TCLSDlu_
{
    TCLSDlu* fNext;
    byte fClassId[4];
    TCLSIQueue*	fWriteQ;
    TCLSIQueue*	fReadQ;
    queuetype fSapList;
    queuetype fCepList;
    queuetype fPendingMsg;
    boolean fWorkToDo;
};

TCLSDlu* CLSDluCreate(void);
boolean CLSDluCheck(TCLSDlu* this);
void CLSDluDestroy(TCLSDlu* this);
const char* CLSDluGetDLUName(const TCLSDlu* this);
void CLSDluSendError(TCLSDlu* this, TCLSIMsg* theMsg, hword errorCode,
                                      CLSIdentifierType_e idType, word id);
void CLSDluAddPendingMsg(TCLSDlu* this, TCLSIMsg* theMsg);

static inline void CLSDluSendToDLU (TCLSDlu* this, TCLSIMsg* theMsg)
{
    CLSASSERT_FATAL(CLSDluCheck(this));

    CLSDebugMessage("CLS", "DLU", CLSDluGetDLUName(this), theMsg);
    
    CLSIQueuePut(this->fReadQ, theMsg);
}


/* Methods for the scheduler to call at background process level */

void CLSDluDoWork(TCLSDlu* this);
static inline void CLSDluSetWorkToDo(TCLSDlu* this)
{
    this->fWorkToDo = TRUE;
    process_set_boolean(gCLSProcess, TRUE); /*Awaken process*/
}

/* CEP list management methods */

void CLSDluAddCep(TCLSDlu* this, TCLSCep* aCep);
TCLSCep*  CLSDluRemoveCep(TCLSDlu* this, TCLSCep* aCep);
void CLSDluSendToCep(TCLSDlu* this, TCLSSap *theSap, TCLSIMsg* theMsg);
TCLSCep* CLSDluFindCepByUCepId(TCLSDlu* this, word theIdentifier);

/* SAP list management methods */

void CLSDluAddSap(TCLSDlu* this, TCLSSap* aSap);
TCLSSap*  CLSDluRemoveSap(TCLSDlu* this, TCLSSap* aSap);
TCLSSap* CLSDluFindSap(TCLSDlu* this, word theIdentifier);
void CLSDluSendToSap(TCLSDlu* this, TCLSIMsg* theMsg);


struct TCLSEntityManager_
{
    TCLSEntityManager* fNext;
    byte fClassId[4];
    queuetype fPortList;
};

TCLSEntityManager* CLSEntityManagerCreate(void);
void CLSEntityManagerDestroy(TCLSEntityManager* this);
boolean CLSEntityManagerCheck(TCLSEntityManager* this);

void CLSEntityManagerHandleMsg(TCLSEntityManager* this,
			       TCLSIMsg* theMsg, TCLSDlu* dlu);
void CLSEntityManagerEnable(TCLSEntityManager* this,
			    EnableReq_t* theMsg, TCLSDlu* dlu);
void CLSEntityManagerDisable(TCLSEntityManager* this,
			    DisableReq_t* theMsg, TCLSDlu* dlu);
void CLSEntityManagerActivateSap(TCLSEntityManager* this,
				 ActSapReq_t* theMsg, TCLSDlu* dlu);
void CLSEntityManagerHandleDLCMsg(TCLSEntityManager* this,
				  ClsOpcodeT opCode, ClsDlcParamBlockT* clsDlcPB);
void CLSEntityManagerEnableCnf(TCLSEntityManager* this,
			       ClsDlcParamBlockT* pb);
void CLSEntityManagerDisableCnf(TCLSEntityManager* this,
			       ClsDlcParamBlockT* pb);
void CLSEntityManagerActSapCnf(TCLSEntityManager* this,
			       ClsDlcParamBlockT* pb);
void CLSEntityManagerIdInd(TCLSEntityManager* this,
			   ClsDlcParamBlockT* pb);

TCLSPort* CLSEntityManagerFindPort(TCLSEntityManager* this,
				   CLSDLCType_e portType, CLSPortID portID);
TCLSPort* CLSEntityManagerFindPortByPPortId(TCLSEntityManager* this,
                                   CLSDLCType_e portType, DlcPPortIdT pPortId);
void CLSEntityManagerAddPort(TCLSEntityManager* this, TCLSPort* thePort);
void CLSEntityManagerDeactSapCnf(TCLSEntityManager* this,
				 ClsDlcParamBlockT* pb);

void CLSEntityManagerActRngCnf(TCLSEntityManager* this,ClsDlcParamBlockT* pb);
                               
void CLSEntityManagerDeactRngCnf(TCLSEntityManager* this,ClsDlcParamBlockT* pb);

typedef enum
{
    kPortStateInit,
    kPortStateEnablePending,
    kPortStateEnabled,
    kPortStateActSapPending,
    kPortStateDisablePending,
    kPortStateDisabled,
    kPortStateDeactSapPending
} PortState_e;

typedef union
{
    EnableTRParms_t fTRParms;
    EnableRSRBParms_t fRSRBParms;
    EnableTBRIDGEParms_t fTBRIDGEParms;
    EnableSDLCParms_t fSDLCParms;
    EnableVDLCParms_t fVDLCParms;
    EnableNCIAParms_t fNCIAParms;
} EnableParms_t;


struct TCLSPort_
{
    TCLSPort* fNext;
    byte fClassId[4];
    CLSPortID fPortId;
    CLSDLCType_e fDLCType;
    DlcPPortIdT fDLCPortId;
    PortState_e fPortState;
    TCLSIMsg* fRequest;			/* enable req waiting for response */
    TCLSDlu* fDluPend;			/* dlu that is waiting for it */
    hword fPortNumber;
    hword fPad;
    EnableParms_t fParms;
};

TCLSPort* CLSPortCreate(void);
void CLSPortDestroy(TCLSPort* this);
boolean CLSPortCheck(TCLSPort* this);

void CLSPortEnable(TCLSPort* this, EnableReq_t* theMsg, TCLSDlu* theDlu);
void CLSPortDisable(TCLSPort* this, DisableReq_t* theMsg, TCLSDlu* theDlu);

void CLSPortActSap(TCLSPort* thePort, ActSapReq_t* theMsg, TCLSDlu* dlu);

void CLSPortEnableCnf(TCLSPort* this, ClsDlcParamBlockT* clsDlcPB);
void CLSPortDisableCnf(TCLSPort* this, ClsDlcParamBlockT* clsDlcPB);
void CLSPortActSapCnf(TCLSPort* this, ClsDlcParamBlockT* pb);
void CLSPortDeactSapCnf(TCLSPort* this, ClsDlcParamBlockT* pb);
void CLSPortActRngCnf(TCLSPort* this, ClsDlcParamBlockT* pb);
void CLSPortDeactRngCnf(TCLSPort* this, ClsDlcParamBlockT* pb);

hword CLSPortAddSap(TCLSPort* this, TCLSSap* theSap);
void CLSPortRemoveSap(TCLSPort* this, TCLSSap* theSap);
CLSDLCType_e CLSPortGetType(TCLSPort* this);
boolean CLSPortIsTokenRing(TCLSPort* this);
boolean CLSPortIsEthernet(TCLSPort* this);
boolean CLSPortIsSmds(TCLSPort* this);
boolean CLSPortIsFddi(TCLSPort* this);
boolean CLSPortIsLLC(TCLSPort* this);

typedef enum
{
    kSapStateInit,
    kSapStateDeactPending
} SapState_e;

typedef union
{
    ActSapLLCParms_t fLLCParms;
    ActSapSDLCParms_t fSDLCParms;
    ActSapQLLCParms_t fQLLCParms;
    ActSapVDLCParms_t fVDLCParms;
    ActSapNCIAParms_t fNCIAParms;
} ActSapParms_t;

struct TCLSSap_
{
    TCLSSap* fNext;
    byte fClassId[4];
    TCLSPort* fPortPtr;
    TCLSDlu* fDluPtr;
    word fUSapId;
    word fPSapId;
    DlcPSapIdT fDlcPSapId;
    byte fServiceType;
    TCLSIMsg* fRequest;
    SapState_e fSapState;
    ActSapParms_t fParms;
};


TCLSSap* CLSSapCreate(TCLSPort* thePort, TCLSDlu* theDlu);
void CLSSapDestroy(TCLSSap* this);
boolean CLSSapCheck(TCLSSap* this);
void CLSSapCleanup(TCLSSap* this);

CLSDLCType_e CLSSapGetType(TCLSSap* this);
byte CLSSapGetSapValue(TCLSSap* this);

/* Handle messages from DLU */

void CLSSapHandleMsg(TCLSSap* this, TCLSIMsg* theMsg);
void CLSSapActSap(TCLSSap* this, ActSapReq_t* theMsg);
void CLSSapDeactSapReq(TCLSSap* this, DeactSapReq_t* theMsg);
void CLSSapReqOpnStnReq(TCLSSap* this, ReqOpnStnReq_t* theMsg);
void CLSSapActRng(TCLSSap* this, ActRngReq_t* theMsg);
void CLSSapDeactRngReq(TCLSSap* this, DeActRngReq_t* theMsg);


/* Handle calls from DLC */

void CLSSapHandleDLC(TCLSSap* this);
void CLSSapIdStnInd(TCLSSap* this, ClsDlcParamBlockT* pb);
void CLSSapDeactSapCnf(TCLSSap* this, ClsDlcParamBlockT* pb);
void CLSSapDeactRngCnf(TCLSSap* this, ClsDlcParamBlockT* pb);

typedef enum
{
    kCepStateInit,
    kCepStateReqOpnPending,
    kCepStateOpen,
    kCepStateIdReqPending,
    kCepStateTestReqPending,
    kCepStateConnectReqPending,
    kCepStateDisconnectReqPending,
    kCepStateCloseStnPending,
    kCepStatePongPending,
    kCepStateModifyStnReqPending
} CepState_e;

extern const char* CLSStateToString(CepState_e fCepState); 

typedef union
{
    ReqOpnStnLLCParms_t fLLCParms;
    ReqOpnStnSDLCParms_t fSDLCParms;
    ReqOpnStnQLLCParms_t fQLLCParms;
    ReqOpnStnVDLCParms_t fVDLCParms;
    ReqOpnStnNCIAParms_t fNCIAParms;
} ReqOpnStnParms_t;

struct TCLSCep_
{
    TCLSCep* fNext;
    byte fClassId[4];
    TCLSDlu* fDlu;			/* pointer to DLU that owns */
					/* this Cep */
    TCLSSap* fSap;			/* pointer to Sap that owns */
					/* this Cep */
    word fPCepId;
    word fUCepId;
    word fDataUCepId;
    TCLSIMsg* fRequest;
    TCLSIMsg* fPongReq;
    word fPendingCorrelator;
    CepState_e fCepState;
    boolean fNeedsDLCClose;
    DlcPCepIdT fDLCPCepId;
    ReqOpnStnParms_t fParms;
    boolean fDataIndBlocked;
    queuetype fDataIndQueue;
    queuetype fDataReqQueue;
    boolean fNeedsFlowInd;
    ulong fFlowLevel;
    boolean fNeedsDisconnectInd;
    word fDisconnectReason;
};

TCLSCep* CLSCepCreate(TCLSSap* theSap, TCLSDlu* theDlu);
void CLSCepDestroy(TCLSCep* this);
boolean CLSCepCheck(TCLSCep* this);

/* Accessor methods */

/* Handle messages from DLU */


void CLSCepReqOpnStnReq(TCLSCep* this, TCLSSap* theSap, ReqOpnStnReq_t* theMsg);
void CLSCepIdReq(TCLSCep* this, IdReq_t* theMsg);
void CLSCepIdRsp(TCLSCep* this, IdRsp_t* theMsg);
void CLSCepConnectReq(TCLSCep* this, ConnectReq_t* theMsg);
void CLSCepConnectRsp(TCLSCep* this, ConnectRsp_t* theMsg);
void CLSCepSignalStnReq(TCLSCep* this, SignalStnReq_t* theMsg);
void CLSCepDataReq(TCLSCep* this, DataReq_t* theMsg);
void CLSCepFlowReq(TCLSCep* this, FlowReq_t* theMsg);
void CLSCepDisconnectReq(TCLSCep* this, DisconnectReq_t* theMsg);
void CLSCepDisconnectRsp(TCLSCep* this, DisconnectRsp_t* theMsg);
void CLSCepCloseStnReq(TCLSCep* this, CloseStnReq_t* theMsg);
void CLSCepOpenStnReq(TCLSCep* this, OpenStnReq_t* theMsg);
void CLSCepPongReq(TCLSCep* this, PongReq_t* theMsg);
void CLSCepModifyStnReq(TCLSCep* this, ModifyStnReq_t* theMsg);

void ClsCepReqOpnStnCnf(TCLSCep* this, ClsDlcParamBlockT* theMsg);
void CLSCepIdInd(TCLSCep* this, ClsDlcParamBlockT* pb);
void CLSCepConnectInd(TCLSCep* this, ClsDlcParamBlockT* pb);
void CLSCepConnectCnf(TCLSCep* this, ClsDlcParamBlockT* pb);
void CLSCepConnectedInd(TCLSCep* this, ClsDlcParamBlockT* pb);
void CLSCepDisconnectInd(TCLSCep* this, ClsDlcParamBlockT* pb);
void CLSCepDisconnectCnf(TCLSCep* this, ClsDlcParamBlockT* pb);
void CLSCepDataInd(TCLSCep* this, ClsDlcParamBlockT* pb);
void CLSCepDataCnf(TCLSCep* this, ClsDlcParamBlockT* pb);
void CLSCepFlowInd(TCLSCep* this, ClsDlcParamBlockT* pb);
void CLSCepCloseStnCnf(TCLSCep* this, ClsDlcParamBlockT* pb);
void CLSCepPongCnf(TCLSCep* this, ClsDlcParamBlockT* pb);
void CLSCepModifyStnCnf(TCLSCep* this, ClsDlcParamBlockT* pb);

void CLSCepSendFlowInd(TCLSCep* this);
void CLSCepSendDisconnectInd(TCLSCep* this);

/*
 * Prototypes for DLSw Extensions. 
 */
void CLSEntityManagerActivateRing (TCLSEntityManager* this, ActRngReq_t* theMsg, TCLSDlu* dlu);
void CLSEntityManagerTestStnReq (TCLSEntityManager* this, TestStnReq_t* theMsg, TCLSDlu* dlu);
void CLSEntityManagerTestStnRsp (TCLSEntityManager* this, TestStnRsp_t* theMsg, TCLSDlu* dlu);
void CLSEntityManagerIdStnReq (TCLSEntityManager* this, IdStnReq_t* theMsg, TCLSDlu* dlu);
void CLSEntityManagerIdStnRsp (TCLSEntityManager* this, IdStnRsp_t* theMsg, TCLSDlu* dlu);
void CLSEntityManagerUDataStnReq (TCLSEntityManager* this, UDataStnReq_t* theMsg, TCLSDlu* dlu);

void CLSEntityManagerTestInd (TCLSEntityManager* this, ClsDlcParamBlockT* pb);
void CLSEntityManagerConnectInd (TCLSEntityManager* this, ClsDlcParamBlockT* pb);
void CLSEntityManagerIdInd (TCLSEntityManager* this, ClsDlcParamBlockT* pb);

void CLSPortActRing (TCLSPort* this, ActRngReq_t* theMsg, TCLSDlu* dlu);
void CLSPortDeactRing (TCLSPort* this, DeActRngReq_t* theMsg, TCLSDlu* dlu);
void CLSSapConnectStnInd (TCLSSap* this, ClsDlcParamBlockT* pb);
void CLSCepUDataReq(TCLSCep* this, UDataReq_t* theMsg);
void CLSCepUDataInd (TCLSCep* this, ClsDlcParamBlockT* pb);
void CLSCepTestInd(TCLSCep* this, ClsDlcParamBlockT* pb);

ActRngCnf_t* BuildActRngCnf(ActRngReq_t* theReq, hword retCode);
void CLSCepTestReq (TCLSCep* this, TestReq_t* theMsg);
void CLSCepTestRsp (TCLSCep* this, TestRsp_t* theMsg);
void CLSSapTestStnReq (TCLSSap* this, TestStnReq_t* theMsg);
void CLSSapTestStnRsp (TCLSSap* this, TestStnRsp_t* theMsg);
void CLSSapTestStnInd (TCLSSap* this, ClsDlcParamBlockT* pb);
void CLSSapIdStnReq (TCLSSap* this, IdStnReq_t* theMsg);
void CLSSapIdStnRsp (TCLSSap* this, IdStnRsp_t* theMsg);
void CLSSapUDataStnReq(TCLSSap* this, UDataStnReq_t* theMsg);
void CLSSapUDataStnInd (TCLSSap* this, ClsDlcParamBlockT* pb);
void CLSEntityManagerUDataInd (TCLSEntityManager* this, ClsDlcParamBlockT* pb);

#endif

