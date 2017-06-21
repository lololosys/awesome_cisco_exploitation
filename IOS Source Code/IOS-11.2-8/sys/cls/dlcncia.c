/* $Id: dlcncia.c,v 3.1.2.10 1996/08/28 12:39:25 thille Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlcncia.c,v $
 *------------------------------------------------------------------
 * dlcncia.c - Glue for CLSI/DLC to NCIA
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dlcncia.c,v $
 * Revision 3.1.2.10  1996/08/28  12:39:25  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.9  1996/07/26  06:23:53  schiang
 * CSCdi64000:  remove unused variable ncia_cisco_debug
 * Branch: California_branch
 *
 * Revision 3.1.2.8  1996/07/23  13:20:41  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.2.7  1996/07/11  21:13:52  jolee
 * CSCdi62553:  Move externs from .c to .h that was lost in CSCdi61974
 * Branch: California_branch
 *
 * Revision 3.1.2.6  1996/07/03  06:37:52  jolee
 * CSCdi61974:  Fix NCIA crash if unconfig a client in active open
 * processing
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/06/27  06:05:23  schiang
 * CSCdi61391:  I_CAN_REACH packet is not properly setup
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/06/18  19:01:10  schiang
 * CSCdi60706:  move extern in .c to .h
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/12  06:51:34  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/28  22:33:43  schiang
 * CSCdi58852:  router crashes when ncia server is un-configured
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:42:33  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.4  1996/05/14  18:42:01  schiang
 * CSCdi57622:  ncia start/stop doesnt work
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.3  1996/05/03  22:06:12  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.2  1996/04/24  06:28:20  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.1.4.1  1996/04/05  06:05:35  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:10:09  schiang
 * CSCdi53012:  add placeholder for NCIA files
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
#include "types.h"
#include "ieee.h"
#include "../if/rif.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "../if/if_vidb.h"
#include "address.h"
#include "../cls/clsi_msg.h"
#include "../cls/ncia.h"
#include "../cls/ncia_client.h"
#include "../cls/ncia_server_fsm.h"
#include "../cls/dlc.h"
#include "../cls/ncia_debug.h"

/* local function declarations */
static void nciaInitSystem(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaEnableReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaDisableReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaActivateSapReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaActivateRingReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaModifySapReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaDeactivateSapReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaDeactivateRingReq(NciaCepT *pCepId,ClsDlcParamBlockT *clsBlock);
static void nciaReqOpnStnReq(NciaCepT *NciaCep, ClsDlcParamBlockT *clsBlock);
static void nciaCloseStnReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaModifyStnReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaIdGuts(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock, 
                       nciami_dlu_t opCode);
static void nciaIdReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaIdRsp(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaTestReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaUDataReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaTestStnReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaTestStnRsp(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaUDataStnReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaIdStnReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaConnectReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaConnectRsp(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaSignalStnReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaDataReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaFlowReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaPongReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaDisconnectReq(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);
static void nciaDisconnectRsp(NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock);

static boolean NciaBuildParamBlock(NciaCepT *pCepId, ClsOpcodeT opCode, 
                                ClsDlcParamBlockT *clsBlock);

static void NciaBounceIdStnInd(DlcPCepIdT pCepId, ClsDlcParamBlockT *clsBlock);


/************************************************************************
 *                             CLSI Primitive: (none)
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_INIT_SYSTEM, &parm_block)
 *
 * No Op.
 *
 ************************************************************************
 */

static 
void nciaInitSystem (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: nciaInitSystem");

    return;
}



/************************************************************************
 *                           CLSI Primitive: Enable.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * ncia_wput(*cep, DLC_ENABLE_REQ, &param_block)
 *
 * No Op.
 *
 ************************************************************************
 */

static 
void nciaEnableReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    idbtype         *idb;
    DlcEnableT      *dlcEnable;
    ncia_servertype *ncia_server;
    NCIAEnableT     *nciaDlcParms;
    int              server_num;

    NciaServerDebugAll("\nNCIA: Receive Enable.Req");

    dlcEnable = &clsBlock->fParameters.fDlcEnable;

    if (dlcEnable->fDLCType != CLS_NCIA) {
        dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_UNKNOWN_PORT);
        return;
    }

    /*
     * get idb base on server number.
     */
    nciaDlcParms = (NCIAEnableT *) &(dlcEnable->fMediaEnable.fNCIAEnable);
    server_num   = nciaDlcParms->fServerNum;
    ncia_server  = ncia_get_server_by_index(server_num);

    if (!ncia_server || !(ncia_server->flags & NCIA_SERVER_CONFIGURED)) {
        dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_UNKNOWN_PORT);
        return;
    }

    idb = ncia_server->ncia_swidb;
    if (idb == NULL) {
        dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_UNKNOWN_PORT);
        return;
    }

    /*
     * since all info is in the server control block already,
     * we don't need to copy in again.
     */

    dlcEnable->fPPortId = (DlcPPortIdT) idb;
    dlcEnable->fMediaEnable.fNCIAEnable.fVersion = CLSI_VERSION;

    dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_OK);

    ncia_server->flags |= NCIA_SERVER_ACTIVE;
    ncia_server->enable_count++;
        
    return;
}



/************************************************************************
 *                           CLSI Primitive: Disable.req
 ************************************************************************
 *
 * Disable port , no op command 
 *
 * DLC Message Generated
 * ---------------------
 * ncia_wput(*cep, DLC_DISABLE_REQ, &param_block)
 *
 ************************************************************************
 */

static 
void nciaDisableReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    ncia_servertype *ncia_server;
    idbtype         *idb;
    DlcDisableT     *dlcDisable;

    NciaServerDebugAll("\nNCIA: Receive Disable.Req");

    dlcDisable = &clsBlock->fParameters.fDlcDisable;
    idb = DlcToIdb(&dlcDisable->fPortId);
    if (idb == NULL) {
        dlc_rput(clsBlock, CLS_DISABLE_CNF, CLS_UNKNOWN_PORT);
        return;
    }

    ncia_server = ncia_idb_to_server(idb);
    if (ncia_server->enable_count)
        ncia_server->enable_count--;
    if (!ncia_server->enable_count)
        ncia_server->flags &= ~NCIA_SERVER_ACTIVE;
        
    dlc_rput(clsBlock, CLS_DISABLE_CNF, CLS_OK);
}


/************************************************************************
 *                             CLSI Primitive: ActivateSap.Req
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_ACTIVATE_SAP_REQ, &parm_block)
 *
 ************************************************************************
 */

static 
void nciaActivateSapReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    idbtype         *idb;
    DlcActSapT      *dlcActSap;
    NciaActSapT     *nciaActSap;
    ClsUSapIdT       uSapId;
    ncia_servertype *ncia_server;
    NciaSapT        *sap_registry;
    int              Sap;

    NciaServerDebugAll("\nNCIA: Receive ActivateSap.Req");

    dlcActSap  = &clsBlock->fParameters.fDlcActSap;
    uSapId     = dlcActSap->fUSapId;
    nciaActSap = &dlcActSap->fDlcSap.fNciaActSap;

    idb = (idbtype *)(dlcActSap->fPPortId);
    if (idb == NULL) {
        errmsg(&msgsym(OPTNULL, DLC));
        dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_UNKNOWN_PORT);
        return;
    }

    /* find the SAP Registry */
    ncia_server = ncia_idb_to_server(idb);
    sap_registry = (NciaSapT *) &(ncia_server->sap_registry[0]);

    for (Sap = 1; Sap < NCIA_MAX_SAP; Sap++)
        if (!sap_registry[Sap].fSapValue || 
             sap_registry[Sap].fSapValue == nciaActSap->fLSapValue)
            break;

    if (Sap == NCIA_MAX_SAP || 
        sap_registry[Sap].fSapValue == nciaActSap->fLSapValue) {
        dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_SAP_BUSY);
        return;
    }

    sap_registry[Sap].fSapValue = nciaActSap->fLSapValue;
    sap_registry[Sap].fUSapId = (word) uSapId;
    sap_registry[Sap].fPSapId = (word) sap_registry;

    /* pass back up to CLS core  */
    dlcActSap->fDlcPSapId = (DlcPSapIdT) &sap_registry[Sap]; 
    dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_OK);
    return;
}


/************************************************************************
 *                             CLSI Primitive: ActivateRing.Req
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_ACTIVATE_RING_REQ, &parm_block)
 *
 ************************************************************************
 */

