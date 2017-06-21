/* $Id: dlc.c,v 3.5.12.4 1996/08/28 12:39:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlc.c,v $
 *------------------------------------------------------------------
 * dlc.c
 *
 * September 1994, Anna Lam
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Common routines for data link providers using CLS.
 *------------------------------------------------------------------
 * $Log: dlc.c,v $
 * Revision 3.5.12.4  1996/08/28  12:39:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.12.3  1996/08/07  08:57:36  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.12.2  1996/05/17  10:42:15  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.3  1996/04/11  14:36:05  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.2  1996/04/03  13:54:29  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.2.1  1996/03/28  08:19:28  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.5.12.1  1996/03/18  19:10:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.3  1996/03/13  01:14:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.12.2  1996/03/07  08:41:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.12.1  1996/02/20  00:21:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  20:49:29  hampton
 * Clean up includes of gdb.h and references to breakpoint.  [CSCdi48844]
 *
 * Revision 3.4  1996/01/25  11:16:34  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.3  1996/01/04  19:19:58  sberl
 * CSCdi45974:  router crashed at CepPongRequest after running aping
 * overnight
 * Fix race condition between process and interrupt level in handling of
 * Pong messages.
 *
 * Revision 3.2.4.1  1995/12/17  09:50:04  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.2  1995/11/17  08:55:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:53:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/09  23:03:14  fbordona
 * CSCdi38407:  Test Request to SDLC port not bounced correctly
 *              Copy Test.Req Data into Test.Ind (response) Msg
 *
 * Revision 2.4  1995/08/09  13:04:33  fbordona
 * CSCdi38407:  Test Request to SDLC port not bounced correctly
 *
 * Revision 2.3  1995/07/19  20:58:24  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add support for Activate/Deactivate Ring
 *
 * Revision 2.2  1995/07/02  05:15:30  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.1  1995/06/07  20:20:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../cls/msg_dlc.c"		/* Not a typo, see logger.h */
#include "cls_private.h"
#include "packet.h"
#include "../h/subsys.h"
#include "../if/if_vidb.h"	/* prototypes for vidb search fn's */
#include "../if/fddi.h"
#include "../if/rif.h"
#include "../cls/dlc_registry.regc"

static gCLSbreakOnAssert = FALSE;
static ulong cls_assertline;
static char* cls_assertfile;
static boolean cls_badassert;

/*
 * Forward declarations
 */
void dlc_wput(int clsDLCPortType, DlcPCepIdT pCepId,
	       DlcOpcodeT opCode, ClsDlcParamBlockT *clsParamBlock);

void cls_assert(char* file,int line, char *exp, boolean fatal);
static void dlc_default_wput(DlcPCepIdT pCepId, DlcOpcodeT opCode, ClsDlcParamBlockT *clsParamBlock);

static void dlc_init (subsystype *subsys)
{
    cls_assertline = 0;
    cls_assertfile = NULL;
    cls_badassert = 0;
    gCLSbreakOnAssert = FALSE;

    /*
     * Input routine from CLS
     */
    reg_add_default_dlc_wput(dlc_default_wput, "dlc_default_wput");

    /*
     * Service routines
     */
    reg_add_cls_assert(cls_assert, "cls_assert");
}

/*
 * d l c _ d e f a u l t _ w p u t ( )
 *
 */
static void dlc_default_wput(DlcPCepIdT pCepId, DlcOpcodeT opCode, ClsDlcParamBlockT *clsParamBlock)
{
    /*
     * This routine could be modified to return an error, but 
     * it seems that the error message is sufficient.
     */
    errmsg(&msgsym(WPUTERR, DLC), opCode);
}


/*
 * d l c _ r p u t ( )
 *
 */
void dlc_rput (ClsDlcParamBlockT *clsBlock, 
	       ClsOpcodeT opCode, 
	       ClsErrorCodeT error) 
{
    /* 
     * Note assumptions made about valid input block
     */

    clsBlock->fResultError = error;
    clsBlock->fPakPtr = NULL;
    reg_invoke_cls_rput((ClsUCepIdT) CLS_NO_CEP, opCode, clsBlock);
    return;
}

/* 
 * DlcBuildParmBlock
 *
 * Populate the popular fields in a ClsDlcParamBlock for input to
 * cls_rput.
 *
 * Code extracted from dlcllc.c
 */
 
 
