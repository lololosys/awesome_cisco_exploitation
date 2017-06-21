/* $Id: lnm_disp.c,v 3.3.26.5 1996/09/10 03:14:13 ioakley Exp $
 * $Source: /release/112/cvs/Xsys/ibm/lnm_disp.c,v $
 *------------------------------------------------------------------
 * lnm_disp.c -- CLSI Lan Manager Dispatcher
 *
 * August 1995, Ivan P. Oakley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lnm_disp.c,v $
 * Revision 3.3.26.5  1996/09/10  03:14:13  ioakley
 * CSCdi60079:  Correct clsi compliant LNM dlu clean-up problems
 *              that result in LNM clsi related error messages when
 *              "no source-bridge"/"source-bridge" commands are
 *              entered on token ring interfaces.
 * Branch: California_branch
 *
 * Revision 3.3.26.4  1996/08/26  15:08:18  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.3.26.3  1996/07/09  05:58:09  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.3.26.2  1996/05/17  11:20:38  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.14.4  1996/04/26  15:19:49  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.14.3  1996/04/26  07:54:15  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.14.2  1996/04/03  14:33:58  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.14.1  1996/03/29  19:36:55  ioakley
 * CSCdi52877:  VDLC merge into IbuMod_Calif - Resolve various LNM/VDLC
 *              inconsistencies & include LNM/VDLC disable port updates.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.26.1  1996/03/18  20:11:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  09:41:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/19  00:16:55  ioakley
 * CSCdi47146:  Place correct CLS primitive in lnmllcbuginf statement.
 *
 * Revision 3.2  1995/11/17  09:22:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/24  02:36:04  ioakley
 * Placeholder for new file.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "globs.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "ieee.h"
#include "address.h" /* Defines hwaddrtype for clsi_msg.h    */
#include "../if/tring.h"
#include "../if/network.h" /* access to net_bridge_enqueue() for TBRIDGE Tx..*/
#include "../ui/debug.h"
#include "../cls/clsi_msg.h"
#include "../cls/dlc_registry.h"
#include "../cls/clsi_queue.h"
#include "../dlsw/dlsw_peer_externs.h" /* peek into peer definitions... */
#include "../dlsw/dlsw_csm_externs.h"
#include "../ibm/lnm_dlu.h"
#include "../ibm/lanmgr.h"
#include "../ibm/ibmnm.h"
#include "../llc2/llc2.h"
#include "../srt/srb_core.h"
#include "../h/fastswitch_registry.regh"

/*
 * Lan_to_Lnm
 */
void Lan_to_Lnm(ctlbuf, databuf, flags)
struct str_lnm_buf *ctlbuf;
struct str_lnm_buf *databuf;
int *flags;
{
    struct str_lnm_rcvctl *rcvctlbuf;

    rcvctlbuf = (struct str_lnm_rcvctl *) ctlbuf -> buf;

    switch(rcvctlbuf -> prim_type) {
      case LNM_LAN_INIT:
	  ldisp_to_init(rcvctlbuf -> result);
	  break;
      case LNM_LAN_FSM:
	  ldisp_to_lfsm(rcvctlbuf -> prim_type, (TCLSIMsg *) databuf -> buf);
	  break;
      default:
	  lnmllcbuginf("LNM_DISP: Lan_to_Lnm input message not recognized.\n");
	  break;
    }
}

/*
 * lfsm_to_ldisp()
 */
