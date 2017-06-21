/* $Id: cls_entitymanager.c,v 3.16.6.7 1996/09/11 04:45:10 kchiu Exp $
 * $Source: /release/112/cvs/Xsys/cls/cls_entitymanager.c,v $
 *------------------------------------------------------------------
 * TCLSEntityManager manages global TCLSPort objects in CLS Subsystem.
 *
 * March 1994, Steve Berl
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * A single TCLSEntityManager object exists in the CLS subsystem. It 
 * is responsible for handling enable, and activate_sap requests from
 * the DLUs. It has associated with it a list of TCLSPort objects that
 * represent the interfaces that have been enabled by DLUs.
 *------------------------------------------------------------------
 * $Log: cls_entitymanager.c,v $
 * Revision 3.16.6.7  1996/09/11  04:45:10  kchiu
 * CSCdi68756:  DLSw+/fddi is not working
 * Branch: California_branch
 *
 * Revision 3.16.6.6  1996/09/10  03:13:17  ioakley
 * CSCdi60079:  Correct clsi compliant LNM dlu clean-up problems
 *              that result in LNM clsi related error messages when
 *              "no source-bridge"/"source-bridge" commands are
 *              entered on token ring interfaces.
 * Branch: California_branch
 *
 * Revision 3.16.6.5  1996/08/31  05:20:12  schiang
 * CSCdi67779:  debug ncia client message does not display all packet types
 * Branch: California_branch
 *
 * Revision 3.16.6.4  1996/08/16  07:05:14  ioakley
 * CSCdi63385:  Prevent router crash caused by CLS Connect_Req failure
 *              and resolve LNM conn pending/reporting link issues.
 * Branch: California_branch
 *
 * Revision 3.16.6.3  1996/08/10  01:02:50  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.16.6.2  1996/05/17  10:41:52  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.14.2.10  1996/05/14  18:41:28  schiang
 * CSCdi57622:  ncia start/stop doesnt work
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.9  1996/05/03  22:05:57  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.8  1996/04/26  01:52:35  doner
 * CSCdi55831:  IBU Modularity - CLSI and VDLC converted to new IOS
 * scheduler
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.7  1996/04/24  06:28:03  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.14.2.6  1996/04/12  18:58:23  kpatel
 * CSCdi54229:  Avoid NULL access if can't get pak for connCnf
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.5  1996/04/11  14:35:36  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.4  1996/04/05  06:05:10  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.14.2.3  1996/04/03  13:54:13  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.14.2.2  1996/03/28  08:19:15  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 * Revision 3.16.6.1  1996/03/18  19:09:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.2.4  1996/03/16  06:32:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.11.2.3  1996/03/13  01:14:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.11.2.2  1996/03/07  08:41:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11.2.1  1996/02/20  00:21:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.16  1996/03/06  23:43:13  fbordona
 * CSCdi50868:  Dlsw Interop: Support vendor context cap exchg control
 *              vector. Avoid NULL access if can't get pak for connInd.
 *
 * Revision 3.15  1996/03/05  23:57:01  kpatel
 * CSCdi50720:  SNA session hangs under heavy traffic
 *
 * Revision 3.14  1996/02/26  06:09:05  dmccowan
 * CSCdi49560:  APPN Connection Network over FDDI fails to activate
 * Fill in EnableTRParms_t structure for FDDI.
 * Remove Obsolete EnableFDDIParms_t structure.
 *
 * Revision 3.13  1996/02/21  03:36:32  fbordona
 * CSCdi48915:  DLSw+ send frames out out sequence.
 *              Packets that are not queued on the fDataIndQueue
 *              can pass queued packets.
 *
 * Revision 3.12  1996/02/07  19:06:29  pmorton
 * CSCdi47867:  DLSw : Over QLLC broken
 * Finish backing out the change from CSCdi44928.
 *
 * Revision 3.11  1996/01/22  03:53:30  dmccowan
 * CSCdi45007:  LLC2 LCP negotiated over PPP when not wanted
 *
 * Revision 3.10  1996/01/18  20:37:24  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.14.2.1  1996/03/17  17:36:44  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6.2.5  1996/03/03  00:26:07  ioakley
 * CSCdi50585:  Pick up Phil Morton's Vdlc_Rework and
 *              Mark Regan's Appn over DLSw updates.
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.6.2.4  1996/02/07  23:33:47  ioakley
 * Branch: Cal_Ibm_branch
 * Ensure fClsUSapId field in pb is set-up for IdStn/UDataStn
 * to correct problem of looping/corrupted UI frames for lnm/vdlc.
 *
 * Revision 3.6.2.3  1996/01/16  00:22:01  ioakley
 * Branch: Cal_Ibm_branch
 * Correct "show cls" command "type" and "user class" output errors.
 *
 * Revision 3.6.2.2  1995/12/24  19:13:12  ioakley
 * Branch: Cal_Ibm_branch
 * Check for CLS_VDLC this->fPortPtr->fDLCType in CLSSapUDataStnInd.
 *
 * Revision 3.6.2.1  1995/12/17  09:49:55  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.6  1995/11/23  00:52:47  sberl
 * CSCdi44391:  CLS assertion failure cepstate != pongPending
 * Give pong flows their own field in the cep control block. This resolves
 * conflicts caused by other events changing the state while there is an
 * outstanding pong request.
 *
 * Revision 3.5  1995/11/21  22:56:41  kpatel
 * CSCdi42053:  DLSWC-3-BADCLSIIDTYPE dlsw_is_bad_clsi
 *
 * Revision 3.4  1995/11/20  23:21:19  fbordona
 * CSCdi43362:  DLSW fst transport not working.
 *              Add fReserveLength fields to IdStnInd_t and
 *              ConnectStnInd_t.
 *
 * Revision 3.3  1995/11/17  08:54:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:14:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:10:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.19  1995/11/08  20:52:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.18  1995/10/13  22:24:41  rnaderi
 * CSCdi41309:  router crashes when dlsw configured over frame-relay
 *
 * Revision 2.17  1995/10/11  04:20:04  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.16  1995/09/07  04:38:13  kpatel
 * CSCdi39871:  DLSw+ does not interoperate with remote window size
 *              of one
 *
 * Revision 2.15  1995/08/09  13:04:59  fbordona
 * CSCdi38407:  Test Request to SDLC port not bounced correctly
 *
 * Revision 2.14  1995/07/31  21:29:34  ppearce
 * CSCdi37949:  CLS-3-CLSFAIL when DSPU/SDLC connection becomes active
 *
 * Revision 2.13  1995/07/24  13:01:35  fbordona
 * CSCdi34580:  DLS show circuit displays HALT PENDING NOACK after
 *              successful disc
 *
 * Revision 2.12  1995/07/21  17:07:54  sberl
 * CSCdi37437:  Add new ModifyStn primitive to CLSI
 * Just set the right bits in fModifyFlags and put the new values in the
 * parameter structure.
 *
 * Revision 2.11  1995/07/20  21:13:19  kchiu
 * CSCdi37258:  FRAS Dial-Backup for multiple stations failed
 *
 * Revision 2.10  1995/07/19  20:58:13  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add support for Activate/Deactivate Ring
 *
 * Revision 2.9  1995/07/02  05:15:13  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.8  1995/06/30  23:18:30  sberl
 * CSCdi33897:  CLSI race between ReqOpnstn.cnf and disconnect.ind
 * If the interface goes down while we are in req_opnstn pending state,
 * send the dlu a req_opnstn.cnf instead of a disconnect.ind. Make sure
 * that the cls cep and the dlc cep get cleaned up.
 *
 * Revision 2.7  1995/06/28  18:46:19  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *
 * Revision 2.6  1995/06/21  23:22:53  sberl
 * CSCdi33675:  APPN crashes if DSPU configured for same sap
 * Make sure that error activate_sap.cnf and req_opnstn.cnf have the
 * correct fIdentifier and fIdentifierType values.
 *
 * Revision 2.5  1995/06/20  06:08:52  sberl
 * CSCdi35842:  debug cls error needs more!
 * Add another buginf() to help trap pong problems.
 *
 * Revision 2.4  1995/06/15  22:19:24  sberl
 * CSCdi35842:  debug cls error needs more!
 * Add debug messages. Fix some formatting.
 *
 * Revision 2.3  1995/06/14  18:02:09  ravip
 * CSCdi34577:  dlsw session not established SAPs changed
 *              Handle XIDs to NULL SAP.
 *
 * Revision 2.2  1995/06/09  19:26:51  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.1  1995/06/07  20:19:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "cls_private.h"
#include "ieee.h"
#include "packet.h"

/*
 * Local static constant data
 */
static const byte gEntityManagerClassId[] = {'E','N','T','Y'};
static const byte gPortClassId[] = {'P','O','R','T'};
static const byte gSapClassId[] = {'S','A','P',' '};
static const byte gCepClassId[] = {'C','E','P',' '};


/*
 * And now for the code itself!
 */

TCLSEntityManager* CLSEntityManagerCreate (void)
{
    TCLSEntityManager* this;
    this = malloc(sizeof(TCLSEntityManager));
    if (this != NULL)
    {
	this->fNext = NULL;
	bcopy(gEntityManagerClassId, this->fClassId, sizeof(gEntityManagerClassId));
	queue_init(&this->fPortList, 0);
    }
    return (this);
}

boolean CLSEntityManagerCheck (TCLSEntityManager* this)
{
    int i;

    if (this == NULL)
	return FALSE;
    for (i = 0; i < sizeof(this->fClassId); i++)
    {
	if (this->fClassId[i] != gEntityManagerClassId[i])
	    return(FALSE);
    }
    return(TRUE);
}

void CLSEntityManagerDestroy (TCLSEntityManager* this)
{
    TCLSPort* aPort;
    
    while (this->fPortList.count < 0)
    {
	aPort = p_dequeue(&this->fPortList);
	CLSASSERT_FATAL(CLSPortCheck(aPort));

	CLSPortDestroy(aPort);
    }
    memset(this->fClassId, 0, sizeof(this->fClassId));
    free(this);
}

void CLSEntityManagerHandleMsg (TCLSEntityManager* this, 
				TCLSIMsg* theMsg, 
				TCLSDlu* dlu)
{
/*
    CLSDebugMessage("DLU", "CLS", CLSDluGetDLUName(dlu), theMsg);
*/
    switch (CLSIMsgGetPrimitiveCommand(theMsg))
    {
    case CLS_ENABLE  | CLS_REQUEST:
	CLSEntityManagerEnable(this, (EnableReq_t*) theMsg, dlu);
	break;
    case CLS_DISABLE  | CLS_REQUEST:
	CLSEntityManagerDisable(this, (DisableReq_t*) theMsg, dlu);
	break;
    case CLS_ACTIVATE_SAP  | CLS_REQUEST:
	CLSEntityManagerActivateSap(this, (ActSapReq_t*) theMsg, dlu);
	break;
    case CLS_ACTIVATE_RING:
        CLSEntityManagerActivateRing(this, (ActRngReq_t*) theMsg, dlu);
        break;
    default:			/* Unknown primitive */
	if (cls_debug_err)
	    buginf("\nCLSEntityManagerHandleMsg: unknown primitive 0x%x", 
		   CLSIMsgGetPrimitiveCommand(theMsg));
 	CLSDluSendError(dlu, theMsg, CLS_UNKNOWN_PRIMITIVE,
                              theMsg->fIdentifierType, theMsg->fIdentifier);
	break;
    }
}

void CLSEntityManagerDisable (TCLSEntityManager* this, 
                              DisableReq_t* theMsg, 
                              TCLSDlu* dlu)
{
    TCLSPort* thePort;

    /*
     * Find the port number in the message
     *
     */
    thePort = CLSEntityManagerFindPort(this, theMsg->fDLCType, theMsg->fPortId);

    /* 
     * Send error if port doesn't exist
     *   otherwise disable the port
     *
     */
    if (thePort == NULL) {
	if (cls_debug_err)
	    buginf("\nCLSEntityManagerDisable: unknown port");
 	CLSDluSendError(dlu, (TCLSIMsg*)theMsg, CLS_UNKNOWN_PORT,
           ((TCLSIMsg *)theMsg)->fIdentifierType, ((TCLSIMsg *)theMsg)->fIdentifier);
    } else
      CLSPortDisable(thePort, theMsg, dlu);

} /* End CLSEntityManagerDisable() */

void CLSEntityManagerEnable (TCLSEntityManager* this, 
			     EnableReq_t* theMsg, 
			     TCLSDlu* dlu)
{
    TCLSPort* thePort;

    /* find the port number in the message */
    thePort = CLSEntityManagerFindPort(this, theMsg->fDLCType, theMsg->fPortId);
    
    /* check if the port already exists */
    if (thePort == NULL)
    {
	thePort = CLSPortCreate();	
	if (thePort != NULL)
	    CLSEntityManagerAddPort(this, thePort);
    }
    if (thePort != NULL)
    {
	CLSPortEnable(thePort, theMsg, dlu);
    }
    else
    {
	if (cls_debug_err)
	    buginf("\nCLSEntityManagerEnable: out of memory error");
	CLSIMsgSetPrimitiveType((TCLSIMsg*) theMsg, CLS_CONFIRM);
	CLSIMsgSetReturnCode((TCLSIMsg*) theMsg, CLS_OUT_OF_MEMORY);
	CLSDluAddPendingMsg(dlu, (TCLSIMsg*) theMsg);
    }
}

void CLSEntityManagerActivateSap (TCLSEntityManager* this, ActSapReq_t* theMsg, TCLSDlu* dlu)
{
    TCLSPort* thePort;
    thePort = CLSEntityManagerFindPort(this, theMsg->fDLCType, theMsg->fPortId);
    
    if (thePort != NULL)		/* check if the port already exists */
    {
	CLSPortActSap(thePort, theMsg, dlu);
    }
    else
    {
	if (cls_debug_err)
	    buginf("\nCLSEntityManagerActivateSap: unknown port");
	CLSIMsgSetPrimitiveType((TCLSIMsg*) theMsg, CLS_CONFIRM);
	CLSIMsgSetReturnCode((TCLSIMsg*) theMsg, CLS_UNKNOWN_PORT);
	CLSIMsgSetDestination((TCLSIMsg*) theMsg, CLS_U_SAP_ID, theMsg->fCLSIHdr.fUSapId);
	CLSDluAddPendingMsg(dlu, (TCLSIMsg*) theMsg);
    }
}    

void CLSEntityManagerActivateRing (TCLSEntityManager* this, ActRngReq_t* theMsg,
 TCLSDlu* dlu)
{
    TCLSPort* thePort;
    CLSASSERT_FATAL(CLSDluCheck(dlu));
    thePort = CLSEntityManagerFindPort(this, theMsg->fDLCType, theMsg->fPortId);

    if (thePort != NULL)                /* check if the port already exists */
    {
        CLSPortActRing(thePort, theMsg, dlu);
    }
    else
    {
	if (cls_debug_err)
	    buginf("\nCLSEntityManagerActivateRing: unknown port");
	CLSIMsgSetPrimitiveType((TCLSIMsg*) theMsg, CLS_CONFIRM);
	CLSIMsgSetReturnCode((TCLSIMsg*) theMsg, CLS_UNKNOWN_PORT);
	CLSDluAddPendingMsg(dlu, (TCLSIMsg*) theMsg);
    }
}

void CLSEntityManagerHandleDLCMsg (TCLSEntityManager* this, ClsOpcodeT
				  opCode, ClsDlcParamBlockT* pb)
{
    switch (opCode)
    {
    case CLS_ENABLE_CNF:
	CLSEntityManagerEnableCnf(this, pb);
	break;
    case CLS_DISABLE_CNF:
	CLSEntityManagerDisableCnf(this, pb);
	break;
    case CLS_ACTIVATE_SAP_CNF:
	CLSEntityManagerActSapCnf(this, pb);
	break;
    case CLS_ID_IND:
	CLSEntityManagerIdInd(this, pb);
	break;
    case CLS_CONNECT_IND:		/* Unsolicited SABME */
	CLSASSERT(FALSE);
	break;
    case CLS_DEACTIVATE_SAP_CNF:
	CLSEntityManagerDeactSapCnf(this, pb);
	break;
    case CLS_ID_STN_IND:
        CLSEntityManagerIdInd(this, pb);
        break;
    case CLS_TEST_STN_IND:
        CLSEntityManagerTestInd(this, pb);
        break;
    case CLS_CONNECT_STN_IND:
        CLSEntityManagerConnectInd(this, pb);
        break;
    case CLS_UDATA_STN_IND:
        CLSEntityManagerUDataInd(this, pb);
        break;
    case CLS_DEACTIVATE_RING_CNF:
        CLSEntityManagerDeactRngCnf(this, pb);
        break;
    case CLS_ACTIVATE_RING_CNF:
        CLSEntityManagerActRngCnf(this, pb);
        break;
    case CLS_DISC_IND:                  /* Ignore Disc.Ind for now */
        break;        
    default:				/* Unknown primitive code */
	if (cls_debug_err)
	    buginf("\nCLSEntityManagerHandleDLCMsg: unknown primitive 0x%x", opCode);
	CLSASSERT(FALSE);
	break;
    }
}

void CLSEntityManagerEnableCnf (TCLSEntityManager* this,
			       ClsDlcParamBlockT* pb)
{
    TCLSPort* thePort;
    CLSPortID tmpPort;

    CLSIPortIdCopy((CLSPortID*)&pb->fParameters.fDlcEnable.fPortId, &tmpPort);
    thePort = CLSEntityManagerFindPort(this, 
                                       pb->fParameters.fDlcEnable.fDLCType,
                                       tmpPort);
    CLSASSERT_FATAL(thePort != NULL);

    CLSPortEnableCnf(thePort, pb);
}

void CLSEntityManagerDisableCnf (TCLSEntityManager* this,
                                 ClsDlcParamBlockT* pb)
{
    TCLSPort* thePort;
    CLSPortID tmpPort;

    CLSIPortIdCopy((CLSPortID*)&pb->fParameters.fDlcDisable.fPortId, &tmpPort);
    thePort = CLSEntityManagerFindPort(this, 
                                       pb->fParameters.fDlcDisable.fDLCType,
                                       tmpPort);

    CLSASSERT_FATAL(thePort != NULL);

    CLSPortDisableCnf(thePort, pb);

} /* End CLSEntityManagerDisableCnf() */


void CLSEntityManagerActSapCnf (TCLSEntityManager* this, ClsDlcParamBlockT* pb)
{
    TCLSPort* thePort;
    thePort = CLSEntityManagerFindPortByPPortId(this, pb->fParameters.fDlcActSap.fDLCType, pb->fParameters.fDlcActSap.fPPortId);


    CLSASSERT_FATAL(thePort != NULL);

    CLSPortActSapCnf(thePort, pb);
}