static 
void nciaActivateRingReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    idbtype         *idb;
    DlcActRngT      *dlcActRng;
    NciaActRngT     *nciaActRng;
    NciaSapT        *sap_registry;
    ncia_servertype *ncia_server;
    ClsUSapIdT       uSapId;

    dlcActRng  = &clsBlock->fParameters.fDlcActRng;
    uSapId     = dlcActRng->fClsUSapId;
    nciaActRng = &dlcActRng->fDlcRng.fNciaActRng;

    NciaServerDebugAll("\nNCIA: Receive ActivateRing.Req usap %x", 
                       dlcActRng->fClsUSapId);

    idb = (idbtype *)(dlcActRng->fPPortId);
    if (idb == NULL) {
        errmsg(&msgsym(OPTNULL, DLC));
        dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_UNKNOWN_PORT);
        return;
    }

    /* find the SAP Registry */
    ncia_server = ncia_idb_to_server(idb);
    sap_registry = (NciaSapT *) &(ncia_server->sap_registry[NCIA_DLSW_SAP]);

    if (sap_registry->fSapValue) {
        /*
         * Sap already registered.
         */
        dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_SAP_BUSY);
        return;
    }

    bzero(sap_registry, sizeof(NciaSapT));
    sap_registry->fSapValue = NCIA_SAP_IN_USE_BY_BRIDGE;

    sap_registry->fUSapId   = (word) uSapId;
    sap_registry->fPSapId   = (word) sap_registry;

    /* pass back up to CLS core  */
    dlcActRng->fDlcPSapId = (DlcPSapIdT) sap_registry; 
    dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_OK);

    return;
}



/***********************************************************************
 *                    CLSI Primitive: Modify_sap.req
 ************************************************************************
 * Modifies operational parameters on the SAP
 *
 * DLC Message Generated
 * ---------------------
 * ncia_wput(*cep, ncia_MODIFY_SAP_REQ, &param_block)
 *
 ************************************************************************
 */

static 
void nciaModifySapReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive ModifySap.Req");

    return;
}



/************************************************************************
 *                    CLSI Primitive: Deactivate_sap.req
 ************************************************************************
 */

static 
void nciaDeactivateSapReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    idbtype         *idb;
    DlcActSapT      *dlcActSap;
    ClsUSapIdT       uSapId;
    NciaActSapT     *nciaActSap;
    ncia_servertype *ncia_server;
    NciaSapT        *sap_registry;
    int              Sap;

    NciaServerDebugAll("\nNCIA DeactivateSap.Req");

    dlcActSap  = &clsBlock->fParameters.fDlcActSap;
    uSapId     = dlcActSap->fUSapId;
    nciaActSap = &dlcActSap->fDlcSap.fNciaActSap;

    idb = (idbtype *)(dlcActSap->fPPortId);
    if (idb == NULL) {
        errmsg(&msgsym(OPTNULL, DLC));
        dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_UNKNOWN_PORT);
        return;
    }

    /* find the SAP Registry */
    ncia_server = ncia_idb_to_server(idb);
    sap_registry = (NciaSapT *) &(ncia_server->sap_registry[0]);

    for (Sap = 1; Sap < NCIA_MAX_SAP; Sap++)
        if (sap_registry[Sap].fSapValue == nciaActSap->fLSapValue)
            break;

    NciaServerDebugAll(", sap: %d", nciaActSap->fLSapValue);
    if (Sap == NCIA_MAX_SAP) {
        dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_NO_SAP_REGISTRY);
        return;
    }
 
    sap_registry->fSapValue = 0;

    dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_OK);
}


/************************************************************************
 *                             CLSI Primitive: DeactivateRing.Req
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_DEACTIVATE_RING_REQ, &parm_block)
 *
 ************************************************************************
 */

static 
void nciaDeactivateRingReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    idbtype         *idb;
    DlcDeActRngT    *dlcDeActRng;
    ClsUSapIdT       uSapId;
    NciaDeActRngT   *nciaDeActRng;
    NciaSapT        *sap_registry;
    ncia_servertype *ncia_server;

    NciaServerDebugAll("\nNCIA DeactivateRing.Req");

    dlcDeActRng  = &clsBlock->fParameters.fDlcDeActRng;
    uSapId       = dlcDeActRng->fUSapId;
    nciaDeActRng = &dlcDeActRng->fDlcRng.fNciaDeActRng;

    idb = (idbtype *)(dlcDeActRng->fPPortId);
    if (idb == NULL) {
        errmsg(&msgsym(OPTNULL, DLC));
        dlc_rput(clsBlock, CLS_DEACTIVATE_RING_CNF, CLS_UNKNOWN_PORT);
        return;
    }

    /* find the SAP Registry */
    ncia_server = ncia_idb_to_server(idb);
    if (!ncia_server) {
        dlc_rput(clsBlock, CLS_DEACTIVATE_RING_CNF, CLS_LOCAL_ERROR);
        return;
    }

    sap_registry = (NciaSapT *) &(ncia_server->sap_registry[NCIA_DLSW_SAP]);
    if (!sap_registry) {
        dlc_rput(clsBlock, CLS_DEACTIVATE_RING_CNF, CLS_NO_SAP_REGISTRY);
        return;
    }

    if (sap_registry->fSapValue != NCIA_SAP_IN_USE_BY_BRIDGE) {
        dlc_rput(clsBlock, CLS_DEACTIVATE_RING_CNF, CLS_UNKNOWN_RING);
        return;
    }

    /*
     * free all active circuits
    ncia_free_all_ceps(sap_registry);
     */

    sap_registry->fSapValue = 0;
    sap_registry->fUSapId   = (word) 0;
    sap_registry->fPSapId   = (word) 0;
        
    dlc_rput(clsBlock, CLS_DEACTIVATE_RING_CNF, CLS_OK);
}



/************************************************************************
 *               CLSI Primitive: Req_opnstn.req (outgoing)
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * ncia_wput(*cep, ncia_REQ_OPNSTN_REQ, &parm_block)
 *
 * Error Conditions
 * ----------------
 * Could not allocate NciaCepT memory structure.
 *
 ************************************************************************
 */