void lfsm_to_ldisp(union L_primitives *lp)
{
    struct str_lnm_buf ctlbuf;
    struct str_lnm_buf databuf;
    int flags;
    struct str_lnm_sndctl sndctlbuf;
    TCLSIMsg *themsg;

    sndctlbuf.prim_type = LNM_FSM_LAN;
    sndctlbuf.msg_type = lp->type;
    ctlbuf.len = sizeof(struct str_lnm_sndctl);
    ctlbuf.buf = (char *) &sndctlbuf;

    switch(lp->type) {
      case L_ENABLE_REQ:
	  themsg = LCreateEnableReq((struct L_enable_req *) lp);
	  break;
      case L_DISABLE_REQ:
	  themsg = LCreateDisableReq((struct L_disable_req *) lp);
	  break;
      case L_ACTIVATESAP_REQ:
	  themsg = LCreateActSapReq((struct L_activatesap_req *) lp);
	  break;
      case L_DEACTIVATESAP_REQ:
	  themsg = LCreateDeActSapReq((struct L_deactivatesap_req *) lp);
	  break;
      case L_REQ_OPNSTN_REQ:
	  themsg = LCreateReqOpnStnReq((struct L_reqopenstn_req *) lp);
	  break;
      case L_CLOSESTN_REQ:
	  themsg = LCreateCloseStnReq((struct L_closestn_req *) lp);
	  break;
      case L_CONNECT_REQ:
	  themsg = LCreateConnectReq((struct L_connect_req *) lp);
	  break;
      case L_SIGNALSTN_REQ:
	  themsg = LCreateSignalStnReq((struct L_signalstn_req *) lp);
	  break;
      case L_DATA_REQ:
	  themsg = LCreateDataReq((struct L_data_req *) lp);
	  break;
      case L_UDATASTN_REQ:
	  themsg = LCreateUDataStnReq((struct L_udatastn_req *) lp);
	  break;
      case L_UDATA_REQ:
	  themsg = LCreateUDataReq((struct L_udata_req *) lp);
	  break;
      case L_DISCONNECT_REQ:
	  themsg = LCreateDiscReq((struct L_disconnect_req *) lp);
	  break;
      case L_DISCONNECT_RSP:
	  themsg = LCreateDiscRsp((struct L_disconnect_rsp *) lp);
	  break;
      default:
	  lnmllcbuginf("LNM_DISP: lfsm_to_ldisp - CLS primitive not supported/recognized.");
	  themsg = NULL;
	  break;
    }
    databuf.buf = (char *) themsg;
    databuf.len = 0;    /* not used, zeroed out for future compatibility */
    flags = 0;
    Lnm_to_Lan(&ctlbuf, &databuf, &flags);
}

/*
 *  init_to_ldisp()
 */
void init_to_ldisp(union L_primitives *lp)
{
    struct str_lnm_buf ctlbuf;
    struct str_lnm_buf databuf;
    int flags;

    struct str_lnm_sndctl sndctlbuf;    
 
    sndctlbuf.prim_type = LNM_INIT_LAN;
    sndctlbuf.msg_type = lp->type;
    ctlbuf.len = sizeof(struct str_lnm_sndctl);
    ctlbuf.buf = (char *) &sndctlbuf;
    databuf.len = 0;   /* not used, set to 0 for now, future compability */
    databuf.buf = (char *) lp;
    flags = 0;
    Lnm_to_Lan(&ctlbuf,&databuf,&flags);
}
	  