void CLSEntityManagerDeactSapCnf (TCLSEntityManager* this, ClsDlcParamBlockT* pb)
{
    TCLSPort* thePort;
    thePort = CLSEntityManagerFindPortByPPortId(this, pb->fParameters.fDlcActSap.fDLCType, pb->fParameters.fDlcActSap.fPPortId);


    CLSASSERT_FATAL(thePort != NULL);

    CLSPortDeactSapCnf(thePort, pb);
}
void CLSEntityManagerActRngCnf (TCLSEntityManager* this, ClsDlcParamBlockT* pb)
{
    TCLSPort* thePort;
    thePort = CLSEntityManagerFindPortByPPortId(this, pb->fParameters.fDlcActRng.fDLCType, pb->fParameters.fDlcActRng.fPPortId);

    CLSASSERT_FATAL(thePort != NULL);

    CLSPortActRngCnf(thePort, pb);
}
void CLSEntityManagerDeactRngCnf (TCLSEntityManager* this, ClsDlcParamBlockT* pb
)
{
    TCLSPort* thePort;
    thePort = CLSEntityManagerFindPortByPPortId(this, pb->fParameters.fDlcActRng.fDLCType, pb->fParameters.fDlcActRng.fPPortId);

    CLSASSERT_FATAL(thePort != NULL);

    CLSPortDeactRngCnf(thePort, pb);
}


void CLSEntityManagerIdInd (TCLSEntityManager* this, ClsDlcParamBlockT* pb)
{
    TCLSSap* theSap;
    
    theSap = (TCLSSap*) pb->fParameters.fDlcIdStn.fClsUSapId;

    CLSASSERT_FATAL(CLSSapCheck(theSap));
    
    CLSSapIdStnInd(theSap, pb);
}

void CLSEntityManagerTestInd (TCLSEntityManager* this, ClsDlcParamBlockT* pb)
{
    TCLSSap* theSap;

    theSap = (TCLSSap*) pb->fParameters.fDlcTestStn.fClsUSapId;

    CLSASSERT_FATAL(CLSSapCheck(theSap));

    CLSSapTestStnInd(theSap, pb);
}
void CLSEntityManagerConnectInd (TCLSEntityManager* this, ClsDlcParamBlockT* pb)
{
    TCLSSap* theSap;

    theSap = (TCLSSap*) pb->fParameters.fDlcConnectStn.fClsUSapId;

    CLSASSERT_FATAL(CLSSapCheck(theSap));

    CLSSapConnectStnInd(theSap, pb);
}



void CLSEntityManagerUDataInd (TCLSEntityManager* this, ClsDlcParamBlockT* pb)
{
    TCLSSap* theSap;

    theSap = (TCLSSap*) pb->fParameters.fDlcUdataStn.fClsUSapId;

    CLSASSERT_FATAL(CLSSapCheck(theSap));

    CLSSapUDataStnInd(theSap, pb);
}

void CLSEntityManagerAddPort (TCLSEntityManager* this, TCLSPort*
			     thePort)
{
    CLSASSERT_FATAL(CLSPortCheck(thePort));
    p_enqueue(&this->fPortList, thePort);
}

TCLSPort* CLSEntityManagerFindPortByPPortId (TCLSEntityManager* this,
                                             CLSDLCType_e dlcType,
					     DlcPPortIdT portId)
{
    TCLSPort* thePort = (TCLSPort*) this->fPortList.qhead;
    while (thePort != NULL)
    {
        if ((thePort->fDLCPortId == portId) && (thePort->fDLCType == dlcType))
	{
	    break;
	}
	thePort = thePort->fNext;
    }
    return(thePort);
}
    
TCLSPort* CLSEntityManagerFindPort (TCLSEntityManager* this, CLSDLCType_e
				   dlcType, CLSPortID portID)
{
    TCLSPort* thePort = (TCLSPort*) this->fPortList.qhead;
    while (thePort != NULL)
    {
       if ((thePort->fDLCType == dlcType) &&
          CLSIPortIdEqual(&thePort->fPortId, &portID))

	{
	    break;
	}
	thePort = thePort->fNext;
    }
    return(thePort);
}

TCLSPort* CLSPortCreate (void)
{
    TCLSPort* this = malloc(sizeof(TCLSPort));
    if (this != NULL)
    {
	bcopy(gPortClassId, this->fClassId, sizeof(gPortClassId));
	this->fNext = NULL;
	this->fPortState = kPortStateInit;
        this->fPortId.type = CLS_P_TYPE_ILLEGAL;
        this->fDLCType = 0;
    }
    return (this);
}


boolean CLSPortCheck (TCLSPort* this)
{
    int i;

    if (this == NULL)
	return FALSE;
    for (i = 0; i < sizeof(gPortClassId); i++)
    {
	if (this->fClassId[i] != gPortClassId[i])
	    return(FALSE);
    }
    return(TRUE);
}

void CLSPortDestroy (TCLSPort* this)
{
    free(this);
}

void CLSPortEnable (TCLSPort* this, EnableReq_t* theMsg, TCLSDlu* dlu)
{
    ClsDlcParamBlockT pb;
    DlcEnableT* enablePB;
    
    size_t parmSize;
    
    this->fPortId = theMsg->fPortId;
    this->fDLCType = theMsg->fDLCType;
    
    /* Save the port type specific parameter structure */
    /* Size is datasize less the fixed fields in the structure */
    
    parmSize = theMsg->fCLSIHdr.fDataSize -
	(offsetof(EnableReq_t, fParms) - sizeof(EnableReqh_t));
    
    this->fPortState = kPortStateEnablePending;
    this->fRequest = (TCLSIMsg*) theMsg; /* hang on to the request */
    this->fDluPend = dlu;		/* and where it came from */
					/* so we can respond later */
    
    enablePB = &pb.fParameters.fDlcEnable;
    
    enablePB->fDLCType = this->fDLCType; /* generic fields of DlcEnableT */
    CLSIPortIdCopy(&this->fPortId, (CLSPortID*)&enablePB->fPortId);

    /* Switch on the dlcType to do media specific things */
    
    switch (this->fDLCType)
    {
    case CLS_TBRIDGE:
    {
	/* TBRIDGEEnableT is native (tongue) to dlc's and is in dlc.h */
	/* EnableTBRIDGEParms_t is native (tongue) to cls and is in clsi_msg.h */
	TBRIDGEEnableT *tbridgeDlcParms =
	    &pb.fParameters.fDlcEnable.fMediaEnable.fTBRIDGEEnable;
	EnableTBRIDGEParms_t *tbridgeClsParms =
	    (EnableTBRIDGEParms_t*) &(theMsg->fParms);

	tbridgeDlcParms->fVersion         = tbridgeClsParms->fVersion;

	snpa_copy(&tbridgeClsParms->fVMacAddr,&tbridgeDlcParms->fVMacAddr);
	tbridgeDlcParms->fTbridgeOptionFlags =
	    tbridgeClsParms->fTbridgeOptionFlags;
	tbridgeDlcParms->fMaxIFieldLength = tbridgeClsParms->fMaxIFieldLength;
	tbridgeDlcParms->fOqueueFn = tbridgeClsParms->fOqueueFn;
	tbridgeDlcParms->fSpanIndex = tbridgeClsParms->fSpanIndex;
    }
	break;

    case CLS_RSRB:
    {
	/* RSRBEnableT is native (tongue) to dlc's and is in dlc.h */
	/* EnableRSRBParms_t is native (tongue) to cls and is in clsi_msg.h */
	RSRBEnableT *rsrbDlcParms = 
	    &pb.fParameters.fDlcEnable.fMediaEnable.fRSRBEnable;
	EnableRSRBParms_t *rsrbClsParms =
	    (EnableRSRBParms_t*) &(theMsg->fParms);

	rsrbDlcParms->fVersion         = rsrbClsParms->fVersion;

	snpa_copy(&rsrbClsParms->fVMacAddr,&rsrbDlcParms->fVMacAddr);

	rsrbDlcParms->fRsrbOptionFlags = rsrbClsParms->fRsrbOptionFlags;
	rsrbDlcParms->fMaxIFieldLength = rsrbClsParms->fMaxIFieldLength;
	rsrbDlcParms->fLocalRing       = rsrbClsParms->fLocalRing;
	rsrbDlcParms->fBridgeNumber    = rsrbClsParms->fBridgeNumber;
	rsrbDlcParms->fTargetRing      = rsrbClsParms->fTargetRing;
    }
	break;

    case CLS_SDLC:
    {
	SDLCEnableT *sdlcDlcParms = 
	    &pb.fParameters.fDlcEnable.fMediaEnable.fSDLCEnable;
	EnableSDLCParms_t *sdlcClsParms =
	    (EnableSDLCParms_t*) &(theMsg->fParms);

	sdlcDlcParms->fVersion         = sdlcClsParms->fVersion;
    }
	break;

    case CLS_QLLC:
    {
	QLLCEnableT *qllcDlcParms = 
	    &pb.fParameters.fDlcEnable.fMediaEnable.fQLLCEnable;
	EnableQLLCParms_t *qllcClsParms =
	    (EnableQLLCParms_t*) &(theMsg->fParms);

	qllcDlcParms->fVersion         = qllcClsParms->fVersion;
    }
	break;
	
    case CLS_LLC:		/* code is broken, assumes LLC == TKR! */
    {
	TREnableT *trDlcParms =
	    &pb.fParameters.fDlcEnable.fMediaEnable.fTREnable;
	EnableTRParms_t *trClsParms =
	    (EnableTRParms_t*) &(theMsg->fParms);

	trDlcParms->fVersion         = trClsParms->fVersion;

	snpa_copy(&trClsParms->fLocalMac, &trDlcParms->fLocalMac);

	trDlcParms-> fTrSpeed = trClsParms->fTrSpeed;
	trDlcParms->fMaxIFieldLength = trClsParms->fMaxIFieldLength;
    }
	break;
	
    case CLS_FRAME_RELAY:
    case CLS_ATM_1483:
    case CLS_PPP:
	break;

    case CLS_VDLC:
    {
	VDLCEnableT *vdlcDlcParms = 
	    &pb.fParameters.fDlcEnable.fMediaEnable.fVDLCEnable;
	EnableVDLCParms_t *vdlcClsParms =
	    (EnableVDLCParms_t*) &(theMsg->fParms);

	vdlcDlcParms->fVersion         = vdlcClsParms->fVersion;

	vdlcDlcParms->fRingGroup = vdlcClsParms->fRingGroup;
	snpa_copy(&vdlcClsParms->fVMacAddr,&vdlcDlcParms->fVMacAddr);

	vdlcDlcParms->fVdlcOptionFlags = vdlcClsParms->fVdlcOptionFlags;
	vdlcDlcParms->fMaxIFieldLength = vdlcClsParms->fMaxIFieldLength;
    }
        break;
	
    case CLS_NCIA:
    {
        NCIAEnableT *nciaDlcParms =
            &pb.fParameters.fDlcEnable.fMediaEnable.fNCIAEnable;
        EnableNCIAParms_t *nciaClsParms =
            (EnableNCIAParms_t*) &(theMsg->fParms);
 
        nciaDlcParms->fVersion         = nciaClsParms->fVersion;
        nciaDlcParms->fIpAddr = nciaClsParms->fIpAddr;
        ieee_copy(nciaClsParms->fServerMac, nciaDlcParms->fServerMac);
        ieee_copy(nciaClsParms->fVMacAddr, nciaDlcParms->fVMacAddr);
        nciaDlcParms->fVMacRange = nciaClsParms->fVMacRange;
        nciaDlcParms->fServerNum = nciaClsParms->fServerNum;
    }
        break;

    default:				/* Unknown port type */
        if (cls_debug_err)
	    buginf("\nCLSPortEnable: unknown dlc type 0x%x", this->fDLCType);
	CLSIMsgSetPrimitiveType((TCLSIMsg*) theMsg, CLS_CONFIRM);
	CLSIMsgSetReturnCode((TCLSIMsg*) theMsg, CLS_UNKNOWN_DLC_TYPE);
	CLSDluAddPendingMsg(dlu, (TCLSIMsg*) theMsg);
	this->fPortState = kPortStateInit;
	this->fRequest = NULL;
	this->fDluPend = NULL;
	return;
	break;
    } /* endSwitch(this->fDLCType) */

    reg_invoke_dlc_wput(this->fDLCType, CLS_NO_CEP, DLC_ENABLE_REQ, &pb);
}

void CLSPortEnableCnf (TCLSPort* this, ClsDlcParamBlockT* pb)
{
    word     correlator = 0;
    idbtype* swidb = pb->fParameters.fDlcEnable.fPPortId;

    EnableReq_t* enableReq = (EnableReq_t*) this->fRequest;
    EnableCnf_t* enableCnf;

    CLSASSERT_FATAL(enableReq != NULL);
    CLSASSERT(this->fPortState == kPortStateEnablePending);

    correlator = enableReq->fCorrelator;
    
    enableCnf =  (EnableCnf_t*) enableReq;
    CLSIMsgSetPrimitiveType((TCLSIMsg*) enableCnf, CLS_CONFIRM);
    if (cls_debug_err && (pb->fResultError != CLS_OK))
	buginf("\nCLSPortEnableCnf: error %d", pb->fResultError);
    CLSIMsgSetReturnCode((TCLSIMsg*) enableCnf, pb->fResultError);

    /*
     * Return swidb in Enable.Cnf for use by DLU
     *
     */
    enableCnf->fPortId.type = CLS_P_TYPE_SWIDB;
    enableCnf->fPortId.fTypeInfo.swidb = swidb;

    enableCnf->fCorrelator = correlator;

    if (pb->fResultError == CLS_OK)
    {
	this->fPortState = kPortStateEnabled;
	this->fDLCPortId = pb->fParameters.fDlcEnable.fPPortId;

        /*
         *   Save idb in port structure
         *
         * Once usage of CLS_P_TYPE_NUMBERS is removed from all DLUs
         *   the "if" statement below may be removed
	 *
	 *         if (this->fPortId.type == CLS_P_TYPE_SWIDB)
         *
	 * DONE 9/15/95 PHIL
	 */

	this->fPortId.fTypeInfo.swidb = swidb;

	switch (this->fDLCType)
	{
	case CLS_LLC:	/* Add EnableTRParmT based on returned values */
	{
          if (CLSPortIsTokenRing(this) ||
              CLSPortIsFddi(this) ||
              CLSPortIsEthernet(this))
          {
	    /* Save returned parameters in this port object */
	    TREnableT* trEnable = &pb->fParameters.fDlcEnable.fMediaEnable.fTREnable;
		
	    this->fParms.fTRParms.fVersion = trEnable->fVersion;
	    snpa_copy(&trEnable->fLocalMac, &this->fParms.fTRParms.fLocalMac);
	    this->fParms.fTRParms.fTrSpeed = trEnable->fTrSpeed;
	    this->fParms.fTRParms.fMaxIFieldLength = trEnable->fMaxIFieldLength;

	    /* Copy returned values to confirm message */
	    bcopy(&this->fParms.fTRParms, enableCnf->fParms, sizeof(EnableTRParms_t));
	  }
	}
	    break;
	    /* someday in the future[>1/6/95], virtual interfaces will
	     * * return *portID in this primitive.  The portID will be
	     * a * *swidb of the vidb created in the enable. [ccartee]
	     * waiting for DLSw migration from port_num to swidb...
	     * Until that time, DLSw stores a *swidb in the lpeer
	     * structure.
	     */
	case CLS_RSRB:
	case CLS_TBRIDGE:
	    break;

        case CLS_SDLC:
        case CLS_QLLC:
        case CLS_FRAME_RELAY:
        case CLS_ATM_1483:
        case CLS_PPP:
        case CLS_VDLC:
        case CLS_NCIA:
            break;
	default:		/* Unknown port type */
	    if (cls_debug_err)
		buginf("\nCLSPortEnableCnf: unknown port type 0x%x", this->fDLCType);
	    CLSASSERT(FALSE);
	    break;		/* Should never happen */
	}
    }
    else
    {
	this->fPortState = kPortStateInit;
    }
    CLSDluAddPendingMsg(this->fDluPend, (TCLSIMsg*) enableCnf);
    return;
}

void CLSPortDisable (TCLSPort* this, DisableReq_t* theMsg, TCLSDlu* dlu)
{

    ClsDlcParamBlockT pb;
    DlcDisableT* disablePB;

    this->fPortId = theMsg->fPortId;
    this->fDLCType = theMsg->fDLCType;
 
    this->fPortState = kPortStateDisablePending;
    this->fRequest = (TCLSIMsg*) theMsg; /* hang on to the request */
    this->fDluPend = dlu;                /* and where it came from */
                                         /* so we can respond later */

    disablePB = &pb.fParameters.fDlcDisable;
    disablePB->fDLCType = this->fDLCType; 
    CLSIPortIdCopy(&this->fPortId, (CLSPortID*)&disablePB->fPortId);

    reg_invoke_dlc_wput(this->fDLCType, CLS_NO_CEP, DLC_DISABLE_REQ, &pb);

} /* End CLSPortDisable() */

void CLSPortDisableCnf (TCLSPort* this, ClsDlcParamBlockT* pb)
{
    DisableReq_t* disableReq = (DisableReq_t*) this->fRequest;
    DisableCnf_t* disableCnf;
    
    CLSASSERT_FATAL(disableReq != NULL);
    CLSASSERT(this->fPortState == kPortStateDisablePending);

    disableCnf =  (DisableCnf_t*) disableReq;
    CLSIMsgSetPrimitiveType((TCLSIMsg*) disableCnf, CLS_CONFIRM);
    if (cls_debug_err && (pb->fResultError != CLS_OK))
	buginf("CLSPortDisableCnf: error %d", pb->fResultError);
    CLSIMsgSetReturnCode((TCLSIMsg*) disableCnf, pb->fResultError);

    this->fPortState = kPortStateDisabled;

    CLSDluAddPendingMsg(this->fDluPend, (TCLSIMsg*) disableCnf);

} /* End CLSPortDisableCnf() */

static void ClsPortActQSapGuts (ActSapQLLCParms_t *clsiQLLCParms, ClsDlcParamBlockT *pb)
{
    QLLCActSapT		*qActSap;
	    
    qActSap = &pb->fParameters.fDlcActSap.fDlcSap.fQLLCActSap;
	    
    /* structure copy */
    memcpy(&qActSap->fQActSap, &clsiQLLCParms->fQActSap, sizeof(QSapT));

    if (QTestVmac( &clsiQLLCParms->fQActSap.fQSapFlags)) {
	ieee_copy( &clsiQLLCParms->fVmacStart[0], 
		  &qActSap->fVmacStart[0]);
	qActSap->fNumVmacs = clsiQLLCParms->fNumVmacs;
    }
}