static 
void nciaReqOpnStnReq (NciaCepT *NciaCep, ClsDlcParamBlockT *clsBlock)
{
    NciaSapT        *NciaSap;
    DlcReqOpenStnT  *dlcReqOpenStn;
    NciaReqOpenStnT *NciaReqOpenStn;
    ClsUCepIdT       uCepId;
    idbtype         *idb;
    nciapeertype    *NciaClient;
    ncia_servertype *ncia_server;

    NciaServerDebugAll("\nNCIA: Receive ReqOpnStn.Req");

    CLSASSERT(NciaCep == (NciaCepT *)CLS_NO_CEP);
        
    dlcReqOpenStn = &clsBlock->fParameters.fDlcReqOpenStn;
    NciaReqOpenStn = &dlcReqOpenStn->fDlcStn.fNciaReqOpenStn;
    uCepId = dlcReqOpenStn->fClsUCepId;
    
    NciaServerDebugAll(" dmac:%e smac:%e dsap:%d ssap:%d, ucep:%x",
        NciaReqOpenStn->fDestAddr.addr, NciaReqOpenStn->fSrcAddr.addr,
        NciaReqOpenStn->fDestSap, NciaReqOpenStn->fSrcSap, uCepId);

    idb = (idbtype *)dlcReqOpenStn->fPPortId;
    if (idb == NULL) {
        errmsg(&msgsym(OPTNULL, DLC));
        DlcReqOpenStnConf((DlcPCepIdT) 0, uCepId, CLS_UNKNOWN_PORT);
        return;
    }

    ncia_server = ncia_idb_to_server(idb);

    NCIA_CHECK_SERVER_IS_UP(ncia_server, clsBlock);

    NciaSap = (NciaSapT *)dlcReqOpenStn->fDlcPSapId;

    if (ncia_cep_find_opened(NciaSap, uCepId)) {
        /*
         * duplicated uCep.
         */
        NciaServerDebugAll(": duplicated uCep %x", uCepId);
        DlcReqOpenStnConf((DlcPCepIdT) 0, uCepId, CLS_SAP_BUSY);
        return;
    }

    NciaClient = ncia_find_client_by_mac(idb, NciaReqOpenStn->fDestAddr.addr);

    if (!NciaClient) {
        NciaServerDebugAll(": can't find client control block");
        DlcReqOpenStnConf((DlcPCepIdT) 0, uCepId,  CLS_NO_REM_STN);
        return;
    }

    /* the circuit control block might have been created */
    /* we have to provide the local src mac.  This is from DSPU */
    if (NciaSap->fSapValue != NCIA_SAP_IN_USE_BY_BRIDGE)
        ieee_copy(&(ncia_server->server_mac[0]), 
                  &(NciaReqOpenStn->fSrcAddr.addr[0]));

    NciaCep = ncia_find_circuit(NciaClient, NciaReqOpenStn->fSrcAddr.addr, 
        NciaReqOpenStn->fSrcSap, NciaReqOpenStn->fDestSap);

    /* create one if one can't be found */
    if (!NciaCep)
        NciaCep = ncia_circuit_create(NciaClient, NciaReqOpenStn->fSrcAddr.addr,
            NciaReqOpenStn->fSrcSap, NciaReqOpenStn->fDestSap, NciaSap);

    if (NciaCep == NULL) {
        NciaServerDebugAll(": can't create circuit control block");
        DlcReqOpenStnConf((DlcPCepIdT) 0, uCepId,  CLS_OUT_OF_MEMORY);
        return;
    }
        
    if (NciaCep->fState == nciast_opening) {
        NciaServerDebugAll(
            "\n      waiting for client response. send ReqOpenStnConf");
        DlcReqOpenStnConf((DlcPCepIdT) 0, uCepId,  CLS_TIMEOUT);
        /* use the new uCep */
    } else if (NciaCep->fState == nciast_dl_started_snd) {
        NciaServerDebugAll("\n      xid has sent to DLU");
    } else if (NciaCep->fState != nciast_closed) {
        NciaCircuitDebugAll("\n      invalid circuit state %x",
                            NciaCep->fState);
        DlcReqOpenStnConf((DlcPCepIdT) 0, uCepId,  CLS_SAP_BUSY);
        return;
    }

    NciaCep->fUCepId = (word) uCepId;

    if (NciaCep->fPacket) {
        datagram_done(NciaCep->fPacket);
        NciaCep->fPacket = NULL;
        NciaServerDebugAll("\nNCIA: invalid packet in nciaReqOpnStnReq");
    }

    /*
     * it will get inserted into SAP's Cep list after connect successfully
     */

    /* give it to the FSM */
    if (ncia_server_fsm(NciaCep, nciama_dlu, nciami_cls_req_opn_stn_req) < 0) {
	DlcReqOpenStnConf((DlcPCepIdT) 0, uCepId,  CLS_LOCAL_ERROR);
        nciaClearCircuit(NciaClient, NciaCep);
    } else {
        if (NciaCep->fNciaCepFlags & NciaFreeCkt)
            nciaClearCircuit(NciaClient, NciaCep);
        else
            dlcReqOpenStn->fDlcPCepId = (DlcPCepIdT) NciaCep;
    }
    return;
}



/************************************************************************
 *                             CLSI Primitive: CloseStn.Req
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_CLOSE_STN_REQ, &parm_block)
 *
 ************************************************************************
 */

static 
void nciaCloseStnReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive CloseStn.Req");

    CLSASSERT(pCepId != NO_CLS);  

    if (pCepId == (NciaCepT *)CLS_NO_CEP) {
        /* 
         * Trying to cancel a pending ReqOpnStn. CLS did not know the 
         * p_cep_id, but it does know the u_cep_id it sent us in 
         * the ReqOpnStn.req. We need to scan all ncia structures to 
         * find the correct one to close.
         */

        ClsUCepIdT uCepId;

        uCepId = clsBlock->fParameters.fDlcCloseStn.fClsUCepId;

        /* scan for pCepId->fUCepId */
        pCepId = ncia_cep_search_by_ucepid(uCepId);
    }

    if (pCepId == (NciaCepT *)CLS_NO_CEP || pCepId == NO_CLS) {
        errmsg(&msgsym(INVPCEP, DLC));
        NciaServerDebugAll(", ncia cep is not found.");
            dlc_rput(clsBlock, CLS_CLOSE_STN_CNF, CLS_UNKNOWN_CEP);
        return;
    } else {
        /* give it to the FSM */
        ncia_server_fsm(pCepId, nciama_dlu, nciami_cls_close_stn_req); 
    }

    nciaClearCircuit(pCepId->fNciaClient, pCepId);

    return;
}



void nciaClearCircuit (nciapeertype *ncia_client, NciaCepT *pCepId)
{
    p_unqueue(&ncia_client->NciaCircuit, pCepId);
    if (pCepId->fPacket)
        datagram_done(pCepId->fPacket);
    if (pCepId->fXid)
        datagram_done(pCepId->fXid);

    NciaServerDebugAll("\nNCIA: ckt id (%x) is freed", pCepId);
    free(pCepId);

    if (!ncia_client->NciaCircuit.qhead)
        ncia_client_fsm(ncia_client, CIRCUIT_FREED);
}



void nciaClearCircuitOnly (nciapeertype *ncia_client, NciaCepT *pCepId)
{
    p_unqueue(&ncia_client->NciaCircuit, pCepId);
    if (pCepId->fPacket)
        datagram_done(pCepId->fPacket);
    if (pCepId->fXid)
        datagram_done(pCepId->fXid);

    NciaServerDebugAll("\nNCIA: ckt id (%x) is freed", pCepId);
    free(pCepId);

}



/************************************************************************
 *                             CLSI Primitive: ModifyStn.Req
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_MODIFY_STN_REQ, &parm_block)
 *
 * NOT SUPPORTED !!!
 *
 ************************************************************************
 */

static 
void nciaModifyStnReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive ModifyStn.Req");
    return;
}




static 
void nciaIdGuts (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock, 
                 nciami_dlu_t opCode)
{
    DlcIdStnT *dlcIdStn;

    CLSASSERT(pCepId != NO_CLS &&         /* no legacy code allowed here */
              pCepId != (NciaCepT *)CLS_NO_CEP); /* pCep should be valid */ 

    dlcIdStn = &clsBlock->fParameters.fDlcIdStn;
    pCepId->fPacket = clsBlock->fPakPtr;
    clsBlock->fPakPtr = NULL;

    if (dlcIdStn->fXidT == XID_COMMAND) {
        pCepId->fNciaCepFlags |= NciaXidRspPending;
    }
    
    /* give it to the FSM */
    ncia_server_fsm(pCepId, nciama_dlu, opCode);        

    return;
}


/************************************************************************
 *                         CLSI Primitive: Id.Req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, DLC_ID_REQ, &param_block)
 *
 * Error Conditions
 * ----------------
 * No memory for packet
 *
 ************************************************************************
 */

static 
void nciaIdReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive Id.Req");

    nciaIdGuts(pCepId, clsBlock, nciami_cls_id_req);
}


/************************************************************************
 *                             CLSI Primitive: Id.Rsp
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_ID_RSP, &parm_block)
 *
 ************************************************************************
 */

static 
void nciaIdRsp (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive Id.Rsp");

    nciaIdGuts(pCepId, clsBlock, nciami_cls_id_rsp);
}



/************************************************************************
 *                             CLSI Primitive: Test.Req
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_TEST_REQ, &parm_block)
 *
 ************************************************************************
 */
static 
void nciaTestReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    ncia_servertype *ncia_server;

    NciaServerDebugAll("\nNCIA: Receive Test.Req");

    ncia_server = pCepId->fNciaClient->ncia_server;

    NCIA_CHECK_SERVER_IS_UP(ncia_server, clsBlock);

    if (ncia_server->flags & NCIA_INBOUND_ONLY) {
        NciaServerDebugAll(". Server is for inbound only. Frame discard.");
        /* don't respond if we can't connect out */
        NCIA_FREE_PAK(clsBlock);
        return;
    }

    pCepId->fPacket = clsBlock->fPakPtr;
    clsBlock->fPakPtr = NULL;
    pCepId->fNciaCepFlags |= NciaTestRspPending;

    /* give it to the FSM */
    ncia_server_fsm(pCepId, nciama_dlu, nciami_cls_test_req);
}



/************************************************************************
 *                             CLSI Primitive: Udata.Req
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_UDATA_REQ, &parm_block)
 * For NetBIOS.  We assume SNA doesn't send UData frames.
 *
 ************************************************************************
 */
static 
void nciaUDataReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive UData.Req");

    if (pCepId && pCepId->fNciaClient) {
        NCIA_CHECK_SERVER_IS_UP((pCepId->fNciaClient->ncia_server), clsBlock);
    }

    /* give it to the FSM */
    pCepId->fPacket = clsBlock->fPakPtr;
    clsBlock->fPakPtr = NULL;
    ncia_server_fsm(pCepId, nciama_dlu, nciami_cls_udata_req);
}