TCLSIMsg* LCreateEnableReq(struct L_enable_req *lp)
{
    EnableReq_t *enableReq;
    uint parmSize;
    uchar lnm_vdlc_vmacaddr[IEEEBYTES];

    switch (lp->port_type) {
      case CLS_TBRIDGE:
	  parmSize = sizeof(EnableTBRIDGEParms_t);
	  break;
      case CLS_LLC:    /* Code is broken, CLS_LLC != TKR port !!! */
	  parmSize = sizeof(EnableTRParms_t);
	  break;
      case CLS_VDLC:
	  parmSize = sizeof(EnableVDLCParms_t);
	  break;
      case CLS_RSRB:
	  parmSize = sizeof(EnableRSRBParms_t);
	  break;
      default:
	  lnmllcbuginf("\nLNM_DISP: CreateEnableReq, invalid port type (%d)",
		 lp->port_type);
	  return(NULL);
	  break;
    }
    enableReq = (EnableReq_t*) CLSIMsgCreateHeader(CLS_ENABLE | CLS_REQUEST,
					       offsetof(EnableReq_t, fParms) -
					       CLS_ENABLE_REQ_HDR_SZ +
					       parmSize,
					       CLS_LLC_LAYER,
					       CLS_ENTITY_MGR,
					       0);
    if (enableReq == NULL)
	return NULL;
    enableReq->fDLCType = lp->port_type;
    enableReq->fPortId.type = CLS_P_TYPE_SWIDB;
    enableReq->fPortId.fTypeInfo.swidb = lp->swidb;
    enableReq->fCorrelator = lp->corr;

    switch (lp->port_type) {
      case CLS_LLC:
      { 
	EnableTRParms_t* trParms = (EnableTRParms_t*) &(enableReq->fParms);
	trParms->fVersion = 1;
	   /* Use defaults for everything now */
	snpa_clear(&trParms->fLocalMac);
	trParms->fTrSpeed = 0;
	trParms->fMaxIFieldLength = 0;
	break;
      }
      case CLS_VDLC:
      {
	EnableVDLCParms_t* vdlcParms = (EnableVDLCParms_t *) &(enableReq->fParms);  
        ieee_copy(lp->hwidb->hardware, lnm_vdlc_vmacaddr);
	lnm_vdlc_vmacaddr[0] |= 0x40;
	vdlcParms->fVersion = CLSI_VERSION;
	vdlcParms->fRingGroup = srbV.srb_sb_get_targetring(lp->swidb);
        snpa_init(&vdlcParms->fVMacAddr, STATION_IEEE48, TARGET_UNICAST, 
            (uchar *) lnm_vdlc_vmacaddr);	
	vdlcParms->fVdlcOptionFlags = 0;
	vdlcParms->fMaxIFieldLength = 0;
	break;
      }	  
      case CLS_RSRB:
      {
        srb_triplet_t srb_triplet;
        static uchar lnm_rsrb_vmacaddr[IEEEBYTES] ={0x00,0x00,0x0,0x0,0x0,0x0};
        EnableRSRBParms_t* rsrbParms =
	    (EnableRSRBParms_t *) &(enableReq->fParms);
        rsrbParms->fVersion = CLSI_VERSION;
        snpa_init(&rsrbParms->fVMacAddr, STATION_IEEE48, TARGET_UNICAST, 
            (uchar *) lnm_rsrb_vmacaddr);
        rsrbParms->fMaxIFieldLength = 0;

        srbV.srb_sb_get_triplet(lp->swidb, &srb_triplet);
        rsrbParms->fLocalRing    = srb_triplet.localRing;
        rsrbParms->fBridgeNumber = srb_triplet.bridgeNum;
        rsrbParms->fTargetRing   = srb_triplet.remoteRing;
        rsrbParms->fRsrbOptionFlags = 0;
	break;
      }  
      default:
        lnmllcbuginf("\nLNM_DISP: CreateEnableReq for unknown port type %d",
		     lp->port_type);
	break;
    }
    return((TCLSIMsg*) enableReq);
}

TCLSIMsg* LCreateDisableReq(struct L_disable_req *lp)
{
    DisableReq_t *disableReq;
    
    disableReq = (DisableReq_t*) CLSIMsgCreateHeader(CLS_DISABLE | CLS_REQUEST,
						      0,
						      CLS_LLC_LAYER,
						      CLS_ENTITY_MGR,
						      0);
    if (disableReq == NULL)
	return NULL;
    disableReq->fPortId.type = CLS_P_TYPE_SWIDB;
    disableReq->fPortId.fTypeInfo.swidb = lp->swidb;
    disableReq->fDLCType = lp->port_type;
    disableReq->fCorrelator = 0x00;    /* 0xdead */
    /* Use defaults for everything, for now */
    return((TCLSIMsg*) disableReq);
}