void CLSPortActSap (TCLSPort* this, ActSapReq_t* theMsg, TCLSDlu* dlu)
{
    TCLSSap* theSap;
    ClsDlcParamBlockT pb;

    CLSASSERT_FATAL(CLSDluCheck(dlu));
    theSap = CLSSapCreate(this, dlu);
    if (theSap != NULL) {
    
    	/* set up common parameters */
    
	
	this->fPortState = kPortStateActSapPending;
	this->fRequest = (TCLSIMsg*) theMsg; /* hang on to the request */
	this->fDluPend = dlu;		/* and where it came from */
					/* so we can respond later */	

	CLSIPortIdCopy(&this->fPortId, (CLSPortID*)&pb.fPortId);
        pb.fParameters.fDlcActSap.fDLCType = this->fDLCType;
	pb.fParameters.fDlcActSap.fPPortId = this->fDLCPortId;
	pb.fParameters.fDlcActSap.fUSapId = (ClsUSapIdT)theSap->fPSapId;

	/* DLC specific part */
	
	switch(this->fDLCType) {

	case CLS_TBRIDGE:    /* intentional fall through, behaves as CLS_LLC */
	case CLS_RSRB:
	case CLS_FRAME_RELAY:
	case CLS_ATM_1483:
        case CLS_PPP:
	case CLS_LLC: 
	    {
	    ActSapLLCParms_t* clsiLLCParms = 
		(ActSapLLCParms_t*) theMsg->fParms;

	    if (clsiLLCParms->fSapValue == 0)
		{
		    /* 
		     * That's a no-no - Sap Value on Act Sap may not be 0 
		     * Fail and bail
		     */
		    CLSSapDestroy(theSap);
		    CLSIMsgSetPrimitiveType((TCLSIMsg*) theMsg, CLS_CONFIRM);
		    CLSIMsgSetReturnCode((TCLSIMsg*) theMsg, CLS_UNKNOWN_SAP);
		    CLSIMsgSetDestination((TCLSIMsg*) theMsg, CLS_U_SAP_ID, 
					  theMsg->fCLSIHdr.fUSapId);
		    CLSDluAddPendingMsg(dlu, (TCLSIMsg*) theMsg);
		    this->fPortState = kPortStateEnabled;
		    return;
		}
	    pb.fParameters.fDlcActSap.fDlcSap.fLlcActSap.fUserClass =
		clsiLLCParms->fUserClass;
	    pb.fParameters.fDlcActSap.fDlcSap.fLlcActSap.fSapFlags =
		(SAP_TEST_AUTORESPONSE     |
		 SAP_TEST_FORWARD          |
		 SAP_IEEE_XID_AUTORESPONSE |
		 SAP_SNA_XID_FORWARD); 
	    
	    pb.fParameters.fDlcActSap.fDlcSap.fLlcActSap.fLSapValue =
		clsiLLCParms->fSapValue;
	    }
	    break;
	    
	case CLS_SDLC:	    
	    {
	    ActSapSDLCParms_t* clsiSDLCParms =
		(ActSapSDLCParms_t*)theMsg->fParms;
	    
	    pb.fParameters.fDlcActSap.fDlcSap.fSdlcActSap.fAddrType =
		clsiSDLCParms->fAddrType;
	    pb.fParameters.fDlcActSap.fDlcSap.fSdlcActSap.fLSapValue =
		clsiSDLCParms->fSapValue;
	    
	    }
	    break;

	case CLS_QLLC:		/* transfer qllc-spceific parameters from the
				 * CLSI Message to the Cls-Dlc parameter block
				 */

		ClsPortActQSapGuts((ActSapQLLCParms_t *)theMsg->fParms, &pb);
		break;

        case CLS_VDLC:
	    {
	    ActSapVDLCParms_t* clsiVDLCParms = 
		(ActSapVDLCParms_t*) theMsg->fParms;

	    pb.fParameters.fDlcActSap.fDlcSap.fVdlcActSap.fUserClass =
		clsiVDLCParms->fUserClass;
	    pb.fParameters.fDlcActSap.fDlcSap.fVdlcActSap.fSapFlags =
		SAP_TEST_AUTORESPONSE;     
	    
	    pb.fParameters.fDlcActSap.fDlcSap.fVdlcActSap.fLSapValue =
		clsiVDLCParms->fSapValue;
            }
	    break;

        case CLS_NCIA:
        {
            ActSapNCIAParms_t* clsiNciaParms =
                (ActSapNCIAParms_t*) theMsg->fParms;
 
            pb.fParameters.fDlcActSap.fDlcSap.fNciaActSap.fLSapValue =
                clsiNciaParms->fSapValue;
            pb.fParameters.fDlcActSap.fDlcSap.fNciaActSap.fUserClass =
                clsiNciaParms->fUserClass;
        }
            break;

	default:
	    CLSASSERT(FALSE);
	    return;
	}
	reg_invoke_dlc_wput(this->fDLCType, CLS_NO_CEP, 
			    DLC_ACTIVATE_SAP_REQ, &pb);
    } else {
	CLSIMsgSetPrimitiveType((TCLSIMsg*) theMsg, CLS_CONFIRM);
	CLSIMsgSetReturnCode((TCLSIMsg*) theMsg, CLS_OUT_OF_MEMORY);
	CLSIMsgSetDestination((TCLSIMsg*) theMsg, CLS_U_SAP_ID, theMsg->fCLSIHdr.fUSapId);
	CLSDluAddPendingMsg(dlu, (TCLSIMsg*) theMsg);
    }
    
}

void CLSPortActSapCnf (TCLSPort* this, ClsDlcParamBlockT* pb)
{
    TCLSSap* theSap;
    TCLSIMsg* theMsg;
    
    CLSASSERT(this->fPortState == kPortStateActSapPending);
    
    this->fPortState = kPortStateEnabled;
    theMsg = this->fRequest;

    CLSASSERT(theMsg != NULL);
    
    theSap = (TCLSSap*) pb->fParameters.fDlcActSap.fUSapId;
    theSap->fDlcPSapId = pb->fParameters.fDlcActSap.fDlcPSapId;
    if (pb->fResultError == CLS_OK)
    {
	CLSDluAddSap(this->fDluPend, theSap);
	CLSSapActSap(theSap, (ActSapReq_t*) theMsg); 
					/* Get config info from the msg */
    }
    else
    {
	CLSSapDestroy(theSap);
	if (cls_debug_err)
	    buginf("\nCLSPortActSapCnf: error %d", pb->fResultError);
	CLSIMsgSetDestination(theMsg, CLS_U_SAP_ID, ((ActSapReq_t*)theMsg)->fCLSIHdr.fUSapId);
	CLSIMsgSetPrimitiveType(theMsg, CLS_CONFIRM);
	CLSIMsgSetReturnCode(theMsg, pb->fResultError);
	CLSDluAddPendingMsg(this->fDluPend, (TCLSIMsg*) theMsg);
    }
}

void CLSPortActRing (TCLSPort* this, ActRngReq_t* theMsg, TCLSDlu* dlu)
{
    TCLSSap* theSap;
    ClsDlcParamBlockT pb;

    CLSASSERT_FATAL(CLSDluCheck(dlu));
    theSap = CLSSapCreate(this, dlu);
    if (theSap != NULL)
    {
	theSap->fParms.fLLCParms.fSapValue = 0;
	/*
	 * We use the value 0 to identify * a bridging SAP 
	 * The value 0 is off limits for Activate Sap
	 */

	this->fPortState = kPortStateActSapPending;
	this->fRequest = (TCLSIMsg*) theMsg; /* hang on to the request*/
	this->fDluPend = dlu;		/* and where it came from 
					 * so we can respond later
					 */

        pb.fParameters.fDlcActRng.fDLCType = this->fDLCType;
	pb.fParameters.fDlcActRng.fPPortId = this->fDLCPortId;
	pb.fParameters.fDlcActRng.fClsUSapId = (ClsUSapIdT) theSap->fPSapId;
	CLSIPortIdCopy(&this->fPortId, (CLSPortID*)&pb.fPortId);

        switch(this->fDLCType)
        {
	case CLS_TBRIDGE:    /* intentional fall through, behaves as CLS_LLC */
        case CLS_LLC:
	case CLS_SDLC:
        case CLS_FRAME_RELAY:
	case CLS_ATM_1483:
        case CLS_PPP:
        case CLS_RSRB:
	    pb.fParameters.fDlcActRng.fDlcRng.fLlcActRng.fSapFlags = 0;
            break;

        case CLS_VDLC:
            pb.fParameters.fDlcActRng.fDlcRng.fVdlcActRng.fSapFlags = 0;
	    break;
		
        case CLS_NCIA:
            pb.fParameters.fDlcActRng.fDlcRng.fNciaActRng.fSapFlags = 0;
            break;

	case CLS_QLLC:
	    ClsPortActQSapGuts((ActSapQLLCParms_t *)theMsg->fParms, &pb);
	    break;

        default:
            CLSASSERT(FALSE);
            return;
        }
	reg_invoke_dlc_wput(this->fDLCType, CLS_NO_CEP, DLC_ACTIVATE_RING_REQ, &pb);
    }
    else
    {
        CLSIMsgSetPrimitiveType((TCLSIMsg*) theMsg, CLS_CONFIRM);
        CLSIMsgSetReturnCode((TCLSIMsg*) theMsg, CLS_OUT_OF_MEMORY);
	if (cls_debug_err)
	    buginf("\nCLSPortActRing: out of memory error");
        CLSDluAddPendingMsg(dlu, (TCLSIMsg*) theMsg);
    }
}

void CLSPortActRngCnf (TCLSPort* this, ClsDlcParamBlockT* pb)
{
    TCLSSap* theSap;
    TCLSIMsg* theMsg;

    CLSASSERT(this->fPortState == kPortStateActSapPending);

    theSap = (TCLSSap*) pb->fParameters.fDlcActRng.fClsUSapId;
    theSap->fDlcPSapId = pb->fParameters.fDlcActRng.fDlcPSapId;
    this->fPortState = kPortStateEnabled;
    theMsg = this->fRequest;

    if (pb->fResultError == CLS_OK)
    {
        CLSDluAddSap(this->fDluPend, theSap);
        CLSSapActRng(theSap, (ActRngReq_t*) theMsg); /* Get config info from the msg */
    }
    else
    {
        CLSSapDestroy(theSap);
        CLSIMsgSetPrimitiveType(theMsg, CLS_CONFIRM);
        CLSIMsgSetReturnCode(theMsg, pb->fResultError);
	if (cls_debug_err)
	    buginf("\nCLSPortActRngCnf: error %d", pb->fResultError);
        CLSDluAddPendingMsg(this->fDluPend, (TCLSIMsg*) theMsg);
    }
}

static void CLSPortDeactSapReq (TCLSPort* this, TCLSSap* theSap)
{
    ClsDlcParamBlockT pb;

    CLSASSERT_FATAL(CLSSapCheck(theSap));

    pb.fParameters.fDlcActSap.fDLCType = this->fDLCType;
    pb.fParameters.fDlcActSap.fPPortId = this->fDLCPortId;
    pb.fParameters.fDlcActSap.fUSapId = (ClsUSapIdT)theSap;
    CLSIPortIdCopy(&this->fPortId, (CLSPortID*)&pb.fPortId);

    switch (this->fDLCType)
    {
    case CLS_RSRB:
    case CLS_TBRIDGE:	/* intentional fall through, behaves as CLS_LLC */
    case CLS_FRAME_RELAY:
    case CLS_ATM_1483:
    case CLS_PPP:
    case CLS_LLC:
	pb.fParameters.fDlcActSap.fDlcSap.fLlcActSap.fLSapValue = CLSSapGetSapValue(theSap);
	break;
    case CLS_SDLC:
	pb.fParameters.fDlcActSap.fDlcSap.fSdlcActSap.fLSapValue =
	    CLSSapGetSapValue(theSap);
	break;
    case CLS_QLLC:
	/* use the UsapId to identify the sap; QLLC looks through its SAP registry */

	break;
    case CLS_VDLC:
	pb.fParameters.fDlcActSap.fDlcSap.fVdlcActSap.fLSapValue = CLSSapGetSapValue(theSap);
	break;
    case CLS_NCIA:
        break;
    default:
	CLSASSERT(FALSE);
	return;
    }
    reg_invoke_dlc_wput(this->fDLCType, CLS_NO_CEP, DLC_DEACTIVATE_SAP_REQ, &pb);

}

void CLSPortDeactSapCnf (TCLSPort* this, ClsDlcParamBlockT* pb)
{
    TCLSSap* theSap;
    
    theSap = (TCLSSap*) pb->fParameters.fDlcActSap.fUSapId;
    CLSASSERT_FATAL(CLSSapCheck(theSap));
    CLSSapDeactSapCnf(theSap, pb);
}

static void CLSPortDeactRngReq (TCLSPort* this, TCLSSap* theSap)
{
    ClsDlcParamBlockT pb;

    pb.fParameters.fDlcDeActRng.fDLCType = this->fDLCType;
    pb.fParameters.fDlcDeActRng.fPPortId = this->fDLCPortId;
    pb.fParameters.fDlcDeActRng.fUSapId = (ClsUSapIdT)theSap;
    CLSIPortIdCopy(&this->fPortId, (CLSPortID*)&pb.fPortId);

    reg_invoke_dlc_wput(this->fDLCType, CLS_NO_CEP, DLC_DEACTIVATE_RING_REQ, &pb);
}

void CLSPortDeactRngCnf (TCLSPort* this, ClsDlcParamBlockT* pb)
{
    TCLSSap* theSap;
    
    theSap = (TCLSSap*) pb->fParameters.fDlcDeActRng.fUSapId;
    CLSASSERT_FATAL(CLSSapCheck(theSap));
    CLSSapDeactRngCnf(theSap, pb);
}

CLSDLCType_e CLSPortGetType (TCLSPort* this)
{
    return(this->fDLCType);
}

static DlcPPortIdT CLSPortGetPortId (TCLSPort* this)
{
    return(this->fDLCPortId);
}

boolean CLSPortIsTokenRing(TCLSPort* this)
{
    CLSPortID* portA=&this->fPortId;
    
    switch (portA->type) {
    case CLS_P_TYPE_SWIDB:
        return(is_tokenring(portA->fTypeInfo.swidb->hwptr));
    default:
	return(FALSE);
    }
    return(TRUE);
}

boolean CLSPortIsEthernet(TCLSPort* this)
{
   
    CLSPortID* portA=&this->fPortId;

     switch (portA->type)
     {
       case CLS_P_TYPE_SWIDB:
         return(is_ethernet(portA->fTypeInfo.swidb->hwptr));

       default:
          return(FALSE);
          break;
     }
     return(TRUE);
}

boolean CLSPortIsSmds(TCLSPort* this)
{
    CLSPortID* portA=&this->fPortId;
    
    switch (portA->type) {
    case CLS_P_TYPE_SWIDB:
         return(is_smds(portA->fTypeInfo.swidb->hwptr));

    default:
	return(FALSE);
    }
    return(TRUE);
}

boolean CLSPortIsFddi(TCLSPort* this)
{
    CLSPortID* portA=&this->fPortId;
    
    switch (portA->type) {
    case CLS_P_TYPE_SWIDB:
         return(is_fddi(portA->fTypeInfo.swidb->hwptr));

    default:
	return(FALSE);
    }
    return(TRUE);
}

boolean CLSPortIsLLC(TCLSPort* this)
{
    if (this->fDLCType == CLS_LLC) 
	return TRUE;
    else
	return FALSE;
}


TCLSSap* CLSSapCreate (TCLSPort* thePort, TCLSDlu* theDlu)
{
    TCLSSap* this;
    CLSASSERT_FATAL(CLSDluCheck(theDlu));
    this = malloc(sizeof(TCLSSap));
    if (this != NULL)
    {
	bcopy(gSapClassId, this->fClassId, sizeof(gSapClassId));
	this->fNext = NULL;
	this->fPortPtr = thePort;
	this->fUSapId = 0;
	this->fPSapId = (word)this;
	this->fServiceType = 0;
	this->fDluPtr = theDlu;
	this->fRequest = NULL;
	this->fSapState = kSapStateInit;
    }
    return (this);
}

boolean CLSSapCheck (TCLSSap* this)
{
    int i;

    if (this == NULL)
	return FALSE;

    for (i = 0; i < sizeof(gSapClassId); i++)
    {
	if (this->fClassId[i] != gSapClassId[i])
	    return(FALSE);
    }
    if (this->fPSapId != (word) this)
 	return(FALSE);
    
    return(TRUE);
}


void CLSSapDestroy (TCLSSap* this)
{
    memset(this->fClassId, 0, sizeof(gSapClassId));
    free(this);
}

void CLSSapCleanup (TCLSSap* this)
{
    if (this->fRequest != NULL) {
	CLSIMsgDestroy(this->fRequest);
	this->fRequest = NULL;
    }
    this->fSapState = kSapStateDeactPending;
    CLSPortDeactSapReq(this->fPortPtr, this);
}


void CLSSapActSap (TCLSSap* this, ActSapReq_t* theMsg)
{
    size_t parmSize = 0;
    hword retCode = CLS_OK;
    ActSapCnf_t* actSapCnf;
    
    switch (CLSPortGetType(this->fPortPtr))
    {
    case CLS_RSRB:
    case CLS_TBRIDGE:	/* intentional fall through, behaves as CLS_LLC */
    case CLS_FRAME_RELAY:
    case CLS_ATM_1483:
    case CLS_PPP:
    case CLS_LLC:
	parmSize = sizeof(ActSapLLCParms_t);
	break;
    case CLS_SDLC:
	parmSize = sizeof(ActSapSDLCParms_t);
	break;
    case CLS_QLLC:
	parmSize = sizeof(ActSapQLLCParms_t);
	break;
    case CLS_VDLC:
	parmSize = sizeof(ActSapVDLCParms_t);
	break;
    case CLS_NCIA:
        parmSize = sizeof(ActSapNCIAParms_t);
        break;
    default:
	retCode = CLS_UNKNOWN_DLC_TYPE;		/* Unknown port type */
	break;
    }
    
    if (retCode == CLS_OK) 
    {
	bcopy(theMsg->fParms, &this->fParms, parmSize);
	this->fUSapId = theMsg->fCLSIHdr.fUSapId;
    } else {
	if (cls_debug_err)
	    buginf("\nCLSSapActSap: error %d", retCode);
    }

    CLSIMsgSetDestination((TCLSIMsg*) theMsg, CLS_U_SAP_ID, this->fUSapId);
    CLSIMsgSetPrimitiveType((TCLSIMsg*) theMsg, CLS_CONFIRM);
    CLSIMsgSetReturnCode((TCLSIMsg*) theMsg, retCode);

    actSapCnf = (ActSapCnf_t*) theMsg;
    actSapCnf->fCLSIHdr.fPSapId = this->fPSapId;
    actSapCnf->fType1Length = 0;
    actSapCnf->fType2Length = 0;

    CLSDluAddPendingMsg(this->fDluPtr, (TCLSIMsg*) theMsg);
}

