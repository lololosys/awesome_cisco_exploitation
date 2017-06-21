/* $Id: dlcsw.c,v 3.8.8.9 1996/08/07 08:57:38 snyder Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlcsw.c,v $
 *------------------------------------------------------------------
 * General Protocol Converter Based on CLSI Primitives
 * 
 * September 1994,  Meng Lyu
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dlcsw.c,v $
 * Revision 3.8.8.9  1996/08/07  08:57:38  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.8.8.8  1996/08/04  00:54:05  dignacio
 * CSCdi61179:  FRAS RSRB Dial Backup for FRAS BNN fails on SDLC attached
 * nodes.
 * Branch: California_branch
 *
 * Revision 3.8.8.7  1996/08/04  00:25:38  dignacio
 * CSCdi65059:  Unconfiguring fras map sdlc  command crashes router
 * Branch: California_branch
 *
 * Revision 3.8.8.6  1996/06/28  23:29:54  dignacio
 * CSCdi47909:  show fras command does not include dest. mac-address
 * Branch: California_branch
 *
 * Revision 3.8.8.5  1996/06/04  17:50:17  dignacio
 * CSCdi58769:  Fras fails with a fr_doencap failed message
 * Branch: California_branch
 *
 * Revision 3.8.8.4  1996/05/23  21:23:13  dignacio
 * CSCdi56882:  FRAS-BAN does not forward XID/F from FEP
 * Branch: California_branch
 *
 * Revision 3.8.8.3  1996/05/17  10:42:44  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.6.6  1996/05/10  21:14:20  dignacio
 * CSCdi56233:  cleanup FRAS backup code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.6.5  1996/05/05  22:51:22  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.7.6.4  1996/04/26  07:33:10  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.6.3  1996/04/16  00:35:21  dignacio
 * CSCdi36042:  FRAS RIF direction bit not changing
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.6.2  1996/04/03  13:54:50  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.6.1  1996/03/17  17:36:56  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.8.8.2  1996/04/11  23:09:00  monical
 * CSCdi53198:  show fras command output exceeds terminal length
 * Branch: California_branch
 *
 * Revision 3.8.8.1  1996/03/18  19:10:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.6.4  1996/03/16  06:32:28  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.6.3  1996/03/13  01:14:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.6.2  1996/03/07  08:41:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.6.1  1996/02/20  00:22:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/28  09:32:34  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.7  1996/02/01  06:00:34  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/11  02:45:31  dignacio
 * CSCdi45225:  No error message when FRAS/BAN configured without
 * ring-group
 *
 * Revision 3.5  1995/12/16  22:49:05  sberl
 * CSCdi45227:  FRAS/BAN and BNN dont work together
 * Get rid of return in loop to make sure that all sap in the sap list get
 * activated, instead of just the first in the list.
 * Also, fix alignment error when no backup defined.
 * General code cleanup.
 *
 * Revision 3.4  1995/12/12  18:53:32  vrawat
 * CSCdi45396:  FRAS : memory leak when Data Confirm received
 *
 * Revision 3.3  1995/12/08  17:22:19  sberl
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
 * Revision 3.2  1995/11/17  08:55:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  20:53:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/10/11  04:20:20  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.10  1995/09/15  17:08:55  sberl
 * CSCdi40292:  FRAS backup needs to retry
 * Implement retry logic for fras backup command. We will not try to
 * activate the primary link several times before switching over to the
 * backup.
 *
 * Revision 2.9  1995/09/13  22:29:35  sberl
 * CSCdi40289:  FRAS backup doesnt clean up vring
 * Copy portID from enable.cnf into port structure. Use this in the
 * disable.req. Do some code cleanup while in the area.
 *
 * Revision 2.8  1995/08/13  19:04:02  kchiu
 * CSCdi38691:  FRAS: the frame-relay map commmand must come after the
 * sdlc address
 *
 * Revision 2.7  1995/08/13  18:28:52  kchiu
 * CSCdi38280:  removing fras backup rsrb command removes fras map command
 *
 * Revision 2.6  1995/07/20  21:13:24  kchiu
 * CSCdi37258:  FRAS Dial-Backup for multiple stations failed
 *
 * Revision 2.5  1995/07/12  22:16:38  kchiu
 * CSCdi33081:  removing fras dial-backup command crashes router
 *
 * Revision 2.4  1995/07/02  05:15:40  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.3  1995/06/18  19:40:03  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  12:59:29  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:20:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#include "master.h"
#include <string.h>
#include "subsys.h"
#include "address.h"
#include "interface.h"
#include "interface_generic.h"
#include "../cls/clsi_msg.h"
#include "../cls/clsi_queue.h"
#include "config.h"
#include "ieee.h"
#include "parser_defs_dlcsw.h"
#include "../if/rif.h"
#include "dlu.h"
#include "dlcsw.h"
#include "../ui/debug.h"
#include "dlcsw_debug.h"
#include "../wan/frame_relay.h"
#include "../srt/rsrb.h"
#include "../ibm/sdlc.h"
#include "../ibm/sdlc_registry.h"
#include "../wan/fr_registry.regh"

static void frDLCIStatusChange_fras(fr_pvc_info_type *fr_pvc);
static uchar get_free_sap(void);

char *ls2aTable[NUM_LS_STATES] = {
	"illegal", "ls_reset", "ls_RqOpnStnSent", "ls_ExchgXid",
	"ls_ConnRqSent", "ls_SigStnWait", "ls_ConnRspWait",
	"ls_ConnRspSent", "ls_Contacted", "ls_DiscWait"
};
/*
 * Global variables for all of dlcswitch
 */
static boolean pcdlu_enable;
int pcdlu_pid;
static TDLUEntity pcDlu, *pcDluPtr;
static queuetype   pcdlu_iqueue;
static TDLUBackup *Backup=NULL;
static uchar FrasMaxRetry; 
static TDLUPort *globalPort=NULL;


/* boolean pcdlu_enable;
int pcdlu_pid;
TDLUEntity pcDlu, *pcDluPtr;
queuetype   pcdlu_iqueue;
TDLUBackup *Backup=NULL;
uchar DummyMac[6]={0x00,0x00,0x00,0x00,0x00,0x00};
uchar FrasMaxRetry; */


static void dlcsw_init (subsystype *subsys)
{
    dlcsw_debug_init();
    pcDluPtr = &pcDlu;
    DLUInit(pcDluPtr);
    reg_add_frDLCIStatusChange(frDLCIStatusChange_fras, "frDLCIStatusChange_fras");
}

static boolean pcDluInit (void)
{

    if (!(pcDluPtr->fQinit = CLSIQInitCreate("FRAS Daemon", pcDluReadPut))) {
	dlcSwErrDebug("\nFRAS: Qinit Failed");
	return FALSE;
    }
		
    if (!(pcDluPtr->fReadQ = CLSIQueueCreate(pcDluPtr->fQinit, NULL))) {
	dlcSwErrDebug("\nFRAS: ReadQ Open Failed");
	CLSIQInitDestroy(pcDluPtr->fQinit);
	return FALSE;
    }

    if (!(pcDluPtr->fWriteQ = CLSOpen(pcDluPtr->fReadQ))) {
	dlcSwErrDebug("\nFRAS: WriteQ Open Failed");
	CLSIQueueDestroy(pcDluPtr->fReadQ);
	CLSIQInitDestroy(pcDluPtr->fQinit);
	return FALSE;
    }

    if (cfork (pcdlu_main, 0L, 4000, "FRAS Server", 0) < 0) {
	CLSIQueueDestroy(pcDluPtr->fWriteQ);
	CLSIQueueDestroy(pcDluPtr->fReadQ);
	CLSIQInitDestroy(pcDluPtr->fQinit);
	return FALSE;
    }

    /*
     * Initialize DLU test global variables here...
     */
    pcdlu_enable = TRUE;

    return TRUE;
}

int pcDluReadPut(TCLSIQueue* dluReadQ, TCLSIMsg* theMsg)
{
    hword primitive;
#ifdef DLCSW_FS
    word id;
    TDLUCep *aCep;
#endif

    primitive = CLSIMsgGetPrimitiveCode(theMsg);

    switch (primitive) {
    case CLS_DATA | CLS_INDICATION :
    case CLS_DATA | CLS_RESPONSE:
#ifdef DLCSW_FS
	id = CLSIMsgGetIdentifier(theMsg);
	aCep = (TDLUCep *)id;
	if (aCep->fCvtPtr->fCvtState == C_CONN_ACTIVE)
	    DLUFsData(aCep, theMsg);
	else
    	    p_enqueue(&pcdlu_iqueue, theMsg);
	break;
#endif
    default:
        p_enqueue(&pcdlu_iqueue, theMsg);
	break;
    }
    return (CLS_OK);
}

      
boolean pcdluBlock(void)
{
    if (pcdlu_iqueue.count > 0)
	return(FALSE);
    else
        return(pcdlu_enable);
}