/************************************************************************
 *                             CLSI Primitive: TestStn
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_TEST_STN_REQ, &parm_block)
 *
 ************************************************************************
 */

static 
void nciaTestStnReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    DlcTestStnT     *dlcTestStn;
    nciapeertype    *NciaClient;
    idbtype         *idb;
    ncia_servertype *ncia_server;

    dlcTestStn = &clsBlock->fParameters.fDlcTestStn;

    NciaServerDebugAll("\nNCIA: Receive TestStn.Req (%e -> %e).",
        dlcTestStn->fRMac, dlcTestStn->fLMac);

    idb = (idbtype *) clsBlock->fPortId.fTypeInfo.swidb;
    ncia_server = ncia_idb_to_server(idb);

    if (ncia_server->flags & NCIA_INBOUND_ONLY) {
        NciaServerDebugAll("\n      Server is for inbound only.  Frame discard.",
            ncia_server->flags);
        NCIA_FREE_PAK(clsBlock);
        return;
    }

    NciaClient = ncia_find_client_by_mac(idb, dlcTestStn->fRMac);
    if (!NciaClient) {
        NciaServerDebugAll(" Not reachable.");
        NCIA_FREE_PAK(clsBlock);
        return;
    }

    NCIA_CHECK_SERVER_IS_UP(ncia_server, clsBlock);

    if (!(ncia_server->flags & NCIA_INBOUND_ONLY) ||
        NciaClient->client_state == NCIA_OPENED ||
        NciaClient->client_state == NCIA_BUSY) {
        /*
         * if this server supports connect-out, or the tcp session is up
         */
        if (NciaClient && 
            ncia_check_client_sap(NciaClient, dlcTestStn->fRSap)) {
            NciaServerDebugAll("\nNCIA: send TestStn.Ind");
            DlcBounceTestStnInd((DlcPCepIdT)pCepId, clsBlock);
            return;
        }
    }
    NCIA_FREE_PAK(clsBlock);
}



/************************************************************************
 *                             CLSI Primitive: TestStn.Rsp
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_TEST_STN_RSP, &parm_block)
 *
 ************************************************************************
 */

static 
void nciaTestStnRsp (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    DlcTestStnT  *dlcTestStn;
    nciapeertype *NciaClient;
    idbtype      *idb;

    NciaServerDebugAll("\nNCIA: Receive TestStn.Rsp");

    dlcTestStn = &clsBlock->fParameters.fDlcTestStn;

    idb = (idbtype *) clsBlock->fPortId.fTypeInfo.swidb;

    NciaClient = ncia_find_client_by_mac(idb, dlcTestStn->fRMac);
    if (!NciaClient) {
        NCIA_FREE_PAK(clsBlock);
        /* no client info is available. */
        return;
    }

    if (NciaClient->client_state != NCIA_CLOSED) {
        NciaCepT *ncia_ptr;
        boolean   found = FALSE;

        /*
         * only send I_CAN_REACH when the tcp is up
         */
        clsBlock->fPakPtr->datagramsize = dlcTestStn->fTESTLength;
 
        if (clsBlock->fLoadLength) {
            NciaCircuitDebugAll("\nNCIA move datagramstart from %x to %x",
                   clsBlock->fPakPtr->datagramstart, dlcTestStn->fTESTPtr);
            clsBlock->fPakPtr->datagramstart = dlcTestStn->fTESTPtr;
        }
 
        ForEachNciaCircuit(NciaClient, ncia_ptr) {
            if (ieee_equal(ncia_ptr->fTargetAddr, dlcTestStn->fLMac) &&
               (ncia_ptr->fClientSap == dlcTestStn->fRSap)) {
                if (ncia_ptr->fnext) {
                    ncia_ptr->fPacket = pak_duplicate(clsBlock->fPakPtr);
                       if (!ncia_ptr->fPacket) {
                           NCIA_FREE_PAK(clsBlock);
                           return;
                       }
                } else {
                    ncia_ptr->fPacket = clsBlock->fPakPtr;
                    clsBlock->fPakPtr = NULL;
                }

                /* give it to the FSM if circuit is available */
                ncia_server_fsm(ncia_ptr, nciama_dlu, nciami_cls_test_stn_rsp);
                found = TRUE;
            }
        }

        if (!found)
            ncia_send_ndlc_no_ckt(NciaClient, clsBlock, DLC_TEST_STN_RSP);
        else
            NCIA_FREE_PAK(clsBlock);
    } else 
        NCIA_FREE_PAK(clsBlock);
}


/************************************************************************
 *                             CLSI Primitive: UDataStn.Req
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_UDATA_STN_REQ, &parm_block)
 *
 ************************************************************************
 */

static 
void nciaUDataStnReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    ncia_servertype *ncia_server;
    DlcUdataStnT    *dlcUdataStn;
    nciapeertype    *NciaClient;
    idbtype         *idb;
    NciaCepT        *ncia_ptr;

    NciaServerDebugAll("\nNCIA: Receive UDataStn.Req");

    dlcUdataStn = &clsBlock->fParameters.fDlcUdataStn;
    idb = (idbtype *) clsBlock->fPortId.fTypeInfo.swidb;
    ncia_server = ncia_idb_to_server(idb);

    NCIA_CHECK_SERVER_IS_UP(ncia_server, clsBlock);

    if (ncia_server->num_netbios_stn == 0) {
        /*
         * no client supports NetBIOS
         */
        NCIA_FREE_PAK(clsBlock);
        return;
    }

    /* check if it is sent to a particular client */
    NciaClient = ncia_find_client_by_mac(idb, dlcUdataStn->fRMac);
    if (NciaClient && (NciaClient->NciaFlags & NCIA_CLIENT_NETBIOS &&
                      (NciaClient->client_state == NCIA_OPENED ||
                       NciaClient->client_state == NCIA_BUSY))) {

        ncia_ptr = ncia_find_circuit(NciaClient, dlcUdataStn->fLMac,
                           dlcUdataStn->fLSap, dlcUdataStn->fRSap);

        clsBlock->fPakPtr->datagramsize = dlcUdataStn->fUDataLength;
        if (clsBlock->fLoadLength)
            clsBlock->fPakPtr->datagramstart = dlcUdataStn->fUDataPtr;

        if (ncia_ptr) {
            ncia_ptr->fPacket = clsBlock->fPakPtr;
            clsBlock->fPakPtr = NULL;
            /* give it to the FSM if circuit is available */
            ncia_server_fsm(ncia_ptr, nciama_dlu, nciami_cls_udata_stn_req);
        } else
            ncia_send_ndlc_no_ckt(NciaClient, clsBlock, DLC_UDATA_STN_REQ);

    } else {
        /* 
         * we use num_netbios_stn to decide if how many times we need to
         * duplicate a packet.
         */
        int      num = ncia_server->num_netbios_stn;
        int      error = 0;
        paktype *pak;

        clsBlock->fPakPtr->datagramsize = dlcUdataStn->fUDataLength;
        if (clsBlock->fLoadLength)
            clsBlock->fPakPtr->datagramstart = dlcUdataStn->fUDataPtr;

        ForEachNciaClient(ncia_server, NciaClient) {
            if (NciaClient->NciaFlags & NCIA_CLIENT_NETBIOS) {
                if(NciaClient->client_state == NCIA_OPENED ||
                   NciaClient->client_state == NCIA_BUSY) {
                    /*
                     * if this server supports NetBIOS and the tcp session is up
                     */
                    if (num) {
                        pak = pak_duplicate(clsBlock->fPakPtr);
                        if (!pak) {
                             NCIA_FREE_PAK(clsBlock);
                             return;
                        }
                        ncia_send_ndlc_no_ckt(NciaClient, clsBlock, 
                                              DLC_UDATA_STN_REQ);
                        clsBlock->fPakPtr = pak;
                        num--;
                    } else
                        error++;
                } else
                    num--;
            }
        }
        if (error)
        NciaCircuitDebugError(
          "\nNCIA: incorrect NetBIOS station count: should be %d instead of %d",
          ncia_server->num_netbios_stn + error, ncia_server->num_netbios_stn);
        NCIA_FREE_PAK(clsBlock);
    }
}


/************************************************************************
 *                             CLSI Primitive: IdStn.Req
 ************************************************************************
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_ID_STN_REQ, &parm_block)
 *
 * We only handle NULL XID for ID_STN.
 *
 ************************************************************************
 */