void CLSSapActRng (TCLSSap* this, ActRngReq_t* theMsg)
{
    size_t parmSize = 0;
    hword retCode = CLS_OK;
    ActRngCnf_t* actRngCnf;

    switch (CLSPortGetType(this->fPortPtr))
    {
    case CLS_TBRIDGE:	/* intentional fall through, behaves as CLS_LLC */
        parmSize = sizeof(ActRngLLCParms_t);
	break;
    case CLS_RSRB:
        parmSize = sizeof(ActRngTRParms_t);
        break;

    case CLS_LLC:
	if (CLSPortIsTokenRing(this->fPortPtr))
        	parmSize = sizeof(ActRngLLCParms_t);
        break;
    case CLS_SDLC:
	parmSize = sizeof(ActRngSDLCParms_t);
	break;
    case CLS_QLLC:
	parmSize = sizeof(ActRngQLLCParms_t);
	break;
    case CLS_VDLC:
	parmSize = sizeof(ActRngVDLCParms_t);
	break;
    case CLS_NCIA:
        parmSize = sizeof(ActRngNCIAParms_t);
        break;
	
    case CLS_FRAME_RELAY:
    case CLS_ATM_1483:
    case CLS_PPP:
                parmSize = sizeof(ActRngLLCParms_t);
        break;
    default:
        retCode = CLS_UNKNOWN_DLC_TYPE;         /* Unknown port type */
        break;
    }

    if (retCode == CLS_OK)
    {
        bcopy(theMsg->fParms, &this->fParms, parmSize);
        this->fUSapId = theMsg->fCLSIHdr.fUSapId;
    }

    CLSIMsgSetDestination((TCLSIMsg*) theMsg, CLS_U_SAP_ID, this->fUSapId);
    CLSIMsgSetPrimitiveType((TCLSIMsg*) theMsg, CLS_CONFIRM);
    CLSIMsgSetReturnCode((TCLSIMsg*) theMsg, retCode);

    if (cls_debug_err && (retCode != CLS_OK))
	buginf("\nCLSSapActRng: error %d", retCode);

    actRngCnf = (ActRngCnf_t*) theMsg;
    actRngCnf->fCLSIHdr.fPSapId = this->fPSapId;

    CLSDluAddPendingMsg(this->fDluPtr, (TCLSIMsg*)actRngCnf);
}

CLSDLCType_e CLSSapGetType (TCLSSap* this)
{
    return(this->fPortPtr->fDLCType);
}

byte CLSSapGetSapValue (TCLSSap* this)
{

    switch (this->fPortPtr->fDLCType)
    {
    case CLS_RSRB:
    case CLS_TBRIDGE:	/* intentional fall through, behaves as CLS_LLC */
    case CLS_FRAME_RELAY:
    case CLS_ATM_1483:
    case CLS_PPP:
    case CLS_LLC:
	return(this->fParms.fLLCParms.fSapValue);

    case CLS_SDLC:
	return(this->fParms.fSDLCParms.fSapValue);

    case CLS_QLLC:			/* Should never get here */
	CLSASSERT(!"Get Sap Value called for QLLC.  Shouldn't happen");
	return (0);

    case CLS_VDLC:
	return(this->fParms.fVDLCParms.fSapValue);

    case CLS_NCIA:
        return(this->fParms.fNCIAParms.fSapValue);

    default:
	CLSASSERT(FALSE);
	return(0);
    }
}

void CLSSapHandleMsg (TCLSSap* this, TCLSIMsg* theMsg)
{
    switch (CLSIMsgGetPrimitiveCode(theMsg))
    {
    case CLS_DEACTIVATE_SAP | CLS_REQUEST:
	CLSSapDeactSapReq(this, (DeactSapReq_t*) theMsg);
	break;
    case CLS_DEACTIVATE_RING | CLS_REQUEST:
	CLSSapDeactRngReq(this, (DeActRngReq_t*) theMsg);
	break;
    case CLS_REQ_OPNSTN | CLS_REQUEST:
	CLSSapReqOpnStnReq(this, (ReqOpnStnReq_t*) theMsg);
	break;
    case CLS_TEST_STN | CLS_REQUEST:
        CLSSapTestStnReq(this, (TestStnReq_t*) theMsg);
        break;

    case CLS_TEST_STN | CLS_RESPONSE:
        CLSSapTestStnRsp(this, (TestStnRsp_t*) theMsg);
        break;
    case CLS_ID_STN | CLS_REQUEST:
        CLSSapIdStnReq(this, (IdStnReq_t*) theMsg);
        break;
    case CLS_ID_STN | CLS_RESPONSE:
        CLSSapIdStnRsp(this, (IdStnRsp_t*) theMsg);
        break;
    case CLS_UDATA_STN | CLS_REQUEST:
        CLSSapUDataStnReq(this, (UDataStnReq_t*) theMsg);
        break;

    default:
	CLSASSERT(FALSE);
	break;				/* illegal primitive */
    }
    return;
}

void CLSSapDeactSapReq (TCLSSap* this, DeactSapReq_t* theMsg)
{
    TCLSCep* theCep;
    TCLSCep* nextCep;

    this->fRequest = (TCLSIMsg*) theMsg;
    this->fSapState = kSapStateDeactPending;
    theCep = (TCLSCep*) this->fDluPtr->fCepList.qhead;
    while (theCep != NULL)
    {
	nextCep = theCep->fNext;
	if (theCep->fSap == this)
	{
	    p_unqueue(&this->fDluPtr->fCepList, theCep);
	    CLSCepDestroy((TCLSCep*) theCep);
        }
	theCep = nextCep;
    }
    
    CLSPortDeactSapReq(this->fPortPtr, this);
}

void CLSSapDeactSapCnf (TCLSSap* this, ClsDlcParamBlockT* pb)
{
    CLSASSERT(this->fSapState == kSapStateDeactPending);

    if (this->fRequest != NULL) {
	DeactSapReq_t* deactSapReq;
	DeactSapCnf_t* deactSapCnf;
	TCLSDlu* ourDlu;
	word correlator;
	
	deactSapReq = (DeactSapReq_t*) this->fRequest;
	this->fRequest = NULL;

	correlator = deactSapReq->fCorrelator;
	deactSapCnf = (DeactSapCnf_t*) deactSapReq;

	CLSIMsgSetPrimitiveType((TCLSIMsg*) deactSapCnf, CLS_CONFIRM);
	CLSIMsgSetReturnCode((TCLSIMsg*) deactSapCnf, pb->fResultError);

	if (cls_debug_err && (pb->fResultError != CLS_OK))
	    buginf("\nCLSSapDeactSapCnf: error %d", pb->fResultError);

	CLSIMsgSetDestination((TCLSIMsg*) deactSapCnf, CLS_U_SAP_ID, 
			      this->fUSapId);
	deactSapCnf->fVersion = CLSI_VERSION;
	deactSapCnf->fCorrelator = correlator;
	ourDlu = this->fDluPtr;
	CLSDluRemoveSap(this->fDluPtr, this);
	CLSSapDestroy(this);
	CLSDluAddPendingMsg(ourDlu, (TCLSIMsg*) deactSapCnf);
    } else {
	CLSSapDestroy(this);
    }
}

void CLSSapDeactRngReq (TCLSSap* this, DeActRngReq_t* theMsg)
{
    TCLSCep* theCep;
    TCLSCep* nextCep;

    this->fRequest = (TCLSIMsg*) theMsg;
    this->fSapState = kSapStateDeactPending;
    theCep = (TCLSCep*) this->fDluPtr->fCepList.qhead;
    while (theCep != NULL)
    {
	nextCep = theCep->fNext;
	if (theCep->fSap == this)
	{
	    p_unqueue(&this->fDluPtr->fCepList, theCep);
	    CLSCepDestroy((TCLSCep*) theCep);
        }
	theCep = nextCep;
    }
    
    CLSPortDeactRngReq(this->fPortPtr, this);
}

void CLSSapDeactRngCnf (TCLSSap* this, ClsDlcParamBlockT* pb)
{
    CLSASSERT(this->fSapState == kSapStateDeactPending);
    
    if (this->fRequest != NULL) {
	DeActRngReq_t* deactRngReq;
	DeActRngCnf_t* deactRngCnf;
	TCLSDlu* ourDlu;
	word correlator;
	
	deactRngReq = (DeActRngReq_t*) this->fRequest;
	this->fRequest = NULL;
	
	correlator = deactRngReq->fCorrelator;
	deactRngCnf = (DeActRngCnf_t *) deactRngReq;
	
	CLSIMsgSetPrimitiveType((TCLSIMsg*) deactRngCnf, CLS_CONFIRM);
	CLSIMsgSetReturnCode((TCLSIMsg*) deactRngCnf, pb->fResultError);

	if (cls_debug_err && (pb->fResultError != CLS_OK))
	    buginf("\nCLSSapDeactRngCnf: error %d", pb->fResultError);

	CLSIMsgSetDestination((TCLSIMsg*) deactRngCnf, CLS_U_SAP_ID, this->fUSapId);
	deactRngCnf->fVersion = CLSI_VERSION;
	deactRngCnf->fCorrelator = correlator;
	ourDlu = this->fDluPtr;
	CLSDluRemoveSap(this->fDluPtr, this);
	free(this);
	
	CLSDluAddPendingMsg(ourDlu, (TCLSIMsg*) deactRngCnf);
    } else {
	free(this);
    }
}

void 
CLSSapReqOpnStnReq (TCLSSap* this, ReqOpnStnReq_t* theMsg)
{
    TCLSCep* theCep;
    
    theCep = CLSCepCreate(this, this->fDluPtr);
    if (theCep != NULL)
    {
	CLSDluAddCep(this->fDluPtr, theCep);
	CLSCepReqOpnStnReq(theCep, this, theMsg);
    }
    else
    {
	CLSIMsgSetPrimitiveType((TCLSIMsg*) theMsg, CLS_CONFIRM);
	CLSIMsgSetReturnCode((TCLSIMsg*) theMsg, CLS_OUT_OF_MEMORY);
	if (cls_debug_err)
	    buginf("\nCLSSapReqOpnStnReq: CLS_OUT_OF_MEMORY error");
	CLSIMsgSetDestination((TCLSIMsg*) theMsg, CLS_U_CEP_ID, theMsg->fCLSIHdr.fUCepId);
	CLSDluAddPendingMsg(this->fDluPtr, (TCLSIMsg*) theMsg);
    }
}

void CLSSapIdStnInd (TCLSSap* this, ClsDlcParamBlockT* pb)
{
    paktype* pak;
    IdStnInd_t* idStnInd;
    hword reservedSize;

    CLSASSERT_FATAL(pb != NULL);
    
    pak = pb->fPakPtr;
    pb->fPakPtr = NULL;
    
    CLSASSERT_FATAL(pak != NULL);
    
    idStnInd = (IdStnInd_t*) CLSIMsgCreateFromPak(pak, 
						  CLS_ID_STN | CLS_INDICATION,
						  CLS_LLC_LAYER, 
						  CLS_U_SAP_ID,
						  this->fUSapId, 
						  0, 
						  &reservedSize);

    CLSASSERT_FATAL(idStnInd != NULL);
    
    idStnInd->fCLSIHdr.fPCepId = CLS_NO_CEP;
    idStnInd->fVersion = CLSI_VERSION;
    idStnInd->fCorrelator = 0;
    idStnInd->fReserveLength = reservedSize;

    if (pb->fParameters.fDlcIdStn.fXidT == XID_COMMAND) {
	idStnInd->fXIDCmdRsp = CLS_XID_CMD;
    } else {
	idStnInd->fXIDCmdRsp = CLS_XID_RSP;
    }

    idStnInd->fXIDPollFinal = 0;	/* DLC doesn't tell us */

    idStnInd->fXIDLength = pb->fParameters.fDlcIdStn.fXIDLength;
    idStnInd->fXIDOffset = pb->fParameters.fDlcIdStn.fXIDPtr - ((byte*) idStnInd);
    idStnInd->fDlci      = pb->fParameters.fDlcIdStn.fDlcXid.fLlcXidStn.fDlci;
    
    switch (CLSSapGetType(this)) {

    case CLS_TBRIDGE:	/* intentional fall through, behaves as CLS_LLC */
    case CLS_RSRB:
    case CLS_LLC:
    case CLS_FRAME_RELAY:
    case CLS_ATM_1483:
    case CLS_PPP:
    case CLS_SDLC:
    case CLS_QLLC:
    case CLS_VDLC:
    case CLS_NCIA:
	{				/* DLC type is LLC2 */
	    DlcXidIdStnT* llcIdStn;
	    
	    llcIdStn = 
		&(pb->fParameters.fDlcIdStn.fDlcXid.fLlcXidStn);
	    
	    idStnInd->fMacType = llcIdStn->fMacType;
	    idStnInd->fRemoteMacLength = llcIdStn->fRMacLength;
	    idStnInd->fRemoteMacOffset = llcIdStn->fRMacPtr - ((byte*) idStnInd);
	    idStnInd->fLocalMacLength = llcIdStn->fLMacLength;
	    idStnInd->fLocalMacOffset = llcIdStn->fLMacPtr - ((byte*) idStnInd);
	    idStnInd->fRoutingLength = llcIdStn->fRoutingLength;
	    idStnInd->fRoutingOffset = llcIdStn->fRoutingPtr - ((byte*) idStnInd);
	    idStnInd->fLSap = llcIdStn->fLSap;
	    idStnInd->fRSap = llcIdStn->fRSap;
	}
	break;
	
    default:
	CLSASSERT(FALSE);
	break;
    }
    CLSDluAddPendingMsg(this->fDluPtr, (TCLSIMsg*) idStnInd);
}

void CLSSapTestStnInd (TCLSSap* this, ClsDlcParamBlockT* pb)
{
    paktype* thePak;
    TestStnInd_t* testStnInd;
    hword reservedSize;

    thePak = pb->fPakPtr;
    pb->fPakPtr = NULL;


    CLSASSERT_FATAL(thePak != NULL);


    testStnInd = (TestStnInd_t*) CLSIMsgCreateFromPak(thePak,
						      CLS_TEST_STN | CLS_INDICATION,
						      CLS_LLC_LAYER, 
						      CLS_U_SAP_ID,
						      this->fUSapId, 
						      0, 
						      &reservedSize);
    CLSASSERT_FATAL(testStnInd != NULL);


    testStnInd->fVersion = CLSI_VERSION;
    testStnInd->fCorrelator = 0xdead;

    if (pb->fParameters.fDlcTestStn.fTestT == TEST_COMMAND)
        testStnInd->fTESTCmdRsp = CLS_TEST_CMD;
    else
        testStnInd->fTESTCmdRsp = CLS_TEST_RSP;

    testStnInd->fTESTPollFinal = 0;     /* DLC doesn't tell us */

    testStnInd->fTESTLength = pb->fParameters.fDlcTestStn.fTESTLength;
    testStnInd->fTESTOffset = pb->fParameters.fDlcTestStn.fTESTPtr - ((byte*) testStnInd);

    switch (CLSSapGetType(this))
    {
    case CLS_RSRB:
    case CLS_TBRIDGE:	/* intentional fall through, behaves as CLS_LLC */
    case CLS_LLC:
    case CLS_SDLC:
    case CLS_QLLC:
    case CLS_VDLC:
    case CLS_NCIA:
        {
            LlcTestStnT* llcTestStn;

            llcTestStn = &(pb->fParameters.fDlcTestStn.fDlcTest.fLlcTestStn);
	    
            testStnInd->fMacType = llcTestStn->fMacType;
            testStnInd->fRemoteMacLength = llcTestStn->fRMacLength;
            testStnInd->fRemoteMacOffset = llcTestStn->fRMacPtr - ((byte*) testStnInd);
	    
            testStnInd->fLocalMacLength = llcTestStn->fLMacLength;
            testStnInd->fLocalMacOffset = llcTestStn->fLMacPtr - ((byte*) testStnInd);
	    
            testStnInd->fRoutingLength = llcTestStn->fRoutingLength;
            testStnInd->fRoutingOffset = llcTestStn->fRoutingPtr - ((byte*) testStnInd);
	    
            testStnInd->fLSap = llcTestStn->fLSap;
            testStnInd->fRSap = llcTestStn->fRSap;
	    
        }
        break;
    default:
        CLSASSERT(FALSE);
        break;
    }
    CLSDluAddPendingMsg(this->fDluPtr, (TCLSIMsg*) testStnInd);
}

void CLSSapConnectStnInd (TCLSSap* this, ClsDlcParamBlockT* pb)
{
    paktype* thePak;
    ConnectStnInd_t* connectStnInd;
    hword reservedSize;

    thePak = pb->fPakPtr;
    pb->fPakPtr = NULL;
    
    CLSASSERT_FATAL(thePak != NULL);
    connectStnInd = 
	(ConnectStnInd_t*) CLSIMsgCreateFromPak(thePak,
						CLS_CONNECT_STN | CLS_INDICATION,
						CLS_LLC_LAYER, CLS_U_SAP_ID, 
						this->fUSapId, 0, &reservedSize);
    CLSASSERT_FATAL(connectStnInd != NULL);

    connectStnInd->fVersion = CLSI_VERSION;
    connectStnInd->fCorrelator = 0xdead;
    connectStnInd->fReserveLength = reservedSize;

    connectStnInd->fCONNECTPollFinal = 0;     /* DLC doesn't tell us */

    connectStnInd->fCONNECTLength = pb->fParameters.fDlcConnectStn.fCONNECTLength;

    connectStnInd->fCONNECTOffset = pb->fParameters.fDlcConnectStn.fCONNECTPtr - ((byte*) connectStnInd);

    switch (CLSPortGetType(this->fPortPtr)) {
    /*
     * intentional fall-through, as CLS_TBRIDGE is really an LLC type port
     */
    case CLS_TBRIDGE:
    case CLS_LLC:
    case CLS_VDLC:
	{
	    LlcConnectStnT* llcConnectStn;
	    
	    llcConnectStn = (LlcConnectStnT*)
		&(pb->fParameters.fDlcConnectStn.fDlcConnect.fLlcConnectStn);
	    
	    connectStnInd->fMacType = llcConnectStn->fMacType;
	    connectStnInd->fRemoteMacLength = llcConnectStn->fRMacLength;
	    connectStnInd->fRemoteMacOffset = llcConnectStn->fRMacPtr - ((byte*) connectStnInd);
	    
	    connectStnInd->fLocalMacLength = llcConnectStn->fLMacLength;
	    connectStnInd->fLocalMacOffset = llcConnectStn->fLMacPtr - ((byte*)connectStnInd);
	    
	    connectStnInd->fRoutingLength = llcConnectStn->fRoutingLength;
	    connectStnInd->fRoutingOffset = llcConnectStn->fRoutingPtr - ((byte*) connectStnInd);
	    
	    connectStnInd->fLSap = llcConnectStn->fLSap;
	    connectStnInd->fRSap = llcConnectStn->fRSap;
	}
	break;
    case CLS_RSRB:
    case CLS_NCIA:
        break;
    default:
        CLSASSERT(FALSE);
	break;
    }
    CLSDluAddPendingMsg(this->fDluPtr, (TCLSIMsg*) connectStnInd);
}