void DlcBuildParmBlock (
		   DlcPCepIdT 		pCepId,
		   ClsUCepIdT 		uCepId,
		   ClsOpcodeT 		opcode,
		   ClsDlcParamBlockT 	*paramBlock,
		   ClsErrorCodeT 	ClsDlcErrorCode,
		   paktype		*pak)
{
    paramBlock->fResultError = ClsDlcErrorCode;
    /*
     * If fPakPtr is not equal to NULL, then CLS will assume that it
     * contains a paktype that it can use to build a CLSI message.
     *
     * In the future, other opcodes may provide a paktype for CLS to
     * optionally use. If CLS finds a non-NULL value here, it will
     * assume responsibility for releasing the paktype when it is
     * finished using it.
     *
     * For now, the only opcodes that should have a non-NULL value for
     * this field are CLS_DATA_IND, CLS_ID_STN_IND, and
     * CLS_ID_IND. The cases for these opcodes handle setting the
     * field. 
     */
    paramBlock->fPakPtr = NULL;

    switch (opcode) {
    case CLS_NO_OP:
	break;				/* invalid */
    case CLS_ENABLE_CNF:
    case CLS_DISABLE_CNF:
    	CLSASSERT(FALSE);
	break;				/* not a cep message */

	/* SAP management operations always run right through, so we can
	 * reuse the parameter block that was sent down (AND it has the
	 * correct ClsUSapId in it)
	 */
    case CLS_ACTIVATE_SAP_CNF:
    case CLS_MODIFY_SAP_CNF:
    	CLSASSERT(FALSE);
	break;				/* not a cep message */

    case CLS_DEACTIVATE_SAP_CNF:
    	CLSASSERT(FALSE);
	break;				/* not a cep message */

    case CLS_REQ_OPNSTN_CNF:
    case CLS_MODIFY_STN_CNF:
	break;	

    case CLS_CLOSE_STN_CNF:
	break;	

    case CLS_TEST_IND:
	break;				/* handled elsewhere */

    case CLS_ID_IND:
	paramBlock->fPakPtr = pak;
	break;		

    case CLS_CONNECT_IND:
    case CLS_CONNECT_CNF:
	paramBlock->fParameters.fDlcConnect.fDlcPCepId = pCepId;
	paramBlock->fParameters.fDlcConnect.fClsUCepId = uCepId;
	break;

    case CLS_DISC_IND:
    case CLS_DISC_CNF:
	break;


    case CLS_DATA_IND:
        paramBlock->fLoadLength =  pak->datagramsize;
	paramBlock->fPakPtr = pak;
	break;

    case CLS_ID_STN_IND:
	paramBlock->fPakPtr = pak;
	break;

    case CLS_TEST_STN_IND:
    case CLS_CONNECT_STN_IND:
    case CLS_ACTIVATE_RING_CNF:
    case CLS_DEACTIVATE_RING_CNF:
        break;                          /* handled elsewhere */

    case CLS_UDATA_STN_IND:
    case CLS_UDATA_IND:
        paramBlock->fPakPtr = pak;
        break;

    case CLS_DATA_CNF:
	/* need something here for error reply to DLC_DATA_REQ */
	break;

    case CLS_PONG_CNF:
	CLSASSERT(!"Inappropriate call to build Pong Confirm");
	break;
    case CLS_FLOW_IND:
        /*
         * Convert LLC to CLS congestion level.
         */
	CLSASSERT(!"Inappropriate call to build Flow Ind");
	break;
    case CLS_CONNECTED_IND:
	break;
    }

    return;
}

/* Llc1GenUFrame
 *
 * based on llc1_gen_explorer, 
 * this routine will build a bare-bones LLC1 uframe
 *
 * This is useful for building TEST and XID packets
 */