forktype pcdlu_main (void)
{
    word retCode;
    hword primitive;
    TCLSIMsg* theMsg;
    boolean dataFlag;

    while (TRUE) {

	edisms((blockproc*)pcdluBlock, 0L);
	if (!pcdlu_enable)
	    break;

	if (!(theMsg = p_dequeue(&pcdlu_iqueue)))
	    continue;

	primitive = CLSIMsgGetPrimitiveCode(theMsg);
	dataFlag = FALSE;
	    
	switch(primitive) {
	case CLS_ENABLE | CLS_CONFIRM:
	    retCode = CLSIMsgGetReturnCode(theMsg);
            if (retCode == CLS_OK || retCode == CLS_ALREADY_ENABLED) {
	        DLUPortActSap(theMsg);
	        CLSIMsgDestroy(theMsg);
	        break;
            }
	    if (retCode != CLS_OK) {
	    	dlcSwErrDebug("\nDLU Enable failed error 0x%x", retCode);
	    	CLSIMsgDestroy(theMsg);
		break;
	    }
		
	case CLS_ACTIVATE_SAP | CLS_CONFIRM:
	case CLS_ACTIVATE_RING | CLS_CONFIRM:
	{
	    TDLUSap *aSap = (TDLUSap *) ((ActSapCnfh_t *)theMsg)->fIdentifier;

	    retCode = CLSIMsgGetReturnCode(theMsg);
	    if (retCode != CLS_OK) {
		dlcSwErrDebug("\nDLU ActSap failed error 0x%x", retCode);
	    	CLSIMsgDestroy(theMsg);
		break;
	    }

	    aSap->fSapState = DLU_SAP_STATE_ACTIVE;
	    aSap->fPSapId =  ((ActSapCnfh_t *)theMsg)->fPSapId;
		
	    CLSIMsgDestroy(theMsg);
	    break;
	}
	case CLS_DISABLE | CLS_CONFIRM:
	{
	    DisableCnf_t *disableCnf = (DisableCnf_t *)theMsg;
	    TDLUPort *aPort;

	    if ((aPort = DLUFindPort(pcDluPtr, &disableCnf->fPortId))){
                if(aPort == globalPort) {
                    globalPort = NULL;
                }
		DLURemovePort(pcDluPtr, aPort);
		DLUPortDestroy(aPort);
	    }
	    CLSIMsgDestroy(theMsg);

	    if (QUEUEEMPTY(&pcDluPtr->fCepList))
		pcdlu_enable = FALSE;

	    break;
	}

	case CLS_DEACTIVATE_SAP | CLS_CONFIRM:
        case CLS_DEACTIVATE_RING | CLS_CONFIRM:
	{
	    TDLUSap *aSap;

	    if (CLSIMsgGetReturnCode(theMsg) == CLS_OK) {
		aSap = (TDLUSap *)((DeactSapCnfh_t *)theMsg)->fIdentifier;

		aSap->fSapState = DLU_SAP_STATE_RESET;

		DLURemoveSap(pcDluPtr, aSap);
		if (aSap->fPortPtr->fRefCount == 0)
		    DLUSendPortDisable(pcDluPtr, aSap->fPortPtr);

		DLUSapDestroy(aSap);
	    }
	    CLSIMsgDestroy(theMsg);
	    break;
	}

	case CLS_DATA | CLS_CONFIRM:
        {
            retCode = CLSIMsgGetReturnCode(theMsg);
	    if (retCode != CLS_OK) {
		dlcSwErrDebug("\nDLU DataConfirm failed error 0x%x", retCode);
	    	CLSIMsgDestroy(theMsg);
	    }
	    break;
	}

	case CLS_DATA | CLS_INDICATION:
	    dataFlag = TRUE;
	    /* Deliberate fall thru. No break */
	default:
	{
	    TDLUCep* theConn;
	    
	    dlcSwMsgDebug("\nFRAS: receive %x", primitive);
	    theConn = DLUGetCep(theMsg);
	    if (theConn == NULL) {
		if (primitive == (CLS_ID_STN | CLS_INDICATION)) {
		    theConn = DLUFindNewCep(theMsg);
		}
		else {
		    dlcSwErrDebug("\nFRAS  bad u_cep_id");
	    	    CLSIMsgDestroy(theMsg);
		    break;
		}
                if (theConn) {
                  if (theConn->fSapPtr->fDLCType == CLS_FRAME_RELAY) {
                    theConn->fExplorerOption = 0;
                  } else {
                    theConn->fExplorerOption = EO_INBOUND_DEFAULT;
                  }
                  if (theConn->fCepPtr->fSapPtr->fDLCType == CLS_FRAME_RELAY) {
                    theConn->fCepPtr->fExplorerOption = 0;
                  } else {
                    theConn->fCepPtr->fExplorerOption = EO_OUTBOUND_DEFAULT;
                  }
                }
	    }
	    if (theConn == NULL) {
		dlcSwErrDebug("\nFRAS: Can not find the Setup");
	    } else if (primitive == (CLS_CLOSE_STN | CLS_CONFIRM)) {
		/*
		 * We sometimes will destroy the DLUCep before we
		 * get the confirmation of the close_stn, so we need
		 * to be careful to make sure that the ucepid is
		 * still pointing at a valid control block. Verify
		 * that theConn is still in the cep list before
		 * invoking the fsm. If it isn't in the list, we can
		 * go ahead and discard theMsg.
		 */
		if (checkqueue(&pcDluPtr->fCepList, theConn)) {
		    (void)pcls_fsm(theConn, theMsg, 0);
		}
	    } else {
	    	(void)pcls_fsm(theConn, theMsg, 0);
		if (dataFlag && theConn->fCepState == LS_CONTACTED)
		    continue;
	    }
	    /* This discards all except data.ind messages. */
	    CLSIMsgDestroy(theMsg);
	    break;
	} /* end of default case */

	} /* end switch(primitive) */
    } /* end while(TRUE) */

    DLUShutdown(pcDluPtr);

    while (pcdlu_iqueue.count > 0) {
	TCLSIMsg* aMsg = p_dequeue(&pcdlu_iqueue);
	CLSIMsgDestroy(aMsg);
    }
    process_kill(THIS_PROCESS);
}

static TDLUPort* DLUPortInit(TDLUEntity *this, idbtype *idb, CLSDLCType_e portType, 
		hword SrcRn, hword Bridge, hword TargetRn)
{
    TDLUPort *aPort;
    
    aPort = (TDLUPort*) this->fPortList.qhead;
    while (aPort != NULL) {
	if (portType == CLS_RSRB) {
	    if ((aPort->fSrcRn == SrcRn) &&
			(aPort->fBridge == Bridge) &&
			(aPort->fTargetRn == TargetRn)) {
		return aPort;
            }
	} else if (aPort->fPortId.fTypeInfo.swidb == idb) {
	    return aPort;
	}
	aPort = aPort->fNext;
    }

    if (!(aPort = DLUPortCreate())) {
	return aPort;
    }

    aPort->fPortId.type = CLS_P_TYPE_SWIDB;
    aPort->fPortId.fTypeInfo.swidb = idb;
    aPort->fDLCType = portType;
    aPort->fPortState = DLU_PORT_STATE_RESET;
    aPort->fSrcRn = SrcRn;
    aPort->fBridge = Bridge;
    aPort->fTargetRn = TargetRn;
    aPort->fDlu = this;
    aPort->fRefCount = 0;

    DLUAddPort(this, aPort);
    return aPort;
}

static TDLUSap* DLUSapInit(TDLUPort* thePort, word lsap, boolean isActSap)
{
    TDLUSap *aSap1;

    aSap1 = (TDLUSap *)thePort->fDlu->fSapList.qhead;
    while (aSap1 != NULL) {
	if ((aSap1->fPortPtr == thePort) &&
			(aSap1->fSapNumber == lsap)) {
	    return aSap1;
	}
	aSap1 = aSap1->fNext;
    }
    if(!(aSap1 = DLUSapCreate(thePort))) 
	return aSap1;

    aSap1->fSapNumber = lsap;
    aSap1->fRefCount = 0;
    if (isActSap) {
        aSap1->fActivate = DLUSendActSap;
        aSap1->fDeactivate = DLUSendDeactSap;
    }
    else {
        aSap1->fActivate = DLUSendActRing;
        aSap1->fDeactivate = DLUSendDeactRing;
    }
    DLUAddSap(thePort->fDlu, aSap1);

    return aSap1;
}

static TDLUCvt* DLUCvtInit(TDLUEntity* this, TDLUCep* aCep1, TDLUCep* aCep2) 
{
    TDLUCvt* aConn;

    if (!(aConn = DLUCvtCreate(this))) {
	dlcSwErrDebug("\nFRAS: Switcher Created Failed");
	return aConn;
    }

    aConn->fPriCep = NULL;
    aConn->fSecCep = NULL;
    aCep1->fCvtPtr = aConn;
    aCep1->fCepPtr = aCep2;
    aCep2->fCvtPtr = aConn;
    aCep2->fCepPtr = aCep1;

    DLUAddCvt(this, aConn);

    return aConn;
}

static TDLUCep* DLUCepInit(TDLUSap *aSap, hwaddrtype hwAddr, 
			int lsap, int rsap, boolean genFlag)
{
    TDLUCep *aCep;

    if (!(aCep = DLUCepCreate(aSap))) {
	dlcSwErrDebug("\nFRAS: Cep Init Failed");
	return NULL;
    }
    aCep->fLSap = lsap;
    aCep->fRSap = rsap;
    aCep->fCepState = LS_RESET;
    aCep->fReqOpnStnMaxTry = 0;
    if (genFlag)
        aCep->fFlags |= FDLCSW_NVGEN;
    bcopy(&hwAddr, &aCep->fDstAddr, sizeof(hwaddrtype));
    DLUAddCep(pcDluPtr, aCep);
    return aCep;
}

static TDLUCep* DLUCheckCep(idbtype *idb, 
			hwaddrtype DstAddr, int lSap, int rSap)
{
    TDLUCep *aCep;
	
    aCep = pcDluPtr->fCepList.qhead;
    while (aCep != NULL) {
	if ((aCep->fRSap == rSap) && (aCep->fLSap == lSap) &&
        	(idb == aCep->fSapPtr->fPortPtr->fPortId.fTypeInfo.swidb) &&
		(snpa_equ(&DstAddr, &aCep->fDstAddr))) 
	    break;
	aCep = aCep->fNext;
    }
    return aCep;
}