static 
void nciaIdStnReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaCepT     *ncia_ptr;
    DlcIdStnT    *dlcIdStn;
    nciapeertype *ncia_client;

    NciaServerDebugAll("\nNCIA: Receive IdStn.Req");

    dlcIdStn = &clsBlock->fParameters.fDlcIdStn;
 
    /*
     * only response to null xid
     */
    if (dlcIdStn->fXIDLength == 0 ||
        clsBlock->fPortId.type != CLS_P_TYPE_SWIDB ||
        !clsBlock->fPortId.fTypeInfo.swidb) {
        NCIA_FREE_PAK(clsBlock);
        return;
    }

    ncia_client = ncia_find_client_by_mac(
                      (idbtype *) clsBlock->fPortId.fTypeInfo.swidb, 
                      dlcIdStn->fRMac);

    /* can't find client */
    if (!ncia_client) {
        NCIA_FREE_PAK(clsBlock);
        return;
    }

    NCIA_CHECK_SERVER_IS_UP((ncia_client->ncia_server), clsBlock);

    ncia_ptr = ncia_find_circuit(ncia_client, dlcIdStn->fLMac,
                       dlcIdStn->fLSap, dlcIdStn->fRSap);
 
    if (ncia_ptr) {
        /* give it to the FSM */
        ncia_ptr->fNciaCepFlags |= NciaXidRspPending;
        ncia_server_fsm(ncia_ptr, nciama_dlu, nciami_cls_id_stn_req);
    } else {
        /* no existing circuit available */
        if (ncia_check_client_sap(ncia_client, dlcIdStn->fRSap))
            NciaBounceIdStnInd((DlcPCepIdT) pCepId, clsBlock);
    }
}



/*
 * use the received IdStnReq to build IdStnInd and send it back 
 * same as DlcBounceTestStnInd()
 */
static 
void NciaBounceIdStnInd (DlcPCepIdT pCepId, ClsDlcParamBlockT *clsBlock)
{
    DlcIdStnT    *dlcIdStn;
    LlcXidIdStnT *llcXidStn;
    idbtype      *swidb;
    int           size;
    paktype      *pak;
 
    NciaServerDebugAll("\nNCIA: Receive BounceIdStnInd");

    dlcIdStn = &clsBlock->fParameters.fDlcIdStn;
    llcXidStn = &dlcIdStn->fDlcXid.fLlcXidStn;
    pak = clsBlock->fPakPtr;
    swidb = DlcToIdb(&clsBlock->fPortId);
 
    size = TR_ACFC_SIZE + (2 * IEEEBYTES) + SAP_HDRBYTES +
        clsBlock->fLoadLength;
 
    pak->datagramsize = size;
 
    /* find a word aligned place */
    pak->datagramstart -= TR_ACFC_SIZE + (2 * IEEEBYTES);
    pak->addr_start = pak->datagramstart + TR_ACFC_SIZE;
    pak->info_start = pak->addr_start + (2 * IEEEBYTES);
    pak->network_start = pak->info_start + SAP_HDRBYTES;
    pak->rif_start = NULL;
    pak->riflen = 0;
    bcopy(clsBlock->fRawData, pak->network_start, clsBlock->fLoadLength);
 
    llcXidStn->fMacType = STATION_IEEE48;
    llcXidStn->fLMacLength = IEEEBYTES;
    llcXidStn->fLMacPtr = pak->addr_start;
    llcXidStn->fRMacPtr = pak->addr_start + IEEEBYTES;
    llcXidStn->fRoutingLength = 0;
    llcXidStn->fRoutingPtr = NULL;
 
    /* Swap addresses in packet */
    ieee_copy(dlcIdStn->fLMac, pak->addr_start);
    ieee_copy(dlcIdStn->fRMac, pak->addr_start + IEEEBYTES);
 
    /* transfer the SAP's between structs */
    llcXidStn->fLSap = dlcIdStn->fLSap & ~SAP_RESPONSE;
    llcXidStn->fRSap = dlcIdStn->fRSap | SAP_RESPONSE;
 
    /* Set network_start to point at start of test data */
    pak->network_start = pak->info_start + SAP_HDRBYTES;

    dlcIdStn->fXIDLength = pak->datagramsize -
                           (pak->network_start - pak->datagramstart);
    dlcIdStn->fXIDPtr = pak->network_start;

    dlcIdStn->fXidT        = XID_RESPONSE;
    dlcIdStn->fDlcPCepId   = (DlcPCepIdT) pCepId;
    dlcIdStn->fClsUCepId   = (ClsUCepIdT) CLS_NO_CEP;

    clsBlock->fResultError = CLS_OK;
    clsBlock->fPakPtr      = pak;
    clsBlock->fPortId.type = CLS_P_TYPE_SWIDB;
    clsBlock->fPortId.fTypeInfo.swidb = swidb;

    reg_invoke_cls_rput((ClsUCepIdT) CLS_NO_CEP, CLS_ID_STN_IND, clsBlock);
    return;
}



/************************************************************************
 *                      CLSI Primitive: Connect.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, ncia_CONNECT_REQ, &param_block)
 *
 ************************************************************************
 */

static 
void nciaConnectReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive Connect.Req");

    if (!pCepId) {
        clsBlock->fResultError = CLS_UNKNOWN_CEP;
        return;
    }

    if (pCepId && pCepId->fNciaClient) {
        NCIA_CHECK_SERVER_IS_UP((pCepId->fNciaClient->ncia_server), clsBlock);
    }

    /* give it to the FSM */
    ncia_server_fsm(pCepId, nciama_dlu, nciami_cls_conn_req); 
    
    return;
}



/************************************************************************
 *                     CLSI Primitive: Connect.rsp
 *************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_CONNECT_RSP, &param_block)
 *         cookie should be LLC's p_cep_id
 *
 ***********************************************************************
 */

static 
void nciaConnectRsp (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive Connect.Rsp");

    CLSASSERT(pCepId != NO_CLS &&        /* no legacy code allowed here */
              pCepId != (NciaCepT *)CLS_NO_CEP); /* CEP should be valid */ 

    /* give it to the FSM */
    ncia_server_fsm(pCepId, nciama_dlu, nciami_cls_conn_rsp);        
   
    return;
}



/************************************************************************
 *                     CLSI Primitive: Signal_stn.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_SIGNAL_STN_REQ, &param_block)
 *
 **********************************************************************
 */

static 
void nciaSignalStnReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive SignalStn.Req");

    CLSASSERT(pCepId != NO_CLS &&        /* no legacy code allowed here */
              pCepId != (NciaCepT *)CLS_NO_CEP); /* CEP should be valid */ 

    /* give it to the FSM */
    ncia_server_fsm(pCepId, nciama_dlu, nciami_cls_flow_start); 
   
    return;
}



/************************************************************************
 *                       CLSI Primitive: Data.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_DATA_REQ, &param_block)
 *
 **********************************************************************
 */

static 
void nciaDataReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive Data.Req");

    CLSASSERT(pCepId != NO_CLS &&        /* no legacy code allowed here */
              pCepId != (NciaCepT *)CLS_NO_CEP); /* CEP should be valid */ 

    if (pCepId && pCepId->fNciaClient) {
        NCIA_CHECK_SERVER_IS_UP((pCepId->fNciaClient->ncia_server), clsBlock);
    }

    pCepId->fPacket = clsBlock->fPakPtr;
    clsBlock->fPakPtr = NULL;

    /* give it to the FSM */
    ncia_server_fsm(pCepId, nciama_dlu, nciami_cls_data_req); 

    return;
}



/************************************************************************
 *                        CLSI Primitive: Flow.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_FLOW_REQ, &param_block)
 *
 * CLSI Primitive
 * --------------
 * Flow.ind
 *
 * DLC Message Generated
 * ---------------------
 * cls_rput(cookie, CLS_FLOW_IND, &param_block)
 *
 * ***********************************************************************/
static 
void nciaFlowReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive Flow.Req");

    CLSASSERT(pCepId != NO_CLS &&        /* no legacy code allowed here */
              pCepId != (NciaCepT *)CLS_NO_CEP); /* CEP should be valid */ 

    /* give it to the FSM */
    if (clsBlock->fParameters.fFlowLevel == DLC_CONGESTION_HIGH)
        ncia_server_fsm(pCepId, nciama_dlu, nciami_cls_flow_stop); 
    else
        ncia_server_fsm(pCepId, nciama_dlu, nciami_cls_flow_start); 

    return;
}