TCLSIMsg* LCreateActSapReq(struct L_activatesap_req *lp)
{
    ActSapReq_t* actSapReq;
    ActSapLLCParms_t* trParms;
    ActSapVDLCParms_t* vdlcParms;
    
    actSapReq = (ActSapReq_t*) CLSIMsgCreateHeader(CLS_ACTIVATE_SAP |
						   CLS_REQUEST,
						   offsetof(ActSapReq_t,
						   fParms) -
						   CLS_ACT_SAP_REQ_HDR_SZ +
						   sizeof(ActSapLLCParms_t),
						   CLS_LLC_LAYER,
						   CLS_ENTITY_MGR,
						   0);
    if (actSapReq == NULL)
	return NULL;
    actSapReq->fCLSIHdr.fUSapId = lp->u_sap_id;
    actSapReq->fVersion = 1;
    actSapReq->fCorrelator = 0x00;      /*0xdead */
    actSapReq->fPortId.type = CLS_P_TYPE_SWIDB;
    actSapReq->fPortId.fTypeInfo.swidb = lp->swidb;
    actSapReq->fDLCType = lp->port_type;

    switch (lp->port_type) {
      case CLS_LLC:
      {
	  trParms = (ActSapLLCParms_t*) &(actSapReq->fParms);
	  trParms->fUserClass = 0x02;
	  trParms->fSapValue = lp->sap_value;  
	  trParms->fT1 = 0;
	  trParms->fT2 = 0;
	  trParms->fTi = 0;
	  trParms->fMaxOut = 0;
	  trParms->fMaxRetryCount = 0;		/* no limit */
	  trParms->fMaxIn = 0;
	  trParms->fMaxIField = 0;
	  break;
      }
      case CLS_VDLC:
      {
	  vdlcParms = (ActSapVDLCParms_t*) &(actSapReq->fParms);
	  vdlcParms->fUserClass = 0x02;
	  vdlcParms->fSapValue = lp->sap_value;
	  /*
	   * Note: vdlcParms->fSapFlags not specified!
	   *       In ../cls/cls_entitymanager.c activate
	   *       sap processing this value is hard coded
	   *       to SAP_TEST_AUTORESPONSE.
	   */
	  break;
      }
      default:
	lnmllcbuginf("\nLNM_DISP: CreateActSapReq invoked for unknown port type %d", lp->port_type);
	break;
    }
    return((TCLSIMsg*) actSapReq);
}

TCLSIMsg* LCreateDeActSapReq(struct L_deactivatesap_req *lp)
{
    DeactSapReq_t* deactSapReq;
    
    deactSapReq = (DeactSapReq_t*) CLSIMsgCreateHeader(CLS_DEACTIVATE_SAP |
						       CLS_REQUEST,
						       0,
						       CLS_LLC_LAYER,
						       CLS_P_SAP_ID,
						       lp->p_sap_id);
    if (deactSapReq == NULL)
	return NULL;
    deactSapReq->fCLSIHdr.fUSapId = lp->u_sap_id;
    deactSapReq->fVersion = 1;
    deactSapReq->fCorrelator = 0x00;      /*0xdead */
    return((TCLSIMsg*) deactSapReq);
}
					      
TCLSIMsg* LCreateReqOpnStnReq(struct L_reqopenstn_req *lp)
{
    ReqOpnStnReq_t* reqOpnStnReq;
    ReqOpnStnLLCParms_t* trParms;
    
    reqOpnStnReq =
	   (ReqOpnStnReq_t*) CLSIMsgCreateHeader(CLS_REQ_OPNSTN | CLS_REQUEST,
					 offsetof(ReqOpnStnReq_t, fParms) -
					 CLS_REQ_OPNSTN_REQ_HDR_SZ +
					 sizeof(ReqOpnStnLLCParms_t),
					 CLS_LLC_LAYER,
					 CLS_P_SAP_ID,
					 lp->p_sap_id);
    if (reqOpnStnReq == NULL)
	return NULL;
    reqOpnStnReq->fCLSIHdr.fUCepId = lp->uCepId;
    reqOpnStnReq->fVersion = 1;
    reqOpnStnReq->fCorrelator = 0xdead;
    reqOpnStnReq->fQueuingType = CLS_QUEUING_DEFAULT;
    trParms = (ReqOpnStnLLCParms_t*) &(reqOpnStnReq->fParms);
    /*
     * If this is an incoming call, these came from the idStn.ind
     */
    trParms->fDestAddr.type = STATION_IEEE48 ;
    trParms->fDestAddr.length = STATIONLEN_IEEE48;
    ieee_copy(lp->remoteAddr, trParms->fDestAddr.addr);
    trParms->fDestSap = lp->rSap;
    trParms->fSrcAddr.type = STATION_IEEE48 ;
    trParms->fSrcAddr.length = STATIONLEN_IEEE48;
    ieee_copy(lp->localAddr, trParms->fSrcAddr.addr); 
    trParms->fSrcSap = lp->lSap;
    trParms->fRifLength = lp->rifLength;
    if (lp->rifLength > 0)
	bcopy(lp->rif, trParms->fRif, lp->rifLength);
    trParms->fExplorerRetries = 3;
    trParms->fExplorerTimer = 1000;
    /*
     * These are other configuration parameters. Mostly use defaults.
     */
    trParms->fT1 = 0;
    trParms->fT2 = 0;
    trParms->fTi = 0;
    trParms->fMaxOut = 0;
    trParms->fMaxRetryCount = 0;
    trParms->fMaxIn = 0;
    /*
    trParms->fWindowOption = 0;
    */
    trParms->fMaxOutIncr = 0;
    trParms->fMaxIField = 0;
    trParms->fXIDRetries = 0;
    trParms->fXIDTimer = 0;
    trParms->fExplorerRetries = 0;
    trParms->fExplorerTimer = 0;
    trParms->fExplorerOption = lp->options;
    trParms->fDlci = 0 ; 
    return((TCLSIMsg*) reqOpnStnReq);
}
    
