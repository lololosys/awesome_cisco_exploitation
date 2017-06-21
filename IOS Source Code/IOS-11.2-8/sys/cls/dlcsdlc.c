/* $Id: dlcsdlc.c,v 3.7.26.9 1996/09/11 14:22:36 ggyurek Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlcsdlc.c,v $
 *------------------------------------------------------------------
 * dlcsdlc.c - Glue for converting from CLS to SDLC
 *
 * September 1994, Anna Lam
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Routines needed to map from CLS to existing SDLC code base.
 * (Glue)
 *------------------------------------------------------------------
 * $Log: dlcsdlc.c,v $
 * Revision 3.7.26.9  1996/09/11  14:22:36  ggyurek
 * CSCdi56633:  SDLC PU2.0 link stuck in CLS_ROSCNF_PEND state for DLUR
 * router
 * Branch: California_branch
 *
 * Revision 3.7.26.8  1996/08/28  12:39:31  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.26.7  1996/08/02  18:11:22  dbilling
 * CSCdi64791: APPN/DLUR does not send DISC after SSCP-PU(REQDISCONT) for
 * SDLC PU2
 * Change order of logic in sdlcCloseStnReq to change state after disc
 * check
 * Branch: California_branch
 *
 * Revision 3.7.26.6  1996/08/02  17:55:31  dbilling
 * CSCdi63154:  CLS(SDLC) trips assert when dlc sends data ind w/CLS_NO_CEP
 * Change check for null u_cep_id to check for CLS_NO_CEP
 * Branch: California_branch
 *
 * Revision 3.7.26.5  1996/07/23  13:20:44  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.7.26.4  1996/07/17  21:29:31  vkamat
 * CSCdi60439:  SDLC-primary interface dies if serial line is looped
 * Branch: California_branch
 *
 * Revision 3.7.26.3  1996/06/25  15:26:45  fbordona
 * CSCdi60090:  2500 SDLC connection stuck in USBUSY state after DLSW
 *              session closed. Improper use of TIMER_STOP and SLEEPING
 *              caused CLS to stop sending XID NULL.
 * Branch: California_branch
 *
 * Revision 3.7.26.2  1996/05/17  10:42:40  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.14.6  1996/05/13  19:36:32  vkamat
 * CSCdi54072:  Disallow Null XIDs to upstream SDLC when configured as
 * secondary
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.14.5  1996/05/11  21:55:08  athippes
 * CSCdi54990:  Router wont reboot if SDLC device is active
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.14.4  1996/05/02  20:23:40  pmorton
 * CSCdi42678:  Router crashed after removing SDLC address
 * Branch: IbuMod_Calif_branch
 * Mark sdlc data block for deletion (zombie).  Don't remove
 * till CLS issues CLOSE Station request.
 *
 * Revision 3.7.14.3  1996/04/11  14:36:15  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.14.2  1996/04/03  13:54:47  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.14.1  1996/04/01  21:03:06  fbordona
 * CSCdi53247:  DLSw: SDLC interface stuck in XIDSTOP state.
 *              sdlc state -> XIDSTOP only if prev state was XIDSENT.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.26.1  1996/03/18  19:10:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.1  1996/03/13  01:14:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7  1996/01/19  21:36:56  fbordona
 * CSCdi47202:  SHARED buffer error message when router is
 *              sdlc secondary 2.1
 *
 * Revision 3.6  1996/01/18  20:37:58  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/15  10:32:30  richl
 * CSCdi46893:  cleanup on CSCdi46267 (minor dlcsdlc changes)
 *              Missed some states for DM.
 *              Correct some comments and case indentation.
 *              Add CLSASSERT for error case.
 *
 * Revision 3.4  1995/12/31  18:16:26  fbordona
 * CSCdi46267:  DLSw sdlc secondary multidrop PUs do not connect.
 *              Fix protocol polling violation with primary multidrop.
 *
 * Revision 3.3  1995/11/30  19:07:20  ppearce
 * CSCdi44393:  DSPU/SDLC memory leak when SDLC device disconnected
 *   (1) SDLC sends DISC_IND when it shouldn't
 *   (2) DSPU memory heap management isn't working properly
 *
 * Revision 3.2  1995/11/17  08:55:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  20:53:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/19  08:08:47  bchan
 * CSCdi27730:  SNA Fragmentation break if no buffers available
 *
 * Revision 2.6  1995/08/07  05:30:03  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.5  1995/07/24  13:02:55  fbordona
 * CSCdi34580:  DLS show circuit displays HALT PENDING NOACK after
 *              successful disc
 *
 * Revision 2.4  1995/07/06  01:31:42  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.3  1995/07/02 05:15:38  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.2  1995/06/28  18:47:04  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *
 * Revision 2.1  1995/06/07  20:20:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../ibm/msg_sdlc.c"		/* Not a type, see logger.h */
#include "cls_private.h"
#include "packet.h"
#include "config.h"
#include "../if/network.h"
#include "address.h"
#include "../ibm/sdlc.h"
#include "../ibm/sna_util.h"
#include "../dspu/snautil.h"
#include "../ibm/libibm_externs.h"
#include "../os/free.h"

/* local function declarations */