void CLSCepTestInd (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    paktype* pakPtr;

    pakPtr = pb->fPakPtr;

    pb->fPakPtr = NULL;

    if (pakPtr == NULL)
    {
        /* This must be a timeout. Build an test.cnf for the DLU */
        TestCnf_t* testCnfPtr;
	
        CLSASSERT(this->fCepState == kCepStateTestReqPending);
        this->fCepState = kCepStateOpen;

        testCnfPtr = (TestCnf_t*) CLSIMsgCreateHeader(CLS_TEST | CLS_CONFIRM,
                                       offsetof(TestCnf_t, fTEST) -
                                       sizeof(TestCnfh_t),
                                       CLS_LLC_LAYER,
                                       CLS_U_CEP_ID,
                                       this->fUCepId);
        CLSASSERT(testCnfPtr != NULL);

        CLSIMsgSetReturnCode((TCLSIMsg*) testCnfPtr, pb->fResultError);
        testCnfPtr->fVersion = CLSI_VERSION;
        testCnfPtr->fCorrelator = this->fPendingCorrelator;
        CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) testCnfPtr);
    }
    else if (pb->fParameters.fDlcTest.fTestT == TEST_COMMAND)
    {
        TestInd_t* testIndPtr;
	hword reservedSize;

        testIndPtr = (TestInd_t*) CLSIMsgCreateFromPak(pakPtr, 
						       CLS_TEST | CLS_INDICATION,
						       CLS_LLC_LAYER, 
						       CLS_U_CEP_ID,
						       this->fUCepId, 
						       0,
						       &reservedSize);

        CLSASSERT_FATAL(testIndPtr != NULL);

        testIndPtr->fVersion = CLSI_VERSION;
        testIndPtr->fCorrelator = 0;
        testIndPtr->fReserveLength = reservedSize;
        testIndPtr->fTESTLength = pb->fParameters.fDlcTest.fTESTLength;
        testIndPtr->fTESTOffset = pb->fParameters.fDlcTest.fTESTPtr - ((byte*) testIndPtr);

        CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) testIndPtr);
    } else {
        TestCnf_t* testCnfPtr;
	hword reservedSize;

        testCnfPtr = (TestCnf_t*) CLSIMsgCreateFromPak(pakPtr, 
						       CLS_TEST |
						       CLS_CONFIRM,
						       CLS_LLC_LAYER, 
						       CLS_U_CEP_ID,
						       this->fUCepId, 
						       0, 
						       &reservedSize);

        CLSASSERT_FATAL(testCnfPtr != NULL);

        CLSIMsgSetReturnCode((TCLSIMsg*) testCnfPtr, pb->fResultError);

        testCnfPtr->fVersion = CLSI_VERSION;
        if (this->fCepState == kCepStateTestReqPending) {
	    testCnfPtr->fCorrelator = this->fPendingCorrelator;
	    this->fCepState = kCepStateOpen;
        } else {
	    testCnfPtr->fCorrelator = 0;
        }

        testCnfPtr->fReserveLength = reservedSize;
        testCnfPtr->fTESTLength = pb->fParameters.fDlcTest.fTESTLength;
        testCnfPtr->fTESTOffset = pb->fParameters.fDlcTest.fTESTPtr - ((byte*) testCnfPtr);
        CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) testCnfPtr);
    }

}

void CLSSapUDataStnInd (TCLSSap* this, ClsDlcParamBlockT* pb)
{
    paktype* pakPtr;
    UDataStnInd_t* udataStnInd;
    hword reservedSize;

    pakPtr = pb->fPakPtr;
    pb->fPakPtr = NULL;

    CLSASSERT_FATAL(pakPtr != NULL);
    CLSASSERT(pakPtr->datagramsize != 0);
    udataStnInd = (UDataStnInd_t*) CLSIMsgCreateFromPak(pakPtr, 
							CLS_UDATA_STN |	CLS_INDICATION,
							CLS_LLC_LAYER,
							CLS_U_SAP_ID, 
							this->fUSapId,
							0,
							&reservedSize);

    CLSASSERT(udataStnInd != NULL);

    udataStnInd->fVersion = CLSI_VERSION;
    udataStnInd->fReserveLength = reservedSize;

    /*
     * udataStnInd->fUDataLength = pb->fLoadLength;
     */
    udataStnInd->fUDataLength = pb->fParameters.fDlcUdataStn.fUDataLength;
    udataStnInd->fUDataOffset = pb->fParameters.fDlcUdataStn.fUDataPtr
	- ((byte*) udataStnInd);

    if ((CLSPortIsTokenRing(this->fPortPtr)) || 
        (CLSPortIsFddi(this->fPortPtr)) ||
	(this->fPortPtr->fDLCType == CLS_VDLC)) {
	LlcUdataStnT* llcUdataStn;

	llcUdataStn= (LlcUdataStnT*) &(pb->fParameters.fDlcUdataStn.fDlcUdata.fLlcUdataStn);

	udataStnInd->fMacType = llcUdataStn->fMacType;
	udataStnInd->fRemoteMacLength = llcUdataStn->fRMacLength;
	udataStnInd->fRemoteMacOffset = llcUdataStn->fRMacPtr - ((byte*) udataStnInd);

	udataStnInd->fLocalMacLength = llcUdataStn->fLMacLength;
	udataStnInd->fLocalMacOffset = llcUdataStn->fLMacPtr - ((byte*) udataStnInd);
	udataStnInd->fRoutingLength = llcUdataStn->fRoutingLength;
	udataStnInd->fRoutingOffset = llcUdataStn->fRoutingPtr - ((byte*) udataStnInd);

	udataStnInd->fLSap = llcUdataStn->fLSap;
	udataStnInd->fRSap = llcUdataStn->fRSap;
    } else {
	CLSASSERT(FALSE);
    }

    /* No blocking for now */
    /*
       if (this->fDataIndBlocked) {
       	p_enqueue(&this->fDataIndQueue, udataStnInd);
       } else
       */

    CLSDluAddPendingMsg(this->fDluPtr, (TCLSIMsg*) udataStnInd);
}

void CLSCepListManager (void)
{
    TCLSCep* cepPtr;

    if (onintstack())			/* No malloc from interrupt!! */
	return;
    
    while (gCLSFreeCepCount < CLS_FREE_CEP_MIN)
    {
	cepPtr = malloc(sizeof(TCLSCep));
	if (cepPtr == NULL)
	    break;
	cepPtr->fNext = gCLSFreeCepList;
	gCLSFreeCepList = cepPtr;
	gCLSFreeCepCount++;
    }
    if (gCLSFreeCepCount >= CLS_FREE_CEP_MIN) {
	process_set_boolean(cep_count_low, FALSE); /*List minimum at least*/
    }
}

TCLSCep* CLSCepCreate (TCLSSap* theSap, TCLSDlu* theDlu)
{
    TCLSCep* this;
    leveltype level;
    
    CLSASSERT_FATAL(CLSDluCheck(theDlu));
    level = raise_interrupt_level(NETS_DISABLE);
    CLSCepListManager();
    
    this = gCLSFreeCepList;
    if (this != NULL)
    {
	gCLSFreeCepList = this->fNext;
	gCLSFreeCepCount--;
        if (gCLSFreeCepCount < CLS_FREE_CEP_MIN) {
	    process_set_boolean(cep_count_low, TRUE); /*Below minimum*/
	}	    
    }
    reset_interrupt_level(level);
    
    if (this != NULL)
    {
	bcopy(gCepClassId, this->fClassId, sizeof(gCepClassId));
	this->fNext = NULL;
	this->fDlu = theDlu;
	this->fSap = theSap;
	this->fDLCPCepId = CLS_NO_CEP;
	this->fPCepId = (word) this;
	this->fUCepId = 0;
	this->fRequest = NULL;
	this->fPongReq = NULL;
	this->fCepState = kCepStateInit;
	this->fNeedsDLCClose = FALSE;
	this->fDataIndBlocked = FALSE;
	queue_init(&this->fDataIndQueue, 0);
	this->fNeedsFlowInd = FALSE;
	this->fNeedsDisconnectInd = FALSE;
    }
    return(this);
}

boolean CLSCepCheck (TCLSCep* this)
{
    int i;

    if (this == (TCLSCep*) CLS_NO_CEP || this == NULL)
	return (FALSE);
    for (i = 0; i < sizeof(gCepClassId); i++)
    {
	if (this->fClassId[i] != gCepClassId[i])
	    return(FALSE);
    }
    if (this->fPCepId != (word) this)
	return(FALSE);
    
    return(TRUE);
}

void CLSCepDestroy (TCLSCep* this)
{
    ClsDlcParamBlockT pb;

    this->fCepState = kCepStateCloseStnPending;
    if (this->fRequest != NULL) {
	CLSIMsgDestroy(this->fRequest);
	this->fRequest = NULL;
    }

    if (this->fNeedsDLCClose == TRUE) {
	pb.fParameters.fDlcCloseStn.fActionCode = CLS_ACTION_FORCED;
	pb.fParameters.fDlcCloseStn.fClsUCepId = (ClsUCepIdT) this->fPCepId;
	pb.fParameters.fDlcCloseStn.fDlcPSapId = this->fSap->fDlcPSapId;
    
	reg_invoke_dlc_wput(CLSSapGetType(this->fSap),
			    this->fDLCPCepId, DLC_CLOSE_STN_REQ, &pb);
    }

    if (this->fPongReq != NULL) {
	CLSIMsgDestroy(this->fPongReq);
	this->fPongReq = NULL;
    }

    while (this->fDataIndQueue.count > 0) {
	TCLSIMsg* aMsg = p_dequeue(&this->fDataIndQueue);
	CLSIMsgDestroy(aMsg);
    }
    
    memset(this->fClassId, 0, sizeof(gCepClassId));
    
    if (gCLSFreeCepCount < CLS_FREE_CEP_MAX) {
	leveltype  level;

	level = raise_interrupt_level(NETS_DISABLE);
	this->fNext = gCLSFreeCepList;
	gCLSFreeCepList = this;
	gCLSFreeCepCount++;
	reset_interrupt_level(level);
	if (gCLSFreeCepCount >= CLS_FREE_CEP_MIN) {
	    process_set_boolean(cep_count_low, FALSE); /*At least minimum*/
	}
    } else {
	free(this);
    }
}

void 
CLSCepReqOpnStnReq (TCLSCep* this, TCLSSap* theSap, ReqOpnStnReq_t* theMsg)
{
    ClsDlcParamBlockT pb;
    DlcReqOpenStnT *dlcReqOpen;
    size_t parmSize;
    
    /* Copy the parameter block and hold on to it. */
    parmSize = theMsg->fCLSIHdr.fDataSize -
	(offsetof(ReqOpnStnReq_t, fParms) - sizeof(ReqOpnStnReqh_t));

    this->fUCepId = theMsg->fCLSIHdr.fUCepId;
    this->fDataUCepId = this->fUCepId;
    this->fCepState = kCepStateReqOpnPending;
    this->fRequest  = (TCLSIMsg*) theMsg;
    this->fDLCPCepId = (DlcPCepIdT) CLS_NO_CEP;

    dlcReqOpen = &pb.fParameters.fDlcReqOpenStn;
    
    dlcReqOpen->fPPortId = CLSPortGetPortId(this->fSap->fPortPtr);
    dlcReqOpen->fClsUCepId = (ClsUCepIdT) this->fPCepId;
    dlcReqOpen->fDlcPSapId  = (DlcPSapIdT)theSap->fDlcPSapId;
    dlcReqOpen->fQueuingType = theMsg->fQueuingType;
    dlcReqOpen->fQueuingParameter = theMsg->fQueuingParameter;

    switch (CLSSapGetType(this->fSap))
    {
    case CLS_RSRB:
    case CLS_TBRIDGE:		/* intentional fall through, behaves as CLS_LLC */
    case CLS_FRAME_RELAY:
    case CLS_ATM_1483:
    case CLS_PPP:
    case CLS_LLC:
	{
	    ReqOpnStnLLCParms_t* llcParms;
	    ActSapLLCParms_t* sapLLCParms;
	    LlcReqOpenStnT* llcReqOpen;
	    
	    if (parmSize >= sizeof(ReqOpnStnLLCParms_t))
		bcopy(&theMsg->fParms[0], &this->fParms, sizeof(ReqOpnStnLLCParms_t));
    
	    llcParms = (ReqOpnStnLLCParms_t*) &theMsg->fParms[0];
	    sapLLCParms = (ActSapLLCParms_t*) &this->fSap->fParms.fLLCParms;
	    llcReqOpen = &dlcReqOpen->fDlcStn.fLlcReqOpenStn;
	    
	    snpa_copy(&llcParms->fDestAddr, &llcReqOpen->fRMacAddr);
	    snpa_copy(&llcParms->fSrcAddr, &llcReqOpen->fLMacAddr);
            llcReqOpen->fDlci = llcParms -> fDlci;

	    /*
	     * These first parameters have no defaults. 
	     * They must be specified in the ReqOpnStn request.
	     */

	    llcReqOpen->fRSapValue = llcParms->fDestSap;
	    llcReqOpen->fExplorerOption = llcParms->fExplorerOption;

	    /*
	     * If the caller has included a RIF, copy it. Make sure
	     * the fRifLength is set appropriately in all cases.
	     */
	    if ((llcParms->fExplorerOption & EO_RIF_SPECIFIED) &&
		(llcParms->fRifLength > 0)) {
		llcReqOpen->fRifLength = llcParms->fRifLength;
		bcopy(llcParms->fRif, llcReqOpen->fRif, llcParms->fRifLength);
	    } else {
		llcReqOpen->fRifLength = 0;
	    }

	    
	    /*
	     * Check if we are to use the value specified in the
	     * ReqOpnStn, or the default value from the ActSap.
	     */

	    llcReqOpen->fLSapValue = (llcParms->fSrcSap == 0) ?
		sapLLCParms->fSapValue :
		llcParms->fSrcSap;

	    llcReqOpen->fTimerT1 = (llcParms->fT1 == 0) ?
		    sapLLCParms->fT1 :
		    llcParms->fT1;

	    llcReqOpen->fTimerT2 = (llcParms->fT2 == 0) ? 
		    sapLLCParms->fT2 : 
		    llcParms->fT2;

	    llcReqOpen->fTimerTi = (llcParms->fTi == 0) ? 
		    sapLLCParms->fTi : 
		    llcParms->fTi;

	    llcReqOpen->fMaxout = (llcParms->fMaxOut == 0) ? 
		    sapLLCParms->fMaxOut : 
		    llcParms->fMaxOut;
	    
	    llcReqOpen->fMaxTries = (llcParms->fMaxRetryCount == 0) ?
		    sapLLCParms->fMaxRetryCount : 
		    llcParms->fMaxRetryCount;

	    llcReqOpen->fMaxin = (llcParms->fMaxIn == 0) ?
		    sapLLCParms->fMaxIn :
		    llcParms->fMaxIn;

	    llcReqOpen->fXIDRetries = (llcParms->fXIDRetries == 0) ?
		    sapLLCParms->fXIDRetries :
		    llcParms->fXIDRetries;

	    llcReqOpen->fXIDTimer = (llcParms->fXIDTimer == 0) ?
		    sapLLCParms->fXIDTimer :
		    llcParms->fXIDTimer;

	    llcReqOpen->fExplorerRetries = (llcParms->fExplorerRetries == 0) ?
		sapLLCParms->fExplorerRetries :
		llcParms->fExplorerRetries;
	    
	    llcReqOpen->fExplorerTimer = (llcParms->fExplorerTimer == 0) ?
		sapLLCParms->fExplorerTimer :
		llcParms->fExplorerTimer;
		
	    llcReqOpen->fL3Pid = (llcParms->fL3Pid == 0) ?
		sapLLCParms->fL3Pid :
		llcParms->fL3Pid;
	    
            llcReqOpen->fFlags = llcParms->fFlags;
	    /*
	     * End of default parameter setting. Anything that is
	     * still set to 0 at this point will be grabbed from
	     * the idb by the llc2 code.
	     */
	}
	break;
    case CLS_SDLC:
	{
	    ReqOpnStnSDLCParms_t* sdlcParms;
	    ActSapSDLCParms_t* sapSDLCParms;
	    SdlcReqOpenStnT* sdlcReqOpen;
	    
	    if (parmSize >= sizeof(ReqOpnStnSDLCParms_t))
		bcopy(&theMsg->fParms[0], &this->fParms, sizeof(ReqOpnStnSDLCParms_t));
    
	    sdlcParms = (ReqOpnStnSDLCParms_t*) &theMsg->fParms[0];
	    sapSDLCParms = (ActSapSDLCParms_t*) &this->fSap->fParms.fSDLCParms;
	    sdlcReqOpen = &dlcReqOpen->fDlcStn.fSdlcReqOpenStn;
	    
	    snpa_copy(&sdlcParms->fDestAddr, &sdlcReqOpen->fRMacAddr);
	    snpa_copy(&sdlcParms->fSrcAddr, &sdlcReqOpen->fLMacAddr);

	    sdlcReqOpen->fRSapValue = sdlcParms->fDestSap;
	    sdlcReqOpen->fLSapValue = sdlcParms->fSrcSap;

	}
	break;
	
    case CLS_QLLC:		/* unlike SDLC and LLC2 QLLC keeps Sap Defaults
    				 * in a dlc layer sap type, and so it doesn't
				 * do the overrides in this routine
				 */
	{
	    
	    ReqOpnStnQLLCParms_t* msgParms;
	    QLLCReqOpenStnT* qllcReqOpen;

	    msgParms = (ReqOpnStnQLLCParms_t*) &theMsg->fParms[0];

	    /* copy parameters into CLS's CEP */
	    bcopy(msgParms, &this->fParms, sizeof(ReqOpnStnQLLCParms_t));
    
	    /* blast parameters into ClsDlcParamBlock */
	    qllcReqOpen = &dlcReqOpen->fDlcStn.fQLLCReqOpenStn;    
	    bcopy(msgParms, qllcReqOpen, sizeof(ReqOpnStnQLLCParms_t));
	}
	break;
    case CLS_VDLC:
        {
	    ReqOpnStnVDLCParms_t* vdlcParms;
	    ActSapVDLCParms_t* sapVDLCParms;
	    VdlcReqOpenStnT* vdlcReqOpen;
	    
	    if (parmSize >= sizeof(ReqOpnStnVDLCParms_t))
		bcopy(&theMsg->fParms[0], &this->fParms, sizeof(ReqOpnStnVDLCParms_t));
    
	    vdlcParms = (ReqOpnStnVDLCParms_t*) &theMsg->fParms[0];
	    sapVDLCParms = (ActSapVDLCParms_t*) &this->fSap->fParms.fVDLCParms;
	    vdlcReqOpen = &dlcReqOpen->fDlcStn.fVdlcReqOpenStn;
	    
	    snpa_copy(&vdlcParms->fDestAddr, &vdlcReqOpen->fRMacAddr);
	    snpa_copy(&vdlcParms->fSrcAddr, &vdlcReqOpen->fLMacAddr);

	    vdlcReqOpen->fRSapValue = vdlcParms->fDestSap;

	    vdlcReqOpen->fLSapValue = (vdlcParms->fSrcSap == 0) ?
		sapVDLCParms->fSapValue : vdlcParms->fSrcSap;


	    vdlcReqOpen->fTimerT1 = (vdlcParms->fXIDTimer == 0) ?
		sapVDLCParms->fT1 : vdlcParms->fT1;
	    
	    vdlcReqOpen->fRetries = (vdlcParms->fMaxRetryCount == 0) ?
		sapVDLCParms->fMaxRetryCount : vdlcParms->fMaxRetryCount;      

        }
	break;
    case CLS_NCIA:
        {
            ReqOpnStnNCIAParms_t *nciaParms;
            ActSapNCIAParms_t    *sapNCIAParms;
            NciaReqOpenStnT      *nciaReqOpen;
 
            if (parmSize >= sizeof(ReqOpnStnNCIAParms_t))
                bcopy(&theMsg->fParms[0], &this->fParms, sizeof(ReqOpnStnNCIAParms_t));
 
            nciaParms = (ReqOpnStnNCIAParms_t*) &theMsg->fParms[0];
            sapNCIAParms = (ActSapNCIAParms_t*) &this->fSap->fParms.fNCIAParms;
            nciaReqOpen = &dlcReqOpen->fDlcStn.fNciaReqOpenStn;
 
            snpa_copy(&nciaParms->fDestAddr, &nciaReqOpen->fDestAddr);
            snpa_copy(&nciaParms->fSrcAddr, &nciaReqOpen->fSrcAddr);
 
            nciaReqOpen->fDestSap = nciaParms->fDestSap;
 
            nciaReqOpen->fSrcSap = (nciaParms->fSrcSap == 0) ?
                sapNCIAParms->fSapValue : nciaParms->fSrcSap;
 
            nciaReqOpen->fQReqOpenStnFlags = 0;
        }
        break;

	
    default:
	CLSASSERT(FALSE);
	if (cls_debug_err)
	    buginf("\nCLSCepReqOpnStnReq: unknown DLC Type %d", CLSSapGetType(this->fSap));
	return;
    }
    
    dlcReqOpen->fDlcPCepId = (DlcPCepIdT) CLS_NO_CEP;
    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId,
 				      DLC_REQ_OPNSTN_REQ, &pb); /* Send to DLC */
    this->fDLCPCepId = dlcReqOpen->fDlcPCepId;
  }