static TDLUCep* DLUIdbToCep(TDLUCep *aCep, idbtype *idb)
{
    if (aCep == NULL)
        return(NULL);
    for (; aCep; aCep = aCep->fNext) {
        if (aCep->fSapPtr->fPortPtr->fPortId.fTypeInfo.swidb == idb)
	    return(aCep);
        else
	    continue;
    }
    return (NULL);
}

static idbtype* DLUCepToIdb(TDLUCep *aCep)
{
    return(aCep->fSapPtr->fPortPtr->fPortId.fTypeInfo.swidb);
}

uchar get_free_sap()
{
    TDLUSap* theSap;
    uchar free_sap = 8;
    boolean found = FALSE;

    while(free_sap < 256) {
        theSap = (TDLUSap*) pcDluPtr->fSapList.qhead;
        found = FALSE;
        while (theSap != NULL) {
            if(theSap->fSapNumber == free_sap) {
                found = TRUE;
                free_sap = free_sap + 2;
                break;
            }
            theSap = theSap->fNext;
        }
        if(!found) return free_sap;
    }
    dlcSwErrDebug("\nFRAS: No more free saps available for BAN backup");
    return 0;
}


boolean DLUBackupEnable(TDLUCep *thisCep)
{
    TDLUPort *aBackupPort;
    TDLUSap *aBackupSap;
    TDLUCep *aBackupCep;
    TDLUCep *aCep;
    TDLUCvt *aConn;
    hwaddrtype hwAddr;
    uchar free_sap=8;

    if(!globalPort) {
      if (!(aBackupPort = DLUPortInit(pcDluPtr, NULL, CLS_RSRB, 
	Backup->fLocalRing, Backup->fBridgeNumber, Backup->fTargetRing))) {
        dlcSwErrDebug("\nFRAS: Backup Port Init Failed");
        return (FALSE);
      }
      globalPort = aBackupPort;
    }
    aBackupPort = globalPort;

    if(thisCep->fFrasType == FRAS_BAN) {
        free_sap = get_free_sap();
        dlcSwErrDebug("\nDLUBackupEnable - SapInit - BAN lsap = %d",free_sap);
        if (!(aBackupSap = DLUSapInit(aBackupPort, free_sap, TRUE))) {
            dlcSwErrDebug("\nFRAS: Backup Sap Init Failed");
            return (FALSE);
        }
    } else {
        if (!(aBackupSap = DLUSapInit(aBackupPort, thisCep->fLSap, TRUE))) {
            dlcSwErrDebug("\nFRAS: Backup Sap Init Failed");
            return (FALSE);
        }
    }
    snpa_init(&hwAddr, STATION_IEEE48, TARGET_UNICAST,
                Backup->fHostAddr);

    if(thisCep->fFrasType == FRAS_BAN) {
        dlcSwErrDebug("\nDLUBackupEnable - CepInit - BAN lsap = %d",free_sap);
        if (!(aBackupCep = DLUCepInit(aBackupSap, hwAddr,
              free_sap, thisCep->fRSap, FALSE))) {
            dlcSwErrDebug("\nFRAS: Backup Cep Init Failed");
            return (FALSE);
        }
    } else {
        if (!(aBackupCep = DLUCepInit(aBackupSap, hwAddr, 
              thisCep->fLSap, thisCep->fRSap, FALSE))) {
            dlcSwErrDebug("\nFRAS: Backup Cep Init Failed");
            return (FALSE);
        }
    }
    aBackupCep->fL3Pid = thisCep->fCepPtr->fL3Pid;
    aBackupCep->fExplorerOption = EO_OUTBOUND_DEFAULT;
    aBackupCep->fRifLength = 0;
    aBackupCep->fCepState = LS_RESET;
    DLURemoveCvt(pcDluPtr, thisCep->fCvtPtr);
    DLUCvtDestroy(thisCep->fCvtPtr);
    thisCep->fCepPtr->fCepOrigPtr = thisCep;

    aBackupCep->fCepPtr = thisCep->fCepPtr;

    aCep = thisCep->fCepPtr;
    aCep->fCepPtr = aBackupCep;

    if ((aConn = DLUCvtInit(pcDluPtr, aBackupCep->fCepPtr, aBackupCep))) {
            if (!globalPort->fPortState) {
            DLUSendPortEnable(pcDluPtr, aBackupPort, Backup->fVirAddr,
            Backup->fLocalRing, Backup->fBridgeNumber, Backup->fTargetRing);
      } else if (aBackupSap->fSapState == DLU_SAP_STATE_RESET) {
            CLSIPortIdCopy(&aBackupPort->fPortId, &aBackupSap->fPortId);
            DLUSendActSap(pcDluPtr, aBackupSap);
        }
    }
    else {
        dlcSwErrDebug("FRAS: Backup Enable Failed");
        return (FALSE);
    }
    return (TRUE);
}

static void DLURemoveBackupCep(void)
{
    TDLUCep* aCep;

    aCep = pcDluPtr->fCepList.qhead;
    while (aCep != NULL) {
        if (aCep->fCepOrigPtr) {
            aCep->fFlags |= FDLCSW_DELETE;
            aCep->fCepPtr->fFlags |= FDLCSW_DELETE;
            if ((aCep->fCepState == LS_RESET) &&
                (aCep->fCepPtr->fCepState == LS_RESET)) {
                DLUReplaceBackupCep(aCep);
            }
            else {
                pcls_fsm(aCep, NULL, SND_DISC);
                pcls_fsm(aCep->fCepPtr, NULL, SND_DISC);
            }
        }
        aCep = aCep->fNext;
    }
}

void DLUReplaceBackupCep(TDLUCep* thisCep)
/*  The variable "thisCep" is the lan-cep value.  */
{
    TDLUCvt *aConn;

    DLURemoveCvt(pcDluPtr, thisCep->fCvtPtr);
    DLUCvtDestroy(thisCep->fCvtPtr);
    DLURemoveCep(pcDluPtr, thisCep->fCepPtr);
    if ((thisCep->fCepPtr->fSapPtr->fRefCount == 0) &&
	(thisCep->fCepPtr->fSapPtr->fSapState == DLU_SAP_STATE_ACTIVE)) { 
        (*thisCep->fCepPtr->fSapPtr->fDeactivate)(pcDluPtr,
                thisCep->fCepPtr->fSapPtr);
    }
    DLUCepDestroy(thisCep->fCepPtr);
    thisCep->fCepPtr = thisCep->fCepOrigPtr;
    if ((thisCep->fCepOrigPtr->fFlags & FDLCSW_DELETE) != FDLCSW_DELETE)
        thisCep->fFlags &= ~FDLCSW_DELETE;
    thisCep->fCepOrigPtr->fCepPtr = thisCep;
    thisCep->fCepOrigPtr = NULL;
    if (!(aConn = DLUCvtInit(pcDluPtr, thisCep, thisCep->fCepPtr))) {
        dlcSwErrDebug("\nFRAS: DLUReplaceBackupCep() Converter Init Failed");
    }    
}

static void DLURemoveBanCep(TDLUBan *aBanCmd)
{
    TDLUCep* aCep;

    aCep = pcDluPtr->fCepList.qhead;
    while (aCep != NULL) {
        if ((aCep->fFrasType == FRAS_BAN) && 
            (aCep->fSapPtr->fDLCType == CLS_FRAME_RELAY) &&
            ((ieee_equal(aCep->fCepPtr->fSrcAddr.addr, aBanCmd->fDlciMacAddr)))) {
            aCep->fFlags |= FDLCSW_DELETE;
            aCep->fCepPtr->fFlags |= FDLCSW_DELETE;
            if (!(DLURouteDestroy(aCep, aCep->fCepPtr))) {
                pcls_fsm(aCep, NULL, SND_DISC);
                pcls_fsm(aCep->fCepPtr, NULL, SND_DISC);
            }
	    /*
	     * If we did a RouteDestroy above, the list was modified
	     * and what was the next list element may not be there any
	     * more. We need to start again at the top of the list to
	     * make sure things work properly.
	     */
	    aCep = pcDluPtr->fCepList.qhead;
        } else {
	    aCep = aCep->fNext;
	}
    }
}



static boolean BanCmdValid(TDLUBan *this)
{
    TDLUBan *aBanCmd;

    aBanCmd = (TDLUBan *) pcDluPtr->fBanCmdList.qhead;
    while (aBanCmd) {
         if (aBanCmd->fType == PCDLU_BAN) {
             if (aBanCmd->fLocalRing ==  this->fLocalRing) {
                 printf("\nDuplicate buffer ring number: %d",
                     aBanCmd->fLocalRing);
                 return FALSE;
             }
             if (ieee_equal(aBanCmd->fDlciMacAddr, this->fDlciMacAddr)) {
                 printf("\nDuplicate DLCI-MAC-ADDRESS: %e",
                     aBanCmd->fDlciMacAddr);
                 return FALSE;
             }
         } else {
             if (aBanCmd->fLan_idb == this->fLan_idb) {
                 printf("\nremove the existing command first");
                 return FALSE;
             }
	}
        aBanCmd = aBanCmd->fNext;
    }
    if (this->fType == PCDLU_BAN) {
        if (!vrfind_ringgroup(this->fTargetRing)) {
            printf("\nThis Target-Ring has no source-bridge ring-group definition.");
            return FALSE;
	  }
    }
    return TRUE;
}

