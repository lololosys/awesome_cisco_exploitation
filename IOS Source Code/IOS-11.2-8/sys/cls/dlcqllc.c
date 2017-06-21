/* $Id: dlcqllc.c,v 3.4.26.7 1996/09/12 18:56:44 rbatz Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlcqllc.c,v $
 *------------------------------------------------------------------
 * dlcqllc.c - adapter from clsi/dlc interface to qllc
 *
 * February, 1995		Phil Morton
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dlcqllc.c,v $
 * Revision 3.4.26.7  1996/09/12  18:56:44  rbatz
 * CSCdi65595:  Traceback on multiple "qllc dlsw" commands.
 * Branch: California_branch
 *
 * Revision 3.4.26.6  1996/08/28  12:39:28  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.26.5  1996/08/07  18:22:03  pmorton
 * CSCdi61562:  45k crash in QllcTestStnReq()
 * Branch: California_branch
 * Test whether Virtual Mac Address Range exists before using.
 *
 * Revision 3.4.26.4  1996/07/08  01:12:14  rbatz
 * CSCdi60002:  Npsi-poll and proxy-xid do not work for DLSw+/QLLC.
 * Branch: California_branch
 *
 * Revision 3.4.26.3  1996/05/29  22:51:17  rbatz
 * CSCdi55749:  Multiple "qllc dlsw..." commands do not work.
 * Branch: California_branch
 *
 * Revision 3.4.26.2  1996/05/17  10:42:35  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.14.2  1996/04/11  14:36:12  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.14.1  1996/04/03  13:54:44  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.26.1  1996/03/18  19:10:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/13  01:14:32  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.2.1  1996/03/07  08:41:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/18  20:37:55  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1996/01/18  06:59:49  kpatel
 * CSCdi46758:  DLSw : CLS assertion failures  - DLSw over QLLC
 *
 * Revision 3.2  1995/11/17  08:55:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:53:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/11  07:56:06  pmorton
 * CSCdi40051:  QLLC (CLS) Parameter missing from assertion output
 * Remove %x from format string.  Backtrace on assertion failure provides
 * enough info
 *
 * Revision 2.5  1995/08/22  22:51:00  pmorton
 * CSCdi38936:  QLLC CLS code doesnt initialize ID_STN.Ind fields
 * Initialize them!
 *
 * Revision 2.4  1995/07/19  20:58:31  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add support for Activate/Deactivate Ring
 *
 * Revision 2.3  1995/07/02  05:15:36  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.2  1995/06/14  15:17:33  anke
 * CSCdi35807:  Yet more spellink errors
 *
 * cls/dlcqllc.c:         Unexepcted -> Unexpected
 * ipc/msg_ipc.c:         availble -> available
 * exec_debug_channel.h:  debuggeries -> debugging
 * filesys/fslib.c:       inconsistant -> inconsistent
 * atm/sscop.c:           initalize -> initialize
 * filesys/fslib_print.c: progresss -> progress
 * ts/callback.c:         unnexpected -> unexpecte
 *
 * Revision 2.1  1995/06/07  20:20:27  hampton
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
#include "config.h"
#include "../x25/x25.h"
#include "../x25/x25_address.h"
#include "../util/avl.h"
#include "../srt/qllc.h"
#include "../srt/qllc_private.h"
#include "../if/network.h"
#include "../ibm/sna_util.h"

/* local function declarations */
static clsQllcMapFnT QllcInitSystem(qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcActivateSapReq(qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcReqOpnStnReq(qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcIdReq(qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcIdRsp (qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcConnectReq(qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcConnectRsp(qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcSignalStnReq(qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcDataReq(qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcFlowReq(qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcDisconnectReq(qllctype *qllc,  ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcTestReq(qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcTestStnReq(qllctype *qllc, ClsDlcParamBlockT *clsBlock);

#ifdef DLSw_IN
static clsQllcMapFnT QllcActivateRngReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcDeActivateRngReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcTestRsp (qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcUDataReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcIdStnReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcIdStnRsp (qllctype *qllc, ClsDlcParamBlockT *clsBlock);
static clsQllcMapFnT QllcTestStnRsp (qllctype *qllc, ClsDlcParamBlockT *clsBlock);

inline static boolean QllcTestStnTxPak(ClsDlcParamBlockT *clsBlock, 
				      boolean cmd_resp_flag);
inline static void QllcStnEncap(paktype *pak, uchar *src, uchar *dst,
				   sap_hdr *llc1, 
				   uchar *rif, ushort payload_len, 
				   uchar *payload_ptr);
static clsQllcMapFnT QllcUDataStnReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock);

/* This _is_ needed for timeout on getting a respnse to an XID
 * sent on the link
 */
static void DlcTxXidFail(qllctype *qllc, ClsErrorCodeT error);
#endif /* DLSw_IN */


/************************************************************************
 *			     CLSI Primitive: (none)
 ************************************************************************
 *
 * DLC Message Generated/Received
 * ------------------------------
 * qllc_wput(*cep, DLC_INIT_SYSTEM, &parm_block)
 *
 * Code needs a mechanism to initialize global qllc structures, such as
 * hash tables and queues.  Parameter block will contain initialization
 * information such as:
 * 	qllc_hash size
 *
 *
 * Old Code Function
 * -----------------
 * qllc_init()
 *
 * Code initializes assiciated qllc data structures such as:
 * 	qllc hash table
 * 	internal queues
 * 	parser
 * 	further modular system initializations
 *
 *
 * Enhancements
 * ------------
 * For current code no change will be made to subsystem initialization.
 * We call qllc_start here, not qllc_init.
 ************************************************************************
 */

static inline clsQllcMapFnT 
QllcInitSystem (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    /* qllc_init(); - currently called in system startup stuf */
    qllc_start();
    return;
}



/************************************************************************
 *			   CLSI Primitive: Enable.req
 ************************************************************************
 *
 * Initialize port with hardware specific paramters.
 * Create the QLLC SAP Registry
 *
 * DLC Message Generated
 * ---------------------
 * qllc_wput(*cep, DLC_ENABLE_REQ, &param_block)
 *
 *
 ************************************************************************
 */

static clsQllcMapFnT 
QllcEnableReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    idbtype		*idb = NULL;
    DlcEnableT		*dlcEnable;
    QSapRegistryT	*qsr;

    /* Check if the port exists and is capable of running QLLC */

    dlcEnable = &clsBlock->fParameters.fDlcEnable;
    if (dlcEnable->fDLCType != CLS_QLLC) {
    	dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    idb = DlcToIdb(&dlcEnable->fPortId);

    if (idb == NULL) {
	dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_UNKNOWN_PORT);
	return;
    }
    /*
     * If the sap registry doesn't exist for this port yet, create it.
     */

    qsr = QllcPortGetSapRegistry(idb);
    if (qsr == NULL) {
        qsr = QSapRegistryCreate(idb);
        if (qsr == NULL) {
            dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_OUT_OF_MEMORY);
            return;
        }
    }

#ifdef DO_IT_LATER
    /*
     * If this port requires/allows input parameters, then check that
     * they make sense.
     */

    if (!DlcValidPortParams(idb, clsBlock)) {
        dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_CONFIG_CONFLICT);
        return;
    }
#endif

    dlcEnable->fPPortId = (DlcPPortIdT) idb;
    dlcEnable->fMediaEnable.fQLLCEnable.fVersion = CLSI_VERSION;
	    
	    
    dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_OK);
    qllc_start();
}


/************************************************************************
 *			   CLSI Primitive: Disable.req
 ************************************************************************
 *
 * Disable port 
 *
 * DLC Message Generated
 * ---------------------
 * qllc_wput(*cep, DLC_DISABLE_REQ, &param_block)
 *
 *
 ************************************************************************
 */

static clsQllcMapFnT 
QllcDisableReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{

    idbtype		*idb = NULL;
    DlcDisableT       	*dlcDisable;


    dlcDisable = &clsBlock->fParameters.fDlcDisable;
    if (dlcDisable->fDLCType != CLS_QLLC) {
    	dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    idb = DlcToIdb(&dlcDisable->fPortId);
    if (idb == NULL) {
	dlc_rput(clsBlock, CLS_DISABLE_CNF, CLS_UNKNOWN_PORT);
	return;
    }
	    
    dlc_rput(clsBlock, CLS_DISABLE_CNF, CLS_OK);
}


/************************************************************************
 * 		   CLSI Primitive: Activate_sap.req
 * 		   CLSI Primitive: Activate_ring.req
 *************************************************************************
 * Define Service Access Points
 *
 * There is no difference for QLLC between a SAP and A RING. as far as
 * the disaptching mechanism is concerned.  If there was a way to
 * accept Incoming Calls for arbitrary X.121 addresses, then we could
 * consider a Ring to be like a SAP that operates in promiscuous mode.
 *
 * For cleanliness we have two different entry points, but they both
 * call a common routine:
 * 			QllcActivateSapGuts
 *
 * DLC Message Generated
 * ---------------------
 * qllc_wput(*cep, qllc_ACTIVATE_SAP_REQ, &param_block)
 * qllc_wput(*cep, qllc_ACTIVATE_RING_REQ, &param_block)
 *
 * Build qsap with data from QSap in pb, add it to QSap Registry, 
 *
 * add in logic for virtual mac ranges
 *
 * Algorithm:
 *
 * check for conflicts with existing saps
 * a) by subaddress/pvc range
 * b) by virtual mac address range
 *
 * build data structures for QSapT, and if needed for QSapVmacRangeT
 * use malloc since it will usually be successful - free only if failure
 *
 * add them to their respective lists, and build pointer links
 * 
 **********************************************************************
 */
static void QllcActSapGuts (ClsDlcParamBlockT *clsBlock, ClsOpcodeT opCode)
{
    idbtype		*idb;
    DlcActSapT		*dlcActSap;
    QLLCActSapT		*qActSap;
    QSapRegistryT	*qsr;
    QSapT		*clsSap;	/* part of the paramter block */
    QPSapT		*newSap;	/* the real thing - gets malloc'ed */
    QSapVmacRangeT	*newqvmr;
    void		*existing;
    ClsUSapIdT 		uSapId;
    boolean		result;


    dlcActSap 	= &clsBlock->fParameters.fDlcActSap;
    qActSap	= &dlcActSap->fDlcSap.fQLLCActSap;
    clsSap 	= &qActSap->fQActSap;
    uSapId	= dlcActSap->fUSapId;

    idb = (idbtype *)(dlcActSap->fPPortId);
    if (idb == NULL) {
	errmsg(&msgsym(OPTNULL, DLC));
	dlc_rput(clsBlock, opCode, CLS_UNKNOWN_PORT);
	return;
    }

    /* find the SAP Registry */	
    qsr = QllcPortGetSapRegistry(idb);
    if (qsr == NULL) {
	dlc_rput(clsBlock, opCode, CLS_NO_SAP_REGISTRY);
	return;
    }

    /* Make a new entry - use fields from the clsSap paramters */
    newSap = QSapCreate(uSapId, clsSap, idb);
    if (newSap == NULL) {
	dlc_rput(clsBlock, opCode, CLS_OUT_OF_MEMORY);
	return;
    }

    /* Make sure that a matching SAP entry does not exist */
    existing = QSRSearchBySap(qsr, newSap);
    if (existing != NULL) {
        printf("\nQLLC: Configuration conflict.");
        QSapDestroy(newSap);
        dlc_rput(clsBlock, opCode, CLS_SAP_BUSY);
        return;
    }
    
    if (QSapTestVmac(clsSap)) {	/* Virtual Mac mapping required */
	/* Make a new Virtual Mac Address Range entry
	 * use fields from the qllcActSap paramters
	 */
	newqvmr = QVmacRangeCreate(newSap, qActSap->fVmacStart,
				   qActSap->fNumVmacs);
		
	if (newqvmr != NULL) {	/* make sure it's not a duplicate or overlap */
	    existing = QVmacRSearch(qsr, newqvmr);
	    if (existing != NULL) {	/* Partial or complete overlap	
					 * Either way we're screwed
					 */
		dlc_rput(clsBlock, opCode, CLS_VMAC_OVERLAP);
		QVmacRangeDestroy(newSap);
		QSapDestroy(newSap);
		return;
	    }
	} else {  	/* couldn't allocate memory for vmac range */
	    dlc_rput(clsBlock, opCode, CLS_OUT_OF_MEMORY);
	    QSapDestroy(newSap);
	    return;
	}
	/* Add it to the Registry */
	result = (boolean)QVmacRInsert (qsr, newqvmr);
	CLSASSERT(result);
    } else {
    	newqvmr = NULL;
    }
    
    if (!(IsSVCSap(clsSap))) {
        if (!QSapClaimPvcs(newSap)) {
            QSapDestroy(newSap);
            dlc_rput(clsBlock, opCode, CLS_CONFIG_CONFLICT);
            return;
        }
    }
    
    /* Add the sap to the Registry */
    result = QSRAddQSap(qsr, newSap);
    if (!result) {
        dlc_rput(clsBlock, opCode, CLS_CONFIG_CONFLICT);
        return;
    }    
    
    dlcActSap->fDlcPSapId = (DlcPSapIdT)newSap; /* pass back up to CLS core  */
    dlc_rput(clsBlock, opCode, CLS_OK);
    return;
}

static inline  clsQllcMapFnT 
QllcActivateSapReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    QllcActSapGuts(clsBlock, CLS_ACTIVATE_SAP_CNF);
    return;
}

static inline  clsQllcMapFnT 
QllcActivateRingReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    QllcActSapGuts(clsBlock, CLS_ACTIVATE_RING_CNF);
    return;
}


/************************************************************************
 * 		   CLSI Primitive: Modify_sap.req
 *************************************************************************
 * Modifies operational parameters on the SAP
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, qllc_MODIFY_SAP_REQ, &param_block)
 *
 *
 * Old Code Function
 * -----------------
 * none/New function
 *
 * Enhancements
 * ------------
 * Not yet implemented.
 *
 ***********************************************************************
 */

static inline  clsQllcMapFnT 
QllcModifySapReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    return;
}

/************************************************************************
 * 		   CLSI Primitive: Deactivate_sap.req
 * 		   CLSI Primitive: Deactivate_ring.req
 *************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * qllc_wput(*cep, qllc_DEACTIVATE_SAP_REQ, &param_block)
 * qllc_wput(*cep, qllc_DEACTIVATE_RING_REQ, &param_block)
 *
 * As with activation, so with deactivation. SAP and RING have the
 * same use for QLLC.
 *
 * Old Code Function
 * -----------------
 * None/New function
 *
 * Enhancements
 * ------------
 * Removes Sap from the registry
 *
 * Extract the USapId from the Parameter Block
 * 
 * Lookup the Sap in the registry
 * Remove the sap, and remove the vmac range if it exists
 *
 *
 ***********************************************************************
 */

static void QllcDeactSapGuts (ClsDlcParamBlockT *clsBlock, ClsOpcodeT opCode)
{
    idbtype		*idb;
    DlcActSapT		*dlcActSap;
    QSapRegistryT	*qsr;
    QPSapT		*qpsap;
    ClsUSapIdT 		uSapId;
    boolean		result;


    dlcActSap 	= &clsBlock->fParameters.fDlcActSap;
    uSapId	= dlcActSap->fUSapId;

    idb = (idbtype *)(dlcActSap->fPPortId);
    if (idb == NULL) {
	errmsg(&msgsym(OPTNULL, DLC));
	dlc_rput(clsBlock, opCode, CLS_UNKNOWN_PORT);
	return;
    }

    /* find the SAP Registry */
	
    qsr = QllcPortGetSapRegistry(idb);
    if (qsr == NULL) {
	dlc_rput(clsBlock, opCode, CLS_NO_SAP_REGISTRY);
	return;
    }

    /* Fail if the SAP entry is not there */

    qpsap = QSRSearchByUSap(qsr, uSapId);
    if (qpsap == NULL) {
	dlc_rput(clsBlock, opCode, CLS_UNKNOWN_CEP);
	return;
    }
	   
    /* Remove from the Registry */ 
    result =  QSRRemoveQSap (qsr, qpsap);
    CLSASSERT(result);
    
    /* Check for a vmac address range, remove if from the registry, and destroy it */
    if (qpsap->fQVmacRange != NULL) {
    	result = (boolean)QVmacRDelete(qsr, qpsap->fQVmacRange);
	CLSASSERT(result);
	QVmacRangeDestroy(qpsap);
    }

    QSapDestroy(qpsap);
    dlc_rput(clsBlock, opCode, CLS_OK);

    return;
}

static inline  clsQllcMapFnT 
QllcDeactivateSapReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    QllcDeactSapGuts(clsBlock, CLS_DEACTIVATE_SAP_CNF);
}

static inline  clsQllcMapFnT 
QllcDeactivateRingReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    QllcDeactSapGuts(clsBlock, CLS_DEACTIVATE_RING_CNF);
}



/************************************************************************
 * 	      CLSI Primitive: Req_opnstn.req (outgoing)
 *************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, qllc_REQ_OPNSTN_REQ, &parm_block)
 *
 *
 * Old Code Function
 * -----------------
 * qllc_open[_upstream_{lnx|snx}]()
 *
 * ------------
 *
 * If outgoing pCep exists then we allready have a connector in
 * place, else make one.  
 *
 * Submit input to  QX25FSM
 *
 * Error Conditions
 * ----------------
 * Could not allocate qllctype memory structure.
 *
 **********************************************************************
 */

static inline  clsQllcMapFnT 
QllcReqOpnStnReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    DlcReqOpenStnT	*dlcReqOpenStn;
    QLLCReqOpenStnT	*qllcReqOpenStn;
    QPSapT		*qpsap;
    ClsUCepIdT		uCepId;

    CLSASSERT(qllc == (qllctype *)CLS_NO_CEP);
        
    dlcReqOpenStn = &clsBlock->fParameters.fDlcReqOpenStn;
    qllcReqOpenStn = &dlcReqOpenStn->fDlcStn.fQLLCReqOpenStn;
    uCepId = dlcReqOpenStn->fClsUCepId;
    
    qpsap = (QPSapT *)dlcReqOpenStn->fDlcPSapId;
    CLSASSERT(qpsap);
	
    qllc = QCepFindOpened(qpsap, qllcReqOpenStn); /* See if CEP's already allocated */

    qllc = QCepReqOpenStn(qpsap, uCepId, qllcReqOpenStn, qllc);
    /* This routine will make sure that the SAP is valid, and that
     * we have enough resources to create the CEP.  e.g. nsvc OK
     */
    CLSASSERT(qllc != NULL);
    if (qllc == NULL) {			/* just in case */
	DlcReqOpenStnConf(0, uCepId,  CLS_LOCAL_ERROR);
	return;
    }
	
    switch ((qllcOpenFailureCode)qllc) {
	
	/* Confirm error conditions with NULL qllc.  Semantically, is
	 * consistent because no llc was created.  Also has side
	 * effect that the NULL condition does the right thing within
	 * DlcReqOpenStnConf(), and the error condition doesn't ;)
	 */
    case QllcInUse:
	DlcReqOpenStnConf((DlcPCepIdT) 0, uCepId,  CLS_CONNECTION_BUSY);
	return;
	    
    case QllcNoMem:
	DlcReqOpenStnConf((DlcPCepIdT) 0, uCepId, CLS_OUT_OF_MEMORY);
	return;
    case QllcNoRemoteAddress:
	DlcReqOpenStnConf((DlcPCepIdT) 0, uCepId, CLS_BAD_ADDRESS);
	return;

    default:		/* since nothing bad happened, qCep is set up */
	dlcReqOpenStn->fDlcPCepId = (DlcPCepIdT)qllc;
	break;
    } /* end switch */
    QXFsmExecute(qllc, QX25ReqOpenStnReq);	/* give it to the FSM */

    return;
}

/************************************************************************
 * 	      CLSI Primitive: closestn.req (outgoing)
 **********************************************************************
 */

static inline  clsQllcMapFnT 
QllcCloseStnReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    CLSASSERT(qllc != NO_CLS);		/* legacy code shouldn't be getting in here */

    if (qllc == (qllctype *)CLS_NO_CEP) {
	/* 
	 * Trying to cancel a pending ReqOpnStn. CLS did not know the 
	 * p_cep_id (llc), but it does know the u_cep_id it sent us in 
	 * the ReqOpnStn.req. We need to scan all llc structures to 
	 * find the correct one to close.
	 */

	ClsUCepIdT uCepId;

	uCepId = clsBlock->fParameters.fDlcCloseStn.fClsUCepId;

	qllc = QCepSearchByUCepId(uCepId);	/* scan for qllc->u_cep_id */
    }

    /* 
     * I've got a qllctype either because it was the pcepid passed in by
     * CLS, or because the code above found it from the ucepid. In either
     * case I do the same thing.
     */
    if (qllc != (qllctype *)CLS_NO_CEP && qllc != NO_CLS) {
	QXFsmExecute(qllc, QX25CloseStnReq); /* give it to the FSM */
    } else {
	errmsg(&msgsym(INVPCEP, DLC));
    }
    return;
}

/************************************************************************
 * 	      CLSI Primitive: modifystn.req (outgoing)
 *************************************************************************
 *
 **********************************************************************
 */

static inline  clsQllcMapFnT 
QllcModifyStnReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    return;
}


/************************************************************************
 * 			CLSI Primitive: ID.req
 *************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, DLC_ID_REQ, &param_block)
 *
 *
 * Old Code Function
 * -----------------
 * qllc_type1_event
 *
 *
 * Enhancements
 * ------------
 *
 * Who transmits on XID time-out?  DLU?  Or CLS?  How does APPN code
 * handle it?  Make it a configuration parameter Req_opnstn.req.
 * Potentially return timeout condition in ID.cnf.
 *
 * Error Conditions
 * ----------------
 * No memory for packet
 *
 * Output interface is down.
 **********************************************************************
 */

static inline  clsQllcMapFnT 
QllcIdReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    DlcIdStnT	*dlcIdStn;
    paktype 	*pak;

    CLSASSERT(qllc != NO_CLS &&		/* no legacy code allowed here */
    		qllc != (qllctype *)CLS_NO_CEP); /* pCep should be valid */ 

    dlcIdStn = &clsBlock->fParameters.fDlcIdStn;
    pak	= clsBlock->fPakPtr;

    if (dlcIdStn->fXidT == XID_COMMAND) {
	qllc->fQClsXidRspPending = TRUE;
    }

    /*
     * If we receive a NULL XID and npsi-poll is configured,
     * drop the XID.  NPSI (or whomever) will send a NULL QXID
     * to solicit the XID(0) from the othger side.
     */
    if (QCepTestNpsiPoll(qllc) && (pak->datagramsize == 0)) {
        datagram_done(pak);
	return;
    }
  
    /*
     * If we receive a NULL XID and proxy-xid is configured,
     * send in the configured IDBLK/IDNUM in the XID(0).
     */
    if (QTestProxyXid(&qllc->fQCepFlags) && (pak->datagramsize == 0)) {
	pak->datagramsize       = XID0_LENGTH;
	pak->datagramstart[0]   = XID_TYPE0_FMT2;
	pak->datagramstart[1]   = 0;        /* Fixed format XID - no length field */
        memcpy(pak->datagramstart + 2, (uchar *) &qllc->fProxyXid, XID_SIZE);
	QllcClsNotify(qllc, CLS_ID_IND, CLS_OK, pak);
	return;
    }
  
    QLFsmExecute(qllc, CLSXID, pak);	/* give it to the FSM */

    return;
}