void ClsCepReqOpnStnCnf (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    TCLSDlu* ourDlu;
    ReqOpnStnReq_t* reqOpnStnReq;
    ReqOpnStnCnf_t* reqOpnStnCnf;
    word correlator;

    if (this->fCepState == kCepStateCloseStnPending) {
	return;
    }

    if (this->fRequest == NULL) {
	if (cls_debug_err) {
	    buginf("\nCLS: Req OpenStn Confirm - null request");
	}
	return;
    }
    if (this->fCepState != kCepStateReqOpnPending) {
	if (cls_debug_err) {
	    buginf("\nCLS: Req OpenStn Confirm state %s "
		   "should be kCepStateReqOpnPending",
		   CLSStateToString(this->fCepState));
	}
	return;
    }

    ourDlu = this->fDlu;
    reqOpnStnReq = (ReqOpnStnReq_t*) this->fRequest;
    this->fRequest = NULL;
    correlator = reqOpnStnReq->fCorrelator;
    
    reqOpnStnCnf = (ReqOpnStnCnf_t*) reqOpnStnReq;
    CLSIMsgSetPrimitiveType((TCLSIMsg*) reqOpnStnCnf, CLS_CONFIRM);
    CLSIMsgSetReturnCode((TCLSIMsg*) reqOpnStnCnf, pb->fResultError);
    CLSIMsgSetDestination((TCLSIMsg*) reqOpnStnCnf, CLS_U_CEP_ID, this->fUCepId);

    reqOpnStnCnf->fVersion = CLSI_VERSION;
    reqOpnStnCnf->fCorrelator = correlator;
    
    if (pb->fResultError == CLS_OK)
    {
	this->fNeedsDLCClose = TRUE;
	this->fCepState = kCepStateOpen;
	this->fDLCPCepId = pb->fParameters.fDlcReqOpenStn.fDlcPCepId;
	reqOpnStnCnf->fCLSIHdr.fPCepId = this->fPCepId;
    }
    else
    {
	if (cls_debug_err)
	    buginf("\nClsCepReqOpnStnCnf: failed %s", 
		   CLSReturnCodeToString(pb->fResultError));
	CLSDluRemoveCep(ourDlu, this);
	CLSCepDestroy(this);
    }
    CLSDluAddPendingMsg(ourDlu, (TCLSIMsg*) reqOpnStnCnf);
}


void CLSCepIdReq (TCLSCep* this, IdReq_t* theMsg)
{
    ClsDlcParamBlockT pb;
    paktype* pakPtr;
    /* uchar print_buf[200]; */

    this->fCepState = kCepStateIdReqPending;

    this->fPendingCorrelator = theMsg->fCorrelator;
    
    pakPtr = CLSIMsgGetPak((TCLSIMsg*) theMsg);
    pakPtr->datagramstart = &theMsg->fXID[0] + theMsg->fReserveLength;
    pakPtr->datagramsize =  theMsg->fCLSIHdr.fDataSize
        	- (offsetof(IdReq_t, fXID) - sizeof(IdReqh_t))
	        - theMsg->fReserveLength;
    
    pb.fPakPtr = pakPtr;
    pb.fParameters.fDlcIdStn.fClsUCepId = (ClsUCepIdT) this->fPCepId;
    pb.fParameters.fDlcIdStn.fClsUSapId = (ClsUSapIdT) this->fSap->fPSapId;
    pb.fParameters.fDlcIdStn.fDlcPCepId = this->fDLCPCepId;
    pb.fParameters.fDlcIdStn.fXidT = XID_COMMAND;

    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId, DLC_ID_REQ, &pb);
}


void CLSCepIdRsp (TCLSCep* this, IdRsp_t* theMsg)
{
    ClsDlcParamBlockT pb;
    paktype* pakPtr;
    
    pakPtr = CLSIMsgGetPak((TCLSIMsg*) theMsg);
    pakPtr->datagramstart = &theMsg->fXID[0] + theMsg->fReserveLength;
    pakPtr->datagramsize =  theMsg->fCLSIHdr.fDataSize -
	(offsetof(IdRsp_t, fXID)-offsetof(IdRsp_t, fVersion));
    
    pb.fParameters.fDlcIdStn.fClsUCepId = (ClsUCepIdT) this->fPCepId;
    pb.fParameters.fDlcIdStn.fClsUSapId = (ClsUSapIdT) this->fSap->fPSapId;
    pb.fParameters.fDlcIdStn.fDlcPCepId = this->fDLCPCepId;
    pb.fPakPtr = pakPtr;
    pb.fParameters.fDlcIdStn.fXidT = XID_RESPONSE;
    
    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId,
                        DLC_ID_RSP, &pb);

}


void CLSCepTestReq (TCLSCep* this, TestReq_t* theMsg)
{
    ClsDlcParamBlockT pb;
    paktype* pakPtr;

    this->fCepState = kCepStateTestReqPending;

    this->fPendingCorrelator = theMsg->fCorrelator;

    pakPtr = CLSIMsgGetPak((TCLSIMsg*) theMsg);
    pakPtr->datagramstart = &theMsg->fTEST[0] + theMsg->fReserveLength;
    pakPtr->datagramsize =  theMsg->fCLSIHdr.fDataSize
                - (offsetof(TestReq_t, fTEST) - sizeof(TestReqh_t))
                - theMsg->fReserveLength;

    pb.fPakPtr = pakPtr;
    pb.fParameters.fDlcTestStn.fClsUCepId = (ClsUCepIdT) this->fPCepId;
    pb.fParameters.fDlcTestStn.fClsUSapId = (ClsUSapIdT) this->fSap->fPSapId;
    pb.fParameters.fDlcTestStn.fDlcPCepId = this->fDLCPCepId;
    pb.fParameters.fDlcTestStn.fTestT = TEST_COMMAND;

    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId, DLC_TEST_REQ, &pb);
}
void CLSCepTestRsp (TCLSCep* this, TestRsp_t* theMsg)
{
    ClsDlcParamBlockT pb;
    paktype* pakPtr;

    pakPtr = CLSIMsgGetPak((TCLSIMsg*) theMsg);
    pakPtr->datagramstart = &theMsg->fTEST[0] + theMsg->fReserveLength;
    pakPtr->datagramsize =  theMsg->fCLSIHdr.fDataSize -
        (offsetof(TestRsp_t, fTEST)-offsetof(TestRsp_t, fVersion));

    pb.fParameters.fDlcTestStn.fClsUCepId = (ClsUCepIdT) this->fPCepId;
    pb.fParameters.fDlcTestStn.fClsUSapId = (ClsUSapIdT) this->fSap->fPSapId;
    pb.fParameters.fDlcTestStn.fDlcPCepId = this->fDLCPCepId;
    pb.fPakPtr = pakPtr;
    pb.fParameters.fDlcTestStn.fTestT = TEST_RESPONSE;

    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId, DLC_TEST_RSP, &pb);
}

void CLSCepConnectReq (TCLSCep* this, ConnectReq_t* theMsg)
{
    ClsDlcParamBlockT pb;

    this->fRequest = (TCLSIMsg*) theMsg;
    this->fCepState = kCepStateConnectReqPending;

    pb.fPakPtr = NULL;
    pb.fParameters.fDlcConnect.fModifyFlags = theMsg->fCLSIHdr.fModifyFlags;
    pb.fParameters.fDlcConnect.fQueuingType = theMsg->fQueuingType;
    pb.fParameters.fDlcConnect.fQueuingParameter = theMsg->fQueuingParameter;

    if (theMsg->fCLSIHdr.fModifyFlags != 0) {
	switch (CLSSapGetType(this->fSap)) {
	case CLS_RSRB:
	case CLS_TBRIDGE:		/* intentional fall through, behaves as CLS_LLC */
	case CLS_FRAME_RELAY:
	case CLS_ATM_1483:
        case CLS_PPP:
	case CLS_LLC:
        case CLS_NCIA:
	    {
		ReqOpnStnLLCParms_t* fromDLUParms;
		LlcReqOpenStnT* toDLCParms;

		fromDLUParms = (ReqOpnStnLLCParms_t*) &theMsg->fParms[0];
		toDLCParms =
		    &pb.fParameters.fDlcConnect.fDlcMod.fLlcReqOpenStn;

		/*
		 * We copy all these parameters into the control
		 * block, but in reality, dlcllc.c only supports
		 * fMaxOut. None of the others are really interesting in
		 * the current implementation.
		 */
		toDLCParms->fMaxout = fromDLUParms->fMaxOut;
		toDLCParms->fMaxin = fromDLUParms->fMaxIn;
		toDLCParms->fTimerT1 = fromDLUParms->fT1;
		toDLCParms->fMaxTries = fromDLUParms->fMaxRetryCount;
	    }
	    break;
	case CLS_SDLC:
	    {
		ReqOpnStnSDLCParms_t* fromDLUParms;
		SdlcReqOpenStnT* toDLCParms;

		fromDLUParms = (ReqOpnStnSDLCParms_t*) &theMsg->fParms[0];
		toDLCParms =
		    &pb.fParameters.fDlcConnect.fDlcMod.fSdlcReqOpenStn;
		/*
		 * Current dlcsdlc.c does not support modification
		 * of parameters. This is an area for further
		 * work.
		 * SHB 2/8/95
		 */
	    }
	    break;
	case CLS_QLLC:
	    {
		ReqOpnStnQLLCParms_t* fromDLUParms;
		QLLCReqOpenStnT* toDLCParms;

		fromDLUParms = (ReqOpnStnQLLCParms_t*) &theMsg->fParms[0];
		toDLCParms =
		    &pb.fParameters.fDlcConnect.fDlcMod.fQllcReqOpenStn;
	    }
	    break;
        case CLS_VDLC:
            {
		ReqOpnStnVDLCParms_t* fromDLUParms;
		VdlcReqOpenStnT* toDLCParms;

		fromDLUParms = (ReqOpnStnVDLCParms_t*) &theMsg->fParms[0];
		toDLCParms =
		    &pb.fParameters.fDlcConnect.fDlcMod.fVdlcReqOpenStn;

            }
            break;
	default:
	    CLSASSERT(FALSE);
	    break;
	}
    }

    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), 
			this->fDLCPCepId, 
			DLC_CONNECT_REQ, 
			&pb);
}

void CLSCepConnectRsp (TCLSCep* this, ConnectRsp_t* theMsg)
{
    ClsDlcParamBlockT pb;

    this->fCepState = kCepStateOpen;
    
    /*
     * Think about merging this fFlag into fModifyFlags to simplify
     * things.
     * SHB 2/8/95
     */
    pb.fPakPtr = NULL;
    pb.fParameters.fDlcConnect.fFlag = theMsg->fFlag;
    pb.fParameters.fDlcConnect.fModifyFlags = theMsg->fCLSIHdr.fModifyFlags;
    pb.fParameters.fDlcConnect.fQueuingType = theMsg->fQueuingType;
    pb.fParameters.fDlcConnect.fQueuingParameter = theMsg->fQueuingParameter;

    if (theMsg->fCLSIHdr.fModifyFlags != 0) {
	switch (CLSSapGetType(this->fSap)) {
	case CLS_RSRB:
	case CLS_TBRIDGE:		/* intentional fall through, behaves as CLS_LLC */
	case CLS_FRAME_RELAY:
	case CLS_ATM_1483:
        case CLS_PPP:
	case CLS_LLC:
	    {
		ReqOpnStnLLCParms_t* fromDLUParms;
		LlcReqOpenStnT* toDLCParms;

		fromDLUParms = (ReqOpnStnLLCParms_t*) &theMsg->fParms[0];
		toDLCParms =
		    &pb.fParameters.fDlcConnect.fDlcMod.fLlcReqOpenStn;
		toDLCParms->fMaxout = fromDLUParms->fMaxOut;
		toDLCParms->fMaxin = fromDLUParms->fMaxIn;
		toDLCParms->fTimerT1 = fromDLUParms->fT1;
		toDLCParms->fMaxTries = fromDLUParms->fMaxRetryCount;
	    }
	    break;
	case CLS_SDLC:
	    {
		ReqOpnStnSDLCParms_t* fromDLUParms;
		SdlcReqOpenStnT* toDLCParms;

		fromDLUParms = (ReqOpnStnSDLCParms_t*) &theMsg->fParms[0];
		toDLCParms =
		    &pb.fParameters.fDlcConnect.fDlcMod.fSdlcReqOpenStn;
		/*
		 * Current dlcsdlc.c does not support modification
		 * of parameters. This is an area for further
		 * work.
		 * SHB 2/8/95
		 */
	    }
	    break;
	case CLS_QLLC:
	    {
		ReqOpnStnQLLCParms_t* fromDLUParms;
		QLLCReqOpenStnT* toDLCParms;

		fromDLUParms = (ReqOpnStnQLLCParms_t*) &theMsg->fParms[0];
		toDLCParms =
		    &pb.fParameters.fDlcConnect.fDlcMod.fQllcReqOpenStn;
	    }
	    break;
	case CLS_VDLC:
            {
		ReqOpnStnVDLCParms_t* fromDLUParms;
		VdlcReqOpenStnT* toDLCParms;

		fromDLUParms = (ReqOpnStnVDLCParms_t*) &theMsg->fParms[0];
		toDLCParms =
		    &pb.fParameters.fDlcConnect.fDlcMod.fVdlcReqOpenStn;
            }
	    break;
	default:
	    CLSASSERT(FALSE);
	    break;
	}
    }

    CLSIMsgDestroy((TCLSIMsg*) theMsg);  /*destroy the buffer */
    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), 
			this->fDLCPCepId, 
			DLC_CONNECT_RSP, 
			&pb);

}

void CLSCepSignalStnReq (TCLSCep* this, SignalStnReq_t* theMsg)
{
    ClsDlcParamBlockT pb;
    
    pb.fPakPtr = NULL;
    pb.fParameters.fSignalStnActionCode = theMsg->fCLSIHdr.fActionCode;
    CLSIMsgDestroy((TCLSIMsg*) theMsg);  /*destroy the buffer */
    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId, DLC_SIGNAL_STN_REQ, &pb);
}

void CLSCepDataReq (TCLSCep* this, DataReq_t* theMsg)
{
    ClsDlcParamBlockT pb;
    paktype* pakPtr;
    
    pakPtr = CLSIMsgGetPak((TCLSIMsg*) theMsg);
    pakPtr->datagramstart = &theMsg->fData[0] + theMsg->fReserveLength;
    pakPtr->datagramsize = theMsg->fCLSIHdr.fDataSize
	    - (offsetof(DataReq_t, fData) - offsetof(DataReq_t,fVersion))
	    - theMsg->fReserveLength;

    pb.fPakPtr = pakPtr;

    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId, DLC_DATA_REQ, &pb);

}

void CLSCepUDataReq (TCLSCep* this, UDataReq_t* theMsg)
{
    ClsDlcParamBlockT pb;
    paktype* pakPtr;

    pakPtr = CLSIMsgGetPak((TCLSIMsg*) theMsg);
    pakPtr->datagramstart = &theMsg->fUData[0] + theMsg->fReserveLength;
    pakPtr->datagramsize = theMsg->fCLSIHdr.fDataSize
            - (offsetof(UDataReq_t, fUData) - offsetof(UDataReq_t,fVersion))
            - theMsg->fReserveLength;

    pb.fPakPtr = pakPtr;

    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId, DLC_UDATA_REQ, &pb);
}


void CLSCepFlowReq (TCLSCep* this, FlowReq_t* theMsg)
{
    ClsDlcParamBlockT pb;

    pb.fPakPtr = NULL;
    if (theMsg->fCLSIHdr.fFlowAction == CLS_ACTION_STOP_FLOW)
    {
	this->fDataIndBlocked = TRUE;
	pb.fParameters.fResumeFlow = FALSE;
    }
    else
    {
        /*
         * Set fDataIndBlocked to prevent fDataIndQueue contention
         * with data.ind.
         */
        this->fDataIndBlocked = TRUE;

	/* queue any pending messages first ... */
	while(this->fDataIndQueue.count > 0)
	{
          if (!reg_invoke_ucep_cantput(
                  (uint)this->fDlu->fReadQ,this->fDataUCepId,
                  (QUEUEEMPTY(&this->fDataIndQueue)))) {
            TCLSIMsg* aMsg = p_dequeue(&this->fDataIndQueue);
	    CLSDluAddPendingMsg(this->fDlu, aMsg);
	  }
          else
            break;
	}
	
	this->fDataIndBlocked = FALSE;
	pb.fParameters.fResumeFlow = TRUE;
    }
    CLSIMsgDestroy((TCLSIMsg*) theMsg);  /*destroy the buffer */
    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId, DLC_FLOW_REQ, &pb);
}

void CLSCepPongReq (TCLSCep* this, PongReq_t* theMsg)
{
    ClsDlcParamBlockT pb;

    CLSASSERT(this->fPongReq == NULL);
    this->fPongReq = (TCLSIMsg*) theMsg;

    pb.fPakPtr = NULL;
    pb.fParameters.fOutboundQEmpty = theMsg->fCLSIHdr.fOutboundQEmpty;
    
    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId, DLC_PONG_REQ, &pb);
}

void CLSCepDisconnectReq (TCLSCep* this, DisconnectReq_t* theMsg)
{
    ClsDlcParamBlockT pb;

    CLSASSERT(this->fCepState != kCepStateInit);

    /* free any request messages that we are holding */
    if (this->fRequest != NULL)
        CLSIMsgDestroy(this->fRequest);

    this->fRequest = (TCLSIMsg*) theMsg;
    this->fCepState = kCepStateDisconnectReqPending;

    pb.fPakPtr = NULL;
    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId, DLC_DISC_REQ, &pb);

}