ushort DLUGetBanDlci(TDLUCep *aCep)
{
    TDLUBan *aBanCmd;

    aBanCmd = (TDLUBan *)pcDluPtr->fBanCmdList.qhead;
    while (aBanCmd) {
        if (ieee_equal(aBanCmd->fDlciMacAddr, aCep->fSrcAddr.addr)) {
            if (aBanCmd->fAvailDlci < aBanCmd->fTotalDlci - 1)
                aBanCmd->fAvailDlci++;
            else
                aBanCmd->fAvailDlci = 0;

            DLUUpdateAvailDlci(aBanCmd);

	    if (aBanCmd->fAvailDlci == NO_DLCI) {
                dlcSwErrDebug(", returns - 0");
                return 0;
            } else {
                dlcSwErrDebug(", returns - %d",aBanCmd->fDlci[aBanCmd->fAvailDlci].number);
                return (aBanCmd->fDlci[aBanCmd->fAvailDlci].number);
	    }
        }
        aBanCmd = aBanCmd->fNext;
    }
    dlcSwErrDebug("DLUGetBanDlci - failed!!");
    return 0;
}

void DLUBanCepEnable(TDLUCep *aCep)
{
    TDLUCep *aBanCep;
    TDLUCep *thisCep;
    TDLUCep *newCep;
    TDLUCvt *aConn;
    hwaddrtype hwaddr1;
    ushort aDlci=0;

    if (aCep->fFrasType == FRAS_BNN) 
	return;
    /* Always initialize the cep for lan first */
    if (aCep->fSapPtr->fDLCType == CLS_FRAME_RELAY)
	aCep = aCep->fCepPtr;

    if (!(thisCep = DLUCepInit(aCep->fSapPtr, hwaddr1, 0x4, 0x4, TRUE))) {
        dlcSwErrDebug("\nFRAS-BAN: DLCI-MAC-ADDRESS Cep Init Failed");
        return;
    }
    snpa_copy(&aCep->fSrcAddr, &thisCep->fSrcAddr);
    bzero(aCep->fDstAddr.addr, IEEEBYTES);
    thisCep->fDlci = 0;

    newCep = aCep->fCepPtr;
    if (aCep->fCepOrigPtr) {
        newCep = aCep->fCepOrigPtr;
    }


    if (!(aBanCep = DLUCepInit(newCep->fSapPtr, newCep->fDstAddr,
        0x4, 0x4, FALSE))) {
        dlcSwErrDebug("\nFRAS-BAN: BAN Cep Init Failed");
        return;
    }

    if ((aDlci = DLUGetBanDlci(aCep))) {
        aBanCep->fDlci = aDlci;
    } else {
        aBanCep->fDlci = newCep->fDlci;
        thisCep->fDlci = newCep->fDlci;
        dlcSwErrDebug("\nFRAS-BAN: No DLCI is available");
    }

    thisCep->fExplorerOption = EO_INBOUND_DEFAULT;
    thisCep->fRifLength = 0;
    thisCep->fFrasType = FRAS_BAN;
    aBanCep->fFrasType = FRAS_BAN;
    ieee_copy(newCep->fBniAddr, aBanCep->fBniAddr);
    if (!(aConn = DLUCvtInit(pcDluPtr, thisCep, aBanCep)))
        dlcSwErrDebug("FRAS: BAN Enable Failed");
}

void DLUBanEnable(TDLUBan *aBanCmd, CLSDLCType_e portType)
{
    TDLUPort *aBanPort;
    TDLUPort *aPort;
    TDLUSap *aBanSap;
    TDLUSap *aSap;
    TDLUCep *aBanCep;
    TDLUCep *aCep;
    TDLUCvt *aConn;
    hwaddrtype hwAddr1;
    hwaddrtype hwAddr2;
    idbtype *idb=NULL;
    word lsap = SAP_IBM_SNA;
 
 
    if (aBanCmd->fType != PCDLU_BAN) {
        idb = aBanCmd->fLan_idb;
        lsap = DLSW_SAP;
    }

    if (!(aPort = DLUPortInit(pcDluPtr, idb, portType, 
	  aBanCmd->fLocalRing, aBanCmd->fBridgeNumber, aBanCmd->fTargetRing))) {
        dlcSwErrDebug("\nFRAS: DLCI-MAC-ADDRESS Port Init Failed");
        return;
    }

    if (!(aSap = DLUSapInit(aPort, lsap,
	    (portType != CLS_SDLC) ? TRUE : FALSE))) {
        dlcSwErrDebug("\nFRAS: DLCI-MAC-ADDRESS Sap Init Failed");
        return;
    }

    if (!(aCep = DLUCepInit(aSap, hwAddr1, SAP_IBM_SNA, SAP_IBM_SNA, TRUE))) {
        dlcSwErrDebug("\nFRAS: DLCI-MAC-ADDRESS Cep Init Failed");
        return;
    }
    snpa_init(&aCep->fSrcAddr, STATION_IEEE48, TARGET_UNICAST, 
              aBanCmd->fDlciMacAddr);
    bzero(aCep->fDstAddr.addr, IEEEBYTES);
    aCep->fDlci = 0;
    if (!(aBanPort = DLUPortInit(pcDluPtr, aBanCmd->fIdb, CLS_FRAME_RELAY, 0, 0, 0))) {
        dlcSwErrDebug("\nFRAS: BAN Port Init Failed");
        return;
    }
    if (!(aBanSap = DLUSapInit(aBanPort, SAP_IBM_SNA, TRUE))) {
        dlcSwErrDebug("\nFRAS: BAN Sap Init Failed");
        return;
    }

    snpa_init(&hwAddr2, STATION_IEEE48, TARGET_UNICAST, aBanCmd->fBniMacAddr);

    if (!(aBanCep = DLUCepInit(aBanSap, hwAddr2, 0x4, 0x4, FALSE))) {
        dlcSwErrDebug("\nFRAS: BAN Cep Init Failed");
        return;
    }

    aBanCep->fDlci = aBanCmd->fDlci[0].number; 

    aCep->fFrasType = FRAS_BAN;
    aCep->fExplorerOption = EO_INBOUND_DEFAULT;
    aCep->fRifLength = 0;
    aBanCep->fFrasType = FRAS_BAN;
    ieee_copy(aBanCmd->fBniMacAddr, aBanCep->fBniAddr);
    if ((aConn = DLUCvtInit(pcDluPtr, aCep, aBanCep))) {
        DLUSendPortEnable(pcDluPtr, aPort, aBanCmd->fDlciMacAddr,
            aBanCmd->fLocalRing, aBanCmd->fBridgeNumber, aBanCmd->fTargetRing);
        DLUSendPortEnable(pcDluPtr, aBanPort, NULL, 0, 0, 0);
    }
    else {
        dlcSwErrDebug("FRAS: BAN Enable Failed");
    }
}
           