static inline  clsQllcMapFnT 
QllcIdRsp (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    QllcIdReq(qllc, clsBlock);
}

static clsQllcMapFnT 
QllcTestStnReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    DlcTestStnT		*dlcTestStn;
    idbtype		*swidb;
    hwidbtype		*hwidb;
    QPSapT		*qpsap;
    uchar		vmacLow[IEEEBYTES];
    uchar		vmacHigh[IEEEBYTES];
    paktype		*pak;
    
    dlcTestStn = &clsBlock->fParameters.fDlcTestStn;
    pak = clsBlock->fPakPtr;
    swidb = DlcToIdb(&clsBlock->fPortId);
    hwidb = hwidb_or_null(swidb);
    
    CLSASSERT(pak);
    if (pak == NULL) {
    	return;
    }

    if (hwidb == NULL) {
    	datagram_done(pak);
	return;
    }
    if (!hwidb->x25_upstate) {
    	datagram_done(pak);
	return;
    }

    qpsap = (QPSapT *) dlcTestStn->fDlcPSapId;
    if ( (!qpsap) || (!QPSapTestVmac(qpsap)) ) {
        datagram_done(pak);
        return;
    }

    /* 
     * Check if the TestStn Mac Address is within the
     * range of this qpsap's virtual Mac Addresses.
     */
    ieee_copy(qpsap->fQVmacRange->fVmacStart, vmacLow);
    ieee_add(vmacLow, qpsap->fQVmacRange->fNumVmacs - 1, vmacHigh);
    if ((ieee_compare(dlcTestStn->fRMac, vmacLow)  < 0) ||
        (ieee_compare(dlcTestStn->fRMac, vmacHigh) > 0)) {
         datagram_done(pak);
         return;
    }

    /* sanity checks completed, build and send reply */
    DlcBounceTestStnInd((DlcPCepIdT)qllc, clsBlock);
    return;
}