static clsSdlcMapFnT sdlcEnableReq(sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcDisableReq(sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcActivateSapReq(sdlc_data_block *sdb,
					ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcActivateRingReq(sdlc_data_block *sdb, 
					 ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcDeactivateRingReq(sdlc_data_block *sdb, 
					 ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcIdReq(sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcTestStnReq(sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcTestReq(sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcReqOpnStnReq(sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcConnectReq(sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcConnectRsp(sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcSignalStnReq(sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcDataReq(sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcFlowReq(sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock);
static clsSdlcMapFnT sdlcDisconnectReq(sdlc_data_block *sdb,  ClsDlcParamBlockT *clsBlock);

static void ClsSdlcDiscInd(sdlc_data_block *sdb);
static void SdlcReqOpenStnConf(sdlc_data_block *, ClsUCepIdT, ClsErrorCodeT);

static void cls_sdlc_debug(idbtype *idb, sdlc_data_block *sdb, char *sdlc_debug_text);
paktype* GetProxyXid(sdlc_data_block *, boolean);
static uchar* SdlcGetPartner(sdlc_data_block *sdb);

char *pr_cls_sdlc_state (sdlc_data_block *sdb) 
{
    switch(sdb->cls_state) {
      case CLS_STN_CLOSED:         return("CLS_STN_CLOSED");
      case CLS_STN_OPENED:         return("CLS_STN_OPENED");
      case CLS_ROSCNF_PEND:        return("CLS_ROSCNF_PEND");
      case CLS_CONNECT_RSP_PEND:   return("CLS_CONNECT_RSP_PEND");
      case CLS_DISCCNF_PEND:       return("CLS_DISCCNF_PEND");
      case CLS_CONNECT_REQ_PEND:   return("CLS_CONNECT_REQ_PEND");
      case CLS_FULL_XID_PEND:      return("CLS_FULL_XID_PEND");
      case CLS_CONNECTED_IND_PEND: return("CLS_CONNECTED_IND_PEND");
      case CLS_DISC_IND_SENT:      return("CLS_DISC_IND_SENT");
      case CLS_IN_SESSION:         return("CLS_IN_SESSION");
      case CLS_CLOSING:		   return("CLS_CLOSING");
      default:                     return("UNKNOWN_CLSSDLC_STATE!");
    }
}

/************************************************************************
 *			     CLSI Primitive: (none)
 ************************************************************************
 *
 * DLC Message Generated/Received
 * ------------------------------
 * sdlc_wput(*cep, DLC_INIT_SYSTEM, &parm_block)
 *
 ************************************************************************
 */

static clsSdlcMapFnT sdlcInitSystem (sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    return;
}


/************************************************************************
 *			   CLSI Primitive: Enable.req
 ************************************************************************
 *
 * Initialize port with hardware specific paramters.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(*cep, SDLC_ENABLE_REQ, &param_block)
 *
 *
 ************************************************************************
 */

static clsSdlcMapFnT sdlcEnableReq (sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    idbtype		*idb = NULL;
    DlcEnableT		*dlcEnable;

    /* Check if the port exists and is capable of running SDLC */

    dlcEnable = &clsBlock->fParameters.fDlcEnable;
    if (dlcEnable->fDLCType == CLS_SDLC) {

	idb = DlcToIdb(&dlcEnable->fPortId);

	if (idb) {
            dlcEnable->fPPortId = (DlcPPortIdT) idb;
	    dlcEnable->fMediaEnable.fSDLCEnable.fVersion = CLSI_VERSION;

	    cls_sdlc_debug(idb, NULL, "Enable Request, sending confirm");
	    dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_OK);
	    return;
        }
    }
    dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_UNKNOWN_PORT);

}


/************************************************************************
 *			   CLSI Primitive: Disable.req
 ************************************************************************
 *
 * Disable port 
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(*cep, DLC_DISABLE_REQ, &param_block)
 *
 *
 ************************************************************************
 */

static clsSdlcMapFnT sdlcDisableReq (sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{

    idbtype		*idb = NULL;
    DlcDisableT       	*dlcDisable;


    dlcDisable = &clsBlock->fParameters.fDlcDisable;
    if (dlcDisable->fDLCType == CLS_SDLC) {

	idb = DlcToIdb(&dlcDisable->fPortId);
	if (idb) {
	    cls_sdlc_debug(idb, NULL, "Disable Request, sending confirm");
	    dlc_rput(clsBlock, CLS_DISABLE_CNF, CLS_OK);
	    return;
        }
    }
    dlc_rput(clsBlock, CLS_DISABLE_CNF, CLS_UNKNOWN_PORT);

} /* End sdlcDisableReq() */


/************************************************************************
 * 		   CLSI Primitive: Activate_sap.req
 *************************************************************************
 * Controls which SDLC station addresses we will respond.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(*cep, SDLC_ACTIVATE_SAP_REQ, &param_block)
 *
 *
 **********************************************************************
 */
static clsSdlcMapFnT sdlcActivateSapReq (
	sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    idbtype		*idb;
    DlcActSapT		*dlcActSap;
    SdlcActSapT		*sdlcActSap;

    dlcActSap = &clsBlock->fParameters.fDlcActSap;
    sdlcActSap = &dlcActSap->fDlcSap.fSdlcActSap;

    idb = (idbtype *)(dlcActSap->fPPortId);
    if (idb == NULL) {
	dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    sdb = idb->hwptr->sdlc_data[sdlcActSap->fLSapValue];
    if (!sdb) {
	if (cls_debug_sdlc)
	    buginf("\nSDLC_WRAPPER:  No sdb for addr:0x%02x", 
		   sdlcActSap->fLSapValue);
	dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_UNKNOWN_SAP);
	return;
    }
    
    if (!sdb->u_sap_id)
	sdb->u_sap_id = (word) dlcActSap->fUSapId;
    sdb->u_cep_id = (void*) CLS_NO_CEP;
    sdb->addr_type = sdlcActSap->fAddrType;

    cls_sdlc_debug(NULL, sdb, "Activate SAP Request, sending confirm");
    dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_OK);

}

/************************************************************************
 * 		   CLSI Primitive: Activate_ring.req
 *************************************************************************
 * Controls which IDB we will respond.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(*cep, SDLC_ACTIVATE_RING_REQ, &param_block)
 *
 *
 **********************************************************************
 */
static clsSdlcMapFnT sdlcActivateRingReq (
	sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    idbtype		*idb;
    hwidbtype		*hwidb;
    DlcActRngT		*dlcActRng;
    SdlcActRngT		*sdlcActRng;
    word		sap_id;
    int			i;

    dlcActRng = &clsBlock->fParameters.fDlcActRng;
    sdlcActRng = &dlcActRng->fDlcRng.fSdlcActRng;

    idb = (idbtype *)(dlcActRng->fPPortId);
    if (idb == NULL) {
	dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    hwidb = idb->hwptr;
    sap_id = (word) dlcActRng->fClsUSapId;
    
    dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_OK);

    cls_sdlc_debug(idb, NULL, "Activate Ring Request");

    for (i=SDLC_MIN_ADDRESS; i<=SDLC_MAX_ADDRESS; i++) {
	sdb = hwidb->sdlc_data[i];
	if (IsDlswSdb(sdb) || IsFrasBanSdb(sdb)) {
	    sdb->u_sap_id = sap_id;
	    sdb->addr_type = STATION_IEEE48;
	}
    }
    hwidb->sdlc_dlsw_u_sap_id = sap_id;
}

/************************************************************************
 * 		   CLSI Primitive: Deactivate_ring.req
 *************************************************************************
 * We won't respond for this IDB anymore.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(*cep, SDLC_DEACTIVATE_RING_REQ, &param_block)
 *
 *
 **********************************************************************
 */
static clsSdlcMapFnT sdlcDeactivateRingReq (
	sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    idbtype		*idb;
    hwidbtype		*hwidb;
    DlcActRngT		*dlcActRng;
    SdlcActRngT		*sdlcActRng;
    int 		i;

    dlcActRng = &clsBlock->fParameters.fDlcActRng;
    sdlcActRng = &dlcActRng->fDlcRng.fSdlcActRng;

    idb = (idbtype *)(dlcActRng->fPPortId);
    if (idb == NULL) {
	dlc_rput(clsBlock, CLS_DEACTIVATE_RING_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    hwidb = idb->hwptr;

    cls_sdlc_debug(idb, NULL, "Deactivate Ring Request");

    for (i=SDLC_MIN_ADDRESS; i<=SDLC_MAX_ADDRESS; i++) {
	sdb = hwidb->sdlc_data[i];
	if (IsDlswSdb(sdb) || IsFrasBanSdb(sdb)) {

	    /* 
	     * We can't hang out for the confirm unless we create a pending
	     * state for the deactivate ring.  Another solution is to reject
	     * the deactivate ring if there are any CEP's.
	     */
	    if (sdb->u_cep_id != (void*) CLS_NO_CEP) {
		sdlcClsNotify(sdb, CLS_DISC_IND, CLS_OK);
		sdb->u_cep_id = (void*) CLS_NO_CEP;
		sdb->cls_state = CLS_STN_CLOSED;
	    }
	    sdb->u_sap_id = 0;

	    if (IS_SDLC_CONNECT(sdb)) 
		sdlc_setstate(sdb->sdlc_idb, SDLC_DISCSENT, sdb);
	    else
		sdlc_reset(hwidb, "Deactivate CLS SAP", hwidb->sdlc_data[i]);
	}
    }
    hwidb->sdlc_default = NO_SDLC;
    hwidb->sdlc_dlsw_u_sap_id = 0;
    dlc_rput(clsBlock, CLS_DEACTIVATE_RING_CNF, CLS_OK);
}    

/************************************************************************
 * 		   CLSI Primitive: Modify_sap.req
 *************************************************************************
 * Modifies operational parameters on the SAP
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(*cep, SDLC_MODIFY_SAP_REQ, &param_block)
 *
 *
 ***********************************************************************
 */

static clsSdlcMapFnT sdlcModifySapReq (
    	sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{

    cls_sdlc_debug(NULL, sdb, "SDLC Modify Sap Request, NO ACTION"); 
    return;
}

/************************************************************************
 * 		   CLSI Primitive: Deactivate_sap.req
 *************************************************************************
 * We won't respond to this SAP any more.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(*cep, SDLC_DEACTIVATE_SAP_REQ, &param_block)
 *
 ***********************************************************************
 */

static clsSdlcMapFnT sdlcDeactivateSapReq (
    sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    idbtype		*idb;
    hwidbtype		*hwidb;
    DlcActSapT		*dlcActSap;
    SdlcActSapT		*sdlcActSap;

    dlcActSap = &clsBlock->fParameters.fDlcActSap;
    sdlcActSap = &dlcActSap->fDlcSap.fSdlcActSap;

    idb = (idbtype *)(dlcActSap->fPPortId);
    hwidb = hwidb_or_null(idb);
    if (idb == NULL || hwidb == NULL) {
	dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_UNKNOWN_PORT);
	return;
    }
    sdb = hwidb->sdlc_data[sdlcActSap->fLSapValue];

    /*
     * Undefined sdb is OK
     *
     * It could have come from removing an SDLC station
     * and then later decativating the SAP
     */
    if (!sdb) {
	dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_OK);
	return;
    }

    if (!sdb->u_sap_id) {
	dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_UNKNOWN_SAP);
	return;
    }

    sdb->u_sap_id = 0;
    sdb->u_cep_id = (void *)CLS_NO_CEP;

    cls_sdlc_debug(idb, NULL, "Deactivate Sap Req"); 

    sdlc_reset(idb->hwptr, "DLU deactivated SAP", sdb);

    dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_OK);

    return;
}


/************************************************************************
 * 	      CLSI Primitive: Req_opnstn.req (outgoing)
 *************************************************************************
 * If PU2 primary node, null xid will be sent to DLU along with
 * confirmation to establish session on the LLC side.
 * Otherwise, just send confirm.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(*cep, SDLC_REQ_OPNSTN_REQ, &parm_block)
 *
 **********************************************************************
 */

static clsSdlcMapFnT sdlcReqOpnStnReq (
    sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    DlcReqOpenStnT	*dlcReqOpenStn;
    SdlcReqOpenStnT	*sdlcReqOpenStn;
    ClsUCepIdT		uCepId;
    idbtype		*idb;
    uchar               *sdlc_partner;
 
    dlcReqOpenStn = &clsBlock->fParameters.fDlcReqOpenStn;
    sdlcReqOpenStn = &dlcReqOpenStn->fDlcStn.fSdlcReqOpenStn;
    idb = (idbtype *)dlcReqOpenStn->fPPortId;

    uCepId = dlcReqOpenStn->fClsUCepId;

    if (snpa_get_type(&(sdlcReqOpenStn->fRMacAddr)) == STATION_IEEE48) {
	if (cls_debug_sdlc) {
	    buginf("\nSDLC Req Open Stn Dmac %e",
		   sdlcReqOpenStn->fRMacAddr.addr);
	}
	sdb = idb->hwptr->sdlc_data[sdlcReqOpenStn->
				    fRMacAddr.addr[IEEEBYTES - 1]];

	if (!sdb) {
	    SdlcReqOpenStnConf(sdb, uCepId, CLS_BAD_ADDRESS);
	    return;
	}
	cls_sdlc_debug(NULL, sdb, "Request Open Station");
	if (cls_debug_sdlc)
	    buginf("\n\t\tlmac %e rmac %e lsap %02x rsap %02x addr",
		   sdlcReqOpenStn->fLMacAddr.addr,
		   sdlcReqOpenStn->fRMacAddr.addr,
		   sdlcReqOpenStn->fLSapValue,
		   sdlcReqOpenStn->fRSapValue);

        sdlc_partner = SdlcGetPartner(sdb);

        if (bcmp(sdlcReqOpenStn->fLMacAddr.addr, sdlc_partner, IEEEBYTES) ||
	    (sdlcReqOpenStn->fLSapValue != sdb->sdlc_ssap) ||
	    (sdlcReqOpenStn->fRSapValue != sdb->sdlc_dsap)) {
	    SdlcReqOpenStnConf(sdb, uCepId, CLS_BAD_ADDRESS);
	    if (cls_debug_sdlc)
		buginf(" No partner configured");
	    return;
	  }
    } else if (snpa_get_type(&(sdlcReqOpenStn->fRMacAddr)) == STATION_SDLC) {
	sdb = idb->hwptr->sdlc_data[sdlcReqOpenStn->fRMacAddr.addr[0]];
    } else {
	SdlcReqOpenStnConf(sdb, uCepId, CLS_BAD_ADDRESS);
	return;
    }

    if (!sdb || is_sdlc_marked_deleted(sdb)) {
	SdlcReqOpenStnConf(sdb, uCepId, CLS_BAD_ADDRESS);
	return;
    }

    dlcReqOpenStn->fDlcPCepId = (DlcPCepIdT)sdb;

    /*
     * If this happens, then the DLU is not cleaning up the previous u_cep_id.
     * If we didn't do this, we'd have a memory leak of CLS ceps, so let's 
     * make noise and get it cleaned up.
     */
    if ((sdb->u_cep_id != (void*) CLS_NO_CEP) && 
	(sdb->cls_state == CLS_ROSCNF_PEND)) {

	SdlcReqOpenStnConf(sdb, sdb->u_cep_id, CLS_REQUEST_OVERRIDE);

	errmsg(&msgsym(DLU_ERROR,SDLC), sdb->sdlc_idb->hw_namestring, 
	       sdb->sdlc_address, pr_cls_sdlc_state(sdb), 
	       sdlc_prstate((sdb), 0));		   
    }
	
    if (idb->hwptr->state != IDBS_UP) {
	SdlcReqOpenStnConf(sdb, uCepId, CLS_NO_REM_STN);
    } else if (IsSecPu2(sdb)) {
	/*
	 * If we are Secondary, DLU gets ROSCNF after we get SNRMed again.
	 * If we are connected, reject the request but also rip the current
	 * connection down, as it is a DLU error.
	 * If sdlc > SNRMSEEN, reset it.
	 */
	if (sdb->cls_state > CLS_ROSCNF_PEND) {
	    SdlcReqOpenStnConf(sdb, uCepId, CLS_CONNECTION_BUSY);
	    sdlcClsNotify(sdb, CLS_DISC_IND, CLS_OK);
	    sdb->cls_state = CLS_DISC_IND_SENT;
	    sdlc_setstate(sdb->sdlc_idb, SDLC_DISCSENT, sdb);
	} 
	else {
	    sdb->cls_state = CLS_ROSCNF_PEND;
	    sdb->u_cep_id = uCepId;
	}
    } else if (IsPrimPu2(sdb)) {
	sdb->u_cep_id = uCepId;
	if (IS_SDLC_CONNECT(sdb)) {
	    SdlcReqOpenStnConf(sdb, uCepId, CLS_OK);
	    sdb->cls_state = CLS_STN_OPENED;
	}
	else 
	    sdb->cls_state = CLS_ROSCNF_PEND;
    }
    else {
	SdlcReqOpenStnConf(sdb, uCepId, CLS_OK);
	sdb->cls_state = CLS_STN_OPENED;
	sdb->u_cep_id = uCepId;
    }
    cls_sdlc_debug(NULL, sdb, "Completed Request Open Station Request");
}

/************************************************************************
 * 	      CLSI Primitive: closestn.req (outgoing)
 *************************************************************************
 **********************************************************************
 */

static clsSdlcMapFnT sdlcCloseStnReq (
    sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    leveltype 		level;

    if (sdb == NULL) 
	return;

    cls_sdlc_debug(NULL, sdb, "Close Station Request");

    level = raise_interrupt_level(NETS_DISABLE);

    sdb->cls_state = CLS_CLOSING;
    if (IsPrimPu2(sdb) && (sdb->cls_state != CLS_IN_SESSION))
	/*
	 * Don't rip down PU 2.0 SDLC session, which is in USBUSY,
	 * if we haven't received ACTPU.  
	 */
	;
    else if (IS_SDLC_CONNECT(sdb)) {
	sdlc_setstate(sdb->sdlc_idb, SDLC_DISCSENT, sdb);
	sdlc_set_pause_timer(sdb->sdlc_idb, sdb);
    } 
    else 
	sdlc_setstate(sdb->sdlc_idb, SDLC_DISCONNECT, sdb);

    if (sdb->u_cep_id != (void*) CLS_NO_CEP) {
	sdlcClsNotify(sdb, CLS_CLOSE_STN_CNF, CLS_OK);
	sdb->u_cep_id = (void*) CLS_NO_CEP;
    } 
    sdb->cls_state = CLS_STN_CLOSED;

    reset_interrupt_level(level);

    if (IsPrimPu2(sdb))
        sdb->sdlc_xid_count = 0;
}


/************************************************************************
 * 	      CLSI Primitive: modifystn.req (outgoing)
 *************************************************************************
 *
 **********************************************************************
 */

static clsSdlcMapFnT sdlcModifyStnReq (
    sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{

    cls_sdlc_debug(NULL, sdb, "SDLC Modify Request");

    return;
}


/************************************************************************
 * 			CLSI Primitive: ID.req
 *************************************************************************
 * If PU2 node, XIDs from DLU will never be sent to the line.
 * Proxy XID response will be sent up to DLU.
 *
 * If PU2.1 node, XIDs will be sent directly to the line.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(*cep, SDLC_ID_REQ, &param_block)
 *
 *
 **********************************************************************
 */

static clsSdlcMapFnT sdlcIdReq (sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    DlcIdStnT	*dlcIdStn;
    paktype	*pak;
    paktype	*xid_pak;
    boolean	sdlc_bcast_addr;
    XID3type	*xid;
    hwidbtype	*hwidb;

    sdlc_bcast_addr = FALSE;
    dlcIdStn = &clsBlock->fParameters.fDlcIdStn;

    hwidb = sdb->sdlc_idb;
    pak	= clsBlock->fPakPtr;
    xid = (XID3type *)pak->datagramstart;
    pak->datagramstart -= SDLCHEADERBYTES;      /* adjust for SDLC header */
    pak->datagramsize  += SDLCHEADERBYTES;

    cls_sdlc_debug(NULL, sdb, "ID Request");
    if (cls_debug_sdlc) {
	memory_dump(pak->datagramstart, 
		    pak->datagramstart + pak->datagramsize -1,
		    FALSE, MEMORY_DUMP_DATA_TYPE_EBCDIC, buginf);
    }

    
    /* 
     * We're not supposed to care, but let's peek at the XID format to display
     * it in the debug.  Rather than use the structure and deal with all the
     * alignment issues, just, shift
     */
    if (cls_debug_sdlc) {
	if (pak->datagramsize > SDLCHEADERBYTES) {
	    buginf("\n\t\tXID format %d, type %d %d bytes",
		   xid_get_format((uchar *)xid),
		   xid_get_node_type((uchar *)xid),
		   xid_get_length((uchar *)xid));
	}
	else
	    buginf("\n\t\tXID (NULL)"); 
    }

    if (sdb->cls_state < CLS_STN_OPENED) {
	if (cls_debug_sdlc)
	    buginf("\n dumping XID, station not opened"); 
	datagram_done(pak);
	return;
    }

    if (IsPrimPu2(sdb)) {
	datagram_done(pak);
	if (IS_SDLC_CONNECT(sdb)) {
            sdb->cls_state = CLS_CONNECT_REQ_PEND;
	    xid_pak = GetProxyXid(sdb, TRUE);
	    if (!xid_pak) {
		if (cls_debug_sdlc)
		    buginf("\nSDLC_WRAPPER: No buffer for Proxy Xid");
		return;
	    }
	    sdlcSendXidToCLS(sdb, xid_pak);
	}
	return;
    }

    if (IsSecPu2(sdb)) {
        /*
         * Ok to drop xid if we haven't been snrm'd yet.
         */
        if (sdb->cls_state == CLS_STN_OPENED) {
            sdb->cls_state = CLS_FULL_XID_PEND;
            xid_pak = GetProxyXid(sdb, FALSE);
            if (!xid_pak) {
                if (cls_debug_sdlc)
                    buginf("\nSDLC_WRAPPER: No buffer for Proxy Xid");
            } else {
                sdlcSendXidToCLS(sdb, xid_pak);
            }
        } else if (sdb->cls_state == CLS_FULL_XID_PEND) {
            /*
             * If this is a NULL XID, then just respond with
             * a NULL XID back to the DLU. If it's an xid3 or
             * format 0 type 2, then finally send the connect
             * indication to the DLU (xid exchange is complete).
             */
            if (pak->datagramsize < 3) {
                xid_pak = GetProxyXid(sdb, FALSE);
                if (!xid_pak) {
                    if (cls_debug_sdlc)
                        buginf("\nSDLC_WRAPPER: No buffer for Proxy Xid");
                } else {
                    sdlcSendXidToCLS(sdb, xid_pak);
                }
            } else {
                /*
                 * If xid validation is added, it goes here.
                 */
                sdb->cls_state = CLS_CONNECT_RSP_PEND;
                sdlcClsNotify(sdb, CLS_CONNECT_IND, CLS_OK);
            }
        }
	else 
	    cls_sdlc_debug(NULL, sdb, "wrong station state to receive XID");
		   
        datagram_done(pak);
        return;
    }
    /*
     * If we've made it this far, then the target sdlc station is
     * a PU 2.1 (or capable of xid exchange). Our role is either
     * prim-xid-poll, none, or secondary passthrough (or negotiable).
     * For all these case, we will eventually send an XID on the wire
     * so prepare the packet for trensmission.
     */

 
    pak->vidb_input = pak->if_input;
    pak->vidb_output = pak->if_output;

    pak->vidb_flags = pak->flags;
    pak->vidb_datagramsize = pak->datagramsize;
    pak->vidb_datagramstart = pak->datagramstart;
    pak->vidb_linktype = pak->linktype;
    pak->vidb_enctype = pak->enctype;

    /*
     * Set address to real station if the role is known to be
     * secondary. Never send broadcast address to a multidropped 2.1
     * station.
     */
    if (is_xid3(pak)) {
        if ((xid->xid3_lsflags & XID3_ROLE) && (!sdlc_is_multidrop(sdb)))
            sdlc_bcast_addr = TRUE;
    } else if (!sdlc_is_multidrop(sdb))
        sdlc_bcast_addr = TRUE;

    if (sdlc_bcast_addr)
        pak->datagramstart[0] = SDLC_BCAST_ADDR;
    else
        pak->datagramstart[0] = sdb->sdlc_address;

    /*
     * Ok, now for the 3 different roles we can have, we need to
     * take different action.
     */

    /* CSCdi70552 For PU2.1, send xids using SDLC main timer */

    if (IsPrimXidPoll(sdb)) {
            /*
             * For primary 2.1, allow the sdlc main timer send all XIDs. 
             * This avoids polling protocol violatios for multidrop and
             * keeps sending XIDs consistent since the sdlc main timer must 
             * retry XIDs anyway.
             *
             * Leave some footprints so sdlc_main_timer() knows what to do
             * and save pak in outbuf so sdlc_main_timer() can find it.
	     *
	     *
             * CSCdi89219: Changed code to restart timer if it's not
             * running, OR if we are slow-polling, i.e.
             * time left > T1. When XID arrives, we wake up early and send it.
             */
            if (!TIMER_RUNNING(sdb->sdlc_idb->sdlc_top_timer) || 
		(TIME_LEFT_SLEEPING(sdb->sdlc_idb->sdlc_top_timer) >
		 sdb->sdlc_idb->sdlc_t1)) {
		if (cls_debug_sdlc)
		  buginf("\nXID arrived from DLU, restarting sdlc_main_timer");
                GET_TIMESTAMP(sdb->sdlc_idb->sdlc_top_timer);
	    }
            if (sdb->sdlc_idb->sdlc_current_polled == 0)
                sdb->sdlc_idb->sdlc_current_polled = sdb->sdlc_address;
            sdb->sdlc_retcnt = 0;
            sdb->xid_resp = FALSE;
            if (sdb->outbuf)
                datagram_done(sdb->outbuf);
            sdb->outbuf = pak;
	    if (sdb->cls_state != CLS_IN_SESSION)
		sdlc_setstate(sdb->sdlc_idb, SDLC_XIDSENT, sdb);

	    return;
    }

 
    /*
     * Role secondary negotiable.
     */
    if (sdb->sdlc_idb->configured_role == SDLC_SECONDARY) {
       /*
        * Null XID's don't get passed through to upstream SDLC, if the
        * router state machine indicates that it's XID COMMAND.
        */
        if ((pak->datagramsize < 3) && (sdb->xid_resp == FALSE)) {
             datagram_done(pak);
             return;
        }

        if (sdb->cls_state < CLS_STN_OPENED) {
            /*
             * If cls state is less than CLS_STN_OPENED, then we
             * are on the way up or down. Don't process IdReq.
             * OK to drop IdReq since it's connectionless.
             */
            datagram_done(pak);
        } else if (sdb->sdlc_address == sdb->sdlc_idb->sdlc_current_polled) {
            sdlc_sendframe(SDLC_XID, TRUE, sdb->sdlc_idb, pak, sdb);
	    if (sdb->cls_state != CLS_IN_SESSION)
		sdlc_setstate(sdb->sdlc_idb, SDLC_XIDSENT, sdb);
            sdlc_settimer(sdb->sdlc_idb, sdb->sdlc_idb->sdlc_t1, sdb);
            sdb->xid_resp = TRUE;
        } else {
            /*
             * Save xid to be sent when we get polled.
             */
            if (sdb->outbuf)
                datagram_done(sdb->outbuf);
            sdb->outbuf = pak;
        }
        return;
    }

    /*
     * Role none.
     */
    sdlc_sendframe(SDLC_XID, TRUE, sdb->sdlc_idb, pak, sdb);
    if (sdb->cls_state != CLS_IN_SESSION)
	sdlc_setstate(sdb->sdlc_idb, SDLC_XIDSENT, sdb);
    sdlc_settimer(sdb->sdlc_idb, sdb->sdlc_idb->sdlc_t1, sdb);
    sdb->xid_resp = TRUE;
    return;
}


static clsSdlcMapFnT  sdlcTestStnReq (sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    DlcTestStnT	*dlcTestStn;
    uchar	sdlc_addr;
    idbtype	*swidb;
    hwidbtype	*hwidb;
    paktype 	*pak;

    dlcTestStn = &clsBlock->fParameters.fDlcTestStn;
    pak = clsBlock->fPakPtr;
    swidb = DlcToIdb(&clsBlock->fPortId);
    hwidb = hwidb_or_null(swidb);

    if (!pak) {
	if (cls_debug_sdlc)
	    buginf("\nSDLC_WRAPPER: No test pak in test request");
	return;
    }

    if (!hwidb) {
	datagram_done(pak);
	return;
    }

    if (cls_debug_sdlc) {
	buginf("\nTest Stn Req: da %e", dlcTestStn->fRMac);
    }
    sdlc_addr = dlcTestStn->fRMac[IEEEBYTES-1];

    sdb = hwidb->sdlc_data[sdlc_addr];

    if (!sdb) {
	datagram_done(pak);
	return;
    }

    if (bcmp(dlcTestStn->fRMac, sdb->sdlc_idb->sdlc_vr_macaddr, 
	     IEEEBYTES -1 )) {
	datagram_done(pak);
	return;
    }

    if (hwidb->state !=IDBS_UP) {
	datagram_done(pak);
	return;
    }

    /* 
     * sanity checks completed, build and send reply 
     */
    cls_sdlc_debug(NULL, sdb, "Test Request, proxy the response to DLU");
    DlcBounceTestStnInd((DlcPCepIdT)sdb, clsBlock);
    return;
}

static clsSdlcMapFnT  sdlcTestReq (sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    cls_sdlc_debug(NULL, sdb, "Test Request");
    DlcHandleTestReq(sdb->u_cep_id, clsBlock);
}

/************************************************************************
 * 		     CLSI Primitive: Connect.req
 *************************************************************************
 * Send SNRM if XID passthru.
 * If PU2 primary node, send confirm back to DLU because SNRM
 * was sent when the SAP was activated.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(*cep, SDLC_CONNECT_REQ, &param_block)
 *
 ************************************************************************
 */

static clsSdlcMapFnT  sdlcConnectReq (sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{

    /*
     * 2.1 SNRM's now.  2.0 is already active, so just confirm
     */
    cls_sdlc_debug(NULL, sdb, "Connect Request");
    sdb->sdlc_idb->sdlc_role = SDLC_PRIMARY;
    if (IsPrimPu2(sdb) && IS_SDLC_CONNECT(sdb)) {
	sdb->cls_state = CLS_IN_SESSION;
	sdlcClsNotify(sdb, CLS_CONNECT_CNF, CLS_OK);
    }
    else if (!IS_SDLC_CONNECT(sdb)) {
        sdb->sdlc_retcnt = 0;
        if (sdb->outbuf) {
            datagram_done(sdb->outbuf);
            sdb->outbuf = NULL;
        }
	sdlc_setstate(sdb->sdlc_idb, SDLC_SNRMSENT, sdb);
	sdlc_set_pause_timer(sdb->sdlc_idb, sdb);
    }
}


/************************************************************************
 *		     CLSI Primitive: Connect.rsp
 *************************************************************************
 * Send UA.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(cookie, DLC_CONNECT_RSP, &param_block)
 * 	cookie should be SDLC's p_cep_id
 *
 ***********************************************************************
 */

static clsSdlcMapFnT sdlcConnectRsp (sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{    
    sdb->sdlc_idb->sdlc_role = SDLC_SECONDARY;
    sdb->cls_state = CLS_CONNECTED_IND_PEND;
    cls_sdlc_debug(NULL, sdb, "received Connect Response from DLU");
}



/************************************************************************
 * 		    CLSI Primitive: Signal_stn.req
 ************************************************************************
 *
 * Station is ready to receive data.  Should send RR.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(cookie, DLC_SIGNAL_STN_REQ, &param_block)
 * 	cookie should be SDLC's p_cep_id
 *
 **********************************************************************
 */

static clsSdlcMapFnT sdlcSignalStnReq (
    sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    cls_sdlc_debug(NULL, sdb, "Signal Station Request, clearing busy");
    sdlc_set_clear_busy(sdb->sdlc_idb, sdb->sdlc_address, FALSE);
}


/************************************************************************
 *		       CLSI Primitive: Data.req
 ************************************************************************
 * Send data to the line.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(cookie, DLC_DATA_REQ, &param_block)
 * 	cookie should be DLC's p_cep_id
 *
 * TH segmentinng is only considered for PU 2.0 devices. Segmenting
 * for 2.1 devices (exchanging XID3's) is performed at the DLU level.
 *
 **********************************************************************
 */

static clsSdlcMapFnT sdlcDataReq (sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    paktype *pak;

    pak = clsBlock->fPakPtr;
    pak->if_output = sdb->sdlc_idb->firstsw;
    if ((pak->datagramsize <= sdb->sdlc_largest_frame) ||
	(sdb->sdlc_idb->configured_role == SDLC_NONE) ||
        (sdb->sdlc_idb->configured_role == SDLC_PRIM_XID_POLL) ||
        (sdb->sdlc_xid_poll == TRUE) ||
	(sdb->sdlc_xid_passthru == TRUE))
    {
	if (!sdlc_encapsulate(pak, pak->datagramsize, sdb)) {
	    datagram_done(pak);
	    return;
	}
	sdlc_holdq(pak->if_output->hwptr, sdb, pak);
    } 
    else
    {
        queuetype q;
	if (sna_th_frag(pak, sdb->sdlc_largest_frame, &q)) {
	    sendqueue(&q, (SendFragProc)sdlc_sendfrag, sdb);
        } else {
	    datagram_done(pak);    /* no buffer, drop packet to ground */
	    sdlcClsNotify(sdb, CLS_DISC_IND, CLS_DISC_ERROR);
	}
    }    

    if (IsDlswSdb(sdb) || IsFrasBanSdb(sdb))
	sdlc_flow_ctrl_ind(sdb);
    
}


/************************************************************************
 * 		       CLSI Primitive: Flow.req
 ************************************************************************
 * Send RNR if fResumeFlow is false; otherwise, send RR.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(cookie, DLC_FLOW_REQ, &param_block)
 * 	cookie should be DLC's p_cep_id
 *
 * ***********************************************************************/
static clsSdlcMapFnT sdlcFlowReq (sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{

    cls_sdlc_debug(NULL, sdb, "Received Flow Request: ");
    if (cls_debug_sdlc)
	buginf(" %s congested condition",
	       (clsBlock->fParameters.fResumeFlow ? "clear" : "set"));
	       
    if (clsBlock->fParameters.fResumeFlow)
	sdlc_set_clear_busy(sdb->sdlc_idb, sdb->sdlc_address, FALSE);
    else
	sdlc_set_clear_busy(sdb->sdlc_idb, sdb->sdlc_address, TRUE);
    
}

/************************************************************************
 * 		       CLSI Primitive: Pong.req
 ************************************************************************
 * Set up for generalized Pong Handling, and off we go
 *
 * ***********************************************************************/
static clsSdlcMapFnT sdlcPongReq (sdlc_data_block *sdb, ClsDlcParamBlockT *clsBlock)
{
    CepPongRequest(sdb->u_cep_id, &sdb->fCep);
    return;
}


/************************************************************************
 *		    CLSI Primitive: Disconnect.req
 ************************************************************************
 * SDLC should send DISC if primary or RDISC if secondary.
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(cookie, DLC_DISC_REQ, &param_block)
 * 	cookie should be SDLC's p_cep_id
 *
 **********************************************************************
 */

static clsSdlcMapFnT sdlcDisconnectReq (sdlc_data_block *sdb,
				       ClsDlcParamBlockT *clsBlock)
{
    cls_sdlc_debug(NULL, sdb, "Disconnect Request");

    if (IS_SDLC_CONNECT(sdb)) {
	sdlc_setstate(sdb->sdlc_idb, SDLC_DISCSENT, sdb);
	sdlc_set_pause_timer(sdb->sdlc_idb, sdb);
	sdb->cls_state = CLS_DISCCNF_PEND;

    }
    else {
	sdlcClsNotify(sdb, CLS_DISC_CNF, CLS_OK);
	sdb->cls_state = CLS_STN_OPENED;
    }

    if (IsPrimPu2(sdb))
        sdb->sdlc_xid_count = 0;
}

/************************************************************************
 *		    CLSI Primitive: Disconnect.rsp
 ************************************************************************
 * If Primary, SDLC has already sent UA to inbound DISC
 *
 * DLC Message Generated
 * ---------------------
 * sdlc_wput(cookie, DLC_DISC_REQ, &param_block)
 * 	cookie should be SDLC's p_cep_id
 *
 **********************************************************************
*/

static clsSdlcMapFnT sdlcDisconnectRsp (sdlc_data_block *sdb,
				       ClsDlcParamBlockT *clsBlock)
{
    cls_sdlc_debug(NULL, sdb, "Disconnect Response");
    if (sdb->cls_state == CLS_DISC_IND_SENT)
	sdb->cls_state = CLS_STN_OPENED;
}


/*
 * sdlc_wput puts CLSI message to SDLC driver.
 */
void sdlc_wput (DlcPCepIdT pCepId, DlcOpcodeT opcode,
	       ClsDlcParamBlockT *clsParamBlock)
{
    sdlc_data_block *sdb;
    if (pCepId == CLS_NO_CEP) {
	sdb = NULL;
    } else {
	sdb = (sdlc_data_block *)pCepId;
    }

    if (!sdb || is_sdlc_marked_deleted(sdb)) {
	switch (opcode) {

	    /* 
	     * These op codes are not specific to a cep
	     * So not affected by zombie marker
	     */

	case DLC_INIT_SYSTEM_REQ:
	case DLC_ENABLE_REQ:
	case DLC_DISABLE_REQ:
	case DLC_ACTIVATE_SAP_REQ:
	case DLC_MODIFY_SAP_REQ:
	case DLC_DEACTIVATE_SAP_REQ:
	case DLC_ACTIVATE_RING_REQ:
	case DLC_DEACTIVATE_RING_REQ:
	case DLC_TEST_STN_REQ:
	case DLC_REQ_OPNSTN_REQ:
	case DLC_MODIFY_STN_REQ:
	    break;

	    /* 
	     * These op codes are specific to a cep
	     * If the CEP is a zombie, ingore the requests
	     */

	case DLC_CONNECT_REQ:
	case DLC_CONNECT_RSP:
	case DLC_SIGNAL_STN_REQ:
	case DLC_DISC_REQ:
	case DLC_DISC_RSP:
	case DLC_FLOW_REQ:
	case DLC_PONG_REQ:
	    return;

	    /* 
	     * These op codes are specific to a cep
	     * If the CEP is a zombie, ingore the requests.
	     * Be nice and clean up the buffers.
	     */

	case DLC_DATA_REQ:
	case DLC_ID_REQ:
	case DLC_ID_RSP:
	case DLC_TEST_REQ:
	    datagram_done(clsParamBlock->fPakPtr);
	    return;

	    /*
	     * The meathook through the liver
	     * Die, Zombie, Die
	     */

	case DLC_CLOSE_STN_REQ:
	    sdlcClsNotify(sdb, CLS_CLOSE_STN_CNF, CLS_OK);
	    free(sdb);
	    return;

	default:
	    if (cls_debug_sdlc) 
		buginf("\nsdlc_wput: ignoring request, unknown opcode %x", opcode);
	    return;
	}
    }
    switch (opcode)
    {
    case DLC_INIT_SYSTEM_REQ:
	sdlcInitSystem(sdb, clsParamBlock); break;
	
    case DLC_ENABLE_REQ:
	sdlcEnableReq(sdb, clsParamBlock); break;

    case DLC_DISABLE_REQ:
	sdlcDisableReq(sdb, clsParamBlock); break;

    case DLC_ACTIVATE_SAP_REQ:
	sdlcActivateSapReq(sdb, clsParamBlock); break;

    case DLC_MODIFY_SAP_REQ:
	sdlcModifySapReq(sdb, clsParamBlock); break;

    case DLC_DEACTIVATE_SAP_REQ:
	sdlcDeactivateSapReq(sdb, clsParamBlock); break;

    case DLC_REQ_OPNSTN_REQ:
	sdlcReqOpnStnReq(sdb, clsParamBlock); break;

    case DLC_CLOSE_STN_REQ:
	sdlcCloseStnReq(sdb, clsParamBlock); break;

    case DLC_MODIFY_STN_REQ:
	sdlcModifyStnReq(sdb, clsParamBlock); break;

    case DLC_ID_REQ:
    case DLC_ID_RSP:
	sdlcIdReq(sdb, clsParamBlock); break;

    case DLC_CONNECT_REQ:
	sdlcConnectReq(sdb, clsParamBlock); break;

    case DLC_CONNECT_RSP:
	sdlcConnectRsp(sdb, clsParamBlock); break;

    case DLC_SIGNAL_STN_REQ:
	sdlcSignalStnReq(sdb, clsParamBlock); break;

    case DLC_DISC_REQ:
	sdlcDisconnectReq(sdb, clsParamBlock);
	break;

    case DLC_DISC_RSP:
	sdlcDisconnectRsp(sdb, clsParamBlock); break;

    case DLC_DATA_REQ:
	sdlcDataReq(sdb, clsParamBlock); break;

    case DLC_FLOW_REQ:
	sdlcFlowReq(sdb, clsParamBlock); break;

    case DLC_PONG_REQ:
	sdlcPongReq(sdb, clsParamBlock); break;

    case DLC_ACTIVATE_RING_REQ:
        sdlcActivateRingReq(sdb, clsParamBlock); break;

    case DLC_DEACTIVATE_RING_REQ:
        sdlcDeactivateRingReq(sdb, clsParamBlock); break;

    case DLC_TEST_STN_REQ:
        sdlcTestStnReq(sdb, clsParamBlock); break;

    case DLC_TEST_REQ:
        sdlcTestReq(sdb, clsParamBlock); break;

    default:
	if (cls_debug_sdlc) 
	    buginf("\nsdlc_wput: ignoring request, unknown opcode %x", opcode);	
    }
}

/* 
 * sdlcBuildParmBlock builds parameter block to be sent to CLS.
 */
static ClsDlcParamBlockT
*sdlcBuildParmBlock (sdlc_data_block *sdb,
		    ClsOpcodeT opcode,
		    ClsDlcParamBlockT *paramBlock,
		    ClsErrorCodeT ClsDlcErrorCode)
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
     * this field are CLS_DATA_IND, and CLS_ID_IND. The cases for
     * these opcodes handle setting the field.
     */
    paramBlock->fPakPtr = NULL;

    switch (opcode) {

	/*
	 * handled elsewhere or unused
	 */
      case CLS_NO_OP:
      case CLS_ENABLE_CNF:
      case CLS_DISABLE_CNF:
      case CLS_ACTIVATE_SAP_CNF:        
      case CLS_MODIFY_SAP_CNF: 
      case CLS_DEACTIVATE_SAP_CNF:
      case CLS_REQ_OPNSTN_CNF:
      case CLS_MODIFY_STN_CNF:
      case CLS_CLOSE_STN_CNF:
      case CLS_TEST_IND:
      case CLS_ID_IND:
      case CLS_CONNECTED_IND:
      case CLS_TEST_STN_IND:          
      case CLS_UDATA_STN_IND:
      case CLS_UDATA_IND:
      case CLS_CONNECT_STN_IND:
      case CLS_ID_STN_IND:
      case CLS_ACTIVATE_RING_CNF:
      case CLS_DEACTIVATE_RING_CNF:
      case CLS_PONG_CNF:
      case CLS_DATA_CNF:
      case CLS_FLOW_IND:
	break;

      case CLS_CONNECT_IND:
      case CLS_CONNECT_CNF:
	
	cls_sdlc_debug(NULL, sdb, "sending CLS_CONNECT_IND/CNF");

	paramBlock->fParameters.fDlcConnect.fPPortId = sdb->sdlc_idb->firstsw;
	paramBlock->fParameters.fDlcConnect.fDlcPCepId = (DlcPCepIdT)sdb;
	paramBlock->fParameters.fDlcConnect.fClsUCepId = sdb->u_cep_id;

	/* return code is ok for now */
	paramBlock->fParameters.fDlcConnect.fLlcRetCode = 0;
	break;

      case CLS_DISC_IND:
      case CLS_DISC_CNF:
	cls_sdlc_debug(NULL, sdb, "sending CLS_DISC_IND/CNF");
	paramBlock->fParameters.fDlcConnect.fPPortId = sdb->sdlc_idb->firstsw;
	paramBlock->fParameters.fDlcConnect.fDlcPCepId = (DlcPCepIdT)sdb;
	paramBlock->fParameters.fDlcConnect.fClsUCepId = sdb->u_cep_id;

	/* return code is ok for now */
	paramBlock->fParameters.fDlcConnect.fLlcRetCode = 0;
	break;

      case CLS_DATA_IND:
	sdb->inbuf->datagramstart += SDLCHEADERBYTES;
	sdb->inbuf->datagramsize -= SDLCHEADERBYTES;
	paramBlock->fLoadLength = sdb->inbuf->datagramsize;
	paramBlock->fPakPtr = sdb->inbuf;
	sdb->inbuf = NULL;
	break;
    }
    return (paramBlock);
}

/*
 * Got some kind of reply for CLS about Request Open Station Request
 */

static void SdlcReqOpenStnConf (
    sdlc_data_block	*sdb,
    ClsUCepIdT		uCepId,
    ClsErrorCodeT 	error)
{
    ClsDlcParamBlockT 	clsBlock;

    clsBlock.fResultError = error;
    clsBlock.fParameters.fDlcReqOpenStn.fClsUCepId = uCepId;

    clsBlock.fParameters.fDlcReqOpenStn.fDlcPCepId = (DlcPCepIdT)sdb;

    clsBlock.fPakPtr = NULL;
    reg_invoke_cls_rput(uCepId, CLS_REQ_OPNSTN_CNF, &clsBlock);
    return;
}

/*
 * sdlcCLSNotify notifies CLS of SDLC events
 */
void sdlcClsNotify (sdlc_data_block *sdb, ClsOpcodeT opcode,
				     ClsErrorCodeT ClsDlcErrorCode)
{
    ClsDlcParamBlockT clsDlcParamBlock;
    
    memset(&clsDlcParamBlock, 0, sizeof(ClsDlcParamBlockT));
    sdlcBuildParmBlock(sdb, opcode, &clsDlcParamBlock, ClsDlcErrorCode);
    reg_invoke_cls_rput(sdb->u_cep_id, opcode, &clsDlcParamBlock);
}

/*
 * sdlcSendXidToCLS receives an sdlc xid packet, formats an ID indication
 * and sends that to CLS.
 */
void sdlcSendXidToCLS (sdlc_data_block *sdb, paktype *pak)
{
    ClsDlcParamBlockT	clsBlock;
    DlcIdStnT		*dlcIdStn;
    SdlcXidIdStnT	*sdlcIdStn;
    uchar		addr = sdb->sdlc_address;
    uchar		*ptr;
    uchar               *sdlc_partner;

    /*
     * This can happen due to a race condition between the DLU going
     * through the Enable.Req, Enable.Cnf, ActivateRing.Req, and
     * ActivateRing.Cnf and the sdlc proxy xid code sending an Id.Ind.
     */
    if (!CLSSapCheck((TCLSSap *) sdb->u_sap_id)) {
        if (cls_debug_sdlc) {
            buginf("\nSDLC SAP for %x is not active", sdb->sdlc_address);
        }
        datagram_done(pak);
        return;
    }
 
    dlcIdStn = &clsBlock.fParameters.fDlcIdStn;
    
    sdlcIdStn = &dlcIdStn->fDlcXid.fSdlcXidStn;

    dlcIdStn->fXIDLength = pak->datagramsize - SDLCHEADERBYTES;
    
     if (sdb->cls_state == CLS_STN_CLOSED) {
	/*
 	 * If the station's closed we'll send ID.STN.IND
	 * This requires a lot of stuff in the encaps area
 	 * We may have to grow the packet to get it in.
 	 *
 	 * This must be done before we compute any offsets, since
 	 * pak_grow_realign may need to acquire a new packet
	 */
 	paktype		*xidpak;
	int		hdrbytes;
 
	dlcIdStn->fClsUCepId = (void *)CLS_NO_CEP;
 	
 	/* 
	 * make allowances if the clsi message won't fit
 	 * in what's left of the encaps area.
 	 * We only want to do this if there's no context
 	 */
 
 	if (cls_debug_sdlc) {
 	    buginf("\npak 0x%x, start 0x%x nw start 0x%x, encaps %d",
 		   pak, pak->datagramstart, pak->network_start, 
 		   (sizeof(IdStnInd_t) + sizeof(long)) );
	}

	hdrbytes = sizeof(IdStnInd_t) + sizeof(long);
	if (sdb->addr_type == STATION_IEEE48) {
	    hdrbytes += TRING_ENCAPBYTES;
	}
  	xidpak = pak_grow_realign(pak, pak->datagramsize, hdrbytes);
	if (xidpak == NULL) {
	    /*
	     * we couldn't use the original pak, and,
	     * even worse - we're fresh out of buffers
	     */
	    datagram_done(pak);
	    return;			/* we just dropped the XID.  Oh well ... */
	}
	/*
	 * make sure we got the latest incarnation of the xid packet; 
	 * it's easier to do the assignment than to test and assign
	 * conditionally.
	 */
	pak = xidpak;
    }
    else {
	dlcIdStn->fClsUCepId = sdb->u_cep_id;
    }

    dlcIdStn->fXIDPtr = pak->datagramstart + SDLCHEADERBYTES;

    if (sdb->addr_type == STATION_SDLC) {
	sdlcIdStn->fMacType = STATION_SDLC;
	sdlcIdStn->fLMacLength = 1;
	sdlcIdStn->fRMacLength = 1;
	pak->addr_start = pak->datagramstart;
	pak->info_start = pak->addr_start + 1;
	pak->network_start = pak->info_start + 1;
	sdlcIdStn->fLMacPtr = pak->addr_start;
	sdlcIdStn->fRMacPtr = pak->addr_start;
    } else {
	/*
	 *  Local MAC address is SDLC partner's MAC address.
	 *  Local MAC address is the frame's destination address.
	 *  Remote MAC address is SDLC virtual MAC for the SDLC station.
	 *  Remote MAC address is the frame's source address.
	 *  No RIF used for SDLC to CLS Mac frames.
	 */

	sdlcIdStn->fMacType = STATION_IEEE48;
	pak->datagramstart = pak->datagramstart - (2 * IEEEBYTES) - 1
	    - TR_ACFC_SIZE;
	pak->datagramsize = pak->datagramsize + (2 * IEEEBYTES) + 1 
	    + TR_ACFC_SIZE;
	pak->addr_start = pak->datagramstart + TR_DA_OFFSET;
        sdlc_partner = SdlcGetPartner(sdb);
        if (sdlc_partner) {
           ieee_copy(sdlc_partner, pak->addr_start);
	}

	/* 
	 * Build the source mac address from the vmac. Then assign the sdlc 
	 * address to the last byte of the source address.
	 */
	ieee_copy(sdb->sdlc_idb->sdlc_vr_macaddr, pak->addr_start + IEEEBYTES);
	ptr = pak->addr_start + (2 * IEEEBYTES) - 1;
	*ptr = addr;

	pak->info_start = pak->addr_start + (2 * IEEEBYTES);
	pak->network_start = pak->info_start + SAP_HDRBYTES;

	sdlcIdStn->fLMacLength = IEEEBYTES;
	sdlcIdStn->fRMacLength = IEEEBYTES;
	sdlcIdStn->fLMacPtr = pak->addr_start;
	sdlcIdStn->fRMacPtr = pak->addr_start + IEEEBYTES;

	sdlcIdStn->fRSap = sdb->sdlc_dsap & ~SAP_RESPONSE;
	sdlcIdStn->fLSap = sdb->sdlc_ssap;
    }
    pak->rif_start = NULL;
    pak->riflen = 0;	
    sdlcIdStn->fRoutingLength = 0;
    sdlcIdStn->fRoutingPtr = NULL;

    dlcIdStn->fXidT = sdb->xid_resp ? XID_RESPONSE: XID_COMMAND;
    dlcIdStn->fDlcPCepId = (DlcPCepIdT)sdb;
    dlcIdStn->fClsUSapId = (ClsUSapIdT)sdb->u_sap_id;
    clsBlock.fResultError    = CLS_OK;
    clsBlock.fPakPtr         = pak;
    clsBlock.fPortId.type = CLS_P_TYPE_SWIDB;
    clsBlock.fPortId.fTypeInfo.swidb = firstsw_or_null(sdb->sdlc_idb);

    cls_sdlc_debug(NULL, sdb, "sending ID.Ind to DLU");
    if (cls_debug_sdlc) {
        if (pak->datagramsize > 0) {
            buginf("\n\t\tXID format %d, type %d %d bytes",
                   xid_get_format(dlcIdStn->fXIDPtr),
                   xid_get_node_type(dlcIdStn->fXIDPtr),
                   xid_get_length(dlcIdStn->fXIDPtr));
	    memory_dump(pak->datagramstart, 
			pak->datagramstart + pak->datagramsize -1,
			FALSE, MEMORY_DUMP_DATA_TYPE_EBCDIC, buginf);
	}
        else
            buginf("\n\t\tXID (NULL)");
    }

    reg_invoke_cls_rput(sdb->u_cep_id, CLS_ID_IND, &clsBlock);
}


/* prepare a NULL or FULL xid */
paktype *GetProxyXid (sdlc_data_block *sdb, boolean fullxid)
{
    paktype *pak;
    sdlc_frametype *frame;
    uchar *xidp;
    
    pak = getbuffer(SDLCHEADERBYTES + SDLC_XID_SIZE + 2);

    if (!pak) {
	return(NULL);
    }

    frame = (sdlc_frametype *)pak->datagramstart;
    frame->address = sdb->sdlc_address;
    frame->control = SDLC_XID | SDLC_PF;
    if (fullxid) {
	xidp = pak->datagramstart + SDLCHEADERBYTES;
	
	xidp[0] = XID_TYPE0_FMT2;
	xidp[1] = 0x0;
	xidp +=2;
	bcopy(&(sdb->sdlc_xid), xidp, SDLC_XID_SIZE);
	pak->datagramsize = SDLCHEADERBYTES + SDLC_XID_SIZE + 2;
    } else 
	pak->datagramsize = SDLCHEADERBYTES;
    return(pak);
}

/* 
 * Returns TRUE if the station is primary PU2 node.  
 * Returns FALSE for 2.1 and any other node type.
 */
boolean IsPrimPu2 (sdlc_data_block *sdb)
{
    if (sdb && (ISCLSSDLC(sdb->sdlc_idb)) &&
        (sdb->sdlc_idb->configured_role == SDLC_PRIMARY) &&
        (sdb->sdlc_xid_poll == FALSE) &&
        (sdb->sdlc_switched == FALSE) &&
	(sdb->sdlc_xid_passthru == FALSE))
        return(TRUE);
    else 
	return(FALSE);
}

/* Returns TRUE if the station is primary node that should initiate xids*/
boolean IsPrimXidPoll (sdlc_data_block *sdb)
{
    if (sdb && (ISCLSSDLC(sdb->sdlc_idb)) &&
        ((sdb->sdlc_idb->configured_role == SDLC_PRIM_XID_POLL) ||
         ((sdb->sdlc_idb->configured_role == SDLC_PRIMARY) &&
	  ((sdb->sdlc_switched == TRUE) ||
	   (sdb->sdlc_xid_passthru == TRUE) ||
	   (sdb->sdlc_xid_poll == TRUE)))))
      return(TRUE);
    else 
      return(FALSE);
}

/* Returns TRUE if the station is secondary PU2 node */
boolean IsSecPu2 (sdlc_data_block *sdb)
{
    if (sdb && (ISCLSSDLC(sdb->sdlc_idb)) &&
	(sdb->sdlc_idb->configured_role == SDLC_SECONDARY) &&
	(sdb->sdlc_xid_poll != TRUE) &&
	(sdb->sdlc_xid_passthru != TRUE))
	return(TRUE);
    else
	return(FALSE);
}

/* Returns TRUE if the station is configured for DLSw */
boolean IsDlswSdb (sdlc_data_block *sdb)
{
     if (!sdb || !ISCLSSDLC(sdb->sdlc_idb))
        return(FALSE);
     if ((sdb->sdlc_dlsw) || (sdb->sdlc_idb->sdlc_default == DLSW_SDLC))
         return(TRUE);
     return(FALSE);
}

/* Returns TRUE if the station is configured for FRAS-BAN-SDLC */
boolean IsFrasBanSdb (sdlc_data_block *sdb)
{
     if (!sdb || !ISCLSSDLC(sdb->sdlc_idb))
        return(FALSE);
     if (sdb->sdlc_fras_ban)
        return(TRUE); 
     return(FALSE);
}

/* Returns the pointer to sdlc partner if present, else returns NULL */
static uchar* SdlcGetPartner (sdlc_data_block *sdb)
{
     if (!sdb || !ISCLSSDLC(sdb->sdlc_idb)) {
         return(NULL);
     }
     if (sdb->sdlc_have_prtnr) {
        return(sdb->sdlc_prtnr);
     }
     if (sdb->sdlc_idb->sdlc_have_default_partner) {
        return(sdb->sdlc_idb->sdlc_default_partner);
     }
     return(NULL);
}

/* Scan through all Network Name Control Vector(s) (0x0E) in the  
 * XID3 searching for the Link Station Name Vector Data (0xF7). If
 * found, copy the link station name into the sdlc linkstation 
 * operName.  
 */
static void FindLinkStationNameInControlVector (byte *xidP, char* operName)
{
    byte nameLen;
    byte lenLeft = xidP[1] - 29;
    byte *cvP = &xidP[29];

    while (lenLeft > 3) {             /* 0-1 vector header key+len, 2 vector data key */
        if (cvP[2] == 0xF7) {                           /* find vector data */
            nameLen = ((cvP[1] > 10) ? 10 : cvP[1] - 1);
            bcopy(&cvP[3], operName, nameLen);
            sys_ebcdic_to_ascii(operName, nameLen);
            operName[nameLen] = 0;  /* null temination */
            return;
        }
        lenLeft -=  cvP[1] + 2;
        cvP += cvP[1] + 2;
    }
}


/* 
 * SdlcDeleteClsSdb
 *
 * User has issued no sdlc address <addr>, but there's still
 * an active sdlc connection
 */

void SdlcDeleteClsSdb (sdlc_data_block *sdb)
{
    if (sdb->u_cep_id == (void *)CLS_NO_CEP) {
	free(sdb);
    } else {
	sdlc_mark_deleted(sdb);
	ClsSdlcDiscInd(sdb);
    }
}

/* 
 * parse the XID to find the fields, RIM/SIM, Transmit 
 * Receive Capabilities and Link Station Name (if XID 3).  
 */
static void ParseXid (sdlc_data_block *sdb, paktype *pak)
{
    byte *xidP = (byte *)(pak->datagramstart + SDLCHEADERBYTES);
    int xidLen = pak->datagramsize - SDLCHEADERBYTES;
    /* 
     * Set the operName if this is an XID3 which has a link station 
     * name control vector otherwise, the operName is null.
     */
    sdb->sdlc_operName[0] = '\0';
    if (xidLen < 1)
        return;

    switch (xidP[0] & 0xF0){
        case 0x00: 
            sdb->sdlc_simRim = 0x00;        /* default : unsupported */
            sdb->sdlc_xmitRcvCap = 0x01;    /* default : tws         */        
            break;

        case 0x10:
            if (xidLen < 14)
                return;
            sdb->sdlc_simRim = (xidP[13] & 0x20);     /* 0 not supported, 
							 1 supported */
            sdb->sdlc_xmitRcvCap = (xidP[8] & 0x0F);  /* 0 twa, 1 tws */
            break;

        case 0x20: 
            if (xidLen < 36)
               return;
            sdb->sdlc_simRim = (xidP[35] & 0x30);     /* 0 unsupported, 
							 1 supported */
            sdb->sdlc_xmitRcvCap = (xidP[31] & 0x03); /* 0 twa, 1 tws */
            break;

        case 0x30: 
            if (xidLen < 25)
                return;
            sdb->sdlc_simRim = (xidP[24] & 0x20);     /* 0 unsupported, 
							 1 supported */
            sdb->sdlc_xmitRcvCap = (xidP[19] & 0x03); /* 0 twa, 1 tws */

            
            if (xidLen >= 32 && xidLen >= xidP[1])
                FindLinkStationNameInControlVector(xidP, sdb->sdlc_operName);
            break;
    }
}

/*
 * sdlc_cls_reset is called from sdlc_setstate.  It is the mechanism by
 * which CLS notifications are sent to clean up session context.
 */
void sdlc_cls_reset (sdlc_data_block *sdb) 
{
    cls_sdlc_debug(NULL, sdb, "sdlc_cls_reset");

    if (sdb->sdlc_state == SDLC_DISCONNECT) {
	switch (sdb->cls_state) {
	case CLS_ROSCNF_PEND:
	    SdlcReqOpenStnConf(sdb, sdb->u_cep_id, CLS_NO_REM_STN);
	    sdb->u_cep_id = (void*) CLS_NO_CEP;
	    sdb->cls_state = CLS_STN_CLOSED;
	    break;
	    
	case CLS_CLOSING:
	case CLS_DISC_IND_SENT:
	case CLS_STN_CLOSED:
	    break;
	    
	default:
	    sdb->inbuf = NULL;
	    sdlcClsNotify(sdb, CLS_DISC_IND, CLS_OK);
	    sdb->cls_state = CLS_DISC_IND_SENT;
	    break;
	}
    } else if (sdb->sdlc_state == SDLC_ERROR) {
	sdb->inbuf = NULL;
	sdlcClsNotify(sdb, CLS_DISC_IND, CLS_DISC_ERROR);
    }
}

/* 
 * SDLC wrapper wraps around SDLC driver providing interface
 * to CLS and serial driver.
 */
void sdlc_wrap_input (hwidbtype *idb, paktype *pak)
{
    sdlc_frametype *frame = (sdlc_frametype *)pak->datagramstart;
    uchar addr;
    sdlc_data_block *sdb;
    int i;
    paktype *newpak;

    sdlc_count_input(idb, pak);

    addr = frame->address;
    sdb = idb->sdlc_data[addr];

    /* 
     * If the address is a broadcast address, 
     * there should be only one station on this line.
     */
    if (addr == SDLC_BCAST_ADDR) {
	for (i=1; i<=SDLC_MAX_ADDRESS; i++) {
	    if (idb->sdlc_data[i]) {
		sdb = idb->sdlc_data[i];
		break;
	    }
	}
    }
    
    /*
     * Current_polled used for secondary to determine if
     * ID.Req can immediately send XID on the line.
     */
    if (idb->configured_role == SDLC_SECONDARY)
        idb->sdlc_current_polled = addr;

    if (!sdb) {
        sdlc_input(idb, pak);
	return;
    }

    addr = frame->address = sdb->sdlc_address;

    if (!sdb->u_sap_id) {
	datagram_done(pak);
	return;
    }

    if (SDLC_ISUFRAME(frame)) {
	switch(frame->control & ~SDLC_PF) {
	  case SDLC_TEST:           
            sdlc_input(sdb->sdlc_idb, pak);
            break;                
	  case SDLC_XID:

            /* 
             * Are we in switched mode and is the stn closed?
             * If so, this must be the response from our proxy XID.
	     * Toss it and send a proxy to the host.
	     */

	    if (ISCLSSDLC(sdb->sdlc_idb)   &&
		(sdb->sdlc_switched)       &&
		ISPSDLC(sdb->sdlc_idb)     &&
		(sdb->cls_state == CLS_STN_CLOSED)) {
		/* 
		 *  check response -- if this isn't an XID, we don't
	         *   want to send a proxy XID to the DLU.
		 * Could be in DM?
		 */
	        pak->datagramstart += SDLCHEADERBYTES;
		if ((pak->datagramsize > 0) &&          /* make sure it's an XID   */
		    (!is_valid_xid(pak))) {
		    if (sdlc_debug)
		      buginf("\nswitched device answered with non-xid");
		    datagram_done(pak);
		    return;
		}
		/*
		 * If the received XID is a response to an xid poll, then
		 * reset the sdlc state to XID_STOP. This avoids retrying
		 * the xid poll.
		 */
		if (sdb->sdlc_state == SDLC_XIDSENT)  
		  sdlc_setstate(sdb->sdlc_idb, SDLC_XIDSTOP, sdb);

		/*
		 * Free the proxy NULL XID we obtained in sdlc_retry_cls().
		 */
		if (sdb->outbuf) {                                                               
		    datagram_done(sdb->outbuf);          
		    sdb->outbuf = NULL;
		}

		datagram_done(pak);                     /* toss the response packet */
		pak = GetProxyXid(sdb,FALSE);           /* get a null XID           */
		ParseXid(sdb,pak);

		if (sdlc_debug)
		  buginf("\nSDLC_WRAP_INPUT: Sending XID to CLS");
		sdlcSendXidToCLS(sdb,pak);

		/* 
		 * For switched_startup interval, use slow-poll timer
		 * value if configured. If not, use T1 value.
		 */
		if (idb->sdlc_slow_poll_interval) { 
		    TIMER_START(sdb->sdlc_switched_startup_timer,
				idb->sdlc_slow_poll_interval);
		}  else {
		    TIMER_START(sdb->sdlc_switched_startup_timer,
				sdb->sdlc_idb->sdlc_t1);	
		}
		return;
	    }





            /*
             * If our role is secondary and we're holding an XID and
             * waiting for a poll, this is it.
             *
             * If our role is primary and we're holding an XID, then this
             * is our response so we free the XID.
             */
            sdb->sdlc_retcnt = 0;

            if (sdb->cls_state == CLS_DISCCNF_PEND) {
                sdlcClsNotify(sdb, CLS_DISC_CNF, CLS_OK);
                sdb->cls_state = CLS_STN_OPENED;
            }

            if (sdb->outbuf) {
                if (sdb->sdlc_idb->configured_role == SDLC_SECONDARY) {
                    datagram_done(pak);
                    sdlc_sendframe(SDLC_XID, TRUE, idb, sdb->outbuf, sdb);
                    if (sdb->cls_state != CLS_IN_SESSION)
                        sdlc_setstate(sdb->sdlc_idb, SDLC_XIDSENT, sdb);
                    sdb->outbuf = NULL;
                    return;
                }
                else {
                    datagram_done(sdb->outbuf);
                    sdb->outbuf = NULL;
                }
            }

            /*
             * If group poll, respond to NULL XID with DM so
             * primary will poll the next station without delay.
             * Set current polled to zero so that when an ID.req comes in,
             * it won't go thru to the FEP.
             */
            if ((pak->datagramsize < 3) && (idb->sdlc_group_addr)) {
                newpak = getsdlc(idb, "DM");
                if (newpak)
                    sdlc_sendframe(SDLC_DM, TRUE, idb, newpak, sdb);
                idb->sdlc_current_polled = 0;
            }

            /*
             * NULL XID is too big for ENCAPBYTES.  Instead of forwarding pak,
             * or adjusting the pak, just forward a new NULL XID.
             */
            if (pak->datagramsize < 3) {
                /*
                 * This happens if attached device was reset.  We don't get
                 * a physical layer drop if we're connected to a line-sharing
                 * device.
                 */
                if (sdb->cls_state > CLS_STN_OPENED) {
                    sdlc_sendframe(SDLC_DM, TRUE, idb, pak, sdb);
                    sdlcClsNotify(sdb, CLS_DISC_IND, CLS_OK);
                    sdb->cls_state = CLS_DISC_IND_SENT;
                    sdlc_setstate(sdb->sdlc_idb, SDLC_DISCONNECT, sdb);
                }
                else
                    datagram_done(pak);
                pak = GetProxyXid(sdb, FALSE);
                if (!pak) {
                    if (cls_debug_sdlc)
                        buginf("\nSDLC_WRAPPER: No buffer for Proxy Xid");
                    return;
                }
            }
            ParseXid(sdb, pak);
            if (sdb->sdlc_state == SDLC_XIDSENT)
                sdlc_setstate(sdb->sdlc_idb, SDLC_XIDSTOP, sdb);
            sdlcSendXidToCLS(sdb, pak);
            break;
	  case SDLC_SNRM:
            /*
             * Don't save SNRM, reuse pak or free it.
             */
            if (sdb->outbuf) {
                datagram_done(sdb->outbuf);
                sdb->outbuf = NULL;
            }

            /*
             * Since we received a SNRM, the router must
             * be secondary. We need to take different actions
             * depending on whether we are connected to a
             * PU 2.0 or 2.1 device.
             */
            if (IsSecPu2(sdb)) {
                switch (sdb->cls_state) {
                  case CLS_STN_CLOSED:
                    /*
                     * Have not received ReqOpnStn.Req yet, so
                     * drop the SNRM.
                     */
                    sdb->sdlc_state = SDLC_SNRMSEEN;
                    if (idb->sdlc_group_addr)
                        sdlc_sendframe(SDLC_DM, TRUE, idb, pak, sdb);
		    else
			datagram_done(pak);
                    return;

                  case CLS_DISCCNF_PEND:
		    if (sdb->cls_state == CLS_DISCCNF_PEND) {
			sdlcClsNotify(sdb, CLS_DISC_CNF, CLS_OK);
			sdb->cls_state = CLS_STN_OPENED;
		    }
		    datagram_done(pak);
		    return;

                  case CLS_DISC_IND_SENT:
                    /*
                     * We are on our way down. Drop the SNRM.
                     */
                    if (idb->sdlc_group_addr)
                        sdlc_sendframe(SDLC_DM, TRUE, idb, pak, sdb);
		    else
			datagram_done(pak);
                    return;

                  case CLS_ROSCNF_PEND:
                    /*
                     * The DLU previously issued a ReqOpnStn.Req, it is
                     * now time to answer it. If configured for
                     * group poll send DM to allow FEP to move on to
                     * polling other stations.
                     */
                    SdlcReqOpenStnConf(sdb, sdb->u_cep_id, CLS_OK);
                    sdb->cls_state = CLS_STN_OPENED;
                    sdb->sdlc_state = SDLC_SNRMSEEN;
                    if (idb->sdlc_group_addr)
                        sdlc_sendframe(SDLC_DM, TRUE, idb, pak, sdb);
                    else
                        datagram_done(pak);
                    return;

                  case CLS_STN_OPENED:
                  case CLS_FULL_XID_PEND:
                  case CLS_CONNECT_RSP_PEND:
                    /*
                     * We are waiting for downstream XID negotiation
                     * followed by connect.ind to complete. 
                     * If configured for group poll, send DM to allow
                     * FEP to move on to polling other stations.
                     */
                    if (idb->sdlc_group_addr)
                        sdlc_sendframe(SDLC_DM, TRUE, idb, pak, sdb);
                    else
                        datagram_done(pak);
                    return;

                  case CLS_CONNECTED_IND_PEND:
                    /*
                     * CLS_CONNECTED_IND_PEND state indicates that
                     * we've received Connect.Rsp from DLU.
                     * Finally allow sdlc_input() to send UA.
                     * sdlc_input() consumes pak.
                     */
                    sdb->sdlc_state = SDLC_SNRMSEEN;
                    sdlc_input(sdb->sdlc_idb, pak);
                    return;

                  case CLS_IN_SESSION:
                    /*
                     * SNRM in the middle of a session.
                     */
                    sdlc_sendframe(SDLC_DM, TRUE, idb, pak, sdb);
                    sdlcClsNotify(sdb, CLS_DISC_IND, CLS_OK);
                    sdb->cls_state = CLS_DISC_IND_SENT;
		    sdlc_setstate(sdb->sdlc_idb, SDLC_DISCONNECT, sdb);
                    return;

                  default:
                    datagram_done(pak);
                    return;
                }
            } else {
                /* If this idb is configured as sdlc Primary,
                 * then it shouldn't be seeing SNRM's. So 
                 * drop the packet and report an error.
                 */
                if (IsPrimPu2(sdb) || IsPrimXidPoll(sdb)) {
                    errmsg(&msgsym(CONFIGERR, SDLC), 
                            sdb->sdlc_idb->hw_namestring, sdb->sdlc_address);
                    datagram_done(pak);
                    return;
                }
                /*
                 * Router is secondary  and negotiable.
                 * XID exchange has completed.
                 */
                switch (sdb->cls_state) {
                  case CLS_STN_CLOSED:
                    sdb->sdlc_state = SDLC_SNRMSEEN;
                    datagram_done(pak);
		    return;

                  case CLS_DISCCNF_PEND:
		    if (sdb->cls_state == CLS_DISCCNF_PEND) {
			sdlcClsNotify(sdb, CLS_DISC_CNF, CLS_OK);
			sdb->cls_state = CLS_STN_OPENED;
		    }
                    datagram_done(pak);
		    return;

                  case CLS_DISC_IND_SENT:
                  case CLS_ROSCNF_PEND:
                    /*
                     * If DISC_IND_SENT then we
                     * are on our way down. Drop the SNRM.
                     * For secondary negotiable the ReqOpnStn.Cfm
                     * is sent immediattely, so we shouldn't be
                     * in CLS_ROSCNF_PEND state.
                     */
                    datagram_done(pak);
                    return;

                  case CLS_STN_OPENED:
                    /*
                     * XID exchange has obviously completed
                     * so pass connect ind to dlu.
                     */
                    sdb->cls_state = CLS_CONNECT_RSP_PEND;
                    sdlcClsNotify(sdb, CLS_CONNECT_IND, CLS_OK);
                    datagram_done(pak);
                    return;

                  case CLS_CONNECTED_IND_PEND:
                    /*
                     * We've received a connect response.  Now it is time to
                     * respond to SNRM with UA.  
                     */
                    sdlc_input(sdb->sdlc_idb, pak);
                    return;

                  case CLS_CONNECT_RSP_PEND:
                    /*
                     * Waiting for DLU to send Connect.Rsp.
                     * Drop the SNRM.
                     */
                    datagram_done(pak);
                    return;

                  case CLS_IN_SESSION:
                    /*
                     * SNRM in the middle of a session.
                     */
                    sdlc_sendframe(SDLC_DM, TRUE, idb, pak, sdb);
                    sdb->sdlc_state = SDLC_DISCONNECT;
                    sdb->cls_state = CLS_DISC_IND_SENT;
                    sdlcClsNotify(sdb, CLS_DISC_IND, CLS_OK);
                    return;

                  case CLS_FULL_XID_PEND:
                  default:
		    CLSASSERT(!"SDLC: SNRM - Invalid state for 2.1 station\n");
		    datagram_done(pak);
		    return;
                }
            }
	    break;

	  case SDLC_DISC:		/* Received DISC/RD */

	    cls_sdlc_debug(NULL, sdb, "Received DISC");
	    ClsSdlcDiscInd(sdb);	/* Tell CLS */
	    sdlc_input(sdb->sdlc_idb, pak);
	    break;

	  case SDLC_UA:
	    /* 
	     * For primary PU2 stations, send reqopnstn confirm
	     * when the secondary station responds to SNRM with UA.
	     * Delay the null proxy XID to DLU to allow the throttling logic
	     * to control the pace which we bring PU's online.
	     *
	     * For NT2.1 stations, send connect confirm to DLU
	     * since everything is pass through.
	     */
	    if (IsPrimPu2(sdb)) {
                TIMER_START(sdb->sdlc_initiation_throttle_time, 0);
		if (sdb->cls_state == CLS_ROSCNF_PEND) {
		    SdlcReqOpenStnConf(sdb, sdb->u_cep_id, CLS_OK);
                    sdb->cls_state = CLS_STN_OPENED;
		}
	    }
	    else if (sdb->sdlc_state == SDLC_SNRMSENT) {
	      /*
	       * Primary PU2.1, post XID SNRM sent.  This is the processing
	       * of the UA sent back by the secondary.
	       */
	      sdlcClsNotify(sdb, CLS_CONNECT_CNF, CLS_OK);
	      sdb->cls_state = CLS_IN_SESSION;
	    }

	    if (sdb->cls_state == CLS_DISCCNF_PEND) {
		sdlcClsNotify(sdb, CLS_DISC_CNF, CLS_OK);
		sdb->cls_state = CLS_STN_OPENED;
	    }

	    cls_sdlc_debug(NULL, sdb, "Received UA");
	    sdlc_input(sdb->sdlc_idb, pak);
	    break;
	  case SDLC_DM:
	    if (sdb->sdlc_state == SDLC_XIDSENT) {
		sdlc_setstate(sdb->sdlc_idb, SDLC_XIDSTOP, sdb);
		sdb->sdlc_state = SDLC_DISCONNECT;
	    } else if (sdb->sdlc_state == SDLC_XIDSTOP)
		sdb->sdlc_state = SDLC_DISCONNECT;

	    if (sdb->cls_state == CLS_DISCCNF_PEND) {
		sdlcClsNotify(sdb, CLS_DISC_CNF, CLS_OK);
		sdb->cls_state = CLS_STN_OPENED;
	    }
	    /* intentional fall through */
	  default:
	    sdlc_input(sdb->sdlc_idb, pak);
	    break;
	    }
	    sdb->xid_resp = FALSE;
    } else if (SDLC_ISSFRAME(frame)) {
	switch(frame->control & 0xf) {
	  case SDLC_RNR:
	  case SDLC_RR: 
	    if (sdb->cls_state == CLS_CONNECTED_IND_PEND) {
		sdlcClsNotify(sdb, CLS_CONNECTED_IND, CLS_OK);
		sdb->cls_state = CLS_IN_SESSION;
	    }
	    /* intentional fall through */
	  default:
	    sdlc_input(sdb->sdlc_idb, pak);
	    break;
	}
    } else {
	if (SDLC_ISIFRAME(frame) && (sdb->cls_state ==
				     CLS_CONNECTED_IND_PEND)) {
	    sdlcClsNotify(sdb, CLS_CONNECTED_IND, CLS_OK);
	    sdb->cls_state = CLS_IN_SESSION;
	}
	sdlc_input(sdb->sdlc_idb, pak);
    }
}


/*
 * ClsSdlcDiscInd
 *
 * Received DISC/RD from the SDLC link, or user
 * deconfigured the SDLC address.
 * Tell CLS (and DLU).
 *
 * If the cls state is such that we haven't sent a
 * Disc.Ind yet, send one (or negative ReqOpnStn.Cfm).
 */

static void ClsSdlcDiscInd (sdlc_data_block *sdb)
{
    switch (sdb->cls_state) {

    case CLS_STN_CLOSED:
	/*
	 * This is weird, but not fatal
	 *
	 * There's an sdb, otherwise we never get to this routine
	 * If there's a u_cep_id, then we should not be in Closed 
	 * State
	 */
	if (sdb->u_cep_id != (void*) CLS_NO_CEP) {
	    errmsg(&msgsym(ILLEGSTATE, SDLC),sdb->sdlc_idb->hw_namestring,
		   sdb->sdlc_address, sdlc_prstate((sdb), 0),
		   sdb->sdlc_state);
	
	    sdlcClsNotify(sdb, CLS_DISC_IND, CLS_BAD_ADDRESS);
	    sdb->cls_state = CLS_DISC_IND_SENT;
	}
	break;

    case CLS_DISC_IND_SENT:

	/*
	 * Already received DISC/RD from SDLC
	 * Awaiting DISC_RSP from CLS
	 *
	 * We'll ignore repeated DISC from SDLC, and accept DIS_RSP from DLU
	 * After that  DLU should Close the station
	 */
	break;

    case CLS_DISCCNF_PEND:
	sdlcClsNotify(sdb, CLS_DISC_CNF, CLS_OK);
	sdb->cls_state = CLS_STN_OPENED;
	break;

    case CLS_ROSCNF_PEND:
	SdlcReqOpenStnConf(sdb, sdb->u_cep_id, CLS_DISC_ERROR);
	sdb->cls_state = CLS_STN_CLOSED;
	break;

    case CLS_CLOSING:
	break;				/* Won't ever happen - unless someone
					 * re-enables interrupts in CLSCepCloseStnReq 
					 */

    case CLS_STN_OPENED:		/* ROS Req, Ros Cnf completed */
    case CLS_FULL_XID_PEND:
    case CLS_CONNECTED_IND_PEND:	/* Recvd SNRM, awaiting UA from CLS */
    case CLS_CONNECT_REQ_PEND:
    case CLS_CONNECT_RSP_PEND:		/* Sent SNRM, awaiting UA from SDLC */
    case CLS_IN_SESSION:
	sdlcClsNotify(sdb, CLS_DISC_IND, CLS_OK);
	sdb->cls_state = CLS_DISC_IND_SENT;
	if (cls_debug_sdlc)
	    buginf(" sending CLS_DISC_IND");
	break;
    }
}

/*
 * Debug routine for consistent formatting
 */
static void cls_sdlc_debug (idbtype *idb, sdlc_data_block *sdb, 
			   char *sdlc_debug_text) {

    if (cls_debug_sdlc) {
	if (sdb)
	    buginf("\n%s SDLC: %02x [%s,%s] %s", 
		   sdb->sdlc_idb->hw_namestring, sdb->sdlc_address, 
		   pr_cls_sdlc_state(sdb), sdlc_prstate((sdb),0), 
		   sdlc_debug_text);
	else
	    buginf("\n%s SDLC: %s", idb->namestring, sdlc_debug_text);
    }
}