void pcdlu_command (parseinfo *csb)
{
    idbtype *idb1 = csb->interface;
    idbtype *idb2 = NULL;
    TDLUPort *aPort1, *aPort2;
    TDLUSap *aSap1, *aSap2;
    TDLUCep *aCep1, *aCep2;
    TDLUCvt *aConn;
    int lsap1, rsap1, lsap2, rsap2;
    int dlci = 0;
    int addrType1, addrType2;
    uchar dstAddr1[IEEEBYTES], dstAddr2[IEEEBYTES];
    CLSDLCType_e portType1, portType2;
    byte *pidString;
    hwaddrtype hwAddr1, hwAddr2;
    uchar defaultBniMacAddr[IEEEBYTES] = {0x4F,0xFF,0x00,0x00,0x00,0x00};
    int dlci_count, i = 0; 
    TDLUBan *aBanCmd=NULL, Ban;

    if (csb->nvgen) {

        if (csb->which == PCDLU_BAN) {
            aBanCmd = (TDLUBan *)pcDluPtr->fBanCmdList.qhead;
            while (aBanCmd) {
                    if (aBanCmd->fType == PCDLU_BAN && aBanCmd->fIdb == idb1) {
                        nv_write(TRUE, "%s %d %d %d %e %s %d ", csb->nv_command,
			aBanCmd->fLocalRing, aBanCmd->fBridgeNumber,
			aBanCmd->fTargetRing, aBanCmd->fDlciMacAddr, "dlci",
			aBanCmd->fDlci[0].number);
                    }
                    else if  (aBanCmd->fLan_idb == idb1) {
                        nv_write(TRUE, "%s %s %s %e %s %d ", csb->nv_command,
			"frame-relay", aBanCmd->fIdb->namestring,
			aBanCmd->fDlciMacAddr, "dlci",
			aBanCmd->fDlci[0].number);
                    } else {
                        aBanCmd = aBanCmd->fNext;
                        continue;
                    }
                    for (dlci_count = 1; dlci_count < aBanCmd->fTotalDlci;
			 dlci_count++) {
                         nv_add (TRUE, "%d ", aBanCmd->fDlci[dlci_count].number);
                    }
                    if (!ieee_equal(aBanCmd->fBniMacAddr,defaultBniMacAddr))
                        nv_add (TRUE, "%s %e ", "bni", aBanCmd->fBniMacAddr);
 
                aBanCmd = aBanCmd->fNext;
            }
	    return;
	}

        if (!pcdlu_enable || 
	        !(aCep1 = (TDLUCep *)pcDluPtr->fCepList.qhead)) {
	    return;
	}
        if ((csb->which == PCDLU_BACKUP) && 
	    (Backup != NULL) && 
	    (Backup->fIdb == idb1)) {
            if (FrasMaxRetry) {
                nv_write(TRUE, "%s %s %e %d %d %e retry %d", csb->nv_command,
                    "rsrb", Backup->fVirAddr, Backup->fLocalRing,
                    Backup->fTargetRing, Backup->fHostAddr, FrasMaxRetry - 1);
            } else {
                nv_write(TRUE, "%s %s %e %d %d %e", csb->nv_command,
                    "rsrb", Backup->fVirAddr, Backup->fLocalRing,
                    Backup->fTargetRing, Backup->fHostAddr);
            }
            return;
        }

        if (csb->which != PCDLU_ROUTE) {
            return;
	}
        while ((aCep1 = DLUIdbToCep(aCep1,idb1)) != NULL) {
            if (aCep1->fFlags & FDLCSW_NVGEN) {
                if  (aCep1->fFrasType == FRAS_BAN ||
                        aCep1->fCepPtr->fFrasType ==FRAS_BAN)
                    goto nextcep;
                portType1 = aCep1->fSapPtr->fDLCType;
                if (aCep1->fCepOrigPtr != NULL)
                    aCep2 = aCep1->fCepOrigPtr;
                else
                    aCep2 = aCep1->fCepPtr;
                idb2 = DLUCepToIdb(aCep2);
                portType2 = aCep2->fSapPtr->fDLCType;
		pidString = ((aCep2->fL3Pid == CLS_L3PID_FID4) ? "fid4"  :
			     ((aCep2->fL3Pid == CLS_L3PID_FID2) ? "pfid2" :
			     ""));
                switch (portType1) {
                case CLS_SDLC:
                    if (portType2 == CLS_LLC) {
                        nv_write(TRUE, "%s %s %x %s llc %e %x %x",
                            csb->nv_command, "sdlc", aCep1->fLSap,
                            idb2->namestring,
                            aCep2->fDstAddr.addr,
                            aCep2->fLSap, aCep2->fRSap);
                    }
                    if (portType2 == CLS_FRAME_RELAY) {
                        dlci = ((aCep2->fDstAddr.addr[0] << 8) |
                                                aCep2->fDstAddr.addr[1]);
                        nv_write(TRUE, "%s %s %x %s %s %d %x %x %s",
                            csb->nv_command, "sdlc", aCep1->fLSap,
                            idb2->namestring, "frame-relay",
                            dlci, aCep2->fLSap, aCep2->fRSap, pidString);
                    }
		    break;
                case CLS_LLC:
                    if (portType2 == CLS_LLC) {
                        nv_write(TRUE,
                            "%s %s %e %x %x %s %s %e %x %x",
                            csb->nv_command, "llc", aCep1->fDstAddr.addr,
                            aCep1->fLSap, aCep1->fRSap,
                            idb2->namestring, "llc", aCep2->fDstAddr.addr,
                            aCep2->fLSap, aCep2->fRSap);
                    }
                    if (portType2 == CLS_FRAME_RELAY) {
                        dlci = ((aCep2->fDstAddr.addr[0] << 8) |
                                                aCep2->fDstAddr.addr[1]);
                        nv_write(TRUE, "%s %s %e %x %x %s %s %d %x %x %s",
                            csb->nv_command, "llc", aCep1->fDstAddr.addr,
                            aCep1->fLSap, aCep1->fRSap,
                            idb2->namestring, "frame-relay", dlci,
                            aCep2->fLSap, aCep2->fRSap, pidString);
                    }
		    break;
                default:
		    return;
                }                           
            }
nextcep:
            aCep1 = aCep1->fNext;
        }
    return;
    }

    /* Initialize background process and register
     * with CLS component
     */

    if (!pcdlu_enable && !pcDluInit()) {
	dlcSwErrDebug("FRAS: Init Failed");
	return;
    }

    if (csb->which == PCDLU_BACKUP) {
        if (!csb->sense) {
            if ((Backup) && (Backup->fIdb == idb1)) {
		/*
		 * sberl - At this point we need to disable the RSRB backup
		 * port, so end a message to CLSI.
		 */
                free(Backup);
                Backup = NULL;
                FrasMaxRetry = 0;
                DLURemoveBackupCep();
            }
            return;
        }
        if ((Backup) && (Backup->fIdb == idb1)) {
            printf("\nYou have to remove the existing command.");
            return;
        }
        if (Backup == NULL)
            Backup = malloc(sizeof (TDLUBackup));
        if (Backup == NULL) {
            printf(nomemory);
            return;
        }
        bzero(Backup->fVirAddr, IEEEBYTES);
        bzero(Backup->fHostAddr, IEEEBYTES);
        ieee_copy(GETOBJ(hwaddr, 1)->addr, Backup->fVirAddr);
        ieee_copy(GETOBJ(hwaddr, 4)->addr, Backup->fHostAddr);
        Backup->fLocalRing = GETOBJ(int, 2);
        Backup->fBridgeNumber = CLS_RSRB_DEFAULT_BRIDGE_NUM;
        Backup->fTargetRing = GETOBJ(int, 3);
        Backup->fIdb = idb1;	/* frame relay subinterface to back up */
        if (GETOBJ(int, 5))
            FrasMaxRetry = GETOBJ(int, 5) + 1;
	/*
	 * sberl - At this point we have all the info we need to
	 * enable the virtual interface for the RSRB backup port. Let's
	 * go ahead and do that now. If there is an error returned in
	 * the enable.cnf, we will report it asynchronously with an errmsg.
	 */
        return;
    }

    if (csb->which == PCDLU_BAN_SDLC) {
        if (!ISCLSSDLC(idb1->hwptr)) {
            printf("%%SDLC encapsulation is not configured on this interface.");
            return;
	}
    }
 
    if (csb->which == PCDLU_BAN || csb->which == PCDLU_BAN_SDLC) {
        if (!csb->sense) {
            aBanCmd = (TDLUBan *)pcDluPtr->fBanCmdList.qhead;
            while (aBanCmd) {
                if (aBanCmd->fIdb == idb1 &&
                    aBanCmd->fDlci[0].number == GETOBJ(int, 4)) {
                    DLURemoveBanCep(aBanCmd);
                    DLURemoveBanCmd(pcDluPtr, aBanCmd);
                    DLUBanCmdDestroy(aBanCmd);
                    return;
                }
                else if (aBanCmd->fLan_idb == idb1) {
                    DLURemoveBanCep(aBanCmd);
                    DLURemoveBanCmd(pcDluPtr, aBanCmd);
                    DLUBanCmdDestroy(aBanCmd);
                    return;
                  }
                aBanCmd = aBanCmd->fNext;
            }
	    return;
        }
        bzero(Ban.fDlciMacAddr, IEEEBYTES);
        bzero(Ban.fBniMacAddr, IEEEBYTES);
        Ban.fType = csb->which;

        if (csb->which == PCDLU_BAN) {
            ieee_copy(GETOBJ(hwaddr, 1)->addr, Ban.fDlciMacAddr);
            Ban.fLocalRing = GETOBJ(int, 1);
            Ban.fTargetRing = GETOBJ(int, 3);
            Ban.fBridgeNumber = GETOBJ(int, 2);
        }
        
        if (GETOBJ(hwaddr, 2)->length) { 
            ieee_copy(GETOBJ(hwaddr, 2)->addr, Ban.fBniMacAddr);
	} else {         
            ieee_copy(defaultBniMacAddr, Ban.fBniMacAddr);
	}
       
        if (csb->which == PCDLU_BAN) {
            Ban.fIdb = idb1;
            Ban.fType = PCDLU_BAN;
        } else { /* csb->which == PCDLU_BAN_SDLC */
            Ban.fType = PCDLU_BAN_SDLC;
            Ban.fIdb = GETOBJ(idb, 1);
            Ban.fLan_idb = idb1;
            ieee_copy(GETOBJ(hwaddr, 1)->addr, Ban.fDlciMacAddr);
	    if (Ban.fIdb->hwptr->sdlc_default == DLSW_SDLC) {
	      printf("\nDLSw default is configured on this interface");
	      return;
	    }
        }

	
	Ban.fTotalDlci = 0;
        /*  Retrieves the DLCI's entered in the fras ban command.  ML  */
    	if (GETOBJ(int, 4)) {
            Ban.fDlci[0].number = GETOBJ(int, 4);
            Ban.fTotalDlci++;
        }
    	if (GETOBJ(int, 5)) {
            Ban.fDlci[1].number = GETOBJ(int, 5);
            Ban.fTotalDlci++; 
	} 
	if (GETOBJ(int, 6)) {
            Ban.fDlci[2].number = GETOBJ(int, 6);
            Ban.fTotalDlci++;
        }
    	if (GETOBJ(int, 7)) {
            Ban.fDlci[3].number = GETOBJ(int, 7);
            Ban.fTotalDlci++;
        }
    	if (GETOBJ(int, 8)) {
            Ban.fDlci[4].number = GETOBJ(int, 8);
            Ban.fTotalDlci++;
        }

        if (BanCmdValid(&Ban)) {
            if ((aBanCmd = DLUBanCmdCreate(&Ban))) {
                DLUAddBanCmd(pcDluPtr, aBanCmd);

                if (csb->which == PCDLU_BAN) {
                    DLUBanEnable(aBanCmd, CLS_RSRB);
                } else 
		  if (csb->interface->hwptr->sdlc_default == NO_SDLC) {

		    /* Searching for only BAN sdb's */

		    for (i = SDLC_MIN_ADDRESS; i <= SDLC_MAX_ADDRESS; i++) {
		      if ((csb->interface->hwptr->sdlc_data[i]) && 
			  (csb->interface->hwptr->sdlc_data[i]->sdlc_have_prtnr) &&
			  (!csb->interface->hwptr->sdlc_data[i]->sdlc_dlsw)) {
			   csb->interface->hwptr->sdlc_data[i]->sdlc_fras_ban = TRUE;
		      } 
		    } 
		    
		    /* BAN sdb's differ from 
		     * BNN sdb's by the partner address.
		     */
		    		    
		    DLUBanEnable(aBanCmd, CLS_SDLC);
		    csb->interface->hwptr->sdlc_default = FRAS_SDLC;

		  } else 
		    if (csb->interface->hwptr->sdlc_default == DLSW_SDLC) {
		      printf("\nFRAS BAN already configured on this interface.");
		      return;
		    }  
	    } else
	      printf("\nFail to create a BAN command memory block!");
        }
        return;
    }
    lsap1 = rsap1 = lsap2 = rsap2 = 0;
    bzero(dstAddr1, IEEEBYTES);
    bzero(dstAddr2, IEEEBYTES);
    portType1 = portType2 = 0;

    switch (GETOBJ(int,1)) {
    case PCDLU_LLC:
       if (is_ethernet(idb1->hwptr) || is_tokenring(idb1->hwptr)) {
            portType1 = CLS_LLC;
	    addrType1 = STATION_IEEE48;
	    ieee_copy(GETOBJ(hwaddr,1)->addr, dstAddr1);
	    lsap1 = GETOBJ(int, 2);
	    rsap1 = GETOBJ(int, 3);
       }
       else {
           printf(" \n Error: Command not valid on this interface");
           return;
       }
       break;

    case PCDLU_SDLC:
        if (!is_sdlc(idb1->hwptr)) {
           printf(" \n Error: Command not valid on non-SDLC interface");
           return;
	}
        if (idb1->hwptr->sdlc_default == DLSW_SDLC) {
	    printf("\nDLSw default is configured on this interface.\n");
	    return;
        }
        portType1 = CLS_SDLC;
	addrType1 = STATION_SDLC;
	lsap1 = dstAddr1[0] = GETOBJ(int, 4);
          if (idb1->hwptr->sdlc_data[lsap1] == NULL) {
            printf(" \n Error: SDLC address %x not configured on this interface.\n", lsap1);
            return;
	  }
        idb1->hwptr->sdlc_data[lsap1]->sdlc_dlsw = FALSE;
        break;
    default:
	dlcSwErrDebug("\nFRAS: Unsupported DLC type");
	return;
    } 
    snpa_init(&hwAddr1, addrType1, TARGET_UNICAST, dstAddr1);

    switch (GETOBJ(int,5)) {
    case PCDLU_LLC:
	portType2 = CLS_LLC;
	addrType2 = STATION_IEEE48;
	ieee_copy((unsigned char*)GETOBJ(hwaddr,2)->addr, dstAddr2);
	lsap2 = GETOBJ(int, 6);
	rsap2 = GETOBJ(int, 7);
	break;

    case PCDLU_FR:
	portType2 = CLS_FRAME_RELAY;
	dlci = GETOBJ(int,9);
	lsap2 = GETOBJ(int, 10);
	rsap2 = GETOBJ(int, 11);
	addrType2 = STATION_FR10;
	dstAddr2[0] = dlci >> 8;
	dstAddr2[1] = dlci & 0xFF;
	break;
    default:
	dlcSwErrDebug("\nFRAS: Unsupported DLC type");
	return;

    } 

    idb2 = GETOBJ(idb, 1);
    snpa_init(&hwAddr2, addrType2, TARGET_UNICAST, dstAddr2);

    aCep1 = DLUCheckCep(idb1, hwAddr1, lsap1, rsap1);

    if (!(csb->sense)) {
        if (aCep1 != NULL) {
	    aCep2 = aCep1->fCepPtr;
	    aCep1->fFlags |= FDLCSW_DELETE;
	    aCep2->fFlags |= FDLCSW_DELETE;
	    if (aCep1->fCepOrigPtr)
                aCep1->fFlags |= FDLCSW_DELETE;
	    if (DLURouteDestroy(aCep1, aCep2))
		return;
	    pcls_fsm(aCep1, NULL, SND_DISC);
	    pcls_fsm(aCep2, NULL, SND_DISC);
        }
	return;
    } else {
        aCep2 = DLUCheckCep(idb2, hwAddr2, lsap2, rsap2);
	if (aCep1 || aCep2) {
	    printf("\nFRAS: The Connection already existed");
	    return;
	}
    }

    if (!(aPort1 = DLUPortInit(pcDluPtr, idb1, portType1, 0, 0, 0))) {
	dlcSwErrDebug("\nFRAS: Port1 Init Failed");
	return;
    }
    if (!(aSap1 = DLUSapInit(aPort1, lsap1, TRUE))) {
	dlcSwErrDebug("\nFRAS: Sap1 Init Failed");
	return;
    }


    if (!(aPort2 = DLUPortInit(pcDluPtr, idb2, portType2, 0, 0, 0))) {
	dlcSwErrDebug("\nFRAS: Port2 Init Failed");
	return;
    }
    if (!(aSap2 = DLUSapInit(aPort2, lsap2, TRUE))) {
	dlcSwErrDebug("\nFRAS: Sap2 Init Failed");
	return;
    }

    if (!(aCep1 = DLUCepInit(aSap1, hwAddr1, lsap1, rsap1, TRUE))) {
	dlcSwErrDebug("\nFRAS: Cep1 Init Failed");
	return;
    }

    if (!(aCep2 = DLUCepInit(aSap2, hwAddr2, lsap2, rsap2, FALSE))) {
	dlcSwErrDebug("\nFRAS: Cep2 Init Failed");
	return;
    }

    aCep1->fFrasType = FRAS_BNN;
    aCep2->fFrasType = FRAS_BNN;
    aCep1->fDlci = 0;
    aCep2->fDlci = 0;
    switch (GETOBJ(int, 12)) {
    case PCDLU_OPTS_PFID2:
	aCep2->fL3Pid = CLS_L3PID_FID2;
	break;
    case PCDLU_OPTS_FID4:
	aCep2->fL3Pid = CLS_L3PID_FID4;
	break;
    case PCDLU_OPTS_AFID2:
    default:
	aCep2->fL3Pid = CLS_L3PID_APPN;
	break;
    }

    if (!(aConn = DLUCvtInit(pcDluPtr, aCep1, aCep2))) {
	dlcSwErrDebug("\nFRAS: Converter Init Failed");
	return;
    }
    DLUSendPortEnable(pcDluPtr, aPort2, NULL, 0, 0, 0);
    DLUSendPortEnable(pcDluPtr, aPort1, NULL, 0, 0, 0);
}