static inline clsQllcMapFnT  
QllcTestReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    DlcHandleTestReq(qllc->fUCepId, clsBlock);
}

/************************************************************************
 * 		     CLSI Primitive: Connect.req
 *************************************************************************
 * (Cause QSM to flow...)
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, qllc_CONNECT_REQ, &param_block)
 *
 * Old Code Function
 * -----------------
 *
 * qllc_open
 *
 * Enhancements
 * ------------
 *
 * DLC Message Received
 ************************************************************************
 */

static inline  clsQllcMapFnT  
QllcConnectReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    if (!qllc) {
	clsBlock->fResultError = CLS_UNKNOWN_CEP;
	return;
    }

    QLFsmExecute(qllc, ConnectRequest, NULL); /* give it to the FSM */
    
    return;
}


/************************************************************************
 *		     CLSI Primitive: Connect.rsp
 *************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_CONNECT_RSP, &param_block)
 * 	cookie should be LLC's p_cep_id
 *
 * Old Code Function
 * -----------------
 * qllc_contact_accept
 * 	qllc sends a QUA
 *
 * Enhancements
 * ------------
 * Function carries out the transition from Contact_Cnf_Pending to
 * another new state (Contacted_Ind_Pending).
 ***********************************************************************
 */

static inline  clsQllcMapFnT 
QllcConnectRsp (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    CLSASSERT(qllc != NO_CLS &&		/* no legacy code allowed here */
    		qllc != (qllctype *)CLS_NO_CEP); /* CEP should be valid */ 

    QLFsmExecute(qllc, ConnectResponse, NULL);	/* give it to the FSM */
   
    return;
}