paktype *Llc1GenUFrame (uchar *daddr, uchar *saddr,
			uchar dsap, uchar ssap, uchar control,
			hwidbtype *interface)
{
    tring_hdr *trframe;
    fddi_hdr *fdframe;
    mac_hdr *macframe;
    llc1_test_t *test;
    paktype *pak;

    if ((pak = getbuffer(sizeof(llc1_test_t))) == NULL) {
        return(NULL);
    }

    /*
     * Write the MAC header
     */
    if (is_fddi(interface)) {
	fdframe = (fddi_hdr *)(pak->network_start - FDDI_ENCAPBYTES -
			       RC_NORD_LTH);
	fdframe->fc = FDDI_LLC_FC_BYTE;
	daddr[0] &= ~FDDIT_RII;
 	ieee_copy(daddr, fdframe->daddr);
	ieee_copy(saddr, fdframe->saddr);
	fdframe->saddr[0] |= (uchar)FDDIT_RII;
	macframe = (mac_hdr *)fdframe->daddr;
	pak->datagramstart = (uchar *) fdframe;
	pak->datagramsize  = SAP_HDRBYTES + FDDI_ENCAPBYTES + RC_NORD_LTH;
    } else { /* token ring */
	trframe = (tring_hdr *)(pak->network_start - TRING_ENCAPBYTES - 
				RC_NORD_LTH);
	trframe->ac = AC_PRI0;
	trframe->fc = FC_TYPE_LLC;
 	ieee_copy_sans_rii(daddr, trframe->daddr);
	ieee_copy(saddr, trframe->saddr);
	trframe->saddr[0] |= (uchar)TR_RII;
	macframe = (mac_hdr *)trframe->daddr;
	pak->datagramstart = (uchar *) trframe;
	pak->datagramsize  = SAP_HDRBYTES + TRING_ENCAPBYTES + RC_NORD_LTH;
    }

    /*
     * Write the RIF.  
     * Twould be nice to merge this routine with DlcBuildDirectedTest, which
     * requires an LLC2 session to exist. This means updating LNX and SDLLC
     * to "open" llc2 before exploring, which is cool for a number of reasons.
     * Then, one would make the following RIF code optional, for ethernet and
     * other non-srb uses.
     */

    macframe->data[0] = RC_DEF_ALLRINGS;
    macframe->data[1] = RC_TR_LFDEF;

    /*
     * for SDLLC limit max frame size to largest size given on interface.
     */
    if (interface->sdllc_active)
	macframe->data[1] = (macframe->data[1] & ~(RC_LFMASK)) |
	    interface->sdllc_largest_token_frame;

    /*
     * Write the rest of the TEST packet.
     */
    test = (llc1_test_t *)pak->network_start;
    test->control       = control | LLC1_P;      /* assume cmd/poll */
    test->dsap = dsap;
    test->ssap = ssap;
    pak->info_start    = (uchar *)test;
    pak->if_output     = interface->firstsw;
    pak->enctype       = interface->enctype;
    return(pak);
}

/* DlcSendUControlStnInd
 *
 * Routine to take a uframe, remodel it for CLS and pass it in.
 * This is a routine to be used when there is no CEP
 * to pass the info to.  The message goes to the SAP.
 *
 * common routine called by dlsw_work, and VSapSendUControlStnInd
 *
 * By making sure that we use common structures for ID_STN and for
 * TEST_STN, we can share code.
 *
 * So, although the code refers to TestStn, It works just as well
 * for IdStn (and probably for UDataStn
 */

void DlcSendUControlStnInd (ClsDlcParamBlockT *pb, paktype *pak, 
			uchar control, boolean isResponse,
			ClsUSapIdT uSap, DlcPCepIdT pCep)
{
    DlcTestStnT         *dlcTestStn;
    LlcTestStnT         *llcTestStn;
    sap_hdr             *llc1;
    ClsOpcodeT		opcode;
    
    switch (control) {
      default:
      	CLSASSERT(FALSE);
	return;
      case LLC1_XID:
        opcode = CLS_ID_STN_IND;
	break;
      case LLC1_TEST:
      	opcode = CLS_TEST_STN_IND;
	break;
    }

    dlcTestStn = &pb->fParameters.fDlcTestStn;
    /* 
     * give CLS parameters for future req open stn 
     */
    llcTestStn = &dlcTestStn->fDlcTest.fLlcTestStn;
    
    llcTestStn->fMacType = STATION_IEEE48;
    llcTestStn->fLMacLength = IEEEBYTES;
    llcTestStn->fLMacPtr = pak->addr_start;
    llcTestStn->fRMacLength = IEEEBYTES;
    llcTestStn->fRMacPtr = pak->addr_start + IEEEBYTES;
    llcTestStn->fRoutingLength = pak->riflen;
    llcTestStn->fRoutingPtr = pak->rif_start;
    
    llc1 = (sap_hdr *) pak->info_start;
    llcTestStn->fRSap = llc1->ssap & ~SAP_RESPONSE;
    llcTestStn->fLSap = llc1->dsap;
    /* 
     * Set network_start to point at start of test data 
     */
    pak->network_start = pak->info_start + SAP_HDRBYTES;
    dlcTestStn->fTESTLength = 
	pak->datagramsize -(pak->network_start - pak->datagramstart);
    dlcTestStn->fTESTPtr = pak->network_start;
    dlcTestStn->fTestT          = isResponse  ? TEST_RESPONSE: TEST_COMMAND;
    pb->fResultError    = CLS_OK;
    pb->fPakPtr         = pak;
    dlcTestStn->fClsUSapId = uSap;
    pb->fParameters.fDlcTestStn.fDlcPCepId = pCep;
    reg_invoke_cls_rput((ClsUCepIdT) CLS_NO_CEP, opcode, pb);
}