TCLSIMsg* LCreateConnectReq(struct L_connect_req *lp)
{
    ConnectReq_t* connectReq;
    size_t parmSize = 0;    /* For now.  Do parameters later. */

    connectReq = (ConnectReq_t*) CLSIMsgCreateHeader(CLS_CONNECT |
						     CLS_REQUEST,
						     offsetof(ConnectReq_t,
						     fParms) -
						     CLS_CONNECT_REQ_HDR_SZ +
						     parmSize,
						     CLS_LLC_LAYER,
						     CLS_P_CEP_ID,
						     lp->pCepId);
    if (connectReq == NULL)
	return NULL;
    /* we might want to lock the Rif here, this will send out a SABME */
    connectReq->fCLSIHdr.fModifyFlags = 0;
    connectReq->fVersion = CLSI_VERSION;
    connectReq->fCorrelator = 0xdead;
    return((TCLSIMsg*) connectReq);
}

TCLSIMsg* LCreateSignalStnReq(struct L_signalstn_req *lp)
{
 SignalStnReq_t* sigStnReq;
    
 sigStnReq = (SignalStnReq_t*)
 CLSIMsgCreateHeader(CLS_SIGNAL_STN | CLS_REQUEST,
		    sizeof(SignalStnReq_t) -
		    CLS_SIGNAL_STN_REQ_HDR_SZ,
		    CLS_LLC_LAYER,
		    CLS_P_CEP_ID,
		    lp->pCepId);
 if (sigStnReq == NULL)
  return NULL;
    
 sigStnReq->fCLSIHdr.fActionCode = 0; /* start data (exit local */
				 /* busy state) */
 sigStnReq->fVersion = CLSI_VERSION;
 sigStnReq->fCorrelator = 0xdead;

 return((TCLSIMsg*) sigStnReq);
}


TCLSIMsg* LCreateDataReq(struct L_data_req *lp)
{
    DataReq_t* dataReq;
    hword reservedSize;
    
    dataReq = (DataReq_t*) CLSIMsgCreateFromPak(lp->pak,
						CLS_DATA | CLS_REQUEST,
						CLS_LLC_LAYER,
						CLS_P_CEP_ID,
						lp->pCepId,
						0,
						&reservedSize);
    if (dataReq == NULL)
	return NULL;
    dataReq->fVersion = CLSI_VERSION;
    dataReq->fCorrelator = 0xdead;
    dataReq->fReserveLength = 0;
    return((TCLSIMsg*) dataReq);
}

TCLSIMsg* LCreateDiscReq(struct L_disconnect_req *lp)
{
    DisconnectReq_t* disconnectReq;
    size_t parmSize = 0;    /* For now. Do parameters later. */

    disconnectReq = (DisconnectReq_t*) CLSIMsgCreateHeader(CLS_DISCONNECT |
						   CLS_REQUEST,
						   offsetof(DisconnectReq_t,
						   fCorrelator) -
						   CLS_DISCONNECT_REQ_HDR_SZ +
						   parmSize,
						   CLS_LLC_LAYER,
						   CLS_P_CEP_ID,
						   lp->pCepId);
    if (disconnectReq == NULL)
	return NULL;
    disconnectReq->fVersion = CLSI_VERSION;
    disconnectReq->fCorrelator = 0xdead;
    return((TCLSIMsg*) disconnectReq);
}