/************************************************************************
 * 		    CLSI Primitive: Signal_stn.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_SIGNAL_STN_REQ, &param_block)
 * 	cookie should be LLC's p_cep_id
 *
 * Old Code Function
 * -----------------
 * (none)/New Function
 * 	qllc_quench(qllctype *qllc, boolean quench_on)
 *
 **********************************************************************
 */

static inline  clsQllcMapFnT 
QllcSignalStnReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    CLSASSERT(qllc != NO_CLS &&		/* no legacy code allowed here */
    		qllc != (qllctype *)CLS_NO_CEP); /* CEP should be valid */ 

    QLFsmExecute(qllc, SignalStationRequest, NULL); /* give it to the FSM */
   
    return;
}


/************************************************************************
 *		       CLSI Primitive: Data.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_DATA_REQ, &param_block)
 * 	cookie should be DLC's p_cep_id
 *
 * Old Code Function
 * -----------------
 * qllc_send()/sna_th_frag
 * 	callout when the _first_ I- or S- frame is received.
 **********************************************************************
 */

static inline  clsQllcMapFnT 
QllcDataReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    paktype *pak;

    CLSASSERT(qllc != NO_CLS &&		/* no legacy code allowed here */
    		qllc != (qllctype *)CLS_NO_CEP); /* CEP should be valid */ 

    pak	= clsBlock->fPakPtr;
    
    QLFsmExecute(qllc, DataRequest, pak); /* give it to the FSM */

    return;
}