/*
 * Got some kind of reply for CLS about Request Open Station Request
 */
void DlcReqOpenStnConf (
    DlcPCepIdT		pCepId,
    ClsUCepIdT		uCepId,
    ClsErrorCodeT 	error)
{
    ClsDlcParamBlockT 	clsBlock;

    clsBlock.fResultError = error;
    clsBlock.fParameters.fDlcReqOpenStn.fClsUCepId = uCepId;

    clsBlock.fParameters.fDlcReqOpenStn.fDlcPCepId = pCepId;

    clsBlock.fPakPtr = NULL;
    reg_invoke_cls_rput(uCepId, CLS_REQ_OPNSTN_CNF, &clsBlock);
    return;
}

/*  DlcHandleTestReq
 *
 * Bounce it back (making the naive assumption that it matches
 * the partner
 */

void DlcHandleTestReq (ClsUCepIdT uCepId, ClsDlcParamBlockT *clsBlock)
{
    DlcTestT	*dlcTest;

    dlcTest = &clsBlock->fParameters.fDlcTest;
    clsBlock->fResultError = CLS_OK;
    dlcTest->fTestT = TEST_RESPONSE;
    reg_invoke_cls_rput(uCepId, CLS_TEST_IND, clsBlock);
}

/*
 * DlcBounceTestStnInd
 *
 * Bounce a response to a Test Station Request up to the CLS layer as
 * a Test Station Indication.
 *
 * This routine assumes that all sanity checks have already been made.
 * It also assumes that LMac, RMac, LSap, and RSap are valid.
 * It uses the LlcTestStnT as a generic representation */
 
void 
DlcBounceTestStnInd (DlcPCepIdT pCepId,
		     ClsDlcParamBlockT *clsBlock)
{
    DlcTestStnT	*dlcTestStn;
    LlcTestStnT	*llcTestStn;
    idbtype	*swidb;
    int		size;
    paktype	*pak;
    
    dlcTestStn = &clsBlock->fParameters.fDlcTestStn;
    llcTestStn = &dlcTestStn->fDlcTest.fLlcTestStn;
    pak = clsBlock->fPakPtr;
    swidb = DlcToIdb(&clsBlock->fPortId);

    size = TR_ACFC_SIZE + (2 * IEEEBYTES) + SAP_HDRBYTES +
	clsBlock->fLoadLength;

    pak->datagramsize = size;

    pak->datagramstart -= TR_ACFC_SIZE + (2 * IEEEBYTES);
    pak->addr_start = pak->datagramstart + TR_ACFC_SIZE;
    pak->info_start = pak->addr_start + (2 * IEEEBYTES);
    pak->network_start = pak->info_start + SAP_HDRBYTES;
    pak->rif_start = NULL;
    pak->riflen = 0;
    bcopy(clsBlock->fRawData, pak->network_start, clsBlock->fLoadLength);
    
    llcTestStn->fMacType = STATION_IEEE48;
    llcTestStn->fLMacLength = IEEEBYTES;
    llcTestStn->fLMacPtr = pak->addr_start;
    llcTestStn->fRMacPtr = pak->addr_start + IEEEBYTES;
    llcTestStn->fRoutingLength = 0;
    llcTestStn->fRoutingPtr = NULL;

    /* Swap addresses in packet */
    ieee_copy(dlcTestStn->fLMac, pak->addr_start);
    ieee_copy(dlcTestStn->fRMac, pak->addr_start + IEEEBYTES);

    /* transfer the SAP's between structs */
    llcTestStn->fLSap = dlcTestStn->fLSap & ~SAP_RESPONSE;   
    llcTestStn->fRSap = dlcTestStn->fRSap | SAP_RESPONSE;

     /* Set network_start to point at start of test data */
     pak->network_start = pak->info_start + SAP_HDRBYTES;
 
     dlcTestStn->fTESTLength = pak->datagramsize -(pak->network_start - pak->datagramstart);    
     dlcTestStn->fTESTPtr = pak->network_start;
 
     dlcTestStn->fTestT         = TEST_RESPONSE;
     dlcTestStn->fDlcPCepId     = pCepId;
     dlcTestStn->fClsUCepId 	= (ClsUCepIdT) CLS_NO_CEP;
 
     clsBlock->fResultError    = CLS_OK;
     clsBlock->fPakPtr         = pak;
     clsBlock->fPortId.type = CLS_P_TYPE_SWIDB;
     clsBlock->fPortId.fTypeInfo.swidb = swidb;

     reg_invoke_cls_rput((ClsUCepIdT) CLS_NO_CEP, CLS_TEST_STN_IND, clsBlock);
     return;
}