void CLSCepDisconnectRsp (TCLSCep* this, DisconnectRsp_t* theMsg)
{
    ClsDlcParamBlockT pb;

    this->fCepState = kCepStateOpen;
    CLSIMsgDestroy((TCLSIMsg*) theMsg);  /*destroy the buffer */

    pb.fPakPtr = NULL;
    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId, DLC_DISC_RSP, &pb);
}

void CLSCepCloseStnReq (TCLSCep* this, CloseStnReq_t* theMsg)
{
    ClsDlcParamBlockT pb;

    if (this == (TCLSCep*) CLS_NO_CEP){
        CLSIMsgDestroy((TCLSIMsg*) theMsg);
        return;
    }
    
    if (this->fCepState != kCepStateOpen && 
	this->fCepState != kCepStateReqOpnPending) {
	if (cls_debug_err) {
	    buginf("\nCLSCepCloseStnReq: this %x, state %s, fRequest %x", 
		   this, CLSStateToString(this->fCepState), this->fRequest);
	}
    }
    
    this->fCepState = kCepStateCloseStnPending;
    /* avoid memory leaks if ROS Req pending */
    if (this->fRequest != NULL) {
	CLSIMsgDestroy(this->fRequest);
    }
    this->fRequest = (TCLSIMsg*) theMsg;
    
    pb.fParameters.fDlcCloseStn.fActionCode = theMsg->fCLSIHdr.fActionCode;
    pb.fParameters.fDlcCloseStn.fClsUCepId = (ClsUCepIdT) this->fPCepId;
    pb.fParameters.fDlcCloseStn.fDlcPSapId = this->fSap->fDlcPSapId;
    
    /* 
     * If this is cancelling an outstanding ReqOpnStn, then we only
     * know the dlcpcepid, because the DLCs put it in the parameter
     * block, and CLSCepReqOpenStnReq gets it from there.
     *
     * The better solution would be for all DLCs to implement something
     * that could take the CLS Cep and look up the coresponding DLC Cep
     */

    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), this->fDLCPCepId, DLC_CLOSE_STN_REQ, &pb);

}

void CLSCepOpenStnReq(TCLSCep* this, OpenStnReq_t* openStnReq)
{
    OpenStnCnf_t* openStnCnf;
    hword correlator;
    
    CLSASSERT(openStnReq != NULL);

    this->fDataUCepId = openStnReq->fCLSIHdr.fUCepId;
    correlator = openStnReq->fCorrelator;
    
    openStnCnf = (OpenStnCnf_t*) openStnReq;
    openStnCnf->fCLSIHdr.fHeaderLength = CLSIGetHdrSize(CLS_OPEN_STN | CLS_CONFIRM);
    openStnCnf->fCLSIHdr.fPrimitiveCode = CLS_OPEN_STN | CLS_CONFIRM;
    openStnCnf->fCLSIHdr.fDataSize = sizeof(OpenStnCnf_t) - sizeof(OpenStnCnfh_t);
    openStnCnf->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    openStnCnf->fCLSIHdr.fPCepId = this->fPCepId;

    CLSIMsgSetDestination((TCLSIMsg*) openStnCnf, CLS_U_CEP_ID, this->fDataUCepId);
    CLSIMsgSetReturnCode((TCLSIMsg*) openStnCnf, CLS_OK);

    openStnCnf->fVersion = CLSI_VERSION;
    openStnCnf->fCorrelator = correlator;
    
    CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) openStnCnf);
}

void CLSCepIdInd (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    paktype* pakPtr;
    
    pakPtr = pb->fPakPtr;
    pb->fPakPtr = NULL;
    
    if (pakPtr == NULL)
    {
	/* This must be a timeout. Build an Id.cnf for the DLU */
	IdCnf_t* idCnfPtr;

	CLSASSERT(this->fCepState == kCepStateIdReqPending);
	this->fCepState = kCepStateOpen;

	/* Timers always go off at process level, so we can always get */
	/* the memory we need for this message. */
	idCnfPtr = (IdCnf_t*) CLSIMsgCreateHeader(CLS_ID | CLS_CONFIRM,
				       offsetof(IdCnf_t, fXID) -
				       sizeof(IdCnfh_t),
				       CLS_LLC_LAYER,
				       CLS_U_CEP_ID,
				       this->fUCepId);
	CLSASSERT(idCnfPtr != NULL);
        if (idCnfPtr == NULL)
            return; /* don't play with NULL pointers */

	CLSIMsgSetReturnCode((TCLSIMsg*) idCnfPtr, pb->fResultError);
	idCnfPtr->fVersion = CLSI_VERSION;
	idCnfPtr->fCorrelator = this->fPendingCorrelator;

	CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) idCnfPtr);
	return;
    }

 
    /*
     * When the pak comes to us, datagramstart and datagramsize
     * are set such that the entire mac encapsulation is still
     * there. We want to adjust the packet so that the CLSI header
     * immediately precedes the XID data field. We can do this
     * because the DLC code has set pak->network_start for us. We
     * are effectly stripping off all the mac layer headers and then
     * allowing CLSIMsgCreateFromPak() to build the CLSI message
     * header in its place.
     */
    pakPtr->datagramsize -= (pakPtr->network_start - pakPtr->datagramstart);
    pakPtr->datagramstart = pakPtr->network_start;
    
    if (pb->fParameters.fDlcIdStn.fXidT == XID_COMMAND)
    {
	IdInd_t* idIndPtr;
	hword reservedSize;

	idIndPtr = (IdInd_t*) CLSIMsgCreateFromPak(pakPtr, 
						   CLS_ID | CLS_INDICATION,
						   CLS_LLC_LAYER, 
						   CLS_U_CEP_ID,
						   this->fUCepId, 
						   0,
						   &reservedSize);
	CLSASSERT_FATAL(idIndPtr != NULL);
	CLSIMsgSetReturnCode((TCLSIMsg*) idIndPtr, pb->fResultError);
	idIndPtr->fVersion = CLSI_VERSION;
	idIndPtr->fCorrelator = 0;
	idIndPtr->fReserveLength = reservedSize;

        idIndPtr->fXIDLength = pb->fParameters.fDlcId.fXIDLength;
        idIndPtr->fXIDOffset = pb->fParameters.fDlcId.fXIDPtr - ((byte*) idIndPtr);
	
	CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) idIndPtr);
    }
    else
    {
	IdCnf_t* idCnfPtr;
	hword reservedSize;

	idCnfPtr = (IdCnf_t*) CLSIMsgCreateFromPak(pakPtr, 
						   CLS_ID | CLS_CONFIRM,
						   CLS_LLC_LAYER, 
						   CLS_U_CEP_ID,
						   this->fUCepId, 
						   0,
						   &reservedSize);
	CLSASSERT_FATAL(idCnfPtr != NULL);

	CLSIMsgSetReturnCode((TCLSIMsg*) idCnfPtr, pb->fResultError);

	idCnfPtr->fVersion = CLSI_VERSION;
	if (this->fCepState == kCepStateIdReqPending)
	{
	    idCnfPtr->fCorrelator = this->fPendingCorrelator;
	    this->fCepState = kCepStateOpen;
	}
	else
	{
	    idCnfPtr->fCorrelator = 0;
	}
	
	idCnfPtr->fReserveLength = reservedSize;

        idCnfPtr->fXIDLength = pb->fParameters.fDlcId.fXIDLength;
        idCnfPtr->fXIDOffset = pb->fParameters.fDlcId.fXIDPtr - ((byte*) idCnfPtr);
	
	CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) idCnfPtr);
    }
}

void CLSCepConnectInd (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    ConnectInd_t* connInd;
    hword reservedSize;

    /* This happened because we received a SABME or the like. We */
    /* should be able to reuse the packet to build the CLSI message. */

    if (pb->fPakPtr != NULL) {
	connInd = (ConnectInd_t*) 
	    CLSIMsgCreateFromPak(pb->fPakPtr,
				 CLS_CONNECT | CLS_INDICATION,
				 CLS_LLC_LAYER,
				 CLS_U_CEP_ID,
				 this->fUCepId,
				 0,
				 &reservedSize);
	pb->fPakPtr = NULL;
    } else {
	connInd = (ConnectInd_t*) CLSIMsgCreateHeader(CLS_CONNECT | CLS_INDICATION,
						      sizeof(ConnectInd_t) - sizeof(ConnectIndh_t),
						      CLS_LLC_LAYER,
						      CLS_U_CEP_ID,
						      this->fUCepId);
    }
    
    CLSASSERT(connInd != NULL);
    if (connInd == NULL)
        return;

    connInd->fVersion = CLSI_VERSION;
    connInd->fCorrelator = 0;
    connInd->fRifLength = pb->fParameters.fDlcConnect.fDlcMod.fLlcReqOpenStn.fRifLength;
    if (connInd->fRifLength) {
       memcpy(&connInd->fRif[0],
             &pb->fParameters.fDlcConnect.fDlcMod.fLlcReqOpenStn.fRif[0],
             connInd->fRifLength);
    }
   
    CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) connInd);
}

void CLSCepConnectCnf (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    ConnectReq_t* connReq;
    ConnectCnf_t* connCnf;
    word correlator;
    
    CLSASSERT(this->fCepState == kCepStateConnectReqPending);
    this->fCepState = kCepStateOpen;
    connReq = (ConnectReq_t*) this->fRequest;
    this->fRequest = NULL;
    
    CLSASSERT(connReq != NULL);
    if (connReq == NULL)
      return;

    correlator = connReq->fCorrelator;
    
    connCnf = (ConnectCnf_t*) connReq;

    connCnf->fCLSIHdr.fHeaderLength = CLSIGetHdrSize(CLS_CONNECT | CLS_CONFIRM);
    connCnf->fCLSIHdr.fPrimitiveCode = CLS_CONNECT | CLS_CONFIRM;
    connCnf->fCLSIHdr.fDataSize = sizeof(ConnectCnf_t) - sizeof(ConnectCnfh_t);
    connCnf->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    connCnf->fCLSIHdr.fIdentifierType = CLS_U_CEP_ID;
    connCnf->fCLSIHdr.fIdentifier = this->fUCepId;
 
    CLSIMsgSetReturnCode((TCLSIMsg*) connCnf, pb->fResultError);

    /* If we had modify_flags, report results, here */

    connCnf->fVersion = CLSI_VERSION;
    connCnf->fCorrelator = correlator;
    connCnf->fRifLength = pb->fParameters.fDlcConnect.fDlcMod.fLlcReqOpenStn.fRifLength;
    if (connCnf->fRifLength) {
       memcpy(&connCnf->fRif[0],
             &pb->fParameters.fDlcConnect.fDlcMod.fLlcReqOpenStn.fRif[0],
             connCnf->fRifLength);
    }

    CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) connCnf);
}

void CLSCepConnectedInd (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    ConnectedInd_t* connInd;
    hword reservedSize;

    /* This is happening because we received an RR or the like. We */
    /* should always be able to reuse the packet that the frame was */
    /* received in to build the CLSI message */

    if (pb->fPakPtr != NULL)
    {
	connInd = (ConnectedInd_t*) 
	    CLSIMsgCreateFromPak(pb->fPakPtr,
				 CLS_CONNECTED | CLS_INDICATION,
				 CLS_LLC_LAYER,
				 CLS_U_CEP_ID,
				 this->fUCepId,
				 0,
				 &reservedSize);
	pb->fPakPtr = NULL;
    }
    else
    {
	connInd = (ConnectedInd_t*) 
	    CLSIMsgCreateHeader(CLS_CONNECTED | CLS_INDICATION,
				sizeof(ConnectedInd_t) - sizeof(ConnectedIndh_t),
				CLS_LLC_LAYER,
				CLS_U_CEP_ID,
				this->fUCepId);
    }
    
    CLSASSERT(connInd != NULL);
    if (connInd == NULL)
        return;

    connInd->fCLSIHdr.fActionCode = 0;
    connInd->fVersion = CLSI_VERSION;
    connInd->fCorrelator = 0;
    CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) connInd);
    
}

void CLSCepDisconnectInd (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    if (this->fCepState == kCepStateReqOpnPending) {
	/*
	 * Instead of a disconnect.ind, we want to send a
	 * req_opnstn.confirm to the DLU.
	 */
	TCLSDlu* ourDlu;
	ReqOpnStnReq_t* reqOpnStnReq;
	ReqOpnStnCnf_t* reqOpnStnCnf;
	word correlator;
	
	CLSASSERT(this->fRequest != NULL);

	ourDlu = this->fDlu;
	reqOpnStnReq = (ReqOpnStnReq_t*) this->fRequest;
	this->fRequest = NULL;
	correlator = reqOpnStnReq->fCorrelator;
    
	reqOpnStnCnf = (ReqOpnStnCnf_t*) reqOpnStnReq;
	CLSIMsgSetPrimitiveType((TCLSIMsg*) reqOpnStnCnf, CLS_CONFIRM);
	CLSIMsgSetReturnCode((TCLSIMsg*) reqOpnStnCnf, CLS_INTERFACE_DOWN);
	CLSIMsgSetDestination((TCLSIMsg*) reqOpnStnCnf, CLS_U_CEP_ID, this->fUCepId);

	reqOpnStnCnf->fVersion = CLSI_VERSION;
	reqOpnStnCnf->fCorrelator = correlator;

    	CLSDluRemoveCep(ourDlu, this);
	this->fNeedsDLCClose = TRUE;
	CLSCepDestroy(this);
	CLSDluAddPendingMsg(ourDlu, (TCLSIMsg*) reqOpnStnCnf);
    } else {
	this->fNeedsDisconnectInd = TRUE;

        switch (pb->fResultError) {
        case CLS_OK:
            this->fDisconnectReason = CLS_NORMAL_DISC;  break;
        case CLS_INACTIVITY_TIMEOUT:
            this->fDisconnectReason = CLS_TIMEOUT_DISC;  break;
        default:
            this->fDisconnectReason = CLS_ERROR_DISC;  break;
        }

	CLSCepSendDisconnectInd(this);
    }
}

void CLSCepSendDisconnectInd(TCLSCep* this)
{
    DisconnectInd_t* discInd;
    
    if (this->fNeedsDisconnectInd) 
    {
	discInd = (DisconnectInd_t*) 
	    CLSIMsgCreateHeader(CLS_DISCONNECT | CLS_INDICATION,
				sizeof(DisconnectInd_t) -
				sizeof(DisconnectIndh_t),
				CLS_LLC_LAYER,
				CLS_U_CEP_ID,
				this->fUCepId);
	if (discInd != NULL)
	{
	    this->fNeedsDisconnectInd = FALSE;
	    discInd->fVersion = CLSI_VERSION;
	    discInd->fCorrelator = 0;

	    discInd->fCLSIHdr.fLinkStatus = 
		this->fDisconnectReason;

	    CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) discInd);
	}
	else
	    CLSDluSetWorkToDo(this->fDlu);
    }
}

void CLSCepDisconnectCnf (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    DisconnectReq_t* discReq;
    DisconnectCnf_t* discCnf;
    word correlator;
    
    CLSASSERT(this->fCepState == kCepStateDisconnectReqPending);
    this->fCepState = kCepStateOpen;

    /*
     * If we are not holding a CLSI message to use to send the Disc.cnf,
     * then return. Somehow, between Disc.req and Disc.cnf the message
     * the CEP was holding was freed.
     */
    CLSASSERT(this->fRequest != NULL);
    if (this->fRequest == NULL)
        return;

    discReq = (DisconnectReq_t*) this->fRequest;
    this->fRequest = NULL;
    correlator = discReq->fCorrelator;
    
    discCnf = (DisconnectCnf_t*) discReq;
    discCnf->fCLSIHdr.fHeaderLength = CLSIGetHdrSize(CLS_DISCONNECT | CLS_CONFIRM);
    discCnf->fCLSIHdr.fPrimitiveCode = CLS_DISCONNECT | CLS_CONFIRM;
    discCnf->fCLSIHdr.fDataSize = sizeof(DisconnectCnf_t) - sizeof(DisconnectCnfh_t);
    discCnf->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    discCnf->fCLSIHdr.fIdentifierType = CLS_U_CEP_ID;
    discCnf->fCLSIHdr.fIdentifier = this->fUCepId;
 
    CLSIMsgSetReturnCode((TCLSIMsg*) discCnf, pb->fResultError);

    discCnf->fVersion = CLSI_VERSION;
    discCnf->fCorrelator = correlator;

    CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) discCnf);
}

void CLSCepPongCnf (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    PongReq_t* pongReq;
    PongCnf_t* pongCnf;
    word correlator;
    
    /*
     * The PongReq can be null in a race condition where
     * CLSCepDestroy has been called, but the DLC has not had a
     * chance to completely clean up yet.  If this happens there's
     * no need to send PongCnf since cep is going down
     */     
    if (this->fPongReq != NULL) 
    {
        pongReq = (PongReq_t*) this->fPongReq;
	this->fPongReq = NULL;

	correlator = pongReq->fCorrelator;
    
	pongCnf = (PongCnf_t*) pongReq;
	pongCnf->fCLSIHdr.fHeaderLength = CLSIGetHdrSize(CLS_PONG | CLS_CONFIRM);
	pongCnf->fCLSIHdr.fPrimitiveCode = CLS_PONG | CLS_CONFIRM;
	pongCnf->fCLSIHdr.fDataSize = sizeof(PongCnf_t) - sizeof(PongCnfh_t);
	pongCnf->fCLSIHdr.fSelector = CLS_LLC_LAYER;
	pongCnf->fCLSIHdr.fIdentifierType = CLS_U_CEP_ID;
	pongCnf->fCLSIHdr.fIdentifier = this->fDataUCepId;
	pongCnf->fCLSIHdr.fNws = pb->fParameters.fNextWindowSize;
 
	CLSIMsgSetReturnCode((TCLSIMsg*) pongCnf, pb->fResultError);

	pongCnf->fVersion = CLSI_VERSION;
	pongCnf->fCorrelator = correlator;

	CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) pongCnf);
    }
}