/************************************************************************
 * 		       CLSI Primitive: Flow.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_FLOW_REQ, &param_block)
 * 	cookie should be DLC's p_cep_id
 *
 * Old Code Function
 * -----------------
 * 	qllc_quench(qllctype *qllc, boolean quench_on)
 *
 * Need to make qllc_quench do nasty things with the x25_process_other
 *
 * CLSI Primitive
 * --------------
 * Flow.ind
 *
 * DLC Message Generated
 * ---------------------
 * cls_rput(cookie, CLS_FLOW_IND, &param_block)
 * 	cookie should be CLS's u_cep_id
 *
 * Old Code Function
 * -----------------
 * (none)/New Function		- qllc_input().
 * 	Need to call out from qllc code.
 *
 * ?? Should we queue any packets received after making the call
 *    to CLS
 *    I say yes.
 * ??
 * ***********************************************************************/
static inline  clsQllcMapFnT 
QllcFlowReq(qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    CLSASSERT(qllc != NO_CLS &&		/* no legacy code allowed here */
    		qllc != (qllctype *)CLS_NO_CEP); /* CEP should be valid */ 

    QLFsmExecute(qllc, clsBlock->fParameters.fResumeFlow?
	     CLSQuenchOff: CLSQuenchOn, NULL); /* give it to the FSM */

    return;
}