/* CepInit
 *
 * Initialize fields in the Cep
 *
 * For convenience everything gets initialized with ZERO;
 *
 * Any field where ZERO is not the correct initial state needs to get
 * adjusted.  CepUpdate does most of the work, but not if initial
 * parameters are NULL or ZERO
 *
 */

void CepInit (CepT *cep, queuetype *queue, ulong queueMax, 
	 uint lowWaterPct, uint highWaterPct, uint damping)
{
    memset(cep, 0, sizeof(CepT));

    cep->fCurrCongLevel = DLC_OQUEUE_LOW;
    cep->fPrevCongLevel = DLC_OQUEUE_LOW;

    CepUpdate(cep, queue, queueMax, 
	      lowWaterPct, highWaterPct, damping);
}

/* CepUpdate
 *
 * Update fields in the Cep;
 * A zero (or NULL) means don't touch it
 *
 * Use a "damping" zone around lo and hi water marks to avoid
 * excessive flow indications around marks.
 *
 * Constraints:
 *	LowWaterLowDamp must be above the empty mark, 
 *	and has been designed to be at least halfway up to the
 *	low water mark.
 *
 * 	HighWaterHighDamp may not be more the the maximum queue size.
 */

void CepUpdate (CepT *cep, queuetype *queue, ulong queueMax, 
		uint lowWaterPct, uint highWaterPct, uint damping)
{
#ifdef HOLDME
    if (uCepId != NO_CLS)
	cep->fUCepId = uCepId;
#endif

    if (queue != NULL)
	cep->fOqueue = queue;
    
    if (queueMax != 0)
	cep->fOqueueMax = queueMax;

    if (lowWaterPct != 0)
	cep->fLowWater = (cep->fOqueueMax * lowWaterPct)/100;

    if (highWaterPct != 0)
	cep->fHighWater = (cep->fOqueueMax * highWaterPct)/100;

    if (damping  != 0) {
	cep->fDamping = damping;
    } else {
	damping = cep->fDamping;
    }
    if (cep->fLowWater > 2 * damping)
	cep->fLowWaterLowDamp = cep->fLowWater - damping;
    else
	cep->fLowWaterLowDamp = cep->fLowWater;

    cep->fLowWaterHighDamp = cep->fLowWater + damping;

    cep->fHighWaterLowDamp = cep->fHighWater - damping;

    if (cep->fHighWater + damping > cep->fOqueueMax)
	cep->fHighWaterHighDamp = cep->fHighWater;
    else   
	cep->fHighWaterHighDamp = cep->fHighWater + damping;

    cep->fPongOptQueueDepth = cep->fLowWater;
}

/*
 * CepGetOqueueStatus()
 *
 * Get current level of DLC OQueue.
 * Note that for legacy code *LowDamp is the same as *HighDamp, so
 * dlc->fCurrCongLevel will never be returned for legacy DLC users.
 */