/************************************************************************
 *                             CLSI Primitive: Pong.req
 ************************************************************************
 * Set up for generalized Pong Handling, and off we go
 *
 * DLC Message Generate
 * ------------------------------
 * ncia_wput(*cep, DLC_PONG_REQ, &parm_block)
 *
 * NOT SUPPORTED !!!
 *
 ************************************************************************
 */
static 
void nciaPongReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive Pong.Req");

    return;
}



/************************************************************************
 *                    CLSI Primitive: Disconnect.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_DISC_REQ, &param_block)
 *
 **********************************************************************
 */

static 
void nciaDisconnectReq (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive Disconnect.Req");

    CLSASSERT(pCepId != NO_CLS &&     /* no legacy code allowed here */
              pCepId != (NciaCepT *)CLS_NO_CEP); /* CEP should valid */ 

    pCepId->fNciaCepFlags |= NCIA_DISC_CNF_PENDING;

    /* give it to the FSM */
    ncia_server_fsm(pCepId, nciama_dlu, nciami_cls_disc_req); 
   
    return;
}



/************************************************************************
 *                    CLSI Primitive: Disconnect.rsp
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_DISC_REQ, &param_block)
 *
 **********************************************************************
 */

static 
void nciaDisconnectRsp (NciaCepT *pCepId, ClsDlcParamBlockT *clsBlock)
{
    NciaServerDebugAll("\nNCIA: Receive Disconnect.Rsp");

    CLSASSERT(pCepId != NO_CLS &&        /* no legacy code allowed here */
              pCepId != (NciaCepT *)CLS_NO_CEP); /* CEP should be valid */

    /* give it to the FSM */
    ncia_server_fsm(pCepId, nciama_dlu, nciami_cls_disc_rsp); 
   
    return;
}



void ncia_wput (DlcPCepIdT pCepId, DlcOpcodeT opcode, 
                ClsDlcParamBlockT *clsBlock)
{
    switch (opcode)
    {
    case DLC_INIT_SYSTEM_REQ:
        nciaInitSystem((NciaCepT *)pCepId, clsBlock); break;
        
    case DLC_ENABLE_REQ:
        nciaEnableReq((NciaCepT *)pCepId, clsBlock); break;
        
    case DLC_DISABLE_REQ:
        nciaDisableReq((NciaCepT *)pCepId, clsBlock); break;

    case DLC_ACTIVATE_SAP_REQ:
        nciaActivateSapReq((NciaCepT *)pCepId, clsBlock); break;
        
    case DLC_MODIFY_SAP_REQ:
        nciaModifySapReq((NciaCepT *)pCepId, clsBlock); break;
        
    case DLC_DEACTIVATE_SAP_REQ:
        nciaDeactivateSapReq((NciaCepT *)pCepId, clsBlock); break;
        
    case DLC_REQ_OPNSTN_REQ:
        nciaReqOpnStnReq((NciaCepT *)pCepId, clsBlock); break;
        
    case DLC_CLOSE_STN_REQ:
        nciaCloseStnReq((NciaCepT *)pCepId, clsBlock); break;
        
    case DLC_MODIFY_STN_REQ:
        nciaModifyStnReq((NciaCepT *)pCepId, clsBlock); break;
        
    case DLC_ID_REQ:
        nciaIdReq((NciaCepT *)pCepId, clsBlock); break;
        
    case DLC_CONNECT_REQ:
        nciaConnectReq((NciaCepT *)pCepId, clsBlock); break;
        
    case DLC_CONNECT_RSP:
        nciaConnectRsp((NciaCepT *)pCepId, clsBlock); break;

    case DLC_SIGNAL_STN_REQ:
        nciaSignalStnReq((NciaCepT *)pCepId, clsBlock); break;

    case DLC_DISC_REQ:
        nciaDisconnectReq((NciaCepT *)pCepId, clsBlock); break;

    case DLC_DISC_RSP:
        nciaDisconnectRsp((NciaCepT *)pCepId, clsBlock); break;

    case DLC_DATA_REQ:
        nciaDataReq((NciaCepT *)pCepId, clsBlock); break;

    case DLC_FLOW_REQ:
        nciaFlowReq((NciaCepT *)pCepId, clsBlock); break;

    case DLC_PONG_REQ:
        nciaPongReq((NciaCepT *)pCepId, clsBlock); break;

    case DLC_ACTIVATE_RING_REQ:
        nciaActivateRingReq((NciaCepT *)pCepId, clsBlock); break;
        break;

    case DLC_DEACTIVATE_RING_REQ:
            nciaDeactivateRingReq((NciaCepT *)pCepId, clsBlock);
        break;

    case DLC_ID_RSP:
        nciaIdRsp((NciaCepT *)pCepId, clsBlock); break;

    case DLC_TEST_REQ:
        nciaTestReq((NciaCepT *)pCepId, clsBlock);
        break;

    case DLC_TEST_RSP:
        /* we never send Test.Ind (cmd) so we should never receive response */
        NCIA_FREE_PAK(clsBlock);
        CLSASSERT(!"nciaTestRsp() unexpected");
        break;

    case DLC_UDATA_REQ:
            nciaUDataReq((NciaCepT *)pCepId, clsBlock);
        break;
 
    case DLC_ID_STN_REQ:
        nciaIdStnReq((NciaCepT *)pCepId, clsBlock); break;

    case DLC_ID_STN_RSP:
        NCIA_FREE_PAK(clsBlock);
        CLSASSERT(!"nciaIdStnRsp() unexpected");
        break;

    case DLC_TEST_STN_REQ:
            nciaTestStnReq((NciaCepT *)pCepId, clsBlock);
        break;

    case DLC_TEST_STN_RSP:
            nciaTestStnRsp((NciaCepT *)pCepId, clsBlock);
        break;

    case DLC_UDATA_STN_REQ:
        nciaUDataStnReq((NciaCepT *)pCepId, clsBlock);
        break;

    default:
        NCIA_FREE_PAK(clsBlock);
        CLSASSERT(!"ncia_wput: unknown opcode: 0x%x!!\n");
    }
}



/* 
 * NciaBuildParamBlock - Add ncia specific things to Parameter block
 */