/************************************************************************
 * 		       CLSI Primitive: Pong.req
 ************************************************************************
 * Set up for generalized Pong Handling, and off we go
 *
 * ***********************************************************************/
static clsQllcMapFnT QllcPongReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    CepPongRequest(qllc->fUCepId, &qllc->fCep);
    return;
}


/************************************************************************
 *		    CLSI Primitive: Disconnect.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_DISC_REQ, &param_block)
 * 	cookie should be LLC's p_cep_id
 *
 * Old Code Function
 * -----------------
 * qllc_close()
 * 	Sends QDISC.
 * ?? Should we allow for immediate disconnection?
 * ??
 *
 * Error Conditions
 * ----------------
 * Invalid cookie.
 **********************************************************************
*/

static inline  clsQllcMapFnT 
QllcDisconnectReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    CLSASSERT(qllc != NO_CLS &&		/* no legacy code allowed here */
	      qllc != (qllctype *)CLS_NO_CEP); /* CEP should valid */ 

    QLFsmExecute(qllc, DisconnectRequest, NULL); /* give it to the FSM */
   
    return;
}

/************************************************************************
 *		    CLSI Primitive: Disconnect.rsp
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_DISC_REQ, &param_block)
 * 	cookie should be LLC's p_cep_id
 *
 * Old Code Function
 * -----------------
 * qllc_close()
 * 	Sends QUA.
 * ?? Should we allow for immediate disconnection?
 * ??
 *
 * Error Conditions
 * ----------------
 * Invalid cookie.
 **********************************************************************
*/

static inline  clsQllcMapFnT 
QllcDisconnectRsp (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    CLSASSERT(qllc != NO_CLS &&		/* no legacy code allowed here */
    		qllc != (qllctype *)CLS_NO_CEP); /* CEP should be valid */

    QLFsmExecute(qllc, DisconnectResponse, NULL); /* give it to the FSM */
   
    return;
}

/*
 * 		   CLSI Primitive: Idstn.req
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC_ID_STN_REQ	, &param_block)
 */
static inline clsQllcMapFnT 
qllcIdStnReq (qllctype *qllc, ClsDlcParamBlockT *clsBlock)
{
    CLSASSERT(!"NO qllc id station request");
}