dlc_oqueue_status_t CepGetOqueueStatus (CepT *cep)
{
    int count = QUEUESIZE(cep->fOqueue);

    if (count == 0) 
        return(DLC_OQUEUE_EMPTY);
    else if (count < cep->fLowWaterLowDamp) 
        return(DLC_OQUEUE_LOW);
    else if (count < cep->fLowWaterHighDamp) 
        return(cep->fCurrCongLevel);
    else if (count < cep->fHighWaterLowDamp) 
        return(DLC_OQUEUE_NORMAL);
    else if (count < cep->fHighWaterHighDamp) 
        return(cep->fCurrCongLevel);
    else if (count < cep->fOqueueMax) 
        return(DLC_OQUEUE_HIGH);
    else
        return(DLC_OQUEUE_FULL);
}

/* CepFlowControlInd
 *
 * Generates flow control call back indications for CLS DLUs.
 * Note: Since llc2 checks for flow control level changes on each
 * enqueue on the txQ, indications will not skip levels.
 *
 * Note: txQ Empty is not a significant event.
 */

void CepFlowControlInd (void *uCepId, CepT *cep)
{
    dlc_oqueue_status_t new_level;

    if (uCepId == (ClsUCepIdT) CLS_NO_CEP)
       /*
        * Got a message from DLC for a CEP that has since been
        * destroyed. If this happens, there is no need to send
        * CLS_FLOW_IND or CLS_PONG_CNF since cep is gone.
        */
       return;

    new_level = CepGetOqueueStatus(cep);
    if (new_level == DLC_OQUEUE_EMPTY)
        new_level = DLC_OQUEUE_LOW;

    if (cep->fCurrCongLevel != new_level) {
        cep->fCurrCongLevel = new_level;
	CepFlowNotify(uCepId, cep);
	cep->fPrevCongLevel = cep->fCurrCongLevel;
    }
    CepHandlePong(uCepId, cep);
}

/* CepPongRequest
 *
 * Process Pong Request
 * If not yet ready to send PongConfirm, just set PongPending
 */

void CepPongRequest(void *uCepId, CepT *cep)
{    
    cep->fPongPending = TRUE;
    CepHandlePong(uCepId, cep);
}

/* CepHandlePong
 *
 * If we can send the dlu a Pong Confirm do so
 */

void CepHandlePong(void *uCepId, CepT *cep)
{
    int nextWindow;
    int queueSize;
    leveltype level;
    ClsDlcParamBlockT param_block;

    queueSize = QUEUESIZE(cep->fOqueue);
    
    nextWindow = cep->fPongOptQueueDepth - queueSize;

    level = raise_interrupt_level(NETS_DISABLE);

    if (cep->fPongPending && nextWindow >= CLS_MIN_PONG_WINDOW) {
	cep->fPongPending = FALSE;
	reset_interrupt_level(level);

	/*
	 * fPongOptQueueDepth varies between CLS_MIN_PONG_Q_DEPTH
	 * and cep->fLowWaterHighDamp.  When the cep's queue
	 * depth is zero (meaning it got rid of everything on its
	 * plate before the next pong req, the fPongOptQueueDepth 
	 * (and thus the pong window) can grow to avoid lots of pongs
         * an uncongested dlc.
         *
         * However, when the cep's queue depth is non-zero, it
         * means a pong was requested before the dlc was able to
         * process its entire queue.  When this happens we want
         * to lower fPongOptQueueDepth to keep the traffic out of
         * the cep queues and in the dlu's queues so that the dlu
         * can prioritize traffic better on the congested dlc.
         *
         * Because the queue depth is a fairly volitile value,
         * fPongOptQueueDepth only adjusts by 1 for each pong
         * sent.  This will smooth out the adjustment action as
         * an interface becomes congested or uncongested, requireing
         * a good trend of congestion to appreciably move
         * fPongOptQueueDepth lower, and vice-versa.
	 */

	if (queueSize == 0) {
	  if (cep->fPongOptQueueDepth < cep->fLowWaterHighDamp) 
	    cep->fPongOptQueueDepth++;
	  }
	else {
          if (cep->fPongOptQueueDepth > CLS_MIN_PONG_Q_DEPTH)
	    cep->fPongOptQueueDepth--;
          }

	param_block.fPakPtr = NULL;
	param_block.fResultError = CLS_OK;
	param_block.fParameters.fNextWindowSize = nextWindow;
	reg_invoke_cls_rput(uCepId, CLS_PONG_CNF, &param_block);
    } else {
	reset_interrupt_level(level);
    }
}