static 
boolean NciaBuildParamBlock (NciaCepT *pCepId, ClsOpcodeT opCode, 
        ClsDlcParamBlockT *clsBlock)
{
    if (!pCepId || !clsBlock) {
	CLSASSERT(!"NciaBuildParamBlock: invalid pCepId or clsBlock\n");
	return(FALSE);
    }

    clsBlock->fPortId.type = CLS_P_TYPE_SWIDB;
    clsBlock->fPortId.fTypeInfo.swidb = 
                               pCepId->fNciaClient->ncia_server->ncia_swidb;
    clsBlock->fLoadLength = 0;

    switch (opCode) {
    case CLS_DATA_IND:
         CLSASSERT_FATAL(clsBlock->fPakPtr);
         clsBlock->fLoadLength = clsBlock->fPakPtr->datagramsize;
         break;

    case CLS_REQ_OPNSTN_CNF:
         {
             DlcReqOpenStnT *ReqOpenStn = &clsBlock->fParameters.fDlcReqOpenStn;
             NciaReqOpenStnT *NciaReqOpenStn;

             ReqOpenStn->fPPortId = 
                                  pCepId->fNciaClient->ncia_server->ncia_swidb;
             ReqOpenStn->fDlcPSapId = (DlcPSapIdT) pCepId->fPSap;
             ReqOpenStn->fClsUCepId = (ClsUCepIdT) pCepId->fUCepId;
             ReqOpenStn->fDlcPCepId = (DlcPCepIdT) pCepId;;

             NciaReqOpenStn = &ReqOpenStn->fDlcStn.fNciaReqOpenStn;
             snpa_init(&NciaReqOpenStn->fDestAddr, STATION_IEEE48, 
                       TARGET_UNICAST, &pCepId->fNciaClient->Vmac[0]);
             snpa_init(&NciaReqOpenStn->fSrcAddr, STATION_IEEE48, 
                       TARGET_UNICAST, &pCepId->fTargetAddr[0]);
             NciaReqOpenStn->fDestSap = pCepId->fClientSap;
             NciaReqOpenStn->fSrcSap = pCepId->fTargetSap;
         }
         break;

    case CLS_ID_IND:
         {
             DlcIdT  *dlcId = &clsBlock->fParameters.fDlcId;
             paktype *pak = clsBlock->fPakPtr;
 
             CLSASSERT_FATAL(clsBlock->fPakPtr);

             clsBlock->fResultError = CLS_OK;
             if (pCepId->fNciaCepFlags & NciaXidRspPending) {
                 pCepId->fNciaCepFlags &= ~NciaXidRspPending;
                 dlcId->fXidT = XID_RESPONSE;
             } else
                 dlcId->fXidT = XID_COMMAND;
             dlcId->fDlcPCepId = (DlcPCepIdT) pCepId;
             dlcId->fClsUCepId = (ClsUCepIdT) pCepId->fUCepId;
             dlcId->fClsUSapId = (ClsUSapIdT) pCepId->fPSap->fUSapId;

             dlcId->fXIDLength = pak->datagramsize;
             dlcId->fXIDPtr = pak->datagramstart;

             pak->network_start = pak->datagramstart;
         }
         break;

    case CLS_CONNECT_IND:
    case CLS_CONNECT_CNF:
         {
             DlcConnectT  *dlcConnect = &clsBlock->fParameters.fDlcConnect;
             NciaReqOpenStnT *NciaReqOpenStn = 
                                    &dlcConnect->fDlcMod.fNciaReqOpenStn;

             dlcConnect->fPPortId = 
                                  pCepId->fNciaClient->ncia_server->ncia_swidb;
             dlcConnect->fClsUCepId = (ClsUCepIdT)(pCepId->fUCepId);
             dlcConnect->fDlcPCepId = (DlcPCepIdT) pCepId;
             dlcConnect->fLlcRetCode = opCode;

             snpa_init(&NciaReqOpenStn->fDestAddr, STATION_IEEE48, 
                       TARGET_UNICAST, &pCepId->fNciaClient->Vmac[0]);
             snpa_init(&NciaReqOpenStn->fSrcAddr, STATION_IEEE48, 
                       TARGET_UNICAST, &pCepId->fTargetAddr[0]);
             NciaReqOpenStn->fDestSap = pCepId->fClientSap;
             NciaReqOpenStn->fSrcSap = pCepId->fTargetSap;
         }
         break;
        
    case CLS_CONNECTED_IND:
         /* nothing to set */
         break;

    case CLS_DISC_IND:
    case CLS_DISC_CNF:
         {
             DlcDisconnectT  *dlcDisconnect = 
                                        &clsBlock->fParameters.fDlcDisconnect;

             dlcDisconnect->fPPortId = 
                                 pCepId->fNciaClient->ncia_server->ncia_swidb;
             dlcDisconnect->fClsUCepId = (ClsUCepIdT)(pCepId->fUCepId);
             dlcDisconnect->fDlcPCepId = (DlcPCepIdT)pCepId;
             dlcDisconnect->fLlcRetCode = opCode;
         }
         break;

    case CLS_FLOW_IND:
         if (pCepId->fNciaCepFlags & (NciaClientBusy | NciaTcpBusy)) {
             /* caused by flow control or tcp congestion */
             clsBlock->fParameters.fFlowLevel = DLC_CONGESTION_HIGH;
         } else {
             clsBlock->fParameters.fFlowLevel = DLC_CONGESTION_LOW;
         }
         break;
        
    case CLS_CLOSE_STN_CNF:
         {
             DlcCloseStnT  *dlcDisconnect = &clsBlock->fParameters.fDlcCloseStn;

             dlcDisconnect->fClsUCepId = (ClsUCepIdT)(pCepId->fUCepId);
             dlcDisconnect->fDlcPSapId = (DlcPSapIdT) pCepId->fPSap;
         }
         break;

    case CLS_UDATA_IND:
         {
             CLSASSERT_FATAL(clsBlock->fPakPtr);
             clsBlock->fLoadLength =  clsBlock->fPakPtr->datagramsize;
         }
         break;

    case CLS_TEST_IND:
         {
             DlcTestT  *dlcTest = &clsBlock->fParameters.fDlcTest;

             CLSASSERT_FATAL(clsBlock->fPakPtr);

             clsBlock->fLoadLength = clsBlock->fPakPtr->datagramsize;
             dlcTest->fClsUCepId = (ClsUCepIdT)(pCepId->fUCepId);
             dlcTest->fClsUSapId = (ClsUSapIdT) pCepId->fPSap->fUSapId;
             dlcTest->fDlcPCepId = (DlcPCepIdT) pCepId;
             if (pCepId->fNciaCepFlags & NciaTestRspPending) {
                 pCepId->fNciaCepFlags &= ~NciaTestRspPending;
                 dlcTest->fTestT = TEST_RESPONSE;
             } else
                 dlcTest->fTestT = TEST_COMMAND;
             dlcTest->fTESTLength = clsBlock->fPakPtr->datagramsize;
             dlcTest->fTESTPtr = clsBlock->fPakPtr->datagramstart;
         }
         break;

    default:
         /* shouldn't come here */
	 return(FALSE);
    }
    return(TRUE);
}




void NciaSendUControlStnInd (ClsDlcParamBlockT *clsBlock, paktype *pak, 
                             ClsUSapIdT uSap)
{
    DlcUdataStnT *dlcUdataStn;
    LlcUdataStnT *llcUdataStn;
    sap_hdr      *llc1;

    clsBlock->fResultError    = CLS_OK;
    clsBlock->fPakPtr         = pak;
    dlcUdataStn = &clsBlock->fParameters.fDlcUdataStn;
    llcUdataStn = &dlcUdataStn->fDlcUdata.fLlcUdataStn;
 
    llcUdataStn->fMacType       = STATION_IEEE48;
    llcUdataStn->fLMacLength    = IEEEBYTES;
    llcUdataStn->fLMacPtr       = pak->addr_start;
    llcUdataStn->fRMacLength    = IEEEBYTES;
    llcUdataStn->fRMacPtr       = pak->addr_start + IEEEBYTES;
    llcUdataStn->fRoutingLength = 0;
    llcUdataStn->fRoutingPtr    = NULL;

    llc1 = (sap_hdr *) pak->info_start;
    llcUdataStn->fRSap = llc1->ssap & ~SAP_RESPONSE;
    llcUdataStn->fLSap = llc1->dsap;

    dlcUdataStn->fClsUSapId   = (ClsUSapIdT) uSap;

    dlcUdataStn->fUDataLength = pak->datagramsize -
                     (pak->network_start - pak->datagramstart);
    dlcUdataStn->fUDataPtr = pak->network_start;

    reg_invoke_cls_rput((ClsUCepIdT) CLS_NO_CEP, CLS_UDATA_STN_IND, clsBlock);
}


/*
 * send XID_STN_IND or TEST_STN_IND or DATA_STN_IND (poll, not final)
 */
void NciaSetupUControlStnInd (uchar dsap, uchar ssap, 
                  uchar *dmac, uchar *smac, uchar control, paktype *pak)
{
    llc1_test_t *test;
    tring_hdr   *trframe;

    /*
     * TR_ACFC_SIZE + 2 * IEEEBYTES + SAP_HDRBYTES = 17
     * The token ring header is 17 bytes long.  To make it word
     * aligned, we have to move the data by 3 bytes.
     */
    if (pak->datagramsize > 0) {
        bcopy(pak->datagramstart, pak->datagramstart - 3, pak->datagramsize);
    }
    pak->datagramstart -= 3;

    /* no rif */
    pak->riflen = 0;
    pak->rif_start = NULL;

    pak->network_start = pak->datagramstart;
    pak->info_start = pak->network_start - SAP_HDRBYTES;
    pak->addr_start = pak->info_start - (2 * IEEEBYTES);
    pak->datagramstart = pak->addr_start - TR_ACFC_SIZE;
    pak->datagramsize += TR_ACFC_SIZE + 2 * IEEEBYTES + SAP_HDRBYTES;

    /*
     * Write the MAC header
     */
    trframe = (tring_hdr *)(pak->datagramstart);
    trframe->ac = AC_PRI0;
    trframe->fc = FC_TYPE_LLC;
    ieee_copy(dmac, trframe->daddr);
    ieee_copy(smac, trframe->saddr);
    trframe->saddr[0] |= (uchar)TR_RII;

    /*
     * Write the rest of the packet.
     */
    test = (llc1_test_t *)pak->info_start;
    test->control       = control | LLC1_P;      /* assume cmd/poll */
    test->dsap          = dsap;
    test->ssap          = ssap;
}



/* 
 * ncia_cls_notify, pCepId must be valid even if it's for SAP primitives.
 * this is because we need to get the necessary info.
 */