void qllc_wput (DlcPCepIdT pCepId, DlcOpcodeT opcode,
	       ClsDlcParamBlockT *clsParamBlock)
{
    switch (opcode)
    {
    case DLC_INIT_SYSTEM_REQ:
	QllcInitSystem((qllctype *)pCepId, clsParamBlock); break;
	
    case DLC_ENABLE_REQ:
	QllcEnableReq((qllctype *)pCepId, clsParamBlock); break;
	
    case DLC_DISABLE_REQ:
	QllcDisableReq((qllctype *)pCepId, clsParamBlock); break;

    case DLC_ACTIVATE_SAP_REQ:
	QllcActivateSapReq((qllctype *)pCepId, clsParamBlock); break;
	
    case DLC_MODIFY_SAP_REQ:
	QllcModifySapReq((qllctype *)pCepId, clsParamBlock); break;
	
    case DLC_DEACTIVATE_SAP_REQ:
	QllcDeactivateSapReq((qllctype *)pCepId, clsParamBlock); break;
	
    case DLC_REQ_OPNSTN_REQ:
	QllcReqOpnStnReq((qllctype *)pCepId, clsParamBlock); break;
	
    case DLC_CLOSE_STN_REQ:
	QllcCloseStnReq((qllctype *)pCepId, clsParamBlock); break;
	
    case DLC_MODIFY_STN_REQ:
	QllcModifyStnReq((qllctype *)pCepId, clsParamBlock); break;
	
    case DLC_ID_REQ:
	QllcIdReq((qllctype *)pCepId, clsParamBlock); break;
	
    case DLC_CONNECT_REQ:
	QllcConnectReq((qllctype *)pCepId, clsParamBlock); break;
	
    case DLC_CONNECT_RSP:
	QllcConnectRsp((qllctype *)pCepId, clsParamBlock); break;

    case DLC_SIGNAL_STN_REQ:
	QllcSignalStnReq((qllctype *)pCepId, clsParamBlock); break;

    case DLC_DISC_REQ:
	QllcDisconnectReq((qllctype *)pCepId, clsParamBlock); break;

    case DLC_DISC_RSP:
	QllcDisconnectRsp((qllctype *)pCepId, clsParamBlock); break;

    case DLC_DATA_REQ:
	QllcDataReq((qllctype *)pCepId, clsParamBlock); break;

    case DLC_FLOW_REQ:
	QllcFlowReq((qllctype *)pCepId, clsParamBlock); break;

    case DLC_PONG_REQ:
	QllcPongReq((qllctype *)pCepId, clsParamBlock); break;

    case DLC_ACTIVATE_RING_REQ:
	QllcActivateRingReq((qllctype *)pCepId, clsParamBlock); break;
	break;

    case DLC_DEACTIVATE_RING_REQ:
    	QllcDeactivateRingReq((qllctype *)pCepId, clsParamBlock);
	break;

    case DLC_ID_RSP:
        QllcIdRsp((qllctype *)pCepId, clsParamBlock); break;

    case DLC_TEST_REQ:
        QllcTestReq((qllctype *)pCepId, clsParamBlock);
	break;

    case DLC_TEST_RSP:
        CLSASSERT(!"QllcTestRsp((qllctype *)pCepId, clsParamBlock); unexpected");
	break;

    case DLC_UDATA_REQ:
        CLSASSERT(!"QllcUDataReq((qllctype *)pCepId, clsParamBlock); unexpected");
	break;
 
    case DLC_ID_STN_REQ:
        CLSASSERT(!"QllcIdStnReq((qllctype *)pCepId, clsParamBlock); unexpected");
	break;

    case DLC_ID_STN_RSP:
        CLSASSERT(!"QllcIdStnRsp((qllctype *)pCepId, clsParamBlock); unexpected");
	break;

    case DLC_TEST_STN_REQ:
    	QllcTestStnReq((qllctype *)pCepId, clsParamBlock);
	break;

    case DLC_TEST_STN_RSP:
    	CLSASSERT(!"QllcTestStnRsp((qllctype *)pCepId, clsParamBlock);  unexpected");
	break;

    case DLC_UDATA_STN_REQ:
    	CLSASSERT(!"QllcUDataStnReq((qllctype *)pCepId, clsParamBlock);  unexpected");
	break;

    default:
	CLSASSERT(!"qllc_wput: opcode out of range!!\n");
    }
}

/* ---------------------- CLSI Action Helper Routines ---------------------- */


/* QBuildParamBlock
 *
 * Add Qllc specific things to Parameter block - the goal is that most of
 * the work should be done in the generic DlcBuildParamBlock routine
 *
 * Qllc specifc tasks fall into tw ocategories:
 *
 * 1. extracting CEPs and idb from the qllc type
 * 2. making mock token ring packets.
 */