/* CepFlowNotify
 *
 * Make current congestion level to the flow level required at the
 * DLC/CLS interface, and issue the rput command
 */

void CepFlowNotify (void *uCepId, CepT *cep)
{
    ClsDlcParamBlockT paramBlock;

    paramBlock.fPakPtr = NULL;
    paramBlock.fResultError = CLS_OK;

    /*
     * Convert LLC to CLS congestion level. 
     */
    if (cep->fCurrCongLevel == DLC_OQUEUE_LOW)
	paramBlock.fParameters.fFlowLevel = DLC_CONGESTION_LOW;
    else if (cep->fCurrCongLevel == DLC_OQUEUE_NORMAL)
	paramBlock.fParameters.fFlowLevel = DLC_CONGESTION_MEDIUM;
    else if (cep->fCurrCongLevel == DLC_OQUEUE_FULL)
	paramBlock.fParameters.fFlowLevel = DLC_CONGESTION_HIGH;
    else
	paramBlock.fParameters.fFlowLevel = DLC_CONGESTION_MAX;
    
    if (cep->fCurrCongLevel > cep->fPrevCongLevel)
	paramBlock.fParameters.fFlowLevel |= DLC_CONGESTION_INCR;

    reg_invoke_cls_rput(uCepId, CLS_FLOW_IND, &paramBlock);
}

/* PrintDlcOpCode
 * 
 * Display the op code in a meaningful way
 */

static char *const DlcOpCodeName[DLC_NUM_OPCODES] = {
    "INIT_SYSTEM_REQ",
    "ENABLE_REQ ",
    "DISABLE_REQ ",
    "ACTIVATE_SAP_REQ ",
    "MODIFY_SAP_REQ ",
    "DEACTIVATE_SAP_REQ ",
    "ACTIVATE_RING_REQ ",
    "DEACTIVATE_RING_REQ ",
    "RQ_OPNSTN_REQ ",
    "CLOSE_STN_REQ ",
    "MODIFY_STN_REQ ",
    "ID_REQ ",
    "ID_RSP ",
    "ID_STN_REQ ",
    "ID_STN_RSP ",
    "TEST_REQ ",
    "TEST_RSP ",
    "TEST_STN_REQ ",
    "TEST_STN_RSP ",
    "UDATA_REQ ",
    "UDATA_STN_REQ ",
    "CONNECT_REQ ",
    "CONNECT_RSP ",
    "SIGNAL_STN_REQ ",
    "DISC_REQ ",
    "DISC_RSP ",
    "DATA_REQ ",
    "PONG_REQ ",
    "FLOW_REQ "
};

void PrintDlcOpCode (DlcOpcodeT opcode)
{
    if (opcode > DLC_NUM_OPCODES) {
	buginf("Bad dlc opcode ?%d? ", opcode);
    } else {
	buginf(DlcOpCodeName[opcode]);
    }
}

/* 
 * cls_assert - Check condition and report failures
 */
void cls_assert (char* file,int line, char *exp, boolean fatal)
{
#ifdef CLS_ALWAYS_CHECK_SANITY
    static char msg[1000];
    sprintf(msg,"file \"%s\", line %d\n%s", file, line, exp);
    cls_badassert++;
    cls_assertfile = file;		/* Keep record of last */
    cls_assertline = line;

#ifndef QUIET_SANITY_CHECKS
    errmsg(&msgsym(CLSFAIL, CLS), msg);
#else
    buginf("\nCLS: Assertion failed: %s",msg);
#endif
    
    if (gCLSbreakOnAssert)
	if (fatal)
	    /*
	     * CRASH THE MACHINE!!!!
	     */
	    crashdump(10);
	else
	    breakpoint();
    
#endif
}

/*
 * DLC subsystem header
 */

#define DLC_MAJVERSION 1
#define DLC_MINVERSION 0
#define DLC_EDITVERSION  1

SUBSYS_HEADER(dlc, DLC_MAJVERSION, DLC_MINVERSION, DLC_EDITVERSION,
	      dlc_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);