void CLSCepDataInd (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    paktype* pakPtr;
    DataInd_t* dataInd;
    hword reservedSize;

    CLSASSERT_FATAL(CLSCepCheck(this));

    pakPtr = pb->fPakPtr;
    pb->fPakPtr = NULL;
    
    CLSASSERT_FATAL(pakPtr != NULL);
    CLSASSERT(pakPtr->datagramsize != 0);
    dataInd = (DataInd_t*) CLSIMsgCreateFromPak(pakPtr, 
						CLS_DATA | CLS_INDICATION,
						CLS_LLC_LAYER, 
						CLS_U_CEP_ID,
						this->fDataUCepId, 
						0,
						&reservedSize);
    CLSASSERT(dataInd != NULL);

    dataInd->fVersion = CLSI_VERSION;
    dataInd->fCorrelator = 0;
    dataInd->fReserveLength = reservedSize;
    dataInd->fDataLength = pb->fLoadLength;

    if (this->fDataIndBlocked) {
        /*
         * Either the DLU has issued a FLOW STOP or
         * CLSCepFlowReq() is already unloading the fDataIndQueue.
         */
        p_enqueue(&this->fDataIndQueue, dataInd);
    } else if (!QUEUEEMPTY(&this->fDataIndQueue)) {
        /*
         * fDataIndQueue contains paks. Add new pak to tail
         * and try to unload in order.
         */
      p_enqueue(&this->fDataIndQueue, dataInd);
        while ((!QUEUEEMPTY(&this->fDataIndQueue)) &&
               (!reg_invoke_ucep_cantput(
                    (uint)this->fDlu->fReadQ,this->fDataUCepId,
	            QUEUEEMPTY(&this->fDataIndQueue)))) {
            TCLSIMsg* aMsg = p_dequeue(&this->fDataIndQueue);
            CLSDluAddPendingMsg(this->fDlu, aMsg);
    }
    } else if (reg_invoke_ucep_cantput(
                  (uint)this->fDlu->fReadQ,this->fDataUCepId,
                  QUEUEEMPTY(&this->fDataIndQueue))) {
        /*
         * If the DLU will not accept the pak, queue it.
         */
        p_enqueue(&this->fDataIndQueue, dataInd);
    } else {
	CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) dataInd);
    }
}

void CLSCepUDataInd (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    paktype* pakPtr;
    UDataInd_t* udataInd;
    hword reservedSize;

    CLSASSERT_FATAL(CLSCepCheck(this));

    pakPtr = pb->fPakPtr;
    pb->fPakPtr = NULL;


    CLSASSERT_FATAL(pakPtr != NULL);
    CLSASSERT(pakPtr->datagramsize != 0);
    udataInd = (UDataInd_t*) CLSIMsgCreateFromPak(pakPtr, 
						  CLS_UDATA |
						  CLS_INDICATION,
						  CLS_LLC_LAYER, 
						  CLS_U_CEP_ID,
						  this->fDataUCepId, 
						  0,
						  &reservedSize);

    CLSASSERT(udataInd != NULL);

    udataInd->fVersion = CLSI_VERSION;
    udataInd->fCorrelator = 0;
    udataInd->fReserveLength = reservedSize;
    udataInd->fUDataLength = pb->fLoadLength;
    udataInd->fUDataPtr = pakPtr->network_start;
    if (this->fDataIndBlocked) {
	p_enqueue(&this->fDataIndQueue, udataInd);
    } else {
	CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) udataInd);
    }
}

void CLSCepDataCnf (TCLSCep* this, ClsDlcParamBlockT* pb)
{
}

void CLSCepFlowInd (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    this->fNeedsFlowInd = TRUE;
    this->fFlowLevel = pb->fParameters.fFlowLevel;
    CLSCepSendFlowInd(this);
}

void CLSCepSendFlowInd(TCLSCep* this)
{
    FlowInd_t* flowInd;
    
    if (this->fNeedsFlowInd) {
	flowInd = (FlowInd_t*) 
	    CLSIMsgCreateHeader(CLS_FLOW | CLS_INDICATION,
				sizeof(FlowInd_t) - sizeof(FlowIndh_t),
				CLS_LLC_LAYER,
				CLS_U_CEP_ID,
				this->fUCepId);
	if (flowInd == NULL) {
	    /* We want to send a FLOW.ind to the DLU, but we can't get the */
	    /* memory that we need to build the message. Try again */
	    /* later from the background process. */
	    CLSDluSetWorkToDo(this->fDlu);
	    return;
	}
	
        flowInd->fCLSIHdr.fFlowLevel = this->fFlowLevel;
        flowInd->fVersion = CLSI_VERSION;
        flowInd->fCorrelator = 0;
        CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) flowInd);
        this->fNeedsFlowInd = FALSE;
    }
    
}

void CLSCepCloseStnCnf (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    CloseStnCnf_t* closeCnf;
    CloseStnReq_t* closeReq;
    TCLSDlu* theDlu;
    word correlator;
    
    CLSASSERT(this->fCepState == kCepStateCloseStnPending);

    closeReq = (CloseStnReq_t*) this->fRequest;

    if (closeReq == NULL)		/* This happens on CLSClose() */
	return;

    this->fRequest = NULL;  /* Ensure CloseStnCnf buffer will not be freed */

    correlator = closeReq->fCorrelator;
    
    closeCnf = (CloseStnCnf_t*) closeReq;
    
    closeCnf->fCLSIHdr.fHeaderLength = CLSIGetHdrSize(CLS_CLOSE_STN | CLS_CONFIRM);
    closeCnf->fCLSIHdr.fPrimitiveCode = CLS_CLOSE_STN | CLS_CONFIRM;
    closeCnf->fCLSIHdr.fDataSize = sizeof(CloseStnCnf_t) - sizeof(CloseStnCnfh_t);
    closeCnf->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    closeCnf->fCLSIHdr.fIdentifierType = CLS_U_CEP_ID;
    closeCnf->fCLSIHdr.fIdentifier = this->fDataUCepId;

    CLSIMsgSetReturnCode((TCLSIMsg*) closeCnf, pb->fResultError);
    closeCnf->fVersion = CLSI_VERSION;
    closeCnf->fCorrelator = correlator;
	
    theDlu = this->fDlu;
	
    this->fNeedsDLCClose = FALSE;	/* So we don't close again */
    CLSCepDestroy(CLSDluRemoveCep(theDlu, this));
	
    CLSDluAddPendingMsg(theDlu, (TCLSIMsg*) closeCnf);
}


void CLSSapTestStnReq(TCLSSap* this, TestStnReq_t* theMsg)
{
    ClsDlcParamBlockT pb;

    pb.fLoadLength =  theMsg->fCLSIHdr.fDataSize - 
	              (offsetof(TestStnReq_t, fTEST) - sizeof(TestStnReqh_t)) - 
		      theMsg->fReserveLength;

    pb.fPakPtr = CLSIMsgGetPak((TCLSIMsg*) theMsg);
    pb.fRawData = &theMsg->fTEST[0] + theMsg->fReserveLength;

    CLSIPortIdCopy(&(this->fPortPtr->fPortId), (CLSPortID *)&pb.fPortId);
    pb.fRawPortType = this->fPortPtr->fDLCType;
    pb.fParameters.fDlcTestStn.fClsUSapId = (ClsUSapIdT) this->fPSapId;
    pb.fParameters.fDlcTestStn.fDlcPSapId = this->fDlcPSapId;

    pb.fParameters.fDlcTestStn.fRSap = theMsg->fRSap;
    pb.fParameters.fDlcTestStn.fLSap = theMsg->fLSap;
    pb.fParameters.fDlcTestStn.fRifLength = theMsg->fRifLength;
    if(theMsg->fRifLength)
	bcopy(theMsg->fRif, pb.fParameters.fDlcTestStn.fRif, theMsg->fRifLength);
    ieee_copy(theMsg->fLMac, pb.fParameters.fDlcTestStn.fLMac);
    ieee_copy(theMsg->fRMac, pb.fParameters.fDlcTestStn.fRMac);

    pb.fParameters.fDlcTestStn.fTestT = TEST_COMMAND;

    reg_invoke_dlc_wput(this->fPortPtr->fDLCType, CLS_NO_CEP, DLC_TEST_STN_REQ, &pb);
}

void CLSSapTestStnRsp(TCLSSap* this, TestStnRsp_t* theMsg)
{
    ClsDlcParamBlockT pb;

    pb.fLoadLength =  theMsg->fCLSIHdr.fDataSize - (offsetof(TestStnRsp_t, fTEST)-offsetof(TestStnRsp_t, fVersion));
        
    pb.fPakPtr = CLSIMsgGetPak((TCLSIMsg*) theMsg);
    pb.fRawData = &theMsg->fTEST[0] + theMsg->fReserveLength;

    CLSIPortIdCopy(&(this->fPortPtr->fPortId), (CLSPortID *)&pb.fPortId);
    pb.fRawPortType = this->fPortPtr->fDLCType;

    pb.fParameters.fDlcTestStn.fRSap = theMsg->fRSap;
    pb.fParameters.fDlcTestStn.fLSap = theMsg->fLSap;
    pb.fParameters.fDlcTestStn.fRifLength = theMsg->fRifLength;
    pb.fParameters.fDlcTestStn.fDlcPSapId = this->fDlcPSapId;
    if(theMsg->fRifLength)
	bcopy(theMsg->fRif, pb.fParameters.fDlcTestStn.fRif, theMsg->fRifLength);
    ieee_copy(theMsg->fLMac, pb.fParameters.fDlcTestStn.fLMac);
    ieee_copy(theMsg->fRMac, pb.fParameters.fDlcTestStn.fRMac);

    pb.fParameters.fDlcTestStn.fTestT = TEST_RESPONSE;

    reg_invoke_dlc_wput(this->fPortPtr->fDLCType, CLS_NO_CEP, DLC_TEST_STN_RSP, &pb);
}

void CLSSapIdStnReq(TCLSSap* this, IdStnReq_t* theMsg)
{
    ClsDlcParamBlockT pb;

    pb.fLoadLength =  theMsg->fCLSIHdr.fDataSize - (offsetof(IdStnReq_t, fXID) - sizeof(IdStnReqh_t)) - theMsg->fReserveLength;

    pb.fPakPtr = CLSIMsgGetPak((TCLSIMsg*) theMsg);
    pb.fRawData = &theMsg->fXID[0] + theMsg->fReserveLength;
    CLSIPortIdCopy(&(this->fPortPtr->fPortId), (CLSPortID *)&pb.fPortId);
    pb.fRawPortType = this->fPortPtr->fDLCType;
    pb.fParameters.fDlcIdStn.fClsUSapId = (ClsUSapIdT) this->fPSapId;
    pb.fParameters.fDlcIdStn.fDlcPSapId = this->fDlcPSapId;
    pb.fParameters.fDlcIdStn.fRifLength = theMsg->fRifLength;
    if(theMsg->fRifLength)
	bcopy(theMsg->fRif, pb.fParameters.fDlcIdStn.fRif, theMsg->fRifLength);
    ieee_copy(theMsg->fLMac, pb.fParameters.fDlcIdStn.fLMac);
    ieee_copy(theMsg->fRMac, pb.fParameters.fDlcIdStn.fRMac);

    pb.fParameters.fDlcIdStn.fXidT = XID_COMMAND;

    reg_invoke_dlc_wput(this->fPortPtr->fDLCType, CLS_NO_CEP, DLC_ID_STN_REQ, &pb);
}

void CLSSapIdStnRsp(TCLSSap* this, IdStnRsp_t* theMsg)
{
    ClsDlcParamBlockT pb;

    /*
     * pb.fLoadLength =  theMsg->fCLSIHdr.fDataSize - (offsetof(IdStnRsp_t, fXID) - sizeof(IdStnRsph_t)) - theMsg->fReserveLength;
     */

    pb.fLoadLength =  theMsg->fCLSIHdr.fDataSize
	- (offsetof(IdStnRsp_t, fXID) - offsetof(IdStnRsp_t, fVersion));

    pb.fPakPtr = CLSIMsgGetPak((TCLSIMsg*) theMsg);
    pb.fRawData = &theMsg->fXID[0] + theMsg->fReserveLength;

    CLSIPortIdCopy(&(this->fPortPtr->fPortId), (CLSPortID *)&pb.fPortId);
    pb.fRawPortType = this->fPortPtr->fDLCType;
    pb.fParameters.fDlcIdStn.fClsUSapId = (ClsUSapIdT) this->fPSapId;
    pb.fParameters.fDlcIdStn.fDlcPSapId = this->fDlcPSapId;
    pb.fParameters.fDlcIdStn.fRifLength = theMsg->fRifLength;
    if(theMsg->fRifLength)
	bcopy(theMsg->fRif, pb.fParameters.fDlcIdStn.fRif, theMsg->fRifLength);

    pb.fParameters.fDlcIdStn.fRSap = theMsg->fRSap;
    pb.fParameters.fDlcIdStn.fLSap = theMsg->fLSap;
    ieee_copy(theMsg->fLMac, pb.fParameters.fDlcIdStn.fLMac);
    ieee_copy(theMsg->fRMac, pb.fParameters.fDlcIdStn.fRMac);
    pb.fParameters.fDlcIdStn.fXidT = XID_RESPONSE;

    reg_invoke_dlc_wput(this->fPortPtr->fDLCType, CLS_NO_CEP, DLC_ID_STN_RSP, &pb);

}

void CLSSapUDataStnReq(TCLSSap* this, UDataStnReq_t* theMsg)
{
    ClsDlcParamBlockT pb;

    pb.fLoadLength =  theMsg->fCLSIHdr.fDataSize - (offsetof(UDataStnReq_t, fUData) - sizeof(UDataStnReqh_t)) - theMsg->fReserveLength;
                
    pb.fPakPtr = CLSIMsgGetPak((TCLSIMsg*) theMsg);
    pb.fRawData = &theMsg->fUData[0] + theMsg->fReserveLength;
    CLSIPortIdCopy(&(this->fPortPtr->fPortId), (CLSPortID *)&pb.fPortId);
    pb.fRawPortType = this->fPortPtr->fDLCType;
    pb.fParameters.fDlcUdataStn.fClsUSapId = (ClsUSapIdT) this->fPSapId;
    pb.fParameters.fDlcUdataStn.fDlcPSapId = this->fDlcPSapId;
    pb.fParameters.fDlcUdataStn.fRifLength = theMsg->fRifLength;
    pb.fParameters.fDlcUdataStn.fRSap = theMsg->fRSap;
    pb.fParameters.fDlcUdataStn.fLSap = theMsg->fLSap;
    if(theMsg->fRifLength)
	bcopy(theMsg->fRif, pb.fParameters.fDlcUdataStn.fRif, theMsg->fRifLength);
    ieee_copy(theMsg->fLMac, pb.fParameters.fDlcUdataStn.fLMac);
    ieee_copy(theMsg->fRMac, pb.fParameters.fDlcUdataStn.fRMac);

    reg_invoke_dlc_wput(this->fPortPtr->fDLCType, CLS_NO_CEP, DLC_UDATA_STN_REQ, &pb);
}

void CLSCepModifyStnReq (TCLSCep* this, ModifyStnReq_t* theMsg)
{
    ClsDlcParamBlockT pb;

    this->fRequest = (TCLSIMsg*) theMsg;
    this->fCepState = kCepStateModifyStnReqPending;

    pb.fParameters.fDlcConnect.fModifyFlags = theMsg->fCLSIHdr.fModifyFlags;
    pb.fParameters.fDlcConnect.fQueuingType = theMsg->fQueuingType;
    pb.fParameters.fDlcConnect.fQueuingParameter = theMsg->fQueuingParameter;

    if (theMsg->fCLSIHdr.fModifyFlags != 0) {
	switch (CLSSapGetType(this->fSap)) {
	case CLS_RSRB:
	case CLS_TBRIDGE:		/* intentional fall through, behaves as CLS_LLC */
	case CLS_FRAME_RELAY:
	case CLS_ATM_1483:
        case CLS_PPP:
	case CLS_LLC:
        case CLS_NCIA:
	    {
		ReqOpnStnLLCParms_t* fromDLUParms;
		LlcReqOpenStnT* toDLCParms;

		fromDLUParms = (ReqOpnStnLLCParms_t*) &theMsg->fParms[0];
		toDLCParms =
		    &pb.fParameters.fDlcConnect.fDlcMod.fLlcReqOpenStn;

		/*
		 * We copy all these parameters into the control
		 * block, but in reality, dlcllc.c only supports
		 * fMaxOut. None of the others are really interesting in
		 * the current implementation.
		 */
		toDLCParms->fMaxout = fromDLUParms->fMaxOut;
		toDLCParms->fMaxin = fromDLUParms->fMaxIn;
		toDLCParms->fTimerT1 = fromDLUParms->fT1;
		toDLCParms->fMaxTries = fromDLUParms->fMaxRetryCount;
	    }
	    break;
	case CLS_SDLC:
	    {
		ReqOpnStnSDLCParms_t* fromDLUParms;
		SdlcReqOpenStnT* toDLCParms;

		fromDLUParms = (ReqOpnStnSDLCParms_t*) &theMsg->fParms[0];
		toDLCParms =
		    &pb.fParameters.fDlcConnect.fDlcMod.fSdlcReqOpenStn;
		/*
		 * Current dlcsdlc.c does not support modification
		 * of parameters. This is an area for further
		 * work.
		 * SHB 2/8/95
		 */
	    }
	    break;
	case CLS_QLLC:
	    {
		ReqOpnStnQLLCParms_t* fromDLUParms;
		QLLCReqOpenStnT* toDLCParms;

		fromDLUParms = (ReqOpnStnQLLCParms_t*) &theMsg->fParms[0];
		toDLCParms =
		    &pb.fParameters.fDlcConnect.fDlcMod.fQllcReqOpenStn;
	    }
	    break;
	case CLS_VDLC:
            {
		ReqOpnStnVDLCParms_t* fromDLUParms;
		VdlcReqOpenStnT* toDLCParms;

		fromDLUParms = (ReqOpnStnVDLCParms_t*) &theMsg->fParms[0];
		toDLCParms =
		    &pb.fParameters.fDlcConnect.fDlcMod.fVdlcReqOpenStn;
            }
	    break;
	default:
	    CLSASSERT(FALSE);
	    break;
	}
    }

    reg_invoke_dlc_wput(CLSSapGetType(this->fSap), 
			this->fDLCPCepId, 
			DLC_MODIFY_STN_REQ, 
			&pb);
}

void CLSCepModifyStnCnf (TCLSCep* this, ClsDlcParamBlockT* pb)
{
    ModifyStnReq_t* modStnReq;
    ModifyStnCnf_t* modStnCnf;
    word correlator;
    
    CLSASSERT(this->fCepState == kCepStateModifyStnReqPending);
    this->fCepState = kCepStateOpen;
    modStnReq = (ModifyStnReq_t*) this->fRequest;
    this->fRequest = NULL;
    
    CLSASSERT(modStnReq != NULL);

    correlator = modStnReq->fCorrelator;
    
    modStnCnf = (ModifyStnCnf_t*) modStnReq;

    modStnCnf->fCLSIHdr.fHeaderLength = CLSIGetHdrSize(CLS_MODIFY_STN | CLS_CONFIRM);
    modStnCnf->fCLSIHdr.fPrimitiveCode = CLS_MODIFY_STN | CLS_CONFIRM;
    modStnCnf->fCLSIHdr.fDataSize = sizeof(ModifyStnCnf_t) - sizeof(ModifyStnCnfh_t);
    modStnCnf->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    modStnCnf->fCLSIHdr.fIdentifierType = CLS_U_CEP_ID;
    modStnCnf->fCLSIHdr.fIdentifier = this->fUCepId;
 
    /* 
     * If we had modify_flags, report results, here
     */
    CLSIMsgSetReturnCode((TCLSIMsg*) modStnCnf, pb->fResultError);

    modStnCnf->fVersion = CLSI_VERSION;
    modStnCnf->fCorrelator = correlator;

    CLSDluAddPendingMsg(this->fDlu, (TCLSIMsg*) modStnCnf);
}