static void QBuildParamBlock (qllctype *qllc, ClsOpcodeT opCode, 
                              ClsDlcParamBlockT *paramBlock, paktype *pak)
{
    switch (opCode) {
	    
    default:
	break;
	
	/* Part 1.  Getting CEPs and idb */
	
    case CLS_CONNECT_IND:
    case CLS_CONNECT_CNF:
	paramBlock->fParameters.fDlcConnect.fPPortId = qllc->idb;
	paramBlock->fParameters.fDlcConnect.fDlcPCepId = (DlcPCepIdT)qllc;
	paramBlock->fParameters.fDlcConnect.fClsUCepId = qllc->fUCepId;
	
	/* return code is ok for now */
	paramBlock->fParameters.fDlcConnect.fLlcRetCode = 0;
	break;
	
    case CLS_DISC_IND:
    case CLS_DISC_CNF:
	paramBlock->fParameters.fDlcDisconnect.fPPortId = qllc->idb;
	paramBlock->fParameters.fDlcDisconnect.fDlcPCepId = (DlcPCepIdT)qllc;
	paramBlock->fParameters.fDlcDisconnect.fClsUCepId = qllc->fUCepId;
	
	/* return code is ok for now */
	paramBlock->fParameters.fDlcConnect.fLlcRetCode = 0;
	break;
    case CLS_FLOW_IND:
	if (qllc->qllc_x25_busy) {
	    paramBlock->fParameters.fFlowLevel = DLC_CONGESTION_HIGH;
	} else {
	    paramBlock->fParameters.fFlowLevel = DLC_CONGESTION_LOW;
	}
	break;
	
    case CLS_ID_IND:
	{
	    DlcIdStnT		*dlcIdStn;
	    DlcXidIdStnT	*dlcXidStn;

	    dlcIdStn = &paramBlock->fParameters.fDlcIdStn;
	    dlcXidStn = &dlcIdStn->fDlcXid.fDlcXidStn;

	    dlcIdStn->fClsUSapId = (ClsUSapIdT)(qllc->fQSap->fUSapId);

	    CLSASSERT_FATAL(pak);

	    if (qllc->fQClsXidRspPending) {
		qllc->fQClsXidRspPending = FALSE;
		dlcIdStn->fXidT = XID_RESPONSE;
	    } else {
		dlcIdStn->fXidT = XID_COMMAND;
	    }
	    /*
	     * packet reaches this point in canonical form.
	     * No qllc header, and no XID Control Field (0xBF)
	     * datagramstart points to the first byte of the XID I Field
	     */

	    dlcIdStn->fXIDLength = pak->datagramsize;
	    dlcIdStn->fXIDPtr = pak->datagramstart;

	    pak->network_start = pak->datagramstart;
	
	    if (QCepTestVmac(qllc)) { /* Token Ring Form */
		dlcXidStn->fMacType = STATION_IEEE48;
	    
		/* allows for AC, FC, DMAC and SMAC */
		pak_delta_adjust_start(pak, -TRING_ENCAPBYTES);
	    
	    /* An ancient Indian Philospher asks:
	     *
	     * What is local, and what is remote?
	     *
	     * Ah my friend, I answer, it is is a matter of where we stand
	     *
	     * In llc2 over Token Ring which is a close to canonical
	     * as we get, the Local Station address is the destination
	     * mac address of the XID packet received, and the Remote
	     * Station address is the address of the station that
	     * originated the XID message.
	     *
	     * Transferring this concept to mock token ring, we find
	     * that the originating station is the virtual mac address
	     * that maps onto the qllc.  That makes it the Remote MAC
	     * Address.  Conversely the Local MAC address is the
	     * partner address.
	     *
	     * thus:
	     *
	     *  Local MAC address is QLLC partner's MAC address = Dest addr.
	     *  Remote MAC address is QLLC virtual MAC for the qllc connection
	     *   = source address.
	     */
	    
		pak->addr_start = pak->datagramstart + TR_DA_OFFSET;
		pak->info_start = pak->datagramstart + TRING_ENCAPBYTES;
	    
		if (QCepTestPartner(qllc)) {
		    ieee_copy(qllc->fPartner, pak->addr_start);
		} else {
		    ieee_zero(pak->addr_start);
		}
	    
		if (QCepTestVmac(qllc)) {
		    ieee_copy(qllc->fVmac, pak->addr_start+IEEEBYTES);
		} else {
		    ieee_zero(pak->addr_start+IEEEBYTES);
		}
		
		
		dlcXidStn->fLMacLength = IEEEBYTES;
		dlcXidStn->fRMacLength = IEEEBYTES;
		dlcXidStn->fLMacPtr = pak->addr_start;
		dlcXidStn->fRMacPtr = pak->addr_start + IEEEBYTES;
		
		dlcXidStn->fRSap = qllc->fSsap;
		dlcXidStn->fLSap = qllc->fDsap;

		dlcXidStn->fRoutingLength = 0;
		dlcXidStn->fRoutingPtr = NULL;
	    } else {
		/* 
		 *X.25 native form - Use X121 address or lcn for pvc
		 */
		
		ushort	lcn;
		void	*addr;
		uchar	type;
		uchar	length;

		if (!qllc->qllc_ispvc) {
		    type = STATION_X121;
		    length = STATIONLEN_X121;
		    addr = qllc->fRemote;
		} else {
		    lcn = QCepGetLcn(qllc);
		    type = STATION_X25PVC;
		    length = STATIONLEN_X25PVC;
		    addr = &lcn;
		}

		pak_delta_adjust_start(pak, -length);
		dlcXidStn->fLMacLength = length;
		dlcXidStn->fRMacLength = length;

		pak->addr_start = pak->datagramstart;
		pak->info_start = pak->addr_start;
		dlcXidStn->fLMacPtr = pak->addr_start;
		dlcXidStn->fRMacPtr = pak->addr_start;

		dlcXidStn->fMacType = type;
		memcpy(pak->addr_start, addr, length);
	    }	/* end of X.25 native form */
	}
	break;

	/* Part 2.  making a packet look like mock token ring */
	
    case CLS_TEST_STN_IND:
	CLSASSERT(!"Unexpected CLS_TEST_STN_IND");
	/* mama don' 'low no data link switchin' round' here */
	break;
    }
}

/* QllcClsNotify
 *
 * Return TRUE if qllctype under CLSI control...
 */
boolean QllcClsNotify (qllctype *qllc, ClsOpcodeT opcode,
		       ClsErrorCodeT error, paktype *pak)
{
    ClsDlcParamBlockT 	clsDlcParamBlock;
    ClsUCepIdT		uCepId;
    
    uCepId = qllc->fUCepId;
    if (uCepId == NO_CLS) {
        datagram_done(pak);
	return (FALSE);
    }

    memset(&clsDlcParamBlock, 0, sizeof(ClsDlcParamBlockT));
    /* Handle special Op Code not handled by generic BuildParam Block */
    QBuildParamBlock(qllc, opcode, &clsDlcParamBlock, pak);
    DlcBuildParmBlock((DlcPCepIdT)qllc, uCepId, opcode, &clsDlcParamBlock, 
		      error, pak);
    reg_invoke_cls_rput(uCepId, opcode, &clsDlcParamBlock);
    return (TRUE);
}