void DLUPortActSap(TCLSIMsg* theMsg)
{
    EnableCnf_t *enableCnf = (EnableCnf_t*) theMsg;
    CLSDLCType_e portType = enableCnf->fDLCType; 
    CLSPortID *portID = &enableCnf->fPortId;
    word retCode;
    TDLUSap* theSap;
    TDLUPort *thePort;

    /*
     * Make sure the port structure gets updated with the portID
     * information returned to us in the Enable.cnf message.
     */
    thePort = (TDLUPort*) enableCnf->fCorrelator;
    retCode = CLSIMsgGetReturnCode(theMsg);
    if (retCode == CLS_OK) {
        thePort->fPortState = DLU_PORT_STATE_ACTIVE;
	if (portType == CLS_RSRB)
    	    CLSIPortIdCopy(portID, &thePort->fPortId);
    }

    theSap = (TDLUSap*) pcDluPtr->fSapList.qhead;
    while (theSap != NULL) {
        if (theSap->fPortPtr == thePort) {
            if (theSap->fSapState == DLU_SAP_STATE_RESET) {
                if (theSap->fDLCType == CLS_RSRB) {
                    CLSIPortIdCopy(portID, &theSap->fPortId);
		} 
                (*theSap->fActivate)((TDLUEntity *)pcDluPtr, (TDLUSap *)theSap);
	    }
	}
        theSap = theSap->fNext;
    } 
}

/* Checks to see if this Cep has the same fId as the in theMsg.
 * If it is then return this Cep.
 */

TDLUCep* DLUGetCep(TCLSIMsg* theMsg)
{
    TDLUCep* theConn;
    
    switch (theMsg->fIdentifierType)
    {
    case CLS_U_CEP_ID:
	for (theConn = (TDLUCep*) pcDluPtr->fCepList.qhead;
	     theConn != NULL;
	     theConn = theConn->fNext)
	{
	    if (theConn->fUCepId == theMsg->fIdentifier)
		break;
	}
	break;
    default:
	theConn = NULL;
	break;
    }
    return(theConn);
}

TDLUCep* DLUFindNewCep(TCLSIMsg* theMsg)
{
    IdStnInd_t* idStn = (IdStnInd_t*) theMsg;
    TDLUSap *aSap = (TDLUSap *) idStn->fCLSIHdr.fIdentifier;
    hwaddrtype DstAddr;
    hwaddrtype SrcAddr;
    TDLUCep *aConn;
    byte *rif, RSAP;

    /* 
     * Get the source mac address from the received XID
     */
    CLSIIdStnGetRemoteMac(idStn, &DstAddr);
    CLSIIdStnGetLocalMac(idStn, &SrcAddr);

    /*
     * Get the endstation's Local Sap from the received XID
     */
    RSAP = (idStn->fRSap);
    buginf("\n RSap = %d", RSAP);

    /*
     * Loop through all the configured CEPs looking to match the mac
     * addresses.
     */
    for (aConn = pcDluPtr->fCepList.qhead;
	 aConn != NULL; 
	 aConn = aConn->fNext) {

	/*
	 * Switch on the address type
	 */
	switch (snpa_get_type(&DstAddr)) {
	case STATION_SDLC:
	    if (snpa_equ(&DstAddr, &aConn->fDstAddr) &&
			(aSap == aConn->fSapPtr))
		    return aConn;
	    break;

	case STATION_IEEE48:
	case STATION_FR10:
            if ((aConn->fFrasType == FRAS_BNN) &&
	        (aConn->fRSap == idStn->fRSap) &&
	        	(aConn->fLSap == idStn->fLSap) &&
	        	snpa_equ(&DstAddr, &aConn->fDstAddr)) {
		rif = CLSIIdStnGetRoute(idStn, &(aConn->fRifLength));
		if (aConn->fRifLength) 
	    	    bcopy((char *)rif, (char *)aConn->fRif, aConn->fRifLength);
	    	return aConn;
	    }
            if (aConn->fFrasType != FRAS_BAN)
                break;
            /* check if it is BAN */
            if (aConn->fSapPtr->fDLCType == CLS_FRAME_RELAY)
                break;

            if ((snpa_equ(&SrcAddr, &aConn->fSrcAddr)) &&
                (aConn->fCepState == LS_RESET)){
		rif = CLSIIdStnGetRoute(idStn, &(aConn->fRifLength));
		if (aConn->fRifLength)
	    	    bcopy((char *)rif, (char *)aConn->fRif, aConn->fRifLength);
                snpa_copy(&DstAddr, &aConn->fDstAddr);
                snpa_copy(&DstAddr, &aConn->fCepPtr->fSrcAddr);
                aConn->fRSap = RSAP;
                aConn->fCepPtr->fLSap = RSAP;
		return aConn;
	    }
	    break;

	default:
	    break;
	}
    }
     
    if (aConn == NULL) {
        dlcSwErrDebug("\nFRAS: No route , lmac %e rmac %e, lSap=0x%x, rSap=0x%x",
		      &DstAddr.addr[0], 
		      &SrcAddr.addr[0], 
		      idStn->fLSap, 
		      idStn->fRSap);
    }

    return(aConn);

}