void ncia_cls_notify (NciaCepT *pCepId, ClsOpcodeT opcode,
                         ClsErrorCodeT error, paktype *pak)
{
    ClsDlcParamBlockT clsBlock;
    ClsUCepIdT        uCepId;
    
    memset(&clsBlock, 0, sizeof(ClsDlcParamBlockT));
    uCepId = (ClsUCepIdT) (pCepId->fUCepId);

    clsBlock.fResultError = error;
    clsBlock.fPakPtr      = pak;

    switch (opcode) {
    /* CEP */
    case CLS_DATA_IND:
    case CLS_REQ_OPNSTN_CNF:
    case CLS_ID_IND:
    case CLS_DISC_IND:
    case CLS_DISC_CNF:

    case CLS_CONNECT_IND:
    case CLS_CONNECT_CNF:
    case CLS_CONNECTED_IND:
    case CLS_FLOW_IND: /* DONE */

    case CLS_CLOSE_STN_CNF:
    case CLS_UDATA_IND:
    case CLS_TEST_IND:
         if (uCepId == NO_CLS)
             return;
         if (!NciaBuildParamBlock(pCepId, opcode, &clsBlock))
	     return;
         break;

    case CLS_DATA_CNF:
    case CLS_PONG_CNF:
    case CLS_MODIFY_STN_CNF:
         /* NOT SUPPORTED */
         /* fall through */

    /* NO CEP */
    case CLS_ENABLE_CNF:
    case CLS_DISABLE_CNF:
    case CLS_ACTIVATE_SAP_CNF:
    case CLS_DEACTIVATE_SAP_CNF:
    case CLS_DEACTIVATE_RING_CNF:
    case CLS_ACTIVATE_RING_CNF:
         /* These primitives shouldn't come here */
         return;

    case CLS_TEST_STN_IND:
    case CLS_ID_STN_IND:
         {
         boolean isResponse;
         uchar   control;

         if (!pak) {
             NciaServerDebugAll(" null pak");
             return;
         }

         if (opcode == CLS_ID_STN_IND)
             control = LLC1_XID;
         else
             control = LLC1_TEST;

         if (pCepId->fNciaCepFlags & NciaXidRspPending) {
             pCepId->fNciaCepFlags &= ~NciaXidRspPending;
             isResponse = XID_RESPONSE;
         } else {
             isResponse = XID_COMMAND;
         }

         clsBlock.fLoadLength = pak->datagramsize;

         if (control == LLC1_XID) {
             DlcIdStnT *dlcIdStn;

             dlcIdStn = (DlcIdStnT *) &clsBlock.fParameters.fDlcIdStn;
             dlcIdStn->fClsUSapId = (ClsUSapIdT) pCepId->fPSap->fUSapId;
             dlcIdStn->fDlcPCepId = (DlcPCepIdT) pCepId->fPCepId;
             dlcIdStn->fClsUCepId = (ClsUCepIdT) pCepId->fUCepId;
             dlcIdStn->fXidT = isResponse;
             dlcIdStn->fXIDLength = pak->datagramsize;
             dlcIdStn->fXIDPtr = pak->datagramstart;
         } else {
             DlcTestStnT *dlcTestStn;

             dlcTestStn = (DlcTestStnT *) &clsBlock.fParameters.fDlcTestStn;
             dlcTestStn->fClsUSapId = (ClsUSapIdT) pCepId->fPSap->fUSapId;
             dlcTestStn->fDlcPCepId = (DlcPCepIdT) pCepId->fPCepId;
             dlcTestStn->fClsUCepId = (ClsUCepIdT) pCepId->fUCepId;
         }

         /*
          * because DLSw+ local switch doesn't support non-zero DSAP, we set
          * it to zero.
          */
         if (opcode == CLS_TEST_STN_IND)
             NciaSetupUControlStnInd(0, pCepId->fClientSap, 
                 &pCepId->fTargetAddr[0], &pCepId->fNciaClient->Vmac[0], 
                 control, pak);
         else
             NciaSetupUControlStnInd(pCepId->fTargetSap, pCepId->fClientSap, 
                                 &pCepId->fTargetAddr[0],
                                 &pCepId->fNciaClient->Vmac[0], control, pak);

         DlcSendUControlStnInd(&clsBlock, pak, control, isResponse,
              (ClsUSapIdT) (pCepId->fPSap->fUSapId), (DlcPCepIdT) pCepId);
         /* send to cls already */
         return;
         }

    case CLS_UDATA_STN_IND:
         {
         DlcUdataStnT *dlcUdataStn;

         if (!pak) {
             NciaServerDebugAll(" null pak");
             return;
         }

         dlcUdataStn = &clsBlock.fParameters.fDlcUdataStn;
         dlcUdataStn->fClsUSapId = (ClsUSapIdT) pCepId->fPSap->fUSapId;
         dlcUdataStn->fDlcPCepId = (DlcPCepIdT) pCepId->fPCepId;
         dlcUdataStn->fClsUCepId = (ClsUCepIdT) pCepId->fUCepId;
         dlcUdataStn->fUDataLength = pak->datagramsize;
         dlcUdataStn->fUDataPtr = pak->datagramstart;
         dlcUdataStn->fDlcPSapId = (DlcPSapIdT) pCepId->fPSap;

         NciaSetupUControlStnInd(pCepId->fTargetSap,
                                 pCepId->fClientSap, &pCepId->fTargetAddr[0],
                                 &pCepId->fNciaClient->Vmac[0], LLC1_UI, pak);

         NciaSendUControlStnInd (&clsBlock, pak,
                     (ClsUSapIdT) (pCepId->fPSap->fUSapId));
         return;
         }

    case CLS_CONNECT_STN_IND:
         {
         DlcConnectStnT *dlcConnectStn;
         LlcConnectStnT *llcConnectStn;
         sap_hdr        *llc1;

         if (!pak) {
             NciaServerDebugAll(" null pak");
             return;
         }

#define LLC2_SAMBE 0x6F

         dlcConnectStn = &clsBlock.fParameters.fDlcConnectStn;
 
         dlcConnectStn->fClsUCepId = (ClsUCepIdT) pCepId->fUCepId;
         dlcConnectStn->fClsUSapId = (ClsUSapIdT) pCepId->fPSap->fUSapId;
         dlcConnectStn->fDlcPCepId = (DlcPCepIdT) pCepId->fPCepId;
         dlcConnectStn->fCONNECTLength = pak->datagramsize;
         dlcConnectStn->fCONNECTPtr = pak->datagramstart;

         NciaSetupUControlStnInd(pCepId->fTargetSap,
                                 pCepId->fClientSap, &pCepId->fTargetAddr[0],
                                 &pCepId->fNciaClient->Vmac[0], LLC2_SAMBE, pak);
 
         llcConnectStn = &dlcConnectStn->fDlcConnect.fLlcConnectStn;
         llcConnectStn->fMacType = STATION_IEEE48;
         llcConnectStn->fLMacLength = IEEEBYTES;
         llcConnectStn->fLMacPtr = pak->addr_start;
         llcConnectStn->fRMacLength = IEEEBYTES;
         llcConnectStn->fRMacPtr = pak->addr_start + IEEEBYTES;
         llcConnectStn->fRoutingLength = 0;
         llcConnectStn->fRoutingPtr = NULL;
 
         llc1 = (sap_hdr *) pak->info_start;
         llcConnectStn->fRSap = llc1->ssap & ~SAP_RESPONSE;
         llcConnectStn->fLSap = llc1->dsap;
 
         /*
          * Set network_start to point at start of xid data
          */
         pak->network_start = pak->info_start + SAP_HDRBYTES;
 
         dlcConnectStn->fCONNECTLength = pak->datagramsize -
                         (pak->network_start - pak->datagramstart);
 
         dlcConnectStn->fCONNECTPtr = pak->network_start;
 
         dlcConnectStn->fClsUSapId = (ClsUSapIdT) (pCepId->fPSap->fUSapId);
 
         clsBlock.fResultError    = CLS_OK;
         clsBlock.fPakPtr         = pak;
         reg_invoke_cls_rput((ClsUCepIdT) CLS_NO_CEP, CLS_CONNECT_STN_IND, 
                             &clsBlock);
         return;
         }

    default:
        NciaServerDebugAll("\nNCIA: ncia_cls_notify: invalid primitive %x", 
                           opcode);
        return;
    }

    DlcBuildParmBlock((DlcPCepIdT)pCepId, uCepId, opcode, &clsBlock,
                      error, pak);

    reg_invoke_cls_rput(uCepId, opcode, &clsBlock);
    return;
}