TCLSIMsg* LCreateDiscRsp(struct L_disconnect_rsp *lp)
{
    DisconnectRsp_t* disconnectRsp;
    size_t parmSize = 0;    /* For now. Do parameters later. */

    disconnectRsp = (DisconnectRsp_t*) CLSIMsgCreateHeader(CLS_DISCONNECT |
						   CLS_RESPONSE,
						   offsetof(DisconnectReq_t,
						   fCorrelator) -
						   CLS_DISCONNECT_RSP_HDR_SZ +
						   parmSize,
						   CLS_LLC_LAYER,
						   CLS_P_CEP_ID,
						   lp->pCepId);
    /*
     * Question in previous code taken from dlsw why do we use DisconnectReq_t
     * in the offset (would have thought it would be DisconnectRsp_t -
     * apparently this does not make any difference!
     */
    if (disconnectRsp == NULL)
	return NULL;
    disconnectRsp->fVersion = CLSI_VERSION;
    disconnectRsp->fCorrelator = 0xdead;
    return((TCLSIMsg*) disconnectRsp);
}

TCLSIMsg* LCreateCloseStnReq(struct L_closestn_req *lp)
{
    CloseStnReq_t* closeStnReq;
    
    closeStnReq = (CloseStnReq_t*) CLSIMsgCreateHeader(CLS_CLOSE_STN |
					           CLS_REQUEST,
						   offsetof(CloseStnReq_t,
						   fCorrelator) -
						   CLS_CLOSE_STN_REQ_HDR_SZ,
						   CLS_LLC_LAYER,
						   CLS_P_CEP_ID,
						   lp->pCepId);
    if (closeStnReq == NULL)
	return NULL;
    closeStnReq->fCLSIHdr.fUCepId = 0x00abcdef;
    closeStnReq->fCLSIHdr.fActionCode = CLS_ACTION_FORCED;
    
    closeStnReq->fVersion = CLSI_VERSION;
    closeStnReq->fCorrelator = 0xdead;

    return((TCLSIMsg*) closeStnReq);
}

/*
 * New Messages
 */

/*
 * udata request.
 */
TCLSIMsg* LCreateUDataReq(struct L_udata_req *lp)
{
    UDataReq_t* udataReq;

    udataReq = (UDataReq_t*) CLSIMsgCreateHeader(CLS_UDATA | CLS_REQUEST,
						 offsetof(UDataReq_t, fUData) -
						 CLS_UDATA_REQ_HDR_SZ +
						 lp -> udataLength,
						 CLS_LLC_LAYER,
						 CLS_P_CEP_ID,
						 lp->pCepId);
    if (udataReq == NULL)
	return NULL;
    udataReq->fVersion = CLSI_VERSION;
    udataReq->fCorrelator = 0xdead;
    udataReq->fReserveLength = 0;
    bcopy(lp->theUData, &udataReq->fUData[0], lp->udataLength);
    return((TCLSIMsg*) udataReq);
}

TCLSIMsg* LCreateUDataStnReq(struct L_udatastn_req *lp)
{
    UDataStnReq_t* udataStnReq;
    
    udataStnReq = (UDataStnReq_t*) CLSIMsgCreateHeader(CLS_UDATA_STN |
						   CLS_REQUEST,
						   offsetof(UDataStnReq_t,
						   fUData) -
						   CLS_UDATA_STN_REQ_HDR_SZ +
						   lp->udataLength,
						   CLS_LLC_LAYER,
						   CLS_P_SAP_ID,
						   lp->p_sap_id);
    if (udataStnReq == NULL)
	return NULL;
    udataStnReq->fVersion = CLSI_VERSION;
    udataStnReq->fCorrelator = 0xdead;
    udataStnReq->fReserveLength = 0;
    udataStnReq->fRSap = lp->rSap;
    udataStnReq->fLSap = lp->lSap;
    udataStnReq->fRifLength = lp->rifLength;
    bcopy(lp->rif, udataStnReq->fRif, udataStnReq->fRifLength);
    ieee_copy(lp->localAddr, udataStnReq->fLMac); 
    ieee_copy(lp->remoteAddr, udataStnReq->fRMac); 
    bcopy(lp->theUData, &udataStnReq->fUData[0], lp->udataLength);
    return((TCLSIMsg*) udataStnReq);
}