void DLUFsData(TDLUCep *aCep, TCLSIMsg *theMsg)
{
    dlcSwMsgDebug("FRAS: FS DATA");
    CLSIMsgSetPrimitiveType(theMsg, CLS_REQUEST);
    theMsg->fIdentifierType = CLS_P_CEP_ID;
    theMsg->fIdentifier = aCep->fCepPtr->fPCepId;
    
    CLSIQueuePut(aCep->fDlu->fWriteQ, theMsg);
}

static void displayCep (TDLUCep *aCep)
/*  This function is called when show fras command is executed by the
    user.  This displays the current BNN connections, as described in  
    the column headder in the funcion displayDlci.                         */
{
    char role;
    ushort dlci = 0;
    int state = 0;
    boolean backup = FALSE;

    if (aCep->fFrasType == FRAS_BNN || aCep->fCepPtr->fFrasType == FRAS_BNN) {

        role = ((aCep->fRole == CONN_PRIMARY) ? 'P' : 'S');
        state = (((aCep->fCepState < 0) || (aCep->fCepState > NUM_LS_STATES)) ?
    		     NUM_LS_STATES : aCep->fCepState);
    
        switch (aCep->fSapPtr->fDLCType) {
        case CLS_SDLC:
	printf("\n   sdlc   %12x  %s  na   na   %c    %s ",
    		aCep->fDstAddr.addr[0], 
    		(aCep->fSapPtr->fPortId.fTypeInfo.swidb)->short_namestring,
    		role, ls2aTable[state]);
    	break;
        case CLS_FRAME_RELAY:
            if (aCep->fCepPtr->fCepOrigPtr != NULL) {
                backup = TRUE;
                aCep->fCepPtr->fCepPtr->fFlags |= FDLCSW_SHOW;
            }
    	dlci = (ushort) ((aCep->fDstAddr.addr[0] << 8) | aCep->fDstAddr.addr[1]);
    	printf("\n     fr   -             %s   %2x   %2x  %c    %s %s",
    		(aCep->fSapPtr->fPortId.fTypeInfo.swidb)->short_namestring,
    		aCep->fLSap, aCep->fRSap, role, ls2aTable[state],
                    (backup) ? "(Backup is enabled)" : "");
    	break;
        case CLS_LLC:
    	printf("\n    llc %e  %s %2x     %2x  %c    %s ",
    		aCep->fDstAddr.addr, 
    		(aCep->fSapPtr->fPortId.fTypeInfo.swidb)->short_namestring,
    		aCep->fLSap, aCep->fRSap, role, ls2aTable[state]);
    	break;
        default:
            break;
        }
        aCep->fFlags |= FDLCSW_SHOW;
    }    /* if aCep->fFrasType  */
}

static void displayDlciBni (TDLUCep *aCep)
/*  This function is called when show fras command is executed by the
    user.  This displays the current DLCI number and BNI MAC address
    of the connection, at the top of the each BAN connection display.      */
{
    boolean backup = FALSE;
 
    if (aCep->fFrasType == FRAS_BAN || aCep->fCepPtr->fFrasType == FRAS_BAN) {
 
        if (aCep->fSapPtr->fDLCType == CLS_FRAME_RELAY) {
            if (aCep->fCepPtr->fCepOrigPtr != NULL) {
                backup = TRUE;
                aCep->fCepPtr->fCepPtr->fFlags |= FDLCSW_SHOW;
            }
            if (aCep->fDlci > MIN_DLCI-1 && aCep->fDlci < MAX_DLCI+1)
                printf("\n DLCI:  %d       BNI:  %e", aCep->fDlci, 
                    aCep->fBniAddr);
        printf("\n   Type   Source           Int LSap RSap Role  State");
        }
    }  /*  if aCep->fFrasType  */
    aCep->fFlags |= FDLCSW_SHOW;
}
 
static void displayDlci (TDLUCep *aCep)
/*  This function is called when show fras command is executed by the
    user.  This displays the current DLCI number of the connection, at 
    the top of each BNN connection display.                                */
{
    ushort dlci = 0;
    boolean backup = FALSE;
 
    if (aCep->fFrasType == FRAS_BNN || aCep->fCepPtr->fFrasType == FRAS_BNN) {
        if (aCep->fSapPtr->fDLCType == CLS_FRAME_RELAY) {
            if (aCep->fCepPtr->fCepOrigPtr != NULL) {
                backup = TRUE;
                aCep->fCepPtr->fCepPtr->fFlags |= FDLCSW_SHOW;
            }
    	   dlci = (ushort) ((aCep->fDstAddr.addr[0] << 8) | 
               aCep->fDstAddr.addr[1]);
            if (dlci > MIN_DLCI-1 && dlci < MAX_DLCI+1)
                printf("\n DLCI:  %d", dlci);
        printf("\n   Type   Destination   Int LSap RSap Role  State");
        }
    }  /*  if aCep->fFrasType  */
    aCep->fFlags |= FDLCSW_SHOW;
}

static void displayBanCep (TDLUCep *aCep)
/*  This function is called when show fras command is executed by the
    user.  This displays the current BAN connections, as described in  
    the column headder in the funcion displayDlciBni.                      */
{
    char role;
    int state = 0;
    boolean backup = FALSE;
 
    if (aCep->fFrasType == FRAS_BAN || aCep->fCepPtr->fFrasType == FRAS_BAN) {
 
        role = ((aCep->fRole == CONN_PRIMARY) ? 'P' : 'S');
        state = (((aCep->fCepState < 0) || (aCep->fCepState > NUM_LS_STATES)) ?
                     NUM_LS_STATES : aCep->fCepState);

        if (aCep->fSapPtr->fDLCType == CLS_FRAME_RELAY) {
            if (aCep->fCepPtr->fCepOrigPtr != NULL) {
                backup = TRUE;
                aCep->fCepPtr->fCepPtr->fFlags |= FDLCSW_SHOW;
            }
            printf("\n    fr    %e   %s %2x   %2x   %c     %s %s",
                aCep->fSrcAddr.addr, 
                (aCep->fSapPtr->fPortId.fTypeInfo.swidb)->short_namestring,
                aCep->fLSap, aCep->fRSap, role, 
                ls2aTable[state], (backup) ? "(Backup is enabled)" : "");
	} else if (aCep->fSapPtr->fDLCType == CLS_RSRB) {
            printf("\n   llc    %e   %s %2x   %2x   %c     %s",
                aCep->fSrcAddr.addr,
                (aCep->fSapPtr->fPortId.fTypeInfo.swidb)->short_namestring,
                aCep->fLSap, aCep->fRSap, role,
                ls2aTable[state]);
	} else if (aCep->fSapPtr->fDLCType == CLS_SDLC) {
            printf("\n   sdlc   %e   %s %2x   %2x   %c     %s",
                aCep->fSrcAddr.addr,
                (aCep->fSapPtr->fPortId.fTypeInfo.swidb)->short_namestring,
                aCep->fLSap, aCep->fRSap, role,
                ls2aTable[state]);
	} 
	aCep->fFlags |= FDLCSW_SHOW;
    }  /*  if aCep->fFrasType  */
}
 
static void show_dlcsw_ban_command (parseinfo *csb)
{
    TDLUCep *aCep, *usedCep;
    boolean first=TRUE;
 
    if (!pcdlu_enable)
        return;
 
    aCep = (TDLUCep *) pcDluPtr->fCepList.qhead;
    while ((aCep != NULL) && (aCep->fCepPtr != NULL)) {
        mem_lock(aCep);
        if(first) {
            first = FALSE;
            printf("\n Boundary Access Node (BAN): ");
        }
        if (!(aCep->fFlags & FDLCSW_SHOW)) {
            printf("\n");
            displayDlciBni(aCep);
            displayDlciBni(aCep->fCepPtr);
            displayBanCep(aCep);
            displayBanCep(aCep->fCepPtr); 
	}
        aCep->fFlags &= ~FDLCSW_SHOW;
        usedCep = aCep;
        aCep = aCep->fNext;
        free(usedCep);
    }
}   /* SHOW FRAS BAN COMMAND */

void show_dlcsw_command (parseinfo *csb)
{
    TDLUCep *aCep;

    automore_enable(NULL);
    if (!pcdlu_enable)
        return;

    aCep = (TDLUCep *) pcDluPtr->fCepList.qhead;
    if (aCep != NULL) {
        printf("\n");          
        printf("\n Boundary Network Node (BNN): ");
    }
    while ((aCep != NULL) && (aCep->fCepPtr != NULL)) {
	if (!(aCep->fFlags & FDLCSW_SHOW)) {
            printf("\n");
            displayDlci(aCep);   
            displayDlci(aCep->fCepPtr);   
	    displayCep(aCep);
	    displayCep(aCep->fCepPtr);
	}
	aCep->fFlags &= ~FDLCSW_SHOW;
	aCep = aCep->fNext;
    }
    show_dlcsw_ban_command (csb);
}

boolean DLURouteDestroy(TDLUCep *aCep1, TDLUCep *aCep2)
{
    TDLUCep* thisCep;

    if ((aCep1->fFlags & FDLCSW_DELETE) && 
                (aCep2->fFlags & FDLCSW_DELETE) &&
		(aCep1->fCepState == LS_RESET) && 
		(aCep2->fCepState == LS_RESET)) {

        if (aCep1->fCepOrigPtr)
	    thisCep = aCep1->fCepOrigPtr;
        else if (aCep2->fCepOrigPtr)
	    thisCep = aCep2->fCepOrigPtr;
	else 
	    thisCep = NULL;

	DLURemoveCvt(pcDluPtr, aCep1->fCvtPtr);
	DLUCvtDestroy(aCep1->fCvtPtr);

	DLURemoveCep(pcDluPtr, aCep2);
	DLURemoveCep(pcDluPtr, aCep1);

	if ((aCep1->fSapPtr->fRefCount == 0) &&
	    (aCep1->fSapPtr->fSapState == DLU_SAP_STATE_ACTIVE)) {
	    (*aCep1->fSapPtr->fDeactivate)(pcDluPtr, aCep1->fSapPtr);
        }
	DLUCepDestroy(aCep1);

	if ((aCep2->fSapPtr->fRefCount == 0) &&
	    (aCep2->fSapPtr->fSapState == DLU_SAP_STATE_ACTIVE)) {
	    (*aCep2->fSapPtr->fDeactivate)(pcDluPtr, aCep2->fSapPtr);
        }
	DLUCepDestroy(aCep2);

        if ((thisCep) && (thisCep->fFlags & FDLCSW_DELETE)) {
            DLURemoveCep(pcDluPtr, thisCep);
	    if ((thisCep->fSapPtr->fRefCount == 0) &&
		(thisCep->fSapPtr->fSapState == DLU_SAP_STATE_ACTIVE)) {
	        (*thisCep->fSapPtr->fDeactivate)(pcDluPtr, thisCep->fSapPtr);
	      }
	    DLUCepDestroy(thisCep);
        }

	return TRUE;
    }
    return FALSE;
}

void DLUBanRouteDestroy(TDLUCep *aCep1, TDLUCep *aCep2)
{
    if ((aCep1->fCepState == LS_RESET) &&
        (aCep2->fCepState == LS_RESET)) {
        DLURemoveCvt(pcDluPtr, aCep1->fCvtPtr);
        DLUCvtDestroy(aCep1->fCvtPtr);
        DLURemoveCep(pcDluPtr, aCep2);
        DLURemoveCep(pcDluPtr, aCep1);
        DLUCepDestroy(aCep1);
        DLUCepDestroy(aCep2);
    }
}

TDLUBackup *IsAnyBackup()
{
    return(Backup);
}

uchar GetFrasMaxRetry()
{
    return(FrasMaxRetry);
}

void DLUUpdateAvailDlci(TDLUBan *aBanCmd)
{
    uchar i = 0;

    if (aBanCmd->fDlci[aBanCmd->fAvailDlci].status == DLU_DLCI_INACTIVE) {
        dlcSwErrDebug("\n DLUUpdateAvailDlci - found %d inactive",
                   aBanCmd->fDlci[aBanCmd->fAvailDlci].number);
        for (i = aBanCmd->fAvailDlci; i < aBanCmd->fTotalDlci; i++) {
            if (aBanCmd->fDlci[i].status == DLU_DLCI_ACTIVE) {
                aBanCmd->fAvailDlci = i;
                return;
            }
        }
        for (i = 0; i < aBanCmd->fAvailDlci; i++) {
            if (aBanCmd->fDlci[i].status == DLU_DLCI_ACTIVE) {
                aBanCmd->fAvailDlci = i;
                return;
            }
        }
        /* no dlci is active */
        aBanCmd->fAvailDlci = NO_DLCI;
    }
    return;
}


void DLUUpdateDlciStatus(TDLUCep *aCep, uchar status)
{
    TDLUBan *aBanCmd;
    uchar i;

    aBanCmd = (TDLUBan *)pcDluPtr->fBanCmdList.qhead;
    while (aBanCmd) {
        if (ieee_equal(aBanCmd->fDlciMacAddr, aCep->fSrcAddr.addr))
            break;
        aBanCmd = aBanCmd->fNext;
    }
    if (!aBanCmd) {
        dlcSwErrDebug("FRAS-BAN: DLCI status update failed");
        return;
    }
    for (i = 0; i < aBanCmd->fTotalDlci; i++) {
        if (aBanCmd->fDlci[i].number == aCep->fCepPtr->fDlci) {
            aBanCmd->fDlci[i].status = status;
           
            if (status == DLU_DLCI_INACTIVE) {
                aBanCmd->fDlci[i].session = 0;
            }
            DLUUpdateAvailDlci(aBanCmd);
	}
    }
}


void frDLCIStatusChange_fras(fr_pvc_info_type *fr_pvc)
{
    TDLUBan *aBanCmd;
    TDLUCep *aCep;
    TDLUCep *aConn;
    int i;
    uchar null_address[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    aBanCmd = (TDLUBan *)pcDluPtr->fBanCmdList.qhead;
    while (aBanCmd) {
        for (i = 0; i < aBanCmd->fTotalDlci; i++) {
            if (aBanCmd->fDlci[i].number == fr_pvc->this_dlci) {
                dlcSwErrDebug("\nfrDLCIStatusChange_fras - update %d",
                                 fr_pvc->this_dlci);
                if (DLCI_ACTIVE(fr_pvc->pvc_status))
                    aBanCmd->fDlci[i].status = DLU_DLCI_ACTIVE;
                else
                    aBanCmd->fDlci[i].status = DLU_DLCI_INACTIVE;
                aCep = (TDLUCep *) pcDluPtr->fCepList.qhead;
                if (aBanCmd->fDlci[i].status == DLU_DLCI_ACTIVE) {
                while (aCep != NULL) {
                    if ( (aCep->fFrasType == FRAS_BAN) &&
                         (aCep->fSapPtr->fDLCType == CLS_FRAME_RELAY) &&
                         (!(ieee_equal(null_address, aCep->fSrcAddr.addr)))) {
                            if (aCep->fDlci ==
                               aBanCmd->fDlci[aBanCmd->fAvailDlci].number) {
                               dlcSwErrDebug("\ndlci %d, being used",aCep->fDlci);
                               if (aBanCmd->fAvailDlci<aBanCmd->fTotalDlci-1)
                                   aBanCmd->fAvailDlci++;
                               else
                                   aBanCmd->fAvailDlci = 0;
                               DLUUpdateAvailDlci(aBanCmd);
                               dlcSwErrDebug(", changed to %d",
                                 aBanCmd->fDlci[aBanCmd->fAvailDlci].number);
                               aConn = (TDLUCep *) pcDluPtr->fCepList.qhead;
                               while(aConn != NULL) {
                                 if( (aConn->fCepState == LS_RESET) &&
                                 (aConn->fFrasType == FRAS_BAN) &&
                                 (aConn->fSapPtr->fDLCType!= CLS_FRAME_RELAY)){
                                   aConn->fCepPtr->fDlci =
                                    aBanCmd->fDlci[aBanCmd->fAvailDlci].number;
                                      break;
                                 }
                                 aConn = aConn->fNext;
                               }
                               if ( aConn == NULL )
                                  buginf("did not find BAN cep");
                               break;
			    }
		    }
                    aCep = aCep->fNext;
		} /* end of while */
	        } /* end of DLU_DLCI_ACTIVE */
	    }
        }
        aBanCmd = aBanCmd->fNext;
     }
}

boolean isDlciDown(TDLUCep *aCep, ushort fDlci)
{
    TDLUBan *aBanCmd;
    int i;

    aBanCmd = (TDLUBan *)pcDluPtr->fBanCmdList.qhead;
    while (aBanCmd) {
        if (ieee_equal(aBanCmd->fDlciMacAddr, aCep->fSrcAddr.addr))
            break;
        aBanCmd = aBanCmd->fNext;
    }
    if (!aBanCmd) {
        dlcSwErrDebug("FRAS-BAN: no aBanCmd found");
        return FALSE;
    }
    for (i = 0; i < aBanCmd->fTotalDlci; i++) {
        if (aBanCmd->fDlci[i].number == fDlci) {
            dlcSwErrDebug("\nisDlciDown - got DLCI %d", fDlci);
            if(aBanCmd->fDlci[i].status == DLU_DLCI_INACTIVE) {
                dlcSwErrDebug("- DOWN");
                return TRUE;
	    } else {
                dlcSwErrDebug("- UP");
                return FALSE;
	    }
	}
    }
    return FALSE;
}


/*
 * DLCSW subsystem header
 */

#define DLCSW_MAJVERSION 1
#define DLCSW_MINVERSION 0
#define DLCSW_EDITVERSION 1

SUBSYS_HEADER(dlcsw, DLCSW_MAJVERSION, DLCSW_MINVERSION, DLCSW_EDITVERSION,
		dlcsw_init, SUBSYS_CLASS_PROTOCOL,
        "req: cls, srb, sdlc", NULL);
